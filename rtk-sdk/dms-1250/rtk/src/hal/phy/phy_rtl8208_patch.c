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
 * $Revision$
 * $Date$
 *
 * Purpose : PHY 8218B/8218FB/8214FC Driver APIs.
 *
 * Feature : PHY 8218B/8218FB/8214FC Driver APIs
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/debug/rt_log.h>
#include <hal/common/halctrl.h>
#include <hal/mac/miim_common_drv.h>
#include <hal/phy/phydef.h>
#include <hal/phy/phy_common.h>
#include <hal/phy/phy_rtl8208.h>


/*
 * Symbol Definition
 */

#if defined(CONFIG_SDK_RTL8380)
phy_patch_array_type0_t rtl8380_8208_perchip[] = {\
    {0, 0x1f, 0x0040},{0, 0x17, 0x0b49},{0, 0x13, 0x7146},{0, 0x13, 0x7106},\
    {0, 0x1f, 0x0004},{0, 0x19, 0x7230},{0, 0x18, 0xc0f3},{1, 0x1f, 0x0004},\
    {1, 0x19, 0x7230},{1, 0x18, 0xc0f3},{2, 0x1f, 0x0004},{2, 0x19, 0x7230},\
    {2, 0x18, 0xc0f3},{3, 0x1f, 0x0004},{3, 0x19, 0x7230},{3, 0x18, 0xc0f3},\
    {4, 0x1f, 0x0004},{4, 0x19, 0x7230},{4, 0x18, 0xc0f3},{5, 0x1f, 0x0004},\
    {5, 0x19, 0x7230},{5, 0x18, 0xc0f3},{6, 0x1f, 0x0004},{6, 0x19, 0x7230},\
    {6, 0x18, 0xc0f3},{7, 0x1f, 0x0004},{7, 0x19, 0x7230},{7, 0x18, 0xc0f3},\
    {7, 0x1c, 0x01bc},{7, 0x1d, 0xffc4},{7, 0x1c, 0x0080},{0, 0x1f, 0x0001},\
    {0, 0x1d, 0x0005},{0, 0x1c, 0x8888},{0, 0x1f, 0x0000},{0, 0x16, 0x1b8e},\
    {0, 0x10, 0x031f},{0, 0x1f, 0x0001},{0, 0x12, 0x9230},{1, 0x1f, 0x0001},\
    {1, 0x1d, 0x0005},{1, 0x1c, 0x8888},{1, 0x1f, 0x0000},{1, 0x16, 0x1b8e},\
    {1, 0x10, 0x031f},{1, 0x1f, 0x0001},{1, 0x12, 0x9230},{2, 0x1f, 0x0001},\
    {2, 0x1d, 0x0005},{2, 0x1c, 0x8888},{2, 0x1f, 0x0000},{2, 0x16, 0x1b8e},\
    {2, 0x10, 0x031f},{2, 0x1f, 0x0001},{2, 0x12, 0x9230},{3, 0x1f, 0x0001},\
    {3, 0x1d, 0x0005},{3, 0x1c, 0x8888},{3, 0x1f, 0x0000},{3, 0x16, 0x1b8e},\
    {3, 0x10, 0x031f},{3, 0x1f, 0x0001},{3, 0x12, 0x9230},{4, 0x1f, 0x0001},\
    {4, 0x1d, 0x0005},{4, 0x1c, 0x8888},{4, 0x1f, 0x0000},{4, 0x16, 0x1b8e},\
    {4, 0x10, 0x031f},{4, 0x1f, 0x0001},{4, 0x12, 0x9230},{5, 0x1f, 0x0001},\
    {5, 0x1d, 0x0005},{5, 0x1c, 0x8888},{5, 0x1f, 0x0000},{5, 0x16, 0x1b8e},\
    {5, 0x10, 0x031f},{5, 0x1f, 0x0001},{5, 0x12, 0x9230},{6, 0x1f, 0x0001},\
    {6, 0x1d, 0x0005},{6, 0x1c, 0x8888},{6, 0x1f, 0x0000},{6, 0x16, 0x1b8e},\
    {6, 0x10, 0x031f},{6, 0x1f, 0x0001},{6, 0x12, 0x9230},{7, 0x1f, 0x0001},\
    {7, 0x1d, 0x0005},{7, 0x1c, 0x8888},{7, 0x1f, 0x0000},{7, 0x16, 0x1b8e},\
    {7, 0x10, 0x031f},{7, 0x1f, 0x0001},{7, 0x12, 0x9230},\
    {4, 0x1f, 0x0040},{4, 0x13, 0x5348},\
    /* Enable Jumbo frame 16K selection & select diglitch, patch for force power down&up*/
      {0, 0x1f, 0x0000},{0, 0x17, 0x0101},{0, 0x1A, 0x4100},\
      {1, 0x1f, 0x0000},{1, 0x17, 0x0101},{1, 0x1A, 0x4100},\
      {2, 0x1f, 0x0000},{2, 0x17, 0x0101},{2, 0x1A, 0x4100},\
      {3, 0x1f, 0x0000},{3, 0x17, 0x0101},{3, 0x1A, 0x4100},\
      {4, 0x1f, 0x0000},{4, 0x17, 0x0101},{4, 0x1A, 0x4100},\
      {5, 0x1f, 0x0000},{5, 0x17, 0x0101},{5, 0x1A, 0x4100},\
      {6, 0x1f, 0x0000},{6, 0x17, 0x0101},{6, 0x1A, 0x4100},\
      {7, 0x1f, 0x0000},{7, 0x17, 0x0101},{7, 0x1A, 0x4100},\
    /* end  Enable Jumbo frame 16K selection */
    };
phy_patch_array_type1_t rtl8380_8208_perport[] = {\

    };



/* Function Name:
 *      rtl8380_rtl8208_config
 * Description:
 *      Patch the PHY:8208 for RTL8380.
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
rtl8380_rtl8208_config(uint32 unit, rtk_port_t port)
{
    uint32 i;
    uint32 val;
    int32  ret = RT_ERR_FAILED;

    /*Port should be base-port*/
    if((port%8) != 0)
    {
        ret = RT_ERR_OK;
        return ret;
    }

    /*Check the 8208 internal chip ID*/
   /*Page0 Reg3   dft:0xc882  bit[9:4] modle number:0x8  [3:0] revision number:0x2 */
    ret = hal_miim_read(unit, port, 0, 3, &val);
     if(ret  != RT_ERR_OK)
        return ret;

   val = (val >> 4) & 0x3f;

    if(val == 0x8)
    {
        /* Per-Chip */
        for (i=0; i<(sizeof(rtl8380_8208_perchip)/sizeof(phy_patch_array_type0_t)); i++)
        {
             ret = hal_miim_write(unit, port + rtl8380_8208_perchip[i].phy, \
                4095, rtl8380_8208_perchip[i].reg, rtl8380_8208_perchip[i].val);
             if(ret  != RT_ERR_OK)
                return ret;
        }

    }

    return ret;
}
#endif


/* Function Name:
 *      sub_phy_8208_patch_set
 * Description:
 *      Patch the PHY:8218B.
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
sub_phy_8208_patch_set(uint32 unit, rtk_port_t port)
{
    int32   ret;

#if defined(CONFIG_SDK_RTL8380)
    if (HWP_8330_FAMILY_ID(unit))
    {
        /* For 833x, enable serdes always linkup */
        {
            uint32 serdes_data;

            if((port % 8) == 0)
            {
                /*08L*/
                serdes_data = 0x8403;
                hal_miim_write(unit,  port,  64,  16 , serdes_data);
            }
        }

        /*Enable pktgen en*/
        {
            uint32 phy_data;

            /*08L*/
            phy_data = 0x1;
            hal_miim_write(unit,  port,  65,  19 , phy_data);
            phy_data = 0xff;
            hal_miim_write(unit,  port,  65,  21 , phy_data);
            phy_data = 0xff;
            hal_miim_write(unit,  port,  65,  20 , phy_data);
        }

        if ((ret = rtl8380_rtl8208_config(unit, port)) != RT_ERR_OK)
            return ret;

    }
#endif

    ret = RT_ERR_OK;
    return ret;
} /* end of sub_phy_8208_patch_set */


