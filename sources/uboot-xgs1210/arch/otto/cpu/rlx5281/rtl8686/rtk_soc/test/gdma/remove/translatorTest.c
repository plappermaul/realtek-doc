/*
* Copyright c                  Realtek Semiconductor Corporation, 2005
* All rights reserved.
* 
* Program : Model code for packet translator
* Abstract : 
* Author : Tung-Chieh Lo (yjlou@realtek.com.tw)               
* $Id: translatorTest.c,v 1.9 2006-08-29 13:00:00 chenyl Exp $
*/

#include "rtl_types.h"
#include "asicRegs.h"
#ifdef CONFIG_RTL865XC
#include "rtl865xC_tblAsicDrv.h"
#else
#include "rtl8651_tblAsicDrv.h"
#endif
#include "virtualMac.h"
#include "rtl_glue.h"
#include "rtl_utils.h"
#include "drvTest.h"
#include "icTest.h"
#include "l2Test.h"
#include "icModel.h" /* for FIRST_CPURPDCR? */
#include "pktForm.h"


/*
 *  Since we focus on the translator function, this function only tests the model code part.
 *  The FPGA and real IC DO NOT provide the ability to test translator only.
 *
 *  This case is L2-forward only, the packet content, which is random, is not modified, except L2 CRC.
 */
int32 testTranslatorOnly1(uint32 caseNo)
{
	int len, len2;
	enum PORT_MASK toPort;
	int32 retval;
	int i;
	hsa_param_t hsa =
	{
		nhmac: { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05 }, /* [47:0] ARP MAC (next hop MAC address) */
		trip: 0x11223344, /* [31:0] Translated IP */
		port: 5566,       /* [15:0] Translated PORT/ICMP ID (type= ICMP) /Call ID (type=PPTP) */
		l3csdt: 0x0000,   /* [15:0] The substrate distance to fix the L3 Checksum. It requires to consider the TTL-1 simultaneously. */
		l4csdt: 0x0000,   /* [15:0] The substrate distance to fix the L4 Checksum. It requires to consider the TTL-1 simultaneously*/
		egif: 1,          /*        IF the packet is from internal VLAN. */
		l2tr: 0,          /*        To indicate if L2 (MAC Address) translation is needed. */
		l34tr: 0,         /*        To indicate if L3 (IP) translation and L4 (PORT) translation is needed. (boundled because the L3 translation will influence the L4 checksum insertion). */
		dirtxo: 0,        /*        Direct to CPU or from CPU(for CRC auto-insertion to distinguish L2 switch or from CPU packets) */
		typeo: 0,         /* [2:0] The type categories after Input packet parsing flow. 000: Ethernet, 001: PPTP, 010: IP, 011: ICMP, 100: IGMP, 101: TCP, 110: UDP */
		snapo: 0,         /*       1: LLC exists */
		rxtag: 0,	      /*       Indicate the rx-packet carries tag header */
		dvid: 0x123,      /*       The destination VLAN, for VLAN Tagging. */
		pppoeifo: 0,      /* [1:0] To indicate if PPPoE session stage header is needed to be tagged. 00: intact, 01: tagging, 10: remove, 11: modify */
		pppidx: 0,        /* [2:0] The PPPoE Session ID Index (to a 8-entry register table) for tagging in the translated packet header if needed. */
		leno: 128,        /* [14:0] Packet length (not included L2 CRC) */
		l3csoko: 0,       /*        L3 CRC OK? */
		l4csoko: 0,       /*        L4 CRC OK? */
		frag: 0,          /*        If this packet is fragment packet? */
		lastfrag: 0,      /*        If this packet is the last fragment packet? */
		ipmcastr: 0,      /*        Routed IP Multicast packet */
		svid: 1,          /* [11:0] Source vid */
		fragpkt: 0,       /*        Enable output port to fragmentize this packet */
		ttl_1if: 0x000,   /* [8:0] Per MAC port TTL operation indication */
		dpc: 1,           /* [2:0] Destination ports count */
		spao: 2,          /* [4:0] Packet source Port ( refer HAS.SPA definitation ) */
		dpext: 0,         /* [3:0] Packet destination indication : 0-2: ext port0-2 3: cpu */
		spaext: 0,        /* [1:0] ( refer HAS.EXTSPA definitation ) */
		why2cpu: 0x0000,  /* [15:0] CPU reason */
		spcp: 0,          /* [2:0] Source priority code point */
		dvtag: 0x000,     /* [8:0] Destination VLAN tag set: 0-5: port 0-5, 6-8: extension port 0-2 */
		difid: 0,         /* [2:0] Destination Interface ID ( MAC uses this to get gateway MAC if L2Trans = 1) */
		linkid: 0,        /* [6:0] WLAN link ID; if is valid only if the packets are from extension ports. 0: this field is invalid. */
		siptos: 0,        /* [7:0] Source IPToS for those packets which were delivered to extension ports. */
		dp: 0x000,        /* [6:0] destinatio port mask, formally not included in HSA. */
		priority: 0,      /* [2:0] priority ID (valid: 0~7) */
	};

	/* prepare packet */
	memset( sharedPkt[0], 0, _PKT_LEN );
	memset( sharedPkt[1], 0, _PKT_LEN );
	for( i = 0; i < _PKT_LEN; i++ )
		sharedPkt[0][i] = rtlglue_random();

	/* We try for every port */
	for( i = PN_PORT0; i<=PN_PORT5; i++ )
	{
		rtlglue_printf("[%d]", i );
		/* Set Destination Port */
		hsa.dp = (1<<i);
		
		retval = model_setTestTarget( IC_TYPE_MODEL );
		ASSERT( retval==SUCCESS );

		/* initial ASIC */
		rtl8651_clearAsicAllTable();
		rtl8651_clearRegister();
		NORMAL_MODE;
		rtl8651_setAsicOperationLayer(2);

		/* Recalculate L2 CRC */
		l2FormCrc( 0,  sharedPkt[0], 124 );

		/* Set HSA and run */
		retval = virtualMacSetHsa( &hsa );
		ASSERT( retval==SUCCESS );
		len = 128; /* This length includes L2 CRC */
		retval = modelPktTranslator( sharedPkt[0], len );
		ASSERT( retval==SUCCESS );

		/* get output packet and compare */
		toPort = (1<<i);
		retval = virtualMacOutput( &toPort, sharedPkt[1], &len2 );
		ASSERT( SUCCESS==retval );
		retval = memComp( sharedPkt[0], sharedPkt[1], max(len,len2), "Input and output packet content differs." );
		ASSERT( SUCCESS==retval );
		IS_EQUAL_INT( "Input length and output length is not equal.", len, len2, __FUNCTION__, __LINE__ );
	}

	rtlglue_printf("\n");
	return SUCCESS;
}


/*
 * #if 1 -- for endian free, all big-endian version.
 * #if 0 -- hybrid little/big-endian.
 */
#if 0
    #define RM32(a) READ_MEM32(a)
    #define WM32(a,v) WRITE_MEM32(a,v)
#else
    #define RM32(a) (*(uint32*)a)
    #define WM32(a,v) ((*(uint32*)a)=v)
#endif

/*
 *  This function will search all pkthdr and mbuf descriptor ring to find corresponding pPkt and pPkt->ph_mbuf.
 *  Then, change them to ASIC owned.
 *  If the pRecyclePkt==NULL, it mean recycle all pkthdr and mbuf.
 */
int32 testRecyclePkthdrMbuf( struct rtl_pktHdr *pRecyclePkt )
{
	uint32 uuPkt;
	uint32 uuMbuf;
	uint32 uPkt;
	uint32 uMbuf;
	struct rtl_pktHdr *pPkt;
	struct rtl_mBuf *pMbuf;
	struct rtl_mBuf *pRecycleMbuf;
	enum RXPKTDESC desc;

	for( desc = RxPktDesc0; desc <= RxPktDesc5; desc++ )
	{
		switch( desc )
		{
			case RxPktDesc0:
				uuPkt = FIRST_CPURPDCR0;
				break;
			case RxPktDesc1:
				uuPkt = FIRST_CPURPDCR1;
				break;
			case RxPktDesc2:
				uuPkt = FIRST_CPURPDCR2;
				break;
			case RxPktDesc3:
				uuPkt = FIRST_CPURPDCR3;
				break;
			case RxPktDesc4:
				uuPkt = FIRST_CPURPDCR4;
				break;
			case RxPktDesc5:
			default:
				uuPkt = FIRST_CPURPDCR5;
				break;
		}

		if ( uuPkt!=0 ) /* search only when pktDesc ring is set. */
		{
			while( 1 )
			{
				uPkt = RM32( uuPkt );
				pPkt = (struct rtl_pktHdr*)(uPkt&~(DESC_WRAP|DESC_OWNED_BIT));

				if ( pRecyclePkt==NULL ||
				     pPkt==pRecyclePkt )
				{
					/* Yes, we found the corresponding pktHdr. */
					WM32( uuPkt, uPkt|DESC_SWCORE_OWNED );

					pRecycleMbuf = pPkt->ph_mbuf;
					uuMbuf = FIRST_CPURMDCR0;
					while( pRecycleMbuf!=NULL )
					{
						while( 1 )
						{
							uMbuf = RM32( uuMbuf );
							pMbuf = (struct rtl_mBuf*)(uMbuf&~(DESC_WRAP|DESC_OWNED_BIT));

							if ( pMbuf==pRecycleMbuf )
							{
								/* Yes, we found the corresponding mBuf. */
								WM32( uuMbuf, uMbuf|DESC_SWCORE_OWNED );
								break; /* break to find m_next */
							}

							if ( uMbuf&DESC_WRAP ) break;
							uuMbuf += 4; /* move to next descriptor */
						}

						pRecycleMbuf = pRecycleMbuf->m_next;
					}
				}

				if ( uPkt&DESC_WRAP ) break;
				uuPkt += 4; /* move to next descriptor */
			}
		}
	}

	return SUCCESS;
}


#define PKTHDR_NUM 3
#define MBUf_NUM 8
/*
 *  This function focus on the modelChainPMC().
 */
int32 testModelChainPMC(uint32 caseNo)
{
	int32 retval;
	struct rtl_pktHdr pkthdr[PKTHDR_NUM];
	struct rtl_mBuf mbuf[MBUf_NUM];
	uint32 descPkthdr[PKTHDR_NUM];
	uint32 descMbuf[MBUf_NUM];
	struct rtl_pktHdr *pPkt;
	int i;
	
	retval = model_setTestTarget( IC_TYPE_MODEL );
	ASSERT( retval==SUCCESS );

	/* backup NIC descriptor registers */
	modelBackupDescRegisters();

	/* initial ASIC */
	rtl8651_clearAsicAllTable();
	rtl8651_clearRegister();
	NORMAL_MODE;
	rtl8651_setAsicOperationLayer(2);
		
	memset( pkthdr, 0, sizeof(pkthdr) );
	memset( mbuf, 0, sizeof(mbuf) );

	/* Initialize pkthdr and mbuf ring */
	for( i=0; i < PKTHDR_NUM; i++ )
		WM32( (uint32)&descPkthdr[i], ((uint32)&pkthdr[i])|DESC_SWCORE_OWNED );
	WM32( (uint32)&descPkthdr[PKTHDR_NUM-1], RM32( (uint32)&descPkthdr[PKTHDR_NUM-1] ) | DESC_WRAP );
	for( i=0; i < MBUf_NUM; i++ )
	{
		WM32( (uint32)&descMbuf[i], ((uint32)&mbuf[i])|DESC_SWCORE_OWNED );
		mbuf[i].m_extsize = 2048;
		mbuf[i].m_extbuf = NULL;
		mbuf[i].m_data = NULL+128;
	}
	WM32( (uint32)&descMbuf[MBUf_NUM-1], RM32( (uint32)&descMbuf[MBUf_NUM-1] ) | DESC_WRAP );
	WRITE_MEM32( CPURPDCR0, (uint32)&descPkthdr[0] );
	WRITE_MEM32( CPURMDCR0, (uint32)&descMbuf[0] );
#if 0
	memDump( descPkthdr, sizeof(descPkthdr), "descPkthdr" );
	memDump( descMbuf, sizeof(descMbuf), "descMbuf" );
#endif
	/* 2048-128: expect get one mbuf */
	retval = modelChainPMC( RxPktDesc0, 2048-128, &pPkt );
	ASSERT( retval == SUCCESS );
	ASSERT( pPkt!=NULL );
	ASSERT( pPkt->ph_mbuf!=NULL );
	ASSERT( pPkt->ph_mbuf->m_next==NULL );
	testRecyclePkthdrMbuf(pPkt);

	/* 2048-128+1: expect get two mbufs */
	retval = modelChainPMC( RxPktDesc0, 2048-128+1, &pPkt );
	ASSERT( retval == SUCCESS );
	ASSERT( pPkt!=NULL );
	ASSERT( pPkt->ph_mbuf!=NULL );
	ASSERT( pPkt->ph_mbuf->m_next!=NULL );
	ASSERT( pPkt->ph_mbuf->m_next->m_next==NULL );
	testRecyclePkthdrMbuf(pPkt);

	/* 1920+2048: expect get two mbufs */
	retval = modelChainPMC( RxPktDesc0, 1920+2048, &pPkt );
	ASSERT( retval == SUCCESS );
	ASSERT( pPkt!=NULL );
	ASSERT( pPkt->ph_mbuf!=NULL );
	ASSERT( pPkt->ph_mbuf->m_next!=NULL );
	ASSERT( pPkt->ph_mbuf->m_next->m_next==NULL );
	testRecyclePkthdrMbuf(pPkt);

	/* 1920+2048+1: expect get three mbufs */
	retval = modelChainPMC( RxPktDesc0, 1920+2048+1, &pPkt );
	ASSERT( retval == SUCCESS );
	ASSERT( pPkt!=NULL );
	ASSERT( pPkt->ph_mbuf!=NULL );
	ASSERT( pPkt->ph_mbuf->m_next!=NULL );
	ASSERT( pPkt->ph_mbuf->m_next->m_next!=NULL );
	ASSERT( pPkt->ph_mbuf->m_next->m_next->m_next==NULL );
	testRecyclePkthdrMbuf(pPkt);

	/* 1920+2048+2048: expect get three mbufs */
	retval = modelChainPMC( RxPktDesc0, 1920+2048+2048, &pPkt );
	ASSERT( retval == SUCCESS );
	ASSERT( pPkt!=NULL );
	ASSERT( pPkt->ph_mbuf!=NULL );
	ASSERT( pPkt->ph_mbuf->m_next!=NULL );
	ASSERT( pPkt->ph_mbuf->m_next->m_next!=NULL );
	ASSERT( pPkt->ph_mbuf->m_next->m_next->m_next==NULL );
	testRecyclePkthdrMbuf(pPkt);

	/* 1920+2048+2048+1: expect get four mbufs */
	retval = modelChainPMC( RxPktDesc0, 1920+2048+2048+1, &pPkt );
	ASSERT( retval == SUCCESS );
	ASSERT( pPkt!=NULL );
	ASSERT( pPkt->ph_mbuf!=NULL );
	ASSERT( pPkt->ph_mbuf->m_next!=NULL );
	ASSERT( pPkt->ph_mbuf->m_next->m_next!=NULL );
	ASSERT( pPkt->ph_mbuf->m_next->m_next->m_next!=NULL );
	ASSERT( pPkt->ph_mbuf->m_next->m_next->m_next->m_next==NULL );
	testRecyclePkthdrMbuf(pPkt);

	/* restore NIC descriptor registers */
	modelRestoreDescRegisters();
	
	return SUCCESS;
}


/*
 *  Since we focus on the translator function, this function only tests the model code part.
 *  The FPGA and real IC DO NOT provide the ability to test translator only.
 *
 *  This case is to test NIC RX.
 */
int32 testTranslatorOnlyNIC(uint32 caseNo)
{
	int len;
	int32 retval;
	int i;
	hsa_param_t hsa =
	{
		nhmac: { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05 }, /* [47:0] ARP MAC (next hop MAC address) */
		trip: 0x11223344, /* [31:0] Translated IP */
		port: 5566,       /* [15:0] Translated PORT/ICMP ID (type= ICMP) /Call ID (type=PPTP) */
		l3csdt: 0x0000,   /* [15:0] The substrate distance to fix the L3 Checksum. It requires to consider the TTL-1 simultaneously. */
		l4csdt: 0x0000,   /* [15:0] The substrate distance to fix the L4 Checksum. It requires to consider the TTL-1 simultaneously*/
		egif: 1,          /*        IF the packet is from internal VLAN. */
		l2tr: 0,          /*        To indicate if L2 (MAC Address) translation is needed. */
		l34tr: 0,         /*        To indicate if L3 (IP) translation and L4 (PORT) translation is needed. (boundled because the L3 translation will influence the L4 checksum insertion). */
		dirtxo: 0,        /*        Direct to CPU or from CPU(for CRC auto-insertion to distinguish L2 switch or from CPU packets) */
		typeo: 0,         /* [2:0] The type categories after Input packet parsing flow. 000: Ethernet, 001: PPTP, 010: IP, 011: ICMP, 100: IGMP, 101: TCP, 110: UDP */
		snapo: 0,         /*       1: LLC exists */
		rxtag: 0,	      /*       Indicate the rx-packet carries tag header */
		dvid: 0x123,      /*       The destination VLAN, for VLAN Tagging. */
		pppoeifo: 0,      /* [1:0] To indicate if PPPoE session stage header is needed to be tagged. 00: intact, 01: tagging, 10: remove, 11: modify */
		pppidx: 0,        /* [2:0] The PPPoE Session ID Index (to a 8-entry register table) for tagging in the translated packet header if needed. */
		leno: 124,        /* [14:0] Packet length (not included L2 CRC) */
		l3csoko: 0,       /*        L3 CRC OK? */
		l4csoko: 0,       /*        L4 CRC OK? */
		frag: 0,          /*        If this packet is fragment packet? */
		lastfrag: 0,      /*        If this packet is the last fragment packet? */
		ipmcastr: 0,      /*        Routed IP Multicast packet */
		svid: 1,          /* [11:0] Source vid */
		fragpkt: 0,       /*        Enable output port to fragmentize this packet */
		ttl_1if: 0x000,   /* [8:0] Per MAC port TTL operation indication */
		dpc: 1,           /* [2:0] Destination ports count */
		spao: 2,          /* [4:0] Packet source Port ( refer HAS.SPA definitation ) */
		dpext: 0,         /* [3:0] Packet destination indication : 0-2: ext port0-2 3: cpu */
		spaext: 0,        /* [1:0] ( refer HAS.EXTSPA definitation ) */
		why2cpu: 0x0000,  /* [15:0] CPU reason */
		spcp: 0,          /* [2:0] Source priority code point */
		dvtag: 0x000,     /* [8:0] Destination VLAN tag set: 0-5: port 0-5, 6-8: extension port 0-2 */
		difid: 0,         /* [2:0] Destination Interface ID ( MAC uses this to get gateway MAC if L2Trans = 1) */
		linkid: 0,        /* [6:0] WLAN link ID; if is valid only if the packets are from extension ports. 0: this field is invalid. */
		siptos: 0,        /* [7:0] Source IPToS for those packets which were delivered to extension ports. */
		dp: 0x000,        /* [6:0] destinatio port mask, formally not included in HSA. */
		priority: 0,      /* [2:0] priority ID (valid: 0~7) */
	};

	/* prepare packet */
	memset( sharedPkt[0], 0, _PKT_LEN );
	memset( sharedPkt[1], 0, _PKT_LEN );
	for( i = 0; i < _PKT_LEN; i++ )
		sharedPkt[0][i] = rtlglue_random();

	{ /* TEST to CPU only */
		hsa.dp = (1<<PN_PORT_NOTPHY);
		hsa.dpext = (1<<3/*_CPU*/);
		
		retval = model_setTestTarget( IC_TYPE_MODEL );
		ASSERT( retval==SUCCESS );

		/* initial ASIC */
		rtl8651_clearAsicAllTable();
		rtl8651_clearRegister();
		NORMAL_MODE;
		rtl8651_setAsicOperationLayer(2);
		
		/* Set HSA and run */
		retval = virtualMacSetHsa( &hsa );
		ASSERT( retval==SUCCESS );
		len = 128; /* This length includes L2 CRC */
		retval = modelPktTranslator( sharedPkt[0], len-4 );
		ASSERT( retval==SUCCESS );
	}
	
	rtlglue_printf("\n");
	return SUCCESS;
}


