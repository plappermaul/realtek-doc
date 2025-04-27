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

#define CONFIG_RTL8390

/* System registers */

#define SWCORE_MAC_GLB_CTRL_REG_ADDR                    (0xBB0002A8)
#define SWCORE_MAC_GLB_CTRL_REG_CPU_CLK_SEL_FD_S        (22)
#define SWCORE_MAC_GLB_CTRL_REG_CPU_CLK_SEL_MASK        (3 << SWCORE_MAC_GLB_CTRL_REG_CPU_CLK_SEL_FD_S)
#define SWCORE_MAC_GLB_CTRL_REG_DDR_CLK_SEL_FD_S        (19)
#define SWCORE_MAC_GLB_CTRL_REG_DDR_CLK_SEL_MASK        (7 << SWCORE_MAC_GLB_CTRL_REG_DDR_CLK_SEL_FD_S)

#define SYS_LX_CLK_FRQ_SLOWER_REG			(0xB8003600)
#define SYS_LX_CLK_FRQ_SLOWER_REG_LX_CLK_SLOW_FD_S      (31)
#define SYS_LX_CLK_FRQ_SLOWER_REG_OCP_CLK_SLOW_FD_S     (30)
#define SYS_LX_CLK_FRQ_SLOWER_REG_LX_CLK_SLOW_MASK      (1 << SYS_LX_CLK_FRQ_SLOWER_REG_LX_CLK_SLOW_FD_S)
#define SYS_LX_CLK_FRQ_SLOWER_REG_OCP_CLK_SLOW_MASK     (1 << SYS_LX_CLK_FRQ_SLOWER_REG_OCP_CLK_SLOW_FD_S)
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
#define DCDR_A            (0xB8001060)
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
