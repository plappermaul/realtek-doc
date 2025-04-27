/*
 * Copyright(c) Realtek Semiconductor Corporation, 2008
 * All rights reserved.
 *
 * Purpose : RTL83xx SOC and SWCORE commands for U-Boot.
 *
 * Feature :
 *
 */


/*
 * Include Files
 */
#include <config.h>
#include <common.h>
#include <command.h>
#include <common/util.h>
#include <uboot/cmd/uboot_cmd.h>
#include <common/type.h>
#include <common/rt_autoconf.h>
#include <osal/time.h>
#include "hwp/hw_profile.h"
#include "dal/dal_construct.h"
#include "dal/maple/dal_maple_construct.h"
#include "dal/cypress/dal_cypress_construct.h"
#include "hal/phy/construct/conftypes.h"
#include "hal/mac/miim_common_drv.h"
#include <hal/mac/drv/drv.h>
#include <hal/mac/serdes.h>
#include <rtk/l2.h>
#include <rtk/port.h>
#include <dal/longan/dal_longan_port.h>
#include <dal/longan/dal_longan_sds.h>
#if defined(CONFIG_SDK_RTL8380)
#include <hal/chipdef/maple/rtk_maple_reg_struct.h>
#include <hal/mac/drv/drv_rtl8380.h>
#endif
#if defined(CONFIG_SDK_RTL8390)
#include <hal/chipdef/cypress/rtk_cypress_reg_struct.h>
#include <hal/mac/drv/drv_rtl8390.h>
#endif
#if defined(CONFIG_SDK_RTL9310)
#include <hal/chipdef/mango/rtk_mango_reg_struct.h>
#include <hal/mac/drv/drv_rtl9310.h>
#include "dal/mango/dal_mango_port.h"
#include "hal/phy/phy_rtl9310.h"
#endif
#if defined(CONFIG_SDK_RTL9300)
#include <hal/chipdef/longan/rtk_longan_reg_struct.h>
#endif
#if defined(CONFIG_SDK_DRIVER_GPIO)
#include <drv/gpio/generalCtrl_gpio.h>
#endif
#if defined(CONFIG_SDK_DRIVER_I2C)
#include <drv/i2c/i2c.h>
#endif/* CONFIG_SDK_DRIVER_I2C */

extern unsigned int uboot_cmd_unit;


#if (defined(CONFIG_SDK_RTL8214FB) || defined(CONFIG_SDK_RTL8214B) || defined(CONFIG_SDK_RTL8212B))
  #include "hal/phy/phy_rtl8214fb.h"
#endif/* defined(CONFIG_SDK_RTL8214FB) || defined(CONFIG_SDK_RTL8214B) || defined(CONFIG_SDK_RTL8212B) */
#if (defined(CONFIG_SDK_RTL8214FC) || defined(CONFIG_SDK_RTL8218FB))
  #include "hal/phy/phy_rtl8218b.h"
#endif/* defined(CONFIG_SDK_RTL8214FC) || defined(CONFIG_SDK_RTL8218FB) */
#ifdef CONFIG_EEE
  #if (defined(CONFIG_SDK_RTL8214FB) || defined(CONFIG_SDK_RTL8214B) || defined(CONFIG_SDK_RTL8212B))
    #if defined(CONFIG_SDK_RTL8380)
confcode_prv_t rtl8380_rtl8214fb_eee_enable[] = {
    //puts "=============================================================================="
    //puts "Start at: [clock format [clock seconds] -format %c]"
    //puts "=============================================================================="
    //set PHYID 0
    //####################################################################################################
    //#    Force Select Copper Standard Register
    //####################################################################################################
    //puts "Force Select Copper Standard Register ..."
    {1, 31, 0x0008}, // change to page 8
    {1, 16, 0x0F00}, // force select copper standard register
    {1, 31, 0x0008}, // change to page 8
    //####################################################################################################
    //#    Enable Parallel Write
    //####################################################################################################
    //puts "Enable Parallel Write ..."
    {3, 31, 0x0008}, // change to page 8
    {3, 24, 0x0001}, // enable parallel write
    {3, 31, 0x0008}, // change to page 8
    //####################################################################################################
    //#    Power Down PHY
    //####################################################################################################
    //puts "Power Down PHY ..."
    {3, 31, 0x0008}, // change to page 8
    {3,  0, 0x1940}, // power down PHY
    {3, 31, 0x0008}, // change to page 8
    //####################################################################################################
    //#    Micro-C Enable or Disable Auto Turn off EEE
    //####################################################################################################
    //puts "Micro-C Enable or Disable Auto Turn off EEE ..."
    {3, 31, 0x0005}, // change to page 5
    {3,  5, 0x8B85}, // set Micro-C memory address (enable or disable auto turn off EEE)
    {3,  6, 0xE286}, // set Micro-C memory data (enable or disable auto turn off EEE) (enable: 0xE286, disable: 0xC286)
    {3, 31, 0x0008}, // change to page 8
    //####################################################################################################
    //#    Micro-C Control 10M EEE
    //####################################################################################################
    //puts "Micro-C Control 10M EEE ..."
    {3, 31, 0x0005}, // change to page 5
    {3,  5, 0x8B86}, // set Micro-C memory address (control 10M EEE)
    {3,  6, 0x8600}, // set Micro-C memory data (control 10M EEE) (enable: 0x8601, disable: 0x8600)
    {3, 31, 0x0008}, // change to page 8
    //####################################################################################################
    //#    Enable or Disable EEE
    //####################################################################################################
    //puts "Enable or Disable EEE ..."
    {3, 31, 0x0007}, // change to page 7
    {3, 30, 0x0020}, // change to ext. page 32
    {3, 21, 0x0100}, // enable or disable EEE (enable: 0x0100, disable: 0x0000)
    {3, 27, 0xA03A}, // force EEE PHY mode (PHY mode: 0xA0BA, MAC mode: 0xA03A)
    {3, 31, 0x0008}, // change to page 8
    //####################################################################################################
    //#    100/1000M EEE Capability
    //####################################################################################################
    //puts "100/1000M EEE Capability ..."
    {3, 31, 0x0000}, // change to page 0
    {3, 13, 0x0007}, // MMD register 7.60
    {3, 14, 0x003C}, // MMD register 7.60
    {3, 13, 0x4007}, // MMD register 7.60
    {3, 14, 0x0006}, // enable 100/1000M EEE capability
    {3, 13, 0x0000}, // MMD register
    {3, 31, 0x0008}, // change to page 8
    //####################################################################################################
    //#    10M EEE Amplitude
    //####################################################################################################
    //puts "10M EEE Amplitude ..."
    {3, 31, 0x0002}, // change to page 2
    {3, 11, 0x17A7}, // 10M EEE amplitude
    {3, 31, 0x0008}, // change to page 8
    //####################################################################################################
    //#    Disable Parallel Write
    //####################################################################################################
    //puts "Disable Parallel Write ..."
    {3, 31, 0x0008}, // change to page 8
    {3, 24, 0x0000}, // disable parallel write
    {3, 31, 0x0008}, // change to page 8
};

confcode_prv_t rtl8380_rrtl8214fb_eee_disable[] = {
    //puts "=============================================================================="
    //puts "Start at: [clock format [clock seconds] -format %c]"
    //puts "=============================================================================="
    //set PHYID 0
    //####################################################################################################
    //#    Force Select Copper Standard Register
    //####################################################################################################
    //puts "Force Select Copper Standard Register ..."
    {1, 31, 0x0008}, // change to page 8
    {1, 16, 0x0F00}, // force select copper standard register
    {1, 31, 0x0008}, // change to page 8
    //####################################################################################################
    //#    Enable Parallel Write
    //####################################################################################################
    //puts "Enable Parallel Write ..."
    {3, 31, 0x0008}, // change to page 8
    {3, 24, 0x0001}, // enable parallel write
    {3, 31, 0x0008}, // change to page 8
    //####################################################################################################
    //#    Power Down PHY
    //####################################################################################################
    //puts "Power Down PHY ..."
    {3, 31, 0x0008}, // change to page 8
    {3,  0, 0x1940}, // power down PHY
    {3, 31, 0x0008}, // change to page 8
    //####################################################################################################
    //#    Micro-C Enable or Disable Auto Turn off EEE
    //####################################################################################################
    //puts "Micro-C Enable or Disable Auto Turn off EEE ..."
    {3, 31, 0x0005}, // change to page 5
    {3,  5, 0x8B85}, // set Micro-C memory address (enable or disable auto turn off EEE)
    {3,  6, 0xC286}, // set Micro-C memory data (enable or disable auto turn off EEE) (enable: 0xE286, disable: 0xC286)
    {3, 31, 0x0008}, // change to page 8
    //####################################################################################################
    //#    Micro-C Control 10M EEE
    //####################################################################################################
    //puts "Micro-C Control 10M EEE ..."
    {3, 31, 0x0005}, // change to page 5
    {3,  5, 0x8B86}, // set Micro-C memory address (control 10M EEE)
    {3,  6, 0x8600}, // set Micro-C memory data (control 10M EEE) (enable: 0x8601, disable: 0x8600)
    {3, 31, 0x0008}, // change to page 8
    //####################################################################################################
    //#    Enable or Disable EEE
    //####################################################################################################
    //puts "Enable or Disable EEE ..."
    {3, 31, 0x0007}, // change to page 7
    {3, 30, 0x0020}, // change to ext. page 32
    {3, 21, 0x0000}, // enable or disable EEE (enable: 0x0100, disable: 0x0000)
    {3, 27, 0xA03A}, // force EEE PHY mode (PHY mode: 0xA0BA, MAC mode: 0xA03A)
    {3, 31, 0x0008}, // change to page 8
    //####################################################################################################
    //#    100/1000M EEE Capability
    //####################################################################################################
    //puts "100/1000M EEE Capability ..."
    {3, 31, 0x0000}, // change to page 0
    {3, 13, 0x0007}, // MMD register 7.60
    {3, 14, 0x003C}, // MMD register 7.60
    {3, 13, 0x4007}, // MMD register 7.60
    {3, 14, 0x0000}, // disable 100/1000M EEE capability
    {3, 13, 0x0000}, // MMD register
    {3, 31, 0x0008}, // change to page 8
    //####################################################################################################
    //#    10M EEE Amplitude
    //####################################################################################################
    //puts "10M EEE Amplitude ..."
    {3, 31, 0x0002}, // change to page 2
    {3, 11, 0x17A7}, // 10M EEE amplitude
    {3, 31, 0x0008}, // change to page 8
    //####################################################################################################
    //#    Disable Parallel Write
    //####################################################################################################
    //puts "Disable Parallel Write ..."
    {3, 31, 0x0008}, // change to page 8
    {3, 24, 0x0000}, // disable parallel write
    {3, 31, 0x0008}, // change to page 8
};
    #endif/*end of CONFIG_SDK_RTL8380*/
    #if defined(CONFIG_SDK_RTL8390)
confcode_prv_t rtl8214fb_eee_enable[] = {
    //puts "=============================================================================="
    //puts "Start at: [clock format [clock seconds] -format %c]"
    //puts "=============================================================================="
    //set PHYID 0
    //####################################################################################################
    //#    Force Select Copper Standard Register
    //####################################################################################################
    //puts "Force Select Copper Standard Register ..."
    {1, 31, 0x0008}, // change to page 8
    {1, 16, 0x0F00}, // force select copper standard register
    {1, 31, 0x0008}, // change to page 8
    //####################################################################################################
    //#    Enable Parallel Write
    //####################################################################################################
    //puts "Enable Parallel Write ..."
    {3, 31, 0x0008}, // change to page 8
    {3, 24, 0x0001}, // enable parallel write
    {3, 31, 0x0008}, // change to page 8
    //####################################################################################################
    //#    Power Down PHY
    //####################################################################################################
    //puts "Power Down PHY ..."
    {3, 31, 0x0008}, // change to page 8
    {3,  0, 0x1940}, // power down PHY
    {3, 31, 0x0008}, // change to page 8
    //####################################################################################################
    //#    Micro-C Enable or Disable Auto Turn off EEE
    //####################################################################################################
    //puts "Micro-C Enable or Disable Auto Turn off EEE ..."
    {3, 31, 0x0005}, // change to page 5
    {3,  5, 0x8B85}, // set Micro-C memory address (enable or disable auto turn off EEE)
    {3,  6, 0xE286}, // set Micro-C memory data (enable or disable auto turn off EEE) (enable: 0xE286, disable: 0xC286)
    {3, 31, 0x0008}, // change to page 8
    //####################################################################################################
    //#    Micro-C Control 10M EEE
    //####################################################################################################
    //puts "Micro-C Control 10M EEE ..."
    {3, 31, 0x0005}, // change to page 5
    {3,  5, 0x8B86}, // set Micro-C memory address (control 10M EEE)
    {3,  6, 0x8600}, // set Micro-C memory data (control 10M EEE) (enable: 0x8601, disable: 0x8600)
    {3, 31, 0x0008}, // change to page 8
    //####################################################################################################
    //#    Enable or Disable EEE
    //####################################################################################################
    //puts "Enable or Disable EEE ..."
    {3, 31, 0x0007}, // change to page 7
    {3, 30, 0x0020}, // change to ext. page 32
    {3, 21, 0x0100}, // enable or disable EEE (enable: 0x0100, disable: 0x0000)
    {3, 27, 0xA03A}, // force EEE PHY mode (PHY mode: 0xA0BA, MAC mode: 0xA03A)
    {3, 31, 0x0008}, // change to page 8
    //####################################################################################################
    //#    100/1000M EEE Capability
    //####################################################################################################
    //puts "100/1000M EEE Capability ..."
    {3, 31, 0x0000}, // change to page 0
    {3, 13, 0x0007}, // MMD register 7.60
    {3, 14, 0x003C}, // MMD register 7.60
    {3, 13, 0x4007}, // MMD register 7.60
    {3, 14, 0x0006}, // enable 100/1000M EEE capability
    {3, 13, 0x0000}, // MMD register
    {3, 31, 0x0008}, // change to page 8
    //####################################################################################################
    //#    10M EEE Amplitude
    //####################################################################################################
    //puts "10M EEE Amplitude ..."
    {3, 31, 0x0002}, // change to page 2
    {3, 11, 0x17A7}, // 10M EEE amplitude
    {3, 31, 0x0008}, // change to page 8
    //####################################################################################################
    //#    Disable Parallel Write
    //####################################################################################################
    //puts "Disable Parallel Write ..."
    {3, 31, 0x0008}, // change to page 8
    {3, 24, 0x0000}, // disable parallel write
    {3, 31, 0x0008}, // change to page 8
};

confcode_prv_t rtl8214fb_eee_disable[] = {
    //puts "=============================================================================="
    //puts "Start at: [clock format [clock seconds] -format %c]"
    //puts "=============================================================================="
    //set PHYID 0
    //####################################################################################################
    //#    Force Select Copper Standard Register
    //####################################################################################################
    //puts "Force Select Copper Standard Register ..."
    {1, 31, 0x0008}, // change to page 8
    {1, 16, 0x0F00}, // force select copper standard register
    {1, 31, 0x0008}, // change to page 8
    //####################################################################################################
    //#    Enable Parallel Write
    //####################################################################################################
    //puts "Enable Parallel Write ..."
    {3, 31, 0x0008}, // change to page 8
    {3, 24, 0x0001}, // enable parallel write
    {3, 31, 0x0008}, // change to page 8
    //####################################################################################################
    //#    Power Down PHY
    //####################################################################################################
    //puts "Power Down PHY ..."
    {3, 31, 0x0008}, // change to page 8
    {3,  0, 0x1940}, // power down PHY
    {3, 31, 0x0008}, // change to page 8
    //####################################################################################################
    //#    Micro-C Enable or Disable Auto Turn off EEE
    //####################################################################################################
    //puts "Micro-C Enable or Disable Auto Turn off EEE ..."
    {3, 31, 0x0005}, // change to page 5
    {3,  5, 0x8B85}, // set Micro-C memory address (enable or disable auto turn off EEE)
    {3,  6, 0xC286}, // set Micro-C memory data (enable or disable auto turn off EEE) (enable: 0xE286, disable: 0xC286)
    {3, 31, 0x0008}, // change to page 8
    //####################################################################################################
    //#    Micro-C Control 10M EEE
    //####################################################################################################
    //puts "Micro-C Control 10M EEE ..."
    {3, 31, 0x0005}, // change to page 5
    {3,  5, 0x8B86}, // set Micro-C memory address (control 10M EEE)
    {3,  6, 0x8600}, // set Micro-C memory data (control 10M EEE) (enable: 0x8601, disable: 0x8600)
    {3, 31, 0x0008}, // change to page 8
    //####################################################################################################
    //#    Enable or Disable EEE
    //####################################################################################################
    //puts "Enable or Disable EEE ..."
    {3, 31, 0x0007}, // change to page 7
    {3, 30, 0x0020}, // change to ext. page 32
    {3, 21, 0x0000}, // enable or disable EEE (enable: 0x0100, disable: 0x0000)
    {3, 27, 0xA03A}, // force EEE PHY mode (PHY mode: 0xA0BA, MAC mode: 0xA03A)
    {3, 31, 0x0008}, // change to page 8
    //####################################################################################################
    //#    100/1000M EEE Capability
    //####################################################################################################
    //puts "100/1000M EEE Capability ..."
    {3, 31, 0x0000}, // change to page 0
    {3, 13, 0x0007}, // MMD register 7.60
    {3, 14, 0x003C}, // MMD register 7.60
    {3, 13, 0x4007}, // MMD register 7.60
    {3, 14, 0x0000}, // disable 100/1000M EEE capability
    {3, 13, 0x0000}, // MMD register
    {3, 31, 0x0008}, // change to page 8
    //####################################################################################################
    //#    10M EEE Amplitude
    //####################################################################################################
    //puts "10M EEE Amplitude ..."
    {3, 31, 0x0002}, // change to page 2
    {3, 11, 0x17A7}, // 10M EEE amplitude
    {3, 31, 0x0008}, // change to page 8
    //####################################################################################################
    //#    Disable Parallel Write
    //####################################################################################################
    //puts "Disable Parallel Write ..."
    {3, 31, 0x0008}, // change to page 8
    {3, 24, 0x0000}, // disable parallel write
    {3, 31, 0x0008}, // change to page 8
};
    #endif/*(defined(CONFIG_SDK_RTL8390)*/
  #endif /* (defined(CONFIG_SDK_RTL8214FB) || defined(CONFIG_SDK_RTL8214B) || defined(CONFIG_SDK_RTL8212B)) */
#endif/*(defined(CONFIG_EEE)*/

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
extern int sdk_init_times[];


/*
 * Macro Definition
 */
#define LEDON(_unit) (HWP_8380_30_FAMILY(_unit) ? (0x05) : (0x07))
#define BDINFO_VAR_BOARDID         "boardid"
#define BDINFO_VAR_BOARDMODEL      "boardmodel"

/*
 * Function Declaration
 */


/* Function Name:
 *      rtk_network_on
 * Description:
 *      Enable networking functionality and configure hardware to be able to send and receive packets.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void rtk_network_on(void)
{
    uint32  unit;
    uint32  port;

    if (RT_ERR_FAILED == uboot_sdk_init(UBOOT_SDK_INIT_PHASE_RTK))
        return;

    osal_printf("Please wait for PHY init-time ...\n\n");

    HWP_UNIT_TRAVS_LOCAL(unit){
#if defined(CONFIG_SDK_RTL8380)
        if(HWP_8380_30_FAMILY(unit))
        {
            ioal_mem32_write(unit, 0xa174, 0x6192f);  /*port 28 force link up*/
            HWP_PORT_TRAVS(unit, port)
            {
                phy_enable_set(unit, port, ENABLED);
            }
        }
        else
#endif
#if defined(CONFIG_SDK_RTL9310)
        if(HWP_9310_FAMILY_ID(unit))
        {
            HWP_PORT_TRAVS(unit, port)
            {
                dal_mango_port_adminEnable_set(unit, port, ENABLED);
            }
        }
        else
#endif
        {
            HWP_PORT_TRAVS(unit, port)
            {
                phy_enable_set(unit, port, ENABLED);
            }
        }
    }//HWP_UNIT_TRAVS_LOCAL()

    /* delay for link-status stable */
    if(HWP_8380_30_FAMILY(HWP_MY_UNIT_ID()))
        osal_time_mdelay(4000);
    else
        osal_time_mdelay(2000);
}

/* Function Name:
 *      rtk_network_off
 * Description:
 *      Disable networking functionality and disable hardware from send and receive packets.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void rtk_network_off(void)
{
    uint32 unit;

    HWP_UNIT_TRAVS_LOCAL(unit){

#if defined(CONFIG_SDK_RTL8380)
        if(HWP_8380_30_FAMILY(unit))
        {
            ioal_mem32_write(unit, 0xa174, 0x6192d); /*port 28 force link down*/
            dal_maple_construct_phy_powerOff(unit);
        }
        else
#endif
        {
            uint32      port;

            HWP_PORT_TRAVS(unit, port)
            {
                phy_enable_set(unit, port, DISABLED);
            }
        }
    }//HWP_UNIT_TRAVS_LOCAL()
}


/* Function Name:
 *      rtk_hwProfileList_show
 * Description:
 *      Show available hardware profile list
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void rtk_hwProfileList_show(void)
{
   hwp_profileList_show();
}
/* Function Name:
 *      rtk_forcelinkDown10Gserdes
 * Description:
 *      force linkdown 10g serdes port
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void rtk_forcelinkDown10Gserdes(uint32 unit)
{
#if defined(CONFIG_SDK_RTL9300)
    uint32 port;
    HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
    {
        if ((HWP_PHY_EXIST(unit, port) || HWP_SERDES_PORT(unit, port)) && !HWP_CASCADE_PORT(unit, port))
            dal_longan_port_adminEnable_set(unit, port, DISABLED);
        if (  (HWP_10GE_SERDES_PORT(unit, port) &&  !HWP_CASCADE_PORT(unit, port))
          || (HWP_PHY_MODEL_BY_PORT(unit, port) == RTK_PHYTYPE_RTL8295R))
            _dal_longan_port_macForceLink_set(unit, port, ENABLED, PORT_LINKDOWN,PORT_DEFAULT);
    }
#endif
}
/* Function Name:
 *      _rtk_comboport_media_set
 * Description:
 *      Configure all Combo ports' media mode
 * Input:
 *      unit  - unit id
 *      media - media mode
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void _rtk_comboport_media_set(uint32  unit, rtk_port_media_t  media)
{
    rtk_port_t      macId;
    int32           ret;


    HWP_PORT_TRAVS(unit, macId)
    {
        if (!HWP_PHY_EXIST(unit, macId))
            continue;

        if (!HWP_COMBO_PORT(unit, macId))
            continue;

        if ((ret = phy_media_set(unit, macId, media)) != RT_ERR_OK)
        {
            osal_printf("Set comboport media(%d) fail(%d) on unit %u port %u.\n", media, ret, unit, macId);
        }
    }/* end HWP_PORT_TRAVS */

    return;
}

/* Function Name:
 *      rtk_comboport_fiber
 * Description:
 *      Force comboport into fiber mode.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void rtk_comboport_fiber(void)
{
    uint32  unit = uboot_cmd_unit;

    _rtk_comboport_media_set(unit, PORT_MEDIA_FIBER);
}

/* Function Name:
 *      rtk_comboport_copper
 * Description:
 *      Force comboport into copper mode.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void rtk_comboport_copper(void)
{
    uint32  unit = uboot_cmd_unit;

    _rtk_comboport_media_set(unit, PORT_MEDIA_COPPER);
}


/* Function Name:
 *      _rtk_comboport_port_media_set
 * Description:
 *      Configure Combo port media mode
 * Input:
 *      unit  - unit id
 *      macId - mac id (port number)
 *      media - media mode
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void _rtk_comboport_port_media_set(uint32 unit, uint32 macId, rtk_port_media_t media)
{
    if (macId >= RTK_MAX_PORT_PER_UNIT)
    {
        return;
    }

    if (!HWP_PHY_EXIST(unit, macId))
    {
        return;
    }

    switch (HWP_PHY_MODEL_BY_PORT(unit, macId))
    {
#if defined(CONFIG_SDK_RTL8214FB) || defined(CONFIG_SDK_RTL8214B) || defined(CONFIG_SDK_RTL8212B)
      case RTK_PHYTYPE_RTL8214FB:
      case RTK_PHYTYPE_RTL8214B:
      case RTK_PHYTYPE_RTL8212B:
        phy_8214fb_media_set(unit, macId, media);
        break;
#endif/* defined(CONFIG_SDK_RTL8214FB) || defined(CONFIG_SDK_RTL8214B) || defined(CONFIG_SDK_RTL8212B) */
#if defined(CONFIG_SDK_RTL8214FC)
      case RTK_PHYTYPE_RTL8214FC:
        phy_8214fc_media_set(unit, macId, media);
        break;
#endif/* CONFIG_SDK_RTL8214FC */
#if defined(CONFIG_SDK_RTL8218FB)
      case RTK_PHYTYPE_RTL8218FB:
        phy_8218fb_media_set(unit, macId, media);
        break;
#endif/* CONFIG_SDK_RTL8218FB */
      default:
        break;
    }/* end of switch */

    return;
}

/* Function Name:
 *      rtk_comboport_portcopper
 * Description:
 *      Configure Combo-port as copper mode
 * Input:
 *      macId  - mac id (port number) to be configured
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void rtk_comboport_portcopper(int macId)
{
    uint32  unit = uboot_cmd_unit;

    _rtk_comboport_port_media_set(unit, (uint32)macId, PORT_MEDIA_COPPER);
}


/* Function Name:
 *      rtk_comboport_portfiber
 * Description:
 *      Configure Combo-port as fiber mode
 * Input:
 *      macId - mac id (port number) to be configured
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void rtk_comboport_portfiber(int macId)
{
    uint32  unit = uboot_cmd_unit;

    if (unit >= RTK_MAX_NUM_OF_UNIT_LOCAL)
    {
        osal_printf("\nUnit %u invalid unit id \n", unit);
        return;
    }

    _rtk_comboport_port_media_set(unit, (uint32)macId, PORT_MEDIA_FIBER);
}

/* Function Name:
 *      rtk_saLearning
 * Description:
 *      Configure Source MAC Address Learning mode
 * Input:
 *      unit        - unit ID
 *      lrnMode     - learning mode
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void rtk_saLearning(uint32 unit, rtk_l2_newMacLrnMode_t lrnMode)
{
    uint32          port;
    int32           ret;

    HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
    {
        if ((ret = rtk_l2_portNewMacOp_set(unit, port, lrnMode, ACTION_FORWARD)) != RT_ERR_OK)
        {
            osal_printf("Configure SA learning mode %d fail(%d) on unit %u port %u.\n", lrnMode, ret, unit, port);
        }
    }/* HWP_PORT_TRAVS */

}

/* Function Name:
 *      rtk_port_isolation_on
 * Description:
 *      Enable the port isolation.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void rtk_port_isolation_on(void)
{
    uint32          unit = uboot_cmd_unit;
    uint32          port;
    rtk_port_t      portA, portB;
    int32           ret;
    rtk_portmask_t  portmask;

    osal_printf("\nUnit %u\n", unit);
    /*Port Isolation settings: port 0<-->1, 2<-->3, 4<-->5.......*/
    HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
    {
        portA = port;
        portB = 0xFFFF;
        port++;
        for (; port < RTK_MAX_PORT_PER_UNIT; port++)
        {
            if (!HWP_PORT_EXIST(unit, port) || HWP_IS_CPU_PORT(unit, port))
                continue;
            portB = port;
            break;
        }

        if (portB == 0xFFFF)
        {
            break;
        }

        osal_printf("isolation: %2u <--> %2u\n", portA, portB);
        osal_memset(&portmask, 0, sizeof(rtk_portmask_t));
        RTK_PORTMASK_PORT_SET(portmask, portB);
        if ((ret = rtk_port_isolation_set(unit, portA, &portmask)) != RT_ERR_OK)
        {
            osal_printf("isolation fail(%d): source=%u, destination=%u\n", ret, portA, portB);
        }

        osal_memset(&portmask, 0, sizeof(rtk_portmask_t));
        RTK_PORTMASK_PORT_SET(portmask, portA);
        if ((ret = rtk_port_isolation_set(unit, portB, &portmask)) != RT_ERR_OK)
        {
            osal_printf("isolation fail(%d): source=%u, destination=%u\n", ret, portB, portA);
        }
    }/* end for */

    /*Disable SA learning*/
    rtk_saLearning(unit, NOT_LEARNING);
}

/* Function Name:
 *      rtk_port_isolation_off
 * Description:
 *      Disable the port isolation.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void rtk_port_isolation_off(void)
{
    uint32          unit = uboot_cmd_unit;
    uint32          port;
    int32           ret;
    rtk_portmask_t  portmask;

    osal_memset(&portmask, 0, sizeof(rtk_portmask_t));
    HWP_PORT_TRAVS(unit, port)
    {
        RTK_PORTMASK_PORT_SET(portmask, port);
    }

    HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
    {
        if ((ret = rtk_port_isolation_set(unit, port, &portmask)) != RT_ERR_OK)
        {
            osal_printf("isolation disable fail(%d): source=%u, destination=ALL\n", ret, port);
        }
    }/* end for */

    /*Enable SA learning*/
    rtk_saLearning(unit, HARDWARE_LEARNING);
}


#ifdef CONFIG_EEE

/* Function Name:
 *      rtk_eee_on
 * Description:
 *      Enable all PHY EEE feature
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void rtk_eee_on(void)
{
    int i;
    uint32  unit = uboot_cmd_unit;

#if defined(CONFIG_SDK_RTL8390)
    rtk_enable_t gval;
    uint32 pollSts = 0x0;
    if(HWP_8390_50_FAMILY(unit))
    {
        rtl8390_miim_globalPollingEnable_set(unit, pollSts);
    }
#endif

#if defined(CONFIG_SDK_RTL8380)
    unsigned int polling_mask = 0;

    if(HWP_8380_30_FAMILY(unit))
    {
        /*Backup polling mask*/
        reg_read(unit, MAPLE_SMI_POLL_CTRLr, &polling_mask);

        /*Disable MAC polling PHY*/
        reg_write(unit, MAPLE_SMI_POLL_CTRLr, 0);
    }
#endif

#if defined(CONFIG_SDK_RTL9310)
        unsigned int polling_mask = 0;

        if(HWP_9310_FAMILY_ID(unit))
        {
            HWP_PHY_TRAVS(unit, i)
            {
                rtl9310_miim_pollingEnable_set(unit, i, DISABLED);
            }
        }
#endif

    HWP_PHY_TRAVS(unit, i)
    {
        switch (HWP_SWITCH_DESCP(unit)->phy.descp[i].chip)
        {
            #if (defined(CONFIG_SDK_RTL8208D) || defined(CONFIG_SDK_RTL8208L))
            case RTK_PHYTYPE_RTL8208D:
            case RTK_PHYTYPE_RTL8208L:
                {
                    int j;
                    unsigned int phyData;
                    unsigned int rtl8208d_phy0_id = HWP_PHY_BASE_MACID_BY_IDX(unit, i);

                    for (j = (rtl8208d_phy0_id); j < (rtl8208d_phy0_id + HWP_SWITCH_DESCP(unit)->phy.descp[i].phy_max); j++)
                    {
                       phy_eeeEnable_set(unit, j, ENABLED);
                       phy_reg_get(unit, (int)j, 0, 0, &phyData);
                       if (phyData & 0x1000)
                       {
                           phyData |= (1 << 9);
                       }
                       phy_reg_set(unit, j, 0, 0, phyData);
                    }
                }
                break;
            #endif
            #if (defined(CONFIG_SDK_RTL8214FB) || defined(CONFIG_SDK_RTL8214B) || defined(CONFIG_SDK_RTL8212B))
            case RTK_PHYTYPE_RTL8214FB:
            case RTK_PHYTYPE_RTL8214B:
            case RTK_PHYTYPE_RTL8212B:
                {
                    unsigned int rtl8214fb_phy0_macid = HWP_PHY_BASE_MACID_BY_IDX(unit, i);
                    unsigned int          forceReg, phyData;
                    int          idx;

                    /* store the original register value */
                    phy_reg_set(unit, rtl8214fb_phy0_macid + 1,
                            HAL_MIIM_FIX_PAGE, 31, 8);
                    phy_reg_get(unit, rtl8214fb_phy0_macid + 1,
                            HAL_MIIM_FIX_PAGE, 16, &forceReg);
                    phy_reg_set(unit, rtl8214fb_phy0_macid + 1,
                            HAL_MIIM_FIX_PAGE, 31, 8);
              #if defined(CONFIG_SDK_RTL8380)
                    if(HWP_8380_30_FAMILY(unit))
                    {
                        for (idx = 0; idx < (sizeof(rtl8380_rtl8214fb_eee_enable)/sizeof(confcode_prv_t)); ++idx)
                        {
                            phy_reg_set(unit, rtl8214fb_phy0_macid + rtl8380_rtl8214fb_eee_enable[idx].phy, \
                                    HAL_MIIM_FIX_PAGE,
                                    (int)rtl8380_rtl8214fb_eee_enable[idx].reg,
                                    (int)rtl8380_rtl8214fb_eee_enable[idx].val);
                        }
                    }
              #endif
              #if defined(CONFIG_SDK_RTL8390)
                    if(HWP_8390_50_FAMILY(unit))
                    {
                        for (idx = 0; idx < (sizeof(rtl8214fb_eee_enable)/sizeof(confcode_prv_t)); ++idx)
                        {
                            phy_reg_set(unit, rtl8214fb_phy0_macid + rtl8214fb_eee_enable[idx].phy, \
                                    HAL_MIIM_FIX_PAGE,
                                    (int)rtl8214fb_eee_enable[idx].reg,
                                    (int)rtl8214fb_eee_enable[idx].val);
                        }
                    }
              #endif

                    /* Restart Auto-Negotiation */
                    for (idx = 0; idx < 4; ++idx)
                    {
                        phy_reg_get(unit, rtl8214fb_phy0_macid + idx,
                                HAL_MIIM_FIX_PAGE, 0, &phyData);

                        phyData |= (1 << 9);
                        phy_reg_set(unit, rtl8214fb_phy0_macid + idx,
                                HAL_MIIM_FIX_PAGE, 0, phyData);
                    }

                    /* restore the original register value */
                    phy_reg_set(unit, rtl8214fb_phy0_macid + 1,
                            HAL_MIIM_FIX_PAGE, 31, 8);
                    phy_reg_set(unit, rtl8214fb_phy0_macid + 1,
                            HAL_MIIM_FIX_PAGE, 16, forceReg);
                    phy_reg_set(unit, rtl8214fb_phy0_macid + 1,
                            HAL_MIIM_FIX_PAGE, 31, 8);
                    osal_time_mdelay(1000);
                }
                break;
            #endif
            #if (defined(RTK_CONFIG_RTL8218))
            case RTK_PHYTYPE_RTL8218:
                {
                    unsigned int rtl8218_macid;
                    uint j;
                    rtl8218_macid = HWP_PHY_BASE_MACID_BY_IDX(unit, i);

                    for (j = rtl8218_macid; j < (rtl8218_macid + HWP_SWITCH_DESCP(unit)->phy.descp[i].phy_max); j++)
                    {

                        //## ext page 32
                        phy_reg_set(unit, j,7,0x1e,0x0020);
                        phy_reg_set(unit, j,7,0x17,0x000a);
                        phy_reg_set(unit, j,7,0x1b,0x2fca);
                        phy_reg_set(unit, j,7,0x15,0x0100);

                        //###### negear EEE Nway ability autooff
                        phy_reg_set(unit, j,31,0x1f,0x0005);
                        phy_reg_set(unit, j,31,0x05,0x8b84);
                        //phy_reg_set(unit, j,31,0x06,0x0026);
                        phy_reg_set(unit, j,31,0x06,0x0062);
                        phy_reg_set(unit, j,31,0x1f,0x0000);
                    }

                    /* Force re-autonegotiation if AN is on */
                    unsigned int val;

                    for (j = (HWP_PHY_BASE_MACID_BY_IDX(unit, i) + 0); j < (HWP_PHY_BASE_MACID_BY_IDX(unit, i) + HWP_SWITCH_DESCP(unit)->phy.descp[i].phy_max); j++)
                    {
                        phy_reg_get(unit, j, 0, 0, &val);
                        if (val & (1 << 12))
                        {
                            phy_reg_set(unit, j, 0, 0, (val | (1 << 9)));
                        }
                    }
                }
                break;
            #endif
            #if (defined(CONFIG_SDK_RTL8218B) || defined(CONFIG_SDK_RTL8218FB) || defined(CONFIG_SDK_RTL8214FC))
            case RTK_PHYTYPE_RTL8208L_INT:
            case RTK_PHYTYPE_RTL8218B:
            case RTK_PHYTYPE_RTL8218FB:
            case RTK_PHYTYPE_RTL8214FC:
                {
              #if defined(CONFIG_SDK_RTL8390)
                    if(HWP_8390_50_FAMILY(unit))
                    {
                        int base = HWP_PHY_BASE_MACID_BY_IDX(unit, i);
                        int port, maxPort;
                        int j;

                        osal_printf("PHY[%d]: enable EEE\n", i);

                        if (RTK_PHYTYPE_RTL8214FC == HWP_SWITCH_DESCP(unit)->phy.descp[i].chip)
                            maxPort = 4;
                        else
                            maxPort = 8;

                        for (j = 0; j < maxPort; ++j)
                        {
                            unsigned int val;
                            port = base + j;

                            phy_reg_set(unit, port, 0xa42, 29, 0x1);
                            phy_reg_get(unit, port, 0xa43, 25, &val);
                            val &= ~(1 << 5);
                            phy_reg_set(unit, port, 0xa43, 25, val);

                            phy_reg_set(unit, port, 7, 60, 0x6);

                            /* Force re-autonegotiation if AN is on */
                            phy_reg_get(unit, port, 0, 0, &val);
                            if (val & (1 << 12))
                            {
                                val |= (1 << 9);
                                phy_reg_set(unit, port, 0, 0, val);
                            }

                            phy_reg_set(unit, port, 0xa42, 29, 0x0);
                        }
                    }
                #endif
                #if defined(CONFIG_SDK_RTL8380)
                    if(HWP_8380_30_FAMILY(unit))
                    {
                        int phy_idx;
                        unsigned int value;
                        unsigned int val;


                        /* 10M EEE Amplitude */
                        for(phy_idx = 0; phy_idx < HWP_SWITCH_DESCP(unit)->phy.descp[i].phy_max; phy_idx++)
                        {
                             /* Force Copper */
                            phy_reg_set(unit, HWP_PHY_BASE_MACID_BY_IDX(unit, i)+phy_idx, 0, 30, 0x0001);

                             /* 100/1000M EEE Capability */
                            phy_reg_set(unit, HWP_PHY_BASE_MACID_BY_IDX(unit, i) + phy_idx, 0, 13, 0x0007);
                            phy_reg_set(unit, HWP_PHY_BASE_MACID_BY_IDX(unit, i) + phy_idx, 0, 14, 0x003C);
                            phy_reg_set(unit, HWP_PHY_BASE_MACID_BY_IDX(unit, i) + phy_idx, 0, 13, 0x4007);
                            phy_reg_set(unit, HWP_PHY_BASE_MACID_BY_IDX(unit, i) + phy_idx, 0, 14, 0x0006);


                            phy_reg_get(unit, HWP_PHY_BASE_MACID_BY_IDX(unit, i) + phy_idx, 0x0A43, 25, &value);
                            value |= 1UL<<4;
                            phy_reg_set(unit, HWP_PHY_BASE_MACID_BY_IDX(unit, i) + phy_idx, 0x0A43, 25, value);

                            /* Force re-autonegotiation if AN is on */
                            phy_reg_get(unit, HWP_PHY_BASE_MACID_BY_IDX(unit, i) + phy_idx, 0, 0, &val);
                            if (val & (1 << 12))
                            {
                                phy_reg_set(unit, HWP_PHY_BASE_MACID_BY_IDX(unit, i) + phy_idx, 0, 0, (val | (1 << 9)));
                            }

                             /* Back to  AUTO */
                            phy_reg_set(unit, HWP_PHY_BASE_MACID_BY_IDX(unit, i) + phy_idx,0,30,0x0000);
                        }
                    }
                #endif
                }

                break;
            #endif
            default:
                osal_printf("PHY[%d]: not supported in EEE\n", i);
                break;
        }
    }
#if defined(CONFIG_SDK_RTL8390)

    pollSts = 0x1;
    if(HWP_8390_50_FAMILY(unit))
    {
        rtl8390_miim_globalPollingEnable_set(unit, pollSts);

        gval = 0;
        HWP_PORT_TRAVS(unit, i)
        {
            unsigned int cmp_val;
            unsigned int val=1;
            cmp_val = ~val;

            if (i >= 48)
                gval = 1;
            reg_array_field_write(unit,
                                      CYPRESS_MAC_FORCE_MODE_CTRLr,
                                      i,
                                      REG_ARRAY_INDEX_NONE,
                                      CYPRESS_EEE_100M_ENf,
                                      &val);
            reg_array_field_write(unit,
                                      CYPRESS_MAC_FORCE_MODE_CTRLr,
                                      i,
                                      REG_ARRAY_INDEX_NONE,
                                      CYPRESS_EEE_500M_ENf,
                                      &cmp_val);
            reg_array_field_write(unit,
                                      CYPRESS_MAC_FORCE_MODE_CTRLr,
                                      i,
                                      REG_ARRAY_INDEX_NONE,
                                      CYPRESS_EEE_1000M_ENf,
                                      &gval);
            reg_array_field_write(unit,
                                      CYPRESS_MAC_FORCE_MODE_CTRLr,
                                      i,
                                      REG_ARRAY_INDEX_NONE,
                                      CYPRESS_EEE_10G_ENf,
                                      &cmp_val);
            reg_array_field_write(unit,
                                      CYPRESS_MAC_FORCE_MODE_CTRLr,
                                      i,
                                      REG_ARRAY_INDEX_NONE,
                                      CYPRESS_EEE_TX_ENf,
                                      &(val));
            reg_array_field_write(unit,
                                      CYPRESS_MAC_FORCE_MODE_CTRLr,
                                      i,
                                      REG_ARRAY_INDEX_NONE,
                                      CYPRESS_EEE_RX_ENf,
                                      &(val));
        }
    }
#endif
#if defined(CONFIG_SDK_RTL8380)
    if(HWP_8380_30_FAMILY(unit))
    {
        /*Restore polling mask*/
        reg_write(unit, MAPLE_SMI_POLL_CTRLr, &polling_mask);
    }
#endif
}


/* Function Name:
 *      rtk_eee_off
 * Description:
 *      Disable all PHY EEE feature
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void rtk_eee_off(void)
{
    int i;
    uint32  unit = uboot_cmd_unit;
    rtk_enable_t enable = DISABLED;
    if (enable){}
#if defined(CONFIG_SDK_RTL8390)

    uint32 pollSts = 0x0;

    if(HWP_8390_50_FAMILY(unit))
    {
        rtl8390_miim_globalPollingEnable_set(unit, pollSts);
    }

#endif
#if defined(CONFIG_SDK_RTL8380)
    unsigned int polling_mask = 0;

    if(HWP_8380_30_FAMILY(unit))
    {
        /*Backup polling mask*/
        reg_read(unit, MAPLE_SMI_POLL_CTRLr, &polling_mask);

        /*Disable MAC polling PHY*/
        reg_write(unit, MAPLE_SMI_POLL_CTRLr, 0);
    }

#endif

#if defined(CONFIG_SDK_RTL9310)
            unsigned int polling_mask = 0;

            if(HWP_9310_FAMILY_ID(unit))
            {
                HWP_PHY_TRAVS(unit, i)
                {
                    rtl9310_miim_pollingEnable_set(unit, i, DISABLED);
                }
            }
#endif

    HWP_PHY_TRAVS(unit, i)
    {
        switch (HWP_SWITCH_DESCP(unit)->phy.descp[i].chip)
        {

            #if (defined(CONFIG_SDK_RTL8208D) || defined(CONFIG_SDK_RTL8208L))
            case RTK_PHYTYPE_RTL8208D:
            case RTK_PHYTYPE_RTL8208L:
            {
                unsigned int phyData;
                unsigned int rtl8208d_phy0_id = HWP_PHY_BASE_MACID_BY_IDX(unit, i);
                int j;
                for (j = (rtl8208d_phy0_id); j < (rtl8208d_phy0_id+HWP_SWITCH_DESCP(unit)->phy.descp[i].phy_max); j++)
                {
                    phy_eeeEnable_set(unit, j, enable);

                    /* Force re-autonegotiation if AN is on */
                    phy_reg_get(unit, (int)j, 0, 0, &phyData);
                    if (phyData & 0x1000)
                    {
                        phyData |= (1 << 9);
                    }
                    phy_reg_set(unit, j, 0, 0, phyData);
                }
            }
                break;
            #endif
            #if (defined(CONFIG_SDK_RTL8214FB) || defined(CONFIG_SDK_RTL8214B) || defined(CONFIG_SDK_RTL8212B))
            case RTK_PHYTYPE_RTL8214FB:
            case RTK_PHYTYPE_RTL8214B:
            case RTK_PHYTYPE_RTL8212B:
                {
                    unsigned int    rtl8214fb_phy0_macid = HWP_PHY_BASE_MACID_BY_IDX(unit, i);
                    unsigned int             forceReg, phyData;
                    int             idx;

                    /* store the original register value */
                    phy_reg_set(unit, rtl8214fb_phy0_macid + 1,
                            HAL_MIIM_FIX_PAGE, 31, 8);
                    phy_reg_get(unit, rtl8214fb_phy0_macid + 1,
                            HAL_MIIM_FIX_PAGE, 16, &forceReg);
                    phy_reg_set(unit, rtl8214fb_phy0_macid + 1,
                            HAL_MIIM_FIX_PAGE, 31, 8);
                #if defined(CONFIG_SDK_RTL8380)
                    if(HWP_8380_30_FAMILY(unit))
                    {
                        for (idx = 0; idx < (sizeof(rtl8380_rrtl8214fb_eee_disable)/sizeof(confcode_prv_t)); ++idx)
                        {
                            phy_reg_set(unit, rtl8214fb_phy0_macid + rtl8380_rrtl8214fb_eee_disable[idx].phy,
                                    HAL_MIIM_FIX_PAGE,
                                    (int)rtl8380_rrtl8214fb_eee_disable[idx].reg,
                                    (int)rtl8380_rrtl8214fb_eee_disable[idx].val);
                        }
                    }
                #endif
                #if defined(CONFIG_SDK_RTL8390)
                    if(HWP_8390_50_FAMILY(unit))
                    {
                        for (idx = 0; idx < (sizeof(rtl8214fb_eee_disable)/sizeof(confcode_prv_t)); ++idx)
                        {
                            phy_reg_set(unit, rtl8214fb_phy0_macid + rtl8214fb_eee_disable[idx].phy, \
                                    HAL_MIIM_FIX_PAGE,
                                    (int)rtl8214fb_eee_disable[idx].reg,
                                    (int)rtl8214fb_eee_disable[idx].val);
                        }
                    }
                #endif

                    /* Restart Auto-Negotiation */
                    for (idx = 0; idx < 4; ++idx)
                    {
                        phy_reg_get(unit, rtl8214fb_phy0_macid + idx,
                                HAL_MIIM_FIX_PAGE, 0, &phyData);

                        phyData |= (1 << 9);
                        phy_reg_set(unit, rtl8214fb_phy0_macid + idx,
                                HAL_MIIM_FIX_PAGE, 0, phyData);
                    }

                    /* restore the original register value */
                    phy_reg_set(unit, rtl8214fb_phy0_macid + 1,
                            HAL_MIIM_FIX_PAGE, 31, 8);
                    phy_reg_set(unit, rtl8214fb_phy0_macid + 1,
                            HAL_MIIM_FIX_PAGE, 16, forceReg);
                    phy_reg_set(unit, rtl8214fb_phy0_macid + 1,
                            HAL_MIIM_FIX_PAGE, 31, 8);
                    osal_time_mdelay(1000);
                }
                break;
            #endif
            #if (defined(RTK_CONFIG_RTL8218))
            case RTK_PHYTYPE_RTL8218:
            {
                unsigned int rtl8218_macid;
                unsigned int j;
                rtl8218_macid = HWP_PHY_BASE_MACID_BY_IDX(unit, i);

                //osal_printf("PHY[%d]: disable EEE on port [0x%08x]\n", i, rtl8218_macid_pm);
                for (j = rtl8218_macid; j < (rtl8218_macid + HWP_SWITCH_DESCP(unit)->phy.descp[i].phy_max); j++)
                {

                    //###### negear EEE Nway ability autooff
                    phy_reg_set(unit, j, 127, 0x1f, 0x0005);
                    phy_reg_set(unit, j, 127, 0x05, 0x8b84);
                    phy_reg_set(unit, j, 127, 0x06, 0x0042);
                    phy_reg_set(unit, j, 127, 0x1f, 0x0000);

                    //## ext page 32
                    phy_reg_set(unit, j, 7, 0x1e, 0x0020);
                    phy_reg_set(unit, j, 7, 0x17, 0x000a);
                    phy_reg_set(unit, j, 7, 0x1b, 0x2f4a);
                    phy_reg_set(unit, j, 7, 0x15, 0x0000);
                }

                /* Force re-autonegotiation if AN is on */
                unsigned int val;

                for (j = (HWP_PHY_BASE_MACID_BY_IDX(unit, i) + 0); j < (HWP_PHY_BASE_MACID_BY_IDX(unit, i) + HWP_SWITCH_DESCP(unit)->phy.descp[i].phy_max); j++)
                {
                    phy_reg_get(unit, j, 0, 0, &val);
                    if (val & (1 << 12))
                    {
                       phy_reg_set(unit, j, 0, 0, (val | (1 << 9)));
                    }
                }
            }
           break;
           #endif

            #if (defined(CONFIG_SDK_RTL8218B) || defined(CONFIG_SDK_RTL8214FC) || defined(CONFIG_SDK_RTL8218FB))
            case RTK_PHYTYPE_RTL8208L_INT:
            case RTK_PHYTYPE_RTL8218B:
            case RTK_PHYTYPE_RTL8214FC:
            case RTK_PHYTYPE_RTL8218FB:
                {
              #if defined(CONFIG_SDK_RTL8390)
                    if(HWP_8390_50_FAMILY(unit))
                    {
                        int base = HWP_PHY_BASE_MACID_BY_IDX(unit, i);
                        int port, maxPort;
                        int j;
                        unsigned int val;

                        osal_printf("PHY[%d]: disable EEE\n", i);

                        if (RTK_PHYTYPE_RTL8214FC == HWP_SWITCH_DESCP(unit)->phy.descp[i].chip)
                            maxPort = 4;
                        else
                            maxPort = 8;

                        for (j = 0; j < maxPort; ++j)
                        {
                            port = base + j;

                            phy_reg_set(unit, port, 0xa42, 29, 0x1);
                            phy_reg_get(unit, port, 0xa43, 25, &val);
                            val &= ~(1 << 5);
                            phy_reg_set(unit, port, 0xa43, 25, val);

                            phy_reg_set(unit, port, 7, 60, 0x0);

                            /* Force re-autonegotiation if AN is on */
                            phy_reg_get(unit, port, 0, 0, &val);
                            if (val & (1 << 12))
                            {
                                val |= (1 << 9);
                                phy_reg_set(unit, port, 0, 0, val);
                            }

                            phy_reg_set(unit, port, 0xa42, 29, 0x0);
                        }
                    }
              #endif
              #if defined(CONFIG_SDK_RTL8380)
                    if(HWP_8380_30_FAMILY(unit))
                    {
                        int phy_idx;
                        unsigned int value;
                        unsigned int val;

                        /* 10M EEE Amplitude */
                        for(phy_idx = 0; phy_idx < HWP_SWITCH_DESCP(unit)->phy.descp[i].phy_max; phy_idx++)
                        {
                            /* Force Copper */
                            phy_reg_set(unit, HWP_PHY_BASE_MACID_BY_IDX(unit, i)+phy_idx,0,30,0x0001);

                            /* 100/1000M EEE Capability */
                            phy_reg_set(unit, HWP_PHY_BASE_MACID_BY_IDX(unit, i)+phy_idx,0,13,0x0007);
                            phy_reg_set(unit, HWP_PHY_BASE_MACID_BY_IDX(unit, i)+phy_idx,0,14,0x003C);
                            phy_reg_set(unit, HWP_PHY_BASE_MACID_BY_IDX(unit, i)+phy_idx,0,13,0x4007);
                            phy_reg_set(unit, HWP_PHY_BASE_MACID_BY_IDX(unit, i)+phy_idx,0,14,0x0000);

                            phy_reg_get(unit, HWP_PHY_BASE_MACID_BY_IDX(unit, i)+phy_idx,0x0A43,25,&value);
                            value &= ~(1UL<<4);
                            phy_reg_set(unit, HWP_PHY_BASE_MACID_BY_IDX(unit, i)+phy_idx,0x0A43,25,value);

                            /* Force re-autonegotiation if AN is on */
                            phy_reg_get(unit, HWP_PHY_BASE_MACID_BY_IDX(unit, i)+phy_idx, 0, 0, &val);
                            if (val & (1 << 12))
                            {
                                phy_reg_set(unit, HWP_PHY_BASE_MACID_BY_IDX(unit, i)+phy_idx, 0, 0, (val | (1 << 9)));
                            }

                             /* Back to  AUTO */
                            phy_reg_set(unit, HWP_PHY_BASE_MACID_BY_IDX(unit, i)+phy_idx,0,30,0x0000);
                         }
                    }
              #endif
                }
                break;
            #endif
            default:
                osal_printf("PHY[%d]: not supported in EEE\n", i);
                break;
        }
    }
#if defined(CONFIG_SDK_RTL8390)

    pollSts = 0x1;
    if(HWP_8390_50_FAMILY(unit))
    {
        rtl8390_miim_globalPollingEnable_set(unit, pollSts);

        HWP_PORT_TRAVS(unit, i)
        {
            unsigned int val=0;

            reg_array_field_write(unit,
                                      CYPRESS_MAC_FORCE_MODE_CTRLr,
                                      i,
                                      REG_ARRAY_INDEX_NONE,
                                      CYPRESS_EEE_100M_ENf,
                                      &val);
            reg_array_field_write(unit,
                                      CYPRESS_MAC_FORCE_MODE_CTRLr,
                                      i,
                                      REG_ARRAY_INDEX_NONE,
                                      CYPRESS_EEE_500M_ENf,
                                      &val);
            reg_array_field_write(unit,
                                      CYPRESS_MAC_FORCE_MODE_CTRLr,
                                      i,
                                      REG_ARRAY_INDEX_NONE,
                                      CYPRESS_EEE_1000M_ENf,
                                      &val);
            reg_array_field_write(unit,
                                      CYPRESS_MAC_FORCE_MODE_CTRLr,
                                      i,
                                      REG_ARRAY_INDEX_NONE,
                                      CYPRESS_EEE_10G_ENf,
                                      &val);
            reg_array_field_write(unit,
                                      CYPRESS_MAC_FORCE_MODE_CTRLr,
                                      i,
                                      REG_ARRAY_INDEX_NONE,
                                      CYPRESS_EEE_TX_ENf,
                                      &val);
            reg_array_field_write(unit,
                                      CYPRESS_MAC_FORCE_MODE_CTRLr,
                                      i,
                                      REG_ARRAY_INDEX_NONE,
                                      CYPRESS_EEE_RX_ENf,
                                      &val);
        }
    }
#endif
#if defined(CONFIG_SDK_RTL8380)
    if(HWP_8380_30_FAMILY(unit))
    {
        /*Restore polling mask*/
        reg_write(unit, MAPLE_SMI_POLL_CTRLr, &polling_mask);
    }
#endif
}
#endif

#ifdef CONFIG_SDK_DRIVER_GPIO
/* Function Name:
 *      rtk_intGpio_pin_set
 * Description:
 *      Set the internal GPIO status of specific pin
 * Input:
 *      pinNum    - the pin number of internal GPIO.
 *      pinStatus - the status of specific pin.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_OUT_OF_RANGE
 *      RT_ERR_DRIVER_NOT_FOUND
 * Note:
 *      None
 */
int rtk_intGpio_pin_set(uint pinNum, uint pinStatus)
{
    drv_generalCtrlGpio_pinConf_t gpioConfig;
    uint32  unit = HWP_MY_UNIT_ID();

    drv_generalCtrlGPIO_dev_init(unit, GEN_GPIO_DEV_ID0_INTERNAL, 0);
    drv_generalCtrlGPIO_devEnable_set(unit, GEN_GPIO_DEV_ID0_INTERNAL, ENABLED);

    gpioConfig.direction = GPIO_DIR_OUT;
    gpioConfig.default_value = 1;
    gpioConfig.int_gpio.function = GPIO_CTRLFUNC_NORMAL;
    gpioConfig.int_gpio.interruptEnable = GPIO_INT_DISABLE;
    gpioConfig.ext_gpio.debounce = 0;
    gpioConfig.ext_gpio.inverter = 0;
    gpioConfig.ext_gpio.direction = 0;
    drv_generalCtrlGPIO_pin_init(unit,GEN_GPIO_DEV_ID0_INTERNAL,pinNum,&gpioConfig);


    return drv_generalCtrlGPIO_dataBit_set(unit, GEN_GPIO_DEV_ID0_INTERNAL, pinNum, pinStatus);
}
#endif /* CONFIG_SDK_DRIVER_GPIO */

#ifdef CONFIG_SDK_DRIVER_GPIO
/* Function Name:
 *      rtk_intGpio_pin_get
 * Description:
 *      Get the internal GPIO status of specific pin
 * Input:
 *      pinNum     - the pin number of internal GPIO.
 *      *pinStatus - the status of specific pin.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_OUT_OF_RANGE
 *      RT_ERR_DRIVER_NOT_FOUND
 * Note:
 *      None
 */
int rtk_intGpio_pin_get(uint pinNum, uint *pinStatus)
{

    drv_generalCtrlGpio_pinConf_t gpioConfig;
    uint32  unit = HWP_MY_UNIT_ID();


    drv_generalCtrlGPIO_dev_init(unit, GEN_GPIO_DEV_ID0_INTERNAL, 0);
    drv_generalCtrlGPIO_devEnable_set(unit, GEN_GPIO_DEV_ID0_INTERNAL, ENABLED);

    gpioConfig.direction = GPIO_DIR_IN;
    gpioConfig.default_value = 1;
    gpioConfig.int_gpio.function = GPIO_CTRLFUNC_NORMAL;
    gpioConfig.int_gpio.interruptEnable = GPIO_INT_DISABLE;
    gpioConfig.ext_gpio.debounce = 0;
    gpioConfig.ext_gpio.inverter = 0;
    gpioConfig.ext_gpio.direction = 0;
    drv_generalCtrlGPIO_pin_init(unit, GEN_GPIO_DEV_ID0_INTERNAL, pinNum, &gpioConfig);


    return drv_generalCtrlGPIO_dataBit_get(unit, GEN_GPIO_DEV_ID0_INTERNAL, pinNum, (uint32*)pinStatus);
}
#endif /* CONFIG_SDK_DRIVER_GPIO */

#ifdef CONFIG_SDK_DRIVER_GPIO

drv_generalCtrlGpio_devConf_t   g_gpioDevConfig;
int                             g_extGPIODev_init;

/* Function Name:
 *      rtk_extGpio_device_init
 * Description:
 *      Initial Device Conf structure for external GPIO
 * Input:
 *      unit              - Unit id
 *      deviceAddress     - RTL8231 MDC/MDIO address.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_OUT_OF_RANGE
 *      RT_ERR_DRIVER_NOT_FOUND
 * Note:
 *      Always use generalCtrl device ID 1 and MDC access mode.
 */
int rtk_extGpio_device_init(unsigned int unit, uint deviceAddress)
{
    int32   ret;

    osal_memset(&g_gpioDevConfig, 0, sizeof(drv_generalCtrlGpio_devConf_t));
    g_extGPIODev_init = 0;

    g_gpioDevConfig.ext_gpio.address = deviceAddress;
    g_gpioDevConfig.ext_gpio.access_mode= EXT_GPIO_ACCESS_MODE_MDC;

    if((ret = drv_generalCtrlGPIO_dev_init(unit, GEN_GPIO_DEV_ID1, &g_gpioDevConfig)) != RT_ERR_OK)
        return ret;

    ret = drv_generalCtrlGPIO_devEnable_set(unit, GEN_GPIO_DEV_ID1, ENABLED);

    if(ret == RT_ERR_OK)
        g_extGPIODev_init = 1;

    return RT_ERR_OK;
}


/* Function Name:
 *      rtk_extGpio_pin_set
 * Description:
 *      Set the external GPIO status of specific pin
 * Input:
 *      unit       - Unit id
 *      pinNum     - the pin number of internal GPIO.
 *      *pinStatus - the status of specific pin.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_OUT_OF_RANGE
 *      RT_ERR_NOT_INIT
 *      RT_ERR_DRIVER_NOT_FOUND
 * Note:
 *      None
 */
int rtk_extGpio_pin_set(unsigned int unit, uint pinNum, uint pinStatus)
{
    drv_generalCtrlGpio_pinConf_t gpioConfig;

    if(g_extGPIODev_init != 1)
        return RT_ERR_NOT_INIT;

    gpioConfig.direction = GPIO_DIR_OUT;
    gpioConfig.default_value = 1;
    gpioConfig.int_gpio.function = 0;
    gpioConfig.int_gpio.interruptEnable = 0;
    gpioConfig.ext_gpio.debounce = 0;
    gpioConfig.ext_gpio.inverter = 0;
    gpioConfig.ext_gpio.direction = GPIO_DIR_OUT;
    drv_generalCtrlGPIO_pin_init(unit,GEN_GPIO_DEV_ID1,pinNum,&gpioConfig);

    return drv_generalCtrlGPIO_dataBit_set(unit, GEN_GPIO_DEV_ID1, pinNum, pinStatus);
}

/* Function Name:
 *      rtk_extGpio_pin_get
 * Description:
 *      Get the external GPIO status of specific pin
 * Input:
 *      unit       - Unit id
 *      pinNum     - the pin number of internal GPIO.
 *      *pinStatus - the status of specific pin.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_OUT_OF_RANGE
 *      RT_ERR_NOT_INIT
 *      RT_ERR_DRIVER_NOT_FOUND
 * Note:
 *      None
 */
int rtk_extGpio_pin_get(unsigned int unit, uint pinNum, uint *pinStatus)
{

    drv_generalCtrlGpio_pinConf_t gpioConfig;

    if(g_extGPIODev_init != 1)
        return RT_ERR_NOT_INIT;

    gpioConfig.direction = GPIO_DIR_IN;
    gpioConfig.default_value = 1;
    gpioConfig.int_gpio.function = 0;
    gpioConfig.int_gpio.interruptEnable = 0;
    gpioConfig.ext_gpio.debounce = 0;
    gpioConfig.ext_gpio.inverter = 0;
    gpioConfig.ext_gpio.direction = GPIO_DIR_IN;
    drv_generalCtrlGPIO_pin_init(unit, GEN_GPIO_DEV_ID1, pinNum, &gpioConfig);

    return drv_generalCtrlGPIO_dataBit_get(unit, GEN_GPIO_DEV_ID1, pinNum, (uint32*)pinStatus);
}
#endif /* CONFIG_SDK_DRIVER_GPIO */

/* Function Name:
 *      rtk_swledtest_on
 * Description:
 *      Light on the led on specific port.
 * Input:
 *      port  - Port id
 *      index - Led index
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void rtk_swledtest_on(int port, int index)
{
    uint32  unit = uboot_cmd_unit;
    int portNum;
    unsigned int bit[RTK_MAX_PORT_PER_UNIT];
#if defined(CONFIG_SDK_RTL8380)

    unsigned int val_led_glb_ctrl = 0;
    unsigned int val_led_mode_sel = 0;
    unsigned int val_led_p_en_ctrl = 0;
    /*Backup led value*/
    if(HWP_8380_30_FAMILY(unit))
    {
        reg_read(unit, MAPLE_LED_GLB_CTRLr, &val_led_glb_ctrl);
        reg_read(unit, MAPLE_LED_MODE_SELr, &val_led_mode_sel);
        reg_read(unit, MAPLE_LED_P_EN_CTRLr, &val_led_p_en_ctrl);
    }
#endif

    if (NULL == HWP_SWITCH_DESCP(unit))
    {
        osal_printf("\n led test: Cant get board model\n");
        return;
    }

    if (!HWP_PORT_EXIST(unit, port))
    {
        osal_printf("The port (%d) is invalid.\n", port);
        return;
    }

    if ((index < 0) || (index > HWP_PORT_LED_NUM(unit, port)))
    {
        osal_printf("The led index (%d) is invalid.\n", index);
        return;
    }

    rtk_swled_init(unit);

    HWP_PORT_TRAVS(unit, portNum)
    {
        bit[portNum] = 0;
    }

    bit[port] = LEDON(unit);

    rtk_swled_out(unit, index, bit);

    osal_printf("\n press any to end.\n");
    getc();

    bit[port] = 0;
    rtk_swled_out(unit, index, bit);

#if defined(CONFIG_SDK_RTL8380)
    if(HWP_8380_30_FAMILY(unit))
    {
        reg_write(unit, MAPLE_LED_GLB_CTRLr, &val_led_glb_ctrl);
        reg_write(unit, MAPLE_LED_MODE_SELr, &val_led_mode_sel);
        reg_write(unit, MAPLE_LED_P_EN_CTRLr, &val_led_p_en_ctrl);
    }
#endif

    rtk_swled_uninit(unit);

    return;
}

/* Function Name:
 *      rtk_ledTest_run
 * Description:
 *      Run the led testing
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void rtk_ledTest_run(void)
{
    int portNum, index = 0;
    unsigned int bit[RTK_MAX_PORT_PER_UNIT];
    unsigned int port_index;
    uint32  unit = uboot_cmd_unit;

#if defined(CONFIG_SDK_RTL8380)

    unsigned int val_led_glb_ctrl = 0;
    unsigned int val_led_mode_sel = 0;
    unsigned int val_led_p_en_ctrl = 0;
    /*Backup led value*/
    if(HWP_8380_30_FAMILY(unit))
    {
        reg_read(unit, MAPLE_LED_GLB_CTRLr, &val_led_glb_ctrl);
        reg_read(unit, MAPLE_LED_MODE_SELr, &val_led_mode_sel);
        reg_read(unit, MAPLE_LED_P_EN_CTRLr, &val_led_p_en_ctrl);
    }
#endif
    rtk_swled_init(unit);

    osal_printf("\n all led on \n");

    HWP_PORT_TRAVS_EXCEPT_CPU(unit, portNum)
    {
        bit[portNum] = LEDON(unit);
    }
    rtk_swled_out(unit, index, bit);

    osal_time_udelay(200000);
    osal_printf("\n all led off \n");
    HWP_PORT_TRAVS_EXCEPT_CPU(unit, portNum)
    {
        bit[portNum] = 0;
    }
    rtk_swled_out(unit, index, bit);

    osal_printf("\n walking one \n");

    HWP_PORT_TRAVS(unit, port_index)
    {
        for (index = 0; index < HWP_PORT_LED_NUM(unit, port_index); index++)
        {
            HWP_PORT_TRAVS(unit, portNum)
            {
                bit[portNum] = 0;
            }
            bit[port_index-1] = LEDON(unit);
            rtk_swled_out(unit, index, bit);
        }
    }

    osal_printf("\n all led off \n");

    HWP_PORT_TRAVS_EXCEPT_CPU(unit, portNum)
    {
        bit[portNum] = 0;
    }
    rtk_swled_out(unit, index, bit);

#if defined(CONFIG_SDK_RTL8380)
    if(HWP_8380_30_FAMILY(unit))
    {
        reg_write(unit, MAPLE_LED_GLB_CTRLr, &val_led_glb_ctrl);
        reg_write(unit, MAPLE_LED_MODE_SELr, &val_led_mode_sel);
        reg_write(unit, MAPLE_LED_P_EN_CTRLr, &val_led_p_en_ctrl);
    }
#endif

    rtk_swled_uninit(unit);

    return;
}

/* Function Name:
 *      rtk_swled_init
 * Description:
 *      initialize the software control led.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void rtk_swled_init(unsigned int unit)
{
#if defined(CONFIG_SDK_RTL9310)
        {
            int port;
            unsigned int value = 0;

            HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
            {
                reg_array_field_read(unit, MANGO_LED_PORT_SW_EN_CTRLr, port,
                    REG_ARRAY_INDEX_NONE, MANGO_SW_CTRL_LED_ENf, &value);
                value = 0x7;
                reg_array_field_write(unit, MANGO_LED_PORT_SW_EN_CTRLr, port,
                    REG_ARRAY_INDEX_NONE, MANGO_SW_CTRL_LED_ENf, &value);
            }
        }
#endif
#if defined(CONFIG_SDK_RTL9300)
        {
            int port;
            unsigned int value = 0;

            HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
            {
                reg_array_field_read(unit, LONGAN_LED_PORT_SW_EN_CTRLr, port,
                    REG_ARRAY_INDEX_NONE, LONGAN_SW_CTRL_LED_ENf, &value);
                value = 0x7;
                reg_array_field_write(unit, LONGAN_LED_PORT_SW_EN_CTRLr, port,
                    REG_ARRAY_INDEX_NONE, LONGAN_SW_CTRL_LED_ENf, &value);
            }
        }
#endif

#if defined(CONFIG_SDK_RTL8390)
    {
        int port;
        unsigned int value = 0;

        HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
        {
            reg_array_field_read(unit, CYPRESS_LED_SW_P_EN_CTRLr, port,
                REG_ARRAY_INDEX_NONE, CYPRESS_SW_CTRL_LED_ENf, &value);
            value = 0x7;
            reg_array_field_write(unit, CYPRESS_LED_SW_P_EN_CTRLr, port,
                REG_ARRAY_INDEX_NONE, CYPRESS_SW_CTRL_LED_ENf, &value);
        }
    }
#endif
#if defined(CONFIG_SDK_RTL8380)
    {
        unsigned int value = 0;

        reg_read(unit, MAPLE_LED_GLB_CTRLr, &value);
        value &= 0xFFFFFE00;
        value |= 0x1B;
        reg_write(unit, MAPLE_LED_GLB_CTRLr, &value);

        value |= 0xFFFFFFFF;
        reg_write(unit, MAPLE_LED_P_EN_CTRLr, &value);
        reg_write(unit, MAPLE_LED_SW_CTRLr, &value);
        reg_write(unit, MAPLE_LED0_SW_P_EN_CTRLr, &value);
        reg_write(unit, MAPLE_LED1_SW_P_EN_CTRLr, &value);
        value &= 0;
        reg_write(unit, MAPLE_LED2_SW_P_EN_CTRLr, 0);
    }
#endif
    return;

}

/* Function Name:
 *      rtk_swled_out
 * Description:
 *      show out the software control led.
 * Input:
 *      unit  - unit id
 *      index - led index
 *      bit   - led status
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void rtk_swled_out(unsigned int unit, int index, unsigned int *bit)
{
    uint32 port;
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL9310)
    uint32 value;
    value = 1;
#endif

    HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
    {
#if defined(CONFIG_SDK_RTL8380)
        if(HWP_8380_30_FAMILY(unit))
        {
            uint32 field;

            if(0 == bit[port])
            {
                reg_array_write(unit, MAPLE_LED_SW_P_CTRLr, port, REG_ARRAY_INDEX_NONE, &bit[port]);
            }
            else
            {
                switch (index)
                {
                    case 0:
                        field = MAPLE_SW_P_LED0_MODEf;
                        break;
                    case 1:
                        field = MAPLE_SW_P_LED1_MODEf;
                        break;
                    case 2:
                        field = MAPLE_SW_P_LED2_MODEf;
                        break;
                    default:
                        return;
                }
                reg_array_field_write(unit, MAPLE_LED_SW_P_CTRLr, port,
                                        REG_ARRAY_INDEX_NONE, field, &bit[port]);
            }

        }
        __asm__ volatile ("sync");
#endif
#if defined(CONFIG_SDK_RTL8390)


        if(HWP_8390_50_FAMILY(unit))
        {
            uint32 field;

            if(0 == bit[port])
            {
                reg_array_write(unit, CYPRESS_LED_SW_P_CTRLr, port, REG_ARRAY_INDEX_NONE, &bit[port]);
            }
            else
            {
                if(HWP_LED_LAYOUT(unit, port) == SINGLE_SET)
                {
                    switch (index)
                    {
                        case 0:
                            field = CYPRESS_SW_COPR_LED0_MODEf;
                            break;
                        case 1:
                            field = CYPRESS_SW_COPR_LED1_MODEf;
                            break;
                        case 2:
                            field = CYPRESS_SW_COPR_LED2_MODEf;
                            break;
                        default:
                            return;
                    }
                }
                else
                {
                    switch (index)
                    {
                        case 0:
                            field = CYPRESS_SW_FIB_LED0_MODEf;
                            break;
                        case 1:
                            field = CYPRESS_SW_FIB_LED1_MODEf;
                            break;
                        case 2:
                            field = CYPRESS_SW_FIB_LED2_MODEf;
                            break;
                        default:
                            return;
                    }
                }
                reg_array_field_write(unit, CYPRESS_LED_SW_P_CTRLr, port,
                                        REG_ARRAY_INDEX_NONE, field, &bit[port]);
            }
            __asm__ volatile ("sync");
        }
#endif

#if defined(CONFIG_SDK_RTL9310)


            if(HWP_9310_FAMILY_ID(unit))
            {
                uint32 field;

                if(0 == bit[port])
                {
                    reg_array_write(unit, MANGO_LED_PORT_SW_EN_CTRLr, port, REG_ARRAY_INDEX_NONE, &bit[port]);
                }
                else
                {
                    if(HWP_LED_LAYOUT(unit, port) == SINGLE_SET)
                    {
                        switch (index)
                        {
                            case 0:
                                field = MANGO_SW_COPR_LED0_MODEf;
                                break;
                            case 1:
                                field = MANGO_SW_COPR_LED1_MODEf;
                                break;
                            case 2:
                                field = MANGO_SW_COPR_LED2_MODEf;
                                break;
                            case 3:
                                field = MANGO_SW_COPR_LED3_MODEf;
                                break;
                            default:
                                return;
                        }
                    }
                    else
                    {
                        switch (index)
                        {
                            case 0:
                                field = MANGO_SW_FIB_LED0_MODEf;
                                break;
                            case 1:
                                field = MANGO_SW_FIB_LED1_MODEf;
                                break;
                            case 2:
                                field = MANGO_SW_FIB_LED2_MODEf;
                                break;
                            case 3:
                                field = MANGO_SW_FIB_LED3_MODEf;
                                break;
                            default:
                                return;
                        }
                    }
                    reg_array_field_write(unit, MANGO_LED_PORT_SW_CTRLr, port,
                                            REG_ARRAY_INDEX_NONE, field, &bit[port]);
                }
                __asm__ volatile ("sync");
            }
#endif

#if defined(CONFIG_SDK_RTL9300)

                if(HWP_9300_FAMILY_ID(unit))
                {
                    uint32 field;

                    if(0 == bit[port])
                    {
                        reg_array_write(unit, LONGAN_LED_PORT_SW_EN_CTRLr, port, REG_ARRAY_INDEX_NONE, &bit[port]);
                    }
                    else
                    {
                        if(HWP_LED_LAYOUT(unit, port) == SINGLE_SET)
                        {
                            switch (index)
                            {
                                case 0:
                                    field = LONGAN_SW_COPR_LED0_MODEf;
                                    break;
                                case 1:
                                    field = LONGAN_SW_COPR_LED1_MODEf;
                                    break;
                                case 2:
                                    field = LONGAN_SW_COPR_LED2_MODEf;
                                    break;
                                case 3:
                                    field = LONGAN_SW_COPR_LED3_MODEf;
                                    break;
                                default:
                                    return;
                            }
                        }
                        else
                        {
                            switch (index)
                            {
                                case 0:
                                    field = LONGAN_SW_FIB_LED0_MODEf;
                                    break;
                                case 1:
                                    field = LONGAN_SW_FIB_LED1_MODEf;
                                    break;
                                case 2:
                                    field = LONGAN_SW_FIB_LED2_MODEf;
                                    break;
                                case 3:
                                    field = LONGAN_SW_FIB_LED3_MODEf;
                                    break;
                                default:
                                    return;
                            }
                        }
                        reg_array_field_write(unit, LONGAN_LED_PORT_SW_CTRLr, port,
                                                REG_ARRAY_INDEX_NONE, field, &bit[port]);
                    }
                    __asm__ volatile ("sync");
                }
#endif

    }

#if defined(CONFIG_SDK_RTL8390)
    value = 1;
    reg_write(unit, CYPRESS_LED_SW_CTRLr, &value);
    __asm__ volatile ("sync");
#endif
#if defined(CONFIG_SDK_RTL9310)
    value = 1;
    reg_write(unit, MANGO_SW_LED_LOADr, &value);
    __asm__ volatile ("sync");
#endif


    osal_time_udelay(300000);
    return;
}

/* Function Name:
 *      rtk_swled_uninit
 * Description:
 *      uninitialize the software control led.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void rtk_swled_uninit(unsigned int unit)
{
#if defined(CONFIG_SDK_RTL8380)
    reg_write(unit, MAPLE_LED_SW_CTRLr, 0);
    reg_write(unit, MAPLE_LED0_SW_P_EN_CTRLr, 0);
    reg_write(unit, MAPLE_LED1_SW_P_EN_CTRLr, 0);
    reg_write(unit, MAPLE_LED2_SW_P_EN_CTRLr, 0);
#endif

#if defined(CONFIG_SDK_RTL8390)
    int port;
    unsigned int value;

    HWP_PORT_TRAVS(unit, port)
    {
        reg_array_field_read(unit, CYPRESS_LED_SW_P_EN_CTRLr, port,
            REG_ARRAY_INDEX_NONE, CYPRESS_SW_CTRL_LED_ENf, &value);
        value = 0;
        reg_array_field_write(unit, CYPRESS_LED_SW_P_EN_CTRLr, port,
            REG_ARRAY_INDEX_NONE, CYPRESS_SW_CTRL_LED_ENf, &value);
    }
  #ifdef CONFIG_CUSTOMER_BOARD
    customer_led_config_init(gSwitchModel);
  #endif

#endif
#if defined(CONFIG_SDK_RTL9310)
    int port;
    unsigned int value;

    HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
    {
        reg_array_field_read(unit, MANGO_LED_PORT_SW_EN_CTRLr, port,
            REG_ARRAY_INDEX_NONE, MANGO_SW_CTRL_LED_ENf, &value);
        value = 0x0;
        reg_array_field_write(unit, MANGO_LED_PORT_SW_EN_CTRLr, port,
            REG_ARRAY_INDEX_NONE, MANGO_SW_CTRL_LED_ENf, &value);
    }
  #ifdef CONFIG_CUSTOMER_BOARD
    customer_led_config_init(gSwitchModel);
  #endif

#endif
#if defined(CONFIG_SDK_RTL9300)
    int port;
    unsigned int value;

    HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
    {
        reg_array_field_read(unit, LONGAN_LED_PORT_SW_EN_CTRLr, port,
            REG_ARRAY_INDEX_NONE, LONGAN_SW_CTRL_LED_ENf, &value);
        value = 0x0;
        reg_array_field_write(unit, LONGAN_LED_PORT_SW_EN_CTRLr, port,
            REG_ARRAY_INDEX_NONE, LONGAN_SW_CTRL_LED_ENf, &value);
    }
  #ifdef CONFIG_CUSTOMER_BOARD
    customer_led_config_init(gSwitchModel);
  #endif

#endif


}

/* Function Name:
 *      rtk_sfp_speed_set
 * Description:
 *      Configure SFP port speed
 * Input:
 *      port  - port number (mac id)
 *      speed - 100 or 1000
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void rtk_sfp_speed_set(int port, int speed)
{
    uint32  unit = uboot_cmd_unit;

    if (unit >= RTK_MAX_NUM_OF_UNIT_LOCAL)
    {
        osal_printf("\nUnit %u invalid unit id \n", unit);
        return;
    }

    if (port > 2)
    {
        osal_printf("Error port number: %d.\n\n", port);
        return;
    }

    if (speed != 100 && speed != 1000)
    {
        osal_printf("Error speed: %d.\n\n", speed);
        return;
    }

#if defined(CONFIG_SDK_RTL8390)
    if(HWP_8390_50_FAMILY(unit))
    {
        rtl8390_sfp_speed_set(unit, port, speed);
    }
#endif/* defined(CONFIG_SDK_RTL8390) */

    return;
}

/* Function Name:
 *      rtk_phyreg_get
 * Description:
 *      Read specific PHY register value
 * Input:
 *      argv[3] - specific port id
 *      argv[4] - page number
 *      argv[5] - register index
 * Output:
 *      None
 * Return:
 *      0
 * Note:
 *      None
 */
int rtk_phyreg_get(char * const argv[])
{
    uint32 unit = uboot_cmd_unit;
    int portid = simple_strtoul(argv[3], NULL, 10);
    int page = simple_strtoul(argv[4], NULL, 10);
    int reg = simple_strtoul(argv[5], NULL, 10);
    unsigned int val;
    int32   rv;

    osal_printf("Get Unit %u Port %02d page %02d reg %02d", unit, portid, page, reg);
    if (unit >= RTK_MAX_NUM_OF_UNIT_LOCAL)
    {
        osal_printf("\nUnit %u invalid unit id \n", unit);
        return 0;
    }

    if ((rv = phy_construct_reg_park_get(unit, portid, page, 0x1F, reg, &val)) == RT_ERR_OK)
        osal_printf("  val: 0x%04X \n", val);
    else
        osal_printf("    Failed!! 0x%x \n", (uint32)rv);

    return 0;
}

/* Function Name:
 *      rtk_phyreg_set
 * Description:
 *      Write data to specific PHY register
 * Input:
 *      argv[3] - specific port id
 *      argv[4] - page number
 *      argv[5] - register index
 *      argv[6] - write data value
 * Output:
 *      None
 * Return:
 *      0
 * Note:
 *      None
 */
int rtk_phyreg_set(char * const argv[])
{
    uint32 unit = uboot_cmd_unit;
    int portid = simple_strtoul(argv[3], NULL, 10);
    int page = simple_strtoul(argv[4], NULL, 10);
    int reg = simple_strtoul(argv[5], NULL, 10);
    int val = simple_strtoul(argv[6], NULL, 10);
    int32   rv;

    osal_printf("Set Unit %u Port %02d page %02d reg %02d val: 0x%04X", unit, portid, page, reg, val);
    if (unit >= RTK_MAX_NUM_OF_UNIT_LOCAL)
    {
        osal_printf("\nUnit %u invalid unit id \n", unit);
        return 0;
    }

    if ((rv = phy_construct_reg_park_set(unit, portid, page, 0x1F, reg, val)) == RT_ERR_OK)
        osal_printf(" \n");
    else
        osal_printf("    Failed!! 0x%x\n", (uint32)rv);

    return 0;
}

/* Function Name:
 *      rtk_phymmd_get
 * Description:
 *      Read specific PHY MMD register value
 * Input:
 *      uint   - unit id
 *      portid - specific port id
 *      addr   - address number
 *      reg    - register index
 * Output:
 *      None
 * Return:
 *      0
 * Note:
 *      None
 */
int rtk_phymmd_get(unsigned int unit, unsigned int portid, unsigned int addr, unsigned int reg)
{
    unsigned int val;
    int32   rv;

    osal_printf("Get Unit %u Port %02d mmd-address %02d mmd-reg %02d", unit, portid, addr, reg);
    if (unit >= RTK_MAX_NUM_OF_UNIT_LOCAL)
    {
        osal_printf("\nUnit %u invalid unit id \n", unit);
        return 0;
    }

    if ((rv = phy_reg_mmd_get(unit, portid, addr, reg, &val)) == RT_ERR_OK)
        osal_printf("  val: 0x%04X \n", val);
    else
        osal_printf("    Failed!! 0x%x \n", (uint32)rv);

    return 0;
}

/* Function Name:
 *      rtk_phymmd_set
 * Description:
 *      Write data to specific PHY MMD register
 * Input:
 *      uint   - unit id
 *      portid - specific port id
 *      addr   - address number
 *      reg    - register index
 *      val    - write data value
 * Output:
 *      None
 * Return:
 *      0
 * Note:
 *      None
 */
int rtk_phymmd_set(unsigned int unit, unsigned int portid, unsigned int addr, unsigned int reg, unsigned int val)
{
    int32       rv;

    osal_printf("Set Unit %u Port %02u mmd-address %02u mmd-reg %02u val: 0x%04X", unit, portid, addr, reg, val);
    if (unit >= RTK_MAX_NUM_OF_UNIT_LOCAL)
    {
        osal_printf("\nUnit %u invalid unit id \n", unit);
        return 0;
    }

    if ((rv = phy_reg_mmd_set(unit, portid, addr, reg, val)) == RT_ERR_OK)
        osal_printf(" \n");
    else
        osal_printf("    Failed!! 0x%x\n", (uint32)rv);

    return 0;
}

/* Function Name:
 *      rtk_sdsreg_get
 * Description:
 *      Read specific Serdes register value
 * Input:
 *      unit - unit id
 *      sdsId - specific serdes id
 *      page - page number
 *      reg - register index
 * Output:
 *      None
 * Return:
 *      0
 * Note:
 *      None
 */
int rtk_sdsreg_get(unsigned int unit, unsigned int sdsId, unsigned int page, unsigned int reg)
{
    uint32  val;
    int32   rv;

    osal_printf("Get unit %u sds %02d page %02d reg %02d", unit, sdsId, page, reg);
    if (unit >= RTK_MAX_NUM_OF_UNIT_LOCAL)
    {
        osal_printf("\nUnit %u invalid unit id \n", unit);
        return 0;
    }

    if ((rv = hal_serdes_reg_get(unit, sdsId, page,reg, &val)) == RT_ERR_OK)
        osal_printf("  val: 0x%04X \n", val);
    else
        osal_printf("    Failed!! 0x%x \n", (uint32)rv);

    return 0;
}

/* Function Name:
 *      rtk_sdsreg_set
 * Description:
 *      Write data to specific Serdes register
 * Input:
 *      unit - unit id
 *      sdsId - specific serdes id
 *      page - page number
 *      reg - register index
 *      val - write data value
 * Output:
 *      None
 * Return:
 *      0
 * Note:
 *      None
 */
int rtk_sdsreg_set(unsigned int unit, unsigned int sdsId, unsigned int page, unsigned int reg, unsigned int val)
{
    int32   rv;

    osal_printf("Set unit %u sds %02d page %02d reg %02d val: 0x%04X", unit, sdsId, page, reg, val);
    if (unit >= RTK_MAX_NUM_OF_UNIT_LOCAL)
    {
        osal_printf("\nUnit %u invalid unit id \n", unit);
        return 0;
    }

    if ((rv = hal_serdes_reg_set(unit, sdsId, page, reg, val)) == RT_ERR_OK)
        osal_printf(" \n");
    else
        osal_printf("    Failed!! 0x%x\n", (uint32)rv);

    return 0;
}

/* Function Name:
 *      rtk_phyreg_portmask_set
 * Description:
 *      Write data to specific PHYs register by port mask.
 * Input:
 *      argv[3] - specific port mask
 *      argv[4] - page number
 *      argv[5] - register index
 *      argv[6] - write data value
 * Output:
 *      None
 * Return:
 *      0
 * Note:
 *      None
 */
int rtk_phyreg_portmask_set(char * const argv[])
{
    uint32 unit = uboot_cmd_unit;
    unsigned long long port_mask = simple_strtoul(argv[3], NULL, 10);
    int page = simple_strtoul(argv[4], NULL, 10);
    int reg = simple_strtoul(argv[5], NULL, 10);
    int val = simple_strtoul(argv[6], NULL, 10);
    rtk_portmask_t portMask;
    unsigned long long      bit;
    int                     port;

    if (unit >= RTK_MAX_NUM_OF_UNIT_LOCAL)
    {
        osal_printf("\nUnit %u invalid unit id \n", unit);
        return 0;
    }

    RTK_PORTMASK_RESET(portMask);
    for (port=0; port<sizeof(port_mask)*8; port++)
    {
        bit = (unsigned long long)(0x1) << port;
        if (!(bit & port_mask))
            continue;

        if (!HWP_PORT_EXIST(unit, port))
            continue;

        RTK_PORTMASK_PORT_SET(portMask, port);
    }

    phy_reg_portmask_set(unit, portMask, page, reg, val);
    osal_printf("Set Unit %u PortMask %08x page %02d reg %02d val: 0x%04X \n", unit, (unsigned int)port_mask, page, reg, val);

    return 0;
}

/* Function Name:
 *      rtk_phyreg_dump_top
 * Description:
 *      Dump Top information of specific PHY.
 * Input:
 *      argv[3] - specific port id
 * Output:
 *      None
 * Return:
 *      0
 * Note:
 *      For RTL8214FC only.
 */
int rtk_phyreg_dump_top(char * const argv[])
{
#ifdef CONFIG_SDK_RTL8214FC
    uint32 unit = uboot_cmd_unit;
    int portid = simple_strtoul(argv[3], NULL, 10);

    if (unit >= RTK_MAX_NUM_OF_UNIT_LOCAL)
    {
        osal_printf("\nUnit %u invalid unit id \n", unit);
        return 0;
    }

    phy_8214fc_dumpTop(unit, portid);
#endif

    return 0;
}

/* Function Name:
 *      rtk_sysEsd_set
 * Description:
 *      Configure system ESD
 * Input:
 *      state - 1: enable; 0: disable
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void rtk_sysEsd_set(int state)
{
    uint32          unit = uboot_cmd_unit;
    unsigned int    romId, val;
    int             baseMacId, macId;
    int             stsVal, setSysEsd;

    if (1 == state)
        stsVal = 0x7;
    else
        stsVal = 0x0;

    HWP_PORT_TRAVS(unit, macId)
    {
        if (!HWP_PHY_EXIST(unit, macId))
            continue;

        setSysEsd = FALSE;
        switch (HWP_PHY_MODEL_BY_PORT(unit,macId))
        {
#if (defined(CONFIG_SDK_RTL8218B))
            case RTK_PHYTYPE_RTL8208L_INT:
            case RTK_PHYTYPE_RTL8218B:
                setSysEsd = TRUE;
                break;
#endif
#if (defined(CONFIG_SDK_RTL8218FB))
            case RTK_PHYTYPE_RTL8218FB:
                setSysEsd = TRUE;
                break;
#endif
#if (defined(CONFIG_SDK_RTL8214FC))
            case RTK_PHYTYPE_RTL8214FC:
                setSysEsd = TRUE;
                break;
#endif
            default:
                break;
        }/* end switch */

        if (setSysEsd == FALSE)
            continue;

        baseMacId = HWP_PHY_BASE_MACID(unit,macId);

        phy_reg_set(unit, baseMacId, HAL_MIIM_FIX_PAGE, 27, 0x0004);
        phy_reg_get(unit, baseMacId, HAL_MIIM_FIX_PAGE, 28, &romId);
        if (romId > 2)
        {
            osal_printf("invalid rom id %u\n", romId);
            return;
        }

        /* patch request */
        phy_reg_get(unit, macId, 0xb82, 16, &val);
        val |= (1 << 4);
        phy_reg_set(unit, macId, 0xb82, 16, val);

        /* polling patch ready */
        do
        {
           phy_reg_get(unit, macId, 0xb80, 16, &val);
        } while ((val & 0x40) == 0);

        phy_reg_get(unit, macId, 0xa00, 16, &val);
        val &= ~(0xF << 12);
        val |= (stsVal << 12);
        phy_reg_set(unit, macId, 0xa00, 16, val);

        /* patch release */
        phy_reg_get(unit, macId, 0xb82, 16, &val);
        val &= ~(1 << 4);
        phy_reg_set(unit, macId, 0xb82, 16, val);
    }

    return;
}

/* Function Name:
 *      rtk_l2testmode_on
 * Description:
 *      Power on all ports without CPU port.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      RTL838x/3xM test command only.
 */
void rtk_l2testmode_on(void)
{
    uint32 unit = uboot_cmd_unit;

    if(HWP_8380_30_FAMILY(unit))
    {
        /*Enable all ports, PHY power up*/
        rtk_network_on();

        osal_printf("Force port28 link down\n\n");
        /*port 28 force link down*/
        ioal_mem32_write(unit, 0xa174, 0x6192d);
    }
}

/* Function Name:
 *      rtk_l2testmode_off
 * Description:
 *      Power off all ports.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      RTL838x/3xM test command only.
 */
void rtk_l2testmode_off(void)
{
    uint32 unit = uboot_cmd_unit;

    if(HWP_8380_30_FAMILY(unit))
    {
        /*Disable all ports, PHY power down*/
        rtk_network_off();
    }
}

#if defined(CONFIG_SDK_RTL8390)
/* Function Name:
 *      rtk_parameter_version_show
 * Description:
 *      Display command of each version for reference
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void rtk_parameter_version_show(void)
{
    osal_printf("V1.1\n");
    osal_printf("V1.2\n");
    osal_printf("update SYS ESD\n");
    osal_printf("update green in short cable\n");
    osal_printf("add SYS ESD command (rtk sys-esd <on|off>)\n");
    osal_printf("add fiber command:\n");
    osal_printf("    rtk fiber down-speed [enable | disable]\n");
    osal_printf("    rtk fiber nway [enable | disable] speed [100 | 1000 | auto]\n");
    osal_printf("    rtk fiber nway-force-link [enable | disable]\n");
    osal_printf("    rtk fiber get speed\n");
    osal_printf("    rtk fiber port [port] loopback [enable | disable]\n");
    osal_printf("V1.3\n");
    osal_printf("add 10g serdes and command (rtk 10g PORT [10gFiber | 1gFiber])\n");
    osal_printf("add 10g serdes restart and command (rtk 10g PORT restart)\n");
    osal_printf("add 10g serdes init and command (rtk 10g PORT init)\n");
    osal_printf("V1.4\n");
    osal_printf("add 10g serdes and command (rtk 10g UNIT PORT [10gFiber | 1gFiber])\n");
    osal_printf("add 10g serdes restart and command (rtk 10g UNIT PORT restart)\n");
    osal_printf("add 10g serdes init and command (rtk 10g UNIT PORT init)\n");
}
#endif/* CONFIG_SDK_RTL8390 */

/* Function Name:
 *      rtk_phy_testmode
 * Description:
 *      Enanle specific port testmode
 * Input:
 *      argv[2] - test mode
 *      argv[3] - specific port id
 *      argv[4] - test chn
 * Output:
 *      None
 * Return:
 *      0 - specific port test
 *      1 - all port test
 * Note:
 *      None
 */
int rtk_phy_testmode(char * const argv[])
{
    unsigned int unit = uboot_cmd_unit;
    unsigned int mode = simple_strtoul(argv[2], NULL, 10);
    unsigned int port = simple_strtoul(argv[3], NULL, 10);
    unsigned int chn = simple_strtoul(argv[4], NULL, 10);
    unsigned int ret = 0;
    rtk_port_phyTestMode_t TestMode;

    printf("test unit: %u\n", unit);
    printf("test mode: %u\n", mode);
    printf("test port: %u\n", port);

    if (unit >= RTK_MAX_NUM_OF_UNIT_LOCAL)
    {
        osal_printf("\nUnit %u invalid unit id \n", unit);
        return 0;
    }

    TestMode.channel = chn;
    TestMode.mode = mode;
    TestMode.flags = 0;

    if (0 == strcmp(argv[3], "all"))
    {
        TestMode.flags = 1;
    }
    if (RT_ERR_OK != (ret = rtk_port_phyIeeeTestMode_set(unit, port, &TestMode)))
        printf("PHY test mode failed ret = %d\n", ret);
    return ret;
}

#if defined(CONFIG_SDK_DRIVER_I2C)

/* Function Name:
 *      rtk_i2c_sw_init
 * Description:
 *      Initial I2C device for handle I2C interface
 * Input:
 *      i2c_dev      - I2C controller device
 *      clk_pin_dev  - specific GPIO device id of CLK pin
 *      clk_pin      - CLK pin id of specific GPIO device
 *      data_pin_dev - specific GPIO device id of DATA pin
 *      data_pin     - DATA pin id of specific GPIO device
 *      *typeStr     - "8" : 8 bits device ; "16" : 16 bits device
 *      chipid       - controlled I2C chip ID
 *      delay        - clock delay time
 *      rtl8231_addr - for ext-gpio used
 *      read_type    - 0 : Randon Read / 1 : Sequential Read
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      I2C interface is simulated by GPIO.
 */
void rtk_i2c_sw_init(unsigned int i2c_dev, unsigned int clk_pin_dev, unsigned int clk_pin, unsigned int data_pin_dev, unsigned int data_pin, char *typeStr, unsigned int chipid, unsigned int delay, unsigned int rtl8231_addr, unsigned int read_type)
{
    uint32           unit = HWP_MY_UNIT_ID();
    i2c_devConf_t    i2cDevice;
    uint32           i2c_type;
    int32            ret;
    drv_generalCtrlGpio_pinConf_t gpioConfig;
    drv_generalCtrlGpio_devConf_t gpioDev;

    if (strcmp(typeStr, "8") == 0)
        i2c_type = I2C_ADDR_WIDTH_BYTE_1;
    else if (strcmp(typeStr, "16") == 0)
        i2c_type = I2C_ADDR_WIDTH_BYTE_2;
    else
    {
        osal_printf("type mode %s is invalid.\n", typeStr);
        return;
    }

    osal_memset(&i2cDevice, 0, sizeof(i2c_devConf_t));
    i2cDevice.device_id = i2c_dev;
    i2cDevice.mem_addr_width = i2c_type;
    i2cDevice.data_width = i2c_type;
    i2cDevice.sda_dev = data_pin_dev;
    i2cDevice.sda_pin_id = data_pin;
    i2cDevice.dev_addr = chipid;
    i2cDevice.scl_dev = clk_pin_dev;
    i2cDevice.scl_pin_id = clk_pin;
    i2cDevice.scl_delay = delay;
    i2cDevice.read_type = read_type;
    i2cDevice.i2c_interface_id = I2C_INTF_SOFTWARE_DRV_ID;

    gpioDev.default_value = 1;
    gpioDev.ext_gpio.access_mode = EXT_GPIO_ACCESS_MODE_MDC;
    gpioDev.ext_gpio.address = rtl8231_addr;
    if ((ret = drv_generalCtrlGPIO_dev_init(unit, clk_pin_dev, &gpioDev)) != RT_ERR_OK)
        osal_printf("CLK GPIO device init failed(%d)!\n", ret);
    if ((ret = drv_generalCtrlGPIO_devEnable_set(unit, clk_pin_dev, ENABLED)) != RT_ERR_OK)
        osal_printf("CLK GPIO device init failed(%d)!\n", ret);
    gpioConfig.direction = GPIO_DIR_OUT;
    gpioConfig.default_value = 1;
    gpioConfig.int_gpio.function = GPIO_CTRLFUNC_NORMAL;
    gpioConfig.int_gpio.interruptEnable = GPIO_INT_DISABLE;
    gpioConfig.ext_gpio.debounce = 0;
    gpioConfig.ext_gpio.inverter = 0;
    if ((ret = drv_generalCtrlGPIO_pin_init(unit, clk_pin_dev, clk_pin, &gpioConfig)) != RT_ERR_OK)
        osal_printf("PIN init failed(%d)!\n", ret);

    gpioDev.default_value = 1;
    gpioDev.ext_gpio.access_mode = EXT_GPIO_ACCESS_MODE_MDC;
    gpioDev.ext_gpio.address = rtl8231_addr;
    if ((ret = drv_generalCtrlGPIO_dev_init(unit, data_pin_dev, &gpioDev)) != RT_ERR_OK)
        osal_printf("CLK GPIO device init failed(%d)!\n", ret);
    if ((ret = drv_generalCtrlGPIO_devEnable_set(unit, data_pin_dev, ENABLED)) != RT_ERR_OK)
        osal_printf("CLK GPIO device init failed(%d)!\n", ret);
    gpioConfig.direction = GPIO_DIR_OUT;
    gpioConfig.default_value = 1;
    gpioConfig.int_gpio.function = GPIO_CTRLFUNC_NORMAL;
    gpioConfig.int_gpio.interruptEnable = GPIO_INT_DISABLE;
    gpioConfig.ext_gpio.debounce = 0;
    gpioConfig.ext_gpio.inverter = 0;
    if ((ret = drv_generalCtrlGPIO_pin_init(unit, data_pin_dev, data_pin, &gpioConfig)) != RT_ERR_OK)
        osal_printf("PIN init failed(%d)!\n", ret);

    if ((ret = drv_i2c_init(unit)) != RT_ERR_OK)
        osal_printf("Initial fail (%d)!\n", ret);

    if ((ret = drv_i2c_dev_init(unit, &i2cDevice)) != RT_ERR_OK)
        osal_printf("Device Initial fail (%d)!\n", ret);

}

/* Function Name:
 *      rtk_i2c_hw_init
 * Description:
 *      Initial I2C device for handle I2C interface
 * Input:
 *      i2c_dev      - I2C controller device
 *      clk_pin_dev  - specific GPIO device id of CLK pin
 *      clk_pin      - CLK pin id of specific GPIO device
 *      data_pin_dev - specific GPIO device id of DATA pin
 *      data_pin     - DATA pin id of specific GPIO device
 *      *typeStr     - "8" : 8 bits device ; "16" : 16 bits device
 *      chipid       - controlled I2C chip ID
 *      freq         - bus frequency
 *      intf_id      - controller ID
 *      read_type    - 0 : Randon Read / 1 : Sequential Read
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      I2C interface is HW I2C master controller.
 */
void rtk_i2c_hw_init(unsigned int i2c_dev, unsigned int data_pin, char *typeStr, unsigned int chipid, unsigned int freq, unsigned int intf_id, unsigned int read_type)
{
    uint32           unit = HWP_MY_UNIT_ID();
    uint32           i2c_type;
    i2c_devConf_t    i2cDevice;
    int32            ret;

    if (strcmp(typeStr, "8") == 0)
        i2c_type = I2C_ADDR_WIDTH_BYTE_1;
    else if (strcmp(typeStr, "16") == 0)
        i2c_type = I2C_ADDR_WIDTH_BYTE_2;
    else
    {
        osal_printf("type mode %s is invalid.\n", typeStr);
        return;
    }

    i2cDevice.device_id = i2c_dev;
    i2cDevice.mem_addr_width = i2c_type;
    i2cDevice.data_width = i2c_type;
    i2cDevice.sda_pin_id = data_pin;
    i2cDevice.dev_addr = chipid;
    i2cDevice.clk_freq = freq;
    i2cDevice.i2c_interface_id= intf_id;
    i2cDevice.read_type = read_type;

    if ((ret = drv_i2c_init(unit)) != RT_ERR_OK)
        osal_printf("Initial fail (%d)!\n", ret);

    if ((ret = drv_i2c_dev_init(unit, &i2cDevice)) != RT_ERR_OK)
        osal_printf("Device Initial fail (%d)!\n", ret);

}


/* Function Name:
 *      rtk_i2c_read
 * Description:
 *      Read register from I2C device
 * Input:
 *      dev     - I2C controller device
 *      reg_idx - specific read register id
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      I2C interface is simulated by GPIO .
 */
void rtk_i2c_read(unsigned int dev, unsigned int reg_idx)
{
    uint32      unit = HWP_MY_UNIT_ID();
    int32       ret;
    uint8       data;

    if ((ret = drv_i2c_read(unit, dev, reg_idx, &data)) == RT_ERR_OK)
    {
        osal_printf("data 0x%x\n", data);
    }
    else
    {
        osal_printf("FAIL (%d)\n", ret);
    }
}

/* Function Name:
 *      rtk_i2c_write
 * Description:
 *      Write data to I2C device register
 * Input:
 *      dev     - I2C controller device
 *      reg_idx - specific write register id
 *      wData   - write data
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      I2C interface is simulated by GPIO .
 */
void rtk_i2c_write(unsigned int dev, unsigned int reg_idx, unsigned int wData)
{
    uint32      unit = HWP_MY_UNIT_ID();
    int32       ret;
    uint8       data;

    data = (uint8)wData;

    if ((ret = drv_i2c_write(unit, dev, reg_idx, &data)) == RT_ERR_OK)
    {
        osal_printf("OK\n");
    }
    else
    {
        osal_printf("FAIL (%d)\n", ret);
    }
}

#endif/* CONFIG_SDK_DRIVER_GPIO */

/* Function Name:
 *      rtk_boardId_get
 * Description:
 *      Set board id to env
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
int rtk_boardId_get (void)
{
    char *idStr;

    idStr = getenv(BDINFO_VAR_BOARDID);

    osal_printf("Board Model ID: %s\n", idStr);
    return 1;
}

/* Function Name:
 *      rtk_boardId_set
 * Description:
 *      Get board id from env
 * Input:
 *      id - board id
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
int rtk_boardId_set (int id)
{
    char valStr[16] = {0};

    sprintf(valStr, "%d", id);
    setenv(BDINFO_VAR_BOARDID, valStr);
    saveenv();

    return 0;
}

/* Function Name:
 *      rtk_boardModel_get
 * Description:
 *      Set board model to env
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
int rtk_boardModel_get (void)
{
    char *modelStr;

    modelStr = getenv(BDINFO_VAR_BOARDMODEL);

    osal_printf("Board Model: %s\n", modelStr);
    return 1;
}

/* Function Name:
 *      rtk_boardModel_set
 * Description:
 *      Get board model from env
 * Input:
 *      argv[2] - board model
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
int rtk_boardModel_set (char * const argv[])
{
    setenv(BDINFO_VAR_BOARDMODEL, argv[2]);
    saveenv();

    return 0;
}


void
_rtk_10gMedia_set(rtk_port_t port, rtk_port_10gMedia_t media)
{
    uint32      unit = uboot_cmd_unit;
    int32       ret;

    osal_printf("10gMedia set Unit %u Port %u\n", unit, port);
    if (unit >= RTK_MAX_NUM_OF_UNIT_LOCAL)
    {
        osal_printf("\nUnit %u invalid unit id \n", unit);
        return;
    }

    if ((ret = rtk_port_10gMedia_set(unit, port, media)) != RT_ERR_OK)
    {
        osal_printf("FAIL (0x%x)\n", ret);
    }
}

/* Function Name:
 *      rtk_10gMedia_none_set
 *      rtk_10gMedia_fiber10g_set
 *      rtk_10gMedia_fiber1g_set
 *      rtk_10gMedia_dac50cm_set
 *      rtk_10gMedia_dac100cm_set
 *      rtk_10gMedia_dac300cm_set
 * Description:
 *      Set 10G port media as defined in rtk_port_10gMedia_t
 * Input:
 *      port        mac ID
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void
rtk_10gMedia_none_set(unsigned int port)
{
    _rtk_10gMedia_set(port, PORT_10GMEDIA_NONE);
}
void
rtk_10gMedia_fiber10g_set(unsigned int port)
{
    _rtk_10gMedia_set(port, PORT_10GMEDIA_FIBER_10G);
}
void
rtk_10gMedia_fiber1g_set(unsigned int port)
{
    _rtk_10gMedia_set(port, PORT_10GMEDIA_FIBER_1G);
}
void
rtk_10gMedia_dac50cm_set(unsigned int port)
{
    _rtk_10gMedia_set(port, PORT_10GMEDIA_DAC_50CM);
}
void
rtk_10gMedia_dac100cm_set(unsigned int port)
{
    _rtk_10gMedia_set(port, PORT_10GMEDIA_DAC_100CM);
}
void
rtk_10gMedia_dac300cm_set(unsigned int port)
{
    _rtk_10gMedia_set(port, PORT_10GMEDIA_DAC_300CM);
}

/* Function Name:
 *      rtk_mac_sdsMode_set
 * Description:
 *      Configure mac serdes mode
 * Input:
 *      sds - serdes id
 *      mode - serdes mode
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void
rtk_mac_sdsMode_set(uint32 sds, uint32 mode)
{
#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    int32       ret  = RT_ERR_OK;
    uint32      unit = uboot_cmd_unit;
#endif
#if defined(CONFIG_SDK_RTL9300)
    if(HWP_9300_FAMILY_ID(unit))
    {
        printf("unit %u sds %u mode %u\n", unit, sds, mode);
        if ((ret = dal_longan_sds_macSds_set(unit, sds, mode)) != RT_ERR_OK)
        {
            printf("FAIL (0x%x)\n", ret);
        }
    }
    else
#endif
#if defined(CONFIG_SDK_RTL9310)
    if(HWP_9310_FAMILY_ID(unit))
    {
        printf("unit %u sds %u mode %u\n", unit, sds, mode);
        if ((ret = phy_rtl9310_sds_mode_set(unit, sds, mode)) != RT_ERR_OK)
        {
            printf("FAIL (0x%x)\n", ret);
        }
    }
    else
#endif
    {
        printf("Chip not supported\n");
    }

    return;
}


