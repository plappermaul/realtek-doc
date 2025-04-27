/*
 * Copyright 2006, Realtek Semiconductor Corp.
 *
 * rtl8196b/bsp/bspchip.h:
 *   RTL8196B chip-level header file
 *
 * Tony Wu (tonywu@realtek.com.tw)
 * Nov. 07, 2006
 */
#ifndef _BSPCHIP_H_
#define _BSPCHIP_H_

/* Memory controller register */
#define REG_MEM_CTRL_DCDR     (0xB8001060)

/* PLL Gen3 global control register */
#define REG_PLL_GLB_CTRL      (0xBB000FC0)

/* PLL Gen3 global control register bit-field shift and mask definition*/
#define BF_CLKM_INI       (13)
#define PLL_GLB_CTRL_MEM_CHIP_CLK      (1 << BF_CLKM_INI)

/* System registers */
//#define SYSREG_PIN_STATUS_SPI_4B_EN_FD_S    (6)
//#define SYSREG_PIN_STATUS_SPI_4B_EN_MASK    (1 << SYSREG_PIN_STATUS_SPI_4B_EN_FD_S)

#define PERIREG_CMUCTLR_REG          (0xB8003600)
#define PERIREG_OCP_SMALLER_MASK     (1<<30)
#define PERIREG_LX_SMALLER_MASK      (1<<31)

/*
 * IRQ Controller
 */

/*
 *  ====================================
 *  Platform Configurable Common Options
 *  ====================================
 */

            
/*                        
 * IRQ Mapping
 */


/*
 * Interrupt Routing Selection
 */

/*
 *  ==========================
 *  Platform Register Settings
 *  ==========================
 */

/*
 * CPU
 */

/*
 * SoC
 */

/*
 * Memory Controller
 */

/*
 * NAND flash controller address
 */
//czyao , nand flash address

/*
 * UART
 */

/*
 * Interrupt Controller
 */

/*
 * System Clock
 */


#endif   /* _BSPCHIP_H */
