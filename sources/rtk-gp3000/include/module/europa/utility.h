#ifndef __EUROPA_UTILITY_H__
#define __EUROPA_UTILITY_H__

#include <module/europa/europa.h>

#define DELAY_TIME                                             2000

//extern void drv_setAsicRegBit(uint16 reg, uint16 rBit, uint16 value);
//extern void drv_getAsicRegBit(uint16 reg, uint16 rBit, uint16 *pValue);
//extern void drv_setAsicRegBits(uint16 reg, uint16 rBits, uint16 value);
//extern void drv_getAsicRegBits(uint16 reg, uint16 rBits, uint16 *pValue);
extern void ddmi_reg_move(uint16 i2cAddr, uint16 addrStart, uint16 addrEnd);
extern void busy_loop(uint16 loopCnt);
extern uint8 check_all_one(uint32 reg, uint8 start, uint8 length);
extern uint32 volt_trans_func_1(uint16 internal);
extern uint32 impd_trans_func_1(uint16 internal, uint32 power_ddmi) ;
extern void set_impd_threshold(uint16 internal, uint32 Resister, uint32 Gain);
extern void tx_performanceVlue_set(void);
extern void apc_performanceVlueDCL_set(void);
extern void apc_performanceVlueSCL_set(void);

extern void bubble_sort(uint16 iarr[], uint8 num) ;
extern uint16 get_temperature(void);
//extern uint32 volt_trans_tx_power(uint32 ref_volt);
//extern uint32 volt_trans_rx_power(uint32 ref_volt);
extern void get_rssi_voltage(europa_volt_para_t *rssi_para);
extern uint64 cal_rssi_voltage(uint16 count);


#endif /* End of __UTILITY_H__ */
