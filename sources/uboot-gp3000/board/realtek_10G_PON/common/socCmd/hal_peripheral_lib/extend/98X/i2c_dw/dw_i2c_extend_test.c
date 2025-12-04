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
#include "dw_hal_i2c.h"
#include "rand.h"
#include "section_config.h"
//#include "dw_i2c_base_test.h"
#include "rtl_utility.h"
#include "peripheral.h"



//3 I2C Data
SRAM_BF_DATA_SECTION
HAL_DW_I2C_ADAPTER CodecI2CMaster;
SRAM_BF_DATA_SECTION
HAL_DW_I2C_DAT_ADAPTER CodecI2CMasterDat;
SRAM_BF_DATA_SECTION
HAL_DW_I2C_OP CodecHalI2COp;

#define CODEC_ADDR 0x34

static void LocalTimeDelay(void)
{
    int i;

    i=10000;
    while (i) {
        i--;
        asm volatile ("nop\n\t");
    }

}


void Write5651Codec(unsigned int codec_reg,unsigned int codec_value)
{
  char buf[3];
    PHAL_DW_I2C_DAT_ADAPTER pI2CDatAdapterMtr = &CodecI2CMasterDat;
    PHAL_DW_I2C_ADAPTER pI2CAdapterMtr = &CodecI2CMaster;
  
  buf[0]=(char)codec_reg;
  buf[1]=(char)(codec_value>>8);
  buf[2]=(char)codec_value&0xff;
  
  //Rtl_I2C_WriteReg(0x34,3,buf);
    pI2CDatAdapterMtr->RWDat = (u8 *)&buf[0];
    pI2CDatAdapterMtr->DatLen = 3;
    pI2CDatAdapterMtr->STP = 1;
    pI2CDatAdapterMtr->RWCmd = 0;//master write
    pI2CAdapterMtr->pI2CIrqOp->HalI2CDATWrite(pI2CDatAdapterMtr);

    LocalTimeDelay();
}



void Read5651Codec(unsigned int codec_reg,unsigned int *codec_value)
{
    char writeBuf[1];
    char readBuf[2];
    int i;
 
    PHAL_DW_I2C_DAT_ADAPTER pI2CDatAdapterMtr = &CodecI2CMasterDat;
    PHAL_DW_I2C_ADAPTER pI2CAdapterMtr = &CodecI2CMaster;

    writeBuf[0]=(char)codec_reg;

    //Rtl_I2C_WriteReg(0x34,1,buf);
    pI2CDatAdapterMtr->RWDat = (u8 *)&writeBuf[0];
    pI2CDatAdapterMtr->DatLen = 1;
    pI2CDatAdapterMtr->STP = 0;
    pI2CDatAdapterMtr->RWCmd = 0;//master write
    pI2CAdapterMtr->pI2CIrqOp->HalI2CDATWrite(pI2CDatAdapterMtr);
    
    //Rtl_I2C_ReadReg(0x34,2,readBuf);	
    pI2CDatAdapterMtr->RWDat = (u8 *)readBuf[0];
    pI2CDatAdapterMtr->DatLen = 2;
    pI2CDatAdapterMtr->STP = 1;
    pI2CDatAdapterMtr->RWCmd = 1;//master read
    pI2CAdapterMtr->pI2CIrqOp->HalI2CDATWrite(pI2CDatAdapterMtr);

    i=10000;
    while (i) {
        i--;
        asm volatile ("nop\n\t");
    }

    if (HAL_I2C_READ32(pI2CDatAdapterMtr->Idx,REG_DW_I2C_IC_STATUS) & BIT_CTRL_IC_STATUS_RFNE(1)) 
    {
        readBuf[0] = HalI2CDATRead(pI2CDatAdapterMtr);
    } else {
        i=10000;
        while (i) {
            i--;
            asm volatile ("nop\n\t");
        }
        if (HAL_I2C_READ32(pI2CDatAdapterMtr->Idx,REG_DW_I2C_IC_STATUS) & BIT_CTRL_IC_STATUS_RFNE(1)) 
        {
            readBuf[0] = HalI2CDATRead(pI2CDatAdapterMtr);
        } else {
            DBG_8195A_I2C("i2c codec reg read not ready.\n");
            readBuf[0] = 0xaa;
        }
    }
    if (HAL_I2C_READ32(pI2CDatAdapterMtr->Idx,REG_DW_I2C_IC_STATUS) & BIT_CTRL_IC_STATUS_RFNE(1)) 
    {
        readBuf[1] = HalI2CDATRead(pI2CDatAdapterMtr);
    } else {
        i=10000;
        while (i) {
            i--;
            asm volatile ("nop\n\t");
        }
        if (HAL_I2C_READ32(pI2CDatAdapterMtr->Idx,REG_DW_I2C_IC_STATUS) & BIT_CTRL_IC_STATUS_RFNE(1)) 
        {
            readBuf[1] = HalI2CDATRead(pI2CDatAdapterMtr);
        } else {
            DBG_8195A_I2C("i2c codec reg read not ready.\n");
            readBuf[1] = 0xaa;
        }
    }
    *codec_value=readBuf[0]<<8 | readBuf[1]; 


}

void Write5651CodecIndex(unsigned int codec_reg,unsigned int codec_value)
{
  Write5651Codec(0x6A,codec_reg);	
  Write5651Codec(0x6C,codec_value); 
}

void Read5651CodecIndex(unsigned int codec_reg,unsigned int *codec_value)
{
  Write5651Codec(0x6A,codec_reg);	
  Read5651Codec(0x6C,codec_value); 
}


void Dump_codec_reg()
{
  int i;
  unsigned int CodecValue;
  
  for(i=0;i<=0xff;i++)
  {
    Read5651Codec(i,&CodecValue);
    DBG_8195A_I2C("i2c codec reg%x=%x.\n", i, CodecValue);
  }		

}

void Dump_codec_index()
{
  int i;
  unsigned int CodecValue;
  
  for(i=0;i<=0xff;i++)
  {
   Read5651CodecIndex(i,&CodecValue);
    DBG_8195A_I2C("i2c codec priv%x=%x.\n", i, CodecValue);
  }		
}


VOID
CodecInit(
     IN  VOID
)
{

    PHAL_DW_I2C_OP pHalI2COp = (PHAL_DW_I2C_OP)&CodecHalI2COp;

    PHAL_DW_I2C_DAT_ADAPTER pI2CDatAdapterMtr = &CodecI2CMasterDat;

    PHAL_DW_I2C_ADAPTER pI2CAdapterMtr = &CodecI2CMaster;


    HalI2COpInit(pHalI2COp);
    
    DBG_8195A_I2C("i2c codec init\n");    

    //4 Master Setting
    pI2CDatAdapterMtr->Idx = I2C3_SEL;
    pI2CDatAdapterMtr->PinMux = I2C3_TO_B;
    pI2CDatAdapterMtr->ICClk = SYSTEM_CLK/1000000;
    pI2CDatAdapterMtr->SpdMd = FS_MODE;
    pI2CDatAdapterMtr->SpdHz = 100;
    pI2CDatAdapterMtr->Master = 1;
    pI2CDatAdapterMtr->AddrMd = ADDR_7BIT;

    pI2CDatAdapterMtr->ReSTR = 1;

    pI2CDatAdapterMtr->TSarAddr= CODEC_ADDR>>1;//slave addr
    pI2CDatAdapterMtr->ModulEn = 1;
    pI2CDatAdapterMtr->SDAHd = 1;//mini value:1
    pI2CDatAdapterMtr->Spec = 0;
    pI2CDatAdapterMtr->GC_STR = 0;//0 for using GC
    pI2CAdapterMtr->pI2CIrqOp = (PHAL_DW_I2C_OP)pHalI2COp;
    pI2CAdapterMtr->pI2CIrqDat = (PHAL_DW_I2C_DAT_ADAPTER)pI2CDatAdapterMtr;


        //4 To Disable SDR pinmux if I2C used
        PinCtrl(SDR,S0,OFF);

                ACTCK_I2C3_CCTRL(ON);
                PinCtrl(I2C3,pI2CDatAdapterMtr->PinMux,ON);
                I2C3_FCTRL(ON);

                        pI2CAdapterMtr->pI2CIrqOp->HalI2CInitMaster(pI2CDatAdapterMtr);
                        DBG_8195A_I2C_LVL(VERI_I2C_LVL,"    --->  I2C tar addr in master: %x\n",HAL_I2C_READ32(pI2CDatAdapterMtr->Idx,REG_DW_I2C_IC_TAR));
                        pI2CAdapterMtr->pI2CIrqOp->HalI2CSetCLK(pI2CDatAdapterMtr);
                        
                        DBG_8195A_I2C_LVL(VERI_I2C_LVL,"    --->  I2C Master Idx: %x\n",pI2CDatAdapterMtr->Idx);


                        //disable all intr first
                        pI2CDatAdapterMtr->IC_INRT_MSK = 0;
                        pI2CAdapterMtr->pI2CIrqOp->HalI2CEnableIntr(pI2CDatAdapterMtr);          


                        //enable master/slave after init.
                        pI2CDatAdapterMtr->ModulEn = 1;
                        pI2CAdapterMtr->pI2CIrqOp->HalI2CModuleEn(pI2CDatAdapterMtr);                       

                        //clear all pending intrs.
                        pI2CAdapterMtr->pI2CIrqOp->HalI2CClrAllIntr(pI2CDatAdapterMtr);
                        //to test master read/write
                        //I2CTestMrtWR(pI2CAdapterMtr,pI2CAdapterSlv,pI2CVeriPara);






    Dump_codec_reg();
    Dump_codec_index();

//#error "abcd"





}

#if 0
@start@
#rng63   #rve8fe  #rd50
#rng61   #rv9800  #rd50
#rng62   #rv0c00  #rd50
#rng73   #rv0000  #rd50
#rng2a   #rv1212  #rd50
#rng45   #rv2000  #rd50
#rng02   #rv4848  #rd50
#rng8e   #rv0019  #rd50
#rng8f   #rv3100  #rd50
#rng91   #rv0e00  #rd50
#rni3d   #rv3e00  #rd50
#rngfa   #rv0011  #rd50
#rng83   #rv0800  #rd50
#rng84   #rva000  #rd50
#rngfa   #rv0c11  #rd50
#rng64   #rv4010  #rd50
#rng65   #rv0c00  #rd50
#rng61   #rv9806  #rd50
#rng62   #rvcc00  #rd50
#rng3c   #rv004f  #rd50
#rng3e   #rv004f  #rd50
#rng27   #rv3820  #rd50
#rng77   #rv0000  #rd50
@end@

Note:"rng" indicates "register name general"
"rni" indicates "register name index"
"rnv" indicates "register name vodsp"
"rv" indicates "register value"
"rd" indicates "register delay"


#endif
// init alc5651 i2s1 interface
void InitAlc5651If1_Start()
{
    Write5651Codec(0x00,0x0021);
    Write5651Codec(0x63,0xE8FE);
    Write5651Codec(0x61,0x5800);
    Write5651Codec(0x62,0x0C00);
    Write5651Codec(0x73,0x0000);
    Write5651Codec(0x2A,0x4242);
    Write5651Codec(0x45,0x2000);
    Write5651Codec(0x02,0x4848);
    Write5651Codec(0x8E,0x0019);
    Write5651Codec(0x8F,0x3100);
    Write5651Codec(0x91,0x0E00);
    Write5651CodecIndex(0x3D,0x3E00);
    Write5651Codec(0xFA,0x0011);
    Write5651Codec(0x83,0x0800);
    Write5651Codec(0x84,0xA000);
    Write5651Codec(0xFA,0x0C11);
    Write5651Codec(0x64,0x4010);
    Write5651Codec(0x65,0x0C00);
    Write5651Codec(0x61,0x5806);
    Write5651Codec(0x62,0xCC00);
    Write5651Codec(0x3C,0x004F);
    Write5651Codec(0x3E,0x004F);
    Write5651Codec(0x27,0x3820);
    Write5651Codec(0x77,0x0000);


}
#if 0
@start@
#rng63   #rve8fe  #rd50
#rng61   #rv5800  #rd50
#rng62   #rv0c00  #rd50
#rng73   #rv0000  #rd50
#rng2a   #rv4242  #rd50
#rng45   #rv2000  #rd50
#rng02   #rv4848  #rd50
#rng8e   #rv0019  #rd50
#rng8f   #rv3100  #rd50
#rng91   #rv0e00  #rd50
#rni3d   #rv3e00  #rd50
#rngfa   #rv0011  #rd50
#rng83   #rv0800  #rd50
#rng84   #rva000  #rd50
#rngfa   #rv0c11  #rd50
#rng64   #rv4010  #rd50
#rng65   #rv0c00  #rd50
#rng61   #rv5806  #rd50
#rng62   #rvcc00  #rd50
#rng3c   #rv004f  #rd50
#rng3e   #rv004f  #rd50
#rng28   #rv3030  #rd50
#rng2f   #rv0080  #rd50
@end@

Note:"rng" indicates "register name general"
"rni" indicates "register name index"
"rnv" indicates "register name vodsp"
"rv" indicates "register value"
"rd" indicates "register delay"



#endif

// init alc5651 i2s2 interface
void InitAlc5651If2_Start()
{

    Write5651Codec(0x00,0x0021);
    Write5651Codec(0x63,0xE8FE);
    Write5651Codec(0x61,0x5800);
    Write5651Codec(0x62,0x0C00);
    Write5651Codec(0x73,0x0000);
    Write5651Codec(0x2A,0x4242);
    Write5651Codec(0x45,0x2000);
    Write5651Codec(0x02,0x4848);
    Write5651Codec(0x8E,0x0019);
    Write5651Codec(0x8F,0x3100);
    Write5651Codec(0x91,0x0E00);
    Write5651CodecIndex(0x3D,0x3E00);
    Write5651Codec(0xFA,0x0011);
    Write5651Codec(0x83,0x0800);
    Write5651Codec(0x84,0xA000);
    Write5651Codec(0xFA,0x0C11);
    Write5651Codec(0x64,0x4010);
    Write5651Codec(0x65,0x0C00);
    Write5651Codec(0x61,0x5806);
    Write5651Codec(0x62,0xCC00);
    Write5651Codec(0x3C,0x004F);
    Write5651Codec(0x3E,0x004F);
    Write5651Codec(0x28,0x3030);
    Write5651Codec(0x2F,0x0080);



}


