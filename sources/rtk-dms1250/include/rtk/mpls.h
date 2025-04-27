/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
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
 * Purpose : Definition those public MPLS routing APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *              1) MPLS
 */

#ifndef __RTK_MPLS_H__
#define __RTK_MPLS_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <rtk/l3.h>

/*
 * Symbol Definition
 */
/* MPLS related flags */
#define RTK_MPLS_EXP_MAX    7
#define RTK_MPLS_TTL_MAX    255

typedef uint32 rtk_mpls_entryId_t;

/*
 * MPLS label operation
 */
typedef enum rtk_mpls_label_oper_e
{
    RTK_MPLS_LABEL_OPER_SINGLE,
    RTK_MPLS_LABEL_OPER_DOUBEL,
    RTK_MPLS_LABEL_OPER_MAX,
} rtk_mpls_label_oper_t;

/*
 * MPLS TTL inherit type
 */
typedef enum rtk_mpls_ttlInherit_e {
    RTK_MPLS_TTL_INHERIT_UNIFORM,
    RTK_MPLS_TTL_INHERIT_PIPE,
    RTK_MPLS_TTL_INHERIT_MAX,
} rtk_mpls_ttlInherit_t;

/* MPLS common operation flag */
typedef enum rtk_mpls_flag_e
{
    RTK_MPLS_FLAG_NONE          = (1 << 0),
    RTK_MPLS_FLAG_TTL_ASSIGN    = (1 << 1),     /* assign TTL operation */
    RTK_MPLS_FLAG_TTL_INHERIT   = (1 << 2),     /* inherit TTL operation */
    RTK_MPLS_FLAG_TC_INHERIT    = (1 << 3),     /* inherit traffic class operation */
    RTK_MPLS_FLAG_ECMP          = (1 << 4),     /* pointer to ECMP entry */
    RTK_MPLS_FLAG_INTPRI_ASSIGN = (1 << 5),     /* assign internal priority */
    RTK_MPLS_FLAG_NEXTLABEL     = (1 << 6),     /* configure next label entry operation */
} rtk_mpls_flag_t;

/* MPLS traffic class action */
typedef enum rtk_mpls_tcAct_e
{
    RTK_MPLS_TC_NONE,           /* none TC operation */
    RTK_MPLS_TC_ASSIGN,         /* assign TC fixed value */
    RTK_MPLS_TC_INTPRI,         /* assign TC from internal priority */
    RTK_MPLS_TC_INHERIT,        /* assign TC by inherit */
    RTK_MPLS_TC_END,
} rtk_mpls_tcAct_t;

/* MPLS label action */
typedef enum rtk_mpls_labelAct_e
{
    RTK_MPLS_LABELACT_PUSH,     /* push label */
    RTK_MPLS_LABELACT_SWAP,     /* swap label */
    RTK_MPLS_LABELACT_POP,      /* pop label */
    RTK_MPLS_LABELACT_PHP,      /* Penultimate hop popping */
    RTK_MPLS_LABELACT_END,
} rtk_mpls_labelAct_t;

/* MPLS exception type */
typedef enum rtk_mpls_exceptionType_e
{
    RTK_MPLS_ET_LABEL_EXCEED,   /* number of label exceeds system support */
    RTK_MPLS_ET_LABEL_UNKNOWN,  /* unknown label */
    RTK_MPLS_ET_TTL_FAIL,       /* forward TTL is 0 */
    RTK_MPLS_ET_LABEL_0_15,     /* reserved label 0 ~ 15 */
    RTK_MPLS_ET_END,
} rtk_mpls_exceptionType_t;

/* MPLS hash algorithm type */
typedef enum rtk_mpls_hashAlgo_e
{
    RTK_MPLS_HASHALGO_0,
    RTK_MPLS_HASHALGO_1,
    RTK_MPLS_HASHALGO_END,
} rtk_mpls_hashAlgo_t;

/* egress MPLS TC map source */
typedef enum rtk_mpls_egrTcMapSrcType_e
{
    RTK_MPLS_EGRTCMAPSRCTYPE_DP_AND_INTPRI,
    RTK_MPLS_EGRTCMAPSRCTYPE_END
} rtk_mpls_egrTcMapSrcType_t;

/*
 * Data Declaration
 */

/*
 * MPLS encapsulation information structure.
 */
typedef struct rtk_mpls_encap_s {
#if defined(CONFIG_SDK_RTL8390)
    uint32                  label0;     /* VC label */
    uint32                  label1;     /* Tunnel label */
    int                     exp0;       /* VC label experimental */
    int                     exp1;       /* Tunnel label experimental */
    int                     ttl0;       /* VC label TTL */
    int                     ttl1;       /* Tunnel label TTL */
    rtk_mpls_label_oper_t   oper;       /* push operation */
#endif
#if defined(CONFIG_SDK_RTL9310)
    rtk_mpls_flag_t     flags;
    rtk_mpls_labelAct_t labelAct;
    uint32              label;          /* the label value for labelAct */
    uint8               ttl;            /* the field is used if RTK_MPLS_FLAG_TTL_ASSIGN is set */
    rtk_mpls_tcAct_t    tcAct;          /* traffic class operation */
    uint8               tc;             /* the field is used if tcAct = RTK_MPLS_TC_ASSIGN */
    rtk_mpls_entryId_t  nextEntryId;    /* the field is used if RTK_MPLS_FLAG_NEXTLABEL is set */
#endif  /* CONFIG_SDK_RTL8390 */
} rtk_mpls_encap_t;

/* MPLS next hop entry structure */
typedef struct rtk_mpls_nextHop_s
{
    rtk_intf_id_t       intfIdx;        /* interface index (source MAC, destination VID ) */
    rtk_mpls_entryId_t  encapId;
    rtk_mac_t           mac_addr;       /* next hop MAC address */

    rtk_l3_act_t        l3_act;         /* L3 forwarding action:
                                         * RTK_L3_ACT_FORWARD        - keep forwarding
                                         * RTK_L3_ACT_DROP           - NULL interface (drop)
                                         * RTK_L3_ACT_TRAP2CPU       - trap to local CPU
                                         * RTK_L3_ACT_TRAP2MASTERCPU - trap to master CPU
                                         */
} rtk_mpls_nextHop_t;

/* MPLS decapsulation entry */
typedef struct rtk_mpls_decapEntry_s
{
    rtk_mpls_flag_t         flags;
    uint32                  label;
    rtk_mpls_labelAct_t     labelAct;
    rtk_intf_id_t           intfId;         /*
                                             * if labelAct = RTK_MPLS_LABELACT_POP, the field is L3 interface ID.
                                             *
                                             * if labelAct = RTK_MPLS_LABELACT_SWAP or RTK_MPLS_LABELACT_PHP,
                                             *    and if RTK_MPLS_FLAG_ECMP is set, the field is ECMP entry index,
                                             *    else RTK_MPLS_FLAG_ECMP is unset, the field is next hop entry index.
                                             */
    uint8                   priSelTblId;    /* pointer to priority selection table */
    rtk_pri_t               intPri;         /* the field is used if RTK_MPLS_FLAG_INTPRI_ASSIGN is set */
} rtk_mpls_decapEntry_t;

/* egress MPLS TC map source */
typedef struct rtk_mpls_egrTcMapSrc_s
{
    rtk_mpls_egrTcMapSrcType_t  type;
    rtk_pri_t                   pri;
    uint8                       dp;
} rtk_mpls_egrTcMapSrc_t;

/*
 * Macro Declaration
 */

/*
 * Function Declaration
 */

/* Module Name : MPLS */

/* Function Name:
 *      rtk_mpls_init
 * Description:
 *      Initialize mpls module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8390, 9310
 * Note:
 *      Must initialize MPLS module before calling any MPLS APIs.
 * Changes:
 *      None
 */
extern int32
rtk_mpls_init(uint32 unit);

/* Function Name:
 *      rtk_mpls_ttlInherit_get
 * Description:
 *      Get MPLS TTL inherit properties
 * Input:
 *      unit     - unit id
 * Output:
 *      pInherit - pointer buffer of MPLS TTL inherit information
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390
 * Note:
 *      (1) inherit = uniform, the TTL of MPLS header inherits from IP TTL and decrement 1.
 *      (2) inherit = pipe, the TTL of MPLS header is from rtk_mpls_encap_set.
 * Changes:
 *      None
 */
extern int32
rtk_mpls_ttlInherit_get(uint32 unit, rtk_mpls_ttlInherit_t *pInherit);

/* Function Name:
 *      rtk_mpls_ttlInherit_set
 * Description:
 *      Set MPLS TTL inherit properties
 * Input:
 *      unit    - Device number
 *      inherit - MPLS TTL inherit information
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8390
 * Note:
 *      (1) inherit = uniform, the TTL of MPLS header inherits from IP TTL and decrement 1.
 *      (2) inherit = pipe, the TTL of MPLS header is from rtk_mpls_encap_set.
 * Changes:
 *      None
 */
extern int32
rtk_mpls_ttlInherit_set(uint32 unit, rtk_mpls_ttlInherit_t inherit);

/* Function Name:
 *      rtk_mpls_encap_get
 * Description:
 *      Get MPLS encapsulation properties
 * Input:
 *      unit    - Device number
 *      encapId  - MPLS encapsulation entry ID
 * Output:
 *      pInfo    - pointer buffer of MPLS encapsulation information
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390, 9310
 * Note:
 *      For 8390:
 *          (1) For single label operation, lib_idx ranges from 0 to 511 and only info->label0 is used.
 *              For double label operation, lib_idx ranges from 0 to 255 and both info->label0 and info->label1 are used.
 *          (2) The TLL of MPLS header inheritance is controlled by 'rtk_mpls_ttlInherit_set'.
 * Changes:
 *      None
 */
extern int32
rtk_mpls_encap_get(uint32 unit, rtk_mpls_entryId_t encapId, rtk_mpls_encap_t *pInfo);

/* Function Name:
 *      rtk_mpls_encap_set
 * Description:
 *      Set MPLS encapsulation properties
 * Input:
 *      unit    - Device number
 *      encapId  - MPLS encapsulation entry ID
 *      pInfo   - MPLS encapsulation information
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390, 9310
 * Note:
 *      For 8390:
 *          (1) For single label operation, lib_idx ranges from 0 to 511 and only info->label0 is used.
 *              For double label operation, lib_idx ranges from 0 to 255 and both info->label0 and info->label1 are used.
 *          (2) The TLL of MPLS header inheritance is controlled by 'rtk_mpls_ttlInherit_set'.
 *      For 9310:
 *          (1) flags only support:
 *              - RTK_MPLS_FLAG_NONE,
 *              - RTK_MPLS_FLAG_TTL_ASSIGN
 *              - RTK_MPLS_FLAG_NEXTLABEL
 *          (2) labelAct only support:
 *              - RTK_MPLS_LABELACT_PUSH
 *              - RTK_MPLS_LABELACT_SWAP
 * Changes:
 *      None
 */
extern int32
rtk_mpls_encap_set(uint32 unit, rtk_mpls_entryId_t encapId, rtk_mpls_encap_t *pInfo);

/* Function Name:
 *      rtk_mpls_enable_get
 * Description:
 *      Get MPLS state
 * Input:
 *      unit    - Device number
 * Output:
 *      pEnable - MPLS state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390
 * Note:
 *      For 8390:
 *          MPLS packet is treated as L2 unknown packet if MPLS function is disabled.
 * Changes:
 *      None
 */
extern int32
rtk_mpls_enable_get(uint32 unit, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_mpls_enable_set
 * Description:
 *      Set MPLS state
 * Input:
 *      unit    - Device number
 *      enable  - state of MPLS
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8390
 * Note:
 *      For 8390:
 *          MPLS packet is treated as L2 unknown packet if MPLS function is disabled.
 * Changes:
 *      None
 */
extern int32
rtk_mpls_enable_set(uint32 unit, rtk_enable_t enable);

/* Function Name:
 *      rtk_mpls_trapTarget_get
 * Description:
 *      Get information of MPLS trap packet to local or master CPU.
 * Input:
 *      unit    - unit id
 * Output:
 *      pTarget - pointer to the information of MPLS trap packet
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_mpls_trapTarget_get(uint32 unit, rtk_trapTarget_t *pTarget);

/* Function Name:
 *      rtk_mpls_trapTarget_set
 * Description:
 *      Set information of MPLS trap packet to local or master CPU.
 * Input:
 *      unit    - unit id
 *      target  - the status of MPLS trap packet
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_mpls_trapTarget_set(uint32 unit, rtk_trapTarget_t target);

/* Function Name:
 *      rtk_mpls_exceptionCtrl_get
 * Description:
 *      Get action of packet with exception situation.
 * Input:
 *      unit    - unit id
 *      type    - exception type
 * Output:
 *      pAction    - pointer to action of exception type
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID              - invalid unit id
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_MPLS_EXCEPT_TYPE     - invalid exception type
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_mpls_exceptionCtrl_get(uint32 unit, rtk_mpls_exceptionType_t type,
    rtk_action_t *pAction);

/* Function Name:
 *      rtk_mpls_exceptionCtrl_set
 * Description:
 *      Set action of packet with exception situation.
 * Input:
 *      unit    - unit id
 *      type    - exception type
 *      action  - action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID              - invalid unit id
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_MPLS_EXCEPT_TYPE     - invalid exception type
 *      RT_ERR_INPUT                - invalid input parameter
 * Applicable:
 *      9310
 * Note:
 *      (1) the action is as following:
 *          - ACTION_DROP
 *          - ACTION_TRAP2CPU
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_mpls_exceptionCtrl_set(uint32 unit, rtk_mpls_exceptionType_t type,
    rtk_action_t action);

/* next hop */
/* Function Name:
 *      rtk_mpls_nextHop_create
 * Description:
 *      Create an MPLS nexthop and get the returned path ID
 * Input:
 *      unit    - unit id
 *      pNh     - pointer to nexthop
 * Output:
 *      pPathId - pointer to L3 path ID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_NOT_ALLOWED  - the module is not enabled
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_mpls_nextHop_create(uint32 unit, rtk_mpls_nextHop_t *pNh,
    rtk_l3_pathId_t *pPathId);

/* Function Name:
 *      rtk_mpls_nextHop_destroy
 * Description:
 *      Destroy an MPLS Next-Hop
 * Input:
 *      unit    - unit id
 *      pathId  - pointer to MPLS path ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NOT_ALLOWED  - the module is not enabled
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_mpls_nextHop_destroy(uint32 unit, rtk_l3_pathId_t pathId);

/* Function Name:
 *      rtk_mpls_nextHop_get
 * Description:
 *      Get an MPLS Next-Hop by path ID
 * Input:
 *      unit     - unit id
 *      pathId   - MPLS path ID
 * Output:
 *      pNextHop - pointer to nexthop
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_NOT_ALLOWED  - the module is not enabled
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_mpls_nextHop_get(uint32 unit, rtk_l3_pathId_t pathId,
    rtk_mpls_nextHop_t *pNh);

/* Function Name:
 *      rtk_mpls_nextHop_set
 * Description:
 *      Set an MPLS Next-Hop by path ID
 * Input:
 *      unit     - unit id
 *      pathId   - MPLS path ID
 *      pNextHop - pointer to nexthop
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_NOT_ALLOWED  - the module is not enabled
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_mpls_nextHop_set(uint32 unit, rtk_l3_pathId_t pathId,
    rtk_mpls_nextHop_t *pNh);

/* encapsulation */
/* Function Name:
 *      rtk_mpls_encap_create
 * Description:
 *      Create an MPLS encapsulation entry and get the returned encapsulation ID
 * Input:
 *      unit    - unit id
 *      pEncap  - pointer to nexthop
 * Output:
 *      pEncapId    - pointer to MPLS encapsulation entry ID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_NOT_ALLOWED  - the module is not enabled
 * Applicable:
 *      9310
 * Note:
 *      (1) flags only support:
 *          - RTK_MPLS_FLAG_NONE,
 *          - RTK_MPLS_FLAG_TTL_ASSIGN
 *          - RTK_MPLS_FLAG_NEXTLABEL
 *      (2) labelAct only support:
 *          - RTK_MPLS_LABELACT_PUSH
 *          - RTK_MPLS_LABELACT_SWAP
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_mpls_encap_create(uint32 unit, rtk_mpls_encap_t *pEncap,
    rtk_mpls_entryId_t *pEncapId);

/* Function Name:
 *      rtk_mpls_encap_destroy
 * Description:
 *      Destory an MPLS encapsulation entry by encapsulation entry ID
 * Input:
 *      unit     - unit id
 *      encapId  - MPLS encapsulation entry ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NOT_ALLOWED  - the module is not enabled
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_mpls_encap_destroy(uint32 unit, rtk_mpls_entryId_t encapId);

/* Function Name:
 *      rtk_mpls_encapId_find
 * Description:
 *      Find an ID pointing to a MPLS encapsulation entry
 * Input:
 *      unit    - unit id
 *      pEncap  - pointer to MPLS encapsulation entry
 * Output:
 *      pEncapId    - pointer to MPLS encapsulation entry ID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_NOT_ALLOWED  - the module is not enabled
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_mpls_encapId_find(uint32 unit, rtk_mpls_encap_t *pEncap,
    rtk_mpls_entryId_t *pEncapId);

/* decapsulation */
/* Function Name:
 *      rtk_mpls_hashAlgo_get
 * Description:
 *      Get hash algorithm of MPLS decapsulation table
 * Input:
 *      unit        - unit id
 * Output:
 *      pHashAlgo   - pointer to hash algorithm of MPLS
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_mpls_hashAlgo_get(uint32 unit, uint8 *pHashAlgo);

/* Function Name:
 *      rtk_mpls_hashAlgo_set
 * Description:
 *      Set hash algorithm of MPLS decapsulation table
 * Input:
 *      unit        - unit id
 *      hashAlgo    - hash algorithm of MPLS
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      9310
 * Note:
 *      The valid input is 0 and 1. 0 mean hash_algo_0; 1 mean hash_algo_1.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_mpls_hashAlgo_set(uint32 unit, uint8 hashAlgo);

/* Function Name:
 *      rtk_mpls_decap_create
 * Description:
 *      Create an MPLS decapsulation entry
 * Input:
 *      unit    - unit id
 *      pDecap  - pointer to MPLS decapsulation entry
 * Output:
 *      pDecapId    - pointer to MPLS decapsultion entry ID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_NOT_ALLOWED  - the module is not enabled
 * Applicable:
 *      9310
 * Note:
 *      (1) flags only support:
 *          - RTK_MPLS_FLAG_NONE,
 *          - RTK_MPLS_FLAG_TTL_INHERIT
 *          - RTK_MPLS_FLAG_TC_INHERIT
 *          - RTK_MPLS_FLAG_ECMP
 *          - RTK_MPLS_FLAG_INTPRI_ASSIGN
 *      (2) labelAct only support:
 *          - RTK_MPLS_LABELACT_POP,
 *          - RTK_MPLS_LABELACT_SWAP
 *          - RTK_MPLS_LABELACT_PHP
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_mpls_decap_create(uint32 unit, rtk_mpls_decapEntry_t *pDecap,
    rtk_mpls_entryId_t *pDecapId);

/* Function Name:
 *      rtk_mpls_decap_destroy
 * Description:
 *      Destroy an MPLS decapsulation entry by decapsulation entry ID
 * Input:
 *      unit    - unit id
 *      decapId - MPLS decapsulation entry ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_NOT_ALLOWED  - the module is not enabled
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_mpls_decap_destroy(uint32 unit, rtk_mpls_entryId_t decapId);

/* Function Name:
 *      rtk_mpls_decap_get
 * Description:
 *      Get an MPLS decapsulation entry
 * Input:
 *      unit     - unit id
 *      decapId  - MPLS decapsulation entry ID
 * Output:
 *      pDecap   - pointer to MPLS decapsulation entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_NOT_ALLOWED  - the module is not enabled
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_mpls_decap_get(uint32 unit, rtk_mpls_entryId_t decapId,
    rtk_mpls_decapEntry_t *pDecap);

/* Function Name:
 *      rtk_mpls_decap_set
 * Description:
 *      Set an MPLS decapsulation entry
 * Input:
 *      unit     - unit id
 *      decapId  - MPLS decapsulation entry ID
 * Output:
 *      pDecap   - pointer to MPLS decapsulation entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_NOT_ALLOWED  - the module is not enabled
 * Applicable:
 *      9310
 * Note:
 *      (1) flags only support:
 *          - RTK_MPLS_FLAG_NONE,
 *          - RTK_MPLS_FLAG_TTL_INHERIT
 *          - RTK_MPLS_FLAG_TC_INHERIT
 *          - RTK_MPLS_FLAG_ECMP
 *          - RTK_MPLS_FLAG_INTPRI_ASSIGN
 *      (2) labelAct only support:
 *          - RTK_MPLS_LABELACT_POP,
 *          - RTK_MPLS_LABELACT_SWAP
 *          - RTK_MPLS_LABELACT_PHP
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_mpls_decap_set(uint32 unit, rtk_mpls_entryId_t decapId,
    rtk_mpls_decapEntry_t *pDecap);

/* Function Name:
 *      rtk_mpls_decapId_find
 * Description:
 *      Find an ID pointing to a MPLS decapsulation entry
 * Input:
 *      unit    - unit id
 *      pDecap  - pointer to MPLS decapsulation entry
 * Output:
 *      pEncapId    - pointer to MPLS encapsulation entry ID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_NOT_ALLOWED  - the module is not enabled
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_mpls_decapId_find(uint32 unit, rtk_mpls_decapEntry_t *pDecap,
    rtk_mpls_entryId_t *pDecapId);

/* Function Name:
 *      rtk_mpls_egrTcMap_get
 * Description:
 *      Get the map to egress MPLS TC configuration
 * Input:
 *      unit    - unit id
 *      src     - source map to egress MPLS TC
 * Output:
 *      pTc     - egress MPLS TC value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - Invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_DROP_PRECEDENCE  - invalid drop precedence
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_mpls_egrTcMap_get(uint32 unit, rtk_mpls_egrTcMapSrc_t *pSrc, uint8 *pTc);

/* Function Name:
 *      rtk_mpls_egrTcMap_set
 * Description:
 *      Set the map to egress MPLS TC configuration
 * Input:
 *      unit    - unit id
 *      src     - source map to egress MPLS TC
 *      tc      - egress MPLS TC value
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - Invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - Invalid input
 *      RT_ERR_DROP_PRECEDENCE  - invalid drop precedence
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_mpls_egrTcMap_set(uint32 unit, rtk_mpls_egrTcMapSrc_t *pSrc, uint8 tc);

/* Function Name:
 *      rtk_mpls_nextHop_create_id
 * Description:
 *      Create an MPLS nexthop by specific path ID
 * Input:
 *      unit    - unit id
 *      pNh     - pointer to nexthop
 *      pathId  - L3 path ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_NOT_ALLOWED  - the module is not enabled
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.7.0]
 *          New added function.
 */
extern int32
rtk_mpls_nextHop_create_id(uint32 unit, rtk_mpls_nextHop_t *pNh,
    rtk_l3_pathId_t pathId);

/* Function Name:
 *      rtk_mpls_encap_create_id
 * Description:
 *      Create an MPLS encapsulation entry by specific encapsulation ID
 * Input:
 *      unit    - unit id
 *      pEncap  - pointer to nexthop
 *      encapId - MPLS encapsulation entry ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_NOT_ALLOWED  - the module is not enabled
 * Applicable:
 *      9310
 * Note:
 *      (1) flags only support:
 *          - RTK_MPLS_FLAG_NONE,
 *          - RTK_MPLS_FLAG_TTL_ASSIGN
 *          - RTK_MPLS_FLAG_NEXTLABEL
 *      (2) labelAct only support:
 *          - RTK_MPLS_LABELACT_PUSH
 *          - RTK_MPLS_LABELACT_SWAP
 * Changes:
 *      [SDK_3.7.0]
 *          New added function.
 */
extern int32
rtk_mpls_encap_create_id(uint32 unit, rtk_mpls_encap_t *pEncap,
    rtk_mpls_entryId_t encapId);

#endif /* __RTK_MPLS_H__ */
