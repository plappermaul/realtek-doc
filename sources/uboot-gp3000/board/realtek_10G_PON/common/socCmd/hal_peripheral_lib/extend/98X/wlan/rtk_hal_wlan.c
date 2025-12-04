/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


//#include "rtk_hal_wlan.h"
//#include "rtk_wlan_base.h"
#include "peripheral.h"
#include "../../../base/wlan/rtk_98f/8852A/include/HAL/halmac_pwr_seq_8852A.h"

struct halmac_wlan_pwr_cfg *card_en_flow_8852A[];
struct halmac_wlan_pwr_cfg *card_dis_flow_8852A[];


RTK_STATUS
InitDummy(VOID *Adapter, VOID *Data)
{
    return _EXIT_SUCCESS;
}

VOID 
HalRTKWLANOpInit(
    IN  VOID *Data
)
{
    PHAL_WLAN_OP pHalWLANOp = (PHAL_WLAN_OP) Data;

    pHalWLANOp->HALWLMacInit = InitMACHALCommon;
	pHalWLANOp->HALWLDMacInit = InitDMACHALCommon;
	pHalWLANOp->HALWLCMacInit = InitCMACHALCommon;
    pHalWLANOp->HALWLPwrInit = InitPowerHALCommon;
    pHalWLANOp->HALWLApModePwrInit = InitMACToAPHALCommon;
    pHalWLANOp->HALWLHCIDmaInit = InitHCIDmaHALCommon;
    pHalWLANOp->HALWLTxDesxGenHandler = TXDescGenHALCommon;
    pHalWLANOp->HALWLNormallCfg = InitCfgNormallHALCommon;
    pHalWLANOp->HALWLSwBcnCfg = InitCfgSwBcnHALCommon;
    pHalWLANOp->HALWLDmaLBCfg = InitCfgDmaLBHALCommon;
    pHalWLANOp->HALWLInInitMbssid = InitMBSSIDCommon;
    pHalWLANOp->HALWLInStopMbssid = StopMBSSIDCommon;
    pHalWLANOp->HALWLSecurityInit = SetSecurityHALCommon;
//    pHalWLANOp->HALWLInterruptHandle = InterruptHandleExtend;  
    pHalWLANOp->HALWLInterruptInit = InterruptInitExtend;      
    pHalWLANOp->HALWLSWPSInit = SWPSInit;     
    pHalWLANOp->HALWLMBIDCAMInit = InitMBIDCAM_98F;  
    pHalWLANOp->HALWLPCIEInit = InitPCIE98F;  
    pHalWLANOp->HalEnableWlanIP = EnableWlanIP98F;
}

VOID 
HalRTKWLAN_PWRseq(
    IN  VOID *Data
)
{

    PWLAN_PWR_CFG_TABLE pHalWLAN_pwrseq = (PWLAN_PWR_CFG_TABLE) Data;
    pHalWLAN_pwrseq->card_disable_flow = &rtl8198F_card_disable_flow;
    pHalWLAN_pwrseq->card_enable_flow = &rtl8198F_card_enable_flow;
    pHalWLAN_pwrseq->power_on_flow = &rtl8198F_power_on_flow;
    pHalWLAN_pwrseq->radio_off_flow = &rtl8198F_radio_off_flow;
    pHalWLAN_pwrseq->enter_lps_flow = &rtl8198F_enter_lps_flow;
    pHalWLAN_pwrseq->leave_lps_flow = &rtl8198F_leave_lps_flow;
}


VOID 
HalRTKWLAN_PWRseq8852A(
    IN  VOID *Data
)
{

    PWLAN_PWR_CFG_TABLE pHalWLAN_pwrseq = (PWLAN_PWR_CFG_TABLE) Data;
    pHalWLAN_pwrseq->card_disable_flow = &card_dis_flow_8852A;
    pHalWLAN_pwrseq->card_enable_flow = &card_en_flow_8852A;
    //pHalWLAN_pwrseq->power_on_flow = &rtl8198F_power_on_flow;
    //pHalWLAN_pwrseq->radio_off_flow = &rtl8198F_radio_off_flow;
    //pHalWLAN_pwrseq->enter_lps_flow = &enter_lps_flow_8852A;
    //pHalWLAN_pwrseq->leave_lps_flow = &leave_lps_flow_8852A;
}


