/*
 * Copyright (C) 2010 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 10455 $
 * $Date: 2010-06-25 18:27:53 +0800 (Fri, 25 Jun 2010) $
 *
 * Purpose : Export the public APIs in lower layer module
 *
 * Feature : Export the public APIs in lower layer module
 *
 */
 
#ifndef __PROTO_IGMP_TX_H__
#define __PROTO_IGMP_TX_H__

int32 mcast_snooping_tx(sys_nic_pkt_t * pkt, uint16 vid, uint32 length, sys_logic_portmask_t * portmask);

#endif
 
