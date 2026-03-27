# ──────────────────────────────────────────────────────────────────────
#
# Name					Bootloader.asm
# Project				Twilight
# Author				Elisey Konstantinov
#
# License				GPL v3
#
# ──────────────────────────────────────────────────────────────────────

.code16
.globl _start

# ──────────────────────────────────────────────────────────────────────

# TODO: add proper LBA-to-CHS conversion!

_start:
	movw $0x0003, %ax
	int $0x10

	movw $(0x7E00 >> 4), %ax
	movw %ax, %es

	movw $0x0208, %ax
	movw $0, %bx
	movw $0x0002, %cx
	int $0x13

	jc error

	ljmp $(0x7E00 >> 4), $0

error:
	movb $0x0E, %ah
	movb $'E', %al
	int $0x10

	cli
	hlt

# ──────────────────────────────────────────────────────────────────────

.org 448
.word 0xAA55

# ──────────────────────────────────────────────────────────────────────
