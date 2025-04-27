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
 * $Revision: 97631 $
 * $Date: 2019-06-14 17:27:31 +0800 (Fri, 14 Jun 2019) $
 *
 * Purpose : IOAL Layer Init Module
 *
 * Feature : IOAL parameter Functions
 *
 */
#ifndef __IOAL_PARAM_H__
#define __IOAL_PARAM_H__

/*
 * Include Files
 */
#include <hwp/hw_profile.h>


/*
 * Function Declaration
 */

/* Function Name:
 *      ioal_param_hwpIdKey_get
 * Description:
 *      Each h/w board has its own ID (we call hardware profile ID), either integer or string.
 *      Please put your own code here according to your hardware design.
 *
 *      Typical design may be (1)store hardware profile ID in EEPROM, (2) read from GPIO PINs, (3) or store in NVRAM, whatever.
 *
 *      How-To-implement:
 *          (a) Fill identifier->type to be HWP_ID_TYPE_STRING or HWP_ID_TYPE_INTEGER, if your board ID is by string or integer
 *          (b) Put the ID to identifier->name, if type is HWP_ID_TYPE_STRING, OR
 *              put the ID to identifier->id, if type is HWP_ID_TYPE_INTEGER
 *
 * Input:
 *      None
 * Output:
 *      hwp_identifier_t *identifier
 * Return:
 *      RT_ERR_OK       - Got
 *      RT_ERR_FAILED   - Not found
 * Note:
 *      None
 */
extern int32
ioal_param_hwpIdKey_get(hwp_identifier_t *identifier);

/* Function Name:
 *      ioal_param_ledInitFlag_get
 * Description:
 *      Port's link/speed/... LED can be controled by the switch chip through RTL8231.
 *      SDK will initial related registers according to hardware profile's LED config.
 *
 *      You can skip the LED initial by setting *ledModeInitSkip = LEDMODEINITSKIP_YES
 *      Set *ledModeInitSkip = LEDMODEINITSKIP_NO, SDK will do the intitial.
 * Input:
 *      None
 * Output:
 *      ledModeInitSkip   - LEDMODEINITSKIP_NO or LEDMODEINITSKIP_YES.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *    None
 */
extern int32
ioal_param_ledInitFlag_get(uint8 *ledModeInitSkip);


/* Function Name:
 *      ioal_param_sysMac_get
 * Description:
 *      Get System MAC address.
 * Input:
 *      pMacAddr   - mac address buffer
 * Output:
 *      pMacAddr   - mac address
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *    None
 */
extern int32
ioal_param_sysMac_get(rtk_mac_t *pMacAddr);

/* Function Name:
 *      ioal_param_phyXfmrRj45Impd_get
 * Description:
 *      impedance between transformer and RJ45 a.k.a copper MDI differential pair impedance for CS patch.
 *      This API will be called by RTL8218D chip initial flow.
 *      The initial value of some registers will be different according to the different impedance level on the board.
 *
 *      Consult your HW what enum in rtk_phy_xfmrRj45Impd_t shall be used for your board.
 *      Different board may have to return differnt value in this API.
 * Input:
 *      unit        - unit id
 * Output:
 *      pImpd - enum value of impedance level.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *    Apply for PHY RTL8218D
 */
extern int32
ioal_param_phyXfmrRj45Impd_get(uint32 unit, rtk_phy_xfmrRj45Impd_t *pImpd);

#endif  /* __IOAL_PARAM_H__ */

