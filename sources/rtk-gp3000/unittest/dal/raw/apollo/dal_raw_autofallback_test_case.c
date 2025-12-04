#include <osal/lib.h>
#include <common/debug/rt_log.h>
#include <dal/apollo/raw/apollo_raw_autofallback.h>
#include <dal/raw/apollo/dal_raw_autofallback_test_case.h>

int32 dal_raw_autoFallback_state_test(uint32 caseNo)
{
    rtk_port_t port;
    rtk_enable_t stateW;
    rtk_enable_t stateR;

    /* error input check */
    /* out of range */
    if( apollo_raw_autofallback_enable_set(APOLLO_PORTIDMAX + 1, DISABLED) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_autofallback_enable_set(2, DISABLED) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_autofallback_enable_set(6, DISABLED) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_autofallback_enable_set(0, RTK_ENABLE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* get/set test */
    for(port = 0; port <= APOLLO_PORTIDMAX; port++)
    {
        if( (port != 2) && (port !=6) )
        {
            for(stateW = DISABLED; stateW < RTK_ENABLE_END; stateW++)
            {
                if( apollo_raw_autofallback_enable_set(port, stateW) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }

                if( apollo_raw_autofallback_enable_get(port, &stateR) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }

                if(stateW != stateR)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }
            }
        }
    }

    /* null pointer */
    if( apollo_raw_autofallback_enable_get(port, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32 dal_raw_autoFallback_MaxMonitorCount_test(uint32 caseNo)
{
    raw_autofallback_monitor_t  mon_cntW;
    raw_autofallback_monitor_t  mon_cntR;
    rtk_port_t port;
    uint32 cnt;

    /* error input check */
    /* out of range */
    if( apollo_raw_autofallback_maxMonitorCount_set(RAW_MONITOR_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* get/set test */
    for(mon_cntW = RAW_MONITOR_8K; mon_cntW < RAW_MONITOR_END; mon_cntW++)
    {
        if( apollo_raw_autofallback_maxMonitorCount_set(mon_cntW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if( apollo_raw_autofallback_maxMonitorCount_get(&mon_cntR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if(mon_cntW != mon_cntR)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }

    /* null pointer */
    if( apollo_raw_autofallback_maxMonitorCount_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    for(port = 0; port <= APOLLO_PORTIDMAX; port++)
    {
        if( (port != 2) && (port !=6) )
        {
            if( apollo_raw_autofallback_monitorCount_get(port, NULL) == RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
        }
        else
        {
            if( apollo_raw_autofallback_monitorCount_get(port, &cnt) == RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if( apollo_raw_autofallback_monitorCount_get(port, NULL) == RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_raw_autoFallback_MaxErrorCount_test(uint32 caseNo)
{
    raw_autofallback_error_t  err_cntW;
    raw_autofallback_error_t  err_cntR;
    rtk_port_t port;
    uint32 cnt;

    /* error input check */
    /* out of range */
    if( apollo_raw_autofallback_maxErrorCount_set(RAW_ERROR_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* get/set test */
    for(err_cntW = RAW_ERROR_1_PKT; err_cntW < RAW_ERROR_END; err_cntW++)
    {
        if( apollo_raw_autofallback_maxErrorCount_set(err_cntW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if( apollo_raw_autofallback_maxErrorCount_get(&err_cntR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if(err_cntW != err_cntR)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }

    /* null pointer */
    if( apollo_raw_autofallback_maxErrorCount_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    for(port = 0; port <= APOLLO_PORTIDMAX; port++)
    {
        if( (port != 2) && (port !=6) )
        {
            if( apollo_raw_autofallback_errorCount_get(port, NULL) == RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
        }
        else
        {
            if( apollo_raw_autofallback_errorCount_get(port, &cnt) == RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if( apollo_raw_autofallback_errorCount_get(port, NULL) == RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_raw_autoFallback_reducePL_test(uint32 caseNo)
{
    rtk_enable_t stateW;
    rtk_enable_t stateR;
    rtk_port_t port;
    raw_autofallback_restorePL_t pl_state;

    /* error input check */
    /* out of range */
    if( apollo_raw_autofallback_reducePL_set(RTK_ENABLE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* get/set test */
    for(stateW = DISABLED; stateW < RTK_ENABLE_END; stateW++)
    {
        if( apollo_raw_autofallback_reducePL_set(stateW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if( apollo_raw_autofallback_reducePL_get(&stateR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if(stateW != stateR)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }

    /* null pointer */
    if( apollo_raw_autofallback_reducePL_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    for(port = 0; port <= APOLLO_PORTIDMAX; port++)
    {
        if( (port != 2) && (port !=6) )
        {
            if( apollo_raw_autofallback_restorePL_get(port, NULL) == RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
        }
        else
        {
            if( apollo_raw_autofallback_restorePL_get(port, &pl_state) == RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if( apollo_raw_autofallback_restorePL_get(port, NULL) == RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_raw_autoFallback_timeoutIgnore_test(uint32 caseNo)
{
    rtk_enable_t stateW;
    rtk_enable_t stateR;

    /* error input check */
    /* out of range */
    if( apollo_raw_autofallback_timeoutIgnore_set(RTK_ENABLE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* get/set test */
    for(stateW = DISABLED; stateW < RTK_ENABLE_END; stateW++)
    {
        if( apollo_raw_autofallback_timeoutIgnore_set(stateW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if( apollo_raw_autofallback_timeoutIgnore_get(&stateR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if(stateW != stateR)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }

    /* null pointer */
    if( apollo_raw_autofallback_timeoutIgnore_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }


    return RT_ERR_OK;
}

int32 dal_raw_autoFallback_timeoutTH_test(uint32 caseNo)
{
    uint32 timeW;
    uint32 timeR;

    /* error input check */
    /* out of range */
    if( apollo_raw_autofallback_timeoutTH_set(RAW_AUTOFALLBACK_TIMEOUT_MAX + 1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_autofallback_timeoutTH_set(RAW_AUTOFALLBACK_TIMEOUT_STEP + 1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* get/set test */
    for(timeW = 0; timeW <= RAW_AUTOFALLBACK_TIMEOUT_MAX; timeW = timeW + 4)
    {
        if( apollo_raw_autofallback_timeoutTH_set(timeW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if( apollo_raw_autofallback_timeoutTH_get(&timeR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if(timeW != timeR)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }

    /* null pointer */
    if( apollo_raw_autofallback_timeoutTH_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32 dal_raw_autoFallback_timer_test(uint32 caseNo)
{
    rtk_enable_t stateW;
    rtk_enable_t stateR;

    /* error input check */
    /* out of range */
    if( apollo_raw_autofallback_timer_set(RTK_ENABLE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* get/set test */
    for(stateW = DISABLED; stateW < RTK_ENABLE_END; stateW++)
    {
        if( apollo_raw_autofallback_timer_set(stateW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if( apollo_raw_autofallback_timer_get(&stateR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if(stateW != stateR)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }

    /* null pointer */
    if( apollo_raw_autofallback_timer_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32 dal_raw_autoFallback_validFlow_test(uint32 caseNo)
{
    rtk_port_t port;
    raw_autofallback_validFlow_t valid;

    /* error input check */
    /* out of range */

    /* get/set test */

    /* null pointer */
    for(port = 0; port <= APOLLO_PORTIDMAX; port++)
    {
        if( (port != 2) && (port !=6) )
        {
            if( apollo_raw_autofallback_validFlow_get(port, NULL) == RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
        }
        else
        {
            if( apollo_raw_autofallback_validFlow_get(port, &valid) == RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if( apollo_raw_autofallback_validFlow_get(port, NULL) == RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
        }
    }

    return RT_ERR_OK;
}


