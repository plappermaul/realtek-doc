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
 * $Revision: 71708 $
 * $Date: 2016-09-19 11:31:17 +0800 (Mon, 19 Sep 2016) $
 *
 * Purpose : chip symbol and data type definition in the SDK.
 *
 * Feature : chip symbol and data type definition
 *
 */

#ifndef __DRV_SWCORE_SWCORE_H__
#define __DRV_SWCORE_SWCORE_H__

/*
 * Include Files
 */
#include <common/type.h>
#include <hal/chipdef/chip.h>

/*
 * Symbol Definition
 */

/* Definition chip CPU Freq reg (0xBB00 switch part)*/
#define RTL839x5x_CPU_PLL0			(0x0028)
#define RTL839x5x_CPU_PLL1			(0x002C)

#define RTL838x3x_CPU_READ_CTL		(0x0058)
#define RTL838x3x_CPU_CTL0			(0x0FC4)
#define RTL838x3x_CPU_CTL1			(0x0FC8)
#define RTL838x3x_CPU_MISC			(0x0FCC)


/* Definition chip CPU Freq */
#define RTL839x5x_CPU_750M_PLL0     (0x000041AC)
#define RTL839x5x_CPU_750M_PLL1		(0x00000004)
#define RTL839x5x_CPU_700M_PLL0     (0x0000418C)
#define RTL839x5x_CPU_700M_PLL1		(0x00000004)
#define RTL839x5x_CPU_650M_PLL0     (0x0000416C)
#define RTL839x5x_CPU_650M_PLL1		(0x00000004)
#define RTL839x5x_CPU_500M_PLL0     (0x000041AC)
#define RTL839x5x_CPU_500M_PLL1		(0x00000005)

#define RTL838x3x_CPU_500M_CTL0		(0x00004748)
#define RTL838x3x_CPU_500M_CTL1     (0x0C14530E)
#define RTL838x3x_CPU_500M_MISC		(0x2DA0CFB8)
#define RTL838x3x_CPU_300M_CTL0		(0x000045C8)
#define RTL838x3x_CPU_300M_CTL1     (0x1414530E)
#define RTL838x3x_CPU_300M_MISC		(0x2D20CFB8)

/* define JTAG interface */
typedef enum drv_swcore_jtag_intf_e
{
    JTAG_INTF_JTAG = 0,
    JTAG_INTF_UART1,
    JTAG_INTF_GPIO,
    JTAG_INTF_END
} drv_swcore_jtag_intf_t;

/*
 * Data Type Definition
 */

/*
 * Function Declaration
 */

/* Function Name:
 *      drv_swcore_CPU_freq_get
 * Description:
 *      Get Chip CPU Frequency
 * Input:
 *      unit           - unit id
 * Output:
 *      pCPU_freq       - CPU_freq
 * Return:
 *      RT_ERR_OK        - OK
 *      RT_ERR_FAILED   - Failed
 *      RT_ERR_CHIP_NOT_SUPPORTED - Not support by this API
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
drv_swcore_CPU_freq_get(uint32 unit, uint32 * pCPU_freq);

/* Function Name:
 *      drv_swcore_register_dump
 * Description:
 *      Dump Chip peripher registers
 * Input:
 *      unit           - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK        - OK
 *      RT_ERR_FAILED   - Failed
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
drv_swcore_register_dump(uint32 unit);

/* Function Name:
 *      drv_swcore_jtag_intf_get
 * Description:
 *      Get the configuration of JTAG interface.
 * Input:
 *      unit           - unit id
 *      intf_sel       - interface type
 * Output:
 *      intf_sel       - interface type
 * Return:
 *      RT_ERR_OK      - OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
drv_swcore_jtag_intf_get(uint32 unit, drv_swcore_jtag_intf_t * intf_sel);

/* Function Name:
 *      drv_swcore_jtag_intf_set
 * Description:
 *      Set the usage of JTAG interface.
 * Input:
 *      unit           - unit id
 *      intf_sel       - interface type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK      - OK
 *      RT_ERR_FAILED
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
drv_swcore_jtag_intf_set(uint32 unit, drv_swcore_jtag_intf_t intf_sel);


#endif  /* __DRV_SWCORE_SWCORE_H__ */

