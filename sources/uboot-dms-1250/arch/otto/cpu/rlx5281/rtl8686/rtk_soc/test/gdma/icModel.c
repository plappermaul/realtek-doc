/*
* Copyright c                  Realtek Semiconductor Corporation, 2002  
* All rights reserved.
* 
* Program : Source File for IC-specific Function
* Abstract : 
* Author : Louis Yung-Chieh Lo (yjlou@realtek.com.tw)               
* $Id: icModel.c,v 1.136 2007-03-16 04:04:55 chenyl Exp $
*/

#include "rtl_types.h"
#include "asicregs.h"
#include "assert.h"
#include "gdma_glue.h"
#include "virtualMac.h"
#include "icModel.h"
//#include "hsModel.h"
//#include "l2Model.h"
//#include "l34Model.h"
//#include "aclModel.h"
#include "modelTrace.h"
#include "gdmaModel.h"
//#include "swNic2.h"

#include "gdma_utils.h"
//#include "pktForm.h"
#include "rtl865xC_tblAsicDrv.h"
//#include "rtl8651_tblDrvProto.h"
//#include "rtl8651_tblDrvFwdLocal.h"
#include "icExport.h"
//#if defined(VSV)||defined(MIILIKE)
//#include "vsv_conn.h"
//#endif
//#ifdef VSV
//#include <unistd.h> /* for sleep() */
//#endif


#define CHECK_RETVAL(A) do { \
		if ((retval=A)==MRET_DROP) \
			goto drop;\
		else if (retval==MRET_TOCPU)\
			goto tocpu; \
	} while (0)

//#ifdef CONFIG_RTL865XC 
uint32 FIRST_CPUTPDCR0=0, FIRST_CPUTPDCR1=1;
uint32 FIRST_CPURPDCR0=0,FIRST_CPURPDCR1=0,FIRST_CPURPDCR2=0,FIRST_CPURPDCR3=0,FIRST_CPURPDCR4=0,FIRST_CPURPDCR5=0;
uint32 FIRST_CPURMDCR0=0;

//#define MAX_QUEUE_ENTRY 16
//uint32 QueuePriority[MAX_QUEUE_ENTRY]={0};
//uint8   QueuePriorityIndex=0;
//#else
//uint32 FIRST_CPUTPDCR=0;
//uint32 FIRST_CPURPDCR=0;
//uint32 FIRST_CPURMDCR=0;
//#endif

/*
 * Even if we use VSV to speed up simulation, the packet is still pushed into IC with 100M speed.
 * To optimize, we can configure to 1G speed.
 */
#define GIGAIO

#if 0
/*
 *  Since packet translator uses HSA to modify original packet, the final step of translator is enqueue the packet to
 *  either physical port or CPU port.
 *  The following structure is used to store these packets.
 *  The txPort[] is used for virtualMacOutput() to poll the packets destinated to physical ports.
 *  Those packets destinated to CPU (including extension ports) are enqueued in NIC p-m-c architecture.
 */
packetHead_t txPort[10]; /* PN_PORT0~PN_PORT5 + PN_PORT_EXT0~PN_PORT_CPU */;
packetHead_t pktFree;


/*
 *  Since parser and translator of model code only accept continuous packet content,
 *  NIC must merge packet content from several mbufs.
 *  Another issue is endian problem. In this buffer, the packet content is in network order.
 */
static uint8 clusterBuffer[16*1024];
uint8 *tranClusterEndianForCpuTx( struct rtl_pktHdr *pPkt )
{
	uint8 *pBuf, *pMdata;
	struct rtl_mBuf *pMbuf;

	assert( sizeof(clusterBuffer)!=sizeof(void*) ); /* to prevent programmer change clusterBuffer from array to pointer. */

	for( pBuf = &clusterBuffer[0], pMbuf = pPkt->ph_mbuf;
	     pMbuf!=NULL;
	     pBuf += pMbuf->m_len, pMbuf = pMbuf->m_next )
	{
		int l; /* completed length */
		pMdata = pMbuf->m_data;
		for( l = 0; l<((pMbuf->m_len+3)&~3); l+=4 )
		{
			assert( ((&pBuf[l+3])-(&clusterBuffer[0])) <= sizeof(clusterBuffer) ); /* to prevent overwrite other data */
			
			pBuf[l+0] = pMdata[l+3];
			pBuf[l+1] = pMdata[l+2];
			pBuf[l+2] = pMdata[l+1];
			pBuf[l+3] = pMdata[l+0];
		}
	}
rtlglue_printf("pBuf-&clusterBuffer[0]=%d pPkt->ph_len=%d\n",pBuf-&clusterBuffer[0],pPkt->ph_len);

	assert( (pBuf-&clusterBuffer[0])==pPkt->ph_len );
	return &clusterBuffer[0];
}


/*
 *  Model code that simulates switch core full reset.
 */
void modelSwcoreFullReset( void )
{
}


/*
 *  Model code that simulates switch core semi reset.
 */
void modelSwcoreSemiReset( void )
{
}





struct grehdr					   /* Enhanced GRE header. */
{
	uint16    gh_flags;			   /* Flags. */
	uint16    gh_protocol;			   /* Protocol type. */
	uint16    gh_length;			   /* Payload length. */
	uint16    gh_call_id;			   /* Call ID. */
	uint32    gh_seq_no;			   /* Sequence number (optional). */
	uint32    gh_ack_no;			   /* Acknowledgment number (optional). */
};
#endif


#if 0
#if defined(RTL865X_MODEL_USER)||defined(RTL865X_MODEL_KERNEL)
/*********************************************************************************
* Routine Name :  model_ipChecksum                                   
* Description : 
* Input :                                                               
* Output :                                                             
* Return :                                                             
* Note :   
*        ThreadSafe: n
**********************************************************************************/
uint16 model_ipChecksum(struct ip * pip)
{
	uint32 sum = 0 , oddbyte = 0;
	uint16 *ptr = (uint16 *)pip;
	uint32 nbytes = ((pip->ip_vhl & 0xf) << 2);	

	while (nbytes > 1)
	{
		sum += (*ptr++);
		nbytes -= 2;
	}
	if (nbytes == 1)
	{
		oddbyte = (*ptr & 0xff00);
		sum += oddbyte;
	}
	
	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	
	return (~sum);
}


/*********************************************************************************
* Routine Name :  model_tcpChecksum
* Description : 
* Input :                                                               
* Output :                                                             
* Return :                                                             
* Note :   
*        ThreadSafe: n
**********************************************************************************/
uint16 model_tcpChecksum(struct ip *pip)
{
	int32 sum, nbytes, nhdr, i;
	uint16 *sip, *dip, *ptr;
	uint8 backup; /* Since pading zero will modify packet content, we backup the original charact. */

	nhdr =  ((pip->ip_vhl & 0xf) << 2);
	nbytes = ntohs(pip->ip_len) - nhdr;
	ptr = (uint16 *) ((char *) pip + nhdr);
	sum = 0;

	/* Note: We always padding zero to the tail of the packet!! */
	backup = *(((uint8 *)pip) + nhdr + nbytes);
	*(((uint8 *)pip) + nhdr + nbytes) = (uint8)0;
	for (i=0; i<nbytes ;i=i+2)
	{
		sum += (unsigned long)ntohs(*ptr);
		ptr++;
	}

	/* "Pseudo-header" data */
	dip=(uint16 *)&pip->ip_dst;
	sum += ntohs(*dip); dip++;
	sum += ntohs(*dip);
	sip=(uint16 *)&pip->ip_src;
	sum += ntohs(*sip); sip++;
	sum += ntohs(*sip);	

	sum +=  nbytes;
	sum += ((uint16) pip->ip_p);

	/* Roll over carry bits */
	while (sum>>16)
		sum = (sum & 0xFFFF)+(sum >> 16);
	
	*(((uint8 *)pip) + nhdr + nbytes) = backup;
	
	/* Take the one's complement of sum */
	sum = ~sum;
	return htons((uint16)sum);
	
}

/*********************************************************************************
* Routine Name :  model_icmpChecksum
* Description : 
* Input :                                                               
* Output :                                                             
* Return :                                                             
* Note :   
*        ThreadSafe: n
**********************************************************************************/
/* shoud continue memory */
uint16 model_icmpChecksum(struct ip * pip)
{
	uint16   *ptr;
	int32  nhdr, nbytes;
	int32  sum, i;

	nhdr =  ((pip->ip_vhl &0xf) << 2);
	nbytes = ntohs(pip->ip_len) - nhdr;
	ptr = (uint16 *) ((char *) pip + nhdr);
	sum = 0;

	/* Note: we always padding here!! */
	*(((uint8 *)pip) + nhdr + nbytes) = (uint8)0;
	for (i=0; i<nbytes ; i=i+2){
		sum +=  (unsigned long) *ptr++;
	}

	/* Roll over carry bits */
	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);

	/* Return checksum */
	return ((uint16) ~ sum);
}
#endif

enum state_info
{
	DIRECT_TX_FROM_CPU=0,
	DIRECT_TX_TO_CPU,
	RX_FROM_PHYSICAL_PORT,
	RX_FROM_EXT_PORT,
};
/*
func void		| modelPktParser	| Parse packet, and output to HSB.
parm enum PORT_MASK | fromPort | The port number of packet from
parm uint8* | data | Packet content
parm uint8 | srcPortNum | source port number, 7: from CPU
parm uint8 | srcExtPortNum | source extersion port number, 0:Ext0, 1:Ext1, 2:Ext2, 3:CPU (valid only if srcPortNum = 7)
parm uint8 | isDirectTXfromCPU | is this packet DirectTX(skip ALE) and send from CPU.
parm uint16 | length | Length of packet (included L2 CRC)
comm 
This function will model the packet parser of ASIC.
It will generate HSB according to the input packet content.
*/
void modelPktParser(hsb_param_t *hsb,uint8 *data,uint8 srcPortNum,uint16 length,struct rtl_pktHdr *pPkt)
{
	struct ip *ip=NULL;
	struct tcphdr *tc=NULL;
	struct udphdr *ud=NULL;	
	struct icmp *icmp=NULL;
	struct igmp *igmp=NULL;
	struct grehdr *gre=NULL;
	int32 ip_offset;
	uint32 ethTypePos;
	uint8 state=0; 
	int8 isIP=FALSE;
	int8 isBridging=0;
	int8 isHwlookup=1;	
	uint8 queuePriority=0;
	uint8 isDirectTXfromCPU=0;
	uint8 srcExtPortNum=0;
	uint16 vlanId=0;
	uint8 szComment[128];

	MT_WATCH( "==============================" );
	sprintf( szComment, "Parse a packet (%d bytes) from port %d", length, srcPortNum );
	MT_WATCH( szComment );

	ip_offset = 14;
	ethTypePos = 12;
	memset(hsb,0,sizeof(*hsb));
	hsb->spa=srcPortNum;


	/* check state */
	if(srcPortNum==7)
	{
		if(pPkt!=NULL)
		{ 

			isHwlookup=(pPkt->ph_flags&(PKTHDR_HWLOOKUP))?1:0;

			isBridging=(pPkt->ph_flags&(PKTHDR_BRIDGING))?1:0;
			if ( READ_VIR32(CPUICR)&EXCLUDE_CRC )
				hsb->len=(pPkt->ph_len)+4;
			else
				hsb->len=(pPkt->ph_len);

			if(isHwlookup==0)
			{
				isDirectTXfromCPU=1;
				state=DIRECT_TX_FROM_CPU;
				srcExtPortNum=3;				
			}
			else
			{		
				state=RX_FROM_EXT_PORT;
				if(pPkt->ph_srcExtPortNum!=0) 
				{
					srcExtPortNum=pPkt->ph_srcExtPortNum-1;
				}
				else
				{
					rtlglue_printf("Warning: Undefined case: From Ext Port can't set PKTHDR_HWLOOKUP.\n");
					srcExtPortNum=3;
				}
			}			
		}
		else
		{
			rtlglue_printf("Warning: Undefined case: can't get pkthdr when srcPortNum=7.");
		}
		hsb->extspa=srcExtPortNum;	
	}
	else
	{
		state=RX_FROM_PHYSICAL_PORT;
		hsb->len=length;
	}

	
	if(state==RX_FROM_EXT_PORT)
	{		
		if(isBridging)
			hsb->extl2=1;
		else
			hsb->extl2=0;		
		if(!isHwlookup)
		{
			rtlglue_printf("Warning: Undefined case: Direct TX from ext port\n");
		}
		hsb->linkid=(pPkt->ph_linkID);	
	}

	
	if(state==DIRECT_TX_FROM_CPU)
	{
		if(isHwlookup)
		{
			rtlglue_printf("Warning: Undefined case: Direct TX from CPU can't do hardware ALE.\n");
			isHwlookup=0;
		}
		else
		{
			if(isBridging)
			{
				rtlglue_printf("Warning: Undefined case: Invalid setting with BRIDGING=1, HWLOOKUP=0.\n");
				isBridging=0;
			}
		}
	}
	else
	{
		hsb->da[0]=data[0];
		hsb->da[1]=data[1];
		hsb->da[2]=data[2];
		hsb->da[3]=data[3];
		hsb->da[4]=data[4];
		hsb->da[5]=data[5];	
		hsb->sa[0]=data[6];	
		hsb->sa[1]=data[7];	
		hsb->sa[2]=data[8];	
		hsb->sa[3]=data[9];	
		hsb->sa[4]=data[10];	
		hsb->sa[5]=data[11];			
	}

	/* parser packet raw data */
	if((data[ethTypePos]==0x81)&&(data[ethTypePos+1]==0x00))  /* VLAN-Tag */
	{	
		int cfi;
		vlanId=((data[ethTypePos+2]<<8)|data[ethTypePos+3])&0xfff;	
		
		cfi=(data[ethTypePos+2]>>4)&0x1;  /* 802.1Q CFI */
		if(cfi==1) /* cfi==1 , HW can't handle ALE, return reason to CPU */
		{
			if(state!=DIRECT_TX_FROM_CPU)
			{
				hsb->tcpfg|=1; /* reason = 1 */
				state=DIRECT_TX_TO_CPU;
			}
		}

		queuePriority=(data[ethTypePos+2]>>5)&0x7;  /* 802.1Q Priority */		
		ip_offset+=4;
		ethTypePos+=4;
		if(state!=DIRECT_TX_FROM_CPU)
		{
			hsb->tagif=1;
		}
	}	

	if(( ((uint16)(data[ethTypePos])<<8)|((uint16)data[ethTypePos+1]))<0x600)  /* SNAP or LLC other */
	{
		if((data[ethTypePos+2]==0xaa)&&(data[ethTypePos+3]==0xaa)&&(data[ethTypePos+4]==0x03)&&
		   (data[ethTypePos+5]==0)&&(data[ethTypePos+6]==0)&&(data[ethTypePos+7]==0))
		{
			if(state!=DIRECT_TX_FROM_CPU)
			{		
				hsb->snap=TRUE;
			}
			ip_offset+=8; /*for SNAP*/
			ethTypePos+=8; /*for SNAP*/
		}
		else
		{
			if(state!=DIRECT_TX_FROM_CPU)
			{
				hsb->llcothr=TRUE;
//				hsb->ethtype=((data[ethTypePos+2]<<8)&0xff00)|(data[ethTypePos+3]&0xff);
			}
		}
	}	

	if(( ((uint16)(data[ethTypePos])<<8)|((uint16)data[ethTypePos+1]))>=0x600)  /* Ethernet */
	{		
		if((data[ethTypePos]==0x88)&&(data[ethTypePos+1]==0x64)) /*PPPoE-Tag*/
		{

			if(state!=DIRECT_TX_FROM_CPU)
			{
				/* get PPPoE session ID from PPPoE header offset:2byte */			
				hsb->pppoeid=(data[ethTypePos+4]<<8)+data[ethTypePos+5];	
				hsb->pppoeif=1;
			}
			ip_offset+=8;				
		}

		if(((data[ethTypePos]==0x08)&&(data[ethTypePos+1]==0x00)) ||  /* Ethertype = IP */
		    ((data[ethTypePos]==0x88)&&(data[ethTypePos+1]==0x64)&&(data[ethTypePos+8]==0x00)&&(data[ethTypePos+9]==0x21))) /* PPPoE with IPv4 header */
		{
			ip=(struct ip *)(&data[ip_offset]);
			if(ip->ip_vhl!=0x45) /* ip len !=5 or  ver !=4 , HW can't handle ALE, return reason to CPU */
			{
				if(state!=DIRECT_TX_FROM_CPU)
				{
					hsb->tcpfg|=2; /* reason = bit:1 */
					hsb->tcpfg|=(ip->ip_vhl&0xf)<<2; /* keep ip header length to bit[2~5] */
					state=DIRECT_TX_TO_CPU;
					if((ip->ip_vhl>0x45)&&(ip->ip_vhl<=0x4f))
					{
						hsb->tcpfg|=(1<<6);				
						isIP=TRUE;
					}
				}
				else
				{
					if((ip->ip_vhl>0x45)&&(ip->ip_vhl<=0x4f))
					{
						isIP=TRUE;
					}
				}
				
			}
			else 
			{
				isIP=TRUE;
			}

			
		}
	}


	if(isIP)
	{
		
		/* layer 3 */
		/*hsb->len=ntohs(ip->ip_len)+ip_offset;*/
#ifdef _LITTLE_ENDIAN
		hsb->sip=ntohl(ip->ip_src.s_addr);
		hsb->dip=ntohl(ip->ip_dst.s_addr);
#else
		/* This is big endian, said, target platform. Therefore, we must handle alignment issue. */
		hsb->sip=(((uint8*)&ip->ip_src.s_addr)[0]<<24)|(((uint8*)&ip->ip_src.s_addr)[1]<<16)|(((uint8*)&ip->ip_src.s_addr)[2]<<8)|(((uint8*)&ip->ip_src.s_addr)[3]<<0);
		hsb->dip=(((uint8*)&ip->ip_dst.s_addr)[0]<<24)|(((uint8*)&ip->ip_dst.s_addr)[1]<<16)|(((uint8*)&ip->ip_dst.s_addr)[2]<<8)|(((uint8*)&ip->ip_dst.s_addr)[3]<<0);
#endif
		hsb->ipfg=(ntohs(ip->ip_off)>>13)&0x7;
		hsb->iptos=ip->ip_tos;
	
	
		/*set IP TTL value*/
		if(ip->ip_ttl==0)
			hsb->ttlst=0;
		else if(ip->ip_ttl==1)
			hsb->ttlst=1;
		else
			hsb->ttlst=2;
	
	
		/* IP fragmentation offset status , 0:offset =0 , 1:not 0*/
		if((ntohs(ip->ip_off)&IP_OFFMASK)==0) 
			hsb->ipfo0_n=0;
		else
			hsb->ipfo0_n=1;
		

		/* layer 4 */
		switch(ip->ip_p)
		{
			case IPPROTO_TCP:				
				tc = (struct tcphdr *) ((int8 *)ip + ((ip->ip_vhl &0xf) << 2));
				hsb->ipptl=ip->ip_p; 
				if(state!=DIRECT_TX_TO_CPU) hsb->tcpfg=tc->th_flags; 
				hsb->type=5;
				break;

			case IPPROTO_UDP:
				ud = (struct udphdr *) ((int8 *)ip + ((ip->ip_vhl &0xf) << 2));
				hsb->ipptl=ip->ip_p; 
				hsb->type=6;
				/*  UDP and checksum = 0*/
				if(ntohs(ud->uh_sum)==0) hsb->udpnocs=1;
				break;

			case IPPROTO_ICMP:
				icmp = (struct icmp *) ((int8 *)ip + ((ip->ip_vhl &0xf) << 2));
				hsb->ipptl=icmp->icmp_type;
				if(state!=DIRECT_TX_TO_CPU) hsb->tcpfg=icmp->icmp_code; /*ICMP Code*/
				hsb->type=3;
				break;

			case IPPROTO_IGMP:				
				igmp = (struct igmp *) ((int8 *)ip + ((ip->ip_vhl &0xf) << 2));
				hsb->ipptl=igmp->igmp_type;
				hsb->type=4;
				break;			

			case IPPROTO_GRE:
				gre = (struct grehdr *) ((int8 *)ip + ((ip->ip_vhl &0xf) << 2));
				hsb->ipptl=ip->ip_p; 
				hsb->type=1;
				break;

			default:
				hsb->ipptl=ip->ip_p; 
				hsb->type=2;
		}	


		/* Checksum check, and HW checksum calculate (HW do this in MAC-RX, before create HSB) */
		{
	
			uint16 l3sum=0,l4sum=0;


			if((hsb->ipfo0_n==0)&&(hsb->udpnocs==0)) /*don't care UDP no-checksum or fragment packets.*/
			{
				if(ip->ip_p==IPPROTO_TCP)
				{
					l4sum = model_tcpChecksum(ip);

					if(l4sum==0x0000) 
						hsb->l4csok=1;
					else
						hsb->l4csok=0;

					if(!((state==RX_FROM_PHYSICAL_PORT)||(state==DIRECT_TX_TO_CPU)))
					{

						if(pPkt->ph_flags & (CSUM_L4))
						{
							hsb->dprt=ntohs(tc->th_sum);
							tc->th_sum=0;
						}
					}


				
				}
				else if(ip->ip_p==IPPROTO_UDP)
				{
					l4sum = model_tcpChecksum(ip);

					if(l4sum==0x0000) 
						hsb->l4csok=1;
					else
						hsb->l4csok=0;

					if(!((state==RX_FROM_PHYSICAL_PORT)||(state==DIRECT_TX_TO_CPU)))
					{

						if(pPkt->ph_flags & (CSUM_L4))
						{
							hsb->dprt=ntohs(ud->uh_sum);
							ud->uh_sum=0;
						}
					}
					
				}

				else if(ip->ip_p==IPPROTO_ICMP)
				{
					l4sum = model_icmpChecksum(ip);

					if(l4sum==0x0000) 
						hsb->l4csok=1;
					else
						hsb->l4csok=0;

					if(!((state==RX_FROM_PHYSICAL_PORT)||(state==DIRECT_TX_TO_CPU)))
					{

						if(pPkt->ph_flags & (CSUM_L4))
						{
							hsb->dprt=ntohs(icmp->icmp_cksum);
							icmp->icmp_cksum=0;
						}
					}
					
				}

				else if(ip->ip_p==IPPROTO_IGMP)
				{
					l4sum = model_icmpChecksum(ip);

					if(l4sum==0x0000) 
						hsb->l4csok=1;
					else
						hsb->l4csok=0;

					if(!((state==RX_FROM_PHYSICAL_PORT)||(state==DIRECT_TX_TO_CPU)))
					{

						if(pPkt->ph_flags & (CSUM_L4))
						{
							hsb->dprt=ntohs(igmp->igmp_cksum);
							igmp->igmp_cksum=0;
						}
					}
				}
			}

			l3sum = model_ipChecksum(ip);

			if(l3sum==0x0000) 
				hsb->l3csok=1;
			else
				hsb->l3csok=0;			
			if(!((state==RX_FROM_PHYSICAL_PORT)||(state==DIRECT_TX_TO_CPU)))
			{

				if(pPkt->ph_flags & (CSUM_IP))
				{
					hsb->sprt=ntohs(ip->ip_sum);
					ip->ip_sum=0;
				}
			}
		}
	}
	else
	{
		hsb->type=0;
	}

	/* must after L3/L4 parser and pattern match, before url trap */
	if(state==DIRECT_TX_FROM_CPU)
	{
#if 0 /* yjlou: we have added in the start of parser */
		hsb->len=(pPkt->ph_len)+4; //with CRC length
#endif
		hsb->trigpkt=0;
		hsb->vid=(pPkt->ph_vlanId)&0xfff; /* Direct TX: vid copy from pkthdr */
		hsb->tagif=pPkt->ph_vlanTagged;
		hsb->pppoeif=pPkt->ph_pppeTagged;
		hsb->snap=pPkt->ph_LLCTagged;		
		hsb->sip=0;
		hsb->dip=0;
		hsb->ipptl=0;
		hsb->ipfg=0;
		hsb->iptos=0;
		hsb->tcpfg=0;
		hsb->type=pPkt->ph_type;
		hsb->patmatch=0;
		hsb->dirtx=((pPkt->ph_flags&(PKTHDR_BRIDGING|PKTHDR_HWLOOKUP))!=0)?1:0;
		hsb->ethtype=(((pPkt->ph_flags2)&0x3f)<<10)|((pPkt->ph_portlist&0x3f)<<4)|((((pPkt->ph_flags)>>2)&1)<<3)|(pPkt->ph_pppoeIdx); /* for Direct TX, this field is { DoVLANTAG[5:0], PortMask[5:0], DoPPPoETAG, PPPoEID[2:0] }				*/
		hsb->hiprior=((pPkt->ph_vlanId)>>12)&0x7; /* bit 12~14: priority, Direct TX: hiprior copy from pkthdr */
		hsb->udpnocs=0;
		hsb->ttlst=0;
		hsb->l3csok=1;
		hsb->l4csok=1;
		hsb->extspa=3;
		hsb->extl2=0;
		hsb->linkid=0;
	}
	else  /* DIRECT_TX_TO_CPU,   RX_FROM_PHYSICAL_PORT,    RX_FROM_EXT_PORT */
	{

		/* check pattern match */
		if((READ_MEM32(PPMAR)&0x2000)!=0) /*enable Pattern Match*/
		{
			if(srcPortNum<6)
			{
				if((READ_MEM32(PPMAR)&(1<<(srcPortNum+26)))!=0) /*enable Per-Port Pattern Match*/
				{				
					uint32 search,mask,value;
					int i;
					search=ntohl(READ_MEM32(PATP0+srcPortNum*4));
					mask=ntohl(READ_MEM32(MASKP0+srcPortNum*4));
					for(i=0;i<length-4;i++)
					{
						value=(data[i]<<24)|(data[i+1]<<16)|(data[i+2]<<8)|(data[i+3]);
						value=ntohl(value);
						
						if((value&mask)==search) 
						{
							hsb->patmatch=1;
#if 0
							retval=(READ_MEM32(PPMAR)>>24)&0x3; /*save return Action*/
#endif
							break;
						}
					}
				}
			}
		}	

	/* check url match */
	{
		uint32 search,value,i;
		search=('T'<<24)|('T'<<16)|('P'<<8)|('/');			
		for(i=0;i<length-4;i++)
		{
			value=(data[i]<<24)|(data[i+1]<<16)|(data[i+2]<<8)|(data[i+3]);
			if(value==search) 
			{
				hsb->urlmch=1;					
				break;
			}
		}
	}	

	
		hsb->vid= vlanId;
		hsb->hiprior= queuePriority;

		if(isIP)
		{
			switch(ip->ip_p)
			{
				case IPPROTO_TCP:
					hsb->sprt=ntohs(tc->th_sport); /* direct TX: this field is used for L3 checksum */
					hsb->dprt=ntohs(tc->th_dport); /* direct TX: this field is used for L4 checksum */
					break;
					
				case IPPROTO_UDP:
					hsb->sprt=ntohs(ud->uh_sport); /* direct TX: this field is used for L3 checksum */
					hsb->dprt=ntohs(ud->uh_dport); /* direct TX: this field is used for L4 checksum */
					break;
					
				case IPPROTO_ICMP:
					hsb->sprt=ntohs(icmp->icmp_id); /* direct TX: this field is used for L3 checksum */
					break;

				case IPPROTO_IGMP:
					break;					
					
				case IPPROTO_GRE:
					hsb->sprt=ntohs(gre->gh_call_id); /* direct TX: this field is used for L3 checksum */
					break;
			}
		}

		hsb->ethtype=(data[ethTypePos]<<8)|(data[ethTypePos+1]);
		if(hsb->ethtype<0x600) 
		{
			if(hsb->llcothr)				
				hsb->ethtype=((data[ethTypePos+2]<<8)&0xff00)|(data[ethTypePos+3]&0xff);
			else
				hsb->ethtype=0;
		}

		
		
		
	}

	if((state==DIRECT_TX_FROM_CPU)||(state==DIRECT_TX_TO_CPU))
	{
		hsb->dirtx=1;
	}

	modelSetHsb(hsb);
/*	dumpHSB();	*/

}
#endif

//#if defined(RTL865X_MODEL_USER)||defined(RTL865X_MODEL_KERNEL)
#if 1
//#if defined(VERA)
#if 0
/*
 *  This function translates real address 'reg' into virtual address 'reg_tmp'.
 */
void write_mem_wrapper(int reg, int val)
{
	uint32 reg_tmp;

	reg_tmp = reg-REAL_SWCORE_BASE+SWCORE_BASE;
	WRITE_MEM32(reg_tmp,val);
}
#endif


/*
 * For IC hardware, it is different between READ and WRITE.
 * To emulate the hardware, we provide two functions.
 * Therefore, we can emulate some hardware-specific function, such as "write 1 to clear interrupt pending bit".
 */
void WRITE_MEM32(uint32 reg, uint32 val)
{
	enum IC_TYPE ictype;

	model_getTestTarget( &ictype );

	switch( ictype )
	{

		case IC_TYPE_MODEL:

			if ( 0 ) { /* to make program pretty */ }
			/* CPU Interface Control Register */
//#ifdef CONFIG_RTL865XC 
			else if ( reg==GDMACNR )
			{
				if ( ( (READ_VIR32(reg)&GDMA_ENABLE) == 0 ) && ( val & GDMA_ENABLE ) )
				{
					/* write 0, and 1 to reset GDMA */
				}
				
				WRITE_VIR32( reg, val );
				if ( val & GDMA_POLL )
				{
			//		printf("IC_TYPE_MODEL: GDMA_POLL");
					modelGdma();
				}
				else
				{
					/* Set to output 'memw ...' */
					if ( (READ_VIR32(reg)&GDMA_ENABLE) == 0 )
						modelExportComment( "Disable GDMA, enable it to reset." );
					modelExportSetOutputForm(EXPORT_RTK_SIM);
					modelExportRegister( GDMACNR );
				}
			}
			else if ( reg==GDMAISR )
			{
				/* write 1 to clear */
				WRITE_VIR32( reg, READ_VIR32(reg) & ~val );
			}	
			else
			{	/* Default */
				/*Pattern Match: PPMAR,PATP0,PATP1,PATP2,PATP3,PATP4,PATP5,MASKP0,MASKP1,MASKP2,MASKP3,MASKP4,MASKP5...*/
				WRITE_VIR32( reg, val );
			}
			break;
			
		case IC_TYPE_REAL:
//#if defined(VSV)||defined(MIILIKE)
//			/* printf("vsv_write: reg=%x ; val=%x\n",reg,val); */
//			vsv_writeMem32(conn_client, reg, val);
//#elif defined(VERA)
//			printf("write_mem32, ic_type_real: reg=%x ; val=%x\n",reg,val);
//			write2vera( (int)reg, (int)val );
//#else
			REG32(reg) = big_endian(val);
//#endif
			break;
			
		default:
			assert( 0 );
			break;
	}

}


void WRITE_MEM16(uint32 reg, uint32 val)
{
	uint32 _val;
	
	if ( ( reg & 1 ) != 0 )
	{
		rtlglue_printf( "\x1b[31;42m%s():%d unaligned 16-bit access to 0x%08x.\x1b[m\n", __FUNCTION__, __LINE__, reg );
	}

	_val = READ_MEM32( reg & 0xfffffffc );
	_val = _val & ~(0xffff<<((2-(reg&2))*8));
	_val = _val | (val<<((2-(reg&2))*8));
	WRITE_MEM32( reg & 0xfffffffc, _val );
}

void WRITE_MEM8(uint32 reg, uint32 val)
{
	uint32 _val;
	
	_val = READ_MEM32( reg & 0xfffffffc );
	_val = _val & ~(0xff<<((3-(reg&3))*8));
	_val = _val | (val<<((3-(reg&3))*8));
	WRITE_MEM32( reg & 0xfffffffc, _val );
}


/*
 *  This function translates real address 'reg' into virtual address 'reg_tmp'.
 */
void read_mem_wrapper(int reg, int* val)
{
	int reg_tmp;

	reg_tmp = reg-REAL_SWCORE_BASE+SWCORE_BASE;
	*val = READ_MEM32(reg_tmp);
}

uint32 READ_MEM32(uint32 reg)
{
	uint32 ret = 0;
	enum IC_TYPE ictype;
	uint8 msg[128];

	model_getTestTarget( &ictype );
	switch( ictype )
	{
		case IC_TYPE_MODEL:
			if ( 0 ) { /* to make program pretty */ }
			/* CPU Interface Control Register */
			else
			{	/* Default */
				/* Pattern Match: PPMAR,PATP0,PATP1,PATP2,PATP3,PATP4,PATP5,MASKP0,MASKP1,MASKP2,MASKP3,MASKP4,MASKP5... */
				ret = READ_VIR32(reg);
			}
	//		printf("function %s, line %d: reg = 0x%08x\n", __FUNCTION__, __LINE__, reg);
			break;

		case IC_TYPE_REAL:
#if defined(VSV)||defined(MIILIKE)
			{
				uint32 virAddr;

				/* We try to map 'reg' to physical address.
				 * If it is translated, it mean the 'reg' is mapped to IC domain.
				 * Therefore, we should use vsv_readMem32() to get the value.
				 * Else, it is model's domain, use REG32(). 
				 */
				virAddr = queryPhysicalToModel( reg&0x1fffffff );
				if ( virAddr!=reg )
					ret = vsv_readMem32(conn_client, reg);
				else
					ret = big_endian( REG32(reg) );
				/* printf("vsv_read: reg=%x, val=%x\n",reg,ret);  */
			}
#elif defined(VERA)
			{
				uint32 virAddr;

				printf("read_mem32, ic_type_real: reg=%x\n",reg);
				/* We try to map 'reg' to physical address.
				 * If it is translated, it mean the 'reg' is mapped to IC domain.
				 * Therefore, we should use vsv_readMem32() to get the value.
				 * Else, it is model's domain, use REG32(). 
				 */
				virAddr = queryPhysicalToModel( reg&0x1fffffff );
				if ( virAddr!=reg )
					ret = read2vera(reg);
				else
					ret = big_endian( REG32(reg) );
			}
#else
	//	printf("function %s, line %d: reg = 0x%08x\n", __FUNCTION__, __LINE__, reg);
			ret = big_endian( REG32(reg) );
#endif
			break;

		default:
			assert( 0 );
			break;
	}

	return ret;
}


uint32 READ_MEM16(uint32 reg)
{
	if ( ( reg & 1 ) != 0 )
	{
		rtlglue_printf( "\x1b[31;42m%s():%d unaligned 16-bit access to 0x%08x.\x1b[m\n", __FUNCTION__, __LINE__, reg );
	}
	
	return ( READ_MEM32( reg & 0xfffffffc ) >> ((2-(reg&0x2))*8) ) & 0xffff;
}

uint32 READ_MEM8(uint32 reg)
{
	return ( READ_MEM32( reg & 0xfffffffc ) >> ((3-(reg&0x3))*8) ) & 0xff;
}


/*
 *	For Model Code to write into virtual register space.
 *	If you want to monitor which model code touch the specified register, add your code here.
 */
void WRITE_VIR32(uint32 reg, uint32 val)
{
	enum IC_TYPE ictype;

	if ( ( reg & 3 ) != 0 )
	{
		rtlglue_printf( "\x1b[31;42m%s():%d unaligned 32-bit access to 0x%08x.\x1b[m\n", __FUNCTION__, __LINE__, reg );
	}
	
	model_getTestTarget( &ictype );
	switch( ictype )
	{
		case IC_TYPE_MODEL:
	//		printf("function %s, line %d: big_endian( REG32( 0x%08x ) = 0x%08x\n", __FUNCTION__, __LINE__, reg, big_endian( REG32( reg ) ));
			REG32( reg ) = big_endian( val );
			break;
			
		case IC_TYPE_REAL:
			/*rtlglue_printf( "\x1b[31;42m%s(0x%08x):%d In real mode, model code should be inactive.\x1b[m\n", __FUNCTION__, reg, __LINE__ );*/
			/*assert( 0 );*/
#if defined(VSV)||defined(MIILIKE)
			/*printf("vsv_write: reg=%x ; val=%x\n",reg,val);*/
			vsv_writeMem32(conn_client, reg, val);
#elif defined(VERA)
			printf("write_mem32, ic_type_real: reg=%x ; val=%x\n",reg,val);
			write2vera( (int)reg, (int)val );
#else
			//printf("function %s, line %d: big_endian( REG32( 0x%08x ) = 0x%08x\n", __FUNCTION__, __LINE__, reg, big_endian( REG32( reg ) ));
			REG32(reg) = big_endian(val);
#endif
	
			break;

		default:
			rtlglue_printf( "\x1b[31;42m%s(0x%08x):%d Unknown IC type: 0x%x.\x1b[m\n", __FUNCTION__, reg, __LINE__, ictype );
			assert( 0 );
			break;
	}
}

void WRITE_VIR16(uint32 reg, uint32 val)
{
	uint32 _val;
	
	if ( ( reg & 1 ) != 0 )
	{
		rtlglue_printf( "\x1b[31;42m%s():%d unaligned 16-bit access to 0x%08x.\x1b[m\n", __FUNCTION__, __LINE__, reg );
	}

	_val = READ_VIR32( reg & 0xfffffffc );
	_val = _val & ~(0xffff<<((2-(reg&2))*8));
	_val = _val | (val<<((2-(reg&2))*8));
	WRITE_VIR32( reg & 0xfffffffc, _val );
}

void WRITE_VIR8(uint32 reg, uint32 val)
{
	uint32 _val;
	
	_val = READ_VIR32( reg & 0xfffffffc );
	_val = _val & ~(0xff<<((3-(reg&3))*8));
	_val = _val | (val<<((3-(reg&3))*8));
	WRITE_VIR32( reg & 0xfffffffc, _val );
}


/*
 *	For Model Code to read from virtual register space.
 *	If you want to monitor which model code touch the specified register, add your code here.
 */
uint32 READ_VIR32(uint32 reg)
{
	enum IC_TYPE ictype;

	if ( ( reg & 3 ) != 0 )
	{
		rtlglue_printf( "\x1b[31;42m%s():%d unaligned 32-bit access to 0x%08x.\x1b[m\n", __FUNCTION__, __LINE__, reg );
	}
	
	model_getTestTarget( &ictype );

	switch( ictype )
	{
		case IC_TYPE_MODEL:
		//	printf("function %s, line %d: big_endian( REG32( 0x%08x ) = 0x%08x\n", __FUNCTION__, __LINE__, reg, big_endian( REG32( reg ) ));
			return big_endian( REG32( reg ) );
			
		case IC_TYPE_REAL:

#if defined(VSV)||defined(MIILIKE)
			/*printf("vsv readmem= %x \n",reg); */
			return  vsv_readMem32(conn_client, reg);
			/*printf("vsv_read: reg=%x\n",reg); */
#elif defined(VERA)
			return read2vera(reg);
#else
			return big_endian( REG32(reg) );
#endif
			break;

		default:
			assert( 0 );
			break;
	}
	rtlglue_printf( "\x1b[31;42m%s():%d unknown IC type %d when accessing 0x%08x.\x1b[m\n", __FUNCTION__, __LINE__, ictype, reg );
	return 0xDeadC0de; /* Exception !!! */
}

uint32 READ_VIR16(uint32 reg)
{
	if ( ( reg & 1 ) != 0 )
	{
		rtlglue_printf( "\x1b[31;42m%s():%d unaligned 16-bit access to 0x%08x.\x1b[m\n", __FUNCTION__, __LINE__, reg );
	}
	
	return ( READ_VIR32( reg & 0xfffffffc ) >> ((2-(reg&0x2))*8) ) & 0xffff;
}

uint32 READ_VIR8(uint32 reg)
{
	return ( READ_VIR32( reg & 0xfffffffc ) >> ((3-(reg&0x3))*8) ) & 0xff;
}

#endif

#if 0
/*
 *  Initialization sub-routine
 *  This function must be called at the first of every test case.
 *
 */
void modelIcInit( void )
{
	int i;

	/* allocate packet queue */
	{
		packet_t *pktNew;

		for( i = 0; i < sizeof(txPort)/sizeof(txPort[0]); i++ )
		{
			CTAILQ_INIT( &txPort[i] );
		}
		CTAILQ_INIT( &pktFree );
		for( i = 0; i < 128; i++ )
		{
			if ( ( pktNew = rtlglue_malloc( sizeof(*pktNew) ) ) != NULL )
			{
				memset( pktNew, 0, sizeof(*pktNew) );
				CTAILQ_INSERT_HEAD( &pktFree, pktNew, next );
			}
			else
				RTL_BUG( "pktNew allocation failed" );
		}
	}
}


/*
 *  Exit sub-toutine
 *  This function must be called at the end of every test case.
 *
 */
void modelIcExit( void )
{
	/* free allocated packet */
	{
		int i;
		packet_t *pkt;

		for( i = 0; i < sizeof(txPort)/sizeof(txPort[0]); i++ )
		{
			
			while ( ( pkt = CTAILQ_FIRST( &txPort[i] ) ) )
			{
				CTAILQ_REMOVE( &txPort[i], pkt, next );
				rtlglue_free( pkt );
			}
			CTAILQ_INIT( &txPort[i] );
		}
		
		while ( ( pkt = CTAILQ_FIRST( &pktFree ) ) )
		{
			CTAILQ_REMOVE( &pktFree, pkt, next );
			rtlglue_free( pkt );
		}
		CTAILQ_INIT( &pktFree );
	}
}


/*
 *  In the same test case, this function is used to re-chain allocated resource to free resource.
 *
 */
int32 modelIcReinit( void )
{
	/* re-chain allocated packet queue */
	{
		int i;

		for( i = 0; i < sizeof(txPort)/sizeof(txPort[0]); i++ )
		{
			packet_t *pkt;

			while ( ( pkt = CTAILQ_FIRST( &txPort[i] ) ) )
			{
				CTAILQ_REMOVE( &txPort[i], pkt, next );
				memset( pkt, 0, sizeof(*pkt) );
				CTAILQ_INSERT_HEAD( &pktFree, pkt, next );
			}
		}
	}

	return SUCCESS;
}


/*
func int32		| modelIcSetDefaultValue	| Set default value of IC registers after booted up.
parm void | |
rvalue void |
comm 
Set the default value of all registers in model code. Please key-in these default according the 865xC specification.
This function must be called after pVirtualSWReg/pVirtualSysReg/pVirtualSWTable had been allocated and 
  before ROME driver sets its default value.
*/
void modelIcSetDefaultValue( void )
{
	enum IC_TYPE orgType;

	model_getTestTarget( &orgType );
	model_setTestTarget( IC_TYPE_MODEL );

#if 0 /* Enable it when SWCORE is ready. */
	WRITE_VIR32( CHIPID, 0x8651b000 );
	WRITE_VIR32( CRMR, 0x05015788 );
#endif

	modelIcReinit();
	model_setTestTarget( orgType );
}


/*
func void		| startModel	| Starting model ( HSB --> HSA )
parm void | |
rvalue void |
comm 
This function has no parameter and return value.
Because this function will input parameter from global HSB, and generate return value into global HSA.
The decision flow is described according to Alpha's note.
*/
void modelStart( void )
{
	hsb_param_t hsb;
	hsa_param_t hsa;
	ale_data_t ale;
	int32 retval;

	MT_WATCH( "------------------------------" );
	memset((void*)&hsb,0,sizeof(hsb));
	memset((void*)&hsa,0,sizeof(hsa));
	memset((void*)&ale,0,sizeof(ale));
	
	retval = modelGetHsb( &hsb );
	assert( retval == SUCCESS );

	/* Assign HSA field from HSB */
	hsa.spao = hsb.spa;
	hsa.spcp = hsb.hiprior;
	hsa.leno = hsb.len;
	hsa.typeo=hsb.type;
	hsa.l3csoko=hsb.l3csok;
	hsa.l4csoko=hsb.l4csok;
	hsa.rxtag=hsb.tagif;
	hsa.priority = hsb.hiprior;
	hsa.dvtag=BIT9MASK;
	hsa.snapo = hsb.snap;
	
	/* Check direct TX (two cases: from CPU and to CPU) */
	if ( (hsb.dirtx==TRUE) )
	{
		if ( (hsb.spa==7)/* CPU/EXT1/EXT2/EXT3, possible: 1. CPU Direct Tx, 2. from extension port  and parse error */ )
		{
			if ( hsb.extspa==PN_PORT_CPU )
			{	/* hsb.extspa==PN_PORT_CPU, this is CPU direct Tx */
				/* copy HSB value into HSA */
				hsa.dirtxo = hsb.dirtx;
				hsa.dp = (hsb.ethtype>>4)&0x3f; /* EtherType[9:4]: PortMask [5:0] */
				hsa.dpext = 0;
				hsa.pppoeifo = (hsb.pppoeif<<1)|((hsb.ethtype>>3)&1/*hsb.ethtype[3] is DoPPPoETAG */);
				if ( hsa.pppoeifo!=HSA_PPPOE_INTACT ) hsa.l2tr = TRUE; /* If we want to modify pppoe header, we shall set l2tr high. */
				hsa.dvtag = (hsb.ethtype>>10)&0x3f; /* EtherType[15:10]: DoVLANTAG */
				hsa.l3csoko = hsb.l3csok;
				hsa.l4csoko = hsb.l4csok;
				hsa.l2tr = 0; /* alpha: always tight 0 in CPU direct tx mode. */
				hsa.l34tr = 0; /* alpha: always tight 0 in CPU direct tx mode. */
				hsa.l3csdt = hsb.sprt; /* parser has re-computed L3 checksum and stored in hsb.sprt */
				hsa.l4csdt = hsb.dprt; /* parser has re-computed L4 checksum and stored in hsb.dprt */
				goto out;
			}
			else
			{
				/* hsb.extspa == EXT1/EXT2/EXT3, this case is from extension port but parse error. Use ALE for further process. */
			}
		}
		else
		{
			/* hsb.spa == physical port, this case is from physical port but parse error. Use ALE for further process. */
		}
	}
	
	{
		/* TODO: Monitor Rx Port for Port Mirror */	
		/* DONE: DMac==IEEE 802.1D Protocol Reserved Group Address */
		MT_WATCH( "before model802_1dAddress()..." );
		CHECK_RETVAL(model802_1dAddress( &hsb, &hsa,&ale ));

		/* TODO: VLAN Ingress Check */

		MT_WATCH( "before modelIngressCheck()..." );
		CHECK_RETVAL(modelIngressCheck( &hsb, &hsa, &ale ));

		MT_WATCH( "modelSpanningTree....." );
		CHECK_RETVAL(modelSpanningTree( &hsb, &hsa,&ale));

		MT_WATCH( "modelIngress802_1X..." );
		CHECK_RETVAL(modelIngress802_1X(&hsb,&hsa,&ale));

		/* Source MAC Learning */
		/* TODO: (F)To CPU, plz ask Alpha for details */
		/* TODO: (I)Realtek private protocol frame */
		/* TODO: (B-1)Ingress ACL */
		MT_WATCH( "modelLayer2Switching.." );	
		CHECK_RETVAL(modelLayer2Switching( &hsb, &hsa,&ale ));

		/* TODO: (J)Broadcast/Multicast check */
		MT_WATCH( "modelLayer3Switching.." );
		CHECK_RETVAL(modelLayer34Switching(&hsb,&hsa,&ale));

		/* TODO: (K)Trunk processing */

		MT_WATCH( "modelEgress802_1X..." );
		CHECK_RETVAL(modelEgress802_1X(&hsb, &hsa,&ale));		

		MT_WATCH( "before modelEgressACLCheck..." );
		CHECK_RETVAL(modelEgressACLCheck(&hsb, &hsa, &ale));		

		MT_WATCH( "before modelEgressCheck..." );
		CHECK_RETVAL(modelEgressCheck( &hsb, &hsa,&ale ));

		/* TODO: Monitor Tx port for mirror port */
		/* TODO: (M)Mirror Function Process */

		goto drop;
	}

tocpu:
	MT_WATCH( "ALE decides to trap to CPU." );
		
	hsa.dp = PM_PORT_NOTPHY;
	hsa.dpext = PM_PORT_CPU;
	modelDpcCounter( &hsb, &hsa );
	goto out;

drop:

out:
	/* Write-back HSA */
	retval = modelSetHsa( &hsa );
	assert( retval == SUCCESS );

}


/*
 *  Since NIC driver initializes only once in model code,
 *    we must backup Tx/Rx pktHdr/mBuf descritpor ring register if test bench will modify them.
 */
static uint32 _FIRST_CPUTPDCR0=0, _FIRST_CPUTPDCR1=0;
static uint32 _FIRST_CPURPDCR0=0, _FIRST_CPURPDCR1=0, _FIRST_CPURPDCR2=0, _FIRST_CPURPDCR3=0, _FIRST_CPURPDCR4=0, _FIRST_CPURPDCR5=0;
static uint32 _FIRST_CPURMDCR0=0;
static uint32 _CPUTPDCR0=0, _CPUTPDCR1=0;
static uint32 _CPURPDCR0=0, _CPURPDCR1=0, _CPURPDCR2=0, _CPURPDCR3=0, _CPURPDCR4=0, _CPURPDCR5=0;
static uint32 _CPURMDCR0=0;

void modelBackupDescRegisters( void )
{
	_FIRST_CPUTPDCR0 = FIRST_CPUTPDCR0;
	_FIRST_CPUTPDCR1 = FIRST_CPUTPDCR1;
	_FIRST_CPURPDCR0 = FIRST_CPURPDCR0;
	_FIRST_CPURPDCR1 = FIRST_CPURPDCR1;
	_FIRST_CPURPDCR2 = FIRST_CPURPDCR2;
	_FIRST_CPURPDCR3 = FIRST_CPURPDCR3;
	_FIRST_CPURPDCR4 = FIRST_CPURPDCR4;
	_FIRST_CPURPDCR5 = FIRST_CPURPDCR5;
	_FIRST_CPURMDCR0 = FIRST_CPURMDCR0;

	_CPUTPDCR0 = READ_VIR32( CPUTPDCR0 );
	_CPUTPDCR1 = READ_VIR32( CPUTPDCR1 );
	_CPURPDCR0 = READ_VIR32( CPURPDCR0 );
	_CPURPDCR1 = READ_VIR32( CPURPDCR1 );
	_CPURPDCR2 = READ_VIR32( CPURPDCR2 );
	_CPURPDCR3 = READ_VIR32( CPURPDCR3 );
	_CPURPDCR4 = READ_VIR32( CPURPDCR4 );
	_CPURPDCR5 = READ_VIR32( CPURPDCR5 );
	_CPURMDCR0 = READ_VIR32( CPURMDCR0 );
}

void modelRestoreDescRegisters( void )
{
	FIRST_CPUTPDCR0 = _FIRST_CPUTPDCR0;
	FIRST_CPUTPDCR1 = _FIRST_CPUTPDCR1;
	FIRST_CPURPDCR0 = _FIRST_CPURPDCR0;
	FIRST_CPURPDCR1 = _FIRST_CPURPDCR1;
	FIRST_CPURPDCR2 = _FIRST_CPURPDCR2;
	FIRST_CPURPDCR3 = _FIRST_CPURPDCR3;
	FIRST_CPURPDCR4 = _FIRST_CPURPDCR4;
	FIRST_CPURPDCR5 = _FIRST_CPURPDCR5;
	FIRST_CPURMDCR0 = _FIRST_CPURMDCR0;

	WRITE_VIR32( CPUTPDCR0, _CPUTPDCR0 );
	WRITE_VIR32( CPUTPDCR1, _CPUTPDCR1 );
	WRITE_VIR32( CPURPDCR0, _CPURPDCR0 );
	WRITE_VIR32( CPURPDCR1, _CPURPDCR1 );
	WRITE_VIR32( CPURPDCR2, _CPURPDCR2 );
	WRITE_VIR32( CPURPDCR3, _CPURPDCR3 );
	WRITE_VIR32( CPURPDCR4, _CPURPDCR4 );
	WRITE_VIR32( CPURPDCR5, _CPURPDCR5 );
	WRITE_VIR32( CPURMDCR0, _CPURMDCR0 );
}


/*
 * #if 1 -- for endian free, all big-endian version.
 * #if 0 -- hybrid little/big-endian.
 */
#if 0
	#define RV32(a) READ_VIR32(a)
	#define WV32(a,v) WRITE_VIR32(a,v)
#else
	#define RV32(a) (*(uint32*)a)
	#define WV32(a,v) ((*(uint32*)a)=v)
#endif
/*
 *  For NIC Rx, this function returns a tuple of pktHdr-mBufs-Clusters for modelPktTranslator().
 *
 *  INPUT:
 *    enum RXPKTDESC | desc | The
 *    int32 | lenNeed | The total packet length that need
 *    struct rtl_pktHdr **| pPkt | pointer storing the pointer to returned pktHdr.
 *
 *  RVALUE:
 *    SUCCESS | Success combine a tuple of P-M-C, and the pointer of P is return in pPkt.
 *    FAILED |
 *
 *  OUTPUT:
 *    struct rtl_pktHdr **| pPkt | the pointer of P
 */
int32 modelChainPMC( enum RXPKTDESC desc, int32 lenNeed, struct rtl_pktHdr** ppPkt )
{
	struct rtl_pktHdr *pPkt;
	struct rtl_mBuf *pMbuf;
	struct rtl_mBuf *prevMbuf; /* for m_next */
	uint32 uuPkt; /* the current value of CPURPDCR? */
	uint32 uPkt; /* the pointer to pkthdr: pointer[31:2],WRAP[1:1],OWN[0:0] */
	uint32 uuMbuf; /* the current value of free mbuf */
	uint32 uMbuf; /* the pointer to mbuf: pointer[31:2],WRAP[1:1],OWN[0:0] */
	int32 lenAlloc; /* length that allocated */
	int32 nAlloc; /* the number of allocated mbuf */
	char tmp[80];

	lenAlloc = 0;
	nAlloc = 0;
	prevMbuf = NULL;

	if ( ppPkt==NULL )
	{
		MT_WATCH( "ppPkt is NULL" );
		return FAILED;
	}

	/* Get the current pkthdr descriptor */
	uuPkt = READ_VIR32( CPURPDCR0+desc*4 );
	uPkt = RV32( uuPkt );
	if ( (uPkt&DESC_OWNED_BIT)==DESC_RISC_OWNED )
	{
		snprintf( tmp, sizeof(tmp), "Rx Descriptor %d run out!!!! Model code cannot handle!", desc );
		MT_WATCH( tmp );
		WRITE_VIR32( CPUIISR, READ_VIR32(CPUIISR)|(PKTHDR_DESC_RUNOUT_IP0<<desc) );
		return FAILED;
	}
	pPkt = (struct rtl_pktHdr*)(uPkt&~(DESC_OWNED_BIT|DESC_WRAP));

	/* move current pointer to next descriptor and change OWN bit */
	WV32( uuPkt, (uPkt&~DESC_OWNED_BIT)|DESC_RISC_OWNED );
	if ( uPkt&DESC_WRAP )
	{
		switch( desc )
		{
			case RxPktDesc0:
				WRITE_VIR32( CPURPDCR0, FIRST_CPURPDCR0 );
				break;
			case RxPktDesc1:
				WRITE_VIR32( CPURPDCR1, FIRST_CPURPDCR1 );
				break;
			case RxPktDesc2:
				WRITE_VIR32( CPURPDCR2, FIRST_CPURPDCR2 );
				break;
			case RxPktDesc3:
				WRITE_VIR32( CPURPDCR3, FIRST_CPURPDCR3 );
				break;
			case RxPktDesc4:
				WRITE_VIR32( CPURPDCR4, FIRST_CPURPDCR4 );
				break;
			case RxPktDesc5:
				WRITE_VIR32( CPURPDCR5, FIRST_CPURPDCR5 );
				break;
		}
	}
	else
		WRITE_VIR32( CPURPDCR0+desc*4, uuPkt+4 );

	/* initial structure */
	pPkt->ph_mbuf = NULL; /* set later */
	pPkt->ph_len = 0;
	pPkt->ph_srcExtPortNum = 0;
	pPkt->ph_extPortList = 0;
	pPkt->ph_queueId = 0;
	pPkt->ph_type = 0;
	pPkt->ph_vlanTagged = 0;
	pPkt->ph_LLCTagged = 0;
	pPkt->ph_pppeTagged = 0;
	pPkt->ph_pppoeIdx = 0;
	pPkt->ph_linkID = 0;
	pPkt->ph_reason = 0;
	pPkt->ph_reason = 0;
	pPkt->ph_flags = 0;
	pPkt->ph_orgtos = 0;
	pPkt->ph_portlist = 0;
	pPkt->ph_vlanId = 0;
	pPkt->ph_flags2 = 0;


#if 0
	/* Get the current mbuf descriptor */
	while( lenAlloc < lenNeed )
	{
		uuMbuf = READ_VIR32( CPURMDCR0 );
		uMbuf = RV32( uuMbuf );
		if ( (uMbuf&DESC_OWNED_BIT)==DESC_RISC_OWNED )
		{
		rtlglue_printf("Rx Mbuf run out!!!! Model code cannot handle!\n" );
			snprintf( tmp, sizeof(tmp), "Rx Mbuf run out!!!! Model code cannot handle!" );
			MT_WATCH( tmp );
			WRITE_VIR32( CPUIISR, READ_VIR32(CPUIISR)|MBUF_DESC_RUNOUT_IP0 );
			return FAILED;
		}
		
		pMbuf = (struct rtl_mBuf*)(uMbuf&~(DESC_OWNED_BIT|DESC_WRAP));

		/* move current pointer to next descriptor and change OWN bit */
		WV32( uuMbuf, (uMbuf&~DESC_OWNED_BIT)|DESC_RISC_OWNED );
		
		if ( uMbuf&DESC_WRAP )
			WRITE_VIR32( CPURMDCR0, FIRST_CPURMDCR0 );
		else
			WRITE_VIR32( CPURMDCR0, uuMbuf+4 );

		/* initial structure */
		if ( nAlloc==0 ) pPkt->ph_mbuf = pMbuf; /* if this is the first mbuf, set pPkt->ph_mbuf here. */
		if ( prevMbuf!=NULL ) prevMbuf->m_next = pMbuf;
		pMbuf->m_next = NULL;
		pMbuf->m_pkthdr = pPkt;
		/* pMbuf->m_data is initialized by driver. */
		/* pMbuf->m_extsize is initialized by driver. */
		/* pMbuf->m_extbuf is initialized by driver. */
		/* pMbuf->m_len will be filled by modelPktTranslator() */
		pMbuf->m_flags = 0;

		if ( nAlloc==0 )
			lenAlloc += pMbuf->m_extsize-(pMbuf->m_data-pMbuf->m_extbuf); /* the first mbuf */
		else
			lenAlloc += pMbuf->m_extsize; /* the later mbufs */
		nAlloc++;
		prevMbuf = pMbuf;
	}

#else /* new mbuf arch */

	/* Get the current mbuf descriptor */
	while( lenAlloc < lenNeed )
	{
		uint32 uuStartMbuf=READ_VIR32( CPURMDCR0 );
		while(1)
		{
			uuMbuf = READ_VIR32( CPURMDCR0 );
			uMbuf = RV32( uuMbuf );
		
			if ( (uMbuf&DESC_OWNED_BIT)!=DESC_RISC_OWNED ) break;

			if ( uMbuf&DESC_WRAP )
			{
				WRITE_VIR32( CPURMDCR0, FIRST_CPURMDCR0 );
			}
			else
			{
				WRITE_VIR32( CPURMDCR0, uuMbuf+4 );			
			}
			
			uuMbuf = READ_VIR32( CPURMDCR0 );
			if(uuMbuf==uuStartMbuf)
			{
				rtlglue_printf("Rx Mbuf run out!!!! Model code cannot handle!\n" );				
				snprintf( tmp, sizeof(tmp), "Rx Mbuf run out!!!! Model code cannot handle!" );
				MT_WATCH( tmp );			
				WRITE_VIR32( CPUIISR, READ_VIR32(CPUIISR)|MBUF_DESC_RUNOUT_IP0 );
				return FAILED;
			}			
			
		}
		
		pMbuf = (struct rtl_mBuf*)(uMbuf&~(DESC_OWNED_BIT|DESC_WRAP));

		/* move current pointer to next descriptor and change OWN bit */
		WV32( uuMbuf, (uMbuf&~DESC_OWNED_BIT)|DESC_RISC_OWNED );
		
		if ( uMbuf&DESC_WRAP )
			WRITE_VIR32( CPURMDCR0, FIRST_CPURMDCR0 );
		else
			WRITE_VIR32( CPURMDCR0, uuMbuf+4 );

		/* initial structure */
		if ( nAlloc==0 ) pPkt->ph_mbuf = pMbuf; /* if this is the first mbuf, set pPkt->ph_mbuf here. */
		if ( prevMbuf!=NULL ) prevMbuf->m_next = pMbuf;
		pMbuf->m_next = NULL;
		pMbuf->m_pkthdr = pPkt;
		/* pMbuf->m_data is initialized by driver. */
		/* pMbuf->m_extsize is initialized by driver. */
		/* pMbuf->m_extbuf is initialized by driver. */
		/* pMbuf->m_len will be filled by modelPktTranslator() */
		pMbuf->m_hwflags_reserved2 = 0;
		/* pMbuf->m_flags is USED by SW : model CAN NOT modify it : It is different between IC spec and the definition in mbuf.h . */

		if ( nAlloc==0 )
			lenAlloc += pMbuf->m_extsize-(pMbuf->m_data-pMbuf->m_extbuf); /* the first mbuf */
		else
			lenAlloc += pMbuf->m_extsize; /* the later mbufs */
		nAlloc++;
		prevMbuf = pMbuf;
	}

#endif 

	*ppPkt = pPkt;
	snprintf( tmp, sizeof(tmp), "Allocated one pkthdr (%p) and %d mbufs", pPkt, nAlloc );
	MT_WATCH( tmp );
	return SUCCESS;
}


/* Since in kernel mode, max() will cause compiler error, we define max function by ourself. */
#define MYMAX( a, b ) ((a>b)?a:b)

/*
func int32		| modelPktTranslator	| Request packet translator to modify the given packet with HSA.
parm uint8* | packet | Packet content
parm int32 | _len_ | the length of packet content (included L2 CRC), however, no use in current. we use hsa.leno instead.
rvalue SUCCESS | 
rvalue FAILED | The given parameter is wrong.
comm 
This function will model the packet translator of ASIC.
1. It will modify packet content according to HSA.
2. Append the modified packet into per port queue (according packet scheduling mechanism) 
*/
int32 modelPktTranslator( uint8* packet, int32 _len_ )
{
	hsa_param_t hsa;
	int32 ip_offset;
	uint32 ethTypePos;
	uint16 sessionId;
	rtl865x_tblAsicDrv_pppoeParam_t asicPppoe;
	int32 retval;
	enum _PORT /* Locally use only */
	{
		_PORT0 = PN_PORT0,
		_PORT1 = PN_PORT1,
		_PORT2 = PN_PORT2,
		_PORT3 = PN_PORT3,
		_PORT4 = PN_PORT4,
		_PORT5 = PN_PORT5,
		_NOTPHY = PN_PORT_NOTPHY,
		_EXT0 = PN_PORT_EXT0+6,
		_EXT1 = PN_PORT_EXT1+6,
		_EXT2 = PN_PORT_EXT2+6,
		_CPU = PN_PORT_CPU+6,
	} port;

	MT_WATCH( "------------------------------" );

	/* check given parameter */
	if ( packet==NULL )
	{
		MT_WATCH( "packet==NULL" );
		return FAILED;
	}
#if 0 /* we use hsa.leno instead of _len_. */
	if ( _len_ <= 0 )
	{
		MT_WATCH( "_len_<=0" );
		return FAILED;
	}
#endif

	retval = modelGetHsa( &hsa );
	assert( retval == SUCCESS );

#if 0 /* In fact, we can reuse the code of Mac Tx to physical port */
	if ( hsa.dirtxo )
	{
		/* This is a CPU direct Tx packet */
		packet_t *pkt;
		
		MT_WATCH( "CPU direct Tx Packet" );

		for( port = _PORT0; port <= _PORT5; port++ )
		{
			if ( (hsa.dp&(1<<port)) )
			{
				pkt = CTAILQ_FIRST( &pktFree );
				if ( pkt == NULL )
					RTL_BUG( "no more free packet avaliable" );

				memset( pkt, 0, offsetof(packet_t, next) );
				pkt->len = hsa.leno;
				
				/* This packet will be forwarded to physical port. */
				CTAILQ_REMOVE( &pktFree, pkt, next );
				CTAILQ_INSERT_TAIL( &txPort[port], pkt, next );
			}
		}
	}
	else
#endif
	{
		/* to send packet to associated port */
		int bIncludedCpuPort; /* The destination port includes CPU port, then we must pass original packet to extension port. */
		int bMultiExtPort; /* The destination port includes multiple extport. */ 
		int bPacketHasSendToCpu; /* For CPU and EXT ports, we only recv one packet even if it is a multicast packet. */
		int bCpuDirectTx; /* TRUE if this packet is CPU direct Tx */

		bPacketHasSendToCpu = FALSE; /* In default, this packet has not send to CPU port. */
		if ( (hsa.dp&(1<<_NOTPHY)) && (hsa.dpext&(1<<(_CPU-_EXT0))) )
			bIncludedCpuPort = TRUE;
		else
			bIncludedCpuPort = FALSE;

		/* decide if multiple EXT ports. */
		if ( (hsa.dp&(1<<_NOTPHY)) &&
		     ( ( (hsa.dpext&(1<<(_EXT0-_EXT0)))&&(hsa.dpext&(1<<(_EXT1-_EXT0))) ) ||
		       ( (hsa.dpext&(1<<(_EXT1-_EXT0)))&&(hsa.dpext&(1<<(_EXT2-_EXT0))) ) ||
		       ( (hsa.dpext&(1<<(_EXT0-_EXT0)))&&(hsa.dpext&(1<<(_EXT2-_EXT0))) ) ) )
			bMultiExtPort = TRUE;
		else
			bMultiExtPort = FALSE;

		if ( hsa.spao==_CPU )
		{
			assert(hsa.dirtxo==TRUE);
			bCpuDirectTx = TRUE;
		}
		else
			bCpuDirectTx = FALSE;

		/* Not direct Tx, modify packet ! */
		for( port = _PORT0; port <= _CPU; port++ )
		{
			/* Check if this port is the destination port */
			if ( ((port<=_PORT5)&&(hsa.dp&(1<<port))) ||
			     ((port>=_EXT0)&&(hsa.dp&(1<<_NOTPHY))&&(hsa.dpext&(1<<(port-_EXT0)))) )
			{
				packet_t *pkt;
				struct ether_header *ether = NULL;
				struct ip *ip=NULL;
				struct tcphdr *tc=NULL;
				struct udphdr *ud=NULL;	
				struct icmp *icmp=NULL;
				struct igmp *igmp=NULL;
				struct grehdr *gre=NULL;
				pppoeHdr_t *pppoe = NULL;
				vlanHdr_t *vlan = NULL;
				uint8 szComment[128];
				int bAbleToModifyPacket; /* TRUE if translator can modify the packet. FALSE for destgination ext port includes CPU. */
				int lenIpHeader; /* length of IP header length */
				uint32 txUserPriority; /* decided by HSA.priority and HSA.spcp */
				int bUpdateL3CS; /* if need to update L3 csum */
				int bUpdateL4CS; /* if need to update L4 csum */
				enum EOPLAYER
				{
					EOPL2 = 2,
					EOPL3,
					EOPL4,
				} maxOpLayer; /* the maximum operation layer: default: 2-layer2 */

				/*  Procedure:
				 *  + Copy Original Packet to each port
				 *  + Set ether/ip/tcp/udp pointer
				 *  + l2tr/l34tr
				 *  + TTL-1
				 *  + PPPoE tag/untag/modify
				 *  + VLAN tag/untag/modify
				 *  + L2 Padding
				 *  + L2 CRC Recaculation
				 *  + Enqueue to each port
				 */
				
				if ( port<=_PORT5 ) sprintf( szComment, "Packet queue to PHY%d", port );
				else if ( port<=_EXT2 ) sprintf( szComment, "Packet queue to EXT%d", port-6 );
				else sprintf( szComment, "Packet queue to CPU" );
				MT_WATCH( szComment );

				/* If this packet is destinated to the any one of the following cases:
				 * 1. CPU 
				 * 2. CPU AND ( EXT0 or EXT1 or EXT2 )
				 * 3. Multiple EXT ports,
				 * we must keep the packet originally to CPU.
				 */
				if ( port <= _PORT5 )
					bAbleToModifyPacket = TRUE; /* physical port */
				else /* EXT0, EXT1, EXT2, CPU */
				{
					if ( bIncludedCpuPort==TRUE || bMultiExtPort==TRUE )
						bAbleToModifyPacket = FALSE;
					else
						bAbleToModifyPacket = TRUE;
				}

				/* Initial */
				maxOpLayer = EOPL2;

				pkt = CTAILQ_FIRST( &pktFree );
				if ( pkt == NULL )
					RTL_BUG( "no more free packet avaliable" );

				memset( pkt, 0, offsetof(packet_t, next) );
				pkt->len = hsa.leno;
				
				ethTypePos = 12;
				ip_offset = 14;

				ether = (struct ether_header*)&pkt->pkt[0];
				
				if ( hsa.snapo )
				{ /* LLC present */
					ethTypePos += 8;
					ip_offset += 8;
				}

				if ( hsa.rxtag )
				{ /* VLAN tag present */
					ethTypePos += 4;
					ip_offset += 4;

#if 1 /* We assume VLAN tag is in the front of LLC/SNAP. */
					vlan = (vlanHdr_t*)(ether+1);
#else
					if ( hsa.snapo )
						vlan = (vlanHdr_t*)(((uint8*)(ether+1))+8/*sizeof(llc)*/);
					else
						vlan = (vlanHdr_t*)(ether+1);
#endif
				}

				if ( hsa.pppoeifo==HSA_PPPOE_REMOVE_TAG || hsa.pppoeifo==HSA_PPPOE_MODIFY )
				{ /* PPPOE Header present */
					ethTypePos += 8;
					ip_offset += 8;

					if ( vlan )
						pppoe = (pppoeHdr_t*)(vlan+1);
					else
					{
						if ( hsa.snapo )
							pppoe = (pppoeHdr_t*)(((uint8*)(ether+1))+8/*sizeof(llc)*/);
						else
							pppoe = (pppoeHdr_t*)(ether+1);
					}
				}

				rtl8651_getAsicPppoe( hsa.pppidx, &asicPppoe );
				sessionId = asicPppoe.sessionId;

				/* Copy whole original packet first */
				assert( sizeof(pkt->pkt)!= sizeof(void*) ); /* to prevent programmer change pkt->pkt from array to pointer. */
				assert( hsa.leno <= sizeof(pkt->pkt) ); /* To prevent copy over buffer length */
				memcpy( &pkt->pkt[0], packet, hsa.leno );

				switch ( hsa.typeo )
				{
					case HSA_TYPE_ETHERNET:
						break;
					case HSA_TYPE_PPTP:
						ip = (struct ip*)(&pkt->pkt[ip_offset]);
						lenIpHeader = ((ip->ip_vhl)&0x0f)*4;
						gre = (struct grehdr*)(((uint8*)ip)+lenIpHeader);
						break;
					case HSA_TYPE_IP:
						ip = (struct ip*)(&pkt->pkt[ip_offset]);
						break;
					case HSA_TYPE_ICMP:
						ip = (struct ip*)(&pkt->pkt[ip_offset]);
						lenIpHeader = ((ip->ip_vhl)&0x0f)*4;
						icmp = (struct icmp*)(((uint8*)ip)+lenIpHeader);
						break;
					case HSA_TYPE_IGMP:
						ip = (struct ip*)(&pkt->pkt[ip_offset]);
						lenIpHeader = ((ip->ip_vhl)&0x0f)*4;
						igmp = (struct igmp*)(((uint8*)ip)+lenIpHeader);
						break;
					case HSA_TYPE_TCP:
						ip = (struct ip*)(&pkt->pkt[ip_offset]);
						lenIpHeader = ((ip->ip_vhl)&0x0f)*4;
						tc = (struct tcphdr*)(((uint8*)ip)+lenIpHeader);
						break;
					case HSA_TYPE_UDP:
						ip = (struct ip*)(&pkt->pkt[ip_offset]);
						lenIpHeader = ((ip->ip_vhl)&0x0f)*4;
						ud = (struct udphdr*)(((uint8*)ip)+lenIpHeader);
						break;
					case HSA_TYPE_IPV6:
						break;
				}

				/* Does DMAC need to change ? */
				if ( (hsa.l2tr==TRUE) && 
				     (bAbleToModifyPacket==TRUE) &&
				     (hsa.ttl_1if&(1<<port)) )
				{
					/* nhmac */
					sprintf( szComment, "L2 Translate packet (DMAC translated to %02x:%02x:%02x:%02x:%02x:%02x)", hsa.nhmac[0], hsa.nhmac[1], hsa.nhmac[2], hsa.nhmac[3], hsa.nhmac[4], hsa.nhmac[5] );
					MT_WATCH( szComment );
					memcpy( ether->ether_dhost, hsa.nhmac, sizeof(ether->ether_dhost) );

					maxOpLayer = MYMAX( maxOpLayer, EOPL3 );
				}
				
				/* Does SMAC need to change ? */
				if ( (hsa.l2tr==TRUE) && 
				     (bAbleToModifyPacket==TRUE) &&
				     (hsa.ttl_1if&(1<<port)) )
				{
					rtl865x_tblAsicDrv_intfParam_t intf;
					
					retval = rtl8651_getAsicNetInterface( hsa.difid, &intf );
					assert( retval==SUCCESS );
					sprintf( szComment, "L2 Translate packet (SMAC translated to %02x:%02x:%02x:%02x:%02x:%02x) hsa.difid=%d", intf.macAddr.octet[0], intf.macAddr.octet[1], intf.macAddr.octet[2], intf.macAddr.octet[3], intf.macAddr.octet[4], intf.macAddr.octet[5], hsa.difid );
					MT_WATCH( szComment );
					memcpy( ether->ether_shost, &intf.macAddr, sizeof(ether->ether_shost) );
					maxOpLayer = MYMAX( maxOpLayer, EOPL3 );
				}

				/* Decrease TTL -- Only physical ports and extension ports (no CPU port) */
				if ( (port <= _EXT2) &&
				     (hsa.l2tr==TRUE) && 
				     (bAbleToModifyPacket==TRUE) &&
				     (hsa.ttl_1if&(1<<port)) &&
				     (READ_VIR32(ALECR)&EN_TTL1) &&
				     (hsa.typeo!=HSA_TYPE_ETHERNET) &&
				     (hsa.typeo!=HSA_TYPE_IPV6) )
				{
					MT_WATCH( "TTL-1" );
					ip->ip_ttl--; /* Physical Port and Extension Port. We note: CPU port always gets original packet. */
					ip->ip_sum = htons(OCADD(ntohs(ip->ip_sum),0x0100)); /* Since TTL has been minus one, we must plus one in checksum to balance. */
					maxOpLayer = MYMAX( maxOpLayer, EOPL3 );
				}

				/* decide if L3/L4 checksum need to recalculate? */
				bUpdateL3CS = FALSE;
				bUpdateL4CS = FALSE;
				if ( (bAbleToModifyPacket==TRUE)&&
				     ( ( hsa.dirtxo==TRUE ) /* 1. CPU directTx, always update checksum */ ||
				       ( hsa.dirtxo==FALSE&&hsa.l34tr==TRUE /* 2. not direct Tx, l34 needs to translate */ ) ) )
				{
					if ( hsa.ipmcastr )
					{	/* Multicast packet */
						/* We translate source IP of multicast packet only when :
						 *   1. TTL-1 is set
						 *   2. from internal to external
						 */
						if ( (hsa.ttl_1if&(1<<port)) &&
						     ((READ_VIR32(SWTCR0)&(1<<(hsa.spao+MultiPortModeP_OFFSET)))==0/*Internal*/) && 
						     ((READ_VIR32(SWTCR0)&(1<<(port+MultiPortModeP_OFFSET)))!=0/*External*/) )
						{
							bUpdateL3CS = hsa.l3csoko;
							bUpdateL4CS = hsa.l4csoko;
							sprintf( szComment, "Multicast: bUpdateL3CS=%d bUpdateL4CS=%d", bUpdateL3CS, bUpdateL4CS );
							MT_WATCH( szComment );
						}
						else
						{	/* SIP is kept, therefore, keep checksum */
							bUpdateL3CS = FALSE;
							bUpdateL4CS = FALSE;
							sprintf( szComment, "keep SIP: bUpdateL3CS=%d bUpdateL4CS=%d", bUpdateL3CS, bUpdateL4CS );
							MT_WATCH( szComment );
						}
					}
					else
					{	/* non-multicast packet */
						bUpdateL3CS = hsa.l3csoko;
						bUpdateL4CS = hsa.l4csoko;
						sprintf( szComment, "non-multicast: bUpdateL3CS=%d bUpdateL4CS=%d", bUpdateL3CS, bUpdateL4CS );
						MT_WATCH( szComment );
					}
				}
				
				/* Deal with L3/L4 header */
				if ( hsa.l34tr && bAbleToModifyPacket==TRUE )
				{
					uint16 orgL4Port;
					
					MT_WATCH( "L34 Translate packet" );

					assert( hsa.typeo!=HSA_TYPE_ETHERNET && hsa.typeo!=HSA_TYPE_IPV6 ); /* We cannot handle L34 header of these types */
					assert( ip!=NULL );
					
					/* trip */
					if ( hsa.ipmcastr )
					{	/* Multicast packet */
						/* We translate source IP of multicast packet only when :
						 *   1. TTL-1 is set
						 *   2. from internal to external
						 */
						if ( (hsa.ttl_1if&(1<<port)) &&
						     ((READ_VIR32(SWTCR0)&(1<<(hsa.spao+MultiPortModeP_OFFSET)))==0/*Internal*/) && 
						     ((READ_VIR32(SWTCR0)&(1<<(port+MultiPortModeP_OFFSET)))!=0/*External*/) )
						{
#if 0 /* l3csdt is computed by ALE, we don't need compute. */
							uint32 l3csdt;
#endif

							ip->ip_src.s_addr = htonl(hsa.trip); /* outbound packet */
#if 0 /* l3csdt is computed by ALE, we don't need compute. */
							l3csdt = hsa.trip - ntohl(ip->ip_src.s_addr);
							l3csdt = OCADD( l3csdt>>16, l3csdt&0xffff );
							ip->ip_sum = htons( OCADD( ntohs(ip->ip_sum), l3csdt ) );
#endif
							sprintf( szComment, "Multicast from internal to external, new srcIp="NIPQUAD2STR", newIpSum=0x%04x",
							                    NIPQUAD2(ntohl(ip->ip_src.s_addr)), ntohs(ip->ip_sum) );
							MT_WATCH( szComment );
							maxOpLayer = MYMAX( maxOpLayer, EOPL4 );
						}
						else
						{	/* keep SIP */
						}

					}
					else
					{	/* non-multicast packet */
						if ( hsa.egif )
						{
#ifdef _LITTLE_ENDIAN
							ip->ip_src.s_addr = htonl(hsa.trip); /* outbound packet */
							sprintf( szComment, "new srcIp="NIPQUAD2STR, NIPQUAD2(ntohl(ip->ip_src.s_addr)) );
#else
							/* This is big endian, said, target platform. Therefore, we must handle alignment issue. */
							((uint8*)&ip->ip_src.s_addr)[0] = htonl(hsa.trip)>>24;
							((uint8*)&ip->ip_src.s_addr)[1] = htonl(hsa.trip)>>16;
							((uint8*)&ip->ip_src.s_addr)[2] = htonl(hsa.trip)>> 8;
							((uint8*)&ip->ip_src.s_addr)[3] = htonl(hsa.trip)>> 0;
							sprintf( szComment, "new srcIp="NIPQUAD2STR, NIPQUAD2((hsa.trip)) );
#endif
							MT_WATCH( szComment );
							maxOpLayer = MYMAX( maxOpLayer, EOPL4 );
						}
						else
						{
#ifdef _LITTLE_ENDIAN
							ip->ip_dst.s_addr = htonl(hsa.trip); /* inbound packet */
							sprintf( szComment, "new dstIp="NIPQUAD2STR, NIPQUAD2(ntohl(ip->ip_dst.s_addr)) );
#else
							/* This is big endian, said, target platform. Therefore, we must handle alignment issue. */
							((uint8*)&ip->ip_dst.s_addr)[0] = htonl(hsa.trip)>>24;
							((uint8*)&ip->ip_dst.s_addr)[1] = htonl(hsa.trip)>>16;
							((uint8*)&ip->ip_dst.s_addr)[2] = htonl(hsa.trip)>> 8;
							((uint8*)&ip->ip_dst.s_addr)[3] = htonl(hsa.trip)>> 0;
							sprintf( szComment, "new dstIp="NIPQUAD2STR, NIPQUAD2((hsa.trip)) );
#endif
							MT_WATCH( szComment );
							maxOpLayer = MYMAX( maxOpLayer, EOPL4 );
						}
					}

					/* L4 port */
					if ( hsa.ipmcastr==TRUE )
					{
						/* If this is a multicast packet, we never translate L4 port/id. */
					}
					else
					{
						switch ( hsa.typeo )
						{
							case HSA_TYPE_PPTP:
								orgL4Port = ntohs(gre->gh_call_id);
								gre->gh_call_id = htons(hsa.port); /* no matter inbound or outbound, change call id anyway */
								sprintf( szComment, "PPTP ID changed from 0x%04x to %04x", orgL4Port, hsa.port );
								MT_WATCH( szComment );
								maxOpLayer = MYMAX( maxOpLayer, EOPL4 );
								break;
								
							case HSA_TYPE_ICMP:
								orgL4Port = ntohs(icmp->icmp_id);
								icmp->icmp_id = htons(hsa.port); /* no matter inbound or outbound, change icmp id anyway */
								sprintf( szComment, "ICMP ID changed from 0x%04x to 0x%04x", orgL4Port, hsa.port );
								MT_WATCH( szComment );
								maxOpLayer = MYMAX( maxOpLayer, EOPL4 );
								break;
								
							case HSA_TYPE_TCP:
								if ( hsa.egif )
								{
									orgL4Port = ntohs( tc->th_sport );
									tc->th_sport = htons(hsa.port); /* outbound packet */
									sprintf( szComment, "TCP sport changed from 0x%04x to 0x%04x", orgL4Port, hsa.port );
									MT_WATCH( szComment );
								}
								else
								{
									orgL4Port = ntohs( tc->th_dport );
									tc->th_dport = htons(hsa.port); /* inbound packet */
									sprintf( szComment, "TCP dport changed from 0x%04x to 0x%04x", orgL4Port, hsa.port );
									MT_WATCH( szComment );
								}
								maxOpLayer = MYMAX( maxOpLayer, EOPL4 );
								break;
								
							case HSA_TYPE_UDP:
								if ( hsa.egif )
								{
									orgL4Port = ntohs( ud->uh_sport );
									ud->uh_sport = htons(hsa.port); /* outbound packet */
									sprintf( szComment, "UDP sport changed from 0x%04x to 0x%04x", orgL4Port, hsa.port );
									MT_WATCH( szComment );
								}
								else
								{
									orgL4Port = ntohs( ud->uh_dport );
									ud->uh_dport = htons(hsa.port); /* inbound packet */
									sprintf( szComment, "UDP dport changed from 0x%04x to 0x%04x", orgL4Port, hsa.port );
									MT_WATCH( szComment );
								}
								maxOpLayer = MYMAX( maxOpLayer, EOPL4 );
								break;

							case HSA_TYPE_IP:
								break;

							default:
								RTL_BUG( "Never Come here!" );
						}
					}
				}
				
				sprintf( szComment, "maxOpLayer=L%d  1P(L2:%s, L3:%s, L4:%s),OPMask:%s DSCP(L2:%s, L3:%s, L4:%s),OPMask:%s", maxOpLayer,
				                                                                 READ_VIR32(RLRC)&RMLC_8021P_L2?"Y":"N",
				                                                                 READ_VIR32(RLRC)&RMLC_8021P_L3?"Y":"N",
				                                                                 READ_VIR32(RLRC)&RMLC_8021P_L4?"Y":"N",
				                                                                 ((READ_VIR32(RMCR1P)>>RM1P_EN_OFFSET) | ((READ_VIR32(DSCPRM0)&RM1P_EN_8)?(1<<8):0))&(1<<port)?"Y":"N",
				                                                                 READ_VIR32(RLRC)&RMLC_DSCP_L2?"Y":"N",
				                                                                 READ_VIR32(RLRC)&RMLC_DSCP_L3?"Y":"N",
				                                                                 READ_VIR32(RLRC)&RMLC_DSCP_L4?"Y":"N",
				                                                                 READ_VIR32(DSCPRM1)&(1<<(DSCPRM_EN_OFFSET+port))?"Y":"N" );
				MT_WATCH( szComment );
				
				/* Decide Tx User Priority */
				if ( bCpuDirectTx==FALSE )
				{ 
					uint32 rm_en1p; /* continuous 9 bits for every port */

					rm_en1p = (READ_VIR32(RMCR1P)>>RM1P_EN_OFFSET) | ((READ_VIR32(DSCPRM0)&RM1P_EN_8)?(1<<8):0);
					if ( (rm_en1p&(1<<port)) &&
					     (READ_VIR32(RLRC)&(RMLC_8021P_L2<<(maxOpLayer-EOPL2))) )
						txUserPriority = (READ_VIR32(RMCR1P)>>(RM0_1P_OFFSET+3*hsa.priority))&RM0_1P_MASK;
					else
						txUserPriority = hsa.spcp;
				}
				else
					txUserPriority = 8; /* invalid value */

				sprintf( szComment, "decided txUserPriority=%d", txUserPriority );
				MT_WATCH( szComment );

				/* Continue L34 header */
				if ( (hsa.typeo!=HSA_TYPE_ETHERNET) &&
				     (hsa.typeo!=HSA_TYPE_IPV6) &&
				     (bAbleToModifyPacket==TRUE) )
				{
					/* DSCP Remarking */
					if ( (bCpuDirectTx==FALSE) &&
					     (READ_VIR32(RLRC)&(RMLC_DSCP_L2<<(maxOpLayer-EOPL2))) )
					{
						if ( READ_VIR32(DSCPRM1)&(1<<(DSCPRM_EN_OFFSET+port)) )
						{
							uint32 reg, off;
							uint32 newDSCP;
							uint32 orgTos;
							uint32 l3csdt;
							uint32 orgL3cs;

							assert(hsa.priority<=7);
							assert(ip!=NULL);
							reg = DSCPRM0+(hsa.priority/5)*4;
							off = (hsa.priority%5)*6;
							newDSCP = (READ_VIR32( reg )>>off) & DSCPRM0_MASK;
							orgTos = ip->ip_tos;
							ip->ip_tos = (ip->ip_tos&0x3)|(newDSCP<<2); /* DSCP field in IP Header is [7:2]. keep ECN[1:0] field. */

							/* recalculate L3 checksum */
							orgL3cs = ntohs(ip->ip_sum);
							l3csdt = OCSUB( ip->ip_tos, orgTos );
							ip->ip_sum = htons( OCSUB( ntohs(ip->ip_sum), l3csdt ) );
							sprintf( szComment, "DSCP remarking: orgTos=0x%02x newTos=0x%02x hsa.priority=%d (csum:0x%04x-->0x%04x, l3csdt=0x%04x)", orgTos, ip->ip_tos, hsa.priority, orgL3cs, ntohs(ip->ip_sum), l3csdt );
							MT_WATCH( szComment );
						}
					}

					/* l3csdt */
					if ( bUpdateL3CS==TRUE )
					{
						uint16 orgIpSum;
						
						assert( ip!=NULL );
						orgIpSum = ntohs(ip->ip_sum);
						ip->ip_sum = htons( OCADD( ntohs(ip->ip_sum), hsa.l3csdt ) );
						sprintf( szComment, "new IP ChkSum=0x%04x (org:0x%04x)", ntohs(ip->ip_sum), orgIpSum );
						MT_WATCH( szComment );
					}

					/* l4csdt */
					if ( bUpdateL4CS==TRUE )
					{
						uint16 orgL4CS;

						if ( hsa.typeo==HSA_TYPE_TCP )
						{
							assert( tc!=NULL );
							orgL4CS = ntohs( tc->th_sum );
							tc->th_sum = htons( OCADD( ntohs(tc->th_sum), hsa.l4csdt ) );
							sprintf( szComment, "update TCP ChkSum from 0x%04x to 0x%04x", orgL4CS, ntohs(tc->th_sum) );
							MT_WATCH( szComment );
						}
						else if ( hsa.typeo==HSA_TYPE_UDP )
						{
							assert( ud!=NULL );
							orgL4CS = ntohs( ud->uh_sum );
							ud->uh_sum = htons( OCADD( ntohs(ud->uh_sum), hsa.l4csdt ) );
							sprintf( szComment, "update UDP ChkSum from 0x%04x to 0x%04x", orgL4CS, ntohs(ud->uh_sum) );
							MT_WATCH( szComment );
						}
					}
				}
				
				/* In above, all modification about L3/L4 has been done. */
				/*========================================================*/
				/* In below, we will modify packet length. The ip/tcp/udp...
				 * pointers will be invalid. */
				if ( (bAbleToModifyPacket==TRUE) &&
				     ( ( hsa.dirtxo==TRUE ) /* 1. CPU directTx, according to hsa.pppoeifo */ ||
				       ( hsa.dirtxo==FALSE&&hsa.l2tr==TRUE /* 2. not direct Tx, l34 needs to translate */ ) ) )
				{
					switch ( hsa.pppoeifo )
					{
						case HSA_PPPOE_INTACT: /* intact, do nothing. */
							break;

						case HSA_PPPOE_TAGGING: /* tagging */
						{
							if ( hsa.typeo==HSA_TYPE_ETHERNET )
							{
								MT_WATCH( "Do not tag PPPoE because hsa.typeo==HSA_TYPE_ETHERNET"  );
							}
							else
							{
								if ( vlan )
								{
									if ( hsa.snapo )
									{
										pppoe = (pppoeHdr_t*)(((uint8*)(vlan+1))+8/*sizeof(llc)*/);
										ethTypePos += 8; /* sizeof(pppoeHdr_t) */
										memmove( pppoe+1, pppoe, pkt->len - ip_offset ); /* move back IP header and its payload with 8 bytes (for PPPoE header) */
									}
									else
									{
										pppoe = (pppoeHdr_t*)(vlan+1);
										vlan->ether_type = htons(0x8864); /* PPPoE data */
										ethTypePos += 8; /* sizeof(pppoeHdr_t) */
										memmove( pppoe+1, vlan+1, pkt->len - ip_offset ); /* move back IP header and its payload with 8 bytes (for PPPoE header) */
									}
								}
								else
								{
									if ( hsa.snapo )
									{
										pppoe = (pppoeHdr_t*)(((uint8*)(ether+1))+8/*sizeof(llc)*/);
										ethTypePos += 8; /* sizeof(pppoeHdr_t) */
										memmove( pppoe+1, pppoe, pkt->len - ip_offset ); /* move back IP header and its payload with 8 bytes (for PPPoE header) */
									}
									else
									{
										pppoe = (pppoeHdr_t*)(ether+1);
										ether->ether_type = htons(0x8864); /* PPPoE data */
										ethTypePos += 8; /* sizeof(pppoeHdr_t) */
										memmove( pppoe+1, ether+1, pkt->len - ip_offset ); /* move back IP header and its payload with 8 bytes (for PPPoE header) */
									}
								}
								pppoe->ver = 1;
								pppoe->type = 1;
								pppoe->code = 0x00;
								pppoe->sessionId = htons(sessionId);
								pppoe->length = htons(pkt->len-ip_offset+2/*PPP*/-4/*CRC*/);
								if ( hsa.typeo==HSA_TYPE_IPV6 )
									pppoe->proto = htons(0x0057/*IPv6*/);
								else
									pppoe->proto = htons(0x0021/*IP*/);
								pkt->len += 8/*sizeof(pppoeHdr_t)*/;

								sprintf( szComment, "Tagging PPPoE(session:0x%04x, len=%d, type=0x%04x), pkt->len=%d", ntohs(pppoe->sessionId), ntohs(pppoe->length), ntohs(hsa.typeo), pkt->len );
								MT_WATCH( szComment );
							}
							break;
						}
						case HSA_PPPOE_REMOVE_TAG: /* remove */
						{
							assert( pppoe!=NULL );
							if ( vlan )
							{
								if ( hsa.snapo )
								{
									memmove( pppoe, pppoe+1, pkt->len - 14/* sizeof(ether_header) */ - 8/*sizeof(pppoeHdr_t)*/-4/*CRC*/ ); /* move forward IP header and its payload with 8 bytes (strip PPPoE header) */
								}
								else
								{
									assert( vlan->ether_type == htons(0x8864) /* PPPoE data */ );
									memmove( vlan+1, pppoe+1, pkt->len - 14/* sizeof(ether_header) */ - 8/*sizeof(pppoeHdr_t)*/-4/*CRC*/ ); /* move forward IP header and its payload with 8 bytes (strip PPPoE header) */
									vlan->ether_type = htons(0x0800); /* IP */
								}
								ethTypePos -= 8/*sizeof(pppoeHdr_t)*/;
								pkt->len -= 8/*sizeof(pppoeHdr_t)*/;
							}
							else
							{
								if ( hsa.snapo )
								{
									memmove( pppoe, pppoe+1, pkt->len - 14/* sizeof(ether_header) */ - 8/*sizeof(pppoeHdr_t)*/-4/*CRC*/ ); /* move forward IP header and its payload with 8 bytes (strip PPPoE header) */
								}
								else
								{
									assert( ether->ether_type == htons(0x8864) /* PPPoE data */ );
									memmove( ether+1, pppoe+1, pkt->len - 14/* sizeof(ether_header) */ - 8/*sizeof(pppoeHdr_t)*/-4/*CRC*/ ); /* move forward IP header and its payload with 8 bytes (strip PPPoE header) */
									ether->ether_type = htons(0x0800); /* IP */
								}
								ethTypePos -= 8/*sizeof(pppoeHdr_t)*/;
								pkt->len -= 8/*sizeof(pppoeHdr_t)*/;
							}
							pppoe = NULL;
							
							sprintf( szComment, "After removing PPPoE, len=%d bytes", pkt->len );
							MT_WATCH( szComment );
							break;
						}
						case HSA_PPPOE_MODIFY: /* modify */
						{
							if ( hsa.typeo==HSA_TYPE_ETHERNET )
							{
								MT_WATCH( "Remove PPPoE only (do not add) because hsa.typeo==HSA_TYPE_ETHERNET"  );
								assert( pppoe!=NULL );
								if ( vlan )
								{
									if ( hsa.snapo )
									{
										memmove( pppoe, pppoe+1, pkt->len - 14/* sizeof(ether_header) */ - 8/*sizeof(pppoeHdr_t)*/-4/*CRC*/ ); /* move forward IP header and its payload with 8 bytes (strip PPPoE header) */
									}
									else
									{
										assert( vlan->ether_type == htons(0x8864) /* PPPoE data */ );
										memmove( vlan+1, pppoe+1, pkt->len - 14/* sizeof(ether_header) */ - 8/*sizeof(pppoeHdr_t)*/-4/*CRC*/ ); /* move forward IP header and its payload with 8 bytes (strip PPPoE header) */
										vlan->ether_type = htons(0x0800); /* IP */
									}
									ethTypePos -= 8/*sizeof(pppoeHdr_t)*/;
									pkt->len -= 8/*sizeof(pppoeHdr_t)*/;
								}
								else
								{
									if ( hsa.snapo )
									{
										memmove( pppoe, pppoe+1, pkt->len - 14/* sizeof(ether_header) */ - 8/*sizeof(pppoeHdr_t)*/-4/*CRC*/ ); /* move forward IP header and its payload with 8 bytes (strip PPPoE header) */
									}
									else
									{
										assert( ether->ether_type == htons(0x8864) /* PPPoE data */ );
										memmove( ether+1, pppoe+1, pkt->len - 14/* sizeof(ether_header) */ - 8/*sizeof(pppoeHdr_t)*/-4/*CRC*/ ); /* move forward IP header and its payload with 8 bytes (strip PPPoE header) */
										ether->ether_type = htons(0x0800); /* IP */
									}
									ethTypePos -= 8/*sizeof(pppoeHdr_t)*/;
									pkt->len -= 8/*sizeof(pppoeHdr_t)*/;
								}
								pppoe = NULL;
								
								sprintf( szComment, "After removing PPPoE, len=%d bytes", pkt->len );
								MT_WATCH( szComment );
							}
							else
							{
								uint16 orgSession;
								uint16 orgPppoeLen;

								assert( pppoe!=NULL );
								if ( hsa.rxtag==TRUE )
								{
									vlan = (vlanHdr_t*)(ether+1);
									if ( hsa.snapo )
										pppoe = (pppoeHdr_t*)(((uint8*)(vlan+1))+8/*sizeof(llc)*/);
									else
										pppoe = (pppoeHdr_t*)(vlan+1);
								}
								else
								{
									if ( hsa.snapo )
										pppoe = (pppoeHdr_t*)(((uint8*)(ether+1))+8/*sizeof(llc)*/);
									else
										pppoe = (pppoeHdr_t*)(ether+1);
								}
								orgSession = ntohs( pppoe->sessionId );
								pppoe->sessionId = htons(sessionId);
								orgPppoeLen = ntohs(pppoe->length);
								pppoe->length = htons(pkt->len-ip_offset+2/*PPP*/-4/*CRC*/);

								sprintf( szComment, "Modify PPPoE Session from 0x%04x to 0x%04x, pppoe->len from %d to %d", orgSession, sessionId, orgPppoeLen, ntohs(pppoe->length) );
								MT_WATCH( szComment );
							}
							break;
						}
					}
				}

				/* Tag VLAN tag according to destination port,only physical ports and extension ports (no CPU port) */
				if ( port <= _EXT2 && bAbleToModifyPacket==TRUE  )
				{
					if ( hsa.rxtag==1 && (hsa.dvtag&(1<<port))==0 )
					{
						/* remove VLAN Tag */
						uint16 orgType;

						assert( ntohs(ether->ether_type)==0x8100/*VLAN*/ );
						assert( vlan!=NULL );
						orgType = ntohs( vlan->ether_type );
						memmove( vlan, vlan+1, pkt->len-14/*sizeof(ether_header)*/-4/*sizeof(vlanHdr_t)*/-4/*CRC*/ );
						ether->ether_type = htons( orgType );
						pkt->len -= 4/*sizeof(vlanHdr_t)*/;

						sprintf( szComment, "After removing VLAN tag, len=%d bytes, recover ethtype=0x%04x", pkt->len, orgType );
						MT_WATCH( szComment );
					}
					else if ( hsa.rxtag==0 && (hsa.dvtag&(1<<port)) )
					{
						/* insert VLAN Tag */

						vlan = (vlanHdr_t*)(ether+1);
						memmove( vlan+1, vlan, pkt->len-14/*sizeof(ether_header)*/ );
						vlan->priority = txUserPriority;
						vlan->cfi = 0;
						vlan->vidh = hsa.dvid>>8;
						vlan->vidl = hsa.dvid&0xff;
						vlan->ether_type = ether->ether_type;
						ether->ether_type = htons( 0x8100/*VLAN*/ );
						pkt->len += 4/*sizeof(vlanHdr_t)*/;

						sprintf( szComment, "Inserting VLAN tag (prio=%d, vid=%d, ethtype=0x%04x), pkt->len=%d bytes", vlan->priority, hsa.dvid, ntohs(vlan->ether_type), pkt->len );
						MT_WATCH( szComment );
					}
					else if ( hsa.rxtag==1 && (hsa.dvtag&(1<<port)) )
					{
						/* modify VLAN tag */
						uint32 orgPriority;
						uint32 orgDvid;

						vlan = (vlanHdr_t*)(ether+1);
						orgPriority = vlan->priority;
						orgDvid = ((vlan->vidh<<8)|vlan->vidl);
						vlan->priority = txUserPriority;
						vlan->vidh = hsa.dvid>>8;
						vlan->vidl = hsa.dvid&0xff;
						
						sprintf( szComment, "Modifying VLAN tag (prio from %d to %d, vid from %d to %d", orgPriority, vlan->priority, orgDvid, hsa.dvid );
						MT_WATCH( szComment );
					}
					else /* hsa.rxtag==0 && (hsa.dvtag&(1<<i))==0 */
					{
						/* no change */
					}
				}

				/* Consider the untag VLAN case and removing PPPoE header case. A packet may be shorter than Ethernet standard. */
				/* Padding packet to 64 bytes, if destinated to physical ports and extension ports (no CPU port). */
				if ( port<=_EXT2 && bAbleToModifyPacket==TRUE )
				{
					if ( pkt->len < 64 )
					{
						int startPad;

						startPad = pkt->len-4;
						sprintf( szComment, "L2 pad %d bytes from offset %d", 64-startPad, startPad );
						MT_WATCH( szComment );
						memset( &pkt->pkt[startPad], 0x20/*padding byte*/, 64-startPad );
						pkt->len = 64;
					}
				}

				/* Recalculate L2 CRC, only physical ports and extension ports (no CPU port) */
				if ( port<=_EXT2 && bAbleToModifyPacket==TRUE )
				{
					pkt->len -= 4/*CRC*/;
					assert( pkt->len>0 );
					sprintf( szComment, "Recalculate L2 CRC, reduce length to %d bytes", pkt->len );
					MT_WATCH( szComment );

					if ( (READ_VIR32(PCRP0+port*4)&BYPASS_TCRC)==0 )
					{
						/* L2 CRC is behind the packet content, after the pkt->len bytes. */
						l2FormCrc( 0/*flags*/, pkt->pkt, pkt->len );
						sprintf( szComment, "Recalculate L2 CRC (new:%02x-%02x-%02x-%02x)",
						                    pkt->pkt[pkt->len+0],
						                    pkt->pkt[pkt->len+1],
						                    pkt->pkt[pkt->len+2],
						                    pkt->pkt[pkt->len+3] );
						MT_WATCH( szComment );
					}

					pkt->len += 4/*CRC*/;
					sprintf( szComment, "Recalculate L2 CRC, recover length to %d bytes", pkt->len );
					MT_WATCH( szComment );
				}

				/*
				 * Final Step!!! Queue packet to corresponding port.
				 */
				if ( port>=_EXT0 )
				{
					/* This packet will be trapped to CPU port and extension ports. */
					if ( bPacketHasSendToCpu==TRUE )
					{
						/* Packet has queued to CPU, no need to queue again. */
					}
					else
					{
						enum RXPKTDESC desc;
						uint32 qidMap;
						uint32 qid;
						uint32 cpuqdm;
						struct rtl_pktHdr *pPkt;
						struct rtl_mBuf *pMbuf;
						int nMbuf; /* number of mbuf copied */
						int lenCopied; /* the total length that had copied */

						if ( bIncludedCpuPort==TRUE )
						{
							/* include CPU */
							qidMap = READ_VIR32( CPUQIDMCR0 );
						}
						else if ( bMultiExtPort==TRUE )
						{
							/* multiple EXT ports */
							qidMap = READ_VIR32( CPUQIDMCR4 );
						}
						else
						{
							/* Not include CPU, and single EXT port */
							switch( port )
							{
								case _EXT0:
									qidMap = READ_VIR32( CPUQIDMCR1 );
									break;
								case _EXT1:
									qidMap = READ_VIR32( CPUQIDMCR2 );
									break;
								case _EXT2:
									qidMap = READ_VIR32( CPUQIDMCR3 );
									break;
								default:
									RTL_BUG( "Never come here !" );
									break;
							}
						}
						qid = (qidMap>>(hsa.priority*4)) & 0x7;
						assert( qid<=5 );

						if ( bMultiExtPort==TRUE ||
						     bIncludedCpuPort==TRUE )
						{
							/* Oops! In this case, we must refer to CPU port's descriptor setting. */
							cpuqdm = READ_VIR16( CPUQDM0+ qid*2 );
							desc = (cpuqdm&CPURxDesc_MASK)>>CPURxDesc_OFFSET;
						}
						else
						{
							cpuqdm = READ_VIR16( CPUQDM0+ qid*2 );
							switch ( port )
							{
								case _CPU:
									desc = (cpuqdm&CPURxDesc_MASK)>>CPURxDesc_OFFSET;
									break;
								case _EXT0:
									desc = (cpuqdm&Ext1RxDesc_MASK)>>Ext1RxDesc_OFFSET;
									break;
								case _EXT1:
									desc = (cpuqdm&Ext2RxDesc_MASK)>>Ext2RxDesc_OFFSET;
									break;
								case _EXT2:
									desc = (cpuqdm&Ext3RxDesc_MASK)>>Ext3RxDesc_OFFSET;
									break;
								default:
									RTL_BUG( "Never come here !" );
									break;
							}
						}

						/* Ask a pkthdr */
						retval = modelChainPMC( desc, hsa.leno, &pPkt );
						if ( retval == SUCCESS )
						{
							assert( pPkt!=NULL );
							assert( pPkt->ph_mbuf!=NULL );

							/* move data to SDRAM */
							nMbuf = 0;
							lenCopied = 0;
							pMbuf = pPkt->ph_mbuf;

							while( pMbuf )
							{
								uint8* startCopy; /* starting address to copy to */
								int lenMdata; /* m_data length */
								int lenCopy; /* length to copy */
								
								if ( nMbuf==0 )
								{ /* first mbuf */
									startCopy = pMbuf->m_data;
									lenMdata = pMbuf->m_extsize-(pMbuf->m_data-pMbuf->m_extbuf);
								}
								else
								{ /* later mbuf */
									startCopy = pMbuf->m_extbuf;
									lenMdata = pMbuf->m_extsize;
									pMbuf->m_data = pMbuf->m_extbuf;
								}

								lenCopy = min( lenMdata, hsa.leno-lenCopied );
								memcpy( startCopy, &pkt->pkt[lenCopied], lenCopy );
								pMbuf->m_len = lenCopy;
								lenCopied += lenCopy;

								nMbuf++;
								pMbuf = pMbuf->m_next;
							}

							pPkt->ph_len = lenCopied;
							pPkt->ph_queueId = qid;
							pPkt->ph_extPortList = hsa.dpext;
							pPkt->ph_hwFwd = hsa.hwfwrd;
							pPkt->ph_isOriginal = (bIncludedCpuPort==TRUE||bMultiExtPort==TRUE)?TRUE:FALSE;
							pPkt->ph_l2Trans = hsa.l2tr;
							pPkt->ph_srcExtPortNum = hsa.spaext;
							pPkt->ph_type = hsa.typeo;
							pPkt->ph_reason = hsa.why2cpu;
							pPkt->ph_flags = PKTHDR_USED; /* if flags have PKTHDR_DRIVERHOLD , will become rx runout. */
							pPkt->ph_orgtos = hsa.siptos;
							pPkt->ph_portlist = hsa.spao;
							pPkt->ph_vlanId = (hsa.priority<<12)|(hsa.dvid);
							pPkt->ph_flags2 = (hsa.hwfwrd<<15)|(hsa.spcp<<12)|(hsa.svid);

							/* Set interrupt of NIC RxDone */
							WRITE_VIR32( CPUIISR, READ_VIR32(CPUIISR)|(RX_DONE_IP0<<desc) );
							bPacketHasSendToCpu = TRUE; /* For multiple-extport/CPU, we only queue one packet to CPU. */

							sprintf( szComment, "A packet queued to %s(qid=%d) ==> desc=%d", port==_CPU?"CPU":port==_EXT0?"EXT0":port==_EXT1?"EXT1":"EXT2",
							                    qid, desc );
							MT_WATCH( szComment );
						}
						else
						{
							sprintf( szComment, "no avaliable p-m-c. give up." );
							MT_DROP( szComment );
						}
					}
				}
				else
				{
					/* This packet will be forwarded to physical port. */
					CTAILQ_REMOVE( &pktFree, pkt, next );
					CTAILQ_INSERT_TAIL( &txPort[port], pkt, next );
				}
			}
		}
	}


#if 0  /* FIXME */
{
	uint32 hwfwrd:1;    /*        Hardware forward flag. ( S/W use ) */
	uint32 spcp:3;      /* [2:0] Source priority code point */
	uint32 linkid:7;    /* [6:0] WLAN link ID; if is valid only if the packets are from extension ports0: this field is invalid. */
	uint32 siptos:8;    /* [7:0] Source IPToS for those packets which were delivered to extension ports. */
}
#endif

	return SUCCESS;
}


/************************** [ virtualMac Function ] **************************/


int32 rtl865xC_convertHsbToAsic( hsb_param_t* hsb, hsb_t* rawHsb )
{
	/* bit-to-bit mapping */
	rawHsb->spa = hsb->spa;
	rawHsb->trigpkt = hsb->trigpkt;
	rawHsb->resv = 0;
	rawHsb->len = hsb->len;
	rawHsb->vid = hsb->vid;
	rawHsb->tagif = hsb->tagif;
	rawHsb->pppoeif = hsb->pppoeif;
	rawHsb->sip29_0 = hsb->sip&0x3fffffff;
	rawHsb->sip31_30 = hsb->sip>>30;
	rawHsb->sprt = hsb->sprt;
	rawHsb->dip13_0 = hsb->dip&0x3fff;
	rawHsb->dip31_14 = hsb->dip>>14;
	rawHsb->dprt13_0 = hsb->dprt&0x3fff;
	rawHsb->dprt15_14 = hsb->dprt>>14;
	rawHsb->ipptl = hsb->ipptl;
	rawHsb->ipfg = hsb->ipfg;
	rawHsb->iptos = hsb->iptos;
	rawHsb->tcpfg = hsb->tcpfg;
	rawHsb->type = hsb->type;
	rawHsb->patmatch = hsb->patmatch;
	rawHsb->ethtype = hsb->ethtype;
#if 1 /* Since the endian is reversed, we must translate it. */
	rawHsb->da14_0 = hsb->da[5]|(hsb->da[4]<<8);
	rawHsb->da46_15 = (hsb->da[4]>>7)|(hsb->da[3]<<1)|(hsb->da[2]<<9)|(hsb->da[1]<<17)|(hsb->da[0]<<25);
	rawHsb->da47_47 = hsb->da[0]>>7;
	rawHsb->sa30_0 = hsb->sa[5]|(hsb->sa[4]<<8)|(hsb->sa[3]<<16)|(hsb->sa[2]<<24);
	rawHsb->sa47_31 = (hsb->sa[2]>>7)|(hsb->sa[1]<<1)|(hsb->sa[0]<<9);
#else
	rawHsb->da14_0 = hsb->da[0]|(hsb->da[1]<<8);
	rawHsb->da46_15 = (hsb->da[1]>>7)|(hsb->da[2]<<1)|(hsb->da[3]<<9)|(hsb->da[4]<<17)|(hsb->da[5]<<25);
	rawHsb->da47_47 = hsb->da[5]>>7;
	rawHsb->sa30_0 = hsb->sa[0]|(hsb->sa[1]<<8)|(hsb->sa[2]<<16)|(hsb->sa[3]<<24);
	rawHsb->sa47_31 = (hsb->sa[3]>>7)|(hsb->sa[4]<<1)|(hsb->sa[5]<<9);
#endif
	rawHsb->hiprior = hsb->hiprior;
	rawHsb->snap = hsb->snap;
	rawHsb->udpnocs = hsb->udpnocs;
	rawHsb->ttlst = hsb->ttlst;
	rawHsb->dirtx = hsb->dirtx;
	rawHsb->l3csok = hsb->l3csok;
	rawHsb->l4csok = hsb->l4csok;
	rawHsb->ipfo0_n = hsb->ipfo0_n;
	rawHsb->llcothr = hsb->llcothr;
	rawHsb->urlmch = hsb->urlmch;
	rawHsb->extspa = hsb->extspa;
	rawHsb->extl2 = hsb->extl2;
	rawHsb->linkid = hsb->linkid;
	rawHsb->pppoeid = hsb->pppoeid;
	return SUCCESS;
}


int32 rtl865xC_virtualMacSetHsb( hsb_t rawHsb )
{
	int32 retval = SUCCESS;
	
	{ /* Word-Access */
		uint32 *pSrc, *pDst;
		uint32 i;

		/* We must assert structure size is the times of 4-bytes. */
		if ( (sizeof(rawHsb)%4) != 0 ) RTL_BUG( "sizeof(rawHsb) is not the times of 4-bytes." );

		pSrc = (uint32*)&rawHsb;
		pDst = (uint32*)HSB_BASE;
		for( i = 0; i < sizeof(rawHsb); i+=4 )
		{
			WRITE_MEM32((uint32)pDst, (*pSrc));
			pSrc++;
			pDst++;
		}
	}

	{
		/* Before packet enters into model code, we shall dump swcore-related information for debug. */
		WRITE_MEM32( TMCR, READ_MEM32( TMCR) | ENHSBTESTMODE ); /* Before reading HSABUSY, we must enable test mode. */
		modelExportSetOutputForm( EXPORT_RTK_SIM );
		modelExportSwcoreRegistersAndTables();
		modelExportComment( "Export HSB for simulation" );
		modelExportMemory( HSB_BASE, HSB_SIZE );

		WRITE_MEM32( TMCR, READ_MEM32( TMCR ) & ~HSB_RDY );
		WRITE_MEM32( TMCR, READ_MEM32( TMCR ) | HSB_RDY ); /* Once HSB_RDY is set, FPGA and model code start ALE. */

		modelExportComment( "Export TMCR to kick off HSB_RDY" );
		modelExportRegister( TMCR );
		modelExportNop( 2000 ); /* delay for ALE to process. */
		
		while( (READ_MEM32(TMCR)&HSABUSY) == HSABUSY ); /* Wait until HSB has became HAS. */

		/* After packet handled, we shall dump expected information for debug. */
		modelExportSetOutputForm( EXPORT_RTK_EXPECT );
		modelExportComment( "Export HSA for verify" );
		modelExportMemory( HSA_BASE, HSA_SIZE );
		modelExportSwcoreRegistersAndTables();
		modelExportComment( "-=-=-=-  Next Packet -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-" ); /* End of export, next packet! */
	}
	
	return retval;
}


int32 rtl865xC_convertHsbToSoftware( hsb_t* rawHsb, hsb_param_t* hsb )
{
	/* bit-to-bit mapping */
	hsb->spa = rawHsb->spa;
	hsb->trigpkt = rawHsb->trigpkt;
 	hsb->len = rawHsb->len;
	hsb->vid = rawHsb->vid;
	hsb->tagif = rawHsb->tagif;
	hsb->pppoeif = rawHsb->pppoeif;
	hsb->sip = rawHsb->sip29_0 | (rawHsb->sip31_30<<30);
	hsb->sprt = rawHsb->sprt;
	hsb->dip = rawHsb->dip13_0 | (rawHsb->dip31_14<<14);
	hsb->dprt = rawHsb->dprt13_0 | (rawHsb->dprt15_14<<14);
	hsb->ipptl = rawHsb->ipptl;
	hsb->ipfg = rawHsb->ipfg;
	hsb->iptos = rawHsb->iptos;
	hsb->tcpfg = rawHsb->tcpfg;
	hsb->type = rawHsb->type;
	hsb->patmatch = rawHsb->patmatch;
	hsb->ethtype = rawHsb->ethtype;
#if 1 /* Since the endian is reversed, we must translate it. */
	hsb->da[5] = rawHsb->da14_0;
	hsb->da[4] = (rawHsb->da14_0>>8) | (rawHsb->da46_15<<7);
	hsb->da[3] = rawHsb->da46_15>>1;
	hsb->da[2] = rawHsb->da46_15>>9;
	hsb->da[1] = rawHsb->da46_15>>17;
	hsb->da[0] = (rawHsb->da46_15>>25) | (rawHsb->da47_47<<7);
	hsb->sa[5] = rawHsb->sa30_0;
	hsb->sa[4] = rawHsb->sa30_0>>8;
	hsb->sa[3] = rawHsb->sa30_0>>16;
	hsb->sa[2] = (rawHsb->sa30_0>>24) | (rawHsb->sa47_31<<7);
	hsb->sa[1] = rawHsb->sa47_31>>1;
	hsb->sa[0] = rawHsb->sa47_31>>9;
#else
	hsb->da[0] = rawHsb->da14_0;
	hsb->da[1] = (rawHsb->da14_0>>8) | (rawHsb->da46_15<<7);
	hsb->da[2] = rawHsb->da46_15>>1;
	hsb->da[3] = rawHsb->da46_15>>9;
	hsb->da[4] = rawHsb->da46_15>>17;
	hsb->da[5] = (rawHsb->da46_15>>25) | (rawHsb->da47_47<<7);
	hsb->sa[0] = rawHsb->sa30_0;
	hsb->sa[1] = rawHsb->sa30_0>>8;
	hsb->sa[2] = rawHsb->sa30_0>>16;
	hsb->sa[3] = (rawHsb->sa30_0>>24) | (rawHsb->sa47_31<<7);
	hsb->sa[4] = rawHsb->sa47_31>>1;
	hsb->sa[5] = rawHsb->sa47_31>>9;
#endif
	hsb->hiprior = rawHsb->hiprior;
	hsb->snap = rawHsb->snap;
	hsb->udpnocs = rawHsb->udpnocs;
	hsb->ttlst = rawHsb->ttlst;
	hsb->dirtx = rawHsb->dirtx;
	hsb->l3csok = rawHsb->l3csok;
	hsb->l4csok = rawHsb->l4csok;
	hsb->ipfo0_n = rawHsb->ipfo0_n;
	hsb->llcothr = rawHsb->llcothr;
	hsb->urlmch = rawHsb->urlmch;
	hsb->extspa = rawHsb->extspa;
	hsb->extl2 = rawHsb->extl2;
	hsb->linkid = rawHsb->linkid;
	hsb->pppoeid = rawHsb->pppoeid;
	return SUCCESS;
}


int32 rtl865xC_virtualMacGetHsb( hsb_t* rawHsb )
{
	int32 retval = SUCCESS;
	
	{ /* Word-Access */
		uint32 *pSrc, *pDst;
		uint32 i;

		/* We must assert structure size is the times of 4-bytes. */
		if ( (sizeof(*rawHsb)%4) != 0 ) RTL_BUG( "sizeof(rawHsb) is not the times of 4-bytes." );

		pSrc = (uint32*)HSB_BASE;
		pDst = (uint32*)rawHsb;
		for( i = 0; i < sizeof(*rawHsb); i+=4 )
		{
			*pDst = READ_MEM32((uint32)pSrc);
			pSrc++;
			pDst++;
		}
	}

	return retval;
}


int32 rtl865xC_convertHsaToAsic( hsa_param_t* hsa, hsa_t* rawHsa )
{
	/* bit-to-bit mapping */
#if 1 /* Since the endian is reversed, we must translate it. */
	rawHsa->nhmac0 = hsa->nhmac[5];
	rawHsa->nhmac1 = hsa->nhmac[4];
	rawHsa->nhmac2 = hsa->nhmac[3];
	rawHsa->nhmac3 = hsa->nhmac[2];
	rawHsa->nhmac4 = hsa->nhmac[1];
	rawHsa->nhmac5 = hsa->nhmac[0];
#else
	rawHsa->nhmac0 = hsa->nhmac[0];
	rawHsa->nhmac1 = hsa->nhmac[1];
	rawHsa->nhmac2 = hsa->nhmac[2];
	rawHsa->nhmac3 = hsa->nhmac[3];
	rawHsa->nhmac4 = hsa->nhmac[4];
	rawHsa->nhmac5 = hsa->nhmac[5];
#endif
	rawHsa->trip15_0 = hsa->trip&0xffff;
	rawHsa->trip31_16 = hsa->trip>>16;
	rawHsa->port = hsa->port;
	rawHsa->l3csdt = hsa->l3csdt;
	rawHsa->l4csdt = hsa->l4csdt;
	rawHsa->egif = hsa->egif;
	rawHsa->l2tr = hsa->l2tr;
	rawHsa->l34tr = hsa->l34tr;
	rawHsa->dirtxo = hsa->dirtxo;
	rawHsa->typeo = hsa->typeo;
	rawHsa->snapo = hsa->snapo;
	rawHsa->rxtag = hsa->rxtag;
	rawHsa->dvid = hsa->dvid;
	rawHsa->pppoeifo = hsa->pppoeifo;
	rawHsa->pppidx = hsa->pppidx;
	rawHsa->leno5_0 = hsa->leno&0x3f;
	rawHsa->leno14_6 = hsa->leno>>6;
	rawHsa->l3csoko = hsa->l3csoko;
	rawHsa->l4csoko = hsa->l4csoko;
	rawHsa->frag = hsa->frag;
	rawHsa->lastfrag = hsa->lastfrag;
	rawHsa->ipmcastr = hsa->ipmcastr;
	rawHsa->svid = hsa->svid;
	rawHsa->fragpkt = hsa->fragpkt;
	rawHsa->ttl_1if4_0 = hsa->ttl_1if&0x1f;
	rawHsa->ttl_1if5_5 = hsa->ttl_1if>>5;
	rawHsa->ttl_1if8_6 = hsa->ttl_1if>>6;
	rawHsa->dpc = hsa->dpc;
	rawHsa->spao = hsa->spao;
	rawHsa->hwfwrd = hsa->hwfwrd;
	rawHsa->dpext = hsa->dpext;
	rawHsa->spaext = hsa->spaext;
	rawHsa->why2cpu13_0 = hsa->why2cpu&0x3fff;
	rawHsa->why2cpu15_14 = hsa->why2cpu>>14;
	rawHsa->spcp = hsa->spcp;
	rawHsa->dvtag = hsa->dvtag;
	rawHsa->difid = hsa->difid;
	rawHsa->linkid = hsa->linkid;
	rawHsa->siptos = hsa->siptos;
	rawHsa->dp6_0 = hsa->dp;
	rawHsa->priority = hsa->priority;
	return SUCCESS;
}


int32 rtl865xC_virtualMacSetHsa( hsa_t rawHsa )
{
	int32 retval = SUCCESS;
	enum IC_TYPE curIcType;
	
	model_getTestTarget(&curIcType);
	
	switch ( curIcType )
	{
		case IC_TYPE_MODEL:
		{
			{ /* Word-Access */
				uint32 *pSrc, *pDst;
				uint32 i;

				/* We must assert structure size is the times of 4-bytes. */
				if ( (sizeof(rawHsa)%4) != 0 ) RTL_BUG( "sizeof(rawHsa) is not the times of 4-bytes." );

				pSrc = (uint32*)&rawHsa;
				pDst = (uint32*)HSA_BASE;
				for( i = 0; i < sizeof(rawHsa); i+=4 )
				{
					WRITE_MEM32((uint32)pDst, (*pSrc));
					pSrc++;
					pDst++;
				}
			}
			return retval;
		}
		case IC_TYPE_REAL:
			return VMAC_ERROR_NON_AVAILABLE;
		default:
			return FAILED;
	}
}


int32 rtl865xC_convertHsaToSoftware( hsa_t* rawHsa, hsa_param_t* hsa )
{
	/* bit-to-bit mapping */
#if 1 /* Since the endian is reversed, we must translate it. */
	hsa->nhmac[5] = rawHsa->nhmac0;
	hsa->nhmac[4] = rawHsa->nhmac1;
	hsa->nhmac[3] = rawHsa->nhmac2;
	hsa->nhmac[2] = rawHsa->nhmac3;
	hsa->nhmac[1] = rawHsa->nhmac4;
	hsa->nhmac[0] = rawHsa->nhmac5;
#else
	hsa->nhmac[0] = rawHsa->nhmac0;
	hsa->nhmac[1] = rawHsa->nhmac1;
	hsa->nhmac[2] = rawHsa->nhmac2;
	hsa->nhmac[3] = rawHsa->nhmac3;
	hsa->nhmac[4] = rawHsa->nhmac4;
	hsa->nhmac[5] = rawHsa->nhmac5;
#endif
	hsa->trip = rawHsa->trip15_0 | (rawHsa->trip31_16<<16);
	hsa->port = rawHsa->port;
	hsa->l3csdt = rawHsa->l3csdt;
	hsa->l4csdt = rawHsa->l4csdt;
	hsa->egif = rawHsa->egif;
	hsa->l2tr = rawHsa->l2tr;
	hsa->l34tr = rawHsa->l34tr;
	hsa->dirtxo = rawHsa->dirtxo;
	hsa->typeo = rawHsa->typeo;
	hsa->snapo = rawHsa->snapo;
	hsa->rxtag = rawHsa->rxtag;
	hsa->dvid = rawHsa->dvid;
	hsa->pppoeifo = rawHsa->pppoeifo;
	hsa->pppidx = rawHsa->pppidx;
	hsa->leno = rawHsa->leno5_0|(rawHsa->leno14_6<<6);
	hsa->l3csoko = rawHsa->l3csoko;
	hsa->l4csoko = rawHsa->l4csoko;
	hsa->frag = rawHsa->frag;
	hsa->lastfrag = rawHsa->lastfrag;
	hsa->ipmcastr = rawHsa->ipmcastr;
	hsa->svid = rawHsa->svid;
	hsa->fragpkt = rawHsa->fragpkt;
	hsa->ttl_1if = rawHsa->ttl_1if4_0|(rawHsa->ttl_1if5_5<<5)|(rawHsa->ttl_1if8_6<<6);
	hsa->dpc = rawHsa->dpc;
	hsa->spao = rawHsa->spao;
	hsa->hwfwrd = rawHsa->hwfwrd;
	hsa->dpext = rawHsa->dpext;
	hsa->spaext = rawHsa->spaext;
	hsa->why2cpu = rawHsa->why2cpu13_0|(rawHsa->why2cpu15_14<<14);
	hsa->spcp = rawHsa->spcp;
	hsa->dvtag = rawHsa->dvtag;
	hsa->difid = rawHsa->difid;
	hsa->linkid = rawHsa->linkid;
	hsa->siptos = rawHsa->siptos;
	hsa->dp = rawHsa->dp6_0;
	hsa->priority = rawHsa->priority;
	return SUCCESS;
}


int32 rtl865xC_virtualMacGetHsa( hsa_t* rawHsa )
{
	int32 retval = SUCCESS;

	{ /* Word-Access */
		uint32 *pSrc, *pDst;
		uint32 i;

		/* We must assert structure size is the times of 4-bytes. */
		if ( (sizeof(*rawHsa)%4) != 0 ) RTL_BUG( "sizeof(rawHsa) is not the times of 4-bytes." );

		pSrc = (uint32*)HSA_BASE;
		pDst = (uint32*)rawHsa;
		for( i = 0; i < sizeof(*rawHsa); i+=4 )
		{
			*pDst = READ_MEM32((uint32)pSrc);
			pSrc++;
			pDst++;
		}
	}

	return retval;
}


int32 rtl865xC_virtualMacInit( void )
{
	enum IC_TYPE curIcType;

	model_getTestTarget(&curIcType);

#if defined(VSV)
	switch ( curIcType )
	{
		case IC_TYPE_MODEL:
		{
			WRITE_MEM32( TMCR, READ_MEM32(TMCR)|(0x3f<<RX_TEST_PORT_OFFSET));
			WRITE_MEM32( TMCR, READ_MEM32(TMCR)|(0x3f<<TX_TEST_PORT_OFFSET));
			break;
		}
		case IC_TYPE_REAL:
		{
			enum PORT_NUM port;
			uint32 val;
			uint32 forcePortRate;
			uint32 wantedPortRate;

#if defined(GIGAIO)
			forcePortRate = ForceSpeed1000M;
			wantedPortRate = PortStatusLinkSpeed1000M;
#else
			forcePortRate = ForceSpeed100M;
			wantedPortRate = PortStatusLinkSpeed100M;
#endif

			/* Before using VSV, we must configure the following registers. */
			WRITE_MEM32(TMCR, READ_MEM32(TMCR)&~((0x3f<<RX_TEST_PORT_OFFSET)|(0x3f<<TX_TEST_PORT_OFFSET)));
#if defined(GIGAIO)
			WRITE_MEM32(PITCR, Port5_TypeCfg_GMII_MII_RGMII|Port4_TypeCfg_SerDes|Port3_TypeCfg_SerDes|
			                   Port2_TypeCfg_SerDes|Port1_TypeCfg_SerDes|Port0_TypeCfg_GMII_MII_RGMII ); /* Force P0 as GMII/MII/RGMII, and P1~4 to UTP (10/1000M embedded PHY) */
#else
			WRITE_MEM32(PITCR, Port5_TypeCfg_GMII_MII_RGMII|Port4_TypeCfg_UTP|Port3_TypeCfg_UTP|
			                   Port2_TypeCfg_UTP|Port1_TypeCfg_UTP|Port0_TypeCfg_UTP ); /* Force P0 as GMII/MII/RGMII, and P1~4 to UTP (10/1000M embedded PHY) */
#endif
			WRITE_MEM32(P0GMIICR, (READ_MEM32(P0GMIICR)&~(CFG_GMAC_MASK))|CFG_GMAC_GMII_MII_MAC ); /* Configure Port0 as GMII MAC mode */
			WRITE_MEM32(P5GMIICR, (READ_MEM32(P5GMIICR)&~(CFG_GMAC_MASK))|CFG_GMAC_GMII_MII_MAC ); /* Configure Port0 as GMII MAC mode */

			/* We must ensure every port is in 100/1000Mbps force mode. */
			for( port = PN_PORT0; port<=PN_PORT5; port++ )
			{
				uint32 config, status;
				config = READ_MEM32( PCRP0+port*4 );
				status = READ_MEM32( PSRP0+port*4 );
				if ( ( (config&EnablePHYIf)==0 ) ||
				     ( (status&PortStatusLinkUp)==FALSE ) ||
				     ( (status&PortStatusLinkSpeed_MASK)!=wantedPortRate ) )
				{
					/* Not link or not in 100M/1000M mode, force it ! */
					/* rtlglue_printf( "%s():%d Port%d force mode(CFG=0x%08x, STA=0x%08x)\n", __FUNCTION__, __LINE__, port, config, status ); */
					config &= (BYPASS_TCRC|MIIcfg_CRS|MIIcfg_COL|MIIcfg_RXER|GMIIcfg_CRS|BCSC_Types_MASK|
					           EnLoopBack|PauseFlowControl_MASK|DisBKP|STP_PortST_MASK|AcptMaxLen_MASK); /* keep value */
					config |= ((port<<ExtPHYID_OFFSET)|EnForceMode|ForceLink|forcePortRate|ForceDuplex|EnablePHYIf);
					WRITE_MEM32( PCRP0+port*4, config );
				}
			}

			/* Before sending a packet, we must ensure the port has linked and force in 100M/1000M mode. */
			for( port = PN_PORT0; port<=PN_PORT5; port++ )
			{
				while ( 1 )
				{
					val = READ_MEM32( PSRP0+port*4 );
					if ( ( val&PortStatusLinkUp ) &&
				  	     ( (val&PortStatusLinkSpeed_MASK)==wantedPortRate ) )
					{
						break;
					}
					sleep( 1 );
				}
			}
			break;
		}
		default:
			RTL_BUG( "Never come here" );
	   		break;
	}
#elif defined(MIILIKE)
	WRITE_MEM32( TMCR, READ_MEM32(TMCR)|(0x3f<<RX_TEST_PORT_OFFSET));
	WRITE_MEM32( TMCR, READ_MEM32(TMCR)|(0x3f<<TX_TEST_PORT_OFFSET));
#else
	/* In pure model code mode (either USER or MODEL), we configure to MII-Like mode. */
	{
		enum PORT_NUM port;
		int32 needDelay = 0;
		
		WRITE_MEM32( TMCR, READ_MEM32(TMCR)|(0x3f<<RX_TEST_PORT_OFFSET));
		WRITE_MEM32( TMCR, READ_MEM32(TMCR)|(0x3f<<TX_TEST_PORT_OFFSET));
		WRITE_MEM32(PITCR, Port5_TypeCfg_GMII_MII_RGMII|Port4_TypeCfg_SerDes|Port3_TypeCfg_SerDes|
		                   Port2_TypeCfg_SerDes|Port1_TypeCfg_SerDes|Port0_TypeCfg_GMII_MII_RGMII ); /* Force P0 as GMII/MII/RGMII, and P1~4 to UTP (10/1000M embedded PHY) */
		WRITE_MEM32(P0GMIICR, (READ_MEM32(P0GMIICR)&~(CFG_GMAC_MASK))|CFG_GMAC_GMII_MII_MAC ); /* Configure Port0 as GMII MAC mode */
		WRITE_MEM32(P5GMIICR, (READ_MEM32(P5GMIICR)&~(CFG_GMAC_MASK))|CFG_GMAC_GMII_MII_MAC ); /* Configure Port0 as GMII MAC mode */

		/* We set every port in 1000Mbps force mode. */
		for( port = PN_PORT0; port<=PN_PORT5; port++ )
		{
			uint32 config;
			config = READ_MEM32( PCRP0+port*4 );
			if ( (config&EnForceMode)==1 )
			{
				/* already configured, do nothing for speed up */
			}
			else
			{	/* Not configured, this is the first time system initialization. */
				config &= (BYPASS_TCRC|MIIcfg_CRS|MIIcfg_COL|MIIcfg_RXER|GMIIcfg_CRS|BCSC_Types_MASK|
				           EnLoopBack|PauseFlowControl_MASK|DisBKP|STP_PortST_MASK|AcptMaxLen_MASK); /* keep value */
				config |= ((port<<ExtPHYID_OFFSET)|EnForceMode|ForceLink|ForceSpeed1000M|ForceDuplex|PauseFlowControlEtxErx|EnablePHYIf);
				WRITE_MEM32( PCRP0+port*4, config );
				needDelay++;
			}
		}
		if ( ( needDelay > 0 ) && ( curIcType==IC_TYPE_REAL ) )
		{ /* delay 1/100 sec for link REALLY up (only IC need delay) */
			virtualMacWaitCycle( 250000 * 10 ); /* about 10 ms */
		}
	}
#endif
	return SUCCESS;
}


int32 rtl865xC_virtualMacInput( enum PORT_NUM fromPort, uint8* packet, int32 len )
{
	int32 retval;
	enum IC_TYPE curIcType;

	model_getTestTarget(&curIcType);

#if defined(GIGAIO)
	/* GMII, support JUMBO frame */
	if ( len > (16*1024-1) )
	{
		rtlglue_printf("%s():%d Packet is larger than JUMBO frame (len=%d).\n", __FUNCTION__, __LINE__, len );
		return FAILED;
	}
#else
	/* MII, no support JUMBO frame */
	if ( len > 1522 )
	{
		rtlglue_printf("%s():%d In MII mode, we don't support JUMBO frame (len=%d).\n", __FUNCTION__, __LINE__, len );
		return FAILED;
	}
#endif
	
	switch ( curIcType )
	{
		case IC_TYPE_MODEL:
		{
			uint32 acptMaxLen; /* The max length that source port can accept. */
			
			modelExportSetOutputForm( EXPORT_RTK_SIM );
			modelExportSwcoreRegistersAndTables();
			/* call miiLikeRx() to export Mii regisers in model code */
			retval = miiLikeRx( fromPort, packet, len );
			assert( retval==SUCCESS );
			modelExportNop( 1000 ); /* delay for ALE to process. */

			switch ( READ_MEM32(PCRP0+4*fromPort) & AcptMaxLen_MASK )
			{
				case AcptMaxLen_1536:
				default:
					acptMaxLen = 1536;
					break;
				case AcptMaxLen_1552:
					acptMaxLen = 1552;
					break;
				case AcptMaxLen_9K:
					acptMaxLen = 9216;
					break;
				case AcptMaxLen_16K:
					acptMaxLen = 16383-1/*cut off*/-4/*vlan*/-8/*pppoe*/;
					break;					
			}

			if ( len>acptMaxLen )
			{	/* The packet is too long, we don't parse it ! */
				retval = SUCCESS;
			}
			else
			{	/* Good Packet, parse it ! */
				int srcPortNum,srcExtPortNum;
				hsb_param_t hsb;

				assert(fromPort<=PN_PORT5);

				srcPortNum=fromPort;
				srcExtPortNum = 0;
				modelPktParser(&hsb,packet,srcPortNum,len,NULL);

				/* HSB is updated. */
				modelStart();

				/* Call packet translator to generate packet according HSA */
				retval = modelPktTranslator( packet, len );
				assert( retval==SUCCESS );

				modelExportSetOutputForm( EXPORT_RTK_EXPECT );
				modelExportComment( "Export HSB for verify" );
				modelExportMemory( HSB_BASE, HSB_SIZE );
				modelExportComment( "Export HSA for verify" );
				modelExportMemory( HSA_BASE, HSA_SIZE );
				modelExportSwcoreRegistersAndTables();

				retval = SUCCESS;
			}
			break;
		}

//#ifdef RTL865X_MODEL_USER
#if 0
	#if defined(VERA)||defined(MIILIKE)
    	case IC_TYPE_REAL:
			retval = miiLikeRx( fromPort, packet, len );
			break;
	#elif defined(VSV)
    	case IC_TYPE_REAL:
		{
			uint32 bitTime;

#if defined(GIGAIO)
			bitTime = 1; /*ns*/
#else
			bitTime = 10; /*ns*/
#endif
			virtualMacInit();

#if 0 /* for debug */
			rtlglue_printf("--- %s():%d Before calling vsv_writePkt() ---\n", __FUNCTION__, __LINE__ );
			rtlglue_printf("PCRP0:0x%08x  PSRP0=0x%08x\n", READ_MEM32(PCRP0), READ_MEM32(PSRP0) );
			rtlglue_printf("PCRP1:0x%08x  PSRP1=0x%08x\n", READ_MEM32(PCRP1), READ_MEM32(PSRP1) );
			rtlglue_printf("PCRP2:0x%08x  PSRP2=0x%08x\n", READ_MEM32(PCRP2), READ_MEM32(PSRP2) );
			rtlglue_printf("PCRP3:0x%08x  PSRP3=0x%08x\n", READ_MEM32(PCRP3), READ_MEM32(PSRP3) );
			rtlglue_printf("PCRP4:0x%08x  PSRP4=0x%08x\n", READ_MEM32(PCRP4), READ_MEM32(PSRP4) );
			rtlglue_printf("PCRP5:0x%08x  PSRP5=0x%08x\n", READ_MEM32(PCRP5), READ_MEM32(PSRP5) );
#endif
			vsv_writePkt(fromPort, len, packet, 20/*IPG*/);
#if WAIT_METHOD==1
			vsv_waitCycle( conn_client, (8/*preamble*/+len)*8*bitTime/*ns*/ *2/*in/out*/ /4/*ns*/ ); /* Wait packet entering and exiting SWCORE. */
#endif
			retval = SUCCESS;
			break;
		}
	#else
		case IC_TYPE_REAL:
			retval = VMAC_ERROR_NON_AVAILABLE;
			break;
	#endif
#elif defined(RTL865X_MODEL_KERNEL)
		case IC_TYPE_REAL:
			retval = miiLikeRx( fromPort, packet, len );
			break;
#endif

		default:
			retval = FAILED;
			break;
	}

	return retval;
}


int32 rtl865xC_virtualMacOutput( enum PORT_MASK *toPort, uint8* packet, int32 *len )
{
	int32 retval;
	enum IC_TYPE curIcType;

	model_getTestTarget(&curIcType);

	switch ( curIcType )
	{
		case IC_TYPE_MODEL:

			/* call miiLikeTx() to export Mii registers in model code */
			
			retval = miiLikeTx( toPort, packet, len );
			modelExportComment( "-=-=-=-  Next Packet -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-" ); /* End of export, next packet! */
			break;

//#ifdef RTL865X_MODEL_USER
#if 0
	#if defined(VERA)||defined(MIILIKE)
		case IC_TYPE_REAL:
			retval = miiLikeTx( toPort, packet, len );
			break;
	#elif defined(VSV)
		case IC_TYPE_REAL:
		{
			int port_id; 
			int retLen;
			int try; /* for waiting VSV return a packet back. */
			int maxTry;
			int i;
			uint32 startTime, nowTime;

			/* We assume the corresponding registers is configured in virtualMacInput() function. */
			assert( (READ_MEM32(TMCR)&((0x3f<<RX_TEST_PORT_OFFSET)|(0x3f<<TX_TEST_PORT_OFFSET)))==0x00000000 );

#if WAIT_METHOD==1
			vsv_waitCycle( conn_client, RX_ALE_TX_CYCLE ); /* RX-ALE-TX process time. */
			maxTry = 1;
#else
			maxTry = 30; /* about 30 seconds */
#endif
			port_id = 0;
			retval = FAILED;
			/* Wait longer if the simulation server is busy. 
			 * A typical case, Layer2 forwarding, is about 10 sec. 
			 */
			for( try = 0, rtlglue_getmstime( &startTime ); 
			     try < maxTry;
				 try++ ) 
			{
				for( port_id = PN_PORT0; port_id<=PN_PORT5; port_id++ )
				{
					if ((*toPort)&(1<<port_id))
					{
						/* Yes, we should check this port. */
						retLen = vsv_readPkt(port_id, (char*)packet );
						if ( retLen>0 )
						{
							/* Yes, we successfully get a packet. */
							*toPort = (1<<port_id);
							*len = retLen;
							retval = SUCCESS;
						}
					}
					if ( retval==SUCCESS ) break; /* A packet has been got in vsv_readPkt. Break the for loop. */
				}
				if ( retval==SUCCESS ) break; /* A packet has been got in vsv_readPkt. Break the for loop. */

				/* Since bench side is running faster than IC simulation side, we must wait for IC simulation. */
				for( i = 0; i < try; i++ )
					rtlglue_printf(".");
				rtlglue_printf("\r");

				while (1) /* Wait one second pass .... Because sleep(1) always return immediately, we use getmstime() to double confirm time has passed. */
				{
					rtlglue_getmstime( &nowTime );
					if ( ((nowTime-startTime)/1000) > try ) break;
					sleep( 1 );
				}
			}
			if ( (try>0)&&(maxTry!=1) ) rtlglue_printf("\n");
			break;
		}
	#else
		case IC_TYPE_REAL:
			retval =  VMAC_ERROR_NON_AVAILABLE;
			break;
	#endif
#elif defined(RTL865X_MODEL_KERNEL)
		case IC_TYPE_REAL:
			retval = miiLikeTx( toPort, packet, len );
			break;
#endif
		default:
			retval = FAILED;
			break;
	}

	return retval;
}


int32 rtl865xC_miiLikeRx( enum PORT_NUM fromPort, uint8* packet, int32 len )
{
	/*************************************************************
	 *  MiiRx (CPU sends packet to ASIC)
	 *************************************************************/
	int32 i;
#if 1 /* According David Lu 2006/02/10 */
	uint32 regRx;
	uint32 rxEnMask;
	uint32 byteOffset;
#else /* Before 2006/02/09 */
	uint32 txEnMask;
	uint32 nibbleOffset;
#endif
	uint8 szComment[128];

	assert( packet!=NULL );

	modelExportSetIcType( IC_TYPE_MODEL );
	modelExportSetOutputForm( EXPORT_RTK_SIM );
	sprintf( szComment, "%s(from:%d,len=%d)", __FUNCTION__, fromPort, len );
	modelExportComment( szComment );

#if 1 /* According David Lu 2006/02/10 */
	/*------------------------------------------------------------
	 * 0. Prepare variables
	 */
	regRx = MIITM_RXR0+(fromPort/3)*4;
	rxEnMask = 1<<(P0RxEN_OFFSET+(fromPort%3));
	byteOffset = (fromPort%3)*8+P0RXD_OFFSET;
	virtualMacInit();

	/*------------------------------------------------------------
	 * 1. Send preamble (8 bytes, 5555 5555 5555 55d5)
	 */
	WRITE_MEM32( regRx, rxEnMask|(0x55<<byteOffset));
	WRITE_MEM32( regRx, rxEnMask|(0x55<<byteOffset));
	WRITE_MEM32( regRx, rxEnMask|(0x55<<byteOffset));
	WRITE_MEM32( regRx, rxEnMask|(0x55<<byteOffset));
	WRITE_MEM32( regRx, rxEnMask|(0x55<<byteOffset));
	WRITE_MEM32( regRx, rxEnMask|(0x55<<byteOffset));
	WRITE_MEM32( regRx, rxEnMask|(0x55<<byteOffset));
	WRITE_MEM32( regRx, rxEnMask|(0xd5<<byteOffset));
	
	/*------------------------------------------------------------
	 * 2. Send packet content
	 */
	for( i = 0; i<len; i++ )
	{
		WRITE_MEM32( regRx, rxEnMask|(packet[i]<<byteOffset) );
	}

	/*------------------------------------------------------------
	 * 3. Send tailing octets (12 Bytes)
	 */
	WRITE_MEM32( regRx, (0x00<<byteOffset));
	WRITE_MEM32( regRx, (0x00<<byteOffset));
	WRITE_MEM32( regRx, (0x00<<byteOffset));
	WRITE_MEM32( regRx, (0x00<<byteOffset));
	WRITE_MEM32( regRx, (0x00<<byteOffset));
	WRITE_MEM32( regRx, (0x00<<byteOffset));
	WRITE_MEM32( regRx, (0x00<<byteOffset));
	WRITE_MEM32( regRx, (0x00<<byteOffset));
	WRITE_MEM32( regRx, (0x00<<byteOffset));
	WRITE_MEM32( regRx, (0x00<<byteOffset));
	WRITE_MEM32( regRx, (0x00<<byteOffset));
	WRITE_MEM32( regRx, (0x00<<byteOffset));
#else /* Before 2006/02/09 */
	/*------------------------------------------------------------
	 * 0. Prepare variables
	 */
	txEnMask = 1<<(P0_RXDV+fromPort);
	nibbleOffset = fromPort*4+P0_RXD_OFFSET;
	WRITE_MEM32( TMCR, READ_MEM32(TMCR)|(0x3f<<RX_TEST_PORT_OFFSET));

	/*------------------------------------------------------------
	 * 1. Send preamble (8 bytes, aaaa aaaa aaaa aaad)
	 */
	WRITE_MEM32( MIITM_RXR, txEnMask|(0xa<<nibbleOffset));
	WRITE_MEM32( MIITM_RXR, txEnMask|(0xa<<nibbleOffset));
	WRITE_MEM32( MIITM_RXR, txEnMask|(0xa<<nibbleOffset));
	WRITE_MEM32( MIITM_RXR, txEnMask|(0xa<<nibbleOffset));
	WRITE_MEM32( MIITM_RXR, txEnMask|(0xa<<nibbleOffset));
	WRITE_MEM32( MIITM_RXR, txEnMask|(0xa<<nibbleOffset));
	WRITE_MEM32( MIITM_RXR, txEnMask|(0xa<<nibbleOffset));
	WRITE_MEM32( MIITM_RXR, txEnMask|(0xa<<nibbleOffset));
	WRITE_MEM32( MIITM_RXR, txEnMask|(0xa<<nibbleOffset));
	WRITE_MEM32( MIITM_RXR, txEnMask|(0xa<<nibbleOffset));
	WRITE_MEM32( MIITM_RXR, txEnMask|(0xa<<nibbleOffset));
	WRITE_MEM32( MIITM_RXR, txEnMask|(0xa<<nibbleOffset));
	WRITE_MEM32( MIITM_RXR, txEnMask|(0xa<<nibbleOffset));
	WRITE_MEM32( MIITM_RXR, txEnMask|(0xa<<nibbleOffset));
	WRITE_MEM32( MIITM_RXR, txEnMask|(0xa<<nibbleOffset));
	WRITE_MEM32( MIITM_RXR, txEnMask|(0xd<<nibbleOffset));
	
	/*------------------------------------------------------------
	 * 2. Send packet content
	 */
	for( i = 0; i<len; i++ )
	{
		WRITE_MEM32( MIITM_RXR, txEnMask|((packet[i]&0x0f)<<nibbleOffset));
		WRITE_MEM32( MIITM_RXR, txEnMask|((packet[i]>>4)<<nibbleOffset));
	}

	/*------------------------------------------------------------
	 * 3. Send tailing octets (12 Bytes)
	 */
	WRITE_MEM32( MIITM_RXR, txEnMask|(0x00<<nibbleOffset));
	WRITE_MEM32( MIITM_RXR, txEnMask|(0x00<<nibbleOffset));
	WRITE_MEM32( MIITM_RXR, txEnMask|(0x00<<nibbleOffset));
	WRITE_MEM32( MIITM_RXR, txEnMask|(0x00<<nibbleOffset));
	WRITE_MEM32( MIITM_RXR, txEnMask|(0x00<<nibbleOffset));
	WRITE_MEM32( MIITM_RXR, txEnMask|(0x00<<nibbleOffset));
	WRITE_MEM32( MIITM_RXR, txEnMask|(0x00<<nibbleOffset));
	WRITE_MEM32( MIITM_RXR, txEnMask|(0x00<<nibbleOffset));
	WRITE_MEM32( MIITM_RXR, txEnMask|(0x00<<nibbleOffset));
	WRITE_MEM32( MIITM_RXR, txEnMask|(0x00<<nibbleOffset));
	WRITE_MEM32( MIITM_RXR, txEnMask|(0x00<<nibbleOffset));
	WRITE_MEM32( MIITM_RXR, txEnMask|(0x00<<nibbleOffset));
	WRITE_MEM32( MIITM_RXR, txEnMask|(0x00<<nibbleOffset));
	WRITE_MEM32( MIITM_RXR, txEnMask|(0x00<<nibbleOffset));
	WRITE_MEM32( MIITM_RXR, txEnMask|(0x00<<nibbleOffset));
	WRITE_MEM32( MIITM_RXR, txEnMask|(0x00<<nibbleOffset));
	WRITE_MEM32( MIITM_RXR, txEnMask|(0x00<<nibbleOffset));
	WRITE_MEM32( MIITM_RXR, txEnMask|(0x00<<nibbleOffset));
	WRITE_MEM32( MIITM_RXR, txEnMask|(0x00<<nibbleOffset));
	WRITE_MEM32( MIITM_RXR, txEnMask|(0x00<<nibbleOffset));
	WRITE_MEM32( MIITM_RXR, txEnMask|(0x00<<nibbleOffset));
	WRITE_MEM32( MIITM_RXR, txEnMask|(0x00<<nibbleOffset));
	WRITE_MEM32( MIITM_RXR, txEnMask|(0x00<<nibbleOffset));
	WRITE_MEM32( MIITM_RXR, txEnMask|(0x00<<nibbleOffset));
#endif

	return SUCCESS;
}



int32 rtl865xC_miiLikeTx( enum PORT_MASK *toPort, uint8* packet, int32 *len )
{
	/*************************************************************
	 *  MiiTx (ASIC sends packet to CPU)
	 *************************************************************/
	uint32 data;
	uint32 port = 0xffffffff;
	uint32 retlen = 0;
#if 1 /* According David Lu 2006/02/10 */
	uint32 regTx;
	uint32 txEnMask;
	uint32 byteOffset;
	uint32 try = 512; /* Try times, I hope long enough for FPGA. (unit:byte) */
#else /* Before 2006/02/09 */
	uint32 try = (8+16384)*2; /* Max packet length (unit:nibble) */
#endif
	char szComment[128];

	assert( toPort!=NULL );
	assert( *toPort );
	assert( packet!=NULL );
	assert( len!=NULL );
	assert( *len>=64 );

	modelExportSetIcType( IC_TYPE_MODEL );
	modelExportSetOutputForm( EXPORT_RTK_EXPECT );
	sprintf( szComment, "%s(toPort:%d,len=%d)", __FUNCTION__, *toPort, *len );
	modelExportComment( szComment );

	/*------------------------------------------------------------
	 * 0. Prepare variables
	 */
	virtualMacInit();
	retlen = 0;
	
#if 0 /* Since designer does not implement these status bits, we don't check them. */
	/*------------------------------------------------------------
	 * 1. Polling until Tx packet ready
	 *------------------------------------------------------------*/
	while( 1 )
	{
		data = READ_MEM32( TMCR );
		if ( (data>>MiiTxPktRDY_OFFSET)&*toPort )
			break;
			
		if ( (try--)== 0 ) goto failed;
	}
#endif
	
#if 1 /* According David Lu 2006/02/10 */
	/*------------------------------------------------------------
	 * 2. Polling until preamble found (~8Bytes)
	 *------------------------------------------------------------*/
	while( 1 )
	{
		/* Check if any port has valid data. */
		for( port = PN_PORT0; port <= PN_PORT5; port++ )
		{
			if ( (*toPort)&(1<<port) )
			{
				regTx = MIITM_TXR0+(port/3)*4;
				txEnMask = 1<<(P0TxEN_OFFSET+(port%3));
				byteOffset = (port%3)*8+P0TXD_OFFSET;
				WRITE_MEM32( regTx, txEnMask ); /* give Mii-Tx a clock */
				data = READ_MEM32( regTx );
				if ( data & txEnMask )
					goto preamble_found;
			}
		}

		if ( (try--)== 0 ) goto failed;
	}
	/* yes, port 'port' has valid data. */
	if ( port>PN_PORT5 ) goto failed;
preamble_found:
	WRITE_MEM32( regTx, txEnMask ); /* give Mii-Tx a clock */
	*toPort = (READ_MEM32(MIITM_TXR0)>>24) | ((READ_MEM32(MIITM_TXR1)>>24)<<3); /* P0TxEN~P5TxEN */
	
	/* find preample '55'. */
	do
	{
		if (((data>>byteOffset)&0xff)==0x55)
			break;
		WRITE_MEM32( regTx, txEnMask ); /* give Mii-Tx a clock */
		data = READ_MEM32( regTx ); /* read next byte */
		if ( (try--)== 0 ) goto failed;
	} while (1);
	/* find preample 'd5'. */
	do
	{
		if (((data>>byteOffset)&0xff)==0xd5)
			break;
		WRITE_MEM32( regTx, txEnMask ); /* give Mii-Tx a clock */
		data = READ_MEM32( regTx ); /* read next byte */
		if ( (try--)== 0 ) goto failed;
	} while (1);
	
	/*------------------------------------------------------------
	 * 3. Read packet (until packet ends)
	 *------------------------------------------------------------*/
	while( 1 )
	{
		WRITE_MEM32( regTx, txEnMask ); /* give Mii-Tx a clock */
		data = READ_MEM32( regTx ); /* read next byte */
		if ( (data&txEnMask) == 0 ) /* no data valid for P0_TXDV~P5_TXDV */
			break;
		packet[retlen] = data>>byteOffset;
		
		retlen++;
	}
	
	/*------------------------------------------------------------
	 * 4. Read tailing data (12 Bytes) 
	 *------------------------------------------------------------*/
	WRITE_MEM32( regTx, txEnMask ); /* give Mii-Tx a clock */
	data = READ_MEM32( regTx );
	WRITE_MEM32( regTx, txEnMask ); /* give Mii-Tx a clock */
	data = READ_MEM32( regTx );
	WRITE_MEM32( regTx, txEnMask ); /* give Mii-Tx a clock */
	data = READ_MEM32( regTx );
	WRITE_MEM32( regTx, txEnMask ); /* give Mii-Tx a clock */
	data = READ_MEM32( regTx );
	WRITE_MEM32( regTx, txEnMask ); /* give Mii-Tx a clock */
	data = READ_MEM32( regTx );
	WRITE_MEM32( regTx, txEnMask ); /* give Mii-Tx a clock */
	data = READ_MEM32( regTx );
	WRITE_MEM32( regTx, txEnMask ); /* give Mii-Tx a clock */
	data = READ_MEM32( regTx );
	WRITE_MEM32( regTx, txEnMask ); /* give Mii-Tx a clock */
	data = READ_MEM32( regTx );
	WRITE_MEM32( regTx, txEnMask ); /* give Mii-Tx a clock */
	data = READ_MEM32( regTx );
	WRITE_MEM32( regTx, txEnMask ); /* give Mii-Tx a clock */
	data = READ_MEM32( regTx );
	WRITE_MEM32( regTx, txEnMask ); /* give Mii-Tx a clock */
	data = READ_MEM32( regTx );
	WRITE_MEM32( regTx, txEnMask ); /* give Mii-Tx a clock */
	data = READ_MEM32( regTx );
#else /* Before 2006/02/09 */
	/*------------------------------------------------------------
	 * 2. Polling until preamble found (~8Bytes)
	 *------------------------------------------------------------*/
	while( 1 )
	{
		data = READ_MEM32( MIITM_TXR );

		/* Check if any port has valid data. */
		for( port = 0; port < 5; port++ )
		{
			if ( data & (P0_TXDV<<port) )
				break;
		}

		if ( (try--)== 0 ) return FAILED;
	}
	/* yes, port 'port' has valid data. */
	if ( port>5 ) return FAILED;
	*toPort = (data>>25)&0x1f; /* P0_TXDV~P5_TXDV */
	
	/* find preample 'a'. */
	do
	{
		if ((((data&(0xf<<(port*4)))>>(port*4))&0xf)==0xa)
			break;
		data = READ_MEM32( MIITM_TXR ); /* read next nibble */
		if ( (try--)== 0 ) return FAILED;
	} while (1);
	/* find preample 'd'. */
	do
	{
		if ((((data&(0xf<<(port*4)))>>(port*4))&0xf)==0xd)
			break;
		data = READ_MEM32( MIITM_TXR ); /* read next nibble */
		if ( (try--)== 0 ) return FAILED;
	} while (1);
	
	/*------------------------------------------------------------
	 * 3. Read packet (until packet ends)
	 *------------------------------------------------------------*/
	while( 1 )
	{
		/* Top nibble */
		data = READ_MEM32( MIITM_TXR ); /* read next nibble */
		if ( (data&(0x1f<<25)) == 0 ) /* no data valid for P0_TXDV~P5_TXDV */
			break;
		packet[retlen] = (((data&(0xf<<(port*4)))>>(port*4))&0xf)<<4;
		
		/* Bottom nibble */
		data = READ_MEM32( MIITM_TXR ); /* read next nibble */
		if ( (data&(0x1f<<25)) == 0 ) /* no data valid for P0_TXDV~P5_TXDV */
			break;
		packet[retlen] |= ((data&(0xf<<(port*4)))>>(port*4))&0xf;

		retlen++;
		if ( (try--)== 0 ) return FAILED;
	}
	
	/*------------------------------------------------------------
	 * 4. Read tailing data (12 Bytes) 
	 *------------------------------------------------------------*/
	data = READ_MEM32( MIITM_TXR );
	data = READ_MEM32( MIITM_TXR );
	data = READ_MEM32( MIITM_TXR );
	data = READ_MEM32( MIITM_TXR );
	data = READ_MEM32( MIITM_TXR );
	data = READ_MEM32( MIITM_TXR );
	data = READ_MEM32( MIITM_TXR );
	data = READ_MEM32( MIITM_TXR );
	data = READ_MEM32( MIITM_TXR );
	data = READ_MEM32( MIITM_TXR );
	data = READ_MEM32( MIITM_TXR );
	data = READ_MEM32( MIITM_TXR );
	data = READ_MEM32( MIITM_TXR );
	data = READ_MEM32( MIITM_TXR );
	data = READ_MEM32( MIITM_TXR );
	data = READ_MEM32( MIITM_TXR );
	data = READ_MEM32( MIITM_TXR );
	data = READ_MEM32( MIITM_TXR );
	data = READ_MEM32( MIITM_TXR );
	data = READ_MEM32( MIITM_TXR );
	data = READ_MEM32( MIITM_TXR );
	data = READ_MEM32( MIITM_TXR );
	data = READ_MEM32( MIITM_TXR );
	data = READ_MEM32( MIITM_TXR );
#endif
	*len = retlen;
	return SUCCESS;

failed:
	*len = 0;
	*toPort = 0;
	return FAILED;
}

#endif
