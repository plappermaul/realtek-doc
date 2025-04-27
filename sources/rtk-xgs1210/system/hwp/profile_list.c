/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 96290 $
 * $Date: 2019-04-12 17:43:06 +0800 (Fri, 12 Apr 2019) $
 *
 * Purpose : Model list.
 *           This file shall be custom defined.
 * Feature :
 *
 */

/*
 * Include Files
 */
#include <common/rt_autoconf.h>
#include <common/rt_error.h>
#include <osal/lib.h>
#include <osal/print.h>
#include <hwp/hw_profile.h>
#include <hal/chipdef/chipdef.h>
#include <drv/gpio/generalCtrl_gpio.h>
#include <common/debug/rt_log.h>
#include <common/util/rt_util.h>

/* Include hardware profile files of supported boards */

#if defined(CONFIG_SDK_RTL9300)
    #include <hwp/hw_profiles/xgs1210_12.c>
    #include <hwp/hw_profiles/xgs1250_12.c>
#endif

/*
 * Data Declaration
 */

/* list of supported hardware profiles */
const hwp_hwProfile_t *hwp_hwProfile_list[] =
{
#if defined(CONFIG_SDK_RTL9300)
    &xgs1210_12,
    &xgs1250_12,
#endif
    NULL,
}; /* end hwp_hwProfile_list */


