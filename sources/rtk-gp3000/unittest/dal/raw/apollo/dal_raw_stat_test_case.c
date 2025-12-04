#include <osal/lib.h>
#include <common/debug/rt_log.h>
#include <dal/apollo/raw/apollo_raw_stat.h>
#include <dal/raw/apollo/dal_raw_stat_test_case.h>

/* Define symbol used for test input */


int32 dal_raw_stat_test(uint32 caseNo)
{
    rtk_stat_logCnt_mode_t mode;
    rtk_stat_logCnt_type_t type;
    uint64 cntr;
    uint32 code;
    uint32 cnt32;
    rtk_mib_tag_cnt_state_t state;
    uint32 acl_idx;
    rtk_mib_rst_value_t rst;
    rtk_mib_count_mode_t cnt_mode;
    uint32 rlatch_timer, wlatch_timer;
    rtk_mib_sync_mode_t syn_mode;
    rtk_mib_tag_cnt_state_t tag_cnt;

    /*error input check*/
    /*out of range*/
    if (RT_ERR_OK == apollo_raw_stat_port_reset(APOLLO_PORTNO))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if (RT_ERR_OK == apollo_raw_stat_acl_reset(APOLLO_ACL_MIB_MAX + 1))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if (RT_ERR_OK == apollo_raw_stat_acl_mode_set(APOLLO_ACL_MIB_MAX, 0))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if (RT_ERR_OK == apollo_raw_stat_acl_mode_set(1, 0))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if (RT_ERR_OK == apollo_raw_stat_acl_mode_set(0, STAT_LOG_MODE_END))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if (RT_ERR_OK == apollo_raw_stat_acl_mode_get(APOLLO_ACL_MIB_MAX, &mode))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if (RT_ERR_OK == apollo_raw_stat_acl_mode_get(1, &mode))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if (RT_ERR_OK == apollo_raw_stat_acl_type_set(APOLLO_ACL_MIB_MAX, 0))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if (RT_ERR_OK == apollo_raw_stat_acl_type_set(0, STAT_LOG_TYPE_END))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if (RT_ERR_OK == apollo_raw_stat_acl_type_set(1, 0))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if (RT_ERR_OK == apollo_raw_stat_acl_type_get(APOLLO_ACL_MIB_MAX, &type))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if (RT_ERR_OK == apollo_raw_stat_acl_type_get(1, &type))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if (RT_ERR_OK == apollo_raw_stat_port_get(APOLLO_PORTNO, 0, &cntr))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if (RT_ERR_OK == apollo_raw_stat_global_get(MIB_GLOBAL_CNTR_END, &cntr))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if (RT_ERR_OK == apollo_raw_stat_port_get(0, MIB_PORT_CNTR_END, &cntr))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if (RT_ERR_OK == apollo_raw_stat_acl_get(APOLLO_ACL_MIB_MAX + 1, &cntr))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if (RT_ERR_OK == apollo_raw_stat_pktInfo_get(APOLLO_PORTNO, &code))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if (RT_ERR_OK == apollo_raw_stat_rst_mib_value_set(APOLLO_MIB_RST_END))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if (RT_ERR_OK == apollo_raw_stat_acl_counter_get(APOLLO_ACL_MIB_MAX + 1, &cnt32))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if (RT_ERR_OK == apollo_raw_stat_mib_cnt_mode_set(APOLLO_MIB_COUNT_MODE_END))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if (RT_ERR_OK == apollo_raw_stat_mib_latch_timer_set(APOLLO_MIB_LATCH_TIMER_MAX + 1))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if (RT_ERR_OK == apollo_raw_stat_mib_sync_mode_set(APOLLO_MIB_SYNC_MODE_END))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if (RT_ERR_OK == apollo_raw_stat_mib_count_tag_length_get(APOLLO_MIB_TAG_CNT_DIR_END, &state))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if (RT_ERR_OK == apollo_raw_stat_mib_count_tag_length_set(APOLLO_MIB_TAG_CNT_DIR_END, 0))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if (RT_ERR_OK == apollo_raw_stat_mib_count_tag_length_set(0, APOLLO_MIB_TAG_CNT_STATE_END))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }


    /*NULL pointer*/
    if (RT_ERR_OK == apollo_raw_stat_acl_mode_get(0, NULL))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if (RT_ERR_OK == apollo_raw_stat_acl_type_get(0, NULL))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if (RT_ERR_OK == apollo_raw_stat_port_get(0, 0, NULL))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if (RT_ERR_OK == apollo_raw_stat_global_get(0, NULL))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if (RT_ERR_OK == apollo_raw_stat_acl_get(0, NULL))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if (RT_ERR_OK == apollo_raw_stat_pktInfo_get(0, NULL))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if (RT_ERR_OK == apollo_raw_stat_rst_mib_value_get(NULL))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if (RT_ERR_OK == apollo_raw_stat_acl_counter_get(0, NULL))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if (RT_ERR_OK == apollo_raw_stat_mib_cnt_mode_get(NULL))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if (RT_ERR_OK == apollo_raw_stat_mib_latch_timer_get(NULL))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if (RT_ERR_OK == apollo_raw_stat_mib_sync_mode_get(NULL))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if (RT_ERR_OK == apollo_raw_stat_mib_count_tag_length_get(0, NULL))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }


    /* get/set test*/
    for (acl_idx = 0; acl_idx < APOLLO_ACL_MIB_MAX; acl_idx++)
    {
        if (acl_idx % 2 == 0)
        {
            if ( apollo_raw_stat_acl_mode_set(acl_idx, STAT_LOG_MODE_64BITS) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            if ( apollo_raw_stat_acl_mode_get(acl_idx, &mode) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            if (STAT_LOG_MODE_64BITS != mode)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
        }
    }

    for (acl_idx = 0; acl_idx < APOLLO_ACL_MIB_MAX; acl_idx++)
    {
        if (acl_idx % 2 == 0)
        {
            if ( apollo_raw_stat_acl_type_set(acl_idx, STAT_LOG_TYPE_BYTECNT) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            if ( apollo_raw_stat_acl_type_get(acl_idx, &type) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            if (STAT_LOG_TYPE_BYTECNT != mode)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
        }
    }

    if ( apollo_raw_stat_rst_mib_value_set(APOLLO_MIB_RST_TO_1) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if ( apollo_raw_stat_rst_mib_value_get(&rst) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if (APOLLO_MIB_RST_TO_1 != rst)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if ( apollo_raw_stat_mib_cnt_mode_set(APOLLO_MIB_COUNT_MODE_TIMER) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if ( apollo_raw_stat_mib_cnt_mode_get(&cnt_mode) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if (APOLLO_MIB_COUNT_MODE_TIMER != cnt_mode)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    wlatch_timer = 10;
    if ( apollo_raw_stat_mib_latch_timer_set(wlatch_timer) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if ( apollo_raw_stat_mib_latch_timer_get(&rlatch_timer) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if (0 != rlatch_timer)//self reset, so always 0
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if ( apollo_raw_stat_mib_sync_mode_set(APOLLO_MIB_SYNC_MODE_STOP_SYNC) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if ( apollo_raw_stat_mib_sync_mode_get(&syn_mode) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if (APOLLO_MIB_SYNC_MODE_STOP_SYNC != syn_mode)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if ( apollo_raw_stat_mib_count_tag_length_set(APOLLO_MIB_TAG_CNT_DIR_TX, APOLLO_MIB_TAG_CNT_STATE_INCLUDE) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if ( apollo_raw_stat_mib_count_tag_length_get(APOLLO_MIB_TAG_CNT_DIR_TX, &tag_cnt) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if (APOLLO_MIB_TAG_CNT_STATE_INCLUDE != tag_cnt)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if ( apollo_raw_stat_mib_count_tag_length_set(APOLLO_MIB_TAG_CNT_DIR_RX, APOLLO_MIB_TAG_CNT_STATE_INCLUDE) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if ( apollo_raw_stat_mib_count_tag_length_get(APOLLO_MIB_TAG_CNT_DIR_RX, &tag_cnt) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if (APOLLO_MIB_TAG_CNT_STATE_INCLUDE != tag_cnt)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }


    /*register access test*/


    return RT_ERR_OK;
}

