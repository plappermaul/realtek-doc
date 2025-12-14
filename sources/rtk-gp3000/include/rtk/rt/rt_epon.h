/*
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 *
 * *
 * $Revision: $
 * $Date: $
 *
 * Purpose : EPON MAC register access APIs
 *
 * Feature : Provide the APIs to access EPON MAC
 *
 */

#ifndef __RT_EPON_H__
#define __RT_EPON_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <hal/chipdef/chip.h>
#include <rtk/switch.h>

#define RT_EPON_KEY_SIZE 3
#define RT_EPON_MAX_QUEUE_PER_LLID 10

#if defined(CONFIG_SDK_KERNEL_LINUX)
#include <linux/ktime.h>
#endif

typedef struct rt_epon_llid_entry_s
{
    uint8          llidIdx;
    uint16         llid;
    rt_enable_t   valid;
    uint8          reportTimer;
    rt_enable_t   isReportTimeout; /*read only*/
    rt_mac_t      mac;
}rt_epon_llid_entry_t;

typedef struct rt_epon_regReq_s{
    uint8           llidIdx;
    rt_mac_t       mac;
    uint8           pendGrantNum;
    rt_enable_t    doRequest;
}rt_epon_regReq_t;

typedef struct rt_epon_churningKeyEntry_s
{
    uint8   llidIdx;
    uint8   keyIdx;
    uint8   churningKey[RT_EPON_KEY_SIZE];
    uint8   churningKey1[RT_EPON_KEY_SIZE];
    uint8   churningKey2[RT_EPON_KEY_SIZE];
}rt_epon_churningKeyEntry_t;

typedef struct rt_epon_llidCounter_s
{
    uint32  queueTxFrames[RT_EPON_MAX_QUEUE_PER_LLID];
    uint32  mpcpTxReport;
    uint32  mpcpRxGate;
    uint32  onuLlidNotBcst;
}rt_epon_llidCounter_t;

typedef struct rt_epon_counter_s
{
    rt_epon_llidCounter_t  llidIdxCnt;
    uint8   llidIdx; /*indicate LLID relative counter is get from which LLID index*/
    uint32  mpcpRxDiscGate;
    uint32  fecCorrectedBlocks;    
    uint32  fecUncorrectedBlocks;
    uint32  fecTtotalCodeword;
    uint32  fecCodingVio;
    uint32  notBcstBitLlid7fff;
    uint32  notBcstBitNotOnuLlid;
    uint32  bcstBitPlusOnuLLid;
    uint32  bcstNotOnuLLid;
    uint32  crc8Err;
    uint32  mpcpTxRegRequest;
    uint32  mpcpTxRegAck;
    uint32  eponRxEthCrcErr;
}rt_epon_counter_t;

typedef enum rt_epon_llid_scheduler_mode_e{
    RT_EPON_LLID_QUEUE_SCHEDULER_SP = 0, /* Strict Priority */
    RT_EPON_LLID_QUEUE_SCHEDULER_WRR = 1, /* Weighted Round Robin */
} rt_epon_llid_scheduler_mode_t;

typedef struct rt_epon_queueCfg_s{
    rt_epon_llid_scheduler_mode_t      scheduleType;
    uint32                              cir;
    uint32                              pir;
    uint32                              weight;
}rt_epon_queueCfg_t;



typedef enum rt_epon_register_pkt_flag_e{
    RT_EPON_REGISTER_FLAG_RESERVED = 0, 
    RT_EPON_REGISTER_FLAG_REREGISTER = 1, 
    RT_EPON_REGISTER_FLAG_DEREGISTER = 2, 
    RT_EPON_REGISTER_FLAG_ACK = 3, 
    RT_EPON_REGISTER_FLAG_NACK = 4 
} rt_epon_register_pkt_flag_t;

typedef struct rt_epon_report_threshold_e
{
    uint8          levelNum; 
    uint16         th1;
    uint16         th2;
    uint16         th3;
}rt_epon_report_threshold_t;



typedef struct rt_epon_mpcp_info_e
{
    uint32 laser_on_time; 
        /* Indicates the final laser on time after MPCP
        registration process is completed; initially
        laser on time is configured by system startup
        configuration; ONU will update
        laser on time if laser on time in MPCP registration
        is more than the initial value. {0x01 ~ 0xff}*/
    uint32 laser_off_time; /* Indicates the final laser off time after MPCP
        registration process is completed; initially
        laser off time is configured by system startup
        configuration; ONU will update laser off time if laser
        off time in MPCP registration is more than the initial
        value. {0x01 ~ 0xff}*/
    uint32 sync_time; /* Indicates the synchronization time from MPCP
    discovery date and MPCP registration frame. Note that
    this field is only valid if ONU has been in registered
    state. {0x0000 ~ 0xffff} */
    uint32 onu_discovery_info; 
        /* Indicates the 10G MPCP discovery information in
        registration request.
        Bit[0] : ONU 1G upstream capability.
        Bit[1] : ONU 10G upstream capability.
        Bit[4] : ONU 1G registration attempt.
        Bit[5] : ONU 10G registration attempt. */
    uint32 olt_discovery_info; 
        /* Indicates the 10G MPCP discovery information in
        discovery gate. ¡V this field is read only for SW
        Bit[0] : OLT 1G upstream capability.
        Bit[1] : OLT 10G upstream capability.
        Bit[4] : OLT 1G window opening state.
        Bit[5] : OLT 10G window opening state.*/
} rt_epon_mpcp_info_t;


typedef struct rt_epon_register_pkt_info_s{
    uint16     assignPort;
    uint8      flag;
    uint16     syncTime;
    uint8      echoPendingGrant;
}rt_epon_register_pkt_info_t;

typedef uint32 (*rt_epon_oam_rx_callback)(uint32 msgLen,uint8 *pMsg, uint8 llidIdx);

typedef uint32 (*rt_epon_mpcp_rx_callback)(rt_epon_register_pkt_info_t *info, uint8 llidIdx);




typedef enum rt_epon_info_event_type_e
{
    RT_EPON_INFO_MPCP_REG_SUCCESS,
    RT_EPON_INFO_MPCP_REG_FAIL,
    RT_EPON_INFO_MPCP_GATE_TIMEOUT,
    RT_EPON_INFO_LASER_OFF,
    RT_EPON_INFO_LASER_RECOVER,
    RT_EPON_INFO_MPCP_TIMEDRIFT,
    RT_EPON_INFO_MPCP_LLID_ENTRY_DISABLE,
    RT_EPON_INFO_MPCP_RX_REG_PKT,  /*not support in apollo family*/
    RT_EPON_INFO_MPCP_LLID_ENTRY_DISABLE_BY_SW,
    RT_EPON_INFO_END
}rt_epon_info_event_type_t;



typedef struct rt_epon_info_notify_e
{
    uint8                       llidIdx;
    rt_epon_info_event_type_t  event;
    uint32                      info;
}rt_epon_info_notify_t;


typedef uint32 (*rt_epon_info_notify_cb)(rt_epon_info_notify_t info);




typedef struct {
	rt_epon_info_event_type_t    type;      /*log event type*/
	uint32                       subType;   /*log event subtype type*/
#if defined(CONFIG_SDK_KERNEL_LINUX)
	ktime_t                      dbgLogTime; 														
#endif
} rt_epon_mpcp_eventlog_time_entry_t;


/* Function Name:
 *      rt_epon_init
 * Description:
 *      epon register level initial function
 * Input:
 *
 * Output:
 *      
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rt_epon_init(void);

/* Function Name:
 *      rt_epon_llid_entry_get
 * Description:
 *      Get llid entry
 * Input:
 *      llidIdx: LLID table index
 * Output:
 *      pLlidEntry: LLID entry 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rt_epon_llid_entry_get(rt_epon_llid_entry_t *pLlidEntry);  

/* Function Name:
 *      rt_epon_llid_entry_set
 * Description:
 *      Set llid entry
 * Input:
 *      llidIdx: LLID table index
 * Output:
 *      pLlidEntry: LLID entry 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rt_epon_llid_entry_set(rt_epon_llid_entry_t *pLlidEntry);

/* Function Name:
 *      rt_epon_registerReq_get
 * Description:
 *      Get register request relative parameter
 * Input:
 *      pRegEntry : register request relative parament
 * Output:
 *       None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rt_epon_registerReq_get(rt_epon_regReq_t *pRegEntry);

/* Function Name:
 *      rt_epon_registerReq_set
 * Description:
 *      Set register request relative parameter
 * Input:
 *       None
 * Output:
 *       pRegEntry : register request relative parament 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rt_epon_registerReq_set(rt_epon_regReq_t *pRegEntry);

/* Function Name:
 *      rt_epon_churningKey_get
 * Description:
 *      Get churning key entry
 * Input:
 *       None
 * Output:
 *       pEntry : churning key relative parameter 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rt_epon_churningKey_get(rt_epon_churningKeyEntry_t *pEntry);

/* Function Name:
 *      rt_epon_churningKey_set
 * Description:
 *      Set churning key entry
 * Input:
 *       pEntry : churning key relative parameter 
 * Output:
 *       None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rt_epon_churningKey_set(rt_epon_churningKeyEntry_t *pEntry);



/* Function Name:
 *      rt_epon_churningStatus_get
 * Description:
 *      Get EPON tripple churning status
 * Input:
 *	    none
 * Output:
 *      pEnabled: pointer of status
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_NULL_POINTER - NULL input parameter 
 * Note:
 */
extern int32 rt_epon_churningStatus_get(rt_enable_t *pEnable);


/* Function Name:
 *      rt_epon_usFecState_get
 * Description:
 *      Get upstream fec state
 * Input:
 *       None
 * Output:
 *       *pState : upstream FEC state 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rt_epon_usFecState_get(rt_enable_t *pState);


/* Function Name:
 *      rt_epon_usFecState_set
 * Description:
 *      Set upstream fec state
 * Input:
 *       state : upstream FEC state
 * Output:
 *       None 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rt_epon_usFecState_set(rt_enable_t state);


/* Function Name:
 *      rt_epon_dsFecState_get
 * Description:
 *      Get down-stream fec state
 * Input:
 *       None
 * Output:
 *       *pState : down-stream FEC state 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rt_epon_dsFecState_get(rt_enable_t *pState);


/* Function Name:
 *      rt_epon_dsFecState_set
 * Description:
 *      Set down-stream fec state
 * Input:
 *       state : down-stream FEC state
 * Output:
 *       None 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rt_epon_dsFecState_set(rt_enable_t state);

/* Function Name:
 *      rt_epon_dsFecState_set
 * Description:
 *      Set down-stream fec state
 * Input:
 *       None
 * Output:
 *       pCounter : EPON mib counter 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rt_epon_mibCounter_get(rt_epon_counter_t *pCounter);

/* Function Name:
 *      rt_epon_losState_get
 * Description:
 *      Get laser lose of signal state.
 * Input:
 *      pState LOS state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32 rt_epon_losState_get(rt_enable_t *pState);


/* Function Name:
 *      rt_epon_lofState_get
 * Description:
 *      Get laser lose of frame state.
 * Input:
 *      pState LOF state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32 rt_epon_lofState_get(rt_enable_t *pState);




/* Function Name:
 *      rt_epon_mpcp_gate_timer_set
 * Description:
 *      Set mpcp gate expire timer and deregister state.
 * Input:
 *      gate_timer      gate expire timer
 *      deregistration  gate expired deregister or not
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32 rt_epon_mpcp_gate_timer_set(uint32 gateTimer,rt_enable_t deregistration);



/* Function Name:
 *      rt_epon_mpcp_gate_timer_get
 * Description:
 *      Get mpcp gate expire timer and deregister state.
 * Input:
 *      *pGate_timer      gate expire timer
 *      *pDeregistration  gate expired deregister or not
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32 rt_epon_mpcp_gate_timer_get(uint32 *pGateTimer,rt_enable_t *pDeregistration);


/* Function Name:
 *      rt_epon_ponQueue_set
 * Description:
 *      Set the pon queue
 * Input:
 *      llid    - logical llid index
 *      queueId  - logical llid queue id
 *      pQueuecfg  - pointer to the queue configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The unit of granularity is 8Kbps.
 */
extern int32 rt_epon_ponQueue_set(uint32 llid, uint32 queueId, rt_epon_queueCfg_t *pQueuecfg);

/* Function Name:
 *      rt_epon_ponQueue_get
 * Description:
 *      Get the pon queue
 * Input:
 *      llid    - logical llid index
 *      queueId  - logical llid queue id
 * Output:
 *      pQueuecfg - pointer to the queue configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The unit of granularity is 8Kbps.
 */
extern int32 rt_epon_ponQueue_get(uint32 llid, uint32 queueId, rt_epon_queueCfg_t *pQueuecfg);

/* Function Name:
 *      rt_epon_ponQueue_del
 * Description:
 *      Delete the pon queue
 * Input:
 *      llid    - logical llid index
 *      queueId  - logical llid queue id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 */
extern int32 rt_epon_ponQueue_del(uint32 llid, uint32 queueId);

/* Function Name:
 *      rt_epon_egrBandwidthCtrlRate_get
 * Description:
 *      Get the egress bandwidth control rate.
 * Input:
 *      None
 * Output:
 *      pRate - egress bandwidth control rate
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      The unit of granularity is 1Kbps.
 */
extern int32 rt_epon_egrBandwidthCtrlRate_get(uint32 *pRate);

/* Function Name:
 *      rt_epon_egrBandwidthCtrlRate_set
 * Description:
 *      Set the pon port egress bandwidth control rate.
 * Input:
 *      rate - egress bandwidth control rate
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - Invalid port id
 *      RT_ERR_RATE    - Invalid input rate
 * Note:
 *      The unit of granularity is 1Kbps.
 */
extern int32 rt_epon_egrBandwidthCtrlRate_set(uint32 rate);

/* Function Name:
 *      rt_epon_oam_tx
 * Description:
 *      Send OAM Packet.
 * Input:
 *      msgLen - length of OAM Packet
 *      pMsg - content of OAM Packet
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32 rt_epon_oam_tx(uint32 msgLen,uint8 *pMsg,uint8 llidIdx);

/* Function Name:
 *      rt_epon_oam_tx
 * Description:
 *      Send OAM Packet.
 * Input:
 *      oamRx - call back function of OAM Packet
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32 rt_epon_oam_rx_callback_register(rt_epon_oam_rx_callback oamRx);

/* Function Name:
 *      rt_epon_dyinggasp_set
 * Description:
 *      Set dying gasp OAM Packet.
 * Input:
 *      msgLen - length of dying gasp OAM Packet
 *      pMsg - content o dying gaspf OAM Packet
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32 rt_epon_dyinggasp_set(uint32 msgLen,uint8 *pMsg);

/* Function Name:
 *      rt_epon_mpcp_info_get
 * Description:
 *      Get epon mpcp information
 * Input:
 * Output:
 *      info - mpcp relative information 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *
 *   This API routine is used to get generic ONU MPCP information and can be invoked at any time. 
 *   However some of fields like laser on laser_off sync_time and olt_discinfo will be valid if ONU is not in registered
 *   state. Note that field onu_discinfo and olt_discinfo are only appropriate for 10G ONU. 
 */
extern int32 rt_epon_mpcp_info_get(rt_epon_mpcp_info_t *info);




/* Function Name:
 *      rt_epon_mpcp_queue_threshold_set
 * Description:
 *      Set epon threshold report
 * Input:
 *      pThresholdRpt - threshole report setting
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_INPUT            - Error Input
 * Note:
 *      The the report level 1 for normal report type, max value is 4.
 */
extern int32
rt_epon_mpcp_queue_threshold_set(rt_epon_report_threshold_t *pThresholdRpt);

/* Function Name:
 *      rt_epon_mpcp_queue_threshold_get
 * Description:
 *      Get epon threshold report setting
 * Input:
 *      pThresholdRpt - threshole report setting
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_INPUT            - Error Input
 * Note:
 */
extern int32
rt_epon_mpcp_queue_threshold_get(rt_epon_report_threshold_t *pThresholdRpt);


/* Function Name:
 *      rt_epon_mpcpRxregisterInfoCb_register
 * Description:
 *      mpcp packet rx call back
 * Input:
 *      mpcpRx - call back function of mpcp register packet info update
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32 rt_epon_mpcpRxregisterInfoCb_register(rt_epon_mpcp_rx_callback mpcpRx);


/* Function Name:
 *      rt_epon_info_notify_callback_register
 * Description:
 *      Send OAM Packet.
 * Input:
 *      callback - call back function of  call back function of mpcp register packet info update
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32 rt_epon_info_notify_callback_register(rt_epon_info_notify_cb callback);


/* Function Name:
 *      rt_epon_ponLoopback_set
 * Description:
 *      config pon port loopback
 * Input:
 *      enable - set loopback status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32 rt_epon_ponLoopback_set(uint32 llid,rt_enable_t enable);



/* Function Name:
 *      rt_epon_mpcp_dbg_log_dump
 * Description:
 *      dump mpcp debug log
 * Input:
 *      overWrite - set log to overwrite mode
 *      clearLog  - dump log and clear old log
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32 rt_epon_mpcp_dbg_log_dump(rt_enable_t overWrite,rt_enable_t clearLog);

#endif