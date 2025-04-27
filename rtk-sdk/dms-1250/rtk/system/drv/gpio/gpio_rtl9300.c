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
#include <soc/socdef/rtl9300_soc_reg.h>
#include <drv/gpio/gpio.h>
#include <hwp/hw_profile.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
static uint8    gpio_init = {INIT_NOT_COMPLETED};


/*
 * Macro Definition
 */


/*
 * Function Declaration
 */


/* Function Name:
 *      _rtl9300_gpio_intrEnable_init
 * Description:
 *      Interrupt enable initialization
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void
_rtl9300_gpio_intrEnable_init(uint32 unit)
{
    uint32      regData;
    int32       ret;

    /* enable all */
    regData = RTL9300_PABCD_C0_IER_PA_IE_MASK | RTL9300_PABCD_C0_IER_PB_IE_MASK | RTL9300_PABCD_C0_IER_PC_IE_MASK;

    if ((ret = ioal_soc_mem32_write(unit, RTL9300_PABCD_C0_IER_ADDR, regData)) != RT_ERR_OK)
    {
        RT_ERR(ret, MOD_RTCORE, "GPIO interrupt enable init fail.");
    }
}


/* Function Name:
 *      _rtl9300_gpio_imPinOffsetPortA_get
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
_rtl9300_gpio_imPinOffsetPortA_get(uint32 pin, uint32 *offset, uint32 *mask)
{
    uint32      portA_im_offset[] = {RTL9300_PAB_IMR_PA0_IM_OFFSET, RTL9300_PAB_IMR_PA1_IM_OFFSET, RTL9300_PAB_IMR_PA2_IM_OFFSET, RTL9300_PAB_IMR_PA3_IM_OFFSET,
                                     RTL9300_PAB_IMR_PA4_IM_OFFSET, RTL9300_PAB_IMR_PA5_IM_OFFSET, RTL9300_PAB_IMR_PA6_IM_OFFSET, RTL9300_PAB_IMR_PA7_IM_OFFSET};
    uint32      portA_im_mask[] =   {RTL9300_PAB_IMR_PA0_IM_MASK,   RTL9300_PAB_IMR_PA1_IM_MASK,   RTL9300_PAB_IMR_PA2_IM_MASK,   RTL9300_PAB_IMR_PA3_IM_MASK,
                                     RTL9300_PAB_IMR_PA4_IM_MASK,   RTL9300_PAB_IMR_PA5_IM_MASK,   RTL9300_PAB_IMR_PA6_IM_MASK,   RTL9300_PAB_IMR_PA7_IM_MASK};
    *offset = portA_im_offset[pin];
    *mask   = portA_im_mask[pin];
}

/* Function Name:
 *      _rtl9300_gpio_imPinOffsetPortB_get
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
_rtl9300_gpio_imPinOffsetPortB_get(uint32 pin, uint32 *offset, uint32 *mask)
{
    uint32      portB_im_offset[] = {RTL9300_PAB_IMR_PB0_IM_OFFSET, RTL9300_PAB_IMR_PB1_IM_OFFSET, RTL9300_PAB_IMR_PB2_IM_OFFSET, RTL9300_PAB_IMR_PB3_IM_OFFSET,
                                     RTL9300_PAB_IMR_PB4_IM_OFFSET, RTL9300_PAB_IMR_PB5_IM_OFFSET, RTL9300_PAB_IMR_PB6_IM_OFFSET, RTL9300_PAB_IMR_PB7_IM_OFFSET};
    uint32      portB_im_mask[]   = {RTL9300_PAB_IMR_PB0_IM_MASK,   RTL9300_PAB_IMR_PB1_IM_MASK,   RTL9300_PAB_IMR_PB2_IM_MASK,   RTL9300_PAB_IMR_PB3_IM_MASK,
                                     RTL9300_PAB_IMR_PB4_IM_MASK,   RTL9300_PAB_IMR_PB5_IM_MASK,   RTL9300_PAB_IMR_PB6_IM_MASK,   RTL9300_PAB_IMR_PB7_IM_MASK};
    *offset = portB_im_offset[pin];
    *mask   = portB_im_mask[pin];
}

/* Function Name:
 *      _rtl9300_gpio_imPinOffsetPortC_get
 * Description:
 *      Get Interrupt Mode (IM) Register "PC#_IM" field's offset and mask by pin ID.
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
_rtl9300_gpio_imPinOffsetPortC_get(uint32 pin, uint32 *offset, uint32 *mask)
{
    uint32      portC_im_offset[] = {RTL9300_PCD_IMR_PC0_IM_OFFSET, RTL9300_PCD_IMR_PC1_IM_OFFSET, RTL9300_PCD_IMR_PC2_IM_OFFSET, RTL9300_PCD_IMR_PC3_IM_OFFSET,
                                     RTL9300_PCD_IMR_PC4_IM_OFFSET, RTL9300_PCD_IMR_PC5_IM_OFFSET, RTL9300_PCD_IMR_PC6_IM_OFFSET, RTL9300_PCD_IMR_PC7_IM_OFFSET};
    uint32      portC_im_mask[]   = {RTL9300_PCD_IMR_PC0_IM_MASK,   RTL9300_PCD_IMR_PC1_IM_MASK,   RTL9300_PCD_IMR_PC2_IM_MASK,   RTL9300_PCD_IMR_PC3_IM_MASK,
                                     RTL9300_PCD_IMR_PC4_IM_MASK,   RTL9300_PCD_IMR_PC5_IM_MASK,   RTL9300_PCD_IMR_PC6_IM_MASK,   RTL9300_PCD_IMR_PC7_IM_MASK};
    *offset = portC_im_offset[pin];
    *mask   = portC_im_mask[pin];
}


/* Function Name:
 *      rtl9300_gpio_init
 * Description:
 *      GPIO driver initilization
 * Input:
 *      unit    - UNIT ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
rtl9300_gpio_init(uint32 unit)
{
    RT_INIT_REENTRY_CHK(gpio_init);

    _rtl9300_gpio_intrEnable_init(unit);

    gpio_init = INIT_COMPLETED;
    return RT_ERR_OK;
}

/* Function Name:
 *      rtl9300_gpio_direction_set
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
rtl9300_gpio_direction_set(
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
        fOffset = RTL9300_PABCD_DIR_DRC_A_OFFSET;
        break;
      case GPIO_PORT_B:
        fOffset = RTL9300_PABCD_DIR_DRC_B_OFFSET;
        break;
      case GPIO_PORT_C:
        fOffset = RTL9300_PABCD_DIR_DRC_C_OFFSET;
        break;
      default:
        return RT_ERR_OUT_OF_RANGE;
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
        return RT_ERR_OUT_OF_RANGE;
    }

    if ((ret = ioal_soc_mem32_read(unit, RTL9300_PABCD_DIR_ADDR, &regData)) != RT_ERR_OK)
    {
        return ret;
    }

    regData &= ~fMask;
    regData |= (fVal << fOffset) & fMask;

    if ((ret = ioal_soc_mem32_write(unit, RTL9300_PABCD_DIR_ADDR, regData)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      rtl9300_gpio_intrMode_set
 * Description:
 *      Set GPIO pin's interrupt mode
 * Input:
 *      unit      - UNIT ID
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
rtl9300_gpio_intrMode_set(
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
        regAddr = RTL9300_PAB_IMR_ADDR;
        _rtl9300_gpio_imPinOffsetPortA_get(pin, &fOffset, &fMask);
        break;
      case GPIO_PORT_B:
        regAddr = RTL9300_PAB_IMR_ADDR;
        _rtl9300_gpio_imPinOffsetPortB_get(pin, &fOffset, &fMask);
        break;
      case GPIO_PORT_C:
        regAddr = RTL9300_PCD_IMR_ADDR;
        _rtl9300_gpio_imPinOffsetPortC_get(pin, &fOffset, &fMask);
        break;
      default:
        return RT_ERR_OUT_OF_RANGE;
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
        return RT_ERR_OUT_OF_RANGE;
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
 *      rtl9300_gpio_dataBit_get
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
rtl9300_gpio_dataBit_get(uint32 unit, drv_gpio_port_t port, uint32 pin, uint32 *pData)
{
    uint32      fOffset, fMask; /* field offset and mask */
    uint32      regData;
    int32       ret;

    switch(port)
    {
      case GPIO_PORT_A:
        fOffset = RTL9300_PABCD_DAT_PD_A_OFFSET;
        break;
      case GPIO_PORT_B:
        fOffset = RTL9300_PABCD_DAT_PD_B_OFFSET;
        break;
      case GPIO_PORT_C:
        fOffset = RTL9300_PABCD_DAT_PD_C_OFFSET;
        break;
      default:
        return RT_ERR_OUT_OF_RANGE;
    }/* switch */
    fOffset += pin;
    fMask = (GPIO_BITS_TO_MASK(1) << fOffset);

    if ((ret = ioal_soc_mem32_read(unit, RTL9300_PABCD_DAT_ADDR, &regData)) != RT_ERR_OK)
    {
        return ret;
    }

    *pData = (regData & fMask) >> fOffset;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl9300_gpio_dataBit_set
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
rtl9300_gpio_dataBit_set(uint32 unit, drv_gpio_port_t port, uint32 pin, uint32 data)
{
    uint32      fOffset, fMask; /* field offset and mask */
    uint32      regData;
    uint32      fVal;   /* field value */
    int32       ret;

    switch(port)
    {
      case GPIO_PORT_A:
        fOffset = RTL9300_PABCD_DAT_PD_A_OFFSET;
        break;
      case GPIO_PORT_B:
        fOffset = RTL9300_PABCD_DAT_PD_B_OFFSET;
        break;
      case GPIO_PORT_C:
        fOffset = RTL9300_PABCD_DAT_PD_C_OFFSET;
        break;
      default:
        return RT_ERR_OUT_OF_RANGE;
    }/* switch */
    fOffset += pin;
    fMask = (GPIO_BITS_TO_MASK(1) << fOffset);

    if (data)
        fVal = 1;
    else
        fVal = 0;

    if ((ret = ioal_soc_mem32_read(unit, RTL9300_PABCD_DAT_ADDR, &regData)) != RT_ERR_OK)
    {
        return ret;
    }

    regData &= ~fMask;
    regData |= (fVal << fOffset) & fMask;

    if ((ret = ioal_soc_mem32_write(unit, RTL9300_PABCD_DAT_ADDR, regData)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl9300_gpio_intrStatus_get
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
rtl9300_gpio_intrStatus_get(uint32 unit, drv_gpio_port_t port, uint32 pin, uint32 *pIsr)
{
    uint32      fOffset, fMask; /* field offset and mask */
    uint32      regData;
    int32       ret;

    switch(port)
    {
      case GPIO_PORT_A:
        fOffset = RTL9300_PABCD_ISR_IPS_A_OFFSET;
        break;
      case GPIO_PORT_B:
        fOffset = RTL9300_PABCD_ISR_IPS_B_OFFSET;
        break;
      case GPIO_PORT_C:
        fOffset = RTL9300_PABCD_ISR_IPS_C_OFFSET;
        break;
      default:
        return RT_ERR_OUT_OF_RANGE;
    }/* switch */
    fOffset += pin;
    fMask = (GPIO_BITS_TO_MASK(1) << fOffset);

    if ((ret = ioal_soc_mem32_read(unit, RTL9300_PABCD_ISR_ADDR, &regData)) != RT_ERR_OK)
    {
        return ret;
    }

    *pIsr = (regData & fMask) >> fOffset;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl9300_gpio_intrStatus_clear
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
rtl9300_gpio_intrStatus_clear(uint32 unit, drv_gpio_port_t port, uint32 pin)
{
    uint32      fOffset, fMask; /* field offset and mask */
    uint32      regData;
    int32       ret;

    switch(port)
    {
      case GPIO_PORT_A:
        fOffset = RTL9300_PABCD_ISR_IPS_A_OFFSET;
        break;
      case GPIO_PORT_B:
        fOffset = RTL9300_PABCD_ISR_IPS_B_OFFSET;
        break;
      case GPIO_PORT_C:
        fOffset = RTL9300_PABCD_ISR_IPS_C_OFFSET;
        break;
      default:
        return RT_ERR_OUT_OF_RANGE;
    }/* switch */
    fOffset += pin;
    fMask = (GPIO_BITS_TO_MASK(1) << fOffset);

    /* write "1" to clear the interrupt */
    regData = 0;
    regData |= (1 << fOffset) & fMask;

    if ((ret = ioal_soc_mem32_write(unit, RTL9300_PABCD_ISR_ADDR, regData)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      rtl9300_gpio_portRange_get
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
rtl9300_gpio_portRange_get(uint32 unit, int32 *minPort, int32 *maxPort)
{
    *minPort = GPIO_PORT_A;
    *maxPort = GPIO_PORT_C;
    return RT_ERR_OK;
}

