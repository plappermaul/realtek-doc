#ifndef __RTK_RTL9603D_REGFIELD_LIST_H__
#define __RTK_RTL9603D_REGFIELD_LIST_H__

/*
 * Include Files
 */
#include <common/rt_autoconf.h>
#ifdef __BOOTLOADER__
#include <hal/chipdef/rtl9603d/rtk_rtl9603d_uboot_feature_def.h>
#else   /* __BOOTLOADER__ */
#include <hal/chipdef/rtl9603d/rtk_rtl9603d_feature_def.h>
#endif  /* __BOOTLOADER__ */

#if defined(CONFIG_SDK_CHIP_FEATURE_INTERFACE)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t DIGITAL_INTERFACE_SELECT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t I2C_CONFIG_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPHY_IND_WD_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPHY_IND_CMD_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPHY_IND_RD_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FEPHY_POLL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EFUSE_CHK_CFG_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EFUSE_CHK_DATA_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EFUSE_CHK_STATUS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EFUSE_IND_WD_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EFUSE_IND_CMD_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EFUSE_IND_RD_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SC_IND_WD_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SC_IND_CMD_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SC_IND_RD_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t REGCTRL_GLB_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t IOPAD_CFG_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t IO_LED_EN_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t IO_MODE_EN_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t IO_UART_SEL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t IO_GPIO_EN_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CFG_PCSXF_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CFG_PHY_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CFG_PHY_INI_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CFG_POLL_CMD_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CFG_GETPWRCTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CFG_HOTCMD1_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CFG_HOTCMD2_AD_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CFG_HOTCMD2_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CFG_POLL_ADR_0_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CFG_POLL_ADR_1_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CFG_POLL_INV_0_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CFG_POLL_INV_1_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CFG_POLL_WD_0_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CFG_POLL_WD_1_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CFG_POLL_RD_0_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CFG_POLL_RD_1_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CHIP_DEBUG_OUT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EFUSE_BOND_CONTENT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EFUSE_BOND_RSLT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t I2C_IND_WD_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t I2C_IND_ADR_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t I2C_IND_CMD_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t I2C_IND_RD_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CFG_HOTCMD2_TRG_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_INTERFACE */
#if defined(CONFIG_SDK_CHIP_FEATURE_RESET)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SOFTWARE_RST_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_RESET */
#if defined(CONFIG_SDK_CHIP_FEATURE_INTERRUPT)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t INTR_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t INTR_DBGO_POS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t INTR_DBGO_NEG_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t INTR_IMR_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t INTR_IMS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t INTR_STAT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t INTR_STAT_POS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t INTR_STAT_NEG_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_INTERRUPT */
#if defined(CONFIG_SDK_CHIP_FEATURE_BIST___BISR)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_BIST_CFG_3_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_BIST_CFG_6_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_BIST_CFG_8_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_BIST_CFG_9_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_BIST_CFG_10_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_BIST_CFG_11_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_BIST_CFG_14_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_BIST_CFG_15_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_BIST_CFG_17_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_BIST_CFG_18_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_BIST_CFG_19_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_BIST_CFG_20_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_BIST_CFG_21_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_BIST_CFG_22_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_BIST_CFG_23_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_BIST_CFG_24_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_BIST_CFG_25_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_BIST_CFG_26_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_BIST_CFG_27_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_BIST_CFG_28_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_BIST_CFG_29_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_BIST_CFG_30_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_BIST_CFG_31_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_BIST_CFG_32_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_BIST_CFG_OQ_0_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_BIST_CFG_OQ_1_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_BIST_CFG_OQ_2_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_BIST_CFG_OQ_3_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_BIST_CFG_OQ_4_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_BIST_CFG_OQ_5_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_BIST_CFG_OQ_6_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_BIST_CFG_OQ_7_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_BIST_CFG_37_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_BIST_CFG_38_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_BIST_CFG_39_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_BIST_CFG_40_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_BIST_CFG_41_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_BIST_CFG_42_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_BIST_CFG_43_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_BIST_CFG_44_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_BIST_CFG_45_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_BIST_CFG_46_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_BIST_CFG_47_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_BIST_RSLT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_BISD_CFG_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_BISD_DOUT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_BIST_CFG_FERAM_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_BIST_CFG_FEROM_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_BIST_MISR_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t US_BIST_CFG_1_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t US_BIST_CFG_2_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t US_BIST_CFG_3_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t US_BIST_CFG_4_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t US_BIST_CFG_5_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t US_BIST_CFG_6_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t US_BIST_CFG_7_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t US_BIST_CFG_8_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t US_BIST_CFG_9_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t US_BIST_CFG_10_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t US_BIST_CFG_11_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t US_BIST_CFG_12_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t US_BIST_CFG_13_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t US_BIST_CFG_14_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t US_BIST_RSLT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t DS_BIST_CFG_1_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t DS_BIST_CFG_2_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t DS_BIST_CFG_3_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t DS_BIST_CFG_4_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t DS_BIST_CFG_5_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t DS_BIST_CFG_6_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t DS_BIST_CFG_7_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t DS_BIST_CFG_8_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t DS_BIST_RSLT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PBOES_BIST_0_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PBOES_BIST_1_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PBOES_BIST_2_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PBOES_BIST_3_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PBOES_BIST_4_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PBOES_BIST_5_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PBOES_BIST_6_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PBOES_BIST_7_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PBOES_BIST_8_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PBOES_BIST_9_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PBOES_BIST_10_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PBOES_BIST_11_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PBOES_BIST_12_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PBOES_BIST_13_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_BIST___BISR */
#if defined(CONFIG_SDK_CHIP_FEATURE_LED)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t LED_LED_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t DATA_LED_CFG_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t LED_ACTIVE_LOW_CFG_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SERI_LED_ACTIVE_LOW_CFG_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t LED_FORCE_VALUE_CFG_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t LED_BLINK_RATE_CFG_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t LED_EN_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SERI_LED_CLK_PER_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SERI_LED_REFRESH_TIME_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_LED_CFG_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t LED_FLT_MPCP_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_LED */
#if defined(CONFIG_SDK_CHIP_FEATURE_HW_MISC_)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FORCE_P_DMP_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EN_FORCE_P_DMP_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GLB_MAC_MISC_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t WRAP_GPHY_MISC_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SSC_CTRL_0_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SSC_CTRL_1_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SSC_CTRL_2_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SSC_CTRL_3_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SSC_CTRL_4_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SSC_CTRL_5_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SSC_STS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t THERMAL_CTRL_0_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t THERMAL_CTRL_1_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t THERMAL_CTRL_2_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t THERMAL_CTRL_3_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t THERMAL_CTRL_4_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t THERMAL_CTRL_5_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t THERMAL_CTRL_6_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t THERMAL_CTRL_7_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t THERMAL_STS_0_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t THERMAL_STS_1_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t THERMAL_STS_2_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t THERMAL_STS_3_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t THERMAL_STS_4_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t DDRSWR_CTRL_0_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t DDRSWR_CTRL_1_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t DDRSWR_STS_0_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t DDRSWR_STS_1_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CRASH_INTR_EN_0_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CRASH_INTR_EN_1_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CRASH_INTR_STS_0_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CRASH_INTR_STS_1_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CRASH_CONFIG_0_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CRASH_CONFIG_1_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CRASH_CONFIG_2_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CRASH_CONFIG_3_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CRASH_CONFIG_4_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t DDS_CHIP_CTRL_0_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t DDS_CHIP_STS_0_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SCLDO_CTRL_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_HW_MISC_ */
#if defined(CONFIG_SDK_CHIP_FEATURE_CHP_INFORMATION)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t MODEL_NAME_INFO_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CHIP_INFO_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t BOND_INFO_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t MISCELLANEOUS_CONFIGURE0_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FORCE_P_ABLTY_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t MDX_PHY_REG1_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t ROUTER_UPS_CFG_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t P_ABLTY_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RTL_OUI_CFG_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t REVISON_CFG_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t MODEL_CFG_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t ABLTY_FORCE_MODE_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t DEBUG_SEL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t DEBUG_SEL_TOP_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RST_SYNC_FIFO_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_CFG_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t MAC_ACT_CFG_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t BYPS_ABLTY_LOCK_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FIFO_ERR_STS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_AN_RX_CFG_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_FIB_STATUS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t HWPKT_GEN_STA_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPHY_AFE_DBG_CFG_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t DYNGASP_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t BOND_STS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t STRAP_STS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t MISCELLANEOUS_BONDING_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t MISCELLANEOUS_STRAPPING0_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t MAC_DLYLNK_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PONCTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t HSG_SDS_CTRL_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_CHP_INFORMATION */
#if defined(CONFIG_SDK_CHIP_FEATURE_MAC_CONTROL)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CPU_TAG_INSERT_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CPU_TAG_AWARE_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t ACCEPT_MAX_LEN_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CFG_MAC_MISC_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CFG_BACKPRESSURE_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CFG_UNHIOL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SWITCH_MAC_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SWITCH_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CFG_L2TP_SPORT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t INBW_HBOUND_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t INBW_LBOUND_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t P_TX_ERR_CNT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t P_MISC_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t P_CUR_RATE_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t UTP_FIBER_AUTODET_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CHANGE_DUPLEX_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EXTG_ACTYPE0_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EXTG_ACTYPE1_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EXTG_ACTYPE2_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EXTG_ACTYPE3_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EXTG_ACTYPE4_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EXTG_ACTYPE5_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EXTG_ACTYPE6_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EXTG_ACTYPE7_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EXTHDR_DAT_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_MAC_CONTROL */
#if defined(CONFIG_SDK_CHIP_FEATURE_PHY___SERDES)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t WSDS_ANA_00_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t WSDS_ANA_01_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t WSDS_ANA_02_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t WSDS_ANA_03_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t WSDS_ANA_04_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t WSDS_ANA_05_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t WSDS_ANA_06_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t WSDS_ANA_07_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t WSDS_ANA_08_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t WSDS_ANA_09_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t WSDS_ANA_0A_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t WSDS_ANA_0B_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t WSDS_DIG_00_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t WSDS_DIG_01_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t WSDS_DIG_02_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t WSDS_DIG_03_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t WSDS_DIG_04_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t WSDS_DIG_05_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t WSDS_DIG_06_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t WSDS_DIG_07_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t WSDS_DIG_08_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t WSDS_DIG_09_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t WSDS_DIG_0A_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t WSDS_DIG_0B_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t WSDS_DIG_0C_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t WSDS_DIG_0D_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t WSDS_DIG_0E_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t WSDS_DIG_0F_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t WSDS_DIG_10_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t WSDS_DIG_11_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t WSDS_DIG_12_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t WSDS_DIG_13_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t WSDS_DIG_14_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t WSDS_DIG_15_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t WSDS_DIG_16_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t WSDS_DIG_17_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t WSDS_DIG_18_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t WSDS_DIG_19_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t WSDS_DIG_1A_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t WSDS_DIG_1B_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t WSDS_DIG_1C_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t WSDS_DIG_1D_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t WSDS_DIG_1E_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t WSDS_DIG_1F_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t WSDS_DIG_20_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t WSDS_DIG_21_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t WSDS_DIG_22_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t WSDS_DIG_23_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t WSDS_DIG_24_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t WSDS_DIG_25_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t WSDS_DIG_26_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t WSDS_DIG_27_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t WSDS_DIG_28_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t WSDS_DIG_29_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t WSDS_DIG_2A_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t WSDS_DIG_2B_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t WSDS_DIG_2C_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FORCE_BEN_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_REG0_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_REG1_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_REG2_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_REG3_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_REG4_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_REG5_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_REG6_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_REG7_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_REG8_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_REG9_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_REG10_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_REG11_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_REG12_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_REG13_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_REG14_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_REG15_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_REG16_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_REG17_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_REG18_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_REG19_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_REG20_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_REG21_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_REG22_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_REG23_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_REG24_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_REG25_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_REG26_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_REG27_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_REG28_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_REG29_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_REG30_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_REG31_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_EXT_REG0_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_EXT_REG1_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_EXT_REG2_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_EXT_REG3_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_EXT_REG4_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_EXT_REG5_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_EXT_REG6_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_EXT_REG7_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_EXT_REG8_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_EXT_REG9_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_EXT_REG10_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_EXT_REG11_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_EXT_REG12_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_EXT_REG13_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_EXT_REG14_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_EXT_REG15_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_EXT_REG16_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_EXT_REG17_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_EXT_REG18_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_EXT_REG19_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_EXT_REG20_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_EXT_REG21_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_EXT_REG22_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_EXT_REG23_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_EXT_REG24_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_EXT_REG25_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_EXT_REG26_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_EXT_REG27_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_EXT_REG28_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_EXT_REG29_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_EXT_REG30_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_EXT_REG31_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FIB_REG0_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FIB_REG1_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FIB_REG2_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FIB_REG3_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FIB_REG4_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FIB_REG5_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FIB_REG6_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FIB_REG7_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FIB_REG8_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FIB_REG13_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FIB_REG14_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FIB_REG15_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FIB_REG16_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FIB_REG17_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FIB_REG18_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FIB_REG19_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FIB_REG20_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FIB_REG21_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FIB_REG22_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FIB_REG23_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FIB_REG26_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FIB_REG28_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FIB_REG29_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FIB_REG30_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FIB_REG31_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FIB_EXT_REG0_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FIB_EXT_REG1_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FIB_EXT_REG2_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FIB_EXT_REG3_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FIB_EXT_REG4_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FIB_EXT_REG5_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FIB_EXT_REG6_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FIB_EXT_REG7_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FIB_EXT_REG8_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FIB_EXT_REG13_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FIB_EXT_REG14_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FIB_EXT_REG15_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FIB_EXT_REG16_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FIB_EXT_REG17_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FIB_EXT_REG18_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FIB_EXT_REG19_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FIB_EXT_REG20_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FIB_EXT_REG21_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FIB_EXT_REG22_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FIB_EXT_REG23_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FIB_EXT_REG24_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FIB_EXT_REG25_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FIB_EXT_REG26_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FIB_EXT_REG27_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FIB_EXT_REG28_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FIB_EXT_REG29_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FIB_EXT_REG30_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FIB_EXT_REG31_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PHY_RG0X_CEN_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PHY_RG1X_CEN_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PHY_RG2X_CEN_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PHY_RG3X_CEN_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PHY_RG4X_CEN_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PHY_RG5X_CEN_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PHY_RG6X_CEN_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PHY_RG7X_CEN_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PHY_RG8X_CEN_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PHY_RG9X_CEN_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PHY_RG10X_CEN_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PHY_RG11X_CEN_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PHY_RG12X_CEN_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PHY_RG13X_CEN_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PHY_RG0X_PLL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PHY_RG1X_PLL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PHY_RG2X_PLL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PHY_RG3X_PLL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PHY_RG4X_PLL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PHY_RG5X_PLL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PHY_RG6X_PLL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PHY_RG7X_PLL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PHY_RG8X_PLL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PHY_RG9X_PLL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PHY_GDAC_IB_10M_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PHY_GDAC_IB_100M_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PHY_GDAC_IB_1000M_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PHY_POW_PLL_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_PHY___SERDES */
#if defined(CONFIG_SDK_CHIP_FEATURE_POWER_SAVING)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_PWRSAV_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_PWRSAV_CTRL_1_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t LOW_QUEUE_TH_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t HIGH_QUEUE_MSK_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PS_LINKID_GATCLK_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EEE_EEEP_PORT_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EEE_MISC_CTRL0_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EEE_TX_MINIFG_CTRL0_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EEE_TX_MINIFG_CTRL1_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EEE_WAIT_RX_INACTIVE_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EEE_TX_TIMER_100M_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EEE_TX_TIMER_GELITE_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EEE_TX_TIMER_GIGA_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EEE_TX_TIMER_2P5G_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t LOW_QUEUE_MSK_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_POWER_SAVING */
#if defined(CONFIG_SDK_CHIP_FEATURE_ADDRESS_TABLE_LOOKUP)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t LUT_UNMATCHED_SA_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t LUT_UNKN_SA_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t LUT_UNMATCHED_VLAN_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t LUT_UNKN_UC_DA_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t LUT_LEARN_OVER_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t LUT_CFG_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t LUT_AGEOUT_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t LUT_LRN_LIMITNO_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t L2_LRN_CNT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t L2_LRN_OVER_STS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t LUT_SYS_LRN_LIMITNO_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t L2_SYS_LRN_CNT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t L2_SYS_LRN_OVER_STS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t LUT_SYS_LRN_OVER_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t LUT_SYS_LRN_LIMIT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t UNKN_L2_MC_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t UNKN_IP4_MC_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t UNKN_IP6_MC_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t UNKN_MC_CFG_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t LUT_BC_FLOOD_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t LUT_UNKN_MC_FLOOD_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t LUT_UNKN_UC_FLOOD_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t LUT_BC_BEHAVE_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t LUT_WAN_EN_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t LUT_EXT_MBR_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t LUT_SA_TRF_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_ADDRESS_TABLE_LOOKUP */
#if defined(CONFIG_SDK_CHIP_FEATURE_ADDRESS_LEARNING___FLUSH)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t L2_TBL_FLUSH_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t L2_TBL_FLUSH_EN_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_ADDRESS_LEARNING___FLUSH */
#if defined(CONFIG_SDK_CHIP_FEATURE_L2__IP_MULTICAST)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t L2_IPMC_VLAN_LEAKY_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t L2_IPMC_ISO_LEAKY_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_L2__IP_MULTICAST */
#if defined(CONFIG_SDK_CHIP_FEATURE__IEEE802_1Q__VLAN)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t VLAN_PORT_ACCEPT_FRAME_TYPE_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t VLAN_INGRESS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t VLAN_EGRESS_TAG_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t VLAN_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t VLAN_PB_VID_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t IP4MC_EGRESS_MODE_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t IP6MC_EGRESS_MODE_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t VLAN_EXT_VID_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t VLAN_EXT_MBR_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE__IEEE802_1Q__VLAN */
#if defined(CONFIG_SDK_CHIP_FEATURE__IEEE802_1AD__PROVIDER_BRIDGES_Q_IN_Q)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SVLAN_UPLINK_PMSK_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SVLAN_SP2C_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SVLAN_P_SVID_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SVLAN_EXT_SVID_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SVLAN_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SVLAN_CFG_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE__IEEE802_1AD__PROVIDER_BRIDGES_Q_IN_Q */
#if defined(CONFIG_SDK_CHIP_FEATURE__IEEE802_1V__PROTOCOL_BASED_VLAN)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t VLAN_PPB_VLAN_VAL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t VLAN_PORT_PPB_VLAN_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t VLAN_EXT_PPB_VLAN_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE__IEEE802_1V__PROTOCOL_BASED_VLAN */
#if defined(CONFIG_SDK_CHIP_FEATURE_SPANNING_TREE)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t MSTI_CTRL_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_SPANNING_TREE */
#if defined(CONFIG_SDK_CHIP_FEATURE_PORT_ISOLATION)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PISO_PORT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PISO_EXT_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_PORT_ISOLATION */
#if defined(CONFIG_SDK_CHIP_FEATURE_RMA)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RMA_CTRL00_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RMA_CTRL01_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RMA_CTRL02_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RMA_CTRL03_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RMA_CTRL04_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RMA_CTRL08_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RMA_CTRL0D_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RMA_CTRL0E_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RMA_CTRL10_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RMA_CTRL11_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RMA_CTRL12_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RMA_CTRL13_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RMA_CTRL18_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RMA_CTRL1A_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RMA_CTRL20_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RMA_CTRL21_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RMA_CTRL22_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RMA_CTRL_CDP_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RMA_CTRL_SSTP_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RMA_CFG_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_RMA */
#if defined(CONFIG_SDK_CHIP_FEATURE_L2_MISC_)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t L2_SRC_PORT_PERMIT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t L2_SRC_EXT_PERMIT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t L2_CFG_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_L2_MISC_ */
#if defined(CONFIG_SDK_CHIP_FEATURE_STORM_CONTROL__B_M_UM_DLF_)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t STORM_CTRL_UM_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t STORM_CTRL_UC_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t STORM_CTRL_MC_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t STORM_CTRL_BC_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t STORM_CTRL_UM_METER_IDX_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t STORM_CTRL_UC_METER_IDX_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t STORM_CTRL_MC_METER_IDX_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t STORM_CTRL_BC_METER_IDX_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t STORM_CTRL_ARP_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t STORM_CTRL_DHCP_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t STORM_CTRL_IGMPMLD_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t STORM_CTRL_ARP_METER_IDX_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t STORM_CTRL_DHCP_METER_IDX_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t STORM_CTRL_IGMPMLD_METER_IDX_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t HOST_POLICE_CTRL_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_STORM_CONTROL__B_M_UM_DLF_ */
#if defined(CONFIG_SDK_CHIP_FEATURE_BANDWIDTH_CONTROL__INGRESS_EGRESS_)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t IGR_BWCTRL_P_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t IGR_BWCTRL_GLB_CTRL_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_BANDWIDTH_CONTROL__INGRESS_EGRESS_ */
#if defined(CONFIG_SDK_CHIP_FEATURE_METER_MARKER)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t METER_TB_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t METER_GLB_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t METER_LB_EXCEED_STS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t METER_PKT_RATE_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_METER_MARKER */
#if defined(CONFIG_SDK_CHIP_FEATURE_DENIAL_OF_SERVICE_ATTACK_PREVENTION)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t DOS_EN_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t DOS_CFG_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t DOS_SYNFLOOD_TH_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t DOS_FINFLOOD_TH_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t DOS_ICMPFLOOD_TH_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_DENIAL_OF_SERVICE_ATTACK_PREVENTION */
#if defined(CONFIG_SDK_CHIP_FEATURE_MIRRORING)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t MIR_CTRL_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_MIRRORING */
#if defined(CONFIG_SDK_CHIP_FEATURE_STATISTIC_COUNTERS)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t STAT_PRIVATE_REASON_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t STAT_ACL_REASON_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t STAT_PORT_TX_MIB_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t STAT_PORT_RX_MIB_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t STAT_PORT_OAM_MIB_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t STAT_ACL_CNT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t STAT_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t STAT_ACL_CNT_MODE_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t STAT_ACL_CNT_TYPE_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t STAT_ACL_CNT_RST_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t STAT_PORT_RST_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t STAT_RST_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t STAT_RST_CFG_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t DOT3_Q_TX_FRAMES_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t DOT3_MPCP_RX_DISC_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t DOT3_EPON_FEC_CORRECTED_BLOCKS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t DOT3_EPON_FEC_UNCORRECTED_BLOCKS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t DOT3_EPON_FEC_CODING_VIO_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t DOT3_NOT_BROADCAST_BIT_NOT_ONU_LLID_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t DOT3_BROADCAST_BIT_PLUS_ONU_LLID_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t DOT3_BROADCAST_NOT_ONUID_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t DOT3_CRC8_ERRORS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t DOT3_NOT_BROADCAST_LLID_7FFF_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t DOT3_LLID_RX_BROADCAST_DROP_FRAMES_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t DOT3_MPCP_TX_REG_ACK_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t DOT3_MPCP_TX_REPORT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t DOT3_MPCP_RX_GATE_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t DS_FEC_CORRECTED_BLOCKS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t DS_FEC_UNCORRECTED_BLOCKS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t DOT3_ONUID_NOT_BROADCAST_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t STAT_DOT3_LLIDRXFRAMESDROP_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t DOT3_MPCP_TX_REG_REQ_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t OMCI_DROP_PKT_CNT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t OMCI_TX_PKT_CNT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t OMCI_RX_PKT_CNT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t OMCI_TX_BYTE_CNT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t OMCI_RX_BYTE_CNT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t OMCI_CRC_ERROR_PKT_CNT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t STAT_BRIDGE_DOT1DTPLEARNEDENTRYDISCARDS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t STAT_L34_MIB_UC_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t STAT_L34_MIB_MC_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t STAT_L34_MIB_BC_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t STAT_HOST_TX_MIB_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t STAT_HOST_RX_MIB_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t STAT_L34_FLOW_MIB_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t STAT_L34_RST_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t STAT_HOST_RST_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t STAT_L34_MC_RST_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t STAT_L34_BC_RST_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t STAT_L34_FLOW_RST_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_STATISTIC_COUNTERS */
#if defined(CONFIG_SDK_CHIP_FEATURE_FLOWCONTROL___BACKPRESSURE_THRESHOLD)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FC_DROP_ALL_TH_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FC_PAUSE_ALL_TH_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FC_GLB_FCOFF_HI_TH_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FC_GLB_FCOFF_LO_TH_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FC_GLB_HI_TH_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FC_GLB_LO_TH_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FC_P_HI_TH_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FC_P_LO_TH_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FC_P_FCOFF_HI_TH_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FC_P_FCOFF_LO_TH_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FC_Q_EGR_DROP_TH_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FC_P_EGR_DROP_TH_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FC_P_Q_EGR_FC_EN_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_FC_P_Q_EGR_FC_EN_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FC_DBG_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CLR_MAX_USED_PAGE_CNT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FC_TOTAL_PAGE_CNT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FC_PE_USED_PAGE_CNT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FC_Q_USED_PAGE_CNT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FC_TL_USED_PAGE_CNT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FC_PUB_USED_PAGE_CNT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FC_PUB_FCOFF_USED_PAGE_CNT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FC_P_USED_PAGE_CNT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FC_PON_GLB_HI_TH_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FC_PON_GLB_LO_TH_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FC_PON_P_HI_TH_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FC_PON_P_LO_TH_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FC_PON_Q_EGR_FC_EN_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t TH_TX_PREFET_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SPG_GLB_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FC_SWPBO_P_EGR_DROP_TH_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FC_FB_P_EGR_DROP_TH_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FC_SWPBO_USED_PAGE_CNT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FC_SWPBO_Q_USED_PAGE_CNT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FC_SWPBO_GLB_HI_TH_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FC_SWPBO_GLB_LO_TH_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FC_SWPBO_P_HI_TH_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FC_SWPBO_P_LO_TH_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FC_PON_GLB_DROP_HI_TH_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FC_PON_GLB_DROP_LO_TH_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FC_PON_P_DROP_HI_TH_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FC_PON_P_DROP_LO_TH_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FC_P_USED_PAGE_CNT_X_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_INBW_HBOUND_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_INBW_LBOUND_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_PBO_PORT_EN_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_FLOWCONTROL___BACKPRESSURE_THRESHOLD */
#if defined(CONFIG_SDK_CHIP_FEATURE_CONGESTION_AVOIDANCE)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SC_P_CTRL_0_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SC_P_CTRL_1_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_CONGESTION_AVOIDANCE */
#if defined(CONFIG_SDK_CHIP_FEATURE_QUEUE_MANAGEMENT)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t QOS_INTPRI_TO_QID_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t QOS_PORT_QMAP_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EPON_LLID_SIDMAP_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t QOS_UNI_TRAP_PRI_CTRL_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_QUEUE_MANAGEMENT */
#if defined(CONFIG_SDK_CHIP_FEATURE_INGRESS_PRIORITY_DECISION)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t QOS_1Q_PRI_REMAP_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t QOS_DSCP_REMAP_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t QOS_PB_PRI_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PRI_SEL_TBL_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PRI_SEL_TBL_CFG_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t QOS_PRI_REMAP_IN_CPU_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_INGRESS_PRIORITY_DECISION */
#if defined(CONFIG_SDK_CHIP_FEATURE_REMARKING)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RMK_DOT1Q_RMK_EN_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RMK_1Q_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RMK_DSCP_RMK_EN_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RMK_DSCP_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RMK_DSCP_INT_PRI_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RMK_P_DSCP_SEL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RMK_P_1P_SEL_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_REMARKING */
#if defined(CONFIG_SDK_CHIP_FEATURE_SCHEDULING)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t WFQ_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EGR_BWCTRL_P_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t LINE_RATE_1G_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t LINE_RATE_500M_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t LINE_RATE_100M_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t LINE_RATE_10M_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t OUTPUT_DROP_CFG_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t OUTPUT_DROP_EN_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t WFQ_PORT_CFG0_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t WFQ_PORT_CFG1_7_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t WFQ_TYPE_PORT_CFG_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t APR_EN_PORT_CFG_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t APR_METER_PORT_CFG_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t P_QUEUE_EMPTY_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t MOCIR_BPT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t BYTE_TOKEN_METER_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SCH_DUMMY0_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t HYS_GAP_OUTQ_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t LINE_RATE_2500M_0_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t LINE_RATE_2500M_1_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SCH_WFQ_OPT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EGR_BWCTRL_SWPBO_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_SCHEDULING */
#if defined(CONFIG_SDK_CHIP_FEATURE_PIE_TEMPLATE)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t ACL_TEMPLATE_CTRL_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_PIE_TEMPLATE */
#if defined(CONFIG_SDK_CHIP_FEATURE_INGRESS_ACL)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t ACL_EN_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t ACL_PERMIT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t ACL_ACTION_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_INGRESS_ACL */
#if defined(CONFIG_SDK_CHIP_FEATURE_RANGE_CHECK__PORT_VLAN_IP_L4PORT_)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RNG_CHK_VID_RNG_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RNG_CHK_IP_RNG_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RNG_CHK_L4PORT_RNG_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RNG_CHK_PKTLEN_RNG_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_RANGE_CHECK__PORT_VLAN_IP_L4PORT_ */
#if defined(CONFIG_SDK_CHIP_FEATURE_OAM)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t OAM_P_CTRL_0_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t OAM_P_CTRL_1_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t OAM_CTRL_0_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t OAM_P_EN_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_OAM */
#if defined(CONFIG_SDK_CHIP_FEATURE_PARSER_HSB)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t HSB_CFG_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t HSB_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t HSB_DATA_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t HSB_PARSER_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_PARSER_HSB */
#if defined(CONFIG_SDK_CHIP_FEATURE_MODIFIER_HSA)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t HSA_DATA_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t HSA_ACL_REASON_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t HSD_DATA_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t HSD_CFG_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EXTRA_TAG_INFO_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t HSM_DATA_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FBHSA_DATA_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FBHSA_CTRL_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_MODIFIER_HSA */
#if defined(CONFIG_SDK_CHIP_FEATURE_DEBUGGING__ALE__LOOPBACK__DROP_MECHANISM__FC_AND_QM_)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PORT_VM_EN_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PORT_VM_RX_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PORT_VM_TX_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_DEBUGGING__ALE__LOOPBACK__DROP_MECHANISM__FC_AND_QM_ */
#if defined(CONFIG_SDK_CHIP_FEATURE_IGMP_SNOOPING)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t IGMP_MC_GROUP_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t IGMP_GLB_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t IGMP_P_CTRL_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_IGMP_SNOOPING */
#if defined(CONFIG_SDK_CHIP_FEATURE_L3_MISC)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FB_IPID_TBL_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_L3_MISC */
#if defined(CONFIG_SDK_CHIP_FEATURE_TABLE_ACCESS)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t TBL_ACCESS_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t TBL_ACCESS_STS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t TBL_ACCESS_WR_DATA_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t TBL_ACCESS_RD_DATA_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_TABLE_ACCESS */
#if defined(CONFIG_SDK_CHIP_FEATURE_PTP__PRECISION_TIME_PROTOCOL_)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PTP_TIME_SEC_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PTP_TIME_NSEC_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PTP_TIME_OFFSET_SEC_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PTP_TIME_OFFSET_8NSEC_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PTP_TIME_FREQ_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PTP_TIME_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PTP_TRANSPARENT_CFG_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PTP_IGR_MSG_ACT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PTP_EGR_MSG_ACT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PTP_MEANPATH_DELAY_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PTP_RX_TIME_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PTP_P_EN_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PTP_PON_TOD_SEC_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PTP_PON_TOD_NSEC_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PTP_P_TX_IND_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PTP_PON_TOD_FREERUN_NSEC_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_PTP__PRECISION_TIME_PROTOCOL_ */
#if defined(CONFIG_SDK_CHIP_FEATURE_PARSER)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PARSER_FIELD_SELTOR_CTRL_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_PARSER */
#if defined(CONFIG_SDK_CHIP_FEATURE_PON_MAC_SCHEDULING_CONFIG)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_TRAP_CFG_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RX_SID_GOOD_CNT_US_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RX_ERR_CNT_US_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RX_SID_BAD_CNT_US_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t TX_SID_CNT_US_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t TX_SID_FRAG_CNT_US_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t TX_EMPTY_EOB_US_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RX_SID_CNT_US_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CNT_MASK_US_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RX_DROP_CNT_US_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RX_SID_GOOD_CNT_DS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RX_SID_BAD_CNT_DS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RX_SID_FRAG_CNT_DS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RX_DROP_CNT_DS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RX_ERR_CNT_DS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t TX_SID_CNT_DS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CNT_MASK_DS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RX_STS_DS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_LX_MON_SAMPLE_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_LX_MON_PERIOD_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_LX_MON_STSCTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_LX_MON_STS_0_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_LX_MON_STS_1_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_LX_MON_STS_2_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_LX_MON_STS_3_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_LX_MON_STS_4_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_LX_MON_STS_5_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_LX_MON_STS_6_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_LX_MON_STS_7_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_LX_MON_STS_8_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_LX_MON_STS_9_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_LX_MON_STS_10_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_LX_MON_STS_11_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_LX_MON_STS_12_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_LX_MON_STS_13_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_LX_MON_STS_14_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_LX_MON_STS_15_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_LX_MON_STS_16_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_LX_MON_STS_17_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_LX_MON_STS_18_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_LX_MON_STS_19_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_LX_MON_STS_20_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_LX_MON_STS_21_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_DBGO_TOPSEL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t ARB_TIMEOUT_GLB_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_PON_MAC_SCHEDULING_CONFIG */
#if defined(CONFIG_SDK_CHIP_FEATURE_INTERFACE)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PONIP_CTL_US_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t ARB_TIMEOUT_US_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t DSCRUNOUT_US_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t DRN_CMD_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t IP_MSTBASE_US_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_DSC_USAGE_US_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_US_FIFO_CTL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_IPSTS_US_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_SID2QID_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_SIDVALID_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_DSCCTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_BW_THRES_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_OMCI_CFG_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_DSC_STS_US_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_DSC_CFG_US_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_DBGO_CFG_US_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_EMPTY_EOB_CFG_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_INTERFACE */
#if defined(CONFIG_SDK_CHIP_FEATURE_PONIP_SCHEDULING_UPSTREAM)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_DBA_IFG_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t MOCIR_FRC_MD_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t MOCIR_FRC_VAL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EPON_RPT_QSET_NUM_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EPON_RESERVED_REG_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t MOCIR_TH_H_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t MOCIR_TH_L_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_OLT_BW_MTR_FULL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_TB_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_SCH_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_QID_CIR_RATE_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_QID_PIR_RATE_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_SCH_QMAP_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_TCONT_EN_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_WFQ_TYPE_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_WFQ_LEAKY_BUCKET_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_WFQ_WEIGHT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_SID_STOP_TH_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_SID_GLB_TH_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_SID_RPV_TH_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PONIP_DBG_CTRL_US_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PONIP_TOTAL_PAGE_CNT_US_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PONIP_SID_USED_PAGE_CNT_US_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_DPRU_RPT_PRD_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PONIP_SID_OVER_STS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PONIP_SID_OVER_LATCH_STS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_SCH_RATE_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_EGR_RATE_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EPON_REPORT_MODE_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EPON_DYGSP_RPT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t US_SCH_EGR_IFG_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_SCH_OPT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EPON_Q_RPT_LVL1_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EPON_Q_RPT_LVL2_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EPON_Q_RPT_LVL3_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EPON_KT_REPORT_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PONSCH_PIR_SIDOV_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_SCH_ASIC_OPT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_DBRU_DBG_0_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_DBRU_DBG_1_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_PONIP_SCHEDULING_UPSTREAM */
#if defined(CONFIG_SDK_CHIP_FEATURE_PONNIC_CTRL)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PKT_OK_CNT_US_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PKT_ERR_CNT_US_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PKT_MISS_CNT_US_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CMD_REG_US_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t INTERRUPT_US_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t TX_CFG_US_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RX_CFG_US_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CFG_US_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t MEDIA_STS_US_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_INTERRUPT_US_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t BIST_US_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_PONNIC_CTRL */
#if defined(CONFIG_SDK_CHIP_FEATURE_CPU_IF)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t TXCD01_US_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RXFDP1_US_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RXCDO1_US_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PROBE_SELECT_US_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CONFIG_CLK_US_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t IO_CMD_0_US_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t IO_CMD_1_US_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_CPU_IF */
#if defined(CONFIG_SDK_CHIP_FEATURE_INTERFACE)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PONIP_CTL_DS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_DS_BANK1_DBG_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t ARB_TIMEOUT_DS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t DSCRUNOUT_DS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t IP_MSTBASE_DS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_DSC_USAGE_DS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_IPSTS_DS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_DSCCTRL_DS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_DSC_STS_DS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_DSC_CFG_DS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_DBGO_CFG_DS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_SID_Q_MAP_DS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_Q_CONFIG_DS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_FC_CONFIG_DS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_DS_PBO_PAGE_Q0_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_DS_PBO_PAGE_Q1_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_DS_PBO_PAGE_Q2_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_INTERFACE */
#if defined(CONFIG_SDK_CHIP_FEATURE_PONIP_SCHEDULING_DOWNSTREAM)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PONIP_DBG_CTRL_DS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PONIP_TOTAL_PAGE_CNT_DS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PONIP_SID_USED_PAGE_CNT_DS_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_PONIP_SCHEDULING_DOWNSTREAM */
#if defined(CONFIG_SDK_CHIP_FEATURE_PONNIC_CTRL)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PKT_OK_CNT_DS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PKT_ERR_CNT_DS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PKT_MISS_CNT_DS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CMD_REG_DS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t INTERRUPT_DS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t TX_CFG_DS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RX_CFG_DS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CFG_DS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t MEDIA_STS_DS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_INTERRUPT_DS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t BIST_DS_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_PONNIC_CTRL */
#if defined(CONFIG_SDK_CHIP_FEATURE_CPU_IF)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t TXCD01_DS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RXFDP1_DS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RXCDO1_DS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PROBE_SELECT_DS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CONFIG_CLK_DS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t IO_CMD_0_DS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t IO_CMD_1_DS_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_CPU_IF */
#if defined(CONFIG_SDK_CHIP_FEATURE_SWPBO)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_PBO_MSTBASE_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_PBO_CTL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_PBO_IPSTS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_PBO_PORT_TH_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_PBO_Q_QUEUE_TH_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_PBO_PORT_RUNOUT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_PBO_PORT_MAX_DSC_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_PBO_PORT_DSC_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_PBO_PORT_PAGE_CNT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_PBO_PORT_MAX_CLEAR_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_PBO_SID_PAGE_CNT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_PBO_TOTAL_PAGE_CNT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_PBO_TOTAL_MAX_CLEAR_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_PBO_SCH_MISC_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_PBO_DBG_SEL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SW_PBO_DSCCTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t ARB_TIMEOUT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t ES_BIST_RSLT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RX_DROP_CNT_ES_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RX_SID_GOOD_CNT_ES_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RX_SID_BAD_CNT_ES_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t TX_SID_CNT_ES_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CNT_MASK_ES_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_SWPBO */
#if defined(CONFIG_SDK_CHIP_FEATURE_SWPBO_NIC_CTRL)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SWPBO_PKT_OK_CNT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SWPBO_PKT_ERR_CNT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SWPBO_PKT_MISS_CNT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SWPBO_CMD_REG_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SWPBO_INTERRUPT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SWPBO_TX_CFG_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SWPBO_RX_CFG_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SWPBO_CFG_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SWPBO_MEDIA_STS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SWPBO_SW_INTERRUPT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SWPBO_BIST_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_SWPBO_NIC_CTRL */
#if defined(CONFIG_SDK_CHIP_FEATURE_SWPBO_NIC_CPU_IF)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SWPBO_TXCD01_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SWPBO_RXFDP1_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SWPBO_RXCDO1_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SWPBO_PROBE_SELECT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SWPBO_CONFIG_CLK_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SWPBO_IO_CMD_0_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SWPBO_IO_CMD_1_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_SWPBO_NIC_CPU_IF */
#if defined(CONFIG_SDK_CHIP_FEATURE_GPON_MAC_GENERAL_CONFIG)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_INT_DLT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_RESET_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_VERSION_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_TEST_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_AES_BYPASS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_FRAME_PULSE_CFG_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_INTR_MASK_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_INTR_STS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_MAC_BIST_RSLT_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_GPON_MAC_GENERAL_CONFIG */
#if defined(CONFIG_SDK_CHIP_FEATURE_GTC_DOWNSTREAM)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_DS_INTR_DLT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_DS_INTR_MASK_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_DS_INTR_STS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_DS_ONU_ID_STATUS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_DS_CFG_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_DS_REQ_CHECK_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_DS_PLOAM_CFG_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_DS_LOS_CFG_STS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_DS_SUPERFRAME_CNT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_DS_TOD_SUPERFRAME_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_DS_PPS_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_DS_PLOAM_IND_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_DS_PLOAM_MSG_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_DS_ALLOC_IND_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_DS_ALLOC_WR_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_DS_ALLOC_RD_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_DS_PORT_IND_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_DS_PORT_WR_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_DS_PORT_RD_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_DS_PORT_CNTR_IND_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_DS_PORT_CNTR_STAT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_DS_MISC_CNTR_BIP_ERR_BLK_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_DS_MISC_CNTR_BIP_ERR_BIT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_DS_MISC_CNTR_FEC_CORRECT_BIT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_DS_MISC_CNTR_FEC_CORRECT_BYTE_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_DS_MISC_CNTR_FEC_CORRECT_CW_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_DS_MISC_CNTR_FEC_UNCOR_CW_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_DS_MISC_CNTR_LOM_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_DS_MISC_CNTR_PLOAM_ACPT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_DS_MISC_CNTR_PLOAM_FAIL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_DS_MISC_CNTR_BWM_FAIL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_DS_MISC_CNTR_BWM_INV_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_DS_MISC_CNTR_ACTIVE_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_DS_MISC_CNTR_BWM_ACPT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_DS_MISC_CNTR_GEM_LOS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_DS_MISC_CNTR_HEC_CORRECT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_DS_MISC_CNTR_GEM_IDLE_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_DS_MISC_CNTR_GEM_FAIL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_DS_MISC_CNTR_GEM_NON_IDLE_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_DS_MISC_CNTR_PLEN_CORRECT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_DS_TDM_PTI_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_DS_OMCI_PTI_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_DS_ETH_PTI_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_DS_BPOS_SUB_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_DS_BPOS_ADD_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_DS_DUMMY_1_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_DS_DUMMY_2_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_DS_PPS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_DS_PPS_INTR_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t DS_FEC_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PLOAM_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GEM_CNTR_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GEM_TOKEN_A_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t BWMAP_CAP_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t BWMAP_BUF_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t TCONT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GEM_L_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GEM_H_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_DS_TRAFFIC_CFG_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_GTC_DOWNSTREAM */
#if defined(CONFIG_SDK_CHIP_FEATURE_BWMAP_CAPTURE)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_BWMAP_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_BWMAP_STS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_BWMAP_DATA_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_BWMAP_CAPTURE */
#if defined(CONFIG_SDK_CHIP_FEATURE_AES_DECRYPT)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_AES_INTR_DLT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_AES_INTR_MASK_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_AES_INTR_STS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_AES_KEY_SWITCH_REQ_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_AES_KEY_SWITCH_TIME_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_AES_KEY_WORD_IND_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_AES_WORD_DATA_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CTL_INFO_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CTL_DATA_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CIPHER_1_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CIPHER_2_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t KEY_EXPN_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_AES_DECRYPT */
#if defined(CONFIG_SDK_CHIP_FEATURE_GEM_PORT_DOWNSTREAM)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GEM_DS_RX_CNTR_IND_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GEM_DS_RX_CNTR_STAT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GEM_DS_FWD_CNTR_IND_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GEM_DS_FWD_CNTR_STAT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GEM_DS_MISC_IND_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GEM_DS_MISC_CNTR_STAT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GEM_DS_MC_CFG_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GEM_DS_MC_IND_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GEM_DS_MC_WR_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GEM_DS_MC_RD_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GEM_DS_FRM_TIMEOUT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GEM_DS_MC_ADDR_PTN_IPV4_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GEM_DS_MC_ADDR_PTN_IPV6_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CLASSIFY_BUF_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CLASSIFY_CNTR_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t ASSEMBLY_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t MC_FILTER_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_GEM_PORT_DOWNSTREAM */
#if defined(CONFIG_SDK_CHIP_FEATURE_GTC_UPSTREAM)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_US_INTR_DLT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_US_INTR_MASK_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_US_INTR_STS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_US_ONU_ID_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_US_CFG_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_US_WRITE_PROTECT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_US_TX_PATTERN_CTL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_US_TX_PATTERN_BG_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_US_TX_PATTERN_FG_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_US_MIN_DELAY_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_US_EQD_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_US_LASER_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_US_BOH_CFG_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_US_BOH_DATA_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_US_PLOAM_IND_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_US_PLOAM_DATA_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_US_PLOAM_CFG_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_US_MISC_CNTR_IDX_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_US_MISC_CNTR_STAT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_US_RDI_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_US_DG_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_US_OPTIC_SD_TH_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_US_PROC_MODE_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t G_DMY_XX_01_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t G_DMY_XX_02_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t G_DMY_XX_03_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_US_PWR_SAV_MODE_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GTC_US_TX_PTN_BEN_OFF_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t BWM_TBL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t BWM_MEM0_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t BWM_MEM1_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PLOAM_A_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t BISTXXX_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t DBR_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_GTC_UPSTREAM */
#if defined(CONFIG_SDK_CHIP_FEATURE_GEM_UPSTREAM)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GEM_US_INTR_DLT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GEM_US_INTR_MASK_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GEM_US_INTR_STS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GEM_US_PTI_CFG_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GEM_US_PWR_SAV_CFG_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GEM_US_ETH_GEM_RX_CNTR_IDX_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GEM_US_ETH_GEM_RX_CNTR_STAT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GEM_US_PTN_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GEM_US_EOB_MERGE_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PCFG_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GEM_BCNT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FIFO_BANK0_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FIFO_BANK1_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t IDLE_BCNT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GEM_PCNT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GEM_US_PORT_MAP_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GPON_GEM_US_BYTE_STAT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t TCONT_IDLE_BYTE_STAT_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_GEM_UPSTREAM */
#if defined(CONFIG_SDK_CHIP_FEATURE_EPON_CONFIGURATION)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EPON_FEC_CONFIG_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EPON_ASIC_TIMING_ADJUST1_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EPON_ASIC_TIMING_ADJUST2_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EPON_RGSTR1_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EPON_RGSTR2_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EPON_DEBUG1_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EPON_DEBUG2_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EPON_TIMER_CONFIG1_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EPON_INTR_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SYNC_TIME_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t LASER_ON_OFF_TIME_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t MIN_GRANT_START_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t MAX_GRANT_START_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EPON_TIME_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EP_MISC_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t LLID_TABLE_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EPON_MPCP_CTR_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EPON_TX_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EPON_DECRYP_CFG_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EPON_DECRYP_KEY0_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EPON_DECRYP_KEY1_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EPON_SCB_DECRYP_KEY0_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EPON_SCB_DECRYP_KEY1_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EPON_MISC_CFG_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EPON_LOCAL_TIME_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EPON_REG_ACK_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EPON_GATE_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EPON_GRANT_SHIFT_START_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EPON_GATE_ANA_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EPON_GATE_ANA_CNT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EPON_TBL_ACCESS_STS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EPON_TBL_ACCESS_RD_DATA_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EPON_RPT_QSET_NUM1_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EPON_RPT_LVL1_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EPON_SCH_TIMING_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EPON_FEC_RST_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t DET_BEN_OFF_COND_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PRG_EPON_GN0_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PRG_EPON_GN1_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PRG_EPON_GN2_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t DBG_LCTM_DRF_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CLR_INVLD_LID_GN_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t DS_PRS_LC_FEC_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EPON_ERR_INTR2_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t DET_OFF1_OFST_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EPON_REG_BAK0_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EPON_REG_BAK1_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EPON_REG_BAK2_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EPON_MSK_NXT_CNT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EPON_GRANT_INDACS_CMD_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EPON_GRANT_INDACS_DATA0_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EPON_GRANT_INDACS_DATA1_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EPON_GRANT_INDACS_DATA2_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EPON_MPCP_DROP_CNT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EPON_TOD_EN_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EPON_TOD_LOCAL_TIME_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EPON_MSK_NXT_CNT2_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EPON_TOD_TIME_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EPON_RDM_SEED_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EPON_ASIC_OPTI2_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EPON_ASIC_OPTI1_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EPON_REG_REQ_ACK_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EPON_REG_REQ_PAD_DATA_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EPON_REG_ACK_PAD_DATA_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EPON_TX_OPT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EPON_US_INF_DBG_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EPON_TX_SD_CHK_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t EPON_ASIC_OPTI3_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_EPON_CONFIGURATION */
#if defined(CONFIG_SDK_CHIP_FEATURE_FLOW_TRAFFIC_TABLE)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FT_TRF_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_FLOW_TRAFFIC_TABLE */
#if defined(CONFIG_SDK_CHIP_FEATURE_FB_CTRL)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t MODE_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t L34_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t LUP_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PRE_HASH_ITM1_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PRE_HASH_ITM2_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PRE_HASH_ITM3_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PRE_HASH_ITM4_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CAM_TRF_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t IF_TRF_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PIPE_SIG_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t MTR_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t TCP_ACK_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GRE_SEQ_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t GRE_ACK_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_FB_CTRL */
#if defined(CONFIG_SDK_CHIP_FEATURE_TABLE_ACCESS)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t NAT_TBL_ACCESS_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t NAT_TBL_ACCESS_CLR_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t NAT_TBL_ACCESS_RDDATA_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t NAT_TBL_ACCESS_WRDATA_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_TABLE_ACCESS */
#if defined(CONFIG_SDK_CHIP_FEATURE_FB_HSBA)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t HSBA_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t HSB_DESC_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t HSA_DESC_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_FB_HSBA */
#if defined(CONFIG_SDK_CHIP_FEATURE_CC_CTRL)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CC_CFG_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CC_BAB_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CC_INTR_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CC_CMD_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CC_SFLW_0_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CC_SFLW_1_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CC_SFLW_2_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CC_SFLW_3_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CC_SFLW_4_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CC_SFLW_5_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CC_SFLW_6_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CC_SFLW_7_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CC_SFLW_8_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CC_STS_0_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CC_STS_1_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CC_STS_2_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CC_STS_3_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t CC_STA_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_CC_CTRL */
#if defined(CONFIG_SDK_CHIP_FEATURE_FB_MTR)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t MTR_SETTING_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_FB_MTR */
#if defined(CONFIG_SDK_CHIP_FEATURE_TEMP_REGISTER)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RGF_VER_GLB_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RGF_VER_ALE_GLB_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RGF_VER_ALE_ACL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RGF_VER_ALE_CVLAN_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RGF_VER_ALE_DPM_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RGF_VER_ALE_L2_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RGF_VER_ALE_MLTVLAN_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RGF_VER_ALE_SVLAN_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RGF_VER_ALE_EAV_AFBK_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RGF_VER_INTR_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RGF_VER_LED_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RGF_VER_PER_PORT_MAC_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RGF_VER_SDSREG_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RGF_VER_SWCORE_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RGF_VER_EPON_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RGF_VER_ALE_RMA_ATTACK_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RGF_VER_BIST_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RGF_VER_EGR_OUTQ_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RGF_VER_EGR_SCH_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RGF_VER_ALE_HSA_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RGF_VER_ALE_METER_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RGF_VER_MIB_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RGF_VER_ALE_PISO_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RGF_VER_CHIP_INFO_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RGF_VER_PHY_IP_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RGF_VER_PONIP_GLB_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RGF_VER_PONIP_US_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RGF_VER_PONIP_DS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RGF_VER_SWPBO_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RSVD_GLB_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RSVD_ALE_GLB_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RSVD_ALE_DPM_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RSVD_ALE_EAV_AFBK_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RSVD_LED_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RSVD_PER_PORT_MAC_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RSVD_SDSREG_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RSVD_SWCORE_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RSVD_EPON_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RSVD_EGR_OUTQ_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RSVD_EGR_SCH_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RSVD_ALE_HSA_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RSVD_ALE_METER_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RSVD_PONIP_GLB_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RSVD_PONIP_US_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RSVD_PONIP_DS_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t RSVD_SWPBO_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_TEMP_REGISTER */
#if defined(CONFIG_SDK_CHIP_FEATURE_1PPSTOD__1_PLUSE_PER_SECOND_TIME_OF_DAY_)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PPS_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t TOD_FRAME_CONFIG_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t HARDWARE_DEFINE_FRAME_1_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t HARDWARE_DEFINE_FRAME_0_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SOFTWARE_DEFINE_FRAME_7_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SOFTWARE_DEFINE_FRAME_6_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SOFTWARE_DEFINE_FRAME_5_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SOFTWARE_DEFINE_FRAME_4_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SOFTWARE_DEFINE_FRAME_3_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SOFTWARE_DEFINE_FRAME_2_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SOFTWARE_DEFINE_FRAME_1_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SOFTWARE_DEFINE_FRAME_0_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t TOD_BAUDRATE_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t TOD_DATA_FRAME_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_1PPSTOD__1_PLUSE_PER_SECOND_TIME_OF_DAY_ */
#if defined(CONFIG_SDK_CHIP_FEATURE_SDS_WDIG)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_WDIG_REG00_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_WDIG_REG01_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_WDIG_REG02_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_WDIG_REG03_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_WDIG_REG04_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_WDIG_REG05_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_WDIG_REG06_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_WDIG_REG07_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_WDIG_REG08_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_WDIG_REG09_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_WDIG_REG10_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_WDIG_REG11_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_WDIG_REG12_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_WDIG_REG13_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_WDIG_REG14_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_WDIG_REG15_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_WDIG_REG16_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_SDS_WDIG */
#if defined(CONFIG_SDK_CHIP_FEATURE_SDS_MISC)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_MISC_REG00_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_MISC_REG01_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_MISC_REG02_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_MISC_REG03_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_MISC_REG04_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_MISC_REG05_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_MISC_REG06_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_MISC_REG07_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_MISC_REG08_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_MISC_REG09_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_MISC_REG10_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_MISC_REG11_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_MISC_REG12_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_MISC_REG13_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_MISC_REG14_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_MISC_REG15_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_MISC_REG16_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_MISC_REG17_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_MISC_REG18_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_MISC_REG19_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_MISC_REG20_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_MISC_REG21_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_MISC_REG22_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_MISC_REG23_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_MISC_REG24_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_MISC_REG25_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_MISC_REG26_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_MISC_REG27_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_MISC_REG28_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_MISC_REG29_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_MISC_REG30_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_MISC_REG31_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_SDS_MISC */
#if defined(CONFIG_SDK_CHIP_FEATURE_SDS_COM)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_COM_REG00_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_COM_REG01_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_COM_REG02_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_COM_REG03_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_COM_REG04_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_COM_REG05_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_COM_REG06_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_COM_REG07_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_COM_REG08_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_COM_REG09_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_COM_REG10_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_COM_REG11_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_COM_REG12_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_COM_REG13_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_COM_REG14_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_COM_REG15_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_COM_REG16_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_COM_REG17_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_COM_REG18_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_COM_REG19_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_COM_REG20_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_COM_REG21_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_COM_REG22_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_COM_REG23_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_COM_REG24_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_COM_REG25_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_COM_REG26_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_COM_REG27_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_COM_REG28_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_COM_REG29_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_COM_REG30_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_COM_REG31_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_SDS_COM */
#if defined(CONFIG_SDK_CHIP_FEATURE_SDS_SPD)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_SPD_REG32_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_SPD_REG33_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_SPD_REG34_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_SPD_REG35_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_SPD_REG36_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_SPD_REG37_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_SPD_REG38_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_SDS_SPD */
#if defined(CONFIG_SDK_CHIP_FEATURE_SDS_SPD_1P25G)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_1P25_REG32_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_1P25_REG33_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_1P25_REG34_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_1P25_REG35_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_1P25_REG36_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_1P25_REG37_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_1P25_REG38_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_SDS_SPD_1P25G */
#if defined(CONFIG_SDK_CHIP_FEATURE_SDS_SPD_GPON)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_GPON_REG32_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_GPON_REG33_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_GPON_REG34_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_GPON_REG35_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_GPON_REG36_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_GPON_REG37_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_GPON_REG38_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_SDS_SPD_GPON */
#if defined(CONFIG_SDK_CHIP_FEATURE_SDS_SPD_EPON)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_EPON_REG32_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_EPON_REG33_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_EPON_REG34_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_EPON_REG35_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_EPON_REG36_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_EPON_REG37_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t SDS_ANA_EPON_REG38_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_SDS_SPD_EPON */
#if defined(CONFIG_SDK_CHIP_FEATURE_OTHER)
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t PON_RATE_CTRL_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t WRR_LB_CNT_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t METER_KEEP_FF_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t FORCE_P_ABLTY_X_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t P_ABLTY_X_RTL9603D_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9603D)
extern rtk_regField_t ABLTY_FORCE_MODE_X_RTL9603D_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_OTHER */



#endif    /* __RTK_RTL9603D_REGFIELD_LIST_H__ */
