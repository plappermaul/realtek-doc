#ifndef __DAL_RTL9602C_HWMISC_H_
#define __DAL_RTL9602C_HWMISC_H_

/****************************************************************/
/* Header File inclusion                                        */
/****************************************************************/
#include <dal/rtl9602c/dal_rtl9602c.h>
#include <rtk/debug.h>

/****************************************************************/
/* Constant Definition                                          */
/****************************************************************/
#define RTL9602C_VIR_MAC_DUMY_CYCLE  30

#define RTL9602C_VIR_MAC_TX_MAX_CNT  2000


/****************************************************************/
/* Type Definition                                              */
/****************************************************************/
extern int32 rtl9602c_virtualMacMode_Set(rtk_port_t port, rtk_enable_t enable);
extern int32 rtl9602c_virtualMac_Input(rtk_port_t fromPort, uint8 *pPkt, uint32 len);
extern int32 rtl9602c_virtualMac_Output(rtk_port_t toPort, uint8 *pPkt, uint32 *plen);
extern int32 rtl9602c_hsbData_get(rtk_hsb_t *hsbData);
extern int32 rtl9602c_hsaData_get(rtk_hsa_t *hsaData);
extern int32 rtl9602c_hsdData_get(rtk_hsa_debug_t *hsdData);
extern int32 rtl9602c_hsbData_set(rtk_hsb_t *hsbData);
extern int32 rtl9602c_hsbPar_get(rtk_hsb_t *hsbData);
extern int32 rtl9602c_config_set(void);
extern int32 rtl9602c_config_get(void);


#endif /*#ifndef __DAL_RTL9602C_HWMISC_H_*/

