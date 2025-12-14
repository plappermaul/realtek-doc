/*
 * Copyright (C) 2012 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 42664 $
 * $Date: 2013-09-09 11:17:16 +0800 (Mon, 09 Sep 2013) $
 *
 * Purpose : Definition of GPIO API
 *
 * Feature : Provide the APIs to enable and configure GPIO
 *
 */

#ifndef __RTL8290_API_H__
#define __RTL8290_API_H__

#include <module/europa/common.h>
#include <rtk/i2c.h>
#include <module/europa/europa_reg_definition.h>
#include <module/europa/europa.h>
#include <rtk/ldd.h>
/*
 * Include Files
 */



// TODO  sync with rtk_ldd_param_t @ ldd.h

typedef struct  rtl8290_param_s
{
    uint32  rssi_v0;
    uint32  rssi_voltage;
    uint32  rssi_i;
    uint32  rssi_v0_2;
    uint32  rssi_voltage_2;
    uint32  rssi_i_2;
    uint32  mpd0;
    uint32  mpd1;
    uint32  mpd_i;
    uint32  bias;
    uint32  modulation;
    uint32  temperature;
    uint32  vdd;
    int32  rx_a;
    int32  rx_b;
    int32  rx_c;
    uint32  rx_power;
    uint32  rx_loopcnt;
    uint32  rx_windowcnt;	
    uint64  rx_sum;	
    int32   tx_a;
    int32   tx_b;
    int32   tx_c;	
    uint32  tx_power;	
    int8    temp_offset;
    int8    steering_mode;
}  rtl8290_param_t;

extern void   setReg(uint16 address, uint16 value);
extern uint16 getReg(uint16 address);
extern void drv_setAsicRegBit(uint16 reg, uint16 rBit, uint16 value);
extern void drv_getAsicRegBit(uint16 reg, uint16 rBit, uint16 *pValue);
extern void drv_setAsicRegBits(uint16 reg, uint16 rBits, uint16 value);
extern void drv_getAsicRegBits(uint16 reg, uint16 rBits, uint16 *pValue);

extern int32
 rtl8290_reset( rtk_ldd_reset_mode_t mode);

extern int32
 rtl8290_powerOnStatus_get(uint32 *result);

extern int32
 rtl8290_mpd0_get(uint16 count, uint32 *value);

extern int32
 rtl8290_mpd0_get_NoBackUpW48(uint16 count, uint32 *value);

extern int32
 rtl8290_loopMode_set( rtk_ldd_loop_mode_t mode);

extern int32
 rtl8290_temperature_get(uint16 *temp);

extern int32
 rtl8290_laserLut_set( rtk_ldd_loop_mode_t mode, uint8 *lut_data);

extern int32
 rtl8290_apdLut_set(uint8 *lut_data);

extern int32
 rtl8290_apcEnableFlow_set( rtk_ldd_apc_func_t func,  rtk_ldd_loop_mode_t mode);

extern int32
 rtl8290_txEnableFlow_set( rtk_ldd_tx_func_t func,  rtk_ldd_loop_mode_t mode, uint8 *lut_data);

extern int32
 rtl8290_rxEnableFlow_set( rtk_ldd_rx_func_t func);

extern int32
 rtl8290_rssiVoltage_get(uint32 *rssi_voltage);

extern int32
 rtl8290_rssiV0_get(uint16 loopcnt, uint32 *rssi_v0);

extern int32
 rtl8290_rx_power_get(uint32 rssi_v0, uint32 *v_rssi,uint32 *i_rssi);

 extern int32
 rtl8290_i2c_init(rtk_i2c_port_t i2cPort);

extern int32
 rtl8290_gpio_init(uint32 id);

extern void Check_C0_C1(void);

extern int32
 rtl8290_parameter_set(uint32 length, uint32 offset, uint8 *flash_data);

extern int32
 rtl8290_tx_power_get(uint32 mpd0, uint32 *v_mpd, uint32 *i_mpd);

extern int32
 rtl8290_vdd_get(uint32 *vdd);

extern int32
 rtl8290_parameter_get(uint32 length, uint32 offset, uint8 *flash_data);

extern int32
 rtl8290_tx_bias_get(uint32 *bias);

extern int32
 rtl8290_tx_mod_get(uint32 *mod);

extern int32  
 rtl8290_tx_bias_set(uint32 bias);

extern int32  
 rtl8290_tx_mod_set(uint32 mod);

extern int32
 rtl8290_calibration_state_set(rtk_enable_t state);

extern int32
 rtl8290_driver_version_get(uint32 *ver);

extern int32
 rtl8290_steering_mode_set(void);

extern int32
 rtl8290_integration_mode_set(void);

extern int32  
 rtl8290_steering_mode_fixup(void);

extern int32  
 rtl8290_integration_mode_fixup(void);

#endif  /* __RTL8290_API_H__ */
