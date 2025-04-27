/*
 * Copyright (C) 2009-2015 Realtek Semiconductor Corp.
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
 * Purpose : chip table and its field definition in the SDK.
 *
 * Feature : chip table and its field definition
 *
 */

#ifndef __HAL_CHIPDEF_ALLMEM_H__
#define __HAL_CHIPDEF_ALLMEM_H__

/*
 * Include Files
 */
#include <common/rt_autoconf.h>
#include <common/rt_type.h>

/*
 * Data Type Declaration
 */

#if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
#define TBL_NAME_LEN    128
#endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */

/* indirect control group enum definition */
typedef enum rtk_indirectCtrlGroup_e
{
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL9300)
    INDIRECT_CTRL_GROUP_TABLE,
#endif
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL9300)
    INDIRECT_CTRL_GROUP_L2,
    INDIRECT_CTRL_GROUP_PKT_ENC,
    INDIRECT_CTRL_GROUP_EGR_CTRL,
    INDIRECT_CTRL_GROUP_HSB_CTRL,
    INDIRECT_CTRL_GROUP_HSA_CTRL,
#endif
#if defined(CONFIG_SDK_RTL9310)
    INDIRECT_CTRL_GROUP_SET0,
    INDIRECT_CTRL_GROUP_SET1,
    INDIRECT_CTRL_GROUP_SET2,
    INDIRECT_CTRL_GROUP_SET3,
    INDIRECT_CTRL_GROUP_SET4,
    INDIRECT_CTRL_GROUP_SET5,
#endif
    RTK_INDIRECT_CTRL_GROUP_END
} rtk_indirectCtrlGroup_t;


/* table field structure definition */
typedef struct rtk_tableField_s
{
    #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
    char name[TBL_NAME_LEN];  /* field name */
    #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
    uint16 lsp;               /* LSP (Least Significant Position) of the field */
    uint16 len;               /* field length */
} rtk_tableField_t;

/* table structure definition */
typedef struct rtk_table_s
{
    #if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
    char name[TBL_NAME_LEN];          /* table name */
    #endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
    unsigned int set;                 /* access table set */
    unsigned int type;                /* access table type */
    unsigned int size;                /* table size */
    unsigned int datareg_num;         /* total data registers */
    unsigned int field_num;           /* total field numbers */
    rtk_tableField_t *pFields; /* table fields */
} rtk_table_t;

/*
 * Macro Definition
 */

/* Declaration the size of table entry
 * Each structure is a word array that size is the maximum in all supported chips.
 */
#define MEM_ENTRY_DECLARE(name, words)\
    typedef struct {\
        uint32 entry_data[words];\
    } name

/* structure is word array that have the maximum value of all supported chips. */
#if (defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL9300))
MEM_ENTRY_DECLARE(l2cam_entry_t, 5);
MEM_ENTRY_DECLARE(l2_entry_t, 5);
MEM_ENTRY_DECLARE(log_entry_t, 3);
MEM_ENTRY_DECLARE(multicast_index_entry_t, 2);
MEM_ENTRY_DECLARE(port_iso_entry_t, 2);
#endif

MEM_ENTRY_DECLARE(vlan_entry_t, 4);

#if (defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL9300))
MEM_ENTRY_DECLARE(spt_entry_t, 4);
#endif

#if (defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL9300))
MEM_ENTRY_DECLARE(acl_entry_t, 22);
MEM_ENTRY_DECLARE(meter_entry_t, 3);
MEM_ENTRY_DECLARE(vlan_untag_entry_t, 2);
MEM_ENTRY_DECLARE(vlan_igrcnvt_entry_t, 7);
MEM_ENTRY_DECLARE(vlan_egrcnvt_entry_t, 6);
MEM_ENTRY_DECLARE(routing_entry_t, 2);
MEM_ENTRY_DECLARE(sched_entry_t, 9);
MEM_ENTRY_DECLARE(spg_port_entry_t, 8);
MEM_ENTRY_DECLARE(out_q_entry_t, 13);
MEM_ENTRY_DECLARE(acl_igrRule_entry_t, 19);
MEM_ENTRY_DECLARE(acl_egrRule_entry_t, 17);
#endif

#if defined(CONFIG_SDK_RTL8390)
MEM_ENTRY_DECLARE(mpls_lib_entry_t, 2);
#endif

#if (defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL9300))
MEM_ENTRY_DECLARE(lag_entry_t, 3);
MEM_ENTRY_DECLARE(lag_src_map_entry_t, 1);
MEM_ENTRY_DECLARE(rmk_entry_t, 1);
#endif

#if defined(CONFIG_SDK_RTL9310)
MEM_ENTRY_DECLARE(mpls_encap_entry_t, 2);
MEM_ENTRY_DECLARE(mpls_decap_entry_t, 4);
MEM_ENTRY_DECLARE(mpls_nextHop_entry_t, 1);
MEM_ENTRY_DECLARE(egr_qBw_entry_t, 36);
MEM_ENTRY_DECLARE(auto_rcvy_txerr_cnt_entry_t, 1);
MEM_ENTRY_DECLARE(mib_portCntr_entry_t, 53);
MEM_ENTRY_DECLARE(mib_portPrvteCntr_entry_t, 28);
MEM_ENTRY_DECLARE(mib_portPrvteEQCntr_entry_t, 4);
MEM_ENTRY_DECLARE(l3_intf_igr_cntr_t, 6);
MEM_ENTRY_DECLARE(l3_intf_egr_cntr_t, 6);
MEM_ENTRY_DECLARE(tunnel_decap_entry_t, 8);
MEM_ENTRY_DECLARE(tunnel_encap_entry_t, 9);
MEM_ENTRY_DECLARE(of_igr_entry_t, 22);
MEM_ENTRY_DECLARE(of_egr_entry_t, 17);
MEM_ENTRY_DECLARE(of_l2_entry_t, 4);
MEM_ENTRY_DECLARE(of_l3_cam_entry_t, 6);
MEM_ENTRY_DECLARE(of_l3_hash_entry_t, 4);
MEM_ENTRY_DECLARE(of_grp_entry_t, 1);
MEM_ENTRY_DECLARE(of_bucket_entry_t, 3);
MEM_ENTRY_DECLARE(of_dmac_entry_t, 2);
MEM_ENTRY_DECLARE(ecid_pvid_entry_t, 2);
MEM_ENTRY_DECLARE(ecid_pmsk_entry_t, 2);
MEM_ENTRY_DECLARE(l2_tnl_lst_t, 2);
MEM_ENTRY_DECLARE(l2_tnl_entry_t, 3);
MEM_ENTRY_DECLARE(l2_tnl_entry_encap_t, 4);
#endif

#if (defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL9300))
/*Some items below should be merged with above, and the size to be synced with 9310*/
MEM_ENTRY_DECLARE(l3_router_mac_entry_t, 5);
MEM_ENTRY_DECLARE(l3_host_route_entry_t, 4);
MEM_ENTRY_DECLARE(l3_prefix_route_entry_t, 6);
MEM_ENTRY_DECLARE(l3_ecmp_entry_t, 6);
MEM_ENTRY_DECLARE(l3_nexthop_entry_t, 1);
MEM_ENTRY_DECLARE(l3_igr_intf_entry_t, 2);
MEM_ENTRY_DECLARE(l3_egr_intf_entry_t, 4);
MEM_ENTRY_DECLARE(l3_egr_intf_list_entry_t, 2);
MEM_ENTRY_DECLARE(l3_igr_intf_cntr_t, 7);
MEM_ENTRY_DECLARE(l3_egr_intf_cntr_t, 6);
MEM_ENTRY_DECLARE(l3_igr_tunnel_intf_cntr_t, 4);
MEM_ENTRY_DECLARE(l3_egr_tunnel_intf_cntr_t, 4);
MEM_ENTRY_DECLARE(routing_mac_entry_t, 7);
MEM_ENTRY_DECLARE(host_routing_entry_t, 11);
MEM_ENTRY_DECLARE(prefix_routing_entry_t, 20);
MEM_ENTRY_DECLARE(next_hop_entry_t, 1);
MEM_ENTRY_DECLARE(l3_egr_int_entry_t, 2);
MEM_ENTRY_DECLARE(l3_egr_iol_entry_t, 1);
MEM_ENTRY_DECLARE(l3_sa_int_entry_t, 2);
#endif

#if defined(CONFIG_SDK_RTL9310)
MEM_ENTRY_DECLARE(ipmac_bind_entry_t, 4);
#endif

#endif  /* __HAL_CHIPDEF_ALLMEM_H__ */

