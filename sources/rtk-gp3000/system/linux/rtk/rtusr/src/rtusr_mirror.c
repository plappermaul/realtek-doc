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
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <osal/lib.h>
#include <rtk/rtusr/include/rtusr_util.h>
#include <rtdrv/rtdrv_netfilter.h>
#include <common/rt_type.h>
#include <rtk/port.h>


/*
 * Symbol Definition
 */

/*
 * Macro Declaration
 */


/*
 * Function Declaration
 */

/* Module Name : Mirror */

/* Function Name:
 *      rtk_mirror_init
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
rtk_mirror_init(void)
{
    rtdrv_mirrorCfg_t mirror_cfg;

    /* function body */
    osal_memset(&mirror_cfg,0x0,sizeof(rtdrv_mirrorCfg_t));
    SETSOCKOPT(RTDRV_MIRROR_INIT, &mirror_cfg, rtdrv_mirrorCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_mirror_init */

/* Module Name    : Mirror            */
/* Sub-module Name: Port-based mirror */

/* Function Name:
 *      rtk_mirror_portBased_set
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
rtk_mirror_portBased_set(rtk_port_t mirroringPort, rtk_portmask_t *pMirroredRxPortmask, rtk_portmask_t *pMirroredTxPortmask)
{
    rtdrv_mirrorCfg_t mirror_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pMirroredRxPortmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pMirroredTxPortmask), RT_ERR_NULL_POINTER);

    osal_memset(&mirror_cfg,0x0,sizeof(rtdrv_mirrorCfg_t));
    /* function body */
    osal_memcpy(&mirror_cfg.mirroringPort, &mirroringPort, sizeof(rtk_port_t));
    osal_memcpy(&mirror_cfg.mirroredRxPortmask, pMirroredRxPortmask, sizeof(rtk_portmask_t));
    osal_memcpy(&mirror_cfg.mirroredTxPortmask, pMirroredTxPortmask, sizeof(rtk_portmask_t));
    SETSOCKOPT(RTDRV_MIRROR_PORTBASED_SET, &mirror_cfg, rtdrv_mirrorCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_mirror_portBased_set */

/* Function Name:
 *      rtk_mirror_portBased_get
 * Description:
 *      Get port mirror function.
 * Input:
 *      None
 * Output:
 *      pMirroringPort         - Monitor port.
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
rtk_mirror_portBased_get(rtk_port_t *pMirroringPort, rtk_portmask_t *pMirroredRxPortmask, rtk_portmask_t *pMirroredTxPortmask)
{
    rtdrv_mirrorCfg_t mirror_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pMirroringPort), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pMirroredRxPortmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pMirroredTxPortmask), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&mirror_cfg.mirroringPort, pMirroringPort, sizeof(rtk_port_t));
    osal_memcpy(&mirror_cfg.mirroredRxPortmask, pMirroredRxPortmask, sizeof(rtk_portmask_t));
    osal_memcpy(&mirror_cfg.mirroredTxPortmask, pMirroredTxPortmask, sizeof(rtk_portmask_t));
    GETSOCKOPT(RTDRV_MIRROR_PORTBASED_GET, &mirror_cfg, rtdrv_mirrorCfg_t, 1);
    osal_memcpy(pMirroringPort, &mirror_cfg.mirroringPort, sizeof(rtk_port_t));
    osal_memcpy(pMirroredRxPortmask, &mirror_cfg.mirroredRxPortmask, sizeof(rtk_portmask_t));
    osal_memcpy(pMirroredTxPortmask, &mirror_cfg.mirroredTxPortmask, sizeof(rtk_portmask_t));

    return RT_ERR_OK;
}   /* end of rtk_mirror_portBased_get */

/* Function Name:
 *      rtk_mirror_portIso_set
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
rtk_mirror_portIso_set(rtk_enable_t enable)
{
    rtdrv_mirrorCfg_t mirror_cfg;

    /* function body */
    osal_memset(&mirror_cfg,0x0,sizeof(rtdrv_mirrorCfg_t));
    osal_memcpy(&mirror_cfg.enable, &enable, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_MIRROR_PORTISO_SET, &mirror_cfg, rtdrv_mirrorCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_mirror_portIso_set */

/* Function Name:
 *      rtk_mirror_portIso_get
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
rtk_mirror_portIso_get(rtk_enable_t *pEnable)
{
    rtdrv_mirrorCfg_t mirror_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&mirror_cfg.enable, pEnable, sizeof(rtk_enable_t));
    GETSOCKOPT(RTDRV_MIRROR_PORTISO_GET, &mirror_cfg, rtdrv_mirrorCfg_t, 1);
    osal_memcpy(pEnable, &mirror_cfg.enable, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_mirror_portIso_get */


