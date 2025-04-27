/*
* Copyright c                  Realtek Semiconductor Corporation, 2002  
* All rights reserved.
* 
* Program : Header File for IC-specific Function
* Abstract : 
* Author : Louis Yung-Chieh Lo (yjlou@realtek.com.tw)               
* $Id: icModel.h,v 1.41 2006-12-26 07:57:21 michaelhuang Exp $
*/

#ifndef _IC_MODEL_

//#include <mbuf.h>
//#include <rtl_queue.h>
#include "rtl865xC_tblAsicDrv.h"
//#include <hsModel.h>

#define NUMBER_OF_L2_SWITCH_TABLE_ENTRY         1024
#define _IC_MODEL_
#define BIT1MASK        0x00000001
#define BIT2MASK        0x00000003
#define BIT3MASK        0x00000007
#define BIT4MASK        0x0000000F
#define BIT5MASK        0x0000001F
#define BIT6MASK        0x0000003F
#define BIT7MASK        0x0000007F
#define BIT8MASK        0x000000FF
#define BIT9MASK        0x000001FF
#define BIT10MASK       0x000003FF
#define BIT11MASK       0x000007FF
#define BIT12MASK       0x00000FFF
#define BIT13MASK       0x00001FFF
#define BIT14MASK       0x00003FFF
#define BIT15MASK       0x00007FFF
#define BIT16MASK       0x0000FFFF
#define BIT17MASK       0x0001FFFF
#define BIT18MASK       0x0003FFFF
#define BIT19MASK       0x0007FFFF
#define BIT20MASK       0x000FFFFF
#define BIT21MASK       0x001FFFFF
#define BIT22MASK       0x003FFFFF
#define BIT23MASK       0x007FFFFF
#define BIT24MASK       0x00FFFFFF
#define BIT25MASK       0x01FFFFFF
#define BIT26MASK       0x03FFFFFF
#define BIT27MASK       0x07FFFFFF
#define BIT28MASK       0x0FFFFFFF
#define BIT29MASK       0x1FFFFFFF
#define BIT30MASK       0x3FFFFFFF
#define BIT31MASK       0x7FFFFFFF
#define BIT32MASK       0xFFFFFFFF

/* Port number for 'spa' */
enum PORT_NUM
{
	PN_PORT0 = 0,
	PN_PORT1 = 1,
	PN_PORT2 = 2,
	PN_PORT3 = 3,
	PN_PORT4 = 4,
	PN_PORT5 = 5,
	PN_PORT_NOTPHY = 6,
};

enum EXTPORT_NUM
{
	PN_PORT_EXT0 = 0,
	PN_PORT_EXT1 = 1,
	PN_PORT_EXT2 = 2,
	PN_PORT_CPU = 3,
};

/* Port number for 'dp' & 'dpext'*/
enum PORT_MASK
{
	PM_PORT_0 = (1<<PN_PORT0),
	PM_PORT_1 = (1<<PN_PORT1),
	PM_PORT_2 = (1<<PN_PORT2),
	PM_PORT_3 = (1<<PN_PORT3),
	PM_PORT_4 = (1<<PN_PORT4),
	PM_PORT_5 = (1<<PN_PORT5),
	PM_PORT_NOTPHY = (1<<PN_PORT_NOTPHY),
	PM_PORT_ALL = (1<<PN_PORT0)|(1<<PN_PORT1)|(1<<PN_PORT2)|(1<<PN_PORT3)|(1<<PN_PORT4)|(1<<PN_PORT5)|(1<<PN_PORT_NOTPHY),
};

enum EXTPORT_MASK
{
	PM_PORT_EXT0 = (1<<PN_PORT_EXT0), /* PN_PORT_EXT0 is 0. For uniform port mask, use 'PM_PORT_EXT0<<RTL8651_PORT_NUMBER' */
	PM_PORT_EXT1 = (1<<PN_PORT_EXT1), /* PN_PORT_EXT1 is 1. For uniform port mask, use 'PM_PORT_EXT1<<RTL8651_PORT_NUMBER' */
	PM_PORT_EXT2 = (1<<PN_PORT_EXT2), /* PN_PORT_EXT2 is 2. For uniform port mask, use 'PM_PORT_EXT2<<RTL8651_PORT_NUMBER' */
	PM_PORT_CPU = (1<<PN_PORT_CPU),
	PM_EXTPORT = (1<<PN_PORT_CPU)|(1<<PN_PORT_EXT0)|(1<<PN_PORT_EXT1)|(1<<PN_PORT_EXT2),
};

enum MODEL_RETURN_VALUE
{
	/* 0*/ MRET_OK = 0,
	/* 1*/ MRET_DROP,
	/* 2*/ MRET_TOCPU,
};
enum MODEL_BOOLEAN_VALUE
{
	/* 0*/ MNOTMATCH = 0,
	/* 1*/ MMATCH,
	/* 2*/ MNOOP,
};

enum RXPKTDESC
{
	RxPktDesc0 = 0,
	RxPktDesc1 = 1,
	RxPktDesc2 = 2,
	RxPktDesc3 = 3,
	RxPktDesc4 = 4,
	RxPktDesc5 = 5,
};

extern uint32 FIRST_CPURPDCR0,FIRST_CPURPDCR1,FIRST_CPURPDCR2,FIRST_CPURPDCR3,FIRST_CPURPDCR4,FIRST_CPURPDCR5;
extern uint32 FIRST_CPURMDCR0;

#if 0
void modelIcSetDefaultValue( void );
void modelPktParser(hsb_param_t *hsb,uint8 *data,uint8 srcPortNum,uint16 length,struct rtl_pktHdr *pPkt);
int32 modelPktTranslator( uint8* packet, int32 len );
void modelBackupDescRegisters( void );
void modelRestoreDescRegisters( void );
int32 modelChainPMC( enum RXPKTDESC desc, int32 lenNeed, struct rtl_pktHdr** ppPkt );
void modelStart( void );
#endif

#if 0
/*
 *  The state machine for Mii-like Tx register
 */
enum TX_STATE
{
	TX_FREE = 0, /* MIITM_TXR? did not start the packet transmiting */
	TX_PREAMBLE, /* MIITM_TXR? is presenting preambles */
	TX_PACKET, /* MIITM_TXR? is presenting the content of packet */
	TX_TAIL, /* MIITM_TXR? is presenting the tailing words */
	TX_FINISHED, /* MIITM_TXR? is presenting this packet is transmitted finished, debug state */
};

struct packet_s
{
	int32 len; /* Packet length, included L2 CRC */ 
	enum TX_STATE txState; /* Since we need to serialize the packet content to MiiTx, this state machine to record the packet transport status. */
	int32 txDone; /* internal byte count of Tx state machine */
	int32 cntClockKicked; /* This is the counter to record how many clocks kicked, which is strobed by writing MiiTx. */
	uint8 pkt[16*1024+64]; /* for jumbo frame */
	CTAILQ_ENTRY( packet_s ) next;
};
typedef struct packet_s packet_t;
CTAILQ_HEAD( packetHead_s, packet_s ); /* header for linked list */
typedef struct packetHead_s packetHead_t;

void modelIcInit( void );
void modelIcExit( void );
int32 modelIcReinit( void );

/* RTL865xC virtualMac Functions */
int32 rtl865xC_convertHsbToAsic( hsb_param_t* hsb, hsb_t* rawHsb );
int32 rtl865xC_virtualMacSetHsb( hsb_t rawHsb );
int32 rtl865xC_convertHsbToSoftware( hsb_t* rawHsb, hsb_param_t* hsb );
int32 rtl865xC_virtualMacGetHsb( hsb_t* rawHsb );
int32 rtl865xC_convertHsaToAsic( hsa_param_t* hsa, hsa_t* rawHsa );
int32 rtl865xC_virtualMacSetHsa( hsa_t rawHsa );
int32 rtl865xC_convertHsaToSoftware( hsa_t* rawHsa, hsa_param_t* hsa );
int32 rtl865xC_virtualMacGetHsa( hsa_t* rawHsa );
int32 rtl865xC_virtualMacInit( void );
int32 rtl865xC_virtualMacInput( enum PORT_NUM fromPort, uint8* packet, int32 len );
int32 rtl865xC_virtualMacOutput( enum PORT_MASK *toPort, uint8* packet, int32 *len );
int32 rtl865xC_miiLikeRx( enum PORT_NUM fromPort, uint8* packet, int32 len );
int32 rtl865xC_miiLikeTx( enum PORT_MASK *toPort, uint8* packet, int32 *len );

/************************************************************************
 *  ALE Data
 *
 *  In ALE processing, we need workspace to store intermediary information
 *  between sub functions of model code. ale_data_t is what you need.
 *  Pass ale_data_t in every sub function of model code, just like hsb and
 *  hsa.
 *************************************************************************/
struct ale_data_s
{
	rtl865x_tblAsicDrv_intfParam_t srcNetif;
	rtl865x_tblAsicDrv_intfParam_t dstNetif;
	uint32 multiLayerMode;
	uint32 matchSrcNetif:1;
	uint32 matchDstNetif:1;
	uint32 matchPPPoE:1;
	uint32 DMACIsGateway:1;
	uint32 doL3L4:1;
	uint32 doL3:1;
	uint32 doL4:1;
	uint16 decision;
	uint16 aleInternalSvid;
	uint16 nexthop;
	uint8 rouitingSIPIdx;
	uint8 rouitingDIPIdx;
	uint8 srcNetifIdx;
	uint8 dstNetifIdx;
	uint8 pppoeIdx;
	uint8 lastMatchedIngressACLRule; /* keep ACL index for extension port's ph_reason. */
	uint8 lastMatchedEgressACLRule; /* keep ACL index for extension port's ph_reason. */
	uint16 srcType_p;		/*Determine the packet source type:LP, NI, NPI, RP*/
	uint16 dstType_p;		/*Determine the packet dst type:NE, LP, NPE, NI, NPI, RP*/


	int8   priorityControl[5];  /* -1 represent no priority*/

};
typedef struct ale_data_s ale_data_t;
#endif
#if 0
uint16 model_ipChecksum(struct ip * pip);
uint16 model_tcpChecksum(struct ip *pip);
uint16 model_icmpChecksum(struct ip *pip);
#endif
#endif
