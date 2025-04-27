#ifndef __RTK_LONGAN_TABLEFIELD_LIST_H__
#define __RTK_LONGAN_TABLEFIELD_LIST_H__

/*
 * Include Files
 */
#include <common/rt_autoconf.h>
#ifdef __BOOTLOADER__
#include <hal/chipdef/longan/rtk_longan_uboot_feature_def.h>
#else   /* __BOOTLOADER__ */
#include <hal/chipdef/longan/rtk_longan_feature_def.h>
#endif  /* __BOOTLOADER__ */

#if defined(CONFIG_SDK_CHIP_FEATURE_802_1Q_VLAN___QINQ)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_tableField_t RTL9300_UNTAG_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_tableField_t RTL9300_VLAN_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_802_1Q_VLAN___QINQ */
#if defined(CONFIG_SDK_CHIP_FEATURE_VLAN_TRANSLATION)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_tableField_t RTL9300_VLAN_EGR_CNVT_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_tableField_t RTL9300_VLAN_IGR_CNVT_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_tableField_t RTL9300_VLAN_IP_BASED_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_tableField_t RTL9300_VLAN_MAC_BASED_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_VLAN_TRANSLATION */
#if defined(CONFIG_SDK_CHIP_FEATURE_LINK_AGGREGATION)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_tableField_t RTL9300_LAG_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_tableField_t RTL9300_SRC_TRK_MAP_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_LINK_AGGREGATION */
#if defined(CONFIG_SDK_CHIP_FEATURE_SPANNING_TREE)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_tableField_t RTL9300_MSTI_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_SPANNING_TREE */
#if defined(CONFIG_SDK_CHIP_FEATURE_PORT_ISOLATION)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_tableField_t RTL9300_PORT_ISO_CTRL_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_PORT_ISOLATION */
#if defined(CONFIG_SDK_CHIP_FEATURE_MAC_FORWARDING_CONTROL)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_tableField_t RTL9300_L2_CAM_MC_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_tableField_t RTL9300_L2_CAM_UC_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_tableField_t RTL9300_L2_MC_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_tableField_t RTL9300_L2_UC_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_tableField_t RTL9300_MC_PORTMASK_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_MAC_FORWARDING_CONTROL */
#if defined(CONFIG_SDK_CHIP_FEATURE_METER_MARKER)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_tableField_t RTL9300_METER_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_METER_MARKER */
#if defined(CONFIG_SDK_CHIP_FEATURE_REMARKING)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_tableField_t RTL9300_REMARK_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_REMARKING */
#if defined(CONFIG_SDK_CHIP_FEATURE_LAYER_3_ROUTING)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_tableField_t RTL9300_L3_EGR_INTF_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_tableField_t RTL9300_L3_EGR_INTF_LIST_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_tableField_t RTL9300_L3_EGR_INTF_MAC_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_tableField_t RTL9300_L3_HOST_ROUTE_IP6MC_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_tableField_t RTL9300_L3_HOST_ROUTE_IP6UC_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_tableField_t RTL9300_L3_HOST_ROUTE_IPMC_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_tableField_t RTL9300_L3_HOST_ROUTE_IPUC_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_tableField_t RTL9300_L3_NEXTHOP_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_tableField_t RTL9300_L3_PREFIX_ROUTE_IP6MC_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_tableField_t RTL9300_L3_PREFIX_ROUTE_IP6UC_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_tableField_t RTL9300_L3_PREFIX_ROUTE_IPMC_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_tableField_t RTL9300_L3_PREFIX_ROUTE_IPUC_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_tableField_t RTL9300_L3_ROUTER_MAC_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_LAYER_3_ROUTING */
#if defined(CONFIG_SDK_CHIP_FEATURE_ACL)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_tableField_t RTL9300_IACL_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_tableField_t RTL9300_LOG_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_tableField_t RTL9300_VACL_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_ACL */
#if defined(CONFIG_SDK_CHIP_FEATURE_PARSER_HSB)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_tableField_t RTL9300_HSB_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_PARSER_HSB */
#if defined(CONFIG_SDK_CHIP_FEATURE_MODIFIER_HSA)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_tableField_t RTL9300_HSA_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_MODIFIER_HSA */



#endif    /* __RTK_LONGAN_TABLEFIELD_LIST_H__ */
