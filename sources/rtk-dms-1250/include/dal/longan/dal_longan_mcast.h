/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Definition those public MCAST APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *            1) MCAST routing
 *
 */
#ifndef __DAL_LONGAN_MCAST_H__
#define __DAL_LONGAN_MCAST_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/l3.h>
#include <rtk/ipmcast.h>
#include <dal/dal_mapper.h>

/*
 * Symbol Definition
 */
#define DAL_LONGAN_MCAST_OIL_MAX                    (512)
#define DAL_LONGAN_MCAST_GROUP_MAX                    (1024)     //forwad table number
#define DAL_LONGAN_MCAST_FWD_IDX_MAX          (1024)
#define DAL_LONGAN_MCAST_L3_NODE_MAX                 (3072)      /* for IPMC entry */

#define DAL_LONGAN_MCAST_INVALID_FWD_IDX             (-1)
#define DAL_LONGAN_MCAST_INVALID_OIL_IDX             (-1)
#define DAL_LONGAN_MCAST_OIL_END                    (DAL_LONGAN_MCAST_INVALID_OIL_IDX)


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
 *      dal_longan_mcastMapper_init
 * Description:
 *      Hook mcast module of the specified device.
 * Input:
 *      pMapper - pointer of mapper
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Must Hook mcast module before calling any mcast APIs.
 */
extern int32
dal_longan_mcastMapper_init(dal_mapper_t *pMapper);

/* Function Name:
 *      dal_longan_mcast_init
 * Description:
 *      Initialize L3 mcast module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      Must initialize mcast module before calling any mcast APIs.
 */
extern int32
dal_longan_mcast_init(uint32 unit);

/* Function Name:
 *      _dal_longan_mcast_sem_lock
 * Description:
 *      lock multicast APIs' semaphore
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Must initialize mcast module before calling any mcast APIs.
 *      This is an API called by sub-modules (IPMCAST).
 */
extern int32
_dal_longan_mcast_sem_lock(uint32 unit);

/* Function Name:
 *      _dal_longan_mcast_sem_unlock
 * Description:
 *      unlock multicast APIs' semaphore
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Must initialize mcast module before calling any mcast APIs.
 *      This is an API called by sub-modules (IPMCAST).
 */
extern int32
_dal_longan_mcast_sem_unlock(uint32 unit);

/* Function Name:
 *      _dal_longan_mcast_fwdIdx_get
 * Description:
 *      get the group l2 forward table index and oil index
 * Input:
 *      unit  - unit id
 *      group  - mcast group
 * Output:
 *      pL2fwdIdx  - pointer of the l2 forward index
 *      pOilIdx  - pointer of the l3 oil index
 *      pStkPmskVal - pointer of stacking port mask value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      Must initialize mcast module before calling any mcast APIs.
 *      if not found, or invalid. *pL2fwdIdx = -1, *pOilIdx = -1
 */
extern int32
_dal_longan_mcast_fwdIdx_get(uint32 unit, rtk_mcast_group_t groupId, int32 *pL2fwdIdx, int32 *pOilIdx, uint32 *pStkPmskVal);
#if 0
/* Function Name:
 *      _dal_longan_mcast_ipmc_bindState_get
 * Description:
 *      get the binding state of the group to ipmc entry
 * Input:
 *      unit      - unit id
 *      groupId   - mcast group
 *      pBind     - binding state
 *      pEntryIdx - ipmc entry index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 */
 extern int32
_dal_longan_mcast_ipmc_bindState_get(uint32 unit, rtk_mcast_group_t groupId, rtk_enable_t *pBind, uint32 *pEntryIdx);
#endif
/* Function Name:
 *      _dal_longan_mcast_ipmc_bind
 * Description:
 *      bind the group to ipmc entry
 * Input:
 *      unit  - unit id
 *      group - mcast group
 *      pMcastAddr - pointer to the ipmc addr
 *      pHwId - pointer to the ipmc addr table index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID            - invalid unit id
 *      RT_ERR_NOT_INIT           - the module is not initial
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 * Note:
 *      if hwId >= DAL_LONGAN_L3_HOST_TBL_SIZE, it is route table index;
 */
extern int32
_dal_longan_mcast_ipmc_bind(uint32 unit,  rtk_mcast_group_t group, rtk_ipmc_addr_t *pMcastAddr, uint32 entryIdx);

/* Function Name:
 *      _dal_longan_mcast_ipmc_unbind
 * Description:
 *      un-bind the group to ipmc entry
 * Input:
 *      unit  - unit id
 *      group - mcast group
 *      pHwId - pointer to the ipmc addr table index
 * Output:
 *      pHwId - pointer to the ipmc addr table index
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID            - invalid unit id
 *      RT_ERR_NOT_INIT           - the module is not initial
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 * Note:
 *      if hwId >= DAL_LONGAN_L3_HOST_TBL_SIZE, it is route table index;
 */
extern int32
_dal_longan_mcast_ipmc_unbind(uint32 unit, rtk_mcast_group_t group, uint32 entryIdx);

/* Function Name:
 *      dal_longan_mcast_group_create
 * Description:
 *      Create a multicast group
 * Input:
 *      unit  - unit id
 *      flags - RTK_MCAST_FLAG_WITH_ID
 *      type  - multicast group type
 * Output:
 *      pGroup- pointer to multicast group
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 *
 */
extern int32
dal_longan_mcast_group_create(uint32 unit, uint32 flags, rtk_mcast_type_t type, rtk_mcast_group_t *pGroup);

/* Function Name:
 *      dal_longan_mcast_group_destroy
 * Description:
 *      Destrory the multicast group
 * Input:
 *      unit  - unit id
 *      group - multicast group
 * Output:
 *
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 *      RT_ERR_NOT_ALLOWED  - action not allowed to destroy
 * Note:
 */
extern int32
dal_longan_mcast_group_destroy(uint32 unit, rtk_mcast_group_t group);

/* Function Name:
 *      dal_longan_mcast_group_getNext
 * Description:
 *      Get next multicast group
 * Input:
 *      unit  - unit id
 *      type - mcast type
 *      pBase- start index
 * Output:
 *      pGroup - pointer to multicast group
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Note:
 *  *pBase = -1 indicate get first valid group
 *  type = 0 indicate include all type
 */
extern int32
dal_longan_mcast_group_getNext(uint32 unit, rtk_mcast_type_t  type, int32 *pBase, rtk_mcast_group_t *pGroup);

/* Function Name:
 *      dal_longan_mcast_nh_get
 * Description:
 *      Get next hop information of the multicast group
 * Input:
 *      unit  - unit id
 *      group - multicast group
 *      egrIf_max - number of rtk_mcast_egrif_t entry in pEgrIf buffer
 *      pEgrIf - buffer pointer
 * Output:
 *      pEgrIf_count - returned rtk_mcast_egrif_t entry number
 *      pEgrIf - pointer to the next hop information
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_INPUT              - invalid input parameter
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 *      RT_ERR_OUT_OF_RANGE - partition value is out of range
 * Note:
 */
extern int32
dal_longan_mcast_egrIf_get(uint32 unit, rtk_mcast_group_t group, int32 egrIf_max, rtk_mcast_egrif_t *pEgrIf, int32 *pEgrIf_count);


/* Function Name:
 *      dal_longan_mcast_egrIf_add
 * Description:
 *      Get next hop information of the multicast group
 * Input:
 *      unit  - unit id
 *      group - multicast group
 *      pEgrIf - buffer pointer of mcast nh
 * Output:
*       None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_INPUT              - invalid input parameter
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 *      RT_ERR_OUT_OF_RANGE - partition value is out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 * Note:
 */
extern int32
dal_longan_mcast_egrIf_add(uint32 unit, rtk_mcast_group_t group, rtk_mcast_egrif_t *pEgrIf);

/* Function Name:
 *      dal_longan_mcast_egrIf_del
 * Description:
 *      Delete a NH from multicast group replication list
 * Input:
 *      unit  - unit id
 *      group - multicast group
 *      pEgrIf  - pointer to multicast next hop information
 * Output:
 *
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_INPUT              - invalid input parameter
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 *      RT_ERR_OUT_OF_RANGE - partition value is out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 * Note:
 */
extern int32
dal_longan_mcast_egrIf_del(uint32 unit, rtk_mcast_group_t group, rtk_mcast_egrif_t *pEgrIf);


/* Function Name:
 *      rtk_mcast_egrIf_delAll
 * Description:
 *      Delete all NHs from multicast group replication list
 * Input:
 *      unit  - unit id
 *      group - multicast group
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_INPUT              - invalid input parameter
 *      RT_ERR_OUT_OF_RANGE - partition value is out of range
 * Note:
 */

extern int32
dal_longan_mcast_egrIf_delAll(uint32 unit, rtk_mcast_group_t group);

#endif /* __DAL_LONGAN_MCAST_H__ */

