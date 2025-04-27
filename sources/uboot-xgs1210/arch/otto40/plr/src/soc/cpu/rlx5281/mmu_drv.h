#ifndef MMU_DRV_H
#define MMU_DRV_H

#include <soc.h>

#ifndef TLB_PAGE_SIZE
    #error "TLB_PAGE_SIZE should be defined, number of bytes per mmu page."
#endif

#define NUM_LEVEL           (4)         // should be power of 2 and >= 2

#ifndef MMU_VM_SIZE
    #error "MMU_VM_SIZE should be defined, it should be the maximal virtual memory size."
#endif


#define MMU_MIPS16      __attribute__ ((mips16))
#define MMU_NOMIPS16    __attribute__ ((nomips16))

extern SECTION_RECYCLE int mmu_drv_init(u32_t phy_start, u32_t phy_end) ;   // it returns the number of physical pages added into the mmu system

#endif //MMU_DRV_H

