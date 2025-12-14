#ifndef __CA_COMMON_H__
#define __CA_COMMON_H__

#define CA_MAX_PKT_SIZE				(1600)		/*1900Bytes when FEC is enabled */
#define CA_MIN_PKT_SIZE				(64)

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

typedef struct {/*CODEGEN_IGNORE_TAG*/
	ca_uint8_t	*data;		/* Pointer to a block of packet data */
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
	CA_PKT_TYPE_DHCPV6,		/* */
	CA_PKT_TYPE_PPPOE_DIS,		/* PPPOE discovery, Ethertype == 16'h8863 */
	CA_PKT_TYPE_PPPOE_SES,		/* PPPOE session, Ethertype == 16'h8864 */
	CA_PKT_TYPE_IROS_HELLO,		/* (Ethertype == 16'hFFFF) & (MAC_DA == CSR configured HELLO_MAC_ADDR) */
	CA_PKT_TYPE_ICMP,		/* (Ethertype == IPv4) & (IP_PROTOCOL == 8'h01) */
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
	CA_PKT_TYPE_MAX
} ca_pkt_type_t;			/* CPU TX/RX packet type information */

typedef struct {/*CODEGEN_IGNORE_TAG*/
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
	struct ca_pkt_t		*next;			/* Next pointer supporting linked lists of 4 byte data intended for network engine */
	ca_uint16_t		sw_id[4];
} ca_pkt_t;

typedef enum {
	CA_RX_NOT_HANDLED,
	CA_RX_HANDLED,
	CA_RX_HANDLED_OWNED,
} ca_rx_handle_status_t;

#endif

