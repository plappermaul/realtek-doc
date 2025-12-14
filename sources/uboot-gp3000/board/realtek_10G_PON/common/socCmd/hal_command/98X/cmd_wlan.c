
#include <common.h>
#include <command.h>
#include <basic_types.h>
#include "peripheral.h"
#include "cmd_wlan.h"
//#include "rtk_hal_wlan.h"
//#include "wl_log.h"
//#include "wl_common_ctrl.h"

#include "cmd_common.h"



#define REG32(reg)      (*(volatile u32 *)(reg))

HAL_WLAN_OB hal_wlan_OB[2];

extern WLAN_DATA_ELEMENT WlanDataEle[2];
extern unsigned int trx_lock;
extern u4Byte CAMData_Security[CAM_DATA_SIZE];

extern u4Byte CAMData_Wapi[CAM_DATA_SIZE];

extern u4Byte CAMData_Aes256[CAM_DATA_SIZE];

extern u4Byte CAMData_Gcmp128[CAM_DATA_SIZE];

extern u4Byte CAMData_Gcmp256_Gcmsms4[CAM_DATA_SIZE];

u32 
CmdFWDL(
    IN  u16  argc,
    IN  u8* argv[]
)
{
    PHAL_WLAN_ADDR pAddressAdapter;
    u8             interface_sel;
    interface_sel = Strtoul((const u8*)(argv[0]), (u8 **)NULL, 16);
    pAddressAdapter = &hal_wlan_OB[interface_sel].WlanBaseAddress;

    return InitMIPSFirmware88XX(pAddressAdapter);
}
//#endiif 


u32 
CmdFWVERI(
    IN  u16  argc,
    IN  u8* argv[]
)
{
    u8                 Mode;
    u16                subID;
    u8                  interface_sel;
    PHAL_WLAN_ADDR pAddressAdapter;

    subID = 0;
    if (argc < 1) {
        RTL_CMD_DBG_LOG("Input parameters error!!!");
        return RT_STATUS_FAILURE;
    }
    interface_sel = Strtoul((const u8*)(argv[0]), (u8 **)NULL, 16);
    Mode = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 16);
    
    if(argc > 2)
        subID = Strtoul((const u8*)(argv[2]), (u8 **)NULL, 16);

    pAddressAdapter = &hal_wlan_OB[interface_sel].WlanBaseAddress;
    
    return FirmwareVerification(pAddressAdapter,Mode,subID);
    //return 0;
}
//#endif 

u32 
CmdWLOprater(
    IN  u16  argc,
    IN  u8* argv[]
)
{
    u32                 Mode, interface_num;
    
    if (argc!=2) {
        RTL_CMD_DBG_LOG("Input parameters error!!!");
        return 0;
    }
    interface_num = Strtoul((const u8*)(argv[0]), (u8 **)NULL, 16);
    Mode = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 16);

    if (!WlCtrlOperater(Mode,interface_num))
        RTL_CMD_DBG_LOG("Error!!!!!\n");
    else        
        RTL_CMD_DBG_LOG("Success!!!!!\n");

    return _TRUE;
}

u32 
CmdSendTest(
    IN  u16   argc, 
    IN  u8* argv[]
)
{
    TX_LINKLIST_ID Queue;
    TX_LINK_LIST_ELE *LLEle; 
    u32 LoopMode;
    u32 pkt_num,pkt_size,i;
    pkt_num = 1;

    static u32 AcIDInput;
    static u32 loopbackCount;    
    u8 interface_sel;
    PHAL_WLAN_ADDR pAddressAdapter;

    loopbackCount++;
    
    RTL_ALL_DBG_LOG("\n\nLoop back count = %x !\n",loopbackCount);

    AcIDInput++;

    if(AcIDInput > 4)
    {
        AcIDInput = 0;
    }

    Queue = (TX_LINKLIST_ID) AcIDInput;

    interface_sel = Strtoul((const u8*)(argv[0]), (u8 **)NULL, 16);
    pAddressAdapter = &hal_wlan_OB[interface_sel].WlanBaseAddress;
    
#ifdef CONFIG_WHOLE_CODE_IN_SRAM
        LLEle = (TX_LINK_LIST_ELE*)(((u32)&WlanDataEle[interface_sel].StackLLEle));
#else
        LLEle = (TX_LINK_LIST_ELE*)(((u32)&WlanDataEle[interface_sel].StackLLEle)|0xa0000000);
#endif 



#if 0
    LoopMode = 1;

    if (!argc) {
        RTL_CMD_DBG_LOG("NO AC index Input!!!!");
        return 0;
    }

    AcIDInput = Strtoul((const u8*)(argv[0]), (u8 **)NULL, 16);
    if (argc > 1)
    {
        pkt_size = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 10);
    }
    
    Queue = (TX_LINKLIST_ID) AcIDInput;

#ifdef CONFIG_SECURITY_VERIFY                  
    LLEle = (TX_LINK_LIST_ELE*)(((u32)&WlanDataEle.StackLLEle));
#else
    LLEle = (TX_LINK_LIST_ELE*)(((u32)&WlanDataEle.StackLLEle)|0xa0000000);
#endif 
#endif

    //WlanDataEle.gTxLen = pkt_size;
    for(i=0;i<13;i++)
    {
       //WlanDataEle.MacSettingGen.source_data.packet[i].lengthopt = RANDOM; //RANDOM; //FIXED;//INCREASING; 
       //WlanDataEle.MacSettingGen.source_data.packet[i].payloadopt = RANDOM; //RANDOM; //FIXED; //INCREASING;    
    }

#if CONFIG_TX_LOOPBACK_ALWAYSLOOP
    if (!WlanDataEle[interface_sel].WlRxResult) {
       RTL_CMD_DBG_LOG("Loop Num: %d; Rx error!!!!!!! \n", loopbackCount);
       return _FALSE;
    }
#endif 

    LLEle->PortId = 0;
    if((Queue > TX_LINKLIST_MAG) && (Queue <TX_LINKLIST_MAX))
       LLEle->PortDestNum = Queue-5;
    else
       LLEle->PortDestNum = 0;

    u1Byte tx_count = 0;
    for(tx_count=0;tx_count<pkt_num ;tx_count++){
        if(AcIDInput < 13)
            SendPacket(pAddressAdapter,LoopMode, Queue, (TX_LINK_LIST_ELE*)LLEle);
        else
            RTL_CMD_DBG_LOG("No Support AC Type!!!!!!!\n");
    }
    return _TRUE;    
}

u32
CmdSetPktOpt(
    IN  u16   argc, 
    IN  u8* argv[]

)
{
    u8 securetype;
    u8 encrypt_HWSW;
    u8 TxExtendEn;
    u8 lengthopt;
	u8 payloadopt;
    u8 TxdescInSRAM;
    u8 SegThresh;
    u32 Value32;
    u8 interface_sel;
    u4Byte   Chose  = 0;
    u32 i;
    PHAL_WLAN_ADDR pAddressAdapter;
    
    if (argc!=8) {
        RTL_CMD_DBG_LOG("Input parameters error!!!\n");
        return 0;
    }
    interface_sel = Strtoul((const u8*)(argv[0]), (u8 **)NULL, 16);
    securetype = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 16); //0:NONE, 1:WEP40, 2:TKIP, 3:TKIP_NOIMPLEMENT, 4:AES, 5:WEP104, 6:WAPI, 7:AES256, 8:GCMP128, 9:GCMP256, 10:GCMSMS4
    encrypt_HWSW = Strtoul((const u8*)(argv[2]), (u8 **)NULL, 16); //HW 1, SW 0
    TxExtendEn = Strtoul((const u8*)(argv[3]), (u8 **)NULL, 16);
    TxdescInSRAM = Strtoul((const u8*)(argv[4]), (u8 **)NULL, 16);
    lengthopt = Strtoul((const u8*)(argv[5]), (u8 **)NULL, 16); //FIXED 0, RANDOM 1, INCREASING 2
	payloadopt = Strtoul((const u8*)(argv[6]), (u8 **)NULL, 16); //FIXED 0, RANDOM 1, INCREASING 2    
	SegThresh = Strtoul((const u8*)(argv[7]), (u8 **)NULL, 16); 

    printf("securetype=%x, encrypt_HWSW=%x, TxExtendEn=%x,lengthopt=%x, payloadopt=%x\n",securetype,encrypt_HWSW,TxExtendEn,lengthopt,payloadopt);

    WlanDataEle[interface_sel].MacSettingPort.Port[Port_1][0].encryption_info.encryption = encrypt_HWSW;
    WlanDataEle[interface_sel].MacSettingPort.Port[Port_1][0].encryption_info.decryption = HW;
    WlanDataEle[interface_sel].MacSettingPort.Port[Port_1][0].encryption_info.type = securetype;

    pAddressAdapter = &hal_wlan_OB[interface_sel].WlanBaseAddress;

    
#if 1
    Value32 = HAL_WLAN_R32(REG_CR);
            
    Value32 &= ~(BIT_MASK_LBMODE << BIT_SHIFT_LBMODE);

    if(WlanDataEle[interface_sel].MacSettingPort.Port[Port_1][0].encryption_info.type !=TYPE_NONE)
    {
        // If security enable, sey MAC delay LB
        WlanDataEle[interface_sel].LbMode = MAC_Delay_LB;
    }
    
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
    
    Value32 |= Chose;
    HAL_WLAN_W32(REG_CR, Value32);

    if( WlanDataEle[interface_sel].LbMode == MAC_Delay_LB) {
        //Value8 = TX_PAGE_BOUNDARY_LOOPBACK_MODE;
        //HAL_WLAN_W16(REG_WMAC_LBK_BUF_HD_V1, Value8);
        HAL_WLAN_W16(REG_WMAC_LBK_BUF_HD_V1, 0x80);
    }
    
#endif

    WlanDataEle[interface_sel].gEnableExTXBD = TxExtendEn;
    WlanDataEle[interface_sel].EnTxdescInSRAM = TxdescInSRAM;

    switch (SegThresh) {
        case 0:
            WlanDataEle[interface_sel].WlTxSegThreshold = 4096;
            WlanDataEle[interface_sel].WlExTxSegThreshold = 4096;
            break;
        case 1:
            WlanDataEle[interface_sel].WlTxSegThreshold = 1024;
            WlanDataEle[interface_sel].WlExTxSegThreshold = 1024;
            break;
        case 2:
            WlanDataEle[interface_sel].WlTxSegThreshold = 700;
            WlanDataEle[interface_sel].WlExTxSegThreshold = 700;
            break;
        case 3:
            WlanDataEle[interface_sel].WlTxSegThreshold = 512;
            WlanDataEle[interface_sel].WlExTxSegThreshold = 512;
            break;
        case 4:
            WlanDataEle[interface_sel].WlTxSegThreshold = 256;
            WlanDataEle[interface_sel].WlExTxSegThreshold = 256;
            break;
        case 5:
            WlanDataEle[interface_sel].WlTxSegThreshold = 160;
            WlanDataEle[interface_sel].WlExTxSegThreshold = 160;
            break;
        case 6:
            WlanDataEle[interface_sel].WlTxSegThreshold = 100;
            WlanDataEle[interface_sel].WlExTxSegThreshold = 100;
            break;
        case 7:
            WlanDataEle[interface_sel].WlTxSegThreshold = 40;
            WlanDataEle[interface_sel].WlExTxSegThreshold = 40;
            break;
        case 8:
            WlanDataEle[interface_sel].WlTxSegThreshold = 32;            
            WlanDataEle[interface_sel].WlExTxSegThreshold = 32;
            break;

        default:
            break;
    }
    

#if 1
       
       
    for(i=0;i<13;i++)
    {
       WlanDataEle[interface_sel].MacSettingGen.source_data.packet[i].lengthopt = lengthopt; //RANDOM; //FIXED;//INCREASING; 
       WlanDataEle[interface_sel].MacSettingGen.source_data.packet[i].payloadopt = payloadopt; //RANDOM; //FIXED; //INCREASING;    
    }


    

    for (i=0; i<8; i++) {

        /*security info*/

        if((i==0) && (WlanDataEle[interface_sel].MacSettingPort.Port[Port_1][i].encryption_info.type != TYPE_NONE)) {
            if(GCMSMS4== WlanDataEle[interface_sel].MacSettingPort.Port[Port_1][i].encryption_info.type)
    		{
        		memcpy(WlanDataEle[interface_sel].ConnectSetting.CAMData, CAMData_Gcmp256_Gcmsms4, CAM_DATA_SIZE*sizeof(u4Byte));
            }
            else if(GCMP256== WlanDataEle[interface_sel].MacSettingPort.Port[Port_1][i].encryption_info.type)
    		{
    			memcpy(WlanDataEle[interface_sel].ConnectSetting.CAMData, CAMData_Gcmp256_Gcmsms4, CAM_DATA_SIZE*sizeof(u4Byte));
    		}
    		else if(AES256== WlanDataEle[interface_sel].MacSettingPort.Port[Port_1][i].encryption_info.type)
    		{
    			memcpy(WlanDataEle[interface_sel].ConnectSetting.CAMData, CAMData_Aes256, CAM_DATA_SIZE*sizeof(u4Byte));
    		}
    		else if(GCMP128== WlanDataEle[interface_sel].MacSettingPort.Port[Port_1][i].encryption_info.type)
    		{
    			memcpy(WlanDataEle[interface_sel].ConnectSetting.CAMData, CAMData_Gcmp128, CAM_DATA_SIZE*sizeof(u4Byte));
    		}		
    		else if(WAPI == WlanDataEle[interface_sel].MacSettingPort.Port[Port_1][i].encryption_info.type)
    		{
    			memcpy(WlanDataEle[interface_sel].ConnectSetting.CAMData, CAMData_Wapi, CAM_DATA_SIZE*sizeof(u4Byte));
    		}
    		else
    		{
        		memcpy(WlanDataEle[interface_sel].ConnectSetting.CAMData, CAMData_Security, CAM_DATA_SIZE*sizeof(u4Byte));
            }			
        }
    }

    switch(WlanDataEle[interface_sel].MacSettingPort.Port[Port_1][0].encryption_info.type)
    {
    case WEP40:
        WlanDataEle[interface_sel].MacSettingPort.Port[Port_1][0].encryption_info.keyid=KEY0;
    break;    
    case WEP104:
        WlanDataEle[interface_sel].MacSettingPort.Port[Port_1][0].encryption_info.keyid=KEY0;
    break;    
    case TKIP:
        WlanDataEle[interface_sel].MacSettingPort.Port[Port_1][0].encryption_info.keyid=KEY1;        
    break;    
    case AES:
        WlanDataEle[interface_sel].MacSettingPort.Port[Port_1][0].encryption_info.keyid=KEY2;        
    break;        
    case WAPI:
        WlanDataEle[interface_sel].MacSettingPort.Port[Port_1][0].encryption_info.keyid=KEY0;        
    break;       
    default:
    break;    
    }

    
   
    hal_wlan_OB[interface_sel].wlanHalData.encryption = WlanDataEle[interface_sel].MacSettingPort.Port[Port_1][0].encryption_info.encryption;
    hal_wlan_OB[interface_sel].wlanHalData.decryption = WlanDataEle[interface_sel].MacSettingPort.Port[Port_1][0].encryption_info.decryption;
    hal_wlan_OB[interface_sel].wlanHalData.security_type = WlanDataEle[interface_sel].MacSettingPort.Port[Port_1][0].encryption_info.type;             
    hal_wlan_OB[interface_sel].wlanHalOp.HALWLSecurityInit(pAddressAdapter,&hal_wlan_OB[interface_sel].wlanHalData);

#endif
}
#if 0
u32 
CmdPollingReceiveTest(
    IN u16 argc, 
    IN u8* argv[]
)
{
    u32 WaitTime = 0;
    bool Result = 0;

    if (!argc) {
        RTL_CMD_DBG_LOG("NO Wait time Input!!!!\n");
        return 0;
    }

    WaitTime = Strtoul((const u8*)(argv[0]), (u8 **)NULL, 16);

    if ((WaitTime > 0) && (WaitTime < 1000)) { 
        Result = PollingReceivePacket((u32) WaitTime);
    }
    else {
        RTL_CMD_DBG_LOG("Error!! ==> Time is Error.\n");
    }

    if (!Result)
        RTL_CMD_DBG_LOG("ERROR!!!!!\n");

    return _TRUE;    
}
#endif

u32
CmdSWPSTest(
    IN u16 argc, 
    IN u8* argv[]

)
{
#ifdef CONFIG_SWPS_VERIFY

    PHAL_WLAN_ADDR     pAddressAdapter;
    u8                 cmdID;
    u8                 interface_sel;
    if (argc < 2) {
        RTL_CMD_DBG_LOG("Input parameters error!!!\n");
        return RT_STATUS_FAILURE;
    }
    interface_sel = Strtoul((const u8*)(argv[0]), (u8 **)NULL, 16);
    cmdID = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 16);

    pAddressAdapter = &hal_wlan_OB[interface_sel].WlanBaseAddress;

    if(SWPS_Verification(pAddressAdapter,cmdID) == _EXIT_FAILURE)
        RTL_CMD_DBG_LOG("SWPS test cmd %x fail.\n",cmdID);
    else
        RTL_CMD_DBG_LOG("SWPS test cmd %x success.\n",cmdID);

#endif
    return _TRUE;
}

u32
CmdMBIST(
    IN u16 argc, 
    IN u8* argv[]

)
{
    
    PHAL_WLAN_ADDR     pAddressAdapter;
    u8                 cmdID;
    u8                 interface_sel;
    if (argc < 2) {
        RTL_CMD_DBG_LOG("Input parameters error!!!\n");
        return RT_STATUS_FAILURE;
    }
    interface_sel = Strtoul((const u8*)(argv[0]), (u8 **)NULL, 16);
    cmdID = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 16);

    pAddressAdapter = &hal_wlan_OB[interface_sel].WlanBaseAddress;
    if(MBIST_Test(pAddressAdapter,cmdID) == _EXIT_FAILURE)
        RTL_CMD_DBG_LOG("MBIST test cmd %x fail.\n",cmdID);
    else
        RTL_CMD_DBG_LOG("MBIST test cmd %x success.\n",cmdID);
}

u32
CmdCAMExtend(
    IN u16 argc, 
    IN u8* argv[]

)
{
    
    PHAL_WLAN_ADDR     pAddressAdapter;
    PHAL_WLAN_DAT_ADAPTER pHalWLANDatAdapter;
    u8                 cmdID;
    u8                 interface_sel;
    if (argc < 2) {
        RTL_CMD_DBG_LOG("Input parameters error!!!\n");
        return RT_STATUS_FAILURE;
    }
    interface_sel = Strtoul((const u8*)(argv[0]), (u8 **)NULL, 16);
    cmdID = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 16);

    pAddressAdapter = &hal_wlan_OB[interface_sel].WlanBaseAddress;
    pHalWLANDatAdapter = &hal_wlan_OB[interface_sel].wlanHalData;
    if(CAM_Extend_test(pAddressAdapter,cmdID,pHalWLANDatAdapter) == _EXIT_FAILURE)
        RTL_CMD_DBG_LOG("CAM extend test cmd %x fail.\n",cmdID);
    else
        RTL_CMD_DBG_LOG("CAM extend test cmd %x success.\n",cmdID);
}
    
u32 
CmdRsetWL(
    IN u16 argc, 
    IN u8* argv[]
)
{
    u8 interface_sel;

    interface_sel = Strtoul((const u8*)(argv[0]), (u8 **)NULL, 16);

    //HAL_WLAN_OP HalWlanOp;
    TxDataPathInit(interface_sel);

    TxSetPktOption(interface_sel);

    WlanDataInit(interface_sel);

    HalRTKWLANOpInit(&hal_wlan_OB[interface_sel].wlanHalOp);                    


    memset(&hal_wlan_OB[interface_sel].wlanHalData,0,sizeof(hal_wlan_OB[interface_sel].wlanHalData));
    hal_wlan_OB[interface_sel].wlanHalOp.HALWLPwrInit(&hal_wlan_OB[interface_sel].WlanBaseAddress,&hal_wlan_OB[interface_sel].wlanHalData);
    hal_wlan_OB[interface_sel].wlanHalOp.HALWLMacInit(&hal_wlan_OB[interface_sel].WlanBaseAddress,&hal_wlan_OB[interface_sel].wlanHalData);    

    //Initial TX and RX read/write point procedure
    hal_wlan_OB[interface_sel].wlanHalOp.HALWLHCIDmaInit(&hal_wlan_OB[interface_sel].WlanBaseAddress,&hal_wlan_OB[interface_sel].wlanHalData);

    return _TRUE;    
}

u32 
CmdIoTest(
    IN u16 argc, 
    IN u8* argv[]
)
{
    u32 loopIndex = 0;
    u8 interface_sel;


    if (!argc) {
        RTL_CMD_DBG_LOG("Input Loop !!!\n");
        return 0;
    }


    interface_sel = Strtoul((const u8*)(argv[0]), (u8 **)NULL, 16);
    loopIndex = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 10);

    if( _FALSE == wlIOTest(&hal_wlan_OB[interface_sel].WlanBaseAddress,loopIndex)) {
        RTL_CMD_DBG_LOG("IO test fail !!! \n");
    }else {
        RTL_CMD_DBG_LOG("IO test Pass !!! \n");
    }

    return _TRUE;    
}

#if 0
u32 
CmdWLLoopBack(
    IN u16 argc, 
    IN u8* argv[]
)
{
    u32 LoopbackNum = 0;
    u32 LoopbackIndex = 0;
    TX_LINKLIST_ID Queue;
    TX_LINK_LIST_ELE *LLEle; 
    u32 AcIDInput;
    u32 WaitTime,LoopMode;
    u32 type = 0, temp, temp_2 = 0;

    LoopMode = 1;
    
    if (argc<4) {
        RTL_CMD_DBG_LOG("Wrong argument number!\r\n");
        return 0;
    }

    AcIDInput = Strtoul((const u8*)(argv[0]), (u8 **)NULL, 16);
    LoopbackNum = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 16);
    WaitTime = Strtoul((const u8*)(argv[2]), (u8 **)NULL, 16);
    type = Strtoul((const u8*)(argv[3]), (u8 **)NULL, 16);


    if (AcIDInput < 5) {
        Queue = (TX_LINKLIST_ID) AcIDInput;
    }
    else {
        RTL_CMD_DBG_LOG("No Support AC Type!!!!!!!\n");
        return 0;
    }

    if (!LoopbackNum) {
        RTL_CMD_DBG_LOG("0 times Loopback number!!!!!!!\n");
        return 0;

    }

    if (!WaitTime) {
        RTL_CMD_DBG_LOG("0 wait time!!!!!!!\n");
        return 0;
    }

    if (type > 1) {
        temp = 0x1000000*LoopbackNum;
    }
    else {

        temp = LoopbackNum;
        temp_2 = Queue;
    }

    LLEle = (TX_LINK_LIST_ELE*)(((u32)&WlanDataEle.StackLLEle)|0xa0000000);

    for (LoopbackIndex=0; LoopbackIndex< temp; LoopbackIndex++) {

        switch(type) {
            case 0:
                RTL_CMD_DBG_LOG("Loopback number: 0x%x; AC id:%d\n", LoopbackIndex, temp_2);                
                break;
            case 1:
                temp_2 = (LoopbackIndex & 0x3);
                RTL_CMD_DBG_LOG("Loopback number: 0x%x; AC id:%d\n", LoopbackIndex, temp_2);                                
                break;
            case 2:
                temp_2 = (LoopbackIndex & 0x3);
                if(!(LoopbackIndex & 0xFF)) {
                    RTL_CMD_DBG_LOG("Loopback number: 0x%x; AC id:%d\n", LoopbackIndex, temp_2);
                }
                
                break;
            case 3:
                break;

            default:
                break;
        }

        SendPacket(LoopMode, temp_2, (TX_LINK_LIST_ELE*)LLEle);

        if (!PollingReceivePacket(WaitTime)) {
            RTL_CMD_DBG_LOG("Loopback number %d; RX Error!!\n",LoopbackIndex);
            break;
        }
    }

    return _TRUE;    
}
#endif

u32 
CmdWLTxCont(
    IN u16 argc, 
    IN u8* argv[]
)
{
    u32 LoopbackNum = 0;
    u32 LoopbackIndex = 0;
    TX_LINKLIST_ID Queue;
    TX_LINK_LIST_ELE *LLEle; 
    u32 AcIDInput;
    u32 type = 0, LoopMax, InputACIndex, ThreType, TxExtendEn;
    u32 TxLen,LoopMode, TxFailCheckflag;
    u8 RateIndex = 0, QueueTypeIndex = 3, temp;
    u32 VoBufIndex, ViBufIndex, BeBufIndex, BkBufIndex, MagBufIndex = 0;
    u8 TxBw = 0;
    u8 interface_sel;
    PHAL_WLAN_ADDR pAddressAdapter;
    
    interface_sel = Strtoul((const u8*)(argv[0]), (u8 **)NULL, 16);
    pAddressAdapter = &hal_wlan_OB[interface_sel].WlanBaseAddress;

    WlanDataEle[interface_sel].TxFailCheckCounter = 0;
    InputACIndex = 0;
    WlanDataEle[interface_sel].WlRxResult = TRUE;
    LoopMode = 1;
    VoBufIndex = 0;
    ViBufIndex = 0;
    BeBufIndex = 0;
    BkBufIndex = 0;

#if 0
    if (argc<6) {
        RTL_CMD_DBG_LOG("Wrong argument number!\r\n");
        return 0;
    }
#endif
    AcIDInput = 0; //Strtoul((const u8*)(argv[0]), (u8 **)NULL, 16);
    LoopbackNum = 1000000; //Strtoul((const u8*)(argv[1]), (u8 **)NULL, 16);
    type = 0; //Strtoul((const u8*)(argv[2]), (u8 **)NULL, 16);
    ThreType = 0; //Strtoul((const u8*)(argv[3]), (u8 **)NULL, 16);
    TxLen = 0; //Strtoul((const u8*)(argv[4]), (u8 **)NULL, 16);
    RateIndex = 0; // Strtoul((const u8*)(argv[5]), (u8 **)NULL, 16);
    TxExtendEn = 0;




    

    //RTL_CMD_DBG_LOG("Tx Payload Len: %d/0x%x\n",TxLen,TxLen);

    if (AcIDInput < 5) {
        Queue = (TX_LINKLIST_ID) AcIDInput;
    }
    else {
        RTL_CMD_DBG_LOG("No Support AC Type!!!!!!!\n");
        return 0;
    }

    if (!LoopbackNum) {
        RTL_CMD_DBG_LOG("0 times Loopback number!!!!!!!\n");
        return 0;
    }


    if (TxExtendEn) {
        WlanDataEle[interface_sel].gEnableExTXBD = 1;
    }
    else
        WlanDataEle[interface_sel].gEnableExTXBD = 0;


        LoopMax = LoopbackNum;
        InputACIndex = Queue;

    switch (ThreType) {
        case 0:
            WlanDataEle[interface_sel].WlTxSegThreshold = 4096;
            WlanDataEle[interface_sel].WlExTxSegThreshold = 4096;
            break;
        case 1:
            WlanDataEle[interface_sel].WlTxSegThreshold = 1024;
            WlanDataEle[interface_sel].WlExTxSegThreshold = 1024;
            break;
        case 2:
            WlanDataEle[interface_sel].WlTxSegThreshold = 700;
            WlanDataEle[interface_sel].WlExTxSegThreshold = 700;
            break;
        case 3:
            WlanDataEle[interface_sel].WlTxSegThreshold = 512;
            WlanDataEle[interface_sel].WlExTxSegThreshold = 512;
            break;
        case 4:
            WlanDataEle[interface_sel].WlTxSegThreshold = 256;
            WlanDataEle[interface_sel].WlExTxSegThreshold = 256;
            break;
        case 5:
            WlanDataEle[interface_sel].WlTxSegThreshold = 160;
            WlanDataEle[interface_sel].WlExTxSegThreshold = 160;
            break;
        case 6:
            WlanDataEle[interface_sel].WlTxSegThreshold = 100;
            WlanDataEle[interface_sel].WlExTxSegThreshold = 100;
            break;
        case 7:
            WlanDataEle[interface_sel].WlTxSegThreshold = 40;
            WlanDataEle[interface_sel].WlExTxSegThreshold = 40;
            break;
        case 8:
            WlanDataEle[interface_sel].WlTxSegThreshold = 32;            
            WlanDataEle[interface_sel].WlExTxSegThreshold = 32;
            break;

        default:
            break;
    }

    //RTL_CMD_DBG_LOG("Segment Threshold Value: %d\n",WlanDataEle.WlTxSegThreshold);
    RTL_CMD_DBG_LOG("Loopback number: %d; AC id:%d\n", LoopbackIndex, InputACIndex);
#if 0

    if (TxLen) {
        WlanDataEle.gTxLen = TxLen;
    }


    if (8 == type) {
        //Download 8051 firmware
        /*
        RTL_CMD_DBG_LOG("Down Load RAM code\n");            
        if(InitFirmware88XX(NULL,downloadRAM)) {
           RTL_CMD_DBG_LOG("Down Load RAM code Fail!!!!!!!\n");
           goto  ResultError;
        }
        */
            
        //Enable 8051 fw for access
        WRITE_MEM8(0xb8640089,0x10);
    }
#endif 

    //WlanDataEle.TRXLock = 0;

    for (LoopbackIndex=0; LoopbackIndex< LoopMax; LoopbackIndex++) {

        //if (!(LoopbackIndex & 0xF)) {
            InputACIndex++;
            if(InputACIndex > 4) {
               InputACIndex = 0; 
            }

        if(HAL_WLAN_R32(0x210)!=0)  {
            RTL_CMD_DBG_LOG("TX DMA error = %x \n",HAL_WLAN_R32(0x210));
            goto ResultError;
        }
      
        if (!WlanDataEle[interface_sel].WlRxResult) {
            RTL_CMD_DBG_LOG("Loop Num: %d; Rx error!!!!!!! \n", LoopbackIndex);
            goto ResultError;
        }
       
        RTL_CMD_DBG_LOG("Start send packet ! LoopbackIndex = %d \n",LoopbackIndex);
        WlanDataEle[interface_sel].TxCount = LoopbackIndex;
        trx_lock = 1;

        switch (InputACIndex) {
            case 0://VO
                VoBufIndex++;
                VoBufIndex = VoBufIndex & 0xf;
                temp = (u8) VoBufIndex;
                break;
            case 1://VI
                ViBufIndex++;
                ViBufIndex = ViBufIndex & 0xf;            
                temp = (u8) ViBufIndex;
                break;
            case 2://BE
                BeBufIndex++;
                BeBufIndex = BeBufIndex & 0xf;            
                temp = (u8) BeBufIndex;
                break;
            case 3://BK
                BkBufIndex++;
                BkBufIndex = BkBufIndex & 0xf;            
                temp = (u8) BkBufIndex;
                break;
            case 4:
                MagBufIndex++;
                MagBufIndex = MagBufIndex & 0xf;            
                temp = (u8) MagBufIndex;
                break;
            default:
                RTL_CMD_DBG_LOG("AC Index Error!!!!");
                break;
        }

        //LLEle = (TX_LINK_LIST_ELE*)(((u32)&WlanDataEle.StackLLEle[InputACIndex][0])|0xa0000000);
        RTL_CMD_DBG_LOG("Start send packet ! \n");

#ifdef CONFIG_SECURITY_VERIFY  
        LLEle = (TX_LINK_LIST_ELE*)(((u32)&WlanDataEle[interface_sel].StackLLEle[InputACIndex][temp]));
#else
        LLEle = (TX_LINK_LIST_ELE*)(((u32)&WlanDataEle[interface_sel].StackLLEle[InputACIndex][temp])|0xa0000000);
#endif
RTL_CMD_DBG_LOG("Start send packet ! \n");

//        if (!SendPacket(LoopMode, InputACIndex, (TX_LINK_LIST_ELE*)LLEle)) {
#if 1
       LLEle->PortId = 0;
       if((Queue > TX_LINKLIST_MAG) && (Queue <TX_LINKLIST_MAX))
          LLEle->PortDestNum = Queue-5;
       else
          LLEle->PortDestNum = 0;

       if (!SendPacket(pAddressAdapter,LoopMode, InputACIndex, (TX_LINK_LIST_ELE*)LLEle)) {
           RTL_CMD_DBG_LOG("Loop Num: %d;AC %d, TX ERROR!!!!!\n", LoopbackIndex, InputACIndex);
            break;
       }
#else
            WlanDataEle.TxFailCheckCounter++;
            TxFailCheckflag = 1;
            while (TxFailCheckflag) {

                 if (!SendPacket(LoopMode, InputACIndex, (TX_LINK_LIST_ELE*)LLEle)) {
                    WlanDataEle.TxFailCheckCounter++;                    

                    if (WlanDataEle.TxFailCheckCounter > 1000) {
                        RTL_CMD_DBG_LOG("Loop Num: %d;AC %d, TX ERROR!!!!!\n", LoopbackIndex, InputACIndex);
                        TxFailCheckflag = 0;
                    }
                 }
                 else {
                    WlanDataEle.TxFailCheckCounter = 0;
                    TxFailCheckflag = 0;
                 }
            }
            
            if (WlanDataEle.TxFailCheckCounter > 1000) {
                break;
            }
#endif          
            REG32(0x80500008) = WlanDataEle[interface_sel].TxCount;

            while(trx_lock)
            {
                udelay(10000);
                REG32(0x8050000C) = WlanDataEle[interface_sel].TxCount;
    //            RTL_CMD_DBG_LOG("Polling RX Done !!!! \n");
            }



//        }
//        else {
//            WlanDataEle.TxFailCheckCounter = 0;
//        }

          //RTL_CMD_DBG_LOG("Loopback number: 0x%x; AC id:%d\n", LoopbackIndex, InputACIndex);


          //udelay(10000);
//        __delay(100000000);
    }
    goto 
ResultSuccess;


ResultError:
    RTL_CMD_DBG_LOG("=======    Fail   =======\n");
    return _FALSE;


ResultSuccess:
    RTL_CMD_DBG_LOG("========  Susccess Finish  ==========\n");

    return _TRUE;

}

#if 0
u32 
CmdWLTx(
    IN u16 argc, 
    IN u8* argv[]
)
{
    u32 LoopbackNum = 0;
    u32 LoopbackIndex = 0;
    TX_LINKLIST_ID Queue;
    TX_LINK_LIST_ELE *LLEle; 
    u32 AcIDInput;
    u32 LoopMax, InputACIndex,TxFailCheckflag;
    u32 TxLen, LoopMode;
    u8 RateIndex = 0, temp;
    u8 TxBw = 0;

    InputACIndex = 0;
    WlanDataEle.WlRxResult = TRUE;
    LoopMode = 0;

    if (argc<3) {
        RTL_CMD_DBG_LOG("Wrong argument number!\r\n");
        return 0;
    }

    //AcIDInput = strtoul((const char*)(argv[0]), (char **)NULL, 16);
    LoopbackNum = Strtoul((const u8*)(argv[0]), (u8 **)NULL, 16);
    TxLen = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 16);
    RateIndex = Strtoul((const u8*)(argv[2]), (u8 **)NULL, 16);


    //3 Set for RF team test
    AcIDInput = 0;
    RTL_CMD_DBG_LOG("============= Start TX =========================\n");    
    RTL_CMD_DBG_LOG("    AC Queue Index: %d\n",AcIDInput);
    RTL_CMD_DBG_LOG("    Tx Number: %d\n",LoopbackNum);
    RTL_CMD_DBG_LOG("    Tx Payload Len: %d\n",TxLen);    
    RTL_CMD_DBG_LOG("    Rate Index: %d\n",RateIndex);    
    
    if (AcIDInput < 5) {
        Queue = (TX_LINKLIST_ID) AcIDInput;
    }
    else {
        RTL_CMD_DBG_LOG("No Support AC Type!!!!!!!\n");
        return 0;
    }

    if (!LoopbackNum) {
        RTL_CMD_DBG_LOG("0 times Loopback number!!!!!!!\n");
        return 0;
    }

    if (RateIndex > 26) {
        RTL_CMD_DBG_LOG("No Support Rate Index!!!!!!!\n");
        return 0;
    }

    LoopMax = LoopbackNum;
    InputACIndex = Queue;


    if (TxLen) {
        WlanDataEle.gTxLen = TxLen;
    }

    ReplaceTxPktOption(RateIndex,3, TxBw); //set High queue


    for (LoopbackIndex=0; LoopbackIndex< LoopMax; LoopbackIndex++) {
        temp = LoopbackIndex & 0x3;
        LLEle = (TX_LINK_LIST_ELE*)(((u32)&WlanDataEle.StackLLEle[temp])|0xa0000000);
     

#if 0
           if (!SendPacket(LoopMode, InputACIndex, (TX_LINK_LIST_ELE*)LLEle)) {
               RTL_CMD_DBG_LOG("Loop Num: %d;AC %d, TX ERROR!!!!!\n", LoopbackIndex, InputACIndex);
                break;
           }
#else
            WlanDataEle.TxFailCheckCounter++;
            TxFailCheckflag = 1;
            while (TxFailCheckflag) {

                 if (!SendPacket(LoopMode, InputACIndex, (TX_LINK_LIST_ELE*)LLEle)) {
                    WlanDataEle.TxFailCheckCounter++;                    

                    if (WlanDataEle.TxFailCheckCounter > 1000) {
                        RTL_CMD_DBG_LOG("\nLoop Num: %d;AC %d, TX ERROR!!!!!\n", LoopbackIndex, InputACIndex);
                        TxFailCheckflag = 0;
                    }
                 }
                 else {
                    WlanDataEle.TxFailCheckCounter = 0;
                    TxFailCheckflag = 0;
                 }
            }
            
            if (WlanDataEle.TxFailCheckCounter > 1000) {
                break;
            }
#endif 

        

    }
    RTL_CMD_DBG_LOG("\n============= Finish TX =========================\n");    

    return _TRUE;

}
#endif 

u32 
CmdDmaBeacon(
    IN u16 argc, 
    IN u8* argv[]
)
{
    u8                  BssidNum;
    bool                EnSwDownload;
    u8                  interface_sel;
    PHAL_WLAN_ADDR pAddressAdapter;


    if (argc<3) {
        RTL_CMD_DBG_LOG("Input parameters error!!!");
        return 0;
    }


    
    interface_sel = Strtoul((const u8*)(argv[0]), (u8 **)NULL, 16);
    BssidNum = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 16);
    EnSwDownload= Strtoul((const u8*)(argv[2]), (u8 **)NULL, 16);

    pAddressAdapter = &hal_wlan_OB[interface_sel].WlanBaseAddress;

    RTL_CMD_DBG_LOG("DMA Beacon to resever pages\n");        
    DmaBeacon(pAddressAdapter,BssidNum, EnSwDownload);

    return _TRUE;   

}

u32 
CmdWlPower(
    IN u16 argc, 
    IN u8* argv[]
)
{

    u8     powerSeqMode, interface_sel;
    PHAL_WLAN_ADDR pAddressAdapter;
    
    if (!argc) {
          RTL_CMD_DBG_LOG("Input parameters error!!!");
          return 0;
    }
    
    interface_sel = Strtoul((const u8*)(argv[0]), (u8 **)NULL, 10);

    powerSeqMode = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 10);
    
    pAddressAdapter = &hal_wlan_OB[interface_sel].WlanBaseAddress;

    switch(powerSeqMode)
    {
    case CARD_ENABLE:
    	if (!HalPwrSeqCmdParsing(pAddressAdapter,PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK,PWR_INTF_PCI_MSK, *hal_wlan_OB[interface_sel].PwrCfgTable.card_enable_flow))
        {
            RTL_CMD_DBG_LOG("CARD_ENABLE init fail!!!\n");
            return RT_STATUS_FAILURE;
        }
    break;
    case CARD_DISABLE:
    	if (!HalPwrSeqCmdParsing(pAddressAdapter,PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK,PWR_INTF_PCI_MSK, *hal_wlan_OB[interface_sel].PwrCfgTable.card_disable_flow))
        {
            RTL_CMD_DBG_LOG("CARD_DISABLE init fail!!!\n");
            return RT_STATUS_FAILURE;
        }        
    break;
    case POWER_ON:
    	if (!HalPwrSeqCmdParsing(pAddressAdapter,PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK,PWR_INTF_PCI_MSK, *hal_wlan_OB[interface_sel].PwrCfgTable.power_on_flow))
        {
            RTL_CMD_DBG_LOG("POWER_ON init fail!!!\n");
            return RT_STATUS_FAILURE;
        }        
    break;
    case RADIO_ON:
    	if (!HalPwrSeqCmdParsing(pAddressAdapter,PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK,PWR_INTF_PCI_MSK, *hal_wlan_OB[interface_sel].PwrCfgTable.power_on_flow))
        {
            RTL_CMD_DBG_LOG("RADIO_ON init fail!!!\n");
            return RT_STATUS_FAILURE;
        }        
    break;
    case RADIO_OFF:
    	if (!HalPwrSeqCmdParsing(pAddressAdapter,PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK,PWR_INTF_PCI_MSK, *hal_wlan_OB[interface_sel].PwrCfgTable.radio_off_flow))
        {
            RTL_CMD_DBG_LOG("RADIO_OFF init fail!!!\n");
            return RT_STATUS_FAILURE;
        }        
    break;    
    case ENTER_LPS:
    	if (!HalPwrSeqCmdParsing(pAddressAdapter,PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK,PWR_INTF_PCI_MSK, *hal_wlan_OB[interface_sel].PwrCfgTable.enter_lps_flow))
        {
            RTL_CMD_DBG_LOG("ENTER_LPS init fail!!!\n");
            return RT_STATUS_FAILURE;
        }        
    break;
    case LEAVE_LPS:
    	if (!HalPwrSeqCmdParsing(pAddressAdapter,PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK,PWR_INTF_PCI_MSK, *hal_wlan_OB[interface_sel].PwrCfgTable.leave_lps_flow))
        {
            RTL_CMD_DBG_LOG("LEAVE_LPS init fail!!!\n");
            return RT_STATUS_FAILURE;
        }
    break;

    default:
        
        RTL_CMD_DBG_LOG("Wrong argument!!! \n");        
    break;
    }

    
    return;
}

u32 
CmdWlAllTest(
    IN u16 argc, 
    IN u8* argv[]
)
{
    u1Byte i;
    u4Byte interface_num;
    u4Byte interface_sel;
    PHAL_WLAN_ADDR pAddressAdapter;
    
    interface_num = Strtoul((const u8*)(argv[0]), (u8 **)NULL, 16);
    interface_sel = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 16);
    
    pAddressAdapter = &hal_wlan_OB[interface_sel].WlanBaseAddress;

    if(!WlCtrlOperater(1,interface_num))
    {
        RTL_ALL_DBG_LOG("Wifi init fail \n");
        return;
    }

    mdelay(1);

    if(RT_STATUS_SUCCESS != InitMIPSFirmware88XX(pAddressAdapter))
    {
        RTL_ALL_DBG_LOG("FW download fail \n");
        return;        
    }

    mdelay(1);

    if(!WlCtrlOperater(1,interface_num))
     {
         RTL_ALL_DBG_LOG("Wifi init fail \n");
        return;         
     }
    
    mdelay(1);

    for(i=0;i<10;i++)
    {
        WlanDataEle[interface_sel].WlRxResult = _FALSE;
        if(!CmdSendTest(1,argv[1]))
        {
            RTL_ALL_DBG_LOG("Wifi TX fail \n");
            return;        
        }

        mdelay(5000);
        
        if (!WlanDataEle[interface_sel].WlRxResult) {
            RTL_ALL_DBG_LOG("Wlan RX error \n");
            return;        
        }else   {
            RTL_ALL_DBG_LOG("Wlan RX ok \n");
        }
    }

    RTL_ALL_DBG_LOG("Wifi All Test pass ! \n");
    
}
u32
TxRutineForTestall(
    IN u8 interface_sel,
    IN u8 TxLoopNum
){
    u1Byte txloopindex;
    u8* para[1]= {&interface_sel};

    for(txloopindex=0;txloopindex<TxLoopNum;txloopindex++)
    {
        WlanDataEle[interface_sel].WlRxResult = _FALSE;
        if(!CmdSendTest(1,para))
        {
            RTL_ALL_DBG_LOG("Wifi TX fail \n");
            return _FALSE;        
        }

        mdelay(500);
        
        if (!WlanDataEle[interface_sel].WlRxResult) {
            RTL_ALL_DBG_LOG("Wlan RX error \n");
            return _FALSE;        
        }else   {
            RTL_ALL_DBG_LOG("Wlan RX ok \n");
        }
    }
    return _TRUE;     
}

u32
CmdWLTestALL(
    IN u16 argc, 
    IN u8* argv[]
)
{
    u4Byte interface_num;
    u1Byte interface_sel;
    PHAL_WLAN_ADDR pAddressAdapter;
    u8 securetype;
    u8 encrypt_HWSW;
    u8 TxExtendEn;
    u8 lengthopt;
    u8 payloadopt;
    u8 TxdescInSRAM;
    u8 SegThresh;
    u8 pwrseqmode;
    u1Byte* para[8];
    u1Byte loopindex;

    interface_num = Strtoul((const u8*)(argv[0]), (u8 **)NULL, 16);
    interface_sel = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 16);
    pAddressAdapter = &hal_wlan_OB[interface_sel].WlanBaseAddress;
    
    //3 init wlan
    if(!WlCtrlOperater(1,interface_num))
    {
        RTL_ALL_DBG_LOG("Wifi init fail \n");
        return;
    }

    mdelay(1);

    //3 IO test, endian test
    loopindex=5;
    if( _FALSE == wlIOTest(pAddressAdapter,loopindex)) {
        RTL_CMD_DBG_LOG("IO test fail !!! \n");
    }else {
        RTL_CMD_DBG_LOG("IO test Pass !!! \n");
    }
    
    mdelay(1);

    //3 re-init wlan after IO test due to reg value was changed
    if(!WlCtrlOperater(1,interface_num))
    {
        RTL_ALL_DBG_LOG("Wifi init fail \n");
        return;
    }
    
    mdelay(1);

    //3 dma beacon, to check bcn err int
    DmaBeacon(pAddressAdapter, 1, 0);
    if(HAL_WLAN_R32(REG_HISR0) & (BIT25|BIT26))
        RTL_CMD_DBG_LOG("Success, get tx bcn ok/fail int !!! \n");
    else    
        RTL_CMD_DBG_LOG("Fail, no tx bcn ok/fail int !!! \n");

    mdelay(1);

    //3 Normal TXBD Single TX packet
    securetype = TYPE_NONE;
    encrypt_HWSW = HW;
    TxExtendEn = 0;
    lengthopt = FIXED;
    payloadopt = FIXED;
    TxdescInSRAM = 0;
    SegThresh = 0;
    para[0]=&interface_sel;
    para[1]=&securetype;
    para[2]=&encrypt_HWSW;
    para[3]=&TxExtendEn;
    para[4]=&TxdescInSRAM;
    para[5]=&lengthopt;
    para[6]=&payloadopt;
    para[7]=&SegThresh;    
    CmdSetPktOpt(8,para);
    if(!TxRutineForTestall(interface_sel,1)){
       RTL_CMD_DBG_LOG("Test Normal TXBD Single TX packet FAIL!!\n");
       return;
    }

    //3 Normal TXBD Burst TX packet Incresing/Random pattern Fixed/Random length
    //INCREASING+FIXED
    payloadopt = INCREASING;
    CmdSetPktOpt(8,para);
    if(!TxRutineForTestall(interface_sel,10)){
       RTL_CMD_DBG_LOG("Test Normal TXBD Burst TX packet Incresing/Random pattern Fixed/Random length(INCREASING+FIXED) FAIL!!\n");
       return;
    }
    
    //RANDOM+RANDOM
    payloadopt = RANDOM;
    lengthopt = RANDOM;
    CmdSetPktOpt(8,para);
    if(!TxRutineForTestall(interface_sel,10)){
       RTL_CMD_DBG_LOG("Test Normal TXBD Burst TX packet Incresing/Random pattern Fixed/Random length(RANDOM+RANDOM) FAIL!!\n");
       return;
    }


    //3 Extend TXBD Single TX packet
    lengthopt = FIXED;
    payloadopt = FIXED;
    TxExtendEn = 1;
    SegThresh = 4; //set smaller threshold to force use EXTXBD
    CmdSetPktOpt(8,para);
    if(!TxRutineForTestall(interface_sel,1)){
       RTL_CMD_DBG_LOG("Test Extend TXBD Single TX packet FAIL!!\n");
       return;
    }

    //3 Extend TXBD Burst TX packet Incresing/Random pattern Fixed/Random length
    lengthopt = RANDOM;
    payloadopt = RANDOM;
    CmdSetPktOpt(8,para);
    if(!TxRutineForTestall(interface_sel,10)){
       RTL_CMD_DBG_LOG("Test Extend TXBD Burst TX packet Incresing/Random pattern Fixed/Random length(RANDOM+RANDOM) FAIL!!\n");
       return;
    }

    //3 Security test
    TxExtendEn = 0;
    SegThresh = 0;
    encrypt_HWSW = 1;
    //WEP HW encrypt/HW decrypt
    securetype = WEP40;
    CmdSetPktOpt(8,para);
    if(!TxRutineForTestall(interface_sel,5)){
       RTL_CMD_DBG_LOG("Test Security test WEP HW encrypt/HW decrypt FAIL!!\n");
       return;
    }
    
    //TKIP HW encrypt/HW decrypt
    securetype = TKIP;
    CmdSetPktOpt(8,para);
    if(!TxRutineForTestall(interface_sel,5)){
       RTL_CMD_DBG_LOG("Test Security test TKIP HW encrypt/HW decrypt FAIL!!\n");
       return;
    }
    //AES HW encrypt/HW decrypt
    securetype = AES;
    CmdSetPktOpt(8,para);
    if(!TxRutineForTestall(interface_sel,5)){
       RTL_CMD_DBG_LOG("Test Security test AES HW encrypt/HW decrypt FAIL!!\n");
       return;
    }
    
    //WAPI HW encrypt/HW decrypt
    securetype = WAPI;
    CmdSetPktOpt(8,para);
    if(!TxRutineForTestall(interface_sel,5)){
       RTL_CMD_DBG_LOG("Test Security test WAPI HW encrypt/HW decrypt FAIL!!\n");
       return;
    }

    
    encrypt_HWSW = 0;
    //AES SW encrypt/HW encrypt
    securetype = AES;
    CmdSetPktOpt(8,para);
    if(!TxRutineForTestall(interface_sel,5)){
       RTL_CMD_DBG_LOG("Test Security test AES SW encrypt/HW decrypt FAIL!!\n");
       return;
    }
    
    //WAPI SW encrypt/HW encrypt
    securetype = WAPI;
    CmdSetPktOpt(8,para);
    if(!TxRutineForTestall(interface_sel,5)){
       RTL_CMD_DBG_LOG("Test Security test WAPI SW encrypt/HW decrypt FAIL!!\n");
       return;
    }

    //restore
    securetype = TYPE_NONE;
    encrypt_HWSW = 1;
    CmdSetPktOpt(8,para);

    
    //3 card disable and enable
    
    para[1]=&pwrseqmode;

    for(loopindex = 0; loopindex< 5;loopindex++){
        pwrseqmode = 1;
        CmdWlPower(2,para);
        pwrseqmode = 0;
        CmdWlPower(2,para);
        if(!TxRutineForTestall(interface_sel,1)){
           RTL_CMD_DBG_LOG("TX after card disable and enable FAIL!!\n");
           return;
        }
    }


    //3 tx loop max
    //init wlan
    if(!WlCtrlOperater(1,interface_num))
    {
        RTL_ALL_DBG_LOG("Wifi init fail \n");
        return;
    }
    
    CmdWLTxCont(1,para);
   
    
}

u32 
CmdWlDumpAll(
    IN u16 argc, 
    IN u8* argv[]
)
{

    int i, j, len;
    unsigned char tmpbuf[100];
    u4Byte interface_sel;
    u1Byte* interface_name;
    PHAL_WLAN_ADDR pAddressAdapter;
    //interface_sel = Strtoul((const u8*)(argv[0]), (u8 **)NULL, 16);
    interface_sel = Strtoul((const char*)(argv[0]), (char **)NULL, 16);		
    
    pAddressAdapter = &hal_wlan_OB[interface_sel].WlanBaseAddress;

    printf("\nMAC Registers:\n");
	// 8814 merge issue
	for (i=0; i<0x1000; i+=0x10) {
		len = sprintf((char *)tmpbuf, "%03X\t", i);
		for (j=i; j<i+0x10; j+=4)
			len += sprintf((char *)(tmpbuf+len), "%08X ", (unsigned int)(*((volatile u4Byte*)(pAddressAdapter->WLBASE_ADDR + j))));
		len += sprintf((char *)(tmpbuf+len), "\n");
		printf((char *)tmpbuf);
	}
    
	printf("\n");
}



/*************************************
        For wlan command 
*************************************/

#if 1
void ddump(unsigned char * pData, int len)
{
	unsigned char *sbuf = pData;	
	int length=len;

	int i=0,j,offset;
	dprintf(" [Addr]   .0 .1 .2 .3 .4 .5 .6 .7 .8 .9 .A .B .C .D .E .F\r\n" );

	while(i< length)
	{		
			
			dprintf("%08X: ", (sbuf+i) );

			if(i+16 < length)
				offset=16;
			else			
				offset=length-i;
			

			for(j=0; j<offset; j++)
				dprintf("%02x ", sbuf[i+j]);	

			for(j=0;j<16-offset;j++)	//a last line
			dprintf("   ");


			dprintf("    ");		//between byte and char
			
			for(j=0;  j<offset; j++)
			{	
				if( ' ' <= sbuf[i+j]  && sbuf[i+j] <= '~')
					dprintf("%c", sbuf[i+j]);
				else
					dprintf(".");
			}
			dprintf("\n\r");
			i+=16;
	}

	//dprintf("\n\r");


	
}
#endif 

int CmdDumpWLWord( int argc, char* argv[] )
{
    
    unsigned int src;
    unsigned int len,i;
    u1Byte interface_sel;
    PHAL_WLAN_ADDR pAddressAdapter;
    interface_sel = Strtoul((const char*)(argv[0]), (char **)NULL, 16);		
    pAddressAdapter = &hal_wlan_OB[interface_sel].WlanBaseAddress;

    if(argc<2)
    {   dprintf("Wrong argument number!\r\n");
        return;
    }
    
    if(argv[1]) 
    {   
        src = Strtoul((const char*)(argv[1]), (char **)NULL, 16);
        if (src > pAddressAdapter->WLBASE_ADDR) {
            src &= 0xFFFF;
        src|= pAddressAdapter->WLBASE_ADDR;
        }
        else {
            src |= pAddressAdapter->WLBASE_ADDR;
        }
    }
    else
    {   dprintf("Wrong argument number!\r\n");
        return;     
    }
                
    if(!argv[2])
        len = 1;
    else
        len= Strtoul((const char*)(argv[2]), (char **)NULL, 10);            

    while ( (src) & 0x03)
        src++;

    for(i=0; i< len ; i+=4,src+=16)
    {   
        dprintf("%08X:  %08X    %08X    %08X    %08X\n",
        src, *(unsigned int *)(src), *(unsigned int *)(src+4), 
        *(unsigned int *)(src+8), *(unsigned int *)(src+12));
    }  
}


//---------------------------------------------------------------------------
int CmdDumpWLByte( int argc, char* argv[] )
{
	
	unsigned long src;
	unsigned int len,i;
    u1Byte interface_sel;
    PHAL_WLAN_ADDR pAddressAdapter;
    interface_sel = Strtoul((const char*)(argv[0]), (char **)NULL, 16);		
    pAddressAdapter = &hal_wlan_OB[interface_sel].WlanBaseAddress;

	if(argc<2)
	{	dprintf("Wrong argument number!\r\n");
		return;
	}
	
	src = Strtoul((const char*)(argv[1]), (char **)NULL, 16);		
	if(!argv[2])
		len = 16;
	else
	len= Strtoul((const char*)(argv[2]), (char **)NULL, 10);			

    if (src > pAddressAdapter->WLBASE_ADDR) {
        src &= 0xFFFF;
        src |= pAddressAdapter->WLBASE_ADDR;
    }
    else {
        src |= pAddressAdapter->WLBASE_ADDR;
    }
    
	ddump((unsigned char *)src,len);
}


int CmdWriteWLWord( int argc, char* argv[] )
{
	
	unsigned long src;
	unsigned int value,i;
	u1Byte interface_sel;
    PHAL_WLAN_ADDR pAddressAdapter;
    
    interface_sel = Strtoul((const char*)(argv[0]), (char **)NULL, 16);		
    
	src = Strtoul((const char*)(argv[1]), (char **)NULL, 16);		

    pAddressAdapter = &hal_wlan_OB[interface_sel].WlanBaseAddress;
    
    if (src > pAddressAdapter->WLBASE_ADDR) {
        src &= 0xFFFF;
        src |= pAddressAdapter->WLBASE_ADDR;
    }
    else {
    src |= pAddressAdapter->WLBASE_ADDR;
    }

    value= Strtoul((const char*)(argv[i+2]), (char **)NULL, 16);    

	while ( (src) & 0x03)
		src++;

    *(volatile unsigned int *)(src) = value;
	
}


//---------------------------------------------------------------------------
int CmdWriteWLByte( int argc, char* argv[] )
{
	
	unsigned long src;
	unsigned char value,i;
	u1Byte interface_sel;
    PHAL_WLAN_ADDR pAddressAdapter;
    
    interface_sel = Strtoul((const char*)(argv[0]), (char **)NULL, 16);		
	src = Strtoul((const char*)(argv[1]), (char **)NULL, 16);		
    
    pAddressAdapter = &hal_wlan_OB[interface_sel].WlanBaseAddress;

    if (src > pAddressAdapter->WLBASE_ADDR) {
        src &= 0xFFFF;
        src |= pAddressAdapter->WLBASE_ADDR;
    }
    else {
        src |= pAddressAdapter->WLBASE_ADDR;
    }

	for(i=1;i<argc-1;i++,src++)
	{
		value= Strtoul((const char*)(argv[i+1]), (char **)NULL, 16);	
		*(volatile unsigned char *)(src) = value;
	}
	
}

static int CmdWlanDW(cmd_tbl_t * cmdtp, int flag, int argc, char * argv[])
{
    CmdDumpWLWord(argc - 1,(u8**)(argv+1) );
    return CMD_RET_SUCCESS;
}

static int CmdWlanDB(cmd_tbl_t * cmdtp, int flag, int argc, char * argv[])
{
    CmdDumpWLByte(argc - 1,(u8**)(argv+1) );
    return CMD_RET_SUCCESS;
}

static int CmdWlanEW(cmd_tbl_t * cmdtp, int flag, int argc, char * argv[])
{
    CmdWriteWLWord(argc - 1,(u8**)(argv+1) );
    return CMD_RET_SUCCESS;
}

static int CmdWlanEB(cmd_tbl_t * cmdtp, int flag, int argc, char * argv[])
{
    CmdWriteWLByte(argc - 1,(u8**)(argv+1) );
    return CMD_RET_SUCCESS;
}

U_BOOT_CMD(idd, 5, 0,	CmdWlanDW,
        "idd <Interface, Hex> <Address, Hex> <Len, Dec>:",
        "Dump wlan register; \n" 
        "Unit: 4Bytes"
);


U_BOOT_CMD(od, 5, 0,	CmdWlanEW,
        "od <Interface, Hex> <Address, Hex> <Value, Hex>:",
        "Write wlan register \n"
        "Unit: 4Bytes"
);


U_BOOT_CMD(ib, 5, 0,	CmdWlanDB,
        "ib <Interface, Hex> <Address, Hex> <Len, Dec>:",
        "Dump wlan register; \n" 
        "Unit: 1Bytes"
);


U_BOOT_CMD(ob, 5, 0,	CmdWlanEB,
        "ob <Interface, Hex> <Address, Hex> <Value, Hex>:",
        "Write wlan register \n"
        "Unit: 1Bytes"
);



