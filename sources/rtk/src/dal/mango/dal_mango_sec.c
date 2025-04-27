/*
 * Copyright(c) Realtek Semiconductor Corporation, 2014
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
#include <hal/chipdef/mango/rtk_mango_table_struct.h>
#include <hal/chipdef/mango/rtk_mango_reg_struct.h>
#include <hal/mac/reg.h>
#include <hal/mac/mem.h>
#include <hal/common/halctrl.h>
#include <dal/mango/dal_mango_sec.h>
#include <rtk/default.h>
#include <rtk/sec.h>

/*
 * Symbol Definition
 */
#define DAL_MANGO_IPMACBIND_ENTRY_SRAM_HASH_IDX_MAX (256)
#define DAL_MANGO_IPMACBIND_ENTRY_SRAM_BASIC_WIDTH  (4)
#define DAL_MANGO_IPMACBIND_ENTRY_SRAM_DYN_WIDTH    (8)
#define DAL_MANGO_IPMACBIND_ENTRY_DYN_TBL_IDX_BASE  (1024)
#define DAL_MANGO_IPMACBIND_ENTRY_DYN_TBL_SIZE      (2048)
#define DAL_MANGO_IPMACBIND_ENTRY_BCAM_IDX_BASE     (4096)
#define DAL_MANGO_IPMACBIND_ENTRY_BCAM_TBL_SIZE     (64)


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

#define SEC_REG_FIELD_READ_ERR_HDL(_unit, _reg, _field, _val, _errMsg, _gotoErrLbl, _ret)   \
do {                                                                                    \
    if ((_ret = reg_field_read(_unit, _reg, _field, &_val)) != RT_ERR_OK)               \
    {                                                                                   \
        RT_ERR(_ret, (MOD_SEC|MOD_DAL), _errMsg);                                       \
        goto _gotoErrLbl;                                                               \
    }                                                                                   \
} while(0)
#define SEC_REG_FIELD_WRITE_ERR_HDL(_unit, _reg, _field, _val, _errMsg, _gotoErrLbl, _ret)  \
do {                                                                                    \
    if ((_ret = reg_field_write(_unit, _reg, _field, &_val)) != RT_ERR_OK)              \
    {                                                                                   \
        RT_ERR(_ret, (MOD_SEC|MOD_DAL), _errMsg);                                        \
        goto _gotoErrLbl;                                                               \
    }                                                                                   \
} while(0)
#define SEC_REG_ARRAY_FIELD_READ_ERR_HDL(_unit, _reg, _idx1, _idx2, _field, _val, _errMsg, _gotoErrLbl, _ret)   \
do {                                                                                                        \
    if ((_ret = reg_array_field_read(_unit, _reg, _idx1, _idx2, _field, &_val)) != RT_ERR_OK)               \
    {                                                                                                       \
        RT_ERR(_ret, (MOD_SEC|MOD_DAL), _errMsg);                                                           \
        goto _gotoErrLbl;                                                                                   \
    }                                                                                                       \
} while(0)
#define SEC_REG_ARRAY_FIELD_WRITE_ERR_HDL(_unit, _reg, _idx1, _idx2, _field, _val, _errMsg, _gotoErrLbl, _ret)  \
do {                                                                                                        \
    if ((_ret = reg_array_field_write(_unit, _reg, _idx1, _idx2, _field, &_val)) != RT_ERR_OK)              \
    {                                                                                                       \
        RT_ERR(_ret, (MOD_SEC|MOD_DAL), _errMsg);                                                           \
        goto _gotoErrLbl;                                                                                   \
    }                                                                                                       \
} while(0)
#define SEC_TABLE_READ_ERR_HDL(_unit, _tbl, _idx, _entry, _errMsg, _gotoErrLbl, _ret)   \
do {                                                                                \
    if ((_ret = table_read(_unit, _tbl, _idx, (uint32 *)&_entry)) != RT_ERR_OK)     \
    {                                                                               \
        RT_ERR(_ret, (MOD_SEC|MOD_DAL), _errMsg);                                   \
        goto _gotoErrLbl;                                                           \
    }                                                                               \
} while(0)
#define SEC_TABLE_WRITE_ERR_HDL(_unit, _tbl, _idx, _entry, _errMsg, _gotoErrLbl, _ret)  \
do {                                                                                \
    if ((_ret = table_write(_unit, _tbl, _idx, (uint32 *)&_entry)) != RT_ERR_OK)    \
    {                                                                               \
        RT_ERR(_ret, (MOD_SEC|MOD_DAL), _errMsg);                                   \
        goto _gotoErrLbl;                                                           \
    }                                                                               \
} while(0)
#define SEC_TABLE_FIELD_GET_ERR_HDL(_unit, _tbl, _field, _val, _entry, _errMsg, _gotoErrLbl, _ret)  \
do {                                                                                                    \
    if ((_ret = table_field_get(_unit, _tbl, _field, (uint32 *)&_val, (uint32 *)&_entry)) != RT_ERR_OK) \
    {                                                                                                   \
        RT_ERR(_ret, (MOD_SEC|MOD_DAL), _errMsg);                                                       \
        goto _gotoErrLbl;                                                                               \
    }                                                                                                   \
} while(0)
#define SEC_TABLE_FIELD_SET_ERR_HDL(_unit, _tbl, _field, _val, _entry, _errMsg, _gotoErrLbl, _ret)      \
do {                                                                                                    \
    if ((_ret = table_field_set(_unit, _tbl, _field, (uint32 *)&_val, (uint32 *)&_entry)) != RT_ERR_OK) \
    {                                                                                                   \
        RT_ERR(_ret, (MOD_SEC|MOD_DAL), _errMsg);                                                       \
        goto _gotoErrLbl;                                                                               \
    }                                                                                                   \
} while(0)
#define SEC_TABLE_FIELD_MAC_GET_ERR_HDL(_unit, _tbl, _field, _mac, _entry, _errMsg, _gotoErrLbl, _ret)  \
do {                                                                                                        \
    if ((_ret = table_field_mac_get(_unit, _tbl, _field, (uint8 *)&_mac, (uint32 *)&_entry)) != RT_ERR_OK)  \
    {                                                                                                       \
        RT_ERR(_ret, (MOD_SEC|MOD_DAL), _errMsg);                                                           \
        goto _gotoErrLbl;                                                                                   \
    }                                                                                                       \
} while(0)
#define SEC_TABLE_FIELD_MAC_SET_ERR_HDL(_unit, _tbl, _field, _mac, _entry, _errMsg, _gotoErrLbl, _ret)  \
do {                                                                                                        \
    if ((_ret = table_field_mac_set(_unit, _tbl, _field, (uint8 *)&_mac, (uint32 *)&_entry)) != RT_ERR_OK)  \
    {                                                                                                       \
        RT_ERR(ret, (MOD_SEC|MOD_DAL), _errMsg);                                                            \
        goto _gotoErrLbl;                                                                                   \
    }                                                                                                       \
} while(0)


/*
 * Function Declaration
 */
static int32 _dal_mango_getHashIndex(uint32 *pHash, rtk_sec_ipMacBindEntry_t *pEntry);
static int32 _dal_mango_cmpIpMacBindEntry(rtk_sec_ipMacBindEntry_t *pEntry1, rtk_sec_ipMacBindEntry_t *pEntry2);
static int32 _dal_mango_getIpMacBindEntry(uint32 unit, int32 addr, uint32 *pValid, rtk_sec_ipMacBindEntry_t *pEntry);
static int32 _dal_mango_setIpMacBindEntry(uint32 unit, int32 addr, uint32 *pValid, rtk_sec_ipMacBindEntry_t *pEntry);

/* Module Name : Security */

/* Function Name:
 *      dal_mango_secMapper_init
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
dal_mango_secMapper_init(dal_mapper_t *pMapper)
{
    pMapper->sec_init = dal_mango_sec_init;
    pMapper->sec_portAttackPrevent_get = dal_mango_sec_portAttackPrevent_get;
    pMapper->sec_portAttackPrevent_set = dal_mango_sec_portAttackPrevent_set;
    pMapper->sec_portAttackPreventEnable_get = dal_mango_sec_portAttackPreventEnable_get;
    pMapper->sec_portAttackPreventEnable_set = dal_mango_sec_portAttackPreventEnable_set;
    pMapper->sec_attackPreventAction_get = dal_mango_sec_attackPreventAction_get;
    pMapper->sec_attackPreventAction_set = dal_mango_sec_attackPreventAction_set;
    pMapper->sec_minIPv6FragLen_get = dal_mango_sec_minIPv6FragLen_get;
    pMapper->sec_minIPv6FragLen_set = dal_mango_sec_minIPv6FragLen_set;
    pMapper->sec_maxPingLen_get = dal_mango_sec_maxPingLen_get;
    pMapper->sec_maxPingLen_set = dal_mango_sec_maxPingLen_set;
    pMapper->sec_minTCPHdrLen_get = dal_mango_sec_minTCPHdrLen_get;
    pMapper->sec_minTCPHdrLen_set = dal_mango_sec_minTCPHdrLen_set;
    pMapper->sec_smurfNetmaskLen_get = dal_mango_sec_smurfNetmaskLen_get;
    pMapper->sec_smurfNetmaskLen_set = dal_mango_sec_smurfNetmaskLen_set;
    pMapper->sec_trapTarget_get = dal_mango_sec_trapTarget_get;
    pMapper->sec_trapTarget_set = dal_mango_sec_trapTarget_set;
    pMapper->sec_ipMacBindAction_get = dal_mango_sec_ipMacBindAction_get;
    pMapper->sec_ipMacBindAction_set = dal_mango_sec_ipMacBindAction_set;
    pMapper->sec_portIpMacBindEnable_get = dal_mango_sec_portIpMacBindEnable_get;
    pMapper->sec_portIpMacBindEnable_set = dal_mango_sec_portIpMacBindEnable_set;
    pMapper->sec_ipMacBindEntry_add = dal_mango_sec_ipMacBindEntry_add;
    pMapper->sec_ipMacBindEntry_del = dal_mango_sec_ipMacBindEntry_del;
    pMapper->sec_ipMacBindEntry_getNext = dal_mango_sec_ipMacBindEntry_getNext;
    pMapper->sec_attackPreventHit_get = dal_mango_sec_attackPreventHit_get;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_sec_init
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
dal_mango_sec_init(uint32 unit)
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
} /* end of dal_mango_sec_init */

/* Module Name    : Security          */
/* Sub-module Name: Attack prevention */

/* Function Name:
 *      dal_mango_sec_portAttackPrevent_get
 * Description:
 *      Get action for each kind of attack on specified port.
 * Input:
 *      unit            - unit id
 *      port            - port id
 *      attack_type     - type of attack
 * Output:
 *      pAction         - pointer to action for attack
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      Type of attack is as following:
 *      - ARP_INVALID
 *
 *      Action is as following:
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 */
int32
dal_mango_sec_portAttackPrevent_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_sec_attackType_t    attack_type,
    rtk_action_t            *pAction)
{
    int32   ret;
    uint32  value;
    uint32  reg_idx;
    uint32  field_idx;

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
            reg_idx = MANGO_ATK_PRVNT_ARP_INVLD_PORT_ACTr;
            field_idx = MANGO_ACTf;
            break;
        default:
            return RT_ERR_INPUT;
    }

    SEC_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_array_field_read(unit, reg_idx, port,
            REG_ARRAY_INDEX_NONE, field_idx, &value)) != RT_ERR_OK)
    {
        SEC_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
        return ret;
    }
    switch (value)
    {
        default:
        case 0:
            *pAction = ACTION_FORWARD;
            break;

        case 1:
            *pAction = ACTION_DROP;
            break;

        case 2:
            *pAction = ACTION_TRAP2CPU;
            break;
    }

    SEC_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SEC), "pAction=%d", *pAction);

    return RT_ERR_OK;
} /* end of dal_mango_sec_portAttackPrevent_get */

/* Function Name:
 *      dal_mango_sec_portAttackPrevent_set
 * Description:
 *      Set action for each kind of attack on specified port.
 * Input:
 *      unit            - unit id
 *      port            - port id
 *      attack_type     - type of attack
 *      action          - action for attack
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_FWD_ACTION       - invalid forwarding action
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      Type of attack is as following:
 *      - ARP_INVALID
 *
 *      Action is as following:
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 */
int32
dal_mango_sec_portAttackPrevent_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_sec_attackType_t    attack_type,
    rtk_action_t            action)
{
    int32   ret;
    uint32  value;
    uint32  reg_idx;
    uint32  field_idx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SEC), "unit=%d, port=%d, attack_type=%d, action=%d",
           unit, port, attack_type, action);

    /* check Init status */
    RT_INIT_CHK(sec_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((attack_type >= ATTACK_TYPE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((action >= ACTION_END), RT_ERR_FWD_ACTION);

    /* chip's value translate */
    switch (attack_type)
    {
        case ARP_INVALID:
            reg_idx = MANGO_ATK_PRVNT_ARP_INVLD_PORT_ACTr;
            field_idx = MANGO_ACTf;
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

        default:
            return RT_ERR_FWD_ACTION;
    }

    SEC_SEM_LOCK(unit);

    if ((ret = reg_array_field_write(unit, reg_idx, port,
            REG_ARRAY_INDEX_NONE, field_idx, &value)) != RT_ERR_OK)
    {
        SEC_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
        return ret;
    }

    SEC_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_mango_sec_portAttackPrevent_set */

/* Function Name:
 *      dal_mango_sec_portAttackPreventEnable_get
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
 *      RT_ERR_PORT_ID       - invalid port id
 *      RT_ERR_NULL_POINTER  - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_mango_sec_portAttackPreventEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SEC), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(sec_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    SEC_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_array_field_read(unit, MANGO_ATK_PRVNT_PORT_ENr, port,
            REG_ARRAY_INDEX_NONE, MANGO_ENf, pEnable)) != RT_ERR_OK)
    {
        SEC_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
        return ret;
    }
    SEC_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SEC), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
} /* end of dal_mango_sec_portAttackPreventEnable_get */

/* Function Name:
 *      dal_mango_sec_portAttackPreventEnable_set
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
 *      RT_ERR_PORT_ID       - invalid port id
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      None
 */
int32
dal_mango_sec_portAttackPreventEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SEC), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(sec_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(enable >= RTK_ENABLE_END, RT_ERR_INPUT);

    SEC_SEM_LOCK(unit);

    /* program value to CHIP */
    if ((ret = reg_array_field_write(unit, MANGO_ATK_PRVNT_PORT_ENr, port,
            REG_ARRAY_INDEX_NONE, MANGO_ENf, &enable)) != RT_ERR_OK)
    {
        SEC_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
        return ret;
    }
    SEC_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_mango_sec_portAttackPreventEnable_set */


/* Function Name:
 *      dal_mango_sec_attackPreventAction_get
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
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Type of attack is as following:
 *      - TCP_FRAG_OFF_MIN_CHECK
 *      - SYNRST_DENY
 *      - SYNFIN_DENY
 *      - XMA_DENY
 *      - NULLSCAN_DENY
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
 *      - IPV4_INVALID_HDR
 *
 *      Action is as following:
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 */
int32
dal_mango_sec_attackPreventAction_get(
    uint32                      unit,
    rtk_sec_attackType_t        attack_type,
    rtk_action_t                *pAction)
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
            field_idx = MANGO_TCP_FRAG_OFF_MINf;
            break;
        case SYNRST_DENY:
            field_idx = MANGO_SYN_RSTf;
            break;
        case SYNFIN_DENY:
            field_idx = MANGO_SYN_FINf;
            break;
        case XMA_DENY:
            field_idx = MANGO_XMASf;
            break;
        case NULLSCAN_DENY:
            field_idx = MANGO_NULL_SCANf;
            break;
        case SYN_SPORTL1024_DENY:
            field_idx = MANGO_SYN_SPORT_LESS_1024f;
            break;
        case TCPHDR_MIN_CHECK:
            field_idx = MANGO_TCP_HDR_LEN_MINf;
            break;
        case SMURF_DENY:
            field_idx = MANGO_SMURFf;
            break;
        case ICMPV6_PING_MAX_CHECK:
            field_idx = MANGO_ICMPV6_PING_MAXf;
            break;
        case ICMPV4_PING_MAX_CHECK:
            field_idx = MANGO_ICMPV4_PING_MAXf;
            break;
        case ICMP_FRAG_PKTS_DENY:
            field_idx = MANGO_ICMP_FRAG_PKTf;
            break;
        case IPV6_MIN_FRAG_SIZE_CHECK:
            field_idx = MANGO_IPV6_FRAG_LEN_MINf;
            break;
        case POD_DENY:
            field_idx = MANGO_PODf;
            break;
        case TCPBLAT_DENY:
            field_idx = MANGO_TCP_BLATf;
            break;
        case UDPBLAT_DENY:
            field_idx = MANGO_UDP_BLATf;
            break;
        case LAND_DENY:
            field_idx = MANGO_LANDf;
            break;
        case DAEQSA_DENY:
            field_idx = MANGO_DA_EQUAL_SAf;
            break;
        case IPV4_INVALID_HDR:
            field_idx = MANGO_INVALID_HEADERf;
            break;
        default:
            return RT_ERR_INPUT;
    }

    SEC_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_field_read(unit, MANGO_ATK_PRVNT_CTRLr,
            field_idx, &value)) != RT_ERR_OK)
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
        if ((ret = reg_field_read(unit, MANGO_ATK_PRVNT_ACTr,
                field_idx, &value)) != RT_ERR_OK)
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
} /* end of dal_mango_sec_attackPreventAction_get */

/* Function Name:
 *      dal_mango_sec_attackPreventAction_set
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
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_FWD_ACTION       - invalid forwarding action
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      Type of attack is as following:
 *      - TCP_FRAG_OFF_MIN_CHECK
 *      - SYNRST_DENY
 *      - SYNFIN_DENY
 *      - XMA_DENY
 *      - NULLSCAN_DENY
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
 *      - IPV4_INVALID_HDR
 *
 *      Action is as following:
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 */
int32
dal_mango_sec_attackPreventAction_set(
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
    RT_PARAM_CHK((action >= ACTION_END), RT_ERR_INPUT);

    /* chip's value translate */
    switch (attack_type)
    {
        case TCP_FRAG_OFF_MIN_CHECK:
            field_idx = MANGO_TCP_FRAG_OFF_MINf;
            break;
        case SYNRST_DENY:
            field_idx = MANGO_SYN_RSTf;
            break;
        case SYNFIN_DENY:
            field_idx = MANGO_SYN_FINf;
            break;
        case XMA_DENY:
            field_idx = MANGO_XMASf;
            break;
        case NULLSCAN_DENY:
            field_idx = MANGO_NULL_SCANf;
            break;
        case SYN_SPORTL1024_DENY:
            field_idx = MANGO_SYN_SPORT_LESS_1024f;
            break;
        case TCPHDR_MIN_CHECK:
            field_idx = MANGO_TCP_HDR_LEN_MINf;
            break;
        case SMURF_DENY:
            field_idx = MANGO_SMURFf;
            break;
        case ICMPV6_PING_MAX_CHECK:
            field_idx = MANGO_ICMPV6_PING_MAXf;
            break;
        case ICMPV4_PING_MAX_CHECK:
            field_idx = MANGO_ICMPV4_PING_MAXf;
            break;
        case ICMP_FRAG_PKTS_DENY:
            field_idx = MANGO_ICMP_FRAG_PKTf;
            break;
        case IPV6_MIN_FRAG_SIZE_CHECK:
            field_idx = MANGO_IPV6_FRAG_LEN_MINf;
            break;
        case POD_DENY:
            field_idx = MANGO_PODf;
            break;
        case TCPBLAT_DENY:
            field_idx = MANGO_TCP_BLATf;
            break;
        case UDPBLAT_DENY:
            field_idx = MANGO_UDP_BLATf;
            break;
        case LAND_DENY:
            field_idx = MANGO_LANDf;
            break;
        case DAEQSA_DENY:
            field_idx = MANGO_DA_EQUAL_SAf;
            break;
        case IPV4_INVALID_HDR:
            field_idx = MANGO_INVALID_HEADERf;
            break;
        default:
            return RT_ERR_INPUT;
    }

    SEC_SEM_LOCK(unit);

    if (ACTION_FORWARD == action)
    {   /* Disable */
        value = 0;

        /* set value to CHIP */
        if ((ret = reg_field_write(unit, MANGO_ATK_PRVNT_CTRLr,
                field_idx, &value)) != RT_ERR_OK)
        {
            SEC_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
            return ret;
        }
    }
    else if (ACTION_DROP == action || ACTION_TRAP2CPU == action)
    {   /* Enable */
        value = 1;

        /* set value to CHIP */
        if ((ret = reg_field_write(unit, MANGO_ATK_PRVNT_CTRLr,
                field_idx, &value)) != RT_ERR_OK)
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
        if ((ret = reg_field_write(unit, MANGO_ATK_PRVNT_ACTr,
                field_idx, &value)) != RT_ERR_OK)
        {
            SEC_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
            return ret;
        }

    }
    else
    {
        SEC_SEM_UNLOCK(unit);
        return RT_ERR_INPUT;
    }

    SEC_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_mango_sec_attackPreventAction_set */

/* Function Name:
 *      dal_mango_sec_minIPv6FragLen_get
 * Description:
 *      Get minimum length of IPv6 fragments.
 * Input:
 *      unit    - unit id
 * Output:
 *      pLength - pointer to minimum length of IPv6 fragments
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_mango_sec_minIPv6FragLen_get(uint32 unit, uint32 *pLength)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SEC), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(sec_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pLength), RT_ERR_NULL_POINTER);

    SEC_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_field_read(unit, MANGO_ATK_PRVNT_IPV6_CTRLr,
            MANGO_FRAG_LEN_MINf, pLength)) != RT_ERR_OK)
    {
        SEC_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
        return ret;
    }

    SEC_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SEC), "pLength=%d", *pLength);

    return RT_ERR_OK;
} /* end of dal_mango_sec_minIPv6FragLen_get */

/* Function Name:
 *      dal_mango_sec_minIPv6FragLen_set
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
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_OUT_OF_RANGE     - input parameter out of range
 * Note:
 *      None
 */
int32
dal_mango_sec_minIPv6FragLen_set(uint32 unit, uint32 length)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SEC), "unit=%d, length=%d", unit, length);

    /* check Init status */
    RT_INIT_CHK(sec_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((length > HAL_SEC_MINIPV6FRAGLEN_MAX(unit)), RT_ERR_OUT_OF_RANGE);

    SEC_SEM_LOCK(unit);

    /* program value to CHIP */
    if ((ret = reg_field_write(unit, MANGO_ATK_PRVNT_IPV6_CTRLr,
            MANGO_FRAG_LEN_MINf, &length)) != RT_ERR_OK)
    {
        SEC_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
        return ret;
    }

    SEC_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_mango_sec_minIPv6FragLen_set */

/* Function Name:
 *      dal_mango_sec_maxPingLen_get
 * Description:
 *      Get maximum length of ICMP packet.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pLength - pointer to maximum length of ICMP packet
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_mango_sec_maxPingLen_get(uint32 unit, uint32 *pLength)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SEC), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(sec_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pLength), RT_ERR_NULL_POINTER);

    SEC_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_field_read(unit, MANGO_ATK_PRVNT_ICMP_CTRLr,
            MANGO_PKT_LEN_MAXf, pLength)) != RT_ERR_OK)
    {
        SEC_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
        return ret;
    }

    SEC_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SEC), "pLength=%d", *pLength);

    return RT_ERR_OK;
} /* end of dal_mango_sec_maxPingLen_get */

/* Function Name:
 *      dal_mango_sec_maxPingLen_set
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
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_OUT_OF_RANGE     - input parameter out of range
 * Note:
 *      None
 */
int32
dal_mango_sec_maxPingLen_set(uint32 unit, uint32 length)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SEC), "unit=%d, length=%d", unit, length);

    /* check Init status */
    RT_INIT_CHK(sec_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((length > HAL_SEC_MAXPINGLEN_MAX(unit)), RT_ERR_OUT_OF_RANGE);

    SEC_SEM_LOCK(unit);

    /* program value to CHIP */
    if ((ret = reg_field_write(unit, MANGO_ATK_PRVNT_ICMP_CTRLr,
            MANGO_PKT_LEN_MAXf, &length)) != RT_ERR_OK)
    {
        SEC_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
        return ret;
    }

    SEC_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_mango_sec_maxPingLen_set */

/* Function Name:
 *      dal_mango_sec_minTCPHdrLen_get
 * Description:
 *      Get minimum length of TCP header.
 * Input:
 *      unit    - unit id
 * Output:
 *      pLength - pointer to minimum length of TCP header
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_mango_sec_minTCPHdrLen_get(uint32 unit, uint32 *pLength)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SEC), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(sec_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pLength), RT_ERR_NULL_POINTER);

    SEC_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_field_read(unit, MANGO_ATK_PRVNT_TCP_CTRLr,
            MANGO_HDR_LEN_MINf, pLength)) != RT_ERR_OK)
    {
        SEC_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
        return ret;
    }

    SEC_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SEC), "pLength=%d", *pLength);

    return RT_ERR_OK;
} /* end of dal_mango_sec_minTCPHdrLen_get */

/* Function Name:
 *      dal_mango_sec_minTCPHdrLen_set
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
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_OUT_OF_RANGE     - input parameter out of range
 * Note:
 *      None
 */
int32
dal_mango_sec_minTCPHdrLen_set(uint32 unit, uint32 length)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SEC), "unit=%d, length=%d", unit, length);

    /* check Init status */
    RT_INIT_CHK(sec_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((length > 0x1f), RT_ERR_OUT_OF_RANGE);

    SEC_SEM_LOCK(unit);

    /* program value to CHIP */
    if ((ret = reg_field_write(unit, MANGO_ATK_PRVNT_TCP_CTRLr,
            MANGO_HDR_LEN_MINf, &length)) != RT_ERR_OK)
    {
        SEC_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
        return ret;
    }

    SEC_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_mango_sec_minTCPHdrLen_set */

/* Function Name:
 *      dal_mango_sec_smurfNetmaskLen_get
 * Description:
 *      Get netmask length for preventing SMURF attack.
 * Input:
 *      unit    - unit id
 * Output:
 *      pLength - pointer to netmask length
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_mango_sec_smurfNetmaskLen_get(uint32 unit, uint32 *pLength)
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
    if ((ret = reg_field_read(unit, MANGO_ATK_PRVNT_SMURF_CTRLr,
            MANGO_NETMASKf, &netmask)) != RT_ERR_OK)
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
} /* end of dal_mango_sec_smurfNetmaskLen_get */

/* Function Name:
 *      dal_mango_sec_smurfNetmaskLen_set
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
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_OUT_OF_RANGE     - input parameter out of range
 * Note:
 *      None
 */
int32
dal_mango_sec_smurfNetmaskLen_set(uint32 unit, uint32 length)
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
    if ((ret = reg_field_write(unit, MANGO_ATK_PRVNT_SMURF_CTRLr,
            MANGO_NETMASKf, &netmask)) != RT_ERR_OK)
    {
        SEC_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
        return ret;
    }

    SEC_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_mango_sec_smurfNetmaskLen_set */

/* Function Name:
 *      dal_mango_sec_trapTarget_get
 * Description:
 *      Get information of attack trap packet to local or master CPU.
 * Input:
 *      unit    - unit id
 * Output:
 *      pTarget - pointer to the information of attack trap packet
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Note:
 */
int32
dal_mango_sec_trapTarget_get(uint32 unit, rtk_trapTarget_t *pTarget)
{
    uint32  val;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SEC), "unit=%d",unit);

    /* check Init status */
    RT_INIT_CHK(sec_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pTarget), RT_ERR_NULL_POINTER);

    /* function body */
    SEC_SEM_LOCK(unit);

    ret = reg_field_read(unit, MANGO_ATK_PRVNT_CTRLr, MANGO_CPU_SELf, &val);
    if (ret != RT_ERR_OK)
    {
        SEC_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "field read fail");
        return ret;
    }

    if (0 == val)
    {
        *pTarget = RTK_TRAP_LOCAL;
    }
    else
    {
        *pTarget = RTK_TRAP_MASTER;
    }

    SEC_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_sec_trapTarget_get */

/* Function Name:
 *      dal_mango_sec_trapTarget_set
 * Description:
 *      Set information of attack trap packet to local or master CPU.
 * Input:
 *      unit    - unit id
 *      target  - the status of attack trap packet
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 */
int32
dal_mango_sec_trapTarget_set(uint32 unit, rtk_trapTarget_t target)
{
    uint32  val;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SEC), "unit=%d,target=%d",unit, target);

    /* check Init status */
    RT_INIT_CHK(sec_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((RTK_TRAP_END <= target), RT_ERR_INPUT);

    /* function body */
    SEC_SEM_LOCK(unit);

    switch (target)
    {
        case RTK_TRAP_LOCAL:
            val = 0;
            break;
        case RTK_TRAP_MASTER:
            val = 1;
            break;
        default:
            SEC_SEM_UNLOCK(unit);
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_SEC), "target %d is invalid.", target);
            return RT_ERR_INPUT;
    }

    ret = reg_field_write(unit, MANGO_ATK_PRVNT_CTRLr, MANGO_CPU_SELf, &val);
    if (ret != RT_ERR_OK)
    {
        SEC_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "field write fail");
        return ret;
    }

    SEC_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_sec_trapTarget_set */

/* Function Name:
 *      dal_mango_sec_ipMacBindAction_get
 * Description:
 *      Get forwarding actions of various results of IP-MAC bind packets.
 * Input:
 *      unit         - unit id
 * Output:
 *      pLumisAct    - pointer to the forwarding action of IP-MAC bind look-up-miss packets
 *      pMatchAct    - pointer to the forwarding action of IP-MAC bind match packets
 *      pMismatchAct - pointer to the forwarding action of IP-MAC bind mismatch packets
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Note:
 */
int32
dal_mango_sec_ipMacBindAction_get(uint32 unit, rtk_action_t *pLumisAct, rtk_action_t *pMatchAct, rtk_action_t *pMismatchAct)
{
    int32   ret;
    uint32  valLumisAct, valMatchAct, valMismatchAct;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SEC), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(sec_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pLumisAct), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pMatchAct), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pMismatchAct), RT_ERR_NULL_POINTER);

    /* function body */
    SEC_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_field_read(unit, MANGO_SEC_IP_MAC_BIND_CTRLr, MANGO_LU_MIS_ACTf, &valLumisAct) ) != RT_ERR_OK)
    {
        SEC_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
        return ret;
    }
    if ((ret = reg_field_read(unit, MANGO_SEC_IP_MAC_BIND_CTRLr, MANGO_MATCH_ACTf, &valMatchAct) ) != RT_ERR_OK)
    {
        SEC_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
        return ret;
    }
    if ((ret = reg_field_read(unit, MANGO_SEC_IP_MAC_BIND_CTRLr, MANGO_MISMATCH_ACTf, &valMismatchAct) ) != RT_ERR_OK)
    {
        SEC_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
        return ret;
    }

    SEC_SEM_UNLOCK(unit);

    /* lumisAction */
    if (0 == valLumisAct)
        *pLumisAct = ACTION_FORWARD;
    else if(1 == valLumisAct)
        *pLumisAct = ACTION_DROP;
    else if(2 == valLumisAct)
        *pLumisAct = ACTION_TRAP2CPU;
    else if(3 == valLumisAct)
        *pLumisAct = ACTION_COPY2CPU;
    else if(4 == valLumisAct)
        *pLumisAct = ACTION_TRAP2MASTERCPU;
    else if(5 == valLumisAct)
        *pLumisAct = ACTION_COPY2MASTERCPU;
    else
        return RT_ERR_FAILED;

    /* matchAction */
    if (0 == valMatchAct)
        *pMatchAct = ACTION_FORWARD;
    else if(1 == valMatchAct)
        *pMatchAct = ACTION_DROP;
    else if(2 == valMatchAct)
        *pMatchAct = ACTION_TRAP2CPU;
    else if(3 == valMatchAct)
        *pMatchAct = ACTION_COPY2CPU;
    else if(4 == valMatchAct)
        *pMatchAct = ACTION_TRAP2MASTERCPU;
    else if(5 == valMatchAct)
        *pMatchAct = ACTION_COPY2MASTERCPU;
    else
        return RT_ERR_FAILED;

    /* mismatchAction */
    if (0 == valMismatchAct)
        *pMismatchAct = ACTION_FORWARD;
    else if(1 == valMismatchAct)
        *pMismatchAct = ACTION_DROP;
    else if(2 == valMismatchAct)
        *pMismatchAct = ACTION_TRAP2CPU;
    else if(3 == valMismatchAct)
        *pMismatchAct = ACTION_COPY2CPU;
    else if(4 == valMismatchAct)
        *pMismatchAct = ACTION_TRAP2MASTERCPU;
    else if(5 == valMismatchAct)
        *pMismatchAct = ACTION_COPY2MASTERCPU;
    else
        return RT_ERR_FAILED;

    return RT_ERR_OK;
}   /* end of dal_mango_sec_ipMacBindAction_get */

/* Function Name:
 *      dal_mango_sec_ipMacBindAction_get
 * Description:
 *      Get forwarding actions of various results of IP-MAC bind packets.
 * Input:
 *      unit        - unit id
 *      lumisAct    - forwarding action of IP-MAC bind look-up-miss packets
 *      matchAct    - forwarding action of IP-MAC bind match packets
 *      mismatchAct - forwarding action of IP-MAC bind mismatch packets
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Note:
 */
int32
dal_mango_sec_ipMacBindAction_set(uint32 unit, rtk_action_t lumisAct, rtk_action_t matchAct, rtk_action_t mismatchAct)
{
    int32   ret;
    uint32  valLumisAct, valMatchAct, valMismatchAct;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SEC), "unit=%d,lumisAct=%d,matchAct=%d,mismatchAct=%d", unit, lumisAct, matchAct, mismatchAct);

    /* check Init status */
    RT_INIT_CHK(sec_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((ACTION_END <= lumisAct), RT_ERR_INPUT);
    RT_PARAM_CHK((ACTION_END <= matchAct), RT_ERR_INPUT);
    RT_PARAM_CHK((ACTION_END <= mismatchAct), RT_ERR_INPUT);

    /* lumisAction */
    if (ACTION_FORWARD == lumisAct)
        valLumisAct = 0;
    else if(ACTION_DROP == lumisAct)
        valLumisAct = 1;
    else if(ACTION_TRAP2CPU == lumisAct)
        valLumisAct = 2;
    else if(ACTION_COPY2CPU == lumisAct)
        valLumisAct = 3;
    else if(ACTION_TRAP2MASTERCPU == lumisAct)
        valLumisAct = 4;
    else if(ACTION_COPY2MASTERCPU == lumisAct)
        valLumisAct = 5;
    else
        return RT_ERR_FAILED;

    /* matchAction */
    if (ACTION_FORWARD == matchAct)
        valMatchAct = 0;
    else if(ACTION_DROP == matchAct)
        valMatchAct = 1;
    else if(ACTION_TRAP2CPU == matchAct)
        valMatchAct = 2;
    else if(ACTION_COPY2CPU == matchAct)
        valMatchAct = 3;
    else if(ACTION_TRAP2MASTERCPU == matchAct)
        valMatchAct = 4;
    else if(ACTION_COPY2MASTERCPU == matchAct)
        valMatchAct = 5;
    else
        return RT_ERR_FAILED;

    /* mismatchAction */
    if (ACTION_FORWARD == mismatchAct)
        valMismatchAct = 0;
    else if(ACTION_DROP == mismatchAct)
        valMismatchAct = 1;
    else if(ACTION_TRAP2CPU == mismatchAct)
        valMismatchAct = 2;
    else if(ACTION_COPY2CPU == mismatchAct)
        valMismatchAct = 3;
    else if(ACTION_TRAP2MASTERCPU == mismatchAct)
        valMismatchAct = 4;
    else if(ACTION_COPY2MASTERCPU == mismatchAct)
        valMismatchAct = 5;
    else
        return RT_ERR_FAILED;

    /* function body */
    SEC_SEM_LOCK(unit);

    /* set value to CHIP */
    if ((ret = reg_field_write(unit, MANGO_SEC_IP_MAC_BIND_CTRLr, MANGO_LU_MIS_ACTf, &valLumisAct) ) != RT_ERR_OK)
    {
        SEC_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
        return ret;
    }
    if ((ret = reg_field_write(unit, MANGO_SEC_IP_MAC_BIND_CTRLr, MANGO_MATCH_ACTf, &valMatchAct) ) != RT_ERR_OK)
    {
        SEC_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
        return ret;
    }
    if ((ret = reg_field_write(unit, MANGO_SEC_IP_MAC_BIND_CTRLr, MANGO_MISMATCH_ACTf, &valMismatchAct) ) != RT_ERR_OK)
    {
        SEC_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
        return ret;
    }

    SEC_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_sec_ipMacBindAction_set */

/* Function Name:
 *      dal_mango_sec_portIpMacBindEnable_get
 * Description:
 *      Get enable status of IP-MAC bind of the specified packet type.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      type    - packet type
 * Output:
 *      pEnable - pointer to the enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Note:
 */
int32
dal_mango_sec_portIpMacBindEnable_get(uint32 unit, rtk_port_t port, rtk_sec_ipMacBindPktType_t type, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  regField;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SEC), "unit=%d,port=%d,type=%d", unit, port, type);

    /* check Init status */
    RT_INIT_CHK(sec_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_SEC_IPMACBIND_PKTTYPE_END <= type), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if (RTK_SEC_IPMACBIND_PKTTYPE_IP == type)
        regField = MANGO_CHK_ENf;
    else if (RTK_SEC_IPMACBIND_PKTTYPE_ARP == type)
        regField = MANGO_ARP_CHK_ENf;
    else
        return RT_ERR_OK;

    /* function body */
    SEC_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_array_field_read(unit, MANGO_SEC_PORT_IP_MAC_BIND_CTRLr, port,
            REG_ARRAY_INDEX_NONE, regField, &value)) != RT_ERR_OK)
    {
        SEC_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
        return ret;
    }

    SEC_SEM_UNLOCK(unit);

    if (0 == value)
        *pEnable = DISABLED;
    else if (1 == value)
        *pEnable = ENABLED;
    else
        return RT_ERR_OK;

    return RT_ERR_OK;
}   /* end of dal_mango_sec_portIpMacBindEnable_get */

/* Function Name:
 *      dal_mango_sec_portIpMacBindEnable_set
 * Description:
 *      Set enable status of IP-MAC bind of the specified packet type.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      type   - packet type
 *      enable - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 */
int32
dal_mango_sec_portIpMacBindEnable_set(uint32 unit, rtk_port_t port, rtk_sec_ipMacBindPktType_t type, rtk_enable_t enable)
{
    int32   ret;
    uint32  regField;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SEC), "unit=%d,port=%d,type=%d,enable=%d", unit, port, type, enable);

    /* check Init status */
    RT_INIT_CHK(sec_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_SEC_IPMACBIND_PKTTYPE_END <= type), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    if (RTK_SEC_IPMACBIND_PKTTYPE_IP == type)
        regField = MANGO_CHK_ENf;
    else if (RTK_SEC_IPMACBIND_PKTTYPE_ARP == type)
        regField = MANGO_ARP_CHK_ENf;
    else
        return RT_ERR_OK;

    if (DISABLED == enable)
        value = 0;
    else if (ENABLED == enable)
        value = 1;
    else
        return RT_ERR_OK;

    /* function body */
    SEC_SEM_LOCK(unit);

    /* set value from CHIP */
    if ((ret = reg_array_field_write(unit, MANGO_SEC_PORT_IP_MAC_BIND_CTRLr, port,
            REG_ARRAY_INDEX_NONE, regField, &value)) != RT_ERR_OK)
    {
        SEC_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
        return ret;
    }

    SEC_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_sec_portIpMacBindEnable_set */

static int32 _dal_mango_getHashIndex(uint32 *pHash, rtk_sec_ipMacBindEntry_t *pEntry)
{
    /* parameter check */
    RT_PARAM_CHK((NULL == pHash), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* IP address */
    *pHash = (pEntry->ipAddr & 0x000000FF) >> 0;
    *pHash ^= (pEntry->ipAddr & 0x0000FF00) >> 8;
    *pHash ^= (pEntry->ipAddr & 0x00FF0000) >> 16;
    *pHash ^= (pEntry->ipAddr & 0xFF000000) >> 24;

#if 0   /* spec changed */
    /* MAC address */
    *pHash ^= pEntry->macAddr.octet[0];
    *pHash ^= pEntry->macAddr.octet[1];
    *pHash ^= pEntry->macAddr.octet[2];
    *pHash ^= pEntry->macAddr.octet[3];
    *pHash ^= pEntry->macAddr.octet[4];
    *pHash ^= pEntry->macAddr.octet[5];
#endif

    return RT_ERR_OK;
}

static int32 _dal_mango_cmpIpMacBindEntry(rtk_sec_ipMacBindEntry_t *pEntry1, rtk_sec_ipMacBindEntry_t *pEntry2)
{
    uint32 i;

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry1), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEntry2), RT_ERR_NULL_POINTER);

    /* port binding */
    if ((pEntry1->flags & RTK_SEC_IPMACBIND_FLAG_BIND_PORT) != (pEntry2->flags & RTK_SEC_IPMACBIND_FLAG_BIND_PORT))
        return -1;

    /* VLAN binding */
    if ((pEntry1->flags & RTK_SEC_IPMACBIND_FLAG_BIND_VLAN) != (pEntry2->flags & RTK_SEC_IPMACBIND_FLAG_BIND_VLAN))
        return -2;

    /* port type & id */
    if ((pEntry1->flags & RTK_SEC_IPMACBIND_FLAG_BIND_PORT) != 0)
    {   /* port binding is enabled */
        if ((pEntry1->flags & RTK_SEC_IPMACBIND_FLAG_PORT_TRUNK) != (pEntry2->flags & RTK_SEC_IPMACBIND_FLAG_PORT_TRUNK))
            return -3;

        if (pEntry1->flags & RTK_SEC_IPMACBIND_FLAG_PORT_TRUNK)
        {   /* trunk */
            if ((pEntry1->trunkId) != (pEntry2->trunkId))
                return -4;
        }
        else
        {   /* normal port */
            if ((pEntry1->port) != (pEntry2->port))
                return -5;
        }
    }

    /* vlan id */
    if ((pEntry1->flags & RTK_SEC_IPMACBIND_FLAG_BIND_VLAN) != 0)
    {
        if ((pEntry1->vid) != (pEntry2->vid))
            return -6;
    }

    /* IP */
    if (pEntry1->ipAddr != pEntry2->ipAddr)
        return -7;

    /* MAC */
    for (i=0; i<ETHER_ADDR_LEN; i++)
    {
        if (pEntry1->macAddr.octet[i] != pEntry2->macAddr.octet[i])
            return -8;
    }

    return 0;
}

/* Function Name:
 *      _dal_mango_getIpMacBindEntry
 * Description:
 *      Get an IP-MAC Binding entry from chip.
 * Input:
 *      unit - unit id
 *      addr - entry address
 * Output:
 *      pValid - pointer to the validation of the entry
 *      pEntry - pointer to the entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) when addr >= DAL_MANGO_IPMACBIND_ENTRY_BCAM_IDX_BASE (4096),
 *          it means to access BCAM table. otherwise, access SRAM table.
 */
static int32 _dal_mango_getIpMacBindEntry(uint32 unit, int32 addr, uint32 *pValid, rtk_sec_ipMacBindEntry_t *pEntry)
{
    int32   ret;
    ipmac_bind_entry_t ipmac_entry;
    uint32  temp_var[2];    /* the longest length is MAC address (48bits) */
    uint32  hashIdx, entrySel;
    uint32  tbl, idx;
    uint32  tfValid, tfSIP, tfSMAC, tfPortBind, tfPortType, tfPortID, tfVIDBind, tfVID;

    RT_LOG(LOG_TRACE, (MOD_SEC|MOD_DAL), "unit=%d, addr=%d", unit, addr);

    /* parameter check */
    RT_PARAM_CHK((NULL == pValid), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    osal_memset(pEntry, 0x00, sizeof(rtk_sec_ipMacBindEntry_t));

    /* calculation of the entry location */
    /* addr = { 0b0(SRAM), hash-index[7:0], entry-sel[3:0] } */
    /* addr = { 0b1(BCAM), zero[5:0], entry-index[5:0] } */
    if (addr < DAL_MANGO_IPMACBIND_ENTRY_BCAM_IDX_BASE)
    {
        hashIdx  = ((addr & (0xFF << 4)) >> 4);
        entrySel = ((addr & (0xF << 0)) >> 0);
        if (entrySel < DAL_MANGO_IPMACBIND_ENTRY_SRAM_BASIC_WIDTH)
        {
            /* index of the basic SRAM table = { hash-index[7:0], entry-sel[2:0] } */
            tbl = MANGO_IP_MAC_BINDt;
            idx = (hashIdx << 2) | entrySel;
            tfValid = MANGO_IP_MAC_BIND_VALIDtf;
            tfSMAC = MANGO_IP_MAC_BIND_SMACtf;
            tfSIP = MANGO_IP_MAC_BIND_SIPtf;
            tfPortBind = MANGO_IP_MAC_BIND_PORT_BINDtf;
            tfPortType = MANGO_IP_MAC_BIND_PORT_TYPEtf;
            tfPortID = MANGO_IP_MAC_BIND_PORT_IDtf;
            tfVIDBind = MANGO_IP_MAC_BIND_VID_BINDtf;
            tfVID = MANGO_IP_MAC_BIND_VIDtf;
        }
        else
        {
            /* index of the flexible table = { hash-index[7:0], entry-sel[3:0] } */
            tbl = MANGO_IP_MAC_BIND_DYNt;
            idx = (hashIdx << 3) | (entrySel - DAL_MANGO_IPMACBIND_ENTRY_SRAM_BASIC_WIDTH);
            tfValid = MANGO_IP_MAC_BIND_DYN_VALIDtf;
            tfSMAC = MANGO_IP_MAC_BIND_DYN_SMACtf;
            tfSIP = MANGO_IP_MAC_BIND_DYN_SIPtf;
            tfPortBind = MANGO_IP_MAC_BIND_DYN_PORT_BINDtf;
            tfPortType = MANGO_IP_MAC_BIND_DYN_PORT_TYPEtf;
            tfPortID = MANGO_IP_MAC_BIND_DYN_PORT_IDtf;
            tfVIDBind = MANGO_IP_MAC_BIND_DYN_VID_BINDtf;
            tfVID = MANGO_IP_MAC_BIND_DYN_VIDtf;
        }
    }
    else
    {
        /* index for BCAM */
        tbl = MANGO_IP_MAC_BIND_CAMt;
        idx = (addr - DAL_MANGO_IPMACBIND_ENTRY_BCAM_IDX_BASE);
        tfValid = MANGO_IP_MAC_BIND_CAM_VALIDtf;
        tfSMAC = MANGO_IP_MAC_BIND_CAM_SMACtf;
        tfSIP = MANGO_IP_MAC_BIND_CAM_SIPtf;
        tfPortBind = MANGO_IP_MAC_BIND_CAM_PORT_BINDtf;
        tfPortType = MANGO_IP_MAC_BIND_CAM_PORT_TYPEtf;
        tfPortID = MANGO_IP_MAC_BIND_CAM_PORT_IDtf;
        tfVIDBind = MANGO_IP_MAC_BIND_CAM_VID_BINDtf;
        tfVID = MANGO_IP_MAC_BIND_CAM_VIDtf;
    }

    /*** get entry from chip ***/
    SEC_TABLE_READ_ERR_HDL(unit, tbl, idx, ipmac_entry, "", errHandle, ret);

    /* get valid */
    SEC_TABLE_FIELD_GET_ERR_HDL(unit, tbl, tfValid, temp_var[0], ipmac_entry, "", errHandle, ret);
    *pValid = temp_var[0];

    /* get SIP */
    SEC_TABLE_FIELD_GET_ERR_HDL(unit, tbl, tfSIP, temp_var[0], ipmac_entry, "", errHandle, ret);
    pEntry->ipAddr = temp_var[0];

    /* get SMAC */
    SEC_TABLE_FIELD_MAC_GET_ERR_HDL(unit, tbl, tfSMAC, pEntry->macAddr.octet, ipmac_entry, "", errHandle, ret);

    /* get port bind */
    SEC_TABLE_FIELD_GET_ERR_HDL(unit, tbl, tfPortBind, temp_var[0], ipmac_entry, "", errHandle, ret);
    if (temp_var[0] != 0)
    {
        pEntry->flags |= RTK_SEC_IPMACBIND_FLAG_BIND_PORT;
    }

    /* get port type */
    SEC_TABLE_FIELD_GET_ERR_HDL(unit, tbl, tfPortType, temp_var[0], ipmac_entry, "", errHandle, ret);
    if (temp_var[0] != 0)
    {
        pEntry->flags |= RTK_SEC_IPMACBIND_FLAG_PORT_TRUNK;
    }

    /* get port id */
    SEC_TABLE_FIELD_GET_ERR_HDL(unit, tbl, tfPortID, temp_var[0], ipmac_entry, "", errHandle, ret);
    if (pEntry->flags & RTK_SEC_IPMACBIND_FLAG_PORT_TRUNK)
    {
        pEntry->trunkId = temp_var[0];
    }
    else
    {
        pEntry->port = temp_var[0];
    }

    /* get VLAN bind */
    SEC_TABLE_FIELD_GET_ERR_HDL(unit, tbl, tfVIDBind, temp_var[0], ipmac_entry, "", errHandle, ret);
    if (temp_var[0] != 0)
    {
        pEntry->flags |= RTK_SEC_IPMACBIND_FLAG_BIND_VLAN;
    }

    /* get VLAN id */
    SEC_TABLE_FIELD_GET_ERR_HDL(unit, tbl, tfVID, temp_var[0], ipmac_entry, "", errHandle, ret);
    pEntry->vid = temp_var[0];

    RT_LOG(LOG_TRACE, (MOD_SEC|MOD_DAL), "unit=%d, idx=%d", unit, idx);

errHandle:
    return ret;
} /* end of _dal_mango_getIpMacBindEntry */

/* Function Name:
 *      _dal_mango_setIpMacBindEntry
 * Description:
 *      Set an IP-MAC Binding entry to chip.
 * Input:
 *      unit   - unit id
 *      addr   - entry addr
 *      pValid - pointer to the validation of the entry
 *      pEntry - pointer to the entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) when addr >= DAL_MANGO_IPMACBIND_ENTRY_BCAM_IDX_BASE (4096),
 *          it means to access BCAM table. otherwise, access SRAM table.
 */
static int32 _dal_mango_setIpMacBindEntry(uint32 unit, int32 addr, uint32 *pValid, rtk_sec_ipMacBindEntry_t *pEntry)
{
    int32   ret;
    ipmac_bind_entry_t ipmac_entry;
    uint32  temp_var[2];    /* the longest length is MAC address (48bits) */
    uint32  hashIdx, entrySel;
    uint32  tbl, idx;
    uint32  tfValid, tfSIP, tfSMAC, tfPortBind, tfPortType, tfPortID, tfVIDBind, tfVID;

    RT_LOG(LOG_TRACE, (MOD_SEC|MOD_DAL), "unit=%d, addr=%d", unit, addr);

    /* parameter check */
    RT_PARAM_CHK((NULL == pValid), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    osal_memset(&ipmac_entry, 0x00, sizeof(ipmac_entry));

    /* calculation of the entry location */
    /* addr = { 0b0(SRAM), hash-index[7:0], entry-sel[3:0] } */
    /* addr = { 0b1(BCAM), zero[5:0], entry-index[5:0] } */
    if (addr < DAL_MANGO_IPMACBIND_ENTRY_BCAM_IDX_BASE)
    {
        hashIdx  = ((addr & (0xFF << 4)) >> 4);
        entrySel = ((addr & (0xF << 0)) >> 0);
        if (entrySel < DAL_MANGO_IPMACBIND_ENTRY_SRAM_BASIC_WIDTH)
        {
            /* index of the basic SRAM table = { hash-index[7:0], entry-sel[2:0] } */
            tbl = MANGO_IP_MAC_BINDt;
            idx = (hashIdx << 2) | entrySel;
            tfValid = MANGO_IP_MAC_BIND_VALIDtf;
            tfSMAC = MANGO_IP_MAC_BIND_SMACtf;
            tfSIP = MANGO_IP_MAC_BIND_SIPtf;
            tfPortBind = MANGO_IP_MAC_BIND_PORT_BINDtf;
            tfPortType = MANGO_IP_MAC_BIND_PORT_TYPEtf;
            tfPortID = MANGO_IP_MAC_BIND_PORT_IDtf;
            tfVIDBind = MANGO_IP_MAC_BIND_VID_BINDtf;
            tfVID = MANGO_IP_MAC_BIND_VIDtf;
        }
        else
        {
            /* index of the flexible table = { hash-index[7:0], entry-sel[3:0] } */
            tbl = MANGO_IP_MAC_BIND_DYNt;
            idx = (hashIdx << 3) | (entrySel - DAL_MANGO_IPMACBIND_ENTRY_SRAM_BASIC_WIDTH);
            tfValid = MANGO_IP_MAC_BIND_DYN_VALIDtf;
            tfSMAC = MANGO_IP_MAC_BIND_DYN_SMACtf;
            tfSIP = MANGO_IP_MAC_BIND_DYN_SIPtf;
            tfPortBind = MANGO_IP_MAC_BIND_DYN_PORT_BINDtf;
            tfPortType = MANGO_IP_MAC_BIND_DYN_PORT_TYPEtf;
            tfPortID = MANGO_IP_MAC_BIND_DYN_PORT_IDtf;
            tfVIDBind = MANGO_IP_MAC_BIND_DYN_VID_BINDtf;
            tfVID = MANGO_IP_MAC_BIND_DYN_VIDtf;
        }
    }
    else
    {
        /* index for BCAM (same as addr) */
        tbl = MANGO_IP_MAC_BIND_CAMt;
        idx = (addr - DAL_MANGO_IPMACBIND_ENTRY_BCAM_IDX_BASE);
        tfValid = MANGO_IP_MAC_BIND_CAM_VALIDtf;
        tfSMAC = MANGO_IP_MAC_BIND_CAM_SMACtf;
        tfSIP = MANGO_IP_MAC_BIND_CAM_SIPtf;
        tfPortBind = MANGO_IP_MAC_BIND_CAM_PORT_BINDtf;
        tfPortType = MANGO_IP_MAC_BIND_CAM_PORT_TYPEtf;
        tfPortID = MANGO_IP_MAC_BIND_CAM_PORT_IDtf;
        tfVIDBind = MANGO_IP_MAC_BIND_CAM_VID_BINDtf;
        tfVID = MANGO_IP_MAC_BIND_CAM_VIDtf;
    }

    /* set valid */
    temp_var[0] = *pValid;
    SEC_TABLE_FIELD_SET_ERR_HDL(unit, tbl, tfValid, temp_var[0], ipmac_entry, "", errHandle, ret);

    /* set SIP */
    temp_var[0] = pEntry->ipAddr;
    SEC_TABLE_FIELD_SET_ERR_HDL(unit, tbl, tfSIP, temp_var[0], ipmac_entry, "", errHandle, ret);

    /* set SMAC */
    SEC_TABLE_FIELD_MAC_SET_ERR_HDL(unit, tbl, tfSMAC, pEntry->macAddr.octet, ipmac_entry, "", errHandle, ret);

    /* set port bind */
    temp_var[0] = (pEntry->flags & RTK_SEC_IPMACBIND_FLAG_BIND_PORT)? 1 : 0;
    SEC_TABLE_FIELD_SET_ERR_HDL(unit, tbl, tfPortBind, temp_var[0], ipmac_entry, "", errHandle, ret);

    /* set port type */
    temp_var[0] = (pEntry->flags & RTK_SEC_IPMACBIND_FLAG_PORT_TRUNK)? 1 : 0;
    SEC_TABLE_FIELD_SET_ERR_HDL(unit, tbl, tfPortType, temp_var[0], ipmac_entry, "", errHandle, ret);

    /* set port id */
    temp_var[0] = (pEntry->flags & RTK_SEC_IPMACBIND_FLAG_PORT_TRUNK)? pEntry->trunkId : pEntry->port;
    SEC_TABLE_FIELD_SET_ERR_HDL(unit, tbl, tfPortID, temp_var[0], ipmac_entry, "", errHandle, ret);

    /* set VLAN bind */
    temp_var[0] = (pEntry->flags & RTK_SEC_IPMACBIND_FLAG_BIND_VLAN)? 1 : 0;
    SEC_TABLE_FIELD_SET_ERR_HDL(unit, tbl, tfVIDBind, temp_var[0], ipmac_entry, "", errHandle, ret);

    /* set VLAN id */
    temp_var[0] = pEntry->vid;
    SEC_TABLE_FIELD_SET_ERR_HDL(unit, tbl, tfVID, temp_var[0], ipmac_entry, "", errHandle, ret);

    /*** set entry into chip ***/
    SEC_TABLE_WRITE_ERR_HDL(unit, tbl, idx, ipmac_entry, "", errHandle, ret);

    RT_LOG(LOG_TRACE, (MOD_SEC|MOD_DAL), "unit=%d, idx=%d", unit, idx);

errHandle:
    return ret;
} /* end of _dal_mango_setIpMacBindEntry */

/* Function Name:
 *      dal_mango_sec_ipMacBindEntry_add
 * Description:
 *      Add an IP-MAC bind entry.
 * Input:
 *      unit   - unit id
 *      pEntry - pointer to the entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_ENTRY_EXIST  - input entry has already been added
 *      RT_ERR_TBL_FULL     - table is full
 * Note:
 */
int32
dal_mango_sec_ipMacBindEntry_add(uint32 unit, rtk_sec_ipMacBindEntry_t *pEntry)
{
    int32   ret;
    uint32  hashIdx;
    uint32  addr;
    uint32  valid;
    int32   emptyAddr = -1;  /* Not Found */
    uint32  regVal;
    int32   bucketWidth;
    int32   i;
    rtk_sec_ipMacBindEntry_t hwEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SEC), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(sec_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* Calculate the hash index */
    RT_ERR_CHK(_dal_mango_getHashIndex(&hashIdx, pEntry), ret);

    /* function body */
    SEC_SEM_LOCK(unit);

    /* to check the flexible table if it's used by IP-MAC-BINDING */
    SEC_REG_ARRAY_FIELD_READ_ERR_HDL(unit, MANGO_FLEX_TBL_CTRLr, \
        REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, \
        MANGO_FLEX_TBL_FMTf, regVal, "", errHandle, ret);
    bucketWidth = (2 == (int32)regVal)? \
        (DAL_MANGO_IPMACBIND_ENTRY_SRAM_BASIC_WIDTH + DAL_MANGO_IPMACBIND_ENTRY_SRAM_DYN_WIDTH) : \
        (DAL_MANGO_IPMACBIND_ENTRY_SRAM_BASIC_WIDTH);

    /* search all slots to check if there is an existing entry same as this,
     * and find an avaliable entry
     */

    /* SRAM/DYN table */
    for (i=0; i<bucketWidth; i++)
    {
        /* build addr (SRAM/DYN) */
        addr = (hashIdx << 4) | i;
        RT_ERR_HDL(_dal_mango_getIpMacBindEntry(unit, addr, &valid, &hwEntry), errHandle, ret);

        /* find an empty entry */
        if ((valid == 0) && (emptyAddr < 0))
        {
            /* store the first empty entry */
            emptyAddr = addr;
        }
        else if (valid != 0)
        {
            /* check if there is a same entry */
            if (0 == _dal_mango_cmpIpMacBindEntry(&hwEntry, pEntry))
            {
                ret = RT_ERR_ENTRY_EXIST;
                goto errHandle;
            }
        }
    }

    /* BCAM table */
    for (i=0; i<DAL_MANGO_IPMACBIND_ENTRY_BCAM_TBL_SIZE; i++)
    {
        /* build addr (BCAM) */
        addr = (1 << 12) | i;
        RT_ERR_HDL(_dal_mango_getIpMacBindEntry(unit, addr, &valid, &hwEntry), errHandle, ret);

        /* find an empty entry */
        if ((valid == 0) && (emptyAddr < 0))
        {
            /* store the first empty entry */
            emptyAddr = addr;
        }
        else if (valid != 0)
        {
            /* check if there is a same entry */
            if (0 == _dal_mango_cmpIpMacBindEntry(&hwEntry, pEntry))
            {
                ret = RT_ERR_ENTRY_EXIST;
                goto errHandle;
            }
        }
    }

    /* try to add the new entry if there is an empty entry */
    if (emptyAddr >= 0)
    {
        valid = 1;  /* enable */
        RT_ERR_HDL(_dal_mango_setIpMacBindEntry(unit, emptyAddr, &valid, pEntry), errHandle, ret);
    }
    else
    {
        /* table full */
        ret = RT_ERR_TBL_FULL;
    }

errHandle:
    SEC_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_sec_ipMacBindEntry_add */

/* Function Name:
 *      dal_mango_sec_ipMacBindEntry_del
 * Description:
 *      Delete an IP-MAC bind entry.
 * Input:
 *      unit   - unit id
 *      pEntry - pointer to the entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NOT_INIT       - the module is not initial
 *      RT_ERR_INPUT          - invalid input parameter
 *      RT_ERR_NULL_POINTER   - input parameter may be a null pointer
 *      RT_ERR_ENTRY_NOTFOUND - input entry is not found
 * Note:
 */
int32
dal_mango_sec_ipMacBindEntry_del(uint32 unit, rtk_sec_ipMacBindEntry_t *pEntry)
{
    int32   ret;
    uint32  hashIdx;
    uint32  addr;
    uint32  valid;
    uint32  regVal;
    int32   bucketWidth;
    int32   i;
    rtk_sec_ipMacBindEntry_t hwEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SEC), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(sec_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* Calculate the hash index */
    RT_ERR_CHK(_dal_mango_getHashIndex(&hashIdx, pEntry), ret);

    /* function body */
    SEC_SEM_LOCK(unit);

    /* to check the flexible table if it's used by IP-MAC-BINDING */
    SEC_REG_ARRAY_FIELD_READ_ERR_HDL(unit, MANGO_FLEX_TBL_CTRLr, \
        REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, \
        MANGO_FLEX_TBL_FMTf, regVal, "", errHandle, ret);
    bucketWidth = (2 == (int32)regVal)? \
        (DAL_MANGO_IPMACBIND_ENTRY_SRAM_BASIC_WIDTH + DAL_MANGO_IPMACBIND_ENTRY_SRAM_DYN_WIDTH) : \
        (DAL_MANGO_IPMACBIND_ENTRY_SRAM_BASIC_WIDTH);

    /* search all slots to check if there is an existing entry same as this,
     * and find an avaliable entry
     */

    /* SRAM/DYN table */
    for (i=0; i<bucketWidth; i++)
    {
        /* build addr (SRAM/DYN) */
        addr = (hashIdx << 4) | i;
        RT_ERR_HDL(_dal_mango_getIpMacBindEntry(unit, addr, &valid, &hwEntry), errHandle, ret);

        /* check the entry */
        if (valid != 0)
        {
            /* check if there is a same entry */
            if (0 == _dal_mango_cmpIpMacBindEntry(&hwEntry, pEntry))
            {
                valid = 0;  /* disable */
                RT_ERR_HDL(_dal_mango_setIpMacBindEntry(unit, addr, &valid, &hwEntry), errHandle, ret);

                ret = RT_ERR_OK;
                goto errOk;
            }
        }
    }

    /* BCAM table */
    for (i=0; i<DAL_MANGO_IPMACBIND_ENTRY_BCAM_TBL_SIZE; i++)
    {
        /* build addr (BCAM) */
        addr = (1 << 12) | i;
        RT_ERR_HDL(_dal_mango_getIpMacBindEntry(unit, addr, &valid, &hwEntry), errHandle, ret);

        /* check the entry */
        if (valid != 0)
        {
            /* check if there is a same entry */
            if (0 == _dal_mango_cmpIpMacBindEntry(&hwEntry, pEntry))
            {
                valid = 0;  /* disable */
                RT_ERR_HDL(_dal_mango_setIpMacBindEntry(unit, addr, &valid, &hwEntry), errHandle, ret);

                ret = RT_ERR_OK;
                goto errOk;
            }
        }
    }

    ret = RT_ERR_ENTRY_NOTFOUND;

errHandle:
errOk:
    SEC_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_sec_ipMacBindEntry_del */

/* Function Name:
 *      dal_mango_sec_ipMacBindEntry_getNext
 * Description:
 *      Get the next valid IP-MAC bind entry (based on the base index)
 * Input:
 *      unit   - unit id
 *      pEntry - pointer to the entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Note:
 *      (1) Use base index -1 to indicate to search from the beginging of IP-MAC table
 *      (2) If the returned index is -1, means not found the next valid entry
 */
int32
dal_mango_sec_ipMacBindEntry_getNext(uint32 unit, int32 *pBase, rtk_sec_ipMacBindEntry_t *pEntry)
{
    int32   ret;
    uint32  addr;
    uint32  regVal, bucketWidth;
    uint32  hashIdx, entrySel;
    uint32  valid = 0;
    rtk_sec_ipMacBindEntry_t    hwEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SEC), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(sec_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pBase), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    addr = (*pBase < 0)? 0 : (*pBase);
    osal_memset(&hwEntry, 0x00, sizeof(hwEntry));

    /* function body */
    SEC_SEM_LOCK(unit);

    /* to check the flexible table if it's used by IP-MAC-BINDING */
    SEC_REG_ARRAY_FIELD_READ_ERR_HDL(unit, MANGO_FLEX_TBL_CTRLr, \
        REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, \
        MANGO_FLEX_TBL_FMTf, regVal, "", errHandle, ret);
    bucketWidth = (2 == (int32)regVal)? \
        (DAL_MANGO_IPMACBIND_ENTRY_SRAM_BASIC_WIDTH + DAL_MANGO_IPMACBIND_ENTRY_SRAM_DYN_WIDTH) : \
        (DAL_MANGO_IPMACBIND_ENTRY_SRAM_BASIC_WIDTH);

    while (addr < (DAL_MANGO_IPMACBIND_ENTRY_BCAM_IDX_BASE + DAL_MANGO_IPMACBIND_ENTRY_BCAM_TBL_SIZE))
    {
        /* fix addr if need */
        if (addr < DAL_MANGO_IPMACBIND_ENTRY_BCAM_IDX_BASE)
        {
            hashIdx  = ((addr & (0xFF << 4)) >> 4);
            entrySel = ((addr & (0xF << 0)) >> 0);

            if (entrySel >= bucketWidth)
            {
                hashIdx += 1;
                entrySel = 0;
            }
            if (hashIdx < DAL_MANGO_IPMACBIND_ENTRY_SRAM_HASH_IDX_MAX)
            {
                addr = (hashIdx << 4) | entrySel;
            }
            else
            {
                /* jump to BCAM table */
                addr = DAL_MANGO_IPMACBIND_ENTRY_BCAM_IDX_BASE;
            }
        }

        /* get entry */
        RT_ERR_HDL(_dal_mango_getIpMacBindEntry(unit, addr, &valid, &hwEntry), errHandle, ret);

        if (valid != 0)
        {
            *pBase = addr;
            *pEntry = hwEntry;

            ret = RT_ERR_OK;
            goto errOk;
        }

        addr++; /* go to the next */
    }

    *pBase = -1;    /* not found */

errHandle:
errOk:
    SEC_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_sec_ipMacBindEntry_getNext */

/* Function Name:
 *      dal_mango_sec_attackPreventHit_get
 * Description:
 *      Get hit indication for each kind of attack.
 * Input:
 *      unit            - unit id
 *      attack_type     - type of attack
 * Output:
 *      pHit            - pointer to hit indication for attack
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
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
 *      - IPV4_INVALID_HDR
 */
int32
dal_mango_sec_attackPreventHit_get(uint32 unit,
    rtk_sec_attackType_t attack_type, uint32 *pHit)
{
    uint32  value;
    uint32  field_idx;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SEC), "unit=%d,attack_type=%d", unit, attack_type);

    /* check Init status */
    RT_INIT_CHK(sec_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((attack_type >= ATTACK_TYPE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pHit), RT_ERR_NULL_POINTER);

    /* function body */
    /* chip's value translate */
    switch (attack_type)
    {
        case TCP_FRAG_OFF_MIN_CHECK:
            field_idx = MANGO_TCP_FRAG_OFF_MINf;
            break;
        case SYNRST_DENY:
            field_idx = MANGO_SYN_RSTf;
            break;
        case SYNFIN_DENY:
            field_idx = MANGO_SYN_FINf;
            break;
        case XMA_DENY:
            field_idx = MANGO_XMASf;
            break;
        case NULLSCAN_DENY:
            field_idx = MANGO_NULL_SCANf;
            break;
        case SYN_SPORTL1024_DENY:
            field_idx = MANGO_SYN_SPORT_LESS_1024f;
            break;
        case TCPHDR_MIN_CHECK:
            field_idx = MANGO_TCP_HDR_MINf;
            break;
        case SMURF_DENY:
            field_idx = MANGO_SMURFf;
            break;
        case ICMPV6_PING_MAX_CHECK:
            field_idx = MANGO_ICMPV6_PING_MAXf;
            break;
        case ICMPV4_PING_MAX_CHECK:
            field_idx = MANGO_ICMPV4_PING_MAXf;
            break;
        case ICMP_FRAG_PKTS_DENY:
            field_idx = MANGO_ICMP_FRAG_PKTf;
            break;
        case IPV6_MIN_FRAG_SIZE_CHECK:
            field_idx = MANGO_IPV6_FRAG_LEN_MINf;
            break;
        case POD_DENY:
            field_idx = MANGO_PODf;
            break;
        case TCPBLAT_DENY:
            field_idx = MANGO_TCP_BLATf;
            break;
        case UDPBLAT_DENY:
            field_idx = MANGO_UDP_BLATf;
            break;
        case LAND_DENY:
            field_idx = MANGO_LANDf;
            break;
        case DAEQSA_DENY:
            field_idx = MANGO_DA_EQUAL_SAf;
            break;
        case IPV4_INVALID_HDR:
            field_idx = MANGO_INVALID_HEADERf;
            break;
        default:
            return RT_ERR_INPUT;
    }

    SEC_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_field_read(unit, MANGO_ATK_PRVNT_STSr,
            field_idx, &value)) != RT_ERR_OK)
    {
        SEC_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SEC), "");
        return ret;
    }

    SEC_SEM_UNLOCK(unit);

    *pHit = value;

    return RT_ERR_OK;
}   /* end of dal_mango_sec_attackPreventHit_get */

