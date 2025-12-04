/****************************************************************************
            Software License for Customer Use of Cortina Software
                          Grant Terms and Conditions

IMPORTANT NOTICE - READ CAREFULLY: This Software License for Customer Use
of Cortina Software ("LICENSE") is the agreement which governs use of
software of Cortina Access, Inc. and its subsidiaries ("CORTINA"),
including computer software (source code and object code) and associated
printed materials ("SOFTWARE").  The SOFTWARE is protected by copyright laws
and international copyright treaties, as well as other intellectual property
laws and treaties.  The SOFTWARE is not sold, and instead is only licensed
for use, strictly in accordance with this document.  Any hardware sold by
CORTINA is protected by various patents, and is sold but this LICENSE does
not cover that sale, since it may not necessarily be sold as a package with
the SOFTWARE.  This LICENSE sets forth the terms and conditions of the
SOFTWARE LICENSE only.  By downloading, installing, copying, or otherwise
using the SOFTWARE, you agree to be bound by the terms of this LICENSE.
If you do not agree to the terms of this LICENSE, then do not download the
SOFTWARE.

DEFINITIONS:  "DEVICE" means the Cortina Access(TM) SDK product.
"You" or "CUSTOMER" means the entity or individual that uses the SOFTWARE.
"SOFTWARE" means the Cortina Access(TM) SDK software.

GRANT OF LICENSE:  Subject to the restrictions below, CORTINA hereby grants
CUSTOMER a non-exclusive, non-assignable, non-transferable, royalty-free,
perpetual copyright license to (1) install and use the SOFTWARE for
reference only with the DEVICE; and (2) copy the SOFTWARE for your internal
use only for use with the DEVICE.

RESTRICTIONS:  The SOFTWARE must be used solely in conjunction with the
DEVICE and solely with Your own products that incorporate the DEVICE.  You
may not distribute the SOFTWARE to any third party.  You may not modify
the SOFTWARE or make derivatives of the SOFTWARE without assigning any and
all rights in such modifications and derivatives to CORTINA.  You shall not
through incorporation, modification or distribution of the SOFTWARE cause
it to become subject to any open source licenses.  You may not
reverse-assemble, reverse-compile, or otherwise reverse-engineer any
SOFTWARE provided in binary or machine readable form.  You may not
distribute the SOFTWARE to your customers without written permission
from CORTINA.

OWNERSHIP OF SOFTWARE AND COPYRIGHTS. All title and copyrights in and the
SOFTWARE and any accompanying printed materials, and copies of the SOFTWARE,
are owned by CORTINA. The SOFTWARE protected by the copyright laws of the
United States and other countries, and international treaty provisions.
You may not remove any copyright notices from the SOFTWARE.  Except as
otherwise expressly provided, CORTINA grants no express or implied right
under CORTINA patents, copyrights, trademarks, or other intellectual
property rights.

DISCLAIMER OF WARRANTIES. THE SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY
EXPRESS OR IMPLIED WARRANTY OF ANY KIND, INCLUDING ANY IMPLIED WARRANTIES
OF MERCHANTABILITY, NONINFRINGEMENT, OR FITNESS FOR A PARTICULAR PURPOSE,
TITLE, AND NON-INFRINGEMENT.  CORTINA does not warrant or assume
responsibility for the accuracy or completeness of any information, text,
graphics, links or other items contained within the SOFTWARE.  Without
limiting the foregoing, you are solely responsible for determining and
verifying that the SOFTWARE that you obtain and install is the appropriate
version for your purpose.

LIMITATION OF LIABILITY. IN NO EVENT SHALL CORTINA OR ITS SUPPLIERS BE
LIABLE FOR ANY DAMAGES WHATSOEVER (INCLUDING, WITHOUT LIMITATION, LOST
PROFITS, BUSINESS INTERRUPTION, OR LOST INFORMATION) OR ANY LOSS ARISING OUT
OF THE USE OF OR INABILITY TO USE OF OR INABILITY TO USE THE SOFTWARE, EVEN
IF CORTINA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
TERMINATION OF THIS LICENSE. This LICENSE will automatically terminate if
You fail to comply with any of the terms and conditions hereof. Upon
termination, You will immediately cease use of the SOFTWARE and destroy all
copies of the SOFTWARE or return all copies of the SOFTWARE in your control
to CORTINA.  IF you commence or participate in any legal proceeding against
CORTINA, then CORTINA may, in its sole discretion, suspend or terminate all
license grants and any other rights provided under this LICENSE during the
pendency of such legal proceedings.
APPLICABLE LAWS. Claims arising under this LICENSE shall be governed by the
laws of the State of California, excluding its principles of conflict of
laws.  The United Nations Convention on Contracts for the International Sale
of Goods is specifically disclaimed.  You shall not export the SOFTWARE
without first obtaining any required export license or other approval from
the applicable governmental entity, if required.  This is the entire
agreement and understanding between You and CORTINA relating to this subject
matter.
GOVERNMENT RESTRICTED RIGHTS. The SOFTWARE is provided with "RESTRICTED
RIGHTS." Use, duplication, or disclosure by the Government is subject to
restrictions as set forth in FAR52.227-14 and DFAR252.227-7013 et seq. or
its successor. Use of the SOFTWARE by the Government constitutes
acknowledgment of CORTINA's proprietary rights therein. Contractor or
Manufacturer is CORTINA.

Copyright (c) 2015 by Cortina Access Incorporated
****************************************************************************/
#ifndef __RX_H__
#define __RX_H__

#include "ca_types.h"

#define CA_MAX_PKT_SIZE				(1600)		/*1900Bytes when FEC is enabled */
#define CA_MIN_PKT_SIZE				(64)

/* Cortina defined ether type for Saturn */
#define CA_PUC_GLOBAL_LNK_TYPE	0xfff1		/* OMCI */
#define CA_PUC_GLOBAL_MAC_TYPE 	0xfff0		/* PLOAM */
#if 1
#include "ca_common.h"
#else
/* Cortina Time Sync Message type Definition */
#define CA_TSYNC_PTP_SYNC			0x0		/* PTP message type Sync  */
#define CA_TSYNC_PTP_DELAY_REQ			0x1		/* PTP message type Delay_Req */
#define CA_TSYNC_PTP_PDELAY_REQ			0x2		/* PTP message type Pdelay_Req */
#define CA_TSYNC_PTP_PDELAY_RESP		0x3		/* PTP message type Pdelay_Resp */
#define CA_TSYNC_PTP_FOLLOW_UP			0x8		/* PTP message type Follow_Up */
#define CA_TSYNC_PTP_DELAY_RESP			0x9		/* PTP message type Delay_Resp */
#define CA_TSYNC_PTP_PDELAY_RESP_FOLLOW_UP	0xA		/* PTP message type Pdelay_Resp_Follow_Up */
#define CA_TSYNC_PTP_ANNOUNCE			0xB		/* PTP message type Announce */
#define CA_TSYNC_PTP_SIGNALING			0xC		/* PTP message type Signaling */
#define CA_TSYNC_PTP_MANAGEMENT			0xD		/* PTP message type Management */
#define CA_TSYNC_CFM_1DM			0x45		/* CFM message OpCode 1DM */
#define CA_TSYNC_CFM_DMR			0x46		/* CFM message OpCode DMR */
#define CA_TSYNC_CFM_DMM			0x47		/* CFM message OpCode DMM */

/* Cortina Packet RX Callback Flags Definition */
#define CA_RX_CB_FLAG_INTR			(0x1<<0)	/* Flag to indicate that the callback should be made in interrupt context. 0: callback should be made in non-interrupt context; 1: Callback should be made in interrupt context */

typedef ca_int32_t (/*CODEGEN_IGNORE_TAG*/*ca_pkt_blk_cb)(void *arg);

typedef struct {
	ca_uint8_t	*data/*[1024:1024]*/;		/* Pointer to a block of packet data */
	ca_uint32_t	len;		/* The length of this block in bytes, not including FCS */
	ca_pkt_blk_cb	done_fn;	/* The callback function called when use of this block data is complete  */
	void		*cookie;	/* Application data passed to the callback function */
} ca_pkt_blk_t;

typedef enum {
	CA_PKT_TYPE_BPDU,		/* BPDU packet */
	CA_PKT_TYPE_DOT1X,		/* 802.1x packet */
	CA_PKT_TYPE_IGMP,		/* IGMP packet */
	CA_PKT_TYPE_ARP,		/* (Ethertype == 16'h0806) & (ARP_opcode == 1|2) */
	CA_PKT_TYPE_OAM,		/* (Ethertype == 16'h8809) & (MAC_DA == 48'h0180C2000002) */
	CA_PKT_TYPE_OMCI,		/* OMCI frame */
	CA_PKT_TYPE_PLOAM,		/* PLOAM frame */
	CA_PKT_TYPE_MPCP,		/* Ethertype == 16'h8808 */
	CA_PKT_TYPE_DHCP,		/* (Ethertype == 16'h0800) & (IP_PROTOCOL == 8'h11) & (L4_Dest_Port == 16'h43 | 16'h44) */
	CA_PKT_TYPE_DHCPV6,		/* DHCPv6 */
	CA_PKT_TYPE_PPPOE_DIS,		/* PPPOE discovery, Ethertype == 16'h8863 */
	CA_PKT_TYPE_PPPOE_SES,		/* PPPOE session, Ethertype == 16'h8864 */
	CA_PKT_TYPE_IROS_HELLO,		/* (Ethertype == 16'hFFFF) & (MAC_DA == CSR configured HELLO_MAC_ADDR) */
	CA_PKT_TYPE_ICMP,		/* (Ethertype == IPv4) & (IP_PROTOCOL == 8'h01) OR (IPv6_next_header == 58) & (!IPv6 NDP) & (!MLD) */
	CA_PKT_TYPE_ICMPV6_MLD,		/* MLD packet */
	CA_PKT_TYPE_ICMPV6_NDP,		/* NDP packet */
	CA_PKT_TYPE_CFM,		/* CFM, ITU Y1731 packets */
	CA_PKT_TYPE_L2_PTP,		/* IEEE1558 */
	CA_PKT_TYPE_L4_PTP,		/* IEEE1558 */
	CA_PKT_TYPE_HTTP_GET,		/* HTTP request message */
	CA_PKT_TYPE_HTTP_PUT,		/* HTTP request message */
	CA_PKT_TYPE_HTTP_HEAD,		/* HTTP request message */
	CA_PKT_TYPE_HTTP_POST,		/* HTTP request message */
	CA_PKT_TYPE_HTTP_OPTIONS,	/* HTTP request message */
	CA_PKT_TYPE_HTTP_DELETE,	/* HTTP request message */
	CA_PKT_TYPE_HTTP_TRACE,		/* HTTP request message */
	CA_PKT_TYPE_HTTP_CONNECT,	/* HTTP request message */
	CA_PKT_TYPE_HTTP_UNLINK,	/* HTTP request message */
	CA_PKT_TYPE_HTTP_PATCH,		/* HTTP request message */
	CA_PKT_TYPE_DNS,		/* TCP/UDP port 53 */
	CA_PKT_TYPE_RIP,		/* UDP port 520 */
	CA_PKT_TYPE_SSDP,		/* Simple Service Discovery Protocol:MC+UDP-1900/2869/5000 */
	CA_PKT_TYPE_CONTROL_ALL,	/* All control packet types specified above */
	CA_PKT_TYPE_MIRROR,
	CA_PKT_TYPE_LOOPBACK,
	CA_PKT_TYPE_OTHERS,		/* frames not defined as specal packets */
	CA_PKT_TYPE_MAX,
} ca_pkt_type_t;			/* CPU TX/RX packet type information */

typedef struct ca_pkt_s {
	ca_pkt_blk_t		*pkt_data;		/* Pointer to packet data blocks */
	ca_uint32_t		blk_count;		/* Number of packet data blocks */
	ca_device_id_t		device_id;		/* For transmit, this is device identifieron which the packet is transmitted;
							   For receive, this is device identifier on which the packet is received */
	ca_port_id_t		src_port_id;		/* For transmit, this is port identifier
							   from which the packet is transmitted,normally, it's CPU port;
							   For receive, this is port identifier on which the packet is received */
	ca_port_id_t		src_sub_port_id;	/* For transmit, it does not make sense;
							   For receive, this is sub port identifier on which the packet is received. Take two examples:
							   For GPON ONU, this contains the GEM port information when the packet is received from GPON port;
							   For EPON ONU, if the EPON ONU supports multiple LLIDs, this contains the LLID information when the packet is received from EPON port */
	ca_port_id_t		dst_port_id;		/* For transmit, it contains the destination
							   port identifier to which the packet is to be sent;
							   For receive, it does not make sense */
	ca_port_id_t		dst_sub_port_id;	/* For transmit, it contains the destination
							   sub port identifier to which the packet is to be sent; Take two examples:
							   For GPON ONU, this contains the GEM port information when the packet is sent to GPON port;
							   For EPON ONU, if the EPON ONU supports multiple LLIDs, this contains the LLID information when the packet is sent to EPON port.
							   For receive, it does not make sense; */
	ca_uint16_t		pkt_len;		/* Pure packet length without 4 byte FCS.
							   For transmit, it is ignored and only the
							   len members of ca_pkt_blk_t structure are used;
							   For receive, it's normal length of the packet in bytes, not including proprietary headers; */
	ca_uint8_t		cos;			/* CoS queue information, for special
							   hardware fast channels, generally CoS 8 is defined to use for transmit */
	ca_uint8_t		tsync_msg_type;		/* PTP or CFM message type, See Cortina Time Sync
							   Message Type Description */
	ca_uint8_t		tsync_timestamp_offset;	/* The offset from from the first byte of MAC
							   DA to the first byte of the timestamps field in bytes */
	ca_pkt_type_t		pkt_type;		/* For receive only, see Cortina Packet RX Type
							   Description */
	ca_uint32_t		flags;			/* Flags for the packet. See Cortina Packet Flags Description */
	void			*reserved;		/* Pointer to reserved data */
	struct ca_pkt_s		*next;			/* Next pointer supporting linked lists of 4 byte data intended for network engine */
	ca_uint16_t		sw_id[4];
} ca_pkt_t;

typedef enum {
	CA_RX_NOT_HANDLED,
	CA_RX_HANDLED,
	CA_RX_HANDLED_OWNED,
} ca_rx_handle_status_t;
#endif
typedef ca_uint32_t (/*CODEGEN_IGNORE_TAG*/*ca_rx_cb)(
			ca_device_id_t	device_id,
			ca_pkt_t	*pkt,
			void		*cookie);

ca_status_t ca_rx_register(/*CODEGEN_IGNORE_TAG*/
	CA_IN		ca_device_id_t		device_id,
	CA_IN		ca_pkt_type_t		type,
	CA_IN		ca_rx_cb		fn,
	CA_IN		ca_uint8_t		priority,
	CA_IN		void			*cookie,
	CA_IN		ca_uint32_t		flags);

ca_status_t ca_rx_unregister(/*CODEGEN_IGNORE_TAG*/
	CA_IN		ca_device_id_t	device_id,
	CA_IN		ca_pkt_type_t	type,
	CA_IN		ca_rx_cb	fn,
	CA_IN		ca_uint8_t	priority);

ca_status_t ca_rx_register_pkt(
        CA_IN ca_device_id_t       device_id,
        CA_IN ca_uint8_t        priority,
        CA_IN ca_pkt_type_t	type);

ca_status_t ca_rx_unregister_pkt(
        CA_IN ca_device_id_t       device_id,
        CA_IN ca_uint8_t        priority,
        CA_IN ca_pkt_type_t	type);


#endif /* __CPU_PORT_H__ */

