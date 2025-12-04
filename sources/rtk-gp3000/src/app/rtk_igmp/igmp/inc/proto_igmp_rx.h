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

#ifndef __PROTO_IGMP_RX_H__
#define __PROTO_IGMP_RX_H__



#define IP_ADDR_LEN 4
#define MAX_VLAN_NUMBER 4096

typedef struct igmpv3_grec_s
{
    uint8   grec_type;
    uint8   grec_auxwords;
    uint16  grec_nsrcs;
    uint32  grec_mca;
    uint32  grec_src[0];
} igmpv3_grec_t;

typedef struct igmpv3_report_s
{
    uint8 type;
    uint8 resv1;
    uint16 csum;
    uint16 resv2;
    uint16 ngrec;
    igmpv3_grec_t grec[0];
} igmpv3_report_t;

typedef struct igmp_hdr_s
{
    uint8  type;                    /* type*/
    uint8  maxRespTime;             /*maximum response time,unit:0.1 seconds*/
    uint16 checksum;
    uint32 groupAddr;

    struct
    {
        uint8 rsq;                      /* 4bit: reserved, 1bit: suppress router-side processing, 3bit: querier's robustness variable*/
        uint8 qqic;                     /* querier's query interval code */
        uint16 numOfSrc;                /* number of sources */
        uint32 srcList[0];                 /* first entry of source list */
    }v3;
} igmp_hdr_t;

typedef struct ip_addr_s
{
    uint8 addr[IP_ADDR_LEN];
} ip_addr_t;

typedef struct iphdr_s
{
#if !defined(CONFIG_CPU_BIG_ENDIAN)
        uint8   ihl:4,
                version:4;
#else
        uint8   version:4,
                ihl:4;
#endif
        uint8   tos;
        uint16  tot_len;
        uint16  id;
        uint16  frag_off;
        uint8   ttl;
        uint8   protocol;
        uint16  check;
        uint32  sip;
        uint32  dip;

        struct
        {
            /*The options start here. */
            uint8    options[4];
        }v3;
} iphdr_t;

#define MLD_IPV6_HDR_LEN            40

#define HOP_BY_HOP_OPTIONS_HEADER   0
#define ROUTING_HEADER              43
#define FRAGMENT_HEADER             44
#define AUTHENTICATION_HEADER       51
#define DESTINATION_OPTION_HEADER   60
#define MLD_NH_ICMPV6               58
#define NO_NEXT_HEADER              59

#define MLD_SSMGROUP_RECD_SIZE      20
#define MLD_SSMSOURCE_RECD_SIZE     16

typedef struct ipv6hdr_s
{

#if !defined(CONFIG_CPU_BIG_ENDIAN)
        uint32   flowLable:20,
                    trfClass:8,
                    version:4;
#else
        uint32   version:4,
                    trfClass:8,
                    flowLable:20;
#endif
        uint16  tot_len;
        uint8    nextHead;
        uint8    hopLimit;
        uint8    sipv6[IPV6_ADDR_LEN];
        uint8    dipv6[IPV6_ADDR_LEN];
} ipv6hdr_t;


typedef struct mld_hdr_s
{
    uint8  type;                    /* type*/
    uint8  code;
    uint16 checksum;
    uint16 responseDelay;
    uint16 reserved;
    uint8   groupIpv6[IPV6_ADDR_LEN];
} mld_hdr_t;


typedef struct mldv2_qryhdr_s
{
    uint8  type;                    /* type*/
    uint8  code;
    uint16 checksum;
    uint16 responseDelay;
    uint16 reserved;
    uint8   groupIpv6[IPV6_ADDR_LEN];

    uint8 rsq;                      /* 4bit: reserved, 1bit: suppress router-side processing, 3bit: querier's robustness variable*/
    uint8 qqic;                     /* querier's query interval code */
    uint16 numOfSrc;                /* number of sources */
    ipv6_addr_t srcList[0];                 /* first entry of source list */
} mldv2_qryhdr_t;

typedef struct mldv2_grec_s
{
    uint8   grec_type;
    uint8   grec_auxwords;
    uint16  grec_nsrcs;
    uint8    grec_ipv6Mca[IPV6_ADDR_LEN];
    ipv6_addr_t grec_src[0];
} mldv2_grec_t;

typedef struct mldv2_hdr_s
{
    uint8  type;                    /* type*/
    uint8  reserved1;
    uint16 checksum;
    uint16 reserved2;
    uint16 numRcd;
    mldv2_grec_t grec[0];
}mldv2_report_t;


typedef struct mld_ext_hdr_s
{
    uint8   nextHdr;
    uint8   hdrLen;
    uint16 reserved;
}mld_ext_hdr_t;


extern int32 mcast_igmp_rx(sys_nic_pkt_t *pPkt, void *pCookie, struct timespec *pRecvTime);

extern int32 mcast_mld_rx(sys_nic_pkt_t *pPkt, void *pCookie, struct timespec *pRecvTime);

//l2g_act_t  mcast_mld_preCheck(sys_nic_pkt_t *pPkt, void *pCookie);

#endif

