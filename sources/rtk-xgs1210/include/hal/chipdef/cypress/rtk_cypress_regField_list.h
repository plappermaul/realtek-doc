#ifndef __RTK_CYPRESS_REGFIELD_LIST_H__
#define __RTK_CYPRESS_REGFIELD_LIST_H__

/*
 * Include Files
 */
#include <common/rt_autoconf.h>
#ifdef __BOOTLOADER__
#include <hal/chipdef/cypress/rtk_cypress_uboot_feature_def.h>
#else   /* __BOOTLOADER__ */
#include <hal/chipdef/cypress/rtk_cypress_feature_def.h>
#endif  /* __BOOTLOADER__ */

#if defined(CONFIG_SDK_CHIP_FEATURE_INTERFACE)
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t ECO_DUMMY_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t MAC_IF_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t MAC_SERDES_IF_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t MAC_IO_DRIV_ABLTY_RTL8390_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_INTERFACE */
#if defined(CONFIG_SDK_CHIP_FEATURE_RESET)
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t RST_GLB_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t RST_GLB_STS0_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t RST_GLB_STS1_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t MAC_RST_DUR_CTRL_RTL8390_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_RESET */
#if defined(CONFIG_SDK_CHIP_FEATURE_PLL___BIAS)
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t PLL_GLB_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t CPU_PLL_CTRL0_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t CPU_PLL_CTRL1_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t CPU_PLL_SSC_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t CPU_PLL_MISC_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t LXB_PLL_CTRL0_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t LXB_PLL_CTRL1_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t LXB_PLL_SSC_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t LXB_PLL_MISC_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t MEM_PLL_CTRL0_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t MEM_PLL_CTRL1_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t MEM_PLL_SSC_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t MEM_PLL_MISC_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t SW_PLL_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t SW_PLL_MISC_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t SW_PLL_SSC_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t BG_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t CKREFBUF_CTRL_RTL8390_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_PLL___BIAS */
#if defined(CONFIG_SDK_CHIP_FEATURE_INTERRUPT)
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t IMR_GLB_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t IMR_PORT_LINK_STS_CHG_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t IMR_PORT_MEDIA_CHG_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t IMR_PORT_FEFI_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t IMR_PORT_EEE_CHG_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t IMR_PORT_TIMESTAMP_LATCH_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t IMR_SERDES_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t IMR_EXT_GPIO_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t IMR_MISC_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t ISR_GLB_SRC_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t ISR_PORT_LINK_STS_CHG_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t ISR_PORT_MEDIA_CHG_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t ISR_PORT_FEFI_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t ISR_PORT_EEE_CHG_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t ISR_PORT_TIMESTAMP_LATCH_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t ISR_SERDES_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t ISR_EXT_GPIO_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t ISR_MISC_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t GPIO_INT_MODE_RTL8390_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_INTERRUPT */
#if defined(CONFIG_SDK_CHIP_FEATURE_BIST___BISR)
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t BIST_OQ_INFO_STATUS_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t BIST_OQ_LL_STATUS_0_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t BIST_OQ_LL_STATUS_1_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t BISR_OQ_HSA_0_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t BISR_INFO_OQ_HSA0_L_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t BISR_OQ_HSA_1_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t BISR_INFO_OQ_HSA1_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t BIST_EGR_DVS_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t BIST_PG_MEM_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t BIST_PH_MEM_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t BIST_PT_MEM_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t BIST_ENCAP_MEM_1_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t BIST_ENCAP_MEM_0_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t BIST_ENCAP_TCAM_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t BISR_UNTAG_MEM_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t BISR_INFO_UNTAG_MEM_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t BISR_ENCAP_DVS_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t BIST_CTL_MAC1_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t BIST_CTL_MAC2_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t BIST_CTL_MAC3_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t BIST_CTL_MAC4_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t BIST_CTL_MAC5_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t BIST_CTL_ALE1_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t BIST_CTL_ALE2_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t BIST_CTL_ALE3_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t BIST_CTL_ALE4_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t BIST_CTL_ALE5_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t BIST_CTL_ALE6_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t BIST_CTL_ALE7_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t BIST_CTL_ALE8_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t BIST_CTL_ALE9_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t BIST_CTL_ALE10_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t BIST_CTL_ALE11_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t BIST_TCAM_128_144_9_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t BIST_TCAM_128_72_9_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t BIST_TCAM_128_144_4_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t BIST_BCAM_64_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t BISR_CTL_ALE12_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t BISR_CTL_ALE12_INFO_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t ALE_DVS_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t BIST_NIC_L2MSG_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t NIC_DVS_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t MBIST_SW05_CTL_STD_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t PKT_PARSE_DVS_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t MBIST_SW01_CTL_STD_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t MBIST_SW02_CTL_STD_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t MBIST_SW03_CTL_STD_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t MBIST_SW04_CTL_STD_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t MBISR_SW07_CTL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t MBIST_SW07_0_STD_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t MBIST_SW07_1_STD_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t MBIST_SW07_2_STD_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t INGR_PKT_BUF_BYPASS_PG_ADDR_0_1_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t INGR_PKT_BUF_BYPASS_PG_ADDR_2_3_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t INGR_DVS_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t MBIST_GLB_START_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t MBIST_GLB_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t MBIST_GLB_STS_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t MBISR_GLB_STS_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t MBIST_TCAM_DRF_STS_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t BIST_SOC_DVS_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t SOC_CPU_BIST_0_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t SOC_CPU_BIST_1_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t SOC_CPU_BIST_2_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t SOC_CPU_SRAM_CTRL_BIST_RTL8390_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_BIST___BISR */
#if defined(CONFIG_SDK_CHIP_FEATURE_LED)
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t LED_GLB_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t LED_SET_2_3_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t LED_SET_0_1_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t LED_COPR_SET_SEL_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t LED_FIB_SET_SEL_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t LED_COPR_PMASK_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t LED_FIB_PMASK_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t LED_COMBO_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t LED_SW_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t LED_SW_P_EN_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t LED_SW_P_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t EXT_GPIO_DIR_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t EXT_GPIO_DATA_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t EXT_GPIO_INDRT_ACCESS_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t LED_LOAD_LV1_10G_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t LED_LOAD_LV2_10G_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t LED_LOAD_LV3_10G_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t LED_LOAD_LV1_1G_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t LED_LOAD_LV2_1G_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t LED_LOAD_LV3_1G_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t LED_LOAD_LV1_100M_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t LED_LOAD_LV2_100M_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t LED_LOAD_LV3_100M_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t LED_LOAD_LV1_10M_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t LED_LOAD_LV2_10M_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t LED_LOAD_LV3_10M_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t LED_P_LOAD_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t BUZZER_CTRL_RTL8390_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_LED */
#if defined(CONFIG_SDK_CHIP_FEATURE_HW_MISC_)
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t EFUSE_CMD_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t EFUSE_WDATA_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t EFUSE_RDATA_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t EFUSE_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t VOL_CTRL_RESIS_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t TM0_CTRL_0_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t TM0_CTRL_1_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t TM0_CTRL_2_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t TM0_RESULT_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t TM1_CTRL_0_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t TM1_CTRL_1_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t TM1_CTRL_2_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t TM1_RESULT_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t SPD_SENR_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t SPD_DATA_IN_0_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t SPD_DATA_IN_1_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t SPD_DATA_OUT_0_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t SPD_DATA_OUT_1_RTL8390_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_HW_MISC_ */
#if defined(CONFIG_SDK_CHIP_FEATURE_CHP_INFORMATION)
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t MODEL_NAME_INFO_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t CHIP_INFO_RTL8390_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_CHP_INFORMATION */
#if defined(CONFIG_SDK_CHIP_FEATURE_MAC_CONTROL)
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t MAC_GLB_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t MAC_PADDING_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t MAC_EFUSE_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t MAC_MAX_LEN_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t MAC_ADDR_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t MAC_PORT_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t MAC_FORCE_MODE_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t MAC_LINK_STS_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t MAC_LINK_MEDIA_STS_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t MAC_LINK_SPD_STS_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t MAC_LINK_DUP_STS_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t MAC_TX_PAUSE_STS_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t MAC_RX_PAUSE_STS_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t MAC_EEE_ABLTY_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t MAC_FEFI_STS_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t MAC_CPU_TAG_ID_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t PHYREG_ACCESS_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t PHYREG_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t PHYREG_PORT_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t BROADCAST_PHYID_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t PHYREG_DATA_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t PHYREG_MMD_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t SMI_GLB_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t SMI_PORT_POLLING_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t CHG_DUP_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t MAC_LINK_500M_STS_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t SMI_PORT_500M_POLLING_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t MAC_MISC_CTRL_RTL8390_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_MAC_CONTROL */
#if defined(CONFIG_SDK_CHIP_FEATURE_PHY___SERDES)
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t SDS0_1_XSG0_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t SDS0_1_XSG1_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t SDS0_1_ANA_RG_EXT_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t SDS2_3_XSG0_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t SDS2_3_XSG1_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t SDS2_3_ANA_RG_EXT_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t SDS4_5_XSG0_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t SDS4_5_XSG1_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t SDS4_5_ANA_RG_EXT_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t SDS6_7_XSG0_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t SDS6_7_XSG1_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t SDS6_7_ANA_RG_EXT_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t SDS8_9_XSG0_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t SDS8_9_XSG1_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t SDS8_9_TGRX_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t SDS8_9_ANA_TG_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t SDS10_11_XSG0_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t SDS10_11_XSG1_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t SDS10_11_ANA_RG_EXT_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t SDS12_13_XSG0_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t SDS12_13_XSG1_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t SDS12_13_TGRX_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t SDS12_13_ANA_TG_RTL8390_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_PHY___SERDES */
#if defined(CONFIG_SDK_CHIP_FEATURE_POWER_SAVING)
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t EEE_TX_Q_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t EEE_TX_MINIFG_CTRL0_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t EEE_TX_MINIFG_CTRL1_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t EEE_TX_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t EEE_TX_TIMER_100M_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t EEE_TX_TIMER_GELITE_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t EEE_TX_TIMER_GIGA_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t EEE_TX_TIMER_10G_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t EEE_EEEP_PORT_TX_STS_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t EEE_EEEP_PORT_RX_STS_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t EEEP_PORT_TX_EN_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t EEEP_PORT_RX_EN_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t EEEP_TIMER_UNIT_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t EEEP_TX_100M_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t EEEP_TX_500M_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t EEEP_TX_GIGA_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t EEEP_TX_WAKE_TIMER_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t EEEP_RX_RATE_100M_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t EEEP_RX_RATE_500M_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t EEEP_RX_RATE_GIGA_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t EEEP_RX_SLEEP_STEP_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t EEEP_RX_TIMER_100M_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t EEEP_RX_TIMER_500M_CTRL0_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t EEEP_RX_TIMER_500M_CTRL1_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t EEEP_RX_TIMER_GIGA_CTRL0_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t EEEP_RX_TIMER_GIGA_CTRL1_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t PS_GATCLK_SLCLK_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t PS_LINKID_GATCLK_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t PS_EEE_GATCLK_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t PS_ACL_PWR_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t EEEP_RX_IDLE_TIMER_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t EEEP_TX_IDLE_TIMER_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t EEEP_GBL_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t EEE_CTRL_RTL8390_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_POWER_SAVING */
#if defined(CONFIG_SDK_CHIP_FEATURE_SYSTEM_CLOCK)
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t REF_TIME_SEC_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t REF_TIME_NSEC_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t REF_TIME_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t REF_TIME_EN_RTL8390_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_SYSTEM_CLOCK */
#if defined(CONFIG_SDK_CHIP_FEATURE_PTP__PRECISION_TIME_PROTOCOL_)
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t PTP_PORT_EN_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t PTP_PORT_RX_TIME_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t PTP_PORT_TX_TIME_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t PTP_INTR_STS_RTL8390_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_PTP__PRECISION_TIME_PROTOCOL_ */
#if defined(CONFIG_SDK_CHIP_FEATURE_ADDRESS_TABLE_LOOKUP)
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t L2_CTRL_0_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t L2_CTRL_1_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t L2_PORT_LM_ACT_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t L2_PORT_STTC_MV_ACT_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t L2_FLD_PMSK_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t L2_PORT_NEW_SALRN_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t L2_PORT_NEW_SA_FWD_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t L2_LRN_CONSTRT_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t L2_PORT_LRN_CONSTRT_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t L2_VLAN_LRN_CONSTRT_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t L2_VLAN_LRN_CONSTRT_CNT_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t L2_VLAN_LRN_CONSTRT_ACT_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t L2_NOTIFICATION_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t L2_IGR_P_FLTR_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t L2_PORT_AGING_OUT_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t L2_IPV6_MC_IP_CARE_BYTE_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t L2_PORT_MV_ACT_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t L2_PORT_MV_INVALIDATE_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t L2_PORT_MV_FORBID_RTL8390_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_ADDRESS_TABLE_LOOKUP */
#if defined(CONFIG_SDK_CHIP_FEATURE_ADDRESS_LEARNING___FLUSH)
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t L2_TBL_FLUSH_CTRL_RTL8390_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_ADDRESS_LEARNING___FLUSH */
#if defined(CONFIG_SDK_CHIP_FEATURE__IEEE802_1Q__VLAN)
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t VLAN_TAG_TPID_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t VLAN_ETAG_TPID_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t VLAN_PORT_ITAG_TPID_CMP_MSK_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t VLAN_PORT_OTAG_TPID_CMP_MSK_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t VLAN_PORT_ETAG_TPID_CMP_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t VLAN_PORT_ACCEPT_FRAME_TYPE_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t VLAN_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t VLAN_PORT_PB_VLAN_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t VLAN_PORT_FWD_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t VLAN_PORT_IGR_FLTR_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t VLAN_PORT_EGR_FLTR_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t VLAN_PROFILE_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t VLAN_IGR_CNVT_BLK_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t VLAN_EGR_CNVT_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t VLAN_PORT_TAG_STS_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t VLAN_PORT_EGR_ITPID_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t VLAN_PORT_EGR_OTPID_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t RNG_CHK_VID_EGR_XLATE_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t VLAN_PORT_NTO1_AGGR_RTL8390_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE__IEEE802_1Q__VLAN */
#if defined(CONFIG_SDK_CHIP_FEATURE__IEEE802_1V__PROTOCOL_BASED_VLAN)
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t VLAN_PPB_VLAN_VAL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t VLAN_PORT_PPB_VLAN_RTL8390_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE__IEEE802_1V__PROTOCOL_BASED_VLAN */
#if defined(CONFIG_SDK_CHIP_FEATURE_LINK_AGGREGATION)
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t TRK_MBR_CTR_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t TRK_HASH_IDX_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t TRK_HASH_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t TRK_SEP_TRAFFIC_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t TRK_EXTRA_EN_CTRL_RTL8390_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_LINK_AGGREGATION */
#if defined(CONFIG_SDK_CHIP_FEATURE_SPANNING_TREE)
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t ST_CTRL_RTL8390_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_SPANNING_TREE */
#if defined(CONFIG_SDK_CHIP_FEATURE_PORT_ISOLATION_FORWARDING_FORCE_MODE)
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t PORT_ISO_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t PORT_ISO_VB_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t PORT_ISO_VB_ISO_PM_CTRL_RTL8390_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_PORT_ISOLATION_FORWARDING_FORCE_MODE */
#if defined(CONFIG_SDK_CHIP_FEATURE_RMA)
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t RMA_CTRL_0_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t RMA_CTRL_1_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t RMA_CTRL_2_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t RMA_CTRL_3_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t RMA_SMAC_LRN_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t RMA_MGN_LRN_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t RMA_USR_DEF_CTRL_SET0_0_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t RMA_USR_DEF_CTRL_SET0_1_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t RMA_USR_DEF_CTRL_SET1_0_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t RMA_USR_DEF_CTRL_SET1_1_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t RMA_PORT_BPDU_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t RMA_PORT_PTP_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t RMA_PORT_LLDP_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t RMA_BPDU_FLD_PMSK_RTL8390_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_RMA */
#if defined(CONFIG_SDK_CHIP_FEATURE_NIC___DMA)
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t DMA_IF_RX_BASE_DESC_ADDR_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t DMA_IF_RX_CUR_DESC_ADDR_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t DMA_IF_TX_BASE_DESC_ADDR_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t DMA_IF_TX_CUR_DESC_ADDR_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t DMA_IF_NBUF_BASE_DESC_ADDR_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t DMA_IF_NBUF_CUR_DESC_ADDR_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t DMA_IF_INTR_MSK_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t DMA_IF_INTR_STS_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t DMA_IF_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t DMA_IF_PKT_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t DMA_IF_RX_RING_SIZE_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t DMA_IF_RX_RING_CNTR_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t DMA_IF_PKT_FLTR_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t DMA_IF_PHYSICAL_ADDR_MSK_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t DMA_L2MSG_TMROUT_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t DMA_L2MSG_CNT_SEL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t DMA_RDMA_CNT_SEL_RTL8390_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_NIC___DMA */
#if defined(CONFIG_SDK_CHIP_FEATURE_STORM_CONTROL__B_M_UM_DLF_)
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STORM_CTRL_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STORM_CTRL_LB_TICK_TKN_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STORM_CTRL_PORT_BC_EXCEED_FLG_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STORM_CTRL_PORT_MC_EXCEED_FLG_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STORM_CTRL_PORT_UC_EXCEED_FLG_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STORM_CTRL_PORT_MC_TYPE_10G_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STORM_CTRL_PORT_UC_TYPE_10G_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STORM_CTRL_PORT_BC_RATE_10G_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STORM_CTRL_PORT_MC_RATE_10G_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STORM_CTRL_PORT_UC_RATE_10G_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STORM_CTRL_PORT_BC_BURST_10G_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STORM_CTRL_PORT_MC_BURST_10G_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STORM_CTRL_PORT_UC_BURST_10G_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STORM_CTRL_PORT_UC_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STORM_CTRL_PORT_MC_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STORM_CTRL_PORT_BC_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STORM_CTRL_SPCL_LB_TICK_TKN_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STORM_CTRL_SPCL_PORT_BPDU_EXCEED_FLG_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STORM_CTRL_SPCL_PORT_IGMP_EXCEED_FLG_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STORM_CTRL_SPCL_PORT_ARP_EXCEED_FLG_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STORM_CTRL_SPCL_PORT_RATE_RTL8390_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_STORM_CONTROL__B_M_UM_DLF_ */
#if defined(CONFIG_SDK_CHIP_FEATURE_BANDWIDTH_CONTROL__INGRESS_EGRESS_)
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t IGR_BWCTRL_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t IGR_BWCTRL_LB_TICK_TKN_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t IGR_BWCTRL_PORT_EXCEED_FLG_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t IGR_BWCTRL_CTRL_LB_THR_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t IGR_BWCTRL_PORT_CTRL_10G_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t IGR_BWCTRL_PORT_CTRL_RTL8390_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_BANDWIDTH_CONTROL__INGRESS_EGRESS_ */
#if defined(CONFIG_SDK_CHIP_FEATURE_METER_MARKER)
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t METER_GLB_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t METER_MODE_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t METER_LB_TICK_TKN_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t METER_BYTE_DLB_LB_THR_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t METER_BYTE_SRTCM_LB_THR_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t METER_BYTE_TRTCM_LB_THR_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t METER_PKT_DLB_LB_THR_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t METER_PKT_SRTCM_LB_THR_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t METER_PKT_TRTCM_LB_THR_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t METER_LB_EXCEED_STS_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t METER_CNTR_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t METER_GREEN_CNTR_STS_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t METER_YELLOW_CNTR_STS_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t METER_RED_CNTR_STS_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t METER_TOTAL_CNTR_STS_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t METER_RATE_MODE_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t METER_LB_GLB_EXCEED_STS_RTL8390_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_METER_MARKER */
#if defined(CONFIG_SDK_CHIP_FEATURE_ATTACK_PREVENTION)
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t ATK_PRVNT_PORT_EN_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t ATK_PRVNT_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t ATK_PRVNT_ACT_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t ATK_PRVNT_IPV6_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t ATK_PRVNT_ICMP_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t ATK_PRVNT_TCP_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t ATK_PRVNT_SMURF_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t ATK_PRVNT_STS_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t ATK_PRVNT_ARP_INVLD_PORT_ACT_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t ATK_PRVNT_PORT_GARP_ACT_RTL8390_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_ATTACK_PREVENTION */
#if defined(CONFIG_SDK_CHIP_FEATURE_MIRRORING)
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t MIR_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t MIR_SPM_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t MIR_DPM_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t MIR_RSPAN_VLAN_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t MIR_RSPAN_TX_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t MIR_RSPAN_RX_TAG_RM_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t MIR_RSPAN_RX_TAG_EN_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t MIR_SAMPLE_RATE_CTRL_RTL8390_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_MIRRORING */
#if defined(CONFIG_SDK_CHIP_FEATURE_SFLOW)
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t SFLOW_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t SFLOW_PORT_RATE_CTRL_RTL8390_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_SFLOW */
#if defined(CONFIG_SDK_CHIP_FEATURE_STATISTIC_COUNTERS)
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER0_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER1_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER2_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER3_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER4_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER5_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER6_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER7_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER8_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER9_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER10_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER11_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER12_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER13_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER14_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER15_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER16_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER17_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER18_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER19_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER20_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER21_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER22_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER23_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER24_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER25_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER26_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER27_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER28_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER29_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER30_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER31_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER32_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER33_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER34_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER35_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER36_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER37_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER38_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER39_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER40_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER41_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER42_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER43_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STAT_PRVTE_DROP_COUNTER44_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STAT_PORT_STANDARD_MIB_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STAT_BRIDGE_DOT1DTPLEARNEDENTRYDISCARDS_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STAT_RST_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STAT_PORT_RST_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t STAT_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t MIB_BIST_CTL_RTL8390_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_STATISTIC_COUNTERS */
#if defined(CONFIG_SDK_CHIP_FEATURE_FLOWCONTROL___BACKPRESSURE_THRESHOLD)
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t FC_DROP_THR_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t FC_GLB_HI_THR_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t FC_GLB_LO_THR_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t FC_GLB_FCOFF_HI_THR_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t FC_GLB_FCOFF_LO_THR_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t FC_P_THR_SET_SEL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t FC_P_HI_THR_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t FC_P_LO_THR_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t FC_P_FCOFF_HI_THR_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t FC_P_Q_EGR_DROP_EN_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t FC_P_RXCNGST_IGNORE_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t FC_Q_EGR_DROP_THR_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t FC_CPUQ_EGR_DROP_THR_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t FC_P_USED_PAGE_CNT_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t FC_TL_USED_PAGE_CNT_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t FC_ACT_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t FC_P_FLD_HOL_PRVNT_EN_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t FC_FLD_HOL_PRVNT_CTRL_RTL8390_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_FLOWCONTROL___BACKPRESSURE_THRESHOLD */
#if defined(CONFIG_SDK_CHIP_FEATURE_CONGESTION_AVOIDANCE)
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t WRED_GLB_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t WRED_PORT_THR_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t WRED_QUEUE_THR_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t WRED_DROP_CNTR_PIDX_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t WRED_DROP_CNTR_CIDX_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t WRED_DROP_CNTR_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t WRED_CNTR_RST_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t SC_P_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t SC_P_EN_RTL8390_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_CONGESTION_AVOIDANCE */
#if defined(CONFIG_SDK_CHIP_FEATURE_QUEUE_MANAGEMENT)
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t QM_INTPRI2QID_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t QM_PORT_QNUM_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t QM_PKT2CPU_INTPRI_0_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t QM_PKT2CPU_INTPRI_1_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t QM_PKT2CPU_INTPRI_2_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t QM_PKT2CPU_INTPRI_MAP_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t FC_EGR_DROP_CTRL_RTL8390_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_QUEUE_MANAGEMENT */
#if defined(CONFIG_SDK_CHIP_FEATURE_INGRESS_PRIORITY_DECISION)
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t PRI_SEL_IPRI_REMAP_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t PRI_SEL_OPRI_DEI0_REMAP_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t PRI_SEL_OPRI_DEI1_REMAP_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t PRI_SEL_DSCP_REMAP_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t PRI_SEL_PORT_PRI_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t PRI_SEL_PORT_TBL_IDX_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t PRI_SEL_TBL_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t PRI_SEL_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t PRI_SEL_PORT_DEI_TAG_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t PRI_SEL_DEI2DP_REMAP_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t PRI_SEL_DSCP2DP_REMAP_RTL8390_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_INGRESS_PRIORITY_DECISION */
#if defined(CONFIG_SDK_CHIP_FEATURE_REMARKING)
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t RMK_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t RMK_PORT_RMK_EN_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t RMK_PORT_OPRI_SRC_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t RMK_IPRI_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t RMK_OPRI_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t RMK_PORT_DEI_TAG_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t RMK_DEI_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t RMK_DSCP_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t RMK_PORT_OPRI_SRC_EXT_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t PRI_SEL_PORT_PRI_COPY_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t RMK_DSCP2IPRI_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t RMK_DSCP2OPRI_CTRL_RTL8390_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_REMARKING */
#if defined(CONFIG_SDK_CHIP_FEATURE_SCHEDULING)
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t SCHED_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t SCHED_LB_TICK_TKN_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t SCHED_LB_THR_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t SCHED_LB_TICK_TKN_PPS_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t SCHED_PPS_CTRL_RTL8390_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_SCHEDULING */
#if defined(CONFIG_SDK_CHIP_FEATURE_ETHERNET_AV)
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t AVB_PORT_CLASS_A_EN_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t AVB_PORT_CLASS_B_EN_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t AVB_PORT_CLASS_A_EN_MAC_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t AVB_PORT_CLASS_B_EN_MAC_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t AVB_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t AVB_CTRL_MAC_RTL8390_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_ETHERNET_AV */
#if defined(CONFIG_SDK_CHIP_FEATURE_INGRESS_AND_EGRESS_ACL)
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t ACL_BLK_LOOKUP_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t ACL_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t ACL_BLK_TMPLTE_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t ACL_RESERVED_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t ACL_BLK_GROUP_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t ACL_BLK_RESULT_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t ACL_MV_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t ACL_MV_LEN_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t ACL_CLR_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t ACL_TMPLTE_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t ACL_VLAN_CNVT_DFLT_ACT_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t ACL_RULE_HIT_INDICATION_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t ACL_GLB_HIT_INDICATION_RTL8390_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_INGRESS_AND_EGRESS_ACL */
#if defined(CONFIG_SDK_CHIP_FEATURE_RANGE_CHECK__PORT_VLAN_IP_L4PORT_)
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t RNG_CHK_SPM_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t RNG_CHK_DPM_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t RNG_CHK_VID_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t RNG_CHK_IP_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t RNG_CHK_IP_RNG_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t RNG_CHK_L4PORT_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t RNG_CHK_L4PORT_RNG_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t RNG_CHK_PKT_LEN_CTRL_RTL8390_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_RANGE_CHECK__PORT_VLAN_IP_L4PORT_ */
#if defined(CONFIG_SDK_CHIP_FEATURE_OAM)
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t OAM_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t OAM_PORT_ACT_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t OAM_GLB_DYING_GASP_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t OAM_PORT_DYING_GASP_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t DYING_GASP_POLARITY_CTRL_RTL8390_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_OAM */
#if defined(CONFIG_SDK_CHIP_FEATURE_CFM)
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t CFM_UNKN_RX_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t CFM_LBLT_RX_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t CFM_CCM_RX_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t CCM_TAG_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t CCM_LIFETIME_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t CCM_TX_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t CCM_TX_INST_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t CCM_TX_INST_P_CFG_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t CCM_RX_INST_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t CCM_INST_CNTR_1_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t CCM_INST_CNTR_0_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t ETH_DM_PORT_EN_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t ETH_DM_RX_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t ETH_DM_TX_DLY_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t ETH_DM_RX_TIME_RTL8390_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_CFM */
#if defined(CONFIG_SDK_CHIP_FEATURE_PARSER_HSB)
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSB_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSB_DATA0_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSB_DATA1_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSB_DATA2_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSB_DATA3_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSB_DATA4_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSB_DATA5_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSB_DATA6_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSB_DATA7_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSB_DATA8_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSB_DATA9_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSB_DATA10_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSB_DATA11_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSB_DATA12_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSB_DATA13_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSB_DATA14_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSB_DATA15_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSB_DATA16_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSB_DATA17_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSB_DATA18_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSB_DATA19_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSB_DATA20_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSB_DATA21_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSB_DATA22_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSB_DATA23_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSB_DATA24_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSB_DATA25_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSB_DATA26_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSB_DATA27_RTL8390_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_PARSER_HSB */
#if defined(CONFIG_SDK_CHIP_FEATURE_HSM)
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSM_PRE_DATA0_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSM_PRE_DATA1_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSM_PRE_DATA2_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSM_PRE_DATA3_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSM_PRE_DATA4_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSM_PRE_DATA5_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSM_PRE_DATA6_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSM_PRE_DATA7_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSM_PRE_DATA8_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSM_PRE_DATA9_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSM_PRE_DATA10_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSM_PRE_DATA11_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSM_PRE_DATA12_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSM_PRE_DATA13_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSM_PRE_DATA14_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSM_PRE_DATA15_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSM_LU_DATA0_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSM_LU_DATA1_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSM_LU_DATA2_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSM_LU_DATA3_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSM_LU_DATA4_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSM_LU_DATA5_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSM_LU_DATA6_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSM_LU_DATA7_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSM_LU_DATA8_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSM_LU_DATA9_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSM_LU_DATA10_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSM_LU_DATA11_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSM_LU_DATA12_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSM_LU_DATA13_RTL8390_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_HSM */
#if defined(CONFIG_SDK_CHIP_FEATURE_MODIFIER_HSA)
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSA_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSA_DATA0_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSA_DATA1_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSA_DATA2_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSA_DATA3_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSA_DATA4_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSA_DATA5_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSA_DATA6_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSA_DATA7_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSA_DATA8_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSA_DATA9_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSA_DATA10_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSA_DATA11_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSA_DATA12_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSA_DATA13_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSA_DATA14_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSA_DATA15_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSA_DATA16_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t HSA_DATA17_RTL8390_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_MODIFIER_HSA */
#if defined(CONFIG_SDK_CHIP_FEATURE_DEBUGGING__ALE__LOOPBACK__DROP_MECHANISM__FC_AND_QM_)
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t DBG_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t DBG_DATA_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t ALE_DBG_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t ALE_DBG_CMP_DATA_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t ALE_CARE_BIT_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t ALE_DBG_RISING_CHK_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t ALE_DBG_FALLING_CHK_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t ALE_DBG_OUT_1_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t ALE_DBG_OUT_2_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t ALE_DBG_OUT_3_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t ALE_DBG_OUT_4_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t ALE_DBG_OUT_5_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t ALE_DBG_OUT_6_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t ALE_DBG_OUT_7_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t ALE_DBG_OUT_8_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t ALE_DBG_OUT_9_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t ALE_DBG_OUT_10_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t ALE_DBG_OUT_11_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t ALE_DBG_OUT_12_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t ALE_DBG_OUT_13_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t ALE_DBG_OUT_14_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t ALE_DBG_OUT_15_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t ALE_DBG_OUT_16_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t IGR_PORT_DBG_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t IGR_DBG_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t PKTENCAP_PORT_DBG_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t PARSER_DBG_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t MAC_DBG_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t CFG_NICDBG_SEL_0_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t CFG_NICDBG_SEL_1_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t CFG_NICDBG_SEL_2_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t CFG_NICDBG_SEL_3_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t CFG_NICDBG_SEL_4_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t CFG_NICDBG_SEL_5_RTL8390_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_DEBUGGING__ALE__LOOPBACK__DROP_MECHANISM__FC_AND_QM_ */
#if defined(CONFIG_SDK_CHIP_FEATURE_SMART_PACKET_GENERATOR)
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t SPG_GLB_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t SPG_PORT_TX_GRP_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t SPG_PORT_STS_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t SPG_PORT_PKT_TRAP_RTL8390_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_SMART_PACKET_GENERATOR */
#if defined(CONFIG_SDK_CHIP_FEATURE_L3_ROUTING)
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t ROUTING_EXCPT_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t ROUTING_SA_CTRL_RTL8390_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_L3_ROUTING */
#if defined(CONFIG_SDK_CHIP_FEATURE_L3_VPN_MPLS)
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t MPLS_CTRL_RTL8390_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_L3_VPN_MPLS */
#if defined(CONFIG_SDK_CHIP_FEATURE_TABLE_ACCESS)
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t TBL_ACCESS_L2_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t TBL_ACCESS_L2_DATA_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t TBL_ACCESS_CTRL_0_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t TBL_ACCESS_DATA_0_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t TBL_ACCESS_CTRL_1_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t TBL_ACCESS_DATA_1_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t TBL_ACCESS_CTRL_2_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t TBL_ACCESS_DATA_2_RTL8390_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_TABLE_ACCESS */
#if defined(CONFIG_SDK_CHIP_FEATURE_SPECIAL_TRAP)
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t SPCL_TRAP_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t SPCL_TRAP_IGMP_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t SPCL_TRAP_EAPOL_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t SPCL_TRAP_ARP_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t SPCL_TRAP_IPV6_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t SPCL_TRAP_SWITCH_MAC_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t SPCL_TRAP_SWITCH_IPV4_ADDR_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t SPCL_TRAP_CRC_CTRL_RTL8390_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_SPECIAL_TRAP */
#if defined(CONFIG_SDK_CHIP_FEATURE_PARSER)
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t PARSER_FIELD_SELTOR_CTRL_RTL8390_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL8390)
extern rtk_regField_t PARSER_CTRL_RTL8390_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_PARSER */



#endif    /* __RTK_CYPRESS_REGFIELD_LIST_H__ */
