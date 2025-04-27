#ifndef __RTK_MANGO_REGFIELD_LIST_H__
#define __RTK_MANGO_REGFIELD_LIST_H__

/*
 * Include Files
 */
#include <common/rt_autoconf.h>
#ifdef __BOOTLOADER__
#include <hal/chipdef/mango/rtk_mango_uboot_feature_def.h>
#else   /* __BOOTLOADER__ */
#include <hal/chipdef/mango/rtk_mango_feature_def.h>
#endif  /* __BOOTLOADER__ */

#if defined(CONFIG_SDK_CHIP_FEATURE_CHIP_INFORMATION)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t CHIP_INFO_DMY_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MODEL_NAME_INFO_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t CHIP_INFO_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t CFG_DMY_CHIP_INFO_0_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t CFG_DMY_CHIP_INFO_1_RTL9310_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_CHIP_INFORMATION */
#if defined(CONFIG_SDK_CHIP_FEATURE_RESET)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RST_GLB_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t CHIP_RST_STS_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t WDOG_RST_STS_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t BONDING_STS_RTL9310_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_RESET */
#if defined(CONFIG_SDK_CHIP_FEATURE_PLL___BIAS)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t BG_POR_TOP_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t XTAL_TOP_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PLL_GLB_COM_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PLL_TOP_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PLL_GLB_SIG_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PLL_GLB_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PLL_CPU_CTRL0_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PLL_CPU_CTRL1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PLL_CPU_CTRL2_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PLL_CPU_MISC_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PLL_SW_CTRL0_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PLL_SW_CTRL1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PLL_SW_CTRL2_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PLL_SW_MISC_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PLL_LXB_CTRL0_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PLL_LXB_CTRL1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PLL_LXB_CTRL2_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PLL_LXB_MISC_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PLL_PCIE_CTRL0_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PLL_PCIE_CTRL1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PLL_PCIE_CTRL2_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PLL_PCIE_MISC_CTRL_RTL9310_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_PLL___BIAS */
#if defined(CONFIG_SDK_CHIP_FEATURE_BIST___BISR)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG00_RX_MAC01_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG00_RX_MAC05_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG00_RX_MAC_DMY_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG00_TX_MAC01_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG00_TX_MAC_DMY_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG01_RX_MAC01_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG01_RX_MAC05_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG01_RX_MAC_DMY_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG01_TX_MAC01_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG01_TX_MAC_DMY_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG02_RX_MAC01_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG02_RX_MAC05_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG02_RX_MAC_DMY_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG02_TX_MAC01_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG02_TX_MAC_DMY_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG03_RX_MAC01_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG03_RX_MAC05_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG03_RX_MAC_DMY_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG03_TX_MAC01_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG03_TX_MAC_DMY_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG04_RX_MAC01_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG04_RX_MAC05_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG04_RX_MAC_DMY_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG04_TX_MAC01_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG04_TX_MAC_DMY_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG05_RX_MAC01_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG05_RX_MAC05_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG05_RX_MAC_DMY_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG05_TX_MAC01_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG05_TX_MAC_DMY_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG06_RX_MAC08_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG06_RX_MAC04_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG06_RX_MAC06_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG06_RX_MAC06_DMY0_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG06_RX_MAC06_DMY1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG06_TX_MAC08_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG06_TX_MAC02_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG07_RX_MAC08_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG07_RX_MAC04_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG07_RX_MAC06_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG07_RX_MAC06_DMY0_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG07_RX_MAC06_DMY1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG07_TX_MAC08_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG07_TX_MAC02_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG08_RX_MAC10_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG08_RX_MAC07_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG08_RX_MAC11_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG08_RX_MAC_DMY_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG08_TX_MAC10_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG08_TX_MAC12_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG09_RX_MAC10_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG09_RX_MAC07_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG09_RX_MAC11_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG09_RX_MAC_DMY_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG09_TX_MAC10_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG09_TX_MAC12_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG10_RX_MAC10_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG10_RX_MAC07_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG10_RX_MAC11_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG10_RX_MAC_DMY_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG10_TX_MAC10_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG10_TX_MAC12_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG11_RX_MAC10_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG11_RX_MAC07_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG11_RX_MAC11_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG11_RX_MAC_DMY_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG11_TX_MAC10_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG11_TX_MAC12_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG12_RX_MAC03_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG12_RX_MAC09_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG12_RX_MAC07_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG12_RX_MAC_DMY0_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG12_RX_MAC_DMY1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG12_TX_MAC02_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG12_PG_MAC13_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PG12_PG_MAC14_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_MAC_RESET_00_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_MAC_RESET_01_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_MAC_RESET_02_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_MAC_RESUME_00_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_MAC_RESUME_01_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_MAC_RESUME_02_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ALE_START_0_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ALE_START_1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ALE_RESUME_0_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ALE_RESUME_1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ALE_GLB_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ALE01_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ALE02_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ALE03_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ALE04_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ALE05_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ALE06_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ALE07_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ALE08_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ALE09_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ALE10_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ALE011_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ALE012_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ALE13_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ALE14_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ALE15_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ALE16_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ALE17_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ALE17_DLY_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ALE18_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ALE18_DLY_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ALE19_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ALE19_DLY_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ALE20_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ALE20_DLY_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ALE21_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ALE21_DLY_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ALE22_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ALE22_DLY_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ALE23_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ALE23_DLY_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ALE24_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ALE24_DLY_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ALE25_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ALE25_DLY_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ALE26_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ALE26_DLY_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ALE27_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ALE27_DLY_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ALE28_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ALE28_DLY_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ALE29_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ALE29_DLY_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ALE30_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ALE30_DLY_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ALE31_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ALE31_DLY_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ALE32_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ALE32_DLY_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ALE33_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ALE34_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ALE35_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ALE36_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ALE37_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ALE38_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ALE39_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBISR_ALE_START_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBISR_ALE_RESUME_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBISR_ALE01_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBISR_ALE01_OUT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBISR_ALE02_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBISR_ALE02_OUT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBISR_ALE03_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBISR_ALE03_OUT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBISR_ALE04_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBISR_ALE04_OUT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBISR_ALE05_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBISR_ALE05_OUT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBISR_ALE06_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBISR_ALE06_OUT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBISR_ALE07_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBISR_ALE07_OUT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBISR_ALE08_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBISR_ALE08_OUT_0_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBISR_ALE08_OUT_1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBISR_ALE09_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBISR_ALE09_OUT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBISR_ALE10_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBISR_ALE10_OUT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBISR_ALE11_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBISR_ALE11_OUT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ACL_START_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ACL_RESUME_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ACL_GLB_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_TCM00_ACL1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ACL00_DLY_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_TCM01_ACL1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ACL01_DLY_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_TCM02_ACL1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ACL02_DLY_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_TCM03_ACL1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ACL03_DLY_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_TCM04_ACL1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ACL04_DLY_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_TCM05_ACL1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ACL05_DLY_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_TCM06_ACL1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ACL06_DLY_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_TCM07_ACL1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ACL07_DLY_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_TCM08_ACL1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ACL08_DLY_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_TCM09_ACL1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ACL09_DLY_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_TCM10_ACL1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ACL10_DLY_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_TCM11_ACL1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ACL11_DLY_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_TCM12_ACL1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ACL12_DLY_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_TCM13_ACL1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ACL13_DLY_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_TCM14_ACL1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ACL14_DLY_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_TCM15_ACL1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ACL15_DLY_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_LOG00_ACL2_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_LOG01_ACL2_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_LOG02_ACL2_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_LOG03_ACL2_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ACT00_ACL3_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ACT01_ACL3_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ACT02_ACL3_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ACT03_ACL3_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ACT04_ACL3_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ACT05_ACL3_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ACT06_ACL3_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ACT07_ACL3_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MEM_ALE_INIT_0_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MEM_ALE_INIT_1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MEM_ALE_INIT_2_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t GLB_MBIST_CTRL_DMY_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t GLB_MBIST_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t GLB_MBIST_STA_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_MIB_START_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_MIB_RESUME_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_MIB_GLB_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_MIB00_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_MIB01_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_MIB02_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ENCAP_START_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ENCAP_RESUME_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ENCAP_GLB_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ENCAP01_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ENCAP02_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ENCAP03_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ENCAP04_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ENCAP05_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ENCAP06_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ENCAP07_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ENCAP08_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ENCAP09_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ENCAP10_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ENCAP11_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ENCAP12_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ENCAP13_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBISR_ENCAP01_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBISR_ENCAP02_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBISR_ENCAP_OUT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ENCAP_TCAM_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_ENCAP_DLY_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_EGR_START_0_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_EGR_START_1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_EGR_START_2_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_EGR_RESUME_0_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_EGR_RESUME_1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_EGR_RESUME_2_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_EGR_GLB_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_EGR01_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_EGR02_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_EGR03_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_EGR04_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_EGR05_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_EGR06_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_EGR07_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_EGR08_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_EGR09_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_EGR10_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_EGR11_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_EGR12_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_EGR13_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_EGR14_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_EGR15_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_EGR16_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_EGR18_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_EGR19_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_EGR20_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBISR_EXA_EGR_OUT_0_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBISR_EXA_EGR_OUT_1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBISR_EGR_OUT_0_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBISR_EGR_OUT_1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MEM_EGR_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PAR_START_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PAR_RESUME_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PAR_GLB_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PAR01_0_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PAR01_1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PAR02_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_IGR_START_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_IGR_RESUME_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_IGR_GLB_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBISR_IGR_RESULT_01_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBISR_IGR_RESULT_23_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_IGR01_0_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_IGR01_1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_IGR02_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_IGR03_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_IGR04_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_IGR05_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_IGR06_0_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_IGR06_1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_IGR06_2_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_IGR06_3_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_IGR06_4_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_IGR07_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MEM_IGR_INIT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MEM_ENCAP_INIT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MEM_EGR_INIT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MEM_MIB_INIT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MEM_ACL_INIT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_PAR03_RTL9310_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_BIST___BISR */
#if defined(CONFIG_SDK_CHIP_FEATURE_INTERFACE)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t I2C_SLV_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t I2C_MST_IF_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t I2C_MST_IF_SEL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t I2C_MST1_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t I2C_MST1_MEMADDR_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t I2C_MST1_DATA_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t I2C_MST2_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t I2C_MST2_MEMADDR_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t I2C_MST2_DATA_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SPI_CTRL0_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SPI_CTRL1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SPI_CTRL2_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SPI_DATA_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SPI_ADDR_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MAC_SLV_TIMEOUT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t TEST_MDX_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t TEST_MDX_DATA_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t GPIO_DRV_STRENGTH_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t GPIO_SLEW_RATE_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t IO_DRV_STRENGTH_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t IO_SLEW_RATE_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t IO_SMIT_TRIG_RTL9310_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_INTERFACE */
#if defined(CONFIG_SDK_CHIP_FEATURE_LED)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t LED_GLB_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t LED_PORT_NUM_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t LED_SET_3_CTRL1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t LED_SET_3_CTRL0_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t LED_SET_2_CTRL1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t LED_SET_2_CTRL0_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t LED_SET_1_CTRL1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t LED_SET_1_CTRL0_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t LED_SET_0_CTRL1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t LED_SET_0_CTRL0_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t LED_PORT_COPR_SET_SEL_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t LED_PORT_FIB_SET_SEL_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t LED_PORT_COPR_MASK_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t LED_PORT_FIB_MASK_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t LED_PORT_COMBO_MASK_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SW_LED_LOAD_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t LED_PORT_SW_EN_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t LED_PORT_SW_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t LED_INDRT_ACCESS_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t LED_LOAD_LV1_10G_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t LED_LOAD_LV2_10G_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t LED_LOAD_LV3_10G_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t LED_LOAD_LV1_5G_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t LED_LOAD_LV2_5G_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t LED_LOAD_LV3_5G_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t LED_LOAD_LV1_2P5G_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t LED_LOAD_LV2_2P5G_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t LED_LOAD_LV3_2P5G_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t LED_LOAD_LV1_TP1G_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t LED_LOAD_LV2_TP1G_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t LED_LOAD_LV3_TP1G_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t LED_LOAD_LV1_1G_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t LED_LOAD_LV2_1G_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t LED_LOAD_LV3_1G_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t LED_LOAD_LV1_500M_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t LED_LOAD_LV2_500M_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t LED_LOAD_LV3_500M_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t LED_LOAD_LV1_100M_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t LED_LOAD_LV2_100M_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t LED_LOAD_LV3_100M_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t LED_LOAD_LV1_10M_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t LED_LOAD_LV2_10M_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t LED_LOAD_LV3_10M_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t LED_P_LOAD_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EXT_GPIO_GLB_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EXT_GPIO_TRIG_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EXT_GPIO_DIR_CTRL_1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EXT_GPIO_DIR_CTRL_2_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EXT_GPIO_DIR_CTRL_3_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EXT_GPIO_DATA_CTRL_1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EXT_GPIO_DATA_CTRL_2_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EXT_GPIO_DATA_CTRL_3_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EXT_GPIO_INDRT_ACCESS_CTRL_RTL9310_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_LED */
#if defined(CONFIG_SDK_CHIP_FEATURE_INTERRUPT)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t IMR_PORT_LINK_STS_CHG_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t IMR_SERDES_ERR_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t IMR_REMOTE_INTR_STS_UPD_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t IMR_MISC_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t IMR_EXT_GPIO_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t IMR_THERMAL_METER_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t IMR_AUTO_RECOVERY_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t IMR_SERDES_UPD_PHYSTS_0_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t IMR_RLFD_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t IMR_SERDES_RXIDLE_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t IMR_DBGO_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t DBG_CTRL0_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t DBG_CTRL1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t DBG_CTRL2_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ISR_GLB_SRC_STS_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ISR_PORT_LINK_STS_CHG_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ISR_SERDES_ERR_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ISR_REMOTE_INTR_STS_UPD_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ISR_MISC_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ISR_EXT_GPIO_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EXT_GPIO_INTR_MODE_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ISR_THERMAL_METER_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ISR_AUTO_RECOVERY_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ISR_SERDES_UPD_PHYSTS_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ISR_STAT_TRIGGER_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ISR_RLFD_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ISR_SERDES_RXIDLE_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ISR_DBGO_RTL9310_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_INTERRUPT */
#if defined(CONFIG_SDK_CHIP_FEATURE_HW_MISC_)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EFUSE_ACCESS_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EFUSE_WDATA_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EFUSE_RDATA_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SPD_SENR_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SPD_DATA_IN_0_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SPD_DATA_IN_1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SPD_DATA_IN_2_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SPD_DATA_OUT_0_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SPD_DATA_OUT_1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SPD_DATA_OUT_2_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SPD_DBG_OUT_0_1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SPD_DBG_OUT_2_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t VOL_CTRL_RESIS_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t TM0_CTRL0_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t TM0_CTRL1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t TM0_CTRL2_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t TM0_CTRL3_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t TM0_CTRL4_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t TM0_CTRL5_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t TM0_CTRL6_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t TM0_CTRL7_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t TM1_CTRL0_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t TM1_CTRL1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t TM1_CTRL2_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t TM1_CTRL3_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t TM1_CTRL4_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t TM1_CTRL5_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t TM1_CTRL6_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t TM1_CTRL7_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t TM0_RESULT0_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t TM0_RESULT1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t TM0_RESULT2_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t TM0_RESULT3_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t TM1_RESULT0_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t TM1_RESULT1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t TM1_RESULT2_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t TM1_RESULT3_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t GLB_DEBUG_SELECT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t GLB_DEBUG_DATA_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t GLB_DEBUG_ENABLE_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t GLB_DEBUG_PIN_EN_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t VOLT_PROB_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EFUSE_CHK_RSLT_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EFUSE_RIR0_DATA_CTRL_RSVD_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EFUSE_RIR1_DATA_CTRL_RSVD_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t GLB_DEBUG_DATA_VALUE_RTL9310_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_HW_MISC_ */
#if defined(CONFIG_SDK_CHIP_FEATURE_NIC___DMA)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t DMA_IF_RX_BASE_DESC_ADDR_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t DMA_IF_RX_CUR_DESC_ADDR_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t DMA_IF_TX_BASE_DESC_ADDR_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t DMA_IF_TX_CUR_DESC_ADDR_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t DMA_IF_INTR_RX_RUNOUT_MSK_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t DMA_IF_INTR_RX_DONE_MSK_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t DMA_IF_INTR_TX_DONE_MSK_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t DMA_IF_INTR_RX_RUNOUT_STS_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t DMA_IF_INTR_RX_DONE_STS_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t DMA_IF_INTR_TX_DONE_STS_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t DMA_IF_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t DMA_IF_PKT_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t DMA_IF_RX_RING_SIZE_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t DMA_IF_RX_RING_CNTR_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t DMA_IF_PHYSICAL_ADDR_MSK_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_NIC_W_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_NIC_R_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t NIC_RING_DUMMYREAD_EN_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t NIC_RDMA_CNT_SEL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t NICDBG_SEL_0_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t NICDBG_SEL_1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t NICDBG_SEL_2_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t NICDBG_SEL_3_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t NICDBG_SEL_4_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t NICDBG_SEL_5_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t NICDBG_RD_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t NICDBG_TEST_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t DMA_IF_RX_RING_CNTR_FLAG_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t NIC_DBGMUX_SEL_0_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t NIC_DBGMUX_SEL_1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t NIC_DBGMUX_SEL_2_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t NIC_DBGMUX_SEL_3_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t NIC_DRIVER_READY_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t NIC_DBG_DT_0_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t NIC_DBG_DT_1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t NIC_DBG_DT_2_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t NIC_DBG_DT_3_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t NIC_DBG_DT_4_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t NIC_DBG_DT_5_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t NIC_DBG_DT_6_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t NIC_DBG_DT_7_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t NIC_DBG_OUT_CUR_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t NIC_DBG_OUT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t NIC_RX_SOP_CNT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t NIC_RX_EOP_CNT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t NIC_TX_SOP_CNT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t NIC_TX_EOP_CNT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t NIC_L2MSG_CNT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t NIC_CNT_CLR_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t NIC_RX_TX_FIFO_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t NIC_L2MSG_FIFO_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t NIC_PRST_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t NIC_FRC_NIC_DBGO_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t NIC_FRCV_NIC_DBGO_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t NIC_RX_SOP_CNT_LX_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t NIC_RX_EOP_CNT_LX_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t NIC_TX_SOP_CNT_LX_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t NIC_TX_EOP_CNT_LX_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t NIC_L2MSG_CNT_LX_RTL9310_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_NIC___DMA */
#if defined(CONFIG_SDK_CHIP_FEATURE_APPLICATION_TRAP)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t TRAP_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t TRAP_ARP_GRAT_PORT_ACT_RTL9310_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_APPLICATION_TRAP */
#if defined(CONFIG_SDK_CHIP_FEATURE_TABLE_ACCESS)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t TBL_ACCESS_CTRL_0_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t TBL_ACCESS_L2_METHOD_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t TBL_ACCESS_DATA_0_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t TBL_ACCESS_CTRL_1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t TBL_ACCESS_DATA_1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t TBL_ACCESS_CTRL_2_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t TBL_ACCESS_DATA_2_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t TBL_ACCESS_CTRL_3_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t TBL_ACCESS_DATA_3_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t TBL_ACCESS_CTRL_4_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t TBL_ACCESS_DATA_4_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t TBL_ACCESS_CTRL_5_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t TBL_ACCESS_DATA_5_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t FLEX_TBL_CTRL_RTL9310_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_TABLE_ACCESS */
#if defined(CONFIG_SDK_CHIP_FEATURE_MAC_CONTROL)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MAC_L2_GLOBAL_CTRL1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MAC_L2_GLOBAL_CTRL2_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MAC_L2_PORT_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MAC_L2_PADDING_SEL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MAC_L2_ADDR_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MAC_L2_CPU_CRC_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SMI_INDRT_ACCESS_CTRL_0_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SMI_INDRT_ACCESS_CTRL_1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SMI_INDRT_ACCESS_CTRL_2_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SMI_INDRT_ACCESS_CTRL_3_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SMI_INDRT_ACCESS_BC_PHYID_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SMI_INDRT_ACCESS_MMD_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SMI_DBG_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MDXDBG_SEL0_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MDXDBG_SEL1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MDXDBG_SEL2_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MDXDBG_SEL3_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MDXDBG_SEL4_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MDXDBG_SEL5_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MDXDBG_TSET_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MDXDBG_READ_DATA_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MDX_DBGMUX_SEL_1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MDX_DBGMUX_SEL_2_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MDX_DBGMUX_SEL_3_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MDX_DBG_DT_0_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MDX_DBG_DT_1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MDX_DBG_DT_2_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MDX_DBG_DT_3_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MDX_DBG_DT_4_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MDX_DBG_DT_5_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MDX_DBG_DT_6_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MDX_DBG_DT_7_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MDX_DBG_OUT_CUR_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MDX_DBG_OUT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MAC_GLB_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MAC_PORT_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MAC_CPU_TAG_ID_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t HALF_CHG_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SMI_PORT_ADDR_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SMI_PORT_POLLING_SEL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SMI_PHY_ABLTY_GET_SEL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SMI_GLB_CTRL1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SMI_GLB_CTRL0_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SMI_BYPASS_ABLTY_LOCK_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SMI_PORT_POLLING_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t TX_IDLE_TMR_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t LINK_DELAY_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MDIO_FREE_CNT_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SMI_GPHY_RLFD_POLLING_SEL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SMI_RLFD_POLLING_BIT_SEL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SMI_10GPHY_RLFD_POLLING_SEL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SMI_GPHY_POLLING_SEL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SMI_10GPHY_POLLING_SEL0_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SMI_10GPHY_POLLING_SEL1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SMI_10GPHY_POLLING_SEL2_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SMI_10GPHY_POLLING_SEL3_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SMI_10GPHY_POLLING_SEL4_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SMI_PHY_REG_CHK_CHK0_MSK_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SMI_PHY_REG_CHK_CHK1_MSK_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SMI_PHY_REG_CHK_CHK2_MSK_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SMI_PHY_REG_CHK_CHK3_MSK_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SMI_PHY_REG_CHK_CHK4_MSK_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SMI_PHY_REG_CHK0_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SMI_PHY_REG_CHK0_MMD_ADDR_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SMI_PHY_REG_CHK1_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SMI_PHY_REG_CHK1_MMD_ADDR_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SMI_PHY_REG_CHK2_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SMI_PHY_REG_CHK2_MMD_ADDR_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SMI_PHY_REG_CHK3_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SMI_PHY_REG_CHK3_MMD_ADDR_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SMI_PHY_REG_CHK4_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SMI_PHY_REG_CHK4_MMD_ADDR_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SMI_PHY_REG_CHK0_DATA_GPHY_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SMI_PHY_REG_CHK1_DATA_GPHY_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SMI_PHY_REG_CHK2_DATA_GPHY_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SMI_PHY_REG_CHK3_DATA_GPHY_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SMI_PHY_REG_CHK4_DATA_GPHY_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SMI_PHY_REG_CHK0_DATA_10GPHY_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SMI_PHY_REG_CHK1_DATA_10GPHY_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SMI_PHY_REG_CHK2_DATA_10GPHY_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SMI_PHY_REG_CHK3_DATA_10GPHY_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SMI_PHY_REG_CHK4_DATA_10GPHY_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SMI_PHY_REG_CHK0_RESULT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SMI_PHY_REG_CHK1_RESULT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SMI_PHY_REG_CHK2_RESULT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SMI_PHY_REG_CHK3_RESULT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SMI_PHY_REG_CHK4_RESULT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SMI_PHY_REG_CHK0_RESULT_ERR_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SMI_PHY_REG_CHK1_RESULT_ERR_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SMI_PHY_REG_CHK2_RESULT_ERR_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SMI_PHY_REG_CHK3_RESULT_ERR_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SMI_PHY_REG_CHK4_RESULT_ERR_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MAC_FORCE_MODE_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MAC_RLFD_EN_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MAC_RLFD_STS_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MAC_LINK_STS_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MAC_LINK_MEDIA_STS_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MAC_LINK_SPD_STS_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MAC_LINK_DUP_STS_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MAC_TX_PAUSE_STS_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MAC_RX_PAUSE_STS_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MAC_EEE_ABLTY_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MAC_MSTR_SLV_STS_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t FEFI_STS_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t FORCE_MAC_SPD_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SPD_FORCE_VAL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MAC_L2_PORT_MAX_LEN_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MAC_L2_CPU_MAX_LEN_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MAC_SERDES_MODE_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MAC_GROUP0_1_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MAC_GROUP2_3_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MAC_GROUP4_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MAC_GROUP5_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MAC_GROUP6_7_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MAC_GROUP8_11_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PROT_SERDSE_MUX_CTRL_0_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PROT_SERDSE_MUX_CTRL_1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t FIB_UNIDIR_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MAC_DBG_SEL_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SDS_ABLTY_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RXDV_H_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RXDV_L_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t TX_IDLE_H_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t TX_IDLE_L_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PHY_UNIDIR_H_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PHY_UNIDIR_L_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MACRX_DUPDET_H_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MACRX_DUPDET_L_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t FRC_RXDV_H_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t FRC_RXDV_L_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t FRC_TX_IDLE_H_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t FRC_TX_IDLE_L_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t FRC_PHY_UNIDIR_H_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t FRC_PHY_UNIDIR_L_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t FRC_MACRX_DUPDET_H_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t FRC_MACRX_DUPDET_L_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t FRCV_RXDV_H_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t FRCV_RXDV_L_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t FRCV_TX_IDLE_H_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t FRCV_TX_IDLE_L_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t FRCV_PHY_UNIDIR_H_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t FRCV_PHY_UNIDIR_L_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t FRCV_MACRX_DUPDET_H_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t FRCV_MACRX_DUPDET_L_RTL9310_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_MAC_CONTROL */
#if defined(CONFIG_SDK_CHIP_FEATURE_PHY___SERDES)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SERDES_MODE_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SERDES_INDRT_ACCESS_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SERDES_INDRT_DATA_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SERDES_BC_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SERDES_UNIDIR_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SERDES_MISC_CTRL0_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SERDES_MISC_CTRL1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t USXGMII_SUBMODE_CTRL_RTL9310_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_PHY___SERDES */
#if defined(CONFIG_SDK_CHIP_FEATURE_POWER_SAVING)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EEE_PORT_TX_EN_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EEE_PORT_RX_EN_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EEE_MISC_CTRL0_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EEE_MISC_CTRL1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EEE_TX_MINIFG_CTRL0_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EEE_TX_MINIFG_CTRL1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EEE_TX_MINIFG_CTRL2_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EEE_WAIT_RX_INACTIVE_CTRL0_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EEE_WAIT_RX_INACTIVE_CTRL1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EEE_MULTIWAKE_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EEE_TX_TIMER_100M_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EEE_TX_TIMER_GELITE_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EEE_TX_TIMER_GIGA_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EEE_TX_TIMER_2P5G_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EEE_TX_TIMER_5G_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EEE_TX_TIMER_10G_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EEE_PORT_TX_STS_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EEE_PORT_RX_STS_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EEEP_PORT_TX_EN_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EEEP_PORT_RX_EN_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EEEP_PORT_1000M_EN_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EEEP_PORT_500M_EN_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EEEP_PORT_100M_EN_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EEEP_SLV_EN_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EEEP_TIMER_UNIT_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EEEP_TX_WAKE_TIMER_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EEEP_TX_IDLE_TIMER_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EEEP_TX_RATE_100M_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EEEP_TX_RATE_500M_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EEEP_TX_RATE_GIGA_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EEEP_RX_SLEEP_STEP_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EEEP_RX_IDLE_TIMER_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EEEP_RX_TIMER_100M_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EEEP_RX_TIMER_500M_CTRL0_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EEEP_RX_TIMER_500M_CTRL1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EEEP_RX_TIMER_GIGA_CTRL0_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EEEP_RX_TIMER_GIGA_CTRL1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EEEP_RX_RATE_100M_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EEEP_RX_RATE_500M_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EEEP_RX_RATE_GIGA_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PS_SLOW_SYSCLK_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PS_SERDES_OFF_MODE_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PS_SOC_CTRL_RTL9310_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_POWER_SAVING */
#if defined(CONFIG_SDK_CHIP_FEATURE_802_1Q_VLAN___QINQ)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t VLAN_TAG_TPID_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t VLAN_ETAG_TPID_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t VLAN_PORT_OTAG_TPID_CMP_MSK_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t VLAN_PORT_ITAG_TPID_CMP_MSK_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t VLAN_PORT_ETAG_TPID_CMP_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t VLAN_PORT_AFT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t VLAN_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t VLAN_PORT_IGR_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t VLAN_PORT_FWD_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t VLAN_APP_PKT_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t VLAN_PORT_IGR_FLTR_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t VLAN_PORT_EGR_FLTR_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t VLAN_PORT_TAG_CTRL_RTL9310_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_802_1Q_VLAN___QINQ */
#if defined(CONFIG_SDK_CHIP_FEATURE_VLAN_PROFILE)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t VLAN_PROFILE_SET_RTL9310_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_VLAN_PROFILE */
#if defined(CONFIG_SDK_CHIP_FEATURE__IEEE802_1V__PROTOCOL_BASED_VLAN)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t VLAN_PPB_VLAN_SET_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t VLAN_PORT_PPB_VLAN_SET_RTL9310_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE__IEEE802_1V__PROTOCOL_BASED_VLAN */
#if defined(CONFIG_SDK_CHIP_FEATURE_VLAN_RANGE_CHECK)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t VLAN_IGR_VID_RNG_CHK_SET_0_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t VLAN_IGR_VID_RNG_CHK_SET_1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t VLAN_IGR_VID_RNG_CHK_SET_2_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t VLAN_IGR_VID_RNG_CHK_SET_3_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t VLAN_EGR_VID_RNG_CHK_SET_0_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t VLAN_EGR_VID_RNG_CHK_SET_1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t VLAN_EGR_VID_RNG_CHK_SET_2_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t VLAN_EGR_VID_RNG_CHK_SET_3_RTL9310_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_VLAN_RANGE_CHECK */
#if defined(CONFIG_SDK_CHIP_FEATURE_VLAN_TRANSLATION)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t VLAN_IVC_BLK_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t VLAN_L2TBL_CNVT_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t VLAN_PORT_L2TBL_CNVT_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t VLAN_TRUNK_L2TBL_CNVT_CTRL_3_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t VLAN_TRUNK_L2TBL_CNVT_CTRL_2_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t VLAN_TRUNK_L2TBL_CNVT_CTRL_1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t VLAN_TRUNK_L2TBL_CNVT_CTRL_0_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t VLAN_PORT_IVC_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t VLAN_PORT_EVC_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t VLAN_IVC_ENTRY_INDICATION_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t VLAN_EVC_ENTRY_INDICATION_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t VLAN_IVC_ENTRY_INDICATION_AGGR_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t VLAN_EVC_ENTRY_INDICATION_AGGR_RTL9310_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_VLAN_TRANSLATION */
#if defined(CONFIG_SDK_CHIP_FEATURE_RMA)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RMA_CTRL_0_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RMA_CTRL_1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RMA_CTRL_2_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RMA_MIRROR_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RMA_SMAC_LRN_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RMA_MGN_LRN_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RMA_PORT_BPDU_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RMA_PORT_PTP_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RMA_PORT_LLDP_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RMA_PORT_EAPOL_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RMA_FLD_PMSK_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RMA_BPDU_FLD_PMSK_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RMA_EAPOL_FLD_PMSK_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RMA_LLDP_FLD_PMSK_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RMA_USR_DEF_FLD_PMSK_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RMA_USR_DEF_CTRL_RTL9310_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_RMA */
#if defined(CONFIG_SDK_CHIP_FEATURE_LINK_AGGREGATION)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t TRK_ID_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t TRK_MBR_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t TRK_HASH_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t TRK_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t TRK_SHFT_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t TRK_LOCAL_TBL_REFRESH_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t TRK_LOCAL_TBL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t TRK_STK_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t LOCAL_PORT_TRK_MAP_RTL9310_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_LINK_AGGREGATION */
#if defined(CONFIG_SDK_CHIP_FEATURE_SPANNING_TREE)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ST_CTRL_RTL9310_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_SPANNING_TREE */
#if defined(CONFIG_SDK_CHIP_FEATURE_PORT_ISOLATION)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PORT_ISO_RESTRICT_ROUTE_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PORT_ISO_VB_ISO_PMSK_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PORT_ISO_VB_EGR_CTRL_RTL9310_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_PORT_ISOLATION */
#if defined(CONFIG_SDK_CHIP_FEATURE_MAC_FORWARDING_CONTROL)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L2_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L2_AGE_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L2_PORT_AGE_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L2_TRK_AGE_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L2_PORT_SALRN_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L2_PORT_NEW_SA_FWD_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L2_PORT_DYN_MV_ACT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L2_PORT_DYN_MV_LRN_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L2_PORT_STT_MV_ACT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L2_PORT_STT_MV_LRN_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L2_TRK_STT_MV_ACT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L2_TRK_STT_MV_LRN_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L2_GLB_STT_PORT_MV_ACT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L2_GLB_STT_PORT_MV_LRN_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L2_PORT_MV_FORBID_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L2_TRK_MV_FORBID_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L2_PORT_MV_FORBID_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L2_PORT_SABLK_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L2_PORT_DABLK_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L2_UNKN_UC_FLD_PMSK_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L2_BC_FLD_PMSK_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L2_PORT_UC_LM_ACT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L2_PORT_L2_MC_LM_ACT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L2_PORT_IP4_MC_LM_ACT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L2_PORT_IP6_MC_LM_ACT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L2_LRN_CONSTRT_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L2_LRN_CONSTRT_CNT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L2_LRN_PORT_CONSTRT_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L2_LRN_PORT_CONSTRT_CNT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L2_LRN_TRK_CONSTRT_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L2_LRN_TRK_CONSTRT_CNT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L2_LRN_VLAN_CONSTRT_ENTRY_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L2_LRN_VLAN_CONSTRT_CNT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L2_VLAN_CONSTRT_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L2_CONSTRT_PORT_CNT_DBG_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L2_CONSTRT_TRK_CNT_DBG_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L2_CONSTRT_SYS_CNT_DBG_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L2_CONSTRT_VLAN_CNT_DBG_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L2_TBL_FLUSH_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L2_SRC_P_FLTR_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L2_SA_ACT_REF_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L2_HASH_FULL_CNT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L2_CAM_ENTRY_STS_RTL9310_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_MAC_FORWARDING_CONTROL */
#if defined(CONFIG_SDK_CHIP_FEATURE_L2_ENTRY_NOTIFICATION)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L2_NTFY_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L2_NTFY_PKT_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L2_NTFY_PKT_TIMEOUT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L2_NTFY_PKT_MAC_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L2_NTFY_PKT_ITAG_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L2_NTFY_RST_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L2_NTFY_PKT_MAGIC_NUM_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L2_NTFY_RING_BASE_ADDR_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L2_NTFY_RING_CUR_ADDR_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L2_NTFY_IF_INTR_MSK_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L2_NTFY_IF_INTR_STS_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L2_NTFY_NIC_FIFO_STS_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L2_NTFY_PKT_FIFO_STS_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L2_NTFY_NIC_TIMEOUT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L2_NTFY_PKT_QUE_ID_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L2_NTFY_NIC_MSG_CNT_SEL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L2_NTFY_DBG_PKT_CNT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L2_NTFY_DBG_EVENT_CNT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L2_NTFY_BP_STS_RTL9310_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_L2_ENTRY_NOTIFICATION */
#if defined(CONFIG_SDK_CHIP_FEATURE_STORM_CONTROL)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STORM_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STORM_LB_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STORM_LB_PPS_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STORM_LB_PROTO_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STORM_PORT_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STORM_PORT_UC_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STORM_PORT_UC_LB_RST_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STORM_PORT_UC_EXCEED_FLAG_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STORM_PORT_MC_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STORM_PORT_MC_LB_RST_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STORM_PORT_MC_EXCEED_FLAG_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STORM_PORT_BC_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STORM_PORT_BC_LB_RST_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STORM_PORT_BC_EXCEED_FLAG_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STORM_PORT_PROTO_DHCP_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STORM_PORT_PROTO_DHCP_LB_RST_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STORM_PORT_PROTO_DHCP_EXCEED_FLAG_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STORM_PORT_PROTO_BPDU_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STORM_PORT_PROTO_BPDU_LB_RST_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STORM_PORT_PROTO_BPDU_EXCEED_FLAG_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STORM_PORT_PROTO_IGMP_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STORM_PORT_PROTO_IGMP_LB_RST_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STORM_PORT_PROTO_IGMP_EXCEED_FLAG_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STORM_PORT_PROTO_ARP_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STORM_PORT_PROTO_ARP_LB_RST_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STORM_PORT_PROTO_ARP_EXCEED_FLAG_RTL9310_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_STORM_CONTROL */
#if defined(CONFIG_SDK_CHIP_FEATURE_INGRESS_BANDWIDTH_CONTROL)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t IGBW_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t IGBW_LB_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t IGBW_PORT_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t IGBW_PORT_LB_RST_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t IGBW_PORT_EXCEED_FLAG_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t IGBW_PORT_FC_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t IGBW_Q_DROP_THR_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t IGBW_PORT_Q_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t IGBW_PORT_Q_LB_RST_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t IGBW_PORT_Q_LB_EXCEED_FLAG_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t IGBW_PORT_SCHED_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t IGBW_RATE_10M_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t IGBW_RATE_100M_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t IGBW_RATE_500M_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t IGBW_RATE_1G_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t IGBW_RATE_1250M_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t IGBW_RATE_2500M_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t IGBW_RATE_5G_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t IGBW_RATE_10G_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t IGBW_PAGE_CNT_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t IGBW_Q_PAGE_CNT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t IGBW_ADMIT_Q_PAGE_CNT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t IGBW_DROP_Q_PAGE_CNT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t IGBW_WFQ_LB_CTRL_RTL9310_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_INGRESS_BANDWIDTH_CONTROL */
#if defined(CONFIG_SDK_CHIP_FEATURE_EGRESS_BANDWIDTH_CONTROL)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EGBW_ENCAP_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EGBW_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EGBW_CPU_PPS_LB_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EGBW_LB_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EGBW_PORT_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EGBW_PORT_LB_RST_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EGBW_PORT_Q_MAX_LB_RST_SET0_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EGBW_PORT_Q_MAX_LB_RST_SET1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EGBW_CPU_Q_MAX_LB_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EGBW_CPU_Q_MAX_LB_RST_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EGBW_PORT_Q_ASSURED_LB_RST_SET0_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EGBW_PORT_Q_ASSURED_LB_RST_SET1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EGBW_RATE_10M_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EGBW_RATE_100M_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EGBW_RATE_500M_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EGBW_RATE_1G_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EGBW_RATE_1250M_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EGBW_RATE_2500M_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EGBW_RATE_5G_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EGBW_RATE_10G_CTRL_RTL9310_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_EGRESS_BANDWIDTH_CONTROL */
#if defined(CONFIG_SDK_CHIP_FEATURE_METER_MARKER)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t METER_GLB_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t METER_LB_EXCEED_STS_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t METER_LB_GLB_EXCEED_STS_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t METER_CNTR_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t METER_GREEN_CNTR_STS_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t METER_YELLOW_CNTR_STS_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t METER_RED_CNTR_STS_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t METER_TOTAL_CNTR_STS_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t METER_BYTE_TB_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t METER_PKT_TB_CTRL_RTL9310_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_METER_MARKER */
#if defined(CONFIG_SDK_CHIP_FEATURE_FLOWCONTROL___BACKPRESSURE)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t FC_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t FC_PORT_ACT_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t FC_GLB_SYS_UTIL_THR_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t FC_GLB_DROP_THR_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t FC_GLB_HI_THR_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t FC_GLB_LO_THR_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t FC_GLB_FCOFF_HI_THR_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t FC_GLB_FCOFF_LO_THR_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t FC_JUMBO_HI_THR_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t FC_JUMBO_LO_THR_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t FC_JUMBO_FCOFF_HI_THR_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t FC_JUMBO_FCOFF_LO_THR_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t FC_JUMBO_THR_ADJUST_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t FC_PORT_HI_THR_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t FC_PORT_LO_THR_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t FC_PORT_FCOFF_HI_THR_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t FC_PORT_FCOFF_LO_THR_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t FC_PORT_GUAR_THR_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t FC_PORT_THR_SET_SEL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t FC_PORT_EGR_DROP_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t FC_HOL_PRVNT_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t FC_PORT_Q_EGR_FORCE_DROP_CTRL_SET0_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t FC_PORT_Q_EGR_FORCE_DROP_CTRL_SET1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t FC_CPU_Q_EGR_FORCE_DROP_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t FC_Q_EGR_DROP_THR_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t FC_CPU_Q_EGR_DROP_THR_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t FC_PORT_EGR_DROP_THR_SET_SEL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t FC_LB_PORT_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t FC_LB_PORT_Q_EGR_DROP_THR_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t FC_REPCT_FCOFF_THR_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t FC_GLB_PAGE_CNT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t FC_PB_PAGE_CNT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t FC_PORT_PAGE_CNT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t FC_CPU_EGR_PAGE_CNT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t FC_CPU_Q_EGR_PAGE_CNT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t FC_ALE_NON_REPCT_Q_PAGE_CNT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t FC_ALE_REPCT_Q_PAGE_CNT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t FC_REPCT_FCON_PAGE_CNT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t FC_REPCT_FCOFF_PAGE_CNT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t FC_REPCT_FCOFF_DROP_CNT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t FC_HSA_PAGE_CNT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t FC_DUMMY_PORT_THR_SET_SEL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t FC_DUMMY_PORT_IGR_PAGE_CNT_RTL9310_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_FLOWCONTROL___BACKPRESSURE */
#if defined(CONFIG_SDK_CHIP_FEATURE_CONGESTION_AVOIDANCE)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SWRED_Q_DROP_RATE_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SWRED_Q_THR_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SWRED_DROP_CNTR_PIDX_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SWRED_DROP_CNTR_CIDX_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SWRED_DROP_CNTR_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SWRED_DROP_CNTR_RST_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SC_PORT_TIMER_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SC_DRAIN_OUT_THR_RTL9310_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_CONGESTION_AVOIDANCE */
#if defined(CONFIG_SDK_CHIP_FEATURE_INGRESS_PRIORITY_DECISION)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PRI_SEL_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PRI_SEL_PORT_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PRI_SEL_REMAP_PORT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PRI_SEL_REMAP_IPRI_CFI0_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PRI_SEL_REMAP_IPRI_CFI1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PRI_SEL_REMAP_OPRI_DEI0_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PRI_SEL_REMAP_OPRI_DEI1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PRI_SEL_REMAP_DSCP_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PRI_SEL_REMAP_11E_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PRI_SEL_REMAP_MPLS_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PRI_SEL_REMAP_1BR_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PRI_SEL_PORT_TBL_IDX_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PRI_SEL_TBL_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t DP_SEL_REMAP_ITAG_CFI0_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t DP_SEL_REMAP_ITAG_CFI1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t DP_SEL_REMAP_OTAG_DEI0_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t DP_SEL_REMAP_OTAG_DEI1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t DP_SEL_REMAP_DSCP_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t DP_SEL_REMAP_MPLS_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t DP_SEL_PORT_TBL_CTRL_RTL9310_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_INGRESS_PRIORITY_DECISION */
#if defined(CONFIG_SDK_CHIP_FEATURE_SCHEDULING___QUEUE_MANAGEMENT)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t QM_INTPRI2QID_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t QM_CPUQID2QID_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t QM_CPUQID2XGSQID_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t QM_RSN2CPUQID_CTRL_0_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t QM_RSN2CPUQID_CTRL_1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t QM_RSN2CPUQID_CTRL_2_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t QM_RSN2CPUQID_CTRL_3_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t QM_RSN2CPUQID_CTRL_4_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t QM_RSN2CPUQID_CTRL_5_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t QM_RSN2CPUQID_CTRL_6_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t QM_RSN2CPUQID_CTRL_7_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t QM_RSN2CPUQID_CTRL_8_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t QM_RSN2CPUQID_CTRL_9_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t QM_RSN2CPUQID_CTRL_10_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t QM_FLAG2CPUQID_CTRL_0_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t QM_FLAG2CPUQID_CTRL_1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t QM_FLAG2CPUQID_CTRL_2_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SCHED_PORT_Q_CTRL_SET0_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SCHED_PORT_Q_CTRL_SET1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SCHED_CPU_Q_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SCHED_PORT_ALGO_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SCHED_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t WFQ_LB_CTRL_RTL9310_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_SCHEDULING___QUEUE_MANAGEMENT */
#if defined(CONFIG_SDK_CHIP_FEATURE_REMARKING)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RMK_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RMK_PORT_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RMK_INTPRI2IPRI_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RMK_IPRI2IPRI_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RMK_OPRI2IPRI_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RMK_DSCP2IPRI_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RMK_INTPRI2OPRI_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RMK_IPRI2OPRI_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RMK_OPRI2OPRI_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RMK_DSCP2OPRI_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RMK_INTPRI2DEI_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RMK_DP2DEI_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RMK_INTPRI2DSCP_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RMK_IPRI2DSCP_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RMK_OPRI2DSCP_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RMK_DSCP2DSCP_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RMK_DPINTPRI2DSCP_CTRL_RTL9310_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_REMARKING */
#if defined(CONFIG_SDK_CHIP_FEATURE_802_1QAV)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t AVB_PORT_CLASS_A_EN_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t AVB_PORT_CLASS_B_EN_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t AVB_PORT_CLASS_A_EN_MAC_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t AVB_PORT_CLASS_B_EN_MAC_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t AVB_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t AVB_CTRL_MAC_RTL9310_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_802_1QAV */
#if defined(CONFIG_SDK_CHIP_FEATURE_LAYER_3_ROUTING)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L3_IP_ROUTE_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L3_HOST_TBL_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L3_IPUC_ROUTE_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L3_IP6UC_ROUTE_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L3_IPMC_ROUTE_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L3_IP6MC_ROUTE_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L3_PORT_IP_ROUTE_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L3_PORT_IP6_ROUTE_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L3_INTF_IP_MTU_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L3_INTF_IP6_MTU_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L3_ENTRY_MV_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L3_ENTRY_MV_PARAM_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L3_ECMP_HASH_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L3_MONT_CNTR_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L3_MONT_CNTR_DATA_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L3_HW_LU_KEY_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L3_HW_LU_KEY_SIP_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L3_HW_LU_KEY_DIP_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L3_HW_LU_CTRL_RTL9310_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_LAYER_3_ROUTING */
#if defined(CONFIG_SDK_CHIP_FEATURE_IP_TUNNEL)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t TUNNEL_IP_ADDR_CHK_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t TUNNEL_ROUTE_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t TUNNEL_IP_IDENTIFICATION_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t TUNNEL_6RD_DOMAIN_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t TUNNEL_QOS_PROFILE_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t TUNNEL_VXLAN_CTRL_RTL9310_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_IP_TUNNEL */
#if defined(CONFIG_SDK_CHIP_FEATURE_MPLS_VPLS)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MPLS_GLB_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MPLS_PARSE_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MPLS_ENCAP_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MPLS_DPINTPRI2TC_CTRL_RTL9310_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_MPLS_VPLS */
#if defined(CONFIG_SDK_CHIP_FEATURE_MIB_CONTROL)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_RST_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PORT_RST_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_CNT_SET1_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_CNT_SET0_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t CFG_DMY_MIB_0_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t CFG_DMY_MIB_1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t CFG_DMY_MIB_2_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t CFG_DMY_MIB_3_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t CFG_DMY_MIB_4_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_SRAM_CTRL_RTL9310_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_MIB_CONTROL */
#if defined(CONFIG_SDK_CHIP_FEATURE_MIB_COUNTER)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_BRIDGE_DOT1DTPLEARNEDENTRYDISCARDS_RTL9310_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_MIB_COUNTER */
#if defined(CONFIG_SDK_CHIP_FEATURE_DEBUG_COUNTER)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR0_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR2_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR3_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR4_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR5_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR6_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR7_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR8_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR9_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR10_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR11_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR12_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR13_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR14_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR15_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR16_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR17_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR18_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR19_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR20_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR21_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR22_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR23_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR24_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR25_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR26_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR27_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR28_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR29_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR30_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR31_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR32_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR33_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR34_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR35_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR36_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR37_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR38_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR39_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR40_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR41_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR42_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR43_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR44_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR45_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR46_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR47_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR48_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR49_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR50_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR51_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR52_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR53_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR54_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR55_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR56_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR57_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR58_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR59_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR60_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR61_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR62_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR63_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR64_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR65_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR66_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR67_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR68_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR69_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR70_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR71_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR72_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR73_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR74_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR75_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR76_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR77_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR78_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR79_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_CNTR80_RTL9310_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_DEBUG_COUNTER */
#if defined(CONFIG_SDK_CHIP_FEATURE_MIRRORING)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MIR_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MIR_SPM_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MIR_DPM_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MIR_SAMPLE_RATE_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MIR_QID_CTRL_RTL9310_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_MIRRORING */
#if defined(CONFIG_SDK_CHIP_FEATURE_RSPAN)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MIR_RSPAN_VLAN_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MIR_RSPAN_TX_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MIR_RSPAN_RX_TAG_RM_CTRL_RTL9310_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_RSPAN */
#if defined(CONFIG_SDK_CHIP_FEATURE_SFLOW)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SFLOW_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SFLOW_PORT_RATE_CTRL_RTL9310_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_SFLOW */
#if defined(CONFIG_SDK_CHIP_FEATURE_PIE)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PIE_BLK_LOOKUP_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PIE_BLK_PHASE_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PIE_BLK_GROUP_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PIE_BLK_TMPLTE_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PIE_TMPLTE_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PIE_MV_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PIE_MV_LEN_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PIE_CLR_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PIE_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PIE_TAG_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PIE_RULE_HIT_INDICATION_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PIE_GLB_HIT_INDICATION_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PIE_MISC_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PIE_FIELD_SELTOR_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PIE_ENCAP_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PIE_MISC2_RTL9310_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_PIE */
#if defined(CONFIG_SDK_CHIP_FEATURE_ACL)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ACL_PORT_LOOKUP_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ACL_CTRL_RTL9310_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_ACL */
#if defined(CONFIG_SDK_CHIP_FEATURE_RANGE_CHECK__PKT_LEN_L4PORT_)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RNG_CHK_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RNG_CHK_IP_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RNG_CHK_IP_RNG_RTL9310_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_RANGE_CHECK__PKT_LEN_L4PORT_ */
#if defined(CONFIG_SDK_CHIP_FEATURE_ATTACK_PREVENTION)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ATK_PRVNT_PORT_EN_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ATK_PRVNT_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ATK_PRVNT_ACT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ATK_PRVNT_IPV6_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ATK_PRVNT_ICMP_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ATK_PRVNT_TCP_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ATK_PRVNT_SMURF_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ATK_PRVNT_STS_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ATK_PRVNT_ARP_INVLD_PORT_ACT_RTL9310_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_ATTACK_PREVENTION */
#if defined(CONFIG_SDK_CHIP_FEATURE_IP_MAC_BIDING)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SEC_IP_MAC_BIND_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SEC_PORT_IP_MAC_BIND_CTRL_RTL9310_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_IP_MAC_BIDING */
#if defined(CONFIG_SDK_CHIP_FEATURE_OAM)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t OAM_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t OAM_PORT_ACT_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t OAM_GLB_DYING_GASP_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t OAM_PORT_DYING_GASP_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t DYING_GASP_POLARITY_CTRL_RTL9310_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_OAM */
#if defined(CONFIG_SDK_CHIP_FEATURE_CFM)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t CFM_RX_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t CFM_RX_CCM_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t CFM_RX_LB_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t CFM_RX_LT_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t CCM_RX_LIFETIME_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t CCM_RX_INST_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t CCM_RX_INST_CNT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t CCM_TX_TAG_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t CCM_TX_INST_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t CCM_TX_INST_MEM_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t CCM_TX_INST_TRK_PRESENT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t CCM_TX_INST_PKT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ETH_DM_PORT_EN_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ETH_DM_RX_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ETH_DM_TX_DLY_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ETH_DM_RX_TIME_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ETH_DM_CLK_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ETH_DM_TIME_FREQ_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ETH_DM_TIME_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ETH_DM_TIME_CTRL_SEC_RTL9310_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_CFM */
#if defined(CONFIG_SDK_CHIP_FEATURE_OPENFLOW)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t OF_PORT_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t OF_VLAN_EN_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t OF_VLAN_AND_PORT_EN_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t OF_ACT_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t OF_L2_FLOW_TBL_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t OF_L3_FLOW_TBL_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t OF_GRP_HASH_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t OF_IGR_TBL_MIS_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t OF_EGR_TBL_MIS_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t OF_IGR_TBL_CNTR_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t OF_EGR_TBL_CNTR_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t OF_EXCPT_CTRL_RTL9310_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_OPENFLOW */
#if defined(CONFIG_SDK_CHIP_FEATURE_L2_TUNNEL)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t CAPWAP_UDP_PORT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t CAPWAP_PARSE_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t L2_TUNNEL_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t CAPWAP_PASSENGER_QOS_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t CAPWAP_TID_RMK_RTL9310_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_L2_TUNNEL */
#if defined(CONFIG_SDK_CHIP_FEATURE_PORT_EXTENSION)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PE_ETAG_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PE_ETAG_MAC_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PE_PORT_ETAG_MAC_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PE_ETAG_RMK_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PE_PORT_ETAG_IGR_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PE_PORT_ETAG_EGR_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PE_PORT_PCID_CTRL_RTL9310_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_PORT_EXTENSION */
#if defined(CONFIG_SDK_CHIP_FEATURE_UNIT_CONTROL)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STK_GBL_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STK_PORT_ID_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STK_DEV_PORT_MAP_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STK_NON_UNICAST_BLOCK_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STK_DBG_CTRL_RTL9310_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_UNIT_CONTROL */
#if defined(CONFIG_SDK_CHIP_FEATURE_REMOTE_ACCESS)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RMT_REG_ACCESS_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RMT_REG_ACCESS_DATA_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RMT_REG_ACCESS_CFG_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RMT_ACCESS_SEM_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RMT_ACCESS_SEM_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RMT_TBL_ACCESS_SET_0_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RMT_TBL_ACCESS_SET_0_DATA_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RMT_TBL_ACCESS_SET_0_CFG_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RMT_TBL_ACCESS_SET_0_L2_READ_MTHD_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RMT_TBL_ACCESS_SET_1_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RMT_TBL_ACCESS_SET_1_DATA_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RMT_TBL_ACCESS_SET_1_CFG_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RMT_TBL_ACCESS_SET_2_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RMT_TBL_ACCESS_SET_2_DATA_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RMT_TBL_ACCESS_SET_2_CFG_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RMT_TBL_ACCESS_SET_3_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RMT_TBL_ACCESS_SET_3_DATA_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RMT_TBL_ACCESS_SET_3_CFG_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RMT_TBL_ACCESS_SET_4_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RMT_TBL_ACCESS_SET_4_DATA_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RMT_TBL_ACCESS_SET_4_CFG_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RMT_TBL_ACCESS_SET_5_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RMT_TBL_ACCESS_SET_5_DATA_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RMT_TBL_ACCESS_SET_5_CFG_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RMT_PHY_ACCESS_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RMT_PHY_ACCESS_DATA_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RMT_PHY_ACCESS_CFG_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RMT_PHY_ACCESS_PHY_MSK_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RMT_PHY_ACCESS_PORT_ID_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RMT_PHY_ACCESS_PARAM_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RMT_PHY_ACCESS_MMD_PARAM_RTL9310_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_REMOTE_ACCESS */
#if defined(CONFIG_SDK_CHIP_FEATURE_REMOTE_INTERRUPT)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RMT_INTR_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RMT_INTR_INFO_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RMT_INTR_DATA_CCM_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RMT_INTR_DATA_LINK_STS_RTL9310_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_REMOTE_INTERRUPT */
#if defined(CONFIG_SDK_CHIP_FEATURE_PARSER)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PARSER_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t GTP_PARSE_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t DIAMETER_PARSE_CTRL_RTL9310_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_PARSER */
#if defined(CONFIG_SDK_CHIP_FEATURE_PARSER_HSB)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t HSB_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t HSB_DATA_RTL9310_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_PARSER_HSB */
#if defined(CONFIG_SDK_CHIP_FEATURE_HSM)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t HSM_DATA_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t HSM_FWD_DATA_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t HSM_POST_DATA_RTL9310_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_HSM */
#if defined(CONFIG_SDK_CHIP_FEATURE_MODIFIER_HSA)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t HSA_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t HSA_DBG_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t HSA_DATA_RTL9310_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_MODIFIER_HSA */
#if defined(CONFIG_SDK_CHIP_FEATURE_DEBUGGING__ALE__LOOPBACK__DROP_MECHANISM__FC_AND_QM_)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ALE_DBG_TRI_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ALE_DBG_CTL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ALE_DBG_MASK_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ALE_DBG_CMP_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ALE_DBG_VAL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ALE_DBG_DATA_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ACL_DBG_TRI_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ACL_DBG_CTL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ACL_DBG_MASK_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ACL_DBG_CMP_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ACL_DBG_VAL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ACL_DBG_DATA_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PER_PORT_MAC_DEBUG0_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PER_PORT_MAC_DEBUG1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PER_PORT_MAC_DEBUG2_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EFUSE_DBG_TRI_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EFUSE_DBG_CTL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EFUSE_DBG_MASK_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EFUSE_DBG_CMP_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EFUSE_DBG_VAL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EFUSE_DBG_DATA_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t GLB_DBG_TRI_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t GLB_DBG_CTL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t GLB_DBG_MASK_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t GLB_DBG_CMP_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t GLB_DBG_VAL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t GLB_DBG_DATA_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t LED_DBG_TRI_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t LED_DBG_CTL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t LED_DBG_MASK_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t LED_DBG_CMP_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t LED_DBG_VAL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t LED_DBG_DATA_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MIB_DBG_TRI_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MIB_DBG_CTL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MIB_DBG_MASK_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MIB_DBG_CMP_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MIB_DBG_VAL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MIB_DBG_DATA_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t CIF_DBG_TRI_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t CIF_DBG_CTL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t CIF_DBG_MASK_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t CIF_DBG_CMP_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t CIF_DBG_VAL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t CIF_DBG_DATA_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t IGR_DBG_TRI_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t IGR_DBG_CTL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t IGR_DBG_MASK_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t IGR_DBG_CMP_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t IGR_DBG_VAL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t IGR_DBG_DATA_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PAR_DBG_TRI_CTL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EGR_DBG_TRI_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EGR_DBG_CTL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EGR_DBG_MASK_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EGR_DBG_CMP_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EGR_DBG_VAL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EGR_DBG_DATA_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ENP_DBG_TRI_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ENP_DBG_CTL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ENP_DBG_MASK_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ENP_DBG_CMP_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ENP_DBG_DATA_RTL9310_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_DEBUGGING__ALE__LOOPBACK__DROP_MECHANISM__FC_AND_QM_ */
#if defined(CONFIG_SDK_CHIP_FEATURE_AUTO_RECOVERY)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RX_PORT_DSC_STC_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RX_PORT_DSC_ERR_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SW_Q_RST_SYS_THR_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SW_Q_RST_P_THR_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t TX_DSC_STS_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t TX_DSC_ERR_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t TX_DSC_CHK_TMR_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RX_FIFO_STS_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RX_FIFO_ERR_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t LINK_LIST_STS_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t LINK_LIST_ERR_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RET_EMPTY_PKT_BUF_STS_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t RET_EMPTY_PKT_BUF_ERR_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SW_Q_RST_CNT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t TRIG_AUTO_RECOVER_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t AUTO_RECOVER_EVENT_FLAG_STS_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t AUTO_RECOVER_EVENT_FLAG_ERR_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t SYS_DSC_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t LINK_LIST_CTRL_0_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t LINK_LIST_CTRL_1_RTL9310_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_AUTO_RECOVERY */
#if defined(CONFIG_SDK_CHIP_FEATURE_ECO)
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t CHIP_INFO_RSVD_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t TM_CTRL_RSVD_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PWRON_REG_RSVD_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PKT_PARSER_RSVD_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t CFM_GEN_RSVD_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t LED_CTRL_RSVD_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t NIC_RSVD_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PLL_CTRL_RSVD_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MDX_CTRL_RSVD_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_STK_W_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MBIST_STK_R_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STKDBG_TSET_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STK_DBGMUX_SEL_1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STK_DBGMUX_SEL_2_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STK_DBGMUX_SEL_3_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STK_DBG_DT_0_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STK_DBG_DT_1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STK_DBG_DT_2_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STK_DBG_DT_3_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STK_DBG_DT_4_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STK_DBG_DT_5_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STK_DBG_DT_6_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STK_DBG_DT_7_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STK_DBG_OUT_CUR_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STK_DBG_OUT_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t GLB_CTRL_RSVD_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EGR_CTRL_RSVD_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ACL_RSVD_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PKT_ENCAP_MISC_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PKT_ENCAP_RSVD1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PKT_ENCAP_RSVD2_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t STAT_PRVTE_DBG_ENCAP_CNTR_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t INGR_CTRL_RSVD_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MAC_CTRL_RSVD_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t PER_PORT_MAC_RSVD_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t MIB_CTRL_RSVD_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ALE_GLB_MISC_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ALE_DEBUG_RSVD_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ALE_PISO_RSVD_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ALE_LUEN_RSVD_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ALE_TM_RSVD_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ALE_BISR_RSVD_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ALE_MIRROR_CTL_RSVD_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ALE_BIST_RSVD_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ALE_SFLOW_CTL_RSVD_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ALE_TABLE_RSVD_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ALE_MIB_RSVD_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ALE_RMA_RSVD_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ALE_VLAN_ISO_CTL_RSVD_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ALE_PRE_MISC_RSVD_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ALE_INGPRI_CTL_RSVD_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ALE_VLAN_CFG_RSVD_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ALE_VLAN_PROFILE_RSVD_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ALE_IVC_MISC_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ALE_PSTORM_RSVD_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ALE_CFM_CTL_RSVD_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ALE_Q_CTRL_RSVD_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ALE_HSB_RSVD_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ALE_HSM_RSVD_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ALE_HSA_RSVD_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ALE_UNSTORM_RSVD_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ALE_TRUNK_RSVD_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ALE_VLAN_PROTOCOL_RSVD_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ALE_L2_CTRL_RSVD_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ALE_L2_CTRL_RSVD1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ALE_L2_CTRL_RSVD2_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ALE_OF_RSVD_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ALE_DCNT_RSVD_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ALE_IBW_CTL_RSVD_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ALE_L3_MISC_CTRL_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ALE_L3_CTRL_RSVD1_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t ALE_L3_CTRL_RSVD2_RTL9310_FIELDS[];
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rtk_regField_t EEE_TX_TIMER_2P5G_5G_LITE_CTRL_RTL9310_FIELDS[];
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_ECO */



#endif    /* __RTK_MANGO_REGFIELD_LIST_H__ */
