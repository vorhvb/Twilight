// ──────────────────────────────────────────────────────────────────────
//
// Name					Kernel.c
// Project				Twilight
// Author				Elisey Konstantinov
//
// License				GPL v3
//
// ──────────────────────────────────────────────────────────────────────

// XXX: Menu.asm’s C realization
// TODO: Rewrite on ASM
//
// FIXME: fix type in cyl~l~inder

#include <stdint.h>
#include <stdbool.h>

struct BPB
{
	uint16_t bytes_per_sector;
	uint8_t sectors_per_cluster;

	uint16_t reserved_sectors;
	uint8_t tables;
	uint16_t root_entries;

	uint16_t total_sectors;
	uint16_t sectors_per_table;

	uint16_t sectors_per_track;
	uint16_t heads;
};

typedef struct Chs_arr
{
	int cyllinder;
	int head;
	int sector;
} chs_arr;

typedef struct Registers
{
	uint8_t ah;
	uint8_t al;
	uint8_t bh;
	uint8_t bl;
	uint8_t ch;
	uint8_t cl;
	uint8_t dh;
	uint8_t dl;
} registers;

typedef struct __attribute__((packed)) FAT_entry
{
	uint8_t filename[11]; // 8.3
	uint8_t attributes; // READ_ONLY=0x01 HIDDEN=0x02 SYSTEM=0x04 VOLUME_ID=0x08 DIRECTORY=0x10 ARCHIVE=0x20 LFN=READ_ONLY|HIDDEN|SYSTEM|VOLUME_ID
	uint8_t reserved_by_NT;
	uint8_t creation_time_seconds; // hundreds of a second
	uint16_t creation_time; // 5 bits hour, 6 bits minutes, 5 bits seconds
	uint16_t creation_date; // 7 bits year, 4 bits month, 5 bits day
	uint16_t last_acessed_date; // same format
	uint16_t high_bits_of_first_cluster;
	uint16_t last_modification_time; // same format
	uint16_t last_modification_date; // same format
	uint16_t low_bits_of_first_cluster;
	uint32_t file_size; // in bytes
} fat_entry;

uint16_t first_cluster_of_boot_folder;
uint8_t* BPB_ptr_byte;
uint16_t* BPB_ptr_word;
uint8_t* root_table_ptr;
uint8_t* boot_folder_ptr;

// XXX: Dunno why but if you’ll declare ptr in global scope,
// and initialize it also in global scope or main it’ll FAIL!
void initialize_ptrs();
chs_arr lba_to_chs(int lba_sector_no, int sectors_per_track, int heads);
bool strcmp(char* string1, char* string2);

void menu()
{
	struct BPB bpb = {
		BPB_ptr_word[11],
		BPB_ptr_byte[13],
		BPB_ptr_word[14],
		BPB_ptr_byte[16],
		BPB_ptr_word[17],
		BPB_ptr_word[19],
		BPB_ptr_word[22],
		BPB_ptr_word[24],
		BPB_ptr_word[26],
	};

	int root_start = bpb.reserved_sectors + (bpb.tables * bpb.sectors_per_table);
	int root_size = bpb.root_entries * 32;
	initialize_ptrs(root_size);
	int root_size_sectors = (root_size + bpb.bytes_per_sector - 1) / bpb.bytes_per_sector;
	chs_arr chs = lba_to_chs (root_start, bpb.sectors_per_track, bpb.heads);
	for (int i=0; i<root_size_sectors; i+=64)
	{
		__asm__ volatile
		(
			"int $0x13\n"
			:
			: "a"(0x0240), "b"(0x2000+(i/bpb.bytes_per_sector)), "c"((chs.cyllinder << 8) | chs.sector), "d"((chs.head << 8) | 0)
			: "memory"
		);
	}
	for (int i=0; i!=root_size; i+=32)
	{
		fat_entry* entry = (fat_entry*)(root_table_ptr + i);
		if (entry->attributes != 0x10) continue;
		if (!strcmp(entry->filename, "BOOT       ")) continue;
		first_cluster_of_boot_folder = entry->low_bits_of_first_cluster;
		break;
	}
	int data_start = bpb.reserved_sectors + (bpb.tables*bpb.sectors_per_table) + root_size_sectors;
	int boot_first_sector = data_start + ((first_cluster_of_boot_folder - 2) *bpb.sectors_per_cluster);
	
	// FIXME: ЗДЕСЬ ВОЗЛЕЖИТ КОД ОТ ИИ. ЕГО НАДО ОТРЕФАКТОРИТЬ (🙄).
	// 1. У нас есть first_cluster_of_boot_folder (например, 5)
	int current_cluster = first_cluster_of_boot_folder;
	uint8_t* buffer = (uint8_t*)0x5000;  // куда читать папку
	uint8_t* ptr = buffer;

	while (current_cluster < 0xFFF8) {
		// Вычисляем LBA для этого кластера
		int lba = data_start + ((current_cluster - 2) * bpb.sectors_per_cluster);
		
		// Читаем кластер (сектор)
		chs_arr chs = lba_to_chs(lba, bpb.sectors_per_track, bpb.heads);
		__asm__ volatile (
			"int $0x13\n"
			: 
			: "a"(0x0201), "b"(ptr),
			"c"((chs.cyllinder << 8) | chs.sector),
			"d"((chs.head << 8) | 0)
			: "memory"
		);
		
		ptr += 512;  // следующий сектор в буфере
		
		// Получаем следующий кластер из FAT
		current_cluster = get_next_cluster(current_cluster);
	}

	// Теперь в buffer лежат записи файлов папки BOOT
	fat_entry* entry = (fat_entry*)buffer;

	// Ищем KERNEL.BIN
	for (int i = 0; i < (ptr - buffer) / 32; i++) {
		if (entry[i].filename[0] == 0x00) break;
		if (entry[i].filename[0] == 0xE5) continue;
		
		// Проверяем имя "KERNEL  BIN"
		if (memcmp_83(entry[i].filename, "KERNEL  BIN") == 0) {
			// Нашли! Начинаем загружать ядро
			load_kernel(entry[i].low_bits_of_first_cluster);
			break;
		}
	}
	// .END
}

void initialize_ptrs(int root_size)
{
	BPB_ptr_byte = (uint8_t*)0x7C00;
	BPB_ptr_word = (uint16_t*)0x7C00;
	root_table_ptr = (uint8_t*)0x2000;
	uint8_t boot_folder_ptr_addr = 0x2000+root_size;
	if (boot_folder_ptr_addr > 0x7C00) __asm__ volatile ("ud2\n"); // XXX:
	boot_folder_ptr = (uint8_t*)boot_folder_ptr_addr;
}

chs_arr lba_to_chs(int lba_sector_no, int sectors_per_track, int heads)
{
	int sector = (lba_sector_no % sectors_per_track) + 1;
	int head = (lba_sector_no / sectors_per_track) % heads;
	int cyllinder = (lba_sector_no / sectors_per_track) / heads;
	chs_arr result = {cyllinder, head, sector};
	return result;
}

bool strcmp_83(char* string1, char* string2)
{
	int i = 0;
	while (i != 11 && i != 11) {
		if (string1[i] != string2[i]) {
			return false;  // найдено различие
		}
		i++;
	}
	// если обе строки закончились одновременно — равны
	return string1[i] == string2[i];
}

// ──────────────────────────────────────────────────────────────────────
