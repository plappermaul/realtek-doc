/*
 * Copyright (C) 2012 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Definition of Mirror API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) Port-based mirror
 */



/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/port.h>
#include <dal/apollomp/dal_apollomp.h>
#include <rtk/mirror.h>
#include <dal/apollomp/dal_apollomp_mirror.h>
#include <dal/apollomp/raw/apollomp_raw_mirror.h>

/*
 * Symbol Definition
 */
static uint32    mirror_init = INIT_NOT_COMPLETED;

/*
 * Macro Declaration
 */


/*
 * Function Declaration
 */

/* Module Name : Mirror */

/* Function Name:
 *      dal_apollomp_mirror_init
 * Description:
 *      Initialize the mirroring database.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Must initialize Mirror module before calling any Mirror APIs.
 */
int32
dal_apollomp_mirror_init(void)
{
    int32   ret;
    rtk_portmask_t tx_portmask;
    rtk_portmask_t rx_portmask;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_MIRROR),"%s",__FUNCTION__);

    mirror_init = INIT_COMPLETED;

    RTK_PORTMASK_RESET(tx_portmask);
    RTK_PORTMASK_RESET(rx_portmask);

    /* port 0 is not exist in 9601, so we change the initial port to cpu port */
    //if( (ret = dal_apollomp_mirror_portBased_set(0, &rx_portmask, &tx_portmask)) != RT_ERR_OK)
    if( (ret = dal_apollomp_mirror_portBased_set(HAL_GET_CPU_PORT(), &rx_portmask, &tx_portmask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    if( (ret = dal_apollomp_mirror_portIso_set(DISABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_mirror_init */

/* Module Name    : Mirror            */
/* Sub-module Name: Port-based mirror */

/* Function Name:
 *      dal_apollomp_mirror_portBased_set
 * Description:
 *      Set port mirror function.
 * Input:
 *      mirroringPort         - Monitor port.
 *      pMirroredRxPortmask   - Rx mirror port mask.
 *      pMirroredTxPortmask   - Tx mirror port mask.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - Invalid port number
 *      RT_ERR_PORT_MASK    - Invalid portmask.
 * Note:
 *      The API is to set mirror function of source port and mirror port.
 *      The mirror port can only be set to one port and the TX and RX mirror ports
 *      should be identical.
 */
int32
dal_apollomp_mirror_portBased_set(rtk_port_t mirroringPort, rtk_portmask_t *pMirroredRxPortmask, rtk_portmask_t *pMirroredTxPortmask)
{
    int32   ret;
    apollomp_raw_mirror_cfg_t mir_cfg;
    rtk_portmask_t mirrored_portmask;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_MIRROR),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(mirror_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(mirroringPort), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pMirroredRxPortmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pMirroredTxPortmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HAL_IS_PORTMASK_VALID((*pMirroredRxPortmask)), RT_ERR_PORT_MASK);
    RT_PARAM_CHK(!HAL_IS_PORTMASK_VALID((*pMirroredTxPortmask)), RT_ERR_PORT_MASK);

    if(RTK_PORTMASK_COMPARE((*pMirroredRxPortmask), (*pMirroredTxPortmask)) != 0)
    {
        if(RTK_PORTMASK_GET_PORT_COUNT((*pMirroredRxPortmask)) == 0)
        {
            mir_cfg.mirror_tx = ENABLED;
            mir_cfg.mirror_rx = DISABLED;
            RTK_PORTMASK_ASSIGN(mirrored_portmask, (*pMirroredTxPortmask));
        }
        else if(RTK_PORTMASK_GET_PORT_COUNT((*pMirroredTxPortmask)) == 0)
        {
            mir_cfg.mirror_tx = DISABLED;
            mir_cfg.mirror_rx = ENABLED;
            RTK_PORTMASK_ASSIGN(mirrored_portmask, (*pMirroredRxPortmask));
        }
        else
            return RT_ERR_PORT_MASK;
    }
    else
    {
        mir_cfg.mirror_tx = ENABLED;
        mir_cfg.mirror_rx = ENABLED;
        RTK_PORTMASK_ASSIGN(mirrored_portmask, (*pMirroredRxPortmask));
    }

    mir_cfg.monitor_port = mirroringPort;
    RTK_PORTMASK_ASSIGN(mir_cfg.source_portmask, mirrored_portmask);
    if( (ret = apollomp_raw_mirrorCfg_set(&mir_cfg)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_mirror_portBased_set */

/* Function Name:
 *      dal_apollomp_mirror_portBased_get
 * Description:
 *      Get port mirror function.
 * Input:
 *      None
 * Output:
 *      pMirroringPort        - Monitor port.
 *      pMirroredRxPortmask   - Rx mirror port mask.
 *      pMirroredTxPortmask   - Tx mirror port mask.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT 		- Invalid input parameters.
 * Note:
 *      The API is to get mirror function of source port and mirror port.
 */
int32
dal_apollomp_mirror_portBased_get(rtk_port_t *pMirroringPort, rtk_portmask_t *pMirroredRxPortmask, rtk_portmask_t *pMirroredTxPortmask)
{
    int32   ret;
    apollomp_raw_mirror_cfg_t mir_cfg;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_MIRROR),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(mirror_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pMirroringPort), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pMirroredRxPortmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pMirroredTxPortmask), RT_ERR_NULL_POINTER);

    if( (ret = apollomp_raw_mirrorCfg_get(&mir_cfg)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    *pMirroringPort = mir_cfg.monitor_port;
    if( (ENABLED == mir_cfg.mirror_tx) && (ENABLED == mir_cfg.mirror_rx) )
    {
        RTK_PORTMASK_ASSIGN( (*pMirroredRxPortmask), mir_cfg.source_portmask);
        RTK_PORTMASK_ASSIGN( (*pMirroredTxPortmask), mir_cfg.source_portmask);
    }
    else if((DISABLED == mir_cfg.mirror_tx) && (ENABLED == mir_cfg.mirror_rx))
    {
        RTK_PORTMASK_ASSIGN( (*pMirroredRxPortmask), mir_cfg.source_portmask);
        RTK_PORTMASK_RESET( (*pMirroredTxPortmask) );
    }
    else if((ENABLED == mir_cfg.mirror_tx) && (DISABLED == mir_cfg.mirror_rx))
    {
        RTK_PORTMASK_RESET( (*pMirroredRxPortmask) );
        RTK_PORTMASK_ASSIGN( (*pMirroredTxPortmask), mir_cfg.source_portmask);
    }
    else
    {
        RTK_PORTMASK_RESET( (*pMirroredRxPortmask) );
        RTK_PORTMASK_RESET( (*pMirroredTxPortmask) );
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_mirror_portBased_get */

/* Function Name:
 *      dal_apollomp_mirror_portIso_set
 * Description:
 *      Set mirror port isolation.
 * Input:
 *      enable Mirror isolation status.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_ENABLE       - Invalid enable input
 * Note:
 *      The API is to set mirror isolation function that prevent normal forwarding packets to miror port.
 */
int32
dal_apollomp_mirror_portIso_set(rtk_enable_t enable)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_MIRROR),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(mirror_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    if( (ret = apollomp_raw_mirrorIso_set(enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_mirror_portIso_set */

/* Function Name:
 *      dal_apollomp_mirror_portIso_get
 * Description:
 *      Get mirror port isolation.
 * Input:
 *      None
 * Output:
 *      pEnable Mirror isolation status.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT 		- Invalid input parameters.
 * Note:
 *      The API is to get mirror isolation status.
 */
int32
dal_apollomp_mirror_portIso_get(rtk_enable_t *pEnable)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_MIRROR),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(mirror_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if( (ret = apollomp_raw_mirrorIso_get(pEnable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_mirror_portIso_get */


