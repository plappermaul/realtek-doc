/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 71708 $
 * $Date: 2016-09-19 11:31:17 +0800 (Mon, 19 Sep 2016) $
 *
 * Purpose : Definition of OSAL test APIs in the SDK
 *
 * Feature : OSAL test APIs
 *
 */

/*
 * Include Files
 */
#include <osal/osal_test_case.h>

/* assume int as 4 bytes data */

#define VERI_ALLOC_SIZE        (1024*1024)  /* 4-byte alignment */
#define VERI_ALLOC_TIME        (256)
#define VERI_CACHE_TEST_SIZE   (256)     /* 4-byte alignment */
#define VERI_CACHE_PATTERN     (0x12483CA5)
#define VERI_UNCACHE_PATTERN   (0x5AC38421)

#define MEMORY_BARRIER()       asm volatile("":::"memory")

int32 osal_memory_cache_test(uint32 caseNo, uint32 unit)
{
    unsigned int alloc_num = 0;
    int i;
    int *buf;
    int *uncached_buf;

    RT_LOG(LOG_FUNC_ENTER, MOD_UNITTEST, "[%s] Running MEMORY test cases", __FUNCTION__);

    /* osal_alloc/free test */
    do{
        buf = osal_alloc(VERI_ALLOC_SIZE);
        if( NULL!=buf ){
            for(i=0;i<VERI_ALLOC_SIZE/sizeof(int);i++){
                REG32(buf+i) = 0xffffffff;
            }
            osal_printf("\r"); //print nothing, just invokes a function
            osal_free(buf);
        }else{
            osal_printf("[OSAL Memory Test Fail]\n");
            goto osal_memory_fail;
        }
        alloc_num++;
    }while( alloc_num<VERI_ALLOC_TIME );
    osal_printf("[OSAL Memory Test Pass]\n");

osal_memory_fail:
    /* osal_cache_memory_flush test */
    buf = osal_alloc(VERI_CACHE_TEST_SIZE);
    uncached_buf =  (int*)(((unsigned int)buf) | 0xA0000000);

    /* test address */
    if ( 0x80000000!=(((unsigned int)buf)&0x80000000) ){
        osal_printf("[%s] Addr:0x%08x Fail\n", __FUNCTION__, (unsigned int)buf);
        return RT_ERR_FAILED;
    }

    /* cached write */
    for(i=0;i<VERI_CACHE_TEST_SIZE/sizeof(int);i++){
        REG32(buf+i) = VERI_CACHE_PATTERN;
    }
    MEMORY_BARRIER();

    osal_cache_memory_flush( (unsigned int)buf, VERI_CACHE_TEST_SIZE);
    MEMORY_BARRIER();

    /* uncached read */
    for(i=0;i<VERI_CACHE_TEST_SIZE/sizeof(int);i++){
        if( VERI_CACHE_PATTERN!=REG32(uncached_buf+i) ){
            osal_printf("[%s]C Write->UC Read Fail\n", __FUNCTION__);
            return RT_ERR_FAILED;
        }
    }
    MEMORY_BARRIER();

    /* uncached write */
    for(i=0;i<VERI_CACHE_TEST_SIZE/sizeof(int);i++){
        REG32(uncached_buf+i) = VERI_UNCACHE_PATTERN;
    }
    MEMORY_BARRIER();

    /* cached read */
    for(i=0;i<VERI_CACHE_TEST_SIZE/sizeof(int);i++){
        if( VERI_UNCACHE_PATTERN!=REG32(buf+i) ){
            osal_printf("[%s][1]UC Write->C Read Fail\n", __FUNCTION__);
            return RT_ERR_FAILED;
        }
    }

    osal_cache_memory_flush((unsigned int) buf, VERI_CACHE_TEST_SIZE);
    MEMORY_BARRIER();
    /* uncached write */
    for(i=0;i<VERI_CACHE_TEST_SIZE/sizeof(int);i++){
        REG32(uncached_buf+i) = VERI_CACHE_PATTERN;
    }

    /* cached read */
    for(i=0;i<VERI_CACHE_TEST_SIZE/sizeof(int);i++){
        if( VERI_CACHE_PATTERN!=REG32(buf+i) ){
            osal_printf("[%s][2]UC Write->C Read Fail\n", __FUNCTION__);
            return RT_ERR_FAILED;
        }
    }

    osal_printf("[OSAL Cache Test Pass]\n");
    //osal_result |= OSALTEST_MEM_CACHE_OK;
    return RT_ERR_OK;
}
