/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#if 0 
#include "platform_autoconf.h"
#include "basic_types.h"
#include "diag.h"

#define SECTION_ROM_BANK0_END            SECTION(".test_bank0_end")
#define SECTION_ROM_BANK1_START          SECTION(".test_bank1_start")
#define SECTION_ROM_BANK1_END            SECTION(".test_bank1_end")
#define SECTION_ROM_BANK2_START          SECTION(".test_bank2_start")
#define SECTION_ROM_BANK2_END            SECTION(".test_bank2_end")

SECTION_ROM_BANK0_END
VOID 
ROM_BANK0_END_FUN(VOID)
{
    DBG_8195A_HAL("ROM_BANK0_END_FUN Enter\n");
    DBG_8195A_HAL("ROM_BANK0_END_FUN Leave\n");
}

SECTION_ROM_BANK1_START
VOID 
ROM_BANK1_START_FUN(VOID)
{
    DBG_8195A_HAL("ROM_BANK1_START_FUN Enter\n");
    DBG_8195A_HAL("ROM_BANK1_START_FUN Leave\n");    
}


SECTION_ROM_BANK1_END
VOID 
ROM_BANK1_END_FUN(VOID)
{
    DBG_8195A_HAL("ROM_BANK1_END_FUN Enter\n");
    DBG_8195A_HAL("ROM_BANK1_END_FUN Leave\n");        
}



SECTION_ROM_BANK2_START
VOID 
ROM_BANK2_START_FUN(VOID)
{
    DBG_8195A_HAL("ROM_BANK2_START_FUN Enter\n");
    DBG_8195A_HAL("ROM_BANK2_START_FUN Leave\n");        
}


SECTION_ROM_BANK2_END
VOID 
ROM_BANK2_END_FUN(VOID)
{
    DBG_8195A_HAL("ROM_BANK2_END_FUN Enter\n");
    DBG_8195A_HAL("ROM_BANK2_END_FUN Leave\n");            
}



#endif
