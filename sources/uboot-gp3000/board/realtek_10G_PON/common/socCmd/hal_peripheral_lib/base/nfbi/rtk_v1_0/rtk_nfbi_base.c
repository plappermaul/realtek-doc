/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#include "platform_autoconf.h" 
#include "section_config.h"
#include "hal_platform.h"
#include "rtk_nfbi_base.h"
#include "diag.h"


//=================   Function Declare ===========================

extern BOOL
HalNFBIIrqRegisterCommon(
    IN  VOID    *Data
);


VOID
HalNFBIIrqClearCommon(
)
{
    HAL_NFBI_WRITE32(REG_NFBI_ISR, HAL_NFBI_READ32(REG_NFBI_ISR));
}
BOOL
HalNFBIInitCommon(
    IN  VOID    *Data
)
{
    HAL_NFBI_WRITE32(REG_NFBI_IMR, HAL_NFBI_READ32(REG_NFBI_IMR) | 0x1806); //temp fix value
    return _TRUE;
}

