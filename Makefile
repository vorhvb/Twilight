# ──────────────────────────────────────────────────────────────────────
#
# Name					Makefile
# Project				Twilight
# Author				Elisey Konstantinov
#
# License				GPL v3
#
# ──────────────────────────────────────────────────────────────────────

all: compile link pack
compile:
	#
link:
	#
pack:
	#

prepare:
	#

# ──────────────────────────────────────────────────────────────────────

clean:
	#
scrub:
	#
run:
	#

# ──────────────────────────────────────────────────────────────────────

dump:
	tree || find .

	cat *.asm
	cat Kernel.c

	cat Modules/*

	cat Makefile

edit:
	#

# ──────────────────────────────────────────────────────────────────────
