#ifndef _RTL9601B_RAW_PBO_H_
#define _RTL9601B_RAW_PBO_H_

/****************************************************************/
/* Header File inclusion                                        */
/****************************************************************/
#include <common/rt_type.h>
#include <rtk/port.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <dal/rtl9601b/dal_rtl9601b.h>


/****************************************************************/
/* Constant Definition                                          */
/****************************************************************/
typedef enum rtl9601b_pbo_ponMode_e {
    RTL9601B_PBO_PONMODE_GPON = 0,
    RTL9601B_PBO_PONMODE_EPON,
    RTL9601B_PBO_PONMODE_END
} rtl9601b_pbo_ponMode_t;


/****************************************************************/
/* Type Definition                                              */
/****************************************************************/

typedef struct rtl9601b_pbo_cnt_s {
    uint32 rx_err_cnt;
    uint32 tx_empty_eob_cnt;
    uint32 rx_in_err;
} rtl9601b_pbo_cnt_t;

typedef struct rtl9601b_pbo_group_cnt_s {
    uint32 rx_good_cnt;
    uint32 rx_bad_cnt;
    uint32 rx_in_cnt;
    uint32 tx_cnt;
    uint32 tx_frag_cnt;
} rtl9601b_pbo_group_cnt_t;

typedef enum rtl9601b_pbo_fc_status_e {
    RTL9601B_PBO_FLOWCONTROL_STATUS_CURRENT,
    RTL9601B_PBO_FLOWCONTROL_STATUS_LATCHED,
    RTL9601B_PBO_FLOWCONTROL_STATUS_END
} rtl9601b_pbo_fc_status_t;

extern int32 rtl9601b_raw_pbo_stopAllThreshold_set(uint32 threshold);
extern int32 rtl9601b_raw_pbo_stopAllThreshold_get(uint32 *pThreshold);
extern int32 rtl9601b_raw_pbo_stopAllState_set(rtk_enable_t enable);
extern int32 rtl9601b_raw_pbo_stopAllState_get(rtk_enable_t *pEnable);
extern int32 rtl9601b_raw_pbo_ponMode_set(rtl9601b_pbo_ponMode_t mode);
extern int32 rtl9601b_raw_pbo_ponMode_get(rtl9601b_pbo_ponMode_t *pMode);
extern int32 rtl9601b_raw_pbo_dscRunoutThreshold_set(uint32 dram, uint32 sram);
extern int32 rtl9601b_raw_pbo_dscRunoutThreshold_get(uint32 *pDram, uint32 *pSram);
extern int32 rtl9601b_raw_pbo_globalThreshold_set(uint32 onThreshold, uint32 offThreshold);
extern int32 rtl9601b_raw_pbo_globalThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold);
extern int32 rtl9601b_raw_pbo_sidThreshold_set(uint32 sid, uint32 onThreshold, uint32 offThreshold);
extern int32 rtl9601b_raw_pbo_sidThreshold_get(uint32 sid,uint32 *pOnThreshold, uint32 *pOffThreshold);
extern int32 rtl9601b_raw_pbo_state_set(rtk_enable_t enable);
extern int32 rtl9601b_raw_pbo_state_get(rtk_enable_t *pEnable);
extern int32 rtl9601b_raw_pbo_status_get(rtk_enable_t *pEnable);
extern int32 rtl9601b_raw_pbo_usedPageCount_get(uint32 *pSram, uint32 *pDram);
extern int32 rtl9601b_raw_pbo_maxUsedPageCount_get(uint32 *pSram, uint32 *pDram);
extern int32 rtl9601b_raw_pbo_maxUsedPageCount_clear(void);
extern int32 rtl9601b_raw_pbo_counterGroupMember_add(uint32 group, uint32 sid);
extern int32 rtl9601b_raw_pbo_counterGroupMember_clear(uint32 group);
extern int32 rtl9601b_raw_pbo_counterGroupMember_get(uint32 group, uint32 sid, rtk_enable_t *pEnabled);
extern int32 rtl9601b_raw_pbo_counter_get(rtl9601b_pbo_cnt_t *pCnt);
extern int32 rtl9601b_raw_pbo_groupCounter_clear(uint32 group);
extern int32 rtl9601b_raw_pbo_groupCounter_get(uint32 group, rtl9601b_pbo_group_cnt_t *pCnt);
extern int32 rtl9601b_raw_pbo_flowcontrolStatus_get(uint32 sid, rtl9601b_pbo_fc_status_t latched, rtk_enable_t *pStatus);
extern int32 rtl9601b_raw_pbo_flowcontrolLatchStatus_clear(uint32 sid);
extern int32 rtl9601b_raw_pbo_init(void);

#endif /*_RTL9601B_RAW_FLOWCTRL_H_*/

