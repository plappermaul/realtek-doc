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

#ifndef __DAL_RTL9601B_GPIO_H__
#define __DAL_RTL9601B_GPIO_H__


/*
 * GPIO SOC Register Profile
 */
#define RTL9601B_GPIO_PIN_NUM 26
#define RTL9601B_GPIO_INTR_NUM 26
#define RTL9601B_GPIO_BASE 0xB8003300

/*********************************************************************
Offset	Size (byte)	Name						Description					Field
0x08		4		PABCD_DIR		Port A, B, C, D 	direction register.				MISC
0x0C	4		PABCD_DAT		Port A, B, C,D 	data register.					MISC
0x10		4		PABCD_ISR		Port A, B, C,D 	interrupt status register.		MISC
0x14		4		PAB_IMR			Port A, B 			interrupt mask register.			MISC
0x18		4		PCD_IMR			Port C, D 		interrupt mask register.			MISC
0x38		4		PABCD_CPU0_IMR	Port A, B, C, D 	cpu0 interrupt mask register.		MISC
0x3C	4		PABCD_CPU1_IMR	Port A, B, C, D 	cpu1 interrupt mask register.		MISC
****************************************************************************/


/*PIN definitation
GPIO Num	Pin Name 						Register Profile Name		Field Name
GPIO_0		M_SDA							IO_MODE_EN				I2C_MASTER_EN
GPIO_1		M_SCK							IO_MODE_EN				I2C_MASTER_EN
GPIO_2		JTAG_CK /LED0					IO_MODE_EN				DIS_JTAG
											IO_LED_EN				LED0_EN
GPIO_3		JTAG_TMS/LED1					IO_MODE_EN/IO_LED_EN	LED1_EN/DIS_JTAG
GPIO_4		JTAG_TDO						IO_MODE_EN				DIS_JTAG
GPIO_5		JTAG_TDI /LED2					IO_MODE_EN/IO_LED_EN	LED2_EN /DIS_JTAG
GPIO_6		JTAG_nRST /LED3					IO_MODE_EN/IO_LED_EN	LED3_EN /DIS_JTAG
GPIO_7		DIS_TX							IO_MODE_EN				DIS_TX_EN
GPIO_8		TX_SD							IO_MODE_EN				TX_SD_EN
GPIO_9		URXD							IO_MODE_EN				UART_EN
GPIO_10		UTXD							IO_MODE_EN				UART_EN
GPIO_11		S_SDA							IO_MODE_EN				DIS_I2C_SLAVE
GPIO_12		S_SCK							IO_MODE_EN				DIS_I2C_SLAVE
GPIO_13		SLICSPI_CLK /ZPACK/MSI_CLK24M	IO_MODE_EN				VOIP0_EN
GPIO_14		SLICSPI_MOSI/Z0MOSI/MSIF0_PSDO	IO_MODE_EN				VOIP1_EN
GPIO_15		SLICSPI_MISO/Z0MISO/MSIF0_PSDI	IO_MODE_EN				VOIP2_EN
GPIO_16		SLICSPI_nCS0/Z0SYNC				IO_MODE_EN				VOIP3_EN
GPIO_17		SPI_RST							IO_MODE_EN				SPI_RST_EN
GPIO_18		SLICPCM_FS/Z1SYNC				IO_MODE_EN				VOIP4_EN
GPIO_19		SLICPCM_RXD/Z1MISO/MSIF1_PSDI	IO_MODE_EN				VOIP5_EN
GPIO_20		SLICPCM_TXD/Z1MOSI/MSIF1_PSDO	IO_MODE_EN				VOIP6_EN
GPIO_21		SLICPCM_CLK						IO_MODE_EN				VOIP7_EN
GPIO_22		SLICSPI_nCS1						IO_MODE_EN				VOIP8_EN
GPIO_23		TOD								IO_MODE_EN				IO_MODE_EN
GPIO_24		GPONPPS							IO_MODE_EN				IO_MODE_EN
GPIO_25		GPIO25 (??ªºGPIO Pin)	-	-

*/


enum {
RTL9601B_GPIO_CTRL_ABCDr= RTL9601B_GPIO_BASE+0x0, 	/*enable gpio function*/
RTL9601B_GPIO_DIR_ABCDr	= RTL9601B_GPIO_BASE+0x08, 	/*configure gpio pin to gpo or gpi*/
RTL9601B_GPIO_DATA_ABCDr= RTL9601B_GPIO_BASE+0x0c, 	/*datatit for input/output mode*/
RTL9601B_GPIO_IMS_ABCDr = RTL9601B_GPIO_BASE+0x10,	/*interrupt status */
RTL9601B_GPIO_IMR_ABr 	= RTL9601B_GPIO_BASE+0x14,  /*interrupt mask register AB */
RTL9601B_GPIO_IMR_CDr 	= RTL9601B_GPIO_BASE+0x18,  /*interrupt mask register CD*/
RTL9601B_GPIO_C0_IER_ABCDr 	= RTL9601B_GPIO_BASE+0x38,  /*cpu0 interrupt mask register ABCD*/
RTL9601B_GPIO_C1_IER_ABCDr 	= RTL9601B_GPIO_BASE+0x3c,  /*cpu1 interrupt mask register ABCD*/

};


enum {
RTK_GPIO_0,
RTK_GPIO_1,
RTK_GPIO_2,
RTK_GPIO_3,
RTK_GPIO_4,
RTK_GPIO_5,
RTK_GPIO_6,
RTK_GPIO_7,
RTK_GPIO_8,
RTK_GPIO_9,
RTK_GPIO_10,
RTK_GPIO_11,
RTK_GPIO_12,
RTK_GPIO_13,
RTK_GPIO_14,
RTK_GPIO_15,
RTK_GPIO_16,
RTK_GPIO_17,
RTK_GPIO_18,
RTK_GPIO_19,
RTK_GPIO_20,
RTK_GPIO_21,
RTK_GPIO_22,
RTK_GPIO_23,
RTK_GPIO_24,
RTK_GPIO_25,

};


/*
 * Include Files
 */


/* Function Name:
 *      dal_rtl9601b_gpio_init
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
dal_rtl9601b_gpio_init(void );



/* Function Name:
 *      dal_rtl9601b_gpio_state_set
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
dal_rtl9601b_gpio_state_set(uint32 gpioId, rtk_enable_t enable);


/* Function Name:
 *      dal_rtl9601b_gpio_state_get
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
dal_rtl9601b_gpio_state_get(uint32 gpioId, rtk_enable_t *enable);


/* Function Name:
 *      dal_rtl9601b_gpio_mode_set
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
dal_rtl9601b_gpio_mode_set(uint32 gpioId, rtk_gpio_mode_t mode);


/* Function Name:
 *      dal_rtl9601b_gpio_mode_get
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
dal_rtl9601b_gpio_mode_get(uint32 gpioId, rtk_gpio_mode_t *mode);


/* Function Name:
 *      dal_rtl9601b_gpio_databit_get
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
dal_rtl9601b_gpio_databit_get(uint32 gpioId, uint32 *data);


/* Function Name:
 *      dal_rtl9601b_gpio_databit_set
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
dal_rtl9601b_gpio_databit_set(uint32 gpioId, uint32 data);


/* Function Name:
 *      dal_rtl9601b_gpio_intr_get
 * Description:
 *     write data to gpio
 * Input:
 *      gpioId - gpio pin id from 0 to 63
 *      pIntrMode - point for interrupt mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
extern int32
dal_rtl9601b_gpio_intr_get(uint32 gpioId,rtk_gpio_intrMode_t *pIntrMode);


/* Function Name:
 *      dal_rtl9601b_gpio_intr_set
 * Description:
 *     write data to gpio
 * Input:
 *      gpioId - gpio pin id from 0 to 63
 *      intrMode - point for interrupt mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
extern int32
dal_rtl9601b_gpio_intr_set(uint32 gpioId,rtk_gpio_intrMode_t intrMode);

/* Function Name:
 *      dal_rtl9601b_gpio_intrStatus_clean
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
dal_rtl9601b_gpio_intrStatus_clean(uint32 gpioId);

/* Function Name:
 *      dal_rtl9601b_gpio_intrStatus_get
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
dal_rtl9601b_gpio_intrStatus_get(uint32 gpioId,rtk_enable_t *pState);

#endif  /* __DAL_RTL9601B_GPIO_H__ */
