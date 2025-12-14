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
/*
 * ca_types.h
 *
 * Include file containing some basic and common data types and defines
 * used by driver.
*/
#ifndef __CA_TYPES_H__
#define __CA_TYPES_H__

#if 0//yocto
#else//sd1 uboot for 98f
#define CONFIG_ARCH_CORTINA_G3LITE      1
#define CONFIG_ARCH_RTL8198F    1
#endif

#include <basic_types.h>

#ifdef CA_DONT_USE_STDLIB
#ifndef CA_DONT_USE_VAR_ARGS
#define CA_DONT_USE_VAR_ARGS
#endif
#endif

/*
 * Basic data types
 */
/*Used by 64-bits 32-bits transfer between Pointer and integer  */
typedef  unsigned long          ca_uint;
typedef  long                   ca_int;
typedef  unsigned long          ca_uint_t;
typedef  long                   ca_int_t;

typedef unsigned long long      ca_uint64_t;
typedef long long               ca_int64_t;
typedef unsigned int            ca_uint32_t;
typedef int                     ca_int32_t;
typedef unsigned short          ca_uint16_t;
typedef short                   ca_int16_t;
typedef unsigned char           ca_uint8_t;
typedef char                    ca_int8_t;
typedef ca_uint32_t             ca_boolean_t;
typedef double                  ca_double_t;
typedef char                    ca_char8_t;
typedef unsigned char           ca_uchar8_t;
typedef ca_uint16_t             ca_device_id_t;
typedef ca_uint32_t             ca_port_id_t;
typedef ca_uint32_t             ca_vlan_id_t;
typedef ca_uint32_t             ca_port_bmp_t;
typedef ca_uint8_t              ca_pon_tunnel_id_t;
typedef ca_uint16_t             ca_llid_t;
typedef ca_uint16_t             ca_sub_id_t;
typedef ca_uint16_t             ca_intf_id_t;
typedef ca_uint16_t             ca_tunnel_id_t;
typedef ca_uint16_t             ca_l3_nexthop_id_t;
typedef ca_uint16_t             ca_session_id_t;
typedef ca_uint32_t             ca_ipsec_sa_id_t;
typedef ca_uint32_t             ca_pptp_sa_id_t;

/* backward compatible */
typedef ca_uint64_t             ca_uint64;
typedef ca_int64_t              ca_int64;
typedef ca_uint32_t             ca_uint32;
typedef ca_int32_t              ca_int32;
typedef ca_uint16_t             ca_uint16;
typedef ca_int16_t              ca_int16;
typedef ca_uint8_t              ca_uint8;
typedef ca_int8_t               ca_int8;
typedef ca_boolean_t            ca_boolean;
typedef ca_uint32_t             ca_addr;
typedef ca_double_t             ca_double;
typedef ca_uint32_t             ca_llid_instance_t;
typedef ca_uint32_t             ca_flow_id_t;
typedef ca_uint32_t             ca_ipv4_addr_t;
typedef ca_uint16_t             ca_ipv6_addr_t[8];
typedef ca_char8_t              ca_char;
typedef ca_uint16_t             aal_lport_id_t;
typedef ca_uint16_t             ca_tpid_t;

typedef ca_uint32_t*            handle_t;

typedef enum {
    CA_UINT8_INVALID    = 0xFF,
    CA_UINT16_INVALID   = 0xFFFF,
    CA_UINT32_INVALID   = 0xFFFFFFFF,
} ca_uint_invalid_t;

#define CA_FLOW_ID_INVALID	CA_UINT16_INVALID

#define CA_TRUE		1
#define CA_FALSE	0

typedef enum
{
    CA_E_ERROR          = -1,
    CA_E_OK             = 0x0,
    CA_E_RESOURCE       = 0x1,
    CA_E_PARAM          = 0x2,
    CA_E_NOT_FOUND      = 0x3,
    CA_E_CONFLICT       = 0x4,
    CA_E_TIMEOUT        = 0x5,
    CA_E_INTERNAL       = 0x6,
    CA_E_NOT_SUPPORT    = 0x7,
    CA_E_CONFIG         = 0x8,
    CA_E_UNAVAIL        = 0x9,
    CA_E_MEMORY         = 0xa,
    CA_E_BUSY           = 0xb,
    CA_E_FULL           = 0xc,
    CA_E_EMPTY          = 0xd,
    CA_E_EXISTS         = 0xe,
    CA_E_DEV            = 0xf,
    CA_E_PORT           = 0x10,
    CA_E_LLID           = 0x11,
    CA_E_VLAN           = 0x12,
    CA_E_INIT           = 0x13,
    CA_E_INTF           = 0x14,
    CA_E_NEXTHOP        = 0x15,
    CA_E_ROUTE          = 0x16,
    CA_E_DB_CHANGED     = 0x17,
    CA_E_INACTIVE       = 0x18,
} ca_status_t;


typedef enum {
    CA_IPV4 = 0,
    CA_IPV6 = 1,
} ca_ip_afi_t;

/* All stores in host order.
 * Example:
 *   192.168.0.1
 *     ipv4_addr = 0xC0A80001
 *   2001::1234:5678
 *     ipv6_addr[0] = 0x20010000
 *     ipv6_addr[1] = 0x0
 *     ipv6_addr[2] = 0x0
 *     ipv6_addr[3] = 0x12345678
 */
typedef union {
    ca_uint32_t addr[4];
    ca_uint32_t ipv4_addr;
    ca_uint32_t ipv6_addr[4];
} ca_l3_ip_addr_t;

typedef struct ca_ip_address {
    ca_ip_afi_t afi;        /* address family identifier */
    ca_l3_ip_addr_t ip_addr;    /* IP address */
    ca_uint8_t  addr_len;   /* length in bits */
} ca_ip_address_t;

#define CA_MAX_ITERATOR_RETRIEVE_ENTRIES	16

typedef struct ca_iterator {
    /* iterator is type independent. Caller and callee must have the same entry data type
       definition so caller can parse the returned entry_data correctly.
       Data type definitions are included in <cortina/xxx.h>
     */
    ca_uint32_t    entry_count;         /* number of entries to retreive */
    ca_uint32_t    entry_index[CA_MAX_ITERATOR_RETRIEVE_ENTRIES]; /* array to store unique indexes of retreived entries*/  
    void           *p_entry_data;       /* pointer to entry data - buffer allocated by caller */
    ca_uint32_t    user_buffer_size;    /* size of buffer provide by user */
    void           *p_prev_handle;      /* used by callee to store prev entry context. Caller must not modify this value*/
} ca_iterator_t;

#define CA_ETH_ADDR_LEN     6
typedef ca_uint8_t ca_mac_addr_t[CA_ETH_ADDR_LEN];

typedef enum {
    CA_PORT_TYPE_GPON              = 0x1, 
    CA_PORT_TYPE_EPON              = 0x2, 
    CA_PORT_TYPE_ETHERNET           = 0x3, 
    CA_PORT_TYPE_CPU                = 0x4, 
    CA_PORT_TYPE_SUBPORT         = 0x5, 
    CA_PORT_TYPE_OFFLOAD        = 0x6,
    CA_PORT_TYPE_BLACKHOLE     = 0x7,
    CA_PORT_TYPE_L2RP               = 0x8,
    CA_PORT_TYPE_TRUNK                = 0x9,
    CA_PORT_TYPE_GEM                  = 0xa,
    CA_PORT_TYPE_OMCC                 = 0xb,
    CA_PORT_TYPE_L3                   = 0xc,
    CA_PORT_TYPE_INVALID    = 0xFFFF,
} ca_port_type_t;

#define CA_PORT_TYPE_SHIFT   16
#define CA_PORT_TYPE_MASK    0xFFFF
#define CA_PORT_ID_MASK      0xFFFF
#define CA_PORT_ID(port_type, port_id)   ((((port_type)&CA_PORT_TYPE_MASK)<<CA_PORT_TYPE_SHIFT) | ((port_id)&CA_PORT_ID_MASK))
#define PORT_TYPE(ca_portid)             (((ca_portid) >> CA_PORT_TYPE_SHIFT) & CA_PORT_TYPE_MASK)
#define PORT_ID(ca_portid)               (ca_portid & CA_PORT_ID_MASK)


#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#ifndef CA_IN
#define CA_IN
#endif

#ifndef CA_OUT
#define CA_OUT
#endif

#ifndef CA_IN_OUT
#define CA_IN_OUT
#endif

/* $feat_start: CORTINA  */
/* FIXME: re-visit CA_UNION and CA_DI - for now ported over from Cortina */
/* $feat_end: CORTINA  */
#ifndef CA_DONT_USE_DESGNTD_INITLZR
#define  CA_DI(x)   x
#else
#define  CA_DI(x)
#endif

typedef struct {
    /* IPv6 address in network order, e.g. for 4001:1234:5678:0:0:0:abcd:ef,
       ca_s6_addr(a, 0)   is 0x40
       ca_s6_addr(a, 15)  is 0xef
       ca_s6_addr16(a, 0) is 0x4001
       ca_s6_addr16(a, 7) is 0x00ef
       ca_s6_addr32(a, 0) is 0x40011234
       ca_s6_addr32(a, 3) is 0xabcd00ef

       In various configurations, where byte mask of IPv6 address is
       applicable, bit[0-7] is masking byte[0] of the address. If any of the
       bits are set then the byte mask is considered as set. If the
       data structure for the mask is the same as the address (key), then the
       mask is bit-by-bit, and byte-by-byte masking the key.
     */
    union {
        ca_uint8_t  u6_addr8[16];
        ca_uint16_t u6_addr16[8];
        ca_uint32_t u6_addr32[4];
    } in6_u;
#   ifdef CA_BIG_ENDIAN
#define ca_s6_addr(a, i)    ((a).in6_u.u6_addr8[i])
#define ca_s6_addr16(a, i)  ((a).in6_u.u6_addr16[i])
#define ca_s6_addr32(a, i)  ((a).in6_u.u6_addr32[i])
#   else /* CA_LITTLE_ENDIAN */
#define ca_s6_addr(a, i)    ((a).in6_u.u6_addr8[((i) & 0xc) | (~(i) & 3)])
#define ca_s6_addr16(a, i)  ((a).in6_u.u6_addr16[((i) & 0xe) | (~(i) & 1)])
#define ca_s6_addr32(a, i)  ((a).in6_u.u6_addr32[i])
#   endif
} ca_in6_addr_t;

typedef struct {
    /* IPv4 in network order, e.g. for 192.168.0.1, (s_addr & 0xff) is 1.

       In various configuration, where byte mask of IPv4 addresses is applicable,
       bit[0-7]  will act as the mask for byte[0]. If any of this bits are set
       then the byte mask is considered as set.  If bit wise mask
       is available, then bit[0] is mask of bit 0 of byte ".1".  If the
       data structure for the mask is the same as the address (key), then the
       mask is bit-by-bit, and byte-by-byte masking the key.
     */
    ca_uint32_t     s_addr;
} ca_in_addr_t;

typedef struct {
    /* Common data structure for IPv4 and IPv6.  ip_ver selects the version */
    union {
        ca_in_addr_t    v4;
        ca_in6_addr_t   v6;
    } addr;
    ca_ip_afi_t     ip_ver;
} ca_ip_addr_t;

typedef struct{
    ca_vlan_id_t        vid;
    ca_uint32_t     dot1p;
    ca_uint32_t     tpid;
}ca_vlan_t;

/*
 * Register data type
 */
typedef ca_uint32_t     ca_reg;

/*
 * Chip(Driver) Type
 */
typedef enum {
    CA_CHIP_UNKNOWN     = 0,
    CA_CHIP_MISANO,
    CA_CHIP_DAYTONA,
    CA_CHIP_ESTORIL,
    CA_CHIP_SUNI_TETRA
} ca_chip_type_t ;


#ifndef ca_ntohl
#define ca_ntohl(x) ((((x) & 0x000000ff) << 24) | \
            (((x) & 0x0000ff00) <<  8) | \
            (((x) & 0x00ff0000) >>  8) | \
            (((x) & 0xff000000) >> 24))
#endif


#ifndef ca_htonl
#define ca_htonl(x) ((((x) & 0x000000ff) << 24) | \
            (((x) & 0x0000ff00) <<  8) | \
            (((x) & 0x00ff0000) >>  8) | \
            (((x) & 0xff000000) >> 24))
#endif


#ifndef ca_ntohs
#define ca_ntohs(x) ((((x) & 0x00ff) << 8) | \
            (((x) & 0xff00) >> 8))
#endif

#ifndef ca_htons
#define ca_htons(x) ((((x) & 0x00ff) << 8) | \
            (((x) & 0xff00) >> 8))

#endif

#ifndef ca_ntohll
#define ca_ntohll(x)    ((((ca_uint64)ca_ntohl(x)) << 32) | \
            ca_ntohl(x >> 32))
#endif


#ifndef ca_htonll
#define ca_htonll(x)    ((((ca_uint64)ca_htonl(x)) << 32) | \
            ca_htonl(x >> 32))
#endif

typedef enum {
    CA_SPEED_10MBPS     = 0,
    CA_SPEED_100MBPS,
    CA_SPEED_1000MBPS,
    CA_SPEED_INVALID    = 0xFF,
} ca_speed_t;

typedef enum {
    CA_DUPLEX_HALF      = 0,
    CA_DUPLEX_FULL      = 1,
    CA_DUPLEX_INVALID   = 0xFF,
} ca_duplex_t;

typedef enum {
    AAL_PORT_UNI0       = 0x0,
    AAL_PORT_PON        = 0x1,
    AAL_PORT_RESERVE    = 0x2,
    AAL_PORT_BLACKHOLE  = 0x3,
    AAL_PORT_CPU        = 0x4,
    AAL_PORT_UNI1       = 0x5,
    AAL_PORT_DMA        = 0x6,
    AAL_PORT_FE     = 0x7,
    AAL_PORT_END        = 0x8,
    AAL_PORT_INVALID    = 0xff,
} aal_port_t;

typedef enum {
    AAL_FE_GE_ID        = 0,
    AAL_FE_PON_ID       = 1,
    AAL_FE_MA_ID        = 2,
    AAL_FE_INVALID      = 0xff,
} ca_aal_fe_instance_t;


/*
 * Other defines
 */
#ifndef TRUE
#define TRUE        1
#endif

#ifndef FALSE
#define FALSE       0
#endif

#ifndef NULL
#define NULL        0
#endif

#ifdef CA_DONT_USE_INLINE
#       define CA_INLINE    static
#else
#       define CA_INLINE    __inline__ static
#endif

#define IS_AAL_PON_PORT(port_id)    ((port_id) == AAL_PORT_PON)

#define IS_AAL_ETH_PORT(port_id)    (((port_id) == AAL_PORT_UNI0) || \
                    ((port_id) == AAL_PORT_UNI1))

typedef enum {
  CA_DISABLE   = 0,
  CA_ENABLE    = 1
} ca_ctl_t ;

#if 0//yocto
#else//sd1 uboot for 98f
typedef void                    ca_void;
#endif//sd1 uboot for 98f

#endif /* __CA_TYPES_H__ */

