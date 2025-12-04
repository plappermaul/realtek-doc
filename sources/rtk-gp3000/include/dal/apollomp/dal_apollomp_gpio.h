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
 * $Revision$
 * $Date$
 *
 * Purpose : Definition of GPIO API
 *
 * Feature : Provide the APIs to enable and configure GPIO
 *
 */

#ifndef __DAL_APOLLOMP_GPIO_H__
#define __DAL_APOLLOMP_GPIO_H__

#define APOLLOMP_GPIO_PIN_NUM 64
#define APOLLOMP_GPIO_INTR_NUM 64
#define APOLLOMP_GPIO_BASE 0xB8003300

/*********************************************************************
Offset	Size (byte)	Name						Description					Field
0x00		4		PABCD_CNR		Port A, B, C, D 	control register.				MISC
0x04		4		PABCD_Ptype		Port A,B,C, D 		peripheral type control register.	MISC
0x08		4		PABCD_DIR		Port A, B, C, D 	direction register.				MISC
0x0C	4		PABCD_DAT		Port A, B, C,D 	data register.					MISC
0x10		4		PABCD_ISR		Port A, B, C,D 	interrupt status register.		MISC
0x14		4		PAB_IMR			Port A, B 			interrupt mask register.			MISC
0x18		4		PCD_IMR			Port C, D 		interrupt mask register.			MISC
0x1C	4		PEFGH_CNR		Port E, F, G, H 	control register.				MISC
0x20		4		PEFGH_Ptype		Port E, F, G, H 	peripheral type control register.	MISC
0x24		4		PEFGH_DIR		Port E, F, G, H 	direction register.				MISC
0x28		4		PEFGH_DAT		Port E, F, G, H 	data register.					MISC
0x2C	4		PEFGH_ISR		Port E, F, G, H 	interrupt status register.		MISC
0x30		4		PEF_IMR			Port E, F 			interrupt mask register.			MISC
0x34		4		PGH_IMR			Port G, H 		interrupt mask register.			MISC
0x38		4		PABCD_CPU0_IMR	Port A, B, C, D 	cpu0 interrupt mask register.		MISC
0x3C	4		PABCD_CPU1_IMR	Port A, B, C, D 	cpu1 interrupt mask register.		MISC
****************************************************************************/


enum {
APOLLOMP_GPIO_CTRL_ABCDr= APOLLOMP_GPIO_BASE+0x0, 	/*enable gpio function*/
APOLLOMP_GPIO_CTRL_EFGHr= APOLLOMP_GPIO_BASE+0x1c, 	/*enable gpio function*/
APOLLOMP_GPIO_DIR_ABCDr	= APOLLOMP_GPIO_BASE+0x08, 	/*configure gpio pin to gpo or gpi*/
APOLLOMP_GPIO_DIR_EFGHr = APOLLOMP_GPIO_BASE+0x24,	/*configure gpio pin to gpo or gpi*/
APOLLOMP_GPIO_DATA_ABCDr= APOLLOMP_GPIO_BASE+0x0c, 	/*datatit for input/output mode*/
APOLLOMP_GPIO_DATA_EFGHr= APOLLOMP_GPIO_BASE+0x28,	/*datatit for input/output mode*/
APOLLOMP_GPIO_IMS_ABCDr = APOLLOMP_GPIO_BASE+0x10,	/*interrupt status */
APOLLOMP_GPIO_IMS_EFGHr = APOLLOMP_GPIO_BASE+0x2c,	/*interrupt status */
APOLLOMP_GPIO_IMR_ABr 	= APOLLOMP_GPIO_BASE+0x14,  /*interrupt mask register AB */
APOLLOMP_GPIO_IMR_CDr 	= APOLLOMP_GPIO_BASE+0x18,  /*interrupt mask register CD*/
APOLLOMP_GPIO_IMR_EFr	= APOLLOMP_GPIO_BASE+0x30,	/*interrupt mask register EF*/
APOLLOMP_GPIO_IMR_GHr	= APOLLOMP_GPIO_BASE+0x34,	/*interrupt mask register GH*/
APOLLOMP_GPIO_CPU0_ABCDr= APOLLOMP_GPIO_BASE+0x38,	/*configure gpio imr to cpu 0*/
APOLLOMP_GPIO_CPU1_ABCDr= APOLLOMP_GPIO_BASE+0x3c,	/*configure gpio imr to cpu 1*/

};

/*
 * Include Files
 */


/* Function Name:
 *      dal_apollomp_gpio_init
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
dal_apollomp_gpio_init(void );



/* Function Name:
 *      dal_apollomp_gpio_state_set
 * Description:
 *      enable or disable gpio function
 * Input:
 *      gpioId		- gpio id from 0~71
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
dal_apollomp_gpio_state_set(uint32 gpioId, rtk_enable_t enable);


/* Function Name:
 *      dal_apollomp_gpio_state_get
 * Description:
 *      enable or disable gpio function
 * Input:
 *      gpioId		- gpio id from 0~71
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
dal_apollomp_gpio_state_get(uint32 gpioId, rtk_enable_t *enable);


/* Function Name:
 *      dal_apollomp_gpio_mode_set
 * Description:
 *     set gpio to input or output mode
 * Input:
 *      gpioId 		-gpio id from 0 to 71
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
dal_apollomp_gpio_mode_set(uint32 gpioId, rtk_gpio_mode_t mode);


/* Function Name:
 *      dal_apollomp_gpio_mode_get
 * Description:
 *     set gpio to input or output mode
 * Input:
 *      gpioId 		-gpio id from 0 to 71
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
dal_apollomp_gpio_mode_get(uint32 gpioId, rtk_gpio_mode_t *mode);


/* Function Name:
 *      dal_apollomp_gpio_databit_get
 * Description:
 *     read gpio data
 * Input:
 *      gpioId 		-gpio id from 0 to 71
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
dal_apollomp_gpio_databit_get(uint32 gpioId, uint32 *data);


/* Function Name:
 *      dal_apollomp_gpio_databit_set
 * Description:
 *     write data to gpio
 * Input:
 *      gpioId 		-gpio id from 0 to 71
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
dal_apollomp_gpio_databit_set(uint32 gpioId, uint32 data);


/* Function Name:
 *      dal_apollomp_gpio_intr_get
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
dal_apollomp_gpio_intr_get(uint32 gpioId,rtk_gpio_intrMode_t *pIntrMode);

/* Function Name:
 *      dal_apollomp_gpio_intr_set
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
dal_apollomp_gpio_intr_set(uint32 gpioId,rtk_gpio_intrMode_t intrMode);


/* Function Name:
 *      dal_apollomp_gpio_intrStatus_clean
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
dal_apollomp_gpio_intrStatus_clean(uint32 gpioId);


/* Function Name:
 *      dal_apollomp_gpio_intrStatus_get
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
dal_apollomp_gpio_intrStatus_get(uint32 gpioId,rtk_enable_t *pState);




#endif  /* __DAL_APOLLOMP_GPIO_H__ */
