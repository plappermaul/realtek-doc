/*
 * Copyright (C) 2011 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 *
 *
 * $Revision$
 * $Date$
 *
 * Purpose : GMac Driver OMCI Processor
 *
 * Feature : GMac Driver OMCI Processor
 *
 */

#ifndef __GPON_OMCI_H__
#define __GPON_OMCI_H__

#include <module/gpon/gpon.h>

extern rtk_port_t gpon_omciMirroringPort;
#if defined(OLD_FPGA_DEFINED)
int32 gpon_omci_rx_reg(gpon_dev_obj_t* obj);
#endif
int32 gpon_omci_tx(gpon_dev_obj_t* obj, rtk_gpon_omci_msg_t* omci);
int32 gpon_omci_rx(gpon_dev_obj_t* obj, rtk_gpon_omci_msg_t* omci);


#endif  /* __GPON_OMCI_H__ */

