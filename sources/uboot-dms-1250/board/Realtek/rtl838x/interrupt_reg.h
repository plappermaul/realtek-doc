#ifndef __INTERRUPT_REG_H
#define __INTERRUPT_REG_H

#include <rtk_reg.h>

/*
 * Interrupt Controller
 */
#define GIMR            (SOC_BASE_ADDR | RTL8380_GLBL_INTR_MSK_ADDR)
   #define UART0_IE        RTL8380_GLBL_INTR_MSK_UART0_IE_MASK
   #define UART1_IE        RTL8380_GLBL_INTR_MSK_UART1_IE_MASK
   #define TC0_IE          RTL8380_GLBL_INTR_MSK_TC0_IE_MASK
   #define TC1_IE          RTL8380_GLBL_INTR_MSK_TC1_IE_MASK
   #define OCPTO_IE        RTL8380_GLBL_INTR_MSK_OCPTO_IE_MASK
   #define HLXTO_IE        RTL8380_GLBL_INTR_MSK_HLXTO_IE_MASK
   #define SLXTO_IE        RTL8380_GLBL_INTR_MSK_SLXTO_IE_MASK
   #define NIC_IE          RTL8380_GLBL_INTR_MSK_NIC_IE_MASK
   #define GPIO_ABC_IE     RTL8380_GLBL_INTR_MSK_GPIO_ABC_IE_MASK
   #define SECURITY_IE     RTL8380_GLBL_INTR_MSK_SECURITY_IE_MASK
   #define SWCORE_IE       RTL8380_GLBL_INTR_MSK_SWCORE_IE_MASK
   #define WDT_PH1TO_IE    RTL8380_GLBL_INTR_MSK_WDT_PH1TO_IE_MASK
   #define WDT_PH2TO_IE    RTL8380_GLBL_INTR_MSK_WDT_PH2TO_IE_MASK

#define GISR            (SOC_BASE_ADDR | RTL8380_GLBL_INTR_STS_ADDR)
   #define UART0_IP        RTL8380_GLBL_INTR_STS_UART0_IP_MASK
   #define UART1_IP        RTL8380_GLBL_INTR_STS_UART1_IP_MASK
   #define TC0_IP          RTL8380_GLBL_INTR_STS_TC0_IP_MASK
   #define TC1_IP          RTL8380_GLBL_INTR_STS_TC1_IP_MASK
   #define OCPTO_IP        RTL8380_GLBL_INTR_STS_OCPTO_IP_MASK
   #define HLXTO_IP        RTL8380_GLBL_INTR_STS_HLXTO_IP_MASK
   #define SLXTO_IP        RTL8380_GLBL_INTR_STS_SLXTO_IP_MASK
   #define NIC_IP          RTL8380_GLBL_INTR_STS_NIC_IP_MASK
   #define GPIO_ABC_IP     RTL8380_GLBL_INTR_STS_GPIO_ABC_IP_MASK
   #define SECURITY_IP     RTL8380_GLBL_INTR_STS_SECURITY_IP_MASK
   #define SWCORE_IP       RTL8380_GLBL_INTR_STS_SWCORE_IP_MASK
   #define WDT_PH1TO_IP    RTL8380_GLBL_INTR_STS_WDT_PH1TO_IP_MASK
   #define WDT_PH2TO_IP    RTL8380_GLBL_INTR_STS_WDT_PH2TO_IP_MASK

#define IRR0            (SOC_BASE_ADDR | RTL8380_INTR_ROUTINE_0_ADDR)
#define IRR0_SETTING    ((UART0_RS  << RTL8380_INTR_ROUTINE_0_UART0_RS_OFFSET) | \
                         (UART1_RS  << RTL8380_INTR_ROUTINE_0_UART1_RS_OFFSET) | \
                         (TC0_RS    << RTL8380_INTR_ROUTINE_0_TC0_RS_OFFSET) | \
                         (TC1_RS    << RTL8380_INTR_ROUTINE_0_TC1_RS_OFFSET) | \
                         (OCPTO_RS  << RTL8380_INTR_ROUTINE_0_OCPTO_RS_OFFSET) | \
                         (HLXTO_RS  << RTL8380_INTR_ROUTINE_0_HLXTO_RS_OFFSET)  | \
                         (SLXTO_RS  << RTL8380_INTR_ROUTINE_0_SLXTO_RS_OFFSET)  | \
                         (NIC_RS    << RTL8380_INTR_ROUTINE_0_NIC_RS_OFFSET)    \
                        )

#define IRR1            (SOC_BASE_ADDR | RTL8380_INTR_ROUTINE_1_ADDR)
#define IRR1_SETTING    ((GPIO_ABC_RS  << RTL8380_INTR_ROUTINE_1_GPIO_ABC_RS_OFFSET)  | \
                           (SECURITY_RS  << RTL8380_INTR_ROUTINE_1_SECURITY_RS_OFFSET)  | \
                           (SWCORE_RS    << RTL8380_INTR_ROUTINE_1_SWCORE_RS_OFFSET)    | \
                           (WDT_PH1TO_RS << RTL8380_INTR_ROUTINE_1_WDT_PH1TO_RS_OFFSET) | \
                         (WDT_PH2TO_RS << RTL8380_INTR_ROUTINE_1_WDT_PH2TO_RS_OFFSET) \
                        )

/*
 * Interrupt IRQ Assignments
 */
#define UART0_IRQ       31
#define UART1_IRQ       30
#define TC0_IRQ         29
#define TC1_IRQ         28
#define OCPTO_IRQ       27
#define HLXTO_IRQ       26
#define SLXTO_IRQ       25
#define NIC_IRQ         24
#define GPIO_ABC_IRQ    23
#define SECURITY_IRQ    22
#define SWCORE_IRQ      20
#define WDT_PH1TO_IRQ   19
#define WDT_PH2TO_IRQ   18

/*
 * Interrupt Routing Selection
 */
#define UART0_RS       2
#define UART1_RS       1
#define TC0_RS         5
#define TC1_RS         1
#define OCPTO_RS       1
#define HLXTO_RS       1
#define SLXTO_RS       1
#define NIC_RS         4
#define GPIO_ABC_RS    4
#define SECURITY_RS    4
#define SWCORE_RS      3
#define WDT_PH1TO_RS   4
#define WDT_PH2TO_RS   4



#endif /*__INTERRUPT_H*/
