#ifndef __EUROPA_LASER_DRV_H__
#define __EUROPA_LASER_DRV_H__
extern void reset_8051(void)  ;
//extern void get_lut_addr(void)  ;
//extern void set_loop_mode(uint8 mode);
//extern void get_loop_mode(uint8 *pMode);
//extern void powerOnStatusCheck(void)  ;
//extern void update_laser_lut(apc_loop_mode_t mode)  ;
//extern void update_APD_lut(void)  ;
//extern void update_lookup_table(void) ;
extern void Check_C0_C1(void)  ;
//extern void rx_enable(rx_enable_fun_t func)  ;
//extern void laser_drv_kickWatchdog(void)  ;
extern void laser_drv_powerOnSequence_1(void)  ;
extern void laser_drv_powerOnSequence_2(void)  ;
extern void laser_drv_powerOnSequence_3(void)  ;
extern void europa_init(void);
#endif