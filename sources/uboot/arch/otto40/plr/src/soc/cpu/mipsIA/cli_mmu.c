#include <soc.h>
#include <cpu/tlb.h>
#include <cli/cli_access.h>

#ifndef SECTION_CLI_UTIL
    #define SECTION_CLI_UTIL
#endif

// TLB_GET_ENTRY(idx, entry_hi, entry_lo0, entry_lo1)
// TLB_SET_ENTRY(idx, entry_hi, entry_lo0, entry_lo1)
#define VA_SFT      (13)
#define PA_SFT      (12)

SECTION_CLI_UTIL cli_cmd_ret_t
cli_mmu_set_map(const void *user, u32_t argc, const char *argv[])
{
    u32_t i, ent, va, pa;
    tlb_entry_hi_t eh;
    tlb_entry_lo_t el, el0, el1;
    if(argc<6) return CCR_INCOMPLETE_CMD;
    
    i=atoi(argv[2]);
    ent=atoi(argv[3]);
    va=atoi(argv[4]);
    pa=atoi(argv[5]);
    
    TLB_GET_ENTRY(i, &eh.v, &el0.v, &el1.v);
    
    eh.v = 0;
    eh.f.vpn2 = va>>VA_SFT;
    eh.f.asid = 0;

    el.v = 0;
    el.f.pfn = pa>>PA_SFT;
    el.f.c = 3;
    el.f.v = 1;
    el.f.d = 1; // writable
    el.f.g = 1; // ignore asid compare
    
    if (1==ent) {
        el1.v = el.v;
    } else {
        el0.v = el.v;
    }
    printf("DD: set entry %d, entry_hi 0x%x, entry_lo%d 0x%x\n", i, eh.v, ent, el.v);
    TLB_SET_ENTRY(i, eh.v, el0.v, el1.v);
    return CCR_OK;
}

SECTION_CLI_UTIL cli_cmd_ret_t
cli_mmu_set(const void *user, u32_t argc, const char *argv[])
{
    u32_t i;
    u32_t eh, el0, el1;
    if(argc<6) return CCR_INCOMPLETE_CMD;
    i=atoi(argv[2]);
    eh=atoi(argv[3]);
    el0=atoi(argv[4]);
    el1=atoi(argv[5]);
    printf("DD: set entry %d, entry_hi 0x%x, entry_lo0 0x%x, entry_lo1 0x%x\n", i, eh, el0, el1);
    TLB_SET_ENTRY(i, eh, el0, el1);
    return CCR_OK;
}

SECTION_CLI_UTIL cli_cmd_ret_t
cli_mmu_get(const void *user, u32_t argc, const char *argv[])
{
    u32_t i, s, e;
    tlb_entry_hi_t eh;
    tlb_entry_lo_t el0, el1;

    if(argc<3) { s=0; e=s+16; }
    else { s=atoi(argv[2]); e=s+1; }
    
    eh.v=el0.v=el1.v=0xBEEFBEEF;
    
    printf("II: TLB Entry Dump\n");
    for(i=s; i<e; i++) {
        TLB_GET_ENTRY(i, &eh.v, &el0.v, &el1.v);
        printf("[%02d]\n  entry_hi =0x%08x => vnp2=0x%08x, asid=%d\n", i, eh.v, eh.f.vpn2<<VA_SFT, eh.f.asid);
        printf("  entry_lo0=0x%08x => pfn =0x%08x, c=%d, d=%d, v=%d, g=%d\n", el0.v, el0.f.pfn<<PA_SFT, el0.f.c, el0.f.d, el0.f.v, el0.f.g);
        printf("  entry_lo1=0x%08x => pfn =0x%08x, c=%d, d=%d, v=%d, g=%d\n", el1.v, el1.f.pfn<<PA_SFT, el1.f.c, el1.f.d, el1.f.v, el1.f.g);
    }
    
    return CCR_OK;
}

SECTION_CLI_UTIL cli_cmd_ret_t
cli_mmu_pmap(const void *user, u32_t argc, const char *argv[])
{
    u32_t pmask;
    if('g'==argv[1][0]) {
        pmask = TLB_GET_PAGEMASK();
        printf("II: pagemask = 0x%x (0x%x)\n", ((pmask>>13)+1)*0x1000, pmask);
    } else {    // set
        u32_t _pmask = atoi(argv[2]);
        pmask=((_pmask/0x1000)-1)<<13;
        TLB_SET_PAGEMASK(pmask);
    }
    return CCR_OK;
}

cli_top_node(mmu, VZERO);
    cli_add_node(set_map, mmu, cli_mmu_set_map);
    cli_add_help(set_map, "mmu set_map <entry idx> <kuseg2 addr> <physical addr>");
    cli_add_node(set_ent, mmu, cli_mmu_set);
    cli_add_help(set_ent, "mmu set_ent <idx> <entry_hi> <entry_lo0> <entry_lo1>");
    cli_add_node(get_ent, mmu, cli_mmu_get);
    cli_add_help(get_ent, "mmu get_ent <idx>");
    cli_add_node(set_pmask, mmu, cli_mmu_pmap);    
    cli_add_help(set_pmask, "mmu set_pmask <pagemask>");
    cli_add_node(get_pmask, mmu, cli_mmu_pmap);    
    cli_add_help(get_pmask, "mmu get_pmask");

