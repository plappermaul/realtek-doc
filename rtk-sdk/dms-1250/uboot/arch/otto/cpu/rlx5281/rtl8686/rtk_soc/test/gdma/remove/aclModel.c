/*
* Copyright c                  Realtek Semiconductor Corporation, 2002  
* All rights reserved.
* 
* Program : Source File for Layer3/4 Model Code
* Abstract : 
* Author : Louis Yung-Chieh Lo (yjlou@realtek.com.tw)               
* $Id: aclModel.c,v 1.28 2006-09-07 09:14:04 evinlien Exp $
*/

#include <rtl_glue.h>
#include "assert.h"
#include "hsModel.h"
#include "asicRegs.h"
#include "icModel.h"
#include "modelTrace.h"
#include "l2Model.h"
#include "l34Model.h"
#include "aclModel.h"
#include "rtl865xC_tblAsicDrv.h"
#define RTL8651_TBLDRV_LOCAL_H
#include "rtl8651_aclLocal.h"
static char logmsg[120];
enum DIRECT_TX_REASON
{
	DIRECT_TX_CFI=1,
	DIRECT_TX_IPV4=1<<1,
};
int8 *actionT[] = { "", "permit", "drop", "cpu", "drop log", "drop notify", "redirect to ethernet", "redirect to pppoe", "mirror", "mirro keep match", "drop rate exceed pps", "log rate exceed pps", "drop rate exceed bps", "log rate exceed bps" };
extern int32 rtl8651_getAsicAclRule(uint32 index, _rtl8651_tblDrvAclRule_t *rule);    

void  __modelDebugDumpACL(char *filen,int line,_rtl8651_tblDrvAclRule_t *rule,int start)
{


	switch(rule->ruleType_)
	{
	case RTL8651_ACL_MAC:
		rtlglue_printf(" [%d] rule type: %s   rule action: %s\n", start, "Ethernet", actionT[rule->actionType_]);
		rtlglue_printf("\tether type: %x   ether type mask: %x\n", rule->typeLen_, rule->typeLenMask_);
		rtlglue_printf("\tDMAC: %x:%x:%x:%x:%x:%x  DMACM: %x:%x:%x:%x:%x:%x\n",
			rule->dstMac_.octet[0], rule->dstMac_.octet[1], rule->dstMac_.octet[2],
			rule->dstMac_.octet[3], rule->dstMac_.octet[4], rule->dstMac_.octet[5],
			rule->dstMacMask_.octet[0], rule->dstMacMask_.octet[1], rule->dstMacMask_.octet[2],
			rule->dstMacMask_.octet[3], rule->dstMacMask_.octet[4], rule->dstMacMask_.octet[5]
		);
		break;

	case RTL8651_ACL_IP:
	case RTL8652_ACL_IP_RANGE:
		rtlglue_printf(" [%d] rule type: %s   rule action: %s\n", start, "IP", actionT[rule->actionType_]);
		rtlglue_printf("\tdip: %d.%d.%d.%d dipM: %d.%d.%d.%d\n", (rule->dstIpAddr_>>24),
			((rule->dstIpAddr_&0x00ff0000)>>16), ((rule->dstIpAddr_&0x0000ff00)>>8),
			(rule->dstIpAddr_&0xff), (rule->dstIpAddrMask_>>24), ((rule->dstIpAddrMask_&0x00ff0000)>>16),
			((rule->dstIpAddrMask_&0x0000ff00)>>8), (rule->dstIpAddrMask_&0xff)
		);
		rtlglue_printf("\tsip: %d.%d.%d.%d sipM: %d.%d.%d.%d\n", (rule->srcIpAddr_>>24),
			((rule->srcIpAddr_&0x00ff0000)>>16), ((rule->srcIpAddr_&0x0000ff00)>>8),
			(rule->srcIpAddr_&0xff), (rule->srcIpAddrMask_>>24), ((rule->srcIpAddrMask_&0x00ff0000)>>16),
			((rule->srcIpAddrMask_&0x0000ff00)>>8), (rule->srcIpAddrMask_&0xff)
		);
		rtlglue_printf("\tTos: %x   TosM: %x   ipProto: %x   ipProtoM: %x   ipFlag: %x   ipFlagM: %x\n",
			rule->tos_, rule->tosMask_, rule->ipProto_, rule->ipProtoMask_, rule->ipFlag_, rule->ipFlagMask_
		);
		rtlglue_printf("\t<FOP:%x> <FOM:%x> <http:%x> <httpM:%x> <IdentSdip:%x> <IdentSdipM:%x> \n",
			rule->ipFOP_, rule->ipFOM_, rule->ipHttpFilter_, rule->ipHttpFilterM_, rule->ipIdentSrcDstIp_,
			rule->ipIdentSrcDstIpM_
		);
		rtlglue_printf("\t<DF:%x> <MF:%x>\n", rule->ipDF_, rule->ipMF_); 
		break;			

	case RTL8651_ACL_ICMP:
	case RTL8652_ACL_ICMP_IPRANGE:
		rtlglue_printf(" [%d] rule type: %s   rule action: %s\n", start, "ICMP", actionT[rule->actionType_]);
		rtlglue_printf("\tdip: %d.%d.%d.%d dipM: %d.%d.%d.%d\n", (rule->dstIpAddr_>>24),
			((rule->dstIpAddr_&0x00ff0000)>>16), ((rule->dstIpAddr_&0x0000ff00)>>8),
			(rule->dstIpAddr_&0xff), (rule->dstIpAddrMask_>>24), ((rule->dstIpAddrMask_&0x00ff0000)>>16),
			((rule->dstIpAddrMask_&0x0000ff00)>>8), (rule->dstIpAddrMask_&0xff)
		);
		rtlglue_printf("\tsip: %d.%d.%d.%d sipM: %d.%d.%d.%d\n", (rule->srcIpAddr_>>24),
			((rule->srcIpAddr_&0x00ff0000)>>16), ((rule->srcIpAddr_&0x0000ff00)>>8),
			(rule->srcIpAddr_&0xff), (rule->srcIpAddrMask_>>24), ((rule->srcIpAddrMask_&0x00ff0000)>>16),
			((rule->srcIpAddrMask_&0x0000ff00)>>8), (rule->srcIpAddrMask_&0xff)
		);
		rtlglue_printf("\tTos: %x   TosM: %x   type: %x   typeM: %x   code: %x   codeM: %x\n",
			rule->tos_, rule->tosMask_, rule->icmpType_, rule->icmpTypeMask_, 
			rule->icmpCode_, rule->icmpCodeMask_);
		break;

	case RTL8651_ACL_IGMP:
	case RTL8652_ACL_IGMP_IPRANGE:
		rtlglue_printf(" [%d] rule type: %s   rule action: %s\n", start, "IGMP", actionT[rule->actionType_]);
		rtlglue_printf("\tdip: %d.%d.%d.%d dipM: %d.%d.%d.%d\n", (rule->dstIpAddr_>>24),
			((rule->dstIpAddr_&0x00ff0000)>>16), ((rule->dstIpAddr_&0x0000ff00)>>8),
			(rule->dstIpAddr_&0xff), (rule->dstIpAddrMask_>>24), ((rule->dstIpAddrMask_&0x00ff0000)>>16),
			((rule->dstIpAddrMask_&0x0000ff00)>>8), (rule->dstIpAddrMask_&0xff)
		);
		rtlglue_printf("\tsip: %d.%d.%d.%d sipM: %d.%d.%d.%d\n", (rule->srcIpAddr_>>24),
			((rule->srcIpAddr_&0x00ff0000)>>16), ((rule->srcIpAddr_&0x0000ff00)>>8),
			(rule->srcIpAddr_&0xff), (rule->srcIpAddrMask_>>24), ((rule->srcIpAddrMask_&0x00ff0000)>>16),
			((rule->srcIpAddrMask_&0x0000ff00)>>8), (rule->srcIpAddrMask_&0xff)
		);
		rtlglue_printf("\tTos: %x   TosM: %x   type: %x   typeM: %x\n", rule->tos_, rule->tosMask_,
			rule->igmpType_, rule->igmpTypeMask_
		);
		break;

	case RTL8651_ACL_TCP:
	case RTL8652_ACL_TCP_IPRANGE:
		rtlglue_printf(" [%d] rule type: %s   rule action: %s\n", start, "TCP", actionT[rule->actionType_]);
		rtlglue_printf("\tdip: %d.%d.%d.%d dipM: %d.%d.%d.%d\n", (rule->dstIpAddr_>>24),
			((rule->dstIpAddr_&0x00ff0000)>>16), ((rule->dstIpAddr_&0x0000ff00)>>8),
			(rule->dstIpAddr_&0xff), (rule->dstIpAddrMask_>>24), ((rule->dstIpAddrMask_&0x00ff0000)>>16),
			((rule->dstIpAddrMask_&0x0000ff00)>>8), (rule->dstIpAddrMask_&0xff)
		);
		rtlglue_printf("\tsip: %d.%d.%d.%d sipM: %d.%d.%d.%d\n", (rule->srcIpAddr_>>24),
			((rule->srcIpAddr_&0x00ff0000)>>16), ((rule->srcIpAddr_&0x0000ff00)>>8),
			(rule->srcIpAddr_&0xff), (rule->srcIpAddrMask_>>24), ((rule->srcIpAddrMask_&0x00ff0000)>>16),
			((rule->srcIpAddrMask_&0x0000ff00)>>8), (rule->srcIpAddrMask_&0xff)
		);
		rtlglue_printf("\tTos:%x  TosM:%x  sportL:%d  sportU:%d  dportL:%d  dportU:%d\n",
			rule->tos_, rule->tosMask_, rule->tcpSrcPortLB_, rule->tcpSrcPortUB_,
			rule->tcpDstPortLB_, rule->tcpDstPortUB_
		);
		rtlglue_printf("\tflag: %x  flagM: %x  <URG:%x> <ACK:%x> <PSH:%x> <RST:%x> <SYN:%x> <FIN:%x>\n",
			rule->tcpFlag_, rule->tcpFlagMask_, rule->tcpURG_, rule->tcpACK_,
			rule->tcpPSH_, rule->tcpRST_, rule->tcpSYN_, rule->tcpFIN_
		);
		break;

	case RTL8651_ACL_UDP:
	case RTL8652_ACL_UDP_IPRANGE:
		rtlglue_printf(" [%d] rule type: %s   rule action: %s\n", start, "UDP", actionT[rule->actionType_]);
		rtlglue_printf("\tdip: %d.%d.%d.%d dipM: %d.%d.%d.%d\n", (rule->dstIpAddr_>>24),
			((rule->dstIpAddr_&0x00ff0000)>>16), ((rule->dstIpAddr_&0x0000ff00)>>8),
			(rule->dstIpAddr_&0xff), (rule->dstIpAddrMask_>>24), ((rule->dstIpAddrMask_&0x00ff0000)>>16),
			((rule->dstIpAddrMask_&0x0000ff00)>>8), (rule->dstIpAddrMask_&0xff)
		);
		rtlglue_printf("\tsip: %d.%d.%d.%d sipM: %d.%d.%d.%d\n", (rule->srcIpAddr_>>24),
			((rule->srcIpAddr_&0x00ff0000)>>16), ((rule->srcIpAddr_&0x0000ff00)>>8),
			(rule->srcIpAddr_&0xff), (rule->srcIpAddrMask_>>24), ((rule->srcIpAddrMask_&0x00ff0000)>>16),
			((rule->srcIpAddrMask_&0x0000ff00)>>8), (rule->srcIpAddrMask_&0xff)
		);
		rtlglue_printf("\tTos:%x  TosM:%x  sportL:%d  sportU:%d  dportL:%d  dportU:%d\n",
			rule->tos_, rule->tosMask_, rule->udpSrcPortLB_, rule->udpSrcPortUB_,
			rule->udpDstPortLB_, rule->udpDstPortUB_
		);
		break;				

	case RTL8651_ACL_IFSEL:
		rtlglue_printf(" [%d] rule type: %s   rule action: %s\n", start, "UDP", actionT[rule->actionType_]);
		rtlglue_printf("\tgidxSel: %x\n", rule->gidxSel_);
		break;

	case RTL8651_ACL_SRCFILTER:
		rtlglue_printf(" [%d] rule type: %s   rule action: %s\n", start, "Source Filter", actionT[rule->actionType_]);
		rtlglue_printf("\tSMAC: %x:%x:%x:%x:%x:%x  SMACM: %x:%x:%x:%x:%x:%x\n", 
			rule->srcFilterMac_.octet[0], rule->srcFilterMac_.octet[1], rule->srcFilterMac_.octet[2], 
			rule->srcFilterMac_.octet[3], rule->srcFilterMac_.octet[4], rule->srcFilterMac_.octet[5],
			rule->srcFilterMacMask_.octet[0], rule->srcFilterMacMask_.octet[1], rule->srcFilterMacMask_.octet[2],
			rule->srcFilterMacMask_.octet[3], rule->srcFilterMacMask_.octet[4], rule->srcFilterMacMask_.octet[5]
		);
		rtlglue_printf("\tsvidx: %d   svidxM: %x   sport: %d   sportM: %x   ProtoType: %x\n",
			rule->srcFilterVlanIdx_, rule->srcFilterVlanIdxMask_, rule->srcFilterPort_, rule->srcFilterPortMask_,
			(rule->srcFilterIgnoreL3L4_==TRUE? 2: (rule->srcFilterIgnoreL4_ == 1? 1: 0))
		);
		rtlglue_printf("\tsip: %d.%d.%d.%d   sipM: %d.%d.%d.%d\n", (rule->srcFilterIpAddr_>>24),
			((rule->srcFilterIpAddr_&0x00ff0000)>>16), ((rule->srcFilterIpAddr_&0x0000ff00)>>8),
			(rule->srcFilterIpAddr_&0xff), (rule->srcFilterIpAddrMask_>>24),
			((rule->srcFilterIpAddrMask_&0x00ff0000)>>16), ((rule->srcFilterIpAddrMask_&0x0000ff00)>>8),
			(rule->srcFilterIpAddrMask_&0xff)
		);
		rtlglue_printf("\tsportL: %d   sportU: %d\n", rule->srcFilterPortLowerBound_, rule->srcFilterPortUpperBound_);
		break;

	case RTL8651_ACL_DSTFILTER:
		rtlglue_printf(" [%d] rule type: %s   rule action: %s\n", start, "Deatination Filter", actionT[rule->actionType_]);
		rtlglue_printf("\tDMAC: %x:%x:%x:%x:%x:%x  DMACM: %x:%x:%x:%x:%x:%x\n", 
			rule->dstFilterMac_.octet[0], rule->dstFilterMac_.octet[1], rule->dstFilterMac_.octet[2], 
			rule->dstFilterMac_.octet[3], rule->dstFilterMac_.octet[4], rule->dstFilterMac_.octet[5],
			rule->dstFilterMacMask_.octet[0], rule->dstFilterMacMask_.octet[1], rule->dstFilterMacMask_.octet[2],
			rule->dstFilterMacMask_.octet[3], rule->dstFilterMacMask_.octet[4], rule->dstFilterMacMask_.octet[5]
		);
		rtlglue_printf("\tdvidx: %d   dvidxM: %x  ProtoType: %x   dportL: %d   dportU: %d\n",
			rule->dstFilterVlanIdx_, rule->dstFilterVlanIdxMask_, 
			(rule->dstFilterIgnoreL3L4_==TRUE? 2: (rule->dstFilterIgnoreL4_ == 1? 1: 0)), 
			rule->dstFilterPortLowerBound_, rule->dstFilterPortUpperBound_
		);
		rtlglue_printf("\tdip: %d.%d.%d.%d   dipM: %d.%d.%d.%d\n", (rule->dstFilterIpAddr_>>24),
			((rule->dstFilterIpAddr_&0x00ff0000)>>16), ((rule->dstFilterIpAddr_&0x0000ff00)>>8),
			(rule->dstFilterIpAddr_&0xff), (rule->dstFilterIpAddrMask_>>24),
			((rule->dstFilterIpAddrMask_&0x00ff0000)>>16), ((rule->dstFilterIpAddrMask_&0x0000ff00)>>8),
			(rule->dstFilterIpAddrMask_&0xff)
		);
		break;

	}


	/* Action type */
	switch (rule->actionType_) {

	case RTL8651_ACL_PERMIT: /* 0x00 */
	case RTL8651_ACL_REDIRECT: /* 0x01 */
	case RTL8651_ACL_CPU: /* 0x03 */
	case RTL8651_ACL_DROP: /* 0x02, 0x04 */
	case RTL8651_ACL_DROP_LOG: /* 0x05 */
	case RTL8651_ACL_MIRROR: /* 0x06 */
	case RTL8651_ACL_REDIRECT_PPPOE: /* 0x07 */
	case RTL8651_ACL_MIRROR_KEEP_MATCH: /* 0x09 */
		rtlglue_printf("\tdvidx: %d   hp: %d   pppoeIdx: %d   nxtHop:%d  ", rule->dvid_, rule->priority_,
			rule->pppoeIdx_, rule->nextHop_);
		break;

	case RTL8651_ACL_POLICY: /* 0x08 */
		rtlglue_printf("\thp: %d   nxtHopIdx: %d  ", rule->priority_, rule->nhIndex);
		break;

	case RTL8651_ACL_DROP_RATE_EXCEED_PPS: /* 0x0a */
	case RTL8651_ACL_LOG_RATE_EXCEED_PPS: /* 0x0b */
	case RTL8651_ACL_DROP_RATE_EXCEED_BPS: /* 0x0c */
	case RTL8651_ACL_LOG_RATE_EXCEED_BPS: /* 0x0d */
		rtlglue_printf("\trlIdx: %d  ", rule->rlIndex);
		break;
	default: assert(0);

	}
	rtlglue_printf("pktOpApp: %d\n", rule->pktOpApp);

}
static enum MODEL_BOOLEAN_VALUE modelaclmatch(hsb_param_t*hsb,hsa_param_t *hsa,_rtl8651_tblDrvAclRule_t *rule,ale_data_t* ale)
{

	ether_addr_t srcMac, dstMac;
	switch (rule->ruleType_)
	{
		case RTL8651_ACL_MAC:			
			 srcMac.octet[0] = rule->srcMacMask_.octet[0] &hsb->sa[0];
 			 srcMac.octet[1] = rule->srcMacMask_.octet[1] &hsb->sa[1];
 			 srcMac.octet[2] = rule->srcMacMask_.octet[2] &hsb->sa[2];			 
 			 srcMac.octet[3] = rule->srcMacMask_.octet[3] &hsb->sa[3];
 			 srcMac.octet[4] = rule->srcMacMask_.octet[4] &hsb->sa[4];			 
  			 srcMac.octet[5] = rule->srcMacMask_.octet[5]&hsb->sa[5];			 
			 dstMac.octet[0] = rule->dstMacMask_.octet[0]&hsb->da[0];
 			 dstMac.octet[1] = rule->dstMacMask_.octet[1]&hsb->da[1];
 			 dstMac.octet[2] = rule->dstMacMask_.octet[2]&hsb->da[2];
 			 dstMac.octet[3] = rule->dstMacMask_.octet[3]&hsb->da[3];
 			 dstMac.octet[4] = rule->dstMacMask_.octet[4]&hsb->da[4];			 
 			 dstMac.octet[5] = rule->dstMacMask_.octet[5]&hsb->da[5];	
  			 rule->srcMac_.octet[0] = rule->srcMacMask_.octet[0] & rule->srcMac_.octet[0]; 
  			 rule->srcMac_.octet[1] = rule->srcMacMask_.octet[1] & rule->srcMac_.octet[1]; 			 
  			 rule->srcMac_.octet[2] = rule->srcMacMask_.octet[2] & rule->srcMac_.octet[2]; 			 
  			 rule->srcMac_.octet[3] = rule->srcMacMask_.octet[3] & rule->srcMac_.octet[3]; 			 			 
  			 rule->srcMac_.octet[4] = rule->srcMacMask_.octet[4] & rule->srcMac_.octet[4]; 			 
  			 rule->srcMac_.octet[5] = rule->srcMacMask_.octet[5] & rule->srcMac_.octet[5]; 			 			 
   			 rule->dstMac_.octet[0] = rule->dstMacMask_.octet[0] & rule->dstMac_.octet[0]; 
   			 rule->dstMac_.octet[1] = rule->dstMacMask_.octet[1] & rule->dstMac_.octet[1]; 
   			 rule->dstMac_.octet[2] = rule->dstMacMask_.octet[2] & rule->dstMac_.octet[2]; 			 
   			 rule->dstMac_.octet[3] = rule->dstMacMask_.octet[3] & rule->dstMac_.octet[3]; 
   			 rule->dstMac_.octet[4] = rule->dstMacMask_.octet[4] & rule->dstMac_.octet[4]; 			 
   			 rule->dstMac_.octet[5] = rule->dstMacMask_.octet[5] & rule->dstMac_.octet[5]; 

			 if (((hsb->ethtype& rule->typeLenMask_) == (rule->typeLen_ & rule->typeLenMask_)) &&
			 	(memcmp(dstMac.octet, rule->dstMac_.octet, 6) == 0) &&
			 	(memcmp(srcMac.octet, rule->srcMac_.octet, 6) == 0) )
			 		goto MAYBEMATCH;
			 
			break;			
		case RTL8651_ACL_IP:			
			if ( 	(rule->srcIpAddr_&rule->srcIpAddrMask_) ==(hsb->sip & rule->srcIpAddrMask_) &&
				(rule->dstIpAddr_ & rule->dstIpAddrMask_) ==(hsb->dip & rule->dstIpAddrMask_) &&
				 ((hsb->iptos &rule->tosMask_) == (rule->tos_ & rule->tosMask_))  &&				 
				(rule->ipHttpFilter_ ==hsb->urlmch)
				)
				{	
				
			 		goto MAYBEMATCH;
					
				}
			break;
		case RTL8651_ACL_TCP:			
			if (hsb->ipfo0_n && (rule->actionType_==RTL8651_ACL_PERMIT) &&
				 	(rule->srcIpAddr_&rule->srcIpAddrMask_) ==(hsb->sip & rule->srcIpAddrMask_) &&
					(rule->dstIpAddr_&rule->dstIpAddrMask_) ==(hsb->dip & rule->dstIpAddrMask_) )
						goto MAYBEMATCH;

			if ( 	(rule->srcIpAddr_) ==(hsb->sip & rule->srcIpAddrMask_) &&
				(rule->dstIpAddr_) ==(hsb->dip & rule->dstIpAddrMask_) &&
				 (hsb->type==HSB_TYPE_TCP) &&
				 ((hsb->iptos &rule->tosMask_) == (rule->tos_ & rule->tosMask_))  &&
				((hsb->tcpfg & rule->tcpFlagMask_) == (rule->tcpFlag_ & rule->tcpFlagMask_)) &&
				( hsb->sprt >=rule->tcpSrcPortLB_ ) &&
				(hsb->sprt  <= rule->tcpSrcPortUB_)&&
				(hsb->dprt  >= rule->tcpDstPortLB_)&&
				(hsb->dprt <=  rule->tcpDstPortUB_) 				
				)
				{		

					if ((hsb->ipfo0_n) && (rule->tcpSrcPortLB_==0)&&
							(rule->tcpSrcPortUB_==0xFFFF) &&
							(rule->tcpDstPortUB_==0xFFFF) &&
							(rule->tcpDstPortLB_==0) 	)
					{
						 		goto MAYBEMATCH;
					}
					else
						goto MAYBEMATCH;
				}
			
			break;
		case RTL8651_ACL_UDP:			
			if ( 	(rule->srcIpAddr_ ) ==(hsb->sip & rule->srcIpAddrMask_) &&
				(rule->dstIpAddr_ ) ==(hsb->dip & rule->dstIpAddrMask_) &&
				 (hsb->type==HSB_TYPE_UDP) &&
 		 		 ((hsb->iptos &rule->tosMask_) == (rule->tos_ & rule->tosMask_))  &&
				(hsb->sprt>=rule->udpSrcPortLB_)&&
				(hsb->sprt<= rule->udpSrcPortUB_) &&
				(hsb->dprt >=rule->udpDstPortLB_)&&
				(hsb->dprt <= rule->udpDstPortUB_) 				
			)
			{		
			 		goto MAYBEMATCH;
				
			}
			break;
		case RTL8651_ACL_ICMP:			
			if ( 	((rule->srcIpAddr_ &rule->srcIpAddrMask_) ==(hsb->sip & rule->srcIpAddrMask_) )&&
				((rule->dstIpAddr_ &rule->dstIpAddrMask_) ==(hsb->dip & rule->dstIpAddrMask_) )&&
			       ((hsb->iptos &rule->tosMask_) == (rule->tos_ & rule->tosMask_))  &&
			       (hsb->type==HSB_TYPE_ICMP) &&
				((hsb->ipptl & rule->icmpTypeMask_)== (rule->icmpType_&rule->icmpTypeMask_)) &&
				((hsb->sprt & rule->icmpCodeMask_) == (rule->icmpCode_ & rule->icmpCodeMask_))
				)
				{
			 		goto MAYBEMATCH;
				}
			break;
		case RTL8652_ACL_ICMP_IPRANGE:			
			if ( 	((hsb->sip >= rule->srcIpAddrLB_) &&
				(hsb->sip  <= rule->srcIpAddrUB_) )&&
				(hsb->dip >= rule->dstIpAddrLB_) &&
				(hsb->dip <= rule->dstIpAddrUB_) &&
			       ((hsb->iptos &rule->tosMask_) == (rule->tos_ & rule->tosMask_))  &&
			       (hsb->type==HSB_TYPE_ICMP) &&
				((hsb->ipptl & rule->icmpTypeMask_)== (rule->icmpType_&rule->icmpTypeMask_)) &&
				((hsb->sprt & rule->icmpCodeMask_) == (rule->icmpCode_ & rule->icmpCodeMask_))
				)
				{
			 		goto MAYBEMATCH;
				}
			break;

		case RTL8651_ACL_IGMP:			
			if ( 	((rule->srcIpAddr_ & rule->srcIpAddrMask_ ) ==(hsb->sip & rule->srcIpAddrMask_) )&&
				((rule->dstIpAddr_ & rule->dstIpAddrMask_) ==(hsb->dip & rule->dstIpAddrMask_) )&&
				 (hsb->type==HSB_TYPE_IGMP) &&
				 ((hsb->iptos &rule->tosMask_) == (rule->tos_ & rule->tosMask_))  &&
			 	((hsb->ipptl & rule->igmpTypeMask_)== (rule->igmpType_&rule->igmpTypeMask_)) 
				)
			 	{
			 		goto MAYBEMATCH;
			 	}
			
			break;
		case RTL8651_ACL_SRCFILTER:
			 srcMac.octet[0] =rule->srcFilterMacMask_.octet[0] &hsb->sa[0];
 			 srcMac.octet[1] =rule->srcFilterMacMask_.octet[1] &hsb->sa[1];
 			 srcMac.octet[2] =rule->srcFilterMacMask_.octet[2] &hsb->sa[2];			 
 			 srcMac.octet[3] =rule->srcFilterMacMask_.octet[3] &hsb->sa[3];
 			 srcMac.octet[4] =rule->srcFilterMacMask_.octet[4] &hsb->sa[4];			 
  			 srcMac.octet[5] =rule->srcFilterMacMask_.octet[5] &hsb->sa[5];			 
 			 rule->srcFilterMac_.octet[0] = rule->srcFilterMacMask_.octet[0] & rule->srcFilterMac_.octet[0];
  			 rule->srcFilterMac_.octet[1] = rule->srcFilterMacMask_.octet[1] & rule->srcFilterMac_.octet[1];
   			 rule->srcFilterMac_.octet[2] = rule->srcFilterMacMask_.octet[2] & rule->srcFilterMac_.octet[2];
   			 rule->srcFilterMac_.octet[3] = rule->srcFilterMacMask_.octet[3] & rule->srcFilterMac_.octet[3];
   			 rule->srcFilterMac_.octet[4] = rule->srcFilterMacMask_.octet[4] & rule->srcFilterMac_.octet[4];
   			 rule->srcFilterMac_.octet[5] = rule->srcFilterMacMask_.octet[5] & rule->srcFilterMac_.octet[5];

			if (rule->srcFilterIgnoreL3L4_)			
			{
			 	if ( (memcmp(srcMac.octet, rule->srcFilterMac_.octet, 6) == 0) &&
					((1<<hsb->spa)&rule->srcFilterPort_) &&
					((ale->aleInternalSvid& rule->srcFilterVlanIdMask_) == (rule->srcFilterVlanIdx_ &rule->srcFilterVlanIdxMask_))
				)
		 		{
			 		goto MAYBEMATCH;
		 		}
			}
			else if (rule->srcFilterIgnoreL4_)
			{
			 	if ( (memcmp(srcMac.octet, rule->srcFilterMac_.octet, 6) == 0) &&
					((ale->aleInternalSvid& rule->srcFilterVlanIdMask_) == (rule->srcFilterVlanId_ &rule->srcFilterVlanIdMask_))&&
					((hsb->sip & rule->srcFilterIpAddrMask_) == (rule->srcFilterIpAddr_ & rule->srcFilterIpAddrMask_)) &&
					((1<<hsb->spa)&rule->srcFilterPort_) 
				)
				{
			 		goto MAYBEMATCH;
		 		}
			}
			else
			{
			 	if ( (memcmp(srcMac.octet, rule->srcFilterMac_.octet, 6) == 0) &&
					((ale->aleInternalSvid& rule->srcFilterVlanIdMask_) == (rule->srcFilterVlanId_ &rule->srcFilterVlanIdMask_))&&
					((hsb->sip & rule->srcFilterIpAddrMask_) == (rule->srcFilterIpAddr_ & rule->srcFilterIpAddrMask_)) &&
					((1<<hsb->spa)&rule->srcFilterPort_) &&
					( hsb->sprt>=rule->srcFilterPortLowerBound_ ) &&
					(hsb->sprt<=  rule->srcFilterPortUpperBound_ )
				)
				{
			 		goto MAYBEMATCH;
		 		}
			}
			
			break;
		case RTL8651_ACL_DSTFILTER:
			 dstMac.octet[0]=rule->dstMacMask_.octet[0]&hsb->da[0];
 			 dstMac.octet[1]=rule->dstMacMask_.octet[1]&hsb->da[1];
 			 dstMac.octet[2]=rule->dstMacMask_.octet[2]&hsb->da[2];
 			 dstMac.octet[3]=rule->dstMacMask_.octet[3]&hsb->da[3];
 			 dstMac.octet[4]=rule->dstMacMask_.octet[4]&hsb->da[4];			 
 			 dstMac.octet[5]=rule->dstMacMask_.octet[5]&hsb->da[5];			 
  			 rule->dstFilterMac_.octet[0] = rule->dstFilterMacMask_.octet[0] & rule->dstFilterMac_.octet[0];
  			 rule->dstFilterMac_.octet[1] = rule->dstFilterMacMask_.octet[1] & rule->dstFilterMac_.octet[1];
   			 rule->dstFilterMac_.octet[2] = rule->dstFilterMacMask_.octet[2] & rule->dstFilterMac_.octet[2];
   			 rule->dstFilterMac_.octet[3] = rule->dstFilterMacMask_.octet[3] & rule->dstFilterMac_.octet[3];
   			 rule->dstFilterMac_.octet[4] = rule->dstFilterMacMask_.octet[4] & rule->dstFilterMac_.octet[4];
   			 rule->dstFilterMac_.octet[5] = rule->dstFilterMacMask_.octet[5] & rule->dstFilterMac_.octet[5];

			if (rule->dstFilterIgnoreL3L4_)			
			{
			 	if ( (memcmp(dstMac.octet, rule->dstFilterMac_.octet, 6) == 0) &&
					((hsa->dvid& rule->dstFilterVlanIdMask_) == (rule->dstFilterVlanIdx_ &rule->dstFilterVlanIdxMask_))
				)
		 		{
			 		goto MAYBEMATCH;
		 		}
			}
			else if (rule->dstFilterIgnoreL4_)
			{
			 	if ( (memcmp(dstMac.octet, rule->dstFilterMac_.octet, 6) == 0) &&
					((hsa->dvid & rule->dstFilterVlanIdMask_) == (rule->dstFilterVlanId_ &rule->dstFilterVlanIdMask_))&&
					((hsb->dip & rule->dstFilterIpAddrMask_) == (rule->dstFilterIpAddr_ & rule->dstFilterIpAddrMask_)) 
				)
				{
			 		goto MAYBEMATCH;
		 		}
			}
			else
			{
			 	if ( (memcmp(dstMac.octet, rule->dstFilterMac_.octet, 6) == 0) &&
					((hsa->dvid & rule->dstFilterVlanIdMask_) == (rule->dstFilterVlanId_ &rule->dstFilterVlanIdMask_))&&
					((hsb->dip & rule->dstFilterIpAddrMask_) == (rule->dstFilterIpAddr_ & rule->dstFilterIpAddrMask_)) &&
					( hsb->dprt>=rule->dstFilterPortLowerBound_ ) &&
					(hsb->dprt<=  rule->dstFilterPortUpperBound_ )
				)
				{
			 		goto MAYBEMATCH;
		 		}
			}
			 break;
		case RTL8652_ACL_IP_RANGE:
				if ( 	(rule->srcIpAddrLB_ <= hsb->sip) &&(rule->srcIpAddrUB_ >= hsb->sip) &&
	   			        (rule->dstIpAddrLB_ <= hsb->dip) &&(rule->dstIpAddrUB_ >= hsb->dip) &&
      				       ((hsb->iptos &rule->tosMask_) == (rule->tos_ & rule->tosMask_))  &&
	   			        (rule->ipHttpFilter_ ==hsb->urlmch)
				)
				{		
					 		goto MAYBEMATCH;					
				}
				break;
		case RTL8652_ACL_TCP_IPRANGE:			
				if (hsb->ipfo0_n && (rule->actionType_==RTL8651_ACL_PERMIT) &&
					(rule->srcIpAddrLB_ <= hsb->sip) &&(rule->srcIpAddrUB_ >= hsb->sip) &&
   			        	(rule->dstIpAddrLB_ <= hsb->dip) &&(rule->dstIpAddrUB_ >= hsb->dip))
						goto MAYBEMATCH;
				if ( 	(rule->srcIpAddrLB_ <= hsb->sip) &&(rule->srcIpAddrUB_ >= hsb->sip) &&
	   			        (rule->dstIpAddrLB_ <= hsb->dip) &&(rule->dstIpAddrUB_ >= hsb->dip) &&
      					 (hsb->type==HSB_TYPE_TCP) &&
      				       ((hsb->iptos &rule->tosMask_) == (rule->tos_ & rule->tosMask_))  &&	   			        
					((hsb->tcpfg & rule->tcpFlagMask_) == (rule->tcpFlag_ & rule->tcpFlagMask_)) &&
					( hsb->sprt >=rule->tcpSrcPortLB_ ) &&	(hsb->sprt  <= rule->tcpSrcPortUB_)&&
					(hsb->dprt  >= rule->tcpDstPortLB_)&&	(hsb->dprt <=  rule->tcpDstPortUB_) 				
					)
				{		
					if ((hsb->ipfo0_n) && (rule->tcpSrcPortLB_==0)&&
							(rule->tcpSrcPortUB_==0xFFFF) &&
							(rule->tcpDstPortUB_==0xFFFF) &&
							(rule->tcpDstPortLB_==0) 	)
					{
						 		goto MAYBEMATCH;
					}
					else
						goto MAYBEMATCH;

					
				}			
			break;
		case RTL8652_ACL_UDP_IPRANGE:			
				if ( 	(rule->srcIpAddrLB_ <= hsb->sip) &&(rule->srcIpAddrUB_ >= hsb->sip) &&
	   			        (rule->dstIpAddrLB_ <= hsb->dip) &&(rule->dstIpAddrUB_ >= hsb->dip) &&
      					 ((hsb->iptos &rule->tosMask_) == (rule->tos_ & rule->tosMask_))  &&
      					 (hsb->type==HSB_TYPE_UDP) &&
					(hsb->sprt>=rule->udpSrcPortLB_)&&
					(hsb->sprt<= rule->udpSrcPortUB_) &&
					(hsb->dprt >=rule->udpDstPortLB_)&&
					(hsb->dprt <= rule->udpDstPortUB_) 				
				)
				{		
			 		goto MAYBEMATCH;					
				}
				break;
		case RTL8652_ACL_IGMP_IPRANGE:			
				if ( 	(rule->srcIpAddrLB_ <= hsb->sip) &&(rule->srcIpAddrUB_ >= hsb->sip) &&
	   			        (rule->dstIpAddrLB_ <= hsb->dip) &&(rule->dstIpAddrUB_ >= hsb->dip) &&
     					 (hsb->type==HSB_TYPE_IGMP) &&
					 ((hsb->iptos &rule->tosMask_) == (rule->tos_ & rule->tosMask_))  &&
				 	((hsb->ipptl & rule->igmpTypeMask_)== (rule->igmpType_&rule->igmpTypeMask_)) 
					)
				 	{
				 		goto MAYBEMATCH;
				 	}
				
				break;			
		case RTL8652_ACL_SRCFILTER_IPRANGE:
			 srcMac.octet[0] =rule->srcFilterMacMask_.octet[0] &hsb->sa[0];
 			 srcMac.octet[1] =rule->srcFilterMacMask_.octet[1] &hsb->sa[1];
 			 srcMac.octet[2] =rule->srcFilterMacMask_.octet[2] &hsb->sa[2];			 
 			 srcMac.octet[3] =rule->srcFilterMacMask_.octet[3] &hsb->sa[3];
 			 srcMac.octet[4] =rule->srcFilterMacMask_.octet[4] &hsb->sa[4];			 
  			 srcMac.octet[5] =rule->srcFilterMacMask_.octet[5] &hsb->sa[5];			 
 			 rule->srcFilterMac_.octet[0] = rule->srcFilterMacMask_.octet[0] & rule->srcFilterMac_.octet[0];
  			 rule->srcFilterMac_.octet[1] = rule->srcFilterMacMask_.octet[1] & rule->srcFilterMac_.octet[1];
   			 rule->srcFilterMac_.octet[2] = rule->srcFilterMacMask_.octet[2] & rule->srcFilterMac_.octet[2];
   			 rule->srcFilterMac_.octet[3] = rule->srcFilterMacMask_.octet[3] & rule->srcFilterMac_.octet[3];
   			 rule->srcFilterMac_.octet[4] = rule->srcFilterMacMask_.octet[4] & rule->srcFilterMac_.octet[4];
   			 rule->srcFilterMac_.octet[5] = rule->srcFilterMacMask_.octet[5] & rule->srcFilterMac_.octet[5];			 
			if (rule->srcFilterIgnoreL3L4_)			
			{
			 	if ( (memcmp(srcMac.octet, rule->srcFilterMac_.octet, 6) == 0) &&
					((1<<hsb->spa)&rule->srcFilterPort_) &&
					((ale->aleInternalSvid& rule->srcFilterVlanIdMask_) == (rule->srcFilterVlanIdx_ &rule->srcFilterVlanIdxMask_))
				)
		 		{
			 		goto MAYBEMATCH;
		 		}
			}
			else if (rule->srcFilterIgnoreL4_)
			{
			 	if ( (memcmp(srcMac.octet, rule->srcFilterMac_.octet, 6) == 0) &&
					((ale->aleInternalSvid& rule->srcFilterVlanIdMask_) == (rule->srcFilterVlanId_ &rule->srcFilterVlanIdMask_))&&
					(hsb->sip >=  rule->srcFilterIpAddrLB_) &&
					(hsb->sip <=  rule->srcFilterIpAddrUB_) &&
					((1<<hsb->spa)&rule->srcFilterPort_) 
				)
				{
			 		goto MAYBEMATCH;
		 		}
			}
			else
			{
			 	if ( (memcmp(srcMac.octet, rule->srcFilterMac_.octet, 6) == 0) &&
					((ale->aleInternalSvid& rule->srcFilterVlanIdMask_) == (rule->srcFilterVlanId_ &rule->srcFilterVlanIdMask_))&&
					(hsb->sip >=  rule->srcFilterIpAddrLB_) &&
					(hsb->sip <=  rule->srcFilterIpAddrUB_) &&
					((1<<hsb->spa)&rule->srcFilterPort_) &&
					( hsb->sprt>=rule->srcFilterPortLowerBound_ ) &&
					(hsb->sprt<=  rule->srcFilterPortUpperBound_ )
				)
				{
			 		goto MAYBEMATCH;
		 		}
			}
			
			break;
		case RTL8652_ACL_DSTFILTER_IPRANGE:
			 dstMac.octet[0]=rule->dstMacMask_.octet[0]&hsb->da[0];
 			 dstMac.octet[1]=rule->dstMacMask_.octet[1]&hsb->da[1];
 			 dstMac.octet[2]=rule->dstMacMask_.octet[2]&hsb->da[2];
 			 dstMac.octet[3]=rule->dstMacMask_.octet[3]&hsb->da[3];
 			 dstMac.octet[4]=rule->dstMacMask_.octet[4]&hsb->da[4];			 
 			 dstMac.octet[5]=rule->dstMacMask_.octet[5]&hsb->da[5];			 
  			 rule->dstFilterMac_.octet[0] = rule->dstFilterMacMask_.octet[0] & rule->dstFilterMac_.octet[0];
  			 rule->dstFilterMac_.octet[1] = rule->dstFilterMacMask_.octet[1] & rule->dstFilterMac_.octet[1];
   			 rule->dstFilterMac_.octet[2] = rule->dstFilterMacMask_.octet[2] & rule->dstFilterMac_.octet[2];
   			 rule->dstFilterMac_.octet[3] = rule->dstFilterMacMask_.octet[3] & rule->dstFilterMac_.octet[3];
   			 rule->dstFilterMac_.octet[4] = rule->dstFilterMacMask_.octet[4] & rule->dstFilterMac_.octet[4];
   			 rule->dstFilterMac_.octet[5] = rule->dstFilterMacMask_.octet[5] & rule->dstFilterMac_.octet[5];

			if (rule->dstFilterIgnoreL3L4_)			
			{

			 	if ( (memcmp(dstMac.octet, rule->dstFilterMac_.octet, 6) == 0) &&
					((hsa->dvid& rule->dstFilterVlanIdMask_) == (rule->dstFilterVlanIdx_ &rule->dstFilterVlanIdxMask_))
				)
		 		{
			 		goto MAYBEMATCH;
		 		}
			}
			else if (rule->dstFilterIgnoreL4_)
			{
			 	if ( (memcmp(dstMac.octet, rule->dstFilterMac_.octet, 6) == 0) &&
					((hsa->dvid & rule->dstFilterVlanIdMask_) == (rule->dstFilterVlanId_ &rule->dstFilterVlanIdMask_))&&
					((hsb->dip >= rule->dstFilterIpAddrLB_) == (hsb->dip <= rule->dstFilterIpAddrUB_)) 
				)
				{
			 		goto MAYBEMATCH;
		 		}
			}
			else
			{
			 	if ( (memcmp(dstMac.octet, rule->dstFilterMac_.octet, 6) == 0) &&
					((hsa->dvid & rule->dstFilterVlanIdMask_) == (rule->dstFilterVlanId_ &rule->dstFilterVlanIdMask_))&&
					((hsb->dip >= rule->dstFilterIpAddrLB_) == (hsb->dip <= rule->dstFilterIpAddrUB_)) &&
					( hsb->dprt>=rule->dstFilterPortLowerBound_ ) &&
					(hsb->dprt<=  rule->dstFilterPortUpperBound_ )
				)
				{
			 		goto MAYBEMATCH;
		 		}
			}



			
				
		default:
			break;
			
	}
	return MNOTMATCH;
MAYBEMATCH:
	if (rule->pktOpApp==RTL865XC_ACLTBL_ALL_LAYER)
			return MMATCH;

	
	if ((rule->pktOpApp==RTL8651_ACLTBL_ONLY_L2) && (ale->doL3==0))
			return MMATCH;
	if ((rule->pktOpApp==RTL8651_ACLTBL_ONLY_L3) && (ale->doL3==1) &&(ale->doL4==0))
			return MMATCH;
	if ((rule->pktOpApp==RTL8651_ACLTBL_L2_AND_L3) && (ale->doL4==0))
			return MMATCH;
	if ((rule->pktOpApp==RTL8651_ACLTBL_L3_AND_L4) && (ale->doL3==1))
			return MMATCH;
	if ((rule->pktOpApp==RTL8651_ACLTBL_ONLY_L4) && (ale->doL4==1))
			return MMATCH;
	
	return MNOTMATCH;		
}
/*
@func enum MODEL_RETURN_VALUE | modelAfterL34IngressACLCheck	| model code for ingress check
@parm hsb_param_t* | hsb | Header Stamp of Before (for input)
@parm hsa_param_t* | hsa | Header Stamp of After (for output)
@rvalue enum MODEL_RETURN_VALUE |
@comm 
*/
enum MODEL_RETURN_VALUE modelAfterL34IngressACLCheck( hsb_param_t* hsb, hsa_param_t* hsa, ale_data_t *ale )
{	
	_rtl8651_tblDrvAclRule_t rule;
	uint32 i,start,end;
	uint32 spa;

	spa = hsb->spa;	
	if ((READ_VIR32(MSCR) & EN_IN_ACL) != EN_IN_ACL)
	{
		return MRET_OK;
	}
	/* We check VLAN ingress filtering only when enabled. */
	
	if (ale->matchSrcNetif)
	{
		start= ale->srcNetif.inAclStart;
		end = ale->srcNetif.inAclEnd;
	}	
	else
	{
		MT_WATCH("modelAfterL34IngressACLCheck Should not happen");
		return MRET_OK;
			
	}
	/* Verify the member port of VLAN. Drop this packet if the packet is coming from the non-existed member port. */
	for (i=start;i<=end;i++)
	{
			rtl8651_getAsicAclRule(i,&rule);
			switch  (rule.actionType_)
			{
				case RTL8651_ACL_POLICY:				
					if (modelaclmatch(hsb,hsa,&rule,ale)==MMATCH)
					{
						sprintf(logmsg,"ACL POLICY Match %d rule nexthop %d.",i,rule.nhIndex);						
						MT_WATCH(logmsg);
						ale->nexthop=rule.nhIndex;
						modelHandleNexthop(hsb,hsa,ale);
					}
					break;
				case RTL8651_ACL_REDIRECT:				
					if (modelaclmatch(hsb,hsa,&rule,ale)==MMATCH)
					{
						sprintf(logmsg,"ACL REDIRECT Match %d rule l2 entry %d.",i,rule.nextHop_);						
						MT_WATCH(logmsg);						
						ale->dstNetifIdx=rule.dvid_;
						modelHandleRedirect(hsb,hsa,ale,rule.nextHop_);
					}
					break;

			}

		
	}	
	return MRET_OK;
}

/*
@func enum MODEL_RETURN_VALUE | modelIngressACLCheck	| model code for ingress check
@parm hsb_param_t* | hsb | Header Stamp of Before (for input)
@parm hsa_param_t* | hsa | Header Stamp of After (for output)
@rvalue enum MODEL_RETURN_VALUE |
@comm 
*/
enum MODEL_RETURN_VALUE modelIngressACLCheck( hsb_param_t* hsb, hsa_param_t* hsa, ale_data_t *ale )
{	
	_rtl8651_tblDrvAclRule_t rule;
	uint32 i,start,end;
	uint32 spa;

	spa = hsb->spa;	
	if (hsb->dirtx)
	{	/* Direct Tx to CPU */
		MT_WATCH("Direct TO CPU");
		if (hsb->tcpfg ==DIRECT_TX_CFI)
		{
			modelCheckIgnore1QTag(hsb,hsa,ale);
			MT_TOCPU("Due to Direct TX Reason:CFI");
			return MRET_TOCPU;
		}
		if( (hsb->tcpfg&DIRECT_TX_IPV4) && (READ_VIR32(MSCR) & (EN_OUT_ACL| EN_IN_ACL)))
		{
			modelCheckIgnore1QTag(hsb,hsa,ale);
			MT_TOCPU("Due to Direct TX Reason:IPV4 Header is not 0x45");
			modelCPUport(hsb, hsa,  ale);
			return MRET_TOCPU;
		}
		if ((hsb->tcpfg !=DIRECT_TX_CFI)&&(hsb->tcpfg!=DIRECT_TX_IPV4))
		{
			modelCheckIgnore1QTag(hsb,hsa,ale);
			MT_TOCPU("Due to Direct TX Reason:?????????");
			modelCPUport(hsb, hsa,  ale);
			return MRET_TOCPU;
		}
			
		
	}

	if ((READ_VIR32(MSCR) & EN_IN_ACL) != EN_IN_ACL)
	{
		return MRET_OK;
	}
	/* We check VLAN ingress filtering only when enabled. */
	
	if (ale->matchSrcNetif)
	{
		start= ale->srcNetif.inAclStart;
		end = ale->srcNetif.inAclEnd;
		sprintf(logmsg," srcNetif %d start %d, end %d",ale->srcNetifIdx,start,end);						
		MT_WATCH(logmsg);
	}	
	else
	{
		start = READ_VIR32(DACLRCR)&0x7f;
		end = (READ_VIR32(DACLRCR)>>7)&0x7f;
	}
	/* Verify the member port of VLAN. Drop this packet if the packet is coming from the non-existed member port. */
	for (i=start;i<=end;i++)
	{
		 	if ((READ_MEM32(SWTCR1)&EN_FRAG_TO_ACLPT)==0  && (hsb->ipfo0_n) &&(hsb->type>=HSB_TYPE_IP))
	 		{
				MT_TOCPU("ACL Ingress due to Fragment Packet (SWTCR1)");
 				modelCPUport( hsb,  hsa,ale);
				
	 		}
			rtl8651_getAsicAclRule(i,&rule);
			switch  (rule.actionType_)
			{
				case RTL8651_ACL_PERMIT:
					if (modelaclmatch(hsb,hsa,&rule,ale)==MMATCH)
					{
						sprintf(logmsg," MATCH PERMIT %d",i);						
						MT_WATCH(logmsg);
						ale->lastMatchedIngressACLRule = i;
						return MRET_OK;
					}
					break;
				case RTL8651_ACL_DROP:					
					if (modelaclmatch(hsb,hsa,&rule,ale)==MMATCH)
					{
						sprintf(logmsg,"ACL Ingress Drop Match %d rule...",i);
						MT_DROP(logmsg);
						modelDrop( hsb, hsa,ale);
						return MRET_DROP;
					}
					break;
				case RTL8651_ACL_CPU:						
					if (modelaclmatch(hsb,hsa,&rule,ale)==MMATCH)
					{
						sprintf(logmsg,"ACL Ingress to CPU Match %d rule...",i);						
						MT_TOCPU(logmsg);
						modelCPUport(hsb,hsa,ale);
						hsa->why2cpu = i<<5 | 0x4;		/*ACL matched to CPU */
						return MRET_TOCPU;
					}
					break;
				case RTL8651_ACL_DROP_NOTIFY:						
					if (modelaclmatch(hsb,hsa,&rule,ale)==MMATCH)
					{
						sprintf(logmsg,"ACL Ingress to Log to CPU Match %d rule...",i);						
						MT_TOCPU(logmsg);
						modelCPUport(hsb,hsa,ale);
						hsa->why2cpu = i<<5 | 0x6;		/*ACL matched log to CPU*/
						return MRET_TOCPU;
					}
					break;
				case RTL8651_ACL_PRIORITY:
					if (modelaclmatch(hsb,hsa,&rule,ale)==MMATCH)
					{
						sprintf(logmsg,"ACL Priority Match %d rule...",i);						
						MT_WATCH(logmsg);
						modelSetPriroity(PRI_ACL, (uint8)rule.priority,ale);
					}
					break;

			}
				
	}	

	modelDrop(hsb,hsa,ale);
	MT_DROP("Ingress ACL Drop (No Rule)");	
	return MRET_DROP;
}



/*
@func enum MODEL_RETURN_VALUE | modelIngressACLCheck	| model code for ingress check
@parm hsb_param_t* | hsb | Header Stamp of Before (for input)
@parm hsa_param_t* | hsa | Header Stamp of After (for output)
@rvalue enum MODEL_RETURN_VALUE |
@comm 
*/

enum MODEL_RETURN_VALUE modelEgressACLCheck( hsb_param_t* hsb, hsa_param_t* hsa, ale_data_t *ale )
{
	_rtl8651_tblDrvAclRule_t rule;
	uint32 i,start,end;
	
	if ((READ_VIR32(MSCR) & EN_OUT_ACL) != EN_OUT_ACL)
	{
		return MRET_OK;
	}
	if (hsa->ipmcastr)
	{
		MT_WATCH("IP Muliticast Packet and Ignore Egress Check");
		return MRET_OK;
	}
	/* We check VLAN ingress filtering only when enabled. */	
	if ( ale->matchDstNetif )
	{
		start = ale->dstNetif.outAclStart;
		end = ale->dstNetif.outAclEnd;

	}
	else
	{
		start = (READ_VIR32(DACLRCR)>>14)&0x7f;
		end= (READ_VIR32(DACLRCR)>>21)&0x7f;
	}	
	/* Verify the member port of VLAN. Drop this packet if the packet is coming from the non-existed member port. */
	for (i=start;i<=end;i++)
	{
		 	if ((READ_MEM32(SWTCR1)&EN_FRAG_TO_ACLPT)==0  && (hsb->ipfo0_n) &&(hsb->type>=HSB_TYPE_IP))
	 		{
				MT_TOCPU("ACL Egress due to Fragment Packet (SWTCR1)");
 				modelCPUport( hsb,  hsa,ale);
				
	 		}
			rtl8651_getAsicAclRule(i,&rule);
			switch  (rule.actionType_)
			{
				case RTL8651_ACL_PERMIT:
					if (modelaclmatch(hsb,hsa,&rule,ale)==MMATCH)
					{
						sprintf(logmsg,"ACL Egress PERMIT Match %d rule ....",i);
						ale->lastMatchedEgressACLRule = i;
						return MRET_OK;
					}
					break;
				case RTL8651_ACL_DROP:					
					if (modelaclmatch(hsb,hsa,&rule,ale)==MMATCH)
					{
						modelDrop(hsb,hsa,ale);
						sprintf(logmsg,"ACL Egress Drop Match %d rule ....",i);
						MT_DROP(logmsg);
						return MRET_DROP;
					}
					break;
				case RTL8651_ACL_CPU:						
					if (modelaclmatch(hsb,hsa,&rule,ale)==MMATCH)
					{
						modelCPUport(hsb,hsa,ale);
						return MRET_TOCPU;
					}
					break;
				case RTL8651_ACL_PRIORITY:
					if (modelaclmatch(hsb,hsa,&rule,ale)==MMATCH)
					{
						sprintf(logmsg,"ACL (out)Priority Match %d rule...",i);						
						MT_WATCH(logmsg);
						modelSetPriroity(PRI_ACL, (uint8)rule.priority,ale);
					}
					break;

			}
				
	}	
	modelDrop(hsb,hsa,ale);
	sprintf(logmsg,"ACL Egress Drop (No Rule Match ) VID %d, Using  acl start %d end %d ...",ale->dstNetif.vid,start,end);
	MT_DROP(logmsg);
	return MRET_DROP;
}

