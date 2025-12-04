/*
 * Copyright (c) Cortina-Access Limited 2015.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#ifndef __CA_NI_TX_H__
#define __CA_NI_TX_H__

#if 0//yocto
#include <linux/netdevice.h>
#else//sd1 uboot for 98f
#endif

#include "ca_ni_common.h"

#ifdef CA_BIG_ENDIAN
#define CRCPOLY_BE                                              0x04c11db7
#else /* CA_LITTLE_ENDIAN */
#define CRCPOLY_LE                                              0xedb88320
#endif

//#define CA_NI_DMA_LSO_TXQ_NUM		6       /* Maximum is 8 */
#define CA_NI_DMA_LSO_TXQ_IDX		0
#define CA_NI_DMA_LSO_CA_TX_TXQ_IDX	7	/* TX queue ID 7 has highest priority */
#define CA_PE_DMA_LSO_TXQ_IDX		6

#define CA_NI_RWPTR_ADVANCE_ONE(x, max)       ((x == (max -1)) ? 0 : x+1)

/* struct used for Management mode */
struct ca_ni_priv {
        ca_uint64  rx_xram_base_addr;
        ca_uint64  rx_xram_end_addr;
        ca_uint32  rx_xram_start;
        ca_uint32  rx_xram_end;
        ca_uint64  tx_xram_base_addr;
        ca_uint64  tx_xram_end_addr;
        ca_uint32  tx_xram_start;
        ca_uint32  tx_xram_end;
};

typedef volatile union {
  struct {
    ca_uint32 next_link            : 10 ; /* bits  9: 0 */
    ca_uint32 bytes_valid          :  4 ; /* bits 13:10 */
    ca_uint32 reserved             : 16 ; /* bits 29:14 */
    ca_uint32 hdr_a                :  1 ; /* bits 30:30 */
    ca_uint32 ownership            :  1 ; /* bits 31:31 */
  } bf ;
  ca_uint32     wrd ;
} NI_HEADER_X_T;

typedef volatile union {
  struct {
    ca_uint32 packet_size       : 14 ; /* bits 13:0 */
    ca_uint32 byte_valid        :  4 ; /* bits 17:14 */
    ca_uint32 pfc               :  1 ; /* bits 18:18 */
    ca_uint32 valid             :  1 ; /* bits 19:19 */
    ca_uint32 drop              :  1 ; /* bits 20:20 */
    ca_uint32 runt              :  1 ; /* bits 21:21 */
    ca_uint32 oversize          :  1 ; /* bits 22:22 */
    ca_uint32 jumbo             :  1 ; /* bits 23:23 */
    ca_uint32 link_status       :  1 ; /* bits 24:24 */
    ca_uint32 jabber            :  1 ; /* bits 25:25 */
    ca_uint32 crc_error         :  1 ; /* bits 26:26 */
    ca_uint32 pause             :  1 ; /* bits 27:27 */
    ca_uint32 oam               :  1 ; /* bits 28:28 */
    ca_uint32 unknown_opcode    :  1 ; /* bits 29:29 */
    ca_uint32 multicast         :  1 ; /* bits 30:30 */
    ca_uint32 broadcast         :  1 ; /* bits 31:31 */
  } bf ;
  ca_uint32     wrd ;
} NI_PACKET_STATUS_T;

typedef union {
        u32 wrd;
        struct bit_9009c {
                u32 rptr                : 13;   /* bits 12:0  */
                u32 reserved            : 19;   /* bits 31:13 */
        } bf;
} dma_rptr_t;

/*
 * DMA LSO Tx Description Word 0
 */

#define CA_NI_DMA_LSO_HEADER_A_EXIST	0x81000000	/* HP1:HP0=01 header_a exists, 10 header_a and header_cpu exists */
#define CA_NI_DMA_LSO_SOF_BIT         	0x40000000      /* 10: first descriptor */
#define CA_NI_DMA_LSO_EOF_BIT         	0x20000000      /* 01: last descriptor  */
#define CA_NI_DMA_LSO_ONE_BIT         	0x60000000      /* 11: only one descriptor */
#define CA_NIDMA_LSO_LINK_BIT        	0x00000000      /* 00: link descriptor  */

#define CA_NI_DMA_LSO_SOF           	0x2	/* 10: first descriptor */
#define CA_NI_DMA_LSO_EOF 		0x1	/* 01: last descriptor  */

typedef union {
        u32 wrd;
        struct bit_038 {
                u32 buf_addr;                   /* bits 31:0 */
        } bf;
} dma_txdesc_0_t;

typedef union {
        u32 wrd;
        struct bit_03c {
                u32 buf_addr_39_32      : 8;    /* bits 7:0 */
                u32 buf_len             : 16;   /* bits 23:8 */
                u32 hp0                 : 1;    /* bits 24 */
                u32 fbm                 : 1;    /* bits 25 */
                u32 fbm_pool_id         : 3;    /* bits 28:26 */
                u32 sop_eop             : 2;    /* bits 30:29 */
                u32 hp1                 : 1;    /* bits 31 */
        } bf;
} dma_txdesc_1_t;

#ifndef __BIT
#define __BIT(x)                (1 << (x))
#endif
#define CA_NI_LSO_SEGMENT_EN          __BIT(25)
#define CA_NI_LSO_IPV4_FRAGMENT_EN    __BIT(26)
#define CA_NI_LSO_IPV6_FREGMENT_EN    __BIT(27)
#define CA_NI_LSO_TCP_CHECKSUM_EN     __BIT(28)
#define CA_NI_LSO_UDP_CHECKSUM_EN     __BIT(29)
#define CA_NI_LSO_BYPASS_EN           __BIT(30)
#define CA_NI_LSO_IP_LENFIX_EN        __BIT(31)

typedef union {
        u32 wrd;
        struct bit_040 {
                u32 segment_size        : 14;    /* bits 13:0 */
                u32 reserved            : 10;    /* bits 23:14 */
                u32 unknown_tag_en      : 1;     /* bits 24:24 */
                u32 segment_en          : 1;     /* bits 25:25 */
                u32 ipv4_en             : 1;     /* bits 26:26 */
                u32 ipv6_en             : 1;     /* bits 27:27 */
                u32 tcp_en              : 1;     /* bits 28:28 */
                u32 udp_en              : 1;     /* bits 29:29 */
                u32 bypass_en           : 1;     /* bits 30:30 */
                u32 lenfix_en           : 1;     /* bits 31:31 */
        } bf;
} dma_lsopara_0_t;

typedef union {
	u32 wrd;
	struct bit_044 {
		u32 packet_size        : 16;     /* bits 15:0 */
		u32 reserved           : 16;     /* bits 31:16 */
	} bf;
} dma_lsopara_1_t;

typedef struct {
	dma_txdesc_0_t  word0;
	dma_txdesc_1_t  word1;
} dma_txdesc_t;

typedef struct {
	dma_lsopara_0_t lso_word0;
	dma_lsopara_1_t lso_word1;
	HEADER_A_T	header_a;
	HEADER_CPU_T    header_cpu;
} dma_txbuf_t;

typedef ca_status_t (*ni_cb_fn_xmit_done)(
	void * cookie,	void * pkt);

typedef struct {
	u32 rptr_reg;
	u32 wptr_reg;
	dma_txdesc_t *desc_base;
	dma_addr_t   desc_dma_addr;
	dma_txbuf_t  *buf_base;
	dma_addr_t   buf_dma_addr;
	u32 total_desc_num;
	u16 wptr;
	unsigned short finished_idx;
	struct sk_buff *tx_skb[CA_NI_DMA_LSO_TXDESC_NUM];
	ni_cb_fn_xmit_done xmit_done_cb_fn[CA_NI_DMA_LSO_TXDESC_NUM];
	void *xmit_done_cookie[CA_NI_DMA_LSO_TXDESC_NUM];
	unsigned long total_finished;
	unsigned long intr_cnt;
	spinlock_t lock;
} dma_swtxq_t;

typedef union {
	ca_uint64 longlong;
	struct bit {
		ca_uint8_t txq_index;
		ca_uint8_t ldpid;
		ca_uint8_t lspid;
		ca_uint8_t is_from_ca_tx:1;
		ca_uint8_t bypass_fwd_engine:1;
		ca_uint8_t cb_fn_mask:1;
		ca_uint8_t no_hdr_cpu:1;
		ca_uint8_t flow_id_set:1;
		ca_uint32_t sw_id;
	} bf;
} ni_tx_core_config_t;

typedef struct {
	ca_uint32_t flow_id;
	ni_tx_core_config_t core_config;
	ni_cb_fn_xmit_done xmit_done_cb_fn;
	void * cb_cookie;
} ca_ni_tx_config_t;

#if 0//yocto
#else//sd1 uboot for 98f - linux/netdevice.h
#define INT_MAX		((int)(~0U>>1))
#define INT_MIN		(-INT_MAX - 1)

enum netdev_tx {
	__NETDEV_TX_MIN	 = INT_MIN,	/* make sure enum is signed */
	NETDEV_TX_OK	 = 0x00,	/* driver took care of packet */
	NETDEV_TX_BUSY	 = 0x10,	/* driver tx path was busy*/
	NETDEV_TX_LOCKED = 0x20,	/* driver tx lock was already taken */
};
typedef enum netdev_tx netdev_tx_t;
#endif

netdev_tx_t __ca_ni_start_xmit(struct sk_buff *skb, struct net_device *dev, ca_ni_tx_config_t *tx_config);
void ca_ni_init_tx_dma_lso(struct platform_device *pdev);
void ca_ni_cleanup_tx_dma_lso(struct platform_device *pdev);
void ca_ni_tx_completion_timer_cb(unsigned long data);
void cancel_tx_completion_timer(struct net_device *dev);
void ca_ni_reset_tx_ring(void);
int ca_ni_mgmt_start_xmit(struct sk_buff *skb, struct net_device *dev, int txq_index, int ldpid);
int ca_ni_mgmt_send_packet(u8 *pkt_data, int length);
netdev_tx_t ca_ni_start_xmit_buf(void * buf0, int len0, void * buf1, int len1,
	struct sk_buff *skb, ca_ni_tx_config_t *tx_config);

#endif /* __CA_NI_TX_H__ */

