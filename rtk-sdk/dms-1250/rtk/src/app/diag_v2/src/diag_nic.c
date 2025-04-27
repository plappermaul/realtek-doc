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
 * $Revision: 101241 $
 * $Date: 2019-10-23 17:36:13 +0800 (Wed, 23 Oct 2019) $
 *
 * Purpose : Definition those NIC command and APIs in the SDK diagnostic shell.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) NIC
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/debug/rt_log.h>
#include <common/rt_error.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <drv/nic/nic.h>
#include <private/drv/nic/nic_diag.h>
#include <diag_util.h>
#include <diag_om.h>
#include <diag_str.h>
#include <parser/cparser_priv.h>

#ifdef CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE
  #include <rtrpc/rtrpc_diag.h>
#endif


#define TRACE_RX    (1)
#define TRACE_TX    (2)

#define START_OF_TX_RING   (0)
#define END_OF_TX_RING     (1)

#define START_OF_RX_RING   (0)

#ifdef CMD_NIC_RESET_DUMP_COUNTER
/*
 * nic ( reset | dump ) counter
 */
cparser_result_t cparser_cmd_nic_reset_dump_counter(cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('r' == TOKEN_CHAR(1,0))
    {
        DIAG_UTIL_ERR_CHK(drv_nic_cntr_clear(unit), ret);
    }
    else if ('d' == TOKEN_CHAR(1,0))
    {
        DIAG_UTIL_ERR_CHK(drv_nic_cntr_dump(unit), ret);
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
} /* end of cparser_cmd_nic_reset_dump_counter */
#endif

#ifdef CMD_NIC_DUMP_BUFFER_USAGE
/*
 * nic dump buffer-usage ( tx | rx)
 */
cparser_result_t cparser_cmd_nic_dump_buffer_usage_tx_rx(cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;


    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    {
        nic_dir_t   dir;

        if ('t' == TOKEN_CHAR(3, 0))
            dir = NIC_DIR_TX;
        else
            dir = NIC_DIR_RX;

        DIAG_UTIL_ERR_CHK(drv_nic_ringbuf_dump(unit, dir), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_NIC_DUMP_PKTHDR_MBUF_RAW_DATA
/*
 * nic dump pkthdr-mbuf { raw-data }
 */
cparser_result_t cparser_cmd_nic_dump_pkthdr_mbuf_raw_data(cparser_context_t *context)
{
    uint32      unit = 0;
    uint32      flag_rawdata = FALSE;
    int32       ret = RT_ERR_FAILED;
    uint32      end = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if (3 == TOKEN_NUM)
    {
        flag_rawdata = FALSE;
    }
    else if ('r' == context->parser->tokens[3].buf[0])
    {
        flag_rawdata = TRUE;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
        end = 31;
    else
        end = 7;
    DIAG_UTIL_ERR_CHK(drv_nic_pktHdrMBuf_dump(unit, NIC_PKTHDR_MBUF_MODE_RX, START_OF_RX_RING, end, flag_rawdata), ret);
    DIAG_UTIL_ERR_CHK(drv_nic_pktHdrMBuf_dump(unit, NIC_PKTHDR_MBUF_MODE_TX, START_OF_TX_RING, END_OF_TX_RING, flag_rawdata), ret);

    return CPARSER_OK;
} /* end of cparser_cmd_nic_dump_pkthdr_mbuf_raw_data */
#endif

#ifdef CMD_NIC_DUMP_PKTHDR_MBUF_TX_RING_IDX_RAW_DATA
/*
 * nic dump pkthdr-mbuf tx { <UINT:ring_idx> } { raw-data }
 */
cparser_result_t cparser_cmd_nic_dump_pkthdr_mbuf_tx_ring_idx_raw_data(cparser_context_t *context,
    uint32_t *ring_idx_ptr)
{
    uint32      unit = 0;
    uint32      flag_rawdata = FALSE;
    uint32      start = 0;
    uint32      end = 0;
    int32       ret = RT_ERR_FAILED;

    /* Don't check the (NULL == ring_idx_ptr) due to it is optional token */
    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if (6 == TOKEN_NUM)
    {
        DIAG_UTIL_ERR_CHK(diag_util_str2ul(&start, TOKEN_STR(4)), ret);
        end = start;
        flag_rawdata = TRUE;
    }
    else if (5 == TOKEN_NUM)
    {
        if ('r' == context->parser->tokens[4].buf[0])
        {
            start = START_OF_TX_RING;
            end = END_OF_TX_RING;
            flag_rawdata = TRUE;
        }
        else
        {
            DIAG_UTIL_ERR_CHK(diag_util_str2ul(&start, TOKEN_STR(4)), ret);
            end = start;
            flag_rawdata = FALSE;
        }
    }
    else
    {
        start = START_OF_TX_RING;
        end = END_OF_TX_RING;
        flag_rawdata = FALSE;
    }

    if (end > END_OF_TX_RING)
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(drv_nic_pktHdrMBuf_dump(unit, NIC_PKTHDR_MBUF_MODE_TX, start, end, flag_rawdata), ret);

    return CPARSER_OK;
} /* end of cparser_cmd_nic_dump_pkthdr_mbuf_tx_ring_idx_raw_data */
#endif

#ifdef CMD_NIC_DUMP_PKTHDR_MBUF_RX_RING_IDX_RAW_DATA
/*
 * nic dump pkthdr-mbuf rx { <UINT:ring_idx> } { raw-data }
 */
cparser_result_t cparser_cmd_nic_dump_pkthdr_mbuf_rx_ring_idx_raw_data(cparser_context_t *context,
    uint32_t *ring_idx_ptr)
{
    uint32      unit = 0;
    uint32      flag_rawdata = FALSE;
    uint32      start = 0;
    uint32      end = 0;
    int32       ret = RT_ERR_FAILED;

    /* Don't check the (NULL == ring_idx_ptr) due to it is optional token */
    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if (6 == TOKEN_NUM)
    {
        DIAG_UTIL_ERR_CHK(diag_util_str2ul(&start, TOKEN_STR(4)), ret);
        end = start;
        flag_rawdata = TRUE;
    }
    else if (5 == TOKEN_NUM)
    {
        if ('r' == context->parser->tokens[4].buf[0])
        {
            start = START_OF_RX_RING;
            if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
                end = 31;
            else
                end = 7;
            flag_rawdata = TRUE;
        }
        else
        {
            DIAG_UTIL_ERR_CHK(diag_util_str2ul(&start, TOKEN_STR(4)), ret);
            end = start;
            flag_rawdata = FALSE;
        }
    }
    else
    {
        start = START_OF_RX_RING;
        if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
            end = 31;
        else
            end = 7;
        flag_rawdata = FALSE;
    }

    if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        if (end > 31)
        {
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
        }
    }
    else
    {
        if (end > 7)
        {
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
        }
    }

    DIAG_UTIL_ERR_CHK(drv_nic_pktHdrMBuf_dump(unit, NIC_PKTHDR_MBUF_MODE_RX, start, end, flag_rawdata), ret);

    return CPARSER_OK;
} /* end of cparser_cmd_nic_dump_pkthdr_mbuf_rx_ring_idx_raw_data */
#endif

#ifdef CMD_NIC_SET_RX_STATE_DISABLE_ENABLE
/*
 * nic set rx state ( disable | enable )
 */
cparser_result_t cparser_cmd_nic_set_rx_state_disable_enable(cparser_context_t *context)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('e' == context->parser->tokens[4].buf[0])
    {
        DIAG_UTIL_ERR_CHK(drv_nic_rx_start(unit), ret);
    }
    else
    {
        DIAG_UTIL_ERR_CHK(drv_nic_rx_stop(unit), ret);
    }

    return CPARSER_OK;
} /* end of cparser_cmd_nic_set_rx_state_disable_enable */
#endif

#ifdef CMD_NIC_SET_RX_TX_TRACE_START_RAW_DATA_CPU_TAG
/*
 * nic set ( rx | tx ) trace start { raw-data } { cpu-tag }
 */
cparser_result_t cparser_cmd_nic_set_rx_tx_trace_start_raw_data_cpu_tag(cparser_context_t *context)
{
    uint32      unit = 0;
    uint32      flags = 0;
    uint32      trace = 0;
    int32       ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('r' == TOKEN_CHAR(2,0))
    {
        trace = TRACE_RX;
    }
    else if ('t' == TOKEN_CHAR(2,0))
    {
        trace = TRACE_TX;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(drv_nic_dbg_get(unit, &flags), ret);
    if (TRACE_RX == trace)
    {
        if (7 == TOKEN_NUM)
        {
            flags = flags | DEBUG_RX_RAW_LEN_BIT | DEBUG_RX_CPU_TAG_BIT;
        }
        else if (6 == TOKEN_NUM)
        {
            if ('r' == TOKEN_CHAR(5,0))
                flags = flags | DEBUG_RX_RAW_LEN_BIT;
            if ('c' == TOKEN_CHAR(5,0))
                flags = flags | DEBUG_RX_CPU_TAG_BIT;
        }
    }
    else if (TRACE_TX == trace)
    {
        if (7 == TOKEN_NUM)
        {
            flags = flags | DEBUG_TX_RAW_LEN_BIT | DEBUG_TX_CPU_TAG_BIT;
        }
        else if (6 == TOKEN_NUM)
        {
            if ('r' == TOKEN_CHAR(5,0))
                flags = flags | DEBUG_TX_RAW_LEN_BIT;
            if ('c' == TOKEN_CHAR(5,0))
                flags = flags | DEBUG_TX_CPU_TAG_BIT;
        }
    }

    DIAG_UTIL_ERR_CHK(drv_nic_dbg_set(unit, flags), ret);

    return CPARSER_OK;
} /* end of cparser_cmd_nic_set_rx_tx_trace_start_raw_data_cpu_tag */
#endif

#ifdef CMD_NIC_SET_RX_TX_TRACE_STOP
/*
 * nic set ( rx | tx ) trace stop
 */
cparser_result_t cparser_cmd_nic_set_rx_tx_trace_stop(cparser_context_t *context)
{
    uint32      unit = 0;
    uint32      flags = 0;
    uint32      trace = 0;
    int32       ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('r' == TOKEN_CHAR(2,0))
    {
        trace = TRACE_RX;
    }
    else if ('t' == TOKEN_CHAR(2,0))
    {
        trace = TRACE_TX;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(drv_nic_dbg_get(unit, &flags), ret);
    if (TRACE_RX == trace)
    {
        flags = flags & (~ DEBUG_RX_RAW_LEN_BIT);
        flags = flags & (~ DEBUG_RX_CPU_TAG_BIT);
    }
    else if (TRACE_TX == trace)
    {
        flags = flags & (~ DEBUG_TX_RAW_LEN_BIT);
        flags = flags & (~ DEBUG_TX_CPU_TAG_BIT);
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(drv_nic_dbg_set(unit, flags), ret);

    return CPARSER_OK;
} /* end of cparser_cmd_nic_set_rx_tx_trace_stop */
#endif

#ifdef CMD_NIC_GET
/*
 * nic get
 */
cparser_result_t cparser_cmd_nic_get(cparser_context_t *context)
{
    uint32      unit = 0;
    uint32      flags = 0, rx_status = 0;
    int32       ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(drv_nic_dbg_get(unit, &flags), ret);
    DIAG_UTIL_ERR_CHK(drv_nic_rx_status_get(unit, &rx_status), ret);

    if (rx_status)
    {
        diag_util_mprintf("Rx status : Enabled\n");
    }
    else
    {
        diag_util_mprintf("Rx status : Disabled\n");
    }

    diag_util_mprintf("Rx debug flags:\n");
    diag_util_printf("    +raw-data : ");
    if (flags & DEBUG_RX_RAW_LEN_BIT)
    {
        diag_util_mprintf("%s\n",DIAG_STR_ENABLE);
    }
    else
    {
        diag_util_mprintf("%s\n", DIAG_STR_DISABLE);
    }
    diag_util_printf("    +cpu-tag : ");
    if (flags & DEBUG_RX_CPU_TAG_BIT)
    {
        diag_util_mprintf("%s\n",DIAG_STR_ENABLE);
    }
    else
    {
        diag_util_mprintf("%s\n", DIAG_STR_DISABLE);
    }

    diag_util_mprintf("Tx debug flags:\n");
    diag_util_printf("    +raw-data : ");
    if (flags & DEBUG_TX_RAW_LEN_BIT)
    {
        diag_util_mprintf("%s\n",DIAG_STR_ENABLE);
    }
    else
    {
        diag_util_mprintf("%s\n", DIAG_STR_DISABLE);
    }
    diag_util_printf("    +cpu-tag : ");
    if (flags & DEBUG_TX_CPU_TAG_BIT)
    {
        diag_util_mprintf("%s\n",DIAG_STR_ENABLE);
    }
    else
    {
        diag_util_mprintf("%s\n", DIAG_STR_DISABLE);
    }

    return CPARSER_OK;
} /* end of cparser_cmd_nic_get */
#endif

#ifdef CMD_NIC_GET_RX_TX
/*
 * nic get ( rx | tx )
 */
cparser_result_t cparser_cmd_nic_get_rx_tx(cparser_context_t *context)
{
    uint32      unit = 0;
    uint32      flags = 0, rx_status = 0;
    int32       ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(drv_nic_dbg_get(unit, &flags), ret);

    if ('r' == TOKEN_CHAR(2,0))
    {
        DIAG_UTIL_ERR_CHK(drv_nic_rx_status_get(unit, &rx_status), ret);
        if (rx_status)
        {
            diag_util_mprintf("Rx status : Enabled\n");
        }
        else
        {
            diag_util_mprintf("Rx status : Disabled\n");
        }

        diag_util_mprintf("Rx debug flags:\n");
        diag_util_printf("    +raw-data : ");
        if (flags & DEBUG_RX_RAW_LEN_BIT)
        {
            diag_util_mprintf("%s\n",DIAG_STR_ENABLE);
        }
        else
        {
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);
        }
        diag_util_printf("    +cpu-tag : ");
        if (flags & DEBUG_RX_CPU_TAG_BIT)
        {
            diag_util_mprintf("%s\n",DIAG_STR_ENABLE);
        }
        else
        {
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);
        }
    }

    if ('t' == TOKEN_CHAR(2,0))
    {
        diag_util_mprintf("Tx debug flags:\n");
        diag_util_printf("    +raw-data : ");
        if (flags & DEBUG_TX_RAW_LEN_BIT)
        {
            diag_util_mprintf("%s\n",DIAG_STR_ENABLE);
        }
        else
        {
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);
        }
        diag_util_printf("    +cpu-tag : ");
        if (flags & DEBUG_TX_CPU_TAG_BIT)
        {
            diag_util_mprintf("%s\n",DIAG_STR_ENABLE);
        }
        else
        {
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);
        }
    }

    return CPARSER_OK;
} /* end of cparser_cmd_nic_get_rx_tx */
#endif

#ifdef CMD_NIC_SEND_PACKET_PORT_PORT_ALL_NUM_NUM_LEN
/*
 * nic send packet port ( <PORT_LIST:ports> | all ) num <UINT:num> { <UINT:len> }
 */
cparser_result_t cparser_cmd_nic_send_packet_port_ports_all_num_num_len(cparser_context_t *context,
    char **ports_ptr, uint32_t *num_ptr, uint32_t *len_ptr)
{
#ifndef CONFIG_SDK_NATIVE
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t          port = 0;
    diag_portlist_t     portlist;
    uint32      len = 64;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if (TOKEN_NUM == 8)
    {
        if (*len_ptr < 64 || *len_ptr > JUMBO_FRAME_SIZE_MAX)
        {
            osal_printf("%s():%d  Length:%d not supported\n", __FUNCTION__, __LINE__, *len_ptr);
            return CPARSER_OK;
        }
        len = *len_ptr;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(drv_nic_diagPkt_send(unit, port, *num_ptr, len, NULL), ret);
    }
#endif
    return CPARSER_OK;
}
#endif

#ifdef CMD_NIC_SEND_PACKET_TX_TAG_TAG_NUM_NUM_LEN
/*
 * nic send packet tx-tag <STRING:tag> num <UINT:num> { <UINT:len> }
 */
cparser_result_t cparser_cmd_nic_send_packet_tx_tag_tag_num_num_len(cparser_context_t *context,
    char **txTag_ptr, uint32_t *num_ptr, uint32_t *len_ptr)
{
#ifndef CONFIG_SDK_NATIVE
    uint32      unit = 0, size;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port = 0;
    uint32      len = 64;
    uint8       data[64];

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    size = strlen(*txTag_ptr);
    if (size > 64)
    {
        diag_util_printf("The length of CPU TX-tag is too long!  len:%d\n", size);
        return CPARSER_NOT_OK;
    }

    size = ((size - 2) + 1) / 2;
    if (diag_util_str2IntArray(data, (char *)*txTag_ptr, size) != RT_ERR_OK)
    {
        diag_util_printf("field data error!\n");
        return CPARSER_NOT_OK;
    }

    if (TOKEN_NUM == 8)
    {
        if (*len_ptr < 64 || *len_ptr > JUMBO_FRAME_SIZE_MAX)
        {
            osal_printf("%s():%d  Length:%d not supported\n", __FUNCTION__, __LINE__, *len_ptr);
            return CPARSER_OK;
        }
        len = *len_ptr;
    }

    DIAG_UTIL_ERR_CHK(drv_nic_diagPkt_send(unit, port, *num_ptr, len, data), ret);
#endif
    return CPARSER_OK;
}
#endif

#ifdef CMD_NIC_SET_DEBUG_FLAG
/*
 * nic set debug <UINT:flag>
 */
cparser_result_t cparser_cmd_nic_set_debug_flag(cparser_context_t *context, uint32_t *flag_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);


    DIAG_UTIL_ERR_CHK(drv_nic_dbg_set(unit, *flag_ptr), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_NIC_GET_DEBUG_FLAG
/*
 * nic get debug flag
 */
cparser_result_t cparser_cmd_nic_get_debug_flag(cparser_context_t *context)
{
    uint32      unit = 0;
    uint32      flags;
    int32       ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);


    DIAG_UTIL_ERR_CHK(drv_nic_dbg_get(unit, &flags), ret);
    diag_util_mprintf("debug flag : %d\n", flags);

    return CPARSER_OK;
}
#endif

#ifdef CMD_NIC_SET_TAG_MANUAL_TAG
/*
 * nic set tag manual <STRING:tag>
 */
cparser_result_t
cparser_cmd_nic_set_tag_manual_tag(
    cparser_context_t *context,
    char **tag_ptr)
{
#ifndef CONFIG_SDK_NATIVE
    uint32      unit = 0, size;
    int32       ret = RT_ERR_FAILED;
    uint8       data[32];
    nic_txTagStatus_t tagStatus;
    rtk_portmask_t portmask;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    size = strlen(*tag_ptr);
    if (size > 64)
    {
        diag_util_printf("The length of CPU TX-tag is too long!  len:%d\n", size);
        return CPARSER_NOT_OK;
    }
    tagStatus = NIC_TXTAG_MANUAL;

    size = ((size - 2) + 1) / 2;
    if (diag_util_str2IntArray(data, (char *)*tag_ptr, size) != RT_ERR_OK)
    {
        diag_util_printf("field data error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(drv_nic_tag_set(unit, tagStatus, data, &portmask), ret);
#endif
    return CPARSER_OK;
}   /* end of cparser_cmd_nic_set_tag_manual_tag */
#endif

#ifdef CMD_NIC_SET_TAG_NONE
/*
 * nic set tag none
 */
cparser_result_t
cparser_cmd_nic_set_tag_none(
    cparser_context_t *context)
{
#ifndef CONFIG_SDK_NATIVE
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    uint8       data[32];
    nic_txTagStatus_t tagStatus;
    rtk_portmask_t portmask;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(data, 0, sizeof(data));
    osal_memset(&portmask, 0, sizeof(portmask));
    tagStatus = NIC_TXTAG_NONE;

    DIAG_UTIL_ERR_CHK(drv_nic_tag_set(unit, tagStatus, data, &portmask), ret);
#endif
    return CPARSER_OK;
}   /* end of cparser_cmd_nic_set_tag_none */
#endif

#ifdef CMD_NIC_SET_TAG_AUTO_PORT_PORTS_ALL
/*
 * nic set tag auto port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_nic_set_tag_auto_port_ports_all(
    cparser_context_t *context,
    char **ports_ptr)
{
#ifndef CONFIG_SDK_NATIVE
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    uint8       data[32];
    diag_portlist_t     portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&portlist, 0, sizeof(portlist));
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    osal_memset(data, 0, sizeof(data));

    DIAG_UTIL_ERR_CHK(drv_nic_tag_set(unit, NIC_TXTAG_AUTO, data, &portlist.portmask), ret);
#endif
    return CPARSER_OK;
}   /* end of cparser_cmd_nic_set_tag_auto_port_ports_all */
#endif

#ifdef CMD_NIC_SET_TX_DATA_DATA_AUTO_LEN
/*
 * nic set tx-data ( <STRING:data> | auto ) <UINT:len>
 */
cparser_result_t
cparser_cmd_nic_set_tx_data_data_auto_len(
    cparser_context_t *context,
    char **data_ptr,
    uint32_t *len_ptr)
{
#ifndef CONFIG_SDK_NATIVE
    uint32      unit = 0, size;
    int32       ret = RT_ERR_FAILED;
    uint8       data[JUMBO_FRAME_SIZE_MAX];
    uint32      isAuto;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(data, 0, sizeof(data));

    if (*len_ptr < 64 || *len_ptr > JUMBO_FRAME_SIZE_MAX)
    {
        osal_printf("%s():%d  Length:%d not supported\n", __FUNCTION__, __LINE__, *len_ptr);
        return CPARSER_OK;
    }

    if ('a' == TOKEN_CHAR(3,0))
    {
        isAuto = 1;
        size = 4;
        osal_strcpy(*data_ptr, "0x00");
    }
    else
    {
        isAuto = 0;
        size = strlen(*data_ptr);
        if (size > *len_ptr * 2 + 2) /*2 is 0x*/
        {
            diag_util_printf("The length of packet data is too long!  len:%d\n", size);
            return CPARSER_NOT_OK;
        }
    }

    size = ((size - 2) + 1) / 2;
    if (diag_util_str2IntArray(data, (char *)*data_ptr, size) != RT_ERR_OK)
    {
        diag_util_printf("field data error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(drv_nic_txData_set(unit, isAuto, data, *len_ptr), ret);
#endif
    return CPARSER_OK;
}   /* end of cparser_cmd_nic_set_tx_data_data_auto_len */
#endif

#ifdef CMD_NIC_SEND_PACKET_NUM_NUM
/*
 * nic send packet num <UINT:num>
 */
cparser_result_t
cparser_cmd_nic_send_packet_num_num(
    cparser_context_t *context,
    uint32_t *num_ptr)
{
#ifndef CONFIG_SDK_NATIVE
    uint32      unit = 0;
    int32        ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(drv_nic_diagPkt_send(unit, *num_ptr), ret);
#endif
    return CPARSER_OK;
}   /* end of cparser_cmd_nic_send_packet_num_num */
#endif

#ifdef CMD_NIC_SET_DEBUG_APP_LOOPBACK_MODE_STATE_DISABLE_ENABLE
/*
 * nic set debug app-loopback-mode state ( disable | enable )
 */
cparser_result_t
cparser_cmd_nic_set_debug_app_loopback_mode_state_disable_enable(
    cparser_context_t *context)
{
#ifdef CONFIG_SDK_KERNEL_LINUX_USER_MODE
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_enable_t    enable;


    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if('e' == TOKEN_CHAR(5, 0))
    {
        enable = ENABLED;
    }
    else
    {
        enable = DISABLED;
    }

    DIAG_UTIL_ERR_CHK(drv_nic_loopbackMode_set(unit, enable), ret);
#else
    diag_util_printf("This command is only supported in SDK user mode\n");
#endif
    return CPARSER_OK;
}   /* end of cparser_cmd_nic_set_loopback_mode_state_disable_enable */
#endif

#ifdef CMD_NIC_GET_DEBUG_APP_LOOPBACK_MODE_STATE
/*
 * nic get debug app-loopback-mode state
 */
cparser_result_t
cparser_cmd_nic_get_debug_app_loopback_mode_state(
    cparser_context_t *context)
{
#ifdef CONFIG_SDK_KERNEL_LINUX_USER_MODE
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_enable_t    enable;


    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(drv_nic_loopbackMode_get(unit, &enable), ret);
    diag_util_mprintf("\tapp-loopback-mode : %s\n", enable ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
#else
    diag_util_printf("This command is only supported in SDK user mode\n");
#endif
    return CPARSER_OK;
}   /* end of cparser_cmd_nic_set_loopback_mode_state_disable_enable */
#endif

#ifdef CMD_NIC_GET_REG_REG16_REG8_ADDRESS
/*
 * nic get ( reg | reg16 | reg8 ) <UINT:address>
 */
cparser_result_t
cparser_cmd_nic_get_reg_reg16_reg8_address(
    cparser_context_t *context, uint32_t *address_ptr)
{
#ifdef CONFIG_SDK_DRIVER_EXTC_NIC
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    uint32          type, data;


    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('\0' == TOKEN_CHAR(2, 3))
        type = 0;
    else if ('1' == TOKEN_CHAR(2, 3))
        type = 1;
    else
        type = 2;

    DIAG_UTIL_ERR_CHK(drv_nic_reg_get(unit, type, *address_ptr, &data), ret);
    diag_util_mprintf("\treg : %#x\n", data);

    return CPARSER_OK;
#else
    DIAG_ERR_PRINT(RT_ERR_CHIP_NOT_SUPPORTED);
    return CPARSER_OK;
#endif /* CONFIG_SDK_DRIVER_EXTC_NIC */
}   /* end of cparser_cmd_nic_set_loopback_mode_state_disable_enable */
#endif

#ifdef CMD_NIC_SET_REG_REG16_REG8_ADDRESS_VALUE
/*
 * nic set ( reg | reg16 | reg8 ) <UINT:address>  <UINT:value>
 */
cparser_result_t
cparser_cmd_nic_set_reg_reg16_reg8_address_value(
    cparser_context_t *context, uint32_t *address_ptr, uint32_t *value_ptr)
{
#ifdef CONFIG_SDK_DRIVER_EXTC_NIC
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    uint32          type;


    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('\0' == TOKEN_CHAR(2, 3))
        type = 0;
    else if ('1' == TOKEN_CHAR(2, 3))
        type = 1;
    else
        type = 2;

    DIAG_UTIL_ERR_CHK(drv_nic_reg_set(unit, type, *address_ptr, *value_ptr), ret);

    return CPARSER_OK;
#else
    DIAG_ERR_PRINT(RT_ERR_CHIP_NOT_SUPPORTED);
    return CPARSER_OK;
#endif /* CONFIG_SDK_DRIVER_EXTC_NIC */
}   /* end of cparser_cmd_nic_set_loopback_mode_state_disable_enable */
#endif

