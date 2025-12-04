#include <osal/lib.h>
#include <common/debug/rt_log.h>
#include <dal/apollo/raw/apollo_raw_cf.h>
#include <ioal/mem32.h>
#include <dal/raw/apollo/dal_raw_cf_test_case.h>


//#define CONFIG_FPGA_TEST
/* Define symbol used for test input */
int32 dal_raw_cf_ruleAll_test(uint32 caseNo)
{
    int32 ret = RT_ERR_FAILED;
    int32 test_port = 0;
    uint32 idx;
    uint32  test_tmp=0;

    apollo_raw_cf_RuleEntry_t pattern;
    apollo_raw_cf_RuleEntry_t patternRd;

	uint32 index;
	uint32 type,upperValue,lowerValue;


    osal_memset(&pattern,0,sizeof(apollo_raw_cf_RuleEntry_t));
    osal_memset(&patternRd,0,sizeof(apollo_raw_cf_RuleEntry_t));
#if 0
    pattern.care_bits.etherType = 0xffff;
    pattern.care_bits.ifCtag = 1;
    pattern.care_bits.ifStag = 1;
    pattern.care_bits.interPri = 0x7;
    pattern.care_bits.tagPri = 0x7;
    pattern.care_bits.tagVid = 0xfff;
    pattern.care_bits.tos_gemidx = 0xff;
    pattern.care_bits.direction = 1;
    pattern.care_bits.uni = 0x7;
    pattern.data_bits.etherType = 0xffff;
    pattern.data_bits.ifCtag = 1;
    pattern.data_bits.ifStag = 1;
    pattern.data_bits.interPri = 0x7;
    pattern.data_bits.tagPri = 0x7;
    pattern.data_bits.tagVid = 0xfff;
    pattern.data_bits.tos_gemidx = 0xff;
    pattern.data_bits.direction = 1;
    pattern.data_bits.uni = 0x7;
    pattern.valid = 1;

#else
    pattern.care_bits.etherType = 0xffff;
    pattern.care_bits.ifCtag = 1;
    pattern.care_bits.ifStag = 1;
    pattern.care_bits.interPri = 0x7;
    pattern.care_bits.tagPri = 0x7;
    pattern.care_bits.tagVid = 0xfff;
    pattern.care_bits.tos_gemidx = 0x7f;
    pattern.care_bits.direction = 1;
    pattern.care_bits.uni = 0x5;
    pattern.data_bits.etherType = 0xeeee;
    pattern.data_bits.ifCtag = 0;
    pattern.data_bits.ifStag = 1;
    pattern.data_bits.interPri = 0x3;
    pattern.data_bits.tagPri = 0x7;
    pattern.data_bits.tagVid = 0xeee;
    pattern.data_bits.tos_gemidx = 0x7e;
    pattern.data_bits.direction = 1;
    pattern.data_bits.uni = 0x5;
    pattern.valid = 1;
#endif

    for(idx = 0; idx <= APOLLO_CF_PATTERN_MAX; idx++)
    {
        if (idx >= 128)
        {
            pattern.care_bits.etherType = 0;
            pattern.data_bits.etherType = 0;
        }
        pattern.idx = idx;
        if((ret = apollo_raw_cf_rule_set(&pattern)) != RT_ERR_OK)
        {
           osal_printf("\n %s %d, idx=%d, ret=%d\n",__FUNCTION__,__LINE__,idx,ret);
           return ret;
        }
#if defined(CONFIG_FPGA_TEST)
        if((idx&0x7) == 0x7)
            idx += 120;
#endif
    }
    pattern.care_bits.etherType = 0xffff;
    pattern.data_bits.etherType = 0xeeee;
    for (idx = 0; idx <= APOLLO_CF_PATTERN_MAX; idx++)
    {
        if (idx >= 128)
        {
            pattern.care_bits.etherType = 0;
            pattern.data_bits.etherType = 0;
        }
        osal_memset(&patternRd,0,sizeof(apollo_raw_cf_RuleEntry_t));
        pattern.idx = idx;
        patternRd.idx = idx;
        if (( ret = apollo_raw_cf_rule_get(&patternRd)) == RT_ERR_OK)
        {

#if 0
            osal_printf("idx:%u, Data: %x,%x,%x,%x,%x,%x,%x,%x,%x,\nCare : %x,%x,%x,%x,%x,%x,%x,%x,%x\n",idx,
                patternRd.care_bits.etherType,  patternRd.care_bits.ifCtag ,    patternRd.care_bits.ifStag ,
                patternRd.care_bits.interPri ,    patternRd.care_bits.tagPri , patternRd.care_bits.tagVid, patternRd.care_bits.tos_gemidx ,
                patternRd.care_bits.direction ,patternRd.care_bits.uni ,patternRd.data_bits.etherType, patternRd.data_bits.ifCtag,
                patternRd.data_bits.ifStag , patternRd.data_bits.interPri, patternRd.data_bits.tagPri, patternRd.data_bits.tagVid ,
                patternRd.data_bits.tos_gemidx , patternRd.data_bits.direction , patternRd.data_bits.uni);
#endif

            if (osal_memcmp( &patternRd, &pattern,sizeof(apollo_raw_cf_RuleEntry_t)) )
            {
#if 1
                osal_printf("idx:%u, Data: %x,%x,%x,%x,%x,%x,%x,%x,%x,\nCare : %x,%x,%x,%x,%x,%x,%x,%x,%x\npattern.valid: %x",idx,
                    patternRd.care_bits.etherType,  patternRd.care_bits.ifCtag ,    patternRd.care_bits.ifStag ,
                    patternRd.care_bits.interPri ,    patternRd.care_bits.tagPri , patternRd.care_bits.tagVid, patternRd.care_bits.tos_gemidx ,
                    patternRd.care_bits.direction ,patternRd.care_bits.uni ,patternRd.data_bits.etherType, patternRd.data_bits.ifCtag,
                    patternRd.data_bits.ifStag , patternRd.data_bits.interPri, patternRd.data_bits.tagPri, patternRd.data_bits.tagVid ,
                    patternRd.data_bits.tos_gemidx , patternRd.data_bits.direction , patternRd.data_bits.uni,pattern.valid);
#endif
                ret = RT_ERR_FAILED;
                osal_printf("%s(%u), index:%u , compare error\n",__FUNCTION__, __LINE__, idx);
                return ret;

            }
        }
        else {
            ret = RT_ERR_FAILED;
            osal_printf("%s(%u), index:%u , read error\n",__FUNCTION__, __LINE__, idx);
            return ret;
        }
#if defined(CONFIG_FPGA_TEST)
        if((idx&0x7) == 0x7)
            idx += 120;
#endif
    }
    return RT_ERR_OK;
}

int32 dal_raw_cf_downStreamAll_test(uint32 caseNo)
{
    int32 ret = RT_ERR_FAILED;
    int32 test_port = 0;
    uint32 idx;
    uint32  test_tmp=0;

    apollo_raw_cf_dsAct_t daActCtl;
    apollo_raw_cf_dsAct_t daActCtlRd;

	uint32 index;
	uint32 type,upperValue,lowerValue;
    osal_memset(&daActCtl,0,sizeof(apollo_raw_cf_dsAct_t));
    osal_memset(&daActCtlRd,0,sizeof(apollo_raw_cf_dsAct_t));
    daActCtl.cact = 1;
    daActCtl.cpri_act = 0;
    daActCtl.cfpri = 1;
    daActCtl.cfpri_act = 0;
    daActCtl.cvid_act = 1;
    daActCtl.csact = 0;
    daActCtl.tag_pri = 1;
    daActCtl.tag_vid = 0;
    daActCtl.uni_act = 1;
    daActCtl.uni_mask = 0;
    for (idx = 0; idx <= APOLLO_CF_PATTERN_MAX; idx++)
    {
        daActCtl.idx = idx;
        apollo_raw_cf_dsAct_set(&daActCtl);
    }

    for (idx = 0; idx <= APOLLO_CF_PATTERN_MAX; idx++)
    {
        daActCtl.idx = idx;
        daActCtlRd.idx = idx;
        if (( ret = apollo_raw_cf_dsAct_get(&daActCtlRd)) == RT_ERR_OK)
        {

            if (osal_memcmp( &daActCtl, &daActCtlRd, sizeof(apollo_raw_cf_dsAct_t)))
            {
                osal_printf("%s(%u), index:%u , compare error\n", __FUNCTION__, __LINE__, idx);
                ret = RT_ERR_FAILED;
                return ret;
            }
        }
        else {
            osal_printf("%s(%u), index:%u , read error\n",__FUNCTION__, __LINE__, idx);
            ret = RT_ERR_FAILED;
            return ret;
        }
    }
    osal_memset(&daActCtl,0,sizeof(apollo_raw_cf_dsAct_t));
    osal_memset(&daActCtlRd,0,sizeof(apollo_raw_cf_dsAct_t));
    daActCtl.cact = 0x3;
    daActCtl.cpri_act = 0x3;
    daActCtl.cfpri = 0x7;
    daActCtl.cfpri_act = 0x1;
    daActCtl.cvid_act = 0x3;
    daActCtl.csact = 0x3;
    daActCtl.tag_pri = 0x7;
    daActCtl.tag_vid = 0xfff;
    daActCtl.uni_act = 0x1;
    daActCtl.uni_mask = 0x3f;
    for (idx = 0; idx <= APOLLO_CF_PATTERN_MAX; idx++)
    {
        daActCtl.idx = idx;
        if ((ret = apollo_raw_cf_dsAct_set(&daActCtl)) != RT_ERR_OK){
            osal_printf("%s(%u), index:%u , write error, ret=%d\n",__FUNCTION__, __LINE__, idx, ret);
            ret = RT_ERR_FAILED;
            return ret;
        }
    }

    for (idx = 0; idx <= APOLLO_CF_PATTERN_MAX; idx++)
    {
        daActCtl.idx = idx;
        daActCtlRd.idx = idx;
        if (( ret = apollo_raw_cf_dsAct_get(&daActCtlRd)) == RT_ERR_OK)
        {

            if (osal_memcmp( &daActCtl, &daActCtlRd, sizeof(apollo_raw_cf_dsAct_t)))
            {
                osal_printf("%s(%u), index:%u , compare error\n", __FUNCTION__, __LINE__, idx);
                ret = RT_ERR_FAILED;
                return ret;
            }
        }
        else {
            osal_printf("%s(%u), index:%u , read error\n",__FUNCTION__, __LINE__, idx);
            ret = RT_ERR_FAILED;
            return ret;
        }
    }
    return RT_ERR_OK;
}

int32 dal_raw_cf_upStreamAll_test(uint32 caseNo)
{
    int32 ret = RT_ERR_FAILED;
    int32 test_port = 0;
    uint32 idx;
    uint32  test_tmp=0;

    apollo_raw_cf_usAct_t upActCtl;
    apollo_raw_cf_usAct_t upActCtlRd;

	uint32 index;
	uint32 type,upperValue,lowerValue;

    osal_memset(&upActCtl,0,sizeof(apollo_raw_cf_usAct_t));
    osal_memset(&upActCtlRd,0,sizeof(apollo_raw_cf_usAct_t));

    upActCtl.assign_idx = 1;
    upActCtl.cact = 0;
    upActCtl.csact = 1;
    upActCtl.tag_pri= 0;
    upActCtl.cspri_act = 1;
    upActCtl.tag_vid = 0;
    upActCtl.csvid_act = 1;
    upActCtl.sid_act = 0;
    for (idx = 0; idx <= APOLLO_CF_PATTERN_MAX; idx++)
    {
        upActCtl.idx = idx;
        apollo_raw_cf_usAct_set(&upActCtl);
    }

    for (idx = 0; idx <= APOLLO_CF_PATTERN_MAX; idx++)
    {
        upActCtl.idx = idx;
        upActCtlRd.idx = idx;
        if (( ret = apollo_raw_cf_usAct_get(&upActCtlRd)) == RT_ERR_OK)
        {

            if (osal_memcmp( &upActCtl, &upActCtlRd, sizeof(apollo_raw_cf_usAct_t)))
            {
                osal_printf("%s(%u), index:%u , compare error\n", __FUNCTION__, __LINE__, idx);
                ret = RT_ERR_FAILED;
                return ret;
            }
        }
        else {
            osal_printf("%s(%u), index:%u , read error\n", __FUNCTION__, __LINE__, idx);
            ret = RT_ERR_FAILED;
            return ret;
        }
    }

    osal_memset(&upActCtl,0,sizeof(apollo_raw_cf_usAct_t));
    osal_memset(&upActCtlRd,0,sizeof(apollo_raw_cf_usAct_t));

    upActCtl.assign_idx = 0x3f;
    upActCtl.cact = 0x3;
    upActCtl.csact = 0x2;
    upActCtl.tag_pri= 0x7;
    upActCtl.cspri_act = 0x2;
    upActCtl.tag_vid = 0xfff;
    upActCtl.csvid_act = 0x1;
    upActCtl.sid_act = 0x1;
    for (idx = 0; idx <= APOLLO_CF_PATTERN_MAX; idx++)
    {
        upActCtl.idx = idx;
        apollo_raw_cf_usAct_set(&upActCtl);
    }

    for (idx = 0; idx <= APOLLO_CF_PATTERN_MAX; idx++)
    {
        upActCtl.idx = idx;
        upActCtlRd.idx = idx;
        if (( ret = apollo_raw_cf_usAct_get(&upActCtlRd)) == RT_ERR_OK)
        {

            if (osal_memcmp( &upActCtl, &upActCtlRd, sizeof(apollo_raw_cf_usAct_t)))
            {
                osal_printf("%s(%u), index:%u , compare error\n", __FUNCTION__, __LINE__, idx);
                ret = RT_ERR_FAILED;
                break;
            }
        }
        else {
            osal_printf("%s(%u), index:%u , read error\n", __FUNCTION__, __LINE__, idx);
            ret = RT_ERR_FAILED;
            return ret;
        }
    }
    return RT_ERR_OK;
}

int32 dal_raw_cf_rule_test(uint32 caseNo)
{
    int32 ret = RT_ERR_FAILED;
    int32 test_port = 0;
    uint32 idx;
    uint32  test_tmp=0;

    apollo_raw_cf_RuleEntry_t pattern;
    apollo_raw_cf_RuleEntry_t patternRd;

	uint32 index;
	uint32 type,upperValue,lowerValue;

    osal_memset(&pattern,0,sizeof(apollo_raw_cf_RuleEntry_t));
    osal_memset(&patternRd,0,sizeof(apollo_raw_cf_RuleEntry_t));
#if 1

    pattern.care_bits.etherType = 0xffff;
    pattern.care_bits.ifCtag = 1;
    pattern.care_bits.ifStag = 1;
    pattern.care_bits.interPri = 0x7;
    pattern.care_bits.tagPri = 0x7;
    pattern.care_bits.tagVid = 0xfff;
    pattern.care_bits.tos_gemidx = 0xff;
    pattern.care_bits.direction = 1;
    pattern.care_bits.uni = 0x7;
    pattern.data_bits.etherType = 0xffff;
    pattern.data_bits.ifCtag = 1;
    pattern.data_bits.ifStag = 1;
    pattern.data_bits.interPri = 0x7;
    pattern.data_bits.tagPri = 0x7;
    pattern.data_bits.tagVid = 0xfff;
    pattern.data_bits.tos_gemidx = 0xff;
    pattern.data_bits.direction = 1;
    pattern.data_bits.uni = 0x7;
#else
    pattern.care_bits.etherType = 0x5555;
    pattern.care_bits.ifCtag = 1;
    pattern.care_bits.ifStag = 0;
    pattern.care_bits.interPri = 0x5;
    pattern.care_bits.tagPri = 0x5;
    pattern.care_bits.tagVid = 0x555;
    pattern.care_bits.tos_gemidx = 0x55;
    pattern.care_bits.direction = 1;
    pattern.care_bits.uni = 0x5;
    pattern.data_bits.etherType = 0xeeee;
    pattern.data_bits.ifCtag = 0;
    pattern.data_bits.ifStag = 1;
    pattern.data_bits.interPri = 0x3;
    pattern.data_bits.tagPri = 0x7;
    pattern.data_bits.tagVid = 0xeee;
    pattern.data_bits.tos_gemidx = 0xee;
    pattern.data_bits.direction = 1;
    pattern.data_bits.uni = 0x5;
#endif
    idx = 8;
    pattern.idx = idx;
    apollo_raw_cf_rule_set(&pattern);


    {
        pattern.idx = idx;
        patternRd.idx = idx;
        if (( ret = apollo_raw_cf_rule_get(&patternRd)) == RT_ERR_OK)
        {

#if 0
            osal_printf("Data: %x,%x,%x,%x,%x,%x,%x,%x,%x,\nCare : %x,%x,%x,%x,%x,%x,%x,%x,%x\n",
                patternRd.care_bits.etherType,  patternRd.care_bits.ifCtag ,    patternRd.care_bits.ifStag ,
                patternRd.care_bits.interPri ,    patternRd.care_bits.tagPri , patternRd.care_bits.tagVid, patternRd.care_bits.tos_gemidx ,
                patternRd.care_bits.direction ,patternRd.care_bits.uni ,patternRd.data_bits.etherType, patternRd.data_bits.ifCtag,
                patternRd.data_bits.ifStag , patternRd.data_bits.interPri, patternRd.data_bits.tagPri, patternRd.data_bits.tagVid ,
                patternRd.data_bits.tos_gemidx , patternRd.data_bits.direction , patternRd.data_bits.uni);
#endif

            if (osal_memcmp( &patternRd, &pattern,sizeof(apollo_raw_cf_RuleEntry_t)) )
            {
#if 0
                osal_printf("Data: %x,%x,%x,%x,%x,%x,%x,%x,%x,\nCare : %x,%x,%x,%x,%x,%x,%x,%x,%x\n",
                    patternRd.care_bits.etherType,  patternRd.care_bits.ifCtag ,    patternRd.care_bits.ifStag ,
                    patternRd.care_bits.interPri ,    patternRd.care_bits.tagPri , patternRd.care_bits.tagVid, patternRd.care_bits.tos_gemidx ,
                    patternRd.care_bits.direction ,patternRd.care_bits.uni ,patternRd.data_bits.etherType, patternRd.data_bits.ifCtag,
                    patternRd.data_bits.ifStag , patternRd.data_bits.interPri, patternRd.data_bits.tagPri, patternRd.data_bits.tagVid ,
                    patternRd.data_bits.tos_gemidx , patternRd.data_bits.direction , patternRd.data_bits.uni);
#endif
                ret = RT_ERR_FAILED;
                osal_printf("%s(%u), compare error\n", __FUNCTION__, __LINE__);
                return ret;

            }
        }
        else {
            ret = RT_ERR_FAILED;
            osal_printf("%s(%u), read error\n", __FUNCTION__, __LINE__);
            return ret;
        }
    }
    return RT_ERR_OK;
}

int32 dal_raw_cf_cfg_test(uint32 caseNo)
{
    apollo_raw_cf_RuleEntry_t pattern;
    apollo_raw_cf_dsAct_t cfDsAct;
    apollo_raw_cf_usAct_t cfUsAct;

    uint32 regData;
    uint32 wData;
    uint32 dataMask;
    uint32 checkData;
    uint32 index;

    /*error input check*/
    /*out of range*/
    if( apollo_raw_cf_usPermit_set(APOLLO_RAW_CF_US_PERMIT_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( apollo_raw_cf_valid_set(APOLLO_CF_PATTERN_MAX + 1, ENABLED) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( apollo_raw_cf_valid_set(APOLLO_CF_PATTERN_MAX, RTK_ENABLE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&pattern, 0, sizeof(apollo_raw_cf_RuleEntry_t));
    pattern.care_bits.etherType = 0xffff;
    pattern.care_bits.ifCtag = 1;
    pattern.care_bits.ifStag = 1;
    pattern.care_bits.interPri = 0x7;
    pattern.care_bits.tagPri = 0x7;
    pattern.care_bits.tagVid = 0xfff;
    pattern.care_bits.tos_gemidx = 0xff;
    pattern.care_bits.direction = 1;
    pattern.care_bits.uni = 0x7;
    pattern.data_bits.etherType = 0xffff;
    pattern.data_bits.ifCtag = 1;
    pattern.data_bits.ifStag = 1;
    pattern.data_bits.interPri = 0x7;
    pattern.data_bits.tagPri = 0x7;
    pattern.data_bits.tagVid = 0xfff;
    pattern.data_bits.tos_gemidx = 0xff;
    pattern.data_bits.direction = 1;
    pattern.data_bits.uni = 0x7;
    pattern.idx = APOLLO_CF_PATTERN_MAX +1;
    pattern.valid = 1;

    if( apollo_raw_cf_rule_set(&pattern) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&pattern, 0, sizeof(apollo_raw_cf_RuleEntry_t));
    pattern.care_bits.etherType = 0xffff;
    pattern.care_bits.ifCtag = 1;
    pattern.care_bits.ifStag = 1;
    pattern.care_bits.interPri = 0x7;
    pattern.care_bits.tagPri = 0x7;
    pattern.care_bits.tagVid = 0xfff;
    pattern.care_bits.tos_gemidx = 0xff;
    pattern.care_bits.direction = 1;
    pattern.care_bits.uni = 0x7;
    pattern.data_bits.etherType = 0xffff;
    pattern.data_bits.ifCtag = 1;
    pattern.data_bits.ifStag = 1;
    pattern.data_bits.interPri = 0x7;
    pattern.data_bits.tagPri = 0x7;
    pattern.data_bits.tagVid = 0xfff;
    pattern.data_bits.tos_gemidx = 0xff;
    pattern.data_bits.direction = 1;
    pattern.data_bits.uni = 0x7;
    pattern.idx = APOLLO_CF_PATTERN_MAX;
    pattern.valid = 2;

    if( apollo_raw_cf_rule_set(&pattern) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&pattern, 0, sizeof(apollo_raw_cf_RuleEntry_t));
    pattern.care_bits.etherType = 0xffff;
    pattern.care_bits.ifCtag = 1;
    pattern.care_bits.ifStag = 1;
    pattern.care_bits.interPri = 0x7;
    pattern.care_bits.tagPri = 0x7;
    pattern.care_bits.tagVid = 0xfff;
    pattern.care_bits.tos_gemidx = 0xff;
    pattern.care_bits.direction = 1;
    pattern.care_bits.uni = 0x7;
    pattern.data_bits.etherType = 0xffff;
    pattern.data_bits.ifCtag = 1;
    pattern.data_bits.ifStag = 1;
    pattern.data_bits.interPri = 0x7;
    pattern.data_bits.tagPri = 0x7;
    pattern.data_bits.tagVid = 0xfff;
    pattern.data_bits.tos_gemidx = 0xff;
    pattern.data_bits.direction = 1;
    pattern.data_bits.uni = 0x8;
    pattern.idx = APOLLO_CF_PATTERN_MAX;
    pattern.valid = 1;

    if( apollo_raw_cf_rule_set(&pattern) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&pattern, 0, sizeof(apollo_raw_cf_RuleEntry_t));
    pattern.care_bits.etherType = 0xffff;
    pattern.care_bits.ifCtag = 1;
    pattern.care_bits.ifStag = 1;
    pattern.care_bits.interPri = 0x7;
    pattern.care_bits.tagPri = 0x7;
    pattern.care_bits.tagVid = 0xfff;
    pattern.care_bits.tos_gemidx = 0xff;
    pattern.care_bits.direction = 1;
    pattern.care_bits.uni = 0x7;
    pattern.data_bits.etherType = 0xffff;
    pattern.data_bits.ifCtag = 1;
    pattern.data_bits.ifStag = 1;
    pattern.data_bits.interPri = 0x7;
    pattern.data_bits.tagPri = 0x7;
    pattern.data_bits.tagVid = 0xfff;
    pattern.data_bits.tos_gemidx = 0xff;
    pattern.data_bits.direction = 2;
    pattern.data_bits.uni = 0x7;
    pattern.idx = APOLLO_CF_PATTERN_MAX;
    pattern.valid = 1;

    if( apollo_raw_cf_rule_set(&pattern) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    osal_memset(&pattern, 0, sizeof(apollo_raw_cf_RuleEntry_t));
    pattern.care_bits.etherType = 0xffff;
    pattern.care_bits.ifCtag = 1;
    pattern.care_bits.ifStag = 1;
    pattern.care_bits.interPri = 0x7;
    pattern.care_bits.tagPri = 0x7;
    pattern.care_bits.tagVid = 0xfff;
    pattern.care_bits.tos_gemidx = 0xff;
    pattern.care_bits.direction = 1;
    pattern.care_bits.uni = 0x7;
    pattern.data_bits.etherType = 0xffff;
    pattern.data_bits.ifCtag = 1;
    pattern.data_bits.ifStag = 1;
    pattern.data_bits.interPri = 0x7;
    pattern.data_bits.tagPri = 0x7;
    pattern.data_bits.tagVid = 0xfff;
    pattern.data_bits.tos_gemidx = 0x100;
    pattern.data_bits.direction = 1;
    pattern.data_bits.uni = 0x7;
    pattern.idx = APOLLO_CF_PATTERN_MAX;
    pattern.valid = 1;

    if( apollo_raw_cf_rule_set(&pattern) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    osal_memset(&pattern, 0, sizeof(apollo_raw_cf_RuleEntry_t));
    pattern.care_bits.etherType = 0xffff;
    pattern.care_bits.ifCtag = 1;
    pattern.care_bits.ifStag = 1;
    pattern.care_bits.interPri = 0x7;
    pattern.care_bits.tagPri = 0x7;
    pattern.care_bits.tagVid = 0xfff;
    pattern.care_bits.tos_gemidx = 0xff;
    pattern.care_bits.direction = 1;
    pattern.care_bits.uni = 0x7;
    pattern.data_bits.etherType = 0xffff;
    pattern.data_bits.ifCtag = 1;
    pattern.data_bits.ifStag = 1;
    pattern.data_bits.interPri = 0x7;
    pattern.data_bits.tagPri = 0x7;
    pattern.data_bits.tagVid = 0x1000;
    pattern.data_bits.tos_gemidx = 0xff;
    pattern.data_bits.direction = 1;
    pattern.data_bits.uni = 0x7;
    pattern.idx = APOLLO_CF_PATTERN_MAX;
    pattern.valid = 1;

    if( apollo_raw_cf_rule_set(&pattern) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    osal_memset(&pattern, 0, sizeof(apollo_raw_cf_RuleEntry_t));
    pattern.care_bits.etherType = 0xffff;
    pattern.care_bits.ifCtag = 1;
    pattern.care_bits.ifStag = 1;
    pattern.care_bits.interPri = 0x7;
    pattern.care_bits.tagPri = 0x7;
    pattern.care_bits.tagVid = 0xfff;
    pattern.care_bits.tos_gemidx = 0xff;
    pattern.care_bits.direction = 1;
    pattern.care_bits.uni = 0x7;
    pattern.data_bits.etherType = 0xffff;
    pattern.data_bits.ifCtag = 1;
    pattern.data_bits.ifStag = 1;
    pattern.data_bits.interPri = 0x7;
    pattern.data_bits.tagPri = 0x8;
    pattern.data_bits.tagVid = 0xfff;
    pattern.data_bits.tos_gemidx = 0xff;
    pattern.data_bits.direction = 1;
    pattern.data_bits.uni = 0x7;
    pattern.idx = APOLLO_CF_PATTERN_MAX;
    pattern.valid = 1;

    if( apollo_raw_cf_rule_set(&pattern) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    osal_memset(&pattern, 0, sizeof(apollo_raw_cf_RuleEntry_t));
    pattern.care_bits.etherType = 0xffff;
    pattern.care_bits.ifCtag = 1;
    pattern.care_bits.ifStag = 1;
    pattern.care_bits.interPri = 0x7;
    pattern.care_bits.tagPri = 0x7;
    pattern.care_bits.tagVid = 0xfff;
    pattern.care_bits.tos_gemidx = 0xff;
    pattern.care_bits.direction = 1;
    pattern.care_bits.uni = 0x7;
    pattern.data_bits.etherType = 0xffff;
    pattern.data_bits.ifCtag = 1;
    pattern.data_bits.ifStag = 1;
    pattern.data_bits.interPri = 0x8;
    pattern.data_bits.tagPri = 0x7;
    pattern.data_bits.tagVid = 0xfff;
    pattern.data_bits.tos_gemidx = 0xff;
    pattern.data_bits.direction = 1;
    pattern.data_bits.uni = 0x7;
    pattern.idx = APOLLO_CF_PATTERN_MAX;
    pattern.valid = 0;

    if( apollo_raw_cf_rule_set(&pattern) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    osal_memset(&pattern, 0, sizeof(apollo_raw_cf_RuleEntry_t));
    pattern.care_bits.etherType = 0xffff;
    pattern.care_bits.ifCtag = 1;
    pattern.care_bits.ifStag = 1;
    pattern.care_bits.interPri = 0x7;
    pattern.care_bits.tagPri = 0x7;
    pattern.care_bits.tagVid = 0xfff;
    pattern.care_bits.tos_gemidx = 0xff;
    pattern.care_bits.direction = 1;
    pattern.care_bits.uni = 0x7;
    pattern.data_bits.etherType = 0xffff;
    pattern.data_bits.ifCtag = 1;
    pattern.data_bits.ifStag = 2;
    pattern.data_bits.interPri = 0x7;
    pattern.data_bits.tagPri = 0x7;
    pattern.data_bits.tagVid = 0xfff;
    pattern.data_bits.tos_gemidx = 0xff;
    pattern.data_bits.direction = 1;
    pattern.data_bits.uni = 0x7;
    pattern.idx = APOLLO_CF_PATTERN_MAX;
    pattern.valid = 1;

    if( apollo_raw_cf_rule_set(&pattern) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    osal_memset(&pattern, 0, sizeof(apollo_raw_cf_RuleEntry_t));
    pattern.care_bits.etherType = 0xffff;
    pattern.care_bits.ifCtag = 1;
    pattern.care_bits.ifStag = 1;
    pattern.care_bits.interPri = 0x7;
    pattern.care_bits.tagPri = 0x7;
    pattern.care_bits.tagVid = 0xfff;
    pattern.care_bits.tos_gemidx = 0xff;
    pattern.care_bits.direction = 1;
    pattern.care_bits.uni = 0x7;
    pattern.data_bits.etherType = 0xffff;
    pattern.data_bits.ifCtag = 2;
    pattern.data_bits.ifStag = 1;
    pattern.data_bits.interPri = 0x7;
    pattern.data_bits.tagPri = 0x7;
    pattern.data_bits.tagVid = 0xfff;
    pattern.data_bits.tos_gemidx = 0xff;
    pattern.data_bits.direction = 1;
    pattern.data_bits.uni = 0x7;
    pattern.idx = APOLLO_CF_PATTERN_MAX;
    pattern.valid = 0;

    if( apollo_raw_cf_rule_set(&pattern) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    osal_memset(&pattern, 0, sizeof(apollo_raw_cf_RuleEntry_t));
    pattern.care_bits.etherType = 0xffff;
    pattern.care_bits.ifCtag = 1;
    pattern.care_bits.ifStag = 1;
    pattern.care_bits.interPri = 0x7;
    pattern.care_bits.tagPri = 0x7;
    pattern.care_bits.tagVid = 0xfff;
    pattern.care_bits.tos_gemidx = 0xff;
    pattern.care_bits.direction = 1;
    pattern.care_bits.uni = 0x7;
    pattern.data_bits.etherType = 0x10000;
    pattern.data_bits.ifCtag = 1;
    pattern.data_bits.ifStag = 1;
    pattern.data_bits.interPri = 0x7;
    pattern.data_bits.tagPri = 0x7;
    pattern.data_bits.tagVid = 0xfff;
    pattern.data_bits.tos_gemidx = 0xff;
    pattern.data_bits.direction = 1;
    pattern.data_bits.uni = 0x7;
    pattern.idx = APOLLO_CF_PATTERN_MAX;
    pattern.valid = 1;

    if( apollo_raw_cf_rule_set(&pattern) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    osal_memset(&pattern, 0, sizeof(apollo_raw_cf_RuleEntry_t));
    pattern.care_bits.etherType = 0xffff;
    pattern.care_bits.ifCtag = 1;
    pattern.care_bits.ifStag = 1;
    pattern.care_bits.interPri = 0x7;
    pattern.care_bits.tagPri = 0x7;
    pattern.care_bits.tagVid = 0xfff;
    pattern.care_bits.tos_gemidx = 0xff;
    pattern.care_bits.direction = 1;
    pattern.care_bits.uni = 0x8;
    pattern.data_bits.etherType = 0xffff;
    pattern.data_bits.ifCtag = 1;
    pattern.data_bits.ifStag = 1;
    pattern.data_bits.interPri = 0x7;
    pattern.data_bits.tagPri = 0x7;
    pattern.data_bits.tagVid = 0xfff;
    pattern.data_bits.tos_gemidx = 0xff;
    pattern.data_bits.direction = 1;
    pattern.data_bits.uni = 0x7;
    pattern.idx = APOLLO_CF_PATTERN_MAX;
    pattern.valid = 1;

    if( apollo_raw_cf_rule_set(&pattern) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    osal_memset(&pattern, 0, sizeof(apollo_raw_cf_RuleEntry_t));
    pattern.care_bits.etherType = 0xffff;
    pattern.care_bits.ifCtag = 1;
    pattern.care_bits.ifStag = 1;
    pattern.care_bits.interPri = 0x7;
    pattern.care_bits.tagPri = 0x7;
    pattern.care_bits.tagVid = 0xfff;
    pattern.care_bits.tos_gemidx = 0xff;
    pattern.care_bits.direction = 2;
    pattern.care_bits.uni = 0x7;
    pattern.data_bits.etherType = 0xffff;
    pattern.data_bits.ifCtag = 1;
    pattern.data_bits.ifStag = 1;
    pattern.data_bits.interPri = 0x7;
    pattern.data_bits.tagPri = 0x7;
    pattern.data_bits.tagVid = 0xfff;
    pattern.data_bits.tos_gemidx = 0xff;
    pattern.data_bits.direction = 1;
    pattern.data_bits.uni = 0x7;
    pattern.idx = APOLLO_CF_PATTERN_MAX;
    pattern.valid = 1;

    if( apollo_raw_cf_rule_set(&pattern) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    osal_memset(&pattern, 0, sizeof(apollo_raw_cf_RuleEntry_t));
    pattern.care_bits.etherType = 0xffff;
    pattern.care_bits.ifCtag = 1;
    pattern.care_bits.ifStag = 1;
    pattern.care_bits.interPri = 0x7;
    pattern.care_bits.tagPri = 0x7;
    pattern.care_bits.tagVid = 0xfff;
    pattern.care_bits.tos_gemidx = 0x100;
    pattern.care_bits.direction = 1;
    pattern.care_bits.uni = 0x7;
    pattern.data_bits.etherType = 0xffff;
    pattern.data_bits.ifCtag = 1;
    pattern.data_bits.ifStag = 1;
    pattern.data_bits.interPri = 0x7;
    pattern.data_bits.tagPri = 0x7;
    pattern.data_bits.tagVid = 0xfff;
    pattern.data_bits.tos_gemidx = 0xff;
    pattern.data_bits.direction = 1;
    pattern.data_bits.uni = 0x7;
    pattern.idx = APOLLO_CF_PATTERN_MAX;
    pattern.valid = 1;

    if( apollo_raw_cf_rule_set(&pattern) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    osal_memset(&pattern, 0, sizeof(apollo_raw_cf_RuleEntry_t));
    pattern.care_bits.etherType = 0xffff;
    pattern.care_bits.ifCtag = 1;
    pattern.care_bits.ifStag = 1;
    pattern.care_bits.interPri = 0x7;
    pattern.care_bits.tagPri = 0x7;
    pattern.care_bits.tagVid = 0x1000;
    pattern.care_bits.tos_gemidx = 0xff;
    pattern.care_bits.direction = 1;
    pattern.care_bits.uni = 0x7;
    pattern.data_bits.etherType = 0xffff;
    pattern.data_bits.ifCtag = 1;
    pattern.data_bits.ifStag = 1;
    pattern.data_bits.interPri = 0x7;
    pattern.data_bits.tagPri = 0x7;
    pattern.data_bits.tagVid = 0xfff;
    pattern.data_bits.tos_gemidx = 0xff;
    pattern.data_bits.direction = 1;
    pattern.data_bits.uni = 0x7;
    pattern.idx = APOLLO_CF_PATTERN_MAX;
    pattern.valid = 1;

    if( apollo_raw_cf_rule_set(&pattern) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&pattern, 0, sizeof(apollo_raw_cf_RuleEntry_t));
    pattern.care_bits.etherType = 0xffff;
    pattern.care_bits.ifCtag = 1;
    pattern.care_bits.ifStag = 1;
    pattern.care_bits.interPri = 0x7;
    pattern.care_bits.tagPri = 0x8;
    pattern.care_bits.tagVid = 0xfff;
    pattern.care_bits.tos_gemidx = 0xff;
    pattern.care_bits.direction = 1;
    pattern.care_bits.uni = 0x7;
    pattern.data_bits.etherType = 0xffff;
    pattern.data_bits.ifCtag = 1;
    pattern.data_bits.ifStag = 1;
    pattern.data_bits.interPri = 0x7;
    pattern.data_bits.tagPri = 0x7;
    pattern.data_bits.tagVid = 0xfff;
    pattern.data_bits.tos_gemidx = 0xff;
    pattern.data_bits.direction = 1;
    pattern.data_bits.uni = 0x7;
    pattern.idx = APOLLO_CF_PATTERN_MAX;
    pattern.valid = 1;

    if( apollo_raw_cf_rule_set(&pattern) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
        osal_memset(&pattern, 0, sizeof(apollo_raw_cf_RuleEntry_t));
    pattern.care_bits.etherType = 0xffff;
    pattern.care_bits.ifCtag = 1;
    pattern.care_bits.ifStag = 1;
    pattern.care_bits.interPri = 0x8;
    pattern.care_bits.tagPri = 0x7;
    pattern.care_bits.tagVid = 0xfff;
    pattern.care_bits.tos_gemidx = 0xff;
    pattern.care_bits.direction = 1;
    pattern.care_bits.uni = 0x7;
    pattern.data_bits.etherType = 0xffff;
    pattern.data_bits.ifCtag = 1;
    pattern.data_bits.ifStag = 1;
    pattern.data_bits.interPri = 0x7;
    pattern.data_bits.tagPri = 0x7;
    pattern.data_bits.tagVid = 0xfff;
    pattern.data_bits.tos_gemidx = 0xff;
    pattern.data_bits.direction = 1;
    pattern.data_bits.uni = 0x7;
    pattern.idx = APOLLO_CF_PATTERN_MAX;
    pattern.valid = 1;

    if( apollo_raw_cf_rule_set(&pattern) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
        osal_memset(&pattern, 0, sizeof(apollo_raw_cf_RuleEntry_t));
    pattern.care_bits.etherType = 0xffff;
    pattern.care_bits.ifCtag = 1;
    pattern.care_bits.ifStag = 2;
    pattern.care_bits.interPri = 0x7;
    pattern.care_bits.tagPri = 0x7;
    pattern.care_bits.tagVid = 0xfff;
    pattern.care_bits.tos_gemidx = 0xff;
    pattern.care_bits.direction = 1;
    pattern.care_bits.uni = 0x7;
    pattern.data_bits.etherType = 0xffff;
    pattern.data_bits.ifCtag = 1;
    pattern.data_bits.ifStag = 1;
    pattern.data_bits.interPri = 0x7;
    pattern.data_bits.tagPri = 0x7;
    pattern.data_bits.tagVid = 0xfff;
    pattern.data_bits.tos_gemidx = 0xff;
    pattern.data_bits.direction = 1;
    pattern.data_bits.uni = 0x7;
    pattern.idx = APOLLO_CF_PATTERN_MAX;
    pattern.valid = 1;

    if( apollo_raw_cf_rule_set(&pattern) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
        osal_memset(&pattern, 0, sizeof(apollo_raw_cf_RuleEntry_t));
    pattern.care_bits.etherType = 0xffff;
    pattern.care_bits.ifCtag = 2;
    pattern.care_bits.ifStag = 1;
    pattern.care_bits.interPri = 0x7;
    pattern.care_bits.tagPri = 0x7;
    pattern.care_bits.tagVid = 0xfff;
    pattern.care_bits.tos_gemidx = 0xff;
    pattern.care_bits.direction = 1;
    pattern.care_bits.uni = 0x7;
    pattern.data_bits.etherType = 0xffff;
    pattern.data_bits.ifCtag = 1;
    pattern.data_bits.ifStag = 1;
    pattern.data_bits.interPri = 0x7;
    pattern.data_bits.tagPri = 0x7;
    pattern.data_bits.tagVid = 0xfff;
    pattern.data_bits.tos_gemidx = 0xff;
    pattern.data_bits.direction = 1;
    pattern.data_bits.uni = 0x7;
    pattern.idx = APOLLO_CF_PATTERN_MAX;
    pattern.valid = 1;

    if( apollo_raw_cf_rule_set(&pattern) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    osal_memset(&pattern, 0, sizeof(apollo_raw_cf_RuleEntry_t));
    pattern.care_bits.etherType = 0x10000;
    pattern.care_bits.ifCtag = 1;
    pattern.care_bits.ifStag = 1;
    pattern.care_bits.interPri = 0x7;
    pattern.care_bits.tagPri = 0x7;
    pattern.care_bits.tagVid = 0xfff;
    pattern.care_bits.tos_gemidx = 0xff;
    pattern.care_bits.direction = 1;
    pattern.care_bits.uni = 0x7;
    pattern.data_bits.etherType = 0xffff;
    pattern.data_bits.ifCtag = 1;
    pattern.data_bits.ifStag = 1;
    pattern.data_bits.interPri = 0x7;
    pattern.data_bits.tagPri = 0x7;
    pattern.data_bits.tagVid = 0xfff;
    pattern.data_bits.tos_gemidx = 0xff;
    pattern.data_bits.direction = 1;
    pattern.data_bits.uni = 0x7;
    pattern.idx = APOLLO_CF_PATTERN_MAX;
    pattern.valid = 1;

    if( apollo_raw_cf_rule_set(&pattern) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }



    osal_memset(&cfDsAct, 0, sizeof(apollo_raw_cf_dsAct_t));
    cfDsAct.idx = APOLLO_CF_PATTERN_MAX + 1;
    cfDsAct.cact = 0x3;
    cfDsAct.cpri_act = 0x3;
    cfDsAct.cfpri = 0x7;
    cfDsAct.cfpri_act = 0x1;
    cfDsAct.cvid_act = 0x3;
    cfDsAct.csact = 0x3;
    cfDsAct.tag_pri = 0x7;
    cfDsAct.tag_vid = 0xffff;
    cfDsAct.uni_act = 0x3;
    cfDsAct.uni_mask = 0x3f;
    if( apollo_raw_cf_dsAct_set(&cfDsAct) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&cfDsAct, 0, sizeof(apollo_raw_cf_dsAct_t));
    cfDsAct.idx = APOLLO_CF_PATTERN_MAX;
    cfDsAct.cact = 0x4;
    cfDsAct.cpri_act = 0x3;
    cfDsAct.cfpri = 0x7;
    cfDsAct.cfpri_act = 0x1;
    cfDsAct.cvid_act = 0x3;
    cfDsAct.csact = 0x3;
    cfDsAct.tag_pri = 0x7;
    cfDsAct.tag_vid = 0xffff;
    cfDsAct.uni_act = 0x3;
    cfDsAct.uni_mask = 0x3f;
    if( apollo_raw_cf_dsAct_set(&cfDsAct) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&cfDsAct, 0, sizeof(apollo_raw_cf_dsAct_t));
    cfDsAct.idx = APOLLO_CF_PATTERN_MAX;
    cfDsAct.cact = 0x3;
    cfDsAct.cpri_act = 0x4;
    cfDsAct.cfpri = 0x7;
    cfDsAct.cfpri_act = 0x1;
    cfDsAct.cvid_act = 0x3;
    cfDsAct.csact = 0x3;
    cfDsAct.tag_pri = 0x7;
    cfDsAct.tag_vid = 0xffff;
    cfDsAct.uni_act = 0x3;
    cfDsAct.uni_mask = 0x3f;
    if( apollo_raw_cf_dsAct_set(&cfDsAct) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&cfDsAct, 0, sizeof(apollo_raw_cf_dsAct_t));
    cfDsAct.idx = APOLLO_CF_PATTERN_MAX;
    cfDsAct.cact = 0x3;
    cfDsAct.cpri_act = 0x3;
    cfDsAct.cfpri = 0x8;
    cfDsAct.cfpri_act = 0x1;
    cfDsAct.cvid_act = 0x3;
    cfDsAct.csact = 0x3;
    cfDsAct.tag_pri = 0x7;
    cfDsAct.tag_vid = 0xffff;
    cfDsAct.uni_act = 0x3;
    cfDsAct.uni_mask = 0x3f;
    if( apollo_raw_cf_dsAct_set(&cfDsAct) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&cfDsAct, 0, sizeof(apollo_raw_cf_dsAct_t));
    cfDsAct.idx = APOLLO_CF_PATTERN_MAX;
    cfDsAct.cact = 0x3;
    cfDsAct.cpri_act = 0x3;
    cfDsAct.cfpri = 0x7;
    cfDsAct.cfpri_act = 0x2;
    cfDsAct.cvid_act = 0x3;
    cfDsAct.csact = 0x3;
    cfDsAct.tag_pri = 0x7;
    cfDsAct.tag_vid = 0xffff;
    cfDsAct.uni_act = 0x3;
    cfDsAct.uni_mask = 0x3f;
    if( apollo_raw_cf_dsAct_set(&cfDsAct) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&cfDsAct, 0, sizeof(apollo_raw_cf_dsAct_t));
    cfDsAct.idx = APOLLO_CF_PATTERN_MAX;
    cfDsAct.cact = 0x3;
    cfDsAct.cpri_act = 0x3;
    cfDsAct.cfpri = 0x7;
    cfDsAct.cfpri_act = 0x1;
    cfDsAct.cvid_act = 0x4;
    cfDsAct.csact = 0x3;
    cfDsAct.tag_pri = 0x7;
    cfDsAct.tag_vid = 0xffff;
    cfDsAct.uni_act = 0x3;
    cfDsAct.uni_mask = 0x3f;
    if( apollo_raw_cf_dsAct_set(&cfDsAct) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&cfDsAct, 0, sizeof(apollo_raw_cf_dsAct_t));
    cfDsAct.idx = APOLLO_CF_PATTERN_MAX;
    cfDsAct.cact = 0x3;
    cfDsAct.cpri_act = 0x3;
    cfDsAct.cfpri = 0x7;
    cfDsAct.cfpri_act = 0x1;
    cfDsAct.cvid_act = 0x3;
    cfDsAct.csact = 0x4;
    cfDsAct.tag_pri = 0x7;
    cfDsAct.tag_vid = 0xffff;
    cfDsAct.uni_act = 0x3;
    cfDsAct.uni_mask = 0x3f;
    if( apollo_raw_cf_dsAct_set(&cfDsAct) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&cfDsAct, 0, sizeof(apollo_raw_cf_dsAct_t));
    cfDsAct.idx = APOLLO_CF_PATTERN_MAX;
    cfDsAct.cact = 0x3;
    cfDsAct.cpri_act = 0x3;
    cfDsAct.cfpri = 0x7;
    cfDsAct.cfpri_act = 0x1;
    cfDsAct.cvid_act = 0x3;
    cfDsAct.csact = 0x3;
    cfDsAct.tag_pri = 0x8;
    cfDsAct.tag_vid = 0xffff;
    cfDsAct.uni_act = 0x3;
    cfDsAct.uni_mask = 0x3f;
    if( apollo_raw_cf_dsAct_set(&cfDsAct) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&cfDsAct, 0, sizeof(apollo_raw_cf_dsAct_t));
    cfDsAct.idx = APOLLO_CF_PATTERN_MAX;
    cfDsAct.cact = 0x3;
    cfDsAct.cpri_act = 0x3;
    cfDsAct.cfpri = 0x7;
    cfDsAct.cfpri_act = 0x1;
    cfDsAct.cvid_act = 0x3;
    cfDsAct.csact = 0x3;
    cfDsAct.tag_pri = 0x7;
    cfDsAct.tag_vid = 0x10000;
    cfDsAct.uni_act = 0x3;
    cfDsAct.uni_mask = 0x3f;
    if( apollo_raw_cf_dsAct_set(&cfDsAct) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&cfDsAct, 0, sizeof(apollo_raw_cf_dsAct_t));
    cfDsAct.idx = APOLLO_CF_PATTERN_MAX;
    cfDsAct.cact = 0x3;
    cfDsAct.cpri_act = 0x3;
    cfDsAct.cfpri = 0x7;
    cfDsAct.cfpri_act = 0x1;
    cfDsAct.cvid_act = 0x3;
    cfDsAct.csact = 0x3;
    cfDsAct.tag_pri = 0x7;
    cfDsAct.tag_vid = 0xffff;
    cfDsAct.uni_act = 0x4;
    cfDsAct.uni_mask = 0x3f;
    if( apollo_raw_cf_dsAct_set(&cfDsAct) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    osal_memset(&cfDsAct, 0, sizeof(apollo_raw_cf_dsAct_t));
    cfDsAct.idx = APOLLO_CF_PATTERN_MAX;
    cfDsAct.cact = 0x3;
    cfDsAct.cpri_act = 0x3;
    cfDsAct.cfpri = 0x7;
    cfDsAct.cfpri_act = 0x1;
    cfDsAct.cvid_act = 0x3;
    cfDsAct.csact = 0x3;
    cfDsAct.tag_pri = 0x7;
    cfDsAct.tag_vid = 0xffff;
    cfDsAct.uni_act = 0x3;
    cfDsAct.uni_mask = 0x40;
    if( apollo_raw_cf_dsAct_set(&cfDsAct) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&cfUsAct, 0, sizeof(apollo_raw_cf_usAct_t));
    cfUsAct.idx = APOLLO_CF_PATTERN_MAX + 1;
    cfUsAct.assign_idx = 0x7f;
    cfUsAct.cact = 0x3;
    cfUsAct.csact = 0x3;
    cfUsAct.tag_pri= 0x7;
    cfUsAct.cspri_act = 0x1;
    cfUsAct.tag_vid = 0xfff;
    cfUsAct.csvid_act = 0x3;
    cfUsAct.sid_act = 0x1;

    if( apollo_raw_cf_usAct_set(&cfUsAct) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&cfUsAct, 0, sizeof(apollo_raw_cf_usAct_t));
    cfUsAct.idx = APOLLO_CF_PATTERN_MAX;
    cfUsAct.assign_idx = 0x80;
    cfUsAct.cact = 0x3;
    cfUsAct.csact = 0x3;
    cfUsAct.tag_pri= 0x7;
    cfUsAct.cspri_act = 0x1;
    cfUsAct.tag_vid = 0xfff;
    cfUsAct.csvid_act = 0x3;
    cfUsAct.sid_act = 0x1;

    if( apollo_raw_cf_usAct_set(&cfUsAct) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&cfUsAct, 0, sizeof(apollo_raw_cf_usAct_t));
    cfUsAct.idx = APOLLO_CF_PATTERN_MAX;
    cfUsAct.assign_idx = 0x3f;
    cfUsAct.cact = 0x4;
    cfUsAct.csact = 0x3;
    cfUsAct.tag_pri= 0x7;
    cfUsAct.cspri_act = 0x1;
    cfUsAct.tag_vid = 0xfff;
    cfUsAct.csvid_act = 0x3;
    cfUsAct.sid_act = 0x1;

    if( apollo_raw_cf_usAct_set(&cfUsAct) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&cfUsAct, 0, sizeof(apollo_raw_cf_usAct_t));
    cfUsAct.idx = APOLLO_CF_PATTERN_MAX;
    cfUsAct.assign_idx = 0x3f;
    cfUsAct.cact = 0x3;
    cfUsAct.csact = 0x4;
    cfUsAct.tag_pri= 0x7;
    cfUsAct.cspri_act = 0x1;
    cfUsAct.tag_vid = 0xfff;
    cfUsAct.csvid_act = 0x3;
    cfUsAct.sid_act = 0x1;

    if( apollo_raw_cf_usAct_set(&cfUsAct) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    osal_memset(&cfUsAct, 0, sizeof(apollo_raw_cf_usAct_t));
    cfUsAct.idx = APOLLO_CF_PATTERN_MAX;
    cfUsAct.assign_idx = 0x3f;
    cfUsAct.cact = 0x3;
    cfUsAct.csact = 0x3;
    cfUsAct.tag_pri= 0x10;
    cfUsAct.cspri_act = 0x2;
    cfUsAct.tag_vid = 0xfff;
    cfUsAct.csvid_act = 0x3;
    cfUsAct.sid_act = 0x1;

    if( apollo_raw_cf_usAct_set(&cfUsAct) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    osal_memset(&cfUsAct, 0, sizeof(apollo_raw_cf_usAct_t));
    cfUsAct.idx = APOLLO_CF_PATTERN_MAX;
    cfUsAct.assign_idx = 0x3f;
    cfUsAct.cact = 0x3;
    cfUsAct.csact = 0x3;
    cfUsAct.tag_pri= 0x7;
    cfUsAct.cspri_act = 0x3;
    cfUsAct.tag_vid = 0xfff;
    cfUsAct.csvid_act = 0x3;
    cfUsAct.sid_act = 0x1;

    if( apollo_raw_cf_usAct_set(&cfUsAct) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    osal_memset(&cfUsAct, 0, sizeof(apollo_raw_cf_usAct_t));
    cfUsAct.idx = APOLLO_CF_PATTERN_MAX;
    cfUsAct.assign_idx = 0x3f;
    cfUsAct.cact = 0x3;
    cfUsAct.csact = 0x3;
    cfUsAct.tag_pri= 0x7;
    cfUsAct.cspri_act = 0x1;
    cfUsAct.tag_vid = 0x1000;
    cfUsAct.csvid_act = 0x3;
    cfUsAct.sid_act = 0x1;

    if( apollo_raw_cf_usAct_set(&cfUsAct) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    osal_memset(&cfUsAct, 0, sizeof(apollo_raw_cf_usAct_t));
    cfUsAct.idx = APOLLO_CF_PATTERN_MAX;
    cfUsAct.assign_idx = 0x3f;
    cfUsAct.cact = 0x3;
    cfUsAct.csact = 0x3;
    cfUsAct.tag_pri= 0x7;
    cfUsAct.cspri_act = 0x1;
    cfUsAct.tag_vid = 0xfff;
    cfUsAct.csvid_act = 0x4;
    cfUsAct.sid_act = 0x1;

    if( apollo_raw_cf_usAct_set(&cfUsAct) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    osal_memset(&cfUsAct, 0, sizeof(apollo_raw_cf_usAct_t));
    cfUsAct.idx = APOLLO_CF_PATTERN_MAX;
    cfUsAct.assign_idx = 0x3f;
    cfUsAct.cact = 0x3;
    cfUsAct.csact = 0x3;
    cfUsAct.tag_pri= 0x7;
    cfUsAct.cspri_act = 0x1;
    cfUsAct.tag_vid = 0xfff;
    cfUsAct.csvid_act = 0x3;
    cfUsAct.sid_act = 0x2;

    if( apollo_raw_cf_usAct_set(&cfUsAct) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*null pointer*/
    if( apollo_raw_cf_valid_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_cf_rule_set(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( apollo_raw_cf_rule_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( apollo_raw_cf_dsAct_set(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( apollo_raw_cf_dsAct_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( apollo_raw_cf_usAct_set(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( apollo_raw_cf_dsOp_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( apollo_raw_cf_usOp_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( apollo_raw_cf_usPermit_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*register access test*/

    for (index = 0; index < 16; index++)
        ioal_mem32_write(HAL_GET_REG_ADDR(CF_VALIDr) + index*4, 0x0);
    for (index = 0; index <= APOLLO_CF_PATTERN_MAX; index++)
        apollo_raw_cf_valid_set(index,(index%2));
    for (index =0; index <16; index++)
    {
        ioal_mem32_read(HAL_GET_REG_ADDR(CF_VALIDr) + index*4, &regData);
        checkData =0xaaaaaaaa;
        /*mask out reserve bits*/
        if(regData != checkData)
        {
            osal_printf("\n %s %d, regData=0x%x\n",__FUNCTION__,__LINE__,regData);
            return RT_ERR_FAILED;
        }
    }

    for (index = 0; index < 16; index++)
        ioal_mem32_write(HAL_GET_REG_ADDR(CF_VALIDr) + index*4, 0x0);
    for (index = 0; index <= APOLLO_CF_PATTERN_MAX; index++)
        apollo_raw_cf_valid_set(index,1 - (index%2));
    for (index =0; index <16; index++)
    {
        ioal_mem32_read(HAL_GET_REG_ADDR(CF_VALIDr) + index*4, &regData);
        checkData =0x55555555;
        /*mask out reserve bits*/
        if(regData != checkData)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }

    for (index = 0; index < 16; index++)
        ioal_mem32_write(HAL_GET_REG_ADDR(CF_VALIDr) + index*4, 0x0);
    for (index = 0; index <= APOLLO_CF_PATTERN_MAX; index++)
    {
       apollo_raw_cf_valid_set(index, (index&0x10)>>4);

    }
    for (index =0; index <16; index++)
    {
        ioal_mem32_read(HAL_GET_REG_ADDR(CF_VALIDr) + index*4, &regData);
        checkData =0xffff0000;
        /*mask out reserve bits*/
        if(regData != checkData)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }
    for (index = 0; index < 16; index++)
        ioal_mem32_write(HAL_GET_REG_ADDR(CF_VALIDr) + index*4, 0x0);
    for (index = 0; index <= APOLLO_CF_PATTERN_MAX; index++)
    {
        apollo_raw_cf_valid_set(index, 1-((index&0x10)>>4));
    }
    for (index =0; index <16; index++)
    {
        ioal_mem32_read(HAL_GET_REG_ADDR(CF_VALIDr) + index*4, &regData);
        checkData =0xffff;
        /*mask out reserve bits*/
        if(regData != checkData)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }

    ioal_mem32_write(HAL_GET_REG_ADDR(CF_CFGr), 0x0);
    apollo_raw_cf_usPermit_set(APOLLO_RAW_CF_US_PERMIT_NORMAL);
    ioal_mem32_read(HAL_GET_REG_ADDR(CF_CFGr), &regData);
    checkData =APOLLO_RAW_CF_US_PERMIT_NORMAL;
    dataMask = 0x3;
    checkData =APOLLO_RAW_CF_US_PERMIT_NORMAL;
    /*mask out reserve bits*/
    regData = regData & dataMask;
    if(regData != checkData)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

