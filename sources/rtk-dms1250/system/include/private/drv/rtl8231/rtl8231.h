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
 * Purpose : DRV APIs definition.
 *
 * Feature : RTL8231 relative API
 *
 */

#ifndef __DRV_RTL8231_H__
#define __DRV_RTL8231_H__


/*
 * Include Files
 */
#include <common/error.h>
#include <drv/gpio/gpio.h>
#include <drv/gpio/ext_gpio.h>

/*
 * Symbol Definition
 */

#define RTL8231_GPIO_PIN_NUM_MAX                                37

#define RTL8231_LED_FUNC0_ADDR                                  (0x0000)
  #define RTL8231_LED_FUNC0_LED_START_OFFSET                    (1)
  #define RTL8231_LED_FUNC0_LED_START_MASK                      (0x1 << RTL8231_LED_FUNC0_LED_START_OFFSET)
  #define RTL8231_LED_FUNC0_EN_SYNC_GPIO_OFFSET                 (15)
  #define RTL8231_LED_FUNC0_EN_SYNC_GPIO_MASK                   (0x1 << RTL8231_LED_FUNC0_EN_SYNC_GPIO_OFFSET)

#define RTL8231_LED_FUNC1_READY                                 (0x370)
#define RTL8231_LED_FUNC1_ADDR                                  (0x0001)
  #define RTL8231_LED_FUNC1_EN_DEBOUNCING_31_OFFSET             (10)
  #define RTL8231_LED_FUNC1_EN_DEBOUNCING_31_MASK               (0x1 << RTL8231_LED_FUNC1_EN_DEBOUNCING_31_OFFSET)
  #define RTL8231_LED_FUNC1_EN_DEBOUNCING_32_OFFSET             (11)
  #define RTL8231_LED_FUNC1_EN_DEBOUNCING_32_MASK               (0x1 << RTL8231_LED_FUNC1_EN_DEBOUNCING_32_OFFSET)
  #define RTL8231_LED_FUNC1_EN_DEBOUNCING_33_OFFSET             (12)
  #define RTL8231_LED_FUNC1_EN_DEBOUNCING_33_MASK               (0x1 << RTL8231_LED_FUNC1_EN_DEBOUNCING_33_OFFSET)
  #define RTL8231_LED_FUNC1_EN_DEBOUNCING_34_OFFSET             (13)
  #define RTL8231_LED_FUNC1_EN_DEBOUNCING_34_MASK               (0x1 << RTL8231_LED_FUNC1_EN_DEBOUNCING_34_OFFSET)
  #define RTL8231_LED_FUNC1_EN_DEBOUNCING_35_OFFSET             (14)
  #define RTL8231_LED_FUNC1_EN_DEBOUNCING_35_MASK               (0x1 << RTL8231_LED_FUNC1_EN_DEBOUNCING_35_OFFSET)
  #define RTL8231_LED_FUNC1_EN_DEBOUNCING_36_OFFSET             (15)
  #define RTL8231_LED_FUNC1_EN_DEBOUNCING_36_MASK               (0x1 << RTL8231_LED_FUNC1_EN_DEBOUNCING_36_OFFSET)

#define RTL8231_GPIO_PIN_SEL_ADDR(gpio)                         ((0x0002) + ((gpio) >> 4))
#define RTL8231_GPIO_PIN_SEL0_ADDR                              (0x0002)
#define RTL8231_GPIO_PIN_SEL1_ADDR                              (0x0003)
  #define RTL8231_GPIO_PIN_SEL1_SELGPIO_31_OFFSET               (15)
  #define RTL8231_GPIO_PIN_SEL1_SELGPIO_31_MASK                 (0x1 << RTL8231_GPIO_PIN_SEL1_SELGPIO_31_OFFSET)
#define RTL8231_GPIO_PIN_SEL2_ADDR                              (0x0004)
  #define RTL8231_GPIO_PIN_SEL2_SELGPIO_32_OFFSET               (0)
  #define RTL8231_GPIO_PIN_SEL2_SELGPIO_32_MASK                 (0x1 << RTL8231_GPIO_PIN_SEL2_SELGPIO_32_OFFSET)
  #define RTL8231_GPIO_PIN_SEL2_SELGPIO_33_OFFSET               (1)
  #define RTL8231_GPIO_PIN_SEL2_SELGPIO_33_MASK                 (0x1 << RTL8231_GPIO_PIN_SEL2_SELGPIO_33_OFFSET)
  #define RTL8231_GPIO_PIN_SEL2_SELGPIO_34_OFFSET               (2)
  #define RTL8231_GPIO_PIN_SEL2_SELGPIO_34_MASK                 (0x1 << RTL8231_GPIO_PIN_SEL2_SELGPIO_34_OFFSET)
  #define RTL8231_GPIO_PIN_SEL2_IOMASK_32_OFFSET                (5)
  #define RTL8231_GPIO_PIN_SEL2_IOMASK_32_MASK                  (0x1 << RTL8231_GPIO_PIN_SEL2_IOMASK_32_OFFSET)
  #define RTL8231_GPIO_PIN_SEL2_IOMASK_33_OFFSET                (6)
  #define RTL8231_GPIO_PIN_SEL2_IOMASK_33_MASK                  (0x1 << RTL8231_GPIO_PIN_SEL2_IOMASK_33_OFFSET)
  #define RTL8231_GPIO_PIN_SEL2_IOMASK_34_OFFSET                (7)
  #define RTL8231_GPIO_PIN_SEL2_IOMASK_34_MASK                  (0x1 << RTL8231_GPIO_PIN_SEL2_IOMASK_34_OFFSET)
  #define RTL8231_GPIO_PIN_SEL2_IOMASK_35_OFFSET                (8)
  #define RTL8231_GPIO_PIN_SEL2_IOMASK_35_MASK                  (0x1 << RTL8231_GPIO_PIN_SEL2_IOMASK_35_OFFSET)
  #define RTL8231_GPIO_PIN_SEL2_IOMASK_36_OFFSET                (9)
  #define RTL8231_GPIO_PIN_SEL2_IOMASK_36_MASK                  (0x1 << RTL8231_GPIO_PIN_SEL2_IOMASK_36_OFFSET)
  #define RTL8231_GPIO_PIN_SEL2_INVMASK_32_OFFSET               (10)
  #define RTL8231_GPIO_PIN_SEL2_INVMASK_32_MASK                 (0x1 << RTL8231_GPIO_PIN_SEL2_INVMASK_32_OFFSET)
  #define RTL8231_GPIO_PIN_SEL2_INVMASK_33_OFFSET               (11)
  #define RTL8231_GPIO_PIN_SEL2_INVMASK_33_MASK                 (0x1 << RTL8231_GPIO_PIN_SEL2_INVMASK_33_OFFSET)
  #define RTL8231_GPIO_PIN_SEL2_INVMASK_34_OFFSET               (12)
  #define RTL8231_GPIO_PIN_SEL2_INVMASK_34_MASK                 (0x1 << RTL8231_GPIO_PIN_SEL2_INVMASK_34_OFFSET)
  #define RTL8231_GPIO_PIN_SEL2_INVMASK_35_OFFSET               (13)
  #define RTL8231_GPIO_PIN_SEL2_INVMASK_35_MASK                 (0x1 << RTL8231_GPIO_PIN_SEL2_INVMASK_35_OFFSET)
  #define RTL8231_GPIO_PIN_SEL2_INVMASK_36_OFFSET               (14)
  #define RTL8231_GPIO_PIN_SEL2_INVMASK_36_MASK                 (0x1 << RTL8231_GPIO_PIN_SEL2_INVMASK_36_OFFSET)

#define RTL8231_GPIO_IO_SEL_ADDR(gpio)                          ((0x0005) + ((gpio) >> 4))
#define RTL8231_GPIO_IO_SEL0_ADDR                               (0x0005)
#define RTL8231_GPIO_IO_SEL1_ADDR                               (0x0006)
  #define RTL8231_GPIO_IO_SEL1_IOMASK_31_OFFSET                 (15)
  #define RTL8231_GPIO_IO_SEL1_IOMASK_31_MASK                   (0x1 << RTL8231_GPIO_IO_SEL1_IOMASK_31_OFFSET)

#define RTL8231_GPIO_INV_SEL_ADDR(gpio)                         ((0x0007) + ((gpio) >> 4))
#define RTL8231_GPIO_INV_SEL0_ADDR                              (0x0007)
#define RTL8231_GPIO_INV_SEL1_ADDR                              (0x0008)
  #define RTL8231_GPIO_INV_SEL1_INVMASK_31_OFFSET               (15)
  #define RTL8231_GPIO_INV_SEL1_INVMASK_31_MASK                 (0x1 << RTL8231_GPIO_INV_SEL1_INVMASK_31_OFFSET)

#define RTL8231_GPIO_CTRL_ADDR(gpio)                            ((0x001C) + ((gpio) >> 4))
#define RTL8231_GPIO_CTRL0_ADDR                                 (0x001C)
#define RTL8231_GPIO_CTRL1_ADDR                                 (0x001D)
  #define RTL8231_GPIO_CTRL1_DATA_31_OFFSET                     (15)
  #define RTL8231_GPIO_CTRL1_DATA_31_MASK                       (0x1 << RTL8231_GPIO_CTRL1_DATA_31_OFFSET)
#define RTL8231_GPIO_CTRL2_ADDR                                 (0x001E)
  #define RTL8231_GPIO_CTRL2_DATA_32_OFFSET                     (0)
  #define RTL8231_GPIO_CTRL2_DATA_32_MASK                       (0x1 << RTL8231_GPIO_CTRL2_DATA_32_OFFSET)
  #define RTL8231_GPIO_CTRL2_DATA_33_OFFSET                     (1)
  #define RTL8231_GPIO_CTRL2_DATA_33_MASK                       (0x1 << RTL8231_GPIO_CTRL2_DATA_33_OFFSET)
  #define RTL8231_GPIO_CTRL2_DATA_34_OFFSET                     (2)
  #define RTL8231_GPIO_CTRL2_DATA_34_MASK                       (0x1 << RTL8231_GPIO_CTRL2_DATA_34_OFFSET)
  #define RTL8231_GPIO_CTRL2_DATA_35_OFFSET                     (3)
  #define RTL8231_GPIO_CTRL2_DATA_35_MASK                       (0x1 << RTL8231_GPIO_CTRL2_DATA_35_OFFSET)
  #define RTL8231_GPIO_CTRL2_DATA_36_OFFSET                     (4)
  #define RTL8231_GPIO_CTRL2_DATA_36_MASK                       (0x1 << RTL8231_GPIO_CTRL2_DATA_36_OFFSET)
  #define RTL8231_GPIO_CTRL2_SYNC_GPIO_OFFSET                   (15)
  #define RTL8231_GPIO_CTRL2_SYNC_GPIO_MASK                     (0x1 << RTL8231_GPIO_CTRL2_SYNC_GPIO_OFFSET)

typedef int32 (*drv_rtl8231_mdcSem_cb_f)(uint32 unit, uint32 type);

/*
 * Data Type Definition
 */
typedef struct rtl8231_mdcSem_cb_entry_s
{
    drv_rtl8231_mdcSem_cb_f mdcSem_callback;
} rtl8231_mdcSem_cb_entry_t;

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
extern int32
drv_rtl8231_init(uint32 unit);

/* !!! Below are private API !!! */

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
 * Note:
 *      None
 */
extern int32
drv_rtl8231_i2c_read(uint32 unit, uint32 slave_addr, uint32 reg_addr, uint32 *pData);

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
 * Note:
 *      None
 */
extern int32
drv_rtl8231_i2c_write(uint32 unit, uint32 slave_addr, uint32 reg_addr, uint32 data);

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
 * Note:
 *      1) valid page as following:
 *      - 0x1D is internal register page
 *      - 0x1E is system register page (default)
 */
extern int32
drv_rtl8231_mdc_read(uint32 unit, uint32 phy_id, uint32 page, uint32 reg_addr, uint32 *pData);

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
 * Note:
 *      1) valid page as following:
 *      - 0x1D is internal register page
 *      - 0x1E is system register page (default)
 */
extern int32
drv_rtl8231_mdc_write(uint32 unit, uint32 phy_id, uint32 page, uint32 reg_addr, uint32 data);

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
 * Note:
 *      None
 */
extern int32
drv_rtl8231_mdcSem_register(uint32 unit, drv_rtl8231_mdcSem_cb_f fMdcSemCb);

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
 * Note:
 *      None
 */
extern int32
drv_rtl8231_mdcSem_unregister(uint32 unit, drv_rtl8231_mdcSem_cb_f fMdcSemCb);

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
extern int32
drv_rtl8231_intrStatus_get(uint32 unit, uint32 dev, uint32 pinId, uint32 * pData);

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
extern int32
drv_rtl8231_intrStatus_clear(uint32 unit, uint32 dev, uint32 pinId);

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
extern int32
drv_rtl8231_extGPIOIntrStatus_get(uint32 unit, uint32 dev, uint32 * pData);

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
extern int32
drv_rtl8231_intrEnable_set(uint32 unit, uint32 dev, uint32 pinId, rtk_enable_t enable);

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
extern int32
drv_rtl8231_intrEnable_get(uint32 unit, uint32 dev, uint32 pinId, rtk_enable_t * pEnable);

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
extern int32
drv_rtl8231_intrMode_set(uint32 unit, uint32 dev, uint32 pinId, drv_extGpio_interruptType_t mode);

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
extern int32
drv_rtl8231_intrMode_get(uint32 unit, uint32 dev, uint32 pinId, drv_extGpio_interruptType_t * pMode);

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
extern int32
drv_rtl8231_directAccess_set(uint32 unit, uint32 address, drv_extGpio_directAccessMode_t mode, rtk_enable_t enable);

#endif /* __DRV_RTL8231_H__ */
