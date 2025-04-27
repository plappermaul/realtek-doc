/*
* ----------------------------------------------------------------
* Copyright c                  Realtek Semiconductor Corporation, 2002  
* All rights reserved.
* 
* $Header: /usr/local/dslrepos/u-boot-2011.12/arch/mips/cpu/rlx5281/rtl8676/loader.h,v 1.1.1.1 2012/02/01 07:50:52 yachang Exp $
*
* Abstract: Board specific definitions.

*
* ---------------------------------------------------------------
*/

#ifndef _BOARD_H
#define _BOARD_H


#define CONFIG_RTL865XC 1	//wei add
#define BICOLOR_LED 1	//wei add
//#define _L2_MODE_
//#define _DEBUG_

/* support single 8MB/16MB flash */
/*#undef _SUPPORT_LARGE_FLASH_ use menuconfig setting */ 

/* Enable dump hub */
#define _HUB_MODE_

/* Ping mode */
#define _PING_MODE_

/* Bi-color LED */
#ifdef CONFIG_RTL865X_BICOLOR_LED /* use menuconfig setting */
	#define BICOLOR_LED 
#endif

/* Bi-color LED for BXXX */
//#define BICOLOR_LED_VENDOR_BXXX

/* Compile update ROM function */
#define _SUPPORT_UPDATE_ROM_

/* Compile SDRAM test function */
#define _TEST_SDRAM_

/* Compile flash test function */
#define _TEST_FLASH_

/* Compile erase flash function */
#define _ERASE_FLASH_

/* Console definition 
*/
#define DEFAULT_BAUDRATE 38400



/* Interrupt source definition
*/
#define MAX_ILEV                        8
#define MAX_NUM_OF_CHAINING_INTR        3

#if 1
#define LBCTMO_ILEV                     6
#define UART1_ILEV                      6
#define TICK_ILEV                       5
#define SW_ILEV                         4
#define UART0_ILEV                      3
#define PABC_ILEV                       3

#endif




/* Address Map
*/

#define CRMR_ADDR                           CRMR
#define WDTCNR_ADDR                         WDTCNR
#ifdef CONFIG_RTL865X_DRAM_MAP_DLOAD_BUF_ADDR
	#define DRAM_MAP_DLOAD_BUF_ADDR				CONFIG_RTL865X_DRAM_MAP_DLOAD_BUF_ADDR
#else
	#define DRAM_MAP_DLOAD_BUF_ADDR             0x80C00000
#endif
#define DRAM_MAP_INJECT_BUF_ADDR            0x80100000 /* buffer for inject JUMP instructions */
#ifdef CONFIG_RTL865X_RUNTIME_ADDRESS
	#define DRAM_MAP_RUN_IMAGE_ADDR             0x80080000 /* default */
#else
	#define DRAM_MAP_RUN_IMAGE_ADDR             CONFIG_RTL865X_RUNTIME_ADDRESS /* according to menuconfig */
#endif
#define BSP_STACK_SIZE                      4096        /* size of system stack space */

#define UNCACHED_MALLOC(x)  (void *) (0xa0000000 | (uint32) malloc(x))



/* Tick definition 
*/
#define TICK_FREQ       TICK_10MS_FREQ
#define TICK_10MS_FREQ  100 /* 100 Hz */
#define TICK_100MS_FREQ 1000 /* 1000 Hz */



/* Define flash device 
*/
#define FLASH_AM29LV800BB   /* only use 1MB currently */
#undef FLASH_AM29LV160BB

// for debug
//#define MAX_PORT_NUM    5
#define MAX_PORT_NUM    6



/* Nic definitions 
*/
#define NUM_RX_PKTHDR_DESC          16 //80 //16
#define NUM_RX_MBUF_DESC            16
#define NUM_TX_PKTHDR_DESC          16
#define MBUF_LEN                    2048



/* Debug function calls 
*/
#define debug_printf printf



/* Calculation
*/
#define DIV(x,y)        ((x)/(y))
#define MOD(x,y)        ((x)%(y))
#define MUL(x,y)        ((x)*(y))


/* macro to tell 865X_A or 865X_B */

#define IS_865XC() (1)
#define IS_865XB() (0)
#define IS_865XA() (0)

#endif   /* _BOARD_H */

