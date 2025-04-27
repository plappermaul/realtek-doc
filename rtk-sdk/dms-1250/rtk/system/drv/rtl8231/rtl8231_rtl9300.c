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
#include <private/drv/rtl8231/rtl8231_rtl9300.h>
#if defined(CONFIG_SDK_RTL9300)
#include <private/drv/swcore/swcore_rtl9300.h>
#endif
#include <hal/common/halctrl.h>


/*
 * Symbol Definition
 */

/* Access RTL8231 maximum timeout time:
 * MDC clock = M (ns).
 */
#define ACCESS_RTL8231_TIMEOUT_TIME                 (19*1000)   /* microsecond */
#define ACCESS_RTL8231_CHECK_INTVL                  1           /* microsecond */

/*
 * Data Type Definition
 */

/*
 * Data Declaration
 */
static uint32                       rtl8231_init[RTK_MAX_NUM_OF_UNIT];
static osal_mutex_t                 rtl8231_sem[RTK_MAX_NUM_OF_UNIT];

/*
 * Macro Definition
 */
#define RTL8231_MDC_SEM_LOCK(unit)      do {\
                                            if (osal_sem_mutex_take(rtl8231_sem[unit], OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)\
                                            {\
                                                RT_ERR(RT_ERR_SEM_LOCK_FAILED, (MOD_HAL), "semaphore lock failed");\
                                                return RT_ERR_SEM_LOCK_FAILED;\
                                            }\
                                        } while(0)

#define RTL8231_MDC_SEM_UNLOCK(unit)   do {\
                                            if (osal_sem_mutex_give(rtl8231_sem[unit]) != RT_ERR_OK)\
                                            {\
                                                RT_ERR(RT_ERR_SEM_UNLOCK_FAILED, (MOD_HAL), "semaphore unlock failed");\
                                                return RT_ERR_SEM_UNLOCK_FAILED;\
                                            }\
                                        } while(0)



#define GPIO_READ       0
#define GPIO_WRITE      1

#define REG_FIELD_SET(_pData, _val, _fOffset, _fMask)       (*_pData) = ((*_pData) & ~_fMask) | ((_val << _fOffset) & _fMask)
#define REG_FIELD_GET(_data, _fOffset, _fMask)              (_data & _fMask) >> _fOffset

/*
 * Wait for RTL8231 MDC access complete
 */
#define RTL8231_MDC_WAIT_COMPLETE(unit) \
    do {\
        uint32 t=0;\
        uint32 regVal;\
        do {\
            if (ioal_mem32_read(unit, RTL9300_EXT_GPIO_INDRT_ACCESS_CTRL_ADDR, &regVal) != RT_ERR_OK)\
            {\
                RTL8231_MDC_SEM_UNLOCK(unit);\
                return RT_ERR_FAILED;\
            }\
            if (regVal & RTL9300_EXT_GPIO_INDRT_ACCESS_CTRL_GPIO_RCMD_FAIL_MASK)\
            {\
                RTL8231_MDC_SEM_UNLOCK(unit);\
                return RT_ERR_FAILED;\
            }\
            if (0 == (regVal & RTL9300_EXT_GPIO_INDRT_ACCESS_CTRL_GPIO_CMD_MASK))\
            {\
                break;\
            }\
            osal_time_udelay(ACCESS_RTL8231_CHECK_INTVL); /* check the register every N microsecond */ \
            t += ACCESS_RTL8231_CHECK_INTVL;\
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
 *      r9300_rtl8231_init
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
int32 r9300_rtl8231_init(uint32 unit)
{
    int32       ret;

    RT_INIT_REENTRY_CHK(rtl8231_init[unit]);

    rtl8231_init[unit] = INIT_NOT_COMPLETED;

    /* create semaphore */
    rtl8231_sem[unit] = osal_sem_mutex_create();
    if (0 == rtl8231_sem[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "semaphore create failed");
        return RT_ERR_FAILED;
    }

    RTL8231_MDC_SEM_LOCK(unit);
    /* Configure GPIO[2:1] acting as MDC/MDIO for indirect access of 8231 */
    if ((ret = ioal_mem32_field_write(unit, RTL9300_EXT_GPIO_GLB_CTRL_ADDR,
                                      RTL9300_EXT_GPIO_GLB_CTRL_EXT_GPIO_EN_OFFSET,
                                      RTL9300_EXT_GPIO_GLB_CTRL_EXT_GPIO_EN_MASK, 0x1)) != RT_ERR_OK)
    {
        RTL8231_MDC_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_HAL), "");
        return ret;
    }
    RTL8231_MDC_SEM_UNLOCK(unit);
    rtl8231_init[unit] = INIT_COMPLETED;
    return RT_ERR_OK;
} /* end of r9300_rtl8231_init */

/* Function Name:
 *      r9300_rtl8231_mdc_read
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
int32 r9300_rtl8231_mdc_read(uint32 unit, uint32 phy_id, uint32 page, uint32 reg_addr, uint32 *pData)
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
    REG_FIELD_SET(&temp, phy_id, RTL9300_EXT_GPIO_INDRT_ACCESS_CTRL_GPIO_PHYADRR_OFFSET, RTL9300_EXT_GPIO_INDRT_ACCESS_CTRL_GPIO_PHYADRR_MASK);

    /* Select register number to access */
    REG_FIELD_SET(&temp, reg_addr, RTL9300_EXT_GPIO_INDRT_ACCESS_CTRL_GPIO_REG_OFFSET, RTL9300_EXT_GPIO_INDRT_ACCESS_CTRL_GPIO_REG_MASK);

    /* Read/Write operation
     * 0b0: read
     * 0b1: write
     */
    REG_FIELD_SET(&temp, GPIO_READ, RTL9300_EXT_GPIO_INDRT_ACCESS_CTRL_GPIO_RWOP_OFFSET, RTL9300_EXT_GPIO_INDRT_ACCESS_CTRL_GPIO_RWOP_MASK);

    /* Request MAC to access PHY MII register
     * 0b0: complete access
     * 0b1: execute access
     * Note: When MAC completes access, it will clear this bit.
     */
    REG_FIELD_SET(&temp, 1, RTL9300_EXT_GPIO_INDRT_ACCESS_CTRL_GPIO_CMD_OFFSET, RTL9300_EXT_GPIO_INDRT_ACCESS_CTRL_GPIO_CMD_MASK);

    /* write register to active the read operation */
    ioal_mem32_write(unit, RTL9300_EXT_GPIO_INDRT_ACCESS_CTRL_ADDR, temp);

    /* busy waiting until reg.bit[0] = 0b0 (MAC completes access) */
    RTL8231_MDC_WAIT_COMPLETE(unit);

    /* get the read operation result and fill the DATA[15:0] pData  */
    ioal_mem32_field_read(unit, RTL9300_EXT_GPIO_INDRT_ACCESS_CTRL_ADDR,
                          RTL9300_EXT_GPIO_INDRT_ACCESS_CTRL_GPIO_DATA_OFFSET,
                          RTL9300_EXT_GPIO_INDRT_ACCESS_CTRL_GPIO_DATA_MASK,
                          pData);

    RTL8231_MDC_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      r9300_rtl8231_mdc_write
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
int32 r9300_rtl8231_mdc_write(uint32 unit, uint32 phy_id, uint32 page, uint32 reg_addr, uint32 data)
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
    REG_FIELD_SET(&temp, phy_id, RTL9300_EXT_GPIO_INDRT_ACCESS_CTRL_GPIO_PHYADRR_OFFSET, RTL9300_EXT_GPIO_INDRT_ACCESS_CTRL_GPIO_PHYADRR_MASK);

    /* Select register number to access */
    REG_FIELD_SET(&temp, reg_addr, RTL9300_EXT_GPIO_INDRT_ACCESS_CTRL_GPIO_REG_OFFSET, RTL9300_EXT_GPIO_INDRT_ACCESS_CTRL_GPIO_REG_MASK);

    /* Read/Write operation
     * 0b0: read
     * 0b1: write
     */
    REG_FIELD_SET(&temp, GPIO_WRITE, RTL9300_EXT_GPIO_INDRT_ACCESS_CTRL_GPIO_RWOP_OFFSET, RTL9300_EXT_GPIO_INDRT_ACCESS_CTRL_GPIO_RWOP_MASK);

    /* Input parameters:
     * If RWOP = 0(read), then GPIO_DATA[15:0] = input data[15:0]
     * If RWOP = 1(write), then GPIO_DATA [15:0] = output data[15:0]
     */
    REG_FIELD_SET(&temp, data, RTL9300_EXT_GPIO_INDRT_ACCESS_CTRL_GPIO_DATA_OFFSET, RTL9300_EXT_GPIO_INDRT_ACCESS_CTRL_GPIO_DATA_MASK);

    /* Request MAC to access PHY MII register
     * 0b0: complete access
     * 0b1: execute access
     * Note: When MAC completes access, it will clear this bit.
     */
    REG_FIELD_SET(&temp, 1, RTL9300_EXT_GPIO_INDRT_ACCESS_CTRL_GPIO_CMD_OFFSET, RTL9300_EXT_GPIO_INDRT_ACCESS_CTRL_GPIO_CMD_MASK);

    /* write register to active the read operation */
    ioal_mem32_write(unit, RTL9300_EXT_GPIO_INDRT_ACCESS_CTRL_ADDR, temp);

    /* busy waiting until reg.bit[0] = 0b0 (MAC completes access) */
    RTL8231_MDC_WAIT_COMPLETE(unit);

    RTL8231_MDC_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      r9300_rtl8231_pinIntrEnable_set
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
int32
r9300_rtl8231_pinIntrEnable_set(uint32 unit, uint32 dev, uint32 pin, rtk_enable_t enable)
{
    uint32  temp = 0;
    uint32  reg_idx, pin_idx;

    /* check Init status */
    RT_INIT_CHK(rtl8231_init[unit]);
    if(pin >= RTL8231_GPIO_PIN_NUM_MAX){
        /* PIN No. is out of range */
        return RT_ERR_FAILED;
    }

    RTL8231_MDC_SEM_LOCK(unit);

    reg_idx = (uint32)((pin)/RTL9300_EXT_GPIO_FIELD_BIT1_REG0_SEL);
    pin_idx = (uint32)((pin)%RTL9300_EXT_GPIO_FIELD_BIT1_REG0_SEL);

    if(reg_idx == RTL9300_EXT_GPIO_REG_SEL_0)
    {
        REG_FIELD_SET(&temp, (enable << pin_idx), RTL9300_IMR_EXT_GPIO0_IMR_EXT_GPIO_31_0_OFFSET, RTL9300_IMR_EXT_GPIO0_IMR_EXT_GPIO_31_0_MASK);
        /* write register to active the read operation */
        ioal_mem32_write(unit, RTL9300_IMR_EXT_GPIO0_ADDR, temp);
    }else if(reg_idx == RTL9300_EXT_GPIO_REG_SEL_1){
        REG_FIELD_SET(&temp, (enable << pin_idx), RTL9300_IMR_EXT_GPIO1_IMR_EXT_GPIO_36_32_OFFSET, RTL9300_IMR_EXT_GPIO1_IMR_EXT_GPIO_36_32_MASK);
        /* write register to active the read operation */
        ioal_mem32_write(unit, RTL9300_IMR_EXT_GPIO1_ADDR, temp);
    }

    RTL8231_MDC_SEM_UNLOCK(unit);
    return RT_ERR_OK;
}


/* Function Name:
 *      r9300_rtl8231_pinIntrEnable_get
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
int32
r9300_rtl8231_pinIntrEnable_get(uint32 unit, uint32 dev, uint32 pin, rtk_enable_t * pEnable)
{
    uint32  temp = 0;
    uint32  reg_idx, pin_idx;

    /* check Init status */
    RT_INIT_CHK(rtl8231_init[unit]);
    if(pin >= RTL8231_GPIO_PIN_NUM_MAX){
        /* PIN No. is out of range */
        return RT_ERR_FAILED;
    }

    reg_idx = (uint32)((pin)/RTL9300_EXT_GPIO_FIELD_BIT1_REG0_SEL);
    pin_idx = (uint32)((pin)%RTL9300_EXT_GPIO_FIELD_BIT1_REG0_SEL);
    RTL8231_MDC_SEM_LOCK(unit);

    if(reg_idx == RTL9300_EXT_GPIO_REG_SEL_0)
    {
        /* Read write register to active the read operation */
        ioal_mem32_read(unit, RTL9300_IMR_EXT_GPIO0_ADDR, &temp);
        *pEnable = (rtk_enable_t)((temp&(1 << pin_idx)) >> (pin_idx));
    }else if(reg_idx == RTL9300_EXT_GPIO_REG_SEL_1){
        /* Read write register to active the read operation */
        ioal_mem32_read(unit, RTL9300_IMR_EXT_GPIO1_ADDR, &temp);
        *pEnable = (rtk_enable_t)((temp&(1 << pin_idx)) >> (pin_idx));
    }

    RTL8231_MDC_SEM_UNLOCK(unit);
    return RT_ERR_OK;
}


/* Function Name:
 *      r9300_rtl8231_intrMode_set
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
int32
r9300_rtl8231_pinIntrMode_set(uint32 unit, uint32 dev, uint32 pin, drv_extGpio_interruptType_t intrMode)
{
    uint32  temp = 0;
    uint32  reg_idx, pin_idx;

    /* check Init status */
    RT_INIT_CHK(rtl8231_init[unit]);
    if(pin >= RTL8231_GPIO_PIN_NUM_MAX){
        /* PIN No. is out of range */
        return RT_ERR_FAILED;
    }
    reg_idx = (uint32)((pin)/RTL9300_EXT_GPIO_FIELD_BIT2_REG0_SEL);
    pin_idx = (uint32)((pin)%RTL9300_EXT_GPIO_FIELD_BIT2_REG0_SEL);
    pin_idx = (pin_idx * RTL9300_EXT_GPIO_FIELD_BIT2_REG0_OFFSET);

    RTL8231_MDC_SEM_LOCK(unit);

    if(reg_idx == RTL9300_EXT_GPIO_REG_SEL_0)
    {
        ioal_mem32_read(unit, RTL9300_ISR_EXT_GPIO_MODE0_ADDR, &temp);
        REG_FIELD_SET(&temp, (intrMode), (pin_idx), (0x3 << (pin_idx)));
        /* write register to active the read operation */
        ioal_mem32_write(unit, RTL9300_ISR_EXT_GPIO_MODE0_ADDR, temp);
    }else if(reg_idx == RTL9300_EXT_GPIO_REG_SEL_1){
        ioal_mem32_read(unit, RTL9300_ISR_EXT_GPIO_MODE1_ADDR, &temp);
        REG_FIELD_SET(&temp, (intrMode), (pin_idx), (0x3 << (pin_idx)));
        /* write register to active the read operation */
        ioal_mem32_write(unit, RTL9300_ISR_EXT_GPIO_MODE1_ADDR, temp);

    }else if(reg_idx == RTL9300_EXT_GPIO_REG_SEL_2){
        ioal_mem32_read(unit, RTL9300_ISR_EXT_GPIO_MODE2_ADDR, &temp);
        REG_FIELD_SET(&temp, (intrMode), (pin_idx), (0x3 << (pin_idx)));
        /* write register to active the read operation */
        ioal_mem32_write(unit, RTL9300_ISR_EXT_GPIO_MODE2_ADDR, temp);
    }

    RTL8231_MDC_SEM_UNLOCK(unit);
    return RT_ERR_OK;
}

/* Function Name:
 *      r9300_rtl8231_intrMode_get
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
int32
r9300_rtl8231_pinIntrMode_get(uint32 unit, uint32 dev, uint32 pin, drv_extGpio_interruptType_t * pIntrMode)
{
    uint32  temp;
    uint32  reg_idx, pin_idx;

    /* check Init status */
    RT_INIT_CHK(rtl8231_init[unit]);
    if(pin >= RTL8231_GPIO_PIN_NUM_MAX){
        /* PIN No. is out of range */
        return RT_ERR_FAILED;
    }

    reg_idx = (uint32)((pin)/RTL9300_EXT_GPIO_FIELD_BIT2_REG0_SEL);
    pin_idx = (uint32)((pin)%RTL9300_EXT_GPIO_FIELD_BIT2_REG0_SEL);
    pin_idx = (pin_idx * RTL9300_EXT_GPIO_FIELD_BIT2_REG0_OFFSET);
    RTL8231_MDC_SEM_LOCK(unit);

    if(reg_idx == RTL9300_EXT_GPIO_REG_SEL_0)
    {
        ioal_mem32_read(unit, RTL9300_ISR_EXT_GPIO_MODE0_ADDR, &temp);
        *pIntrMode = (drv_gpio_interruptType_t)((temp&(0x3 << (pin_idx))) >> (pin_idx));
    }else if(reg_idx == RTL9300_EXT_GPIO_REG_SEL_1){
        ioal_mem32_read(unit, RTL9300_ISR_EXT_GPIO_MODE1_ADDR, &temp);
        *pIntrMode = (drv_gpio_interruptType_t)((temp&(0x3 << (pin_idx))) >> (pin_idx));
    }else if(reg_idx == RTL9300_EXT_GPIO_REG_SEL_2){
        ioal_mem32_read(unit, RTL9300_ISR_EXT_GPIO_MODE2_ADDR, &temp);
        *pIntrMode = (drv_gpio_interruptType_t)((temp&(0x3 << (pin_idx))) >> (pin_idx));
    }

    RTL8231_MDC_SEM_UNLOCK(unit);
    return RT_ERR_OK;
}


/* Function Name:
 *      r9300_rtl8231_extGPIOIntrStatus_get
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
int32
r9300_rtl8231_extGPIOIntrStatus_get(uint32 unit, uint32 dev, uint32 *pIsr)
{
    uint32  temp;
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(rtl8231_init[unit]);

    /* initialize variable */
    if((ret = ioal_mem32_read(unit, RTL9300_ISR_GLB_ADDR, &temp)) != RT_ERR_OK)
    {
        return ret;
    }

    RTL8231_MDC_SEM_LOCK(unit);
    *pIsr = (temp & RTL9300_ISR_GLB_ISR_GLB_EXT_GPIO_MASK) >> RTL9300_ISR_GLB_ISR_GLB_EXT_GPIO_OFFSET;
    RTL8231_MDC_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}


/* Function Name:
 *      r9300_rtl8231_pinIntrStatus_get
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
int32
r9300_rtl8231_pinIntrStatus_get(uint32 unit, uint32 dev, uint32 pin, uint32 *pIsr)
{
    uint32  temp;
    uint32  reg_idx, pin_idx;
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(rtl8231_init[unit]);

    /* initialize variable */
    if((ret = ioal_mem32_read(unit, RTL9300_ISR_GLB_ADDR, &temp)) != RT_ERR_OK)
    {
        return ret;
    }

    if(pin >= RTL8231_GPIO_PIN_NUM_MAX){
        /* PIN No. is out of range */
        return RT_ERR_FAILED;
    }

    reg_idx = (uint32)((pin)/RTL9300_EXT_GPIO_FIELD_BIT1_REG0_SEL);
    pin_idx = (uint32)((pin)%RTL9300_EXT_GPIO_FIELD_BIT1_REG0_SEL);

    if(reg_idx == RTL9300_EXT_GPIO_REG_SEL_0){
        if((ret = ioal_mem32_read(unit, RTL9300_ISR_EXT_GPIO0_ADDR, &temp)) != RT_ERR_OK)
            return ret;
    }else if(reg_idx == RTL9300_EXT_GPIO_REG_SEL_1){
        if((ret = ioal_mem32_read(unit, RTL9300_ISR_EXT_GPIO1_ADDR, &temp)) != RT_ERR_OK)
            return ret;
    }

    RTL8231_MDC_SEM_LOCK(unit);
    *pIsr = (temp & (0x1 << pin_idx)) >> pin_idx;
    RTL8231_MDC_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      r9300_rtl8231_pinIntrStatus_clear
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
int32
r9300_rtl8231_pinIntrStatus_clear(uint32 unit, uint32 dev, uint32 pin)
{
    uint32  temp;
    uint32  reg_idx, pin_idx;
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(rtl8231_init[unit]);

    if(pin >= RTL8231_GPIO_PIN_NUM_MAX){
        /* PIN No. is out of range */
        return RT_ERR_FAILED;
    }

    reg_idx = (uint32)((pin)/RTL9300_EXT_GPIO_FIELD_BIT1_REG0_SEL);
    pin_idx = (uint32)((pin)%RTL9300_EXT_GPIO_FIELD_BIT1_REG0_SEL);

    if(reg_idx == RTL9300_EXT_GPIO_REG_SEL_0){
        if((ret = ioal_mem32_read(unit, RTL9300_ISR_EXT_GPIO0_ADDR, &temp)) != RT_ERR_OK)
            return ret;
    }else if(reg_idx == RTL9300_EXT_GPIO_REG_SEL_1){
        if((ret = ioal_mem32_read(unit, RTL9300_ISR_EXT_GPIO1_ADDR, &temp)) != RT_ERR_OK)
            return ret;
    }

    RTL8231_MDC_SEM_LOCK(unit);
    temp= (0x1 << pin_idx);
    if(reg_idx == RTL9300_EXT_GPIO_REG_SEL_0){
        if((ret = ioal_mem32_write(unit, RTL9300_ISR_EXT_GPIO0_ADDR, temp)) != RT_ERR_OK)
        {
            RTL8231_MDC_SEM_UNLOCK(unit);
            return ret;
        }
    }else if(reg_idx == RTL9300_EXT_GPIO_REG_SEL_1){
        if((ret = ioal_mem32_write(unit, RTL9300_ISR_EXT_GPIO1_ADDR, temp)) != RT_ERR_OK)
        {
            RTL8231_MDC_SEM_UNLOCK(unit);
            return ret;
        }
    }
    RTL8231_MDC_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      r9300_rtl8231_directAccess_set
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
int32
r9300_rtl8231_directAccess_set(uint32 unit, uint32 address, drv_extGpio_directAccessMode_t mode, rtk_enable_t enable)
{
    uint32  temp = 0;

    /* check Init status */
    RT_INIT_CHK(rtl8231_init[unit]);

    RTL8231_MDC_SEM_LOCK(unit);

    /* initialize variable */

    ioal_mem32_read(unit, RTL9300_EXT_GPIO_GLB_CTRL_ADDR, &temp);

    /* Set Bit 3~2 : continuous mode */
    if(mode == EXT_GPIO_DIRECT_ACCESS_MODE_CONTINUOUS)
    {
        temp |= (address == 0) ? \
        (RTL9300_EXT_GPIO_GLB_CTRL_EXT_GPIO_MDX2_ACCESS_MODE_MASK) : \
        (RTL9300_EXT_GPIO_GLB_CTRL_EXT_GPIO_MDX3_ACCESS_MODE_MASK);
    }else{
        temp &= (address == 0) ? \
        (~(RTL9300_EXT_GPIO_GLB_CTRL_EXT_GPIO_MDX2_ACCESS_MODE_MASK)) : \
        (~(RTL9300_EXT_GPIO_GLB_CTRL_EXT_GPIO_MDX3_ACCESS_MODE_MASK));
    }

    /* Set Bit 1~0 : Enable Polling */
    if(enable == ENABLED)
    {
        temp |= (address == 0) ? \
        (RTL9300_EXT_GPIO_GLB_CTRL_EXT_GPIO_MDX2_EN_MASK) : \
        (RTL9300_EXT_GPIO_GLB_CTRL_EXT_GPIO_MDX3_EN_MASK);
    }else{
        temp &= (address == 0) ? \
        (~(RTL9300_EXT_GPIO_GLB_CTRL_EXT_GPIO_MDX2_EN_MASK)) : \
        (~(RTL9300_EXT_GPIO_GLB_CTRL_EXT_GPIO_MDX3_EN_MASK));
    }

    /* write register to active the read operation */
    ioal_mem32_write(unit, RTL9300_EXT_GPIO_GLB_CTRL_ADDR, temp);

    RTL8231_MDC_SEM_UNLOCK(unit);
    return RT_ERR_OK;
}
