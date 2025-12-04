#ifndef _RTL9602C_RAW_PBO_H_
#define _RTL9602C_RAW_PBO_H_

/****************************************************************/
/* Header File inclusion                                        */
/****************************************************************/
#include <common/rt_type.h>
#include <rtk/port.h>
#include <rtk/pbo.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <dal/rtl9602c/dal_rtl9602c.h>


/****************************************************************/
/* Constant Definition                                          */
/****************************************************************/
typedef enum rtl9602c_pbo_ponMode_e {
    RTL9602C_PBO_PONMODE_GPON = 0,
    RTL9602C_PBO_PONMODE_EPON,
    RTL9602C_PBO_PONMODE_END
} rtl9602c_pbo_ponMode_t;

#define RTL9602C_PBO_PAGE_CNT_US_MAX  0x2000
#define RTL9602C_PBO_PAGE_CNT_DS_MAX  0x2000
#define RTL9602C_PBO_SYS_CNT_128_MAX  7080
#define RTL9602C_PBO_SYS_CNT_256_MAX  3790
#define RTL9602C_PBO_SYS_CNT_512_MAX  1970
#define RTL9602C_PBO_SIDONTH_DEFAULT  150
#define RTL9602C_PBO_SIDOFFTH_DEFAULT 130

/****************************************************************/
/* Type Definition                                              */
/****************************************************************/

typedef struct rtl9602c_pbo_cnt_s {
    uint32 rx_err_cnt;
    uint32 rx_drop_cnt;
    uint32 tx_empty_eob_cnt;
    uint32 rx_in_err;
} rtl9602c_pbo_cnt_t;

typedef struct rtl9602c_pbo_ponnic_cnt_s {
    uint16 pkt_ok_cnt_tx;
    uint16 pkt_ok_cnt_rx;
    uint16 pkt_err_cnt_tx;
    uint16 pkt_err_cnt_rx;
    uint32 pkt_miss_cnt;
} rtl9602c_pbo_ponnic_cnt_t;

typedef struct rtl9602c_pbo_group_cnt_s {
    uint32 rx_good_cnt;
    uint32 rx_bad_cnt;
    uint32 rx_cnt;
    uint32 tx_cnt;
    uint32 tx_frag_cnt;
    uint32 rx_frag_cnt;
} rtl9602c_pbo_group_cnt_t;

typedef struct rtl9602c_memUsage_s {
    uint32 us_base;
    uint32 us_offset;
    uint32 us_page_size;
    uint32 us_page_cnt;

    uint32 ds_base;
    uint32 ds_offset;
    uint32 ds_page_size;
    uint32 ds_page_cnt;
} rtl9602c_memUsage_t;

typedef enum rtl9602c_pbo_fc_status_e {
    RTL9602C_PBO_FLOWCONTROL_STATUS_CURRENT,
    RTL9602C_PBO_FLOWCONTROL_STATUS_LATCHED,
    RTL9602C_PBO_FLOWCONTROL_STATUS_END
} rtl9602c_pbo_fc_status_t;

typedef enum rtl9602c_pbo_dsQueue_type_e {
    RTL9602C_PBO_DSQUEUE_TYPE_NORMAL0,
    RTL9602C_PBO_DSQUEUE_TYPE_NORMAL1,
    RTL9602C_PBO_DSQUEUE_TYPE_HIGH,
    RTL9602C_PBO_DSQUEUE_TYPE_END
} rtl9602c_pbo_dsQueue_type_t;

typedef enum rtl9602c_pbo_dsQueueQos_type_e {
    RTL9602C_PBO_DSQUEUEQOS_TYPE_ROUNDROBIN,
    RTL9602C_PBO_DSQUEUEQOS_TYPE_STRICTPRIORITY,
    RTL9602C_PBO_DSQUEUEQOS_TYPE_END
} rtl9602c_pbo_dsQueueQos_type_t;

extern int32 rtl9602c_raw_pbo_stopAllThreshold_set(uint32 threshold);
extern int32 rtl9602c_raw_pbo_stopAllThreshold_get(uint32 *pThreshold);
extern int32 rtl9602c_raw_pbo_globalThreshold_set(uint32 onThreshold, uint32 offThreshold);
extern int32 rtl9602c_raw_pbo_globalThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold);
extern int32 rtl9602c_raw_pbo_sidThreshold_set(uint32 sid, uint32 onThreshold, uint32 offThreshold);
extern int32 rtl9602c_raw_pbo_sidThreshold_get(uint32 sid,uint32 *pOnThreshold, uint32 *pOffThreshold);
extern int32 rtl9602c_raw_pbo_usState_set(rtk_enable_t enable);
extern int32 rtl9602c_raw_pbo_usState_get(rtk_enable_t *pEnable);
extern int32 rtl9602c_raw_pbo_dsState_set(rtk_enable_t enable);
extern int32 rtl9602c_raw_pbo_dsState_get(rtk_enable_t *pEnable);
extern int32 rtl9602c_raw_pbo_usStatus_get(rtk_enable_t *pEnable);
extern int32 rtl9602c_raw_pbo_dsStatus_get(rtk_enable_t *pEnable);
extern int32 rtl9602c_raw_pbo_usDscRunoutThreshold_set(uint32 dram, uint32 sram);
extern int32 rtl9602c_raw_pbo_usDscRunoutThreshold_get(uint32 *pDram, uint32 *pSram);
extern int32 rtl9602c_raw_pbo_dsDscRunoutThreshold_set(uint32 dram, uint32 sram);
extern int32 rtl9602c_raw_pbo_dsDscRunoutThreshold_get(uint32 *pDram, uint32 *pSram);
extern int32 rtl9602c_raw_pbo_usUsedPageCount_get(uint32 *pSram, uint32 *pDram);
extern int32 rtl9602c_raw_pbo_usMaxUsedPageCount_get(uint32 *pSram, uint32 *pDram);
extern int32 rtl9602c_raw_pbo_usMaxUsedPageCount_clear(void);
extern int32 rtl9602c_raw_pbo_dsUsedPageCount_get(uint32 *pSram, uint32 *pDram);
extern int32 rtl9602c_raw_pbo_dsMaxUsedPageCount_get(uint32 *pSram, uint32 *pDram);
extern int32 rtl9602c_raw_pbo_dsMaxUsedPageCount_clear(void);
extern int32 rtl9602c_raw_pbo_usCounterGroupMember_add(uint32 group, uint32 sid);
extern int32 rtl9602c_raw_pbo_usCounterGroupMember_clear(uint32 group);
extern int32 rtl9602c_raw_pbo_usCounterGroupMember_get(uint32 group, uint32 sid, rtk_enable_t *pEnabled);
extern int32 rtl9602c_raw_pbo_usCounter_get(rtl9602c_pbo_cnt_t *pCnt);
extern int32 rtl9602c_raw_pbo_usCounter_clear(void);
extern int32 rtl9602c_raw_pbo_usPonnicCounter_get(rtl9602c_pbo_ponnic_cnt_t *pCnt);
extern int32 rtl9602c_raw_pbo_usPonnicCounter_clear(void);
extern int32 rtl9602c_raw_pbo_usGroupCounter_clear(uint32 group);
extern int32 rtl9602c_raw_pbo_usGroupCounter_get(uint32 group, rtl9602c_pbo_group_cnt_t *pCnt);
extern int32 rtl9602c_raw_pbo_dsCounterGroupMember_add(uint32 group, uint32 sid);
extern int32 rtl9602c_raw_pbo_dsCounterGroupMember_clear(uint32 group);
extern int32 rtl9602c_raw_pbo_dsCounterGroupMember_get(uint32 group, uint32 sid, rtk_enable_t *pEnabled);
extern int32 rtl9602c_raw_pbo_dsCounter_get(rtl9602c_pbo_cnt_t *pCnt);
extern int32 rtl9602c_raw_pbo_dsCounter_clear(void);
extern int32 rtl9602c_raw_pbo_dsPonnicCounter_get(rtl9602c_pbo_ponnic_cnt_t *pCnt);
extern int32 rtl9602c_raw_pbo_dsPonnicCounter_clear(void);
extern int32 rtl9602c_raw_pbo_dsGroupCounter_clear(uint32 group);
extern int32 rtl9602c_raw_pbo_dsGroupCounter_get(uint32 group, rtl9602c_pbo_group_cnt_t *pCnt);
extern int32 rtl9602c_raw_pbo_flowcontrolStatus_get(uint32 sid, rtl9602c_pbo_fc_status_t latched, rtk_enable_t *pStatus);
extern int32 rtl9602c_raw_pbo_flowcontrolLatchStatus_clear(uint32 sid);
extern int32 rtl9602c_raw_pbo_dsSidToQueueMap_set(uint32 sid, rtl9602c_pbo_dsQueue_type_t qType);
extern int32 rtl9602c_raw_pbo_dsSidToQueueMap_get(uint32 sid, rtl9602c_pbo_dsQueue_type_t *pQType);
extern int32 rtl9602c_raw_pbo_dsQueueQos_set(rtl9602c_pbo_dsQueueQos_type_t qosType);
extern int32 rtl9602c_raw_pbo_dsQueueQos_get(rtl9602c_pbo_dsQueueQos_type_t *pQosType);
extern int32 rtl9602c_raw_pbo_dsFcIgnoreThreshold_set(uint32 onThreshold, uint32 offThreshold);
extern int32 rtl9602c_raw_pbo_dsFcIgnoreThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold);

/* Function Name:
 *      dal_rtl9602c_pbo_init
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
int32 dal_rtl9602c_pbo_init(rtk_pbo_initParam_t initParam);

#endif /*_RTL9602C_PBO_H_*/

