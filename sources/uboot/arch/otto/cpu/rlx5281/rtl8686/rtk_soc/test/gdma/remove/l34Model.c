/*
* Copyright c                  Realtek Semiconductor Corporation, 2002  
* All rights reserved.
* 
* Program : Source File for Layer3/4 Model Code
* Abstract : 
* Author : Louis Yung-Chieh Lo (yjlou@realtek.com.tw)               
* $Id: l34Model.c,v 1.48 2007-04-23 08:01:58 michaelhuang Exp $
*/

#include <rtl_glue.h>
#include <rtl_utils.h>
#include "assert.h"
#include "hsModel.h"
#include "asicRegs.h"
#include "icModel.h"
#include "l2Model.h"
#include "aclModel.h"
#include "l34Model.h"
#include "naptModel.h"
#include "modelTrace.h"
#include "rtl865xC_tblAsicDrv.h"
#include "rtl8651_tblDrvLocal.h"

#define RTL8651_TBLDRV_LOCAL_H
#include "rtl8651_aclLocal.h"
const int8 str[][12] = { "PPPoE", "L2", "ARP", "Invalid ", "CPU", "NxtHop", "DROP", " Invalid" };
static char msg[120];
static uint32 RoudRobinCounter;
static rtl865xc_tblAsic_l3RouteTable_t asic_l3[RTL8651_ROUTINGTBL_SIZE];			
static rtl8651_tblAsic_extIpTable_t asic_ipt[RTL8651_IPTABLE_SIZE];
static rtl865xc_tblAsic_srvPortTable_t   asic_serverp[RTL8651_SERVERPORTTBL_SIZE];
static rtl865xc_tblAsic_nextHopTable_t  asic_nexthop;
static uint16 routingSipIdx,routingDipIdx,asic_ipt_idx;

#define PPPOE_HDRLEN 8
#define MORE_FRAG 2
#define ALLPORTMASK 0x1FF
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

/* check valid hostType */
#define _RTL8651_HOSTTYPE_ISVALID(type) \
	(((type) >= _RTL8651_HOST_NPI) && ((type) < _RTL8651_HOST_MAX))

/* =========== decide NAT behavior ===========
   \dst  |
src \    | NPI  NI   LP   NPE  NE   RP
--------------------------------------------
    NPI  | FW   FW   FW   CPU  CPU  NAPT 
    NI   | FW   FW   FW   CPU  CPU  NAT 
    LP   | FW   FW   FW   CPU  CPU  FW 
    RP   | DROP DROP FW   NAPTR NATR WAN? */



/* decision */
enum _RTL8651_DECISIONTYPE {
	_RTL8651_DEC_NONE = 0,
/* <---- L3 ----> */
	_RTL8651_DEC_RT_LR,		/* L3 Routing -- Local Public to Remote Host */
	_RTL8651_DEC_RT_RL,		/* L3 Routing -- Remote host to Local Public */
	_RTL8651_DEC_RT_LNP,		/* L3 Routing -- Local Public to NAPT Internal */
	_RTL8651_DEC_RT_NPL,		/* L3 Routing -- NAPT Internal to Local Public */
	_RTL8651_DEC_RT_LN,		/* L3 Routing -- Local Public to NAT Internal */
	_RTL8651_DEC_RT_NL,		/* L3 Routing -- NAT Internal to Local Public */
	_RTL8651_DEC_RT_LL,		/* L3 Routing -- Local Public to Local Public */
	_RTL8651_DEC_RT_NN,		/* L3 Routing -- NAT Internal to NAT Internal */
	_RTL8651_DEC_RT_NPNP,		/* L3 Routing -- NAPT Internal to NAPT Internal */
	_RTL8651_DEC_RT_NPN,		/* L3 Routing -- NAPT Internal to NAT Internal */
	_RTL8651_DEC_RT_NNP,		/* L3 Routing -- NAT Internal to NAPT Internal */
	_RTL8651_DEC_RT_RR,		/* L3 Routing -- Remote Public to Remote Public (Wan Side routing) */
/* <---- L4 ----> */
	_RTL8651_DEC_NT,			/* L4 NAT - outbound */
	_RTL8651_DEC_NTR,			/* L4 NAT - inbound */
	_RTL8651_DEC_NTBC_NP,		/* port bouncing -- NAPT Internal to NE */
	_RTL8651_DEC_NTBC_N,		/* port bouncing -- NAT Internal to NE */
	_RTL8651_DEC_NTBC_L,		/* port bouncing -- Local Public to NE */
	_RTL8651_DEC_NPT,			/* L4 NAPT - outbound */
	_RTL8651_DEC_NPTR,		/* L4 NAPT - inbound */
	_RTL8651_DEC_NPTBC_NP,	/* port bouncing -- NAPT Internal to NPE */
	_RTL8651_DEC_NPTBC_N,		/* port bouncing -- NAT Internal to NPE */
	_RTL8651_DEC_NPTBC_L,		/* port bouncing -- Local Public to NPE */
/* <---- Other ----> */
	_RTL8651_DEC_DP,		/* decide to drop packet */
	/* =============================================================================== */
	_RTL8651_DEC_MAX
};

/* get the operation layer of decision */
#define _RTL8651_OPTLAYER(decision)	\
	(((decision) >= _RTL8651_DEC_RT_LR) && ((decision) <= _RTL8651_DEC_RT_RR))?	\
		(_RTL8651_OPERATION_LAYER3): \
		((((decision) >= _RTL8651_DEC_NT) && ((decision) <= _RTL8651_DEC_NPTBC_L))?	\
			(_RTL8651_OPERATION_LAYER4): \
			(_RTL8651_OPERATION_OTHER))

static uint8 _RTL8651_PROC[_RTL8651_HOST_MAX][_RTL8651_HOST_MAX] = {
/*						_RTL8651_HOST_NPI		_RTL8651_HOST_NI		_RTL8651_HOST_LP		_RTL8651_HOST_RP	_RTL8651_HOST_NPE		_RTL8651_HOST_NE		*/
/* _RTL8651_HOST_NPI */	{ _RTL8651_DEC_RT_NPNP,	_RTL8651_DEC_RT_NPN,	_RTL8651_DEC_RT_NPL,	_RTL8651_DEC_NPT,	_RTL8651_DEC_NPTBC_NP,	_RTL8651_DEC_NTBC_NP},
/* _RTL8651_HOST_NI  */	{ _RTL8651_DEC_RT_NNP,	_RTL8651_DEC_RT_NN,		_RTL8651_DEC_RT_NL,		_RTL8651_DEC_NT,	_RTL8651_DEC_NPTBC_N,	_RTL8651_DEC_NTBC_N},
/* _RTL8651_HOST_LP  */	{ _RTL8651_DEC_RT_LNP,	_RTL8651_DEC_RT_LN,		_RTL8651_DEC_RT_LL,		_RTL8651_DEC_RT_LR,	_RTL8651_DEC_NPTBC_L,	_RTL8651_DEC_NTBC_L},
/* _RTL8651_HOST_RP */	{_RTL8651_DEC_DP,		_RTL8651_DEC_DP,		_RTL8651_DEC_RT_RL,		_RTL8651_DEC_RT_RR,	_RTL8651_DEC_NPTR,		_RTL8651_DEC_NTR},
/* _RTL8651_HOST_NPE */	{_RTL8651_DEC_DP,		_RTL8651_DEC_DP,		_RTL8651_DEC_DP,		_RTL8651_DEC_DP,	_RTL8651_DEC_DP,		_RTL8651_DEC_DP},
/* _RTL8651_HOST_NE */	{_RTL8651_DEC_DP,		_RTL8651_DEC_DP,		_RTL8651_DEC_DP,		_RTL8651_DEC_DP,	_RTL8651_DEC_DP,		_RTL8651_DEC_DP}
};





/*
@func enum MODEL_RETURN_VALUE | modelAlgCheck	| model code for Alg Check
@parm hsb_param_t* | hsb | Header Stamp of Before (for input)
@parm hsa_param_t* | hsa | Header Stamp of After (for output)
@rvalue enum MODEL_RETURN_VALUE |
@comm 
*/
enum MODEL_RETURN_VALUE modelAlgCheck( hsb_param_t* hsb, hsa_param_t* hsa,int LanToWan, ale_data_t *ale)
{
	rtl865x_tblAsicDrv_algParam_t AlgEntry;
	uint16 algport,dport,sport,type;
	int i;
	for (i=0;i<RTL865XC_ALGTBL_SIZE;i++)
	{
		if (rtl8651_getAsicAlg(i, &AlgEntry)!=SUCCESS)
			continue;
		algport = AlgEntry.port;
		dport=hsb->dprt;
		sport=hsb->sprt;
		type=hsb->type;
		if ((algport != dport) && (algport!=sport))
			continue;		
		if (AlgEntry.direction==ALG_DIRECTION_BOTH)
		{
			if ( (type==HSB_TYPE_TCP) && (AlgEntry.protocol==ALG_PROTOCOL_TCP1||AlgEntry.protocol==ALG_PROTOCOL_TCP2||AlgEntry.protocol==ALG_PROTOCOL_BOTH))
			{
				if ((algport==dport)  || (algport==sport)) 
				{
					MT_TOCPU("MATCH ALG ENTRY PACKET SHOULD TO CPU  ");
					modelCPUport(hsb,  hsa,ale);
					return MRET_TOCPU;
				}
			}
			if ( (type==HSB_TYPE_UDP) && (AlgEntry.protocol==ALG_PROTOCOL_UDP||AlgEntry.protocol==ALG_PROTOCOL_BOTH))
			{
					if ((algport==dport)  || (algport==sport)) 
					{
						MT_TOCPU("MATCH ALG ENTRY PACKET SHOULD TO CPU  ");
						modelCPUport(hsb,  hsa,ale);
						return MRET_TOCPU;
					}
					
			}
		}
		if (LanToWan)
		{
			if (AlgEntry.direction==ALG_DIRECTION_LANTOWAN)
			{
				if ( (type==HSB_TYPE_TCP) && (AlgEntry.protocol==ALG_PROTOCOL_TCP1||AlgEntry.protocol==ALG_PROTOCOL_TCP2||AlgEntry.protocol==ALG_PROTOCOL_BOTH))
				{
					if (dport==algport)
					{
						MT_TOCPU("MATCH ALG ENTRY PACKET SHOULD TO CPU  ");
						modelCPUport(hsb,  hsa,ale);
						return MRET_TOCPU;
					}
				}
				if ( (type==HSB_TYPE_UDP) && (AlgEntry.protocol==ALG_PROTOCOL_UDP ||AlgEntry.protocol==ALG_PROTOCOL_BOTH))
				{
					if (dport==algport) 
					{
						MT_TOCPU("MATCH ALG ENTRY PACKET SHOULD TO CPU  ");
						modelCPUport(hsb,  hsa,ale);
						return MRET_TOCPU;
					}
				}
				

			}
			if (AlgEntry.direction==ALG_DIRECTION_WANTOLAN)		
			{
				if ( (type==HSB_TYPE_TCP) && (AlgEntry.protocol==ALG_PROTOCOL_TCP1||AlgEntry.protocol==ALG_PROTOCOL_TCP2||AlgEntry.protocol==ALG_PROTOCOL_BOTH))
				{
					if (sport==algport) 
					{
						MT_TOCPU("MATCH ALG ENTRY PACKET SHOULD TO CPU  ");
						modelCPUport(hsb,  hsa,ale);
						return MRET_TOCPU;
					}
				}
				if ( (type==HSB_TYPE_UDP) && (AlgEntry.protocol==ALG_PROTOCOL_UDP||AlgEntry.protocol==ALG_PROTOCOL_BOTH))
				{
					if (sport==algport) 
					{
						MT_TOCPU("MATCH ALG ENTRY PACKET SHOULD TO CPU  ");
						modelCPUport(hsb,  hsa,ale);
						return MRET_TOCPU;
					}
				}
			}
		}
		else
		{
			if (AlgEntry.direction==ALG_DIRECTION_LANTOWAN)
			{
				if ( (type==HSB_TYPE_TCP) && (AlgEntry.protocol==ALG_PROTOCOL_TCP1||AlgEntry.protocol==ALG_PROTOCOL_TCP2||AlgEntry.protocol==ALG_PROTOCOL_BOTH))
				{
					if (sport==algport) 
					{
						MT_TOCPU("MATCH ALG ENTRY PACKET SHOULD TO CPU  ");
						modelCPUport(hsb,  hsa,ale);
						return MRET_TOCPU;
					}
				}
				if ( (type==HSB_TYPE_UDP) && (AlgEntry.protocol==ALG_PROTOCOL_UDP||AlgEntry.protocol==ALG_PROTOCOL_BOTH))
				{
					if (sport==algport) 
					{
						MT_TOCPU("MATCH ALG ENTRY PACKET SHOULD TO CPU  ");
						modelCPUport(hsb,  hsa,ale);
						return MRET_TOCPU;
					}
				}

			}
			if (AlgEntry.direction==ALG_DIRECTION_WANTOLAN)		
			{
				if ( (type==HSB_TYPE_TCP) && (AlgEntry.protocol==ALG_PROTOCOL_TCP1||AlgEntry.protocol==ALG_PROTOCOL_TCP2||AlgEntry.protocol==ALG_PROTOCOL_BOTH))
				{
					if (dport==algport) 
					{
						MT_TOCPU("MATCH ALG ENTRY PACKET SHOULD TO CPU  ");
						modelCPUport(hsb,  hsa,ale);
						return MRET_TOCPU;
					}
				}
				if ( (type==HSB_TYPE_UDP) && (AlgEntry.protocol==ALG_PROTOCOL_UDP||AlgEntry.protocol==ALG_PROTOCOL_BOTH))
				{
					if (dport==algport) 
					{
						MT_TOCPU("MATCH ALG ENTRY PACKET SHOULD TO CPU  ");
						modelCPUport(hsb,  hsa,ale);
						return MRET_TOCPU;
					}
				}
			}
		
		}

	}
	return MRET_OK;
}

void	modelModifyDA(hsb_param_t * hsb,hsa_param_t *hsa, int nextHop)
{
	rtl865xc_tblAsic_l2Table_t   asic_l2;

	_rtl8651_readAsicEntry(TYPE_L2_SWITCH_TABLE,nextHop, &asic_l2);							
	hsa->nhmac[0]=asic_l2.mac47_40;
	hsa->nhmac[1]=asic_l2.mac39_24>>8;
	hsa->nhmac[2]=asic_l2.mac39_24&0xff;
	hsa->nhmac[3]=asic_l2.mac23_8>>8;
	hsa->nhmac[4]=asic_l2.mac23_8&0xff;
	hsa->nhmac[5]=hsa->nhmac[0] ^hsa->nhmac[1]^hsa->nhmac[2]^hsa->nhmac[3]^hsa->nhmac[4] ^ (nextHop>>2) ^fidHash[asic_l2.fid];				
	hsa->dp=asic_l2.memberPort;
	if (asic_l2.extMemberPort)
	{
		hsa->dpext=asic_l2.extMemberPort;				
		hsa->dp |= PM_PORT_NOTPHY;
	}
	if (asic_l2.toCPU)
		modelAddCPUport( hsb, hsa);
}

/*
@func uint8 | modelIPRouting	| IP routing
@parm uint32  | ipaddr | ip address for routing
@rvalue uint8 |
@comm 
*/

uint8 modelIPRouting(uint32 ipaddr)
{

	uint32 mask;
	uint8 index,maxIndx=0,i; 
	for (index=0,maxIndx=RTL8651_ROUTINGTBL_SIZE-1; index < RTL8651_ROUTINGTBL_SIZE;index++)			
	{
		if(asic_l3[index].linkTo.ARPEntry.valid == 0)
			continue;
		for(i=0, mask = 0; i<=asic_l3[index].linkTo.ARPEntry.IPMask; i++)
		{
			mask |= 1<<(31-i);
		}	
		if (index==RTL8651_ROUTINGTBL_SIZE-1) mask=0;
		if( (ipaddr & mask ) == (asic_l3[index].IPAddr & mask))
		{				
		      
			if (mask > asic_l3[maxIndx].linkTo.ARPEntry.IPMask)
				maxIndx=index;
		}
	}
	return maxIndx;
}	
/*
@func int32 | modelLaery3Action	| modify hsa for l3 action
@parm hsb_param_t* | hsb | Header Stamp of Before (for input)
@parm hsa_param_t* | hsa | Header Stamp of After (for output)
@parm PORT_MASK | [port mask  | do l3 action on assigned port
@rvalue int32 |	 
@comm 
*/
int32  modelLayer3Action( hsb_param_t* hsb, hsa_param_t* hsa,enum PORT_MASK portmask)
{
	hsa->l2tr=1;
	hsa->ttl_1if=portmask;
	return 0;
}


/*
@func uint8 | modelDIPmatchNATIP	| DIP whether NAT IP
@@parm hsb_param_t* | hsb | Header Stamp of Before (for input)
@parm hsa_param_t* | hsa | Header Stamp of After (for output)
@rvalue uint32 | 1 : true 0:false
@comm 
*/
uint32  modelDIPmatchNAPTIP(hsb_param_t* hsb, hsa_param_t* hsa)
{	
	uint16 i; 
	uint32 ipaddr=hsb->dip;
	for (i =0;i< RTL8651_IPTABLE_SIZE;i++)			
	{
		if (asic_ipt[i].valid==0)
			continue;		
		if ( (asic_ipt[i].externalIP==ipaddr) &&  (asic_ipt[i].isLocalPublic==0 )&&(asic_ipt[i].isOne2One==1))
		{
			hsa->trip=asic_ipt[i].internalIP;
			return 1;	
		}
	}
	return 0;
}	


/*
@func uint8 | modelSIPmatchNATIP	| SIP whether NAT IP
@@parm hsb_param_t* | hsb | Header Stamp of Before (for input)
@parm hsa_param_t* | hsa | Header Stamp of After (for output)
@rvalue uint32 | 1 : true 0:false
@comm 
*/
uint32  modelSIPmatchNAPTIP(hsb_param_t* hsb, hsa_param_t* hsa)
{	
	uint16 i; 
	uint32 ipaddr=hsb->sip;
	for (i =0;i< RTL8651_IPTABLE_SIZE;i++)			
	{
		if (asic_ipt[i].valid==0)
			continue;		
		if ( (asic_ipt[i].externalIP==ipaddr) &&  (asic_ipt[i].isLocalPublic==0 )&&(asic_ipt[i].isOne2One==1))
		{
			hsa->trip=asic_ipt[i].externalIP;
			return 1;	
		}
	}
	return 0;
}	




/*
@func uint8 | modelDIPmatchLP	| IP whether Local Public
@@parm hsb_param_t* | hsb | Header Stamp of Before (for input)
@parm hsa_param_t* | hsa | Header Stamp of After (for output)
@rvalue uint32 | 1 : true 0:false
@comm 
*/
uint32  modelDIPmatchLP(hsb_param_t* hsb, hsa_param_t* hsa)
{	
	uint16 i; 
	uint32 ipaddr=hsb->dip;
	for (i =0;i< RTL8651_IPTABLE_SIZE;i++)			
	{
		if (asic_ipt[i].valid==0)
			continue;		
		if ( (asic_ipt[i].externalIP==ipaddr) &&  asic_ipt[i].isLocalPublic)
		{
			hsa->trip=hsb->dip;
			hsa->port=hsb->dprt;
			return 1;	
		}
	}
	return 0;
}	

/*
@func uint8 | modelSIPmatchLP	| IP whether Local Public
@parm hsb_param_t* | hsb | Header Stamp of Before (for input)
@parm hsa_param_t* | hsa | Header Stamp of After (for output)

@rvalue uint32 | 1 : true 0:false
@comm 
*/
uint32  modelSIPmatchLP(hsb_param_t* hsb, hsa_param_t* hsa)
{	
	uint16 i; 
	uint32 ipaddr=hsb->sip;
	for (i =0;i< RTL8651_IPTABLE_SIZE;i++)			
	{
		if (asic_ipt[i].valid==0)
			continue;		
		if ((asic_ipt[i].internalIP==ipaddr) &&  asic_ipt[i].isLocalPublic)
		{
			hsa->trip=hsb->dip;
			hsa->port=hsb->dprt;
			return TRUE;	
		}
	}
	return 0;
}	

uint32  modelLookupLP(ipaddr_t ipaddr)
{	
	uint16 i; 
	for (i =0;i< RTL8651_IPTABLE_SIZE;i++)			
	{
		if (asic_ipt[i].valid==0)
			continue;		
		if ((asic_ipt[i].internalIP==ipaddr) &&  asic_ipt[i].isLocalPublic)
		{
			return TRUE;	
		}
	}
	return 0;
}	

void modelNatTransDstIpAddr(hsb_param_t *hsb,hsa_param_t *hsa,ale_data_t*ale)
{

	uint16 i; 
	uint32 ip=hsb->dip;
	for (i =0;i< RTL8651_IPTABLE_SIZE;i++)			
	{
		if (asic_ipt[i].valid==0)
			continue;		
		if ( (asic_ipt[i].externalIP==ip) &&  (asic_ipt[i].isLocalPublic==0 )&&(asic_ipt[i].isOne2One==1))
		{
			ale->doL4=1;
			hsa->trip=asic_ipt[i].internalIP;
			return;
		}
	}
	

}
void modelNatTransSrcIpAddr(hsb_param_t *hsb,hsa_param_t *hsa ,ale_data_t *ale)
{

	uint16 i; 
	uint32 ip=hsb->sip;
	for (i =0;i< RTL8651_IPTABLE_SIZE;i++)			
	{
		if (asic_ipt[i].valid==0)
			continue;		
		if ( (asic_ipt[i].internalIP==ip) &&  (asic_ipt[i].isLocalPublic==0 )&&(asic_ipt[i].isOne2One==1))
		{
			ale->doL4=1;
			hsa->trip=asic_ipt[i].externalIP;		
			return;
		}
	}
	

}

uint32 modelLookupNatExternalIpAddr(ipaddr_t ip)
{

	uint16 i; 
	for (i =0;i< RTL8651_IPTABLE_SIZE;i++)			
	{
		if (asic_ipt[i].valid==0)
			continue;		
		if ( (asic_ipt[i].externalIP==ip) &&  (asic_ipt[i].isLocalPublic==0 )&&(asic_ipt[i].isOne2One==1))
		{
			MT_WATCH("Match NAT External  Ip Addr ");	
			asic_ipt_idx=i;
			return TRUE;	
		}
	}
	return FALSE;

}


uint32 modelLookupNatInternalIpAddr(ipaddr_t ip)
{

	uint16 i; 
	for (i =0;i< RTL8651_IPTABLE_SIZE;i++)			
	{
		if (asic_ipt[i].valid==0)
			continue;		
		if ( (asic_ipt[i].internalIP==ip) &&  (asic_ipt[i].isLocalPublic==0 )&&(asic_ipt[i].isOne2One==1))
		{
			MT_WATCH("Match NAT  Internal Ip Addr ");	
			asic_ipt_idx=i;
			return TRUE;	
		}
	}
	return FALSE;

}
uint32 modelLookupNaptIpAddr(ipaddr_t ip)
{

	uint16 i; 
	for (i =0;i< RTL8651_IPTABLE_SIZE;i++)			
	{
		if (asic_ipt[i].valid==0)
			continue;		
		if ( (asic_ipt[i].externalIP==ip) &&  (asic_ipt[i].isLocalPublic==0 )&&(asic_ipt[i].isOne2One==0))
		{
			return TRUE;	
		}
	}
	return FALSE;

}
/*
@func enum MODEL_RETURN_VALUE | modelGREPaassthrough	| model code for L3 switching
@parm hsb_param_t* | hsb | Header Stamp of Before (for input)
@parm hsa_param_t* | hsa | Header Stamp of After (for output)
@rvalue enum MODEL_RETURN_VALUE |
@comm 
*/
enum MODEL_ACTION_VALUE modelGREPaassthrough( hsb_param_t* hsb, hsa_param_t* hsa, ale_data_t *ale )
{
	int tblIdx;
	rtl865xc_tblAsic_naptIcmpTable_t asic_icmp;
	rtl865xc_tblAsic_l3RouteTable_t 	asic_l3;
	rtl8651_tblAsic_extIpTable_t asic_ip;

	rtl8651_naptIcmpTableIndex(hsb->sip,hsb->sprt, hsb->dip,&tblIdx); 
	_rtl8651_readAsicEntry(TYPE_L4_ICMP_TABLE, tblIdx, &asic_icmp);
	
	switch(ale->decision)
	{
		case _RTL8651_DEC_NPT:	/* L4 NAPT - outbound */				
		case _RTL8651_DEC_NT:	/* L4 NAT - outbound */	 
			if((asic_icmp.valid) &&(asic_icmp.intIPAddr == hsb->sip) &&((asic_icmp.ICMPIDH<<15|asic_icmp.ICMPIDL) ==hsb->sprt) &&
				( (asic_icmp.dir==GRE_DIRECTION_BOTH) || (asic_icmp.dir==GRE_DIRECTION_LANTOWAN) ||
					(asic_icmp.dir==0)))
			{
				MT_WATCH("MATCH  GRE  LAN2WAN");			
				modelLayer3Action(hsb,hsa,ALLPORTMASK);
				hsa->l34tr=1;
				hsa->port=(asic_icmp.offsetH<<10) |(asic_icmp.offsetL);
				_rtl8651_readAsicEntry(TYPE_L3_ROUTING_TABLE, ale->rouitingSIPIdx, &asic_l3);
				_rtl8651_readAsicEntry(TYPE_EXT_INT_IP_TABLE, asic_l3.linkTo.ARPEntry.ARPIpIdx, &asic_ip);
				hsa->trip=asic_ip.externalIP;			
			    hsa->l3csdt =OCSUB(OCADD(hsb->sip>>16,hsb->sip&0xffff), OCADD(hsa->trip>>16, (hsa->trip & 0xffff)));		
				return MACT_L3ROUTING;				   
			}
			break;
		case _RTL8651_DEC_NPTR:	/* L4 NAPT - inbound */						
		case _RTL8651_DEC_NTR:	/* L4 NAT - inbound */ 
			if((asic_icmp.valid) && ((asic_icmp.offsetH<<10|asic_icmp.offsetL) == hsb->sprt)  &&
				( (asic_icmp.dir==GRE_DIRECTION_BOTH) || (asic_icmp.dir==GRE_DIRECTION_WANTOLAN) ||
					(asic_icmp.dir==0)))
			{
				MT_WATCH("MATCH  GRE  WAN2LAN");			
				modelLayer3Action(hsb,hsa,ALLPORTMASK);
				hsa->l34tr=1;
				hsa->port=(asic_icmp.ICMPIDH<<15) |(asic_icmp.ICMPIDL);
				hsa->trip=asic_icmp.intIPAddr;
			    hsa->l3csdt =OCSUB(OCADD(hsb->dip>>16,hsb->dip&0xffff), OCADD(hsa->trip>>16, (hsa->trip & 0xffff)));		
				return MACT_L3ROUTING;				
			}
			break;

	}
	MT_TOCPU("NO Match GRE Flow");			 
	return MRET_TOCPU;
}


static rtl865xc_tblAsic_ipMulticastTable_t asic_multi;
/*
@func enum MODEL_RETURN_VALUE | modelIPMulticastSwitching	| model code for IP multicast switching
@parm hsb_param_t* | hsb | Header Stamp of Before (for input)
@parm hsa_param_t* | hsa | Header Stamp of After (for output)
@rvalue enum MODEL_RETURN_VALUE |
@comm 
*/
enum MODEL_RETURN_VALUE modelIPMulticastSwitching( hsb_param_t* hsb, hsa_param_t* hsa, ale_data_t *ale )
{
	uint16 i,retval,mtu,pktMtu;
	uint32 mcast_addr;
	rtl865xc_tblAsic_vlanTable_t vlan;
	
	/* It is not necessary to check when Mac Based Mode */
	if ((ale->srcNetif.enableRoute==FALSE) && (ale->multiLayerMode!=LIMDBC_MAC))
	{
		MT_WATCH("Netif doesn't enable l3/4 route");
		retval=modelIngressACLCheck(hsb, hsa, ale);
		return retval;;
	}
	mcast_addr=(hsb->da[3]<<16)|(hsb->da[4]<<8)|(hsb->da[5]);		
	hsa->ipmcastr=1;

	/* The last entry is the SMAC when MAC MODE*/
	if (ale->multiLayerMode==LIMDBC_MAC)
		hsa->difid = RTL865XC_NETIFTBL_SIZE-1;
	else
		hsa->difid = ale->srcNetifIdx;

	if (mcast_addr != (hsb->dip& ~0xE0000000) && (hsb->pppoeif==0))
	{
		MT_TOCPU("IP Muliticast Address and DMAC mismatch");
		modelCPUport( hsb,hsa,ale);
		hsa->why2cpu = 1<<3;		/*ACL filter pass, but before checking DMAC=GMAC*/
		return MRET_TOCPU;
		
	}

	i = rtl8651_ipMulticastTableIndex(hsb->sip, hsb->dip);
	_rtl8651_readAsicEntry(TYPE_MULTICAST_TABLE, i, &asic_multi);
	if ((asic_multi.valid)&& (asic_multi.destIPAddrLsbs==(hsb->dip&0xfffffff) ) && (asic_multi.srcIPAddr==hsb->sip) )
	{
			if ((asic_multi.srcVidH<<4 | asic_multi.srcVidL)!=ale->aleInternalSvid)
			{
				sprintf(msg," Muliticast  SVID Dismatch Table : %d  Packet %d ",(asic_multi.srcVidH<<4|asic_multi.srcVidL),ale->aleInternalSvid);
				MT_DROP(msg);
				modelDrop(hsb,hsa,ale);
				return MRET_DROP;
			}
			/****************************************************************
				Check Source Port 
			*/
			if (hsb->spa==RTL8651_MAC_NUMBER)
			{
				if (asic_multi.srcPortExt!= hsb->extspa)
				{
					sprintf(msg," Muliticast  SPA Dismatch Table : %d  Packet %d ",(asic_multi.srcPort),hsb->spa);
					MT_DROP(msg);
					modelDrop(hsb,hsa,ale);
					return MRET_DROP;
				
				}

			}
			else
			{
				if (asic_multi.srcPort!= hsb->spa)
				{
					sprintf(msg," Muliticast  SPA Dismatch Table : %d  Packet %d ",(asic_multi.srcPort),hsb->spa);
					MT_DROP(msg);
					modelDrop(hsb,hsa,ale);
					return MRET_DROP;
				
				}
					
			}
			if ((asic_multi.srcVidH<<4 | asic_multi.srcVidL)!=ale->aleInternalSvid)
			{
				sprintf(msg," Muliticast  SVID Dismatch Table : %d  Packet %d ",(asic_multi.srcVidH<<4|asic_multi.srcVidL),ale->aleInternalSvid);
				MT_DROP(msg);
				modelDrop(hsb,hsa,ale);
				return MRET_DROP;
			}
			ale->doL3=1;
			_rtl8651_readAsicEntry(TYPE_EXT_INT_IP_TABLE,asic_multi.extIPIndex,&asic_ipt[0]);
			sprintf(msg,"MATCH IP Muliticast Table: %d ",i);
			MT_WATCH(msg);
			hsa->trip=asic_ipt[0].externalIP;
			hsa->l2tr=1;
			hsa->l34tr=1;
		       hsa->l3csdt =OCSUB(OCADD(hsb->sip>>16,hsb->sip&0xffff), OCADD(hsa->trip>>16, (hsa->trip & 0xffff)));
		       hsa->l4csdt =OCSUB(OCADD(hsb->sip>>16,hsb->sip&0xffff), OCADD(hsa->trip>>16, (hsa->trip & 0xffff)));       
		 	hsa->nhmac[0] = 0x1;	
			hsa->nhmac[1] = 0;	
		  	hsa->nhmac[2] = 0x5E;	
		  	hsa->nhmac[3] = (hsb->dip>>16)&0xFF;
		  	hsa->nhmac[4] = (hsb->dip>>8)&0xFF;
		  	hsa->nhmac[5] = hsb->dip&0xFF;
			hsa->dp=asic_multi.portList;
			hsa->dpext = asic_multi.extPortList;
			_rtl8651_readAsicEntry(TYPE_VLAN_TABLE, ale->aleInternalSvid, &vlan);
			for (i=0;i<RTL8651_MAC_NUMBER;i++)
			{
					if (((hsa->dp >>i)&0x1)==0)
						continue;
					if (((vlan.memberPort >>i) &0x1)==0)
						hsa->ttl_1if |= (1<<i);
			}
			for (i=0;i<3;i++)
			{
					if (((hsa->dpext >>i)&0x1)==0)
						continue;
					if (( (vlan.extMemberPort>>i) &0x1)  ==0)
						hsa->ttl_1if |= 1<<(i+RTL8651_MAC_NUMBER);
			}

			hsa->dp=asic_multi.portList;
			hsa->dpext=asic_multi.extPortList;
			if (asic_multi.extPortList)
			{			
				hsa->dp |= PM_PORT_NOTPHY;
			}

			if (asic_multi.toCPU)
			{
				hsa->dpext |= PM_PORT_CPU;
				hsa->why2cpu = 1<<3;		/*ACL filter pass, but before checking DMAC=GMAC*/
			}
			hsa->dvtag=0;
			hsa->port=hsb->sprt;
			if (hsb->pppoeif)
				hsa->pppoeifo=HSA_PPPOE_REMOVE_TAG;

	}
	retval=modelIngressACLCheck(hsb, hsa, ale);
	mtu=READ_VIR32(ALECR)& MultiCastMTU_MASK;
	pktMtu=hsb->len-18;/* DA(6)/SA(6)/etherType(2)/CRC(4) */	
	if (hsa->rxtag) pktMtu -=4;
	if (hsa->pppoeifo==2/*remove*/ ) pktMtu -= 8; /* If the ALE will remove pppoe header, we should minus pppoe header length. */
	sprintf(msg,"Asic MultiMTU %d, packet  DataLen %d",mtu,pktMtu);
	MT_WATCH(msg);	
	if (pktMtu>mtu)
	{

		modelCPUport( hsb, hsa,ale);
		sprintf(msg,"MTU %d, %x(h) packet len %d, pktMTU %d  type %d",mtu,mtu,hsb->len,pktMtu,hsb->type);
		MT_TOCPU(msg);
		modelCPUport( hsb, hsa,ale);
		hsa->why2cpu = 1<<3;		/*ACL filter pass, but before checking DMAC=GMAC*/
		return MACT_TOCPU;/* packet exceed MTU*/

	}
	if ( (ale->doL3==0) && (retval==MRET_OK))
	{
		modelCPUport( hsb, hsa,ale);
		MT_WATCH(" IP Multicast Lookup Miss!!!!!");
		return MRET_TOCPU;
	}		
	return retval;
	
}
/*
@func enum MODEL_RETURN_VALUE | modelHandleRedirect	| model code hande Layer2 Action
@parm hsb_param_t* | hsb | Header Stamp of Before (for input)
@parm hsa_param_t* | hsa | Header Stamp of After (for output)
@rvalue enum MODEL_RETURN_VALUE |
@comm 
*/
enum MODEL_RETURN_VALUE modelHandleRedirect( hsb_param_t* hsb, hsa_param_t* hsa, ale_data_t *ale,int idx  )
{
	rtl865x_tblAsicDrv_intfParam_t  *dstNetif;	  
	rtl865xc_tblAsic_l2Table_t   asic_l2;
	int32 retval;

	dstNetif = &ale->dstNetif;
	hsa->difid=ale->dstNetifIdx;
	_rtl8651_readAsicEntry(TYPE_L2_SWITCH_TABLE,idx, &asic_l2);			
	retval = rtl8651_getAsicNetInterface(ale->dstNetifIdx,dstNetif);
	if ( retval!=SUCCESS ) RTL_BUG( "rtl8651_getAsicNetInterface() returns NOT SUCCESS" );
	ale->matchDstNetif = TRUE;
	hsa->dvid= dstNetif->vid;	
	MT_WATCH(msg);
	sprintf(msg,"DVID %d MTU %d  %x(h)",hsa->dvid,dstNetif->mtu,dstNetif->mtu);
	MT_WATCH(msg);
	if (dstNetif->mtu<(hsb->len-16))
	{						
		MT_TOCPU("Packet Exceed MTU prcoess:Redirect");
		modelCPUport( hsb, hsa,ale);
		return MACT_TOCPU;/* packet exceed MTU*/
	}
	hsa->nhmac[0]=asic_l2.mac47_40;
	hsa->nhmac[1]=asic_l2.mac39_24>>8;
	hsa->nhmac[2]=asic_l2.mac39_24&0xff;
	hsa->nhmac[3]=asic_l2.mac23_8>>8;
	hsa->nhmac[4]=asic_l2.mac23_8&0xff;
	hsa->nhmac[5]=hsa->nhmac[0]^hsa->nhmac[1]^hsa->nhmac[2]^hsa->nhmac[3] ^hsa->nhmac[4] ^ (asic_nexthop.nextHop>>2) ^fidHash[asic_l2.fid];		
	hsa->dp=asic_l2.memberPort;
	if (asic_l2.extMemberPort)
	{
		hsa->dpext=asic_l2.extMemberPort;				
		hsa->dp |= PM_PORT_NOTPHY;
	}
	if (asic_l2.toCPU)
	modelAddCPUport( hsb, hsa);
	sprintf(msg," dp %x dpext %x",hsa->dp,hsa->dpext);
	MT_WATCH(msg);
	return MRET_OK;

}
/*
@func enum MODEL_RETURN_VALUE | modelHandlePPPoE	| model code hande nexthop action
@parm hsb_param_t* | hsb | Header Stamp of Before (for input)
@parm hsa_param_t* | hsa | Header Stamp of After (for output)
@parm ale_data_t* | ale |  ale (for input/output)
@rvalue enum MODEL_RETURN_VALUE |
@comm 
*/
enum MODEL_RETURN_VALUE modelHandlePPPoE( hsb_param_t* hsb, hsa_param_t* hsa, ale_data_t *ale )
{
	int nexthop;
	rtl865x_tblAsicDrv_intfParam_t  *dstNetif;	  
	rtl865xc_tblAsic_l2Table_t   asic_l2;
	nexthop=ale->nexthop;
	dstNetif = &ale->dstNetif;
	if(nexthop>=RTL8651_NEXTHOPTBL_SIZE)
		return MRET_TOCPU;				
	_rtl8651_readAsicEntry(TYPE_L2_SWITCH_TABLE,nexthop, &asic_l2);			
	rtl8651_getAsicNetInterface(ale->dstNetifIdx,dstNetif);
	ale->matchDstNetif = TRUE;
	hsa->difid=ale->dstNetifIdx;
	hsa->dvid= dstNetif->vid;	
	MT_WATCH("Handle PPPoE");
	hsa->pppidx = ale->pppoeIdx;
	if (hsb->pppoeif)
		hsa->pppoeifo=HSA_PPPOE_MODIFY;
	else
		hsa->pppoeifo=HSA_PPPOE_TAGGING;
	sprintf(msg,"DVID %d MTU %d  %x(h)",hsa->dvid,dstNetif->mtu,dstNetif->mtu);
	MT_WATCH(msg);
	if (dstNetif->mtu<(hsb->len-16))
	{						
		MT_TOCPU("Packet Exceed MTU prcoess:nexthop");
		modelCPUport( hsb, hsa,ale);
		return MACT_TOCPU;/* packet exceed MTU*/
	}
	hsa->nhmac[0]=asic_l2.mac47_40;
	hsa->nhmac[1]=asic_l2.mac39_24>>8;
	hsa->nhmac[2]=asic_l2.mac39_24&0xff;
	hsa->nhmac[3]=asic_l2.mac23_8>>8;
	hsa->nhmac[4]=asic_l2.mac23_8&0xff;
	hsa->nhmac[5]=hsa->nhmac[0]^hsa->nhmac[1]^hsa->nhmac[2]^hsa->nhmac[3] ^hsa->nhmac[4] ^ (nexthop>>2) ^fidHash[asic_l2.fid];		
	hsa->dp=asic_l2.memberPort;
	if (asic_l2.extMemberPort)
	{
		hsa->dpext=asic_l2.extMemberPort;				
		hsa->dp |= PM_PORT_NOTPHY;
	}
	if (asic_l2.toCPU)
	modelAddCPUport( hsb, hsa);
	sprintf(msg," dp %x dpext %x",hsa->dp,hsa->dpext);
	MT_WATCH(msg);
	return MRET_OK;

}


/*
@func enum MODEL_RETURN_VALUE | modelHandleNexthop	| model code hande nexthop action
@parm hsb_param_t* | hsb | Header Stamp of Before (for input)
@parm hsa_param_t* | hsa | Header Stamp of After (for output)
@rvalue enum MODEL_RETURN_VALUE |
@comm 
*/
enum MODEL_RETURN_VALUE modelHandleNexthop( hsb_param_t* hsb, hsa_param_t* hsa, ale_data_t *ale )
{
	int nexthop;
	rtl865x_tblAsicDrv_intfParam_t  *dstNetif;	  
	rtl865xc_tblAsic_l2Table_t   asic_l2;
	nexthop=ale->nexthop;
	dstNetif = &ale->dstNetif;
	sprintf(msg,"nexthop %d ",nexthop);
	MT_WATCH(msg);
	if(nexthop>=RTL8651_NEXTHOPTBL_SIZE)
		return MRET_TOCPU;				
	_rtl8651_readAsicEntry(TYPE_NEXT_HOP_TABLE, nexthop,&asic_nexthop);
	_rtl8651_readAsicEntry(TYPE_L2_SWITCH_TABLE,asic_nexthop.nextHop, &asic_l2);			
	rtl8651_getAsicNetInterface(asic_nexthop.dstnetif,dstNetif);
	ale->matchDstNetif = TRUE;
	hsa->difid=asic_nexthop.dstnetif;
	hsa->dvid= dstNetif->vid;	
	MT_WATCH("Handle NextHop");
	if (asic_nexthop.type)
	{
		hsa->pppidx = (asic_nexthop.PPPoEIndex);
		if (hsb->pppoeif)
			hsa->pppoeifo=HSA_PPPOE_MODIFY;
		else
			hsa->pppoeifo=HSA_PPPOE_TAGGING;

	}		
	else
	{
		if (hsb->pppoeif)
			hsa->pppoeifo=HSA_PPPOE_REMOVE_TAG;
	
	}
	sprintf(msg,"DVID %d MTU %d(0x%x)",hsa->dvid,dstNetif->mtu,dstNetif->mtu);
	MT_WATCH(msg);
	if (dstNetif->mtu<(hsb->len-16))
	{						
		MT_TOCPU("Packet Exceed MTU prcoess:nexthop");
		modelCPUport( hsb, hsa,ale);
		return MACT_TOCPU;/* packet exceed MTU*/
	}
	hsa->nhmac[0]=asic_l2.mac47_40;
	hsa->nhmac[1]=asic_l2.mac39_24>>8;
	hsa->nhmac[2]=asic_l2.mac39_24&0xff;
	hsa->nhmac[3]=asic_l2.mac23_8>>8;
	hsa->nhmac[4]=asic_l2.mac23_8&0xff;
	hsa->nhmac[5]=hsa->nhmac[0]^hsa->nhmac[1]^hsa->nhmac[2]^hsa->nhmac[3] ^hsa->nhmac[4] ^ (asic_nexthop.nextHop>>2) ^fidHash[asic_l2.fid];		
	hsa->dp=asic_l2.memberPort;
	if (asic_l2.extMemberPort)
	{
		hsa->dpext=asic_l2.extMemberPort;				
		hsa->dp |= PM_PORT_NOTPHY;
	}
	if (asic_l2.toCPU)
		modelAddCPUport( hsb, hsa);

	if ( hsa->dp&PM_PORT_NOTPHY )
		sprintf(msg," hsa->dp=0x%x dpext=0x%x",hsa->dp,hsa->dpext);
	else
		sprintf(msg," hsa->dp=0x%x",hsa->dp);
	MT_WATCH(msg);
	
	return MRET_OK;
}


/*
@func enum MODEL_RETURN_VALUE | modelLayer34Switching	| model code for L3 switching
@parm hsb_param_t* | hsb | Header Stamp of Before (for input)
@parm hsa_param_t* | hsa | Header Stamp of After (for output)
@rvalue enum MODEL_RETURN_VALUE |
@comm 
*/
enum MODEL_RETURN_VALUE modelLayer34Switching( hsb_param_t* hsb, hsa_param_t* hsa, ale_data_t *ale )
{
	uint16 decision_p,srcType_p, dstType_p;
	uint16 i,arpIndex,nhStart,lan2wan;
	uint32 retval,mcast_decision,routingIP,pmcrDecision;
	rtl865xc_tblAsic_arpTable_t  asic_arp;
	rtl865x_tblAsicDrv_intfParam_t  *srcNetif, *dstNetif;	  
	rtl865xc_tblAsic_l3RouteTable_t *l3_entry;
	rtl8651_tblAsic_pppoeTable_t asic_pppoe;
	ipaddr_t sip = hsb->sip;
	ipaddr_t dip = hsb->dip;
	

	srcNetif = dstNetif = NULL;	
	lan2wan=0;
	routingIP=0;
	/************************************* ***************************************
	ingoreEgressCheck is true when the packet  IP Muliticast
	************************************* **************************************/
	
	/************************************* ***************************************
			Check PPPoE Header 
			HSA_PPPOE_MODIFY  --> imply HSB is pppoeif.
	************************************* **************************************/
	if (hsb->pppoeif)
		hsa->pppoeifo=HSA_PPPOE_MODIFY;
	/******************************Pattern Match Checking*********************************/
	pmcrDecision = READ_VIR32(PPMAR);

	if (((pmcrDecision>>(26+hsb->spa)) & 0x1) &&( hsb->patmatch))
	{
		pmcrDecision = (pmcrDecision >>(14+2*hsb->spa))& 0x3;
		switch  (pmcrDecision)
		{
			case 0:
				/* Drop*/
				modelDrop( hsb,  hsa,ale);
				MT_DROP("Due to Patttern Match!!!");
				return MRET_DROP;
			case 1:
				/* Mirror to CPU*/
				modelAddCPUport( hsb,  hsa);
				MT_WATCH("Mirro to CPU due  to Patttern Match!!!");
				hsa->why2cpu |= 1;		/*set ph_reason field bit 0 to 1*/
				break;
			case 2:
				/*To  CPU*/
				modelCPUport( hsb,  hsa,ale);
				MT_TOCPU("To CPU due  to Patttern Match!!!");
				return MRET_TOCPU;
			case 3:
				/*Mirror to Mirror Port*/				
				break;
				
		}

	}
		
		
	if( (hsb->type!=HSB_TYPE_ICMP) &&(hsb->type!=HSB_TYPE_IP) &&(hsb->type !=HSB_TYPE_TCP) &&(hsb->type!=HSB_TYPE_UDP) &&(hsb->type!=HSB_TYPE_IGMP) &&
		(hsb->type!=HSB_TYPE_PPTP))
	{
		if (ale->DMACIsGateway )
		{
			MT_TOCPU("ether type is NOT IP, but DMAC==GMAC. So, to CPU." )
			modelCPUport(hsb, hsa, ale);
			hsa->why2cpu = 0x2;		/*pre-ACL filtering trap. set ph_reason field bit 1 to 1*/
			return MRET_TOCPU;
		}
		else
		{
			MT_WATCH("NOT IP PROTOCOL")
			retval=modelIngressACLCheck(hsb, hsa, ale);
			return  retval;
		}

	}

		
	if (READ_VIR32(ALECR)&FRAG2CPU && ((hsb->ipfo0_n )|| (hsb->ipfg==MORE_FRAG)))
	{
			MT_TOCPU("Fragment to CPU !!!!")
			modelCPUport(hsb,  hsa,ale);
			hsa->why2cpu = 0x2;		/*pre-ACL filtering trap. set ph_reason field bit 1 to 1*/
			return MRET_TOCPU;
	}

	

	/******************************Guest VLAN Checking*********************************/
	if ((retval=modelsrcMacUnauthRouting())!=MRET_OK)
	{
		if (retval==MRET_TOCPU) 
		{
			MT_TOCPU("TO CPU due to Guest VLAN (Layer3 Mode) ");
			modelCPUport(hsb,  hsa,ale);
			return retval;
		}
		retval = modelIngressACLCheck(hsb, hsa, ale);
		if (retval!=MRET_OK)
			return retval;
		if (retval==MRET_TOCPU)
		{
			MT_TOCPU("TO CPU due to Guest VLAN (Layer3 Mode) ");
			modelCPUport(hsb,  hsa,ale);
			return MRET_TOCPU;
		}
		else
		{
			MT_DROP("Drop due to Guest VLAN (Layer3 Mode) ");
			modelDrop(hsb, hsa,ale);
			return MRET_DROP;
		}		
	}


	for (i=0; i < RTL8651_ROUTINGTBL_SIZE;i++)		
		_rtl8651_readAsicEntry(TYPE_L3_ROUTING_TABLE, i, &asic_l3[i]);
	asic_l3[i].linkTo.ARPEntry.IPMask = 0;/* default routing*/
	
	for (i=0; i < RTL8651_IPTABLE_SIZE;i++)		
		_rtl8651_readAsicEntry(TYPE_EXT_INT_IP_TABLE, i, &asic_ipt[i]);
	for (i=0;i<RTL8651_SERVERPORTTBL_SIZE;i++)
		_rtl8651_readAsicEntry(TYPE_SERVER_PORT_TABLE, i, &asic_serverp[i]);			


	/****************************** Determine Source Type ******************************/
	ale->rouitingSIPIdx=routingSipIdx=modelIPRouting(hsb->sip);
	l3_entry=&asic_l3[routingSipIdx]; 	
	hsa->egif=l3_entry->linkTo.ARPEntry.internal;
	/****************************** Determine Source Type ******************************/

	if (modelLookupLP(sip) == TRUE)
	{
		srcType_p = _RTL8651_HOST_LP;
	} else
	{
		if ( l3_entry->linkTo.ARPEntry.internal)
		{	/* this packet is from Internal VLAN */
			if (modelLookupNatInternalIpAddr(sip))
			{
				srcType_p = _RTL8651_HOST_NI;
			} else
			{
				srcType_p = _RTL8651_HOST_NPI;
			}
		} else
		{	/* this packet is from External VLAN */
			srcType_p = _RTL8651_HOST_RP;
		}
	}
	/****************************** Determine Destination Type ******************************/
	if (modelLookupNatExternalIpAddr(dip))
	{	/* nat external */
		dstType_p = _RTL8651_HOST_NE;
	}else if (modelLookupLP(dip) == TRUE)
	{	/* local public */
		dstType_p = _RTL8651_HOST_LP;
	}else if (modelLookupNaptIpAddr(dip) == TRUE)
	{	/* napt external */
		dstType_p = _RTL8651_HOST_NPE;
	}else
	{	
		ale->rouitingDIPIdx=routingDipIdx=modelIPRouting(dip);		
		l3_entry=&asic_l3[routingDipIdx]; 	
		if ( l3_entry->linkTo.ARPEntry.internal)
		{
			if (modelLookupNatInternalIpAddr(dip) == TRUE)
				{	/* nat internal */
					dstType_p = _RTL8651_HOST_NI;
				}else
				{	/* napt internal */
					dstType_p = _RTL8651_HOST_NPI;
				}
		}
		else
			dstType_p = _RTL8651_HOST_RP;
	}



	/****************************** Decision Table Lookup ******************************/
	assert( ((dstType_p) < _RTL8651_HOST_MAX) && ((srcType_p) < _RTL8651_HOST_MAX) );
	ale->decision=decision_p = _RTL8651_PROC[srcType_p][dstType_p];
	ale->srcType_p = srcType_p;
	ale->dstType_p = dstType_p;
	switch(decision_p)
	{
		case _RTL8651_DEC_RT_LR:
			MT_WATCH( "L3 Routing -- Local Public to Remote Host" ); break;
		case _RTL8651_DEC_RT_RL:
			MT_WATCH( "L3 Routing -- Remote host to Local Public" ); break;
		case _RTL8651_DEC_RT_LNP:
			MT_WATCH( "L3 Routing -- Local Public to NAPT Internal" ); break;
		case _RTL8651_DEC_RT_NPL:
			MT_WATCH( "L3 Routing -- NAPT Internal to Local Public" ); break;
		case _RTL8651_DEC_RT_LN:
			MT_WATCH( "L3 Routing -- Local Public to NAT Internal" ); break;
		case _RTL8651_DEC_RT_NL:
			MT_WATCH( "L3 Routing -- NAT Internal to Local Public" ); break;
		case _RTL8651_DEC_RT_LL:
			MT_WATCH( "L3 Routing -- Local Public to Local Public" ); break;
		case _RTL8651_DEC_RT_NN:
			MT_WATCH( "L3 Routing -- NAT Internal to NAT Internal" ); break;
		case _RTL8651_DEC_RT_NPNP:
			MT_WATCH( "L3 Routing -- NAPT Internal to NAPT Internal" ); break;
		case _RTL8651_DEC_RT_NPN:
			MT_WATCH( "L3 Routing -- NAPT Internal to NAT Internal" ); break;
		case _RTL8651_DEC_RT_NNP:
			MT_WATCH( "L3 Routing -- NAT Internal to NAPT Internal" ); break;
		case _RTL8651_DEC_RT_RR:
			MT_WATCH( "L3 Routing -- Remote Public to Remote Public (Wan Side routing)" ); break;
		/* <---- L4 ----> */
		case _RTL8651_DEC_NT:
			MT_WATCH( "L4 NAT - outbound" ); break;
		case _RTL8651_DEC_NTR:
			MT_WATCH( "L4 NAT - inbound" ); break;
		case _RTL8651_DEC_NTBC_NP:
			MT_WATCH( "port bouncing -- NAPT Internal to NE" ); break;
		case _RTL8651_DEC_NTBC_N:
			MT_WATCH( "port bouncing -- NAT Internal to NE" ); break;
		case _RTL8651_DEC_NTBC_L:
			MT_WATCH( "port bouncing -- Local Public to NE" ); break;
		case _RTL8651_DEC_NPT:
			MT_WATCH( "L4 NAPT - outbound" ); break;
		case _RTL8651_DEC_NPTR:
			MT_WATCH( "L4 NAPT - inbound" ); break;
		case _RTL8651_DEC_NPTBC_NP:
			MT_WATCH( "port bouncing -- NAPT Internal to NPE" ); break;
		case _RTL8651_DEC_NPTBC_N:
			MT_WATCH( "port bouncing -- NAT Internal to NPE" ); break;
		case _RTL8651_DEC_NPTBC_L:
			MT_WATCH( "port bouncing -- Local Public to NPE" ); break;
/* <---- Other ----> */
		case _RTL8651_DEC_DP:
			MT_WATCH( "decide to drop packet" ); break;
		default:
			MT_WATCH( "*** UNKNOWN decision_p ***" ); break;
			break;
	}

	
	/****************************** Check TTL******************************/
	if (((READ_VIR32(ALECR)&TTL_1Enable) && (hsb->ttlst - 1 == 0)) || (hsb->ttlst == 0))
	{
		if (l3_entry->linkTo.NxtHopEntry.process == 6)	/*DROP*/
		{
			MT_DROP( "Nexthop decide drop, before trap to CPU (TTL=0)" );
			return MRET_DROP;
		}
		
		modelCPUport(hsb,  hsa,ale);
		hsa->why2cpu = cvtHostType2PHReason(ale, hsb) | 0xa;	/*DIP:RP   SIP:NPI and NAPT process */
		MT_TOCPU("TTL == 0, trap to CPU !!!!")
		return MRET_TOCPU;
	}

	
	/****************************** PPPoE Table Lookup ******************************/
	if ((READ_VIR32(SWTCR1)&EN_RTL8650B))
	{
		if (hsb->pppoeif)
		{
			for (i=0;i<RTL8651_PPPOETBL_SIZE;i++)
			{
				_rtl8651_readAsicEntry(TYPE_PPPOE_TABLE, i, &asic_pppoe);			
				if (asic_pppoe.sessionID==hsb->pppoeid)
				{
					ale->matchPPPoE=1;
					break;
				}
			}
			if (ale->matchPPPoE==0)
			{
				MT_TOCPU("Due to PPPoE ID Mismatch");
				modelCPUport( hsb, hsa,ale);
				return MRET_TOCPU;
			}			

		}
		}




	/******************************IP Multicast Checking*********************************
	IP Mulitcat Address Range  224.0.0.0 to 238.255.255.255 
	********************************************************************************/
	if (( (hsb->da[0]==0x1 ) && (hsb->da[1]==0x00 ) && (hsb->da[2]==0x5e)) ||
	((ale->doL3L4)&& (hsb->pppoeif)&&(hsb->dip>=0xE0000000)&&(hsb->dip<=0XEEFFFFFF)))
	{
		mcast_decision=(READ_VIR32(FFCR)&IPMltCstCtrl_MASK);
		switch(mcast_decision)
		{
			case IPMltCstCtrl_Disable:
				MT_WATCH("IPMltCstCtrl_Disable, continue ... even it is multicast packet.");
				break;
			case IPMltCstCtrl_Enable:	
				MT_WATCH("IPMltCstCtrl_Enable, enter modelIPMulticastSwitching()");
				return modelIPMulticastSwitching(hsb,hsa,ale);
			case IPMltCstCtrl_TrapToCpu:
				MT_TOCPU("DA is multicast !!!!")
				modelCPUport(hsb, hsa, ale);
				return MRET_TOCPU;
				
		}
	}
	if (ale->doL3L4==0)
	{
		if (ale->DMACIsGateway )
		{
			MT_TOCPU("L3/L4 is disabled, but DMAC==GMAC. So, to CPU." )
			modelCPUport(hsb, hsa, ale);
			hsa->why2cpu = 0x2;		/*pre-ACL filtering trap. set ph_reason field bit 1 to 1*/
			return MRET_TOCPU;
		}
		else
		{
			uint8 strTmp[256];
			
			retval=modelIngressACLCheck(hsb, hsa, ale);
			snprintf( strTmp, sizeof(strTmp)-1, "DA is not  MAC of  GateWay or EN_L3 EN_L4 is not enabled!!!! modelIngressACLCheck()=%d", retval );
			MT_WATCH( strTmp );
			return retval;
		}
	}


	/******************************  ALG and Server Port Process*************************/
	switch (decision_p )
	{
			case _RTL8651_DEC_NPT:	/* L4 NAPT - outbound */				
			case _RTL8651_DEC_NT:	/* L4 NAT - outbound */		
				lan2wan=1;
				MT_WATCH("Lan2Wan ALG and Server Port Processing")
				if (modelAlgCheck(hsb,hsa,lan2wan,ale)==MRET_TOCPU)
				{
					MT_TOCPU("Lan2Wan - matched ALG entry");
					return MRET_TOCPU;	
				}
				retval=modelMatchInternalServerPort(hsb,hsa,ale);
				if (retval==MACT_TOCPU)
				{
					retval = modelIngressACLCheck( hsb, hsa, ale );
					if (retval==MRET_DROP);
					{
						MT_DROP("modelIngressACLCheck decided DROP" );
						return MRET_DROP;
					}
					MT_TOCPU("Due to Port Pouncing");
					modelCPUport( hsb, hsa,ale);
					return MRET_TOCPU;			
				}
				if (retval==MACT_L3ROUTING)  /* Match Server Port*/
				{
					hsa->l3csdt =OCSUB(OCADD(hsb->sip>>16,hsb->sip&0xffff), OCADD(hsa->trip>>16, (hsa->trip & 0xffff)));
					hsa->l4csdt= OCADD(OCSUB(hsb->sprt,hsa->port),hsa->l3csdt);
					routingIP=hsb->dip;		
					
					goto layer3_routing;
				}

				
				break;
			case _RTL8651_DEC_NPTR:	/* L4 NAPT - inbound */						
			case _RTL8651_DEC_NTR:	/* L4 NAT - inbound */	
				MT_WATCH("Wan2Lan ALG and Server Port Processing")
				if (modelAlgCheck(hsb,hsa,lan2wan,ale)==MRET_TOCPU)
				{
					MT_TOCPU("Wan2Lan - matched ALG entry");
					return MRET_TOCPU;	
				}
				retval=modelMatchExternalServerPort(hsb,hsa,ale);
				if (retval==MACT_L3ROUTING)
				{
					routingDipIdx=modelIPRouting(hsa->trip);
					routingIP=hsa->trip;
					hsa->l3csdt =OCSUB(OCADD(hsb->dip>>16,hsb->dip&0xffff), OCADD(hsa->trip>>16, (hsa->trip & 0xffff)));
					hsa->l4csdt= OCADD(OCSUB(hsb->dprt,hsa->port),hsa->l3csdt);
					if (hsb->pppoeif) 
						hsa->pppoeifo=HSA_PPPOE_REMOVE_TAG;					
					goto layer3_routing;
				}
				break;
	}


	if (hsb->type==HSB_TYPE_PPTP)
	{
		if ((READ_VIR32(MSCR) &(EN_L4)) &&( ale->doL3L4))
		{
			retval=modelGREPaassthrough(hsb,hsa,ale);
			switch(retval)
			{
				case MACT_L3ROUTING:
					routingDipIdx=modelIPRouting(hsa->trip);
					routingIP=hsa->trip;
					goto layer3_routing;
				case MACT_TOCPU:
					retval=modelIngressACLCheck(hsb, hsa,  ale);
					MT_WATCH( "After IngressACLCheck()" );
					if (retval==MRET_OK) retval=MRET_TOCPU;
					return  retval;
				default:
					goto layer3_routing;

			}
		}
		else
		{
		}
	}
	modelLayer3Action(hsb,hsa,ALLPORTMASK);
	switch (decision_p )
	{
		/* =================================================================
											NAPT
		     ================================================================= */
		case _RTL8651_DEC_NPT:	/* L4 NAPT - outbound */
		case _RTL8651_DEC_NPTR:	/* L4 NAPT - inbound */

			if ( hsb->ipfg&1/*IP_MF*/ )
			{
				MT_TOCPU("Trap to CPU due to IP MF=1 (More Fragment)");
				modelCPUport( hsb, hsa,ale);
				return MACT_TOCPU;
			}
			else if ( hsb->ipfo0_n==TRUE )
			{
				MT_TOCPU("Trap to CPU due to IP offset!=0");
				modelCPUport( hsb, hsa,ale);
				return MACT_TOCPU;
			}
			else
			{
				MT_WATCH("NAPT Processing    ");
				retval = modelLayer4Switching( hsb, hsa, lan2wan,ale);
				if(retval==MACT_L3ROUTING)
				{
					routingDipIdx=modelIPRouting(hsa->trip);
					routingIP=hsa->trip;
					goto layer3_routing;

				}
				else if(retval==MACT_FORWARD)
				{
					routingIP=hsb->dip;
					goto layer3_routing;
				}
			}

			MT_TOCPU("Trap to CPU due to No Rule  layer4 routing   ");
			modelCPUport( hsb, hsa,ale);
			return MACT_TOCPU;/* port bouncing*/


		/* =================================================================
											NAT
		     ================================================================= */
		case _RTL8651_DEC_NT:	/* L4 NAT - outbound */
			MT_WATCH("NAT  outbound Processing    ");
			modelNatTransSrcIpAddr(hsb,hsa,ale);
			routingIP=hsa->trip;
			hsa->l34tr=1;
			hsa->port=hsb->sprt;
			hsa->l3csdt =OCSUB(OCADD(hsb->sip>>16,hsb->sip&0xffff), OCADD(hsa->trip>>16, (hsa->trip & 0xffff)));
			hsa->l4csdt= OCADD(OCSUB(hsb->sprt,hsa->port),hsa->l3csdt);
			break;
		case _RTL8651_DEC_NTR:	/* L4 NAT - inbound */
			MT_WATCH("NAT  inboud Processing    ");
			modelNatTransDstIpAddr(hsb,hsa,ale);
			routingIP=hsa->trip;
			hsa->l34tr=1;
			hsa->port=hsb->dprt;
			hsa->l3csdt =OCSUB(OCADD(hsb->sip>>16,hsb->sip&0xffff), OCADD(hsa->trip>>16, (hsa->trip & 0xffff)));
			hsa->l4csdt= OCADD(OCSUB(hsb->sprt,hsa->port),hsa->l3csdt);
			if (ale->matchPPPoE) 
				hsa->pppoeifo=HSA_PPPOE_REMOVE_TAG;					
			routingDipIdx=modelIPRouting(hsa->trip);
			
			break;
		/* =================================================================
											Routing
		     ================================================================= */
		case _RTL8651_DEC_RT_LR:	/* L3 Routing -- Local Public to Remote Host */
		case _RTL8651_DEC_RT_RL:	/* L3 Routing -- Remote host to Local Public */
		case _RTL8651_DEC_RT_LNP:	/* L3 Routing -- Local Public to NAPT Internal */
		case _RTL8651_DEC_RT_NPL:	/* L3 Routing -- NAPT Internal to Local Public */
		case _RTL8651_DEC_RT_LN:	/* L3 Routing -- Local Public to NAT Internal */
		case _RTL8651_DEC_RT_NL:	/* L3 Routing -- NAT Internal to Local Public */
		case _RTL8651_DEC_RT_LL:	/* L3 Routing -- Local Public to Local Public */
		case _RTL8651_DEC_RT_NN:	/* L3 Routing -- NAT Internal to NAT Internal */
		case _RTL8651_DEC_RT_NPNP:	/* L3 Routing -- NAPT Internal to NAPT Internal */
		case _RTL8651_DEC_RT_NPN:	/* L3 Routing -- NAPT Internal to NAT Internal */
		case _RTL8651_DEC_RT_NNP:	/* L3 Routing -- NAT Internal to NAPT Internal */
		case _RTL8651_DEC_RT_RR:	/* L3 Routing -- Remote Public to Remote Public (Wan Side routing) */
			hsa->trip=hsb->sip;
			hsa->port=hsb->sprt;
			routingIP=hsb->dip;
			break;
		/* =================================================================
											Port bouncing
		     ================================================================= */

		case _RTL8651_DEC_NPTBC_NP:		/* port bouncing -- NAPT Internal to NPE */
		case _RTL8651_DEC_NPTBC_N:		/* port bouncing -- NAT Internal to NPE */
		case _RTL8651_DEC_NPTBC_L:		/* port bouncing -- Local Public to NPE */	
		case _RTL8651_DEC_NTBC_NP:		/* port bouncing -- NAPT Internal to NE */
		case _RTL8651_DEC_NTBC_N:		/* port bouncing -- NAT Internal to NE */
		case _RTL8651_DEC_NTBC_L:		/* port bouncing -- Local Public to NE */
#ifdef CONFIG_RTL865X_MODEL_TEST_FT2
	/* To speed up, do not show any message */
#else
			rtlglue_printf("New FPGA\n");
#endif
			ale->doL4=1;
			retval=modelIngressACLCheck(hsb, hsa, ale);
			MT_WATCH( "After IngressACLCheck()" );
			if (retval!=MRET_OK)
				return retval;
			MT_WATCH("Due to Port Bouncing")
			modelCPUport( hsb, hsa,ale);			
			return MRET_TOCPU;
		/* =================================================================
											Other
		     ================================================================= */

		case _RTL8651_DEC_DP:	/* Drop */
			retval=modelIngressACLCheck(hsb, hsa, ale);
			MT_WATCH( "After IngressACLCheck()" );
			if (retval!=MRET_OK)
				return retval;
			break;

		default:
			assert(0);
			/* return FAILED */ 
	}

layer3_routing:	
	l3_entry=&asic_l3[routingDipIdx]; 	
	assert( l3_entry->linkTo.ARPEntry.valid==TRUE );
	sprintf(msg,"DipIdx[%d] IPAddr="NIPQUAD2STR"/%d process=%d", routingDipIdx, NIPQUAD2(l3_entry->IPAddr), l3_entry->linkTo.ARPEntry.IPMask, l3_entry->linkTo.ARPEntry.process );
	MT_WATCH(msg);
	assert( ale->matchSrcNetif==TRUE );
	srcNetif = &ale->srcNetif;
	dstNetif = &ale->dstNetif;

	MT_WATCH( "before Ingress ACL.." );	
	retval=modelIngressACLCheck(hsb, hsa, ale);
	MT_WATCH( "After IngressACLCheck()" );
	if (retval!=MRET_OK)
		return retval;
	if (ale->doL4&& (hsb->ipfo0_n))
	{
		MT_TOCPU("Trap to CPU due to NON Initial IP Fragment ");
		modelCPUport(hsb, hsa,ale);
		return MRET_TOCPU;	
					
	}	
	if (srcNetif->enableRoute==0) 
	{
		MT_TOCPU("NETIF l34 DISABLE");
		hsa->why2cpu=8;
		modelCPUport( hsb, hsa,ale);
		return MACT_TOCPU;
	}
	/* Get netif  and   change pointer for Egress ACL... .*/
	switch(l3_entry->linkTo.ARPEntry.process)
	{
		case PROCESS_PPPOE:/*PPPoE*/
			ale->nexthop = l3_entry->linkTo.PPPoEEntry.nextHop;
			ale->dstNetifIdx=l3_entry->linkTo.PPPoEEntry.netif;
			ale->pppoeIdx=l3_entry->linkTo.PPPoEEntry.PPPoEIndex;
			modelHandlePPPoE(hsb,  hsa, ale);
			break;
		case PROCESS_DIRECT:/*Direct*/
			break;
		case PROCESS_INDIRECT:/*Indirect*/
			rtl8651_getAsicNetInterface(l3_entry->linkTo.ARPEntry.netif, dstNetif);
			ale->matchDstNetif = TRUE;
			if (dstNetif->mtu<(hsb->len-16))
			{						
				MT_TOCPU("Packet Exceed MTU  prcoess:indirect");
				modelCPUport( hsb, hsa, ale);
				return MACT_TOCPU;/* packet length exceed MTU*/
			}	
			hsa->difid=l3_entry->linkTo.ARPEntry.netif;	
		    	hsa->dvid = dstNetif->vid;	
			arpIndex = routingIP &( (1<<(31-(l3_entry->linkTo.ARPEntry.IPMask)))-1) ;
			sprintf(msg,"Calculate DIP %08x,arpOffset:%d, MASK:%08x,MASKLEN:%d",routingIP,arpIndex, (1<<(31-(l3_entry->linkTo.ARPEntry.IPMask)))-1,l3_entry->linkTo.ARPEntry.IPMask);
			MT_WATCH(msg);
			if (arpIndex >((l3_entry->linkTo.ARPEntry.ARPEnd-l3_entry->linkTo.ARPEntry.ARPStart+1)<<3))
			{
				modelCPUport(hsb,  hsa,ale);
				sprintf(msg,"DIP:%08x,Arp offset:%d excced capacity:%d",routingIP,arpIndex,(l3_entry->linkTo.ARPEntry.ARPEnd-l3_entry->linkTo.ARPEntry.ARPStart+1)<<3);
				MT_TOCPU(msg);
				return MRET_TOCPU;
			}								
			arpIndex += (l3_entry->linkTo.ARPEntry.ARPStart<<3);
			sprintf(msg,"DIP:%08x,arpIndex:%d arp length:%d",routingIP,arpIndex,(((l3_entry->linkTo.ARPEntry.ARPEnd-l3_entry->linkTo.ARPEntry.ARPStart+1)<<3)-1));		
			MT_WATCH(msg);
			_rtl8651_readAsicEntry(TYPE_ARP_TABLE, arpIndex, &asic_arp);						
			if (asic_arp.valid) 
			{
				modelModifyDA(hsb,  hsa,asic_arp.nextHop);
				asic_arp.aging=0x1f;
				_rtl8651_forceAddAsicEntry(TYPE_ARP_TABLE,arpIndex, &asic_arp);							
				
			}				
			else	
			{
				modelCPUport(hsb,  hsa,ale);
				sprintf(msg,"Due to No Arp Entry  ip %x ",routingIP);
				MT_TOCPU(msg);
				return MRET_TOCPU;
			}				
			break;
		case PROCESS_S_CPU: /* CPU */
			modelCPUport(hsb,  hsa,ale);
			MT_TOCPU("Due to L3 Entry to CPU");
			return MRET_TOCPU;
		case PROCESS_S_DROP: /* Drop */	
			MT_DROP( "l3_entry->linkTo.ARPEntry.process==PROCESS_S_DROP" );
			return MRET_DROP;
		case PROCESS_NXT_HOP:/* NextHop*/
			nhStart = (l3_entry->linkTo.NxtHopEntry.nhStart) << 1;
     			if(nhStart>=RTL8651_NEXTHOPTBL_SIZE)
     			{
     				MT_TOCPU("nhStart>=RTL8651_NEXTHOPTBL_SIZE");
			       	return MRET_TOCPU;				
		       	}
			switch(l3_entry->linkTo.NxtHopEntry.nhAlgo)
			{
				case ALGO_RR_BASE:
					ale->nexthop=nhStart+(RoudRobinCounter& ( (1<<(l3_entry->linkTo.NxtHopEntry.nhNum+1))-1));
					RoudRobinCounter++;					
					break;
				case ALGO_SESSION_BASE:
					ale->nexthop=nhStart+ 
						(rtl8651_naptTcpUdpTableIndex(hsb->type==HSB_TYPE_TCP?1:0,hsb->sip,hsb->sprt,hsb->dip,hsb->dprt) &
							( (1<<(l3_entry->linkTo.NxtHopEntry.nhNum+1))-1)	)		; 					
					break;
				case ALGO_SOURCE_BASE:
					ale->nexthop=nhStart+(hsb->sip&( (1<<(l3_entry->linkTo.NxtHopEntry.nhNum+1))-1));			
					break;
				
			}			
			sprintf(msg,"nhStart %d .nhNum %d nextop %d,Algo:%d ",nhStart,l3_entry->linkTo.NxtHopEntry.nhNum,ale->nexthop,
						l3_entry->linkTo.NxtHopEntry.nhAlgo);
			MT_WATCH(msg);
			modelHandleNexthop(  hsb, hsa, ale );
			break;
		default:
			return MRET_TOCPU;				
	}	
	retval=modelAfterL34IngressACLCheck(hsb, hsa, ale);
	MT_WATCH( "modelAfterL34IngressACLCheck" ); 
	return MRET_OK;

}





/*
@func
@parm hsb_param_t* | hsb | Header Stamp of Before (for input)
@parm hsa_param_t* | hsa | Header Stamp of After (for output)
@rvalue int32 |	 
@comm 
*/
enum MODEL_ACTION_VALUE modelMatchExternalServerPort( hsb_param_t* hsb, hsa_param_t* hsa,ale_data_t *ale)
{
	uint16 i,port;
	uint32 ip;
	rtl865xc_tblAsic_srvPortTable_t   *entry;

	port = hsb->dprt;
	ip = hsb->dip;
	if (((hsb->type!=HSB_TYPE_UDP) &&( hsb->type!=HSB_TYPE_TCP)) ||(hsb->ipfo0_n==1))
		return MACT_FORWARD;
	for (i=0;i<RTL8651_SERVERPORTTBL_SIZE;i++)
	{	
		entry = &asic_serverp[i];
		if(entry->protocol == SERVERP_INVALID)
			continue;
		if ((entry->protocol ==SERVERP_PROTUDP) && (hsb->type!=HSB_TYPE_UDP))
			continue;		
		if ((entry->protocol ==SERVERP_PROTTCP) && (hsb->type!=HSB_TYPE_TCP))
			continue;		
		if (entry->isPortRange &&(port>=entry->internalPort)&&(port<=entry->externalPort))
		{

			hsa->trip=entry->internalIP;
			hsa->port=hsb->dprt;
			hsa->l34tr=1;
			entry->traffic=1;
			modelLayer3Action(hsb,hsa,ALLPORTMASK);
			if (entry->PValid)
				modelSetPriroity(PRI_NAPT, entry->PID,ale);
			_rtl8651_forceAddAsicEntry(TYPE_SERVER_PORT_TABLE, i, entry);
			ale->doL4=1;
			MT_WATCH("Match External Server Port!!!!!!!");
			return MACT_L3ROUTING;
		

		}
		else if ((port==entry->externalPort)&& (ip==entry->externalIP))
		{
			hsa->trip=entry->internalIP;
			hsa->port=entry->internalPort;
			hsa->l34tr=1;
			modelLayer3Action(hsb,hsa,ALLPORTMASK);
			if( entry->PValid)
				modelSetPriroity(PRI_NAPT, entry->PID,ale);
			entry->traffic=1;
			_rtl8651_forceAddAsicEntry(TYPE_SERVER_PORT_TABLE, i, entry);			
			ale->doL4=1;
			MT_WATCH("Match External Server Port!!!!!!!");
			return MACT_L3ROUTING;
		}		
			
	}
	return MACT_FORWARD;
}

/*
@func
@parm hsb_param_t* | hsb | Header Stamp of Before (for input)
@parm hsa_param_t* | hsa | Header Stamp of After (for output)
@rvalue int32 |	 
@comm 
*/

enum MODEL_ACTION_VALUE modelMatchInternalServerPort( hsb_param_t* hsb, hsa_param_t* hsa,ale_data_t *ale)
{
	uint16 i,port;
	uint32 ip;
	rtl865xc_tblAsic_srvPortTable_t   *entry;

	port = hsb->sprt;
	ip = hsb->sip;
	if (((hsb->type!=HSB_TYPE_UDP) &&( hsb->type!=HSB_TYPE_TCP)) ||(hsb->ipfo0_n==1))
		return MACT_FORWARD;
	for (i=0;i<RTL8651_SERVERPORTTBL_SIZE;i++)
	{	
		entry = &asic_serverp[i];
		if(entry->protocol == SERVERP_INVALID)
			continue;
		if ((entry->protocol ==SERVERP_PROTUDP) && hsb->type!=HSB_TYPE_UDP)
			continue;		
		if ((entry->protocol ==SERVERP_PROTTCP) && hsb->type!=HSB_TYPE_TCP)
			continue;		
		if (entry->isPortRange )
		{
			sprintf(msg,"server port range sport %d  %d %d %x(ip)\n",port,entry->internalPort,entry->externalPort,entry->internalIP);
			MT_WATCH(msg);
		}
		if ((entry->isPortRange) &&(port>=entry->internalPort)&&(port<=entry->externalPort) &&(ip==entry->internalIP))
		{

			ale->doL4=1;
			if ((hsb->dip==entry->externalIP) && (hsb->dprt>=entry->internalPort)&&(hsb->dprt<=entry->externalPort))
			{
				if (entry->PValid)
					modelSetPriroity(PRI_NAPT, entry->PID,ale);
				MT_WATCH("PORT Bouncing!!!!!!!");
				return MACT_TOCPU;// * port bouncing*/
			}
			MT_WATCH("Match Server Port Range!!!!!!!");
			hsa->trip=entry->externalIP;
			hsa->port=hsb->sprt;
			hsa->l34tr=1;
			modelLayer3Action(hsb,hsa,ALLPORTMASK);
			entry->traffic=1;
			if (entry->PValid)
				modelSetPriroity(PRI_NAPT, entry->PID,ale);
			_rtl8651_forceAddAsicEntry(TYPE_SERVER_PORT_TABLE, i, entry);
			return MACT_L3ROUTING;
			
				
		}
		else if ((port==entry->internalPort)&& (ip==entry->internalIP))
		{
		
			if ((hsb->dip==entry->externalIP) && (hsb->dprt==entry->externalPort))
			{
			
				if (entry->PValid)
					modelSetPriroity(PRI_NAPT, entry->PID,ale);
				MT_WATCH("PORT Bouncing!!!!!!!");				
				return MACT_TOCPU;//* port bouncing*/
			}
			MT_WATCH("Match Internal Server Port!!!!!!!");
			hsa->trip=entry->externalIP;
			hsa->port=entry->externalPort;
			hsa->l34tr=1;
			modelLayer3Action(hsb,hsa,ALLPORTMASK);
			entry->traffic=1;
			if (entry->PValid)
				modelSetPriroity(PRI_NAPT, entry->PID,ale);
			_rtl8651_forceAddAsicEntry(TYPE_SERVER_PORT_TABLE, i, entry);
			ale->doL4=1;
			return MACT_L3ROUTING;
		}
	}
	MT_WATCH("Not Match Internal Server Port!!!!!!!");	
	return MACT_FORWARD;
	
}

/*
@func
@parm hsb_param_t* | hsb | Header Stamp of Before (for input)
@parm hsa_param_t* | hsa | Header Stamp of After (for output)
@rvalue int32 |	 
@comm 
*/
enum MODEL_ACTION_VALUE modelLayer4Switching( hsb_param_t* hsb, hsa_param_t* hsa,uint8 LanToWan, ale_data_t *ale)
{
	int retval;
	if (LanToWan)
	{
	      retval = modelLayer4NaptLanWanProcessing(hsb, hsa, ale);
	}
	else
	{
      		retval = modelLayer4NaptWanLanProcessing(hsb, hsa, ale);

	 }
	switch (retval)
	{
		case MRET_OK:	
			ale->doL4=1;
			if (LanToWan==0)
			{
				hsa->l3csdt =OCSUB(OCADD(hsb->dip>>16,hsb->dip&0xffff), OCADD(hsa->trip>>16, (hsa->trip & 0xffff)));
				hsa->l4csdt= OCADD(OCSUB(hsb->dprt,hsa->port),hsa->l3csdt);
				hsa->l34tr=1;
				retval=MACT_L3ROUTING;
			}
			else
			{
				hsa->l3csdt =OCSUB(OCADD(hsb->sip>>16,hsb->sip&0xffff), OCADD(hsa->trip>>16, (hsa->trip & 0xffff)));
				hsa->l4csdt= OCADD(OCSUB(hsb->sprt,hsa->port),hsa->l3csdt);
				hsa->l34tr=1;
				retval=MACT_FORWARD;
			}
			break;
		case MRET_DROP:
			modelDrop(hsb,hsa,ale);
			retval=MACT_DROP;
			break;
		case MRET_TOCPU:
			modelCPUport(hsb,hsa,ale);
			retval=MACT_TOCPU;

		
	}
	return retval;
}

