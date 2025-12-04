#include <osal/lib.h>
#include <common/debug/rt_log.h>
#include <dal/apollo/raw/apollo_raw_mirror.h>
#include <dal/raw/apollo/dal_raw_mirror_test_case.h>

int32 dal_raw_mirror_cfg_test(uint32 caseNo)
{
    raw_mirror_cfg_t cfgW;
    raw_mirror_cfg_t cfgR;

    /*error input check*/
    /*out of range*/
    memset(&cfgW, 0x00, sizeof(raw_mirror_cfg_t));
    cfgW.mirror_rx = RTK_ENABLE_END;
    cfgW.mirror_tx = ENABLED;
    cfgW.monitor_port = 4;
    cfgW.source_portmask.bits[0] = 0x0F;
    if( apollo_raw_mirrorCfg_set(&cfgW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    memset(&cfgW, 0x00, sizeof(raw_mirror_cfg_t));
    cfgW.mirror_rx = ENABLED;
    cfgW.mirror_tx = RTK_ENABLE_END;
    cfgW.monitor_port = 4;
    cfgW.source_portmask.bits[0] = 0x0F;
    if( apollo_raw_mirrorCfg_set(&cfgW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    memset(&cfgW, 0x00, sizeof(raw_mirror_cfg_t));
    cfgW.mirror_rx = ENABLED;
    cfgW.mirror_tx = ENABLED;
    cfgW.monitor_port = APOLLO_PORTIDMAX + 1;
    cfgW.source_portmask.bits[0] = 0x0F;
    if( apollo_raw_mirrorCfg_set(&cfgW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    memset(&cfgW, 0x00, sizeof(raw_mirror_cfg_t));
    cfgW.mirror_rx = ENABLED;
    cfgW.mirror_tx = ENABLED;
    cfgW.monitor_port = 4;
    cfgW.source_portmask.bits[0] = APOLLO_PORTMASK + 1;
    if( apollo_raw_mirrorCfg_set(&cfgW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    memset(&cfgW, 0x00, sizeof(raw_mirror_cfg_t));
    cfgW.mirror_rx = ENABLED;
    cfgW.mirror_tx = ENABLED;
    cfgW.monitor_port = 4;
    cfgW.source_portmask.bits[0] = (0x1 << 4);
    if( apollo_raw_mirrorCfg_set(&cfgW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* get/set test*/
    for(cfgW.mirror_tx = DISABLED; cfgW.mirror_tx < RTK_ENABLE_END; cfgW.mirror_tx++)
    {
        for(cfgW.mirror_rx = DISABLED; cfgW.mirror_rx < RTK_ENABLE_END; cfgW.mirror_rx++)
        {
            for(cfgW.monitor_port = 0; cfgW.monitor_port <= APOLLO_PORTIDMAX; cfgW.monitor_port++)
            {
                for(cfgW.source_portmask.bits[0] = 0; cfgW.source_portmask.bits[0] <= APOLLO_PORTMASK; cfgW.source_portmask.bits[0]++)
                {
                    if( (cfgW.source_portmask.bits[0] & (0x1 << cfgW.monitor_port)) == 0)
                    {
                        if( apollo_raw_mirrorCfg_set(&cfgW) != RT_ERR_OK)
                        {
                            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                            return RT_ERR_FAILED;
                        }

                        if( apollo_raw_mirrorCfg_get(&cfgR) != RT_ERR_OK)
                        {
                            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                            return RT_ERR_FAILED;
                        }

                        if(memcmp(&cfgW, &cfgR, sizeof(raw_mirror_cfg_t)) != 0)
                        {
                            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                            return RT_ERR_FAILED;
                        }
                    }
                }
            }
        }
    }

    /*null pointer*/
    if( apollo_raw_mirrorCfg_set(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_mirrorCfg_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32 dal_raw_mirror_isolation_test(uint32 caseNo)
{
    rtk_enable_t stateW;
    rtk_enable_t stateR;

    /*error input check*/
    /*out of range*/
    if( apollo_raw_mirrorIso_set(RTK_ENABLE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* get/set test*/
    for(stateW = DISABLED; stateW < RTK_ENABLE_END; stateW++)
    {
        if( apollo_raw_mirrorIso_set(stateW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if( apollo_raw_mirrorIso_get(&stateR) != RT_ERR_OK)
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

    /*null pointer*/
    if( apollo_raw_mirrorIso_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}


