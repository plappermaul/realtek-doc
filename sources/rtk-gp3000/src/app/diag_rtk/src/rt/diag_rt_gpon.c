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
// Purpose : Define diag shell commands for gpon.
//
// Feature : The file have include the following module and sub-modules
//           1) gpon commands.
//
#include <stdio.h>
#include <string.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <diag_util.h>
#include <diag_str.h>
#include <parser/cparser_priv.h>
#include <rtk/rt/rt_gpon.h>

typedef enum rt_gpon_diag_pm_type_e{
    RT_GPON_DIAG_PM_TYPE_PHY,            /* PHY PM counters */
    RT_GPON_DIAG_PM_TYPE_PLOAM,           /* PLOAM PM counters*/
    RT_GPON_DIAG_PM_TYPE_OMCI,             /* OMCI PM counters */
    RT_GPON_DIAG_PM_TYPE_GEM,               /* GEM PM counters */
    RT_GPON_DIAG_PM_TYPE_ETH,               /* ETHER PM counters */
    RT_GPON_DIAG_PM_TYPE_MAX
}rt_gpon_diag_pm_type_t;


/*
 * rt_gpon init
 */
cparser_result_t
cparser_cmd_rt_gpon_init(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rt_gpon_init(), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_gpon_init */

/*
 * rt_gpon set serialnumber <STRING:sn>
 */
cparser_result_t
cparser_cmd_rt_gpon_set_serialnumber_sn(
    cparser_context_t *context,
    char * *sn_ptr)
{
    rt_gpon_serialNumber_t sn_tmp;
    uint32 specific_tmp;
    int ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_PARAM_RANGE_CHK((strlen(*sn_ptr) != 12),CPARSER_ERR_INVALID_PARAMS);
    osal_memcpy(sn_tmp.vendor,*sn_ptr, RT_GPON_VENDOR_ID_LEN);

    DIAG_UTIL_PARAM_RANGE_CHK(!diag_util_str2hex32(*sn_ptr+4,&specific_tmp),CPARSER_ERR_INVALID_PARAMS);

    sn_tmp.specific[0] = ((specific_tmp)>>24)&0xFF;
    sn_tmp.specific[1] = ((specific_tmp)>>16)&0xFF;
    sn_tmp.specific[2] = ((specific_tmp)>>8)&0xFF;
    sn_tmp.specific[3] = ((specific_tmp)>>0)&0xFF;

    DIAG_UTIL_ERR_CHK(rt_gpon_serialNumber_set(&sn_tmp), ret);
    return CPARSER_OK;
}    /* end of cparser_cmd_rt_gpon_set_serialnumber_sn */

/*
 * rt_gpon set serial-number <STRING:vendor_id> <UINT:serial_number>
 */
cparser_result_t
cparser_cmd_rt_gpon_set_serial_number_vendor_id_serial_number(
    cparser_context_t *context,
    char * *vendor_id_ptr,
    uint32_t  *serial_number_ptr)
{
    rt_gpon_serialNumber_t sn;
    int32                   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((strlen(*vendor_id_ptr) != RT_GPON_VENDOR_ID_LEN),CPARSER_ERR_INVALID_PARAMS);

    osal_memcpy(sn.vendor,*vendor_id_ptr,RT_GPON_VENDOR_ID_LEN);
    sn.specific[0] = ((*serial_number_ptr)>>24)&0xFF;
    sn.specific[1] = ((*serial_number_ptr)>>16)&0xFF;
    sn.specific[2] = ((*serial_number_ptr)>>8)&0xFF;
    sn.specific[3] = ((*serial_number_ptr)>>0)&0xFF;

    DIAG_UTIL_ERR_CHK(rt_gpon_serialNumber_set(&sn), ret);
    return CPARSER_OK;
}    /* end of cparser_cmd_rt_gpon_set_serial_number_vendor_id_serial_number */

/*
 * rt_gpon set serial-number-hex <STRING:vendor_id> <STRING:serial_number>
 */
cparser_result_t
cparser_cmd_rt_gpon_set_serial_number_hex_vendor_id_serial_number(
    cparser_context_t *context,
    char * *vendor_id_ptr,
    char * *serial_number_ptr)
{
    rt_gpon_serialNumber_t sn;
    uint32 vendor_tmp, specific_tmp;
    int32                   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((strlen(*vendor_id_ptr) != 8),CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((strlen(*serial_number_ptr) != 8),CPARSER_ERR_INVALID_PARAMS);

    diag_util_str2hex32(*vendor_id_ptr, &vendor_tmp);
    sn.vendor[0] = ((vendor_tmp)>>24)&0xFF;
    sn.vendor[1] = ((vendor_tmp)>>16)&0xFF;
    sn.vendor[2] = ((vendor_tmp)>>8)&0xFF;
    sn.vendor[3] = ((vendor_tmp)>>0)&0xFF;

    diag_util_str2hex32(*serial_number_ptr, &specific_tmp);
    sn.specific[0] = ((specific_tmp)>>24)&0xFF;
    sn.specific[1] = ((specific_tmp)>>16)&0xFF;
    sn.specific[2] = ((specific_tmp)>>8)&0xFF;
    sn.specific[3] = ((specific_tmp)>>0)&0xFF;

    DIAG_UTIL_ERR_CHK(rt_gpon_serialNumber_set(&sn), ret);
    return CPARSER_OK;
}    /* end of cparser_cmd_rt_gpon_set_serial_number_hex_vendor_id_serial_number */

/*
 * rt_gpon set registrationId <STRING:registrationId>
 */
cparser_result_t
cparser_cmd_rt_gpon_set_registrationId_registrationId(
    cparser_context_t *context,
    char * *registrationId_ptr)
{
    rt_gpon_registrationId_t regId;
    int32               ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK(((strlen(*registrationId_ptr) != RT_GPON_REGISTRATION_ID_LEN) && (strlen(*registrationId_ptr) != RT_GPON_PASSWORD_LEN)),CPARSER_ERR_INVALID_PARAMS);

    osal_memcpy(regId.regId,*registrationId_ptr,RT_GPON_REGISTRATION_ID_LEN);

    DIAG_UTIL_ERR_CHK(rt_gpon_registrationId_set(&regId), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_gpon_set_registrationid_registrationid */

/*
 * rt_gpon set registrationId-hex <STRING:registrationId>
 */
cparser_result_t
cparser_cmd_rt_gpon_set_registrationId_hex_registrationId(
    cparser_context_t *context,
    char * *registrationId_ptr)
{
    rt_gpon_registrationId_t regId;
    uint8               tmpStr[9];
    uint32              tmpHex;
    int32               ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK(((strlen(*registrationId_ptr) != RT_GPON_REGISTRATION_ID_LEN*2)  && (strlen(*registrationId_ptr) != RT_GPON_PASSWORD_LEN*2))  ,CPARSER_ERR_INVALID_PARAMS);

    tmpStr[8] = '\0';
    memcpy(tmpStr, *registrationId_ptr, 8);
    diag_util_str2hex32(tmpStr, &tmpHex);
    regId.regId[0] = ((tmpHex)>>24)&0xFF;
    regId.regId[1] = ((tmpHex)>>16)&0xFF;
    regId.regId[2] = ((tmpHex)>>8)&0xFF;
    regId.regId[3] = ((tmpHex)>>0)&0xFF;

    memcpy(tmpStr, *registrationId_ptr+8, 8);
    diag_util_str2hex32(tmpStr, &tmpHex);
    regId.regId[4] = ((tmpHex)>>24)&0xFF;
    regId.regId[5] = ((tmpHex)>>16)&0xFF;
    regId.regId[6] = ((tmpHex)>>8)&0xFF;
    regId.regId[7] = ((tmpHex)>>0)&0xFF;

    memcpy(tmpStr, *registrationId_ptr+12, 8);
    diag_util_str2hex32(tmpStr, &tmpHex);
    regId.regId[8] = ((tmpHex)>>8)&0xFF;
    regId.regId[9] = ((tmpHex)>>0)&0xFF;

    /* GPON support 10 bytes only, NGPON extend to 36 bytes */
    memcpy(tmpStr, *registrationId_ptr+20, 8);
    diag_util_str2hex32(tmpStr, &tmpHex);
    regId.regId[10] = ((tmpHex)>>24)&0xFF;
    regId.regId[11] = ((tmpHex)>>16)&0xFF;
    regId.regId[12] = ((tmpHex)>>8)&0xFF;
    regId.regId[13] = ((tmpHex)>>0)&0xFF;

    memcpy(tmpStr, *registrationId_ptr+28, 8);
    diag_util_str2hex32(tmpStr, &tmpHex);
    regId.regId[14] = ((tmpHex)>>24)&0xFF;
    regId.regId[15] = ((tmpHex)>>16)&0xFF;
    regId.regId[16] = ((tmpHex)>>8)&0xFF;
    regId.regId[17] = ((tmpHex)>>0)&0xFF;

    memcpy(tmpStr, *registrationId_ptr+36, 8);
    diag_util_str2hex32(tmpStr, &tmpHex);
    regId.regId[18] = ((tmpHex)>>24)&0xFF;
    regId.regId[19] = ((tmpHex)>>16)&0xFF;
    regId.regId[20] = ((tmpHex)>>8)&0xFF;
    regId.regId[21] = ((tmpHex)>>0)&0xFF;

    memcpy(tmpStr, *registrationId_ptr+44, 8);
    diag_util_str2hex32(tmpStr, &tmpHex);
    regId.regId[22] = ((tmpHex)>>24)&0xFF;
    regId.regId[23] = ((tmpHex)>>16)&0xFF;
    regId.regId[24] = ((tmpHex)>>8)&0xFF;
    regId.regId[25] = ((tmpHex)>>0)&0xFF;

    memcpy(tmpStr, *registrationId_ptr+52, 8);
    diag_util_str2hex32(tmpStr, &tmpHex);
    regId.regId[26] = ((tmpHex)>>24)&0xFF;
    regId.regId[27] = ((tmpHex)>>16)&0xFF;
    regId.regId[28] = ((tmpHex)>>8)&0xFF;
    regId.regId[29] = ((tmpHex)>>0)&0xFF;

    memcpy(tmpStr, *registrationId_ptr+60, 8);
    diag_util_str2hex32(tmpStr, &tmpHex);
    regId.regId[30] = ((tmpHex)>>24)&0xFF;
    regId.regId[31] = ((tmpHex)>>16)&0xFF;
    regId.regId[32] = ((tmpHex)>>8)&0xFF;
    regId.regId[33] = ((tmpHex)>>0)&0xFF;

    memcpy(tmpStr, *registrationId_ptr+64, 8);
    diag_util_str2hex32(tmpStr, &tmpHex);
    regId.regId[34] = ((tmpHex)>>8)&0xFF;
    regId.regId[35] = ((tmpHex)>>0)&0xFF;

    DIAG_UTIL_ERR_CHK(rt_gpon_registrationId_set(&regId), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_gpon_set_registrationid_hex_registrationid */

/*
 * rt_gpon activate init-state ( o1 | o7 )
 */
cparser_result_t
cparser_cmd_rt_gpon_activate_init_state_o1_o7(
    cparser_context_t *context)
{
    rt_gpon_initialState_t state;
    int32                   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    if (0 == strcmp( "o1",TOKEN_STR(3)))
        state = RT_GPON_ONU_INIT_STATE_O1;
    else //if (0 == strcmp( "o7",TOKEN_STR(3)))
        state = RT_GPON_ONU_INIT_STATE_O7;

    DIAG_UTIL_ERR_CHK(rt_gpon_activate(state), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_gpon_activate_init_state_o1_o7 */

/*
 * rt_gpon deactivate
 */
cparser_result_t
cparser_cmd_rt_gpon_deactivate(
    cparser_context_t *context)
{
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rt_gpon_deactivate(), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_gpon_deactivate */

/*
 * rt_gpon get onu-state
 */
cparser_result_t
cparser_cmd_rt_gpon_get_onu_state(
    cparser_context_t *context)
{
    rt_gpon_onuState_t   state;
    int32                   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rt_gpon_onuState_get(&state), ret);

#ifdef CONFIG_LUNA_G3_SERIES
    diag_util_mprintf("ONU state: %s \n\r", diagStr_ngp2OnuState[state]);
#else
    diag_util_mprintf("ONU state: %s \n\r", diagStr_rtGponFsmStatus[state]);
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_gpon_get_onu_state */

/*
 * rt_gpon add tcont alloc-id <UINT:allocId>
 */
cparser_result_t
cparser_cmd_rt_gpon_add_tcont_alloc_id_allocId(
    cparser_context_t *context,
    uint32_t  *allocId_ptr)
{
    uint32   tcont_id;
    uint32   alloc_id;
    int32                   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*allocId_ptr > 16383),CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_OUTPUT_INIT();

    alloc_id = *allocId_ptr;

    DIAG_UTIL_ERR_CHK(rt_gpon_tcont_set(&tcont_id, alloc_id), ret);
    diag_util_mprintf("Add alloc_id %d, return TCONT_id=%d \n\r", alloc_id, tcont_id);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_gpon_add_tcont_alloc_id_allocid */

/*
 * rt_gpon del tcont tcont-id <UINT:tcontId>
 */
cparser_result_t
cparser_cmd_rt_gpon_del_tcont_tcont_id_tcontId(
    cparser_context_t *context,
    uint32_t  *tcontId_ptr)
{
    uint32    tcont_id;
    int32                   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*tcontId_ptr > 31),CPARSER_ERR_INVALID_PARAMS);
    tcont_id = *tcontId_ptr;

    DIAG_UTIL_ERR_CHK(rt_gpon_tcont_del(tcont_id), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_gpon_del_tcont_tcont_id_tcontid */

/*
 * rt_gpon add ds-flow flow-id <UINT:flowId> gem-port <UINT:gemPort> { aes }
 */
cparser_result_t
cparser_cmd_rt_gpon_add_ds_flow_flow_id_flowId_gem_port_gemPort_aes(
    cparser_context_t *context,
    uint32_t  *flowId_ptr,
    uint32_t  *gemPort_ptr)
{
    rt_gpon_dsFlow_t  dsFlow;
    int32                   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*flowId_ptr > 255),CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*gemPort_ptr > 65534),CPARSER_ERR_INVALID_PARAMS);

    memset(&dsFlow, 0x0, sizeof(dsFlow));

    dsFlow.gemPortId = *gemPort_ptr;

    if (TOKEN_NUM() < 8)
    {
        dsFlow.aesEn = 0;
    }
    else
    {
        dsFlow.aesEn = 1;
    }

    DIAG_UTIL_ERR_CHK(rt_gpon_dsFlow_set(*flowId_ptr, &dsFlow), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_gpon_add_ds_flow_flow_id_flowid_gem_port_gemport_aes */

/*
 * rt_gpon del ds-flow ( flow-id | all ) { <UINT:flowId> }
 */
cparser_result_t
cparser_cmd_rt_gpon_del_ds_flow_flow_id_all_flowId(
    cparser_context_t *context,
    uint32_t  *flowId_ptr)
{
    int32                   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    if ('a' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(rt_gpon_dsFlow_delAll(), ret);
    }
    else
    {
        DIAG_UTIL_PARAM_RANGE_CHK((NULL == flowId_ptr),CPARSER_ERR_INVALID_PARAMS);
        DIAG_UTIL_PARAM_RANGE_CHK((*flowId_ptr > 255),CPARSER_ERR_INVALID_PARAMS);
        DIAG_UTIL_ERR_CHK(rt_gpon_dsFlow_del(*flowId_ptr), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_gpon_del_ds_flow_flow_id_all_flowid */

/*
 * rt_gpon add us-flow flow-id <UINT:flowId> gem-port <UINT:gemPort> tcont <UINT:tcontId> queue <UINT:queueId> { aes }
 */
cparser_result_t
cparser_cmd_rt_gpon_add_us_flow_flow_id_flowId_gem_port_gemPort_tcont_tcontId_queue_queueId_aes(
    cparser_context_t *context,
    uint32_t  *flowId_ptr,
    uint32_t  *gemPort_ptr,
    uint32_t  *tcontId_ptr,
    uint32_t  *queueId_ptr)
{
    rt_gpon_usFlow_t  usFlow;
    int32                   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*flowId_ptr > 255),CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*gemPort_ptr > 65534),CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*tcontId_ptr > 31),CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*queueId_ptr > 255),CPARSER_ERR_INVALID_PARAMS);

    usFlow.gemPortId = *gemPort_ptr;
    usFlow.tcontId = *tcontId_ptr;
    usFlow.tcQueueId = *queueId_ptr;
  
    if (TOKEN_NUM() < 12)
    {
        usFlow.aesEn = 0;
    }
    else
    {
        usFlow.aesEn = 1;
    }

    DIAG_UTIL_ERR_CHK(rt_gpon_usFlow_set(*flowId_ptr, &usFlow), ret);


    return CPARSER_OK;
}    /* end of cparser_cmd_rt_gpon_add_us_flow_flow_id_flowid_gem_port_gemport_tcont_tcontid_queue_queueid_aes */

/*
 * rt_gpon del us-flow ( flow-id | all ) { <UINT:flowId> }
 */
cparser_result_t
cparser_cmd_rt_gpon_del_us_flow_flow_id_all_flowId(
    cparser_context_t *context,
    uint32_t  *flowId_ptr)
{
    int32                   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    if ('a' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(rt_gpon_usFlow_delAll(), ret);
    }
    else
    {
        DIAG_UTIL_PARAM_RANGE_CHK((NULL == flowId_ptr),CPARSER_ERR_INVALID_PARAMS);
        DIAG_UTIL_PARAM_RANGE_CHK((*flowId_ptr > 255),CPARSER_ERR_INVALID_PARAMS);
        DIAG_UTIL_ERR_CHK(rt_gpon_usFlow_del(*flowId_ptr), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_gpon_del_us_flow_flow_id_all_flowid */

/*
 * rt_gpon add queue tcont-id <UINT:tcontId> queue-id <UINT:queueId> cir <UINT:cir> pir <UINT:pir> weight <UINT:weight> schedule-type ( sp | wrr | sp-wrr )
 */
cparser_result_t
cparser_cmd_rt_gpon_add_queue_tcont_id_tcontId_queue_id_queueId_cir_cir_pir_pir_weight_weight_schedule_type_sp_wrr_sp_wrr(
    cparser_context_t *context,
    uint32_t  *tcontId_ptr,
    uint32_t  *queueId_ptr,
    uint32_t  *cir_ptr,
    uint32_t  *pir_ptr,
    uint32_t  *weight_ptr)
{
    rt_gpon_queueCfg_t queueCfg;
    int32                   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*tcontId_ptr > 31),CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*queueId_ptr > 255),CPARSER_ERR_INVALID_PARAMS);

    queueCfg.cir = *cir_ptr;
    queueCfg.pir = *pir_ptr;
    queueCfg.weight = *weight_ptr;
  
    if ('w' == TOKEN_CHAR(14,0))
    {
        queueCfg.scheduleType = RT_GPON_TCONT_QUEUE_SCHEDULER_WRR;
    }
    else if ('w' == TOKEN_CHAR(14, 3))
    {
        queueCfg.scheduleType = RT_GPON_TCONT_QUEUE_SCHEDULER_SP_WRR;
    }
    else
    {
        queueCfg.scheduleType = RT_GPON_TCONT_QUEUE_SCHEDULER_SP;
    }

    DIAG_UTIL_ERR_CHK(rt_gpon_ponQueue_set(*tcontId_ptr, *queueId_ptr, &queueCfg), ret);


    return CPARSER_OK;
}    /* end of cparser_cmd_rt_gpon_add_queue_tcont_id_tcontid_queue_id_queueid_cir_cir_pir_pir_weight_weight_schedule_type_sp_wrr_sp_wrr */

/*
 * rt_gpon del queue tcont-id <UINT:tcontId> queue-id <UINT:queueId>
 */
cparser_result_t
cparser_cmd_rt_gpon_del_queue_tcont_id_tcontId_queue_id_queueId(
    cparser_context_t *context,
    uint32_t  *tcontId_ptr,
    uint32_t  *queueId_ptr)
{
    int32                   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*tcontId_ptr > 31),CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*queueId_ptr > 255),CPARSER_ERR_INVALID_PARAMS);

    DIAG_UTIL_ERR_CHK(rt_gpon_ponQueue_del(*tcontId_ptr, *queueId_ptr), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_gpon_del_queue_tcont_id_tcontid_queue_id_queueid */

void _diag_rt_gpon_show_tcont(void)
{
    uint32                  tcont_id;
    uint32                  alloc_id;
    uint32                  found=0;
    int32                   ret = RT_ERR_FAILED;
    uint32                  first=1;
    rt_gpon_schedule_info_t  schldInfo;

    ret = rt_gpon_scheInfo_get(&schldInfo);
    if ( RT_ERR_OK != ret )
    {
        diag_util_mprintf("Unable to fatch the device's capbility.\r\n");
    }
    
    for(tcont_id=0;tcont_id<schldInfo.max_tcont;tcont_id++)
    {
        ret = rt_gpon_tcont_get(tcont_id, &alloc_id);
        if(ret == RT_ERR_OK)
        {
            found = 1;
            if(first==1)
            {
                diag_util_mprintf("============================================================\r\n");
                diag_util_mprintf("     GPON ONU MAC TCONT Status\r\n");
                diag_util_mprintf("TCONT ID | Alloc ID\r\n");
                first = 0;
            }
            diag_util_mprintf("    %4d |     %4d\r\n",
                              tcont_id, alloc_id);
        }
    }
    if(found)
    {
        diag_util_mprintf("============================================================\r\n");
    }
    else
    {
        diag_util_mprintf("The TCONT is not created.\r\n");
    }
}

void _diag_rt_gpon_show_tcont_queue(void)
{
    uint32                  tcont_id;
    uint32                  alloc_id;
    uint32                  found=0;
    int32                   ret = RT_ERR_FAILED;
    uint32                  first=1;
    uint32                  tcQueueId;
    rt_gpon_schedule_info_t  schldInfo;
    rt_gpon_queueCfg_t  pQueuecfg;

    ret = rt_gpon_scheInfo_get(&schldInfo);
    if ( RT_ERR_OK != ret )
    {
        diag_util_mprintf("Unable to fatch the device's capbility.\r\n");
    }

    for(tcont_id=0;tcont_id<schldInfo.max_tcont;tcont_id++)
    {
        for(tcQueueId=0; tcQueueId<schldInfo.max_tcon_queue; tcQueueId++)
        {
            ret = rt_gpon_ponQueue_get(tcont_id, tcQueueId, &pQueuecfg);
            if(ret == RT_ERR_OK)
            {
                found = 1;
                if(first==1)
                {   
                    diag_util_mprintf("============================================================\r\n");
                    diag_util_mprintf("     GPON ONU MAC TCONT Queue Status\r\n");
                    diag_util_mprintf(" TCONT | Queue-Idx | Queue-Weight | CIR(8Kbps) | PIR(8Kbps)\r\n");
                    first = 0;
                }
                diag_util_mprintf(" %5d | %9d | %12d | %10d | %10d\r\n",
                            tcont_id, tcQueueId, pQueuecfg.weight, pQueuecfg.cir, pQueuecfg.pir);
            }
        }
    }
    
    if(found == 1)
        diag_util_mprintf("============================================================\r\n");
}

void _diag_rt_gpon_show_ds_flow(void)
{
    uint32                  flow_id;
    rt_gpon_dsFlow_t  flow_attr;
    uint32                  found=0;
    int32                   ret = RT_ERR_FAILED;
    uint32                  first=1;
    rt_gpon_schedule_info_t  schldInfo;

    ret = rt_gpon_scheInfo_get(&schldInfo);
    if ( RT_ERR_OK != ret )
    {
        diag_util_mprintf("Unable to fatch the device's capbility.\r\n");
    }
    
    for(flow_id=0;flow_id<schldInfo.max_flow;flow_id++)
    {
        ret = rt_gpon_dsFlow_get(flow_id, &flow_attr);
        if(ret == RT_ERR_OK)
        {
            found = 1;
            if(first==1)
            {
                diag_util_mprintf("============================================================\r\n");
                diag_util_mprintf("     GPON ONU MAC D/S Flow Status\r\n");
                diag_util_mprintf("Flow ID | GEM Port | AES\r\n");
                first = 0;
            }
            diag_util_mprintf("   %4d |     %4d |   %1s\r\n",
                              flow_id, flow_attr.gemPortId, flow_attr.aesEn?"*":" ");
        }
    }
    if(found)
    {
        diag_util_mprintf("============================================================\r\n");
    }
    else
    {
        diag_util_mprintf("The D/S Flow is not created.\r\n");
    }
}

void _diag_rt_gpon_show_us_flow(void)
{
    uint32                  flow_id;
    rt_gpon_usFlow_t  flow_attr;
    uint32                  found=0;
    int32                   ret = RT_ERR_FAILED;
    uint32                  first=1;
    rt_gpon_schedule_info_t  schldInfo;

    ret = rt_gpon_scheInfo_get(&schldInfo);
    if ( RT_ERR_OK != ret )
    {
        diag_util_mprintf("Unable to fatch the device's capbility.\r\n");
    }

    for(flow_id=0;flow_id<schldInfo.max_flow;flow_id++)
    {
        ret = rt_gpon_usFlow_get(flow_id, &flow_attr);
        if(ret == RT_ERR_OK)
        {
            found = 1;
            if(first==1)
            {
                diag_util_mprintf("============================================================\r\n");
                diag_util_mprintf("    GPON ONU MAC U/S Flow Status\r\n");
                diag_util_mprintf("Flow ID | GEM Port | TCont | TContQ | AES\r\n");
                first = 0;
            }
            diag_util_mprintf("   %4d |     %4d |  %4d |   %4d |   %1s\r\n",
                              flow_id, flow_attr.gemPortId, flow_attr.tcontId, flow_attr.tcQueueId, flow_attr.aesEn?"*":" ");
        }
    }
    if(found)
    {
        diag_util_mprintf("============================================================\r\n");
    }
    else
    {
        diag_util_mprintf("The U/S Flow is not created.\r\n");
    }
}

void _diag_rt_gpon_show_omcc(void)
{
    int32 ret = RT_ERR_FAILED;
    rt_gpon_omcc_t omcc;

    ret = rt_gpon_omcc_get(&omcc);
    
    if(ret == RT_ERR_OK)
    {
        diag_util_mprintf("============================================================\r\n");
                diag_util_mprintf("    GPON ONU OMCC Status\r\n");
                diag_util_mprintf("Alloc ID | GEM Port\r\n");
        diag_util_mprintf("   %4d |     %4d \r\n",
                        omcc.allocId,omcc.gemId);
        diag_util_mprintf("============================================================\r\n");
    }
    else if(ret == RT_ERR_ENTRY_NOTFOUND)
    {
        diag_util_mprintf("The OMCC is not established.\r\n");
    }
    else
    {
        diag_util_mprintf("Unable to fatch the device's capbility.\r\n");
    }
}

/*
 * rt_gpon show ( tcont | tcont-queue | ds-flow | us-flow | omcc )
 */
cparser_result_t
cparser_cmd_rt_gpon_show_tcont_tcont_queue_ds_flow_us_flow_omcc(
    cparser_context_t *context)
{
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    if (0 == strcmp("tcont",TOKEN_STR(2)))
        _diag_rt_gpon_show_tcont();
    else if (0 == strcmp("tcont-queue",TOKEN_STR(2)))
        _diag_rt_gpon_show_tcont_queue();
    else if (0 == strcmp("ds-flow",TOKEN_STR(2)))
        _diag_rt_gpon_show_ds_flow();
    else if (0 == strcmp("us-flow",TOKEN_STR(2)))
        _diag_rt_gpon_show_us_flow();
    else if (0 == strcmp("omcc",TOKEN_STR(2)))
        _diag_rt_gpon_show_omcc();

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_gpon_show_tcont_tcont_queue_ds_flow_us_flow_omcc */


void _diag_rt_gpon_globalCounter_show(rt_gpon_pm_type_t type)
{
    switch(type)
    {
        case RT_GPON_DIAG_PM_TYPE_PHY:
        {
            rt_gpon_pm_counter_t counter;
            rt_gpon_pm_counter_t fecCnt;

            if(rt_gpon_pmCounter_get(RT_GPON_PM_TYPE_PHY_LOS, &counter)!=RT_ERR_OK)
            {
                diag_util_mprintf("Read Counter fail\r\n");
                return;
            }

            if(rt_gpon_pmCounter_get(RT_GPON_PM_TYPE_FEC ,&fecCnt)!=RT_ERR_OK)
            {
                diag_util_mprintf("Read Counter fail\r\n");
                return;
            }
            diag_util_mprintf("============================================================\r\n");
            diag_util_mprintf("     GPON ONU MAC Device Counter: PHY\r\n");
            diag_util_mprintf("BIP Error counts: %lu\r\n",(long unsigned int)counter.phyLos.bip_error_count);
            diag_util_mprintf("BIP Checked words: %llu\r\n",(long long unsigned int)counter.phyLos.total_words_protected_by_bip);
            diag_util_mprintf("FEC Correct bytes: %lu\r\n",(long unsigned int)fecCnt.fec.corrected_fec_bytes);
            diag_util_mprintf("FEC Correct codewords: %lu\r\n",(long unsigned int)fecCnt.fec.corrected_fec_codewords);
            diag_util_mprintf("FEC codewords Uncor: %lu\r\n",(long unsigned int)fecCnt.fec.uncorrected_fec_codewords);
            diag_util_mprintf("FEC total block: %lu\r\n",(long unsigned int)fecCnt.fec.total_fec_codewords);
            diag_util_mprintf("Superframe LOS  : %u\r\n",counter.phyLos.total_lods_event_count);
            diag_util_mprintf("============================================================\r\n");
            break;
        }
        case RT_GPON_DIAG_PM_TYPE_PLOAM:
        {
            rt_gpon_pm_counter_t ploam1;
            rt_gpon_pm_counter_t ploam2;
            rt_gpon_pm_counter_t ploam3;

            if(rt_gpon_pmCounter_get(RT_GPON_PM_TYPE_PLOAM_PM1, &ploam1)!=RT_ERR_OK)
            {
                diag_util_mprintf("Read Counter fail\r\n");
                return;
            }

            if(rt_gpon_pmCounter_get(RT_GPON_PM_TYPE_PLOAM_PM2, &ploam2)!=RT_ERR_OK)
            {
                diag_util_mprintf("Read Counter fail\r\n");
                return;
            }

            if(rt_gpon_pmCounter_get(RT_GPON_PM_TYPE_PLOAM_PM3, &ploam3)!=RT_ERR_OK)
            {
                diag_util_mprintf("Read Counter fail\r\n");
                return;
            }

            diag_util_mprintf("============================================================\r\n");
            diag_util_mprintf("     GPON ONU MAC Device Counter: DS PLOAM\r\n");
            diag_util_mprintf("Total RX PLOAMd          : %u\r\n",ploam1.ploam1.downstream_ploam_message_count);
            diag_util_mprintf("CRC/MIC Err RX PLOAM     : %u\r\n",ploam1.ploam1.ploam_mic_errors);
            diag_util_mprintf("Assign ONU ID RX PLOAMd  : %u\r\n",ploam2.ploam2.assign_onu_id_message_count);
            diag_util_mprintf("Ranging RX PLOAMd        : %u\r\n",ploam1.ploam1.ranging_time_message_count);
            diag_util_mprintf("============================================================\r\n");

            diag_util_mprintf("============================================================\r\n");
            diag_util_mprintf("     GPON ONU MAC Device Counter: US PLOAM\r\n");
            diag_util_mprintf("Total TX PLOAM  : %u\r\n", ploam3.ploam3.upstream_ploam_message_count);
            diag_util_mprintf("TX S/N PLOAM    : %u\r\n", ploam3.ploam3.serial_number_onu_in_band_message_count + ploam3.ploam3.serial_number_onu_amcc_message_count);
            diag_util_mprintf("TX Reg PLOAM    : %u\r\n", ploam3.ploam3.registration_message_count);
            diag_util_mprintf("============================================================\r\n");
            break;
        }
        case RT_GPON_DIAG_PM_TYPE_OMCI:
        {
            rt_gpon_pm_counter_t counter;

            if(rt_gpon_pmCounter_get(RT_GPON_PM_TYPE_OMCI, &counter)!=RT_ERR_OK)
            {
                diag_util_mprintf("Read Counter fail\r\n");
                return;
            }

            diag_util_mprintf("============================================================\r\n");
            diag_util_mprintf("     GPON ONU MAC Device Counter: OMCI\r\n");
            diag_util_mprintf("Total RX OMCI   : %lu\r\n",(long unsigned int)counter.omci.omci_rx_msg_count);
            diag_util_mprintf("Total TX OMCI   : %lu\r\n",(long unsigned int)counter.omci.omci_tx_msg_count);
            diag_util_mprintf("============================================================\r\n");
            break;
        }
        case RT_GPON_DIAG_PM_TYPE_GEM:
        {
            rt_gpon_pm_counter_t counter;

            if(rt_gpon_pmCounter_get(RT_GPON_PM_TYPE_XGEM, &counter)!=RT_ERR_OK)
            {
                diag_util_mprintf("Read Counter fail\r\n");
                return;
            }

            diag_util_mprintf("============================================================\r\n");
            diag_util_mprintf("     GPON ONU MAC Device Counter: DS GEM\r\n");
            diag_util_mprintf("RX GEM Blocks  : %lu\r\n",(long unsigned int)counter.xgem.total_received_xgem_frames);
            diag_util_mprintf("RX GEM Bytes   : %lu\r\n",(long unsigned int)counter.xgem.total_received_bytes_in_non_idle_xgem_frames);
            diag_util_mprintf("RX HEC errors  : %lu\r\n",(long unsigned int)counter.xgem.received_xgem_header_hec_errors);
            diag_util_mprintf("============================================================\r\n");
            diag_util_mprintf("============================================================\r\n");
            diag_util_mprintf("     GPON ONU MAC Device Counter: US GEM\r\n");
            diag_util_mprintf("TX GEM Blocks  : %u\r\n",counter.xgem.total_transmitted_xgem_frames);
            diag_util_mprintf("TX GEM Bytes   : %lu\r\n",(long unsigned int)counter.xgem.total_transmitted_bytes_in_non_idle_xgem_frames);
            diag_util_mprintf("============================================================\r\n");
            break;
        }
        default:
        {
            diag_util_mprintf("Unknown Counter Type\r\n");
            break;
        }
    }
}

/*
 * rt_gpon show counter global ( phy | plm | omci |gem )
 */
cparser_result_t
cparser_cmd_rt_gpon_show_counter_global_phy_plm_omci_gem(
    cparser_context_t *context)
{
    int32                               ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    if ((0 == strcmp("phy",TOKEN_STR(4))))
        _diag_rt_gpon_globalCounter_show(RT_GPON_DIAG_PM_TYPE_PHY);
    else if ((0 == strcmp("omci",TOKEN_STR(4))))
        _diag_rt_gpon_globalCounter_show(RT_GPON_DIAG_PM_TYPE_OMCI);
    else if ((0 == strcmp("plm",TOKEN_STR(4))))
        _diag_rt_gpon_globalCounter_show(RT_GPON_DIAG_PM_TYPE_PLOAM);
    else if ((0 == strcmp("gem",TOKEN_STR(4))))
        _diag_rt_gpon_globalCounter_show(RT_GPON_DIAG_PM_TYPE_GEM);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_gpon_show_counter_global_phy_plm_omci_gem */


int32 _diag_rt_gpon_flowCounter_show(uint32 idx, rt_gpon_flow_counter_type_t type)
{
    rt_gpon_flow_counter_t counter;

    if(rt_gpon_flowCounter_get(idx,type,&counter)!=RT_ERR_OK)
    {
        diag_util_mprintf("Read Counter fail[idx %d, type %d]\r\n",idx,type);
        return RT_ERR_OK;
    }

    switch(type)
    {
        case RT_GPON_CNT_TYPE_FLOW_DS_GEM:
        {
            diag_util_mprintf("============================================================\r\n");
            diag_util_mprintf("     GPON ONU MAC Device Counter: D/S Flow %d Gem\r\n",idx);
            diag_util_mprintf("D/S GEM packets : %lu\r\n",(long unsigned int)counter.dsgem.gem_block);
            diag_util_mprintf("D/S GEM bytes   : %lu\r\n",(long unsigned int)counter.dsgem.gem_byte);
            diag_util_mprintf("============================================================\r\n");
            break;
        }
        case RT_GPON_CNT_TYPE_FLOW_DS_ETH:
        {
            diag_util_mprintf("============================================================\r\n");
            diag_util_mprintf("     GPON ONU MAC Device Counter: D/S Flow %d Eth\r\n",idx);
            diag_util_mprintf("RX Eth packetts : %lu\r\n",(long unsigned int)counter.dseth.eth_pkt_rx);
            diag_util_mprintf("Fwd Eth packets : %lu\r\n",(long unsigned int)counter.dseth.eth_pkt_fwd);
            diag_util_mprintf("============================================================\r\n");
            break;
        }
        case RT_GPON_CNT_TYPE_FLOW_US_GEM:
        {
            diag_util_mprintf("============================================================\r\n");
            diag_util_mprintf("     GPON ONU MAC Device Counter: U/S Flow %d Gem\r\n",idx);
            diag_util_mprintf("U/S GEM counts  : %lu\r\n",(long unsigned int)counter.usgem.gem_block);
            diag_util_mprintf("U/S GEM bytes   : %llu\r\n",counter.usgem.gem_byte);
            diag_util_mprintf("============================================================\r\n");
            break;
        }
        case RT_GPON_CNT_TYPE_FLOW_US_ETH:
        {
            diag_util_mprintf("============================================================\r\n");
            diag_util_mprintf("     GPON ONU MAC Device Counter: U/S Flow %d Eth\r\n",idx);
            diag_util_mprintf("U/S Eth packets   : %lu\r\n",(long unsigned int)counter.useth.eth_cnt);
            diag_util_mprintf("============================================================\r\n");
            break;
        }
        default:
        {
            diag_util_mprintf("Unknown Counter Type\r\n");
            break;
        }
    }
    return RT_ERR_OK;

}

/*
 * rt_gpon show counter flow <UINT:flowId>
 */
cparser_result_t
cparser_cmd_rt_gpon_show_counter_flow_flowId(
    cparser_context_t *context,
    uint32_t  *flowId_ptr)
{
    rt_gpon_flow_counter_type_t    flow;
    int32                               ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_PARAM_RANGE_CHK((*flowId_ptr>127), CPARSER_ERR_INVALID_PARAMS);

    flow = RT_GPON_CNT_TYPE_FLOW_DS_GEM; 
    _diag_rt_gpon_flowCounter_show(*flowId_ptr,flow);

    flow = RT_GPON_CNT_TYPE_FLOW_US_GEM; 
    _diag_rt_gpon_flowCounter_show(*flowId_ptr,flow);

    flow = RT_GPON_CNT_TYPE_FLOW_DS_ETH; 
    _diag_rt_gpon_flowCounter_show(*flowId_ptr,flow);

    flow = RT_GPON_CNT_TYPE_FLOW_US_ETH; 
    _diag_rt_gpon_flowCounter_show(*flowId_ptr,flow);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_gpon_show_counter_flow_flowid */

/*
 * rt_gpon get egress rate
 */
cparser_result_t
cparser_cmd_rt_gpon_get_egress_rate(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 rate;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rt_gpon_egrBandwidthCtrlRate_get(&rate), ret);
    
    diag_util_mprintf("egress rate: %d\n",rate);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_gpon_get_egress_rate */

/*
 * rt_gpon set egress rate <UINT:rate> */
cparser_result_t
cparser_cmd_rt_gpon_set_egress_rate_rate(
    cparser_context_t *context,
    uint32_t  *rate_ptr)
{
    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(rt_gpon_egrBandwidthCtrlRate_set(*rate_ptr), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_gpon_set_egress_rate_rate */

/*
 * rt_gpon get dbru-status
 */
cparser_result_t
cparser_cmd_rt_gpon_get_dbru_status(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rt_gpon_attribute_t attributeValue;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    memset(&attributeValue,0,sizeof(rt_gpon_attribute_t));
    DIAG_UTIL_ERR_CHK(rt_gpon_attribute_get(RT_GPON_ATTRIBUTE_TYPE_DBRU_STATUS,&attributeValue), ret);

    diag_util_mprintf("DBRu status: %s\n",attributeValue.dbruStatus == 1 ? "Enable":"Disable");

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_gpon_get_dbru_status */

/*
 * rt_gpon set dbru-status ( disable | enable )
 */
cparser_result_t
cparser_cmd_rt_gpon_set_dbru_status_disable_enable(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rt_gpon_attribute_t attributeValue;
    DIAG_UTIL_PARAM_CHK();

    memset(&attributeValue,0,sizeof(rt_gpon_attribute_t));
    if ((0 == strcmp("disable",TOKEN_STR(3))))
        attributeValue.dbruStatus = 0;
    else
        attributeValue.dbruStatus = 1;
    DIAG_UTIL_ERR_CHK(rt_gpon_attribute_set(RT_GPON_ATTRIBUTE_TYPE_DBRU_STATUS,&attributeValue), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_gpon_set_dbru_status_disable_enable */

/*
 * rt_gpon get gem-block-size
 */
cparser_result_t
cparser_cmd_rt_gpon_get_gem_block_size(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rt_gpon_attribute_t attributeValue;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    memset(&attributeValue,0,sizeof(rt_gpon_attribute_t));
    DIAG_UTIL_ERR_CHK(rt_gpon_attribute_get(RT_GPON_ATTRIBUTE_TYPE_GEM_BLOCK_LENGTH,&attributeValue), ret);

    diag_util_mprintf("Gem block length: %d\n",attributeValue.gemBlockLength);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_gpon_get_gem_block_size */

/*
 * rt_gpon set gem-block-size <UINT:size> */
cparser_result_t
cparser_cmd_rt_gpon_set_gem_block_size_size(
    cparser_context_t *context,
    uint32_t  *size_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rt_gpon_attribute_t attributeValue;
    DIAG_UTIL_PARAM_CHK();

    memset(&attributeValue,0,sizeof(rt_gpon_attribute_t));
    attributeValue.gemBlockLength = *size_ptr;
    DIAG_UTIL_ERR_CHK(rt_gpon_attribute_set(RT_GPON_ATTRIBUTE_TYPE_GEM_BLOCK_LENGTH,&attributeValue), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_gpon_set_gem_block_size_size */

/*
 *rt_gpon set loop-gemport  gemport <UINT:gemport> ( disable | enable )*/
cparser_result_t 
cparser_cmd_rt_gpon_set_loop_gemport_gemport_gemport_disable_enable(cparser_context_t *context,
    uint32_t *gemport_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rt_enable_t enable;

    DIAG_UTIL_PARAM_CHK();
    
    if ((0 == strcmp("disable",TOKEN_STR(5))))
        enable = 0;
    else
        enable = 1;

    DIAG_UTIL_ERR_CHK(rt_gpon_loop_gemport_set(*gemport_ptr,enable), ret);

    return CPARSER_OK;

}    /* end of cparser_cmd_rt_gpon_set_loop_gemport_gemport_gemport_disable_enable */


    
cparser_result_t 
cparser_cmd_rt_gpon_get_loop_gemport_gemport_gemport(cparser_context_t *context,
    uint32_t *gemport_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rt_enable_t enable;   
    
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rt_gpon_loop_gemport_get(*gemport_ptr, &enable), ret);

    diag_util_mprintf("Gemport:%d loop status: %s\n", *gemport_ptr, enable == 1 ? "Enable":"Disable");


    return CPARSER_OK;
}    /* end of cparser_cmd_rt_gpon_get_loop_gemport_gemport_gemport */
