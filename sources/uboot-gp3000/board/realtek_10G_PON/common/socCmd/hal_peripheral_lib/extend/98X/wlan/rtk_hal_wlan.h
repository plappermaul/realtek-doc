/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _RTK_HAL_WLAN_H_
#define _RTK_HAL_WLAN_H_

#include "hal_api.h"
//#include "wl_common_hw.h"
//#include "peripheral.h"

typedef struct _HAL_WLAN_DAT_ADAPTER_ {
    u32          PayloadLen;
    u32          Queue;
    u8           IsBcn;
    u8           IsDlForFw;
    u8          *curDesc;
    u8          *curExtendDesc;
    u8           IsRootInterface;
    u8           IsVapInterface;
    u8           vap_init_seq;
    u16           TXBD_IDX;
    u32          security_type;
    u32          encryption;
    u32          decryption;  
    PORT_INFO   *PortInfo;    
}HAL_WLAN_DAT_ADAPTER, *PHAL_WLAN_DAT_ADAPTER;


typedef struct _HAL_WLAN_OP_ {
    RTK_STATUS (*HALWLMacInit) (VOID *Adapter,VOID *Data);
	RTK_STATUS (*HALWLDMacInit) (VOID *Adapter,VOID *Data);
	RTK_STATUS (*HALWLCMacInit) (VOID *Adapter,VOID *Data);
    RTK_STATUS (*HALWLPwrInit) (VOID *Adapter,VOID* Data);
    RTK_STATUS (*HALWLApModePwrInit) (VOID *Adapter, VOID *Data);
    RTK_STATUS (*HALWLHCIDmaInit) (VOID *Adapter,VOID *Data);    
    u32 (*HALWLTxDesxGenHandler)(u1Byte interface_sel,VOID *Data);
    RTK_STATUS (*HALWLSwBcnCfg) (VOID *Adapter,VOID *Data);
    RTK_STATUS (*HALWLNormallCfg) (VOID *Adapter,VOID *Data);
    RTK_STATUS (*HALWLDmaLBCfg) (VOID *Adapter,VOID *Data);
    RTK_STATUS (*HALWLInInitMbssid) (VOID *Data);
    RTK_STATUS (*HALWLInStopMbssid) (VOID *Data);
    RTK_STATUS (*HALWLSecurityInit) (VOID *Adapter,VOID *Data);
    RTK_STATUS (*HALWLInterruptHandle) (VOID *Data);    
    RTK_STATUS (*HALWLInterruptInit) (u1Byte interface_sel,VOID *Data); 
    RTK_STATUS (*HALWLSWPSInit) (u1Byte interface_sel,VOID *Data); 
	RTK_STATUS (*HALWLMBIDCAMInit) (VOID *Adapter,u1Byte CAMsize); 
    int (*HALWLPCIEInit) (VOID *Adapter, VOID *Data);
    VOID (*HalEnableWlanIP) ();
}HAL_WLAN_OP,*PHAL_WLAN_OP;

typedef struct _HAL_WLAN_ADAPTER_{
    PHAL_WLAN_DAT_ADAPTER    pWLANIrqDat;
    PHAL_WLAN_OP             pWLANIrqOp;
}HAL_WLAN_ADAPTER,*PHAL_WLAN_ADAPTER;

RTK_STATUS
HAL_ReadTypeID(
	u1Byte interface_sel
);

VOID 
HalRTKWLANOpInit(
    IN  VOID *Data
);

#endif //_RTK_HAL_WLAN_H_
