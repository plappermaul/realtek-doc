/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#include "rtl8195a.h"
#include "hal_sdr_controller.h"
#include "rtl8195a_sdr.h"

C_CUT_ROM_RODATA_SECTION
DRAM_INFO SdrDramDev_rom = {
    DRAM_INFO_TYPE,
    DRAM_INFO_COL_ADDR_WTH,
    DRAM_INFO_BANK_SZ,
    DRAM_INFO_DQ_WTH
};


C_CUT_ROM_RODATA_SECTION
DRAM_MODE_REG_INFO SdrDramModeReg_rom = {
    BST_LEN_4,
    SENQUENTIAL,
    0x3, // Mode0Cas: 3
    0x0, // Mode0Wr
    0,   // Mode1DllEnN
    0,   // Mode1AllLat
    0    // Mode2Cwl
};

C_CUT_ROM_RODATA_SECTION
DRAM_TIMING_INFO SdrDramTiming_rom = {
    DRAM_TIMING_TRFC,         // TrfcPs;
    DRAM_TIMING_TREFI,        // TrefiPs;
    DRAM_TIMING_TWRMAXTCK,    // WrMaxTck;
    DRAM_TIMING_TRCD,         // TrcdPs;
    DRAM_TIMING_TRP,          // TrpPs;
    DRAM_TIMING_TRAS,         // TrasPs;
    DRAM_TIMING_TRRD,         // TrrdTck;
    DRAM_TIMING_TWR,          // TwrPs;
    DRAM_TIMING_TWTR,         // TwtrTck;
    //13090,      // TrtpPs;
    DRAM_TIMING_TMRD,         // TmrdTck;
    DRAM_TIMING_TRTP,         // TrtpTck;
    DRAM_TIMING_TCCD,         // TccdTck;
    DRAM_TIMING_TRC           // TrcPs;
};

C_CUT_ROM_RODATA_SECTION
DRAM_DEVICE_INFO SdrDramInfo_rom = {
    &SdrDramDev_rom,
    &SdrDramModeReg_rom,
    &SdrDramTiming_rom,
    DRAM_TIMING_TCK,
    DFI_RATIO_1
};

//#define FPGA
#define FPGA_TEMP
#define SDR_CLK_DLY_CTRL 0x40000300
#define MIN_RD_PIPE 0x0
#define MAX_RD_PIPE 0x7


#ifdef FPGA
#ifdef FPGA_TEMP
#define MAX_TAP_DLY 0xC
#else
#define MAX_TAP_DLY 0x7F
#define SPEC_MAX_TAP 0xFF
#endif
#else
#define MAX_TAP_DLY 99		// 0~99
#define SPEC_MAX_TAP 99
#define WINDOW_COMBIN		// combine window [0~a] and [b~99] (for asic mode)
#endif

#define TAP_DLY 0x1
#define REC_NUM 512


u32 SdrControllerInit_rom(DRAM_DEVICE_INFO *DramInfo);
VOID DramInit_rom(DRAM_DEVICE_INFO *DramInfo);
s32 MemTest_rom(VOID);
u32 SdrCalibration_rom(VOID);
u32 Sdr_Rand2_rom(VOID);

//3 Note: stack overfloat if the arrary is declared in the task
#if 0
C_CUT_ROM_DATA_SECTION
u32  AvaWds_rom[2][REC_NUM];

C_CUT_ROM_DATA_SECTION
unsigned int rand_x_rom = 123456789;
#else
// Since the SDRC RAM code has declared these memory and it always be there, so just use it
extern u32 AvaWds[2][REC_NUM];
extern unsigned int rand_x;
#define AvaWds_rom      AvaWds
#define rand_x_rom      rand_x
#endif

C_CUT_ROM_DATA_SECTION static unsigned int y = 362436;
C_CUT_ROM_DATA_SECTION static unsigned int z = 521288629;
C_CUT_ROM_DATA_SECTION static unsigned int c = 7654321;

C_CUT_ROM_TEXT_SECTION
u32
SdrControllerInit_rom(
    IN  DRAM_DEVICE_INFO *DramInfo
)
{
    DBG_SDR_INFO("SDR Controller Init\n");

    SRAM_MUX_CFG(0x2);

    SDR_CLK_SEL(SDR_CLOCK_SEL_VALUE);

    HAL_PERI_ON_WRITE32(REG_GPIO_PULL_CTRL4,0);

    ACTCK_SDR_CCTRL(ON);

    SLPCK_SDR_CCTRL(ON);

    PinCtrl(SDR, 0, ON);

    //MEM_CTRL_FCTRL(OFF);

    MEM_CTRL_FCTRL(ON);

    // sdr initialization
    DramInit_rom(DramInfo);

    // sdr calibration
    if(!SdrCalibration_rom()) {
        return 0;
    } else {
        return 1;
    }
}


C_CUT_ROM_TEXT_SECTION
VOID
DramInit_rom (
    IN  DRAM_DEVICE_INFO *DramInfo
)
{
    u32 CsBstLen = 0;            // 0:bst_4, 1:bst_8
    u32 CasWr = 0;//, CasWrT;       // cas write latency
    u32 CasRd = 0, CasRdT = 0, CrlSrt = 0;  // cas read latency
    u32 AddLat;
    u32 DramEmr2 = 0, DramMr0 = 0;
    u32 CrTwr, DramMaxWr, DramWr;
    u32 CrTrtw = 0, CrTrtwT = 0;
    u32 DrmaPeriod;
    DRAM_TYPE         DdrType;
    DRAM_DQ_WIDTH     DqWidth;
    DRAM_COLADDR_WTH    Page;
    u32 DfiRate;
    volatile struct ms_rxi310_portmap *ms_ctrl_0_map;
    ms_ctrl_0_map = (struct ms_rxi310_portmap*) SDR_CTRL_BASE;
    ms_ctrl_0_map = ms_ctrl_0_map;

    DfiRate = 1 << (u32) (DramInfo->DfiRate);
    DrmaPeriod = (DramInfo->DdrPeriodPs)*(DfiRate); // according DFI_RATE to setting

    // In PHY, write latency == 3
    DramMaxWr= (DramInfo->Timing->WrMaxTck)/(DfiRate) +1;
    DramWr = ((DramInfo->Timing->TwrPs) / DrmaPeriod)+1;
    CrTwr = ((DramInfo->Timing->TwrPs) / DrmaPeriod) + 3;

    if (CrTwr < DramMaxWr) {
        CrTwr = CrTwr;
    } else {
        CrTwr = DramMaxWr;
    }

    if ((DramInfo->Dev->DeviceType) == DRAM_DDR_2) {
        DdrType = DRAM_DDR_2;
        if (DramInfo->ModeReg->BstLen == BST_LEN_4) {
            CsBstLen = 0; //bst_4
            CrTrtwT = 2+2;    //4/2+2
            DramMr0 = 0x2;
        } else { // BST_LEN_8
            CsBstLen = 1; // bst_8
            CrTrtwT = 4+2;    // 8/2+2
            DramMr0 = 0x3;
        }
        CasRd = DramInfo->ModeReg->Mode0Cas;
        AddLat = DramInfo->ModeReg ->Mode1AllLat;
        CasWr = CasRd + AddLat -1;
        DramEmr2 = 0;

        DramMr0  =(((DramWr%6)-1)                  << (PCTL_MR_OP_BFO+1)) | // write_recovery
                  (0                                << PCTL_MR_OP_BFO    ) | // dll
                  (DramInfo->ModeReg->Mode0Cas << PCTL_MR_CAS_BFO   ) |
                  (DramInfo->ModeReg->BstType  << PCTL_MR_BT_BFO    ) |
                  DramMr0;
    } else if ((DramInfo->Dev->DeviceType) == DRAM_DDR_3) {
        DdrType = DRAM_DDR_3;
        if (DramInfo->ModeReg->BstLen == BST_LEN_4) {
            CsBstLen = 0; //bst_4
            DramMr0 = 0x2;
        } else { // BST_LEN_8
            CsBstLen = 1; // bst_8
            DramMr0 = 0x0;
        }

        CrlSrt = (DramInfo->ModeReg->Mode0Cas >> 1);
        if (((DramInfo->ModeReg->Mode0Cas) & 0x1) ) {
            CasRdT = CrlSrt+ 12;
        } else {
            CasRdT = CrlSrt+ 4;
        }

        if (DramInfo->ModeReg->Mode1AllLat == 1) { // CL-1
            AddLat = CasRd -1;
        } else if (DramInfo->ModeReg->Mode1AllLat == 2) { // CL-2
            AddLat = CasRd -2;
        } else {
            AddLat = 0;
        }

        CasRd = CasRdT + AddLat;

        CasWr =  DramInfo->ModeReg->Mode2Cwl + 5 + AddLat;

        DramEmr2 = DramInfo->ModeReg->Mode2Cwl << 3;

        if (DramWr == 16) {
            DramWr = 0;
        } else if (DramWr <= 9) { // 5< wr <= 9
            DramWr = DramWr - 4;
        } else {
            DramWr = (DramWr + 1) / 2;
        }

        DramMr0  =(DramWr                            << (PCTL_MR_OP_BFO+1) ) | // write_recovery
                  (0                                   << PCTL_MR_OP_BFO     ) | // dll
                  ((DramInfo->ModeReg->Mode0Cas >>1 )  << PCTL_MR_CAS_BFO    ) |
                  (DramInfo->ModeReg->BstType          << PCTL_MR_BT_BFO     ) |
                  ((DramInfo->ModeReg->Mode0Cas & 0x1) << 2                  ) |
                  DramMr0;

        CrTrtwT = (CasRdT + 6) - CasWr;

    }  // ddr2/ddr3
    else if ((DramInfo->Dev->DeviceType) == DRAM_SDR) {
        DdrType = DRAM_SDR;
        if (DramInfo->ModeReg->BstLen == BST_LEN_4) {
            DramMr0 = 2; // bst_4
            CsBstLen = 0; //bst_4
            CasRd = 0x2;
        } else { // BST_LEN_8
            DramMr0 = 3; // bst_8
            CsBstLen = 1; // bst_8
            CasRd  = 0x3;
        }

        CasWr = 0;

        DramMr0  =(CasRd                    << PCTL_MR_CAS_BFO) |
                  (DramInfo->ModeReg->BstType << PCTL_MR_BT_BFO ) |
                  DramMr0;

        CrTrtwT = 0; // tic: CasRd + rd_rtw + rd_pipe
    } // SDR


    // countting tRTW
    if ((CrTrtwT & 0x1)) {
        CrTrtw = (CrTrtwT+1) /(DfiRate);
    } else {
        CrTrtw = CrTrtwT /(DfiRate);
    }

    DqWidth = (DramInfo->Dev->DqWidth);
    Page = DramInfo->Dev->ColAddrWth +1; // DQ16 -> memory:byte_unit *2
    if (DqWidth == DRAM_DQ_32) { // paralle dq_16 => Page + 1
        Page = Page +1;
    }
#if 1

    // WRAP_MISC setting
    HAL_SDR_WRITE32(REG_SDR_MISC,(
                        (Page                 << WRAP_MISC_PAGE_SIZE_BFO) |
                        (DramInfo->Dev->Bank  << WRAP_MISC_BANK_SIZE_BFO) |
                        (CsBstLen             << WRAP_MISC_BST_SIZE_BFO ) |
                        (DqWidth              << WRAP_MISC_DDR_PARAL_BFO)
                    ));
    // PCTL setting
    HAL_SDR_WRITE32(REG_SDR_DCR,(
                        (0x2      << PCTL_DCR_DFI_RATE_BFO) |
                        (DqWidth << PCTL_DCR_DQ32_BFO    ) |
                        (DdrType << PCTL_DCR_DDR3_BFO    )
                    ));

    HAL_SDR_WRITE32(REG_SDR_IOCR,(
                        ((CasRd -4)/(DfiRate)  << PCTL_IOCR_TPHY_RD_EN_BFO ) |
                        (0                        << PCTL_IOCR_TPHY_WL_BFO   ) |
                        (((CasWr -3)/(DfiRate)) << PCTL_IOCR_TPHY_WD_BFO   ) |
                        (0                        << PCTL_IOCR_RD_PIPE_BFO   )
                    ));

    if ((DramInfo->Dev->DeviceType) != SDR) { // DDR2/3
        HAL_SDR_WRITE32(REG_SDR_EMR2,DramEmr2);
        HAL_SDR_WRITE32(REG_SDR_EMR1,(
                            (1               << 2 ) | //RTT
                            (1               << 1 ) | //D.I.C
                            (DramInfo->ModeReg->Mode1DllEnN )
                        ));
    } // DDR2/3

    HAL_SDR_WRITE32(REG_SDR_MR,DramMr0);

    HAL_SDR_WRITE32(REG_SDR_DRR, (
                        (0                                             << PCTL_DRR_REF_DIS_BFO) |
                        (9                                             << PCTL_DRR_REF_NUM_BFO) |
                        ((((DramInfo->Timing->TrefiPs)/DrmaPeriod)+1) << PCTL_DRR_TREF_BFO   ) |
                        ((((DramInfo->Timing->TrfcPs)/DrmaPeriod)+1)  << PCTL_DRR_TRFC_BFO   )
                    ));

    HAL_SDR_WRITE32(REG_SDR_TPR0,(
                        ((((DramInfo->Timing->TrtpTck)/DfiRate)+1)   << PCTL_TPR0_TRTP_BFO) |
                        (CrTwr                                        << PCTL_TPR0_TWR_BFO ) |
                        ((((DramInfo->Timing->TrasPs)/DrmaPeriod)+1) << PCTL_TPR0_TRAS_BFO) |
                        ((((DramInfo->Timing->TrpPs)/DrmaPeriod)+1)  << PCTL_TPR0_TRP_BFO )
                    ));

    HAL_SDR_WRITE32(REG_SDR_TPR1, (
                        (CrTrtw                                       << PCTL_TPR1_TRTW_BFO) |
                        ((((DramInfo->Timing->TwtrTck)/DfiRate)+3)   << PCTL_TPR1_TWTR_BFO) |
                        ((((DramInfo->Timing->TccdTck)/DfiRate)+1)   << PCTL_TPR1_TCCD_BFO) |
                        ((((DramInfo->Timing->TrcdPs)/DrmaPeriod)+1) << PCTL_TPR1_TRCD_BFO) |
                        ((((DramInfo->Timing->TrcPs)/DrmaPeriod)+1)  << PCTL_TPR1_TRC_BFO ) |
                        (((DramInfo->Timing->TrrdTck/DfiRate)+1)     << PCTL_TPR1_TRRD_BFO)
                    ));

    HAL_SDR_WRITE32(REG_SDR_TPR2, (
                        (DramInfo->Timing->TmrdTck << PCTL_TPR2_TMRD_BFO ) |
                        (0                         << PCTL_TPR2_INIT_NS_EN_BFO  ) |
                        (2                         << PCTL_TPR2_INIT_REF_NUM_BFO)
                    ));

    // set all_mode _idle
    HAL_SDR_WRITE32(REG_SDR_CSR,0x700);

    // start to init
    HAL_SDR_WRITE32(REG_SDR_CCR,0x01);
    while ((HAL_SDR_READ32(REG_SDR_CCR)& 0x1) == 0x0);

    // enter mem_mode
    HAL_SDR_WRITE32(REG_SDR_CSR,0x600);
#else
    // WRAP_MISC setting
    ms_ctrl_0_map->misc = //0x12;
        (
            (Page                   << WRAP_MISC_PAGE_SIZE_BFO) |
            (DramInfo->Dev->Bank << WRAP_MISC_BANK_SIZE_BFO) |
            (CsBstLen             << WRAP_MISC_BST_SIZE_BFO ) |
            (DqWidth               << WRAP_MISC_DDR_PARAL_BFO)
        );
    // PCTL setting
    ms_ctrl_0_map->dcr = //0x208;
        (
            (0x2      << PCTL_DCR_DFI_RATE_BFO) |
            (DqWidth << PCTL_DCR_DQ32_BFO    ) |
            (DdrType << PCTL_DCR_DDR3_BFO    )
        );

    ms_ctrl_0_map->iocr = (
                              ((CasRd -4)/(DfiRate)  << PCTL_IOCR_TPHY_RD_EN_BFO ) |
                              (0                        << PCTL_IOCR_TPHY_WL_BFO   ) |
                              (((CasWr -3)/(DfiRate)) << PCTL_IOCR_TPHY_WD_BFO   ) |
                              (0                        << PCTL_IOCR_RD_PIPE_BFO   )
                          );

    if ((DramInfo->Dev->DeviceType) != SDR) { // DDR2/3
        ms_ctrl_0_map->emr2 = DramEmr2;

        ms_ctrl_0_map->emr1 = (
                                  (1               << 2 ) | //RTT
                                  (1               << 1 ) | //D.I.C
                                  (DramInfo->ModeReg->Mode1DllEnN )
                              );
    } // DDR2/3

    ms_ctrl_0_map->mr = DramMr0;

    ms_ctrl_0_map->drr = (
                             (0                                                 << PCTL_DRR_REF_DIS_BFO) |
                             (9                                                 << PCTL_DRR_REF_NUM_BFO) |
                             ((((DramInfo->Timing->TrefiPs)/DrmaPeriod)+1)<< PCTL_DRR_TREF_BFO   ) |
                             ((((DramInfo->Timing->TrfcPs)/DrmaPeriod)+1) << PCTL_DRR_TRFC_BFO   )
                         );

    ms_ctrl_0_map->tpr0= (
                             ((((DramInfo->Timing->TrtpTck)/DfiRate)+1)   << PCTL_TPR0_TRTP_BFO) |
                             (CrTwr                                            << PCTL_TPR0_TWR_BFO ) |
                             ((((DramInfo->Timing->TrasPs)/DrmaPeriod)+1) << PCTL_TPR0_TRAS_BFO) |
                             ((((DramInfo->Timing->TrpPs)/DrmaPeriod)+1)  << PCTL_TPR0_TRP_BFO )
                         );

    ms_ctrl_0_map->tpr1= (
                             (CrTrtw                                           << PCTL_TPR1_TRTW_BFO) |
                             ((((DramInfo->Timing->TwtrTck)/DfiRate)+3)   << PCTL_TPR1_TWTR_BFO) |
                             ((((DramInfo->Timing->TccdTck)/DfiRate)+1)   << PCTL_TPR1_TCCD_BFO) |
                             ((((DramInfo->Timing->TrcdPs)/DrmaPeriod)+1) << PCTL_TPR1_TRCD_BFO) |
                             ((((DramInfo->Timing->TrcPs)/DrmaPeriod)+1)  << PCTL_TPR1_TRC_BFO ) |
                             (((DramInfo->Timing->TrrdTck/DfiRate)+1)     << PCTL_TPR1_TRRD_BFO)
                         );

    ms_ctrl_0_map->tpr2= (
                             (DramInfo->Timing->TmrdTck << PCTL_TPR2_TMRD_BFO ) |
                             (0                         << PCTL_TPR2_INIT_NS_EN_BFO  ) |
                             (2                         << PCTL_TPR2_INIT_REF_NUM_BFO)
                         );
    // set all_mode _idle
    ms_ctrl_0_map->csr = 0x700;

    // start to init
    ms_ctrl_0_map->ccr = 0x1;
    while (((ms_ctrl_0_map->ccr)& 0x1) == 0x0);

    // enter mem_mode
    ms_ctrl_0_map->csr= 0x600;
#endif
} // DramInit

//3
extern void *
_memset( void *s, int c, SIZE_T n );

C_CUT_ROM_TEXT_SECTION
u32
SdrCalibration_rom(
    VOID
)
{
#ifdef FPGA
#ifdef FPGA_TEMP
//    u32 Value32;
#endif
#else
//    u32 Value32;
#endif
    u32 RdPipe = 0, TapCnt = 0, Pass = 0, AvaWdsCnt = 0;
    u32 RdPipeCounter, RecNum[2], RecRdPipe[2];//, AvaWds_rom[2][REC_NUM];
    BOOL RdPipeFlag, PassFlag = 0, Result;
    u32 tempval32;

    Result = _FALSE;
    tempval32 = HAL_READ32(0x40000000,0x40);
    tempval32 |= BIT1;
    HAL_WRITE32(0x40000000,0x40,tempval32);

    rtl_memset((u8*)AvaWds_rom, 0, sizeof(u32)*REC_NUM*2);

    volatile struct ms_rxi310_portmap *ms_ctrl_0_map;
    ms_ctrl_0_map = (struct ms_rxi310_portmap*) SDR_CTRL_BASE;
    ms_ctrl_0_map = ms_ctrl_0_map;
    PassFlag = PassFlag;
    RdPipeCounter =0;

//    DBG_8195A("%d\n",__LINE__);

    for(RdPipe=MIN_RD_PIPE; RdPipe<=MAX_RD_PIPE; RdPipe++) {
//        ms_ctrl_0_map->iocr = (ms_ctrl_0_map->iocr & 0xff) | (RdPipe << PCTL_IOCR_RD_PIPE_BFO);
        HAL_SDR_WRITE32(REG_SDR_IOCR, ((HAL_SDR_READ32(REG_SDR_IOCR) & 0xff) | (RdPipe << PCTL_IOCR_RD_PIPE_BFO)));

        DBG_SDR_INFO("IOCR: 0x%x; Write: 0x%x\n",HAL_SDR_READ32(REG_SDR_IOCR), (RdPipe << PCTL_IOCR_RD_PIPE_BFO));
//        DBG_8195A("IOCR: 0x%x; Write: 0x%x\n",ms_ctrl_0_map->iocr, (RdPipe << PCTL_IOCR_RD_PIPE_BFO));

        RdPipeFlag = _FALSE;
        PassFlag = _FALSE;
        AvaWdsCnt = 0;

        for(TapCnt=0; TapCnt < (MAX_TAP_DLY+1); TapCnt++) {
            // Modify clk delay
#ifdef FPGA
#ifdef FPGA_TEMP
            SDR_DDL_FCTRL(TapCnt);
//            Value32 = (RD_DATA(SDR_CLK_DLY_CTRL) & 0xFF00FFFF);
//            Value32 = (Value32 | (TapCnt << 16));
//            WR_DATA(SDR_CLK_DLY_CTRL, Value32);
#else
            HAL_SDR_WRITE32(REG_SDR_DLY0, TapCnt);
//            ms_ctrl_0_map->phy_dly0 = TapCnt;
#endif
            DBG_SDR_INFO("DLY: 0x%x; Write: 0x%x\n",HAL_PERI_ON_READ32(REG_PESOC_MEM_CTRL), TapCnt);
#else
            SDR_DDL_FCTRL(TapCnt);
//            Value32 = (RD_DATA(SDR_CLK_DLY_CTRL) & 0xFF00FFFF);
//            Value32 = (Value32 | (TapCnt << 16));
//            WR_DATA(SDR_CLK_DLY_CTRL, Value32);
#endif

            rand_x_rom = 123456789;
            y = 362436;
            z = 521288629;
            c = 7654321;
            Pass = MemTest_rom();
            PassFlag = _FALSE;

            if(Pass==_TRUE) {   // PASS

                if (!RdPipeFlag) {
                    DBG_SDR_INFO("%d Time Pass\n", RdPipeCounter);
                    RdPipeCounter++;
                    RdPipeFlag = _TRUE;
                    RecRdPipe[RdPipeCounter - 1] = RdPipe;
                }

                AvaWds_rom[RdPipeCounter-1][AvaWdsCnt] = TapCnt;
                AvaWdsCnt++;

                RecNum[RdPipeCounter-1] = AvaWdsCnt;

                if((TapCnt+TAP_DLY)>=MAX_TAP_DLY) {
                    break;
                }

                PassFlag = _TRUE;

                DBG_SDR_INFO("Verify Pass => RdPipe:%d; TapCnt: %d\n", RdPipe, TapCnt);

            } else {    // FAIL
//                if(PassFlag==_TRUE) {
//                    break;
//                }
//                else {
                if (RdPipeCounter > 0) {
                    RdPipeCounter++;
                    if (RdPipeCounter < 3) {
                        RecNum[RdPipeCounter-1] = 0;
                        RecRdPipe[RdPipeCounter - 1] = RdPipe;
                    }
                    break;
                }
//                }
            }
        }


        if (RdPipeCounter > 2) {
            u8 BestRangeIndex, BestIndex;
            u32 i;

            DBG_SDR_INFO("Avaliable RdPipe 0\n");

            for (i=0; i<256; i++) {
                DBG_SDR_INFO("%d\n", AvaWds_rom[0][i]);
            }
            DBG_SDR_INFO("Avaliable RdPipe 1\n");
            for (i=0; i<256; i++) {
                DBG_SDR_INFO("%d\n", AvaWds_rom[1][i]);
            }

            DBG_SDR_INFO("Rec 0 => total counter %d; RdPipe:%d;\n", RecNum[0], RecRdPipe[0]);
            DBG_SDR_INFO("Rec 1 => total counter %d; RdPipe:%d;\n", RecNum[1], RecRdPipe[1]);

            BestRangeIndex = (RecNum[0] > RecNum[1]) ? 0 : 1;

            BestIndex = RecNum[BestRangeIndex]>>1;

            DBG_SDR_INFO("The Finial RdPipe: %d; TpCnt: 0x%x\n", RecRdPipe[BestRangeIndex], AvaWds_rom[BestRangeIndex][BestIndex]);

            // set RdPipe and tap_dly
//            ms_ctrl_0_map->iocr = (ms_ctrl_0_map->iocr & 0xff) | (RecRdPipe[BestRangeIndex] << PCTL_IOCR_RD_PIPE_BFO);
            HAL_SDR_WRITE32(REG_SDR_IOCR, ((HAL_SDR_READ32(REG_SDR_IOCR) & 0xff) | (RecRdPipe[BestRangeIndex] << PCTL_IOCR_RD_PIPE_BFO)));

#ifdef FPGA
#ifdef FPGA_TEMP
            SDR_DDL_FCTRL(AvaWds_rom[BestRangeIndex][BestIndex]);

//            Value32 = (RD_DATA(SDR_CLK_DLY_CTRL) & 0xFF00FFFF);
//            Value32 = Value32 | (AvaWds_rom[BestRangeIndex][BestIndex] << 16);
//            WR_DATA(SDR_CLK_DLY_CTRL, Value32);
#else
            HAL_SDR_WRITE32(REG_SDR_DLY0, AvaWds_rom[BestRangeIndex][BestIndex]);
//            ms_ctrl_0_map->phy_dly0 = AvaWds_rom[BestRangeIndex][BestIndex];
#endif
#else
            SDR_DDL_FCTRL(AvaWds_rom[BestRangeIndex][BestIndex]);
//            Value32 = (RD_DATA(SDR_CLK_DLY_CTRL) & 0xFF00FFFF);
//            Value32 = Value32 | (AvaWds_rom[BestRangeIndex][BestIndex] << 16);
//            WR_DATA(SDR_CLK_DLY_CTRL, Value32);
#endif
            Result = _TRUE;
            break;
        }
    }

    return Result;
} // SdrCalibration


C_CUT_ROM_TEXT_SECTION
VOID
ChangeRandSeed_rom(
    IN  u32 Seed
)
{
    rand_x_rom = Seed;
}

C_CUT_ROM_TEXT_SECTION
u32
Sdr_Rand2_rom(
    VOID
)
{
    unsigned long long t, a;

    a = 698769069;
    rand_x_rom = 69069 * rand_x_rom + 12345;
    y ^= (y << 13);
    y ^= (y >> 17);
    y ^= (y << 5);
    t = a * z + c;
    c = (t >> 32);
    z = t;

    return rand_x_rom + y + z;
}

C_CUT_ROM_TEXT_SECTION
s32
MemTest_rom(
    VOID
)
{
    u32 LoopIndex = 0;
    u32 Value32, Addr;
    for (LoopIndex = 0; LoopIndex<10000; LoopIndex++) {
        Value32 = Sdr_Rand2_rom();
        Addr = Sdr_Rand2_rom();
        Addr &= 0x1FFFFF;
        Addr &= (~0x3);

        HAL_SDRAM_WRITE32(Addr, Value32);

        if (HAL_SDRAM_READ32(Addr) != Value32) {
            DBG_SDR_ERR("Test %d: No match addr 0x%x => 0x%x != 0x%x\n",LoopIndex,
                        Addr, Value32, HAL_SDRAM_READ32(Addr));
            return _FALSE;
        } else {
            //            HAL_SDRAM_WRITE32(Addr, 0);
        }
    }
    return _TRUE;

} // MemTest



