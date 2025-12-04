#ifndef __ASM_ARM_AARCH64_SYSREG_A55_TOOL_H
#define __ASM_ARM_AARCH64_SYSREG_A55_TOOL_H
// copy from uboot arch/arm/linclude/asm/gic.h, arch/arm/lib/gicv3.c
// cp15_a17_tool.h


#include <linux/stringify.h>





/* Core sysreg macros */
#define read_sysreg(reg) ({					\
	uint64_t val;						\
	__asm__ volatile("mrs %0, " __stringify(reg) : "=r" (val));\
	val;							\
})

#define write_sysreg(val, reg) ({				\
	__asm__ volatile("msr " __stringify(reg) ", %0" : : "r" (val));\
})



/* Cpu Interface System Registers */
#define PMCR_EL0		S3_3_C9_C12_0
#define PMCNTENSET_EL0		S3_3_C9_C12_1
#define PMCCNTR_EL0		S3_3_C9_C13_0

#define PMCCFILTR_EL0   S3_3_C14_C15_3


/* PMCR_EL0 */
#define PMCR_EL0_P			(1U << 1)   /* Reset all counters, not include PMCCNTR_EL0 */
#define PMCR_EL0_C			(1U << 2)   /* Cycle counter reset */
#define PMCR_EL0_D			(1U << 3)  /* CCNT counts every 64th cpu cycle */
#define PMCR_EL0_E			(1U << 0)  /* Enable all counters */

// enable cycle counter
#define PMCNTENSET_EL0_C     (1U << 31) 


#define PMCCFILTR_EL0_P      (1U << 31)
#define PMCCFILTR_EL0_NSH      (1U << 27)




#endif /* __ASM_ARM_AARCH64_SYSREG_A55_TOOL_H */












