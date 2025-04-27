/*
* ----------------------------------------------------------------
* Copyright c              Realtek Semiconductor Corporation, 2002
* All rights reserved.
* 
* Abstract: header stamp module header file.
*
* $Author: yjlou $
* $Id: hsModel.h,v 1.26 2006-11-24 05:26:40 yjlou Exp $
*/

#ifndef _HS_MODEL_H_
#define _HS_MODEL_H_

#include "rtl_types.h"

/* HSB (Header Stamp Before):
 * Software-friendly structure definition */
struct hsb_param_s
{
	uint32 spa:3;       /* [2:0] source port number 0~5: rx port0-5,  7: cpu & extension ports (includes extension ports, see also extspa ) */
	uint32 trigpkt:1;   /* [3:3] This is a Triggered packet for Smartbit-like function. */
	uint32 len:15;      /* [15:0] BYTECOUNT FOR THE PACKET, included L2 CRC */
	uint32 vid:12;      /* [11:0] VLANID for normal packets and CPU-DirecTX packets */
	uint32 tagif:1;     /*        To indicate if the incoming packet is VLAN tagged */
	uint32 pppoeif:1;   /*        To indicate if the incoming packet is a PPPoE packet */
	uint32 sip:32;      /* [31:0] Source IP */
	uint32 sprt:16;     /* [15:0] Source Port (TCP/UDP) / ICMPID (ICMP) / L3 Checksum (DirectTx) / Call ID (PPTP) */
	uint32 dip:32;      /* [31:0] Destination IP */
	uint32 dprt:16;     /* [15:0] Destination Port / L4 Checksum (DirectTx) */
	uint32 ipptl:8;     /* [7:0] 8 bit protocol in IP header (IP packet)ICMP Type (ICMP)IGMP Type (IGMP) */
	uint32 ipfg:3;      /* [2:0] Flag field in IP header=[IP_NONFG,IP_DF, IP_MF] */
	uint32 iptos:8;     /* [7:0] TOS 8 bits in IP header (IP packet) Reason to CPU (DirectTx)*/
	uint32 tcpfg:8;     /* [7:0]=TCP flag or ICMP code */
	uint32 type:3;      /* [2:0] The type categories after Input packet parsing flow. 000: Ethernet, 001: PPTP, 010: IPv4, 011: ICMP, 100: IGMP, 101: TCP, 110: UDP, 111:IPv6 */
	uint32 patmatch:1;  /*       To indicate if the per-port pattern match is matched and corresponding action. Bit0: Match or not? */
	uint32 ethtype:16;  /* [15:0] EtherType{ DoVLANTAG[5:0], PortMask [5:0], DoPPPoETAG, PPPoEID[2:0]} for DirectTx. When LLC=1, EtherType should be the 16 bit value after AA-AA-03 00-00-00. */
	uint8 da[6];        /* [47:0] Destination MAC */
	uint8 sa[6];        /* [47:0] Source MAC */
	uint32 hiprior:3;	/* [2:0] User Priority from VLAN.pri[2:0] */
	uint32 snap:1;      /*       With LLC-SNAP hader */
	uint32 udpnocs:1;   /*       UDP and checksum=0x0000 */
	uint32 ttlst:2;     /* [1:0] 00: TTL=0, 01: TTL=1, 10: TTL>1 */
	uint32 dirtx:1;     /*       Direct to CPU or from CPU */
	uint32 l3csok:1;    /*       L3 checksum OK */
	uint32 l4csok:1;    /*       L4 checksum OK */
	uint32 ipfo0_n:1;   /*       IP fragmentation offset status. 0: IP fragmentation offset=0, 1: not 0 */
	uint32 llcothr:1;   /*       LLC header with {DSAP, SSAP} != AAAA */
	uint32 urlmch:1;    /*       To indicate if the packet is URL trapped */
	uint32 extspa:2;	/* [1:0] Extension Source Port Address ( valid only if SPA = 7 ). 2-0: extension port 2~0. 3: cpu*/
	uint32 extl2:1;     /*       S/W Directs ALE to do L2 lookup only for those packets from extension ports; it's valid if the incoming packets are from extension ports */
	uint32 linkid:7;    /*       WLAN link ID; if is valid only if the packets are from extension ports */
	uint32 pppoeid:16;  /*       PPPoE ID */
};
typedef struct hsb_param_s hsb_param_t;


/* HSA (Header Stamp After):
 * Software-friendly structure definition */
struct hsa_param_s
{
	uint8 nhmac[6];     /* [47:0] ARP MAC (next hop MAC address) */
	uint32 trip:32;     /* [31:0] Translated IP */
	uint32 port:16;     /* [15:0] Translated PORT/ICMP ID (type= ICMP) /Call ID (type=PPTP) */
	uint32 l3csdt:16;   /* [15:0] The substrate distance to fix the L3 Checksum. It requires to consider the TTL-1 simultaneously. */
	uint32 l4csdt:16;   /* [15:0] The substrate distance to fix the L4 Checksum. It requires to consider the TTL-1 simultaneously*/
	uint32 egif:1;      /*        IF the packet is from internal VLAN. */
	uint32 l2tr:1;      /*        To indicate if L2 (MAC Address) translation is needed. */
	uint32 l34tr:1;     /*        To indicate if L3 (IP) translation and L4 (PORT) translation is needed. (boundled because the L3 translation will influence the L4 checksum insertion). */
	uint32 dirtxo:1;    /*        Direct to CPU or from CPU(for CRC auto-insertion to distinguish L2 switch or from CPU packets) */
	uint32 typeo:3;     /* [2:0] The type categories that outputs. 000: Ethernet, 001: PPTP, 010: IPv4, 011: ICMP, 100: IGMP, 101: TCP, 110: UDP, 111:IPv6 */
	uint32 snapo:1;     /*       1: SNAP exists (copy from hsb.snap) */
	uint32 rxtag:1;	    /*       Indicate the rx-packet carries tag header */
	uint32 dvid:12;	    /*       The destination VLAN, for VLAN Tagging. */
	uint32 pppoeifo:2;  /* [1:0] To indicate if PPPoE session stage header is needed to be tagged. 00: intact, 01: tagging, 10: remove, 11: modify */
	uint32 pppidx:3;    /* [2:0] The PPPoE Session ID Index (to a 8-entry register table) for tagging in the translated packet header if needed. */
	uint32 leno:15;     /* [14:0] Packet length */
	uint32 l3csoko:1;   /*        L3 CRC OK? */
	uint32 l4csoko:1;   /*        L4 CRC OK? */
	uint32 frag:1;      /*        If this packet is fragment packet? */
	uint32 lastfrag:1;  /*        If this packet is the last fragment packet? */
	uint32 ipmcastr:1;  /*        Routed IP Multicast packet */
	uint32 svid:12;     /* [11:0] Source vid */
	uint32 fragpkt:1;   /*        Enable output port to fragmentize this packet */
	uint32 ttl_1if:9;   /* [8:0] Per MAC port TTL operation indication */
	uint32 dpc:3;       /* [2:0] Destination ports count */
	uint32 spao:4;      /* [3:0] Packet source Port ( refer HSB.SPA definitation ) */
	uint32 hwfwrd:1;    /* [0:0] Hardware forward flag. ( S/W use ) */
	uint32 dpext:4;     /* [3:0] Packet destination indication : 0-2: ext port0-2 3: cpu */
	uint32 spaext:2;    /* [1:0] ( refer HSB.EXTSPA definitation ) */
	uint32 why2cpu:16;  /* [15:0] CPU reason */
	uint32 spcp:3;      /* [2:0] Source priority code point */
	uint32 dvtag:9;     /* [8:0] Destination VLAN tag set: 0-5: port 0-5, 6-8: extension port 0-2 */
	uint32 difid:3;     /* [2:0] Destination Interface ID ( MAC uses this to get gateway MAC if L2Trans = 1) */
	uint32 linkid:7;    /* [6:0] WLAN link ID; if is valid only if the packets are from extension ports. 0: this field is invalid. */
	uint32 siptos:8;    /* [7:0] Source IPToS for those packets which were delivered to extension ports. */
	uint32 dp:7;        /* [6:0] destinatio port mask, formally not included in HSA. */
	uint32 priority:3;  /* [2:0] priority ID (valid: 0~7) */
};
typedef struct hsa_param_s hsa_param_t;


/* To directly access ASIC-mapped header stamp, we must set pack pragma. */
#pragma pack(push,1)

/* RAW HSB: Raw structure to access ASIC.
 * The structure is directly mapped to ASIC, however, it is not friendly for software. */
struct hsb_s
{
#ifdef _LITTLE_ENDIAN
	uint32 spa:3;       /* W0[2:0] */
	uint32 trigpkt:1;   /* W0[3:3] */
	uint32 resv:1;      /* W0[4:4] */
	uint32 len:15;      /* W0[19:5] */
	uint32 vid:12;      /* W0[31:20] */
#else
	uint32 vid:12;      /* W0[31:20] */
	uint32 len:15;      /* W0[19:5] */
	uint32 resv:1;      /* W0[4:4] */
	uint32 trigpkt:1;   /* W0[3:3] */
	uint32 spa:3;       /* W0[2:0] */
#endif
	
#ifdef _LITTLE_ENDIAN
	uint32 tagif:1;     /* W1[0:0] */
	uint32 pppoeif:1;   /* W1[1:1] */
	uint32 sip29_0:30;  /* W1[31:2] */
#else
	uint32 sip29_0:30;  /* W1[31:2] */
	uint32 pppoeif:1;   /* W1[1:1] */
	uint32 tagif:1;     /* W1[0:0] */
#endif
	
#ifdef _LITTLE_ENDIAN
	uint32 sip31_30:2;  /* W2[1:0] */
	uint32 sprt:16;     /* W2[17:2] */
	uint32 dip13_0:14;  /* W2[31:18] */
#else
	uint32 dip13_0:14;  /* W2[31:18] */
	uint32 sprt:16;     /* W2[17:2] */
	uint32 sip31_30:2;  /* W2[1:0] */
#endif
	
#ifdef _LITTLE_ENDIAN
	uint32 dip31_14:18; /* W3[17:0] */
	uint32 dprt13_0:14; /* W3[31:18] */
#else
	uint32 dprt13_0:14; /* W3[31:18] */
	uint32 dip31_14:18; /* W3[17:0] */
#endif
	
#ifdef _LITTLE_ENDIAN
	uint32 dprt15_14:2; /* W4[1:0] */
	uint32 ipptl:8;     /* W4[9:2] */
	uint32 ipfg:3;      /* W4[12:10]=[IP_NONFG,IP_DF, IP_MF] */
	uint32 iptos:8;     /* W4[20:13] */
	uint32 tcpfg:8;     /* W4[28:21]=TCP flag or ICMP code */
	uint32 type:3;      /* W4[31:0] */
#else
	uint32 type:3;      /* W4[31:0] */
	uint32 tcpfg:8;     /* W4[28:21]=TCP flag or ICMP code */
	uint32 iptos:8;     /* W4[20:13] */
	uint32 ipfg:3;      /* W4[12:10]=[IP_NONFG,IP_DF, IP_MF] */
	uint32 ipptl:8;     /* W4[9:2] */
	uint32 dprt15_14:2; /* W4[1:0] */
#endif
	
#ifdef _LITTLE_ENDIAN
	uint32 patmatch:1;  /* W5[0:0] */
	uint32 ethtype:16;  /* W5[16:1] */
	uint32 da14_0:15;   /* W5[31:17] */
#else
	uint32 da14_0:15;   /* W5[31:17] */
	uint32 ethtype:16;  /* W5[16:1] */
	uint32 patmatch:1;  /* W5[0:0] */
#endif
	
	uint32 da46_15:32;  /* W6[31:0] */
	
#ifdef _LITTLE_ENDIAN
	uint32 da47_47:1;   /* W7[0:0] */
	uint32 sa30_0:31;   /* W7[31:1] */
#else
	uint32 sa30_0:31;   /* W7[31:1] */
	uint32 da47_47:1;   /* W7[0:0] */
#endif
	
#ifdef _LITTLE_ENDIAN
	uint32 sa47_31:17;  /* W8[16:0] */
	uint32 hiprior:3;	/* W8[19:17] */
	uint32 snap:1;      /* W8[20:20] */
	uint32 udpnocs:1;   /* W8[21:21] */
	uint32 ttlst:2;     /* W8[23:22] */
	uint32 dirtx:1;     /* W8[24:24] */
	uint32 l3csok:1;    /* W8[25:25] */
	uint32 l4csok:1;    /* W8[26:26] */
	uint32 ipfo0_n:1;   /* W8[27:27] */
	uint32 llcothr:1;   /* W8[28:28] */
	uint32 urlmch:1;    /* W8[29:29] */
	uint32 extspa:2;	/* W8[31:30] */
#else
	uint32 extspa:2;	/* W8[31:30] */
	uint32 urlmch:1;    /* W8[29:29] */
	uint32 llcothr:1;   /* W8[28:28] */
	uint32 ipfo0_n:1;   /* W8[27:27] */
	uint32 l4csok:1;    /* W8[26:26] */
	uint32 l3csok:1;    /* W8[25:25] */
	uint32 dirtx:1;     /* W8[24:24] */
	uint32 ttlst:2;     /* W8[23:22] */
	uint32 udpnocs:1;   /* W8[21:21] */
	uint32 snap:1;      /* W8[20:20] */
	uint32 hiprior:3;	/* W8[19:17] */
	uint32 sa47_31:17;  /* W8[16:0] */
#endif
	
#ifdef _LITTLE_ENDIAN
	uint32 extl2:1;     /* W9[0:0] */
	uint32 linkid:7;    /* W9[7:1] */
	uint32 pppoeid:16;  /* W9[23:8] */
	uint32 res1:8;      /* W9[31:24] */
#else
	uint32 res1:8;      /* W9[31:24] */
	uint32 pppoeid:16;  /* W9[23:8] */
	uint32 linkid:7;    /* W9[7:1] */
	uint32 extl2:1;     /* W9[0:0] */
#endif
};
typedef struct hsb_s hsb_t;


/* RAW HSA: Raw structure to access ASIC.
 * The structure is directly mapped to ASIC, however, it is not friendly for software. */
struct hsa_s
{
#ifdef _LITTLE_ENDIAN
	uint32 nhmac0:8;    /* W0[7:0] */
	uint32 nhmac1:8;    /* W0[15:8] */
	uint32 nhmac2:8;    /* W0[23:16] */
	uint32 nhmac3:8;    /* W0[31:24] */
#else
	uint32 nhmac3:8;    /* W0[31:24] */
	uint32 nhmac2:8;    /* W0[23:16] */
	uint32 nhmac1:8;    /* W0[15:8] */
	uint32 nhmac0:8;    /* W0[7:0] */
#endif
	
#ifdef _LITTLE_ENDIAN
	uint32 nhmac4:8;    /* W1[7:0] */
	uint32 nhmac5:8;    /* W1[15:8] */
	uint32 trip15_0:16; /* W1[31:16] */
#else
	uint32 trip15_0:16; /* W1[31:16] */
	uint32 nhmac5:8;    /* W1[15:8] */
	uint32 nhmac4:8;    /* W1[7:0] */
#endif
	
#ifdef _LITTLE_ENDIAN
	uint32 trip31_16:16;/* W2[15:0] */
	uint32 port:16;		/* W2[31:16] */
#else
	uint32 port:16;		/* W2[31:16] */
	uint32 trip31_16:16;/* W2[15:0] */
#endif

#ifdef _LITTLE_ENDIAN
	uint32 l3csdt:16;	/* W3[15:0] */
	uint32 l4csdt:16;	/* W3[31:16] */
#else
	uint32 l4csdt:16;	/* W3[31:16] */
	uint32 l3csdt:16;	/* W3[15:0] */
#endif
	
#ifdef _LITTLE_ENDIAN
	uint32 egif:1;      /* W4[0:0] */
	uint32 l2tr:1;      /* W4[1:1] */
	uint32 l34tr:1;     /* W4[2:2] */
	uint32 dirtxo:1;    /* W4[3:3] */
	uint32 typeo:3;		/* W4[6:4] */
	uint32 snapo:1;     /* W4[7:7] */
	uint32 rxtag:1;     /* W4[8:8] */
	uint32 dvid:12;     /* W4[20:9] */
	uint32 pppoeifo:2;  /* W4[22:21] */
	uint32 pppidx:3;    /* W4[25:23] */
	uint32 leno5_0:6;   /* W4[31:26] */
#else
	uint32 leno5_0:6;   /* W4[31:26] */
	uint32 pppidx:3;    /* W4[25:23] */
	uint32 pppoeifo:2;  /* W4[22:21] */
	uint32 dvid:12;     /* W4[20:9] */
	uint32 rxtag:1;     /* W4[8:8] */
	uint32 snapo:1;     /* W4[7:7] */
	uint32 typeo:3;		/* W4[6:4] */
	uint32 dirtxo:1;    /* W4[3:3] */
	uint32 l34tr:1;     /* W4[2:2] */
	uint32 l2tr:1;      /* W4[1:1] */
	uint32 egif:1;      /* W4[0:0] */
#endif
	
#ifdef _LITTLE_ENDIAN
	uint32 leno14_6:9;  /* W5[8:0] */
	uint32 l3csoko:1;   /* W5[9:9] */
	uint32 l4csoko:1;   /* W5[10:10] */
	uint32 frag:1;      /* W5[11:11] */
	uint32 lastfrag:1;  /* W5[12:12] */
	uint32 ipmcastr:1;  /* W5[13:13] */
	uint32 svid:12;     /* W5[25:14] */
	uint32 fragpkt:1;   /* W5[26:26] */
	uint32 ttl_1if4_0:5;/* W5[31:27] */
#else
	uint32 ttl_1if4_0:5;/* W5[31:27] */
	uint32 fragpkt:1;   /* W5[26:26] */
	uint32 svid:12;     /* W5[25:14] */
	uint32 ipmcastr:1;  /* W5[13:13] */
	uint32 lastfrag:1;  /* W5[12:12] */
	uint32 frag:1;      /* W5[11:11] */
	uint32 l4csoko:1;   /* W5[10:10] */
	uint32 l3csoko:1;   /* W5[9:9] */
	uint32 leno14_6:9;  /* W5[8:0] */
#endif
	
#ifdef _LITTLE_ENDIAN
	uint32 ttl_1if5_5:1;/* W6[0:0] */
	uint32 dpc:3;       /* W6[3:1] */
	uint32 spao:4;      /* W6[7:4] */
	uint32 hwfwrd:1;    /* W6[8:8] */
	uint32 ttl_1if8_6:3;/* W6[11:9] */
	uint32 dpext:4;     /* W6[15:12] */
	uint32 spaext:2;    /* W6[17:16] 0~3 */
	uint32 why2cpu13_0:14;/* W6[31:18] */
#else
	uint32 why2cpu13_0:14;/* W6[31:18] */
	uint32 spaext:2;    /* W6[17:16] 0~3 */
	uint32 dpext:4;     /* W6[15:12] */
	uint32 ttl_1if8_6:3;/* W6[11:9] */
	uint32 hwfwrd:1;    /* W6[8:8] */
	uint32 spao:4;      /* W6[7:4] */
	uint32 dpc:3;       /* W6[3:1] */
	uint32 ttl_1if5_5:1;/* W6[0:0] */
#endif
	
#ifdef _LITTLE_ENDIAN
	uint32 why2cpu15_14:2;/* W7[1:0] */
	uint32 spcp:3;      /* W7[4:2] */
	uint32 dvtag:9;     /* W7[13:5] */
	uint32 difid:3;     /* W7[16:14] */
	uint32 linkid:7;    /* W7[23:17]*/
	uint32 siptos:8;    /* W7[31:24] */
#else
	uint32 siptos:8;    /* W7[31:24] */
	uint32 linkid:7;    /* W7[23:17]*/
	uint32 difid:3;     /* W7[16:14] */
	uint32 dvtag:9;     /* W7[13:5] */
	uint32 spcp:3;      /* W7[4:2] */
	uint32 why2cpu15_14:2;/* W7[1:0] */
#endif

#ifdef _LITTLE_ENDIAN
	uint32 dp6_0:7;     /* W8[6_0]=destinatio port mask*/
	uint32 resv2:25;    /* W8[31:7] */
#else
	uint32 resv2:25;    /* W8[31:7] */
	uint32 dp6_0:7;     /* W8[6:0]=destinatio port mask*/
#endif

#ifdef _LITTLE_ENDIAN
        uint32 priority:3;/* W9[6:0] */
        uint32 resv3:29;
#else
        uint32 resv3:29;
        uint32 priority:3;/* W9[6:0] */
#endif



};
typedef struct hsa_s hsa_t;

#pragma pack(pop)
/* Recover pragma setting */


int32 modelGetHsb( hsb_param_t* hsb );
int32 modelSetHsb( hsb_param_t* hsb );
int32 modelGetHsa( hsa_param_t* hsa );
int32 modelSetHsa( hsa_param_t* hsa );


enum HSB_TYPE 
{
	HSB_TYPE_ETHERNET = 0,
	HSB_TYPE_PPTP = 1,
	HSB_TYPE_IP = 2,
	HSB_TYPE_ICMP = 3,
	HSB_TYPE_IGMP = 4,
	HSB_TYPE_TCP = 5,
	HSB_TYPE_UDP = 6,
	HSB_TYPE_IPV6 = 7,
};

enum HSA_TYPE 
{
	HSA_TYPE_ETHERNET = 0,
	HSA_TYPE_PPTP = 1,
	HSA_TYPE_IP = 2,
	HSA_TYPE_ICMP = 3,
	HSA_TYPE_IGMP = 4,
	HSA_TYPE_TCP = 5,
	HSA_TYPE_UDP = 6,
	HSA_TYPE_IPV6 = 7,
};


#endif /* _HS_H_ */
