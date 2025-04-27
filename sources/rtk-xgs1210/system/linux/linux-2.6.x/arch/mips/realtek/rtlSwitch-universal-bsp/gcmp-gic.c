#include <linux/init.h>
#include <linux/irq.h>
#include <linux/sched.h>
#include <linux/smp.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/io.h>

#include <linux/kernel.h>

#include <asm/traps.h>

#include <asm/irq_cpu.h>
#include <asm/irq_regs.h>
#include <asm/gic.h>
#include <asm/gcmpregs.h>
#include "gcmp-gic.h"
int gcmp_present = -1;
int gic_present = -1;
unsigned long _gcmp_base;

#define GIC_CPU_NMI GIC_MAP_TO_NMI_MSK
#define X GIC_UNUSED
/* GIC_FLAG_TRANSPARENT : The GIS MASK is set,(IRQ ENABLE)*/

/*
 *   RTL9310 Interrupt Scheme
 *
 *   Source     IRQ      CPU INT   GIC_OUTPUT     CPU_CAUSE
 *   --------   ------   -------   ------------   --------
 *   UART0      31       2         GIC_CPU_INT1    IP3
 *   UART1      30       1         GIC_CPU_INT0    IP2
 *   TIMER0     29       5         GIC_CPU_INT4    IP6      HW4
 *   TIMER1     28       1         GIC_CPU_INT0    IP2
 *   OCPTO      27       1         GIC_CPU_INT0    IP2
 *   HLXTO      26       1         GIC_CPU_INT0    IP2
 *   SLXTO      25       1         GIC_CPU_INT0    IP2
 *   NIC        24       4         GIC_CPU_INT3    IP5
 *   GPIO_ABCD  23       4         GIC_CPU_INT3    IP5
 *   SWCORE     20       3       G  IC_CPU_INT2    IP4
 */

/*
 * MIPS 1004k/IA Interrupt Scheme
 *   GIC_FLAG_TRANSPARENT : The GIS MASK is set,(IRQ ENABLE).
 *                          irq_request/setup_irq also set GIS MASK.
 *
 *   Source     EXT_INT   IRQ      PIC Out  GIC_Out     CPU_CAUSE
 *                                 (GIC IN)
 *   --------   -------   ------   ------- ------------   --------
 *  VPE Num --- VPE's IP2~IP7 ---  Interrupt Polarity --- Trigger Type -- FLAG
********************************************************************************/

extern struct gic_intr_map rtl9310_gic_intr_map[GIC_NUM_INTRS];

/********************************************************
 * For 1004k Errat : E357 SYNC(3) Depends on SI_SyncCtl"
********************************************************/
void __init gcr_control_syncctl(void){
    unsigned long *gcr_control = NULL;

    gcr_control = (unsigned long *)(_gcmp_base + 0x10);
    *gcr_control |= (1<<16); /* For L2 enable */

}

/*
 * GCMP needs to be detected before any SMP initialisation
 */
int __init gcmp_probe(unsigned long addr, unsigned long size)
{

    if (gcmp_present >= 0)
        return gcmp_present;

    _gcmp_base = (unsigned long) ioremap_nocache(GCMP_BASE_ADDR, GCMP_ADDRSPACE_SZ);
    gcmp_present = (GCMPGCB(GCMPB) & GCMP_GCB_GCMPB_GCMPBASE_MSK) == GCMP_BASE_ADDR;
    if (gcmp_present){
      gcr_control_syncctl();
      pr_debug("GCMP present\n");
    }else{
        iounmap((void *)_gcmp_base);
    }
    return gcmp_present;
}

/* Return the number of IOCU's present */
int __init gcmp_niocu(void)
{
  return gcmp_present ?
    (GCMPGCB(GC) & GCMP_GCB_GC_NUMIOCU_MSK) >> GCMP_GCB_GC_NUMIOCU_SHF : 0;
}

/* Set GCMP region attributes */
void __init gcmp_setregion(int region, unsigned long base,
                           unsigned long mask, int type)
{
    GCMPGCBn(CMxBASE, region) = base;
    GCMPGCBn(CMxMASK, region) = mask | type;
}


void __init init_gic(void){

    /* Early detection of CMP support */
    if (gcmp_probe(GCMP_BASE_ADDR, GCMP_ADDRSPACE_SZ)){
        printk("GCMP present\n");
    }else{
        printk("No GCMP present! Not init GIC!\n");
        return;
    }

    if (gcmp_present)  {
        GCMPGCB(GICBA) = GIC_BASE_ADDR | GCMP_GCB_GICBA_EN_MSK;
        gic_present = 1;
    } else {
        return;
    }
    if (gic_present) {
#if defined(CONFIG_MIPS_MT_SMP)
        int i;
        gic_call_int_base = GIC_NUM_INTRS - NR_CPUS;
        gic_resched_int_base = gic_call_int_base - NR_CPUS;
        fill_ipi_map();
#endif
        gic_init(GIC_BASE_ADDR, GIC_ADDRSPACE_SZ, rtl9310_gic_intr_map,
                ARRAY_SIZE(rtl9310_gic_intr_map), MIPS_GIC_IRQ_BASE);

#if defined(CONFIG_MIPS_MT_SMP)
        /* set up ipi interrupts */
        if (cpu_has_vint) {
            set_vi_handler(MIPSCPU_INT_IPI0, malta_ipi_irqdispatch);
            set_vi_handler(MIPSCPU_INT_IPI1, malta_ipi_irqdispatch);
        }
        /* Argh.. this really needs sorting out.. */
        printk("CPU%d: status register was %08x\n", smp_processor_id(), read_c0_status());
        write_c0_status(read_c0_status() | STATUSF_IP3 | STATUSF_IP4);
        printk("CPU%d: status register now %08x\n", smp_processor_id(), read_c0_status());
        write_c0_status(0x1100dc00);
        printk("CPU%d: status register frc %08x\n", smp_processor_id(), read_c0_status());
        for (i = 0; i < NR_CPUS; i++) {
            arch_init_ipiirq(MIPS_GIC_IRQ_BASE +
                     GIC_RESCHED_INT(i), &irq_resched);
            arch_init_ipiirq(MIPS_GIC_IRQ_BASE +
                     GIC_CALL_INT(i), &irq_call);
        }
#endif
        }
}
