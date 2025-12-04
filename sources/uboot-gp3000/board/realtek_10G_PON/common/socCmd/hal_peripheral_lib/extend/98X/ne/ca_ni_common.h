#ifndef __CA_NI_COMMON_H__
#define __CA_NI_COMMON_H__

typedef enum {
        HEADER_TYPE_UC = 0,
        HEADER_TYPE_MC,
        HEADER_TYPE_CPU_BOUND,
        HEADER_TYPE_PTP
} ca_ni_hdr_type_t;

/*
 * Special version of lists, where end of list is not a NULL pointer,
 * but a 'nulls' marker, which can have many different values.
 * (up to 2^31 different values guaranteed on all platforms)
 *
 * In the standard hlist, termination of a list is the NULL pointer.
 * In this special 'nulls' variant, we use the fact that objects stored in
 * a list are aligned on a word (4 or 8 bytes alignment).
 * We therefore use the last significant bit of 'ptr' :
 * Set to 1 : This is a 'nulls' end-of-list marker (ptr >> 1)
 * Set to 0 : This is a pointer to some object (ptr)
 */
struct hlist_nulls_node {
        struct hlist_nulls_node *next, **pprev;
};

struct hlist_nulls_head {
        struct hlist_nulls_node *first;
};

/* Map to Mdata field of Header_I; host order. */
typedef union {
	uint64_t bits64;
	struct {
		uint32_t mdata_h		: 32;
		uint32_t lpm_result		: 12;	/* lpm_result key to NHOP */
		uint32_t reserved_0		: 4;
		uint32_t l3_egress_if_id	: 8;	/* L3_egress_IF_ID */
		uint32_t l3_ingress_if_id	: 8;	/* L3_ingress_IF_ID */
	} __attribute__((packed)) mdata_uc;
	struct {
		uint32_t mdata_h		: 32;
		uint32_t mc_fib_id		: 8;	/* FIB ID */
		uint32_t reserved_0		: 8;
		uint32_t l3_egress_if_id	: 8;	/* L3_egress_IF_ID */
		uint32_t l3_ingress_if_id	: 8;	/* L3_ingress_IF_ID */
	} __attribute__((packed)) mdata_mc;
	struct {
		uint32_t hash_dbl_chk_fail	: 3;
		uint32_t hash_idx		: 21;
		uint32_t cls_action		: 12;
		uint32_t fwd_vld		: 9;
		uint32_t l2l3_flag		: 5;
		uint32_t l4_offset		: 7;
		uint32_t l3_offset		: 7;
	} __attribute__((packed)) hdr_i_info;
	struct {
		uint32_t mdata_h		: 32;
		uint32_t mdata_l		: 32;
	} __attribute__((packed)) mdata_raw;
} HEADER_CPU_T;

typedef union {
        uint32_t bits32[4];
        struct header_ptp {
                uint64_t correction_l           : 32;   /*
                                                         * When cmd_mode is 0, this field is 0
                                                         * When cmd_mode is 1, this is part of 64 bit correction field
                                                         */
                uint64_t correction_h           : 32;   /*
                                                         * When cmd_mode is 0, this is the ns part of the TimeStamp in packet
                                                         * When cmd_mode is 1, this is part of 64 bit correction field
                                                         */
                uint64_t rx_time_stamp          : 32;   /* PTP timer time latched by ingress PTP process packet SOP.
                                                         * LSB align to 16ns
                                                         */
                uint64_t cmd_op                 : 2;    /* SW command:
                                                         * 00: no op
                                                         * 01: timestamp correct
                                                         * 10: timestamp and asymmetry with (+) sign
                                                         * 11: timestamp and asymmetry with (-) sign
                                                         */
                uint64_t cmd_asel               : 2;    /* SW Command Asymmetry Offset select */
                uint64_t cmd_mode               : 1;    /* If set, 32-bit timestamp is filled to the field starting with sw_cmd_offset.
                                                         * Software is expected to provide 48-bit timer second field.
                                                         * When this bit is 0, correction field is calculated, and saved to field start
                                                         * with sw_cmd_offset.
                                                         */
                uint64_t cmd_rewrite            : 1;    /* If set enables the packet CorrectionField/TimeStamp to be rewritten */
                uint64_t ptp_type               : 2;    /* Indicated PTP type:
                                                         * 00: PTP over IEEE802.3 /Ethernet
                                                         * 01: CFM
                                                         * 10: PTP over UDP over IPv4
                                                         * 11: PTP over UDP over IPv6
                                                         */
                uint64_t udp_chksum_offset      : 7;    /* Valid when ptp_type indicates UDP type.  Byte offset of the UDP checksum */
                uint64_t reserved_0             : 1;
                uint64_t cmd_offset             : 7;    /* Indicates position of correction field or timestamp in the packet */
                uint64_t reserved_1             : 9;
        } bits;
} HEADER_PTP_T;

#if 0//yocto
#else//sd1 uboot for 98f - linux/netdevice.h
#include <linux/ns_common.h>
#include <net/netns/core.h>
#include <net/netns/mib.h>
//#include <net/snmp.h>
#include <net/netns/packet.h>
#include <net/netns/unix.h>
#include <net/netns/ipv4.h>

#if 0//debug - TBD
typedef __u32 __bitwise __portpair;

struct hrtimer_clock_base;

#ifndef __HAS_RB_NODE
#define __HAS_RB_NODE
struct rb_node {
        unsigned long  __rb_parent_color;
        struct rb_node *rb_right;
        struct rb_node *rb_left;
} __attribute__((aligned(sizeof(long))));
/* The alignment might seem pointless, but allegedly CRIS needs it */

struct rb_root {
        struct rb_node *rb_node;
};
#endif /* __HAS_RB_NODE */

#define IFNAMSIZ        16
#define MAX_ADDR_LEN    32      /* Largest hardware address length */

typedef __u64 __bitwise __addrpair;

typedef u64     netdev_features_t;

#if BITS_PER_LONG > 32
#define NET_SKBUFF_DATA_USES_OFFSET 1
#endif

typedef struct {
	long counter;
} atomic64_t;

#if BITS_PER_LONG == 64
typedef atomic64_t atomic_long_t;
#else /* BITS_PER_LONG == 32 */
typedef atomic_t atomic_long_t;
#endif

#ifndef L1_CACHE_BYTES
#define L1_CACHE_SHIFT		6
#define L1_CACHE_BYTES		BIT(L1_CACHE_SHIFT)
#endif

#ifndef SMP_CACHE_BYTES
#define SMP_CACHE_BYTES L1_CACHE_BYTES
#endif

#ifndef ____cacheline_aligned
#define ____cacheline_aligned __attribute__((__aligned__(SMP_CACHE_BYTES)))
#endif

#ifndef ____cacheline_aligned_in_smp
#ifdef CONFIG_SMP
#define ____cacheline_aligned_in_smp ____cacheline_aligned
#else
#define ____cacheline_aligned_in_smp
#endif /* CONFIG_SMP */
#endif

#if 0
struct work_struct;
typedef void (*work_func_t)(struct work_struct *work);

struct work_struct {
        atomic_long_t data;
        struct list_head entry;
        work_func_t func;
#ifdef CONFIG_LOCKDEP
        struct lockdep_map lockdep_map;
#endif
};
#endif

#ifdef NET_SKBUFF_DATA_USES_OFFSET
typedef unsigned int sk_buff_data_t;
#else
typedef unsigned char *sk_buff_data_t;
#endif

#ifdef CONFIG_64BIT
# define HRTIMER_CLOCK_BASE_ALIGN       64
#else
# define HRTIMER_CLOCK_BASE_ALIGN       32
#endif

#define DUPLEX_FULL     0x01
#define FLOW_CTRL_TX    0x01
#define FLOW_CTRL_RX    0x02

#define TC_MAX_QUEUE    16
#define TC_BITMASK      15

/*
 * Network interface message level settings
 */
enum {
        NETIF_MSG_DRV           = 0x0001,
        NETIF_MSG_PROBE         = 0x0002,
        NETIF_MSG_LINK          = 0x0004,
        NETIF_MSG_TIMER         = 0x0008,
        NETIF_MSG_IFDOWN        = 0x0010,
        NETIF_MSG_IFUP          = 0x0020,
        NETIF_MSG_RX_ERR        = 0x0040,
        NETIF_MSG_TX_ERR        = 0x0080,
        NETIF_MSG_TX_QUEUED     = 0x0100,
        NETIF_MSG_INTR          = 0x0200,
        NETIF_MSG_TX_DONE       = 0x0400,
        NETIF_MSG_RX_STATUS     = 0x0800,
        NETIF_MSG_PKTDATA       = 0x1000,
        NETIF_MSG_HW            = 0x2000,
        NETIF_MSG_WOL           = 0x4000,
};

/*
 * We want shallower trees and thus more bits covered at each layer.  8
 * bits gives us large enough first layer for most use cases and maximum
 * tree depth of 4.  Each idr_layer is slightly larger than 2k on 64bit and
 * 1k on 32bit.
 */
#define IDR_BITS 8
#define IDR_SIZE (1 << IDR_BITS)
#define IDR_MASK ((1 << IDR_BITS)-1)

//#define rcu_head callback_head

struct idr_layer {
        int                     prefix; /* the ID prefix of this idr_layer */
        int                     layer;  /* distance from leaf */
        struct idr_layer __rcu  *ary[1<<IDR_BITS];
        int                     count;  /* When zero, we can release it */
        union {
                /* A zero bit means "space here" */
                DECLARE_BITMAP(bitmap, IDR_SIZE);
                struct rcu_head         rcu_head;
        };
};

struct idr {
        struct idr_layer __rcu  *hint;  /* the last layer allocated from */
        struct idr_layer __rcu  *top;
        int                     layers; /* only valid w/o concurrent changes */
        int                     cur;    /* current pos for cyclic allocation */
        spinlock_t              lock;
        int                     id_free_cnt;
        struct idr_layer        *id_free;
};

#if 0
struct hlist_node {
	struct hlist_node *next, **pprev;
};
#endif

struct sk_buff_head;
struct sock;
//struct list_head;

struct net {
        atomic_t                passive;        /* To decided when the network
                                                 * namespace should be freed.
                                                 */
        atomic_t                count;          /* To decided when the network
                                                 *  namespace should be shut down.
                                                 */
        spinlock_t              rules_mod_lock;

        atomic64_t              cookie_gen;

        struct list_head        list;           /* list of network namespaces */
        struct list_head        cleanup_list;   /* namespaces on death row */
        struct list_head        exit_list;      /* Use only net_mutex */

        struct user_namespace   *user_ns;       /* Owning user namespace */
        spinlock_t              nsid_lock;
        struct idr              netns_ids;

        struct ns_common        ns;

        struct proc_dir_entry   *proc_net;
        struct proc_dir_entry   *proc_net_stat;

#ifdef CONFIG_SYSCTL
        struct ctl_table_set    sysctls;
#endif

        struct sock             *rtnl;                  /* rtnetlink socket */
        struct sock             *genl_sock;

        struct list_head        dev_base_head;
        struct hlist_head       *dev_name_head;
        struct hlist_head       *dev_index_head;
        unsigned int            dev_base_seq;   /* protected by rtnl_mutex */
        int                     ifindex;
        unsigned int            dev_unreg_count;

        /* core fib_rules */
        struct list_head        rules_ops;


        struct net_device       *loopback_dev;          /* The loopback */
        struct netns_core       core;
        struct netns_mib        mib;
        struct netns_packet     packet;
        struct netns_unix       unx;
        struct netns_ipv4       ipv4;
#if IS_ENABLED(CONFIG_IPV6)
        struct netns_ipv6       ipv6;
#endif
#if IS_ENABLED(CONFIG_IEEE802154_6LOWPAN)
        struct netns_ieee802154_lowpan  ieee802154_lowpan;
#endif
#if defined(CONFIG_IP_SCTP) || defined(CONFIG_IP_SCTP_MODULE)
        struct netns_sctp       sctp;
#endif
#if defined(CONFIG_IP_DCCP) || defined(CONFIG_IP_DCCP_MODULE)
        struct netns_dccp       dccp;
#endif
#ifdef CONFIG_NETFILTER
        struct netns_nf         nf;
        struct netns_xt         xt;
#if defined(CONFIG_NF_CONNTRACK) || defined(CONFIG_NF_CONNTRACK_MODULE)
        struct netns_ct         ct;
#endif
#if defined(CONFIG_NF_TABLES) || defined(CONFIG_NF_TABLES_MODULE)
        struct netns_nftables   nft;
#endif
#if IS_ENABLED(CONFIG_NF_DEFRAG_IPV6)
        struct netns_nf_frag    nf_frag;
#endif
        struct sock             *nfnl;
        struct sock             *nfnl_stash;
#if IS_ENABLED(CONFIG_NETFILTER_NETLINK_ACCT)
        struct list_head        nfnl_acct_list;
#endif
#endif
#ifdef CONFIG_WEXT_CORE
        struct sk_buff_head     wext_nlevents;
#endif
        struct net_generic __rcu        *gen;

        /* Note : following structs are cache line aligned */
#ifdef CONFIG_XFRM
        struct netns_xfrm       xfrm;
#endif
#if IS_ENABLED(CONFIG_IP_VS)
        struct netns_ipvs       *ipvs;
#endif
#if IS_ENABLED(CONFIG_MPLS)
        struct netns_mpls       mpls;
#endif
        struct sock             *diag_nlsk;
        atomic_t                fnhe_genid;
};

typedef struct {
#ifdef CONFIG_NET_NS
        struct net *net;
#endif
} possible_net_t;

#if 0
struct timer_list {
        /*
         * All fields that change during normal runtime grouped to the
         * same cacheline
         */
        struct hlist_node entry;
        unsigned long expires;
        void (*function)(unsigned long);
        unsigned long data;
        u32 flags;
        int slack;
#ifdef CONFIG_TIMER_STATS
        int start_pid;
        void *start_site;
        char start_comm[16];
#endif
#ifdef CONFIG_LOCKDEP
        struct lockdep_map lockdep_map;
#endif
};
#endif

/**
 * struct gnet_stats_rate_est64 - rate estimator
 * @bps: current byte rate
 * @pps: current packet rate
 */
struct gnet_stats_rate_est64 {
        __u64   bps;
        __u64   pps;
};
struct gnet_stats_basic_packed {
        __u64   bytes;
        __u32   packets;
} __attribute__ ((packed));
/**
 * struct gnet_stats_queue - queuing statistics
 * @qlen: queue length
 * @backlog: backlog size of queue
 * @drops: number of dropped packets
 * @requeues: number of requeues
 * @overlimits: number of enqueues over the limit
 */
struct gnet_stats_queue {
        __u32   qlen;
        __u32   backlog;
        __u32   drops;
        __u32   requeues;
        __u32   overlimits;
};

struct sk_buff;

struct sk_buff_head {
	/* These two members must be first. */
	struct sk_buff	*next;
	struct sk_buff	*prev;

	__u32		qlen;
	spinlock_t	lock;
};

/* HW offloaded queuing disciplines txq count and offset maps */
struct netdev_tc_txq {
        u16 count;
        u16 offset;
};

/*
 * enum rx_handler_result - Possible return values for rx_handlers.
 * @RX_HANDLER_CONSUMED: skb was consumed by rx_handler, do not process it
 * further.
 * @RX_HANDLER_ANOTHER: Do another round in receive path. This is indicated in
 * case skb->dev was changed by rx_handler.
 * @RX_HANDLER_EXACT: Force exact delivery, no wildcard.
 * @RX_HANDLER_PASS: Do nothing, passe the skb as if no rx_handler was called.
 *
 * rx_handlers are functions called from inside __netif_receive_skb(), to do
 * special processing of the skb, prior to delivery to protocol handlers.
 *
 * Currently, a net_device can only have a single rx_handler registered. Trying
 * to register a second rx_handler will return -EBUSY.
 *
 * To register a rx_handler on a net_device, use netdev_rx_handler_register().
 * To unregister a rx_handler on a net_device, use
 * netdev_rx_handler_unregister().
 *
 * Upon return, rx_handler is expected to tell __netif_receive_skb() what to
 * do with the skb.
 *
 * If the rx_handler consumed to skb in some way, it should return
 * RX_HANDLER_CONSUMED. This is appropriate when the rx_handler arranged for
 * the skb to be delivered in some other ways.
 *
 * If the rx_handler changed skb->dev, to divert the skb to another
 * net_device, it should return RX_HANDLER_ANOTHER. The rx_handler for the
 * new device will be called if it exists.
 *
 * If the rx_handler consider the skb should be ignored, it should return
 * RX_HANDLER_EXACT. The skb will only be delivered to protocol handlers that
 * are registered on exact device (ptype->dev == skb->dev).
 *
 * If the rx_handler didn't changed skb->dev, but want the skb to be normally
 * delivered, it should return RX_HANDLER_PASS.
 *
 * A device without a registered rx_handler will behave as if rx_handler
 * returned RX_HANDLER_PASS.
 */
enum rx_handler_result {
	RX_HANDLER_CONSUMED,
	RX_HANDLER_ANOTHER,
	RX_HANDLER_EXACT,
	RX_HANDLER_PASS,
};
typedef enum rx_handler_result rx_handler_result_t;
typedef rx_handler_result_t rx_handler_func_t(struct sk_buff **pskb);

struct netdev_hw_addr_list {
	struct list_head list;
	int count;
};

struct netdev_queue {
/*
 * read mostly part
 */
	struct net_device *dev;
	struct Qdisc __rcu *qdisc;
	struct Qdisc *qdisc_sleeping;
#ifdef CONFIG_SYSFS
	struct kobject kobj;
#endif
#if defined(CONFIG_XPS) && defined(CONFIG_NUMA)
	int numa_node;
#endif
/*
 * write mostly part
 */
	spinlock_t _xmit_lock ____cacheline_aligned_in_smp;
	int xmit_lock_owner;
	/*
	 * please use this field instead of dev->trans_start
	 */
	unsigned long trans_start;

	/*
	 * Number of TX timeouts for this queue
	 * (/sys/class/net/DEV/Q/trans_timeout)
	 */
	unsigned long trans_timeout;
	unsigned long state;
#ifdef CONFIG_BQL
	struct dql dql;
#endif
	unsigned long tx_maxrate;
} ____cacheline_aligned_in_smp;

#if 0
struct net_device_stats {
        unsigned long   rx_packets;
        unsigned long   tx_packets;
        unsigned long   rx_bytes;
        unsigned long   tx_bytes;
        unsigned long   rx_errors;
        unsigned long   tx_errors;
        unsigned long   rx_dropped;
        unsigned long   tx_dropped;
        unsigned long   multicast;
        unsigned long   collisions;
        unsigned long   rx_length_errors;
        unsigned long   rx_over_errors;
        unsigned long   rx_crc_errors;
        unsigned long   rx_frame_errors;
        unsigned long   rx_fifo_errors;
        unsigned long   rx_missed_errors;
        unsigned long   tx_aborted_errors;
        unsigned long   tx_carrier_errors;
        unsigned long   tx_fifo_errors;
        unsigned long   tx_heartbeat_errors;
        unsigned long   tx_window_errors;
        unsigned long   rx_compressed;
        unsigned long   tx_compressed;
};
#endif

#if defined(CONFIG_NF_CONNTRACK) || defined(CONFIG_NF_CONNTRACK_MODULE)
struct nf_conntrack {
	atomic_t use;
};
#endif

#if IS_ENABLED(CONFIG_BRIDGE_NETFILTER)
struct nf_bridge_info {
	atomic_t		use;
	enum {
		BRNF_PROTO_UNCHANGED,
		BRNF_PROTO_8021Q,
		BRNF_PROTO_PPPOE
	} orig_proto:8;
	u8			pkt_otherhost:1;
	u8			in_prerouting:1;
	u8			bridged_dnat:1;
	__u16			frag_max_size;
	struct net_device	*physindev;

	/* always valid & non-NULL from FORWARD on, for physdev match */
	struct net_device	*physoutdev;
	union {
		/* prerouting: detect dnat in orig/reply direction */
		__be32          ipv4_daddr;
		struct in6_addr ipv6_daddr;

		/* after prerouting + nat detected: store original source
		 * mac since neigh resolution overwrites it, only used while
		 * skb is out in neigh layer.
		 */
		char neigh_header[8];
	};
};
#endif

/**
 * struct skb_mstamp - multi resolution time stamps
 * @stamp_us: timestamp in us resolution
 * @stamp_jiffies: timestamp in jiffies
 */
struct skb_mstamp {
        union {
                u64 v64;
                struct {
                        u32 stamp_us;
                        u32 stamp_jiffies;
                };
        };
};

#ifndef __HAS_RB_NODE
#define __HAS_RB_NODE
struct rb_node {
        unsigned long  __rb_parent_color;
        struct rb_node *rb_right;
        struct rb_node *rb_left;
} __attribute__((aligned(sizeof(long))));
/* The alignment might seem pointless, but allegedly CRIS needs it */
#endif /* __HAS_RB_NODE */

struct sk_buff {
	union {
		struct {
			/* These two members must be first. */
			struct sk_buff *next;
			struct sk_buff *prev;

			union {
				ktime_t tstamp;
				struct skb_mstamp skb_mstamp;
			};
		};
		struct rb_node rbnode; /* used in netem & tcp stack */
	};
	struct sock *sk;
	struct net_device *dev;

	/*
	 * This is the control buffer. It is free to use for every
	 * layer. Please put your private variables there. If you
	 * want to keep them across layers you have to do a skb_clone()
	 * first. This is owned by whoever has the skb queued ATM.
	 */
	char			cb[48] __aligned(8);
	/*This field is only needed by RtL8190 Driver.FIX ME!!!*/
        unsigned char   __unused;

	unsigned long		_skb_refdst;
	void			(*destructor)(struct sk_buff *skb);
#ifdef CONFIG_XFRM
	struct	sec_path	*sp;
#endif
#if defined(CONFIG_NF_CONNTRACK) || defined(CONFIG_NF_CONNTRACK_MODULE)
	struct nf_conntrack	*nfct;
#endif
#if IS_ENABLED(CONFIG_BRIDGE_NETFILTER)
	struct nf_bridge_info	*nf_bridge;
#endif
	unsigned int		len,
				data_len;
	__u16			mac_len,
				hdr_len;

	/* Following fields are _not_ copied in __copy_skb_header()
	 * Note that queue_mapping is here mostly to fill a hole.
	 */
	kmemcheck_bitfield_begin(flags1);
	__u16			queue_mapping;
	__u8			cloned:1,
				nohdr:1,
				fclone:2,
				peeked:1,
				head_frag:1,
				xmit_more:1;
	/* one bit hole */
	kmemcheck_bitfield_end(flags1);

	/* fields enclosed in headers_start/headers_end are copied
	 * using a single memcpy() in __copy_skb_header()
	 */
	/* private: */
	__u32			headers_start[0];
	/* public: */

/* if you move pkt_type around you also must adapt those constants */
#ifdef __BIG_ENDIAN_BITFIELD
#define PKT_TYPE_MAX	(7 << 5)
#else
#define PKT_TYPE_MAX	7
#endif
#define PKT_TYPE_OFFSET()	offsetof(struct sk_buff, __pkt_type_offset)

	__u8			__pkt_type_offset[0];
	__u8			pkt_type:3;
	__u8			pfmemalloc:1;
	__u8			ignore_df:1;
	__u8			nfctinfo:3;

	__u8			nf_trace:1;
	__u8			ip_summed:2;
	__u8			ooo_okay:1;
	__u8			l4_hash:1;
	__u8			sw_hash:1;
	__u8			wifi_acked_valid:1;
	__u8			wifi_acked:1;

	__u8			no_fcs:1;
	/* Indicates the inner headers are valid in the skbuff. */
	__u8			encapsulation:1;
	__u8			encap_hdr_csum:1;
	__u8			csum_valid:1;
	__u8			csum_complete_sw:1;
	__u8			csum_level:2;
	__u8			csum_bad:1;

#ifdef CONFIG_IPV6_NDISC_NODETYPE
	__u8			ndisc_nodetype:2;
#endif
	__u8			ipvs_property:1;
	__u8			inner_protocol_type:1;
	__u8			remcsum_offload:1;
	/* 3 or 5 bit hole */

#ifdef CONFIG_NET_SCHED
	__u16			tc_index;	/* traffic control index */
#ifdef CONFIG_NET_CLS_ACT
	__u16			tc_verd;	/* traffic control verdict */
#endif
#endif
//#if defined( CONFIG_RTL_HARDWARE_MULTICAST) || defined(CONFIG_RTL865X_LANPORT_RESTRICTION) ||defined(CONFIG_RTL_QOS_VLANID_SUPPORT)||defined (CONFIG_RTL_VLAN_8021Q)
	__u16			srcPort;
	__u16			srcVlanId:12;
//#endif
	union {
		__wsum		csum;
		struct {
			__u16	csum_start;
			__u16	csum_offset;
		};
	};
	__u32			priority;
	int			skb_iif;
	__u32			hash;
	__be16			vlan_proto;
	__u16			vlan_tci;
#if defined(CONFIG_NET_RX_BUSY_POLL) || defined(CONFIG_XPS)
	union {
		unsigned int	napi_id;
		unsigned int	sender_cpu;
	};
#endif
	union {
#ifdef CONFIG_NETWORK_SECMARK
		__u32		secmark;
#endif
#ifdef CONFIG_NET_SWITCHDEV
		__u32		offload_fwd_mark;
#endif
	};

	union {
		__u32		mark;
		__u32		reserved_tailroom;
	};

	union {
		__be16		inner_protocol;
		__u8		inner_ipproto;
	};

	__u16			inner_transport_header;
	__u16			inner_network_header;
	__u16			inner_mac_header;

	__be16			protocol;
	__u16			transport_header;
	__u16			network_header;
	__u16			mac_header;
	
#if defined(CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE)
        rtk_rg_fc_ingress_data_t fcIngressData;
#endif

	/* private: */
	__u32			headers_end[0];
	/* public: */

	/* These elements must be at the end, see alloc_skb() for details.  */
	sk_buff_data_t tail;
	sk_buff_data_t end;
	unsigned char *head, *data;
	unsigned int truesize;
	atomic_t users;

	/* private data used in Cortina NI driver */
#ifdef CONFIG_ARCH_CORTINA
	__u8 is_from_l3qm;
	__u8 fbm_pool_id;
#ifdef CONFIG_CORTINA_KERNEL_HOOK
	__u16 vid;
#endif

#endif
};

/* from include/net/sch_generic.h*/
struct Qdisc {
        int (*enqueue)(struct sk_buff *skb, struct Qdisc *dev);
        struct sk_buff * (*dequeue)(struct Qdisc *dev);
        unsigned int flags;
#define TCQ_F_BUILTIN   1
#define TCQ_F_INGRESS   2
#define TCQ_F_CAN_BYPASS        4
#define TCQ_F_MQROOT    8
#define TCQ_F_ONETXQUEUE        0x10 /* dequeue_skb() can assume all skbs are for
                                      * q->dev_queue : It can test
                                      * netif_xmit_frozen_or_stopped() before
                                      * dequeueing next packet.
                                      * Its true for MQ/MQPRIO slaves, or non
                                      * multiqueue device.
                                      */
#define TCQ_F_WARN_NONWC        (1 << 16)
#define TCQ_F_CPUSTATS  0x20 /* run using percpu statistics */
#define TCQ_F_NOPARENT  0x40 /* root of its hierarchy :
                                      * qdisc_tree_decrease_qlen() should stop.
                                      */
        u32     limit;
        const struct Qdisc_ops  *ops;
        struct qdisc_size_table __rcu *stab;
        struct list_head        list;
        u32 handle;
        u32 parent;
        int (*reshape_fail)(struct sk_buff *skb, struct Qdisc *q);
        void *u32_node;

        /* This field is deprecated, but it is still used by CBQ
         * and it will live until better solution will be invented.
         */
        struct Qdisc *__parent;
        struct netdev_queue *dev_queue;

        struct gnet_stats_rate_est64 rate_est;
        struct gnet_stats_basic_cpu __percpu *cpu_bstats;
        struct gnet_stats_queue __percpu *cpu_qstats;

        struct Qdisc            *next_sched;
        struct sk_buff          *gso_skb;
        /*
         * For performance sake on SMP, we put highly modified fields at the end
         */
        unsigned long           state;
        struct sk_buff_head     q;
        struct gnet_stats_basic_packed bstats;
        unsigned int            __state;
        struct gnet_stats_queue qstats;
        struct rcu_head         rcu_head;
        int                     padded;
        atomic_t                refcnt;

        spinlock_t              busylock ____cacheline_aligned_in_smp;
};

typedef unsigned short __kernel_dev_t;
typedef __kernel_dev_t  dev_t;
struct device {
	struct device *parent;
	struct class *class;
	dev_t devt;	/* dev_t, creates the sysfs "dev" */
	void (*release)(struct device *dev);
	/* This is used from drivers/usb/musb-new subsystem only */
	void *driver_data;	/* data private to the driver */
	void *device_data;   /* data private to the device */
};

struct net_device {
        char name[IFNAMSIZ];
        struct hlist_node name_hlist;
        char *ifalias;
        /*
         *	I/O specific fields
         *	FIXME: Merge these and struct ifmap into one
         */
        unsigned long mem_end;
        unsigned long mem_start;
        unsigned long base_addr;
        int irq;

        atomic_t carrier_changes;

        /*
         *	Some hardware also needs these fields (state,dev_list,
         *	napi_list,unreg_list,close_list) but they are not
         *	part of the usual set specified in Space.c.
         */
        unsigned long state;

        struct list_head dev_list;
        struct list_head napi_list;
        struct list_head unreg_list;
        struct list_head close_list;
        struct list_head ptype_all;
        struct list_head ptype_specific;

        struct {
                struct list_head upper;
                struct list_head lower;
        } adj_list;

        struct {
                struct list_head upper;
                struct list_head lower;
        } all_adj_list;

        netdev_features_t features;
        netdev_features_t hw_features;
        netdev_features_t wanted_features;
        netdev_features_t vlan_features;
        netdev_features_t hw_enc_features;
        netdev_features_t mpls_features;

        int ifindex;
        int group;

        struct net_device_stats stats;

        atomic_long_t rx_dropped;
        atomic_long_t tx_dropped;

#ifdef CONFIG_WIRELESS_EXT
        const struct iw_handler_def * wireless_handlers;
        struct iw_public_data * wireless_data;
#endif
        const struct net_device_ops *netdev_ops;
        const struct ethtool_ops *ethtool_ops;
#ifdef CONFIG_NET_SWITCHDEV
        const struct switchdev_ops *switchdev_ops;
#endif
#ifdef CONFIG_NET_L3_MASTER_DEV
        const struct l3mdev_ops *l3mdev_ops;
#endif

        const struct header_ops *header_ops;

        unsigned int flags;
        unsigned int priv_flags;

        unsigned short gflags;
        unsigned short padded;

        unsigned char operstate;
        unsigned char link_mode;

        unsigned char if_port;
        unsigned char dma;

        unsigned int mtu;
        unsigned short type;
        unsigned short hard_header_len;

        unsigned short needed_headroom;
        unsigned short needed_tailroom;

        /* Interface address info. */
        unsigned char perm_addr[MAX_ADDR_LEN];
        unsigned char addr_assign_type;
        unsigned char addr_len;
        unsigned short neigh_priv_len;
        unsigned short dev_id;
        unsigned short dev_port;
        spinlock_t addr_list_lock;
        unsigned char name_assign_type;
        bool uc_promisc;
        struct netdev_hw_addr_list uc;
        struct netdev_hw_addr_list mc;
        struct netdev_hw_addr_list dev_addrs;

#ifdef CONFIG_SYSFS
        struct kset *queues_kset;
#endif
        unsigned int promiscuity;
        unsigned int allmulti;

        /* Protocol specific pointers */

#if IS_ENABLED(CONFIG_VLAN_8021Q)
        struct vlan_info __rcu	*vlan_info;
#endif
#if IS_ENABLED(CONFIG_NET_DSA)
        struct dsa_switch_tree	*dsa_ptr;
#endif
#if IS_ENABLED(CONFIG_TIPC)
        struct tipc_bearer __rcu *tipc_ptr;
#endif
        void *atalk_ptr;
        struct in_device __rcu	*ip_ptr;
        struct dn_dev __rcu *dn_ptr;
        struct inet6_dev __rcu	*ip6_ptr;
        void *ax25_ptr;
        struct wireless_dev *ieee80211_ptr;
        struct wpan_dev *ieee802154_ptr;
#if IS_ENABLED(CONFIG_MPLS_ROUTING)
        struct mpls_dev __rcu	*mpls_ptr;
#endif

        /*
        * Cache lines mostly used on receive path (including eth_type_trans())
        */
        unsigned long last_rx;

        /* Interface address info used in eth_type_trans() */
        unsigned char *dev_addr;
#ifdef CONFIG_SYSFS
        struct netdev_rx_queue *_rx;

        unsigned int num_rx_queues;
        unsigned int real_num_rx_queues;
#endif
        unsigned long gro_flush_timeout;
        rx_handler_func_t __rcu *rx_handler;
        void __rcu *rx_handler_data;
#ifdef CONFIG_NET_CLS_ACT
        struct tcf_proto __rcu *ingress_cl_list;
#endif
        struct netdev_queue __rcu *ingress_queue;
#ifdef CONFIG_NETFILTER_INGRESS
        struct list_head nf_hooks_ingress;
#endif
        unsigned char broadcast[MAX_ADDR_LEN];
#ifdef CONFIG_RFS_ACCEL
        struct cpu_rmap		*rx_cpu_rmap;
#endif
        struct hlist_node	index_hlist;

        /*
        * Cache lines mostly used on transmit path
        */
        struct netdev_queue	*_tx ____cacheline_aligned_in_smp;
        unsigned int		num_tx_queues;
        unsigned int		real_num_tx_queues;
        struct Qdisc		*qdisc;
        unsigned long		tx_queue_len;
        spinlock_t		tx_global_lock;
        int			watchdog_timeo;

#ifdef CONFIG_XPS
        struct xps_dev_maps __rcu *xps_maps;
#endif

#ifdef CONFIG_NET_SWITCHDEV
        u32			offload_fwd_mark;
#endif

        /* These may be needed for future network-power-down code. */

        /*
         * trans_start here is expensive for high speed devices on SMP,
         * please use netdev_queue->trans_start instead.
         */
        unsigned long		trans_start;

        struct timer_list	watchdog_timer;

        int __percpu		*pcpu_refcnt;
        struct list_head	todo_list;

        struct list_head	link_watch_list;

        enum { NETREG_UNINITIALIZED=0,
                NETREG_REGISTERED,	/* completed register_netdevice */
                NETREG_UNREGISTERING,	/* called unregister_netdevice */
                NETREG_UNREGISTERED,	/* completed unregister todo */
                NETREG_RELEASED,		/* called free_netdev */
                NETREG_DUMMY,		/* dummy device for NAPI poll */
        } reg_state:8;

        bool dismantle;

        enum {
                RTNL_LINK_INITIALIZED,
                RTNL_LINK_INITIALIZING,
        } rtnl_link_state:16;

        void (*destructor)(struct net_device *dev);

#ifdef CONFIG_NETPOLL
        struct netpoll_info __rcu	*npinfo;
#endif

        possible_net_t			nd_net;

        /* mid-layer private */
        union {
                void					*ml_priv;
                struct pcpu_lstats __percpu		*lstats;
                struct pcpu_sw_netstats __percpu	*tstats;
                struct pcpu_dstats __percpu		*dstats;
                struct pcpu_vstats __percpu		*vstats;
        };

        struct garp_port __rcu	*garp_port;
        struct mrp_port __rcu	*mrp_port;

        struct device	dev;
        const struct attribute_group *sysfs_groups[4];
        const struct attribute_group *sysfs_rx_queue_group;

        const struct rtnl_link_ops *rtnl_link_ops;

        /* for setting kernel sock attribute on TCP connection setup */
#define GSO_MAX_SIZE		65536
        unsigned int		gso_max_size;
#define GSO_MAX_SEGS		65535
        u16			gso_max_segs;
        u16			gso_min_segs;
#ifdef CONFIG_DCB
        const struct dcbnl_rtnl_ops *dcbnl_ops;
#endif
        u8 num_tc;
        struct netdev_tc_txq tc_to_txq[TC_MAX_QUEUE];
        u8 prio_tc_map[TC_BITMASK + 1];

#if IS_ENABLED(CONFIG_FCOE)
        unsigned int		fcoe_ddp_xid;
#endif
#if IS_ENABLED(CONFIG_CGROUP_NET_PRIO)
        struct netprio_map __rcu *priomap;
#endif
        struct phy_device *phydev;
        struct lock_class_key *qdisc_tx_busylock;
        bool proto_down;

        /*Realtek private fields*/
        void                    *priv;  /* pointer to private data      */
};

struct timerqueue_node {
	struct rb_node node;
	ktime_t expires;
};

static inline ktime_t hrtimer_get_softexpires(const struct hrtimer *timer)
{
	return timer->_softexpires;
}

static inline ktime_t hrtimer_get_expires(const struct hrtimer *timer)
{
	return timer->node.expires;
}

/*
 * Mode arguments of xxx_hrtimer functions:
 */
enum hrtimer_mode {
	HRTIMER_MODE_ABS = 0x0,		/* Time value is absolute */
	HRTIMER_MODE_REL = 0x1,		/* Time value is relative to now */
	HRTIMER_MODE_PINNED = 0x02,	/* Timer is bound to CPU */
	HRTIMER_MODE_ABS_PINNED = 0x02,
	HRTIMER_MODE_REL_PINNED = 0x03,
};

enum  hrtimer_base_type {
        HRTIMER_BASE_MONOTONIC,
        HRTIMER_BASE_REALTIME,
        HRTIMER_BASE_BOOTTIME,
        HRTIMER_BASE_TAI,
        HRTIMER_MAX_CLOCK_BASES,
};

/* Basic timer operations: */
extern void hrtimer_start_range_ns(struct hrtimer *timer, ktime_t tim,
			unsigned long range_ns, const enum hrtimer_mode mode);

static inline void hrtimer_start_expires(struct hrtimer *timer,
					 enum hrtimer_mode mode)
{
	unsigned long delta;
	ktime_t soft, hard;
	soft = hrtimer_get_softexpires(timer);
	hard = hrtimer_get_expires(timer);
	delta = ktime_to_ns(ktime_sub(hard, soft));
	hrtimer_start_range_ns(timer, soft, delta, mode);
}

#if 0
/*
 * Version using sequence counter only.
 * This can be used when code has its own mutex protecting the
 * updating starting before the write_seqcountbeqin() and ending
 * after the write_seqcount_end().
 */
typedef struct seqcount {
	unsigned sequence;
#ifdef CONFIG_DEBUG_LOCK_ALLOC
	struct lockdep_map dep_map;
#endif
} seqcount_t;
#endif

struct hrtimer_cpu_base {
	raw_spinlock_t			lock;
	seqcount_t			seq;
	struct hrtimer			*running;
	unsigned int			cpu;
	unsigned int			active_bases;
	unsigned int			clock_was_set_seq;
	bool				migration_enabled;
	bool				nohz_active;
#ifdef CONFIG_HIGH_RES_TIMERS
	unsigned int			in_hrtirq	: 1,
					hres_active	: 1,
					hang_detected	: 1;
	ktime_t				expires_next;
	struct hrtimer			*next_timer;
	unsigned int			nr_events;
	unsigned int			nr_retries;
	unsigned int			nr_hangs;
	unsigned int			max_hang_time;
#endif
	struct hrtimer_clock_base	clock_base[HRTIMER_MAX_CLOCK_BASES];
} ____cacheline_aligned;

struct timerqueue_head {
	struct rb_root head;
	struct timerqueue_node *next;
};

/**
 * struct hrtimer_clock_base - the timer base for a specific clock
 * @cpu_base:		per cpu clock base
 * @index:		clock type index for per_cpu support when moving a
 *			timer to a base on another cpu.
 * @clockid:		clock id for per_cpu support
 * @active:		red black tree root node for the active timers
 * @get_time:		function to retrieve the current time of the clock
 * @offset:		offset of this clock to the monotonic base
 */
struct hrtimer_clock_base {
	struct hrtimer_cpu_base	*cpu_base;
	int			index;
	clockid_t		clockid;
	struct timerqueue_head	active;
	ktime_t			(*get_time)(void);
	ktime_t			offset;
} __attribute__((__aligned__(HRTIMER_CLOCK_BASE_ALIGN)));

static inline void hrtimer_restart(struct hrtimer *timer)
{
	hrtimer_start_expires(timer, HRTIMER_MODE_ABS);
}

struct hrtimer {
        struct timerqueue_node node;
        ktime_t _softexpires;
        enum hrtimer_restart (*function)(struct hrtimer *);
        struct hrtimer_clock_base *base;
        u8 state;
        u8 is_rel;
#ifdef CONFIG_TIMER_STATS
        int start_pid;
        void *start_site;
        char start_comm[16];
#endif
};

typedef struct {
        spinlock_t              slock;
        int                     owned;
        wait_queue_head_t       wq;
        /*
         * We express the mutex-alike socket_lock semantics
         * to the lock validator by explicitly managing
         * the slock as a lock variant (in addition to
         * the slock itself):
         */
#ifdef CONFIG_DEBUG_LOCK_ALLOC
        struct lockdep_map dep_map;
#endif
} socket_lock_t;

struct proto {
        void                    (*close)(struct sock *sk,
                                        long timeout);
        int                     (*connect)(struct sock *sk,
                                        struct sockaddr *uaddr,
                                        int addr_len);
        int                     (*disconnect)(struct sock *sk, int flags);

        struct sock *           (*accept)(struct sock *sk, int flags, int *err);

        int                     (*ioctl)(struct sock *sk, int cmd,
                                         unsigned long arg);
        int                     (*init)(struct sock *sk);
        void                    (*destroy)(struct sock *sk);
        void                    (*shutdown)(struct sock *sk, int how);
        int                     (*setsockopt)(struct sock *sk, int level,
                                        int optname, char __user *optval,
                                        unsigned int optlen);
        int                     (*getsockopt)(struct sock *sk, int level,
                                        int optname, char __user *optval,
                                        int __user *option);
#ifdef CONFIG_COMPAT
        int                     (*compat_setsockopt)(struct sock *sk,
                                        int level,
                                        int optname, char __user *optval,
                                        unsigned int optlen);
        int                     (*compat_getsockopt)(struct sock *sk,
                                        int level,
                                        int optname, char __user *optval,
                                        int __user *option);
        int                     (*compat_ioctl)(struct sock *sk,
                                        unsigned int cmd, unsigned long arg);
#endif
        int                     (*sendmsg)(struct sock *sk, struct msghdr *msg,
                                           size_t len);
        int                     (*recvmsg)(struct sock *sk, struct msghdr *msg,
                                           size_t len, int noblock, int flags,
                                           int *addr_len);
        int                     (*sendpage)(struct sock *sk, struct page *page,
                                        int offset, size_t size, int flags);
        int                     (*bind)(struct sock *sk,
                                        struct sockaddr *uaddr, int addr_len);

        int                     (*backlog_rcv) (struct sock *sk,
                                                struct sk_buff *skb);

        void            (*release_cb)(struct sock *sk);

        /* Keeping track of sk's, looking them up, and port selection methods. */
        void                    (*hash)(struct sock *sk);
        void                    (*unhash)(struct sock *sk);
        void                    (*rehash)(struct sock *sk);
        int                     (*get_port)(struct sock *sk, unsigned short snum);
        void                    (*clear_sk)(struct sock *sk, int size);

        /* Keeping track of sockets in use */
#ifdef CONFIG_PROC_FS
        unsigned int            inuse_idx;
#endif

        bool                    (*stream_memory_free)(const struct sock *sk);
        /* Memory pressure */
        void                    (*enter_memory_pressure)(struct sock *sk);
        atomic_long_t           *memory_allocated;      /* Current allocated memory. */
        struct percpu_counter   *sockets_allocated;     /* Current number of sockets. */
        /*
         * Pressure flag: try to collapse.
         * Technical note: it is used by multiple contexts non atomically.
         * All the __sk_mem_schedule() is of this nature: accounting
         * is strict, actions are advisory and have some latency.
         */
        int                     *memory_pressure;
        long                    *sysctl_mem;
        int                     *sysctl_wmem;
        int                     *sysctl_rmem;
        int                     max_header;
        bool                    no_autobind;

        struct kmem_cache       *slab;
        unsigned int            obj_size;
        int                     slab_flags;

        struct percpu_counter   *orphan_count;

        struct request_sock_ops *rsk_prot;
        struct timewait_sock_ops *twsk_prot;

        union {
                struct inet_hashinfo    *hashinfo;
                struct udp_table        *udp_table;
                struct raw_hashinfo     *raw_hash;
        } h;

        struct module           *owner;

        char                    name[32];

        struct list_head        node;
#ifdef SOCK_REFCNT_DEBUG
        atomic_t                socks;
#endif
#ifdef CONFIG_MEMCG_KMEM
        /*
         * cgroup specific init/deinit functions. Called once for all
         * protocols that implement it, from cgroups populate function.
         * This function has to setup any files the protocol want to
         * appear in the kmem cgroup filesystem.
         */
        int                     (*init_cgroup)(struct mem_cgroup *memcg,
                                               struct cgroup_subsys *ss);
        void                    (*destroy_cgroup)(struct mem_cgroup *memcg);
        struct cg_proto         *(*proto_cgroup)(struct mem_cgroup *memcg);
#endif
};

struct sock_common {
        /* skc_daddr and skc_rcv_saddr must be grouped on a 8 bytes aligned
         * address on 64bit arches : cf INET_MATCH()
         */
        union {
                __addrpair      skc_addrpair;
                struct {
                        __be32  skc_daddr;
                        __be32  skc_rcv_saddr;
                };
        };
        union  {
                unsigned int    skc_hash;
                __u16           skc_u16hashes[2];
        };
        /* skc_dport && skc_num must be grouped as well */
        union {
                __portpair      skc_portpair;
                struct {
                        __be16  skc_dport;
                        __u16   skc_num;
                };
        };

        unsigned short          skc_family;
        volatile unsigned char  skc_state;
        unsigned char           skc_reuse:4;
        unsigned char           skc_reuseport:1;
        unsigned char           skc_ipv6only:1;
        unsigned char           skc_net_refcnt:1;
        int                     skc_bound_dev_if;
        union {
                struct hlist_node       skc_bind_node;
                struct hlist_nulls_node skc_portaddr_node;
        };
        struct proto            *skc_prot;
        possible_net_t          skc_net;

#if IS_ENABLED(CONFIG_IPV6)
        struct in6_addr         skc_v6_daddr;
        struct in6_addr         skc_v6_rcv_saddr;
#endif

        atomic64_t              skc_cookie;

        /* following fields are padding to force
         * offset(struct sock, sk_refcnt) == 128 on 64bit arches
         * assuming IPV6 is enabled. We use this padding differently
         * for different kind of 'sockets'
         */
        union {
                unsigned long   skc_flags;
                struct sock     *skc_listener; /* request_sock */
                struct inet_timewait_death_row *skc_tw_dr; /* inet_timewait_sock */
        };
        /*
         * fields between dontcopy_begin/dontcopy_end
         * are not copied in sock_copy()
         */
        /* private: */
        int                     skc_dontcopy_begin[0];
        /* public: */
        union {
                struct hlist_node       skc_node;
                struct hlist_nulls_node skc_nulls_node;
        };
        int                     skc_tx_queue_mapping;
        union {
                int             skc_incoming_cpu;
                u32             skc_rcv_wnd;
                u32             skc_tw_rcv_nxt; /* struct tcp_timewait_sock  */
        };

        atomic_t                skc_refcnt;
        /* private: */
        int                     skc_dontcopy_end[0];
        union {
                u32             skc_rxhash;
                u32             skc_window_clamp;
                u32             skc_tw_snd_nxt; /* struct tcp_timewait_sock */
        };
        /* public: */
};

struct sock {
        /*
         * Now struct inet_timewait_sock also uses sock_common, so please just
         * don't add nothing before this first member (__sk_common) --acme
         */
        struct sock_common      __sk_common;
#define sk_node                 __sk_common.skc_node
#define sk_nulls_node           __sk_common.skc_nulls_node
#define sk_refcnt               __sk_common.skc_refcnt
#define sk_tx_queue_mapping     __sk_common.skc_tx_queue_mapping

#define sk_dontcopy_begin       __sk_common.skc_dontcopy_begin
#define sk_dontcopy_end         __sk_common.skc_dontcopy_end
#define sk_hash                 __sk_common.skc_hash
#define sk_portpair             __sk_common.skc_portpair
#define sk_num                  __sk_common.skc_num
#define sk_dport                __sk_common.skc_dport
#define sk_addrpair             __sk_common.skc_addrpair
#define sk_daddr                __sk_common.skc_daddr
#define sk_rcv_saddr            __sk_common.skc_rcv_saddr
#define sk_family               __sk_common.skc_family
#define sk_state                __sk_common.skc_state
#define sk_reuse                __sk_common.skc_reuse
#define sk_reuseport            __sk_common.skc_reuseport
#define sk_ipv6only             __sk_common.skc_ipv6only
#define sk_net_refcnt           __sk_common.skc_net_refcnt
#define sk_bound_dev_if         __sk_common.skc_bound_dev_if
#define sk_bind_node            __sk_common.skc_bind_node
#define sk_prot                 __sk_common.skc_prot
#define sk_net                  __sk_common.skc_net
#define sk_v6_daddr             __sk_common.skc_v6_daddr
#define sk_v6_rcv_saddr __sk_common.skc_v6_rcv_saddr
#define sk_cookie               __sk_common.skc_cookie
#define sk_incoming_cpu         __sk_common.skc_incoming_cpu
#define sk_flags                __sk_common.skc_flags
#define sk_rxhash               __sk_common.skc_rxhash

        socket_lock_t           sk_lock;
        struct sk_buff_head     sk_receive_queue;
        /*
         * The backlog queue is special, it is always used with
         * the per-socket spinlock held and requires low latency
         * access. Therefore we special case it's implementation.
         * Note : rmem_alloc is in this structure to fill a hole
         * on 64bit arches, not because its logically part of
         * backlog.
         */
        struct {
                atomic_t        rmem_alloc;
                int             len;
                struct sk_buff  *head;
                struct sk_buff  *tail;
        } sk_backlog;
#define sk_rmem_alloc sk_backlog.rmem_alloc
        int                     sk_forward_alloc;

        __u32                   sk_txhash;
#ifdef CONFIG_NET_RX_BUSY_POLL
        unsigned int            sk_napi_id;
        unsigned int            sk_ll_usec;
#endif
        atomic_t                sk_drops;
        int                     sk_rcvbuf;

        struct sk_filter __rcu  *sk_filter;
        union {
                struct socket_wq __rcu  *sk_wq;
                struct socket_wq        *sk_wq_raw;
        };
#ifdef CONFIG_XFRM
        struct xfrm_policy __rcu *sk_policy[2];
#endif
        struct dst_entry        *sk_rx_dst;
        struct dst_entry __rcu  *sk_dst_cache;
        /* Note: 32bit hole on 64bit arches */
        atomic_t                sk_wmem_alloc;
        atomic_t                sk_omem_alloc;
        int                     sk_sndbuf;
        struct sk_buff_head     sk_write_queue;
        kmemcheck_bitfield_begin(flags);
        unsigned int            sk_shutdown  : 2,
                                sk_no_check_tx : 1,
                                sk_no_check_rx : 1,
                                sk_userlocks : 4,
                                sk_protocol  : 8,
                                sk_type      : 16;
#define SK_PROTOCOL_MAX U8_MAX
        kmemcheck_bitfield_end(flags);
        int                     sk_wmem_queued;
        gfp_t                   sk_allocation;
        u32                     sk_pacing_rate; /* bytes per second */
        u32                     sk_max_pacing_rate;
        netdev_features_t       sk_route_caps;
        netdev_features_t       sk_route_nocaps;
        int                     sk_gso_type;
        unsigned int            sk_gso_max_size;
        u16                     sk_gso_max_segs;
        int                     sk_rcvlowat;
        unsigned long           sk_lingertime;
        struct sk_buff_head     sk_error_queue;
        struct proto            *sk_prot_creator;
        rwlock_t                sk_callback_lock;
        int                     sk_err,
                                sk_err_soft;
        u32                     sk_ack_backlog;
        u32                     sk_max_ack_backlog;
        __u32                   sk_priority;
#if IS_ENABLED(CONFIG_CGROUP_NET_PRIO)
        __u32                   sk_cgrp_prioidx;
#endif
        struct pid              *sk_peer_pid;
        const struct cred       *sk_peer_cred;
        long                    sk_rcvtimeo;
        long                    sk_sndtimeo;
        struct timer_list       sk_timer;
        ktime_t                 sk_stamp;
        u16                     sk_tsflags;
        u32                     sk_tskey;
        struct socket           *sk_socket;
        void                    *sk_user_data;
        struct page_frag        sk_frag;
        struct sk_buff          *sk_send_head;
        __s32                   sk_peek_off;
        int                     sk_write_pending;
#ifdef CONFIG_SECURITY
        void                    *sk_security;
#endif
        __u32                   sk_mark;
#ifdef CONFIG_CGROUP_NET_CLASSID
        u32                     sk_classid;
#endif
        struct cg_proto         *sk_cgrp;
        void                    (*sk_state_change)(struct sock *sk);
        void                    (*sk_data_ready)(struct sock *sk);
        void                    (*sk_write_space)(struct sock *sk);
        void                    (*sk_error_report)(struct sock *sk);
        int                     (*sk_backlog_rcv)(struct sock *sk,
                                                  struct sk_buff *skb);
        void                    (*sk_destruct)(struct sock *sk);
};

/*
 * Structure for NAPI scheduling similar to tasklet but with weighting
 */
struct napi_struct {
	/* The poll_list must only be managed by the entity which
	 * changes the state of the NAPI_STATE_SCHED bit.  This means
	 * whoever atomically sets that bit can add this napi_struct
	 * to the per-cpu poll_list, and whoever clears that bit
	 * can remove from the list right before clearing the bit.
	 */
	struct list_head	poll_list;

	unsigned long		state;
	int			weight;
	unsigned int		gro_count;
	int			(*poll)(struct napi_struct *, int);
#ifdef CONFIG_NETPOLL
	spinlock_t		poll_lock;
	int			poll_owner;
#endif
	struct net_device	*dev;
	struct sk_buff		*gro_list;
	struct sk_buff		*skb;
	struct hrtimer		timer;
	struct list_head	dev_list;
	struct hlist_node	napi_hash_node;
	unsigned int		napi_id;
};
#endif
#endif//sd1 uboot for 98f

#endif /* __CA_NI_COMMON_H__ */
