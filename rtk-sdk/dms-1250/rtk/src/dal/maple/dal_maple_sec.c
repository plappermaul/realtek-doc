/*
 * Copyright(c) Realtek Semiconductor Corporation, 2009
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Definition those public security APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) Attack prevention
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_bitop.h>
#include <common/util/rt_util.h>
#include <common/debug/rt_log.h>
#include <osal/sem.h>
#include <osal/lib.h>
#include <osal/memory.h>
#include <hal/chipdef/allmem.h>
#include <hal/chipdef/allreg.h>
#include <hal/chipdef/maple/rtk_maple_table_struct.h>
#include <hal/chipdef/maple/rtk_maple_reg_struct.h>
#include <hal/mac/reg.h>
#include <hal/mac/mem.h>
#include <hal/common/halctrl.h>
#include <dal/maple/dal_maple_sec.h>
#include <dal/dal_mgmt.h>
#include <rtk/default.h>
#include <rtk/sec.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
static uint32               sec_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};
static osal_mutex_t         sec_sem[RTK_MAX_NUM_OF_UNIT];

/*
 * Macro Declaration
 */
#define SEC_SEM_LOCK(unit)    \
do {\
    if (osal_sem_mutex_take(sec_sem[unit], OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_LOCK_FAILED, (MOD_SEC|MOD_DAL), "semaphore lock failed");\
        return RT_ERR_SEM_LOCK_FAILED;\
    }\
} while(0)

#define SEC_SEM_UNLOCK(unit)   \
do {\
    if (osal_sem_mutex_give(sec_sem[unit]) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_UNLOCK_FAILED, (MOD_SEC|MOD_DAL), "semaphore unlock failed");\
        return RT_ERR_SEM_UNLOCK_FAILED;\
    }\
} while(0)

/*
 * Function Declaration
 */

/* Module Name : Security */

/* Function Name:
 *      dal_maple_secMapper_init
 * Description:
 *      Hook sec module of the specified device.
 * Input:
 *      pMapper - pointer of mapper
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Must Hook sec module before calling any sec APIs.
 */
int32
dal_maple_secMapper_init(dal_mapper_t *pMapper)
{
    pMapper->sec_init = dal_maple_sec_init;
    pMapper->sec_portAttackPrevent_get = dal_maple_sec_portAttackPrevent_get;
    pMapper->sec_portAttackPrevent_set = dal_maple_sec_portAttackPrevent_set;
    pMapper->sec_portAttackPreventEnable_get = dal_maple_sec_portAttackPreventEnable_get;
    pMapper->sec_portAttackPreventEnable_set = dal_maple_sec_portAttackPreventEnable_set;
    pMapper->sec_attackPreventAction_get = dal_maple_sec_attackPreventAction_get;
    pMapper->sec_attackPreventAction_set = dal_maple_sec_attackPreventAction_set;
    pMapper->sec_minIPv6FragLen_get = dal_maple_sec_minIPv6FragLen_get;
    pMapper->sec_minIPv6FragLen_set = dal_maple_sec_minIPv6FragLen_set;
    pMapper->sec_maxPingLen_get = dal_maple_sec_maxPingLen_get;
    pMapper->sec_maxPingLen_set = dal_maple_sec_maxPingLen_set;
    pMapper->sec_minTCPHdrLen_get = dal_maple_sec_minTCPHdrLen_get;
    pMapper->sec_minTCPHdrLen_set = dal_maple_sec_minTCPHdrLen_set;
    pMapper->sec_smurfNetmaskLen_get = dal_maple_sec_smurfNetmaskLen_get;
    pMapper->sec_smurfNetmaskLen_set = dal_maple_sec_smurfNetmaskLen_set;

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_maple_sec_init
 * Description:
 *      Initialize security module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize security module before calling any sec APIs.
 */
int32
dal_maple_sec_init(uint32 unit)
{
    RT_LOG(LOG_DEBUG, (MOD_SEC|MOD_DAL), "unit=%d", unit);

    RT_INIT_REENTRY_CHK(sec_init[unit]);
    sec_init[unit] = INIT_NOT_COMPLETED;

    /* create semaphore */
    sec_sem[unit] = osal_sem_mutex_create();
    if (0 == sec_sem[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_SEC|MOD_DAL), "semaphore create failed");
        return RT_ERR_FAILED;
    }

    sec_init[unit] = INIT_COMPLETED;

    return RT_ERR_OK;
} /* end of dal_maple_sec_init */


/* Module Name    : Security          */
/* Sub-module Name: Attack prevention */

/* Function Name:
 *      dal_maple_sec_portAttackPrevent_get
 * Description:
 *      Get action for each kind of attack on specified port.
 * Input:
 *      unit        - unit id
 *      port        - port id
 *      attack_type - type of attack
 * Output:
 *      pAction     - pointer to action for attack
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Type of attack is as following:
 *      - ARP_INVALID
 *      - GRATUITOUS_ARP
 *
 *      Action is as following:
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_COPY2CPU for GRATUITOUS_ARP
 */
int32
dal_maple_sec_portAttackPrevent_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_sec_attackType_t    attack_type,
    rtk_action_t            *pAction)
{
    int32   ret;
    uint32  value;
    uint32  reg_idx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SEC), "unit=%d, port=%d, attack_type=%d", unit, port, attack_type);

    /* check Init status */
    RT_INIT_CHK(sec_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((attack_type >= ATTACK_TYPE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    /* chip's value translate */
    switch (attack_type)
    {
        case ARP_INVALID:
            reg_idx = MAPLE_ATK_PRVNT_ARP_INVLD_PORT_ACTr;
            break;
        case GRATUITOUS_ARP:
            reg_idx = MAPLE_ATK_PRVNT_ARP_GRAT_PORT_ACTr;
            break;
        default:
            return RT_ERR_INPUT;
    }

    SEC_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_array_field_read(unit, reg_idx, port, REG_ARRAY_INDEX_NONE, MAPLE_ACTf, &value)) != RT_ERR_OK)
    {
        SEC_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
        return ret;
    }

    switch (value)
    {
        case 0:
            *pAction = ACTION_FORWARD;
            break;
        case 1:
            *pAction = ACTION_DROP;
            break;
        case 2:
            *pAction = ACTION_TRAP2CPU;
            break;
        case 3:
            if(ARP_INVALID == attack_type)
            {
                SEC_SEM_UNLOCK(unit);
                return RT_ERR_FAILED;
            }
            else
            {
                *pAction = ACTION_COPY2CPU;
            }
            break;
        default:
            SEC_SEM_UNLOCK(unit);
            return RT_ERR_FAILED;
    }

    SEC_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SEC), "pAction=%d", *pAction);

    return RT_ERR_OK;
} /* end of dal_maple_sec_portAttackPrevent_get */

/* Function Name:
 *      dal_maple_sec_portAttackPrevent_set
 * Description:
 *      Set action for each kind of attack on specified port.
 * Input:
 *      unit        - unit id
 *      port        - port id
 *      attack_type - type of attack
 *      action      - action for attack
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_PORT_ID    - invalid port id
 *      RT_ERR_FWD_ACTION - invalid forwarding action
 *      RT_ERR_INPUT      - invalid input parameter
 * Note:
 *      Type of attack is as following:
 *      - ARP_INVALID
 *      - GRATUITOUS_ARP
 *
 *      Action is as following:
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_COPY2CPU for GRATUITOUS_ARP
 */
int32
dal_maple_sec_portAttackPrevent_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_sec_attackType_t    attack_type,
    rtk_action_t            action)
{
    int32   ret;
    uint32  value;
    uint32  reg_idx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SEC), "unit=%d, port=%d, attack_type=%d, action=%d",
           unit, port, attack_type, action);

    /* check Init status */
    RT_INIT_CHK(sec_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((attack_type >= ATTACK_TYPE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((action > ACTION_COPY2CPU), RT_ERR_INPUT);

    /* chip's value translate */
    switch (attack_type)
    {
        case ARP_INVALID:
            reg_idx = MAPLE_ATK_PRVNT_ARP_INVLD_PORT_ACTr;
            break;
        case GRATUITOUS_ARP:
            reg_idx = MAPLE_ATK_PRVNT_ARP_GRAT_PORT_ACTr;
            break;
        default:
            return RT_ERR_INPUT;
    }

    switch (action)
    {
        case ACTION_FORWARD:
            value = 0;
            break;
        case ACTION_DROP:
            value = 1;
            break;
        case ACTION_TRAP2CPU:
            value = 2;
            break;
        case ACTION_COPY2CPU:
            if (ARP_INVALID == attack_type)
                return RT_ERR_INPUT;
            else
                value = 3;
            break;
        default:
            return RT_ERR_INPUT;
    }

    SEC_SEM_LOCK(unit);

    if ((ret = reg_array_field_write(unit, reg_idx, port, REG_ARRAY_INDEX_NONE, MAPLE_ACTf, &value)) != RT_ERR_OK)
    {
        SEC_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
        return ret;
    }

    SEC_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_sec_portAttackPrevent_set */

/* Module Name    : Security          */
/* Sub-module Name: Attack prevention */

/* Function Name:
 *      dal_maple_sec_portAttackPreventEnable_get
 * Description:
 *      Get the attack prevention status of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to the status of the attack prevention
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_maple_sec_portAttackPreventEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SEC), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(sec_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    SEC_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_array_field_read(unit, MAPLE_ATK_PRVNT_PORT_ENr, port, REG_ARRAY_INDEX_NONE, MAPLE_ENf, pEnable)) != RT_ERR_OK)
    {
        SEC_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
        return ret;
    }
    SEC_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SEC), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
} /* end of dal_maple_sec_portAttackPreventEnable_get */

/* Function Name:
 *      dal_maple_sec_portAttackPreventEnable_set
 * Description:
 *      Set the attack prevention status of the specific port
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - pointer to the status of the attack prevention
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
dal_maple_sec_portAttackPreventEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SEC), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(sec_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    SEC_SEM_LOCK(unit);

    /* program value to CHIP */
    if ((ret = reg_array_field_write(unit, MAPLE_ATK_PRVNT_PORT_ENr, port, REG_ARRAY_INDEX_NONE, MAPLE_ENf, &enable)) != RT_ERR_OK)
    {
        SEC_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
        return ret;
    }
    SEC_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_sec_portAttackPreventEnable_set */

/* Function Name:
 *      dal_maple_sec_attackPreventAction_get
 * Description:
 *      Get action for each kind of attack.
 * Input:
 *      unit        - unit id
 *      attack_type - type of attack
 * Output:
 *      pAction     - pointer to action for attack
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Type of attack is as following:
 *      - TCP_FRAG_OFF_MIN_CHECK
 *      - SYNRST_DENY
 *      - SYNFIN_DENY
 *      - XMAS_DENY
 *      - NULL_DENY
 *      - SYN_SPORTL1024_DENY
 *      - TCPHDR_MIN_CHECK
 *      - SMURF_DENY
 *      - ICMPV6_PING_MAX_CHECK
 *      - ICMPV4_PING_MAX_CHECK
 *      - ICMP_FRAG_PKTS_DENY
 *      - IPV6_MIN_FRAG_SIZE_CHECK
 *      - POD_DENY
 *      - TCPBLAT_DENY
 *      - UDPBLAT_DENY
 *      - LAND_DENY
 *      - DAEQSA_DENY
 *
 *      Action is as following:
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 */
int32
dal_maple_sec_attackPreventAction_get(
    uint32                  unit,
    rtk_sec_attackType_t    attack_type,
    rtk_action_t            *pAction)
{
    int32   ret;
    uint32  value;
    uint32  field_idx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SEC), "unit=%d, attack_type=%d", unit, attack_type);

    /* check Init status */
    RT_INIT_CHK(sec_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((attack_type >= ATTACK_TYPE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    /* chip's value translate */
    switch (attack_type)
    {
        case TCP_FRAG_OFF_MIN_CHECK:
            field_idx = MAPLE_TCP_FRAG_OFF_MINf;
            break;
        case SYNRST_DENY:
            field_idx = MAPLE_SYN_RSTf;
            break;
        case SYNFIN_DENY:
            field_idx = MAPLE_SYN_FINf;
            break;
        case XMA_DENY:
            field_idx = MAPLE_XMASf;
            break;
        case NULLSCAN_DENY:
            field_idx = MAPLE_NULL_SCANf;
            break;
        case SYN_SPORTL1024_DENY:
            field_idx = MAPLE_SYN_SPORT_LESS_1024f;
            break;
        case TCPHDR_MIN_CHECK:
            field_idx = MAPLE_TCP_HDR_LEN_MINf;
            break;
        case SMURF_DENY:
            field_idx = MAPLE_SMURFf;
            break;
        case ICMPV6_PING_MAX_CHECK:
            field_idx = MAPLE_ICMPV6_PING_MAXf;
            break;
        case ICMPV4_PING_MAX_CHECK:
            field_idx = MAPLE_ICMPV4_PING_MAXf;
            break;
        case ICMP_FRAG_PKTS_DENY:
            field_idx = MAPLE_ICMP_FRAG_PKTf;
            break;
        case IPV6_MIN_FRAG_SIZE_CHECK:
            field_idx = MAPLE_IPV6_FRAG_LEN_MINf;
            break;
        case POD_DENY:
            field_idx = MAPLE_PODf;
            break;
        case TCPBLAT_DENY:
            field_idx = MAPLE_TCP_BLATf;
            break;
        case UDPBLAT_DENY:
            field_idx = MAPLE_UDP_BLATf;
            break;
        case LAND_DENY:
            field_idx = MAPLE_LANDf;
            break;
        case DAEQSA_DENY:
            field_idx = MAPLE_DA_EQUAL_SAf;
            break;
        default:
            return RT_ERR_FAILED;
    }

    SEC_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_field_read(unit, MAPLE_ATK_PRVNT_CTRLr, field_idx, &value)) != RT_ERR_OK)
    {
        SEC_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
        return ret;
    }
    if (0 == value)
    {
        *pAction = ACTION_FORWARD;
    }
    else
    {
        if ((ret = reg_field_read(unit, MAPLE_ATK_PRVNT_ACTr, field_idx, &value)) != RT_ERR_OK)
        {
            SEC_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
            return ret;
        }

        if (0 == value)
        {
            *pAction = ACTION_DROP;
        }
        else
        {
            *pAction = ACTION_TRAP2CPU;
        }
    }

    SEC_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SEC), "pAction=%d", *pAction);

    return RT_ERR_OK;
} /* end of dal_maple_sec_attackPreventAction_get */

/* Function Name:
 *      dal_maple_sec_attackPreventAction_set
 * Description:
 *      Set action for each kind of attack.
 * Input:
 *      unit        - unit id
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
 *      Type of attack is as following:
 *      - TCP_FRAG_OFF_MIN_CHECK
 *      - SYNRST_DENY
 *      - SYNFIN_DENY
 *      - XMA_DENY
 *      - XMA_DENY
 *      - SYN_SPORTL1024_DENY
 *      - TCPHDR_MIN_CHECK
 *      - SMURF_DENY
 *      - ICMPV6_PING_MAX_CHECK
 *      - ICMPV4_PING_MAX_CHECK
 *      - ICMP_FRAG_PKTS_DENY
 *      - IPV6_MIN_FRAG_SIZE_CHECK
 *      - POD_DENY
 *      - TCPBLAT_DENY
 *      - UDPBLAT_DENY
 *      - LAND_DENY
 *      - DAEQSA_DENY
 *
 *      Action is as following:
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 */
int32
dal_maple_sec_attackPreventAction_set(
    uint32                  unit,
    rtk_sec_attackType_t    attack_type,
    rtk_action_t            action)
{
    int32   ret;
    uint32  value;
    uint32  field_idx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SEC), "unit=%d, attack_type=%d, action=%d",
           unit, attack_type, action);

    /* check Init status */
    RT_INIT_CHK(sec_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((attack_type >= ATTACK_TYPE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((action > ACTION_TRAP2CPU), RT_ERR_INPUT);

    /* chip's value translate */
    switch (attack_type)
    {
        case TCP_FRAG_OFF_MIN_CHECK:
            field_idx = MAPLE_TCP_FRAG_OFF_MINf;
            break;
        case SYNRST_DENY:
            field_idx = MAPLE_SYN_RSTf;
            break;
        case SYNFIN_DENY:
            field_idx = MAPLE_SYN_FINf;
            break;
        case XMA_DENY:
            field_idx = MAPLE_XMASf;
            break;
        case NULLSCAN_DENY:
            field_idx = MAPLE_NULL_SCANf;
            break;
        case SYN_SPORTL1024_DENY:
            field_idx = MAPLE_SYN_SPORT_LESS_1024f;
            break;
        case TCPHDR_MIN_CHECK:
            field_idx = MAPLE_TCP_HDR_LEN_MINf;
            break;
        case SMURF_DENY:
            field_idx = MAPLE_SMURFf;
            break;
        case ICMPV6_PING_MAX_CHECK:
            field_idx = MAPLE_ICMPV6_PING_MAXf;
            break;
        case ICMPV4_PING_MAX_CHECK:
            field_idx = MAPLE_ICMPV4_PING_MAXf;
            break;
        case ICMP_FRAG_PKTS_DENY:
            field_idx = MAPLE_ICMP_FRAG_PKTf;
            break;
        case IPV6_MIN_FRAG_SIZE_CHECK:
            field_idx = MAPLE_IPV6_FRAG_LEN_MINf;
            break;
        case POD_DENY:
            field_idx = MAPLE_PODf;
            break;
        case TCPBLAT_DENY:
            field_idx = MAPLE_TCP_BLATf;
            break;
        case UDPBLAT_DENY:
            field_idx = MAPLE_UDP_BLATf;
            break;
        case LAND_DENY:
            field_idx = MAPLE_LANDf;
            break;
        case DAEQSA_DENY:
            field_idx = MAPLE_DA_EQUAL_SAf;
            break;
        default:
            return RT_ERR_FAILED;
    }

    SEC_SEM_LOCK(unit);

    if (ACTION_FORWARD == action)
    {   /* Disable */
        value = 0;

        /* set value to CHIP */
        if ((ret = reg_field_write(unit, MAPLE_ATK_PRVNT_CTRLr, field_idx, &value)) != RT_ERR_OK)
        {
            SEC_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
            return ret;
        }
    }
    else
    {   /* Enable */
        value = 1;

        /* set value to CHIP */
        if ((ret = reg_field_write(unit, MAPLE_ATK_PRVNT_CTRLr, field_idx, &value)) != RT_ERR_OK)
        {
            SEC_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
            return ret;
        }

        if (ACTION_DROP == action)
        {
            value = 0;
        }
        else
        {
            value = 1;
        }

        /* set value to CHIP */
        if ((ret = reg_field_write(unit, MAPLE_ATK_PRVNT_ACTr, field_idx, &value)) != RT_ERR_OK)
        {
            SEC_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
            return ret;
        }

    }

    SEC_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}/* end of dal_maple_sec_attackPreventAction_set */

/* Function Name:
 *      dal_maple_sec_minIPv6FragLen_get
 * Description:
 *      Get minimum length of IPv6 fragments.
 * Input:
 *      unit    - unit id
 * Output:
 *      pLength - pointer to minimum length of IPv6 fragments
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_maple_sec_minIPv6FragLen_get(uint32 unit, uint32 *pLength)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SEC), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(sec_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pLength), RT_ERR_NULL_POINTER);

    SEC_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_field_read(unit, MAPLE_ATK_PRVNT_IPV6_CTRLr, MAPLE_FRAG_LEN_MINf, pLength)) != RT_ERR_OK)
    {
        SEC_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
        return ret;
    }
    SEC_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SEC), "pLength=%d", *pLength);

    return RT_ERR_OK;
}/* end of dal_maple_sec_minIPv6FragLen_get */

/* Function Name:
 *      dal_maple_sec_minIPv6FragLen_set
 * Description:
 *      Set minimum length of IPv6 fragments.
 * Input:
 *      unit   - unit id
 *      length - minimum length of IPv6 fragments
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      None
 */
int32
dal_maple_sec_minIPv6FragLen_set(uint32 unit, uint32 length)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SEC), "unit=%d, length=%d", unit, length);

    /* check Init status */
    RT_INIT_CHK(sec_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((length > HAL_SEC_MINIPV6FRAGLEN_MAX(unit)), RT_ERR_OUT_OF_RANGE);

    SEC_SEM_LOCK(unit);

    /* program value to CHIP */
    if ((ret = reg_field_write(unit, MAPLE_ATK_PRVNT_IPV6_CTRLr, MAPLE_FRAG_LEN_MINf, &length)) != RT_ERR_OK)
    {
        SEC_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
        return ret;
    }
    SEC_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}/* end of dal_maple_sec_minIPv6FragLen_set */

/* Function Name:
 *      dal_maple_sec_maxPingLen_get
 * Description:
 *      Get maximum length of ICMP packet.
 * Input:
 *      unit    - unit id
 * Output:
 *      pLength - pointer to maximum length of ICMP packet
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_maple_sec_maxPingLen_get(uint32 unit, uint32 *pLength)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SEC), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(sec_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pLength), RT_ERR_NULL_POINTER);

    SEC_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_field_read(unit, MAPLE_ATK_PRVNT_ICMP_CTRLr, MAPLE_PKT_LEN_MAXf, pLength)) != RT_ERR_OK)
    {
        SEC_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
        return ret;
    }
    SEC_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SEC), "pLength=%d", *pLength);

    return RT_ERR_OK;
} /* end of dal_maple_sec_maxPingLen_get */

/* Function Name:
 *      dal_maple_sec_maxPingLen_set
 * Description:
 *      Set maximum length of ICMP packet.
 * Input:
 *      unit   - unit id
 *      length - maximum length of ICMP packet
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      None
 */
int32
dal_maple_sec_maxPingLen_set(uint32 unit, uint32 length)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SEC), "unit=%d, length=%d", unit, length);

    /* check Init status */
    RT_INIT_CHK(sec_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((length > HAL_SEC_MAXPINGLEN_MAX(unit)), RT_ERR_OUT_OF_RANGE);

    SEC_SEM_LOCK(unit);

    /* program value to CHIP */
    if ((ret = reg_field_write(unit, MAPLE_ATK_PRVNT_ICMP_CTRLr, MAPLE_PKT_LEN_MAXf, &length)) != RT_ERR_OK)
    {
        SEC_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
        return ret;
    }
    SEC_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_sec_maxPingLen_set */

/* Function Name:
 *      dal_maple_sec_minTCPHdrLen_get
 * Description:
 *      Get minimum length of TCP header.
 * Input:
 *      unit    - unit id
 * Output:
 *      pLength - pointer to minimum length of TCP header
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_maple_sec_minTCPHdrLen_get(uint32 unit, uint32 *pLength)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SEC), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(sec_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pLength), RT_ERR_NULL_POINTER);

    SEC_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_field_read(unit, MAPLE_ATK_PRVNT_TCP_CTRLr, MAPLE_HDR_LEN_MINf, pLength)) != RT_ERR_OK)
    {
        SEC_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
        return ret;
    }
    SEC_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SEC), "pLength=%d", *pLength);

    return RT_ERR_OK;
} /* end of dal_maple_sec_minTCPHdrLen_get */

/* Function Name:
 *      dal_maple_sec_minTCPHdrLen_set
 * Description:
 *      Set minimum length of TCP header.
 * Input:
 *      unit   - unit id
 *      length - minimum length of TCP header
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      None
 */
int32
dal_maple_sec_minTCPHdrLen_set(uint32 unit, uint32 length)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SEC), "unit=%d, length=%d", unit, length);

    /* check Init status */
    RT_INIT_CHK(sec_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((length > 0x1f), RT_ERR_OUT_OF_RANGE);

    SEC_SEM_LOCK(unit);

    /* program value to CHIP */
    if ((ret = reg_field_write(unit, MAPLE_ATK_PRVNT_TCP_CTRLr, MAPLE_HDR_LEN_MINf, &length)) != RT_ERR_OK)
    {
        SEC_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
        return ret;
    }
    SEC_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_sec_minTCPHdrLen_set */

/* Function Name:
 *      dal_maple_sec_smurfNetmaskLen_get
 * Description:
 *      Get netmask length for preventing SMURF attack.
 * Input:
 *      unit    - unit id
 * Output:
 *      pLength - pointer to netmask length
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_maple_sec_smurfNetmaskLen_get(uint32 unit, uint32 *pLength)
{
    int32   ret;
    uint32  netmask, bitcount;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SEC), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(sec_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pLength), RT_ERR_NULL_POINTER);

    SEC_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_field_read(unit, MAPLE_ATK_PRVNT_SMURF_CTRLr, MAPLE_NETMASKf, &netmask)) != RT_ERR_OK)
    {
        SEC_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
        return ret;
    }
    SEC_SEM_UNLOCK(unit);

    for (bitcount = 0; bitcount < 32; bitcount++)
    {
        if ((netmask & (0x1 << (31 - bitcount))) == 0)
        {
            break;
        }
    }

    *pLength = bitcount;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SEC), "pLength=%d", *pLength);

    return RT_ERR_OK;
}/* end of dal_maple_sec_smurfNetmaskLen_get */

/* Function Name:
 *      dal_maple_sec_smurfNetmaskLen_set
 * Description:
 *      Set netmask length for preventing SMURF attack.
 * Input:
 *      unit   - unit id
 *      length - netmask length
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      None
 */
int32
dal_maple_sec_smurfNetmaskLen_set(uint32 unit, uint32 length)
{
    int32   ret;
    uint32  netmask = (length == 32)? 0xFFFFFFFF : (0xFFFFFFFF ^ (0xFFFFFFFF >> (length % 32)));

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SEC), "unit=%d, length=%d", unit, length);

    /* check Init status */
    RT_INIT_CHK(sec_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((length > HAL_SEC_SMURFNETMASKLEN_MAX(unit)), RT_ERR_OUT_OF_RANGE);

    SEC_SEM_LOCK(unit);

    /* program value to CHIP */
    if ((ret = reg_field_write(unit, MAPLE_ATK_PRVNT_SMURF_CTRLr, MAPLE_NETMASKf, &netmask)) != RT_ERR_OK)
    {
        SEC_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
        return ret;
    }
    SEC_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_sec_smurfNetmaskLen_set */
