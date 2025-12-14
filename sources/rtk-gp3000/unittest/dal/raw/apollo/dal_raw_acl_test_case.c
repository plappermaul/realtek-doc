#include <osal/lib.h>
#include <common/debug/rt_log.h>
#include <dal/apollo/raw/apollo_raw_acl.h>
#include <ioal/mem32.h>

#include <dal/raw/apollo/dal_raw_acl_test_case.h>

#define CONFIG_FPGA_TEST /*for FPGA test*/

/* Define symbol used for test input */
int32 dal_raw_aclRule64_test(uint32 caseNo)
{
    int32 ret = RT_ERR_FAILED;
    uint32 i,j;
    apollo_raw_acl_ruleEntry_t aclRule;
    apollo_raw_acl_ruleEntry_t aclRuleRd;

    apollo_raw_acl_mode_set(APOLLO_ACL_MODE_64ENTRIES);
    osal_memset(&aclRule,0,sizeof(apollo_raw_acl_ruleEntry_t));
	aclRule.care_bits.active_portmsk=0x7f;
	aclRule.care_bits.tag_exist=0x7f;
	aclRule.care_bits.type=3;
	aclRule.data_bits.active_portmsk=0x44;
	aclRule.data_bits.tag_exist=0x5;
	aclRule.data_bits.type=3;
    aclRule.valid=1;
    aclRule.mode = APOLLO_ACL_MODE_64ENTRIES;
#if 0
    osal_printf(" Set val:%x  ,%x  %x %x  %x  %x  %x  %x\n",AclRule.valid,AclRule.care_bits.active_portmsk,AclRule.care_bits.tag_exist,
    AclRule.care_bits.type,AclRule.data_bits.active_portmsk,AclRule.data_bits.tag_exist,AclRule.data_bits.type);
#endif
	for(i = 0; i < APOLLO_ACLRULEFIELDNO; i++)
	{
        aclRule.care_bits.field[i]= 0xffff;
	    aclRule.data_bits.field[i]= i+5;
	}

#if defined(CONFIG_FPGA_TEST)
     for(i = 0; i < 8; i++)
#else
     for(i = 0; i < APOLLO_ACLRULENO; i++)
#endif

    {
        aclRule.idx = i;
		if (( ret = apollo_raw_acl_rule_set(&aclRule)) != RT_ERR_OK)
        {
            return ret;
        }
    }
#if defined(CONFIG_FPGA_TEST)
    for(i = 0; i < 8; i++)
#else
    for(i = 0; i < APOLLO_ACLRULENO; i++)
#endif
    {
        osal_memset(&aclRuleRd, 0, sizeof(apollo_raw_acl_ruleEntry_t));
        aclRuleRd.idx = i;
        aclRule.idx = i;
        aclRuleRd.mode = APOLLO_ACL_MODE_64ENTRIES;
        if (( ret = apollo_raw_acl_rule_get(&aclRuleRd)) == RT_ERR_OK)
        {
            if (osal_memcmp( &aclRule, &aclRuleRd, sizeof(apollo_raw_acl_ruleEntry_t)))
            {
#if 0
              	osal_printf("get val:%x  ,%x  %x %x  %x  %x  %x \n",aclRuleRd.valid,aclRuleRd.care_bits.active_portmsk,aclRuleRd.care_bits.tag_exist,
                aclRuleRd.care_bits.type,aclRuleRd.data_bits.active_portmsk,aclRuleRd.data_bits.tag_exist,aclRule.data_bits.type);
#endif
                osal_printf("%s(%u), index:%u , compare error\n", __FUNCTION__, __LINE__, i);
                return RT_ERR_FAILED;
            }
        }
        else
        {
            osal_printf("%s(%u), Acl rule get error!\n", __FUNCTION__, __LINE__);
            return ret;

        }
    }

	return RT_ERR_OK;
}
int32 dal_raw_aclRule128_test(uint32 caseNo)
{
    int32 ret = RT_ERR_FAILED;
    int32 test_port = 0;
    uint32 i;
    uint32  test_tmp=0;
	apollo_raw_acl_act_t aclAct;
    apollo_raw_acl_act_t aclActRd;
	apollo_raw_acl_ruleEntry_t aclRule;
    apollo_raw_acl_ruleEntry_t aclRuleRd;
	apollo_raw_acl_template_t AclType;

	uint32 index;
	uint32 type,upperValue,lowerValue;

	apollo_raw_acl_mode_set(APOLLO_ACL_MODE_128ENTRIES);
    osal_memset(&aclRule,0,sizeof(apollo_raw_acl_ruleEntry_t));
	aclRule.care_bits.active_portmsk=0x7f;
	aclRule.care_bits.tag_exist=0x7f;
	aclRule.care_bits.type=3;
	aclRule.data_bits.active_portmsk=0x44;
	aclRule.data_bits.tag_exist=0x5;
	aclRule.data_bits.type=3;
    aclRule.valid=1;
    aclRule.mode = APOLLO_ACL_MODE_128ENTRIES;
#if 0
    osal_printf(" Set val:%x  ,%x  %x %x  %x  %x  %x\n",aclRule.valid,aclRule.care_bits.active_portmsk,aclRule.care_bits.tag_exist,
    aclRule.care_bits.type,aclRule.data_bits.active_portmsk,aclRule.data_bits.tag_exist,aclRule.data_bits.type);
#endif
	for(i = 0; i < APOLLO_ACLRULEFIELDNO; i++)
	{
        aclRule.care_bits.field[i]= 0xffff;
	    aclRule.data_bits.field[i]= i+5;
	}
    aclRule.idx = 0;

#if defined(CONFIG_FPGA_TEST)
    for(i = 0; i < 72; i++)
    {
        if(i == 8)
            i = 64;
#else
    for(i = 0; i < APOLLO_ACLACTIONNO; i++)
    {
#endif

        aclRule.idx = i;
		if((ret = apollo_raw_acl_rule_set(&aclRule)) != RT_ERR_OK)
            return ret;
    }
    aclRule.data_bits.field[7] = 0;
    aclRule.data_bits.field[6] = 0;
    aclRule.data_bits.field[5] = 0;
    aclRule.data_bits.field[4] = 0;
    aclRule.care_bits.field[7] = 0;
    aclRule.care_bits.field[6] = 0;
    aclRule.care_bits.field[5] = 0;
    aclRule.care_bits.field[4] = 0;    
#if defined(CONFIG_FPGA_TEST)
    for(i = 0; i < 72; i++)
    {
        if(i == 8)
            i = 64;
#else
    for(i = 0; i < APOLLO_ACLACTIONNO; i++)
    {
#endif
        osal_memset(&aclRuleRd, 0, sizeof(apollo_raw_acl_ruleEntry_t));
        aclRuleRd.idx = i;
        aclRule.idx = i;
        aclRuleRd.mode = APOLLO_ACL_MODE_128ENTRIES;
        if (( ret = apollo_raw_acl_rule_get(&aclRuleRd)) == RT_ERR_OK)
        {
            aclRuleRd.data_bits.field[7] = 0;
            aclRuleRd.data_bits.field[6] = 0;
            aclRuleRd.data_bits.field[5] = 0;
            aclRuleRd.data_bits.field[4] = 0;
            aclRuleRd.care_bits.field[7] = 0;
            aclRuleRd.care_bits.field[6] = 0;
            aclRuleRd.care_bits.field[5] = 0;
            aclRuleRd.care_bits.field[4] = 0;
            
            if( APOLLO_ACLRULENO == i)
             {
                 aclRule.data_bits.field[3] = 0;
                 aclRule.care_bits.field[3] = 0;
                 aclRuleRd.data_bits.field[3] = 0;
                 aclRuleRd.care_bits.field[3] = 0;
             }

            if (osal_memcmp( &aclRule, &aclRuleRd, sizeof(apollo_raw_acl_ruleEntry_t)))
            {
#if 0
                    osal_printf("aclRule:%x,%x,%x,%x,%x,%x,%x,%x\n",aclRule.data_bits.field[0],aclRule.data_bits.field[1],aclRule.data_bits.field[2],aclRule.data_bits.field[3],
                    aclRule.care_bits.field[0],aclRule.care_bits.field[1],aclRule.care_bits.field[2],aclRule.care_bits.field[3]);
                    osal_printf("aclRuleRd:%x,%x,%x,%x,%x,%x,%x,%x\n",aclRuleRd.data_bits.field[0],aclRuleRd.data_bits.field[1],aclRuleRd.data_bits.field[2],aclRuleRd.data_bits.field[3],
                    aclRule.care_bits.field[0],aclRuleRd.care_bits.field[1],aclRuleRd.care_bits.field[2],aclRuleRd.care_bits.field[3]);
                	osal_printf("get val:%x  ,%x  %x %x  %x  %x  %x  \n",aclRuleRd.valid,aclRuleRd.care_bits.active_portmsk,aclRuleRd.care_bits.tag_exist,
                        aclRuleRd.care_bits.type,aclRuleRd.data_bits.active_portmsk,aclRuleRd.data_bits.tag_exist,aclRule.data_bits.type);
                    
#endif
                    osal_printf("%s(%u), index:%u , compare error\n",__FUNCTION__, __LINE__, i);
                    return RT_ERR_FAILED;
            }
        }
        else
        {
            osal_printf("%s(%u), Acl rule get error index:%u!\n",__FUNCTION__, __LINE__, i);
            return ret;

        }
    }
	return RT_ERR_OK;
}
int32 dal_raw_aclAct_test(uint32 caseNo)
{
    int32 ret;
    uint32 i;
    apollo_raw_acl_act_t aclAct;
    apollo_raw_acl_act_t aclActRd;

    osal_memset(&aclAct, 0, sizeof(aclAct));

	aclAct.cfidx=3;
	aclAct.cfact=2;
	aclAct.aclint=1;
	aclAct.pridx=1;
	aclAct.priact=2;
	aclAct.fwdpmask=3;
	aclAct.fwdact=1;
	aclAct.meteridx=2;
	aclAct.policact=0;
	aclAct.svidx_sact=1;
	aclAct.sact=2;
	aclAct.cvidx_cact=3;
	aclAct.cact=3;

    for(i = 0; i < APOLLO_ACLRULENO; i++)
	{
	    aclAct.idx = i;
	   	if((ret = apollo_raw_acl_act_set(&aclAct)) != RT_ERR_OK)
        {
           osal_printf("%s(%u), index:%u\n", __FUNCTION__, __LINE__, i);
           break;
        }
	}

    for(i = 0; i < APOLLO_ACLRULENO; i++)
	{
	    osal_memset(&aclActRd, 0, sizeof(aclActRd));
        aclAct.idx = i;
        aclActRd.idx = i;
        if((ret = apollo_raw_acl_act_get(&aclActRd)) != RT_ERR_OK)
        {
            osal_printf("%s(%u), index:%u\n", __FUNCTION__, __LINE__, i);
            break;
        }
        if (osal_memcmp( &aclAct, &aclActRd, sizeof(aclActRd)))
        {
            osal_printf("%s(%u), index:%u ,compare error\n", __FUNCTION__, __LINE__,i);
            break;
        }

    }
#if 0
        osal_printf("\nACL %u act values:\n", aclAct.idx);
        osal_memset(&aclAct, 0, sizeof(apollo_raw_acl_act_t));
        aclAct.idx = 0;
        apollo_raw_acl_act_get(&aclAct);

        osal_printf("%8u %8u %8u %8u %8u %8u %8u %8u %8u %8u %8u %8u %8u\n",
            aclAct.cfidx,
            aclAct.cfact,
            aclAct.aclint,
            aclAct.pridx,
            aclAct.priact,
            aclAct.fwdpmask,
            aclAct.fwdact,
            aclAct.meteridx,
            aclAct.policact,
            aclAct.svidx_sact,
            aclAct.sact,
            aclAct.cvidx_cact,
            aclAct.cact
        );
#endif
    osal_memset(&aclAct, 0, sizeof(aclAct));

	aclAct.cfidx=0x7f;
	aclAct.cfact=0x3;
	aclAct.aclint=1;
	aclAct.pridx=0x3f;
	aclAct.priact=0x3;
	aclAct.fwdpmask=0x7f;
	aclAct.fwdact=0x3;
	aclAct.meteridx=0x1f;
	aclAct.policact=1;
	aclAct.svidx_sact=0x3f;
	aclAct.sact=0x7;
	aclAct.cvidx_cact=0x1f;
	aclAct.cact=0x7;

    for(i = 0; i < APOLLO_ACLRULENO; i++)
	{
	    aclAct.idx = i;
	   	if((ret = apollo_raw_acl_act_set(&aclAct)) != RT_ERR_OK)
        {
           osal_printf("%s(%u), index:%u\n", __FUNCTION__, __LINE__, i);
           break;
        }
	}

    for(i = 0; i < APOLLO_ACLRULENO; i++)
	{
	    osal_memset(&aclActRd, 0, sizeof(aclActRd));
        aclAct.idx = i;
        aclActRd.idx = i;
        if((ret = apollo_raw_acl_act_get(&aclActRd)) != RT_ERR_OK)
        {
            osal_printf("%s(%u), index:%u\n", __FUNCTION__, __LINE__, i);
            break;
        }
        if (osal_memcmp( &aclAct, &aclActRd, sizeof(aclActRd)))
        {
            osal_printf("%s(%u), index:%u ,compare error\n", __FUNCTION__, __LINE__,i);
            break;
        }

    }
	return RT_ERR_OK;
}


int32 dal_acl_raw_test(uint32 caseNo)
{
    int32 ret;
    rtk_port_t port;
    rtk_enable_t enableR;
    rtk_enable_t enableW;
    apollo_raw_acl_modeTypes_t modeR;
    apollo_raw_acl_modeTypes_t modeW;
    apollo_raw_acl_portRange_t portRangeR;
    apollo_raw_acl_portRange_t portRangeW;
    apollo_raw_acl_vidRange_t vidRangeR;
    apollo_raw_acl_vidRange_t vidRangeW;
    apollo_raw_acl_ipRange_t ipRangeW;
    apollo_raw_acl_ipRange_t ipRangeR;
    apollo_raw_acl_pktLenRange_t pktLenRangeR;
    apollo_raw_acl_pktLenRange_t pktLenRangeW;
    apollo_raw_acl_template_t aclTemplateR;
    apollo_raw_acl_template_t aclTemplateW;
    apollo_raw_acl_actCtrl_t aclActCtrlR;
    apollo_raw_acl_actCtrl_t aclActCtrlW;
    uint32 regData;
    uint32 dataMask;
    uint32 checkData;

    /*error input check*/
    /*out of range*/
    if( apollo_raw_acl_mode_set(APOLLO_ACL_MODE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_acl_state_set(APOLLO_PORTNO, 0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_acl_state_set(0, RTK_ENABLE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_acl_state_get(APOLLO_PORTNO, &enableR) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_acl_permit_set(APOLLO_PORTNO, 0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_acl_permit_set(0, RTK_ENABLE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_acl_permit_get(APOLLO_PORTNO, &enableR) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }


    osal_memset(&portRangeW, 0x0, sizeof(apollo_raw_acl_portRange_t));
    portRangeW.idx = APOLLO_ACLRANGEMAX1 + 1;
    if( apollo_raw_acl_portRange_set(&portRangeW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    portRangeR.idx = APOLLO_ACLRANGEMAX1 + 1;
    if( apollo_raw_acl_portRange_get(&portRangeR) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&portRangeW, 0x0, sizeof(apollo_raw_acl_portRange_t));
    portRangeW.type = APOLLO_ACL_RNGL4PORTTYPE_END;
    if( apollo_raw_acl_portRange_set(&portRangeW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&portRangeW, 0x0, sizeof(apollo_raw_acl_portRange_t));
    portRangeW.upperPort = APOLLO_L4PORT_MAX + 1;
    if( apollo_raw_acl_portRange_set(&portRangeW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&portRangeW, 0x0, sizeof(apollo_raw_acl_portRange_t));
    portRangeW.lowerPort = APOLLO_L4PORT_MAX + 1;
    if( apollo_raw_acl_portRange_set(&portRangeW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&vidRangeW, 0x0, sizeof(apollo_raw_acl_vidRange_t));
    vidRangeW.idx = APOLLO_ACLRANGEMAX2 + 1;
    if( apollo_raw_acl_vidRange_set(&vidRangeW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    vidRangeR.idx = APOLLO_ACLRANGEMAX2 + 1;
    if( apollo_raw_acl_vidRange_get(&vidRangeR) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&vidRangeW, 0x0, sizeof(apollo_raw_acl_vidRange_t));
    vidRangeW.type = APOLLO_ACL_RNGVIDTYPE_END;
    if( apollo_raw_acl_vidRange_set(&vidRangeW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&vidRangeW, 0x0, sizeof(apollo_raw_acl_vidRange_t));
    vidRangeW.upperVid = APOLLO_VIDMAX + 1;
    if( apollo_raw_acl_vidRange_set(&vidRangeW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&vidRangeW, 0x0, sizeof(apollo_raw_acl_vidRange_t));
    vidRangeW.lowerVid = APOLLO_VIDMAX + 1;
    if( apollo_raw_acl_vidRange_set(&vidRangeW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&ipRangeW, 0x0, sizeof(apollo_raw_acl_ipRange_t));
    ipRangeW.idx = APOLLO_ACLRANGEMAX2 + 1;
    if( apollo_raw_acl_ipRange_set(&ipRangeW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    ipRangeR.idx = APOLLO_ACLRANGEMAX2 + 1;
    if( apollo_raw_acl_ipRange_get(&ipRangeR) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&ipRangeW, 0x0, sizeof(apollo_raw_acl_ipRange_t));
    ipRangeW.type = APOLLO_ACL_RNGIPTYPE_END;
    if( apollo_raw_acl_ipRange_set(&ipRangeW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&pktLenRangeW, 0x0, sizeof(apollo_raw_acl_pktLenRange_t));
    pktLenRangeW.idx = APOLLO_ACLRANGEMAX2 + 1;
    if( apollo_raw_acl_pktLenRange_set(&pktLenRangeW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    pktLenRangeR.idx = APOLLO_ACLRANGEMAX2 + 1;
    if( apollo_raw_acl_pktLenRange_get(&pktLenRangeR) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&pktLenRangeW, 0x0, sizeof(apollo_raw_acl_pktLenRange_t));
    pktLenRangeW.type = APOLLO_ACL_RNGPKTLENTYPE_END;
    if( apollo_raw_acl_pktLenRange_set(&pktLenRangeW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&pktLenRangeW, 0x0, sizeof(apollo_raw_acl_pktLenRange_t));
    pktLenRangeW.upperPktLen = APOLLO_RAW_ACL_PKTLEN_MAX + 1;
    if( apollo_raw_acl_pktLenRange_set(&pktLenRangeW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&pktLenRangeW, 0x0, sizeof(apollo_raw_acl_pktLenRange_t));
    pktLenRangeW.lowerPktLen = APOLLO_RAW_ACL_PKTLEN_MAX + 1;
    if( apollo_raw_acl_pktLenRange_set(&pktLenRangeW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&aclTemplateW, 0x0, sizeof(apollo_raw_acl_template_t));
    aclTemplateW.idx = APOLLO_ACLTEMPLATEMAX + 1;
    if( apollo_raw_acl_template_set(&aclTemplateW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&aclTemplateR, 0x0, sizeof(apollo_raw_acl_template_t));
    aclTemplateR.idx = APOLLO_ACLTEMPLATEMAX + 1;
    if( apollo_raw_acl_template_get(&aclTemplateR) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&aclActCtrlW, 0x0, sizeof(apollo_raw_acl_actCtrl_t));
    aclActCtrlW.idx = APOLLO_ACLACTIONMAX + 1;
    if( apollo_raw_acl_actCtrl_set(&aclActCtrlW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&aclActCtrlR, 0x0, sizeof(apollo_raw_acl_actCtrl_t));
    aclActCtrlR.idx = APOLLO_ACLACTIONMAX + 1;
    if( apollo_raw_acl_actCtrl_get(&aclActCtrlR) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }



    /* get/set test*/
    for(port=0; port<7 ; port++)
    {
        for(enableW = DISABLED; enableW < RTK_ENABLE_END; enableW ++)
        {
            if( apollo_raw_acl_state_set(port, enableW) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            if( apollo_raw_acl_state_get(port, &enableR) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if(enableW != enableR)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if( apollo_raw_acl_permit_set(port, enableW) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            if( apollo_raw_acl_permit_get(port, &enableR) != RT_ERR_OK)
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
    }

    for(modeW = 0; modeW < APOLLO_ACL_MODE_END; modeW ++)
    {
        if( apollo_raw_acl_mode_set(modeW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if( apollo_raw_acl_mode_get(&modeR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if(modeW != modeR)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }

    for(portRangeW.idx = 0; portRangeW.idx <= APOLLO_ACLRANGEMAX1; portRangeW.idx ++)
    {
        for(portRangeW.type = 0; portRangeW.type < APOLLO_ACL_RNGL4PORTTYPE_END; portRangeW.type ++)
        {
            for(portRangeW.upperPort = 0, portRangeW.lowerPort = APOLLO_L4PORT_MAX - portRangeW.upperPort; portRangeW.upperPort <= APOLLO_L4PORT_MAX; portRangeW.upperPort +=0x1FFF, portRangeW.lowerPort = APOLLO_L4PORT_MAX - portRangeW.upperPort)
            {
                if( apollo_raw_acl_portRange_set(&portRangeW) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }

                portRangeR.idx = portRangeW.idx;
                if( apollo_raw_acl_portRange_get(&portRangeR) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }

                if((portRangeW.type != portRangeR.type) ||
                    (portRangeW.upperPort != portRangeR.upperPort) ||
                    (portRangeW.lowerPort != portRangeR.lowerPort))
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }

            }
        }
    }

    for(vidRangeW.idx = 0; vidRangeW.idx <= APOLLO_ACLRANGEMAX2; vidRangeW.idx ++)
    {
        for(vidRangeW.type = 0; vidRangeW.type < APOLLO_ACL_RNGVIDTYPE_END; vidRangeW.type ++)
        {
            for(vidRangeW.upperVid = 0, vidRangeW.lowerVid = APOLLO_VIDMAX - vidRangeW.upperVid; vidRangeW.upperVid <= APOLLO_VIDMAX; vidRangeW.upperVid +=0x3FF, vidRangeW.lowerVid = APOLLO_VIDMAX - vidRangeW.upperVid)
            {
                if( apollo_raw_acl_vidRange_set(&vidRangeW) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }

                vidRangeR.idx = vidRangeW.idx;
                if( apollo_raw_acl_vidRange_get(&vidRangeR) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }

                if((vidRangeW.type != vidRangeR.type) ||
                    (vidRangeW.upperVid != vidRangeR.upperVid) ||
                    (vidRangeW.lowerVid != vidRangeR.lowerVid))
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }

            }
        }
    }

    for(ipRangeW.idx = 0; ipRangeW.idx <= APOLLO_ACLRANGEMAX2; ipRangeW.idx ++)
    {
        for(ipRangeW.type = 0; ipRangeW.type < APOLLO_ACL_RNGIPTYPE_END; ipRangeW.type ++)
        {
            for(ipRangeW.upperIp = 0, ipRangeW.lowerIp = 0xFFFFFFFF - ipRangeW.upperIp; ipRangeW.upperIp <= 0x87654321; ipRangeW.upperIp += 0x12345678, ipRangeW.lowerIp = 0xFFFFFFFF - ipRangeW.upperIp)
            {
                if( apollo_raw_acl_ipRange_set(&ipRangeW) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }

                ipRangeR.idx = ipRangeW.idx;
                if( apollo_raw_acl_ipRange_get(&ipRangeR) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }


                if((ipRangeW.type != ipRangeR.type) ||
                    (ipRangeW.upperIp != ipRangeR.upperIp) ||
                    (ipRangeW.lowerIp != ipRangeR.lowerIp))
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }
            }
        }
    }

    for(pktLenRangeW.idx = 0; pktLenRangeW.idx <= APOLLO_ACLRANGEMAX2; pktLenRangeW.idx ++)
    {
        for(pktLenRangeW.type = 0; pktLenRangeW.type < APOLLO_ACL_RNGPKTLENTYPE_END; pktLenRangeW.type ++)
        {
            for(pktLenRangeW.upperPktLen = 0, pktLenRangeW.lowerPktLen = APOLLO_RAW_ACL_PKTLEN_MAX - pktLenRangeW.upperPktLen; pktLenRangeW.upperPktLen <= APOLLO_RAW_ACL_PKTLEN_MAX; pktLenRangeW.upperPktLen += 0x1234, pktLenRangeW.lowerPktLen = APOLLO_RAW_ACL_PKTLEN_MAX - pktLenRangeW.upperPktLen)
            {
                if( apollo_raw_acl_pktLenRange_set(&pktLenRangeW) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }

                pktLenRangeR.idx = pktLenRangeW.idx;
                if( apollo_raw_acl_pktLenRange_get(&pktLenRangeR) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }

                if((pktLenRangeW.type != pktLenRangeR.type) ||
                    (pktLenRangeW.upperPktLen != pktLenRangeR.upperPktLen) ||
                    (pktLenRangeW.lowerPktLen != pktLenRangeR.lowerPktLen))
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }
            }
        }
    }

    for(aclTemplateW.idx = 0; aclTemplateW.idx <= APOLLO_ACLTEMPLATEMAX; aclTemplateW.idx ++)
    {
        for(aclTemplateW.field[0] = 0; aclTemplateW.field[0] < 0x7F; aclTemplateW.field[0] += 0x38)
        {
            aclTemplateW.field[1] = aclTemplateW.field[0] + 1;
            aclTemplateW.field[2] = aclTemplateW.field[1] + 1;
            aclTemplateW.field[3] = aclTemplateW.field[2] + 1;
            aclTemplateW.field[4] = aclTemplateW.field[3] + 1;
            aclTemplateW.field[5] = aclTemplateW.field[4] + 1;
            aclTemplateW.field[6] = aclTemplateW.field[5] + 1;
            aclTemplateW.field[7] = aclTemplateW.field[6] + 1;


            if( apollo_raw_acl_template_set(&aclTemplateW) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            aclTemplateR.idx = aclTemplateW.idx;
            if( apollo_raw_acl_template_get(&aclTemplateR) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if((aclTemplateR.field[0] != aclTemplateW.field[0]) ||
                (aclTemplateR.field[1] != aclTemplateW.field[1]) ||
                (aclTemplateR.field[2] != aclTemplateW.field[2]) ||
                (aclTemplateR.field[3] != aclTemplateW.field[3]) ||
                (aclTemplateR.field[4] != aclTemplateW.field[4]) ||
                (aclTemplateR.field[5] != aclTemplateW.field[5]) ||
                (aclTemplateR.field[6] != aclTemplateW.field[6]) ||
                (aclTemplateR.field[7] != aclTemplateW.field[7]))
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
        }
    }

    for(aclActCtrlW.idx = 0; aclActCtrlW.idx <= APOLLO_ACLACTIONMAX; aclActCtrlW.idx ++)
    {
        for(aclActCtrlW.valid = 0; aclActCtrlW.valid < RTK_ENABLE_END; aclActCtrlW.valid ++)
        {
            aclActCtrlW.not = aclActCtrlW.valid;
            aclActCtrlW.cvlan_en = (!aclActCtrlW.valid);
            aclActCtrlW.svlan_en = aclActCtrlW.valid;
            aclActCtrlW.polic_en = (!aclActCtrlW.valid);
            aclActCtrlW.pri_remark_en = aclActCtrlW.valid;
            aclActCtrlW.int_cf_en = (!aclActCtrlW.valid);
            aclActCtrlW.fwd_en = aclActCtrlW.valid;
#if 0
            osal_printf("%x %x %x %x %x %x %x %x - %x %x %x %x %x %x %x %x\n",
                                aclActCtrlR.valid,
                                aclActCtrlR.not,
                                aclActCtrlR.cvlan_en,
                                aclActCtrlR.svlan_en,
                                aclActCtrlR.polic_en,
                                aclActCtrlR.pri_remark_en,
                                aclActCtrlR.int_cf_en,
                                aclActCtrlR.fwd_en,
                                aclActCtrlW.valid,
                                aclActCtrlW.not,
                                aclActCtrlW.cvlan_en,
                                aclActCtrlW.svlan_en,
                                aclActCtrlW.polic_en,
                                aclActCtrlW.pri_remark_en,
                                aclActCtrlW.int_cf_en,
                                aclActCtrlW.fwd_en);
#endif
            if( apollo_raw_acl_actCtrl_set(&aclActCtrlW) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            aclActCtrlR.idx = aclActCtrlW.idx;
            if( apollo_raw_acl_actCtrl_get(&aclActCtrlR) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if((aclActCtrlR.valid != aclActCtrlW.valid) ||
                (aclActCtrlR.not != aclActCtrlW.not) ||
                (aclActCtrlR.cvlan_en != aclActCtrlW.cvlan_en) ||
                (aclActCtrlR.svlan_en != aclActCtrlW.svlan_en) ||
                (aclActCtrlR.polic_en != aclActCtrlW.polic_en) ||
                (aclActCtrlR.pri_remark_en != aclActCtrlW.pri_remark_en) ||
                (aclActCtrlR.int_cf_en != aclActCtrlW.int_cf_en) ||
                (aclActCtrlR.fwd_en != aclActCtrlW.fwd_en))
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
        }
    }


    /*null pointer*/
    if( apollo_raw_acl_mode_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_acl_state_get(0,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_acl_permit_get(0,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_acl_portRange_set(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_acl_portRange_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_acl_vidRange_set(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_acl_vidRange_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_acl_ipRange_set(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_acl_ipRange_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_acl_pktLenRange_set(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_acl_pktLenRange_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_acl_template_set(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_acl_template_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_acl_actCtrl_set(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_acl_actCtrl_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }


    /*register access test*/
    for (port = 0; port < APOLLO_PORTNO; port++)
    {
        ioal_mem32_write(HAL_GET_REG_ADDR(ACL_ENr), 0x0);
        apollo_raw_acl_state_set(port, ENABLED);
        ioal_mem32_read(HAL_GET_REG_ADDR(ACL_ENr), &regData);
        dataMask = 1<<port;
        checkData = 1<<port;
        /*mask out reserve bits*/
        regData = regData & dataMask;
        if(regData != checkData)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

    }

    for (port = 0; port < APOLLO_PORTNO; port++)
    {
        ioal_mem32_write(HAL_GET_REG_ADDR(ACL_PERMITr), 0x0);
        apollo_raw_acl_permit_set(port, ENABLED);
        ioal_mem32_read(HAL_GET_REG_ADDR(ACL_PERMITr), &regData);
        dataMask = 1<<port;
        checkData = 1<<port;
        /*mask out reserve bits*/
        regData = regData & dataMask;
        if(regData != checkData)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

    }

    for(aclActCtrlW.idx = 0; aclActCtrlW.idx <= APOLLO_ACLACTIONMAX; aclActCtrlW.idx ++)
    {
            aclActCtrlW.valid = 1;
            aclActCtrlW.not = 0;
            aclActCtrlW.int_cf_en = 0;
            aclActCtrlW.fwd_en = 0;
            aclActCtrlW.polic_en = 0;
            aclActCtrlW.pri_remark_en = 0;
            aclActCtrlW.svlan_en = 0;
            aclActCtrlW.cvlan_en = 0;
            ioal_mem32_write(HAL_GET_REG_ADDR(ACL_ACTIONr) + aclActCtrlW.idx*4, 0x0);
            apollo_raw_acl_actCtrl_set(&aclActCtrlW);
            ioal_mem32_read(HAL_GET_REG_ADDR(ACL_ACTIONr) + aclActCtrlW.idx*4, &regData);
            dataMask = 0x80;
            checkData = 0x80;
            regData = regData & dataMask;
            if(regData != checkData)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            aclActCtrlW.valid = 0;
            aclActCtrlW.not = 1;
            ioal_mem32_write(HAL_GET_REG_ADDR(ACL_ACTIONr) + aclActCtrlW.idx*4, 0x0);
            apollo_raw_acl_actCtrl_set(&aclActCtrlW);
            ioal_mem32_read(HAL_GET_REG_ADDR(ACL_ACTIONr) + aclActCtrlW.idx*4, &regData);
            dataMask = 0x40;
            checkData = 0x40;
            regData = regData & dataMask;
            if(regData != checkData)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            aclActCtrlW.not = 0;
            aclActCtrlW.int_cf_en = 1;
            ioal_mem32_write(HAL_GET_REG_ADDR(ACL_ACTIONr) + aclActCtrlW.idx*4, 0x0);
            apollo_raw_acl_actCtrl_set(&aclActCtrlW);
            ioal_mem32_read(HAL_GET_REG_ADDR(ACL_ACTIONr) + aclActCtrlW.idx*4, &regData);
            dataMask = 0x20;
            checkData = 0x20;
            regData = regData & dataMask;
            if(regData != checkData)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            aclActCtrlW.int_cf_en = 0;
            aclActCtrlW.fwd_en = 1;
            ioal_mem32_write(HAL_GET_REG_ADDR(ACL_ACTIONr) + aclActCtrlW.idx*4, 0x0);
            apollo_raw_acl_actCtrl_set(&aclActCtrlW);
            ioal_mem32_read(HAL_GET_REG_ADDR(ACL_ACTIONr) + aclActCtrlW.idx*4, &regData);
            dataMask = 0x10;
            checkData = 0x10;
            regData = regData & dataMask;
            if(regData != checkData)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            aclActCtrlW.fwd_en = 0;
            aclActCtrlW.polic_en = 1;
            ioal_mem32_write(HAL_GET_REG_ADDR(ACL_ACTIONr) + aclActCtrlW.idx*4, 0x0);
            apollo_raw_acl_actCtrl_set(&aclActCtrlW);
            ioal_mem32_read(HAL_GET_REG_ADDR(ACL_ACTIONr) + aclActCtrlW.idx*4, &regData);
            dataMask = 0x8;
            checkData = 0x8;
            regData = regData & dataMask;
            if(regData != checkData)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            aclActCtrlW.polic_en = 0;
            aclActCtrlW.pri_remark_en = 1;
            ioal_mem32_write(HAL_GET_REG_ADDR(ACL_ACTIONr) + aclActCtrlW.idx*4, 0x0);
            apollo_raw_acl_actCtrl_set(&aclActCtrlW);
            ioal_mem32_read(HAL_GET_REG_ADDR(ACL_ACTIONr) + aclActCtrlW.idx*4, &regData);
            dataMask = 0x4;
            checkData = 0x4;
            regData = regData & dataMask;
            if(regData != checkData)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            aclActCtrlW.pri_remark_en = 0;
            aclActCtrlW.svlan_en = 1;
            ioal_mem32_write(HAL_GET_REG_ADDR(ACL_ACTIONr) + aclActCtrlW.idx*4, 0x0);
            apollo_raw_acl_actCtrl_set(&aclActCtrlW);
            ioal_mem32_read(HAL_GET_REG_ADDR(ACL_ACTIONr) + aclActCtrlW.idx*4, &regData);
            dataMask = 0x2;
            checkData = 0x2;
            regData = regData & dataMask;
            if(regData != checkData)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            aclActCtrlW.svlan_en = 0;
            aclActCtrlW.cvlan_en = 1;
            ioal_mem32_write(HAL_GET_REG_ADDR(ACL_ACTIONr) + aclActCtrlW.idx*4, 0x0);
            apollo_raw_acl_actCtrl_set(&aclActCtrlW);
            ioal_mem32_read(HAL_GET_REG_ADDR(ACL_ACTIONr) + aclActCtrlW.idx*4, &regData);
            dataMask = 0x1;
            checkData = 0x1;
            regData = regData & dataMask;
            if(regData != checkData)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

    }

    for(aclTemplateW.idx = 0; aclTemplateW.idx <= APOLLO_ACLTEMPLATEMAX; aclTemplateW.idx++)
    {

        aclTemplateW.field[0] = 0x7f;
        aclTemplateW.field[1] = 0x7f;
        aclTemplateW.field[2] = 0x7f;
        aclTemplateW.field[3] = 0x7f;
        aclTemplateW.field[4] = 0x7f;
        aclTemplateW.field[5] = 0x7f;
        aclTemplateW.field[6] = 0x7f;
        aclTemplateW.field[7] = 0x7f;
        ioal_mem32_write(HAL_GET_REG_ADDR(ACL_TEMPLATE_CTRLr) + aclTemplateW.idx*8, 0x0);
        ioal_mem32_write(HAL_GET_REG_ADDR(ACL_TEMPLATE_CTRLr) + aclTemplateW.idx*8 + 4, 0x0);
        apollo_raw_acl_template_set(&aclTemplateW);
        ioal_mem32_read(HAL_GET_REG_ADDR(ACL_TEMPLATE_CTRLr) + aclTemplateW.idx*8, &regData);
        dataMask = 0xfffffff;
        checkData = 0xfffffff;
        regData = regData & dataMask;
        if(regData != checkData)
        {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
        }
        ioal_mem32_read(HAL_GET_REG_ADDR(ACL_TEMPLATE_CTRLr) + aclTemplateW.idx*8 + 4, &regData);
        dataMask = 0xfffffff;
        checkData = 0xfffffff;
        regData = regData & dataMask;
        if(regData != checkData)
        {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
        }


    }

    for(portRangeW.idx = 0; portRangeW.idx <= APOLLO_ACLRANGEMAX1; portRangeW.idx ++)
    {
        portRangeW.lowerPort = 0xcccc;
        portRangeW.upperPort = 0x5555;
        portRangeW.type = APOLLO_ACL_RNGL4PORTTYPE_DEST;
        ioal_mem32_write(HAL_GET_REG_ADDR(RNG_CHK_L4PORT_RNGr) + portRangeW.idx*8, 0x0);
        ioal_mem32_write(HAL_GET_REG_ADDR(RNG_CHK_L4PORT_RNGr) + portRangeW.idx*8 + 4, 0x0);
        apollo_raw_acl_portRange_set(&portRangeW);
        ioal_mem32_read(HAL_GET_REG_ADDR(RNG_CHK_L4PORT_RNGr) + portRangeW.idx*8 +4, &regData);
        dataMask = 0xffffffff;
        checkData = 0x5555cccc;
        regData = regData & dataMask;
        if(regData != checkData)
        {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
        }
        ioal_mem32_read(HAL_GET_REG_ADDR(RNG_CHK_L4PORT_RNGr) + portRangeW.idx*8, &regData);
        dataMask = 0x3;
        checkData = APOLLO_ACL_RNGL4PORTTYPE_DEST;
        regData = regData & dataMask;
        if(regData != checkData)
        {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
        }

    }

    for(vidRangeW.idx = 0; vidRangeW.idx <= APOLLO_ACLRANGEMAX2; vidRangeW.idx ++)
    {
        vidRangeW.upperVid =0xf00;
        vidRangeW.lowerVid =0x00e;
        vidRangeW.type = APOLLO_ACL_RNGVIDTYPE_SVLAN;
        ioal_mem32_write(HAL_GET_REG_ADDR(RNG_CHK_VID_RNGr) + vidRangeW.idx*4, 0x0);
        apollo_raw_acl_vidRange_set(&vidRangeW);
        ioal_mem32_read(HAL_GET_REG_ADDR((RNG_CHK_VID_RNGr)) + vidRangeW.idx*4, &regData);
        dataMask = 0x3ffffff;
        checkData = 0x2f0000e;
        regData = regData & dataMask;
        if(regData != checkData)
        {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
        }
    }


    for(ipRangeW.idx = 0; ipRangeW.idx <= APOLLO_ACLRANGEMAX2; ipRangeW.idx ++)
    {

        ipRangeW.upperIp = 0xcccccccc;
        ipRangeW.lowerIp = 0x55555555;
        ipRangeW.type = APOLLO_ACL_RNGIPTYPE_V6DIP;

        ioal_mem32_write(HAL_GET_REG_ADDR(RNG_CHK_IP_RNGr) + ipRangeW.idx*12, 0x0);
        ioal_mem32_write(HAL_GET_REG_ADDR(RNG_CHK_IP_RNGr) + ipRangeW.idx*12 +4, 0x0);
        ioal_mem32_write(HAL_GET_REG_ADDR(RNG_CHK_IP_RNGr) + ipRangeW.idx*12 +8, 0x0);
        apollo_raw_acl_ipRange_set(&ipRangeW);
        ioal_mem32_read(HAL_GET_REG_ADDR((RNG_CHK_IP_RNGr)) + ipRangeW.idx*12 + 8, &regData);
        dataMask =  0xffffffff;
        checkData = 0x55555555;
        regData = regData & dataMask;
        if(regData != checkData)
        {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
        }

        ioal_mem32_read(HAL_GET_REG_ADDR((RNG_CHK_IP_RNGr)) + ipRangeW.idx*12 + 4, &regData);
        dataMask =  0xffffffff;
        checkData = 0xcccccccc;
        regData = regData & dataMask;
        if(regData != checkData)
        {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
        }

         ioal_mem32_read(HAL_GET_REG_ADDR((RNG_CHK_IP_RNGr)) + ipRangeW.idx*12, &regData);
         dataMask =  0x7;
         checkData = APOLLO_ACL_RNGIPTYPE_V6DIP;
         regData = regData & dataMask;
         if(regData != checkData)
         {
                 osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                 return RT_ERR_FAILED;
         }

    }

    for(pktLenRangeW.idx = 0; pktLenRangeW.idx <= APOLLO_ACLRANGEMAX2; pktLenRangeW.idx ++)
    {
        pktLenRangeW.upperPktLen = 0x3fff;
        pktLenRangeW.lowerPktLen = 0x2555;
        pktLenRangeW.type = APOLLO_ACL_RNGPKTLENTYPE_REVISE;
        ioal_mem32_write(HAL_GET_REG_ADDR(RNG_CHK_PKTLEN_RNGr) + pktLenRangeW.idx*4, 0x0);
        apollo_raw_acl_pktLenRange_set(&pktLenRangeW);
        ioal_mem32_read(HAL_GET_REG_ADDR((RNG_CHK_PKTLEN_RNGr)) + pktLenRangeW.idx*4, &regData);
        dataMask = 0x1fffffff;
        checkData = 0x1fffe555;
        regData = regData & dataMask;
        if(regData != checkData)
        {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
        }

    }
    modeW = APOLLO_ACL_MODE_128ENTRIES;
    ioal_mem32_write(HAL_GET_REG_ADDR(ACL_CFGr), 0x0);
    apollo_raw_acl_mode_set(modeW);
    ioal_mem32_read(HAL_GET_REG_ADDR((ACL_CFGr)), &regData);
    dataMask = 0x1;
    checkData = 0x1;
    regData = regData & dataMask;
    if(regData != checkData)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}

