/*
 * Copyright (C) 2012 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Definition those XXX command and APIs in the SDK diagnostic shell.
 *
 * Feature : The file have include the following module and sub-modules
 *
 */

/*
 * Include Files
 */
#include <stdio.h>
#include <string.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <diag_util.h>
#include <parser/cparser_priv.h>
#include <diag_str.h>

#include <hal/common/halctrl.h>
#include <hal/chipdef/apollo/apollo_reg_struct.h>
#include <hal/chipdef/apollomp/rtk_apollomp_reg_struct.h>
#include <hal/chipdef/rtl9601b/rtk_rtl9601b_reg_struct.h>
#include <hal/chipdef/rtl9602c/rtk_rtl9602c_reg_struct.h>
#include <hal/chipdef/rtl9607c/rtk_rtl9607c_reg_struct.h>
#include <hal/chipdef/rtl9603cvd/rtk_rtl9603cvd_reg_struct.h>
#include <hal/mac/mem.h>
#include <hal/mac/reg.h>
#include <rtk/rate.h>

#ifdef CONFIG_SDK_APOLLOMP
#include <dal/apollomp/dal_apollomp.h>
#include <dal/apollomp/raw/apollomp_raw_qos.h>
#endif

/*
 * bandwidth init
 */
cparser_result_t
cparser_cmd_bandwidth_init(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    /*init rate module*/
    DIAG_UTIL_ERR_CHK(rtk_rate_init(), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_bandwidth_init */

/*
 * bandwidth get egress ifg
 */
cparser_result_t
cparser_cmd_bandwidth_get_egress_ifg(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_rate_egrBandwidthCtrlIncludeIfg_get(&enable), ret);

    diag_util_mprintf("Egress Rate counting ifg: %s\n",diagStr_ifgState[enable]);

    return CPARSER_OK;
}    /* end of cparser_cmd_bandwidth_get_egress_ifg */

/*
 * bandwidth set egress ifg ( exclude | include )
 */
cparser_result_t
cparser_cmd_bandwidth_set_egress_ifg_exclude_include(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();

    if('i'==TOKEN_CHAR(4,0))
        enable = ENABLED;
    else
        enable = DISABLED;

    DIAG_UTIL_ERR_CHK(rtk_rate_egrBandwidthCtrlIncludeIfg_set(enable), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_bandwidth_set_egress_ifg_exclude_include */

/*
 * bandwidth get egress ifg port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_bandwidth_get_egress_ifg_port_ports_all(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_portEgrBandwidthCtrlIncludeIfg_get(port,&enable), ret);
        diag_util_mprintf("port:%d Egress Rate counting ifg: %s\n",port,diagStr_ifgState[enable]);

    }

    return CPARSER_OK;
}    /* end of cparser_cmd_bandwidth_get_egress_port_ports_all_ifg */

/*
 * bandwidth set egress ifg port ( <PORT_LIST:ports> | all ) ( exclude | include )
 */
cparser_result_t
cparser_cmd_bandwidth_set_egress_ifg_port_ports_all_exclude_include(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();

    if('i'==TOKEN_CHAR(6,0))
        enable = ENABLED;
    else
        enable = DISABLED;



    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_portEgrBandwidthCtrlIncludeIfg_set(port,enable), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_bandwidth_set_egress_port_ports_all_ifg_exclude_include */

/*
 * bandwidth get egress port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_bandwidth_get_egress_port_ports_all(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port;
    diag_mask_t mask;
    uint32    queue;
    uint32    index;
    uint32    rate;
    uint32    ponId;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_ERR_CHK(rtk_switch_phyPortId_get(RTK_PORT_PON,&ponId), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        switch(DIAG_UTIL_CHIP_TYPE)
        {
    #ifdef CONFIG_SDK_RTL9601B
            case RTL9601B_CHIP_ID:
                break;
    #endif 
    #ifdef CONFIG_SDK_RTL9602C
            case RTL9602C_CHIP_ID:
                break;
    #endif 
    #ifdef CONFIG_SDK_RTL9607C
            case RTL9607C_CHIP_ID:
                break;
    #endif
    #ifdef CONFIG_SDK_RTL9603CVD
            case RTL9603CVD_CHIP_ID:
                break;
    #endif 
    #ifdef CONFIG_SDK_CA8279
            case CA8279_CHIP_ID:
                break;
    #endif 
    #ifdef CONFIG_SDK_CA8277B
            case CA8277B_CHIP_ID:
                break;
    #endif 
    #ifdef CONFIG_SDK_RTL8277C
            case RTL8277C_CHIP_ID:
                break;
    #endif
#if defined(CONFIG_SDK_RTL8198X)
    #ifdef CONFIG_SDK_RTL9607F
            case RTL9607F_CHIP_ID:
                break;
    #endif
#endif
    #ifdef CONFIG_SDK_APOLLOMP
            case APOLLOMP_CHIP_ID:
                break;
    #endif 
            default:
                if(ponId == port)
                    continue;
                break;
        }  

        DIAG_UTIL_ERR_CHK(rtk_rate_portEgrBandwidthCtrlRate_get(port,&rate), ret);
        diag_util_mprintf("port:%2d  rate:%d\n",port, rate);

#if defined(CONFIG_LUNA_G3_SERIES) && !defined(CONFIG_SDK_RTL8198X)
        break;
#else
        for(queue=0 ; queue<HAL_MAX_NUM_OF_QUEUE() ; queue++)
        {
        #if defined(CONFIG_SDK_RTL8198X)
			DIAG_UTIL_ERR_CHK(rtk_rate_egrQueueBwCtrlRate_get(port,queue,&rate), ret);
		#else
            DIAG_UTIL_ERR_CHK(rtk_rate_egrQueueBwCtrlMeterIdx_get(port,queue,&index), ret);
		#endif

            switch(DIAG_UTIL_CHIP_TYPE)
            {
        #ifdef CONFIG_SDK_RTL9601B
                case RTL9601B_CHIP_ID:
                    break;
        #endif 
        #ifdef CONFIG_SDK_RTL9602C
                case RTL9602C_CHIP_ID:
                    break;
        #endif 
        #ifdef CONFIG_SDK_RTL9607C
                case RTL9607C_CHIP_ID:
                    break;
        #endif 
        #ifdef CONFIG_SDK_RTL9603CVD
                case RTL9603CVD_CHIP_ID:
                    break;
        #endif
        #ifdef CONFIG_SDK_CA8279
                case CA8279_CHIP_ID:
                    break;
        #endif
        #ifdef CONFIG_SDK_CA8277B
                case CA8277B_CHIP_ID:
                    break;
        #endif
        #ifdef CONFIG_SDK_RTL8277C
                case RTL8277C_CHIP_ID:
                    break;
        #endif
#if defined(CONFIG_SDK_RTL8198X)
        #ifdef CONFIG_SDK_RTL9607F
                case RTL9607F_CHIP_ID:
                    break;
        #endif
#endif
        #ifdef CONFIG_SDK_APOLLOMP
                case APOLLOMP_CHIP_ID:
        #endif 
                default:
                    index = index - ((port%4)*8);
                    break;
            }  
		 #if defined(CONFIG_SDK_RTL8198X)
			diag_util_mprintf("         queue:%2d  rate:%10d\n", queue, rate);
		 #else
            diag_util_mprintf("         queue:%2d  apr-index:%2d\n",queue,index);
		 #endif
        }
#endif /*#if defined(CONFIG_LUNA_G3_SERIES) && !defined(CONFIG_SDK_RTL8198X)*/
    }

#if defined(CONFIG_SDK_RTL8198X)
	return CPARSER_OK;
#endif

    diag_util_mprintf("============================\n");
    diag_util_mprintf("(ARP index to meter mapping)\n");
    diag_util_mprintf("============================\n");
    switch(DIAG_UTIL_CHIP_TYPE)
    {
    #ifdef CONFIG_SDK_RTL9601B
            case RTL9601B_CHIP_ID:
            diag_util_mprintf("Port 0~2	 Meter 0~Meter   7\n");
                break;
    #endif 
    #ifdef CONFIG_SDK_RTL9602C
            case RTL9602C_CHIP_ID:
            diag_util_mprintf("Port 0	 Meter 0~Meter   7\n");
            diag_util_mprintf("Port 1	 Meter 8~Meter  15\n");
            diag_util_mprintf("Port 2	 Meter 16~Meter 23\n");
            diag_util_mprintf("Port 3	 Meter 24~Meter 31\n");
                break;
    #endif 
    #ifdef CONFIG_SDK_RTL9607C
            case RTL9607C_CHIP_ID:
            diag_util_mprintf("Port 0,6  Meter 0~Meter   7\n");
            diag_util_mprintf("Port 1,7  Meter 8~Meter  15\n");
            diag_util_mprintf("Port 2,8  Meter 16~Meter 23\n");
            diag_util_mprintf("Port 3,9  Meter 24~Meter 31\n");
            diag_util_mprintf("Port 4,10 Meter 32~Meter 39\n");
            diag_util_mprintf("Port 5    Meter 40~Meter 47\n");
                break;
    #endif
    #ifdef CONFIG_SDK_RTL9603CVD
            case RTL9603CVD_CHIP_ID:
            diag_util_mprintf("Port 0  Meter 0~Meter   7\n");
            diag_util_mprintf("Port 1  Meter 8~Meter  15\n");
            diag_util_mprintf("Port 2  Meter 16~Meter 23\n");
            diag_util_mprintf("Port 3  Meter 24~Meter 31\n");
            diag_util_mprintf("Port 4  Meter 32~Meter 39\n");
            diag_util_mprintf("Port 5  Meter 40~Meter 47\n");
                break;
    #endif
    #ifdef CONFIG_SDK_CA8279
            case CA8279_CHIP_ID:
                break;
    #endif
    #ifdef CONFIG_SDK_CA8277B
            case CA8277B_CHIP_ID:
                break;
    #endif
    #ifdef CONFIG_SDK_RTL8277C
            case RTL8277C_CHIP_ID:
                break;
    #endif
            default:
            diag_util_mprintf("Port 0,4	 Meter 0~Meter   7\n");
            diag_util_mprintf("Port 1,5	 Meter 8~Meter  15\n");
            diag_util_mprintf("Port 2,6	 Meter 16~Meter 23\n");
            diag_util_mprintf("Port 3 	 Meter 24~Meter 31\n");
                break;
    } 
    return CPARSER_OK;
}    /* end of cparser_cmd_bandwidth_get_egress_port_ports_all */

/*
 * bandwidth get egress port ( <PORT_LIST:ports> | all ) queue-id ( <MASK_LIST:qid> | all )
 */
cparser_result_t
cparser_cmd_bandwidth_get_egress_port_ports_all_queue_id_qid_all(
    cparser_context_t *context,
    char * *ports_ptr,
    char * *qid_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port;
    diag_mask_t mask;
    uint32 queue;
    uint32    index;
    uint32    ponId;
    rtk_enable_t enable;
	uint32    rate;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK8(mask, 6), ret);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_ERR_CHK(rtk_switch_phyPortId_get(RTK_PORT_PON,&ponId), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        switch(DIAG_UTIL_CHIP_TYPE)
        {
    #ifdef CONFIG_SDK_RTL9601B
            case RTL9601B_CHIP_ID:
                break;
    #endif 
    #ifdef CONFIG_SDK_RTL9602C
            case RTL9602C_CHIP_ID:
                break;
    #endif 
    #ifdef CONFIG_SDK_RTL9607C
            case RTL9607C_CHIP_ID:
                break;
    #endif 
    #ifdef CONFIG_SDK_RTL9603CVD
            case RTL9603CVD_CHIP_ID:
                break;
    #endif 
    #ifdef CONFIG_SDK_APOLLOMP
            case APOLLOMP_CHIP_ID:
                break;
    #endif
    #ifdef CONFIG_SDK_RTL8277C
            case RTL8277C_CHIP_ID:
                break;
    #endif
#if defined(CONFIG_SDK_RTL8198X)
    #ifdef CONFIG_SDK_RTL9607F
            case RTL9607F_CHIP_ID:
                break;
    #endif
#endif
            default:
                if(ponId == port)
                    continue;
                break;
        }

        DIAG_UTIL_MASK_SCAN(mask, queue)
        {
         #if defined(CONFIG_SDK_RTL8198X)
			DIAG_UTIL_ERR_CHK(rtk_rate_egrQueueBwCtrlRate_get(port,queue,&rate), ret);
		 #else
            DIAG_UTIL_ERR_CHK(rtk_rate_egrQueueBwCtrlMeterIdx_get(port,queue,&index), ret);
		 #endif

            switch(DIAG_UTIL_CHIP_TYPE)
            {
        #ifdef CONFIG_SDK_RTL9601B
                case RTL9601B_CHIP_ID:
                    break;
        #endif 
        #ifdef CONFIG_SDK_RTL9602C
                case RTL9602C_CHIP_ID:
                    break;
        #endif 
        #ifdef CONFIG_SDK_RTL9607C
                case RTL9607C_CHIP_ID:
                    break;
        #endif 
        #ifdef CONFIG_SDK_RTL9603CVD
                case RTL9603CVD_CHIP_ID:
                    break;
        #endif
        #ifdef CONFIG_SDK_CA8279
                case CA8279_CHIP_ID:
                    break;
        #endif
        #ifdef CONFIG_SDK_CA8277B
                case CA8277B_CHIP_ID:
                    break;
        #endif
        #ifdef CONFIG_SDK_RTL8277C
                case RTL8277C_CHIP_ID:
                    break;
        #endif
#if defined(CONFIG_SDK_RTL8198X)
        #ifdef CONFIG_SDK_RTL9607F
                case RTL9607F_CHIP_ID:
                    break;
        #endif
#endif
        #ifdef CONFIG_SDK_APOLLOMP
                case APOLLOMP_CHIP_ID:
        #endif 
                default:
                    index = index - ((port%4)*8);
                    break;
            }  

		#if defined(CONFIG_SDK_RTL8198X)
			diag_util_mprintf("port:%2d	queue:%2d  rate:%10d\n", port, queue, rate);
		#else
            DIAG_UTIL_ERR_CHK(rtk_rate_egrQueueBwCtrlEnable_get(port,queue,&enable), ret);

            diag_util_mprintf("port:%2d  queue:%2d  apr-index:%2d  state:%s\n",port,queue,index,diagStr_enable[enable]);
		#endif
        }
    }

#if defined(CONFIG_SDK_RTL8198X)
	return CPARSER_OK;
#endif

    diag_util_mprintf("============================\n");
    diag_util_mprintf("(ARP index to meter mapping)\n");
    diag_util_mprintf("============================\n");
    switch(DIAG_UTIL_CHIP_TYPE)
    {
    #ifdef CONFIG_SDK_RTL9601B
            case RTL9601B_CHIP_ID:
            diag_util_mprintf("Port 0~2	 Meter 0~Meter   7\n");
                break;
    #endif 
    #ifdef CONFIG_SDK_RTL9602C
            case RTL9602C_CHIP_ID:
            diag_util_mprintf("Port 0	 Meter 0~Meter   7\n");
            diag_util_mprintf("Port 1	 Meter 8~Meter  15\n");
            diag_util_mprintf("Port 2	 Meter 16~Meter 23\n");
            diag_util_mprintf("Port 3	 Meter 24~Meter 31\n");
                break;
    #endif 
    #ifdef CONFIG_SDK_RTL9607C
            case RTL9607C_CHIP_ID:
            diag_util_mprintf("Port 0,6  Meter 0~Meter   7\n");
            diag_util_mprintf("Port 1,7  Meter 8~Meter  15\n");
            diag_util_mprintf("Port 2,8  Meter 16~Meter 23\n");
            diag_util_mprintf("Port 3,9  Meter 24~Meter 31\n");
            diag_util_mprintf("Port 4,10 Meter 32~Meter 39\n");
            diag_util_mprintf("Port 5    Meter 40~Meter 47\n");
                break;
    #endif
    #ifdef CONFIG_SDK_RTL9603CVD
            case RTL9603CVD_CHIP_ID:
            diag_util_mprintf("Port 0  Meter 0~Meter   7\n");
            diag_util_mprintf("Port 1  Meter 8~Meter  15\n");
            diag_util_mprintf("Port 2  Meter 16~Meter 23\n");
            diag_util_mprintf("Port 3  Meter 24~Meter 31\n");
            diag_util_mprintf("Port 4  Meter 32~Meter 39\n");
            diag_util_mprintf("Port 5  Meter 40~Meter 47\n");
                break;
    #endif 
    #ifdef CONFIG_SDK_CA8279
            case CA8279_CHIP_ID:
                break;
    #endif 
    #ifdef CONFIG_SDK_CA8277B
            case CA8277B_CHIP_ID:
                break;
    #endif
    #ifdef CONFIG_SDK_RTL8277C
            case RTL8277C_CHIP_ID:
                break;
    #endif
            default:
            diag_util_mprintf("Port 0,4	 Meter 0~Meter   7\n");
            diag_util_mprintf("Port 1,5	 Meter 8~Meter  15\n");
            diag_util_mprintf("Port 2,6	 Meter 16~Meter 23\n");
            diag_util_mprintf("Port 3 	 Meter 24~Meter 31\n");
                break;
    } 

    return CPARSER_OK;
}    /* end of cparser_cmd_bandwidth_get_egress_port_ports_all_queue_id_qid_all */

/*
 * bandwidth set egress port ( <PORT_LIST:ports> | all ) queue-id <UINT:qid> apr-index <UINT:index>
 */
cparser_result_t
cparser_cmd_bandwidth_set_egress_port_ports_all_queue_id_qid_apr_index_index(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *qid_ptr,
    uint32_t  *index_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port;
    uint32    index;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        switch(DIAG_UTIL_CHIP_TYPE)
        {
    #ifdef CONFIG_SDK_RTL9601B
            case RTL9601B_CHIP_ID:
                index = *index_ptr;
                break;
    #endif 
    #ifdef CONFIG_SDK_RTL9602C
            case RTL9602C_CHIP_ID:
                index = *index_ptr;
                break;
    #endif 
    #ifdef CONFIG_SDK_RTL9607C
            case RTL9607C_CHIP_ID:
                index = *index_ptr;
                break;
    #endif
    #ifdef CONFIG_SDK_RTL9603CVD
            case RTL9603CVD_CHIP_ID:
                index = *index_ptr;
                break;
    #endif
    #ifdef CONFIG_SDK_CA8279
            case CA8279_CHIP_ID:
                index = *index_ptr;
                break;
    #endif
    #ifdef CONFIG_SDK_CA8277B
            case CA8277B_CHIP_ID:
                index = *index_ptr;
                break;
    #endif
    #ifdef CONFIG_SDK_RTL8277C
            case RTL8277C_CHIP_ID:
                index = *index_ptr;
                break;
    #endif
    #ifdef CONFIG_SDK_APOLLOMP
            case APOLLOMP_CHIP_ID:
    #endif 
            default:
                /*for diag command backward compatiable*/
                if(*index_ptr < 8 )
                    index = *index_ptr + ((port%4)*8);
                else
                    index = *index_ptr;
                break;
        }            
        DIAG_UTIL_ERR_CHK(rtk_rate_egrQueueBwCtrlMeterIdx_set(port,*qid_ptr,index), ret);

    }
    return CPARSER_OK;
}    /* end of cparser_cmd_bandwidth_set_egress_port_ports_all_queue_id_qid_apr_index_index */

/*
 * bandwidth set egress port ( <PORT_LIST:ports> | all ) queue-id <UINT:qid> share-bandwidth state ( disable | enable )
 */
cparser_result_t
cparser_cmd_bandwidth_set_egress_port_ports_all_queue_id_qid_share_bandwidth_state_disable_enable(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *qid_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();

    if('e'==TOKEN_CHAR(9,0))
        enable = ENABLED;
    else
        enable = DISABLED;

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_egrQueueBwCtrlEnable_set(port,*qid_ptr,enable), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_bandwidth_set_egress_port_ports_all_queue_id_qid_share_bandwidth_state_disable_enable */

/*
 * bandwidth set egress port ( <PORT_LIST:ports> | all ) queue-id <UINT:qid> rate <UINT:rate>
 */
cparser_result_t
cparser_cmd_bandwidth_set_egress_port_ports_all_queue_id_qid_rate_rate(
	cparser_context_t *context,
	char * *ports_ptr,
	uint32_t  *qid_ptr,
    uint32_t  *rate_ptr)
{
	int32 ret = RT_ERR_FAILED;
	diag_portlist_t portlist;
	rtk_port_t port;

	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
	DIAG_UTIL_PORTMASK_SCAN(portlist, port)
	{
		DIAG_UTIL_ERR_CHK(rtk_rate_egrQueueBwCtrlRate_set(port, *qid_ptr, *rate_ptr), ret);
	}
	return CPARSER_OK;
}	 /* end of cparser_cmd_bandwidth_set_egress_port_ports_all_queue_id_qid_rate_rate */

/*
 * bandwidth set egress port ( <PORT_LIST:ports> | all ) rate <UINT:rate>
 */
cparser_result_t
cparser_cmd_bandwidth_set_egress_port_ports_all_rate_rate(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *rate_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port;
    uint32 rate;

    DIAG_UTIL_PARAM_CHK();
    rate = *rate_ptr;

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_portEgrBandwidthCtrlRate_set(port,rate), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_bandwidth_set_egress_port_ports_all_rate_rate */

/*
 * bandwidth get ingress bypass-packet state
 */
cparser_result_t
cparser_cmd_bandwidth_get_ingress_bypass_packet_state(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    int32 diagRet = CPARSER_OK;
    uint32 enable= ENABLED;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO


        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK((reg_field_read(IGR_BWCTRL_GLB_CTRLr, BYPASS_ENf, &enable)), ret);
            break;
#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK((reg_field_read(APOLLOMP_IGR_BWCTRL_GLB_CTRLr, APOLLOMP_BYPASS_ENf, &enable)), ret);
            break;
#endif
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
            DIAG_UTIL_ERR_CHK((reg_field_read(RTL9601B_IGR_BWCTRL_GLB_CTRLr, RTL9601B_BYPASS_ENf, &enable)), ret);
            break;
#endif
#ifdef CONFIG_SDK_RTL9602C
        case RTL9602C_CHIP_ID:
            DIAG_UTIL_ERR_CHK((reg_field_read(RTL9602C_IGR_BWCTRL_GLB_CTRLr, RTL9602C_BYPASS_ENf, &enable)), ret);
            break;
#endif
#ifdef CONFIG_SDK_RTL9607C
        case RTL9607C_CHIP_ID:
            DIAG_UTIL_ERR_CHK((reg_field_read(RTL9607C_IGR_BWCTRL_GLB_CTRLr, RTL9607C_BYPASS_ENf, &enable)), ret);
            break;
#endif
#ifdef CONFIG_SDK_RTL9603CVD
        case RTL9603CVD_CHIP_ID:
            DIAG_UTIL_ERR_CHK((reg_field_read(RTL9603CVD_IGR_BWCTRL_GLB_CTRLr, RTL9603CVD_BYPASS_ENf, &enable)), ret);
            break;
#endif
        case UNKNOWN_CHIP_ID:
            /*for coverity dead code issue*/
            diag_util_mprintf("chip id:%x %s\n",DIAG_UTIL_CHIP_TYPE,DIAG_STR_NOTSUPPORT);
            break;

        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            diagRet = CPARSER_NOT_OK;
            break;
    }
    if(CPARSER_NOT_OK==diagRet && context!= NULL)
    {
        return diagRet;        
    }
    else
    {
        diag_util_mprintf("Ingress Rate byapss:%s\n",diagStr_enable[enable]);
        diag_util_mprintf("byapss packet format:\n");
        diag_util_mprintf("    -DMAC=01-80-C2-00-00-xx\n");
        diag_util_mprintf("    -IGMP/MLD control packet\n");
        diag_util_mprintf("    -8899 frames\n");
    
        return diagRet;
    }
}    /* end of cparser_cmd_bandwidth_get_ingress_bypass_packet_state */

/*
 * bandwidth set ingress bypass-packet state ( disable | enable )
 */
cparser_result_t
cparser_cmd_bandwidth_set_ingress_bypass_packet_state_disable_enable(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 enable=ENABLED;

    if('e'==TOKEN_CHAR(5,0))
        enable = ENABLED;
    else
        enable = DISABLED;

    DIAG_UTIL_PARAM_CHK();
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK((reg_field_write(IGR_BWCTRL_GLB_CTRLr, BYPASS_ENf, &enable)), ret);
            break;
#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK((reg_field_write(APOLLOMP_IGR_BWCTRL_GLB_CTRLr, APOLLOMP_BYPASS_ENf, &enable)), ret);
            break;
#endif
#ifdef CONFIG_SDK_RTL9601B
            case RTL9601B_CHIP_ID:
                DIAG_UTIL_ERR_CHK(reg_field_write(RTL9601B_IGR_BWCTRL_GLB_CTRLr, RTL9601B_BYPASS_ENf, &enable), ret);
                break;
#endif
#ifdef CONFIG_SDK_RTL9602C
            case RTL9602C_CHIP_ID:
                DIAG_UTIL_ERR_CHK(reg_field_write(RTL9602C_IGR_BWCTRL_GLB_CTRLr, RTL9602C_BYPASS_ENf, &enable), ret);
                break;
#endif
#ifdef CONFIG_SDK_RTL9607C
            case RTL9607C_CHIP_ID:
                DIAG_UTIL_ERR_CHK(reg_field_write(RTL9607C_IGR_BWCTRL_GLB_CTRLr, RTL9607C_BYPASS_ENf, &enable), ret);
                break;
#endif
#ifdef CONFIG_SDK_RTL9603CVD
            case RTL9603CVD_CHIP_ID:
                DIAG_UTIL_ERR_CHK(reg_field_write(RTL9603CVD_IGR_BWCTRL_GLB_CTRLr, RTL9603CVD_BYPASS_ENf, &enable), ret);
                break;
#endif
        case UNKNOWN_CHIP_ID:
            /*for coverity dead code issue*/
            diag_util_mprintf("chip id:%x %s\n",DIAG_UTIL_CHIP_TYPE,DIAG_STR_NOTSUPPORT);
            break;

        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            ret = CPARSER_NOT_OK;
            break;
    }

    return ret;
}    /* end of cparser_cmd_bandwidth_set_ingress_bypass_packet_state_disable_enable */

/*
 * bandwidth get ingress flow-control port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t
cparser_cmd_bandwidth_get_ingress_flow_control_port_ports_all_state(
    cparser_context_t *context,
    char * *ports_ptr)
{
    diag_portlist_t portlist;
    rtk_port_t port;
    rtk_enable_t enable=ENABLED;
    int32 ret = RT_ERR_FAILED;
    int32 diagRet = CPARSER_OK;        
    

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    diag_util_mprintf("Port     Flow-Control\n");
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#ifdef CONFIG_SDK_APOLLO
            case APOLLO_CHIP_ID:
                DIAG_UTIL_ERR_CHK(apollo_raw_qos_bwCtrlIgrRateFc_get(port, &enable), ret);
                break;
#endif
#ifdef CONFIG_SDK_APOLLOMP
            case APOLLOMP_CHIP_ID:
                DIAG_UTIL_ERR_CHK(apollomp_raw_qos_bwCtrlIgrRateFc_get(port, &enable), ret);
                break;
#endif

#ifdef CONFIG_SDK_RTL9601B
            case RTL9601B_CHIP_ID:
                DIAG_UTIL_ERR_CHK(reg_array_field_read(RTL9601B_IGR_BWCTRL_P_CTRLr, port, REG_ARRAY_INDEX_NONE, RTL9601B_MODEf, &enable), ret);
                break;
#endif
#ifdef CONFIG_SDK_RTL9602C
            case RTL9602C_CHIP_ID:
                DIAG_UTIL_ERR_CHK(reg_array_field_read(RTL9602C_IGR_BWCTRL_P_CTRLr, port, REG_ARRAY_INDEX_NONE, RTL9602C_MODEf, &enable), ret);
                break;
#endif
#ifdef CONFIG_SDK_RTL9607C
			case RTL9607C_CHIP_ID:
				DIAG_UTIL_ERR_CHK(reg_array_field_read(RTL9607C_IGR_BWCTRL_P_CTRLr, port, REG_ARRAY_INDEX_NONE, RTL9607C_MODEf, &enable), ret);
				break;
#endif
#ifdef CONFIG_SDK_RTL9603CVD
			case RTL9603CVD_CHIP_ID:
				DIAG_UTIL_ERR_CHK(reg_array_field_read(RTL9603CVD_IGR_BWCTRL_P_CTRLr, port, REG_ARRAY_INDEX_NONE, RTL9603CVD_MODEf, &enable), ret);
				break;
#endif

        case UNKNOWN_CHIP_ID:
            /*for coverity dead code issue*/
            diag_util_mprintf("chip id:%x %s\n",DIAG_UTIL_CHIP_TYPE,DIAG_STR_NOTSUPPORT);
            break;
            
            default:
                diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
                diagRet =  CPARSER_NOT_OK;
                break;
        }  
		if(CPARSER_OK==diagRet)
		{
            diag_util_mprintf("%-10u  %s\n", port, diagStr_enable[enable]);
        }
    }

    return diagRet;
}    /* end of cparser_cmd_bandwidth_get_ingress_flow_control_port_ports_all_state */

/*
 * bandwidth set ingress flow-control port ( <PORT_LIST:ports> | all ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_bandwidth_set_ingress_flow_control_port_ports_all_state_disable_enable(
    cparser_context_t *context,
    char * *ports_ptr)
{
    diag_portlist_t portlist;
    rtk_port_t port;
    rtk_enable_t enable;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    if ('e' == TOKEN_CHAR(7,0)) {
        enable = ENABLED;
    } else if ('d' == TOKEN_CHAR(7,0)) {
        enable = DISABLED;
    } else {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#ifdef CONFIG_SDK_APOLLO
            case APOLLO_CHIP_ID:
                DIAG_UTIL_ERR_CHK(apollo_raw_qos_bwCtrlIgrRateFc_set(port, enable), ret);
                break;
#endif
#ifdef CONFIG_SDK_APOLLOMP
            case APOLLOMP_CHIP_ID:
                DIAG_UTIL_ERR_CHK(apollomp_raw_qos_bwCtrlIgrRateFc_set(port, enable), ret);
                break;
#endif

#ifdef CONFIG_SDK_RTL9601B
            case RTL9601B_CHIP_ID:
                DIAG_UTIL_ERR_CHK(reg_array_field_write(RTL9601B_IGR_BWCTRL_P_CTRLr, port, REG_ARRAY_INDEX_NONE, RTL9601B_MODEf, &enable), ret);
                break;
#endif
#ifdef CONFIG_SDK_RTL9602C
            case RTL9602C_CHIP_ID:
                DIAG_UTIL_ERR_CHK(reg_array_field_write(RTL9602C_IGR_BWCTRL_P_CTRLr, port, REG_ARRAY_INDEX_NONE, RTL9602C_MODEf, &enable), ret);
                break;
#endif
#ifdef CONFIG_SDK_RTL9607C
			case RTL9607C_CHIP_ID:
				DIAG_UTIL_ERR_CHK(reg_array_field_write(RTL9607C_IGR_BWCTRL_P_CTRLr, port, REG_ARRAY_INDEX_NONE, RTL9607C_MODEf, &enable), ret);
				break;
#endif
#ifdef CONFIG_SDK_RTL9603CVD
			case RTL9603CVD_CHIP_ID:
				DIAG_UTIL_ERR_CHK(reg_array_field_write(RTL9603CVD_IGR_BWCTRL_P_CTRLr, port, REG_ARRAY_INDEX_NONE, RTL9603CVD_MODEf, &enable), ret);
				break;
#endif
            case UNKNOWN_CHIP_ID:
                /*for coverity dead code issue*/
                diag_util_mprintf("chip id:%x %s\n",DIAG_UTIL_CHIP_TYPE,DIAG_STR_NOTSUPPORT);
                break;

            default:
                diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;
                break;
        }        
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_bandwidth_set_ingress_flow_control_port_ports_all_state_disable_enable */

/*
 * bandwidth get ingress ifg port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_bandwidth_get_ingress_ifg_port_ports_all(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_portIgrBandwidthCtrlIncludeIfg_get(port,&enable), ret);
        diag_util_mprintf("port:%2d ifg: %s\n",port,diagStr_ifgState[enable]);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_bandwidth_get_ingress_ifg_port_ports_all */

/*
 * bandwidth set ingress ifg port ( <PORT_LIST:ports> | all ) ( exclude | include )
 */
cparser_result_t
cparser_cmd_bandwidth_set_ingress_ifg_port_ports_all_exclude_include(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();

    if('e'==TOKEN_CHAR(6,0))
        enable = DISABLED;
    else
        enable = ENABLED;

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_portIgrBandwidthCtrlIncludeIfg_set(port,enable), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_bandwidth_set_ingress_ifg_port_ports_all_exclude_include */

/*
 * bandwidth get ingress port ( <PORT_LIST:ports> | all ) rate
 */
cparser_result_t
cparser_cmd_bandwidth_get_ingress_port_ports_all_rate(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port;
    uint32 rate,enable;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_portIgrBandwidthCtrlRate_get(port,&rate), ret);

        diag_util_mprintf("port:%2d rate:%d\n",port , rate);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_bandwidth_get_ingress_port_ports_all_rate */

/*
 * bandwidth set ingress port ( <PORT_LIST:ports> | all ) rate <UINT:rate>
 */
cparser_result_t
cparser_cmd_bandwidth_set_ingress_port_ports_all_rate_rate(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *rate_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port;
    uint32 rate;

    DIAG_UTIL_PARAM_CHK();

    rate = *rate_ptr;

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_portIgrBandwidthCtrlRate_set(port,rate), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_bandwidth_set_ingress_port_ports_all_rate_rate */

/*
 * bandwidth get host entry <UINT:index> ( ingress | egress ) state
 */
cparser_result_t
cparser_cmd_bandwidth_get_host_entry_index_ingress_egress_state(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    rtk_enable_t state=ENABLED;
    int32 ret = RT_ERR_FAILED;
	
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    if ('i' == TOKEN_CHAR(5,0))
    {
		DIAG_UTIL_ERR_CHK(rtk_rate_hostIgrBwCtrlState_get(*index_ptr, &state), ret);
		diag_util_mprintf("Index %d: Ingress State: ", *index_ptr);
    }
    else if ('e' == TOKEN_CHAR(5,0))
    {
		DIAG_UTIL_ERR_CHK(rtk_rate_hostEgrBwCtrlState_get(*index_ptr, &state), ret);
		diag_util_mprintf("Index %d: Egress State: ", *index_ptr);
    }

    diag_util_mprintf("%s\n", diagStr_enable[state]);

    return CPARSER_OK;
}    /* end of cparser_cmd_bandwidth_get_host_entry_index_ingress_egress_state */

/*
 * bandwidth set host entry <UINT:index> ( ingress | egress ) state ( disable | enable ) 
 */
cparser_result_t
cparser_cmd_bandwidth_set_host_entry_index_ingress_egress_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    rtk_enable_t state=ENABLED;
    int32 ret = RT_ERR_FAILED;
	
    DIAG_UTIL_PARAM_CHK();

    if ('d' == TOKEN_CHAR(7,0))
    {
        state = DISABLED;
    }
    else if ('e' == TOKEN_CHAR(7,0))
    {
        state = ENABLED;
    }

    if ('i' == TOKEN_CHAR(5,0))
    {
		DIAG_UTIL_ERR_CHK(rtk_rate_hostIgrBwCtrlState_set(*index_ptr, state), ret);
    }
    else if ('e' == TOKEN_CHAR(5,0))
    {
		DIAG_UTIL_ERR_CHK(rtk_rate_hostEgrBwCtrlState_set(*index_ptr, state), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_bandwidth_set_host_entry_index_ingress_egress_state_disable_enable */

/*
 * bandwidth get host entry <UINT:index> mac-address
 */
cparser_result_t
cparser_cmd_bandwidth_get_host_entry_index_mac_address(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    rtk_mac_t mac;
    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
   
    DIAG_UTIL_ERR_CHK(rtk_rate_hostMacAddr_get(*index_ptr, &mac), ret); 		

    diag_util_mprintf("Index %d: MAC-Address: %s\n", *index_ptr, diag_util_inet_mactoa(&mac.octet[0]));

    return CPARSER_OK;
}    /* end of cparser_cmd_bandwidth_get_host_entry_index_mac_address */

/*
 * bandwidth set host entry <UINT:index> mac-address <MACADDR:mac>
 */
cparser_result_t
cparser_cmd_bandwidth_set_host_entry_index_mac_address_mac(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    cparser_macaddr_t  *mac_ptr)
{
    rtk_mac_t mac;
    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();

    osal_memcpy(&mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);    
    DIAG_UTIL_ERR_CHK(rtk_rate_hostMacAddr_set(*index_ptr, &mac), ret); 		

    return CPARSER_OK;
}    /* end of cparser_cmd_bandwidth_set_host_entry_index_mac_address_mac */

/*
 * bandwidth get host entry <UINT:index> meter_index
 */
cparser_result_t
cparser_cmd_bandwidth_get_host_entry_index_meter_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    uint32 meterIndex;
    int32 ret = RT_ERR_FAILED;
	
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

	DIAG_UTIL_ERR_CHK(rtk_rate_hostBwCtrlMeterIdx_get(*index_ptr, &meterIndex), ret);

    diag_util_mprintf("Index %d: Meter: %d\n", *index_ptr, meterIndex);


    return CPARSER_OK;
}    /* end of cparser_cmd_bandwidth_get_host_entry_index_meter_index */

/*
 * bandwidth set host entry <UINT:index> meter-index <UINT:meter_index>
 */
cparser_result_t
cparser_cmd_bandwidth_set_host_entry_index_meter_index_meter_index(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *meter_index_ptr)
{
    int32 ret = RT_ERR_FAILED;
	
    DIAG_UTIL_PARAM_CHK();

	DIAG_UTIL_ERR_CHK(rtk_rate_hostBwCtrlMeterIdx_set(*index_ptr, *meter_index_ptr), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_bandwidth_set_host_entry_index_meter_index_meter_index */

/*
 * bandwidth get host entry <UINT:index> ( ingress | egress ) meter-index
 */
cparser_result_t
cparser_cmd_bandwidth_get_host_entry_index_ingress_egress_meter_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 meterIndex;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    if ('i' == TOKEN_CHAR(5,0))
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_hostIgrBwCtrlMeterIdx_get(*index_ptr, &meterIndex), ret);
    }
    else 
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_hostEgrBwCtrlMeterIdx_get(*index_ptr, &meterIndex), ret);
    }
    
    diag_util_mprintf("Index %d: Meter: %d\n", *index_ptr, meterIndex);

    return CPARSER_OK;
}    /* end of cparser_cmd_bandwidth_get_host_entry_index_ingress_egress_meter_index */

/*
 * bandwidth set host entry <UINT:index> ( ingress | egress ) meter-index <UINT:meter_index>
 */
cparser_result_t
cparser_cmd_bandwidth_set_host_entry_index_ingress_egress_meter_index_meter_index(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *meter_index_ptr)
{
    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();
    

    if ('i' == TOKEN_CHAR(5,0))
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_hostIgrBwCtrlMeterIdx_set(*index_ptr, *meter_index_ptr), ret);
    }
    else 
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_hostEgrBwCtrlMeterIdx_set(*index_ptr, *meter_index_ptr), ret);
    }
    
    return CPARSER_OK;
}    /* end of cparser_cmd_bandwidth_set_host_entry_index_ingress_egress_meter_index_meter_index */

