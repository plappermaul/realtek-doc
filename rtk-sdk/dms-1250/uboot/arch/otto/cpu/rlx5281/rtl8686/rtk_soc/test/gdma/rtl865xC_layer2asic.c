/*
* Copyright c                  Realtek Semiconductor Corporation, 2002  
* All rights reserved.
* 
* Program : rtl8651_layer2asic.c
* Abstract : RTL8651 Home gateway controller Layer2 asic table driver
* Author : Chun-Feng Liu(cfliu@realtek.com.tw)
* $Id: rtl865xC_layer2asic.c,v 1.163 2008-04-09 04:36:48 bo_zhao Exp $
*/

/*	@doc RTL865XC_LAYER2ASIC_API

	@module rtl865xC_layer2asic.c - RTL8651 Home gateway controller Layer2 ASIC related  API documentation	|
	This document explains the internal and external API interface of the layer 2 ASIC module. Functions with _rtl8651 prefix
	are internal functions and functions with rtl8651_ prefix are external functions.
	@normal Gateway Team (gwhp@realtek.com.tw) <date>

	Copyright <cp>2006 Realtek<tm> Semiconductor Cooperation, All Rights Reserved.

 	@head3 List of Symbols |
 	Here is a list of all functions and variables in this module.

 	@index | RTL865XC_LAYER2ASIC_API
*/

#include "rtl_types.h"
#include "assert.h"
//#include "types.h"
#include "rtl865xC_tblAsicDrv.h"
//#ifndef CONFIG_RTL865X_LIGHT_ROMEDRV
//#include "rtl8651_layer2.h"
//#ifdef CONFIG_RTL865X_MULTILAYER_BSP
//#include "rtl8651_tblDrvLocal.h"
//#endif
//#else
//#define RTL8651_TBLDRV_LOCAL_H
//#include "rtl865x_lightrome.h"
//#include "rtl8651_debug.h"
//#endif
//#if defined(CONFIG_DEFAULTS_KERNEL_2_6)
//#include "rtl8651_debug.h"
//#endif
#include "asicregs.h"
#include "gdma_glue.h"
#include "gdma_utils.h"
//#ifdef RTL865X_CPUQOS
//#include "../rtl865xc_swNic.h"
//#endif
//#include "rtl8651_asicDebug.h"

//#if defined(VSV)||defined(MIILIKE)
//#include "vsv_conn.h"
//#include "virtualMac.h" /* for model_getTestTarget() only */
//#endif

int8						*pVirtualSWReg = (int8*)REAL_SWCORE_BASE;
int8						*pVirtualSysReg = (int8*)REAL_SYSTEM_BASE;
int8						*pVirtualHsb = (int8*)REAL_HSB_BASE;
int8						*pVirtualHsa = (int8*)REAL_HSA_BASE;
int8						*pVirtualSWTable = (int8*)REAL_SWTBL_BASE;

#if 0
int8 rtl8651_layer2Asic_Id[] = "$Id: rtl865xC_layer2asic.c,v 1.163 2008-04-09 04:36:48 bo_zhao Exp $";
static uint8 fidHashTable[]={0x00,0x0f,0xf0,0xff};
/*=========================================
  * Internal Data structure
  *=========================================*/

/* For Bandwidth control - RTL865xB Backward compatible only */
#define _RTL865XB_BANDWIDTHCTRL_X1			(1 << 0)
#define _RTL865XB_BANDWIDTHCTRL_X4			(1 << 1)
#define _RTL865XB_BANDWIDTHCTRL_X8			(1 << 2)
#define _RTL865XB_BANDWIDTHCTRL_CFGTYPE		2		/* Ingress (0) and Egress (1) : 2 types of configuration */
static int32 _rtl865xB_BandwidthCtrlMultiplier = _RTL865XB_BANDWIDTHCTRL_X1;
static uint32 _rtl865xB_BandwidthCtrlPerPortConfiguration[RTL8651_PORT_NUMBER][_RTL865XB_BANDWIDTHCTRL_CFGTYPE /* Ingress (0), Egress (1) */ ];

/* Mapping table for RTL865xC Setting and RTL865xB bandwidth index */

#ifdef	CONFIG_RTL865XB
/* Ingress Setting (unit: 16Kbps defined in RTL865xC spec) */
static uint32 _rtl865xB_BandwidthCtrlIngressSettingUnit[] = {	0,	/* BW_FULL_RATE */
														8,	/* BW_128K */
														16,	/* BW_256K */
														32,	/* BW_512K */
														64,	/* BW_1M */
														128,	/* BW_2M */
														256,	/* BW_4M */
														512	/* BW_8M */
													};
/* Egress Setting (unit: 64Kbps defined in RTL865xC spec) */
static uint32 _rtl865xB_BandwidthCtrlEgressSettingUnit[] = {	0,	/* BW_FULL_RATE */
														2,	/* BW_128K */
														4,	/* BW_256K */
														8,	/* BW_512K */
														16,	/* BW_1M */
														32,	/* BW_2M */
														64,	/* BW_4M */
														128	/* BW_8M */
														};
#elif defined(CONFIG_RTL865XC)
static uint32 _rtl865xC_BandwidthCtrlNum[] = {	0,	/* BW_FULL_RATE */
														131072,	/* BW_128K */
														262144,	/* BW_256K */
														524288,	/* BW_512K */
														1048576,	/* BW_1M */
														2097152,	/* BW_2M */
														4194304,	/* BW_4M */
														8388608	/* BW_8M */
														};

#define	RTL865XC_INGRESS_16KUNIT	16384
#define	RTL865XC_EGRESS_64KUNIT	64000
#endif

#if defined(RTL865XC_MNQUEUE_OUTPUTQUEUE)  || defined(RTL865XC_QOS_OUTPUTQUEUE)

#define	QNUM_IDX_123		0
#define	QNUM_IDX_45		1
#define	QNUM_IDX_6		2

static rtl865xC_outputQueuePara_t	outputQueuePara[3] = {
										{
											1, 		/* default: Bandwidth Control Include/exclude Preamble & IFG */
											20, 		/* default: Per Queue Physical Length Gap = 20 */
											504, 		/* default: Descriptor Run Out Threshold = 504 */
											180, 		/*default: System shared buffer flow control turn off threshold = 212 */
											196,		/*default: System shared buffer flow control turn on threshold = 248 */
											500, 		/*default: system flow control turn off threshold = 500*/
											502,		/*default: system flow control turn on threshold = 502*/
											330, 		/*default: port base flow control turn off threshold = 0xf8*/
											400,		/*default: port base flow control turn on threshold = 0x108*/
											31, 		/* Queue-Descriptor=Based Flow Control turn off Threshold =0x14 */
											48, 		/* Queue-Descriptor=Based Flow Control turn on Threshold = 0x21 */
											0x03, 	/* Queue-Packet=Based Flow Control turn off Threshold = 0x03 */
											0x05	/* Queue-Packet=Based Flow Control turn on Threshold =0x05 */
										},
										{
											1, 		/* default: Bandwidth Control Include/exclude Preamble & IFG */
											20, 		/* default: Per Queue Physical Length Gap = 20 */
											504, 		/* default: Descriptor Run Out Threshold = 504 */
											120, 		/*default: System shared buffer flow control turn off threshold = 212 */
											136,		/*default: System shared buffer flow control turn on threshold = 248 */
											330, 		/*default: system flow control turn off threshold = 500*/
											344,		/*default: system flow control turn on threshold = 502*/
											248, 		/*default: port base flow control turn off threshold = 0xf8*/
											264,		/*default: port base flow control turn on threshold = 0x108*/
											20, 		/* Queue-Descriptor=Based Flow Control turn off Threshold =0x14 */
											33, 		/* Queue-Descriptor=Based Flow Control turn on Threshold = 0x21 */
											0x03, 	/* Queue-Packet=Based Flow Control turn off Threshold = 0x03 */
											0x05	/* Queue-Packet=Based Flow Control turn on Threshold =0x05 */
										},
										{
											1, 		/* default: Bandwidth Control Include/exclude Preamble & IFG */
											20, 		/* default: Per Queue Physical Length Gap = 20 */
											500, 		/* default: Descriptor Run Out Threshold = 504 */
											324, 		/*default: System shared buffer flow control turn off threshold = 212 */
											340,		/*default: System shared buffer flow control turn on threshold = 248 */
											330, 		/*default: system flow control turn off threshold = 500*/
											400,		/*default: system flow control turn on threshold = 502*/
											240, 		/*default: port base flow control turn off threshold = 0xf8*/
											282,		/*default: port base flow control turn on threshold = 0x108*/
											20, 		/* Queue-Descriptor=Based Flow Control turn off Threshold =0x14 */
											28, 		/* Queue-Descriptor=Based Flow Control turn on Threshold = 0x21 */
											10, 	/* Queue-Packet=Based Flow Control turn off Threshold = 0x03 */
											11	/* Queue-Packet=Based Flow Control turn on Threshold =0x05 */
										}
										};
#endif

static int32 _rtl865xC_lockTLUCounter	=	0;
static void _rtl8651_syncToAsicEthernetBandwidthControl(void);
static void _rtl8651_syncFromAsicEthernetBandwidthControl(void);

/*=========================================
  * Static Function Prototype
  *=========================================*/
static int32 _rtl8651_initAsicPara( rtl8651_tblAsic_InitPara_t *para );
static void _rtl8651_clearSpecifiedAsicTable(uint32 type, uint32 count);
static void _rtl8651_initialRead(void);
static uint32 _rtl8651_findAsicLinkupPortmask(uint32 portMask);
static int32 _rtl8651_rearrangeAsicEthernetMacLinkStatus( uint32 updateMacMask );
static int32 _rtl8651_macLinkStatus(uint32 macIdx);
static inline void rtl8651_procLinkUp(uint32 currentLinkUpStatus[]);
static inline void rtl8651_procLinkDown(uint32 currentLinkUpStatus[]);
#define RTL865XC_QM_DESC_READROBUSTPARAMETER	10
static int32 _rtl865xC_QM_init( void );


#if defined(CONFIG_RTL865X_LIGHT_ROMEDRV)

typedef struct rtl8651_tblAsic_ethernet_s {

#ifdef CONFIG_RTL865XC
	uint8	linkUp: 1,
			phyId: 5,
			isGPHY: 1;
#else
	uint8 linkUp:1;
#endif

} rtl8651_tblAsic_ethernet_t;


#endif
rtl8651_tblAsic_ethernet_t 	rtl8651AsicEthernetTable[9];//RTL8651_PORT_NUMBER+rtl8651_totalExtPortNum
uint32 	linkChangePendingCount;
int32 	miiPhyAddress;
int32 	wlan_acc_debug = 0;

int32		rtl8651_totalExtPortNum=0; //this replaces all RTL8651_EXTPORT_NUMBER defines
int32		rtl8651_allExtPortMask=0; //this replaces all RTL8651_EXTPORTMASK defines






/*=========================================
  * ASIC DRIVER API: SWITCH MODE
  *=========================================*/
#define RTL865X_ASIC_DRIVER_SWITCH_MODE_API

int32 rtl8651_operationlayer=0;
int32 rtl8651_setAsicOperationLayer(uint32 layer) {
	if(layer<1 || layer>4)
		return FAILED;
	/*   for bridge mode ip multicast patch
		When  in bridge mode,
		only one vlan(8) is available,
		if rtl8651_operationlayer is set less than 3,
		(pelase refer to rtl8651_setAsicVlan() )
		the "enable routing bit" of VLAN table will be set to 0 according to rtl8651_operationlayer.
		On the one hand, multicast data is flooded in vlan(8) by hardware, 
		on the other hand,it will be also trapped to cpu.
		In romedriver process,
		it will do _rtl8651_l2PhysicalPortRelay() in _rtl8651_l2switch(),
		and results in same multicast packet being flooded twice: 
		one is by hareware, the other is by romedriver.
		so the minimum  rtl8651_operationlayer will be set 3.
	*/
	if(layer<3)
	{
		layer=3;
	}
	
	if(layer == 1) {
		WRITE_MEM32(MSCR,READ_MEM32(MSCR)&~(EN_L2|EN_L3|EN_L4));
		WRITE_MEM32(MSCR,READ_MEM32(MSCR)&~(EN_IN_ACL));
		WRITE_MEM32(MSCR,READ_MEM32(MSCR)&~(EN_OUT_ACL));
	}else{
		/*
		 * Egress acl check should never enable, becuase of some hardware bug 
		 * reported by alpha    2007/12/05
		 */
		WRITE_MEM32(MSCR,READ_MEM32(MSCR)|(EN_IN_ACL));
		/*WRITE_MEM32(MSCR,READ_MEM32(MSCR)|(EN_OUT_ACL));*/

		if(layer == 2) {
			WRITE_MEM32(MSCR,READ_MEM32(MSCR)|(EN_L2));
			WRITE_MEM32(MSCR,READ_MEM32(MSCR)&~(EN_L3|EN_L4));
		}
		else
		{	// options for L3/L4 enable
			//WRITE_MEM32(MISCCR,READ_MEM32(MISCCR)|FRAG2CPU); //IP fragment packet need to send to CPU when multilayer is enabled
			/*
			  * Only for RTL8650B:
			  * It is no need to trap fragment packets to CPU in pure rouitng mode while ACL is enabled, hence we should 
			  * turn this bit (FRAG2CPU) off.
			  * NOTE: if we do this, we should also turn ENFRAG2ACLPT on.     
			  *														-chhuang, 7/30/2004
			  */
			WRITE_MEM32(MSCR,READ_MEM32(MSCR) & ~FRAG2CPU);
		
			if(layer == 3) {
				WRITE_MEM32(MSCR,READ_MEM32(MSCR)|(EN_L2|EN_L3));
				WRITE_MEM32(MSCR,READ_MEM32(MSCR)&~(EN_L4));
			}
			else {	// layer 4
				WRITE_MEM32(MSCR,READ_MEM32(MSCR)|(EN_L2|EN_L3|EN_L4));
			}
		}
	}
	if(layer == 1)
		rtl8651_setAsicAgingFunction(FALSE, FALSE);
	else if (layer == 2 || layer == 3)
		rtl8651_setAsicAgingFunction(TRUE, FALSE);
	else
		rtl8651_setAsicAgingFunction(TRUE, TRUE);
	rtl8651_operationlayer	=layer;
	return SUCCESS;
}

int32 rtl8651_getAsicOperationLayer(void) {
#ifdef RTL865X_DEBUG
	uint32 regValue, layer=0;

	regValue = READ_MEM32(MSCR);
	switch(regValue & (EN_L2|EN_L3|EN_L4)) {
		case 0:
			layer = 1;
		break;
		case EN_L2:
			layer = 2;
		break;
		case (EN_L2|EN_L3):
			layer = 3;
		break;
		case (EN_L2|EN_L3|EN_L4):
			layer = 4;
		break;
		default:
			assert(0);//ASIC should not have such value
	}
	if(layer!=rtl8651_operationlayer){
		rtlglue_printf( "READ_MEM32(MSCR)=0x%08x\n", READ_MEM32(MSCR) );
		rtlglue_printf( "layer=%d, rtl8651_operationlayer=%d\n", layer, rtl8651_operationlayer );
		RTL_BUG( "MSCR layer setting is not the same as rtl8651_operationlayer" );
		return -1;
	}
		
#endif
	return  rtl8651_operationlayer;
}


/*=========================================
  * ASIC DRIVER API: SPANNING TREE
  *=========================================*/
#define RTL865X_ASIC_DRIVER_SPANNING_TREE_API

/*
@func int32		| rtl865xC_setAsicSpanningTreePortState 	| Configure Spanning Tree Protocol Port State
@parm uint32 | port | port number under consideration
@parm uint32 | portState | Spanning tree port state: RTL8651_PORTSTA_DISABLED, RTL8651_PORTSTA_BLOCKING, RTL8651_PORTSTA_LISTENING, RTL8651_PORTSTA_LEARNING, RTL8651_PORTSTA_FORWARDING
@rvalue SUCCESS	| 	Success
@rvalue FAILED | Failed
@comm
Config IEEE 802.1D spanning tree port sate into ASIC.
 */
int32 rtl865xC_setAsicSpanningTreePortState(uint32 port, uint32 portState)
{
	uint32 offset = port * 4;
	
	if ( port >= RTL865XC_PORT_NUMBER )
		return FAILED;

	switch(portState)
	{
		case RTL8651_PORTSTA_DISABLED:
			WRITE_MEM32( PCRP0 + offset, ( READ_MEM32( PCRP0 + offset ) & (~STP_PortST_MASK) ) | STP_PortST_DISABLE );
			break;
		case RTL8651_PORTSTA_BLOCKING:
		case RTL8651_PORTSTA_LISTENING:
			WRITE_MEM32( PCRP0 + offset, ( READ_MEM32( PCRP0 + offset ) & (~STP_PortST_MASK) ) | STP_PortST_BLOCKING );
			break;
		case RTL8651_PORTSTA_LEARNING:
			WRITE_MEM32( PCRP0 + offset, ( READ_MEM32( PCRP0 + offset ) & (~STP_PortST_MASK) ) | STP_PortST_LEARNING );
			break;
		case RTL8651_PORTSTA_FORWARDING:
			WRITE_MEM32( PCRP0 + offset, ( READ_MEM32( PCRP0 + offset ) & (~STP_PortST_MASK) ) | STP_PortST_FORWARDING );
			break;
		default:
			return FAILED;
	}

	return SUCCESS;
}

/*
@func int32		| rtl865xC_getAsicSpanningTreePortState 	| Retrieve Spanning Tree Protocol Port State
@parm uint32 | port | port number under consideration
@parm uint32 | portState | pointer to memory to store the port state
@rvalue SUCCESS	| 	Success
@rvalue FAILED | Failed
@comm
Possible spanning tree port state: RTL8651_PORTSTA_DISABLED, RTL8651_PORTSTA_BLOCKING, RTL8651_PORTSTA_LISTENING, RTL8651_PORTSTA_LEARNING, RTL8651_PORTSTA_FORWARDING
 */
int32 rtl865xC_getAsicSpanningTreePortState(uint32 port, uint32 *portState)
{
	uint32 reg;
	uint32 offset = port * 4;
	
	if ( port >= RTL865XC_PORT_NUMBER || portState == NULL )
		return FAILED;

	reg = ( READ_MEM32( PCRP0 + offset ) & (~STP_PortST_MASK) );

	switch(reg)
	{
		case STP_PortST_DISABLE:
			*portState = RTL8651_PORTSTA_DISABLED;
			break;
		case STP_PortST_BLOCKING:
			*portState = RTL8651_PORTSTA_BLOCKING;
			break;
		case STP_PortST_LEARNING:
			*portState = RTL8651_PORTSTA_LEARNING;
			break;
		case STP_PortST_FORWARDING:
			*portState = RTL8651_PORTSTA_FORWARDING;
			break;
		default:
			return FAILED;
	}
	return SUCCESS;

}

/*
@func int32		| rtl8651_setAsicMulticastSpanningTreePortState 	| Configure Multicast Spanning Tree Protocol Port State
@parm uint32 | port | port number under consideration
@parm uint32 | portState | Spanning tree port state: RTL8651_PORTSTA_DISABLED, RTL8651_PORTSTA_BLOCKING, RTL8651_PORTSTA_LISTENING, RTL8651_PORTSTA_LEARNING, RTL8651_PORTSTA_FORWARDING
@rvalue SUCCESS	| 	Success
@rvalue FAILED | Failed
@comm
In RTL865xC platform, Multicast spanning tree configuration is set by this API.
@xref  <p rtl865xC_setAsicSpanningTreePortState()>
 */
int32 rtl8651_setAsicMulticastSpanningTreePortState(uint32 port, uint32 portState)
{
	uint32 offset = port * 4;
	
	if ( port >= RTL865XC_PORT_NUMBER )
	{
		return FAILED;
	}

	switch(portState)
	{
		case RTL8651_PORTSTA_DISABLED:
			WRITE_MEM32( PCRP0 + offset, ( READ_MEM32( PCRP0 + offset ) & (~IPMSTP_PortST_MASK) ) | IPMSTP_PortST_DISABLE );
			break;
		case RTL8651_PORTSTA_BLOCKING:
		case RTL8651_PORTSTA_LISTENING:
			WRITE_MEM32( PCRP0 + offset, ( READ_MEM32( PCRP0 + offset ) & (~IPMSTP_PortST_MASK) ) | IPMSTP_PortST_BLOCKING );
			break;
		case RTL8651_PORTSTA_LEARNING:
			WRITE_MEM32( PCRP0 + offset, ( READ_MEM32( PCRP0 + offset ) & (~IPMSTP_PortST_MASK) ) | IPMSTP_PortST_LEARNING );
			break;
		case RTL8651_PORTSTA_FORWARDING:
			WRITE_MEM32( PCRP0 + offset, ( READ_MEM32( PCRP0 + offset ) & (~IPMSTP_PortST_MASK) ) | IPMSTP_PortST_FORWARDING );
			break;
		default:
			return FAILED;
	}

	return SUCCESS;
}

/*
@func int32		| rtl8651_getAsicMulticastSpanningTreePortState 	| Retrieve Spanning Tree Protocol Port State
@parm uint32 | port | port number under consideration
@parm uint32 | portState | pointer to memory to store the port state
@rvalue SUCCESS	| 	Success
@rvalue FAILED | Failed
@comm
In RTL865xC platform, Multicast spanning tree configuration is gotten by this API.
@xref  <p rtl865xC_getAsicSpanningTreePortState()>
 */
int32 rtl8651_getAsicMulticastSpanningTreePortState(uint32 port, uint32 *portState)
{
	uint32 reg;
	uint32 offset = port * 4;
	
	if ( port >= RTL865XC_PORT_NUMBER || portState == NULL )
		return FAILED;

	reg = ( READ_MEM32( PCRP0 + offset ) & (~IPMSTP_PortST_MASK) );

	switch(reg)
	{
		case IPMSTP_PortST_DISABLE:
			*portState = RTL8651_PORTSTA_DISABLED;
			break;
		case IPMSTP_PortST_BLOCKING:
			*portState = RTL8651_PORTSTA_BLOCKING;
			break;
		case IPMSTP_PortST_LEARNING:
			*portState = RTL8651_PORTSTA_LEARNING;
			break;
		case IPMSTP_PortST_FORWARDING:
			*portState = RTL8651_PORTSTA_FORWARDING;
			break;
		default:
			return FAILED;
	}
	return SUCCESS;
}

/*
@func int32		| rtl8651_setAsicSpanningEnable 	| Enable/disable ASIC spanning tree support
@parm int8		| spanningTreeEnabled | TRUE to indicate spanning tree is enabled; FALSE to indicate spanning tree is disabled.
@rvalue SUCCESS	| 	Success
@comm
Global switch to enable or disable ASIC spanning tree support.
If ASIC spanning tree support is enabled, further configuration would be refered by ASIC to prcoess packet forwarding / MAC learning.
If ASIC spanning tree support is disabled, all MAC learning and packet forwarding would be done regardless of port state.
Note that the configuration does not take effect for spanning tree BPDU CPU trapping. It is set in <p rtl8651_setAsicResvMcastAddrToCPU()>.
@xref <p rtl8651_setAsicMulticastSpanningTreePortState()>, <p rtl865xC_setAsicSpanningTreePortState()>, <p rtl8651_getAsicMulticastSpanningTreePortState()>, <p rtl865xC_getAsicSpanningTreePortState()>
 */
int32 rtl8651_setAsicSpanningEnable(int8 spanningTreeEnabled)
{
	if(spanningTreeEnabled == TRUE)
	{
		WRITE_MEM32(MSCR,READ_MEM32(MSCR)|(EN_STP));
		WRITE_MEM32(RMACR ,READ_MEM32(RMACR)|MADDR00);		

	}else
	{
		WRITE_MEM32(MSCR,READ_MEM32(MSCR)&~(EN_STP));
		WRITE_MEM32(RMACR, READ_MEM32(RMACR)&~MADDR00);

	}
	return SUCCESS;
}

/*
@func int32		| rtl8651_getAsicSpanningEnable 	| Getting the ASIC spanning tree support status
@parm int8*		| spanningTreeEnabled | The pointer to get the status of ASIC spanning tree configuration status.
@rvalue FAILED	| 	Failed
@rvalue SUCCESS	| 	Success
@comm
Get the ASIC global switch to enable or disable ASIC spanning tree support.
The switch can be set by calling <p rtl8651_setAsicSpanningEnable()>
@xref <p rtl8651_setAsicSpanningEnable()>, <p rtl8651_setAsicMulticastSpanningTreePortState()>, <p rtl865xC_setAsicSpanningTreePortState()>, <p rtl8651_getAsicMulticastSpanningTreePortState()>, <p rtl865xC_getAsicSpanningTreePortState()>
 */
int32 rtl8651_getAsicSpanningEnable(int8 *spanningTreeEnabled)
{
	if(spanningTreeEnabled == NULL)
		return FAILED;
	*spanningTreeEnabled = (READ_MEM32(MSCR)&(EN_STP)) == (EN_STP)? TRUE: FALSE;
	return SUCCESS;
}


/*=========================================
  * ASIC DRIVER API: BROADCAST REG
  *=========================================*/
#define RTL865X_ASIC_DRIVER_BROADCAST_REG_API

int32 rtl865xC_setBrdcstStormCtrlRate(uint32 percentage)
{
	uint32 rate = RTL865XC_MAXALLOWED_BYTECOUNT * percentage / 100;

	WRITE_MEM32( BSCR, rate );
	return SUCCESS;
}

int32 rtl8651_setBroadCastStormReg(int8 enable)
{
	uint32 port, pcr;

	for ( port = 0; port < RTL8651_PORT_NUMBER + rtl8651_totalExtPortNum; port++ )
	{
		pcr = PCRP0 + port * 4;
		
		WRITE_MEM32( pcr, READ_MEM32( pcr ) | ENBCSC | BCSC_ENBROADCAST );
	}
	
	return SUCCESS;
}

int32 rtl8651_getBroadCastSTormReg(int8 *enable)		/* How to define this function @@? */
{
	rtlglue_printf("Not Support Yet\n");
	return SUCCESS;
}

#ifdef RTL865X_PER_PORT_STORM_CONTROL
/*
@func int32	| rtl8651_perPortStormControl	| API for per port enable/disable storm control.
@parm uint32	| type	|Specifies control type: MULTICAST_STORM_CONTROL or BROADCAST_STORM_CONTROL
@parm uint32	| portNum	|Indicates which port to perform storm control
@parm uint32	| enable	| To enable or disable storm control. TRUE to enable, FALSE to disable.
@rvalue SUCCESS	| 	Config storm control successfully.
@rvalue FAILED	| 	Config storm control failed.
@comm 
	Note:Don't forget to call rtl865xC_setBrdcstStormCtrlRate() to set the global control rate, because the default value is 0.
*/
int32 rtl8651_perPortStormControl(uint32 type, uint32 portNum, uint32 enable)
{
	uint32 regAddress;
	uint32 oldRegValue;
	uint32 newRegValue;
	
	if(portNum>=RTL8651_PORT_NUMBER + rtl8651_totalExtPortNum)
	{
		rtlglue_printf("wrong port number\n");
		return FAILED;
	}
	
	regAddress=PCRP0 + portNum * 4;
		
	oldRegValue=READ_MEM32(regAddress);
	
	newRegValue=oldRegValue;
	
	if((type & BROADCAST_STORM_CONTROL) !=0)
	{
		if(enable == TRUE)
		{
			newRegValue = newRegValue |ENBCSC |BCSC_ENBROADCAST;
		}

		if(enable==FALSE)
		{
			newRegValue = newRegValue & (~BCSC_ENBROADCAST);
		}
		
	}
	
	if((type & MULTICAST_STORM_CONTROL) !=0)
	{
		if(enable == TRUE)
		{
			newRegValue = newRegValue | ENBCSC |BCSC_ENMULTICAST;
		}

		if(enable==FALSE)
		{
			newRegValue = newRegValue & (~BCSC_ENMULTICAST);
		}
		
	}

	if((newRegValue & (BCSC_ENMULTICAST |BCSC_ENBROADCAST ))==0)
	{
		/*no needn't storm control*/
		newRegValue = newRegValue & (~ENBCSC);
	}
		
	if(newRegValue!=oldRegValue)
	{
		WRITE_MEM32(regAddress, newRegValue);
	}
	
	return SUCCESS;

	
}
#endif


/*=========================================
  * ASIC DRIVER API: PATTERN MATCH
  *=========================================*/
#define RTL865X_ASIC_DRIVER_PATTERN_MATCH_API

int32 rtl8651_setAsicPortPatternMatch(uint32 port, uint32 pattern, uint32 patternMask, int32 operation) {
	//not for ext port
	if(port>=RTL8651_PORT_NUMBER)
		return FAILED;

	if(pattern==0&&patternMask==0){
		if((READ_MEM32(PPMAR)&0x2000)==0) //system pattern match not enabled.
			return SUCCESS;
		WRITE_MEM32(PPMAR,READ_MEM32(PPMAR)&~(1<<(port+26)));
		if((READ_MEM32(PPMAR)&0xfc000000)==0)
			WRITE_MEM32(PPMAR,READ_MEM32(PPMAR)&~(1<<13)); //turn off system pattern match switch.

		return SUCCESS;
	}
	if(operation>3)
		return FAILED; //valid operations: 0(drop), 1(mirror to cpu),2(fwd to cpu), 3(to mirror port) 
	WRITE_MEM32(PPMAR,READ_MEM32(PPMAR)|((1<<(port+26))|(1<<13))); //turn on system pattern match and turn on pattern match on indicated port.
	WRITE_MEM32(PPMAR,(READ_MEM32(PPMAR) & (~(0x3<<(14+2*port))))|(operation<<(14+2*port)));   //specify operation
	WRITE_MEM32(PATP0+4*port,pattern);
	WRITE_MEM32(MASKP0+4*port,patternMask);
	return SUCCESS;
}

int32 rtl8651_getAsicPortPatternMatch(uint32 port, uint32 *pattern, uint32 *patternMask, int32 *operation) {
	//not for ext port
	if(port>=RTL8651_PORT_NUMBER)
		return FAILED;
	if(((READ_MEM32(PPMAR)& (1<<13))==0)||((READ_MEM32(PPMAR)& (1<<(26+port)))==0))
		return FAILED;
	if(pattern)
		*pattern=READ_MEM32(PATP0+4*port);
	if(patternMask)
		*patternMask=READ_MEM32(MASKP0+4*port);
	if(operation)
		*operation=(READ_MEM32(PPMAR)>>(14+2*port))&0x3;
	return SUCCESS;		
}




/*=========================================
  * ASIC DRIVER API: PORT MIRROR
  *=========================================*/
#define RTL865X_ASIC_DRIVER_PORT_MIRROR_TBL_API

int32 rtl8651_setAsicPortMirror(uint32 mTxMask, uint32 mRxMask, uint32 mPortMask)
{
	uint32 i, nPort;

	mTxMask &= RTL8651_ALLPORTMASK;
	mRxMask &= RTL8651_ALLPORTMASK;
	
	/* For Tx Mirror, only one Mirror Tx port is allowed */
	for( i = 0, nPort = 0; i < RTL8651_PORT_NUMBER + rtl8651_totalExtPortNum; i++ )
	{
		if ( mTxMask & ( 1 << i ) )
			nPort++;
	}

	if ( nPort > 1 )
		return FAILED;
	
	WRITE_MEM32( PMCR, ( mTxMask << MirrorTxPrtMsk_OFFSET ) | ( mRxMask << MirrorRxPrtMsk_OFFSET ) | ( mPortMask << MirrorPortMsk_OFFSET ) );

	return SUCCESS;
}

int32 rtl8651_getAsicPortMirror(uint32 *mRxMask, uint32 *mTxMask, uint32 *mPortMask)
{
	uint32 pmcr = READ_MEM32( PMCR );

	if ( mPortMask )
	{
		*mPortMask = ( pmcr & MirrorPortMsk_MASK ) >> MirrorPortMsk_OFFSET;
	}

	if ( mRxMask )
	{
		*mRxMask = ( pmcr & MirrorRxPrtMsk_MASK ) >> MirrorRxPrtMsk_OFFSET;
	}

	if ( mTxMask )
	{
		*mTxMask = ( pmcr & MirrorTxPrtMsk_MASK ) >> MirrorTxPrtMsk_OFFSET;
	}
	
	return SUCCESS;
}




/*=========================================
  * ASIC DRIVER API: ETHERNET MII
  *=========================================*/
#define RTL865X_ASIC_DRIVER_ETHERNET_MII_API

int32 rtl865xC_setAsicEthernetMIIMode(uint32 port, uint32 mode)
{
	if ( port != 0 && port != RTL8651_MII_PORTNUMBER )
		return FAILED;
	if ( mode != LINK_RGMII && mode != LINK_MII_MAC && mode != LINK_MII_PHY )
		return FAILED;

	if ( port == 0 )
	{
		/* MII port MAC interface mode configuration */
		WRITE_MEM32( P0GMIICR, ( READ_MEM32( P0GMIICR ) & ~CFG_GMAC_MASK ) | ( mode << LINKMODE_OFFSET ) );
	}
	else
	{
		/* MII port MAC interface mode configuration */
		WRITE_MEM32( P5GMIICR, ( READ_MEM32( P5GMIICR ) & ~CFG_GMAC_MASK ) | ( mode << LINKMODE_OFFSET ) );
	}
	return SUCCESS;

}

int32 rtl865xC_setAsicEthernetRGMIITiming(uint32 port, uint32 Tcomp, uint32 Rcomp)
{
	if ( port != 0 && port != RTL8651_MII_PORTNUMBER )
		return FAILED;
	if ( Tcomp < RGMII_TCOMP_0NS || Tcomp > RGMII_TCOMP_7NS || Rcomp < RGMII_RCOMP_0NS || Rcomp > RGMII_RCOMP_2DOT5NS )
		return FAILED;
	
	if ( port == 0 )
	{
		WRITE_MEM32(P0GMIICR, ( ( ( READ_MEM32(P0GMIICR) & ~RGMII_TCOMP_MASK ) | Tcomp ) & ~RGMII_RCOMP_MASK ) | Rcomp );
	}
	else
	{
		WRITE_MEM32(P5GMIICR, ( ( ( READ_MEM32(P5GMIICR) & ~RGMII_TCOMP_MASK ) | Tcomp ) & ~RGMII_RCOMP_MASK ) | Rcomp );
	}

	return SUCCESS;
}

/* For backward-compatible issue, this API is used to set MII port 5. */
int32 rtl8651_setAsicEthernetMII(uint32 phyAddress, int32 mode, int32 enabled)
{
	/* Input validation */
	if ( phyAddress < 0 || phyAddress > 31 )
		return FAILED;
	if ( mode != P5_LINK_RGMII && mode != P5_LINK_MII_MAC && mode != P5_LINK_MII_PHY )
		return FAILED;
	
	/* Configure driver level information about mii port 5 */
	if ( enabled )
	{
		if ( miiPhyAddress >= 0 && miiPhyAddress != phyAddress )
			return FAILED;

		miiPhyAddress = phyAddress;
	}
	else
	{
		miiPhyAddress = -1;
	}

	/* MII port MAC interface mode configuration */
	WRITE_MEM32( P5GMIICR, ( READ_MEM32( P5GMIICR ) & ~CFG_GMAC_MASK ) | ( mode << P5_LINK_OFFSET ) );

	return SUCCESS;
}

int32 rtl8651_getAsicEthernetMII(uint32 *phyAddress){
	*phyAddress=miiPhyAddress;
	return SUCCESS;
}

/*=========================================
  * ASIC DRIVER API: MDC/MDIO Control
  *=========================================*/
#define RTL865X_ASIC_DRIVER_MDCMDIO_CONTROL_API

int32 rtl8651_getAsicEthernetPHYReg(uint32 phyId, uint32 regId, uint32 *rData)
{
	uint32 status;
	
	WRITE_MEM32( MDCIOCR, COMMAND_READ | ( phyId << PHYADD_OFFSET ) | ( regId << REGADD_OFFSET ) );

#ifdef RTL865X_TEST
	status = pVirtualMIIRegs[phyId][regId];
#else
	do { status = READ_MEM32( MDCIOSR ); } while ( ( status & STATUS ) != 0 );
#endif

	status &= 0xffff;
	*rData = status;

	return SUCCESS;
}

int32 rtl8651_setAsicEthernetPHYReg(uint32 phyId, uint32 regId, uint32 wData)
{
	WRITE_MEM32( MDCIOCR, COMMAND_WRITE | ( phyId << PHYADD_OFFSET ) | ( regId << REGADD_OFFSET ) | wData );

#ifdef RTL865X_TEST
	if ( regId >=_RTL865X_MII_REGISTER_CNT )
	{
		return FAILED;
	}
	pVirtualMIIRegs[phyId][regId] = wData;
#else
	while( ( READ_MEM32( MDCIOSR ) & STATUS ) != 0 );		/* wait until command complete */
#endif

	return SUCCESS;
}

/*=========================================
  * ASIC DRIVER API: PHY LOOPBACK
  *=========================================*/
#define RTL865X_ASIC_DRIVER_PHY_LOOPBACK_TBL_API

int32 rtl8651_setAsicEthernetPHYLoopback(uint32 port, int32 enabled)
{
	uint32 phyid, statCtrlReg0;

	/* port number validation */
	if (rtl8651_tblAsicDrvPara.externalPHYProperty & RTL8651_TBLASIC_EXTPHYPROPERTY_PORT5_RTL8211B)
	{
		if ( port > RTL8651_MAC_NUMBER )
		{
			return FAILED;
		}
	} else
	{
		if ( port > RTL8651_PHY_NUMBER )
		{
			return FAILED;
		}
	}

	/* PHY id determination */
	phyid = rtl8651AsicEthernetTable[port].phyId;

	rtl8651_getAsicEthernetPHYReg( phyid, 0, &statCtrlReg0 );

	if ( enabled && ( statCtrlReg0 & ENABLE_LOOPBACK ) == 0 )
	{
		statCtrlReg0 |= ENABLE_LOOPBACK;
	}
	else if ( enabled == 0 && ( statCtrlReg0 & ENABLE_LOOPBACK ) )
	{
		statCtrlReg0 &= ~ENABLE_LOOPBACK;
	}
	else
		return SUCCESS;		/* If the configuration does not change. Do nothing. */
	
	rtl8651_setAsicEthernetPHYReg( phyid, 0, statCtrlReg0 );

	return SUCCESS;

}

int32 rtl8651_getAsicEthernetPHYLoopback(uint32 port, int32 *flag)
{
	uint32 phyid, statCtrlReg0;

	if ( flag == NULL )
		return FAILED;
	
	/* port number validation */
	if (rtl8651_tblAsicDrvPara.externalPHYProperty & RTL8651_TBLASIC_EXTPHYPROPERTY_PORT5_RTL8211B)
	{
		if ( port > RTL8651_MAC_NUMBER )
		{
			return FAILED;
		}
	} else
	{
		if ( port > RTL8651_PHY_NUMBER )
		{
			return FAILED;
		}
	}

	/* PHY id determination */
	phyid = rtl8651AsicEthernetTable[port].phyId;

	rtl8651_getAsicEthernetPHYReg( phyid, 0, &statCtrlReg0 );

	*flag = ( statCtrlReg0 & ENABLE_LOOPBACK )? TRUE: FALSE;

	return SUCCESS;
}




/*=========================================
  * ASIC DRIVER API: ETHERNET PHY
  *=========================================*/
#define RTL865X_ASIC_DRIVER_ETHERNET_PHY_API

int32 rtl8651_restartAsicEthernetPHYNway(uint32 port)
{
	uint32 statCtrlReg0, phyid;

	/* port number validation */
	if (rtl8651_tblAsicDrvPara.externalPHYProperty & RTL8651_TBLASIC_EXTPHYPROPERTY_PORT5_RTL8211B)
	{
		if ( port > RTL8651_MAC_NUMBER )
		{
			return FAILED;
		}
	} else
	{
		if ( port > RTL8651_PHY_NUMBER )
		{
			return FAILED;
		}
	}

	/* PHY id determination */
	phyid = rtl8651AsicEthernetTable[port].phyId;

	/* read current PHY reg 0 */
	rtl8651_getAsicEthernetPHYReg( phyid, 0, &statCtrlReg0 );

	/* enable 'restart Nway' bit */
	statCtrlReg0 |= RESTART_AUTONEGO;

	/* write PHY reg 0 */
	rtl8651_setAsicEthernetPHYReg( phyid, 0, statCtrlReg0 );

	return SUCCESS;
}

int32 rtl8651_setAsicEthernetPHYPowerDown( uint32 port, uint32 pwrDown )
{
	uint32 statCtrlReg0, phyid;

	/* port number validation */
	if (rtl8651_tblAsicDrvPara.externalPHYProperty & RTL8651_TBLASIC_EXTPHYPROPERTY_PORT5_RTL8211B)
	{
		if ( port > RTL8651_MAC_NUMBER )
		{
			return FAILED;
		}
	} else
	{
		if ( port > RTL8651_PHY_NUMBER )
		{
			return FAILED;
		}
	}

	/* PHY id determination */
	phyid = rtl8651AsicEthernetTable[port].phyId;

	/* read current PHY reg 0 value */
	rtl8651_getAsicEthernetPHYReg( phyid, 0, &statCtrlReg0 );

	if ( pwrDown )
		statCtrlReg0 |= POWER_DOWN;
	else
		statCtrlReg0 &= ~POWER_DOWN;

	/* write PHY reg 0 */
	rtl8651_setAsicEthernetPHYReg( phyid, 0, statCtrlReg0 );

	return SUCCESS;

}

#if 0
/* 
	shliu: After verification, modifying embedded PHY IDs to non-default-value can't link up.
	That is, we should remain the default value:

	port 0 - phy id 0
	port 1 - phy id 1
	port 2 - phy id 2
	port 3 - phy id 3
	port 4 - phy id 4
*/
int32 rtl865xC_setAsicEmbeddedPhyId(uint32 port, uint32 phyId)
{
	/* Input validation */
	if ( port >= RTL8651_PHY_NUMBER || phyId < 0 || phyId > 31 )
		return FAILED;

	WRITE_MEM32( EPIDR,  ( READ_MEM32( EPIDR ) & ( ~ Port_embPhyID_MASK(port) ) ) | Port_embPhyID(phyId, port) );
	return SUCCESS;
}
#endif

int32 rtl8651_setAsicEthernetPHY(uint32 port, int8 autoNegotiation, uint32 advCapability, uint32 speed, int8 fullDuplex) 
{
	uint32 phyId, isGPHY, statCtrlReg0, statCtrlReg4, statCtrlReg9;

	/* Not for ext port */
	if (rtl8651_tblAsicDrvPara.externalPHYProperty & RTL8651_TBLASIC_EXTPHYPROPERTY_PORT5_RTL8211B)
	{
		if ( port > RTL8651_MAC_NUMBER )
		{
			return FAILED;
		}
	} else
	{
		if ( port > RTL8651_PHY_NUMBER )
		{
			return FAILED;
		}
	}
	
	/* Lookup phy id of the specified port. */
	phyId = rtl8651AsicEthernetTable[port].phyId;
	isGPHY = rtl8651AsicEthernetTable[port].isGPHY;

	/* ====================
		Arrange PHY reg 0
	   ==================== */

	/* Read PHY reg 0 (control register) first */
	rtl8651_getAsicEthernetPHYReg( phyId, 0, &statCtrlReg0 );

	if ( autoNegotiation == TRUE )	
	{
		statCtrlReg0 |= ENABLE_AUTONEGO;
	}
	else
	{
		statCtrlReg0 &= ~ENABLE_AUTONEGO;

		/* Clear speed & duplex setting */
		if ( isGPHY )
			statCtrlReg0 &= ~SPEED_SELECT_1000M;
		statCtrlReg0 &= ~SPEED_SELECT_100M;
		statCtrlReg0 &= ~SELECT_FULL_DUPLEX;

		if ( speed == 1 )	/* 100Mbps, assume 10Mbps by default */
			statCtrlReg0 |= SPEED_SELECT_100M;

		if ( fullDuplex == TRUE )
			statCtrlReg0 |= SELECT_FULL_DUPLEX;
	}

	/* =============================================================
		Arrange PHY reg 4, if GPHY, also need to arrange PHY reg 9.
	   ============================================================= */
	rtl8651_getAsicEthernetPHYReg( phyId, 4, &statCtrlReg4 );

	/* Clear all capability */
	statCtrlReg4 &= ~CAP_100BASE_MASK;

	if ( isGPHY )
	{
		rtl8651_getAsicEthernetPHYReg( phyId, 9, &statCtrlReg9 );

		/* Clear all 1000BASE capability */
		statCtrlReg9 &= ~ADVCAP_1000BASE_MASK;
	}
	else
	{
		statCtrlReg9 = 0;
	}
	
	if ( advCapability == RTL8651_ETHER_AUTO_1000FULL )
	{
		statCtrlReg9 = statCtrlReg9 | CAPABLE_1000BASE_TX_FD | CAPABLE_1000BASE_TX_HD;
		statCtrlReg4 = statCtrlReg4 | CAPABLE_100BASE_TX_FD | CAPABLE_100BASE_TX_HD | CAPABLE_10BASE_TX_FD | CAPABLE_10BASE_TX_HD;
	}
	else if ( advCapability == RTL8651_ETHER_AUTO_1000HALF )
	{
		statCtrlReg9 = statCtrlReg9 | CAPABLE_1000BASE_TX_HD;
		statCtrlReg4 = statCtrlReg4 | CAPABLE_100BASE_TX_FD | CAPABLE_100BASE_TX_HD | CAPABLE_10BASE_TX_FD | CAPABLE_10BASE_TX_HD;
	}
	else if ( advCapability == RTL8651_ETHER_AUTO_100FULL )
	{
		statCtrlReg4 = statCtrlReg4 | CAPABLE_100BASE_TX_FD | CAPABLE_100BASE_TX_HD | CAPABLE_10BASE_TX_FD | CAPABLE_10BASE_TX_HD;
	}
	else if ( advCapability == RTL8651_ETHER_AUTO_100HALF )
	{
		statCtrlReg4 = statCtrlReg4 | CAPABLE_100BASE_TX_HD | CAPABLE_10BASE_TX_FD | CAPABLE_10BASE_TX_HD;
	}
	else if ( advCapability == RTL8651_ETHER_AUTO_10FULL )
	{
		statCtrlReg4 = statCtrlReg4 | CAPABLE_10BASE_TX_FD | CAPABLE_10BASE_TX_HD;
	}
	else if ( advCapability == RTL8651_ETHER_AUTO_10HALF )
	{
		statCtrlReg4 = statCtrlReg4 | CAPABLE_10BASE_TX_HD;
	}
	else
	{
		RTL_WARN(RTL_MSG_GENERIC, "Invalid advertisement capability!");
		return FAILED;
	}

	/* ===============================
		Set PHY reg 4.
		Set PHY reg 9 if necessary.
	   =============================== */
	rtl8651_setAsicEthernetPHYReg( phyId, 4, statCtrlReg4 );

	if ( isGPHY )
	{
		rtl8651_setAsicEthernetPHYReg( phyId, 9, statCtrlReg9 );
	}

	/* =================
		Set PHY reg 0.
	   ================= */
	rtl8651_setAsicEthernetPHYReg( phyId, 0, statCtrlReg0 );

	/* =======================================================
		Restart Nway.
		If 'Nway enable' is FALSE, ASIC won't execute Nway.
	   ======================================================= */
	rtl8651_restartAsicEthernetPHYNway(port);

	return SUCCESS;
}

int32 rtl8651_getAsicEthernetPHY(uint32 port, int8 *autoNegotiation, uint32 *advCapability, uint32 *speed, int8 *fullDuplex) 
{
	uint32 isGPHY, phyId, statCtrlReg0, statCtrlReg1, statCtrlReg4, statCtrlReg5, statCtrlReg9, statCtrlReg10;

	/* Input validation */
	if ( autoNegotiation == NULL || advCapability == NULL || speed == NULL || fullDuplex == NULL )
	{
		return FAILED;
	}

	/* Initiate parameters */
	{
		*autoNegotiation = 0;
		*advCapability = 0;
		*speed = 0;
		*fullDuplex = 0;
	}

	if (rtl8651_tblAsicDrvPara.externalPHYProperty & RTL8651_TBLASIC_EXTPHYPROPERTY_PORT5_RTL8211B)
	{
		if ( port > RTL8651_MAC_NUMBER )
		{
			return FAILED;
		}
	} else
	{
		if ( port > RTL8651_PHY_NUMBER )
		{
			return FAILED;
		}
	}

	/* Lookup phy id of the specified port */
	phyId = rtl8651AsicEthernetTable[port].phyId;
	isGPHY = rtl8651AsicEthernetTable[port].isGPHY;

	/* =====================================================
		Get autoNegotiation capability first from phy reg 0 
	   ===================================================== */
	rtl8651_getAsicEthernetPHYReg( phyId, 0, &statCtrlReg0 );

	*autoNegotiation = ( statCtrlReg0 & ENABLE_AUTONEGO )? TRUE: FALSE;

	/* ========================================================
		If autoNegotiation is TRUE, check it completed or not. 
	   ======================================================== */
	if ( (rtl8651AsicEthernetTable[port].linkUp == TRUE) && *autoNegotiation == TRUE )
	{
		/* Make sure that Auto-Negotiation Completes */
		rtl8651_getAsicEthernetPHYReg( phyId, 1, &statCtrlReg1 );

		if ( !( statCtrlReg1 & STS_AUTONEGO_COMPLETE ) )
			RTL_NOTICE(RTL_MSG_GENERIC, "Auto negotiation on port %u does not complete.", port);
	}

	/* =====================================
	    Read PHY Reg 4 to get advCapability
	   ===================================== */
	rtl8651_getAsicEthernetPHYReg( phyId, 4, &statCtrlReg4 );

	/* ===================================================================================
		If it is Giga PHY, we need to check other more phy regs to know the advCapability
	   =================================================================================== */
	if ( isGPHY )
	{
		rtl8651_getAsicEthernetPHYReg( phyId, 9, &statCtrlReg9 );
	}
	else
	{
		statCtrlReg9 = 0;
	}

	if ( statCtrlReg9 & CAPABLE_1000BASE_TX_FD )
	{
		*advCapability = RTL8651_ETHER_AUTO_1000FULL;
	}
	else if ( statCtrlReg9 & CAPABLE_1000BASE_TX_HD )
	{
		*advCapability = RTL8651_ETHER_AUTO_1000HALF;
	}
	else if ( statCtrlReg4 & CAPABLE_100BASE_TX_FD )
	{
		*advCapability = RTL8651_ETHER_AUTO_100FULL;
	}
	else if ( statCtrlReg4 & CAPABLE_100BASE_TX_HD )
	{
		*advCapability = RTL8651_ETHER_AUTO_100HALF;
	}
	else if ( statCtrlReg4 & CAPABLE_10BASE_TX_FD )
	{
		*advCapability = RTL8651_ETHER_AUTO_10FULL;
	}
	else if ( statCtrlReg4 & CAPABLE_10BASE_TX_HD )
	{
		*advCapability = RTL8651_ETHER_AUTO_10HALF;
	}
	else
	{
		RTL_WARN(RTL_MSG_GENERIC, "PHY does not support any Giga/FE technology?!");
		return FAILED;
	}

	/* =============================
	    Get the port speed & duplex
	   ============================= */
	if ( rtl8651AsicEthernetTable[port].linkUp == FALSE )
	{
		*speed = 0;
		*fullDuplex = 0;
	}
	else if ( isGPHY && *autoNegotiation == TRUE )
	{
		uint32 HCD;	/* Highest Common Denominator */
		
		/* Read link partner's capability. (Reg 10 & Reg 5) */
		rtl8651_getAsicEthernetPHYReg( phyId, 10, &statCtrlReg10 );

		HCD = ( ( statCtrlReg9 & ADVCAP_1000BASE_MASK ) >> ADVCAP_1000BASE_OFFSET ) &
				( ( statCtrlReg10 & LPCAP_1000BASE_MASK ) >> LPCAP_1000BASE_OFFSET );

		if ( HCD >= 0x2 )
		{
			*speed = 2;
			*fullDuplex = 1;
		}
		else if ( HCD >= 0x1 )
		{
			*speed = 2;
			*fullDuplex = 0;
		}
		else
		{
			/* Need to read PHY Reg 5 */
			rtl8651_getAsicEthernetPHYReg( phyId, 5, &statCtrlReg5 );

			HCD = ( ( statCtrlReg4 & CAP_100BASE_MASK ) >> CAP_100BASE_OFFSET ) &
					( ( statCtrlReg5 & CAP_100BASE_MASK ) >> CAP_100BASE_OFFSET );

			if ( HCD >= 0x8 )
			{
				*speed = 1;
				*fullDuplex = 1;
			}
			else if ( HCD >= 0x4 )
			{
				*speed = 1;
				*fullDuplex = 0;
			}
			else if ( HCD >= 0x2 )
			{
				*speed = 0;
				*fullDuplex = 1;
			}
			else if ( HCD >= 0x1 )
			{
				*speed = 0;
				*fullDuplex = 0;
			}
			else
			{
				RTL_NOTICE(RTL_MSG_GENERIC, "No common denominator with the link partner?!");
			}

		}
	}
	else
	{
		*speed = (statCtrlReg0 & SPEED_SELECT_100M)? 1: 0;
		*fullDuplex = ( statCtrlReg0 & SELECT_FULL_DUPLEX )? TRUE: FALSE;
	}

	return SUCCESS;
}


int32 rtl8651_asicEthernetCableMeterInit(void){
	rtlglue_printf("NOT YET\n");
#if 0	
	uint32 old_value;
	//set PHY6 Reg0 TxD latch internal clock phase
	WRITE_MEM32(SWTAA, 0xbc8020c0);
	WRITE_MEM32(TCR0,0x56);
 	WRITE_MEM32(SWTACR,ACTION_START | CMD_FORCE);
 #ifndef RTL865X_TEST
	while ( (READ_MEM32(SWTACR) & ACTION_MASK) != ACTION_DONE );//Wait for command done
#endif /* RTL865X_TEST */
	//set PHY7 Reg 5
	WRITE_MEM32(SWTAA, 0xbc8020f4);
	WRITE_MEM32(TCR0,0x4b68);
 	WRITE_MEM32(SWTACR,ACTION_START | CMD_FORCE);
 #ifndef RTL865X_TEST
	while ( (READ_MEM32(SWTACR) & ACTION_MASK) != ACTION_DONE );//Wait for command done
#endif /* RTL865X_TEST */
	//set PHY7 Reg 6
	WRITE_MEM32(SWTAA, 0xbc8020f8);
	WRITE_MEM32(TCR0,0x0380);
 	WRITE_MEM32(SWTACR,ACTION_START | CMD_FORCE);
 #ifndef RTL865X_TEST
	while ( (READ_MEM32(SWTACR) & ACTION_MASK) != ACTION_DONE );//Wait for command done
#endif /* RTL865X_TEST */
	old_value=READ_MEM32(0xbc8020fc);
	//set PHY7 Reg 7
	WRITE_MEM32(SWTAA, 0xbc8020fc);
	WRITE_MEM32(TCR0,old_value|0x300);
 	WRITE_MEM32(SWTACR,ACTION_START | CMD_FORCE);
 #ifndef RTL865X_TEST
	while ( (READ_MEM32(SWTACR) & ACTION_MASK) != ACTION_DONE );//Wait for command done
#endif /* RTL865X_TEST */
#endif
return SUCCESS;
}


int32 rtl8651_asicEthernetCableMeter(uint32 port, int32 *rxStatus, int32 *txStatus) {
	uint32 i, tmp1, tmp2, tmp3;

	/* not for ext port */
	if((port >= RTL8651_PHY_NUMBER)||!rxStatus || !txStatus)
		return FAILED;

	/* Enable PHY Access */
	WRITE_MEM32(MDCIOCR, (0x0 << 31) | (6 << 24) | (0 << 16) | 0x0000);
	while ((READ_MEM32(MDCIOSR) & 0x80000000) != 0);
	tmp1 = READ_MEM32(MDCIOSR);
	WRITE_MEM32(MDCIOCR, (0x1 << 31) | (6 << 24) | (0 << 16) | tmp1 | 0x1200);
	while ((READ_MEM32(MDCIOSR) & 0x80000000) != 0);

	/* Set Internal Parameter */
	WRITE_MEM32(MDCIOCR, (0x0 << 31) | (6 << 24) | (31 << 16) | 0x0000);
	while ((READ_MEM32(MDCIOSR) & 0x80000000) != 0);
	tmp2 = READ_MEM32(MDCIOSR);
	WRITE_MEM32(MDCIOCR, (0x1 << 31) | (6 << 24) | (31 << 16) | tmp2 | (1 << 14) | (0x1F << 8));
	while ((READ_MEM32(MDCIOSR) & 0x80000000) != 0);

	/* Set RTCT Parameter */
	WRITE_MEM32(MDCIOCR, (0x0 << 31) | (6 << 24) | (1 << 16) | 0x0000);
	while ((READ_MEM32(MDCIOSR) & 0x80000000) != 0);
	tmp3 = READ_MEM32(MDCIOSR);
	WRITE_MEM32(MDCIOCR, (0x1 << 31) | (6 << 24) | (1 << 16) | tmp3 | (0x3 << 6));
	while ((READ_MEM32(MDCIOSR) & 0x80000000) != 0);

	/* Fine Tune RTCT Parameter */ 
	/* These Parameters Seem Not Suit for RTL8652 - Temporarily Disabled */
	#if 0
	WRITE_MEM32(MDCIOCR, (0x1 << 31) | (6 << 24) | (10 << 16) | (0x0000 << 6));
	while ((READ_MEM32(MDCIOSR) & 0x80000000) != 0);
	WRITE_MEM32(MDCIOCR, (0x1 << 31) | (6 << 24) | (11 << 16) | (0x07FA << 6));
	while ((READ_MEM32(MDCIOSR) & 0x80000000) != 0);
	WRITE_MEM32(MDCIOCR, (0x1 << 31) | (6 << 24) | (12 << 16) | (0xFC29 << 6));
	while ((READ_MEM32(MDCIOSR) & 0x80000000) != 0);
	WRITE_MEM32(MDCIOCR, (0x1 << 31) | (6 << 24) | (13 << 16) | (0x7AB5 << 6));
	while ((READ_MEM32(MDCIOSR) & 0x80000000) != 0);
	WRITE_MEM32(MDCIOCR, (0x1 << 31) | (6 << 24) | (14 << 16) | (0xA458 << 6));
	while ((READ_MEM32(MDCIOSR) & 0x80000000) != 0);
	#endif

	/* Set Port */
	WRITE_MEM32(MDCIOCR, (0x0 << 31) | (6 << 24) | (0 << 16) | 0x0000);
	while ((READ_MEM32(MDCIOSR) & 0x80000000) != 0);
	WRITE_MEM32(MDCIOCR, (0x1 << 31) | (6 << 24) | (0 << 16) | READ_MEM32(MDCIOSR) | (port << 4));
	while ((READ_MEM32(MDCIOSR) & 0x80000000) != 0);

	/* Set TX */
	WRITE_MEM32(MDCIOCR, (0x1 << 31) | (6 << 24) | (17 << 16) | 0x8000);
	while ((REG32(MDCIOSR) & 0x80000000) != 0);

	/* Start Test */
	WRITE_MEM32(MDCIOCR, (0x1 << 31) | (6 << 24) | (17 << 16) | 0x9000);
	while ((REG32(MDCIOSR) & 0x80000000) != 0);

	/* Wait and Check status */
	for(i=0;i<10000000;i++)
	{
		WRITE_MEM32(MDCIOCR, (0x0 << 31) | (6 << 24) | (17 << 16) | 0x0000);
		while ((READ_MEM32(MDCIOSR) & 0x80000000) != 0);
		if (READ_MEM32(MDCIOSR)&0x800)
		{
			*txStatus = READ_MEM32(MDCIOSR)&0x7ff;
			break;
		}
	}

	/* Set RX */
	WRITE_MEM32(MDCIOCR, (0x1 << 31) | (6 << 24) | (17 << 16) | 0x0000);
	while ((REG32(MDCIOSR) & 0x80000000) != 0);

	/* Start Test */
	WRITE_MEM32(MDCIOCR, (0x1 << 31) | (6 << 24) | (17 << 16) | 0x1000);
	while ((REG32(MDCIOSR) & 0x80000000) != 0);

	/* Wait and Check status */
	for(i=0;i<10000000;i++)
	{
		WRITE_MEM32(MDCIOCR, (0x0 << 31) | (6 << 24) | (17 << 16) | 0x0000);
		while ((READ_MEM32(MDCIOSR) & 0x80000000) != 0);
		if (READ_MEM32(MDCIOSR)&0x800)
		{
			*rxStatus = READ_MEM32(MDCIOSR)&0x7ff;
			break;
		}
	}

	/* Restore RTCT Parameters */
	WRITE_MEM32(MDCIOCR, (0x1 << 31) | (6 << 24) | (1 << 16) | tmp3);
	while ((READ_MEM32(MDCIOSR) & 0x80000000) != 0);

	/* Restore Internal Parameters */
	WRITE_MEM32(MDCIOCR, (0x1 << 31) | (6 << 24) | (31 << 16) | tmp2);
	while ((READ_MEM32(MDCIOSR) & 0x80000000) != 0);

	/* Disable PHY Access */
	WRITE_MEM32(MDCIOCR, (0x1 << 31) | (6 << 24) | (0 << 16) | tmp1);
	while ((READ_MEM32(MDCIOSR) & 0x80000000) != 0);

	return SUCCESS;
}


/*=========================================
  * ASIC DRIVER API: BANDWIDTH CONTROL
  *=========================================*/
#define RTL865X_ASIC_DRIVER_BANDWIDTH_CONTROL_API

/*
 *	_rtl8651_syncToAsicEthernetBandwidthControl()
 *
 *	Sync SW bandwidth control () configuration to ASIC:
 *
 *
 *		_rtl865xB_BandwidthCtrlPerPortConfiguration -----> Translate from RTL865xB Index to ACTUAL
 *														 	 token count in RTL865xC
 *																		|
 *																---------
 *																|
 *		_rtl865xB_BandwidthCtrlMultiplier	---- Translate using         ---->*
 *										 RTL865xB's mechanism		|
 *																|
 *											---------------------
 *											|
 *											-- > Actual Token count which need to set to ASIC.
 *												 => Set it to ASIC if value in SW is different from ASIC.
 *
*/
static void _rtl8651_syncToAsicEthernetBandwidthControl(void)
{
	uint32 port;
	uint32 cfgTypeIdx;
	int32 retval;

	for ( port = 0 ; port < RTL8651_PORT_NUMBER ; port ++ )
	{
		for ( cfgTypeIdx = 0 ; cfgTypeIdx < _RTL865XB_BANDWIDTHCTRL_CFGTYPE ; cfgTypeIdx ++ )
		{
			uint32 currentSwBandwidthCtrlBasicSetting;
			uint32 currentSwBandwidthCtrlMultiplier;
			uint32 currentSwBandwidthCtrlSetting;
			uint32 currentAsicBandwidthCtrlSetting;
			
			/*
				We would check for rate and _rtl865xB_BandwidthCtrlMultiplier for the rate-multiply.

				In RTL865xB, the bits definition is as below.

				SWTECR

				bit 14(x8)		bit 15 (x4)		Result
				=============================================
				0				0				x1
				0				1				x4
				1				0				x8
				1				1				x8
			*/
			if (_rtl865xB_BandwidthCtrlMultiplier & _RTL865XB_BANDWIDTHCTRL_X8)
			{	/* case {b'10, b'11} */
				currentSwBandwidthCtrlMultiplier = 8;
			} else if ( _rtl865xB_BandwidthCtrlMultiplier & _RTL865XB_BANDWIDTHCTRL_X4)
			{	/* case {b'01} */
				currentSwBandwidthCtrlMultiplier = 4;
			} else
			{	/* case {b'00} */
				currentSwBandwidthCtrlMultiplier = 1;
			}

#ifdef	CONFIG_RTL865XB
			/* Calculate Current SW configuration : 0 : Full Rate */
			currentSwBandwidthCtrlBasicSetting = (cfgTypeIdx == 0)?
					/* Ingress */
					_rtl865xB_BandwidthCtrlIngressSettingUnit[_rtl865xB_BandwidthCtrlPerPortConfiguration[port][cfgTypeIdx]]:
					/* Egress */
					_rtl865xB_BandwidthCtrlEgressSettingUnit[_rtl865xB_BandwidthCtrlPerPortConfiguration[port][cfgTypeIdx]];

			/* Mix BASIC setting and Multiplier -> to get the ACTUAL bandwidth setting */
			currentSwBandwidthCtrlSetting =	currentSwBandwidthCtrlBasicSetting * currentSwBandwidthCtrlMultiplier;
#elif	defined(CONFIG_RTL865XC)
			/* Calculate Current SW configuration : 0 : Full Rate */
			/* Mix BASIC setting and Multiplier -> to get the ACTUAL bandwidth setting */

			currentSwBandwidthCtrlBasicSetting = ((_rtl865xC_BandwidthCtrlNum[_rtl865xB_BandwidthCtrlPerPortConfiguration[port][cfgTypeIdx]])*(currentSwBandwidthCtrlMultiplier));
			currentSwBandwidthCtrlSetting = (cfgTypeIdx == 0)?
					/* Ingress */
					(((currentSwBandwidthCtrlBasicSetting%RTL865XC_INGRESS_16KUNIT)<(RTL865XC_INGRESS_16KUNIT>>1))?(currentSwBandwidthCtrlBasicSetting/RTL865XC_INGRESS_16KUNIT):((currentSwBandwidthCtrlBasicSetting/RTL865XC_INGRESS_16KUNIT)+1)):
					/* Egress */
					(((currentSwBandwidthCtrlBasicSetting%RTL865XC_EGRESS_64KUNIT)<(RTL865XC_EGRESS_64KUNIT>>1))?(currentSwBandwidthCtrlBasicSetting/RTL865XC_EGRESS_64KUNIT):((currentSwBandwidthCtrlBasicSetting/RTL865XC_EGRESS_64KUNIT)+1));

#endif
			/* Get Current ASIC configuration */
			retval = (cfgTypeIdx == 0)?
					/* Ingress */
					(rtl8651_getAsicPortIngressBandwidth(	port,
															&currentAsicBandwidthCtrlSetting)):
					/* Egress */
					(rtl8651_getAsicPortEgressBandwidth(		port,
															&currentAsicBandwidthCtrlSetting));

			if ( retval != SUCCESS )
			{
				assert(0);
				goto out;
			}

			/* SYNC configuration to HW if the configuration is different */
			if (	(!( (currentSwBandwidthCtrlSetting) == 0 && (currentAsicBandwidthCtrlSetting == 0x3fff) ) /* for FULL Rate case */) ||
				( currentSwBandwidthCtrlSetting != currentAsicBandwidthCtrlSetting ))
			{
#if 0			
				if (cfgTypeIdx==0)
				{
					rtlglue_printf("set ingress bandwidth port %d, %d.\n", port, (currentSwBandwidthCtrlSetting == 0)?
																(0	/* For Ingress Bandwidth control, 0 means "disabled" */):
																(currentSwBandwidthCtrlSetting));
				}
				else 
				{
					rtlglue_printf("set ingress bandwidth port %d, %d.\n", port, (currentSwBandwidthCtrlSetting == 0)?
																(0x3fff	/* For Egress Bandwidth control, 0x3fff means "disabled" */):
																(currentSwBandwidthCtrlSetting));
				}
#endif				
				retval = (cfgTypeIdx == 0)?
					/* Ingress */
					(rtl8651_setAsicPortIngressBandwidth(	port,
															(currentSwBandwidthCtrlSetting == 0)?
																(0	/* For Ingress Bandwidth control, 0 means "disabled" */):
																(currentSwBandwidthCtrlSetting))		):
					/* Egress */
					(rtl8651_setAsicPortEgressBandwidth(		port,
															(currentSwBandwidthCtrlSetting == 0)?
																(0x3fff	/* For Egress Bandwidth control, 0x3fff means "disabled" */):
																(currentSwBandwidthCtrlSetting))		);

				if ( retval != SUCCESS )
				{
					assert(0);
					goto out;
				}
			}

		}
	}
out:
	return;
}

/*
 *	_rtl8651_syncFromAsicEthernetBandwidthControl()
 *
 *	Sync ASIC bandwidth control () configuration to SW:
 *
 *	NOTE: This function can be used only for GET function.
 *
 *
 *
 *		_rtl865xB_BandwidthCtrlPerPortConfiguration[][] -----> Translate from RTL865xB Index to ACTUAL
 *														 	 token count in RTL865xC
 *																		|
 *																---------
 *																|
 *		_rtl865xB_BandwidthCtrlMultiplier	---- Translate using         ---->*
 *										 RTL865xB's mechanism		|
 *																|
 *											---------------------
 *											|
 *											-- > Actual Token count which need to set to ASIC.
 *												 	|
 *												 	|
 *												 	|
 *		---------------------------------------------
 *		|
 *		----> ( If different ) -------- ( Not Full Rate ) -----
 *							|						|
 *							|						| 
 *						(Full Rate [0x3fff])				/ <----	Use _rtl865xB_BandwidthCtrlMultiplier
 *							|						|	       to reverse-translate ASIC's token
 *							|						|		to ACTUAL token.
 *							|<----------------------
 *							|
 *							--->	Reverse-translate ACTUAL token count to
 *									RTL865xC Bandwidth index.
 *												|
 *												|
 *												--> Set back to _rtl865xB_BandwidthCtrlPerPortConfiguration[][]
 *
 *
*/
static void _rtl8651_syncFromAsicEthernetBandwidthControl(void)
{
	uint32 port;
	uint32 cfgTypeIdx;

	int32 retval;

	for ( port = 0 ; port < RTL8651_PORT_NUMBER ; port ++ )
	{
		for ( cfgTypeIdx = 0 ; cfgTypeIdx < _RTL865XB_BANDWIDTHCTRL_CFGTYPE ; cfgTypeIdx ++ )
		{
			uint32 currentAsicBandwidthCtrlSetting;
			uint32 currentSwBandwidthCtrlBasicSetting;
			uint32 currentSwBandwidthCtrlMultiplier;
			uint32 currentSwBandwidthCtrlSetting;

			/* Get Current ASIC configuration */
			retval = (cfgTypeIdx == 0)?
					/* Ingress */
					(rtl8651_getAsicPortIngressBandwidth(	port,
															&currentAsicBandwidthCtrlSetting)):
					/* Egress */
					(rtl8651_getAsicPortEgressBandwidth(		port,
															&currentAsicBandwidthCtrlSetting));

			if ( retval != SUCCESS )
			{
				assert(0);
				goto out;
			}

			/*
				We would check for rate and _rtl865xB_BandwidthCtrlMultiplier for the rate-multiply.

				In RTL865xB, the bits definition is as below.

				SWTECR

				bit 14(x8)		bit 15 (x4)		Result
				=============================================
				0				0				x1
				0				1				x4
				1				0				x8
				1				1				x8
			*/
			if (_rtl865xB_BandwidthCtrlMultiplier & _RTL865XB_BANDWIDTHCTRL_X8)
			{	/* case {b'10, b'11} */
				currentSwBandwidthCtrlMultiplier = 8;
			} else if ( _rtl865xB_BandwidthCtrlMultiplier & _RTL865XB_BANDWIDTHCTRL_X4)
			{	/* case {b'01} */
				currentSwBandwidthCtrlMultiplier = 4;
			} else
			{	/* case {b'00} */
				currentSwBandwidthCtrlMultiplier = 1;
			}
			
#ifdef	CONFIG_RTL865XB
			/* Calculate Current SW configuration : 0 : Full Rate */
			currentSwBandwidthCtrlBasicSetting = (cfgTypeIdx == 0)?
					/* Ingress */
					_rtl865xB_BandwidthCtrlIngressSettingUnit[_rtl865xB_BandwidthCtrlPerPortConfiguration[port][cfgTypeIdx]]:
					/* Egress */
					_rtl865xB_BandwidthCtrlEgressSettingUnit[_rtl865xB_BandwidthCtrlPerPortConfiguration[port][cfgTypeIdx]];

			/* Mix BASIC setting and Multiplier -> to get the ACTUAL bandwidth setting */
			currentSwBandwidthCtrlSetting =	currentSwBandwidthCtrlBasicSetting * currentSwBandwidthCtrlMultiplier;
#elif	defined(CONFIG_RTL865XC)
			/* Calculate Current SW configuration : 0 : Full Rate */
			/* Mix BASIC setting and Multiplier -> to get the ACTUAL bandwidth setting */

			currentSwBandwidthCtrlBasicSetting = ((_rtl865xC_BandwidthCtrlNum[_rtl865xB_BandwidthCtrlPerPortConfiguration[port][cfgTypeIdx]])*(currentSwBandwidthCtrlMultiplier));
			currentSwBandwidthCtrlSetting = (cfgTypeIdx == 0)?
					/* Ingress */
					(((currentSwBandwidthCtrlBasicSetting%RTL865XC_INGRESS_16KUNIT)<(RTL865XC_INGRESS_16KUNIT>>1))?(currentSwBandwidthCtrlBasicSetting/RTL865XC_INGRESS_16KUNIT):((currentSwBandwidthCtrlBasicSetting/RTL865XC_INGRESS_16KUNIT)+1)):
					/* Egress */
					(((currentSwBandwidthCtrlBasicSetting%RTL865XC_EGRESS_64KUNIT)<(RTL865XC_EGRESS_64KUNIT>>1))?(currentSwBandwidthCtrlBasicSetting/RTL865XC_EGRESS_64KUNIT):((currentSwBandwidthCtrlBasicSetting/RTL865XC_EGRESS_64KUNIT)+1));

#endif

			/* SYNC configuration to SW if the configuration is different */
			if (	(!( (currentSwBandwidthCtrlSetting == 0) && (currentAsicBandwidthCtrlSetting == 0x3fff) ) /* for FULL Rate case */) ||
				( currentSwBandwidthCtrlSetting != currentAsicBandwidthCtrlSetting ))
			{
				uint32 mappedRtl865xBBandwidthCtrlIndex;

				/* Translate RTL865xC setting to RTL865xB's convention */
				if (cfgTypeIdx == 0)
				{	/* Ingress */
					if ( currentAsicBandwidthCtrlSetting == 0x3fff )
					{
						mappedRtl865xBBandwidthCtrlIndex = BW_FULL_RATE;
					} else
					{
#ifdef	CONFIG_RTL865XB					
						/* consider multiplier */
						currentAsicBandwidthCtrlSetting = currentAsicBandwidthCtrlSetting / currentSwBandwidthCtrlMultiplier;

						/* table lookup and map */
						switch ( currentAsicBandwidthCtrlSetting )
						{
							case 8:
								mappedRtl865xBBandwidthCtrlIndex = BW_128K;
								break;
							case 16:
								mappedRtl865xBBandwidthCtrlIndex = BW_256K;
								break;
							case 32:
								mappedRtl865xBBandwidthCtrlIndex = BW_512K;
								break;
							case 64:
								mappedRtl865xBBandwidthCtrlIndex = BW_1M;
								break;
							case 128:
								mappedRtl865xBBandwidthCtrlIndex = BW_2M;
								break;
							case 256:
								mappedRtl865xBBandwidthCtrlIndex = BW_4M;
								break;
							case 512:
								mappedRtl865xBBandwidthCtrlIndex = BW_8M;
								break;
							default:
								rtlglue_printf(	"[%s %d] SYNC Error for Backward compatible Bandwidth control (%d)\n",
											__FUNCTION__,
											__LINE__,
											currentAsicBandwidthCtrlSetting);
								goto out;
						}
#elif defined(CONFIG_RTL865XC)
						/* consider multiplier */
						currentAsicBandwidthCtrlSetting = (currentAsicBandwidthCtrlSetting*RTL865XC_INGRESS_16KUNIT) / (currentSwBandwidthCtrlMultiplier);
						if ( (currentAsicBandwidthCtrlSetting > (_rtl865xC_BandwidthCtrlNum[BW_128K]-RTL865XC_INGRESS_16KUNIT)) &&
							(currentAsicBandwidthCtrlSetting < (_rtl865xC_BandwidthCtrlNum[BW_128K]+RTL865XC_INGRESS_16KUNIT)))
						{
							mappedRtl865xBBandwidthCtrlIndex = BW_128K;
						}
						else if( (currentAsicBandwidthCtrlSetting > (_rtl865xC_BandwidthCtrlNum[BW_256K]-RTL865XC_INGRESS_16KUNIT)) &&
							(currentAsicBandwidthCtrlSetting < (_rtl865xC_BandwidthCtrlNum[BW_256K]+RTL865XC_INGRESS_16KUNIT)))
						{
							mappedRtl865xBBandwidthCtrlIndex = BW_256K;
						}
						else if( (currentAsicBandwidthCtrlSetting > (_rtl865xC_BandwidthCtrlNum[BW_512K]-RTL865XC_INGRESS_16KUNIT)) &&
							(currentAsicBandwidthCtrlSetting < (_rtl865xC_BandwidthCtrlNum[BW_512K]+RTL865XC_INGRESS_16KUNIT)))
						{
							mappedRtl865xBBandwidthCtrlIndex = BW_512K;
						}
						else if( (currentAsicBandwidthCtrlSetting > (_rtl865xC_BandwidthCtrlNum[BW_1M]-RTL865XC_INGRESS_16KUNIT)) &&
							(currentAsicBandwidthCtrlSetting < (_rtl865xC_BandwidthCtrlNum[BW_1M]+RTL865XC_INGRESS_16KUNIT)))
						{
							mappedRtl865xBBandwidthCtrlIndex = BW_1M;
						}
						else if( (currentAsicBandwidthCtrlSetting > (_rtl865xC_BandwidthCtrlNum[BW_2M]-RTL865XC_INGRESS_16KUNIT)) &&
							(currentAsicBandwidthCtrlSetting < (_rtl865xC_BandwidthCtrlNum[BW_2M]+RTL865XC_INGRESS_16KUNIT)))
						{
							mappedRtl865xBBandwidthCtrlIndex = BW_2M;
						}
						else if( (currentAsicBandwidthCtrlSetting > (_rtl865xC_BandwidthCtrlNum[BW_4M]-RTL865XC_INGRESS_16KUNIT)) &&
							(currentAsicBandwidthCtrlSetting < (_rtl865xC_BandwidthCtrlNum[BW_4M]+RTL865XC_INGRESS_16KUNIT)))
						{
							mappedRtl865xBBandwidthCtrlIndex = BW_4M;
						}
						else if( (currentAsicBandwidthCtrlSetting > (_rtl865xC_BandwidthCtrlNum[BW_8M]-RTL865XC_INGRESS_16KUNIT)) &&
							(currentAsicBandwidthCtrlSetting < (_rtl865xC_BandwidthCtrlNum[BW_8M]+RTL865XC_INGRESS_16KUNIT)))
						{
							mappedRtl865xBBandwidthCtrlIndex = BW_8M;
						} 
						else 
						{
							rtlglue_printf(	"[%s %d] SYNC Error for Backward compatible Bandwidth control (%d)\n",
											__FUNCTION__,
											__LINE__,
											currentAsicBandwidthCtrlSetting);
							goto out;
						}
#endif						
					}
				} else
				{	/* Egress */
					if ( currentAsicBandwidthCtrlSetting == 0x3fff )
					{
						mappedRtl865xBBandwidthCtrlIndex = BW_FULL_RATE;
					} else
					{
#ifdef	CONFIG_RTL865XB						
						/* consider multiplier */
						currentAsicBandwidthCtrlSetting = currentAsicBandwidthCtrlSetting / currentSwBandwidthCtrlMultiplier;

						/* table lookup and map */
						switch ( currentAsicBandwidthCtrlSetting )
						{
							case 2:
								mappedRtl865xBBandwidthCtrlIndex = BW_128K;
								break;
							case 4:
								mappedRtl865xBBandwidthCtrlIndex = BW_256K;
								break;
							case 8:
								mappedRtl865xBBandwidthCtrlIndex = BW_512K;
								break;
							case 16:
								mappedRtl865xBBandwidthCtrlIndex = BW_1M;
								break;
							case 32:
								mappedRtl865xBBandwidthCtrlIndex = BW_2M;
								break;
							case 64:
								mappedRtl865xBBandwidthCtrlIndex = BW_4M;
								break;
							case 128:
								mappedRtl865xBBandwidthCtrlIndex = BW_8M;
								break;
							default:
								rtlglue_printf(	"[%s %d] SYNC Error for Backward compatible Bandwidth control (%d)\n",
											__FUNCTION__,
											__LINE__,
											currentAsicBandwidthCtrlSetting);
								goto out;
						}
#elif defined(CONFIG_RTL865XC)
						/* consider multiplier */
						currentAsicBandwidthCtrlSetting = (currentAsicBandwidthCtrlSetting*RTL865XC_EGRESS_64KUNIT) / (currentSwBandwidthCtrlMultiplier);
						if ( (currentAsicBandwidthCtrlSetting > (_rtl865xC_BandwidthCtrlNum[BW_128K]-RTL865XC_EGRESS_64KUNIT)) &&
							(currentAsicBandwidthCtrlSetting < (_rtl865xC_BandwidthCtrlNum[BW_128K]+RTL865XC_EGRESS_64KUNIT)))
						{
							mappedRtl865xBBandwidthCtrlIndex = BW_128K;
						}
						else if( (currentAsicBandwidthCtrlSetting > (_rtl865xC_BandwidthCtrlNum[BW_256K]-RTL865XC_EGRESS_64KUNIT)) &&
							(currentAsicBandwidthCtrlSetting < (_rtl865xC_BandwidthCtrlNum[BW_256K]+RTL865XC_EGRESS_64KUNIT)))
						{
							mappedRtl865xBBandwidthCtrlIndex = BW_256K;
						}
						else if( (currentAsicBandwidthCtrlSetting > (_rtl865xC_BandwidthCtrlNum[BW_512K]-RTL865XC_EGRESS_64KUNIT)) &&
							(currentAsicBandwidthCtrlSetting < (_rtl865xC_BandwidthCtrlNum[BW_512K]+RTL865XC_EGRESS_64KUNIT)))
						{
							mappedRtl865xBBandwidthCtrlIndex = BW_512K;
						}
						else if( (currentAsicBandwidthCtrlSetting > (_rtl865xC_BandwidthCtrlNum[BW_1M]-RTL865XC_EGRESS_64KUNIT)) &&
							(currentAsicBandwidthCtrlSetting < (_rtl865xC_BandwidthCtrlNum[BW_1M]+RTL865XC_EGRESS_64KUNIT)))
						{
							mappedRtl865xBBandwidthCtrlIndex = BW_1M;
						}
						else if( (currentAsicBandwidthCtrlSetting > (_rtl865xC_BandwidthCtrlNum[BW_2M]-RTL865XC_EGRESS_64KUNIT)) &&
							(currentAsicBandwidthCtrlSetting < (_rtl865xC_BandwidthCtrlNum[BW_2M]+RTL865XC_EGRESS_64KUNIT)))
						{
							mappedRtl865xBBandwidthCtrlIndex = BW_2M;
						}
						else if( (currentAsicBandwidthCtrlSetting > (_rtl865xC_BandwidthCtrlNum[BW_4M]-RTL865XC_EGRESS_64KUNIT)) &&
							(currentAsicBandwidthCtrlSetting < (_rtl865xC_BandwidthCtrlNum[BW_4M]+RTL865XC_EGRESS_64KUNIT)))
						{
							mappedRtl865xBBandwidthCtrlIndex = BW_4M;
						}
						else if( (currentAsicBandwidthCtrlSetting > (_rtl865xC_BandwidthCtrlNum[BW_8M]-RTL865XC_EGRESS_64KUNIT)) &&
							(currentAsicBandwidthCtrlSetting < (_rtl865xC_BandwidthCtrlNum[BW_8M]+RTL865XC_EGRESS_64KUNIT)))
						{
							mappedRtl865xBBandwidthCtrlIndex = BW_8M;
						} 
						else 
						{
							rtlglue_printf(	"[%s %d] SYNC Error for Backward compatible Bandwidth control (%d)\n",
											__FUNCTION__,
											__LINE__,
											currentAsicBandwidthCtrlSetting);
							goto out;
						}
#endif
					}
				}

				/* Sync the configuration to SW table */
				_rtl865xB_BandwidthCtrlPerPortConfiguration[port][cfgTypeIdx] = mappedRtl865xBBandwidthCtrlIndex;

			}

		}
	}
out:
	return;
}


/*
@func int32 | rtl8651_setAsicEthernetBandwidthControl | set ASIC per-port total ingress bandwidth
@parm uint32 | port | the port number
@parm int8 | input | Ingress or egress control to <p port>
@parm uint32 | rate | rate to set.
@rvalue SUCCESS | 
@rvalue FAILED | invalid parameter
@comm
The <p rate> can be set to several different values:
BW_FULL_RATE
BW_128K
BW_256K
BW_512K
BW_1M
BW_2M
BW_4M
BW_8M

Note: This function is backward compatible to RTL865xB.
 */
int32 rtl8651_setAsicEthernetBandwidthControl(uint32 port, int8 input, uint32 rate)
{
	uint32 *currentConfig_p;

	if ( port >= RTL8651_PORT_NUMBER )
	{
		goto err;
	}

	switch ( rate )
	{
		case BW_FULL_RATE:
		case BW_128K:
		case BW_256K:
		case BW_512K:
		case BW_1M:
		case BW_2M:
		case BW_4M:
		case BW_8M:
			break;
		default:
			goto err;
	}

	currentConfig_p = &(_rtl865xB_BandwidthCtrlPerPortConfiguration[port][(input)?0 /* Ingress */:1 /* Egress */]);

	/* We just need to re-config HW when it's updated */
	if ( *currentConfig_p != rate )
	{
		/* Update configuration table */
		*currentConfig_p = rate;

		/* sync the configuration to ASIC */
		_rtl8651_syncToAsicEthernetBandwidthControl();
	}

	return SUCCESS;
err:
	return FAILED;
}

/*
@func int32 | rtl8651_getAsicEthernetBandwidthControl | get ASIC per-port total ingress bandwidth
@parm uint32 | port | the port number
@parm int8 | input | Ingress or egress control to <p port>
@parm uint32* | rate | rate to get.
@rvalue SUCCESS | 
@rvalue FAILED | invalid parameter
@comm
The <p rate> can be gotten in several different values:
BW_FULL_RATE
BW_128K
BW_256K
BW_512K
BW_1M
BW_2M
BW_4M
BW_8M

Note: This function is backward compatible to RTL865xB.
 */
int32 rtl8651_getAsicEthernetBandwidthControl(uint32 port, int8 input, uint32 *rate)
{
	if (rate == NULL)
	{
		goto err;
	}

	if ( port >= RTL8651_PORT_NUMBER )
	{
		goto err;
	}

	/* Always sync configuration from ASIC before getting it */
	_rtl8651_syncFromAsicEthernetBandwidthControl();

	*rate = _rtl865xB_BandwidthCtrlPerPortConfiguration[port][(input)?0/* Ingress */:1/* Egress */];

	return SUCCESS;

err:
	return FAILED;
}

/*
@func int32 | rtl8651_setAsicEthernetBandwidthControlX4 | set ASIC per-port bandwidth to 4-times
@parm int8 | enable | Enable or disable
@rvalue SUCCESS | 
@rvalue FAILED | invalid parameter
@comm
Set the bandwidth of each port to be 4-times.
Note: This function is backward compatible to RTL865xB.
 */
int32 rtl8651_setAsicEthernetBandwidthControlX4(int8 enable)
{
	int32 org_rtl865xB_BandwidthCtrlMultiplier;

	org_rtl865xB_BandwidthCtrlMultiplier = _rtl865xB_BandwidthCtrlMultiplier;

	if ( enable == TRUE )
	{
		_rtl865xB_BandwidthCtrlMultiplier |= _RTL865XB_BANDWIDTHCTRL_X4;
	} else
	{
		_rtl865xB_BandwidthCtrlMultiplier &= ~_RTL865XB_BANDWIDTHCTRL_X4;
	}

	/* Bandwidth Control Multiplier is modified, we need to rearrange ASIC setting for this modification */
	if (org_rtl865xB_BandwidthCtrlMultiplier != _rtl865xB_BandwidthCtrlMultiplier)
	{
		_rtl8651_syncToAsicEthernetBandwidthControl();
	}

	return SUCCESS;
}

/*
@func int32 | rtl8651_getAsicEthernetBandwidthControlX4 | get if ASIC per-port bandwidth to 4-times or not
@parm int8* | enable | Enable or disable
@rvalue SUCCESS | 
@rvalue FAILED | invalid parameter
@comm
Get the status about if the bandwidth of each port to be 4-times or not.
Note: This function is backward compatible to RTL865xB.
 */
int32 rtl8651_getAsicEthernetBandwidthControlX4(int8 *enable)
{
	if ( enable == NULL )
	{
		return FAILED;
	}

	*enable = ( _rtl865xB_BandwidthCtrlMultiplier & _RTL865XB_BANDWIDTHCTRL_X4 )?TRUE:FALSE;

	return SUCCESS;
}

/*
@func int32 | rtl8651_setAsicEthernetBandwidthControlX8 | set ASIC per-port bandwidth to 8-times
@parm int8 | enable | Enable or disable
@rvalue SUCCESS | 
@rvalue FAILED | invalid parameter
@comm
Set the bandwidth of each port to be 8-times.
Note: This function is backward compatible to RTL865xB.
 */
int32 rtl8651_setAsicEthernetBandwidthControlX8(int8 enable)
{
	int32 org_rtl865xB_BandwidthCtrlMultiplier;

	org_rtl865xB_BandwidthCtrlMultiplier = _rtl865xB_BandwidthCtrlMultiplier;

	if ( enable == TRUE )
	{
		_rtl865xB_BandwidthCtrlMultiplier |= _RTL865XB_BANDWIDTHCTRL_X8;
	} else
	{
		_rtl865xB_BandwidthCtrlMultiplier &= ~_RTL865XB_BANDWIDTHCTRL_X8;
	}

	/* Bandwidth Control Multiplier is modified, we need to rearrange ASIC setting for this modification */
	if (org_rtl865xB_BandwidthCtrlMultiplier != _rtl865xB_BandwidthCtrlMultiplier)
	{
		_rtl8651_syncToAsicEthernetBandwidthControl();
	}

	return SUCCESS;
}

/*
@func int32 | rtl8651_getAsicEthernetBandwidthControlX8 | get if ASIC per-port bandwidth to 8-times or not
@parm int8* | enable | Enable or disable
@rvalue SUCCESS | 
@rvalue FAILED | invalid parameter
@comm
Get the status about if the bandwidth of each port to be 8-times or not.
Note: This function is backward compatible to RTL865xB.
 */
int32 rtl8651_getAsicEthernetBandwidthControlX8(int8 *enable)
{
	if ( enable == NULL )
	{
		return FAILED;
	}

	*enable = ( _rtl865xB_BandwidthCtrlMultiplier & _RTL865XB_BANDWIDTHCTRL_X8 )?TRUE:FALSE;

	return SUCCESS;
}

/*=========================================
  * ASIC DRIVER API: MULTICAST PROCESSING CONTROL
  *=========================================*/
#define RTL865X_ASIC_DRIVER_MULTICAST_PROCESSING_CONTROL

/*
@func int32		| rtl8651_setAsicResvMcastAddrToCPU 	| Configure packets with L2 reserved multicast address to CPU or not
@parm uint32 | mCastAddrMask | Indicate the concerned multicast address.
@parm uint32 | enable | set TRUE to trap the packet to CPU; set FALSE to process the packet normally
@rvalue SUCCESS	| 	Success
@comm
Possible value of parameter mCastAddrMask:
(1) MADDR00 -- Reserved Address: 01-80-C2-00-00-00 Bridge Group Address
(2) MADDR02 -- Reserved Address: 01-80-C2-00-00-02 IEEE Std 802.3ad Slow_Protocols-Multicast address
(3) MADDR03 -- Reserved Address: 01-80-C2-00-00-03 IEEE Std 802.1X PAE address
(4) MADDR0E -- Reserved Address: 01-80-C2-00-00-0E IEEE Std. 802.1AB Link Layer Discovery protocol multicast address
(5) MADDR10 -- Reserved Address: 01-80-C2-00-00-10 All LANs Bridge Management Group Address
(6) MADDR20 -- Reserved Address: 01-80-C2-00-00-20 GMRP Address
(7) MADDR21 -- Reserved Address: 01-80-C2-00-00-21  GVRP address
(8) MADDR00_10 -- Reserved Address: 01-80-C2-00-00-00~01-80-C2-00-00-10 802.1d Reserved Address
(9) MADDR20_2F -- Reserved Address: 01-80-C2-00-00-20~01-80-C2-00-00-2FGARP Reserved Address
 */
int32 rtl8651_setAsicResvMcastAddrToCPU(uint32 mCastAddrMask, uint32 enable)
{
	if ( enable )
		WRITE_MEM32( RMACR, READ_MEM32( RMACR ) | mCastAddrMask );
	else
		WRITE_MEM32( RMACR, READ_MEM32( RMACR ) & ~mCastAddrMask );
	
	return SUCCESS;
}

/*=========================================
  * ASIC DRIVER API: PORT INTERNAL
  *=========================================*/
#define RTL865X_ASIC_DRIVER_IPV4_MULTICAST_CONTROL_API

/*
@func int32		|	rtl8651_setAsicMulticastEnable 	| Enable / disable ASIC IP multicast support.
@parm uint32		|	enable	| TRUE to indicate ASIC IP multicast process is enabled; FALSE to indicate ASIC IP multicast process is disabled.
@rvalue FAILED	|	Failed
@rvalue SUCCESS	|	Success
@comm
We would use this API to enable/disable ASIC IP multicast process.
If it's disabled here, Hardware IP multicast table would be ignored.
If it's enabled here, IP multicast table is used to forwarding IP multicast packets.
 */
int32 rtl8651_setAsicMulticastEnable(uint32 enable)
{
	if (enable == TRUE)
	{
		WRITE_MEM32(FFCR, READ_MEM32(FFCR)|EN_MCAST);
	} else
	{
		WRITE_MEM32(FFCR, READ_MEM32(FFCR) & ~EN_MCAST);
	}

	return SUCCESS;
}

/*
@func int32		|	rtl8651_getAsicMulticastEnable 	| Get the state about ASIC IP multicast support.
@parm uint32*		|	enable	| Pointer to store the state about ASIC IP multicast support.
@rvalue FAILED	|	Failed
@rvalue SUCCESS	|	Success
@comm
We would use this API to get the status of ASIC IP multicast process.
TRUE to indicate ASIC IP multicast process is enabled; FALSE to indicate ASIC IP multicast process is disabled.
*/
int32 rtl8651_getAsicMulticastEnable(uint32 *enable)
{
	if (enable == NULL)
	{
		return FAILED;
	}

	*enable = (READ_MEM32(FFCR) & EN_MCAST) ? TRUE : FALSE;

	return SUCCESS;
}

/*
@func int32		|	rtl8651_setAsicMulticastPortInternal 	| Configure internal/external state for each port
@parm uint32		|	port		| Port to set its state.
@parm int8		|	isInternal	| set TRUE to indicate <p port> is internal port; set FALSE to indicate <p port> is external port
@rvalue FAILED	|	Failed
@rvalue SUCCESS	|	Success
@comm
In RTL865x platform,
user would need to config the Internal/External state for each port to support HW multicast NAPT.
If packet is sent from internal port to external port, and source VLAN member port checking indicates that L34 is needed.
Source IP modification would be applied.
 */
int32 rtl8651_setAsicMulticastPortInternal(uint32 port, int8 isInternal)
{
	if (port >= RTL8651_PORT_NUMBER + rtl8651_totalExtPortNum)
	{	/* Invalid port */
		return FAILED;
	}

	/*
		RTL865XC : All multicast mode are stored in [SWTCR0 / Switch Table Control Register 0]
	*/
	if (isInternal == TRUE)
	{
		WRITE_MEM32(SWTCR0, READ_MEM32(SWTCR0) | (((1 << port) & MCAST_PORT_EXT_MODE_MASK) << MCAST_PORT_EXT_MODE_OFFSET));
	} else
	{
		WRITE_MEM32(SWTCR0, READ_MEM32(SWTCR0) & ~(((1 << port) & MCAST_PORT_EXT_MODE_MASK) << MCAST_PORT_EXT_MODE_OFFSET));
	}

	return SUCCESS;
}

/*
@func int32		|	rtl8651_getAsicMulticastPortInternal 	| Get internal/external state for each port
@parm uint32		|	port		| Port to set its state.
@parm int8*		|	isInternal	| Pointer to get port state of <p port>.
@rvalue FAILED	|	Failed
@rvalue SUCCESS	|	Success
@comm
To get the port internal / external state for <p port>:
TRUE to indicate <p port> is internal port; FALSE to indicate <p port> is external port
 */
int32 rtl8651_getAsicMulticastPortInternal(uint32 port, int8 *isInternal)
{
	if (isInternal == NULL)
	{
		return FAILED;
	}

	if (port >= RTL8651_PORT_NUMBER + rtl8651_totalExtPortNum)
	{	/* Invalid port */
		return FAILED;
	}

	if (READ_MEM32(SWTCR0) & (((1 << port) & MCAST_PORT_EXT_MODE_MASK) << MCAST_PORT_EXT_MODE_OFFSET))
	{
		*isInternal = TRUE;
	} else
	{
		*isInternal = FALSE;
	}

	return SUCCESS;
}

/*
@func int32		|	rtl8651_setAsicMulticastMTU 	| Set MTU for ASIC IP multicast forwarding
@parm uint32		|	mcastMTU	| MTU used by HW IP multicast forwarding.
@rvalue FAILED	|	Failed
@rvalue SUCCESS	|	Success
@comm
To set the MTU for ASIC IP multicast forwarding.
Its independent from other packet forwarding because IP multicast would include L2/L3/L4 at one time.
*/
int32 rtl8651_setAsicMulticastMTU(uint32 mcastMTU)
{
	if (mcastMTU & ~(MultiCastMTU_MASK) )
	{	/* multicast MTU overflow */
		return FAILED;
	}

	UPDATE_MEM32(ALECR, mcastMTU, MultiCastMTU_MASK, MultiCastMTU_OFFSET);

	return SUCCESS;
}

/*
@func int32		|	rtl8651_setAsicMulticastMTU 	| Get MTU for ASIC IP multicast forwarding
@parm uint32*	|	mcastMTU	| Pointer to get MTU used by HW IP multicast forwarding.
@rvalue FAILED	|	Failed
@rvalue SUCCESS	|	Success
@comm
To get the MTU value for ASIC IP multicast forwarding.
Its independent from other packet forwarding because IP multicast would include L2/L3/L4 at one time.
*/
int32 rtl8651_getAsicMulticastMTU(uint32 *mcastMTU)
{
	if (mcastMTU == NULL)
	{
		return FAILED;
	}

	*mcastMTU = GET_MEM32_VAL(ALECR, MultiCastMTU_MASK, MultiCastMTU_OFFSET);

	return SUCCESS;
}

/*=========================================
  * ASIC DRIVER API: L2 TABLE
  *=========================================*/
#define RTL865X_ASIC_DRIVER_L2_TBL_API

int32 rtl8651_setAsicL2Table(uint32 row, uint32 column, rtl865x_tblAsicDrv_l2Param_t *l2p) {
	rtl865xc_tblAsic_l2Table_t entry;

	if((row >= RTL8651_L2TBL_ROW) || (column >= RTL8651_L2TBL_COLUMN) || (l2p == NULL))
		return FAILED;
	if(l2p->macAddr.octet[5] != ((row^(fidHashTable[l2p->fid])^ l2p->macAddr.octet[0] ^ l2p->macAddr.octet[1] ^ l2p->macAddr.octet[2] ^ l2p->macAddr.octet[3] ^ l2p->macAddr.octet[4] ) & 0xff))
		return FAILED;

	memset(&entry, 0,sizeof(entry));
	entry.mac47_40 = l2p->macAddr.octet[0];
	entry.mac39_24 = (l2p->macAddr.octet[1] << 8) | l2p->macAddr.octet[2];
	entry.mac23_8 = (l2p->macAddr.octet[3] << 8) | l2p->macAddr.octet[4];


#if 1 //chhuang: #ifdef CONFIG_RTL8650B
	if( l2p->memberPortMask  > RTL8651_PHYSICALPORTMASK) //this MAC is on extension port
		entry.extMemberPort = (l2p->memberPortMask >>RTL8651_PORT_NUMBER);   
#endif /* CONFIG_RTL8650B */

	entry.memberPort = l2p->memberPortMask & RTL8651_PHYSICALPORTMASK;
	entry.toCPU = l2p->cpu==TRUE? 1: 0;
	entry.isStatic = l2p->isStatic==TRUE? 1: 0;
	entry.nxtHostFlag = l2p->nhFlag==TRUE? 1: 0;

	/* RTL865xC: modification of age from ( 2 -> 3 -> 1 -> 0 ) to ( 3 -> 2 -> 1 -> 0 ). modification of granularity 100 sec to 150 sec. */
	entry.agingTime = ( l2p->ageSec > 300 )? 0x03: ( l2p->ageSec <= 300 && l2p->ageSec > 150 )? 0x02: (l2p->ageSec <= 150 && l2p->ageSec > 0 )? 0x01: 0x00;
	
	entry.srcBlock = (l2p->srcBlk==TRUE)? 1: 0;
	entry.fid=l2p->fid;
	entry.auth=l2p->auth;
	return _rtl8651_forceAddAsicEntry(TYPE_L2_SWITCH_TABLE, row<<2 | column, &entry);
}

int32 rtl8651_delAsicL2Table(uint32 row, uint32 column) {
	rtl8651_tblAsic_l2Table_t entry;

	if(row >= RTL8651_L2TBL_ROW || column >= RTL8651_L2TBL_COLUMN)
		return FAILED;

	bzero(&entry, sizeof(entry));
	return _rtl8651_forceAddAsicEntry(TYPE_L2_SWITCH_TABLE, row<<2 | column, &entry);
}


ether_addr_t cachedDA;
static uint32 cachedMbr;
unsigned int rtl8651_asicL2DAlookup(uint8 *dmac){
	uint32 column;
//	rtl8651_tblAsic_l2Table_t   entry;
	rtl865xc_tblAsic_l2Table_t	entry;
	
//	unsigned int row = dmac[0]^dmac[1]^dmac[2]^dmac[3]^dmac[4]^dmac[5];
	uint32 row = rtl8651_filterDbIndex((ether_addr_t *)dmac, 0);
	//rtlglue_printf("mac %02x %02x %02x %02x %02x %02x \n",	mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);

#if 0 //chhuang:
	//cache hit...
	if(memcmp(&cachedDA, dmac, 6)==0)
		return cachedMbr;
#endif

	//cache miss...
	cachedMbr=0;
	for(column=0;column<RTL8651_L2TBL_COLUMN; column++) {
/* Should be fixed 		WRITE_MEM32(TEACR,READ_MEM32(TEACR)|0x1);ASIC patch: disable L2 Aging while reading L2 table */
		_rtl8651_readAsicEntry(TYPE_L2_SWITCH_TABLE, row<<2 | column, &entry);
/*		WRITE_MEM32(TEACR,READ_MEM32(TEACR)&~0x1); ASIC patch: enable L2 Aging aftrer reading L2 table */
		if(entry.agingTime == 0 && entry.isStatic == 0)
			continue;
		if(	dmac[0]==entry.mac47_40 &&
		    	dmac[1]==(entry.mac39_24>>8) &&
		    	dmac[2]==(entry.mac39_24 & 0xff)&&
		    	dmac[3]==(entry.mac23_8 >> 8)&&
		    	dmac[4]==(entry.mac23_8 & 0xff)&&
			dmac[5]== (row ^dmac[0]^dmac[1]^dmac[2]^dmac[3]^dmac[4])){

			cachedDA=*((ether_addr_t *)dmac);
			cachedMbr =(entry.extMemberPort<<RTL8651_PORT_NUMBER) | entry.memberPort;
			return cachedMbr;
		}
	}
	if(column==RTL8651_L2TBL_COLUMN)
		return 0xffffffff;//can't find this MAC, broadcast it.
	return cachedMbr;
}



int32 rtl8651_getAsicL2Table(uint32 row, uint32 column, rtl865x_tblAsicDrv_l2Param_t *l2p) {
	rtl865xc_tblAsic_l2Table_t   entry;
 
	if((row >= RTL8651_L2TBL_ROW) || (column >= RTL8651_L2TBL_COLUMN) || (l2p == NULL))
		return FAILED;

/*	RTL865XC should fix this problem.WRITE_MEM32(TEACR,READ_MEM32(TEACR)|0x1); ASIC patch: disable L2 Aging while reading L2 table */
	_rtl8651_readAsicEntry(TYPE_L2_SWITCH_TABLE, row<<2 | column, &entry);
	//WRITE_MEM32(TEACR,READ_MEM32(TEACR)&0x1); ASIC patch: enable L2 Aging aftrer reading L2 table */

	if(entry.agingTime == 0 && entry.isStatic == 0)
		return FAILED;
	l2p->macAddr.octet[0] = entry.mac47_40;
	l2p->macAddr.octet[1] = entry.mac39_24 >> 8;
	l2p->macAddr.octet[2] = entry.mac39_24 & 0xff;
	l2p->macAddr.octet[3] = entry.mac23_8 >> 8;
	l2p->macAddr.octet[4] = entry.mac23_8 & 0xff;
	l2p->macAddr.octet[5] = row ^ l2p->macAddr.octet[0] ^ l2p->macAddr.octet[1] ^ l2p->macAddr.octet[2] ^ l2p->macAddr.octet[3] ^ l2p->macAddr.octet[4]  ^(fidHashTable[entry.fid]);
	l2p->cpu = entry.toCPU==1? TRUE: FALSE;
	l2p->srcBlk = entry.srcBlock==1? TRUE: FALSE;
	l2p->nhFlag = entry.nxtHostFlag==1? TRUE: FALSE;
	l2p->isStatic = entry.isStatic==1? TRUE: FALSE;
#if 1 //chhuang: #ifdef CONFIG_RTL8650B
	l2p->memberPortMask = (entry.extMemberPort<<RTL8651_PORT_NUMBER) | entry.memberPort;
#else
	l2p->memberPortMask = entry.memberPort;
#endif /* CONFIG_RTL8650B */

	/* RTL865xC: modification of age from ( 2 -> 3 -> 1 -> 0 ) to ( 3 -> 2 -> 1 -> 0 ). modification of granularity 100 sec to 150 sec. */
	l2p->ageSec = entry.agingTime * 150;

	l2p->fid=entry.fid;
	l2p->auth=entry.auth;
	return SUCCESS;
}




/*=========================================
  * ASIC DRIVER API: PORT TRUNKING
  *=========================================*/
#define RTL865X_ASIC_DRIVER_TRUNKING_API

/*
@func int32 | rtl8651_updateAsicLinkAggregatorLMPR | Arrange the table which maps hashed index to port.
@parm	uint32	|	portMask |  Specify the port mask for the aggregator.
@rvalue SUCCESS | Update the mapping table successfully.
@rvalue FAILED | When the port mask is invalid, return FAILED
@comm
RTL865x provides an aggregator port. This API updates the table which maps hashed index to port.
If portmask = 0: clear all aggregation port mappings.
Rearrange policy is round-robin. ie. if port a,b,c is in portmask, then hash block 0~7's port number is a,b,c,a,b,c,a,b
*/
int32  rtl8651_updateAsicLinkAggregatorLMPR(int32 portmask) 
{
	uint32 hIdx, portIdx, reg;

	/* Clear all mappings */
	WRITE_MEM32( LAGHPMR0, 0 );	
	
	if ( portmask == 0 )
	{
		return SUCCESS;
	}

	reg = 0;
	portIdx = 0;	
	for ( hIdx = 0; hIdx < RTL865XC_LAGHASHIDX_NUMBER; hIdx++ )
	{		
		while ( ( ( 1 << portIdx ) & portmask ) == 0 )	/* Don't worry about infinite loop because portmask won't be 0. */
		{
			portIdx = ( portIdx + 1 ) % ( RTL8651_PORT_NUMBER + rtl8651_totalExtPortNum );
		}

		reg |= ( portIdx << ( hIdx * LAG_HASHIDX_BITNUM ) );
		portIdx = ( portIdx + 1 ) % ( RTL8651_PORT_NUMBER + rtl8651_totalExtPortNum );
	}

	WRITE_MEM32( LAGHPMR0, reg );
	return SUCCESS;
}

/*
@func int32 | rtl8651_setAsicLinkAggregator | Configure the aggregator port mask.
@parm	uint32	|	portMask |  Specify the port mask for the aggregator.
@rvalue SUCCESS | When successfully setting the aggregator portmask, return SUCCESS
@rvalue FAILED | When the port mask is invalid, return FAILED
@comm
RTL865x provides an aggregator port. This API configures the port member of this aggregator.
*/
int32 rtl8651_setAsicLinkAggregator(uint32 portMask)
{
	uint32 lnkUp_portMask;

	/* portMask invalidation */
	if ( portMask >= ( 1 << RTL8651_AGGREGATOR_NUMBER ) )
		return FAILED;

	/* Configure port mask for the aggregator */
	WRITE_MEM32( LAGCR0, ( READ_MEM32( LAGCR0 ) & TRUNKMASK_MASK ) | portMask );

	/* Find the link-up portmask */
	lnkUp_portMask = _rtl8651_findAsicLinkupPortmask(portMask);

	/* Arrange the table which maps hashed index to port */
	rtl8651_updateAsicLinkAggregatorLMPR(lnkUp_portMask);

	return SUCCESS;
}

int32 rtl8651_getAsicLinkAggregator(uint32 *portMask, uint32 *mapping)
{
	*portMask = READ_MEM32( LAGCR0 ) & TRUNKMASK_MASK;
	*mapping = READ_MEM32( LAGHPMR0 );
	return SUCCESS;
}






/*=========================================
  * ASIC DRIVER API: PVID 
  *=========================================*/
#define RTL865X_ASIC_DRIVER_PVID_API

int32 rtl8651_clearAsicPvid()
{


	WRITE_MEM32( PVCR0,READ_MEM32(PVCR0) &( ~(0xfff<<16)) & (~0xffff));
	WRITE_MEM32( PVCR1,READ_MEM32(PVCR1) &( ~(0xfff<<16)) & (~0xffff));
	WRITE_MEM32( PVCR2,READ_MEM32(PVCR2) &( ~(0xfff<<16)) & (~0xffff));

	return SUCCESS;
}
//extension port ready.
int32 rtl8651_setAsicPvid(uint32 port, uint32 pvid) {
	uint32 regValue,offset;
	
	if(port>=RTL8651_AGGREGATOR_NUMBER || pvid>=RTL865XC_VLAN_NUMBER)
		return FAILED;;
	offset=(port*2)&(~0x3);
	regValue=READ_MEM32(PVCR0+offset);
	if((port&0x1))
	{
		regValue=  ((pvid &0xfff) <<16) | (regValue&~0xFFF0000);
	}
	else
	{	
		regValue =  (pvid &0xfff) | (regValue &~0xFFF);
	}
	WRITE_MEM32(PVCR0+offset,regValue);
	return SUCCESS;
}

#if defined(CONFIG_DEFAULTS_KERNEL_2_6)
//extension port ready.
int32 rtl8651_getAsicPVlanId(uint32 port, uint32 *pvid) {
	uint16 offset;
	offset=(port*2)&(~0x3);
	if(port>=RTL8651_AGGREGATOR_NUMBER || pvid == NULL)
		return FAILED;
	if((port&0x1))
	{
		*pvid=(((READ_MEM32(PVCR0+offset)>>16)&0xFFF));		
	}
	else
	{
		*pvid=((READ_MEM32(PVCR0+offset)&0xFFF));
	}
	return SUCCESS;
}
#else
//extension port ready.
int32 rtl8651_getAsicPvid(uint32 port, uint32 *pvidx) {
	uint16 offset;
	offset=(port*2)&(~0x3);
	if(port>=RTL8651_AGGREGATOR_NUMBER || pvidx == NULL)
		return FAILED;
	if((port&0x1))
	{
		*pvidx=rtl8651_vlanTableIndex(((READ_MEM32(PVCR0+offset)>>16)&0xFFF));		
	}
	else
	{
		*pvidx=rtl8651_vlanTableIndex((READ_MEM32(PVCR0+offset)&0xFFF));
	}
	return SUCCESS;
}
#endif

/*  Full Capability
 *  Since FPGA does not have full entries of table, this flag indicate the mode is real IC.
 *
 *  defined  -- Real IC, have full entries for every SWCORE table.
 *  undef    -- FPGA, reduced entries for some tables.
 */
//#if defined(CONFIG_RTL865X_FPGA) ||\
	defined(CONFIG_RTL865X_MODEL_TEST_FT2) ||\
	defined(CONFIG_RTL865X_MODEL_KERNEL) ||\
	defined(VSV)
#undef FULL_CAP
//#else
//#define FULL_CAP
//#endif



/*=========================================
  * ASIC DRIVER API: INTERFACE TABLE
  *=========================================*/
#define RTL865X_ASIC_DRIVER_INTERFACE_TBL_API

/*
@func int32		| rtl865xC_setNetDecisionPolicy	| Set Interface Multilayer-Decision-Base Control 
@parm uint32 | policy | Possible values: NETIF_VLAN_BASED / NETIF_PORT_BASED / NETIF_MAC_BASED
@rvalue SUCCESS	| 	Success
@comm
RTL865xC supports Multilayer-Decision-Base for interface lookup.
 */
int32 rtl865xC_setNetDecisionPolicy( enum ENUM_NETDEC_POLICY policy )
{
	if ( policy == NETIF_PORT_BASED )
		WRITE_MEM32( SWTCR0, ( READ_MEM32( SWTCR0 ) & ~LIMDBC_MASK ) | LIMDBC_PORT );
	else if ( policy == NETIF_MAC_BASED )
		WRITE_MEM32( SWTCR0, ( READ_MEM32( SWTCR0 ) & ~LIMDBC_MASK ) | LIMDBC_MAC );
	else
		WRITE_MEM32( SWTCR0, ( READ_MEM32( SWTCR0 ) & ~LIMDBC_MASK ) | LIMDBC_VLAN );

	return SUCCESS;
}

/*
@func int32		| rtl865x_delNetInterfaceByVid	| Delete ASIC Interface Table according to Vlan ID
@parm uint16 | vid | vlan id .
@rvalue SUCCESS	| 	Success
@rvalue FAILED	| 	Failed
@comm
 */
int32 rtl865x_delNetInterfaceByVid(uint16 vid)
{
	rtl865xc_tblAsic_netifTable_t entry;
	uint32 i,netIfIdx;
	int32 retVal = FAILED;

	netIfIdx = RTL865XC_NETIFTBL_SIZE;

	/*search...*/
	for (i=0;i<RTL865XC_NETIFTBL_SIZE;i++)
	{
		_rtl8651_readAsicEntry(TYPE_NETINTERFACE_TABLE, i, &entry);
	 	if ( entry.valid && entry.vid==vid )
	 	{
	 		netIfIdx = i;
			break;
		}
	}

	if(netIfIdx < RTL865XC_NETIFTBL_SIZE)
	{
		bzero(&entry, sizeof(entry));
		retVal = _rtl8651_forceAddAsicEntry(TYPE_NETINTERFACE_TABLE, netIfIdx, &entry);		
	}

	return retVal;
	
}

/*
@func int32		| rtl8651_setAsicNetInterface	| Set ASIC Interface Table 
@parm uint32 | idx | Table index. Specific RTL865XC_NETIFTBL_SIZE to auto-search.
@parm rtl865x_tblAsicDrv_intfParam_t* | intfp | pointer to interface structure to add
@rvalue SUCCESS	| 	Success
@rvalue FAILED	| 	Failed
@comm
To read an interface entry, we provide two ways:
1. given the index which we want to force set
2. leave the index with RTL865XC_NETIFTBL_SIZE, we will search the whole table to find out existed entry or empty entry.
 */
 #ifdef RTL865X_TEST
 #define FULL_CAP
 #endif
int32 rtl8651_setAsicNetInterface( uint32 idx, rtl865x_tblAsicDrv_intfParam_t *intfp )
{
	rtl865xc_tblAsic_netifTable_t entry;
	uint32 i;
 
	if(intfp == NULL)
		return FAILED;

	if ( idx==RTL865XC_NETIFTBL_SIZE )
	{
		/* User does not specific idx, we shall find out idx first. */
#ifdef FULL_CAP
		/* search Interface table to see if exists */
		for (i=0;i<RTL865XC_NETIFTBL_SIZE;i++)
		{
			_rtl8651_readAsicEntry(TYPE_NETINTERFACE_TABLE, i, &entry);
	 		if ( entry.valid && entry.vid==intfp->vid )
	 		{
	 			idx = i;
				goto exist;
			}
		}	
		/* Not existed, find an empty entry */
		for (i=0;i<RTL865XC_NETIFTBL_SIZE;i++)
		{
			_rtl8651_readAsicEntry(TYPE_NETINTERFACE_TABLE, i, &entry);
			if ( !entry.valid )
				break;
		}
		if ( i>=RTL865XC_NETIFTBL_SIZE )
			return FAILED; /* no empty entry */
		idx = i;
#else
		/* search Interface table to see if exists */
		for (i=0;i<RTL865XC_NETIFTBL_SIZE;i++)
		{	
			/* Since FPGA only has entry 0,1,6,7, we ignore null entry. */
			if (i>1&&(i<(RTL865XC_NETIFTBL_SIZE-2))) continue;
			
			_rtl8651_readAsicEntry(TYPE_NETINTERFACE_TABLE, i, &entry);
			if (entry.valid)
				if (entry.vid==intfp->vid)
		 		{
		 			idx = i;
					goto exist;
				}
		}	
		/* Not existed, find an empty entry */
		for (i=0;i<RTL865XC_NETIFTBL_SIZE;i++)
		{	
			/* Since FPGA only has entry 0,1,6,7, we ignore null entry. */
			if (i>1&&(i<(RTL865XC_NETIFTBL_SIZE-2))) continue;

			_rtl8651_readAsicEntry(TYPE_NETINTERFACE_TABLE, i, &entry);
			if (!entry.valid)
			{ 
				break;
			}
		}
		if ( i>=RTL865XC_NETIFTBL_SIZE )
			return FAILED; /* no empty entry */
		idx = i;
#endif
	}
	
exist:
	assert( idx < RTL865XC_NETIFTBL_SIZE );

	bzero(&entry, sizeof(entry));
	entry.valid = intfp->valid;
	entry.vid = intfp->vid;
	entry.mac47_19 = (intfp->macAddr.octet[0]<<21) | (intfp->macAddr.octet[1]<<13) | (intfp->macAddr.octet[2]<<5) |
					 (intfp->macAddr.octet[3]>>3);
	entry.mac18_0 = (intfp->macAddr.octet[3]<<16) | (intfp->macAddr.octet[4]<<8) | (intfp->macAddr.octet[5]);

	entry.inACLStartH = (intfp->inAclStart >>2)&0x1f;
	entry.inACLStartL = intfp->inAclStart&0x3;
	entry.inACLEnd = intfp->inAclEnd;
	entry.outACLStart= intfp->outAclStart;
	entry.outACLEnd = intfp->outAclEnd;
	
	entry.enHWRoute = (rtl8651_getAsicOperationLayer()>2)?
		(intfp->enableRoute==TRUE? 1: 0):0;

	switch(intfp->macAddrNumber) {
		case 0:
		case 1:
		    entry.macMask = 7;
		break;
		case 2:
		    entry.macMask = 6;
		break;
		case 4:
		    entry.macMask = 4;
		break;
		case 8:
		    entry.macMask = 0;
			break;
		default:
		    return FAILED;//Not permitted macNumber value
	}
	entry.mtuH = intfp->mtu >>3;
	entry.mtuL = intfp->mtu & 0x7;

	return _rtl8651_forceAddAsicEntry(TYPE_NETINTERFACE_TABLE, idx, &entry);
}


/*
@func int32		| rtl8651_getAsicNetInterface	| Get ASIC Interface Table 
@parm uint32 | idx | Table index. 
@parm rtl865x_tblAsicDrv_intfParam_t* | intfp | pointer to store interface structure
@rvalue SUCCESS	| 	Success
@rvalue FAILED	| 	Failed. Possible reason: idx error, or invalid entry.
@comm
To read an interface entry, we provide two ways:
1. given the index which we want to read
2. leave the index with RTL865XC_NETIFTBL_SIZE, we will search the whole table according the given intfp->vid
 */
int32 rtl8651_getAsicNetInterface( uint32 idx, rtl865x_tblAsicDrv_intfParam_t *intfp )
{
	rtl865xc_tblAsic_netifTable_t entry;
	uint32 i;

	if(intfp == NULL)
		return FAILED;

	intfp->valid=0;

	if ( idx == RTL865XC_NETIFTBL_SIZE )
	{
		/* idx is not specified, we search whole interface table. */
		for( i = 0; i < RTL865XC_NETIFTBL_SIZE; i++ )
		{
#ifdef FULL_CAP
#else
			/* Since FPGA only has entry 0,1,6,7, we ignore null entry. */
			if (i>1&&(i<(RTL865XC_NETIFTBL_SIZE-2))) continue;
#endif

			_rtl8651_readAsicEntry(TYPE_NETINTERFACE_TABLE, i, &entry);
			if ( entry.valid && entry.vid==intfp->vid ){
				goto found;
			}
		}

		/* intfp.vid is not found. */
		return FAILED;
	}
	else
	{
		/* idx is specified, read from ASIC directly. */
		_rtl8651_readAsicEntry(TYPE_NETINTERFACE_TABLE, idx, &entry);
	}

found:
	intfp->valid=entry.valid;
	intfp->vid=entry.vid;
	intfp->macAddr.octet[0] = entry.mac47_19>>21;
	intfp->macAddr.octet[1] = (entry.mac47_19 >>13)&0xff;
	intfp->macAddr.octet[2] = (entry.mac47_19 >>5)&0xff;
	intfp->macAddr.octet[3] = ((entry.mac47_19 &0x3f) <<3) | (entry.mac18_0 >>16);
	intfp->macAddr.octet[4] = (entry.mac18_0 >> 8)&0xff;
	intfp->macAddr.octet[5] = entry.mac18_0 & 0xff;
	intfp->inAclEnd = entry.inACLEnd;
	intfp->inAclStart= (entry.inACLStartH<<2)|entry.inACLStartL;
	intfp->outAclStart = entry.outACLStart;
	intfp->outAclEnd = entry.outACLEnd;
	intfp->enableRoute = entry.enHWRoute==1? TRUE: FALSE;

	switch(entry.macMask)
	{
		case 0:
			intfp->macAddrNumber =8;
			break;
		case 6:
			intfp->macAddrNumber =2;
			break;
		case 4:
			intfp->macAddrNumber =4;
			break;
		case 7:
			intfp->macAddrNumber =1;
			break;

			
	}
	intfp->mtu = (entry.mtuH <<3)|entry.mtuL;

	return SUCCESS;
}


/*=========================================
  * ASIC DRIVER API: VLAN TABLE
  *=========================================*/
#define RTL865X_ASIC_DRIVER_VLAN_TBL_API
int32 rtl8651_setAsicVlan(uint16 vid, rtl865x_tblAsicDrv_vlanParam_t *vlanp) {
	rtl865xc_tblAsic_vlanTable_t entry;

	bzero(&entry,sizeof(entry));
	if(vlanp == NULL)
		return FAILED;        
	if (vid>=4096)
		return FAILED;
	if(vlanp->memberPortMask > RTL8651_PHYSICALPORTMASK )
		entry.extMemberPort = vlanp->memberPortMask >> RTL8651_PORT_NUMBER;
	if(vlanp->untagPortMask > RTL8651_PHYSICALPORTMASK )
		entry.extEgressUntag = vlanp->untagPortMask >> RTL8651_PORT_NUMBER;	
	entry.memberPort = vlanp->memberPortMask & RTL8651_PHYSICALPORTMASK;
	entry.egressUntag = vlanp->untagPortMask & RTL8651_PHYSICALPORTMASK;
	entry.fid=vlanp->fid;
	_rtl8651_forceAddAsicEntry(TYPE_VLAN_TABLE, vid, &entry);
	
	return SUCCESS;
}


int32 rtl8651_delAsicVlan(uint16 vid) {
	rtl8651_tblAsic_vlanTable_t entry;

	bzero(&entry, sizeof(entry));
	entry.valid = 0;
	return _rtl8651_forceAddAsicEntry(TYPE_VLAN_TABLE, vid, &entry);
}


int32 rtl8651_getAsicVlan(uint16 vid, rtl865x_tblAsicDrv_vlanParam_t *vlanp) {	
	rtl865xc_tblAsic_vlanTable_t entry;
	if(vlanp == NULL||vid>=4096)
		return FAILED;        

	_rtl8651_readAsicEntry(TYPE_VLAN_TABLE, vid, &entry);	
	if((entry.extMemberPort | entry.memberPort) == 0)
	{
		return FAILED;
	}
	vlanp->memberPortMask = (entry.extMemberPort<<RTL8651_PORT_NUMBER) | entry.memberPort;
	vlanp->untagPortMask = (entry.extEgressUntag<<RTL8651_PORT_NUMBER) |entry.egressUntag;
	vlanp->fid=entry.fid;
	return SUCCESS;
}


/*=========================================
  * ASIC DRIVER API: ASIC Counter
  *=========================================*/
#define RTL865X_ASIC_DRIVER_COUNTER_API

static void _rtl8651_initialRead(void) {//RTL8651 read counter for the first time will get value -1 and this is meaningless
	uint32 i;
	for(i=0; i<=0xac; i+=0x4)
	{
		rtl8651_returnAsicCounter(i);
	}
}

int32 rtl8651_returnAsicCounter(uint32 offset) 
{
	if(offset & 0x3)
		return 0;
	return  READ_MEM32(MIB_COUNTER_BASE + offset);
}

uint64 rtl865xC_returnAsicCounter64(uint32 offset)
{
	if ( offset & 0x3 )
		return 0;
	return ( READ_MEM32( MIB_COUNTER_BASE + offset ) + ( ( uint64 ) READ_MEM32( MIB_COUNTER_BASE + offset + 4 ) << 32 ) );
}

int32 rtl8651_clearAsicCounter(void) 
{
	WRITE_MEM32(MIB_CONTROL, ALL_COUNTER_RESTART_MASK);
#if 0	/* We don't want to read once first. */
	_rtl8651_initialRead();
#endif
	return SUCCESS;
}

int32 rtl8651_resetAsicMIBCounter(uint32 port, uint32 inCounterReset, uint32 outCounterReset ) 
{
	if(port==0xffffffff)
	{
		WRITE_MEM32(MIB_CONTROL, SYS_COUNTER_RESTART);
		return SUCCESS;
	}
	else
	{
		switch(port)
		{
			case 0:
				if(inCounterReset==TRUE)
				{
					WRITE_MEM32(MIB_CONTROL, IN_COUNTER_RESTART_P0);
				}
				
				if(outCounterReset==TRUE)
				{
					WRITE_MEM32(MIB_CONTROL, OUT_COUNTER_RESTART_P0);
				}
				break;
				
			case 1:
				if(inCounterReset==TRUE)
				{
					WRITE_MEM32(MIB_CONTROL, IN_COUNTER_RESTART_P1);
				}
				
				if(outCounterReset==TRUE)
				{
					WRITE_MEM32(MIB_CONTROL, OUT_COUNTER_RESTART_P1);
				}
				break;
			case 2:
				if(inCounterReset==TRUE)
				{
					WRITE_MEM32(MIB_CONTROL, IN_COUNTER_RESTART_P2);
				}
				
				if(outCounterReset==TRUE)
				{
					WRITE_MEM32(MIB_CONTROL, OUT_COUNTER_RESTART_P2);
				}
				break;
			case 3:
				if(inCounterReset==TRUE)
				{
					WRITE_MEM32(MIB_CONTROL, IN_COUNTER_RESTART_P3);
				}
				
				if(outCounterReset==TRUE)
				{
					WRITE_MEM32(MIB_CONTROL, OUT_COUNTER_RESTART_P3);
				}
				break;
			case 4:
				if(inCounterReset==TRUE)
				{
					WRITE_MEM32(MIB_CONTROL, IN_COUNTER_RESTART_P4);
				}
				
				if(outCounterReset==TRUE)
				{
					WRITE_MEM32(MIB_CONTROL, OUT_COUNTER_RESTART_P4);
				}
				break;
			case 5:
				if(inCounterReset==TRUE)
				{
					WRITE_MEM32(MIB_CONTROL, IN_COUNTER_RESTART_P5);
				}
				
				if(outCounterReset==TRUE)
				{
					WRITE_MEM32(MIB_CONTROL, OUT_COUNTER_RESTART_P5);
				}
				break;
			case 6:
				if(inCounterReset==TRUE)
				{
					WRITE_MEM32(MIB_CONTROL, IN_COUNTER_RESTART_P6);
				}
				
				if(outCounterReset==TRUE)
				{
					WRITE_MEM32(MIB_CONTROL, OUT_COUNTER_RESTART_P6);
				}
				break;
			case 7:
				if(inCounterReset==TRUE)
				{
					WRITE_MEM32(MIB_CONTROL, IN_COUNTER_RESTART_P7);
				}
				
				if(outCounterReset==TRUE)
				{
					WRITE_MEM32(MIB_CONTROL, OUT_COUNTER_RESTART_P7);
				}
				break;
				
			case 8:
				if(inCounterReset==TRUE)
				{
					WRITE_MEM32(MIB_CONTROL, IN_COUNTER_RESTART_P8);
				}
				
				if(outCounterReset==TRUE)
				{
					WRITE_MEM32(MIB_CONTROL, OUT_COUNTER_RESTART_P8);
				}
				break;
		
			default:
				rtlglue_printf("wrong port number\n"); 
				return FAILED;
		}
		
		return SUCCESS;
		
	}
	
}

int32 rtl8651_getSimpleAsicMIBCounter(uint32 port, rtl865x_tblAsicDrv_simpleCounterParam_t * simpleCounter) 
{
	uint32 addrOffset_fromP0 =0;
	
	if((port>6)||(simpleCounter==NULL) )
	{
		return FAILED;
	}
	
	addrOffset_fromP0= port* MIB_ADDROFFSETBYPORT;
	
	simpleCounter->rxBytes = rtl865xC_returnAsicCounter64( OFFSET_IFINOCTETS_P0 + addrOffset_fromP0 );
	simpleCounter->rxPkts = 	rtl8651_returnAsicCounter( OFFSET_IFINUCASTPKTS_P0 + addrOffset_fromP0 ) + rtl8651_returnAsicCounter( OFFSET_ETHERSTATSMULTICASTPKTS_P0 + addrOffset_fromP0 ) + rtl8651_returnAsicCounter( OFFSET_ETHERSTATSBROADCASTPKTS_P0 + addrOffset_fromP0 );
	simpleCounter->rxPausePkts=rtl8651_returnAsicCounter( OFFSET_DOT3INPAUSEFRAMES_P0 + addrOffset_fromP0 ) ;

	simpleCounter->drops = rtl8651_returnAsicCounter( OFFSET_DOT1DTPPORTINDISCARDS_P0 + addrOffset_fromP0 ) + rtl8651_returnAsicCounter( OFFSET_ETHERSTATSUNDERSIZEPKTS_P0 + addrOffset_fromP0 ) + rtl8651_returnAsicCounter( OFFSET_ETHERSTATSFRAGMEMTS_P0 + addrOffset_fromP0 ) + rtl8651_returnAsicCounter( OFFSET_ETHERSTATSOVERSIZEPKTS_P0 + addrOffset_fromP0 ) + rtl8651_returnAsicCounter( OFFSET_ETHERSTATSJABBERS_P0 + addrOffset_fromP0 ) + rtl8651_returnAsicCounter( OFFSET_ETHERSTATSDROPEVENTS_P0 + addrOffset_fromP0 ) + rtl8651_returnAsicCounter( OFFSET_DOT3STATSFCSERRORS_P0 + addrOffset_fromP0 ) + rtl8651_returnAsicCounter( OFFSET_DOT3STATSSYMBOLERRORS_P0 + addrOffset_fromP0 );
	simpleCounter->txBytes =rtl865xC_returnAsicCounter64( OFFSET_IFOUTOCTETS_P0 + addrOffset_fromP0 );				
	simpleCounter->txPkts =rtl8651_returnAsicCounter( OFFSET_IFOUTUCASTPKTS_P0 + addrOffset_fromP0 ) + rtl8651_returnAsicCounter( OFFSET_IFOUTMULTICASTPKTS_P0 + addrOffset_fromP0 ) + rtl8651_returnAsicCounter( OFFSET_IFOUTBROADCASTPKTS_P0 + addrOffset_fromP0 );
	simpleCounter->txPausePkts=rtl8651_returnAsicCounter( OFFSET_DOT3OUTPAUSEFRAMES_P0 + addrOffset_fromP0 );
	return SUCCESS;
}

int32 rtl8651_getAdvancedMIBCounter(uint32 port, uint32 asicCounterType, rtl865x_tblAsicDrv_advancedCounterParam_t * advancedCounter)
{
	uint32 addrOffset_fromP0 =0;
	
	if((port>6)||(advancedCounter==NULL) )
	{
		return FAILED;
	}
	
	addrOffset_fromP0= port* MIB_ADDROFFSETBYPORT;
	
	bzero(advancedCounter, sizeof(rtl865x_tblAsicDrv_advancedCounterParam_t));

	if((asicCounterType & ASIC_IN_COUNTERS)!=0)
	{
		advancedCounter->ifInOctets=rtl865xC_returnAsicCounter64(OFFSET_IFINOCTETS_P0 + addrOffset_fromP0);
		advancedCounter->ifInUcastPkts=rtl8651_returnAsicCounter(OFFSET_IFINUCASTPKTS_P0 + addrOffset_fromP0) ;
		advancedCounter->etherStatsOctets=rtl865xC_returnAsicCounter64(OFFSET_ETHERSTATSOCTETS_P0 + addrOffset_fromP0);
		advancedCounter->etherStatsUndersizePkts=rtl8651_returnAsicCounter( OFFSET_ETHERSTATSUNDERSIZEPKTS_P0 + addrOffset_fromP0);
		advancedCounter->etherStatsFraments=rtl8651_returnAsicCounter( OFFSET_ETHERSTATSFRAGMEMTS_P0 + addrOffset_fromP0);
		advancedCounter->etherStatsPkts64Octets=rtl8651_returnAsicCounter( OFFSET_ETHERSTATSPKTS64OCTETS_P0 + addrOffset_fromP0);
		advancedCounter->etherStatsPkts65to127Octets=rtl8651_returnAsicCounter( OFFSET_ETHERSTATSPKTS65TO127OCTETS_P0 + addrOffset_fromP0);
		advancedCounter->etherStatsPkts128to255Octets=rtl8651_returnAsicCounter( OFFSET_ETHERSTATSPKTS128TO255OCTETS_P0 + addrOffset_fromP0);
		advancedCounter->etherStatsPkts256to511Octets=rtl8651_returnAsicCounter( OFFSET_ETHERSTATSPKTS256TO511OCTETS_P0 + addrOffset_fromP0);
		advancedCounter->etherStatsPkts512to1023Octets=rtl8651_returnAsicCounter( OFFSET_ETHERSTATSPKTS512TO1023OCTETS_P0 + addrOffset_fromP0);
		advancedCounter->etherStatsPkts1024to1518Octets=rtl8651_returnAsicCounter( OFFSET_ETHERSTATSPKTS1024TO1518OCTETS_P0 + addrOffset_fromP0);
		advancedCounter->etherStatsOversizePkts=rtl8651_returnAsicCounter( OFFSET_ETHERSTATSOVERSIZEPKTS_P0 + addrOffset_fromP0);
		advancedCounter->etherStatsJabbers=rtl8651_returnAsicCounter( OFFSET_ETHERSTATSJABBERS_P0 + addrOffset_fromP0);
		advancedCounter->etherStatsMulticastPkts=rtl8651_returnAsicCounter( OFFSET_ETHERSTATSMULTICASTPKTS_P0 + addrOffset_fromP0);
		advancedCounter->etherStatsBroadcastPkts=rtl8651_returnAsicCounter( OFFSET_ETHERSTATSBROADCASTPKTS_P0 + addrOffset_fromP0);
		advancedCounter->dot1dTpPortInDiscards=rtl8651_returnAsicCounter( OFFSET_DOT1DTPPORTINDISCARDS_P0 + addrOffset_fromP0);
		advancedCounter->etherStatusDropEvents=rtl8651_returnAsicCounter( OFFSET_ETHERSTATSDROPEVENTS_P0 + addrOffset_fromP0);
		advancedCounter->dot3FCSErrors=rtl8651_returnAsicCounter( OFFSET_DOT3STATSFCSERRORS_P0 + addrOffset_fromP0);
		advancedCounter->dot3StatsSymbolErrors=rtl8651_returnAsicCounter( OFFSET_DOT3STATSSYMBOLERRORS_P0 + addrOffset_fromP0);
		advancedCounter->dot3ControlInUnknownOpcodes=rtl8651_returnAsicCounter( OFFSET_DOT3CONTROLINUNKNOWNOPCODES_P0 + addrOffset_fromP0);
		advancedCounter->dot3InPauseFrames=rtl8651_returnAsicCounter( OFFSET_DOT3INPAUSEFRAMES_P0 + addrOffset_fromP0);
	}
		
	if((asicCounterType & ASIC_OUT_COUNTERS)!=0)
	{
		advancedCounter->ifOutOctets=rtl865xC_returnAsicCounter64(OFFSET_IFOUTOCTETS_P0 + addrOffset_fromP0);
		advancedCounter->ifOutUcastPkts=rtl8651_returnAsicCounter(OFFSET_IFOUTUCASTPKTS_P0 + addrOffset_fromP0);
		advancedCounter->ifOutMulticastPkts=rtl8651_returnAsicCounter(OFFSET_IFOUTMULTICASTPKTS_P0 + addrOffset_fromP0);
		advancedCounter->ifOutBroadcastPkts=rtl8651_returnAsicCounter(OFFSET_IFOUTBROADCASTPKTS_P0 + addrOffset_fromP0);
		advancedCounter->ifOutDiscards=rtl8651_returnAsicCounter(OFFSET_IFOUTDISCARDS + addrOffset_fromP0);
		advancedCounter->dot3StatsSingleCollisionFrames=rtl8651_returnAsicCounter(OFFSET_DOT3STATSSINGLECOLLISIONFRAMES_P0+ addrOffset_fromP0);
		advancedCounter->dot3StatsMultipleCollisionFrames=rtl8651_returnAsicCounter(OFFSET_DOT3STATSMULTIPLECOLLISIONFRAMES_P0 + addrOffset_fromP0);
		advancedCounter->dot3StatsDefferedTransmissions=rtl8651_returnAsicCounter(OFFSET_DOT3STATSDEFERREDTRANSMISSIONS_P0 + addrOffset_fromP0);
		advancedCounter->dot3StatsLateCollisions=rtl8651_returnAsicCounter(OFFSET_DOT3STATSLATECOLLISIONS_P0 + addrOffset_fromP0);
		advancedCounter->dot3StatsExcessiveCollisions=rtl8651_returnAsicCounter(OFFSET_DOT3STATSEXCESSIVECOLLISIONS_P0 + addrOffset_fromP0);
		advancedCounter->dot3OutPauseFrames=rtl8651_returnAsicCounter(OFFSET_DOT3OUTPAUSEFRAMES_P0 + addrOffset_fromP0);
		advancedCounter->dot1dBasePortDelayExceededDiscards=rtl8651_returnAsicCounter(OFFSET_DOT1DBASEPORTDELAYEXCEEDEDDISCARDS_P0 + addrOffset_fromP0);
		advancedCounter->etherStatsCollisions=rtl8651_returnAsicCounter(OFFSET_ETHERSTATSCOLLISIONS_P0 + addrOffset_fromP0);

	}

	if((asicCounterType & ASIC_WHOLE_SYSTEM_COUNTERS)!=0)
	{
		advancedCounter->dot1dTpLearnedEntryDiscards=rtl8651_returnAsicCounter(MIB_ADDROFFSETBYPORT);
		advancedCounter->etherStatsCpuEventPkts=rtl8651_returnAsicCounter(MIB_ADDROFFSETBYPORT);
	}
	
	return SUCCESS;
}

/*
@func int32 | rtl865xC_dumpAsicCounter | Dump common counters of all ports (CPU port included).
@rvalue SUCCESS | Finish showing the counters.
@comm
Dump common counters of all ports. Includes Rx/Tx Bytes, Rx/Tx pkts, Rx/Tx Pause frames, Rx Drops.
*/
int32 rtl865xC_dumpAsicCounter(void)
{
	uint32 i;

	for ( i = 0; i <= RTL8651_PORT_NUMBER; i++ )
	{
		uint32 addrOffset_fromP0 = i * MIB_ADDROFFSETBYPORT;

		if ( i == RTL8651_PORT_NUMBER )
			rtlglue_printf("<CPU port>\n");
		else
			rtlglue_printf("<Port: %d>\n", i);
		rtlglue_printf("Rx %llu Bytes, Rx %u Pkts, Drop %u pkts, Rx Pause %u pkts\n", 
			rtl865xC_returnAsicCounter64( OFFSET_IFINOCTETS_P0 + addrOffset_fromP0 ),
			rtl8651_returnAsicCounter( OFFSET_IFINUCASTPKTS_P0 + addrOffset_fromP0 ) + rtl8651_returnAsicCounter( OFFSET_ETHERSTATSMULTICASTPKTS_P0 + addrOffset_fromP0 ) + rtl8651_returnAsicCounter( OFFSET_ETHERSTATSBROADCASTPKTS_P0 + addrOffset_fromP0 ),
			rtl8651_returnAsicCounter( OFFSET_DOT1DTPPORTINDISCARDS_P0 + addrOffset_fromP0 ) + rtl8651_returnAsicCounter( OFFSET_ETHERSTATSUNDERSIZEPKTS_P0 + addrOffset_fromP0 ) + rtl8651_returnAsicCounter( OFFSET_ETHERSTATSFRAGMEMTS_P0 + addrOffset_fromP0 ) + rtl8651_returnAsicCounter( OFFSET_ETHERSTATSOVERSIZEPKTS_P0 + addrOffset_fromP0 ) + rtl8651_returnAsicCounter( OFFSET_ETHERSTATSJABBERS_P0 + addrOffset_fromP0 ) + rtl8651_returnAsicCounter( OFFSET_ETHERSTATSDROPEVENTS_P0 + addrOffset_fromP0 ) + rtl8651_returnAsicCounter( OFFSET_DOT3STATSFCSERRORS_P0 + addrOffset_fromP0 ) + rtl8651_returnAsicCounter( OFFSET_DOT3STATSSYMBOLERRORS_P0 + addrOffset_fromP0 ), 
			rtl8651_returnAsicCounter( OFFSET_DOT3INPAUSEFRAMES_P0 + addrOffset_fromP0 ) );
		rtlglue_printf("Tx %llu Bytes, Tx %u Pkts Tx Pause %u pkts\n", 
			rtl865xC_returnAsicCounter64( OFFSET_IFOUTOCTETS_P0 + addrOffset_fromP0 ),
			rtl8651_returnAsicCounter( OFFSET_IFOUTUCASTPKTS_P0 + addrOffset_fromP0 ) + rtl8651_returnAsicCounter( OFFSET_IFOUTMULTICASTPKTS_P0 + addrOffset_fromP0 ) + rtl8651_returnAsicCounter( OFFSET_IFOUTBROADCASTPKTS_P0 + addrOffset_fromP0 ),
			rtl8651_returnAsicCounter( OFFSET_DOT3OUTPAUSEFRAMES_P0 + addrOffset_fromP0 ) );
	}

	return SUCCESS;
}

/*
@func int32 | rtl865xC_dumpAsicDiagCounter | Dump complex counters of all ports (CPU port included).
@rvalue SUCCESS | Finish showing the counters.
@comm
Dump complex counters of all ports.
*/
int32 rtl865xC_dumpAsicDiagCounter(void)
{
	uint32 i;

	for ( i = 0; i <= RTL8651_PORT_NUMBER; i++ )
	{
		uint32 addrOffset_fromP0 = i * MIB_ADDROFFSETBYPORT;
		
		if ( i == RTL8651_PORT_NUMBER )
			rtlglue_printf("<CPU port (extension port included)>\n");
		else
			rtlglue_printf("<Port: %d>\n", i);

		rtlglue_printf("Rx counters\n");
		rtlglue_printf("   Rcv %llu bytes, Drop %u pkts, CRCAlignErr %u, FragErr %u, JabberErr %u\n", 
			rtl865xC_returnAsicCounter64( OFFSET_IFINOCTETS_P0 + addrOffset_fromP0 ), 
			rtl8651_returnAsicCounter( OFFSET_DOT1DTPPORTINDISCARDS_P0 + addrOffset_fromP0 ), 
			rtl8651_returnAsicCounter( OFFSET_DOT3STATSFCSERRORS_P0 + addrOffset_fromP0 ), 
			rtl8651_returnAsicCounter( OFFSET_ETHERSTATSFRAGMEMTS_P0 + addrOffset_fromP0 ), 
			rtl8651_returnAsicCounter( OFFSET_ETHERSTATSJABBERS_P0 + addrOffset_fromP0 ));
		rtlglue_printf("   Unicast %u pkts, Multicast %u pkts, Broadcast %u pkts\n", 
			rtl8651_returnAsicCounter( OFFSET_IFINUCASTPKTS_P0 + addrOffset_fromP0 ), 
			rtl8651_returnAsicCounter( OFFSET_ETHERSTATSMULTICASTPKTS_P0 + addrOffset_fromP0 ), 
			rtl8651_returnAsicCounter( OFFSET_ETHERSTATSBROADCASTPKTS_P0 + addrOffset_fromP0 ));
		rtlglue_printf("   < 64: %u pkts, 64: %u pkts, 65 -127: %u pkts, 128 -255: %u pkts\n", 
			rtl8651_returnAsicCounter( OFFSET_ETHERSTATSUNDERSIZEPKTS_P0 + addrOffset_fromP0 ), 
			rtl8651_returnAsicCounter( OFFSET_ETHERSTATSPKTS64OCTETS_P0 + addrOffset_fromP0 ), 
			rtl8651_returnAsicCounter( OFFSET_ETHERSTATSPKTS65TO127OCTETS_P0 + addrOffset_fromP0 ), 
			rtl8651_returnAsicCounter( OFFSET_ETHERSTATSPKTS128TO255OCTETS_P0 + addrOffset_fromP0 ));
		rtlglue_printf("   256 - 511: %u pkts, 512 - 1023: %u pkts, 1024 - 1518: %u pkts\n", 
			rtl8651_returnAsicCounter( OFFSET_ETHERSTATSPKTS256TO511OCTETS_P0 + addrOffset_fromP0 ), 
			rtl8651_returnAsicCounter( OFFSET_ETHERSTATSPKTS512TO1023OCTETS_P0 + addrOffset_fromP0), 
			rtl8651_returnAsicCounter( OFFSET_ETHERSTATSPKTS1024TO1518OCTETS_P0 + addrOffset_fromP0 ) );
		rtlglue_printf("   oversize: %u pkts, Control unknown %u pkts, Pause %u pkts\n", 
			rtl8651_returnAsicCounter( OFFSET_ETHERSTATSOVERSIZEPKTS_P0 + addrOffset_fromP0 ),
			rtl8651_returnAsicCounter( OFFSET_DOT3CONTROLINUNKNOWNOPCODES_P0 + addrOffset_fromP0 ), 
			rtl8651_returnAsicCounter( OFFSET_DOT3INPAUSEFRAMES_P0 + addrOffset_fromP0 ));
		
		rtlglue_printf("Output counters\n");
		rtlglue_printf("   Snd %llu bytes, Unicast %u pkts, Multicast %u pkts\n",
			rtl865xC_returnAsicCounter64( OFFSET_IFOUTOCTETS_P0 + addrOffset_fromP0 ), 
			rtl8651_returnAsicCounter( OFFSET_IFOUTUCASTPKTS_P0 + addrOffset_fromP0 ),
			rtl8651_returnAsicCounter( OFFSET_IFOUTMULTICASTPKTS_P0 + addrOffset_fromP0 ));
		rtlglue_printf("   Broadcast %u pkts, Late collision %u, Deferred transmission %u \n",
			rtl8651_returnAsicCounter( OFFSET_IFOUTBROADCASTPKTS_P0 + addrOffset_fromP0 ),
			rtl8651_returnAsicCounter( OFFSET_DOT3STATSLATECOLLISIONS_P0 + addrOffset_fromP0 ), 
			rtl8651_returnAsicCounter( OFFSET_DOT3STATSDEFERREDTRANSMISSIONS_P0 + addrOffset_fromP0 ));
		rtlglue_printf("   Collisions %u Single collision %u Multiple collision %u pause %u\n",
			rtl8651_returnAsicCounter( OFFSET_ETHERSTATSCOLLISIONS_P0 + addrOffset_fromP0 ), 
			rtl8651_returnAsicCounter( OFFSET_DOT3STATSSINGLECOLLISIONFRAMES_P0 + addrOffset_fromP0 ), 
			rtl8651_returnAsicCounter( OFFSET_DOT3STATSMULTIPLECOLLISIONFRAMES_P0 + addrOffset_fromP0 ), 
			rtl8651_returnAsicCounter( OFFSET_DOT3OUTPAUSEFRAMES_P0 + addrOffset_fromP0 ));
		
	}

	rtlglue_printf("<Whole system counters>\n");
	rtlglue_printf("   CpuEvent %u pkts\n", rtl8651_returnAsicCounter(OFFSET_ETHERSTATSCPUEVENTPKT));

	return SUCCESS;
}

/*
@func int32 | rtl8651_clearAsicCounter | Clear specified ASIC counter to zero
@parm	uint32	|	counterIdx |  Specify the counter to clear
@rvalue SUCCESS | When counter index is valid, return SUCCESS
@rvalue FAILED | When counter index is invalid, return FAILED
@comm
	When specify a vlid counter, the corresponding counter will be reset to zero and read the counter once
	for guarantee following read can get correct value. 
*/
int32 rtl8651_clearAsicSpecifiedCounter(uint32 counterIdx) {

	uint32 reg;
	rtlglue_printf("attention!this function is obsolete, please use new api:rtl8651_resetAsicMIBCounter()  or rtl8651_clearAsicCounter()\n");
	return FAILED;
	switch(counterIdx) {
		case 0:
		reg = READ_MEM32(MIB_CONTROL);
		WRITE_MEM32(MIB_CONTROL, SYS_COUNTER_RESTART);
		WRITE_MEM32(MIB_CONTROL, reg);
		break;
	default:	
		rtlglue_printf("Not Comptable Counter Index  %d\n",counterIdx);
			return FAILED;//counter index out of range

	}
	_rtl8651_initialRead();
	return SUCCESS;
}

/*
@func int32 | rtl8651_resetAsicCounterMemberPort | Clear the specified counter value and its member port
@parm	uint32	|	counterIdx |  Specify the counter to clear
@rvalue SUCCESS | When counter index is valid, return SUCCESS
@rvalue FAILED | When counter index is invalid, return FAILED
@comm
	When specify a vlid counter, the member port of the specified counter will be cleared to null set. 
*/
int32 rtl8651_resetAsicCounterMemberPort(uint32 counterIdx){

	rtlglue_printf("attention!this function is obsolete, please use new api:rtl8651_resetAsicMIBCounter()  or rtl8651_clearAsicCounter()\n");
	return FAILED;
	switch(counterIdx) {
		case 0:
		WRITE_MEM32(MIB_CONTROL, 0x0);
		break;
		default:	
			rtlglue_printf("Not Comptable Counter Index  %d\n",counterIdx);
			return FAILED;//counter index out of range

	}
	_rtl8651_initialRead();
	return SUCCESS;
}

/*
@func int32 | rtl8651_addAsicCounterMemberPort | The specified counter value add the specified port port into counter monitor member
@parm	uint32	|	counterIdx |  Specify the counter to add member port
@parm	uint32	|	port |  The added member port
@rvalue SUCCESS | When counter index is valid, return SUCCESS
@rvalue FAILED | When counter index is invalid, return FAILED
@comm
	When specify a vlid counter and a valid port number, the specified port will be added to the counter coverage. 
*/
int32 rtl8651_addAsicCounterMemberPort(uint32 counterIdx, uint32 port) {
	uint32 reg, portMask;
	rtlglue_printf("attention!this function is obsolete, it shouldn't be used any more\n");
	return FAILED;

	
	if(port <RTL8651_PORT_NUMBER)
		portMask = 1<<(port + PORT_FOR_COUNTING_OFFSET);
	else if (port < RTL8651_PORT_NUMBER+rtl8651_totalExtPortNum)
		portMask = 1<<(port - RTL8651_PORT_NUMBER + EXT_PORT_FOR_COUNTING_OFFSET);
	else
		return FAILED;//Port number out of range
	switch(counterIdx) {
		case 0:
		reg = READ_MEM32(MIB_CONTROL) & 0x3FE00000;
		WRITE_MEM32(MIB_CONTROL, reg|portMask);
		break;
		default:	
			rtlglue_printf("Not Comptable Counter Index  %d\n",counterIdx);
			return FAILED;//counter index out of range
	}
	return SUCCESS;
}


/*
@func int32 | rtl8651_resetAsicCounterMemberPort | Clear the specified counter value and its member port
@parm	uint32	|	counterIdx |  Specify the counter to clear
@rvalue SUCCESS | When counter index is valid, return SUCCESS
@rvalue FAILED | When counter index is invalid, return FAILED
@comm
	When specify a vlid counter, the corresponding counter will be reset to zero, its member port will be cleared to null set and read the counter once
	for guarantee following read can get correct value. 
*/
int32 rtl8651_delAsicCounterMemberPort(uint32 counterIdx, uint32 port) {
	uint32 reg, portMask;
	rtlglue_printf("attention!this function is obsolete, it shouldn't be used any more\n");
	return FAILED;
	if(port <RTL8651_PORT_NUMBER)
		portMask = 1<<(port + PORT_FOR_COUNTING_OFFSET);
	else if (port < RTL8651_PORT_NUMBER+rtl8651_totalExtPortNum)
		portMask = 1<<(port - RTL8651_PORT_NUMBER + EXT_PORT_FOR_COUNTING_OFFSET);
	else
		return FAILED;//Port number out of range
	switch(counterIdx) {
		case 0:
		reg = READ_MEM32(MIB_CONTROL) & 0x3FE00000;
		WRITE_MEM32(MIB_CONTROL, reg&~portMask);
		break;
		default:	
			rtlglue_printf("Not Comptable Counter Index  %d\n",counterIdx);
			return FAILED;//counter index out of range
	}
	return SUCCESS;
}


/*
@func int32 | rtl8651_resetAsicCounterMemberPort | Clear the specified counter value and its member port
@parm	uint32	|	counterIdx |  Specify the counter to clear
@rvalue SUCCESS | When counter index is valid, return SUCCESS
@rvalue FAILED | When counter index is invalid, return FAILED
@comm
	When specify a vlid counter, the corresponding counter will be reset to zero, its member port will be cleared to null set and read the counter once
	for guarantee following read can get correct value. 
*/
int32 rtl8651_getAsicCounter(uint32 counterIdx, rtl865x_tblAsicDrv_basicCounterParam_t * basicCounter) {

	rtlglue_printf("attention!this function is obsolete, please use new api:rtl8651_getSimpleAsicMIBCounter()  or rtl8651_getAdvancedMIBCounter()\n");
	return FAILED;
	_rtl8651_initialRead();
	switch(counterIdx) {
		case 0:
				basicCounter->rxBytes = READ_MEM32(MIB_COUNTER_BASE);
				basicCounter->rxPackets = READ_MEM32(MIB_COUNTER_BASE+0x14) +	//Unicast
						READ_MEM32(MIB_COUNTER_BASE+0x18) + 	//Multicast
						READ_MEM32(MIB_COUNTER_BASE+0x1c);	//Broadcast
				basicCounter->rxErrors = READ_MEM32(MIB_COUNTER_BASE+0x8) +	//CRC error and Alignment error
						READ_MEM32(MIB_COUNTER_BASE+0xc) +	//Fragment error
						READ_MEM32(MIB_COUNTER_BASE+010);	//Jabber error
				basicCounter->drops = READ_MEM32(MIB_COUNTER_BASE+0x4);
				basicCounter->cpus = READ_MEM32(MIB_COUNTER_BASE+0x74);
				basicCounter->txBytes = READ_MEM32(MIB_COUNTER_BASE+0x48);
				basicCounter->txPackets = READ_MEM32(MIB_COUNTER_BASE+0x4c) +	//Unicast
						READ_MEM32(MIB_COUNTER_BASE+0x50) +	//Multicast
						READ_MEM32(MIB_COUNTER_BASE+0x54);	//Broadcast
				/*
				basicCounter->mbr = (READ_MEM32(MIB_CONTROL)&PORT_FOR_COUNTING_MASK)>>PORT_FOR_COUNTING_OFFSET | 
								((READ_MEM32(MIB_CONTROL)&EXT_PORT_FOR_COUNTING_MASK)>>EXT_PORT_FOR_COUNTING_OFFSET)<<6;
				*/
		break;
		default:
			rtlglue_printf("Not Comptable Counter Index  %d\n",counterIdx);
			return FAILED;//counter index out of range

	}
	return SUCCESS;
}

/*=========================================
  * ASIC DRIVER API: LINK STATUS
  *=========================================*/
#define RTL865X_ASIC_DRIVER_LINK_STATUS_API

void rtl8651_updateLinkChangePendingCount(void)
{
	linkChangePendingCount++;
}

static uint32 _rtl8651_findAsicLinkupPortmask(uint32 portMask)
{
	uint32 port, lnkUp_portMask = portMask;
	for ( port = 0; port < RTL8651_PORT_NUMBER + rtl8651_totalExtPortNum; port++ )
	{
		if ( ( portMask & ( 1 << port ) ) && (rtl8651AsicEthernetTable[port].linkUp == FALSE) )
		{
			lnkUp_portMask &= ~( 1 << port );
		}
	}
	return lnkUp_portMask;
}

/*
	Re-arrange configuration for port update.
	Parameter "updateMacMask" is used to indicates the port update which is need to take care.
	if "updateMacMask" == 0, we always arrange ALL ports.
*/
static int32 _rtl8651_rearrangeAsicEthernetMacLinkStatus( uint32 updateMacMask )
{
	int macIdx;
	uint32 offset;
	static _rtl8651_tblAsic_psr_t _rtl8651_swPsr[RTL8651_MAC_NUMBER] = {{0}};

	RTL_ASIC_TRACE_IN(	RTL_ASIC_MSG_LINKCTRL,
						"Start to Rearrange MAC Link status");

	if (!RTL865X_TX_LINKDOWN_BUG)
		return SUCCESS;
	/*
		We would need to do arrangement for each MAC.
	*/
	for ( macIdx = 0 ; macIdx < RTL8651_MAC_NUMBER ; macIdx ++ )
	{
		uint32 pcr;			/* snapshot of port control register of the specific MAC */
		uint32 psr;			/* snapshot of port status register of the specific MAC */
		uint32 phyId;			/* PHYID of the specific MAC */
		uint32 isForcedMode;	/* is set to force mode or not */
		uint32 phyIsLinkUp;	/* Link status in PHY using MDCMDIO */
		uint32 pcrIsLinkUp;	/* Link status set in PCR */
		uint32 psrIsLinkUp;	/* Link status set in PSR */
		uint32 macIsLinkUp;	/* Link status in MAC : Force Mode [PCR], non-Force Mode [PSR] */
		uint32 procCase;

		if (	(updateMacMask) &&
			((updateMacMask & (1 << macIdx)) == 0))
		{
			/* It means this MAC is not be updated : ingore it. */
			continue;
		}

		offset = macIdx * 4;
		pcr = READ_MEM32( PCRP0 + offset );

		do
		{
			psr = READ_MEM32( PSRP0 + offset );
		} while ((psr & (psr & PortStatusLinkSpeed_MASK) >> PortStatusLinkSpeed_OFFSET) == PortStatusLinkSpeedReserved);
		phyId = (pcr & ExtPHYID_MASK) >> ExtPHYID_OFFSET;
		isForcedMode = ( pcr & EnForceMode )?TRUE:FALSE;
		pcrIsLinkUp = ( pcr & ForceLink )?TRUE:FALSE;
		psrIsLinkUp = ( psr & PortStatusLinkUp )?TRUE:FALSE;
		macIsLinkUp = ( isForcedMode == TRUE )?pcrIsLinkUp:psrIsLinkUp;

		if((phyId==0) && (macIdx!=0))
		{
			goto nextMac;
		}
		
		{	/* Force mode */
			uint32 mii_bmcr;		/* Basic Mode Control Register : MII register */
			uint32 mii_bmsr;		/* Basic Mode Status Register : MII register */

			/* Check if MII Register is ok or not */
			rtl8651_getAsicEthernetPHYReg(phyId, 0x00, &mii_bmcr);
			if ( mii_bmcr == 0xFFFF /* Impossible : Skip its configuration */ )
			{
				goto nextMac;
			}

			rtl8651_getAsicEthernetPHYReg(phyId, 0x01, &mii_bmsr);
			if (mii_bmsr == 0xFFFF  /* Impossible : Skip its configuration */ )
			{
				goto nextMac;
			}
			phyIsLinkUp = (mii_bmsr & 0x04 /* bit 2: Link Status */)?TRUE:FALSE;
		}

		/* ==============================================
				To process RTL865xC Link-down-congestion problem

					=> When port become Link-down from Link-up,
					page leakage would happen for non-forced mode.
					So we need to:
						1. Enable Force mode when port is Linked-up
						2. Control port TX when port is Linked-down
						3. Set port status back to original mode AFTER
						    port is linked-down and no more TXed pages
						    for that port.
		     ============================================== */

		RTL_ASIC_DEBUG(	RTL_ASIC_MSG_LINKCTRL,
						"MAC [%d] - PHYID(%d) : Force [%s] MAC-Lnk[%s] PHY-Lnk[%s]",
						macIdx,
						phyId,
						(isForcedMode == TRUE)?"F":"N",
						(macIsLinkUp == TRUE)?"U":"D",
						(phyIsLinkUp == TRUE)?"U":"D");

		/*
			Process for the ASIC problem:

				{isForcedMode, macIsLinkUp, phyIsLinkUp}		Status					To-do
				---------------------------------------------------------------------------------------------
				{0,0,0}									Link down				N/A
				{0,0,1}									Impossible				Error
				{0,1,0}									Impossible				Error
				{0,1,1}									Link up					Change to FORCE Mode
				{1,0,0}									Link down (User cfg)		N/A
				{1,0,1}									Link up (User cfg)			N/A
				{1,1,0}									Link down				Back to AUTO Mode if we modified it before
				{1,1,1}									Link up					N/A
		*/
		#define _PROCCASE_3BIT_VAR(first, second, third)				\
					(	(((first) == TRUE)?		(1 << 2):(0 << 2)) |	\
					(	((second) == TRUE)?		(1 << 1):(0 << 1)) |	\
					(	((third) == TRUE)?		(1 << 0):(0 << 0))		)

		procCase = _PROCCASE_3BIT_VAR(isForcedMode, macIsLinkUp, phyIsLinkUp);

		switch ( procCase )
		{
			case _PROCCASE_3BIT_VAR(FALSE, FALSE, FALSE):	/* {0, 0, 0} */
				/*
					The port is set to non-FORCE mode, but both MDCMDIO
					and PCR are set to LINK DOWN.
				*/

				/* To-do : N/A */
				break;
			case _PROCCASE_3BIT_VAR(FALSE, FALSE, TRUE):	/* {0, 0, 1} */
				/*
					The port is set to AUTO mode, but the result from MDCMDIO and PCR is not same.
				*/
				RTL_ASIC_ERR(	RTL_ASIC_MSG_LINKCTRL,
								"MAC [%d] - PHYID(%d) PCR(0x%x) PSR(0x%x) : Auto-Mode is set but Link status is mis-match",
								macIdx,
								phyId,
								pcr,
								psr);

				/* To-do : N/A */
				break;
			case _PROCCASE_3BIT_VAR(FALSE, TRUE, FALSE):	/* {0, 1, 0} */
				/*
					The port is set to AUTO mode, but the result from MDCMDIO and PCR is not same.
				*/
				RTL_ASIC_ERR(	RTL_ASIC_MSG_LINKCTRL,
								"MAC [%d] - PHYID(%d) PCR(0x%x) PSR(0x%x) : Auto-Mode is set but Link status is mis-match",
								macIdx,
								phyId,
								pcr,
								psr);

				/* To-do : N/A */
				break;
			case _PROCCASE_3BIT_VAR(FALSE, TRUE, TRUE):		/* {0, 1, 1} */
				/*
					Link up and is non-forced mode :
						1. Record its original ability and original result.
						2. Become force mode.
				*/

				RTL_ASIC_INFO(	RTL_ASIC_MSG_LINKCTRL,
									"MAC [%d] : Link-UP : Change to [FORCE-MODE]",
									macIdx);

				/* Record the original ability */
				_rtl8651_swPsr[macIdx].valid = TRUE;
				_rtl8651_swPsr[macIdx].nwayEnable = (psr & PortStatusNWayEnable)?TRUE:FALSE;
				_rtl8651_swPsr[macIdx].rxPauseAbility = (psr & PortStatusRXPAUSE)?TRUE:FALSE;
				_rtl8651_swPsr[macIdx].txPauseAbility = (psr & PortStatusTXPAUSE)?TRUE:FALSE;
				_rtl8651_swPsr[macIdx].linkUp = (psr & PortStatusLinkUp)?TRUE:FALSE;
				_rtl8651_swPsr[macIdx].duplex = (psr & PortStatusDuplex)?TRUE:FALSE;
				_rtl8651_swPsr[macIdx].linkSpeed = (psr & PortStatusLinkSpeed_MASK) >> PortStatusLinkSpeed_OFFSET;

				if (	(_rtl8651_swPsr[macIdx].linkSpeed != PortStatusLinkSpeed1000M) &&
					(_rtl8651_swPsr[macIdx].linkSpeed != PortStatusLinkSpeed100M) &&
					(_rtl8651_swPsr[macIdx].linkSpeed != PortStatusLinkSpeed10M)	)
				{
					RTL_ASIC_ERR(	RTL_ASIC_MSG_LINKCTRL,
									"Error Link Speed (0x%x) for MAC [%d]\n",
									_rtl8651_swPsr[macIdx].linkSpeed,
									macIdx);
				}

				/* Check mode */
				{
					uint32 newPcr;

					newPcr = pcr;	/* Store original PCR */

					/* Set Linkup bit */
					newPcr = 	(newPcr & ~(ForceLink)) |
								(( _rtl8651_swPsr[macIdx].linkUp == TRUE )?(ForceLink):(0));
					/* Set duplex bit */
					newPcr = 	(newPcr & ~(ForceDuplex)) |
								(( _rtl8651_swPsr[macIdx].duplex == TRUE )?(ForceDuplex):(0));
					/* Set speed bits */
					newPcr = 	(newPcr & ~(ForceSpeedMask)) |
								(	(_rtl8651_swPsr[macIdx].linkSpeed == PortStatusLinkSpeed1000M)?ForceSpeed1000M:
									(_rtl8651_swPsr[macIdx].linkSpeed == PortStatusLinkSpeed100M)?ForceSpeed100M:
									(_rtl8651_swPsr[macIdx].linkSpeed == PortStatusLinkSpeed10M)?ForceSpeed10M:
									0	);

					/* set PAUSE FRAME capability */
					newPcr = (newPcr & ~(PauseFlowControl_MASK)) |
									( ((_rtl8651_swPsr[macIdx].txPauseAbility == TRUE)?(PauseFlowControlEtxDrx):(0)) |
									((_rtl8651_swPsr[macIdx].rxPauseAbility == TRUE)?(PauseFlowControlDtxErx):(0)) );
										
					RTL_ASIC_INFO(	RTL_ASIC_MSG_LINKCTRL,
										"MAC [%d] : Link status : Link status (%s) Duplex (%s) Speed (%s) FC (%s)",
										macIdx,
										( _rtl8651_swPsr[macIdx].linkUp == TRUE )?"Up":"Down",
										( _rtl8651_swPsr[macIdx].duplex == TRUE )?"FULL":"HALF",
										(	( _rtl8651_swPsr[macIdx].linkSpeed == PortStatusLinkSpeed1000M )?	"1000M":
											( _rtl8651_swPsr[macIdx].linkSpeed == PortStatusLinkSpeed100M )?	"100M":
											( _rtl8651_swPsr[macIdx].linkSpeed == PortStatusLinkSpeed10M )?		"10M":"N/A"	),
										((_rtl8651_swPsr[macIdx].txPauseAbility == TRUE)?
										 ((_rtl8651_swPsr[macIdx].rxPauseAbility == TRUE)? "Rx and Tx Enable":"Rx Disable, Tx Enable"):	
										((_rtl8651_swPsr[macIdx].rxPauseAbility == FALSE)? "Rx and Tx Disable":"Rx Enable, Tx Disable"))
										);

					/* Set to force mode */
					newPcr |= EnForceMode;

					WRITE_MEM32( PCRP0 + offset, newPcr );
				}

				break;
			case _PROCCASE_3BIT_VAR(TRUE, FALSE, FALSE):	/* {1, 0, 0} */
				/*
					The port is set to FORCE mode, but both PSR and MDCMDIO indicate this port is Link-down.
				*/

				/* N/A */

				break;
			case _PROCCASE_3BIT_VAR(TRUE, FALSE, TRUE):		/* {1, 0, 1} */
				/*
					The port is set to FORCE mode, PSR indicates this port is Link-down but MDCMDIO indicates this port is
					linked up.
					But we don't need to do anything for this event:
					===================================================
					We only change the PCR to force mode for Port from LinkUP to LinkDOWN.
					If port is originally linked DOWN, it's FORCE mode is not configured by us.

					In other words, [ the FORCE mode here is not set by us, so we don't change it ].
				*/
				RTL_ASIC_WARN(	RTL_ASIC_MSG_LINKCTRL,
									"MAC [%d] : It is link-up, but the status is linked-up before : Do nothing",
									macIdx);

				/* N/A */

				break;
			case _PROCCASE_3BIT_VAR(TRUE, TRUE, FALSE):		/* {1, 1, 0} */
				/*
					The port is set to FORCE mode, and PSR indicates this port is Link-up. But currently
					MDCMDIO report indicates this port is now linked down.
					=> We would change the mode back to auto-mode if we ever changed it before.
				*/
				if ( _rtl8651_swPsr[macIdx].valid == TRUE )
				{
					uint32 newPcr;
					uint32 org_DOT1XPORTCR;
					uint32 remCount;

					RTL_ASIC_INFO(	RTL_ASIC_MSG_LINKCTRL,
										"MAC [%d] : Link-DOWN : Do post-process and change to [AUTO-MODE] back",
										macIdx);

					/* We Lock the packet TX to this MAC first */

					RTL_ASIC_INFO(	RTL_ASIC_MSG_LINKCTRL,
										"=> Port-based 1X to Lock packet TX");

					/* => Currently, we use port-based 802.1X to stop packet TX before actually let it link-down */
					org_DOT1XPORTCR = READ_MEM32(DOT1XPORTCR);

					WRITE_MEM32(	DOT1XPORTCR,
									(	org_DOT1XPORTCR |
										Dot1XPB_POPDIR(macIdx, DOT1X_DIR_BOTH) |
										Dot1XPB_PAuth(macIdx, DOT1X_UNAUTH) |
										Dot1XPB_PEn(macIdx, DOT1X_ENABLE)	)		);

					RTL_ASIC_INFO(	RTL_ASIC_MSG_LINKCTRL,
										"=> Wait until no-more packet pending.");

					/* Check output queue : if one of DSCR0/1/2 is not 0, it means there are some packets need TX */
					remCount = 5000;
					while ((	READ_MEM32(Pn_DSCR0(macIdx)) ||
							READ_MEM32(Pn_DSCR1(macIdx)) ||
							READ_MEM32(Pn_DSCR2(macIdx))		) &&
							((remCount --) > 0)		)
					{
						REG32(WDTCNR)|=WDTCLR;
					}
					if ( remCount == 0 )
					{
						RTL_ASIC_WARN(	RTL_ASIC_MSG_LINKCTRL,
											"Time-out : Link down before clear all TXed packet.");
					}

					RTL_ASIC_INFO(	RTL_ASIC_MSG_LINKCTRL,
									"=> Actually change status back to [AUTO-MODE].");

					newPcr = pcr;	/* Store original PCR */

					/* Set Linkup bit */
					newPcr = 	(newPcr & ~(ForceLink));
					/* Set duplex bit */
					newPcr = 	(newPcr & ~(ForceDuplex));
					/* Set speed bits */
					newPcr = 	(newPcr & ~(ForceSpeedMask));
					/* set PAUSE FRAME capability */
					newPcr = 	(newPcr & ~(PauseFlowControl_MASK));
 					/* Set to non-force mode */
					newPcr =		(newPcr & ~(EnForceMode));

					WRITE_MEM32( PCRP0 + offset, newPcr );

					_rtl8651_swPsr[macIdx].valid = FALSE;		/* Config back to FALSE */

					/* Let DOT1X setting back to original one */

					RTL_ASIC_INFO(	RTL_ASIC_MSG_LINKCTRL,
										"=> Restore original Port-Based 1X setting.");

					WRITE_MEM32(DOT1XPORTCR, org_DOT1XPORTCR);


				}
				break;
			case _PROCCASE_3BIT_VAR(TRUE, TRUE, TRUE):		/* {1, 1, 1} */
				/*
					The port is set to FORCE mode, and both PSR and MDCMDIO indicate this port is Link-up.
					=> Do nothing.
				*/

				/* N/A */
				break;
 
		}
nextMac:
		continue;
	}

	RTL_ASIC_TRACE_IN(	RTL_ASIC_MSG_LINKCTRL,
						"Done.");

	return SUCCESS;
}

static int32 _rtl8651_macLinkStatus(uint32 macIdx)
{
	uint32 pcr;			/* snapshot of port control register of the specific MAC */
	uint32 psr;			/* snapshot of port status register of the specific MAC */
	uint32 phyId;			/* PHYID of the specific MAC */
	uint32 isForcedMode;	/* is set to force mode or not */
	uint32 isLinkUp;		/* Link status */
	uint32 offset;

	if (!RTL865X_TX_LINKDOWN_BUG)
		return SUCCESS;
	
	offset = macIdx * 4;
	pcr = READ_MEM32( PCRP0 + offset );
	psr = READ_MEM32( PSRP0 + macIdx * 4 );

	phyId = (pcr & ExtPHYID_MASK) >> ExtPHYID_OFFSET;
	isForcedMode = ( pcr & EnForceMode )?TRUE:FALSE;

	if ( isForcedMode == TRUE )
	{	/* Force mode */
		uint32 mii_bmcr;		/* Basic Mode Control Register : MII register */
		uint32 mii_bmsr;		/* Basic Mode Status Register : MII register */

		/* Check if MII Register is ok or not */
		rtl8651_getAsicEthernetPHYReg(phyId, 0x00, &mii_bmcr);
		if ( mii_bmcr == 0xFFFF /* Impossible : Only get information from PSR */ )
		{
			goto getLinkUpFromPsr;
		}

		rtl8651_getAsicEthernetPHYReg(phyId, 0x01, &mii_bmsr);
		if (mii_bmsr == 0xFFFF  /* Impossible : Only get information from PSR */ )
		{
			goto getLinkUpFromPsr;
		}
		isLinkUp = (mii_bmsr & 0x04 /* bit 2: Link Status */)?TRUE:FALSE;

		RTL_ASIC_DEBUG(	RTL_ASIC_MSG_LINKCTRL,
							"MAC [%d] : get link status from MDCMDIO (PHYID: %d) : < Link %s >",
							macIdx,
							phyId,
							(isLinkUp == TRUE)?"Up":"Down");


	} else
	{	/* non-force mode */
getLinkUpFromPsr:
		isLinkUp = ( psr & PortStatusLinkUp )?TRUE:FALSE;

		RTL_ASIC_DEBUG(	RTL_ASIC_MSG_LINKCTRL,
							"MAC [%d] : get link status from Port-Status-Register : < Link %s >",
							macIdx,
							(isLinkUp == TRUE)?"Up":"Down");
	}

	return isLinkUp;
}

static void (*linkChangeNotifier)(uint32, int8) = NULL;

int32 rtl8651_regLinkChangeCallBackFun(void (*callBackFun)(uint32, int8))
{
	RTL_ASIC_INFO(	RTL_ASIC_MSG_LINKCTRL,
					"Register Callback function [%p] for ASIC Link change.",
					callBackFun);

	linkChangeNotifier = callBackFun;
	return SUCCESS;
}

int32 rtl8651_setAsicEthernetLinkStatus(uint32 port, int8 linkUp)
{
	int8 notify;
	uint32 portmask;

	if (port >= (RTL8651_PORT_NUMBER+rtl8651_totalExtPortNum))
	{
		return FAILED;
	}

	notify = (rtl8651AsicEthernetTable[port].linkUp != ((linkUp==TRUE)? TRUE: FALSE))?TRUE:FALSE;

	RTL_ASIC_INFO(	RTL_ASIC_MSG_LINKCTRL,
					"Port %d Link status update : ORG [%s] NEW [%s] : Need notify [%s]",
					port,
					(rtl8651AsicEthernetTable[port].linkUp == TRUE)?"U":"D",
					(linkUp == TRUE)?"U":"D",
					(notify == TRUE)?"YES":"NO");

	rtl8651AsicEthernetTable[port].linkUp = (linkUp == TRUE)? TRUE: FALSE;

	/* 
		If the specified port is a member of the aggregator, 
		update the table which maps hashed index to the port
		because the member port of the aggregator link changes.
	*/
	portmask = READ_MEM32( LAGCR0 ) & TRUNKMASK_MASK;
	if ( portmask & ( 1 << port ) )
	{
		/* Find the link-up portmask */
		uint32 lnkUp_portMask = _rtl8651_findAsicLinkupPortmask(portmask);
		rtl8651_updateAsicLinkAggregatorLMPR( lnkUp_portMask );
	}

	if (notify == TRUE)
	{
		/*
		 * Hard code to notify upper layer is an ugly way. Recommend to use callback function
		 * registration to achieve link-change notification.
		 */

		RTL_ASIC_DEBUG(	RTL_ASIC_MSG_LINKCTRL,
							"Upper-layer callBack function : %p",
							linkChangeNotifier);



		if (linkChangeNotifier)
		{
			RTL_ASIC_INFO(	RTL_ASIC_MSG_LINKCTRL,
							"MAC [%d] link change : notify upper-layer by calling Call-back function [%p]",
							port,
							linkChangeNotifier);

			linkChangeNotifier(port, linkUp);
		}
	}

	return SUCCESS;
}

int32 rtl8651_getAsicEthernetLinkStatus(uint32 port, int8 *linkUp)
{
	if(	(port >= (RTL8651_PORT_NUMBER+rtl8651_totalExtPortNum)) ||
		(linkUp == NULL))
	{
		return FAILED;
	}

	*linkUp = rtl8651AsicEthernetTable[port].linkUp;

	return SUCCESS;
}

static inline void rtl8651_procLinkUp(uint32 currentLinkUpStatus[])
{
	uint32 macIdx;
	uint32 updateMacMask;
	/*
		Batch process for LINK up events : NOTE that the sequence of the process CAN NOT be changed.
	*/

	/* ==========================================================
		Record the Link UP port mask
	    ========================================================== */
	updateMacMask = 0;
	for ( macIdx = 0 ; macIdx < RTL8651_MAC_NUMBER ; macIdx ++ )
	{
		/* Only process LINK up events */
		if ( currentLinkUpStatus[macIdx] == TRUE )
		{
			if ( currentLinkUpStatus[macIdx] != rtl8651AsicEthernetTable[macIdx].linkUp )
			{
				updateMacMask |= (1 << macIdx);
			}
		}
	}

	/* ==========================================================
		Pre-process for LINK UP
	    ========================================================== */
	/* Re-arrange */
	_rtl8651_rearrangeAsicEthernetMacLinkStatus(updateMacMask);

	/* ==========================================================
		Link status update and upper layer call-back
	    ========================================================== */

	for ( macIdx = 0 ; macIdx < RTL8651_MAC_NUMBER ; macIdx ++ )
	{
		/* Only process LINK up events */
		if ( currentLinkUpStatus[macIdx] == TRUE )
		{
			if ( currentLinkUpStatus[macIdx] != rtl8651AsicEthernetTable[macIdx].linkUp )
			{
				rtl8651_setAsicEthernetLinkStatus( macIdx, currentLinkUpStatus[macIdx] );
			}
		}
	}

	/* ==========================================================
		Post-process for LINK UP
	    ========================================================== */

}

static inline void rtl8651_procLinkDown(uint32 currentLinkUpStatus[])
{
	uint32 macIdx;
	uint32 updateMacMask;

	/*
		Batch process for LINK down events : NOTE that the sequence of the process CAN NOT be changed.
	*/
	/* ==========================================================
		Record the Link UP port mask
	    ========================================================== */
	updateMacMask = 0;

	for ( macIdx = 0 ; macIdx < RTL8651_MAC_NUMBER ; macIdx ++ )
	{
		/* Only process LINK down events */
		if ( currentLinkUpStatus[macIdx] == FALSE )
		{
			if ( currentLinkUpStatus[macIdx] != rtl8651AsicEthernetTable[macIdx].linkUp )
			{
				updateMacMask |= (1 << macIdx);
			}
		}
	}

	/* ==========================================================
		Pre-process for LINK DOWN
	    ========================================================== */


	/* ==========================================================
		Link status update and upper layer call-back
	    ========================================================== */
	for ( macIdx = 0 ; macIdx < RTL8651_MAC_NUMBER ; macIdx ++ )
	{
		/* Only process LINK down events */
		if ( currentLinkUpStatus[macIdx] == FALSE )
		{
			if ( currentLinkUpStatus[macIdx] != rtl8651AsicEthernetTable[macIdx].linkUp )
			{
				rtl8651_setAsicEthernetLinkStatus( macIdx, currentLinkUpStatus[macIdx] );
			}
		}
	}

	/* ==========================================================
		Post-process for LINK DOWN
	    ========================================================== */
	/* Re-arrange */
	_rtl8651_rearrangeAsicEthernetMacLinkStatus(updateMacMask);
}

/* Sync Link status to ASIC driver for all ports */
void rtl8651_syncLinkStatus( void )
{
	uint32 currentLinkUpStatus[RTL8651_MAC_NUMBER];
	uint32 macIdx;
	uint32 needProcessLinkDown;
	uint32 needProcessLinkUp;

	if (!RTL865X_TX_LINKDOWN_BUG)
		return;
	
	needProcessLinkDown = FALSE;
	needProcessLinkUp = FALSE;

	/* (1) Check each port to collect current Link status */
	for ( macIdx = 0 ; macIdx < RTL8651_MAC_NUMBER ; macIdx ++ )
	{
		int32 isLinkUp;		/* Link status */

		isLinkUp = _rtl8651_macLinkStatus( macIdx );

		currentLinkUpStatus[macIdx] = isLinkUp;
	}

	/* (2) Check if there is any Link down or Link up events */
	for ( macIdx = 0; macIdx < RTL8651_MAC_NUMBER; macIdx ++ )
	{
		if ( currentLinkUpStatus[macIdx] != rtl8651AsicEthernetTable[macIdx].linkUp )
		{
			if ( currentLinkUpStatus[macIdx] == TRUE )
			{
				/* It means there is least 1 Link-UP status need to process */
				needProcessLinkUp = TRUE;
			} else
			{
				/* It means there is least 1 Link-DOWN status need to process */
				needProcessLinkDown = TRUE;
			}
		}
	}

	/*
		(3) Batch process for LINK-UP events and LINK-DOWN events.
	*/

	if ( needProcessLinkDown == TRUE )
	{
		rtl8651_procLinkDown(currentLinkUpStatus);
	}

	if ( needProcessLinkUp == TRUE )
	{
		rtl8651_procLinkUp(currentLinkUpStatus);
	}

	return;
}

int32 rtl8651_updateLinkStatus(void) 
{
	if ( linkChangePendingCount > 0 )	/* 'linkChangePendingCount' will be incremented when link change happens. */
	{
		linkChangePendingCount = 0;

		if (!RTL865X_TX_LINKDOWN_BUG)
		{
			{
				uint32 i, status;

				{
					/* Check each port. */
					for ( i = 0; i < RTL8651_MAC_NUMBER; i++ )
					{
						/* Read Port Status Register to know the port is link-up or link-down. */
						status = READ_MEM32( PSRP0 + i * 4 );
						if ( ( status & PortStatusLinkUp ) == FALSE )
						{
							/* Link is down. */
							rtl8651_setAsicEthernetLinkStatus( i, FALSE );
						}
						else
						{
							/* Link is up. */
							rtl8651_setAsicEthernetLinkStatus( i, TRUE );
						}
					}

					return SUCCESS;
				}

				return FAILED;	/* No link change. */

			}
		}
		else
		{
			rtl8651_syncLinkStatus();
		}
		return SUCCESS;
	}

	return FAILED;	/* No link change. */

}

int32 rtl865xC_setAsicEthernetForceModeRegs(uint32 port, uint32 enForceMode, uint32 forceLink, uint32 forceSpeed, uint32 forceDuplex)
{
	uint32 offset = port * 4;
	uint32 PCR = READ_MEM32( PCRP0 + offset );
	
	if (rtl8651_tblAsicDrvPara.externalPHYProperty & RTL8651_TBLASIC_EXTPHYPROPERTY_PORT5_RTL8211B)
	{
		if ( port > RTL8651_MAC_NUMBER )
		{
			return FAILED;
		}
	} else
	{
		if ( port > RTL8651_PHY_NUMBER )
		{
			return FAILED;
		}
	}

	PCR &= ~EnForceMode;
	PCR &= ~ForceLink;
	PCR &= ~ForceSpeedMask;
	PCR &= ~ForceDuplex;

	if ( enForceMode )
	{
		PCR |= EnForceMode;

		if ( forceLink )
			PCR |= ForceLink;

		if ( forceSpeed == 2 )
			PCR |= ForceSpeed1000M;
		else if ( forceSpeed == 1 )
			PCR |= ForceSpeed100M;
		else
			PCR |= ForceSpeed10M;

		if ( forceDuplex )
			PCR |= ForceDuplex;
	}
	
	WRITE_MEM32( PCRP0 + offset, PCR );
	return SUCCESS;

}

int32 rtl865xC_setAsicPortPauseFlowControl(uint32 port, uint8 rxEn, uint8 txEn)
{
	uint32 offset = port<<2;
	uint32 pauseFC = 0;

	if(rxEn!=0)
		pauseFC |= PauseFlowControlDtxErx;
	if(txEn!=0)
		pauseFC |= PauseFlowControlEtxDrx;
	
	WRITE_MEM32(PCRP0+offset, (~(PauseFlowControl_MASK)&(READ_MEM32(PCRP0+offset)))|pauseFC);

	return SUCCESS;
}

int32 rtl865xC_getAsicPortPauseFlowControl(uint32 port, uint8 *rxEn, uint8 *txEn)
{
	uint32 offset = port<<2;
	uint32 pauseFC = 0;
	
	pauseFC = ((PauseFlowControl_MASK)&(READ_MEM32(PCRP0+offset)));

	if (pauseFC&PauseFlowControlDtxErx)
		*rxEn = TRUE;

	if (pauseFC&PauseFlowControlEtxDrx)
		*txEn = TRUE;

	return SUCCESS;
}
/*=========================================
  * ASIC DRIVER API: SYSTEM INIT
  *=========================================*/
#define RTL865X_ASIC_DRIVER_SYSTEM_INIT_API

/*
@func void | rtl8651_clearRegister | Clear ALL registers in ASIC
@comm
	Clear ALL registers in ASIC.
	for RTL865xC only
*/

void rtl8651_clearRegister(void)
{
#ifdef CONFIG_RTL865XC
	int i = 0;

	WRITE_MEM32(MACCR,LONG_TXE);	
	WRITE_MEM32(MGFCR_E0R0,0);
	WRITE_MEM32(MGFCR_E0R1,0);
	WRITE_MEM32(MGFCR_E0R2,0);
	WRITE_MEM32(MGFCR_E1R0,0);
	WRITE_MEM32(MGFCR_E1R1,0);
	WRITE_MEM32(MGFCR_E1R2,0);	
	WRITE_MEM32(MGFCR_E2R0,0);
	WRITE_MEM32(MGFCR_E2R1,0);
	WRITE_MEM32(MGFCR_E2R2,0);
	WRITE_MEM32(MGFCR_E3R0,0);
	WRITE_MEM32(MGFCR_E3R1,0);
	WRITE_MEM32(MGFCR_E3R2,0);	
	WRITE_MEM32(VCR0,0);		
	WRITE_MEM32(VCR1,0);	
	WRITE_MEM32(PVCR0,0);	
	WRITE_MEM32(PVCR1,0);	
	WRITE_MEM32(PVCR2,0);	
	WRITE_MEM32(PVCR3,0);	
	WRITE_MEM32(PVCR4,0);	
	WRITE_MEM32(TEACR,0);
	WRITE_MEM32(FFCR,0);
	WRITE_MEM32(DOT1XPORTCR,0);
	WRITE_MEM32(DOT1XMACCR,0);
	WRITE_MEM32(GVGCR,0);
	WRITE_MEM32(SWTCR0,0);
	WRITE_MEM32(SWTCR1,0);
	WRITE_MEM32(PLITIMR,0);
	/* Set all Protocol-Based Reg. to 0 */
	WRITE_MEM32(PBVCR0,  0x00000000);
	WRITE_MEM32(PBVCR1, 0x00000000);
	WRITE_MEM32(PBVR0_0, 0x00000000);	/* IPX */
	WRITE_MEM32(PBVR0_1, 0x00000000);	/* IPX */
	WRITE_MEM32(PBVR0_2, 0x00000000);	/* IPX */
	WRITE_MEM32(PBVR0_3, 0x00000000);	/* IPX */
	WRITE_MEM32(PBVR0_4, 0x00000000);	/* IPX */		
	WRITE_MEM32(PBVR1_0,  0x00000000);	/* NetBIOS */
	WRITE_MEM32(PBVR1_1,  0x00000000);	/* NetBIOS */
	WRITE_MEM32(PBVR1_2,  0x00000000);	/* NetBIOS */
	WRITE_MEM32(PBVR1_3,  0x00000000);	/* NetBIOS */
	WRITE_MEM32(PBVR1_4,  0x00000000);	/* NetBIOS */
	WRITE_MEM32(PBVR2_0,  0x00000000);	/* PPPoE Control */
	WRITE_MEM32(PBVR2_1,  0x00000000);	
	WRITE_MEM32(PBVR2_2,  0x00000000);	
	WRITE_MEM32(PBVR2_3,  0x00000000);	
	WRITE_MEM32(PBVR2_4,  0x00000000);	
	WRITE_MEM32(PBVR3_0,  0x00000000);	/* PPPoE Session */
	WRITE_MEM32(PBVR3_1,  0x00000000);	/* PPPoE Session */
	WRITE_MEM32(PBVR3_2,  0x00000000);	/* PPPoE Session */
	WRITE_MEM32(PBVR3_3,  0x00000000);	/* PPPoE Session */
	WRITE_MEM32(PBVR3_4,  0x00000000);	/* PPPoE Session */
	WRITE_MEM32(PBVR4_0,  0x00000000);	/* User-defined 1 */
	WRITE_MEM32(PBVR4_1,  0x00000000);	/* User-defined 1 */
	WRITE_MEM32(PBVR4_2,  0x00000000);	/* User-defined 1 */
	WRITE_MEM32(PBVR4_3,  0x00000000);	/* User-defined 1 */
	WRITE_MEM32(PBVR4_4,  0x00000000);	/* User-defined 1 */
	WRITE_MEM32(PBVR5_0,  0x00000000);	/* User-defined 2 */
	WRITE_MEM32(PBVR5_1,  0x00000000);	/* User-defined 2 */
	WRITE_MEM32(PBVR5_2,  0x00000000);	/* User-defined 2 */
	WRITE_MEM32(PBVR5_3,  0x00000000);	/* User-defined 2 */
	WRITE_MEM32(PBVR5_4,  0x00000000);	/* User-defined 2 */
	WRITE_MEM32(MSCR,0);   
	WRITE_MEM32(PCRP0,1);   
	WRITE_MEM32(PCRP1,1);   
	WRITE_MEM32(PCRP2,1);   
	WRITE_MEM32(PCRP3,1);   
	WRITE_MEM32(PCRP4,1);   
	WRITE_MEM32(PCRP5,1);   
	WRITE_MEM32(PCRP6,1);   
	WRITE_MEM32(PCRP7,1);   
	WRITE_MEM32(PCRP8,1);   
	WRITE_MEM32(PPMAR,1);
	WRITE_MEM32(SIRR, TRXRDY);
	/* WRITE_MEM32(TMCR,0xFCFC0000); */
	WRITE_MEM32(QIDDPCR,(0x1<<NAPT_PRI_OFFSET)|(0x1<<ACL_PRI_OFFSET)|(0x1<<DSCP_PRI_OFFSET)|(0x1<<BP8021Q_PRI_OFFSET)|(0x1<<PBP_PRI_OFFSET));
	WRITE_MEM32(LPTM8021Q,0);
	WRITE_MEM32(DSCPCR0,0);
	WRITE_MEM32(DSCPCR1,0);
	WRITE_MEM32(DSCPCR2,0);
	WRITE_MEM32(DSCPCR3,0);
	WRITE_MEM32(DSCPCR4,0);
	WRITE_MEM32(DSCPCR5,0);
	WRITE_MEM32(DSCPCR6,0);	
	WRITE_MEM32(RMACR,0);
	WRITE_MEM32(ALECR,0);

	/* System Based Flow Control Threshold Register */
	WRITE_MEM32( SBFCR0, (0x1E0<<S_DSC_RUNOUT_OFFSET) );
	WRITE_MEM32( SBFCR1, (0x0190<<S_DSC_FCOFF_OFFSET)|(0x01CC<<S_DSC_FCON_OFFSET) );
	WRITE_MEM32( SBFCR2, (0x0050<<S_Max_SBuf_FCOFF_OFFSET)|(0x006C<<S_Max_SBuf_FCON_OFFSET) );
	/* Port Based Flow Control Threshold Register */
	WRITE_MEM32( PBFCR0, (0x003C<<P_MaxDSC_FCOFF_OFFSET)|(0x005A<<P_MaxDSC_FCON_OFFSET) );
	WRITE_MEM32( PBFCR1, (0x003C<<P_MaxDSC_FCOFF_OFFSET)|(0x005A<<P_MaxDSC_FCON_OFFSET) );
	WRITE_MEM32( PBFCR2, (0x003C<<P_MaxDSC_FCOFF_OFFSET)|(0x005A<<P_MaxDSC_FCON_OFFSET) );
	WRITE_MEM32( PBFCR3, (0x003C<<P_MaxDSC_FCOFF_OFFSET)|(0x005A<<P_MaxDSC_FCON_OFFSET) );
	WRITE_MEM32( PBFCR4, (0x003C<<P_MaxDSC_FCOFF_OFFSET)|(0x005A<<P_MaxDSC_FCON_OFFSET) );
	WRITE_MEM32( PBFCR5, (0x003C<<P_MaxDSC_FCOFF_OFFSET)|(0x005A<<P_MaxDSC_FCON_OFFSET) );

	/* Packet Schecduling Control Register */
	WRITE_MEM32(ELBPCR, (51<<Token_OFFSET)|(38<<Tick_OFFSET) );
	WRITE_MEM32(ELBTTCR, /*0x99*/0x400<<L2_OFFSET );
	WRITE_MEM32(ILBPCR1, (0<<UpperBound_OFFSET)|(0<<LowerBound_OFFSET) );
	WRITE_MEM32(ILBPCR2, (0x33<<ILB_feedToken_OFFSET)|(0x26<<ILB_Tick_OFFSET) );
	for( i = 0; i<42; i++ )
		WRITE_MEM32(P0Q0RGCR+i*4, (7<<PPR_OFFSET)|(0xff<<L1_OFFSET)|(0x3FFF<<APR_OFFSET) );
	for( i = 0; i<7; i++ )
	{
		WRITE_MEM32(WFQRCRP0+i*4*3+ 0, 0x3fff<<APR_OFFSET );
		WRITE_MEM32(WFQRCRP0+i*4*3+ 4, 0 ); /* WFQWCR0P? */
		WRITE_MEM32(WFQRCRP0+i*4*3+ 8, 0 ); /* WFQWCR1P? */
	}

	for (i=0;i<48;i+=4)
			WRITE_MEM32(PATP0+i,0xfe12);	

	rtl8651_totalExtPortNum=0;

	/* Set chip's mode as NORMAL mode */
	WRITE_MEM32(TMCR,~ENHSBTESTMODE&READ_MEM32(TMCR));

	/* CPU NIC */
	WRITE_MEM32(CPUICR,READ_MEM32(CPUICR)&~(LBMODE|LB10MHZ|MITIGATION|EXCLUDE_CRC)); 
	WRITE_MEM32(CPUIISR,READ_MEM32(CPUIISR));  /* clear all interrupt */
	WRITE_MEM16(CPUQDM0,0); 
	WRITE_MEM16(CPUQDM1,0); 
	WRITE_MEM16(CPUQDM2,0); 
	WRITE_MEM16(CPUQDM3,0); 
	WRITE_MEM16(CPUQDM4,0); 
	WRITE_MEM16(CPUQDM5,0); 
	WRITE_MEM32(RMCR1P,0); 
	WRITE_MEM32(DSCPRM0,0); 
	WRITE_MEM32(DSCPRM1,0); 
	WRITE_MEM32(RLRC,0); 
	
#if 0 /* Since swNic only init once when starting model code, we don't clear the following registers. */
	WRITE_MEM32(CPURPDCR0,0); 
	WRITE_MEM32(CPURPDCR1,0); 
	WRITE_MEM32(CPURPDCR2,0); 
	WRITE_MEM32(CPURPDCR3,0); 
	WRITE_MEM32(CPURPDCR4,0); 
	WRITE_MEM32(CPURPDCR5,0); 
	WRITE_MEM32(CPURMDCR0,0); 
	WRITE_MEM32(CPUTPDCR0,0); 
	WRITE_MEM32(CPUTPDCR1,0); 
	WRITE_MEM32(CPUIIMR,0); 
#endif

#else
	rtlglue_printf("Driver of current CHIP version does not support [%s]\n", __FUNCTION__);
#endif
}

static void _rtl8651_clearSpecifiedAsicTable(uint32 type, uint32 count) {
	rtl8651_tblAsic_aclTable_t entry;
	uint32 idx;
	
	bzero(&entry, sizeof(entry));
	for (idx=0; idx<count; idx++)// Write into hardware
		_rtl8651_forceAddAsicEntry(type, idx, &entry);

}

int32 rtl8651_clearAsicAllTable(void) {
#if defined(VSV)||defined(MIILIKE)
	/* In VERA mode, we need to speed up table clean. */
	{
		enum IC_TYPE ictype;

		model_getTestTarget( &ictype );

		if ( ictype==IC_TYPE_REAL )
		{	
			vsv_clearAllTable(conn_client);
			return SUCCESS;
		}
	}
#endif

	_rtl8651_clearSpecifiedAsicTable(TYPE_L2_SWITCH_TABLE, RTL8651_L2TBL_ROW*RTL8651_L2TBL_COLUMN);
	_rtl8651_clearSpecifiedAsicTable(TYPE_ARP_TABLE, RTL8651_ARPTBL_SIZE);
	_rtl8651_clearSpecifiedAsicTable(TYPE_L3_ROUTING_TABLE, RTL8651_ROUTINGTBL_SIZE);
	_rtl8651_clearSpecifiedAsicTable(TYPE_MULTICAST_TABLE, RTL8651_IPMULTICASTTBL_SIZE);
	_rtl8651_clearSpecifiedAsicTable(TYPE_NETINTERFACE_TABLE, RTL865XC_NETINTERFACE_NUMBER);
	_rtl8651_clearSpecifiedAsicTable(TYPE_VLAN_TABLE, RTL865XC_VLAN_NUMBER);
	_rtl8651_clearSpecifiedAsicTable(TYPE_EXT_INT_IP_TABLE, RTL8651_IPTABLE_SIZE);
	_rtl8651_clearSpecifiedAsicTable(TYPE_ALG_TABLE, RTL865XC_ALGTBL_SIZE);
	_rtl8651_clearSpecifiedAsicTable(TYPE_SERVER_PORT_TABLE, RTL8651_SERVERPORTTBL_SIZE);
	_rtl8651_clearSpecifiedAsicTable(TYPE_L4_TCP_UDP_TABLE, RTL8651_TCPUDPTBL_SIZE);
	_rtl8651_clearSpecifiedAsicTable(TYPE_L4_ICMP_TABLE, RTL8651_ICMPTBL_SIZE);
	_rtl8651_clearSpecifiedAsicTable(TYPE_PPPOE_TABLE, RTL8651_PPPOE_NUMBER);
	_rtl8651_clearSpecifiedAsicTable(TYPE_ACL_RULE_TABLE, RTL8651_ACLTBL_SIZE);
	_rtl8651_clearSpecifiedAsicTable(TYPE_NEXT_HOP_TABLE, RTL8651_NEXTHOPTBL_SIZE);
	_rtl8651_clearSpecifiedAsicTable(TYPE_RATE_LIMIT_TABLE, RTL8651_RATELIMITTBL_SIZE);
	return SUCCESS;
}

unsigned int GetSysClockRateForAsicDrv(void)
{
	unsigned int SysClkRate;
	
    REG32(MACMR) = REG32(MACMR) & ~SYS_CLK_MASK;
    switch ( REG32(MACMR) & SYS_CLK_MASK )
    {
        case SYS_CLK_100M:
            SysClkRate = 100000000;
            break;
        case SYS_CLK_90M:
            SysClkRate = 90000000;
            break;
        case SYS_CLK_85M:
            SysClkRate = 85000000;
            break;
        case SYS_CLK_96M:
            SysClkRate = 96000000;
            break;
        case SYS_CLK_80M:
            SysClkRate = 80000000;
            break;
        case SYS_CLK_75M:
            SysClkRate = 75000000;
            break;
        case SYS_CLK_70M:
            SysClkRate = 70000000;
            break;
        case SYS_CLK_50M:
            SysClkRate = 50000000;
            break;
        default:
            while(1);
    }

    return SysClkRate;
}

/*cfliu: Remove legacy patch code for 8650Bv1 !!*/
#if 0

/*
 * Configure Timer 1 to be a counter to emulate 10ms.
 */

#define DIVISOR 4096   /*change from 0xe to 4096 for watchdog*/
#define HZ 100
void tick_Delay10ms( uint32 count )
{
#ifndef RTL865X_TEST
	uint32 passed;
	uint32 timerData = (((GetSysClockRateForAsicDrv()/DIVISOR)/HZ)+1) << TCD_OFFSET;
	
	for( passed = 0; passed < count; passed++ )
	{
		/* set counter initial value */
		WRITE_MEM32( TC1DATA, timerData );	
		
		/* Enable timer1 */
		WRITE_MEM32( TCCNR, READ_MEM32(TCCNR) | TC1EN | TC1MODE_COUNTER );

		/* Wait until the counter reaches */
		while( READ_MEM32( TC1CNT ) >= ( 2 << TCD_OFFSET ) ); /* The counter will stop counting at 1. */

		/* Disable timer1 */
		WRITE_MEM32( TCCNR, READ_MEM32(TCCNR) & ~TC1EN );
	}
#endif
}

void tick_Delay100ms( uint32 count )
{
	tick_Delay10ms( count * 10 );
}



void FullAndSemiReset( void )
{
	int c;

	/* Perform full-reset for sw-core. */
	WRITE_MEM32( MACCR, READ_MEM32(MACCR) | FULL_RST );

	/* Wait BISTing bit become 1. */
	c = 0;
	while( ((READ_MEM32( BISTCR )) & 0x40000000) == 0 )
	{
		tick_Delay10ms(1);
		c++;
		if(c>100) break;
	}

	/* Wait BISTing bit become 0. */
	c = 0;
	while( READ_MEM32( BISTCR ) & 0x40000000 )
	{
		tick_Delay10ms(1);
		c++;
		if(c>100) break;
	}

	/* Delay 100ms after BISTing bit became 0. */
	tick_Delay100ms(1);

	/* Disable TRXRDY */
	WRITE_MEM32( BISTCR, READ_MEM32( BISTCR ) & ~TRXRDY);

	/* Semi-Reset */
	WRITE_MEM32( MACCR, READ_MEM32( MACCR ) | SEMI_RST );

	/* Enable TRXRDY */
	WRITE_MEM32( BISTCR, READ_MEM32( BISTCR ) | TRXRDY );
	
	/* Wait QOK and COK bit all become 1. */
	c = 0;
	while( ( READ_MEM32( BISTCR ) & 0x30000000 ) != 0x30000000 )
	{
		tick_Delay10ms(1);
		c++;
		if(c>100) break;
	}

	/* clear bits */
	REG32(MACCR) &=  ~(FULL_RST|SEMI_RST);
}
#endif
#endif

int32 _rtl8651_mapToVirtualRegSpace( void )
{
#if 0
	/* Faster: Use malloc to speed-up FPGA download. ICE no need to initialize virtual space. */
	static int8 *virtualSwReg = NULL;
	static int8 *virtualSysReg = NULL;
	static int8 *virtualHsb = NULL;
	static int8 *virtualHsa = NULL;
	static int8 *virtualSwTable = NULL;

	if ( virtualSwReg==NULL )
	{
		virtualSwReg = rtlglue_malloc( VIRTUAL_SWCORE_REG_SIZE );
		assert( virtualSwReg );
		memset( virtualSwReg, 0, VIRTUAL_SWCORE_REG_SIZE );
	}
	if ( virtualSysReg==NULL )
	{
		virtualSysReg = rtlglue_malloc( VIRTUAL_SYSTEM_REG_SIZE );
		assert( virtualSysReg );
		memset( virtualSysReg, 0, VIRTUAL_SYSTEM_REG_SIZE );
	}
	if ( virtualHsb==NULL )
	{
		virtualHsb = rtlglue_malloc( HSB_SIZE );
		assert( virtualHsb );
		memset( virtualHsb, 0, HSB_SIZE );
	}
	if ( virtualHsa==NULL )
	{
		virtualHsa = rtlglue_malloc( HSA_SIZE );
		assert( virtualHsa );
		memset( virtualHsa, 0, HSA_SIZE );
	}
	if ( virtualSwTable==NULL )
	{
		virtualSwTable = rtlglue_malloc( VIRTUAL_SWCORE_TBL_SIZE);
		assert( virtualSwTable );
		memset( virtualSwTable, 0, VIRTUAL_SWCORE_TBL_SIZE );
	}
#else
	/* Slower: ICE needs to initialize static virtual space . */
	static int8 virtualSwReg[VIRTUAL_SWCORE_REG_SIZE];
	static int8 virtualSysReg[VIRTUAL_SYSTEM_REG_SIZE];
	static int8 virtualHsb[HSB_SIZE];
	static int8 virtualHsa[HSA_SIZE];
	static int8 virtualSwTable[VIRTUAL_SWCORE_TBL_SIZE];
#endif
	pVirtualSWReg = (int8*)(virtualSwReg);
	pVirtualSysReg = (int8*)(virtualSysReg);
	pVirtualHsb = virtualHsb;
	pVirtualHsa = virtualHsa;
	pVirtualSWTable = virtualSwTable;
	
	return SUCCESS;
}

int32 _rtl8651_mapToRealRegSpace( void )
{
	pVirtualSWReg = (int8*)REAL_SWCORE_BASE;
	pVirtualSysReg = (int8*)REAL_SYSTEM_BASE;
	pVirtualHsb = (int8*)REAL_HSB_BASE;
	pVirtualHsa = (int8*)REAL_HSA_BASE;
	pVirtualSWTable = (int8*)REAL_SWTBL_BASE;
	return SUCCESS;
}

#if 0
#if defined(RTL865XC_MNQUEUE_OUTPUTQUEUE)  || defined(RTL865XC_QOS_OUTPUTQUEUE)
static int32 _rtl865x_setQosThresholdByQueueIdx(uint32 qidx)
{
	/* Set the threshold value for qos sytem */
	int32 retval;
	int32	i,j;

	retval = rtl8651_setAsicSystemBasedFlowControlRegister(outputQueuePara[qidx].systemSBFCON, outputQueuePara[qidx].systemSBFCOFF, outputQueuePara[qidx].systemFCON, outputQueuePara[qidx].systemFCOFF, outputQueuePara[qidx].drop);
	if (retval!= SUCCESS)
	{
		rtlglue_printf("Set System Base Flow Control Para Error.\n");
		return retval;
	}
	
	for(i =0; i < RTL8651_OUTPUTQUEUE_SIZE; i++)
	{
		retval = rtl8651_setAsicQueueDescriptorBasedFlowControlRegister(0, i, outputQueuePara[qidx].queueDescFCON, outputQueuePara[qidx].queueDescFCOFF);
		if (retval!= SUCCESS)
		{
			rtlglue_printf("Set Queue Descriptor Base Flow Control Para Error.\n");
			return retval;
		}
		for(j=1;j<CPU;j++)
			rtl8651_setAsicQueueDescriptorBasedFlowControlRegister(PHY0+j, i, outputQueuePara[qidx].queueDescFCON, outputQueuePara[qidx].queueDescFCOFF);


		retval = rtl8651_setAsicQueuePacketBasedFlowControlRegister(0, i, outputQueuePara[qidx].queuePktFCON, outputQueuePara[qidx].queuePktFCOFF);
		if (retval!= SUCCESS)
		{
			rtlglue_printf("Set Queue Packet Base Flow Control Para Error.\n");
			return retval;
		}
		for(j=1;j<CPU;j++)
			rtl8651_setAsicQueuePacketBasedFlowControlRegister(PHY0+j, i, outputQueuePara[qidx].queuePktFCON, outputQueuePara[qidx].queuePktFCOFF);

	}

	retval = rtl8651_setAsicPortBasedFlowControlRegister(0, outputQueuePara[qidx].portFCON, outputQueuePara[qidx].portFCOFF);
	if (retval!= SUCCESS)
	{
		rtlglue_printf("Set Port Base Flow Control Para Error.\n");
		return retval;
	}
	for(j=1;j<CPU;j++)
		rtl8651_setAsicPortBasedFlowControlRegister(PHY0+j, outputQueuePara[qidx].portFCON, outputQueuePara[qidx].portFCOFF);
	
	retval = rtl8651_setAsicPerQueuePhysicalLengthGapRegister(outputQueuePara[qidx].gap);
	if (retval!= SUCCESS)
	{
		rtlglue_printf("Set Queue Physical Lenght Gap Reg Error.\n");
		return retval;
	}
	return SUCCESS;
}
#endif
/*
	Initiate ASIC configurating parameter.
*/
static int32 _rtl8651_initAsicPara( rtl8651_tblAsic_InitPara_t *para )
{
	memset(&rtl8651_tblAsicDrvPara, 0, sizeof(rtl8651_tblAsic_InitPara_t));

	if ( para )
	{
		/* Parameter != NULL, check its correctness */
		if (para->externalPHYProperty & RTL8651_TBLASIC_EXTPHYPROPERTY_PORT1234_RTL8212)
		{
			ASICDRV_ASSERT(para->externalPHYId[1] != 0);
			ASICDRV_ASSERT(para->externalPHYId[2] != 0);
			ASICDRV_ASSERT(para->externalPHYId[3] != 0);
			ASICDRV_ASSERT(para->externalPHYId[4] != 0);
			ASICDRV_ASSERT(para->externalPHYId[2] == (para->externalPHYId[1] + 1));
			ASICDRV_ASSERT(para->externalPHYId[4] == (para->externalPHYId[3] + 1));
		}
		if (para->externalPHYProperty & RTL8651_TBLASIC_EXTPHYPROPERTY_PORT5_RTL8211B)
		{
			ASICDRV_ASSERT(para->externalPHYId[5] != 0);
		}

		/* ============= Check passed : set it =============  */
		rtl8651_memcpy(&rtl8651_tblAsicDrvPara, para, sizeof(rtl8651_tblAsic_InitPara_t));
	}

	return SUCCESS;
}

int32 rtl8651_initAsic(rtl8651_tblAsic_InitPara_t *para) 
{
	int32 index;
	int32 bondingType_PCM;
#if defined(VERA)||defined(VSV)||defined(MIILIKE)
#else
	uint16 flowTblIdx;
	rtl865x_tblAsicDrv_naptTcpUdpParam_t naptTcpUdp;
	rtl865x_tblAsicDrv_naptIcmpParam_t naptIcmp;
#endif

#if defined(RTL865X_TEST)
	char chipVersion[16];
	int rev;

	/* _rtl8651_mapToVirtualRegSpace(); to be removed */
	rtl8651_getChipVersion(chipVersion, sizeof(chipVersion), &rev);
	bzero(pVirtualSWReg, VIRTUAL_SWCORE_REG_SIZE);
	bzero(pVirtualSysReg, VIRTUAL_SYSTEM_REG_SIZE);
	bzero(pVirtualHsb, HSB_SIZE);
	bzero(pVirtualHsa, HSA_SIZE);
	bzero(pVirtualSWTable, VIRTUAL_SWCORE_TBL_SIZE);
	bzero(pVirtualMIIRegs, sizeof(pVirtualMIIRegs));
	rtl8651_setChipVersion(chipVersion,  &rev);

#elif 0 && defined(RTL865X_MODEL_USER) /* map to real reg space will cause VSV test crash in model code mode. */
#if defined(VSV)||defined(MIILIKE)
	_rtl8651_mapToRealRegSpace();
#else
	_rtl8651_mapToVirtualRegSpace(); /* for cleshell.c:main() only ! */
	modelIcSetDefaultValue();
#endif
#elif defined(RTL865X_MODEL_KERNEL)
#endif

	ASICDRV_INIT_CHECK(_rtl8651_initAsicPara(para));

	rtl8651_getChipVersion(RtkHomeGatewayChipName, sizeof(RtkHomeGatewayChipName), &RtkHomeGatewayChipRevisionID);
	rtl8651_getChipNameID(&RtkHomeGatewayChipNameID);
	/*bit1~bit2*/
	bondingType_PCM = READ_MEM32(CRMR) & 0x06;

#if 0
	/*
	*this driver version is only for 865xB or (865xC-D cut && bondingType_PCM > 0)
	*hyking
	*/
	if((strcmp(RtkHomeGatewayChipName,"865xC") == 0) && ((RtkHomeGatewayChipRevisionID < RTL865X_CHIP_REV_D)||(bondingType_PCM < 1)))
	{
		//RTL_BUG("Can't initialize this chip version.");
		
		rtlglue_printf("Can't initialize this chip version.\n");		
		while(1);
	}
#endif

#ifndef RTL865X_TEST
	rtlglue_printf("chip name: %s, chip revid: %d\n", RtkHomeGatewayChipName, RtkHomeGatewayChipRevisionID);
#endif

	if (rtl8651_tblAsicDrvPara.externalPHYProperty & RTL8651_TBLASIC_EXTPHYPROPERTY_PORT1234_RTL8212)
	{
		/* Set P1 - P4 to SerDes Interface. */
		WRITE_MEM32(PITCR, Port4_TypeCfg_SerDes | Port3_TypeCfg_SerDes | Port2_TypeCfg_SerDes | Port1_TypeCfg_SerDes );
	}

	if (rtl8651_tblAsicDrvPara.externalPHYProperty & RTL8651_TBLASIC_EXTPHYPROPERTY_PORT5_RTL8211B)
	{
		/* Patch for 'RGMII port does not get descritpors'. Set to MII PHY mode first and later we'll change to RGMII mode again. */
		rtl865xC_setAsicEthernetMIIMode(RTL8651_MII_PORTNUMBER, LINK_MII_PHY);

		/*
			# According to Hardware SD: David & Maxod,
			
			Set Port5_GMII Configuration Register.
			- RGMII Output Timing compensation control : 0 ns
			- RGMII Input Timing compensation control : 0 ns
		*/
		rtl865xC_setAsicEthernetRGMIITiming(RTL8651_MII_PORTNUMBER, RGMII_TCOMP_0NS, RGMII_RCOMP_0NS);
	}

	/* 	2006.12.12
		We turn on bit.10 (ENATT2LOG). 

		* Current implementation of unnumbered pppoe in multiple session
		When wan type is multiple-session, and one session is unnumbered pppoe, WAN to unnumbered LAN is RP --> NPI.
		And user adds ACL rule to trap dip = unnumbered LAN to CPU.

		However, when pktOpApp of this ACL rule is set, it seems that this toCPU ACL does not work.
		Therefore, we turn on this bit (ENATT2LOG) to trap pkts (WAN --> unnumbered LAN) to CPU.
		
	*/
	WRITE_MEM32( SWTCR1, READ_MEM32( SWTCR1 ) | EnNATT2LOG );

	/* 
	  * Turn on ENFRAG2ACLPT for Rate Limit. For those packets which need to be trapped to CPU, we turn on
	  * this bit to tell ASIC ACL and Protocol Trap to process these packets. If this bit is not turnned on, packets
	  * which need to be trapped to CPU will not be processed by ASIC ACL and Protocol Trap.
	  * NOTE: 	If this bit is turned on, the backward compatible will disable.
	  *																- chhuang
	  */
	WRITE_MEM32( SWTCR1, READ_MEM32( SWTCR1 ) | ENFRAGTOACLPT );
	
#ifdef CONFIG_RTL865X_LIGHT_ROMEDRV
	WRITE_MEM32( SWTCR1, READ_MEM32( SWTCR1 ) | L4EnHash1 );    /*Turn on Napt Enhanced hash1*/
#endif

	/* 
	  * Cannot turn on EnNAP8651B due to:
	  * If turn on, NAT/LP/ServerPort will reference nexthop. This will result in referecing wrong L2 entry when
	  * the destination host is in the same subnet as WAN.
	  */


	/*Although chip is in 8650 compatible mode, 
	some 865XB features are independent to compatibility register*/
	/*Initialize them here if needed*/

	{
		 int rev;
		char chipVersion[16];
		rtl8651_getChipVersion(chipVersion, sizeof(chipVersion), &rev);
		if(chipVersion[strlen(chipVersion)-1]=='B' || chipVersion[strlen(chipVersion) - 1] == 'C' ){
			rtl8651_totalExtPortNum=3; //this replaces all RTL8651_EXTPORT_NUMBER defines
			rtl8651_allExtPortMask = 0x7<<RTL8651_MAC_NUMBER; //this replaces all RTL8651_EXTPORTMASK defines
#if !defined(RTL865X_TEST) && !defined(RTL865X_MODEL_USER)
			rtl8651_asicEthernetCableMeterInit();
#endif
		}
		
	}
	//Disable layer2, layer3 and layer4 function
	//Layer 2 enabled automatically when a VLAN is added
	//Layer 3 enabled automatically when a network interface is added.
	//Layer 4 enabled automatically when an IP address is setup.
	rtl8651_setAsicOperationLayer(1);
	rtl8651_clearAsicAllTable();//MAY BE OMITTED. FULL_RST clears all tables already.
	rtl8651_setAsicSpanningEnable(FALSE);

#ifdef RTL865XB_URL_FILTER
	WRITE_MEM32( SWTCR1, READ_MEM32( SWTCR1 ) | EN_51B_CPU_REASON );	/* Use 8650B's new reason bit definition. */
#endif

#if !defined(RTL865X_TEST) && !defined(RTL865X_MODEL_USER)
	WRITE_MEM32(0xbd012064,READ_MEM32(0xbd012064)|0xf0000000);//Enable Lexra bus timeout interrupt and timeout limit
	#if 0
	// shliu: Why we set PORT5_PHY_CONTROL to value "0x2c7"?? Should set "0x2c2"?!
	WRITE_MEM32(0xbc800020,0x000002c7);
	WRITE_MEM32(0xbc800008,0xbc8020a0);
	WRITE_MEM32(0xbc800000,0x00000009);
	#endif
#endif

	//MAC Control (0xBC803000)
/*	WRITE_MEM32(MACCR,READ_MEM32(MACCR)&~DIS_IPG);//Set IFG range as 96+-4bit time*/
	WRITE_MEM32(MACCR,READ_MEM32(MACCR)&~NORMAL_BACKOFF);//Normal backoff
	WRITE_MEM32(MACCR,READ_MEM32(MACCR)&~BACKOFF_EXPONENTIAL_3);//Exponential parameter is 9
	WRITE_MEM32(MACCR,READ_MEM32(MACCR)|INFINITE_PAUSE_FRAMES);//send pause frames infinitely.
	WRITE_MEM32(MACCR,READ_MEM32(MACCR)|DIS_MASK_CGST);

	miiPhyAddress = -1;		/* not ready to use mii port 5 */

	memset( &rtl8651AsicEthernetTable[0], 0, ( RTL8651_PORT_NUMBER + rtl8651_totalExtPortNum ) * sizeof(rtl8651_tblAsic_ethernet_t) );
	/* Record the PHYIDs of physical ports. Default values are 0. */
	rtl8651AsicEthernetTable[0].phyId = 0;	/* Default value of port 0's embedded phy id -- 0 */
	rtl8651AsicEthernetTable[0].isGPHY = FALSE;

	if (rtl8651_tblAsicDrvPara.externalPHYProperty & RTL8651_TBLASIC_EXTPHYPROPERTY_PORT1234_RTL8212)
	{
		rtl8651AsicEthernetTable[1].phyId = rtl8651_tblAsicDrvPara.externalPHYId[1];
		rtl8651AsicEthernetTable[2].phyId = rtl8651_tblAsicDrvPara.externalPHYId[2];
		rtl8651AsicEthernetTable[3].phyId = rtl8651_tblAsicDrvPara.externalPHYId[3];
		rtl8651AsicEthernetTable[4].phyId = rtl8651_tblAsicDrvPara.externalPHYId[4];
		rtl8651AsicEthernetTable[1].isGPHY = TRUE;
		rtl8651AsicEthernetTable[2].isGPHY = TRUE;
		rtl8651AsicEthernetTable[3].isGPHY = TRUE;
		rtl8651AsicEthernetTable[4].isGPHY = TRUE;
	} else
	{	/* USE internal 10/100 PHY */
		rtl8651AsicEthernetTable[1].phyId = 1;	/* Default value of port 1's embedded phy id -- 1 */
		rtl8651AsicEthernetTable[2].phyId = 2;	/* Default value of port 2's embedded phy id -- 2 */
		rtl8651AsicEthernetTable[3].phyId = 3;	/* Default value of port 3's embedded phy id -- 3 */
		rtl8651AsicEthernetTable[4].phyId = 4;	/* Default value of port 4's embedded phy id -- 4 */
		rtl8651AsicEthernetTable[1].isGPHY = FALSE;
		rtl8651AsicEthernetTable[2].isGPHY = FALSE;
		rtl8651AsicEthernetTable[3].isGPHY = FALSE;
		rtl8651AsicEthernetTable[4].isGPHY = FALSE;
	}

	if (rtl8651_tblAsicDrvPara.externalPHYProperty & RTL8651_TBLASIC_EXTPHYPROPERTY_PORT5_RTL8211B)
	{
		rtl8651AsicEthernetTable[RTL8651_MII_PORTNUMBER].phyId = rtl8651_tblAsicDrvPara.externalPHYId[5];
		rtl8651AsicEthernetTable[RTL8651_MII_PORTNUMBER].isGPHY = TRUE;
		rtl8651_setAsicEthernetMII(	rtl8651AsicEthernetTable[RTL8651_MII_PORTNUMBER].phyId,
									P5_LINK_RGMII,
									TRUE );
	}

	WRITE_MEM32(PCRP0, (rtl8651AsicEthernetTable[0].phyId<<ExtPHYID_OFFSET)|AcptMaxLen_16K|EnablePHYIf  ); /* Jumbo Frame */
	WRITE_MEM32(PCRP1, (rtl8651AsicEthernetTable[1].phyId<<ExtPHYID_OFFSET)|AcptMaxLen_16K|EnablePHYIf ); /* Jumbo Frame */
	WRITE_MEM32(PCRP2, (rtl8651AsicEthernetTable[2].phyId<<ExtPHYID_OFFSET)|AcptMaxLen_16K|EnablePHYIf ); /* Jumbo Frame */
	WRITE_MEM32(PCRP3, (rtl8651AsicEthernetTable[3].phyId<<ExtPHYID_OFFSET)|AcptMaxLen_16K|EnablePHYIf ); /* Jumbo Frame */
	WRITE_MEM32(PCRP4, (rtl8651AsicEthernetTable[4].phyId<<ExtPHYID_OFFSET)|AcptMaxLen_16K|EnablePHYIf ); /* Jumbo Frame */

	if (rtl8651_tblAsicDrvPara.externalPHYProperty & RTL8651_TBLASIC_EXTPHYPROPERTY_PORT5_RTL8211B)
	{
		WRITE_MEM32(PCRP5, (rtl8651AsicEthernetTable[5].phyId<<ExtPHYID_OFFSET)|AcptMaxLen_16K|EnablePHYIf ); /* Jumbo Frame */
	}

#if 0	/* No need to set PHYID of port 6. Just use ASIC default value. */
	/*  Due to MSb of phyid has been added an inverter in b-cut,
	 *  although we want to set 6(0b00110) as phyid, we have to write 22(0b10110) instead. */
	WRITE_MEM32( PCRP6, ( 22 << ExtPHYID_OFFSET ) | AcptMaxLen_16K | EnablePHYIf );
#endif
	if(RTL865X_PHY6_DSP_BUG)
		WRITE_MEM32(PCRP6, (6<<ExtPHYID_OFFSET)|AcptMaxLen_16K|EnablePHYIf );
	/* Set PHYID 6 to PCRP6. (By default, PHYID of PCRP6 is 0. It will collide with PHYID of port 0. */
	


/*	WRITE_MEM32(MACCR,READ_MEM32(MACCR)&~(EN_FX_P4 | EN_FX_P3 | EN_FX_P2 | EN_FX_P1 | EN_FX_P0));//Disable FX mode (UTP mode)*/
	/* Initialize MIB counters */
	rtl8651_clearAsicCounter();

#if patch_localpublic_by_macbased_interface
	rtl865xC_setNetDecisionPolicy( NETIF_MAC_BASED );	/* Net interface Multilayer-Decision-Based Control -- Set to MAC-Based mode. */
#else
	rtl865xC_setNetDecisionPolicy( NETIF_VLAN_BASED );	/* Net interface Multilayer-Decision-Based Control -- Set to VLAN-Based mode. */
#endif
	WRITE_MEM32(SWTCR0,READ_MEM32(SWTCR0)&~NAPTR_NOT_FOUND_DROP);//When reverse NAPT entry not found, CPU process it.
	rtl8651_setAsicNaptAutoAddDelete(FALSE, TRUE);
	WRITE_MEM32( VCR0, READ_MEM32( VCR0 ) & (~EN_ALL_PORT_VLAN_INGRESS_FILTER) );		/* Disable VLAN ingress filter of all ports */ /* Please reference to the maintis bug 2656# */
	WRITE_MEM32(SWTCR0,READ_MEM32(SWTCR0)&~WAN_ROUTE_MASK);//Set WAN route toEnable (Allow traffic from WAN port to WAN port)
	WRITE_MEM32(SWTCR0,READ_MEM32(SWTCR0)|NAPTF2CPU);//When packet destination to switch. Just send to CPU
	rtl8651_setAsicMulticastEnable(TRUE); /* Enable multicast table */

	/* Enable unknown unicast / multicast packet to be trapped to CPU. */
	WRITE_MEM32( FFCR, READ_MEM32( FFCR ) | EN_UNUNICAST_TOCPU );
	WRITE_MEM32( FFCR, READ_MEM32( FFCR ) | EN_UNMCAST_TOCPU );
/*	WRITE_MEM32(SWTMCR,READ_MEM32(SWTMCR)&~MCAST_TO_CPU);*/
/*	WRITE_MEM32(SWTMCR,READ_MEM32(SWTMCR)|EN_BCAST);//Enable special broadcast handling*/
/*	WRITE_MEM32(SWTMCR,READ_MEM32(SWTMCR)&~BCAST_TO_CPU);//When EN_BCAST enables, this bit is invalid*/
/*	WRITE_MEM32(SWTMCR,READ_MEM32(SWTMCR)&~BRIDGE_PKT_TO_CPU);//Current no bridge protocol is supported*/

	WRITE_MEM32(ALECR, READ_MEM32(ALECR)|(uint32)EN_PPPOE);//enable PPPoE auto encapsulation
	WRITE_MEM32(CSCR,READ_MEM32(CSCR)&~ALLOW_L2_CHKSUM_ERR);//Don't allow chcksum error pkt be forwarded.
	WRITE_MEM32(CSCR,READ_MEM32(CSCR)&~ALLOW_L3_CHKSUM_ERR);
	WRITE_MEM32(CSCR,READ_MEM32(CSCR)&~ALLOW_L4_CHKSUM_ERR);
	WRITE_MEM32(CSCR,READ_MEM32(CSCR)|EN_ETHER_L3_CHKSUM_REC); //Enable L3 checksum Re-calculation
	WRITE_MEM32(CSCR,READ_MEM32(CSCR)|EN_ETHER_L4_CHKSUM_REC); //Enable L4 checksum Re-calculation

/*	WRITE_MEM32(MISCCR,READ_MEM32(MISCCR)&~FRAG2CPU);//IP fragment packet does not need to send to CPU when initial ASIC
	WRITE_MEM32(MISCCR,READ_MEM32(MISCCR)&~MULTICAST_L2_MTU_MASK);
	WRITE_MEM32(MISCCR,READ_MEM32(MISCCR)|(1522&MULTICAST_L2_MTU_MASK));//Multicast packet layer2 size 1522 at most*/
	/* follow RTL865xB's convention, we use 1522 as default multicast MTU */
	rtl8651_setAsicMulticastMTU(1522);

	//Set all Protocol-Based Reg. to 0

	for (index=0;index<32;index++)
		WRITE_MEM32(PBVCR0+index*4,  0x00000000);	
	//Enable TTL-1
	WRITE_MEM32(TTLCR,READ_MEM32(TTLCR)|(uint32)EN_TTL1);//Don't hide this router. enable TTL-1 when routing on this gateway.

	for (index=0; index<RTL8651_PORT_NUMBER+rtl8651_totalExtPortNum; index++) {
		if(	rtl8651_setAsicMulticastPortInternal(index, TRUE)||
			rtl8651_setAsicMulticastSpanningTreePortState(index, RTL8651_PORTSTA_FORWARDING))
			return FAILED;
		rtl865xC_setAsicSpanningTreePortState(index, RTL8651_PORTSTA_FORWARDING);

		rtl8651_setAsicEthernetBandwidthControl(index, TRUE, RTL8651_BC_FULL);
		rtl8651_setAsicEthernetBandwidthControl(index, FALSE, RTL8651_BC_FULL);
	}

#if !defined(RTL865X_TEST) && !defined(RTL865X_MODEL_USER)
#if defined(CONFIG_RTL865X_FPGA)
	/* In FPGA, we CANNOT BIST. */
#else

	{
		int cnt;

		cnt = 100000000; /* delay for a while */
		while( (cnt>0)&&(READ_MEM32(BISTCR)&BIST_READY_MASK)!=BIST_READY_PATTERN )
			cnt--;
		if ( cnt==0 )
			rtlglue_printf( "%s():%d READ_MEM32(BISTCR)=0x%08x\n", __FUNCTION__, __LINE__, READ_MEM32(BISTCR) );

		cnt = 100000000; /* delay for a while */
		while( (cnt>0)&&(READ_MEM32(BISTTSDR0)&BISTTSDR0_READY_MASK)!=BISTTSDR0_READY_PATTERN )
			cnt--;
		if ( cnt==0 )
			rtlglue_printf( "%s():%d READ_MEM32(BISTTSDR0)=0x%08x\n", __FUNCTION__, __LINE__, READ_MEM32(BISTTSDR0) );
	}

#endif
#endif
	/* Enable TX/RX After ALL ASIC configurations are done */
	WRITE_MEM32( SIRR, READ_MEM32(SIRR)| TRXRDY );

#ifndef CONFIG_RTL865XC  
	/* 100M TxD latch clock phase */
	WRITE_MEM32(SWTAA, (uint32)PORT6_PHY_CONTROL);
	WRITE_MEM32(TCR0, 0x00000056);
	WRITE_MEM32(SWTACR, CMD_FORCE | ACTION_START); // force add

#if !defined(RTL865X_TEST) && !defined(RTL865X_MODEL_USER)
	while ( (READ_MEM32(SWTACR) & ACTION_MASK) != ACTION_DONE ); /* Wait for command done */
#endif
#endif

#if defined(VERA)||defined(VSV)||defined(MIILIKE)
	/* To speed up vera, we ignore set NAPT default value. */
#else
	/* TCPUDP Table: Set collision bit to 0 */
	memset( &naptTcpUdp, 0, sizeof(naptTcpUdp) );
	naptTcpUdp.isCollision = 1;
	naptTcpUdp.isCollision2 = 1;
	for(flowTblIdx=0; flowTblIdx<RTL8651_TCPUDPTBL_SIZE; flowTblIdx++)
//		rtl8651_setAsicNaptTcpUdpTable( TRUE, flowTblIdx, &naptTcpUdp );
;

	/* ICMP Table: Set collision bit to 1 */
	memset( &naptIcmp, 0, sizeof(naptIcmp) );
	naptIcmp.isCollision = 1;
	for(flowTblIdx=0; flowTblIdx<RTL8651_ICMPTBL_SIZE; flowTblIdx++)
//		rtl8651_setAsicNaptIcmpTable( TRUE, flowTblIdx, &naptIcmp );	
;
	
#endif

	/* Initiate Bandwidth control backward compatible mode : Set all of them to FULL-Rate */
	{
		int32 portIdx, typeIdx;
		_rtl865xB_BandwidthCtrlMultiplier = _RTL865XB_BANDWIDTHCTRL_X1;
		for ( portIdx = 0 ; portIdx < RTL8651_PORT_NUMBER ; portIdx ++ )
		{
			for ( typeIdx = 0 ; typeIdx < _RTL865XB_BANDWIDTHCTRL_CFGTYPE ; typeIdx ++ )
			{
				_rtl865xB_BandwidthCtrlPerPortConfiguration[portIdx][typeIdx] = BW_FULL_RATE;
			}
		}
		/* Sync the configuration to ASIC */
		_rtl8651_syncToAsicEthernetBandwidthControl();
	}
	


	/* ==================================================================================================
		Embedded PHY patch -- According to the designer, internal PHY's parameters need to be adjusted. 
	 ================================================================================================== */
	if(RTL865X_PHY6_DSP_BUG) /*modified by Mark*/
	{
		rtl8651_setAsicEthernetPHYReg( 6, 9, 0x0505 );
		rtl8651_setAsicEthernetPHYReg( 6, 4, 0x1F10 );
		rtl8651_setAsicEthernetPHYReg( 6, 0, 0x1200 );
	}

	/* ===============================
	    =============================== */
	{
		uint32 port;
		uint32 maxPort;

		maxPort =	(rtl8651_tblAsicDrvPara.externalPHYProperty & RTL8651_TBLASIC_EXTPHYPROPERTY_PORT5_RTL8211B)?
					RTL8651_MAC_NUMBER:
					RTL8651_PHY_NUMBER;

		for ( port = 0 ; port < maxPort ; port ++ )
		{
			RTL_ASIC_INFO(	RTL_ASIC_MSG_INIT,
							"Default Port (%d) Flow control RX/TX setting : [ENABLE]", port);
			rtl8651_setAsicFlowControlRegister(port, TRUE);
			rtl865xC_setAsicPortPauseFlowControl(port, TRUE, TRUE);
		}
	}
	
	/* ===============================
	 	(1) Handling port 0.
	    =============================== */	
	rtl8651_restartAsicEthernetPHYNway(0);	/* Restart N-way of port 0 to let embedded phy patch take effect. */

	/* ===============================
	 	(2) Handling port 1 - port 4.
	    =============================== */
	if (rtl8651_tblAsicDrvPara.externalPHYProperty & RTL8651_TBLASIC_EXTPHYPROPERTY_PORT1234_RTL8212)
	{
		
	} else
	{
		/* Restart N-way of port 1 - port 4 to let embedded phy patch take effect. */
		{
			uint32 port;

			/* Restart N-way of port 1 - port 4 */
			for ( port = 1; port < RTL8651_PHY_NUMBER; port++ )
			{
				rtl8651_restartAsicEthernetPHYNway(port);
			}
		}
	}

	/* ===============================
		(3) Handling port 5.
	    =============================== */


	/* =====================
		QoS-related patch 
	    ===================== */
	{
		#define DEFAULT_ILB_UBOUND 0x3FBE  /*added by Mark for suggested Leacky Bucket value*/
		#define DEFAULT_ILB_LBOUND 0x3FBC

		uint32 token, tick, hiThreshold,i;
		rtl8651_getAsicLBParameter( &token, &tick, &hiThreshold );
		hiThreshold = 0x400;	/* New default vlue. */
		rtl8651_setAsicLBParameter( token, tick, hiThreshold );
		/*Mantis(2307): Ingress leaky bucket need to be initized with suggested value . added by mark*/
		WRITE_MEM32( ILBPCR1, DEFAULT_ILB_UBOUND << UpperBound_OFFSET | DEFAULT_ILB_LBOUND << LowerBound_OFFSET );
		for(i=0;i<=(RTL8651_PHY_NUMBER/2);i++) /*Current Token Register is 2 bytes per port*/
			WRITE_MEM32( ILB_CURRENT_TOKEN + 4*i , DEFAULT_ILB_UBOUND << UpperBound_OFFSET | DEFAULT_ILB_UBOUND );
		
	}
	{
			WRITE_MEM32(CPUQDM0, 0x00001111);
			WRITE_MEM32(CPUQDM2, 0x22223333);
			WRITE_MEM32(CPUQDM4, 0x55555555);
	}
	/* Enable Flow Control for each QUEUE / Port */
	{
		int32 port, queue;

		for ( port = PHY0 ; port <= CPU ; port ++ )
		{
			/*	Set the Ingress & Egress bandwidth control NO LIMIT */
			/* Has been done by _rtl8651_syncToAsicEthernetBandwidthControl() */
			/*
			rtl8651_setAsicPortIngressBandwidth(port, (IBWC_ODDPORT_MASK>>IBWC_ODDPORT_OFFSET));
			rtl8651_setAsicPortEgressBandwidth(port, (APR_MASK>>APR_OFFSET));
			*/
			
			for ( queue = QUEUE0 ; queue <= QUEUE5 ; queue ++ )
			{
				rtl8651_setAsicQueueFlowControlConfigureRegister( port, queue, TRUE);
			}
		}
	}

		/* Initiate Queue Management system */
	_rtl865xC_QM_init();

	/*
		Init QUEUE Number configuration for RTL865xC : For Port 0~5 and CPU Port - All ports have 1 queue for each.
	*/
				{
		int32 port;

		rtl865xC_lockSWCore();
		for ( port = PHY0 ; port <= CPU ; port ++)
			{
			rtl8651_setAsicOutputQueueNumber(port, 1	/* According to DataSheet of QNUMCR : All ports use 1 queue by default */);
//			rtl8651_setAsicOutputQueueNumber(port, RTL8651_OUTPUTQUEUE_SIZE	/* For Qos requirement: All ports use 6 queue by default*/);
			}
		rtl865xC_waitForOutputQueueEmpty();
		rtl8651_resetAsicOutputQueue();
		rtl865xC_unLockSWCore();
		}
	/* set default include IFG */
	WRITE_MEM32( QOSFCR, BC_withPIFG_MASK);
		
#if defined(RTL865XC_MNQUEUE_OUTPUTQUEUE)  || defined(RTL865XC_QOS_OUTPUTQUEUE)
		{
		/* Set the threshold value for qos sytem */
		_rtl865x_setQosThresholdByQueueIdx(QNUM_IDX_123);

	/*	clear dscp priority assignment, otherwise, pkt with dscp value 0 will be assign priority 1		*/
		WRITE_MEM32(DSCPCR0,0);
		WRITE_MEM32(DSCPCR1,0);
		WRITE_MEM32(DSCPCR2,0);
		WRITE_MEM32(DSCPCR3,0);
		WRITE_MEM32(DSCPCR4,0);
		WRITE_MEM32(DSCPCR5,0);
		WRITE_MEM32(DSCPCR6,0);			
	}
#endif


	/* DSP bug (PHY-ID for DSP controller is set same as PHY 0 ) in RTL865xC A-Cut */
	if(RTL865X_PHY6_DSP_BUG)
	/* correct the default value of input queue flow control threshold */
		WRITE_MEM32( IQFCTCR, 0xC8 << IQ_DSC_FCON_OFFSET | 0x96 << IQ_DSC_FCOFF_OFFSET );

	if ( RTL865X_IQFCTCR_DEFAULT_VALUE_BUG )
	{
		rtl8651_setAsicSystemInputFlowControlRegister(0xc8, 0x96);	/* Configure the ASIC Input Queue Flow control threshold to the default value ( ASIC default value is opposite from correct ) */
	}


#if defined(RTL865X_TEST) || defined(RTL865X_MODEL_USER)
#if defined(VERA)||defined(VSV)||defined(MIILIKE)
	/* To speed up vera, we ignore set NAPT default value. */
#else
	/* ICMP Table: Set collision bit to 1 */
	memset( &naptIcmp, 0, sizeof(naptIcmp) );
	naptIcmp.isCollision = 1;
	for(flowTblIdx=0; flowTblIdx<RTL8651_ICMPTBL_SIZE; flowTblIdx++)
		rtl8651_setAsicNaptIcmpTable( TRUE, flowTblIdx, &naptIcmp );	
#endif
#endif

	return SUCCESS;
}


/*=========================================
  * ASIC DRIVER API: HASHING FUNCTION
  *=========================================*/
#define RTL865X_ASIC_DRIVER_HASHING_FUNCTION_API

__IRAM_L2_FWD uint32 rtl8651_filterDbIndex(ether_addr_t * macAddr,uint16 fid) {
    return ( macAddr->octet[0] ^ macAddr->octet[1] ^
                    macAddr->octet[2] ^ macAddr->octet[3] ^
                    macAddr->octet[4] ^ macAddr->octet[5] ^fidHashTable[fid]) & 0xFF;
}

//uint32 rtl8651_vlanTableIndex(uint16 vid) {
//	return (vid& (RTL8651_VLAN_NUMBER-1));
//}



int32 rtl8651_setAsicAgingFunction(int8 l2Enable, int8 l4Enable) {
	WRITE_MEM32( TEACR, (READ_MEM32(TEACR) & ~0x3) |(l2Enable == TRUE? 0x0: 0x1) | (l4Enable == TRUE? 0x0 : 0x2));
	return SUCCESS;
}

int32 rtl8651_getAsicAgingFunction(int8 * l2Enable, int8 * l4Enable) {
	if(l2Enable == NULL || l4Enable == NULL)
		return FAILED;

	*l2Enable = (READ_MEM32(TEACR) & 0x1)? FALSE: TRUE;
	*l4Enable = (READ_MEM32(TEACR) & 0x2)? FALSE: TRUE;
	return SUCCESS;
}


/*=========================================
  * ASIC DRIVER API: FLOW CONTROL
  *=========================================*/
#define RTL865X_ASIC_DRIVER_FLOW_CONTROL_API

int32 rtl865xC_dump_flowCtrlRegs(void)
{
	uint32 port, enable;

	rtlglue_printf(">> Flow Control Register:\n\n");
	for ( port = 0; port < RTL8651_PHY_NUMBER; port++ )
	{
		if ( rtl8651_getAsicFlowControlRegister( port, &enable ) != FAILED )
		{
			rtlglue_printf("\tFlow Control for port%u: %s\n", port, ( enable )? "<enable>": "<disable>");
		}
	}

	if (rtl8651_tblAsicDrvPara.externalPHYProperty & RTL8651_TBLASIC_EXTPHYPROPERTY_PORT5_RTL8211B)
	{
		if ( rtl8651_getAsicFlowControlRegister( RTL8651_MII_PORTNUMBER, &enable ) != FAILED )
		{
			rtlglue_printf("\tFlow Control for MII port: %s\n", ( enable )? "<enable>": "<disable>");
		}
	}

	rtlglue_printf("\n");
	return SUCCESS;
}

int32 rtl8651_setAsicFlowControlRegister(uint32 port, uint32 enable)
{
	uint32 phyid, statCtrlReg4;

	if (rtl8651_tblAsicDrvPara.externalPHYProperty & RTL8651_TBLASIC_EXTPHYPROPERTY_PORT5_RTL8211B)
	{
		if ( port > RTL8651_MAC_NUMBER )
		{
			return FAILED;
		}
	} else
	{
		if ( port > RTL8651_PHY_NUMBER )
		{
			return FAILED;
		}
	}
	/* phy id determination */
	phyid = rtl8651AsicEthernetTable[port].phyId;

	/* Read */
	rtl8651_getAsicEthernetPHYReg( phyid, 4, &statCtrlReg4 );

	if ( enable && ( statCtrlReg4 & CAPABLE_PAUSE ) == 0 )
	{
		statCtrlReg4 |= CAPABLE_PAUSE;		
	}
	else if ( enable == 0 && ( statCtrlReg4 & CAPABLE_PAUSE ) )
	{
		statCtrlReg4 &= ~CAPABLE_PAUSE;
	}
	else
		return SUCCESS;	/* The configuration does not change. Do nothing. */

	rtl8651_setAsicEthernetPHYReg( phyid, 4, statCtrlReg4 );
	
	/* restart N-way. */
	rtl8651_restartAsicEthernetPHYNway(port);

	return SUCCESS;
}

int32 rtl8651_getAsicFlowControlRegister(uint32 port, uint32 *enable)
{
	uint32 phyid, statCtrlReg4;

	if (rtl8651_tblAsicDrvPara.externalPHYProperty & RTL8651_TBLASIC_EXTPHYPROPERTY_PORT5_RTL8211B)
	{
		if ( port > RTL8651_MAC_NUMBER )
		{
			return FAILED;
		}
	} else
	{
		if ( port > RTL8651_PHY_NUMBER )
		{
			return FAILED;
		}
	}
	/* phy id determination */
	phyid = rtl8651AsicEthernetTable[port].phyId;

	/* Read */
	rtl8651_getAsicEthernetPHYReg( phyid, 4, &statCtrlReg4 );

	*enable = ( statCtrlReg4 & CAPABLE_PAUSE )? TRUE: FALSE;

	return SUCCESS;
}


/* new APIs */
int32 rtl8651_setAsicSystemBasedFlowControlRegister(uint32 sharedON, uint32 sharedOFF, uint32 fcON, uint32 fcOFF, uint32 drop)
{
	/* Invalid input parameter */
	if ((sharedON > (SDC_FCON_MASK >> SDC_FCON_OFFSET)) || 
		(sharedOFF > (S_DSC_FCOFF_MASK >> S_DSC_FCOFF_OFFSET)) || 
		(fcON > ((S_Max_SBuf_FCON_MASK >> S_Max_SBuf_FCON_OFFSET))) || 
		(fcOFF > (S_Max_SBuf_FCOFF_MASK >> S_Max_SBuf_FCOFF_OFFSET)) || 
		(drop > (S_DSC_RUNOUT_MASK >> S_DSC_RUNOUT_OFFSET)))
		return FAILED; 

	WRITE_MEM32(SBFCR0, (READ_MEM32(SBFCR0) & ~(S_DSC_RUNOUT_MASK)) | (drop << S_DSC_RUNOUT_OFFSET));
	WRITE_MEM32(SBFCR1, (READ_MEM32(SBFCR1) & ~(S_DSC_FCON_MASK | S_DSC_FCOFF_MASK)) | ( fcON<< S_DSC_FCON_OFFSET) | (fcOFF << S_DSC_FCOFF_OFFSET));
	WRITE_MEM32(SBFCR2, (READ_MEM32(SBFCR2) & ~(S_Max_SBuf_FCON_MASK | S_Max_SBuf_FCOFF_MASK)) | (sharedON << S_Max_SBuf_FCON_OFFSET) | (sharedOFF << S_Max_SBuf_FCOFF_OFFSET));
	return SUCCESS;
}

int32 rtl8651_getAsicSystemBasedFlowControlRegister(uint32 *sharedON, uint32 *sharedOFF, uint32 *fcON, uint32 *fcOFF, uint32 *drop)
{
	if (drop != NULL)
		*drop = (READ_MEM32(SBFCR0) & S_DSC_RUNOUT_MASK) >> S_DSC_RUNOUT_OFFSET;
	if (sharedON != NULL)
		*sharedON = (READ_MEM32(SBFCR1) & S_DSC_FCON_MASK) >> S_DSC_FCON_OFFSET;
	if (sharedOFF != NULL)
		*sharedOFF = (READ_MEM32(SBFCR1) & S_DSC_FCOFF_MASK) >> S_DSC_FCOFF_OFFSET;
	if (fcON != NULL)
		*fcON = (READ_MEM32(SBFCR2) & S_Max_SBuf_FCON_MASK) >> S_Max_SBuf_FCON_OFFSET;
	if (fcOFF != NULL)
		*fcOFF = (READ_MEM32(SBFCR2) & S_Max_SBuf_FCOFF_MASK) >> S_Max_SBuf_FCOFF_OFFSET;
	return SUCCESS;
}

/*
@func int32 | rtl8651_setAsicSystemInputFlowControlRegister | Set System input queue flow control register
@parm uint32 | fcON		| Threshold for Flow control OFF
@parm uint32 | fcOFF		| Threshold for Flow control ON
@rvalue SUCCESS | 
@comm
Set input-queue flow control threshold on RTL865xC platform.
 */
int32 rtl8651_setAsicSystemInputFlowControlRegister(uint32 fcON, uint32 fcOFF)
{
	/* Check the correctness */
	if (	(fcON > ( IQ_DSC_FCON_MASK >> IQ_DSC_FCON_OFFSET )) ||
	   	(fcOFF > ( IQ_DSC_FCOFF_MASK >> IQ_DSC_FCOFF_OFFSET ))	)
	{
		return FAILED;
	}

	/* Write the flow control threshold value into ASIC */
	WRITE_MEM32(	IQFCTCR,
			(	(READ_MEM32(IQFCTCR) & ~(IQ_DSC_FCON_MASK | IQ_DSC_FCOFF_MASK)) |
				(fcON << IQ_DSC_FCON_OFFSET) |
				(fcOFF << IQ_DSC_FCOFF_OFFSET))	);
	return SUCCESS;
}
/*
@func int32 | rtl8651_getAsicSystemInputFlowControlRegister | Get System input queue flow control register
@parm uint32* | fcON		| pointer to get Threshold for Flow control OFF
@parm uint32* | fcOFF		| pointer to get Threshold for Flow control ON
@rvalue SUCCESS | 
@comm
Set input-queue flow control threshold on RTL865xC platform.
 */
int32 rtl8651_getAsicSystemInputFlowControlRegister(uint32 *fcON, uint32 *fcOFF)
{
	uint32 iqfctcr;

	iqfctcr = READ_MEM32( IQFCTCR );

	if ( fcON )
	{
		*fcON = ( iqfctcr & IQ_DSC_FCON_MASK ) >> IQ_DSC_FCON_OFFSET;
	}

	if ( fcOFF )
	{
		*fcOFF = ( iqfctcr & IQ_DSC_FCOFF_MASK ) >> IQ_DSC_FCOFF_OFFSET;
	}

	return SUCCESS;
}


int32 rtl8651_setAsicPortBasedFlowControlRegister(enum PORTID port, uint32 fcON, uint32 fcOFF)
{
	/* Invalid input parameter */
	if ((fcON > (P_MaxDSC_FCON_MASK >> P_MaxDSC_FCON_OFFSET)) || 
		(fcOFF > (P_MaxDSC_FCOFF_MASK >> P_MaxDSC_FCOFF_OFFSET)))
		return FAILED; 

	switch (port)
	{
		case PHY0:
			WRITE_MEM32(PBFCR0, (READ_MEM32(PBFCR0) & ~(P_MaxDSC_FCON_MASK | P_MaxDSC_FCOFF_MASK)) | (fcON << P_MaxDSC_FCON_OFFSET) | (fcOFF << P_MaxDSC_FCOFF_OFFSET)); break;
		case PHY1:
			WRITE_MEM32(PBFCR1, (READ_MEM32(PBFCR1) & ~(P_MaxDSC_FCON_MASK | P_MaxDSC_FCOFF_MASK)) | (fcON << P_MaxDSC_FCON_OFFSET) | (fcOFF << P_MaxDSC_FCOFF_OFFSET)); break;
		case PHY2:
			WRITE_MEM32(PBFCR2, (READ_MEM32(PBFCR2) & ~(P_MaxDSC_FCON_MASK | P_MaxDSC_FCOFF_MASK)) | (fcON << P_MaxDSC_FCON_OFFSET) | (fcOFF << P_MaxDSC_FCOFF_OFFSET)); break;
		case PHY3:
			WRITE_MEM32(PBFCR3, (READ_MEM32(PBFCR3) & ~(P_MaxDSC_FCON_MASK | P_MaxDSC_FCOFF_MASK)) | (fcON << P_MaxDSC_FCON_OFFSET) | (fcOFF << P_MaxDSC_FCOFF_OFFSET)); break;
		case PHY4:
			WRITE_MEM32(PBFCR4, (READ_MEM32(PBFCR4) & ~(P_MaxDSC_FCON_MASK | P_MaxDSC_FCOFF_MASK)) | (fcON << P_MaxDSC_FCON_OFFSET) | (fcOFF << P_MaxDSC_FCOFF_OFFSET)); break;
		case PHY5:
			WRITE_MEM32(PBFCR5, (READ_MEM32(PBFCR5) & ~(P_MaxDSC_FCON_MASK | P_MaxDSC_FCOFF_MASK)) | (fcON << P_MaxDSC_FCON_OFFSET) | (fcOFF << P_MaxDSC_FCOFF_OFFSET)); break;
		case CPU:
			WRITE_MEM32(PBFCR6, (READ_MEM32(PBFCR6) & ~(P_MaxDSC_FCON_MASK | P_MaxDSC_FCOFF_MASK)) | (fcON << P_MaxDSC_FCON_OFFSET) | (fcOFF << P_MaxDSC_FCOFF_OFFSET)); break;
		default:
			return FAILED;
	}

	return SUCCESS;
}

int32 rtl8651_getAsicPortBasedFlowControlRegister(enum PORTID port, uint32 *fcON, uint32 *fcOFF)
{
	uint32 regValue;
	
	switch (port)
	{
		case PHY0:
			regValue = READ_MEM32(PBFCR0); break;
		case PHY1:
			regValue = READ_MEM32(PBFCR1); break;
		case PHY2:
			regValue = READ_MEM32(PBFCR2); break;
		case PHY3:
			regValue = READ_MEM32(PBFCR3); break;
		case PHY4:
			regValue = READ_MEM32(PBFCR4); break;
		case PHY5:
			regValue = READ_MEM32(PBFCR5); break;
		case CPU:
			regValue = READ_MEM32(PBFCR6); break;
		default:
			return FAILED;
	}
	
	if (fcON != NULL)
		*fcON = (regValue & P_MaxDSC_FCON_MASK) >> P_MaxDSC_FCON_OFFSET;
	if (fcOFF != NULL)
		*fcOFF = (regValue & P_MaxDSC_FCOFF_MASK) >> P_MaxDSC_FCOFF_OFFSET;
	return SUCCESS;
}

int32 rtl8651_setAsicQueueDescriptorBasedFlowControlRegister(enum PORTID port, enum QUEUEID queue, uint32 fcON, uint32 fcOFF)
{
	/* Invalid input parameter */
	if ((port < PHY0) || (port > CPU))
		return FAILED; 

	if ((fcON > (QG_DSC_FCON_MASK >> QG_DSC_FCON_OFFSET)) || 
		(fcOFF > (QG_DSC_FCOFF_MASK >> QG_DSC_FCOFF_OFFSET)))
		return FAILED; 


	switch (queue)
	{
		case QUEUE0:
			WRITE_MEM32((QDBFCRP0G0+(port*0xC)), (READ_MEM32(QDBFCRP0G0+(port*0xC)) & ~(QG_DSC_FCON_MASK | QG_DSC_FCOFF_MASK)) | (fcON << QG_DSC_FCON_OFFSET) | (fcOFF << QG_DSC_FCOFF_OFFSET)); 		
			break;
		case QUEUE1:
		case QUEUE2:
		case QUEUE3:
		case QUEUE4:		
			WRITE_MEM32((QDBFCRP0G1+(port*0xC)), (READ_MEM32(QDBFCRP0G1+(port*0xC)) & ~(QG_DSC_FCON_MASK | QG_DSC_FCOFF_MASK)) | (fcON << QG_DSC_FCON_OFFSET) | (fcOFF << QG_DSC_FCOFF_OFFSET)); 		
			break;
		case QUEUE5:
			WRITE_MEM32((QDBFCRP0G2+(port*0xC)), (READ_MEM32(QDBFCRP0G2+(port*0xC)) & ~(QG_DSC_FCON_MASK | QG_DSC_FCOFF_MASK)) | (fcON << QG_DSC_FCON_OFFSET) | (fcOFF << QG_DSC_FCOFF_OFFSET)); 		
			break;
		default:
			return FAILED;
	}
	return SUCCESS;
}

int32 rtl8651_getAsicQueueDescriptorBasedFlowControlRegister(enum PORTID port, enum QUEUEID queue, uint32 *fcON, uint32 *fcOFF)
{
	uint32 regValue;
	
	/* Invalid input parameter */
	if ((port < PHY0) || (port > CPU))
		return FAILED; 

	switch (queue)
	{
		case QUEUE0:
			regValue = READ_MEM32(QDBFCRP0G0+(port*0xC));
			break;
		case QUEUE1:
		case QUEUE2:
		case QUEUE3:
		case QUEUE4:		
			regValue = READ_MEM32(QDBFCRP0G1+(port*0xC));
			break;
		case QUEUE5:
			regValue = READ_MEM32(QDBFCRP0G2+(port*0xC));
			break;
		default:
			return FAILED;
	}

	if (fcON != NULL)
		*fcON = (regValue & QG_DSC_FCON_MASK) >> QG_DSC_FCON_OFFSET;
	if (fcOFF != NULL)
		*fcOFF = (regValue & QG_DSC_FCOFF_MASK) >> QG_DSC_FCOFF_OFFSET;
	return SUCCESS;
}


int32 rtl8651_setAsicQueuePacketBasedFlowControlRegister(enum PORTID port, enum QUEUEID queue, uint32 fcON, uint32 fcOFF)
{
	/* Invalid input parameter */
	if ((port < PHY0) || (port > CPU))
		return FAILED; 

	if ((fcON > (QG_QLEN_FCON_MASK>> QG_QLEN_FCON_OFFSET)) || 
		(fcOFF > (QG_QLEN_FCOFF_MASK >> QG_QLEN_FCOFF_OFFSET)))
		return FAILED; 

	switch (queue)
	{
		case QUEUE0:
			WRITE_MEM32((QPKTFCRP0G0+(port*0xC)), (READ_MEM32(QPKTFCRP0G0+(port*0xC)) & ~(QG_QLEN_FCON_MASK | QG_QLEN_FCOFF_MASK)) | (fcON << QG_QLEN_FCON_OFFSET) | (fcOFF << QG_QLEN_FCOFF_OFFSET)); 		
			break;
		case QUEUE1:
		case QUEUE2:
		case QUEUE3:
		case QUEUE4:		
			WRITE_MEM32((QPKTFCRP0G1+(port*0xC)), (READ_MEM32(QPKTFCRP0G1+(port*0xC)) & ~(QG_QLEN_FCON_MASK | QG_QLEN_FCOFF_MASK)) | (fcON << QG_QLEN_FCON_OFFSET) | (fcOFF << QG_QLEN_FCOFF_OFFSET)); 
			break;
		case QUEUE5:
			WRITE_MEM32((QPKTFCRP0G2+(port*0xC)), (READ_MEM32(QPKTFCRP0G2+(port*0xC)) & ~(QG_QLEN_FCON_MASK | QG_QLEN_FCOFF_MASK)) | (fcON << QG_QLEN_FCON_OFFSET) | (fcOFF << QG_QLEN_FCOFF_OFFSET)); 
			break;
		default:
			return FAILED;
	}

	return SUCCESS;
}

int32 rtl8651_getAsicQueuePacketBasedFlowControlRegister(enum PORTID port, enum QUEUEID queue, uint32 *fcON, uint32 *fcOFF)
{
	uint32 regValue;
	
	/* Invalid input parameter */
	if ((port < PHY0) || (port > CPU))
		return FAILED; 

	switch (queue)
	{
		case QUEUE0:
			regValue = READ_MEM32(QPKTFCRP0G0+(port*0xC));
			break;
		case QUEUE1:
		case QUEUE2:
		case QUEUE3:
		case QUEUE4:		
			regValue = READ_MEM32(QPKTFCRP0G1+(port*0xC));
			break;
		case QUEUE5:
			regValue = READ_MEM32(QPKTFCRP0G2+(port*0xC));
			break;
		default:
			return FAILED;
	}

	if (fcON != NULL)
		*fcON = (regValue & QG_QLEN_FCON_MASK) >> QG_QLEN_FCON_OFFSET;
	if (fcOFF != NULL)
		*fcOFF = (regValue & QG_QLEN_FCOFF_MASK) >> QG_QLEN_FCOFF_OFFSET;
	return SUCCESS;
}


int32 rtl8651_setAsicQueueFlowControlConfigureRegister(enum PORTID port, enum QUEUEID queue, uint32 enable)
{
	switch (port)
	{
		case PHY0:
			WRITE_MEM32(FCCR0, (READ_MEM32(FCCR0) & ~(0x1<<(queue+Q_P0_EN_FC_OFFSET))) | (enable << (queue+Q_P0_EN_FC_OFFSET)));  break;
		case PHY1:
			WRITE_MEM32(FCCR0, (READ_MEM32(FCCR0) & ~(0x1<<(queue+Q_P1_EN_FC_OFFSET))) | (enable << (queue+Q_P1_EN_FC_OFFSET)));  break;
		case PHY2:
			WRITE_MEM32(FCCR0, (READ_MEM32(FCCR0) & ~(0x1<<(queue+Q_P2_EN_FC_OFFSET))) | (enable << (queue+Q_P2_EN_FC_OFFSET)));  break;
		case PHY3:
			WRITE_MEM32(FCCR0, (READ_MEM32(FCCR0) & ~(0x1<<(queue+Q_P3_EN_FC_OFFSET))) | (enable << (queue+Q_P3_EN_FC_OFFSET)));  break;
		case PHY4:
			WRITE_MEM32(FCCR1, (READ_MEM32(FCCR1) & ~(0x1<<(queue+Q_P4_EN_FC_OFFSET))) | (enable << (queue+Q_P4_EN_FC_OFFSET)));  break;
		case PHY5:
			WRITE_MEM32(FCCR1, (READ_MEM32(FCCR1) & ~(0x1<<(queue+Q_P5_EN_FC_OFFSET))) | (enable << (queue+Q_P5_EN_FC_OFFSET)));  break;
		case CPU:
			WRITE_MEM32(FCCR1, (READ_MEM32(FCCR1) & ~(0x1<<(queue+Q_P6_EN_FC_OFFSET))) | (enable << (queue+Q_P6_EN_FC_OFFSET)));  break;
		default:
			return FAILED;
	}

	return SUCCESS;
}

int32 rtl8651_getAsicQueueFlowControlConfigureRegister(enum PORTID port, enum QUEUEID queue, uint32 *enable)
{
	if (enable != NULL)
	{
		switch (port)
		{
			case PHY0:
				*enable = (READ_MEM32(FCCR0) & (0x1<<(queue+Q_P0_EN_FC_OFFSET))) >> (queue+Q_P0_EN_FC_OFFSET);  break;
			case PHY1:
				*enable = (READ_MEM32(FCCR0) & (0x1<<(queue+Q_P1_EN_FC_OFFSET))) >> (queue+Q_P1_EN_FC_OFFSET);  break;
			case PHY2:
				*enable = (READ_MEM32(FCCR0) & (0x1<<(queue+Q_P2_EN_FC_OFFSET))) >> (queue+Q_P2_EN_FC_OFFSET);  break;
			case PHY3:
				*enable = (READ_MEM32(FCCR0) & (0x1<<(queue+Q_P3_EN_FC_OFFSET))) >> (queue+Q_P3_EN_FC_OFFSET);  break;
			case PHY4:
				*enable = (READ_MEM32(FCCR1) & (0x1<<(queue+Q_P4_EN_FC_OFFSET))) >> (queue+Q_P4_EN_FC_OFFSET);  break;
			case PHY5:
				*enable = (READ_MEM32(FCCR1) & (0x1<<(queue+Q_P5_EN_FC_OFFSET))) >> (queue+Q_P5_EN_FC_OFFSET);  break;
			case CPU:
				*enable = (READ_MEM32(FCCR1) & (0x1<<(queue+Q_P6_EN_FC_OFFSET))) >> (queue+Q_P6_EN_FC_OFFSET);  break;
			default:
				return FAILED;
		}
	}

	return SUCCESS;
}


int32 rtl8651_setAsicPerQueuePhysicalLengthGapRegister(uint32 gap)
{
	/* Invalid input parameter */
	if (gap > (QLEN_GAP_MASK >> QLEN_GAP_OFFSET))
		return FAILED;

	WRITE_MEM32(PQPLGR, (READ_MEM32(PQPLGR) & ~(QLEN_GAP_MASK)) | (gap << QLEN_GAP_OFFSET)); 		
	return SUCCESS;
}

int32 rtl8651_getAsicPerQueuePhysicalLengthGapRegister(uint32 *gap)
{
	if (gap != NULL)
		*gap = (READ_MEM32(PQPLGR) & QLEN_GAP_MASK) >> QLEN_GAP_OFFSET; 		
	return SUCCESS;
}


/*=========================================
  * ASIC DRIVER API: QoS
  *=========================================*/
#define RTL865X_ASIC_DRIVER_QOS_API

static int32 rtl865xC_outputQueue_HLWeight	=	RTL8651_RR_H_FIRST;
int32 rtl8651_setAsicHLQueueWeight(uint32 weight)
{
	rtl865xC_outputQueue_HLWeight = weight;
	return SUCCESS;
}

int32 rtl8651_getAsicHLQueueWeight(uint32* weight)
{
	*weight = rtl865xC_outputQueue_HLWeight;
	return SUCCESS;
}


int32 rtl8651_getAsicQoSControlRegister(uint32 *qoscr)
{
	if (qoscr == NULL)
		return FAILED;
	*qoscr = READ_MEM32(QOSCR);
	return SUCCESS;
}


int32 rtl8651_setAsicDiffServReg(uint32 dscp, int8 highPriority)
{
	uint32	idx,offset;

	if (dscp>63 || highPriority>7)
		return FAILED;

	idx = dscp/10;
	offset = (dscp%10)*3;
	
	WRITE_MEM32((DSCPCR0+(idx<<2)), ((highPriority&0x7)<<offset)|(READ_MEM32(DSCPCR0+(idx<<2))&(~(0x7<<offset))));
	return SUCCESS;
}

int32 rtl8651_getAsicDiffServReg(uint32 *dscr)
{
	if (dscr == NULL)
		return FAILED;
	dscr[0] = READ_MEM32(DSCPCR0);
	dscr[1] = READ_MEM32(DSCPCR1);
	dscr[2] = READ_MEM32(DSCPCR2);
	dscr[3] = READ_MEM32(DSCPCR3);
	dscr[4] = READ_MEM32(DSCPCR4);
	dscr[5] = READ_MEM32(DSCPCR5);
	dscr[5] = READ_MEM32(DSCPCR6);
	return SUCCESS;
}

int32 rtl8651_getAsicDiffServ(uint32 dscp, int8 *highPriority)
{
	uint32	idx,offset;

	if (dscp>63 || highPriority==NULL)
		return FAILED;

	idx = dscp/10;
	offset = (dscp%10)*3;

	*highPriority = (READ_MEM32(DSCPCR0+(idx<<2))>>offset)&0x7;
	return SUCCESS;
}


/*=========================================
 * ASIC DRIVER API: Protocol-based VLAN
 *=========================================*/

/*
@func int32 | rtl8651_defineProtocolBasedVLAN | configure user-definable protocol-based VLAN
@parm uint32 | ruleNo        | Valid values: RTL8651_PBV_RULE_USR1 and RTL8651_PBV_RULE_USR2
@parm uint8  | ProtocolType  | 00:ethernetII, 01:RFC-1042, 10: LLC-Other, 11:reserved
@parm uint16 | ProtocolValue | ethernetII:ether type, RFC-1042:ether type, LLC-Other:PDSAP(8),SSAP(8)}
@rvalue SUCCESS | 
@comm
 */
int32 rtl8651_defineProtocolBasedVLAN( uint32 ruleNo, uint8 ProtocolType, uint16 ProtocolValue )
{
	assert( ruleNo == RTL8651_PBV_RULE_USR1 ||
	        ruleNo == RTL8651_PBV_RULE_USR2 );

	if ( ruleNo == RTL8651_PBV_RULE_USR1 )
	{
		WRITE_MEM32( PBVCR0, ( ProtocolType << PBVCR_PROTO_TYPE_OFFSET ) |
		                    ( ProtocolValue << PBVCR_PROTO_VALUE_OFFSET ) );
	}
	else if ( ruleNo == RTL8651_PBV_RULE_USR2 )
	{
		WRITE_MEM32( PBVCR1, ( ProtocolType << PBVCR_PROTO_TYPE_OFFSET ) |
		                     ( ProtocolValue << PBVCR_PROTO_VALUE_OFFSET ) );
	}

	return SUCCESS;
}


/*
@func int32 | rtl8651_queryProtocolBasedVLAN | Query user-definable protocol-based VLAN
@parm uint32 | ruleNo        | Valid values: RTL8651_PBV_RULE_USR1 and RTL8651_PBV_RULE_USR2
@parm uint8*  | ProtocolType  | 00:ethernetII, 01:RFC-1042, 10: LLC-Other, 11:reserved
@parm uint16* | ProtocolValue | ethernetII:ether type, RFC-1042:ether type, LLC-Other:PDSAP(8),SSAP(8)}
@rvalue SUCCESS | 
@comm
 */
int32 rtl8651_queryProtocolBasedVLAN( uint32 ruleNo, uint8* ProtocolType, uint16* ProtocolValue )
{
	uint32 regValue=0;
	
	assert( ruleNo == RTL8651_PBV_RULE_USR1 ||
	        ruleNo == RTL8651_PBV_RULE_USR2 );

	if ( ruleNo == RTL8651_PBV_RULE_USR1 )
	{
		regValue = READ_MEM32( PBVCR0 );
	}
	else if ( ruleNo == RTL8651_PBV_RULE_USR2 )
	{
		regValue = READ_MEM32( PBVCR1 );
	}

	if ( ProtocolType ) *ProtocolType = ( regValue & PBVCR_PROTO_TYPE_MASK ) >> PBVCR_PROTO_TYPE_OFFSET;
	if ( ProtocolValue ) *ProtocolValue = ( regValue & PBVCR_PROTO_VALUE_MASK ) >> PBVCR_PROTO_VALUE_OFFSET;

	return SUCCESS;
}


/*
@func int32 | rtl8651_setProtocolBasedVLAN | set corresponding table index of protocol-based VLAN
@parm uint32 | ruleNo  | rule Number (1~6)
@parm uint8  | port    | 0~4:PHY  5:MII  6~8:ExtPort
@parm uint8  | vlanIdx | VLAN Table index (0~7)
@rvalue SUCCESS | 
@comm
 */
int32 rtl8651_setProtocolBasedVLAN( uint32 ruleNo, uint32 port, uint8 valid, uint16 vlanId )
{
	uint32 addr;
	uint32 value;

	assert( ruleNo > 0 && ruleNo < RTL8651_PBV_RULE_MAX );
//	assert( vlanId < RTL865XC_VLAN_NUMBER );
//	assert( port < RTL8651_AGGREGATOR_NUMBER );
	ruleNo = ruleNo-1;
	valid = valid ? TRUE : FALSE;
	if ( valid == FALSE )
	{
		vlanId = 0; // clear it for looking pretty.
	}

	if ( port < RTL865XC_PORT_NUMBER )
	{
		// Port0 ~ Port9
		addr=PBVR0_0 +(ruleNo*5*4) + ((port/2)*4) ;
		value = ( vlanId<<1 | valid );
		if (port&0x1)
			value =(value <<16) | (0x0000FFFF& READ_MEM32(addr));
		else
			value =value|( 0xFFFF0000& READ_MEM32(addr));		
		WRITE_MEM32(addr,value);
	}
	return SUCCESS;
}

/*
@func int32 | rtl8651_getProtocolBasedVLAN | get corresponding table index of protocol-based VLAN
@parm uint32 | ruleNo  | rule Number (1~6)
@parm uint8* | port    | (output) 0~4:PHY  5:MII  6~8:ExtPort
@parm uint8* | vlanIdx | (output) VLAN Table index (0~7)
@rvalue SUCCESS | 
@comm
 */
int32 rtl8651_getProtocolBasedVLAN( uint32 ruleNo, uint32 port, uint8* valid, uint32* vlanId )
{
	uint32 value;

	assert( ruleNo > 0 && ruleNo < RTL8651_PBV_RULE_MAX );
	assert( port < RTL8651_AGGREGATOR_NUMBER );
	ruleNo=ruleNo-1;
	if ( port < RTL865XC_PORT_NUMBER )
	{
		// Port0 ~ Port9		
		value =  READ_MEM32(PBVR0_0+(ruleNo*5*4) +(port/2)*4 );
		if (port&0x1)
			value =(value & 0xffff0000)>>16;
		else
			value &= 0x0000ffff ;
		if ( valid ) *valid = value & 1;
		if ( vlanId ) *vlanId = value >> 1;
	}
	
	assert( *vlanId < RTL865XC_VLAN_NUMBER );

	return SUCCESS;
}



/*=========================================
  * ASIC DRIVER API: Auto MDI/MDIX 
  *=========================================*/
#define RTL865X_ASIC_DRIVER_AUTO_MDI_MDIX_API

int32 rtl8651_autoMdiMdix(uint32 port, uint32 isEnable)
{
	uint32 value;
	
	if (port > 4)
		return FAILED;
	value = READ_MEM32(VLANTCR);

	/* Patch for RTL865xB cut-d only: the bit of MII_enForceMode must inverse when read it. */
	if (RtkHomeGatewayChipRevisionID == 0x03) 
		value = ((~value)&MII_ENFORCE_MODE)  | (value&(~MII_ENFORCE_MODE));
	if (isEnable == FALSE)
		WRITE_MEM32(VLANTCR, value | (1<<(port+27))); /* 1: disable */
	else WRITE_MEM32(VLANTCR, value & ~(1<<(port+27))); /* 0: enable */

#if 0
	if (isEnable == FALSE)
		WRITE_MEM32(VLANTCR, READ_MEM32(VLANTCR) | (1<<(port+27))); /* 1: disable */
	else WRITE_MEM32(VLANTCR, READ_MEM32(VLANTCR) & ~(1<<(port+27))); /* 0: enable */
#endif

	return SUCCESS;
}

int32 rtl8651_getAutoMdiMdix(uint32 port, uint32 *isEnable)
{
	uint32 value;
	if (port > 4 || isEnable == NULL)
		return FAILED;
	value = READ_MEM32(VLANTCR);
	/* Patch for RTL865xB cut-d only: the bit of MII_enForceMode must inverse when read it. */
	if (RtkHomeGatewayChipRevisionID == 0x03) 
		value = ((~value)&MII_ENFORCE_MODE)  | (value&(~MII_ENFORCE_MODE));
	*isEnable = (value & (1<<(port+27)))? FALSE: TRUE;
	return SUCCESS;
}

int32 rtl8651_selectMdiMdix(uint32 port, uint32 isMdi)
{
	uint32 value;
	
	if (port > 4)
		return FAILED;
	value = READ_MEM32(VLANTCR);
	/* Patch for RTL865xB cut-d only: the bit of MII_enForceMode must inverse when read it. */
	if (RtkHomeGatewayChipRevisionID == 0x03) 
		value = ((~value)&MII_ENFORCE_MODE)  | (value&(~MII_ENFORCE_MODE));
	if (isMdi == TRUE)
		WRITE_MEM32(VLANTCR, value | (1<<(port+22)));
	else WRITE_MEM32(VLANTCR, value & ~(1<<(port+22)));

#if 0
	if (isMdi == TRUE)
		WRITE_MEM32(VLANTCR, READ_MEM32(VLANTCR) | (1<<(port+22)));
	else WRITE_MEM32(VLANTCR, READ_MEM32(VLANTCR) & ~(1<<(port+22)));
#endif

	return SUCCESS;
}

int32 rtl8651_getSelectMdiMdix(uint32 port, uint32 *isMdi)
{
	uint32 value;
	if (port > 4 || isMdi == NULL)
		return FAILED;
	value = READ_MEM32(VLANTCR);
	/* Patch for RTL865xB cut-d only: the bit of MII_enForceMode must inverse when read it. */
	if (RtkHomeGatewayChipRevisionID == 0x03) 
		value = ((~value)&MII_ENFORCE_MODE)  | (value&(~MII_ENFORCE_MODE));
	*isMdi = (value & (1<<(port+22)))? TRUE: FALSE;
	return SUCCESS;
}


/*=========================================
  * ASIC DRIVER API: Packet Scheduling Control Register 
  *=========================================*/
#define RTL865X_ASIC_DRIVER_PACKET_SCHEDULING_API

/*
@func int32 | rtl8651_setAsicLBParameter | set Leaky Bucket Paramters
@parm uint32 | token | Token is used for adding budget in each time slot.
@parm uint32 | tick | Tick is used for time slot size slot.
@parm uint32 | hiThreshold | leaky bucket token high-threshold register
@rvalue SUCCESS | 
@rvalue FAILED | invalid parameter
@comm
 */
int32 rtl8651_setAsicLBParameter( uint32 token, uint32 tick, uint32 hiThreshold )
{
	WRITE_MEM32( ELBPCR, (READ_MEM32(ELBPCR) & ~(Token_MASK | Tick_MASK)) | (token << Token_OFFSET) | (tick << Tick_OFFSET));
	WRITE_MEM32( ELBTTCR, (READ_MEM32(ELBTTCR) & ~0xFFFF/*L2_MASK*/) | (hiThreshold << L2_OFFSET));
	WRITE_MEM32( ILBPCR2, (READ_MEM32(ILBPCR2) & ~(ILB_feedToken_MASK|ILB_Tick_MASK)) | (token << ILB_feedToken_OFFSET) | (tick << ILB_Tick_OFFSET) );
	return SUCCESS;
}


/*
@func int32 | rtl8651_getAsicLBParameter | get Leaky Bucket Paramters
@parm uint32* | pToken | pointer to return token
@parm uint32* | pTick | pointer to return tick
@parm uint32* | pHiThreshold | pointer to return hiThreshold
@rvalue SUCCESS | 
@rvalue FAILED | invalid parameter
@comm
 */
int32 rtl8651_getAsicLBParameter( uint32* pToken, uint32* pTick, uint32* pHiThreshold )
{
	uint32 regValue;

	regValue = READ_MEM32(ELBPCR);

	if (pToken != NULL)
		*pToken = (regValue & Token_MASK) >> Token_OFFSET;
	if (pTick != NULL)
		*pTick = (regValue & Tick_MASK) >> Tick_OFFSET;
	if (pHiThreshold != NULL)
		*pHiThreshold = (READ_MEM32(ELBTTCR) & 0xFF) >> L2_OFFSET;
	
	return SUCCESS;
}


/*
@func int32 | rtl8651_setAsicQueueRate | set per queue rate
@parm enum PORTID | port | the port number
@parm enum QUEUEID | queueid | the queue ID wanted to set
@parm uint32 | pprTime | Peak Packet Rate (in times of APR). 0~6: PPR = (2^pprTime)*apr. 7: disable PPR 
@parm uint32 | aprBurstSize | Bucket Burst Size of Average Packet Rate (unit: 1KByte). 0xFF: disable
@parm uint32 | apr | Average Packet Rate (unit: 64Kbps). 0x3FFF: unlimited rate
@rvalue SUCCESS | 
@rvalue FAILED | invalid parameter
@comm
 */
int32 rtl8651_setAsicQueueRate( enum PORTID port, enum QUEUEID queueid, uint32 pprTime, uint32 aprBurstSize, uint32 apr )
{
	uint32 reg1, regValue;

	if ((port < PHY0) || (port > CPU) || (queueid < QUEUE0) || (queueid > QUEUE5))
		return FAILED;

	reg1 = P0Q0RGCR + (port * 0x18) + (queueid * 0x4);  /* offset to get corresponding register */

	regValue = READ_MEM32(reg1) & ~(PPR_MASK | L1_MASK | APR_MASK);
	regValue |= ((pprTime << PPR_OFFSET) | (aprBurstSize << L1_OFFSET) | (apr << APR_OFFSET));
	WRITE_MEM32( reg1, regValue);
	return SUCCESS;
}


/*
@func int32 | rtl8651_getAsicQueueRate | get per queue rate configuration
@parm enum PORTID | port | the port number
@parm enum QUEUEID | queueid | the queue ID wanted to set
@parm uint32* | pPprTime | pointer to Peak Packet Rate (in times of APR). 0~6: PPR = (2^pprTime)*apr. 7: disable PPR 
@parm uint32* | pAprBurstSize | pointer to APR Burst Size (unit: 1KBytes). 0xff: disable
@parm uint32* | pApr | pointer to Average Packet Rate (unit: 64Kbps). 0x3FFF: unlimited rate
@rvalue SUCCESS | 
@rvalue FAILED | invalid parameter
@comm
 */
int32 rtl8651_getAsicQueueRate( enum PORTID port, enum QUEUEID queueid, uint32* pPprTime, uint32* pAprBurstSize, uint32* pApr )
{
	uint32 reg1, regValue;

	if ((port < PHY0) || (port > CPU) || (queueid < QUEUE0) || (queueid > QUEUE5))
		return FAILED;

	reg1 = P0Q0RGCR + (port * 0x18) + (queueid * 0x4);  /* offset to get corresponding register */
	regValue = READ_MEM32(reg1);

	if (pPprTime != NULL)
		*pPprTime = (regValue & PPR_MASK) >> PPR_OFFSET;
	if (pAprBurstSize != NULL)
		*pAprBurstSize = (regValue & L1_MASK) >> L1_OFFSET;
	if (pApr != NULL)
		*pApr = (regValue & APR_MASK) >> APR_OFFSET;
	return SUCCESS;
}

/*
@func int32 | rtl8651_setAsicPortIngressBandwidth | set per-port total ingress bandwidth
@parm enum PORTID | port | the port number
@parm uint32 | bandwidth | the total ingress bandwidth (unit: 16Kbps), 0:disable
@rvalue SUCCESS | 
@rvalue FAILED | invalid parameter
@comm
 */
int32 rtl8651_setAsicPortIngressBandwidth( enum PORTID port, uint32 bandwidth)
{
	uint32 reg1;

	/* For ingress bandwidth control, its only for PHY0 to PHY5 */
	if ((port < PHY0) || (port > PHY5))
		return FAILED;

	reg1 = IBCR0 + ((port / 2) * 0x04);		/* offset to get corresponding register */

	if ( port % 2)
	{	/* ODD-port */
		WRITE_MEM32( reg1, ((READ_MEM32(reg1) & ~(IBWC_ODDPORT_MASK)) | ((bandwidth << IBWC_ODDPORT_OFFSET) & IBWC_ODDPORT_MASK)));
	} else
	{	/* EVEN-port */
		WRITE_MEM32( reg1, ((READ_MEM32(reg1) & ~(IBWC_EVENPORT_MASK)) | ((bandwidth << IBWC_EVENPORT_OFFSET) & IBWC_EVENPORT_MASK)));
	}


	return SUCCESS;
}

/*
@func int32 | rtl8651_getAsicPortIngressBandwidth | get per-port total ingress bandwidth
@parm enum PORTID | port | the port number
@parm uint32* | pBandwidth | pointer to the returned total ingress bandwidth (unit: 16Kbps), 0:disable
@rvalue SUCCESS | 
@rvalue FAILED | invalid parameter
@comm
 */
int32 rtl8651_getAsicPortIngressBandwidth( enum PORTID port, uint32* pBandwidth )
{
	uint32 reg1, regValue;

	/* For ingress bandwidth control, its only for PHY0 to PHY5 */
	if ((port < PHY0) || (port > PHY5))
		return FAILED;

	reg1 = IBCR0 + ((port / 2) * 0x04);		/* offset to get corresponding register */

	regValue = READ_MEM32(reg1);

	if (pBandwidth != NULL)
	{
		*pBandwidth = (port % 2)?
						/* Odd port */((regValue & IBWC_ODDPORT_MASK) >> IBWC_ODDPORT_OFFSET):
						/* Even port */((regValue & IBWC_EVENPORT_MASK) >> IBWC_EVENPORT_OFFSET);
	}

	return SUCCESS;
}


/*
@func int32 | rtl8651_setAsicPortEgressBandwidth | set per-port total egress bandwidth
@parm enum PORTID | port | the port number
@parm uint32 | bandwidth | the total egress bandwidth (unit: 64kbps). 0x3FFF: disable 
@rvalue SUCCESS | 
@rvalue FAILED | invalid parameter
@comm
 */
int32 rtl8651_setAsicPortEgressBandwidth( enum PORTID port, uint32 bandwidth )
{
	uint32 reg1;

	if ((port < PHY0) || (port > CPU))
		return FAILED;

	reg1 = WFQRCRP0 + (port * 0xC);  /* offset to get corresponding register */
	WRITE_MEM32( reg1, (READ_MEM32(reg1) & ~(APR_MASK)) | (bandwidth << APR_OFFSET));

	return SUCCESS;
}


/*
@func int32 | rtl8651_getAsicPortEgressBandwidth | get per-port total egress bandwidth
@parm enum PORTID | port | the port number
@parm uint32* | pBandwidth | pointer to the returned total egress bandwidth (unit: 64kbps). 0x3FFF: disable 
@rvalue SUCCESS | 
@rvalue FAILED | invalid parameter
@comm
 */
int32 rtl8651_getAsicPortEgressBandwidth( enum PORTID port, uint32* pBandwidth )
{
	uint32 reg1, regValue;

	if ((port < PHY0) || (port > CPU))
		return FAILED;

	reg1 = WFQRCRP0 + (port * 0xC);  /* offset to get corresponding register */
	regValue = READ_MEM32(reg1);

	if (pBandwidth != NULL)
		*pBandwidth = (regValue & APR_MASK) >> APR_OFFSET;

	return SUCCESS;
}


/*
@func int32 | rtl8651_setAsicQueueWeight | set WFQ weighting
@parm enum PORTID | port | the port number
@parm enum QUEUEID | queueid | the queue ID wanted to set
@parm enum QUEUETYPE | queueType | the specified queue type
@parm uint32 | weight | the weight value wanted to set (valid:0~127)
@rvalue SUCCESS | 
@rvalue FAILED | invalid parameter
@comm
 */
int32 rtl8651_setAsicQueueWeight( enum PORTID port, enum QUEUEID queueid, enum QUEUETYPE queueType, uint32 weight )
{
	uint32 reg1, regOFFSET, regValue;
	
	if ((port < PHY0) || (port > CPU) || (queueid < QUEUE0) || (queueid > QUEUE5))
		return FAILED;
	if ((queueType < STR_PRIO) || (queueType > WFQ_PRIO))
		return FAILED;

	reg1 = WFQWCR0P0 + (port * 0xC) + ((queueid >> 2) * 0x4);	/* offset to get corresponding register */
	regOFFSET = (queueid % 4) * 0x8;	/* used to offset register value */	 	 

	regValue = READ_MEM32(reg1) & ~((WEIGHT0_MASK | SCHE0_MASK) << regOFFSET);
	regValue |= ((queueType << (SCHE0_OFFSET + regOFFSET)) | (weight << (WEIGHT0_OFFSET + regOFFSET)));
	WRITE_MEM32( reg1, regValue);
	return SUCCESS;
}


/*
@func int32 | rtl8651_getAsicQueueWeight | get WFQ weighting
@parm enum PORTID | port | the port number
@parm enum QUEUEID | queueid | the queue ID wanted to set
@parm enum QUEUETYPE* | pQueueType | pointer to the returned queue type
@parm uint32* | pWeight | pointer to the returned weight value
@rvalue SUCCESS | 
@rvalue FAILED | invalid parameter
@comm
 */
int32 rtl8651_getAsicQueueWeight( enum PORTID port, enum QUEUEID queueid, enum QUEUETYPE *pQueueType, uint32 *pWeight )
{
	uint32 reg1, regOFFSET, regValue;
	
	if ((port < PHY0) || (port > CPU) || (queueid < QUEUE0) || (queueid > QUEUE5))
		return FAILED;

	reg1 = WFQWCR0P0 + (port * 0xC) + ((queueid >> 2) * 0x4);  /* offset to get corresponding register */
	regOFFSET = (queueid % 4) * 0x8;	/* used to offset register value */	 
	regValue = READ_MEM32(reg1);

	if (pQueueType != NULL)
		*pQueueType = ((regValue & (SCHE0_MASK << regOFFSET)) >> SCHE0_OFFSET) >> regOFFSET;
	if (pWeight != NULL)
		*pWeight = ((regValue & (WEIGHT0_MASK << regOFFSET)) >> WEIGHT0_OFFSET) >> regOFFSET;

	return SUCCESS;
}



/*=========================================
  * ASIC DRIVER API: Remarking Control Register 
  *=========================================*/
#define RTL865X_ASIC_DRIVER_REMARKING_API


/*
@func int32 | rtl8651_setAsicDot1pRemarkingAbility | set 802.1p remarking ability for a port
@parm enum PORTID | port | the port number (physical port: 0~5, extension port: 6~8)
@parm uint32 | isEnabled | 1: enabled, 0: disabled.
@rvalue SUCCESS | 
@rvalue FAILED | invalid parameter
@comm
 */
int32 rtl8651_setAsicDot1pRemarkingAbility( enum PORTID port, uint32 isEnable )
{
	uint32 con9bit;

	/* Invalid input parameter */
	if (((port < PHY0) || (port > EXT3))) 
		return FAILED;
	
	/* Invalid input parameter */
	if ((isEnable != 0) && (isEnable != 1))
		return FAILED; 

	con9bit = (READ_MEM32(RMCR1P) >> RM1P_EN_OFFSET) | ((READ_MEM32(DSCPRM0) >> RM1P_EN_8_OFFSET) << 8);

	if (isEnable == TRUE) 
		con9bit |= (1 << port);
	else                  
		con9bit &= ~(1 << port);

	WRITE_MEM32(RMCR1P, (READ_MEM32(RMCR1P) & ~(RM1P_EN_MASK)) | ((con9bit & 0xFF) << RM1P_EN_OFFSET));
	WRITE_MEM32(DSCPRM0, (READ_MEM32(DSCPRM0) & ~(RM1P_EN_8)) | (((con9bit & 0x100) >> 8) << RM1P_EN_8_OFFSET));

	return SUCCESS;
}


/*
@func int32 | rtl8651_getAsicDot1pRemarkingAbility | get 802.1p remarking ability for a port
@parm enum PORTID | port | the port number (physical port: 0~5, extension port: 6~8)
@parm uint32* | isEnable | 1: enabled, 0: disabled.
@rvalue SUCCESS | 
@rvalue FAILED | invalid parameter
@comm
 */
int32 rtl8651_getAsicDot1pRemarkingAbility( enum PORTID port, uint32* isEnable )
{
	uint32 regValue;
	
	/* Invalid input parameter */
	if ((port < PHY0) || (port > EXT3)) 
		return FAILED;

	regValue = (READ_MEM32(RMCR1P) >> RM1P_EN_OFFSET) | ((READ_MEM32(DSCPRM0) >> RM1P_EN_8_OFFSET) << 8);

	if (isEnable != NULL)
		*isEnable = (regValue >> port) & 1;

	return SUCCESS;
}




/*
@func int32 | rtl8651_setAsicDot1pRemarkingParameter | set 802.1p remarking parameter
@parm enum PRIORITYVALUE | priority | priority value
@parm enum PRIORITYVALUE | newpriority | new priority value
@rvalue SUCCESS | 
@rvalue FAILED | invalid parameter
@comm
 */
int32 rtl8651_setAsicDot1pRemarkingParameter( enum PRIORITYVALUE priority, enum PRIORITYVALUE newpriority )
{
	uint32 regValue;
	
	/* Invalid input parameter */
	if ((priority < PRI0) || (priority > PRI7) || (newpriority < PRI0) || (newpriority > PRI7))
		return FAILED; 

	regValue = READ_MEM32(RMCR1P);
	switch (priority)
	{
		case PRI0:
			WRITE_MEM32(RMCR1P, (regValue & ~(RM0_1P_MASK)) | (newpriority << RM0_1P_OFFSET));  break;
		case PRI1:
			WRITE_MEM32(RMCR1P, (regValue & ~(RM1_1P_MASK)) | (newpriority << RM1_1P_OFFSET));  break;
		case PRI2:
			WRITE_MEM32(RMCR1P, (regValue & ~(RM2_1P_MASK)) | (newpriority << RM2_1P_OFFSET));  break;
		case PRI3:
			WRITE_MEM32(RMCR1P, (regValue & ~(RM3_1P_MASK)) | (newpriority << RM3_1P_OFFSET));  break;
		case PRI4:
			WRITE_MEM32(RMCR1P, (regValue & ~(RM4_1P_MASK)) | (newpriority << RM4_1P_OFFSET));  break;
		case PRI5:
			WRITE_MEM32(RMCR1P, (regValue & ~(RM5_1P_MASK)) | (newpriority << RM5_1P_OFFSET));  break;
		case PRI6:
			WRITE_MEM32(RMCR1P, (regValue & ~(RM6_1P_MASK)) | (newpriority << RM6_1P_OFFSET));  break;
		case PRI7:
			WRITE_MEM32(RMCR1P, (regValue & ~(RM7_1P_MASK)) | (newpriority << RM7_1P_OFFSET));  break;
	}

	return SUCCESS;
}


/*
@func int32 | rtl8651_getAsicDot1pRemarkingParameter | get 802.1p remarking parameter
@parm enum PRIORITYVALUE | priority | priority value
@parm enum PRIORITYVALUE *| pNewpriority | it will return the new priority value of a specified priority
@rvalue SUCCESS | 
@rvalue FAILED | invalid parameter
@comm
 */
int32 rtl8651_getAsicDot1pRemarkingParameter( enum PRIORITYVALUE priority, enum PRIORITYVALUE *pNewpriority )
{
	uint32 regValue;

	/* Invalid input parameter */
	if ((priority < PRI0) || (priority > PRI7))
		return FAILED; 

	regValue = READ_MEM32(RMCR1P);
	if (pNewpriority != NULL)
	{
		switch (priority)
		{
			case PRI0:
				*pNewpriority = (regValue & RM0_1P_MASK) >> RM0_1P_OFFSET;  break;
			case PRI1:
				*pNewpriority = (regValue & RM1_1P_MASK) >> RM1_1P_OFFSET;  break;
			case PRI2:
				*pNewpriority = (regValue & RM2_1P_MASK) >> RM2_1P_OFFSET;  break;
			case PRI3:
				*pNewpriority = (regValue & RM3_1P_MASK) >> RM3_1P_OFFSET;  break;
			case PRI4:
				*pNewpriority = (regValue & RM4_1P_MASK) >> RM4_1P_OFFSET;  break;
			case PRI5:
				*pNewpriority = (regValue & RM5_1P_MASK) >> RM5_1P_OFFSET;  break;
			case PRI6:
				*pNewpriority = (regValue & RM6_1P_MASK) >> RM6_1P_OFFSET;  break;
			case PRI7:
				*pNewpriority = (regValue & RM7_1P_MASK) >> RM7_1P_OFFSET;  break;
		}
	}

	return SUCCESS;
}

/*
@func int32 | rtl8651_flushAsicDot1pRemarkingParameter | set 802.1p remarking parameter the default value 0
@parm void
@rvalue SUCCESS | 
@comm
 */
int32 rtl8651_flushAsicDot1pRemarkingParameter(void)
{
	WRITE_MEM32(RMCR1P, 0);
	return SUCCESS;
}



/*
@func int32 | rtl8651_setAsicDscpRemarkingAbility | set DSCP remarking ability
@parm enum PORTID | port | the port number (physical port: 0~5, extension port: 6~8)
@parm uint32* | isEnable | 1: enabled, 0: disabled.
@rvalue SUCCESS | 
@rvalue FAILED | invalid parameter
@comm
 */
int32 rtl8651_setAsicDscpRemarkingAbility( enum PORTID port, uint32 isEnable )
{
	/* Invalid input parameter */
	if ((port < PHY0) || (port > EXT3)) 
		return FAILED;
	
	/* Invalid input parameter */
	if ((isEnable != TRUE) && (isEnable != FALSE))
		return FAILED; 

	WRITE_MEM32(DSCPRM1, isEnable == TRUE ? (READ_MEM32(DSCPRM1) | ((1 << port) << DSCPRM_EN_OFFSET)):
											(READ_MEM32(DSCPRM1) & ~((1 << port) << DSCPRM_EN_OFFSET)));  

	return SUCCESS;
}



/*
@func int32 | rtl8651_getAsicDscpRemarkingAbility | get DSCP remarking ability
@parm enum PORTID | port | the port number (physical port: 0~5, extension port: 6~8)
@parm uint32* | isEnable | 1: enabled, 0: disabled.
@rvalue SUCCESS | 
@rvalue FAILED | invalid parameter
@comm
 */
int32 rtl8651_getAsicDscpRemarkingAbility( enum PORTID port, uint32* isEnable )
{
	/* Invalid input parameter */
	if ((port < PHY0) || (port > EXT3)) 
		return FAILED;

	if (isEnable != NULL)
		*isEnable = ((READ_MEM32(DSCPRM1) >> DSCPRM_EN_OFFSET) >> port) & 1;

	return SUCCESS;
}



/*
@func int32 | rtl8651_setAsicDot1pRemarking | set 802.1p remarking parameter
@parm enum PRIORITYVALUE | priority | priority value
@parm uint32 | newdscp | new DSCP value
@rvalue SUCCESS | 
@rvalue FAILED | invalid parameter
@comm
 */
int32 rtl8651_setAsicDscpRemarkingParameter( enum PRIORITYVALUE priority, uint32 newdscp )
{
	uint32 regValue;
	
	/* Invalid input parameter */
	if ((priority < PRI0) || (priority > PRI7))
		return FAILED; 
	
	if ((newdscp < 0) || (newdscp > 63))
		return FAILED; 

	switch (priority)
	{
		case PRI0:
			regValue = READ_MEM32(DSCPRM0);
			WRITE_MEM32(DSCPRM0, (regValue & ~(DSCPRM0_MASK)) | (newdscp << DSCPRM0_OFFSET));  
			break;
		case PRI1:
			regValue = READ_MEM32(DSCPRM0);
			WRITE_MEM32(DSCPRM0, (regValue & ~(DSCPRM1_MASK)) | (newdscp << DSCPRM1_OFFSET));  
			break;
		case PRI2:
			regValue = READ_MEM32(DSCPRM0);
			WRITE_MEM32(DSCPRM0, (regValue & ~(DSCPRM2_MASK)) | (newdscp << DSCPRM2_OFFSET));  
			break;
		case PRI3:
			regValue = READ_MEM32(DSCPRM0);
			WRITE_MEM32(DSCPRM0, (regValue & ~(DSCPRM3_MASK)) | (newdscp << DSCPRM3_OFFSET));  
			break;
		case PRI4:
			regValue = READ_MEM32(DSCPRM0);
			WRITE_MEM32(DSCPRM0, (regValue & ~(DSCPRM4_MASK)) | (newdscp << DSCPRM4_OFFSET));  
			break;
		case PRI5:
			regValue = READ_MEM32(DSCPRM1);
			WRITE_MEM32(DSCPRM1, (regValue & ~(DSCPRM5_MASK)) | (newdscp << DSCPRM5_OFFSET));  
			break;
		case PRI6:
			regValue = READ_MEM32(DSCPRM1);
			WRITE_MEM32(DSCPRM1, (regValue & ~(DSCPRM6_MASK)) | (newdscp << DSCPRM6_OFFSET));  
			break;
		case PRI7:
			regValue = READ_MEM32(DSCPRM1);
			WRITE_MEM32(DSCPRM1, (regValue & ~(DSCPRM7_MASK)) | (newdscp << DSCPRM7_OFFSET));  
			break;
	}

	return SUCCESS;
}



/*
@func int32 | rtl8651_getAsicDot1pRemarking | get 802.1p remarking parameter
@parm enum PRIORITYVALUE | priority | priority value
@parm uint32* | pNewdscp | it will return the new DSCP value of the specified priority
@rvalue SUCCESS | 
@rvalue FAILED | invalid parameter
@comm
 */
int32 rtl8651_getAsicDscpRemarkingParameter( enum PRIORITYVALUE priority, uint32* pNewdscp )
{
	uint32 regValue;

	/* Invalid input parameter */
	if ((priority < PRI0) || (priority > PRI7))
		return FAILED; 

	if (pNewdscp != NULL)
	{
		switch (priority)
		{
			case PRI0:
				regValue = READ_MEM32(DSCPRM0);
				*pNewdscp = (regValue & DSCPRM0_MASK) >> DSCPRM0_OFFSET;  
				break;
			case PRI1:
				regValue = READ_MEM32(DSCPRM0);
				*pNewdscp = (regValue & DSCPRM1_MASK) >> DSCPRM1_OFFSET;  
				break;
			case PRI2:
				regValue = READ_MEM32(DSCPRM0);
				*pNewdscp = (regValue & DSCPRM2_MASK) >> DSCPRM2_OFFSET;  
				break;
			case PRI3:
				regValue = READ_MEM32(DSCPRM0);
				*pNewdscp = (regValue & DSCPRM3_MASK) >> DSCPRM3_OFFSET;  
				break;
			case PRI4:
				regValue = READ_MEM32(DSCPRM0);
				*pNewdscp = (regValue & DSCPRM4_MASK) >> DSCPRM4_OFFSET;  
				break;
			case PRI5:
				regValue = READ_MEM32(DSCPRM1);
				*pNewdscp = (regValue & DSCPRM5_MASK) >> DSCPRM5_OFFSET;  
				break;
			case PRI6:
				regValue = READ_MEM32(DSCPRM1);
				*pNewdscp = (regValue & DSCPRM6_MASK) >> DSCPRM6_OFFSET;  
				break;
			case PRI7:
				regValue = READ_MEM32(DSCPRM1);
				*pNewdscp = (regValue & DSCPRM7_MASK) >> DSCPRM7_OFFSET;  
				break;
		}
	}

	return SUCCESS;
}


/*
@func int32 | rtl8651_flushAsicDscpRemarkingParameter | set dscp remarking parameter the default value 0
@parm void
@rvalue SUCCESS | 
@comm
 */
int32 rtl8651_flushAsicDscpRemarkingParameter(void)
{
	WRITE_MEM32(DSCPRM0, 0);
	WRITE_MEM32(DSCPRM1, 0);
	return SUCCESS;
}





/*=========================================
  * ASIC DRIVER API: Priority Assignment Control Register 
  *=========================================*/
#define RTL865X_ASIC_DRIVER_PRIORITY_ASSIGNMENT_API

/*
@func int32 | rtl8651_setAsicPriorityDecision | set priority selection
@parm uint32 | portpri | output queue decision priority assign for Port Based Priority.
@parm uint32 | dot1qpri | output queue decision priority assign for 1Q Based Priority.
@parm uint32 | dscppri | output queue decision priority assign for DSCP Based Priority
@parm uint32 | aclpri | output queue decision priority assign for ACL Based Priority.
@parm uint32 | natpri | output queue decision priority assign for NAT Based Priority.
@rvalue SUCCESS | 
@rvalue FAILED | invalid parameter
@comm
 */
int32 rtl8651_setAsicPriorityDecision( uint32 portpri, uint32 dot1qpri, uint32 dscppri, uint32 aclpri, uint32 natpri )
{
	/* Invalid input parameter */
	if ((portpri < 0) || (portpri > 0xF) || (dot1qpri < 0) || (dot1qpri > 0xF) || 
		(dscppri < 0) || (dscppri > 0xF) || (aclpri < 0) || (aclpri > 0xF) ||
		(natpri < 0) || (natpri > 0xF)) 
		return FAILED;

	WRITE_MEM32(QIDDPCR, (portpri << PBP_PRI_OFFSET) | (dot1qpri << BP8021Q_PRI_OFFSET) | 
		                 (dscppri << DSCP_PRI_OFFSET) | (aclpri << ACL_PRI_OFFSET) | 
		                 (natpri << NAPT_PRI_OFFSET));

	return SUCCESS;
}


/*
@func int32 | rtl8651_getAsicPriorityDecision | get priority selection
@parm uint32* | pPortpri | output queue decision priority assign for Port Based Priority.
@parm uint32* | pDot1qpri | output queue decision priority assign for 1Q Based Priority.
@parm uint32* | pDscppri | output queue decision priority assign for DSCP Based Priority
@parm uint32* | pAclpri | output queue decision priority assign for ACL Based Priority.
@parm uint32* | pNatpri | output queue decision priority assign for NAT Based Priority.
@rvalue SUCCESS | 
@rvalue FAILED | invalid parameter
@comm
 */
int32 rtl8651_getAsicPriorityDecision( uint32* pPortpri, uint32* pDot1qpri, uint32* pDscppri, uint32* pAclpri, uint32* pNatpri )
{
	uint32 regValue;
	
	regValue = READ_MEM32(QIDDPCR);

	if (pPortpri != NULL)
		*pPortpri = (regValue & PBP_PRI_MASK) >> PBP_PRI_OFFSET;
	if (pDot1qpri != NULL)
		*pDot1qpri = (regValue & BP8021Q_PRI_MASK) >> BP8021Q_PRI_OFFSET;
	if (pDscppri != NULL)
		*pDscppri = (regValue & DSCP_PRI_MASK) >> DSCP_PRI_OFFSET;
	if (pAclpri != NULL)
		*pAclpri = (regValue & ACL_PRI_MASK) >> ACL_PRI_OFFSET;
	if (pNatpri != NULL)
		*pNatpri = (regValue & NAPT_PRI_MASK) >> NAPT_PRI_OFFSET;

	return SUCCESS;
}


/*
@func int32 | rtl8651_setAsicPortPriority | set port based priority
@parm enum PORTID | port | the port number (valid: physical ports(0~5) and extension ports(7~9) )
@parm enum PRIORITYVALUE | priority | priority value.
@rvalue SUCCESS | 
@rvalue FAILED | invalid parameter
@comm
 */
int32 rtl8651_setAsicPortPriority( enum PORTID port, enum PRIORITYVALUE priority )
{
	/* Invalid input parameter */
	if ((priority < PRI0) || (priority > PRI7))
		return FAILED; 

	switch (port)
	{
		case PHY0:
			WRITE_MEM32(PBPCR, (READ_MEM32(PBPCR) & ~(PBPRI_P0_MASK)) | (priority << PBPRI_P0_OFFSET)); break;
		case PHY1:
			WRITE_MEM32(PBPCR, (READ_MEM32(PBPCR) & ~(PBPRI_P1_MASK)) | (priority << PBPRI_P1_OFFSET)); break;
		case PHY2:
			WRITE_MEM32(PBPCR, (READ_MEM32(PBPCR) & ~(PBPRI_P2_MASK)) | (priority << PBPRI_P2_OFFSET)); break;
		case PHY3:
			WRITE_MEM32(PBPCR, (READ_MEM32(PBPCR) & ~(PBPRI_P3_MASK)) | (priority << PBPRI_P3_OFFSET)); break;
		case PHY4:
			WRITE_MEM32(PBPCR, (READ_MEM32(PBPCR) & ~(PBPRI_P4_MASK)) | (priority << PBPRI_P4_OFFSET)); break;
		case PHY5:
			WRITE_MEM32(PBPCR, (READ_MEM32(PBPCR) & ~(PBPRI_P5_MASK)) | (priority << PBPRI_P5_OFFSET)); break;
		case EXT1:
			WRITE_MEM32(PBPCR, (READ_MEM32(PBPCR) & ~(PBPRI_P6_MASK)) | (priority << PBPRI_P6_OFFSET)); break;
		case EXT2:
			WRITE_MEM32(PBPCR, (READ_MEM32(PBPCR) & ~(PBPRI_P7_MASK)) | (priority << PBPRI_P7_OFFSET)); break;
		case EXT3:
			WRITE_MEM32(PBPCR, (READ_MEM32(PBPCR) & ~(PBPRI_P8_MASK)) | (priority << PBPRI_P8_OFFSET)); break;
		case CPU: /* fall thru */
		default:
			return FAILED;
	}

	return SUCCESS;
}


/*
@func int32 | rtl8651_getAsicPortPriority | get port based priority
@parm enum PORTID | port | the port number (valid: physical ports(0~5) and extension ports(7~9) )
@parm enum PRIORITYVALUE* | pPriority | pPriority will return the priority of the specified port.
@rvalue SUCCESS | 
@rvalue FAILED | invalid parameter
@comm
 */
#ifdef CONFIG_RTL865X_KERNEL_MIPS16_ROMEDRIVER
__NOMIPS16
#endif 
int32 rtl8651_getAsicPortPriority( enum PORTID port, enum PRIORITYVALUE *pPriority )
{
	if (pPriority != NULL) 
	{
		switch (port)
		{
			case PHY0:
				*pPriority = (READ_MEM32(PBPCR) & PBPRI_P0_MASK) >> PBPRI_P0_OFFSET;  break;
			case PHY1:
				*pPriority = (READ_MEM32(PBPCR) & PBPRI_P1_MASK) >> PBPRI_P1_OFFSET;  break;
			case PHY2:
				*pPriority = (READ_MEM32(PBPCR) & PBPRI_P2_MASK) >> PBPRI_P2_OFFSET;  break;
			case PHY3:
				*pPriority = (READ_MEM32(PBPCR) & PBPRI_P3_MASK) >> PBPRI_P3_OFFSET;  break;
			case PHY4:
				*pPriority = (READ_MEM32(PBPCR) & PBPRI_P4_MASK) >> PBPRI_P4_OFFSET;  break;
			case PHY5:
				*pPriority = (READ_MEM32(PBPCR) & PBPRI_P5_MASK) >> PBPRI_P5_OFFSET;  break;
			case EXT1:
				*pPriority = (READ_MEM32(PBPCR) & PBPRI_P6_MASK) >> PBPRI_P6_OFFSET;  break;
			case EXT2:
				*pPriority = (READ_MEM32(PBPCR) & PBPRI_P7_MASK) >> PBPRI_P7_OFFSET;  break;
			case EXT3:
				*pPriority = (READ_MEM32(PBPCR) & PBPRI_P8_MASK) >> PBPRI_P8_OFFSET;  break;
			case CPU: /* fall thru */
			default:
				return FAILED;
		}
	}

	return SUCCESS;
}


/*
@func int32 | rtl8651_setAsicDot1qAbsolutelyPriority | set 802.1Q absolutely priority
@parm enum PRIORITYVALUE | srcpriority | priority value
@parm enum PRIORITYVALUE | priority | absolute priority value 
@rvalue SUCCESS | 
@rvalue FAILED | invalid parameter
@comm
 */
int32 rtl8651_setAsicDot1qAbsolutelyPriority( enum PRIORITYVALUE srcpriority, enum PRIORITYVALUE priority )
{
	/* Invalid input parameter */
	if ((srcpriority < PRI0) || (srcpriority > PRI7) || (priority < PRI0) || (priority > PRI7)) 
		return FAILED;

	switch (srcpriority)
	{
		case PRI0:
			WRITE_MEM32(LPTM8021Q, (READ_MEM32(LPTM8021Q) & ~(EN_8021Q2LTMPRI0_MASK)) | (priority << EN_8021Q2LTMPRI0)); break;
		case PRI1:
			WRITE_MEM32(LPTM8021Q, (READ_MEM32(LPTM8021Q) & ~(EN_8021Q2LTMPRI1_MASK)) | (priority << EN_8021Q2LTMPRI1)); break;
		case PRI2:
			WRITE_MEM32(LPTM8021Q, (READ_MEM32(LPTM8021Q) & ~(EN_8021Q2LTMPRI2_MASK)) | (priority << EN_8021Q2LTMPRI2)); break;
		case PRI3:
			WRITE_MEM32(LPTM8021Q, (READ_MEM32(LPTM8021Q) & ~(EN_8021Q2LTMPRI3_MASK)) | (priority << EN_8021Q2LTMPRI3)); break;
		case PRI4:
			WRITE_MEM32(LPTM8021Q, (READ_MEM32(LPTM8021Q) & ~(EN_8021Q2LTMPRI4_MASK)) | (priority << EN_8021Q2LTMPRI4)); break;
		case PRI5:
			WRITE_MEM32(LPTM8021Q, (READ_MEM32(LPTM8021Q) & ~(EN_8021Q2LTMPRI5_MASK)) | (priority << EN_8021Q2LTMPRI5)); break;
		case PRI6:
			WRITE_MEM32(LPTM8021Q, (READ_MEM32(LPTM8021Q) & ~(EN_8021Q2LTMPRI6_MASK)) | (priority << EN_8021Q2LTMPRI6)); break;
		case PRI7:
			WRITE_MEM32(LPTM8021Q, (READ_MEM32(LPTM8021Q) & ~(EN_8021Q2LTMPRI7_MASK)) | (priority << EN_8021Q2LTMPRI7)); break;
	}

	return SUCCESS;
}


/*
@func int32 | rtl8651_getAsicDot1qAbsolutelyPriority | get 802.1Q absolutely priority 
@parm enum PRIORITYVALUE | srcpriority | priority value
@parm enum PRIORITYVALUE* | pPriority | pPriority will return the absolute priority value 
@rvalue SUCCESS | 
@rvalue FAILED | invalid parameter
@comm
 */
int32 rtl8651_getAsicDot1qAbsolutelyPriority( enum PRIORITYVALUE srcpriority, enum PRIORITYVALUE *pPriority )
{

	/* Invalid input parameter */
	if ((srcpriority < PRI0) || (srcpriority > PRI7)) 
		return FAILED;

	if (pPriority != NULL)
	{
		switch (srcpriority)
		{
			case PRI0:
				*pPriority = (READ_MEM32(LPTM8021Q) & EN_8021Q2LTMPRI0_MASK) >> EN_8021Q2LTMPRI0;  break;
			case PRI1:
				*pPriority = (READ_MEM32(LPTM8021Q) & EN_8021Q2LTMPRI1_MASK) >> EN_8021Q2LTMPRI1;  break;
			case PRI2:
				*pPriority = (READ_MEM32(LPTM8021Q) & EN_8021Q2LTMPRI2_MASK) >> EN_8021Q2LTMPRI2;  break;
			case PRI3:
				*pPriority = (READ_MEM32(LPTM8021Q) & EN_8021Q2LTMPRI3_MASK) >> EN_8021Q2LTMPRI3;  break;
			case PRI4:
				*pPriority = (READ_MEM32(LPTM8021Q) & EN_8021Q2LTMPRI4_MASK) >> EN_8021Q2LTMPRI4;  break;
			case PRI5:
				*pPriority = (READ_MEM32(LPTM8021Q) & EN_8021Q2LTMPRI5_MASK) >> EN_8021Q2LTMPRI5;  break;
			case PRI6:
				*pPriority = (READ_MEM32(LPTM8021Q) & EN_8021Q2LTMPRI6_MASK) >> EN_8021Q2LTMPRI6;  break;
			case PRI7:
				*pPriority = (READ_MEM32(LPTM8021Q) & EN_8021Q2LTMPRI7_MASK) >> EN_8021Q2LTMPRI7;  break;
		}
	}

	return SUCCESS;
}


/*
@func int32 | rtl8651_flushAsicDot1qAbsolutelyPriority | set 802.1Q absolutely priority the default value 0
@parm void
@rvalue SUCCESS | 
@comm
 */
int32 rtl8651_flushAsicDot1qAbsolutelyPriority(void)
{
	WRITE_MEM32(LPTM8021Q, 0);
	return SUCCESS;
}

 

/*
@func int32 | rtl8651_setAsicDscpPriority | set DSCP-based priority 
@parm uint32 | dscp | DSCP value
@parm enum PRIORITYVALUE | priority | priority value 
@rvalue SUCCESS | 
@rvalue FAILED | invalid parameter
@comm
 */
int32 rtl8651_setAsicDscpPriority( uint32 dscp, enum PRIORITYVALUE priority )
{
	/* Invalid input parameter */
	if ((dscp < 0) || (dscp > 63)) 
		return FAILED;
	if ((priority < PRI0) || (priority > PRI7)) 
		return FAILED;

	if ((0 <= dscp) && (dscp <= 9))
		WRITE_MEM32(DSCPCR0, (READ_MEM32(DSCPCR0) & ~(0x7 << (dscp*3))) | (priority << (dscp*3))); 
	else if ((10 <= dscp) && (dscp <= 19))
		WRITE_MEM32(DSCPCR1, (READ_MEM32(DSCPCR1) & ~(0x7 << ((dscp%10)*3))) | (priority << ((dscp%10)*3))); 
	else if ((20 <= dscp) && (dscp <= 29))
		WRITE_MEM32(DSCPCR2, (READ_MEM32(DSCPCR2) & ~(0x7 << ((dscp%10)*3))) | (priority << ((dscp%10)*3))); 
	else if ((30 <= dscp) && (dscp <= 39))
		WRITE_MEM32(DSCPCR3, (READ_MEM32(DSCPCR3) & ~(0x7 << ((dscp%10)*3))) | (priority << ((dscp%10)*3))); 
	else if ((40 <= dscp) && (dscp <= 49))
		WRITE_MEM32(DSCPCR4, (READ_MEM32(DSCPCR4) & ~(0x7 << ((dscp%10)*3))) | (priority << ((dscp%10)*3))); 
	else if ((50 <= dscp) && (dscp <= 59))
		WRITE_MEM32(DSCPCR5, (READ_MEM32(DSCPCR5) & ~(0x7 << ((dscp%10)*3))) | (priority << ((dscp%10)*3))); 
	else if ((60 <= dscp) && (dscp <= 63))
		WRITE_MEM32(DSCPCR6, (READ_MEM32(DSCPCR6) & ~(0x7 << ((dscp%10)*3))) | (priority << ((dscp%10)*3))); 

	return SUCCESS;
}


/*
@func int32 | rtl8651_setAsicDscpPriority | set DSCP-based priority 
@parm uint32 | dscp | DSCP value
@parm enum PRIORITYVALUE* | pPriority | pPriority will return the priority of the specified DSCP
@rvalue SUCCESS | 
@rvalue FAILED | invalid parameter
@comm
 */
int32 rtl8651_getAsicDscpPriority( uint32 dscp, enum PRIORITYVALUE *pPriority )
{
	/* Invalid input parameter */
	if ((dscp < 0) || (dscp > 63)) 
		return FAILED;

	if (pPriority != NULL)
	{
		if ((0 <= dscp) && (dscp <= 9))
			*pPriority = (READ_MEM32(DSCPCR0) & (0x7 << (dscp*3))) >> (dscp*3);
		else if ((10 <= dscp) && (dscp <= 19))
			*pPriority = (READ_MEM32(DSCPCR1) & (0x7 << ((dscp%10)*3))) >> ((dscp%10)*3);
		else if ((20 <= dscp) && (dscp <= 29))
			*pPriority = (READ_MEM32(DSCPCR2) & (0x7 << ((dscp%10)*3))) >> ((dscp%10)*3);
		else if ((30 <= dscp) && (dscp <= 39))
			*pPriority = (READ_MEM32(DSCPCR3) & (0x7 << ((dscp%10)*3))) >> ((dscp%10)*3);
		else if ((40 <= dscp) && (dscp <= 49))
			*pPriority = (READ_MEM32(DSCPCR4) & (0x7 << ((dscp%10)*3))) >> ((dscp%10)*3);
		else if ((50 <= dscp) && (dscp <= 59))
			*pPriority = (READ_MEM32(DSCPCR5) & (0x7 << ((dscp%10)*3))) >> ((dscp%10)*3);
		else if ((60 <= dscp) && (dscp <= 63))
			*pPriority = (READ_MEM32(DSCPCR6) & (0x7 << ((dscp%10)*3))) >> ((dscp%10)*3);
	}

	return SUCCESS;
}


/*
@func int32 | rtl8651_setAsicPriorityToQIDMappingTable | set user priority to QID mapping table parameter
@parm enum QUEUENUM | qnum | the output queue number
@parm enum PRIORITYVALUE | priority | priority
@parm enum QUEUEID | qid | queue ID
@rvalue SUCCESS | 
@rvalue FAILED | invalid parameter
@comm
 */
int32 rtl8651_setAsicPriorityToQIDMappingTable( enum QUEUENUM qnum, enum PRIORITYVALUE priority, enum QUEUEID qid )
{
	/* Invalid input parameter */
	if ((priority < PRI0) || (priority > PRI7)) 
		return FAILED;
	if ((qid < QUEUE0) || (qid > QUEUE5)) 
		return FAILED;

	switch (qnum) 
	{
		case QNUM1:
			WRITE_MEM32(UPTCMCR0, (READ_MEM32(UPTCMCR0) & ~(0x7 << (priority*3))) | (qid << (priority*3))); break;
		case QNUM2:
			WRITE_MEM32(UPTCMCR1, (READ_MEM32(UPTCMCR1) & ~(0x7 << (priority*3))) | (qid << (priority*3))); break;
		case QNUM3:
			WRITE_MEM32(UPTCMCR2, (READ_MEM32(UPTCMCR2) & ~(0x7 << (priority*3))) | (qid << (priority*3))); break;
		case QNUM4:
			WRITE_MEM32(UPTCMCR3, (READ_MEM32(UPTCMCR3) & ~(0x7 << (priority*3))) | (qid << (priority*3))); break;
		case QNUM5:
			WRITE_MEM32(UPTCMCR4, (READ_MEM32(UPTCMCR4) & ~(0x7 << (priority*3))) | (qid << (priority*3))); break;
		case QNUM6:
			WRITE_MEM32(UPTCMCR5, (READ_MEM32(UPTCMCR5) & ~(0x7 << (priority*3))) | (qid << (priority*3))); break;
		default: 
			return FAILED;
	}

	return SUCCESS;
}


/*
@func int32 | rtl8651_getAsicPriorityToQIDMappingTable | get user priority to QID mapping table parameter
@parm enum QUEUENUM | qnum | the output queue number
@parm enum PRIORITYVALUE | priority | priority
@parm enum QUEUEID* | pQid | pQid will return the mapping queue ID
@rvalue SUCCESS | 
@rvalue FAILED | invalid parameter
@comm
 */
int32 rtl8651_getAsicPriorityToQIDMappingTable( enum QUEUENUM qnum, enum PRIORITYVALUE priority, enum QUEUEID* pQid )
{
	/* Invalid input parameter */
	if ((priority < PRI0) || (priority > PRI7)) 
		return FAILED;

	if (pQid != NULL)
	{
		switch (qnum) 
		{
			case QNUM1:
				*pQid = (READ_MEM32(UPTCMCR0) & (0x7 << (priority*3))) >> (priority*3); break;
			case QNUM2:
				*pQid = (READ_MEM32(UPTCMCR1) & (0x7 << (priority*3))) >> (priority*3); break;
			case QNUM3:
				*pQid = (READ_MEM32(UPTCMCR2) & (0x7 << (priority*3))) >> (priority*3); break;
			case QNUM4:
				*pQid = (READ_MEM32(UPTCMCR3) & (0x7 << (priority*3))) >> (priority*3); break;
			case QNUM5:
				*pQid = (READ_MEM32(UPTCMCR4) & (0x7 << (priority*3))) >> (priority*3); break;
			case QNUM6:
				*pQid = (READ_MEM32(UPTCMCR5) & (0x7 << (priority*3))) >> (priority*3); break;
			default: 
				return FAILED;
		}
	}

	return SUCCESS;
}



/*
@func int32 | rtl8651_getAsicCPUPriorityToQIDMappingTable | get user priority to QID mapping table parameter based on destination port & priority information
@parm enum PORTID | port | the destination port
@parm enum PRIORITYVALUE | priority | priority
@parm enum QUEUEID* | pQid | pQid will return the mapping queue ID
@rvalue SUCCESS | 
@rvalue FAILED | invalid parameter
@comm
 */
int32 rtl8651_getAsicCPUPriorityToQIDMappingTable( enum PORTID port, enum PRIORITYVALUE priority, enum QUEUEID *pQid )
{
	uint32	reg;
	/* Invalid input parameter */
	if ((priority < PRI0) || (priority > PRI7)) 
		return FAILED;

	if (port<CPU || port>MULTEXT)
		return FAILED;

	reg = (uint32)(((uint32*)CPUQIDMCR0) + (port-CPU));
	if (pQid != NULL)
	{
		*pQid = (READ_MEM32(reg) & (0x7 << (priority<<2)))>>(priority<<2); 
	}

	return SUCCESS;
}


/*
@func int32 | rtl8651_setAsicCPUPriorityToQIDMappingTable | set user priority to QID mapping table parameter based on destination port & priority information
@parm enum PORTID | port | the destination port
@parm enum PRIORITYVALUE | priority | priority
@parm enum QUEUEID | qid | queue ID
@rvalue SUCCESS | 
@rvalue FAILED | invalid parameter
@comm
 */
int32 rtl8651_setAsicCPUPriorityToQIDMappingTable( enum PORTID port, enum PRIORITYVALUE priority, enum QUEUEID qid )
{
	uint32	reg;
	/* Invalid input parameter */
	if ((priority < PRI0) || (priority > PRI7)) 
		return FAILED;
	if ((qid < QUEUE0) || (qid > QUEUE5)) 
		return FAILED;
	if (port<CPU || port>MULTEXT)
		return FAILED;

	reg = (uint32)(((uint32*)CPUQIDMCR0) + (port-CPU));

	WRITE_MEM32(reg, (READ_MEM32(reg) & ~(0x7 << (priority<<2))) | (qid << (priority<<2))); 

	return SUCCESS;
}



/*
@func int32 | rtl8651_setAsicOutputQueueNumber | set output queue number for a specified port
@parm enum PORTID | port | the port number (valid: physical ports(0~5) and CPU port(6) )
@parm enum QUEUENUM | qnum | the output queue number
@rvalue SUCCESS | 
@rvalue FAILED | invalid parameter
@comm
 */
int32 rtl8651_setAsicOutputQueueNumber( enum PORTID port, enum QUEUENUM qnum )
{
	/* Invalid input parameter */

	enum QUEUENUM orgQnum;
	
	if ((port < PHY0) || (port > CPU) || (qnum < QNUM1) || (qnum > QNUM6))
		return FAILED; 

	orgQnum = (READ_MEM32(QNUMCR) >> (3*port)) & 0x7;
	WRITE_MEM32(QNUMCR, (READ_MEM32(QNUMCR) & ~(0x7 << (3*port))) | (qnum << (3*port)));
	if (qnum==6)
	{
		if (orgQnum!=6)
			_rtl865x_setQosThresholdByQueueIdx(QNUM_IDX_6);
	}
	else if (qnum>3)
	{
		if((orgQnum==6)||orgQnum<4)
			_rtl865x_setQosThresholdByQueueIdx(QNUM_IDX_45);
	}
	else
	{
		if(orgQnum>3)
			_rtl865x_setQosThresholdByQueueIdx(QNUM_IDX_123);
	}

	return SUCCESS;
}


/*
@func int32 | rtl8651_getAsicOutputQueueNumber | get output queue number for a specified port
@parm enum PORTID | port | the port number (valid: physical ports(0~5) and CPU port(6) )
@parm enum QUEUENUM | qnum | the output queue number
@rvalue SUCCESS | 
@rvalue FAILED | invalid parameter
@comm
 */
int32 rtl8651_getAsicOutputQueueNumber( enum PORTID port, enum QUEUENUM *qnum )
{
	/* Invalid input parameter */
	if ((port < PHY0) || (port > CPU))
		return FAILED; 

	if (qnum != NULL)
		*qnum = (READ_MEM32(QNUMCR) >> (3*port)) & 0x7;
	
	return SUCCESS;
}

#define RTL865XC_LOCKTLU_USE_TRXRDY

/*
@func int32 | rtl865xC_lockSWCore | stop TLU operation
@rvalue SUCCESS | 
@comm
	When TLU operation stopped, all the received pkt will be queue in rx buffer.
 */
int32 rtl865xC_lockSWCore(void)
{
#if defined(RTL865X_TEST) || defined(RTL865X_MODEL_USER) || defined(RTL865X_MODEL_KERNEL)
#else
	if (_rtl865xC_lockTLUCounter == 0)
	{
		/*	disable phy */
		WRITE_MEM32(PCRP0, ((READ_MEM32(PCRP0))&(~EnablePHYIf)) ); /* Jumbo Frame */
		WRITE_MEM32(PCRP1, ((READ_MEM32(PCRP1))&(~EnablePHYIf)) ); /* Jumbo Frame */
		WRITE_MEM32(PCRP2, ((READ_MEM32(PCRP2))&(~EnablePHYIf)) ); /* Jumbo Frame */
		WRITE_MEM32(PCRP3, ((READ_MEM32(PCRP3))&(~EnablePHYIf)) ); /* Jumbo Frame */
		WRITE_MEM32(PCRP4, ((READ_MEM32(PCRP4))&(~EnablePHYIf)) ); /* Jumbo Frame */

		if (rtl8651_tblAsicDrvPara.externalPHYProperty & RTL8651_TBLASIC_EXTPHYPROPERTY_PORT5_RTL8211B)
		{
			WRITE_MEM32(PCRP5, ((READ_MEM32(PCRP5))&(~EnablePHYIf)) ); /* Jumbo Frame */
		}

		if(RTL865X_PHY6_DSP_BUG)
			WRITE_MEM32(PCRP6, ((READ_MEM32(PCRP6))&(~EnablePHYIf)) );
		WRITE_MEM32(CPUIIMR, READ_MEM32(CPUIIMR)|(RX_DONE_IE_ALL|PKTHDR_DESC_RUNOUT_IE_ALL));
#ifdef RTL865X_CPUQOS
		_swNic_flushLowPriorityQueue();
		_swNic_rxRunoutHandler();
#endif
	}
#endif
	_rtl865xC_lockTLUCounter++;
	return SUCCESS;
}

/*
@func int32 | rtl865xC_unLockSWCore | restart TLU operation
@rvalue SUCCESS | 
@comm
	restore the system operation.
 */
 int32 rtl865xC_unLockSWCore(void)
 {
 #if defined(RTL865X_TEST) || defined(RTL865X_MODEL_USER) || defined(RTL865X_MODEL_KERNEL)
 #else
 	if (_rtl865xC_lockTLUCounter == 1)
 	{
		/*	disable phy */
		WRITE_MEM32(PCRP0, ((READ_MEM32(PCRP0))|(EnablePHYIf)) ); /* Jumbo Frame */
		WRITE_MEM32(PCRP1, ((READ_MEM32(PCRP1))|(EnablePHYIf)) ); /* Jumbo Frame */
		WRITE_MEM32(PCRP2, ((READ_MEM32(PCRP2))|(EnablePHYIf)) ); /* Jumbo Frame */
		WRITE_MEM32(PCRP3, ((READ_MEM32(PCRP3))|(EnablePHYIf)) ); /* Jumbo Frame */
		WRITE_MEM32(PCRP4, ((READ_MEM32(PCRP4))|(EnablePHYIf)) ); /* Jumbo Frame */

		if (rtl8651_tblAsicDrvPara.externalPHYProperty & RTL8651_TBLASIC_EXTPHYPROPERTY_PORT5_RTL8211B)
		{
			WRITE_MEM32(PCRP5, ((READ_MEM32(PCRP5))|(EnablePHYIf)) ); /* Jumbo Frame */
		}

		if(RTL865X_PHY6_DSP_BUG)
			WRITE_MEM32(PCRP6, ((READ_MEM32(PCRP6))|(EnablePHYIf)) );
 	}
#endif	
	_rtl865xC_lockTLUCounter--;
	return SUCCESS;
 }

static int32 _rtl865xC_QM_init( void )
{
#if defined(RTL865X_TEST) || defined(RTL865X_MODEL_USER) || defined(RTL865X_MODEL_KERNEL)
#else
	uint32 originalDescGetReady;

	rtlglue_printf("Start to initiate QM\n");
	
	/*
	   1. Get the original decriptor usage for QM.
	 */
	
	rtl865xC_lockSWCore();

	_rtl865xC_QM_orgDescUsage = 0;	/* by default, set it to 0 */
	
	do
	{
		int32 idx;

		originalDescGetReady = TRUE;	/* by default, set it to TRUE */
		
		for ( idx = 0 ; idx < RTL865XC_QM_DESC_READROBUSTPARAMETER ; idx ++ )
		{
			uint32 currentDescUsage;

			currentDescUsage = (READ_MEM32( GDSR0 ) & USEDDSC_MASK) >> USEDDSC_OFFSET;
			if (	( currentDescUsage == 0 /* It's impossible */ ) ||
				(( _rtl865xC_QM_orgDescUsage != 0 ) &&
				 ( currentDescUsage != _rtl865xC_QM_orgDescUsage) ) )
			{
				rtlglue_printf("INIT swCore descriptor count Failed : (%d [%d])\n", currentDescUsage, _rtl865xC_QM_orgDescUsage);
				originalDescGetReady = FALSE;
			} else
			{
				_rtl865xC_QM_orgDescUsage = currentDescUsage;
			}
		}
	} while ( originalDescGetReady != TRUE );

	rtl865xC_unLockSWCore();
#endif	
	return SUCCESS;
}


/*
@func int32 | rtl865xC_waitForOutputQueueEmpty | wait until output queue empty
@rvalue SUCCESS | 
@comm
	The function will not return until all the output queue is empty.
 */
int32 rtl865xC_waitForOutputQueueEmpty(void)
{
#if defined(RTL865X_TEST) || defined(RTL865X_MODEL_USER) || defined(RTL865X_MODEL_KERNEL)
#else
	int32	currentDescUsage;

	while ( ((READ_MEM32(LAGCR0)&OUTPUTQUEUE_STAT_MASK_CR0)^OUTPUTQUEUE_STAT_MASK_CR0) ||
       	((READ_MEM32(LAGCR1)&OUTPUTQUEUE_STAT_MASK_CR1)^OUTPUTQUEUE_STAT_MASK_CR1) );

	/*	There are something wrong when check the input queue is empty or not	*/
	while ( (currentDescUsage = ((READ_MEM32( GDSR0 ) & USEDDSC_MASK) >> USEDDSC_OFFSET)) > _rtl865xC_QM_orgDescUsage)
	{
#if 0	
		rtlglue_printf("Waiting for input queue empty.  ==> currently used %d.\n", (currentDescUsage-_rtl865xC_QM_orgDescUsage));
#else
		;	/* do nothing */
#endif
#ifdef RTL865X_CPUQOS
		_swNic_flushLowPriorityQueue();
		_swNic_rxRunoutHandler();
#endif
	}
#endif
       return SUCCESS;
}
 
/*
@func int32 | rtl8651_resetAsicOutputQueue | reset output queue
@rvalue SUCCESS | 
@comm
	When reset is done, all queue pointer will be reset to the initial base address.
 */

 int32 rtl8651_resetAsicOutputQueue(void)
 {
	uint32	i;
	uint32	scr, pauseTicks;
	
 	WRITE_MEM32(QRR, 0x0);
	scr = (REG32(SCCR) & 0x00000070) >> 4;
	switch( scr )
	{
		case 0: pauseTicks = 12500000; break;
		case 1: pauseTicks = 25000000; break;
		case 2: pauseTicks = 31250000; break;
		case 3: pauseTicks = 32500000; break;
		case 4: pauseTicks = 33750000; break;
		case 5: pauseTicks = 35000000; break;
		case 6: pauseTicks = 36250000; break;
		case 7: pauseTicks = 37500000; break;
		default:pauseTicks = 25000000; break;
	}
	/* waiting 500ms */
	for(i=pauseTicks;i<0;i--)
	{
		i = i;
	}
 	WRITE_MEM32(QRR, 0x1);

 	return SUCCESS;
 }


/*
@func int32 | rtl8651_initQoSParameter for testing
@comm
@	The API will be removed after verifying 865xC QoS function.
 */
int32 rtl8651_initQoSParameter(void)
{
	WRITE_MEM32( 0xbb804500, 0x000001e0);
	WRITE_MEM32( 0xbb804504, 0x00c801cc);
	WRITE_MEM32( 0xbb804508, 0x0028006c);
	WRITE_MEM32( 0xbb80450c, 0x001e005a);
	WRITE_MEM32( 0xbb804510, 0x001e005a);
	WRITE_MEM32( 0xbb804514, 0x001e005a);
	WRITE_MEM32( 0xbb804518, 0x001e005a);
	WRITE_MEM32( 0xbb80451c, 0x001e005a);
	WRITE_MEM32( 0xbb804520, 0x001e005a);
	WRITE_MEM32( 0xbb804524, 0x001e005a);
	WRITE_MEM32( 0xbb804528, 0x0010001e);
	WRITE_MEM32( 0xbb80452c, 0x0010001e);
	WRITE_MEM32( 0xbb804530, 0x0010001e);
	WRITE_MEM32( 0xbb804534, 0x0010001e);
	WRITE_MEM32( 0xbb804538, 0x0010001e);
	WRITE_MEM32( 0xbb80453c, 0x0010001e);
	WRITE_MEM32( 0xbb804540, 0x0010001e);
	WRITE_MEM32( 0xbb804544, 0x0010001e);
	WRITE_MEM32( 0xbb804548, 0x0010001e);
	WRITE_MEM32( 0xbb80454c, 0x0010001e);
	WRITE_MEM32( 0xbb804550, 0x0010001e);
	WRITE_MEM32( 0xbb804554, 0x0010001e);
	WRITE_MEM32( 0xbb804558, 0x0010001e);
	WRITE_MEM32( 0xbb80455c, 0x0010001e);
	WRITE_MEM32( 0xbb804560, 0x0010001e);
	WRITE_MEM32( 0xbb804564, 0x0010001e);
	WRITE_MEM32( 0xbb804568, 0x0010001e);
	WRITE_MEM32( 0xbb80456c, 0x0010001e);
	WRITE_MEM32( 0xbb804570, 0x0010001e);
	WRITE_MEM32( 0xbb804574, 0x0010001e);
	WRITE_MEM32( 0xbb804578, 0x0010001e);
	WRITE_MEM32( 0xbb80457c, 0x00018028);
	WRITE_MEM32( 0xbb804580, 0x00018028);
	WRITE_MEM32( 0xbb804584, 0x00018028);
	WRITE_MEM32( 0xbb804588, 0x00018028);
	WRITE_MEM32( 0xbb80458c, 0x00018028);
	WRITE_MEM32( 0xbb804590, 0x00018028);
	WRITE_MEM32( 0xbb804594, 0x00018028);
	WRITE_MEM32( 0xbb804598, 0x00018028);
	WRITE_MEM32( 0xbb80459c, 0x00018028);
	WRITE_MEM32( 0xbb8045a0, 0x00018028);
	WRITE_MEM32( 0xbb8045a4, 0x00018028);
	WRITE_MEM32( 0xbb8045a8, 0x00018028);
	WRITE_MEM32( 0xbb8045ac, 0x00018028);
	WRITE_MEM32( 0xbb8045b0, 0x00018028);
	WRITE_MEM32( 0xbb8045b4, 0x00018028);
	WRITE_MEM32( 0xbb8045b8, 0x00018028);
	WRITE_MEM32( 0xbb8045bc, 0x00018028);
	WRITE_MEM32( 0xbb8045c0, 0x00018028);
	WRITE_MEM32( 0xbb8045c4, 0x00018028);
	WRITE_MEM32( 0xbb8045c8, 0x00018028);
	WRITE_MEM32( 0xbb8045cc, 0x00018028);
	WRITE_MEM32( 0xbb8045d0, 0x1f1f1f1f);
	WRITE_MEM32( 0xbb8045d4, 0x001f1f1f);
	WRITE_MEM32( 0xbb8045d8, 0x0000003c);
	WRITE_MEM32( 0xbb8045dc, 0);
	WRITE_MEM32( 0xbb8045e0, 0x000096c8);
	WRITE_MEM32( 0xbb8045e4, 0);
	WRITE_MEM32( 0xbb8045e8, 0);
	WRITE_MEM32( 0xbb8045ec, 0);
	WRITE_MEM32( 0xbb8045f0, 0);
	WRITE_MEM32( 0xbb8045f4, 0);
	WRITE_MEM32( 0xbb8045f8, 0);
	WRITE_MEM32( 0xbb8045fc, 0);

	WRITE_MEM32( 0xbb804600, 0x0000007e);
	WRITE_MEM32( 0xbb804604, 0);
	WRITE_MEM32( 0xbb804608, 0);
	WRITE_MEM32( 0xbb80460c, 0);
	WRITE_MEM32( 0xbb804610, 0);
	WRITE_MEM32( 0xbb804614, 0);
	WRITE_MEM32( 0xbb804618, 0);
	WRITE_MEM32( 0xbb80461c, 0);     
	WRITE_MEM32( 0xbb804620, 0);
	WRITE_MEM32( 0xbb804624, 0);
	WRITE_MEM32( 0xbb804628, 0);
	WRITE_MEM32( 0xbb80462c, 0);
	WRITE_MEM32( 0xbb804630, 0);
	WRITE_MEM32( 0xbb804634, 0);
	WRITE_MEM32( 0xbb804638, 0);
	WRITE_MEM32( 0xbb80463c, 0);
	WRITE_MEM32( 0xbb804640, 0);
	WRITE_MEM32( 0xbb804644, 0);
	WRITE_MEM32( 0xbb804648, 0);
	WRITE_MEM32( 0xbb80464c, 0);
	WRITE_MEM32( 0xbb804650, 0);
	WRITE_MEM32( 0xbb804654, 0);
	WRITE_MEM32( 0xbb804658, 0);
	WRITE_MEM32( 0xbb80465c, 0);
	WRITE_MEM32( 0xbb804660, 0);
	WRITE_MEM32( 0xbb804664, 0);
	WRITE_MEM32( 0xbb804668, 0);
	WRITE_MEM32( 0xbb80466c, 0);
	WRITE_MEM32( 0xbb804670, 0);
	WRITE_MEM32( 0xbb804674, 0);
	WRITE_MEM32( 0xbb804678, 0);
	WRITE_MEM32( 0xbb80467c, 0);
	WRITE_MEM32( 0xbb804680, 0);
	WRITE_MEM32( 0xbb804684, 0);
	WRITE_MEM32( 0xbb804688, 0);
	WRITE_MEM32( 0xbb80468c, 0);
	WRITE_MEM32( 0xbb804690, 0);
	WRITE_MEM32( 0xbb804694, 0);
	WRITE_MEM32( 0xbb804698, 0);
	WRITE_MEM32( 0xbb80469c, 0);     
	WRITE_MEM32( 0xbb8046a0, 0);     
	WRITE_MEM32( 0xbb8046a4, 0);     
	WRITE_MEM32( 0xbb8046a8, 0);    
	WRITE_MEM32( 0xbb8046ac, 0);    
	WRITE_MEM32( 0xbb8046b0, 0);
	WRITE_MEM32( 0xbb8046b4, 0);
	WRITE_MEM32( 0xbb8046b8, 0);
	WRITE_MEM32( 0xbb8046bc, 0);
	WRITE_MEM32( 0xbb8046c0, 0);
	WRITE_MEM32( 0xbb8046c4, 0);
	WRITE_MEM32( 0xbb8046c8, 0);
	WRITE_MEM32( 0xbb8046cc, 0);
	WRITE_MEM32( 0xbb8046d0, 0);
	WRITE_MEM32( 0xbb8046d4, 0);
	WRITE_MEM32( 0xbb8046d8, 0);
	WRITE_MEM32( 0xbb8046dc, 0);
	WRITE_MEM32( 0xbb8046e0, 0);
	WRITE_MEM32( 0xbb8046e4, 0);
	WRITE_MEM32( 0xbb8046e8, 0);
	WRITE_MEM32( 0xbb8046ec, 0);
	WRITE_MEM32( 0xbb8046f0, 0);
	WRITE_MEM32( 0xbb8046f4, 0);
	WRITE_MEM32( 0xbb8046f8, 0);
	WRITE_MEM32( 0xbb8046fc, 0);

	WRITE_MEM32( 0xbb804700, 0);
	WRITE_MEM32( 0xbb804704, 0);
	WRITE_MEM32( 0xbb804708, 0);
	WRITE_MEM32( 0xbb80470c, 0);
	WRITE_MEM32( 0xbb804710, 0);
	WRITE_MEM32( 0xbb804714, 0);
	WRITE_MEM32( 0xbb804718, 0);
	WRITE_MEM32( 0xbb80471c, 0x00b6d000);    
	WRITE_MEM32( 0xbb804720, 0x00b49000);
	WRITE_MEM32( 0xbb804724, 0x00b52201);
	WRITE_MEM32( 0xbb804728, 0x00b5a201);
	WRITE_MEM32( 0xbb80472c, 0x00b63401);
	WRITE_MEM32( 0xbb804730, 0x00fac642);
	WRITE_MEM32( 0xbb804734, 0x09249249);
	WRITE_MEM32( 0xbb804738, 0x09249249);
	WRITE_MEM32( 0xbb80473c, 0x09249249);
	WRITE_MEM32( 0xbb804740, 0x09249249);
	WRITE_MEM32( 0xbb804744, 0x09249249);
	WRITE_MEM32( 0xbb804748, 0x09249249);
	WRITE_MEM32( 0xbb80474c, 0x00000249);
	WRITE_MEM32( 0xbb804750, 0x00011111);
	WRITE_MEM32( 0xbb804754, 0x00001249);
	WRITE_MEM32( 0xbb804758, 0);
	WRITE_MEM32( 0xbb80475c, 0);
	WRITE_MEM32( 0xbb804760, 0);
	WRITE_MEM32( 0xbb804764, 0);
	WRITE_MEM32( 0xbb804768, 0);
	WRITE_MEM32( 0xbb80476c, 0x00fac688);
	WRITE_MEM32( 0xbb804770, 0x2e000000);
	WRITE_MEM32( 0xbb804774, 0x0002ebae);
	WRITE_MEM32( 0xbb804778, 0);
	WRITE_MEM32( 0xbb80477c, 0);
	WRITE_MEM32( 0xbb804780, 0);
	WRITE_MEM32( 0xbb804784, 0);
	WRITE_MEM32( 0xbb804788, 0);
	WRITE_MEM32( 0xbb80478c, 0);
	WRITE_MEM32( 0xbb804790, 0);
	WRITE_MEM32( 0xbb804794, 0);
	WRITE_MEM32( 0xbb804798, 0);
	WRITE_MEM32( 0xbb80479c, 0);
	WRITE_MEM32( 0xbb8047a0, 0);
	WRITE_MEM32( 0xbb8047a4, 0);
	WRITE_MEM32( 0xbb8047a8, 0);
	WRITE_MEM32( 0xbb8047ac, 0);
	WRITE_MEM32( 0xbb8047b0, 0);
	WRITE_MEM32( 0xbb8047b4, 0);
	WRITE_MEM32( 0xbb8047b8, 0);
	WRITE_MEM32( 0xbb8047bc, 0);
	WRITE_MEM32( 0xbb8047c0, 0);
	WRITE_MEM32( 0xbb8047c4, 0);
	WRITE_MEM32( 0xbb8047c8, 0);
	WRITE_MEM32( 0xbb8047cc, 0);
	WRITE_MEM32( 0xbb8047d0, 0);
	WRITE_MEM32( 0xbb8047d4, 0);
	WRITE_MEM32( 0xbb8047d8, 0);
	WRITE_MEM32( 0xbb8047dc, 0);
	WRITE_MEM32( 0xbb8047e0, 0);
	WRITE_MEM32( 0xbb8047e4, 0);
	WRITE_MEM32( 0xbb8047e8, 0);
	WRITE_MEM32( 0xbb8047ec, 0);
	WRITE_MEM32( 0xbb8047f0, 0);
	WRITE_MEM32( 0xbb8047f4, 0);
	WRITE_MEM32( 0xbb8047f8, 0);
	WRITE_MEM32( 0xbb8047fc, 0);

	WRITE_MEM32( 0xbb804800, 0x07ff3fff);
	WRITE_MEM32( 0xbb804804, 0x07ff3fff);
	WRITE_MEM32( 0xbb804808, 0x07ff3fff);
	WRITE_MEM32( 0xbb80480c, 0x07ff3fff);
	WRITE_MEM32( 0xbb804810, 0x07ff3fff);
	WRITE_MEM32( 0xbb804814, 0x07ff3fff);
	WRITE_MEM32( 0xbb804818, 0x07ff3fff);
	WRITE_MEM32( 0xbb80481c, 0x07ff3fff);
	WRITE_MEM32( 0xbb804820, 0x07ff3fff);
	WRITE_MEM32( 0xbb804824, 0x07ff3fff);
	WRITE_MEM32( 0xbb804828, 0x07ff3fff);
	WRITE_MEM32( 0xbb80482c, 0x07ff3fff);
	WRITE_MEM32( 0xbb804830, 0x07ff3fff);
	WRITE_MEM32( 0xbb804834, 0x07ff3fff);
	WRITE_MEM32( 0xbb804838, 0x07ff3fff);
	WRITE_MEM32( 0xbb80483c, 0x07ff3fff);
	WRITE_MEM32( 0xbb804840, 0x07ff3fff);
	WRITE_MEM32( 0xbb804844, 0x07ff3fff);
	WRITE_MEM32( 0xbb804848, 0x07ff3fff);
	WRITE_MEM32( 0xbb80484c, 0x07ff3fff);
	WRITE_MEM32( 0xbb804850, 0x07ff3fff);
	WRITE_MEM32( 0xbb804854, 0x07ff3fff);
	WRITE_MEM32( 0xbb804858, 0x07ff3fff);
	WRITE_MEM32( 0xbb80485c, 0x07ff3fff);
	WRITE_MEM32( 0xbb804860, 0x07ff3fff);
	WRITE_MEM32( 0xbb804864, 0x07ff3fff);
	WRITE_MEM32( 0xbb804868, 0x07ff3fff);
	WRITE_MEM32( 0xbb80486c, 0x07ff3fff);
	WRITE_MEM32( 0xbb804870, 0x07ff3fff);
	WRITE_MEM32( 0xbb804874, 0x07ff3fff);
	WRITE_MEM32( 0xbb804878, 0x07ff3fff);
	WRITE_MEM32( 0xbb80487c, 0x07ff3fff);
	WRITE_MEM32( 0xbb804880, 0x07ff3fff);
	WRITE_MEM32( 0xbb804884, 0x07ff3fff);
	WRITE_MEM32( 0xbb804888, 0x07ff3fff);
	WRITE_MEM32( 0xbb80488c, 0x07ff3fff);
	WRITE_MEM32( 0xbb804890, 0x07ff3fff);
	WRITE_MEM32( 0xbb804894, 0x07ff3fff);
	WRITE_MEM32( 0xbb804898, 0x07ff3fff);
	WRITE_MEM32( 0xbb80489c, 0x07ff3fff);
	WRITE_MEM32( 0xbb8048a0, 0x07ff3fff);
	WRITE_MEM32( 0xbb8048a4, 0x07ff3fff);
	WRITE_MEM32( 0xbb8048a8, 0);
	WRITE_MEM32( 0xbb8048ac, 0);
	WRITE_MEM32( 0xbb8048b0, 0x00003fff);
	WRITE_MEM32( 0xbb8048b4, 0);
	WRITE_MEM32( 0xbb8048b8, 0);
	WRITE_MEM32( 0xbb8048bc, 0x00003fff);
	WRITE_MEM32( 0xbb8048c0, 0);
	WRITE_MEM32( 0xbb8048c4, 0);
	WRITE_MEM32( 0xbb8048c8, 0x00003fff);
	WRITE_MEM32( 0xbb8048cc, 0);
	WRITE_MEM32( 0xbb8048d0, 0);
	WRITE_MEM32( 0xbb8048d4, 0x00003fff); 
	WRITE_MEM32( 0xbb8048d8, 0);
	WRITE_MEM32( 0xbb8048dc, 0);
	WRITE_MEM32( 0xbb8048e0, 0x00003fff); 
	WRITE_MEM32( 0xbb8048e4, 0); 
	WRITE_MEM32( 0xbb8048e8, 0); 
	WRITE_MEM32( 0xbb8048ec, 0x00003fff); 
	WRITE_MEM32( 0xbb8048f0, 0); 
	WRITE_MEM32( 0xbb8048f4, 0); 
	WRITE_MEM32( 0xbb8048f8, 0x00003fff); 
	WRITE_MEM32( 0xbb8048fc, 0);

	WRITE_MEM32( 0xbb804900, 0);
	WRITE_MEM32( 0xbb804904, 0x00003326);
	WRITE_MEM32( 0xbb804908, 0x00000099);
	WRITE_MEM32( 0xbb80490c, 0);
	WRITE_MEM32( 0xbb804910, 0x00003326);
	WRITE_MEM32( 0xbb804914, 0);
	WRITE_MEM32( 0xbb804918, 0);
	WRITE_MEM32( 0xbb80491c, 0x38000000);
	WRITE_MEM32( 0xbb804920, 0); 
	WRITE_MEM32( 0xbb804924, 0); 
	WRITE_MEM32( 0xbb804928, 0); 
	WRITE_MEM32( 0xbb80492c, 0); 
	WRITE_MEM32( 0xbb804930, 0); 
	WRITE_MEM32( 0xbb804934, 0); 
	WRITE_MEM32( 0xbb804938, 0); 
	WRITE_MEM32( 0xbb80493c, 0); 
	WRITE_MEM32( 0xbb804940, 0); 
	WRITE_MEM32( 0xbb804944, 0); 
	WRITE_MEM32( 0xbb804948, 0); 
	WRITE_MEM32( 0xbb80494c, 0); 
	WRITE_MEM32( 0xbb804950, 0); 
	WRITE_MEM32( 0xbb804954, 0); 
	WRITE_MEM32( 0xbb804958, 0); 
	WRITE_MEM32( 0xbb80495c, 0); 
	WRITE_MEM32( 0xbb804960, 0); 
	WRITE_MEM32( 0xbb804964, 0); 
	WRITE_MEM32( 0xbb804968, 0); 
	WRITE_MEM32( 0xbb80496c, 0); 
	WRITE_MEM32( 0xbb804970, 0); 
	WRITE_MEM32( 0xbb804974, 0); 
	WRITE_MEM32( 0xbb804978, 0); 
	WRITE_MEM32( 0xbb80497c, 0); 
	WRITE_MEM32( 0xbb804980, 0); 
	WRITE_MEM32( 0xbb804984, 0); 
	WRITE_MEM32( 0xbb804988, 0); 
	WRITE_MEM32( 0xbb80498c, 0); 
	WRITE_MEM32( 0xbb804990, 0); 
	WRITE_MEM32( 0xbb804994, 0); 
	WRITE_MEM32( 0xbb804998, 0); 
	WRITE_MEM32( 0xbb80499c, 0); 
	WRITE_MEM32( 0xbb8049a0, 0); 
	WRITE_MEM32( 0xbb8049a4, 0); 
	WRITE_MEM32( 0xbb8049a8, 0); 
	WRITE_MEM32( 0xbb8049ac, 0); 
	WRITE_MEM32( 0xbb8049b0, 0); 
	WRITE_MEM32( 0xbb8049b4, 0); 
	WRITE_MEM32( 0xbb8049b8, 0); 
	WRITE_MEM32( 0xbb8049bc, 0); 
	WRITE_MEM32( 0xbb8049c0, 0); 
	WRITE_MEM32( 0xbb8049c4, 0); 
	WRITE_MEM32( 0xbb8049c8, 0);
	WRITE_MEM32( 0xbb8049cc, 0); 
	WRITE_MEM32( 0xbb8049d0, 0); 
	WRITE_MEM32( 0xbb8049d4, 0); 
	WRITE_MEM32( 0xbb8049d8, 0); 
	WRITE_MEM32( 0xbb8049dc, 0);
	WRITE_MEM32( 0xbb8049e0, 0); 
	WRITE_MEM32( 0xbb8049e4, 0); 
	WRITE_MEM32( 0xbb8049e8, 0); 
	WRITE_MEM32( 0xbb8049ec, 0); 
	WRITE_MEM32( 0xbb8049f0, 0); 
	WRITE_MEM32( 0xbb8049f4, 0); 
	WRITE_MEM32( 0xbb8049f8, 0); 
	WRITE_MEM32( 0xbb8049fc, 0); 
	            
	WRITE_MEM32( 0xbb804a00, 0x000001ff);
	WRITE_MEM32( 0xbb804a04, 0);
	WRITE_MEM32( 0xbb804a08, 0x00090008);
	WRITE_MEM32( 0xbb804a0c, 0x00090009); 
	WRITE_MEM32( 0xbb804a10, 0x00080009);
	WRITE_MEM32( 0xbb804a14, 0x00080009);
	WRITE_MEM32( 0xbb804a18, 0x00000001);
	WRITE_MEM32( 0xbb804a1c, 0); 
	WRITE_MEM32( 0xbb804a20, 0); 
	WRITE_MEM32( 0xbb804a24, 0); 
	WRITE_MEM32( 0xbb804a28, 0); 
	WRITE_MEM32( 0xbb804a2c, 0); 
	WRITE_MEM32( 0xbb804a30, 0); 
	WRITE_MEM32( 0xbb804a34, 0); 
	WRITE_MEM32( 0xbb804a38, 0); 
	WRITE_MEM32( 0xbb804a3c, 0); 
	WRITE_MEM32( 0xbb804a40, 0); 
	WRITE_MEM32( 0xbb804a44, 0); 
	WRITE_MEM32( 0xbb804a48, 0); 
	WRITE_MEM32( 0xbb804a4c, 0); 
	WRITE_MEM32( 0xbb804a50, 0); 
	WRITE_MEM32( 0xbb804a54, 0); 
	WRITE_MEM32( 0xbb804a58, 0); 
	WRITE_MEM32( 0xbb804a5c, 0); 
	WRITE_MEM32( 0xbb804a60, 0); 
	WRITE_MEM32( 0xbb804a64, 0); 
	WRITE_MEM32( 0xbb804a68, 0); 
	WRITE_MEM32( 0xbb804a6c, 0); 
	WRITE_MEM32( 0xbb804a70, 0); 
	WRITE_MEM32( 0xbb804a74, 0); 
	WRITE_MEM32( 0xbb804a78, 0); 
	WRITE_MEM32( 0xbb804a7c, 0); 
	WRITE_MEM32( 0xbb804a80, 0); 
	WRITE_MEM32( 0xbb804a84, 0); 
	WRITE_MEM32( 0xbb804a88, 0); 
	WRITE_MEM32( 0xbb804a8c, 0); 
	WRITE_MEM32( 0xbb804a90, 0); 
	WRITE_MEM32( 0xbb804a94, 0); 
	WRITE_MEM32( 0xbb804a98, 0); 
	WRITE_MEM32( 0xbb804a9c, 0); 
	WRITE_MEM32( 0xbb804aa0, 0); 
	WRITE_MEM32( 0xbb804aa4, 0); 
	WRITE_MEM32( 0xbb804aa8, 0); 
	WRITE_MEM32( 0xbb804aac, 0); 
	WRITE_MEM32( 0xbb804ab0, 0); 
	WRITE_MEM32( 0xbb804ab4, 0); 
	WRITE_MEM32( 0xbb804ab8, 0); 
	WRITE_MEM32( 0xbb804abc, 0); 
	WRITE_MEM32( 0xbb804ac0, 0); 
	WRITE_MEM32( 0xbb804ac4, 0); 
	WRITE_MEM32( 0xbb804ac8, 0); 
	WRITE_MEM32( 0xbb804acc, 0); 
	WRITE_MEM32( 0xbb804ad0, 0); 
	WRITE_MEM32( 0xbb804ad4, 0); 
	WRITE_MEM32( 0xbb804ad8, 0); 
	WRITE_MEM32( 0xbb804adc, 0); 
	WRITE_MEM32( 0xbb804ae0, 0); 
	WRITE_MEM32( 0xbb804ae4, 0); 
	WRITE_MEM32( 0xbb804ae8, 0); 
	WRITE_MEM32( 0xbb804aec, 0); 
	WRITE_MEM32( 0xbb804af0, 0); 
	WRITE_MEM32( 0xbb804af4, 0); 
	WRITE_MEM32( 0xbb804af8, 0); 
	WRITE_MEM32( 0xbb804afc, 0); 


	return SUCCESS;
}





/*
 ACL-based Priority and NAT-based priority setting are not in here. 
 */


/*=========================================
  * ASIC DRIVER API: Legacy APIs
  *=========================================*/
#define RTL865X_ASIC_DRIVER_LEGACY_API

int32 rtl8651_getHardwiredProtoTrap(uint8 protoType, uint16 protoContent, int8 *isEnable)
{
	rtlglue_printf("Current version of chip does not support hardware protocol-trap anymore.\n");
	return FAILED;
}


int32 rtl865xC_configAsicEthernetMIIPHYMode(uint32 port, uint32 Tcomp, uint32 Rcomp, uint32 enForceMode, uint32 forceLink, uint32 forceSpeed, uint32 forceDuplex)
{
	/*set port (MII port) to PHY mode */
	rtl865xC_setAsicEthernetMIIMode(port, LINK_MII_PHY);

	/*set MII port timing*/
	rtl865xC_setAsicEthernetRGMIITiming(port, Tcomp, Rcomp); 

	/*set MII port to force mode, 100M, full duplex*/
	rtl865xC_setAsicEthernetForceModeRegs(port, enForceMode, forceLink, forceSpeed, forceDuplex);

		return SUCCESS;
}

/*
@func int32 | rtl8651_setAsic802D1xMacBaseAbility | set 802.1x mac based ability
@parm enum PORTID | port | the port number (physical port: 0~5, extension port: 6~8)
@parm uint32* | isEnable | 1: enabled, 0: disabled.
@rvalue SUCCESS | 
@rvalue FAILED | invalid parameter
@comm
 */
int32 rtl8651_setAsic802D1xMacBaseAbility( enum PORTID port, uint32 isEnable )
{
	/* Invalid input parameter */
	if ((port < PHY0) || (port > EXT2)) 
		return FAILED;
	
	/* Invalid input parameter */
	if ((isEnable != TRUE) && (isEnable != FALSE))
		return FAILED; 

	switch (port)
	{
		case PHY0:
		WRITE_MEM32(DOT1XMACCR, isEnable == TRUE ? (READ_MEM32(DOT1XMACCR) | ( Dot1xMAC_P0En)):
											(READ_MEM32(DOT1XMACCR) & ~( Dot1xMAC_P0En))); 
			break;
			
		case PHY1:
		WRITE_MEM32(DOT1XMACCR, isEnable == TRUE ? (READ_MEM32(DOT1XMACCR) | ( Dot1xMAC_P1En)):
											(READ_MEM32(DOT1XMACCR) & ~( Dot1xMAC_P1En)));  
			break;	
			
		case PHY2:
		WRITE_MEM32(DOT1XMACCR, isEnable == TRUE ? (READ_MEM32(DOT1XMACCR) | ( Dot1xMAC_P2En)):
											(READ_MEM32(DOT1XMACCR) & ~( Dot1xMAC_P2En)));
			break;
			
		case PHY3:
		WRITE_MEM32(DOT1XMACCR, isEnable == TRUE ? (READ_MEM32(DOT1XMACCR) | ( Dot1xMAC_P3En)):
											(READ_MEM32(DOT1XMACCR) & ~( Dot1xMAC_P3En)));  
			break;
			
		case PHY4:
		WRITE_MEM32(DOT1XMACCR, isEnable == TRUE ? (READ_MEM32(DOT1XMACCR) | ( Dot1xMAC_P4En)):
											(READ_MEM32(DOT1XMACCR) & ~( Dot1xMAC_P4En)));  											
			break;		
			
		case PHY5:
		WRITE_MEM32(DOT1XMACCR, isEnable == TRUE ? (READ_MEM32(DOT1XMACCR) | ( Dot1xMAC_P5En)):
											(READ_MEM32(DOT1XMACCR) & ~( Dot1xMAC_P5En)));
			break;
											
		case CPU:
		WRITE_MEM32(DOT1XMACCR, isEnable == TRUE ? (READ_MEM32(DOT1XMACCR) | ( Dot1xMAC_P6En)):
											(READ_MEM32(DOT1XMACCR) & ~( Dot1xMAC_P6En)));
			break;
			
		case EXT1:
		WRITE_MEM32(DOT1XMACCR, isEnable == TRUE ? (READ_MEM32(DOT1XMACCR) | ( Dot1xMAC_P7En)):
											(READ_MEM32(DOT1XMACCR) & ~( Dot1xMAC_P7En)));  
			break;
			
		case EXT2:
		WRITE_MEM32(DOT1XMACCR, isEnable == TRUE ? (READ_MEM32(DOT1XMACCR) | ( Dot1xMAC_P8En)):
											(READ_MEM32(DOT1XMACCR) & ~( Dot1xMAC_P8En))); 
			break;
			
		case EXT3:
			return FAILED;

		case MULTEXT:
			return FAILED;
											
	}
	return SUCCESS;
}

/*
@func int32 | rtl8651_setAsic802D1xMacBaseDirection | set 802.1x mac based direction
@parm enum uint32 | dir | OperCOnntrolledDirections for MAC-Based ACCESS Control. 0:BOTH, 1:IN
@rvalue SUCCESS | 
@rvalue FAILED | invalid parameter
@comm
 */
int32 rtl8651_setAsic802D1xMacBaseDirection(int32 dir)
{


	/* Invalid input parameter */
	if ((dir != Dot1xMAC_OPDIR_IN) && (dir != Dot1xMAC_OPDIR_BOTH))
	{
		return FAILED;
	}

	if(dir == Dot1xMAC_OPDIR_IN)
	{
		WRITE_MEM32(DOT1XMACCR,(READ_MEM32(DOT1XMACCR)) |Dot1xMAC_OPDIR);
	}
	else
	{
		WRITE_MEM32(DOT1XMACCR,(READ_MEM32(DOT1XMACCR)) &(~Dot1xMAC_OPDIR));	
	}
	return SUCCESS;
}

/*
@func int32 | rtl8651_setAsicGuestVlanProcessControl | set guest vlan process control
@parm enum  uint32 | process |default process for unauthenticated client  (00~11)<<12
@rvalue SUCCESS 
@rvalue FAILED | invalid parameter
@comm
 */
int32 rtl8651_setAsicGuestVlanProcessControl( uint32 process)
{
	/* Invalid input parameter */	
	if((process < Dot1xUNAUTHBH_DROP) || (process > Dot1xUNAUTHBH_RESERVED))
		return FAILED;	
	
	WRITE_MEM32(GVGCR, ((READ_MEM32(GVGCR)) & 0x0fff) | process);

	return SUCCESS;
}
#endif

