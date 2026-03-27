// ──────────────────────────────────────────────────────────────────────
//
// Name					Core.c
// Project				Twilight
// Author				Elisey Konstantinov
//
// License				GPL v3
//
// ──────────────────────────────────────────────────────────────────────

#include "Core.h"

// ──────────────────────────────────────────────────────────────────────

void *memcpy(void *dst, const void *src, size_t n)
{
    uint8_t *d = dst;
    const uint8_t *s = src;

    for (size_t i = 0; i < n; i++)
        d[i] = s[i];

    return dst;
}

// ──────────────────────────────────────────────────────────────────────
