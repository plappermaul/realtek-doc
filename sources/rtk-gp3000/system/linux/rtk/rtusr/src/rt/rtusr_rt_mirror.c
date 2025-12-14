/*
 * Copyright (C) 2019 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: $
 * $Date: $
 *
 * Purpose : Definition of Mirror API
 *
 * Feature : The file have include the following module and sub-modules
 *           (1) Port-based mirror
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
 * Macro Declaration
 */


/*
 * Function Declaration
 */

/* Module Name : Mirror */

/* Function Name:
 *      rt_mirror_init
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
rt_mirror_init(void)
{
    rtdrv_rt_mirrorCfg_t rt_mirror_cfg;

    /* function body */
    osal_memset(&rt_mirror_cfg,0x0,sizeof(rtdrv_rt_mirrorCfg_t));
    SETSOCKOPT(RTDRV_RT_MIRROR_INIT, &rt_mirror_cfg, rtdrv_rt_mirrorCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_mirror_init */

/* Module Name    : Mirror            */
/* Sub-module Name: Port-based mirror */

/* Function Name:
 *      rt_mirror_portBased_set
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
rt_mirror_portBased_set(rt_port_t mirroringPort, rt_portmask_t *pMirroredRxPortmask, rt_portmask_t *pMirroredTxPortmask)
{
    rtdrv_rt_mirrorCfg_t rt_mirror_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pMirroredRxPortmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pMirroredTxPortmask), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_mirror_cfg.mirroringPort, &mirroringPort, sizeof(rt_port_t));
    osal_memcpy(&rt_mirror_cfg.mirroredRxPortmask, pMirroredRxPortmask, sizeof(rt_portmask_t));
    osal_memcpy(&rt_mirror_cfg.mirroredTxPortmask, pMirroredTxPortmask, sizeof(rt_portmask_t));
    SETSOCKOPT(RTDRV_RT_MIRROR_PORTBASED_SET, &rt_mirror_cfg, rtdrv_rt_mirrorCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_mirror_portBased_set */

/* Function Name:
 *      rt_mirror_portBased_get
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
rt_mirror_portBased_get(rt_port_t *pMirroringPort, rt_portmask_t *pMirroredRxPortmask, rt_portmask_t *pMirroredTxPortmask)
{
    rtdrv_rt_mirrorCfg_t rt_mirror_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pMirroringPort), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pMirroredRxPortmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pMirroredTxPortmask), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_mirror_cfg.mirroringPort, pMirroringPort, sizeof(rt_port_t));
    osal_memcpy(&rt_mirror_cfg.mirroredRxPortmask, pMirroredRxPortmask, sizeof(rt_portmask_t));
    osal_memcpy(&rt_mirror_cfg.mirroredTxPortmask, pMirroredTxPortmask, sizeof(rt_portmask_t));
    GETSOCKOPT(RTDRV_RT_MIRROR_PORTBASED_GET, &rt_mirror_cfg, rtdrv_rt_mirrorCfg_t, 1);
    osal_memcpy(pMirroringPort, &rt_mirror_cfg.mirroringPort, sizeof(rt_port_t));
    osal_memcpy(pMirroredRxPortmask, &rt_mirror_cfg.mirroredRxPortmask, sizeof(rt_portmask_t));
    osal_memcpy(pMirroredTxPortmask, &rt_mirror_cfg.mirroredTxPortmask, sizeof(rt_portmask_t));

    return RT_ERR_OK;
}   /* end of rt_mirror_portBased_get */

