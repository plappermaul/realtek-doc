/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 99568 $
 * $Date: 2019-08-21 18:31:35 +0800 (Wed, 21 Aug 2019) $
 *
 * Purpose : define common command flags
 *
 * Feature : None
 *
 */

#ifndef __CMD_FLAG_COMMON_H__
#define __CMD_FLAG_COMMON_H__

/*
 * Include Files
 */


/*
 * Symbol Definition
 */

/* Common Commands */
/* Debug */
#ifdef CONFIG_SDK_DEBUG
//#define CMD_DEBUG_DUMP_HSA
//#define CMD_DEBUG_DUMP_HSB
//#define CMD_DEBUG_DUMP_HSM
//#undef CMD_DEBUG_DUMP_HSM_INDEX
//#undef CMD_DEBUG_DUMP_PMI
//#undef CMD_DEBUG_DUMP_PPI_INDEX
//#define CMD_DEBUG_DUMP_MIB_COUNTER_DEBUG_INDEX
//#define CMD_DEBUG_DUMP_MIB_COUNTER_DEBUG_ALL
//#define CMD_DEBUG_GET_FLOWCTRL_USED_PAGE_CNT_EGRESS_INGRESS_PORT_PORTS_ALL
//#define CMD_DEBUG_GET_FLOWCTRL_USED_PAGE_CNT_SYSTEM
//#define CMD_DEBUG_GET_FLOWCTRL_USED_PAGE_CNT_QUEUE_BASED_PORT_PORTS_ALL
//#define CMD_DEBUG_RESET_FLOWCTRL_USED_PAGE_CNT_EGRESS_INGRESS_PORT_PORTS_ALL
//#define CMD_DEBUG_RESET_FLOWCTRL_USED_PAGE_CNT_SYSTEM
#define CMD_DEBUG_GET_LOG
#define CMD_DEBUG_SET_LOG_STATE_DISABLE_ENABLE
#define CMD_DEBUG_SET_LOG_LEVEL_VALUE
#define CMD_DEBUG_SET_LOG_LEVEL_MASK_BITMASK
#define CMD_DEBUG_SET_LOG_LEVEL_TYPE_LEVEL_LEVEL_MASK
#define CMD_DEBUG_SET_LOG_FORMAT_NORMAL_DETAIL
#define CMD_DEBUG_SET_LOG_MODULE_BITMASK
//#define CMD_DEBUG_GET_TABLE_TABLE_IDX_ADDRESS
#endif

#define CMD_DEBUG_GET_VERSION
#define CMD_DEBUG_GET_SW_WD
#define CMD_DEBUG_SET_SW_WD_STATE_DISABLE_ENABLE
#define CMD_DEBUG_GET_CHIP
#define CMD_DEBUG_FLASHTEST_MTD_MTD_IDX
#define CMD_DEBUG_GET_MEMORY_ALLOCATED
#define CMD_PORT_SET_PHY_EYE_MONITOR_PORT_SDSID_FRAMENUM
#define CMD_PORT_GET_PHY_EYE_MONITOR_INFO_PORT_SDSID_FRAMENUM
#define CMD_DEBUG_GET_MEMORY_AVAILABLE
#define CMD_DEBUG_GET_MEMORY_BASE_ADDRESS
#define CMD_DEBUG_GET_IMAGE_INFO
#define CMD_DEBUG_PTHREAD_CREATE


/* Diag */
#define CMD_DIAG_SET_CABLE_DOCTOR_PORT_PORTS_ALL_START
#define CMD_DIAG_GET_CABLE_DOCTOR_PORT_PORTS_ALL
#define CMD_DIAG_DUMP_TABLE_INDEX
#define CMD_DIAG_DUMP_TABLE_INDEX_START_INDEX_END_INDEX_DETAIL
#define CMD_DIAG_WHOLEDUMP_MAC_REG_PHY_REG_SOC_REG_TABLE_ALL
#define CMD_DIAG_GET_PHY_PHYID_SERDES_LINK_STATUS
#define CMD_DIAG_GET_PHY_PHYID_SERDES_RX_SYM_ERR
#define CMD_DIAG_CLEAR_PHY_PHYID_SERDES_RX_SYM_ERR
#define CMD_DIAG_GET_PHY_PORT_PORTS_ALL_RX_CNT
#define CMD_DIAG_CLEAR_PHY_PORT_PORTS_ALL_RX_CNT
#define CMD_DIAG_SET_PHY_PORT_PORTS_ALL_RX_CNT_MAC_TX_PHY_RX
#define CMD_DIAG_DUMP_TABLE_INDEX_NAME


/* GeneralCtrl GPIO */
#define CMD_GPIO_SET_DEV_DEV_ID_PIN_GPIO_ID_INIT_DIRECTION_IN_OUT_DEFAULT_DEFAULT_VALUE
#define CMD_GPIO_SET_DEV_DEV_ID_ACCESS_MDC_EXTRA_ADDRESS_DEV_ADDRESS
#define CMD_GPIO_SET_DEV_DEV_ID_PIN_GPIO_ID_DATA
#define CMD_GPIO_GET_DEV_DEV_ID_PIN_GPIO_ID
#define CMD_GPIO_SET_DEV_DEV_ID_STATE_DISABLE_ENABLE

/* LED */
#define CMD_LED_GET_SOFTWARE_CONTROL_PORT_ALL_MODE
#define CMD_LED_GET_SOFTWARE_CONTROL_PORT_ALL_STATE
#define CMD_LED_GET_SYSTEM_SYS_MODE
#define CMD_LED_GET_SYSTEM_SYS_STATE
#define CMD_LED_SET_SOFTWARE_CONTROL_PORT_ALL_ENTITY_ENTITY_MEDIA_COPPER_FIBER_MODE_OFF_BLINK_32_BLINK_64_BLINK_128_BLINK_256_BLINK_512_BLINK_1024_CONTINUE
#define CMD_LED_SET_SOFTWARE_CONTROL_PORT_ALL_ENTITY_ENTITY_STATE_DISABLE_ENABLE
//#define CMD_LED_SET_SOFTWARE_CONTROL_START
#define CMD_LED_SET_SYSTEM_SYS_MODE_OFF_BLINK_64_BLINK_1024_CONTINUE
#define CMD_LED_SET_SYSTEM_SYS_STATE_DISABLE_ENABLE

/* Register */
#define CMD_REGISTER_SET_ADDRESS_VALUE
#define CMD_REGISTER_GET_ADDRESS_WORDS
#define CMD_REGISTER_LIST_KEYWORD
#define CMD_REGISTER_MODIFY_REG_NAME_FIELD_VAL_PAIRS

#define CMD_REGISTER_GET_ALL

/* RTL8231 */
//#undef CMD_RTL8231_GET_I2C_SLAVE_ADDR_REGISTER
//#undef CMD_RTL8231_SET_I2C_SLAVE_ADDR_REGISTER_DATA
//#undef CMD_RTL8231_GET_MDC_PHY_ID_PAGE_REGISTER
//#undef CMD_RTL8231_SET_MDC_PHY_ID_PAGE_REGISTER_DATA

/* external GPIO */
#define CMD_EXT_GPIO_DUMP_DEV_DEV_ID
#define CMD_EXT_GPIO_GET_DEV_DEV_ID_PIN_GPIO_ID
#define CMD_EXT_GPIO_GET_DEV_DEV_ID_PIN_GPIO_ID_DIRECTION
#define CMD_EXT_GPIO_GET_DEV_DEV_ID_READY
#define CMD_EXT_GPIO_GET_DEV_DEV_ID_STATE
//#undef CMD_EXT_GPIO_GET_DEV_DEV_ID_SYNC_STATE
//#undef CMD_EXT_GPIO_GET_DEV_DEV_ID_SYNC_STATUS
#define CMD_EXT_GPIO_GET_DEV_DEV_ID_REGISTER
//#undef CMD_EXT_GPIO_GET_DEV_DEV_ID_READ_I2C_REGISTER_NUMBER
//#undef CMD_EXT_GPIO_SET_DEV_DEV_ID_INIT_I2C_SCK_PIN_SCK_GPIO_ID_SDA_PIN_SDA_GPIO_ID
#define CMD_EXT_GPIO_SET_DEV_DEV_ID_INIT_MDC_PHY_ID_PAGE
//#undef CMD_EXT_GPIO_SET_DEV_DEV_ID_PIN_GPIO_ID_INIT_IN_OUT_DEBOUNCE_INVERTER
#define CMD_EXT_GPIO_SET_DEV_DEV_ID_PIN_GPIO_ID_DIRECTION_IN_OUT
#define CMD_EXT_GPIO_SET_DEV_DEV_ID_PIN_GPIO_ID_DATA
#define CMD_EXT_GPIO_SET_DEV_DEV_ID_STATE_DISABLE_ENABLE
//#undef CMD_EXT_GPIO_SET_DEV_DEV_ID_SYNC_START
//#undef CMD_EXT_GPIO_SET_DEV_DEV_ID_SYNC_STATE_DISABLE_ENABLE
#define CMD_EXT_GPIO_SET_DEV_DEV_ID_REGISTER_DATA
//#undef CMD_EXT_GPIO_SET_DEV_DEV_ID_WRITE_I2C_REGISTER_DATA

/* SDK */
#define CMD_SDK_TEST_GROUP_ITEM
#define CMD_SDK_TEST_CASE_ID_START_END
#define CMD_SDK_SET_TEST_MODE_NORMAL_SCAN
#define CMD_SDK_GET_TEST_MODE

/* SPI */
#define CMD_SPI_SET_REG_REG_ADD_REG_DATA
#define CMD_SPI_GET_REG_REG_ADD
#define CMD_SPI_SET_SCLK_SCLK_MOSI_MOSI_MISO_MISO_SS_SS_RESET_RESET_INT_INT

/* Switch */
#define CMD_SWITCH_GET_HALCTRL_INFORMATION
#define CMD_SWITCH_GET_PROBE_INFORMATION
#define CMD_SWITCH_GET_SDK_VERSION
#define CMD_SWITCH_GET_WATCHDOG_MODE
#define CMD_SWITCH_GET_WATCHDOG_SCALE_BITS
#define CMD_SWITCH_GET_WATCHDOG_STATE
#define CMD_SWITCH_SET_WATCHDOG_MODE_ISR_NORMAL
#define CMD_SWITCH_SET_WATCHDOG_SCALE_BITS_25_26_27_28
#define CMD_SWITCH_SET_WATCHDOG_STATE_DISABLE_ENABLE

/* UART */
#define CMD_UART1_GET_CHARACTER_NUMBER_TIMEOUT_TIMEOUT
#define CMD_UART1_PUT_CHARACTER_STRING
#define CMD_UART1_GET_BAUDRATE
#define CMD_UART1_SET_BAUDRATE_9600_19200_38400_57600_115200

/* Port */
#define CMD_PORT_DUMP_CPU_PORT
#define CMD_PORT_DUMP_PORT_PORTS_ALL
#define CMD_PORT_GET_AUTO_NEGO_PORT_PORTS_ALL_ABILITY
#define CMD_PORT_GET_AUTO_NEGO_PORT_PORTS_ALL_STATE
#define CMD_PORT_GET_BACK_PRESSURE_PORT_PORTS_ALL_STATE
#define CMD_PORT_GET_COMBO_MODE_PORT_PORTS_ALL
#define CMD_PORT_GET_GREEN_PORT_PORTS_ALL_STATE
#define CMD_PORT_GET_GIGA_LITE_PORT_PORTS_ALL_STATE
#define CMD_PORT_GET_2PT5G_LITE_PORT_PORTS_ALL_STATE
#define CMD_PORT_GET_PHY_FORCE_PORT_PORTS_ALL
#define CMD_PORT_GET_LINK_DOWN_POWER_SAVING_PORT_PORT_ALL_STATE
#define CMD_PORT_GET_PHY_REG_PORT_PORTS_ALL_PAGE_PAGE_REGISTER_REGISTER
#define CMD_PORT_GET_PHY_REG_PORT_PORTS_ALL_PAGE_PAGE_REGISTER_REGISTER_NUMBER_NUMBER
#define CMD_PORT_GET_PHY_EXT_REG_PORT_PORTS_ALL_MAIN_PAGE_MAINPAGE_EXT_PAGE_EXTPAGE_PARK_PAGE_PARKPAGE_REGISTER_REGISTER
#define CMD_PORT_GET_PHY_EXT_REG_PORT_PORTS_ALL_MAIN_PAGE_MAINPAGE_EXT_PAGE_EXTPAGE_PARK_PAGE_PARKPAGE_REGISTER_REGISTER_NUMBER_NUMBER
#define CMD_PORT_GET_PHY_MMD_REG_PORT_PORTS_ALL_MMD_ADDR_MMD_ADDR_MMD_REG_MMD_REG
#define CMD_PORT_GET_PHY_MMD_REG_PORT_PORTS_ALL_MMD_ADDR_MMD_ADDR_MMD_REG_MMD_REG_NUMBER_NUMBER
#define CMD_PORT_GET_PHY_REG_PORT_PORTS_ALL_PAGE_PAGE
#define CMD_PORT_GET_PHY_REG_PORT_PORTS_ALL
#define CMD_PORT_GET_RX_TX_PORT_PORTS_ALL_STATE
#define CMD_PORT_GET_PORT_PORTS_ALL_STATE
#define CMD_PORT_SET_AUTO_NEGO_PORT_PORTS_ALL_ABILITY_10H_10F_100H_100F_1000F_FLOW_CONTROL_ASY_FLOW_CONTROL
#define CMD_PORT_SET_AUTO_NEGO_PORT_PORTS_ALL_STATE_DISABLE_ENABLE
#define CMD_PORT_SET_BACK_PRESSURE_PORT_PORTS_ALL_STATE_DISABLE_ENABLE
#define CMD_PORT_SET_COMBO_MODE_PORT_PORTS_ALL_COPPER_FORCE_FIBER_FORCE
#define CMD_PORT_SET_PHY_FORCE_PORT_PORTS_ALL_ABILITY_10H_10F_100H_100F_1000F
#define CMD_PORT_SET_PHY_FORCE_PORT_PORTS_ALL_FLOW_CONTROL_STATE_DISABLE_ENABLE
#define CMD_PORT_SET_PHY_FORCE_PORT_PORTS_ALL_FLOW_CONTROL_TX_PAUSE_DISABLE_ENABLE_RX_PAUSE_DISABLE_ENABLE
#define CMD_PORT_SET_GREEN_PORT_PORTS_ALL_STATE_DISABLE_ENABLE
#define CMD_PORT_SET_LINK_DOWN_POWER_SAVING_PORT_PORT_ALL_STATE_DISABLE_ENABLE
#define CMD_PORT_SET_GIGA_LITE_PORT_PORTS_ALL_STATE_DISABLE_ENABLE
#define CMD_PORT_SET_2PT5G_LITE_PORT_PORTS_ALL_STATE_DISABLE_ENABLE
#define CMD_PORT_SET_PHY_REG_PORT_PORTS_ALL_PAGE_PAGE_REGISTER_REGISTER_DATA_DATA
#define CMD_PORT_SET_PHY_EXT_REG_PORT_PORTS_ALL_MAIN_PAGE_MAINPAGE_EXT_PAGE_EXTPAGE_PARK_PAGE_PARKPAGE_REGISTER_REGISTER_DATA_DATA
#define CMD_PORT_SET_PHY_MMD_REG_PORT_PORTS_ALL_MMD_ADDR_MMD_ADDR_MMD_REG_MMD_REG_DATA_DATA
#define CMD_PORT_SET_RX_TX_PORT_PORTS_ALL_STATE_DISABLE_ENABLE
#define CMD_PORT_SET_PORT_PORTS_ALL_STATE_DISABLE_ENABLE
#define CMD_PORT_SET_CROSS_OVER_PORT_PORTS_ALL_MODE_AUTO_MDI_MDIX
#define CMD_PORT_GET_CROSS_OVER_PORT_PORTS_ALL_MODE
#define CMD_PORT_GET_CROSS_OVER_PORT_PORTS_ALL_STATUS
#define CMD_PORT_GET_COMBO_FIBER_MODE_PORT_PORTS_ALL
#define CMD_PORT_SET_COMBO_FIBER_MODE_PORT_PORTS_ALL_FIBER_1000_FIBER_100_FIBER_AUTO
#define CMD_PORT_SET_MASTER_SLAVE_PORT_PORTS_ALL_AUTO_MASTER_SLAVE
#define CMD_PORT_GET_MASTER_SLAVE_PORT_PORTS_ALL
#define CMD_PORT_SET_PORT_PORTS_ALL_FIBER_LOOPBACK_ENABLE_DISABLE
#define CMD_PORT_GET_PORT_PORTS_ALL_FIBER_LOOPBACK
#define CMD_PORT_SET_PORT_PORTS_ALL_DOWN_SPEED_ENABLE_DISABLE
#define CMD_PORT_GET_PORT_PORTS_ALL_DOWN_SPEED
#define CMD_PORT_GET_PORT_PORTS_ALL_DOWN_SPEED_STATUS
#define CMD_PORT_SET_PORT_PORTS_ALL_FIBER_NWAY_FORCE_LINK_ENABLE_DISABLE
#define CMD_PORT_GET_PORT_PORTS_ALL_FIBER_NWAY_FORCE_LINK
#define CMD_PORT_SET_PORT_PORTS_ALL_FIBER_OAM_LOOPBACK_ENABLE_DISABLE
#define CMD_PORT_SET_10G_MEDIA_PORT_PORTS_ALL_FIBER10G_FIBER1G_DAC50CM_DAC100CM_DAC300CM
#define CMD_PORT_GET_10G_MEDIA_PORT_PORTS_ALL
#define CMD_PORT_GET_PORT_PORTS_ALL_PHY_LOOPBACK
#define CMD_PORT_SET_PORT_PORTS_ALL_PHY_LOOPBACK_ENABLE_DISABLE
#define CMD_PORT_GET_PORT_PORTS_ALL_FIBER_RX
#define CMD_PORT_SET_PORT_PORTS_ALL_FIBER_RX_ENABLE_DISABLE
#define CMD_PORT_SET_PORT_PORTS_ALL_FIBER_UNIDIR_ENABLE_DISABLE
#define CMD_PORT_GET_PORT_PORTS_ALL_FIBER_UNIDIR_STATE
#define CMD_PORT_SET_PHY_REG_PORT_PORTS_ALL_PAGE_PAGE_REGISTER_REGISTER_BITS_MSB_LSB_VALUE
#define CMD_PORT_GET_PHY_REG_PORT_PORTS_ALL_PAGE_PAGE_REGISTER_REGISTER_BITS_MSB_LSB
#define CMD_PORT_GET_PHY_MAC_INTF_SERDES_MODE_PORT_PORTS_ALL
#define CMD_PORT_SET_PHY_LED_MODE_PORT_PORTS_ALL_LED_ID_MDI_MDI_LED_INDICATOR_1000M_SPEED_100M_SPEED_10M_SPEED_1000M_ACT_100M_ACT_10M_ACT_DUPLEX_COLLISION_TX_ACT_RX_ACT
#define CMD_PORT_SET_PHY_LED_CONTROL_PORT_PORTS_ALL_BLINK_RATE_32MS_48MS_64MS_96MS_128MS_256MS_512MS_1024MS
#define CMD_PORT_SET_PHY_LED_CONTROL_PORT_PORTS_ALL_BURST_CYCLE_8MS_16MS_32MS_64MS
#define CMD_PORT_SET_PHY_LED_CONTROL_PORT_PORTS_ALL_CLOCK_CYCLE_32NS_64NS_96NS_192NS
#define CMD_PORT_SET_PHY_LED_CONTROL_PORT_PORTS_ALL_ACTIVE_HIGH_LOW
#define CMD_PORT_GET_PHY_LED_CONTROL_PORT_PORTS_ALL
#define CMD_PORT_GET_PORT_PORTS_ALL_PHY_MDI_LOOPBACK
#define CMD_PORT_SET_PORT_PORTS_ALL_PHY_MDI_LOOPBACK_ENABLE_DISABLE
#define CMD_PORT_GET_PHY_MAC_INTF_SERDES_LINK_STATUS_PORT_PORTS_ALL
#define CMD_PORT_GET_PHY_SERDES_EYE_PARAM_PORT_PORTS_ALL_SERDES_ID_SDSID
#define CMD_PORT_SET_PHY_SERDES_EYE_PARAM_PORT_PORTS_ALL_SERDES_ID_SDSID_PRE_STATE_DISABLE_ENABLE
#define CMD_PORT_SET_PHY_SERDES_EYE_PARAM_PORT_PORTS_ALL_SERDES_ID_SDSID_POST_STATE_DISABLE_ENABLE
#define CMD_PORT_SET_PHY_SERDES_EYE_PARAM_PORT_PORTS_ALL_SERDES_ID_SDSID_PRE_AMP_PRE_AMP
#define CMD_PORT_SET_PHY_SERDES_EYE_PARAM_PORT_PORTS_ALL_SERDES_ID_SDSID_MAIN_AMP_MAIN_AMP
#define CMD_PORT_SET_PHY_SERDES_EYE_PARAM_PORT_PORTS_ALL_SERDES_ID_SDSID_POST_AMP_POST_AMP
#define CMD_PORT_SET_PHY_SERDES_EYE_PARAM_PORT_PORTS_ALL_SERDES_ID_SDSID_IMPEDANCE_IMPEDANCE
/* SerDes */
#define CMD_SERDES_GET_ID_SDSID_PAGE_PAGE_REG_REG
#define CMD_SERDES_SET_ID_SDSID_PAGE_PAGE_REG_REG_DATA_DATA
#define CMD_SERDES_CLEAR_SDSID_RX_SYM_ERR
#define CMD_SERDES_GET_SDSID_LINK_STATUS
#define CMD_SERDES_RESET_SDSID
#define CMD_SERDES_GET_SDSID_RX_SYM_ERR
#define CMD_SERDES_SET_SDSID_10G_SQUARE_STATE_DISABLE_ENABLE
#define CMD_SERDES_SET_SDSID_10G_PRBS9_STATE_DISABLE_ENABLE
#define CMD_SERDES_SET_SDSID_10G_PRBS31_STATE_DISABLE_ENABLE
#define CMD_SERDES_GET_SDSID_TESTMODE_CNT
#define CMD_SERDES_SET_SDSID_LEQ_ADAPT
#define CMD_SERDES_GET_SDSID_LEQ
#define CMD_SERDES_SET_SDSID_XSG_NWAY_STATE_DISABLE_ENABLE
#define CMD_SERDES_GET_SDSID_CMU_BAND
#define CMD_SERDES_SET_SDSID_CMU_BAND_STATE_DISABLE_ENABLE_DATA_DATA
#define CMD_SERDES_EYE_MONITOR_SDSID_FRAMENUM
#define CMD_SERDES_EYE_MONITOR_INFO_SDSID_FRAMENUM
#define CMD_SERDES_SET_SDSID_EYE_PARAM_PRE_STATE_DISABLE_ENABLE
#define CMD_SERDES_SET_SDSID_EYE_PARAM_POST_STATE_DISABLE_ENABLE
#define CMD_SERDES_SET_SDSID_EYE_PARAM_PRE_AMP_PRE_AMP
#define CMD_SERDES_SET_SDSID_EYE_PARAM_MAIN_AMP_MAIN_AMP
#define CMD_SERDES_SET_SDSID_EYE_PARAM_POST_AMP_POST_AMP
#define CMD_SERDES_GET_SDSID_EYE_PARAM

#define CMD_SERDES_GET_SDSID_RXCALI_PARAM_CFG
#define CMD_SERDES_GET_SDSID_RXCALI_STATE
#define CMD_SERDES_SET_SDSID_RXCALI_STATE_DISABLE_ENABLE
#define CMD_SERDES_SET_SDSID_RXCALI_START

#undef  CMD_RTSTK_DC_DCBOX_TA_CFG_PORT

#endif /* __CMD_FLAG_COMMON_H__ */

