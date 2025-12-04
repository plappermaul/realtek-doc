#include <osal/lib.h>
#include <osal/print.h>
#include <ioal/mem32.h>
#include <hal/common/halctrl.h>
#include <common/error.h>
#include <common/unittest_util.h>
#include <rtk/stat.h>
#include <dal/dal_stat_test_case.h>

/* Define symbol used for test input */


int32 dal_stat_test(uint32 caseNo)
{
    uint64 cnt;
    rtk_mib_rst_value_t rst_val, r_rst_val;
    rtk_stat_log_ctrl_t log_ctrl, r_log_ctrl;
    rtk_mib_count_mode_t mode, r_mode;
    uint32 timer, r_timer;
    rtk_mib_sync_mode_t sync, r_sync;
    rtk_mib_tag_cnt_dir_t /*dir, */r_dir;
    rtk_mib_tag_cnt_state_t state, r_state;
    rtk_port_t port;
    uint32 code;
    uint32 idx;
    int32 ret;

    /*error input check*/
    /*out of range*/
    if(RT_ERR_OK == rtk_stat_global_get(MIB_GLOBAL_CNTR_END, &cnt))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if(RT_ERR_OK == rtk_stat_port_get(HAL_GET_MAX_PORT()+2, IF_IN_OCTETS_INDEX, &cnt))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if(RT_ERR_OK == rtk_stat_rstCntValue_set(STAT_MIB_RST_END))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if(RT_ERR_OK == rtk_stat_log_get(HAL_MAX_NUM_OF_LOG_MIB(), &cnt))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    log_ctrl.mode = STAT_LOG_MODE_32BITS;
    log_ctrl.type = STAT_LOG_TYPE_BYTECNT;
    if(RT_ERR_OK == rtk_stat_logCtrl_set(HAL_MAX_NUM_OF_LOG_MIB(), log_ctrl))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    log_ctrl.mode = STAT_LOG_MODE_END;
    log_ctrl.type = STAT_LOG_TYPE_BYTECNT;
    if(RT_ERR_OK == rtk_stat_logCtrl_set(0, log_ctrl))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    log_ctrl.mode = STAT_LOG_MODE_32BITS;
    log_ctrl.type = STAT_LOG_TYPE_END;
    if(RT_ERR_OK == rtk_stat_logCtrl_set(0, log_ctrl))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if(RT_ERR_OK == rtk_stat_mibCntMode_set(STAT_MIB_COUNT_MODE_END))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if(RT_ERR_OK == rtk_stat_mibLatchTimer_set(HAL_GET_MIB_LATCH_TIMER_MAX()+1))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if(RT_ERR_OK == rtk_stat_mibSyncMode_set(STAT_MIB_SYNC_MODE_END))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if(RT_ERR_OK == rtk_stat_mibCntTagLen_set(STAT_MIB_TAG_CNT_DIR_END, STAT_MIB_TAG_CNT_STATE_EXCLUDE))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if(RT_ERR_OK == rtk_stat_mibCntTagLen_set(STAT_MIB_TAG_CNT_DIR_TX, STAT_MIB_TAG_CNT_STATE_END))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if(RT_ERR_OK == rtk_stat_pktInfo_get(HAL_GET_MAX_PORT()+2,&code))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*NULL pointer*/
    if(RT_ERR_OK == rtk_stat_rstCntValue_get(NULL))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if(RT_ERR_OK == rtk_stat_global_get(DOT1D_TP_LEARNED_ENTRY_DISCARDS_INDEX, NULL))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if(RT_ERR_OK == rtk_stat_global_getAll(NULL))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if(RT_ERR_OK == rtk_stat_port_get(0, IF_IN_OCTETS_INDEX, NULL))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if(RT_ERR_OK == rtk_stat_port_getAll(0, NULL))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if(RT_ERR_OK == rtk_stat_log_get(0, NULL))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if(RT_ERR_OK == rtk_stat_logCtrl_get(0, NULL))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if(RT_ERR_OK == rtk_stat_mibCntMode_get(NULL))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if(RT_ERR_OK == rtk_stat_mibLatchTimer_get(NULL))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if(RT_ERR_OK == rtk_stat_mibSyncMode_get(NULL))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if(RT_ERR_OK == rtk_stat_mibCntTagLen_get(STAT_MIB_TAG_CNT_DIR_TX, NULL))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if(RT_ERR_OK == rtk_stat_pktInfo_get(0, NULL))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* get/set test*/
    /* mib reset value */
    for(rst_val = 0; rst_val < STAT_MIB_RST_END; rst_val++)
    {
        if(RT_ERR_OK != rtk_stat_rstCntValue_set(rst_val))
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if(RT_ERR_OK != rtk_stat_rstCntValue_get(&r_rst_val))
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if(rst_val != r_rst_val)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }

    /* log control */
    for(idx = 0; idx < HAL_MAX_NUM_OF_LOG_MIB(); idx+=2)
    {
        for(log_ctrl.mode = 0; log_ctrl.mode < 2; log_ctrl.mode++)
        {
            log_ctrl.type = (rtk_stat_logCnt_type_t)log_ctrl.mode;
            if(RT_ERR_OK != rtk_stat_logCtrl_set(idx, log_ctrl))
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            if(RT_ERR_OK != rtk_stat_logCtrl_get(idx, &r_log_ctrl))
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            if((log_ctrl.mode != r_log_ctrl.mode) || (log_ctrl.type != r_log_ctrl.type))
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
        }
    }

    /* mib cnt mode */
    for(mode = 0; mode < STAT_MIB_COUNT_MODE_END; mode++)
    {
        if(RT_ERR_OK != rtk_stat_mibCntMode_set(mode))
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if(RT_ERR_OK != rtk_stat_mibCntMode_get(&r_mode))
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if(mode != r_mode)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }

    /* mib latch timer */
    for(timer = 0; timer < HAL_GET_MIB_LATCH_TIMER_MAX(); timer+=64)
    {
        if(RT_ERR_OK != rtk_stat_mibLatchTimer_set(timer))
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if(RT_ERR_OK != rtk_stat_mibLatchTimer_get(&r_timer))
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if(timer != r_timer)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }

    /* mib sync mode */
    for(sync = 0; sync < STAT_MIB_SYNC_MODE_END; sync++)
    {
        if(RT_ERR_OK != rtk_stat_mibSyncMode_set(sync))
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if(RT_ERR_OK != rtk_stat_mibSyncMode_get(&r_sync))
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if(sync != r_sync)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }

    /* pkt info */
    HAL_SCAN_ALL_PORT(port)
    { 
        if(port == HAL_GET_SWPBOLB_PORT())
			continue;

        if(RT_ERR_OK != rtk_stat_pktInfo_get(port, &code))
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }

    return RT_ERR_OK;
}

