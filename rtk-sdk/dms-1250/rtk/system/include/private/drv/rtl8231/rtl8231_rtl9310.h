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
 * $Revision: 76381 $
 * $Date$
 *
 * Purpose : Definition those public RTL8231 APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *            1) i2c read & write
 *            2) mdc read & write
 */
#ifndef __RTL8231_RTL9310_H__
#define __RTL8231_RTL9310_H__

/*
 * Include Files
 */
#include <private/drv/rtl8231/rtl8231.h>

/*
 * Symbol Definition
 */

/*
 * Data Type Definition
 */

/*
 * Data Declaration
 */

/*
 * Macro Definition
 */

/*
 * Function Declaration
 */

/* Function Name:
 *      r9310_rtl8231_init
 * Description:
 *      Initialize rtl8231 driver.
 * Input:
 *      unit     - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
r9310_rtl8231_init(uint32 unit);

/* Function Name:
 *      r9310_rtl8231_mdc_read
 * Description:
 *      Read rtl8231 register via MAC indirect access mechanism. (MDC/MDIO)
 * Input:
 *      unit     - unit id
 *      phy_id   - PHY id
 *      page     - PHY page
 *      reg_addr - 8231 register address
 * Output:
 *      pData    - pointer buffer of data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - pData is a null pointer.
 * Note:
 *      1) valid page as following:
 *      - 0x1D is internal register page
 *      - 0x1E is system register page (default)
 */
extern int32
r9310_rtl8231_mdc_read(uint32 unit, uint32 phy_id, uint32 page, uint32 reg_addr, uint32 *pData);

/* Function Name:
 *      r9310_rtl8231_mdc_write
 * Description:
 *      Write rtl8231 register via MAC indirect access mechanism. (MDC/MDIO)
 * Input:
 *      unit     - unit id
 *      phy_id   - PHY id
 *      page     - PHY page
 *      reg_addr - 8231 register address
 *      data     - configure data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      1) valid page as following:
 *      - 0x1D is internal register page
 *      - 0x1E is system register page (default)
 */
extern int32
r9310_rtl8231_mdc_write(uint32 unit, uint32 phy_id, uint32 page, uint32 reg_addr, uint32 data);

/* Function Name:
 *      r9310_rtl8231_extGPIOIntrStatus_get
 * Description:
 *      Get EXT GPIO interrupt status
 * Input:
 *      unit      - unit id
 *      dev       - RTL8231 Device ID
 * Output:
 *      pIsr      - Interrupt status
 * Return:
 *      None
 * Note:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 */
extern int32
r9310_rtl8231_extGPIOIntrStatus_get(uint32 unit, uint32 dev, uint32 *pIsr);

/* Function Name:
 *      r9310_rtl8231_pinIntrStatus_clear
 * Description:
 *      Clear External GPIO pin's interrupt status
 * Input:
 *      unit      - unit id
 *      dev       - RTL8231 Device ID
 *      pin       - pin ID
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 */
extern int32
r9310_rtl8231_pinIntrStatus_clear(uint32 unit, uint32 dev, uint32 pin);

/* Function Name:
 *      r9310_rtl8231_pinIntrStatus_get
 * Description:
 *      Get EXT GPIO pin's interrupt status
 * Input:
 *      unit      - unit id
 *      dev       - RTL8231 Device ID
 *      pin       - pin ID
 * Output:
 *      pIsr      - Interrupt status
 * Return:
 *      None
 * Note:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 */
extern int32
r9310_rtl8231_pinIntrStatus_get(uint32 unit, uint32 dev, uint32 pin, uint32 *pIsr);

/* Function Name:
 *      r9310_rtl8231_pinIntrEnable_set
 * Description:
 *      Set GPIO pin's interrupt enable bit
 * Input:
 *      unit      - unit id
 *      dev       - RTL8231 Device ID
 *      pin       - pin ID
 *      enable    - enable/disable interrupt
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 */
extern int32
r9310_rtl8231_pinIntrEnable_set(uint32 unit, uint32 dev, uint32 pin, rtk_enable_t enable);

/* Function Name:
 *      r9310_rtl8231_pinIntrEnable_get
 * Description:
 *      Set GPIO pin's interrupt enable bit
 * Input:
 *      unit      - unit id
 *      dev       - RTL8231 Device ID
 *      pin       - pin ID
 * Output:
 *      pEnable   - PIN interrupt enable
 * Return:
 *      None
 * Note:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 */
extern int32
r9310_rtl8231_pinIntrEnable_get(uint32 unit, uint32 dev, uint32 pin, rtk_enable_t * pEnable);

/* Function Name:
 *      r9310_rtl8231_intrMode_set
 * Description:
 *      Set GPIO pin's interrupt mode
 * Input:
 *      unit      - unit id
 *      dev       - RTL8231 Device ID
 *      pin       - pin ID
 *      intrMode  - disable interrupt, enable falling edge, enable rising edge,
 *                  or enable both falling and rising edge.
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 */
extern int32
r9310_rtl8231_pinIntrMode_set(uint32 unit, uint32 dev, uint32 pin, drv_extGpio_interruptType_t intrMode);

/* Function Name:
 *      r9310_rtl8231_intrMode_get
 * Description:
 *      Set GPIO pin's interrupt mode
 * Input:
 *      unit      - unit id
 *      dev       - RTL8231 Device ID
 *      pin       - pin ID
 * Output:
 *      pIntrMode - PIN interrupt Mode
 * Return:
 *      None
 * Note:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 */
extern int32
r9310_rtl8231_pinIntrMode_get(uint32 unit, uint32 dev, uint32 pin, drv_extGpio_interruptType_t * pIntrMode);

/* Function Name:
 *      r9310_rtl8231_directAccess_set
 * Description:
 *      Set EXT GPIO direct access mode and enable status
 * Input:
 *      unit      - unit id
 *      dev       - RTL8231 Device ID
 *      mode      - Direct Access Mode
 *      enable    - enable/disable interrupt
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 */
extern int32
r9310_rtl8231_directAccess_set(uint32 unit, uint32 adderss, drv_extGpio_directAccessMode_t mode, rtk_enable_t enable);

#endif /* __RTL8231_RTL9310_H__ */

