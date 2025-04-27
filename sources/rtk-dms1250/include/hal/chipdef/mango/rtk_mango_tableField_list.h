#ifndef __RTK_MANGO_TABLEFIELD_LIST_H__
#define __RTK_MANGO_TABLEFIELD_LIST_H__

/*
 * Include Files
 */
#include <common/rt_autoconf.h>
#ifdef __BOOTLOADER__
#include <hal/chipdef/mango/rtk_mango_uboot_feature_def.h>
#else   /* __BOOTLOADER__ */
#include <hal/chipdef/mango/rtk_mango_feature_def.h>
#endif  /* __BOOTLOADER__ */

#if defined(CONFIG_SDK_CHIP_FEATURE_802_1Q_VLAN___QINQ)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_VLAN_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_VLAN_UNTAG_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_802_1Q_VLAN___QINQ */
#if defined(CONFIG_SDK_CHIP_FEATURE_VLAN_TRANSLATION)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_VLAN_EVC_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_VLAN_IP_BASED_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_VLAN_IVC_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_VLAN_MAC_BASED_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_VLAN_TRANSLATION */
#if defined(CONFIG_SDK_CHIP_FEATURE_LINK_AGGREGATION)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_LAG_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_SRC_TRK_MAP_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_LINK_AGGREGATION */
#if defined(CONFIG_SDK_CHIP_FEATURE_SPANNING_TREE)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_MSTI_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_SPANNING_TREE */
#if defined(CONFIG_SDK_CHIP_FEATURE_PORT_ISOLATION)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_PORT_ISO_CTRL_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_PORT_ISOLATION */
#if defined(CONFIG_SDK_CHIP_FEATURE_MAC_FORWARDING_CONTROL)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_L2_CAM_CB_MC_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_L2_CAM_CB_UC_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_L2_CAM_MC_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_L2_CAM_UC_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_L2_CB_MC_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_L2_CB_UC_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_L2_MC_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_L2_UC_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_MC_PMSK_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_MAC_FORWARDING_CONTROL */
#if defined(CONFIG_SDK_CHIP_FEATURE_EGRESS_BANDWIDTH_CONTROL)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_EGR_Q_BW_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_EGRESS_BANDWIDTH_CONTROL */
#if defined(CONFIG_SDK_CHIP_FEATURE_METER_MARKER)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_METER_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_METER_MARKER */
#if defined(CONFIG_SDK_CHIP_FEATURE_FLOWCONTROL___BACKPRESSURE)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_OUT_Q_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_FLOWCONTROL___BACKPRESSURE */
#if defined(CONFIG_SDK_CHIP_FEATURE_REMARKING)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_REMARK_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_REMARKING */
#if defined(CONFIG_SDK_CHIP_FEATURE_LAYER_3_ROUTING)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_L3_ECMP_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_L3_EGR_INTF_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_L3_EGR_INTF_CNTR_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_L3_EGR_INTF_LIST_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_L3_EGR_TUNNEL_INTF_CNTR_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_L3_HOST_ROUTE_IP6MC_0_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_L3_HOST_ROUTE_IP6MC_1_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_L3_HOST_ROUTE_IP6MC_2_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_L3_HOST_ROUTE_IP6MC_3_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_L3_HOST_ROUTE_IP6MC_4_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_L3_HOST_ROUTE_IP6MC_5_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_L3_HOST_ROUTE_IP6UC_0_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_L3_HOST_ROUTE_IP6UC_1_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_L3_HOST_ROUTE_IP6UC_2_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_L3_HOST_ROUTE_IPMC_0_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_L3_HOST_ROUTE_IPMC_1_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_L3_HOST_ROUTE_IPUC_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_L3_IGR_INTF_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_L3_IGR_INTF_CNTR_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_L3_IGR_TUNNEL_INTF_CNTR_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_L3_NEXTHOP_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_L3_PREFIX_ROUTE_IP6MC_0_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_L3_PREFIX_ROUTE_IP6MC_1_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_L3_PREFIX_ROUTE_IP6MC_2_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_L3_PREFIX_ROUTE_IP6MC_3_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_L3_PREFIX_ROUTE_IP6MC_4_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_L3_PREFIX_ROUTE_IP6MC_5_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_L3_PREFIX_ROUTE_IP6UC_0_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_L3_PREFIX_ROUTE_IP6UC_1_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_L3_PREFIX_ROUTE_IP6UC_2_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_L3_PREFIX_ROUTE_IPMC_0_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_L3_PREFIX_ROUTE_IPMC_1_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_L3_PREFIX_ROUTE_IPUC_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_L3_ROUTER_MAC_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_LAYER_3_ROUTING */
#if defined(CONFIG_SDK_CHIP_FEATURE_IP_TUNNEL)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_TUNNEL_START_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_TUNNEL_START_ENCAP_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_TUNNEL_START_ENCAP_1_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_TUNNEL_START_ENCAP_2_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_TUNNEL_TERMINATION_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_TUNNEL_TERMINATION_1_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_TUNNEL_TERMINATION_2_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_IP_TUNNEL */
#if defined(CONFIG_SDK_CHIP_FEATURE_MPLS_VPLS)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_MPLS_DECAP_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_MPLS_DECAP_CAM_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_MPLS_ENCAP_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_MPLS_NH_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_MPLS_VPLS */
#if defined(CONFIG_SDK_CHIP_FEATURE_MIB_COUNTER)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_STAT_PORT_MIB_CNTR_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_MIB_COUNTER */
#if defined(CONFIG_SDK_CHIP_FEATURE_FLOW_BASED_COUNTER)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_FLOW_CNTR_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_FLOW_BASED_COUNTER */
#if defined(CONFIG_SDK_CHIP_FEATURE_PRIVATE_COUNTER)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_AUTO_RECOVERY_TXERR_CNT_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_STAT_PORT_PRVTE_CNTR_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_STAT_PORT_PRVTE_E_Q_CNTR_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_PRIVATE_COUNTER */
#if defined(CONFIG_SDK_CHIP_FEATURE_ACL)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_EACL_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_IACL_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_VACL_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_ACL */
#if defined(CONFIG_SDK_CHIP_FEATURE_IP_MAC_BIDING)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_IP_MAC_BIND_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_IP_MAC_BIND_CAM_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_IP_MAC_BIND_DYN_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_IP_MAC_BIDING */
#if defined(CONFIG_SDK_CHIP_FEATURE_OPENFLOW)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_ACTION_BUCKET_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_FT_EGR_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_FT_IGR_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_FT_L2_CAM_FMT0_0_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_FT_L2_CAM_FMT0_1_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_FT_L2_CAM_FMT1_0_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_FT_L2_CAM_FMT1_1_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_FT_L2_CAM_FMT2_0_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_FT_L2_HASH_FMT0_0_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_FT_L2_HASH_FMT0_1_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_FT_L2_HASH_FMT1_0_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_FT_L2_HASH_FMT1_1_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_FT_L2_HASH_FMT2_0_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_FT_L2_HASH_FMT2_1_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_FT_L3_HASH_0_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_FT_L3_HASH_1_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_FT_L3_HASH_2_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_FT_L3_TCAM_0_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_FT_L3_TCAM_1_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_FT_L3_TCAM_2_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_GROUP_TBL_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_OF_DMAC_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_OPENFLOW */
#if defined(CONFIG_SDK_CHIP_FEATURE_L2_TUNNEL)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_BSSID_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_BSSID_CAM_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_BSSID_LIST_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_BSSID_TUNNEL_START_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_CAPWAP_TUNNEL_START_0_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_CAPWAP_TUNNEL_START_1_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_L2_TNL_LST_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_L2_TNL_MCAST_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_L2_TNL_MCAST_CAM_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_L2_TNL_UCAST_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_L2_TNL_UCAST_CAM_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_L2_TNL_VXLAN_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_L2_TNL_VXLAN_CAM_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_L2_TNL_VXLAN_ENCAP_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_VXLAN_TUNNEL_START_0_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_VXLAN_TUNNEL_START_1_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_WLC_MCAST_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_WLC_MCAST_CAM_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_WLC_UCAST_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_WLC_UCAST_CAM_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_L2_TUNNEL */
#if defined(CONFIG_SDK_CHIP_FEATURE_PORT_EXTENSION)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_ECID_PMSK_LIST_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_ECID_PVID_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_ECID_PVID_CAM_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_PE_FWD_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_tableField_t RTL9310_PE_FWD_CAM_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_PORT_EXTENSION */



#endif    /* __RTK_MANGO_TABLEFIELD_LIST_H__ */
