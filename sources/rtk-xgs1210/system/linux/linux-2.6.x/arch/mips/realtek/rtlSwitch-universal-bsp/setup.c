/*
 * Copyright (C) 2009-2015 Realtek Semiconductor Corp.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * $Revision: 85728 $
 * $Date: 2018-01-25 13:40:22 +0800 (Thu, 25 Jan 2018) $
 *
 */

/*
 * Include Files
 */
#include <linux/init.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/ioport.h>
#include <linux/tty.h>
#include <linux/serial.h>
#include <linux/serial_core.h>

#include <asm/cpu.h>
#include <asm/bootinfo.h>
#include <asm/reboot.h>
#include <asm/irq.h>
#include <asm/serial.h>
#include <asm/io.h>
#include <asm/time.h>

#include <prom.h>
#include <platform.h>

#include "chip_probe.h"
#include <int_setup.h>
#include "gcmp-gic.h"

//#include <asm/gic.h>

/*
 * Symbol Definition
 */
static void __init serial_init(void);

#if defined(CONFIG_RTL8390_SERIES)
static void __init rtl8390_time_init(void);
static void rtl8390_timer_ack(void);
static void __init rtl8390_timer_setup(struct irqaction *irq);
#endif
#if defined(CONFIG_RTL8380_SERIES)
static void __init rtl8380_time_init(void);
static void rtl8380_timer_ack(void);
static void __init rtl8380_timer_setup(struct irqaction *irq);
#endif

#if defined(CONFIG_RTL9300_SERIES)
static void __init rtl9300_time_init(void);
static void rtl9300_timer_ack(void);
static void __init rtl9300_timer_setup(struct irqaction *irq);
#endif
#if defined(CONFIG_RTL9310_SERIES)
static void __init rtl9310_time_init(void);
static void rtl9310_timer_ack(void);
static void __init rtl9310_timer_setup(struct irqaction *irq);
#endif


/*
 * Data Declaration
 */

unsigned int bsp_chip_id, bsp_chip_rev_id;
unsigned int bsp_chip_family_id, bsp_chip_type;

unsigned int bsp_Interrupt_IRQ_Mapping_IP[ICTL_MAX_IP];
unsigned int bsp_Interrupt_srcID_Mapping_IRQ[GIC_NUM_INTRS];


struct gic_intr_map {
    unsigned int cpunum;	/* Directed to this CPU */
#define GIC_UNUSED		0xdead			/* Dummy data */
    unsigned int pin;	/* Directed to this Pin */
    unsigned int polarity;	/* Polarity : +/-	*/
    unsigned int trigtype;	/* Trigger  : Edge/Levl */
    unsigned int flags;	/* Misc flags	*/
#define GIC_FLAG_IPI           0x01
#define GIC_FLAG_TRANSPARENT   0x02
};


extern int drv_swcore_cid_get(unsigned int unit, unsigned int *pCid, unsigned int *pCrevid);
extern uint32 drv_swcore_chipFamilyId_get(uint32 chip_id);


#define SWCORE_VIRT_BASE    0xBB000000

#define RT_ERR_OK 0
#define RT_ERR_FAILED -1


/* Function Name:
 *      bsp_mem32_read
 * Description:
 *      Get the value from register.
 * Input:
 *      unit - unit id
 *      addr - register address
 * Output:
 *      pVal - pointer buffer of the register value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      1. Support single unit right now and ignore unit
 *      2. When we support the multiple chip in future, we will check the input unit
 */
int32
bsp_mem32_read(uint32 unit, uint32 addr, uint32 *pVal)
{
    int32 ret = RT_ERR_OK;

    /* Upper layer have check the unit, and don't need to check again */

    *pVal = MEM32_READ(SWCORE_VIRT_BASE | addr);

    return ret;
} /* end of bsp_mem32_read */


/* Function Name:
 *      bsp_mem32_write
 * Description:
 *      Set the value to register.
 * Input:
 *      unit - unit id
 *      addr - register address
 *      val  - the value to write register
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      1. Support single unit right now and ignore unit
 *      2. When we support the multiple chip in future, we will check the input unit
 */
int32 bsp_mem32_write(uint32 unit, uint32 addr, uint32 val)
{
    int32 ret = RT_ERR_OK;

    MEM32_WRITE(SWCORE_VIRT_BASE | addr, val);

    return ret;
} /* end of bsp_mem32_write */


/*
 * Macro Definition
 */

/*
 * Function Declaration
 */

/**************************************************
 * For L2 Cache initialization
**************************************************/
extern void init_l2_cache(void);

static void inline l2_cache_bypass(void){
   struct cpuinfo_mips *c = &current_cpu_data;

   write_c0_config2(  read_c0_config2() | L2_BYPASS_MODE );
   /* For setup_scache@c-r4k.c */
   c->scache.flags = MIPS_CACHE_NOT_PRESENT;
}

void __init bsp_setup_scache(void)
{
    unsigned long config2;
    unsigned int tmp;
    config2 = read_c0_config2();
    tmp = (config2 >> 4) & 0x0f;

        printk("[%s]config2=0x%lx\n", __FUNCTION__, config2);

    /*if enable l2_bypass mode, linesize will be 0       */
    /*if arch not implement L2cache, linesize will be 0  */
    if (0 < tmp && tmp <= 7){ //Scache linesize >0 and <=256 (B)
        init_l2_cache();
     /* Kernel not to use L2cache, so force the l2cache into bypass mode.*/
     /* L2B : bit 12 of config2 */
//            l2_cache_bypass();
        }
}


const char *get_system_type(void)
{
    if(bsp_chip_family_id == RTL8390_FAMILY_ID)
        return "RTL8390";
    if(bsp_chip_family_id == RTL8380_FAMILY_ID)
        return "RTL8380";
    if(bsp_chip_family_id == RTL9300_FAMILY_ID)
        return "RTL9300";
    if(bsp_chip_family_id == RTL9310_FAMILY_ID)
        return "RTL9310";

    return "Unknow chip";
}

extern interrupt_mapping_conf_t rtl839X8X_intr_mapping[];
extern interrupt_mapping_conf_t rtl9300_intr_mapping[];
extern interrupt_mapping_conf_t rtl9310_intr_mapping[];
extern struct gic_intr_map rtl9310_gic_intr_map[];

void bsp_interrupt_irq_mapping_setup(void)
{
    int	index;
    int	irq_index;
    int int_type;
    unsigned int	src_index;
    interrupt_mapping_conf_t *intr_mapping_table;
    struct gic_intr_map *gic_mapping_table = NULL;

    if((bsp_chip_family_id == RTL8390_FAMILY_ID)||(bsp_chip_family_id == RTL8350_FAMILY_ID)||(bsp_chip_family_id == RTL8380_FAMILY_ID)||(bsp_chip_family_id == RTL8330_FAMILY_ID))
    {
        intr_mapping_table = rtl839X8X_intr_mapping;
    }
    else if((bsp_chip_family_id == RTL9300_FAMILY_ID))
    {
        intr_mapping_table = rtl9300_intr_mapping;
    }
    else if((bsp_chip_family_id == RTL9310_FAMILY_ID))
    {
        intr_mapping_table = rtl9310_intr_mapping;
        gic_mapping_table = rtl9310_gic_intr_map;
    }else{
        printk("\n Interrupt Mapping failed !!! \n");
        return;
    }

    for(index = 0; index < ICTL_MAX_IP; index++)
    {
        src_index = (unsigned int)((interrupt_mapping_conf_t *)(intr_mapping_table + index)->intr_src_id);
        bsp_Interrupt_IRQ_Mapping_IP[index] = src_index;
        if((bsp_chip_family_id == RTL9310_FAMILY_ID) && (src_index < GIC_NUM_INTRS) && (gic_mapping_table != NULL))
        {
            ((struct gic_intr_map *)(gic_mapping_table + src_index))->cpunum = (unsigned int)((interrupt_mapping_conf_t *)(intr_mapping_table + index)->target_cpu_id);
            ((struct gic_intr_map *)(gic_mapping_table + src_index))->pin = (unsigned int)((unsigned int)((interrupt_mapping_conf_t *)(intr_mapping_table + index)->cpu_ip_id) - 2);
            int_type = (int)((interrupt_mapping_conf_t *)(intr_mapping_table + index)->intr_type);
            if(((interrupt_mapping_conf_t *)(intr_mapping_table + index)->intr_type) != INTR_TYPE_DEFAULT)
            {
                switch(int_type)
                {
                    case INTR_TYPE_HIGH_LEVEL:
                        ((struct gic_intr_map *)(gic_mapping_table + src_index))->polarity = GIC_POL_POS;
                        ((struct gic_intr_map *)(gic_mapping_table + src_index))->trigtype = GIC_TRIG_LEVEL;
                        break;
                    case INTR_TYPE_LOW_LEVEL:
                        ((struct gic_intr_map *)(gic_mapping_table + src_index))->polarity = GIC_POL_NEG;
                        ((struct gic_intr_map *)(gic_mapping_table + src_index))->trigtype = GIC_TRIG_LEVEL;
                        break;
                    case INTR_TYPE_FALLING_EDGE:
                        ((struct gic_intr_map *)(gic_mapping_table + src_index))->polarity = GIC_POL_NEG;
                        ((struct gic_intr_map *)(gic_mapping_table + src_index))->trigtype = GIC_TRIG_EDGE;
                        break;
                    case INTR_TYPE_RISING_EDGE:
                        ((struct gic_intr_map *)(gic_mapping_table + src_index))->polarity = GIC_POL_POS;
                        ((struct gic_intr_map *)(gic_mapping_table + src_index))->trigtype = GIC_TRIG_EDGE;
                        break;
                     default:
                        break;
                }
            }
        }
    }

    if((bsp_chip_family_id == RTL9310_FAMILY_ID))
    {
        for(src_index = 0; src_index < GIC_NUM_INTRS; src_index++)
        {
            for(irq_index = 0; irq_index < ICTL_MAX_IP; irq_index++){
                if(bsp_Interrupt_IRQ_Mapping_IP[irq_index] == src_index) /*interrupt src index*/
                    bsp_Interrupt_srcID_Mapping_IRQ[src_index] = irq_index;
            }
        }
    }
}


void __init prom_init(void)
{
    int ret;

    int argc = fw_arg0;
    char **arg = (char **)fw_arg1;
    int i;

    bsp_chip_id = 0;
    bsp_chip_rev_id = 0;
    bsp_chip_family_id = 0;
    bsp_chip_type = 0;

    drv_swcore_earlyInit_start();
    ret = drv_swcore_cid_get((unsigned int)0, (unsigned int *)&bsp_chip_id, (unsigned int *)&bsp_chip_rev_id);
    if(ret != 0)
    {
        printk("\nbsp_init(), RTK Switch chip is not found!!!\n");
    }
    else
    {
        bsp_chip_family_id = FAMILY_ID(bsp_chip_id);
    }

    printk("RTK chip =%X, Chip_family_id=%X.\n",(bsp_chip_id >> 16), (bsp_chip_family_id >> 16));

    bsp_interrupt_irq_mapping_setup();
    prom_console_init();
    prom_meminit();

    /* if user passes kernel args, ignore the default one */
    if (argc > 1)
          arcs_cmdline[0] = '\0';

    /* arg[0] is "g", the rest is boot parameters */
    for (i = 1; i < argc; i++) {
        if (strlen(arcs_cmdline) + strlen(arg[i] + 1)
          >= sizeof(arcs_cmdline))
          break;
        strcat(arcs_cmdline, arg[i]);
        strcat(arcs_cmdline, " ");
    }

}


void (* hook_restart_func)(void) = NULL;

void rtk_hook_restart_function(void (*func)(void))
{
    hook_restart_func = func;
    return;
}


#if defined(CONFIG_RTL8390_SERIES)
void rtl8390_machine_restart(char *command)
{
    if(hook_restart_func != NULL)
    {
        hook_restart_func();
    }
    printk("System restart.\n");
    REG32(0xBB000014) = 0xFFFFFFFF;    /* Reset whole chip */

}
#endif

#if defined(CONFIG_RTL8380_SERIES)
void rtl8380_machine_restart(char *command)
{
    uint32 tmp = 0;

    if(hook_restart_func != NULL)
    {
        hook_restart_func();
    }

    OTTO838x_FLASH_DISABLE_4BYTE_ADDR_MODE();

    printk("System restart.\n");
    REG32(0xBB000040) = 0x1;    /* Reset Global Control Register */

}
#endif

#if defined(CONFIG_RTL9300_SERIES)
void rtl9300_machine_restart(char *command)
{
    if(hook_restart_func != NULL)
    {
        hook_restart_func();
    }
    printk("System restart.\n");

    /* Reset whole chip */
    REG32(0xBB00000C) = 0x1;
}
#endif

#if defined(CONFIG_RTL9310_SERIES)
void rtl9310_machine_restart(char *command)
{
    if(hook_restart_func != NULL)
    {
        hook_restart_func();
    }
    printk("System restart.\n");

    /* Reset whole chip */
    REG32(0xBB000400) = 0x1;
}
#endif


#if defined(CONFIG_RTL8390_SERIES)
static void rtl8390_timer_ack(void)
{
    unsigned int model_info;
    unsigned int chip_info;
    static unsigned int is_probe = 0, is_tc = 0;

    if (is_probe == 0)
    {
        model_info = REG32(0xBB000FF0);
        REG32(0xBB000FF4) = 0xA0000000;
        chip_info = REG32(0xBB000FF4);
        REG32(0xBB000FF4) = 0;
        if ((chip_info & 0xFFFF) == 0x0399)
            is_tc = 1;
        else
            is_tc = 0;
        is_probe = 1;
    }

    REG32(RTL8390MP_TC0INT) |= RTL8390MP_TCIP;

    return;
}

static void __init rtl8390_time_init(void)
{
}


static void __init rtl8390_timer_setup(struct irqaction *irq)
{
    unsigned int model_info;
    unsigned int chip_info;


    model_info = REG32(0xBB000FF0);
    REG32(0xBB000FF4) = 0xA0000000;
    chip_info = REG32(0xBB000FF4);
    REG32(0xBB000FF4) = 0;

    /* Setup Timer0 */

    /* Clear Timer IP status */
    if (REG32(RTL8390MP_TC0INT) & RTL8390MP_TCIP)
        REG32(RTL8390MP_TC0INT) |= RTL8390MP_TCIP;

    /* Here irq->handler is passed from outside */
    irq->handler = timer_interrupt;
    setup_irq(TC0_IRQ, irq);

    REG32(RTL8390MP_TC0CTL) = 0; /* disable timer before setting CDBR */
    REG32(TC09X8XDATA)= ((MHZ * 1000000)/(DIVISOR_RTL8390 * HZ));
    REG32(RTL8390MP_TC0CTL) = RTL8390MP_TCEN | RTL8390MP_TCMODE_TIMER | DIVISOR_RTL8390 ;
    REG32(RTL8390MP_TC0INT) = RTL8390MP_TCIE;

    return;
}
#endif /*End of CONFIG_RTL8390_SERIES*/

#if defined(CONFIG_RTL8380_SERIES)
static void rtl8380_timer_ack(void)
{
    unsigned int original_data_intRd;
    unsigned int temp = 0;
    static unsigned int is_probe = 0;

    if (is_probe == 0)
    {
        original_data_intRd = REG32(0xBB000058);
        REG32(0xBB000058) = (original_data_intRd | 0x3);
        temp = REG32(0xBB0000D4);
        REG32(0xBB000058) = original_data_intRd;
        is_probe = 1;
    }

    REG32(RTL8380MP_TC0INT) |= RTL8380MP_TCIP;

    return;
}

static void __init rtl8380_time_init(void)
{
}

static void __init rtl8380_timer_setup(struct irqaction *irq)
{
    unsigned int original_data_intRd;
    unsigned int original_data_chipRd;
    unsigned int temp = 0;
    unsigned int temp_chip_info = 0;

    original_data_intRd = REG32(0xBB000058);
    REG32(0xBB000058) = (original_data_intRd | 0x3);
    original_data_chipRd = REG32(0xBB0000D8);
    REG32(0xBB0000D8) = (original_data_chipRd | 0xA0000000);
    temp = REG32(0xBB0000D4);
    temp_chip_info = REG32(0xBB0000D8);
    REG32(0xBB0000D8) = original_data_chipRd;
    REG32(0xBB000058) = original_data_intRd;

    /* Setup Timer0 */

    /* Clear Timer IP status */
    if (REG32(RTL8380MP_TC0INT) & RTL8380MP_TCIP)
        REG32(RTL8380MP_TC0INT) |= RTL8380MP_TCIP;

    /* Here irq->handler is passed from outside */
    irq->handler = timer_interrupt;
    setup_irq(TC0_IRQ, irq);

    REG32(RTL8380MP_TC0CTL) = 0; /* disable timer before setting CDBR */
    REG32(TC09X8XDATA)= ((MHZ * 1000000)/(DIVISOR * HZ));
    REG32(RTL8380MP_TC0CTL) = RTL8380MP_TCEN | RTL8380MP_TCMODE_TIMER | DIVISOR ;
    REG32(RTL8380MP_TC0INT) = RTL8380MP_TCIE;

    return;
}
#endif /*End of CONFIG_RTL8380_SERIES*/


#if defined(CONFIG_RTL9300_SERIES)
static void rtl9300_timer_ack(void)
{
    REG32(RTL93XXMP_TC0INT) |= RTL93XXMP_TCIP;

    return;
}

static void __init rtl9300_time_init(void)
{
}


static void __init rtl9300_timer_setup(struct irqaction *irq)
{
    /* Setup Timer0 */

    /* Clear Timer IP status */
    if (REG32(RTL93XXMP_TC0INT) & RTL93XXMP_TCIP)
        REG32(RTL93XXMP_TC0INT) |= RTL93XXMP_TCIP;

    /* Here irq->handler is passed from outside */
    irq->handler = timer_interrupt;

    setup_irq(TC0_IRQ, irq);

     /* disable timer before setting CDBR */
    REG32(RTL93XXMP_TC0CTL) = 0;

    REG32(RTL93XXMP_TC0DATA)= ((MHZ_9300_MP * 1000000)/(DIVISOR_RTL9300 * HZ));

    REG32(RTL93XXMP_TC0CTL) = RTL93XXMP_TCEN | RTL93XXMP_TCMODE_TIMER | DIVISOR_RTL9300 ;

    REG32(RTL93XXMP_TC0INT) = RTL93XXMP_TCIE;

    return;
}
#endif /*End of CONFIG_RTL9300_SERIES*/


#if defined(CONFIG_RTL9310_SERIES)
static void rtl9310_timer_ack(void)
{
    REG32(RTL93XXMP_TC0INT) |= RTL93XXMP_TCIP;

    return;
}

static void __init rtl9310_time_init(void)
{
}


static void __init rtl9310_timer_setup(struct irqaction *irq)
{
    /* Setup Timer0 */

    /* Clear Timer IP status */
    if (REG32(RTL93XXMP_TC0INT) & RTL93XXMP_TCIP)
        REG32(RTL93XXMP_TC0INT) |= RTL93XXMP_TCIP;

    /* Here irq->handler is passed from outside */
    irq->handler = timer_interrupt;

    setup_irq(TC0_IRQ, irq);

     /* disable timer before setting CDBR */
    REG32(RTL93XXMP_TC0CTL) = 0;
    REG32(RTL93XXMP_TC0DATA)= ((MHZ_9300 * 1000000)/(DIVISOR_RTL9300 * HZ));
    REG32(RTL93XXMP_TC0CTL) = RTL93XXMP_TCEN | RTL93XXMP_TCMODE_TIMER | DIVISOR_RTL9300 ;

    REG32(RTL93XXMP_TC0INT) = RTL93XXMP_TCIE;

    return;
}
#endif /*End of CONFIG_RTL9310_SERIES*/


static void __init serial_init(void)
{
#ifdef CONFIG_SERIAL_8250
    struct uart_port s;

    memset(&s, 0, sizeof(s));

    s.type = PORT_16550A;
    s.membase = (unsigned char *) UART0_BASE;

    s.irq = UART0_IRQ;

    if((bsp_chip_family_id == RTL9310_FAMILY_ID) || (bsp_chip_family_id == RTL8390_FAMILY_ID) || (bsp_chip_family_id == RTL8350_FAMILY_ID)  || (bsp_chip_family_id == RTL8380_FAMILY_ID) || (bsp_chip_family_id == RTL8330_FAMILY_ID))
        s.uartclk = SYSCLK - BAUDRATE * 24;
    else
        s.uartclk = SYSCLK_9300_MP - BAUDRATE * 24;

    s.flags = UPF_SKIP_TEST | UPF_LOW_LATENCY | UPF_SPD_CUST;
    s.iotype = UPIO_MEM;
    s.regshift = 2;
    s.fifosize = 1;

    if((bsp_chip_family_id == RTL9310_FAMILY_ID) || (bsp_chip_family_id == RTL8390_FAMILY_ID) || (bsp_chip_family_id == RTL8350_FAMILY_ID)  || (bsp_chip_family_id == RTL8380_FAMILY_ID) || (bsp_chip_family_id == RTL8330_FAMILY_ID))
        s.custom_divisor = SYSCLK / (BAUDRATE * 16) - 1;
    else
        s.custom_divisor = SYSCLK_9300_MP / (BAUDRATE * 16) - 1;

    /* Call early_serial_setup() here, to set up 8250 console driver */
    if (early_serial_setup(&s) != 0) {
        prom_printf("Serial setup failed!\n");
    }
#endif
}

void __init plat_mem_setup(void)
{
#if defined(CONFIG_RTL8390_SERIES)
    if((bsp_chip_family_id == RTL8390_FAMILY_ID) || (bsp_chip_family_id == RTL8350_FAMILY_ID))
    {
        _machine_restart = rtl8390_machine_restart;

        /* Platform Specific Setup */
        serial_init();

        board_time_init = rtl8390_time_init;
        mips_timer_ack = rtl8390_timer_ack;
    }
#endif
#if defined(CONFIG_RTL8380_SERIES)
    if((bsp_chip_family_id == RTL8380_FAMILY_ID) || (bsp_chip_family_id == RTL8330_FAMILY_ID))
    {
        _machine_restart = rtl8380_machine_restart;

        /* Platform Specific Setup */
        serial_init();

        board_time_init = rtl8380_time_init;
        mips_timer_ack = rtl8380_timer_ack;
    }
#endif

#if defined(CONFIG_RTL9300_SERIES)
    if((bsp_chip_family_id == RTL9300_FAMILY_ID))
    {
        _machine_restart = rtl9300_machine_restart;

        /* Platform Specific Setup */
        serial_init();

        board_time_init = rtl9300_time_init;
        mips_timer_ack = rtl9300_timer_ack;
    }
#endif

#if defined(CONFIG_RTL9310_SERIES)
    if((bsp_chip_family_id == RTL9310_FAMILY_ID))
    {
        _machine_restart = rtl9310_machine_restart;

        /* Platform Specific Setup */
        serial_init();

        board_time_init = rtl9310_time_init;
        mips_timer_ack = rtl9310_timer_ack;
    }
#endif

}


void __init plat_timer_setup(struct irqaction *irq)
{
#if defined(CONFIG_RTL8390_SERIES)
    if((bsp_chip_family_id == RTL8390_FAMILY_ID) || (bsp_chip_family_id == RTL8350_FAMILY_ID))
        rtl8390_timer_setup(irq);
#endif
#if defined(CONFIG_RTL8380_SERIES)
    if((bsp_chip_family_id == RTL8380_FAMILY_ID) || (bsp_chip_family_id == RTL8330_FAMILY_ID))
        rtl8380_timer_setup(irq);
#endif
#if defined(CONFIG_RTL9300_SERIES)
    if((bsp_chip_family_id == RTL9300_FAMILY_ID) )
        rtl9300_timer_setup(irq);
#endif
#if defined(CONFIG_RTL9310_SERIES)
    if((bsp_chip_family_id == RTL9310_FAMILY_ID) )
        rtl9310_timer_setup(irq);
#endif
    return;
}

/* ===== Export BSP symbols ===== */
EXPORT_SYMBOL(rtk_hook_restart_function);
EXPORT_SYMBOL(drv_swcore_ioalCB_register);
EXPORT_SYMBOL(drv_swcore_earlyInit_end);

