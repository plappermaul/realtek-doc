#ifndef __DAL_CA8279_H_
#define __DAL_CA8279_H_

#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_bitop.h>
#include <common/util/rt_util.h>
#include <common/debug/rt_log.h>
#include <osal/memory.h>

#include <dal/dal_common.h>
#include <hal/common/halctrl.h>

#include <rtk/default.h>

#include <ca_types.h>
#include <scfg.h>

extern ca_uint8_t CA_PON_MODE;

#define CAERR_PORT_ID 0xFFFF

#define RTK2CA_PORT_ID(port) \
	(HAL_IS_PON_PORT(port)?((CA_PON_MODE == ONU_PON_MAC_EPON_D10G||CA_PON_MODE == ONU_PON_MAC_EPON_BI10G||CA_PON_MODE == ONU_PON_MAC_EPON_1G) ? CA_PORT_ID(CA_PORT_TYPE_EPON,port):CA_PORT_ID(CA_PORT_TYPE_GPON,port)) : HAL_IS_ETHER_PORT(port)?CA_PORT_ID(CA_PORT_TYPE_ETHERNET,port) : HAL_IS_CPU_PORT(port)?CA_PORT_ID(CA_PORT_TYPE_CPU,port) : (port == 0x18 || port == 0x19) ? port : CAERR_PORT_ID)

#endif /*__DAL_CA8279_H_*/

