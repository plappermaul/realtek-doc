#ifndef __DAL_RTL9601B_HWMISC_H_
#define __DAL_RTL9601B_HWMISC_H_

/****************************************************************/
/* Header File inclusion                                        */
/****************************************************************/

#include <dal/rtl9601b/dal_rtl9601b.h>
#include <rtk/debug.h>

/****************************************************************/
/* Constant Definition                                          */
/****************************************************************/



/****************************************************************/
/* Type Definition                                              */
/****************************************************************/

extern int32 rtl9601b_hsbData_get(rtk_hsb_t *hsbData);
extern int32 rtl9601b_hsaData_get(rtk_hsa_t *hsaData);
extern int32 rtl9601b_hsdData_get(rtk_hsa_debug_t *hsdData);
extern int32 rtl9601b_hsbData_set(rtk_hsb_t *hsbData);
#endif /*#ifndef __DAL_RTL9601B_HWMISC_H_*/

