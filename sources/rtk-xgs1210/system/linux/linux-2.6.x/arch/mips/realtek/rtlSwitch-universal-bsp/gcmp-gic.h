#ifndef __RTK_SOC_GCMP_GIC_H__
#define __RTK_SOC_GCMP_GIC_H__


/* MIPS32R2
* *CP0, Others
*/
#define CP0_INTCTL	$12,1
#define CP0_SRSCTL	$12,2
#define CP0_SRSMAP	$12,3
#define CP0_EBASE	$15,1
#define CP0_CONFIG0	$16,0
#define CP0_CONFIG1	$16,1
#define CP0_CONFIG2	$16,2
#define CP0_CONFIG3	$16,3
#define CP0_CONFIG7     $16,7
#define CP0_DTAGLO	$28,2
#define CP0_TAGLO2	$28,4
#define CP0_DATALO	$28,1
#define CP0_IDATALO	$28,1
#define CP0_DDATALO	$28,3
#define CP0_DATALO2	$28,5
#define CP0_DATAHI	$29,1
#define CP0_DATAHI2	$29,5





/*
 * GCMP Specific definitions
 */
#define GCMP_BASE_ADDR		0x1fbf8000
#define GCR_CONFIG_ADDR     	0xbfbf8000  // KSEG0 address of the GCR registers
#define GCMP_ADDRSPACE_SZ		(256 * 1024)

#define GCR_CONTROL     	0xbfbf8010
/*
 * GIC Specific definitions
 */
// #define GIC_BASE_ADDR			0x1bdc0000
#define GIC_BASE_ADDR			0x1ddc0000
// #define GIC_BASE_ADDR			0xCE000000
#define GIC_ADDRSPACE_SZ		(128 * 1024)

/*
 * CPU core Interrupt Numbers
 */
#define MIPS_CPU_IRQ_BASE 0
/*
 * IRQ Controller
 */
#define BSP_IRQ_CPU_BASE	0
#define BSP_IRQ_CPU_NUM		8

#define BSP_IRQ_ICTL_BASE	(BSP_IRQ_CPU_BASE + BSP_IRQ_CPU_NUM)
#define BSP_IRQ_ICTL_NUM	32



/* GIC's Nomenclature for Core Interrupt Pins on the Malta */
#define GIC_CPU_INT0		0 /* Core Interrupt 2 	*/
#define GIC_CPU_INT1		1 /* .			*/
#define GIC_CPU_INT2		2 /* .			*/
#define GIC_CPU_INT3		3 /* .			*/
#define GIC_CPU_INT4		4 /* .			*/
#define GIC_CPU_INT5		5 /* Core Interrupt 5   */

#define GIC_EXT_INTR(x)		x

#define MIPS_GIC_IRQ_BASE	(BSP_IRQ_CPU_BASE)

/* External Interrupts used for IPI */
#define GIC_IPI_EXT_INTR_RESCHED_VPE0	(MIPS_GIC_IRQ_BASE + 32 - 8)
#define GIC_IPI_EXT_INTR_CALLFNC_VPE0	(GIC_IPI_EXT_INTR_RESCHED_VPE0 + 1)
#define GIC_IPI_EXT_INTR_RESCHED_VPE1	(GIC_IPI_EXT_INTR_RESCHED_VPE0 + 2)
#define GIC_IPI_EXT_INTR_CALLFNC_VPE1	(GIC_IPI_EXT_INTR_RESCHED_VPE0 + 3)
#define GIC_IPI_EXT_INTR_RESCHED_VPE2	(GIC_IPI_EXT_INTR_RESCHED_VPE0 + 4)
#define GIC_IPI_EXT_INTR_CALLFNC_VPE2	(GIC_IPI_EXT_INTR_RESCHED_VPE0 + 5)
#define GIC_IPI_EXT_INTR_RESCHED_VPE3	(GIC_IPI_EXT_INTR_RESCHED_VPE0 + 6)
#define GIC_IPI_EXT_INTR_CALLFNC_VPE3	(GIC_IPI_EXT_INTR_RESCHED_VPE0 + 7)

#define GIC_BASE_SIZE		    (128 * 1024)
/* Constants */
#define GIC_POL_POS			1
#define GIC_POL_NEG			0
#define GIC_TRIG_EDGE			1
#define GIC_TRIG_LEVEL			0

// #define GIC_NUM_INTRS			(24 + NR_CPUS * 2)
#define GIC_NUM_INTRS			48

#ifndef __ASSEMBLY__

extern int gcmp_present;
extern int gic_present;
extern unsigned long _gcmp_base;
//extern struct gic_pcpu_mask pcpu_masks[NR_CPUS];

extern int __init gcmp_probe(unsigned long addr, unsigned long size);
extern int gcmp_niocu(void);
extern void gcmp_setregion(int region, unsigned long base, unsigned long mask, int type);
extern void init_gic(void);
#endif
#endif
