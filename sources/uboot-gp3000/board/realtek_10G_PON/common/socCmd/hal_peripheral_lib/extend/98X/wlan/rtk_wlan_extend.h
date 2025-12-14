
#ifndef _RTK_WLAN_EXTEND_H_
#define _RTK_WLAN_EXTEND_H_

#define WLAN0_WLBASE_ADDR                     0xF4440000

#if defined(CONFIG_CMD_RTK_WLAN_MP_8198F_8814A)
#define WLAN1_WLBASE_ADDR                     0xF1020000 //PCIE
#elif CONFIG_FPGA_SETTING
#define WLAN1_WLBASE_ADDR                     0xF4460000 //FPGA
#else
#define WLAN1_WLBASE_ADDR                     0xF4440000

#endif



#define RPTBufferStartHiAddr                0x660    // ReportBuffer base address: 0x18660000
#define macIDNumperGroup                    12       // 64/5=12
#define CRC5BitNum                          5       
#define CRC5StartAddr                       0xC00    // CRC5 start address in RPTbuffer: 0x18661C00   
#define CRC5GroupByteNum                    8
#define RWCtrlBit                           BIT15
#define CRCAcrossSHT                        30       // The six number of CRC val will across the 32bit, start at bit30
#define SecondBitSHT                        2       
#define CRC5ValidBit                        BIT29      
#define CRC5EndBit                          BIT28
#define MaxMacIDGroupNum                    11


//RPWM
#define 	PS_DPS				BIT(0)
#define 	PS_LCLK				(PS_DPS)
#define	PS_RF_OFF			BIT(1)
#define	PS_ALL_ON			BIT(2)
#define	PS_ST_ACTIVE		BIT(3)
#define	PS_LP				BIT(4)	// low performance
#define	PS_ACK				BIT(6)
#define	PS_TOGGLE			BIT(7)

#define TX_PAGE_SIZE_8198F    128
#define TX_PAGE_SIZE_8814A 128

#define SECURITY_CAM_ENTRY_NUM_8198F 132
#define SECURITY_CAM_ENTRY_SIZE    8 //unit:4 bytes, 8*4 bytes for one entry
#define BIT_SHIFT_HW_IDX    BIT_SHIFT_VOQ_HW_IDX //use VO to present
#define BIT_MASK_HW_IDX BIT_MASK_VOQ_HW_IDX //use VO to present
#define TEST_SUB_BEACON_SPACE 0x32
#define RXDESC_SIZE 24

/*------SWPS related----*/
#define SWPS_BITMAP_SIZE    64
#define SWPS_ONEMACID_REPORT_SIZE    16
#define SWPS_REPORT_DROPBIT_MASK    0xF
#define BIT_MASK_SWPS_PSFLAG 0x1
#define BIT_SHIFT_SWPS_PSFLAG 0x7
#define SWPS_GET_BIT_PSFLAG(x)   (((x)>> BIT_SHIFT_SWPS_PSFLAG)& BIT_MASK_SWPS_PSFLAG ) 

extern WLAN_DATA_ELEMENT WlanDataEle[2];

int
InitPCIE98F(
);

VOID
InterruptHandleExtend(
    IN  VOID    *Data  
);



RTK_STATUS 
InterruptInitExtend(
    u1Byte  interface_sel,
    IN  VOID    *Data  
);

RTK_STATUS
InitMBIDCAM_98F(
    IN  VOID    *pAdapter,
    IN  u1Byte  CAMsize
);

VOID
EnableWlanIP98F(
);

#endif  // #ifndef _RTK_WLAN_EXTEND_H_
