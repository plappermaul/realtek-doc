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
 * $Revision: 85849 $
 * $Date: 2018-01-30 10:05:23 +0800 (Tue, 30 Jan 2018) $
 *
 * Purpose : Definition of Security API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) attack prevention
 */

#ifndef __RT_SEC_H__
#define __RT_SEC_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
/*
 * Symbol Definition
 */

/* Type of attack prevention */
typedef enum rt_sec_attackType_e
{
    RT_DAEQSA_DENY = 0,
    RT_LAND_DENY,
    RT_BLAT_DENY,
    RT_SYNFIN_DENY,
    RT_XMA_DENY,
    RT_NULLSCAN_DENY,
    RT_SYN_SPORTL1024_DENY,
    RT_TCPHDR_MIN_CHECK,
    RT_TCP_FRAG_OFF_MIN_CHECK,
    RT_ICMP_FRAG_PKTS_DENY,
    RT_POD_DENY,
    RT_UDPDOMB_DENY,
    RT_SYNWITHDATA_DENY,
    RT_SYNFLOOD_DENY,
    RT_FINFLOOD_DENY,
    RT_ICMPFLOOD_DENY,
    RT_ATTACK_TYPE_END
} rt_sec_attackType_t;

typedef enum rt_sec_attackFloodType_e
{
    RT_SEC_ICMPFLOOD,
    RT_SEC_SYNCFLOOD,
    RT_SEC_FINFLOOD,
    RT_SEC_FLOOD_END
} rt_sec_attackFloodType_t;

/*
 * Data Declaration
 */


/*
 * Function Declaration
 */

/* Module Name : Security */

/* Function Name:
 *      rt_sec_init
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
extern int32
rt_sec_init(void);

/* Module Name    : Security          */
/* Sub-module Name: Attack prevention */


/* Function Name:
 *      rt_sec_portAttackPreventState_get
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
extern int32
rt_sec_portAttackPreventState_get(rt_port_t port, rt_enable_t *pEnable);

/* Function Name:
 *      rt_sec_portAttackPreventState_set
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
extern int32
rt_sec_portAttackPreventState_set(rt_port_t port, rt_enable_t enable);



/* Function Name:
 *      rt_sec_attackPrevent_get
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
extern int32
rt_sec_attackPrevent_get(
    rt_sec_attackType_t    attackType,
    rt_action_t            *pAction);

/* Function Name:
 *      rt_sec_attackPrevent_set
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
extern int32
rt_sec_attackPrevent_set(
    rt_sec_attackType_t    attackType,
    rt_action_t            action);


/* Function Name:
 *      rt_sec_attackFloodThresh_get
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
extern int32
rt_sec_attackFloodThresh_get(rt_sec_attackFloodType_t type, uint32 *pFloodThresh);

/* Function Name:
 *      rt_sec_attackFloodThresh_set
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
extern int32
rt_sec_attackFloodThresh_set(rt_sec_attackFloodType_t type, uint32 floodThresh);

/* Function Name:
 *      rt_sec_attackFloodThreshUnit_get
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
extern int32
rt_sec_attackFloodThreshUnit_get(rt_sec_attackFloodType_t type, uint32 *pFloodThreshUnit);

/* Function Name:
 *      rt_sec_attackFloodThreshUnit_set
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
extern int32
rt_sec_attackFloodThreshUnit_set(rt_sec_attackFloodType_t type, uint32 floodThreshUnit);


#endif /* __RT_SEC_H__ */

