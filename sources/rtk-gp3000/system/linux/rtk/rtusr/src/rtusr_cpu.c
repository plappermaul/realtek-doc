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
 * Purpose : Definition of CPU Tag API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) CPU tag functions set/get
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
/* Function Name:
 *      rtk_cpu_init
 * Description:
 *      Initialize cpu tag module.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize cpu tag module before calling any cpu tag APIs.
 */
int32
rtk_cpu_init(void)
{
    rtdrv_cpuCfg_t cpu_cfg;

    /* function body */
    osal_memset(&cpu_cfg, 0x0, sizeof(rtdrv_cpuCfg_t));
    SETSOCKOPT(RTDRV_CPU_INIT, &cpu_cfg, rtdrv_cpuCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_cpu_init */

/* Function Name:
 *      rtk_cpu_awarePortMask_set
 * Description:
 *      Set CPU awared port mask.
 * Input:
 *      port_mask   - CPU awared port mask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      Must initialize cpu tag module before calling any cpu tag APIs.
 */
int32
rtk_cpu_awarePortMask_set(rtk_portmask_t port_mask)
{
    rtdrv_cpuCfg_t cpu_cfg;

    /* function body */
    osal_memset(&cpu_cfg, 0x0, sizeof(rtdrv_cpuCfg_t));
    osal_memcpy(&cpu_cfg.port_mask, &port_mask, sizeof(rtk_portmask_t));
    SETSOCKOPT(RTDRV_CPU_AWAREPORTMASK_SET, &cpu_cfg, rtdrv_cpuCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_cpu_awarePortMask_set */

/* Function Name:
 *      dal_apollo_cpu_awarePortMask_get
 * Description:
 *      Get CPU awared port mask.
 * Input:
 *      pPort_mask   - the pointer of CPU awared port mask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      Must initialize cpu tag module before calling any cpu tag APIs.
 */
int32
rtk_cpu_awarePortMask_get(rtk_portmask_t *pPort_mask)
{
    rtdrv_cpuCfg_t cpu_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pPort_mask), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&cpu_cfg.port_mask, pPort_mask, sizeof(rtk_portmask_t));
    GETSOCKOPT(RTDRV_CPU_AWAREPORTMASK_GET, &cpu_cfg, rtdrv_cpuCfg_t, 1);
    osal_memcpy(pPort_mask, &cpu_cfg.port_mask, sizeof(rtk_portmask_t));

    return RT_ERR_OK;
}   /* end of rtk_cpu_awarePortMask_get */

/* Function Name:
 *      rtk_cpu_tagFormat_set
 * Description:
 *      Set CPU tag format.
 * Input:
 *      mode    - CPU tag format mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_OUT_OF_RANGE
 * Note:
 *      Must initialize cpu tag module before calling any cpu tag APIs.
 */
int32
rtk_cpu_tagFormat_set(rtk_cpu_tag_fmt_t mode)
{
    rtdrv_cpuCfg_t cpu_cfg;

    /* function body */
    osal_memset(&cpu_cfg, 0x0, sizeof(rtdrv_cpuCfg_t));
    osal_memcpy(&cpu_cfg.mode, &mode, sizeof(rtk_cpu_tag_fmt_t));
    SETSOCKOPT(RTDRV_CPU_TAGFORMAT_SET, &cpu_cfg, rtdrv_cpuCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_cpu_tagFormat_set */

/* Function Name:
 *      rtk_cpu_tagFormat_get
 * Description:
 *      Get CPU tag format.
 * Input:
 *      pMode    - the pointer of CPU tag format mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      Must initialize cpu tag module before calling any cpu tag APIs.
 */
int32
rtk_cpu_tagFormat_get(rtk_cpu_tag_fmt_t *pMode)
{
    rtdrv_cpuCfg_t cpu_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pMode), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&cpu_cfg.mode, pMode, sizeof(rtk_cpu_tag_fmt_t));
    GETSOCKOPT(RTDRV_CPU_TAGFORMAT_GET, &cpu_cfg, rtdrv_cpuCfg_t, 1);
    osal_memcpy(pMode, &cpu_cfg.mode, sizeof(rtk_cpu_tag_fmt_t));

    return RT_ERR_OK;
}   /* end of rtk_cpu_tagFormat_get */

/* Function Name:
 *      rtk_cpu_trapInsertTag_set
 * Description:
 *      Set trap CPU insert tag state.
 * Input:
 *      state    - insert CPU tag state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_OUT_OF_RANGE
 * Note:
 *      Must initialize cpu tag module before calling any cpu tag APIs.
 */
int32
rtk_cpu_trapInsertTag_set(rtk_enable_t state)
{
    rtdrv_cpuCfg_t cpu_cfg;

    /* function body */
    osal_memset(&cpu_cfg, 0x0, sizeof(rtdrv_cpuCfg_t));
    osal_memcpy(&cpu_cfg.state, &state, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_CPU_TRAPINSERTTAG_SET, &cpu_cfg, rtdrv_cpuCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_cpu_trapInsertTag_set */

/* Function Name:
 *      rtk_cpu_trapInsertTag_get
 * Description:
 *      Get trap CPU insert tag state.
 * Input:
 *      pState    - the pointer of insert CPU tag state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      Must initialize cpu tag module before calling any cpu tag APIs.
 */
int32
rtk_cpu_trapInsertTag_get(rtk_enable_t *pState)
{
    rtdrv_cpuCfg_t cpu_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&cpu_cfg.state, pState, sizeof(rtk_enable_t));
    GETSOCKOPT(RTDRV_CPU_TRAPINSERTTAG_GET, &cpu_cfg, rtdrv_cpuCfg_t, 1);
    osal_memcpy(pState, &cpu_cfg.state, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_cpu_trapInsertTag_get */

/* Function Name:
 *      rtk_cpu_tagAware_set
 * Description:
 *      Set CPU tag aware state.
 * Input:
 *      state    - CPU tag aware state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_OUT_OF_RANGE
 * Note:
 *      Must initialize cpu tag module before calling any cpu tag APIs.
 */
int32
rtk_cpu_tagAware_set(rtk_enable_t state)
{
    rtdrv_cpuCfg_t cpu_cfg;

    /* function body */
    osal_memset(&cpu_cfg, 0x0, sizeof(rtdrv_cpuCfg_t));
    osal_memcpy(&cpu_cfg.state, &state, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_CPU_TAGAWARE_SET, &cpu_cfg, rtdrv_cpuCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_cpu_tagAware_set */

/* Function Name:
 *      rtk_cpu_tagAware_get
 * Description:
 *      Get CPU tag aware state.
 * Input:
 *      pState    - the pointer of CPU tag aware state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      Must initialize cpu tag module before calling any cpu tag APIs.
 */
int32
rtk_cpu_tagAware_get(rtk_enable_t *pState)
{
    rtdrv_cpuCfg_t cpu_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&cpu_cfg.state, pState, sizeof(rtk_enable_t));
    GETSOCKOPT(RTDRV_CPU_TAGAWARE_GET, &cpu_cfg, rtdrv_cpuCfg_t, 1);
    osal_memcpy(pState, &cpu_cfg.state, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_cpu_tagAware_get */

/* Function Name:
 *      rtk_cpu_trapInsertTagByPort_set
 * Description:
 *      Set trap CPU insert tag state.
 * Input:
 *      port     - CPU port ID
 *      state    - insert CPU tag state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_OUT_OF_RANGE
 * Note:
 *      Must initialize cpu tag module before calling any cpu tag APIs.
 */
int32
rtk_cpu_trapInsertTagByPort_set(rtk_port_t port, rtk_enable_t state)
{
    rtdrv_cpuCfg_t cpu_cfg;

    /* function body */
    osal_memset(&cpu_cfg, 0x0, sizeof(rtdrv_cpuCfg_t));
    osal_memcpy(&cpu_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&cpu_cfg.state, &state, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_CPU_TRAPINSERTTAGBYPORT_SET, &cpu_cfg, rtdrv_cpuCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_cpu_trapInsertTagByPort_set */

/* Function Name:
 *      rtk_cpu_trapInsertTagByPort_get
 * Description:
 *      Get trap CPU insert tag state.
 * Input:
 *      port     - CPU port ID
 *      pState   - the pointer of insert CPU tag state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      Must initialize cpu tag module before calling any cpu tag APIs.
 */
int32
rtk_cpu_trapInsertTagByPort_get(rtk_port_t port, rtk_enable_t *pState)
{
    rtdrv_cpuCfg_t cpu_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&cpu_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_CPU_TRAPINSERTTAGBYPORT_GET, &cpu_cfg, rtdrv_cpuCfg_t, 1);
    osal_memcpy(pState, &cpu_cfg.state, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_cpu_trapInsertTagByPort_get */

/* Function Name:
 *      rtk_cpu_tagAwareByPort_set
 * Description:
 *      Set CPU tag aware state.
 * Input:
 *      port     - CPU port ID
 *      state    - CPU tag aware state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_OUT_OF_RANGE
 * Note:
 *      Must initialize cpu tag module before calling any cpu tag APIs.
 */
int32
rtk_cpu_tagAwareByPort_set(rtk_port_t port, rtk_enable_t state)
{
    rtdrv_cpuCfg_t cpu_cfg;

    /* function body */
    osal_memset(&cpu_cfg, 0x0, sizeof(rtdrv_cpuCfg_t));
    osal_memcpy(&cpu_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&cpu_cfg.state, &state, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_CPU_TAGAWAREBYPORT_SET, &cpu_cfg, rtdrv_cpuCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_cpu_tagAwareByPort_set */

/* Function Name:
 *      rtk_cpu_tagAwareByPort_get
 * Description:
 *      Get CPU tag aware state.
 * Input:
 *      port     - CPU port ID
 *      pState   - the pointer of CPU tag aware state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      Must initialize cpu tag module before calling any cpu tag APIs.
 */
int32
rtk_cpu_tagAwareByPort_get(rtk_port_t port, rtk_enable_t *pState)
{
    rtdrv_cpuCfg_t cpu_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&cpu_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_CPU_TAGAWAREBYPORT_GET, &cpu_cfg, rtdrv_cpuCfg_t, 1);
    osal_memcpy(pState, &cpu_cfg.state, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_cpu_tagAwareByPort_get */
