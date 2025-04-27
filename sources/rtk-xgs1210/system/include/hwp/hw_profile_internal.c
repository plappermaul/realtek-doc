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
 * Purpose : Internal model list.
 *
 * Feature :
 *
 */

/*
 * Data Type Definition
 */

typedef enum hwp_id_internal_e
{
    HWP_RTL8391M_RTL9310_FPGA_L3               = 8390501,
    HWP_RTL8391M_RTL9310_FPGA,
    HWP_RTL8393M_TWO8382L_SIMU,
    HWP_RTL8393M_RTL9300_24FE_4XGE_FPGA,
    HWP_RTL8393M_RTL9300_8XGE_FPGA,
    HWP_RTL8393M_DEMO_SKIP_PHY1,
    HWP_RTL9300_24GE_4XGE_I2C_FPGA             = 9300501,
    HWP_RTL9300_CASCADE_24GE_4XGE_I2C_FPGA,
    HWP_RTL9300_24GE_4XGE_FPGA,
    HWP_RTL9300_8XGE_FPGA,
    HWP_RTL9303_8XGE_QA1,
    HWP_RTL9310_FPGA_L3                        = 9310501,
    HWP_RTL9310_FPGA,
    HWP_RTL9310_48GE_4XGE_FPGA,
    HWP_RTL9311_FT,
}hwp_id_internal_t;


/*
 * Include Files
 */

#include <common/rt_autoconf.h>

#if defined(CONFIG_SDK_RTL9310)
    #include <hwp/hw_profiles/internel_profiles/rtl9310_48ge_4xge_fpga.c>
    #include <hwp/hw_profiles/internel_profiles/rtl9310_fpga.c>
    #include <hwp/hw_profiles/internel_profiles/rtl9310_fpga_l3.c>
    #include <hwp/hw_profiles/internel_profiles/rtl9311_ft.c>
#endif/* CONFIG_SDK_RTL9310 */

#if defined(CONFIG_SDK_RTL9300)
    #include <hwp/hw_profiles/internel_profiles/rtl9300_24ge_4xge_fpga.c>
    #include <hwp/hw_profiles/internel_profiles/rtl9300_8xge_fpga.c>
    #include <hwp/hw_profiles/internel_profiles/rtl9300_24ge_4xge_i2c_fpga.c>
    #include <hwp/hw_profiles/internel_profiles/rtl9300_cascade_24ge_4xge_i2c_fpga.c>
    #include <hwp/hw_profiles/internel_profiles/rtl9303_8xge_qa1.c>
#endif/* CONFIG_SDK_RTL9300 */

#if defined(CONFIG_SDK_RTL8390) && defined(CONFIG_SDK_RTL9300)
    #include <hwp/hw_profiles/internel_profiles/rtl8393m_rtl9300_24fe_4xge_fpga.c>
    #include <hwp/hw_profiles/internel_profiles/rtl8393m_rtl9300_8xge_fpga.c>
#endif/* CONFIG_SDK_RTL8390 && CONFIG_SDK_RTL9300 */

#if (defined(__BOOTLOADER__) && defined(CONFIG_SDK_RTL8390)) || (defined(CONFIG_SDK_RTL8390) && defined(CONFIG_SDK_RTL8380))
    #include <hwp/hw_profiles/internel_profiles/rtl8393m_two8382m_simu.c>
#endif

#if defined(CONFIG_SDK_RTL8390)

#endif /* CONFIG_SDK_RTL8390 */

#if (defined(__BOOTLOADER__) && defined(CONFIG_SDK_RTL8390)) || (defined(CONFIG_SDK_RTL8390) && defined(CONFIG_SDK_RTL9310))
  #include <hwp/hw_profiles/internel_profiles/rtl8391m_rtl9310_fpga.c>
  #include <hwp/hw_profiles/internel_profiles/rtl8391m_rtl9310_fpga_l3.c>
#endif

#if defined(CONFIG_SDK_RTL9310)
    #define RTL9310_INTERNAL_PROFILE\
        &rtl9310_48ge_4xge_fpga,\
        &rtl9310_fpga,\
        &rtl9310_fpga_l3,\
        &rtl9311_ft,
#else
    #define RTL9310_INTERNAL_PROFILE
#endif

#if defined(CONFIG_SDK_RTL9300)
    #define RTL9300_INTERNAL_PROFILE\
        &rtl9300_24ge_4xge_fpga,\
        &rtl9300_8xge_fpga,\
        &rtl9300_24ge_4xge_i2c_fpga,\
        &rtl9300_cascade_24ge_4xge_i2c_fpga,\
        &rtl9303_8xge_qa1,
#else
    #define RTL9300_INTERNAL_PROFILE
#endif

#if defined(CONFIG_SDK_RTL8390) && defined(CONFIG_SDK_RTL9300)
    #define RTL8390_RTL9300_INTERNAL_PROFILE\
        &rtl8393m_rtl9300_24fe_4xge_fpga,\
        &rtl8393m_rtl9300_8xge_fpga,
#else
    #define RTL8390_RTL9300_INTERNAL_PROFILE
#endif

#if (defined(__BOOTLOADER__) && defined(CONFIG_SDK_RTL8390)) || (defined(CONFIG_SDK_RTL8390) && defined(CONFIG_SDK_RTL8380))
    #define RTL8390_RTL8380_INTERNAL_PROFILE\
        &rtl8393m_two8382m_simu,
#else
    #define RTL8390_RTL8380_INTERNAL_PROFILE
#endif

#if defined(CONFIG_SDK_RTL8390)
    #define RTL8390_STK_INTERNAL_PROFILE
#else
    #define RTL8390_STK_INTERNAL_PROFILE
#endif

#if (defined(__BOOTLOADER__) && defined(CONFIG_SDK_RTL8390)) || (defined(CONFIG_SDK_RTL8390) && defined(CONFIG_SDK_RTL9310))
    #define RTL8390_RTL9310_INTERNAL_PROFILE\
        &rtl8391m_rtl9310_fpga,\
        &rtl8391m_rtl9310_fpga_l3,
#else
    #define RTL8390_RTL9310_INTERNAL_PROFILE
#endif


/*
 * Data Declaration
 */
#define RTK_INTERNAL_PROFILE\
    RTL9310_INTERNAL_PROFILE\
    RTL9300_INTERNAL_PROFILE\
    RTL8390_RTL9300_INTERNAL_PROFILE\
    RTL8390_RTL9310_INTERNAL_PROFILE\
    RTL8390_RTL8380_INTERNAL_PROFILE



