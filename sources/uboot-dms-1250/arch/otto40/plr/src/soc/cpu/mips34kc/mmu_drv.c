#include <soc.h>
#include <cpu/tlb.h>
#include <cpu/mmu_drv.h>
#include <spi_nand/spi_nand_util.h>

#ifdef MSG_FAIL_LOAD_PAGE
    const char MSG_FAIL_LOAD_PAGE[] SECTION_SDATA = {"loading virtual page %d fail\n"};
#endif

#ifndef MMU_READ_PAGE_FROM_FLASH
    #error "MMU_READ_PAGE_FROM_FLASH((void*)dst_addr, (u32_t)flash_addr, (u32_t)page_size) should be defined"
#endif

#ifndef MMU_PHY_BASE_ADDR
    #warning "MMU_PHY_BASE_ADDR sould be defined, default to 0x9fc00000"
    #define MMU_PHY_BASE_ADDR       (0x9fc00000)
#endif

#ifndef MMU_VM_BASE_ADDR
    #define MMU_VM_BASE_ADDR        OTTO_KSEG2_BASE
#endif

#ifndef TLB_PAGE_SIZE
    #error "MMU_PHY_SIZE should be defined, it could be the total sram size."
#endif

// the function should be implemented in some .S
extern MMU_NOMIPS16 void tlb_update_badvaddr_entry(unsigned entry_hi, unsigned entry_lo0, unsigned entry_lo1);

// -----------------------------
// ASSUMPTION:
//      1. VM address mapped address is from MMU_VM_BASE_ADDR to MMU_VM_SIZE
//      2. NUM_LEVEL should be power of 2
//      3. only one piece of physical memory which starts from MMU_PHY_BASE_ADDR
//      4. the function read_page_from_flash() should be implemented
// -----------------------------

// for mips3kc, each entry contains 2 pages, each page size is 1<<12 
// simple conduction
#define NUM_VA_PAGES            ((MMU_VM_SIZE+TLB_PAGE_SIZE-1)/(TLB_PAGE_SIZE))
#define LEVEL_MASK              (NUM_LEVEL-1)
#define NUM_PA_PAGES            ((MMU_PHY_SIZE)>>(TLB_PAGE_SHIFT))
#define PA_BY_PAGE(pa_num)      ((((u32_t)(pa_num))<<TLB_PAGE_SHIFT)+(MMU_PHY_BASE_ADDR))
#define PROMO_LEVEL(l)          (((l)+1)&LEVEL_MASK)
#define PROMO_LEVEL_2(l)        (((l)+2)&LEVEL_MASK)

//#define PROMO_LEVEL_N(l, n)     (((l)+n)&LEVEL_MASK)
#define PA_TO_PFN(pa)           (pa>>TLB_PAGE_SHIFT)

struct page_st;
struct va_st;
typedef struct list_st {
    struct list_st *head, **tail;
} list_t;
typedef struct page_st {
    list_t entry;
    u32_t pa;
    u32_t vpage_num;
    u32_t ent_lo_idx;
    u32_t list_num;
} page_t;
#define page_next       entry.head
#define page_prev       entry.tail
#define PSVA_FREE       (0xcafecafe)    // this phy page has no virtual memory mapped
#define PSVA_NOT_VM     (0xdeaddead)    // this phy page is not used in virtual memory

static list_t           _list[NUM_LEVEL] SECTION_SBSS;
static page_t           _page[NUM_PA_PAGES] SECTION_SBSS;
static unsigned         _lowest_level SECTION_SDATA =0;
static tlb_entry_hi_t   _current_asid SECTION_SDATA ={.v=0};

#define ASID_DELTA	        (1<<TLB_ASID_SHIFT)
#define INCREASE_ASID()     (_current_asid.v=(_current_asid.v+ASID_DELTA)&TLB_ASID_MASK)
#define SYNC_ENTRYHI(hi)    (((hi) & ~(TLB_ASID_MASK)) | _current_asid.v)
#define GET_ENTRYHI(bvaddr) (bvaddr&TLB_ENTRYHI_MASK)
#define ENTRYLO_IDX(bvaddr) ((bvaddr>>TLB_PAGE_SHIFT)&0x1)

#define get_page_num_by_page(p) ({const page_t *q=p; q-_page;})

#define sync_cache_page(page_pa) ({u32_t __page_from=(page_pa), __page_to=__page_from+TLB_PAGE_SIZE-1; icache_inv(__page_from, __page_to); dcache_wr_inv(__page_from, __page_to);})

SECTION_UNS_TEXT MMU_NOMIPS16 static void
invalid_tlb_entry(u32_t vpage_num, u32_t lo_index) {
    u32_t cp0_index = (vpage_num>>1);
    if(lo_index)
        TLB_SET_ENTRYLO1_VALID(cp0_index, 0);
    else
        TLB_SET_ENTRYLO0_VALID(cp0_index, 0);
}

SECTION_UNS_TEXT MMU_MIPS16 static void 
insert_to_list_tail(u32_t list_num, page_t *p) {
    list_t *l=_list+list_num;
    p->page_prev=l->tail;
    p->page_next=*(l->tail);
    *(l->tail)=&(p->entry);
    l->tail=&(p->page_next);
}

SECTION_UNS_TEXT MMU_MIPS16 static void
move_to_list(u32_t new_list_num, page_t *p) {
    p->page_next->tail=p->page_prev;
    *(p->page_prev)=p->page_next;
    insert_to_list_tail(new_list_num, p);
}

extern u32_t start_of_mapping_area SECTION_RO;
extern u32_t end_of_mapping_area SECTION_RO;


static char _init_error_size_msg[] SECTION_RECYCLE_DATA = {"size of virtual memory (0x%08x) is larger than predefined size (0x%08x)\n"};

SECTION_RECYCLE int
mmu_drv_init(u32_t phy_start, u32_t phy_end) {
#ifdef MMU_CHECK_INETRNAL_ERROR
    u32_t mapping_area_size=(u32_t)&end_of_mapping_area-(u32_t)&start_of_mapping_area;
    if (mapping_area_size>MMU_VM_SIZE) {
        printf(_init_error_size_msg , mapping_area_size, MMU_VM_SIZE);
        while(1);
    }
#endif
    // set tlb page mask
    TLB_SET_PAGEMASK(0);

    // clear all valid bits in TLB
    tlb_entry_lo_t lo0={.f={
		.pfn=0,
		.c=CCAE_UC,
		.d=0,
		.v=TLB_INVALID,
		.g=0,
		.mbz=0
		}};
    tlb_entry_lo_t lo1={.f={
		.pfn=0,
		.c=CCAE_UC,
		.d=0,
		.v=TLB_INVALID,
		.g=0,
		.mbz=0
		}};

    tlb_entry_hi_t hi={.f={
        .vpn2=OTTO_KSEG2_BASE>>(TLB_PAGE_SHIFT+1), 
        .asid=0
    }};
    unsigned int vpage_num=0;
    for (;vpage_num<TLB_NUM_TLB_ENTRY; ++vpage_num, ++hi.f.vpn2) {
        TLB_SET_ENTRY(vpage_num, hi.v, lo0.v, lo1.v);
    }

    // clear up internal structure
    inline_wzero(_page, sizeof(_page));
    
    list_t *l=_list, *le=_list+NUM_LEVEL;
    for (;l!=le;++l) {
        l->head=l;
        l->tail=&(l->head);
    }

    page_t *p=_page;
    u32_t i, c;
//    phy_end &= (TLB_PAGE_SIZE-1);
    for (i=0, c=0; i<NUM_PA_PAGES; ++i,++p) {
        u32_t pa=PA_BY_PAGE(i);
        p->pa=pa;
        if ((pa>=phy_start) && (pa<phy_end)) {
            p->vpage_num=PSVA_FREE;
            insert_to_list_tail(0, p);
            ++c;
        } else {
            p->vpage_num=PSVA_NOT_VM;
        }
    }

    return c;
}

SECTION_UNS_TEXT void
mmu_tlb_miss_va_to_pa(u32_t badvaddr, u32_t entry_lo0, u32_t entry_lo1, u32_t vpage_num)  {
#ifdef MMU_CHECK_INETRNAL_ERROR
    while (badvaddr<MMU_VM_BASE_ADDR);                  // out of range, not translate
    while (badvaddr>=(MMU_VM_BASE_ADDR+MMU_VM_SIZE));   // out of range, not translate
    tlb_entry_lo_t lo0={.v=entry_lo0};
    tlb_entry_lo_t lo1={.v=entry_lo1};
    while (lo0.f.v && lo1.f.v) ;                        // should be done in mmu_tlb_miss_check_first()
#endif    

    u32_t ent_lo_idx=ENTRYLO_IDX(badvaddr);
    u8_t inv=0;

    // find a victim
    list_t *l=_list+_lowest_level;
    while(l->head==l) {
        _lowest_level=PROMO_LEVEL(_lowest_level);
        l=_list+_lowest_level;
    }
    page_t *vic=(page_t *)l->head;

#ifdef MMU_CHECK_INETRNAL_ERROR
    while (vic==VZERO);
#endif

    // this virtual page should be not mapped to any phyical page
    u32_t vic_vpage_num=vic->vpage_num;
    u32_t vic_ent_lo_idx=vic->ent_lo_idx;
#ifdef MMU_CHECK_INETRNAL_ERROR
    while (vic_vpage_num==PSVA_NOT_VM);
    while ((vic_vpage_num!=PSVA_FREE)&&(vic_vpage_num>=NUM_VA_PAGES));
#endif
    
    // update vpage_num of this victim
    if (vic_vpage_num!=PSVA_FREE) {
        invalid_tlb_entry(vic_vpage_num, vic_ent_lo_idx);
        inv=1;
    }
    vic->vpage_num=vpage_num;
    vic->ent_lo_idx=ent_lo_idx;
    
    // update the weight of this phyical page
    move_to_list(PROMO_LEVEL_2(_lowest_level), vic);
    
    // I/D-cache flush and invalidate
    u32_t vic_pa=vic->pa;
    u32_t vic_va=(vic_pa|0x80000000);
    sync_cache_page(vic_va);

    if (MMU_READ_PAGE_FROM_FLASH((void*)vic_va, vpage_num)) {
        #ifdef MSG_FAIL_LOAD_PAGE
            printf(MSG_FAIL_LOAD_PAGE, vpage_num);
        #endif
        while(1);
    }

    // update entrylo buddy
    tlb_entry_lo_t lo_bud = {.v=(ent_lo_idx)?entry_lo0:entry_lo1};
    if (lo_bud.f.pfn) {
        u32_t ppage_num = lo_bud.f.pfn&(OTTO_SRAM_MASK>>TLB_PAGE_SHIFT);
        page_t *p=_page+ppage_num;

        // increase the weight of this phyical page
        u32_t current_list_num=p->list_num;
        u32_t promote_list_num=PROMO_LEVEL(current_list_num); 
        if (promote_list_num!=_lowest_level) {
            move_to_list(promote_list_num, p);
        }
    }

    // clean up all entries in TLB (by increase global asid), which means all TLB 
    //  entries will need to be refill so that the access can be counted
    INCREASE_ASID();
    u32_t entry_hi=GET_ENTRYHI(badvaddr);
    u32_t new_hi=SYNC_ENTRYHI(entry_hi);

    tlb_entry_lo_t new_lo={.f={
		.pfn=PA_TO_PFN(vic_pa),
		.c=CCAE_CA_NC_CO_WB_WA,
		.d=0,
		.v=TLB_VALID,
		.g=0,
		.mbz=0
		}};

    // re-check ENTRYLO0 and ENTRYLO1 if invalidate 
    if(inv) {
        u32_t h, l0, l1;
        TLB_GET_ENTRY((vpage_num>>1), &h, &l0, &l1);
        entry_lo0=l0;
        entry_lo1=l1;
    }

    if(ent_lo_idx) {
        tlb_update_badvaddr_entry(new_hi, entry_lo0, new_lo.v);
    } else {
        tlb_update_badvaddr_entry(new_hi, new_lo.v, entry_lo1);
    }

}


// return non-zero: new entry_hi if it can resolved here,
// return zero: can't resolve here
SECTION_UNS_TEXT MMU_MIPS16 int
mmu_tlb_miss_check_first(u32_t badvaddr, u32_t entry_lo0, u32_t entry_lo1, u32_t vpage_num)  {
    u32_t ent_lo_idx=ENTRYLO_IDX(badvaddr);

    tlb_entry_lo_t lo = {.v=(ent_lo_idx)?entry_lo1:entry_lo0};

    if (lo.f.v==TLB_INVALID) return 0;

    // this means it has valid physical page (just asid too old)
#ifdef MMU_CHECK_INETRNAL_ERROR
    while (lo.f.pfn==0);
#endif
    u32_t entry_hi=GET_ENTRYHI(badvaddr);
    
    entry_hi=SYNC_ENTRYHI(entry_hi);
    tlb_update_badvaddr_entry(entry_hi, entry_lo0, entry_lo1);

    // lookup its pa_page
    u32_t ppage_num = lo.f.pfn&(OTTO_SRAM_MASK>>TLB_PAGE_SHIFT);
    page_t *p=_page+ppage_num;
    
    // increase the weight of this phyical page
    u32_t current_list_num=p->list_num;
    u32_t promote_list_num=PROMO_LEVEL(current_list_num); 
    if (promote_list_num!=_lowest_level) {
        move_to_list(promote_list_num, p);
    } 
    return 1;
}
