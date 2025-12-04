//
// Copyright (C) 2018 Realtek Semiconductor Corp.
// All Rights Reserved.
//
// This program is the proprietary software of Realtek Semiconductor
// Corporation and/or its licensors, and only be used, duplicated,
// modified or distributed under the authorized license from Realtek.
//
// ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
// THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
//
// $Revision:  $
// $Date:  $
//
// Purpose : Define diag shell commands for trunk.
//
// Feature : The file have include the following module and sub-modules
//           1) switch commands.
//
#include <stdio.h>
#include <string.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <diag_util.h>
#include <diag_str.h>
#include <parser/cparser_priv.h>
#include <rtk/rt/rt_epon.h>

/*
 * rt_epon init
 */
cparser_result_t
cparser_cmd_rt_epon_init(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rt_epon_init(), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_epon_init */

/*
 * rt_epon get llid-table <UINT:index>
 */
cparser_result_t
cparser_cmd_rt_epon_get_llid_table_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rt_epon_llid_entry_t llidEntry;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    llidEntry.llidIdx = *index_ptr;

    DIAG_UTIL_ERR_CHK(rt_epon_llid_entry_get(&llidEntry), ret); 
    diag_util_mprintf("idx:%d LLID:%6d valid:%d report_timer:%4d report_timeout:%d mac-address:%s\n",
                            *index_ptr,llidEntry.llid,llidEntry.valid,llidEntry.reportTimer,llidEntry.isReportTimeout,diag_util_inet_mactoa(&llidEntry.mac.octet[0]));

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_epon_get_llid_table_index */

/*
 * rt_epon get llid-table
 */
cparser_result_t
cparser_cmd_rt_epon_get_llid_table(
    cparser_context_t *context)
{
    uint32 index;
    int32 ret = RT_ERR_FAILED;
    rt_epon_llid_entry_t llidEntry;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    for(index = 0; index<HAL_MAX_LLID_ENTRY(); index++)
    {
        llidEntry.llidIdx = index;
        DIAG_UTIL_ERR_CHK(rt_epon_llid_entry_get(&llidEntry), ret); 
        diag_util_mprintf("idx:%d LLID:%6d valid:%d report_timer:%4d report_timeout:%d mac-address:%s\n",
                            index,llidEntry.llid,llidEntry.valid,llidEntry.reportTimer,llidEntry.isReportTimeout,diag_util_inet_mactoa(&llidEntry.mac.octet[0]));
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_epon_get_llid_table */

/*
 * rt_epon set llid-table <UINT:index> state ( disable | enable )
 */
cparser_result_t
cparser_cmd_rt_epon_set_llid_table_index_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    rt_enable_t enable;
    int32 ret = RT_ERR_FAILED;
    rt_epon_llid_entry_t llidEntry;

    DIAG_UTIL_PARAM_CHK();

    if('e'==TOKEN_CHAR(5,0))
        enable = ENABLED;
    else
        enable = DISABLED;

    llidEntry.llidIdx = *index_ptr;

    DIAG_UTIL_ERR_CHK(rt_epon_llid_entry_get(&llidEntry), ret); 
    llidEntry.valid = enable;
    DIAG_UTIL_ERR_CHK(rt_epon_llid_entry_set(&llidEntry), ret); 

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_epon_set_llid_table_index_state_disable_enable */

/*
 * rt_epon set llid-table <UINT:index> llid <UINT:llid>
 */
cparser_result_t
cparser_cmd_rt_epon_set_llid_table_index_llid_llid(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *llid_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rt_epon_llid_entry_t llidEntry;

    DIAG_UTIL_PARAM_CHK();

    llidEntry.llidIdx = *index_ptr;
        
    DIAG_UTIL_ERR_CHK(rt_epon_llid_entry_get(&llidEntry), ret); 
    llidEntry.llid = *llid_ptr;
    DIAG_UTIL_ERR_CHK(rt_epon_llid_entry_set(&llidEntry), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_epon_set_llid_table_index_llid_llid */

/*
 * rt_epon set llid-table <UINT:index> report-timer <UINT:timer>
 */
cparser_result_t
cparser_cmd_rt_epon_set_llid_table_index_report_timer_timer(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *timer_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rt_epon_llid_entry_t llidEntry;

    DIAG_UTIL_PARAM_CHK();

    llidEntry.llidIdx = *index_ptr;
        
    DIAG_UTIL_ERR_CHK(rt_epon_llid_entry_get(&llidEntry), ret); 
    llidEntry.reportTimer = *timer_ptr;
    DIAG_UTIL_ERR_CHK(rt_epon_llid_entry_set(&llidEntry), ret); 

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_epon_set_llid_table_index_report_timer_timer */

/*
 * rt_epon get register llid-idx
 */
cparser_result_t
cparser_cmd_rt_epon_get_register_llid_idx(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rt_epon_regReq_t regReq;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rt_epon_registerReq_get(&regReq), ret); 
    diag_util_mprintf("register llid table index:%d\n",regReq.llidIdx);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_epon_get_register_llid_idx */

/*
 * rt_epon set register llid-idx <UINT:index> 
 */
cparser_result_t
cparser_cmd_rt_epon_set_register_llid_idx_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rt_epon_regReq_t regReq;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rt_epon_registerReq_get(&regReq), ret); 
    regReq.llidIdx = *index_ptr; 
    DIAG_UTIL_ERR_CHK(rt_epon_registerReq_set(&regReq), ret); 

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_epon_set_register_llid_idx_index */

/*
 * rt_epon get register state
 */
cparser_result_t
cparser_cmd_rt_epon_get_register_state(
    cparser_context_t *context)
{
    rt_enable_t enable;
    int32 ret = RT_ERR_FAILED;
    rt_epon_regReq_t regReq;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rt_epon_registerReq_get(&regReq), ret); 
    diag_util_printf("state:%s\n",diagStr_enable[regReq.doRequest]);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_epon_get_register_state */

/*
 * rt_epon set register state ( disable | enable )
 */
cparser_result_t
cparser_cmd_rt_epon_set_register_state_disable_enable(
    cparser_context_t *context)
{
    rt_enable_t enable;
    int32 ret = RT_ERR_FAILED;
    rt_epon_regReq_t regReq;

    DIAG_UTIL_PARAM_CHK();

    if('e'==TOKEN_CHAR(4,0))
        enable = ENABLED;
    else
        enable = DISABLED;


    DIAG_UTIL_ERR_CHK(rt_epon_registerReq_get(&regReq), ret); 
    regReq.doRequest = enable; 
    DIAG_UTIL_ERR_CHK(rt_epon_registerReq_set(&regReq), ret); 

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_epon_set_register_state_disable_enable */

/*
 * rt_epon get register mac-address
 */
cparser_result_t
cparser_cmd_rt_epon_get_register_mac_address(
    cparser_context_t *context)
{
    rt_mac_t   regMac;
    int32 ret = RT_ERR_FAILED;
    rt_epon_regReq_t regReq;
    rt_epon_llid_entry_t llidEntry;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rt_epon_registerReq_get(&regReq), ret);
    llidEntry.llidIdx = regReq.llidIdx;
    DIAG_UTIL_ERR_CHK(rt_epon_llid_entry_get(&llidEntry), ret);
    
    diag_util_mprintf("dmac data: %s\n",diag_util_inet_mactoa(&llidEntry.mac.octet[0]));

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_epon_get_register_mac_address */

/*
 * rt_epon set register mac-address <MACADDR:mac> 
 */
cparser_result_t
cparser_cmd_rt_epon_set_register_mac_address_mac(
    cparser_context_t *context,
    cparser_macaddr_t  *mac_ptr)
{
    rt_mac_t   regMac;
    int32 ret = RT_ERR_FAILED;
    rt_epon_regReq_t regReq;
    rt_epon_llid_entry_t llidEntry;

    DIAG_UTIL_PARAM_CHK();

    osal_memcpy(&regMac.octet, mac_ptr->octet, ETHER_ADDR_LEN);    
    DIAG_UTIL_ERR_CHK(rt_epon_registerReq_get(&regReq), ret);
    llidEntry.llidIdx = regReq.llidIdx;
    DIAG_UTIL_ERR_CHK(rt_epon_llid_entry_get(&llidEntry), ret);
    osal_memcpy(&llidEntry.mac.octet, &regMac.octet, ETHER_ADDR_LEN);
    DIAG_UTIL_ERR_CHK(rt_epon_llid_entry_set(&llidEntry), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_epon_set_register_mac_address_mac */

/*
 * rt_epon get churning llid-idx <UINT:index> key-idx <UINT:kidx>
 */
cparser_result_t
cparser_cmd_rt_epon_get_churning_llid_idx_index_key_idx_kidx(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *kidx_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rt_epon_churningKeyEntry_t keyEntry;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    memset(&keyEntry,0,sizeof(rt_epon_churningKeyEntry_t));

    keyEntry.llidIdx = *index_ptr;
    keyEntry.keyIdx = *kidx_ptr;

    DIAG_UTIL_ERR_CHK(rt_epon_churningKey_get(&keyEntry), ret); 

    diag_util_mprintf("llid index = %d\n",keyEntry.llidIdx);
    diag_util_mprintf("key index = %d\n",keyEntry.keyIdx);
    diag_util_mprintf("key data 0 = 0x%02x 0x%02x 0x%02x\n",keyEntry.churningKey[0],keyEntry.churningKey[1],keyEntry.churningKey[2]);
    diag_util_mprintf("key data 1 = 0x%02x 0x%02x 0x%02x\n",keyEntry.churningKey1[0],keyEntry.churningKey1[1],keyEntry.churningKey1[2]);
    diag_util_mprintf("key data 2 = 0x%02x 0x%02x 0x%02x\n",keyEntry.churningKey2[0],keyEntry.churningKey2[1],keyEntry.churningKey2[2]);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_epon_get_churning_llid_idx_index_key_idx_kidx */

/*
 * rt_epon set churning llid-idx <UINT:index> key-idx <UINT:kidx> key0-2 <UINT:data0> key3-5 <UINT:data1> key6-8 <UINT:data2>
 */
cparser_result_t
cparser_cmd_rt_epon_set_churning_llid_idx_index_key_idx_kidx_key0_2_data0_key3_5_data1_key6_8_data2(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *kidx_ptr,
    uint32_t  *data0_ptr,
    uint32_t  *data1_ptr,
    uint32_t  *data2_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rt_epon_churningKeyEntry_t keyEntry;

    DIAG_UTIL_PARAM_CHK();

    memset(&keyEntry,0,sizeof(rt_epon_churningKeyEntry_t));

    keyEntry.llidIdx = *index_ptr;
    keyEntry.keyIdx = *kidx_ptr;

    keyEntry.churningKey[0] = (*data0_ptr >> 16) & 0xFF; 
    keyEntry.churningKey[1] = (*data0_ptr >> 8) & 0xFF; 
    keyEntry.churningKey[2] = (*data0_ptr) & 0xFF; 

    keyEntry.churningKey1[0] = (*data1_ptr >> 16) & 0xFF; 
    keyEntry.churningKey1[1] = (*data1_ptr >> 8) & 0xFF; 
    keyEntry.churningKey1[2] = (*data1_ptr) & 0xFF; 

    keyEntry.churningKey2[0] = (*data2_ptr >> 16) & 0xFF; 
    keyEntry.churningKey2[1] = (*data2_ptr >> 8) & 0xFF; 
    keyEntry.churningKey2[2] = (*data2_ptr) & 0xFF;

    DIAG_UTIL_ERR_CHK(rt_epon_churningKey_set(&keyEntry), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_epon_set_churning_llid_idx_index_key_idx_kidx_key0_2_data0_key3_5_data1_key6_8_data2 */

/*
 * rt_epon get us-fec state
 */
cparser_result_t
cparser_cmd_rt_epon_get_us_fec_state(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rt_enable_t enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rt_epon_usFecState_get(&enable), ret); 
        
    diag_util_mprintf("US FEC state:%s\n",diagStr_enable[enable]);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_epon_get_us_fec_state */

/*
 * rt_epon set us-fec state ( disable | enable )
 */
cparser_result_t
cparser_cmd_rt_epon_set_us_fec_state_disable_enable(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rt_enable_t enable;

    DIAG_UTIL_PARAM_CHK();

    if('e'==TOKEN_CHAR(4,0))
        enable = ENABLED;
    else
        enable = DISABLED;

    DIAG_UTIL_ERR_CHK(rt_epon_usFecState_set(enable), ret); 

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_epon_set_us_fec_state_disable_enable */

/*
 * rt_epon get ds-fec state
 */
cparser_result_t
cparser_cmd_rt_epon_get_ds_fec_state(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rt_enable_t enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rt_epon_dsFecState_get(&enable), ret); 
        
    diag_util_mprintf("DS FEC state:%s\n",diagStr_enable[enable]);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_epon_get_ds_fec_state */

/*
 * rt_epon set ds-fec state ( disable | enable )
 */
cparser_result_t
cparser_cmd_rt_epon_set_ds_fec_state_disable_enable(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rt_enable_t enable;

    DIAG_UTIL_PARAM_CHK();

    if('e'==TOKEN_CHAR(4,0))
        enable = ENABLED;
    else
        enable = DISABLED;

    DIAG_UTIL_ERR_CHK(rt_epon_dsFecState_set(enable), ret); 

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_epon_set_ds_fec_state_disable_enable */

/*
 * rt_epon get los state
 */
cparser_result_t
cparser_cmd_rt_epon_get_los_state(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rt_enable_t state;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("");

    DIAG_UTIL_ERR_CHK(rt_epon_losState_get(&state), ret); 

    diag_util_mprintf("Los state:%s\n",diagStr_enable[state]);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_epon_get_los_state */

/*
 * rt_epon add queue llid <UINT:llid> queue-id <UINT:queueId> cir <UINT:cir> pir <UINT:pir> weight <UINT:weight> schedule-type ( sp | wrr )
 */
cparser_result_t
cparser_cmd_rt_epon_add_queue_llid_llid_queue_id_queueId_cir_cir_pir_pir_weight_weight_schedule_type_sp_wrr(
    cparser_context_t *context,
    uint32_t  *llid_ptr,
    uint32_t  *queueId_ptr,
    uint32_t  *cir_ptr,
    uint32_t  *pir_ptr,
    uint32_t  *weight_ptr)
{
    rt_epon_queueCfg_t queueCfg;
    int32                   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*llid_ptr > 31),CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*queueId_ptr > 127),CPARSER_ERR_INVALID_PARAMS);

    queueCfg.cir = *cir_ptr;
    queueCfg.pir = *pir_ptr;
    queueCfg.weight = *weight_ptr;
  
    if ('w' == TOKEN_CHAR(14,0))
    {
        queueCfg.scheduleType = RT_EPON_LLID_QUEUE_SCHEDULER_WRR;
    }
    else
    {
        queueCfg.scheduleType = RT_EPON_LLID_QUEUE_SCHEDULER_SP;
    }

    DIAG_UTIL_ERR_CHK(rt_epon_ponQueue_set(*llid_ptr, *queueId_ptr, &queueCfg), ret);


    return CPARSER_OK;
}    /* end of cparser_cmd_rt_epon_add_queue_llid_llid_queue_id_queueid_cir_cir_pir_pir_weight_weight_schedule_type_sp_wrr_sp_wrr */

/*
 * rt_epon del queue llid <UINT:llid> queue-id <UINT:queueId>
 */
cparser_result_t
cparser_cmd_rt_epon_del_queue_llid_llid_queue_id_queueId(
    cparser_context_t *context,
    uint32_t  *llid_ptr,
    uint32_t  *queueId_ptr)
{
    int32                   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*llid_ptr > 31),CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*queueId_ptr > 127),CPARSER_ERR_INVALID_PARAMS);

    DIAG_UTIL_ERR_CHK(rt_epon_ponQueue_del(*llid_ptr, *queueId_ptr), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_gpon_del_queue_tcont_id_tcontid_queue_id_queueid */

/*
 * rt_epon show llid-queue
 */
cparser_result_t
cparser_cmd_rt_epon_show_llid_queue(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 queueId,first=1;
    rt_epon_queueCfg_t pQueuecfg;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();


     for(queueId=0; queueId<8; queueId++)
    {
        ret = rt_epon_ponQueue_get(0, queueId, &pQueuecfg);
        if(ret == RT_ERR_OK)
        {
            if(first==1)
            {
                diag_util_mprintf("============================================================\r\n");
                diag_util_mprintf("     EPON ONU MAC LLID Queue Status\r\n");
                diag_util_mprintf(" LLID | Queue-Idx | Queue-Weight | CIR(8Kbps) | PIR(8Kbps)\r\n");
                first = 0;
            }
            diag_util_mprintf(" %4d | %9d | %12d | %10d | %10d\r\n",
                      0, queueId, pQueuecfg.weight, pQueuecfg.cir, pQueuecfg.pir);
        }
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_epon_show_llid_queue */


/*
 * rt_epon get egress rate
 */
cparser_result_t
cparser_cmd_rt_epon_get_egress_rate(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 rate;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rt_epon_egrBandwidthCtrlRate_get(&rate), ret);
    
    diag_util_mprintf("egress rate: %d\n",rate);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_epon_get_egress_rate */

/*
 * rt_epon set egress rate <UINT:rate> */
cparser_result_t
cparser_cmd_rt_epon_set_egress_rate_rate(
    cparser_context_t *context,
    uint32_t  *rate_ptr)
{
    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(rt_epon_egrBandwidthCtrlRate_set(*rate_ptr), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_epon_set_egress_rate_rate */




/*
 * rt_epon dump mib-counter { <UINT:llididx> }
 */
cparser_result_t
cparser_cmd_rt_epon_dump_mib_counter_llididx(
    cparser_context_t *context,
    uint32_t  *llididx_ptr)
{
    rt_epon_counter_t counter;
    int32 ret = RT_ERR_FAILED;
    int i;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    memset(&counter,0x0,sizeof(rt_epon_counter_t));

    if((4 == TOKEN_NUM())&&(llididx_ptr!=NULL))
	{
		counter.llidIdx = *llididx_ptr;
	}
	else
	{    
		counter.llidIdx = 0;
	}
    DIAG_UTIL_ERR_CHK(rt_epon_mibCounter_get(&counter), ret); 

    diag_util_mprintf("[Per LLID counter--LLID index :%d]\n",counter.llidIdx);
    
    for(i=0;i<10;i++)
        diag_util_mprintf("queue %d             : %8d\n",i,counter.llidIdxCnt.queueTxFrames[i]);

    diag_util_mprintf("mpcpTxReport        : %8d\n",counter.llidIdxCnt.mpcpTxReport);
    diag_util_mprintf("mpcpRxGate          : %8d\n",counter.llidIdxCnt.mpcpRxGate);
    diag_util_mprintf("onuLlidNotBcst      : %8d\n\n",counter.llidIdxCnt.onuLlidNotBcst);

    diag_util_mprintf("[Globol counter]\n");

    diag_util_mprintf("mpcpRxDiscGate      : %8d\n",counter.mpcpRxDiscGate);
    diag_util_mprintf("mpcpTxRegRequest    : %8d\n",counter.mpcpTxRegRequest);
    diag_util_mprintf("crc8Err             : %8d\n",counter.crc8Err);
    diag_util_mprintf("notBcstBitNotOnuLlid: %8d\n",counter.notBcstBitNotOnuLlid);
    diag_util_mprintf("bcstBitPlusOnuLLid  : %8d\n",counter.bcstBitPlusOnuLLid);
    diag_util_mprintf("bcstNotOnuLLid      : %8d\n",counter.bcstNotOnuLLid);
    diag_util_mprintf("[FEC]\n");
    diag_util_mprintf("fecCorrectedBlocks  : %8d\n",counter.fecCorrectedBlocks);
    diag_util_mprintf("fecUncorrectedBlocks: %8d\n",counter.fecUncorrectedBlocks);
    diag_util_mprintf("fecTotalCodeWord    : %8d\n",counter.fecTtotalCodeword);

    return CPARSER_OK;
}   /* end of cparser_cmd_epon_dump_mib_counter_llididx */

