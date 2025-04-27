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
 * Feature : GPIO relative API
 *
 */
#ifndef __DRV_GPIO_RTL8380_H__
#define __DRV_GPIO_RTL8380_H__

/*
* Include Files
*/


/*
* Symbol Definition
*/

/*
 * Data Declaration
 */

/*
 * Macro Declaration
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
rtl8380_gpio_init(uint32 unit);

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
extern int32
rtl8380_gpio_direction_set(uint32 unit, drv_gpio_port_t port, uint32 pin, drv_gpio_direction_t direction);

/* Function Name:
 *      rtl8380_gpio_control_set
 * Description:
 *      Set pin as GPIO-pin or dedicated-peripheral-pin
 * Input:
 *      unit      - UNIt ID
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
extern int32
rtl8380_gpio_control_set(uint32 unit, drv_gpio_port_t port, uint32 pin, drv_gpio_control_t ctrl);

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
extern int32
rtl8380_gpio_intrMode_set(uint32 unit, drv_gpio_port_t port, uint32 pin, drv_gpio_interruptType_t intrMode);

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
extern int32
rtl8380_gpio_dataBit_init(uint32 unit, drv_gpio_port_t port, uint32 pin, uint32 data);

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
extern int32
rtl8380_gpio_dataBit_get(uint32 unit, drv_gpio_port_t port, uint32 pin, uint32 *pData);

/* Function Name:
 *      rtl8380_gpio_dataBit_set
 * Description:
 *      Set GPIO pin's data as 1 or 0
 * Input:
 *      unit      - UNIt ID
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
extern int32
rtl8380_gpio_dataBit_set(uint32 unit, drv_gpio_port_t port, uint32 pin, uint32 data);

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
extern int32
rtl8380_gpio_intrStatus_get(uint32 unit, drv_gpio_port_t port, uint32 pin, uint32 *pIsr);

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
extern int32
rtl8380_gpio_intrStatus_clear(uint32 unit, drv_gpio_port_t port, uint32 pin);

/* Function Name:
 *      rtl8380_gpio_portRange_get
 * Description:
 *      Get GPIO port range
 * Input:
 *      unit      - UNIT ID
 *      minPort   - minimum GPIO port number
 *      maxPort   - maximum GPIO port number
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      RT_ERR_OK           - get success.
 */
extern int32
rtl8380_gpio_portRange_get(uint32 unit, int32 *minPort, int32 *maxPort);

#endif /* __DRV_GPIO_RTL8380_H__ */



