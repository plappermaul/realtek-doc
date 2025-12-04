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
 * $Revision: 39101 $
 * $Date: 2013-05-03 17:35:27 +0800 (週五, 03 五月 2013) $
 *
 * Purpose : Definition of Mirror API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) Port-based mirror
 */



/*
 * Include Files
 */
#include <rtk/port.h>
#include <dal/rtl9607c/dal_rtl9607c.h>
#include <rtk/mirror.h>
#include <dal/rtl9607c/dal_rtl9607c_mirror.h>
#include <dal/rtl9607c/dal_rtl9607c_pbo.h>

/*
 * Symbol Definition
 */
static uint32    mirror_init = INIT_NOT_COMPLETED;
static rtk_enable_t swPboState[RTK_MAX_NUM_OF_PORTS];
static rtk_enable_t swPboAutoConf[RTK_MAX_NUM_OF_PORTS];

/*
 * Macro Declaration
 */


/*
 * Function Declaration
 */

/* Module Name : Mirror */

/* Function Name:
 *      dal_rtl9607c_mirror_init
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
dal_rtl9607c_mirror_init(void)
{
    int32   ret;
    uint32  port;
    rtk_portmask_t tx_portmask;
    rtk_portmask_t rx_portmask;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_MIRROR),"%s",__FUNCTION__);

    mirror_init = INIT_COMPLETED;

    HAL_SCAN_ALL_PORT(port)
    {
        if(HAL_IS_SWPBO_PORT(port))
        {
            swPboState[port] = RTK_ENABLE_END;
        }
    }

    RTK_PORTMASK_RESET(tx_portmask);
    RTK_PORTMASK_RESET(rx_portmask);
    if( (ret = dal_rtl9607c_mirror_portBased_set(HAL_GET_CPU_PORT(), &rx_portmask, &tx_portmask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    if( (ret = dal_rtl9607c_mirror_portIso_set(DISABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607c_mirror_init */

/* Module Name    : Mirror            */
/* Sub-module Name: Port-based mirror */

/* Function Name:
 *      dal_rtl9607c_mirror_portBased_set
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
dal_rtl9607c_mirror_portBased_set(rtk_port_t mirroringPort, rtk_portmask_t *pMirroredRxPortmask, rtk_portmask_t *pMirroredTxPortmask)
{
    int32   ret;
    rtk_portmask_t mirrored_portmask;
    uint32 mirror_tx;
    uint32 mirror_rx;
    uint32 monitor_port, prev_monitor;

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
            mirror_tx = ENABLED;
            mirror_rx = DISABLED;
            RTK_PORTMASK_ASSIGN(mirrored_portmask, (*pMirroredTxPortmask));
        }
        else if(RTK_PORTMASK_GET_PORT_COUNT((*pMirroredTxPortmask)) == 0)
        {
            mirror_tx = DISABLED;
            mirror_rx = ENABLED;
            RTK_PORTMASK_ASSIGN(mirrored_portmask, (*pMirroredRxPortmask));
        }
        else
            return RT_ERR_PORT_MASK;
    }
    else
    {
        mirror_tx = ENABLED;
        mirror_rx = ENABLED;
        RTK_PORTMASK_ASSIGN(mirrored_portmask, (*pMirroredRxPortmask));
    }

    /* SW PBO port can't be used as monitor port
     * Disable it while using as monitor port
     * Restore when it is not
     */
    if ((ret = reg_field_read(RTL9607C_MIR_CTRLr, RTL9607C_MIR_MONITOR_PORTf, &prev_monitor)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }
    if((HAL_IS_SWPBO_PORT(prev_monitor)) && (swPboState[prev_monitor] != RTK_ENABLE_END))
    {
        /* Restore previous monitor port's SW PBO config */
        if ((ret = dal_rtl9607c_swPbo_portState_set(prev_monitor, swPboState[prev_monitor])) != RT_ERR_OK)
        {
            RT_LOG(ret, (MOD_DAL|MOD_TRAP), "");
            /* Don't care return value since SW PBO might not be init for some mode */
        }
        if ((ret = dal_rtl9607c_swPbo_portAutoConf_set(prev_monitor, swPboAutoConf[prev_monitor])) != RT_ERR_OK)
        {
            RT_LOG(ret, (MOD_DAL|MOD_TRAP), "");
            /* Don't care return value since SW PBO might not be init for some mode */
        }
        swPboState[prev_monitor] = RTK_ENABLE_END;
        swPboAutoConf[prev_monitor] = RTK_ENABLE_END;
    }

    if(HAL_IS_SWPBO_PORT(mirroringPort))
    {
        /* Store original settings */
        if ((ret = dal_rtl9607c_swPbo_portState_get(mirroringPort, &swPboState[mirroringPort])) != RT_ERR_OK)
        {
            RT_LOG(ret, (MOD_DAL|MOD_TRAP), "");
            /* Don't care return value since SW PBO might not be init for some mode */
        }
        if ((ret = dal_rtl9607c_swPbo_portAutoConf_get(mirroringPort, &swPboAutoConf[mirroringPort])) != RT_ERR_OK)
        {
            RT_LOG(ret, (MOD_DAL|MOD_TRAP), "");
            /* Don't care return value since SW PBO might not be init for some mode */
        }

        /* Disable SW PBO of monitor port */
        if ((ret = dal_rtl9607c_swPbo_portState_set(mirroringPort, DISABLED)) != RT_ERR_OK)
        {
            RT_LOG(ret, (MOD_DAL|MOD_TRAP), "");
            /* Don't care return value since SW PBO might not be init for some mode */
        }
        if ((ret = dal_rtl9607c_swPbo_portAutoConf_set(mirroringPort, DISABLED)) != RT_ERR_OK)
        {
            RT_LOG(ret, (MOD_DAL|MOD_TRAP), "");
            /* Don't care return value since SW PBO might not be init for some mode */
        }
    }

    monitor_port = mirroringPort;
    if ((ret = reg_field_write(RTL9607C_MIR_CTRLr, RTL9607C_MIR_MONITOR_PORTf, &monitor_port)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    if ((ret = reg_field_write(RTL9607C_MIR_CTRLr, RTL9607C_MIR_RXf, &mirror_rx)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    if ((ret = reg_field_write(RTL9607C_MIR_CTRLr, RTL9607C_MIR_TXf, &mirror_tx)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    if ((ret = reg_field_write(RTL9607C_MIR_CTRLr, RTL9607C_MIR_SRC_PMSKf, &(mirrored_portmask.bits[0]))) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607c_mirror_portBased_set */

/* Function Name:
 *      dal_rtl9607c_mirror_portBased_get
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
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      The API is to get mirror function of source port and mirror port.
 */
int32
dal_rtl9607c_mirror_portBased_get(rtk_port_t *pMirroringPort, rtk_portmask_t *pMirroredRxPortmask, rtk_portmask_t *pMirroredTxPortmask)
{
    int32   ret;
    uint32 mirrored_portmask;
    uint32 mirror_tx;
    uint32 mirror_rx;
    uint32 monitor_port;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_MIRROR),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(mirror_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pMirroringPort), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pMirroredRxPortmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pMirroredTxPortmask), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9607C_MIR_CTRLr, RTL9607C_MIR_MONITOR_PORTf, &monitor_port)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    if ((ret = reg_field_read(RTL9607C_MIR_CTRLr, RTL9607C_MIR_RXf, &mirror_rx)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    if ((ret = reg_field_read(RTL9607C_MIR_CTRLr, RTL9607C_MIR_TXf, &mirror_tx)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    if ((ret = reg_field_read(RTL9607C_MIR_CTRLr, RTL9607C_MIR_SRC_PMSKf, &mirrored_portmask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    *pMirroringPort = monitor_port;
    if( (ENABLED == mirror_tx) && (ENABLED == mirror_rx) )
    {
        pMirroredRxPortmask->bits[0] = mirrored_portmask;
        pMirroredTxPortmask->bits[0] = mirrored_portmask;
    }
    else if((DISABLED == mirror_tx) && (ENABLED == mirror_rx))
    {
        pMirroredRxPortmask->bits[0] = mirrored_portmask;
        pMirroredTxPortmask->bits[0] = 0;
    }
    else if((ENABLED == mirror_tx) && (DISABLED == mirror_rx))
    {
        pMirroredRxPortmask->bits[0] = 0;
        pMirroredTxPortmask->bits[0] = mirrored_portmask;
    }
    else
    {
        pMirroredRxPortmask->bits[0] = 0;
        pMirroredTxPortmask->bits[0] = 0;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607c_mirror_portBased_get */

/* Function Name:
 *      dal_rtl9607c_mirror_portIso_set
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
dal_rtl9607c_mirror_portIso_set(rtk_enable_t enable)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_MIRROR),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(mirror_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    if ((ret = reg_field_write(RTL9607C_MIR_CTRLr, RTL9607C_MIR_ISOf, (uint32 *)&enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607c_mirror_portIso_set */

/* Function Name:
 *      dal_rtl9607c_mirror_portIso_get
 * Description:
 *      Get mirror port isolation.
 * Input:
 *      None
 * Output:
 *      pEnable Mirror isolation status.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      The API is to get mirror isolation status.
 */
int32
dal_rtl9607c_mirror_portIso_get(rtk_enable_t *pEnable)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_MIRROR),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(mirror_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9607C_MIR_CTRLr, RTL9607C_MIR_ISOf, (uint32 *)pEnable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607c_mirror_portIso_get */


