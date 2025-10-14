/*
 * Copyright (C) 2019 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * Purpose : PHY 8251B Driver
 *
 * Feature : PHY 8251B Driver
 *
 */
#ifndef __NIC_RTL8251B_H__
#define __NIC_RTL8251B_H__

#include "rtl8251b_typedef.h"

BOOLEAN
Rtl8251b_phy_reset(
    IN HANDLE hDevice
    );

BOOLEAN
Rtl8251b_autoNegoEnable_get(
    IN  HANDLE hDevice,
    OUT BOOL *pEnable
    );

BOOLEAN
Rtl8251b_autoNegoEnable_set(
    IN HANDLE hDevice,
    IN BOOL Enable
    );

BOOLEAN
Rtl8251b_autoNegoRestart(
    IN HANDLE hDevice
    );

BOOLEAN
Rtl8251b_autoNegoAbility_get(
    IN  HANDLE hDevice,
    OUT PHY_LINK_ABILITY *pPhyAbility
    );

BOOLEAN
Rtl8251b_autoNegoAbility_set(
    IN HANDLE hDevice,
    IN PHY_LINK_ABILITY *pPhyAbility
    );

BOOLEAN
Rtl8251b_duplex_get(
    IN  HANDLE hDevice,
    OUT BOOL *pEnable
    );

BOOLEAN
Rtl8251b_duplex_set(
        IN HANDLE hDevice,
    IN BOOL Enable
    );

BOOLEAN
Rtl8251b_is_link(
    IN  HANDLE hDevice,
    OUT BOOL *plinkOK
    );

BOOLEAN
Rtl8251b_speed_get(
    IN  HANDLE hDevice,
    OUT UINT16 *pSpeed
    );

BOOLEAN
Rtl8251b_enable_set(
    IN HANDLE hDevice,
    IN BOOL Enable
    );

BOOLEAN
Rtl8251b_force_speed_set(
    IN HANDLE hDevice,
    IN UINT16 Speed
    );

BOOLEAN
Rtl8251b_greenEnable_get(
    IN  HANDLE hDevice,
    OUT BOOL *pEnable
    );

BOOLEAN
Rtl8251b_greenEnable_set(
    IN HANDLE hDevice,
    IN BOOL Enable
    );

BOOLEAN
Rtl8251b_eeeEnable_get(
    IN  HANDLE hDevice,
    OUT PHY_EEE_ENABLE *pEeeEnable
    );

BOOLEAN
Rtl8251b_eeeEnable_set(
    IN HANDLE hDevice,
    IN PHY_EEE_ENABLE *pEeeEnable
    );

BOOLEAN
Rtl8251b_crossOverMode_get(
    IN  HANDLE hDevice,
    OUT PHY_CROSSPVER_MODE *CrossOverMode
    );

BOOLEAN
Rtl8251b_crossOverMode_set(
    IN HANDLE hDevice,
    IN PHY_CROSSPVER_MODE CrossOverMode
    );

BOOLEAN
Rtl8251b_crossOverStatus_get(
    IN  HANDLE hDevice,
    OUT PHY_CROSSPVER_STATUS *pCrossOverStatus
    );

BOOLEAN
Rtl8251b_masterSlave_get(
    IN  HANDLE hDevice,
    OUT PHY_MASTERSLAVE_MODE *MasterSlaveMode
    );

BOOLEAN
Rtl8251b_masterSlave_set(
    IN HANDLE hDevice,
    IN PHY_MASTERSLAVE_MODE MasterSlaveMode
    );

BOOLEAN
Rtl8251b_loopback_get(
    IN  HANDLE hDevice,
    OUT BOOL *pEnable
    );

BOOLEAN
Rtl8251b_loopback_set(
    IN HANDLE hDevice,
    IN BOOL Enable
    );

BOOLEAN
Rtl8251b_PMA_loopback_get(
	IN  HANDLE hDevice,
	OUT BOOL *pEnable
	);

BOOLEAN
Rtl8251b_PMA_loopback_set(
	IN HANDLE hDevice,
	IN BOOL Enable
	);

BOOLEAN
Rtl8251b_downSpeedEnable_get(
    IN  HANDLE hDevice,
    OUT BOOL *pEnable
    );

BOOLEAN
Rtl8251b_downSpeedEnable_set(
    IN HANDLE hDevice,
    IN BOOL Enable
    );

BOOLEAN
Rtl8251b_gigaLiteEnable_get(
    IN  HANDLE hDevice,
    OUT BOOL *pEnable
    );

BOOLEAN
Rtl8251b_gigaLiteEnable_set(
    IN HANDLE hDevice,
    IN BOOL Enable
    );

BOOLEAN
Rtl8251b_mdiSwapEnable_get(
    IN  HANDLE hDevice,
    OUT BOOL *pEnable
    );

BOOLEAN
Rtl8251b_mdiSwapEnable_set(
    IN HANDLE hDevice,
    IN BOOL Enable
    );

BOOLEAN
Rtl8251b_rtct_start(
    IN HANDLE hDevice
    );

BOOLEAN
Rtl8251b_rtctResult_convert(
    IN UINT16 phydata,
    OUT PHY_RTCT_STATUS *pRtctStatus
    );

BOOLEAN
Rtl8251b_rtctResult_get(
    IN HANDLE hDevice,
    OUT PHY_RTCT_RESULT *pRtctResult
    );

BOOLEAN
Rtl8251b_linkDownPowerSavingEnable_get(
    IN  HANDLE hDevice,
    OUT BOOL *pEnable
    );

BOOLEAN
Rtl8251b_linkDownPowerSavingEnable_set(
    IN HANDLE hDevice,
    IN BOOL Enable
    );

BOOLEAN
Rtl8251b_2p5gLiteEnable_get(
    IN  HANDLE hDevice,
    OUT BOOL *pEnable
    );

BOOLEAN
Rtl8251b_2p5gLiteEnable_set(
    IN HANDLE hDevice,
    IN BOOL Enable
    );

BOOLEAN
Rtl8251b_ThermalSensorEnable_get(
    IN  HANDLE hDevice,
    OUT BOOL *pEnable
    );

BOOLEAN
Rtl8251b_ThermalSensorEnable_set(
    IN HANDLE hDevice,
    IN BOOL Enable
    );

BOOLEAN
Rtl8251b_ieeeTestMode_set(
    IN HANDLE hDevice,
    IN UINT16 Speed,
    IN PHY_IEEE_TEST_MODE *pIEEEtestmode
    );

BOOLEAN
Rtl8251b_serdes_rst(
    IN HANDLE hDevice
    );

BOOLEAN
Rtl8251b_serdes_link_get(
    IN  HANDLE hDevice,
    OUT BOOL *perdesLink,
    OUT PHY_SERDES_MODE *SerdesMode
    );

BOOLEAN
Rtl8251b_serdes_option_set(
    IN HANDLE hDevice,
    IN UINT8 functioninput
    );

BOOLEAN
Rtl8251b_serdes_polarity_swap(
    IN HANDLE hDevice,
    IN PHY_SERDES_POLARITY_SWAP *ppolarityswap
    );

BOOLEAN
Rtl8251b_serdes_autoNego_set(
    IN HANDLE hDevice,
    IN BOOL Enable
    );

#endif /* __NIC_RTL8251B_H__ */

