#include "dram/concur_test.h"
#include "concur_gdma.h"
#include "concur_idmem.h"
#include "concur_cpu.h"
//#include "sramctl.h"
#include "dram/plr_dram_gen2_memctl.h"
//#include "memctl.h"
#define TEST_TIMES (10)

//#define MEMSET_BUF_SIZE (0x800)

#define printf(...)

#ifdef CONFIG_RTL8686
    #define GDMA0
    #define GDMA1
#endif
#define CPU
//#define IDMEM

int concur_test_for_cali(unsigned int gdma0_src_addr, unsigned int gdma0_dist_addr, \
            unsigned int gdma1_src_addr, unsigned int gdma1_dist_addr, unsigned int cpu_addr)
{
//    int gdma_status, sgdma_status,idmem_status;
    int cpu_caseNo;
    unsigned int test_times, i; //unsigned int test_times, i, polling_limit;
//    int gdma_max_case, sgdma_max_case, idmem_max_case, cpu_max_case;
    int cpu_max_case;
//    volatile unsigned int *gdma_memset_sbuf, *gdma_memset_dbuf, *sgdma_memset_sbuf, *sgdma_memset_dbuf;
//    unsigned int gdma_memset_sbuf_size, gdma_memset_dbuf_size, sgdma_memset_sbuf_size, sgdma_memset_dbuf_size;
    //unsigned int gdma_kick_value;
    //unsigned int sgdma_kick_value;
    //unsigned int idmem_kick_value;
//    unsigned int idmem_complete;
//    unsigned int gdma_complete;
//    unsigned int sgdma_complete;
    unsigned int cpu_complete;
    unsigned int cpu_write_value;
//    unsigned int gdma_base;
//    unsigned int sgdma_base;
    //unsigned int config_case;
    unsigned int cpu_target_addr;

    cpu_max_case        = 7;
//    gdma_max_case       = 3;
//    sgdma_max_case      = 3;
//    idmem_max_case      = 5;
//    gdma_base       = 0xB800a000;
//    sgdma_base      = 0xB8018000;
//    gdma_memset_sbuf    = (unsigned int *)gdma0_src_addr;
//    gdma_memset_dbuf    = (unsigned int *)gdma0_dist_addr;
//    sgdma_memset_sbuf   = (unsigned int *)gdma1_src_addr;
//    sgdma_memset_dbuf   = (unsigned int *)gdma1_dist_addr;
    cpu_target_addr     = (unsigned int)cpu_addr;
//    gdma_memset_sbuf_size   = 0x2000; 
//    gdma_memset_dbuf_size   = 0x2000;
//    sgdma_memset_sbuf_size  = 0x2000;
//    sgdma_memset_dbuf_size  = 0x2000;
    



    /* 
     * Block on polling 
     */
    /* initialize the counter */
//    idmem_complete = 0;
//    gdma_complete = 0;
//    sgdma_complete = 0;
    cpu_complete = 0;
    test_times = TEST_TIMES;    
//    gdma_status = IP_IDLE;
//    sgdma_status = IP_IDLE;
//    idmem_status = IP_IDLE;
//    gdma_caseNo  = 0;
//    sgdma_caseNo  = 0;
//    idmem_caseNo = 0;
    cpu_caseNo = 0;

    printf("Concurrent access and blocking on polling\n");
    for(i=0; i< test_times; i++){

        printf("\rcpu %05d, gdma %05d, sgdma %05d, idmem %05d ", cpu_complete, gdma_complete, sgdma_complete, idmem_complete);
        
#ifdef CPU
        cpu_write_value = CPU_setting(cpu_caseNo, cpu_target_addr);
#endif
#ifdef GDMA0
        gdma_kick_value = GDMA_setting(gdma_caseNo, gdma_base, (unsigned int)gdma_memset_sbuf, gdma_memset_sbuf_size, \
                                                (unsigned int)gdma_memset_dbuf, gdma_memset_dbuf_size);
#endif
#ifdef GDMA1
        sgdma_kick_value = GDMA_setting(sgdma_caseNo, sgdma_base, (unsigned int)sgdma_memset_sbuf, sgdma_memset_sbuf_size, \
                                                (unsigned int)sgdma_memset_dbuf, sgdma_memset_dbuf_size);
#endif
#ifdef IDMEM
        idmem_kick_value = IDMEM_setting(idmem_caseNo);
#endif
#ifdef GDMA0
        GDMA_KICKOFF(gdma_base, gdma_kick_value);
#endif
#ifdef GDMA1
        GDMA_KICKOFF(sgdma_base, sgdma_kick_value);
#endif
#ifdef IDMEM
        IDMEM_KICKOFF(idmem_kick_value);
#endif
#ifdef CPU
        CPU_KICKOFF(cpu_caseNo);
#endif

#ifdef GDMA0
        polling_limit = 0x100000;
        while(GDMA_nonb_polling(gdma_base) == IP_BUSY){
            polling_limit--;
            if(polling_limit == 0){
                printf("\nGDMA(0x%08x) polling timeout\n",gdma_base);
                return -1;
            }
        }
#endif
#ifdef GDMA1
        polling_limit = 0x100000;
        while(GDMA_nonb_polling(sgdma_base) == IP_BUSY){
            polling_limit--;
            if(polling_limit == 0){
                printf("\nGDMA(0x%08x) polling timeout\n",sgdma_base);
                return -1;
            }
        }
#endif
#ifdef IDMEM
        polling_limit = 0x100000;
        while(IDMEM_nonb_polling() == IP_BUSY){
            polling_limit--;
            if(polling_limit == 0){
                printf("\nIDMEM polling timeout\n",);
                return -1;
            }
        }
#endif
#ifdef GDMA0
        gdma_complete++;
        if( GDMA_checking(gdma_caseNo, gdma_base, (unsigned int)gdma_memset_sbuf, gdma_memset_sbuf_size, \
                                 (unsigned int)gdma_memset_dbuf, gdma_memset_dbuf_size)==CHECKING_FAIL )
            return -1;
        gdma_caseNo = (gdma_caseNo + 1) % gdma_max_case;
#endif
#ifdef GDMA1
        sgdma_complete++;
        if( GDMA_checking(sgdma_caseNo, sgdma_base, (unsigned int)sgdma_memset_sbuf, sgdma_memset_sbuf_size, \
                                 (unsigned int)sgdma_memset_dbuf, sgdma_memset_dbuf_size)==CHECKING_FAIL )
            return -1;
        sgdma_caseNo = (sgdma_caseNo + 1) % sgdma_max_case;
#endif
#ifdef IDMEM
        idmem_complete++;
        if( IDMEM_checking(idmem_caseNo) == CHECKING_FAIL )
            return -1;
        idmem_caseNo = (idmem_caseNo + 1) % idmem_max_case;
#endif
#ifdef CPU
        cpu_complete++;
        if( CPU_checking(cpu_caseNo, cpu_write_value, cpu_target_addr) == CHECKING_FAIL )
                return -1;
        cpu_caseNo = (cpu_caseNo + 1) % cpu_max_case;
#endif
    }

    printf("\n");
    



    /* 
     * No blocking on polling 
     */
    /* initialize the counter */
//    idmem_complete = 0;
//    gdma_complete = 0;
//    sgdma_complete = 0;
    cpu_complete = 0;
    test_times = TEST_TIMES;    
//    gdma_status = IP_IDLE;
//    sgdma_status = IP_IDLE;
//    idmem_status = IP_IDLE;
//    gdma_caseNo  = 0;
//    sgdma_caseNo  = 0;
//    idmem_caseNo = 0;

    printf("Concurrent access and no blocking on polling\n");
    for(i=0; i< test_times; i++){
        printf("\rcpu %05d, gdma %05d, sgdma %05d, idmem %05d ", cpu_complete, gdma_complete, sgdma_complete, idmem_complete);
#ifdef GDMA0
        if(gdma_status == IP_IDLE)
            gdma_kick_value = GDMA_setting(gdma_caseNo, gdma_base, (unsigned int)gdma_memset_sbuf, gdma_memset_sbuf_size, \
                                                (unsigned int)gdma_memset_dbuf, gdma_memset_dbuf_size);
#endif
#ifdef GDMA1
        if(sgdma_status == IP_IDLE)
            sgdma_kick_value = GDMA_setting(sgdma_caseNo, sgdma_base, (unsigned int)sgdma_memset_sbuf, sgdma_memset_sbuf_size, \
                                                (unsigned int)sgdma_memset_dbuf, sgdma_memset_dbuf_size);
#endif
#ifdef IDMEM
        if(idmem_status == IP_IDLE)
            idmem_kick_value = IDMEM_setting(idmem_caseNo);
#endif
#ifdef CPU
        cpu_write_value = CPU_setting(cpu_caseNo, cpu_target_addr);
#endif
#ifdef GDMA0
        if(gdma_status == IP_IDLE)
            GDMA_KICKOFF(gdma_base, gdma_kick_value);
#endif
#ifdef GDMA1
        if(sgdma_status == IP_IDLE)
            GDMA_KICKOFF(sgdma_base, sgdma_kick_value);
#endif
#ifdef IDMEM
        if(idmem_status == IP_IDLE)
            IDMEM_KICKOFF(idmem_kick_value);
#endif
#ifdef CPU
        CPU_KICKOFF(cpu_caseNo);
#endif
#ifdef GDMA0
        gdma_status = GDMA_nonb_polling(gdma_base);
#endif
#ifdef GDMA1
        sgdma_status = GDMA_nonb_polling(sgdma_base);
#endif
#ifdef IDMEM
        idmem_status = IDMEM_nonb_polling();
#endif
#ifdef CPU
        cpu_complete++;
        if(CPU_checking(cpu_caseNo, cpu_write_value, cpu_target_addr) == CHECKING_FAIL)
            return -1;
        cpu_caseNo = (cpu_caseNo + 1) % cpu_max_case;
#endif
#ifdef GDMA0
        if(gdma_status == IP_IDLE){
            gdma_complete++;
            if(GDMA_checking(gdma_caseNo, gdma_base, (unsigned int)gdma_memset_sbuf, gdma_memset_sbuf_size, \
                                 (unsigned int)gdma_memset_dbuf, gdma_memset_dbuf_size) == CHECKING_FAIL)
                return -1;
            gdma_caseNo = (gdma_caseNo + 1) % gdma_max_case;
        }
#endif
#ifdef GDMA1
        if(sgdma_status == IP_IDLE){
            sgdma_complete++;
            if(GDMA_checking(sgdma_caseNo, sgdma_base, (unsigned int)sgdma_memset_sbuf, sgdma_memset_sbuf_size, \
                                 (unsigned int)sgdma_memset_dbuf, sgdma_memset_dbuf_size) == CHECKING_FAIL)
                return -1;
            sgdma_caseNo = (sgdma_caseNo + 1) % sgdma_max_case;
        }
#endif
#if IDMEM
        if(idmem_status == IP_IDLE){
            idmem_complete++;
            if(IDMEM_checking(idmem_caseNo) == CHECKING_FAIL)
                return -1;
            idmem_caseNo = (idmem_caseNo + 1) % idmem_max_case;
        }
#endif
    }

    return 0;
}




