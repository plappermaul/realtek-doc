#ifndef __EUROPA_8290C_DDMI_H__
#define __EUROPA_8290C_DDMI_H__

#include "rtl8291_api.h"


#define DDMI_A0_REG_START_ADDRESS 0
#define DDMI_A2_REG_START_ADDRESS 256
#define DDMI_RX_POWER_NUM     20
#define DDMI_TX_POWER_NUM     20

extern void update_8290c_ddmi(uint32 devId, rtl8291_param_t *ldd_param);
extern void update_ddmi_8290c_temperature(uint32 devId, rtl8291_param_t *ldd_param, uint32 *temp_sys);
extern void ddmi_8290c_diagnostic_temperature(uint32 devId, uint32 temp_sys); 
extern void ddmi_8290c_temperature(uint32 devId, rtl8291_param_t *ldd_param);
extern void update_ddmi_8290c_vcc(uint32 devId, uint32 *vcc); 
extern void ddmi_8290c_diagnostic_vcc(uint32 devId, uint32 volt_sys); 
extern void ddmi_8290c_vcc(uint32 devId); 
extern void update_ddmi_8290c_tx_bias(uint32 devId, uint32 *bias_sys); 
extern void ddmi_8290c_diagnostic_tx_bias(uint32 devId, uint32 bias_sys); 
extern void ddmi_8290c_tx_bias(uint32 devId); 
extern void update_ddmi_8290c_tx_power(uint32 devId, rtl8291_param_t *ldd_param, uint32 *tx_power); 
extern void ddmi_8290c_diagnostic_tx_power(uint32 devId, uint32 power_sys); 
extern void ddmi_8290c_tx_power(uint32 devId, rtl8291_param_t *ldd_param);
extern void update_ddmi_8290c_rx_power(uint32 devId, rtl8291_param_t *ldd_param, uint32 *rx_power);
extern void ddmi_8290c_diagnostic_rx_power(uint32 devId, uint32 power_sys);
extern void ddmi_8290c_rx_power(uint32 devId, rtl8291_param_t *ldd_param);

#if 0

//extern void _rxpwr_offset(int16 *pow1, int16 *pow2);

extern void get_internal(uint32 devId) ;
//extern void set_temperature_threshold(void) ;
extern int32 ddmi_temp_threshold_set(void);
extern int32 ddmi_volt_threshold_set(uint16 internal);
extern void ddmi_reg_move(uint16 i2cAddr, uint16 addrStart, uint16 addrEnd);

extern void compare_imod_threshold(void) ;

extern void ddmi_tx_bias(uint16 chip_type) ;
extern void ddmi_tx_power(uint16 chip_type) ;
extern void ddmi_rx_power(void) ;

extern void ddmi_intr_handle(uint16 status) ;

extern uint32 update_ddmi_tx_bias(uint16 internal, uint16 chip_type) ;
extern void ddmi_diagnostic_monitor_tx_bias(uint32 bias_sys) ;
extern void update_ddmi_tx_power(void ) ;
extern void ddmi_diagnostic_monitor_tx_power(uint32 power_sys) ;
extern void update_ddmi_rx_power(void) ;
extern void ddmi_diagnostic_monitor_rx_power(uint32 power_sys) ;
#endif
#endif
