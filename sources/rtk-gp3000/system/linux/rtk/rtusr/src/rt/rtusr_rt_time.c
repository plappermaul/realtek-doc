/*
 * Copyright (C) 2023 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision:  $
 * $Date:  $
 *
 * Purpose : Definition of TIME API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) IEEE 1588
 *           (2) PON ToD
 *           (3) 1PPS/ToD
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
#include <rtk/rt/rt_time.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */

/*
 * Function Declaration
 */

/* Function Name:
 *      rt_time_init
 * Description:
 *      Initialize Time module.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize Time module before calling any Time APIs.
 */
int32
rt_time_init(void)
{
    rtdrv_rt_timeCfg_t rt_time_cfg;

    /* function body */
    osal_memset(&rt_time_cfg,0x0,sizeof(rtdrv_rt_timeCfg_t));
    SETSOCKOPT(RTDRV_RT_TIME_INIT, &rt_time_cfg, rtdrv_rt_timeCfg_t, 1);

    return RT_ERR_OK;
} /* end of rt_time_init */

/* Function Name:
 *      rt_time_curTime_get
 * Description:
 *      Get the current time.
 * Input:
 *      None
 * Output:
 *      pTimeStamp - pointer buffer of the current time
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
rt_time_curTime_get(rt_time_timeStamp_t *pTimeStamp)
{
    rtdrv_rt_timeCfg_t rt_time_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pTimeStamp), RT_ERR_NULL_POINTER);

    /* function body */
    GETSOCKOPT(RTDRV_RT_TIME_CURTIME_GET, &rt_time_cfg, rtdrv_rt_timeCfg_t, 1);
    osal_memcpy(pTimeStamp, &rt_time_cfg.timeStamp, sizeof(rt_time_timeStamp_t));

    return RT_ERR_OK;
} /* end of rt_time_curTime_get */

/* Function Name:
 *      rt_time_offsetTime_set
 * Description:
 *      Set the offset time.
 * Input:
 *      sign      - significant
 *      timeStamp - offset timestamp value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      sign=0 for positive adjustment, sign=1 for negative adjustment.
 */
int32
rt_time_offsetTime_set(rt_time_ptpSign_t sign, rt_time_timeStamp_t timeStamp)
{
    rtdrv_rt_timeCfg_t rt_time_cfg;

    osal_memset(&rt_time_cfg,0x0,sizeof(rtdrv_rt_timeCfg_t));
    /* function body */
    osal_memcpy(&rt_time_cfg.sign,&sign,sizeof(rt_time_ptpSign_t));
    osal_memcpy(&rt_time_cfg.timeStamp,&timeStamp,sizeof(rt_time_timeStamp_t));
    SETSOCKOPT(RTDRV_RT_TIME_OFFSETTIME_SET, &rt_time_cfg, rtdrv_rt_timeCfg_t, 1);

    return RT_ERR_OK;
} /* end of rt_time_offsetTime_set */

/* Function Name:
 *      rt_time_frequency_set
 * Description:
 *      Set frequency of timer
 * Input:
 *      freq - reference timestamp value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      setting frequency of calculating timer
 */
int32
rt_time_frequency_set(uint32 freq)
{
    rtdrv_rt_timeCfg_t rt_time_cfg;

    osal_memset(&rt_time_cfg,0x0,sizeof(rtdrv_rt_timeCfg_t));
    /* function body */
    osal_memcpy(&rt_time_cfg.freq,&freq,sizeof(uint32));
    SETSOCKOPT(RTDRV_RT_TIME_FREQUENCY_SET, &rt_time_cfg, rtdrv_rt_timeCfg_t, 1);

    return RT_ERR_OK;
} /* end of rt_time_frequency_set */

/* Function Name:
 *      rt_time_frequency_get
 * Description:
 *      Set frequency of timer
 * Input:
 *      freq - reference timestamp value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      getting frequency of calculating timer
 */
int32
rt_time_frequency_get(uint32 *pFreq)
{
    rtdrv_rt_timeCfg_t rt_time_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pFreq), RT_ERR_NULL_POINTER);

    /* function body */
    GETSOCKOPT(RTDRV_RT_TIME_FREQUENCY_GET, &rt_time_cfg, rtdrv_rt_timeCfg_t, 1);
    osal_memcpy(pFreq, &rt_time_cfg.freq, sizeof(uint32));

    return RT_ERR_OK;
} /* end of rt_time_frequency_get */

/* Function Name:
 *      rt_time_ptpPortEnable_set
 * Description:
 *      Set PTP status of the specified port.
 * Input:
 *      port   - port id
 *      enable - status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - Invalid port id.
 *      RT_ERR_INPUT        - Invalid input parameters.
  * Note:
 *      Set enabling ptp function by port
 */
int32
rt_time_ptpPortEnable_set(rt_port_t port, rt_enable_t enable)
{
    rtdrv_rt_timeCfg_t rt_time_cfg;

    osal_memset(&rt_time_cfg,0x0,sizeof(rtdrv_rt_timeCfg_t));
    /* function body */
    osal_memcpy(&rt_time_cfg.port,&port,sizeof(rt_port_t));
    osal_memcpy(&rt_time_cfg.enable,&enable,sizeof(rt_enable_t));
    SETSOCKOPT(RTDRV_RT_TIME_PTPPORTENABLE_SET, &rt_time_cfg, rtdrv_rt_timeCfg_t, 1);

    return RT_ERR_OK;
} /* end of rt_time_ptpPortEnable_set */

/* Function Name:
 *      rt_time_ptpPortEnable_get
 * Description:
 *      Get PTP status of the specified port.
 * Input:
 *      port    - port id
 * Output:
 *      pEnable - status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      Get enabling status of prp function
 */
int32
rt_time_ptpPortEnable_get(rt_port_t port, rt_enable_t *pEnable)
{
    rtdrv_rt_timeCfg_t rt_time_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_time_cfg.port,&port,sizeof(rt_port_t));
    GETSOCKOPT(RTDRV_RT_TIME_PTPPORTENABLE_GET, &rt_time_cfg, rtdrv_rt_timeCfg_t, 1);
    osal_memcpy(pEnable, &rt_time_cfg.enable, sizeof(rt_enable_t));

    return RT_ERR_OK;
} /* end of rt_time_ptpPortEnable_get */

/* Function Name:
 *      rt_time_ptpIgrAction_set
 * Description:
 *      Set Type of PTP ingress action .
 * Input:
 *      type   - ptp packtet type
 *      action - ptp ingress action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - Invalid input parameters.
  * Note:
 *      Type is Ethertype0x88f7 or udp port 319/320
 *      Action is no-operation or trap-to-cpu
 */
int32
rt_time_ptpIgrAction_set(rt_time_ptpIgr_type_t type,rt_time_ptpIgr_action_t action)
{
    rtdrv_rt_timeCfg_t rt_time_cfg;

    osal_memset(&rt_time_cfg,0x0,sizeof(rtdrv_rt_timeCfg_t));
    /* function body */
    osal_memcpy(&rt_time_cfg.type,&type,sizeof(rt_time_ptpIgr_type_t));
    osal_memcpy(&rt_time_cfg.igrAction,&action,sizeof(rt_time_ptpIgr_action_t));
    SETSOCKOPT(RTDRV_RT_TIME_PTPIGRACTION_SET, &rt_time_cfg, rtdrv_rt_timeCfg_t, 1);

    return RT_ERR_OK;
} /* end of rt_time_ptpIgrAction_set */

/* Function Name:
 *      rt_time_ptpIgrAction_get
 * Description:
 *      Get Type of PTP ingress action .
 * Input:
 *      type   - ptp packtet type
 * Output:
 *      pAction - ptp ingress action
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      Type is Ethertype0x88f7 or udp port 319/320
 *      Action is no-operation or trap-to-cpu
 */
int32
rt_time_ptpIgrAction_get(rt_time_ptpIgr_type_t type,rt_time_ptpIgr_action_t *pAction)
{
    rtdrv_rt_timeCfg_t rt_time_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_time_cfg.type,&type,sizeof(rt_time_ptpIgr_type_t));
    GETSOCKOPT(RTDRV_RT_TIME_PTPIGRACTION_GET, &rt_time_cfg, rtdrv_rt_timeCfg_t, 1);
    osal_memcpy(pAction, &rt_time_cfg.igrAction, sizeof(rt_time_ptpIgr_action_t));

    return RT_ERR_OK;
} /* end of rt_time_ptpIgrAction_get */

/* Function Name:
 *      rt_time_ptpEgrAction_set
 * Description:
 *      Set Type of PTP egress action .
 * Input:
 *      action - ptp engress action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
  * Note:
 *      Action is no-operation or one-step or two-step
 */
int32
rt_time_ptpEgrAction_set(rt_time_ptpEgr_action_t action)
{
    rtdrv_rt_timeCfg_t rt_time_cfg;

    osal_memset(&rt_time_cfg,0x0,sizeof(rtdrv_rt_timeCfg_t));
    /* function body */
    osal_memcpy(&rt_time_cfg.egrAction,&action,sizeof(rt_time_ptpEgr_action_t));
    SETSOCKOPT(RTDRV_RT_TIME_PTPEGRACTION_SET, &rt_time_cfg, rtdrv_rt_timeCfg_t, 1);

    return RT_ERR_OK;
} /* end of rt_time_ptpEgrAction_set */

/* Function Name:
 *      rt_time_ptpEgrAction_get
 * Description:
 *      Get Type of PTP egress action .
 * Input:
 *      None
 * Output:
 *      pAction - ptp egress action
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *     Action is no-operation or one-step or two-step
 */
int32
rt_time_ptpEgrAction_get(rt_time_ptpEgr_action_t *pAction)
{
    rtdrv_rt_timeCfg_t rt_time_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    /* function body */
    GETSOCKOPT(RTDRV_RT_TIME_PTPEGRACTION_GET, &rt_time_cfg, rtdrv_rt_timeCfg_t, 1);
    osal_memcpy(pAction, &rt_time_cfg.egrAction, sizeof(rt_time_ptpEgr_action_t));

    return RT_ERR_OK;
} /* end of rt_time_ptpEgrAction_get */

/* Function Name:
 *      rt_time_ponTodTime_set
 * Description:
 *      Set the PON TOD time.
 * Input:
 *      ponTod   - pon tod configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 * Note:
 *      Set corresponding superfram/localtime to update timer
 */
int32
rt_time_ponTodTime_set(rt_pon_tod_t ponTod)
{
    rtdrv_rt_timeCfg_t rt_time_cfg;

    osal_memset(&rt_time_cfg,0x0,sizeof(rtdrv_rt_timeCfg_t));
    
    osal_memcpy(&rt_time_cfg.ponTod, &ponTod, sizeof(rt_pon_tod_t));
    SETSOCKOPT(RTDRV_RT_TIME_PONTODTIME_SET, &rt_time_cfg, rtdrv_rt_timeCfg_t, 1);

    return RT_ERR_OK;
} /* end of rt_time_ponTodTime_set */

/* Function Name:
 *      rt_time_ponTodTime_get
 * Description:
 *      Get the PON TOD time.
 * Input:
 *      None
 * Output:
 *      pPonTod   - pon tod configuration
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 * Note:
 *      Get corresponding superfram/localtime to update timer
 */
int32
rt_time_ponTodTime_get(rt_pon_tod_t *pPonTod)
{
    rtdrv_rt_timeCfg_t rt_time_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pPonTod), RT_ERR_NULL_POINTER);

    /* function body */
    GETSOCKOPT(RTDRV_RT_TIME_PONTODTIME_GET, &rt_time_cfg, rtdrv_rt_timeCfg_t, 1);
    osal_memcpy(pPonTod, &rt_time_cfg.ponTod, sizeof(rt_pon_tod_t));

    return RT_ERR_OK;
} /* end of rt_time_ponTodTime_get */

/* Function Name:
 *      rt_time_ponTodTimeEnable_set
 * Description:
 *      Set the PON TOD time.
 * Input:
 *      enable   - enable pon tod configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 * Note:
 *      Enable/Disable PON ToD to update time
 */
int32
rt_time_ponTodTimeEnable_set(rt_enable_t enable)
{
    rtdrv_rt_timeCfg_t rt_time_cfg;

    osal_memset(&rt_time_cfg,0x0,sizeof(rtdrv_rt_timeCfg_t));

    osal_memcpy(&rt_time_cfg.enable, &enable, sizeof(rt_enable_t));
    SETSOCKOPT(RTDRV_RT_TIME_PONTODTIMEENABLE_SET, &rt_time_cfg, rtdrv_rt_timeCfg_t, 1);

    return RT_ERR_OK;
} /* end of rt_time_ponTodTimeEnable_set */

/* Function Name:
 *      rt_time_ponTodTimeEnable_get
 * Description:
 *      Get the PON TOD time.
 * Input:
 *      None
 * Output:
 *      pEnable   - enable pon tod configuration
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 * Note:
 *      Getting status enable/eisable PON ToD to update time
 */
int32
rt_time_ponTodTimeEnable_get(rt_enable_t *pEnable)
{
    rtdrv_rt_timeCfg_t rt_time_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    GETSOCKOPT(RTDRV_RT_TIME_PONTODTIMEENABLE_GET, &rt_time_cfg, rtdrv_rt_timeCfg_t, 1);
    osal_memcpy(pEnable, &rt_time_cfg.enable, sizeof(rt_enable_t));

    return RT_ERR_OK;
} /* end of rt_time_ponTodTimeEnable_get */
