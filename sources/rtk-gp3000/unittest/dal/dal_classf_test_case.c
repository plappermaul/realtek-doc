#include <osal/lib.h>
#include <osal/print.h>
#include <ioal/mem32.h>
#include <hal/common/halctrl.h>
#include <common/error.h>
#include <common/unittest_util.h>
#include <dal/apollomp/dal_apollomp_classify.h>
//#include <dal/apollo/dal_apollo_classify.h>
#include <rtk/classify.h>
#include <rtk/switch.h>
#include <dal/dal_classf_test_case.h>
#include <osal/memory.h>

typedef struct {
    rtk_classify_field_type_t type;
    uint16 value;
    uint16 mask;
}cf_range_field_t;

#if defined(CONFIG_SDK_APOLLOMP)
static cf_range_field_t outrange_field[] =
{
    {CLASSIFY_FIELD_TOS_DSIDX, 0xff+1, 0},
    {CLASSIFY_FIELD_TOS_DSIDX, 0, 0xff+1},
    {CLASSIFY_FIELD_TAG_VID, RTK_VLAN_ID_MAX+1, 0},
    {CLASSIFY_FIELD_TAG_VID, 0, RTK_VLAN_ID_MAX+1},
    {CLASSIFY_FIELD_TAG_PRI, RTK_DOT1P_PRIORITY_MAX+1, 0},
    {CLASSIFY_FIELD_TAG_PRI, 0, RTK_DOT1P_PRIORITY_MAX+1},
    {CLASSIFY_FIELD_INTER_PRI, RTK_DOT1P_PRIORITY_MAX+1, 0},
    {CLASSIFY_FIELD_INTER_PRI, 0, RTK_DOT1P_PRIORITY_MAX+1},
    {CLASSIFY_FIELD_IS_CTAG, 2, 0},
    {CLASSIFY_FIELD_IS_CTAG, 0, 2},
    {CLASSIFY_FIELD_IS_STAG, 2, 0},
    {CLASSIFY_FIELD_IS_STAG, 0, 2},
    {CLASSIFY_FIELD_UNI, 8, 0},
    {CLASSIFY_FIELD_UNI, 0, 8},

    {CLASSIFY_FIELD_PORT_RANGE, 0xf+1, 0},
    {CLASSIFY_FIELD_PORT_RANGE, 0, 0xf+1},
    {CLASSIFY_FIELD_IP_RANGE, 0xf+1, 0},
    {CLASSIFY_FIELD_IP_RANGE, 0, 0xf+1},
    {CLASSIFY_FIELD_ACL_HIT, 0xff+1, 0},
    {CLASSIFY_FIELD_ACL_HIT, 0, 0xff+1},
    {CLASSIFY_FIELD_WAN_IF, 8, 0},
    {CLASSIFY_FIELD_WAN_IF, 0, 8},
    {CLASSIFY_FIELD_IP6_MC, 2, 0},
    {CLASSIFY_FIELD_IP6_MC, 0, 2},
    {CLASSIFY_FIELD_IP4_MC, 2, 0},
    {CLASSIFY_FIELD_IP4_MC, 0, 2},
    {CLASSIFY_FIELD_MLD, 2, 0},
    {CLASSIFY_FIELD_MLD, 0, 2},
    {CLASSIFY_FIELD_IGMP, 2, 0},
    {CLASSIFY_FIELD_IGMP, 0, 2},
    {CLASSIFY_FIELD_DEI, RTK_DOT1P_DEI_MAX+1, 0},
    {CLASSIFY_FIELD_DEI, 0, RTK_DOT1P_DEI_MAX+1},
};
#elif defined(CONFIG_SDK_RTL9602C)
static cf_range_field_t outrange_field[] =
{
    {CLASSIFY_FIELD_TOS_DSIDX, 0xff+1, 0},
    {CLASSIFY_FIELD_TOS_DSIDX, 0, 0xff+1},
    {CLASSIFY_FIELD_TAG_VID, RTK_VLAN_ID_MAX+1, 0},
    {CLASSIFY_FIELD_TAG_VID, 0, RTK_VLAN_ID_MAX+1},
    {CLASSIFY_FIELD_TAG_PRI, RTK_DOT1P_PRIORITY_MAX+1, 0},
    {CLASSIFY_FIELD_TAG_PRI, 0, RTK_DOT1P_PRIORITY_MAX+1},
    {CLASSIFY_FIELD_INTER_PRI, RTK_DOT1P_PRIORITY_MAX+1, 0},
    {CLASSIFY_FIELD_INTER_PRI, 0, RTK_DOT1P_PRIORITY_MAX+1},
    {CLASSIFY_FIELD_IS_CTAG, 2, 0},
    {CLASSIFY_FIELD_IS_CTAG, 0, 2},
    {CLASSIFY_FIELD_IS_STAG, 2, 0},
    {CLASSIFY_FIELD_IS_STAG, 0, 2},
    {CLASSIFY_FIELD_UNI, 8, 0},
    {CLASSIFY_FIELD_UNI, 0, 8},

    {CLASSIFY_FIELD_DEI, RTK_DOT1P_DEI_MAX+1, 0},
    {CLASSIFY_FIELD_DEI, 0, RTK_DOT1P_DEI_MAX+1},
    {CLASSIFY_FIELD_STPID_SEL, 2, 0},
    {CLASSIFY_FIELD_STPID_SEL, 0, 2},

    /*value only uint16*/
    //{CLASSIFY_FIELD_ETHERTYPE, RTK_ETHERTYPE_MAX+1, 0},
    //{CLASSIFY_FIELD_ETHERTYPE, 0, RTK_ETHERTYPE_MAX+1},
    //{CLASSIFY_FIELD_INNER_VLAN, 0xFFFF+1, 0},
    //{CLASSIFY_FIELD_INNER_VLAN, 0, 0xFFFF+1},

    {CLASSIFY_FIELD_WAN_IF, 16, 0},
    {CLASSIFY_FIELD_WAN_IF, 0, 16},
    {CLASSIFY_FIELD_PPPOE, 2, 0},
    {CLASSIFY_FIELD_PPPOE, 0, 2},
    {CLASSIFY_FIELD_IPV6, 2, 0},
    {CLASSIFY_FIELD_IPV6, 0, 2},
    {CLASSIFY_FIELD_IPV4, 2, 0},
    {CLASSIFY_FIELD_IPV4, 0, 2},
    {CLASSIFY_FIELD_IPMC, 2, 0},
    {CLASSIFY_FIELD_IPMC, 0, 2},
    {CLASSIFY_FIELD_IGMP_MLD, 2, 0},
    {CLASSIFY_FIELD_IGMP_MLD, 0, 2},
    {CLASSIFY_FIELD_ACL_HIT, 0x7f+1, 0},
    {CLASSIFY_FIELD_ACL_HIT, 0, 0x7f+1},
    {CLASSIFY_FIELD_INTERNAL_VID_TAG_IF, 2, 0},
    {CLASSIFY_FIELD_INTERNAL_VID_TAG_IF, 0, 2},
    {CLASSIFY_FIELD_CF_ACL_HIT, 0x7f+1, 0},
    {CLASSIFY_FIELD_CF_ACL_HIT, 0, 0x7f+1},
};
#elif defined(CONFIG_SDK_RTL9607C)
static cf_range_field_t outrange_field[] =
{
    {CLASSIFY_FIELD_TOS_DSIDX, 0xff+1, 0},
    {CLASSIFY_FIELD_TOS_DSIDX, 0, 0xff+1},
    {CLASSIFY_FIELD_TAG_VID, RTK_VLAN_ID_MAX+1, 0},
    {CLASSIFY_FIELD_TAG_VID, 0, RTK_VLAN_ID_MAX+1},
    {CLASSIFY_FIELD_TAG_PRI, RTK_DOT1P_PRIORITY_MAX+1, 0},
    {CLASSIFY_FIELD_TAG_PRI, 0, RTK_DOT1P_PRIORITY_MAX+1},
    {CLASSIFY_FIELD_INTER_PRI, RTK_DOT1P_PRIORITY_MAX+1, 0},
    {CLASSIFY_FIELD_INTER_PRI, 0, RTK_DOT1P_PRIORITY_MAX+1},
    {CLASSIFY_FIELD_IS_CTAG, 2, 0},
    {CLASSIFY_FIELD_IS_CTAG, 0, 2},
    {CLASSIFY_FIELD_IS_STAG, 2, 0},
    {CLASSIFY_FIELD_IS_STAG, 0, 2},
    {CLASSIFY_FIELD_UNI, 16, 0},
    {CLASSIFY_FIELD_UNI, 0, 16},
    {CLASSIFY_FIELD_STPID_SEL, 2, 0},
    {CLASSIFY_FIELD_STPID_SEL, 0, 2},
    {CLASSIFY_FIELD_PPPOE, 2, 0},
    {CLASSIFY_FIELD_PPPOE, 0, 2},
    {CLASSIFY_FIELD_IPV6, 2, 0},
    {CLASSIFY_FIELD_IPV6, 0, 2},
    {CLASSIFY_FIELD_IPV4, 2, 0},
    {CLASSIFY_FIELD_IPV4, 0, 2},
    {CLASSIFY_FIELD_IPMC, 2, 0},
    {CLASSIFY_FIELD_IPMC, 0, 2},
    {CLASSIFY_FIELD_IGMP_MLD, 2, 0},
    {CLASSIFY_FIELD_IGMP_MLD, 0, 2},
    {CLASSIFY_FIELD_ACL_HIT, 0xff+1, 0},
    {CLASSIFY_FIELD_ACL_HIT, 0, 0xff+1},
};
#else
static cf_range_field_t outrange_field[] = {};
#endif

int32 dal_classf_rule_range_test(uint32 caseNo)
{
    rtk_classify_cfg_t      entry;
    rtk_classify_field_t    field;
    int32 ret;
    uint32 i, item_no=0;
#if defined(CONFIG_SDK_RTL9603CVD)
    if(UNITTEST_UTIL_CHIP_TYPE == RTL9603CVD_CHIP_ID)
    {
        osal_printf("\n this chip not support %s!!\n",__FUNCTION__);
        return RT_ERR_OK;
    }
#endif

    if((ret = rtk_classify_init()) != RT_ERR_OK)
    {
        osal_printf("\n rtk_classify_init fail! %s %d, ret = %d!!\n",__FUNCTION__,__LINE__, ret);
        return RT_ERR_FAILED;
    }

    osal_memset(&field, 0, sizeof(rtk_classify_field_t));
    osal_memset(&entry, 0, sizeof(rtk_classify_cfg_t));
    /* field outrange check */
    switch(UNITTEST_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            item_no = 14;
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            item_no = sizeof(outrange_field)/sizeof(cf_range_field_t);
            break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
            item_no = sizeof(outrange_field)/sizeof(cf_range_field_t);
            break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
            item_no = sizeof(outrange_field)/sizeof(cf_range_field_t);
            break;
#endif
        default:
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
    }

    for(i=0; i<item_no; i++)
    {
        //osal_printf("test item %d\n\r", i);
        osal_memset(&entry, 0, sizeof(rtk_classify_cfg_t));
        osal_memset(&field, 0, sizeof(rtk_classify_field_t));
        field.fieldType = outrange_field[i].type;
        field.classify_pattern.fieldData.value = outrange_field[i].value;
        field.classify_pattern.fieldData.mask = outrange_field[i].mask;
        if((ret = rtk_classify_field_add(&entry, &field)) != RT_ERR_OK)
        {
            osal_printf("\n add field NG: %s %d, ret=%d\n",__FUNCTION__,__LINE__,ret);
            return RT_ERR_FAILED;
        }
        if((ret = rtk_classify_cfgEntry_add(&entry)) == RT_ERR_OK)
        {
            osal_printf("\n outrange return OK: %s %d, ret=%d, type=%d, value=0x%x, mask=0x%x\n",__FUNCTION__,__LINE__,ret,
                        field.fieldType, field.classify_pattern.fieldData.value, field.classify_pattern.fieldData.mask);
            return RT_ERR_FAILED;
        }
    }
    osal_memset(&entry, 0, sizeof(rtk_classify_cfg_t));
    entry.direction = CLASSIFY_DIRECTION_END;
    if((ret = rtk_classify_cfgEntry_add(&entry)) == RT_ERR_OK)
    {
        osal_printf("\n outrange return OK: %s %d, ret=%d\n",__FUNCTION__,__LINE__,ret);
        return RT_ERR_FAILED;
    }
#if defined(CONFIG_SDK_APOLLOMP)
    if(APOLLOMP_CHIP_ID == UNITTEST_UTIL_CHIP_TYPE)
    {
        osal_memset(&entry, 0, sizeof(rtk_classify_cfg_t));
        entry.direction = CLASSIFY_DIRECTION_US;
        entry.invert = CLASSIFY_INVERT_END;
        if((ret = rtk_classify_cfgEntry_add(&entry)) == RT_ERR_OK)
        {
            osal_printf("\n outrange return OK: %s %d, ret=%d\n",__FUNCTION__,__LINE__,ret);
            return RT_ERR_FAILED;
        }
        osal_memset(&entry, 0, sizeof(rtk_classify_cfg_t));
        entry.direction = CLASSIFY_DIRECTION_DS;
        entry.invert = CLASSIFY_INVERT_END;
        if((ret = rtk_classify_cfgEntry_add(&entry)) == RT_ERR_OK)
        {
            osal_printf("\n outrange return OK: %s %d, ret=%d\n",__FUNCTION__,__LINE__,ret);
            return RT_ERR_FAILED;
        }
    }
#endif

    if((ret = rtk_classify_init()) != RT_ERR_OK)
    {
        osal_printf("\n rtk_classify_init fail! %s %d, ret = %d!!\n",__FUNCTION__,__LINE__, ret);
        return RT_ERR_FAILED;
    }

    osal_memset(&entry, 0, sizeof(rtk_classify_cfg_t));
    entry.index = HAL_CLASSIFY_ENTRY_MAX();
    if((ret = rtk_classify_cfgEntry_add(&entry)) == RT_ERR_OK)
    {
        osal_printf("\n outrange return OK: %s %d, ret=%d\n",__FUNCTION__,__LINE__,ret);
        return RT_ERR_FAILED;
    }
    osal_memset(&entry, 0, sizeof(rtk_classify_cfg_t));
    if((ret = rtk_classify_cfgEntry_add(&entry)) != RT_ERR_OK)
    {
        osal_printf("\n outrange return OK: %s %d, ret=%d\n",__FUNCTION__,__LINE__,ret);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32 dal_classf_usAct_range_test(uint32 caseNo)
{
    rtk_classify_cfg_t  entry;
    uint32              i, item_no=0;
    int32               ret;

    osal_memset(&entry, 0, sizeof(rtk_classify_cfg_t));
    entry.act.usAct.csAct = CLASSIFY_US_CSACT_END;
    if((ret = rtk_classify_cfgEntry_add(&entry)) == RT_ERR_OK)
    {
        osal_printf("\n outrange return OK: %s %d, ret=%d\n",__FUNCTION__,__LINE__,ret);
        return RT_ERR_FAILED;
    }
    osal_memset(&entry, 0, sizeof(rtk_classify_cfg_t));
    entry.act.usAct.csVidAct = CLASSIFY_US_VID_ACT_END;
    if((ret = rtk_classify_cfgEntry_add(&entry)) == RT_ERR_OK)
    {
        osal_printf("\n outrange return OK: %s %d, ret=%d\n",__FUNCTION__,__LINE__,ret);
        return RT_ERR_FAILED;
    }
    osal_memset(&entry, 0, sizeof(rtk_classify_cfg_t));
    entry.act.usAct.csPriAct = CLASSIFY_US_PRI_ACT_END;
    if((ret = rtk_classify_cfgEntry_add(&entry)) == RT_ERR_OK)
    {
        osal_printf("\n outrange return OK: %s %d, ret=%d\n",__FUNCTION__,__LINE__,ret);
        return RT_ERR_FAILED;
    }
    osal_memset(&entry, 0, sizeof(rtk_classify_cfg_t));
    entry.act.usAct.cAct = CLASSIFY_US_CACT_END;
    if((ret = rtk_classify_cfgEntry_add(&entry)) == RT_ERR_OK)
    {
        osal_printf("\n outrange return OK: %s %d, ret=%d\n",__FUNCTION__,__LINE__,ret);
        return RT_ERR_FAILED;
    }
    osal_memset(&entry, 0, sizeof(rtk_classify_cfg_t));
    entry.act.usAct.sidQidAct = CLASSIFY_US_SQID_ACT_END;
    if((ret = rtk_classify_cfgEntry_add(&entry)) == RT_ERR_OK)
    {
        osal_printf("\n outrange return OK: %s %d, ret=%d\n",__FUNCTION__,__LINE__,ret);
        return RT_ERR_FAILED;
    }
    osal_memset(&entry, 0, sizeof(rtk_classify_cfg_t));
    entry.act.usAct.sTagVid = RTK_VLAN_ID_MAX+1;
    if((ret = rtk_classify_cfgEntry_add(&entry)) == RT_ERR_OK)
    {
        osal_printf("\n outrange return OK: %s %d, ret=%d\n",__FUNCTION__,__LINE__,ret);
        return RT_ERR_FAILED;
    }
    osal_memset(&entry, 0, sizeof(rtk_classify_cfg_t));
    entry.act.usAct.sTagPri = RTK_DOT1P_PRIORITY_MAX+1;
    if((ret = rtk_classify_cfgEntry_add(&entry)) == RT_ERR_OK)
    {
        osal_printf("\n outrange return OK: %s %d, ret=%d\n",__FUNCTION__,__LINE__,ret);
        return RT_ERR_FAILED;
    }
    osal_memset(&entry, 0, sizeof(rtk_classify_cfg_t));
    entry.act.usAct.sidQid = HAL_CLASSIFY_SID_NUM();
    if((ret = rtk_classify_cfgEntry_add(&entry)) == RT_ERR_OK)
    {
        osal_printf("\n outrange return OK: %s %d, ret=%d\n",__FUNCTION__,__LINE__,ret);
        return RT_ERR_FAILED;
    }

#if defined(CONFIG_SDK_APOLLOMP)
    if(UNITTEST_UTIL_CHIP_TYPE == CONFIG_SDK_APOLLOMP)
    {
        osal_memset(&entry, 0, sizeof(rtk_classify_cfg_t));
        entry.act.usAct.cVidAct = CLASSIFY_US_VID_ACT_END;
        if((ret = rtk_classify_cfgEntry_add(&entry)) == RT_ERR_OK)
        {
            osal_printf("\n outrange return OK: %s %d, ret=%d\n",__FUNCTION__,__LINE__,ret);
            return RT_ERR_FAILED;
        }
        osal_memset(&entry, 0, sizeof(rtk_classify_cfg_t));
        entry.act.usAct.cPriAct = CLASSIFY_US_PRI_ACT_END;
        if((ret = rtk_classify_cfgEntry_add(&entry)) == RT_ERR_OK)
        {
            osal_printf("\n outrange return OK: %s %d, ret=%d\n",__FUNCTION__,__LINE__,ret);
            return RT_ERR_FAILED;
        }
        osal_memset(&entry, 0, sizeof(rtk_classify_cfg_t));
        entry.act.usAct.interPriAct = CLASSIFY_CF_PRI_ACT_END;
        if((ret = rtk_classify_cfgEntry_add(&entry)) == RT_ERR_OK)
        {
            osal_printf("\n outrange return OK: %s %d, ret=%d\n",__FUNCTION__,__LINE__,ret);
            return RT_ERR_FAILED;
        }
        osal_memset(&entry, 0, sizeof(rtk_classify_cfg_t));
        entry.act.usAct.dscp = CLASSIFY_DSCP_ACT_END;
        if((ret = rtk_classify_cfgEntry_add(&entry)) == RT_ERR_OK)
        {
            osal_printf("\n outrange return OK: %s %d, ret=%d\n",__FUNCTION__,__LINE__,ret);
            return RT_ERR_FAILED;
        }
        osal_memset(&entry, 0, sizeof(rtk_classify_cfg_t));
        entry.act.usAct.drop = CLASSIFY_DROP_ACT_END;
        if((ret = rtk_classify_cfgEntry_add(&entry)) == RT_ERR_OK)
        {
            osal_printf("\n outrange return OK: %s %d, ret=%d\n",__FUNCTION__,__LINE__,ret);
            return RT_ERR_FAILED;
        }
        osal_memset(&entry, 0, sizeof(rtk_classify_cfg_t));
        entry.act.usAct.log = CLASSIFY_US_LOG_ACT_END;
        if((ret = rtk_classify_cfgEntry_add(&entry)) == RT_ERR_OK)
        {
            osal_printf("\n outrange return OK: %s %d, ret=%d\n",__FUNCTION__,__LINE__,ret);
            return RT_ERR_FAILED;
        }
        osal_memset(&entry, 0, sizeof(rtk_classify_cfg_t));
        entry.act.usAct.cTagVid = RTK_VLAN_ID_MAX+1;
        if((ret = rtk_classify_cfgEntry_add(&entry)) == RT_ERR_OK)
        {
            osal_printf("\n outrange return OK: %s %d, ret=%d\n",__FUNCTION__,__LINE__,ret);
            return RT_ERR_FAILED;
        }
        osal_memset(&entry, 0, sizeof(rtk_classify_cfg_t));
        entry.act.usAct.cTagPri = RTK_DOT1P_PRIORITY_MAX+1;
        if((ret = rtk_classify_cfgEntry_add(&entry)) == RT_ERR_OK)
        {
            osal_printf("\n outrange return OK: %s %d, ret=%d\n",__FUNCTION__,__LINE__,ret);
            return RT_ERR_FAILED;
        }
        osal_memset(&entry, 0, sizeof(rtk_classify_cfg_t));
        entry.act.usAct.cfPri = RTK_DOT1P_PRIORITY_MAX+1;
        if((ret = rtk_classify_cfgEntry_add(&entry)) == RT_ERR_OK)
        {
            osal_printf("\n outrange return OK: %s %d, ret=%d\n",__FUNCTION__,__LINE__,ret);
            return RT_ERR_FAILED;
        }
        osal_memset(&entry, 0, sizeof(rtk_classify_cfg_t));
        entry.act.usAct.logCntIdx = HAL_MAX_NUM_OF_LOG_MIB();
        if((ret = rtk_classify_cfgEntry_add(&entry)) == RT_ERR_OK)
        {
            osal_printf("\n outrange return OK: %s %d, ret=%d\n",__FUNCTION__,__LINE__,ret);
            return RT_ERR_FAILED;
        }
    }
#endif

    return RT_ERR_OK;
}

int32 dal_classf_dsAct_range_test(uint32 caseNo)
{
    rtk_classify_cfg_t  entry;
    uint32              i, item_no=0;
    int32               ret;

#if defined(CONFIG_SDK_RTL9603CVD)
    if(UNITTEST_UTIL_CHIP_TYPE == RTL9603CVD_CHIP_ID)
    {
        osal_printf("\n this chip not support %s!!\n",__FUNCTION__);
        return RT_ERR_OK;
    }
#endif

    osal_memset(&entry, 0, sizeof(rtk_classify_cfg_t));
    entry.direction = CLASSIFY_DIRECTION_DS;
    entry.act.dsAct.csAct = CLASSIFY_DS_CSACT_END;
    if((ret = rtk_classify_cfgEntry_add(&entry)) == RT_ERR_OK)
    {
        osal_printf("\n outrange return OK: %s %d, ret=%d\n",__FUNCTION__,__LINE__,ret);
        return RT_ERR_FAILED;
    }
    osal_memset(&entry, 0, sizeof(rtk_classify_cfg_t));
    entry.direction = CLASSIFY_DIRECTION_DS;
    entry.act.dsAct.cAct = CLASSIFY_DS_CACT_END;
    if((ret = rtk_classify_cfgEntry_add(&entry)) == RT_ERR_OK)
    {
        osal_printf("\n outrange return OK: %s %d, ret=%d\n",__FUNCTION__,__LINE__,ret);
        return RT_ERR_FAILED;
    }
    osal_memset(&entry, 0, sizeof(rtk_classify_cfg_t));
    entry.direction = CLASSIFY_DIRECTION_DS;
    entry.act.dsAct.cVidAct = CLASSIFY_DS_VID_ACT_END;
    if((ret = rtk_classify_cfgEntry_add(&entry)) == RT_ERR_OK)
    {
        osal_printf("\n outrange return OK: %s %d, ret=%d\n",__FUNCTION__,__LINE__,ret);
        return RT_ERR_FAILED;
    }
    osal_memset(&entry, 0, sizeof(rtk_classify_cfg_t));
    entry.direction = CLASSIFY_DIRECTION_DS;
    entry.act.dsAct.cPriAct = CLASSIFY_DS_PRI_ACT_END;
    if((ret = rtk_classify_cfgEntry_add(&entry)) == RT_ERR_OK)
    {
        osal_printf("\n outrange return OK: %s %d, ret=%d\n",__FUNCTION__,__LINE__,ret);
        return RT_ERR_FAILED;
    }
    osal_memset(&entry, 0, sizeof(rtk_classify_cfg_t));
    entry.direction = CLASSIFY_DIRECTION_DS;
    entry.act.dsAct.interPriAct = CLASSIFY_CF_PRI_ACT_END;
    if((ret = rtk_classify_cfgEntry_add(&entry)) == RT_ERR_OK)
    {
        osal_printf("\n outrange return OK: %s %d, ret=%d\n",__FUNCTION__,__LINE__,ret);
        return RT_ERR_FAILED;
    }
    osal_memset(&entry, 0, sizeof(rtk_classify_cfg_t));
    entry.direction = CLASSIFY_DIRECTION_DS;
    entry.act.dsAct.uniAct = CLASSIFY_DS_UNI_ACT_END;
    if((ret = rtk_classify_cfgEntry_add(&entry)) == RT_ERR_OK)
    {
        osal_printf("\n outrange return OK: %s %d, ret=%d\n",__FUNCTION__,__LINE__,ret);
        return RT_ERR_FAILED;
    }
    osal_memset(&entry, 0, sizeof(rtk_classify_cfg_t));
    entry.direction = CLASSIFY_DIRECTION_DS;
    entry.act.dsAct.cTagVid = RTK_VLAN_ID_MAX+1;
    if((ret = rtk_classify_cfgEntry_add(&entry)) == RT_ERR_OK)
    {
        osal_printf("\n outrange return OK: %s %d, ret=%d\n",__FUNCTION__,__LINE__,ret);
        return RT_ERR_FAILED;
    }
    osal_memset(&entry, 0, sizeof(rtk_classify_cfg_t));
    entry.direction = CLASSIFY_DIRECTION_DS;
    entry.act.dsAct.cTagPri = RTK_DOT1P_PRIORITY_MAX+1;
    if((ret = rtk_classify_cfgEntry_add(&entry)) == RT_ERR_OK)
    {
        osal_printf("\n outrange return OK: %s %d, ret=%d\n",__FUNCTION__,__LINE__,ret);
        return RT_ERR_FAILED;
    }
    osal_memset(&entry, 0, sizeof(rtk_classify_cfg_t));
    entry.direction = CLASSIFY_DIRECTION_DS;
    entry.act.dsAct.cfPri = RTK_DOT1P_PRIORITY_MAX+1;
    if((ret = rtk_classify_cfgEntry_add(&entry)) == RT_ERR_OK)
    {
        osal_printf("\n outrange return OK: %s %d, ret=%d\n",__FUNCTION__,__LINE__,ret);
        return RT_ERR_FAILED;
    }

#if defined(CONFIG_SDK_APOLLOMP)
    if(UNITTEST_UTIL_CHIP_TYPE == CONFIG_SDK_APOLLOMP)
    {
        osal_memset(&entry, 0, sizeof(rtk_classify_cfg_t));
        entry.direction = CLASSIFY_DIRECTION_DS;
        entry.act.dsAct.csVidAct = CLASSIFY_DS_VID_ACT_END;
        if((ret = rtk_classify_cfgEntry_add(&entry)) == RT_ERR_OK)
        {
            osal_printf("\n outrange return OK: %s %d, ret=%d\n",__FUNCTION__,__LINE__,ret);
            return RT_ERR_FAILED;
        }
        osal_memset(&entry, 0, sizeof(rtk_classify_cfg_t));
        entry.direction = CLASSIFY_DIRECTION_DS;
        entry.act.dsAct.csPriAct = CLASSIFY_DS_PRI_ACT_END;
        if((ret = rtk_classify_cfgEntry_add(&entry)) == RT_ERR_OK)
        {
            osal_printf("\n outrange return OK: %s %d, ret=%d\n",__FUNCTION__,__LINE__,ret);
            return RT_ERR_FAILED;
        }
        osal_memset(&entry, 0, sizeof(rtk_classify_cfg_t));
        entry.direction = CLASSIFY_DIRECTION_DS;
        entry.act.dsAct.dscp = CLASSIFY_DSCP_ACT_END;
        if((ret = rtk_classify_cfgEntry_add(&entry)) == RT_ERR_OK)
        {
            osal_printf("\n outrange return OK: %s %d, ret=%d\n",__FUNCTION__,__LINE__,ret);
            return RT_ERR_FAILED;
        }
        osal_memset(&entry, 0, sizeof(rtk_classify_cfg_t));
        entry.direction = CLASSIFY_DIRECTION_DS;
        entry.act.dsAct.sTagVid = RTK_VLAN_ID_MAX+1;
        if((ret = rtk_classify_cfgEntry_add(&entry)) == RT_ERR_OK)
        {
            osal_printf("\n outrange return OK: %s %d, ret=%d\n",__FUNCTION__,__LINE__,ret);
            return RT_ERR_FAILED;
        }
        osal_memset(&entry, 0, sizeof(rtk_classify_cfg_t));
        entry.direction = CLASSIFY_DIRECTION_DS;
        entry.act.dsAct.sTagPri = RTK_DOT1P_PRIORITY_MAX+1;
        if((ret = rtk_classify_cfgEntry_add(&entry)) == RT_ERR_OK)
        {
            osal_printf("\n outrange return OK: %s %d, ret=%d\n",__FUNCTION__,__LINE__,ret);
            return RT_ERR_FAILED;
        }
    }
#endif

    return RT_ERR_OK;
}

int32 dal_classf_misc_range_test(uint32 caseNo)
{
    rtk_classify_unmatch_action_t action;
    rtk_classify_rangeCheck_l4Port_t port_range;
    rtk_classify_rangeCheck_ip_t     ip_range;
    rtk_port_t port;
    rtk_classify_cf_sel_t cf_sel;
    rtk_pri_t pri;
    rtk_dscp_t dscp;
    int32               ret;

#if defined(CONFIG_SDK_RTL9603CVD)
    if(UNITTEST_UTIL_CHIP_TYPE == RTL9603CVD_CHIP_ID)
    {
        osal_printf("\n this chip not support %s!!\n",__FUNCTION__);
        return RT_ERR_OK;
    }
#endif

    action = CLASSIFY_UNMATCH_END;
    if((ret = rtk_classify_unmatchAction_set(action)) == RT_ERR_OK)
    {
        osal_printf("\n outrange return OK: %s %d, ret=%d\n",__FUNCTION__,__LINE__,ret);
        return RT_ERR_FAILED;
    }

#if defined(CONFIG_SDK_APOLLOMP)
    if(UNITTEST_UTIL_CHIP_TYPE == CONFIG_SDK_APOLLOMP)
    {
        osal_memset(&port_range, 0, sizeof(rtk_classify_rangeCheck_l4Port_t));
        port_range.index = HAL_CLASSIFY_L4PORT_RANGE_NUM();
        if((ret = rtk_classify_portRange_set(&port_range)) == RT_ERR_OK)
        {
            osal_printf("\n outrange return OK: %s %d, ret=%d\n",__FUNCTION__,__LINE__,ret);
            return RT_ERR_FAILED;
        }
        osal_memset(&port_range, 0, sizeof(rtk_classify_rangeCheck_l4Port_t));
        port_range.type = CLASSIFY_PORTRANGE_END;
        if((ret = rtk_classify_portRange_set(&port_range)) == RT_ERR_OK)
        {
            osal_printf("\n outrange return OK: %s %d, ret=%d\n",__FUNCTION__,__LINE__,ret);
            return RT_ERR_FAILED;
        }
        osal_memset(&ip_range, 0, sizeof(rtk_classify_rangeCheck_ip_t));
        ip_range.index = HAL_CLASSIFY_IP_RANGE_NUM();
        if((ret = rtk_classify_ipRange_set(&ip_range)) == RT_ERR_OK)
        {
            osal_printf("\n outrange return OK: %s %d, ret=%d\n",__FUNCTION__,__LINE__,ret);
            return RT_ERR_FAILED;
        }
        osal_memset(&ip_range, 0, sizeof(rtk_classify_rangeCheck_ip_t));
        ip_range.type = CLASSIFY_IPRANGE_END;
        if((ret = rtk_classify_ipRange_set(&ip_range)) == RT_ERR_OK)
        {
            osal_printf("\n outrange return OK: %s %d, ret=%d\n",__FUNCTION__,__LINE__,ret);
            return RT_ERR_FAILED;
        }
        port = HAL_GET_CPU_PORT();
        cf_sel = CLASSIFY_CF_SEL_ENABLE;
        if((ret = rtk_classify_cfSel_set(port, cf_sel)) == RT_ERR_OK)
        {
            osal_printf("\n outrange return OK: %s %d, ret=%d\n",__FUNCTION__,__LINE__,ret);
            return RT_ERR_FAILED;
        }
        port = HAL_GET_PON_PORT();
        cf_sel = CLASSIFY_CF_SEL_END;
        if((ret = rtk_classify_cfSel_set(port, cf_sel)) == RT_ERR_OK)
        {
            osal_printf("\n outrange return OK: %s %d, ret=%d\n",__FUNCTION__,__LINE__,ret);
            return RT_ERR_FAILED;
        }
        pri = RTK_DOT1P_PRIORITY_MAX+1;
        dscp = 63;
        if((ret = rtk_classify_cfPri2Dscp_set(pri, dscp)) == RT_ERR_OK)
        {
            osal_printf("\n outrange return OK: %s %d, ret=%d\n",__FUNCTION__,__LINE__,ret);
            return RT_ERR_FAILED;
        }
        pri = 7;
        dscp = RTK_VALUE_OF_DSCP_MAX;
        if((ret = rtk_classify_cfPri2Dscp_set(pri, dscp)) == RT_ERR_OK)
        {
            osal_printf("\n outrange return OK: %s %d, ret=%d\n",__FUNCTION__,__LINE__,ret);
            return RT_ERR_FAILED;
        }
    }
#endif

    return RT_ERR_OK;
}

typedef struct {
    rtk_classify_dirct_t direct;
    rtk_classify_invert_t invert;
    rtk_enable_t valid;
}cf_rule_item_t;

static cf_rule_item_t rule_item[] ={
    {CLASSIFY_DIRECTION_US, CLASSIFY_INVERT_DISABLE, DISABLED},
    {CLASSIFY_DIRECTION_US, CLASSIFY_INVERT_ENABLE, ENABLED},
    {CLASSIFY_DIRECTION_US, CLASSIFY_INVERT_DISABLE, ENABLED},
    {CLASSIFY_DIRECTION_DS, CLASSIFY_INVERT_ENABLE, ENABLED},
    {CLASSIFY_DIRECTION_DS, CLASSIFY_INVERT_DISABLE, ENABLED},
};

typedef struct {
    rtk_classify_dirct_t      direct;
    cf_range_field_t          data;
    rtk_classify_raw_field_t  raw;
}cf_field_check_t;

#if defined(CONFIG_SDK_APOLLOMP)
static cf_field_check_t test_field[] =
{   /*direction           ,  field type              ,value, mask,  raw_data,   raw_mask*/
    {CLASSIFY_DIRECTION_US, {CLASSIFY_FIELD_ETHERTYPE, 0, 0xffff}, {{0, 0, 0}, {0, 0x8000, 0xffff}}},
    {CLASSIFY_DIRECTION_DS, {CLASSIFY_FIELD_ETHERTYPE, 0x1ff1, 0xffff}, {{0, 0x8000, 0x1ff1}, {0, 0x8000, 0xffff}}},
    {CLASSIFY_DIRECTION_US, {CLASSIFY_FIELD_ETHERTYPE, 0xffff, 0xffff}, {{0, 0, 0xffff}, {0, 0x8000, 0xffff}}},
    {CLASSIFY_DIRECTION_DS, {CLASSIFY_FIELD_TOS_DSIDX, 0, 0x7f}, {{0, 0x8000, 0}, {0, 0xbf80, 0}}},
    {CLASSIFY_DIRECTION_US, {CLASSIFY_FIELD_TOS_DSIDX, 0x40, 0x58}, {{0, 0x2000, 0}, {0, 0xac00, 0}}},
    {CLASSIFY_DIRECTION_DS, {CLASSIFY_FIELD_TOS_DSIDX, 0x7f, 0x7f}, {{0, 0xbf80, 0}, {0, 0xbf80, 0}}},
    {CLASSIFY_DIRECTION_US, {CLASSIFY_FIELD_TAG_VID, 0, 0xfff}, {{0, 0, 0}, {0xf800, 0x807f, 00}}},
    {CLASSIFY_DIRECTION_DS, {CLASSIFY_FIELD_TAG_VID, 0xf00, 0xff0}, {{0x0, 0x8078, 0}, {0x8000, 0x807f, 0}}},
    {CLASSIFY_DIRECTION_US, {CLASSIFY_FIELD_TAG_VID, RTK_VLAN_ID_MAX, 0xfff}, {{0xf800, 0x7f, 0}, {0xf800, 0x807f, 0}}},
    {CLASSIFY_DIRECTION_DS, {CLASSIFY_FIELD_TAG_PRI, 0, 0x7}, {{0, 0x8000, 0}, {0x700, 0x8000, 0x0}}},
    {CLASSIFY_DIRECTION_US, {CLASSIFY_FIELD_TAG_PRI, 5, 0x5}, {{0x500, 0, 0}, {0x500, 0x8000, 0}}},
    {CLASSIFY_DIRECTION_DS, {CLASSIFY_FIELD_TAG_PRI, RTK_DOT1P_PRIORITY_MAX, 0x7}, {{0x700, 0x8000, 0}, {0x700, 0x8000, 0}}},
    {CLASSIFY_DIRECTION_US, {CLASSIFY_FIELD_INTER_PRI, 0, 0x7}, {{0, 0, 0}, {0xe0, 0x8000, 0}}},
    {CLASSIFY_DIRECTION_DS, {CLASSIFY_FIELD_INTER_PRI, 6, 0x6}, {{0xc0, 0x8000, 0}, {0xc0, 0x8000, 0}}},
    {CLASSIFY_DIRECTION_US, {CLASSIFY_FIELD_INTER_PRI, RTK_DOT1P_PRIORITY_MAX, 0x7}, {{0xe0, 0, 0}, {0xe0, 0x8000, 0}}},
    {CLASSIFY_DIRECTION_DS, {CLASSIFY_FIELD_IS_CTAG, 0, 1}, {{0, 0x8000, 0}, {0x8, 0x8000, 0}}},
    {CLASSIFY_DIRECTION_US, {CLASSIFY_FIELD_IS_CTAG, 1, 1}, {{0x8, 0, 0}, {0x8, 0x8000, 0}}},
    {CLASSIFY_DIRECTION_DS, {CLASSIFY_FIELD_IS_CTAG, 1, 1}, {{0x8, 0x8000, 0}, {0x8, 0x8000, 0}}},
    {CLASSIFY_DIRECTION_US, {CLASSIFY_FIELD_IS_STAG, 0, 1}, {{0, 0, 0}, {0x10, 0x8000, 0}}},
    {CLASSIFY_DIRECTION_DS, {CLASSIFY_FIELD_IS_STAG, 1, 1}, {{0x10, 0x8000, 0}, {0x10, 0x8000, 0}}},
    {CLASSIFY_DIRECTION_US, {CLASSIFY_FIELD_IS_STAG, 1, 1}, {{0x10, 0, 0}, {0x10, 0x8000, 0}}},
    {CLASSIFY_DIRECTION_DS, {CLASSIFY_FIELD_UNI, 0, 7}, {{0, 0x8000, 0}, {0x7, 0x8000, 0}}},
    {CLASSIFY_DIRECTION_US, {CLASSIFY_FIELD_UNI, 4, 5}, {{0x4, 0, 0}, {0x5, 0x8000, 0}}},
    {CLASSIFY_DIRECTION_DS, {CLASSIFY_FIELD_UNI, 6, 7}, {{0x6, 0x8000, 0}, {0x7, 0x8000, 0}}},

    {CLASSIFY_DIRECTION_US, {CLASSIFY_FIELD_PORT_RANGE, 0, 0xf}, {{0, 0, 0}, {0, 0, 0}}},
    {CLASSIFY_DIRECTION_DS, {CLASSIFY_FIELD_PORT_RANGE, 9, 0xf}, {{0, 0, 0}, {0, 0, 0}}},
    {CLASSIFY_DIRECTION_US, {CLASSIFY_FIELD_PORT_RANGE, 0xf, 0xf}, {{0, 0, 0}, {0, 0, 0}}},
    {CLASSIFY_DIRECTION_DS, {CLASSIFY_FIELD_IP_RANGE, 0, 0xf}, {{0, 0, 0}, {0, 0, 0}}},
    {CLASSIFY_DIRECTION_US, {CLASSIFY_FIELD_IP_RANGE, 1, 0xf}, {{0, 0, 0}, {0, 0, 0}}},
    {CLASSIFY_DIRECTION_DS, {CLASSIFY_FIELD_IP_RANGE, 0xf, 0xf}, {{0, 0, 0}, {0, 0, 0}}},
    {CLASSIFY_DIRECTION_US, {CLASSIFY_FIELD_ACL_HIT, 0, 0xff}, {{0, 0, 0}, {0, 0, 0}}},
    {CLASSIFY_DIRECTION_DS, {CLASSIFY_FIELD_ACL_HIT, 0x8f, 0xff}, {{0, 0, 0}, {0, 0, 0}}},
    {CLASSIFY_DIRECTION_US, {CLASSIFY_FIELD_ACL_HIT, 0xff, 0xff}, {{0, 0, 0}, {0, 0, 0}}},
    {CLASSIFY_DIRECTION_DS, {CLASSIFY_FIELD_WAN_IF, 0, 7}, {{0, 0, 0}, {0, 0, 0}}},
    {CLASSIFY_DIRECTION_US, {CLASSIFY_FIELD_WAN_IF, 5, 6}, {{0, 0, 0}, {0, 0, 0}}},
    {CLASSIFY_DIRECTION_DS, {CLASSIFY_FIELD_WAN_IF, 7, 7}, {{0, 0, 0}, {0, 0, 0}}},
    {CLASSIFY_DIRECTION_US, {CLASSIFY_FIELD_IP6_MC, 0, 1}, {{0, 0, 0}, {0, 0, 0}}},
    {CLASSIFY_DIRECTION_DS, {CLASSIFY_FIELD_IP6_MC, 1, 0}, {{0, 0, 0}, {0, 0, 0}}},
    {CLASSIFY_DIRECTION_US, {CLASSIFY_FIELD_IP6_MC, 1, 1}, {{0, 0, 0}, {0, 0, 0}}},
    {CLASSIFY_DIRECTION_DS, {CLASSIFY_FIELD_IP4_MC, 0, 1}, {{0, 0, 0}, {0, 0, 0}}},
    {CLASSIFY_DIRECTION_US, {CLASSIFY_FIELD_IP4_MC, 1, 0}, {{0, 0, 0}, {0, 0, 0}}},
    {CLASSIFY_DIRECTION_DS, {CLASSIFY_FIELD_IP4_MC, 1, 1}, {{0, 0, 0}, {0, 0, 0}}},
    {CLASSIFY_DIRECTION_US, {CLASSIFY_FIELD_MLD, 0, 1}, {{0, 0, 0}, {0, 0, 0}}},
    {CLASSIFY_DIRECTION_DS, {CLASSIFY_FIELD_MLD, 1, 0}, {{0, 0, 0}, {0, 0, 0}}},
    {CLASSIFY_DIRECTION_US, {CLASSIFY_FIELD_MLD, 1, 1}, {{0, 0, 0}, {0, 0, 0}}},
    {CLASSIFY_DIRECTION_DS, {CLASSIFY_FIELD_IGMP, 0, 1}, {{0, 0, 0}, {0, 0, 0}}},
    {CLASSIFY_DIRECTION_US, {CLASSIFY_FIELD_IGMP, 1, 0}, {{0, 0, 0}, {0, 0, 0}}},
    {CLASSIFY_DIRECTION_DS, {CLASSIFY_FIELD_IGMP, 1, 1}, {{0, 0, 0}, {0, 0, 0}}},
    {CLASSIFY_DIRECTION_US, {CLASSIFY_FIELD_DEI, 0, 1}, {{0, 0, 0}, {0, 0, 0}}},
    {CLASSIFY_DIRECTION_DS, {CLASSIFY_FIELD_DEI, 1, 0}, {{0, 0, 0}, {0, 0, 0}}},
    {CLASSIFY_DIRECTION_US, {CLASSIFY_FIELD_DEI, 1, 1}, {{0, 0, 0}, {0, 0, 0}}},
};
#elif defined(CONFIG_SDK_RTL9602C)
static cf_field_check_t test_field[] =
{   /*direction           ,  field type              ,value, mask,  raw_data,   raw_mask*/
    {CLASSIFY_DIRECTION_US, {CLASSIFY_FIELD_ETHERTYPE, 0, 0xffff}, {{0, 0, 0}, {0, 0x8000, 0xffff}}},
    {CLASSIFY_DIRECTION_DS, {CLASSIFY_FIELD_ETHERTYPE, 0x1ff1, 0xffff}, {{0, 0x8000, 0x1ff1}, {0, 0x8000, 0xffff}}},
    {CLASSIFY_DIRECTION_US, {CLASSIFY_FIELD_ETHERTYPE, 0xffff, 0xffff}, {{0, 0, 0xffff}, {0, 0x8000, 0xffff}}},
    {CLASSIFY_DIRECTION_DS, {CLASSIFY_FIELD_TOS_DSIDX, 0, 0x7f}, {{0, 0x8000, 0}, {0, 0xbf80, 0}}},
    {CLASSIFY_DIRECTION_US, {CLASSIFY_FIELD_TOS_DSIDX, 0x40, 0x58}, {{0, 0x2000, 0}, {0, 0xac00, 0}}},
    {CLASSIFY_DIRECTION_DS, {CLASSIFY_FIELD_TOS_DSIDX, 0x7f, 0x7f}, {{0, 0xbf80, 0}, {0, 0xbf80, 0}}},
    {CLASSIFY_DIRECTION_US, {CLASSIFY_FIELD_TAG_VID, 0, 0xfff}, {{0, 0, 0}, {0xf800, 0x807f, 00}}},
    {CLASSIFY_DIRECTION_DS, {CLASSIFY_FIELD_TAG_VID, 0xf00, 0xff0}, {{0x0, 0x8078, 0}, {0x8000, 0x807f, 0}}},
    {CLASSIFY_DIRECTION_US, {CLASSIFY_FIELD_TAG_VID, RTK_VLAN_ID_MAX, 0xfff}, {{0xf800, 0x7f, 0}, {0xf800, 0x807f, 0}}},
    {CLASSIFY_DIRECTION_DS, {CLASSIFY_FIELD_TAG_PRI, 0, 0x7}, {{0, 0x8000, 0}, {0x700, 0x8000, 0x0}}},
    {CLASSIFY_DIRECTION_US, {CLASSIFY_FIELD_TAG_PRI, 5, 0x5}, {{0x500, 0, 0}, {0x500, 0x8000, 0}}},
    {CLASSIFY_DIRECTION_DS, {CLASSIFY_FIELD_TAG_PRI, RTK_DOT1P_PRIORITY_MAX, 0x7}, {{0x700, 0x8000, 0}, {0x700, 0x8000, 0}}},
    {CLASSIFY_DIRECTION_US, {CLASSIFY_FIELD_INTER_PRI, 0, 0x7}, {{0, 0, 0}, {0xe0, 0x8000, 0}}},
    {CLASSIFY_DIRECTION_DS, {CLASSIFY_FIELD_INTER_PRI, 6, 0x6}, {{0xc0, 0x8000, 0}, {0xc0, 0x8000, 0}}},
    {CLASSIFY_DIRECTION_US, {CLASSIFY_FIELD_INTER_PRI, RTK_DOT1P_PRIORITY_MAX, 0x7}, {{0xe0, 0, 0}, {0xe0, 0x8000, 0}}},
    {CLASSIFY_DIRECTION_DS, {CLASSIFY_FIELD_IS_CTAG, 0, 1}, {{0, 0x8000, 0}, {0x8, 0x8000, 0}}},
    {CLASSIFY_DIRECTION_US, {CLASSIFY_FIELD_IS_CTAG, 1, 1}, {{0x8, 0, 0}, {0x8, 0x8000, 0}}},
    {CLASSIFY_DIRECTION_DS, {CLASSIFY_FIELD_IS_CTAG, 1, 1}, {{0x8, 0x8000, 0}, {0x8, 0x8000, 0}}},
    {CLASSIFY_DIRECTION_US, {CLASSIFY_FIELD_IS_STAG, 0, 1}, {{0, 0, 0}, {0x10, 0x8000, 0}}},
    {CLASSIFY_DIRECTION_DS, {CLASSIFY_FIELD_IS_STAG, 1, 1}, {{0x10, 0x8000, 0}, {0x10, 0x8000, 0}}},
    {CLASSIFY_DIRECTION_US, {CLASSIFY_FIELD_IS_STAG, 1, 1}, {{0x10, 0, 0}, {0x10, 0x8000, 0}}},
    {CLASSIFY_DIRECTION_DS, {CLASSIFY_FIELD_UNI, 0, 7}, {{0, 0x8000, 0}, {0x7, 0x8000, 0}}},
    {CLASSIFY_DIRECTION_US, {CLASSIFY_FIELD_UNI, 4, 5}, {{0x4, 0, 0}, {0x5, 0x8000, 0}}},
    {CLASSIFY_DIRECTION_DS, {CLASSIFY_FIELD_UNI, 6, 7}, {{0x6, 0x8000, 0}, {0x7, 0x8000, 0}}},

    {CLASSIFY_DIRECTION_US, {CLASSIFY_FIELD_DEI, 0, 1}, {{0, 0, 0}, {0x0080, 0x8000, 0}}},
    {CLASSIFY_DIRECTION_DS, {CLASSIFY_FIELD_DEI, 1, 0}, {{0x0080, 0x8000, 0}, {0, 0x8000, 0}}},
    {CLASSIFY_DIRECTION_US, {CLASSIFY_FIELD_DEI, 1, 1}, {{0x0080, 0, 0}, {0x0080, 0x8000, 0}}},

    {CLASSIFY_DIRECTION_US, {CLASSIFY_FIELD_ACL_HIT, 0, 0xff}, {{0, 0, 0}, {0, 0, 0}}},
    {CLASSIFY_DIRECTION_DS, {CLASSIFY_FIELD_ACL_HIT, 0x8f, 0xff}, {{0, 0, 0}, {0, 0, 0}}},
    {CLASSIFY_DIRECTION_US, {CLASSIFY_FIELD_ACL_HIT, 0xff, 0xff}, {{0, 0, 0}, {0, 0, 0}}},
    {CLASSIFY_DIRECTION_DS, {CLASSIFY_FIELD_WAN_IF, 0, 7}, {{0, 0, 0}, {0, 0, 0}}},
    {CLASSIFY_DIRECTION_US, {CLASSIFY_FIELD_WAN_IF, 5, 6}, {{0, 0, 0}, {0, 0, 0}}},
    {CLASSIFY_DIRECTION_DS, {CLASSIFY_FIELD_WAN_IF, 7, 7}, {{0, 0, 0}, {0, 0, 0}}},
    {CLASSIFY_DIRECTION_US, {CLASSIFY_FIELD_IP6_MC, 0, 1}, {{0, 0, 0}, {0, 0, 0}}},
    {CLASSIFY_DIRECTION_DS, {CLASSIFY_FIELD_IP6_MC, 1, 0}, {{0, 0, 0}, {0, 0, 0}}},
    {CLASSIFY_DIRECTION_US, {CLASSIFY_FIELD_IP6_MC, 1, 1}, {{0, 0, 0}, {0, 0, 0}}},
    {CLASSIFY_DIRECTION_DS, {CLASSIFY_FIELD_IP4_MC, 0, 1}, {{0, 0, 0}, {0, 0, 0}}},
    {CLASSIFY_DIRECTION_US, {CLASSIFY_FIELD_IP4_MC, 1, 0}, {{0, 0, 0}, {0, 0, 0}}},
    {CLASSIFY_DIRECTION_DS, {CLASSIFY_FIELD_IP4_MC, 1, 1}, {{0, 0, 0}, {0, 0, 0}}},
    {CLASSIFY_DIRECTION_US, {CLASSIFY_FIELD_MLD, 0, 1}, {{0, 0, 0}, {0, 0, 0}}},
    {CLASSIFY_DIRECTION_DS, {CLASSIFY_FIELD_MLD, 1, 0}, {{0, 0, 0}, {0, 0, 0}}},
    {CLASSIFY_DIRECTION_US, {CLASSIFY_FIELD_MLD, 1, 1}, {{0, 0, 0}, {0, 0, 0}}},
    {CLASSIFY_DIRECTION_DS, {CLASSIFY_FIELD_IGMP, 0, 1}, {{0, 0, 0}, {0, 0, 0}}},
    {CLASSIFY_DIRECTION_US, {CLASSIFY_FIELD_IGMP, 1, 0}, {{0, 0, 0}, {0, 0, 0}}},
    {CLASSIFY_DIRECTION_DS, {CLASSIFY_FIELD_IGMP, 1, 1}, {{0, 0, 0}, {0, 0, 0}}},

};
#else
static cf_field_check_t test_field[] = {};
#endif

int32 dal_classf_rule_test(uint32 caseNo)
{
    rtk_classify_cfg_t  entry, r_entry;
    uint32              item, item_no;
    rtk_classify_field_t field;
    int32               ret;

#if defined(CONFIG_SDK_RTL9603CVD)
    if(UNITTEST_UTIL_CHIP_TYPE == RTL9603CVD_CHIP_ID)
    {
        osal_printf("\n this chip not support %s!!\n",__FUNCTION__);
        return RT_ERR_OK;
    }
#endif

    if((ret = rtk_classify_init()) != RT_ERR_OK)
    {
        osal_printf("\n rtk_classify_init fail! ret = %d, %s %d!!\n",ret,__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    for(item=0; item<(sizeof(rule_item)/sizeof(cf_rule_item_t)); item++)
    {
        osal_memset(&entry, 0, sizeof(rtk_classify_cfg_t));
        entry.index = item;
        entry.direction = rule_item[item].direct;
        entry.invert = rule_item[item].invert;
        entry.valid = rule_item[item].valid;
        if((ret = rtk_classify_cfgEntry_add(&entry)) != RT_ERR_OK)
        {
            osal_printf("\n rtk_classify_cfgEntry_add fail: %s %d, ret=%d, item=%d, dir=%d, inv=%d, valid=%d\n",__FUNCTION__,__LINE__,ret,
                        entry.index, entry.direction, entry.invert, entry.valid);
            return RT_ERR_FAILED;
        }
    }

    switch(UNITTEST_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            item_no = 24;
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            item_no = sizeof(test_field)/sizeof(cf_field_check_t);
            break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
            item_no = sizeof(test_field)/sizeof(cf_field_check_t);
            break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
            item_no = sizeof(test_field)/sizeof(cf_field_check_t);
            break;
#endif
        default:
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
    }

    for(item=0; item<item_no; item++)
    {
        osal_memset(&entry, 0, sizeof(rtk_classify_cfg_t));
        entry.index = item;
        entry.direction = test_field[item%item_no].direct;
        entry.invert = item%2;
        entry.valid = ENABLED;
        field.fieldType = test_field[item%item_no].data.type;
        field.classify_pattern.fieldData.value = test_field[item%item_no].data.value;
        field.classify_pattern.fieldData.mask = test_field[item%item_no].data.mask;
        if((ret = rtk_classify_field_add(&entry,&field)) != RT_ERR_OK)
        {
            osal_printf("\n outrange return OK: %s %d, ret=%d, item=%d, type=%d, value=%d, mask=%d\n",__FUNCTION__,__LINE__,ret,
                        item, field.fieldType, field.classify_pattern.fieldData.value, field.classify_pattern.fieldData.mask);
            return RT_ERR_FAILED;
        }
        if((ret = rtk_classify_cfgEntry_add(&entry)) != RT_ERR_OK)
        {
            osal_printf("\n outrange return OK: %s %d, ret=%d, item=%d, type=%d, value=%d, mask=%d\n",__FUNCTION__,__LINE__,ret,
                        item, field.fieldType, field.classify_pattern.fieldData.value, field.classify_pattern.fieldData.mask);
            return RT_ERR_FAILED;
        }

        r_entry.index = item;
        if((ret = rtk_classify_cfgEntry_get(&r_entry)) != RT_ERR_OK)
        {
            osal_printf("\n get rule fail: %s %d, ret=%d, item=%d\n",__FUNCTION__,__LINE__,ret,item);
            return RT_ERR_FAILED;
        }
        if(osal_memcmp(&r_entry.field.readField, &test_field[item%item_no].raw, sizeof(rtk_classify_raw_field_t)) != 0)
        {
            osal_printf("\n rule cmp fail: %s %d, ret=%d, item=%d, read data:0x%x-%x-%x, care:0x%x-%x-%x\n",__FUNCTION__,__LINE__,ret,item,
                        r_entry.field.readField.dataFieldRaw[0],r_entry.field.readField.dataFieldRaw[1],r_entry.field.readField.dataFieldRaw[2],
                        r_entry.field.readField.careFieldRaw[0],r_entry.field.readField.careFieldRaw[1],r_entry.field.readField.careFieldRaw[2]);
            osal_printf("\n write data:0x%x-%x-%x, care:0x%x-%x-%x\n",
                        test_field[item%item_no].raw.dataFieldRaw[0],test_field[item%item_no].raw.dataFieldRaw[1],test_field[item%item_no].raw.dataFieldRaw[2],
                        test_field[item%item_no].raw.careFieldRaw[0],test_field[item%item_no].raw.careFieldRaw[1],test_field[item%item_no].raw.careFieldRaw[2]);
            return RT_ERR_FAILED;
        }
    }

    return RT_ERR_OK;
}

#if 0
    rtk_classify_us_csact_t     csAct;
    rtk_classify_us_vid_act_t   csVidAct;
    rtk_classify_us_pri_act_t   csPriAct;
    rtk_classify_us_cact_t      cAct;
    rtk_classify_us_sqid_act_t  sidQidAct;
    rtk_classify_us_vid_act_t   cVidAct; /* not support in test chip */
    rtk_classify_us_pri_act_t   cPriAct; /* not support in test chip */
    rtk_classify_cf_pri_act_t   interPriAct; /* not support in test chip */
    rtk_classify_dscp_act_t     dscp; /* not support in test chip */
    rtk_classify_drop_act_t     drop; /* not support in test chip */
    rtk_classify_log_act_t      log; /* not support in test chip */
    uint32 sTagVid;
    uint8  sTagPri;
    uint32 sidQid;
    uint32 cTagVid; /* not support in test chip */
    uint8  cTagPri; /* not support in test chip */
    uint8  cfPri; /* not support in test chip */
    uint8  logCntIdx; /* not support in test chip */


    rtk_classify_ds_csact_t   csAct;
    rtk_classify_ds_vid_act_t csVidAct; /* not support in test chip */
    rtk_classify_ds_pri_act_t csPriAct; /* not support in test chip */
    rtk_classify_ds_cact_t    cAct;
    rtk_classify_ds_vid_act_t cVidAct;
    rtk_classify_ds_pri_act_t cPriAct;
    rtk_classify_cf_pri_act_t interPriAct;
    rtk_classify_ds_uni_act_t uniAct;
    rtk_classify_dscp_act_t dscp; /* not support in test chip */
    uint32 sTagVid; /* not support in test chip */
    uint8  sTagPri; /* not support in test chip */
    uint32 cTagVid;
    uint8  cTagPri;
    uint8  cfPri;
    uint32 uniMask;
#endif
#if defined(CONFIG_SDK_APOLLO)
static rtk_classify_us_act_t test_usAct[] = {
    {CLASSIFY_US_CSACT_NOP ,CLASSIFY_US_VID_ACT_ASSIGN, CLASSIFY_US_PRI_ACT_ASSIGN, CLASSIFY_US_CACT_NOP, CLASSIFY_US_SQID_ACT_ASSIGN_SID,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {CLASSIFY_US_CSACT_ADD_TAG_VS_TPID, CLASSIFY_US_VID_ACT_FROM_1ST_TAG, CLASSIFY_US_PRI_ACT_FROM_1ST_TAG, CLASSIFY_US_CACT_TRANSLATION_C2S, CLASSIFY_US_SQID_ACT_ASSIGN_QID,0,0,0,0,0,0,1111,1,11,0,0,0,0},
    {CLASSIFY_US_CSACT_ADD_TAG_8100, CLASSIFY_US_VID_ACT_ASSIGN, CLASSIFY_US_PRI_ACT_FROM_INTERNAL, CLASSIFY_US_CACT_DEL_CTAG, CLASSIFY_US_SQID_ACT_ASSIGN_SID,0,0,0,0,0,0,2222,2,22,0,0,0,0},
    {CLASSIFY_US_CSACT_ADD_TAG_VS_TPID, CLASSIFY_US_VID_ACT_ASSIGN, CLASSIFY_US_PRI_ACT_ASSIGN, CLASSIFY_US_CACT_TRANSPARENT, CLASSIFY_US_SQID_ACT_ASSIGN_SID,0,0,0,0,0,0,3333,3,33,0,0,0,0},
    {CLASSIFY_US_CSACT_ADD_TAG_8100, CLASSIFY_US_VID_ACT_ASSIGN, CLASSIFY_US_PRI_ACT_ASSIGN, CLASSIFY_US_CACT_DEL_CTAG, CLASSIFY_US_SQID_ACT_ASSIGN_SID,0,0,0,0,0,0,4000,4,44,0,0,0,0},
    {CLASSIFY_US_CSACT_ADD_TAG_VS_TPID, CLASSIFY_US_VID_ACT_ASSIGN, CLASSIFY_US_PRI_ACT_ASSIGN, CLASSIFY_US_CACT_DEL_CTAG, CLASSIFY_US_SQID_ACT_ASSIGN_SID,0,0,0,0,0,0,111,5,55,0,0,0,0},
    {CLASSIFY_US_CSACT_ADD_TAG_8100, CLASSIFY_US_VID_ACT_ASSIGN, CLASSIFY_US_PRI_ACT_ASSIGN, CLASSIFY_US_CACT_TRANSPARENT, CLASSIFY_US_SQID_ACT_ASSIGN_SID,0,0,0,0,0,0,222,6,66,0,0,0,0},
    {CLASSIFY_US_CSACT_ADD_TAG_VS_TPID, CLASSIFY_US_VID_ACT_ASSIGN, CLASSIFY_US_PRI_ACT_ASSIGN, CLASSIFY_US_CACT_TRANSPARENT, CLASSIFY_US_SQID_ACT_ASSIGN_SID,0,0,0,0,0,0,4095,7,77,0,0,0,0},
};
static rtk_classify_ds_act_t test_dsAct[] = {
    {CLASSIFY_DS_CSACT_NOP ,0, 0, CLASSIFY_DS_CACT_NOP, CLASSIFY_DS_VID_ACT_NOP, CLASSIFY_DS_PRI_ACT_NOP, CLASSIFY_CF_PRI_ACT_NOP, CLASSIFY_DS_UNI_ACT_MASK_BY_UNIMASK,0,0,0, 0,0,7,{{0}}},
    {CLASSIFY_DS_CSACT_ADD_TAG_VS_TPID, 0, 0, CLASSIFY_DS_CACT_ADD_CTAG_8100, CLASSIFY_DS_VID_ACT_ASSIGN, CLASSIFY_DS_PRI_ACT_ASSIGN, CLASSIFY_CF_PRI_ACT_ASSIGN, CLASSIFY_DS_UNI_ACT_MASK_BY_UNIMASK,0,0,0, 1111,1,6,{{0x11}}},
    {CLASSIFY_DS_CSACT_ADD_TAG_8100, 0, 0, CLASSIFY_DS_CACT_TRANSLATION_SP2C, CLASSIFY_DS_VID_ACT_FROM_1ST_TAG, CLASSIFY_DS_PRI_ACT_FROM_1ST_TAG, CLASSIFY_CF_PRI_ACT_NOP ,CLASSIFY_DS_UNI_ACT_MASK_BY_UNIMASK,0,0,0, 2222,2,5,{{0x22}}},
    {CLASSIFY_DS_CSACT_DEL_STAG, 0, 0, CLASSIFY_DS_CACT_TRANSPARENT, CLASSIFY_DS_VID_ACT_FROM_LUT, CLASSIFY_DS_PRI_ACT_FROM_INTERNAL, CLASSIFY_CF_PRI_ACT_ASSIGN, CLASSIFY_DS_UNI_ACT_FORCE_FORWARD,0,0,0, 3333,3,4,{{0x33}}},
    {CLASSIFY_DS_CSACT_ADD_TAG_8100, 0, 0, CLASSIFY_DS_CACT_NOP, CLASSIFY_DS_VID_ACT_FROM_LUT, CLASSIFY_DS_PRI_ACT_NOP, CLASSIFY_CF_PRI_ACT_ASSIGN, CLASSIFY_DS_UNI_ACT_FORCE_FORWARD,0,0,0, 4023,4,3,{{0x7}}},
    {CLASSIFY_DS_CSACT_ADD_TAG_VS_TPID, 0, 0, CLASSIFY_DS_CACT_ADD_CTAG_8100, CLASSIFY_DS_VID_ACT_ASSIGN, CLASSIFY_DS_PRI_ACT_ASSIGN, CLASSIFY_CF_PRI_ACT_ASSIGN, CLASSIFY_DS_UNI_ACT_FORCE_FORWARD,0,0,0, 111,5,2,{{0x34}}},
    {CLASSIFY_DS_CSACT_DEL_STAG, 0, 0, CLASSIFY_DS_CACT_TRANSPARENT, CLASSIFY_DS_VID_ACT_FROM_1ST_TAG, CLASSIFY_DS_PRI_ACT_FROM_1ST_TAG, CLASSIFY_CF_PRI_ACT_ASSIGN, CLASSIFY_DS_UNI_ACT_FORCE_FORWARD,0,0,0, 222,6,1,{{0x30}}},
    {CLASSIFY_DS_CSACT_ADD_TAG_VS_TPID, 0, 0, CLASSIFY_DS_CACT_TRANSLATION_SP2C, CLASSIFY_DS_VID_ACT_FROM_LUT, CLASSIFY_DS_PRI_ACT_FROM_INTERNAL, CLASSIFY_CF_PRI_ACT_NOP, CLASSIFY_DS_UNI_ACT_MASK_BY_UNIMASK,0,0,0, 333,7,0,{{0x37}}},
};
#else
static rtk_classify_us_act_t test_usAct[] = {};
static rtk_classify_ds_act_t test_dsAct[] = {};
#endif
int32 dal_classf_action_test(uint32 caseNo)
{
    rtk_classify_cfg_t  entry, r_entry;
    uint32 item;
    int32               ret;

#if defined(CONFIG_SDK_RTL9603CVD)
    if(UNITTEST_UTIL_CHIP_TYPE == RTL9603CVD_CHIP_ID)
    {
        osal_printf("\n this chip not support %s!!\n",__FUNCTION__);
        return RT_ERR_OK;
    }
#endif

    /* upstream action */
    for(item=0; item<(sizeof(test_usAct)/sizeof(rtk_classify_us_act_t)); item++)
    {
        osal_memset(&entry, 0, sizeof(rtk_classify_cfg_t));
        entry.index = item;
        entry.direction = CLASSIFY_DIRECTION_US;
        entry.act.usAct = test_usAct[item];
        /*osal_printf("\n item %d: %x-%x-%x-%x-%x-%x-%x-%x-%x-%x-%x-%x-%x-%x-%x-%x-%x-%x\n",item,
                        entry.act.usAct.csAct, entry.act.usAct.csVidAct, entry.act.usAct.csPriAct, entry.act.usAct.cAct, entry.act.usAct.sidQidAct, entry.act.usAct.cVidAct,
                        entry.act.usAct.cPriAct, entry.act.usAct.interPriAct, entry.act.usAct.dscp, entry.act.usAct.drop, entry.act.usAct.log, entry.act.usAct.sTagVid,
                        entry.act.usAct.sTagPri, entry.act.usAct.sidQid, entry.act.usAct.cTagVid, entry.act.usAct.cTagPri, entry.act.usAct.cfPri, entry.act.usAct.logCntIdx);
        */
        if((ret = rtk_classify_cfgEntry_add(&entry)) != RT_ERR_OK)
        {
            osal_printf("\n entry set NG: %s %d, ret=%d, item=%d \n",__FUNCTION__,__LINE__,ret,item);
            return RT_ERR_FAILED;
        }

        osal_memset(&r_entry, 0, sizeof(rtk_classify_cfg_t));
        r_entry.index = item;
        if((ret = rtk_classify_cfgEntry_get(&r_entry)) != RT_ERR_OK)
        {
            osal_printf("\n get entry fail: %s %d, ret=%d, item=%d\n",__FUNCTION__,__LINE__,ret,item);
            return RT_ERR_FAILED;
        }
        if(osal_memcmp(&r_entry.act.usAct, &test_usAct[item], sizeof(rtk_classify_us_act_t)) != 0)
        {
            osal_printf("\n rule cmp fail: %s %d, ret=%d, item=%d, read data:%x-%x-%x-%x-%x-%x-%x-%x-%x-%x-%x-%x-%x-%x-%x-%x-%x-%x\n",__FUNCTION__,__LINE__,ret,item,
                        r_entry.act.usAct.csAct, r_entry.act.usAct.csVidAct, r_entry.act.usAct.csPriAct, r_entry.act.usAct.cAct, r_entry.act.usAct.sidQidAct, r_entry.act.usAct.cVidAct,
                        r_entry.act.usAct.cPriAct, r_entry.act.usAct.interPriAct, r_entry.act.usAct.dscp, r_entry.act.usAct.drop, r_entry.act.usAct.log, r_entry.act.usAct.sTagVid,
                        r_entry.act.usAct.sTagPri, r_entry.act.usAct.sidQid, r_entry.act.usAct.cTagVid, r_entry.act.usAct.cTagPri, r_entry.act.usAct.cfPri, r_entry.act.usAct.logCntIdx);
            osal_printf("\n write data:%x-%x-%x-%x-%x-%x-%x-%x-%x-%x-%x-%x-%x-%x-%x-%x-%x-%x\n",
                        test_usAct[item].csAct, test_usAct[item].csVidAct, test_usAct[item].csPriAct, test_usAct[item].cAct, test_usAct[item].sidQidAct, test_usAct[item].cVidAct,
                        test_usAct[item].cPriAct, test_usAct[item].interPriAct, test_usAct[item].dscp, test_usAct[item].drop, test_usAct[item].log, test_usAct[item].sTagVid,
                        test_usAct[item].sTagPri, test_usAct[item].sidQid, test_usAct[item].cTagVid, test_usAct[item].cTagPri, test_usAct[item].cfPri, test_usAct[item].logCntIdx);
            return RT_ERR_FAILED;
        }
    }

    /* dpstream action */
    for(item=0; item<(sizeof(test_dsAct)/sizeof(rtk_classify_ds_act_t)); item++)
    {
        osal_memset(&entry, 0, sizeof(rtk_classify_cfg_t));
        entry.index = item;
        entry.direction = CLASSIFY_DIRECTION_DS;
        entry.act.dsAct = test_dsAct[item];
        /*osal_printf("\n item %d: %x-%x-%x-%x-%x-%x-%x-%x-%x-%x-%x-%x-%x-%x-%x\n",item,
                        entry.act.dsAct.csAct, entry.act.dsAct.csVidAct, entry.act.dsAct.csPriAct, entry.act.dsAct.cAct, entry.act.dsAct.cVidAct,
                        entry.act.dsAct.cPriAct, entry.act.dsAct.interPriAct, entry.act.dsAct.uniAct, entry.act.dsAct.dscp, entry.act.dsAct.sTagVid,
                        entry.act.dsAct.sTagPri, entry.act.dsAct.cTagVid, entry.act.dsAct.cTagPri, entry.act.dsAct.cfPri, entry.act.dsAct.uniMask);
        */
        if((ret = rtk_classify_cfgEntry_add(&entry)) != RT_ERR_OK)
        {
            osal_printf("\n entry set NG: %s %d, ret=%d, item=%d \n",__FUNCTION__,__LINE__,ret,item);
            return RT_ERR_FAILED;
        }

        osal_memset(&r_entry, 0, sizeof(rtk_classify_cfg_t));
        r_entry.index = item;
        if((ret = rtk_classify_cfgEntry_get(&r_entry)) != RT_ERR_OK)
        {
            osal_printf("\n get entry fail: %s %d, ret=%d, item=%d\n",__FUNCTION__,__LINE__,ret,item);
            return RT_ERR_FAILED;
        }
        if(osal_memcmp(&r_entry.act.dsAct, &test_dsAct[item], sizeof(rtk_classify_ds_act_t)) != 0)
        {
            osal_printf("\n rule cmp fail: %s %d, ret=%d, item=%d, read data:%x-%x-%x-%x-%x-%x-%x-%x-%x-%x-%x-%x-%x-%x-%x\n",__FUNCTION__,__LINE__,ret,item,
                        r_entry.act.dsAct.csAct, r_entry.act.dsAct.csVidAct, r_entry.act.dsAct.csPriAct, r_entry.act.dsAct.cAct, r_entry.act.dsAct.cVidAct,
                        r_entry.act.dsAct.cPriAct, r_entry.act.dsAct.interPriAct, r_entry.act.dsAct.uniAct, r_entry.act.dsAct.dscp, r_entry.act.dsAct.sTagVid,
                        r_entry.act.dsAct.sTagPri, r_entry.act.dsAct.cTagVid, r_entry.act.dsAct.cTagPri, r_entry.act.dsAct.cfPri, r_entry.act.dsAct.uniMask.bits[0]);
            osal_printf("\n write data:%x-%x-%x-%x-%x-%x-%x-%x-%x-%x-%x-%x-%x-%x-%x\n",
                        test_dsAct[item].csAct, test_dsAct[item].csVidAct, test_dsAct[item].csPriAct, test_dsAct[item].cAct, test_dsAct[item].cVidAct,
                        test_dsAct[item].cPriAct, test_dsAct[item].interPriAct, test_dsAct[item].uniAct, test_dsAct[item].dscp, test_dsAct[item].sTagVid,
                        test_dsAct[item].sTagPri, test_dsAct[item].cTagVid, test_dsAct[item].cTagPri, test_dsAct[item].cfPri, test_dsAct[item].uniMask.bits[0]);
            return RT_ERR_FAILED;
        }
    }

    return RT_ERR_OK;
}

typedef struct {
    rtk_classify_field_t      field;
    rtk_classify_raw_field_t  raw;
}cf_field_entry_t;

#if defined(CONFIG_SDK_APOLLOMP)
static cf_field_entry_t entry_field[][9] =
{   /*field type               ,value, mask,      raw_data,              raw_mask*/
    {{{CLASSIFY_FIELD_ETHERTYPE, {{0xffff, 0xffff}}}, {{0, 0x8000, 0xffff}, {0, 0x8000, 0xffff}}},
    {{CLASSIFY_FIELD_TOS_DSIDX, {{0x7f, 0x7f}}}, {{0, 0xbf80, 0}, {0, 0xbf80, 0}}},
    {{CLASSIFY_FIELD_TAG_VID, {{RTK_VLAN_ID_MAX, 0xfff}}}, {{0xf800, 0x807f, 0}, {0xf800, 0x807f, 0}}},
    {{CLASSIFY_FIELD_TAG_PRI, {{RTK_DOT1P_PRIORITY_MAX, 0x7}}}, {{0x700, 0x8000, 0}, {0x700, 0x8000, 0}}},
    {{CLASSIFY_FIELD_INTER_PRI, {{RTK_DOT1P_PRIORITY_MAX, 0x7}}}, {{0xe0, 0x8000, 0}, {0xe0, 0x8000, 0}}},
    {{CLASSIFY_FIELD_IS_CTAG, {{1, 1}}}, {{0x8, 0x8000, 0}, {0x8, 0x8000, 0}}},
    {{CLASSIFY_FIELD_IS_STAG, {{1, 1}}}, {{0x10, 0x8000, 0}, {0x10, 0x8000, 0}}},
    {{CLASSIFY_FIELD_UNI, {{6, 7}}}, {{0x6, 0x8000, 0}, {0x7, 0x8000, 0}}},
    {{CLASSIFY_FIELD_END, {{0, 0}}}, {{0, 0, 0}, {0, 0, 0}}}},
};

static rtk_classify_ds_act_t entry_dsAct[] = {
    {CLASSIFY_DS_CSACT_ADD_TAG_8100 ,0, 0, CLASSIFY_DS_CACT_TRANSLATION_SP2C, CLASSIFY_DS_VID_ACT_ASSIGN, CLASSIFY_DS_PRI_ACT_FROM_1ST_TAG, CLASSIFY_CF_PRI_ACT_ASSIGN, CLASSIFY_DS_UNI_ACT_FORCE_FORWARD,0,0,0, 1234,2,6,{{0x37}}},
};

#elif defined(CONFIG_SDK_RTL9602C)
static cf_field_entry_t entry_field[][16] =
{   /*Pattern 0*/
    /*field type               ,value, mask,      raw_data,              raw_mask*/
    {{{CLASSIFY_FIELD_WAN_IF, {{0xf, 0xf}}}, {{0, 0x8000, 0xf000}, {0, 0x8000, 0xf000}}},
    {{CLASSIFY_FIELD_PPPOE, {{1, 1}}}, {{0, 0x8000, 0x0800}, {0, 0x8000, 0x0800}}},
    {{CLASSIFY_FIELD_IPV4, {{1, 1}}}, {{0, 0x8000, 0x0400}, {0, 0x8000, 0x0400}}},
    {{CLASSIFY_FIELD_IPV6, {{1, 1}}}, {{0, 0x8000, 0x0200}, {0, 0x8000, 0x0200}}},
    {{CLASSIFY_FIELD_IPMC, {{1, 1}}}, {{0, 0x8000, 0x0100}, {0, 0x8000, 0x0100}}},
    {{CLASSIFY_FIELD_IGMP_MLD, {{1, 1}}}, {{0, 0x8000, 0x0080}, {0, 0x8000, 0x0080}}},
    {{CLASSIFY_FIELD_ACL_HIT, {{0x7f, 0x7f}}}, {{0, 0x8000, 0x007f}, {0, 0x8000, 0x007f}}},
    {{CLASSIFY_FIELD_INTERNAL_VID_TAG_IF, {{1, 1}}}, {{0, 0xC000, 0}, {0, 0xC000, 0}}},
    {{CLASSIFY_FIELD_CF_ACL_HIT, {{0x7f, 0x7f}}}, {{0, 0xBF80, 0}, {0, 0xBF80, 0}}},
    {{CLASSIFY_FIELD_TAG_VID, {{RTK_VLAN_ID_MAX, 0xfff}}}, {{0xf800, 0x807f, 0}, {0xf800, 0x807f, 0}}},
    {{CLASSIFY_FIELD_TAG_PRI, {{RTK_DOT1P_PRIORITY_MAX, 0x7}}}, {{0x700, 0x8000, 0}, {0x700, 0x8000, 0}}},
    {{CLASSIFY_FIELD_INTER_PRI, {{RTK_DOT1P_PRIORITY_MAX, 0x7}}}, {{0xe0, 0x8000, 0}, {0xe0, 0x8000, 0}}},
    {{CLASSIFY_FIELD_IS_CTAG, {{1, 1}}}, {{0x8, 0x8000, 0}, {0x8, 0x8000, 0}}},
    {{CLASSIFY_FIELD_IS_STAG, {{1, 1}}}, {{0x10, 0x8000, 0}, {0x10, 0x8000, 0}}},
    {{CLASSIFY_FIELD_UNI, {{6, 7}}}, {{0x6, 0x8000, 0}, {0x7, 0x8000, 0}}},
    {{CLASSIFY_FIELD_END, {{0, 0}}}, {{0, 0, 0}, {0, 0, 0}}}},

    /*Pattern 1*/
    /*field type               ,value, mask,      raw_data,              raw_mask*/
    {{{CLASSIFY_FIELD_ETHERTYPE, {{0x8177, 0xffff}}}, {{0, 0x8000, 0x8177}, {0, 0x8000, 0xffff}}},
    {{CLASSIFY_FIELD_TOS_DSIDX, {{0x3f, 0x7f}}}, {{0, 0x9f80, 0}, {0, 0xbf80, 0}}},
    {{CLASSIFY_FIELD_TAG_VID, {{RTK_VLAN_ID_MAX, 0xfff}}}, {{0xf800, 0x807f, 0}, {0xf800, 0x807f, 0}}},
    {{CLASSIFY_FIELD_TAG_PRI, {{RTK_DOT1P_PRIORITY_MAX, 0x7}}}, {{0x700, 0x8000, 0}, {0x700, 0x8000, 0}}},
    {{CLASSIFY_FIELD_INTER_PRI, {{RTK_DOT1P_PRIORITY_MAX, 0x7}}}, {{0xe0, 0x8000, 0}, {0xe0, 0x8000, 0}}},
    {{CLASSIFY_FIELD_IS_CTAG, {{1, 1}}}, {{0x8, 0x8000, 0}, {0x8, 0x8000, 0}}},
    {{CLASSIFY_FIELD_IS_STAG, {{1, 1}}}, {{0x10, 0x8000, 0}, {0x10, 0x8000, 0}}},
    {{CLASSIFY_FIELD_UNI, {{6, 7}}}, {{0x6, 0x8000, 0}, {0x7, 0x8000, 0}}},
    {{CLASSIFY_FIELD_END, {{0, 0}}}, {{0, 0, 0}, {0, 0, 0}}}},
};

static rtk_classify_ds_act_t entry_dsAct[] = {
    {CLASSIFY_DS_CSACT_ADD_TAG_VS_TPID ,CLASSIFY_DS_VID_ACT_ASSIGN, CLASSIFY_DS_PRI_ACT_ASSIGN,
     CLASSIFY_DS_CACT_ADD_CTAG_8100, CLASSIFY_DS_VID_ACT_ASSIGN, CLASSIFY_DS_PRI_ACT_FROM_1ST_TAG,
     CLASSIFY_CF_PRI_ACT_ASSIGN, CLASSIFY_DS_UNI_ACT_FORCE_FORWARD,CLASSIFY_DSCP_ACT_ENABLE,
     1122, 5, 1234, 2, 6,{{0xf}}, 50},
};
#elif defined(CONFIG_SDK_RTL9607C)
static cf_field_entry_t entry_field[][16] =
{   /*Pattern 0*/
    /*field type               ,value, mask,      raw_data,              raw_mask*/
    {{{CLASSIFY_FIELD_PPPOE, {{1, 1}}}, {{0, 0x8000, 0x1000}, {0, 0x8000, 0x1000}}},
    {{CLASSIFY_FIELD_IPV4, {{1, 1}}}, {{0, 0x8000, 0x0800}, {0, 0x8000, 0x0800}}},
    {{CLASSIFY_FIELD_IPV6, {{1, 1}}}, {{0, 0x8000, 0x0400}, {0, 0x8000, 0x04200}}},
    {{CLASSIFY_FIELD_IPMC, {{1, 1}}}, {{0, 0x8000, 0x0200}, {0, 0x8000, 0x0200}}},
    {{CLASSIFY_FIELD_IGMP_MLD, {{1, 1}}}, {{0, 0x8000, 0x0100}, {0, 0x8000, 0x0100}}},
    {{CLASSIFY_FIELD_ACL_HIT, {{0x7f, 0x7f}}}, {{0, 0x8000, 0x00ff}, {0, 0x8000, 0x00ff}}},
    {{CLASSIFY_FIELD_TOS_DSIDX, {{0x3f, 0xff}}}, {{0, 0x9f80, 0}, {0, 0xff80, 0}}},
    {{CLASSIFY_FIELD_TAG_VID, {{RTK_VLAN_ID_MAX, 0xfff}}}, {{0xff80, 0x8007, 0}, {0xff80, 0x8007, 0}}},
    {{CLASSIFY_FIELD_TAG_PRI, {{RTK_DOT1P_PRIORITY_MAX, 0x7}}}, {{0, 0x8070, 0}, {0, 0x8070, 0}}},
    {{CLASSIFY_FIELD_INTER_PRI, {{RTK_DOT1P_PRIORITY_MAX, 0x7}}}, {{0x0, 0x8000, 0xe000}, {0x0, 0x8000, 0xe000}}},
    {{CLASSIFY_FIELD_IS_CTAG, {{1, 1}}}, {{0x10, 0x8000, 0}, {0x10, 0x8000, 0}}},
    {{CLASSIFY_FIELD_IS_STAG, {{1, 1}}}, {{0x20, 0x8000, 0}, {0x20, 0x8000, 0}}},
    {{CLASSIFY_FIELD_UNI, {{6, 0xf}}}, {{0x6, 0x8000, 0}, {0xf, 0x8000, 0}}},
    {{CLASSIFY_FIELD_END, {{0, 0}}}, {{0, 0, 0}, {0, 0, 0}}}},

    /*Pattern 1*/
    /*field type               ,value, mask,      raw_data,              raw_mask*/
    {{{CLASSIFY_FIELD_ETHERTYPE, {{0x8177, 0xffff}}}, {{0, 0x8000, 0x8177}, {0, 0x8000, 0xffff}}},
    {{CLASSIFY_FIELD_TOS_DSIDX, {{0x3f, 0xff}}}, {{0, 0x9f80, 0}, {0, 0xff80, 0}}},
    {{CLASSIFY_FIELD_TAG_VID, {{RTK_VLAN_ID_MAX, 0xfff}}}, {{0xff80, 0x8007, 0}, {0xff80, 0x8007, 0}}},
    {{CLASSIFY_FIELD_TAG_PRI, {{RTK_DOT1P_PRIORITY_MAX, 0x7}}}, {{0, 0x8070, 0}, {0, 0x8070, 0}}},
    {{CLASSIFY_FIELD_INTER_PRI, {{RTK_DOT1P_PRIORITY_MAX, 0x7}}}, {{0x0, 0x8000, 0xe000}, {0x0, 0x8000, 0xe000}}},
    {{CLASSIFY_FIELD_IS_CTAG, {{1, 1}}}, {{0x10, 0x8000, 0}, {0x10, 0x8000, 0}}},
    {{CLASSIFY_FIELD_IS_STAG, {{1, 1}}}, {{0x20, 0x8000, 0}, {0x20, 0x8000, 0}}},
    {{CLASSIFY_FIELD_UNI, {{6, 0xf}}}, {{0x6, 0x8000, 0}, {0xf, 0x8000, 0}}},
    {{CLASSIFY_FIELD_END, {{0, 0}}}, {{0, 0, 0}, {0, 0, 0}}}},
};

static rtk_classify_ds_act_t entry_dsAct[] = {
    {CLASSIFY_DS_CSACT_ADD_TAG_VS_TPID ,CLASSIFY_DS_VID_ACT_ASSIGN, CLASSIFY_DS_PRI_ACT_ASSIGN,
     CLASSIFY_DS_CACT_ADD_CTAG_8100, CLASSIFY_DS_VID_ACT_ASSIGN, CLASSIFY_DS_PRI_ACT_FROM_1ST_TAG,
     CLASSIFY_CF_PRI_ACT_ASSIGN, CLASSIFY_DS_UNI_ACT_FORCE_FORWARD,0,
     1122, 5, 1234, 2, 6,{{0xf}}, 0},
};
#else
static cf_field_entry_t entry_field[][1] =
{
    {{{CLASSIFY_FIELD_END, {{0, 0}}}, {{0, 0, 0}, {0, 0, 0}}}},
};
static rtk_classify_ds_act_t entry_dsAct[] = {};
#endif



int32 dal_classf_entry_test(uint32 caseNo)
{
    rtk_classify_cfg_t  entry, r_entry;
    rtk_classify_raw_field_t readField;
    uint32 item, item_no, i;
    uint32 pattern, pattern_no, entry_start;
    int32  ret;

#if defined(CONFIG_SDK_RTL9603CVD)
    if(UNITTEST_UTIL_CHIP_TYPE == RTL9603CVD_CHIP_ID)
    {
        osal_printf("\n this chip not support %s!!\n",__FUNCTION__);
        return RT_ERR_OK;
    }
#endif

    if((ret = rtk_classify_init()) != RT_ERR_OK)
    {
        osal_printf("\n rtk_classify_init fail! %s %d, ret = %d!!\n",__FUNCTION__,__LINE__, ret);
        return RT_ERR_FAILED;
    }

    pattern_no = sizeof(entry_field)/sizeof(entry_field[0]);
    item_no = (sizeof(entry_field[0])/sizeof(cf_field_entry_t));

    for(pattern = 0; pattern < pattern_no; pattern++)
    {
        osal_memset(&readField, 0, sizeof(rtk_classify_raw_field_t));
        osal_memset(&entry, 0, sizeof(rtk_classify_cfg_t));


        switch(UNITTEST_UTIL_CHIP_TYPE)
        {
#if defined(CONFIG_SDK_RTL9602C)
            case RTL9602C_CHIP_ID:
                entry.templateIdx = 0;
                if(0 == pattern)
                {
                    rtk_classify_entryNumPattern1_set(0);
                    entry_start = 0;
                } else {
                    rtk_classify_entryNumPattern1_set(255);
                    entry_start = 1;
                }
            break;
#endif
            default:
            break;
        }

        entry.direction = CLASSIFY_DIRECTION_DS;
        entry.act.dsAct = entry_dsAct[0];
        entry.invert = CLASSIFY_INVERT_DISABLE;
        entry.valid = ENABLED;

        for(item=0; item<item_no; item++)
        {
            for(i=0; i<CLASSIFY_RAW_FIELD_NUMBER; i++)
            {
                readField.dataFieldRaw[i] |= entry_field[pattern][item].raw.dataFieldRaw[i];
                readField.careFieldRaw[i] |= entry_field[pattern][item].raw.careFieldRaw[i];
                /*
                osal_printf("\n\ritem %d, data[%d]=%x (%x), care[%d]=%x (%x)",item,
                                                                            i, readField.dataFieldRaw[i], entry_field[pattern][item].raw.dataFieldRaw[i],
                                                                            i, readField.careFieldRaw[i], entry_field[pattern][item].raw.careFieldRaw[i]);
                */
            }

            if((CLASSIFY_FIELD_END == entry_field[pattern][item+1].field.fieldType) || (item == (item_no-1)))
            {
                entry_field[pattern][item].field.next = NULL;
                break;
            } else {
                entry_field[pattern][item].field.next = &entry_field[pattern][item+1].field;
            }
        }
        entry.field.pFieldHead = &entry_field[pattern][0].field;

        /* Add entries */
        for(item = entry_start; item<HAL_CLASSIFY_ENTRY_MAX(); item++)
        {
            entry.index = item;

            if((ret = rtk_classify_cfgEntry_add(&entry)) != RT_ERR_OK)
            {
                osal_printf("\n entry set NG: %s %d, ret=%d, item=%d \n",__FUNCTION__,__LINE__,ret,item);
                return RT_ERR_FAILED;
            }
        }

        /*Get entries*/
        for(item=entry_start; item<HAL_CLASSIFY_ENTRY_MAX(); item++)
        {
            osal_memset(&r_entry, 0, sizeof(rtk_classify_cfg_t));
            r_entry.index = item;
            if((ret = rtk_classify_cfgEntry_get(&r_entry)) != RT_ERR_OK)
            {
                osal_printf("\n get entry fail: %s %d, ret=%d, pattern=%d, item=%d\n",__FUNCTION__,__LINE__,ret,pattern, item);
                return RT_ERR_FAILED;
            }

            if(osal_memcmp(&r_entry.field.readField, &readField, sizeof(rtk_classify_raw_field_t)) != 0)
            {
                osal_printf("\n rule cmp fail: %s %d, ret=%d, pattern=%d, item=%d, read data:0x%x-%x-%x, care:0x%x-%x-%x\n",__FUNCTION__,__LINE__,ret, pattern, item,
                            r_entry.field.readField.dataFieldRaw[0],r_entry.field.readField.dataFieldRaw[1],r_entry.field.readField.dataFieldRaw[2],
                            r_entry.field.readField.careFieldRaw[0],r_entry.field.readField.careFieldRaw[1],r_entry.field.readField.careFieldRaw[2]);
                osal_printf("\n write data:0x%x-%x-%x, care:0x%x-%x-%x\n",
                            readField.dataFieldRaw[0],readField.dataFieldRaw[1],readField.dataFieldRaw[2],
                            readField.careFieldRaw[0],readField.careFieldRaw[1],readField.careFieldRaw[2]);
                return RT_ERR_FAILED;
            }

            if(osal_memcmp(&r_entry.act.dsAct, &entry_dsAct[0], sizeof(rtk_classify_ds_act_t)) != 0)
            {
                osal_printf("\n action cmp fail: %s %d, ret=%d, pattern=%d, item=%d, read data:%x-%x-%x-%x-%x-%x-%x-%x-%x-%x-%x-%x-%x-%x-%x\n",__FUNCTION__,__LINE__,ret, pattern, item,
                            r_entry.act.dsAct.csAct, r_entry.act.dsAct.csVidAct, r_entry.act.dsAct.csPriAct, r_entry.act.dsAct.cAct, r_entry.act.dsAct.cVidAct,
                            r_entry.act.dsAct.cPriAct, r_entry.act.dsAct.interPriAct, r_entry.act.dsAct.uniAct, r_entry.act.dsAct.dscp, r_entry.act.dsAct.sTagVid,
                            r_entry.act.dsAct.sTagPri, r_entry.act.dsAct.cTagVid, r_entry.act.dsAct.cTagPri, r_entry.act.dsAct.cfPri, r_entry.act.dsAct.uniMask.bits[0]);
                osal_printf("\n write data:%x-%x-%x-%x-%x-%x-%x-%x-%x-%x-%x-%x-%x-%x-%x\n",
                            test_dsAct[item].csAct, test_dsAct[item].csVidAct, test_dsAct[item].csPriAct, test_dsAct[item].cAct, test_dsAct[item].cVidAct,
                            test_dsAct[item].cPriAct, test_dsAct[item].interPriAct, test_dsAct[item].uniAct, test_dsAct[item].dscp, test_dsAct[item].sTagVid,
                            test_dsAct[item].sTagPri, test_dsAct[item].cTagVid, test_dsAct[item].cTagPri, test_dsAct[item].cfPri, test_dsAct[item].uniMask.bits[0]);
                return RT_ERR_FAILED;
            }
        }
    }
    return RT_ERR_OK;
}

int32 dal_classf_misc_test(uint32 caseNo)
{
    rtk_classify_unmatch_action_t action, r_action;
    rtk_classify_rangeCheck_l4Port_t port_range;
    rtk_classify_rangeCheck_ip_t     ip_range;
    rtk_port_t port;
    rtk_classify_cf_sel_t cf_sel;
    rtk_pri_t pri;
    rtk_dscp_t dscp;
    int32               ret;

#if defined(CONFIG_SDK_RTL9603CVD)
    if(UNITTEST_UTIL_CHIP_TYPE == RTL9603CVD_CHIP_ID)
    {
        osal_printf("\n this chip not support %s!!\n",__FUNCTION__);
        return RT_ERR_OK;
    }
#endif

    for(action=CLASSIFY_UNMATCH_DROP; action<CLASSIFY_UNMATCH_END; action++)
    {
#if defined(CONFIG_SDK_RTL9607C)
        if(UNITTEST_UTIL_CHIP_TYPE == RTL9607C_CHIP_ID)
        {
            /* RTL9607C doesn't support unmatch drop */
            if (action == CLASSIFY_UNMATCH_DROP)
                continue;
        }
#endif

        if((ret = rtk_classify_unmatchAction_set(action)) != RT_ERR_OK)
        {
            osal_printf("\n umatch action set fail: %s %d, ret=%d\n",__FUNCTION__,__LINE__,ret);
            osal_printf("\n action = %d", action);
            return RT_ERR_FAILED;
        }
        if((ret = rtk_classify_unmatchAction_get(&r_action)) != RT_ERR_OK)
        {
            osal_printf("\n umatch action get fail: %s %d, ret=%d\n",__FUNCTION__,__LINE__,ret);
            return RT_ERR_FAILED;
        }
        if(action != r_action)
        {
            osal_printf("\n umatch action compare fail: %s %d, action=%d, r_action=%d\n",__FUNCTION__,__LINE__,action,r_action);
            return RT_ERR_FAILED;
        }
    }

#if defined(CONFIG_SDK_APOLLOMP)

#endif

    return RT_ERR_OK;
}

int32 dal_classf_test1(uint32 caseNo)
{
    rtk_classify_cfg_t  entry;
    rtk_classify_field_t *pField_vid, *pField_pri, *pField_sid;
    rtk_portmask_t portMask;
    int32  ret;

#if defined(CONFIG_SDK_RTL9603CVD)
    if(UNITTEST_UTIL_CHIP_TYPE == RTL9603CVD_CHIP_ID)
    {
        osal_printf("\n this chip not support %s!!\n",__FUNCTION__);
        return RT_ERR_OK;
    }
#endif

    rtk_classify_init();

    /* DS: filtering SID 10, C-tag VID 1000, priority 5, and action to forward to UNI_0, translate VID 1000 to VID 100, copy priority*/
    osal_memset(&entry, 0, sizeof(rtk_classify_cfg_t));

    entry.index = 0;
    entry.direction = CLASSIFY_DIRECTION_DS;
    entry.valid = ENABLED;
    entry.invert = CLASSIFY_INVERT_DISABLE;

    pField_vid = (rtk_classify_field_t *)osal_alloc(sizeof(rtk_classify_field_t));

    pField_vid->fieldType = CLASSIFY_FIELD_TAG_VID;
    pField_vid->classify_pattern.tagVid.value = 1000;
    pField_vid->classify_pattern.tagVid.mask = 0xfff;
    if((ret = rtk_classify_field_add(&entry, pField_vid)) != RT_ERR_OK)
        return ret;

    pField_pri = (rtk_classify_field_t *)osal_alloc(sizeof(rtk_classify_field_t));
    pField_pri->fieldType = CLASSIFY_FIELD_TAG_PRI;
    pField_pri->classify_pattern.tagPri.value = 5;
    pField_pri->classify_pattern.tagPri.mask = 7;
    if((ret = rtk_classify_field_add(&entry, pField_pri)) != RT_ERR_OK)
        return ret;

    pField_sid = (rtk_classify_field_t *)osal_alloc(sizeof(rtk_classify_field_t));
    pField_sid->fieldType = CLASSIFY_FIELD_TOS_DSIDX;
    pField_sid->classify_pattern.tosDsidx.value = 10;
    pField_sid->classify_pattern.tosDsidx.mask = 0x7f;
    if((ret = rtk_classify_field_add(&entry, pField_sid)) != RT_ERR_OK)
        return ret;

    entry.act.dsAct.csAct = CLASSIFY_DS_CSACT_DEL_STAG;
    entry.act.dsAct.cAct = CLASSIFY_DS_CACT_ADD_CTAG_8100;
    entry.act.dsAct.cVidAct = CLASSIFY_DS_VID_ACT_ASSIGN;
    entry.act.dsAct.cPriAct = CLASSIFY_DS_PRI_ACT_FROM_1ST_TAG;
    entry.act.dsAct.cTagVid = 100;
    entry.act.dsAct.uniAct = CLASSIFY_DS_UNI_ACT_FORCE_FORWARD;
    rtk_switch_port2PortMask_set(&entry.act.dsAct.uniMask, RTK_PORT_UTP0);

    if((ret = rtk_classify_cfgEntry_add(&entry)) != RT_ERR_OK)
    {
        osal_printf("\n rtk_classify_cfgEntry_add fail: %s %d, ret=%d\n",__FUNCTION__,__LINE__,ret);
        return RT_ERR_FAILED;
    }

    osal_free(pField_vid);
    osal_free(pField_pri);

    return RT_ERR_OK;
}

