
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
 * Purpose : Definition those public L2 APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) l2 address table
 *
 */
#ifndef __DAL_CYPRESS_L2_H__
#define __DAL_CYPRESS_L2_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <rtk/l2.h>
#include <dal/dal_mapper.h>

/*
 * Symbol Definition
 */

typedef enum dal_cypress_l2_entry_type_e
{
    L2_UNICAST = 0,
    L2_MULTICAST,
    IP4_MULTICAST,
    IP6_MULTICAST,
    L2_ENTRY_TYPE_END
} dal_cypress_l2_entry_type_t;

typedef enum dal_cypress_l2_indexType_e
{
    L2_IN_HASH = 0,
    L2_IN_CAM,
    L2_INDEXTYPE_END
} dal_cypress_l2_indexType_t;

typedef struct dal_cypress_l2_entry_s {
    dal_cypress_l2_entry_type_t  entry_type; /* unicast, l2 multicast, ip multicast */
    uint32           is_entry_exist;
    union {
        struct unicast_entry_s {
            rtk_fid_t   fid;
            rtk_mac_t   mac;
            rtk_port_t  port;
            uint32      aging;
            uint32      sablock;
            uint32      dablock;
            uint32      is_static;
            uint32      suspending;
            uint32      nh;
            rtk_vlan_t  agg_vid;
            uint32      vlan_target; /* combo with agg_vid */
            uint32      route_idx;   /* combo with agg_vid */
        } unicast;
        struct l2mcast_entry_s {
            rtk_vlan_t  rvid;
            rtk_mac_t   mac;
            uint32      index;
        } l2mcast;
        struct ipmcast_entry_mc_ip_s {
            rtk_vlan_t  rvid;
            rtk_mac_t   mac;
            uint32      index;
        } ipmcast_mc_ip;
        struct ipmcast_entry_ip_mc_sip_s {
            rtk_vlan_t  rvid;
            ipaddr_t    dip;
            ipaddr_t    sip;
            uint32      index;
        } ipmcast_ip_mc_sip;
        struct ipmcast_entry_ip_mc_s {
            rtk_vlan_t  rvid;
            ipaddr_t    dip;
            uint32      index;
        } ipmcast_ip_mc;
    };
} dal_cypress_l2_entry_t;

typedef struct dal_cypress_l2_index_s {
    uint32  index_type;
    uint32  index;
    uint32  hashdepth;      /* only useful when entry is in hash table */
} dal_cypress_l2_index_t;


/*
 * Data Declaration
 */

/*
 * Macro Definition
 */


/*
 * Function Declaration
 */

/* Function Name:
 *      dal_cypress_l2Mapper_init
 * Description:
 *      Hook l2 module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must Hook l2 module before calling any l2 APIs.
 */
extern int32
dal_cypress_l2Mapper_init(dal_mapper_t *pMapper);
/* Module Name    : L2     */
/* Sub-module Name: Global */

/* Function Name:
 *      dal_cypress_l2_init
 * Description:
 *      Initialize l2 module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize l2 module before calling any l2 APIs.
 */
extern int32
dal_cypress_l2_init(uint32 unit);

/* Function Name:
 *      dal_cypress_l2_getL2EntryfromHash
 * Description:
 *      Get L2 Entry from Chip
 * Input:
 *      unit      - unit id
 *      hashKey   - Hash Key for this Entry
 *      location  - Entry location in Hash Bucket
 *      pL2_entry - L2 entry used to do search
 *      pIsValid  - Is valid entry
 *
 * Output:
 *      pL2_entry - L2 entry
 *      pIsValid  - Is valid or invalid entry
 *                    TRUE: valid entry
 *                    FALSE: invalid entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_VLAN_VID - invalid vid
 * Note:
 *      We can not distinguish an entry between IP multicast in L2 mode and L2 multicast
 */
extern int32
dal_cypress_l2_getL2EntryfromHash(uint32 unit, uint32 hashkey, uint32 location, dal_cypress_l2_entry_t *pL2_entry, uint32 *pIsValid);

/* Function Name:
 *      dal_cypress_l2_flushLinkDownPortAddrEnable_get
 * Description:
 *      Get HW flush linkdown port mac configuration of the specified device.
 * Input:
 *      unit     - unit id
 * Output:
 *      *pEnable - pointer buffer of state of HW clear linkdown port mac
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_l2_flushLinkDownPortAddrEnable_get(uint32 unit, rtk_enable_t *pEnable);


/* Function Name:
 *      dal_cypress_l2_flushLinkDownPortAddrEnable_set
 * Description:
 *      Set HW flush linkdown port mac configuration of the specified device.
 * Input:
 *      unit   - unit id
 *      enable - configure value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT   - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_cypress_l2_flushLinkDownPortAddrEnable_set(uint32 unit, rtk_enable_t enable);

/* Function Name:
 *      dal_cypress_l2_ucastAddr_flush
 * Description:
 *      Flush unicast address
 * Input:
 *      unit    - unit id
 *      pConfig - flush config
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_VLAN_VID     - invalid vlan id
 * Note:
 *      None
 */
extern int32
dal_cypress_l2_ucastAddr_flush(uint32 unit, rtk_l2_flushCfg_t *pConfig);

/* Function Name:
 *      dal_cypress_l2_learningCnt_get
 * Description:
 *      Get the mac learning counts of the specified device.
 * Input:
 *      unit      - unit id
 * Output:
 *      *pMac_cnt - pointer buffer of mac learning counts of the port
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      1. The mac learning counts only calculate dynamic mac numbers.
 */
extern int32
dal_cypress_l2_learningCnt_get(uint32 unit, uint32 *pMac_cnt);

/* Function Name:
 *      dal_cypress_l2_limitLearningCnt_get
 * Description:
 *      Get the maximum mac learning counts of the specified device.
 * Input:
 *      unit      - unit id
 * Output:
 *      *pMac_cnt - pointer buffer of maximum mac learning counts
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      1. The maximum mac learning counts only limit for dynamic learning mac
 *      address, not apply to static mac address.
 *      2. Set the mac_cnt to 0 mean disable learning in the system.
 */
extern int32
dal_cypress_l2_limitLearningCnt_get(uint32 unit, uint32 *pMac_cnt);

/* Function Name:
 *      dal_cypress_l2_limitLearningCnt_set
 * Description:
 *      Set the maximum mac learning counts of the specified device.
 * Input:
 *      unit    - unit id
 *      mac_cnt - maximum mac learning counts
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_LIMITED_L2ENTRY_NUM - invalid limited L2 entry number
 * Note:
 *      1. The maximum mac learning counts only limit for dynamic learning mac
 *      address, not apply to static mac address.
 *      2. Set the mac_cnt to 0 mean disable learning in the system.
 */
extern int32
dal_cypress_l2_limitLearningCnt_set(uint32 unit, uint32 mac_cnt);

/* Function Name:
 *      dal_cypress_l2_limitLearningCntAction_get
 * Description:
 *      Get the action when over learning maximum mac counts of the specified device.
 * Input:
 *      unit     - unit id
 * Output:
 *      *pAction - pointer buffer of action when over learning maximum mac counts
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      1. The action symbol as following
 *      - LIMIT_LEARN_CNT_ACTION_DROP
 *      - LIMIT_LEARN_CNT_ACTION_FORWARD
 *      - LIMIT_LEARN_CNT_ACTION_TO_CPU
 */
extern int32
dal_cypress_l2_limitLearningCntAction_get(uint32 unit, rtk_l2_limitLearnCntAction_t *pAction);

/* Function Name:
 *      dal_cypress_l2_limitLearningCntAction_set
 * Description:
 *      Set the action when over learning maximum mac counts of the specified device.
 * Input:
 *      unit   - unit id
 *      action - action when over learning maximum mac counts
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT   - invalid input parameter
 * Note:
 *      1. The action symbol as following
 *      - LIMIT_LEARN_CNT_ACTION_DROP
 *      - LIMIT_LEARN_CNT_ACTION_FORWARD
 *      - LIMIT_LEARN_CNT_ACTION_TO_CPU
 */
extern int32
dal_cypress_l2_limitLearningCntAction_set(uint32 unit, rtk_l2_limitLearnCntAction_t action);

/* Function Name:
 *      dal_cypress_l2_portLearningCnt_get
 * Description:
 *      Get the mac learning counts of the port from the specified device.
 * Input:
 *      unit     - unit id
 *      port     - port id
 * Output:
 *      pMac_cnt - pointer buffer of mac learning counts of the port
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      1. The mac learning counts only calculate dynamic mac numbers.
 */
extern int32
dal_cypress_l2_portLearningCnt_get(uint32 unit, rtk_port_t port, uint32 *pMac_cnt);

/* Function Name:
 *      dal_cypress_l2_portLimitLearningCnt_get
 * Description:
 *      Get the maximum mac learning counts of the port from the specified device.
 * Input:
 *      unit     - unit id
 *      port     - port id
 * Output:
 *      pMac_cnt - pointer buffer of maximum mac learning counts
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      1. The maximum mac learning counts only limit for dynamic learning mac
 *      address, not apply to static mac address.
 *      2. Set the mac_cnt to 0 mean disable learning in the port.
 */
extern int32
dal_cypress_l2_portLimitLearningCnt_get(uint32 unit, rtk_port_t port, uint32 *pMac_cnt);

/* Function Name:
 *      dal_cypress_l2_portLimitLearningCnt_set
 * Description:
 *      Set the maximum mac learning counts of the port to the specified device.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      mac_cnt - maximum mac learning counts
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID             - invalid port id
 *      RT_ERR_LIMITED_L2ENTRY_NUM - invalid limited L2 entry number
 * Note:
 *      1. The maximum mac learning counts only limit for dynamic learning mac
 *      address, not apply to static mac address.
 *      2. Set the mac_cnt to 0 mean disable learning in the port.
 */
extern int32
dal_cypress_l2_portLimitLearningCnt_set(uint32 unit, rtk_port_t port, uint32 mac_cnt);

/* Function Name:
 *      dal_cypress_l2_limitLearningCntAction_get
 * Description:
 *      Get the action when over learning maximum mac counts of the port from the specified device.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pAction - pointer buffer of action when over learning maximum mac counts
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      1. The action symbol as following
 *      - LIMIT_LEARN_CNT_ACTION_DROP
 *      - LIMIT_LEARN_CNT_ACTION_FORWARD
 *      - LIMIT_LEARN_CNT_ACTION_TO_CPU
 */
extern int32
dal_cypress_l2_portLimitLearningCntAction_get(uint32 unit, rtk_port_t port, rtk_l2_limitLearnCntAction_t *pAction);

/* Function Name:
 *      dal_cypress_l2_limitLearningCntAction_set
 * Description:
 *      Set the action when over learning maximum mac counts of the port to the specified device.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      action - action when over learning maximum mac counts
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_INPUT   - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_cypress_l2_portLimitLearningCntAction_set(uint32 unit, rtk_port_t port, rtk_l2_limitLearnCntAction_t action);

/* Function Name:
 *      dal_cypress_l2_fidLimitLearningEntry_get
 * Description:
 *      Get FID MAC limit entry.
 * Input:
 *      unit              - unit id
 *      fid_macLimit_idx  - index of FID MAC limit entry
 * Output:
 *      pFidMacLimitEntry - pointer to MAC limit entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_OUT_OF_RANGE     - input parameter out of range
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      Forwarding action is as following
 *      - LIMIT_LEARN_CNT_ACTION_DROP
 *      - LIMIT_LEARN_CNT_ACTION_FORWARD
 *      - LIMIT_LEARN_CNT_ACTION_TO_CPU
 */
extern int32
dal_cypress_l2_fidLimitLearningEntry_get(uint32 unit, uint32 fid_macLimit_idx, rtk_l2_fidMacLimitEntry_t *pFidMacLimitEntry);

/* Function Name:
 *      dal_cypress_l2_fidLimitLearningEntry_set
 * Description:
 *      Set FID MAC limit entry.
 * Input:
 *      unit              - unit id
 *      fid_macLimit_idx  - index of FID MAC limit entry
 *      pFidMacLimitEntry - MAC limit entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 *      RT_ERR_OUT_OF_RANGE     - input parameter out of range
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_VLAN_VID         - invalid vlan id
 * Note:
 *      Forwarding action is as following
 *      - LIMIT_LEARN_CNT_ACTION_DROP
 *      - LIMIT_LEARN_CNT_ACTION_FORWARD
 *      - LIMIT_LEARN_CNT_ACTION_TO_CPU
 *      - LIMIT_LEARN_CNT_ACTION_COPY_TO_CPU
 */
extern int32
dal_cypress_l2_fidLimitLearningEntry_set(uint32 unit, uint32 fid_macLimit_idx, rtk_l2_fidMacLimitEntry_t *pFidMacLimitEntry);

/* Function Name:
 *      dal_cypress_l2_fidLearningCnt_get
 * Description:
 *      Get number of learned MAC addresses on specified fid.
 * Input:
 *      unit              - unit id
 *      fid_macLimit_idx  - index of FID MAC limit entry
 * Output:
 *      pNum - number of learned MAC addresses
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_OUT_OF_RANGE     - input parameter out of range
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_l2_fidLearningCnt_get(uint32 unit, uint32 fid_macLimit_idx, uint32 *pNum);

/* Function Name:
 *      dal_cypress_l2_fidLearningCnt_reset
 * Description:
 *      Reset number of learned MAC addresses on specified entry of fid MAC limit.
 * Input:
 *      unit             - unit id
 *      fid_macLimit_idx - index of FID MAC limit entry
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
extern int32
dal_cypress_l2_fidLearningCnt_reset(uint32 unit, uint32 fid_macLimit_idx);

/* Function Name:
 *      dal_cypress_l2_fidLearningCntAction_get
 * Description:
 *      Get the action when over learning maximum mac counts on specified entry of fid MAC limit.
 * Input:
 *      unit                - unit id
 *      fid_macLimit_idx    - index of FID MAC limit entry
 * Output:
 *      pAction             - pointer buffer of action when over learning maximum mac counts
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_l2_fidLearningCntAction_get(uint32 unit, rtk_l2_limitLearnCntAction_t *pAction);

/* Function Name:
 *      dal_cypress_l2_fidLearningCntAction_set
 * Description:
 *      Set the action when over learning maximum mac counts on specified entry of fid MAC limit.
 * Input:
 *      unit             - unit id
 *      fid_macLimit_idx - index of FID MAC limit entry
 *      action           - action when over learning maximum mac counts
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_cypress_l2_fidLearningCntAction_set(uint32 unit, rtk_l2_limitLearnCntAction_t action);

/* Function Name:
 *      dal_cypress_l2_aging_get
 * Description:
 *      Get the dynamic address aging time from the specified device.
 * Input:
 *      unit         - unit id
 * Output:
 *      *pAging_time - pointer buffer of aging time
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Get aging_time as 0 mean disable aging mechanism. (seconds)
 */
extern int32
dal_cypress_l2_aging_get(uint32 unit, uint32 *pAging_time);


/* Function Name:
 *      dal_cypress_l2_aging_set
 * Description:
 *      Set the dynamic address aging time to the specified device.
 * Input:
 *      unit       - unit id
 *      aging_time - aging time
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT   - invalid input parameter
 * Note:
 *      1. apply aging_time as 0 mean disable aging mechanism.
 */
extern int32
dal_cypress_l2_aging_set(uint32 unit, uint32 aging_time);

/* Function Name:
 *      dal_cypress_l2_portAgingEnable_get
 * Description:
 *      Get the dynamic address aging out configuration of the specified port to the specified device.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - enable status of aging out
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_cypress_l2_portAgingEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_cypress_l2_portAgingEnable_set
 * Description:
 *      Set the dynamic address aging out configuration of the specified port to the specified device.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      enable  - enable status of aging out
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_cypress_l2_portAgingEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      dal_cypress_l2_hashAlgo_get
 * Description:
 *      Get hash algorithm of layer2 table.
 * Input:
 *      unit        - unit id
 * Output:
 *      pHash_algo  - pointer to hash algorithm of layer2 switching
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_BUCKET_ID    - invalid bucket id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      none.
 */
extern int32
dal_cypress_l2_hashAlgo_get(uint32 unit, uint32 *pHash_algo);

/* Function Name:
 *      dal_cypress_l2_hashAlgo_set
 * Description:
 *      Set hash algorithm of layer2 table.
 * Input:
 *      unit        - unit id
 * Output:
 *      pHash_algo  - pointer to hash algorithm of layer2 switching
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_BUCKET_ID    - invalid bucket id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      none.
 */
extern int32
dal_cypress_l2_hashAlgo_set(uint32 unit, uint32 hash_algo);

/* Function Name:
 *      dal_cypress_l2_bucketHashAlgo_get
 * Description:
 *      Get bucket hash algorithm of layer2 table.
 * Input:
 *      unit        - unit id
 *      bucket      - bucket id
 * Output:
 *      pHash_algo  - pointer to hash algorithm of layer2 switching
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_BUCKET_ID    - invalid bucket id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      bucket can only be 0.
 */
extern int32
dal_cypress_l2_bucketHashAlgo_get(uint32 unit, uint32 bucket, uint32 *pHash_algo);

/* Function Name:
 *      dal_cypress_l2_bucketHashAlgo_set
 * Description:
 *      Set bucket hash algorithm of layer2 table.
 * Input:
 *      unit        - unit id
 *      bucket      - bucket id
 *      hash_algo   - hash algorithm of layer2 switching
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_BUCKET_ID    - invalid bucket id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      The valid bucket can only be 0, and hash_algo is 0 and 1
 */
extern int32
dal_cypress_l2_bucketHashAlgo_set(uint32 unit, uint32 bucket, uint32 hash_algo);

/* Function Name:
 *      dal_cypress_l2_portNewMacOp_get
 * Description:
 *      Get learning mode and forwarding action of new learned address on specified port.
 * Input:
 *      unit       - unit id
 *      port       - port id
 * Output:
 *      pLrnMode   - pointer to learning mode
 *      pFwdAction - pointer to forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_COPY2CPU
 *
 *      Learning mode is as following
 *      - HARDWARE_LEARNING
 *      - SOFTWARE_LEARNING
 *      - NOT_LEARNING
 */
extern int32
dal_cypress_l2_portNewMacOp_get(uint32 unit, rtk_port_t port, rtk_l2_newMacLrnMode_t *pLrnMode, rtk_action_t *pFwdAction);

/* Function Name:
 *      dal_cypress_l2_portNewMacOp_set
 * Description:
 *      Set learning mode and forwarding action of new learned address on specified port.
 * Input:
 *      unit      - unit id
 *      port      - port id
 *      lrnMode   - learning mode
 *      fwdAction - forwarding action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_FWD_ACTION - invalid forwarding action
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_COPY2CPU
 *
 *      Learning mode is as following
 *      - HARDWARE_LEARNING
 *      - SOFTWARE_LEARNING
 *      - NOT_LEARNING
 */
extern int32
dal_cypress_l2_portNewMacOp_set(uint32 unit, rtk_port_t port, rtk_l2_newMacLrnMode_t lrnMode, rtk_action_t fwdAction);


/* Module Name    : L2      */
/* Sub-module Name: Unicast */

/* Function Name:
 *      dal_cypress_l2_addr_init
 * Description:
 *      Initialize content of buffer of L2 entry.
 *      Will fill vid ,MAC address and reset other field of L2 entry.
 * Input:
 *      unit     - unit id
 *      vid      - vlan id
 *      pMac     - MAC address
 *      pL2_addr - L2 entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_VLAN_VID     - invalid vlan id
 *      RT_ERR_MAC          - invalid mac address
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Applicable:
 *      8390
 * Note:
 *      Need to initialize L2 entry before add it.
 */
extern int32
dal_cypress_l2_addr_init(
    uint32              unit,
    rtk_vlan_t          vid,
    rtk_mac_t           *pMac,
    rtk_l2_ucastAddr_t  *pL2_addr);

/* Function Name:
 *      dal_cypress_l2_addr_add
 * Description:
 *      Add L2 entry to ASIC.
 * Input:
 *      unit      - unit id
 *      pL2_addr - L2 entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_VLAN_VID         - invalid vlan id
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_MAC               - invalid mac address
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      Need to initialize L2 entry before add it.
 */
extern int32
dal_cypress_l2_addr_add(uint32 unit, rtk_l2_ucastAddr_t *pL2_addr);


/* Function Name:
 *      dal_cypress_l2_addr_del
 * Description:
 *      Delete a L2 unicast address entry from the specified device.
 * Input:
 *      unit   - unit id
 *      vid    - vlan id
 *      *pMac  - mac address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_VLAN_VID          - invalid vid
 *      RT_ERR_MAC               - invalid mac address
 *      RT_ERR_L2_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      1. vid is same as fid in IVL mode.
 *      2. For IVL and SVL co-work mode, need to discuss API late.
 */
extern int32
dal_cypress_l2_addr_del(uint32 unit, rtk_vlan_t vid, rtk_mac_t *pMac);


/* Function Name:
 *      dal_cypress_l2_addr_get
 * Description:
 *      Get a L2 unicast address entry from the specified device.
 * Input:
 *      unit      - unit id
 *      *pL2_data - structure of l2 address data
 * Output:
 *      *pL2_data - structure of l2 address data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_VLAN_VID          - invalid vid
 *      RT_ERR_MAC               - invalid mac address
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_L2_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      1. vid is same as fid in IVL mode.
 *      2. For IVL and SVL co-work mode, need to discuss API late.
 *      3. The *pL2_data.vid and *pL2_data.mac is input key
 *      4. The *pL2_data.port, *pL2_data.auth, *pL2_data.sa_block,
 *         *pL2_data.da_block and *pL2_data.is_static is output.
 */
extern int32
dal_cypress_l2_addr_get(uint32 unit, rtk_l2_ucastAddr_t *pL2_data);


/* Function Name:
 *      dal_cypress_l2_addr_set
 * Description:
 *      Update content of L2 entry.
 * Input:
 *      unit     - unit id
 *      pL2_addr - L2 entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_VLAN_VID         - invalid vlan id
 *      RT_ERR_MAC               - invalid mac address
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_cypress_l2_addr_set(uint32 unit, rtk_l2_ucastAddr_t *pL2_addr);


/* Function Name:
 *      dal_cypress_l2_addr_delAll
 * Description:
 *      Delete all L2 unicast address entry from the specified device.
 * Input:
 *      unit           - unit id
 *      include_static - include static mac or not?
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
dal_cypress_l2_addr_delAll(uint32 unit, uint32 include_static);


/* Function Name:
 *      dal_cypress_l2_nextValidAddr_get
 * Description:
 *      Get next valid L2 unicast address entry from the specified device.
 * Input:
 *      unit                - unit id
 *      *pScan_idx          - currently scan index of l2 table to get next.
 *      include_static      - the get type, include static mac or not.
 * Output:
 *      *pL2_data           - structure of l2 address data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 * Note:
 *      1. The function will skip valid l2 multicast and ip multicast entry and
 *         reply next valid L2 unicast address is based on index order of l2 table.
 *      2. Please input -1 for get the first entry of l2 table.
 *      3. The *pScan_idx is the input and also is the output argument.
 */
extern int32
dal_cypress_l2_nextValidAddr_get(
    uint32              unit,
    int32               *pScan_idx,
    uint32              include_static,
    rtk_l2_ucastAddr_t  *pL2_data);


/* Function Name:
 *      dal_cypress_l2_nextValidMcastAddr_get
 * Description:
 *      Get next valid L2 multicast address entry from the specified device.
 * Input:
 *      unit                - unit id
 *      *pScan_idx          - currently scan index of l2 table to get next.
 * Output:
 *      *pL2_data           - structure of l2 address data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_L2_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      1. The function will skip valid l2 unicast and ip multicast entry and
 *         reply next valid L2 multicast address is based on index order of l2 table.
 *      2. Please input -1 for get the first entry of l2 table.
 *      3. The *pScan_idx is the input and also is the output argument.
 */
extern int32
dal_cypress_l2_nextValidMcastAddr_get(
    uint32              unit,
    int32               *pScan_idx,
    rtk_l2_mcastAddr_t  *pL2_data);


/* Function Name:
 *      dal_cypress_l2_nextValidIpMcastAddr_get
 * Description:
 *      Get next valid L2 ip multicast address entry from the specified device.
 * Input:
 *      unit                - unit id
 *      *pScan_idx          - currently scan index of l2 table to get next.
 * Output:
 *      *pL2_data           - structure of l2 address data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_L2_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      1. The function will skip valid l2 unicast and multicast entry and
 *         reply next valid L2 ip multicast address is based on index order of l2 table.
 *      2. Please input -1 for get the first entry of l2 table.
 *      3. The *pScan_idx is the input and also is the output argument.
 */
extern int32
dal_cypress_l2_nextValidIpMcastAddr_get(
    uint32                  unit,
    int32                   *pScan_idx,
    rtk_l2_ipMcastAddr_t    *pL2_data);


/* Module Name    : L2           */
/* Sub-module Name: l2 multicast */

/* Function Name:
 *      dal_cypress_l2_mcastAddr_init
 * Description:
 *      Initialize content of buffer of L2 multicast entry.
 *      Will fill vid ,MAC address and reset other field of L2 multicast entry.
 * Input:
 *      unit        - unit id
 *      vid         - vlan id
 *      pMac        - MAC address
 *      pMcast_addr - L2 multicast entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_UNIT_ID          - invalid unit id
 *      RT_ERR_VLAN_VID         - invalid vlan id
 *      RT_ERR_MAC              - invalid mac address
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      Need to initialize L2 multicast entry before add it.
 */
extern int32
dal_cypress_l2_mcastAddr_init(uint32 unit, rtk_vlan_t vid, rtk_mac_t *pMac, rtk_l2_mcastAddr_t *pMcast_addr);

/* Function Name:
 *      dal_cypress_l2_mcastAddr_add
 * Description:
 *      Add L2 multicast entry to ASIC.
 * Input:
 *      unit       - unit id
 *      pMcast_addr - L2 multicast entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_VLAN_VID         - invalid vlan id
 *      RT_ERR_MAC              - invalid mac address
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      Need to initialize L2 multicast entry before add it.
 */
extern int32
dal_cypress_l2_mcastAddr_add(uint32 unit, rtk_l2_mcastAddr_t *pMcast_addr);


/* Function Name:
 *      dal_cypress_l2_mcastAddr_get
 * Description:
 *      Get L2 multicast entry based on specified vid and MAC address.
 * Input:
 *      unit       - unit id
 *      pMcast_addr - L2 multicast entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_VLAN_VID         - invalid vlan id
 *      RT_ERR_MAC              - invalid mac address
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_cypress_l2_mcastAddr_get(uint32 unit, rtk_l2_mcastAddr_t *pMcast_addr);

/* Function Name:
 *      dal_cypress_l2_mcastAddr_set
 * Description:
 *      Update content of L2 multicast entry.
 * Input:
 *      unit       - unit id
 * Output:
 *      pMcast_addr - pointer to L2 multicast entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_VLAN_VID     - invalid vlan id
 *      RT_ERR_MAC          - invalid mac address
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_l2_mcastAddr_set(uint32 unit, rtk_l2_mcastAddr_t *pMcast_addr);

/* Function Name:
 *      dal_cypress_l2_mcastAddr_del
 * Description:
 *      Delete a L2 multicast address entry from the specified device.
 * Input:
 *      unit  - unit id
 *      vid   - vlan id
 *      *pMac - multicast mac address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_NULL_POINTER   - input parameter may be null pointer
 *      RT_ERR_VLAN_VID       - invalid vlan id
 *      RT_ERR_MAC            - invalid mac address
 *      RT_ERR_L2_EMPTY_ENTRY - the entry is empty(invalid)
 * Note:
 *      None
 */
extern int32
dal_cypress_l2_mcastAddr_del(uint32 unit, rtk_vlan_t vid, rtk_mac_t *pMac);

/* Function Name:
 *      dal_cypress_l2_mcastAddr_addByIndex
 * Description:
 *      Add a L2 multicast address entry and multicast index to the specified device.
 * Input:
 *      unit                   - unit id
 *      vid                    - vlan id
 *      pMcast_addr            - content of L2 multicast address entry
 *      pMacast_addr->fwdIndex - index of portmask entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 *      RT_ERR_VLAN_VID             - invalid vlan id
 *      RT_ERR_MAC                  - invalid mac address
 *      RT_ERR_L2_MULTI_FWD_INDEX   - invalid index of multicast forwarding portmask
 *      RT_ERR_L2_ENTRY_EXIST       - the entry is already existed
 * Note:
 *      (1) pMacast_addr->fwdIndex is used for pointing to portmask entry if its value is larger than or equal to 0.
 *          In this case, pMacast_addr->portmask is not used.
 *
 *      (2) Driver automatically allocates a free portmask entry index and return it back to pMacast_addr->fwdIndex
 *          if pMacast_addr->fwdIndex is smaller than 0. In this case, pMacast_addr->portmask is used to
 *          configure the allocated portmask entry.
 */
extern int32
dal_cypress_l2_mcastAddr_addByIndex(uint32 unit, rtk_l2_mcastAddr_t *pMcast_addr);

/* Module Name    : L2           */
/* Sub-module Name: ip multicast */

/* Function Name:
 *      dal_cypress_l2_ipmcMode_get
 * Description:
 *      Get lookup mode of layer2 ip multicast switching.
 * Input:
 *      unit  - unit id
 * Output:
 *      pMode - pointer to lookup mode of layer2 ip multicast switching
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      Lookup mode of layer2 ip multicast switching for RTL8390 is as following
 *      - LOOKUP_ON_FVID_AND_MAC
 *      - LOOKUP_ON_DIP_AND_SIP
 *      - LOOKUP_ON_DIP_AND_FVID
 */
extern int32
dal_cypress_l2_ipmcMode_get(uint32 unit, rtk_l2_ipmcMode_t *pMode);

/* Function Name:
 *      dal_cypress_l2_ipmcMode_set
 * Description:
 *      Set lookup mode of layer2 ip multicast switching.
 * Input:
 *      unit - unit id
 *      mode - lookup mode of layer2 ip multicast switching
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      Lookup mode of layer2 ip multicast switching for RTL8390 is as following
 *      - LOOKUP_ON_FVID_AND_MAC
 *      - LOOKUP_ON_DIP_AND_SIP
 *      - LOOKUP_ON_DIP_AND_FVID
 */
extern int32
dal_cypress_l2_ipmcMode_set(uint32 unit, rtk_l2_ipmcMode_t mode);

/* Function Name:
 *      dal_cypress_l2_ipMcastAddrExt_init
 * Description:
 *      Initialize content of buffer of IP multicast entry.
 *      Will destination IP ,source IP and reset other field of IP multicast entry.
 * Input:
 *      unit          		- unit id
 *      pIpMcast_hashKey 	- the hash key to initialize content of buffer
 *      pIpMcast_addr 		- IP multicast entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_UNIT_ID          - invalid unit id
 *      RT_ERR_IPV4_ADDRESS     - Invalid IPv4 address
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      Need to initialize IP multicast entry before add it.
 */
extern int32
dal_cypress_l2_ipMcastAddrExt_init(uint32 unit, rtk_l2_ipMcastHashKey_t *pIpMcast_hashKey, rtk_l2_ipMcastAddr_t *pIpMcast_addr);

/* Function Name:
 *      dal_cypress_l2_ipMcastAddr_add
 * Description:
 *      Add IP multicast entry to ASIC.
 * Input:
 *      unit       - unit id
 *      pIpmcast_addr - IP multicast entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_L2_MULTI_FWD_INDEX   - invalid index of multicast forwarding portmask
 *      RT_ERR_VLAN_VID             - invalid VLAN ID
 *      RT_ERR_CHIP_NOT_SUPPORTED   - chip not support
 *      RT_ERR_L2_ENTRY_EXIST       - the entry is already existed
 * Note:
 *      Need to initialize IP multicast entry before add it.
 */
extern int32
dal_cypress_l2_ipMcastAddr_add(uint32 unit, rtk_l2_ipMcastAddr_t *pIpmcast_addr);


/* Function Name:
 *      dal_cypress_l2_ipMcastAddr_get
 * Description:
 *      Get IP multicast entry on specified dip and sip.
 * Input:
 *      unit       - unit id
 *      pIpmcast_addr - IP multicast entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_IPV4_ADDRESS     - Invalid IPv4 address
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Need to initialize IP multicast entry before add it.
 */
extern int32
dal_cypress_l2_ipMcastAddr_get(uint32 unit, rtk_l2_ipMcastAddr_t *pIpmcast_addr);

/* Function Name:
 *      dal_cypress_l2_ipMcastAddr_set
 * Description:
 *      Update content of IP multicast entry.
 * Input:
 *      unit       - unit id
 *      pIpmcast_addr - IP multicast entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_IPV4_ADDRESS - Invalid IPv4 address
 *      RT_ERR_VLAN_VID     - invalid vlan id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_CHIP_NOT_SUPPORTED   - chip not support
 * Note:
 *      None
 */
extern int32
dal_cypress_l2_ipMcastAddr_set(uint32 unit, rtk_l2_ipMcastAddr_t *pIpmcast_addr);


/* Function Name:
 *      dal_cypress_l2_ipMcastAddr_del
 * Description:
 *      Delete a L2 ip multicast address entry from the specified device.
 * Input:
 *      unit - unit id
 *      sip  - source ip address
 *      dip  - destination ip address
 *      vid  - vlan id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vlan id
 *      RT_ERR_L2_EMPTY_ENTRY - the entry is empty(invalid)
 *      RT_ERR_CHIP_NOT_SUPPORTED   - chip not support
 * Note:
 *      (1) In vlan unaware mode (SVL), the vid will be ignored, suggest to
 *         input vid=0 in vlan unaware mode.
 *      (2) In vlan aware mode (IVL), the vid is used.
 */
extern int32
dal_cypress_l2_ipMcastAddr_del(uint32 unit, ipaddr_t sip, ipaddr_t dip, rtk_vlan_t vid);

/* Function Name:
 *      dal_cypress_l2_ipMcastAddr_addByIndex
 * Description:
 *      Add a IP multicast address entry and multicast index to the specified device.
 * Input:
 *      unit                     - unit id
 *      vid                      - vlan id
 *      pIpMcast_addr            - content of IP multicast address entry
 * Output:
 *      pIpMacast_addr->fwdIndex - index of multicast forwarding entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 *      RT_ERR_L2_MULTI_FWD_INDEX   - invalid index of multicast forwarding portmask
 *      RT_ERR_VLAN_VID             - invalid VLAN ID
 *      RT_ERR_CHIP_NOT_SUPPORTED   - chip not support
 *      RT_ERR_L2_ENTRY_EXIST       - the entry is already existed
 * Note:
 *      If fwdIndex is larger than or equal to 0, will use fwdIndex as multicast index
 *          and won't config portmask.
 *
 *      If fwdIndex is smaller than 0, will allocate a free index and return it.
 *          It will also config portmask.
 */
extern int32
dal_cypress_l2_ipMcastAddr_addByIndex(uint32 unit, rtk_l2_ipMcastAddr_t *pIpMcast_addr);

/* Function Name:
 *      dal_cypress_l2_ipMcastAddrChkEnable_get
 * Description:
 *      Get DIP check status for IPv4 multicast packet.
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - pointer to DIP check status for IPv4 multicast packet
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None.
 */
extern int32
dal_cypress_l2_ipMcastAddrChkEnable_get(uint32 unit, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_cypress_l2_ipMcastAddrChkEnable_set
 * Description:
 *      Set DIP check status for IPv4 multicast packet.
 * Input:
 *      unit    - unit id
 *      enable  - DIP check status for IPv4 multicast packet
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_cypress_l2_ipMcastAddrChkEnable_set(uint32 unit, rtk_enable_t enable);

/* Function Name:
 *      dal_cypress_l2_ipMcstFidVidCompareEnable_get
 * Description:
 *      Get VID/FID comparison configuration of IP multicast entry.
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - pointer to FID/VID comparison status for IPv4 multicast packet
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_l2_ipMcstFidVidCompareEnable_get(uint32 unit, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_cypress_l2_ipMcstFidVidCompareEnable_set
 * Description:
 *      Get VID/FID comparison configuration of IP multicast entry.
 * Input:
 *      unit    - unit id
 *      enable  - configure value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_l2_ipMcstFidVidCompareEnable_set(uint32 unit, rtk_enable_t enable);

/* Function Name:
 *      dal_cypress_l2_ip6mcMode_get
 * Description:
 *      Get lookup mode of layer2 IPv6 multicast switching.
 * Input:
 *      unit  - unit id
 * Output:
 *      pMode - pointer to lookup mode of layer2 IPv6 multicast switching
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      Lookup mode of layer2 IPv6 multicast switching for RTL8390 is as following
 *      - LOOKUP_ON_FVID_AND_MAC
 *      - LOOKUP_ON_DIP_AND_SIP
 *      - LOOKUP_ON_DIP_AND_FVID
 */
extern int32
dal_cypress_l2_ip6mcMode_get(uint32 unit, rtk_l2_ipmcMode_t *pMode);

/* Function Name:
 *      dal_cypress_l2_ip6mcMode_set
 * Description:
 *      Set lookup mode of layer2 IPv6 multicast switching.
 * Input:
 *      unit - unit id
 *      mode - lookup mode of layer2 IPv6 multicast switching
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_CHIP_NOT_SUPPORTED   - chip not support
 * Note:
 *      Lookup mode of layer2 IPv6 multicast switching for RTL8390 is as following
 *      - LOOKUP_ON_FVID_AND_MAC
 *      - LOOKUP_ON_DIP_AND_SIP
 *      - LOOKUP_ON_DIP_AND_FVID
 */
extern int32
dal_cypress_l2_ip6mcMode_set(uint32 unit, rtk_l2_ipmcMode_t mode);

/* Function Name:
 *      dal_cypress_l2_ip6CareByte_get
 * Description:
 *      Get the hash care-byte of IPv6 DIP/SIP address. These bytes are used to compose the LUT hash key.
 * Input:
 *      unit        - unit id
 *      type        - type of care-byte
 * Output:
 *      pCareByte   - pointer to the care-byte of IPv6 DIP/SIP address
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_l2_ip6CareByte_get(uint32 unit, rtk_l2_ip6_careByte_type_t type, uint32 *pCareByte);

/* Function Name:
 *      dal_cypress_l2_ip6CareByte_set
 * Description:
 *      Set the hash care-byte of IPv6 DIP/SIP address. These bytes are used to compose the LUT hash key.
 * Input:
 *      unit        - unit id
 *      type        - type of care-byte
 *      careByte    - the care-byte of IPv6 DIP/SIP address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_l2_ip6CareByte_set(uint32 unit, rtk_l2_ip6_careByte_type_t type, uint32 careByte);

/* Function Name:
 *      dal_cypress_l2_ip6McastAddrExt_init
 * Description:
 *      Initialize content of buffer of IPv6 multicast entry.
 *      Will destination IP ,source IP (or vid) and reset other field of IPv6 multicast entry.
 * Input:
 *      unit          		- unit id
 *      pIp6Mcast_hashKey 	- the hash key to initialize content of buffer
 *      pIp6Mcast_addr    	- IPv6 multicast entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_UNIT_ID          - invalid unit id
 *      RT_ERR_IPV6_ADDRESS     - Invalid IPv6 address
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      Need to initialize IPv6 multicast entry before add it.
 */
int32
dal_cypress_l2_ip6McastAddrExt_init(uint32 unit, rtk_l2_ip6McastHashKey_t *pIp6Mcast_hashKey, rtk_l2_ip6McastAddr_t *pIp6Mcast_addr);

/* Function Name:
 *      dal_cypress_l2_ip6McastAddr_add
 * Description:
 *      Add IPv6 multicast entry to ASIC.
 * Input:
 *      unit            - unit id
 *      pIpmcast_addr   - IP multicast entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_IPV6_ADDRESS     - Invalid IPv6 address
 *      RT_ERR_VLAN_VID         - invalid vlan id
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      Need to initialize IP multicast entry before add it.
 */
extern int32
dal_cypress_l2_ip6McastAddr_add(uint32 unit, rtk_l2_ip6McastAddr_t *pIpmcast_addr);

/* Function Name:
 *      dal_cypress_l2_ip6McastAddr_get
 * Description:
 *      Get IP multicast entry on specified dip and sip.
 * Input:
 *      unit      - unit id
 *      pIpmcast_addr - IP multicast entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_IPV4_ADDRESS     - Invalid IPv4 address
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Need to initialize IP multicast entry before add it.
 */
extern int32
dal_cypress_l2_ip6McastAddr_get(uint32 unit, rtk_l2_ip6McastAddr_t *pIpmcast_addr);

/* Function Name:
 *      dal_cypress_l2_ip6McastAddr_set
 * Description:
 *      Update content of IP multicast entry.
 * Input:
 *      unit            - unit id
 *      pIpmcast_addr   - IP multicast entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_IPV6_ADDRESS - Invalid IPv4 address
 *      RT_ERR_VLAN_VID     - invalid vlan id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_cypress_l2_ip6McastAddr_set(uint32 unit, rtk_l2_ip6McastAddr_t *pIpmcast_addr);

/* Function Name:
 *      dal_cypress_l2_ip6McastAddr_del
 * Description:
 *      Delete a L2 ipv6 multicast address entry from the specified device.
 * Input:
 *      unit - unit id
 *      sip  - source ip address
 *      dip  - destination ip address
 *      vid  - vlan id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_L2_EMPTY_ENTRY - the entry is empty(invalid)
 * Note:
 *      1. In vlan unaware mode (SVL), the vid will be ignore, suggest to
 *         input vid=0 in vlan unaware mode.
 *      2. In vlan aware mode (IVL), the vid will be care.
 */
extern int32
dal_cypress_l2_ip6McastAddr_del(uint32 unit, rtk_ipv6_addr_t sip, rtk_ipv6_addr_t dip, rtk_vlan_t vid);

/* Function Name:
 *      dal_cypress_l2_ip6McastAddr_addByIndex
 * Description:
 *      Add a IPv6 multicast address entry and multicast index to the specified device.
 * Input:
 *      unit                     - unit id
 *      vid                      - vlan id
 *      pIpMcast_addr            - content of IP multicast address entry
 * Output:
 *      pIpMacast_addr->fwdIndex - index of multicast forwarding entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 *      RT_ERR_L2_MULTI_FWD_INDEX   - invalid index of multicast forwarding portmask
 * Note:
 *      If fwdIndex is larger than or equal to 0, will use fwdIndex as multicast index
 *          and won't config portmask.
 *
 *      If fwdIndex is smaller than 0, will allocate a free index and return it.
 *          It will also config portmask.
 */
extern int32
dal_cypress_l2_ip6McastAddr_addByIndex(uint32 unit, rtk_l2_ip6McastAddr_t *pIpMcast_addr);

/* Function Name:
 *      dal_cypress_l2_nextValidIp6McastAddr_get
 * Description:
 *      Get next valid L2 IPv6 multicast address entry from the specified device.
 * Input:
 *      unit      - unit id
 *      pScan_idx - currently scan index of l2 table to get next.
 * Output:
 *      pL2_data  - structure of l2 address data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_VLAN_VID          - invalid vid
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_L2_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      1. The function will skip valid l2 unicast and multicast entry and
 *         reply next valid L2 ip multicast address is based on index order of l2 table.
 *      2. Please input -1 for get the first entry of l2 table.
 *      3. The *pScan_idx is the input and also is the output argument.
 */
extern int32
dal_cypress_l2_nextValidIp6McastAddr_get(uint32 unit, int32 *pScan_idx, rtk_l2_ip6McastAddr_t *pL2_data);

/* Function Name:
 *      dal_cypress_l2_mcastFwdIndex_alloc
 * Description:
 *      Allocate index for multicast forwarding entry
 * Input:
 *      unit           - unit id
 *      pFwdIndex      - pointer to index of multicast forwarding entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER             - input parameter may be null pointer
 *      RT_ERR_L2_MULTI_FWD_INDEX       - invalid index of multicast forwarding entry
 *      RT_ERR_L2_MCAST_FWD_ENTRY_EXIST - index of forwarding entry is used.
 * Note:
 *      If *pFwdIndex is larger than or equal to 0, will use *pFwdIndex as multicast index.
 *      If *pFwdIndex is smaller than 0, will allocate a free index and return it.
 */
extern int32
dal_cypress_l2_mcastFwdIndex_alloc(
    uint32          unit,
    int32           *pFwdIndex);

/* Function Name:
 *      dal_cypress_l2_mcastFwdIndex_free
 * Description:
 *      Free index for multicast forwarding entry
 * Input:
 *      unit       - unit id
 *      index      - index of multicast forwarding entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_L2_MULTI_FWD_INDEX           - invalid index of multicast forwarding portmask
 *      RT_ERR_L2_MCAST_FWD_ENTRY_NOT_EXIST - index of forwarding entry is not exist
 * Note:
 */
extern int32
dal_cypress_l2_mcastFwdIndex_free(
    uint32          unit,
    int32           index);

/* Function Name:
 *      dal_cypress_l2_mcastFwdIndexFreeCount_get
 * Description:
 *      Get free count of multicast forwarding entry
 * Input:
 *      unit       - unit id
 *      pFreeCount - pointer to free count of multicast forwarding entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_l2_mcastFwdIndexFreeCount_get(uint32 unit, uint32 *pFreeCount);

/* Function Name:
 *      dal_cypress_l2_mcastFwdPortmask_set
 * Description:
 *      Get portmask of multicast forwarding entry
 * Input:
 *      unit       - unit id
 *      index      - index of multicast forwarding portmask
 *      *pPortmask - pointer buffer of ip multicast ports
 *      crossVlan  - cross vlan flag of ip multicast entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_L2_MULTI_FWD_INDEX   - invalid index of multicast forwarding portmask
 * Note:
 *      None
 */
extern int32
dal_cypress_l2_mcastFwdPortmask_set(
    uint32          unit,
    int32           index,
    rtk_portmask_t  *pPortmask,
    uint32          crossVlan);

/* Function Name:
 *      dal_cypress_l2_mcastFwdPortmask_get
 * Description:
 *      Get portmask of multicast forwarding entry
 * Input:
 *      unit        - unit id
 *      index       - index of multicast forwarding portmask
 * Output:
 *      *pPortmask  - pointer buffer of ip multicast ports
 *      *pCrossVlan - pointer of cross vlan flag
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_L2_MULTI_FWD_INDEX   - invalid index of multicast forwarding portmask
 * Note:
 *      None
 */
extern int32
dal_cypress_l2_mcastFwdPortmask_get(
    uint32          unit,
    int32           index,
    rtk_portmask_t  *pPortmask,
    uint32          *pCrossVlan);

/* Function Name:
 *      dal_cypress_l2_cpuMacAddr_add
 * Description:
 *      Add a CPU mac adress of the vlan id to the specified device.
 * Input:
 *      unit  - unit id
 *      vid   - vlan id
 *      *pMac - cpu mac address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_VLAN_VID - invalid vid
 * Note:
 *      None
 */
extern int32
dal_cypress_l2_cpuMacAddr_add(uint32 unit, rtk_vlan_t vid, rtk_mac_t *pMac);


/* Function Name:
 *      dal_cypress_l2_cpuMacAddr_del
 * Description:
 *      Delete a CPU mac adress of the vlan id from the specified device.
 * Input:
 *      unit  - unit id
 *      vid   - vlan id
 *      *pMac - cpu mac address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_VLAN_VID - invalid vid
 * Note:
 *      None
 */
extern int32
dal_cypress_l2_cpuMacAddr_del(uint32 unit, rtk_vlan_t vid, rtk_mac_t *pMac);

/* Function Name:
 *      dal_cypress_l2_portLegalPortMoveAction_get
 * Description:
 *      Get forwarding action when port moving happen on specified port which is learnt as dynamic entry.
 * Input:
 *      unit       - unit id
 *      port       - port id
 * Output:
 *      pFwdAction - pointer to forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_l2_portLegalPortMoveAction_get(uint32 unit, rtk_port_t port, rtk_action_t *pFwdAction);

/* Function Name:
 *      dal_cypress_l2_portLegalPortMoveAction_set
 * Description:
 *      Set forwarding action when legal port moving happen on specified port which is learnt as dynamic entry.
 * Input:
 *      unit      - unit id
 *      port      - port id
 *      fwdAction - forwarding action
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_l2_portLegalPortMoveAction_set(uint32 unit, rtk_l2_lookupMissType_t port, rtk_action_t fwdAction);

/* Function Name:
 *      dal_cypress_l2_legalPortMoveFlushAddrEnable_get
 * Description:
 *      Get the configuration of HW flush moved-port's mac of the specified device.
 * Input:
 *      unit        - unit id
 *      port        - port id
 * Output:
 *      pEnable     - enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_l2_legalPortMoveFlushAddrEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_cypress_l2_legalPortMoveFlushAddrEnable_set
 * Description:
 *      Set the configuration of HW flush moved-port's mac of the specified device.
 * Input:
 *      unit        - unit id
 *      port        - port id
 *      enable      - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_FWD_ACTION       - invalid forwarding action
 * Note:
 *      None
 */
extern int32
dal_cypress_l2_legalPortMoveFlushAddrEnable_set(uint32 unit, rtk_l2_lookupMissType_t port, rtk_enable_t enable);

/* Function Name:
 *      dal_cypress_l2_staticPortMoveAction_get
 * Description:
 *      Get forwarding action when port moving happen on specified port which is learnt as static entry.
 * Input:
 *      unit       - unit id
 *      port       - port id
 * Output:
 *      pFwdAction - pointer to forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_l2_staticPortMoveAction_get(uint32 unit, rtk_port_t port, rtk_action_t *pFwdAction);

/* Function Name:
 *      dal_cypress_l2_staticPortMoveAction_get
 * Description:
 *      Set forwarding action when legal port moving happen on specified port which is learnt as static entry.
 * Input:
 *      unit      - unit id
 *      port      - port id
 *      fwdAction - forwarding action
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_l2_staticPortMoveAction_set(uint32 unit, rtk_l2_lookupMissType_t port, rtk_action_t fwdAction);

/* Function Name:
 *      dal_cypress_l2_lookupMissFloodPortMask_get
 * Description:
 *      Get flooding port mask when unicast or multicast address lookup missed in L2 table.
 * Input:
 *      unit            - unit id
 *      type            - type of lookup miss
 * Output:
 *      pFlood_portmask - flooding port mask configuration when unicast/multicast lookup missed.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_l2_lookupMissFloodPortMask_get(uint32 unit, rtk_l2_lookupMissType_t type, rtk_portmask_t *pFlood_portmask);

/* Function Name:
 *      dal_cypress_l2_lookupMissFloodPortMask_setByIndex
 * Description:
 *      Set flooding port mask when unicast or multicast address lookup missed in L2 table.
 * Input:
 *      unit            - unit id
 *      type            - type of lookup miss
 *      idx             - index to multicast portmask table
 *      pFlood_portmask - flooding port mask configuration when unicast/multicast lookup missed.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_l2_lookupMissFloodPortMask_setByIndex(uint32 unit, rtk_l2_lookupMissType_t type, uint32 idx, rtk_portmask_t *pFlood_portmask);

/* Function Name:
 *      dal_cypress_l2_lookupMissFloodPortMask_add
 * Description:
 *      Add one port member to flooding port mask when unicast or multicast address lookup missed in L2 table.
 * Input:
 *      unit            - unit id
 *      type            - type of lookup miss
 *      flood_port      - port id that is going to be added in flooding port mask.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_l2_lookupMissFloodPortMask_add(uint32 unit, rtk_l2_lookupMissType_t type, rtk_port_t flood_port);

/* Function Name:
 *      dal_cypress_l2_lookupMissFloodPortMask_del
 * Description:
 *      Del one port member in flooding port mask when unicast or multicast address lookup missed in L2 table.
 * Input:
 *      unit            - unit id
 *      type            - type of lookup miss
 *      flood_port      - port id that is going to be added in flooding port mask.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_l2_lookupMissFloodPortMask_del(uint32 unit, rtk_l2_lookupMissType_t type, rtk_port_t flood_port);

/* Function Name:
 *      dal_cypress_l2_lookupMissFloodPortMaskIdx_get
 * Description:
 *      Get the entry index of forwarding table that is used as unicast/broadcast flooding port mask.
 * Input:
 *      unit 	- unit id
 *      type    - type of lookup miss
 * Output:
 *      pIdx	- flooding port mask configuration when unicast/multicast lookup missed.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_L2_PMSK_NOT_INIT - flooding portmask is not initialized
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      In 8390, flood-portmask get/set only supports DLF_TYPE_UCAST and DLF_TYPE_BCAST. For DLF_TYPE_MCAST,
 *      DLF_TYPE_IPMC and DLF_TYPE_IP6MC, please refer to rtk_vlan_profile_set()/rtk_vlan_profile_get().
 */
extern int32
dal_cypress_l2_lookupMissFloodPortMaskIdx_get(uint32 unit, rtk_l2_lookupMissType_t type, uint32 *pIdx);

/* Function Name:
 *      dal_cypress_l2_lookupMissFloodPortMaskIdx_set
 * Description:
 *      Set the entry index of forwarding table that is used as unicast/broadcast flooding port mask.
 * Input:
 *      unit 	- unit id
 *      type    - type of lookup miss
 *      idx		- flooding port mask configuration when unicast/multicast lookup missed.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_L2_PMSK_NOT_INIT - flooding portmask is not initialized
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      In 8390, flood-portmask get/set only supports DLF_TYPE_UCAST and DLF_TYPE_BCAST. For DLF_TYPE_MCAST,
 *      DLF_TYPE_IPMC and DLF_TYPE_IP6MC, please refer to rtk_vlan_profile_set()/rtk_vlan_profile_get().
 */
extern int32
dal_cypress_l2_lookupMissFloodPortMaskIdx_set(uint32 unit, rtk_l2_lookupMissType_t type, uint32 idx);

/* Function Name:
 *      dal_cypress_l2_portLookupMissAction_get
 * Description:
 *      Get forwarding action of specified port when destination address lookup miss.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      type    - type of lookup miss
 * Output:
 *      pAction - pointer to forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid type of lookup miss
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_l2_portLookupMissAction_get(uint32 unit, rtk_port_t port, rtk_l2_lookupMissType_t type, rtk_action_t *pAction);

/* Function Name:
 *      dal_cypress_l2_portLookupMissAction_set
 * Description:
 *      Set forwarding action of specified port when destination address lookup miss.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      type    - type of lookup miss
 *      action  - forwarding action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid type of lookup miss
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_FWD_ACTION       - invalid forwarding action
 * Note:
 *      None
 */
extern int32
dal_cypress_l2_portLookupMissAction_set(uint32 unit, rtk_port_t port, rtk_l2_lookupMissType_t type, rtk_action_t action);

/* Function Name:
 *      dal_cypress_l2_srcPortEgrFilterMask_get
 * Description:
 *      Get loopback filtering function on specified ports.
 * Input:
 *      unit             - unit id
 * Output:
 *      pFilter_portmask - ports which turn on loopback filtering function
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      The loopback(egress port == ingress port) packet is dropped if the function is enabled.
 */
extern int32
dal_cypress_l2_srcPortEgrFilterMask_get(uint32 unit, rtk_portmask_t *pFilter_portmask);

/* Function Name:
 *      dal_cypress_l2_srcPortEgrFilterMask_set
 * Description:
 *      Set loopback filtering function on specified ports.
 * Input:
 *      unit             - unit id
 *      pFilter_portmask - ports which turn on loopback filtering function
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      The loopback(egress port == ingress port) packet is dropped if the function is enabled.
 */
extern int32
dal_cypress_l2_srcPortEgrFilterMask_set(uint32 unit, rtk_portmask_t *pFilter_portmask);

/* Function Name:
 *      dal_cypress_l2_srcPortEgrFilterMask_add
 * Description:
 *      Enable the loopback filtering function on specified port.
 * Input:
 *      unit             - unit id
 *      filter_port - ports which turn on loopback filtering function
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      The loopback(egress port == ingress port) packet is dropped if the function is enabled.
 */
extern int32
dal_cypress_l2_srcPortEgrFilterMask_add(uint32 unit, rtk_port_t filter_port);

/* Function Name:
 *      dal_cypress_l2_srcPortEgrFilterMask_del
 * Description:
 *      Disable the loopback filtering function on specified port.
 * Input:
 *      unit             - unit id
 *      filter_port - ports which turn off loopback filtering function
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      The loopback(egress port == ingress port) packet is dropped if the function is enabled.
 */
extern int32
dal_cypress_l2_srcPortEgrFilterMask_del(uint32 unit, rtk_port_t filter_port);

/* Function Name:
 *      dal_cypress_l2_exceptionAddrAction_get
 * Description:
 *      Get forwarding action of packet with exception address.
 * Input:
 *      unit       - unit id
 *      exceptType - type of exception address
 * Output:
 *      pAction    - pointer to forward action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_L2_EXCEPT_ADDR_TYPE  - invalid exception address type
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      SA_IS_ZERO is not supported.
 *      Exception address type is as following
 *      - RTL8390 doesn't provide separated actions for SA_IS_MCAST and SA_IS_BCAST. They use the same action.
 *
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 */
extern int32
dal_cypress_l2_exceptionAddrAction_get(uint32 unit, rtk_l2_exceptionAddrType_t exceptType, rtk_action_t *pAction);

/* Function Name:
 *      dal_cypress_l2_exceptionAddrAction_set
 * Description:
 *      Set forwarding action of packet with exception address.
 * Input:
 *      unit       - unit id
 *      exceptType - type of exception address
 *      action     - forward action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_L2_EXCEPT_ADDR_TYPE  - invalid exception address type
 *      RT_ERR_FWD_ACTION           - invalid forwarding action
 * Note:
 *      SA_IS_ZERO is not supported.
 *      Exception address type is as following
 *      - RTL8390 doesn't provide separated actions for SA_IS_MCAST and SA_IS_BCAST. They use the same action.
 *
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 */
extern int32
dal_cypress_l2_exceptionAddrAction_set(uint32 unit, rtk_l2_exceptionAddrType_t exceptType, rtk_action_t action);

/* Function Name:
 *      dal_cypress_l2_zeroSALearningEnable_get
 * Description:
 *      Get enable status of all-zero-SA learning.
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - pointer to enable status of CPU tag adding
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_l2_zeroSALearningEnable_get(uint32 unit, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_cypress_l2_zeroSALearningEnable_set
 * Description:
 *      Set enable status of all-zero-SA learning.
 * Input:
 *      unit   - unit id
 *      enable - enable status of CPU tag adding
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_cypress_l2_zeroSALearningEnable_set(uint32 unit, rtk_enable_t enable);

/* Function Name:
 *      dal_cypress_l2_vlanMode_get
 * Description:
 *      Get vlan(inner/outer vlan) for L2 lookup on specified port.
 * Input:
 *      unit            - unit id
 *      port            - port id
 * Output:
 *      pVlanMode       - pointer to inner/outer vlan
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_l2_vlanMode_get(uint32 unit, rtk_port_t port, rtk_l2_vlanMode_t *pVlanMode);

/* Function Name:
 *      dal_cypress_l2_vlanMode_set
 * Description:
 *      Set vlan(inner/outer vlan) for L2 lookup on specified port.
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      vlanMode - inner/outer vlan
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_cypress_l2_vlanMode_set(uint32 unit, rtk_port_t port, rtk_l2_vlanMode_t vlanMode);

/* Function Name:
 *      dal_cypress_l2_secureMacMode_get
 * Description:
 *      Get enable status of secure source MAC address mode.
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - pointer to enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_l2_secureMacMode_get(uint32 unit, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_cypress_l2_secureMacMode_set
 * Description:
 *      Set enable status of secure source MAC address mode.
 * Input:
 *      unit   - unit id
 *      enable - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_cypress_l2_secureMacMode_set(uint32 unit, rtk_enable_t enable);

/* Function Name:
 *      dal_cypress_l2_portDynamicPortMoveForbidEnable_get
 * Description:
 *      Get the port moveforbiddance configuration of the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      (1) Dynamic address entry port move in/out forbiddance enabled port is not allowed and the entry
 *          is not updated.
 *      (2) Regarding the forwrding action, refer to rtk_l2_dynamicPortMoveForbidAction_set.
 */
extern int32
dal_cypress_l2_portDynamicPortMoveForbidEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_cypress_l2_portDynamicPortMoveForbidEnable_set
 * Description:
 *      Set the port move forbiddance configuration of the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      enable  - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      (1) Dynamic address entry port move in/out forbiddance enabled port is not allowed and the entry
 *          is not updated.
 *      (2) Regarding the forwrding action, refer to rtk_l2_dynamicPortMoveForbidAction_set.
 */
extern int32
dal_cypress_l2_portDynamicPortMoveForbidEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      dal_cypress_l2_dynamicPortMoveForbidAction_get
 * Description:
 *      Get the forwarding action when the port moving is detected on port move forbiddance port.
 * Input:
 *      unit     - unit id
 * Output:
 *      *pAction - pointer buffer of forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The action is taken either a dynamic address entry moved out from a port move forbiddance port
 *      or to a port move forbiddance port.
 */
extern int32
dal_cypress_l2_dynamicPortMoveForbidAction_get(uint32 unit, rtk_action_t *pAction);

/* Function Name:
 *      dal_cypress_l2_dynamicPortMoveForbidAction_set
 * Description:
 *      Set the forwarding action when the port moving is detected on port move forbiddance port.
 * Input:
 *      unit   - unit id
 *      action - forwarding action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT   - invalid input parameter
 * Note:
 *      The action is taken either a dynamic address entry moved out from a port move forbiddance port
 *      or to a port move forbiddance port.
 */
extern int32
dal_cypress_l2_dynamicPortMoveForbidAction_set(uint32 unit, rtk_action_t action);

/* Function Name:
 *      dal_cypress_l2_portMacFilterEnable_get
 * Description:
 *      Get the mac filter configuration of the specified port.
 * Input:
 *      unit        - unit id
 *      port        - port id
 *      filterMode  - filter DA or SA
 * Output:
 *      pEnable     - enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390
 * Note:
 *      None
 */
extern int32
dal_cypress_l2_portMacFilterEnable_get(uint32 unit, rtk_port_t port, rtk_l2_macFilterMode_t filterMode, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_cypress_l2_portMacFilterEnable_set
 * Description:
 *      Set the mac filter configuration of the specified port.
 * Input:
 *      unit        - unit id
 *      port        - port id
 *      filterMode  - filter DA or SA
 *      enable      - drop procedence assignment status for trapped packet
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390
 * Note:
 *      None
 */
extern int32
dal_cypress_l2_portMacFilterEnable_set(uint32 unit, rtk_port_t port, rtk_l2_macFilterMode_t filterMode, rtk_enable_t enable);

/* Function Name:
 *      dal_cypress_l2_addrEntry_get
 * Description:
 *      Get the L2 table entry by index of the specified unit.
 * Input:
 *      unit  - unit id
 *      index - l2 table index
 * Output:
 *      pL2_entry - pointer buffer of l2 table entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      1) The index valid range is from 0 to (L2 hash table size - 1)
 *         - 0 ~ (L2 hash table size - 1) entry in L2 hash table
 *      2) The output entry have 2 variables (valid and entry_type) and its detail data structure
 *         - valid: 1 mean the entry is valid; 0: invalid
 *         - entry_type: FLOW_TYPE_UNICAST, FLOW_TYPE_L2_MULTI, FLOW_TYPE_IP4_MULTI and FLOW_TYPE_IP6_MULTI
 *                       the field is ignored if valid field is 0.
 *         - detail data structure is ignored if valid is 0, and its field meanings are depended
 *           on the entry_type value.
 *      3) If pL2_entry->flags have enabled the RTK_L2_UCAST_FLAG_TRUNK_PORT flag, mean the
 *         pL2_entry->unicast.trk_gid value is valid trunk id value.
 */
extern int32
dal_cypress_l2_addrEntry_get(uint32 unit, uint32 index, rtk_l2_entry_t *pL2_entry);

/* Function Name:
 *      dal_cypress_l2_conflictAddr_get
 * Description:
 *      Get the conflict L2 table entry from one given L2 address in the specified unit.
 * Input:
 *      unit            - unit id
 *      pL2Addr         - l2 address to find its conflict entries
 *      cfAddrList_size - buffer size of the pCfAddrList
 * Output:
 *      pCfAddrList - pointer buffer of the conflict l2 table entry list
 *      pCf_retCnt  - return number of find conflict l2 table entry list
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The function can be used if add l2 entry return RT_ERR_L2_NO_EMPTY_ENTRY.
 *          Input the pL2Addr->entry_type and its hash key to get conflict entry information.
 *      (2) User want to prepare the return buffer pCfAddrList and via. cfAddrList_size argument
 *          tell driver its size.
 *      (3) The function will return valid L2 hash entry from the same bucket and the return number
 *          is filled in pCf_retCnt, entry data is filled in pCfAddrList.
 */
extern int32
dal_cypress_l2_conflictAddr_get(uint32 unit, rtk_l2_entry_t *pL2Addr, rtk_l2_entry_t *pCfAddrList, uint32 cfAddrList_size, uint32 *pCf_retCnt);

/* Function Name:
 *     dal_cypress_l2_getL2EntryfromHash_dump
 * Description:
 *      Get L2 Entry from Chip
 * Input:
 *      unit      - unit id
 *      hashKey   - Hash Key for this Entry
 *      location  - Entry location in Hash Bucket
 *      pL2_entry - L2 entry used to do search
 *      pIsValid  - Is valid entry
 *
 * Output:
 *      pL2_entry - L2 entry
 *      pIsValid  - Is valid or invalid entry
 *                    TRUE: valid entry
 *                    FALSE: invalid entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 * Note:
 *      We can not distinguish an entry between IP multicast in L2 mode and L2 multicast
 */

extern int32
dal_cypress_l2_getL2EntryfromHash_dump(uint32 unit, uint32 entry_idx, dal_cypress_l2_entry_t *pL2_entry, uint32 *pIsValid);

/* Function Name:
 *      dal_cypress_l2_macLearningCnt_get
 * Description:
 *      Get number of learned MAC addresses of specified type.
 * Input:
 *      unit             - unit id
 *      fid_macLimit_idx - index of VLAN MAC limit entry
 * Output:
 *      pNum             - number of learned MAC addresses
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The valid range of fid_macLimit_idx is 0~31 in 8390 and 9310, and 0~7 in 8380 and 9300
 */
extern int32
dal_cypress_l2_macLearningCnt_get(uint32 unit, rtk_l2_macLimitType_t type, rtk_l2_macCnt_t *pLimitCnt);

/* Function Name:
 *      dal_cypress_l2_limitLearningNum_get
 * Description:
 *      Get the mac limit learning counts of specified device.
 * Input:
 *      unit     - unit id
 *      type - mac limit type
 *      pLimitCnt - pointer ro mac limit parameters
 * Output:
 *      pLimitCnt - pointer of mac limit learning counts
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_TRUNK_ID     - invalid trunk id
 * Note:
 *      (1) The mac learning counts only calculate dynamic mac numbers.
 */
extern int32
dal_cypress_l2_limitLearningNum_get(uint32 unit, rtk_l2_macLimitType_t type, rtk_l2_macCnt_t *pLimitCnt);


/* Function Name:
 *      dal_cypress_l2_limitLearningNum_set
 * Description:
 *      Set the mac limit learning counts of specified device.
 * Input:
 * Input:
 *      unit     - unit id
 *      type - mac limit type
 *      pLimitCnt - pointer ro mac limit parameters and mac limit learning counts
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_TRUNK_ID     - invalid trunk id
 * Note:
 *      (1) The mac learning counts only calculate dynamic mac numbers.
 */
extern int32
dal_cypress_l2_limitLearningNum_set(uint32 unit, rtk_l2_macLimitType_t type, rtk_l2_macCnt_t *pLimitCnt);

/* Function Name:
 *      dal_cypress_l2_limitLearningAction_get
 * Description:
 *      Get the mac limit action of specified device.
 * Input:
 *      unit     - unit id
 *      type - mac limit type
 *      pAction - pointer ro mac limit parameters
 * Output:
 *      pAction - pointer of mac limit action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_TRUNK_ID     - invalid trunk id
 * Note:
 *      (1) The mac learning counts only calculate dynamic mac numbers.
 */
extern int32
dal_cypress_l2_limitLearningAction_get(uint32 unit, rtk_l2_macLimitType_t type, rtk_l2_macLimitAction_t *pAction);

/* Function Name:
 *      dal_cypress_l2_limitLearningAction_set
 * Description:
 *      Set the mac limit action of specified device.
 * Input:
 * Input:
 *      unit     - unit id
 *      type - mac limit type
 *      pAction - pointer ro mac limit parameters and mac limit action
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_TRUNK_ID     - invalid trunk id
 * Note:
 *      (1) The mac learning counts only calculate dynamic mac numbers.
 */
extern int32
dal_cypress_l2_limitLearningAction_set(uint32 unit, rtk_l2_macLimitType_t type, rtk_l2_macLimitAction_t *pAction);

/* Function Name:
 *      dal_cypress_l2_agingTime_get
 * Description:
 *      Get the aging time of from the specified device.
 * Input:
 *      unit        - unit id
 *      type       - age time type
 * Output:
 *      pAging_time - pointer buffer of aging time
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The unit is second.
 */
extern int32
dal_cypress_l2_agingTime_get(uint32 unit, rtk_l2_ageTimeType_t type, uint32 *pAging_time);

/* Function Name:
 *      dal_cypress_l2_agingTime_set
 * Description:
 *      Set the aging time of suspending entry from the specified device.
 * Input:
 *      unit       - unit id
 *      type       - age time type
 *      aging_time - aging time
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      The unit is second.
 */
extern int32
dal_cypress_l2_agingTime_set(uint32 unit, rtk_l2_ageTimeType_t type, uint32 aging_time);

/* Function Name:
 *      dal_cypress_l2_portMoveAction_get
 * Description:
 *      Get forwarding action when port moving is detected.
 * Input:
 *      unit       - unit id
 *      type      - port move type
 *      pAction  - pointer to portmove parameter
 * Output:
 *      pAction - pointer to forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_l2_portMoveAction_get(
    uint32              unit,
    rtk_l2_portMoveType_t type,
    rtk_l2_portMoveAct_t        *pAction);

/* Function Name:
 *      dal_cypress_l2_portMoveAction_set
 * Description:
 *      Set forwarding action when port moving is detected.
 * Input:
 *      unit       - unit id
 *      type      - port move type
 *      pAction  - pointer to portmove parameter and action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID    - invalid unit id
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_PORT_ID    - invalid port id
 *      RT_ERR_FWD_ACTION - invalid forwarding action
 * Note:
 *      None
 */
extern int32
dal_cypress_l2_portMoveAction_set(
    uint32              unit,
    rtk_l2_portMoveType_t type,
    rtk_l2_portMoveAct_t        *pAction);

/* Function Name:
 *      dal_cypress_l2_portUcastLookupMissAction_get
 * Description:
 *      Get forwarding action of specified port when unicast destination address lookup miss.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pAction - pointer to forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Forwarding action is as following:
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_COPY2CPU
 *      - ACTION_TRAP2MASTERCPU
 *      - ACTION_COPY2MASTERCPU
 */
extern int32
dal_cypress_l2_portUcastLookupMissAction_get(uint32 unit, rtk_port_t port, rtk_action_t *pAction);

/* Function Name:
 *      dal_cypress_l2_portUcastLookupMissAction_set
 * Description:
 *      Set forwarding action of specified port when unicast destination address lookup miss.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      action - forwarding action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID    - invalid unit id
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_PORT_ID    - invalid port id
 *      RT_ERR_FWD_ACTION - invalid forwarding action
 * Note:
 *      Forwarding action is as following:
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_COPY2CPU
 *      - ACTION_TRAP2MASTERCPU
 *      - ACTION_COPY2MASTERCPU
 */
extern int32
dal_cypress_l2_portUcastLookupMissAction_set(uint32 unit, rtk_port_t port, rtk_action_t action);

 /* Function Name:
 *      dal_cypress_l2_mcastFwdPortmaskEntry_get
 * Description:
 *      Get portmask of multicast forwarding entry
 * Input:
 *      unit       - unit id
 *      index      - index of multicast forwarding portmask
 * Output:
 *      pPortmask  - pointer buffer of multicast ports
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID            - invalid unit id
 *      RT_ERR_NOT_INIT           - The module is not initial
 *      RT_ERR_NULL_POINTER       - input parameter may be null pointer
 *      RT_ERR_L2_MULTI_FWD_INDEX - invalid index of multicast forwarding portmask
 * Note:
 *      The valid range of indx is 0 ~ (multicast forwarding table size - 1)
 */
extern int32
dal_cypress_l2_mcastFwdPortmaskEntry_get(
    uint32          unit,
    int32           index,
    rtk_portmask_t  *pPortmask);

/* Function Name:
 *      dal_cypress_l2_mcastFwdPortmaskEntry_set
 * Description:
 *      Set portmask of multicast forwarding entry
 * Input:
 *      unit      - unit id
 *      index     - index of multicast forwarding portmask
 *      pPortmask - pointer buffer of multicast ports
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID            - invalid unit id
 *      RT_ERR_NOT_INIT           - The module is not initial
 *      RT_ERR_NULL_POINTER       - input parameter may be null pointer
 *      RT_ERR_L2_MULTI_FWD_INDEX - invalid index of multicast forwarding portmask
 * Note:
 *      The valid range of indx is 0 ~ (multicast forwarding table size - 1)
 */
extern int32
dal_cypress_l2_mcastFwdPortmaskEntry_set(
    uint32          unit,
    int32           index,
    rtk_portmask_t  *pPortmask);

/* Function Name:
 *      dal_cypress_l2_hashIdx_get
 * Description:
 *      Get L2 hash index based on specified vid and MAC address
 * Input:
 *      unit        - unit id
 *      pMacHashIdx - pointer to vid and mac
 * Output:
 *      pMacHashIdx - pointer to hash indexes
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID           - invalid unit id
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_VLAN_VID          - invalid vlan id
 *      RT_ERR_MAC               - invalid mac address
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 * Note:
 *      (1) VID is same as FID in IVL mode.
 *      (2) The pMacHashIdx.vid and pMacHashIdx.mac is input key
 *      (3) validBlk and validAlgo fields in pMacHashIdx specify how many blocks and hash-algo number
 *          the target unit supports.
 */
extern int32
dal_cypress_l2_hashIdx_get(uint32 unit, rtk_l2_macHashIdx_t *pMacHashIdx);
#endif /* __DAL_CYPRESS_L2_H__ */

