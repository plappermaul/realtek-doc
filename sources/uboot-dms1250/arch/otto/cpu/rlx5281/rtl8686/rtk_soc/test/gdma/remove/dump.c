#include "types.h"
#include "assert.h"
#include "cle_utility.h"
#include "rtl8651_tblDrv.h"
#include "rtl8651_tblDrvFwd.h"
#include "rtl865xC_tblAsicDrv.h"
#include "rtl8651_tblDrvLocal.h"
#include "rtl8651_tblDrvFwdLocal.h"
#include "rtl8651_tblDrvProto.h"
#include "rtl8651_tblDrvStatistic.h"
#include "rtl8651_tblDrvPatch.h"
#include "rtl8651_alg_pptp.h"
#include "rtl8651_tblDrvStatistic.h"
#include "asicRegs.h"
#include "../swNic2.h"
#include "rtl_utils.h"
#include "utility.h"
#include "virtualMac.h" 
#include "vsv_conn.h"
/* dump all fields to one string */

void hs_displayHsb(hsb_param_t * hsbWatch)
{
	int32	i=0;
	
	rtlglue_printf("---------------------------- BEGIN HSB-----------------------------\n");
	switch(hsbWatch->type) {
		case 0:	rtlglue_printf("Packet Type Ethernet, "); break;
		case 1:	rtlglue_printf("Packet Type PPTP, "); break;
		case 2:	rtlglue_printf("Packet Type IP, "); break;
		case 3:	rtlglue_printf("Packet Type ICMP, "); break;
		case 4:	rtlglue_printf("Packet Type IGMP, "); break;
		case 5:	rtlglue_printf("Packet Type TCP, "); break;
		case 6:	rtlglue_printf("Packet Type UDP, "); break;
		default:	rtlglue_printf("Packet Unknown Type: %d",hsbWatch->type); break;
	}
	if(hsbWatch->spa<7)
		rtlglue_printf("source port: Physical port %d, ",hsbWatch->spa);
	else if(hsbWatch->extspa<3)
		rtlglue_printf("source port: Extension port %d, ",hsbWatch->extspa);
	else
		rtlglue_printf("source port: CPU, ");
	rtlglue_printf("bytecount: %d (0x%x)\n",hsbWatch->len,hsbWatch->len);
	rtlglue_printf("Layer2 Format %s",hsbWatch->vid ?"VLAN":"NoVLAN");
	if(hsbWatch->vid)
		rtlglue_printf(" ID: %d (0x%x)",hsbWatch->vid,hsbWatch->vid);
	rtlglue_printf(", %s, %s", hsbWatch->llcothr?"LLC Other":"NoLLC", hsbWatch->pppoeif?"PPPoE":"NoPPPoE");
	//if(hsbWatch->pppoesid)
//		rtlglue_printf("ID: %d (0x%x), ", hsbWatch->pppoesid, hsbWatch->pppoesid));
	rtlglue_printf("\n");
	
	// Protocol contents
	rtlglue_printf("DMAC: ");
	for (i=0; i<6; i++)
		rtlglue_printf("%02x%s", hsbWatch->da[i], i<5?"-":"");
	rtlglue_printf(", ");
	
	rtlglue_printf("SMAC: ");
	for (i=0; i<6; i++)
		rtlglue_printf("%02x%s", hsbWatch->sa[i], i<5?"-":"");
	rtlglue_printf("\n");
	rtlglue_printf("%s, ",hsbWatch->dirtx?"DirectTX":"FromPHY");
	if(hsbWatch->dirtx) {
		rtlglue_printf("L3 Checksum 0x%x ", hsbWatch->sprt);
		rtlglue_printf("L4 Checksum 0x%x\n", hsbWatch->dprt);
		rtlglue_printf("Reason %x ", hsbWatch->iptos);
		if((hsbWatch->ethtype>>3)&0x1) 
			rtlglue_printf("Add PPPoE Header ID: %d\n", hsbWatch->ethtype&0x7);
//		if((hsbWatch->pppoesid>>3)&0x1)
	//		rtlglue_printf("Add VLAN Header ID: %d\n", hsbWatch->pppoesid&0x7);
//		else
	//		rtlglue_printf("\n");
		rtlglue_printf("Specify output port: ");
//		for(i=0;i<6;i++)
	//		if((hsbWatch->pppoesid>>5)&(1<<i))
		//		rtlglue_printf("%d ",i);
//		if(hsbWatch->pppoesid>>11)	
	//		rtlglue_printf("CPU\n");
		//else
//			rtlglue_printf("\n");
			

//		if(hsbWatch->spa== 7 && hsbWatch->extspa) {//Only from extension port, the cpu_l2 has meaning
//			if(hsbWatch->c)
	//			rtlglue_printf("CPU Send with Layer2 action");
		//	else
			//	rtlglue_printf("CPU Send with Multi-layer action");
		//}
	}
	else {
#if 0		
		rtlglue_printf("ethertype: %d (0x%x)",hsbWatch->ethtype,hsbWatch->ethtype);		
		rtlglue_printf("sip: ");
		for (i=0; i<4; i++)
			rtlglue_printf("%d%s", hsbWatch->sip[i], i<3?".":"");
		rtlglue_printf(", ");
		
		rtlglue_printf("sprt (ICMP ID, L3 Checksum): %d 0x%x\n",(int)hsbWatch->sprt,(int)hsbWatch->sprt);
		
		rtlglue_printf("dip: ");
		for (i=0; i<4; i++)
			rtlglue_printf("%d%s", hsbWatch->dip[i], i<3?".":"");
		rtlglue_printf(", ");
			
		rtlglue_printf("dprt: %d 0x%x \n",hsbWatch->dprt,hsbWatch->dprt);
		
		rtlglue_printf("ip protocol (ICMP, IGMP Type): %d 0x%x, ",(int)hsbWatch->ipptl,(int)hsbWatch->ipptl);
		rtlglue_printf("ip flag: %d 0x%x, ",hsbWatch->ipflg,hsbWatch->ipflg);
		rtlglue_printf("ip tos: %d 0x%x\n",hsbWatch->iptos,hsbWatch->iptos);
		rtlglue_printf("tcp flag: %d 0x%x, ",hsbWatch->tcpflg,hsbWatch->tcpflg);
		
		rtlglue_printf("PatternMatch: %d, ",hsbWatch->prtnmat);
	       
		rtlglue_printf("udp_nocs: %d\n",hsbWatch->udp_nocs);
		if(hsbWatch->ttlst == 2)
			rtlglue_printf("TTL > 1,"));
		else
			rtlglue_printf("TTL %x, ",hsbWatch->ttlst);
		rtlglue_printf("pktend: %d, ",hsbWatch->pktend);

		rtlglue_printf("Packet priority is  %d ",hsbWatch->hiprior);
		rtlglue_printf(" L3CSOK:%d, L4CSOK:%d, IPFRAGIF:%d",hsbWatch->l3crcok,hsbWatch->l4crcok,hsbWatch->ipfragif);
		rtlglue_printf("\n"));

		rtlglue_printf("LLC_Other_IF: %d, ",hsbWatch->llc_other);
		rtlglue_printf("URL_Trap: %d\n",hsbWatch->urlmch);
#endif			
	}

		
}


void hs_displayHsa_S(hsa_param_t *hsaWatch)
{
	struct in_addr addr;
	rtlglue_printf(("HSA("));
//	rtlglue_printf("\tmac:%02x-%02x-%02x-%02x-%02x-%02x\n",hsaWatch->nhmac[5],hsaWatch->nhmac[4],hsaWatch->nhmac[3],hsaWatch->nhmac[2],hsaWatch->nhmac[1],hsaWatch->nhmac[0]);
	rtlglue_printf("\tmac:%02x-%02x-%02x-%02x-%02x-%02x\n",hsaWatch->nhmac[0],hsaWatch->nhmac[1],hsaWatch->nhmac[2],hsaWatch->nhmac[3],hsaWatch->nhmac[4],hsaWatch->nhmac[5]);

	addr.s_addr =ntohl( hsaWatch->trip);
	rtlglue_printf("\ttrip:%s(hex:%08x)",inet_ntoa(addr),hsaWatch->trip);	
	rtlglue_printf("\tprt:%d\tipmcast:%d\n",hsaWatch->port,hsaWatch->ipmcastr);
	rtlglue_printf("\tl3cs:%d",hsaWatch->l3csdt);
	rtlglue_printf("\tl4cs:%d",hsaWatch->l4csdt);
	rtlglue_printf("\tInternal NETIF:%d",hsaWatch->egif);
	rtlglue_printf("\tl2tr:%d,\n ",hsaWatch->l2tr);
	rtlglue_printf("\tl34tr:%d",hsaWatch->l34tr);
	rtlglue_printf("\tdirtx:%d",hsaWatch->dirtxo);
	rtlglue_printf("\ttype:%d",hsaWatch->typeo);
	rtlglue_printf("\tsnapo:%d",hsaWatch->snapo);
	rtlglue_printf("\twhy2cpu 0x%x (%d)\n",hsaWatch->why2cpu,hsaWatch->why2cpu);
	rtlglue_printf("\tpppif:%d",hsaWatch->pppoeifo);
	rtlglue_printf("\tpppid:%d",hsaWatch->pppidx);
	rtlglue_printf("\tttl_1:0x%x",hsaWatch->ttl_1if);
	rtlglue_printf("\tdpc:%d,",hsaWatch->dpc);

	rtlglue_printf("\tleno:%d(0x%x)\n",hsaWatch->leno,hsaWatch->leno);

	rtlglue_printf("\tl3CrcOk:%d",hsaWatch->l3csoko);
	rtlglue_printf("\tl4CrcOk:%d",hsaWatch->l4csoko);
	rtlglue_printf("\tfrag:%d",hsaWatch->frag);
	rtlglue_printf("\tlastFrag:%d\n",hsaWatch->lastfrag);



	rtlglue_printf("\tsvid:0x%x",hsaWatch->svid);
	rtlglue_printf("\tdvid:%d(0x%x)",hsaWatch->dvid,hsaWatch->dvid);
	rtlglue_printf("\tdestination interface :%d\n",hsaWatch->difid);
	rtlglue_printf("\trxtag:%d",hsaWatch->rxtag);
	rtlglue_printf("\tdvtag:0x%x",hsaWatch->dvtag);
	rtlglue_printf("\tspa:%d",hsaWatch->spao);
	rtlglue_printf("\tdpext:0x%x\thwfwrd:%d\n",hsaWatch->dpext,hsaWatch->hwfwrd);
	rtlglue_printf("\tspcp:%d",hsaWatch->spcp);
	rtlglue_printf("\tpriority:%d",hsaWatch->priority);
	
	rtlglue_printf("\tdp:0x%x\n",hsaWatch->dp);
	rtlglue_printf(")\n");
	
#ifndef VERI_PLATEFORM
#ifdef RTL8651B
	swTableCmd_parseCpursn(hsaWatch->cpursn | (hsaWatch->cpursn2<<9));
#endif /*RTL8651B*/
#endif
}


void hs_displayHsb_S(hsb_param_t * hsbWatch)
{
	struct in_addr addr;
	rtlglue_printf("HSB(");
	rtlglue_printf("\ttype:%d",hsbWatch->type);
	
	rtlglue_printf("\tspa:%d",hsbWatch->spa);
	rtlglue_printf("\tlen:%d",hsbWatch->len);
	rtlglue_printf("\tvid :%d\n",hsbWatch->vid);
	rtlglue_printf("\tpppoe:%d",hsbWatch->pppoeif);
	
	// Protocol contents
	rtlglue_printf("\ttagif:%d\tpppoeId:%d",hsbWatch->tagif,hsbWatch->pppoeid);
	rtlglue_printf("\tethrtype:0x%04x\n",hsbWatch->ethtype);
	rtlglue_printf("\tllc_other:%d\tsnap:%d\n",hsbWatch->llcothr,hsbWatch->snap);
	rtlglue_printf("\tda:%02x-%02x-%02x-%02x-%02x-%02x",hsbWatch->da[0],hsbWatch->da[1],hsbWatch->da[2],hsbWatch->da[3],hsbWatch->da[4],hsbWatch->da[5]);
	rtlglue_printf("\tsa:%02x-%02x-%02x-%02x-%02x-%02x\n",hsbWatch->sa[0],hsbWatch->sa[1],hsbWatch->sa[2],hsbWatch->sa[3],hsbWatch->sa[4],hsbWatch->sa[5]);
	
	//v32 = (hsbWatch->ethrtype15_12 << 12) | hsbWatch->ethrtype11_0;

	addr.s_addr =ntohl( hsbWatch->sip);
	rtlglue_printf("\tsip:%s(hex:%08x)   ",inet_ntoa(addr),hsbWatch->sip);
	rtlglue_printf("\tsprt:%d (hex:%x)\n ",(int)hsbWatch->sprt,hsbWatch->sprt);
	addr.s_addr = ntohl(hsbWatch->dip);
	rtlglue_printf("\tdip:%s(hex:%08x) ",inet_ntoa(addr),hsbWatch->dip);;		
	rtlglue_printf("\tdprt:%d(hex:%08x)\n",hsbWatch->dprt,hsbWatch->dprt);
	
	rtlglue_printf("\tipptl:%d,",(int)hsbWatch->ipptl);
	rtlglue_printf("\tipflg:%d,",hsbWatch->ipfg);
	rtlglue_printf("\tiptos:%d,",hsbWatch->iptos);
	rtlglue_printf("\ttcpflg:%d\n",hsbWatch->tcpfg);
	
	rtlglue_printf("\tdirtx:%d,",hsbWatch->dirtx);
	rtlglue_printf("\tprtnmat:%d",hsbWatch->patmatch);
       
	rtlglue_printf("\tudp_nocs:%d",hsbWatch->udpnocs);
	rtlglue_printf("\tttlst:0x%x\n",hsbWatch->ttlst);

	
	rtlglue_printf("\thp:%d",hsbWatch->hiprior);
	rtlglue_printf("\tl3csok:%d\tl4csok:%d\tipfragif:%d\n",hsbWatch->l3csok,hsbWatch->l4csok,hsbWatch->ipfo0_n);
	
//#ifdef RTL8651B
	rtlglue_printf("\textspa:%d",hsbWatch->extspa);
	rtlglue_printf("\turlmch:%d\n)\n",hsbWatch->urlmch);
//#endif /*RTL8651B*/

}
int dump_pbvlan(void)
{


	int32 ruleNo, port;

	rtlglue_printf("\n<<Protocol-Based VLAN Table>>\n\n");

	for( ruleNo = RTL8651_PBV_RULE_USR1; ruleNo <= RTL8651_PBV_RULE_USR2; ruleNo++ )
	{
		uint8 ProtocolType;
		uint16 ProtocolValue;
		
		rtlglue_printf("Rule %d : ", ruleNo);
		rtl8651_queryProtocolBasedVLAN( ruleNo, &ProtocolType, &ProtocolValue );
		
		if ( ProtocolType == 0x0 && ProtocolValue == 0x0000 )
		{
			rtlglue_printf("(Not Used)\n");
		}
		else
		{
			switch( ProtocolType )
			{
				case 0: rtlglue_printf( "EthernetII, Type:" ); break;
				case 1: rtlglue_printf( "RFC-1042  , Type:" ); break;
				case 2: rtlglue_printf( "LLC-Other , {DSAP,SSAP}:" ); break;
				case 3: rtlglue_printf( "Reserved" ); break;
				default:break;
			}
			rtlglue_printf( "0x%04x\n", ProtocolValue );
		}
	}

	rtlglue_printf("\nProtocol      P0 P1 P2 P3 P4 P5 E0 E1 E2\n");
	for( ruleNo = RTL8651_PBV_RULE_IPX; ruleNo <= RTL8651_PBV_RULE_USR2; ruleNo++  )
	{
		switch( ruleNo )
		{
			case 1: rtlglue_printf("%-12s  ", "IPX"); break;
			case 2: rtlglue_printf("%-12s  ", "NetBIOS"); break;
			case 3: rtlglue_printf("%-12s  ", "PPPoEControl"); break;
			case 4: rtlglue_printf("%-12s  ", "PPPoESession"); break;
			case 5: rtlglue_printf("%-12s  ", "Rule 5"); break;
			case 6: rtlglue_printf("%-12s  ", "Rule 6"); break;
		}

		for( port = 0; port < RTL8651_PORT_NUMBER + 3; port++ )
		{
			uint8 valid;
			uint32 vlanIdx;
			rtl8651_getProtocolBasedVLAN( ruleNo, port, &valid, &vlanIdx );

			if ( valid )
				rtlglue_printf( " %d ", vlanIdx&0xFFF);
			else
				rtlglue_printf( " - " );
		}
		rtlglue_printf("\n");
	}
	rtlglue_printf("\n");
	return SUCCESS;


}
void dump_l2(void)
{
		rtl865x_tblAsicDrv_l2Param_t asic_l2;
 		uint32 row, col, port, m=0;
		rtlglue_printf(">>ASIC L2 Table:\n");
		for(row=0x0; row<RTL8651_L2TBL_ROW; row++)
			for(col=0; col<RTL8651_L2TBL_COLUMN; col++) {
				memset((void*)&asic_l2,0,sizeof(asic_l2));
				if (rtl8651_getAsicL2Table(row, col, &asic_l2) == FAILED)
					continue;					
				if (asic_l2.isStatic && asic_l2.ageSec==0 && asic_l2.cpu && asic_l2.memberPortMask == 0 &&asic_l2.auth==0)
					continue;
				rtlglue_printf("%4d.[%3d,%d] %02x:%02x:%02x:%02x:%02x:%02x FID:%x mbr(",m, row, col, 
						asic_l2.macAddr.octet[0], asic_l2.macAddr.octet[1], asic_l2.macAddr.octet[2], 
						asic_l2.macAddr.octet[3], asic_l2.macAddr.octet[4], asic_l2.macAddr.octet[5],asic_l2.fid
				);
				m++;
				for(port=0; port<RTL8651_PORT_NUMBER+3; port++){
					if(asic_l2.memberPortMask & (1<<port))
						rtlglue_printf("%d ", port);
				}
				rtlglue_printf(")");
				rtlglue_printf("%s %s %s %s age:%d ",asic_l2.cpu?"CPU":"FWD", asic_l2.isStatic?"STA":"DYN",  asic_l2.srcBlk?"BLK":"", asic_l2.nhFlag?"NH":"", asic_l2.ageSec);
				if(0==(asic_l2.macAddr.octet[0]&1)&&asic_l2.memberPortMask&peripheralExtPortMask){
					 rtl8651_tblDrv_filterDbTableEntry_t * entry;
					 uint8 queryMac[6];
					 memcpy(queryMac,&asic_l2.macAddr,6);
					 queryMac[5]=row^asic_l2.macAddr.octet[0]^asic_l2.macAddr.octet[1]^asic_l2.macAddr.octet[2]^asic_l2.macAddr.octet[3]^asic_l2.macAddr.octet[4];
					entry=_rtl8651_getVlanFilterDatabaseEntry(0, (ether_addr_t *)queryMac) ;
					if (entry)
						rtlglue_printf("W%d",entry->linkId );
				}
				if (asic_l2.auth)
					rtlglue_printf("AUTH:%d",asic_l2.auth);				
				rtlglue_printf("\n");
		}
	

}
void dump_l3(void)
{
	rtl865x_tblAsicDrv_routingParam_t asic_l3;
	int8 *str[] = { "PPPoE", "L2", "ARP", " ", "CPU", "NxtHop", "DROP", " " };
	int8 *strNetType[] = { "WAN", "DMZ", "LAN",  "RLAN"};
	uint32 idx, mask;
	int netIdx;

	rtlglue_printf(">>L3 Routing Table:\n");
	for(idx=0; idx<RTL8651_ROUTINGTBL_SIZE; idx++) {
		if (rtl8651_getAsicRouting(idx, &asic_l3) == FAILED) {
			rtlglue_printf("\t[%d]  (Invalid)\n", idx);
			continue;
		}
		if (idx == RTL8651_ROUTINGTBL_SIZE-1)
			mask = 0;
		else for(mask=32; !(asic_l3.ipMask&0x01); asic_l3.ipMask=asic_l3.ipMask>>1)
				mask--;
		netIdx = asic_l3.internal<<1|asic_l3.DMZFlag;
		rtlglue_printf("\t[%d]  %d.%d.%d.%d/%d process(%s) %s \n", idx, (asic_l3.ipAddr>>24),
			((asic_l3.ipAddr&0x00ff0000)>>16), ((asic_l3.ipAddr&0x0000ff00)>>8), (asic_l3.ipAddr&0xff), 
			mask, str[asic_l3.process],strNetType[netIdx]
		);
		switch(asic_l3.process) 
		{
		case 0x00:	/* PPPoE */
			rtlglue_printf("\t           dvidx(%d)  pppidx(%d) nxthop(%d)\n", asic_l3.vidx, asic_l3.pppoeIdx, (asic_l3.nextHopRow<<2)+asic_l3.nextHopColumn);
			break;
			
		case 0x01:	/* L2 */
			rtlglue_printf("              dvidx(%d) nexthop(%d)\n", asic_l3.vidx, (asic_l3.nextHopRow<<2)+asic_l3.nextHopColumn);
			break;

		case 0x02:	/* ARP */
			rtlglue_printf("             dvidx(%d) ARPSTA(%d) ARPEND(%d) IPIDX(%d)\n", asic_l3.vidx, asic_l3.arpStart, asic_l3.arpEnd, asic_l3.arpIpIdx);
			break;

		case 0x03:	/* Reserved */
			assert(0);

		case 0x04:	/* CPU */
			rtlglue_printf("             dvidx(%d)\n", asic_l3.vidx);
			break;

		case 0x05:	/* NAPT Next Hop */
			rtlglue_printf("              NHSTA(%d) NHNUM(%d) NHNXT(%d) NHALGO(%d) IPDOMAIN(%d)\n", asic_l3.nhStart,
				asic_l3.nhNum, asic_l3.nhNxt, asic_l3.nhAlgo, asic_l3.ipDomain);
			break;

		case 0x06:	/* DROP */
			rtlglue_printf("             dvidx(%d)\n", asic_l3.vidx);
			break;

		case 0x07:	/* Reserved */
			/* pass through */
		default: assert(0);
		}
	}

}
void dump_arp(void)
{
	rtl865x_tblAsicDrv_routingParam_t asic_l3;
	rtl865x_tblAsicDrv_arpParam_t asic_arp;
	rtl865x_tblAsicDrv_l2Param_t asic_l2;

	uint32	i, j, port;
	ipaddr_t ipAddr;
	int8 ipBuf[sizeof"255.255.255.255"];

	
	rtlglue_printf(">>Arp Table:\n");
	for(i=0; i<RTL8651_ARPTBL_SIZE; i++) {
		if (rtl8651_getAsicArp(i,  &asic_arp) == FAILED)
			continue;
		for(j=0; j<RTL8651_ROUTINGTBL_SIZE; j++) {
			if (rtl8651_getAsicRouting(j, &asic_l3) == FAILED || asic_l3.process!= 2)
				continue;
			if(asic_l3.arpStart <= (i>>3) &&  (i>>3) <= asic_l3.arpEnd) {
				ipAddr = (asic_l3.ipAddr & asic_l3.ipMask) + (i - (asic_l3.arpStart<<3));
				if(rtl8651_getAsicL2Table_Patch(asic_arp.nextHopRow, asic_arp.nextHopColumn, &asic_l2) == FAILED){
					inet_ntoa_r(ntohl(ipAddr), ipBuf);
					rtlglue_printf("FAILED %-16s [%3d,%d] ", ipBuf, asic_arp.nextHopRow, asic_arp.nextHopColumn);
				}else {
					inet_ntoa_r(ntohl(ipAddr), ipBuf);
					rtlglue_printf("%03d : %-16s %02x-%02x-%02x-%02x-%02x-%02x (",i, ipBuf, asic_l2.macAddr.octet[0], asic_l2.macAddr.octet[1], asic_l2.macAddr.octet[2], asic_l2.macAddr.octet[3], asic_l2.macAddr.octet[4], asic_l2.macAddr.octet[5]);
					for(port=0; port< RTL8651_PORT_NUMBER+rtl8651_totalExtPortNum; port++){
						if(asic_l2.memberPortMask& (1<<port))
							rtlglue_printf("%d ", port);
						else
							rtlglue_printf("  ");
					}							
					rtlglue_printf(") %us", asic_arp.aging);
				}
				continue;
			}
		}
		rtlglue_printf(" ARP:%3d  L2:%3d,%d\n", i, asic_arp.nextHopRow, asic_arp.nextHopColumn);

	}



}
void dump_pppoe(void)
{
	rtl865x_tblAsicDrv_pppoeParam_t asic_pppoe;
	int32	i;

	rtlglue_printf(">>PPPOE Table:\n");
	for(i=0; i<RTL8651_PPPOETBL_SIZE; i++) {
		memset(&asic_pppoe,0,sizeof(asic_pppoe));
		if (rtl8651_getAsicPppoe(i,  &asic_pppoe) == FAILED)
			continue;
		rtlglue_printf("\t[%d]  sessionID(%d)  ageSec(%d)\n", i, asic_pppoe.sessionId, asic_pppoe.age);
	}

}


void dump_ip(void)
{
	rtl865x_tblAsicDrv_extIntIpParam_t asic_ip;
	int32	i;
	int8 intIpBuf[sizeof"255.255.255.255"];
	int8 extIpBuf[sizeof"255.255.255.255"];

	rtlglue_printf(">>IP Table:\n");
	for(i=0; i<RTL8651_IPTABLE_SIZE; i++) {
		if (rtl8651_getAsicExtIntIpTable(i,  &asic_ip) == FAILED) {
			rtlglue_printf("  [%d] (Invalid)\n", i);
			continue;
		}
		inet_ntoa_r(ntohl(asic_ip.intIpAddr), intIpBuf);
		inet_ntoa_r(ntohl(asic_ip.extIpAddr),extIpBuf);			
		rtlglue_printf("  [%d] intip(%-14s) extip(%-14s) type(%s) nhIdx(%d) \n",
				i, intIpBuf,extIpBuf,
				(asic_ip.localPublic==TRUE? "LP" : (asic_ip.nat==TRUE ? "NAT" : "NAPT")), asic_ip.nhIndex);
	}

}
void dump_serverp(void)
{
	rtl865x_tblAsicDrv_serverPortParam_t asic_sp;
	int32 i, entry;
	int8 intIpBuf[sizeof"255.255.255.255"];
	int8 extIpBuf[sizeof"255.255.255.255"];
	int8 protocol[][10]={"VALID","TCP","UDP","BOTH"};

	rtlglue_printf(">>ServerPort Table:\n");
	for(entry=0, i=0; i<RTL8651_SERVERPORTTBL_SIZE; i++) {
		if (rtl8651_getAsicServerPortTable(i,  &asic_sp) == FAILED) {
			rtlglue_printf("  [%d] (Invalid)\n", i);
			continue;
		}	
		inet_ntoa_r(ntohl(asic_sp.intIpAddr),intIpBuf);
		inet_ntoa_r(ntohl(asic_sp.extIpAddr),extIpBuf);			
		rtlglue_printf("  [%d] extIP(%-16s) extPort(%d) isRange(%d) \n", i, extIpBuf, asic_sp.extPort,asic_sp.portRange );
		rtlglue_printf("      intIP(%-16s) intPort(%d) protocol %s  Traffic:%d", intIpBuf, asic_sp.intPort,protocol[asic_sp.protocol],asic_sp.traffic);
		if (asic_sp.pvaild)
			rtlglue_printf("  Priority %d\n", asic_sp.pid);				
		else
			rtlglue_printf("\n");
		entry++;
	}
	rtlglue_printf("total entry: %d\n", entry);

}
void dump_alg(void)
{
	rtl865x_tblAsicDrv_algParam_t asic_alg;
	int32	i, entry;
	int8		direction[][10]={"VALID","LAN2WAN","WAN2LAN","BOTH"};


	rtlglue_printf(">>ALG Table:\n");
	for(entry=0, i=0; i<RTL865XC_ALGTBL_SIZE; i++) {
		if (rtl8651_getAsicAlg(i, &asic_alg) == SUCCESS) {
			rtlglue_printf("\t[%d] port(%d) direction:%s \n", i, asic_alg.port,direction[asic_alg.direction]);
			entry++;
		}
	}
	rtlglue_printf("total entry: %d\n", entry);

}
void dump_acl(void)
{
		int8 *actionT[] = { "", "permit", "drop", "cpu", "drop log", 
						"drop notify", 	"redirect to ethernet","redirect to pppoe", "mirror", "mirro keep match", 
						"drop rate exceed pps", "log rate exceed pps", "drop rate exceed bps", "log rate exceed bps","policy ",
						"priority selection"};
		_rtl8651_tblDrvAclRule_t asic_acl;
		rtl865x_tblAsicDrv_intfParam_t asic_intf;
		uint32 start, end;

		uint16 vid;
		int8 outRule;


				
		rtlglue_printf(">>ASIC ACL Table:\n\n");
		for(vid=0; vid<8; vid++ ) {
			/* Read VLAN Table */
			if (rtl8651_getAsicNetInterface(vid, &asic_intf) == FAILED)
				continue;
			if (asic_intf.valid==FALSE)
				continue;

			outRule = FALSE;
			start = asic_intf.inAclStart; end = asic_intf.inAclEnd;
	again:
			if (outRule == FALSE)
				rtlglue_printf("\n<<Ingress Rule for Netif  %d: (VID %d)>>\n", vid,asic_intf.vid);
			else rtlglue_printf("\n<<Egress Rule for Netif %d (VID %d)>>:\n", vid,asic_intf.vid);
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
				case RTL8652_ACL_IP_RANGE:
					rtlglue_printf(" [%d] rule type: %s   rule action: %s\n", start, "IP Range", actionT[asic_acl.actionType_]);
					rtlglue_printf("\tdipU: %d.%d.%d.%d dipL: %d.%d.%d.%d\n", (asic_acl.dstIpAddr_>>24),
						((asic_acl.dstIpAddr_&0x00ff0000)>>16), ((asic_acl.dstIpAddr_&0x0000ff00)>>8),
						(asic_acl.dstIpAddr_&0xff), (asic_acl.dstIpAddrMask_>>24), ((asic_acl.dstIpAddrMask_&0x00ff0000)>>16),
						((asic_acl.dstIpAddrMask_&0x0000ff00)>>8), (asic_acl.dstIpAddrMask_&0xff)
					);
					rtlglue_printf("\tsipU: %d.%d.%d.%d sipL: %d.%d.%d.%d\n", (asic_acl.srcIpAddr_>>24),
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


				case RTL8652_ACL_ICMP_IPRANGE:
					rtlglue_printf(" [%d] rule type: %s   rule action: %s\n", start, "ICMP IP RANGE", actionT[asic_acl.actionType_]);
					rtlglue_printf("\tdipU: %d.%d.%d.%d dipL: %d.%d.%d.%d\n", (asic_acl.dstIpAddr_>>24),
						((asic_acl.dstIpAddr_&0x00ff0000)>>16), ((asic_acl.dstIpAddr_&0x0000ff00)>>8),
						(asic_acl.dstIpAddr_&0xff), (asic_acl.dstIpAddrMask_>>24), ((asic_acl.dstIpAddrMask_&0x00ff0000)>>16),
						((asic_acl.dstIpAddrMask_&0x0000ff00)>>8), (asic_acl.dstIpAddrMask_&0xff)
					);
					rtlglue_printf("\tsipU: %d.%d.%d.%d sipL: %d.%d.%d.%d\n", (asic_acl.srcIpAddr_>>24),
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


				case RTL8652_ACL_IGMP_IPRANGE:
					rtlglue_printf(" [%d] rule type: %s   rule action: %s\n", start, "IGMP IP RANGE", actionT[asic_acl.actionType_]);
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

				case RTL8652_ACL_TCP_IPRANGE:
					rtlglue_printf(" [%d] rule type: %s   rule action: %s\n", start, "TCP IP RANGE", actionT[asic_acl.actionType_]);
					rtlglue_printf("\tdipU: %d.%d.%d.%d dipL: %d.%d.%d.%d\n", (asic_acl.dstIpAddr_>>24),
						((asic_acl.dstIpAddr_&0x00ff0000)>>16), ((asic_acl.dstIpAddr_&0x0000ff00)>>8),
						(asic_acl.dstIpAddr_&0xff), (asic_acl.dstIpAddrMask_>>24), ((asic_acl.dstIpAddrMask_&0x00ff0000)>>16),
						((asic_acl.dstIpAddrMask_&0x0000ff00)>>8), (asic_acl.dstIpAddrMask_&0xff)
					);
					rtlglue_printf("\tsipU: %d.%d.%d.%d sipL: %d.%d.%d.%d\n", (asic_acl.srcIpAddr_>>24),
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
					rtlglue_printf(" [%d] rule type: %s   rule action: %s\n", start,"UDP", actionT[asic_acl.actionType_]);
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


				case RTL8652_ACL_UDP_IPRANGE:
					rtlglue_printf(" [%d] rule type: %s   rule action: %s\n", start, "UDP IP RANGE", actionT[asic_acl.actionType_]);
					rtlglue_printf("\tdipU: %d.%d.%d.%d dipL: %d.%d.%d.%d\n", (asic_acl.dstIpAddr_>>24),
						((asic_acl.dstIpAddr_&0x00ff0000)>>16), ((asic_acl.dstIpAddr_&0x0000ff00)>>8),
						(asic_acl.dstIpAddr_&0xff), (asic_acl.dstIpAddrMask_>>24), ((asic_acl.dstIpAddrMask_&0x00ff0000)>>16),
						((asic_acl.dstIpAddrMask_&0x0000ff00)>>8), (asic_acl.dstIpAddrMask_&0xff)
					);
					rtlglue_printf("\tsipU: %d.%d.%d.%d sipL: %d.%d.%d.%d\n", (asic_acl.srcIpAddr_>>24),
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


				case RTL8652_ACL_SRCFILTER_IPRANGE:
					rtlglue_printf(" [%d] rule type: %s   rule action: %s\n", start, "Source Filter(IP RANGE)", actionT[asic_acl.actionType_]);
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
					rtlglue_printf("\tsipU: %d.%d.%d.%d   sipL: %d.%d.%d.%d\n", (asic_acl.srcFilterIpAddr_>>24),
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
				case RTL8652_ACL_DSTFILTER_IPRANGE:
					rtlglue_printf(" [%d] rule type: %s   rule action: %s\n", start, "Deatination Filter(IP Range)", actionT[asic_acl.actionType_]);
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
					rtlglue_printf("\tdipU: %d.%d.%d.%d   dipL: %d.%d.%d.%d\n", (asic_acl.dstFilterIpAddr_>>24),
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

			case RTL8651_ACL_PRIORITY: /* 0x08 */
				rtlglue_printf("\tprioirty: %d   ", asic_acl.priority) ;
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
				start = asic_intf.outAclStart; end = asic_intf.outAclEnd;
				outRule = TRUE;
				goto again;
			}

	}
}
void dump_vlan(void)
{
	int i,j;
	rtl8651_totalExtPortNum=3;
	rtlglue_printf(">>ASIC VLAN Table:\n\n");
		for(i=0; i<RTL865XC_VLAN_NUMBER; i++) {
			rtl865x_tblAsicDrv_vlanParam_t vlan;

			if (rtl8651_getAsicVlan(i,&vlan ) == FAILED)
				continue;
			rtlglue_printf("  VID[%d] ", i);			
			rtlglue_printf("\n\tmember ports:");				
			for(j=0; j<RTL8651_PORT_NUMBER+rtl8651_totalExtPortNum; j++)
				if(vlan.memberPortMask & (1<<j))
						rtlglue_printf("%d ", j);
			rtlglue_printf("\n\tUntag member ports:");				
				for(j=0; j<RTL8651_PORT_NUMBER+rtl8651_totalExtPortNum; j++)
					if(vlan.untagPortMask & (1<<j))
						rtlglue_printf("%d ", j);
					

			rtlglue_printf("\n\tFID:\t%d\n",vlan.fid);

		}

}
void dump_netif(void)
{
		int8	*pst[] = { "DIS/BLK",  "LIS", "LRN", "FWD" };
		uint8	*mac;
		int32	i, j;

		rtlglue_printf(">>ASIC Netif Table:\n\n");
		for(i=0; i<RTL865XC_NETIFTBL_SIZE; i++) {
			rtl865x_tblAsicDrv_intfParam_t intf;
			rtl865x_tblAsicDrv_vlanParam_t vlan;

			if (rtl8651_getAsicNetInterface( i, &intf ) == FAILED)
				continue;

			if (intf.valid)
			{
				mac = (uint8 *)&intf.macAddr.octet[0];
				rtlglue_printf("%d-vid[%d] %x:%x:%x:%x:%x:%x", 
					i, intf.vid, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
				rtlglue_printf("  L3/4 HW acc %s \n",
					intf.enableRoute==TRUE? "enabled": "disabled" );
				rtlglue_printf("      ingress ");
				if(RTL8651_ACLTBL_DROP_ALL<=intf.inAclStart){
					if(intf.inAclStart==RTL8651_ACLTBL_PERMIT_ALL)
						rtlglue_printf("permit all,");
					if(intf.inAclStart==RTL8651_ACLTBL_ALL_TO_CPU)
						rtlglue_printf("all to cpu,");
					if(intf.inAclStart==RTL8651_ACLTBL_DROP_ALL)
						rtlglue_printf("drop all,");
				}else
					rtlglue_printf("ACL %d-%d, ",intf.inAclStart, intf.inAclEnd);
				rtlglue_printf("  egress ");
				if(RTL8651_ACLTBL_DROP_ALL<=intf.outAclStart){
					if(intf.outAclStart==RTL8651_ACLTBL_PERMIT_ALL)
						rtlglue_printf("permit all,");
					if(intf.outAclStart==RTL8651_ACLTBL_ALL_TO_CPU)
						rtlglue_printf("all to cpu,");
					if(intf.outAclStart==RTL8651_ACLTBL_DROP_ALL)
						rtlglue_printf("drop all,");
				}else
					rtlglue_printf("ACL %d-%d, ",intf.outAclStart, intf.outAclEnd);
				rtlglue_printf("\n      %d MAC Addresses, MTU %d Bytes\n", intf.macAddrNumber, intf.mtu);

				rtl8651_getAsicVlan( intf.vid, &vlan );
				rtlglue_printf("\n      Untag member ports:");
				for(j=0; j<RTL8651_PORT_NUMBER+3; j++)
					if(vlan.untagPortMask & (1<<j))
						rtlglue_printf("%d ", j);
				rtlglue_printf("\n      Active member ports:");
				for(j=0; j<RTL8651_PORT_NUMBER+3; j++)
					if(vlan.memberPortMask & (1<<j))
						rtlglue_printf("%d ", j);
				
				rtlglue_printf("      Port state(");
				for(j=0; j<RTL8651_PORT_NUMBER+rtl8651_totalExtPortNum; j++)
				{
					if((vlan.memberPortMask & (1<<j))==0)
						continue;
					if ((( READ_MEM32( PCRP0+j*4 )&STP_PortST_MASK)>>STP_PortST_OFFSET ) > 4 )
						rtlglue_printf("--- ");
					else
						rtlglue_printf("%d:%s ", j, pst[(( READ_MEM32( PCRP0+j*4 )&STP_PortST_MASK)>>STP_PortST_OFFSET )]);

				}
				rtlglue_printf(")\n\n");
			}

	}
}
void dump_nexthop(void)
{
	rtl865x_tblAsicDrv_nextHopParam_t asic_nxthop;

	uint32 idx, refcnt, rt_flag;

	rtlglue_printf(">>ASIC Next Hop Table:\n");
	for(idx=0; idx<RTL8651_NEXTHOPTBL_SIZE; idx++) {
		refcnt = rt_flag = 0;
		if (rtl8651_getAsicNextHopTable(idx, &asic_nxthop) == FAILED)
			continue;
		rtlglue_printf("  [%d]  type(%s) IPIdx(%d) dstVid(%d) pppoeIdx(%d) nextHop(%d) rf(%d) rt(%d)\n", idx,
			(asic_nxthop.isPppoe==TRUE? "pppoe": "ethernet"), asic_nxthop.extIntIpIdx, 
			asic_nxthop.dvid, asic_nxthop.pppoeIdx, (asic_nxthop.nextHopRow<<2)+asic_nxthop.nextHopColumn, refcnt, rt_flag);
	}
}
void dump_pvid(void)
{

	uint32 vidp[9];
	int32  i;
	for(i=0; i<RTL8651_PORT_NUMBER+rtl8651_totalExtPortNum; i++)
		rtl8651_getAsicPvid(i, &vidp[i]);
	rtlglue_printf(">> PVID Reg:\n");
	for(i=0; i<RTL8651_PORT_NUMBER+rtl8651_totalExtPortNum; i++)
		rtlglue_printf("p%d: %d,", i, vidp[i]);
	rtlglue_printf("\n");


}
void dump_mcast(void)
{
	rtl865x_tblAsicDrv_multiCastParam_t asic;
	uint32 entry;
	
	rtlglue_printf(">>ASIC Multicast Table:\n");
	for(entry=0; entry<RTL8651_MULTICASTTBL_SIZE; entry++) {
		if (rtl8651_getAsicIpMulticastTable(entry, &asic) != SUCCESS) {
			rtlglue_printf("\t[%d]  (Invalid Entry)\n", entry);
			continue;
		}
		rtlglue_printf("\t[%d]  Mip(%d.%d.%d.%d) sip(%d.%d.%d.%d) MBR(0x%x)\n", entry,
			asic.dip>>24, (asic.dip&0x00ff0000)>>16, (asic.dip&0x0000ff00)>>8, (asic.dip&0xff), 
			asic.sip>>24, (asic.sip&0x00ff0000)>>16, (asic.sip&0x0000ff00)>>8, (asic.sip&0xff),
			asic.mbr);
		rtlglue_printf("\t       svid:%d, spa:%d, EXtIP:%d, age:%d, cpu:%d\n", asic.svid, asic.port, asic.extIdx,
			asic.age, asic.cpu);
	}
}

void dump_icmp(void)
{
	uint32 i;
	rtl865x_tblAsicDrv_naptIcmpParam_t naptIcmp;
	uint32 total = 0;
	rtlglue_printf(">>ICMP Table:\n");
	for(i=0; i<32; i++) {
		if (rtl8651_getAsicNaptIcmpTable(i, &naptIcmp) == FAILED)
			continue;

		if (naptIcmp.isValid ||naptIcmp.isStatic) {
			rtlglue_printf("[%02d]GId(%04x) %s Local %d.%d.%d.%d-(ID) %04x %s %s %s %s %s Age:%d\n",
					i, naptIcmp.offset, naptIcmp.isPptp?"PPTP": "ICMP",
					(naptIcmp.insideLocalIpAddr>>24)&0xff, (naptIcmp.insideLocalIpAddr>>16)&0xff, (naptIcmp.insideLocalIpAddr>>8)&0xff, naptIcmp.insideLocalIpAddr&0xff,
					naptIcmp.insideLocalId, 
					naptIcmp.direction==0?"B":naptIcmp.direction==1?"O":naptIcmp.direction==2?"I":"B",
					naptIcmp.isCollision? "Col":"",
					naptIcmp.isStatic? "STA": "", 
					naptIcmp.isSpi? "SPI": "", 
					naptIcmp.isValid? "VAL":"",
					naptIcmp.ageSec
					);
			total++;
		}
	}
	rtlglue_printf("Total entry: %u\n", total);

}

void dump_hs(void)
{
	hsb_param_t hsb_r;
	hsa_param_t hsa_r;
	memset((void*)&hsb_r,0,sizeof(hsb_r));
	memset((void*)&hsa_r,0,sizeof(hsa_r));
	virtualMacGetHsb( &hsb_r );	
	hs_displayHsb_S(&hsb_r);
	virtualMacGetHsa( &hsa_r );	
	hs_displayHsa_S(&hsa_r);

}
void dump_tcpudp(void)
{
	rtl865x_tblAsicDrv_naptTcpUdpParam_t asic_tcpudp;
	uint32 idx, entry=0;

	rtlglue_printf(">>ASIC NAPT TCP/UDP Table:\n");

	for(idx=0; idx<RTL8651_TCPUDPTBL_SIZE; idx++) {
		if (rtl8651_getAsicNaptTcpUdpTable(idx, &asic_tcpudp) == FAILED)
			continue;

		if (asic_tcpudp.isValid == 1 || asic_tcpudp.isDedicated == 1 ) {
			rtlglue_printf("[%4d] %d.%d.%d.%d:%d {V,D}={%d,%d} col1(%d) col2(%d) static(%d) tcp(%d)\n",
			       idx,
			       asic_tcpudp.insideLocalIpAddr>>24, (asic_tcpudp.insideLocalIpAddr&0x00ff0000) >> 16,
			       (asic_tcpudp.insideLocalIpAddr&0x0000ff00)>>8, asic_tcpudp.insideLocalIpAddr&0x000000ff,
			       asic_tcpudp.insideLocalPort, 
			       asic_tcpudp.isValid, asic_tcpudp.isDedicated,
			       asic_tcpudp.isCollision, asic_tcpudp.isCollision2, asic_tcpudp.isStatic, asic_tcpudp.isTcp );

			rtlglue_printf("   age(%d) offset(%d) tcpflag(%d) SelEIdx(%d) SelIPIdx(%d) priValid:%d pri(%d)\n",
			        asic_tcpudp.ageSec, asic_tcpudp.offset<<10, asic_tcpudp.tcpFlag, 
			        asic_tcpudp.selEIdx, asic_tcpudp.selExtIPIdx,asic_tcpudp.priValid,asic_tcpudp.priority );
			entry++;
		}
	}
	rtlglue_printf("Total entry: %d\n", entry);
}

void tmpFunc(void)
{
#if defined(VSV)
	enum IC_TYPE ictype;
	model_getTestTarget( &ictype );
	switch (ictype)
	{
		case IC_TYPE_REAL:
			break;
		case IC_TYPE_MODEL:			
			 break;

	break;

		default:
			rtlglue_printf("   WARNING ....................................\n" );			
	}
#endif
}
int32	_rtl8651_dumpCmd(uint32 userId,  int32 argc,int8 **saved){
	int8 *nextToken;
	int32 size;
	enum IC_TYPE ictype;
	model_getTestTarget( &ictype );
	rtlglue_printf(" Current State :" );
	switch (ictype)
	{
		case IC_TYPE_REAL:
			rtlglue_printf("   IC ....................................\n" );			
			break;
		case IC_TYPE_MODEL:			
			rtlglue_printf("   MODEL ....................................\n" );			
			break;

		default:
			rtlglue_printf("   WARNING ....................................\n" );			
	}
		
	cle_getNextCmdToken(&nextToken,&size,saved); 
	if ( 0 )
	{
		// never been here ....
		// just for 'else if' statements ....
	}
	else if (strcmp(nextToken, "icmp") == 0) {
		dump_icmp();
		
	}
	else if(strcmp(nextToken, "hs") == 0) 
	{
		dump_hs();
	}	
	else if(strcmp(nextToken, "l2") == 0) {
		dump_l2();
	}
	else if(strcmp(nextToken, "l3") == 0) {
		dump_l3();
	}
	else if(strcmp(nextToken, "arp") == 0) {
		dump_arp();
	}
	else if(strcmp(nextToken, "pppoe") == 0) {
		dump_pppoe();
	}
	else if(strcmp(nextToken, "ip") == 0) {
		dump_ip();
	}
	else if(strcmp(nextToken, "serverp") == 0) {
		dump_serverp();
	}
	else if(strcmp(nextToken, "alg") == 0) {
		dump_alg();
	}
	else if(strcmp(nextToken, "pvid") == 0) {
		dump_pvid();
	}
	else if(strcmp(nextToken, "nexthop") == 0) {
		dump_nexthop();
	}
	else if(strcmp(nextToken, "multicast") == 0) {		
		dump_mcast();
	}
	else if(strcmp(nextToken, "netif") == 0) {
		dump_netif();
	}
	else if(strcmp(nextToken, "vlan") == 0) {
		dump_vlan();
	}
	else if(strcmp(nextToken, "acl") == 0) {
		dump_acl();
	}

	else if(strcmp(nextToken, "pbvlan") == 0) {
		dump_pbvlan();
	}
	else if(strcmp(nextToken, "napt") == 0) {
		dump_tcpudp();		
	}
	else if(strcmp(nextToken, "nic") == 0) {
		swNic_dumpPkthdrDescRing();
		swNic_dumpMbufDescRing();
		swNic_dumpTxDescRing();
	}	

	return 0;
}

