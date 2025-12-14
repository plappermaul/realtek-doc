/*
 * Copyright (C) 2012 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 42664 $
 * $Date: 2013-09-09 11:17:16 +0800 (Mon, 09 Sep 2013) $
 *
 * Purpose : Definition of GPIO API
 *
 * Feature : Provide the APIs to enable and configure GPIO
 *
 */

#ifndef __DAL_RTL9607C_GPIO_H__
#define __DAL_RTL9607C_GPIO_H__

#define RTL9607C_GPIO_PIN_NUM 96
#define RTL9607C_GPIO_INTR_NUM 96
#define RTL9607C_GPIO_BASE 0xB8003300

/*********************************************************************
Offset	Size (byte)	Name			Description			Field
0x00		4		PABCD_CNR		Port A, B, C, D 	control register.		    		MISC
0x04		4		PABCD_Ptype		Port A,B,C, D 		peripheral type control register.	MISC
0x08		4		PABCD_DIR		Port A, B, C, D 	direction register.			    	MISC
0x0C		4		PABCD_DAT		Port A, B, C,D 	    data register.					    MISC
0x10		4		PABCD_ISR		Port A, B, C,D 	    interrupt status register.		    MISC
0x14		4		PAB_IMR			Port A, B 			interrupt mask register.			MISC
0x18		4		PCD_IMR			Port C, D 		    interrupt mask register.			MISC
0x1C		4		PEFGH_CNR		Port E, F, G, H 	control register.				    MISC
0x20		4		PEFGH_Ptype		Port E, F, G, H 	peripheral type control register.	MISC
0x24		4		PEFGH_DIR		Port E, F, G, H 	direction register.				    MISC
0x28		4		PEFGH_DAT		Port E, F, G, H 	data register.					    MISC
0x2C		4		PEFGH_ISR		Port E, F, G, H 	interrupt status register.		    MISC
0x30		4		PEF_IMR			Port E, F 			interrupt mask register.			MISC
0x34		4		PGH_IMR			Port G, H 		    interrupt mask register.			MISC
0x38		4		PABCD_CPU0_IMR	Port A, B, C, D 	cpu0 interrupt mask register.		MISC
0x3c		4		PABCD_CPU1_IMR	Port A, B, C, D 	cpu1 interrupt mask register.		MISC
0x40		4		PJKMN_CNR		Port J, K, M, N 	control register.				    MISC
0x44		4		PJKMN_Ptype		Port J, K, M, N 	peripheral type control register.	MISC
0x48		4		PJKMN_DIR		Port J, K, M, N 	direction register.				    MISC
0x4c		4		PJKMN_DAT		Port J, K, M, N 	data register.					    MISC
0x50		4		PJKMN_ISR		Port J, K, M, N 	interrupt status register.		    MISC
0x54		4		PJK_IMR			Port I, J 			interrupt mask register.			MISC
0x58		4		PMN_IMR			Port K, L 		    interrupt mask register.			MISC
****************************************************************************/


enum
{
    RTL9607C_GPIO_CTRL_ABCDr = RTL9607C_GPIO_BASE + 0x0, /*enable gpio function*/
    RTL9607C_GPIO_CTRL_EFGHr = RTL9607C_GPIO_BASE + 0x1c, /*enable gpio function*/
    RTL9607C_GPIO_CTRL_JKMNr = RTL9607C_GPIO_BASE + 0x40, /*enable gpio function*/
    RTL9607C_GPIO_DIR_ABCDr = RTL9607C_GPIO_BASE + 0x08, /*configure gpio pin to gpo or gpi*/
    RTL9607C_GPIO_DIR_EFGHr = RTL9607C_GPIO_BASE + 0x24, /*configure gpio pin to gpo or gpi*/
    RTL9607C_GPIO_DIR_JKMNr = RTL9607C_GPIO_BASE + 0x48, /*configure gpio pin to gpo or gpi*/
    RTL9607C_GPIO_DATA_ABCDr = RTL9607C_GPIO_BASE + 0x0c, /*datatit for input/output mode*/
    RTL9607C_GPIO_DATA_EFGHr = RTL9607C_GPIO_BASE + 0x28, /*datatit for input/output mode*/
    RTL9607C_GPIO_DATA_JKMNr = RTL9607C_GPIO_BASE + 0x4c, /*datatit for input/output mode*/
    RTL9607C_GPIO_IMS_ABCDr = RTL9607C_GPIO_BASE + 0x10, /*interrupt status */
    RTL9607C_GPIO_IMS_EFGHr = RTL9607C_GPIO_BASE + 0x2c, /*interrupt status */
    RTL9607C_GPIO_IMS_JKMNr = RTL9607C_GPIO_BASE + 0x50, /*interrupt status */
    RTL9607C_GPIO_IMR_ABr = RTL9607C_GPIO_BASE + 0x14, /*interrupt mask register AB */
    RTL9607C_GPIO_IMR_CDr = RTL9607C_GPIO_BASE + 0x18, /*interrupt mask register CD*/
    RTL9607C_GPIO_IMR_EFr = RTL9607C_GPIO_BASE + 0x30, /*interrupt mask register EF*/
    RTL9607C_GPIO_IMR_GHr = RTL9607C_GPIO_BASE + 0x34, /*interrupt mask register GH*/
    RTL9607C_GPIO_IMR_JKr = RTL9607C_GPIO_BASE + 0x54, /*interrupt mask register EF*/
    RTL9607C_GPIO_IMR_MNr = RTL9607C_GPIO_BASE + 0x58, /*interrupt mask register GH*/
    RTL9607C_GPIO_CPU0_ABCDr= RTL9607C_GPIO_BASE+0x38,	/*configure gpio imr to cpu 0*/
    RTL9607C_GPIO_CPU1_ABCDr= RTL9607C_GPIO_BASE+0x3c,	/*configure gpio imr to cpu 1*/
};

/*
 * Include Files
 */


/* Function Name:
 *      dal_rtl9607c_gpio_init
 * Description:
 *      gpio init function
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
extern int32
dal_rtl9607c_gpio_init ( void );


/* Function Name:
 *      dal_rtl9607c_gpio_state_set
 * Description:
 *      enable or disable gpio function
 * Input:
 *      gpioId		- gpio id from 0~95
 *      enable		- enable or disable
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
extern int32
dal_rtl9607c_gpio_state_set ( uint32 gpioId, rtk_enable_t enable );


/* Function Name:
 *      dal_rtl9607c_gpio_state_get
 * Description:
 *      enable or disable gpio function
 * Input:
 *      gpioId		- gpio id from 0~95
 *      enable		- enable or disable
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
extern int32
dal_rtl9607c_gpio_state_get ( uint32 gpioId, rtk_enable_t* enable );

/* Function Name:
 *      dal_rtl9607c_gpio_mode_set
 * Description:
 *     set gpio to input or output mode
 * Input:
 *      gpioId 		-gpio id from 0 to 95 
 *	  mode		-gpio mode, input or output mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
extern int32
dal_rtl9607c_gpio_mode_set ( uint32 gpioId, rtk_gpio_mode_t mode );


/* Function Name:
 *      dal_rtl9607c_gpio_mode_get
 * Description:
 *     set gpio to input or output mode
 * Input:
 *      gpioId 		-gpio id from 0 to 95 
 *	  mode		-point for get input or output mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
extern int32
dal_rtl9607c_gpio_mode_get ( uint32 gpioId, rtk_gpio_mode_t* mode );


/* Function Name:
 *      dal_rtl9607c_gpio_databit_get
 * Description:
 *     read gpio data
 * Input:
 *      gpioId 		-gpio id from 0 to 95
 *	  data		-point for read data from gpio
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
extern int32
dal_rtl9607c_gpio_databit_get ( uint32 gpioId, uint32* data );


/* Function Name:
 *      dal_rtl9607c_gpio_databit_set
 * Description:
 *     write data to gpio
 * Input:
 *      gpioId 		-gpio id from 0 to 95 
 *	  data		-write data to gpio
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
extern int32
dal_rtl9607c_gpio_databit_set ( uint32 gpioId, uint32 data );


/* Function Name:
 *      dal_rtl9607c_gpio_intr_get
 * Description:
 *     write data to gpio
 * Input:
 *      state: point of state, enable or disable
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
extern int32
dal_rtl9607c_gpio_intr_get ( uint32 gpioId, rtk_gpio_intrMode_t* pIntrMode );

/* Function Name:
 *      dal_rtl9607c_gpio_intr_set
 * Description:
 *     write data to gpio
 * Input:
 *      state: point of state, enable or disable
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
extern int32
dal_rtl9607c_gpio_intr_set ( uint32 gpioId, rtk_gpio_intrMode_t intrMode );


/* Function Name:
 *      dal_rtl9607c_gpio_intrStatus_clean
 * Description:
 *     clean gpio interrupt status
 * Input:
 *      gpioId - gpio pin id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
extern int32
dal_rtl9607c_gpio_intrStatus_clean ( uint32 gpioId );


/* Function Name:
 *      dal_rtl9607c_gpio_intrStatus_get
 * Description:
 *     Get gpio interrupt status value
 * Input:
 *      pState - point for gpio interrupt status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
extern int32
dal_rtl9607c_gpio_intrStatus_get ( uint32 gpioId, rtk_enable_t* pState );




#endif  /* __DAL_RTL9607C_GPIO_H__ */
