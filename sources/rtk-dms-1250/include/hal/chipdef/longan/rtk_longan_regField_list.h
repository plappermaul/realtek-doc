#ifndef __RTK_LONGAN_REGFIELD_LIST_H__
#define __RTK_LONGAN_REGFIELD_LIST_H__

/*
 * Include Files
 */
#include <common/rt_autoconf.h>
#ifdef __BOOTLOADER__
#include <hal/chipdef/longan/rtk_longan_uboot_feature_def.h>
#else   /* __BOOTLOADER__ */
#include <hal/chipdef/longan/rtk_longan_feature_def.h>
#endif  /* __BOOTLOADER__ */

#if defined(CONFIG_SDK_CHIP_FEATURE_CHIP_INFORMATION)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MODEL_NAME_INFO_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t CHIP_INFO_RTL9300_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_CHIP_INFORMATION */
#if defined(CONFIG_SDK_CHIP_FEATURE_RESET)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t RST_GLB_CTRL_0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t RST_GLB_STS_0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t RST_GLB_STS_1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t RST_GLB_STS_2_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t RST_GLB_STS_3_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_RST_DUR_RTL9300_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_RESET */
#if defined(CONFIG_SDK_CHIP_FEATURE_PLL___BIAS)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t PLL_GLB_CTRL0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t PLL_GLB_CTRL1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t PLL_CPU_CTRL0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t PLL_CPU_CTRL1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t PLL_CPU_MISC_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t PLL_SW_CTRL0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t PLL_SW_CTRL1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t PLL_SW_MISC_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t PLL_SW_DIV_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t PLL_125M_CTRL0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t PLL_125M_CTRL1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t PLL_125M_MISC_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t PLL_BANDGAP_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t XTAL_CML_CTRL__RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t PLL_CML_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG0_CHIP_PLL_RTL9300_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_PLL___BIAS */
#if defined(CONFIG_SDK_CHIP_FEATURE_BIST___BISR)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_BIST_MODE_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_DRF_BIST_MODE_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_BIST_RSTN_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_DRF_TEST_RESUME_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_GRXF0_RESULT_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_GTXF0_RESULT_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_TGRXF_RESULT_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_TGTXF_RESULT_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_GRXF1_RESULT0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_GTXF1_RESULT0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_DMY_RESULT_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_LBRXF_RESULT_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_DRF_PAUSE_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_GRXF0_GTXF0_SRAM_LS_EN_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_GRXF0_GTXF0_TIMING_CFG_EN_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_GRXF0_RMA_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_GRXF0_RMB_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_GTXF0_RMA_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_GTXF0_RMB_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_TGRXF_TGTXF_SRAM_LS_EN_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_TGRXF_TGTXF_TIMING_CFG_EN_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_TGRXF_RMA_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_TGRXF_RMB_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_TGTXF_RMA_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_TGTXF_RMB_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_GRXF1_GTXF1_SRAM_LS_EN_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_GRXF1_GTXF1_TIMING_CFG_EN_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_GRXF1_RMA_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_GRXF1_RMB_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_GTXF1_RMA_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_GTXF1_RMB_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_DMY_SRAM_LS_EN_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_DMY_TIMING_CFG_EN_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_DMY_RMA_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_DMY_RMB_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_LBRXF_SRAM_LS_EN_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_LBRXF_TIMING_CFG_EN_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_LBRXF_RMA_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_LBRXF_RMB_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ALE_BIST_MODE0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ALE_BIST_MODE1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ALE_DRF_BIST_MODE0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ALE_DRF_BIST_MODE1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ALE_DRF_BIST_RESUME0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ALE_DRF_BIST_RESUME1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ALE_TIMING_CFG0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ALE_TIMING_CFG1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ALE_TIMING_CFG2_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ALE_TIMING_CFG3_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ALE_RM_0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ALE_RM_1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ALE_RM_2_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ALE_RM_3_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ALE_RM_4_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ALE_RM_5_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ALE_RM_6_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ALE_RM_7_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ALE_RM_8_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ALE_RM_9_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ALE_RM_10_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ALE_RM_11_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ALE_RM_12_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ALE_RM_13_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ALE_RM_14_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ALE_RM_15_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ALE_CAM_TIMING_MDS_0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ALE_CAM_TIMING_MDS_1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ALE_CAM_TIMING_MDS_3_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ALE_CAM_TIMING_MDS_4_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ALE_CAM_TIMING_MDS_5_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ALE_CAM_TIMING_MDS_2_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ALE_CAM_TIMING_RDS_0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ALE_CAM_TIMING_RDS_1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ALE_CAM_TIMING_RDS_2_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ALE_CAM_TIMING_RDS_3_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ALE_CAM_TIMING_RDS_6_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ALE_CAM_TIMING_RDS_7_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ALE_CAM_TIMING_RDS_4_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ALE_CAM_TIMING_RDS_5_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ALE_BIST_DONE0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ALE_BIST_DONE1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ALE_DRF_BIST_PAUSE0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ALE_DRF_BIST_PAUSE1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ALE_BIST_FAIL0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ALE_BIST_FAIL1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ALE_BIST_FAIL2_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ALE_BIST_FAIL3_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ALE_BIST_FAIL4_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ALE_BIST_FAIL5_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ALE_DRF_BIST_DONE0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ALE_DRF_BIST_DONE1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ALE_DRF_BIST_FAIL0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ALE_DRF_BIST_FAIL1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ALE_DRF_BIST_FAIL2_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ALE_DRF_BIST_FAIL3_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ALE_DRF_BIST_FAIL4_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ALE_DRF_BIST_FAIL5_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ALE_BIST_LS_MODE_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t CHIP_BIST_MODE_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t CHIP_DRF_BIST_MODE_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t CHIP_BIST_RSTN_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t CHIP_DRF_TEST_RESUME_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t CHIP_BIST_DONE_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t CHIP_DRF_BIST_DONE_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t CHIP_BIST_FAIL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t CHIP_DRF_BIST_FAIL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t CHIP_DRF_START_PAUSE_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t CHIP_ALL_RESULT_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t CHIP_SRAM_LS_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t INGR_BIST_CTRL0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t INGR_BIST_CTRL1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t INGR_BIST_RSLT0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t INGR_BIST_RSLT1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t INGR_BIST_RSLT2_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t INGR_BIST_RSLT3_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t INGR_BIST_RSLT4_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t INGR_SRAM_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t INGR_BISR_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t INGR_BISR_RSLT0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t INGR_BISR_RSLT1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EGR_BIST_CTRL0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EGR_BIST_CTRL1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EGR_BIST_RSLT0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EGR_BIST_RSLT1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EGR_BIST_RSLT2_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EGR_BIST_RSLT3_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EGR_BIST_RSLT4_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EGR_SRAM_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SOC_BIST_CTRL0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SOC_BIST_CTRL1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SOC_BIST_CTRL2_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SOC_BIST_CTRL3_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SOC_BIST_CTRL4_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SOC_BIST_CTRL5_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SOC_BIST_CTRL6_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SOC_BIST_RSTL0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SOC_BIST_RSTL1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SOC_BIST_RSTL2_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SOC_BIST_RSTL3_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SOC_BIST_MISC0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SOC_BIST_MISC1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG0_MAC_BIST_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG0_ALE_BIST_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG0_CHIP_BIST_RTL9300_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_BIST___BISR */
#if defined(CONFIG_SDK_CHIP_FEATURE_INTERFACE)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_IF_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_SLV_I2C_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_SLV_TIMEOUT_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t I2C_MST1_CTRL1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t I2C_MST1_CTRL2_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t I2C_MST1_DATA_WORD0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t I2C_MST1_DATA_WORD1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t I2C_MST1_DATA_WORD2_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t I2C_MST1_DATA_WORD3_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t I2C_MST_GLB_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t I2C_MST2_CTRL1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t I2C_MST2_CTRL2_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t I2C_MST2_DATA_WORD0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t I2C_MST2_DATA_WORD1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t I2C_MST2_DATA_WORD2_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t I2C_MST2_DATA_WORD3_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SPI_CTRL1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SPI_CTRL2_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SPI_ADDR_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SPI_DATA_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t P0_INTF_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t GPIO_SEL_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t JTAG_SEL_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG0_REG_IF_RTL9300_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_INTERFACE */
#if defined(CONFIG_SDK_CHIP_FEATURE_LED)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t LED_GLB_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t LED_PORT_NUM_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t LED_SET3_1_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t LED_SET3_0_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t LED_SET2_1_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t LED_SET2_0_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t LED_SET1_1_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t LED_SET1_0_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t LED_SET0_1_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t LED_SET0_0_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t LED_PORT_COPR_SET_SEL_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t LED_PORT_FIB_SET_SEL_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t LED_PORT_COPR_MASK_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t LED_PORT_FIB_MASK_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t LED_PORT_COMBO_MASK_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SW_LED_LOAD_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t LED_PORT_SW_EN_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t LED_PORT_SW_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t LED_INDRT_ACCESS_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t LED_LOAD_LV1_10G_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t LED_LOAD_LV2_10G_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t LED_LOAD_LV3_10G_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t LED_LOAD_LV1_5G_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t LED_LOAD_LV2_5G_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t LED_LOAD_LV3_5G_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t LED_LOAD_LV1_2P5G_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t LED_LOAD_LV2_2P5G_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t LED_LOAD_LV3_2P5G_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t LED_LOAD_LV1_1G_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t LED_LOAD_LV2_1G_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t LED_LOAD_LV3_1G_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t LED_LOAD_LV1_500M_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t LED_LOAD_LV2_500M_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t LED_LOAD_LV3_500M_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t LED_LOAD_LV1_100M_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t LED_LOAD_LV2_100M_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t LED_LOAD_LV3_100M_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t LED_LOAD_LV1_10M_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t LED_LOAD_LV2_10M_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t LED_LOAD_LV3_10M_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t LED_P_LOAD_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EXT_GPIO_GLB_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EXT_GPIO_TRIG_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EXT_GPIO_DIR_CTRL_1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EXT_GPIO_DIR_CTRL_2_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EXT_GPIO_DIR_CTRL_3_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EXT_GPIO_DATA_CTRL_1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EXT_GPIO_DATA_CTRL_2_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EXT_GPIO_DATA_CTRL_3_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EXT_GPIO_INDRT_ACCESS_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG0_GLB_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG0_LED_RTL9300_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_LED */
#if defined(CONFIG_SDK_CHIP_FEATURE_INTERRUPT)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t IMR_GLB_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t IMR_PORT_LINK_STS_CHG_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t IMR_RSVD_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t IMR_SERDES_LINK_FAULT_P_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t IMR_SERDES_RX_SYM_ERR_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t IMR_OAM_DYGASP_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t IMR_EXT_GPIO0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t IMR_EXT_GPIO1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t IMR_TM_RLFD_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t IMR_AUTO_REC_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t IMR_SDS_UPD_PHYSTS0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t IMR_ROUT_LPBUF_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ISR_GLB_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ISR_SW_INT_MODE_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ISR_PORT_LINK_STS_CHG_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ISR_RSVD_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ISR_SERDES_LINK_FAULT_P_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ISR_SERDES_RX_SYM_ERR_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ISR_OAM_DYGASP_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ISR_EXT_GPIO0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ISR_EXT_GPIO1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ISR_EXT_GPIO_MODE0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ISR_EXT_GPIO_MODE1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ISR_EXT_GPIO_MODE2_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ISR_TM_RLFD_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ISR_AUTO_REC_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ISR_SDS_UPD_PHYSTS_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ISR_ROUT_LPBUF_RTL9300_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_INTERRUPT */
#if defined(CONFIG_SDK_CHIP_FEATURE_HW_MISC_)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EFUSE_ACCESS_EN_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EFUSE_ACCESS_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EFUSE_WDATA_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EFUSE_RDATA_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t CALIB_CTRL_1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t CALIB_CTRL_2_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t CALIB_CTRL_3_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t CALIB_CTRL_4_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SPD_SENSOR0_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SPD_SENSOR1_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SPD_SENSOR2_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SPD_SENSOR3_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SPD_SENSOR0_RESULT_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SPD_SENSOR1_RESULT_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SPD_SENSOR2_RESULT_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SPD_SENSOR3_RESULT_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SPD_DEBUG_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t THERMAL_METER_CTRL_0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t THERMAL_METER_CTRL_1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t THERMAL_METER_CTRL_2_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t THERMAL_METER_RESULT_0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t THERMAL_METER_RESULT_1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t IO_DRIVING_ABILITY_CTRL0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t IO_DRIVING_ABILITY_CTRL1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t IO_DRIVING_ABILITY_CTRL2_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t IO_DRIVING_ABILITY_CTRL3_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t IO_DRIVING_ABILITY_CTRL4_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t IO_DRIVING_ABILITY_CTRL5_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t IO_DRIVING_ABILITY_CTRL6_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t IO_DRIVING_ABILITY_CTRL7_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t IO_DRIVING_ABILITY_CTRL8_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t FT_SCAN_MODE_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SOC_DEBUG_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t BOND_DBG_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STRP_DBG_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DBG_MODE_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DBG_SEL0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DBG_SEL1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DBG_SEL2_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DBG_SEL3_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DBG_SIG_SEL0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DBG_SIG_SEL1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DBG_SIG_SEL2_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DBG_SIG_SEL3_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DBG_VAL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG0_CHIP_MISC_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG0_CHIP_MISC_R_RTL9300_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_HW_MISC_ */
#if defined(CONFIG_SDK_CHIP_FEATURE_NIC___DMA)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMA_IF_RX_BASE_DESC_ADDR_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMA_IF_RX_CUR_DESC_ADDR_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMA_IF_TX_BASE_DESC_ADDR_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMA_IF_TX_CUR_DESC_ADDR_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMA_IF_INTR_RX_RUNOUT_MSK_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMA_IF_INTR_RX_DONE_MSK_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMA_IF_INTR_TX_DONE_MSK_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMA_IF_INTR_RX_RUNOUT_STS_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMA_IF_INTR_RX_DONE_STS_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMA_IF_INTR_TX_DONE_STS_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMA_IF_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMA_IF_PKT_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMA_IF_RX_RING_SIZE_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMA_IF_RX_RING_CNTR_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMA_IF_PHYSICAL_ADDR_MSK_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t NIC_DBG_SEL_0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t NIC_DBG_SEL_1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t NIC_BIST_CTRL_0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t NIC_BIST_CTRL_1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG0_NIC_RTL9300_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_NIC___DMA */
#if defined(CONFIG_SDK_CHIP_FEATURE_APPLICATION_TRAP)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SPCL_SWITCH_IPV4_ADDR_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SPCL_SWITCH_IPV6_ADDR_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SPCL_TRAP_CAPWAP_PORT_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SPCL_TRAP_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SPCL_TRAP_PORT_CTRL_RTL9300_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_APPLICATION_TRAP */
#if defined(CONFIG_SDK_CHIP_FEATURE_TABLE_ACCESS)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t TBL_ACCESS_L2_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t TBL_ACCESS_L2_METHOD_CTRL0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t TBL_ACCESS_L2_METHOD_CTRL1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t TBL_ACCESS_L2_METHOD_CTRL2_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t TBL_ACCESS_L2_DATA_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t TBL_ACCESS_CTRL_0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t TBL_ACCESS_DATA_0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t TBL_ACCESS_CTRL_1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t TBL_ACCESS_DATA_1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t TBL_ACCESS_CTRL_2_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t TBL_ACCESS_DATA_2_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t TBL_ACCESS_HSB_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t TBL_ACCESS_HSB_DATA_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t TBL_ACCESS_HSA_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t TBL_ACCESS_HSA_DATA_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG0_ALE_TAB_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG0_RT_TAB_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG0_PKT_ENCAP_RTL9300_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_TABLE_ACCESS */
#if defined(CONFIG_SDK_CHIP_FEATURE_MAC_CONTROL)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_GLB_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_PORT_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HALF_CHG_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_EEPROM_DOWN_LOAD_CNTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_EEPROM_DOWN_LOAD_STS_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_EEPROM_DOWN_LOAD_MAC_POS_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_EEPROM_DOWN_LOAD_SERDES_POS_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_EEPROM_DOWN_LOAD_PHY_POS_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_EEPROM_DOWN_LOAD_GROUP_MAC_POS_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SMI_GLB_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SMI_MAC_TYPE_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SMI_PORT0_15_POLLING_SEL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SMI_PORT16_27_POLLING_SEL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SMI_PRVTE_POLLING_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MDIO_FREE_CNT_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SMI_10GPHY_POLLING_SEL_0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_FORCE_MODE_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SMI_POLL_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SMI_REG_CHK1_CTRL0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SMI_REG_CHK1_CTRL1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SMI_REG_CHK1_PMSK_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SMI_REG_CHK1_DATA_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SMI_REG_CHK1_DATA_10G_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SMI_REG_CHK1_RESULT_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SMI_REG_CHK2_CTRL0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SMI_REG_CHK2_CTRL1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SMI_REG_CHK2_PMSK_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SMI_REG_CHK2_DATA_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SMI_REG_CHK2_DATA_10G_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SMI_REG_CHK2_RESULT_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SMI_REG_CHK3_CTRL0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SMI_REG_CHK3_CTRL1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SMI_REG_CHK3_PMSK_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SMI_REG_CHK3_DATA_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SMI_REG_CHK3_DATA_10G_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SMI_REG_CHK3_RESULT_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SMI_REG_CHK4_CTRL0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SMI_REG_CHK4_CTRL1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SMI_REG_CHK4_PMSK_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SMI_REG_CHK4_DATA_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SMI_REG_CHK4_DATA_10G_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SMI_REG_CHK4_RESULT_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SMI_REG_CHK5_CTRL0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SMI_REG_CHK5_CTRL1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SMI_REG_CHK5_PMSK_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SMI_REG_CHK5_DATA_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SMI_REG_CHK5_DATA_10G_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SMI_REG_CHK5_RESULT_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t LINK_DELAY_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_LINK_STS_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_LINK_MEDIA_STS_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_LINK_SPD_STS_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_LINK_DUP_STS_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_TX_PAUSE_STS_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_RX_PAUSE_STS_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_EEE_ABLTY_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_MSTR_SLV_STS_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_MSTR_SLV_FAULT_STS_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t PHY_LINK_STS_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t PHY_LINK_MEDIA_STS_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t PHY_LINK_SPD_STS_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t PHY_LINK_DUP_STS_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t PHY_TX_PAUSE_STS_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t PHY_RX_PAUSE_STS_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t PHY_EEE_ABLTY_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t PHY_MSTR_SLV_STS_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t PHY_MSTR_SLV_FAULT_STS_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SMI_ACCESS_PHY_CTRL_0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SMI_ACCESS_PHY_CTRL_1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SMI_ACCESS_PHY_CTRL_2_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SMI_ACCESS_PHY_CTRL_3_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SMI_PORT0_5_ADDR_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SMI_PORT6_11_ADDR_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SMI_PORT12_17_ADDR_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SMI_PORT18_23_ADDR_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SMI_PORT24_27_ADDR_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SDS_MODE_ADJ_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SMI_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_EFUSE_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t BONDING_REDEFINE_REG_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_L2_GLOBAL_CTRL0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_L2_GLOBAL_CTRL1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_L2_PORT_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_L2_PADDING_SEL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_L2_CPU_PORT_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_L2_CPU_TAG_ID_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_L2_ADDR_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_L2_PORT_MAX_LEN_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_L2_CPU_MAX_LEN_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_SSC_CTRL_0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_SSC_CTRL_1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_SSC_CTRL_2_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_SSC_CTRL_3_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_SSC_CTRL_4_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EXT_SMI_ACCESS_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG0_PER_PORT_MAC_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG0_ALE_GLB_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG1_GLB_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG0_SMI_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG0_CHIP_AFE_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t RLFD_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t RLFD_10G_ADDR_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t UNI_DIR_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SMI_10GPHY_POLLING_SEL_1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SMI_10GPHY_POLLING_REG0_CFG_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SMI_10GPHY_POLLING_REG9_CFG_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SMI_10GPHY_POLLING_REG10_CFG_RTL9300_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_MAC_CONTROL */
#if defined(CONFIG_SDK_CHIP_FEATURE_PHY___SERDES)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t FIB0_INTF_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t FIB4_INTF_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t FIB10_INTF_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SDS_CFG_REG0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SDS_CFG_REG1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SDS_CFG_REG2_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SDS_MODE_SEL_0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SDS_MODE_SEL_1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SDS_MODE_SEL_2_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SDS_MODE_SEL_3_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SDS_INDACS_CMD_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SDS_INDACS_DATA_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SDS_OUI_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SDS_VERSION_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SDS_INTF_CTRL0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SDS_INTF_CTRL1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SDS_INTF_CTRL2_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t WRAP_SDS_INTF_CTRL0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t WRAP_SDS_INTF_CTRL1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t WRAP_SDS_INTF_CTRL3_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t WRAP_SDS_INTF_CTRL4_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t WRAP_SDS_INTF_CTRL5_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t WRAP_SDS_INTF_CTRL6_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t RG2X_RG1X_CEN_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t RG0X_CEN_RTT_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SDS_SUBMODE_CTRL0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SDS_SUBMODE_CTRL1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG0_CHIP_CHIP_AFE_RTL9300_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_PHY___SERDES */
#if defined(CONFIG_SDK_CHIP_FEATURE_POWER_SAVING)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EEE_TX_Q_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EEE_TX_MINIFG_CTRL0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EEE_TX_MINIFG_CTRL1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EEE_TX_MINIFG_CTRL2_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EEE_TX_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EEE_TX_TIMER_100M_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EEE_TX_TIMER_GELITE_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EEE_TX_TIMER_GIGA_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EEE_TX_TIMER_2P5G_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EEE_TX_TIMER_5G_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EEE_TX_TIMER_10G_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EEE_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EEE_RX_GELITE_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EEE_RX_GE_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EEE_RX_2P5G_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EEE_RX_5G_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EEE_RX_10G_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EEEP_PORT_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EEEP_GBL_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EEEP_TIMER_UNIT_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EEEP_TX_RATE_100M_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EEEP_TX_RATE_500M_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EEEP_TX_RATE_GIGA_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EEEP_TX_WAKE_TIMER_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EEEP_RX_RATE_100M_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EEEP_RX_RATE_500M_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EEEP_RX_RATE_GIGA_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EEEP_RX_SLEEP_STEP_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EEEP_RX_TIMER_100M_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EEEP_RX_TIMER_500M_CTRL0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EEEP_RX_TIMER_500M_CTRL1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EEEP_RX_TIMER_GIGA_CTRL0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EEEP_RX_TIMER_GIGA_CTRL1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EEEP_RX_IDLE_TIMER_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EEEP_TX_IDLE_TIMER_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t PS_GATCLK_MASK_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t PS_GATCLK_EN_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t PS_GATCLK_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t PS_SOC_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG0_CHIP_MISC_T_RTL9300_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_POWER_SAVING */
#if defined(CONFIG_SDK_CHIP_FEATURE_802_1Q_VLAN___QINQ)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t VLAN_TAG_TPID_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t VLAN_ETAG_TPID_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t VLAN_PORT_ITAG_TPID_CMP_MSK_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t VLAN_PORT_OTAG_TPID_CMP_MSK_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t VLAN_PORT_ETAG_TPID_CMP_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t VLAN_PORT_AFT_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t VLAN_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t VLAN_PORT_PB_VLAN_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t VLAN_PORT_FWD_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t VLAN_APP_PKT_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t VLAN_PORT_IGR_FLTR_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t VLAN_PORT_EGR_FLTR_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t VLAN_PORT_TAG_STS_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t VLAN_PORT_EGR_TPID_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG0_VLAN_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG1_VLAN_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG1_PKT_ENCAP_RTL9300_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_802_1Q_VLAN___QINQ */
#if defined(CONFIG_SDK_CHIP_FEATURE_VLAN_PROFILE)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t VLAN_PROFILE_SET_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG0_VLAN_PROFILE_RTL9300_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_VLAN_PROFILE */
#if defined(CONFIG_SDK_CHIP_FEATURE__IEEE802_1V__PROTOCOL_BASED_VLAN)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t VLAN_PPB_VLAN_SET_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t VLAN_PORT_PPB_VLAN_SET_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG2_VLAN_RTL9300_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE__IEEE802_1V__PROTOCOL_BASED_VLAN */
#if defined(CONFIG_SDK_CHIP_FEATURE_VLAN_RANGE_CHECK)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t VLAN_IGR_VID_RNG_CHK_SET_0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t VLAN_IGR_VID_RNG_CHK_SET_1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t VLAN_EGR_VID_RNG_CHK_SET_0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t VLAN_EGR_VID_RNG_CHK_SET_1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG0_VLAN_IVC_RTL9300_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_VLAN_RANGE_CHECK */
#if defined(CONFIG_SDK_CHIP_FEATURE_VLAN_TRANSLATION)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t VLAN_IVC_BLK_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t VLAN_PORT_L2TBL_CNVT_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t VLAN_TRUNK_L2TBL_CNVT_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t VLAN_IVC_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t VLAN_EVC_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t VLAN_IVC_ENTRY_INDICATION_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t VLAN_EVC_ENTRY_INDICATION_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG0_L2_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG2_PKT_ENCAP_RTL9300_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_VLAN_TRANSLATION */
#if defined(CONFIG_SDK_CHIP_FEATURE_RMA)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t RMA_CTRL_0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t RMA_CTRL_1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t RMA_CTRL_2_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t RMA_MIRROR_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t RMA_SMAC_LRN_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t RMA_MGN_LRN_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t RMA_PORT_BPDU_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t RMA_PORT_PTP_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t RMA_PORT_LLDP_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t RMA_PORT_EAPOL_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t RMA_FLD_PMSK_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t RMA_BPDU_FLD_PMSK_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t RMA_EAPOL_FLD_PMSK_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t RMA_USR_DEF_FLD_PMSK_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t RMA_LLDP_FLD_PMSK_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t RMA_USR_DEF_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG0_RMA_RTL9300_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_RMA */
#if defined(CONFIG_SDK_CHIP_FEATURE_LINK_AGGREGATION)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t TRK_ID_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t TRK_MBR_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t TRK_HASH_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t TRK_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t TRK_SHFT_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t TRK_LOCAL_TBL_REFRESH_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t TRK_LOCAL_TBL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t TRK_STK_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t LOCAL_PORT_TRK_MAP_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG0_TRUNK_RTL9300_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_LINK_AGGREGATION */
#if defined(CONFIG_SDK_CHIP_FEATURE_SPANNING_TREE)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ST_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG3_VLAN_RTL9300_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_SPANNING_TREE */
#if defined(CONFIG_SDK_CHIP_FEATURE_PORT_ISOLATION)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t PORT_ISO_RESTRICT_ROUTE_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t PORT_ISO_VB_ISO_PMSK_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t PORT_ISO_VB_EGR_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG0_PORT_ISO_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG0_VLAN_ISO_RTL9300_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_PORT_ISOLATION */
#if defined(CONFIG_SDK_CHIP_FEATURE_MAC_FORWARDING_CONTROL)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L2_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L2_AGE_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L2_PORT_AGE_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L2_TRK_AGE_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L2_PORT_SALRN_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L2_PORT_NEW_SA_FWD_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L2_DYN_PORT_MV_ACT_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L2_DYN_PORT_MV_LRN_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L2_STT_PORT_MV_ACT_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L2_STT_PORT_MV_LRN_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L2_STT_TRK_MV_ACT_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L2_STT_TRK_MV_LRN_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L2_GLB_STT_PORT_MV_ACT_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L2_GLB_STT_PORT_MV_LRN_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L2_PORT_MV_FORBID_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L2_TRK_MV_FORBID_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L2_PORT_MV_FORBID_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L2_PORT_SABLK_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L2_PORT_DABLK_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L2_UNKN_UC_FLD_PMSK_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L2_BC_FLD_PMSK_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L2_PORT_UC_LM_ACT_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L2_PORT_L2_MC_LM_ACT_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L2_PORT_IP4_MC_LM_ACT_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L2_PORT_IP6_MC_LM_ACT_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L2_LRN_CONSTRT_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L2_LRN_CONSTRT_CNT_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L2_LRN_PORT_CONSTRT_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L2_LRN_PORT_CONSTRT_CNT_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L2_LRN_TRK_CONSTRT_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L2_LRN_TRK_CONSTRT_CNT_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L2_LRN_VLAN_CONSTRT_ENTRY_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L2_LRN_VLAN_CONSTRT_CNT_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L2_VLAN_CONSTRT_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L2_CONSTRT_PORT_CNT_DBG_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L2_CONSTRT_TRK_CNT_DBG_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L2_CONSTRT_SYS_CNT_DBG_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L2_CONSTRT_VLAN_CNT_DBG_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L2_TBL_FLUSH_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L2_SRC_P_FLTR_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L2_SA_ACT_REF_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L2_HASH_FULL_CNT_RTL9300_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_MAC_FORWARDING_CONTROL */
#if defined(CONFIG_SDK_CHIP_FEATURE_L2_ENTRY_NOTIFICATION)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L2_NTFY_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L2_NTFY_PKT_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L2_NTFY_PKT_TIMEOUT_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L2_NTFY_PKT_MAC_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L2_NTFY_PKT_ITAG_0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L2_NTFY_PKT_ITAG_1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L2_NTFY_RST_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L2_NTFY_PKT_MAGIC_NUM_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L2_NTFY_PKT_LOCAL_THR_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L2_NTFY_PKT_REMOTEL_THR_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L2_NTFY_REMOTEL_CONGEST_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L2_NOTIFY_RING_BASE_ADDR_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L2_NOTIFY_RING_CUR_ADDR_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L2_NOTIFY_IF_INTR_MSK_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L2_NOTIFY_IF_INTR_STS_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMA_L2MSG_TMROUT_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMA_L2MSG_CNT_SEL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMA_RDMA_CNT_SEL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG1_NIC_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG0_SPC_PORT_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG0_NIC_SYS_RTL9300_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_L2_ENTRY_NOTIFICATION */
#if defined(CONFIG_SDK_CHIP_FEATURE_L2_MISC_)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MAC_TX_DISABLE_RTL9300_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_L2_MISC_ */
#if defined(CONFIG_SDK_CHIP_FEATURE_STORM_CONTROL__B_M_UM_DLF_)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STORM_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STORM_LB_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STORM_LB_PPS_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STORM_LB_PROTO_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STORM_PORT_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STORM_PORT_UC_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STORM_PORT_UC_LB_RST_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STORM_PORT_UC_EXCEED_FLAG_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STORM_PORT_MC_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STORM_PORT_MC_LB_RST_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STORM_PORT_MC_EXCEED_FLAG_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STORM_PORT_BC_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STORM_PORT_BC_LB_RST_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STORM_PORT_BC_EXCEED_FLAG_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STORM_PORT_PROTO_DHCP_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STORM_PORT_PROTO_DHCP_LB_RST_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STORM_PORT_PROTO_DHCP_EXCEED_FLAG_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STORM_PORT_PROTO_BPDU_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STORM_PORT_PROTO_BPDU_LB_RST_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STORM_PORT_PROTO_BPDU_EXCEED_FLAG_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STORM_PORT_PROTO_IGMP_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STORM_PORT_PROTO_IGMP_LB_RST_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STORM_PORT_PROTO_IGMP_EXCEED_FLAG_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STORM_PORT_PROTO_ARP_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STORM_PORT_PROTO_ARP_LB_RST_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STORM_PORT_PROTO_ARP_EXCEED_FLAG_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG0_STORM_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG1_STORM_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG0_PRESTORM_RTL9300_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_STORM_CONTROL__B_M_UM_DLF_ */
#if defined(CONFIG_SDK_CHIP_FEATURE_INGRESS_BANDWIDTH_CONTROL)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t IGBW_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t IGBW_LB_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t IGBW_PORT_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t IGBW_PORT_BURST_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t IGBW_PORT_LB_RST_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t IGBW_PORT_EXCEED_FLAG_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t IGBW_PORT_FC_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG0_INBW_RTL9300_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_INGRESS_BANDWIDTH_CONTROL */
#if defined(CONFIG_SDK_CHIP_FEATURE_EGRESS_BANDWIDTH_CONTROL)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EGBW_ENCAP_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EGBW_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EGBW_CPU_PPS_LB_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EGBW_LB_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EGBW_PORT_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EGBW_PORT_LB_RST_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EGBW_PORT_Q_MAX_LB_CTRL_SET0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EGBW_PORT_Q_MAX_LB_CTRL_SET1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EGBW_PORT_Q_MAX_LB_RST_SET0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EGBW_PORT_Q_MAX_LB_RST_SET1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EGBW_CPU_Q_MAX_LB_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EGBW_CPU_Q_MAX_LB_RST_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EGBW_PORT_Q_ASSURED_FIX_BURST_CTRL_SET0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EGBW_PORT_Q_ASSURED_FIX_BURST_CTRL_SET1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EGBW_PORT_Q_ASSURED_LB_CTRL_SET0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EGBW_PORT_Q_ASSURED_LB_CTRL_SET1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EGBW_PORT_Q_FIX_LB_CTRL_SET0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EGBW_PORT_Q_FIX_LB_CTRL_SET1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EGBW_PORT_Q_ASSURED_FIX_LB_RST_SET0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EGBW_PORT_Q_ASSURED_FIX_LB_RST_SET1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EGBW_RATE_10M_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EGBW_RATE_100M_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EGBW_RATE_1G_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EGBW_RATE_500M_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EGBW_RATE_10G_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EGBW_RATE_2500M_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EGBW_RATE_1250M_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EGBW_RATE_5G_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t EGBW_RATE_SXG_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG0_EGRESS_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG0_P28_TXQ_REG_RTL9300_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_EGRESS_BANDWIDTH_CONTROL */
#if defined(CONFIG_SDK_CHIP_FEATURE_METER_MARKER)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t METER_GLB_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t METER_LB_EXCEED_STS_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t METER_LB_GLB_EXCEED_STS_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t METER_CNTR_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t METER_GREEN_CNTR_STS_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t METER_YELLOW_CNTR_STS_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t METER_RED_CNTR_STS_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t METER_TOTAL_CNTR_STS_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t METER_LB_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t METER_LB_PPS_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG0_ALE_METER_RTL9300_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_METER_MARKER */
#if defined(CONFIG_SDK_CHIP_FEATURE_FLOWCONTROL___BACKPRESSURE)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t FC_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t FC_PORT_ACT_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t FC_GLB_SYS_UTIL_THR_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t FC_GLB_DROP_THR_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t FC_GLB_HI_THR_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t FC_GLB_LO_THR_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t FC_GLB_FCOFF_HI_THR_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t FC_GLB_FCOFF_LO_THR_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t FC_JUMBO_HI_THR_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t FC_JUMBO_LO_THR_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t FC_JUMBO_FCOFF_HI_THR_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t FC_JUMBO_FCOFF_LO_THR_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t FC_JUMBO_THR_ADJUST_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t FC_PORT_HI_THR_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t FC_PORT_LO_THR_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t FC_PORT_FCOFF_HI_THR_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t FC_PORT_FCOFF_LO_THR_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t FC_PORT_GUAR_THR_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t FC_PORT_THR_SET_SEL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t FC_PORT_EGR_DROP_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t FC_HOL_PRVNT_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t FC_PORT_Q_EGR_DROP_CTRL_SET0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t FC_PORT_Q_EGR_DROP_CTRL_SET1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t FC_CPU_Q_EGR_DROP_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t FC_PORT_Q_EGR_FORCE_DROP_CTRL_SET0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t FC_PORT_Q_EGR_FORCE_DROP_CTRL_SET1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t FC_CPU_Q_EGR_FORCE_DROP_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t FC_Q_EGR_DROP_THR_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t FC_CPU_Q_EGR_DROP_THR_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t FC_PORT_EGR_DROP_THR_SET_SEL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t FC_LB_PORT_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t FC_LB_PORT_Q_EGR_DROP_THR_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t FC_GLB_PAGE_CNT_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t FC_PORT_PAGE_CNT_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t FC_GLB_PAGE_PEAKCNT_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t FC_PORT_CUR_PAGE_CNT_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t FC_PORT_PEAK_PAGE_CNT_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t FC_PORT_EGR_PAGE_CNT_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t FC_PORT_Q_EGR_PAGE_CNT_SET0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t FC_PORT_Q_EGR_PAGE_CNT_SET1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t FC_CPU_Q_EGR_PAGE_CNT_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t FC_PORT_Q_EGR_PKT_CNT_SET0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t FC_PORT_Q_EGR_PKT_CNT_SET1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t FC_CPU_Q_EGR_PKT_CNT_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t FC_LB_PORT_EGR_PAGE_CNT_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t FC_LB_PORT_EGR_PKT_CNT_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t FC_REPCT_Q_HSM_THR_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t FC_REPCT_Q_HSA_THR_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t FC_PORT_REPCT_FC_EN_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t FC_REPCT_Q_PORT_SEL_EN_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t FC_REPCT_Q_PORT_SEL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t FC_REPCT_Q_INT_PRI_MAPPING_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t FC_REPCT_Q_HSM_CNT_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t FC_REPCT_Q_HSA_CNT_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ETE_FC_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ETE_FC_CASCADE_PORT_DROP_THR_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ETE_FC_ON_REMOTE_PORT_THR_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ETE_FC_OFF_REMOTE_PORT_THR_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ETE_FC_REMOTE_PORT_GUAR_THR_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ETE_FC_REMOTE_PORT_THR_SET_SEL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ETE_FC_REMOTE_TX_PAUSE_STS_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ETE_FC_REMOTE_CONGEST_STS_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ETE_FC_SPECIAL_PAUSE_DMAC_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ETE_FC_SPECIAL_PAUSE_SMAC_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ETE_FC_REMOTE_PORT_PAGE_CNT_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ETE_FC_PORT_PAGE_CNT_ERROR_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ETE_FC_REMOTE_PORT_PAGE_CNT_ERROR_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ETE_FC_L2NTFY_PORT_PAGE_CNT_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ETE_FC_REMOTE_L2NTFY_PORT_PAGE_CNT_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ETE_FC_L2NTFY_PORT_PAGE_CNT_ERROR_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ETE_FC_REMOTE_L2NTFY_PORT_PAGE_CNT_ERROR_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ETE_FC_PAUSE_FRAME_PORT_CONGEST_STS_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG0_PER_PORT_TXQ_REG_28P_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG0_PER_PORT_TXQ_REG_29P_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG1_EGRESS_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG1_P28_TXQ_REG_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG0_RT_REG_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG0_EGRESS_DROP_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG0_INGRESS_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG1_INGRESS_CTRL_RTL9300_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_FLOWCONTROL___BACKPRESSURE */
#if defined(CONFIG_SDK_CHIP_FEATURE_CONGESTION_AVOIDANCE)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SWRED_PORT_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SWRED_QUEUE_DROP_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SWRED_DROP_CNTR_PIDX_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SWRED_DROP_CNTR_CIDX_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SWRED_DROP_CNTR_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SWRED_DROP_CNTR_RST_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SC_P_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SC_P_EN_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG0_ALE_DBG_CNT_RTL9300_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_CONGESTION_AVOIDANCE */
#if defined(CONFIG_SDK_CHIP_FEATURE_INGRESS_PRIORITY_DECISION)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t PRI_SEL_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t PRI_SEL_PORT_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t PRI_SEL_REMAP_PORT_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t PRI_SEL_REMAP_IPRI_CFI0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t PRI_SEL_REMAP_IPRI_CFI1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t PRI_SEL_REMAP_OPRI_DEI0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t PRI_SEL_REMAP_OPRI_DEI1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t PRI_SEL_REMAP_DSCP_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t PRI_SEL_PORT_TBL_IDX_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t PRI_SEL_TBL_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DP_SEL_REMAP_ITAG_CFI0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DP_SEL_REMAP_ITAG_CFI1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DP_SEL_REMAP_OTAG_DEI0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DP_SEL_REMAP_OTAG_DEI1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DP_SEL_REMAP_DSCP_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DP_SEL_PORT_TBL_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG0_INGPRI_RTL9300_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_INGRESS_PRIORITY_DECISION */
#if defined(CONFIG_SDK_CHIP_FEATURE_SCHEDULING___QUEUE_MANAGEMENT)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t QM_INTPRI2QID_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t QM_CPUQID2QID_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t QM_CPUQID2XGQID_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t QM_RSN2CPUQID_CTRL_0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t QM_RSN2CPUQID_CTRL_1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t QM_RSN2CPUQID_CTRL_2_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t QM_RSN2CPUQID_CTRL_3_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t QM_RSN2CPUQID_CTRL_4_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t QM_RSN2CPUQID_CTRL_5_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t QM_RSN2CPUQID_CTRL_6_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t QM_RSN2CPUQID_CTRL_7_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t QM_RSN2CPUQID_CTRL_8_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t QM_FLAG2CPUQID_CTRL_0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t QM_FLAG2CPUQID_CTRL_1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SCHED_PORT_Q_CTRL_SET0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SCHED_PORT_Q_CTRL_SET1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SCHED_CPU_Q_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SCHED_PORT_ALGO_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG2_EGRESS_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG0_QUEUE_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG0_PER_PORT_TXQ_REG_24P_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG0_PORT_TXQ_REG_4P_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG1_PORT_TXQ_REG_4P_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG2_PORT_TXQ_REG_4P_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG3_PORT_TXQ_REG_4P_RTL9300_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_SCHEDULING___QUEUE_MANAGEMENT */
#if defined(CONFIG_SDK_CHIP_FEATURE_REMARKING)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t RMK_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t RMK_PORT_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t RMK_INTPRI2IPRI_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t RMK_IPRI2IPRI_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t RMK_OPRI2IPRI_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t RMK_DSCP2IPRI_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t RMK_INTPRI2OPRI_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t RMK_IPRI2OPRI_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t RMK_OPRI2OPRI_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t RMK_DSCP2OPRI_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t RMK_INTPRI2DEI_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t RMK_DP2DEI_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t RMK_INTPRI2DSCP_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t RMK_IPRI2DSCP_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t RMK_OPRI2DSCP_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t RMK_DSCP2DSCP_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t RMK_DPINTPRI2DSCP_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG3_PKT_ENCAP_RTL9300_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_REMARKING */
#if defined(CONFIG_SDK_CHIP_FEATURE_802_1QAV)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t AVB_PORT_CLASS_A_EN_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t AVB_PORT_CLASS_B_EN_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t AVB_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG0_EAVSPE_RTL9300_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_802_1QAV */
#if defined(CONFIG_SDK_CHIP_FEATURE_LAYER_3_ROUTING)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L3_IP_ROUTE_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L3_HOST_TBL_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L3_IPUC_ROUTE_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L3_IP6UC_ROUTE_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L3_IPMC_ROUTE_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L3_IP6MC_ROUTE_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L3_IP_MTU_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L3_IP6_MTU_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L3_PORT_IP_ROUTE_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L3_PORT_IP6_ROUTE_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L3_ENTRY_COUNTER_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L3_ENTRY_COUNTER_DATA_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L3_ENTRY_MV_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L3_ENTRY_MV_PARAM_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L3_HW_LU_KEY_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L3_HW_LU_KEY_IP_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L3_HW_LU_KEY_DIP_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t L3_HW_LU_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG1_RT_REG_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG2_RT_REG_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG3_RT_REG_RTL9300_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_LAYER_3_ROUTING */
#if defined(CONFIG_SDK_CHIP_FEATURE_MIB_CONTROL)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_RST_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PORT_RST_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_CNT_SET1_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_CNT_SET0_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG0_MIB_CTRL_RTL9300_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_MIB_CONTROL */
#if defined(CONFIG_SDK_CHIP_FEATURE_MIB_COUNTER)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_BRIDGE_DOT1DTPLEARNEDENTRYDISCARDS_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PORT_MIB_CNTR_RTL9300_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_MIB_COUNTER */
#if defined(CONFIG_SDK_CHIP_FEATURE_PRIVATE_COUNTER)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PORT_PRVTE_CNTR_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PORT_PRVTE_E_Q_RST_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PORT_E_DROP_CNTR0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PORT_E_DROP_CNTR1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PORT_E_DROP_CNTR2_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG0_ALE_EGR_CNT_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG1_ALE_EGR_CNT_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG2_ALE_EGR_CNT_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG3_ALE_EGR_CNT_RTL9300_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_PRIVATE_COUNTER */
#if defined(CONFIG_SDK_CHIP_FEATURE_DEBUG_COUNTER)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER2_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER3_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER4_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER5_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER6_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER7_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER8_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER9_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER10_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER11_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER12_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER13_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER14_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER15_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER16_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER17_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER18_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER19_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER20_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER21_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER22_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER23_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER24_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER25_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER26_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER27_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER28_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER29_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER30_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER31_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER32_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER33_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER34_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER35_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER36_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER37_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER38_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER39_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER40_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER41_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER42_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER43_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER44_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER45_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER46_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER47_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER48_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER49_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER50_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER51_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER52_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER53_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER54_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER55_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER56_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER57_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER58_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER59_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER60_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER61_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER62_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER63_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER64_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER65_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER66_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER67_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER68_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER69_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER70_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER71_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER72_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER73_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER74_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER75_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER76_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER77_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER78_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER79_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER80_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER81_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER82_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER83_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER84_RTL9300_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_DEBUG_COUNTER */
#if defined(CONFIG_SDK_CHIP_FEATURE_MIRRORING)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MIR_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MIR_SPM_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MIR_DPM_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MIR_SAMPLE_RATE_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MIR_QID_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG0_MIRROR_RTL9300_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_MIRRORING */
#if defined(CONFIG_SDK_CHIP_FEATURE_RSPAN)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MIR_RSPAN_VLAN_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MIR_RSPAN_TX_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t MIR_RSPAN_RX_TAG_RM_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG4_PKT_ENCAP_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG5_PKT_ENCAP_RTL9300_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_RSPAN */
#if defined(CONFIG_SDK_CHIP_FEATURE_SFLOW)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SFLOW_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SFLOW_PORT_RATE_CTRL_RTL9300_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_SFLOW */
#if defined(CONFIG_SDK_CHIP_FEATURE_PIE_TEMPLATE)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t PIE_BLK_LOOKUP_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t PIE_BLK_PHASE_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t PIE_TMPLTE_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t PIE_BLK_GROUP_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t PIE_BLK_TMPLTE_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t PIE_MV_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t PIE_MV_LEN_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t PIE_CLR_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t PIE_RULE_HIT_INDICATION_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t PIE_GLB_HIT_INDICATION_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t PIE_MISC_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t PIE_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG0_ALE_ACL_GLB_RTL9300_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_PIE_TEMPLATE */
#if defined(CONFIG_SDK_CHIP_FEATURE_ACL)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ACL_PORT_LOOKUP_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ACL_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG1_ALE_ACL_GLB_RTL9300_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_ACL */
#if defined(CONFIG_SDK_CHIP_FEATURE_RANGE_CHECK__PORT_VLAN_IP_L4PORT_)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t RNG_CHK_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t RNG_CHK_IP_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t RNG_CHK_IP_RNG_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG0_ACL_RANGE_RTL9300_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_RANGE_CHECK__PORT_VLAN_IP_L4PORT_ */
#if defined(CONFIG_SDK_CHIP_FEATURE_ATTACK_PREVENTION)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ATK_PRVNT_PORT_EN_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ATK_PRVNT_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ATK_PRVNT_ACT_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ATK_PRVNT_IPV6_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ATK_PRVNT_ICMP_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ATK_PRVNT_TCP_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ATK_PRVNT_SMURF_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ATK_PRVNT_STS_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ATK_PRVNT_ARP_INVLD_PORT_ACT_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG0_ATTACK_RTL9300_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_ATTACK_PREVENTION */
#if defined(CONFIG_SDK_CHIP_FEATURE_OAM)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t OAM_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t OAM_PORT_ACT_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t OAM_GLB_DYING_GASP_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t OAM_PORT_DYING_GASP_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DYING_GASP_POLARITY_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG0_OAM_RTL9300_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_OAM */
#if defined(CONFIG_SDK_CHIP_FEATURE_RLDP___RLPP)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t RLDP_RLPP_CTRL_RTL9300_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_RLDP___RLPP */
#if defined(CONFIG_SDK_CHIP_FEATURE_DEVICE_CONTROL)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STK_GLB_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STK_DEV_PORT_MAP_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STK_NON_UNICAST_BLOCK_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STK_CASCADE_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STK_ONE_HOP_REDIR_PM_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t STK_DBG_CTRL_RTL9300_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_DEVICE_CONTROL */
#if defined(CONFIG_SDK_CHIP_FEATURE_PARSER)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t PARSER_FIELD_SELTOR_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t PARSER_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t PARSER_DROP_REASON_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG0_PKT_PARSER_RTL9300_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_PARSER */
#if defined(CONFIG_SDK_CHIP_FEATURE_PARSER_HSB)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSB_DATA0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSB_DATA1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSB_DATA2_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSB_DATA3_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSB_DATA4_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSB_DATA5_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSB_DATA6_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSB_DATA7_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSB_DATA8_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSB_DATA9_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSB_DATA10_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSB_DATA11_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSB_DATA12_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSB_DATA13_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSB_DATA14_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSB_DATA15_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSB_DATA16_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSB_DATA17_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSB_DATA18_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSB_DATA19_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSB_DATA20_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSB_DATA21_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSB_DATA22_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSB_DATA23_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSB_DATA24_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSB_DATA25_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSB_DATA26_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSB_DATA27_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSB_DATA28_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSB_DATA29_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG0_HSB_RTL9300_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_PARSER_HSB */
#if defined(CONFIG_SDK_CHIP_FEATURE_HSM)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM0_DATA0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM0_DATA1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM0_DATA2_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM0_DATA3_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM0_DATA4_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM0_DATA5_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM0_DATA6_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM0_DATA7_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM0_DATA8_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM0_DATA9_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM0_DATA10_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM0_DATA11_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM0_DATA12_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM0_DATA13_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM0_DATA14_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM0_DATA15_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM0_DATA16_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM0_DATA17_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM1_DATA0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM1_DATA1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM1_DATA2_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM1_DATA3_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM1_DATA4_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM1_DATA5_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM1_DATA6_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM1_DATA7_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM1_DATA8_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM1_DATA9_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM1_DATA10_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM1_DATA11_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM1_DATA12_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM1_DATA13_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM1_DATA14_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM1_DATA15_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM1_DATA16_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM2_DATA0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM2_DATA1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM2_DATA2_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM2_DATA3_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM2_DATA4_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM2_DATA5_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM2_DATA6_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM2_DATA7_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM2_DATA8_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM2_DATA9_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM2_DATA10_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM2_DATA11_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM2_DATA12_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM2_DATA13_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM2_DATA14_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM2_DATA15_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM2_DATA16_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM2_DATA17_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM2_DATA18_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM2_DATA19_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM2_DATA20_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM2_DATA21_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM2_DATA22_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM2_DATA23_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM2_DATA24_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM2_DATA25_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM2_DATA26_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM2_DATA27_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM3_DATA0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM3_DATA1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM3_DATA2_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM3_DATA3_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM3_DATA4_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM3_DATA5_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM3_DATA6_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM3_DATA7_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM3_DATA8_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM3_DATA9_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM3_DATA10_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM3_DATA11_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM3_DATA12_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM3_DATA13_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM3_DATA14_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM3_DATA15_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM3_DATA16_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM3_DATA17_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM3_DATA18_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM3_DATA19_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM3_DATA20_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM3_DATA21_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM3_DATA22_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM3_DATA23_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM3_DATA24_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM3_DATA25_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM3_DATA26_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSM3_DATA27_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG0_HSM_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG1_HSM_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG2_HSM_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG3_HSM_RTL9300_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_HSM */
#if defined(CONFIG_SDK_CHIP_FEATURE_MODIFIER_HSA)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSA_DATA0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSA_DATA1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSA_DATA2_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSA_DATA3_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSA_DATA4_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSA_DATA5_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSA_DATA6_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSA_DATA7_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSA_DATA8_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSA_DATA9_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSA_DATA10_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSA_DATA11_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSA_DATA12_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSA_DATA13_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSA_DATA14_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSA_DATA15_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSA_DATA16_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSA_DATA17_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSA_DATA18_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSA_DATA19_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSA_DATA20_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t HSA_DATA21_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG0_HSA_RTL9300_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_MODIFIER_HSA */
#if defined(CONFIG_SDK_CHIP_FEATURE_DEBUGGING__ALE__LOOPBACK__DROP_MECHANISM__FC_AND_QM_)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t TEST_MODE_ALE_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t TEST_MODE_ALE_HSB_MULTI_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t TEST_MODE_ALE_HSA_MULTI_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG4_RT_REG_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t TEST_MODE_ALE_LOOPBACK_CTRL_RTL9300_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_DEBUGGING__ALE__LOOPBACK__DROP_MECHANISM__FC_AND_QM_ */
#if defined(CONFIG_SDK_CHIP_FEATURE_SMART_PACKET_GENERATOR)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SPG_GLB_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t PKB_ACC_DEBUG_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SPG_PORT_TX_GRP_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SPG_GLOBAL_STS_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SPG_PORT_IPG_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SPG_PORT_PKT_CNT_H_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SPG_PORT_PKT_CNT_L_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SPG_PORT_PKT_CNT_DBG_H_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SPG_PORT_PKT_CNT_DBG_L_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SPG_PORT_STREAM0_CTRL0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SPG_PORT_STREAM0_CTRL1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SPG_PORT_STREAM0_CTRL2_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SPG_PORT_STREAM0_CTRL3_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SPG_PORT_STREAM0_CTRL4_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SPG_PORT_STREAM1_CTRL0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SPG_PORT_STREAM1_CTRL1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SPG_PORT_STREAM1_CTRL2_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SPG_PORT_STREAM1_CTRL3_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SPG_PORT_STREAM1_CTRL4_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SPG_PB_ACCESS_CTRL0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SPG_PB_ACCESS_CTRL1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SPG_PB_ACCESS_CTRL2_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SPG_PORT_INDEX_CTRL0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SPG_GLOBAL_INDEX_CTRL0_RTL9300_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_SMART_PACKET_GENERATOR */
#if defined(CONFIG_SDK_CHIP_FEATURE_AUTO_RECOVERY)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t RXPORT_DSC_STS_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t RXPORT_DSC_ERR_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SW_Q_RST_SYS_THR_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SW_Q_RST_P_THR_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG1TO28_INGRESS_CTRL_2_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t LD_TX_DSC_STS_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t LD_TX_DSC_ERR_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t TX_DSC_CHK_TMR_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t RXFIFO_OVERFLOW_STS_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t RXFIFO_OVERFLOW_ERR_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t RXFIFO_RDEMPTY_STS_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t RXFIFO_RDEMPTY_ERR_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t TXFIFO_OVERFLOW_STS_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t TXFIFO_OVERFLOW_ERR_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t TXFIFO_RDEMPTY_STS_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t TXFIFO_RDEMPTY_ERR_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t PINGPONG_PLUS_STS_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t PINGPONG_PLUS_ERR_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t TOKEN_STS_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t TOKEN_ERR_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t SW_Q_RST_CNT_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t TRIG_AUTO_RECOVER_CTRL_INGRESS_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t TRIG_AUTO_RECOVER_CTRL_EGRESS_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t TRIG_AUTO_RECOVER_CTRL_MAC_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t AUTO_RECOVER_EVENT_FLAG_STS_INGRESS_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t AUTO_RECOVER_EVENT_FLAG_STS_EGRESS_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t AUTO_RECOVER_EVENT_FLAG_STS_MAC_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t AUTO_RECOVER_EVENT_FLAG_ERR_INGRESS_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t AUTO_RECOVER_EVENT_FLAG_ERR_EGRESS_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t AUTO_RECOVER_EVENT_FLAG_ERR_MAC_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t TXERR_CNT_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG3_EGRESS_CTRL_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG0_INGRESS_CTRL_2_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t DMY_REG4_EGRESS_CTRL_RTL9300_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_AUTO_RECOVERY */
#if defined(CONFIG_SDK_CHIP_FEATURE_MISC)
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t CODE_PROTECT_STATE_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ENCAP_SRAM_CTRL0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t ENCAP_SRAM_CTRL1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t PAR_SRAM_CTRL0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t PAR_SRAM_CTRL1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t PAR_SRAM_CTRL2_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t PORT29_DBG_REG0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t PORT29_DBG_REG1_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t PORT30_DBG_REG0_RTL9300_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rtk_regField_t PORT30_DBG_REG1_RTL9300_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_MISC */



#endif    /* __RTK_LONGAN_REGFIELD_LIST_H__ */
