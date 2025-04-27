/*
* Copyright c                  Realtek Semiconductor Corporation, 2002  
* All rights reserved.
* 
* Program : Source File for Layer2 Model Code
* Abstract : 
* Author : Louis Yung-Chieh Lo (yjlou@realtek.com.tw)               
* $Id: l2Model.c,v 1.49 2006-11-27 15:37:48 yjlou Exp $
*/

#include <rtl_glue.h>
#include <rtl865x/rtl8651_debug.h>
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
#include "l2Model.h"
#include "l34Model.h"
uint8 fidHash[]={0x0,0xf,0xf0,0xff};  /* global value*/


static int32 _cmpTableMac(ether_addr_t * srcP, ether_addr_t * dstP);
static uint32 l2SwitchTable_getHashIndex(uint8 *,uint8);
static uint32 PSpanningStatus,srcMacControl,dstPortControl,src1XPortControl,ToGuestVlan;
static int8 msg[512];					
static uint16 dot1x_process,smac_auth,dmac_auth,gvid;
#define DPC_NUM	5   /*decision priority control number*/

int32 modelSetPriroity(uint32 idx, uint8 priority,ale_data_t *ale)
{
	if (ale->priorityControl[idx]>=0) return -1;
	ale->priorityControl[idx]=priority;
	return 0;
}

int32 modelPriorityDecision(hsb_param_t *hsb, hsa_param_t *hsa, ale_data_t *ale )
{
	int i,j,bit;
	uint8 tos;
	uint32 dpcr,dscpOffset;
	int8 max;
	
	dpcr=READ_VIR32(QIDDPCR);
	if ((hsb->type>=HSB_TYPE_IP) && (hsb->llcothr==0))
		
	{
		tos = hsb->iptos>>2;
		dscpOffset=(tos/10)*4 ;
		ale->priorityControl[PRI_DSCP]=(READ_VIR32(DSCPCR0+dscpOffset)>> (tos%10)*3)&0x7;
	}
	dpcr=READ_VIR32(QIDDPCR);
	/*
		#define   PRI_PBP       0
		#define   PRI_1Q        1
		#define   PRI_DSCP      2
		#define   PRI_ACL       3
		#define   PRI_NAPT      4*/
	sprintf(msg,"Priority Candidate:\tPORT:%d, 1Q:%d, DSCP:%d, ACL:%d, NAPT:%d", ale->priorityControl[PRI_PBP],ale->priorityControl[PRI_1Q],
	                                                                             ale->priorityControl[PRI_DSCP],ale->priorityControl[PRI_ACL],
	                                                                             ale->priorityControl[PRI_NAPT]);
	MT_WATCH(msg);

	max=-1;
	for (j=3;j>=0;j--)
	{
		for (i=0;i<DPC_NUM;i++)
		{
			bit = (  1<< (j+(i<<2)));
			if ( (dpcr& bit) && (ale->priorityControl[i]>max))
			{
				max = ale->priorityControl[i];
				sprintf(msg,"Candidate Selected is round:%d %s-based, prioirty %d", j, 
				        i==PRI_PBP?"PRI_PBP":i==PRI_1Q?"PRI_1Q":i==PRI_DSCP?"PRI_DSCP":i==PRI_ACL?"PRI_ACL":"PRI_NAPT", max);
				MT_WATCH(msg);
			}
		}
		if (max!=-1)
			break; /* Yes, we found one. */
	}
	if ( max==-1 )
	{
		sprintf( msg, "Error Config: no priority is selected. QIDDPCR=0x%08x, Priority Candidate: PORT:%d, 1Q:%d, DSCP:%d, ACL:%d, NAPT:%d", 
		    READ_MEM32(QIDDPCR), ale->priorityControl[PRI_PBP],ale->priorityControl[PRI_1Q],
		    ale->priorityControl[PRI_DSCP],ale->priorityControl[PRI_ACL],
		    ale->priorityControl[PRI_NAPT] );
		RTL_BUG( msg );
	}
	
	hsa->priority = max;
    sprintf(msg, "Final decided prioirty: %d", hsa->priority );
    MT_WATCH(msg);

	return 0;
}
enum MODEL_RETURN_VALUE modelCheckIgnore1QTag(hsb_param_t *hsb, hsa_param_t *hsa,ale_data_t *ale)
{
	modelPriorityDecision(hsb,hsa,ale);	
	if ((READ_VIR32(VCR0) &EN_1QTAGVIDIGNORE))
	{
		hsa->dvid=hsa->svid;
		if (hsb->tagif)
		{
			uint8 szComment[128];
			
			hsa->dvtag=BIT9MASK;

			sprintf( szComment, "EN_1QTAGVIDIGNORE enabled, thus hsa->dvtag is set to 0x%03x", hsa->dvtag );
			MT_WATCH( szComment );
		}
	}

	return MRET_OK;
}


/* 
@func uint16 | modelsrcMacauthRouting	| This function will return the match or no
@parm null
@rvalue enum MODEL_BOOLEAN_VALUE|
@comm 
*/
enum MODEL_RETURN_VALUE modelsrcMacUnauthRouting(void)
{
		if (!ToGuestVlan)
			return MRET_OK;

		if ((READ_VIR32( GVGCR) & EN_DOT1X_GVR2CPU))
			return MRET_TOCPU;		
		return  MRET_DROP;
}
/*
@func void | 	| This function will set destination port list with cpu port
@parm hsb_param_t* | hsb | Header Stamp of Before (for input)
@parm hsa_param_t* | hsa | Header Stamp of After (for output)
@comm 
*/
void modelCPUport(hsb_param_t* hsb,hsa_param_t* hsa,ale_data_t *ale)
{

	hsa->dp=PM_PORT_NOTPHY;
	hsa->dpext=PM_PORT_CPU;
	modelCheckIgnore1QTag(hsb,hsa,ale);
}
/*
@func void | 	| This function will drop packet
@parm hsb_param_t* | hsb | Header Stamp of Before (for input)
@parm hsa_param_t* | hsa | Header Stamp of After (for output)
@comm 
*/
void modelDrop(hsb_param_t* hsb,hsa_param_t* hsa,ale_data_t *ale)
{

	
	if ((hsa->dpext & PM_PORT_CPU) && (hsa->dp &PM_PORT_NOTPHY))
	{
	
		hsa->dp &=PM_PORT_NOTPHY;
		hsa->dpext &= PM_PORT_CPU;

	}
	else
	{
		hsa->dp=0;
		hsa->dpext=0;

	}
	modelCheckIgnore1QTag(hsb,hsa,ale);
}

/*
@func void | 	| This function will add cpu port into port list.
@parm hsb_param_t* | hsb | Header Stamp of Before (for input)
@parm hsa_param_t* | hsa | Header Stamp of After (for output)
@comm 
*/

void modelAddCPUport(hsb_param_t* hsb,hsa_param_t* hsa)
{

	hsa->dp|=PM_PORT_NOTPHY;
	hsa->dpext |=PM_PORT_CPU;
}



void modelAssignMember(hsb_param_t* hsb,hsa_param_t* hsa,uint32 member,uint32 extMember)
{
	if (hsb->spa<RTL8651_MAC_NUMBER)
	{
	
		hsa->dp = member &(~ (1<<hsb->spa ));
		hsa->dpext= extMember;
	}
	else
	{
		hsa->dpext= extMember & (~(1<<hsb->extspa));
		hsa->dp = member;
	}
	if (hsa->dpext)
	{
		 hsa->dp |= PM_PORT_NOTPHY;
	}

	
}
void modelDeleteExtMember(hsb_param_t* hsb,hsa_param_t* hsa,uint32 port)
{

	hsa->dpext &= ~(1<<port);
	if (hsa->dpext==0)
	{
		hsa->dp &= ~(PM_PORT_NOTPHY);
		
	}
	
}
void modelDeleteMember(hsb_param_t* hsb,hsa_param_t* hsa,uint32 port)
{
	hsa->dp  = hsa->dp & (~(1<<port));						
}				

/*
@func void | 	| This function is used to Debug
@parm char* | hsb | Header Stamp of Before (for input)
@parm hsa_param_t* | hsa | Header Stamp of After (for output)
@comm 
*/

 void  __modelDebugDumpVLAN(char *filen,int line,rtl865x_tblAsicDrv_vlanParam_t *vlanp)
{
	sprintf(msg,"enter %s %d  ",filen,line);
	MT_WATCH(msg);
#if 0 /* Louis: no vid, and macAddr in VLAN table */
	sprintf(msg,"VLAN ID %d  ",vlanp->vid);
	MT_WATCH(msg);
	sprintf(msg, "MAC %02X %02X %02X %2X %02X %02X" ,
		vlanp->macAddr.octet[0],
		vlanp->macAddr.octet[1],
		vlanp->macAddr.octet[2],
		vlanp->macAddr.octet[3],
		vlanp->macAddr.octet[4],
		vlanp->macAddr.octet[5]);
#endif

	MT_WATCH(msg);

}

/*
@func int32 | modelGetProtocolBasedVLAN | get corresponding table index of protocol-based VLAN
@parm uint32 | ruleNo  | rule Number (1~6)
@parm uint8* | port    | (output) 0~4:PHY  5:MII  6~8:ExtPort
@parm uint8* | Tag | (output) VLAN ID and Priority
@rvalue SUCCESS | 
@comm
 */
int32 modelGetProtocolBasedVLAN( uint32 ruleNo, uint32 port, uint8* valid, uint32* tag)
{
	uint32 value;

	assert( ruleNo > 0 && ruleNo < RTL8651_PBV_RULE_MAX );
	assert( port < RTL8651_AGGREGATOR_NUMBER );
	ruleNo=ruleNo-1;
	if ( port < RTL865XC_PORT_NUMBER )
	{
		// Port0 ~ Port9		
		value =  READ_VIR32(PBVR0_0+(ruleNo*5*4) +(port/2)*4 );
		if (port&0x1)
			value =(value & 0xffff0000)>>16;
		else
			value &= 0x0000ffff ;		
		if ( valid ) *valid = value & 1;
		if ( tag ) *tag = value >> 1;
	}
	


	return SUCCESS;
}
/*
@func int32  | 	| This function is used to get PVID
@parm uint32 | port |  port number
@parm uint32* | pvid| VID  (for output)
@comm 
*/

//extension port ready.
int32 modelGetAsicPvid(uint32 port, uint32 *pvid, int32 *priority) {
	uint16 offset;

	offset=(port*2)&(~0x3);
	if(pvid == NULL)
		return FAILED;
	if((port&0x1))
	{
		*pvid=((READ_VIR32(PVCR0+offset)>>16)&0xFFF);		
		*priority=((READ_VIR32(PVCR0+offset)>>28)&0x7);		
	}
	else
	{
		*pvid=(READ_VIR32(PVCR0+offset)&0xFFF);
		*priority=((READ_VIR32(PVCR0+offset)>>12)&0x7);
	}
	return SUCCESS;
}


/*
func uint16 | modelMacMask	| This function will return the vlan ID which this packet belongs.
parm int * | macNumber | total number of  Mac Address
rvalue uint16 | mask
comm 
*/

static uint16 modelMacMask(int macNumber)
{
	switch(macNumber)
	{
		case 0:
		case 1:
			return 0xff;
		case 2:
			return 0xfe;
		case 4:
			return 0xfc;
		case 8:
			return 0xf8;
	}
	return 0;
}
/*
func uint16 | modelFindVlanId	| This function will return the vlan ID which this packet belongs.
parm hsb_param_t* | hsb | Header Stamp of Before (for input)
parm hsa_param_t* | hsa | Header Stamp of After (for output)
rvalue uint16 | VLAN ID (0~4095)
comm 
According the following procedure to decide VLAN ID:
 1. 802.1q VLAN tag existed
 2. protocol-based VLAN
 3. Port-based VLAN (PVID)
*/
static int32 modelFindVlanId( hsb_param_t* hsb, hsa_param_t* hsa, ale_data_t *ale )
{
	uint8 i, valid;
	int32 priority;
	uint16 mask;
	uint32 tag,vlanId;
	rtl865x_tblAsicDrv_intfParam_t intf;
	
	valid = FALSE;
	memset((void*)&intf,0,sizeof(intf));
	ale->matchSrcNetif=FALSE;
	vlanId=0;
	if ( hsb->tagif )
	{
		sprintf(msg,"VLAN TAG packet %d  priority %d   ",hsb->vid, hsb->hiprior);
		MT_WATCH(msg);
	}	
	if (( (hsb->tagif) &&(hsb->vid!=0))  && ((READ_VIR32(VCR0) &EN_1QTAGVIDIGNORE)!=EN_1QTAGVIDIGNORE))
	{
		/* 802.1q VLAN ID present */
		vlanId = hsb->vid;	
		priority = hsb->hiprior;
		sprintf(msg,"1Q vlan unware disabled, therefore, tagged VLAN(%d) and priority(%d) is valid.", vlanId, priority );
		MT_WATCH(msg);
		goto out;
	}
	else 
	{
		/* traverse protocol-based vlan */
		uint8 ptypep, ptype5, ptype6;
		uint16 pvalue5, pvalue6,spa;
		
		if (hsb->spa==7)
			spa=hsb->extspa+6;
		else
			spa=hsb->spa;

		/* LLC_OTHER and SNAP is exclusive */
		switch ( hsb->ethtype )
		{

			case 0xE0E0: /* IPX */
				if ((hsb->llcothr) &&(hsb->snap==0))
					modelGetProtocolBasedVLAN( 1, spa, &valid, &tag);
				break;
			case 0x8040: /* NetBios */
				if (hsb->snap==0 && hsb->llcothr==0)
					modelGetProtocolBasedVLAN( 2, spa, &valid, &tag);
				break;
			case 0x8863: /* PPPoE Control */
				if (hsb->snap==0 && hsb->llcothr==0)
					modelGetProtocolBasedVLAN( 3, spa, &valid, &tag);
				break;
			case 0x8864: /* PPPoE Data */
				if (hsb->snap==0 && hsb->llcothr==0)
					modelGetProtocolBasedVLAN( 4, spa, &valid, &tag);
				break;
			default:
				rtl8651_queryProtocolBasedVLAN( 5, &ptype5, &pvalue5 );
				rtl8651_queryProtocolBasedVLAN( 6, &ptype6, &pvalue6 );
				if ( hsb->llcothr )
					ptypep = 0x2; /* LLC-other, see PBVCR and PBVCR2 */
				else if	(hsb->snap)
					ptypep = 0x1; /* LLC-other, see PBVCR and PBVCR2 */
				else
					ptypep = 0x0; /* Ethernet II, see PBVCR and PBVCR2 */				
				if ( ptype5==ptypep && hsb->ethtype==pvalue5 )
					modelGetProtocolBasedVLAN(5,spa, &valid, &tag);
				else if ( ptype6==ptypep && hsb->ethtype==pvalue6 )
					modelGetProtocolBasedVLAN(6, spa, &valid, &tag);
		}
		
		if ( valid )
		{
			vlanId=tag&0xfff; /* 12 bits VLANID 3bits is priority*/
			priority = (tag>>12)&0x7;
			sprintf(msg,"Protocol-based and Port-based VLAN: VID=%d Priority=%d", vlanId, priority );
			MT_WATCH(msg);
			goto out;
		}

		/* Use PVID */
		if (hsb->spa==7)
		{
			modelGetAsicPvid(hsb->extspa+RTL8651_MAC_NUMBER,&vlanId,&priority);
			sprintf(msg,"No VLAN tag (from CPU and EXT*), get from Port-based setting: VID=%d Priority=%d", vlanId, priority );
			MT_WATCH(msg);
		}
		else
		{
			modelGetAsicPvid(hsb->spa,&vlanId,&priority);
			sprintf(msg,"No VLAN tag (from port %d), get from Port-based setting: VID=%d Priority=%d", hsb->spa, vlanId, priority );
			MT_WATCH(msg);
		}

	}


out: /* assert vlanIdx is set. */
		if (hsb->tagif)
			priority = hsb->hiprior;
		ale->priorityControl[PRI_1Q] =(READ_VIR32(LPTM8021Q)>>(priority *3))&0x7;	
		sprintf(msg,"After Mapgging 1Q Base Priority %d",ale->priorityControl[PRI_1Q] );
		MT_WATCH(msg);

		
		ale->DMACIsGateway=0;
		ale->multiLayerMode =  (READ_VIR32(SWTCR0)&LIMDBC_MASK);
		
		switch(ale->multiLayerMode)
		{
			case LIMDBC_VLAN:
				for (i=0;i<RTL865XC_NETIFTBL_SIZE;i++)
				{
					rtl8651_getAsicNetInterface(i,&intf);
					mask = modelMacMask(intf.macAddrNumber);
					if (intf.valid && (intf.vid==vlanId))
					{

						ale->matchSrcNetif=TRUE;
						ale->srcNetifIdx=i;						
						if ( (intf.macAddr.octet[5]==(hsb->da[5] & mask)) && (intf.macAddr.octet[4]==hsb->da[4]) &&
					  	(intf.macAddr.octet[3]==hsb->da[3]) &&(intf.macAddr.octet[2]==hsb->da[2]) &&
						(intf.macAddr.octet[1]==hsb->da[1]) &&(intf.macAddr.octet[0]==hsb->da[0]) 
						)		
						{
							
							ale->DMACIsGateway=1;
						}
						break;
					}

				}
				break;
			case LIMDBC_PORT:	
				ale->srcNetifIdx=READ_VIR32(PLITIMR) >> ((hsb->spa)*3)&0x3;
				rtl8651_getAsicNetInterface(ale->srcNetifIdx,&intf);
				if (intf.valid)
				{
					ale->matchSrcNetif=TRUE;
					mask = modelMacMask(intf.macAddrNumber);
					if ( (intf.macAddr.octet[5]==(hsb->da[5] & mask)) && (intf.macAddr.octet[4]==hsb->da[4]) &&
				  	(intf.macAddr.octet[3]==hsb->da[3]) &&(intf.macAddr.octet[2]==hsb->da[2]) &&
					(intf.macAddr.octet[1]==hsb->da[1]) &&(intf.macAddr.octet[0]==hsb->da[0]) 
					)		
					{
						ale->DMACIsGateway=1;
					}
				}
				break;
			case LIMDBC_MAC:
				for (i=0;i<(RTL865XC_NETIFTBL_SIZE>>2);i++)   /*  The only first four entrys are valid refenrece when  Mac Mode */
				{
					rtl8651_getAsicNetInterface(i,&intf);
					if (intf.valid)
					{

						mask = modelMacMask(intf.macAddrNumber);
						if ( (intf.macAddr.octet[5]==(hsb->da[5] & mask)) && (intf.macAddr.octet[4]==hsb->da[4]) &&
					  	(intf.macAddr.octet[3]==hsb->da[3]) &&(intf.macAddr.octet[2]==hsb->da[2]) &&
						(intf.macAddr.octet[1]==hsb->da[1]) &&(intf.macAddr.octet[0]==hsb->da[0]) 
						)		
						{
							ale->srcNetifIdx=i;						
							ale->DMACIsGateway=1;
							ale->matchSrcNetif=TRUE;

							break;
						}

					}
				}
				break;
			default:
				MT_TOCPU("Reserved Multi Mode Operatoin!!!!!!!!");
				return MRET_TOCPU;
				
		}
	if (ale->matchSrcNetif==TRUE)		
	{
		memcpy((void*)&ale->srcNetif,(void*)&intf,sizeof(intf));
		memcpy((void*)&ale->dstNetif,(void*)&intf,sizeof(intf));
		ale->matchDstNetif = TRUE;
	}
	ale->aleInternalSvid=(vlanId)&0xfff;;
	sprintf(msg,"Layer2 Internal VLAN %d(hex:%x) ",ale->aleInternalSvid,ale->aleInternalSvid);
	MT_WATCH(msg);
	if ((READ_VIR32(MSCR) & (EN_L3|EN_L4)) &&	 (ale->DMACIsGateway) && (hsb->type!=HSB_TYPE_ETHERNET))
	{
		ale->doL3L4=1;	
		ale->doL3=1;
	}

	sprintf(msg,"ale->doL3=%d doL4=%d doL3L4=%d", ale->doL3, ale->doL4, ale->doL3L4 );
	MT_WATCH(msg);
		
	return (ale->aleInternalSvid);

}


/*
func enum MODEL_RETURN_VALUE | model802_1dAddress	| model code to trap 802.1d reserved address
parm hsb_param_t* | hsb | Header Stamp of Before (for input)
parm hsa_param_t* | hsa | Header Stamp of After (for output)
rvalue enum MODEL_RETURN_VALUE |
comm 
*/
enum MODEL_RETURN_VALUE model802_1dAddress( hsb_param_t* hsb, hsa_param_t* hsa,ale_data_t *ale )
{
	uint32 value;
	
	/**************************************************************************
	Process HSB First Entry 
	**************************************************************************/
	hsa->svid=hsb->vid;
	hsa->spao=hsb->spa;
	value = READ_VIR32(RMACR);

	if ((value==0)|| ((hsb->da[0]&0x1)==0)) return MRET_OK;
	if ((value & MADDR00 )&& ( hsb->da[0]==0x01 && hsb->da[1]==0x80 && hsb->da[2]==0xC2 &&
	  ( ( hsb->da[3] || hsb->da[4] ||hsb->da[5])==0x00) ))
	{
		modelCPUport(hsb,hsa,ale);
		MT_TOCPU( "To CPU due to 802.1d address" );
		return MRET_TOCPU;
	}
	if ((value & MADDR02)&& ( hsb->da[0]==0x01 && hsb->da[1]==0x80 && hsb->da[2]==0xC2 &&
		 hsb->da[3]==0x00 && hsb->da[4]==0x00 && hsb->da[5]==0x02 ))
	{
		modelCPUport(hsb,hsa,ale);
		MT_TOCPU( "To CPU due to 01-80-C2-00-00-02 IEEE Std 802.3ad Slow_Protocols-Multicast address" );
		return MRET_TOCPU;
	}
	if ((value & MADDR03)&& ( hsb->da[0]==0x01 && hsb->da[1]==0x80 && hsb->da[2]==0xC2 &&
		 hsb->da[3]==0x00 && hsb->da[4]==0x00 && hsb->da[5]==0x03 ))
	{
		modelCPUport(hsb,hsa,ale);
		MT_TOCPU( "To CPU due to  01-80-C2-00-00-03 IEEE Std 802.1X PAE address" );
		return MRET_TOCPU;
	}
	if ((value & MADDR0E)&& ( hsb->da[0]==0x01 && hsb->da[1]==0x80 && hsb->da[2]==0xC2 &&
		 hsb->da[3]==0x00 && hsb->da[4]==0x00 && hsb->da[5]==0x0E ))
	{
		modelCPUport(hsb,hsa,ale);
		MT_TOCPU( "To CPU due to  01-80-C2-00-00-0E IEEE Std. 802.1AB Link Layer Discovery protocol multicast address" );
		return MRET_TOCPU;
	}
	if ((value & MADDR10)&& ( hsb->da[0]==0x01 && hsb->da[1]==0x80 && hsb->da[2]==0xC2 &&
		 hsb->da[3]==0x00 && hsb->da[4]==0x00 && hsb->da[5]==0x10 ))
	{
		modelCPUport(hsb,hsa,ale);
		MT_TOCPU( "To CPU due to  01-80-C2-00-00-10   All LANs Bridge Management Group Address" );
		return MRET_TOCPU;
	}
	if ((value & MADDR20)&& ( hsb->da[0]==0x01 && hsb->da[1]==0x80 && hsb->da[2]==0xC2 &&
		 hsb->da[3]==0x00 && hsb->da[4]==0x00 && hsb->da[5]==0x20 ))
	{
		modelCPUport(hsb,hsa,ale);
		MT_TOCPU( "To CPU due to  01-80-C2-00-00-20 GMRP Address" );
		return MRET_TOCPU;
	}
	if ((value & MADDR21)&& ( hsb->da[0]==0x01 && hsb->da[1]==0x80 && hsb->da[2]==0xC2 &&
		 hsb->da[3]==0x00 && hsb->da[4]==0x00 && hsb->da[5]==0x21 ))
	{
		modelCPUport(hsb,hsa,ale);
		MT_TOCPU( "To CPU due to  01-80-C2-00-00-20 GVRP Address" );
		return MRET_TOCPU;
	}
	if ((value & MADDR00_10)&& ( hsb->da[0]==0x01 && hsb->da[1]==0x80 && hsb->da[2]==0xC2 &&
		 hsb->da[3]==0x00 && hsb->da[4]==0x00 && ((hsb->da[5]&0xF0)==0x00 || (hsb->da[5]==0x10)) ))
	{
		modelCPUport(hsb,hsa,ale);
		MT_TOCPU( "To CPU due to  01-80-C2-00-00-00~01-80-C2-00-00-10 802.1d Reserved Address" );
		hsa->why2cpu = 0x2;		/*pre-ACL filtering trap. set ph_reason field bit 1 to 1*/
		return MRET_TOCPU;
	}
	if ((value & MADDR20_2F)&& ( hsb->da[0]==0x01 && hsb->da[1]==0x80 && hsb->da[2]==0xC2 &&
		 hsb->da[3]==0x00 && hsb->da[4]==0x00 && ((hsb->da[5]&0xF0)==0x20) ))
	{
		modelCPUport(hsb,hsa,ale);
		MT_TOCPU( "To CPU due to  01-80-C2-00-00-00~01-80-C2-00-00-10 802.1d Reserved Address" );
		return MRET_TOCPU;
	}

	return MRET_OK;
}
/*
func enum MODEL_RETURN_VALUE | modelSpann	| model code to trap 802.1d reserved address
parm hsb_param_t* | hsb | Header Stamp of Before (for input)
parm hsa_param_t* | hsa | Header Stamp of After (for output)
rvalue enum MODEL_RETURN_VALUE |
comm 
*/
enum MODEL_RETURN_VALUE modelSpanningTree( hsb_param_t* hsb, hsa_param_t* hsa,ale_data_t *ale )
{
	int ingressSpa;
	if ((READ_VIR32(MSCR)&EN_STP)==0) return MRET_OK;
	if (hsb->spa<RTL8651_MAC_NUMBER)
	{
		ingressSpa= hsb->spa;
	}
	else
	{
		ingressSpa = RTL8651_MAC_NUMBER+ hsb->extspa;
	}
	PSpanningStatus = ((READ_VIR32(PCRP0+ingressSpa*PCRP_SIZE))>>STP_PortST) &STP_PortST_BITMASK;
	switch( PSpanningStatus)
	{
		case M1D_DISABLE:
				modelDrop(hsb,hsa,ale);
		      		MT_DROP( "Port status is disable" );
				return MRET_DROP;			
		case M1D_BLOCKING:
				modelDrop(hsb,hsa,ale);
		      		MT_DROP( "Port status is blocking" );
				return MRET_DROP;
	}
	return MRET_OK;
}


/*
func enum MODEL_RETURN_VALUE | modelIngress802_1X	| model code to trap 802.1X Control
parm hsb_param_t* | hsb | Header Stamp of Before (for input)
parm hsa_param_t* | hsa | Header Stamp of After (for output)
rvalue enum MODEL_RETURN_VALUE |
comm 
*/
enum MODEL_RETURN_VALUE modelIngress802_1X( hsb_param_t* hsb, hsa_param_t* hsa,ale_data_t *ale )
{
	int ingressSpa;
	if (hsb->spa<RTL8651_MAC_NUMBER)
	{
		ingressSpa= hsb->spa;
	}
	else
	{
		ingressSpa = RTL8651_MAC_NUMBER+ hsb->extspa;
	}

	dot1x_process=(READ_VIR32(GVGCR) >>GVLAN_PROCESS_BITS) &0x3;	
	srcMacControl = READ_VIR32(DOT1XMACCR) &( 1<<ingressSpa);
	src1XPortControl = READ_VIR32(DOT1XPORTCR) ;
	src1XPortControl = (src1XPortControl>>(ingressSpa*3)) &0x7;	
	ToGuestVlan=0;

	if ((src1XPortControl&EN_8021X_PORT_CONTROL)==0) 
		return MRET_OK;
	MT_WATCH( "dot1X Port Control Enable" );
	if (src1XPortControl & EN_8021X_PORT_AUTH)
	{
		MT_WATCH( "dot1X Port Authenticated" );
		return MRET_OK;
	}
	MT_WATCH( "dot1X Port is Unauthenticated" );
	switch(dot1x_process)
	{
		case DOT1X_PROCESS_DROP:		
			MT_TOCPU( "Igress Drop due to Port Not Unauthized" );
			return MRET_DROP;
		case DOT1X_PROCESS_TOCPU:		
			modelCPUport(hsb,hsa,ale);
			MT_TOCPU( "Igress to CPU due to Port Not Unauthized" );
			hsa->why2cpu |= 1;		/*set ph_reason field bit 0 to 1*/
			return MRET_TOCPU;
		case DOT1X_PROCESS_GVLAN:
			MT_WATCH("To Guest Vlan");
			ToGuestVlan=1;
			return MRET_OK;
		default:
			modelDrop(hsb,hsa,ale);
			MT_DROP( "Drop due to Reserved dot1x Portbase Process" );
			return MRET_DROP;
	
	}

}
/*
func enum MODEL_RETURN_VALUE | modelEgress802_1X	| model code to trap 802.1X Control
parm hsb_param_t* | hsb | Header Stamp of Before (for input)
parm hsa_param_t* | hsa | Header Stamp of After (for output)
rvalue enum MODEL_RETURN_VALUE |
comm 
*/
enum MODEL_RETURN_VALUE modelEgress802_1X( hsb_param_t* hsb, hsa_param_t* hsa ,ale_data_t *ale)
{
	uint32 i,dst1XPortControl,dot1XMacControl,dstMacControl,dpc;
	dst1XPortControl= READ_VIR32(DOT1XPORTCR);
	dot1XMacControl= READ_VIR32(DOT1XMACCR);

	sprintf(msg,"Before 1X_ Egress  portList %x ext %x dot1xProcess %d ",hsa->dp,hsa->dpext,dot1x_process);
	MT_WATCH(msg);
	dstMacControl=0;
	if (hsa->ipmcastr)
	{
		MT_WATCH("Ignore 8021X Egress Check due to IP multicast forward");
		return MRET_OK;
	}

	/**************************************************************************
	Tthe dmac is authenticated, when the destination port is only one.
	**************************************************************************/
	dpc=0;	
	for (i=0;i<RTL8651_MAC_NUMBER;i++)
	{		
		if (hsa->dp & (1<<i))
		{
			dstMacControl= (dot1XMacControl>>i )&0x1;
			dstPortControl  = (READ_VIR32(DOT1XPORTCR) >>(i*3))&0x7;			
			if (dstMacControl &&((dot1XMacControl &EN_DOT1XMAC_OPDIR_IN)==0))
			{
				sprintf(msg,"DestPort %d MacControl Enable ",i);			
				MT_WATCH(msg);
			}
			dpc++;
		}

	}
	for (i=0;i<3;i++)
	{		
		if (hsa->dpext & (1<<i))			
		{
			
			dstMacControl= (READ_VIR32(DOT1XMACCR) >>(i+RTL8651_MAC_NUMBER))&0x1;
			dstPortControl  = (READ_VIR32(DOT1XPORTCR) >>((i+RTL8651_MAC_NUMBER)*3))&0x7;			
			if (dstMacControl &&((dot1XMacControl &EN_DOT1XMAC_OPDIR_IN)==0))
			{
				sprintf(msg,"DestPort %d MacControl Enable ",i+RTL8651_MAC_NUMBER);			
				MT_WATCH(msg);
			}
			dpc++;
		}

	}


	/*Decision whether forward  if Packet belong Gusest Vlan  */
	if ((ToGuestVlan) &&(dmac_auth) &&(dstMacControl) &&((dot1XMacControl&EN_DOT1XMAC_OPDIR_IN)==0)&&(dpc==1))
	{	


		if (( READ_VIR32(GVGCR)& EN_GVOPDIR)==0)
		{
				MT_DROP("Due SA is disallowed to talk to authorized DA");
				modelDrop( hsb,  hsa,ale);
				return MRET_DROP;
		}
	}	

	if (ToGuestVlan) return MRET_OK;
	


		
	
	for (i=0;i<RTL8651_PORT_NUMBER;i++)
	{

	
		if (hsa->dp & (1<<i))
		{	
			if  (((dot1XMacControl>>i) & EN_8021X_MAC_CONTROL))
			{
				if (((dot1XMacControl & EN_DOT1XMAC_OPDIR_IN) ==0) && (dmac_auth==0) )
				{
					sprintf( msg, "Port %d :dot1X MAC Direction(BOTH) : Remove due to unauthenticated", i );
					MT_WATCH(msg);
					modelDeleteMember(hsb,hsa,i);
					continue;
				}
			}
			if ((dst1XPortControl>>(i*3)) & EN_8021X_PORT_CONTROL)
			{
				if ( (dst1XPortControl>>(i*3)) &  EN_8021X_PORT_DIR_IN)
				{
					sprintf( msg, "Port  %d : dot1X PortBase(In)  Enable", i );
					MT_WATCH(msg);
					continue;
				}
				if ( (dst1XPortControl>>(i*3)) & EN_8021X_PORT_AUTH)
				{
					sprintf( msg, "Port  %d : dot1X PortBase Enable : Authenticated", i );
					MT_WATCH(msg);
					continue;
				}
				else
				{

					sprintf( msg, "Port  %d : dot1X PortBase Enable: Remove due to unauthenticated", i );
					MT_WATCH(msg);					
					modelDeleteMember(hsb,hsa,i);
					sprintf(msg,"After Remove  portList %x ext %x dot1xProcess %d ",hsa->dp,hsa->dpext,dot1x_process);
					MT_WATCH(msg);

				}
			}
		}
		
	}
	for (i=0;i<3;i++)
	{
		if (hsa->dpext& (1<<i))
		{
			if  ((dot1XMacControl>>(RTL8651_MAC_NUMBER+i)) & EN_8021X_MAC_CONTROL)
			{
				sprintf( msg, "ExtPort %d :dot1X  MacBase Enable " , i);
				MT_WATCH(msg);
				if (((dot1XMacControl & EN_DOT1XMAC_OPDIR_IN) ==0) && (dmac_auth==0))
				{
					sprintf( msg, "ExtPort %d :dot1X  MacBase Enable(Both): Remove due to unauthenticated", i);
					MT_WATCH(msg);
					modelDeleteExtMember(hsb,hsa,i);
					continue;
				}
			}

			if ((dst1XPortControl>>((i+RTL8651_MAC_NUMBER)*3)) &EN_8021X_PORT_CONTROL)
			{
				sprintf( msg, "ExtPort  %d : dot1X PortBase  Enable", i );
				MT_WATCH(msg);
				if ( (dst1XPortControl>>((i+RTL8651_MAC_NUMBER)*3)) &EN_8021X_PORT_DIR_IN)					
				{
					continue;
				}
				if ( (dst1XPortControl>>((i+RTL8651_MAC_NUMBER)*3)) &EN_8021X_PORT_AUTH)
					continue;
				else
				{
					sprintf( msg, "ExtPort %d :dot1X PortBase Enable(Both): Remove due to unauthenticated", i );
					MT_WATCH(msg);
					modelDeleteExtMember(hsb,hsa,i);
					
				}
			}
			
		}
	}

	return MRET_OK;
}


/*
func enum MODEL_RETURN_VALUE | modelIngressCheck	| model code for ingress check
parm hsb_param_t* | hsb | Header Stamp of Before (for input)
parm hsa_param_t* | hsa | Header Stamp of After (for output)
rvalue enum MODEL_RETURN_VALUE |
comm 
*/
enum MODEL_RETURN_VALUE modelIngressCheck( hsb_param_t* hsb, hsa_param_t* hsa, ale_data_t *ale )
{
	rtl865xc_tblAsic_vlanTable_t vlan;
	uint32 vid;
	uint32 ingressSpa,vcr0;

	
	vid = ( modelFindVlanId( hsb, hsa, ale ) );
	hsa->dvid= vid;
	if (hsb->spa<=5)
	{
		ingressSpa= hsb->spa;
	}
	else
	{
		ingressSpa = 6 + hsb->extspa;
	}
	ale->priorityControl[PRI_PBP]=(READ_VIR32(PBPCR)>>(ingressSpa*3))&0x7;
	ale->priorityControl[PRI_ACL]=-1;
	ale->priorityControl[PRI_DSCP]=-1;
	ale->priorityControl[PRI_NAPT]=-1;
	
	vcr0=READ_VIR32(VCR0);
	switch ( (vcr0>>(ingressSpa*2+P0_AcptFType_OFFSET))&0x3)
	{
		case 0:
		/* Admit  all frame */
			break;
		case 1:
		/* Admit  1Q VLAN Tag */			
			if((hsb->tagif==0) ||(hsb->tagif &&hsb->vid==0) ) 
			{
				modelDrop(hsb,hsa,ale);
				sprintf( msg, "Due to only admit  1Q VLAN Tag");
				MT_DROP( msg );
				return MRET_DROP;
			}
			break;
		case 2:
		/* Admit un-tag frame + priority-tag frame. */
			if ((hsb->tagif) && hsb->vid)
			{
				modelDrop(hsb,hsa,ale);
				sprintf( msg, "Due to only admit un-tag frame + priority-tag frame");
				MT_DROP( msg );
				return MRET_DROP;

			}
			break;
		default:
				modelDrop(hsb,hsa,ale);
				sprintf( msg, "Due to Reserved Setting");
				MT_DROP( msg );
				return MRET_DROP;
			
	}
	/* Get the VLAN member  */
	_rtl8651_readAsicEntry(TYPE_VLAN_TABLE, vid, &vlan);
	if ( (vlan.memberPort==0) &&(vlan.extMemberPort==0))
	{
		if ( READ_VIR32(SWTCR0)&EnUkVIDtoCPU)
		{
			modelCPUport(hsb,hsa,ale);
			sprintf( msg, "To CPU due to VLAN(%d)  No member port.", vid );
			MT_TOCPU( msg );
			return MRET_TOCPU;
		}
		else 
		{
			modelDrop(hsb,hsa,ale);
			sprintf( msg, "Drop due to VLAN(%d)  No member port.", vid );
			MT_DROP( msg );
			return MRET_DROP;
		}

	}
	/* Check VLAN ingress filtering only when enabled. */
	if( (vcr0>>ingressSpa) & EN_VLAN_INGRESS_FILTER )
	{

		/* Verify the member port of VLAN. Drop this packet if the packet is coming from the non-existed member port. */
		if (( (hsb->spa==7) &&(( (1<<hsb->extspa) & vlan.extMemberPort) == 0)) ||
			((hsb->spa<RTL8651_MAC_NUMBER) && (( 1<<(hsb->spa) & vlan.memberPort ) ==0)))		
		{
			modelDrop(hsb,hsa,ale);
			sprintf( msg, "Ingress Drop due to source port %d is not in VLAN(%d)'s member port.", ingressSpa, vid );
			MT_DROP( msg );
			return MRET_DROP;
		}
		sprintf( msg, "SPA %d Ingress Filter Pass vid %d", ingressSpa, vid );
		MT_WATCH( msg );

	}
	return MRET_OK;
}
/*
@func enum MODEL_RETURN_VALUE | modelDpcCounter	| model code for calcuate dpc counter
@parm hsb_param_t* | hsb | Header Stamp of Before (for input)
@parm hsa_param_t* | hsa | Header Stamp of After (for output)
@rvalue enum MODEL_RETURN_VALUE |
@comm 
*/
enum MODEL_RETURN_VALUE modelDpcCounter( hsb_param_t* hsb, hsa_param_t* hsa )
{
	int i;
	hsa->dpc=0;
	for (i=0;i<RTL8651_MAC_NUMBER;i++)
	{		
		if (hsa->dp & (1<<i))
		{
			hsa->dpc++;
		}

	}
	for (i=0;i<3+1/*CPU*/;i++)
	{		
		if (hsa->dpext & (1<<i))			
		{
			hsa->dpc++;
		}
	}

	return MRET_OK;
}

/*
@func enum MODEL_RETURN_VALUE | modelEgressCheck	| model code for ingress check
@parm hsb_param_t* | hsb | Header Stamp of Before (for input)
@parm hsa_param_t* | hsa | Header Stamp of After (for output)
@rvalue enum MODEL_RETURN_VALUE |
@comm 
*/
enum MODEL_RETURN_VALUE modelEgressCheck( hsb_param_t* hsb, hsa_param_t* hsa,ale_data_t *ale )
{

	rtl865xc_tblAsic_vlanTable_t vlan;
	uint32 vid,i;
	uint32 dp=hsa->dp;
	uint32 extdp=hsa->dpext;
	int32 retval;
	
	vid=hsa->dvid;
	
	sprintf(msg,"Before Vlan Egress destiont port list  0x%x ext 0x%x ...",hsa->dp,hsa->dpext);
	MT_WATCH(msg);	
	/* 802.1D Spanning Tree Status Check*/
	if (READ_VIR32(MSCR)&EN_STP)
	{
		switch(PSpanningStatus)
		{
			case M1D_LEARNING:
				modelDrop(hsb,hsa,ale);
				MT_DROP("Port Status is Learning");
				retval = MRET_DROP;
				goto out;
		}
		for (i=0;i<RTL8651_MAC_NUMBER;i++)
		{
		 	/*Spanning Tree Protocol Port State Control 
	 		00 = Disable State 01 = Blocking/Listen State 10 = Learning State 11 = Forwarding State
		 	*/
			if ((READ_VIR32(PCRP0+(i<<2))&0x060)!=0x60)
				hsa->dp  &= ~(1<<i);
			
		}
		/* Extenstion Port*/
		for (i=0;i<3;i++)
		{
		 	/*Spanning Tree Protocol Port State Control 
		 	00 = Disable State 01 = Blocking/Listen State 10 = Learning State 11 = Forwarding State
		 	*/
			if ((READ_VIR32(PCRP0+((i+RTL8651_MAC_NUMBER)<<2))&0x060)!=0x60)
				modelDeleteExtMember( hsb, hsa, i);
		
		}
	}
	modelCheckIgnore1QTag(hsb,hsa,ale);
	if (hsa->ipmcastr)
	{
		sprintf(msg," IP mulitcast Vlan Egress destiont port list  %x ext %x ...",hsa->dp,hsa->dpext);
		MT_WATCH(msg);
		modelDpcCounter(hsb,hsa);
		hsa->hwfwrd=1;
		retval = MRET_OK;
		goto out;
	}
	/* Get the VLAN index which packet belongs. */
	if (ToGuestVlan) 
	{
		sprintf(msg,"Vlan Egress Guest VID %d",gvid);
		MT_WATCH(msg);
		_rtl8651_readAsicEntry(TYPE_VLAN_TABLE, gvid, &vlan);	
	}
	else
	{
		sprintf(msg,"Vlan Egress VID %d ",vid);
		MT_WATCH(msg);
		_rtl8651_readAsicEntry(TYPE_VLAN_TABLE, vid, &vlan);
	}
	sprintf( msg, "Hardware Lookup Finish");
	MT_WATCH(msg);

	hsa->hwfwrd=1;
	/* Verify the member port of VLAN. Drop this packet if the packet is coming from the non-existed member port. */	
	if ( ((dp & vlan.memberPort ) ==0) &&((extdp & vlan.extMemberPort) == 0) && ((extdp & PM_PORT_CPU) !=PM_PORT_CPU))		
	{
		sprintf( msg, "Egress Filter destination port list %x ext:%x  is not in VLAN(%d)'s member port.", dp,extdp, vid );
		MT_DROP(msg);
		modelDrop( hsb, hsa,ale);
		retval = MRET_DROP;
		goto out;

	}
	/* remove non vlan member  (NON PHY Port carefully)*/
	hsa->dp = hsa->dp&(vlan.memberPort | PM_PORT_NOTPHY);
	for (i=0;i<3;i++)
	{
		if ( (hsa->dpext&(1<<i)) && (((1<<i)&vlan.extMemberPort)==0) )
		modelDeleteExtMember( hsb, hsa, i);
	}
	hsa->dvtag = hsa->dvtag& ~(vlan.egressUntag);
	hsa->dvtag= hsa->dvtag  & ~(vlan.extEgressUntag<<RTL8651_MAC_NUMBER);
	modelDpcCounter(hsb,hsa);
	sprintf(msg,"After Vlan Egress destiont port list  dp=0x%x dpext=0x%x ...",hsa->dp,hsa->dpext);
	MT_WATCH(msg);

	retval = MRET_OK;
out:
	/*
	 * If packet is forwarded to extension ports ONLY, set last matched ACL index in why2cpu.
	 */
	if ((hsa->dp&PM_PORT_NOTPHY) && 
	    ((hsa->dpext&0x8)==0) && (hsa->dpext&0x7))
	{
		hsa->why2cpu = ale->lastMatchedIngressACLRule;
		hsa->why2cpu |= ale->lastMatchedEgressACLRule<<7;
		sprintf(msg,"Set why2cpu as =0x%04x ...", hsa->why2cpu );
		MT_WATCH(msg);
	}
	
	sprintf(msg,"After Vlan Egress destiont port list  0x%x ext 0x%x ...",hsa->dp,hsa->dpext);
	MT_WATCH(msg);	
	return retval;
}

static int32 _cmpTableMac(ether_addr_t * srcP, ether_addr_t * dstP)
{
    return (     (srcP->octet[5] == dstP->octet[5]) &&
	   		(srcP->octet[4] == dstP->octet[4]) &&
	   		(srcP->octet[3] == dstP->octet[3]) &&
	   		(srcP->octet[2] == dstP->octet[2]) &&	   		
	   		(srcP->octet[1] == dstP->octet[1]) &&
	   		(srcP->octet[0] == dstP->octet[0]));
}
static uint32 l2SwitchTable_getHashIndex(uint8 *mac,uint8 fid)
{
    return  (mac[0] ^mac[1] ^mac[2]^mac[3]^mac[4]^mac[5] ^ (fidHash[fid]))<<2;
                   
}

static int32  _modelUpdateTimeMember(rtl865xc_tblAsic_l2Table_t *entryContent,int idx,hsb_param_t* hsb,hsa_param_t* hsa)
{
	int32 retval;
	retval=SUCCESS;
	if (entryContent->isStatic!=TRUE)
	{
		if (hsb->spa<RTL8651_MAC_NUMBER)
		{
			entryContent->memberPort=1<<hsb->spa;
			entryContent->extMemberPort=0;
		}
		else
		{
			entryContent->extMemberPort=1<<hsb->extspa;
			entryContent->memberPort=0;
		}
	}
	if (entryContent->isStatic)
	{
		if (hsb->spa<RTL8651_MAC_NUMBER)
		{
			if ((entryContent->memberPort &(1<<hsb->spa)) ==0)
			{
					retval= FAILED;
			}

		}
		else
		{
			if ((entryContent->extMemberPort & (1<<hsb->extspa)) ==0)
			{
					retval= FAILED;
			}

		}
	
	}
	entryContent->agingTime= 0x3;
	 _rtl8651_forceAddAsicEntry(TYPE_L2_SWITCH_TABLE, idx, entryContent);
	return retval;

}
		
/*
func enum MODEL_RETURN_VALUE | modelLayer2Switching	| model code for L2 switching
parm hsb_param_t* | hsb | Header Stamp of Before (for input)
parm hsa_param_t* | hsa | Header Stamp of After (for output)
rvalue enum MODEL_RETURN_VALUE |
comm 
*/
enum MODEL_RETURN_VALUE modelLayer2Switching( hsb_param_t* hsb, hsa_param_t* hsa ,ale_data_t *ale)
{
	uint32  eidx, i,j;
	int32 start,len;
	ether_addr_t mac,entryMac,macMask;
	uint8 fid;
	uint32 regValue;

	rtl865xc_tblAsic_l2Table_t   entryContent;
	rtl865xc_tblAsic_vlanTable_t vlanTable,gvlan;   	

	 dmac_auth=0;	 
	_rtl8651_readAsicEntry(TYPE_VLAN_TABLE, ale->aleInternalSvid,&vlanTable);
	fid=vlanTable.fid;
	smac_auth=0;
	if (ToGuestVlan)
		goto 	ToGuestVlanLabel;
	if (srcMacControl)
	/* Need Source MAC authentication*/
	{
			MT_WATCH("dot1X SMAC Authentication is need");
			mac.octet[0]=hsb->sa[0];    mac.octet[1]=hsb->sa[1];
			mac.octet[2]=hsb->sa[2];    mac.octet[3]=hsb->sa[3];
			mac.octet[4]=hsb->sa[4];    mac.octet[5]=hsb->sa[5];
			eidx = l2SwitchTable_getHashIndex((uint8*)&mac,fid);
			for (i=0; i<RTL8651_L2TBL_COLUMN; i++)
			{
			   	_rtl8651_readAsicEntry(TYPE_L2_SWITCH_TABLE, eidx + i, &entryContent);
				if (entryContent.auth==0) continue;
				entryMac.octet[0] = entryContent.mac47_40;
				entryMac.octet[1] = entryContent.mac39_24>>8;
				entryMac.octet[2] = entryContent.mac39_24&0xff;
				entryMac.octet[3] = entryContent.mac23_8>>8;
				entryMac.octet[4] = entryContent.mac23_8&0xff;	   
				entryMac.octet[5]=	 entryMac.octet[0]^entryMac.octet[1]^entryMac.octet[2]^entryMac.octet[3]^entryMac.octet[4]^(fidHash[entryContent.fid&0x3])^(eidx>>2);		
	       		if ( _cmpTableMac(&entryMac, &mac)  &&( entryContent.fid == (fid&0x3)))
		        	{		

					if (_modelUpdateTimeMember(&entryContent, eidx+i, hsb,hsa)==FAILED)
					{
							MT_WATCH("SMAC is not  match member port");
					}
					else
					{
						smac_auth=entryContent.auth;
					}
					if  (smac_auth)
					{
						MT_WATCH("SMAC is authenticated");
						goto authenicate;
					}
		        	}
			}		
			
			/* SMAC NOT authenticated*/
			
			switch(dot1x_process)
			{
				case DOT1X_PROCESS_DROP:		
					modelDrop(hsb,hsa,ale);
					MT_DROP( "Ingress Drop due to Src Mac Not Unauthized" );
					return MRET_DROP;
				case DOT1X_PROCESS_TOCPU:		
					modelCPUport(hsb,hsa,ale);
					MT_TOCPU( "Due to Src Mac Not Unauthized" );
					return MRET_TOCPU;				
				case DOT1X_PROCESS_GVLAN:
						MT_WATCH( "To Guest Vlan due to SMAC Not Unauthized " );
						ToGuestVlan=1;
					break;
				default:
					modelDrop(hsb,hsa,ale);
					MT_DROP( "Due to reserved Process (dot1x)" );
					return MRET_DROP;

			}	
	ToGuestVlanLabel:			
			hsa->dvid=ale->aleInternalSvid;
			/* Destitntoin Mac Searching*/
			mac.octet[0]=hsb->da[0];	mac.octet[1]=hsb->da[1];
			mac.octet[2]=hsb->da[2];	mac.octet[3]=hsb->da[3];
			mac.octet[4]=hsb->da[4];	mac.octet[5]=hsb->da[5];
			eidx = l2SwitchTable_getHashIndex((uint8*)&mac,fid);			
			gvid=(READ_VIR32(GVGCR)& GUEST_VLAN_BITMASK);	
			sprintf(msg,"Guest Vid %d (%x)	",gvid,gvid);
			MT_WATCH(msg);
			_rtl8651_readAsicEntry(TYPE_VLAN_TABLE, gvid,&gvlan);			
			modelAssignMember(hsb,hsa,0x3F,0x7);
			goto searchDA;

	}

authenicate:	
	hsa->dvid=ale->aleInternalSvid;
	/* Destitntoin Mac Searching*/
	mac.octet[0]=hsb->da[0];	mac.octet[1]=hsb->da[1];
	mac.octet[2]=hsb->da[2];	mac.octet[3]=hsb->da[3];
	mac.octet[4]=hsb->da[4];	mac.octet[5]=hsb->da[5];
	eidx = l2SwitchTable_getHashIndex((uint8*)&mac,fid);
	modelAssignMember(hsb,hsa,0x3F,0x7);
	
	/* Walk through 4 ways to find out */
searchDA:
	for (i=0; i<RTL8651_L2TBL_COLUMN; i++)
	{
	   	_rtl8651_readAsicEntry(TYPE_L2_SWITCH_TABLE, eidx + i, &entryContent);
		entryMac.octet[0] = entryContent.mac47_40;
		entryMac.octet[1] = entryContent.mac39_24>>8;
		entryMac.octet[2] = entryContent.mac39_24&0xff;
		entryMac.octet[3] = entryContent.mac23_8>>8;
		entryMac.octet[4] = entryContent.mac23_8&0xff;	   
		entryMac.octet[5]=	 entryMac.octet[0]^entryMac.octet[1]^entryMac.octet[2]^entryMac.octet[3]^entryMac.octet[4]^(fidHash[entryContent.fid&0x3])^(eidx>>2);		
		if ((entryContent.agingTime==0) &&(entryContent.auth==0) &&(entryContent.isStatic==0)) continue;
	       if ( _cmpTableMac(&entryMac, &mac)  &&( entryContent.fid == (fid&0x3)) )
        	{
        		if ((entryContent.agingTime) ||(entryContent.isStatic))
      			{
				modelAssignMember(hsb,hsa,entryContent.memberPort,entryContent.extMemberPort);
				dmac_auth=entryContent.auth;
				if (entryContent.toCPU)
					modelAddCPUport( hsb, hsa);
				sprintf(msg," L2 Entry destiont port list  %x ext %x ...",hsa->dp,hsa->dpext);
				MT_WATCH(msg);

				break;
      			}
			else
			{
				if (entryContent.toCPU)
					modelAddCPUport( hsb, hsa);
				dmac_auth=entryContent.auth;				
				sprintf(msg,"L2 Entry destiont port list  %x ext %x ...",hsa->dp,hsa->dpext);
				MT_WATCH(msg);
				break;
			}
        	}
	}	
	if ((i==RTL8651_L2TBL_COLUMN))
	{	
		for (j=0;j<4;j++)
		{
			regValue=READ_VIR32(MGFCR_E0R0+j*12);
			entryMac.octet[5] = regValue>>24&0xff;
			entryMac.octet[4] = regValue>>16&0xff;
			entryMac.octet[3] = regValue>>8&0xff;
			entryMac.octet[2] = regValue&0xff;
			regValue=READ_VIR32(MGFCR_E0R1+(j*12));
			entryMac.octet[1] = regValue&0xff;
			entryMac.octet[0] =( regValue>>8)&0xff;			
			start =(regValue>>16)&0x3f; /*bits 16-21 */
			if (start>48) start=48;
			len = (regValue>>22) &0x3f;		
			if (len>48)len=48;
			memset((void*)&macMask,0,sizeof(macMask));
			sprintf(msg,"start %d len %d", start,len);
			MT_WATCH(msg);
			for (;len>=0;start--,len--)
			{

				if (start>=40)
				{
					macMask.octet[0] |=1<<(start-40);
				}
				else if (start>=32)
				{
					macMask.octet[1] |=1<<(start-32);
				}
				else if (start>=24)
				{
					macMask.octet[2] |=1<<(start-24);
				}
				else if (start>=16)
				{
					macMask.octet[3] |=1<<(start-16);
				}
				else if (start>=8)
				{
					macMask.octet[4] |=1<<(start-8);
				}
				else 
				{
					macMask.octet[5] |=1<<(start);
				}
				
			}
			sprintf(msg,"Forwarding Configuration Control(%d)  Mac:%02X %02X %02X %02X %02X %02X  ",j,entryMac.octet[0],entryMac.octet[1],entryMac.octet[2],entryMac.octet[3],entryMac.octet[4],entryMac.octet[5]);
			MT_WATCH(msg);
			sprintf(msg,"Forwarding Configuration Control(%d) MacMask:%02X %02X %02X %02X %02X %02X  ",j,macMask.octet[0],macMask.octet[1],macMask.octet[2],macMask.octet[3],macMask.octet[4],macMask.octet[5]);
			MT_WATCH(msg);
			regValue=READ_VIR32(MGFCR_E0R2+(j*12))&0x1ff;
			if (regValue&& (fid ==(READ_VIR32(MGFCR_E0R2+(j*12))) >>28)&&(entryMac.octet[0] &macMask.octet[0]) == (mac.octet[0] & macMask.octet[0]) &&
				(entryMac.octet[1] &macMask.octet[1]) == (mac.octet[1] & macMask.octet[1])&&
				(entryMac.octet[2] &macMask.octet[2]) == (mac.octet[2] & macMask.octet[2])&&
				(entryMac.octet[3] &macMask.octet[3]) == (mac.octet[3] & macMask.octet[3])&&
				(entryMac.octet[4] &macMask.octet[4]) == (mac.octet[4] & macMask.octet[4])&&			
				(entryMac.octet[5] &macMask.octet[5]) == (mac.octet[5] & macMask.octet[5]))			
			{
				sprintf(msg,"Match Unknown Mac forward Group Register port lst  %x  ",regValue);
				modelAssignMember(hsb,hsa,regValue&0x3f,(regValue >>RTL8651_MAC_NUMBER));			
				MT_WATCH(msg);
				i=RTL8651_L2TBL_COLUMN-1;
				break;

			}

		}
	}
	/* Unknow unicast control*/
	if ((i==RTL8651_L2TBL_COLUMN) && ( (hsb->da[0]&MULTICAST_BROADCAST_BIT)!=MULTICAST_BROADCAST_BIT) &&
		(  (READ_VIR32(FFCR)&EN_UNUNICAST_TOCPU)==EN_UNUNICAST_TOCPU))
	{
		modelAddCPUport(hsb,hsa);
	}
	
	/* Unknow muliticast control*/
	if ( (i==RTL8651_L2TBL_COLUMN) && ( (hsb->da[0]&MULTICAST_BROADCAST_BIT)==MULTICAST_BROADCAST_BIT) &&
		(  (READ_VIR32(FFCR)&EN_UNMCAST_TOCPU)==EN_UNMCAST_TOCPU))
	{
		modelAddCPUport(hsb,hsa);
	}

	
	/* Skip Multicast/Broadcast Learning*/
	if (hsb->sa[0]&MULTICAST_BROADCAST_BIT)     return MRET_OK;


	if (srcMacControl)
	{
		return MRET_OK;
	}
	/**************************************************************************
	Source MAC Learning
	**************************************************************************/
	mac.octet[0]=hsb->sa[0];    mac.octet[1]=hsb->sa[1];
	mac.octet[2]=hsb->sa[2];    mac.octet[3]=hsb->sa[3];
	mac.octet[4]=hsb->sa[4];    mac.octet[5]=hsb->sa[5];
	sprintf(msg,"Learning SMAC:%02X %02X %02X %02X %02X %02X  ",hsb->sa[0],hsb->sa[1],hsb->sa[2],hsb->sa[3],hsb->sa[4],hsb->sa[5]);
	MT_WATCH(msg);
	eidx = l2SwitchTable_getHashIndex((uint8*)&mac,fid);

	/* Walk through 4 ways to check if exist ? */
	for (i=0; i<RTL8651_L2TBL_COLUMN; i++)
	{
	    _rtl8651_readAsicEntry(TYPE_L2_SWITCH_TABLE, eidx + i, &entryContent);
		entryMac.octet[0] = entryContent.mac47_40;
		entryMac.octet[1] = entryContent.mac39_24>>8;
		entryMac.octet[2] = entryContent.mac39_24&0xff;
		entryMac.octet[3] = entryContent.mac23_8>>8;
		entryMac.octet[4] = entryContent.mac23_8&0xff;
		entryMac.octet[5]=	entryMac.octet[0]^entryMac.octet[1]^entryMac.octet[2]^entryMac.octet[3]^entryMac.octet[4]^(eidx>>2);
		if( (entryContent.agingTime==0) &&(entryContent.auth==0) && (entryContent.isStatic==0)) continue;
		if ( _cmpTableMac(&entryMac, &mac) && (fid==entryContent.fid))
		{	/* exist ! update it */
			_modelUpdateTimeMember(&entryContent, eidx+i, hsb,hsa);				
			return MRET_OK;
		}
	}

	/* Walk through 4 ways to find available slot */
	for (i=0; i<RTL8651_L2TBL_COLUMN; i++)
	{
	    _rtl8651_readAsicEntry(TYPE_L2_SWITCH_TABLE, eidx + i, &entryContent);
	    if ( entryContent.isStatic  || entryContent.agingTime || entryContent.auth||entryContent.nxtHostFlag)
			continue;
	    break;
	}
	if ( i == RTL8651_L2TBL_COLUMN )
	{
		/* no available slot found */
		if ( (READ_VIR32(TEACR)&EN_L2LRUHASH)==EN_L2LRUHASH )
		{
			int32 smallestIdx, smallestAge;

			/* L2 LRU is enabled, find the smallest age and non-static entry to replace */
			smallestIdx = 0;
			smallestAge = 999999; /* set very high to let anyone replace it */
			for (i=0; i<RTL8651_L2TBL_COLUMN; i++)
			{
				_rtl8651_readAsicEntry(TYPE_L2_SWITCH_TABLE, eidx + i, &entryContent);
				if ( (entryContent.isStatic==FALSE) &&
				     (entryContent.agingTime<smallestAge) )
				{
					smallestIdx = i;
					smallestAge = entryContent.agingTime;
				}
			}

			/* finally, we found smallest age. Replace it. */
		    sprintf(msg, "LRU enabled, Replace eidx=%d smallestIdx=%d (smallestAge=%d)",eidx,smallestIdx,smallestAge);
		    MT_WATCH(msg);
			memset( &entryContent, 0, sizeof(entryContent) );
			entryContent.mac47_40= (mac.octet[0]);
			entryContent.mac39_24= ((mac.octet[1]) << 8) | mac.octet[2];
			entryContent.mac23_8 = (mac.octet[3]<<8)|mac.octet[4];
			entryContent.fid=vlanTable.fid;	
			_modelUpdateTimeMember(&entryContent, eidx+smallestIdx,  hsb,hsa);	
		}
	}
	else
	{	/* available slot is found. Add it. */
	    sprintf(msg, "available slot is found. ADD eidx=%d i=%d",eidx,i);
	    MT_WATCH(msg);
		memset( &entryContent, 0, sizeof(entryContent) );
		entryContent.mac47_40= (mac.octet[0]);
		entryContent.mac39_24= ((mac.octet[1]) << 8) | mac.octet[2];
		entryContent.mac23_8 = (mac.octet[3]<<8)|mac.octet[4];
		entryContent.fid=vlanTable.fid;	
		_modelUpdateTimeMember(&entryContent, eidx+i,  hsb,hsa);	
	}

	return MRET_OK;
}

