/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#include "platform_autoconf.h"
#include "hal_api.h"
#include "diag.h"
#include "section_config.h"

INFRA_RAM_BSS_SECTION
static u32 z1, z2, z3, z4, first;

INFRA_ROM_TEXT_SECTION
_LONG_CALL_ u32
Rand (
    VOID
)
{
    u32 b;

    if (!first) {
        z1 = 12345, z2 = 12345, z3 = 12345, z4 = 12345;
        first = 1;
    }

    b  = ((z1 << 6) ^ z1) >> 13;
    z1 = ((z1 & 4294967294U) << 18) ^ b;
    b  = ((z2 << 2) ^ z2) >> 27;
    z2 = ((z2 & 4294967288U) << 2) ^ b;
    b  = ((z3 << 13) ^ z3) >> 21;
    z3 = ((z3 & 4294967280U) << 7) ^ b;
    b  = ((z4 << 3) ^ z4) >> 12;
    z4 = ((z4 & 4294967168U) << 13) ^ b;
    return (z1 ^ z2 ^ z3 ^ z4);
}

