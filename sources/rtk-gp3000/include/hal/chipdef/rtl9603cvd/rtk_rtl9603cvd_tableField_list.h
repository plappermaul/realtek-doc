#ifndef __RTK_RTL9603CVD_TABLEFIELD_LIST_H__
#define __RTK_RTL9603CVD_TABLEFIELD_LIST_H__

/*
 * Include Files
 */
#include <common/rt_autoconf.h>
#ifdef __BOOTLOADER__
#include <hal/chipdef/rtl9603cvd/rtk_rtl9603cvd_uboot_feature_def.h>
#else   /* __BOOTLOADER__ */
#include <hal/chipdef/rtl9603cvd/rtk_rtl9603cvd_feature_def.h>
#endif  /* __BOOTLOADER__ */

#if defined(CONFIG_SDK_CHIP_FEATURE_PARSER_HSB)
#if defined(CONFIG_SDK_RTL9603CVD)
extern rtk_tableField_t RTL9603CVD_HSB_DATA_TABLE_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
extern rtk_tableField_t RTL9603CVD_HSB_PAR_TABLE_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_PARSER_HSB */
#if defined(CONFIG_SDK_CHIP_FEATURE_MODIFIER_HSA)
#if defined(CONFIG_SDK_RTL9603CVD)
extern rtk_tableField_t RTL9603CVD_HSA_DATA_FB_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
extern rtk_tableField_t RTL9603CVD_HSA_DATA_NAT_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
extern rtk_tableField_t RTL9603CVD_HSA_DATA_NORMAL_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
extern rtk_tableField_t RTL9603CVD_HSA_DATA_OMCI_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
extern rtk_tableField_t RTL9603CVD_HSA_DATA_PTP_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_MODIFIER_HSA */
#if defined(CONFIG_SDK_CHIP_FEATURE_DEBUGGING__ALE__LOOPBACK__DROP_MECHANISM__FC_AND_QM_)
#if defined(CONFIG_SDK_RTL9603CVD)
extern rtk_tableField_t RTL9603CVD_HSA_DEBUG_DATA_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_DEBUGGING__ALE__LOOPBACK__DROP_MECHANISM__FC_AND_QM_ */
#if defined(CONFIG_SDK_CHIP_FEATURE_TABLE_ACCESS)
#if defined(CONFIG_SDK_RTL9603CVD)
extern rtk_tableField_t RTL9603CVD_ACL_ACTION_TABLE_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
extern rtk_tableField_t RTL9603CVD_ACL_DATA_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
extern rtk_tableField_t RTL9603CVD_ACL_MASK_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
extern rtk_tableField_t RTL9603CVD_L2_MC_DSL_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
extern rtk_tableField_t RTL9603CVD_L2_UC_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
extern rtk_tableField_t RTL9603CVD_L3_MC_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
extern rtk_tableField_t RTL9603CVD_L3_MC_FID_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
extern rtk_tableField_t RTL9603CVD_L3_MC_VID_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
extern rtk_tableField_t RTL9603CVD_VLAN_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_TABLE_ACCESS */
#if defined(CONFIG_SDK_CHIP_FEATURE_EPON_CONFIGURATION)
#if defined(CONFIG_SDK_RTL9603CVD)
extern rtk_tableField_t RTL9603CVD_EPON_GRANT_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_EPON_CONFIGURATION */
#if defined(CONFIG_SDK_CHIP_FEATURE_TABLE_ACCESS)
#if defined(CONFIG_SDK_RTL9603CVD)
extern rtk_tableField_t RTL9603CVD_CAM_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
extern rtk_tableField_t RTL9603CVD_CAM_TAG_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
extern rtk_tableField_t RTL9603CVD_ETHER_TYPE_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
extern rtk_tableField_t RTL9603CVD_FB_EXT_PORT_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
extern rtk_tableField_t RTL9603CVD_FLOW_TABLE_PATH1_2_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
extern rtk_tableField_t RTL9603CVD_FLOW_TABLE_PATH3_4_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
extern rtk_tableField_t RTL9603CVD_FLOW_TABLE_PATH5_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
extern rtk_tableField_t RTL9603CVD_FLOW_TABLE_PATH6_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
extern rtk_tableField_t RTL9603CVD_FLOW_TABLE_TAG_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
extern rtk_tableField_t RTL9603CVD_INTERFACE_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
extern rtk_tableField_t RTL9603CVD_MAC_IDX_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
extern rtk_tableField_t RTL9603CVD_TCAM_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
extern rtk_tableField_t RTL9603CVD_TCAM_RAW_TABLE_PATH1_2_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
extern rtk_tableField_t RTL9603CVD_TCAM_RAW_TABLE_PATH3_5_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_TABLE_ACCESS */
#if defined(CONFIG_SDK_CHIP_FEATURE_FB_HSBA)
#if defined(CONFIG_SDK_RTL9603CVD)
extern rtk_tableField_t RTL9603CVD_L34_HSA_NON_PATH5_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
extern rtk_tableField_t RTL9603CVD_L34_HSA_PATH5_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
extern rtk_tableField_t RTL9603CVD_L34_HSA_TRAP_DROP_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
extern rtk_tableField_t RTL9603CVD_L34_HSB_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_FB_HSBA */



#endif    /* __RTK_RTL9603CVD_TABLEFIELD_LIST_H__ */
