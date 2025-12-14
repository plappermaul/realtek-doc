
#include <asm/types.h>
#include <basic_types.h>
#include "peripheral.h"
#include "hal_api.h"
#include "hal_irqn.h"

extern HAL_WLAN_OB hal_wlan_OB[2];

//-------------------------SWPS Verification--------------------------------
u1Byte 
GetXorWithCRC(
    IN u1Byte a,
    IN u1Byte b
)
{
	if((a^b)&0x1)
		return 1;
	else
		return 0;
}
	

u1Byte 
CRC5(
    pu1Byte dwInput,
    u1Byte len
)
{
    u1Byte poly5 = 0x05;  
    u1Byte crc5  = 0x1f;
    u1Byte i  = 0x0 ;
    u1Byte udata;
    u1Byte BitCount;


	for(i=0 ; i<len ; i++)
	{
		udata = *(dwInput+i);
		BitCount = 0;
		while(BitCount!=8)
		{        
	        if (GetXorWithCRC(udata>>(BitCount),crc5 >> 4)) // bit4 != bit4?			
	        {				
	            crc5 <<= 1;
	            crc5 ^= poly5;
	        }
	        else
	        {
	            crc5 <<= 1;
			}

	        BitCount++;
		}
    }

    crc5 ^= 0x1f;
  
    return (crc5 & 0x1f);
} 
VOID
SetCRC5ValidBit88XX(
	VOID                *pAdapter,
	u1Byte              group,
	u1Byte              bValid

)
{
	u4Byte CRC5bValidAddr;
	u4Byte CRC5Val;

    PHAL_WLAN_ADDR pAddressAdapter = (PHAL_WLAN_ADDR) pAdapter;
    
	CRC5bValidAddr = (CRC5StartAddr + group * CRC5GroupByteNum);
	// First set CRC buffer addr
	HAL_WLAN_W32(REG_PKTBUF_DBG_CTRL, RPTBufferStartHiAddr + 1);

	if (bValid) {
		HAL_WLAN_W32((RWCtrlBit | (CRC5bValidAddr + 4)), HAL_WLAN_R32(RWCtrlBit | (CRC5bValidAddr + 4)) | CRC5ValidBit);
	} else
		HAL_WLAN_W32((RWCtrlBit | (CRC5bValidAddr + 4)), HAL_WLAN_R32(RWCtrlBit | (CRC5bValidAddr + 4)) & (~CRC5ValidBit));

	// set back RPT buffer start address
	HAL_WLAN_W32(REG_PKTBUF_DBG_CTRL, RPTBufferStartHiAddr);

}
VOID
SetCRC5EndBit88XX(
	VOID                *pAdapter,
	u1Byte              group,
	u1Byte              bEnd
)
{
	u4Byte CRC5bEndAddr;
	u4Byte CRC5Val;

    PHAL_WLAN_ADDR pAddressAdapter = (PHAL_WLAN_ADDR) pAdapter;
    
	CRC5bEndAddr = (CRC5StartAddr + group * CRC5GroupByteNum);
	// First set CRC buffer addr
	HAL_WLAN_W32(REG_PKTBUF_DBG_CTRL, RPTBufferStartHiAddr + 1);

	if (bEnd)
		HAL_WLAN_W32((RWCtrlBit | (CRC5bEndAddr + 4)), HAL_WLAN_R32(RWCtrlBit | (CRC5bEndAddr + 4)) | CRC5EndBit);
	else
		HAL_WLAN_W32((RWCtrlBit | (CRC5bEndAddr + 4)), HAL_WLAN_R32(RWCtrlBit | (CRC5bEndAddr + 4)) & (~CRC5EndBit));

	// set back RPT buffer start address
	HAL_WLAN_W32(REG_PKTBUF_DBG_CTRL, RPTBufferStartHiAddr);

}
VOID
InitMACIDSearch88XX(
	VOID                *pAdapter
)
{
    PHAL_WLAN_ADDR pAddressAdapter = (PHAL_WLAN_ADDR) pAdapter;

	u1Byte i;
	for (i = 0; i < MaxMacIDGroupNum; i++) {
		// set valid bit 0

		SetCRC5ValidBit88XX(pAddressAdapter, i, 0);
		// set end bit 1
		SetCRC5EndBit88XX(pAddressAdapter, i, 1);
	}
}

VOID
SetCRC5ToRPTBuffer88XX(
	VOID                *pAdapter,
	u1Byte              val,
	u4Byte              macID,
	u1Byte              bValid

)
{
	u4Byte CRC5GroupAddr;
	u4Byte CRC5Val;
	u1Byte shift, i, Valid, bEnd;
	u1Byte group;
	u1Byte Mask = 0x1f;
	u1Byte AcrossH_MSK = 0x3;
	u1Byte AcrossL_MSK = 0x7;
    PHAL_WLAN_ADDR pAddressAdapter = (PHAL_WLAN_ADDR) pAdapter;


	// First set CRC buffer addr
	HAL_WLAN_W32(REG_PKTBUF_DBG_CTRL, RPTBufferStartHiAddr + 1);

	shift = (macID % macIDNumperGroup) * CRC5BitNum;
	group = macID / macIDNumperGroup;
	CRC5GroupAddr = CRC5StartAddr + group * CRC5GroupByteNum;

	if (CRCAcrossSHT == shift) {
		// Write group First 32bit [31:30]
		CRC5Val = HAL_WLAN_R32(CRC5GroupAddr | RWCtrlBit);
		CRC5Val =  ((CRC5Val & (~(AcrossH_MSK << CRCAcrossSHT))) | ((val & AcrossH_MSK) << CRCAcrossSHT));
		HAL_WLAN_W32(CRC5GroupAddr | RWCtrlBit, CRC5Val);

		// Write group Second 32bit [2:0]
		CRC5Val = HAL_WLAN_R32((CRC5GroupAddr + 4) | RWCtrlBit);
		CRC5Val =  ((CRC5Val & (~(AcrossL_MSK << 0))) | (((val >> SecondBitSHT)&AcrossL_MSK) << 0));
		HAL_WLAN_W32((CRC5GroupAddr + 4) | RWCtrlBit, CRC5Val);
	} else {
		if (shift < 32) {
			// In group low 32bits
			CRC5Val = HAL_WLAN_R32(CRC5GroupAddr | RWCtrlBit);
			CRC5Val =  ((CRC5Val & (~(Mask << shift))) | ((val & Mask) << shift));
			HAL_WLAN_W32(CRC5GroupAddr | RWCtrlBit, CRC5Val);
		} else {
			// In group high 32bits
			shift -= 32;
			CRC5Val = HAL_WLAN_R32((CRC5GroupAddr + 4) | RWCtrlBit);
			CRC5Val =  ((CRC5Val & (~(Mask << shift))) | ((val & Mask) << shift));
			HAL_WLAN_W32((CRC5GroupAddr + 4) | RWCtrlBit, CRC5Val);
		}
	}

	if (bValid) {

		//set this group valid
		SetCRC5ValidBit88XX(pAddressAdapter, group, TRUE);

		//set all group end bit = 0 before this MACID group
		// EX macid 12 = group1, set group0 end bit = 0
		for (i = 0; i < group; i++) {
			SetCRC5EndBit88XX(pAddressAdapter, i, FALSE);
		}
        SetCRC5EndBit88XX(pAddressAdapter, group, TRUE); //set current group end bit =1
	} 

}



VOID
sendpacket_rutine(
    VOID    *pAdapter,
    u4Byte  pkt_num,
    u1Byte  AcID 
)
{
    PHAL_WLAN_ADDR pAddressAdapter = (PHAL_WLAN_ADDR) pAdapter;

    u1Byte  interface_sel = FindInterface(pAddressAdapter->WLBASE_ADDR);
    u1Byte tx_count = 0;
    TX_LINK_LIST_ELE *LLEle; 
    TX_LINKLIST_ID Queue = (TX_LINKLIST_ID) AcID;
    u16  TXBD_IDX;


#if 1 //#ifdef CONFIG_WHOLE_CODE_IN_SRAM
    switch(Queue)
    {
        case TX_LINKLIST_VO:
            TXBD_IDX = (HAL_WLAN_R16(REG_VOQ_TXBD_IDX))&(TEST_TXBD_NUM-1); 
            break;
        case TX_LINKLIST_VI:
            TXBD_IDX = (HAL_WLAN_R16(REG_VIQ_TXBD_IDX))&(TEST_TXBD_NUM-1); 
            break;
        case TX_LINKLIST_BE:
            TXBD_IDX = (HAL_WLAN_R16(REG_BEQ_TXBD_IDX))&(TEST_TXBD_NUM-1); 
            break;
        case TX_LINKLIST_BK:
            TXBD_IDX = (HAL_WLAN_R16(REG_BKQ_TXBD_IDX))&(TEST_TXBD_NUM-1); 
            break;
    }
        

    LLEle = (TX_LINK_LIST_ELE*)(((u32)&WlanDataEle[interface_sel].StackLLEle[Queue][TXBD_IDX]));
    hal_wlan_OB[interface_sel].wlanHalData.TXBD_IDX = TXBD_IDX;
#else
    LLEle = (TX_LINK_LIST_ELE*)(((u32)&WlanDataEle[interface_sel].StackLLEle)|0xa0000000);
#endif 

    LLEle->PortId = 0;
    if((Queue > TX_LINKLIST_MAG) && (Queue <TX_LINKLIST_MAX))
       LLEle->PortDestNum = Queue-5;
    else
       LLEle->PortDestNum = 0;

    for(tx_count=0;tx_count<pkt_num ;tx_count++){
        if(AcID < 13)
            SendPacket(pAddressAdapter, Queue, (TX_LINK_LIST_ELE*)LLEle);
        else
            RTL_CMD_DBG_LOG("No Support AC Type!!!!!!!\n");
        
    }

}
VOID
set_threshold_rutine(
    VOID    *pAdapter,
    u2Byte  pktTH,
    u2Byte  pstimeTH,
    u2Byte  timeTH 
)
{
    PHAL_WLAN_ADDR pAddressAdapter = (PHAL_WLAN_ADDR) pAdapter;
    u1Byte  interface_sel = FindInterface(pAddressAdapter->WLBASE_ADDR);

    WlanDataEle[interface_sel].SWPSCTRL.SWPS_Packet_Threshold = pktTH;
    WlanDataEle[interface_sel].SWPSCTRL.SWPS_PsTime_Threshold = pstimeTH;
    WlanDataEle[interface_sel].SWPSCTRL.SWPS_Time_Threshold = timeTH; 
    //set gen report threshold
    HAL_WLAN_W16(REG_SWPS_PKT_TH,WlanDataEle[interface_sel].SWPSCTRL.SWPS_Packet_Threshold); 
    HAL_WLAN_W32(REG_SWPS_TIME_TH,(WlanDataEle[interface_sel].SWPSCTRL.SWPS_PsTime_Threshold<<16) | WlanDataEle[interface_sel].SWPSCTRL.SWPS_Time_Threshold ); 
    RTL_CMD_DBG_LOG("set Packet_TH=%x, PsTime_TH=%x, Time_TH=%x\n",WlanDataEle[interface_sel].SWPSCTRL.SWPS_Packet_Threshold,WlanDataEle[interface_sel].SWPSCTRL.SWPS_PsTime_Threshold,WlanDataEle[interface_sel].SWPSCTRL.SWPS_Time_Threshold);
}

RTK_STATUS
SWPSInit(
    u1Byte  interface_sel,
    VOID    *Data
)
{
    //reg file is not gen yet, so use hard code reg value
    
    PHAL_WLAN_ADDR pAddressAdapter;

    pAddressAdapter = &hal_wlan_OB[interface_sel].WlanBaseAddress; 
    
    u2Byte SWPS_RPT_init_size;
    u4Byte init_i =0;
    u1Byte macid_num = 0;
    
    WlanDataEle[interface_sel].SWPSCTRL.SWPS_RPT_macid_num = 2; //tmp test set 2, need to change to use cmd to set

    set_threshold_rutine(pAddressAdapter,0x100,0x40,0x7A12); // 1sec

    macid_num = WlanDataEle[interface_sel].SWPSCTRL.SWPS_RPT_macid_num;
    
    HAL_WLAN_W8(REG_SWPS_CTRL+1, macid_num & 0x7F);

    SWPS_RPT_init_size = (macid_num+1)*16+64+24; //16 is one report size,64 is MACID bitmap, 24 is RXDESC size

    //init report 
    HAL_WLAN_W16(REG_PKTBUF_DBG_CTRL, 0x800);
    for(init_i=0;init_i<(SWPS_RPT_init_size/4);init_i++)
        HAL_WLAN_W32(0x8000+(init_i*4), 0x0);

    //set SWPS_RPT bit in RXDESC
    HAL_WLAN_W8(0x8010, BIT(5));



    //4 just test!!!!  MACID_SWPS_EN=1 at start
    //need to check the macid in txdesc is 1
    HAL_WLAN_W8(REG_SWPS_CTRL, HAL_WLAN_R8(REG_SWPS_CTRL) & ~(BIT2 | BIT3));
    HAL_WLAN_W8(REG_MACID_SWPS_EN, BIT1 | BIT2);

    //init SW variable
    for(init_i=0;init_i<4;init_i++)
        WlanDataEle[interface_sel].SWPSCTRL.SWPS_Seq[init_i] = 1;

    WlanDataEle[interface_sel].SWPSCTRL.SWPS_DropID = 0;
    
    //enable SWPS
    HAL_WLAN_W8(REG_SWPS_CTRL, HAL_WLAN_R8(REG_SWPS_CTRL) | BIT0);
    WlanDataEle[interface_sel].SWPSCTRL.SWPS_EN = 1;

	WlanDataEle[interface_sel].SWPSCTRL.HWTXSC_En = 0;
    WlanDataEle[interface_sel].SWPSCTRL.STW_EN = 0;
    WlanDataEle[interface_sel].SWPSCTRL.TXWIFI_CP = 0;

    return _EXIT_SUCCESS;
}

RTK_STATUS
SWPS_Indirect_Access_Verification(
    VOID    *pAdapter
)
{
    PHAL_WLAN_ADDR pAddressAdapter = (PHAL_WLAN_ADDR) pAdapter;
    u4Byte total_size;
    u1Byte veri_result = _EXIT_SUCCESS;
    //total rxdesc + bitmap + report body (128 macid) 
    //64 is MACID bitmap, 24 is RXDESC size , 1 report size = 16
    total_size = 128*16+64+24;

    //disable SWPS
    HAL_WLAN_W8(REG_SWPS_CTRL, HAL_WLAN_R8(REG_SWPS_CTRL) & ~BIT_SWPS_EN);


    //read/write test
    HAL_WLAN_W32(REG_PKTBUF_DBG_CTRL, 0x800);
    
    u4Byte test_loop;
    for(test_loop=0;test_loop<(total_size/4);test_loop++){
        HAL_WLAN_W32((0x8000+(test_loop*4)), 0x11223344);
        if((HAL_WLAN_R8(0x8000+test_loop*4)!=0x44) || (HAL_WLAN_R8(0x8000+test_loop*4+1)!=0x33) || (HAL_WLAN_R8(0x8000+test_loop*4+2)!=0x22) || (HAL_WLAN_R8(0x8000+test_loop*4+3)!=0x11)){
            
            veri_result = _EXIT_FAILURE; 
            break;
        }
    }

    //init report 
    HAL_WLAN_W16(REG_PKTBUF_DBG_CTRL, 0x800);
    for(test_loop=0;test_loop<(total_size/4);test_loop++)
        HAL_WLAN_W32(0x8000+(test_loop*4), 0x0);

    //set SWPS_RPT bit in RXDESC
    HAL_WLAN_W8(0x8010, BIT(5));


    //enable SWPS
    HAL_WLAN_W8(REG_SWPS_CTRL, HAL_WLAN_R8(REG_SWPS_CTRL) | BIT_SWPS_EN);

    if(veri_result == _EXIT_SUCCESS)
        RTL_CMD_DBG_LOG("Indirected access successed.\n");
    else
        RTL_CMD_DBG_LOG("Indirected access failed.\n");

   return veri_result;
}

RTK_STATUS
SWPS_EN_disable_verification(
    VOID    *pAdapter
)
{
    PHAL_WLAN_ADDR pAddressAdapter = (PHAL_WLAN_ADDR) pAdapter;

    u1Byte veri_result = _EXIT_SUCCESS;
    u1Byte  interface_sel = FindInterface(pAddressAdapter->WLBASE_ADDR);
    u2Byte packet_TH;


    //disable SWPS
    HAL_WLAN_W8(REG_SWPS_CTRL, HAL_WLAN_R8(REG_SWPS_CTRL) & ~BIT0);    
    WlanDataEle[interface_sel].SWPSCTRL.SWPS_EN = 0;

    u1Byte tx_count = 0;
    u32 pkt_num;
    pkt_num = 10; //temp fix to 10
    u1Byte AcID = 0; //temp fix to 0
    TX_LINKLIST_ID Queue = (TX_LINKLIST_ID) AcID;    

    WlanDataEle[interface_sel].SWPSCTRL.SWPS_Fillseq_En = 1;
    WlanDataEle[interface_sel].SWPSCTRL.SWPS_Seq[Queue] = 1;
    WlanDataEle[interface_sel].SWPSCTRL.receive_rpt = 0;

    //backup
    packet_TH = WlanDataEle[interface_sel].SWPSCTRL.SWPS_Packet_Threshold;
    WlanDataEle[interface_sel].SWPSCTRL.SWPS_Packet_Threshold = 5;
    //set gen report threshold
    HAL_WLAN_W16(REG_SWPS_PKT_TH,WlanDataEle[interface_sel].SWPSCTRL.SWPS_Packet_Threshold); 


    sendpacket_rutine(pAddressAdapter,pkt_num,AcID);

    //wait for RX packet, and check no report packet is received
    u2Byte wait_count = 100;
    while(wait_count > 0){
        //HAL_delay_ms(1);
        udelay(1000);
        if(WlanDataEle[interface_sel].SWPSCTRL.receive_rpt > 0)
            break;
        wait_count--;
    }

    if(WlanDataEle[interface_sel].SWPSCTRL.receive_rpt > 0){
        RTL_CMD_DBG_LOG("Error!!! receive SWPS rpt\n");
        veri_result = _EXIT_FAILURE;
    }
    else
        RTL_CMD_DBG_LOG("Success, no report is received.\n");

    //restore trigger conditions
    WlanDataEle[interface_sel].SWPSCTRL.SWPS_Packet_Threshold = packet_TH;        
    //set gen report threshold
    HAL_WLAN_W16(REG_SWPS_PKT_TH,WlanDataEle[interface_sel].SWPSCTRL.SWPS_Packet_Threshold); 

    
    //enable SWPS
    HAL_WLAN_W8(REG_SWPS_CTRL, HAL_WLAN_R8(REG_SWPS_CTRL) | BIT0);
    WlanDataEle[interface_sel].SWPSCTRL.SWPS_EN = 1;

    
    return veri_result;
}

RTK_STATUS
SWPS_polling_rpt_verification(
    VOID    *pAdapter
)
{
    PHAL_WLAN_ADDR pAddressAdapter = (PHAL_WLAN_ADDR) pAdapter;

    u1Byte veri_result = _EXIT_SUCCESS;
    u1Byte  interface_sel = FindInterface(pAddressAdapter->WLBASE_ADDR);

    u32 pkt_num;
    pkt_num = 2; //temp fix to 5
    u1Byte AcID = 0; //temp fix to 0
    TX_LINKLIST_ID Queue = (TX_LINKLIST_ID) AcID;
    u2Byte packet_TH,PsTime_TH,Time_TH;
    
    WlanDataEle[interface_sel].SWPSCTRL.SWPS_Seq[Queue] = 1;

    //backup
    packet_TH = WlanDataEle[interface_sel].SWPSCTRL.SWPS_Packet_Threshold;
    PsTime_TH = WlanDataEle[interface_sel].SWPSCTRL.SWPS_PsTime_Threshold;
    Time_TH = WlanDataEle[interface_sel].SWPSCTRL.SWPS_Time_Threshold;

    
    //disable trigger conditions
    set_threshold_rutine(pAddressAdapter,0x0,0x0,0x0);
    
    //3 case 1 normal polling
    u1Byte loop;
    u1Byte tx_count = 0;

    RTL_CMD_DBG_LOG("Case1 test normal polling\n");
    
    WlanDataEle[interface_sel].SWPSCTRL.SWPS_Fillseq_En = 1;

    for(loop=0;loop<3;loop++){
        WlanDataEle[interface_sel].SWPSCTRL.receive_rpt = 0;
        
        sendpacket_rutine(pAddressAdapter,pkt_num,AcID);
        
        //pollig
        HAL_WLAN_W8(REG_SWPS_CTRL, HAL_WLAN_R8(REG_SWPS_CTRL) | BIT1);
        
        u2Byte wait_count = 100;
        while(wait_count > 0){
            //HAL_delay_ms(1);
            udelay(1000);
            if(WlanDataEle[interface_sel].SWPSCTRL.receive_rpt==1)
                break;
            wait_count--;
        }
        if(wait_count==0){
            RTL_CMD_DBG_LOG("Error!!! NO receive SWPS rpt, loop = %x\n",loop);
            veri_result = _EXIT_FAILURE;
        }
        if(veri_result==_EXIT_FAILURE)
            break;
    }

    if(veri_result==_EXIT_SUCCESS){
        RTL_CMD_DBG_LOG("case 1 normal polling Success.\n");

        //3 case 2 tx packet wtth seq=0, polling to get report with no update 

        RTL_CMD_DBG_LOG("Case2 test tx packet wtth seq=0, polling to get report with no update\n");

        WlanDataEle[interface_sel].SWPSCTRL.SWPS_Fillseq_En = 0;

        WlanDataEle[interface_sel].SWPSCTRL.receive_rpt = 0;
                
        sendpacket_rutine(pAddressAdapter,pkt_num,AcID);
        
        //pollig
        HAL_WLAN_W8(REG_SWPS_CTRL, HAL_WLAN_R8(REG_SWPS_CTRL) | BIT1);

        u2Byte wait_count = 100;
        while(wait_count > 0){
            //HAL_delay_ms(1);
            udelay(1000);
            if(WlanDataEle[interface_sel].SWPSCTRL.receive_rpt==1)
                break;
            wait_count--;
        }
        if(wait_count==0){
            RTL_CMD_DBG_LOG("Error!!! NO receive SWPS rpt\n");
            veri_result = _EXIT_FAILURE;
        }
        
    }
    
    if(veri_result==_EXIT_SUCCESS){
        RTL_CMD_DBG_LOG("Case2 polling to get report with no update Success\n");

        //3 case 3 MACIDX_PS_EN=0, polling to get report  

        RTL_CMD_DBG_LOG("Case3 test tx packet wtth MACIDX_PS_EN=0, polling to get report.\n");

        WlanDataEle[interface_sel].SWPSCTRL.SWPS_Fillseq_En = 1;

        WlanDataEle[interface_sel].SWPSCTRL.receive_rpt = 0;

        //test big seq
        WlanDataEle[interface_sel].SWPSCTRL.SWPS_Seq[Queue] = 4094;

        //4 just test!!!! set MACID1 MACID_SWPS_EN=0 
        //need to check the macid in txdesc is 1
        HAL_WLAN_W8(REG_SWPS_CTRL, HAL_WLAN_R8(REG_SWPS_CTRL) & ~(BIT2 | BIT3));
        HAL_WLAN_W8(REG_MACID_SWPS_EN, HAL_WLAN_R8(0x14FC) & ~BIT1);
                
        sendpacket_rutine(pAddressAdapter,pkt_num,AcID); //seq = 4094~3
        
        //pollig
        HAL_WLAN_W8(REG_SWPS_CTRL, HAL_WLAN_R8(REG_SWPS_CTRL) | BIT1);

        u2Byte wait_count = 50;
        while(wait_count > 0){
            //HAL_delay_ms(1);
            udelay(1000);
            if(WlanDataEle[interface_sel].SWPSCTRL.receive_rpt==1)
                break;
            wait_count--;
        }
        if(wait_count==0){
            RTL_CMD_DBG_LOG("Error!!! NO receive SWPS rpt\n");
            veri_result = _EXIT_FAILURE;
        }else{
            RTL_CMD_DBG_LOG("Case3 MACIDX_PS_EN=0, polling to get report Success.\n");
        }

        //3 restore set MACID1 MACID_SWPS_EN=1
        //need to check the macid in txdesc is 1
        HAL_WLAN_W8(REG_SWPS_CTRL, HAL_WLAN_R8(REG_SWPS_CTRL) & ~(BIT2 | BIT3));
        HAL_WLAN_W8(REG_MACID_SWPS_EN, BIT1);

        
    }

    
    //restore trigger conditions
    set_threshold_rutine(pAddressAdapter,packet_TH,PsTime_TH,Time_TH);

    
    return veri_result;
}

RTK_STATUS
SWPS_trigger_condition_pktnum_verification(
    VOID    *pAdapter
)
{
    PHAL_WLAN_ADDR pAddressAdapter = (PHAL_WLAN_ADDR) pAdapter;

    u1Byte veri_result = _EXIT_SUCCESS;
    u1Byte  interface_sel = FindInterface(pAddressAdapter->WLBASE_ADDR);
    u32 pkt_num;
    pkt_num = 15; //temp fix to 15
    u1Byte AcID = 0; //temp fix to 0

    TX_LINKLIST_ID Queue = (TX_LINKLIST_ID) AcID;
    u2Byte packet_TH,PsTime_TH,Time_TH;

    WlanDataEle[interface_sel].SWPSCTRL.SWPS_Fillseq_En = 1;
    WlanDataEle[interface_sel].SWPSCTRL.SWPS_Seq[Queue] = 1;

    //backup
    packet_TH = WlanDataEle[interface_sel].SWPSCTRL.SWPS_Packet_Threshold;
    PsTime_TH = WlanDataEle[interface_sel].SWPSCTRL.SWPS_PsTime_Threshold;
    Time_TH = WlanDataEle[interface_sel].SWPSCTRL.SWPS_Time_Threshold;

    //temp fix tx packet num to 15, trigger condition packet num = 5
    set_threshold_rutine(pAddressAdapter,0x5,0x0,0x0);

    WlanDataEle[interface_sel].SWPSCTRL.receive_rpt = 0;
    
    sendpacket_rutine(pAddressAdapter,pkt_num,AcID);
    
    u2Byte wait_count = 150;
    u1Byte receive_report_num = (pkt_num/WlanDataEle[interface_sel].SWPSCTRL.SWPS_Packet_Threshold)+((pkt_num % WlanDataEle[interface_sel].SWPSCTRL.SWPS_Packet_Threshold)==0? 0:1);
    
    
    while(wait_count > 0){
        //HAL_delay_ms(1);
        udelay(1000);
        if(WlanDataEle[interface_sel].SWPSCTRL.receive_rpt==receive_report_num)
            break;
        wait_count--;
    }
    if(wait_count==0){
        RTL_CMD_DBG_LOG("Error!!! Did not receive correct SWPS rpt number, have received %x reports, need to receive %x reports\n",WlanDataEle[interface_sel].SWPSCTRL.receive_rpt,receive_report_num);
        veri_result = _EXIT_FAILURE;
    }else{
        RTL_CMD_DBG_LOG("Success.\n");
    }
    
    //restore trigger conditions
    set_threshold_rutine(pAddressAdapter,packet_TH,PsTime_TH,Time_TH);
    
    return veri_result;
}

RTK_STATUS
SWPS_trigger_condition_timer_verification(
    VOID    *pAdapter
)
{
    PHAL_WLAN_ADDR pAddressAdapter = (PHAL_WLAN_ADDR) pAdapter;

    u1Byte veri_result = _EXIT_SUCCESS;
    u1Byte  interface_sel = FindInterface(pAddressAdapter->WLBASE_ADDR);
    u2Byte packet_TH,PsTime_TH,Time_TH;
    u1Byte tx_time_ms = 50;
    u32 pkt_num;
    pkt_num = 3; //temp fix to 15
    u4Byte start_time_record;
    u1Byte AcID = 0; //temp fix to 0

    TX_LINKLIST_ID Queue = (TX_LINKLIST_ID) AcID;
    
    HAL_WLAN_W8(REG_MISC_CTRL, HAL_WLAN_R8(REG_MISC_CTRL) | BIT_EN_FREECNT);//enable free run counter

    WlanDataEle[interface_sel].SWPSCTRL.SWPS_Fillseq_En = 1;
    WlanDataEle[interface_sel].SWPSCTRL.SWPS_Seq[Queue] = 16;

    //backup
    packet_TH = WlanDataEle[interface_sel].SWPSCTRL.SWPS_Packet_Threshold;
    PsTime_TH = WlanDataEle[interface_sel].SWPSCTRL.SWPS_PsTime_Threshold;
    Time_TH = WlanDataEle[interface_sel].SWPSCTRL.SWPS_Time_Threshold;

    //temp fix tx time to 50ms
    set_threshold_rutine(pAddressAdapter,0x0,0x0,tx_time_ms*1000/32);//about 50ms
 
    start_time_record = HAL_WLAN_R32(REG_FREERUN_CNT);
    RTL_CMD_DBG_LOG("start SWPS timer threshold count down, FREERUN_CNT = %x\n",start_time_record);


    WlanDataEle[interface_sel].SWPSCTRL.receive_rpt = 0;
   
    sendpacket_rutine(pAddressAdapter,pkt_num,AcID);

    u2Byte wait_count = 100;
    while(wait_count > 0){
        //HAL_delay_ms(1);
        udelay(1000);
        if(WlanDataEle[interface_sel].SWPSCTRL.receive_rpt==1)
            break;
        wait_count--;
    }
    if(wait_count==0){
        RTL_CMD_DBG_LOG("Error!!! NO receive SWPS rpt\n");
        veri_result = _EXIT_FAILURE;
    }
    else{
        
        RTL_CMD_DBG_LOG("Report gen after %x us\n",WlanDataEle[interface_sel].SWPSCTRL.receice_rpt_time - start_time_record);
    }

    //restore trigger conditions
    set_threshold_rutine(pAddressAdapter,packet_TH,PsTime_TH,Time_TH);
    
    return veri_result;

}

RTK_STATUS
SWPS_trigger_condition_reset_verification(
    VOID    *pAdapter
)
{
    PHAL_WLAN_ADDR pAddressAdapter = (PHAL_WLAN_ADDR) pAdapter;

    u1Byte veri_result = _EXIT_SUCCESS;
    u1Byte veri_result1 = _EXIT_SUCCESS;
    u1Byte veri_result2 = _EXIT_SUCCESS;
    u1Byte  interface_sel = FindInterface(pAddressAdapter->WLBASE_ADDR);
    u2Byte packet_TH,PsTime_TH,Time_TH;
    u1Byte AcID = 0; //temp fix to 0

    TX_LINKLIST_ID Queue = (TX_LINKLIST_ID) AcID;
    u1Byte tx_time_ms = 50;
	u1Byte ps_time_ms = 30;
    u32 pkt_num;
    pkt_num = 4; //temp fix to 4
    u4Byte start_time_record;

    WlanDataEle[interface_sel].SWPSCTRL.SWPS_Fillseq_En = 1;
    WlanDataEle[interface_sel].SWPSCTRL.SWPS_Seq[Queue] = 1;

    //backup
    packet_TH = WlanDataEle[interface_sel].SWPSCTRL.SWPS_Packet_Threshold;
    PsTime_TH = WlanDataEle[interface_sel].SWPSCTRL.SWPS_PsTime_Threshold;
    Time_TH = WlanDataEle[interface_sel].SWPSCTRL.SWPS_Time_Threshold;

    //temp fix tx time to 50ms
    set_threshold_rutine(pAddressAdapter,3,0,tx_time_ms*1000/32);//about 50ms

    start_time_record = HAL_WLAN_R32(REG_FREERUN_CNT);
    RTL_CMD_DBG_LOG("start SWPS timer threshold count down, FREERUN_CNT = %x\n",start_time_record);
    


    //3 case 1 test timer reset
    WlanDataEle[interface_sel].SWPSCTRL.receive_rpt = 0;

    sendpacket_rutine(pAddressAdapter,pkt_num,AcID);
    
    //make sure check the printed receive rpt time, (the first report - start_time_record < 50ms) and (the second report - the first report = about 50ms) //check by eyes for now
    u2Byte wait_count = 300;
    while(wait_count > 0){
        //HAL_delay_ms(1);
        udelay(1000);
        if(WlanDataEle[interface_sel].SWPSCTRL.receive_rpt==2)
            break;
        wait_count--;
    }
    if(wait_count==0){
        RTL_CMD_DBG_LOG("Error!!! received rpt num is not correct, have received %x rpt.\n",WlanDataEle[interface_sel].SWPSCTRL.receive_rpt);
        veri_result1 = _EXIT_FAILURE;
    }else{
        RTL_CMD_DBG_LOG("Case 1 receive correct rpt num, make sure to check receive rpt time.\n");
    }

    //3 case 2 test packet counter reset
    WlanDataEle[interface_sel].SWPSCTRL.receive_rpt = 0;
    tx_time_ms = 80;     //temp fix tx time to 80ms

    set_threshold_rutine(pAddressAdapter,4,0,tx_time_ms*1000/32);//about 80ms

    start_time_record = HAL_WLAN_R32(REG_FREERUN_CNT);
    RTL_CMD_DBG_LOG("start SWPS timer threshold count down, FREERUN_CNT = %x\n",start_time_record);
    
    pkt_num = 2;  //don't know if its enough to tx more than 20ms to gen 2 reports to check if timer is reset
    sendpacket_rutine(pAddressAdapter,pkt_num,AcID);

    while(1){
        //HAL_delay_ms(1);
        udelay(1000);
         if(WlanDataEle[interface_sel].SWPSCTRL.receive_rpt==1)
            break;
    }

    pkt_num = 5;
    sendpacket_rutine(pAddressAdapter,pkt_num,AcID);
    
    //make sure check the printed receive rpt time, ex. (the first report - start_time_record = about 80ms) and (the second report - the first report < 80ms because trigger condition is packet count) //check by eyes for now
    wait_count = 300;
    while(1){
        //HAL_delay_ms(1);
        udelay(1000);
        if(WlanDataEle[interface_sel].SWPSCTRL.receive_rpt==3)
            break;
        wait_count--;
    }
    if(wait_count==0){
        RTL_CMD_DBG_LOG("Error!!! received rpt num is not enough, have received %x rpt. maybe need to adjust tx pkt count or timer\n",WlanDataEle[interface_sel].SWPSCTRL.receive_rpt);
        veri_result2 = _EXIT_FAILURE;
    }else{
        RTL_CMD_DBG_LOG("Case 2 receive enough rpt num, make sure to check receive rpt time, too.\n");
    }

    if((veri_result1==_EXIT_FAILURE) || (veri_result2==_EXIT_FAILURE))
        veri_result = _EXIT_FAILURE;
        

    //restore trigger conditions
    set_threshold_rutine(pAddressAdapter,packet_TH,PsTime_TH,Time_TH);
    
    return veri_result;

}

RTK_STATUS
SWPS_many_report_length_verification(
    VOID    *pAdapter
)
{
    PHAL_WLAN_ADDR pAddressAdapter = (PHAL_WLAN_ADDR) pAdapter;

    u1Byte veri_result = _EXIT_SUCCESS;
    u1Byte veri_result1 = _EXIT_SUCCESS;
    u1Byte veri_result2 = _EXIT_SUCCESS;
    u1Byte  interface_sel = FindInterface(pAddressAdapter->WLBASE_ADDR);


    //3 case 1 length = 32 macid
    WlanDataEle[interface_sel].SWPSCTRL.SWPS_RPT_macid_num = 31;
    HAL_WLAN_W8(REG_SWPS_CTRL+1, WlanDataEle[interface_sel].SWPSCTRL.SWPS_RPT_macid_num & 0x7F);
    WlanDataEle[interface_sel].SWPSCTRL.receive_rpt = 0;
    WlanDataEle[interface_sel].SWPSCTRL.SubTestCase = 0;
    WlanDataEle[interface_sel].SWPSCTRL.TestResult = FALSE;

    
    //pollig
    HAL_WLAN_W8(REG_SWPS_CTRL, HAL_WLAN_R8(REG_SWPS_CTRL) | BIT1);

    //pollig
    //HAL_WLAN_W8(REG_SWPS_CTRL, HAL_WLAN_R8(REG_SWPS_CTRL) | BIT1);

    //need to check printed report length, by eyes for now
    u2Byte wait_count = 50;
    while(wait_count > 0){
        //HAL_delay_ms(1);
        udelay(1000);
        if(WlanDataEle[interface_sel].SWPSCTRL.receive_rpt==1)
            break;
        wait_count--;
    }
    if(wait_count==0){
        RTL_CMD_DBG_LOG("Error!!! NO receive SWPS rpt\n");
        veri_result1 = _EXIT_FAILURE;
    }else{
        if(WlanDataEle[interface_sel].SWPSCTRL.TestResult == TRUE)
        RTL_CMD_DBG_LOG("Case1 receive rpt Success, report length is correct.\n");
    }

    //3 case 2 length = 128 macid
    WlanDataEle[interface_sel].SWPSCTRL.SWPS_RPT_macid_num = 127;
    HAL_WLAN_W8(REG_SWPS_CTRL+1, WlanDataEle[interface_sel].SWPSCTRL.SWPS_RPT_macid_num & 0x7F);
    WlanDataEle[interface_sel].SWPSCTRL.receive_rpt = 0;
    WlanDataEle[interface_sel].SWPSCTRL.SubTestCase = 1;
    WlanDataEle[interface_sel].SWPSCTRL.TestResult = FALSE;
    
    //pollig
    HAL_WLAN_W8(REG_SWPS_CTRL, HAL_WLAN_R8(REG_SWPS_CTRL) | BIT1);
    
    //pollig
    //HAL_WLAN_W8(REG_SWPS_CTRL, HAL_WLAN_R8(REG_SWPS_CTRL) | BIT1);
    

    //need to check printed report length, by eyes for now
    wait_count = 50;
    while(wait_count > 0){
        //HAL_delay_ms(1);
        udelay(1000);
        if(WlanDataEle[interface_sel].SWPSCTRL.receive_rpt==1)
            break;
        wait_count--;
    }
    if(wait_count==0){
        RTL_CMD_DBG_LOG("Error!!! NO receive SWPS rpt\n");
        veri_result2 = _EXIT_FAILURE;
    }
    else{
        if(WlanDataEle[interface_sel].SWPSCTRL.TestResult == TRUE)
        RTL_CMD_DBG_LOG("Case1 receive rpt Success, report length is correct.\n");
    }
    if((veri_result1==_EXIT_FAILURE) || (veri_result2==_EXIT_FAILURE))
        veri_result = _EXIT_FAILURE;

    //restore setting
    WlanDataEle[interface_sel].SWPSCTRL.SWPS_RPT_macid_num = 2;
    HAL_WLAN_W8(REG_SWPS_CTRL+1, WlanDataEle[interface_sel].SWPSCTRL.SWPS_RPT_macid_num & 0x7F);

    return veri_result;
}


RTK_STATUS
SWPS_dropID_verification(
    VOID    *pAdapter
)
{
    PHAL_WLAN_ADDR pAddressAdapter = (PHAL_WLAN_ADDR) pAdapter;

    u1Byte veri_result = _EXIT_SUCCESS;
    u1Byte  interface_sel = FindInterface(pAddressAdapter->WLBASE_ADDR);
    u1Byte AcID = 0; //temp fix to 0
    u2Byte packet_TH,PsTime_TH,Time_TH;

    TX_LINKLIST_ID Queue = (TX_LINKLIST_ID) AcID;
    u32 pkt_num;
    pkt_num = 5; //temp fix to 5

	//enable ctrl info
    HAL_WLAN_W32(REG_MCUFW_CTRL, HAL_WLAN_R32(REG_MCUFW_CTRL)| BIT10);
    WlanDataEle[interface_sel].SWPSCTRL.SWPS_Fillseq_En = 1;
    
    //backup
    packet_TH = WlanDataEle[interface_sel].SWPSCTRL.SWPS_Packet_Threshold;
    PsTime_TH = WlanDataEle[interface_sel].SWPSCTRL.SWPS_PsTime_Threshold;
    Time_TH = WlanDataEle[interface_sel].SWPSCTRL.SWPS_Time_Threshold;

    
    //disable trigger conditions
    set_threshold_rutine(pAddressAdapter,0x0,0x0,0x0);

    
    //3 case1 test only set drop id
    
    WlanDataEle[interface_sel].SWPSCTRL.SWPS_Seq[Queue] = 1;
    //set DropID in Ctrl buf, test MACID1 + DROPID=0 now
    HAL_WLAN_W32(REG_PKTBUF_DBG_CTRL, 0x660);
    HAL_WLAN_W8(0x8000+40+15, BIT4);

    WlanDataEle[interface_sel].SWPSCTRL.receive_rpt = 0;

    //drop id =0
    WlanDataEle[interface_sel].SWPSCTRL.SWPS_DropID = 0;
    sendpacket_rutine(pAddressAdapter,pkt_num,AcID); //seq = 1~5
    //drop id =1
    WlanDataEle[interface_sel].SWPSCTRL.SWPS_DropID = 1;
    sendpacket_rutine(pAddressAdapter,pkt_num,AcID); //seq= 6~10

    //pollig
    HAL_WLAN_W8(REG_SWPS_CTRL, HAL_WLAN_R8(REG_SWPS_CTRL) | BIT1);

    u2Byte wait_count = 50;
    while(wait_count > 0){
        //HAL_delay_ms(1);
        udelay(1000);
        if(WlanDataEle[interface_sel].SWPSCTRL.receive_rpt==1)
            break;
        wait_count--;
    }
    if(wait_count==0){
        RTL_CMD_DBG_LOG("Error!!! NO receive SWPS rpt\n");
        veri_result = _EXIT_FAILURE;
    }else
        RTL_CMD_DBG_LOG("Receive rpt success, make sure to check printed TxDone_SEQ=0xa and DropByID_SEQ=0x5 in rpt\n");


    //3 case2 test set macid drop & set drop id
    if(veri_result == _EXIT_SUCCESS){
        WlanDataEle[interface_sel].SWPSCTRL.receive_rpt = 0;

        //set DropID in Ctrl buf, test MACID1 + DROPID=1 now
        HAL_WLAN_W32(REG_PKTBUF_DBG_CTRL, 0x660);
        HAL_WLAN_W8(0x8000+40+15, BIT5);

        //set macid1 drop
        HAL_WLAN_W32(REG_MACID_DROP0, BIT1);

        //drop id =1
        WlanDataEle[interface_sel].SWPSCTRL.SWPS_DropID = 1;
        sendpacket_rutine(pAddressAdapter,pkt_num,AcID); //seq=11~15, drop by macid drop

        //drop id =2
        WlanDataEle[interface_sel].SWPSCTRL.SWPS_DropID = 2;
        sendpacket_rutine(pAddressAdapter,pkt_num,AcID); //seq= 16~20, drop by macid drop
        
        
        //pollig
        HAL_WLAN_W8(REG_SWPS_CTRL, HAL_WLAN_R8(REG_SWPS_CTRL) | BIT1);

        wait_count = 50;
        while(wait_count > 0){
            //HAL_delay_ms(1);
            udelay(1000);
            if(WlanDataEle[interface_sel].SWPSCTRL.receive_rpt==1)
                break;
            wait_count--;
        }
        if(wait_count==0){
            RTL_CMD_DBG_LOG("Error!!! NO receive SWPS rpt\n");
            veri_result = _EXIT_FAILURE;
        }else
            RTL_CMD_DBG_LOG("Receive rpt success, make sure to check printed TxDone_SEQ=0x14 and DropByID_SEQ=0x5 in rpt.\n");
    }

    //3 case3 test set all the drop id, all the packet should be droped
    if(veri_result == _EXIT_SUCCESS){
        //clear macid1 drop
        HAL_WLAN_W32(REG_MACID_DROP0, 0x0);
        
        WlanDataEle[interface_sel].SWPSCTRL.receive_rpt = 0;

        //set DropID in Ctrl buf, test MACID1 + DROPID=0 1 2 3 now
        HAL_WLAN_W32(REG_PKTBUF_DBG_CTRL, 0x660);
        HAL_WLAN_W8(0x8000+40+15, BIT4 | BIT5 | BIT6 | BIT7 );
        
        //drop id =0
        WlanDataEle[interface_sel].SWPSCTRL.SWPS_DropID = 0;
        sendpacket_rutine(pAddressAdapter,pkt_num,AcID); //seq=21~25, drop by drop id

        //drop id =1
        WlanDataEle[interface_sel].SWPSCTRL.SWPS_DropID = 1;
        sendpacket_rutine(pAddressAdapter,pkt_num,AcID); //seq=26~30, drop by drop id

        //drop id =2
        WlanDataEle[interface_sel].SWPSCTRL.SWPS_DropID = 2;
        sendpacket_rutine(pAddressAdapter,pkt_num,AcID); //seq=31~35, drop by drop id

        //drop id =3
        WlanDataEle[interface_sel].SWPSCTRL.SWPS_DropID = 3;
        sendpacket_rutine(pAddressAdapter,pkt_num,AcID); //seq=36~40, drop by drop id
        //pollig
        HAL_WLAN_W8(REG_SWPS_CTRL, HAL_WLAN_R8(REG_SWPS_CTRL) | BIT1);

        wait_count = 100;
        while(wait_count > 0){
            //HAL_delay_ms(1);
            udelay(1000);
            if(WlanDataEle[interface_sel].SWPSCTRL.receive_rpt==1)
                break;
            wait_count--;
        }
        if(wait_count==0){
            RTL_CMD_DBG_LOG("Error!!! NO receive SWPS rpt\n");
            veri_result = _EXIT_FAILURE;
        }else
            RTL_CMD_DBG_LOG("Receive rpt success, make sure to check printed TxDone_SEQ=0x14 and DropByID_SEQ=0x28 in rpt.\n");

        
    }

    //3 case4 test some seq=0, but drop id is set, packet should be dropped
    if(veri_result == _EXIT_SUCCESS){
        WlanDataEle[interface_sel].SWPSCTRL.receive_rpt = 0;
        //drop id =0
        WlanDataEle[interface_sel].SWPSCTRL.SWPS_DropID = 0;
        //set DropID in Ctrl buf, test MACID1 + DROPID=0 now
        HAL_WLAN_W32(REG_PKTBUF_DBG_CTRL, 0x660);
        HAL_WLAN_W8(0x8000+40+15, BIT4);

        sendpacket_rutine(pAddressAdapter,pkt_num,AcID); //seq = 41~45

        WlanDataEle[interface_sel].SWPSCTRL.SWPS_Fillseq_En = 0;

        sendpacket_rutine(pAddressAdapter,pkt_num,AcID); //seq = 0
        
        //pollig
        HAL_WLAN_W8(REG_SWPS_CTRL, HAL_WLAN_R8(REG_SWPS_CTRL) | BIT1);

        wait_count = 50;
        while(wait_count > 0){
            //HAL_delay_ms(1);
            udelay(1000);
            if(WlanDataEle[interface_sel].SWPSCTRL.receive_rpt==1)
                break;
            wait_count--;
        }
        if(wait_count==0){
            RTL_CMD_DBG_LOG("Error!!! NO receive SWPS rpt\n");
            veri_result = _EXIT_FAILURE;
        }else
            RTL_CMD_DBG_LOG("Receive rpt success, make sure to check printed TxDone_SEQ=0x14 and DropByID_SEQ=0x2D in rpt, which means no update, and make sure no packet is received(all dropped).\n");
        
    }

    //restore setting
    WlanDataEle[interface_sel].SWPSCTRL.SWPS_Fillseq_En = 1;
    //clear macid1 drop
    HAL_WLAN_W32(REG_MACID_DROP0, 0x0);
    //clear macid1 drop id
    HAL_WLAN_W32(REG_PKTBUF_DBG_CTRL, 0x660);
    HAL_WLAN_W8(0x8000+40+15, 0x0);


    //restore trigger conditions
    set_threshold_rutine(pAddressAdapter,packet_TH,PsTime_TH,Time_TH);
    
    return veri_result;

}


RTK_STATUS
SWPS_PS_change_verification(
    VOID    *pAdapter,
    u8 sub_cmdID
)
{
    PHAL_WLAN_ADDR pAddressAdapter = (PHAL_WLAN_ADDR) pAdapter;

    u1Byte veri_result = _EXIT_SUCCESS;
    u1Byte RxInterface_sel;
    u1Byte  interface_sel = FindInterface(pAddressAdapter->WLBASE_ADDR);
    u1Byte AcID = 0; //temp fix to 0
    u2Byte packet_TH,PsTime_TH,Time_TH;
    TX_LINKLIST_ID Queue = (TX_LINKLIST_ID) AcID;
    u32 pkt_num;
    u4Byte Value32;
    u4Byte cr_bak;
    LB_MODE LbMode_bak;
    u1Byte cast_type,cast_type_RX;
    u1Byte macID = 1;
    u1Byte loop_max = 1;
    u2Byte tmp_seq;
    u1Byte MAC_addr[6]={0x0,0x0,0x0,0x0,0x0,0x2};
    u1Byte MAC_addr_dst[6]={0x0,0x0,0x0,0x0,0x0,0x3};
    pkt_num = 5; //temp fix to 5

    
    WlanDataEle[interface_sel].SWPSCTRL.SWPS_Fillseq_En = 1;

    //2 please test with no encryption mode, power bit is set when no encryption for now
    
    //3  AP tx some pkt with SWPS seq
    WlanDataEle[interface_sel].SWPSCTRL.SWPS_Seq[Queue] = 1;

    //backup
    packet_TH = WlanDataEle[interface_sel].SWPSCTRL.SWPS_Packet_Threshold;
    PsTime_TH = WlanDataEle[interface_sel].SWPSCTRL.SWPS_PsTime_Threshold;
    Time_TH = WlanDataEle[interface_sel].SWPSCTRL.SWPS_Time_Threshold;
    
    //disable trigger conditions
    set_threshold_rutine(pAddressAdapter,0x0,0x0,0x0);

    WlanDataEle[interface_sel].SWPSCTRL.receive_rpt = 0;

    //2 also test MBSSID CAM extend here
    //set to unicast, and accept only when A1 match
    cast_type = WlanDataEle[interface_sel].MacSettingPort.Port[Port_1][0].address_info.cast_type;
    WlanDataEle[interface_sel].MacSettingPort.Port[Port_1][0].address_info.cast_type =UNICAST;

    HAL_WLAN_W8(REG_RCR, HAL_WLAN_R8(REG_RCR) & ~BIT_AAP);
    HAL_WLAN_W8(REG_BCN_CTRL,HAL_WLAN_R8(REG_BCN_CTRL) | BIT_EN_BCN_FUNCTION);
    
    if(sub_cmdID==0)
        HAL_WLAN_W8(REG_MACID+5, 0x2);
    else{
        HAL_WLAN_W32(REG_RCR, HAL_WLAN_R32(REG_RCR) | BIT_ENMBID); //to not accept all, and set check bssid cam to accept packet, dont need to write 0x610
        HAL_WLAN_W8(REG_MACID+5, 0x0);
        if(sub_cmdID==1){
            CfgMBSSIDTable_98F(pAddressAdapter,71,0x0);
            CfgMBSSIDTable_98F(pAddressAdapter,40,MAC_addr);
        }
        else{
            CfgMBSSIDTable_98F(pAddressAdapter,40,0x0); //although entry 40 is already been set to valid, but mac address is set 0, so it's ok.
            CfgMBSSIDTable_98F(pAddressAdapter,71,MAC_addr);
        }
    }
        
    //set MACID in MACID1 ctrl info
    HAL_WLAN_W32(REG_PKTBUF_DBG_CTRL, 0x660);
    HAL_WLAN_W8(0x8000+40+21, 0x2); //fix to 2
    
    //init CRC5
    InitMACIDSearch88XX(pAddressAdapter);

    //set CRC5, enable pwr int 
    //unsigned char hwaddr[6] = {0x00,0x0,0x0,0x0,0x0,0x2};     
    SetCRC5ToRPTBuffer88XX(pAddressAdapter,CRC5(MAC_addr,6), macID,1);               

    HAL_WLAN_W32(REG_OFDM_CCK_LEN_MASK+4, HAL_WLAN_R32(REG_OFDM_CCK_LEN_MASK+4) | BIT15 | BIT17); // set ps detect enable

    HAL_WLAN_W32(REG_HIMR3, HAL_WLAN_R32(REG_HIMR3) | BIT_PWR_INT_31TO0_MASK);


    u1Byte loop =0;
    for(loop=0;loop<loop_max;loop++){
        
        pkt_num = 1;
        
        sendpacket_rutine(pAddressAdapter,pkt_num,AcID);

        if(WlanDataEle[interface_sel].LbMode==TwoMac){
            //switch to STA
            RxInterface_sel=interface_sel ^ 0x1;
            pAddressAdapter = &hal_wlan_OB[RxInterface_sel].WlanBaseAddress; 
        }
        else
            RxInterface_sel=interface_sel ;

        //3 send data pkt with pwr = 1, if no work, then try send null pkt (need modify code)
        cast_type_RX = WlanDataEle[RxInterface_sel].MacSettingPort.Port[Port_1][0].address_info.cast_type;
        WlanDataEle[RxInterface_sel].MacSettingPort.Port[Port_1][0].address_info.cast_type =UNICAST;
        WlanDataEle[RxInterface_sel].MacSettingGen.tx_discriptor.powersave_en = TRUE;
        WlanDataEle[RxInterface_sel].MacSettingPort.Port[Port_1][0].address_info.src_addr[5]=0x02;
        pkt_num = 1;
        tmp_seq = WlanDataEle[RxInterface_sel].SWPSCTRL.SWPS_Seq[Queue];
        WlanDataEle[RxInterface_sel].SWPSCTRL.SWPS_Seq[Queue] = 0;
        sendpacket_rutine(pAddressAdapter,pkt_num,AcID);
        
        WlanDataEle[RxInterface_sel].MacSettingGen.tx_discriptor.powersave_en = FALSE;
        WlanDataEle[RxInterface_sel].SWPSCTRL.SWPS_Seq[Queue] = 0;
        //to make sure next pwr bit change is 0->1
        //sendpacket_rutine(pAddressAdapter,pkt_num,AcID);
        
        WlanDataEle[RxInterface_sel].SWPSCTRL.SWPS_Seq[Queue]=tmp_seq;
        WlanDataEle[RxInterface_sel].MacSettingPort.Port[Port_1][0].address_info.cast_type =cast_type_RX;
		if(WlanDataEle[interface_sel].LbMode==TwoMac){
			//switch back to AP
			pAddressAdapter = &hal_wlan_OB[interface_sel].WlanBaseAddress; 
		}
        
    }
    
    u2Byte wait_count = 200;
    while(wait_count > 0){
        //HAL_delay_ms(1);
        udelay(1000);
        if(WlanDataEle[interface_sel].SWPSCTRL.receive_rpt==loop_max)
            break;
        wait_count--;
    }
    if(wait_count==0){
        RTL_CMD_DBG_LOG("Error!!! SWPS rpt number is not correct\n");
        veri_result = _EXIT_FAILURE;
    }else
        RTL_CMD_DBG_LOG("Receive rpt success, make sure to check printed TxDone_SEQ, and PS flag = 1 in rpt.\n");
        

    //restore trigger conditions
    set_threshold_rutine(pAddressAdapter,packet_TH,PsTime_TH,Time_TH);
    WlanDataEle[interface_sel].MacSettingPort.Port[Port_1][0].address_info.cast_type =cast_type;
    
    HAL_WLAN_W32(REG_RCR, HAL_WLAN_R32(REG_RCR) & ~BIT_ENMBID | BIT_AAP);
    
    HAL_WLAN_W32(REG_OFDM_CCK_LEN_MASK+4, HAL_WLAN_R32(REG_OFDM_CCK_LEN_MASK+4) & ~(BIT15 | BIT17)); // set ps detect enable
    HAL_WLAN_W32(REG_HIMR3, HAL_WLAN_R32(REG_HIMR3) & ~BIT_PWR_INT_31TO0_MASK);

   
    return veri_result;
    

}

RTK_STATUS

SWPS_PS_timer_verification(
    VOID    *pAdapter
)
{
    
    PHAL_WLAN_ADDR pAddressAdapter = (PHAL_WLAN_ADDR) pAdapter;

    u1Byte veri_result = _EXIT_SUCCESS;
    u1Byte  interface_sel = FindInterface(pAddressAdapter->WLBASE_ADDR);
    u1Byte  RxInterface_sel;
    u1Byte AcID = 0; //temp fix to 0
    u2Byte packet_TH,PsTime_TH,Time_TH;
    TX_LINKLIST_ID Queue = (TX_LINKLIST_ID) AcID;
    u32 pkt_num;
    u1Byte loop_max = 5;
    u1Byte tx_time_ms = 50;
    u1Byte ps_time_ms = 70;    
    u4Byte start_time_record;
    u4Byte Value32;
    u4Byte cr_bak;
    LB_MODE LbMode_bak;
    u1Byte cast_type,cast_type_RX;
    u1Byte macID;
    u2Byte tmp_seq;
    u2Byte wait_count;
    pkt_num = 5; //temp fix to 5


    
    WlanDataEle[interface_sel].SWPSCTRL.SWPS_Fillseq_En = 1;

    //2 please test with no encryption mode, power bit is set when no encryption for now
       
    //3   AP tx some pkt with SWPS seq
    WlanDataEle[interface_sel].SWPSCTRL.SWPS_Seq[Queue] = 1;

    //backup
    packet_TH = WlanDataEle[interface_sel].SWPSCTRL.SWPS_Packet_Threshold;
    PsTime_TH = WlanDataEle[interface_sel].SWPSCTRL.SWPS_PsTime_Threshold;
    Time_TH = WlanDataEle[interface_sel].SWPSCTRL.SWPS_Time_Threshold;
   
    //set trigger conditions
    set_threshold_rutine(pAddressAdapter,0x0,0x100,0x0); //8ms ps timer

    WlanDataEle[interface_sel].SWPSCTRL.receive_rpt = 0;


    //set to unicast, and accept only when A1 match
    cast_type = WlanDataEle[interface_sel].MacSettingPort.Port[Port_1][0].address_info.cast_type;
    WlanDataEle[interface_sel].MacSettingPort.Port[Port_1][0].address_info.cast_type =UNICAST;
    HAL_WLAN_W8(REG_RCR, HAL_WLAN_R8(REG_RCR) & ~BIT_AAP);
    
    
    //set MACID in MACID1 ctrl info
    HAL_WLAN_W32(REG_PKTBUF_DBG_CTRL, 0x660);
    HAL_WLAN_W8(0x8000+40+21, 0x2); //fix to 2

    //set MACID in MACID2 ctrl info
    HAL_WLAN_W8(0x8000+40+40+21, 0x3); //fix to 3

    //init CRC5
    InitMACIDSearch88XX(pAddressAdapter);

    //set CRC5, enable pwr int 
    unsigned char hwaddr[6] = {0x00,0x0,0x0,0x0,0x0,0x2};     
    macID = 1;
    SetCRC5ToRPTBuffer88XX(pAddressAdapter,CRC5(hwaddr,6), macID,1);               
    hwaddr[5] = 0x3;  
    macID = 2;
    SetCRC5ToRPTBuffer88XX(pAddressAdapter,CRC5(hwaddr,6), macID,1);              

    HAL_WLAN_W32(REG_OFDM_CCK_LEN_MASK+4, HAL_WLAN_R32(REG_OFDM_CCK_LEN_MASK+4) | BIT15 | BIT17); // set ps detect enable

    HAL_WLAN_W32(REG_HIMR3, HAL_WLAN_R32(REG_HIMR3) | BIT_PWR_INT_31TO0_MASK);
    
    HAL_WLAN_W8(REG_MISC_CTRL, HAL_WLAN_R8(REG_MISC_CTRL) | BIT_EN_FREECNT); //enable free run counter


#if 1
    u1Byte loop =0;
    //for(loop=0;loop<loop_max;loop++)
    {
        pkt_num = 3;
        WlanDataEle[interface_sel].MacSettingPort.Port[Port_1][0].address_info.src_addr[5]=0x02;
        WlanDataEle[interface_sel].MacSettingPort.Port[Port_1][0].address_info.des_addr[5]=0x02;

        if(WlanDataEle[interface_sel].LbMode==TwoMac){
            //switch to STA
            RxInterface_sel=interface_sel ^ 0x1;
            pAddressAdapter = &hal_wlan_OB[RxInterface_sel].WlanBaseAddress; 
        }
        else
            RxInterface_sel = interface_sel;
        
        //set macid to 2 to accept pkt
        HAL_WLAN_W8(REG_MACID+5, 0x2);
        
        if(WlanDataEle[interface_sel].LbMode==TwoMac){
            //switch to AP
            pAddressAdapter = &hal_wlan_OB[interface_sel].WlanBaseAddress; 
        }

        sendpacket_rutine(pAddressAdapter,pkt_num,AcID);
		if(WlanDataEle[interface_sel].LbMode==TwoMac){
			//3 STA send pkt with pwr mgnt = 1
			//switch to STA
			pAddressAdapter = &hal_wlan_OB[RxInterface_sel].WlanBaseAddress; 
		}


        //3 send data pkt with pwr = 1, if no work, then try send null pkt (need modify code)
        WlanDataEle[RxInterface_sel].MacSettingGen.tx_discriptor.powersave_en = TRUE;
        
        cast_type_RX = WlanDataEle[RxInterface_sel].MacSettingPort.Port[Port_1][0].address_info.cast_type;
        WlanDataEle[RxInterface_sel].MacSettingPort.Port[Port_1][0].address_info.cast_type =UNICAST;
        pkt_num = 1;
        tmp_seq = WlanDataEle[RxInterface_sel].SWPSCTRL.SWPS_Seq[Queue];
        WlanDataEle[RxInterface_sel].SWPSCTRL.SWPS_Seq[Queue] = 0;
        sendpacket_rutine(pAddressAdapter,pkt_num,AcID);
#if 0        
        WlanDataEle[RxInterface_sel].MacSettingGen.tx_discriptor.powersave_en = FALSE;
        WlanDataEle[RxInterface_sel].SWPSCTRL.SWPS_Seq[Queue] = 0;
        sendpacket_rutine(pAddressAdapter,pkt_num,AcID);
        WlanDataEle[RxInterface_sel].MacSettingGen.tx_discriptor.powersave_en = TRUE;
        WlanDataEle[RxInterface_sel].SWPSCTRL.SWPS_Seq[Queue] = 0;
        sendpacket_rutine(pAddressAdapter,pkt_num,AcID);
#endif        
        WlanDataEle[RxInterface_sel].MacSettingGen.tx_discriptor.powersave_en = FALSE;
        

    }

    if(WlanDataEle[interface_sel].LbMode!=TwoMac){
        wait_count = 300;
        while(wait_count > 0){
            //HAL_delay_ms(1);
            udelay(1000);
            if(WlanDataEle[interface_sel].SWPSCTRL.receive_rpt==1)
                break;
            wait_count--;
        }
    }

    WlanDataEle[RxInterface_sel].MacSettingGen.tx_discriptor.powersave_en = FALSE;
    //to make sure next STA tx pkt will have pwr bit 0->1 
    WlanDataEle[RxInterface_sel].SWPSCTRL.SWPS_Seq[Queue] = 0;
    sendpacket_rutine(pAddressAdapter,pkt_num,AcID);
    //WlanDataEle[RxInterface_sel].SWPSCTRL.SWPS_Seq[Queue]=tmp_seq;
    //WlanDataEle[RxInterface_sel].MacSettingPort.Port[Port_1][0].address_info.cast_type =cast_type_RX;
#endif
#if 1

    if(WlanDataEle[interface_sel].LbMode==TwoMac)
    {
        
        //switch back to AP
        pAddressAdapter = &hal_wlan_OB[interface_sel].WlanBaseAddress; 
        
        pkt_num = 3;
        WlanDataEle[interface_sel].MacSettingPort.Port[Port_1][0].address_info.src_addr[5]=0x03;
        WlanDataEle[interface_sel].MacSettingPort.Port[Port_1][0].address_info.des_addr[5]=0x03;
        WlanDataEle[interface_sel].MacSettingGen.tx_discriptor.macid = 2;
        HAL_WLAN_W8(REG_MACID+5, 0x3);        

		//switch to STA
		pAddressAdapter = &hal_wlan_OB[RxInterface_sel].WlanBaseAddress; 
		
        HAL_WLAN_W8(REG_MACID+5, 0x3);      


        //switch to AP
        pAddressAdapter = &hal_wlan_OB[interface_sel].WlanBaseAddress; 
        
        sendpacket_rutine(pAddressAdapter,pkt_num,AcID);
        

        //3 STA send pkt with pwr mgnt = 1
        //switch to STA
        pAddressAdapter = &hal_wlan_OB[RxInterface_sel].WlanBaseAddress; 


        //3 send data pkt with pwr = 1, if no work, then try send null pkt (need modify code)
        WlanDataEle[RxInterface_sel].MacSettingGen.tx_discriptor.powersave_en = TRUE;
        //cast_type_RX = WlanDataEle[RxInterface_sel].MacSettingPort.Port[Port_1][0].address_info.cast_type;
        //WlanDataEle[RxInterface_sel].MacSettingPort.Port[Port_1][0].address_info.cast_type =UNICAST;
        WlanDataEle[RxInterface_sel].MacSettingPort.Port[Port_1][0].address_info.src_addr[5]=0x03;
        WlanDataEle[RxInterface_sel].MacSettingPort.Port[Port_1][0].address_info.des_addr[5]=0x03;        
        pkt_num = 1;
        //tmp_seq = WlanDataEle[RxInterface_sel].SWPSCTRL.SWPS_Seq[Queue];
        WlanDataEle[RxInterface_sel].SWPSCTRL.SWPS_Seq[Queue] = 0;
        sendpacket_rutine(pAddressAdapter,pkt_num,AcID);
#if 0        
        WlanDataEle[interface_sel].MacSettingGen.tx_discriptor.powersave_en = FALSE;  
        WlanDataEle[interface_sel].SWPSCTRL.SWPS_Seq[Queue] = 0;
        sendpacket_rutine(pAddressAdapter,pkt_num,AcID);
        WlanDataEle[RxInterface_sel].MacSettingGen.tx_discriptor.powersave_en = TRUE;
        WlanDataEle[interface_sel].SWPSCTRL.SWPS_Seq[Queue] = 0;
        sendpacket_rutine(pAddressAdapter,pkt_num,AcID);
#endif        
        
        //WlanDataEle[interface_sel].MacSettingGen.tx_discriptor.powersave_en = FALSE;  
        //WlanDataEle[RxInterface_sel].MacSettingPort.Port[Port_1][0].address_info.cast_type =cast_type_RX;
    
        wait_count = 300;
        while(wait_count > 0){
            //HAL_delay_ms(1);
            udelay(1000);
            if(WlanDataEle[interface_sel].SWPSCTRL.receive_rpt>=1)
                break;
            wait_count--;
        }
        WlanDataEle[RxInterface_sel].MacSettingGen.tx_discriptor.powersave_en = FALSE;
        //to make sure next STA tx pkt will have pwr bit 0->1 
        WlanDataEle[RxInterface_sel].SWPSCTRL.SWPS_Seq[Queue] = 0;
        sendpacket_rutine(pAddressAdapter,pkt_num,AcID);
        //WlanDataEle[RxInterface_sel].SWPSCTRL.SWPS_Seq[Queue]=tmp_seq;
        //switch to AP
        pAddressAdapter = &hal_wlan_OB[interface_sel].WlanBaseAddress; 

    }

    
#endif    
    if(wait_count==0){
        RTL_CMD_DBG_LOG("Error!!! NO receive SWPS rpt\n");
        veri_result = _EXIT_FAILURE;
    }else
        RTL_CMD_DBG_LOG("Receive rpt success, make sure to check printed TxDone_SEQ=0x3,and macid1 PS flag = 1 in rpt.\n");
        

#if 0 //open when need to test
     //3  case 3 test ps timer reset
     
     WlanDataEle[interface_sel].SWPSCTRL.receive_rpt = 0;
       
     set_threshold_rutine(pAddressAdapter,3,ps_time_ms*1000/32,0x0);

     pkt_num = 1;
     sendpacket_rutine(pAddressAdapter,pkt_num,AcID);


     start_time_record = HAL_WLAN_R32(REG_FREERUN_CNT);     
     RTL_CMD_DBG_LOG("start SWPS timer threshold count down, FREERUN_CNT = %x\n",start_time_record);

     if(WlanDataEle[interface_sel].LbMode==TwoMac){         
          //3 STA send pkt with pwr mgnt = 1
          //switch to STA
          pAddressAdapter = &hal_wlan_OB[RxInterface_sel].WlanBaseAddress; 
     }  
     
     //3 send data pkt with pwr = 1, if no work, then try send null pkt (need modify code)
     WlanDataEle[RxInterface_sel].MacSettingGen.tx_discriptor.powersave_en = TRUE;     
     //cast_type_RX = WlanDataEle[RxInterface_sel].MacSettingPort.Port[Port_1][0].address_info.cast_type;
     //WlanDataEle[RxInterface_sel].MacSettingPort.Port[Port_1][0].address_info.cast_type =UNICAST;
     pkt_num = 1;
     //tmp_seq = WlanDataEle[RxInterface_sel].SWPSCTRL.SWPS_Seq[Queue];
     WlanDataEle[RxInterface_sel].SWPSCTRL.SWPS_Seq[Queue] = 0;
     sendpacket_rutine(pAddressAdapter,pkt_num,AcID);
     //WlanDataEle[RxInterface_sel].SWPSCTRL.SWPS_Seq[Queue]=tmp_seq;
     //WlanDataEle[RxInterface_sel].MacSettingGen.tx_discriptor.powersave_en = FALSE;
     //WlanDataEle[RxInterface_sel].MacSettingPort.Port[Port_1][0].address_info.cast_type =cast_type_RX;
	if(WlanDataEle[interface_sel].LbMode==TwoMac){
		 //switch back to AP
		 pAddressAdapter = &hal_wlan_OB[interface_sel].WlanBaseAddress; 
	}
     
     pkt_num = 2;  
     sendpacket_rutine(pAddressAdapter,pkt_num,AcID);

     //will receive a report due to pkt count threshold
     wait_count = 300;
     while(wait_count > 0){
         //HAL_delay_ms(1);
         udelay(1000);
         if(WlanDataEle[interface_sel].SWPSCTRL.receive_rpt==1)
             break;
         //wait_count--;
     }
     
     start_time_record = HAL_WLAN_R32(REG_FREERUN_CNT);     
     RTL_CMD_DBG_LOG("start SWPS timer threshold count down, FREERUN_CNT = %x\n",start_time_record);
     if(WlanDataEle[interface_sel].LbMode==TwoMac){         
           //3 STA send pkt with pwr mgnt = 1
           //switch to STA
           pAddressAdapter = &hal_wlan_OB[RxInterface_sel].WlanBaseAddress; 
      }  

     //WlanDataEle[RxInterface_sel].MacSettingGen.tx_discriptor.powersave_en = TRUE;     
     //cast_type_RX = WlanDataEle[RxInterface_sel].MacSettingPort.Port[Port_1][0].address_info.cast_type;
     //WlanDataEle[RxInterface_sel].MacSettingPort.Port[Port_1][0].address_info.cast_type =UNICAST;
     pkt_num = 1;
     //tmp_seq = WlanDataEle[RxInterface_sel].SWPSCTRL.SWPS_Seq[Queue];
     WlanDataEle[RxInterface_sel].SWPSCTRL.SWPS_Seq[Queue] = 0;
     sendpacket_rutine(pAddressAdapter,pkt_num,AcID);
     WlanDataEle[RxInterface_sel].SWPSCTRL.SWPS_Seq[Queue]=tmp_seq;
     WlanDataEle[RxInterface_sel].MacSettingGen.tx_discriptor.powersave_en = FALSE;
     
     if(WlanDataEle[interface_sel].LbMode==TwoMac){
		 //switch back to AP
		 pAddressAdapter = &hal_wlan_OB[interface_sel].WlanBaseAddress; 
	}
     //make sure check the printed receive rpt time, ex. (the first report - start_time_record < 70ms because trigger condition is packet count) and (the second report - the first report about 70ms ) //check by eyes for now
     wait_count = 500;
     while(wait_count > 0){
         //HAL_delay_ms(1);
         udelay(1000);
         if(WlanDataEle[interface_sel].SWPSCTRL.receive_rpt==2)
             break;
         //wait_count--;
     }
     if(wait_count==0){
         RTL_CMD_DBG_LOG("Error!!! received rpt num is not enough, have received %x rpt. maybe need to adjust tx pkt count or timer\n",WlanDataEle[interface_sel].SWPSCTRL.receive_rpt);
         veri_result = _EXIT_FAILURE;
     }else{
         RTL_CMD_DBG_LOG("Receive enough rpt num, make sure to check receive rpt time, too.\n");
     }
#endif        

    //restore trigger conditions
    set_threshold_rutine(pAddressAdapter,packet_TH,PsTime_TH,Time_TH);
    WlanDataEle[interface_sel].MacSettingPort.Port[Port_1][0].address_info.cast_type =cast_type;
    WlanDataEle[RxInterface_sel].MacSettingPort.Port[Port_1][0].address_info.cast_type =cast_type_RX;
    WlanDataEle[interface_sel].MacSettingGen.tx_discriptor.macid = 1;
    HAL_WLAN_W8(REG_MACID+5, 0x2);
    HAL_WLAN_W8(REG_RCR, HAL_WLAN_R8(REG_RCR) | BIT_AAP);
    
    HAL_WLAN_W32(REG_OFDM_CCK_LEN_MASK+4, HAL_WLAN_R32(REG_OFDM_CCK_LEN_MASK+4) & ~(BIT15 | BIT17)); // set ps detect enable
    HAL_WLAN_W32(REG_HIMR3, HAL_WLAN_R32(REG_HIMR3) & ~BIT_PWR_INT_31TO0_MASK);


    return veri_result;
}

RTK_STATUS
SWPS_TXHWSC_verification(
    VOID    *pAdapter
)
{
    PHAL_WLAN_ADDR pAddressAdapter = (PHAL_WLAN_ADDR) pAdapter;

    u1Byte veri_result = _EXIT_SUCCESS;
    u1Byte  interface_sel = FindInterface(pAddressAdapter->WLBASE_ADDR);
    u1Byte AcID = 0; //temp fix to 0
    u2Byte packet_TH,PsTime_TH,Time_TH;
    TX_LINKLIST_ID Queue = (TX_LINKLIST_ID) AcID;
    u32 pkt_num;
    u4Byte HISR3_status;
    u1Byte loop_max = 5;
    
    
    WlanDataEle[interface_sel].SWPSCTRL.SWPS_Fillseq_En = 1;

    WlanDataEle[interface_sel].SWPSCTRL.SWPS_Seq[Queue] = 1;

    //backup
    packet_TH = WlanDataEle[interface_sel].SWPSCTRL.SWPS_Packet_Threshold;
    PsTime_TH = WlanDataEle[interface_sel].SWPSCTRL.SWPS_PsTime_Threshold;
    Time_TH = WlanDataEle[interface_sel].SWPSCTRL.SWPS_Time_Threshold;

    
    //disable trigger conditions
    set_threshold_rutine(pAddressAdapter,0x0,0x0,0x0);


    HAL_WLAN_W32(REG_CR, HAL_WLAN_R32(REG_CR) | BIT_SHCUT_EN);
    HAL_WLAN_W32(REG_SHCUT_SETTING, HAL_WLAN_R32(REG_SHCUT_SETTING) & ~BIT8); 


    WlanDataEle[interface_sel].SWPSCTRL.receive_rpt = 0;
    
    WlanDataEle[interface_sel].SWPSCTRL.HWTXSC_En = 1;

    pkt_num = 1;
    WlanDataEle[interface_sel].SWPSCTRL.TXWIFI_CP = 1;
    sendpacket_rutine(pAddressAdapter,pkt_num,AcID);
    WlanDataEle[interface_sel].SWPSCTRL.TXWIFI_CP = 0;

    //printf("REG_HISR3=0x%x\n",HAL_WLAN_R32(REG_HISR3));
    HAL_WLAN_W32(REG_HISR3, HAL_WLAN_R32(REG_HISR3) & BIT_TXSHORTCUT_TXDESUPDATEOK);

    pkt_num = 1;
    WlanDataEle[interface_sel].SWPSCTRL.STW_EN = 1;
    sendpacket_rutine(pAddressAdapter,pkt_num,AcID);
    WlanDataEle[interface_sel].SWPSCTRL.STW_EN = 0;


    WlanDataEle[interface_sel].SWPSCTRL.HWTXSC_En = 0;

    HISR3_status=HAL_WLAN_R32(REG_HISR3);
    if(HISR3_status & BIT13)
        RTL_CMD_DBG_LOG("HW tx shortcut has updated tx desc\n");
    else
        RTL_CMD_DBG_LOG("HW tx shortcut has not updated tx desc\n");

    HAL_WLAN_W32(REG_HISR3, BIT13);
    
    //pollig
    HAL_WLAN_W8(REG_SWPS_CTRL, HAL_WLAN_R8(REG_SWPS_CTRL) | BIT1);
    
    u2Byte wait_count = 50;
    while(wait_count > 0){
        //HAL_delay_ms(1);
        udelay(1000);
        if(WlanDataEle[interface_sel].SWPSCTRL.receive_rpt==1)
            break;
        wait_count--;
    }
    if(wait_count==0){
        RTL_CMD_DBG_LOG("Error!!! NO receive SWPS rpt\n");
        veri_result = _EXIT_FAILURE;
    }
    else
        RTL_CMD_DBG_LOG("Receive SWPS success, need to check printed seq number\n");
    //restore trigger conditions
    set_threshold_rutine(pAddressAdapter,packet_TH,PsTime_TH,Time_TH);
    HAL_WLAN_W32(REG_CR, HAL_WLAN_R32(REG_CR) & ~BIT_SHCUT_EN);
    HAL_WLAN_W32(REG_SHCUT_SETTING, HAL_WLAN_R32(REG_SHCUT_SETTING) | BIT8); 


    return veri_result;
}

RTK_STATUS
SWPS_retrytodrop_verification(
    VOID    *pAdapter
)
{
    PHAL_WLAN_ADDR pAddressAdapter = (PHAL_WLAN_ADDR) pAdapter;

    u1Byte veri_result = _EXIT_SUCCESS;
    u1Byte  interface_sel = FindInterface(pAddressAdapter->WLBASE_ADDR);
    u1Byte  RxInterface_sel;
    u1Byte AcID = 0; //temp fix to 0
    u2Byte packet_TH,PsTime_TH,Time_TH;
    TX_LINKLIST_ID Queue = (TX_LINKLIST_ID) AcID;
    u32 pkt_num;
    u4Byte Value32;
    u4Byte cr_bak;
    LB_MODE LbMode_bak;
    u1Byte cast_type = WlanDataEle[interface_sel].MacSettingPort.Port[Port_1][0].address_info.cast_type;
    u4Byte   Chose  = 0;
        
    WlanDataEle[interface_sel].MacSettingPort.Port[Port_1][0].address_info.cast_type =UNICAST;

 
    WlanDataEle[interface_sel].SWPSCTRL.SWPS_Fillseq_En = 1;
    
    WlanDataEle[interface_sel].SWPSCTRL.SWPS_Seq[Queue] = 1;

    //backup
    packet_TH = WlanDataEle[interface_sel].SWPSCTRL.SWPS_Packet_Threshold;
    PsTime_TH = WlanDataEle[interface_sel].SWPSCTRL.SWPS_PsTime_Threshold;
    Time_TH = WlanDataEle[interface_sel].SWPSCTRL.SWPS_Time_Threshold;

    
    //disable trigger conditions
    set_threshold_rutine(pAddressAdapter,0x0,0x0,0x0);
#if CONFIG_TWOMAC
	if(WlanDataEle[interface_sel].LbMode==TwoMac){
		//switch to STA
        RxInterface_sel=interface_sel ^ 0x1;
        pAddressAdapter = &hal_wlan_OB[RxInterface_sel].WlanBaseAddress; 
	} 
    else
        RxInterface_sel=interface_sel;
    
    HAL_WLAN_W32(REG_RCR, HAL_WLAN_R32(REG_RCR) & ~BIT_AAP); //to not accept all
    //HAL_WLAN_W32(REG_RCR, HAL_WLAN_R32(REG_RCR) & ~0xf); 


	if(WlanDataEle[interface_sel].LbMode==TwoMac){
		//switch back to AP
		pAddressAdapter = &hal_wlan_OB[interface_sel].WlanBaseAddress; 
	}
#else
//set to normal mode
    HAL_WLAN_W32(REG_CR, HAL_WLAN_R32(REG_CR) & 0x00ffffff);


#endif    
    WlanDataEle[interface_sel].SWPSCTRL.receive_rpt = 0;
    pkt_num = 1;

    WlanDataEle[interface_sel].MacSettingPort.Port[Port_1][0].address_info.des_addr[5]=0x05;
    sendpacket_rutine(pAddressAdapter,pkt_num,AcID);

    WlanDataEle[interface_sel].MacSettingPort.Port[Port_1][0].address_info.des_addr[5]=0x02;
    
    //wait for report update
    mdelay(10);
    //pollig
    HAL_WLAN_W8(REG_SWPS_CTRL, HAL_WLAN_R8(REG_SWPS_CTRL) | BIT1);
    
    u2Byte wait_count = 100;
    while(wait_count > 0){
        //HAL_delay_ms(1);
        udelay(1000);
        if(WlanDataEle[interface_sel].SWPSCTRL.receive_rpt==1)
            break;
        wait_count--;
    }
    if(wait_count==0){
        RTL_CMD_DBG_LOG("Error!!! NO receive SWPS rpt\n");
        veri_result = _EXIT_FAILURE;
    }
    else
        RTL_CMD_DBG_LOG("Receive SWPS success, need to check printed seq number\n");

    //2 need to check the last txrpt here, the pkt should be retry to drop
    
    //restore trigger conditions
    set_threshold_rutine(pAddressAdapter,packet_TH,PsTime_TH,Time_TH);
    HAL_WLAN_W32(REG_RCR, HAL_WLAN_R32(REG_RCR) | BIT_AAP); 
    //HAL_WLAN_W32(REG_RCR, HAL_WLAN_R32(REG_RCR) | 0xf); 
    WlanDataEle[interface_sel].MacSettingPort.Port[Port_1][0].address_info.cast_type =cast_type;
    
#if (!CONFIG_TWOMAC)
//set to origin mode
    Value32 = HAL_WLAN_R32(REG_CR);

    switch(WlanDataEle[interface_sel].LbMode)
    {
        case MAC_LB:
            Chose = BIT_LBMODE(0x0b); 
        break;
        case MAC_Delay_LB:
            Chose = BIT_LBMODE(LOOPBACK_MAC_DELAY);
        break;
        case LB_Disable:
            Chose = BIT_LBMODE(LOOPBACK_NORMAL);  
        break;
        default:
            Chose = BIT_LBMODE(LOOPBACK_NORMAL);
        break;
    }
            
            Value32 = (Value32 & 0x00ffffff) | Chose;
            HAL_WLAN_W32(REG_CR, Value32);

#endif

    return veri_result;
}


RTK_STATUS
SWPS_Verification(
    VOID    *pAdapter,
    u8 CmdID,
    u8 sub_cmdID
)
{
    PHAL_WLAN_ADDR pAddressAdapter= (PHAL_WLAN_ADDR) pAdapter;
    u1Byte interface_sel = FindInterface(pAddressAdapter->WLBASE_ADDR);
    u1Byte veri_result = _EXIT_FAILURE;

    WlanDataEle[interface_sel].SWPSCTRL.TestCase = CmdID;
        
    switch(CmdID)
    {
        case Indirect_Access: // 0
            veri_result = SWPS_Indirect_Access_Verification(pAddressAdapter); //need to be tested first
        break;
        case SWPS_EN_disable: // 1
            veri_result = SWPS_EN_disable_verification(pAddressAdapter); //disable the whole SWPS HW function, check no report received
        break;
        case SWPS_polling: // 2
            veri_result = SWPS_polling_rpt_verification(pAddressAdapter); //other trigger confitions are disabled. only polling.
        break;
        case SWPS_trigger_packetnum: // 3
            veri_result = SWPS_trigger_condition_pktnum_verification(pAddressAdapter); //only trigger by packet num, other disable
        break;
        case SWPS_trigger_timer: // 4
            veri_result = SWPS_trigger_condition_timer_verification(pAddressAdapter);
        break;
        case SWPS_trigger_condition_reset: // 5
            veri_result = SWPS_trigger_condition_reset_verification(pAddressAdapter);
        break;
        case SWPS_many_report_length: // 6
            veri_result = SWPS_many_report_length_verification(pAddressAdapter);
        break;
        case SWPS_dropID: // 7
            veri_result = SWPS_dropID_verification(pAddressAdapter);
        break;
		case SWPS_PS_change: // 8
            veri_result = SWPS_PS_change_verification(pAddressAdapter,sub_cmdID); //if subID=0, no MBID CAM, if subID=1, with MBID CAM
        break;
        case SWPS_PS_timer: // 9
            veri_result = SWPS_PS_timer_verification(pAddressAdapter);
        break;
        case SWPS_TXHWSC: // 10
            veri_result = SWPS_TXHWSC_verification(pAddressAdapter);
        break;
        case SWPS_retrytodrop: // 11
            veri_result = SWPS_retrytodrop_verification(pAddressAdapter);
        break;
    }

    return veri_result;
}
//-------------------------SWPS Verification--------------------------------

