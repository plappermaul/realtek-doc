/*
* Copyright c                  Realtek Semiconductor Corporation, 2005
* All rights reserved.
* 
* Program : Source File for Layer4 Model Code
* Abstract : 
* Author : Jiann-Haur Huang (ghhuang@realtek.com.tw)               
* $Id: naptModel.c,v 1.12 2006-09-11 05:52:26 evinlien Exp $
*/

/*
 * Temporary Comments:
 *
 * - Used HSB fields
 *   o DIP
 *   o SIP
 *   o DPort L4 Checksum
 *   o SPort / ICMP ID
 *   o L3, L4 CRC OK
 *
 * - Used HSA fields
 *   o IP
 *   o Port / ICMP ID
 *   o L4 Checksum
 *   o L3 L4 TransIF
 *   o Direct Tx
 *   o Type
 *   o L4 CRC OK
 *   o CPU Reason
 *
 */

#include <rtl_glue.h>
#include "assert.h"
#include "hsModel.h"
#include "asicRegs.h"
#include "icModel.h"
#include "modelTrace.h"
#ifdef CONFIG_RTL865XC
#include "rtl865xC_tblAsicDrv.h"
#else
#include "rtl8651_tblAsicDrv.h"
#endif
#include "naptModel.h"
#include "l2Model.h"
#include "rtl865xc_asicregs.h"
#include "drvTest.h"



#define BIT(val, bit_shift)  (((val) >> (bit_shift)) & 0x1)
enum _RTL8651_HOSTTYPE {
	_RTL8651_HOST_NPI = 0,
	_RTL8651_HOST_NI,
	_RTL8651_HOST_LP,
	_RTL8651_HOST_RP,
	_RTL8651_HOST_NPE,
	_RTL8651_HOST_NE,
	_RTL8651_HOST_MAX,
	_RTL8651_HOST_NONE,
};

#define IP_IDX_WIDTH 5

static int32 modelLayer4Hash1( hsb_param_t* hsb )
{
	uint64 st = 0, dt = 0;
	uint16 a = 0, b = 0;
	uint32 hash_value = 0;

	/* From alpha's mail (2006/05/02) 
	  hash 1
	  idx[96:0] = { dip, dport, tcp_pkt, sip, sport } ;  
	  hash1id   = idx[9:0] ^ idx[19:10] ^ idx[29:20] ^
	              idx[39:30] ^ idx[49:40] ^ idx[59:50] ^ 
	              idx[69:60] ^ idx[79:70] ^ idx[89:80] ^
	              { 3'd0,  idx[96:90] } ; 
	*/

	/* Compute ST = idx[49:0] */
	st |= ((uint64)hsb->sprt) << 0;
	st |= ((uint64)hsb->sip) << 16;
	if ( hsb->type==HSB_TYPE_TCP/*TCP*/ )
		st |= ((uint64)1<<48);
	st |= ((uint64)hsb->dprt&1) << 49;

	/* Compute DT = idx[96:50] */
	if ( hsb->ipptl!=1/*ICMP*/ ) /* is TCP or UDP */
		dt |= ((uint64)(hsb->dprt)) >> 1;
	dt |= ((uint64)hsb->dip ) << 15;

	/* Compute A */
	a |= ( BIT(st,49) ^ BIT(st,39) ^ BIT(st,29) ^ BIT(st,19) ^ BIT(st,9) ) << 9;
	a |= ( BIT(st,48) ^ BIT(st,38) ^ BIT(st,28) ^ BIT(st,18) ^ BIT(st,8) ) << 8;
	a |= ( BIT(st,47) ^ BIT(st,37) ^ BIT(st,27) ^ BIT(st,17) ^ BIT(st,7) ) << 7;
	a |= ( BIT(st,46) ^ BIT(st,36) ^ BIT(st,26) ^ BIT(st,16) ^ BIT(st,6) ) << 6;
	a |= ( BIT(st,45) ^ BIT(st,35) ^ BIT(st,25) ^ BIT(st,15) ^ BIT(st,5) ) << 5;
	a |= ( BIT(st,44) ^ BIT(st,34) ^ BIT(st,24) ^ BIT(st,14) ^ BIT(st,4) ) << 4;
	a |= ( BIT(st,43) ^ BIT(st,33) ^ BIT(st,23) ^ BIT(st,13) ^ BIT(st,3) ) << 3;
	a |= ( BIT(st,42) ^ BIT(st,32) ^ BIT(st,22) ^ BIT(st,12) ^ BIT(st,2) ) << 2;
	a |= ( BIT(st,41) ^ BIT(st,31) ^ BIT(st,21) ^ BIT(st,11) ^ BIT(st,1) ) << 1;
	a |= ( BIT(st,40) ^ BIT(st,30) ^ BIT(st,20) ^ BIT(st,10) ^ BIT(st,0) ) << 0;

	/* Compute B */
	b |= ( BIT(dt,49) ^ BIT(dt,39) ^ BIT(dt,29) ^ BIT(dt,19) ^ BIT(dt,9) ) << 9;
	b |= ( BIT(dt,48) ^ BIT(dt,38) ^ BIT(dt,28) ^ BIT(dt,18) ^ BIT(dt,8) ) << 8;
	b |= ( BIT(dt,47) ^ BIT(dt,37) ^ BIT(dt,27) ^ BIT(dt,17) ^ BIT(dt,7) ) << 7;
	b |= ( BIT(dt,46) ^ BIT(dt,36) ^ BIT(dt,26) ^ BIT(dt,16) ^ BIT(dt,6) ) << 6;
	b |= ( BIT(dt,45) ^ BIT(dt,35) ^ BIT(dt,25) ^ BIT(dt,15) ^ BIT(dt,5) ) << 5;
	b |= ( BIT(dt,44) ^ BIT(dt,34) ^ BIT(dt,24) ^ BIT(dt,14) ^ BIT(dt,4) ) << 4;
	b |= ( BIT(dt,43) ^ BIT(dt,33) ^ BIT(dt,23) ^ BIT(dt,13) ^ BIT(dt,3) ) << 3;
	b |= ( BIT(dt,42) ^ BIT(dt,32) ^ BIT(dt,22) ^ BIT(dt,12) ^ BIT(dt,2) ) << 2;
	b |= ( BIT(dt,41) ^ BIT(dt,31) ^ BIT(dt,21) ^ BIT(dt,11) ^ BIT(dt,1) ) << 1;
	b |= ( BIT(dt,40) ^ BIT(dt,30) ^ BIT(dt,20) ^ BIT(dt,10) ^ BIT(dt,0) ) << 0;

	/* Compute Hash Value */
	hash_value |= ( BIT(a,9) ^ BIT(b,9) ) << 9;
	hash_value |= ( BIT(a,8) ^ BIT(b,8) ) << 8;
	hash_value |= ( BIT(a,7) ^ BIT(b,7) ) << 7;
	hash_value |= ( BIT(a,6) ^ BIT(b,6) ) << 6;
	hash_value |= ( BIT(a,5) ^ BIT(b,5) ) << 5;
	hash_value |= ( BIT(a,4) ^ BIT(b,4) ) << 4;
	hash_value |= ( BIT(a,3) ^ BIT(b,3) ) << 3;
	hash_value |= ( BIT(a,2) ^ BIT(b,2) ) << 2;
	hash_value |= ( BIT(a,1) ^ BIT(b,1) ) << 1;
	hash_value |= ( BIT(a,0) ^ BIT(b,0) ) << 0;

	return hash_value;
}


static int32 modelLayer4Hash2( hsb_param_t* hsb )
{
	uint64 st = 0;
	uint32 hash_value = 0;

	/* From alpha's mail (2006/05/02)
	hash 2
	hash2id = sip[9:0] ^ sip[19:10] ^ sip[29:20] ^
	          sport[9:0] ^
	          { 1'd0, tcp_pkt, sip[31:30], sport[15:10] } ;
	*/ 
	
	/* Compute ST */
	st |= ((uint64)hsb->sip&0x3fffffff) << 0;
	st |= ((uint64)hsb->sprt) << 30;
	st |= ((uint64)hsb->sip>>30) << 46;
	if (hsb->type==HSB_TYPE_TCP/*TCP*/)
		st |= ((uint64)1) << 48;

	/* Compute Hash Value */
	hash_value |= ( BIT(st,49) ^ BIT(st,39) ^ BIT(st,29) ^ BIT(st, 19) ^ BIT(st, 9) ) << 9;
	hash_value |= ( BIT(st,48) ^ BIT(st,38) ^ BIT(st,28) ^ BIT(st, 18) ^ BIT(st, 8) ) << 8;
	hash_value |= ( BIT(st,47) ^ BIT(st,37) ^ BIT(st,27) ^ BIT(st, 17) ^ BIT(st, 7) ) << 7;
	hash_value |= ( BIT(st,46) ^ BIT(st,36) ^ BIT(st,26) ^ BIT(st, 16) ^ BIT(st, 6) ) << 6;
	hash_value |= ( BIT(st,45) ^ BIT(st,35) ^ BIT(st,25) ^ BIT(st, 15) ^ BIT(st, 5) ) << 5;
	hash_value |= ( BIT(st,44) ^ BIT(st,34) ^ BIT(st,24) ^ BIT(st, 14) ^ BIT(st, 4) ) << 4;
	hash_value |= ( BIT(st,43) ^ BIT(st,33) ^ BIT(st,23) ^ BIT(st, 13) ^ BIT(st, 3) ) << 3;
	hash_value |= ( BIT(st,42) ^ BIT(st,32) ^ BIT(st,22) ^ BIT(st, 12) ^ BIT(st, 2) ) << 2;
	hash_value |= ( BIT(st,41) ^ BIT(st,31) ^ BIT(st,21) ^ BIT(st, 11) ^ BIT(st, 1) ) << 1;
	hash_value |= ( BIT(st,40) ^ BIT(st,30) ^ BIT(st,20) ^ BIT(st, 10) ^ BIT(st, 0) ) << 0;

	return hash_value;
}

/*return the DDDDD or SSSSS fields in the ph_reason*/
int16 cvtHostType2PHReason(ale_data_t *ale, hsb_param_t* hsb)
{
	rtl865xc_tblAsic_l3RouteTable_t l3_entry;
	uint32 nextHopTableIndex;
	rtl8651_tblAsic_nextHopTable_t nexthop_entry;
	int16 rtVal = -1;
	uint16 i;
	static rtl8651_tblAsic_extIpTable_t asic_ipt[RTL8651_IPTABLE_SIZE];

	switch(ale->dstType_p)
	{
		case _RTL8651_HOST_NPI :
			rtVal = 16;	/*b_10000*/
			break;
		case _RTL8651_HOST_RP :
			rtVal = 19; 	/*b_10011*/
			break;
		case _RTL8651_HOST_LP:
			for (i=0; i < RTL8651_IPTABLE_SIZE;i++)		
				_rtl8651_readAsicEntry(TYPE_EXT_INT_IP_TABLE, i, &asic_ipt[i]);
			
			for (i =0;i< RTL8651_IPTABLE_SIZE;i++)			
			{
				if (asic_ipt[i].valid==0)
					continue;		
				if ( (asic_ipt[i].externalIP==hsb->dip) &&  (asic_ipt[i].isLocalPublic==1 ))
				{
					MT_WATCH("Match Local Public  Ip Addr ");	
					rtVal=i;
					break;
				}
			}
			break;
		case _RTL8651_HOST_NE:
			for (i=0; i < RTL8651_IPTABLE_SIZE;i++)		
				_rtl8651_readAsicEntry(TYPE_EXT_INT_IP_TABLE, i, &asic_ipt[i]);
			
			for (i =0;i< RTL8651_IPTABLE_SIZE;i++)			
			{
				if (asic_ipt[i].valid==0)
					continue;		
				if ( (asic_ipt[i].externalIP==hsb->dip) &&  (asic_ipt[i].isLocalPublic==0 )&&(asic_ipt[i].isOne2One==1))
				{
					MT_WATCH("Match NAT External  Ip Addr ");	
					rtVal=i;
					break;
				}
			}
			break;
		case _RTL8651_HOST_NPE:
			_rtl8651_readAsicEntry(TYPE_L3_ROUTING_TABLE, ale->rouitingDIPIdx, &l3_entry);
			if (l3_entry.linkTo.NxtHopEntry.process == 5)
			{
				nextHopTableIndex = l3_entry.linkTo.NxtHopEntry.nhStart;
				_rtl8651_readAsicEntry(TYPE_NEXT_HOP_TABLE, nextHopTableIndex, &nexthop_entry);
				rtVal = nexthop_entry.IPIndex;
			}
			else if (l3_entry.linkTo.NxtHopEntry.process == 2)
				rtVal = READ_VIR32(GIDXMCR) & (1 << ((hsb->sip & 0x7) * 3)) >> ((hsb->sip & 0x7) * 3);
			break;
			
		default:
			ASSERT(0);
	}

	rtVal <<= IP_IDX_WIDTH;

	
	switch(ale->srcType_p)
	{
		case _RTL8651_HOST_NPI :
			rtVal |= 16;	/*b_10000*/
			break;
		case _RTL8651_HOST_RP :
			rtVal |= 19; 	/*b_10011*/
			break;
		case _RTL8651_HOST_NI:
			for (i=0; i < RTL8651_IPTABLE_SIZE;i++)		
				_rtl8651_readAsicEntry(TYPE_EXT_INT_IP_TABLE, i, &asic_ipt[i]);
			
			for (i =0;i< RTL8651_IPTABLE_SIZE;i++)			
			{
				if (asic_ipt[i].valid==0)
					continue;		
				if ( (asic_ipt[i].externalIP==hsb->sip) &&  (asic_ipt[i].isLocalPublic==0 )&&(asic_ipt[i].isOne2One==1))
				{
					MT_WATCH("Match NAT Internal  Ip Addr ");	
					rtVal=i;
					break;
				}
			}
			break;
		case _RTL8651_HOST_LP:	
			for (i=0; i < RTL8651_IPTABLE_SIZE;i++)		
				_rtl8651_readAsicEntry(TYPE_EXT_INT_IP_TABLE, i, &asic_ipt[i]);
			
			for (i =0;i< RTL8651_IPTABLE_SIZE;i++)			
			{
				if (asic_ipt[i].valid==0)
					continue;		
				if ( (asic_ipt[i].externalIP==hsb->sip) &&  (asic_ipt[i].isLocalPublic==1 ))
				{
					MT_WATCH("Match Local Public  Ip Addr ");	
					rtVal=i;
					break;
				}
			}
			break;

		default:
			ASSERT(0);
	}

	return rtVal <<= 5;		/* Reserved for 5-bit reason */
}

/*
@func enum MODEL_RETURN_VALUE | modelLayer4NaptProcessing | model code for L2 switching
@parm hsb_param_t* | hsb | Header Stamp of Before (for input)
@parm hsa_param_t* | hsa | Header Stamp of After (for output)
@rvalue enum MODEL_RETURN_VALUE |
@comm 
*/
enum MODEL_RETURN_VALUE modelLayer4NaptLanWanProcessing( hsb_param_t* hsb, hsa_param_t* hsa, ale_data_t *ale)
{
	uint32 hash1, hash2;
	uint32 i;
	uint8 routingIndex;
	uint32 matched = 0;
	uint32 naptTableIndex;
	rtl865xc_tblAsic_naptTcpUdpTable_t napt_entry;
	rtl865xc_tblAsic_l3RouteTable_t l3_entry;
	int32 naptPriority;
	uint8 msg[128];
	
	routingIndex=ale->rouitingDIPIdx;
	hash1 = modelLayer4Hash1(hsb);	
	hash2 = modelLayer4Hash2(hsb);
	

	/* First Do Hash1 Matching */
	if (READ_VIR32(SWTECR) &(EN_TCPUDP_4WAY_HASH))
	{
		/* 4-Way Set-Associative */
		for (i = 0; i < 4; i++)
		{
			uint32 vd;
			naptTableIndex = ((hash1 >> 2) << 2) + i;

			_rtl8651_readAsicEntry(TYPE_L4_TCP_UDP_TABLE, naptTableIndex, &napt_entry);
			vd = (napt_entry.valid << 1) + napt_entry.dedicate;
			naptPriority = napt_entry.priValid? (napt_entry.priority) : -1;
			modelSetPriroity(PRI_NAPT, naptPriority, ale);

			if (vd == 0x2)
			{
				/* NAPT Hash1 */
				matched = (napt_entry.intIPAddr == hsb->sip) && (napt_entry.intPort == hsb->sprt);

				/*Enhanced Hash1*/
				if (READ_VIR32(SWTCR1) &(L4EnHash1))
					matched &= (napt_entry.agingTime > 0) && (napt_entry.TCPFlag == 1/*outbound*/) ;
				
				if (napt_entry.isTCP)
					matched &= (hsb->type == HSB_TYPE_TCP);
				else
					matched &= (hsb->type == HSB_TYPE_UDP);

				if (matched)
					break;
			}
		}
	}
	else
	{
		/* 1-Way Set-Associative */
		uint32 vd;
		naptTableIndex = hash1;

		_rtl8651_readAsicEntry(TYPE_L4_TCP_UDP_TABLE, naptTableIndex, &napt_entry);
		vd = (napt_entry.valid << 1) + napt_entry.dedicate;
		naptPriority = napt_entry.priValid ? (napt_entry.priority) : -1;
		modelSetPriroity(PRI_NAPT, naptPriority, ale);

/*
		rtlglue_printf("NAPT Index: %d\n", naptTableIndex);

		rtlglue_printf("VD: %d\n",vd);
		rtlglue_printf("SIP: %u\n", napt_entry.intIPAddr);
		rtlglue_printf("SPORT: %u\n", napt_entry.intPort);

		rtlglue_printf("SIP: %u\n", hsb->sip);
		rtlglue_printf("SPORT: %u\n", hsb->sprt);

		rtlglue_printf("Matched: %d\n", matched);
*/

		if (vd == 0x2)
		{
			/* NAPT Hash1 */
			matched = (napt_entry.intIPAddr == hsb->sip) && (napt_entry.intPort == hsb->sprt);

			/*Enhanced Hash1*/
			if (READ_VIR32(SWTCR1) &(L4EnHash1))
				matched &= (napt_entry.agingTime > 0) && ((napt_entry.TCPFlag&1) == 1/*outbound*/) ;
			
			if (napt_entry.isTCP)
				matched &= (hsb->type == HSB_TYPE_TCP);
			else
				matched &= (hsb->type == HSB_TYPE_UDP);
		}
	}

	/* Hash1 Matched */
	if (matched)
	{
		/*Enhanced Hash1*/
		if ((READ_VIR32(SWTCR1) &(L4EnHash1)) && (((hsb->tcpfg & 1) || (hsb->tcpfg & 2) || (hsb->tcpfg & 4)) & ((napt_entry.TCPFlag & 4) == 0)))
		{
			/* SYN, FIN, RST & Quiet Bit Set => Trap to CPU */
			MT_TOCPU("Hash1 Matched, FIN, RST Packets => To CPU")
			return MRET_TOCPU;
		}

		/*Traditional Hash1*/
		if ((hsb->tcpfg == 1) || (hsb->tcpfg == 4))
		{
			/* FIN, RST => Trap to CPU */
			MT_TOCPU("Hash1 Matched, FIN, RST Packets => To CPU")
			return MRET_TOCPU;
		}
		else
		{
			/* Lookup External IP & External Port */
			uint32 extip, extport;
			uint32 agingTime;
			rtl8651_tblAsic_extIpTable_t extip_entry;

			_rtl8651_readAsicEntry(TYPE_L3_ROUTING_TABLE, routingIndex, &l3_entry);

			agingTime = READ_VIR32( TEATCR );
			if (READ_VIR32(SWTCR1) &(L4EnHash1))		/*Enhanced Hash1*/
			{
				extport = (napt_entry.offset << 10) + napt_entry.selEIdx;
				agingTime = napt_entry.isTCP ? (agingTime&TCPLT_MASK)>>TCPLT_OFFSET : (agingTime&UDPT_MASK)>>UDPT_OFFSET;
				sprintf( msg, "entry.age updated to %d", napt_entry.agingTime );
				MT_WATCH( msg );
			}
			else										/*Traditional Hash1*/
			{
				extport = (napt_entry.offset << 10) + naptTableIndex;
				if ( napt_entry.isTCP==TRUE )
				{
					switch ( napt_entry.TCPFlag )
					{
						case 1: /* Inbound SYN received */
						case 2: /* Outbound SYN received */
							agingTime = (agingTime&TCPST_MASK)>>TCPST_OFFSET;
							break;
						case 4: /* Both SYN received */
							agingTime = (agingTime&TCPLT_MASK)>>TCPLT_OFFSET;
							break;
						case 5: /* Inbound FIN received */
						case 6: /* Outbound FIN received */
							agingTime = (agingTime&TCPMT_MASK)>>TCPMT_OFFSET;
							break;
						case 7: /* Both FIN received */
							agingTime = (agingTime&TCPST_MASK)>>TCPST_OFFSET;
							break;
						default:
							agingTime = 0;
							assert( 0 ); /* invalid napt_entry.TCPFlag configured */
					}
				}
				else
				{
					agingTime = (agingTime&UDPT_MASK)>>UDPT_OFFSET;
				}
				sprintf( msg, "entry.age updated to %d", napt_entry.agingTime );
				MT_WATCH( msg );
			}
			napt_entry.agingTime = agingTime;
			_rtl8651_forceAddAsicEntry(TYPE_L4_TCP_UDP_TABLE, naptTableIndex, &napt_entry);

			
			if (l3_entry.linkTo.NxtHopEntry.process == 5)
			{
				/* Next Hop Process */
				uint32 nextHopTableIndex;
				rtl8651_tblAsic_nextHopTable_t nexthop_entry;
				uint32 ipTableIndex;		

				nextHopTableIndex = l3_entry.linkTo.NxtHopEntry.nhStart;
				_rtl8651_readAsicEntry(TYPE_NEXT_HOP_TABLE, nextHopTableIndex, &nexthop_entry);
				ipTableIndex = nexthop_entry.IPIndex;
				_rtl8651_readAsicEntry(TYPE_EXT_INT_IP_TABLE, ipTableIndex, &extip_entry);
			}
			else if (l3_entry.linkTo.NxtHopEntry.process == 2)
			{
				/* ARP Process */
				uint32 ipTableIndex;				

				ipTableIndex = READ_VIR32(GIDXMCR) & (1 << ((hsb->sip & 0x7) * 3)) >> ((hsb->sip & 0x7) * 3);
				_rtl8651_readAsicEntry(TYPE_EXT_INT_IP_TABLE, ipTableIndex, &extip_entry);
			}
			else
			{
				/* No Matched Entry */
				if (READ_VIR32(SWTCR0) &(NAPTR_NOT_FOUND_DROP))
				{
					/* Drop */
					MT_DROP("No Matched NAPT Entry => Drop")
					return MRET_DROP;
				}
				else
				{
					/* Trap to CPU*/
					MT_TOCPU("No Matched NAPT Entry => To CPU")
					return MRET_TOCPU;
				}
			}
			
			extip = extip_entry.externalIP;
			hsa->trip = extip;
			hsa->port = extport;
			MT_WATCH("Hash1 Matched")	
			return MRET_OK;
		}
	}

	/* Not Matched => Do Hash2 Matching */
	if (!matched)
	{
		uint32 index = hash2;
		uint32 vd;

		_rtl8651_readAsicEntry(TYPE_L4_TCP_UDP_TABLE, index, &napt_entry);
		vd = (napt_entry.valid << 1) + napt_entry.dedicate;
		naptPriority = napt_entry.priValid ? (napt_entry.priority) : -1;
		modelSetPriroity(PRI_NAPT, naptPriority, ale);


		if (vd == 0x3)
		{
			/* NAPT Hash2 */
			matched = (napt_entry.intIPAddr == hsb->sip) && (napt_entry.intPort == hsb->sprt);

			if (napt_entry.isTCP)
				matched &= (hsb->type == HSB_TYPE_TCP);
			else
				matched &= (hsb->type == HSB_TYPE_UDP);

			matched &= ((napt_entry.TCPFlag == 0/*bi-dir*/) ||
			            (napt_entry.TCPFlag == 1/*bi-dir*/) || 
			            (napt_entry.TCPFlag == 3/*outbound*/));

			if (matched)
			{
				if ( ((hsb->tcpfg & 1) || (hsb->tcpfg & 2) || (hsb->tcpfg & 4)) & ((napt_entry.TCPFlag & 4)==0) )
				{
					/* SYN, FIN, RST & Quiet Bit Set => Trap to CPU */
					MT_TOCPU("Hash2 Matched, SYN/FIN/RST Packets => To CPU")
					return MRET_TOCPU;
				}
				else
				{
					/* Lookup External IP & External Port */
					uint32 extip, extport;
					rtl8651_tblAsic_extIpTable_t extip_entry;
					uint32 agingTime;

					_rtl8651_readAsicEntry(TYPE_EXT_INT_IP_TABLE, napt_entry.selIPIdx, &extip_entry);

					agingTime = READ_VIR32( TEATCR );
					if ( napt_entry.isTCP==TRUE )
					{
						switch ( napt_entry.TCPFlag )
						{
							case 1: /* Inbound SYN received */
							case 2: /* Outbound SYN received */
								agingTime = (agingTime&TCPST_MASK)>>TCPST_OFFSET;
								break;
							case 4: /* Both SYN received */
								agingTime = (agingTime&TCPLT_MASK)>>TCPLT_OFFSET;
								break;
							case 5: /* Inbound FIN received */
							case 6: /* Outbound FIN received */
								agingTime = (agingTime&TCPMT_MASK)>>TCPMT_OFFSET;
								break;
							case 7: /* Both FIN received */
								agingTime = (agingTime&TCPST_MASK)>>TCPST_OFFSET;
								break;
							default:
								agingTime = 0;
								assert( 0 ); /* invalid napt_entry.TCPFlag configured */
						}
					}
					else
					{
						agingTime = (agingTime&UDPT_MASK)>>UDPT_OFFSET;
					}
					napt_entry.agingTime = agingTime;
					_rtl8651_forceAddAsicEntry(TYPE_L4_TCP_UDP_TABLE, index, &napt_entry);
				
					extip = extip_entry.externalIP;
					extport = (napt_entry.offset << 10) + napt_entry.selEIdx;

					hsa->trip = extip;
					hsa->port = extport;

					MT_WATCH("Hash2 Matched")
					return MRET_OK;
				}
			}
			else
			{
					/* Trap to CPU*/
					MT_TOCPU("No Matched NAPT Entry => To CPU")
					return MRET_TOCPU;
			}
		}
		else
		{
			/* No Matched Entry */
			if (READ_VIR32(SWTCR0) &(NAPTR_NOT_FOUND_DROP))
			{
				/* Drop */
				MT_DROP("No Matched NAPT Entry => Drop")
				return MRET_DROP;
			}
			else
			{
				/* Trap to CPU*/	
				hsa->why2cpu = cvtHostType2PHReason(ale, hsb) | 14;	/*DIP:RP   SIP:NPI and NAPT process */
				MT_TOCPU("No Matched NAPT Entry => To CPU")
				return MRET_TOCPU;
			}
		}
	}

	/* No Matched Entry */
	if (READ_VIR32(SWTCR0) &(NAPTR_NOT_FOUND_DROP))
	{
		/* Drop */
		MT_DROP("No Matched NAPT Entry => Drop")
		return MRET_DROP;
	}
	else
	{
		/* Trap to CPU*/
		MT_TOCPU("No Matched NAPT Entry => To CPU")
		return MRET_TOCPU;
	}
}


enum MODEL_RETURN_VALUE modelLayer4NaptWanLanProcessing( hsb_param_t* hsb, hsa_param_t* hsa,ale_data_t *ale)
{
	uint32 offset;
	rtl865xc_tblAsic_naptTcpUdpTable_t napt_entry;
	rtl865xc_tblAsic_l3RouteTable_t l3_entry;
	uint8 routingIndex ;
	uint32 matched = 0;
	int32 naptPriority;
	uint32 agingTime;
	
	routingIndex=ale->rouitingDIPIdx;
	
	if (READ_VIR32(SWTCR1) &(L4EnHash1))
	{
		uint32 idx1, vd, veri;
		hsb_param_t verihsb = *hsb;
		verihsb.dip = 0;
		verihsb.dprt = 0;
		

		idx1 = modelLayer4Hash1(hsb);
		veri = modelLayer4Hash1(&verihsb);
		_rtl8651_readAsicEntry(TYPE_L4_TCP_UDP_TABLE, idx1, &napt_entry);
		vd = (napt_entry.valid << 1) + napt_entry.dedicate;
		naptPriority = napt_entry.priValid ? (napt_entry.priority) : -1;
		modelSetPriroity(PRI_NAPT, naptPriority, ale);

		
		matched = (vd == 0x2) && (napt_entry.agingTime > 0) &&  (veri == napt_entry.selEIdx) && 
				   (napt_entry.TCPFlag = 3/*outbound*/);
		if (napt_entry.isTCP)
			matched &= (hsb->type == HSB_TYPE_TCP);
		else
			matched &= (hsb->type == HSB_TYPE_UDP);

		if (matched)
		{
			if (((hsb->tcpfg & 1) || (hsb->tcpfg & 2) || (hsb->tcpfg & 4)) & ((napt_entry.TCPFlag & 4) == 0))
			{
				/* SYN, FIN, RST & Quiet Bit Set => Trap to CPU */
				MT_TOCPU("Hash1 Matched, FIN, RST Packets => To CPU")
				return MRET_TOCPU;
			}
			else
			{
				uint16 veri_port;
				_rtl8651_readAsicEntry(TYPE_L3_ROUTING_TABLE, routingIndex, &l3_entry);

				veri_port = (napt_entry.selIPIdx << 6) + napt_entry.offset;
				if ((hsb->dprt & 1023)== veri_port)
				{
					agingTime = READ_VIR32( TEATCR );
					napt_entry.agingTime = napt_entry.isTCP ? (agingTime&TCPLT_MASK)>>TCPLT_OFFSET : (agingTime&UDPT_MASK)>>UDPT_OFFSET;
					_rtl8651_forceAddAsicEntry(TYPE_L4_TCP_UDP_TABLE, idx1, &napt_entry);

					hsa->trip = napt_entry.intIPAddr;
					hsa->port = napt_entry.intPort;

					MT_WATCH("Hash1 Matched")
					return MRET_OK;
				}
				else
				{
					/* Default Inbound Lookup Miss Action */
					if (READ_VIR32(SWTCR0) &(NAPTR_NOT_FOUND_DROP))
					{
						/* Drop */
						MT_DROP("No Matched NAPT Entry => Drop")
						return MRET_DROP;
					}
					else
					{
						/* Trap to CPU*/
						MT_TOCPU("No Matched NAPT Entry => To CPU")
						return MRET_TOCPU;
					}
				}
			}
		}
	}
	else
	{
		uint32 idx;
		
		idx = hsb->dprt & 0x3FF;
		offset = (hsb->dprt & 0xFC00) >> 10;
		_rtl8651_readAsicEntry(TYPE_L4_TCP_UDP_TABLE, idx, &napt_entry);

		naptPriority = napt_entry.priValid ? (napt_entry.priority) : -1;
		modelSetPriroity(PRI_NAPT, naptPriority, ale);
		if (napt_entry.valid &&											/* Valid Bit Set */
		    ( (napt_entry.isTCP==TRUE && hsb->type==HSB_TYPE_TCP) ||
		      (napt_entry.isTCP==FALSE && hsb->type==HSB_TYPE_UDP) ) )
		{
			/* IP Table Index Matched */
			if (napt_entry.selIPIdx == offset)
			{
				/* Dedicate Bit Set */
				if (napt_entry.dedicate)
				{
					/* Hash2 */

					/* Direction == 0 || Outbound == 0 */
					if (((napt_entry.TCPFlag & 0x2) == 0) || ((napt_entry.TCPFlag & 0x1) == 0))
					{
						/* SYN, FIN, RST & Quiet Bit Set => Trap to CPU */
						if ( ((hsb->tcpfg & 1) || (hsb->tcpfg & 2) || (hsb->tcpfg & 4)) & (napt_entry.TCPFlag & 4) )
						{
							MT_TOCPU("Hash2 Matched, SYN/FIN/RST Packets => To CPU")
							return MRET_TOCPU;
						}
						else
						{
							/* Lookup Internal IP & Internal Port */
							uint32 intip, intport;

							agingTime = READ_VIR32( TEATCR );
							if ( napt_entry.isTCP==TRUE )
							{
								switch ( napt_entry.TCPFlag )
								{
									case 1: /* Inbound SYN received */
									case 2: /* Outbound SYN received */
										agingTime = (agingTime&TCPST_MASK)>>TCPST_OFFSET;
										break;
									case 4: /* Both SYN received */
										agingTime = (agingTime&TCPLT_MASK)>>TCPLT_OFFSET;
										break;
									case 5: /* Inbound FIN received */
									case 6: /* Outbound FIN received */
										agingTime = (agingTime&TCPMT_MASK)>>TCPMT_OFFSET;
										break;
									case 7: /* Both FIN received */
										agingTime = (agingTime&TCPST_MASK)>>TCPST_OFFSET;
										break;
									default:
										agingTime = 0;
										assert( 0 ); /* invalid napt_entry.TCPFlag configured */
								}
							}
							else
							{
								agingTime = (agingTime&UDPT_MASK)>>UDPT_OFFSET;
							}
							napt_entry.agingTime = agingTime;
							_rtl8651_forceAddAsicEntry(TYPE_L4_TCP_UDP_TABLE, idx, &napt_entry);
							
							intip = napt_entry.intIPAddr;
							intport = napt_entry.intPort;

							hsa->trip = intip;
							hsa->port = intport;

							MT_WATCH("Hash2 Matched")
							return MRET_OK;
						}
					}
					else
					{
						/* Default Inbound Lookup Miss Action */
						if (READ_VIR32(SWTCR0) &(NAPTR_NOT_FOUND_DROP))
						{
							/* Trap to CPU*/
							MT_TOCPU("No Matched NAPT Entry => To CPU")
							return MRET_TOCPU;
						}
						else
						{
							/* Drop */
							MT_DROP("No Matched NAPT Entry => Drop")
							return MRET_DROP;	
						}
					}
				}
				/* Dedicate Bit Not Set */
				else
				{
					/* Hash1 */
					
					uint32 extip, extport;

					_rtl8651_readAsicEntry(TYPE_L3_ROUTING_TABLE, routingIndex, &l3_entry);

					/* Lookup External IP & External Port */
					if (l3_entry.linkTo.NxtHopEntry.process == 5)
					{
						/* Next Hop Process */
						uint32 nextHopTableIndex;
						rtl8651_tblAsic_nextHopTable_t nexthop_entry;
						uint32 ipTableIndex;
						rtl8651_tblAsic_extIpTable_t extip_entry;

						nextHopTableIndex = l3_entry.linkTo.NxtHopEntry.nhStart;
						_rtl8651_readAsicEntry(TYPE_NEXT_HOP_TABLE, nextHopTableIndex, &nexthop_entry);
						ipTableIndex = nexthop_entry.IPIndex;
						_rtl8651_readAsicEntry(TYPE_EXT_INT_IP_TABLE, ipTableIndex, &extip_entry);

						extip = extip_entry.externalIP;
					}
					else if (l3_entry.linkTo.NxtHopEntry.process == 2)
					{
						/* ARP Process */
						uint32 ipTableIndex;
						rtl8651_tblAsic_extIpTable_t extip_entry;

						ipTableIndex = READ_VIR32(GIDXMCR) & (1 << ((hsb->sip & 0x7) * 3)) >> ((hsb->sip & 0x7) * 3);
						_rtl8651_readAsicEntry(TYPE_EXT_INT_IP_TABLE, ipTableIndex, &extip_entry);

						extip = extip_entry.externalIP;
					}
					else
					{
						/* No Matched Entry */
						if (READ_VIR32(SWTCR0) &(NAPTR_NOT_FOUND_DROP))
						{
							/* Drop */
							MT_DROP("No Matched NAPT Entry => Drop")
							return MRET_DROP;
						}
						else
						{
							/* Trap to CPU*/
							MT_TOCPU("No Matched NAPT Entry => To CPU")
							return MRET_TOCPU;
						}
					}

					extport = (napt_entry.offset << 10) + idx;

					/* Verify This Hash Entry is Matched */
					if ( (extip == hsb->dip) && (extport == hsb->dprt) )
					{
						/* Lookup Internal IP & Internal Port */
						uint32 intip, intport;

						agingTime = READ_VIR32( TEATCR );
						if ( napt_entry.isTCP==TRUE )
						{
							switch ( napt_entry.TCPFlag )
							{
								case 1: /* Inbound SYN received */
								case 2: /* Outbound SYN received */
									agingTime = (agingTime&TCPST_MASK)>>TCPST_OFFSET;
									break;
								case 4: /* Both SYN received */
									agingTime = (agingTime&TCPLT_MASK)>>TCPLT_OFFSET;
									break;
								case 5: /* Inbound FIN received */
								case 6: /* Outbound FIN received */
									agingTime = (agingTime&TCPMT_MASK)>>TCPMT_OFFSET;
									break;
								case 7: /* Both FIN received */
									agingTime = (agingTime&TCPST_MASK)>>TCPST_OFFSET;
									break;
								default:
									agingTime = 0;
									assert( 0 ); /* invalid napt_entry.TCPFlag configured */
							}
						}
						else
						{
							agingTime = (agingTime&UDPT_MASK)>>UDPT_OFFSET;
						}
						napt_entry.agingTime = agingTime;
						_rtl8651_forceAddAsicEntry(TYPE_L4_TCP_UDP_TABLE, idx, &napt_entry);

						intip = napt_entry.intIPAddr;
						intport = napt_entry.intPort;

						hsa->trip = intip;
						hsa->port = intport;

						MT_WATCH("Hash1 Matched")
						return MRET_OK;
					}
					else
					{
						/* Default Inbound Lookup Miss Action */
						if (READ_VIR32(SWTCR0) &(NAPTR_NOT_FOUND_DROP))
						{
							/* Drop */
							MT_DROP("No Matched NAPT Entry => Drop")
							return MRET_DROP;
						}
						else
						{
							/* Trap to CPU*/
							MT_TOCPU("No Matched NAPT Entry => To CPU")
							return MRET_TOCPU;
						}
					}
				}
			}
			/* IP Table Index Not Matched */
			else
			{
				/* Default Inbound Lookup Miss Action */
				if (READ_VIR32(SWTCR0) &(NAPTR_NOT_FOUND_DROP))
				{
					/* Drop */
					MT_DROP("No Matched NAPT Entry => Drop")
					return MRET_DROP;
				}
				else
				{
					/* Trap to CPU*/
					MT_TOCPU("No Matched NAPT Entry => To CPU")
					return MRET_TOCPU;
				}
			}
		}
		/* Valid Bit Not Set */
		else
		{
			/* Default Inbound Lookup Miss Action */
			if (READ_VIR32(SWTCR0) &(NAPTR_NOT_FOUND_DROP))
			{
				/* Drop */
				MT_DROP("No Matched NAPT Entry => Drop")
				return MRET_DROP;
			}
			else
			{
				/* Trap to CPU*/
				MT_TOCPU("No Matched NAPT Entry => To CPU")
				return MRET_TOCPU;
			}
		}
	}

	/* No Matched Entry */
	if (READ_VIR32(SWTCR0) &(NAPTR_NOT_FOUND_DROP))
	{
		/* Drop */
		MT_DROP("No Matched NAPT Entry => Drop")
		return MRET_DROP;
	}
	else
	{
		/* Trap to CPU*/
		MT_TOCPU("No Matched NAPT Entry => To CPU")
		return MRET_TOCPU;
	}
}

