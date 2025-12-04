/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#include "platform_autoconf.h"
#include "basic_types.h"
#include "diag.h"
#include "hal_api.h"
#include "diag.h"
#include "hal_spi_flash.h"


#include "section_config.h"
#include "RTL8195A_Spi_Flash_Bit.h"
#include "RTL8195A_Spi_Flash_Reg.h"


#define SPI_CTRL_BASE 0x1FFEF000
#define SPI_DLY_CTRL_ADDR 0x40000300	// [7:0]
#define MIN_BAUDRATE  0x01
#define MAX_BAUDRATE  0x04
#define MAX_AUTOLEN   0x14
#define MAX_DLYLINE   99
#define GOLD_ID_NO    0xC224




#define WR_DATA(addr, data)    (*((volatile u32*)(addr)) = (data))      
#define RD_DATA(addr)          (*((volatile u32*)(addr)))

BOOLEAN SpicFlashInitRtl8195A(u8 SpicBitMode);
u32 SpicCalibrationRtl8195A(u8 SpicBitMode, u32 DefRdDummyCycle);          // spi-flash calibration
VOID SpicConfigAutoModeRtl8195A(u8 SpicBitMode);      // config spi-flash controller to auto mode


_LONG_CALL_
extern VOID SpicInitRtl8195A(u8 InitBaudRate, u8 SpicBitMode);            // spi-flash controller initialization

_LONG_CALL_
extern VOID SpicRxCmdRtl8195A(u8);         // recieve command

_LONG_CALL_
extern VOID SpicSetFlashStatusRtl8195A(u32 data, SPIC_INIT_PARA SpicInitPara);       // WRSR, write spi-flash status register

_LONG_CALL_
extern VOID SpicWaitBusyDoneRtl8195A(VOID);    // wait sr[0] = 0, wait transmission done

_LONG_CALL_
extern VOID SpicWaitWipDoneRtl8195A(SPIC_INIT_PARA SpicInitPara);     // wait spi-flash status register[0] = 0

_LONG_CALL_
extern VOID SpicEraseFlashRtl8195A(VOID);      // CE, flash chip erase

_LONG_CALL_
extern u32 SpicCmpDataForCalibrationRtl8195A(void);            // compare read_data and golden_data
#ifdef CONFIG_FPGA 
_LONG_CALL_
extern VOID SpicProgFlashForCalibrationRtl8195A(SPIC_INIT_PARA SpicInitPara);         // program spi-flash
#endif
_LONG_CALL_
extern VOID SpicLoadInitParaFromClockRtl8195A(u8 CpuClkMode, u8 BaudRate, PSPIC_INIT_PARA pSpicInitPara); 


struct ava_window {
    u16 baud_rate;
    u16 auto_length;
    u32 dly_line_sp;
    u32 dly_line_ep;
};

#ifdef CONFIG_FPGA
HAL_ROM_DATA_SECTION
SPIC_INIT_PARA FPGASpicInitPara = {1,1,0,0};
#endif

#ifdef CONFIG_SPIC_VERIFY
extern u32 Rand (VOID);

VOID 
SpiFlashTestApp(
    IN  VOID *Data
)
{
    u32 *Cmd =(u32*)Data;
    u32 Loop, LoopIndex, Value32, AddrIndex, SpicBitMode;
    BOOLEAN VerifyStatus = SUCCESS;
    SPIC_INIT_PARA SpicInitPara;

#ifdef CONFIG_FPGA
    SpicInitPara.BaudRate = FPGASpicInitPara.BaudRate;
    SpicInitPara.RdDummyCyle = FPGASpicInitPara.RdDummyCyle;
    SpicInitPara.DelayLine = FPGASpicInitPara.DelayLine;
#else
    u8 CpuClk;
    u8  Value8, InitBaudRate;

    // Load Efuse Setting
    Value8 = ((HAL_READ32(SYSTEM_CTRL_BASE, REG_EFUSE_SYSCFG6) & 0xFF000000)
                >> 24);

    InitBaudRate = ((Value8 & 0xC)>>2);

    // Make sure InitBaudRate != 0
    if (!InitBaudRate) {
        InitBaudRate +=1;
    }
    
    CpuClk = ((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYSON_CLK_CTRL1) & (0x70)) >> 4);
    SpicLoadInitParaFromClockRtl8195A(CpuClk, InitBaudRate, &SpicInitPara);
#endif

    switch (Cmd[0]) {
        case 1:
                SpicBitMode = Cmd[1];
                DBG_8195A("Initial Spi Flash Controller\n");
                FLASH_FCTRL(ON);
                ACTCK_FLASH_CCTRL(ON);
                SLPCK_FLASH_CCTRL(ON);
                PinCtrl(SPI_FLASH,S0,ON);

                switch(SpicBitMode) {
                    case SpicOneBitMode:
                        DBG_8195A("Initial Spic One bit mode\n");
                        SpicInitRtl8195A(SpicInitPara.BaudRate, SpicOneBitMode);
                        break;
                    case SpicDualBitMode:
                        DBG_8195A("Initial Spic Two bit mode\n");
                        SpicInitRtl8195A(SpicInitPara.BaudRate, SpicDualBitMode);
                        break;
                    case SpicQuadBitMode:
                        DBG_8195A("Initial Spic Four bit mode\n");
                        SpicInitRtl8195A(SpicInitPara.BaudRate, SpicQuadBitMode);
                        break;
                }

                if (!SpicFlashInitRtl8195A(SpicBitMode))
                    DBG_8195A("SPI Init Fail!!!!!!\n");
                else
                    DBG_8195A("SPI Init SUCCESS\n");

            break;
        case 2:
            DBG_8195A("Erase Falsh Start\n");
            SpicEraseFlashRtl8195A();
            DBG_8195A("Erase Falsh End\n");

            break;

        case 3:
                Loop = Cmd[1];
                DBG_8195A("Verify SDR: Loop=%d\n",Loop);
                for (LoopIndex=0; LoopIndex < Loop; LoopIndex++) {

                    Value32 = Rand();

                    for (AddrIndex=0; AddrIndex<0x100000;AddrIndex= AddrIndex+4) {
                        HAL_WRITE32(SPI_FLASH_BASE, AddrIndex, Value32);
                        DBG_8195A("SDR: Loop=%d; AddrIndex:0x%x\n",Loop,AddrIndex);

                        // Disable SPI_FLASH
//                        HAL_SPI_WRITE32(REG_SPIC_SSIENR, 0);

                        // wait spic busy done
                        SpicWaitBusyDoneRtl8195A();
                        // wait flash busy done (wip=0)
                        SpicWaitWipDoneRtl8195A(SpicInitPara);

                        // Enable SPI_FLASH
//                        HAL_SPI_WRITE32(REG_SPIC_SSIENR, BIT_SPIC_EN);

                        if (Value32 != HAL_READ32(SPI_FLASH_BASE, AddrIndex)) {
                            DBG_8195A("Write addr: 0x%x Date:0x%x => Read Data:0x%x Error\n"
                                        ,AddrIndex
                                        ,Value32
                                        ,HAL_READ32(SPI_FLASH_BASE, AddrIndex));
                            VerifyStatus = FAIL;
                            break;
                        }
                        Value32++;
                    }
                    
                    if (!VerifyStatus) {
                        break;    
                    }
                    DBG_8195A("Flash Write/Read Compare Success %d\n", LoopIndex);      
                }            
            break;
        case 4:
            
                for (AddrIndex=0; AddrIndex<0x800;AddrIndex = AddrIndex+4) {
                    HAL_WRITE32(SPI_FLASH_BASE, AddrIndex, 0x5A5A5A5A);
                    // wait spic busy done
                    SpicWaitBusyDoneRtl8195A();
                    // wait flash busy done (wip=0)
                    SpicWaitWipDoneRtl8195A(SpicInitPara);
                }                
            break;
        default:
            break;
    }
    
}
#endif

HAL_RAM_TEXT_SECTION
BOOLEAN
SpicFlashInitRtl8195A(
    IN  u8 SpicBitMode
)
{
    u32 DefRdDummyCycle = 0;
    SPIC_INIT_PARA SpicInitPara;

#ifdef CONFIG_FPGA
    SpicInitPara.BaudRate = FPGASpicInitPara.BaudRate;
    SpicInitPara.RdDummyCyle = FPGASpicInitPara.RdDummyCyle;
    SpicInitPara.DelayLine = FPGASpicInitPara.DelayLine;
#else
    u8 CpuClk;
    u8  Value8, InitBaudRate;

    SpicInitPara.BaudRate = 0;
    SpicInitPara.DelayLine = 0;
    SpicInitPara.RdDummyCyle = 0;
    SpicInitPara.Rsvd = 0;

    // Load Efuse Setting
    Value8 = ((HAL_READ32(SYSTEM_CTRL_BASE, REG_SOC_EFUSE_SYSCFG6) & 0xFF000000)
                >> 24);

    InitBaudRate = ((Value8 & 0xC)>>2);

    // Make sure InitBaudRate != 0
    if (!InitBaudRate) {
        InitBaudRate +=1;
    }
    
    CpuClk = ((HAL_READ32(SYSTEM_CTRL_BASE, REG_SOC_SYSON_CLK_CTRL1) & (0x70)) >> 4);
    SpicLoadInitParaFromClockRtl8195A(CpuClk, InitBaudRate, &SpicInitPara);
#endif


#ifdef CONFIG_FPGA
    DBG_8195A("Erase Falsh Start\n");
    SpicEraseFlashRtl8195A();
    DBG_8195A("Erase Falsh End\n");
#endif

    switch (SpicBitMode) {
        case SpicOneBitMode:
//            DBG_8195A("Initial Spic One bit mode\n");
            // wait for flash busy done
            SpicWaitWipDoneRtl8195A(SpicInitPara);

            //3 MXIC spec
            DefRdDummyCycle = 0;
            break;
        case SpicDualBitMode:
//            DBG_8195A("Initial Spic Two bit mode\n");
#ifdef CONFIG_FPGA            
            // program golden_data to golden_address and store golden_data in sram
            SpicProgFlashForCalibrationRtl8195A(SpicInitPara);
#endif            
            // set auto mode
            SpicConfigAutoModeRtl8195A(SpicBitMode);

            //3 MXIC spec
            #if FLASH_RD_2IO_EN
                DefRdDummyCycle = FLASH_DM_CYCLE_2IO;
            #endif
            #if FLASH_RD_2O_EN
                DefRdDummyCycle = FLASH_DM_CYCLE_2O;
            #endif
            
            break;
        case SpicQuadBitMode:
//            DBG_8195A("Initial Spic Four bit mode\n");
#ifdef CONFIG_FPGA   
            // program golden_data to golden_address and store golden_data in sram
            SpicProgFlashForCalibrationRtl8195A(SpicInitPara);
#endif
            // set auto mode
            SpicConfigAutoModeRtl8195A(SpicBitMode);

            // set 4bit-mode
            SpicSetFlashStatusRtl8195A(0x40, SpicInitPara);            

            //3 MXIC spec
            #if FLASH_RD_4IO_EN
                DefRdDummyCycle = FLASH_DM_CYCLE_4IO;
            #endif
            #if FLASH_RD_4O_EN
                DefRdDummyCycle = FLASH_DM_CYCLE_4O;
            #endif

            break;
        default:
            DBG_8195A("No Support SPI Mode!!!!!!!!\n");
            break;
            
    }

    // calibration
//    DBG_8195A("SPI calibration\n");

    if (!SpicCalibrationRtl8195A(SpicBitMode, DefRdDummyCycle)) {

        DBG_8195A("SPI calibration fail and recover one bit mode\n");
        SpicInitRtl8195A(SpicInitPara.BaudRate, SpicOneBitMode);
        SpicConfigAutoModeRtl8195A(SpicOneBitMode);

        return _FALSE;
    }


    return _TRUE; 
}



HAL_RAM_TEXT_SECTION
u32 
SpicCalibrationRtl8195A
(
    IN  u8 SpicBitMode,
    IN  u32 DefRdDummyCycle
) 
{

    u32 rd_data, id_no, baudr, autolen, dly_line;
    u32 total_ava_wds=0;
    u32 tmp_str_pt, tmp_end_pt, pass, last_pass;
    struct ava_window max_wd;

    max_wd.auto_length = 0;
    max_wd.baud_rate = 0;
    max_wd.dly_line_ep = 0;
    max_wd.dly_line_sp = 0;
//    RtlMemset((u8*)&max_wd, 0, sizeof( struct ava_window));
    
    //Spic == one bit mode
    if (SpicOneBitMode == SpicBitMode) {
        // Disable SPI_FLASH User Mode
        HAL_SPI_WRITE32(REG_SPIC_SSIENR, 0);
        HAL_SPI_WRITE32(REG_SPIC_CTRLR1, BIT_NDF(3));
        // Enable SPI_FLASH  User Mode
        HAL_SPI_WRITE32(REG_SPIC_SSIENR, BIT_SPIC_EN);
    }
    
    for(baudr=MIN_BAUDRATE; baudr < (MAX_BAUDRATE+1); baudr++) {
        // Disable SPI_FLASH User Mode
        HAL_SPI_WRITE32(REG_SPIC_SSIENR, 0);
        HAL_SPI_WRITE32(REG_SPIC_BAUDR, BIT_SCKDV(baudr));
        // Enable SPI_FLASH  User Mode
        HAL_SPI_WRITE32(REG_SPIC_SSIENR, BIT_SPIC_EN);
//        DBG_8195A("(0x14)Baudr: 0x%x\n",HAL_SPI_READ32(REG_SPIC_BAUDR));
          
	    for(autolen=DefRdDummyCycle*2*baudr; autolen<(DefRdDummyCycle*2*baudr+MAX_AUTOLEN); autolen++) {
            // Disable SPI_FLASH User Mode
            HAL_SPI_WRITE32(REG_SPIC_SSIENR, 0);
            rd_data = HAL_SPI_READ32(REG_SPIC_AUTO_LENGTH);
    	    rd_data = (rd_data & 0xFFFF0000) | (0x0000FFFF & autolen);
            HAL_SPI_WRITE32(REG_SPIC_AUTO_LENGTH, rd_data);
            // Enable SPI_FLASH  User Mode
            HAL_SPI_WRITE32(REG_SPIC_SSIENR, BIT_SPIC_EN);
//            DBG_8195A("Auto length: 0x%x\n",autolen);            
//            DBG_8195A("(0x11C) Auto address length register: 0x%x\n",HAL_SPI_READ32(REG_SPIC_AUTO_LENGTH));            
    	    tmp_str_pt = MAX_DLYLINE;
    	    tmp_end_pt = 0;
    	    last_pass = 0;

            for(dly_line=0; dly_line<=MAX_DLYLINE; dly_line++) {
        		rd_data = RD_DATA(SPI_DLY_CTRL_ADDR);
        		rd_data = (rd_data & 0xFFFFFF00) | (dly_line & 0x000000FF);
        		WR_DATA(SPI_DLY_CTRL_ADDR, rd_data);
                
                
//                DBG_8195A("SPI_DLY_CTRL_ADDR: 0x%x\n",RD_DATA(SPI_DLY_CTRL_ADDR));

                //Spic == one bit mode
                if (SpicOneBitMode == SpicBitMode) {

            		SpicRxCmdRtl8195A(0x9F);
                    rd_data = HAL_SPI_READ16(REG_SPIC_DR0);
            		id_no = ((rd_data & 0x000000FF) << 16) | (rd_data & 0x0000FF00);
                    rd_data = HAL_SPI_READ8(REG_SPIC_DR0);
            		id_no = (rd_data & 0x000000FF) | id_no;
            		id_no = (id_no >> 8) & 0x0000FFFF;
                    if (id_no==GOLD_ID_NO) 
                        pass = 1;
                    else 
                        pass = 0;

                }
                else {
                    pass = SpicCmpDataForCalibrationRtl8195A();
                }
                
        		if(pass) {	// PASS
        		    if(last_pass==0) {
        			    tmp_str_pt = dly_line;
        		        total_ava_wds++;
        		    }
                    
                    if(dly_line==MAX_DLYLINE) {
                        
                        tmp_end_pt = dly_line;

                        if(total_ava_wds==1) {
                            max_wd.baud_rate = baudr;
                            max_wd.auto_length = autolen;
                            max_wd.dly_line_sp = tmp_str_pt;
                            max_wd.dly_line_ep = tmp_end_pt;
                        }
                        else {
                            if((tmp_end_pt-tmp_str_pt)>(max_wd.dly_line_ep-max_wd.dly_line_sp)) {
                                max_wd.baud_rate = baudr;
                                max_wd.auto_length = autolen;
                                max_wd.dly_line_sp = tmp_str_pt;
                                max_wd.dly_line_ep = tmp_end_pt;
                            }
                        }
                    }
        		    last_pass = 1;
        		}
        		else {			// FAIL
        		    if(last_pass==1) {
            			tmp_end_pt = dly_line;
            			if(total_ava_wds == 1) {
            			    max_wd.baud_rate = baudr;
            			    max_wd.auto_length = autolen;
            			    max_wd.dly_line_sp = tmp_str_pt;
            			    max_wd.dly_line_ep = tmp_end_pt;
            			}
            			else {
            			    if((tmp_end_pt-tmp_str_pt)>(max_wd.dly_line_ep-max_wd.dly_line_sp)) {
                				max_wd.baud_rate = baudr;
                				max_wd.auto_length = autolen;
                				max_wd.dly_line_sp = tmp_str_pt;
                				max_wd.dly_line_ep = tmp_end_pt;
            			    }
            			}
        		    }
        		    last_pass = 0;
        		}
    	    }
//            DBG_8195A("total wds: %d\n",total_ava_wds);
//            DBG_8195A("Baud:%x; auto_length:%x; Delay start:%x; Delay end:%x\n",max_wd.baud_rate, max_wd.auto_length,max_wd.dly_line_sp, max_wd.dly_line_ep);
	    }
        if (total_ava_wds) {
//            DBG_8195A("Find the avaiable window\n");
            break;
        }
    }


    if(total_ava_wds==0) {
	    return 0;
    }
    else {
	// set baudr, auto_length, and delay_line
//        DBG_8195A("Baud:%x; auto_length:%x; Delay start:%x; Delay end:%x\n",max_wd.baud_rate, max_wd.auto_length,max_wd.dly_line_sp, max_wd.dly_line_ep);
        // Disable SPI_FLASH User Mode
        HAL_SPI_WRITE32(REG_SPIC_SSIENR, 0);
        HAL_SPI_WRITE32(REG_SPIC_BAUDR, (max_wd.baud_rate & 0x00000FFF));
        rd_data = HAL_SPI_READ32(REG_SPIC_AUTO_LENGTH);
    	rd_data = (rd_data & 0xFFFF0000) | (max_wd.auto_length & 0x0000FFFF);
        HAL_SPI_WRITE32(REG_SPIC_AUTO_LENGTH, rd_data);
    	rd_data = ((max_wd.dly_line_sp + max_wd.dly_line_ep) >> 1);
    	WR_DATA(SPI_DLY_CTRL_ADDR, ((RD_DATA(SPI_DLY_CTRL_ADDR) & 0xFFFFFF00) | (rd_data & 0x000000FF)));
        // Enable SPI_FLASH  User Mode
        HAL_SPI_WRITE32(REG_SPIC_SSIENR, BIT_SPIC_EN);
	return 1;
    }

}

HAL_RAM_TEXT_SECTION
VOID 
SpicConfigAutoModeRtl8195A
(
    IN  u8 SpicBitMode
) 
{


    // Disable SPI_FLASH User Mode
    HAL_SPI_WRITE32(REG_SPIC_SSIENR, 0);

    if (SpicOneBitMode == SpicBitMode) {

        // set write cmd (ppiix4: 0x38)
        HAL_SPI_WRITE32(REG_SPIC_WRITE_QUAD_ADDR_DATA, 0x38);
        
        // set read cmd (readiox4: 0xEB)
        HAL_SPI_WRITE32(REG_SPIC_READ_QUAD_ADDR_DATA, 0xEB);

        HAL_SPI_WRITE32(REG_SPIC_VALID_CMD, 
                (HAL_SPI_READ32(REG_SPIC_VALID_CMD) & (~(
                BIT_WR_QUAD_II |
                BIT_WR_QUAD_I  |
                BIT_WR_DUAL_II |
                BIT_WR_DUAL_I  |
                BIT_RD_QUAD_IO |
                BIT_RD_QUAD_O  |
                BIT_RD_DUAL_IO |
                BIT_RD_DUAL_I))));//Disable all the four and two bit commands.
    }


    if (SpicDualBitMode == SpicBitMode) {
        #if FLASH_RD_2IO_EN
            HAL_SPI_WRITE32(REG_SPIC_READ_DUAL_ADDR_DATA, FLASH_CMD_2READ);
        #endif
        
        #if FLASH_RD_2O_EN
            HAL_SPI_WRITE32(REG_SPIC_READ_DUAL_DATA, FLASH_CMD_DREAD);
        #endif

        HAL_SPI_WRITE32(REG_SPIC_VALID_CMD, 
                (HAL_SPI_READ32(REG_SPIC_VALID_CMD)|(FLASH_VLD_DUAL_CMDS)));

    }

    if (SpicQuadBitMode == SpicBitMode) {
        #if FLASH_WR_4IO_EN
            HAL_SPI_WRITE32(REG_SPIC_WRITE_QUAD_ADDR_DATA, FLASH_CMD_4PP);
        #endif
        
        #if FLASH_RD_4IO_EN
            HAL_SPI_WRITE32(REG_SPIC_READ_QUAD_ADDR_DATA, FLASH_CMD_4READ);
        #endif

        #if FLASH_RD_4O_EN
            HAL_SPI_WRITE32(REG_SPIC_READ_QUAD_DATA, FLASH_CMD_QREAD);
        #endif
        
        HAL_SPI_WRITE32(REG_SPIC_VALID_CMD, 
                (HAL_SPI_READ32(REG_SPIC_VALID_CMD)|FLASH_VLD_QUAD_CMDS));
    }

    
}


