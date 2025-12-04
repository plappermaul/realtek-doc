#ifndef __EUROPA_INTERRUPT_H__
#define __EUROPA_INTERRUPT_H__

extern void laser_drv_intrEnable(uint16 mask1, uint16 mask2)  ;
extern void laser_drv_intrDisable(void);
extern void laser_drv_functionCtrl(europa_laser_fun_t fun, uint16 enable) ;
extern void laser_drv_faultReleaseIntrHandle(uint16 status) ;
extern void laser_drv_underTxDisFaultReleaseIntrHandle(void) ;
extern void laser_drv_underRxOverPowerFaultReleaseIntrHandle(void) ;
extern void rssi_err_intrHandle(void) ;
extern void rssi_err_intrRelease(void) ;
extern void laser_drv_intrHandle(void) ;

extern void laser_drv_ddmi_tx_dis_intrHandle(uint16 status2) ;
extern void laser_drv_tx_dis_intrHandle(uint16 status2) ;

extern void laser_drv_fault_status(void);

#endif