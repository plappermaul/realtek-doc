#include <stdio.h>
#include "rtl_types.h"
#include <linux/string.h>
#ifdef RTL865X_DEBUG
#include <linux/proc_fs.h>
#endif
#include "types.h"
#include "assert.h"
#include "rtl_cle.h" 
#include "cle_utility.h" 
#include "rtl8651_tblDrv.h"
#include "rtl8651_tblDrvFwd.h"
#include "rtl8651_tblAsicDrv.h"
#include "rtl8651_tblDrvLocal.h"
#include "rtl8651_tblDrvFwdLocal.h"
#include "rtl8651_tblDrvProto.h"
#include "rtl8651_tblDrvStatistic.h"
#include "rtl8651_tblDrvPatch.h"
#include "rtl8651_alg_pptp.h"
#include "rtl8651_tblDrvStatistic.h"
#include "asicRegs.h"
#include "../swNic2.h"
#ifdef CONFIG_RTL865X_MULTICAST
#include "rtl8651_multicast.h"
#endif /* CONFIG_RTL865X_MULTICAST */
#ifdef CONFIG_RTL865X_IPSEC
#include "rtl8651_ipsec_local.h"
#endif /* CONFIG_RTL865X_IPSEC */
#ifdef CONFIG_RTL865X_ROMEPERF
#include "../romeperf.h"
#endif /* CONFIG_RTL865X_ROMEPERF */
#include "rtl8651_dos.h"
#ifdef RTL865X_DEBUG
#include <linux/proc_fs.h>
#endif
#include "rtl_glue.h"
#ifdef CONFIG_RTL865X_VOIP
#include "voip_support.h"
#endif

#include "rtl8651_dns_domainBlock.h"

void dumpAcl(void){

	//int8 *actionT[] = { "", "permit", "drop", "cpu", "drop and log", "redirect to ethernet", "redirect to pppoe" };
	int8 *actionT[] = { "", "permit", "drop", "cpu", "drop log", "drop notify", "redirect to ethernet", "redirect to pppoe", "mirror", "mirro keep match", "drop rate exceed pps", "log rate exceed pps", "drop rate exceed bps", "log rate exceed bps" };
	_rtl8651_tblDrvAclRule_t asic_acl;
	rtl865x_tblAsicDrv_vlanParam_t asic_vlan;
	uint32 start, end;

	uint16 vid;
	int8 outRule;


			
	rtlglue_printf(">>ASIC ACL Table:\n\n");
	for(vid=0; vid<4095; vid++ ) {
		/* Read VLAN Table */
		if (rtl8651_getAsicVlan(vid, &asic_vlan) == FAILED)
			continue;

		outRule = FALSE;
		start = asic_vlan.inAclStart; end = asic_vlan.inAclEnd;
again:
		if (outRule == FALSE)
			rtlglue_printf("\n<<Ingress Rule for Vlan %d:>>\n", vid);
		else rtlglue_printf("\n<<Egress Rule for Vlan %d>>:\n", vid);
		for( ; start<=end; start++) {
			if (rtl8651_getAsicAclRule(start, &asic_acl) == FAILED)
				assert(0);
			switch(asic_acl.ruleType_)
			{
			case RTL8651_ACL_MAC:
				rtlglue_printf(" [%d] rule type: %s   rule action: %s\n", start, "Ethernet", actionT[asic_acl.actionType_]);
				rtlglue_printf("\tether type: %x   ether type mask: %x\n", asic_acl.typeLen_, asic_acl.typeLenMask_);
				rtlglue_printf("\tDMAC: %x:%x:%x:%x:%x:%x  DMACM: %x:%x:%x:%x:%x:%x\n",
					asic_acl.dstMac_.octet[0], asic_acl.dstMac_.octet[1], asic_acl.dstMac_.octet[2],
					asic_acl.dstMac_.octet[3], asic_acl.dstMac_.octet[4], asic_acl.dstMac_.octet[5],
					asic_acl.dstMacMask_.octet[0], asic_acl.dstMacMask_.octet[1], asic_acl.dstMacMask_.octet[2],
					asic_acl.dstMacMask_.octet[3], asic_acl.dstMacMask_.octet[4], asic_acl.dstMacMask_.octet[5]
				);
				rtlglue_printf("\tSMAC: %x:%x:%x:%x:%x:%x  SMACM: %x:%x:%x:%x:%x:%x\n",
					asic_acl.srcMac_.octet[0], asic_acl.srcMac_.octet[1], asic_acl.srcMac_.octet[2],
					asic_acl.srcMac_.octet[3], asic_acl.srcMac_.octet[4], asic_acl.srcMac_.octet[5],
					asic_acl.srcMacMask_.octet[0], asic_acl.srcMacMask_.octet[1], asic_acl.srcMacMask_.octet[2],
					asic_acl.srcMacMask_.octet[3], asic_acl.srcMacMask_.octet[4], asic_acl.srcMacMask_.octet[5]
				);
				break;

			case RTL8651_ACL_IP:
				rtlglue_printf(" [%d] rule type: %s   rule action: %s\n", start, "IP", actionT[asic_acl.actionType_]);
				rtlglue_printf("\tdip: %d.%d.%d.%d dipM: %d.%d.%d.%d\n", (asic_acl.dstIpAddr_>>24),
					((asic_acl.dstIpAddr_&0x00ff0000)>>16), ((asic_acl.dstIpAddr_&0x0000ff00)>>8),
					(asic_acl.dstIpAddr_&0xff), (asic_acl.dstIpAddrMask_>>24), ((asic_acl.dstIpAddrMask_&0x00ff0000)>>16),
					((asic_acl.dstIpAddrMask_&0x0000ff00)>>8), (asic_acl.dstIpAddrMask_&0xff)
				);
				rtlglue_printf("\tsip: %d.%d.%d.%d sipM: %d.%d.%d.%d\n", (asic_acl.srcIpAddr_>>24),
					((asic_acl.srcIpAddr_&0x00ff0000)>>16), ((asic_acl.srcIpAddr_&0x0000ff00)>>8),
					(asic_acl.srcIpAddr_&0xff), (asic_acl.srcIpAddrMask_>>24), ((asic_acl.srcIpAddrMask_&0x00ff0000)>>16),
					((asic_acl.srcIpAddrMask_&0x0000ff00)>>8), (asic_acl.srcIpAddrMask_&0xff)
				);
				rtlglue_printf("\tTos: %x   TosM: %x   ipProto: %x   ipProtoM: %x   ipFlag: %x   ipFlagM: %x\n",
					asic_acl.tos_, asic_acl.tosMask_, asic_acl.ipProto_, asic_acl.ipProtoMask_, asic_acl.ipFlag_, asic_acl.ipFlagMask_
				);
				rtlglue_printf("\t<FOP:%x> <FOM:%x> <http:%x> <httpM:%x> <IdentSdip:%x> <IdentSdipM:%x> \n",
					asic_acl.ipFOP_, asic_acl.ipFOM_, asic_acl.ipHttpFilter_, asic_acl.ipHttpFilterM_, asic_acl.ipIdentSrcDstIp_,
					asic_acl.ipIdentSrcDstIpM_
				);
				rtlglue_printf("\t<DF:%x> <MF:%x>\n", asic_acl.ipDF_, asic_acl.ipMF_); 
				break;			

			case RTL8651_ACL_ICMP:
				rtlglue_printf(" [%d] rule type: %s   rule action: %s\n", start, "ICMP", actionT[asic_acl.actionType_]);
				rtlglue_printf("\tdip: %d.%d.%d.%d dipM: %d.%d.%d.%d\n", (asic_acl.dstIpAddr_>>24),
					((asic_acl.dstIpAddr_&0x00ff0000)>>16), ((asic_acl.dstIpAddr_&0x0000ff00)>>8),
					(asic_acl.dstIpAddr_&0xff), (asic_acl.dstIpAddrMask_>>24), ((asic_acl.dstIpAddrMask_&0x00ff0000)>>16),
					((asic_acl.dstIpAddrMask_&0x0000ff00)>>8), (asic_acl.dstIpAddrMask_&0xff)
				);
				rtlglue_printf("\tsip: %d.%d.%d.%d sipM: %d.%d.%d.%d\n", (asic_acl.srcIpAddr_>>24),
					((asic_acl.srcIpAddr_&0x00ff0000)>>16), ((asic_acl.srcIpAddr_&0x0000ff00)>>8),
					(asic_acl.srcIpAddr_&0xff), (asic_acl.srcIpAddrMask_>>24), ((asic_acl.srcIpAddrMask_&0x00ff0000)>>16),
					((asic_acl.srcIpAddrMask_&0x0000ff00)>>8), (asic_acl.srcIpAddrMask_&0xff)
				);
				rtlglue_printf("\tTos: %x   TosM: %x   type: %x   typeM: %x   code: %x   codeM: %x\n",
					asic_acl.tos_, asic_acl.tosMask_, asic_acl.icmpType_, asic_acl.icmpTypeMask_, 
					asic_acl.icmpCode_, asic_acl.icmpCodeMask_);
				break;

			case RTL8651_ACL_IGMP:
				rtlglue_printf(" [%d] rule type: %s   rule action: %s\n", start, "IGMP", actionT[asic_acl.actionType_]);
				rtlglue_printf("\tdip: %d.%d.%d.%d dipM: %d.%d.%d.%d\n", (asic_acl.dstIpAddr_>>24),
					((asic_acl.dstIpAddr_&0x00ff0000)>>16), ((asic_acl.dstIpAddr_&0x0000ff00)>>8),
					(asic_acl.dstIpAddr_&0xff), (asic_acl.dstIpAddrMask_>>24), ((asic_acl.dstIpAddrMask_&0x00ff0000)>>16),
					((asic_acl.dstIpAddrMask_&0x0000ff00)>>8), (asic_acl.dstIpAddrMask_&0xff)
				);
				rtlglue_printf("\tsip: %d.%d.%d.%d sipM: %d.%d.%d.%d\n", (asic_acl.srcIpAddr_>>24),
					((asic_acl.srcIpAddr_&0x00ff0000)>>16), ((asic_acl.srcIpAddr_&0x0000ff00)>>8),
					(asic_acl.srcIpAddr_&0xff), (asic_acl.srcIpAddrMask_>>24), ((asic_acl.srcIpAddrMask_&0x00ff0000)>>16),
					((asic_acl.srcIpAddrMask_&0x0000ff00)>>8), (asic_acl.srcIpAddrMask_&0xff)
				);
				rtlglue_printf("\tTos: %x   TosM: %x   type: %x   typeM: %x\n", asic_acl.tos_, asic_acl.tosMask_,
					asic_acl.igmpType_, asic_acl.igmpTypeMask_
				);
				break;

			case RTL8651_ACL_TCP:
				rtlglue_printf(" [%d] rule type: %s   rule action: %s\n", start, "TCP", actionT[asic_acl.actionType_]);
				rtlglue_printf("\tdip: %d.%d.%d.%d dipM: %d.%d.%d.%d\n", (asic_acl.dstIpAddr_>>24),
					((asic_acl.dstIpAddr_&0x00ff0000)>>16), ((asic_acl.dstIpAddr_&0x0000ff00)>>8),
					(asic_acl.dstIpAddr_&0xff), (asic_acl.dstIpAddrMask_>>24), ((asic_acl.dstIpAddrMask_&0x00ff0000)>>16),
					((asic_acl.dstIpAddrMask_&0x0000ff00)>>8), (asic_acl.dstIpAddrMask_&0xff)
				);
				rtlglue_printf("\tsip: %d.%d.%d.%d sipM: %d.%d.%d.%d\n", (asic_acl.srcIpAddr_>>24),
					((asic_acl.srcIpAddr_&0x00ff0000)>>16), ((asic_acl.srcIpAddr_&0x0000ff00)>>8),
					(asic_acl.srcIpAddr_&0xff), (asic_acl.srcIpAddrMask_>>24), ((asic_acl.srcIpAddrMask_&0x00ff0000)>>16),
					((asic_acl.srcIpAddrMask_&0x0000ff00)>>8), (asic_acl.srcIpAddrMask_&0xff)
				);
				rtlglue_printf("\tTos:%x  TosM:%x  sportL:%d  sportU:%d  dportL:%d  dportU:%d\n",
					asic_acl.tos_, asic_acl.tosMask_, asic_acl.tcpSrcPortLB_, asic_acl.tcpSrcPortUB_,
					asic_acl.tcpDstPortLB_, asic_acl.tcpDstPortUB_
				);
				rtlglue_printf("\tflag: %x  flagM: %x  <URG:%x> <ACK:%x> <PSH:%x> <RST:%x> <SYN:%x> <FIN:%x>\n",
					asic_acl.tcpFlag_, asic_acl.tcpFlagMask_, asic_acl.tcpURG_, asic_acl.tcpACK_,
					asic_acl.tcpPSH_, asic_acl.tcpRST_, asic_acl.tcpSYN_, asic_acl.tcpFIN_
				);
				break;

			case RTL8651_ACL_UDP:
				rtlglue_printf(" [%d] rule type: %s   rule action: %s\n", start, "UDP", actionT[asic_acl.actionType_]);
				rtlglue_printf("\tdip: %d.%d.%d.%d dipM: %d.%d.%d.%d\n", (asic_acl.dstIpAddr_>>24),
					((asic_acl.dstIpAddr_&0x00ff0000)>>16), ((asic_acl.dstIpAddr_&0x0000ff00)>>8),
					(asic_acl.dstIpAddr_&0xff), (asic_acl.dstIpAddrMask_>>24), ((asic_acl.dstIpAddrMask_&0x00ff0000)>>16),
					((asic_acl.dstIpAddrMask_&0x0000ff00)>>8), (asic_acl.dstIpAddrMask_&0xff)
				);
				rtlglue_printf("\tsip: %d.%d.%d.%d sipM: %d.%d.%d.%d\n", (asic_acl.srcIpAddr_>>24),
					((asic_acl.srcIpAddr_&0x00ff0000)>>16), ((asic_acl.srcIpAddr_&0x0000ff00)>>8),
					(asic_acl.srcIpAddr_&0xff), (asic_acl.srcIpAddrMask_>>24), ((asic_acl.srcIpAddrMask_&0x00ff0000)>>16),
					((asic_acl.srcIpAddrMask_&0x0000ff00)>>8), (asic_acl.srcIpAddrMask_&0xff)
				);
				rtlglue_printf("\tTos:%x  TosM:%x  sportL:%d  sportU:%d  dportL:%d  dportU:%d\n",
					asic_acl.tos_, asic_acl.tosMask_, asic_acl.udpSrcPortLB_, asic_acl.udpSrcPortUB_,
					asic_acl.udpDstPortLB_, asic_acl.udpDstPortUB_
				);
				break;				

			case RTL8651_ACL_IFSEL:
				rtlglue_printf(" [%d] rule type: %s   rule action: %s\n", start, "UDP", actionT[asic_acl.actionType_]);
				rtlglue_printf("\tgidxSel: %x\n", asic_acl.gidxSel_);
				break;

			case RTL8651_ACL_SRCFILTER:
				rtlglue_printf(" [%d] rule type: %s   rule action: %s\n", start, "Source Filter", actionT[asic_acl.actionType_]);
				rtlglue_printf("\tSMAC: %x:%x:%x:%x:%x:%x  SMACM: %x:%x:%x:%x:%x:%x\n", 
					asic_acl.srcFilterMac_.octet[0], asic_acl.srcFilterMac_.octet[1], asic_acl.srcFilterMac_.octet[2], 
					asic_acl.srcFilterMac_.octet[3], asic_acl.srcFilterMac_.octet[4], asic_acl.srcFilterMac_.octet[5],
					asic_acl.srcFilterMacMask_.octet[0], asic_acl.srcFilterMacMask_.octet[1], asic_acl.srcFilterMacMask_.octet[2],
					asic_acl.srcFilterMacMask_.octet[3], asic_acl.srcFilterMacMask_.octet[4], asic_acl.srcFilterMacMask_.octet[5]
				);
				rtlglue_printf("\tsvidx: %d   svidxM: %x   sport: %d   sportM: %x   ProtoType: %x\n",
					asic_acl.srcFilterVlanIdx_, asic_acl.srcFilterVlanIdxMask_, asic_acl.srcFilterPort_, asic_acl.srcFilterPortMask_,
					(asic_acl.srcFilterIgnoreL3L4_==TRUE? 2: (asic_acl.srcFilterIgnoreL4_ == 1? 1: 0))
				);
				rtlglue_printf("\tsip: %d.%d.%d.%d   sipM: %d.%d.%d.%d\n", (asic_acl.srcFilterIpAddr_>>24),
					((asic_acl.srcFilterIpAddr_&0x00ff0000)>>16), ((asic_acl.srcFilterIpAddr_&0x0000ff00)>>8),
					(asic_acl.srcFilterIpAddr_&0xff), (asic_acl.srcFilterIpAddrMask_>>24),
					((asic_acl.srcFilterIpAddrMask_&0x00ff0000)>>16), ((asic_acl.srcFilterIpAddrMask_&0x0000ff00)>>8),
					(asic_acl.srcFilterIpAddrMask_&0xff)
				);
				rtlglue_printf("\tsportL: %d   sportU: %d\n", asic_acl.srcFilterPortLowerBound_, asic_acl.srcFilterPortUpperBound_);
				break;

			case RTL8651_ACL_DSTFILTER:
				rtlglue_printf(" [%d] rule type: %s   rule action: %s\n", start, "Deatination Filter", actionT[asic_acl.actionType_]);
				rtlglue_printf("\tDMAC: %x:%x:%x:%x:%x:%x  DMACM: %x:%x:%x:%x:%x:%x\n", 
					asic_acl.dstFilterMac_.octet[0], asic_acl.dstFilterMac_.octet[1], asic_acl.dstFilterMac_.octet[2], 
					asic_acl.dstFilterMac_.octet[3], asic_acl.dstFilterMac_.octet[4], asic_acl.dstFilterMac_.octet[5],
					asic_acl.dstFilterMacMask_.octet[0], asic_acl.dstFilterMacMask_.octet[1], asic_acl.dstFilterMacMask_.octet[2],
					asic_acl.dstFilterMacMask_.octet[3], asic_acl.dstFilterMacMask_.octet[4], asic_acl.dstFilterMacMask_.octet[5]
				);
				rtlglue_printf("\tdvidx: %d   dvidxM: %x  ProtoType: %x   dportL: %d   dportU: %d\n",
					asic_acl.dstFilterVlanIdx_, asic_acl.dstFilterVlanIdxMask_, 
					(asic_acl.dstFilterIgnoreL3L4_==TRUE? 2: (asic_acl.dstFilterIgnoreL4_ == 1? 1: 0)), 
					asic_acl.dstFilterPortLowerBound_, asic_acl.dstFilterPortUpperBound_
				);
				rtlglue_printf("\tdip: %d.%d.%d.%d   dipM: %d.%d.%d.%d\n", (asic_acl.dstFilterIpAddr_>>24),
					((asic_acl.dstFilterIpAddr_&0x00ff0000)>>16), ((asic_acl.dstFilterIpAddr_&0x0000ff00)>>8),
					(asic_acl.dstFilterIpAddr_&0xff), (asic_acl.dstFilterIpAddrMask_>>24),
					((asic_acl.dstFilterIpAddrMask_&0x00ff0000)>>16), ((asic_acl.dstFilterIpAddrMask_&0x0000ff00)>>8),
					(asic_acl.dstFilterIpAddrMask_&0xff)
				);
				break;

			default: assert(0);

			}


			/* Action type */
			switch (asic_acl.actionType_) {

			case RTL8651_ACL_PERMIT: /* 0x00 */
			case RTL8651_ACL_REDIRECT: /* 0x01 */
			case RTL8651_ACL_CPU: /* 0x03 */
			case RTL8651_ACL_DROP: /* 0x02, 0x04 */
			case RTL8651_ACL_DROP_LOG: /* 0x05 */
			case RTL8651_ACL_MIRROR: /* 0x06 */
			case RTL8651_ACL_REDIRECT_PPPOE: /* 0x07 */
			case RTL8651_ACL_MIRROR_KEEP_MATCH: /* 0x09 */
				rtlglue_printf("\tdvidx: %d   hp: %d   pppoeIdx: %d   nxtHop:%d  ", asic_acl.dvid_, asic_acl.priority_,
					asic_acl.pppoeIdx_, asic_acl.nextHop_);
				break;

			case RTL8651_ACL_POLICY: /* 0x08 */
				rtlglue_printf("\thp: %d   nxtHopIdx: %d  ", asic_acl.priority_, asic_acl.nhIndex);
				break;

			case RTL8651_ACL_DROP_RATE_EXCEED_PPS: /* 0x0a */
			case RTL8651_ACL_LOG_RATE_EXCEED_PPS: /* 0x0b */
			case RTL8651_ACL_DROP_RATE_EXCEED_BPS: /* 0x0c */
			case RTL8651_ACL_LOG_RATE_EXCEED_BPS: /* 0x0d */
				rtlglue_printf("\trlIdx: %d  ", asic_acl.rlIndex);
				break;
			default: assert(0);
		
			}
			rtlglue_printf("pktOpApp: %d\n", asic_acl.pktOpApp);
			
#if 0
			rtlglue_printf("\tpkgopt: %x\n", asic_acl.pktOpApp);
			if (asic_acl.actionType_ < 8 || asic_acl.actionType_ == 9) {
				rtlglue_printf("\tdvidx: %d   hp: %d   pppoeIdx: %d   nxtHop:%d\n", asic_acl.dvid_, asic_acl.priority_,
					asic_acl.pppoeIdx_, asic_acl.nextHop_
				);
			} else if (asic_acl.actionType_ == 8) {
				rtlglue_printf("\thp: %d   nxtHopIdx: %d\n", asic_acl.priority_, asic_acl.nhIndex);
			} else {
				rtlglue_printf("\trlIdx: %d\n", asic_acl.rlIndex);
			}
#endif
		}

		if (outRule == FALSE) {
			start = asic_vlan.outAclStart; end = asic_vlan.outAclEnd;
			outRule = TRUE;
			goto again;
		}
	}
}
