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
 * Purpose : Implementation of the RTL8231 driver
 *
 * Feature : RTL8231 driver
 *
 */

/*
 * Include Files
 */
#include <common/debug/rt_log.h>
#include <private/drv/swcore/swcore.h>
#include <private/drv/swcore/chip_probe.h>
#include <soc/type.h>
#include <common/rt_type.h>
#include <hwp/hw_profile.h>
#include <private/drv/rtl8231/rtl8231.h>
#include <private/drv/rtl8231/rtl8231_probe.h>
#include <drv/intr/intr.h>

/*
 * Symbol Definition
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
 *      drv_rtl8231_i2c_read
 * Description:
 *      Read rtl8231 register via MAC indirect access mechanism. (I2C)
 * Input:
 *      unit       - unit id
 *      slave_addr - I2C slave address
 *      reg_addr   - 8231 register address
 * Output:
 *      pData      - pointer buffer of data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - pData is a null pointer.
 *      RT_ERR_CHIP_NOT_FOUND
 * Note:
 *      None
 */
int32
drv_rtl8231_i2c_read(uint32 unit, uint32 slave_addr, uint32 reg_addr, uint32 *pData)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID), RT_ERR_UNIT_ID);
    RT_PARAM_CHK(!RTL8231_CHK(unit), RT_ERR_CHIP_NOT_FOUND);

    return RTL8231_CTRL(unit).i2c_read(unit, slave_addr, reg_addr, pData);
} /* end of drv_rtl8231_i2c_read */

/* Function Name:
 *      drv_rtl8231_i2c_write
 * Description:
 *      Write rtl8231 register via MAC indirect access mechanism. (I2C)
 * Input:
 *      unit       - unit id
 *      slave_addr - I2C slave address
 *      reg_addr   - 8231 register address
 *      data       - configure data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_CHIP_NOT_FOUND
 * Note:
 *      None
 */
int32
drv_rtl8231_i2c_write(uint32 unit, uint32 slave_addr, uint32 reg_addr, uint32 data)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID), RT_ERR_UNIT_ID);
    RT_PARAM_CHK(!RTL8231_CHK(unit), RT_ERR_CHIP_NOT_FOUND);

    return RTL8231_CTRL(unit).i2c_write(unit, slave_addr, reg_addr, data);
} /* end of drv_rtl8231_i2c_write */

/* Function Name:
 *      drv_rtl8231_mdc_read
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
 *      RT_ERR_CHIP_NOT_FOUND
 * Note:
 *      1) valid page as following:
 *      - 0x1D is internal register page
 *      - 0x1E is system register page (default)
 */
int32
drv_rtl8231_mdc_read(uint32 unit, uint32 phy_id, uint32 page, uint32 reg_addr, uint32 *pData)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID), RT_ERR_UNIT_ID);
    RT_PARAM_CHK(!RTL8231_CHK(unit), RT_ERR_CHIP_NOT_FOUND);

    return RTL8231_CTRL(unit).mdc_read(unit, phy_id, page, reg_addr, pData);
} /* end of drv_rtl8231_mdc_read */

/* Function Name:
 *      drv_rtl8231_mdc_write
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
 *      RT_ERR_CHIP_NOT_FOUND
 * Note:
 *      1) valid page as following:
 *      - 0x1D is internal register page
 *      - 0x1E is system register page (default)
 */
int32
drv_rtl8231_mdc_write(uint32 unit, uint32 phy_id, uint32 page, uint32 reg_addr, uint32 data)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID), RT_ERR_UNIT_ID);
    RT_PARAM_CHK(!RTL8231_CHK(unit), RT_ERR_CHIP_NOT_FOUND);

    return RTL8231_CTRL(unit).mdc_write(unit, phy_id, page, reg_addr, data);
} /* end of drv_rtl8231_mdc_write */

/* Function Name:
 *      drv_rtl8231_init
 * Description:
 *      Initialize rtl8231 driver.
 * Input:
 *      unit     - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_CHIP_NOT_FOUND
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
drv_rtl8231_init(uint32 unit)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID), RT_ERR_UNIT_ID);
    RT_PARAM_CHK(!RTL8231_CHK(unit), RT_ERR_CHIP_NOT_FOUND);

    RT_INIT_MSG("  RTL8231 init (unit %u)\n",unit);

    return RTL8231_CTRL(unit).init(unit);
} /* end of drv_rtl8231_init */

/* Function Name:
 *      drv_rtl8231_mdcSem_register
 * Description:
 *      Register the rtl8231 MDC/MDIO semaphore callback.
 * Input:
 *      unit      - unit id
 *      fMdcSemCb - rtl8231 MDC/MDIO semaphore callback function
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_CHIP_NOT_FOUND
 * Note:
 *      None
 */
int32
drv_rtl8231_mdcSem_register(uint32 unit, drv_rtl8231_mdcSem_cb_f fMdcSemCb)
{
    RT_PARAM_CHK(!RTL8231_CHK(unit), RT_ERR_CHIP_NOT_FOUND);

    return RTL8231_CTRL(unit).mdcSem_register(unit, fMdcSemCb);
} /* end of drv_rtl8231_mdcSem_register */

/* Function Name:
 *      drv_rtl8231_mdcSem_unregister
 * Description:
 *      Unregister the rtl8231 MDC/MDIO semaphore callback.
 * Input:
 *      unit      - unit id
 *      fMdcSemCb - rtl8231 MDC/MDIO semaphore callback function
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_CHIP_NOT_FOUND
 * Note:
 *      None
 */
int32
drv_rtl8231_mdcSem_unregister(uint32 unit, drv_rtl8231_mdcSem_cb_f fMdcSemCb)
{
    RT_PARAM_CHK(!RTL8231_CHK(unit), RT_ERR_CHIP_NOT_FOUND);

    return RTL8231_CTRL(unit).mdcSem_unregister(unit, fMdcSemCb);
} /* end of drv_rtl8231_mdcSem_unregister */

/* Function Name:
 *      drv_rtl8231_intrEnable_set
 * Description:
 *      Set rtl8231 pin interrupt enable status
 * Input:
 *      unit   - unit id
 *      dev    - external GPIO dev id
 *      pinId  - gpio id
 *      enable - enable
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_CHIP_NOT_SUPPORTED
 * Note:
 *      None
 */
int32
drv_rtl8231_intrEnable_set(uint32 unit, uint32 dev, uint32 pinId, rtk_enable_t enable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID), RT_ERR_UNIT_ID);
    RT_PARAM_CHK(!RTL8231_CHK(unit), RT_ERR_CHIP_NOT_FOUND);

#if !defined(__BOOTLOADER__)
    drv_intr_swcoreImrExtGpioPinEnable_set(unit, pinId, enable);
#endif

    return RTL8231_CTRL(unit).pinIntrEnable_set(unit, dev, pinId, enable);
} /* end of drv_rtl8231_intrEnable_set */

/* Function Name:
 *      drv_rtl8231_intrEnable_get
 * Description:
 *      Set rtl8231 pin interrupt enable status
 * Input:
 *      unit   - unit id
 *      dev    - external GPIO dev id
 *      pinId  - gpio id
 * Output:
 *      pEnable    - Enable Status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_CHIP_NOT_SUPPORTED
 * Note:
 *      None
 */
int32
drv_rtl8231_intrEnable_get(uint32 unit, uint32 dev, uint32 pinId, rtk_enable_t * pEnable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID), RT_ERR_UNIT_ID);
    RT_PARAM_CHK(!RTL8231_CHK(unit), RT_ERR_CHIP_NOT_FOUND);

    return RTL8231_CTRL(unit).pinIntrEnable_get(unit, dev, pinId, pEnable);
} /* end of drv_rtl8231_intrEnable_get */

/* Function Name:
 *      drv_rtl8231_intrMode_set
 * Description:
 *      Set rtl8231 pin interrupt Mode
 * Input:
 *      unit   - unit id
 *      dev    - external GPIO dev id
 *      pinId  - gpio pin id
 *      enable - enable
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_CHIP_NOT_SUPPORTED
 * Note:
 *      None
 */
int32
drv_rtl8231_intrMode_set(uint32 unit, uint32 dev, uint32 pinId, drv_extGpio_interruptType_t mode)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID), RT_ERR_UNIT_ID);
    RT_PARAM_CHK(!RTL8231_CHK(unit), RT_ERR_CHIP_NOT_FOUND);

    return RTL8231_CTRL(unit).pinIntrMode_set(unit, dev, pinId, mode);
} /* end of drv_rtl8231_intrMode_set */

/* Function Name:
 *      drv_rtl8231_intrMode_get
 * Description:
 *      Set rtl8231 pin interrupt enable status
 * Input:
 *      unit   - unit id
 *      dev    - external GPIO dev id
 *      pinId  - gpio pin id
 * Output:
 *      pMode      - Enable Status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_CHIP_NOT_SUPPORTED
 * Note:
 *      None
 */
int32
drv_rtl8231_intrMode_get(uint32 unit, uint32 dev, uint32 pinId, drv_extGpio_interruptType_t * pMode)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID), RT_ERR_UNIT_ID);
    RT_PARAM_CHK(!RTL8231_CHK(unit), RT_ERR_CHIP_NOT_FOUND);

    return RTL8231_CTRL(unit).pinIntrMode_get(unit, dev, pinId, pMode);
} /* end of drv_rtl8231_intrMode_get */


/* Function Name:
 *      drv_rtl8231_intrStatus_get
 * Description:
 *      Write rtl8231 register via MAC indirect access mechanism. (I2C)
 * Input:
 *      unit   - unit id
 *      dev    - external GPIO dev id
 *      pinId  - gpio pin id
 * Output:
 *      pData  - gpio pin interrupt pending bit
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_CHIP_NOT_SUPPORTED
 * Note:
 *      None
 */
int32
drv_rtl8231_intrStatus_get(uint32 unit, uint32 dev, uint32 pinId, uint32 * pData)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID), RT_ERR_UNIT_ID);
    RT_PARAM_CHK(!RTL8231_CHK(unit), RT_ERR_CHIP_NOT_FOUND);

    return RTL8231_CTRL(unit).intrStatus_get(unit, dev, pinId, pData);
} /* end of drv_rtl8231_intrStatus_get */

/* Function Name:
 *      drv_rtl8231_intrStatus_clear
 * Description:
 *      Clear rtl8231 EXT GPIO pin's interrupt pending bit
 * Input:
 *      unit   - unit id
 *      dev    - external GPIO dev id
 *      pinId  - gpio pin id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_CHIP_NOT_SUPPORTED
 * Note:
 *      None
 */
int32
drv_rtl8231_intrStatus_clear(uint32 unit, uint32 dev, uint32 pinId)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID), RT_ERR_UNIT_ID);
    RT_PARAM_CHK(!RTL8231_CHK(unit), RT_ERR_CHIP_NOT_FOUND);

    return RTL8231_CTRL(unit).intrStatus_clear(unit, dev, pinId);
} /* end of drv_rtl8231_intrStatus_clear */


/* Function Name:
 *      drv_rtl8231_extGPIOIntrStatus_get
 * Description:
 *      Get rtl8231 Interrupt status.
 * Input:
 *      unit   - unit id
 *      dev    - external GPIO dev id
 * Output:
 *      pData  - the gpio pin configuration data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_CHIP_NOT_SUPPORTED
 * Note:
 *      None
 */
int32
drv_rtl8231_extGPIOIntrStatus_get(uint32 unit, uint32 dev, uint32 * pData)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID), RT_ERR_UNIT_ID);
    RT_PARAM_CHK(!RTL8231_CHK(unit), RT_ERR_CHIP_NOT_FOUND);

    return RTL8231_CTRL(unit).extGPIOIntrStatus_get(unit, dev, pData);
} /* end of drv_rtl8231_intrStatus_get */

/* Function Name:
 *      drv_rtl8231_directAccess_set
 * Description:
 *      Set EXT GPIO direct access mode and enable status
 * Input:
 *      unit      - unit id
 *      address   - RTL8231 address
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
int32
drv_rtl8231_directAccess_set(uint32 unit, uint32 address, drv_extGpio_directAccessMode_t mode, rtk_enable_t enable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID), RT_ERR_UNIT_ID);
    RT_PARAM_CHK(!RTL8231_CHK(unit), RT_ERR_CHIP_NOT_FOUND);

    return RTL8231_CTRL(unit).directAccess_set(unit, address, mode, enable);

}

