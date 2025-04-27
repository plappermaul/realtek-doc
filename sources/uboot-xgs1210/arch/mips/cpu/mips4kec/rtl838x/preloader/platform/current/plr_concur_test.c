#include "dram/concur_test.h"
#include "concur_cpu.h"
#include "dram/plr_dram_gen2_memctl.h"
#define TEST_TIMES (10)

#define printf(...)

#define CPU

int concur_test_for_cali(unsigned int gdma0_src_addr, unsigned int gdma0_dist_addr, \
            unsigned int gdma1_src_addr, unsigned int gdma1_dist_addr, unsigned int cpu_addr)
{
#ifdef CPU
    int cpu_caseNo;
    unsigned int test_times, i;
    int cpu_max_case;
    unsigned int cpu_complete;
    unsigned int cpu_write_value;
    unsigned int cpu_target_addr;

    cpu_max_case        = 7;
    cpu_target_addr     = (unsigned int)cpu_addr;
    /* 
     * Block on polling 
     */
    /* initialize the counter */
    cpu_complete = 0;
    test_times = TEST_TIMES;    
    cpu_caseNo = 0;

    printf("CPU access test and blocking on polling\n");
    for(i=0; i< test_times; i++){

        printf("\rcpu %05d, gdma %05d, sgdma %05d, idmem %05d ", cpu_complete, gdma_complete, sgdma_complete, idmem_complete);
        
        cpu_write_value = CPU_setting(cpu_caseNo, cpu_target_addr);
        CPU_KICKOFF(cpu_caseNo);
        cpu_complete++;
        if( CPU_checking(cpu_caseNo, cpu_write_value, cpu_target_addr) == CHECKING_FAIL )
                return -1;
        cpu_caseNo = (cpu_caseNo + 1) % cpu_max_case;
    }
    printf("\n");
    
    /* 
     * No blocking on polling 
     */
    /* initialize the counter */
    cpu_complete = 0;
    test_times = TEST_TIMES;    

    printf("CPU access test and no blocking on polling\n");
    for(i=0; i< test_times; i++){
        printf("\rcpu %05d, gdma %05d, sgdma %05d, idmem %05d ", cpu_complete, gdma_complete, sgdma_complete, idmem_complete);
        cpu_write_value = CPU_setting(cpu_caseNo, cpu_target_addr);
        CPU_KICKOFF(cpu_caseNo);
        cpu_complete++;
        if(CPU_checking(cpu_caseNo, cpu_write_value, cpu_target_addr) == CHECKING_FAIL)
            return -1;
        cpu_caseNo = (cpu_caseNo + 1) % cpu_max_case;
    }
#endif //#ifdef CPU
    return 0;
}




