/*
 * Realtek Semiconductor Corp.
 *
 * bsp/prom.c
 *     bsp early initialization code
 *
 * Copyright (C) 2006-2012 Tony Wu (tonywu@realtek.com)
 */
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <asm/bootinfo.h>
#include <asm/addrspace.h>
#include <asm/page.h>
#include <asm/cpu.h>
#include <asm/prom.h>
#include <linux/ioport.h>
#include "bspcpu.h"
#include "bspchip.h"
#include "chip_probe.h"
#include <int_setup.h>

#include <asm/mips-cm.h>
#include <asm/mips-cpc.h>

#define CONFIG_L2_CACHE_PRCOESS
#define CONFIG_AUTO_PROBE_DRAM_SIZE
#define CONFIG_REMOVE_CMDLINE_PARA

unsigned int bsp_chip_id, bsp_chip_rev_id;
unsigned int bsp_chip_family_id, bsp_chip_type;
unsigned int bsp_zone2_phy, bsp_zone2_base;
unsigned int bsp_zone0_size, bsp_zone2_size, bsp_dma_size;
unsigned int bsp_highmemory_start;

unsigned int bsp_Interrupt_srcID_Mapping_IRQ[GIC_NUM_INTRS];
unsigned int bsp_Interrupt_IRQ_Mapping_IP[ICTL_MAX_IP];
extern interrupt_mapping_conf_t rtl839X8X_intr_mapping[];
extern interrupt_mapping_conf_t rtl9300_intr_mapping[];
extern interrupt_mapping_conf_t rtl9310_intr_mapping[];
extern struct gic_intr_map rtl9310_gic_intr_map[];


extern int drv_swcore_cid_get(unsigned int unit, unsigned int *pCid, unsigned int *pCrevid);
extern void rtk_intr_func_init(void);

#define RT_ERR_OK       0
#define RT_ERR_FAILED   -1


#ifdef CONFIG_CMDLINE_BOOL
static char __initdata predefine_cmdline[COMMAND_LINE_SIZE] = CONFIG_CMDLINE;
#endif

void prom_bsp_memsize_info(unsigned int *low_memory_size, unsigned int *high_memory_size, unsigned int *dma_reserved_size)
{
    *low_memory_size = bsp_zone0_size;
#ifdef CONFIG_HIGHMEM
    *high_memory_size = bsp_zone2_size;
#else
    *high_memory_size = 0;
#endif
    *dma_reserved_size = bsp_dma_size;
}


void bsp_interrupt_irq_mapping_setup(void)
{
    int	index;
    int	irq_index;
    unsigned int	src_index;
    int system_irq;
    int int_type;
    interrupt_mapping_conf_t *intr_mapping_table;
    struct gic_intr_map *gic_mapping_table = NULL;

    rtk_intr_func_init();

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
        system_irq = (unsigned int)((interrupt_mapping_conf_t *)(intr_mapping_table + index)->mapped_system_irq);
        bsp_Interrupt_IRQ_Mapping_IP[system_irq] = src_index;
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

void bsp_console_init(void)
{
    unsigned int value = 0;
    /* 8 bits, 1 stop bit, no parity. */
    REG32(UART0_LCR) = ((CHAR_LEN_8 | ONE_STOP | PARITY_DISABLE) << 24);
    /* Reset/Enable the FIFO */
    REG32(UART0_FCR) = ((FCR_EN | RXRST | TXRST | CHAR_TRIGGER_14) << 24);
    /* Disable All Interrupts */
    REG32(UART0_IER) = 0x00000000;
    /* Enable Divisor Latch */
    REG32(UART0_LCR) |= (DLAB << 24);
    /* Set Divisor */
   if((bsp_chip_family_id == RTL9310_FAMILY_ID) || (bsp_chip_family_id == RTL8390_FAMILY_ID) || (bsp_chip_family_id == RTL8350_FAMILY_ID)  || (bsp_chip_family_id == RTL8380_FAMILY_ID) || (bsp_chip_family_id == RTL8330_FAMILY_ID))
   {
        value = (SYSCLK / (BAUDRATE * 16) - 1) & 0x00FF;
        REG32(UART0_DLL) = (value << 24);

        value = (((SYSCLK / (BAUDRATE * 16) - 1) & 0xFF00) >> 8);
        REG32(UART0_DLM) = (value << 24);
   }else{

        value = (SYSCLK_9300_MP / (BAUDRATE * 16) - 1) & 0x00FF;
        REG32(UART0_DLL) = (value << 24);

        value = (((SYSCLK_9300_MP / (BAUDRATE * 16) - 1) & 0xFF00) >> 8);
        REG32(UART0_DLM) = (value << 24);
   }

    /* Disable Divisor Latch */
    REG32(UART0_LCR) &= ((~DLAB) << 24);

}

#ifdef CONFIG_HIGHMEM
void __init prom_mem_zone2(void){
    if(bsp_chip_family_id == RTL9300_FAMILY_ID)
    {
        REG32(RTL9300_O0DOR2) = RTL9300_ZONE2_OFF;
        REG32(RTL9300_O0DMAR2) = RTL9300_ZONE2_MAX;    //1G
        __sync();
        bsp_zone2_phy = RTL9300_ZONE2_PHY;
        bsp_zone2_base = RTL9300_ZONE2_BASE;
        PROM_DEBUG_PRINT("mem zone2: OFF=0x%x,MAX=0x%x\n",REG32(RTL9300_O0DOR2) ,  REG32(RTL9300_O0DMAR2));
    }
    if(bsp_chip_family_id == RTL9310_FAMILY_ID)
    {
        /*Hardcoding bsp_zone2_phy = 0x0, bsp_zone2_base = 0xffffffff
        here due to zone2 is designed for 9300 bus property.
        9310 Shepia need to redesign for highmem architechture.*/
        bsp_zone2_phy = 0x0; /* DMA is Shepia view(Logical address)*/
        bsp_zone2_base = 0xffffffff;
    }
}

void __init prom_sram_disable(void){
    if(bsp_chip_family_id == RTL9300_FAMILY_ID)
    {
        REG32(RTL9300_UMSAR0) = 0;
        REG32(RTL9300_UMSAR1) = 0;
        REG32(RTL9300_UMSAR2) = 0;
        REG32(RTL9300_UMSAR3) = 0;
        REG32(RTL9300_SRAMSAR0) = 0;
        REG32(RTL9300_SRAMSAR1) = 0;
        REG32(RTL9300_SRAMSAR2) = 0;
        REG32(RTL9300_SRAMSAR3) = 0;
    }
}
#endif

void plat_remove_mem_parameter(char * p)
{
#ifdef    CONFIG_REMOVE_CMDLINE_PARA

     char *ptr;
     char *ptr_mem;
     char *endptr;	/* local pointer to end of parsed string */
     unsigned long long ret = 0;

     ptr = strstr(p, "mem=");
     ptr_mem= ptr+4;
     PROM_DEBUG_PRINT("cmdline=%s, %p, %p\n", p, p, ptr);
     if(ptr){
        ret = simple_strtoull(ptr_mem, &endptr, 0);
        memmove(ptr, (endptr+1), strlen((endptr+1)) + 1);
}
     PROM_DEBUG_PRINT("cmdline=%s\n", p);
#else
    return;
#endif
}


static int rtl9300_auto_probe_memsize(unsigned int * memory_size)
{
    const unsigned char BNKCNTv[] = {1, 2, 3};
    const unsigned char BUSWIDv[] = {0, 1, 2};
    const unsigned char ROWCNTv[] = {11, 12, 13, 14, 15, 16};
    const unsigned char COLCNTv[] = {8, 9, 10, 11, 12};
    unsigned int dcr = *((volatile int *)(RTL9300_DRAM_DCR));
    unsigned int byte_size;

    byte_size = (1 << (BNKCNTv[(dcr >> 28) & 0x3] +
                 BUSWIDv[(dcr >> 24) & 0x3] +
                 ROWCNTv[(dcr >> 20) & 0xF] +
                 COLCNTv[(dcr >> 16) & 0xF]));

    PROM_DEBUG_PRINT("AUTO byte_size = 0x%08x Byte\n",byte_size);

    if((byte_size != 0x4000000) && (byte_size != 0x8000000) && (byte_size != 0x10000000) && (byte_size != 0x20000000) && (byte_size != 0x40000000))
    {
        *memory_size = 0;
        return RT_ERR_FAILED;
    }else{
        *memory_size = byte_size;
        return RT_ERR_OK;

    }
}

static int rtl9310_auto_probe_memsize(unsigned int * memory_size)
{
    unsigned int v = *((volatile int *)(RTL9310_DRAM_MR6));
    unsigned int b = v >> 12;
    unsigned int r = (v >> 6) & 0x3F;
    unsigned int c = v & 0x3F;
    unsigned int byte_size = (1 << (b+r+c));

    PROM_DEBUG_PRINT("AUTO byte_size = 0x%08x Byte\n",byte_size);

    if((byte_size != 0x4000000) && (byte_size != 0x8000000) && (byte_size != 0x10000000) && (byte_size != 0x20000000) && (byte_size != 0x40000000) && (byte_size != 0x80000000))
    {
        *memory_size = 0;
        return RT_ERR_FAILED;
    }else{
        *memory_size = byte_size;
        return RT_ERR_OK;

    }
}

static int prom_auto_probe_meminfo(unsigned int * memory_size)
{
#ifdef CONFIG_AUTO_PROBE_DRAM_SIZE
    if(bsp_chip_family_id == RTL9310_FAMILY_ID)
    {
        if(rtl9310_auto_probe_memsize(memory_size) == RT_ERR_OK)
            return RT_ERR_OK;
    }else if(bsp_chip_family_id == RTL9300_FAMILY_ID){
        if(rtl9300_auto_probe_memsize(memory_size) == RT_ERR_OK)
            return RT_ERR_OK;
    }else{
        return RT_ERR_FAILED;
}
#endif
    return RT_ERR_FAILED;
}

static int prom_get_memsize_from_cmdline(unsigned int * memory_size)
{
    char *ptr;

    /* Check the command line first for a memsize directive */
    ptr = strstr(arcs_cmdline, "mem=");

    if (ptr)
       *memory_size = memparse(ptr + 4, &ptr);
    else{
        ptr = strstr(predefine_cmdline, "mem=");
        if (ptr){
            *memory_size = memparse(ptr + 4, &ptr);
        }else{
            *memory_size = 0x08000000;  /* Default to 128MB */
            return RT_ERR_FAILED; /*Cannot get "mem" from command line*/
        }
    }
    return RT_ERR_OK;
    }

static int prom_get_dmasize_from_cmdline(unsigned int * dma_size)
{
    char *ptr;

    ptr = strstr(arcs_cmdline, "rtk_dma_size=");

    if (ptr)
        *dma_size = memparse(ptr + 13, &ptr);
    else{
        ptr = strstr(predefine_cmdline, "rtk_dma_size=");
        if (ptr){
            *dma_size = memparse(ptr + 13, &ptr);
        }else{
            *dma_size = 0x0;  /* Default to 0 */
            return RT_ERR_FAILED; /*Cannot get "rtk_dma_size" from command line*/
        }
    }

    return RT_ERR_OK;

}

static int prom_memory_size_get(unsigned int * memory_size, unsigned int * dma_size)
{
    unsigned int total_memsize, dmasize;
    int ret;

    *memory_size = 0;
    *dma_size = 0;

    if(prom_auto_probe_meminfo(&total_memsize) == RT_ERR_OK)
    {
        PROM_DEBUG_PRINT("Get total memory size by auto probe result\n");
        /* Get total memory size by auto probe result */
        *memory_size = total_memsize;
        PROM_DEBUG_PRINT("Get dma size from kernel commnad line\n");
        /* Get dma size from kernel commnad line */
        ret = prom_get_dmasize_from_cmdline(&dmasize);
        *dma_size = dmasize;
    }else{
        PROM_DEBUG_PRINT("Get total memory size and dma size from by kernel command line\n");
        /* Get total memory size and dma size from by kernel command line */
        ret = prom_get_memsize_from_cmdline(&total_memsize);
        /* Get dma size from kernel commnad line */
        ret = prom_get_dmasize_from_cmdline(&dmasize);
        *memory_size = total_memsize;
        *dma_size = dmasize;
    }
#ifndef CONFIG_HIGHMEM
    /* If High Memory is not enabled, and "mem=" is larger than ZONE0 */
    /* Modify the memory size, abd let it equals to ZONE0 SIZE */
    if(bsp_chip_family_id == RTL9300_FAMILY_ID)
    {
        if(*memory_size > RTL9300_ZONE0_SIZE)
            *memory_size = RTL9300_ZONE0_SIZE;
    }else if(bsp_chip_family_id == RTL9310_FAMILY_ID){
        if(*memory_size > RTL9310_ZONE0_SIZE)
            *memory_size = RTL9310_ZONE0_SIZE;
    }else{
        if(*memory_size > RTL9X8X_ZONE0_SIZE)
            *memory_size = RTL9X8X_ZONE0_SIZE;
    }
#endif

    return RT_ERR_OK;
    }


void __init prom_meminit(void)
{
    unsigned int memsize = 0, dmasize = 0;
    unsigned int zone0_memsize = 0;
#ifdef CONFIG_HIGHMEM
    unsigned int zone2_memsize = 0;
#endif
    int ret;

    ret = prom_memory_size_get(&memsize, &dmasize);

    bsp_zone0_size = 0;
    bsp_zone2_size = 0;
    bsp_dma_size = dmasize;

    PROM_DEBUG_PRINT("DMA size=0x%x(B)\n",dmasize);

    /*
     * call <add_memory_region> to register boot_mem_map
     * add_memory_region(base, size, type);
     * type: BOOT_MEM_RAM, BOOT_MEM_ROM_DATA or BOOT_MEM_RESERVED
     */
    if(bsp_chip_family_id == RTL9300_FAMILY_ID)
    {
#ifdef CONFIG_HIGHMEM
        if(memsize > RTL9300_ZONE0_SIZE)
        {
            zone0_memsize = RTL9300_ZONE0_SIZE - dmasize;
            zone2_memsize = memsize - RTL9300_ZONE0_SIZE;
            bsp_zone0_size = zone0_memsize;
            bsp_zone2_size = zone2_memsize;
            bsp_highmemory_start = zone0_memsize;
            prom_sram_disable();
            prom_mem_zone2();
            add_memory_region(RTL9300_ZONE0_BASE, zone0_memsize, BOOT_MEM_RAM);
            add_memory_region(RTL9300_ZONE2_BASE, zone2_memsize, BOOT_MEM_RAM);
            PROM_DEBUG_PRINT("mem zone0: Base=0x%x, size=0x%x(B)\n",RTL9300_ZONE0_BASE, (unsigned int)zone0_memsize);
            PROM_DEBUG_PRINT("mem zone2: Base=0x%x, size=0x%x(B)\n",RTL9300_ZONE2_BASE, (unsigned int)zone2_memsize);
        }else{
            zone0_memsize = memsize - dmasize;
            bsp_zone0_size = zone0_memsize;
            bsp_highmemory_start = zone0_memsize;
            add_memory_region(RTL9300_ZONE0_BASE, zone0_memsize, BOOT_MEM_RAM);
            PROM_DEBUG_PRINT("mem zone0: Base=0x%x, size=0x%x(B)\n",RTL9300_ZONE0_BASE, (unsigned int)zone0_memsize);
        }
#else
        PROM_DEBUG_PRINT("memsize=0x%x, dmasize=0x%x(B)\n",(unsigned int)memsize, (unsigned int)dmasize);
        zone0_memsize = memsize - dmasize;
        bsp_zone0_size = zone0_memsize;
        bsp_highmemory_start = zone0_memsize;
        add_memory_region(RTL9300_ZONE0_BASE, zone0_memsize, BOOT_MEM_RAM);
#endif
    }else if(bsp_chip_family_id == RTL9310_FAMILY_ID)
    {
#ifdef CONFIG_HIGHMEM
        if(memsize > RTL9310_ZONE0_SIZE)
        {
            zone0_memsize = RTL9310_ZONE0_SIZE - dmasize;
            zone2_memsize = memsize - RTL9310_ZONE0_SIZE;
            bsp_zone0_size = zone0_memsize;
            bsp_zone2_size = zone2_memsize;
            bsp_highmemory_start = zone0_memsize;
            /*highmemory_start means the end of normal memory acorrding to the design of MIPS.
                In Sheipa arch, highmem and normal mem are not contiguous.
                So, We cannot set the beginning of highmem as highmem start.*/
            prom_mem_zone2();
            add_memory_region(RTL9310_ZONE0_BASE, zone0_memsize, BOOT_MEM_RAM);
            /* To Avoid add_memory_region() size over flow */
            if(zone2_memsize >= RTL9310_ZONE2_SIZE_BOUND)
                zone2_memsize = RTL9310_ZONE2_SIZE_LIMIT;
            add_memory_region(RTL9310_ZONE2_BASE, zone2_memsize, BOOT_MEM_RAM);
            PROM_DEBUG_PRINT("mem zone0: Base=0x%x, size=0x%x(B)\n",RTL9310_ZONE0_BASE, (unsigned int)zone0_memsize);
            PROM_DEBUG_PRINT("mem zone2: Base=0x%x, size=0x%x(B)\n",RTL9310_ZONE2_BASE, (unsigned int)zone2_memsize);
        }else{
            zone0_memsize = memsize - dmasize;
            bsp_zone0_size = zone0_memsize;
            bsp_highmemory_start = zone0_memsize;
            add_memory_region(RTL9310_ZONE0_BASE, zone0_memsize, BOOT_MEM_RAM);
            PROM_DEBUG_PRINT("mem zone0: Base=0x%x, size=0x%x(B)\n",RTL9310_ZONE0_BASE, (unsigned int)zone0_memsize);
        }
#else
        zone0_memsize = memsize - dmasize;
        bsp_zone0_size = zone0_memsize;
        bsp_highmemory_start = zone0_memsize;
        add_memory_region(RTL9310_ZONE0_BASE, zone0_memsize, BOOT_MEM_RAM);
#endif
    }else{ /* RTL838xM/839xM *//* NO HIGHMEM */
        zone0_memsize = memsize - dmasize;
        bsp_zone0_size = zone0_memsize;
        bsp_highmemory_start = zone0_memsize;
        add_memory_region(RTL9X8X_ZONE0_BASE, zone0_memsize, BOOT_MEM_RAM);
    }
}


#ifdef CONFIG_EARLY_PRINTK
static int promcons_output __initdata = 0;

void unregister_prom_console(void)
{
    if (promcons_output)
        promcons_output = 0;
}

void disable_early_printk(void)
    __attribute__ ((alias("unregister_prom_console")));

void prom_putchar(char c)
{
}

char prom_getchar(void)
{
    return '\0';
}
#endif

const char *get_system_type(void)
{
    if(bsp_chip_family_id == RTL8390_FAMILY_ID)
        return "RTL839xM";
    if(bsp_chip_family_id == RTL8350_FAMILY_ID)
        return "RTL835xM";
    if(bsp_chip_family_id == RTL8380_FAMILY_ID)
        return "RTL838xM";
    if(bsp_chip_family_id == RTL8330_FAMILY_ID)
        return "RTL833xM";
    if(bsp_chip_family_id == RTL9300_FAMILY_ID)
        return "RTL9300";
    if(bsp_chip_family_id == RTL9310_FAMILY_ID)
        return "RTL9310";

    return "CHIP recognization error";
}

void __init prom_free_prom_memory(void)
{
    return;
}

/* Do basic initialization */
phys_addr_t mips_cpc_default_phys_base(void)
{
    return CPC_BASE_ADDR;
}
/**************************************************
 * For L2 Cache initialization
**************************************************/
#ifdef CONFIG_L2_CACHE_PRCOESS
extern void init_l2_cache(void);
void __init bsp_setup_scache(void)
{
    unsigned long config2;
    unsigned int tmp;
    config2 = read_c0_config2();
    tmp = (config2 >> 4) & 0x0f;
    /*if enable l2_bypass mode, linesize will be 0       */
    /*if arch not implement L2cache, linesize will be 0  */
    if (0 < tmp && tmp <= 7){ //Scache linesize >0 and <=256 (B)
        printk("\nExecute init_l2_cache()\n");
        init_l2_cache();
    }
}
#endif
void __init prom_init(void)
{
    int i, ret;
    int argc = fw_arg0;
    char **arg = (char **)fw_arg1;

    extern void plat_smp_init(void);

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


//	mips_set_machine_name(get_system_type());

    bsp_chip_id = 0;
    bsp_chip_rev_id = 0;
    bsp_chip_family_id = 0;
    bsp_chip_type = 0;

    drv_swcore_earlyInit_start();
    ret = drv_swcore_cid_get((unsigned int)0, (unsigned int *)&bsp_chip_id, (unsigned int *)&bsp_chip_rev_id);
    if(ret == -1)
    {
        printk("\nbsp_init(), RTK Switch chip is not found!!!\n");
    }else
    {
        bsp_chip_family_id = FAMILY_ID(bsp_chip_id);
    }

    mips_set_machine_name(get_system_type());
    bsp_interrupt_irq_mapping_setup();

    if((bsp_chip_family_id == RTL9310_FAMILY_ID))
    {
        /* vv-Early detection of CMP support (For GIC init)*/
        mips_cm_probe();
        mips_cpc_probe();
        /* ^^-Early detection of CMP support */
#ifdef CONFIG_L2_CACHE_PRCOESS
        bsp_setup_scache();
#endif
    }

#ifdef CONFIG_SMP
        plat_smp_init();
#endif

}


