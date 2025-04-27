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
 * Purpose : Definition of Multicast API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) Multicast Group ID
 *           (2) Multicast Nexthop
 *
 */

#ifndef __RTK_MCAST_H__
#define __RTK_MCAST_H__

/*
 * Include Files
 */
#include <common/rt_type.h>

/*
 * Symbol Definition
 */
 /* Group ID */
#define RTK_MCAST_GROUP_ID(_type, _index)         ((((_type) & 0xFF) << 24) | ((_index) & 0x00FFFFFF))
#define RTK_MCAST_GROUP_ID_TYPE(_groupId)         (((_groupId) & 0xFF000000) >> 24)
#define RTK_MCAST_GROUP_ID_IDX(_groupId)          ((_groupId) & 0x00FFFFFF)

/* Mulicast Group flags. */
#define RTK_MCAST_FLAG_WITH_ID                  0x00000001

/* Flags for L3 multicast egress-interface */
typedef enum rtk_mcast_egrIf_flag_e
{
    RTK_MCAST_EGRIF_FLAG_PMSK_REPLACE   = 0x00000001,   /* When add next-hop portmask,
                                                         * replace the hardware with user input value
                                                         * instead of adding the ports to hardware. */
    RTK_MCAST_EGRIF_FLAG_BRIDGE         = 0x00000002,   /* forwarding packet as bridging */
} rtk_mcast_egrIf_flag_t;


/* Multicast group type */
typedef enum rtk_mcast_type_e
{
    RTK_MCAST_TYPE_MAC = 1,
    RTK_MCAST_TYPE_IP,
    RTK_MCAST_TYPE_END
} rtk_mcast_type_t;

/*
 * Data Declaration
 */
/* multicast group */
typedef uint32 rtk_mcast_group_t;


/* L3 multicast egrIf type */
typedef enum rtk_mcast_egrIfType_e
{
    RTK_MCAST_EGRIF_TYPE_L2 = 0,       /* L2 Bridging */
    RTK_MCAST_EGRIF_TYPE_L3,           /* L3 Routing */
    RTK_MCAST_EGRIF_TYPE_TUNNEL,       /* IP tunnel */
    RTK_MCAST_EGRIF_TYPE_BPE,          /* Bridge Port Extension */
    RTK_MCAST_EGRIF_TYPE_VXLAN,        /* VXLAN (L2 tunnel) */
    RTK_MCAST_EGRIF_TYPE_STK,          /* Stacking Port Forwarding */
    RTK_MCAST_EGRIF_TYPE_END
} rtk_mcast_egrIf_type_t;

/* L3 multicast egress-interface */
typedef struct rtk_mcast_egrif_s
{
    rtk_mcast_egrIf_flag_t      flags;   /* use for configure */
    rtk_mcast_egrIf_type_t      type;

    union
    {
        /*
         * L2 bridging type
         */

        struct {
            rtk_portmask_t      portmask;
            int32               fwdIndex;   /* index of forwarding table, -1 means auto allocation */
        } l2;

        struct {
            rtk_portmask_t      portmask;
            int32               fwdIndex;   /* index of forwarding table, -1 means auto allocation */
            rtk_bpe_ecid_t      ecid;
        } bpe;

        struct {
            uint32              entry_idx;  /* VXLAN entry index */
        } vxlan;

        /*
         * L3 routing type
         */

        struct {
            rtk_intf_id_t       intf_id;
            rtk_portmask_t      portmask;
            int32               fwdIndex;   /* index of forwarding table, -1 means auto allocation */
            rtk_bpe_ecid_t      ecid;
        } l3;

        struct {
            rtk_intf_id_t       intf_id;
        } tunnel;

        /*
         * Stacking type
         */
        struct {
            rtk_portmask_t      portmask;   /* stacking forward portmask */
        } stk;
    };
} rtk_mcast_egrif_t;

/*
 * Macro Declaration
 */




/*
 * Function Declaration
 */

/* Module Name : Multicast Group */

/* Function Name:
 *      rtk_mcast_init
 * Description:
 *      Initialize multicast group module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      9300, 9310
 * Note:
 *      Must initialize multicast group module before calling any multicast group APIs.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_mcast_init(uint32 unit);

/* Function Name:
 *      rtk_mcast_group_create
 * Description:
 *      Create a multicast group
 * Input:
 *      unit   - unit id
 *      flags  - RTK_MCAST_FLAG_XXX
 *      type   - multicast group type
 *      pGroup - group id (use with RTK_MCAST_FLAG_WITH_ID flag)
 * Output:
 *      pGroup - pointer to multicast group
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Applicable flags:
 *          RTK_MCAST_FLAG_WITH_ID    - to indicate the group ID use pGroup (include group type and group index)
 *                                      could use marco - RTK_MCAST_GROUP_ID(type, index) to specify it
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_mcast_group_create(uint32 unit, uint32 flags, rtk_mcast_type_t type, rtk_mcast_group_t *pGroup);

/* Function Name:
 *      rtk_mcast_group_destroy
 * Description:
 *      Destrory a multicast group
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
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_mcast_group_destroy(uint32 unit, rtk_mcast_group_t group);

/* Function Name:
 *      rtk_mcast_group_getNext
 * Description:
 *      Get the next multicast group
 * Input:
 *      unit   - unit id
 *      type   - mcast type
 *      pBase  - start index
 * Output:
 *      pGroup - pointer to multicast group
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Use base index -1 to indicate to search from the beginging of group entry
 *      (2) If the returned index is -1, means not found the next valid entry
 *      (3) type = 0 indicate include all type
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_mcast_group_getNext(uint32 unit, rtk_mcast_type_t  type, int32 *pBase, rtk_mcast_group_t *pGroup);

/* Function Name:
 *      rtk_mcast_egrIf_get
 * Description:
 *      Get the egress interfaces information of a specified multicast group
 * Input:
 *      unit          - unit id
 *      group         - multicast group
 *      egrIf_max     - number of rtk_mcast_egrif_t entry in pEgrIf buffer
 *      pEgrIf        - buffer pointer
 * Output:
 *      *pEgrIf       - next hop information
 *      *pEgrIf_count - returned rtk_mcast_egrif_t entry number
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) pEgrIf is input buffer,user should alloc memory firstly,
 *          egrIf_max is the max entry number can use rtk_switch_deviceInfo_get()
 *          to get max_num_of_mcast_group_nexthop
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_mcast_egrIf_get(uint32 unit, rtk_mcast_group_t group, int32 egrIf_max, rtk_mcast_egrif_t *pEgrIf, int32 *pEgrIf_count);

/* Function Name:
 *      rtk_mcast_egrIf_add
 * Description:
 *      Add an egress interface to a multicast group replication list
 * Input:
 *      unit   - unit id
 *      group  - multicast group
 *      pEgrIf - pointer to multicast next hop information
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_PORT_MASK    - invalid portmask
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Applicable flags:
 *          RTK_MCAST_EGRIF_FLAG_PMSK_REPLACE   - to indicate replace nexthop type entry portmask
 *          RTK_MCAST_EGRIF_FLAG_BRIDGE         - to indicate L3 nexthop type used for L2 bridge
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_mcast_egrIf_add(uint32 unit, rtk_mcast_group_t group, rtk_mcast_egrif_t *pEgrIf);

/* Function Name:
 *      rtk_mcast_egrIf_del
 * Description:
 *      Delete an egress interface from multicast group replication list
 * Input:
 *      unit   - unit id
 *      group  - multicast group
 *      pEgrIf - pointer to multicast next hop information
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_PORT_MASK    - invalid portmask
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Basic required parameters of pEgrIf as input:
 *          type and type info of l2/l3/stk/tunnel.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_mcast_egrIf_del(uint32 unit, rtk_mcast_group_t group, rtk_mcast_egrif_t *pEgrIf);

/* Function Name:
 *      rtk_mcast_egrIf_delAll
 * Description:
 *      Delete all egress interfaces from a multicast group replication list
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
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_mcast_egrIf_delAll(uint32 unit, rtk_mcast_group_t group);

#endif /* __RTK_MCAST_H__ */

