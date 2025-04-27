/*
 * Copyright (C) 2018 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 75479 $
 * $Date: 2017-01-20 15:17:16 +0800 (Fri, 20 Jan 2017) $
 *
 * Purpose : PHY 8284 Driver APIs.
 *
 * Feature : PHY 8284 Driver APIs
 *
 */
#ifndef __HAL_PHY_PHY_RTL8284_H__
#define __HAL_PHY_PHY_RTL8284_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/port.h>


/*
 * Symbol Definition
 */
#define PORT_NUM_IN_8284        4





/*
 * Data Declaration
 */
extern rt_phyInfo_t phy_8284_info;


/*
 * Macro Declaration
 */


/*
 * Function Declaration
 */



/* Function Name:
 *      _phy_8284_sdsReg_read
 * Description:
 *      read serdes register through indirect access
 * Input:
 *      unit - unit id
 *      port - port id
 *      sdsPage - serdes page
 *      sdsRegAddr - serdes register address
 * Output:
 *      pData - serdes register data
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - read fail
 * Note:
 *      None
 */
extern int32
_phy_8284_sdsReg_read(uint32 unit, rtk_port_t port, uint32 sdsPage, uint32 sdsRegAddr, uint32 *pData);

/* Function Name:
 *      _phy_8284_sdsReg_write
 * Description:
 *      Write serdes register through indirect access
 * Input:
 *      unit - unit id
 *      port - port id
 *      sdsPage - serdes page
 *      sdsRegAddr - serdes register address
 *      data - register data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - write fail
 * Note:
 *      None
 */
extern int32
_phy_8284_sdsReg_write(uint32 unit, rtk_port_t port, uint32 sdsPage, uint32 sdsRegAddr, uint32 data);

/* Function Name:
 *      _phy_8284_sdsRegField_write
 * Description:
 *      Write field of serdes register through indirect access
 * Input:
 *      unit - unit id
 *      port - port id
 *      sdsPage - serdes page
 *      sdsRegAddr - serdes register address
 *      msb - MSB of the field
 *      lsb - LSB of the field
 *      value - field value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - write fail
 * Note:
 *      None
 */
extern int32
_phy_8284_sdsRegField_write(uint32 unit, rtk_port_t port, uint32 sdsPage, uint32 sdsRegAddr, uint32 msb, uint32 lsb, uint32 value);



extern int32
_phy_8284_mmd_read(uint32 unit, rtk_port_t port, uint32 mmdAddr, uint32 mmdReg, uint32 *pData);

extern int32
_phy_8284_mmd_write(uint32 unit, rtk_port_t port, uint32 mmdAddr, uint32 mmdReg, uint32 data);



/* Function Name:
 *      phy_8284drv_mapperInit
 * Description:
 *      Initialize PHY 8284 driver.
 * Input:
 *      pPhydrv - pointer of phy driver
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern void
phy_8284drv_mapperInit(rt_phydrv_t *pPhydrv);


#endif /* __HAL_PHY_PHY_RTL8284_H__ */

