#include <osal/lib.h>
#include <common/debug/rt_log.h>
#include <dal/apollo/raw/apollo_raw_sec.h>
#include <dal/raw/apollo/dal_raw_sec_test_case.h>

/* Define symbol used for test input */


int32 dal_raw_sec_test(uint32 caseNo)
{
    rtk_enable_t enable;
    rtk_action_t act;
    uint32 threshold;
    
    /*error input check*/
    /*out of range*/
    if (RT_ERR_OK == apollo_raw_sec_portAttackPreventEnable_set(APOLLO_PORTNO, DISABLED))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if (RT_ERR_OK == apollo_raw_sec_portAttackPreventEnable_set(0, RTK_ENABLE_END))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if (RT_ERR_OK == apollo_raw_sec_portAttackPreventEnable_get(APOLLO_PORTNO, &enable))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if (RT_ERR_OK == apollo_raw_sec_dslvcAttackPreventEnable_set(APOLLO_VCPORTNOMAX + 1, DISABLED))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if (RT_ERR_OK == apollo_raw_sec_dslvcAttackPreventEnable_set(0, RTK_ENABLE_END))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if (RT_ERR_OK == apollo_raw_sec_dslvcAttackPreventEnable_get(APOLLO_VCPORTNOMAX + 1, &enable))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if (RT_ERR_OK == apollo_raw_sec_attackPreventEnable_set(ATTACK_TYPE_END, DISABLED))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if (RT_ERR_OK == apollo_raw_sec_attackPreventEnable_set(0, RTK_ENABLE_END))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if (RT_ERR_OK == apollo_raw_sec_attackPreventEnable_get(ATTACK_TYPE_END, &enable))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if (RT_ERR_OK == apollo_raw_sec_attackPreventAction_set(ATTACK_TYPE_END, 0))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if (RT_ERR_OK == apollo_raw_sec_attackPreventAction_set(0, ACTION_END))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if (RT_ERR_OK == apollo_raw_sec_attackPreventAction_get(ATTACK_TYPE_END, &act))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if (RT_ERR_OK == apollo_raw_sec_floodThreshold_set(ATTACK_TYPE_END, APOLLO_SEC_FLOOD_THRESHOLD_MAX))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if (RT_ERR_OK == apollo_raw_sec_floodThreshold_set(0, APOLLO_SEC_FLOOD_THRESHOLD_MAX + 1))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if (RT_ERR_OK == apollo_raw_sec_floodThreshold_get(ATTACK_TYPE_END, &threshold))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }



    /*NULL pointer*/


    /* get/set test*/


    return RT_ERR_OK;
}

