
#ifndef __RTL8367D_SMI_H__
#define __RTL8367D_SMI_H__

#include <rtk_types.h>
#include "rtk_error.h"
//#define MDC_MDIO_OPERATION
rtk_int32 rtl8367d_smi_read(rtk_uint32 mAddrs, rtk_uint32 *rData);
rtk_int32 rtl8367d_smi_write(rtk_uint32 mAddrs, rtk_uint32 rData);

#endif /* __RTL8367D_SMI_H__ */


