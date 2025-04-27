/*
 * Copyright(c) Realtek Semiconductor Corporation, 2012
 * All rights reserved.
 *
 * Purpose :
 *
 * Feature :
 *
 */


/*
 * Include Files
 */
#include <common/rt_autoconf.h>
#include <dal/dal_construct.h>
#if defined(CONFIG_SDK_RTL8380)
  #include <dal/maple/dal_maple_construct.h>
#endif
#if defined(CONFIG_SDK_RTL8390)
  #include <dal/cypress/dal_cypress_construct.h>
#endif
#if defined(CONFIG_SDK_RTL9300)
  #include <dal/longan/dal_longan_construct.h>
#endif
#if defined(CONFIG_SDK_RTL9310)
  #include <dal/mango/dal_mango_construct.h>
#endif
#include <hwp/hw_profile.h>

static int _debug_regShow=0;
/* Function Name:
 *      debug_regShow_set
 * Description:
 *      debug purpose, to enable/disable the message print for register writing
 * Input:
 *      x   - 1: enable, 0: disable
 * Output:
 *      None
 * Return:
 *      the original setting
 * Note:
 *      None
 */
int debug_regShow_set(int x)
{
    int tmp=_debug_regShow;

#if defined(CNSTRT_DEBUG) && defined(CNSTRT_DEBUG_REG)
    _debug_regShow=x;
#else
    _debug_regShow=0;
#endif

    return tmp;
}

/* Function Name:
 *      debug_regShow_get
 * Description:
 *      debug purpose, to get the status that debug_regShow_set() set
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      1: enable
 *      0: disable
 * Note:
 *      None
 */
int debug_regShow_get(void)
{
    return _debug_regShow;
}

construct_t constructEntry_838x[]={
#if defined(CONFIG_SDK_RTL8380)
    {dal_maple_construct_default_init,          "Chip Construct"},
    {dal_maple_construct_pollingPhy_disable,    "Disable PHY Polling"},
    {dal_maple_construct_phy_reset,             "PHY Reset"},
    {dal_maple_construct_macConfig_init,        "MAC Construct"},
    {dal_maple_construct_serdesConfig_init,     "Serdes Construct"},
    {dal_maple_construct_phyConfig_init,        "PHY Construct"},
    {dal_maple_construct_pollingPhy_enable,     "Enable PHY Polling"},
    {dal_maple_construct_phy_powerOff,          "PHY Power Off"},
#endif
    {NULL, ""},
};

construct_t constructEntry_839x[]={
#if defined(CONFIG_SDK_RTL8390)
    {dal_cypress_construct_default_init,        "Chip Construct"},
    {dal_cypress_construct_phy_reset,           "PHY Reset"},
    {dal_cypress_construct_phyConfig_init,      "PHY Construct"},
    {dal_cypress_construct_serdesConfig_init,   "Serdes Construct"},
#endif
    {NULL, ""},
};

construct_t constructEntry_930x[]={
#if defined(CONFIG_SDK_RTL9300)
    {dal_longan_construct_default_init,         "Chip Construct"},
    {dal_longan_construct_pollingPhy_disable,   "Disable PHY Polling"},
    {dal_longan_construct_phy_reset,            "PHY Reset"},
    {dal_longan_construct_macConfig_init,       "MAC Construct"},
    {dal_longan_construct_serdes_off,           "Turn Off Serdes"},
    {dal_longan_construct_serdesConfig_init,    "Serdes Construct"},
    {dal_longan_construct_phyConfig_init,       "PHY Construct"},
    {dal_longan_construct_serdes_on,            "Turn On Serdes"},
    {dal_longan_construct_MacPollingPhy_init,   "Mac_Polling_PHY Config"},
    {dal_longan_construct_pollingPhy_enable,    "Enable PHY Polling"},
    {dal_longan_construct_miscConfig_init,      "Misc"},
#endif
    {NULL, ""},
};

construct_t constructEntry_931x[]={
#if defined(CONFIG_SDK_RTL9310)
    {dal_mango_construct_default_init,          "Chip Construct"},
    {dal_mango_construct_serdes_off,            "Turn Off Serdes"},
    {dal_mango_construct_pollingConfig_disable, "Disable PHY Polling"},
    {dal_mango_construct_phy_reset,             "PHY Reset"},
    {dal_mango_construct_phyConfig_init,        "PHY Construct"},
    {dal_mango_construct_serdesConfig_init,     "Serdes Construct"},
    {dal_mango_construct_macPollingPhy_init,    "MAC-Polling-PHY Config"},
    {dal_mango_construct_pollingConfig_init,    "Enable PHY Polling"},
#endif
    {NULL, ""},
};

/* Function Name:
 *      chip_construct
 * Description:
 *      Apply the basic parameters to PHY/MAC chips, named chip construct
 * Input:
 *      unit    - unit ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK: ok
 *      RT_ERR_FAILED: NG
 * Note:
 *      None
 */
int chip_construct(uint32 unit)
{
    construct_t *constructEntry=NULL;

    if((HWP_CHIP_FAMILY_ID(unit)==RTL8380_FAMILY_ID)||(HWP_CHIP_FAMILY_ID(unit)==RTL8330_FAMILY_ID))
    {
        constructEntry = constructEntry_838x;
    }
    else if((HWP_CHIP_FAMILY_ID(unit)==RTL8390_FAMILY_ID)||(HWP_CHIP_FAMILY_ID(unit)==RTL8350_FAMILY_ID))
    {
        constructEntry = constructEntry_839x;
    }
    else if(HWP_CHIP_FAMILY_ID(unit)==RTL9300_FAMILY_ID)
    {
        constructEntry = constructEntry_930x;
    }
    else if(HWP_CHIP_FAMILY_ID(unit)==RTL9310_FAMILY_ID)
    {
        constructEntry = constructEntry_931x;
    }

    if(constructEntry==NULL)
        return RT_ERR_FAILED;

    RT_INIT_MSG("  Chip Construct (unit %d)\n", unit);

    while(constructEntry->pFunc!=NULL)
    {
        RT_INIT_MSG("    %s\n",constructEntry->description);
        (*constructEntry->pFunc)(unit);
        constructEntry++;
    }

    return RT_ERR_OK;

} /* end of chip_construct */

