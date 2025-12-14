#ifndef __DAL_RTL9607C_HWMISC_H_
#define __DAL_RTL9607C_HWMISC_H_

/****************************************************************/
/* Header File inclusion                                        */
/****************************************************************/
#include <dal/rtl9607c/dal_rtl9607c.h>
#include <rtk/debug.h>

/****************************************************************/
/* Constant Definition                                          */
/****************************************************************/
#define RTL9607C_VIR_MAC_DUMMY_CYCLE  30

#define RTL9607C_VIR_MAC_TX_MAX_CNT  2000


/****************************************************************/
/* Type Definition                                              */
/****************************************************************/
extern int32 rtl9607c_virtualMacMode_Set(rtk_port_t port, rtk_enable_t enable);
extern int32 rtl9607c_virtualMac_Input(rtk_port_t fromPort, uint8 *pPkt, uint32 len);
extern int32 rtl9607c_virtualMac_Output(rtk_port_t toPort, uint8 *pPkt, uint32 *plen);
extern int32 rtl9607c_hsbData_get(rtk_hsb_t *hsbData);
extern int32 rtl9607c_hsaData_get(rtk_hsa_t *hsaData);
extern int32 rtl9607c_fbData_get(rtk_hsa_t *hsaData);
extern int32 rtl9607c_hsdData_get(rtk_hsa_debug_t *hsdData);
extern int32 rtl9607c_hsbPar_get(rtk_hsb_t *hsbData);

#if 0
extern int32 rtl9607c_hsbData_set(rtk_hsb_t *hsbData);
#endif
#endif /*#ifndef __DAL_RTL9607C_HWMISC_H_*/

