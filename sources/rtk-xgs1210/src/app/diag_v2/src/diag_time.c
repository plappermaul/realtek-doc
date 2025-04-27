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
 * $Revision: 95405 $
 * $Date: 2019-02-21 21:46:25 +0800 (Thu, 21 Feb 2019) $
 *
 * Purpose : Definition those TIME command and APIs in the SDK diagnostic shell.
 *
 * Feature : The file have include the following module and sub-modules
 *
 */

/*
 * Include Files
 */
#include <common/debug/rt_log.h>
#include <common/rt_error.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <rtk/time.h>
#include <diag_util.h>
#include <diag_om.h>
#include <diag_str.h>
#include <parser/cparser_priv.h>

#ifdef CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE
  #include <rtrpc/rtrpc_time.h>
#endif

#ifdef CMD_TIME_GET_PTP_PORT_PORTS_ALL_STATE
/*
 * time get ptp port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t cparser_cmd_time_get_ptp_port_ports_all_state(cparser_context_t *context,
    char **ports_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_port_t      port;
    diag_portlist_t portlist;
    rtk_enable_t    enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("State of PTP of Ports \n");
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if ((ret = rtk_time_portPtpEnable_get(unit, port, &enable)) != RT_ERR_OK)
        {
            if (RT_ERR_PORT_ID == ret)
            {
                diag_util_mprintf("\tPort %2d State : %s\n", port, "(Invalid Port)");
                continue;
            }

            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }

        diag_util_mprintf("\tPort %2d State : %s\n", port, (enable == ENABLED)? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_TIME_SET_PTP_PORT_PORTS_ALL_STATE_ENABLE_DISABLE
/*
 * time set ptp port ( <PORT_LIST:ports> | all ) state ( enable | disable )
 */
cparser_result_t cparser_cmd_time_set_ptp_port_ports_all_state_enable_disable(cparser_context_t *context,
    char **ports_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port;
    diag_portlist_t  portlist;
    rtk_enable_t    enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    enable = ('e' == TOKEN_CHAR(6, 0))? ENABLED : DISABLED;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if ((ret = rtk_time_portPtpEnable_set(unit, port, enable)) != RT_ERR_OK)
        {
            if (RT_ERR_PORT_ID == ret)
            {
                diag_util_mprintf("\tInvalid Port: %2d (Not Supported)\n", port);
                continue;
            }
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}
#endif


#ifdef CMD_TIME_GET_PTP_PORT_PORTS_ALL_TIME_STAMP_RX_MESSAGE_TYPE_DELAY_REQUEST_PEER_DELAY_REQUEST_PEER_DELAY_RESPONSE_SYNC_SEQUENCE_ID_SEQUENCE_ID
/*
 * time get ptp port ( <PORT_LIST:ports> | all ) time-stamp rx message-type ( delay-request | peer-delay-request | peer-delay-response | sync ) sequence-id <UINT:sequence_id>
 */
cparser_result_t cparser_cmd_time_get_ptp_port_ports_all_time_stamp_rx_message_type_delay_request_peer_delay_request_peer_delay_response_sync_sequence_id_sequence_id(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *sequence_id_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_port_t      port;
    diag_portlist_t portlist;
    rtk_time_ptpIdentifier_t identifier;
    rtk_time_timeStamp_t timeStamp;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    identifier.sequenceId = *sequence_id_ptr;
    if ('s' == TOKEN_CHAR(8, 0))
        identifier.msgType = PTP_MSG_TYPE_SYNC;
    else if ('d' == TOKEN_CHAR(8, 0))
        identifier.msgType = PTP_MSG_TYPE_DELAY_REQ;
    else {
        if ('q' == TOKEN_CHAR(8, 13))
            identifier.msgType = PTP_MSG_TYPE_PDELAY_REQ;
        else
            identifier.msgType = PTP_MSG_TYPE_PDELAY_RESP;
    }

    diag_util_mprintf("RX time stamp of ports \n");
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if ((ret = rtk_time_portPtpRxTimestamp_get(unit, port, identifier, &timeStamp)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
        if (PTP_MSG_TYPE_SYNC == identifier.msgType)
            diag_util_mprintf("\tPort %2d Sync : %10llu.%09u\n", port, timeStamp.sec, timeStamp.nsec);
        else if (PTP_MSG_TYPE_DELAY_REQ == identifier.msgType)
            diag_util_mprintf("\tPort %2d Delay-Request : %10llu.%09u\n", port, timeStamp.sec, timeStamp.nsec);
        else if (PTP_MSG_TYPE_PDELAY_REQ == identifier.msgType)
            diag_util_mprintf("\tPort %2d Peer-delay-Request : %10llu.%09u\n", port, timeStamp.sec, timeStamp.nsec);
        else
            diag_util_mprintf("\tPort %2d Peer-delay-Response : %10llu.%09u\n", port, timeStamp.sec, timeStamp.nsec);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_TIME_GET_PTP_PORT_PORTS_ALL_TIME_STAMP_TX_MESSAGE_TYPE_DELAY_REQUEST_PEER_DELAY_REQUEST_PEER_DELAY_RESPONSE_SYNC_SEQUENCE_ID_SEQUENCE_ID
/*
 * time get ptp port ( <PORT_LIST:ports> | all ) time-stamp tx message-type ( delay-request | peer-delay-request | peer-delay-response | sync ) sequence-id <UINT:sequence_id>
 */
cparser_result_t cparser_cmd_time_get_ptp_port_ports_all_time_stamp_tx_message_type_delay_request_peer_delay_request_peer_delay_response_sync_sequence_id_sequence_id(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *sequence_id_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_port_t      port;
    diag_portlist_t portlist;
    rtk_time_ptpIdentifier_t identifier;
    rtk_time_timeStamp_t timeStamp;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    identifier.sequenceId = *sequence_id_ptr;
    if ('s' == TOKEN_CHAR(8, 0))
        identifier.msgType = PTP_MSG_TYPE_SYNC;
    else if ('d' == TOKEN_CHAR(8, 0))
        identifier.msgType = PTP_MSG_TYPE_DELAY_REQ;
    else {
        if ('q' == TOKEN_CHAR(8, 13))
            identifier.msgType = PTP_MSG_TYPE_PDELAY_REQ;
        else
            identifier.msgType = PTP_MSG_TYPE_PDELAY_RESP;
    }

    diag_util_mprintf("TX time stamp of ports \n");
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if ((ret = rtk_time_portPtpTxTimestamp_get(unit, port, identifier, &timeStamp)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
        if (PTP_MSG_TYPE_SYNC == identifier.msgType)
            diag_util_mprintf("\tPort %2d Sync : %10llu.%09u\n", port, timeStamp.sec, timeStamp.nsec);
        else if (PTP_MSG_TYPE_DELAY_REQ == identifier.msgType)
            diag_util_mprintf("\tPort %2d Delay-Request : %10llu.%09u\n", port, timeStamp.sec, timeStamp.nsec);
        else if (PTP_MSG_TYPE_PDELAY_REQ == identifier.msgType)
            diag_util_mprintf("\tPort %2d Peer-delay-Request : %10llu.%09u\n", port, timeStamp.sec, timeStamp.nsec);
        else
            diag_util_mprintf("\tPort %2d Peer-delay-Response : %10llu.%09u\n", port, timeStamp.sec, timeStamp.nsec);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_TIME_GET_REFERENCE_TIME
/*
 * time get reference-time
 */
cparser_result_t cparser_cmd_time_get_reference_time(cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_time_timeStamp_t    timeStamp;
    rtk_port_t port = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("The time-stamp of reference time \n");
    if ((ret = rtk_time_portRefTime_get(unit, port, &timeStamp)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }
    diag_util_mprintf("\tRef-Time : %llu.%09u\n", timeStamp.sec, timeStamp.nsec);

    return CPARSER_OK;
}
#endif

#ifdef CMD_TIME_SET_REFERENCE_TIME_SECOND_SECOND_NANOSECOND_NANOSECOND
/*
 * time set reference-time second <UINT64:second> nanosecond <UINT:nanosecond>
 */
cparser_result_t cparser_cmd_time_set_reference_time_second_second_nanosecond_nanosecond(cparser_context_t *context,
    uint64_t *second_ptr,
    uint32_t *nanosecond_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_time_timeStamp_t    timeStamp;
    rtk_portmask_t portmask;
    rtk_switch_devInfo_t devInfo;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = diag_om_get_deviceInfo(unit, &devInfo)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return ret;
    }

    osal_memcpy(&portmask, &(devInfo.ether.portmask), sizeof(rtk_portmask_t));

    timeStamp.sec = *second_ptr;
    timeStamp.nsec = *nanosecond_ptr;
    if ((ret = rtk_time_portRefTime_set(unit, portmask, timeStamp)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_TIME_SET_REFERENCE_TIME_INCREASE_DECREASE_SECOND_SECOND_NANOSECOND_NANOSECOND
/*
 * time set reference-time ( increase | decrease ) second <UINT64:second> nanosecond <UINT:nanosecond>
 */
cparser_result_t cparser_cmd_time_set_reference_time_increase_decrease_second_second_nanosecond_nanosecond(cparser_context_t *context,
    uint64_t *second_ptr,
    uint32_t *nanosecond_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    uint32      sign;
    rtk_time_timeStamp_t    timeStamp;
    rtk_portmask_t portmask;
    rtk_switch_devInfo_t devInfo;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    sign = ('i' == TOKEN_CHAR(3, 0))? 0 : 1;

    if ((ret = diag_om_get_deviceInfo(unit, &devInfo)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return ret;
    }

    osal_memcpy(&portmask, &(devInfo.ether.portmask), sizeof(rtk_portmask_t));

    timeStamp.sec = *second_ptr;
    timeStamp.nsec = *nanosecond_ptr;
    if ((ret = rtk_time_portRefTimeAdjust_set(unit, portmask, sign, timeStamp)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_TIME_GET_REFERENCE_TIME_STATE
/*
 * time get reference-time state
 */
cparser_result_t cparser_cmd_time_get_reference_time_state(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_enable_t    enable;
    rtk_port_t  port = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_time_portRefTimeEnable_get(unit, port, &enable)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("\tRef-Time Clock : %s\n", (enable == ENABLED)? DIAG_STR_ENABLE : DIAG_STR_DISABLE);

    return CPARSER_OK;

}
#endif

#ifdef CMD_TIME_SET_REFERENCE_TIME_STATE_ENABLE_DISABLE
/*
 * time set reference-time state ( enable | disable )
 */
cparser_result_t cparser_cmd_time_set_reference_time_state_enable_disable(cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_enable_t    enable;
    rtk_portmask_t portmask;
    rtk_switch_devInfo_t devInfo;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = diag_om_get_deviceInfo(unit, &devInfo)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return ret;
    }

    osal_memcpy(&portmask, &(devInfo.ether.portmask), sizeof(rtk_portmask_t));

    enable = ('e' == TOKEN_CHAR(4, 0))? ENABLED : DISABLED;

    if ((ret = rtk_time_portRefTimeEnable_set(unit, portmask, enable)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_TIME_GET_REFERENCE_TIME_FREQUENCY
/*
 * time get reference-time frequency
 */
cparser_result_t
cparser_cmd_time_get_reference_time_frequency(
    cparser_context_t *context)
{
    uint32      unit = 0;
    int32        ret = RT_ERR_FAILED;
    uint32      freq;
    rtk_port_t  port = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("The frequency of reference time \n");
    if ((ret = rtk_time_portRefTimeFreq_get(unit, port, &freq)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }
    diag_util_mprintf("\tFrequency Of Ref-Time : 0x%x\n", freq);

    return CPARSER_OK;
}   /* end of cparser_cmd_time_get_reference_time_frequency */
#endif

#ifdef CMD_TIME_SET_REFERENCE_TIME_FREQUENCY_FREQUENCYVAL
/*
 * time set reference-time frequency <UINT:frequencyVal>
 */
cparser_result_t
cparser_cmd_time_set_reference_time_frequency_frequencyVal(
    cparser_context_t *context,
    uint32_t *frequencyVal_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_portmask_t portmask;
    rtk_switch_devInfo_t devInfo;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = diag_om_get_deviceInfo(unit, &devInfo)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return ret;
    }

    osal_memcpy(&portmask, &(devInfo.ether.portmask), sizeof(rtk_portmask_t));

    if ((ret = rtk_time_portRefTimeFreq_set(unit, portmask, *frequencyVal_ptr)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_time_set_reference_time_frequency_frequencyVal */
#endif

#ifdef CMD_TIME_GET_CORRECT_FIELD_TRANS_VAL_CORRECT_FIELD_ORI_CF_SECOND_SECOND_NANOSECOND_NANOSECOND
/*
 * time get correct-field-trans-val correct-field <UINT64:ori_cf> second <UINT64:second> nanosecond <UINT:nanosecond>
 */
cparser_result_t
cparser_cmd_time_get_correct_field_trans_val_correct_field_ori_cf_second_second_nanosecond_nanosecond(
    cparser_context_t *context,
    uint64_t *ori_cf_ptr,
    uint64_t *second_ptr,
    uint32_t *nanosecond_ptr)
{
    uint32      unit = 0;
    int32        ret = RT_ERR_FAILED;
    int64        transCf;
    rtk_time_timeStamp_t rxTimeStamp;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    rxTimeStamp.sec = *second_ptr;
    rxTimeStamp.nsec = *nanosecond_ptr;

    if ((ret = rtk_time_correctionFieldTransparentValue_get(unit, *ori_cf_ptr, rxTimeStamp, &transCf)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }
    diag_util_mprintf("Transparent Correction Field : %llx\n", transCf);

    return CPARSER_OK;
}   /* end of cparser_cmd_time_get_correct_field_trans_val_correct_field_64ori_cf_second_64second_nanosecond_nanosecond */
#endif

#ifdef CMD_TIME_GET_PTP_PORT_PORTS_ALL_MAC_ADDRESS
/*
 * time get ptp port ( <PORT_LIST:ports> | all ) mac-address
 */
cparser_result_t
cparser_cmd_time_get_ptp_port_ports_all_mac_address(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_port_t      port;
    diag_portlist_t portlist;
    char macStr[16];
    rtk_mac_t mac;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("PTP MAC address of ports \n");
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_time_portPtpMacAddr_get(unit, port, &mac), ret);
        DIAG_UTIL_ERR_CHK(diag_util_mac2str(macStr, mac.octet), ret);
        diag_util_mprintf("\tPort %2d MAC address : %s\n", port, macStr);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_time_get_ptp_port_ports_all_mac_address */
#endif

#ifdef CMD_TIME_SET_PTP_PORT_PORTS_ALL_MAC_ADDRESS_MAC
/*
 * time set ptp port ( <PORT_LIST:ports> | all ) mac-address <MACADDR:mac>
 */
cparser_result_t
cparser_cmd_time_set_ptp_port_ports_all_mac_address_mac(
    cparser_context_t *context,
    char **ports_ptr,
    cparser_macaddr_t *mac_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_port_t      port;
    diag_portlist_t portlist;
    rtk_mac_t mac;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(diag_util_str2mac(mac.octet, TOKEN_STR(6)), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_time_portPtpMacAddr_set(unit, port, &mac), ret);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_time_set_ptp_port_ports_all_mac_address_range_mac */
#endif

#ifdef CMD_TIME_GET_PTP_PORT_PORTS_ALL_MAC_ADDRESS_RANGE
/*
 * time get ptp port ( <PORT_LIST:ports> | all ) mac-address-range
 */
cparser_result_t
cparser_cmd_time_get_ptp_port_ports_all_mac_address_range(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port;
    diag_portlist_t  portmask;
    uint32 range;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portmask, 4) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("PTP MAC address range of ports \n");
    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_time_portPtpMacAddrRange_get(unit, port, &range), ret);

        diag_util_mprintf("\tPort %2d Range : %d\n", port, range);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_time_get_ptp_port_ports_all_mac_address_range */
#endif

#ifdef CMD_TIME_SET_PTP_PORT_PORTS_ALL_MAC_ADDRESS_RANGE_RANGE
/*
 * time set ptp port ( <PORT_LIST:ports> | all ) mac-address-range <UINT:range>
 */
cparser_result_t
cparser_cmd_time_set_ptp_port_ports_all_mac_address_range_range(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *range_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port;
    diag_portlist_t  portmask;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portmask, 4) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        if ((ret = rtk_time_portPtpMacAddrRange_set(unit, port, *range_ptr)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }

    }

    return CPARSER_OK;
}   /* end of cparser_cmd_time_set_ptp_port_ports_all_mac_address_range_range */
#endif

#ifdef CMD_TIME_GET_PTP_PORT_PORTS_ALL_TPID_OUTER_INNER
/*
 * time get ptp port ( <PORT_LIST:ports> | all ) tpid ( outer | inner)
 */
cparser_result_t
cparser_cmd_time_get_ptp_port_ports_all_tpid_outer_inner(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port;
    diag_portlist_t  portmask;
    rtk_vlanType_t type = INNER_VLAN;
    uint32 tpid[4], idx;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portmask, 4) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    switch (TOKEN_STR(6)[0])
    {
        case 'i':
            type = INNER_VLAN;
            break;
        case 'o':
            type = OUTER_VLAN;
            break;
        default:
            return CPARSER_NOT_OK;
    }

    diag_util_mprintf("%s VLAN TPID of PTP of Ports \n", (type == INNER_VLAN) ?"Inner": "Outer");

    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        for(idx = 0; idx < 4; idx ++)
            ret = rtk_time_portPtpVlanTpid_get(unit, port, type, idx, &tpid[idx]);

        if ((ret  != RT_ERR_OK) && (ret  != RT_ERR_CHIP_NOT_SUPPORTED))
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }

        if(ret == RT_ERR_OK)
            diag_util_mprintf("\tPort %2d TPID : 0x%x 0x%x 0x%x 0x%x\n", port, tpid[0], tpid[1], tpid[2], tpid[3]);
        else
            diag_util_mprintf("\tPort %2d does not support\n", port);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_time_get_ptp_port_ports_all_tpid_outer_inner */
#endif

#ifdef CMD_TIME_SET_PTP_PORT_PORTS_ALL_TPID_OUTER_INNER_ENTRY_TPID_IDX_TPID_TPID
/*
 * time set ptp port ( <PORT_LIST:ports> | all ) tpid ( outer | inner ) entry <UINT:tpid_idx> tpid <UINT:tpid>
 */
cparser_result_t
cparser_cmd_time_set_ptp_port_ports_all_tpid_outer_inner_entry_tpid_idx_tpid_tpid(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *tpid_idx_ptr,
    uint32_t *tpid_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port;
    diag_portlist_t  portmask;
    rtk_vlanType_t type = INNER_VLAN;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portmask, 4) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    switch (TOKEN_STR(6)[0])
    {
        case 'i':
            type = INNER_VLAN;
            break;
        case 'o':
            type = OUTER_VLAN;
            break;
        default:
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        if ((ret = rtk_time_portPtpVlanTpid_set(unit, port, type, *tpid_idx_ptr, *tpid_ptr)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }

    }

    return CPARSER_OK;
}   /* end of cparser_cmd_time_set_ptp_port_ports_all_tpid_outer_inner_tpid */
#endif

#ifdef CMD_TIME_GET_PTP_PORT_PORTS_ALL_TIME_OP
/*
 * time get ptp port ( <PORT_LIST:ports> | all ) time-op
 */
cparser_result_t
cparser_cmd_time_get_ptp_port_ports_all_time_op(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port;
    diag_portlist_t  portmask;
    rtk_time_operCfg_t operCfg;
    char strOper[32];

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portmask, 4) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("PTP operation of Ports \n");

    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        ret = rtk_time_portPtpOper_get(unit, port, &operCfg);

        if ((ret  != RT_ERR_OK) && (ret  != RT_ERR_CHIP_NOT_SUPPORTED))
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }

        switch(operCfg.oper)
        {
            case TIME_OPER_START:
                osal_strcpy(strOper, "Start");
                break;
            case TIME_OPER_STOP:
                osal_strcpy(strOper, "Stop");
                break;
            case TIME_OPER_LATCH:
                osal_strcpy(strOper, "Latch");
                break;
            default:
                break;
        }

        if(ret == RT_ERR_OK)
            diag_util_mprintf("\tPort %2d Operation %s, Rise trigger %s, Fall trigger %s\n", port, strOper, (operCfg.rise_tri==1)?"Enabled":"Disabled",
                (operCfg.fall_tri==1)?"Enabled":"Disabled");
        else
            diag_util_mprintf("\tPort %2d does not support\n", port);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_time_get_ptp_port_ports_all_time_op */
#endif

#ifdef CMD_TIME_SET_PTP_PORT_PORTS_ALL_TIME_OP_START_STOP_LATCH_RISE_TRI_ENABLE_DISABLE_FALL_TRI_ENABLE_DISABLE
/*
 * time set ptp port ( <PORT_LIST:ports> | all ) time-op ( start | stop | latch ) rise_tri ( enable | disable ) fall_tri ( enable | disable )
 */
cparser_result_t
cparser_cmd_time_set_ptp_port_ports_all_time_op_start_stop_latch_rise_tri_enable_disable_fall_tri_enable_disable(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port;
    diag_portlist_t  portmask;
    rtk_time_operCfg_t operCfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portmask, 4) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    osal_memset(&operCfg, 0, sizeof(operCfg));

    switch (TOKEN_STR(6)[0])
    {
        case 'l':
            operCfg.oper = TIME_OPER_LATCH;
            break;
        case 's':
            if('a' == TOKEN_STR(6)[2])
                operCfg.oper = TIME_OPER_START;
            else if('o' == TOKEN_STR(6)[2])
                operCfg.oper = TIME_OPER_STOP;
            else
                return CPARSER_NOT_OK;
            break;
        default:
            return CPARSER_NOT_OK;
    }

    switch (TOKEN_STR(8)[0])
    {
        case 'e':
            operCfg.rise_tri = ENABLED;
            break;
        case 'd':
            operCfg.rise_tri = DISABLED;
            break;
        default:
            return CPARSER_NOT_OK;
    }

    switch (TOKEN_STR(10)[0])
    {
        case 'e':
            operCfg.fall_tri = ENABLED;
            break;
        case 'd':
            operCfg.fall_tri = DISABLED;
            break;
        default:
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        if ((ret = rtk_time_portPtpOper_set(unit, port, &operCfg)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_time_set_ptp_port_ports_all_time_op_start_stop_latch_rise_tri_enable_disable_fall_tri_enable_disable */
#endif

#ifdef CMD_TIME_GET_PTP_PORT_PORTS_ALL_LATCH_TIME
/*
 * time get ptp port ( <PORT_LIST:ports> | all ) latch-time
 */
cparser_result_t
cparser_cmd_time_get_ptp_port_ports_all_latch_time(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port;
    diag_portlist_t  portmask;
    rtk_time_timeStamp_t timeStamp;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portmask, 4) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("PTP operation of Ports \n");

    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        ret = rtk_time_portPtpLatchTime_get(unit, port, &timeStamp);

        if ((ret  != RT_ERR_OK) && (ret  != RT_ERR_CHIP_NOT_SUPPORTED))
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }

        if(ret == RT_ERR_OK)
            diag_util_mprintf("\tPort %2d Latch-Time : %llu.%09u\n", timeStamp.sec, timeStamp.nsec);
        else
            diag_util_mprintf("\tPort %2d does not support\n", port);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_time_get_ptp_port_ports_all_latch_time */
#endif


