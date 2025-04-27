#ifndef __INTERRUPT_H
#define __INTERRUPT_H


#define MIPS_DEFAULT_EBASE 0x80000000
#define UBOOT_EBASE 0xbfc00000

extern void common_irq_init(void);
extern __attribute__((nomips16)) void common_disable_interrupt(void);
extern __attribute__((nomips16)) void common_enable_interrupt(void);

extern void timer_isr(void*);

typedef int uboot_isrret_t;
typedef uboot_isrret_t (*uboot_isr_t)(void *);
#define IS_VALID_DEVICE_ID(_id) ((_id >= 0) && (_id < RTK_DEV_MAX))


typedef struct rtk_dev_s{
    void* isr;
    void* isr_param;
    int irq;
} rtk_dev_t;



enum {
    RTK_DEV_UART0=0,
/*    RTK_DEV_UART1, */
    RTK_DEV_TC0,
/*    RTK_DEV_TC1, */
/*    RTK_DEV_OCPTO, */
/*    RTK_DEV_HLXTO, */
/*    RTK_DEV_SLXTO, */
    RTK_DEV_NIC,
/*    RTK_DEV_SWCORE, */
/*    RTK_DEV_GPIO_ABCD, */
/*    RTK_DEV_GPIO_EFG, */
/*    RTK_DEV_RTC, */
    RTK_DEV_MAX
};

#endif /*__INTERRUPT_H*/

