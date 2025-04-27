/*
* Copyright c                  Realtek Semiconductor Corporation, 2002  
* All rights reserved.
* 
* Program : Header File for Virtual MAC
* Abstract : 
* Author : Louis Yung-Chieh Lo (yjlou@realtek.com.tw)               
* $Id: virtualMac.h,v 1.12 2007-03-15 08:13:44 yjlou Exp $
*/

#ifndef _VIRTUAL_MAC_
#define _VIRTUAL_MAC_

//#include "icModel.h"
//#include "hsModel.h"

/* Since the ASIC simulation is much slower than model code.
 * Therefore, the model code must wait simulation result.
 * We have two methods to use:
 * WAIT_METHOD 0 - Original method, model sleeps for a period time.
 * WAIT_METHOD 1 - use vsv_waitCycle() to wait simulation time pass
 */
#define WAIT_METHOD 1
/* According to Lennie's suggestion: 2000ns */
#define RX_ALE_TX_CYCLE (2000/*ns*/ / 4/*ns*/ )


enum IC_TYPE
{
	IC_TYPE_MIN = 0,    /* to enumerate all IC_TYPE */ 
	IC_TYPE_REAL = 0,   /* to test FPGA/IC/VERA */
	IC_TYPE_MODEL,      /* to test model code */
	IC_TYPE_MAX         /* to enumerate all IC_TYPE */ 
};

#define VMAC_ERROR_NON_AVAILABLE	(-2) /* In this test environment, this IC type is not available. Please ignore it. */
#define VMAC_ERROR_NO_PACKET_OUT	(-3) /* No packet arrived from IC */


int32 model_setTestTargetMask( uint32 mask );
int32 model_getTestTargetMask( uint32* mask );
int32 model_tryTestTarget( enum IC_TYPE ic_type );
int32 model_setTestTarget( enum IC_TYPE ic_type );
int32 model_getTestTarget( enum IC_TYPE *pic_type );

#if 0
int32 miiLikeRx( enum PORT_NUM fromPort, uint8* packet, int32 len );
int32 miiLikeTx( enum PORT_MASK *toPort, uint8* packet, int32 *len );

int32 virtualMacWaitCycle( int32 cycle );
int32 virtualMacInit( void );
int32 virtualMacInput( enum PORT_NUM fromPort, uint8* packet, int32 len );
int32 virtualMacOutput( enum PORT_MASK *toPort, uint8* packet, int32 *len );
int32 virtualMacPumpPackets( enum PORT_MASK toPort );

int32 convertHsaToAsic( hsa_param_t* hsa, hsa_t* rawHsa );
int32 convertHsaToSoftware( hsa_t* rawHsa, hsa_param_t* hsa );
int32 convertHsbToAsic( hsb_param_t* hsb, hsb_t* rawHsb );
int32 convertHsbToSoftware( hsb_t* rawHsb, hsb_param_t* hsb );

int32 virtualMacGetHsb( hsb_param_t* hsb );
int32 virtualMacSetHsb( hsb_param_t* hsb );
int32 virtualMacGetHsa( hsa_param_t* hsa );
int32 virtualMacSetHsa( hsa_param_t* hsa );
#endif

#endif
