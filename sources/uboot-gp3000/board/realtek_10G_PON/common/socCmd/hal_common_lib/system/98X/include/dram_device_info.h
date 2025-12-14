#ifndef DRAM_DEVICE_INFO_H
#define DRAM_DEVICE_INFO_H
#include "basic_types.h"

typedef enum _DRAM_TYPE_ {
  DDR_1 = 1,
  DDR_2 = 2,
  DDR_3 = 3,
  DDR_4 = 4,
  SDR   = 8 
}DRAM_TYPE;

typedef enum _DRAM_PAGE_SIZE_ {
  PAGE_256B = 0,
  PAGE_512B = 1,
  PAGE_1K   = 2,
  PAGE_2K   = 3,
  PAGE_4K   = 4,
  PAGE_8K   = 5,
  PAGE_16K  = 6,
  PAGE_32K  = 7,
  PAGE_64K  = 8 
}DRAM_PAGE_SIZE;

typedef enum _DRAM_BANK_SIZE_ {
  BANK_2   = 0,
  BANK_4   = 1,
  BANK_8   = 2 
}DRAM_BANK_SIZE;

typedef enum _DRAM_DQ_WIDTH_ {
  DQ_16     = 0,
  DQ_32     = 1, 
  HALF_DQ32 = 2 
}DRAM_DQ_WIDTH;

typedef enum _MODE0_BST_LEN_ {
  BST_LEN_4   = 0,
  BST_LEN_FLY = 1,
  BST_LEN_8   = 2 
}MODE0_BST_LEN;

typedef enum _MODE0_BST_TYPE_ {
  SENQUENTIAL = 0,
  INTERLEAVE  = 1 
}MODE0_BST_TYPE;   

typedef enum _DFI_RATIO_TYPE_ {
  DFI_RATIO_1 = 0,  // DFI= 1:1, or SDR
  DFI_RATIO_2 = 1, 
  DFI_RATIO_4 = 2  
}DFI_RATIO_TYPE;   

typedef struct _DRAM_INFO_ {
  DRAM_TYPE         DeviceType;   
  DRAM_PAGE_SIZE    Page;   
  DRAM_BANK_SIZE    Bank;   
  DRAM_DQ_WIDTH     DqWidth;
}DRAM_INFO;

typedef struct _DRAM_MODE_REG_INFO_ {
  MODE0_BST_LEN     BstLen;
  MODE0_BST_TYPE    BstType;
  //enum mode0_cas         rd_cas;
  u32 Mode0Cas;
  u32 Mode0Wr;
  u32 Mode1DllEnN;
  u32 Mode1AllLat;
  u32 Mode2Cwl;
}DRAM_MODE_REG_INFO;

typedef struct _DRAM_TIMING_INFO_ {
  u32  TrfcPs;
  u32  TrefiPs;
  u32  WrMaxTck;
  u32  TrcdPs; 
  u32  TrpPs;
  u32  TrasPs;
  u32  TrrdTck;
  u32  TwrPs;
  u32  TwtrTck; 
  //u32  TrtpPs;
  u32  TmrdTck;
  u32  TrtpTck;
  u32  TccdTck;
  u32  TrcPs;
}DRAM_TIMING_INFO;


typedef struct _DRAM_DEVICE_INFO_ {
  DRAM_INFO          *Dev;
  DRAM_MODE_REG_INFO *ModeReg;
  DRAM_TIMING_INFO   *Timing;
  u32                DdrPeriodPs;
  DFI_RATIO_TYPE     *DfiRate; 
}DRAM_DEVICE_INFO;




#endif /* DRAM_DEVICE_INFO_H */

