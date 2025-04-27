/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Definition those public IPMCAST APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *            1) IPMCAST routing
 *
 */
#ifndef __DAL_LONGAN_IPMC_H__
#define __DAL_LONGAN_IPMC_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/l3.h>
#include <rtk/ipmcast.h>
#include <dal/dal_mapper.h>
#include <dal/longan/dal_longan_l3.h>

/*
 * Symbol Definition
 */
#define DAL_LONGAN_IPMCAST_L3_ENTRY_MAX          (3072)
#define DAL_LONGAN_IPMCAST_STAT_MONT_ENTRY_NUM   (4)
#define DAL_LONGAN_IPMCAST_INVALID_HASH_IDX     (0xFFF)

/*
 * Data Declaration
 */

/*
 * Macro Declaration
 */


/*
 * Function Declaration
 */
#if LONGAN_L3_ROUTE_IPMC_SIZE
extern int32 _dal_longan_ipmc_lpmRouteCnt_get(uint32 unit, uint32 *pIpv4Cnt, uint32 *pIpv6Cnt);
#endif


/* Function Name:
 *      _dal_longan_ipmc_oilIdx_update
 * Description:
 *      update host or route table entry's OIL index
 * Input:
 *      unit  - unit id
 *      l3EntryIdx - table index
 *      oilIdx - oil index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID            - invalid unit id
 *      RT_ERR_NOT_INIT           - the module is not initial
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 * Note:
 *
 */
extern int32
_dal_longan_ipmc_l3OilIdx_update(uint32 unit, uint32 l3EntryIdx, int32 oilIdx);


/* Function Name:
 *      _dal_longan_ipmc_l2PmskIdx_update
 * Description:
 *      update host or route table entry's l2 bridge forward index
 * Input:
 *      unit  - unit id
 *      l3EntryIdx - table index
 *      pmskIdx - l2 forward table index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID            - invalid unit id
 *      RT_ERR_NOT_INIT           - the module is not initial
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 * Note:
 *
 */
extern int32
_dal_longan_ipmc_l2PmskIdx_update(uint32 unit, uint32 l3EntryIdx, int32 pmskIdx);


/* Function Name:
 *      _dal_longan_ipmc_stkPmsk_update
 * Description:
 *      update host or route table entry's l3 stacking forward portmask
 * Input:
 *      unit  - unit id
 *      l3EntryIdx - table index
 *      pmskVal - stacking portmask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID            - invalid unit id
 *      RT_ERR_NOT_INIT           - the module is not initial
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 * Note:
 *      pmskVal is ASIC stacking port mask
 */
extern int32
_dal_longan_ipmc_stkPmsk_update(uint32 unit, uint32 l3EntryIdx, uint32 pmskVal);

/* Function Name:
 *      _dal_longan_ipmc_dip_update
 * Description:
 *      update ipmcast host table entry's dip
 * Input:
 *      unit  - unit id
 *      hw_entry_idx - table index
 *      pHostEntry - ipmcast host entry point
 *      flag - ip version
 * Output:
 *      pHostEntry - ipmcast host entry point
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID            - invalid unit id
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 * Note:
 *
 */
extern int32
_dal_longan_ipmc_dip_update(uint32 unit, uint32 hw_entry_idx, dal_longan_l3_hostEntry_t *pHostEntry, dal_longan_l3_api_flag_t flag);


/* Function Name:
 *      _dal_longan_ipmc_pkt_tx
 * Description:
 *      SS-432
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      Must initialize L3 module before calling any L3 APIs.
 */
extern int32 _dal_longan_ipmc_pkt_tx(uint32 unit);

/* Function Name:
 *      dal_longan_ipmcMapper_init
 * Description:
 *      Hook ipmc module of the specified device.
 * Input:
 *      pMapper - pointer of mapper
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Must Hook ipmc module before calling any ipmc APIs.
 */
extern int32
dal_longan_ipmcMapper_init(dal_mapper_t *pMapper);

/* Function Name:
 *      dal_longan_ipmc_init
 * Description:
 *      Initialize ipmcast module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      Must initialize ipmcast module before calling any ipmcast APIs.
 */
extern int32
dal_longan_ipmc_init(uint32 unit);


/* Function Name:
 *      dal_longan_ipmc_addr_init
 * Description:
 *      Initialize content of buffer of l3 ipmcast entry.
 * Input:
 *      unit     - unit id, not used
 *      pL3_mcastAddr - pointer to l3 ipmcast address
 * Output:
 *      pL3_mcastAddr - pointer to l3 ipmcast address
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_OUT_OF_RANGE -vrf id is out of range
 * Note:
 *
 */
extern int32
dal_longan_ipmc_addr_init(uint32 unit, rtk_ipmc_addr_t *pL3_mcastAddr);

/* Function Name:
 *      dal_longan_ipmc_addr_add
 * Description:
 *      Add an ipmcast entry into the L3 host or route table
 * Input:
 *      unit  - unit id
 *      pMcastAddr - pointer to rtk_ipmc_addr_t containing the basic inputs
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID            - invalid unit id
 *      RT_ERR_NOT_INIT           - the module is not initial
 *      RT_ERR_NULL_POINTER       - input parameter may be a null pointer
 *      RT_ERR_ENTRY_NOTFOUND     - specified entry not found (when try to replace)
 *      RT_ERR_IPMC_ADDR     - invalid multicast ip address
 * Note:
 *      (1) Basic required input parameters of the pRoute as input keys:
 *          vrf_id, ip_addr (either ipv4 or ipv6), and pathId (if RTK_L3_FLAG_NULL_INTF isn't set).
 *          RTK_IPMC_FLAG_REPLACE         - replace the existing entry with the new info
 *          RTK_IPMC_FLAG_NULL_INTF       - the route is destined to a null interface
 *                                        (path ID will not be referred)
 *          RTK_IPMC_FLAG_TTL_DEC_IGNORE  - ignore TTL decreasement
 *          RTK_IPMC_FLAG_TTL_CHK_IGNORE  - ignore TTL check
 *          RTK_IPMC_FLAG_QOS_ASSIGN      - assign a new internal priority (with qos_pri field)
 *          RTK_IPMC_FLAG_SIP - entry is dip + sip
 */
extern int32
dal_longan_ipmc_addr_add(uint32 unit, rtk_ipmc_addr_t *pMcastAddr);

/* Function Name:
 *      dal_longan_ipmc_addr_find
 * Description:
 *      Find an ipmcast entry based on IP address
 * Input:
 *      unit  - unit id
 *      pMcastAddr - pointer to the structure containing the basic inputs
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NOT_INIT       - the module is not initial
 *      RT_ERR_NULL_POINTER   - input parameter may be a null pointer
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      (1) Basic required input parameters of the pMcastAddras input keys:
 *          vrf_id and dst_ip_addr or with src_ip_addr (either ipv4 or ipv6).
 *          RTK_IPMC_FLAG_HIT_CLEAR   - clear the hit bit if it's set
 */
extern int32
dal_longan_ipmc_addr_find(uint32 unit, rtk_ipmc_addr_t *pMcastAddr);

/* Function Name:
 *      dal_longan_ipmc_addr_del
 * Description:
 *      Delete an entry from the L3 host and route table
 * Input:
 *      unit  - unit id
 *      pMcastAddr - pointer to rtk_ipmc_addr_t containing the basic inputs
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NOT_INIT       - the module is not initial
 *      RT_ERR_NULL_POINTER   - input parameter may be a null pointer
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      (1) Basic required input parameters of the pMcastAddr as input keys:
 *          dst_ip_addr and src_ip_addr(either ipv4 or ipv6).
 */
extern int32
dal_longan_ipmc_addr_del(uint32 unit, rtk_ipmc_addr_t *pMcastAddr);


/* Function Name:
 *      dal_longan_ipmc_addr_delAll
 * Description:
 *      Delete all ipmcast entry in host and route table entries
 * Input:
 *      unit  - unit id
 *      flag - configure flag
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 * Note:
 */
extern int32
dal_longan_ipmc_addr_delAll(uint32 unit, rtk_ipmc_flag_t flag);

/* Function Name:
 *      dal_longan_ipmc_addr_getNext
 * Description:
 *      Get the next valid ipmcast  entry (based on the base index)
 * Input:
 *      unit  - unit id
 *      flags - control flags
 *      pBase - pointer to the starting valid entry number to be searched
 * Output:
 *      pBase - pointer to the index of the returned entry (-1 means not found)
 *      pMcastAddr - ipmcast entry (if found)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Note:
 *      (1) Applicable flags:
 *          RTK_IPMC_FLAG_IPV6    - to indicate the entry format is IPv6 rather than IPv4
 *      (2) Use base index -1 to indicate to search from the beginging of L3 host table
 *      (3) If the returned index is -1, means not found the next valid entry
 */
extern int32
dal_longan_ipmc_addr_getNext(uint32 unit, rtk_ipmc_flag_t flags, int32 *pBase, rtk_ipmc_addr_t *pMcastAddr);


/* Function Name:
 *      dal_longan_ipmc_statMont_create
 * Description:
 *      Add an ipmc monitor counter entry for ipmcast entry
 * Input:
 *      unit  - unit id
 *      pStatMont - pointer to rtk_ipmc_statMont_t containing the basic inputs
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID            - invalid unit id
 *      RT_ERR_NOT_INIT           - the module is not initial
 *      RT_ERR_NULL_POINTER       - input parameter may be a null pointer
 *      RT_ERR_ENTRY_NOTFOUND     - specified entry not found (when try to replace)
 *      RT_ERR_TBL_FULL                 - table is full
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      (1) Basic required input parameters of the pStatMont as input keys:
 *          pStatMont->key ipmcast entry key
 *          pStatMont->mont_flags = RTK_IPMCAST_STAT_MONT_FLAG_BYTE_AND_PACKET wil use two counter entry
 */
extern int32
dal_longan_ipmc_statMont_create(uint32 unit, rtk_ipmc_statMont_t *pStatMont);

/* Function Name:
 *      dal_longan_ipmc_statMont_destroy
 * Description:
 *      Delete an ipmc monitor counter entry for ipmcast entry
 * Input:
 *      unit  - unit id
 *      pStatMont - pointer to rtk_ipmc_statMont_t containing the basic inputs
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID            - invalid unit id
 *      RT_ERR_NOT_INIT           - the module is not initial
 *      RT_ERR_NULL_POINTER       - input parameter may be a null pointer
 *      RT_ERR_ENTRY_NOTFOUND     - specified entry not found (when try to replace)
 *      RT_ERR_INPUT        - invalid input parameter

 * Note:
 *      (1) Basic required input parameters of the pStatMont as input keys:
 *          pStatMont->key ipmcast entry key
 *          pStatMont->mont_flags = 0,will also remove the counter entry
 *          delete an ipmcast entry,should destroy the monitor entry
 */
extern int32
dal_longan_ipmc_statMont_destroy(uint32 unit, rtk_ipmc_statMont_t *pStatMont);


/* Function Name:
 *      dal_longan_ipmc_statCntr_reset
 * Description:
 *      reset the ipmc monitor counter
 * Input:
 *      unit  - unit id
 *      pKey - pointer to rtk_ipmc_statKey_t of ipmcast entry key
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID            - invalid unit id
 *      RT_ERR_NOT_INIT           - the module is not initial
 *      RT_ERR_NULL_POINTER       - input parameter may be a null pointer
 *      RT_ERR_ENTRY_NOTFOUND     - specified entry not found (when try to replace)
 * Note:
 *
 */
extern int32
dal_longan_ipmc_statCntr_reset(uint32 unit, rtk_ipmc_statKey_t *pKey);


/* Function Name:
 *      dal_longan_ipmc_statCntr_get
 * Description:
 *      reset the ipmc monitor counter
 * Input:
 *      unit  - unit id
 *      pKey - pointer to rtk_ipmc_statKey_t of ipmcast entry key
 * Output:
 *      pCntr - pointer to counter
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID            - invalid unit id
 *      RT_ERR_NOT_INIT           - the module is not initial
 *      RT_ERR_NULL_POINTER       - input parameter may be a null pointer
 *      RT_ERR_ENTRY_NOTFOUND     - specified entry not found (when try to replace)
 * Note:
 *
 */
extern int32
dal_longan_ipmc_statCntr_get(uint32 unit, rtk_ipmc_statKey_t *pKey, rtk_ipmc_statCntr_t *pCntr);

/* Function Name:
 *      dal_longan_ipmc_globalCtrl_get
 * Description:
 *      Get the configuration of the specified control type
 * Input:
 *      unit - unit id
 *      type - control type
 * Output:
 *      pArg - pointer to the argurment
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Note:
 */
extern int32
dal_longan_ipmc_globalCtrl_get(uint32 unit, rtk_ipmc_globalCtrlType_t type, int32 *pArg);

/* Function Name:
 *      dal_longan_ipmc_globalCtrl_set
 * Description:
 *      Set the configuration of the specified control type
 * Input:
 *      unit - unit id
 *      type - control type
 *      arg  - argurment
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Note:
 */
extern int32
dal_longan_ipmc_globalCtrl_set(uint32 unit, rtk_ipmc_globalCtrlType_t type, int32 arg);

#endif /* __DAL_LONGAN_IPMC_H__ */

