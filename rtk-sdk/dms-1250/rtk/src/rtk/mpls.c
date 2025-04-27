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

/*
 * Include Files
 */
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <dal/dal_mgmt.h>
#include <rtk/default.h>
#include <rtk/mpls.h>

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
int32
rtk_mpls_init(uint32 unit)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->mpls_init(unit);
} /* end of rtk_mpls_init */

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
int32
rtk_mpls_ttlInherit_get(uint32 unit, rtk_mpls_ttlInherit_t *pInherit)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->mpls_ttlInherit_get(unit, pInherit);
} /* end of rtk_mpls_ttlInherit_get */

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
int32
rtk_mpls_ttlInherit_set(uint32 unit, rtk_mpls_ttlInherit_t inherit)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->mpls_ttlInherit_set(unit, inherit);
} /* end of rtk_mpls_ttlInherit_set */

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
int32
rtk_mpls_trapTarget_get(uint32 unit, rtk_trapTarget_t *pTarget)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->mpls_trapTarget_get(unit, pTarget);
}   /* end of rtk_mpls_trapTarget_get */

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
int32
rtk_mpls_trapTarget_set(uint32 unit, rtk_trapTarget_t target)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->mpls_trapTarget_set(unit, target);
}   /* end of rtk_mpls_trapTarget_set */

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
int32
rtk_mpls_exceptionCtrl_get(uint32 unit, rtk_mpls_exceptionType_t type,
    rtk_action_t *pAction)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->mpls_exceptionCtrl_get(unit, type, pAction);
}   /* end of rtk_mpls_exceptionCtrl_get */

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
int32
rtk_mpls_exceptionCtrl_set(uint32 unit, rtk_mpls_exceptionType_t type,
    rtk_action_t action)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->mpls_exceptionCtrl_set(unit, type, action);
}   /* end of rtk_mpls_exceptionCtrl_set */

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
int32
rtk_mpls_nextHop_create(uint32 unit, rtk_mpls_nextHop_t *pNh,
    rtk_l3_pathId_t *pPathId)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->mpls_nextHop_create(unit, pNh, pPathId);
}   /* end of rtk_mpls_nextHop_create */

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
int32
rtk_mpls_nextHop_destroy(uint32 unit, rtk_l3_pathId_t pathId)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->mpls_nextHop_destroy(unit, pathId);
}   /* end of rtk_mpls_nextHop_destroy */

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
int32
rtk_mpls_nextHop_get(uint32 unit, rtk_l3_pathId_t pathId,
    rtk_mpls_nextHop_t *pNh)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->mpls_nextHop_get(unit, pathId, pNh);
}   /* end of rtk_mpls_nextHop_get */

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
int32
rtk_mpls_nextHop_set(uint32 unit, rtk_l3_pathId_t pathId,
    rtk_mpls_nextHop_t *pNh)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->mpls_nextHop_set(unit, pathId, pNh);
}   /* end of rtk_mpls_nextHop_set */

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
int32
rtk_mpls_encap_create(uint32 unit, rtk_mpls_encap_t *pEncap,
    rtk_mpls_entryId_t *pEncapId)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->mpls_encap_create(unit, pEncap, pEncapId);
}   /* end of rtk_mpls_encap_create */

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
int32
rtk_mpls_encap_destroy(uint32 unit, rtk_mpls_entryId_t encapId)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->mpls_encap_destroy(unit, encapId);
}   /* end of rtk_mpls_encap_destroy */

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
int32
rtk_mpls_encap_get(uint32 unit, rtk_mpls_entryId_t encapId, rtk_mpls_encap_t *pInfo)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->mpls_encap_get(unit, encapId, pInfo);
} /* end of rtk_mpls_encap_get */

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
int32
rtk_mpls_encap_set(uint32 unit, rtk_mpls_entryId_t encapId, rtk_mpls_encap_t *pInfo)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->mpls_encap_set(unit, encapId, pInfo);
} /* end of rtk_mpls_encap_set */

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
int32
rtk_mpls_enable_get(uint32 unit, rtk_enable_t *pEnable)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->mpls_enable_get(unit, pEnable);
} /* end of rtk_mpls_enable_get */

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
int32
rtk_mpls_enable_set(uint32 unit, rtk_enable_t enable)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->mpls_enable_set(unit, enable);
} /* end of rtk_mpls_enable_set */

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
int32
rtk_mpls_encapId_find(uint32 unit, rtk_mpls_encap_t *pEncap,
    rtk_mpls_entryId_t *pEncapId)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->mpls_encapId_find(unit, pEncap, pEncapId);
}   /* end of rtk_mpls_encapId_find */

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
int32
rtk_mpls_hashAlgo_get(uint32 unit, uint8 *pHashAlgo)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->mpls_hashAlgo_get(unit, pHashAlgo);
}   /* end of rtk_mpls_hashAlgo_get */

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
int32
rtk_mpls_hashAlgo_set(uint32 unit, uint8 hashAlgo)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->mpls_hashAlgo_set(unit, hashAlgo);
}   /* end of rtk_mpls_hashAlgo_set */

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
int32
rtk_mpls_decap_create(uint32 unit, rtk_mpls_decapEntry_t *pDecap,
    rtk_mpls_entryId_t *pDecapId)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->mpls_decap_create(unit, pDecap, pDecapId);
}   /* end of rtk_mpls_decap_create */

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
int32
rtk_mpls_decap_destroy(uint32 unit, rtk_mpls_entryId_t decapId)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->mpls_decap_destroy(unit, decapId);
}   /* end of rtk_mpls_decap_destroy */

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
int32
rtk_mpls_decap_get(uint32 unit, rtk_mpls_entryId_t decapId,
    rtk_mpls_decapEntry_t *pDecap)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->mpls_decap_get(unit, decapId, pDecap);
}   /* end of rtk_mpls_decap_get */

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
int32
rtk_mpls_decap_set(uint32 unit, rtk_mpls_entryId_t decapId,
    rtk_mpls_decapEntry_t *pDecap)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->mpls_decap_set(unit, decapId, pDecap);
}   /* end of rtk_mpls_decap_set */

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
int32
rtk_mpls_decapId_find(uint32 unit, rtk_mpls_decapEntry_t *pDecap,
    rtk_mpls_entryId_t *pDecapId)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->mpls_decapId_find(unit, pDecap, pDecapId);
}   /* end of rtk_mpls_decapId_find */

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
int32
rtk_mpls_egrTcMap_get(uint32 unit, rtk_mpls_egrTcMapSrc_t *pSrc, uint8 *pTc)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->mpls_egrTcMap_get(unit, pSrc, pTc);
}   /* end of rtk_mpls_egrTcMap_get */

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
int32
rtk_mpls_egrTcMap_set(uint32 unit, rtk_mpls_egrTcMapSrc_t *pSrc, uint8 tc)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->mpls_egrTcMap_set(unit, pSrc, tc);
}   /* end of rtk_mpls_egrTcMap_set */


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
int32
rtk_mpls_nextHop_create_id(uint32 unit, rtk_mpls_nextHop_t *pNh,
    rtk_l3_pathId_t pathId)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || NULL == RT_MGMT(unit), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->mpls_nextHop_create_id(unit, pNh, pathId);
}   /* end of rtk_mpls_nextHop_create_id */

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
int32
rtk_mpls_encap_create_id(uint32 unit, rtk_mpls_encap_t *pEncap,
    rtk_mpls_entryId_t encapId)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || NULL == RT_MGMT(unit), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->mpls_encap_create_id(unit, pEncap, encapId);
}   /* end of rtk_mpls_encap_create_id */

