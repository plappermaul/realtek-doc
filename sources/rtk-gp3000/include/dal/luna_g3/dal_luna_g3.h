#ifndef __DAL_LUNA_G3_H_
#define __DAL_LUNA_G3_H_

#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_bitop.h>
#include <common/util/rt_util.h>
#include <common/debug/rt_log.h>
#include <osal/memory.h>

#include <dal/dal_common.h>
#include <hal/common/halctrl.h>

#include <rtk/default.h>

#define CAERR_PORT_ID 0xFFFF

#define RTK2CA_PORT_ID(port) \
	(HAL_IS_ETHER_PORT(port)?CA_PORT_ID(CA_PORT_TYPE_ETHERNET,port) : HAL_IS_CPU_PORT(port)?CA_PORT_ID(CA_PORT_TYPE_CPU,port) : (port == 0x18 || port == 0x19) ? port : CAERR_PORT_ID)

#endif /*__DAL_LUNA_G3_H_*/

