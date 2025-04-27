/*
 * Copyright(c) Realtek Semiconductor Corporation, 2008-2009
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose :
 *
 * Feature :
 *
 */

#ifndef __DAL_MANGO_MPLS_H__
#define __DAL_MANGO_MPLS_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/mpls.h>
#include <dal/dal_mapper.h>


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
/* common */

/* Function Name:
 *      dal_mango_mplsMapper_init
 * Description:
 *      Hook mpls module of the specified device.
 * Input:
 *      pMapper - pointer of mapper
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Must Hook mpls module before calling any mpls APIs.
 */
extern int32
dal_mango_mplsMapper_init(dal_mapper_t *pMapper);

/* Function Name:
 *      dal_mango_mpls_init
 * Description:
 *      Initialize MPLS module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      Must initialize MPLS module before calling any MPLS APIs.
 */
extern int32
dal_mango_mpls_init(uint32 unit);

/* Function Name:
 *      dal_mango_mpls_trapTarget_get
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
 * Note:
 */
extern int32
dal_mango_mpls_trapTarget_get(uint32 unit, rtk_trapTarget_t *pTarget);

/* Function Name:
 *      dal_mango_mpls_trapTarget_set
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
 * Note:
 */
extern int32
dal_mango_mpls_trapTarget_set(uint32 unit, rtk_trapTarget_t target);

/* Function Name:
 *      dal_mango_mpls_exceptionCtrl_get
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
 * Note:
 */
extern int32
dal_mango_mpls_exceptionCtrl_get(uint32 unit, rtk_mpls_exceptionType_t type,
    rtk_action_t *pAction);

/* Function Name:
 *      dal_mango_mpls_exceptionCtrl_set
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
 * Note:
 *      (1) the action is as following:
 *          - ACTION_DROP
 *          - ACTION_TRAP2CPU
 */
extern int32
dal_mango_mpls_exceptionCtrl_set(uint32 unit, rtk_mpls_exceptionType_t type,
    rtk_action_t action);

/* next hop */
/* Function Name:
 *      dal_mango_mpls_nextHop_create
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
 * Note:
 */
extern int32
dal_mango_mpls_nextHop_create(uint32 unit, rtk_mpls_nextHop_t *pNh,
    rtk_l3_pathId_t *pPathId);

/* Function Name:
 *      dal_mango_mpls_nextHop_destroy
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
 * Note:
 */
extern int32
dal_mango_mpls_nextHop_destroy(uint32 unit, rtk_l3_pathId_t pathId);

/* Function Name:
 *      dal_mango_mpls_nextHop_get
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
 * Note:
 */
extern int32
dal_mango_mpls_nextHop_get(uint32 unit, rtk_l3_pathId_t pathId,
    rtk_mpls_nextHop_t *pNh);

/* Function Name:
 *      dal_mango_mpls_nextHop_set
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
 * Note:
 */
extern int32
dal_mango_mpls_nextHop_set(uint32 unit, rtk_l3_pathId_t pathId,
    rtk_mpls_nextHop_t *pNh);

/* encapsulation */
/* Function Name:
 *      dal_mango_mpls_encap_create
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
 * Note:
 *      (1) flags only support:
 *          - RTK_MPLS_FLAG_NONE,
 *          - RTK_MPLS_FLAG_TTL_ASSIGN
 *          - RTK_MPLS_FLAG_NEXTLABEL
 *      (2) labelAct only support:
 *          - RTK_MPLS_LABELACT_PUSH
 *          - RTK_MPLS_LABELACT_SWAP
 */
extern int32
dal_mango_mpls_encap_create(uint32 unit, rtk_mpls_encap_t *pEncap,
    rtk_mpls_entryId_t *pEncapId);

/* Function Name:
 *      dal_mango_mpls_encap_destroy
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
 * Note:
 */
extern int32
dal_mango_mpls_encap_destroy(uint32 unit, rtk_mpls_entryId_t encapId);

/* Function Name:
 *      dal_mango_mpls_encap_get
 * Description:
 *      Get an MPLS encapsulation entry by encapsulation ID
 * Input:
 *      unit     - unit id
 *      encapId  - MPLS encapsulation entry ID
 * Output:
 *      pEncap   - pointer to MPLS encapsulation entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_NOT_ALLOWED  - the module is not enabled
 * Note:
 */
extern int32
dal_mango_mpls_encap_get(uint32 unit, rtk_mpls_entryId_t encapId,
    rtk_mpls_encap_t *pEncap);

/* Function Name:
 *      dal_mango_mpls_encap_set
 * Description:
 *      Set an MPLS encapsulation entry by encapsulation ID
 * Input:
 *      unit     - unit id
 *      encapId  - MPLS encapsulation entry ID
 * Output:
 *      pEncap   - pointer to MPLS encapsulation entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_NOT_ALLOWED  - the module is not enabled
 * Note:
 *      (1) flags only support:
 *          - RTK_MPLS_FLAG_NONE,
 *          - RTK_MPLS_FLAG_TTL_ASSIGN
 *          - RTK_MPLS_FLAG_NEXTLABEL
 *      (2) labelAct only support:
 *          - RTK_MPLS_LABELACT_PUSH
 *          - RTK_MPLS_LABELACT_SWAP
 */
extern int32
dal_mango_mpls_encap_set(uint32 unit, rtk_mpls_entryId_t encapId,
    rtk_mpls_encap_t *pEncap);

/* Function Name:
 *      dal_mango_mpls_encapId_find
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
 * Note:
 */
extern int32
dal_mango_mpls_encapId_find(uint32 unit, rtk_mpls_encap_t *pEncap,
    rtk_mpls_entryId_t *pEncapId);

/* decapsulation */
/* Function Name:
 *      dal_mango_mpls_hashAlgo_get
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
 * Note:
 *      None
 */
extern int32
dal_mango_mpls_hashAlgo_get(uint32 unit, uint8 *pHashAlgo);

/* Function Name:
 *      dal_mango_mpls_hashAlgo_set
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
 * Note:
 *      The valid input is 0 and 1. 0 mean hash_algo_0; 1 mean hash_algo_1.
 */
extern int32
dal_mango_mpls_hashAlgo_set(uint32 unit, uint8 hashAlgo);

/* Function Name:
 *      dal_mango_mpls_decap_create
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
 * Note:
 *      (1) flags only support:
 *          - RTK_MPLS_FLAG_NONE,
 *          - RTK_MPLS_FLAG_TTL_INHERIT
 *          - RTK_MPLS_FLAG_TC_INHERIT
 *          - RTK_MPLS_FLAG_ECMP
 *          - RTK_MPLS_FLAG_INTPRI_ASSIGN
 *      (2) labelAct only support:
 *          - RTK_MPLS_LABELACT_POP,
 *          - RTK_MPLS_LABELACT_NH
 */
extern int32
dal_mango_mpls_decap_create(uint32 unit, rtk_mpls_decapEntry_t *pDecap,
    rtk_mpls_entryId_t *pDecapId);

/* Function Name:
 *      dal_mango_mpls_decap_destroy
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
 * Note:
 */
extern int32
dal_mango_mpls_decap_destroy(uint32 unit, rtk_mpls_entryId_t decapId);

/* Function Name:
 *      dal_mango_mpls_decap_get
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
 * Note:
 */
extern int32
dal_mango_mpls_decap_get(uint32 unit, rtk_mpls_entryId_t decapId,
    rtk_mpls_decapEntry_t *pDecap);

/* Function Name:
 *      dal_mango_mpls_decap_set
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
 * Note:
 *      (1) flags only support:
 *          - RTK_MPLS_FLAG_NONE,
 *          - RTK_MPLS_FLAG_TTL_INHERIT
 *          - RTK_MPLS_FLAG_TC_INHERIT
 *          - RTK_MPLS_FLAG_ECMP
 *          - RTK_MPLS_FLAG_INTPRI_ASSIGN
 *      (2) labelAct only support:
 *          - RTK_MPLS_LABELACT_POP,
 *          - RTK_MPLS_LABELACT_NH
 */
extern int32
dal_mango_mpls_decap_set(uint32 unit, rtk_mpls_entryId_t decapId,
    rtk_mpls_decapEntry_t *pDecap);

/* Function Name:
 *      dal_mango_mpls_decapId_find
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
 * Note:
 */
extern int32
dal_mango_mpls_decapId_find(uint32 unit, rtk_mpls_decapEntry_t *pDecap,
    rtk_mpls_entryId_t *pDecapId);

/* Function Name:
 *      dal_mango_mpls_egrTcMap_get
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
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_DROP_PRECEDENCE  - invalid drop precedence
 * Note:
 *      None
 */
extern int32
dal_mango_mpls_egrTcMap_get(uint32 unit, rtk_mpls_egrTcMapSrc_t *pSrc, uint8 *pTc);

/* Function Name:
 *      dal_mango_mpls_egrTcMap_set
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
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - Invalid input
 *      RT_ERR_DROP_PRECEDENCE  - invalid drop precedence
 * Note:
 *      None
 */
extern int32
dal_mango_mpls_egrTcMap_set(uint32 unit, rtk_mpls_egrTcMapSrc_t *pSrc, uint8 tc);

/* Function Name:
 *      dal_mango_mpls_nextHop_create_id
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
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_NOT_ALLOWED  - the module is not enabled
 * Note:
 */
extern int32
dal_mango_mpls_nextHop_create_id(uint32 unit, rtk_mpls_nextHop_t *pNh,
    rtk_l3_pathId_t pathId);

/* Function Name:
 *      dal_mango_mpls_encap_create_id
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
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_NOT_ALLOWED  - the module is not enabled
 * Note:
 *      (1) flags only support:
 *          - RTK_MPLS_FLAG_NONE,
 *          - RTK_MPLS_FLAG_TTL_ASSIGN
 *          - RTK_MPLS_FLAG_NEXTLABEL
 *      (2) labelAct only support:
 *          - RTK_MPLS_LABELACT_PUSH
 *          - RTK_MPLS_LABELACT_SWAP
 */
extern int32
dal_mango_mpls_encap_create_id(uint32 unit, rtk_mpls_encap_t *pEncap,
    rtk_mpls_entryId_t encapId);

#endif  /* __DAL_MANGO_MPLS_H__ */
