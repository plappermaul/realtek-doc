


/* Copyright c                  Realtek Semiconductor Corporation, 2005
* All rights reserved.
* 
* Program : Model code for NIC
* Abstract : 
* Author : Tony Tzong-yn Su (tysu@realtek.com.tw)               
* $Id: parserTest.h,v 1.8 2006-07-13 15:58:51 chenyl Exp $
*/


#ifndef _PARSER_MODEL_CODE_
#define _PARSER_MODEL_CODE_

#include "rtl_types.h"

void parserInit(int icType);
void dumpHSB(hsb_param_t *hsb);
int compareHsb(hsb_param_t *hsb1,hsb_param_t *hsb2);
int32 _createSourcePacket(char *data_org,int flags,int8 iphlen,uint8 ip_proto);

enum  flagType
{
	L2_IP=0,
	L2_PPPOE63,		
	L2_PPPOE64_IP,
	L2_OTHER,
	L2_LLC,
	L2_SNAP_IP,
	L2_SNAP_PPPOE63,	
	L2_SNAP_PPPOE64_IP,
	L2_SNAP_OTHER,	
	L2_VLAN_IP,
	L2_VLAN_PPPOE63,	
	L2_VLAN_PPPOE64_IP,
	L2_VLAN_OTHER,	
	L2_VLAN_LLC,	
	L2_VLAN_SNAP_IP,
	L2_VLAN_SNAP_PPPOE63,	
	L2_VLAN_SNAP_PPPOE64_IP,
	L2_VLAN_SNAP_OTHER,	
	
//keep this field end of enum	
	L2_TYPE_MAX
};

int32 parserTagForDirectTXfromCPU(uint32 caseNo);
int32 parserPatternMatch(uint32 caseNo);
int32 parserChecksumCheck(uint32 caseNo);
int32 parserUrlTrap(uint32 caseNo);
int32 parserRxFromPhysicalPort(uint32 caseNo);
int32 parserRxFromExtPort(uint32 caseNo);

#endif

