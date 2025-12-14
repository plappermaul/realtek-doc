/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2014 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "basic_types.h"
#include "diag.h"
#include "section_config.h"
#include "rtl_utility.h"
#include "common.h"
#include "rand.h"

#include "dw_ne_base.h"
#include "peripheral.h"
#include "dw_ne_base_test.h"

static void
Ne_ISR_Verification(bool enGIMR)
{
        Hal_Ne_ISR_Verification(enGIMR);

        return;
}


void
NeTestApp(bool enGIMR)
{
#if ( defined(CONFIG_RTL8197F) || defined(CONFIG_RTL8197G) )
        Ne_ISR_Verification(enGIMR);
#endif /* CONFIG_RTL8197F || CONFIG_RTL8197G *//

        return;
}

 
