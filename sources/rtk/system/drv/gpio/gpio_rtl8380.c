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
 * Purpose : Definition those public GPIO routing APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *           (1) Internal GPIO
 *
 */

/*
 * Include Files
 */
#include <common/debug/rt_log.h>
#include <ioal/mem32.h>
#include <soc/socdef/rtl8380_soc_reg.h>
#include <drv/gpio/gpio.h>
#include <hwp/hw_profile.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
#define GPIO_SHADOW_GPO
#if defined(GPIO_SHADOW_GPO)
static uint32  PABC_DAT_shadow;
static uint32  PABC_DAT_gpoMask;
#endif
static uint32  gpio_init = {INIT_NOT_COMPLETED};

/*
 * Macro Definition
 */


/*
 * Function Declaration
 */


/* Function Name:
 *      rtl8380_gpio_init
 * Description:
 *      GPIO driver initilization
 * Input:
 *      unit  - UNIT ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
rtl8380_gpio_init(uint32 unit)
{
    RT_INIT_REENTRY_CHK(gpio_init);

#if defined(GPIO_SHADOW_GPO)
    PABC_DAT_shadow = 0;
    PABC_DAT_gpoMask = 0;
#endif
    gpio_init = INIT_COMPLETED;

    return RT_ERR_OK;
}


/* Function Name:
 *      _rtl8380_shadow_gpoMask_set
 * Description:
 *      Set data mask for GPO pin
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
#if defined(GPIO_SHADOW_GPO)
static void
_rtl8380_shadow_gpoMask_set(drv_gpio_port_t port, uint32 pin, drv_gpio_direction_t direction)
{
    uint32      fOffset, fMask; /* field offset and mask */
    uint32      fVal;   /* field value */

    switch(port)
    {
      case GPIO_PORT_A:
        fOffset = RTL8380_GPIO_AB_DATA_PD_A_OFFSET;
        break;
      case GPIO_PORT_B:
        fOffset = RTL8380_GPIO_AB_DATA_PD_B_OFFSET;
        break;
      default:
        return;
    }/* switch */
    fOffset += pin;
    fMask = (GPIO_BITS_TO_MASK(1) << fOffset);

    if (direction == GPIO_DIR_OUT)
        fVal = 1;
    else
        fVal = 0;

    PABC_DAT_gpoMask = (PABC_DAT_gpoMask & ~fMask) | ((fVal << fOffset) & fMask);

}
#endif

/* Function Name:
 *      _rtl8380_gpio_imPinOffsetPortA_get
 * Description:
 *      Get Interrupt Mode (IM) Register "PA#_IM" field's offset and mask by pin ID.
 *      ("#" is pin number)
 * Input:
 *      pin - pin ID
 * Output:
 *      offset - offset of the field
 *      mask   - mask of the field
 * Return:
 *      None
 * Note:
 *      None
 */
void
_rtl8380_gpio_imPinOffsetPortA_get(uint32 pin, uint32 *offset, uint32 *mask)
{
    uint32      portA_im_offset[] = {RTL8380_GPIO_AB_INTR_MODE_PA0_IM_OFFSET, RTL8380_GPIO_AB_INTR_MODE_PA1_IM_OFFSET, RTL8380_GPIO_AB_INTR_MODE_PA2_IM_OFFSET, RTL8380_GPIO_AB_INTR_MODE_PA3_IM_OFFSET,
                                     RTL8380_GPIO_AB_INTR_MODE_PA4_IM_OFFSET, RTL8380_GPIO_AB_INTR_MODE_PA5_IM_OFFSET, RTL8380_GPIO_AB_INTR_MODE_PA6_IM_OFFSET, RTL8380_GPIO_AB_INTR_MODE_PA7_IM_OFFSET};
    uint32      portA_im_mask[]   = {RTL8380_GPIO_AB_INTR_MODE_PA0_IM_MASK,   RTL8380_GPIO_AB_INTR_MODE_PA1_IM_MASK,   RTL8380_GPIO_AB_INTR_MODE_PA2_IM_MASK,   RTL8380_GPIO_AB_INTR_MODE_PA3_IM_MASK,
                                     RTL8380_GPIO_AB_INTR_MODE_PA4_IM_MASK,   RTL8380_GPIO_AB_INTR_MODE_PA5_IM_MASK,   RTL8380_GPIO_AB_INTR_MODE_PA6_IM_MASK,   RTL8380_GPIO_AB_INTR_MODE_PA7_IM_MASK};
    *offset = portA_im_offset[pin];
    *mask = portA_im_mask[pin];
}

/* Function Name:
 *      _rtl8380_gpio_imPinOffsetPortB_get
 * Description:
 *      Get Interrupt Mode (IM) Register "PB#_IM" field's offset and mask by pin ID.
 *      ("#" is pin number)
 * Input:
 *      pin - pin ID
 * Output:
 *      offset - offset of the field
 *      mask   - mask of the field
 * Return:
 *      None
 * Note:
 *      None
 */
void
_rtl8380_gpio_imPinOffsetPortB_get(uint32 pin, uint32 *offset, uint32 *mask)
{
    uint32      portB_im_offset[] = {RTL8380_GPIO_AB_INTR_MODE_PB0_IM_OFFSET, RTL8380_GPIO_AB_INTR_MODE_PB1_IM_OFFSET, RTL8380_GPIO_AB_INTR_MODE_PB2_IM_OFFSET, RTL8380_GPIO_AB_INTR_MODE_PB3_IM_OFFSET,
                                     RTL8380_GPIO_AB_INTR_MODE_PB4_IM_OFFSET, RTL8380_GPIO_AB_INTR_MODE_PB5_IM_OFFSET, RTL8380_GPIO_AB_INTR_MODE_PB6_IM_OFFSET, RTL8380_GPIO_AB_INTR_MODE_PB7_IM_OFFSET};
    uint32      portB_im_mask[]   = {RTL8380_GPIO_AB_INTR_MODE_PB0_IM_MASK,   RTL8380_GPIO_AB_INTR_MODE_PB1_IM_MASK,   RTL8380_GPIO_AB_INTR_MODE_PB2_IM_MASK,   RTL8380_GPIO_AB_INTR_MODE_PB3_IM_MASK,
                                     RTL8380_GPIO_AB_INTR_MODE_PB4_IM_MASK,   RTL8380_GPIO_AB_INTR_MODE_PB5_IM_MASK,   RTL8380_GPIO_AB_INTR_MODE_PB6_IM_MASK,   RTL8380_GPIO_AB_INTR_MODE_PB7_IM_MASK};
    *offset = portB_im_offset[pin];
    *mask = portB_im_mask[pin];
}


/* Function Name:
 *      rtl8380_gpio_direction_set
 * Description:
 *      Set pin's direction as input or output
 * Input:
 *      unit      - UNIT ID
 *      port      - GPIO port ID
 *      pin       - pin ID
 *      direction - input or output
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      RT_ERR_OK           - configure success.
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range.
 */
int32
rtl8380_gpio_direction_set(
    uint32 unit,
    drv_gpio_port_t port,
    uint32 pin,
    drv_gpio_direction_t direction)
{
    uint32      fOffset, fMask; /* field offset and mask */
    uint32      regData;
    uint32      fVal;   /* field value */
    int32       ret;

    switch(port)
    {
      case GPIO_PORT_A:
        fOffset = RTL8380_GPIO_AB_DIR_DRC_A_OFFSET;
        break;
      case GPIO_PORT_B:
        fOffset = RTL8380_GPIO_AB_DIR_DRC_B_OFFSET;
        break;
      default:
        return RT_ERR_FAILED;
    }/* switch */
    fOffset += pin;
    fMask = GPIO_BITS_TO_MASK(1) << fOffset;

    switch (direction)
    {
      case GPIO_DIR_IN:
        fVal = 0;
        break;
      case GPIO_DIR_OUT:
        fVal = 1;
        break;
      default:
        return RT_ERR_FAILED;
    }

    if ((ret = ioal_soc_mem32_read(unit, RTL8380_GPIO_AB_DIR_ADDR, &regData)) != RT_ERR_OK)
    {
        return ret;
    }

    regData &= ~fMask;
    regData |= (fVal << fOffset) & fMask;

    if ((ret = ioal_soc_mem32_write(unit, RTL8380_GPIO_AB_DIR_ADDR, regData)) != RT_ERR_OK)
    {
        return ret;
    }

#if defined(GPIO_SHADOW_GPO)
    _rtl8380_shadow_gpoMask_set(port, pin, direction);
#endif

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl8380_gpio_control_set
 * Description:
 *      Set pin as GPIO-pin or dedicated-peripheral-pin
 * Input:
 *      unit      - UNIT ID
 *      port      - GPIO port ID
 *      pin       - pin ID
 *      ctrl      - GPIO (Normal) or peripheral
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      RT_ERR_OK           - configure success.
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range.
 */
int32
rtl8380_gpio_control_set(
    uint32 unit,
    drv_gpio_port_t port,
    uint32 pin,
    drv_gpio_control_t ctrl)
{
    uint32      fOffset, fMask; /* field offset and mask */
    uint32      regData;
    uint32      fVal;   /* field value */
    int32       ret;

    switch(port)
    {
      case GPIO_PORT_A:
        fOffset = RTL8380_GPIO_AB_CTRL_PFC_A_OFFSET;
        break;
      case GPIO_PORT_B:
        fOffset = RTL8380_GPIO_AB_CTRL_PFC_B_OFFSET;
        break;
      default:
        return RT_ERR_FAILED;
    }/* switch */
    fOffset += pin;
    fMask = GPIO_BITS_TO_MASK(1) << fOffset;

    switch (ctrl)
    {
      case GPIO_CTRLFUNC_NORMAL:
        fVal = 0;
        break;
      case GPIO_CTRLFUNC_DEDICATE_PERIPHERAL:
        fVal = 1;
        break;
      default:
        return RT_ERR_FAILED;
    }

    if ((ret = ioal_soc_mem32_read(unit, RTL8380_GPIO_AB_CTRL_ADDR, &regData)) != RT_ERR_OK)
    {
        return ret;
    }

    regData &= ~fMask;
    regData |= (fVal << fOffset) & fMask;

    if ((ret = ioal_soc_mem32_write(unit, RTL8380_GPIO_AB_CTRL_ADDR, regData)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      rtl8380_gpio_intrMode_set
 * Description:
 *      Set GPIO pin's interrupt mode
 * Input:
 *      unit      - UNIt ID
 *      port      - GPIO port ID
 *      pin       - pin ID
 *      intrMode  - disable interrupt, enable falling edge, enable rising edge,
 *                  or enable both falling and rising edge.
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      RT_ERR_OK           - configure success.
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range.
 */
int32
rtl8380_gpio_intrMode_set(
    uint32 unit,
    drv_gpio_port_t port,
    uint32 pin,
    drv_gpio_interruptType_t intrMode)
{
    uint32      regAddr;
    uint32      fOffset, fMask; /* field offset and mask */
    uint32      regData;
    uint32      fVal;
    int32       ret;

    switch(port)
    {
      case GPIO_PORT_A:
        regAddr = RTL8380_GPIO_AB_INTR_MODE_ADDR;
        _rtl8380_gpio_imPinOffsetPortA_get(pin, &fOffset, &fMask);
        break;
      case GPIO_PORT_B:
        regAddr = RTL8380_GPIO_AB_INTR_MODE_ADDR;
        _rtl8380_gpio_imPinOffsetPortB_get(pin, &fOffset, &fMask);
        break;
      default:
        return RT_ERR_FAILED;
    }/* switch */

    switch(intrMode)
    {
      case GPIO_INT_DISABLE:
        fVal = 0;
        break;
      case GPIO_INT_FALLING_EDGE:
        fVal = 0x1;
        break;
      case GPIO_INT_RISING_EDGE:
        fVal = 0x2;
        break;
      case GPIO_INT_BOTH_EDGE:
        fVal = 0x3;
        break;
      default:
        return RT_ERR_FAILED;
    }

    if ((ret = ioal_soc_mem32_read(unit, regAddr, &regData)) != RT_ERR_OK)
    {
        return ret;
    }
    regData &= ~fMask;
    regData |= (fVal << fOffset) & fMask;
    if ((ret = ioal_soc_mem32_write(unit, regAddr, regData)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl8380_gpio_dataBit_init
 * Description:
 *      Initialize the bit value of a specified GPIO ID
 * Input:
 *      unit      - UNIT ID
 *      port      - GPIO port ID
 *      pin       - pin ID
 *      data      - 1 or 0
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Only the GPO pin need to call the API to init default value.
 */
int32
rtl8380_gpio_dataBit_init(uint32 unit, drv_gpio_port_t port, uint32 pin, uint32 data)
{
#if defined(GPIO_SHADOW_GPO)
    uint32      fOffset, fMask; /* field offset and mask */
    uint32      fVal;   /* field value */

    switch(port)
    {
      case GPIO_PORT_A:
        fOffset = RTL8380_GPIO_AB_DATA_PD_A_OFFSET;
        break;
      case GPIO_PORT_B:
        fOffset = RTL8380_GPIO_AB_DATA_PD_B_OFFSET;
        break;
      default:
        return RT_ERR_FAILED;
    }/* switch */
    fOffset += pin;
    fMask = (GPIO_BITS_TO_MASK(1) << fOffset);

    if (data)
        fVal = 1;
    else
        fVal = 0;

    PABC_DAT_shadow = (PABC_DAT_shadow & ~fMask) | ((fVal << fOffset) & fMask);
    PABC_DAT_gpoMask |= 1 << fOffset;
#endif

    return RT_ERR_OK;
} /* end of rtl8390_gpio_dataBit_init */

/* Function Name:
 *      rtl8380_gpio_dataBit_get
 * Description:
 *      Get GPIO pin's data
 * Input:
 *      unit      - UNIT ID
 *      port      - GPIO port ID
 *      pin       - pin ID
 * Output:
 *      pData     - 1 or 0
 * Return:
 *      None
 * Note:
 *      RT_ERR_OK           - configure success.
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range.
 */
int32
rtl8380_gpio_dataBit_get(uint32 unit, drv_gpio_port_t port, uint32 pin, uint32 *pData)
{
    uint32      fOffset, fMask; /* field offset and mask */
    uint32      regData;
    int32       ret;

    switch(port)
    {
      case GPIO_PORT_A:
        fOffset = RTL8380_GPIO_AB_DATA_PD_A_OFFSET;
        break;
      case GPIO_PORT_B:
        fOffset = RTL8380_GPIO_AB_DATA_PD_B_OFFSET;
        break;
      default:
        return RT_ERR_FAILED;
    }/* switch */
    fOffset += pin;
    fMask = (GPIO_BITS_TO_MASK(1) << fOffset);

    if ((ret = ioal_soc_mem32_read(unit, RTL8380_GPIO_AB_DATA_ADDR, &regData)) != RT_ERR_OK)
    {
        return ret;
    }

#if defined(GPIO_SHADOW_GPO)
    /* recover GPO pin data */
    regData = (regData & ~PABC_DAT_gpoMask) | (PABC_DAT_shadow & PABC_DAT_gpoMask);
#endif

    *pData = (regData & fMask) >> fOffset;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl8380_gpio_dataBit_set
 * Description:
 *      Set GPIO pin's data as 1 or 0
 * Input:
 *      unit      - UNIT ID
 *      port      - GPIO port ID
 *      pin       - pin ID
 *      data      - 1 or 0
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      RT_ERR_OK           - configure success.
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range.
 */
int32
rtl8380_gpio_dataBit_set(uint32 unit, drv_gpio_port_t port, uint32 pin, uint32 data)
{
    uint32      fOffset, fMask; /* field offset and mask */
    uint32      regData;
    uint32      fVal;   /* field value */
    int32       ret;

    switch(port)
    {
      case GPIO_PORT_A:
        fOffset = RTL8380_GPIO_AB_DATA_PD_A_OFFSET;
        break;
      case GPIO_PORT_B:
        fOffset = RTL8380_GPIO_AB_DATA_PD_B_OFFSET;
        break;
      default:
        return RT_ERR_FAILED;
    }/* switch */
    fOffset += pin;
    fMask = (GPIO_BITS_TO_MASK(1) << fOffset);

    if (data)
        fVal = 1;
    else
        fVal = 0;

    if ((ret = ioal_soc_mem32_read(unit, RTL8380_GPIO_AB_DATA_ADDR, &regData)) != RT_ERR_OK)
    {
        return ret;
    }

#if defined(GPIO_SHADOW_GPO)
    /* recover GPO pin data */
    regData = (regData & ~PABC_DAT_gpoMask) | (PABC_DAT_shadow & PABC_DAT_gpoMask);
#endif

    regData &= ~fMask;
    regData |= (fVal << fOffset) & fMask;

    if ((ret = ioal_soc_mem32_write(unit, RTL8380_GPIO_AB_DATA_ADDR, regData)) != RT_ERR_OK)
    {
        return ret;
    }

#if defined(GPIO_SHADOW_GPO)
    /* record pin data */
    PABC_DAT_shadow = (PABC_DAT_shadow & ~fMask) | ((fVal << fOffset) & fMask);
#endif

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl8380_gpio_intrStatus_get
 * Description:
 *      Get GPIO pin's interrupt status
 * Input:
 *      unit      - UNIT ID
 *      port      - GPIO port ID
 *      pin       - pin ID
 * Output:
 *      pIst      - Interrupt status
 * Return:
 *      None
 * Note:
 *      RT_ERR_OK           - configure success.
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range.
 */
int32
rtl8380_gpio_intrStatus_get(uint32 unit, drv_gpio_port_t port, uint32 pin, uint32 *pIsr)
{
    uint32      fOffset, fMask; /* field offset and mask */
    uint32      regData;
    int32       ret;

    switch(port)
    {
      case GPIO_PORT_A:
        fOffset = RTL8380_GPIO_AB_INTR_STS_IPS_A_OFFSET;
        break;
      case GPIO_PORT_B:
        fOffset = RTL8380_GPIO_AB_INTR_STS_IPS_B_OFFSET;
        break;
      default:
        return RT_ERR_FAILED;
    }/* switch */
    fOffset += pin;
    fMask = (GPIO_BITS_TO_MASK(1) << fOffset);

    if ((ret = ioal_soc_mem32_read(unit, RTL8380_GPIO_AB_INTR_STS_ADDR, &regData)) != RT_ERR_OK)
    {
        return ret;
    }

    *pIsr = (regData & fMask) >> fOffset;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl8380_gpio_intrStatus_clear
 * Description:
 *      Clear GPIO pin's interrupt status
 * Input:
 *      unit      - UNIT ID
 *      port      - GPIO port ID
 *      pin       - pin ID
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      RT_ERR_OK           - configure success.
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range.
 */
int32
rtl8380_gpio_intrStatus_clear(uint32 unit, drv_gpio_port_t port, uint32 pin)
{
    uint32      fOffset, fMask; /* field offset and mask */
    uint32      regData;
    int32       ret;

    switch(port)
    {
      case GPIO_PORT_A:
        fOffset = RTL8380_GPIO_AB_INTR_STS_IPS_A_OFFSET;
        break;
      case GPIO_PORT_B:
        fOffset = RTL8380_GPIO_AB_INTR_STS_IPS_B_OFFSET;
        break;
      default:
        return RT_ERR_FAILED;
    }/* switch */
    fOffset += pin;
    fMask = (GPIO_BITS_TO_MASK(1) << fOffset);

    /* write "1" to clear the interrupt */
    regData = 0;
    regData |= (1 << fOffset) & fMask;

    if ((ret = ioal_soc_mem32_write(unit, RTL8380_GPIO_AB_INTR_STS_ADDR, regData)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl8380_gpio_portRange_get
 * Description:
 *      Get GPIO port range
 * Input:
 *      unit      - UNIt ID
 *      minPort   - minimum GPIO port number
 *      maxPort   - maximum GPIO port number
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      RT_ERR_OK           - get success.
 */
int32
rtl8380_gpio_portRange_get(uint32 unit, int32 *minPort, int32 *maxPort)
{
    *minPort = GPIO_PORT_A;
    *maxPort = GPIO_PORT_B;
    return RT_ERR_OK;
}

