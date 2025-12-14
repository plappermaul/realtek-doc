
#ifndef _CMD_WLAN_H_

//#include "rtk_hal_wlan.h"
#include "peripheral.h"

typedef enum _RTK_POWERSEQUENCE_ {
    CARD_ENABLE =       0,
    CARD_DISABLE =      1,
    POWER_ON =          2,
    RADIO_ON =          3,
    RADIO_OFF =         4,
    ENTER_LPS =         5,
    LEAVE_LPS =         6,
}RTK_POWERSEQUENCE, *PRTK_POWERSEQUENCE;

u32
CmdSetPktOpt(
    IN  u16 argc, 
    IN  u8  *argv[] 
);


u32
CmdSWPSTest(
    IN u16 argc, 
    IN u8* argv[]

);

u32
CmdMBIST(
    IN u16 argc, 
    IN u8* argv[]

);
u32
CmdCAMExtend(
    IN u16 argc, 
    IN u8* argv[]

);
u32
CmdWLTestALL(
    IN u16 argc, 
    IN u8* argv[]
);

u32
CmdFWDL(
    IN  u16 argc, 
    IN  u8  *argv[] 
);
u32 
CmdFWVERI(
    IN  u16  argc,
    IN  u8* argv[]
);

u32 
CmdWLOprater(
    IN  u16  argc,
    IN  u8* argv[]
);

u32 CmdSendTest(
    IN  u16   argc, 
    IN  u8* argv[]
);

u32 
CmdPollingReceiveTest(
    IN u16 argc, 
    IN u8* argv[]
);

u32 
CmdRsetWL(
    IN u16 argc, 
    IN u8* argv[]
);

u32 
CmdIoTest(
    IN u16 argc, 
    IN u8* argv[]
);

u32 
CmdWLLoopBack(
    IN u16 argc, 
    IN u8* argv[]
);

u32 
CmdWLTxCont(
    IN u16 argc, 
    IN u8* argv[]
);

u32 
CmdWLTxCont(
    IN u16 argc, 
    IN u8* argv[]
);

u32 
CmdDmaBeacon(
    IN u16 argc, 
    IN u8* argv[]
);

u32 
CmdWLTx(
    IN u16 argc, 
    IN u8* argv[]
);

u32 
CmdWlPower(
    IN u16 argc, 
    IN u8* argv[]
);

u32 
CmdWlAllTest(
    IN u16 argc, 
    IN u8* argv[]
);

u32 
CmdWlDumpAll(
    IN u16 argc, 
    IN u8* argv[]
);

#endif //#ifndef _CMD_WLAN_H_

