/*
 * Copyright (C) 2013 Realtek Semiconductor Corp. 
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
 * $Date: 2013-05-03 17:35:27 +0800 (Fri, 03 May 2013) $
 *
 * Purpose : Definition of Security API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) attack prevention 
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
#include <common/rt_error.h>
/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */


/*
 * Function Declaration
 */

/* Module Name : Security */

/* Function Name:
 *      rtk_sec_init
 * Description:
 *      Initialize security module.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize security module before calling any sec APIs.
 */
int32
rtk_sec_init(void)
{
    rtdrv_secCfg_t sec_cfg;

    /* function body */
    osal_memset(&sec_cfg, 0x0, sizeof(rtdrv_secCfg_t));
    SETSOCKOPT(RTDRV_SEC_INIT, &sec_cfg, rtdrv_secCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_sec_init */

/* Module Name    : Security          */
/* Sub-module Name: Attack prevention */


/* Function Name:
 *      rtk_sec_portAttackPreventState_get
 * Description:
 *      Per port get attack prevention confi state
 * Input:
 *      port    - port id
 * Output:
 *      pEnable - status attack prevention
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      The status attack prevention:
 *      - DISABLED
 *      - ENABLED
 */
int32
rtk_sec_portAttackPreventState_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    rtdrv_secCfg_t sec_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&sec_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_SEC_PORTATTACKPREVENTSTATE_GET, &sec_cfg, rtdrv_secCfg_t, 1);
    osal_memcpy(pEnable, &sec_cfg.enable, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_sec_portAttackPreventState_get */

/* Function Name:
 *      rtk_sec_portAttackPreventState_set
 * Description:
 *      Per port set attack prevention confi state
 * Input:
 *      port   - port id.
 *      enable - status attack prevention
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - Invalid port id
 * Note:
 *      The status attack prevention:
 *      - DISABLED
 *      - ENABLED
 */
int32
rtk_sec_portAttackPreventState_set(rtk_port_t port, rtk_enable_t enable)
{
    rtdrv_secCfg_t sec_cfg;

    /* function body */
    osal_memset(&sec_cfg, 0x0, sizeof(rtdrv_secCfg_t));
    osal_memcpy(&sec_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&sec_cfg.enable, &enable, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_SEC_PORTATTACKPREVENTSTATE_SET, &sec_cfg, rtdrv_secCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_sec_portAttackPreventState_set */



/* Function Name:
 *      rtk_sec_attackPrevent_get
 * Description:
 *      Get action for each kind of attack on specified port.
 * Input:
 *      attackType - type of attack
 * Output:
 *      pAction     - pointer to action for attack
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Action is as following:
 *      - ACTION_TRAP2CPU
 *      - ACTION_DROP
 *      - ACTION_FORWARD
 */
int32
rtk_sec_attackPrevent_get(
    rtk_sec_attackType_t    attackType,
    rtk_action_t            *pAction)
{
    rtdrv_secCfg_t sec_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&sec_cfg.attackType, &attackType, sizeof(rtk_sec_attackType_t));
    GETSOCKOPT(RTDRV_SEC_ATTACKPREVENT_GET, &sec_cfg, rtdrv_secCfg_t, 1);
    osal_memcpy(pAction, &sec_cfg.action, sizeof(rtk_action_t));

    return RT_ERR_OK;
}   /* end of rtk_sec_attackPrevent_get */

/* Function Name:
 *      rtk_sec_attackPrevent_set
 * Description:
 *      Set action for each kind of attack.
 * Input:
 *      attack_type - type of attack
 *      action      - action for attack
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_FWD_ACTION - invalid forwarding action
 *      RT_ERR_INPUT      - invalid input parameter
 * Note:
 *      Action is as following:
 *      - ACTION_TRAP2CPU
 *      - ACTION_DROP
 *      - ACTION_FORWARD
 */
int32
rtk_sec_attackPrevent_set(
    rtk_sec_attackType_t    attackType,
    rtk_action_t            action)
{
    rtdrv_secCfg_t sec_cfg;

    /* function body */
    osal_memset(&sec_cfg, 0x0, sizeof(rtdrv_secCfg_t));
    osal_memcpy(&sec_cfg.attackType, &attackType, sizeof(rtk_sec_attackType_t));
    osal_memcpy(&sec_cfg.action, &action, sizeof(rtk_action_t));
    SETSOCKOPT(RTDRV_SEC_ATTACKPREVENT_SET, &sec_cfg, rtdrv_secCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_sec_attackPrevent_set */


/* Function Name:
 *      rtk_sec_attackFloodThresh_get
 * Description:
 *      Get flood threshold.
 * Input:
 *      None
 * Output:
 *      pFloodThresh - pointer to flood threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Flood type is as following:
 *      - SEC_ICMPFLOOD
 *      - SEC_SYNCFLOOD
 *      - SEC_FINFLOOD
 */
int32
rtk_sec_attackFloodThresh_get(rtk_sec_attackFloodType_t type, uint32 *pFloodThresh)
{
    rtdrv_secCfg_t sec_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pFloodThresh), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&sec_cfg.type, &type, sizeof(rtk_sec_attackFloodType_t));
    GETSOCKOPT(RTDRV_SEC_ATTACKFLOODTHRESH_GET, &sec_cfg, rtdrv_secCfg_t, 1);
    osal_memcpy(pFloodThresh, &sec_cfg.floodThresh, sizeof(uint32));

    return RT_ERR_OK;
}   /* end of rtk_sec_attackFloodThresh_get */

/* Function Name:
 *      rtk_sec_attackFloodThresh_set
 * Description:
 *      Set flood threshold.
 * Input:
 *      floodThresh - flood threshold
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Flood type is as following:
 *      - SEC_ICMPFLOOD
 *      - SEC_SYNCFLOOD
 *      - SEC_FINFLOOD
 */
int32
rtk_sec_attackFloodThresh_set(rtk_sec_attackFloodType_t type, uint32 floodThresh)
{
    rtdrv_secCfg_t sec_cfg;

    /* function body */
    osal_memset(&sec_cfg, 0x0, sizeof(rtdrv_secCfg_t));
    osal_memcpy(&sec_cfg.type, &type, sizeof(rtk_sec_attackFloodType_t));
    osal_memcpy(&sec_cfg.floodThresh, &floodThresh, sizeof(uint32));
    SETSOCKOPT(RTDRV_SEC_ATTACKFLOODTHRESH_SET, &sec_cfg, rtdrv_secCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_sec_attackFloodThresh_set */

/* Function Name:
 *      rtk_sec_attackFloodThreshUnit_get
 * Description:
 *      Get time unit of flood threshold, 1/256/512 ms.
 * Input:
 *      None
 * Output:
 *      pFloodThreshUnit - pointer to flood threshold time unit
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Flood type is as following:
 *      - SEC_ICMPFLOOD
 *      - SEC_SYNCFLOOD
 *      - SEC_FINFLOOD
 */
int32
rtk_sec_attackFloodThreshUnit_get(rtk_sec_attackFloodType_t type, uint32 *pFloodThreshUnit)
{
    rtdrv_secCfg_t sec_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pFloodThreshUnit), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&sec_cfg.type, &type, sizeof(rtk_sec_attackFloodType_t));
    GETSOCKOPT(RTDRV_SEC_ATTACKFLOODTHRESHUNIT_GET, &sec_cfg, rtdrv_secCfg_t, 1);
    osal_memcpy(pFloodThreshUnit, &sec_cfg.floodThreshUnit, sizeof(uint32));

    return RT_ERR_OK;
}   /* end of rtk_sec_attackFloodThreshUnit_get */

/* Function Name:
 *      rtk_sec_attackFloodThreshUnit_set
 * Description:
 *      Set time unit of flood threshold, 1/256/512 ms.
 * Input:
 *      floodThresh - flood threshold time unit
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Flood type is as following:
 *      - SEC_ICMPFLOOD
 *      - SEC_SYNCFLOOD
 *      - SEC_FINFLOOD
 */
int32
rtk_sec_attackFloodThreshUnit_set(rtk_sec_attackFloodType_t type, uint32 floodThreshUnit)
{
    rtdrv_secCfg_t sec_cfg;

    /* function body */
    osal_memset(&sec_cfg, 0x0, sizeof(rtdrv_secCfg_t));
    osal_memcpy(&sec_cfg.type, &type, sizeof(rtk_sec_attackFloodType_t));
    osal_memcpy(&sec_cfg.floodThreshUnit, &floodThreshUnit, sizeof(uint32));
    SETSOCKOPT(RTDRV_SEC_ATTACKFLOODTHRESHUNIT_SET, &sec_cfg, rtdrv_secCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_sec_attackFloodThreshUnit_set */


