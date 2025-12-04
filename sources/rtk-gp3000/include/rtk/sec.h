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
 * Purpose : Definition of Security API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) attack prevention 
 */

#ifndef __RTK_SEC_H__
#define __RTK_SEC_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
/*
 * Symbol Definition
 */

/* Type of attack prevention */
typedef enum rtk_sec_attackType_e
{
    DAEQSA_DENY = 0,
    LAND_DENY,
    BLAT_DENY,
    SYNFIN_DENY,
    XMA_DENY,
    NULLSCAN_DENY,
    SYN_SPORTL1024_DENY,
    TCPHDR_MIN_CHECK,
    TCP_FRAG_OFF_MIN_CHECK,
    ICMP_FRAG_PKTS_DENY,
    POD_DENY,
    UDPDOMB_DENY,
    SYNWITHDATA_DENY,
    SYNFLOOD_DENY,
    FINFLOOD_DENY,
    ICMPFLOOD_DENY,    
    ATTACK_TYPE_END
} rtk_sec_attackType_t;

typedef enum rtk_sec_attackFloodType_e
{
    SEC_ICMPFLOOD,
    SEC_SYNCFLOOD,
    SEC_FINFLOOD,
    SEC_FLOOD_END
} rtk_sec_attackFloodType_t;

typedef enum rtk_sec_floodThreshUnit_e
{
    SEC_THRESH_UNIT_1MS = 0,
    SEC_THRESH_UNIT_2MS,
    SEC_THRESH_UNIT_4MS,
    SEC_THRESH_UNIT_8MS,
    SEC_THRESH_UNIT_16MS,
    SEC_THRESH_UNIT_32MS,
    SEC_THRESH_UNIT_64MS,
    SEC_THRESH_UNIT_128MS,
    SEC_THRESH_UNIT_256MS,
    SEC_THRESH_UNIT_512MS,
    SEC_THRESH_UNIT_END
} rtk_sec_floodThreshUnit_t;
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
extern int32
rtk_sec_init(void);

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
extern int32
rtk_sec_portAttackPreventState_get(rtk_port_t port, rtk_enable_t *pEnable);

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
extern int32
rtk_sec_portAttackPreventState_set(rtk_port_t port, rtk_enable_t enable);



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
extern int32
rtk_sec_attackPrevent_get(
    rtk_sec_attackType_t    attackType,
    rtk_action_t            *pAction);

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
extern int32
rtk_sec_attackPrevent_set(
    rtk_sec_attackType_t    attackType,
    rtk_action_t            action);


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
extern int32
rtk_sec_attackFloodThresh_get(rtk_sec_attackFloodType_t type, uint32 *pFloodThresh);

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
extern int32
rtk_sec_attackFloodThresh_set(rtk_sec_attackFloodType_t type, uint32 floodThresh);

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
extern int32
rtk_sec_attackFloodThreshUnit_get(rtk_sec_attackFloodType_t type, uint32 *pFloodThreshUnit);

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
extern int32
rtk_sec_attackFloodThreshUnit_set(rtk_sec_attackFloodType_t type, uint32 floodThreshUnit);

#endif /* __RTK_SEC_H__ */

