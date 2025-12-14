
#include <asm/types.h>
#include <basic_types.h>
#include "peripheral.h"
#include "hal_api.h"
#include "hal_irqn.h"

extern HAL_WLAN_OB hal_wlan_OB[2];
extern WLAN_DATA_ELEMENT WlanDataEle[2];

u4Byte BcnOkCounter = 0;
u4Byte TxbufCheckIndex = 0;
u4Byte TxbufRandomIndex = 0;
u4Byte RxTimeStamp = 0;
u1Byte Global_CPWM=0;
u1Byte Global_RPWM=0;
u1Byte Global_CPWM2=0;
u1Byte Global_RPWM2=0;
u2Byte LB_Count_1 = 0; // for FW RPWM CPWM LB test
u2Byte LB_Count_2 = 0; // for FW RPWM CPWM LB test
extern unsigned int securityCAM_result;

int
InitPCIE98F(
)
{

#ifdef CONFIG_CMD_RTK_PCIE
    HalPCIeOpInit();

	return HalPCIeRCInit(0);

#if 0 //alredy add in pcie init code
    u32 Bar0, Bar2;
    u32 MapIOBase, MapMemBase;
    u8 PCIeIdx;
    MapIOBase = PCIE0_IO_BASE;
    MapMemBase = 0xF1020000;//PCIE0_MEM_BASE;
    PCIeIdx = 0;

    // Set BAR
    Bar0 = PADDR(MapIOBase | BIT_CTRL_DW_PCIE_BAR0_MEM_IO(DW_PCIE_BAR_MEM_IO_IO));
    Bar2 = PADDR(MapMemBase | BIT_CTRL_DW_PCIE_BAR0_MEM_IO(DW_PCIE_BAR_MEM_IO_MEM) | BIT_CTRL_DW_PCIE_BAR0_TYPE(DW_PCIE_BAR_TYPE_64BIT_ADDRESS));
    
    HalPCIeEPConfigWrite32(PCIeIdx, REG_DW_PCIE_BAR0, Bar0);
    HalPCIeEPConfigWrite32(PCIeIdx, REG_DW_PCIE_BAR2, Bar2);
#endif
#else
    printf("Warning, init PCIE need to select PCIE IP in menuconfig!\n");
    return 0;

#endif
    
}


VOID 
InterruptHandleExtend(
    VOID    *Data  
)
{
    u4Byte Value32 = 0;
    u4Byte Hisr = 0;
    u4Byte Himr = 0;    
    u2Byte WritePoint, ReadPoint;

    PHAL_WLAN_ADDR pAddressAdapter;

    PHAL_WLAN_OB wlanOB = (PHAL_WLAN_OB) Data;
    
    pAddressAdapter = &wlanOB->WlanBaseAddress;
    u1Byte interface_sel = FindInterface(pAddressAdapter->WLBASE_ADDR);

    Hisr =  HAL_WLAN_R32(REG_HISR0);
    Himr =  HAL_WLAN_R32(REG_HIMR0);    

    HAL_WLAN_W32(REG_HISR0,Hisr);
  
    //HAL_WLAN_W32(0x140,0x0);        
    //HAL_WLAN_W32(REG_HIMR0,0x00000000);        
    //HAL_WLAN_W32(REG_HISR1,0xffff);        
    //HAL_WLAN_W32(REG_FE1IMR,0xffff);    
    //HAL_WLAN_W32(REG_FE1ISR,0x00000000);    
    //HAL_WLAN_W32(REG_HIMR0,0x0);    
    //GEN_RTL_W32(0xb8003004,GEN_RTL_R32(0xb8003004));    
    //GEN_RTL_W32(0xb8003000,0);        
    //InterruptDis(&wlanIrqHandle);    
   
    
    if (Hisr & BIT(0)) {
        u4Byte Check1Value32,Check2Value32;
            
        Value32 = HAL_WLAN_R32(REG_RXQ_RXBD_IDX);
        WritePoint = (u2Byte)(Value32 >> 16);
        ReadPoint = (u2Byte)(Value32 & 0xFFFF);
   

        WLIsrReceivePacket(pAddressAdapter);

        RxTimeStamp = HAL_WLAN_R32(0x560);

        TxbufCheckIndex++;
#if 0
        switch(TxbufCheckIndex & 0x3) {
            case 0:
                //dprintf("V1\n");
               (*(volatile unsigned int *) (0xB86500E0)) = (0x5A5A5A5A);
               (*(volatile unsigned int *) (0xB86500F0)) = (0x5A5A5A5A);

                Check1Value32 = (*(volatile unsigned int *) (0xB86500E0));
                Check2Value32 = (*(volatile unsigned int *) (0xB86500F0));

                if (Check1Value32 != 0x5A5A5A5A) {
                    dprintf("Check1Value32 IO TXBfu Error 0!!!!\n");                
                }
                if (Check1Value32 != 0x5A5A5A5A) {
                    dprintf("Check2Value32 IO TXBfu Error 0!!!!\n");                                    
                }
                
                break;            
            case 1:
                //dprintf("V2\n");
               (*(volatile unsigned int *) (0xB86500E0)) = (0xFFFFFFFF);
               (*(volatile unsigned int *) (0xB86500F0)) = (0xFFFFFFFF);

                Check1Value32 = (*(volatile unsigned int *) (0xB86500E0));
                Check2Value32 = (*(volatile unsigned int *) (0xB86500F0));                

                if (Check1Value32 != 0xFFFFFFFF) {
                    dprintf("Check1Value32 IO TXBfu Error 1!!!!\n");                
                }
                if (Check1Value32 != 0xFFFFFFFF) {
                    dprintf("Check2Value32 IO TXBfu Error 1!!!!\n");                                    
                }                
                break;
            case 2:
                //dprintf("V3\n");                
                (*(volatile unsigned int *) (0xB86500E0)) = (0);
                (*(volatile unsigned int *) (0xB86500F0)) = (0);
                
                 Check1Value32 = (*(volatile unsigned int *) (0xB86500E0));
                 Check2Value32 = (*(volatile unsigned int *) (0xB86500F0));  


                if (Check1Value32 != 0) {
                    dprintf("Check1Value32 IO TXBfu Error 2!!!!\n");                
                }
                if (Check1Value32 != 0) {
                    dprintf("Check2Value32 IO TXBfu Error 2!!!!\n");                                    
                }                
                
                break;
            case 3:
                //dprintf("V4\n");
                TxbufRandomIndex++;
                (*(volatile unsigned int *) (0xB86500E0)) = (TxbufRandomIndex);

                 Check1Value32 = (*(volatile unsigned int *) (0xB86500E0));
       
                if (Check1Value32 != TxbufRandomIndex) {
                    dprintf("Check1Value32 IO TXBfu Error 3!!!!\n");                
                }
                break;
            default:
                
                break;
        }

#endif
        Value32 = HAL_WLAN_R32(REG_RXQ_RXBD_IDX);
        WritePoint = (u2Byte)(Value32 >> 16);
        ReadPoint = (u2Byte)(Value32 & 0xFFFF);

         // Re send packet again

#if CONFIG_TX_LOOPBACK_ALWAYSLOOP
        CmdSendTest(1,NULL);
#endif 
        //RTL_INT_DBG_LOG("<==+ A RX wp: %d; rp: %d\n", WritePoint, ReadPoint);
    }

    // blow is for page0 interrupt
    //dprintf("REG[B4]=%x\n",Hisr);

    // check CPWM ISR
    if(Hisr & BIT_CPWM) {
        // write one clear
        HAL_WLAN_W32(REG_HISR0, Value32 | BIT_CPWM);
        
        
        Global_CPWM = HAL_WLAN_R8(REG_CPWM+3);
        Global_RPWM++;
        if((Global_CPWM & 0x7F) != (Global_RPWM & 0x7F)) {
            RTL_INT_DBG_LOG("loopback fail RPWM =%x, CPWM =%x\n",Global_RPWM,Global_CPWM);    
        }
        else {
            RTL_INT_DBG_LOG("vaule=[%x] CPWM is equal RPWM!\n",Global_RPWM);
        }
        
        
        //Global_RPWM++;
        //Global_CPWM++;
        
        WlanDataEle[interface_sel].FwElement.RPWM1LB.successed_count++;
        
        if (WlanDataEle[interface_sel].FwElement.RPWM1LB.successed_count<WlanDataEle[interface_sel].FwElement.RPWM1LB.target_count){

            if (HAL_WLAN_R8(REG_HCI_HRPWM1_V1) & PS_TOGGLE) { 
                HAL_WLAN_W8(REG_HCI_HRPWM1_V1, Global_RPWM & (~PS_TOGGLE));
            }
            else {
                HAL_WLAN_W8(REG_HCI_HRPWM1_V1, (Global_RPWM | PS_TOGGLE));
            }
            //LB_Count_1++;
        }
        else{
            RTL_INT_DBG_LOG("int#:%d RPWM1_CPWM1_LB finished, LB target cnt=%d, LB successed cnt=%d\n",interface_sel,
                WlanDataEle[interface_sel].FwElement.RPWM1LB.target_count,WlanDataEle[interface_sel].FwElement.RPWM1LB.successed_count);
			    //WlanDataEle[interface_sel].FwElement.RPWM1LB.successed_count = 0;
        }

    }
    if(Hisr & BIT_CPWM2) {
        // write one clear
        HAL_WLAN_W32(REG_HISR0, Value32 | BIT_CPWM2);
        
        
        Global_CPWM2 = HAL_WLAN_R8(REG_CPWM2);
        Global_RPWM2++;
        if((Global_CPWM2 & 0x7F) != (Global_RPWM2 & 0x7F)) {
            RTL_INT_DBG_LOG("loopback fail RPWM2 =%x, CPWM2 =%x\n",Global_RPWM2,Global_CPWM2);    
        }
        else {
            RTL_INT_DBG_LOG("vaule=[%x] CPWM2 is equal RPWM!\n",Global_RPWM2);
        }
        
        //Global_RPWM2++;
        //Global_CPWM2++;
        WlanDataEle[interface_sel].FwElement.RPWM2LB.successed_count++;
         if (WlanDataEle[interface_sel].FwElement.RPWM2LB.successed_count<WlanDataEle[interface_sel].FwElement.RPWM2LB.target_count){
            if (HAL_WLAN_R16(REG_HCI_HRPWM2_V1) & BIT(15)) {            
                HAL_WLAN_W16(REG_HCI_HRPWM2_V1, Global_RPWM2 & (~ BIT(15)));
            }
            else {            
                HAL_WLAN_W16(REG_HCI_HRPWM2_V1, (Global_RPWM2 | BIT(15)));
            }
            
        }
        else{
            RTL_INT_DBG_LOG("RPWM2_CPWM2_LB finished LB target cnt=%d, LB successed cnt=%d\n",
                WlanDataEle[interface_sel].FwElement.RPWM2LB.target_count,WlanDataEle[interface_sel].FwElement.RPWM2LB.successed_count);
			    //WlanDataEle[interface_sel].FwElement.RPWM2LB.successed_count=0;
        }
    
    }

    if (Hisr & BIT(26)) {
        //dprintf("Transmit Beacon Error!!!\n");
        Value32 = HAL_WLAN_R32(REG_HIMR0);
        Value32 &= (~BIT_TXBCN0ERR_MSK); 
        RTL_INT_DBG_LOG("Disable Beacon Error IMR value: 0x%x\n",Value32);    
        //HAL_WLAN_W32(REG_HIMR0,Value32);
    }

    if (Hisr & BIT(25)) {
        dprintf("Transmit Beacon ok!!!\n");
        BcnOkCounter++;
        if (BcnOkCounter > 10) {
            Value32 = HAL_WLAN_R32(REG_HIMR0);
            Value32 &= (~BIT_TXBCN0OK_MSK); 
            RTL_INT_DBG_LOG("Disable Beacon Ok IMR value: 0x%x\n",Value32);    
            HAL_WLAN_W32(REG_HIMR0,Value32);
        }
    }

    if (Hisr & BIT(20)) {
        RTL_INT_DBG_LOG("Beacon Early ISR\n");
        if (BcnOkCounter > 10) {
            Value32 = HAL_WLAN_R32(REG_HIMR0);
            Value32 &= (~BIT_BCNDMAINT0_MSK); 
            RTL_INT_DBG_LOG("Disable Beacon Early IMR value: 0x%x\n",Value32);    
            HAL_WLAN_W32(REG_HIMR0,Value32);        
        }
    }


    if(Hisr & BIT_C2HCMD) {
        RTL_INT_DBG_LOG("Get C2H interrupt!!");
         // write one clear
        HAL_WLAN_W32(REG_HISR0, Hisr | BIT_C2HCMD);
        RTL_INT_DBG_LOG("0x1a0=[%x] \n",HAL_WLAN_R32(0x1a0));
        RTL_INT_DBG_LOG("0x1a4=[%x] \n",HAL_WLAN_R32(0x1a4));    
        HAL_WLAN_W8(0x1af,0x0);
    }

    //RTL_INT_DBG_LOG("ISR over \n");    
    
}


RTK_STATUS 
InterruptInitExtend(
    u1Byte  interface_sel,
    VOID    *Data  
)
{    
    PHAL_WLAN_ADDR pAddressAdapter;


    /* Set interrupt routing register */
    u4Byte Value32;
    IRQ_HANDLE          wlanIrqHandle;
    wlanIrqHandle.Data = (u32) (&hal_wlan_OB[interface_sel]);
    //wlanIrqHandle.IrqNum = WLAN_IRQ;
    
	//3 need check IRQ number for different chip
    if(interface_sel==0)
        wlanIrqHandle.IrqNum = wifi_top;//WLAN_MAC_IP_IRQ;
    else
        wlanIrqHandle.IrqNum = usbotg_lx_clk;//FPGA //PCIE0_IP_IRQ;
    wlanIrqHandle.IrqFun = (IRQ_FUN) InterruptHandleExtend;
    wlanIrqHandle.Priority = 0;

     // TODO check why
    //Set IRR1_REG
   // (*(volatile u4Byte *)(0xb8003014)) |= (3<<20); 
   // request_IRQ(WLMAC_IRQ, &irq_wlmac29,&(WL[0]));

   //mips
   //InterruptRegister(&wlanIrqHandle);
   //InterruptEn(&wlanIrqHandle);
	
   //arm
   irq_install_handler(wlanIrqHandle.IrqNum, InterruptHandleExtend, &hal_wlan_OB[interface_sel]);
   
   pAddressAdapter = &hal_wlan_OB[interface_sel].WlanBaseAddress; 
   
    Value32 = HAL_WLAN_R32(REG_HIMR0);
//        Value32 |= HIMR_ROK | HIMR_BcnInt |HIMR_TBDOK | HIMR_TBDER | HIMR_CPWM | HIMR_C2HCMD; 
    Value32 |= BIT_RXOK_MSK |BIT_TXBCN0OK_MSK | BIT_TXBCN0ERR_MSK | BIT_CPWM_MSK | BIT_C2HCMD_MSK; 
//          Value32 |= HIMR_TBDOK | HIMR_TBDER | HIMR_CPWM | HIMR_C2HCMD; 

    //RTL_CMD_DBG_LOG("Set WL DMA IMR value: 0x%x\n",Value32);

    //HAL_WLAN_W32(REG_HIMR0,Value32);

    // HISR - turn all on
    Value32 = 0x1;
    //HAL_WLAN_W32(REG_HISR0, Value32);
    // HIMR - turn all on
    HAL_WLAN_W32(REG_HIMR0, Value32);
    //HAL_WLAN_W32(REG_FE1IMR,0x10000);

    RTL_CMD_DBG_LOG("Set WL DMA IMR value: 0x%x\n",HAL_WLAN_R32(REG_HIMR0));

    //Value32 = HAL_WLAN_R32(0x550);
    //Value32 |= 0x8;
    //HAL_WLAN_W32(0x550,Value32);
}

RTK_STATUS 
MBIST_Test(
    VOID    *pAdapter,
    u8 CmdID
)
{
    PHAL_WLAN_ADDR pAddressAdapter= (PHAL_WLAN_ADDR) pAdapter;

    u1Byte veri_result = _EXIT_FAILURE;

    if(CmdID==1) //MBSSID CAM
    {
        //need to ask DD for MBIST flow
        veri_result = _EXIT_SUCCESS;
    }
    else if(CmdID==2) //security cam
    {
        //need to ask DD for MBIST flow
        veri_result = _EXIT_SUCCESS;
    }
    else
        RTL_CMD_DBG_LOG("wrong cmd id\n");

    return veri_result;
}



RTK_STATUS
InitMBIDCAM_98F(
    IN  VOID    *pAdapter,
    IN  u1Byte  CAMsize
)
{
    PHAL_WLAN_ADDR pAddressAdapter = (PHAL_WLAN_ADDR) pAdapter;
    u1Byte interface_sel = FindInterface(pAddressAdapter->WLBASE_ADDR);
    u2Byte j;
	u4Byte command = BIT_MBIDCAM_POLL | BIT_MBIDCAM_WT_EN;   // BIT22 is valid bit


    //3 we set all the entry to 0 and invalid, 
    //3 only set to valid when test needed

    for(j=0;j<CAMsize;j++){

		HAL_WLAN_W32(REG_MBIDCAMCFG_1, 0x0);
		HAL_WLAN_W32(REG_MBIDCAMCFG_2, command | BIT_MBIDCAM_ADDR_V2(j));
    }

    return _EXIT_SUCCESS;
}


RTK_STATUS 
CfgMBSSIDTable_98F(
    IN  VOID    *pAdapter,
    u1Byte CAM_addr,
    u1Byte* pMACaddress
)
{
    PHAL_WLAN_ADDR pAddressAdapter = (PHAL_WLAN_ADDR) pAdapter;
    u1Byte interface_sel = FindInterface(pAddressAdapter->WLBASE_ADDR);
    u2Byte j;
	u4Byte   temp4byte;
    u2Byte   temp2byte;
	u4Byte command = BIT_MBIDCAM_POLL | BIT_MBIDCAM_WT_EN | BIT_MBIDCAM_VALID_V1;   // BIT18 is valid bit

	
	//====write CAM=========================
	temp4byte = pMACaddress[0] | (pMACaddress[1]<<8) | (pMACaddress[2]<<16) | (pMACaddress[3]<<24);
	HAL_WLAN_W32(REG_MBIDCAMCFG_1, temp4byte);

    temp2byte = pMACaddress[4] | (pMACaddress[5]<<8);		
	HAL_WLAN_W32(REG_MBIDCAMCFG_2, command | temp2byte | BIT_MBIDCAM_ADDR_V2(CAM_addr));

	return TRUE;
}

RTK_STATUS
MBIDCAM_Extend_test(
    IN  VOID    *pAdapter
)
{
    //1 MBIDCAM can not be verified by check if ack is received in loopback mobe
    PHAL_WLAN_ADDR pAddressAdapter = (PHAL_WLAN_ADDR) pAdapter;
    u1Byte interface_sel = FindInterface(pAddressAdapter->WLBASE_ADDR);
    u1Byte MAC_addr[6]={0x0,0x0,0x0,0x0,0x0,0x2};
    u32 pkt_num;
    u1Byte AcID = 0;
    u4Byte Value32;
    u4Byte cr_bak;
    LB_MODE LbMode_bak;
    u1Byte cast_type;
	u4Byte   Chose  = 0;
    u1Byte RxInterface_sel;
    u1Byte result = _EXIT_FAILURE;
    //set macid match to accept pkt
    if(WlanDataEle[interface_sel].LbMode==TwoMac){
			RxInterface_sel=interface_sel ^ 0x1;
            pAddressAdapter = &hal_wlan_OB[RxInterface_sel].WlanBaseAddress; 
    }
    
    HAL_WLAN_W32(REG_RCR, (HAL_WLAN_R32(REG_RCR) & ~BIT_AAP) | BIT_ENMBID); //to not accept all, and set check bssid cam to accept packet, dont need to write 0x610
    cast_type = WlanDataEle[interface_sel].MacSettingPort.Port[Port_1][0].address_info.cast_type;
    WlanDataEle[interface_sel].MacSettingPort.Port[Port_1][0].address_info.cast_type =UNICAST;

    if(WlanDataEle[interface_sel].LbMode!=TwoMac){
		Value32 = HAL_WLAN_R32(REG_CR);

        Value32 &= ~(BIT_MASK_LBMODE << BIT_SHIFT_LBMODE);
        WlanDataEle[interface_sel].LbMode = MAC_Delay_LB;
        Chose = BIT_LBMODE(LOOPBACK_MAC_DELAY);
        Value32 |= Chose;
        HAL_WLAN_W32(REG_CR, Value32);

        HAL_WLAN_W16(REG_WMAC_LBK_BUF_HD_V1, 0x300);//temp set 0x300 for test
	}   
    //3 case1 test entry 40
    CfgMBSSIDTable_98F(pAddressAdapter,40,MAC_addr);
	if(WlanDataEle[interface_sel].LbMode==TwoMac){
		//switch to AP    
		pAddressAdapter = &hal_wlan_OB[interface_sel].WlanBaseAddress; 
	}
    //AP tx 1 pkt
    pkt_num = 1;
    if(WlanDataEle[interface_sel].LbMode!=TwoMac)
        result = TxRutineForTestall(interface_sel, pkt_num, AcID);
    else{
        sendpacket_rutine(pAddressAdapter,pkt_num,AcID);
        result = _EXIT_SUCCESS;
    }
    if(result == _EXIT_FAILURE){
        goto EXIT;
        
    }
    //3 check if STA RX 1 pkt , by eyes for now
    RTL_CMD_DBG_LOG("case1 test entry 40, check if AP RX 1 pkt , by eyes for now\n");
    mdelay(10);
    
    //3 case2 test entry 71

    if(WlanDataEle[interface_sel].LbMode==TwoMac){
        pAddressAdapter = &hal_wlan_OB[RxInterface_sel].WlanBaseAddress;
    }
    CfgMBSSIDTable_98F(pAddressAdapter,40,0x0); //although entry 40 is already been set to valid, but mac address is set 0, so it's ok.
    CfgMBSSIDTable_98F(pAddressAdapter,71,MAC_addr);
	if(WlanDataEle[interface_sel].LbMode==TwoMac){    
		//switch to AP   
		pAddressAdapter = &hal_wlan_OB[interface_sel].WlanBaseAddress; 
	}
    //AP tx 1 pkt
    pkt_num = 1;
    if(WlanDataEle[interface_sel].LbMode!=TwoMac)
        result = TxRutineForTestall(interface_sel, pkt_num, AcID);
    else{
        sendpacket_rutine(pAddressAdapter,pkt_num,AcID);
        result = _EXIT_SUCCESS;
    }

    //3 check if STA RX 1 pkt , by eyes for now
    RTL_CMD_DBG_LOG("case2 test entry 71, check if AP RX 1 pkt , by eyes for now\n");
EXIT:
    mdelay(10);
    pAddressAdapter = &hal_wlan_OB[RxInterface_sel].WlanBaseAddress;
    HAL_WLAN_W32(REG_RCR, (HAL_WLAN_R32(REG_RCR) | BIT_AAP) & ~BIT_ENMBID);
    WlanDataEle[interface_sel].MacSettingPort.Port[Port_1][0].address_info.cast_type =cast_type;


    return result;
}

RTK_STATUS
SecurityCAM_Extend_test(
    IN  VOID    *pAdapter,
    IN  PHAL_WLAN_DAT_ADAPTER pHalWLANDatAdapter
)
{
    PHAL_WLAN_ADDR pAddressAdapter = (PHAL_WLAN_ADDR) pAdapter;
    u1Byte  interface_sel = FindInterface(pAddressAdapter->WLBASE_ADDR); 
    u1Byte  veri_result = _EXIT_FAILURE;
    u4Byte   Chose  = 0;
    u4Byte  Value32;
    u4Byte  cam_size = SECURITY_CAM_ENTRY_NUM_8198F * SECURITY_CAM_ENTRY_SIZE;
    u4Byte command = BIT_SECCAM_WE | BIT_SECCAM_POLLING;   // Write Command
    u4Byte fill_i;
    u4Byte cam_start,cam_stop,wapi_cam_start, wapi_cam_stop;
    u4Byte pkt_num;
    u4Byte temp4byte;
    u1Byte AcID = 0;
	u1Byte encryption_bk;
    u1Byte casttype_bk;
    u1Byte encryptiontype_bk;
    u1Byte encryptionKeyid_bk;
	char buf0[5];
    char buf1[5];
    char buf2[5];
    char buf3[5];
    char buf4[5];
    char* buf0_result;
    char* buf1_result;
    char* buf2_result;
    char* buf3_result;
    char* buf4_result;
    u1Byte lengthopt = 0;
    u1Byte payloadopt = 1;
    u1Byte fixlength = 0x100; // 256
    u1Byte rate = 0x13;
    u1Byte lengthopt_bk = 0;
    u1Byte payloadopt_bk = 1;
    u1Byte fixlength_bk = 0x100; // 256
    u1Byte rate_bk = 0x13;
    u32 i = 0;
    securityCAM_result = 0;
    
    u4Byte CAMData_extend_Wapi[16]=
    {
        //addr2
        0x00008019,0x02000000,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB, //set valid for addr = 0x2, key id = 0
        0x00008039,0x02000000,0xACACACAC,0xACACACAC,0xACACACAC,0xACACACAC,0xACACACAC,0xACACACAC  //set valid for addr = 0x2, key id = 0
    };
    u4Byte CAMData_extend_Security[32]=
    {
        //addr2
        0x00008004,0x02000000,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB, // WEP40 Key, key id = 0
        0x00008009,0x02000000,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB, // TKIP key, key id = 1
        0x00008012,0x02000000,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB, // AES key, key id = 2
        0x00008017,0x02000000,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB // WEP104 key, key id = 3
    };

    //HAL_WLAN_W32(REG_RCR, (HAL_WLAN_R32(REG_RCR) | BIT_ENMBID) & ~BIT_AAP); //to not accept all, and enable match in mbssid cam

    //encryption
    HAL_WLAN_W8(REG_SECCFG,HAL_WLAN_R8(REG_SECCFG) | BIT_TXUHUSEDK | BIT_TXENC | BIT_TXBCUSEDK); 
    //decryption
    HAL_WLAN_W8(REG_SECCFG,HAL_WLAN_R8(REG_SECCFG) | BIT_RXUHUSEDK | BIT_RXDEC | BIT_RXBCUSEDK); 
    //enable search by keyID
    HAL_WLAN_W16(REG_SECCFG,HAL_WLAN_R16(REG_SECCFG) | BIT_CHK_KEYID); 

    if(WlanDataEle[interface_sel].LbMode!=TwoMac){
		Value32 = HAL_WLAN_R32(REG_CR);
		Value32 &= ~(BIT_MASK_LBMODE << BIT_SHIFT_LBMODE);
		WlanDataEle[interface_sel].LbMode = MAC_Delay_LB;
		Chose = BIT_LBMODE(LOOPBACK_MAC_DELAY);
		Value32 |= Chose;
		HAL_WLAN_W32(REG_CR, Value32);

		HAL_WLAN_W16(REG_WMAC_LBK_BUF_HD_V1, 0x300);//temp set 0x300 for test
	}
	encryption_bk = WlanDataEle[interface_sel].MacSettingPort.Port[Port_1][0].encryption_info.encryption;
    casttype_bk = WlanDataEle[interface_sel].MacSettingPort.Port[Port_1][0].address_info.cast_type;
    encryptiontype_bk = WlanDataEle[interface_sel].MacSettingPort.Port[Port_1][0].encryption_info.type;
    encryptionKeyid_bk = WlanDataEle[interface_sel].MacSettingPort.Port[Port_1][0].encryption_info.keyid;

    WlanDataEle[interface_sel].MacSettingPort.Port[Port_1][0].encryption_info.encryption = HW; //only test HW here, we dont store key in data structure
    hal_wlan_OB[interface_sel].wlanHalData.encryption = WlanDataEle[interface_sel].MacSettingPort.Port[Port_1][0].encryption_info.encryption;
    WlanDataEle[interface_sel].MacSettingPort.Port[Port_1][0].address_info.cast_type =UNICAST;
    
    for(i=0;i<13;i++){
        lengthopt_bk = WlanDataEle[interface_sel].MacSettingGen.source_data.packet[i].lengthopt;
        payloadopt_bk = WlanDataEle[interface_sel].MacSettingGen.source_data.packet[i].payloadopt;
        fixlength_bk = WlanDataEle[interface_sel].gTxLen;
        rate_bk = WlanDataEle[interface_sel].MacSettingGen.source_data.packet[i].rate;
    }

    buf0_result = ultostr(buf0, interface_sel,16,0);
    buf1_result = ultostr(buf1, lengthopt,16,0);
    buf2_result = ultostr(buf2, payloadopt,16,0);  
    buf3_result = ultostr(buf3, fixlength,16,0);
    buf4_result = ultostr(buf4, rate,16,0);
    u8* para[5]= {buf0_result,buf1_result,buf2_result,buf3_result};

    CmdSendTest(5,para); //set to small packet
    
    u1Byte test_loop;
    
    for(test_loop=0;test_loop<5;test_loop++){
        
        //3 set security cam
        switch(test_loop)
        {
            case 0:
                RTL_CMD_DBG_LOG("==== WEP40 ====\n");
                WlanDataEle[interface_sel].MacSettingPort.Port[Port_1][0].encryption_info.type = WEP40;
                WlanDataEle[interface_sel].MacSettingPort.Port[Port_1][0].encryption_info.keyid=KEY0;
            break;
            case 1:
                RTL_CMD_DBG_LOG("==== TKIP ====\n");
                WlanDataEle[interface_sel].MacSettingPort.Port[Port_1][0].encryption_info.type = TKIP;
                WlanDataEle[interface_sel].MacSettingPort.Port[Port_1][0].encryption_info.keyid=KEY1;
            break;                    
            case 2:
                RTL_CMD_DBG_LOG("==== AES ====\n");
                WlanDataEle[interface_sel].MacSettingPort.Port[Port_1][0].encryption_info.type = AES;
                WlanDataEle[interface_sel].MacSettingPort.Port[Port_1][0].encryption_info.keyid=KEY2;
            break;
            case 3:
                RTL_CMD_DBG_LOG("==== WEP104 ====\n");
                WlanDataEle[interface_sel].MacSettingPort.Port[Port_1][0].encryption_info.type = WEP104;
                WlanDataEle[interface_sel].MacSettingPort.Port[Port_1][0].encryption_info.keyid=KEY3;
            break;
            case 4:
                RTL_CMD_DBG_LOG("==== WAPI ====\n");
                WlanDataEle[interface_sel].MacSettingPort.Port[Port_1][0].encryption_info.type = WAPI;
                WlanDataEle[interface_sel].MacSettingPort.Port[Port_1][0].encryption_info.keyid=KEY1;
            break;
                
        }
        
        hal_wlan_OB[interface_sel].wlanHalData.security_type = WlanDataEle[interface_sel].MacSettingPort.Port[Port_1][0].encryption_info.type;  

        wapi_cam_start = 124*SECURITY_CAM_ENTRY_SIZE; //test CAM 124 125           //32*8 //test CAM 32 33
        wapi_cam_stop = 126*SECURITY_CAM_ENTRY_SIZE;                       //34*8
        cam_start = 124*SECURITY_CAM_ENTRY_SIZE; //test CAM 124 125 126 127               //32*8 //test CAM 32 33 34 35
        cam_stop = 128*SECURITY_CAM_ENTRY_SIZE;                               //36*8
            
        for(fill_i=0;fill_i<cam_size;fill_i++){
            if(WAPI == WlanDataEle[interface_sel].MacSettingPort.Port[Port_1][0].encryption_info.type){
                
                if((fill_i >= wapi_cam_start) && (fill_i <wapi_cam_stop))
                    temp4byte = CAMData_extend_Wapi[fill_i-wapi_cam_start];
                else
                    temp4byte = 0x0;
            }
            else{
                
                if((fill_i >= cam_start) && (fill_i <cam_stop)) 
                    temp4byte = CAMData_extend_Security[fill_i-cam_start];
                else
                    temp4byte = 0x0;
            }
            //====write CAM=========================
            
            HAL_WLAN_W32(REG_CAMWRITE,temp4byte);
            temp4byte =(command | fill_i);
            HAL_WLAN_W32(REG_CAMCMD,temp4byte);
            //====read CAM========================
            //*temp4=(command2 |j);
            //HAL_WLAN_W32(REG_CAMCMD, *temp4);
            //*temp2=HAL_WLAN_R32(REG_CAMREAD);

        }

        //3 tx pkt
            
        pkt_num = 1;
        //sendpacket_rutine(pAddressAdapter,pkt_num,AcID);
		TxRutineForTestall(interface_sel, pkt_num, AcID);
        //check if print decryption ok
        
    }

    //HAL_WLAN_W32(REG_RCR, (HAL_WLAN_R32(REG_RCR) & ~BIT_ENMBID) | BIT_AAP); 

    WlanDataEle[interface_sel].MacSettingPort.Port[Port_1][0].address_info.cast_type =casttype_bk;
	WlanDataEle[interface_sel].MacSettingPort.Port[Port_1][0].encryption_info.encryption = encryption_bk;
    WlanDataEle[interface_sel].MacSettingPort.Port[Port_1][0].encryption_info.type = encryptiontype_bk;
    WlanDataEle[interface_sel].MacSettingPort.Port[Port_1][0].encryption_info.keyid = encryptionKeyid_bk;
    hal_wlan_OB[interface_sel].wlanHalData.encryption = WlanDataEle[interface_sel].MacSettingPort.Port[Port_1][0].encryption_info.encryption;
    hal_wlan_OB[interface_sel].wlanHalData.security_type = WlanDataEle[interface_sel].MacSettingPort.Port[Port_1][0].encryption_info.type;
    
    hal_wlan_OB[interface_sel].wlanHalOp.HALWLSecurityInit(pAddressAdapter,&hal_wlan_OB[interface_sel].wlanHalData);
    
    if(securityCAM_result==5)
        veri_result = _EXIT_SUCCESS;

    buf0_result = ultostr(buf0, interface_sel,16,0);
    buf1_result = ultostr(buf1, lengthopt_bk,16,0);
    buf2_result = ultostr(buf2, payloadopt_bk,16,0);  
    buf3_result = ultostr(buf3, fixlength_bk,16,0);
    buf4_result = ultostr(buf4, rate_bk,16,0);
    u8* para2[5]= {buf0_result,buf1_result,buf2_result,buf3_result};

    CmdSendTest(5,para2); //set back
    
    return veri_result;
}

RTK_STATUS 
CAM_Extend_test(
    IN  VOID    *pAdapter,
    IN  u1Byte  cmdID,
    IN  PHAL_WLAN_DAT_ADAPTER pHalWLANDatAdapter
)
{
    PHAL_WLAN_ADDR pAddressAdapter = (PHAL_WLAN_ADDR) pAdapter;
    u1Byte interface_sel = FindInterface(pAddressAdapter->WLBASE_ADDR);
    u1Byte veri_result = _EXIT_FAILURE;

    if(cmdID==1) //MBSSID CAM
    {
        veri_result = MBIDCAM_Extend_test(pAddressAdapter);
    }
    else if(cmdID==2) //security CAM
    {
        veri_result = SecurityCAM_Extend_test(pAddressAdapter,pHalWLANDatAdapter);
    }
    else
        RTL_CMD_DBG_LOG("wrong cmd id\n");

    return veri_result;

}

VOID
EnableWlanIP98F(
)
{
    //enable wifi
    HAL_WRITE32(0xf43203b0 , 0, HAL_READ32(0xf43203b0,0) | BIT21 | BIT22 | BIT23);

}

