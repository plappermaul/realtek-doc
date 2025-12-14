#include <osal/lib.h>
#include <common/debug/rt_log.h>
#include <dal/apollo/raw/apollo_raw_storm.h>
#include <dal/raw/apollo/dal_raw_storm_test_case.h>

int32 dal_raw_storm_state_test(uint32 caseNo)
{
    raw_storm_type_t type;
    rtk_port_t port;
    rtk_enable_t stateW;
    rtk_enable_t stateR;

    /* error input check */
    /* out of range */
    if( apollo_raw_stormControlState_set(RAW_STORM_END, 0, ENABLED) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_stormControlState_set(RAW_STORM_UNKN_MC, APOLLO_PORTIDMAX + 1, ENABLED) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_stormControlState_set(RAW_STORM_UNKN_MC, 0, RTK_ENABLE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* get/set test */
    for(type = RAW_STORM_UNKN_MC; type < RAW_STORM_END; type++)
    {
        for(port = 0; port <= APOLLO_PORTIDMAX; port++)
        {
            for(stateW = DISABLED; stateW < RTK_ENABLE_END; stateW++)
            {
                if( apollo_raw_stormControlState_set(type, port, stateW) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }

                if( apollo_raw_stormControlState_get(type, port, &stateR) != RT_ERR_OK)
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
    if( apollo_raw_stormControlState_get(type, port, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32 dal_raw_storm_meter_test(uint32 caseNo)
{
    raw_storm_type_t type;
    rtk_port_t port;
    uint32 meter_idxW;
    uint32 meter_idxR;

    /* error input check */
    /* out of range */
    if( apollo_raw_stormControlMeter_set(RAW_STORM_END, 0, 0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_stormControlMeter_set(RAW_STORM_UNKN_MC, APOLLO_PORTIDMAX + 1, 0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_stormControlMeter_set(RAW_STORM_UNKN_MC, 0, HAL_MAX_NUM_OF_METERING()) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* get/set test */
    for(type = RAW_STORM_UNKN_MC; type < RAW_STORM_END; type++)
    {
        for(port = 0; port <= APOLLO_PORTIDMAX; port++)
        {
            for(meter_idxW = 0; meter_idxW < HAL_MAX_NUM_OF_METERING(); meter_idxW++)
            {
                if( apollo_raw_stormControlMeter_set(type, port, meter_idxW) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }

                if( apollo_raw_stormControlMeter_get(type, port, &meter_idxR) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }

                if(meter_idxW != meter_idxR)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }
            }
        }
    }

    /* null pointer */
    if( apollo_raw_stormControlMeter_get(type, port, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32 dal_raw_storm_alt_test(uint32 caseNo)
{
    raw_storm_type_t type;
    raw_storm_alt_type_t alt_typeW;
    raw_storm_alt_type_t alt_typeR;

    /* error input check */
    /* out of range */
    if( apollo_raw_stormControlAlt_set(RAW_STORM_END, RAW_STORM_ALT_DEFAULT) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_stormControlAlt_set(RAW_STORM_UNKN_MC, RAW_STORM_ALT_TYPE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* get/set test */
    for(type = RAW_STORM_UNKN_MC; type < RAW_STORM_END; type++)
    {
        for(alt_typeW = RAW_STORM_ALT_DEFAULT; alt_typeW < RAW_STORM_ALT_TYPE_END; alt_typeW++)
        {
            if( apollo_raw_stormControlAlt_set(type, alt_typeW) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if( apollo_raw_stormControlAlt_get(type, &alt_typeR) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if(alt_typeW != alt_typeR)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
        }
    }

    /* null pointer */
    if( apollo_raw_stormControlAlt_get(type, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

