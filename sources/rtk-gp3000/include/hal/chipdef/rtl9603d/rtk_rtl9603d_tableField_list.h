#ifndef __RTK_RTL9603D_TABLEFIELD_LIST_H__
#define __RTK_RTL9603D_TABLEFIELD_LIST_H__

/*
 * Include Files
 */
#include <common/rt_autoconf.h>
#ifdef __BOOTLOADER__
#include <hal/chipdef/rtl9603d/rtk_rtl9603d_uboot_feature_def.h>
#else   /* __BOOTLOADER__ */
#include <hal/chipdef/rtl9603d/rtk_rtl9603d_feature_def.h>
#endif  /* __BOOTLOADER__ */

#if defined(CONFIG_SDK_CHIP_FEATURE_PARSER_HSB)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_tableField_t RTL9603D_HSB_DATA_TABLE_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_tableField_t RTL9603D_HSB_PAR_TABLE_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_PARSER_HSB */
#if defined(CONFIG_SDK_CHIP_FEATURE_MODIFIER_HSA)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_tableField_t RTL9603D_HSA_DATA_FB_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_tableField_t RTL9603D_HSA_DATA_NAT_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_tableField_t RTL9603D_HSA_DATA_NORMAL_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_tableField_t RTL9603D_HSA_DATA_OMCI_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_tableField_t RTL9603D_HSA_DATA_PTP_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_MODIFIER_HSA */
#if defined(CONFIG_SDK_CHIP_FEATURE_DEBUGGING__ALE__LOOPBACK__DROP_MECHANISM__FC_AND_QM_)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_tableField_t RTL9603D_HSA_DEBUG_DATA_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_DEBUGGING__ALE__LOOPBACK__DROP_MECHANISM__FC_AND_QM_ */
#if defined(CONFIG_SDK_CHIP_FEATURE_TABLE_ACCESS)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_tableField_t RTL9603D_ACL_ACTION_TABLE_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_tableField_t RTL9603D_ACL_DATA_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_tableField_t RTL9603D_ACL_MASK_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_tableField_t RTL9603D_L2_MC_DSL_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_tableField_t RTL9603D_L2_UC_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_tableField_t RTL9603D_L3_MC_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_tableField_t RTL9603D_L3_MC_FID_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_tableField_t RTL9603D_L3_MC_VID_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_tableField_t RTL9603D_VLAN_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_TABLE_ACCESS */
#if defined(CONFIG_SDK_CHIP_FEATURE_EPON_CONFIGURATION)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_tableField_t RTL9603D_EPON_GRANT_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_EPON_CONFIGURATION */
#if defined(CONFIG_SDK_CHIP_FEATURE_TABLE_ACCESS)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_tableField_t RTL9603D_CAM_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_tableField_t RTL9603D_CAM_TAG_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_tableField_t RTL9603D_ETHER_TYPE_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_tableField_t RTL9603D_FB_EXT_PORT_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_tableField_t RTL9603D_FLOW_TABLE_PATH1_2_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_tableField_t RTL9603D_FLOW_TABLE_PATH3_4_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_tableField_t RTL9603D_FLOW_TABLE_PATH5_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_tableField_t RTL9603D_FLOW_TABLE_PATH6_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_tableField_t RTL9603D_FLOW_TABLE_TAG_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_tableField_t RTL9603D_INTERFACE_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_tableField_t RTL9603D_MAC_IDX_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_tableField_t RTL9603D_TCAM_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_tableField_t RTL9603D_TCAM_RAW_TABLE_PATH1_2_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_tableField_t RTL9603D_TCAM_RAW_TABLE_PATH3_5_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_TABLE_ACCESS */
#if defined(CONFIG_SDK_CHIP_FEATURE_FB_HSBA)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_tableField_t RTL9603D_L34_HSA_NON_PATH5_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_tableField_t RTL9603D_L34_HSA_PATH5_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_tableField_t RTL9603D_L34_HSA_TRAP_DROP_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_tableField_t RTL9603D_L34_HSB_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_FB_HSBA */



#endif    /* __RTK_RTL9603D_TABLEFIELD_LIST_H__ */
