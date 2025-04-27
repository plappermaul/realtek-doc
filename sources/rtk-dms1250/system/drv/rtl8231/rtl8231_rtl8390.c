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
 * Purpose : Definition those public RTL8231 APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *            1) i2c read & write
 *            2) mdc read & write
 */

/*
 * Include Files
 */
#include <common/rt_autoconf.h>
#include <common/rt_type.h>
#include <common/debug/rt_log.h>
#include <osal/sem.h>
#include <osal/time.h>
#include <ioal/mem32.h>
#include <private/drv/rtl8231/rtl8231_rtl8390.h>
#include <private/drv/swcore/swcore_rtl8390.h>

/*
 * Symbol Definition
 */

/* Access RTL8231 maximum timeout time:
 * MDC clock = M (ns). Refer to LED_MDC_DUTY_SEL/LED_CLK_DUTY_SEL, it is 384 or 96.
 * Pick 384 for following expression:
 * For serial mode, 64 x M x 1.1 ns.     -->    27,034 ns ~    28 us
 * For scan mode, 64 x M x 152 x 1.1 ns. --> 4,109,108 ns ~ 4,120 us
 * Use Max[scan mode, serial mode]
 */
#define ACCESS_RTL8231_TIMEOUT_TIME                 4120    /* microsecond */
#define ACCESS_RTL8231_CHECK_INTVL                  1       /* microsecond */

/*
 * Data Type Definition
 */

/*
 * Data Declaration
 */
static uint32       rtl8231_init[RTK_MAX_NUM_OF_UNIT];
static rtl8231_mdcSem_cb_entry_t _rtl8231_mdcSem_cb_tbl[RTK_MAX_NUM_OF_UNIT];

/*
 * Macro Definition
 */
#if defined(CONFIG_SDK_RTL8231) && defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE)
#define RTL8231_MDC_SEM_LOCK(unit)      _rtl8231_mdcSem_cb_tbl[unit].mdcSem_callback(unit, 0)
#define RTL8231_MDC_SEM_UNLOCK(unit)    _rtl8231_mdcSem_cb_tbl[unit].mdcSem_callback(unit, 1)
#else
#define RTL8231_MDC_SEM_LOCK(unit)
#define RTL8231_MDC_SEM_UNLOCK(unit)
#endif

#define GPIO_READ       0
#define GPIO_WRITE      1


/*
 * Wait for RTL8231 MDC access complete
 */
#define RTL8231_MDC_WAIT_COMPLETE(unit, REG, MASK) \
    do {\
        uint32 t=0;\
        uint32 regVal;\
        do {\
            osal_time_udelay(ACCESS_RTL8231_CHECK_INTVL); /* check the register every N microsecond */ \
            t += ACCESS_RTL8231_CHECK_INTVL;\
            if (ioal_mem32_read(unit, REG, &regVal) != RT_ERR_OK)\
            {\
                RTL8231_MDC_SEM_UNLOCK(unit);\
                return RT_ERR_FAILED;\
            }\
            if (0 == (regVal & MASK))\
            {\
                break;\
            }\
        } while (t <= ACCESS_RTL8231_TIMEOUT_TIME);\
        if (t > ACCESS_RTL8231_TIMEOUT_TIME)\
        {\
            RTL8231_MDC_SEM_UNLOCK(unit);\
            return RT_ERR_BUSYWAIT_TIMEOUT;\
        }\
    } while(0)


/*
 * Function Declaration
 */

/* Function Name:
 *      r8390_rtl8231_init
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
int32
r8390_rtl8231_init(uint32 unit)
{
    uint32  value = 0;
    RT_INIT_REENTRY_CHK(rtl8231_init[unit]);

    rtl8231_init[unit] = INIT_NOT_COMPLETED;

    /* Do some initialize if need */

    ioal_mem32_read(unit, RTL8390_LED_GLB_CTRL_ADDR, &value);
    value &= ~(RTL8390_LED_GLB_CTRL_EXT_GPIO_EN_MASK);
    value |= (0x4 << RTL8390_LED_GLB_CTRL_EXT_GPIO_EN_OFFSET);
    ioal_mem32_write(unit, RTL8390_LED_GLB_CTRL_ADDR, value);

    rtl8231_init[unit] = INIT_COMPLETED;
    return RT_ERR_OK;
} /* end of r8390_rtl8231_init */

/* Function Name:
 *      r8390_rtl8231_mdcSem_register
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
int32
r8390_rtl8231_mdcSem_register(uint32 unit, drv_rtl8231_mdcSem_cb_f fMdcSemCb)
{
    /* Check init state */
    RT_INIT_CHK(rtl8231_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK(NULL == fMdcSemCb, RT_ERR_NULL_POINTER);

    if (NULL == _rtl8231_mdcSem_cb_tbl[unit].mdcSem_callback)
    {
        _rtl8231_mdcSem_cb_tbl[unit].mdcSem_callback = fMdcSemCb;
    }
    else
    {
        /* Handler is already existing */
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of r8390_rtl8231_mdcSem_register */

/* Function Name:
 *      r8390_rtl8231_mdcSem_unregister
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
int32
r8390_rtl8231_mdcSem_unregister(uint32 unit, drv_rtl8231_mdcSem_cb_f fMdcSemCb)
{
    /* Check init state */
    RT_INIT_CHK(rtl8231_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK(NULL == fMdcSemCb, RT_ERR_NULL_POINTER);

    if (_rtl8231_mdcSem_cb_tbl[unit].mdcSem_callback == fMdcSemCb)
    {
        _rtl8231_mdcSem_cb_tbl[unit].mdcSem_callback = NULL;
    }
    else
    {
        /* Handler is nonexistent */
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of r8390_rtl8231_mdcSem_unregister */

/* Function Name:
 *      r8390_rtl8231_mdc_read
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
int32
r8390_rtl8231_mdc_read(uint32 unit, uint32 phy_id, uint32 page, uint32 reg_addr, uint32 *pData)
{
    uint32  temp;

    /* check Init status */
    RT_INIT_CHK(rtl8231_init[unit]);

    RTL8231_MDC_SEM_LOCK(unit);

    /* initialize variable */
    temp = 0;

    /* Input parameters:
     * If RWOP = 0(read), then INDATA[15:0] = {Reserved & PORT_ID[4:0]}
     * If RWOP = 1(write), then INDATA[15:0] = DATA[15:0]
     */
    temp |= ((phy_id << RTL8390_EXT_GPIO_INDRT_ACCESS_GPIO_PHY_ADDR_OFFSET) & RTL8390_EXT_GPIO_INDRT_ACCESS_GPIO_PHY_ADDR_MASK);

    /* Select register number to access */
    temp |= ((reg_addr << RTL8390_EXT_GPIO_INDRT_ACCESS_GPIO_REG_OFFSET) & RTL8390_EXT_GPIO_INDRT_ACCESS_GPIO_REG_MASK);

    /* Read/Write operation
     * 0b0: read
     * 0b1: write
     */
    temp |= ((GPIO_READ << RTL8390_EXT_GPIO_INDRT_ACCESS_GPIO_RWOP_OFFSET) & RTL8390_EXT_GPIO_INDRT_ACCESS_GPIO_RWOP_MASK);

    /* Request MAC to access PHY MII register
     * 0b0: complete access
     * 0b1: execute access
     * Note: When MAC completes access, it will clear this bit.
     */
    temp |= ((1 << RTL8390_EXT_GPIO_INDRT_ACCESS_GPIO_CMD_OFFSET) & RTL8390_EXT_GPIO_INDRT_ACCESS_GPIO_CMD_MASK);

    /* write register to active the read operation */
    ioal_mem32_write(unit, RTL8390_EXT_GPIO_INDRT_ACCESS_ADDR, temp);

    /* busy waiting until reg.bit[0] = 0b0 (MAC completes access) */
    RTL8231_MDC_WAIT_COMPLETE(unit, RTL8390_EXT_GPIO_INDRT_ACCESS_ADDR, 0x1);

    /* get the read operation result to temp */
    ioal_mem32_read(unit, RTL8390_EXT_GPIO_INDRT_ACCESS_ADDR, &temp);

    /* fill the DATA[15:0] from temp to pData */
    (*pData) = (temp & RTL8390_EXT_GPIO_INDRT_ACCESS_GPIO_DATA_MASK) >> RTL8390_EXT_GPIO_INDRT_ACCESS_GPIO_DATA_OFFSET;

    RTL8231_MDC_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      r8390_rtl8231_mdc_write
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
int32
r8390_rtl8231_mdc_write(uint32 unit, uint32 phy_id, uint32 page, uint32 reg_addr, uint32 data)
{
    uint32  temp;

    /* check Init status */
    RT_INIT_CHK(rtl8231_init[unit]);

    RTL8231_MDC_SEM_LOCK(unit);

    /* initialize variable */
    temp = 0;

    /* Input parameters:
     * If RWOP = 0(read), then INDATA[15:0] = {Reserved & PORT_ID[4:0]}
     * If RWOP = 1(write), then INDATA[15:0] = DATA[15:0]
     */
    temp |= ((phy_id << RTL8390_EXT_GPIO_INDRT_ACCESS_GPIO_PHY_ADDR_OFFSET) & RTL8390_EXT_GPIO_INDRT_ACCESS_GPIO_PHY_ADDR_MASK);

    /* Select register number to access */
    temp |= ((reg_addr << RTL8390_EXT_GPIO_INDRT_ACCESS_GPIO_REG_OFFSET) & RTL8390_EXT_GPIO_INDRT_ACCESS_GPIO_REG_MASK);

    /* Read/Write operation
     * 0b0: read
     * 0b1: write
     */
    temp |= ((GPIO_WRITE << RTL8390_EXT_GPIO_INDRT_ACCESS_GPIO_RWOP_OFFSET) & RTL8390_EXT_GPIO_INDRT_ACCESS_GPIO_RWOP_MASK);

    /* Input parameters:
     * If RWOP = 0(read), then GPIO_DATA[15:0] = input data[15:0]
     * If RWOP = 1(write), then GPIO_DATA [15:0] = output data[15:0]
     */
    temp |= ((data << RTL8390_EXT_GPIO_INDRT_ACCESS_GPIO_DATA_OFFSET) & RTL8390_EXT_GPIO_INDRT_ACCESS_GPIO_DATA_MASK);

    /* Request MAC to access PHY MII register
     * 0b0: complete access
     * 0b1: execute access
     * Note: When MAC completes access, it will clear this bit.
     */
    temp |= ((1 << RTL8390_EXT_GPIO_INDRT_ACCESS_GPIO_CMD_OFFSET) & RTL8390_EXT_GPIO_INDRT_ACCESS_GPIO_CMD_MASK);

    /* write register to active the read operation */
    ioal_mem32_write(unit, RTL8390_EXT_GPIO_INDRT_ACCESS_ADDR, temp);

    /* busy waiting until reg.bit[0] = 0b0 (MAC completes access) */
    RTL8231_MDC_WAIT_COMPLETE(unit, RTL8390_EXT_GPIO_INDRT_ACCESS_ADDR, 0x1);

    RTL8231_MDC_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

