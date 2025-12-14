#include <osal/lib.h>
#include <common/debug/rt_log.h>
#include <dal/apollo/raw/apollo_raw_l2.h>
#include <hal/mac/drv.h>
#include <dal/raw/apollo/dal_raw_lut_test_case.h>

uint8 entry4Way[512];
uint32 calHashIdx(uint32 testArrBitSize, uint32 testBitIdx, uint32 *testBitArr);
void lutTestArrIni(void);

/* Define symbol used for test input */
int32 dal_lut_raw_entry_test(uint32 caseNo)
{
    int32 ret = RT_ERR_OK;
    uint32 entryIdx;
    uint32  test_tmp=0;
    apollo_lut_table_t testEntry;
    

    testEntry.address = 6;

    testEntry.cvid_fid =0x0;
    testEntry.l3lookup = 0;
    testEntry.ivl_svl = 1;
    testEntry.efid = 5;
    testEntry.fid = 6;
    testEntry.sapri_en = 1;
    testEntry.spa = 0x5;
    testEntry.age = 2;
    testEntry.sa_block = 1;
    testEntry.da_block = 0;
    testEntry.ext_dsl_spa =0x3;
    testEntry.arp_used = 0;
    testEntry.lut_pri = 0x4;
    testEntry.fwdpri_en = 1;
    testEntry.nosalearn = 1;
    testEntry.valid = 1;
    testEntry.method = RAW_LUT_READ_METHOD_ADDRESS;
    testEntry.table_type = RAW_LUT_ENTRY_TYPE_L2UC;
    /*error input check*/
    /*out of range*/
    osal_memset(&testEntry, 0, sizeof(apollo_lut_table_t));
    testEntry.address = APOLLO_LUT_4WAY_NO+APOLLO_LUT_CAM_NO + 1;
    if( apollo_raw_l2_lookUpTb_set(&testEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    osal_memset(&testEntry, 0, sizeof(apollo_lut_table_t));
    testEntry.l3lookup = 2;
    if( apollo_raw_l2_lookUpTb_set(&testEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    osal_memset(&testEntry, 0, sizeof(apollo_lut_table_t));
    testEntry.lut_pri = APOLLO_RAW_LUT_FWDPRIMAX + 1;
    if( apollo_raw_l2_lookUpTb_set(&testEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }    
    
    osal_memset(&testEntry, 0, sizeof(apollo_lut_table_t));
    testEntry.fwdpri_en = 2;
    if( apollo_raw_l2_lookUpTb_set(&testEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    osal_memset(&testEntry, 0, sizeof(apollo_lut_table_t));
    testEntry.nosalearn = 2;
    if( apollo_raw_l2_lookUpTb_set(&testEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    osal_memset(&testEntry, 0, sizeof(apollo_lut_table_t));
    testEntry.cvid_fid = APOLLO_VIDMAX + 1;
    if( apollo_raw_l2_lookUpTb_set(&testEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    osal_memset(&testEntry, 0, sizeof(apollo_lut_table_t));
    testEntry.fid = APOLLO_FIDMAX + 1;
    if( apollo_raw_l2_lookUpTb_set(&testEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    osal_memset(&testEntry, 0, sizeof(apollo_lut_table_t));
    testEntry.efid = APOLLO_EFIDMAX + 1;
    if( apollo_raw_l2_lookUpTb_set(&testEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    osal_memset(&testEntry, 0, sizeof(apollo_lut_table_t));
    testEntry.sapri_en = 2;
    if( apollo_raw_l2_lookUpTb_set(&testEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    osal_memset(&testEntry, 0, sizeof(apollo_lut_table_t));
    testEntry.spa = APOLLO_PORTNO + 1;
    if( apollo_raw_l2_lookUpTb_set(&testEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    osal_memset(&testEntry, 0, sizeof(apollo_lut_table_t));
    testEntry.age = APOLLO_RAW_LUT_AGEMAX + 1;
    if( apollo_raw_l2_lookUpTb_set(&testEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    osal_memset(&testEntry, 0, sizeof(apollo_lut_table_t));
    testEntry.auth = 2;
    if( apollo_raw_l2_lookUpTb_set(&testEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    osal_memset(&testEntry, 0, sizeof(apollo_lut_table_t));
    testEntry.sa_block = 2;
    if( apollo_raw_l2_lookUpTb_set(&testEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    osal_memset(&testEntry, 0, sizeof(apollo_lut_table_t));
    testEntry.da_block = 2;
    if( apollo_raw_l2_lookUpTb_set(&testEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    osal_memset(&testEntry, 0, sizeof(apollo_lut_table_t));
    testEntry.ext_dsl_spa = APOLLO_RAW_LUT_EXTDSL_SPAMAX + 1;
    if( apollo_raw_l2_lookUpTb_set(&testEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    osal_memset(&testEntry, 0, sizeof(apollo_lut_table_t));
    testEntry.arp_used = 2;
    if( apollo_raw_l2_lookUpTb_set(&testEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
    osal_memset(&testEntry, 0, sizeof(apollo_lut_table_t));
    testEntry.gip_only = 2;
    if( apollo_raw_l2_lookUpTb_set(&testEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    osal_memset(&testEntry, 0, sizeof(apollo_lut_table_t));
    testEntry.wan_sa = 2;
    if( apollo_raw_l2_lookUpTb_set(&testEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }      
    osal_memset(&testEntry, 0, sizeof(apollo_lut_table_t));
    testEntry.l3_idx = APOLLO_RAW_LUT_L3IDXAMAX + 1;
    if( apollo_raw_l2_lookUpTb_set(&testEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    osal_memset(&testEntry, 0, sizeof(apollo_lut_table_t));
    testEntry.ext_fr = 2;
    if( apollo_raw_l2_lookUpTb_set(&testEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    osal_memset(&testEntry, 0, sizeof(apollo_lut_table_t));
    testEntry.mbr = APOLLO_PORTMASK + 1;
    if( apollo_raw_l2_lookUpTb_set(&testEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    osal_memset(&testEntry, 0, sizeof(apollo_lut_table_t));
    testEntry.ext_mbr = APOLLO_RAW_LUT_EXTMBR + 1;
    if( apollo_raw_l2_lookUpTb_set(&testEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    osal_memset(&testEntry, 0, sizeof(apollo_lut_table_t));
    testEntry.dsl_mbr = APOLLO_RAW_LUT_DSLMBR + 1;
    if( apollo_raw_l2_lookUpTb_set(&testEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    /*null pointer*/
    if( apollo_raw_l2_lookUpTb_set(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( apollo_raw_l2_lookUpTb_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }    
    return RT_ERR_OK;
}

int32 dal_lut_raw_l2UcEntry_test(uint32 caseNo)
{
    int32 ret = RT_ERR_OK;
    uint32 entryIdx;
    uint32  test_tmp=0;
    uint32 i;
    apollo_lut_table_t testEntry;
    apollo_lut_table_t testRdEntry;
    rtk_table_t *pTable;
    osal_memset(&testEntry, 0, sizeof(apollo_lut_table_t));
    
    pTable = table_find(L2_UCt);
    /* NULL means the table is not supported in this chip*/
    RT_PARAM_CHK((NULL == pTable), RT_ERR_CHIP_NOT_SUPPORTED);
    for (i =0; i < 100; i++)
    {

        testEntry.mac.octet[0]=0x0 ;
        testEntry.mac.octet[1]=0x0 + (1<<((i + 1)%8)) + i;
        testEntry.mac.octet[2]=0x0 + (1<<((i + 2)%8)) + i;
        testEntry.mac.octet[3]=0x0 + (1<<((i + 3)%8)) + i;
        testEntry.mac.octet[4]=0x0 + (1<<((i + 4)%8)) + i;
        testEntry.mac.octet[5]=0x0 + (1<<((i + 5)%8)) + i;
        testEntry.cvid_fid = 1<<(i%(pTable->fields[L2_UC_CVIDtf].len));
        testEntry.l3lookup = 0;
        testEntry.ivl_svl = 1 - testEntry.ivl_svl;
        testEntry.efid = 1<<(i%(pTable->fields[L2_UC_EFIDtf].len));
        testEntry.fid = 1<<(i%(pTable->fields[L2_UC_FIDtf].len));
        testEntry.sapri_en = 1 - testEntry.sapri_en;
        testEntry.spa = 1<<(i%(pTable->fields[L2_UC_SPAtf].len));
        testEntry.age = 1;
        testEntry.auth = 1 - testEntry.auth;
        testEntry.sa_block = 1 - testEntry.sa_block;
        testEntry.da_block = 1 - testEntry.da_block;
        testEntry.ext_dsl_spa = 1<<(i%(pTable->fields[L2_UC_EXT_DSL_SPAtf].len));
        testEntry.arp_used = 1 - testEntry.arp_used;
        testEntry.lut_pri = 1<<(i%(pTable->fields[L2_UC_FWDPRItf].len));
        testEntry.fwdpri_en = 1 - testEntry.fwdpri_en;
        testEntry.nosalearn = 1;
        testEntry.valid = 1;
        testEntry.method = RAW_LUT_READ_METHOD_MAC;
        testEntry.table_type = RAW_LUT_ENTRY_TYPE_L2UC;
        if ((ret = apollo_raw_l2_lookUpTb_set(&testEntry)) != RT_ERR_OK)
        {
            osal_printf("\n %s %d err:%d\n",__FUNCTION__,__LINE__, ret);
            return ret;   
        }


    #if 0
            osal_printf("write address:%x\n", testEntry.address);
            osal_printf("mac:%x:%x:%x:%x:%x:%x\n",(int32)testEntry.mac.octet[0], (int32)testEntry.mac.octet[1],
                (int32)testEntry.mac.octet[2] , (int32)testEntry.mac.octet[3], (int32)testEntry.mac.octet[4], (int32)testEntry.mac.octet[5]);
            osal_printf("cvid_fid:%x\n", testEntry.cvid_fid);
            osal_printf("l3lookup:%x\n", testEntry.l3lookup);
            osal_printf("ivl_svl:%x\n", testEntry.ivl_svl);
            osal_printf("efid:%x\n", testEntry.efid);
            osal_printf("fid:%x\n", testEntry.fid);
            osal_printf("sapri_en:%x\n", testEntry.sapri_en);
            osal_printf("spa:%x\n", testEntry.spa);
            osal_printf("age:%x\n", testEntry.age);
            osal_printf("sa_block:%x\n", testEntry.sa_block);
            osal_printf("da_block:%x\n", testEntry.da_block);
            osal_printf("ext_dsl_spa:%x\n", testEntry.ext_dsl_spa);
            osal_printf("arp_used:%x\n", testEntry.arp_used);
            osal_printf("lut_pri:%x\n", testEntry.lut_pri);
            osal_printf("fwdpri_en:%x\n", testEntry.fwdpri_en);
            osal_printf("nosalearn:%x\n", testEntry.nosalearn);
            osal_printf("valid:%x\n", testEntry.valid);
            osal_printf("hit:%x busy:%x\n", testEntry.lookup_hit,testEntry.lookup_busy);
            osal_printf("\n");
    #endif
    }
    #if 0 

        testRdEntry.mac.octet[0]=0x0;
        testRdEntry.mac.octet[1]=0x2;
        testRdEntry.mac.octet[2]=0x3;
        testRdEntry.mac.octet[3]=0x4;
        testRdEntry.mac.octet[4]=0x5;
        testRdEntry.mac.octet[5]=0x6;
        testRdEntry.cvid_fid =0x0;
        testRdEntry.l3lookup = 0;
        testRdEntry.ivl_svl = 1;
        testRdEntry.method = RAW_LUT_READ_METHOD_ADDRESS;
        testEntry.table_type = RAW_LUT_ENTRY_TYPE_L2UC;
        testRdEntry.address = 0x404;  
        if ((ret = apollo_raw_l2_lookUpTb_get(&testRdEntry)) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__, ret);
            return ret;   
        }
        testRdEntry.method = RAW_LUT_READ_METHOD_MAC;
        if (osal_memcmp( &testEntry, &testRdEntry, sizeof(testRdEntry)))
        {
            osal_printf("%s(%u), index:%u \n", __FUNCTION__, __LINE__);
            return RT_ERR_FAILED;
        }
        
       
        osal_printf("read address:%x\n", testRdEntry.address);
        osal_printf("mac:%x:%x:%x:%x:%x:%x\n",(int32) testRdEntry.mac.octet[0], (int32)testRdEntry.mac.octet[1],
            (int32)testRdEntry.mac.octet[2] , (int32)testRdEntry.mac.octet[3], (int32)testRdEntry.mac.octet[4], (int32)testRdEntry.mac.octet[5]);
        osal_printf("cvid_fid:%x\n", testRdEntry.cvid_fid);
        osal_printf("l3lookup:%x\n", testRdEntry.l3lookup);
        osal_printf("ivl_svl:%x\n", testRdEntry.ivl_svl);
        osal_printf("efid:%x\n", testRdEntry.efid);
        osal_printf("fid:%x\n", testRdEntry.fid);
        osal_printf("sapri_en:%x\n", testRdEntry.sapri_en);
        osal_printf("spa:%x\n", testRdEntry.spa);
        osal_printf("age:%x\n", testRdEntry.age);
        osal_printf("sa_block:%x\n", testRdEntry.sa_block);
        osal_printf("da_block:%x\n", testRdEntry.da_block);
        osal_printf("ext_dsl_spa:%x\n", testRdEntry.ext_dsl_spa);
        osal_printf("arp_used:%x\n", testRdEntry.arp_used);
        osal_printf("lut_pri:%x\n", testRdEntry.lut_pri);
        osal_printf("fwdpri_en:%x\n", testRdEntry.fwdpri_en);
        osal_printf("nosalearn:%x\n", testRdEntry.nosalearn);
        osal_printf("valid:%x\n", testRdEntry.valid);
        osal_printf("hit:%x busy:%x\n", testRdEntry.lookup_hit,testRdEntry.lookup_busy);

        osal_memset(&testRdEntry, 0, sizeof(apollo_lut_table_t));
    #endif
#if 0 /*test read entry directly*/
            osal_memset(&testRdEntry, 0, sizeof(apollo_lut_table_t));
            testRdEntry.method = RAW_LUT_READ_METHOD_ADDRESS;
            testRdEntry.address = 0x4;  
            if ((ret = apollo_raw_l2_lookUpTb_get(&testRdEntry)) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__, ret);
                return ret;   
            }
            osal_printf("read address:%x\n", testRdEntry.address);
            osal_printf("mac:%x:%x:%x:%x:%x:%x\n",(int32) testRdEntry.mac.octet[0], (int32)testRdEntry.mac.octet[1],
                (int32)testRdEntry.mac.octet[2] , (int32)testRdEntry.mac.octet[3], (int32)testRdEntry.mac.octet[4], (int32)testRdEntry.mac.octet[5]);
            osal_printf("cvid_fid:%x\n", testRdEntry.cvid_fid);
            osal_printf("l3lookup:%x\n", testRdEntry.l3lookup);
            osal_printf("ivl_svl:%x\n", testRdEntry.ivl_svl);
            osal_printf("efid:%x\n", testRdEntry.efid);
            osal_printf("fid:%x\n", testRdEntry.fid);
            osal_printf("sapri_en:%x\n", testRdEntry.sapri_en);
            osal_printf("spa:%x\n", testRdEntry.spa);
            osal_printf("age:%x\n", testRdEntry.age);
            osal_printf("sa_block:%x\n", testRdEntry.sa_block);
            osal_printf("da_block:%x\n", testRdEntry.da_block);
            osal_printf("ext_dsl_spa:%x\n", testRdEntry.ext_dsl_spa);
            osal_printf("arp_used:%x\n", testRdEntry.arp_used);
            osal_printf("lut_pri:%x\n", testRdEntry.lut_pri);
            osal_printf("fwdpri_en:%x\n", testRdEntry.fwdpri_en);
            osal_printf("nosalearn:%x\n", testRdEntry.nosalearn);
            osal_printf("valid:%x\n", testRdEntry.valid);
            osal_printf("hit:%x busy:%x\n", testRdEntry.lookup_hit,testRdEntry.lookup_busy);
            osal_printf("entryType:%x\n",  testRdEntry.table_type);  
#endif


    osal_memset(&testEntry, 0, sizeof(apollo_lut_table_t));
    osal_memset(&testRdEntry, 0, sizeof(apollo_lut_table_t));    

    for (i = 0; i < 100; i++)
    {

        testEntry.mac.octet[0]=0x0 ;
        testEntry.mac.octet[1]=0x0 + (1<<((i + 1)%8)) + i;
        testEntry.mac.octet[2]=0x0 + (1<<((i + 2)%8)) + i;
        testEntry.mac.octet[3]=0x0 + (1<<((i + 3)%8)) + i;
        testEntry.mac.octet[4]=0x0 + (1<<((i + 4)%8)) + i;
        testEntry.mac.octet[5]=0x0 + (1<<((i + 5)%8)) + i;
        testEntry.cvid_fid = 1<<(i%(pTable->fields[L2_UC_CVIDtf].len));
        testEntry.l3lookup = 0;
        testEntry.ivl_svl = 1 - testEntry.ivl_svl;
        testEntry.efid = 1<<(i%(pTable->fields[L2_UC_EFIDtf].len));
        testEntry.fid = 1<<(i%(pTable->fields[L2_UC_FIDtf].len));
        testEntry.sapri_en = 1 - testEntry.sapri_en;
        testEntry.spa = 1<<(i%(pTable->fields[L2_UC_SPAtf].len));
        testEntry.age = 1;
        testEntry.auth = 1 - testEntry.auth;
        testEntry.sa_block = 1 - testEntry.sa_block;
        testEntry.da_block = 1 - testEntry.da_block;
        testEntry.ext_dsl_spa = 1<<(i%(pTable->fields[L2_UC_EXT_DSL_SPAtf].len));
        testEntry.arp_used = 1 - testEntry.arp_used;
        testEntry.lut_pri = 1<<(i%(pTable->fields[L2_UC_FWDPRItf].len));
        testEntry.fwdpri_en = 1 - testEntry.fwdpri_en;
        testEntry.nosalearn = 1;
        testEntry.valid = 1;
        testEntry.method = RAW_LUT_READ_METHOD_MAC;
        testEntry.table_type = RAW_LUT_ENTRY_TYPE_L2UC;
        testEntry.lookup_hit = 1;
        

        testRdEntry.mac.octet[0]=0x0 ;
        testRdEntry.mac.octet[1]=0x0 + (1<<((i + 1)%8)) + i;
        testRdEntry.mac.octet[2]=0x0 + (1<<((i + 2)%8)) + i;
        testRdEntry.mac.octet[3]=0x0 + (1<<((i + 3)%8)) + i;
        testRdEntry.mac.octet[4]=0x0 + (1<<((i + 4)%8)) + i;
        testRdEntry.mac.octet[5]=0x0 + (1<<((i + 5)%8)) + i;
        testRdEntry.cvid_fid = 1<<(i%(pTable->fields[L2_UC_CVIDtf].len));
        testRdEntry.l3lookup = 0;
        testRdEntry.ivl_svl = 1 - testRdEntry.ivl_svl;
        testRdEntry.method = RAW_LUT_READ_METHOD_MAC;
        testRdEntry.address = 0x0;
        testRdEntry.efid = 1<<(i%(pTable->fields[L2_UC_EFIDtf].len));
        testRdEntry.fid = 1<<(i%(pTable->fields[L2_UC_FIDtf].len));
        testRdEntry.table_type = RAW_LUT_ENTRY_TYPE_L2UC;
        if ((ret = apollo_raw_l2_lookUpTb_get(&testRdEntry)) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__, ret);
            return ret;   
        }
        testEntry.address = testRdEntry.address;
        if (osal_memcmp( &testEntry, &testRdEntry, sizeof(testRdEntry)))
        {
            osal_printf("%s(%u), index:%u \n", __FUNCTION__, __LINE__, i);
     #if 0  
                osal_printf("read address:%x\n", testRdEntry.address);
                osal_printf("mac:%x:%x:%x:%x:%x:%x\n",(int32) testRdEntry.mac.octet[0], (int32)testRdEntry.mac.octet[1],
                    (int32)testRdEntry.mac.octet[2] , (int32)testRdEntry.mac.octet[3], (int32)testRdEntry.mac.octet[4], (int32)testRdEntry.mac.octet[5]);
                osal_printf("cvid_fid:%x\n", testRdEntry.cvid_fid);
                osal_printf("l3lookup:%x\n", testRdEntry.l3lookup);
                osal_printf("ivl_svl:%x\n", testRdEntry.ivl_svl);
                osal_printf("efid:%x\n", testRdEntry.efid);
                osal_printf("fid:%x\n", testRdEntry.fid);
                osal_printf("sapri_en:%x\n", testRdEntry.sapri_en);
                osal_printf("spa:%x\n", testRdEntry.spa);
                osal_printf("age:%x\n", testRdEntry.age);
                osal_printf("sa_block:%x\n", testRdEntry.sa_block);
                osal_printf("da_block:%x\n", testRdEntry.da_block);
                osal_printf("ext_dsl_spa:%x\n", testRdEntry.ext_dsl_spa);
                osal_printf("arp_used:%x\n", testRdEntry.arp_used);
                osal_printf("lut_pri:%x\n", testRdEntry.lut_pri);
                osal_printf("fwdpri_en:%x\n", testRdEntry.fwdpri_en);
                osal_printf("nosalearn:%x\n", testRdEntry.nosalearn);
                osal_printf("valid:%x\n", testRdEntry.valid);
                osal_printf("hit:%x busy:%x\n", testRdEntry.lookup_hit,testRdEntry.lookup_busy);
                osal_printf("entryType:%x\n",  testRdEntry.table_type);

               
    #endif
            return RT_ERR_FAILED;
        }

    }
    return RT_ERR_OK;
}
int32 dal_lut_raw_l2UcEntryAll_test(uint32 caseNo)
{
    int32 ret = RT_ERR_OK;
    uint32 entryIdx;
    uint32  test_tmp=0;
    uint32 i;
    apollo_lut_table_t testEntry;
    apollo_lut_table_t testRdEntry;
    rtk_table_t *pTable;
    osal_memset(&testEntry, 0, sizeof(apollo_lut_table_t));
    
    pTable = table_find(L2_UCt);
    /* NULL means the table is not supported in this chip*/
    RT_PARAM_CHK((NULL == pTable), RT_ERR_CHIP_NOT_SUPPORTED);

    for (i =0; i < APOLLO_LUT_4WAY_NO/4; i++)
    {

        testEntry.mac.octet[0] = 0x0;
        testEntry.mac.octet[1] = 0x2f;
        testEntry.mac.octet[2] = 0xda;
        testEntry.mac.octet[3] = 0x92;
        testEntry.mac.octet[4] = 0xba |(i>>8);
        testEntry.mac.octet[5] = i % 256;
        testEntry.cvid_fid = 0;
        testEntry.l3lookup = 0;
        testEntry.ivl_svl = 1;
        testEntry.efid = 0;
        testEntry.fid = 1<<(i%(pTable->fields[L2_UC_FIDtf].len));
        testEntry.sapri_en = 1 - testEntry.sapri_en;
        testEntry.spa = 1<<(i%(pTable->fields[L2_UC_SPAtf].len));
        testEntry.age = 1;
        testEntry.auth = 1 - testEntry.auth;
        testEntry.sa_block = 1 - testEntry.sa_block;
        testEntry.da_block = 1 - testEntry.da_block;
        testEntry.ext_dsl_spa = 1<<(i%(pTable->fields[L2_UC_EXT_DSL_SPAtf].len));
        testEntry.arp_used =  1 - testEntry.arp_used;
        testEntry.lut_pri = 1<<(i%(pTable->fields[L2_UC_FWDPRItf].len));
        testEntry.fwdpri_en =  1 - testEntry.fwdpri_en;
        testEntry.nosalearn = 1;
        testEntry.valid = 1;
        testEntry.method = RAW_LUT_READ_METHOD_MAC;
        testEntry.table_type = RAW_LUT_ENTRY_TYPE_L2UC;
        if ((ret = apollo_raw_l2_lookUpTb_set(&testEntry)) != RT_ERR_OK)
        {
            osal_printf("\n %s %d err:%d\n",__FUNCTION__,__LINE__, ret);
            return ret;   
        }        

    #if 0
            osal_printf("write address:%x\n", testEntry.address);
            osal_printf("mac:%x:%x:%x:%x:%x:%x\n",(int32)testEntry.mac.octet[0], (int32)testEntry.mac.octet[1],
                (int32)testEntry.mac.octet[2] , (int32)testEntry.mac.octet[3], (int32)testEntry.mac.octet[4], (int32)testEntry.mac.octet[5]);
            osal_printf("cvid_fid:%x\n", testEntry.cvid_fid);
            osal_printf("l3lookup:%x\n", testEntry.l3lookup);
            osal_printf("ivl_svl:%x\n", testEntry.ivl_svl);
            osal_printf("efid:%x\n", testEntry.efid);
            osal_printf("fid:%x\n", testEntry.fid);
            osal_printf("sapri_en:%x\n", testEntry.sapri_en);
            osal_printf("spa:%x\n", testEntry.spa);
            osal_printf("age:%x\n", testEntry.age);
            osal_printf("sa_block:%x\n", testEntry.sa_block);
            osal_printf("da_block:%x\n", testEntry.da_block);
            osal_printf("ext_dsl_spa:%x\n", testEntry.ext_dsl_spa);
            osal_printf("arp_used:%x\n", testEntry.arp_used);
            osal_printf("lut_pri:%x\n", testEntry.lut_pri);
            osal_printf("fwdpri_en:%x\n", testEntry.fwdpri_en);
            osal_printf("nosalearn:%x\n", testEntry.nosalearn);
            osal_printf("valid:%x\n", testEntry.valid);
            osal_printf("hit:%x busy:%x\n", testEntry.lookup_hit,testEntry.lookup_busy);
            osal_printf("\n");
    #endif
    }
    #if 0 

        testRdEntry.mac.octet[0]=0x0;
        testRdEntry.mac.octet[1]=0x2;
        testRdEntry.mac.octet[2]=0x3;
        testRdEntry.mac.octet[3]=0x4;
        testRdEntry.mac.octet[4]=0x5;
        testRdEntry.mac.octet[5]=0x6;
        testRdEntry.cvid_fid =0x0;
        testRdEntry.l3lookup = 0;
        testRdEntry.ivl_svl = 1;
        testRdEntry.method = RAW_LUT_READ_METHOD_ADDRESS;
        testEntry.table_type = RAW_LUT_ENTRY_TYPE_L2UC;
        testRdEntry.address = 0x404;  
        if ((ret = apollo_raw_l2_lookUpTb_get(&testRdEntry)) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__, ret);
            return ret;   
        }
        testRdEntry.method = RAW_LUT_READ_METHOD_MAC;
        if (osal_memcmp( &testEntry, &testRdEntry, sizeof(testRdEntry)))
        {
            osal_printf("%s(%u), index:%u \n", __FUNCTION__, __LINE__);
            return RT_ERR_FAILED;
        }
        
       
        osal_printf("read address:%x\n", testRdEntry.address);
        osal_printf("mac:%x:%x:%x:%x:%x:%x\n",(int32) testRdEntry.mac.octet[0], (int32)testRdEntry.mac.octet[1],
            (int32)testRdEntry.mac.octet[2] , (int32)testRdEntry.mac.octet[3], (int32)testRdEntry.mac.octet[4], (int32)testRdEntry.mac.octet[5]);
        osal_printf("cvid_fid:%x\n", testRdEntry.cvid_fid);
        osal_printf("l3lookup:%x\n", testRdEntry.l3lookup);
        osal_printf("ivl_svl:%x\n", testRdEntry.ivl_svl);
        osal_printf("efid:%x\n", testRdEntry.efid);
        osal_printf("fid:%x\n", testRdEntry.fid);
        osal_printf("sapri_en:%x\n", testRdEntry.sapri_en);
        osal_printf("spa:%x\n", testRdEntry.spa);
        osal_printf("age:%x\n", testRdEntry.age);
        osal_printf("sa_block:%x\n", testRdEntry.sa_block);
        osal_printf("da_block:%x\n", testRdEntry.da_block);
        osal_printf("ext_dsl_spa:%x\n", testRdEntry.ext_dsl_spa);
        osal_printf("arp_used:%x\n", testRdEntry.arp_used);
        osal_printf("lut_pri:%x\n", testRdEntry.lut_pri);
        osal_printf("fwdpri_en:%x\n", testRdEntry.fwdpri_en);
        osal_printf("nosalearn:%x\n", testRdEntry.nosalearn);
        osal_printf("valid:%x\n", testRdEntry.valid);
        osal_printf("hit:%x busy:%x\n", testRdEntry.lookup_hit,testRdEntry.lookup_busy);

        osal_memset(&testRdEntry, 0, sizeof(apollo_lut_table_t));
    #endif
#if 0 /*test read entry directly*/
            osal_memset(&testRdEntry, 0, sizeof(apollo_lut_table_t));
            testRdEntry.method = RAW_LUT_READ_METHOD_ADDRESS;
            testRdEntry.address = 0x4;  
            if ((ret = apollo_raw_l2_lookUpTb_get(&testRdEntry)) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__, ret);
                return ret;   
            }
            osal_printf("read address:%x\n", testRdEntry.address);
            osal_printf("mac:%x:%x:%x:%x:%x:%x\n",(int32) testRdEntry.mac.octet[0], (int32)testRdEntry.mac.octet[1],
                (int32)testRdEntry.mac.octet[2] , (int32)testRdEntry.mac.octet[3], (int32)testRdEntry.mac.octet[4], (int32)testRdEntry.mac.octet[5]);
            osal_printf("cvid_fid:%x\n", testRdEntry.cvid_fid);
            osal_printf("l3lookup:%x\n", testRdEntry.l3lookup);
            osal_printf("ivl_svl:%x\n", testRdEntry.ivl_svl);
            osal_printf("efid:%x\n", testRdEntry.efid);
            osal_printf("fid:%x\n", testRdEntry.fid);
            osal_printf("sapri_en:%x\n", testRdEntry.sapri_en);
            osal_printf("spa:%x\n", testRdEntry.spa);
            osal_printf("age:%x\n", testRdEntry.age);
            osal_printf("sa_block:%x\n", testRdEntry.sa_block);
            osal_printf("da_block:%x\n", testRdEntry.da_block);
            osal_printf("ext_dsl_spa:%x\n", testRdEntry.ext_dsl_spa);
            osal_printf("arp_used:%x\n", testRdEntry.arp_used);
            osal_printf("lut_pri:%x\n", testRdEntry.lut_pri);
            osal_printf("fwdpri_en:%x\n", testRdEntry.fwdpri_en);
            osal_printf("nosalearn:%x\n", testRdEntry.nosalearn);
            osal_printf("valid:%x\n", testRdEntry.valid);
            osal_printf("hit:%x busy:%x\n", testRdEntry.lookup_hit,testRdEntry.lookup_busy);
            osal_printf("entryType:%x\n",  testRdEntry.table_type);  
#endif


    osal_memset(&testEntry, 0, sizeof(apollo_lut_table_t));
    osal_memset(&testRdEntry, 0, sizeof(apollo_lut_table_t));    

    for (i = 0; i < APOLLO_LUT_4WAY_NO/4; i++)
    {

        testEntry.mac.octet[0]=0x0 ;
        testEntry.mac.octet[1]=0x2f;
        testEntry.mac.octet[2]=0xda;
        testEntry.mac.octet[3]=0x92;
        testEntry.mac.octet[4] = 0xba |(i>>8);
        testEntry.mac.octet[5] = i % 256;
        testEntry.cvid_fid = 0;
        testEntry.l3lookup = 0;
        testEntry.ivl_svl = 1;
        testEntry.efid = 0;
        testEntry.fid = 1<<(i%(pTable->fields[L2_UC_FIDtf].len));
        testEntry.sapri_en = 1 - testEntry.sapri_en;
        testEntry.spa = 1<<(i%(pTable->fields[L2_UC_SPAtf].len));
        testEntry.age = 1;
        testEntry.auth = 1 - testEntry.auth;
        testEntry.sa_block = 1 - testEntry.sa_block;
        testEntry.da_block = 1 - testEntry.da_block;
        testEntry.ext_dsl_spa = 1<<(i%(pTable->fields[L2_UC_EXT_DSL_SPAtf].len));
        testEntry.arp_used = 1 - testEntry.arp_used;
        testEntry.lut_pri = 1<<(i%(pTable->fields[L2_UC_FWDPRItf].len));
        testEntry.fwdpri_en =  1 - testEntry.fwdpri_en;
        testEntry.nosalearn = 1;
        testEntry.valid = 1;
        testEntry.method = RAW_LUT_READ_METHOD_MAC;
        testEntry.table_type = RAW_LUT_ENTRY_TYPE_L2UC;
        testEntry.lookup_hit = 1;
        

        testRdEntry.mac.octet[0]=0x0 ;
        testRdEntry.mac.octet[1]=0x2f;
        testRdEntry.mac.octet[2]=0xda;
        testRdEntry.mac.octet[3]=0x92;
        testRdEntry.mac.octet[4]=0xba |(i>>8);
        testRdEntry.mac.octet[5]= i % 256;
        testRdEntry.cvid_fid = 0;
        testRdEntry.l3lookup = 0;
        testRdEntry.ivl_svl = 1;
        testRdEntry.method = RAW_LUT_READ_METHOD_MAC;
        testRdEntry.address = 0x0;
        testRdEntry.efid = 0;
        testRdEntry.fid = 1<<(i%(pTable->fields[L2_UC_FIDtf].len));
        testRdEntry.table_type = RAW_LUT_ENTRY_TYPE_L2UC;
        if ((ret = apollo_raw_l2_lookUpTb_get(&testRdEntry)) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__, ret);
            return ret;   
        }
        testEntry.address = testRdEntry.address;
        if (osal_memcmp( &testEntry, &testRdEntry, sizeof(testRdEntry)))
        {
            osal_printf("%s(%u), index:%u \n", __FUNCTION__, __LINE__, i);
     #if 0   
                osal_printf("read address:%x\n", testRdEntry.address);
                osal_printf("mac:%x:%x:%x:%x:%x:%x\n",(int32) testRdEntry.mac.octet[0], (int32)testRdEntry.mac.octet[1],
                    (int32)testRdEntry.mac.octet[2] , (int32)testRdEntry.mac.octet[3], (int32)testRdEntry.mac.octet[4], (int32)testRdEntry.mac.octet[5]);
                osal_printf("cvid_fid:%x\n", testRdEntry.cvid_fid);
                osal_printf("l3lookup:%x\n", testRdEntry.l3lookup);
                osal_printf("ivl_svl:%x\n", testRdEntry.ivl_svl);
                osal_printf("efid:%x\n", testRdEntry.efid);
                osal_printf("fid:%x\n", testRdEntry.fid);
                osal_printf("sapri_en:%x\n", testRdEntry.sapri_en);
                osal_printf("spa:%x\n", testRdEntry.spa);
                osal_printf("age:%x\n", testRdEntry.age);
                osal_printf("sa_block:%x\n", testRdEntry.sa_block);
                osal_printf("da_block:%x\n", testRdEntry.da_block);
                osal_printf("ext_dsl_spa:%x\n", testRdEntry.ext_dsl_spa);
                osal_printf("arp_used:%x\n", testRdEntry.arp_used);
                osal_printf("lut_pri:%x\n", testRdEntry.lut_pri);
                osal_printf("fwdpri_en:%x\n", testRdEntry.fwdpri_en);
                osal_printf("nosalearn:%x\n", testRdEntry.nosalearn);
                osal_printf("valid:%x\n", testRdEntry.valid);
                osal_printf("hit:%x busy:%x\n", testRdEntry.lookup_hit,testRdEntry.lookup_busy);
                osal_printf("entryType:%x\n",  testRdEntry.table_type);
    #endif
            return RT_ERR_FAILED;
        }

    }
    return RT_ERR_OK;
}
void lutTestArrIni(void)
{
    osal_memset(entry4Way, 0, sizeof(entry4Way));
}
uint32 calHashIdx(uint32 testArrBitSize, uint32 testBitIdx, uint32 *testBitArr)
{
    uint32 bitIdx;
    uint32 scanIdx;
    bitIdx = testBitIdx % testArrBitSize;
    for (scanIdx = 0; scanIdx < 512; scanIdx++)
    {
        if((1<<bitIdx)&scanIdx && entry4Way[scanIdx] == 0)
        {
            *testBitArr = scanIdx - (1<<bitIdx);
            entry4Way[scanIdx] = 1;
            return scanIdx;
        }
    }
    return 0xFFFFFFFF;       

}
int32 dal_lut_raw_writeHashEntry_test(uint32 caseNo)
{
    int32 ret = RT_ERR_OK;
    uint32 entryIdx;
    uint32 testBitIdx;
    uint32 testArrBitSize = 9;
    uint32 testBitArr;
    uint64 bitPattern;
    uint64 bitOffset;
    apollo_lut_table_t testEntry;
    rtk_table_t *pTable;

    lutTestArrIni();
 
    /*l2Uc Ivl */
    
    pTable = table_find(L2_UCt);
    /* NULL means the table is not supported in this chip*/
    RT_PARAM_CHK((NULL == pTable), RT_ERR_CHIP_NOT_SUPPORTED);
    osal_memset(&testEntry, 0, sizeof(apollo_lut_table_t));
    apollo_raw_l2_ipmcHashType_set(RAW_LUT_IPMCHASH_TYPE_DMACFID);
    for (testBitIdx =0; testBitIdx < 63; testBitIdx++)
    {
         
        entryIdx = calHashIdx(testArrBitSize, testBitIdx, &testBitArr);
        if (entryIdx == 0xFFFFFFFF)
        {
            osal_printf("\n %s %d \n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED; 
        }
        bitOffset = 1;
        bitPattern = testBitArr | bitOffset<<testBitIdx;    
     #if 0   
        osal_printf("testBitIdx:%d,entryIdx:%x,testBitArr:%x,bitPattern:%lx\n",testBitIdx, entryIdx, testBitArr,bitPattern);
     #endif
        testEntry.mac.octet[0] = (uint8)((bitPattern>>40)&0xFF);
        testEntry.mac.octet[1] = (uint8)((bitPattern>>32)&0xFF);
        testEntry.mac.octet[2] = (uint8)((bitPattern>>24)&0xFF);
        testEntry.mac.octet[3] = (uint8)((bitPattern>>16)&0xFF);
        testEntry.mac.octet[4] = (uint8)((bitPattern>>8)&0xFF);
        testEntry.mac.octet[5] = (uint8)(bitPattern&0xFF);
        testEntry.cvid_fid = (uint32)((bitPattern>>48)&APOLLO_VIDMAX);
        testEntry.efid = (uint32)((bitPattern>>60)&APOLLO_EFIDMAX);
        
        testEntry.l3lookup = 0;
        testEntry.ivl_svl = 1;
        testEntry.fid = 1<<(testBitIdx%(pTable->fields[L2_UC_FIDtf].len));
        testEntry.sapri_en = 1 - testEntry.sapri_en;
        testEntry.spa = 1<<(testBitIdx%(pTable->fields[L2_UC_SPAtf].len));
        testEntry.age = 1<<(testBitIdx%(pTable->fields[L2_UC_AGEtf].len));
        testEntry.auth = 1 - testEntry.auth;
        testEntry.sa_block = 1 - testEntry.sa_block;
        testEntry.da_block = 1 - testEntry.da_block;
        testEntry.ext_dsl_spa = 1<<(testBitIdx%(pTable->fields[L2_UC_EXT_DSL_SPAtf].len));
        testEntry.arp_used =  1 - testEntry.arp_used;
        testEntry.lut_pri = 1<<(testBitIdx%(pTable->fields[L2_UC_FWDPRItf].len));
        testEntry.fwdpri_en =  1 - testEntry.fwdpri_en;
        testEntry.nosalearn = 1;
        testEntry.valid = 1;
        testEntry.method = RAW_LUT_READ_METHOD_MAC;
        testEntry.table_type = RAW_LUT_ENTRY_TYPE_L2UC;
        if ((ret = apollo_raw_l2_lookUpTb_set(&testEntry)) != RT_ERR_OK)
        {
            osal_printf("\n %s %d err:%d\n",__FUNCTION__,__LINE__, ret);
            return ret;   
        }
        
        if ((testEntry.address)>>2 != entryIdx)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__, ret);
         #if 0   
            osal_printf("write 4way-address:%x ,addr:%d\n", (testEntry.address)>>2, testEntry.address);
         #endif 
            return RT_ERR_FAILED;
        }
   
    }

   /*l2Uc Svl*/
   osal_memset(&testEntry, 0, sizeof(apollo_lut_table_t));
   
   for (testBitIdx =0; testBitIdx < 63; testBitIdx++)
   {

       if (testBitIdx < 60 && testBitIdx > 51)
           continue;
       entryIdx = calHashIdx(testArrBitSize, testBitIdx, &testBitArr);
       if (entryIdx == 0xFFFFFFFF)
       {
           osal_printf("\n %s %d \n",__FUNCTION__,__LINE__);
           return RT_ERR_FAILED; 
       }
       bitOffset = 1;
       bitPattern = testBitArr | bitOffset<<testBitIdx;    
 #if 0   
       osal_printf("testBitIdx:%d,entryIdx:%x,testBitArr:%x,bitPattern:%lx\n",testBitIdx, entryIdx, testBitArr,bitPattern);
 #endif
       testEntry.mac.octet[0] = (uint8)((bitPattern>>40)&0xFF);
       testEntry.mac.octet[1] = (uint8)((bitPattern>>32)&0xFF);
       testEntry.mac.octet[2] = (uint8)((bitPattern>>24)&0xFF);
       testEntry.mac.octet[3] = (uint8)((bitPattern>>16)&0xFF);
       testEntry.mac.octet[4] = (uint8)((bitPattern>>8)&0xFF);
       testEntry.mac.octet[5] = (uint8)(bitPattern&0xFF);
       testEntry.fid = (uint32)((bitPattern>>48)&APOLLO_FIDMAX);
       testEntry.efid = (uint32)((bitPattern>>60)&APOLLO_EFIDMAX);
       
       testEntry.l3lookup = 0;
       testEntry.ivl_svl = 0;
       testEntry.cvid_fid = 1<<(testBitIdx%(pTable->fields[L2_UC_CVIDtf].len));
       testEntry.sapri_en = 1 - testEntry.sapri_en;
       testEntry.spa = 1<<(testBitIdx%(pTable->fields[L2_UC_SPAtf].len));
       testEntry.age = 1<<(testBitIdx%(pTable->fields[L2_UC_AGEtf].len));
       testEntry.auth = 1 - testEntry.auth;
       testEntry.sa_block = 1 - testEntry.sa_block;
       testEntry.da_block = 1 - testEntry.da_block;
       testEntry.ext_dsl_spa = 1<<(testBitIdx%(pTable->fields[L2_UC_EXT_DSL_SPAtf].len));
       testEntry.arp_used =  1 - testEntry.arp_used;
       testEntry.lut_pri = 1<<(testBitIdx%(pTable->fields[L2_UC_FWDPRItf].len));
       testEntry.fwdpri_en =  1 - testEntry.fwdpri_en;
       testEntry.nosalearn = 1;
       testEntry.valid = 1;
       testEntry.method = RAW_LUT_READ_METHOD_MAC;
       testEntry.table_type = RAW_LUT_ENTRY_TYPE_L2UC;
       if ((ret = apollo_raw_l2_lookUpTb_set(&testEntry)) != RT_ERR_OK)
       {
           osal_printf("\n %s %d err:%d\n",__FUNCTION__,__LINE__, ret);
           return ret;   
       }
       
       if ((testEntry.address)>>2 != entryIdx)
       {
           osal_printf("\n %s %d\n",__FUNCTION__,__LINE__, ret);
     #if 0   
           osal_printf("write 4way-address:%x ,addr:%d\n", (testEntry.address)>>2, testEntry.address);
     #endif 
           return RT_ERR_FAILED;
       }
   
    }
#if 0

    /*l2Mc Ivl*/
    pTable = table_find(L2_MC_DSLt);
    /* NULL means the table is not supported in this chip*/
    RT_PARAM_CHK((NULL == pTable), RT_ERR_CHIP_NOT_SUPPORTED);
    osal_memset(&testEntry, 0, sizeof(apollo_lut_table_t));
   
    for (testBitIdx =0; testBitIdx < 60; testBitIdx++)
    {
                
       entryIdx = calHashIdx(testArrBitSize, testBitIdx, &testBitArr);
       if (entryIdx == 0xFFFFFFFF)
       {
           osal_printf("\n %s %d \n",__FUNCTION__,__LINE__);
           return RT_ERR_FAILED; 
       }
       bitOffset = 1;
       bitPattern = testBitArr | bitOffset<<testBitIdx;    
 #if 0   
       osal_printf("testBitIdx:%d,entryIdx:%x,testBitArr:%x,bitPattern:%lx\n",testBitIdx, entryIdx, testBitArr,bitPattern);
 #endif
       testEntry.mac.octet[0] = (uint8)((bitPattern>>40)&0xFF);
       testEntry.mac.octet[1] = (uint8)((bitPattern>>32)&0xFF);
       testEntry.mac.octet[2] = (uint8)((bitPattern>>24)&0xFF);
       testEntry.mac.octet[3] = (uint8)((bitPattern>>16)&0xFF);
       testEntry.mac.octet[4] = (uint8)((bitPattern>>8)&0xFF);
       testEntry.mac.octet[5] = (uint8)(bitPattern&0xFF);
       testEntry.cvid_fid = (uint32)((bitPattern>>48)&APOLLO_VIDMAX);
      
       testEntry.l3lookup = 0;
       testEntry.ivl_svl = 1;
       testEntry.mbr = 1<<(testBitIdx%(pTable->fields[L2_MC_DSL_MBRtf].len));
       testEntry.ext_mbr =  1<<(testBitIdx%(pTable->fields[L2_MC_DSL_EXT_MBRtf].len));
       testEntry.dsl_mbr =  1<<(testBitIdx%(pTable->fields[L2_MC_DSL_DSL_MBRtf].len));
       testEntry.lut_pri = 1<<(testBitIdx%(pTable->fields[L2_MC_DSL_FWDPRItf].len));
       testEntry.fwdpri_en = 1 - testEntry.fwdpri_en;
       testEntry.nosalearn = 1;
       testEntry.valid = 1;
       testEntry.method = RAW_LUT_READ_METHOD_MAC;
       testEntry.table_type = RAW_LUT_ENTRY_TYPE_L2MC_DSL;
       if ((ret = apollo_raw_l2_lookUpTb_set(&testEntry)) != RT_ERR_OK)
       {
           osal_printf("\n %s %d err:%d\n",__FUNCTION__,__LINE__, ret);
           return ret;   
       }
       
       if ((testEntry.address)>>2 != entryIdx)
       {
           osal_printf("\n %s %d\n",__FUNCTION__,__LINE__, ret);
     #if 1   
           osal_printf("write 4way-address:%x ,addr:%d\n", (testEntry.address)>>2, testEntry.address);
     #endif 
           return RT_ERR_FAILED;
       }
   
    }


    /*l2Mc Svl*/
    osal_memset(&testEntry, 0, sizeof(apollo_lut_table_t));
     
    for (testBitIdx =0; testBitIdx < 52; testBitIdx++)
    {
                  
       entryIdx = calHashIdx(testArrBitSize, testBitIdx, &testBitArr);
       if (entryIdx == 0xFFFFFFFF)
       {
           osal_printf("\n %s %d \n",__FUNCTION__,__LINE__);
           return RT_ERR_FAILED; 
       }
       bitOffset = 1;
       bitPattern = testBitArr | bitOffset<<testBitIdx;    
#if 1   
       osal_printf("testBitIdx:%d,entryIdx:%x,testBitArr:%x,bitPattern:%lx\n",testBitIdx, entryIdx, testBitArr,bitPattern);
#endif
       testEntry.mac.octet[0] = (uint8)((bitPattern>>40)&0xFF);
       testEntry.mac.octet[1] = (uint8)((bitPattern>>32)&0xFF);
       testEntry.mac.octet[2] = (uint8)((bitPattern>>24)&0xFF);
       testEntry.mac.octet[3] = (uint8)((bitPattern>>16)&0xFF);
       testEntry.mac.octet[4] = (uint8)((bitPattern>>8)&0xFF);
       testEntry.mac.octet[5] = (uint8)(bitPattern&0xFF);
       testEntry.cvid_fid = (uint32)((bitPattern>>48)&APOLLO_FIDMAX);
        
       testEntry.l3lookup = 0;
       testEntry.ivl_svl = 0;
       testEntry.mbr = 1<<(testBitIdx%(pTable->fields[L2_MC_DSL_MBRtf].len));
       testEntry.ext_mbr =  1<<(testBitIdx%(pTable->fields[L2_MC_DSL_EXT_MBRtf].len));
       testEntry.dsl_mbr =  1<<(testBitIdx%(pTable->fields[L2_MC_DSL_DSL_MBRtf].len));
       testEntry.lut_pri = 1<<(testBitIdx%(pTable->fields[L2_MC_DSL_FWDPRItf].len));
       testEntry.fwdpri_en = 1 - testEntry.fwdpri_en;
       testEntry.nosalearn = 1;
       testEntry.valid = 1;
       testEntry.method = RAW_LUT_READ_METHOD_MAC;
       testEntry.table_type = RAW_LUT_ENTRY_TYPE_L2MC_DSL;
       if ((ret = apollo_raw_l2_lookUpTb_set(&testEntry)) != RT_ERR_OK)
       {
           osal_printf("\n %s %d err:%d\n",__FUNCTION__,__LINE__, ret);
           return ret;   
       }
         
       if ((testEntry.address)>>2 != entryIdx)
       {
           osal_printf("\n %s %d\n",__FUNCTION__,__LINE__, ret);
    #if 0   
           osal_printf("write 4way-address:%x ,addr:%d\n", (testEntry.address)>>2, testEntry.address);
    #endif 
           return RT_ERR_FAILED;
       }
     
    }
#endif         
#if 1
    /*l3Mc*/
    /*GIP_VID test*/
    apollo_raw_l2_ipmcHashType_set(RAW_LUT_IPMCHASH_TYPE_GIPVID);
    pTable = table_find(L3_MC_DSLt);
    /* NULL means the table is not supported in this chip*/
    RT_PARAM_CHK((NULL == pTable), RT_ERR_CHIP_NOT_SUPPORTED);
    osal_memset(&testEntry, 0, sizeof(apollo_lut_table_t));
   
    for (testBitIdx =0; testBitIdx < 40; testBitIdx++)
    {
       entryIdx = calHashIdx(testArrBitSize, testBitIdx, &testBitArr);
       if (entryIdx == 0xFFFFFFFF)
       {
           osal_printf("\n %s %d \n",__FUNCTION__,__LINE__);
           return RT_ERR_FAILED; 
       }
       bitOffset = 1;
       bitPattern = testBitArr | bitOffset<<testBitIdx;    
 #if 0   
       osal_printf("testBitIdx:%d,entryIdx:%x,testBitArr:%x,bitPattern:%lx\n",testBitIdx, entryIdx, testBitArr,bitPattern);
 #endif
       testEntry.gip = (uint32)(bitPattern&0xfffffff);
       testEntry.sip_vid = (uint32)((bitPattern>>28)&0xffff);
       testEntry.l3lookup = 1;
       testEntry.gip_only = 0;// 1 - testEntry.gip_only;
       testEntry.mbr = 1<<(testBitIdx%(pTable->fields[L3_MC_DSL_MBRtf].len));
       testEntry.ext_mbr =  1<<(testBitIdx%(pTable->fields[L3_MC_DSL_EXT_MBRtf].len));
       testEntry.dsl_mbr =  1<<(testBitIdx%(pTable->fields[L3_MC_DSL_DSL_MBRtf].len));
       testEntry.lut_pri = 1<<(testBitIdx%(pTable->fields[L3_MC_DSL_FWDPRItf].len));
       testEntry.fwdpri_en = 1 - testEntry.fwdpri_en;
       testEntry.nosalearn = 1;
       testEntry.valid = 1;
       testEntry.method = RAW_LUT_READ_METHOD_MAC;
       testEntry.table_type = RAW_LUT_ENTRY_TYPE_L3MC_DSL;
        
       if ((ret = apollo_raw_l2_lookUpTb_set(&testEntry)) != RT_ERR_OK)
       {
           osal_printf("\n %s %d err:%d\n",__FUNCTION__,__LINE__, ret);
           return ret;   
       }
       
       if ((testEntry.address)>>2 != entryIdx)
       {
           osal_printf("\n %s %d\n",__FUNCTION__,__LINE__, ret);
     #if 0   
           osal_printf("write 4way-address:%x ,addr:%d\n", (testEntry.address)>>2, testEntry.address);
     #endif 
           return RT_ERR_FAILED;
       }
   
    }
#endif     
   return RT_ERR_OK;
}


int32 dal_lut_raw_l2McDslEntry_test(uint32 caseNo)
{
    int32 ret = RT_ERR_OK;
    uint32 entryIdx;
    uint32  test_tmp=0;
    uint32 i;
    apollo_lut_table_t testEntry;
    apollo_lut_table_t testRdEntry;
    rtk_table_t *pTable;
    
	osal_memset(&testEntry, 0, sizeof(apollo_lut_table_t));
    pTable = table_find(L2_MC_DSLt);
    /* NULL means the table is not supported in this chip*/
    RT_PARAM_CHK((NULL == pTable), RT_ERR_CHIP_NOT_SUPPORTED);

    for (i =0; i < 100; i++)
    {

        testEntry.mac.octet[0]=0x1|((1<<((i + 1)%8 ))+ i );
        testEntry.mac.octet[1]=0x0 + (1<<((i + 1)%8)) + i;
        testEntry.mac.octet[2]=0x0 + (1<<((i + 2)%8)) + i;
        testEntry.mac.octet[3]=0x0 + (1<<((i + 3)%8)) + i;
        testEntry.mac.octet[4]=0x0 + (1<<((i + 4)%8)) + i;
        testEntry.mac.octet[5]=0x0 + (1<<((i + 5)%8)) + i;
        testEntry.cvid_fid = 1<<(i%(pTable->fields[L2_MC_DSL_VID_FIDtf].len));
        testEntry.l3lookup = 0;
        testEntry.ivl_svl = 1 - testEntry.ivl_svl;
        testEntry.mbr = 1<<(i%(pTable->fields[L2_MC_DSL_MBRtf].len));
        testEntry.ext_mbr =  1<<(i%(pTable->fields[L2_MC_DSL_EXT_MBRtf].len));
        testEntry.dsl_mbr =  1<<(i%(pTable->fields[L2_MC_DSL_DSL_MBRtf].len));
        testEntry.lut_pri = 1<<(i%(pTable->fields[L2_MC_DSL_FWDPRItf].len));
        testEntry.fwdpri_en = 1 - testEntry.fwdpri_en;
        testEntry.nosalearn = 1;
        testEntry.valid = 1;
        testEntry.method = RAW_LUT_READ_METHOD_MAC;
        testEntry.table_type = RAW_LUT_ENTRY_TYPE_L2MC_DSL;
        if ((ret = apollo_raw_l2_lookUpTb_set(&testEntry)) != RT_ERR_OK)
        {
            osal_printf("\n %s %d err:%d\n",__FUNCTION__,__LINE__, ret);
            return ret;   
        }
#if 0
                osal_printf("write address:%x\n", testEntry.address);
                osal_printf("mac:%x:%x:%x:%x:%x:%x\n",(int32)testEntry.mac.octet[0], (int32)testEntry.mac.octet[1],
                    (int32)testEntry.mac.octet[2] , (int32)testEntry.mac.octet[3], (int32)testEntry.mac.octet[4], (int32)testEntry.mac.octet[5]);
                osal_printf("cvid_fid:%x\n", testEntry.cvid_fid);
                osal_printf("l3lookup:%x\n", testEntry.l3lookup);
                osal_printf("ivl_svl:%x\n", testEntry.ivl_svl);
                osal_printf("mbr:%x\n", testEntry.mbr);
                osal_printf("ext_mbr:%x\n", testEntry.ext_mbr);
                osal_printf("dsl_mbr:%x\n", testEntry.dsl_mbr);
                osal_printf("lut_pri:%x\n", testEntry.lut_pri);
                osal_printf("fwdpri_en:%x\n", testEntry.fwdpri_en);
                osal_printf("nosalearn:%x\n", testEntry.nosalearn);
                osal_printf("valid:%x\n", testEntry.valid);
                osal_printf("hit:%x busy:%x\n", testEntry.lookup_hit,testEntry.lookup_busy);
                osal_printf("\n");
#endif



    }

#if 0 /*test read entry directly*/
        osal_memset(&testRdEntry, 0, sizeof(apollo_lut_table_t));
        testRdEntry.method = RAW_LUT_READ_METHOD_ADDRESS;
        testRdEntry.address = 0x4;  
        if ((ret = apollo_raw_l2_lookUpTb_get(&testRdEntry)) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__, ret);
            return ret;   
        }
        osal_printf("read address:%x\n", testRdEntry.address);
        osal_printf("mac:%x:%x:%x:%x:%x:%x\n",(int32) testRdEntry.mac.octet[0], (int32)testRdEntry.mac.octet[1],
            (int32)testRdEntry.mac.octet[2] , (int32)testRdEntry.mac.octet[3], (int32)testRdEntry.mac.octet[4], (int32)testRdEntry.mac.octet[5]);
        osal_printf("cvid_fid:%x\n", testRdEntry.cvid_fid);
        osal_printf("l3lookup:%x\n", testRdEntry.l3lookup);
        osal_printf("ivl_svl:%x\n", testRdEntry.ivl_svl);
        osal_printf("mbr:%x\n", testRdEntry.mbr);
        osal_printf("ext_mbr:%x\n", testRdEntry.ext_mbr);
        osal_printf("dsl_mbr:%x\n", testRdEntry.dsl_mbr);
        osal_printf("lut_pri:%x\n", testRdEntry.lut_pri);
        osal_printf("fwdpri_en:%x\n", testRdEntry.fwdpri_en);
        osal_printf("nosalearn:%x\n", testRdEntry.nosalearn);
        osal_printf("valid:%x\n", testRdEntry.valid);
        osal_printf("hit:%x busy:%x\n", testRdEntry.lookup_hit,testRdEntry.lookup_busy);
        osal_printf("entryType:%x\n",  testRdEntry.table_type);    
#endif

    osal_memset(&testEntry, 0, sizeof(apollo_lut_table_t));
    osal_memset(&testRdEntry, 0, sizeof(apollo_lut_table_t));
    for (i = 0; i < 100; i++)
    {

        testEntry.mac.octet[0]=0x1|((1<<((i + 1)%8 ))+ i );
        testEntry.mac.octet[1]=0x0 + (1<<((i + 1)%8)) + i;
        testEntry.mac.octet[2]=0x0 + (1<<((i + 2)%8)) + i;
        testEntry.mac.octet[3]=0x0 + (1<<((i + 3)%8)) + i;
        testEntry.mac.octet[4]=0x0 + (1<<((i + 4)%8)) + i;
        testEntry.mac.octet[5]=0x0 + (1<<((i + 5)%8)) + i;
        testEntry.cvid_fid = 1<<(i%(pTable->fields[L2_MC_DSL_VID_FIDtf].len));
        testEntry.l3lookup = 0;
        testEntry.ivl_svl = 1 - testEntry.ivl_svl;
        testEntry.mbr = 1<<(i%(pTable->fields[L2_MC_DSL_MBRtf].len));
        testEntry.ext_mbr =  1<<(i%(pTable->fields[L2_MC_DSL_EXT_MBRtf].len));
        testEntry.dsl_mbr =  1<<(i%(pTable->fields[L2_MC_DSL_DSL_MBRtf].len));
        testEntry.lut_pri = 1<<(i%(pTable->fields[L2_MC_DSL_FWDPRItf].len));
        testEntry.fwdpri_en = 1 - testEntry.fwdpri_en;
        testEntry.nosalearn = 1;
        testEntry.valid = 1;
        testEntry.method = RAW_LUT_READ_METHOD_MAC;
        testEntry.table_type = RAW_LUT_ENTRY_TYPE_L2MC_DSL;
        testEntry.lookup_hit = 1;
        

        testRdEntry.mac.octet[0]=0x1|((1<<((i + 1)%8 ))+ i );
        testRdEntry.mac.octet[1]=0x0 + (1<<((i + 1)%8)) + i;
        testRdEntry.mac.octet[2]=0x0 + (1<<((i + 2)%8)) + i;
        testRdEntry.mac.octet[3]=0x0 + (1<<((i + 3)%8)) + i;
        testRdEntry.mac.octet[4]=0x0 + (1<<((i + 4)%8)) + i;
        testRdEntry.mac.octet[5]=0x0 + (1<<((i + 5)%8)) + i;
        testRdEntry.cvid_fid = 1<<(i%(pTable->fields[L2_MC_DSL_VID_FIDtf].len));
        testRdEntry.l3lookup = 0;
        testRdEntry.ivl_svl = 1 - testRdEntry.ivl_svl;
        testRdEntry.method = RAW_LUT_READ_METHOD_MAC;
        testRdEntry.address = 0x0;
        testRdEntry.table_type = RAW_LUT_ENTRY_TYPE_L2MC_DSL;
        if ((ret = apollo_raw_l2_lookUpTb_get(&testRdEntry)) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__, ret);
            return ret;   
        }
        testEntry.address = testRdEntry.address;
        if (osal_memcmp( &testEntry, &testRdEntry, sizeof(testRdEntry)))
        {
            osal_printf("%s(%u), index:%u \n", __FUNCTION__, __LINE__, i);
     #if 0   
                osal_printf("read address:%x\n", testRdEntry.address);
                osal_printf("mac:%x:%x:%x:%x:%x:%x\n",(int32) testRdEntry.mac.octet[0], (int32)testRdEntry.mac.octet[1],
                    (int32)testRdEntry.mac.octet[2] , (int32)testRdEntry.mac.octet[3], (int32)testRdEntry.mac.octet[4], (int32)testRdEntry.mac.octet[5]);
                osal_printf("cvid_fid:%x\n", testRdEntry.cvid_fid);
                osal_printf("l3lookup:%x\n", testRdEntry.l3lookup);
                osal_printf("ivl_svl:%x\n", testRdEntry.ivl_svl);
                osal_printf("mbr:%x\n", testRdEntry.mbr);
                osal_printf("ext_mbr:%x\n", testRdEntry.ext_mbr);
                osal_printf("dsl_mbr:%x\n", testRdEntry.dsl_mbr);
                osal_printf("lut_pri:%x\n", testRdEntry.lut_pri);
                osal_printf("fwdpri_en:%x\n", testRdEntry.fwdpri_en);
                osal_printf("nosalearn:%x\n", testRdEntry.nosalearn);
                osal_printf("valid:%x\n", testRdEntry.valid);
                osal_printf("hit:%x busy:%x\n", testRdEntry.lookup_hit,testRdEntry.lookup_busy);
                osal_printf("entryType:%x\n",  testRdEntry.table_type);
    #endif
            return RT_ERR_FAILED;
        }

    }
    return RT_ERR_OK;
}


int32 dal_lut_raw_l3McDslEntry_test(uint32 caseNo)
{
    int32 ret = RT_ERR_OK;
    uint32 entryIdx;
    uint32  test_tmp=0;
    uint32 i;
    apollo_lut_table_t testEntry;
    apollo_lut_table_t testRdEntry;
    rtk_table_t *pTable;
 	osal_memset(&testEntry, 0, sizeof(apollo_lut_table_t));
    pTable = table_find(L3_MC_DSLt);
    /* NULL means the table is not supported in this chip*/
    RT_PARAM_CHK((NULL == pTable), RT_ERR_CHIP_NOT_SUPPORTED);
    for (i =0; i < 100; i++)
    {
        testEntry.gip = (1<<(i%(pTable->fields[L3_MC_DSL_GIPtf].len))) + i;
        testEntry.sip_vid = 1<<(i%(pTable->fields[L3_MC_DSL_SIP_VIDtf].len));
        testEntry.l3lookup = 1;
        testEntry.gip_only = 0;// 1 - testEntry.gip_only;
        testEntry.mbr = 1<<(i%(pTable->fields[L3_MC_DSL_MBRtf].len));
        testEntry.ext_mbr =  1<<(i%(pTable->fields[L3_MC_DSL_EXT_MBRtf].len));
        testEntry.dsl_mbr =  1<<(i%(pTable->fields[L3_MC_DSL_DSL_MBRtf].len));
        testEntry.lut_pri = 1<<(i%(pTable->fields[L3_MC_DSL_FWDPRItf].len));
        testEntry.fwdpri_en = 1 - testEntry.fwdpri_en;
        testEntry.nosalearn = 1;
        testEntry.valid = 1;
        testEntry.method = RAW_LUT_READ_METHOD_MAC;
        testEntry.table_type = RAW_LUT_ENTRY_TYPE_L3MC_DSL;
        if ((ret = apollo_raw_l2_lookUpTb_set(&testEntry)) != RT_ERR_OK)
        {
            osal_printf("\n %s %d err:%d\n",__FUNCTION__,__LINE__, ret);
            return ret;   
        }
#if 0
        osal_printf("Write address:%x\n", testEntry.address);
        osal_printf("gip:%x\n", testEntry.gip);
        osal_printf("sip_vid:%x\n", testEntry.sip_vid);
        osal_printf("l3lookup:%x\n", testEntry.l3lookup);
        osal_printf("gip_only:%x\n", testEntry.gip_only);
        osal_printf("mbr:%x\n", testEntry.mbr);
        osal_printf("ext_mbr:%x\n", testEntry.ext_mbr);
        osal_printf("dsl_mbr:%x\n", testEntry.dsl_mbr);
        osal_printf("lut_pri:%x\n", testEntry.lut_pri);
        osal_printf("fwdpri_en:%x\n", testEntry.fwdpri_en);
        osal_printf("nosalearn:%x\n", testEntry.nosalearn);
        osal_printf("valid:%x\n", testEntry.valid);
        osal_printf("hit:%x busy:%x\n", testEntry.lookup_hit,testEntry.lookup_busy);
          
#endif



    }
#if 0 /*test read entry directly*/
    osal_memset(&testRdEntry, 0, sizeof(apollo_lut_table_t));
    testRdEntry.method = RAW_LUT_READ_METHOD_ADDRESS;
    testRdEntry.address = 0x624;  
    if ((ret = apollo_raw_l2_lookUpTb_get(&testRdEntry)) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__, ret);
        return ret;   
    }
    osal_printf("read address:%x\n", testRdEntry.address);
    osal_printf("gip:%x\n", testRdEntry.gip);
    osal_printf("sip_vid:%x\n", testRdEntry.sip_vid);
    osal_printf("l3lookup:%x\n", testRdEntry.l3lookup);
    osal_printf("gip_only:%x\n", testRdEntry.gip_only);
    osal_printf("mbr:%x\n", testRdEntry.mbr);
    osal_printf("ext_mbr:%x\n", testRdEntry.ext_mbr);
    osal_printf("dsl_mbr:%x\n", testRdEntry.dsl_mbr);
    osal_printf("lut_pri:%x\n", testRdEntry.lut_pri);
    osal_printf("fwdpri_en:%x\n", testRdEntry.fwdpri_en);
    osal_printf("nosalearn:%x\n", testRdEntry.nosalearn);
    osal_printf("valid:%x\n", testRdEntry.valid);
    osal_printf("hit:%x busy:%x\n", testRdEntry.lookup_hit,testRdEntry.lookup_busy);
    osal_printf("entryType:%x\n",  testRdEntry.table_type);

#endif
 
    osal_memset(&testEntry, 0, sizeof(apollo_lut_table_t));
    osal_memset(&testRdEntry, 0, sizeof(apollo_lut_table_t));
    for (i = 0; i < 100; i++)
    {

        testEntry.gip = (1<<(i%(pTable->fields[L3_MC_DSL_GIPtf].len))) + i;
        testEntry.sip_vid = 1<<(i%(pTable->fields[L3_MC_DSL_SIP_VIDtf].len));
        testEntry.l3lookup = 1;
        testEntry.gip_only = 0;// 1 - testEntry.gip_only;
        testEntry.mbr = 1<<(i%(pTable->fields[L3_MC_DSL_MBRtf].len));
        testEntry.ext_mbr =  1<<(i%(pTable->fields[L3_MC_DSL_EXT_MBRtf].len));
        testEntry.dsl_mbr =  1<<(i%(pTable->fields[L3_MC_DSL_DSL_MBRtf].len));
        testEntry.lut_pri = 1<<(i%(pTable->fields[L3_MC_DSL_FWDPRItf].len));
        testEntry.fwdpri_en = 1 - testEntry.fwdpri_en;
        testEntry.nosalearn = 1;
        testEntry.valid = 1;
        testEntry.method = RAW_LUT_READ_METHOD_MAC;
        testEntry.table_type = RAW_LUT_ENTRY_TYPE_L3MC_DSL;
        testEntry.lookup_hit = 1;
        

        testRdEntry.gip = (1<<(i%(pTable->fields[L3_MC_DSL_GIPtf].len))) + i;
        testRdEntry.sip_vid = 1<<(i%(pTable->fields[L3_MC_DSL_SIP_VIDtf].len));
        testRdEntry.l3lookup = 1;
        testRdEntry.gip_only = 0;// 1 - testRdEntry.gip_only;
        testRdEntry.method = RAW_LUT_READ_METHOD_MAC;
        testRdEntry.table_type = RAW_LUT_ENTRY_TYPE_L3MC_DSL;
        if ((ret = apollo_raw_l2_lookUpTb_get(&testRdEntry)) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__, ret);
            return ret;   
        }
        testEntry.address = testRdEntry.address;
        if (osal_memcmp( &testEntry, &testRdEntry, sizeof(apollo_lut_table_t)))
        {
            osal_printf("%s(%u), index:%u \n", __FUNCTION__, __LINE__, i);
     #if 0   
            osal_printf("read address:%x\n", testRdEntry.address);
            osal_printf("gip:%x\n", testRdEntry.gip);
            osal_printf("sip_vid:%x\n", testRdEntry.sip_vid);
            osal_printf("l3lookup:%x\n", testRdEntry.l3lookup);
            osal_printf("gip_only:%x\n", testRdEntry.gip_only);
            osal_printf("mbr:%x\n", testRdEntry.mbr);
            osal_printf("ext_mbr:%x\n", testRdEntry.ext_mbr);
            osal_printf("dsl_mbr:%x\n", testRdEntry.dsl_mbr);
            osal_printf("lut_pri:%x\n", testRdEntry.lut_pri);
            osal_printf("fwdpri_en:%x\n", testRdEntry.fwdpri_en);
            osal_printf("nosalearn:%x\n", testRdEntry.nosalearn);
            osal_printf("valid:%x\n", testRdEntry.valid);
            osal_printf("hit:%x busy:%x\n", testRdEntry.lookup_hit,testRdEntry.lookup_busy);
            osal_printf("entryType:%x\n",  testRdEntry.table_type);

   
    #endif
            return RT_ERR_FAILED;
        }

    }
    return RT_ERR_OK;
}


int32 dal_lut_raw_l3McRouteEntry_test(uint32 caseNo)
{
    int32 ret = RT_ERR_OK;
    uint32 entryIdx;
    uint32  test_tmp=0;
    uint32 i;
    apollo_lut_table_t testEntry;
    apollo_lut_table_t testRdEntry;
    rtk_table_t *pTable;
    	
    pTable = table_find(L3_MC_ROUTEt);
    /* NULL means the table is not supported in this chip*/
    RT_PARAM_CHK((NULL == pTable), RT_ERR_CHIP_NOT_SUPPORTED);
    osal_memset(&testEntry, 0, sizeof(apollo_lut_table_t));
    for (i =0; i < 100; i++)
    {
        testEntry.gip = (1<<(i%(pTable->fields[L3_MC_ROUTE_GIPtf].len))) + i;
        testEntry.l3_idx = 1<<(i%(pTable->fields[L3_MC_ROUTE_L3_IDXtf].len));
        testEntry.ext_fr = 1 - testEntry.ext_fr;
        testEntry.wan_sa = 1; // - testEntry.wan_sa;
        testEntry.l3lookup = 1;
        testEntry.gip_only = 1 ;// - testEntry.gip_only;
        testEntry.mbr = 1<<(i%(pTable->fields[L3_MC_ROUTE_MBRtf].len));
        testEntry.ext_mbr =  1<<(i%(pTable->fields[L3_MC_ROUTE_EXT_MBRtf].len));
        testEntry.dsl_mbr =  1<<(i%(pTable->fields[L3_MC_ROUTE_DSL_MBRtf].len));
        testEntry.lut_pri = 1<<(i%(pTable->fields[L3_MC_ROUTE_FWDPRItf].len));
        testEntry.fwdpri_en = 1 - testEntry.fwdpri_en;
        testEntry.nosalearn = 1;
        testEntry.valid = 1;
        testEntry.method = RAW_LUT_READ_METHOD_MAC;
        testEntry.table_type = RAW_LUT_ENTRY_TYPE_L3MC_ROUTE;
        if ((ret = apollo_raw_l2_lookUpTb_set(&testEntry)) != RT_ERR_OK)
        {
            osal_printf("\n %s %d err:%d\n",__FUNCTION__,__LINE__, ret);
            return ret;   
        }
#if 0
        osal_printf("test idx:%x\n", i);
        osal_printf("Write address:%x\n", testEntry.address);
        osal_printf("gip:%x\n", testEntry.gip);
        osal_printf("l3_idx:%x\n",  testEntry.l3_idx);
        osal_printf("ext_fr:%x\n",  testEntry.ext_fr);
        osal_printf("wan_sa:%x\n",  testEntry.wan_sa);
        osal_printf("l3lookup:%x\n", testEntry.l3lookup);
        osal_printf("gip_only:%x\n", testEntry.gip_only);
        osal_printf("mbr:%x\n", testEntry.mbr);
        osal_printf("ext_mbr:%x\n", testEntry.ext_mbr);
        osal_printf("dsl_mbr:%x\n", testEntry.dsl_mbr);
        osal_printf("lut_pri:%x\n", testEntry.lut_pri);
        osal_printf("fwdpri_en:%x\n", testEntry.fwdpri_en);
        osal_printf("nosalearn:%x\n", testEntry.nosalearn);
        osal_printf("valid:%x\n", testEntry.valid);
        osal_printf("hit:%x busy:%x\n", testEntry.lookup_hit,testEntry.lookup_busy);
        osal_printf("\n");
  
#endif

    }
#if 0 /*test read entry directly*/
        osal_memset(&testRdEntry, 0, sizeof(apollo_lut_table_t));
        testRdEntry.method = RAW_LUT_READ_METHOD_ADDRESS;
        testRdEntry.address = 0x5;  
        if ((ret = apollo_raw_l2_lookUpTb_get(&testRdEntry)) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__, ret);
            return ret;   
        }
        osal_printf("read address:%x\n", testRdEntry.address);
        osal_printf("gip:%x\n", testRdEntry.gip);
        osal_printf("l3_idx:%x\n",  testEntry.l3_idx);
        osal_printf("ext_fr:%x\n",  testEntry.ext_fr);
        osal_printf("wan_sa:%x\n",  testEntry.wan_sa);
        osal_printf("l3lookup:%x\n", testRdEntry.l3lookup);
        osal_printf("gip_only:%x\n", testRdEntry.gip_only);
        osal_printf("mbr:%x\n", testRdEntry.mbr);
        osal_printf("ext_mbr:%x\n", testRdEntry.ext_mbr);
        osal_printf("dsl_mbr:%x\n", testRdEntry.dsl_mbr);
        osal_printf("lut_pri:%x\n", testRdEntry.lut_pri);
        osal_printf("fwdpri_en:%x\n", testRdEntry.fwdpri_en);
        osal_printf("nosalearn:%x\n", testRdEntry.nosalearn);
        osal_printf("valid:%x\n", testRdEntry.valid);
        osal_printf("hit:%x busy:%x\n", testRdEntry.lookup_hit,testRdEntry.lookup_busy);
        osal_printf("entryType:%x\n",  testRdEntry.table_type);
#endif

 
    osal_memset(&testEntry, 0, sizeof(apollo_lut_table_t));
    osal_memset(&testRdEntry, 0, sizeof(apollo_lut_table_t));
    for (i = 0; i < 100; i++)
    {

        testEntry.gip = (1<<(i%(pTable->fields[L3_MC_ROUTE_GIPtf].len))) + i;
        testEntry.l3_idx = 1<<(i%(pTable->fields[L3_MC_ROUTE_L3_IDXtf].len));
        testEntry.ext_fr = 1 - testEntry.ext_fr;
        testEntry.wan_sa = 1;// - testEntry.wan_sa;
        testEntry.l3lookup = 1;
        testEntry.gip_only = 1;// - testEntry.gip_only;
        testEntry.mbr = 1<<(i%(pTable->fields[L3_MC_ROUTE_MBRtf].len));
        testEntry.ext_mbr =  1<<(i%(pTable->fields[L3_MC_ROUTE_EXT_MBRtf].len));
        testEntry.dsl_mbr =  1<<(i%(pTable->fields[L3_MC_ROUTE_DSL_MBRtf].len));
        testEntry.lut_pri = 1<<(i%(pTable->fields[L3_MC_ROUTE_FWDPRItf].len));
        testEntry.fwdpri_en = 1 - testEntry.fwdpri_en;
        testEntry.nosalearn = 1;
        testEntry.valid = 1;
        testEntry.method = RAW_LUT_READ_METHOD_MAC;
        testEntry.table_type = RAW_LUT_ENTRY_TYPE_L3MC_ROUTE;
        testEntry.lookup_hit = 1;
        

        testRdEntry.gip = (1<<(i%(pTable->fields[L3_MC_ROUTE_GIPtf].len))) + i;
        testRdEntry.l3_idx = 1<<(i%(pTable->fields[L3_MC_ROUTE_L3_IDXtf].len));
        testRdEntry.wan_sa = 1;
        testRdEntry.l3lookup = 1;
        testRdEntry.gip_only = 1 ;//- testRdEntry.gip_only;
        testRdEntry.method = RAW_LUT_READ_METHOD_MAC;
        testRdEntry.table_type = RAW_LUT_ENTRY_TYPE_L3MC_ROUTE;
        if ((ret = apollo_raw_l2_lookUpTb_get(&testRdEntry)) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__, ret);
            return ret;   
        }
        testEntry.address = testRdEntry.address;
        if (osal_memcmp( &testEntry, &testRdEntry, sizeof(apollo_lut_table_t)))
        {
            osal_printf("%s(%u), index:%u \n", __FUNCTION__, __LINE__, i);
     #if 0   
            osal_printf("read address:%x\n", testRdEntry.address);
            osal_printf("gip:%x\n", testRdEntry.gip);
            osal_printf("l3_idx:%x\n",  testEntry.l3_idx);
            osal_printf("ext_fr:%x\n",  testEntry.ext_fr);
            osal_printf("wan_sa:%x\n",  testEntry.wan_sa);
            osal_printf("l3lookup:%x\n", testRdEntry.l3lookup);
            osal_printf("gip_only:%x\n", testRdEntry.gip_only);
            osal_printf("mbr:%x\n", testRdEntry.mbr);
            osal_printf("ext_mbr:%x\n", testRdEntry.ext_mbr);
            osal_printf("dsl_mbr:%x\n", testRdEntry.dsl_mbr);
            osal_printf("lut_pri:%x\n", testRdEntry.lut_pri);
            osal_printf("fwdpri_en:%x\n", testRdEntry.fwdpri_en);
            osal_printf("nosalearn:%x\n", testRdEntry.nosalearn);
            osal_printf("valid:%x\n", testRdEntry.valid);
            osal_printf("hit:%x busy:%x\n", testRdEntry.lookup_hit,testRdEntry.lookup_busy);
            osal_printf("entryType:%x\n",  testRdEntry.table_type);
            
    #endif
            return RT_ERR_FAILED;
        }

    }

    return RT_ERR_OK;
}


int32 dal_lut_raw_test(uint32 caseNo)
{
    int32 ret;
    rtk_port_t port;
    rtk_enable_t enableR;
    rtk_enable_t enableW;
    uint32 numR;
    uint32 numW;
    uint32 efidR;
    uint32 efidW;
    uint32 priR;
    uint32 priW;
    apollo_raw_l2_ipMcLookupOp_t opTypeR;
    apollo_raw_l2_ipMcLookupOp_t opTypeW;
    apollo_raw_l2_ipMcHashType_t hashTypeW;
    apollo_raw_l2_ipMcHashType_t hashTypeR;
    uint32 ageTimeW;
    uint32 ageTimeR;
    apollo_raw_flush_ctrl_t flushCtrlW;
    apollo_raw_flush_ctrl_t flushCtrlR;


    /*error input check*/
    /*out of range*/
    if( apollo_raw_l2_flushEn_set(APOLLO_PORTNO, 0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l2_flushEn_set(0, RTK_ENABLE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l2_agingEnable_set(APOLLO_PORTNO, 0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l2_agingEnable_set(0, RTK_ENABLE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l2_bcFlood_set(APOLLO_PORTNO, 0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l2_bcFlood_set(0, RTK_ENABLE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l2_unknUcFlood_set(APOLLO_PORTNO, 0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l2_unknUcFlood_set(0, RTK_ENABLE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l2_unknMcFlood_set(APOLLO_PORTNO, 0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l2_unknMcFlood_set(0, RTK_ENABLE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l2_camEnable_set(RTK_ENABLE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l2_flushLinkDownPortAddrEnable_set(RTK_ENABLE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l2_lrnLimitNo_set(APOLLO_PORTNO, 0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l2_lrnLimitNo_set(0, APOLLO_LUT_4WAY_NO+APOLLO_LUT_CAM_NO) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l2_sysLrnLimitNo_set(APOLLO_LUT_4WAY_NO+APOLLO_LUT_CAM_NO) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l2_efid_set(APOLLO_PORTNO, 0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l2_efid_set(0, APOLLO_EFIDMAX+1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_unkn_mcPri_set(8) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l2_ipmcLookupOp_set(RAW_LUT_IPMCLOOKUP_TYPE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l2_ipmcHashType_set(RAW_LUT_IPMCHASH_TYPE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l2_agingTime_set(APOLLO_RAW_LUT_AGESPEEDMAX+1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }


    osal_memset(&flushCtrlW,0x0, sizeof(apollo_raw_flush_ctrl_t));
    flushCtrlW.fid = APOLLO_FIDMAX + 1;
    if( apollo_raw_l2_flushCtrl_set(&flushCtrlW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&flushCtrlW,0x0, sizeof(apollo_raw_flush_ctrl_t));
    flushCtrlW.vid = APOLLO_VIDMAX + 1;
    if( apollo_raw_l2_flushCtrl_set(&flushCtrlW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&flushCtrlW,0x0, sizeof(apollo_raw_flush_ctrl_t));
    flushCtrlW.flushMode = RAW_FLUSH_MODE_END;
    if( apollo_raw_l2_flushCtrl_set(&flushCtrlW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&flushCtrlW,0x0, sizeof(apollo_raw_flush_ctrl_t));
    flushCtrlW.flushType = RAW_FLUSH_TYPE_END;
    if( apollo_raw_l2_flushCtrl_set(&flushCtrlW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }



    /* get/set test*/
    for(port=0; port<=APOLLO_PORTIDMAX; port++)
    {
        for(enableW = DISABLED; enableW < RTK_ENABLE_END; enableW ++)
        {
#if 0
            if( apollo_raw_l2_flushEn_set(port, enableW) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            if( apollo_raw_l2_flushEn_get(port, &enableR) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if(enableW != enableR)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
#endif
            if( apollo_raw_l2_agingEnable_set(port, enableW) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            if( apollo_raw_l2_agingEnable_get(port, &enableR) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if(enableW != enableR)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if( apollo_raw_l2_bcFlood_set(port, enableW) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            if( apollo_raw_l2_bcFlood_get(port, &enableR) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if(enableW != enableR)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if( apollo_raw_l2_unknUcFlood_set(port, enableW) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            if( apollo_raw_l2_unknUcFlood_get(port, &enableR) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if(enableW != enableR)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if( apollo_raw_l2_unknMcFlood_set(port, enableW) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            if( apollo_raw_l2_unknMcFlood_get(port, &enableR) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if(enableW != enableR)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
        }

        for(numW = 0; numW < (2048+64); numW +=(2408+64-1))
        {
            if( apollo_raw_l2_lrnLimitNo_set(port, numW) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if( apollo_raw_l2_lrnLimitNo_get(port, &numR) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if(numW != numR)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
        }

        for(efidW = 0; efidW <= APOLLO_EFIDMAX; efidW +=3)
        {
            if( apollo_raw_l2_efid_set(port, efidW) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if( apollo_raw_l2_efid_get(port, &efidR) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if(efidW != efidR)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
        }

    }

    for(priW = 0; priW < 7; priW++)
    {
        if( apollo_raw_unkn_mcPri_set(priW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if( apollo_raw_unkn_mcPri_get(&priR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if(priW != priR)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }

    for(numW = 0; numW < (2048+64); numW +=(2408+64-1))
    {
        if( apollo_raw_l2_sysLrnLimitNo_set(numW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        
        if( apollo_raw_l2_sysLrnLimitNo_get(&numR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    
        if(numW != numR)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }           
    }


    for(enableW = DISABLED; enableW < RTK_ENABLE_END; enableW ++)
    {
        if( apollo_raw_l2_camEnable_set(enableW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if( apollo_raw_l2_camEnable_get(&enableR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if(enableW != enableR)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if( apollo_raw_l2_flushLinkDownPortAddrEnable_set(enableW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if( apollo_raw_l2_flushLinkDownPortAddrEnable_get(&enableR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if(enableW != enableR)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

    }

    for(enableW = DISABLED; enableW < RTK_ENABLE_END; enableW ++)
    {
        if( apollo_raw_l2_camEnable_set(enableW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if( apollo_raw_l2_camEnable_get(&enableR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if(enableW != enableR)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }

    for(opTypeW = RAW_LUT_IPMCLOOKUP_TYPE_DIP; opTypeW < RAW_LUT_IPMCLOOKUP_TYPE_END; opTypeW ++)
    {
        if( apollo_raw_l2_ipmcLookupOp_set(opTypeW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if( apollo_raw_l2_ipmcLookupOp_get(&opTypeR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if(opTypeW != opTypeR)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }

    for(hashTypeW = RAW_LUT_IPMCHASH_TYPE_DMACFID; hashTypeW < RAW_LUT_IPMCHASH_TYPE_END; hashTypeW ++)
    {
        if( apollo_raw_l2_ipmcHashType_set(hashTypeW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if( apollo_raw_l2_ipmcHashType_get(&hashTypeR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if(hashTypeW != hashTypeR)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }

    for(ageTimeW = 0; ageTimeW <= 0x1FFFF; ageTimeW += 0x3FFF)
    {
        if( apollo_raw_l2_agingTime_set(ageTimeW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if( apollo_raw_l2_agingTime_get(&ageTimeR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if(ageTimeW != ageTimeR)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }

    for(flushCtrlW.flushMode = RAW_FLUSH_MODE_PORT; flushCtrlW.flushMode < RAW_FLUSH_MODE_END; flushCtrlW.flushMode++)
    {
        for(flushCtrlW.flushType = RAW_FLUSH_TYPE_DYNAMIC; flushCtrlW.flushType < RAW_FLUSH_TYPE_END; flushCtrlW.flushType++)
        {
            for(flushCtrlW.vid = 0; flushCtrlW.vid < 4096; flushCtrlW.vid += 0xFFF)
            {
                for(flushCtrlW.fid = 0; flushCtrlW.fid < 16; flushCtrlW.fid += 3)
                {
                    if( apollo_raw_l2_flushCtrl_set(&flushCtrlW) != RT_ERR_OK)
                    {
                        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                        return RT_ERR_FAILED;
                    }

                    if( apollo_raw_l2_flushCtrl_get(&flushCtrlR) != RT_ERR_OK)
                    {
                        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                        return RT_ERR_FAILED;
                    }

                    if((flushCtrlW.flushMode != flushCtrlR.flushMode) ||
                            (flushCtrlW.flushType != flushCtrlR.flushType) ||
                            (flushCtrlW.vid != flushCtrlR.vid) ||
                            (flushCtrlW.fid != flushCtrlR.fid))
                    {
                        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                        return RT_ERR_FAILED;
                    }
                }
            }
        }
    }
    /*null pointer*/
    if( apollo_raw_l2_flushEn_get(0,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l2_agingEnable_get(0,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l2_bcFlood_get(0,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l2_unknUcFlood_get(0,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l2_unknMcFlood_get(0,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l2_camEnable_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l2_flushLinkDownPortAddrEnable_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l2_lrnLimitNo_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l2_sysLrnLimitNo_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l2_efid_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_unkn_mcPri_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l2_ipmcLookupOp_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l2_ipmcHashType_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l2_agingTime_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l2_flushCtrl_set(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l2_flushCtrl_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32 dal_lut_raw_action_test(uint32 caseNo)
{
    int32 ret;
    rtk_port_t port;
    rtk_action_t actionW;
    rtk_action_t actionR;

    /*error input check*/
    /*out of range*/
    if( apollo_raw_l2_unMatched_saCtl_set(APOLLO_PORTNO, ACTION_FORWARD) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l2_unMatched_saCtl_set(0, ACTION_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l2_unkn_saCtl_set(APOLLO_PORTNO, ACTION_FORWARD) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l2_unkn_saCtl_set(0, ACTION_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }


    if( apollo_raw_l2_unkn_ucDaCtl_set(APOLLO_PORTNO, ACTION_FORWARD) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l2_unkn_ucDaCtl_set(0, ACTION_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l2_learnOverAct_set(APOLLO_PORTNO, ACTION_FORWARD) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l2_learnOverAct_set(0, ACTION_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l2_sysLrnOverAct_set(ACTION_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l2_ipmcAction_set(APOLLO_PORTNO, ACTION_FORWARD) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l2_ipmcAction_set(0, ACTION_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* get/set test*/
    for(port=0; port<=APOLLO_PORTIDMAX; port++)
    {
        for(actionW = ACTION_FORWARD; actionW < ACTION_END; actionW ++)
        {
            if( apollo_raw_l2_unMatched_saCtl_set(port, actionW) == RT_ERR_OK)
            {
                if( apollo_raw_l2_unMatched_saCtl_get(port, &actionR) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }

                if(actionW != actionR)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }
            }

            if( apollo_raw_l2_unkn_saCtl_set(port, actionW) == RT_ERR_OK)
            {
                if( apollo_raw_l2_unkn_saCtl_get(port, &actionR) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }

                if(actionW != actionR)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }
            }

            if( apollo_raw_l2_unkn_ucDaCtl_set(port, actionW) == RT_ERR_OK)
            {
                if( apollo_raw_l2_unkn_ucDaCtl_get(port, &actionR) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }

                if(actionW != actionR)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }
            }

            if( apollo_raw_l2_learnOverAct_set(port, actionW) == RT_ERR_OK)
            {
                if( apollo_raw_l2_learnOverAct_get(port, &actionR) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }

                if(actionW != actionR)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }
            }

            if( apollo_raw_l2_ipmcAction_set(port, actionW) == RT_ERR_OK)
            {
                if( apollo_raw_l2_ipmcAction_get(port, &actionR) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }

                if(actionW != actionR)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }
            }
        }
    }

    for(actionW = ACTION_FORWARD; actionW < ACTION_END; actionW ++)
    {
        if( apollo_raw_l2_sysLrnOverAct_set(actionW) == RT_ERR_OK)
        {
            if( apollo_raw_l2_sysLrnOverAct_get(&actionR) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if(actionW != actionR)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
        }
    }


    /*null pointer*/
    if( apollo_raw_l2_unMatched_saCtl_get(0,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l2_unkn_saCtl_get(0,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l2_unkn_ucDaCtl_get(0,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l2_learnOverAct_get(0,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l2_sysLrnOverAct_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l2_ipmcAction_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32 dal_lut_raw_ipmcTable_test(uint32 caseNo)
{
    int32 ret;
    int32 index;
    rtk_ip_addr_t dipR;
    uint32 pmskR;
    rtk_ip_addr_t dipW;
    uint32 pmskW;

    /*error input check*/
    /*out of range*/
    if( apollo_raw_l2_igmp_Mc_table_set(APOLLO_IPMC_TABLE_IDX_MAX+1, 0xE0000000, 0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l2_igmp_Mc_table_set(0, 0xE0000000, (1 << APOLLO_PORTNO)) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l2_igmp_Mc_table_set(0, 0xD0000000, (1 << APOLLO_PORTNO)) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* get/set test*/
    for(index=0; index<=APOLLO_IPMC_TABLE_IDX_MAX; index++)
    {
        for(pmskW = 0; pmskW <=0x3F; pmskW +=0x1F)
        {
            for(dipW = 0xE0000000; dipW <0xFFFFFFFF; dipW +=0x1FFFFFFF)
            {
                if( apollo_raw_l2_igmp_Mc_table_set(index, dipW, pmskW) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }

                if( apollo_raw_l2_igmp_Mc_table_get(index, &dipR, &pmskR) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }

                if((dipW != dipR) || (pmskW != pmskR))
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }
            }
        }
    }

    /*null pointer*/
    if( apollo_raw_l2_igmp_Mc_table_get(0, &dipR, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_l2_igmp_Mc_table_get(0, NULL, &pmskR) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32 dal_lut_raw_unknIp4Mc_test(uint32 caseNo)
{
    rtk_port_t port;
    rtk_action_t actionW;
    rtk_action_t actionR;

    /*error input check*/
    /*out of range*/
    if( apollo_raw_unkn_ip4Mc_set(APOLLO_PORTIDMAX+1, ACTION_FORWARD) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_unkn_ip4Mc_set(0, ACTION_COPY2CPU) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_unkn_ip4Mc_set(0, ACTION_TO_GUESTVLAN) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_unkn_ip4Mc_set(0, ACTION_TO_GUESTVLAN) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_unkn_ip4Mc_set(0, ACTION_FLOOD_IN_VLAN) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_unkn_ip4Mc_set(0, ACTION_FLOOD_IN_ALL_PORT) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_unkn_ip4Mc_set(0, ACTION_FLOOD_IN_ROUTER_PORTS) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_unkn_ip4Mc_set(0, ACTION_FORWARD_EXCLUDE_CPU) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_unkn_ip4Mc_set(0, ACTION_DROP_EXCLUDE_RMA) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_unkn_ip4Mc_set(0, ACTION_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_unkn_ip4Mc_set(0, ACTION_END + 1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* get/set test*/
    for(port = 0; port <= APOLLO_PORTIDMAX; port++)
    {
        for(actionW = ACTION_FORWARD; actionW <= ACTION_TRAP2CPU; actionW++)
        {
            if( apollo_raw_unkn_ip4Mc_set(port, actionW) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if( apollo_raw_unkn_ip4Mc_get(port, &actionR) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if(actionW != actionR)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
        }
    }

    /*null pointer*/
    if( apollo_raw_unkn_ip4Mc_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32 dal_lut_raw_unknIp6Mc_test(uint32 caseNo)
{
    rtk_port_t port;
    rtk_action_t actionW;
    rtk_action_t actionR;

    /*error input check*/
    /*out of range*/
    if( apollo_raw_unkn_ip6Mc_set(APOLLO_PORTIDMAX+1, ACTION_FORWARD) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_unkn_ip6Mc_set(0, ACTION_COPY2CPU) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_unkn_ip6Mc_set(0, ACTION_TO_GUESTVLAN) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_unkn_ip6Mc_set(0, ACTION_TO_GUESTVLAN) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_unkn_ip6Mc_set(0, ACTION_FLOOD_IN_VLAN) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_unkn_ip6Mc_set(0, ACTION_FLOOD_IN_ALL_PORT) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_unkn_ip6Mc_set(0, ACTION_FLOOD_IN_ROUTER_PORTS) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_unkn_ip6Mc_set(0, ACTION_FORWARD_EXCLUDE_CPU) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_unkn_ip6Mc_set(0, ACTION_DROP_EXCLUDE_RMA) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_unkn_ip6Mc_set(0, ACTION_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_unkn_ip6Mc_set(0, ACTION_END + 1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* get/set test*/
    for(port = 0; port <= APOLLO_PORTIDMAX; port++)
    {
        for(actionW = ACTION_FORWARD; actionW <= ACTION_TRAP2CPU; actionW++)
        {
            if( apollo_raw_unkn_ip6Mc_set(port, actionW) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if( apollo_raw_unkn_ip6Mc_get(port, &actionR) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if(actionW != actionR)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
        }
    }

    /*null pointer*/
    if( apollo_raw_unkn_ip6Mc_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32 dal_lut_raw_unknL2Mc_test(uint32 caseNo)
{
    rtk_port_t port;
    rtk_action_t actionW;
    rtk_action_t actionR;

    /*error input check*/
    /*out of range*/
    if( apollo_raw_unkn_l2Mc_set(APOLLO_PORTIDMAX+1, ACTION_FORWARD) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_unkn_l2Mc_set(0, ACTION_COPY2CPU) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_unkn_l2Mc_set(0, ACTION_TO_GUESTVLAN) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_unkn_l2Mc_set(0, ACTION_TO_GUESTVLAN) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_unkn_l2Mc_set(0, ACTION_FLOOD_IN_VLAN) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_unkn_l2Mc_set(0, ACTION_FLOOD_IN_ALL_PORT) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_unkn_l2Mc_set(0, ACTION_FLOOD_IN_ROUTER_PORTS) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_unkn_l2Mc_set(0, ACTION_FORWARD_EXCLUDE_CPU) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_unkn_l2Mc_set(0, ACTION_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_unkn_l2Mc_set(0, ACTION_END + 1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* get/set test*/
    for(port = 0; port <= APOLLO_PORTIDMAX; port++)
    {
        for(actionW = ACTION_FORWARD; actionW < ACTION_END; actionW++)
        {
            if( (actionW == ACTION_FORWARD) || (actionW == ACTION_DROP) || (actionW == ACTION_TRAP2CPU) || (actionW == ACTION_DROP_EXCLUDE_RMA) )
            {
                if( apollo_raw_unkn_l2Mc_set(port, actionW) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }

                if( apollo_raw_unkn_l2Mc_get(port, &actionR) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }

                if(actionW != actionR)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }
            }
        }
    }

    /*null pointer*/
    if( apollo_raw_unkn_l2Mc_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32 dal_lut_raw_unknMcPri_test(uint32 caseNo)
{
    uint32 priW;
    uint32 priR;

    /*error input check*/
    /*out of range*/
    if( apollo_raw_unkn_mcPri_set(APOLLO_PRIMAX + 1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* get/set test*/
    for(priW = 0; priW <= APOLLO_PRIMAX; priW++)
    {
        if( apollo_raw_unkn_mcPri_set(priW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if( apollo_raw_unkn_mcPri_get(&priR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if(priW != priR)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }

    /*null pointer*/
    if( apollo_raw_unkn_mcPri_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}


