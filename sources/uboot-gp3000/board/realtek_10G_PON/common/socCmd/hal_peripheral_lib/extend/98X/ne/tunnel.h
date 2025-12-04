#ifndef __TUNNEL_H__
#define __TUNNEL_H__

#include "ca_types.h"
#include "port.h"

/***************************************************************************
 * Common
 ***************************************************************************/

#define CA_TUNNEL_MAX_LIMIT	64

typedef enum {
	CA_TUNNEL_DIR_OUTBOUND	= 0,
	CA_TUNNEL_DIR_INBOUND	= 1,

	CA_IPSEC_SPD_ENCRYPT	= CA_TUNNEL_DIR_OUTBOUND,
	CA_IPSEC_SPD_DECRYPT	= CA_TUNNEL_DIR_INBOUND,

	CA_PPTP_DIR_UPSTREAM	= CA_TUNNEL_DIR_OUTBOUND,
	CA_PPTP_DIR_DOWNSTREAM	= CA_TUNNEL_DIR_INBOUND,
} ca_tunnel_direction_t;

/***************************************************************************
 * IPSec
 ***************************************************************************/

#define MAX_ENC_KEY_LEN		8
#define MAX_AUTH_KEY_LEN	8

typedef enum {
	CA_IPSEC_SA_PROTO_ESP	= 0,
	CA_IPSEC_SA_PROTO_AH	= 1,
	CA_IPSEC_SA_PROTO_MAX	= 2,
} ca_ipsec_sa_proto_t;

typedef enum {
	CA_IPSEC_POLICY_ACTION_IPSEC	= 1, /* Apply IPSec    */
	CA_IPSEC_POLICY_ACTION_DISCARD	= 2, /* Discard / Drop */
	CA_IPSEC_POLICY_ACTION_BYPASS	= 3, /* Allow to pass  */
} ca_ipsec_policy_action_t;

typedef enum ca_ipsec_ciph_alg {
	CA_IPSEC_CIPH_NULL	= 0,
	CA_IPSEC_CIPH_DES	= 1,
	CA_IPSEC_CIPH_AES	= 2,
} ca_ipsec_ciph_alg_t;

typedef enum ca_ipsec_ciph_mode {
	CA_IPSEC_CIPH_MODE_ECB	= 0,
	CA_IPSEC_CIPH_MODE_CBC	= 1,
	CA_IPSEC_CIPH_MODE_CTR	= 2,
	CA_IPSEC_CIPH_MODE_CCM	= 3,
	CA_IPSEC_CIPH_MODE_GCM	= 5,
	CA_IPSEC_CIPH_MODE_OFB	= 7,
	CA_IPSEC_CIPH_MODE_CFB	= 8,
} ca_ipsec_ciph_mode_t;

typedef enum ca_ipsec_hash_alg {
	CA_IPSEC_HASH_NULL	= 0,
	CA_IPSEC_HASH_MD5	= 1,
	CA_IPSEC_HASH_SHA1	= 2,
} ca_ipsec_hash_alg_t;

typedef struct ca_ipsec_sa {
	ca_uint8_t	replay_window;
	ca_uint32_t	spi;
	ca_uint32_t	seq_num;
	ca_uint8_t	ekey[MAX_ENC_KEY_LEN];
	ca_uint8_t	akey[MAX_AUTH_KEY_LEN];
	ca_uint32_t	ip_ver:1;		/* 0=IPv4, 1=IPv6         */
	ca_uint32_t	proto:1;		/* 0=ESP; 1=AH            */
	ca_uint32_t	tunnel:1;		/* 0=Transport; 1=Tunnel  */

	/* encryption mode based on elliptic                              */
	ca_uint32_t	ealg:2;			/* ca_ipsec_ciph_alg_t    */
	ca_uint32_t	ealg_mode:4;		/* ca_ipsec_ciph_mode_t   */
	ca_uint32_t	enc_keylen:4;		/* (n * 4) bytes          */
	ca_uint32_t	iv_len:4;		/* (n * 4) bytes for CBC  */

	/* authentication mode based on elliptic                          */
	ca_uint32_t	aalg:3;			/* ca_ipsec_hash_alg_t    */
	ca_uint32_t	auth_keylen:4;
	ca_uint32_t	icv_trunclen:4;		/* (n * 4) bytes          */
	ca_uint32_t	etherIP:1;
	ca_uint32_t	reserved:3;

	ca_uint32_t	is_natt;		/* NAT-Traversal          */
	ca_uint16_t	src_l4_port;		/* UDP port for IPSec NAT-Traversal */
	ca_uint16_t	dest_l4_port;		/* UDP port for IPSec NAT-Traversal (default 4500) */
	ca_tunnel_direction_t	sa_dir;		/* SA direction, see definition of ca_tunnel_direction_t */
} ca_ipsec_sa_t;

typedef struct ca_ipsec_tunnel_cfg_s {
	ca_ipsec_sa_id_t	sa_id_encrypt;	/* Ingress IPSEC policy for tunnel */
	ca_ipsec_sa_id_t	sa_id_decrypt;	/* Egress IPSEC policy for tunnel  */
} ca_ipsec_tunnel_cfg_t;

typedef struct ca_ipsec_sa_iterate_entry_s {
	ca_ipsec_sa_t		ipsec_sa;
	ca_ipsec_sa_id_t	sa_id;
} ca_ipsec_sa_iterate_entry_t;

/***************************************************************************
 * PPPoE
 ***************************************************************************/

typedef struct ca_pppoe_tunnel_cfg_s {
	ca_port_encap_t		port_encap;
	ca_mac_addr_t		dest_mac;
	ca_uint16_t		pppoe_session_id;
} ca_pppoe_tunnel_cfg_t;

/***************************************************************************
 * L2TP
 ***************************************************************************/

#define L2TPv3_COOKIE_SIZE	8

typedef struct ca_l2tp_tunnel_cfg_s {
	ca_uint16_t		ver;			/* Version and flags */
	ca_uint16_t		len;			/* Optional Length */
	ca_uint16_t		tid;			/* Tunnel ID */
	ca_uint16_t		dest_port;		/* UDP port of L2TP hdr */
	ca_uint16_t		src_port;		/* UDP port of L2TP hdr */
	ca_uint32_t		session_id;		/* Session ID */

	ca_uint16_t		ipsec_policy;		/* 0: unused, otherwise: encrypted */

	/* For L2TPv3 */
	ca_uint16_t		encap_type;		/* 1: IP, 0: UDP */
	ca_uint16_t		l2specific_len;		/* default is 4 */
	ca_uint16_t		l2specific_type;	/* 0: NONE, 1: DEFAULT */
	ca_uint8_t		send_seq;		/* 0: Not send sequence */
	ca_uint8_t		calc_udp_csum;		/* For IPv6 outer IP, calculate UDP checksum of L2TP header.
							     0: UDP checksum is zero
							     1: calculate UDP checksum
							 */
	ca_uint32_t		ns;			/* If send_seq, set the start of Session NR state */

	ca_uint16_t		cookie_len;
	ca_uint8_t		cookie[L2TPv3_COOKIE_SIZE];
	ca_uint16_t		offset;			/* offset from the end of L2TP header to beginning of data */
	ca_mac_addr_t		l2tp_src_mac;
	ca_mac_addr_t		peer_l2tp_src_mac;
} ca_l2tp_tunnel_cfg_t;

/***************************************************************************
 * PPTP
 ***************************************************************************/

typedef enum {
	CA_CRYPTO_INVALID	= -1,
	CA_CRYPTO_NONE		= 0,
	CA_CRYPTO_MPPE40,
	CA_CRYPTO_MPPE128,
} ca_pptp_crypto_type_t;

typedef struct ca_pptp_sa_s {
	ca_tunnel_direction_t	dir;		/* direction. 0=outbound, 1=inbound.	*/
	ca_uint16_t		call_id;	/* local call ID			*/

	ca_boolean_t		state_less;	/* TRUE/FALSE				*/
	ca_pptp_crypto_type_t	crypto_type;

	ca_uint8_t		key[16];	/* Start Key				*/
	ca_uint8_t		keylen;
	ca_uint32_t		seq_num;	/* sequence number			*/
} ca_pptp_sa_t;

typedef struct ca_pptp_tunnel_cfg_s {
	ca_pptp_sa_id_t		sa_id_outbound;	/* Egress PPTP SA for tunnel		*/
	ca_pptp_sa_id_t		sa_id_inbound;	/* Ingress PPTP SA for tunnel		*/
} ca_pptp_tunnel_cfg_t;

typedef struct ca_pptp_sa_iterate_entry_s {
	ca_pptp_sa_t		pptp_sa;
	ca_pptp_sa_id_t		sa_id;
} ca_pptp_sa_iterate_entry_t;

/***************************************************************************
 * DS-Lite
 ***************************************************************************/

typedef struct ca_dslite_config_s {
	/* For Unicast,
	   ca_tunnel_cfg_t.src_addr = B4 addr
	   ca_tunnel_cfg_t.dest_addr = AFTR addr

	   For Multicast,
	     ca_tunnel_cfg_t.src_addr = Multicast group addr
	       ca_tunnel_cfg_t.src_addr.ipv6_addr[0] and ipv6_addr[1] is MPrefix64.
	       ca_tunnel_cfg_t.src_addr.ipv6_addr[2] and ipv6_add3[2] is MPrefix64_mask.
	     ca_tunnel_cfg_t.dest_addr = Multicast source addr
	       ca_tunnel_cfg_t.dest_addr.ipv6_addr[0] and ipv6_addr[1] is UPrefix64.
	       ca_tunnel_cfg_t.dest_addr.ipv6_addr[2] and ipv6_add3[2] is UPrefix64_mask.
	 */
	/* ingress tunnel hdr validation check includes:
	   1. Check packet IPv6 prefix matches the configured tunnel end point prefix
	   2. Check packet IPv6 ecn bits matches the IPv4 ECN bits
	   3. Outer packet IPv6 DA, SA /IPv4 part = inner IPv4 DA, SA in case of MC packet
	 */

	/* When '1', filter outer IP is MC, and inner IP is UC packet */
	ca_boolean_t		dsl_ipmc_ilg_chk_en;

	/* When '1', the outer IPv6 is UC, inner IPv4 is MC packet, also treated as DSLite IP MC */
	ca_boolean_t		dsl_ipmc_ext_det_en;

	/* When '1', support the DSLite IPMC packet outer address MPrefix/UPrefix check */
	ca_boolean_t		dsl_ipmc_addr_prfx_chk_en;

	/* When '1', support the DSLite IPMC packet outer address and inner address consistency check */
	ca_boolean_t		dsl_ipmc_addr_consistency_chk_en;

	/* When '1', incoming packet outer IPv6DA and IPv6SA match with B4 and AFTR address for non-outer IPMC packet.
	 * If doesn't match, check failure. Packet will be dropped or redirected.
	 */
	ca_boolean_t		dsl_rpf_chk_en;

	/* When '1', the outer TTL field will be kept while performing the DSLite outer IP pop */
	ca_boolean_t		dsl_ttl_keep_outer;

	/* When '1', the outer DSCP field will be kept while performing the DSLite outer IP pop */
	ca_boolean_t		dsl_dscp_keep_outer;

	/* When '1', the outer ECN field will be kept while performing the DSLite outer IP pop */
	ca_boolean_t		dsl_ecn_keep_outer;

	ca_boolean_t		ecn_check_enable;		/* check ECN of outer and inner hdr */
	ca_boolean_t 		rebuild_mc_mac;			/* rebuild L2 MC hdr based on inner IP version */
	ca_boolean_t		mc_flag;			/* 1: multicast tunnel (only decapsulation is supported) 0: unicast */
	ca_mac_addr_t		mc_mac;				/* lower 25 bits (MSB) specified here */
	ca_boolean_t		mc_drop;			/* drop received MC packets */
	ca_port_id_t		validation_check_failed_dest_port;

	/* encap parameters */
	ca_uint8_t		egress_tc_value;
	ca_uint8_t		egress_hoplimit_value;
	ca_uint32_t		egress_flow_label_value;
} ca_dslite_config_t;

/***************************************************************************
 * 6RD
 ***************************************************************************/

typedef struct ca_6rd_config_s {
	/* For Unicast,
	     ca_tunnel_cfg_t.src_addr = CE addr
	     ca_tunnel_cfg_t.dest_addr = BR addr

	   For Multicast,
	     ca_tunnel_cfg_t.src_addr = Multicast group addr
	     ca_tunnel_cfg_t.dest_addr = Multicast source addr
	 */

	/* 6rd delegated IPv6 prefix and length */
	ca_ip_address_t		ipv6_6rd_prefix;

	/* Used to extract top bits for IP_DA_v4,and calculate the length of rest of IPv4 address bits in IPv4 address */
	ca_uint8_t		six_rd_v4_msk_len;

	/* ingress tunnel hdr validation check includes:
	   1. Check packet IPv6 DA delegated prefix matches the configured tunnel local end point prefix.
	      This will be the CE to CE case. Else it is the BR/CE case
	   2. Packet IPv4 DA/SA match configured tunnel IPv4 DA/SA (tunnel end point check)
	   3. Tunnel IPv4 header suffix matches configured delegated IPv6 prefix/IPv4 suffix part
	   4. | IPv4 | delegated IPv6 prefix | IPv4 suffix | <- 6RD domain defined by IPv4 subnet and delegated IPv6 prefix.
	 */

	ca_uint8_t		six_rd_v6_prfx_len;		/* for 6rd delegated addr calculation & check when decap */
	ca_boolean_t		six_rd_ingr_chk_en;		/* enable ingress validation check */
	ca_boolean_t		six_rd_ipsa_mtch;		/* Check for IPSA_v4v6 match when traffic from BR */

	/* When '1', filter outer IP is MC, and inner IP is UC packet */
	ca_boolean_t		sixrd_ipmc_ilg_chk_en;

	/* When '1', the outer TTL field will be kept while performing the 6RD outer IP pop */
	ca_boolean_t		sixrd_ttl_keep_outer;

	/* When '1', the outer DSCP field will be kept while performing the 6RD outer IP pop */
	ca_boolean_t		sixrd_dscp_keep_outer;

	/* When '1', the outer ECN field will be kept while performing the 6RD outer IP pop */
	ca_boolean_t		sixrd_ecn_keep_outer;

	ca_boolean_t		ecn_check_enable;		/* check ECN of outer match that of inner hdr */
	ca_boolean_t 		rebuild_mc_mac;			/* rebuild L2 MC hdr based on inner IP version */
	ca_mac_addr_t		mc_mac;				/* lower 16 bits (MSB) specified here */
	ca_boolean_t		mc_drop;			/* drop received MC packets */
	ca_port_id_t		validation_check_failed_dest_port;

	/* Check packet IPv6 DA delegated prefix matches the configured tunnel local end point prefix.
	 * This will be the CE to CE case. Else it is the BR/CE case.
	 */
	ca_boolean_t		ce_ce_allowed;			/* allow CE/CE traffic */

	/* encap parameters */
	ca_uint8_t		egress_tos_value;
	ca_uint8_t		egress_ttl_value;
	ca_uint16_t		egress_identification_start;
	ca_uint16_t		egress_identification_end;

	/* 1: Use v6 IP_DA's IPv4 part as outer IPv4 DA.
	 * 0: use ca_tunnel_cfg_t.dest_addr as outer IPv4 DA.
	 */
	ca_boolean_t		six_rd_ipda_from_v6;
} ca_6rd_config_t;

/***************************************************************************
 * Tunnel API
 ***************************************************************************/

typedef enum {
	CA_TUNNELINVALID	= 0,
	CA_TUNNEL_PPPOE		= 1,
	CA_TUNNEL_L2TP		= 2,
	CA_TUNNEL_IPSEC		= 3,
	CA_TUNNEL_PPTP		= 4,
	CA_TUNNEL_4_IN_4	= 5,
	CA_TUNNEL_DSLITE	= 6,
	CA_TUNNEL_6RD		= 7,
	CA_TUNNEL_6_IN_6	= 8,

	CA_TUNNEL_MIN		= CA_TUNNELINVALID,
	CA_TUNNEL_MAX,
} ca_tunnel_type_t;

typedef struct ca_tunnel_cfg_union_s {
	ca_pppoe_tunnel_cfg_t	pppoe;
	ca_l2tp_tunnel_cfg_t	l2tp;
	ca_ipsec_tunnel_cfg_t	ipsec;
	ca_pptp_tunnel_cfg_t	pptp;
	ca_dslite_config_t	dslite;
	ca_6rd_config_t		six_rd;
} ca_tunnel_cfg_union_t;

typedef struct ca_tunnel_cfg_s {
	ca_tunnel_type_t	type;			/* tunnel type                       */
	ca_ip_address_t		dest_addr;		/* tunnel destination IP             */
	ca_ip_address_t		src_addr;		/* tunnel src IP                     */
	ca_intf_id_t		parent_l3_intf_id;	/* Parent L3 interface of the tunnel */
	ca_tunnel_cfg_union_t	tunnel;
} ca_tunnel_cfg_t;

typedef struct ca_tunnel_iterate_entry_s {
	ca_tunnel_id_t		tunnel_id;
	ca_tunnel_cfg_t		tunnel;
} ca_tunnel_iterate_entry_t;

ca_status_t ca_ipsec_init (
	CA_IN		ca_device_id_t		device_id );

ca_status_t ca_ipsec_shut (
	CA_IN		ca_device_id_t		device_id );

ca_status_t ca_ipsec_sa_add (
	CA_IN		ca_device_id_t		device_id,
	CA_IN		ca_ipsec_sa_t		*ipsec_sa,
	CA_OUT		ca_ipsec_sa_id_t	*p_ipsec_sa_id );

ca_status_t ca_ipsec_sa_delete (
	CA_IN		ca_device_id_t		device_id,
	CA_IN		ca_ipsec_sa_id_t	ipsec_sa_id );

ca_status_t ca_ipsec_sa_get (
	CA_IN		ca_device_id_t		device_id,
	CA_IN		ca_ipsec_sa_id_t	ipsec_sa_id,
	CA_OUT		ca_ipsec_sa_t		*p_ipsec_sa );

ca_status_t ca_ipsec_sa_iterate (
	CA_IN		ca_device_id_t		device_id,
	CA_OUT		ca_iterator_t		*p_return_entry);

ca_status_t ca_pptp_sa_add (
	CA_IN		ca_device_id_t		device_id,
	CA_IN		ca_pptp_sa_t		*pptp_sa,
	CA_OUT		ca_pptp_sa_id_t		*p_pptp_sa_id );

ca_status_t ca_pptp_sa_delete (
	CA_IN		ca_device_id_t		device_id,
	CA_IN		ca_pptp_sa_id_t		pptp_sa_id );

ca_status_t ca_pptp_sa_get (
	CA_IN		ca_device_id_t		device_id,
	CA_IN		ca_pptp_sa_id_t		pptp_sa_id,
	CA_OUT		ca_pptp_sa_t		*pptp_sa );

ca_status_t ca_pptp_sa_iterate (
	CA_IN		ca_device_id_t		device_id,
	CA_OUT		ca_iterator_t		*p_return_entry );

ca_status_t ca_pptp_key_change (
	CA_IN		ca_device_id_t		device_id,
	CA_IN		ca_tunnel_id_t		tunnel_id );

ca_status_t ca_tunnel_add (
	CA_IN		ca_device_id_t		device_id,
	CA_IN		ca_tunnel_cfg_t		*tunnel_cfg,
	CA_OUT		ca_tunnel_id_t		*tunnel_id );

ca_status_t ca_tunnel_update (
	CA_IN		ca_device_id_t		device_id,
	CA_IN		ca_tunnel_cfg_t		*tunnel_cfg,
	CA_IN		ca_tunnel_id_t		tunnel_id );

ca_status_t ca_tunnel_delete (
	CA_IN		ca_device_id_t		device_id,
	CA_IN		ca_tunnel_id_t		tunnel_id );

ca_status_t ca_tunnel_delete_all (
	CA_IN		ca_device_id_t		device_id );

ca_status_t ca_tunnel_get (
	CA_IN		ca_device_id_t		device_id,
	CA_IN		ca_tunnel_id_t		tunnel_id,
	CA_OUT		ca_tunnel_cfg_t		*tunnel_cfg );

ca_status_t ca_tunnel_iterate (
	CA_IN		ca_device_id_t		device_id,
	CA_OUT		ca_iterator_t		*p_return_entry );

/***************************************************************************
 * Internal Use
 ***************************************************************************/

typedef struct ca_tunnel_s {/*CODEGEN_IGNORE_TAG*/
	ca_tunnel_cfg_t tcfg;

	/* private data */
	unsigned int hash_idx_upstream;
	unsigned int hash_idx_downstream;

	unsigned int aal_idx; /* currently used by DSLite and 6RD */
} ca_tunnel_t;

#endif /* __TUNNEL_H__ */

