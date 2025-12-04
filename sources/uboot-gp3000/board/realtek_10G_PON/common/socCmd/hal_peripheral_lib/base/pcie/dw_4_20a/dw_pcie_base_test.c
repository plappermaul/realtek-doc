/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2014 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "basic_types.h"
#include "diag.h"
#include "section_config.h"
#include "rtl_utility.h"
#include "common.h"
#include "rand.h"

#include "dw_pcie_base.h"
//#include "dw_hal_pcie.h"
#include "peripheral.h"
#include "dw_pcie_base_test.h"


static void 
PCIeColdReset(IN u32 PCIeIdx)
{
	HalPCIeOpInit();

	HalPCIeRCInit(PCIeIdx);
	
	return;
}


static void
TxBufferFill(IN int length, OUT u8 *pTxBufferStr)
{
	u32 index = 0;

	/* Initialization */
	memset(pTxBufferStr, 0x0, PCIE_TX_BUFF_SIZE);

	/* Set MAC Address: 0x554433221100 */
	for(index = 0; index < MAC_ADDR_LEN; index ++)
	{
		pTxBufferStr[index] = (index << 0) | (index << 4);
	}

	/* Set Sequential Data */
	for(index = MAC_ADDR_LEN; index < length; index ++)
	{
                pTxBufferStr[index] = Rand() & 0xff;
	}
	
	return;
}


static u32
PCIeTRXTest(IN u32 PCIeIdx)
{
	u32 loopbackEn = _TRUE;
	u32 deviceVendorId = 0;
	u8 *pTxBufferAddr = 0;
	u32 length;
	int result = 0;

	//2 0. READ Endpoint Vendor/Device ID
	deviceVendorId = HAL_PCIE_EP_CONFIG_READ32(PCIeIdx, REG_DW_PCIE_DEVICE_ID_VENDOR_ID);

	if( (deviceVendorId != PCIE_EP_DEVICE_VENDOR_ID_RTL8168) && (deviceVendorId != PCIE_EP_DEVICE_VENDOR_ID_RTL8169) )
	{
		printf("=>Fail,PCIE P%d ID =0x%x\r\n", PCIeIdx, deviceVendorId);
		return PCIE_TRX_LOOPBACK_FAIL;
	}

	//2 1. Tx buffer initialization
	if(PCIeIdx == PCIE_MODULE_SEL_0){
		pTxBufferAddr = (u8 *)PCIE0_TX_BUFF_ADDR;
	}else if(PCIeIdx == PCIE_MODULE_SEL_1){
		pTxBufferAddr = (u8 *)PCIE1_TX_BUFF_ADDR;
	}

#if 1
        length = 4000;
#else
	length = Rand() & 0xfff; // limit to 64~4095
	length = (length < 64) ? 64 : length; 
#endif

	TxBufferFill(length, pTxBufferAddr);
	
	//2 2. Endpoint initialization
	HalPCIeEPInit(PCIeIdx, loopbackEn);


	//2 3. Endpoint Tx
	HalPCIeEPTx(PCIeIdx, length);


	//2 4. Endpoint Rx
	result = HalPCIeEPRx(PCIeIdx, length);

	if(result != PCIE_EP_RX_STATUS_SUCCESS)
	{
		return PCIE_TRX_LOOPBACK_FAIL;
	}

	return PCIE_TRX_LOOPBACK_SUCCESS;
}



void
PCIeHostDataLoopback(IN u32 PCIeIdx, IN u32 loopCnt)
{
	u32 PCIeTestIdx = 0;
	u32 testErr = 0;

	if(loopCnt == 0)
	{
		while(1)
		{
			if(PCIeTRXTest(PCIeIdx) == PCIE_TRX_LOOPBACK_FAIL)
			{
				printf("FAIL!\n");
                                break;
			}
                        PCIeTestIdx++;
                        if((PCIeTestIdx % 1000) == 0)
			{
				printf("PCIeTestIdx = %d\n", PCIeTestIdx);
			}
		}
	}else{
		for (PCIeTestIdx = 1; PCIeTestIdx <= loopCnt; PCIeTestIdx++)
		{
			if(PCIeTRXTest(PCIeIdx) == PCIE_TRX_LOOPBACK_FAIL)
			{
				testErr++;
			}
			if((PCIeTestIdx % 1000) == 0)
			{
				printf("PCIeTestIdx = %d\n", PCIeTestIdx);
			}
		}		
		printf("====> Total test cnt=%d, test fail=%d\n", loopCnt,	testErr);
	}

	return;
}

static void
PCIePowerSaving(IN u32 PCIeIdx, IN u32 subItem)
{
	u32 value32;
	
	switch(subItem)
	{
		case PCIE_TEST_SUB_ITEM_POWER_SAVING_D0_L0:
			// Endpoint, set to D0
			value32 = HAL_PCIE_EP_CONFIG_READ32(PCIeIdx, REG_DW_PCIE_CON_STATUS);
			value32 = value32 & (~ BIT_CTRL_DW_PCIE_POWER_STATE(BIT_MASK_DW_PCIE_POWER_STATE));
			value32 = value32 | BIT_CTRL_DW_PCIE_POWER_STATE(DW_PCIE_POWER_STATE_D0);
			HAL_PCIE_EP_CONFIG_WRITE32(PCIeIdx, REG_DW_PCIE_CON_STATUS, value32);
			
			// Check link
			HalPCIeCheckLink(PCIeIdx);
			break;
			
		case PCIE_TEST_SUB_ITEM_POWER_SAVING_D3HOT_L1:
			// Saving more power
			value32 = HAL_PCIE_EP_CONFIG_READ32(PCIeIdx, REG_DW_PCIE_LINK_CONTROL_LINK_STATUS);
			value32 = value32 | BIT_CTRL_DW_PCIE_PCIE_CAP_EN_CLK_POWER_MAN(1);
			HAL_PCIE_EP_CONFIG_WRITE32(PCIeIdx, REG_DW_PCIE_LINK_CONTROL_LINK_STATUS, value32);

			// Endpoint, set to D3Hot
			value32 = HAL_PCIE_EP_CONFIG_READ32(PCIeIdx, REG_DW_PCIE_CON_STATUS);
			value32 = value32 & (~ BIT_CTRL_DW_PCIE_POWER_STATE(BIT_MASK_DW_PCIE_POWER_STATE));
			value32 = value32 | BIT_CTRL_DW_PCIE_POWER_STATE(DW_PCIE_POWER_STATE_D3HOT);
			HAL_PCIE_EP_CONFIG_WRITE32(PCIeIdx, REG_DW_PCIE_CON_STATUS, value32);

			/* (a) EP sends PM_Enter_L1 DLLP packet, then RC responses with PM_Request_Ack packet. 
			*   (b) Finally, EIOS will be sent and go into Link Down state.
			*/
			
			break;
			
		case PCIE_TEST_SUB_ITEM_POWER_SAVING_D3COLD_L2:
			// Endpoint, set to D3Hot
			value32 = HAL_PCIE_EP_CONFIG_READ32(PCIeIdx, REG_DW_PCIE_CON_STATUS);
			value32 = value32 & (~ BIT_CTRL_DW_PCIE_POWER_STATE(BIT_MASK_DW_PCIE_POWER_STATE));
			value32 = value32 | BIT_CTRL_DW_PCIE_POWER_STATE(DW_PCIE_POWER_STATE_D3HOT);
			HAL_PCIE_EP_CONFIG_WRITE32(PCIeIdx, REG_DW_PCIE_CON_STATUS, value32);
			
			mdelay(300);

			// L2/L3 Handshake Sequence. 
			value32 = HAL_PCIE_RC_EXTEND_CONFIG_READ32(PCIeIdx, REG_DW_PCIE_EXTENDED_REG_PWRCR);
			value32 = value32 | BIT_CTRL_DW_PCIE_APPS_PM_XMT_TURNOFF(1);
			HAL_PCIE_RC_EXTEND_CONFIG_WRITE32(PCIeIdx, REG_DW_PCIE_EXTENDED_REG_PWRCR, value32);
			
			mdelay(500);
			
			// Remove system power/clock of EP
			
			break;
			
		case PCIE_TEST_SUB_ITEM_POWER_SAVING_L2_L3_HANDSHAKE:
			// RC broadcast PME Turn off message
			value32 = HAL_PCIE_RC_EXTEND_CONFIG_READ32(PCIeIdx, REG_DW_PCIE_EXTENDED_REG_PWRCR);
			value32 = value32 | BIT_CTRL_DW_PCIE_APPS_PM_XMT_TURNOFF(1);
			HAL_PCIE_RC_EXTEND_CONFIG_WRITE32(PCIeIdx, REG_DW_PCIE_EXTENDED_REG_PWRCR, value32);
			
			/* (a) RC sends PME_Turn_off packet, then EP responses with PME_TO_Ack packet.
			*   (b) EP sends PM_Enter_L23 packet, then RC responses with PM_Request_Ack packet.
			*   (c) Finally, EIOS will be sent and go into Link Down state.
			*/

			break;
			
		case PCIE_TEST_SUB_ITEM_POWER_SAVING_L0_L1_L0:
			// Endpoint, set to D3Hot
			value32 = HAL_PCIE_EP_CONFIG_READ32(PCIeIdx, REG_DW_PCIE_CON_STATUS);
			value32 = value32 & (~ BIT_CTRL_DW_PCIE_POWER_STATE(BIT_MASK_DW_PCIE_POWER_STATE));
			value32 = value32 | BIT_CTRL_DW_PCIE_POWER_STATE(DW_PCIE_POWER_STATE_D3HOT);
			HAL_PCIE_EP_CONFIG_WRITE32(PCIeIdx, REG_DW_PCIE_CON_STATUS, value32);

			mdelay(100);

			// Endpoint, set to D0
			value32 = value32 & (~ BIT_CTRL_DW_PCIE_POWER_STATE(BIT_MASK_DW_PCIE_POWER_STATE));
			value32 = value32 | BIT_CTRL_DW_PCIE_POWER_STATE(DW_PCIE_POWER_STATE_D0);
			HAL_PCIE_EP_CONFIG_WRITE32(PCIeIdx, REG_DW_PCIE_CON_STATUS, value32);

			// Check link
			HalPCIeCheckLink(PCIeIdx);
			
			break;
			
		case PCIE_TEST_SUB_ITEM_POWER_SAVING_L0_L2_L0:
#ifdef CONFIG_RTL8197F
			// Endpoint, set to D3Hot
			value32 = HAL_PCIE_EP_CONFIG_READ32(PCIeIdx, REG_DW_PCIE_CON_STATUS);
			value32 = value32 & (~ BIT_CTRL_DW_PCIE_POWER_STATE(BIT_MASK_DW_PCIE_POWER_STATE));
			value32 = value32 | BIT_CTRL_DW_PCIE_POWER_STATE(DW_PCIE_POWER_STATE_D3HOT);
			HAL_PCIE_EP_CONFIG_WRITE32(PCIeIdx, REG_DW_PCIE_CON_STATUS, value32);

			mdelay(300);

			// RC broadcast PME Turn off message
			value32 = HAL_PCIE_RC_EXTEND_CONFIG_READ32(PCIeIdx, REG_DW_PCIE_EXTENDED_REG_PWRCR);
			value32 = value32 | BIT_CTRL_DW_PCIE_APPS_PM_XMT_TURNOFF(1);
			HAL_PCIE_RC_EXTEND_CONFIG_WRITE32(PCIeIdx, REG_DW_PCIE_EXTENDED_REG_PWRCR, value32);

			mdelay(500);

			// PCIE PHY Reset
			HalPCIePHYReset(PCIeIdx);
			
			// PERST
			value32 = HAL_READ32_SYS(SYS_ENABLE);		//PERST=0  => each PCIe module should have it's own PERST
			value32 = value32 & (~ BIT_SYS_PCIE_RST_N);
			HAL_WRITE32_SYS(SYS_ENABLE, value32);
			
			mdelay(100);
			
			value32 = value32 | BIT_SYS_PCIE_RST_N; 	//PERST=1 
			HAL_WRITE32_SYS(SYS_ENABLE, value32);

			mdelay(100);
			// Check link
			HalPCIeCheckLink(PCIeIdx);
#else
                        HalPCIeWarmReset(PCIeIdx);
#endif
			break;
			
		case PCIE_TEST_SUB_ITEM_POWER_SAVING_ASPM_L0S:
			// Endpoint, set latency
			value32 = HAL_PCIE_EP_CONFIG_READ32(PCIeIdx, REG_DW_PCIE_ACK_F_ASPM_CTRL);
			value32 = value32 & (~ (BIT_CTRL_DW_PCIE_L0S_ENTRANCE_LATENCY(BIT_MASK_DW_PCIE_L0S_ENTRANCE_LATENCY) | BIT_CTRL_DW_PCIE_L1_ENTRANCE_LATENCY(BIT_MASK_DW_PCIE_L1_ENTRANCE_LATENCY)));
			value32 = value32 | BIT_CTRL_DW_PCIE_L0S_ENTRANCE_LATENCY(DW_PCIE_L0S_ENTRANCE_LATENCY_1US) | BIT_CTRL_DW_PCIE_L1_ENTRANCE_LATENCY(DW_PCIE_L1_ENTRANCE_LATENCY_8US) ;
			HAL_PCIE_EP_CONFIG_WRITE32(PCIeIdx, REG_DW_PCIE_ACK_F_ASPM_CTRL, value32);

			// RC, enable L0s
			value32 = HAL_PCIE_RC_CONFIG_READ32(PCIeIdx, REG_DW_PCIE_LINK_CONTROL_LINK_STATUS);
			value32 = value32 & ( ~ BIT_CTRL_DW_PCIE_PCIE_CAP_ACTIVE_STATE_LINK_PM_CONTROL(BIT_MASK_DW_PCIE_PCIE_CAP_ACTIVE_STATE_LINK_PM_CONTROL));
			value32 = value32 | BIT_CTRL_DW_PCIE_PCIE_CAP_ACTIVE_STATE_LINK_PM_CONTROL(DW_PCIE_PCIE_CAP_ACTIVE_STATE_LINK_PM_CONTROL_L0S);
			HAL_PCIE_RC_CONFIG_WRITE32(PCIeIdx, REG_DW_PCIE_LINK_CONTROL_LINK_STATUS, value32);
			
			// Endpoint, enable L0s
			value32 = HAL_PCIE_EP_CONFIG_READ32(PCIeIdx, REG_DW_PCIE_LINK_CONTROL_LINK_STATUS);
			value32 = value32 & ( ~ BIT_CTRL_DW_PCIE_PCIE_CAP_ACTIVE_STATE_LINK_PM_CONTROL(BIT_MASK_DW_PCIE_PCIE_CAP_ACTIVE_STATE_LINK_PM_CONTROL));
			value32 = value32 | BIT_CTRL_DW_PCIE_PCIE_CAP_ACTIVE_STATE_LINK_PM_CONTROL(DW_PCIE_PCIE_CAP_ACTIVE_STATE_LINK_PM_CONTROL_L0S);
			HAL_PCIE_EP_CONFIG_WRITE32(PCIeIdx, REG_DW_PCIE_LINK_CONTROL_LINK_STATUS, value32);


			/* (a) EIOS will be sent and go into Link Down state.
			*   (b) When EP/RC needs Link Up, EP/RC will send FTS to do fast TS.
			*/

			break;
			
		case PCIE_TEST_SUB_ITEM_POWER_SAVING_ASPM_L1:
#if 0
			// Endpoint, set latency
			value32 = HAL_PCIE_EP_CONFIG_READ32(PCIeIdx, REG_DW_PCIE_ACK_F_ASPM_CTRL);
			value32 = value32 & (~ (BIT_CTRL_DW_PCIE_L0S_ENTRANCE_LATENCY(BIT_MASK_DW_PCIE_L0S_ENTRANCE_LATENCY) | BIT_CTRL_DW_PCIE_L1_ENTRANCE_LATENCY(BIT_MASK_DW_PCIE_L1_ENTRANCE_LATENCY)));
			value32 = value32 | BIT_CTRL_DW_PCIE_L0S_ENTRANCE_LATENCY(DW_PCIE_L0S_ENTRANCE_LATENCY_4US) | BIT_CTRL_DW_PCIE_L1_ENTRANCE_LATENCY(DW_PCIE_L1_ENTRANCE_LATENCY_2US);
			HAL_PCIE_EP_CONFIG_WRITE32(PCIeIdx, REG_DW_PCIE_ACK_F_ASPM_CTRL, value32);
#endif

			// RC, enable L1
			value32 = HAL_PCIE_RC_CONFIG_READ32(PCIeIdx, REG_DW_PCIE_LINK_CONTROL_LINK_STATUS);
			value32 = value32 & ( ~ BIT_CTRL_DW_PCIE_PCIE_CAP_ACTIVE_STATE_LINK_PM_CONTROL(BIT_MASK_DW_PCIE_PCIE_CAP_ACTIVE_STATE_LINK_PM_CONTROL));
			value32 = value32 | BIT_CTRL_DW_PCIE_PCIE_CAP_ACTIVE_STATE_LINK_PM_CONTROL(DW_PCIE_PCIE_CAP_ACTIVE_STATE_LINK_PM_CONTROL_L1);
			HAL_PCIE_RC_CONFIG_WRITE32(PCIeIdx, REG_DW_PCIE_LINK_CONTROL_LINK_STATUS, value32);
			
			// Endpoint, enable L1
			value32 = HAL_PCIE_EP_CONFIG_READ32(PCIeIdx, REG_DW_PCIE_LINK_CONTROL_LINK_STATUS);
			value32 = value32 & ( ~ BIT_CTRL_DW_PCIE_PCIE_CAP_ACTIVE_STATE_LINK_PM_CONTROL(BIT_MASK_DW_PCIE_PCIE_CAP_ACTIVE_STATE_LINK_PM_CONTROL));
			value32 = value32 | BIT_CTRL_DW_PCIE_PCIE_CAP_ACTIVE_STATE_LINK_PM_CONTROL(DW_PCIE_PCIE_CAP_ACTIVE_STATE_LINK_PM_CONTROL_L1);
			HAL_PCIE_EP_CONFIG_WRITE32(PCIeIdx, REG_DW_PCIE_LINK_CONTROL_LINK_STATUS, value32);

			break;

		case PCIE_TEST_SUB_ITEM_POWER_SAVING_ASPM_L0S_L1:
#if 0
			// Endpoint, set latency
			value32 = HAL_PCIE_EP_CONFIG_READ32(PCIeIdx, REG_DW_PCIE_ACK_F_ASPM_CTRL);
			value32 = value32 & (~ (BIT_CTRL_DW_PCIE_L0S_ENTRANCE_LATENCY(BIT_MASK_DW_PCIE_L0S_ENTRANCE_LATENCY) | BIT_CTRL_DW_PCIE_L1_ENTRANCE_LATENCY(BIT_MASK_DW_PCIE_L1_ENTRANCE_LATENCY)));
			value32 = value32 | BIT_CTRL_DW_PCIE_L0S_ENTRANCE_LATENCY(DW_PCIE_L0S_ENTRANCE_LATENCY_4US) | BIT_CTRL_DW_PCIE_L1_ENTRANCE_LATENCY(DW_PCIE_L1_ENTRANCE_LATENCY_2US);
			HAL_PCIE_EP_CONFIG_WRITE32(PCIeIdx, REG_DW_PCIE_ACK_F_ASPM_CTRL, value32);
#endif

			// RC, enable L0s & L1
			value32 = HAL_PCIE_RC_CONFIG_READ32(PCIeIdx, REG_DW_PCIE_LINK_CONTROL_LINK_STATUS);
			value32 = value32 & ( ~ BIT_CTRL_DW_PCIE_PCIE_CAP_ACTIVE_STATE_LINK_PM_CONTROL(BIT_MASK_DW_PCIE_PCIE_CAP_ACTIVE_STATE_LINK_PM_CONTROL));
			value32 = value32 | BIT_CTRL_DW_PCIE_PCIE_CAP_ACTIVE_STATE_LINK_PM_CONTROL(DW_PCIE_PCIE_CAP_ACTIVE_STATE_LINK_PM_CONTROL_L0S_L1);
			HAL_PCIE_RC_CONFIG_WRITE32(PCIeIdx, REG_DW_PCIE_LINK_CONTROL_LINK_STATUS, value32);
			
			// Endpoint, enable L0s & L1
			value32 = HAL_PCIE_EP_CONFIG_READ32(PCIeIdx, REG_DW_PCIE_LINK_CONTROL_LINK_STATUS);
			value32 = value32 & ( ~ BIT_CTRL_DW_PCIE_PCIE_CAP_ACTIVE_STATE_LINK_PM_CONTROL(BIT_MASK_DW_PCIE_PCIE_CAP_ACTIVE_STATE_LINK_PM_CONTROL));
			value32 = value32 | BIT_CTRL_DW_PCIE_PCIE_CAP_ACTIVE_STATE_LINK_PM_CONTROL(DW_PCIE_PCIE_CAP_ACTIVE_STATE_LINK_PM_CONTROL_L0S_L1);
			HAL_PCIE_EP_CONFIG_WRITE32(PCIeIdx, REG_DW_PCIE_LINK_CONTROL_LINK_STATUS, value32);


			/* (a) RC/EP goes into L0s state first.
			*   (b) RC/EP wakes up through FTS.
			*   (c) EP sends PM_Active_State_Request_L1 packet, then RC responses with PM_Request_Ack packet. 
			*   (d) EP sends EIOS and goes into Link Down state.
			*/
			
			break;
			
		case PCIE_TEST_SUB_ITEM_POWER_SAVING_DISABLE_ASPM:
			// RC, disable ASPM
			value32 = HAL_PCIE_RC_CONFIG_READ32(PCIeIdx, REG_DW_PCIE_LINK_CONTROL_LINK_STATUS);
			value32 = value32 & ( ~ BIT_CTRL_DW_PCIE_PCIE_CAP_ACTIVE_STATE_LINK_PM_CONTROL(BIT_MASK_DW_PCIE_PCIE_CAP_ACTIVE_STATE_LINK_PM_CONTROL));
			HAL_PCIE_RC_CONFIG_WRITE32(PCIeIdx, REG_DW_PCIE_LINK_CONTROL_LINK_STATUS, value32);
			
			// Endpoint, disable ASPM
			value32 = HAL_PCIE_EP_CONFIG_READ32(PCIeIdx, REG_DW_PCIE_LINK_CONTROL_LINK_STATUS);
			value32 = value32 & ( ~ BIT_CTRL_DW_PCIE_PCIE_CAP_ACTIVE_STATE_LINK_PM_CONTROL(BIT_MASK_DW_PCIE_PCIE_CAP_ACTIVE_STATE_LINK_PM_CONTROL));
			HAL_PCIE_EP_CONFIG_WRITE32(PCIeIdx, REG_DW_PCIE_LINK_CONTROL_LINK_STATUS, value32);

			// Check link
			HalPCIeCheckLink(PCIeIdx);
			break;

		case PCIE_TEST_SUB_ITEM_POWER_SAVING_ENABLE_ASPM_8812B:
			// enable ASPM of 8812B
			value32 = HAL_PCIE_EP_CONFIG_READ32(PCIeIdx, REG_DW_PCIE_ACK_F_ASPM_CTRL);
			value32 = value32 | BIT(31);
			HAL_PCIE_EP_CONFIG_WRITE32(PCIeIdx, REG_DW_PCIE_ACK_F_ASPM_CTRL, value32);

			value32 = HAL_PCIE_EP_CONFIG_READ32(PCIeIdx, 0x718);
			value32 = value32 | 0x1800;
			HAL_PCIE_EP_CONFIG_WRITE32(PCIeIdx, 0x718, value32);

			break;

		default:

			break;

	}

	return;
}


static void
PCIeISRVerification(IN u32 PCIeIdx)
{
	HalPCIeISRVerification(PCIeIdx);

	return;
}


void
PCIeTestApp(IN void *PCIeTestData)
{
	PPCIE_VERI_PARA pPCIeVerParaTmp = (PPCIE_VERI_PARA)PCIeTestData;
	u32 			item = 0;
	u32				subItem = 0;
	u32				PCIeIdx = 0;
	u32				loopCnt = 0;

	item = pPCIeVerParaTmp->VeriItem;
	subItem = pPCIeVerParaTmp->VeriSubItem;
	PCIeIdx = pPCIeVerParaTmp->VeriPCIeIdx;
	loopCnt = pPCIeVerParaTmp->VeriLoopCnt;

	if(PCIeIdx >= PCIE_VERI_PARA_PCIE_INDEX_MAX)
	{
		printf("Unsupported PCIe Index!\r\n");
	}
	
	switch(item)
	{
		case PCIE_TEST_ITEM_INIT:
			switch(subItem)
			{
				case PCIE_TEST_SUB_ITEM_COLD_RESET:
					PCIeColdReset(PCIeIdx);
					break;
#ifdef CONFIG_RTL8197G
                                case PCIE_TEST_SUB_ITEM_WARM_RESET:
                                        HalPCIeWarmReset(PCIeIdx);
                                        break;
                                case PCIE_TEST_SUB_ITEM_HOT_RESET:
                                        HalPCIeHotReset(PCIeIdx);
                                        break;
#endif
				default:
					printf("Unsupported Test Sub-Item!\r\n");
					return;
			}
			break;
	 	case PCIE_TEST_ITEM_TRX_LOOPBACK:
			switch(subItem)
			{
				case PCIE_TEST_SUB_ITEM_TRX_LOOPBACK:
					PCIeHostDataLoopback(PCIeIdx, loopCnt);
					break;
				default:
					printf("Unsupported Test Sub-Item!\r\n");
					return;
			}
			break;
			
	 	case PCIE_TEST_ITEM_POWER_SAVING:
			switch(subItem)
			{
				case PCIE_TEST_SUB_ITEM_POWER_SAVING_D0_L0:
				case PCIE_TEST_SUB_ITEM_POWER_SAVING_D3HOT_L1:
				case PCIE_TEST_SUB_ITEM_POWER_SAVING_D3COLD_L2:
				case PCIE_TEST_SUB_ITEM_POWER_SAVING_L2_L3_HANDSHAKE:
				case PCIE_TEST_SUB_ITEM_POWER_SAVING_L0_L1_L0:
				case PCIE_TEST_SUB_ITEM_POWER_SAVING_L0_L2_L0:
				case PCIE_TEST_SUB_ITEM_POWER_SAVING_ASPM_L0S:
				case PCIE_TEST_SUB_ITEM_POWER_SAVING_ASPM_L1:
				case PCIE_TEST_SUB_ITEM_POWER_SAVING_ASPM_L0S_L1:
				case PCIE_TEST_SUB_ITEM_POWER_SAVING_DISABLE_ASPM:
                                case PCIE_TEST_SUB_ITEM_POWER_SAVING_ENABLE_ASPM_8812B:
					PCIePowerSaving(PCIeIdx, subItem);
					break;
				default:
					printf("Unsupported Test Sub-Item!\r\n");
					return;
			}
			break;

	 	case PCIE_TEST_ITEM_INTERRUPT:
			switch(subItem)
			{
				case PCIE_TEST_SUB_ITEM_INTERRUPT:
					PCIeISRVerification(PCIeIdx);
					break;
				default:
					printf("Unsupported Test Sub-Item!\r\n");
					return;
			}
			break;

	 	default:
			printf("Unsupported Test Item!\r\n");
			return;

	}

	return;
}

 
