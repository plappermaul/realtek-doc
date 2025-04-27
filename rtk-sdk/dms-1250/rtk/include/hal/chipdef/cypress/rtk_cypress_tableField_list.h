#ifndef __RTK_CYPRESS_TABLEFIELD_LIST_H__
#define __RTK_CYPRESS_TABLEFIELD_LIST_H__

/*
 * Include Files
 */
#include <common/rt_autoconf.h>
#ifdef __BOOTLOADER__
#include <hal/chipdef/cypress/rtk_cypress_uboot_feature_def.h>
#else   /* __BOOTLOADER__ */
#include <hal/chipdef/cypress/rtk_cypress_feature_def.h>
#endif  /* __BOOTLOADER__ */

#if defined(CONFIG_SDK_CHIP_FEATURE_ADDRESS_TABLE_LOOKUP)
#if defined(CONFIG_SDK_RTL8390)
extern rtk_tableField_t RTL8390_L2_CAM_IP_MC_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_tableField_t RTL8390_L2_CAM_IP_MC_SIP_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_tableField_t RTL8390_L2_CAM_MC_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_tableField_t RTL8390_L2_CAM_UC_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_tableField_t RTL8390_L2_IP_MC_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_tableField_t RTL8390_L2_IP_MC_SIP_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_tableField_t RTL8390_L2_MC_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_tableField_t RTL8390_L2_NEXT_HOP_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_tableField_t RTL8390_L2_NH_LEGACY_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_tableField_t RTL8390_L2_UC_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_tableField_t RTL8390_MC_PMSK_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_ADDRESS_TABLE_LOOKUP */
#if defined(CONFIG_SDK_CHIP_FEATURE__IEEE802_1Q__VLAN)
#if defined(CONFIG_SDK_RTL8390)
extern rtk_tableField_t RTL8390_UNTAG_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_tableField_t RTL8390_VLAN_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_tableField_t RTL8390_VLAN_EGR_CNVT_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_tableField_t RTL8390_VLAN_IGR_CNVT_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_tableField_t RTL8390_VLAN_IP_SUBNET_BASED_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_tableField_t RTL8390_VLAN_MAC_BASED_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE__IEEE802_1Q__VLAN */
#if defined(CONFIG_SDK_CHIP_FEATURE_SPANNING_TREE)
#if defined(CONFIG_SDK_RTL8390)
extern rtk_tableField_t RTL8390_MSTI_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_SPANNING_TREE */
#if defined(CONFIG_SDK_CHIP_FEATURE_METER_MARKER)
#if defined(CONFIG_SDK_RTL8390)
extern rtk_tableField_t RTL8390_METER_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_METER_MARKER */
#if defined(CONFIG_SDK_CHIP_FEATURE_STATISTIC_COUNTERS)
#if defined(CONFIG_SDK_RTL8390)
extern rtk_tableField_t RTL8390_LOG_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_STATISTIC_COUNTERS */
#if defined(CONFIG_SDK_CHIP_FEATURE_FLOWCONTROL___BACKPRESSURE_THRESHOLD)
#if defined(CONFIG_SDK_RTL8390)
extern rtk_tableField_t RTL8390_OUT_Q_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_FLOWCONTROL___BACKPRESSURE_THRESHOLD */
#if defined(CONFIG_SDK_CHIP_FEATURE_SCHEDULING)
#if defined(CONFIG_SDK_RTL8390)
extern rtk_tableField_t RTL8390_SCHED_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_SCHEDULING */
#if defined(CONFIG_SDK_CHIP_FEATURE_INGRESS_AND_EGRESS_ACL)
#if defined(CONFIG_SDK_RTL8390)
extern rtk_tableField_t RTL8390_EACL_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_tableField_t RTL8390_IACL_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_INGRESS_AND_EGRESS_ACL */
#if defined(CONFIG_SDK_CHIP_FEATURE_SMART_PACKET_GENERATOR)
#if defined(CONFIG_SDK_RTL8390)
extern rtk_tableField_t RTL8390_SPG_PORT_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_SMART_PACKET_GENERATOR */
#if defined(CONFIG_SDK_CHIP_FEATURE_L3_ROUTING)
#if defined(CONFIG_SDK_RTL8390)
extern rtk_tableField_t RTL8390_ROUTING_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_L3_ROUTING */
#if defined(CONFIG_SDK_CHIP_FEATURE_L3_VPN_MPLS)
#if defined(CONFIG_SDK_RTL8390)
extern rtk_tableField_t RTL8390_MPLS_LIB_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_L3_VPN_MPLS */



#endif    /* __RTK_CYPRESS_TABLEFIELD_LIST_H__ */
