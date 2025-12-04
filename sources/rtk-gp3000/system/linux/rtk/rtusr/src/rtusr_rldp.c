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
 * Purpose : Declaration of RLDP and RLPP API
 *
 * Feature : The file have include the following module and sub-modules
 *           1) RLDP and RLPP configuration and status
 *
 */

/*
 * Include Files
 */
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <osal/lib.h>
#include <rtk/rtusr/include/rtusr_util.h>
#include <rtdrv/rtdrv_netfilter.h>
#include <common/rt_type.h>


/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */


/*
 * Macro Declaration
 */


/*
 * Function Declaration
 */

/* Module Name : RLDP */

/* Function Name:
 *      rtk_rldp_init
 * Description:
 *      Initialize rldp module.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize rldp module before calling any rldp APIs.
 */
int32
rtk_rldp_init(void)
{
    rtdrv_rldpCfg_t rldp_cfg;

    /* function body */

    osal_memset(&rldp_cfg,0x0,sizeof(rtdrv_rldpCfg_t));

    SETSOCKOPT(RTDRV_RLDP_INIT, &rldp_cfg, rtdrv_rldpCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_rldp_init */


/* Function Name:
 *      rtk_rldp_config_set
 * Description:
 *      Set RLDP module configuration
 * Input:
 *      pConfig - configuration structure of RLDP
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32
rtk_rldp_config_set(rtk_rldp_config_t *pConfig)
{
    rtdrv_rldpCfg_t rldp_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pConfig), RT_ERR_NULL_POINTER);

    /* function body */

    osal_memset(&rldp_cfg,0x0,sizeof(rtdrv_rldpCfg_t));
    osal_memcpy(&rldp_cfg.config, pConfig, sizeof(rtk_rldp_config_t));
    SETSOCKOPT(RTDRV_RLDP_CONFIG_SET, &rldp_cfg, rtdrv_rldpCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_rldp_config_set */


/* Function Name:
 *      rtk_rldp_config_get
 * Description:
 *      Get RLDP module configuration
 * Input:
 *      None
 * Output:
 *      pConfig - configuration structure of RLDP
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32
rtk_rldp_config_get(rtk_rldp_config_t *pConfig)
{
    rtdrv_rldpCfg_t rldp_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pConfig), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rldp_cfg.config, pConfig, sizeof(rtk_rldp_config_t));
    GETSOCKOPT(RTDRV_RLDP_CONFIG_GET, &rldp_cfg, rtdrv_rldpCfg_t, 1);
    osal_memcpy(pConfig, &rldp_cfg.config, sizeof(rtk_rldp_config_t));

    return RT_ERR_OK;
}   /* end of rtk_rldp_config_get */


/* Function Name:
 *      rtk_rldp_portConfig_set
 * Description:
 *      Set per port RLDP module configuration
 * Input:
 *      port   - port number to be configured
 *      pPortConfig - per port configuration structure of RLDP
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32
rtk_rldp_portConfig_set(rtk_port_t port, rtk_rldp_portConfig_t *pPortConfig)
{
    rtdrv_rldpCfg_t rldp_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortConfig), RT_ERR_NULL_POINTER);

    /* function body */

    osal_memset(&rldp_cfg,0x0,sizeof(rtdrv_rldpCfg_t));
    osal_memcpy(&rldp_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&rldp_cfg.portConfig, pPortConfig, sizeof(rtk_rldp_portConfig_t));
    SETSOCKOPT(RTDRV_RLDP_PORTCONFIG_SET, &rldp_cfg, rtdrv_rldpCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_rldp_portConfig_set */


/* Function Name:
 *      rtk_rldp_portConfig_get
 * Description:
 *      Get per port RLDP module configuration
 * Input:
 *      port    - port number to be get
 * Output:
 *      pPortConfig - per port configuration structure of RLDP
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32
rtk_rldp_portConfig_get(rtk_port_t port, rtk_rldp_portConfig_t *pPortConfig)
{
    rtdrv_rldpCfg_t rldp_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortConfig), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rldp_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_RLDP_PORTCONFIG_GET, &rldp_cfg, rtdrv_rldpCfg_t, 1);
    osal_memcpy(pPortConfig, &rldp_cfg.portConfig, sizeof(rtk_rldp_portConfig_t));

    return RT_ERR_OK;
}   /* end of rtk_rldp_portConfig_get */


/* Function Name:
 *      rtk_rldp_status_get
 * Description:
 *      Get RLDP module status
 * Input:
 *      None
 * Output:
 *      pStatus - status structure of RLDP
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32
rtk_rldp_status_get(rtk_rldp_status_t *pStatus)
{
    rtdrv_rldpCfg_t rldp_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pStatus), RT_ERR_NULL_POINTER);

    /* function body */
    GETSOCKOPT(RTDRV_RLDP_STATUS_GET, &rldp_cfg, rtdrv_rldpCfg_t, 1);
    osal_memcpy(pStatus, &rldp_cfg.status, sizeof(rtk_rldp_status_t));

    return RT_ERR_OK;
}   /* end of rtk_rldp_status_get */


/* Function Name:
 *      rtk_rldp_portStatus_get
 * Description:
 *      Get RLDP module status
 * Input:
 *      port    - port number to be get
 * Output:
 *      pPortStatus - per port status structure of RLDP
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32
rtk_rldp_portStatus_get(rtk_port_t port, rtk_rldp_portStatus_t *pPortStatus)
{
    rtdrv_rldpCfg_t rldp_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortStatus), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rldp_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_RLDP_PORTSTATUS_GET, &rldp_cfg, rtdrv_rldpCfg_t, 1);
    osal_memcpy(pPortStatus, &rldp_cfg.portStatus, sizeof(rtk_rldp_portStatus_t));

    return RT_ERR_OK;
}   /* end of rtk_rldp_portStatus_get */


/* Function Name:
 *      rtk_rldp_portStatus_clear
 * Description:
 *      Clear RLDP module status
 * Input:
 *      port    - port number to be clear
 *      pPortStatus - per port status structure of RLDP
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      Clear operation effect loop_enter and loop_leave only, other field in
 *      the structure are don't care
 */
int32
rtk_rldp_portStatus_clear(rtk_port_t port, rtk_rldp_portStatus_t *pPortStatus)
{
    rtdrv_rldpCfg_t rldp_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortStatus), RT_ERR_NULL_POINTER);

    /* function body */

    osal_memset(&rldp_cfg,0x0,sizeof(rtdrv_rldpCfg_t));
    osal_memcpy(&rldp_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&rldp_cfg.portStatus, pPortStatus, sizeof(rtk_rldp_portStatus_t));
    SETSOCKOPT(RTDRV_RLDP_PORTSTATUS_CLEAR, &rldp_cfg, rtdrv_rldpCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_rldp_portStatus_clear */


/* Function Name:
 *      rtk_rlpp_init
 * Description:
 *      Initialize rlpp module.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize rlpp module before calling any rlpp APIs.
 */
int32
rtk_rlpp_init(void)
{
    rtdrv_rldpCfg_t rldp_cfg;

    /* function body */

    osal_memset(&rldp_cfg,0x0,sizeof(rtdrv_rldpCfg_t));
    SETSOCKOPT(RTDRV_RLPP_INIT, &rldp_cfg, rtdrv_rldpCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_rlpp_init */


/* Function Name:
 *      rtk_rlpp_trap_set
 * Description:
 *      Set RLPP trap to cpu operation, trap or not trap
 * Input:
 *      type - RLPP trap operation type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      Trap the RLPP packet to CPU for software processing
 */
int32
rtk_rlpp_trapType_set(rtk_rlpp_trapType_t type)
{
    rtdrv_rldpCfg_t rldp_cfg;

    /* function body */

    osal_memset(&rldp_cfg,0x0,sizeof(rtdrv_rldpCfg_t));
    osal_memcpy(&rldp_cfg.type, &type, sizeof(rtk_rlpp_trapType_t));
    SETSOCKOPT(RTDRV_RLPP_TRAPTYPE_SET, &rldp_cfg, rtdrv_rldpCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_rlpp_trapType_set */


/* Function Name:
 *      rtk_rlpp_trap_get
 * Description:
 *      Get RLPP trap to cpu operation, trap or not trap
 * Input:
 *      None
 * Output:
 *      pType - RLPP trap operation type
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32
rtk_rlpp_trapType_get(rtk_rlpp_trapType_t *pType)
{
    rtdrv_rldpCfg_t rldp_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pType), RT_ERR_NULL_POINTER);

    /* function body */
    GETSOCKOPT(RTDRV_RLPP_TRAPTYPE_GET, &rldp_cfg, rtdrv_rldpCfg_t, 1);
    osal_memcpy(pType, &rldp_cfg.type, sizeof(rtk_rlpp_trapType_t));

    return RT_ERR_OK;
}   /* end of rtk_rlpp_trapType_get */


