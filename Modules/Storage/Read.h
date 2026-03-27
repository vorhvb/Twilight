// ──────────────────────────────────────────────────────────────────────
//
// Name					Read.h
// Project				Twilight
// Author				Elisey Konstantinov
//
// License				GPL v3
//
// ──────────────────────────────────────────────────────────────────────

#ifndef READ_H
#define READ_H

// ──────────────────────────────────────────────────────────────────────

#include "../Core.h"

// ──────────────────────────────────────────────────────────────────────

typedef struct ParameterBlock ParameterBlock;
typedef struct ExtendedParameterBlock ExtendedParameterBlock;

typedef struct Entry Entry;

extern uint8_t* boot_sector_pointer;

// ──────────────────────────────────────────────────────────────────────

ParameterBlock read_parameter_block();
ExtendedParameterBlock read_extended_parameter_block();

Entry* read_root();

// ──────────────────────────────────────────────────────────────────────

#endif

// ──────────────────────────────────────────────────────────────────────
