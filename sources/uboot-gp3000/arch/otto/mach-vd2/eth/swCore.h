/*
* ----------------------------------------------------------------
* Copyright c                  Realtek Semiconductor Corporation, 2002
* All rights reserved.
*
* $Header: /usr/local/dslrepos/u-boot-2011.12/arch/mips/cpu/rlx5281/rtl8676/swCore.h,v 1.1.1.1 2012/02/01 07:50:52 yachang Exp $
*
* Abstract: Switch core header file.
*
* $Author: yachang $
*
* $Log: swCore.h,v $
* Revision 1.1.1.1  2012/02/01 07:50:52  yachang
* First working u-boot for RTL8676
*
*
* Revision 1.2  2011/09/08 09:49:36  cathy
* add for external phy detection
*
* Revision 1.1.1.1  2011/06/10 08:06:31  yachang
*
*
* Revision 1.5  2006/09/15 03:53:39  ghhuang
* +: Add TFTP download support for RTL8652 FPGA
*
* Revision 1.4  2005/09/22 05:22:31  bo_zhao
* *** empty log message ***
*
* Revision 1.1.1.1  2005/09/05 12:38:24  alva
* initial import for add TFTP server
*
* Revision 1.3  2004/03/31 01:49:20  yjlou
* *: all text files are converted to UNIX format.
*
* Revision 1.2  2004/03/30 11:34:38  yjlou
* *: commit for 80B IC back
*   +: system clock rate definitions have changed.
*   *: fixed the bug of BIST_READY_PATTERN
*   +: clean all ASIC table when init ASIC.
* -: define _L2_MODE_ to support L2 switch mode.
*
* Revision 1.1  2004/03/16 06:36:13  yjlou
* *** empty log message ***
*
* Revision 1.2  2004/03/16 06:04:04  yjlou
* +: support pure L2 switch mode (for hardware testing)
*
* Revision 1.1.1.1  2003/09/25 08:16:56  tony
*  initial loader tree
*
* Revision 1.1.1.1  2003/05/07 08:16:07  danwu
* no message
*
* ---------------------------------------------------------------
*/

#ifndef _SWCORE_H
#define _SWCORE_H

#define CONFIG_RTL865XC 	1
#define CONFIG_RTL8196D 	1
#define CONFIG_RTL8196C 	1

#define SWNIC_DEBUG
#define SWTABLE_DEBUG
#define SWCORE_DEBUG

typedef uint32_t 	memaddr;
typedef uint32_t 	ipaddr_t;

typedef struct {
    uint16_t 	mac47_32;
    uint16_t 	mac31_16;
    uint16_t 	mac15_0;
    uint16_t 	align;
} macaddr_t;

typedef int8_t* 	calladdr_t;

typedef struct ether_addr_s {
    uint8_t octet[6];
} ether_addr_t;

/* --------------------------------------------------------------------
 * ROUTINE NAME - swCore_init
 * --------------------------------------------------------------------
 * FUNCTION: This service initializes the switch core.
 * INPUT   : None.
 * OUTPUT  : None.
 * RETURN  : Upon successful completion, the function returns 0.
        Otherwise,
		ENFILE: Destination slot of vlan table is occupied.
 * NOTE    : None.
 * -------------------------------------------------------------------*/
int swCore_init(void);

/* ACL Rule Action type Definition */
#define RTL8651_ACL_PERMIT			0x01
#define RTL8651_ACL_DROP				0x02
#define RTL8651_ACL_CPU				0x03
#define RTL8651_ACL_DROP_LOG			0x04
#define RTL8651_ACL_DROP_NOTIFY		0x05
#define RTL8651_ACL_L34_DROP			0x06	/* special for default ACL rule */

/* ACL Rule type Definition */
#define RTL8651_ACL_MAC				0x00
#define RTL8651_ACL_IP					0x01
#define RTL8651_ACL_ICMP				0x02
#define RTL8651_ACL_IGMP				0x03
#define RTL8651_ACL_TCP					0x04
#define RTL8651_ACL_UDP				0x05
/* 6-8*/
#define RTL8652_ACL_IP_RANGE			0x0A
#define RTL8652_ACL_ICMP_IPRANGE		0x0B
#define RTL8652_ACL_TCP_IPRANGE		0x0C
#define RTL8652_ACL_IGMP_IPRANGE		0x0D
#define RTL8652_ACL_UDP_IPRANGE		0x0E
#define RTL8652_ACL_SRCFILTER_IPRANGE 0x09
#define RTL8652_ACL_DSTFILTER_IPRANGE 0x0F

/* For PktOpApp */
#define RTL8651_ACLTBL_BACKWARD_COMPATIBLE	0 /* For backward compatible */
#define RTL865XC_ACLTBL_ALL_LAYER			RTL8651_ACLTBL_BACKWARD_COMPATIBLE
#define RTL8651_ACLTBL_ONLY_L2				1 /* Only for L2 switch */
#define RTL8651_ACLTBL_ONLY_L3				2 /* Only for L3 routing (including IP multicast) */
#define RTL8651_ACLTBL_L2_AND_L3			3 /* Only for L2 switch and L3 routing (including IP multicast) */
#define RTL8651_ACLTBL_ONLY_L4				4 /* Only for L4 translation packets */
#define RTL8651_ACLTBL_L3_AND_L4			6 /* Only for L3 routing and L4 translation packets (including IP multicast) */
#define RTL8651_ACLTBL_NOOP				7 /* No operation. Don't apply this rule. */

/* MAC ACL rule Definition */
#define dstMac_				un_ty.MAC._dstMac
#define dstMacMask_			un_ty.MAC._dstMacMask
#define srcMac_				un_ty.MAC._srcMac
#define srcMacMask_			un_ty.MAC._srcMacMask
#define typeLen_				un_ty.MAC._typeLen
#define typeLenMask_			un_ty.MAC._typeLenMask

/* IFSEL ACL rule Definition */
#define gidxSel_				un_ty.IFSEL._gidxSel

/* Common IP ACL Rule Definition */
#define srcIpAddr_				un_ty.L3L4._srcIpAddr
#define srcIpAddrMask_			un_ty.L3L4._srcIpAddrMask
#define srcIpAddrUB_				un_ty.L3L4._srcIpAddr
#define srcIpAddrLB_				un_ty.L3L4._srcIpAddrMask
#define dstIpAddr_				un_ty.L3L4._dstIpAddr
#define dstIpAddrMask_			un_ty.L3L4._dstIpAddrMask
#define dstIpAddrUB_				un_ty.L3L4._dstIpAddr
#define dstIpAddrLB_				un_ty.L3L4._dstIpAddrMask
#define tos_					un_ty.L3L4._tos
#define tosMask_				un_ty.L3L4._tosMask
/* IP Rrange */
#define srcIpAddrStart_			un_ty.L3L4._srcIpAddr
#define srcIpAddrEnd_			un_ty.L3L4._srcIpAddrMask
#define dstIpAddrStart_			un_ty.L3L4._dstIpAddr
#define dstIpAddrEnd_			un_ty.L3L4._dstIpAddrMask

/* IP ACL Rule Definition */
#define ipProto_				un_ty.L3L4.is.ip._proto
#define ipProtoMask_			un_ty.L3L4.is.ip._protoMask
#define ipFlagMask_			un_ty.L3L4.is.ip._flagMask
#if 1 //chhuang: #ifdef RTL8650B
#define ipFOP_      				un_ty.L3L4.is.ip._FOP
#define ipFOM_      				un_ty.L3L4.is.ip._FOM
#define ipHttpFilter_      			un_ty.L3L4.is.ip._httpFilter
#define ipHttpFilterM_			un_ty.L3L4.is.ip._httpFilterM
#define ipIdentSrcDstIp_   		un_ty.L3L4.is.ip._identSrcDstIp
#define ipIdentSrcDstIpM_		un_ty.L3L4.is.ip._identSrcDstIpM
#endif /* RTL8650B */
#define ipFlag_				un_ty.L3L4.is.ip.un._flag
#define ipDF_					un_ty.L3L4.is.ip.un.s._DF
#define ipMF_					un_ty.L3L4.is.ip.un.s._MF

/* ICMP ACL Rule Definition */
#define icmpType_				un_ty.L3L4.is.icmp._type
#define icmpTypeMask_			un_ty.L3L4.is.icmp._typeMask
#define icmpCode_				un_ty.L3L4.is.icmp._code
#define icmpCodeMask_			un_ty.L3L4.is.icmp._codeMask

/* IGMP ACL Rule Definition */
#define igmpType_				un_ty.L3L4.is.igmp._type
#define igmpTypeMask_			un_ty.L3L4.is.igmp._typeMask

/* TCP ACL Rule Definition */
#define tcpl2srcMac_				un_ty.L3L4.is.tcp._l2srcMac		// for srcMac & destPort ACL rule
#define tcpl2srcMacMask_			un_ty.L3L4.is.tcp._l2srcMacMask
#define tcpSrcPortUB_			un_ty.L3L4.is.tcp._srcPortUpperBound
#define tcpSrcPortLB_			un_ty.L3L4.is.tcp._srcPortLowerBound
#define tcpDstPortUB_			un_ty.L3L4.is.tcp._dstPortUpperBound
#define tcpDstPortLB_			un_ty.L3L4.is.tcp._dstPortLowerBound
#define tcpFlagMask_			un_ty.L3L4.is.tcp._flagMask
#define tcpFlag_				un_ty.L3L4.is.tcp.un._flag
#define tcpURG_				un_ty.L3L4.is.tcp.un.s._urg
#define tcpACK_				un_ty.L3L4.is.tcp.un.s._ack
#define tcpPSH_				un_ty.L3L4.is.tcp.un.s._psh
#define tcpRST_				un_ty.L3L4.is.tcp.un.s._rst
#define tcpSYN_				un_ty.L3L4.is.tcp.un.s._syn
#define tcpFIN_				un_ty.L3L4.is.tcp.un.s._fin

/* UDP ACL Rule Definition */
#define udpl2srcMac_				un_ty.L3L4.is.udp._l2srcMac		// for srcMac & destPort ACL rule
#define udpl2srcMacMask_		un_ty.L3L4.is.udp._l2srcMacMask
#define udpSrcPortUB_			un_ty.L3L4.is.udp._srcPortUpperBound
#define udpSrcPortLB_			un_ty.L3L4.is.udp._srcPortLowerBound
#define udpDstPortUB_			un_ty.L3L4.is.udp._dstPortUpperBound
#define udpDstPortLB_			un_ty.L3L4.is.udp._dstPortLowerBound

#if 1 //chhuang: #ifdef RTL8650B
/* Source Filter ACL Rule Definition */
#define srcFilterMac_				un_ty.SRCFILTER._srcMac
#define srcFilterMacMask_		un_ty.SRCFILTER._srcMacMask
#define srcFilterPort_				un_ty.SRCFILTER._srcPort
#define srcFilterPortMask_		un_ty.SRCFILTER._srcPortMask
#define srcFilterVlanIdx_			un_ty.SRCFILTER._srcVlanIdx
#define srcFilterVlanId_			un_ty.SRCFILTER._srcVlanIdx
#define srcFilterVlanIdxMask_		un_ty.SRCFILTER._srcVlanIdxMask
#define srcFilterVlanIdMask_		un_ty.SRCFILTER._srcVlanIdxMask
#define srcFilterIpAddr_			un_ty.SRCFILTER._srcIpAddr
#define srcFilterIpAddrMask_		un_ty.SRCFILTER._srcIpAddrMask
#define srcFilterIpAddrUB_		un_ty.SRCFILTER._srcIpAddr
#define srcFilterIpAddrLB_		un_ty.SRCFILTER._srcIpAddrMask
#define srcFilterPortUpperBound_	un_ty.SRCFILTER._srcPortUpperBound
#define srcFilterPortLowerBound_	un_ty.SRCFILTER._srcPortLowerBound
#define srcFilterIgnoreL3L4_		un_ty.SRCFILTER._ignoreL3L4
#define srcFilterIgnoreL4_		un_ty.SRCFILTER._ignoreL4

/* Destination Filter ACL Rule Definition */
#define dstFilterMac_				un_ty.DSTFILTER._dstMac
#define dstFilterMacMask_		un_ty.DSTFILTER._dstMacMask
#define dstFilterVlanIdx_			un_ty.DSTFILTER._vlanIdx
#define dstFilterVlanIdxMask_		un_ty.DSTFILTER._vlanIdxMask
#define dstFilterVlanId_			un_ty.DSTFILTER._vlanIdx
#define dstFilterVlanIdMask_		un_ty.DSTFILTER._vlanIdxMask
#define dstFilterIpAddr_			un_ty.DSTFILTER._dstIpAddr
#define dstFilterIpAddrMask_		un_ty.DSTFILTER._dstIpAddrMask
#define dstFilterPortUpperBound_	un_ty.DSTFILTER._dstPortUpperBound
#define dstFilterIpAddrUB_		un_ty.DSTFILTER._dstIpAddr
#define dstFilterIpAddrLB_		un_ty.DSTFILTER._dstIpAddrMask
#define dstFilterPortLowerBound_	un_ty.DSTFILTER._dstPortLowerBound
#define dstFilterIgnoreL3L4_		un_ty.DSTFILTER._ignoreL3L4
#define dstFilterIgnoreL4_		un_ty.DSTFILTER._ignoreL4
#endif /* RTL8650B */

/* ACL access parameters */
typedef struct {
    union {
        /* MAC ACL rule */
        struct {
            ether_addr_t _dstMac, _dstMacMask;
            ether_addr_t _srcMac, _srcMacMask;
            uint16_t _typeLen, _typeLenMask;
        } MAC;
        /* IFSEL ACL rule */
        struct {
            uint8_t _gidxSel;
        } IFSEL;
        /* IP Group ACL rule */
        struct {
            ipaddr_t _srcIpAddr, _srcIpAddrMask;
            ipaddr_t _dstIpAddr, _dstIpAddrMask;
            uint8_t _tos, _tosMask;
            union {
                /* IP ACL rle */
                struct {
                    uint8_t _proto, _protoMask, _flagMask;// flag & flagMask only last 3-bit is meaning ful
#if 1 //chhuang: #ifdef RTL8650B
                    uint32_t _FOP:1, _FOM:1, _httpFilter:1, _httpFilterM:1, _identSrcDstIp:1, _identSrcDstIpM:1;
#endif /* RTL8650B */
                    union {
                        uint8_t _flag;
                        struct {
                            uint8_t pend1:5,
                                    pend2:1,
                                    _DF:1,	//don't fragment flag
                                    _MF:1;	//more fragments flag
                        } s;
                    } un;
                } ip;
                /* ICMP ACL rule */
                struct {
                    uint8_t _type, _typeMask, _code, _codeMask;
                } icmp;
                /* IGMP ACL rule */
                struct {
                    uint8_t _type, _typeMask;
                } igmp;
                /* TCP ACL rule */
                struct {
                    ether_addr_t _l2srcMac, _l2srcMacMask;	// for srcMac & destPort ACL rule
                    uint8_t _flagMask;
                    uint16_t _srcPortUpperBound, _srcPortLowerBound;
                    uint16_t _dstPortUpperBound, _dstPortLowerBound;
                    union {
                        uint8_t _flag;
                        struct {
                            uint8_t _pend:2,
                                    _urg:1, //urgent bit
                                    _ack:1, //ack bit
                                    _psh:1, //push bit
                                    _rst:1, //reset bit
                                    _syn:1, //sync bit
                                    _fin:1; //fin bit
                        } s;
                    } un;
                } tcp;
                /* UDP ACL rule */
                struct {
                    ether_addr_t _l2srcMac, _l2srcMacMask;	// for srcMac & destPort ACL rule
                    uint16_t _srcPortUpperBound, _srcPortLowerBound;
                    uint16_t _dstPortUpperBound, _dstPortLowerBound;
                } udp;
            } is;
        } L3L4;
#if 1 //chhuang: #ifdef RTL8650B
        /* Source filter ACL rule */
        struct {
            ether_addr_t _srcMac, _srcMacMask;
            uint16_t _srcPort, _srcPortMask;
            uint16_t _srcVlanIdx, _srcVlanIdxMask;
            ipaddr_t _srcIpAddr, _srcIpAddrMask;
            uint16_t _srcPortUpperBound, _srcPortLowerBound;
            uint32_t _ignoreL3L4:1, //L2 rule
                     _ignoreL4:1; //L3 rule
        } SRCFILTER;
        /* Destination filter ACL rule */
        struct {
            ether_addr_t _dstMac, _dstMacMask;
            uint16_t _vlanIdx, _vlanIdxMask;
            ipaddr_t _dstIpAddr, _dstIpAddrMask;
            uint16_t _dstPortUpperBound, _dstPortLowerBound;
            uint32_t _ignoreL4:1, //L3 rule
                     _ignoreL3L4:1; //L2 rule
        } DSTFILTER;
#endif /* RTL8650B */
    } un_ty;
    uint32_t	ruleType_:4;
    uint32_t	actionType_:4;
#if 1	/* RTL8650B */
    uint32_t  	pktOpApp:3;
#endif	/* RTL8650B */
    uint32_t	isEgressRateLimitRule_:1;
    uint32_t	naptProcessType:4;
    uint32_t	naptProcessDirection:2;
    uint32_t	matchType_;

    uint16_t  	dsid; /* 2004/1/19 orlando */
    uint16_t	priority:3;
    uint32_t	dvid_:3;
    uint32_t	priority_:1;
    uint32_t	nextHop_:10;
    uint32_t  	pppoeIdx_:3;
    uint32_t	isIPRange_:1;			/* support IP Range ACL */
    uint32_t	isRateLimitCounter_:1;	/* support Rate Limit Counter Mode */
#if 1 //chhuang: #ifdef RTL8650B
    uint16_t  	nhIndex; //index of next hop table
    uint16_t  	rlIndex; //index of rate limit table
#endif /* RTL8650B */

    uint32_t	aclIdx;
} rtl_acl_param_t;




typedef struct {
    union {
        struct {
            /* word 0 */
            uint16_t          dMacP31_16;
            uint16_t          dMacP15_0;
            /* word 1 */
            uint16_t          dMacM15_0;
            uint16_t          dMacP47_32;
            /* word 2 */
            uint16_t          dMacM47_32;
            uint16_t          dMacM31_16;
            /* word 3 */
            uint16_t          sMacP31_16;
            uint16_t          sMacP15_0;
            /* word 4 */
            uint16_t          sMacM15_0;
            uint16_t          sMacP47_32;
            /* word 5 */
            uint16_t          sMacM47_32;
            uint16_t          sMacM31_16;
            /* word 6 */
            uint16_t          ethTypeM;
            uint16_t          ethTypeP;
        } ETHERNET;
        struct {
            /* word 0 */
            uint32_t          reserv1     : 24;
            uint32_t          gidxSel     : 8;
            /* word 1~6 */
            uint32_t          reserv2[6];
        } IFSEL;
        struct {
            /* word 0 */
            ipaddr_t        sIPP;
            /* word 1 */
            ipaddr_t        sIPM;
            /* word 2 */
            ipaddr_t        dIPP;
            /* word 3 */
            ipaddr_t        dIPM;
            union {
                struct {
                    /* word 4 */
                    uint8_t           IPProtoM;
                    uint8_t           IPProtoP;
                    uint8_t           IPTOSM;
                    uint8_t           IPTOSP;
                    /* word 5 */
                    uint32_t          reserv0     : 20;
                    uint32_t          identSDIPM  : 1;
                    uint32_t          identSDIPP  : 1;
                    uint32_t          HTTPM       : 1;
                    uint32_t          HTTPP       : 1;
                    uint32_t          FOM         : 1;
                    uint32_t          FOP         : 1;
                    uint32_t          IPFlagM     : 3;
                    uint32_t          IPFlagP     : 3;
                    /* word 6 */
                    uint32_t          reserv1;
                } IP;
                struct {
                    /* word 4 */
                    uint8_t           ICMPTypeM;
                    uint8_t           ICMPTypeP;
                    uint8_t           IPTOSM;
                    uint8_t           IPTOSP;
                    /* word 5 */
                    uint16_t          reserv0;
                    uint8_t           ICMPCodeM;
                    uint8_t           ICMPCodeP;
                    /* word 6 */
                    uint32_t          reserv1;
                } ICMP;
                struct {
                    /* word 4 */
                    uint8_t           IGMPTypeM;
                    uint8_t           IGMPTypeP;
                    uint8_t           IPTOSM;
                    uint8_t           IPTOSP;
                    /* word 5,6 */
                    uint32_t          reserv0[2];
                } IGMP;
                struct {
                    /* word 4 */
                    uint8_t           TCPFlagM;
                    uint8_t           TCPFlagP;
                    uint8_t           IPTOSM;
                    uint8_t           IPTOSP;
                    /* word 5 */
                    uint16_t          TCPSPLB;
                    uint16_t          TCPSPUB;
                    /* word 6 */
                    uint16_t          TCPDPLB;
                    uint16_t          TCPDPUB;
                } TCP;
                struct {
                    /* word 4 */
                    uint16_t          reserv0;
                    uint8_t           IPTOSM;
                    uint8_t           IPTOSP;
                    /* word 5 */
                    uint16_t          UDPSPLB;
                    uint16_t          UDPSPUB;
                    /* word 6 */
                    uint16_t          UDPDPLB;
                    uint16_t          UDPDPUB;
                } UDP;
            } is;
        } L3L4;

        struct {
            /* word 0 */
            uint16_t          sMacP31_16;
            uint16_t          sMacP15_0;
            /* word 1 */
            uint16_t          reserv1:3;
            uint16_t          spaP:9;
            uint16_t          sMacM3_0:4;
            uint16_t          sMacP47_32;
            /* word 2 */
            uint32_t	        reserv3:2;
            uint32_t          sVidM:12;
            uint32_t          sVidP:12;
            uint32_t		    reserv2:6;
            /* word 3 */
            uint32_t          reserv5     : 6;
            uint32_t          protoType   : 2;
            uint32_t          reserv4        : 24;
            /* word 4 */
            ipaddr_t        sIPP;
            /* word 5 */
            ipaddr_t        sIPM;
            /* word 6 */
            uint16_t          SPORTLB;
            uint16_t          SPORTUB;
        } SRC_FILTER;
        struct {
            /* word 0 */
            uint16_t          dMacP31_16;
            uint16_t          dMacP15_0;
            /* word 1 */
            uint16_t 	        vidP:12;
            uint16_t          dMacM3_0:4;
            uint16_t          dMacP47_32;
            /* word 2 */
            uint32_t          reserv2:20;
            uint32_t          vidM:12;
            /* word 3 */
            uint32_t          reserv4     : 24;
            uint32_t          protoType   : 2;
            uint32_t         reserv3:6;
            /* word 4 */
            ipaddr_t        dIPP;
            /* word 5 */
            ipaddr_t        dIPM;
            /* word 6 */
            uint16_t          DPORTLB;
            uint16_t          DPORTUB;
        } DST_FILTER;

    } is;
    /* word 7 */
    uint32_t          reserv0     : 5;
    uint32_t          pktOpApp    : 3;
    uint32_t          PPPoEIndex  : 3;
    uint32_t          vid         : 3;
    uint32_t          nextHop     : 10; //index of l2, next hop, or rate limit tables
    uint32_t          actionType  : 4;
    uint32_t          ruleType    : 4;
} rtl865xc_tblAsic_aclTable_t;


/* Netif access parameters */
typedef struct {
    macaddr_t       gMac;
    uint16_t          macAddrNumber;
    uint16_t          vid;
    uint32_t          inAclStart, inAclEnd, outAclStart, outAclEnd;
    uint32_t          mtu;
    uint32_t          enableRoute:1,
                      valid:1;
} rtl_netif_param_t;

typedef struct rtl865x_tblAsicDrv_intfParam_s {
    ether_addr_t macAddr;
    uint16_t 	macAddrNumber;
    uint16_t 	vid;
    uint32_t 	inAclStart, inAclEnd, outAclStart, outAclEnd;
    uint32_t 	mtu;
    uint32_t 	enableRoute:1,
                valid:1;
} rtl865x_tblAsicDrv_intfParam_t;

typedef struct {
#ifndef _LITTLE_ENDIAN
    /* word 0 */
    uint16_t          mac39_24;
    uint16_t          mac23_8;

    /* word 1 */
    uint32_t          reserv0: 6;
    uint32_t          auth: 1;
    uint32_t          fid:2;
    uint32_t          nxtHostFlag : 1;
    uint32_t          srcBlock    : 1;
    uint32_t          agingTime   : 2;
    uint32_t          isStatic    : 1;
    uint32_t          toCPU       : 1;
    uint32_t          extMemberPort   : 3;
    uint32_t          memberPort : 6;
    uint32_t          mac47_40    : 8;

#else /*LITTLE_ENDIAN*/
    /* word 0 */
    uint16_t          mac23_8;
    uint16_t          mac39_24;

    /* word 1 */
    uint32_t          mac47_40    : 8;
    uint32_t          memberPort : 6;
    uint32_t          extMemberPort   : 3;
    uint32_t          toCPU       : 1;
    uint32_t          isStatic    : 1;
    uint32_t          agingTime   : 2;
    uint32_t          srcBlock    : 1;
    uint32_t          nxtHostFlag : 1;
    uint32_t          fid:2;
    uint32_t          auth:1;
    uint32_t          reserv0:6;

#endif /*LITTLE_ENDIAN*/
    /* word 2 */
    uint32_t          reservw2;
    /* word 3 */
    uint32_t          reservw3;
    /* word 4 */
    uint32_t          reservw4;
    /* word 5 */
    uint32_t          reservw5;
    /* word 6 */
    uint32_t          reservw6;
    /* word 7 */
    uint32_t          reservw7;
} rtl865xc_tblAsic_l2Table_t;



/* VLAN service
*/

#define RTL_STP_DISABLE 0
#define RTL_STP_BLOCK   1
#define RTL_STP_LEARN   2
#define RTL_STP_FORWARD 3


/* VLAN access parameters */
typedef struct {
    uint32_t          memberPort;
    uint32_t          egressUntag;
    uint32_t          fid: 2;
} rtl_vlan_param_t;

typedef struct rtl865x_tblAsicDrv_vlanParam_s {
    uint32_t 	memberPortMask;
    uint32_t 	untagPortMask;
    uint32_t  fid:2;
#ifdef CONFIG_RTL8196D
    uint32_t  vid:12;
#endif
} rtl865x_tblAsicDrv_vlanParam_t;


typedef struct {

    /* word 0 */
    ipaddr_t        srcIPAddr;
    /* word 1 */
    uint32_t          srcPort      : 4;
    uint32_t          destIPAddrLsbs : 28;

    /* word 2*/
    uint32_t          reserv0     : 14;
    uint32_t          ageTime     : 3;
    uint32_t          toCPU       : 1;
    uint32_t          valid       : 1;
    uint32_t          extIPIndex  : 4;
    uint32_t          portList    : 9;

    /* word 3 */
    uint32_t          reservw3;
    /* word 4 */
    uint32_t          reservw4;
    /* word 5 */
    uint32_t          reservw5;
    /* word 6 */
    uint32_t          reservw6;
    /* word 7 */
    uint32_t          reservw7;

} rtl865xc_tblAsic_ipMulticastTable_t;


typedef struct rtl865x_tblAsicDrv_multiCastParam_s {
    ipaddr_t	sip;
    ipaddr_t	dip;
    uint16_t	svid;
    uint16_t	port;
    uint32_t	mbr;
    uint16_t	age;
    uint16_t	cpu;
    uint16_t	extIdx;
} rtl865x_tblAsicDrv_multiCastParam_t;













/* --------------------------------------------------------------------
 * ROUTINE NAME - swCore_vlanCreate
 * --------------------------------------------------------------------
 * FUNCTION: This service creates a vlan.
 * INPUT   :
		param_P: Pointer to the parameters.
 * OUTPUT  : None.
 * RETURN  : Upon successful completion, the function returns 0.
        Otherwise,
    EEXIST: Speicified vlan already exists.
		ENFILE: Destination slot of vlan table is occupied.
 * NOTE    : None.
 * -------------------------------------------------------------------*/
int swCore_vlanCreate(uint32_t vid, rtl_vlan_param_t * param_P);


/* --------------------------------------------------------------------
 * ROUTINE NAME - swCore_vlanDestroy
 * --------------------------------------------------------------------
 * FUNCTION: This service destroys a vlan.
 * INPUT   :
		vid: Vlan ID.
 * OUTPUT  : None.
 * RETURN  : Upon successful completion, the function returns 0.
        Otherwise,
		ENOENT: Specified vlan does not exist.
 * NOTE    : None.
 * -------------------------------------------------------------------*/
int swCore_vlanDestroy(uint32_t vid);


/* --------------------------------------------------------------------
 * ROUTINE NAME - swCore_vlanSetPVid
 * --------------------------------------------------------------------
 * FUNCTION: This service sets port based vlan id.
 * INPUT   :
		portNum: Port number.
		pvid: Vlan ID.
 * OUTPUT  : None.
 * RETURN  : Upon successful completion, the function returns 0.
        Otherwise,
		ENOENT: Specified vlan does not exist.
 * NOTE    : None.
 * -------------------------------------------------------------------*/
int swCore_vlanSetPVid(uint32_t portNum, uint32_t pvid);


/* --------------------------------------------------------------------
 * ROUTINE NAME - swCore_vlanGetPVid
 * --------------------------------------------------------------------
 * FUNCTION: This service gets port based vlan id.
 * INPUT   :
		portNum: Port number.
 * OUTPUT  :
		pvid_P: Pointer to a variable to hold the PVid.
 * RETURN  : Upon successful completion, the function returns 0.
        Otherwise,
		ENOENT: Specified vlan does not exist.
 * NOTE    : None.
 * -------------------------------------------------------------------*/
int swCore_vlanGetPVid(uint32_t portNum, uint32_t *pvid_P);

/* --------------------------------------------------------------------
 * ROUTINE NAME - swCore_vlanSetPortSTPStatus
 * --------------------------------------------------------------------
 * FUNCTION: This service sets the spanning tree status of the
        specified port.
 * INPUT   :
		vid: Vlan ID.
		portNum: Port number.
		STPStatus: Spanning tree status. Valid values are RTL_STP_DISABLE,
		        RTL_STP_BLOCK, RTL_STP_LEARN and RTL_STP_FORWARD.
 * OUTPUT  : None.
 * RETURN  : Upon successful completion, the function returns 0.
        Otherwise,
		ENOENT: Specified vlan does not exist.
 * NOTE    : None.
 * -------------------------------------------------------------------*/
int swCore_vlanSetPortSTPStatus(uint32_t vid, uint32_t portNumber, uint32_t STPStatus);


/* --------------------------------------------------------------------
 * ROUTINE NAME - swCore_vlanSetSTPStatusOfAllPorts
 * --------------------------------------------------------------------
 * FUNCTION: This service sets the spanning tree status.
 * INPUT   :
		vid: Vlan ID.
		STPStatus: Spanning tree status. Valid values are RTL_STP_DISABLE,
		        RTL_STP_BLOCK, RTL_STP_LEARN and RTL_STP_FORWARD.
 * OUTPUT  : None.
 * RETURN  : Upon successful completion, the function returns 0.
        Otherwise,
		ENOENT: Specified vlan does not exist.
 * NOTE    : None.
 * -------------------------------------------------------------------*/
int swCore_vlanSetSTPStatusOfAllPorts(uint32_t vid, uint32_t STPStatus);


/* --------------------------------------------------------------------
 * ROUTINE NAME - swCore_vlanGetPortSTPStatus
 * --------------------------------------------------------------------
 * FUNCTION: This service gets the spanning tree status of the
        specified port.
 * INPUT   :
		vid: Vlan ID.
		portNum: Port number.
 * OUTPUT  :
		STPStatus_P: Pointer to a variable to hold the spanning tree
		        status of the specified port.
 * RETURN  : Upon successful completion, the function returns 0.
        Otherwise,
		ENOENT: Specified vlan does not exist.
 * NOTE    : None.
 * -------------------------------------------------------------------*/
int swCore_vlanGetPortSTPStatus(uint32_t vid, uint32_t portNumber, uint32_t *STPStatus_P);


/* Layer 2 service
*/

/* L2 forwarding table access parameters
*/
typedef struct {
    macaddr_t       mac;
    uint16_t          isStatic    : 1;
    uint16_t          hPriority   : 1;
    uint16_t          toCPU       : 1;
    uint16_t          srcBlock    : 1;
    uint16_t          nxtHostFlag : 1;
    uint16_t          reserv0     : 11;
    uint32_t          memberPort;
    uint32_t          agingTime;
} rtl_l2_param_t;


/* --------------------------------------------------------------------
 * ROUTINE NAME - swCore_staticMacAddrAdd
 * --------------------------------------------------------------------
 * FUNCTION: This service adds the static MAC address.
 * INPUT   :
		param_P: Pointer to the parameters.
 * OUTPUT  : None.
 * RETURN  : Upon successful completion, the function returns 0.
        Otherwise,
		ENFILE: Cannot allocate slot.
 * NOTE    : None.
 * -------------------------------------------------------------------*/
int swCore_staticMacAddrAdd(rtl_l2_param_t * param_P);


/* --------------------------------------------------------------------
 * ROUTINE NAME - swCore_staticMacAddrRemove
 * --------------------------------------------------------------------
 * FUNCTION: This service removes the specified static MAC address.
 * INPUT   :
		param_P: Pointer to the parameters.
 * OUTPUT  : None.
 * RETURN  : Upon successful completion, the function returns 0.
        Otherwise,
		ENOENT: Specified MAC address does not exist.
 * NOTE    : None.
 * -------------------------------------------------------------------*/
int swCore_staticMacAddrRemove(rtl_l2_param_t * param_P);


/* --------------------------------------------------------------------
 * ROUTINE NAME - swCore_layer2TableGetInformation
 * --------------------------------------------------------------------
 * FUNCTION: This service gets information of specified L2 switch table
        entry.
 * INPUT   :
        entryIndex: Index of entry.
 * OUTPUT  :
		param_P: Pointer to an area to hold the parameters.
 * RETURN  : Upon successful completion, the function returns 0.
        Otherwise,
        EEMPTY: Specified entry is empty.
		ENOENT: Specified entry does not exist.
 * NOTE    : None.
 * -------------------------------------------------------------------*/
int swCore_layer2TableGetInformation(uint32_t entryIndex, rtl_l2_param_t * param_P);


/* --------------------------------------------------------------------
 * ROUTINE NAME - swCore_layer2TableGetInformationByMac
 * --------------------------------------------------------------------
 * FUNCTION: This service gets information of specified L2 switch table
        entry.
 * INPUT   : None.
 * OUTPUT  :
		param_P: Pointer to an area to hold the parameters.
 * RETURN  : Upon successful completion, the function returns 0.
        Otherwise,
		ENOENT: Specified entry does not exist.
 * NOTE    : None.
 * -------------------------------------------------------------------*/
int swCore_layer2TableGetInformationByMac(rtl_l2_param_t * param_P);


/* Counter service
*/

typedef struct {
    uint32_t  etherStatsOctets;
    uint32_t  etherStatsDropEvents;
    uint32_t  etherStatsCRCAlignErrors;
    uint32_t  etherStatsFragments;
    uint32_t  etherStatsJabbers;
    uint32_t  ifInUcastPkts;
    uint32_t  etherStatsMulticastPkts;
    uint32_t  etherStatsBroadcastPkts;
    uint32_t  etherStatsUndersizePkts;
    uint32_t  etherStatsPkts64Octets;
    uint32_t  etherStatsPkts65to127Octets;
    uint32_t  etherStatsPkts128to255Octets;
    uint32_t  etherStatsPkts256to511Octets;
    uint32_t  etherStatsPkts512to1023Octets;
    uint32_t  etherStatsPkts1024to1518Octets;
    uint32_t  etherStatsOversizepkts;
    uint32_t  dot3ControlInUnknownOpcodes;
    uint32_t  dot3InPauseFrames;
} rtl_ingress_counter_t;
typedef struct {
    uint32_t  ifOutOctets;
    uint32_t  ifOutUcastPkts;
    uint32_t  ifOutMulticastPkts;
    uint32_t  ifOutBroadcastPkts;
    uint32_t  dot3StatsLateCollisions;
    uint32_t  dot3StatsDeferredTransmissions;
    uint32_t  etherStatsCollisions;
    uint32_t  dot3StatsMultipleCollisionFrames;
    uint32_t  dot3OutPauseFrames;
} rtl_egress_counter_t;

/* --------------------------------------------------------------------
 * ROUTINE NAME - swCore_counterGetMemberPort
 * --------------------------------------------------------------------
 * FUNCTION: This service gets all the member for counting.
 * INPUT   : None.
 * OUTPUT  :
        portList_P: List of member ports.
 * RETURN  : Upon successful completion, the function returns 0.
        Otherwise,
		EINVAL: Invalid argument.
 * NOTE    : None.
 * -------------------------------------------------------------------*/
int swCore_counterGetMemberPort(uint32_t *portList_P);


/* --------------------------------------------------------------------
 * ROUTINE NAME - swCore_counterSetMemberPort
 * --------------------------------------------------------------------
 * FUNCTION: This service gets all the member for counting.
 * INPUT   :
        portList: List of member ports.
 * OUTPUT  : None.
 * RETURN  : Upon successful completion, the function returns 0.
        Otherwise,
		EINVAL: Invalid argument.
 * NOTE    : None.
 * -------------------------------------------------------------------*/
int swCore_counterSetMemberPort(uint32_t portList);


/* --------------------------------------------------------------------
 * ROUTINE NAME - swCore_counterGetIngress
 * --------------------------------------------------------------------
 * FUNCTION: This service gets all the ingress counters.
 * INPUT   : None.
 * OUTPUT  :
        counters_P: Pointer to an area to hold the ingress counters.
 * RETURN  : Upon successful completion, the function returns 0.
        Otherwise,
		EINVAL: Invalid argument.
 * NOTE    : None.
 * -------------------------------------------------------------------*/
int swCore_counterGetIngress(rtl_ingress_counter_t *counters_P);


/* --------------------------------------------------------------------
 * ROUTINE NAME - swCore_counterGetEgress
 * --------------------------------------------------------------------
 * FUNCTION: This service gets all the egress counters.
 * INPUT   : None.
 * OUTPUT  :
        counters_P: Pointer to an area to hold the egress counters.
 * RETURN  : Upon successful completion, the function returns 0.
        Otherwise,
		EINVAL: Invalid argument.
 * NOTE    : None.
 * -------------------------------------------------------------------*/
int swCore_counterGetEgress(rtl_egress_counter_t *counters_P);


/* Port service
*/

#define RTL_PORT_100M_FD        (1 << 0)
#define RTL_PORT_100M_HD        (1 << 1)
#define RTL_PORT_10M_FD         (1 << 2)
#define RTL_PORT_10M_HD         (1 << 3)

typedef struct {
    uint8_t   capableFlowCtrl : 1;
    uint8_t   capable100MFull : 1;
    uint8_t   capable100MHalf : 1;
    uint8_t   capable10MFull  : 1;
    uint8_t   capable10MHalf  : 1;
    uint8_t   reserv0         : 3;
} rtl_auto_nego_ability_t;

typedef struct {
    uint8_t   enAutoNego          : 1;
    uint8_t   enSpeed100M         : 1;
    uint8_t   enFullDuplex        : 1;
    uint8_t   enLoopback          : 1;
    uint8_t   linkEstablished     : 1;
    uint8_t   autoNegoCompleted   : 1;
    uint8_t   remoteFault         : 1;
    uint8_t   reserv0             : 1;
    rtl_auto_nego_ability_t   autoNegoAbility;
    rtl_auto_nego_ability_t   linkPartnerAutoNegoAbility;
    uint32_t  speedDuplex;
} rtl_port_status_t;

typedef struct rtl865x_tblAsicDrv_l2Param_s {
    ether_addr_t	macAddr;
    uint32_t 		memberPortMask; /*extension ports [rtl8651_totalExtPortNum-1:0] are located at bits [RTL8651_PORT_NUMBER+rtl8651_totalExtPortNum-1:RTL8651_PORT_NUMBER]*/
    uint32_t 		ageSec;
    uint32_t	 	cpu:1,
                    srcBlk:1,
                    isStatic:1,
                    nhFlag:1,
                    fid:2,
                    auth:1;

} rtl865x_tblAsicDrv_l2Param_t;



/* --------------------------------------------------------------------
 * ROUTINE NAME - swCore_portSetSpeedDuplex
 * --------------------------------------------------------------------
 * FUNCTION: This service sets speed and duplex mode of specified port.
 * INPUT   :
		portNum: Port number.
		speedDuplex: Speed and duplex mode. Valid values are
		    RTL_PORT_100M_FD, RTL_PORT_100M_HD, RTL_PORT_10M_FD and
		    RTL_PORT_10M_HD.
 * OUTPUT  : None.
 * RETURN  : Upon successful completion, the function returns 0.
        Otherwise,
		EINVAL: Invalid argument.
 * NOTE    : None.
 * -------------------------------------------------------------------*/
int swCore_portSetSpeedDuplex(uint32_t portNum, uint32_t speedDuplex);


/* --------------------------------------------------------------------
 * ROUTINE NAME - swCore_portSetAutoNegociationAbility
 * --------------------------------------------------------------------
 * FUNCTION: This service sets auto negociation pause, speed and duplex
        mode capability of specified port.
 * INPUT   :
		portNum: Port number.
		anAbility_P: Pointer to the data structure which specifies the auto
		    negociation abilities.
 * OUTPUT  : None.
 * RETURN  : Upon successful completion, the function returns 0.
        Otherwise,
		EINVAL: Invalid argument.
 * NOTE    : None.
 * -------------------------------------------------------------------*/
int swCore_portSetAutoNegociationAbility(uint32_t portNum, rtl_auto_nego_ability_t *anAbility_P);


/* --------------------------------------------------------------------
 * ROUTINE NAME - swCore_portEnableAutoNegociation
 * --------------------------------------------------------------------
 * FUNCTION: This service enables auto negociation of specified port.
 * INPUT   :
		portNum: Port number.
 * OUTPUT  : None.
 * RETURN  : Upon successful completion, the function returns 0.
        Otherwise,
		EINVAL: Invalid argument.
 * NOTE    : None.
 * -------------------------------------------------------------------*/
int swCore_portEnableAutoNegociation(uint32_t portNum);


/* --------------------------------------------------------------------
 * ROUTINE NAME - swCore_portDisableAutoNegociation
 * --------------------------------------------------------------------
 * FUNCTION: This service disables auto negociation of specified port.
 * INPUT   :
		portNum: Port number.
 * OUTPUT  : None.
 * RETURN  : Upon successful completion, the function returns 0.
        Otherwise,
		EINVAL: Invalid argument.
 * NOTE    : None.
 * -------------------------------------------------------------------*/
int swCore_portDisableAutoNegociation(uint32_t portNum);


/* --------------------------------------------------------------------
 * ROUTINE NAME - swCore_portRestartAutoNegociation
 * --------------------------------------------------------------------
 * FUNCTION: This service restarts auto negociation of specified port.
 * INPUT   :
		portNum: Port number.
 * OUTPUT  : None.
 * RETURN  : Upon successful completion, the function returns 0.
        Otherwise,
		EINVAL: Invalid argument.
 * NOTE    : None.
 * -------------------------------------------------------------------*/
int swCore_portRestartAutoNegociation(uint32_t portNum);


/* --------------------------------------------------------------------
 * ROUTINE NAME - swCore_portSetLoopback
 * --------------------------------------------------------------------
 * FUNCTION: This service sets specified port to loopback mode.
 * INPUT   :
		portNum: Port number.
 * OUTPUT  : None.
 * RETURN  : Upon successful completion, the function returns 0.
        Otherwise,
		EINVAL: Invalid argument.
 * NOTE    : None.
 * -------------------------------------------------------------------*/
int swCore_portSetLoopback(uint32_t portNum);


/* --------------------------------------------------------------------
 * ROUTINE NAME - swCore_portResetLoopback
 * --------------------------------------------------------------------
 * FUNCTION: This service sets specified port to normal mode.
 * INPUT   :
		portNum: Port number.
 * OUTPUT  : None.
 * RETURN  : Upon successful completion, the function returns 0.
        Otherwise,
		EINVAL: Invalid argument.
 * NOTE    : None.
 * -------------------------------------------------------------------*/
int swCore_portResetLoopback(uint32_t portNum);


/* --------------------------------------------------------------------
 * ROUTINE NAME - swCore_portGetStatus
 * --------------------------------------------------------------------
 * FUNCTION: This service gets port status of specified port.
 * INPUT   :
		portNum: Port number.
 * OUTPUT  :
    portStatus_P: Pointer to an area to hold the port status.
 * RETURN  : Upon successful completion, the function returns 0.
        Otherwise,
		EINVAL: Invalid argument.
 * NOTE    : None.
 * -------------------------------------------------------------------*/
int swCore_portGetStatus(uint32_t portNum, rtl_port_status_t *portStatus_P);



#define swCore_vlanCreate vlanTable_create
#define swCore_vlanDestroy vlanTable_destroy
#define swCore_vlanSetPortSTPStatus vlanTable_setPortStpStatus
#define swCore_vlanSetSTPStatusOfAllPorts vlanTable_setStpStatusOfAllPorts
#define swCore_vlanGetPortSTPStatus vlanTable_getSTPStatus


//cathy
enum external_interface {
    RTL8367B,
    RTL8211E,
    EXT_NONE
};

enum external_phy_id {
    RTL8367B_PHYID = 0,
    RTL8211E_PHYID = 0x7
};

struct external_giga {
    enum external_interface inf;
    unsigned int extPhyId;
    int (*probeFn) (unsigned int phyid);
};

#define ASSERT_CSP(x) if (!(x)) {printf("\nAssertion fail at file %s, function %s, line number %d: (%s).\n", __FILE__, __FUNCTION__, __LINE__, #x); while(1);}
#define ASSERT_ISR(x) if (!(x)) {printf("\nAssertion fail at file %s, function %s, line number %d: (%s).\n", __FILE__, __FUNCTION__, __LINE__, #x);   while(1);}

#ifndef SUCCESS
#define SUCCESS     0
#endif
#ifndef FAILED
#define FAILED -1
#endif

#define DCACHE_LINE_SIZE 	CONFIG_SYS_DCACHE_LINE_SIZE
#define NUM_RX_PKTHDR_DESC 	16
#define MBUF_LEN 			2048

/* return error numbers */
#define EEMPTY 				87
#define ECOLLISION 			88

#endif /* _SWCORE_H */

