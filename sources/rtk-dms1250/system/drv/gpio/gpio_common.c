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
 * $Revision: 81459 $
 * $Date: 2017-08-17 16:21:37 +0800 (Thu, 17 Aug 2017) $
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
#include <soc/socdef/rtl9310_soc_reg.h>
#include <drv/gpio/gpio.h>
#include <private/drv/swcore/swcore.h>
#include <private/drv/swcore/chip_probe.h>
#include <private/drv/gpio/gpio_mapper.h>
#include <private/drv/gpio/gpio_common.h>
#include <hwp/hw_profile.h>

/*
 * Symbol Definition
 */

/* register address and register offset */
typedef struct gpio_portAddrOffset_s
{
    uint32      addr;
    uint32      offset;
} gpio_portAddrOffset_t;

/* GPIO per port registers address and register field offset */
typedef struct gpio_portReg_s
{
    gpio_portAddrOffset_t   cnr;
    gpio_portAddrOffset_t   dir;
    gpio_portAddrOffset_t   dat;
    gpio_portAddrOffset_t   isr;
    gpio_portAddrOffset_t   imr;
} gpio_portReg_t;

/* GPIO register definition */
typedef struct gpio_regDef_s
{
    gpio_portReg_t      *pPortReg;
    uint32              portMax;
} gpio_regDef_t;


/*
 * Data Declaration
 */
static uint8    gpio_init = {INIT_NOT_COMPLETED};

#if defined(CONFIG_SDK_RTL9310)
#define GPIO_9310_PORT_MAX          (GPIO_PORT_D+1)
static gpio_portReg_t   gpio_9310_portReg[GPIO_9310_PORT_MAX] =
        {
            [GPIO_PORT_A] = { .cnr.addr = RTL9310_PABCD_CNR_ADDR, .cnr.offset = RTL9310_PABCD_CNR_PFC_A_OFFSET,
                              .dir.addr = RTL9310_PABCD_DIR_ADDR, .dir.offset = RTL9310_PABCD_DIR_DRC_A_OFFSET,
                              .dat.addr = RTL9310_PABCD_DAT_ADDR, .dat.offset = RTL9310_PABCD_DAT_PD_A_OFFSET,
                              .isr.addr = RTL9310_PABCD_ISR_ADDR, .isr.offset = RTL9310_PABCD_ISR_IPS_A_OFFSET,
                              .imr.addr = RTL9310_PAB_IMR_ADDR,   .imr.offset = RTL9310_PAB_IMR_PA0_IM_OFFSET,
                            },
            [GPIO_PORT_B] = { .cnr.addr = RTL9310_PABCD_CNR_ADDR, .cnr.offset = RTL9310_PABCD_CNR_PFC_B_OFFSET,
                              .dir.addr = RTL9310_PABCD_DIR_ADDR, .dir.offset = RTL9310_PABCD_DIR_DRC_B_OFFSET,
                              .dat.addr = RTL9310_PABCD_DAT_ADDR, .dat.offset = RTL9310_PABCD_DAT_PD_B_OFFSET,
                              .isr.addr = RTL9310_PABCD_ISR_ADDR, .isr.offset = RTL9310_PABCD_ISR_IPS_B_OFFSET,
                              .imr.addr = RTL9310_PAB_IMR_ADDR,   .imr.offset = RTL9310_PAB_IMR_PB0_IM_OFFSET,
                            },
            [GPIO_PORT_C] = { .cnr.addr = RTL9310_PABCD_CNR_ADDR, .cnr.offset = RTL9310_PABCD_CNR_PFC_C_OFFSET,
                              .dir.addr = RTL9310_PABCD_DIR_ADDR, .dir.offset = RTL9310_PABCD_DIR_DRC_C_OFFSET,
                              .dat.addr = RTL9310_PABCD_DAT_ADDR, .dat.offset = RTL9310_PABCD_DAT_PD_C_OFFSET,
                              .isr.addr = RTL9310_PABCD_ISR_ADDR, .isr.offset = RTL9310_PABCD_ISR_IPS_C_OFFSET,
                              .imr.addr = RTL9310_PCD_IMR_ADDR,   .imr.offset = RTL9310_PCD_IMR_PC0_IM_OFFSET,
                            },
            [GPIO_PORT_D] = { .cnr.addr = RTL9310_PABCD_CNR_ADDR, .cnr.offset = RTL9310_PABCD_CNR_PFC_D_OFFSET,
                              .dir.addr = RTL9310_PABCD_DIR_ADDR, .dir.offset = RTL9310_PABCD_DIR_DRC_D_OFFSET,
                              .dat.addr = RTL9310_PABCD_DAT_ADDR, .dat.offset = RTL9310_PABCD_DAT_PD_D_OFFSET,
                              .isr.addr = RTL9310_PABCD_ISR_ADDR, .isr.offset = RTL9310_PABCD_ISR_IPS_D_OFFSET,
                              .imr.addr = RTL9310_PCD_IMR_ADDR,   .imr.offset = RTL9310_PCD_IMR_PD0_IM_OFFSET,
                            },
        };
#endif /* CONFIG_SDK_RTL9310 */


gpio_regDef_t gpio_reg[GPIO_CTRL_END] =
{
#if (defined(CONFIG_SDK_RTL9310))
  [GPIO_R9310] =
    {
        .pPortReg           = gpio_9310_portReg,
        .portMax            = GPIO_9310_PORT_MAX,
    },
#endif /* defined(CONFIG_SDK_RTL9300) */
#if (defined(CONFIG_SDK_RTL9300))
  [GPIO_R9300] =
    {
        .pPortReg           = NULL,
        .portMax            = 0,
    },
#endif /* defined(CONFIG_SDK_RTL9300) */
#if (defined(CONFIG_SDK_RTL8390))
  [GPIO_R8390] =
    {
        .pPortReg           = NULL,
        .portMax            = 0,
    },
#endif/* defined(CONFIG_SDK_RTL8390) */
#if (defined(CONFIG_SDK_RTL8380))
  [GPIO_R8380] =
    {
        .pPortReg           = NULL,
        .portMax            = 0,
    },
#endif/* defined(CONFIG_SDK_RTL8380) */
};


#define GPIO_REG(unit)      gpio_reg[gpio_if[unit]]
#define GPIO_COM_CHK(unit)  (GPIO_CHK(unit) && (GPIO_REG(unit).pPortReg != NULL))

/*
 * Macro Declaration
 */


/*
 * Function Declaration
 */


/* Function Name:
 *      gpio_common_init
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
gpio_common_init(uint32 unit)
{
    RT_INIT_REENTRY_CHK(gpio_init);

    gpio_init = INIT_COMPLETED;
    return RT_ERR_OK;
}

/* Function Name:
 *      gpio_common_direction_set
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
gpio_common_direction_set(
    uint32 unit,
    drv_gpio_port_t port,
    uint32 pin,
    drv_gpio_direction_t direction)
{
    uint32      regAddr, fOffset, fMask; /* field offset and mask */
    uint32      regData;
    uint32      fVal;   /* field value */
    int32       ret;

    if (!GPIO_COM_CHK(unit))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (port > GPIO_REG(unit).portMax)
    {
        return RT_ERR_OUT_OF_RANGE;
    }

    regAddr = GPIO_REG(unit).pPortReg[port].dir.addr;
    fOffset = GPIO_REG(unit).pPortReg[port].dir.offset + pin;
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
 *      gpio_common_intrMode_set
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
gpio_common_intrMode_set(
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

    if (!GPIO_COM_CHK(unit))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (port > GPIO_REG(unit).portMax)
    {
        return RT_ERR_OUT_OF_RANGE;
    }

    regAddr = GPIO_REG(unit).pPortReg[port].imr.addr;
    fOffset = GPIO_REG(unit).pPortReg[port].imr.offset + (pin * 2);
    fMask = 0x3 << fOffset;

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
 *      gpio_common_dataBit_get
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
gpio_common_dataBit_get(uint32 unit, drv_gpio_port_t port, uint32 pin, uint32 *pData)
{
    uint32      regAddr, fOffset, fMask; /* field offset and mask */
    uint32      regData;
    int32       ret;

    if (!GPIO_COM_CHK(unit))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (port > GPIO_REG(unit).portMax)
    {
        return RT_ERR_OUT_OF_RANGE;
    }

    regAddr = GPIO_REG(unit).pPortReg[port].dat.addr;
    fOffset = GPIO_REG(unit).pPortReg[port].dat.offset + pin;
    fMask = (GPIO_BITS_TO_MASK(1) << fOffset);

    if ((ret = ioal_soc_mem32_read(unit, regAddr, &regData)) != RT_ERR_OK)
    {
        return ret;
    }

    *pData = (regData & fMask) >> fOffset;

    return RT_ERR_OK;
}


/* Function Name:
 *      gpio_common_dataBit_set
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
gpio_common_dataBit_set(uint32 unit, drv_gpio_port_t port, uint32 pin, uint32 data)
{
    uint32      regAddr, fOffset, fMask; /* field offset and mask */
    uint32      regData;
    uint32      fVal;   /* field value */
    int32       ret;

    if (!GPIO_COM_CHK(unit))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (port > GPIO_REG(unit).portMax)
    {
        return RT_ERR_OUT_OF_RANGE;
    }

    regAddr = GPIO_REG(unit).pPortReg[port].dat.addr;
    fOffset = GPIO_REG(unit).pPortReg[port].dat.offset + pin;
    fMask = (GPIO_BITS_TO_MASK(1) << fOffset);

    if (data)
        fVal = 1;
    else
        fVal = 0;

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
 *      gpio_common_intrStatus_get
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
gpio_common_intrStatus_get(uint32 unit, drv_gpio_port_t port, uint32 pin, uint32 *pIsr)
{
    uint32      regAddr, fOffset, fMask; /* field offset and mask */
    uint32      regData;
    int32       ret;

    if (!GPIO_COM_CHK(unit))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (port > GPIO_REG(unit).portMax)
    {
        return RT_ERR_OUT_OF_RANGE;
    }

    regAddr = GPIO_REG(unit).pPortReg[port].isr.addr;
    fOffset = GPIO_REG(unit).pPortReg[port].isr.offset + pin;
    fMask = (GPIO_BITS_TO_MASK(1) << fOffset);

    if ((ret = ioal_soc_mem32_read(unit, regAddr, &regData)) != RT_ERR_OK)
    {
        return ret;
    }

    *pIsr = (regData & fMask) >> fOffset;

    return RT_ERR_OK;
}

/* Function Name:
 *      gpio_common_intrStatus_clear
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
gpio_common_intrStatus_clear(uint32 unit, drv_gpio_port_t port, uint32 pin)
{
    uint32      regAddr, fOffset, fMask; /* field offset and mask */
    uint32      regData;
    int32       ret;

    if (!GPIO_COM_CHK(unit))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (port > GPIO_REG(unit).portMax)
    {
        return RT_ERR_OUT_OF_RANGE;
    }

    regAddr = GPIO_REG(unit).pPortReg[port].isr.addr;
    fOffset = GPIO_REG(unit).pPortReg[port].isr.offset + pin;
    fMask = (GPIO_BITS_TO_MASK(1) << fOffset);

    /* write "1" to clear the interrupt */
    regData = 0;
    regData |= (1 << fOffset) & fMask;

    if ((ret = ioal_soc_mem32_write(unit, regAddr, regData)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      gpio_common_portRange_get
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
gpio_common_portRange_get(uint32 unit, int32 *minPort, int32 *maxPort)
{
    if (!GPIO_COM_CHK(unit))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    *minPort = GPIO_PORT_A;
    *maxPort = (GPIO_REG(unit).portMax - 1);
    return RT_ERR_OK;
}

