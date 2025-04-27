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
 * Purpose : PHY 8226 Driver
 *
 * Feature : PHY 8226 Driver
 *
 */
#ifndef __NIC_RTL8226_H__
#define __NIC_RTL8226_H__

#include <hal/phy/nic_rtl8226/rtl8226_typedef.h>

BOOLEAN
Rtl8226_phy_reset(
    IN HANDLE hDevice
    );

BOOLEAN
Rtl8226_autoNegoEnable_get(
    IN  HANDLE hDevice,
    OUT BOOL *pEnable
    );

BOOLEAN
Rtl8226_autoNegoEnable_set(
    IN HANDLE hDevice,
    IN BOOL Enable
    );

BOOLEAN
Rtl8226_autoNegoAbility_get(
    IN  HANDLE hDevice,
    OUT PHY_LINK_ABILITY *pPhyAbility
    );

BOOLEAN
Rtl8226_autoNegoAbility_set(
    IN HANDLE hDevice,
    IN PHY_LINK_ABILITY *pPhyAbility
    );

BOOLEAN
Rtl8226_duplex_get(
    IN  HANDLE hDevice,
    OUT BOOL *pEnable
    );

BOOLEAN
Rtl8226_duplex_set(
    IN HANDLE hDevice,
    IN BOOL Enable
    );

BOOLEAN
Rtl8226_is_link(
    IN  HANDLE hDevice,
    OUT BOOL *plinkok
    );

BOOLEAN
Rtl8226_speed_get(
    IN  HANDLE hDevice,
    OUT UINT16 *pSpeed
    );

BOOLEAN
Rtl8226_force_speed_set(
    IN HANDLE hDevice,
    IN UINT16 Speed
    );

BOOLEAN
Rtl8226_enable_set(
    IN HANDLE hDevice,
    IN BOOL Enable
    );

BOOLEAN
Rtl8226_greenEnable_get(
    IN  HANDLE hDevice,
    OUT BOOL *pEnable
    );

BOOLEAN
Rtl8226_greenEnable_set(
    IN HANDLE hDevice,
    IN BOOL Enable
    );

BOOLEAN
Rtl8226_eeeEnable_get(
    IN  HANDLE hDevice,
    OUT PHY_EEE_ENABLE *pEeeEnable
    );

BOOLEAN
Rtl8226_eeeEnable_set(
    IN HANDLE hDevice,
    IN PHY_EEE_ENABLE *pEeeEnable
    );

BOOLEAN
Rtl8226_crossOverMode_get(
    IN  HANDLE hDevice,
    OUT PHY_CROSSPVER_MODE *CrossOverMode
    );

BOOLEAN
Rtl8226_crossOverMode_set(
    IN HANDLE hDevice,
    IN PHY_CROSSPVER_MODE CrossOverMode
    );

BOOLEAN
Rtl8226_crossOverStatus_get(
    IN  HANDLE hDevice,
    OUT PHY_CROSSPVER_STATUS *pCrossOverStatus
    );

BOOLEAN
Rtl8226_masterSlave_get(
    IN  HANDLE hDevice,
    OUT PHY_MASTERSLAVE_MODE *MasterSlaveMode
    );

BOOLEAN
Rtl8226_masterSlave_set(
    IN HANDLE hDevice,
    IN PHY_MASTERSLAVE_MODE MasterSlaveMode
    );

BOOLEAN
Rtl8226_loopback_get(
    IN  HANDLE hDevice,
    OUT BOOL *pEnable
    );

BOOLEAN
Rtl8226_loopback_set(
    IN HANDLE hDevice,
    IN BOOL Enable
    );

BOOLEAN
Rtl8226_downSpeedEnable_get(
    IN  HANDLE hDevice,
    OUT BOOL *pEnable
    );

BOOLEAN
Rtl8226_downSpeedEnable_set(
    IN HANDLE hDevice,
    IN BOOL Enable
    );

BOOLEAN
Rtl8226_gigaLiteEnable_get(
    IN  HANDLE hDevice,
    OUT BOOL *pEnable
    );

BOOLEAN
Rtl8226_gigaLiteEnable_set(
    IN HANDLE hDevice,
    IN BOOL Enable
    );

BOOLEAN
Rtl8226_mdiSwapEnable_get(
    IN  HANDLE hDevice,
    OUT BOOL *pEnable
    );

BOOLEAN
Rtl8226_mdiSwapEnable_set(
    IN HANDLE hDevice,
    IN BOOL Enable
    );

BOOLEAN
Rtl8226_rtct_start(
    IN HANDLE hDevice
    );

BOOLEAN
Rtl8226_rtctResult_get(
    IN HANDLE hDevice,
    OUT PHY_RTCT_RESULT *pRtctResult
    );

BOOLEAN
Rtl8226_linkDownPowerSavingEnable_get(
    IN  HANDLE hDevice,
    OUT BOOL *pEnable
    );

BOOLEAN
Rtl8226_linkDownPowerSavingEnable_set(
    IN HANDLE hDevice,
    IN BOOL Enable
    );

BOOLEAN
Rtl8226_2p5gLiteEnable_get(
    IN  HANDLE hDevice,
    OUT BOOL *pEnable
    );

BOOLEAN
Rtl8226_2p5gLiteEnable_set(
    IN HANDLE hDevice,
    IN BOOL Enable
    );

BOOLEAN
Rtl8226_ThermalSensorEnable_get(
    IN  HANDLE hDevice,
    OUT BOOL *pEnable
    );

BOOLEAN
Rtl8226_ThermalSensorEnable_set(
    IN HANDLE hDevice,
    IN BOOL Enable
    );

BOOLEAN
Rtl8226_ieeeTestMode_set(
    IN HANDLE hDevice,
    IN UINT16 Speed,
    IN PHY_IEEE_TEST_MODE *pIEEEtestmode
    );

BOOLEAN
Rtl8226_serdes_rst(
    IN HANDLE hDevice
    );

BOOLEAN
Rtl8226_serdes_link_get(
    IN  HANDLE hDevice,
    OUT BOOL *perdesLink,
    OUT PHY_SERDES_MODE *SerdesMode
    );

BOOLEAN
Rtl8226_serdes_option_set(
    IN HANDLE hDevice,
    IN UINT8 functioninput
    );

BOOLEAN
Rtl8226_serdes_polarity_swap(
    IN HANDLE hDevice,
    IN PHY_SERDES_POLARITY_SWAP *ppolarityswap
    );

BOOLEAN
Rtl8226_serdes_autoNego_set(
    IN HANDLE hDevice,
    IN BOOL Enable
    );

#endif /* __NIC_RTL8226_H__ */

