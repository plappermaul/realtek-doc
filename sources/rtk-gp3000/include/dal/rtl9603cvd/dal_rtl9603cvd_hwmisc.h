#ifndef __DAL_RTL9603CVD_HWMISC_H_
#define __DAL_RTL9603CVD_HWMISC_H_

/****************************************************************/
/* Header File inclusion                                        */
/****************************************************************/
#include <dal/rtl9603cvd/dal_rtl9603cvd.h>
#include <rtk/debug.h>

/****************************************************************/
/* Constant Definition                                          */
/****************************************************************/
#define RTL9603CVD_VIR_MAC_DUMMY_CYCLE  30

#define RTL9603CVD_VIR_MAC_TX_MAX_CNT  2000


/****************************************************************/
/* Type Definition                                              */
/****************************************************************/
extern int32 rtl9603cvd_virtualMacMode_Set(rtk_port_t port, rtk_enable_t enable);
extern int32 rtl9603cvd_virtualMac_Input(rtk_port_t fromPort, uint8 *pPkt, uint32 len);
extern int32 rtl9603cvd_virtualMac_Output(rtk_port_t toPort, uint8 *pPkt, uint32 *plen);
extern int32 rtl9603cvd_hsbData_get(rtk_hsb_t *hsbData);
extern int32 rtl9603cvd_hsaData_get(rtk_hsa_t *hsaData);
extern int32 rtl9603cvd_fbData_get(rtk_hsa_t *hsaData);
extern int32 rtl9603cvd_hsdData_get(rtk_hsa_debug_t *hsdData);
extern int32 rtl9603cvd_hsbPar_get(rtk_hsb_t *hsbData);

#endif /*#ifndef __DAL_RTL9603CVD_HWMISC_H_*/

