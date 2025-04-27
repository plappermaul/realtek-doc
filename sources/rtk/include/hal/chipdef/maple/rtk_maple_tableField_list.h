#ifndef __RTK_MAPLE_TABLEFIELD_LIST_H__
#define __RTK_MAPLE_TABLEFIELD_LIST_H__

/*
 * Include Files
 */
#include <common/rt_autoconf.h>
#ifdef __BOOTLOADER__
#include <hal/chipdef/maple/rtk_maple_uboot_feature_def.h>
#else   /* __BOOTLOADER__ */
#include <hal/chipdef/maple/rtk_maple_feature_def.h>
#endif  /* __BOOTLOADER__ */

#if defined(CONFIG_SDK_CHIP_FEATURE_TABLE_ACCESS)
#if defined(CONFIG_SDK_RTL8380)
extern rtk_tableField_t RTL8380_IACL_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8380)
extern rtk_tableField_t RTL8380_L2_CAM_IP_MC_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8380)
extern rtk_tableField_t RTL8380_L2_CAM_IP_MC_SIP_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8380)
extern rtk_tableField_t RTL8380_L2_CAM_MC_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8380)
extern rtk_tableField_t RTL8380_L2_CAM_UC_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8380)
extern rtk_tableField_t RTL8380_L2_IP_MC_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8380)
extern rtk_tableField_t RTL8380_L2_IP_MC_SIP_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8380)
extern rtk_tableField_t RTL8380_L2_MC_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8380)
extern rtk_tableField_t RTL8380_L2_NEXT_HOP_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8380)
extern rtk_tableField_t RTL8380_L2_NEXT_HOP_LEGACY_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8380)
extern rtk_tableField_t RTL8380_L2_UC_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8380)
extern rtk_tableField_t RTL8380_LOG_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8380)
extern rtk_tableField_t RTL8380_MC_PMSK_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8380)
extern rtk_tableField_t RTL8380_MSTI_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8380)
extern rtk_tableField_t RTL8380_ROUTING_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8380)
extern rtk_tableField_t RTL8380_UNTAG_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8380)
extern rtk_tableField_t RTL8380_VLAN_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8380)
extern rtk_tableField_t RTL8380_VLAN_EGR_CNVT_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_TABLE_ACCESS */



#endif    /* __RTK_MAPLE_TABLEFIELD_LIST_H__ */
