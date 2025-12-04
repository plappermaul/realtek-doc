#ifndef __EUROPA_DDMI_H__
#define __EUROPA_DDMI_H__


#define INTERNAL_DDMI_A0_REG_START_ADDRESS 0
#define INTERNAL_DDMI_A2_REG_START_ADDRESS 256

extern void get_internal(void) ;
//extern void set_temperature_threshold(void) ;
extern int32 ddmi_temp_threshold_set(void);
extern int32 ddmi_volt_threshold_set(uint16 internal);
extern void ddmi_reg_move(uint16 i2cAddr, uint16 addrStart, uint16 addrEnd);

extern void compare_imod_threshold(void) ;
extern void ddmi_temperature(void) ;
extern void ddmi_vcc(void) ;
extern void ddmi_tx_bias(void) ;
extern void ddmi_tx_power(void) ;
extern void ddmi_rx_power(void) ;
extern void update_ddmi(void) ;
extern void ddmi_intr_handle(uint16 status) ;

extern int16 update_ddmi_temperature(uint16 internal) ;
extern void ddmi_diagnostic_monitor_temperature(int16 temp_sys) ;
extern uint32 update_ddmi_vcc(uint16 internal) ;
extern void ddmi_diagnostic_monitor_vcc(uint32 volt_sys) ;
extern uint32 update_ddmi_tx_bias(uint16 internal) ;
extern void ddmi_diagnostic_monitor_tx_bias(uint32 bias_sys) ;
extern void update_ddmi_tx_power(void ) ;
extern void ddmi_diagnostic_monitor_tx_power(uint32 power_sys) ;
extern void update_ddmi_rx_power(void) ;
extern void ddmi_diagnostic_monitor_rx_power(uint32 power_sys) ;

#endif
