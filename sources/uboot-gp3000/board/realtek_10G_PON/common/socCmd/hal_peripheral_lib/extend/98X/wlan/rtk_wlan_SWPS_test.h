
#ifndef _RTK_WLAN_SWPS_H_
#define _RTK_WLAN_SWPS_H_

extern WLAN_DATA_ELEMENT WlanDataEle[2];

//3 
//SWPS test case
enum _SWPS_test_cmdID_{
    Indirect_Access,
    SWPS_EN_disable,
    SWPS_polling,
    SWPS_trigger_packetnum,
    SWPS_trigger_timer,
    SWPS_trigger_condition_reset,
    SWPS_many_report_length,
    SWPS_dropID,
	SWPS_PS_change,
    SWPS_PS_timer,
    SWPS_TXHWSC,
    SWPS_retrytodrop,
};


RTK_STATUS
SWPSInit(
    u1Byte  interface_sel,
    VOID    *Data
);

RTK_STATUS
SWPS_Verification(
    VOID    *pAdapter,
    u8 CmdID,
    u8 sub_cmdID
);

VOID
sendpacket_rutine(
    VOID    *pAdapter,
    u4Byte  pkt_num,
    u1Byte  AcID 
);

#endif  // #ifndef _RTK_WLAN_SWPS_H_

