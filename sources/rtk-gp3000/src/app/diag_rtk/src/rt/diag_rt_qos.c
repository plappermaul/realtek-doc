#include <stdio.h>
#include <string.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <diag_util.h>
#include <diag_str.h>
#include <parser/cparser_priv.h>
#include <rtk/rt/rt_qos.h>

/*
 * rt_qos init
 */
cparser_result_t
cparser_cmd_rt_qos_init(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    /*init rate module*/
    DIAG_UTIL_ERR_CHK(rt_qos_init(), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_qos_init */

/*
 * rt_qos get scheduling algorithm port ( <PORT_LIST:ports> | all ) queue-id <UINT:qid>
 */
cparser_result_t
cparser_cmd_rt_qos_get_scheduling_algorithm_port_ports_all_queue_id_qid(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *qid_ptr)
{
    diag_portlist_t portlist;
    rt_port_t port;
    int32 ret = RT_ERR_FAILED;
    uint32 phyPort;
    uint32 reg;
    rt_qos_queue_weights_t weight;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK((RTK_MAX_NUM_OF_QUEUE<=*qid_ptr), ret);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rt_qos_schedulingQueue_get(port,&weight), ret);
        diag_util_mprintf("port: %d queue: %d type:%s\n",port,*qid_ptr,(weight.weights[*qid_ptr]==0)?DIAG_STR_QUEUE_STRICT:DIAG_STR_QUEUE_WFQ );
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_qos_get_scheduling_algorithm_port_ports_all_queue_id_qid */

/*
 * rt_qos set scheduling algorithm port ( <PORT_LIST:ports> | all ) queue-id <UINT:qid> ( strict | wfq )
 */
cparser_result_t
cparser_cmd_rt_qos_set_scheduling_algorithm_port_ports_all_queue_id_qid_strict_wfq(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *qid_ptr)
{
    diag_portlist_t portlist;
    rt_port_t port;
    int32 ret = RT_ERR_FAILED;
    uint32 phyPort;
    uint32 reg;
    uint32 type;
    rt_qos_queue_weights_t weight;


    DIAG_UTIL_ERR_CHK((RTK_MAX_NUM_OF_QUEUE<=*qid_ptr), ret);

    DIAG_UTIL_PARAM_CHK();
    if ('s' == TOKEN_CHAR(8,0))
        type = 0;  /*strict*/
    else
        type = 1;  /*wfq*/

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rt_qos_schedulingQueue_get(port, &weight), ret);
        weight.weights[*qid_ptr] = type;
        DIAG_UTIL_ERR_CHK(rt_qos_schedulingQueue_set(port, &weight), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_qos_set_scheduling_algorithm_port_ports_all_queue_id_qid_strict_wfq */


/*
 * rt_qos get scheduling queue-weight port ( <PORT_LIST:ports> | all ) queue-id <UINT:qid>
 */
cparser_result_t
cparser_cmd_rt_qos_get_scheduling_queue_weight_port_ports_all_queue_id_qid(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *qid_ptr)
{
    diag_portlist_t portlist;
    rtk_port_t port;
    int32 ret = RT_ERR_FAILED;
    uint32 phyPort;
    uint32 reg,field;
    uint32 qid,phyQid;
    rtk_qos_queue_weights_t qWeight;
#ifdef CONFIG_SDK_APOLLO
    uint32 weight;
#endif

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    qid = *qid_ptr;

    DIAG_UTIL_ERR_CHK((RTK_MAX_NUM_OF_QUEUE<=*qid_ptr), ret);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_qos_schedulingQueue_get(port, &qWeight), ret);
        diag_util_mprintf("port: %d queue: %d weight:%d\n",port,*qid_ptr,qWeight.weights[*qid_ptr]);
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_qos_get_scheduling_queue_weight_port_ports_all_queue_id_qid */

/*
 * rt_qos set scheduling queue-weight port ( <PORT_LIST:ports> | all ) queue-id <UINT:qid> weight <UINT:weight>
 */
cparser_result_t
cparser_cmd_rt_qos_set_scheduling_queue_weight_port_ports_all_queue_id_qid_weight_weight(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *qid_ptr,
    uint32_t  *weight_ptr)
{
    diag_portlist_t portlist;
    rtk_port_t port;
    int32 ret = RT_ERR_FAILED;
    uint32 phyPort;
    uint32 reg,field;
    uint32 qid,phyQid;
    rtk_qos_queue_weights_t qWeight;
    uint32 weight;

    weight = *weight_ptr;
    qid = *qid_ptr;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK((RTK_MAX_NUM_OF_QUEUE<=*qid_ptr), ret);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_qos_schedulingQueue_get(port, &qWeight), ret);
        qWeight.weights[qid] = weight;
        DIAG_UTIL_ERR_CHK(rtk_qos_schedulingQueue_set(port, &qWeight), ret);
        break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_qos_set_scheduling_queue_weight_port_ports_all_queue_id_qid_weight_weight */


/*
 * rt_qos set dscp2pbit dscp <UINT:dscp> pbit <UINT:pbit>
 */
cparser_result_t
cparser_cmd_rt_qos_set_dscp2pbit_dscp_dscp_pbit_pbit(
    cparser_context_t *context,
    uint32_t  *dscp_ptr,
    uint32_t  *pbit_ptr)
{

    int32 ret = RT_ERR_FAILED;
    uint32 dscp;
    uint32 pbit;
    rt_qos_dscp2Pbit_t  dscp2PbitTbl;

        
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(*dscp_ptr > RTK_VALUE_OF_DSCP_MAX, ret);
    DIAG_UTIL_ERR_CHK(*pbit_ptr > RTK_DOT1P_PRIORITY_MAX, ret);

    dscp = *dscp_ptr;
    pbit = *pbit_ptr;
    
    DIAG_UTIL_ERR_CHK(rt_qos_dscp2pbit_get(&dscp2PbitTbl), ret);
    dscp2PbitTbl.dscp[dscp] = pbit;
    DIAG_UTIL_ERR_CHK(rt_qos_dscp2pbit_set(&dscp2PbitTbl), ret);


    return CPARSER_OK;
}    /* end of cparser_cmd_rt_qos_set_dscp2pbit_dscp_dscp_pbit_pbit */ 


/*
 * rt_qos get dscp2pbit
 */
cparser_result_t 
cparser_cmd_rt_qos_get_dscp2pbit(cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32      dscp;
    rt_qos_dscp2Pbit_t  dscp2PbitTbl;  

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rt_qos_dscp2pbit_get(&dscp2PbitTbl), ret);
    
    diag_util_mprintf("DSCP  Priority\n");
    for(dscp =0; dscp <= RTK_VALUE_OF_DSCP_MAX ; dscp++ )
    {
        diag_util_mprintf("%-4d  %-4d\n",dscp,dscp2PbitTbl.dscp[dscp]);
    }
    return CPARSER_OK;

}    /* end of cparser_cmd_rt_qos_get_dscp2pbit */

/*
 * rt_qos set ingress qos index <UINT:index> flow-id <UINT:flowid> vid <UINT:vid> priority <UINT:pri>
 */
cparser_result_t
cparser_cmd_rt_qos_set_ingress_qos_index_index_flow_id_flowid_vid_vid_priority_pri(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *flowid_ptr,
    uint32_t  *vid_ptr,
    uint32_t  *pri_ptr)
{
    rt_qos_ingress_qos_t igrQos;
    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(*index_ptr > 15, ret);
    DIAG_UTIL_ERR_CHK(*flowid_ptr > 127 && *flowid_ptr != 0xffff, ret);
    DIAG_UTIL_ERR_CHK(*vid_ptr > 4095 && *vid_ptr != 0xffff, ret);
    DIAG_UTIL_ERR_CHK(*pri_ptr > 7, ret);

    memset(&igrQos,0,sizeof(rt_qos_ingress_qos_t));

    igrQos.priority = *pri_ptr;

    if(*flowid_ptr != 0xffff)
    {
        igrQos.filterMask |= RT_QOS_INGRESS_QOS_FILTER_FLOW_ID;
        igrQos.flowId = *flowid_ptr;
    }
    if(*vid_ptr != 0xffff)
    {
        igrQos.filterMask |= RT_QOS_INGRESS_QOS_FILTER_VID;
        igrQos.vid=*vid_ptr;
    }

    DIAG_UTIL_ERR_CHK(rt_qos_ingress_qos_set(*index_ptr,&igrQos), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_qos_set_ingress_qos_index_index_flow_id_flowid_vid_vid_priority_pri */

/*
 * rt_qos del ingress qos index <UINT:index>
 */
cparser_result_t
cparser_cmd_rt_qos_del_ingress_qos_index_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(*index_ptr > 15, ret);
    DIAG_UTIL_ERR_CHK(rt_qos_ingress_qos_del(*index_ptr), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_qos_del_ingress_qos_index_index */

/*
 * rt_qos get ingress qos <UINT:index>
 */
cparser_result_t
cparser_cmd_rt_qos_get_ingress_qos_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    rt_qos_ingress_qos_t igrQos;
    char str[256]={0}, flow_str[64]={0}, vlan_str[64]={0};
    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(*index_ptr > 15, ret);

    memset(&igrQos,0,sizeof(rt_qos_ingress_qos_t));

    if(rt_qos_ingress_qos_get(*index_ptr,&igrQos) == RT_ERR_OK)
    {
        if(igrQos.filterMask & RT_QOS_INGRESS_QOS_FILTER_FLOW_ID)
        {
            snprintf(flow_str,sizeof(flow_str)," flowId %d,",igrQos.flowId);
        }
        if(igrQos.filterMask & RT_QOS_INGRESS_QOS_FILTER_VID)
        {
            snprintf(vlan_str,sizeof(vlan_str)," vid %d,",igrQos.vid);
        }
        snprintf(str,sizeof(str),"Index: %d,%s%s assign prioriry %d",*index_ptr,flow_str,vlan_str,igrQos.priority);
        diag_util_mprintf("%s\n",str);
    }
    else
    {
        diag_util_mprintf("Ingress QOS setting not found\n");
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_qos_get_ingress_qos_index */

/*
 * rt_qos get ingress qos valid
 */
cparser_result_t
cparser_cmd_rt_qos_get_ingress_qos_valid(
    cparser_context_t *context)
{
    rt_qos_ingress_qos_t igrQos;
    int32 i,found=0;
    char str[256]={0}, flow_str[64]={0}, vlan_str[64]={0};
    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    for(i=0;i<16;i++)
    {
        memset(&igrQos,0,sizeof(rt_qos_ingress_qos_t));

        if(rt_qos_ingress_qos_get(i,&igrQos) == RT_ERR_OK)
        {
            memset(flow_str, 0, sizeof(flow_str));
            memset(vlan_str, 0, sizeof(vlan_str));
            if(igrQos.filterMask & RT_QOS_INGRESS_QOS_FILTER_FLOW_ID)
            {
                snprintf(flow_str,sizeof(flow_str)," flowId %d,",igrQos.flowId);
            }
            if(igrQos.filterMask & RT_QOS_INGRESS_QOS_FILTER_VID)
            {
                snprintf(vlan_str,sizeof(vlan_str)," vid %d,",igrQos.vid);
            }
            snprintf(str,sizeof(str),"Index: %d,%s%s assign prioriry %d", i, flow_str, vlan_str, igrQos.priority);
            diag_util_mprintf("%s\n",str);
            found=1;
        }
    }

    if(found == 0)
    {
        diag_util_mprintf("Ingress QOS setting not found\n");
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_qos_get_ingress_qos_valid */