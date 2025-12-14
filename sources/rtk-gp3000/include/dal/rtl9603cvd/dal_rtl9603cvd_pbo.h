#ifndef _RTL9603CVD_RAW_PBO_H_
#define _RTL9603CVD_RAW_PBO_H_

/****************************************************************/
/* Header File inclusion                                        */
/****************************************************************/
#include <common/rt_type.h>
#include <rtk/port.h>
#include <rtk/pbo.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <dal/rtl9603cvd/dal_rtl9603cvd.h>


/****************************************************************/
/* Constant Definition                                          */
/****************************************************************/
typedef enum rtl9603cvd_pbo_ponMode_e {
    RTL9603CVD_PBO_PONMODE_GPON = 0,
    RTL9603CVD_PBO_PONMODE_EPON,
    RTL9603CVD_PBO_PONMODE_END
} rtl9603cvd_pbo_ponMode_t;

#define RTL9603CVD_PBO_PAGE_CNT_US_MAX    0x2000
#define RTL9603CVD_PBO_PAGE_CNT_DS_MAX    0x2000

#define RTL9603CVD_SWPBO_PAGE_CNT_MAX     0x1000
#define RTL9603CVD_SWPBO_SRAM_CNT_MAX     0x10
#define RTL9603CVD_SWPBO_DRAM_CNT_MAX     0x1000

/****************************************************************/
/* Type Definition                                              */
/****************************************************************/

typedef struct rtl9603cvd_pbo_cnt_s {
    uint32 rx_err_cnt;
    uint32 rx_drop_cnt;
    uint32 tx_empty_eob_cnt;
    uint32 rx_in_err;
} rtl9603cvd_pbo_cnt_t;

typedef struct rtl9603cvd_pbo_ponnic_cnt_s {
    uint16 pkt_ok_cnt_tx;
    uint16 pkt_ok_cnt_rx;
    uint16 pkt_err_cnt_tx;
    uint16 pkt_err_cnt_rx;
    uint32 pkt_miss_cnt;
} rtl9603cvd_pbo_ponnic_cnt_t;

typedef struct rtl9603cvd_pbo_group_cnt_s {
    uint32 rx_good_cnt;
    uint32 rx_bad_cnt;
    uint32 rx_cnt;
    uint32 tx_cnt;
    uint32 tx_frag_cnt;
    uint32 rx_frag_cnt;
} rtl9603cvd_pbo_group_cnt_t;

typedef struct rtl9603cvd_swPbo_cnt_s {
    uint32 rx_drop_cnt;
} rtl9603cvd_swPbo_cnt_t;

typedef struct rtl9603cvd_swPbo_group_cnt_s {
    uint32 rx_good_cnt;
    uint32 rx_bad_cnt;
    uint32 tx_cnt;
} rtl9603cvd_swPbo_group_cnt_t;

typedef struct rtl9603cvd_memUsage_s {
    uint32 us_base;
    uint32 us_offset;
    uint32 us_page_size;
    uint32 us_page_cnt;

    uint32 ds_base;
    uint32 ds_offset;
    uint32 ds_page_size;
    uint32 ds_page_cnt;
} rtl9603cvd_memUsage_t;

typedef enum rtl9603cvd_pbo_fc_status_e {
    RTL9603CVD_PBO_FLOWCONTROL_STATUS_CURRENT,
    RTL9603CVD_PBO_FLOWCONTROL_STATUS_LATCHED,
    RTL9603CVD_PBO_FLOWCONTROL_STATUS_END
} rtl9603cvd_pbo_fc_status_t;

typedef enum rtl9603cvd_pbo_dsQueue_type_e {
    RTL9603CVD_PBO_DSQUEUE_TYPE_NORMAL0,
    RTL9603CVD_PBO_DSQUEUE_TYPE_NORMAL1,
    RTL9603CVD_PBO_DSQUEUE_TYPE_HIGH,
    RTL9603CVD_PBO_DSQUEUE_TYPE_END
} rtl9603cvd_pbo_dsQueue_type_t;

typedef enum rtl9603cvd_pbo_dsQueueQos_type_e {
    RTL9603CVD_PBO_DSQUEUEQOS_TYPE_ROUNDROBIN,
    RTL9603CVD_PBO_DSQUEUEQOS_TYPE_STRICTPRIORITY,
    RTL9603CVD_PBO_DSQUEUEQOS_TYPE_END
} rtl9603cvd_pbo_dsQueueQos_type_t;

/* PON PBO */
extern int32 rtl9603cvd_raw_pbo_stopAllThreshold_set(uint32 threshold);
extern int32 rtl9603cvd_raw_pbo_stopAllThreshold_get(uint32 *pThreshold);
extern int32 rtl9603cvd_raw_pbo_globalThreshold_set(uint32 onThreshold, uint32 offThreshold);
extern int32 rtl9603cvd_raw_pbo_globalThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold);
extern int32 rtl9603cvd_raw_pbo_sidThreshold_set(uint32 sid, uint32 onThreshold, uint32 offThreshold);
extern int32 rtl9603cvd_raw_pbo_sidThreshold_get(uint32 sid,uint32 *pOnThreshold, uint32 *pOffThreshold);
extern int32 rtl9603cvd_raw_pbo_usState_set(rtk_enable_t enable);
extern int32 rtl9603cvd_raw_pbo_usState_get(rtk_enable_t *pEnable);
extern int32 rtl9603cvd_raw_pbo_dsState_set(rtk_enable_t enable);
extern int32 rtl9603cvd_raw_pbo_dsState_get(rtk_enable_t *pEnable);
extern int32 rtl9603cvd_raw_pbo_usStatus_get(rtk_enable_t *pEnable);
extern int32 rtl9603cvd_raw_pbo_dsStatus_get(rtk_enable_t *pEnable);
extern int32 rtl9603cvd_raw_pbo_usDscRunoutThreshold_set(uint32 dram, uint32 sram);
extern int32 rtl9603cvd_raw_pbo_usDscRunoutThreshold_get(uint32 *pDram, uint32 *pSram);
extern int32 rtl9603cvd_raw_pbo_dsDscRunoutThreshold_set(uint32 dram, uint32 sram);
extern int32 rtl9603cvd_raw_pbo_dsDscRunoutThreshold_get(uint32 *pDram, uint32 *pSram);
extern int32 rtl9603cvd_raw_pbo_usUsedPageCount_get(uint32 *pSram, uint32 *pDram);
extern int32 rtl9603cvd_raw_pbo_usMaxUsedPageCount_get(uint32 *pSram, uint32 *pDram);
extern int32 rtl9603cvd_raw_pbo_usMaxUsedPageCount_clear(void);
extern int32 rtl9603cvd_raw_pbo_dsUsedPageCount_get(uint32 *pSram, uint32 *pDram);
extern int32 rtl9603cvd_raw_pbo_dsMaxUsedPageCount_get(uint32 *pSram, uint32 *pDram);
extern int32 rtl9603cvd_raw_pbo_dsMaxUsedPageCount_clear(void);
extern int32 rtl9603cvd_raw_pbo_dsQUsedPageCount_get(rtl9603cvd_pbo_dsQueue_type_t qType, uint32 *pCounter);
extern int32 rtl9603cvd_raw_pbo_dsQMaxUsedPageCount_get(rtl9603cvd_pbo_dsQueue_type_t qType, uint32 *pCounter);
extern int32 rtl9603cvd_raw_pbo_dsQMaxUsedPageCount_clear(void);
extern int32 rtl9603cvd_raw_pbo_usCounterGroupMember_add(uint32 group, uint32 sid);
extern int32 rtl9603cvd_raw_pbo_usCounterGroupMember_clear(uint32 group);
extern int32 rtl9603cvd_raw_pbo_usCounterGroupMember_get(uint32 group, uint32 sid, rtk_enable_t *pEnabled);
extern int32 rtl9603cvd_raw_pbo_usCounter_get(rtl9603cvd_pbo_cnt_t *pCnt);
extern int32 rtl9603cvd_raw_pbo_usCounter_clear(void);
extern int32 rtl9603cvd_raw_pbo_usPonnicCounter_get(rtl9603cvd_pbo_ponnic_cnt_t *pCnt);
extern int32 rtl9603cvd_raw_pbo_usPonnicCounter_clear(void);
extern int32 rtl9603cvd_raw_pbo_usGroupCounter_clear(uint32 group);
extern int32 rtl9603cvd_raw_pbo_usGroupCounter_get(uint32 group, rtl9603cvd_pbo_group_cnt_t *pCnt);
extern int32 rtl9603cvd_raw_pbo_dsCounterGroupMember_add(uint32 group, uint32 sid);
extern int32 rtl9603cvd_raw_pbo_dsCounterGroupMember_clear(uint32 group);
extern int32 rtl9603cvd_raw_pbo_dsCounterGroupMember_get(uint32 group, uint32 sid, rtk_enable_t *pEnabled);
extern int32 rtl9603cvd_raw_pbo_dsCounter_get(rtl9603cvd_pbo_cnt_t *pCnt);
extern int32 rtl9603cvd_raw_pbo_dsCounter_clear(void);
extern int32 rtl9603cvd_raw_pbo_dsPonnicCounter_get(rtl9603cvd_pbo_ponnic_cnt_t *pCnt);
extern int32 rtl9603cvd_raw_pbo_dsPonnicCounter_clear(void);
extern int32 rtl9603cvd_raw_pbo_dsGroupCounter_clear(uint32 group);
extern int32 rtl9603cvd_raw_pbo_dsGroupCounter_get(uint32 group, rtl9603cvd_pbo_group_cnt_t *pCnt);
extern int32 rtl9603cvd_raw_pbo_flowcontrolStatus_get(uint32 sid, rtl9603cvd_pbo_fc_status_t latched, rtk_enable_t *pStatus);
extern int32 rtl9603cvd_raw_pbo_flowcontrolLatchStatus_clear(uint32 sid);
extern int32 rtl9603cvd_raw_pbo_dsSidToQueueMap_set(uint32 sid, rtl9603cvd_pbo_dsQueue_type_t qType);
extern int32 rtl9603cvd_raw_pbo_dsSidToQueueMap_get(uint32 sid, rtl9603cvd_pbo_dsQueue_type_t *pQType);
extern int32 rtl9603cvd_raw_pbo_dsQueueQos_set(rtl9603cvd_pbo_dsQueueQos_type_t qosType);
extern int32 rtl9603cvd_raw_pbo_dsQueueQos_get(rtl9603cvd_pbo_dsQueueQos_type_t *pQosType);
extern int32 rtl9603cvd_raw_pbo_dsFcIgnoreThreshold_set(uint32 onThreshold, uint32 offThreshold);
extern int32 rtl9603cvd_raw_pbo_dsFcIgnoreThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold);

/* SW PBO */
extern int32 rtl9603cvd_raw_swPbo_state_set(rtk_enable_t enable);
extern int32 rtl9603cvd_raw_swPbo_state_get(rtk_enable_t *pEnable);
extern int32 rtl9603cvd_raw_swPbo_dscRunoutThreshold_set(rtk_port_t port, uint32 sramThreshold, uint32 dramThreshold);
extern int32 rtl9603cvd_raw_swPbo_dscRunoutThreshold_get(rtk_port_t port, uint32 *pSramThreshold, uint32 *pDramThreshold);
extern int32 rtl9603cvd_raw_swPbo_dscCount_get(rtk_port_t port, uint32 *pCurrSramDsc, uint32 *pMaxSramDsc, uint32 *pCurrDramDsc, uint32 *pMaxDramDsc);
extern int32 rtl9603cvd_raw_swPbo_dscMax_clear(rtk_port_t port);
extern int32 rtl9603cvd_raw_swPbo_ponnicCounter_get(rtl9603cvd_pbo_ponnic_cnt_t *pCnt);
extern int32 rtl9603cvd_raw_swPbo_groupCounter_get(uint32 group, rtl9603cvd_swPbo_group_cnt_t *pCnt);
extern int32 rtl9603cvd_raw_swPbo_groupCounter_clear(uint32 group);
extern int32 rtl9603cvd_raw_swPbo_counterGroupMember_add(uint32 group, uint32 qid);
extern int32 rtl9603cvd_raw_swPbo_counterGroupMember_clear(uint32 group);
extern int32 rtl9603cvd_raw_swPbo_counterGroupMember_get(uint32 group, uint32 qid, rtk_enable_t *pEnabled);
extern int32 rtl9603cvd_raw_swPbo_counter_get(rtl9603cvd_swPbo_cnt_t *pCnt);
extern int32 rtl9603cvd_raw_swPbo_counter_clear(void);

/******************************************************************************/
/* PON PBO                                                                    */
/******************************************************************************/
/* Function Name:
 *      dal_rtl9603cvd_pbo_init
 * Description:
 *      Inititalize PBO
 * Input:
 *      initParam - PBO initialization parameters
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
extern
int32 dal_rtl9603cvd_pbo_init(rtk_pbo_initParam_t initParam);

/******************************************************************************/
/* SW PBO                                                                     */
/******************************************************************************/
/* Function Name:
 *      dal_rtl9603cvd_swPbo_init
 * Description:
 *      Initialize switch PBO
 * Input:
 *      initParam - SWPBO initialization parameters
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
extern int32 dal_rtl9603cvd_swPbo_init(rtk_swPbo_initParam_t initParam);

/* Function Name:
 *      dal_rtl9603cvd_swPbo_portState_set
 * Description:
 *      Set switch PBO UTP port state to enable or disable
 * Input:
 *      port                - port number to perform
 *      enable              - enable / disable port switch PBO
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_PORT_ID
 * Note:
 *      None
 */
extern int32 dal_rtl9603cvd_swPbo_portState_set(
    rtk_port_t port,
    rtk_enable_t enable);

/* Function Name:
 *      dal_rtl9603cvd_swPbo_portState_get
 * Description:
 *      Get switch PBO UTP port state to enable or disable
 * Input:
 *      port                - port number to perform
 * Output:
 *      pEnable             - enable / disable port switch PBO
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_PORT_ID
 * Note:
 *      None
 */
extern int32 dal_rtl9603cvd_swPbo_portState_get(
    rtk_port_t port,
    rtk_enable_t *pEnable);

/* Function Name:
 *      dal_rtl9603c_swPbo_portAutoConf_set
 * Description:
 *      Set switch PBO UTP port auto switching in ISR to enable or disable
 * Input:
 *      port                - port number to perform
 *      enable              - enable / disable port switch PBO
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_INPUT
 *      RT_ERR_PORT_ID
 * Note:
 *      None
 */
extern int32 dal_rtl9603cvd_swPbo_portAutoConf_set(
    rtk_port_t port,
    rtk_enable_t enable);

/* Function Name:
 *      dal_rtl9603c_swPbo_portAutoConf_get
 * Description:
 *      Get switch PBO UTP port auto switching in ISR to enable or disable
 * Input:
 *      port                - port number to perform
 * Output:
 *      pEnable             - enable / disable port switch PBO
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_INPUT
 *      RT_ERR_PORT_ID
 * Note:
 *      None
 */
extern int32 dal_rtl9603cvd_swPbo_portAutoConf_get(
    rtk_port_t port,
    rtk_enable_t *pEnable);

/* Function Name:
 *      dal_rtl9603cvd_swPbo_portThreshold_set
 * Description:
 *      Set switch PBO UTP port page use threshold
 * Input:
 *      port                - port number to perform
 *      threshold           - page use threshold of port
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_PORT_ID
 * Note:
 *      None
 */
extern int32 dal_rtl9603cvd_swPbo_portThreshold_set(
    rtk_port_t port,
    uint32 threshold);

/* Function Name:
 *      dal_rtl9603cvd_swPbo_portThreshold_get
 * Description:
 *      Get switch PBO UTP port page use threshold
 * Input:
 *      port                - port number to perform
 * Output:
 *      pThreshold          - page use threshold of port
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_PORT_ID
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern int32 dal_rtl9603cvd_swPbo_portThreshold_get(
    rtk_port_t port,
    uint32 *pThreshold);

/* Function Name:
 *      dal_rtl9603cvd_swPbo_queueThreshold_set
 * Description:
 *      Set switch PBO UTP queue page use threshold
 * Input:
 *      port                - port number to perform
 *      queue               - queue number to perform
 *      threshold           - page use threshold of queue
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_PORT_ID
 *      RT_ERR_QUEUE_NUM
 * Note:
 *      None
 */
extern int32 dal_rtl9603cvd_swPbo_queueThreshold_set(
    rtk_port_t port,
    uint32 queue,
    uint32 threshold);

/* Function Name:
 *      dal_rtl9603cvd_swPbo_queueThreshold_get
 * Description:
 *      Get switch PBO UTP queue page use threshold
 * Input:
 *      port                - port number to perform
 *      queue               - queue number to perform
 * Output:
 *      pThreshold          - page use threshold of queue
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_PORT_ID
 *      RT_ERR_QUEUE_NUM
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern int32 dal_rtl9603cvd_swPbo_queueThreshold_get(
    rtk_port_t port,
    uint32 queue,
    uint32 *pThreshold);

/* Function Name:
 *      dal_rtl9603cvd_swPbo_portUsedPageCount_get
 * Description:
 *      Get switch PBO UTP port used page current and maximum
 * Input:
 *      port                - port number to perform
 * Output:
 *      pCurrPage           - current used page
 *      pMaxPage            - maximum used page
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_PORT_ID
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern int32 dal_rtl9603cvd_swPbo_portUsedPageCount_get(
    rtk_port_t port,
    uint32 *pCurrPage,
    uint32 *pMaxPage);

/* Function Name:
 *      dal_rtl9603cvd_swPbo_portUsedPageCount_clear
 * Description:
 *      Clear switch PBO UTP port used page maximum
 * Input:
 *      port                - port number to perform
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_PORT_ID
 * Note:
 *      None
 */
extern int32 dal_rtl9603cvd_swPbo_portUsedPageCount_clear(
    rtk_port_t port);

/* Function Name:
 *      dal_rtl9603cvd_swPbo_queueUsedPageCount_get
 * Description:
 *      Get switch PBO UTP port used page current and maximum
 * Input:
 *      port                - port number to perform
 *      queue               - queue number to perform
 * Output:
 *      pCurrPage           - current used page
 *      pMaxPage            - maximum used page
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_PORT_ID
 *      RT_ERR_QUEUE_NUM
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern int32 dal_rtl9603cvd_swPbo_queueUsedPageCount_get(
    rtk_port_t port,
    uint32 queue,
    uint32 *pCurrPage,
    uint32 *pMaxPage);

/* Function Name:
 *      dal_rtl9603cvd_swPbo_queueUsedPageCount_clear
 * Description:
 *      Clear switch PBO UTP port used page current and maximum
 * Input:
 *      port                - port number to perform
 *      queue               - queue number to perform
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_PORT_ID
 *      RT_ERR_QUEUE_NUM
 * Note:
 *      None
 */
extern int32 dal_rtl9603cvd_swPbo_queueUsedPageCount_clear(
    rtk_port_t port,
    uint32 queue);

/* Function Name:
 *      dal_rtl9603cvd_swPbo_portDscCount_get
 * Description:
 *      Get switch PBO UTP port descriptor current and maximum
 * Input:
 *      port                - port number to perform
 * Output:
 *      pCurrDscSram        - current dscriptor SRAM
 *      pCurrDscDram        - current dscriptor DRAM
 *      pMaxDscSram         - maximum dscriptor SRAM
 *      pMaxDscDram         - maximum dscriptor DRAM
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_PORT_ID
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern int32 dal_rtl9603cvd_swPbo_portDscCount_get(
    rtk_port_t port,
    uint32 *pCurrDscSram,
    uint32 *pCurrDscDram,
    uint32 *pMaxDscSram,
    uint32 *pMaxDscDram);

/* Function Name:
 *      dal_rtl9603cvd_swPbo_portDscCount_clear
 * Description:
 *      Clear switch PBO UTP port descriptor maximum
 * Input:
 *      port                - port number to perform
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_PORT_ID
 * Note:
 *      None
 */
extern int32 dal_rtl9603cvd_swPbo_portDscCount_clear(
    rtk_port_t port);

#endif /* _RTL9603CVD_RAW_PBO_H_ */

