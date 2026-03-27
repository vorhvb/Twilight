// ──────────────────────────────────────────────────────────────────────
//
// Name					Read.c
// Project				Twilight
// Author				Elisey Konstantinov
//
// License				GPL v3
//
// ──────────────────────────────────────────────────────────────────────

#include "Read.h"

// ──────────────────────────────────────────────────────────────────────

struct ParameterBlock
{
	uint8_t instructions[3];
	uint8_t identifier[8];

	uint16_t heads;

	uint16_t sectors;
	uint32_t large_sectors;
	uint32_t hidden_sectors;

	uint16_t bytes_per_sector;
	uint16_t sectors_per_track;

	uint16_t reserved_sectors;
	uint8_t tables;
	uint16_t root_entries;
	uint8_t sectors_per_cluster;
	uint16_t sectors_per_table;
	
	uint8_t media_descriptor_type;
};
struct ExtendedParameterBlock
{
	uint8_t label[11];
	uint8_t drive_number;

	uint8_t reserved;

	uint32_t serial_number;
	uint64_t system_id;
	uint8_t signature;

	uint8_t boot_code[450];
};

struct Entry
{
	uint8_t filename[11];
	uint32_t file_size;
	uint8_t attributes;

	uint8_t reserved;

	uint16_t creation_date;
	uint16_t creation_time;
	uint8_t precise_creation_time;

	uint16_t last_acessed_date;

	uint16_t last_modification_time;
	uint16_t last_modification_date;

	uint16_t low_of_first_cluster;
	uint16_t high_of_first_cluster;
};

uint8_t *boot_sector_pointer;

// ──────────────────────────────────────────────────────────────────────

ParameterBlock read_parameter_block()
{
	boot_sector_pointer = (uint8_t*)0x7C00;
	ParameterBlock result;

	memcpy (&result.instructions, boot_sector_pointer, 3);
	memcpy (&result.identifier, boot_sector_pointer + 3, 8);

	memcpy (&result.heads, boot_sector_pointer + 26, 2);

	memcpy (&result.sectors, boot_sector_pointer + 19, 2);
	memcpy (&result.large_sectors, boot_sector_pointer + 32, 4);
	memcpy (&result.hidden_sectors, boot_sector_pointer + 28, 4);

	memcpy (&result.bytes_per_sector, boot_sector_pointer + 11, 2);
	memcpy (&result.sectors_per_track, boot_sector_pointer + 24, 2);

	memcpy (&result.reserved_sectors, boot_sector_pointer + 14, 2);
	memcpy (&result.tables, boot_sector_pointer + 16, 1);
	memcpy (&result.root_entries, boot_sector_pointer + 17, 2);
	memcpy (&result.sectors_per_cluster, boot_sector_pointer + 13, 1);
	memcpy (&result.sectors_per_table, boot_sector_pointer + 22, 2);

	memcpy (&result.media_descriptor_type, boot_sector_pointer + 21, 1);
	
	return result;
}
ExtendedParameterBlock read_extended_parameter_block()
{
	boot_sector_pointer = (uint8_t*)0x7C24;
	ExtendedParameterBlock result;

	memcpy (&result.label, boot_sector_pointer + 7, 11);
	memcpy (&result.drive_number, boot_sector_pointer, 1);

	memcpy (&result.reserved, boot_sector_pointer + 1, 1);

	memcpy (&result.serial_number, boot_sector_pointer + 3, 4);
	memcpy (&result.system_id, boot_sector_pointer + 18, 8);
	memcpy (&result.signature, boot_sector_pointer + 2, 1);

	memcpy (&result.boot_code, boot_sector_pointer + 26, 450);
	
	return result;
}

Entry* read_root()
{
	;
}

// ──────────────────────────────────────────────────────────────────────
