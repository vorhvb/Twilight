# ──────────────────────────────────────────────────────────────────────
#
# Name					Menu.asm
# Project				Twilight
# Author				Elisey Konstantinov
#
# License				GPL v3
#
# ──────────────────────────────────────────────────────────────────────

# XXX: Hic sunt dracones
# Warning, all code below DOES NOT WORK (partially or completely)
# It could explode in your hands AT ANY MINUTE
#
# TODO: Rewrite (after Menu.c is finished)

.code16
.globl menu

.data
	bytes_per_sector: .space 2
	sectors_per_cluster: .space 1

	reserved_sectors: .space 2
	tables: .space 1
	root_entries: .space 2

	total_sectors: .space 2
	sectors_per_table: .space 2

	sectors_per_track: .space 2
	heads: .space 2

	root_size: .space 2
	root_size_sectors: .space 2
	root_start: .space 2

	folder_name: .space 11
	folder_first_cluster: .space 2

	boot_string: .ascii "BOOT       "

# ──────────────────────────────────────────────────────────────────────

menu:
	call read

	movb $0x0E, %ah
	movb tables, %al
	int $0x10

	cli
	hlt
metadata:
	movw $(0x7C00 >> 4), %ax
	movw %ax, %ds

	movw 11, %dx
	movw %dx, bytes_per_sector
	movb 13, %dl
	movb %dl, sectors_per_cluster

	movw 14, %dx
	movw %dx, reserved_sectors
	movb 16, %dl
	movb %dl, tables
	movw 17, %dx
	movw %dx, root_entries

	movw 19, %dx
	movw %dx, total_sectors
	movw 22, %dx
	movw %dx, sectors_per_table

	movw 24, %dx
	movw %dx, sectors_per_track
	movw 26, %dx
	movw %dx, heads

	ret
calculate_root_start:
	movw tables, %ax
	movw sectors_per_table, %bx
	mulw %bx ; jo overflow

	addw reserved_sectors, %ax

	movw %ax, root_start

	ret
calculate_root_size:
	movw root_entries, %ax
	movw $32, %bx
	mulw %bx ; jo overflow

	addw bytes_per_sector, %ax
	decw %ax

	movw %ax, root_size

	ret
calculate_root_size_sectors:
	movw root_entries, %ax

	xorw %dx, %dx
	divw bytes_per_sector

	test %dx, %dx
	jz done
	incw %ax

	done: movw %ax, root_size_sectors

	ret
load_root:
	movw $0x0240, %ax
	movw $0x2000, %bx
	movb root_start, %cl
	call convert

	.loop:
		#

	ret
find_folder:
	call load_root

	movw $(0x2000 >> 4), %bx
	movw %bx, %ds

	xor %si, %si
	.read_block:
		movb 11(%si), %dl

		cmp $0x10, %dl
		jne .next

		xor %di, %di
		.read_name:
			movb (%si,%di), %dl
			movb %dl, folder_name(%di)
			inc %di

			cmp $11, %di
			jl .read_name

		push %si
		call test_folder
		pop %si

		movw 26(%si), %dl
		movw %dl, folder_first_cluster

		cmp $1, %ax
		jne .next

		.next:
			addw $32, %si
			cmp %si,
			jmp .read_block

	ret
test_folder:
	xor %si, %si
	.cycle:
		movb folder_name(%si), %dl
		cmp %dl, boot_string(%si)
		jne .not_equal

		inc %si

		cmp $11, %si
		jl .cycle

	movw $1, %ax
	ret

	.not_equal:
		movw $0, %ax
		ret


error:
	movb $0x0E, %ah
	movb $'E', %al
	int $0x10

	cli
	hlt
overflow:
	movb $0x0E, %ah
	movb $'O', %al
	int $0x10

	cli
	hlt

convert:
	xor %dx, %dx
	divw sectors_per_track      # AX = LBA/SPT, DX = LBA%SPT
	inc %dl                     # сектор = остаток + 1
	mov %dl, %cl                # CL = сектор (временно)

	xor %dx, %dx
	divw heads                  # AX = cylinder, DX = head

	mov %dl, %dh                # DH = head
	mov %al, %ch                # CH = low cylinder

	shl $6, %ah                 # high cylinder в биты 6-7
	or %ah, %cl                 # CL = (high << 6) | sector

	ret

# ──────────────────────────────────────────────────────────────────────

.org 4096

# ──────────────────────────────────────────────────────────────────────
