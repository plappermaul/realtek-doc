
/*
 * Include Files
 */
#include <linux/sched.h>   /* for schedule_timeout() */
#include <linux/kernel.h>  /* Needed for KERN_ALERT */
#include <linux/delay.h>
#include <asm-generic/div64.h>

#include <rtk/gpio.h>
#include <rtk/ldd.h>
#include <rtk/switch.h>
#if defined(CONFIG_COMMON_RT_API)
#include <rtk/rt/rt_i2c.h>
#endif

#include "include/common.h"
#include "include/init.h"
//#include "include/rtl8290b_reg_definition.h"
//#include "include/rtl8290b_api.h"
//#include "include/ddmi.h"

#include <ioal/mem32.h>
#include <hal/chipdef/allreg.h>

#include "include/rtl8291_reg_acc.h"
#include "include/rtl8291_api.h"
#include "include/rtl8291_reg_definition.h"
#include "include/rtl8291_init.h"
#include "include/ddmi.h"

#define TEMPERATURE_NUM                                  14

extern uint8 I2C_PORT;

extern func_i2c_write _ldd_i2c_write;
extern func_i2c_read _ldd_i2c_read;

extern rtl8291_param_t ldd_param[LDD_DEVICE_NUM];

extern uint8 flash_data[LDD_FLASH_DATA_SIZE];

int _8291_debug_flag(uint32 devId)
{
    uint32 value;
    
    rtl8291_getReg(devId, RTL8291_A0_HIGH_REG7F_ADDR, &value);
    if (value == 0xA5)
        return 1;

    return 0;
}

int rtl8291_chipInfo_get(uint32 devId, uint32 *rtl_num, uint32 *ver)
{
    uint32 value, value2;
    value = 0;
    value2 = 0;
    //RTL NUM
    rtl8291_getReg(devId, RTL8291_REG_RTL_NUM_HB_ADDR, &value);     
    rtl8291_getReg(devId, RTL8291_REG_RTL_NUM_LB_ADDR, &value2);      
    *rtl_num  = (value<<8)|(value2&0xFF);
    //RTL Version
    rtl8291_getReg(devId, RTL8291_REG_RTL_VID_ADDR, &value);     
    rtl8291_getReg(devId, RTL8291_REG_RTL_NUM_LB_ADDR, &value2);      
    *ver  = value<<8;
    
    return SUCCESS;    
}

int rtl8291_sdadc_code_get(uint32 devId, uint32 sdadc_type, uint32 *code_data)
{
    uint32 regData1, regData2, regData3;
    uint32 temp32;
    int    dbg_en;

    dbg_en = _8291_debug_flag(devId);

    //rtl8291_setRegBit(devId, RTL8291_WB_REG4A_ADDR, RTL8291_WB_REG4A_REG_APCDIG_IMPD_READ_DISABLE_OFFSET, 0);
    
    temp32 = regData1 = regData2 = regData3 = 0;
    switch (sdadc_type)
    {
        case RTL8291_CODE_TEM:
            rtl8291_getReg(devId, RTL8291_CODE_TEM_H_ADDR, &regData1);
            rtl8291_getReg(devId, RTL8291_CODE_TEM_M_ADDR, &regData2);
            rtl8291_getReg(devId, RTL8291_CODE_TEM_L_ADDR, &regData3);
            temp32 =((regData1 & 0xff) << 11) | ((regData2 & 0xff) << 3) | ((regData3&0xE0)>>5);
            //printk(KERN_EMERG "rtl8291_sdadc_code_get: regData1 = 0x%x regData2 = 0x%x regData3 = 0x%x\n", regData1, regData2, regData3);
            *code_data = temp32;
            break;

        case RTL8291_CODE_CALV:
            rtl8291_getReg(devId, RTL8291_CODE_CALV_H_ADDR, &regData1);
            rtl8291_getReg(devId, RTL8291_CODE_CALV_M_ADDR, &regData2);
            rtl8291_getReg(devId, RTL8291_CODE_CALV_L_ADDR, &regData3);
            temp32 =((regData1 & 0xff) << 14) | ((regData2 & 0xff) << 6) | (regData3>>2);
            *code_data = temp32;
            //printk(KERN_EMERG "RTL8291_CODE_CALV: regData1 = 0x%x regData2 = 0x%x regData3 = 0x%x\n", regData1, regData2, regData3);        
            break;
            
        case RTL8291_CODE_PS:
            rtl8291_getReg(devId, RTL8291_CODE_PS_H_ADDR, &regData1);
            rtl8291_getReg(devId, RTL8291_CODE_PS_M_ADDR, &regData2);
            rtl8291_getReg(devId, RTL8291_CODE_PS_L_ADDR, &regData3);
            temp32 =((regData1 & 0xff) << 14) | ((regData2 & 0xff) << 6) | (regData3>>2);
            *code_data = temp32;
            //printk(KERN_EMERG "RTL8291_CODE_PS: regData1 = 0x%x regData2 = 0x%x regData3 = 0x%x\n", regData1, regData2, regData3);        
            break;
            
        case RTL8291_CODE_TXAPC:
            rtl8291_getReg(devId, RTL8291_CODE_TXAPC_H_ADDR, &regData1);
            rtl8291_getReg(devId, RTL8291_CODE_TXAPC_M_ADDR, &regData2);
            rtl8291_getReg(devId, RTL8291_CODE_TXAPC_L_ADDR, &regData3);
            temp32 =((regData1 & 0xff) << 14) | ((regData2 & 0xff) << 6) | (regData3>>2);
            *code_data = temp32;
            if (dbg_en==1)
                printk(KERN_EMERG "RTL8291_CODE_TXAPC: regData1 = 0x%06x regData2 = 0x%06x regData3 = 0x%06x code_data = 0x%06x\n", regData1, regData2, regData3, temp32);
            break;
            
        case RTL8291_CODE_LDO:
            //rtl8291_setRegBits(devId, RTL8291_WB_REG12_ADDR, RTL8291_WB_REG12_LDOSEL_1_0_MASK, 0x2);        
            rtl8291_getReg(devId, RTL8291_CODE_LDO_H_ADDR, &regData1);
            rtl8291_getReg(devId, RTL8291_CODE_LDO_M_ADDR, &regData2);
            rtl8291_getReg(devId, RTL8291_CODE_LDO_L_ADDR, &regData3);
            temp32 =((regData1 & 0xff) << 14) | ((regData2 & 0xff) << 6) | (regData3>>2);
            *code_data = temp32;
            //printk(KERN_EMERG "RTL8291_CODE_LDO: regData1 = 0x%x regData2 = 0x%x regData3 = 0x%x\n", regData1, regData2, regData3);                
            break;
            
        case RTL8291_CODE_RSSIINP_SEL:
            rtl8291_getReg(devId, RTL8291_CODE_RSSIINP_SEL_ADDR, &regData1);
            temp32 = regData1;
            *code_data = temp32;
            break;
            
        case RTL8291_CODE_RSSIINP:
            rtl8291_getReg(devId, RTL8291_CODE_RSSIINP_H_ADDR, &regData1);
            rtl8291_getReg(devId, RTL8291_CODE_RSSIINP_M_ADDR, &regData2);
            rtl8291_getReg(devId, RTL8291_CODE_RSSIINP_L_ADDR, &regData3);
            temp32 =((regData1 & 0xff) << 14) | ((regData2 & 0xff) << 6) | (regData3>>2);
            *code_data = temp32;
            break;
                
        case RTL8291_CODE_RSSIINN_SEL:
            rtl8291_getReg(devId, RTL8291_CODE_RSSIINN_SEL_ADDR, &regData1);
            temp32 = regData1;
            *code_data = temp32;
            break;
                    
        case RTL8291_CODE_RSSIINN:     
            rtl8291_getReg(devId, RTL8291_CODE_RSSIINN_H_ADDR, &regData1);
            rtl8291_getReg(devId, RTL8291_CODE_RSSIINN_M_ADDR, &regData2);
            rtl8291_getReg(devId, RTL8291_CODE_RSSIINN_L_ADDR, &regData3);
            temp32 =((regData1 & 0xff) << 14) | ((regData2 & 0xff) << 6) | (regData3>>2);
            *code_data = temp32;
            break;

        case RTL8291_CODE_RSSIFL_SEL:
            rtl8291_getReg(devId, RTL8291_CODE_RSSIFL_SEL_ADDR, &regData1);
            temp32 = regData1;
            *code_data = temp32;
            if (dbg_en==1)
                printk(KERN_EMERG "RTL8291_CODE_RSSIFL_SEL: regData1 = 0x%06x  code_data = 0x%06x\n", regData1, temp32);
            break;

        case RTL8291_CODE_RSSIFL:     
            rtl8291_getReg(devId, RTL8291_CODE_RSSIFL_H_ADDR, &regData1);
            rtl8291_getReg(devId, RTL8291_CODE_RSSIFL_M_ADDR, &regData2);
            rtl8291_getReg(devId, RTL8291_CODE_RSSIFL_L_ADDR, &regData3);
            temp32 =((regData1 & 0xff) << 14) | ((regData2 & 0xff) << 6) | (regData3>>2);
            *code_data = temp32;
            if (dbg_en==1)
                printk(KERN_EMERG "RTL8291_CODE_RSSIFL: regData1 = 0x%06x regData2 = 0x%06x regData3 = 0x%06x code_data = 0x%06x\n", regData1, regData2, regData3, temp32);
            break;
            
        case RTL8291_CODE_VLASERN:
            rtl8291_getReg(devId, RTL8291_CODE_VLASERN_H_ADDR, &regData1);
            rtl8291_getReg(devId, RTL8291_CODE_VLASERN_M_ADDR, &regData2);
            rtl8291_getReg(devId, RTL8291_CODE_VLASERN_L_ADDR, &regData3);
            temp32 =((regData1 & 0xff) << 16) | ((regData2 & 0xff) << 8) | (regData3);
            *code_data = temp32;
            break;
            
        case RTL8291_CODE_ADCIN:
            rtl8291_getReg(devId, RTL8291_CODE_ADCIN_H_ADDR, &regData1);
            rtl8291_getReg(devId, RTL8291_CODE_ADCIN_M_ADDR, &regData2);
            rtl8291_getReg(devId, RTL8291_CODE_ADCIN_L_ADDR, &regData3);
            temp32 =((regData1 & 0xff) << 14) | ((regData2 & 0xff) << 6) | (regData3>>2);
            *code_data = temp32;
            break;
            
        case RTL8291_CODE_PS_TX:
            rtl8291_getReg(devId, RTL8291_CODE_PS_TX_H_ADDR, &regData1);
            rtl8291_getReg(devId, RTL8291_CODE_PS_TX_M_ADDR, &regData2);
            rtl8291_getReg(devId, RTL8291_CODE_PS_TX_L_ADDR, &regData3);
            temp32 =((regData1 & 0xff) << 14) | ((regData2 & 0xff) << 6) | (regData3>>2);
            *code_data = temp32;
            break;
            
        case RTL8291_CODE_PS_RX:
            rtl8291_getReg(devId, RTL8291_CODE_PS_RX_H_ADDR, &regData1);
            rtl8291_getReg(devId, RTL8291_CODE_PS_RX_M_ADDR, &regData2);
            rtl8291_getReg(devId, RTL8291_CODE_PS_RX_L_ADDR, &regData3);
            temp32 =((regData1 & 0xff) << 14) | ((regData2 & 0xff) << 6) | (regData3>>2);
            *code_data = temp32;
            break;            
            
        case RTL8291_CODE_VBG:
            rtl8291_getReg(devId, RTL8291_CODE_VBG_H_ADDR, &regData1);
            rtl8291_getReg(devId, RTL8291_CODE_VBG_M_ADDR, &regData2);
            rtl8291_getReg(devId, RTL8291_CODE_VBG_L_ADDR, &regData3);
            temp32 =((regData1 & 0xff) << 14) | ((regData2 & 0xff) << 6) | (regData3>>2);
            *code_data = temp32;
            break;
                
        case RTL8291_CODE_CALG:
            rtl8291_getReg(devId, RTL8291_CODE_CALG_H_ADDR, &regData1);
            rtl8291_getReg(devId, RTL8291_CODE_CALG_M_ADDR, &regData2);
            rtl8291_getReg(devId, RTL8291_CODE_CALG_L_ADDR, &regData3);
            temp32 =((regData1 & 0xff) << 14) | ((regData2 & 0xff) << 6) | (regData3>>2);
            *code_data = temp32;
            //printk(KERN_EMERG "RTL8291_CODE_CALG: regData1 = 0x%x regData2 = 0x%x regData3 = 0x%x\n", regData1, regData2, regData3);                    
            break;
        //R14[4:0] = APCDIG_REF_GEN_BIAS_SELL<4:0>, bit 2 is IMPD_RSEL        
        case RTL8291_CODE_TXAPC_RSEL:
            rtl8291_getReg(devId, RTL8291_RB_REG14_ADDR, &regData1);
            temp32 = (regData1&0x4)>>2;
            *code_data = temp32;
            if (dbg_en==1)
                printk(KERN_EMERG "RTL8291_CODE_TXAPC_RSEL: regData1 = 0x%06x  code_data = 0x%06x\n", regData1, temp32);            
            break;
            
        default:
            return -1;
    }

    //rtl8291_setRegBit(devId, RTL8291_WB_REG4A_ADDR, RTL8291_WB_REG4A_REG_APCDIG_IMPD_READ_DISABLE_OFFSET, 1);

    //printk(KERN_EMERG "rtl8291_sdadc_code_get: sdadc_type = 0x%x code_data = 0x%x\n", sdadc_type, *code_data);

    return SUCCESS;
}

int32  rtl8291_reset(uint32 devId, rtk_ldd_reset_mode_t mode)
{
    switch(mode)
    {
        case LDD_RESET_CHIP:
            rtl8291_setRegBit(devId, RTL8291_REG_RESET_ADDR, RTL8291_REG_RESET_TOTAL_CHIP_RESET_OFFSET, 1);
            break;
        case LDD_RESET_DIGITAL:
            rtl8291_setRegBit(devId, RTL8291_REG_RESET_ADDR, RTL8291_REG_RESET_ALL_DIGITAL_RESET_OFFSET, 1);
            break;
        case LDD_RESET_8051:
            rtl8291_setRegBit(devId, RTL8291_REG_RESET_ADDR, RTL8291_REG_RESET_MCU_RESET_OFFSET, 1);
            break;
        case LDD_RESET_ANALOG:
            rtl8291_setRegBit(devId, RTL8291_REG_RESET_ADDR, RTL8291_REG_RESET_ALL_ANALOG_RESET_OFFSET, 1);
            break;
        default:
            break;
    }

    return SUCCESS;
}

int32 rtl8291_driver_version_get(uint32 *ver)
{
    *ver = EUROPA_8291_DRIVER_VERSION;
    return SUCCESS;
}

int32  rtl8291_parameter_set(uint32 devId, uint32 address, uint32 data)
{
    if((address >0x7FF)||(data>0xFF))
    {
        printk(KERN_EMERG " rtl8291_parameter_set out of bound !!! address = 0x%x, data = 0x%x\n", address,data);
        return FAILED;
    }
    
    rtl8291_setReg(devId, address, data); 

    //printk(KERN_EMERG " rtl8291_parameter_set: length = %d, offset = 0x%x,  data_ptr = 0x%x!!! \n", length, offset, data_ptr);

    return SUCCESS;
}

int32  rtl8291_parameter_get(uint32 devId, uint32 address, uint32 *pData)
{
    if(address >0x7FF)
    {
        printk(KERN_EMERG "address = 0x%x rtl8291_parameter_set out of bound !!! \n", address);
        return FAILED;
    }

    if(pData == NULL)
    {
        printk(KERN_EMERG " rtl8291_parameter_get pData is NULL!!! \n");
        return FAILED;
    }

    rtl8291_getReg(devId, address, pData); 

    //printk(KERN_EMERG " rtl8291_parameter_get: devId = 0x%x, length = %d, offset = 0x%x,  data = 0x%x!!! \n", devId, length, offset, data);  

    return SUCCESS;
}


int32  rtl8291_dac_set(uint32 devId, uint32 dac_offset, uint32 dac_code)
{
    uint32 regData_h=0, regData_l=0, mask=0;

    if ((dac_code>0x1FF)||(dac_offset>0xF))
    {
      return FAILED;
    }
    //printk(KERN_EMERG " rtl8291_dac_set: dac_offset = 0x%x, dac_code = 0x%x \n", dac_offset, dac_code); 

    mask = RTL8291_WB_REG01_REG_IBX50U2_MASK|RTL8291_WB_REG01_REG_IBX50U1_MASK|RTL8291_WB_REG01_REG_IBX25U2_MASK|RTL8291_WB_REG01_REG_IBX25U1_MASK;
    rtl8291_setRegBits(devId, RTL8291_WB_REG01_ADDR, mask, dac_offset); 
    
    regData_h = dac_code>>1;
    regData_l = dac_code&0x1;    
  
    rtl8291_setReg(devId, RTL8291_REG_DAC_HB_ADDR, regData_h);    
    rtl8291_setReg(devId, RTL8291_REG_DAC_LB_ADDR, regData_l);        
  
    return SUCCESS;
}

int32  rtl8291_dac_get(uint32 devId, uint32 *pDac_offset, uint32 *pDac_code)
{
    uint32 regData_h, regData_l, mask = 0;

    mask = RTL8291_WB_REG01_REG_IBX50U2_MASK|RTL8291_WB_REG01_REG_IBX50U1_MASK|RTL8291_WB_REG01_REG_IBX25U2_MASK|RTL8291_WB_REG01_REG_IBX25U1_MASK;
    rtl8291_getRegBits(devId, RTL8291_WB_REG01_ADDR, mask, pDac_offset); 

    rtl8291_getReg(devId, RTL8291_REG_DAC_HB_ADDR, &regData_h);    
    rtl8291_getReg(devId, RTL8291_REG_DAC_LB_ADDR, &regData_l);        

    *pDac_code = (regData_h<<1)|(regData_l&0x1);
    //printk(KERN_EMERG " rtl8291_dac_get: dac_offset = 0x%x, dac_code = 0x%x \n", *pDac_offset, *pDac_code); 
    return SUCCESS;
}

int32  rtl8291_apdLut_set(uint32 devId, uint32 entry, uint32 data)
{
    uint32 addr_base, entry_addr;
    uint32 din_l_8_1, din_l_0;

    if (entry>31)
        return FAILED;
    if (data>0x1FF)
        return FAILED;

    addr_base  = RTL8291_APD_LUT_REG00_ADDR;
    entry_addr = addr_base + entry*2;
    din_l_8_1  = (data &0x1FE)>>1;
    din_l_0    = (data&0x1);

    rtl8291_setReg(devId, entry_addr, din_l_8_1);
    rtl8291_setReg(devId, entry_addr+1, din_l_0);

    //printk(KERN_EMERG " rtl8291_apdLut_set: entry_addr = 0x%x, din_l_8_1 = 0x%x din_l_0 = 0x%x\n", entry_addr, din_l_8_1, din_l_0);

    return SUCCESS;
}

int32  rtl8291_apdLut_get(uint32 devId, uint32 entry, uint32 *pData)
{
    uint32 addr_base, entry_addr;
    uint32 din_l_8_1, din_l_0;

    if (entry>31)
        return FAILED;

    addr_base  = RTL8291_APD_LUT_REG00_ADDR;
    entry_addr = addr_base + entry*2;
    
    rtl8291_getReg(devId, entry_addr, &din_l_8_1);
    rtl8291_getReg(devId, entry_addr+1, &din_l_0);

    *pData = ((din_l_8_1&0xff)<<1)|(din_l_0&0x1);

    //printk(KERN_EMERG " rtl8291_apdLut_get: entry_addr = 0x%x, din_l_8_1 = 0x%x din_l_0 = 0x%x\n", entry_addr, din_l_8_1, din_l_0);

    return SUCCESS;
}

int32  rtl8291_laserLut_set(uint32 devId, uint32 entry, uint32 bias, uint32 mod)
{
    uint32 addr_base, bias_addr, mod_addr;
    uint32 bias_10_4, bias_3_0;
    uint32 mod_10_4, mod_3_0;

    if (entry>31)
        return FAILED;
    if (bias>0x7FF)
        return FAILED;
    if (mod>0x7FF)
        return FAILED;

    addr_base  = RTL8291_TX_LUT_REG00_ADDR;
    bias_addr  = addr_base + entry*2;
    bias_10_4  = (bias &0x7F0)>>4;
    bias_3_0   = (bias&0xF);
    mod_addr  = addr_base + 64 + entry*2;
    mod_10_4  = (mod &0x7F0)>>4;
    mod_3_0   = (mod&0xF);

    rtl8291_setReg(devId, bias_addr, bias_10_4);
    rtl8291_setReg(devId, bias_addr, bias_3_0);
    rtl8291_setReg(devId, mod_addr, mod_10_4);
    rtl8291_setReg(devId, mod_addr, mod_3_0);

    //printk(KERN_EMERG " rtl8291_txLut_set: bias_addr = 0x%x, bias_10_4 = 0x%x bias_3_0 = 0x%x\n", bias_addr, bias_10_4, bias_3_0);
    //printk(KERN_EMERG " rtl8291_txLut_set: mod_addr = 0x%x, mod_10_4 = 0x%x mod_3_0 = 0x%x\n", mod_addr, mod_10_4, mod_3_0);

    return SUCCESS;
}

int32  rtl8291_laserLut_get(uint32 devId, uint32 entry, uint32 *pBias, uint32 *pMod)
{
    uint32 addr_base, bias_addr, mod_addr;
    uint32 bias_10_4, bias_3_0;
    uint32 mod_10_4, mod_3_0;

    if (entry>31)
        return FAILED;

    addr_base  = RTL8291_TX_LUT_REG00_ADDR;
    bias_addr  = addr_base + entry*2;
    mod_addr  = addr_base + 64 + entry*2;

    rtl8291_getReg(devId, bias_addr, &bias_10_4);
    rtl8291_getReg(devId, bias_addr, &bias_3_0);
    rtl8291_getReg(devId, mod_addr, &mod_10_4);
    rtl8291_getReg(devId, mod_addr, &mod_3_0);

    //printk(KERN_EMERG " rtl8291_txLut_get: bias_addr = 0x%x, bias_10_4 = 0x%x bias_3_0 = 0x%x\n", bias_addr, bias_10_4, bias_3_0);
    //printk(KERN_EMERG " rtl8291_txLut_get: mod_addr = 0x%x, mod_10_4 = 0x%x mod_3_0 = 0x%x\n", mod_addr, mod_10_4, mod_3_0);

    *pBias = ((bias_10_4&0xFF)<<4)|(bias_3_0&0xF);
    *pMod  = ((mod_10_4&0xFF)<<4)|(mod_3_0&0xF);

    return SUCCESS;
}

//Note: When EFUSE is set, it can't be clean. it can only set a bit from 0 to 1, 
//but can't drom 1 to 0.
int32  rtl8291_efuse_set(uint32 devId, uint32 entry, uint32 data)
{
    uint32 regData=0, cnt=0, status;

    if (entry>511)
        return FAILED;
    if (data>0xFF)
        return FAILED;    

    //Enable EFUSE Access
    rtl8291_setReg(devId, RTL8291_REG_EFUSE_ACCESS_EN_ADDR, 0x27);
    //Set entry MSB 3bit
    regData = (entry&0x1C0)>>6;
    rtl8291_setRegBits(devId, RTL8291_REG_EFUSE_READ_MODE_ADDR, RTL8291_REG_EFUSE_READ_MODE_EFUS_IND_AH_MASK, regData);
    //Set data
    rtl8291_setReg(devId, RTL8291_REG_EFUSE_IND_WD_ADDR, data);    
    //Set CMD & entry
    regData = (1<<7)|(1<<6)|(entry&0x3F);
    rtl8291_setReg(devId, RTL8291_REG_EFUSE_IND_CMD_ADDR, regData);

    //Poll EFUSE busy bit
    while(cnt<1000) //check EFUSE BUSY
    {
        rtl8291_getRegBit(devId, RTL8291_REG_EFUSE_IND_CMD_ADDR, RTL8291_REG_EFUSE_IND_CMD_EFUS_IND_STATUS_OFFSET, &status);
        if(status == 0)
        {
            return SUCCESS;
        }
        cnt++;
    }    

    printk("%s : %d %d\n", __func__, __LINE__, status); 

    return SUCCESS; 
}

int32  rtl8291_efuse_get(uint32 devId, uint32 entry, uint32 *pData)
{
    uint32 regData = 0, cnt=0, status;
    
    if (entry>511)
        return FAILED;
    
    //Enable EFUSE Access
    rtl8291_setReg(devId, RTL8291_REG_EFUSE_ACCESS_EN_ADDR, 0x27);
    //Set entry MSB 3bit
    regData = (entry&0x1C0)>>6;
    rtl8291_setRegBits(devId, RTL8291_REG_EFUSE_READ_MODE_ADDR, RTL8291_REG_EFUSE_READ_MODE_EFUS_IND_AH_MASK, regData);
    //Set CMD & entry
    regData = (1<<7)|(entry&0x3F);
    rtl8291_setReg(devId, RTL8291_REG_EFUSE_IND_CMD_ADDR, regData);

    //Poll EFUSE busy bit
    while(cnt<1000) //check EFUSE BUSY
    {
        rtl8291_getRegBit(devId, RTL8291_REG_EFUSE_IND_CMD_ADDR, RTL8291_REG_EFUSE_IND_CMD_EFUS_IND_STATUS_OFFSET, &status);
        if(status == 0)
        {
            break;
        }
        cnt++;
    }    

    if (cnt>=1000)
    {
        printk("%s : %d %d\n", __func__, __LINE__, status); 
        return FAILED;    
    }

    rtl8291_getReg(devId, RTL8291_REG_EFUSE_IND_RD_ADDR, pData);
    rtl8291_setReg(devId, RTL8291_REG_EFUSE_READ_MODE_ADDR, 0);    
    return SUCCESS; 
}

int32  rtl8291_txLutEnable_set(uint32 devId, uint32 enable)
{
    if (enable>0x1)
      return FAILED;
    
    //rtl8291_setRegBit(devId, RTL8291_REG_LUT_CONTROL0_ADDR, RTL8291_REG_LUT_CONTROL0_TX_LUT_SOURCE_OFFSET, enable);
    rtl8291_setRegBit(devId, RTL8291_REG_LUT_CONTROL0_ADDR, RTL8291_REG_LUT_CONTROL0_TX_LUT_LOOKUP_ENABLE_OFFSET, enable);

    if (enable==0)
    {
         rtl8291_setRegBit(devId, RTL8291_REG_LUT_CONTROL0_ADDR, RTL8291_REG_LUT_CONTROL0_TX_LUT_SOURCE_OFFSET, 1);
    }
    else
    {
         rtl8291_setRegBit(devId, RTL8291_REG_LUT_CONTROL0_ADDR, RTL8291_REG_LUT_CONTROL0_TX_LUT_SOURCE_OFFSET, 0);
    }
    return SUCCESS;
}
        
int32  rtl8291_txLutEnable_get(uint32 devId, uint32 *pEnable)
{
    rtl8291_getRegBit(devId, RTL8291_REG_LUT_CONTROL0_ADDR, RTL8291_REG_LUT_CONTROL0_TX_LUT_LOOKUP_ENABLE_OFFSET, pEnable);
    return SUCCESS;
}

int32  rtl8291_apdLutEnable_set(uint32 devId, uint32 enable)
{
    if (enable>0x1)
      return FAILED;

    //rtl8291_setRegBit(devId, RTL8291_REG_LUT_CONTROL0_ADDR, RTL8291_REG_LUT_CONTROL0_APD_LUT_SOURCE_OFFSET, enable);
    rtl8291_setRegBit(devId, RTL8291_REG_LUT_CONTROL0_ADDR, RTL8291_REG_LUT_CONTROL0_APD_LUT_LOOKUP_ENABLE_OFFSET, enable);        
    if (enable==0)
    {
         rtl8291_setRegBit(devId, RTL8291_REG_LUT_CONTROL0_ADDR, RTL8291_REG_LUT_CONTROL0_APD_LUT_SOURCE_OFFSET, 1);
    }
    else
    {
        rtl8291_setRegBit(devId, RTL8291_REG_LUT_CONTROL0_ADDR, RTL8291_REG_LUT_CONTROL0_APD_LUT_SOURCE_OFFSET, 0);
    }
    
    return SUCCESS;
}
        
int32  rtl8291_apdLutEnable_get(uint32 devId, uint32 *pEnable)
{
    //rtl8291_getRegBit(devId, RTL8291_REG_LUT_CONTROL0_ADDR, RTL8291_REG_LUT_CONTROL0_APD_LUT_SOURCE_OFFSET, pEnable);
    rtl8291_getRegBit(devId, RTL8291_REG_LUT_CONTROL0_ADDR, RTL8291_REG_LUT_CONTROL0_APD_LUT_LOOKUP_ENABLE_OFFSET, pEnable);
    return SUCCESS;
}

int32  rtl8291_vdd_get(uint32 devId, uint32 *vdd)
{

    int32 ret = 0;
    uint32 code_data, tmp1, tmp2;
    
    ret = rtl8291_getReg(devId, RTL8291_STD_DDMI_VCC_H_ADDR, &tmp1);
    if (ret!=0)
        return ret;
    
    ret = rtl8291_getReg(devId, RTL8291_STD_DDMI_VCC_L_ADDR, &tmp2);
    if (ret!=0)
        return ret;
    
    code_data = ((tmp1&0xFF)<<8)|(tmp2&0xFF);
    *vdd = code_data;
#if 0
    uint32 temp32;
    uint64 temp64;
    uint32 vdd_code, gnd_code, ldo_code;
    uint32 volt_vdd;//unit is uV
    uint32 code_data;

    rtl8291_sdadc_code_get(devId, RTL8291_CODE_PS, &vdd_code);
    rtl8291_sdadc_code_get(devId, RTL8291_CODE_CALG, &gnd_code);
    rtl8291_sdadc_code_get(devId, RTL8291_CODE_LDO, &ldo_code);


    //Old algorithm
    //temp64 = vdd_code - ldo_code;
    //temp64 = temp64*1200000;
    //temp32 = ldo_code - gnd_code;
    //do_div(temp64, temp32);
    //volt_vdd = (uint32)(temp64+1200000);
    
    //set VCC_FINAL [ expr 1.2*($PS - $CALG)/($LDO-$CALG)*2]    
    temp64 = vdd_code - gnd_code;
    temp64 = temp64*1200000;
    temp32 = ldo_code - gnd_code;
    do_div(temp64, temp32);
    volt_vdd = (uint32)(temp64*2);
    
    //printk(" rtl8291_vdd_get: gnd_code = 0x%x, vdd_code = 0x%x, ldo_code = 0x%x, volt_vdd = 0x%x\n", gnd_code, vdd_code, ldo_code, volt_vdd);
    *vdd = volt_vdd;
#endif
    return SUCCESS;
}

//Range of R6400 is 6000 ~ 7700
int32 rtl8291_rssiR6400_get(uint32 devId, uint32 *value)
{
    uint32 data1, data2;
    uint32 R1_data=0, temp32;
    int    temp2;

    rtl8291_getReg(devId, RTL8291_RSSI_R1_MSB_ADDR, &data1); 
    rtl8291_getRegBits(devId, RTL8291_RSSI_R1_LSB_ADDR, RTL8291_RSSI_R1_LSB_RSSI_R1_LSB_MASK, &data2);
    R1_data = (data1&0xFF)<<4|(data2&0xF); 
    if (R1_data==0)
    {
        //Default Value of R6400
        *value =6400;
    }
    else
    {
        //R6400 = R1*LSB/(200uA-140uA) = R1*(4000000/4095)/(200-140)
        if (R1_data>1000)
        {
            temp32 = 4000000/60;
            temp32 = R1_data*temp32/4095;
        }
        else
        {
            temp32 = R1_data*4000000/60;
            temp32 = temp32/4095;
        }
        *value =temp32;        
    }

#if 0
    temp1 = (data1&0xFF)<<4|(data2&0xF);
    if (temp1&0x800)
    {
        temp1 = 0xFFFFF000|temp1;
    }
    temp2 = RTL8291_RSSI_CODE_STANDARD + (int)temp1;
    *value = (uint32)temp2;
#endif
    return SUCCESS;
}

#define PARAM_RX_A_SHIFT 30
#define PARAM_RX_B_SHIFT 13
#define PARAM_RX_C_SHIFT 12

int32  rtl8291_rxTrans_get(uint32 devId, rtl8291_rxpwr_t *pLdd_rxpwr)
{
    //int32 temp32;
    int64 temp64;
    int32 temp_a, temp_b, temp_c, sum;    
    int32 i_rssi;
    int32 rx_a, rx_b, rx_c;
    uint32 I_RSSI_SCALE = 10;

    if (pLdd_rxpwr==NULL)
      return FAILED;

    i_rssi = pLdd_rxpwr->rssi_code1;
    rx_a  = pLdd_rxpwr->rx_a;
    rx_b  = pLdd_rxpwr->rx_b;
    rx_c  = pLdd_rxpwr->rx_c;

    /* scale down i_rssi to avoid overflow for a x^2 */

    if (i_rssi > 65535)
    {
        I_RSSI_SCALE = 100;
    }
    else
    {
        I_RSSI_SCALE = 10;
    }
    i_rssi/=I_RSSI_SCALE;


    /* multipled by 1000 during calculation for better accuracy */

    /* ax^2 */
    temp64 = (int64)(i_rssi)*i_rssi;
    do_div( temp64, 1000);
    temp64 = temp64*I_RSSI_SCALE*I_RSSI_SCALE;
    temp64 = temp64*rx_a;
    do_div( temp64, (1<<PARAM_RX_A_SHIFT));    
    temp_a =(int32)( temp64);

    /* bx */
    temp64 = (int64)(i_rssi)*rx_b;
    do_div( temp64,  (1<<PARAM_RX_B_SHIFT));    
    temp64 = temp64*I_RSSI_SCALE;
    temp_b =(int32)( temp64);

    /* c */
    temp_c = (rx_c*1000)/(1<<PARAM_RX_C_SHIFT);

    sum = (temp_a+temp_b+temp_c);
    pLdd_rxpwr->rssi_code2 = sum;

    /* Unit is 0.1uW per SFF-8472*/
    sum = sum/100;
    pLdd_rxpwr->rssi_code3 = sum;

    return SUCCESS;
}


//CLI input RSSI Code
int32  rtl8291_rxPower_get(uint32 devId, rtl8291_rxpwr_t *pLdd_rxpwr)
{
    //int32 temp32;
    int64 temp64;
    uint32 code_rssi, code_k, code_R, Vlsb, C_lsb;
    uint32 mode, bit_shift;
    int32 I_rssi, I_k, I_cal;
    int32 code_M, M_shift, F_shift;
    int32 unit, cal_mode;
    //rtl8291_rxpwr_t hw_pwr;

    if (pLdd_rxpwr==NULL)
      return FAILED;

    cal_mode  = pLdd_rxpwr->op_mode;

    if (cal_mode == RTL8291_RX_TRANS_MODE)
    {
        rtl8291_rxTrans_get(devId, pLdd_rxpwr);
        return SUCCESS;
    }
    if (cal_mode == RTL8291_RX_CURR2PWR_MODE)
    {
        _rtl8291_rxCurr2Pwr_get(devId, pLdd_rxpwr);
        return SUCCESS;
    }
    if (cal_mode == RTL8291_RX_REAL_MODE)
    {
        //It is used in DDMI RX Power
        if (pLdd_rxpwr->rx_code_200u == 0)
        {
           pLdd_rxpwr->rx_code_200u = ldd_param[devId-5].rx_code_200u;
        }
        if (pLdd_rxpwr->rx_lsb == 0)
        {
           pLdd_rxpwr->rx_lsb = ldd_param[devId-5].rx_lsb;
        }
        if (pLdd_rxpwr->rx_r6400 == 0)
        {
           pLdd_rxpwr->rx_r6400 = ldd_param[devId-5].rx_r6400;
        }
        rtl8291_rxRssiCurrent_get(devId, pLdd_rxpwr);
        
        return SUCCESS;
    }
    if (cal_mode == RTL8291_RX_HARDWARE_MODE)
    {
        if (pLdd_rxpwr->rx_code_200u == 0)
        {
           pLdd_rxpwr->rx_code_200u = ldd_param[devId-5].rx_code_200u;
        }    
        if (pLdd_rxpwr->rx_lsb == 0)
        {
           pLdd_rxpwr->rx_lsb = ldd_param[devId-5].rx_lsb;
        }
        if (pLdd_rxpwr->rx_r6400 == 0)
        {
           pLdd_rxpwr->rx_r6400 = ldd_param[devId-5].rx_r6400;
        }
        rtl8291_rxRssiCurrentHw_get(devId, pLdd_rxpwr);        
        return SUCCESS;
    }

    return SUCCESS;
}

//Reference to RSSI_Auto-mode_Patch.tcl
int32  rtl8291_rxRssiCurrentHw_get(uint32 devId, rtl8291_rxpwr_t *pLdd_rxpwr)
{
    int32 ret = 0;
    uint32 ddmi_rxpow;
    uint32 regData_h, regData_l;
    uint32 PGA_Sel=0, rssifl_sel=0, rssifl=0;
    uint32 DDMI_IRSSI_B5, DDMI_IRSSI_B4, DDMI_IRSSI_B3, DDMI_IRSSI_B2, DDMI_IRSSI_B1, DDMI_IRSSI_B0;
    uint32 DDMI_IRSSI_2P;
    int32 I_rssi=0;
    int32 temp32;
    int64 temp64;
    uint32 code_k, code_R, Vlsb, C_lsb, bit_shift;
    //uint32 mode, , rssifl_sel;
    int32 I_k, I_cal, I_fin;
    int32 code_M, M_shift, F_shift;
    //int32 unit, cal_mode;
    int    dbg_en;

    dbg_en = _8291_debug_flag(devId);

    if (pLdd_rxpwr==NULL)
      return FAILED;

    //code_K is 1326 in TRL6733_RSSI@AUTO_MODE_Board(V1)_Num(3)_Block(TXEC)_VPADCAP=10nF 19.xlsx. it will be dynamic in future
    //code_k = pLdd_rxpwr->rx_code_200u;


    ret = rtl8291_getReg(devId, RTL8291_STD_DDMI_RXPOW_H_ADDR, &regData_h);
    if (ret!=0)
        return ret;

    ret = rtl8291_getReg(devId, RTL8291_STD_DDMI_RXPOW_L_ADDR, &regData_l); 
    if (ret!=0)
        return ret;

    ddmi_rxpow = ((regData_h&0xFF)<<8)|(regData_l&0xFF);
 
    
    ret = rtl8291_getReg(devId, RTL8291_CFG_DR_EXT2_CONTROL6_ADDR, &regData_l); 
    if (ret!=0)
        return ret;

    PGA_Sel = (regData_h&0xC0)>>6;

    ret = rtl8291_getReg(devId, RTL8291_DDMI_EXTRA_H_ADDR, &regData_h);
    if (ret!=0)
        return ret;
    
    ret = rtl8291_getReg(devId, RTL8291_DDMI_EXTRA_L_ADDR, &regData_l); 
    if (ret!=0)
        return ret;

    rssifl = ((regData_h&0xF)<<8)|(regData_l&0xFF);
    rssifl_sel = ((regData_h&0xF0)>>4);
    if ((pLdd_rxpwr->rssi_code1 ==0)&&(pLdd_rxpwr->rssi_code2 ==0)&&(pLdd_rxpwr->rssi_code3 ==0))
    {
        pLdd_rxpwr->rssi_code1 = rssifl;
        pLdd_rxpwr->rssi_code2 = rssifl_sel;
        pLdd_rxpwr->rssi_code3 = PGA_Sel;
    }


    //B0~B5 & 2P is for debug
    ret = rtl8291_getReg(devId, RTL8291_DDMI_IRSSI_B5_ADDR, &DDMI_IRSSI_B5); 
    if (ret!=0)
        return ret;

    ret = rtl8291_getReg(devId, RTL8291_DDMI_IRSSI_B4_ADDR, &DDMI_IRSSI_B4); 
    if (ret!=0)
        return ret;    

    ret = rtl8291_getReg(devId, RTL8291_DDMI_IRSSI_B3_ADDR, &DDMI_IRSSI_B3); 
    if (ret!=0)
        return ret;

    ret = rtl8291_getReg(devId, RTL8291_DDMI_IRSSI_B2_ADDR, &DDMI_IRSSI_B2); 
    if (ret!=0)
        return ret; 

    ret = rtl8291_getReg(devId, RTL8291_DDMI_IRSSI_B1_ADDR, &DDMI_IRSSI_B1); 
    if (ret!=0)
        return ret;    

    ret = rtl8291_getReg(devId, RTL8291_DDMI_IRSSI_B0_ADDR, &DDMI_IRSSI_B0); 
    if (ret!=0)
        return ret;

    ret = rtl8291_getReg(devId, RTL8291_DDMI_IRSSI_2P_ADDR, &DDMI_IRSSI_2P); 
    if (ret!=0)
        return ret; 

    //The I_rssi should be the same as rtl8291_rxRssiCurrent_get output
    I_rssi = (DDMI_IRSSI_B1*256)+DDMI_IRSSI_B0;
    pLdd_rxpwr->i_rssi = (uint32)I_rssi;
    pLdd_rxpwr->rx_power = ddmi_rxpow;
    pLdd_rxpwr->rx_a = DDMI_IRSSI_B1;
    pLdd_rxpwr->rx_b = DDMI_IRSSI_B0;
    //printk("rtl8291_rxRssiCurrent_ge(1): B0 = 0x%x B1 = 0x%x I-rssi= 0x%x output = 0x%x\n", (DDMI_IRSSI_B1&0xFF), DDMI_IRSSI_B0, I_rssi, pLdd_rxpwr->i_rssi);
#if 1
    code_k    = pLdd_rxpwr->rx_code_200u;
    code_R    = pLdd_rxpwr->rx_r6400;    
    //M: 4^x, x:-2/-1/0/1/2/3/4/5    
    code_M    = (rssifl_sel-2);
    //unit      = pLdd_rxpwr->data_c;

    //printk(" rtl8291_rxRssiCurrent_get: input %d %d %d %d\n", pLdd_rxpwr->rssi_code1, pLdd_rxpwr->rssi_code2, pLdd_rxpwr->rssi_code3, pLdd_rxpwr->data_a);
            
    //ICAL is 200uA in this version, transfer to unit 0.1uV
    I_cal = 200;
    //LSB is 4V, transfer to 0.1uV, 40000000 = 2^9*5^7can combine with bit shift
    Vlsb = 4*1000000;
    ///R:6.4K
    //code_R = 6400;
        
    //20240430: Already shifted for 12bit
    //code_rssi = code_rssi>>10;
    //code_k      = code_k>>10;   
    C_lsb = Vlsb/code_R;       
    bit_shift = 12; //transfer to uA

    //I_rssi = (code_rssi - code_k)*Vlsb/code_R + i_cal;
    I_rssi = (rssifl-code_k)*C_lsb;
    I_k    = code_k*C_lsb;
            
    //printk(" rtl8290c_rxPower_get:1 I_rssi = %d I_cal = %d\n", I_rssi, I_cal);    
            
    M_shift = 2*code_M;
    F_shift = bit_shift + M_shift;
        
        
    //!!!change unit from uA to uA/32, this is to increase accuracy
    M_shift = M_shift-5;
    F_shift = F_shift-5;    
           
    if (M_shift>=0)
    {
        I_cal  = I_cal>>M_shift;     
    }
    else
    {
        I_cal  = I_cal<<abs(M_shift);      
    }
                
    if (F_shift>=0)
    {
        I_rssi = I_rssi>>F_shift;  
        I_k    = I_k>>F_shift;          
    }
    else
    {
        I_rssi    = I_rssi<<abs(F_shift); 
        I_k    = I_k<<abs(F_shift);        
    }

    //printk("rtl8291_rxRssiCurrent_ge(2): B0 = 0x%x B1 = 0x%x I-rssi= 0x%x, output = 0x%x\n", , DDMI_IRSSI_B0, I_rssi, pLdd_rxpwr->i_rssi);
          
    pLdd_rxpwr->data_a = I_rssi;
    pLdd_rxpwr->data_b = I_cal;
    pLdd_rxpwr->data_c = I_rssi+I_cal;     
            
    //printk(" rtl8291_rxRssiCurrent_get:3 I_rssi = %d I_cal = %d\n", I_rssi, I_cal);    
    //    I_rssi = I_rssi + I_cal;
    //pLdd_rxpwr->i_rssi= (uint32)I_rssi;

    //if(dbg_en==1) 
        //printk("rtl8291_rxRssiCurrent_get:  RSSIFL = %d(0x%x), RSSIFL_SEL = %d(0x%x), B0 = 0x%x B1 = 0x%x I-rssi(HW) = %d\n", rssifl, pLdd_rxpwr->rssi_code1, 
        //rssifl_sel, pLdd_rxpwr->rssi_code2, DDMI_IRSSI_B0, DDMI_IRSSI_B1, pLdd_rxpwr->i_rssi);
#endif

    return SUCCESS;
}

//Reference to RSSI_Auto-mode_Patch.tcl
int32  rtl8291_rxRssiCurrent_get(uint32 devId, rtl8291_rxpwr_t *pLdd_rxpwr)
{
    int32 ret = 0;
    uint32 ddmi_rxpow;
    uint32 regData_h, regData_l;
    uint32 rssifl, PGA_Sel, rssifl_sel;
    uint32 DDMI_IRSSI_B5, DDMI_IRSSI_B4, DDMI_IRSSI_B3, DDMI_IRSSI_B2, DDMI_IRSSI_B1, DDMI_IRSSI_B0;
    uint32 DDMI_IRSSI_2P;
    //int32 temp32;
    //int64 temp64;
    uint32 code_rssi, code_k, code_R, Vlsb, C_lsb;
    uint32 mode, bit_shift;
    int32 I_rssi, I_k, I_cal, I_fin;
    int32 code_M, M_shift, F_shift;
    //int32 unit, cal_mode;
    int    dbg_en;

    dbg_en = _8291_debug_flag(devId);

    if (pLdd_rxpwr==NULL)
      return FAILED;

    //code_K is 1326 in TRL6733_RSSI@AUTO_MODE_Board(V1)_Num(3)_Block(TXEC)_VPADCAP=10nF 19.xlsx. it will be dynamic in future
    //code_k = pLdd_rxpwr->rx_code_200u;
#if 0
    ret = rtl8291_getReg(devId, RTL8291_STD_DDMI_RXPOW_H_ADDR, &regData_h);
    if (ret!=0)
        return ret;

    ret = rtl8291_getReg(devId, RTL8291_STD_DDMI_RXPOW_L_ADDR, &regData_l); 
    if (ret!=0)
        return ret;
    ddmi_rxpow = ((regData_h&0xFF)<<8)|(regData_l&0xFF);
  
    ret = rtl8291_getReg(devId, RTL8291_CFG_DR_EXT2_CONTROL6_ADDR, &regData_l); 
    if (ret!=0)
        return ret;
    PGA_Sel = (regData_h&0xC0)>>6;


    //B0~B5 & 2P is for debug
    ret = rtl8291_getReg(devId, RTL8291_DDMI_IRSSI_B5_ADDR, &DDMI_IRSSI_B5); 
    if (ret!=0)
        return ret;

    ret = rtl8291_getReg(devId, RTL8291_DDMI_IRSSI_B4_ADDR, &DDMI_IRSSI_B4); 
    if (ret!=0)
        return ret;    

    ret = rtl8291_getReg(devId, RTL8291_DDMI_IRSSI_B3_ADDR, &DDMI_IRSSI_B3); 
    if (ret!=0)
        return ret;

    ret = rtl8291_getReg(devId, RTL8291_DDMI_IRSSI_B2_ADDR, &DDMI_IRSSI_B2); 
    if (ret!=0)
        return ret; 

    ret = rtl8291_getReg(devId, RTL8291_DDMI_IRSSI_B1_ADDR, &DDMI_IRSSI_B1); 
    if (ret!=0)
        return ret;    

    ret = rtl8291_getReg(devId, RTL8291_DDMI_IRSSI_B0_ADDR, &DDMI_IRSSI_B0); 
    if (ret!=0)
        return ret;

    ret = rtl8291_getReg(devId, RTL8291_DDMI_IRSSI_2P_ADDR, &DDMI_IRSSI_2P); 
    if (ret!=0)
        return ret; 
#endif

    ret = rtl8291_getReg(devId, RTL8291_DDMI_EXTRA_H_ADDR, &regData_h);
    if (ret!=0)
        return ret;
    
    ret = rtl8291_getReg(devId, RTL8291_DDMI_EXTRA_L_ADDR, &regData_l); 
    if (ret!=0)
        return ret;
    
    rssifl       = ((regData_h&0x0F)<<8)|(regData_l&0xFF);
    rssifl_sel = (regData_h&0xF0)>>4;

#if 1
    if ((pLdd_rxpwr->rssi_code1 ==0)&&(pLdd_rxpwr->rssi_code2 ==0))
    {
        pLdd_rxpwr->rssi_code1 = rssifl;
        pLdd_rxpwr->data_a = rssifl_sel;
    }
    else//For external input, usually for debug
    {
        rssifl = pLdd_rxpwr->rssi_code1;
        rssifl_sel = pLdd_rxpwr->data_a;
    }

    code_rssi = rssifl;
    code_k    = pLdd_rxpwr->rssi_code2;
    code_R    = pLdd_rxpwr->rssi_code3;    
    //M: 4^x, x:-2/-1/0/1/2/3/4/5    
    code_M    = (rssifl_sel-2);
    //unit      = pLdd_rxpwr->data_c;

    printk(" rtl8291_rxRssiCurrent_get: input %d %d %d %d\n", pLdd_rxpwr->rssi_code1, pLdd_rxpwr->rssi_code2, pLdd_rxpwr->rssi_code3, pLdd_rxpwr->data_a);
            
    //ICAL is 200uA in this version, transfer to unit 0.1uV
    I_cal = 200;
    //LSB is 4V, transfer to 0.1uV, 40000000 = 2^9*5^7can combine with bit shift
    Vlsb = 4*1000000;
    ///R:6.4K
    //code_R = 6400;
        
    //20240430: Already shifted for 12bit
    //code_rssi = code_rssi>>10;
    //code_k      = code_k>>10;   
    C_lsb = Vlsb/code_R;       
    bit_shift = 12; //transfer to uA

    //I_rssi = (code_rssi - code_k)*Vlsb/code_R + i_cal;
    I_rssi = (code_rssi-code_k)*C_lsb;
    I_k    = code_k*C_lsb;
            
    //printk(" rtl8290c_rxPower_get:1 I_rssi = %d I_cal = %d\n", I_rssi, I_cal);    
            
    M_shift = 2*code_M;
    F_shift = bit_shift + M_shift;
        
        
    //!!!change unit from uA to uA/32, this is to increase accuracy
    M_shift = M_shift-5;
    F_shift = F_shift-5;    
           
    if (M_shift>=0)
    {
        I_cal  = I_cal>>M_shift;     
    }
    else
    {
        I_cal  = I_cal<<abs(M_shift);      
    }
                
    if (F_shift>=0)
    {
        I_rssi = I_rssi>>F_shift;  
        I_k    = I_k>>F_shift;          
    }
    else
    {
        I_rssi    = I_rssi<<abs(F_shift); 
        I_k    = I_k<<abs(F_shift);        
    }
            
    pLdd_rxpwr->rx_a = I_rssi;
    pLdd_rxpwr->rx_b = I_k;
    pLdd_rxpwr->rx_c = I_cal;     
            
    //printk(" rtl8291_rxRssiCurrent_get:3 I_rssi = %d I_cal = %d\n", I_rssi, I_cal);    
    //    I_rssi = I_rssi + I_cal;
    pLdd_rxpwr->i_rssi= (uint32)I_rssi;
#endif
    if(dbg_en==1) 
        printk(" rtl8291_rxRssiCurrent_get:  RSSIFL = %d(0x%x), RSSIFL_SEL = %d(0x%x), I-rssi = %d, fin I-rssi = %d\n",  
        pLdd_rxpwr->rssi_code1, rssifl, pLdd_rxpwr->data_a, rssifl_sel, pLdd_rxpwr->i_rssi);

    return SUCCESS;
}

int _rtl8291_Irssi_shift(uint32 i_rssi, uint32 *output, uint32 *pShift)
{
    int  zero_bit, bit_shift;
    
    bit_shift = 0;
    zero_bit = 31;

    if (i_rssi <0x10000)
    {
        *output = i_rssi & 0xFFFF;
        *pShift = bit_shift;        
        //printk(KERN_EMERG " bit_shift = %4d zero_bit = %4d output = %6d (0x%4x) \n", *pShift, zero_bit, *output, *output);         
        return 0;        
    }

    while ((i_rssi&(1<<zero_bit))==0)
    {
        //printk(KERN_EMERG " zero_bit = %4d \n", zero_bit);       
        zero_bit--;         
    }

    bit_shift = zero_bit - 15;
    *output =  (i_rssi>>bit_shift)&0xFFFF;
    *pShift = bit_shift;
    //printk(KERN_EMERG " bit_shift = %4d zero_bit = %4d output = %6d (0x%4x) \n", *pShift, zero_bit, *output, *output);    

    return 0;
}

//This API is for uA/32 to 0.1uW translation
int32  _rtl8291_rxCurr2Pwr_get(uint32 devId, rtl8291_rxpwr_t *pLdd_rxpwr)
{
    int32 temp32;
    int64 temp64;
    int32 temp_a, temp_b, temp_c, sum;    
    int32 i_rssi, i_rssi_s;
    int32 rx_a, rx_b, rx_c;
    int32 shft_a, shft_b, shft_c;
    int32 fshft_a, fshft_b, fshft_c;    
    uint32 I_RSSI_SCALE = 0;

    if (pLdd_rxpwr==NULL)
      return FAILED;
    
    if ((pLdd_rxpwr->rx_b==0)||(pLdd_rxpwr->i_rssi==0))
    {
        pLdd_rxpwr->rx_power = 0;
        return SUCCESS;
    }

    //i_rssi is current, unit is uA/32.
    i_rssi = pLdd_rxpwr->i_rssi;
    rx_a  = pLdd_rxpwr->rx_a;
    rx_b  = pLdd_rxpwr->rx_b;
    rx_c  = pLdd_rxpwr->rx_c;

    shft_a  = pLdd_rxpwr->data_a;
    shft_b  = pLdd_rxpwr->data_b;
    shft_c  = pLdd_rxpwr->data_c;    

    //printk(KERN_EMERG "\n\nrx_a = %6d rx_a = %6d rx_a = %6d\n", rx_a, rx_b, rx_c);    
    //printk(KERN_EMERG "shft_a = %6d shft_b = %6d shft_c = %6d\n", shft_a, shft_b, shft_c);

    /* scale down i_rssi to 16 bits for avoiding overflow of a x^2 */
    _rtl8291_Irssi_shift(i_rssi, &i_rssi_s, &I_RSSI_SCALE);
    
    //printk(KERN_EMERG "i_rssi = %6d i_rssi_s = %6d\n", i_rssi, i_rssi_s);

    /* ax^2 */
    temp64 = (int64)(i_rssi_s)*i_rssi_s;
    temp64 = temp64*rx_a;
    //do_div( temp64, 1000);
    fshft_a = shft_a - 2*I_RSSI_SCALE;
    do_div( temp64, (1<<fshft_a));    
    temp_a =(int32)( temp64);

    /* bx */
    temp64 = (int64)(i_rssi_s)*rx_b;
    fshft_b = shft_b - I_RSSI_SCALE;    
    do_div( temp64,  (1<<fshft_b));    
    temp_b =(int32)( temp64);

    /* c */
    fshft_c = shft_c;    
    temp_c = (rx_c)/(1<<fshft_c);

    //printk(KERN_EMERG "temp_a = %6d temp_b = %6d temp_c = %6d\n", temp_a, temp_b, temp_c);

    //20230809: Change sum unit is 0.01uW
    sum = (temp_a+temp_b+temp_c);
    //20230809: Do rounding and change to 0.1uW
    sum = (sum+5)/10;
    if (sum<0)
    {
        sum = 0;
    }
    pLdd_rxpwr->rx_power = sum;

    return SUCCESS;
}

//This unit is uA/32, SD suggestion for HW implement 
int32  _rtl8291_mpdCurrent_get(uint32 devId, rtl8291_txpwr_t *pLdd_txpwr)
{
    int32 temp32;
    int64 temp64;
    uint32 code_mpd, code_txapc, code_250u, code_R_MPD, txapc_rsel, Vlsb, C_lsb;
    uint32 mode, bit_shift;
    int32 I_mpd, I_250u, I_cal;
    int32 code_M, F_shift, scale;
    int    dbg_en;

    dbg_en = _8291_debug_flag(devId);

    if (pLdd_txpwr==NULL)
      return FAILED;


    //According to TCL, fix in 12bit mode. Use 32bit in calculation
    //Current unit is uA/32    
    rtl8291_sdadc_code_get(devId, RTL8291_CODE_TXAPC, &code_txapc);
    rtl8291_sdadc_code_get(devId, RTL8291_CODE_TXAPC_RSEL, &txapc_rsel);

    pLdd_txpwr->rssi_code1 = code_txapc;
    pLdd_txpwr->rssi_code2 = txapc_rsel;

    
    //TCL set CODE_250u 2615     ; typical 250uA MPD K code
    code_250u = 2615*1024;
    //LSB is 4V, transfer to uV, 4000000 = 2^8*5^6can combine with bit shift
    Vlsb = 4*1000000;
    //set R_MPD 500 in TCL ddmi example
    code_R_MPD = 500;
    //ICAL is 250 uA in this version (TCL use 250)
    I_cal = 250;


    //Transfer to 12bit mode
    code_mpd = code_txapc>>10;
    code_250u = code_250u>>10;
    //TCL: set Impd [expr ( 250 - ($CODE_250u-$CODE_MPD)*$LSB/$R_MPD*1000000.0)/$Scale ]        
    //12bit: LSB = 40000000/2^12 
    //Combine LSB/R_MPD and get result c_lsb & bit_shift
    //C_lsb combines LSB&R_MPD so it is 8000 in 12bit mode    
    C_lsb = 8000;         
    bit_shift = 12; //transfer to uA,

    //I_rssi = (code_rssi - code_k)*Vlsb/code_R + i_cal;
    I_mpd = (code_mpd-code_250u)*C_lsb;
    I_250u    = code_250u*C_lsb;
    
    //printk(" _rtl8291_mpdCurrent_get:1 I_mpd = %d I_cal = %d\n", I_mpd, I_cal);    
    //Transfer scale to 2's pow
    if (txapc_rsel==0)
    {
         scale = 2;        
    }
    else
    {
        scale = 0; 
    }

    F_shift = bit_shift + scale;

    //!!!change unit from uA to uA/32, this is to increase accuracy
    scale = scale-5;
    F_shift = F_shift-5;    

    if (scale>=0)
    {
        I_cal  = I_cal>>scale;     
    }
    else
    {
        I_cal  = I_cal<<abs(scale);     
    }
        
    if (F_shift>=0)
    {
        I_mpd = I_mpd>>F_shift;  
        I_250u    = I_250u>>F_shift;        
    }
    else
    {
        I_mpd  = I_mpd<<abs(F_shift); 
        I_250u    = I_250u<<abs(F_shift);        
    }
    
    pLdd_txpwr->data_a = I_mpd;
    pLdd_txpwr->data_b = I_250u;
    pLdd_txpwr->data_c = I_cal;    
    
    //printk(" _rtl8291_mpdCurrent_get:3 I_mpd = %d I_cal = %d\n", I_mpd, I_cal);    
    I_mpd = I_mpd + I_cal;
    pLdd_txpwr->i_mpd = (uint32)I_mpd;

    if(dbg_en==1) 
        printk(" _rtl8291_mpdCurrent_get:  code_txapc = %d(0x%x), txapc_rsel = %d(0x%x), I_mpd = %d (0x%x)\n",code_txapc, pLdd_txpwr->rssi_code1, 
        txapc_rsel, pLdd_txpwr->rssi_code2, I_mpd, pLdd_txpwr->i_mpd);
    
    return SUCCESS;
}

//This unit is uA/32, SD suggestion for HW implement 
int32  rtl8291_txMpdCurrent_get(uint32 devId, rtl8291_txpwr_t *pLdd_txpwr)
{
    int32 temp32;
    int64 temp64;
    uint32 code_mpd, code_txapc, code_250u, code_R_MPD, txapc_rsel, Vlsb, C_lsb;
    uint32 mode, bit_shift;
    int32 I_mpd, I_250u, I_cal, I_fin;
    int32 code_M, F_shift, scale;
    int    dbg_en;

    dbg_en = _8291_debug_flag(devId);

    if (pLdd_txpwr==NULL)
      return FAILED;

    //# I_MPD
    //   GET_A2 102     ; # read TX Power, to update Impd
    //   set DDMI_IMPD_H       [ lindex [GET_RREG 0x6C] ]
    //   set DDMI_IMPD_M       [ lindex [GET_RREG 0x6D] ]
    //   set DDMI_IMPD_L       [ lindex [GET_RREG 0x6E] ]
    
    //   set impd [expr (($DDMI_IMPD_H<<16)|($DDMI_IMPD_M<<8)|($DDMI_IMPD_L))]   
    //   set impd [expr ($impd*0.03125) ]  


    //According to TCL, fix in 12bit mode. Use 32bit in calculation
    //Current unit is uA/32    
    //20230810: Do RTL8290C_CODE_TXAPC_RSEL, and then do RTL8290C_CODE_TXAPC
    rtl8291_sdadc_code_get(devId, RTL8291_CODE_TXAPC_RSEL, &txapc_rsel);
    rtl8291_sdadc_code_get(devId, RTL8291_CODE_TXAPC, &code_txapc);
 
    pLdd_txpwr->rssi_code1 = code_txapc;
    pLdd_txpwr->rssi_code2 = txapc_rsel;

    
    //TCL set CODE_250u 2615     ; typical 250uA MPD K code
    code_250u = pLdd_txpwr->tx_code_250u * 1024;
    //LSB is 4V, transfer to uV, 4000000 = 2^8*5^6can combine with bit shift
    Vlsb = pLdd_txpwr->tx_lsb;
    //set R_MPD 500 in TCL ddmi example
    code_R_MPD = pLdd_txpwr->tx_r_mpd;
    //ICAL is 250 uA in this version (TCL use 250)
    I_cal = 250;

    //Transfer to 12bit mode
    code_mpd = code_txapc>>10;
    code_250u = code_250u>>10;
    //TCL: set Impd [expr ( 250 - ($CODE_250u-$CODE_MPD)*$LSB/$R_MPD*1000000.0)/$Scale ]        
    //12bit: LSB = 40000000/2^12 
    //Combine LSB/R_MPD and get result c_lsb & bit_shift
    //C_lsb combines LSB&R_MPD so it is 8000 in 12bit mode    
    C_lsb = Vlsb/code_R_MPD;         
    bit_shift = 12; //transfer to uA,

    //set Impd [expr ( 250 - ($CODE_250u-$CODE_MPD)*$LSB/$R_MPD*1000000.0)/$Scale ]
    //I_mpd  = (code_mpd-code_250u)*C_lsb;
    I_mpd  = code_mpd*C_lsb;
    I_250u = code_250u*C_lsb;
    
    //printk(" rtl8291_txMpdCurrent_get:1 I_mpd = %d I_cal = %d\n", I_mpd, I_cal);    
    //Transfer scale to 2's pow
    if (txapc_rsel==0)
    {
         scale = 2;        
    }
    else
    {
        scale = 0; 
    }

    F_shift = bit_shift + scale;

    //!!!change unit from uA to uA/32, this is to increase accuracy
    scale = scale-5;
    F_shift = F_shift-5;    

    if (scale>=0)
    {
        I_cal  = I_cal>>scale;     
    }
    else
    {
        I_cal  = I_cal<<abs(scale);     
    }
        
    if (F_shift>=0)
    {
        I_mpd = I_mpd>>F_shift;  
        I_250u    = I_250u>>F_shift;        
    }
    else
    {
        I_mpd  = I_mpd<<abs(F_shift); 
        I_250u    = I_250u<<abs(F_shift);        
    }
    //I_mpd=  CODE_MPD*LSB/R_MPD/(2^final_shift)    
    pLdd_txpwr->data_c = I_mpd;
    //I_250u =  CODE_250u*LSB/R_MPD/(Scale*2^final_shift)
    pLdd_txpwr->data_b = I_250u;
    pLdd_txpwr->data_a = I_cal;    
    
    //printk(" _rtl8291_mpdCurrent_get:3 I_mpd = %d I_cal = %d\n", I_mpd, I_cal);    
    I_fin = I_cal + I_mpd -  I_250u;
    //pLdd_txpwr->i_mpd = (uint32)I_mpd;

    if ((code_txapc == 0)||(I_fin<0))
    {
        pLdd_txpwr->i_mpd = 0;
    }
    else
    {
        pLdd_txpwr->i_mpd = (uint32)I_fin;
    }
    //printk(" rtl8291_txMpdCurrent_get:pLdd_txpwr->tx_r_mpd = %d\n", pLdd_txpwr->tx_r_mpd); 

    if(dbg_en==1) 
        printk(" _rtl8291_txMpdCurrent_get:  code_txapc = %d(0x%x), txapc_rsel = %d(0x%x), I_mpd = %d (0x%x)\n",code_txapc, pLdd_txpwr->rssi_code1, 
        txapc_rsel, pLdd_txpwr->rssi_code2, I_mpd, pLdd_txpwr->i_mpd);
    
    return SUCCESS;
}

//This unit is uA/32, SD suggestion for HW implement 
int32  rtl8291_txMpdCurrentHw_get(uint32 devId, rtl8291_txpwr_t *pLdd_txpwr)
{
    int32 ret = 0;
    uint32 ddmi_txpow, I_mpd;
    uint32 regData_h, regData_m, regData_l;

    //int32 temp32;
    //int64 temp64;
    //uint32 code_mpd, code_txapc, code_250u, code_R_MPD, txapc_rsel, Vlsb, C_lsb;
    //uint32 mode, bit_shift;
    //int32 I_mpd, I_250u, I_cal, I_fin;
    //int32 code_M, F_shift, scale;
    //int    dbg_en;

    //dbg_en = _8291_debug_flag(devId);

    if (pLdd_txpwr==NULL)
      return FAILED;

    ret = rtl8291_getReg(devId, RTL8291_STD_DDMI_TXPOW_H_ADDR, &regData_h);
    if (ret!=0)
        return ret;

    ret = rtl8291_getReg(devId, RTL8291_STD_DDMI_TXPOW_L_ADDR, &regData_l); 
    if (ret!=0)
        return ret;

    ddmi_txpow = ((regData_h&0xFF)<<8)|(regData_l&0xFF);

    //# I_MPD
    //   GET_A2 102     ; # read TX Power, to update Impd
    //   set DDMI_IMPD_H       [ lindex [GET_RREG 0x6C] ]
    //   set DDMI_IMPD_M       [ lindex [GET_RREG 0x6D] ]
    //   set DDMI_IMPD_L       [ lindex [GET_RREG 0x6E] ]
    
    //   set impd [expr (($DDMI_IMPD_H<<16)|($DDMI_IMPD_M<<8)|($DDMI_IMPD_L))]   
    //   set impd [expr ($impd*0.03125) ]  

    ret = rtl8291_getReg(devId, RTL8291_DDMI_IMPD_H_ADDR, &regData_h);
    if (ret!=0)
        return ret;

    ret = rtl8291_getReg(devId, RTL8291_DDMI_IMPD_M_ADDR, &regData_m); 
    if (ret!=0)
        return ret;

    ret = rtl8291_getReg(devId, RTL8291_DDMI_IMPD_L_ADDR, &regData_l); 
    if (ret!=0)
        return ret;    

    //The I_rssi should be the same as rtl8291_rxRssiCurrent_get output
    I_mpd = ((regData_h&0xFF)<<16)|((regData_m&0xFF)<<8)|(regData_l&0xFF);
    pLdd_txpwr->i_mpd = I_mpd;
    pLdd_txpwr->tx_power = ddmi_txpow;
    
    return SUCCESS;
}


//This API is for uA/32 to 0.1uW translation
int32  _rtl8291_txCurr2Pwr_get(uint32 devId, rtl8291_txpwr_t *pLdd_txpwr)
{
    int32 temp32;
    int64 temp64;
    int32 temp_a, temp_b, temp_c, sum;    
    int32 i_mpd, i_mpd_s;
    int32 tx_a, tx_b, tx_c;
    int32 shft_a, shft_b, shft_c;
    int32 fshft_a, fshft_b, fshft_c;    
    uint32 I_MPD_SCALE = 0;

    if (pLdd_txpwr==NULL)
      return FAILED;

    if ((pLdd_txpwr->tx_a==0)||(pLdd_txpwr->i_mpd==0))
    {
        pLdd_txpwr->tx_power = 0;
        return SUCCESS;
    }
    
    //i_rssi is current, unit is uA/32.
    i_mpd = pLdd_txpwr->i_mpd;
    tx_a  = pLdd_txpwr->tx_a;
    tx_b  = pLdd_txpwr->tx_b;
    tx_c  = pLdd_txpwr->tx_c;

    shft_a  = pLdd_txpwr->data_a;
    shft_b  = pLdd_txpwr->data_b;
    shft_c  = pLdd_txpwr->data_c;    

    //printk(KERN_EMERG "\n\nrx_a = %6d rx_a = %6d rx_a = %6d\n", rx_a, rx_b, rx_c);    
    //printk(KERN_EMERG "shft_a = %6d shft_b = %6d shft_c = %6d\n", shft_a, shft_b, shft_c);

    /* scale down i_rssi to 16 bits for avoiding overflow of a x^2 */
    _rtl8291_Irssi_shift(i_mpd, &i_mpd_s, &I_MPD_SCALE);
    
    //printk(KERN_EMERG "i_rssi = %6d i_rssi_s = %6d\n", i_rssi, i_rssi_s);


    /* ax */
    temp64 = (int64)(i_mpd_s)*tx_a;
    fshft_a = shft_a - I_MPD_SCALE;    
    do_div( temp64,  (1<<fshft_a));    
    temp_a =(int32)( temp64);

    /* b */
    fshft_b = shft_b;    
    temp_b = (tx_b)/(1<<fshft_b);

    //printk(KERN_EMERG "temp_a = %6d temp_b = %6d temp_c = %6d\n", temp_a, temp_b, temp_c);

    //20230809: Change sum unit is 0.01uW
    sum = (temp_a+temp_b);
    //20230809: Do rounding and change to 0.1uW
    sum = (sum+5)/10;
    pLdd_txpwr->tx_power = sum;

    return SUCCESS;
}


int32  rtl8291_txPower_get(uint32 devId, rtl8291_txpwr_t *pLdd_txpwr)
{
    int32 cal_mode;

    if (pLdd_txpwr==NULL)
      return FAILED;

    cal_mode  = pLdd_txpwr->op_mode;

    if (cal_mode == RTL8291_TX_CURR2PWR_MODE)
    {
        _rtl8291_txCurr2Pwr_get(devId, pLdd_txpwr);
        return SUCCESS;
    }
    else if (cal_mode == RTL8291_TX_HARDWARE_MODE)
    {
        if (pLdd_txpwr->tx_r_mpd != 0)
        {
            rtl8291_setReg(devId, RTL8291_MPD_R0_LB_ADDR, (pLdd_txpwr->tx_r_mpd&0xFF)); 
            rtl8291_setRegBits(devId, RTL8291_MPD_R_HB_ADDR, RTL8291_MPD_R_HB_MPD_R0_HB_MASK, (pLdd_txpwr->tx_r_mpd&0xF00)>>8);
        }
        if (pLdd_txpwr->tx_code_250u != 0)
        {
            rtl8291_setReg(devId, RTL8291_MPD_K_MSB_ADDR, (pLdd_txpwr->tx_code_250u&0xFF0)>>4); 
            rtl8291_setRegBits(devId, RTL8291_RSSI_K1_MPD_K_LSB_ADDR, RTL8291_RSSI_K1_MPD_K_LSB_MPD_K_LSB_MASK, (pLdd_txpwr->tx_code_250u&0xF));
        }            
        
        rtl8291_txMpdCurrentHw_get(devId, pLdd_txpwr);
        return SUCCESS;
    }    
    else if (cal_mode == RTL8291_TX_REAL_MODE)
    {
        if (pLdd_txpwr->tx_code_250u == 0)
        {
           pLdd_txpwr->tx_code_250u = ldd_param[devId-5].tx_code_250u;
        }
        if (pLdd_txpwr->tx_lsb == 0)
        {
           pLdd_txpwr->tx_lsb = ldd_param[devId-5].tx_lsb;
        }
        if (pLdd_txpwr->tx_r_mpd == 0)
        {
           pLdd_txpwr->tx_r_mpd = ldd_param[devId-5].tx_r_mpd;
        }
        //It is used in DDMI TX Power
        //pLdd_txpwr->tx_lsb = ldd_param[devId-5].tx_lsb;
        //pLdd_txpwr->tx_code_250u = ldd_param[devId-5].tx_code_250u;
        //pLdd_txpwr->tx_r_mpd = ldd_param[devId-5].tx_r_mpd;
        rtl8291_txMpdCurrent_get(devId, pLdd_txpwr);
        return SUCCESS;
    }
    else
    {
        printk(KERN_EMERG "rtl8291_txPower_get cal_mode = 0x%x. No such mode.\n", cal_mode);
        return FAILED;
    }    
    return SUCCESS;
}

int32 rtl8291_config_refresh(uint32 devId)
{
    uint32 addr;
/////struct timespec ts;

    /* Set Digital REG_CONTROL1[4]: FREEZE_DDMI_UPDATE */    
     //rtl8291_setRegBit(devId, RTL8291_REG_CONTROL1_ADDR, RTL8291_REG_CONTROL1_FREEZE_DDMI_UPDATE_OFFSET, 1);

    _rtl8291_load_config_file(devId, flash_data);


    /* for TX/RX DDMI coefficients, Temperature Offset */
    rtl8291_user_parameter_init(devId, &flash_data[RTL8291_PARAMETER_BASE_ADDR], &ldd_param[devId-5]); 

    //printk("[DDMI TX Power]\n");
    //ddmi_8290c_tx_power(devId, &ldd_param[devId-5]);

    //printk("[DDMI RX Power]\n");
    //ddmi_8290c_rx_power(devId, &ldd_param[devId-5]);  

    /* update DDMI A0 */    
    _rtl8291_ddmi_a0_init(devId, flash_data);

    /* update LUTs */
    rtl8291_laserLut_init(devId, flash_data);        

    /* de-freeze DDMI update before kick */
    //rtl8291_setRegBit(devId, RTL8291_REG_CONTROL1_ADDR, RTL8291_REG_CONTROL1_FREEZE_DDMI_UPDATE_OFFSET, 0);

    //Update by BOSA Type
    //addr = RTL8290B_PARAM_ADDR_BASE+RTL8290B_BOSA_TYPE_OFFSET_ADDR;
    //rtl8290b_booster_set(flash_data[addr]);
    //rtl8290b_dac_set(flash_data[addr]); 

    return SUCCESS;
}


//Target Power (average mpd current): 00000000~11000100 25u~1943u
//pseudo log formula: Iavg = 25u * 2^(Iavg_set / 32)
//ps. code 194~255 are invalid
int32  rtl8291_apcIavg_set(uint32 devId, uint32 value)
{
    //uint32 regData_h, regData_l;
    if (value>0xFF)
      return FAILED;

    rtl8291_setRegBits(devId, RTL8291_WB_REG71_ADDR, RTL8291_WB_REG71_REG_APCDIG_IAVG_DAC_7_0_MASK, value);
    //regData_h = value>>1;
    //regData_l = value&0x1;    
  
    //rtl8291_setReg(devId, RTL8291_REG_DAC_HB_ADDR, regData_h);    
    //rtl8291_setReg(devId, RTL8291_REG_DAC_LB_ADDR, regData_l); 

    return SUCCESS;
}

int32  rtl8291_apcIavg_get(uint32 devId, uint32 *pValue)
{
    rtl8291_getRegBits(devId, RTL8291_WB_REG71_ADDR, RTL8291_WB_REG71_REG_APCDIG_IAVG_DAC_7_0_MASK, pValue);

    return SUCCESS;
}

//OMA /Pavg set
//000000: 0.72 (3.27dB)
//000001: 0.74 (3.37dB)
//    :
//111111: 1.98 (22.99dB)
//step: 0.02
int32  rtl8291_apcEr_set(uint32 devId, uint32 value)
{
    if (value>0x3F)
      return FAILED;

    rtl8291_setRegBits(devId, RTL8291_WB_REG70_ADDR, RTL8291_WB_REG70_REG_APCDIG_OMA_SET_5_0_MASK, value);

    return SUCCESS;
}

int32  rtl8291_apcEr_get(uint32 devId, uint32 *pValue)
{
    rtl8291_getRegBits(devId, RTL8291_WB_REG70_ADDR, RTL8291_WB_REG70_REG_APCDIG_OMA_SET_5_0_MASK, pValue);

    return SUCCESS;
}

#if 0 //NA in RTL8291
//ER trimming
//0: -10%  1:-9.375% ¡K 14:-1.25%  15:-0.625%  16:0%
//17:0.625%  18:1.25% ¡K 30: 8.75%  31: 9.375%
int32  rtl8291_apcErTrim_set(uint32 devId, uint32 value)
{
    if (value>0x1F)
      return FAILED;

    rtl8291_setRegBits(devId, RTL8290C_WB_REG4D_ADDR, RTL8290C_WB_REG4D_REG_APCDIG_ER_TRIM_4_0_MASK, value);

    return SUCCESS;
}

int32  rtl8291_apcErTrim_get(uint32 devId, uint32 *pValue)
{
    rtl8291_getRegBits(devId, RTL8290C_WB_REG4D_ADDR, RTL8290C_WB_REG4D_REG_APCDIG_ER_TRIM_4_0_MASK, pValue);

    return SUCCESS;
}
#endif

//Input capacitance of REF TIA 
//000:  3.125pF
//001:  6.2500pF
//010:  9.375pF
//011: 12.500pF
//100: 15.625pF
//101: 18.750pF
//110: 21.875pF
//111: 25.000pF
int32  rtl8291_apcCmpd_set(uint32 devId, uint32 value)
{
    if (value>0x7)
      return FAILED;

    rtl8291_setRegBits(devId, RTL8291_WB_REG72_ADDR, RTL8291_WB_REG72_REG_APCDIG_REF_CMPD_2_0_MASK, value);

    return SUCCESS;
}

int32  rtl8291_apcCmpd_get(uint32 devId, uint32 *pValue)
{
    rtl8291_getRegBits(devId, RTL8291_WB_REG72_ADDR, RTL8291_WB_REG72_REG_APCDIG_REF_CMPD_2_0_MASK, pValue);

    return SUCCESS;
}

//LPF bandwidth: 
//00: 160MHz  01: 80MHz  10: 40MHz  11: 20MHz

int32  rtl8291_apcLpfBw_set(uint32 devId, uint32 value)
{
    if (value>0x3)
      return FAILED;

    rtl8291_setRegBits(devId, RTL8291_WB_REG6C_ADDR, RTL8291_WB_REG6C_REG_APCDIG_LPF_BW_1_0_MASK, value);

    return SUCCESS;
}

int32  rtl8291_apcLpfBw_get(uint32 devId, uint32 *pValue)
{
    rtl8291_getRegBits(devId, RTL8291_WB_REG6C_ADDR, RTL8291_WB_REG6C_REG_APCDIG_LPF_BW_1_0_MASK, pValue);

    return SUCCESS;
}

//1: Enable eye crossing adjustment
int32  rtl8291_txCrossEn_set(uint32 devId, uint32 enable)
{
    if (enable>0x1)
      return FAILED;

    rtl8291_setRegBit(devId, RTL8291_WB_REG56_ADDR, RTL8291_WB_REG56_EYEX_LEVEL_ENL_OFFSET, enable);

    return SUCCESS;
}

int32  rtl8291_txCrossEn_get(uint32 devId, uint32 *pEnable)
{
   rtl8291_getRegBit(devId, RTL8291_WB_REG56_ADDR, RTL8291_WB_REG56_EYEX_LEVEL_ENL_OFFSET, pEnable);
 
    return SUCCESS;
}

//Eyes crossing coarse adjustment direction 0: up 1: down
//Eyes crossing coarse adjustment level control 00: 4ps 01: 8ps 10: 12ps 11: 16ps
int32  rtl8291_txCrossAdj_set(uint32 devId, uint32 sign, uint32 str)
{
    if ((sign>0x1)||(str>0x3))
      return FAILED;

    rtl8291_setRegBit(devId, RTL8291_WB_REG56_ADDR, RTL8291_WB_REG56_EYEX_LEVEL_CROSS_DIR_L_OFFSET, sign);
    rtl8291_setRegBits(devId, RTL8291_WB_REG56_ADDR, RTL8291_WB_REG56_EYEX_LEVEL_CROSS_CTRL_L_1_0_MASK, str);

    return SUCCESS;
}

int32  rtl8291_txCrossAdj_get(uint32 devId, uint32 *pSign, uint32 *pStr)
{
    rtl8291_getRegBit(devId, RTL8291_WB_REG56_ADDR, RTL8291_WB_REG56_EYEX_LEVEL_CROSS_DIR_L_OFFSET, pSign);
    rtl8291_getRegBits(devId, RTL8291_WB_REG56_ADDR, RTL8291_WB_REG56_EYEX_LEVEL_CROSS_CTRL_L_1_0_MASK, pStr);

    return SUCCESS;
}

//Input is raw data for BIAS/MOD
int32  rtl8291_apcImod_biasInitCode_set(uint32 devId, uint32 mod_value, uint32 bias_value)
{
    uint32 data1, data2, data3;

    if (mod_value>0xFFF)
      return FAILED;
    if (bias_value>0xFFF)
      return FAILED;

    printk(KERN_EMERG " rtl8291_apcImod_biasInitCode_set: mod_value = 0x%x, bias_value = 0x%x\n", mod_value, bias_value);


    rtl8291_setRegBit(devId, RTL8291_WB_REG76_ADDR, RTL8291_WB_REG76_REG_APCDIG_CODE_LOADIN_OFFSET, 0); 

    rtl8291_setRegBits(devId, RTL8291_WB_REG74_ADDR, RTL8291_WB_REG74_REG_APCDIG_MOD_INI_10_4_MASK, (mod_value&0x7F0)>>4);
    rtl8291_setRegBits(devId, RTL8291_WB_REG75_ADDR, RTL8291_WB_REG75_REG_APCDIG_MOD_INI_3_0_MASK, (mod_value&0xF));

    rtl8291_setRegBits(devId, RTL8291_WB_REG73_ADDR, RTL8291_WB_REG73_REG_APCDIG_BIAS_INI_10_4_MASK, (bias_value&0x7F0)>>4);
    rtl8291_setRegBits(devId, RTL8291_WB_REG75_ADDR, RTL8291_WB_REG75_REG_APCDIG_BIAS_INI_3_0_MASK, (bias_value&0xF));

    rtl8291_setRegBit(devId, RTL8291_WB_REG76_ADDR, RTL8291_WB_REG76_REG_APCDIG_CODE_LOADIN_OFFSET, 1);

    rtl8291_getReg(devId, RTL8291_WB_REG73_ADDR, &data1);
    rtl8291_getReg(devId, RTL8291_WB_REG74_ADDR, &data2);
    rtl8291_getReg(devId, RTL8291_WB_REG75_ADDR, &data3);
    printk(KERN_EMERG " rtl8291_apcImod_biasInitCode_set: W73 = 0x%x, W74 = 0x%x, W75 = 0x%x\n", data1, data2, data3);

    return SUCCESS;
}

/*
 * BiasCurrent: 120mA max for 11bits DAC, input mod unit is uA.  
 */
int32  rtl8291_tx_bias_set(uint32 devId, uint32 bias)
{
    uint16 value;
    uint32 bias_sys;
    uint32 mod, mod_sys;
    uint16 value_mod;

    //if (50000<bias)
    //{
    //    printk(KERN_EMERG " rtl8290b_tx_bias_set: Invalid bias current.\n");
    //    return FAILED;
    //}

    //if (bias == 50000)
    //{
    //    bias_sys = 0xFFF;
    //}
    //else
    //{
        bias_sys = (bias * 2047) / (120*1000); 
    //}

    //RL6733 TCL: set Ibias [expr ( ( $R07_0<<4 | $R09_0>>4 )) * 120/(pow(2,11)-1)]

    rtl8291_tx_mod_get(devId, &mod);
    mod_sys = (mod * 2047) / (120*1000); 
    value_mod = mod_sys&0x7FF;

    value = bias_sys&0x7FF;
    //printk(" rtl8290b_tx_bias_set: mod_sys = %d, bias_sys = %d\n", mod_sys, bias_sys);   
    rtl8291_apcImod_biasInitCode_set(devId, value_mod, value);

    return SUCCESS;
}


/*
 * ModCurrent: 120mA max for 11bits DAC, input mod unit is uA. 
 */
int32  rtl8291_tx_mod_set(uint32 devId, uint32 mod)
{
    uint16 value;
    uint32 mod_sys;
    uint32 bias, bias_sys;
    uint16 value_bias;

    //if (50000<mod) 
    //{
    //    printk(KERN_EMERG " rtl8290b_tx_mod_set: Invalid mod current.\n");
    //    return FAILED;
    //}

    //if (mod == 50000)
    //{
    //    mod_sys = 0x3FF;
    //}
    //else
    //{
        mod_sys = (mod * 2047) / (120*1000); 
    //}

    //RL6733 TCL: set Imod [expr ( ($R08_0<<4 | $R09_0)*120/(pow(2,11)-1) )]
    rtl8291_tx_bias_get(devId, &bias);
    bias_sys = (bias * 2047) / (120*1000);
    value_bias = bias_sys&0x7FF;

    value = mod_sys&0x7FF;
    //printk(" rtl8290b_tx_mod_set: mod_sys = %d, bias_sys = %d\n", mod_sys, bias_sys);    
    //rtl8290b_apcImodInitCode_set(value);
    rtl8291_apcImod_biasInitCode_set(devId, value, value_bias);

    return SUCCESS;
}

/*
 * BiasCurrent: 120mA max for 11bits DAC, bias unit is uA
 */
int32  rtl8291_tx_bias_get(uint32 devId, uint32 *bias)
{
    uint32 regData1, regData2;
    uint32 bias_sys;

    //tx bias translation function
    rtl8291_getRegBits(devId, RTL8291_RB_REG07_ADDR, RTL8291_RB_REG07_REG_APCDIG_RO_BIAS_10_4_MASK, &regData1);
    rtl8291_getRegBits(devId, RTL8291_RB_REG09_ADDR, RTL8291_RB_REG09_REG_APCDIG_RO_BIAS_3_0_MASK, &regData2);
    bias_sys = (regData1 << 4) | regData2;
    //RL6733 TCL: set Ibias [expr ( ( $R07_0<<4 | $R09_0>>4 )) * 120/(pow(2,11)-1)]    
    *bias = ((bias_sys&0x7FF)*(120*1000))>>11;

    //printk(KERN_EMERG " rtl8291_tx_bias_get: ASIC Internal Value = 0x%x\n", bias_sys);

    return SUCCESS;
}

/*
 * ModCurrent: 120mA max for 11bits DAC, mod unit is uA
 */
int32  rtl8291_tx_mod_get(uint32 devId, uint32 *mod)
{
    uint32 regData1, regData2;
    uint32 mod_sys;

    //tx mod translation function
    rtl8291_getRegBits(devId, RTL8291_RB_REG08_ADDR, RTL8291_RB_REG08_REG_APCDIG_RO_MOD_10_4_MASK, &regData1);
    rtl8291_getRegBits(devId, RTL8291_RB_REG09_ADDR, RTL8291_RB_REG09_REG_APCDIG_RO_MOD_3_0_MASK, &regData2);
    mod_sys = (regData1 << 4) | regData2;
    //RL6733 TCL: set Imod [expr ( ($R08_0<<4 | $R09_0)*120/(pow(2,11)-1) )]    
    *mod = ((mod_sys&0x7FF)*(120*1000))>>11;

    //printk(KERN_EMERG " rtl8290b_tx_mod_get: ASIC Internal Value = 0x%x\n", mod_sys);

    return SUCCESS;
}

//Assert refrence DAC code
int32  rtl8291_rxlosRefDac_set(uint32 devId, uint32 code)
{    
    if (code>0x7F)
      return FAILED;

    rtl8291_setRegBits(devId, RTL8291_WB_REG2E_ADDR, RTL8291_WB_REG2E_REG_RXLOS_DAC_CODE_L_6_0_MASK, code);

    return SUCCESS;
}

int32  rtl8291_rxlosRefDac_get(uint32 devId, uint32 *pCode)
{
    rtl8291_getRegBits(devId, RTL8291_WB_REG2E_ADDR, RTL8291_WB_REG2E_REG_RXLOS_DAC_CODE_L_6_0_MASK, pCode);
    
    return SUCCESS;
}

// De-assert threshold selector (electrical)
//0: 1dB,  1: 2dB,    2: 3dB,  3: 4dB, 4: 5dB,  5: 6dB,  6: 7dB,    7: 8dB
// W38[2:0] REG_DEASSERT_SETL<2:0>
int32  rtl8291_rxlosHystSel_set(uint32 devId, uint32 value)
{
    if (value>0x7)
        return FAILED;
           
    rtl8291_setRegBits(devId, RTL8291_WB_REG2D_ADDR, RTL8291_WB_REG2D_REG_RXLOS_DEASSERT_SEL_L_2_0_MASK, value);   
        return SUCCESS;
}
           
int32  rtl8291_rxlosHystSel_get(uint32 devId, uint32 *pValue)
{
    rtl8291_getRegBits(devId, RTL8291_WB_REG2D_ADDR, RTL8291_WB_REG2D_REG_RXLOS_DEASSERT_SEL_L_2_0_MASK, pValue);
    return SUCCESS;
}




#if 1

//TX pre-driver LDO09_2 voltage selection 00: 0.85V 01: 0.90V 10: 0.95V 11: 1.00V 
int32  rtl8291_apcLdo_set(uint32 devId, uint32 apc_ldo)
{
    if (apc_ldo > 0x7)
      return FAILED;

    rtl8291_setRegBits(devId, RTL8291_WB_REG52_ADDR, RTL8291_WB_REG52_LDO09_2_SEL_L_1_0_MASK, apc_ldo);
 
    return SUCCESS;
}

int32  rtl8291_apcLdo_get(uint32 devId, uint32 *pApc_ldo)
{
    rtl8291_getRegBits(devId, RTL8291_WB_REG52_ADDR, RTL8291_WB_REG52_LDO09_2_SEL_L_1_0_MASK, pApc_ldo);
 
    return SUCCESS;
}


//TX pre-driver LDO09 voltage selection 00: 0.85V 01: 0.90V 10: 0.95V 11: 1.00V 
int32  rtl8291_txLdo_set(uint32 devId, uint32 tx_ldo)
{
    if (tx_ldo > 0x7)
      return FAILED;

    rtl8291_setRegBits(devId, RTL8291_WB_REG51_ADDR, RTL8291_WB_REG51_LDO09_SEL_L_1_0_MASK, tx_ldo);
 
    return SUCCESS;
}

int32  rtl8291_txLdo_get(uint32 devId, uint32 *pTx_ldo)
{
    rtl8291_getRegBits(devId, RTL8291_WB_REG51_ADDR, RTL8291_WB_REG51_LDO09_SEL_L_1_0_MASK, pTx_ldo);
 
    return SUCCESS;
}

//RX LDO, 1430mV~1700mV (40mV step), default=b100, 1590mV
int32  rtl8291_rxLdo_set(uint32 devId, uint32 rx_ldo)
{
    if (rx_ldo > 0x7)
      return FAILED;

    rtl8291_setRegBits(devId, RTL8291_WB_REG03_ADDR, RTL8291_WB_REG03_REF_L_2_0_MASK, rx_ldo);
 
    return SUCCESS;
}

int32  rtl8291_rxLdo_get(uint32 devId, uint32 *pRx_ldo)
{
    rtl8291_getRegBits(devId, RTL8291_WB_REG03_ADDR, RTL8291_WB_REG03_REF_L_2_0_MASK, pRx_ldo);
 
    return SUCCESS;
}

#if 0
//1: enable TX bleeding on lasern, 0: disable
int32  rtl8291_txBleedN_set(uint32 devId, uint32 enable)
{
    if (enable > 1)
      return FAILED;

    rtl8291_setRegBit(devId, RTL8290C_WB_REG65_ADDR, RTL8290C_WB_REG65_TX_EN_BLEEDN_OFFSET, enable);
 
    return SUCCESS;
}

int32  rtl8291_txBleedN_get(uint32 devId, uint32 *pEnable)
{
    rtl8291_getRegBit(devId, RTL8290C_WB_REG65_ADDR, RTL8290C_WB_REG65_TX_EN_BLEEDN_OFFSET, pEnable);
 
    return SUCCESS;
}

//1: enable TX bleeding on laserp, 0: disable
int32  rtl8291_txBleedP_set(uint32 devId, uint32 enable)
{
    if (enable > 1)
      return FAILED;

    rtl8291_setRegBit(devId, RTL8290C_WB_REG65_ADDR, RTL8290C_WB_REG65_TX_EN_BLEEDP_OFFSET, enable);
 
    return SUCCESS;
}

int32  rtl8291_txBleedP_get(uint32 devId, uint32 *pEnable)
{
    rtl8291_getRegBit(devId, RTL8290C_WB_REG65_ADDR, RTL8290C_WB_REG65_TX_EN_BLEEDP_OFFSET, pEnable);
 
    return SUCCESS;
}

//type 0: adjust over/under shoot seperately, 1: adjust over/under shoot simutanously
//TX back-termination fine tune (0x00=disable)
int32  rtl8291_txBackTerm_set(uint32 devId, uint32 type, uint32 value)
{
    if ((type > 1)||(value>0x3F))
      return FAILED;

    rtl8291_setRegBit(devId, RTL8290C_WB_REG66_ADDR, RTL8290C_WB_REG66_TX_PBT_TYP_OFFSET, type);
    rtl8291_setRegBits(devId, RTL8290C_WB_REG66_ADDR, RTL8290C_WB_REG66_TX_BACK_TERM_5_0_MASK, value);

    return SUCCESS;
}

int32  rtl8291_txBackTerm_get(uint32 devId, uint32 *pType, uint32 *pValue)
{
    rtl8291_getRegBit(devId, RTL8290C_WB_REG66_ADDR, RTL8290C_WB_REG66_TX_PBT_TYP_OFFSET, pType);
    rtl8291_getRegBits(devId, RTL8290C_WB_REG66_ADDR, RTL8290C_WB_REG66_TX_BACK_TERM_5_0_MASK, pValue);
 
    return SUCCESS;
}

//"0: DC coupling, CML mode, 50 Ohm to VDD_TX2, no vicm bias
//1: DC coupling, Diff mode, 100 Ohm to each other, no vicm bias
//2: AC coupling, Diff mode, 100 Ohm to each other, with vicm bias
//3: DC coupling, LVPECL mode, No internal resistor for matching, no vicm bias, require external 130/82 ohm resistors"

int32  rtl8291_txInterface_set(uint32 devId, uint32 value)
{
    uint32 mask;
    if (value>0x3)
      return FAILED;

    mask = RTL8290C_WB_REG63_TX_INMODE_SEL0_MASK|RTL8290C_WB_REG63_TX_INMODE_SEL1_MASK;
    rtl8291_setRegBits(devId, RTL8290C_WB_REG63_ADDR, mask, value);

    return SUCCESS;
}

int32  rtl8291_txInterface_get(uint32 devId, uint32 *pValue)
{
    uint32 mask;

    mask = RTL8290C_WB_REG63_TX_INMODE_SEL0_MASK|RTL8290C_WB_REG63_TX_INMODE_SEL1_MASK;
    rtl8291_getRegBits(devId, RTL8290C_WB_REG63_ADDR, mask, pValue);
 
    return SUCCESS;
}

int32  rtl8291_txIntenalRlpn_set(uint32 devId, uint32 value)
{
    if (value>0x3)
      return FAILED;

    rtl8291_setRegBits(devId, RTL8290C_WB_REG65_ADDR, RTL8290C_WB_REG65_TX_RPN_1_0_MASK, value);

    return SUCCESS;
}

int32  rtl8291_txIntenalRlpn_get(uint32 devId, uint32 *pValue)
{
   rtl8291_getRegBits(devId, RTL8290C_WB_REG65_ADDR, RTL8290C_WB_REG65_TX_RPN_1_0_MASK, pValue);
 
    return SUCCESS;
}

//TX BIAS DAC accuracy fine tune offset
int32  rtl8291_txBiasDac_set(uint32 devId, uint32 value)
{
    if (value>0x3)
      return FAILED;

    rtl8291_setRegBits(devId, RTL8290C_WB_REG64_ADDR, RTL8290C_WB_REG64_TX_OFFSET_BIAS_1_0_MASK, value);

    return SUCCESS;
}

int32  rtl8291_txBiasDac_get(uint32 devId, uint32 *pValue)
{
    rtl8291_getRegBits(devId, RTL8290C_WB_REG64_ADDR, RTL8290C_WB_REG64_TX_OFFSET_BIAS_1_0_MASK, pValue);

    return SUCCESS;
}

//TX MOD DAC accuracy fine tune offset
int32  rtl8291_txModDac_set(uint32 devId, uint32 value)
{
    if (value>0xF)
      return FAILED;

    rtl8291_setRegBits(devId, RTL8290C_WB_REG64_ADDR, RTL8290C_WB_REG64_TX_OFFSET_MOD_3_0_MASK, value);

    return SUCCESS;
}

int32  rtl8291_txModDac_get(uint32 devId, uint32 *pValue)
{
    rtl8291_getRegBits(devId, RTL8290C_WB_REG64_ADDR, RTL8290C_WB_REG64_TX_OFFSET_MOD_3_0_MASK, pValue);

    return SUCCESS;
}

//1: Enable DCD calibration
int32  rtl8291_txDcdCalEn_set(uint32 devId, uint32 enable)
{
    if (enable>0x1)
      return FAILED;

    rtl8291_setRegBit(devId, RTL8290C_WB_REG69_ADDR, RTL8290C_WB_REG69_TX_DCD_CAL_EN_OFFSET, enable);

    return SUCCESS;
}

int32 rtl8291_txDcdCalEn_get(uint32 devId, uint32 *pEnable)
{
    rtl8291_getRegBit(devId, RTL8290C_WB_REG69_ADDR, RTL8290C_WB_REG69_TX_DCD_CAL_EN_OFFSET, pEnable);

    return SUCCESS;
}
#endif

//1: Swap TXI P/N polarity
int32  rtl8291_txInPolSwap_set(uint32 devId, uint32 value)
{
    if (value>0x1)
      return FAILED;

    rtl8291_setRegBit(devId, RTL8291_WB_REG53_ADDR, RTL8291_WB_REG53_TX_IN_SWAPL_OFFSET, value);

    return SUCCESS;
}

int32 rtl8291_txInPolSwap_get(uint32 devId, uint32 *pValue)
{
    rtl8291_getRegBit(devId, RTL8291_WB_REG53_ADDR, RTL8291_WB_REG53_TX_IN_SWAPL_OFFSET, pValue);

    return SUCCESS;
}

//1: Swap Laser P/N polarity
int32 rtl8291_txOutPolSwap_set(uint32 devId, uint32 value)
{
    if (value>0x1)
      return FAILED;

    rtl8291_setRegBit(devId, RTL8291_WB_REG53_ADDR, RTL8291_WB_REG53_TX_OUT_SWAPL_OFFSET, value);

    return SUCCESS;
}

int32 rtl8291_txOutPolSwap_get(uint32 devId, uint32 *pValue)
{
    rtl8291_getRegBit(devId, RTL8291_WB_REG53_ADDR, RTL8291_WB_REG53_TX_OUT_SWAPL_OFFSET, pValue);

    return SUCCESS;
}

//TX LASER ON DELAY TIME (APC/ERC will start after this delay time when TX LASER ON) 
//000: 0ns, 001: 16ns, 010: 32ns, 011: 64ns, 100: 128ns, 101: 256ns, 110: 512ns, 111: 1024ns
int32 rtl8291_apcLaserOnDelay_set(uint32 devId, uint32 value)
{
    if (value>0x7)
      return FAILED;

    rtl8291_setRegBits(devId, RTL8291_WB_REG78_ADDR, RTL8291_WB_REG78_REG_APCDIG_TX_LASER_ON_DELAY_TIME_2_0_MASK, value);

    return SUCCESS;
}

int32 rtl8291_apcLaserOnDelay_get(uint32 devId, uint32 *pValue)
{
    rtl8291_getRegBits(devId, RTL8291_WB_REG78_ADDR, RTL8291_WB_REG78_REG_APCDIG_TX_LASER_ON_DELAY_TIME_2_0_MASK, pValue);

    return SUCCESS;
}

//APC/ERC settling period, 62.5MHz CLK
//00: 2  (32ns), 01: 4  (64ns), 10: 8 (128ns), 11: 16 (256ns)
int32 rtl8291_apcSettleCnt_set(uint32 devId, uint32 value)
{
    if (value>0x3)
      return FAILED;

    rtl8291_setRegBits(devId, RTL8291_WB_REG79_ADDR, RTL8291_WB_REG79_REG_APCDIG_SETTLE_COUNT_1_0_MASK, value);

    return SUCCESS;
}

int32 rtl8291_apcSettleCnt_get(uint32 devId, uint32 *pValue)
{
    rtl8291_getRegBits(devId, RTL8291_WB_REG79_ADDR, RTL8291_WB_REG79_REG_APCDIG_SETTLE_COUNT_1_0_MASK, pValue);

    return SUCCESS;
}

int32 rtl8291_apcIbiasInitCode_set(uint32 devId, uint32 value)
{
    if (value>0x7FF)
      return FAILED;

    rtl8291_setRegBit(devId, RTL8291_WB_REG76_ADDR, RTL8291_WB_REG76_REG_APCDIG_CODE_LOADIN_OFFSET, 0); 
    rtl8291_setRegBits(devId, RTL8291_WB_REG73_ADDR, RTL8291_WB_REG73_REG_APCDIG_BIAS_INI_10_4_MASK, (value&0x7F0)>>4);
    rtl8291_setRegBits(devId, RTL8291_WB_REG75_ADDR, RTL8291_WB_REG75_REG_APCDIG_BIAS_INI_3_0_MASK, (value&0xF));
    rtl8291_setRegBit(devId, RTL8291_WB_REG76_ADDR, RTL8291_WB_REG76_REG_APCDIG_CODE_LOADIN_OFFSET, 1);
    return SUCCESS;
}

int32 rtl8291_apcIbiasInitCode_get(uint32 devId, uint32 *pValue)
{
    uint32 data1,data2;
    //W54[7:0] REG_APCDIG_BIAS_INI<11:4>W56[3:0] REG_APCDIG_BIAS_INI<3:0>
    rtl8291_getRegBits(devId, RTL8291_WB_REG73_ADDR, RTL8291_WB_REG73_REG_APCDIG_BIAS_INI_10_4_MASK, &data1);
    rtl8291_getRegBits(devId, RTL8291_WB_REG75_ADDR, RTL8291_WB_REG75_REG_APCDIG_BIAS_INI_3_0_MASK, &data2);
    *pValue = ((data1&0xFF)<<4)|(data2&0xF);

    return SUCCESS;
}

int32 rtl8291_apcImodInitCode_set(uint32 devId, uint32 value)
{
    if (value>0x7FF)
      return FAILED;

    rtl8291_setRegBit(devId, RTL8291_WB_REG76_ADDR, RTL8291_WB_REG76_REG_APCDIG_CODE_LOADIN_OFFSET, 0);
    rtl8291_setRegBits(devId, RTL8291_WB_REG74_ADDR, RTL8291_WB_REG74_REG_APCDIG_MOD_INI_10_4_MASK, (value&0x7F0)>>4);
    rtl8291_setRegBits(devId, RTL8291_WB_REG75_ADDR, RTL8291_WB_REG75_REG_APCDIG_MOD_INI_3_0_MASK, (value&0xF));
    rtl8291_setRegBit(devId, RTL8291_WB_REG76_ADDR, RTL8291_WB_REG76_REG_APCDIG_CODE_LOADIN_OFFSET, 1);
    return SUCCESS;
}

int32 rtl8291_apcImodInitCode_get(uint32 devId, uint32 *pValue)
{
    uint32 data1,data2;
    //W54[7:0] REG_APCDIG_BIAS_INI<11:4>W56[3:0] REG_APCDIG_BIAS_INI<3:0>
    rtl8291_getRegBits(devId, RTL8291_WB_REG74_ADDR, RTL8291_WB_REG74_REG_APCDIG_MOD_INI_10_4_MASK, &data1);
    rtl8291_getRegBits(devId, RTL8291_WB_REG75_ADDR, RTL8291_WB_REG75_REG_APCDIG_MOD_INI_3_0_MASK, &data2);
    *pValue = ((data1&0xFF)<<4)|(data2&0xF);

    return SUCCESS;
}

//Loop mode
//000: DOL, 001: APC_B, 010: APC_M, 011:APC and ERC timing division
//100: DOL, 101: SCL_B, 110: ERC only, 111: APC and ERC simultaneously
int32  rtl8291_apcLoopMode_set(uint32 devId, uint32 mode)
{   
    if (mode>0x7)
      return FAILED;  

    rtl8291_setRegBits(devId, RTL8291_WB_REG76_ADDR, RTL8291_WB_REG76_REG_APCDIG_LOOPMODE_2_0_MASK, mode);

    return SUCCESS;
}

//Loop mode
//000: DOL, 001: APC_B, 010: APC_M, 011:APC and ERC timing division
//100: DOL, 101: SCL_B, 110: ERC only, 111: APC and ERC simultaneously
int32  rtl8291_apcLoopMode_get(uint32 devId, uint32 *pMode)
{

    rtl8291_getRegBits(devId, RTL8291_WB_REG76_ADDR, RTL8291_WB_REG76_REG_APCDIG_LOOPMODE_2_0_MASK, pMode);

    return SUCCESS;
}



//APC digital loop filter gain, accumulation mode, 25MHz CLK (min. integration 160ns~20.48us)
//0~7: 2^-2~2^-9
int32  rtl8291_apcApcLoopGain_set(uint32 devId, uint32 value)
{
    if (value>0x7)
      return FAILED;

    rtl8291_setRegBits(devId, RTL8291_WB_REG77_ADDR, RTL8291_WB_REG77_REG_APCDIG_APC_LOOPGAIN_2_0_MASK, value);

    return SUCCESS;
}

int32  rtl8291_apcApcLoopGain_get(uint32 devId, uint32 *pValue)
{
    rtl8291_getRegBits(devId, RTL8291_WB_REG77_ADDR, RTL8291_WB_REG77_REG_APCDIG_APC_LOOPGAIN_2_0_MASK, pValue);

    return SUCCESS;
}

//ERC digital loop filter gain, accumulation mode, 25MHz CLK (min. integration 160ns~20.48us)
//0~7: 2^-2~2^-9
int32  rtl8291_apcErcLoopGain_set(uint32 devId, uint32 value)
{
    if (value>0x7)
      return FAILED;

    rtl8291_setRegBits(devId, RTL8291_WB_REG78_ADDR, RTL8291_WB_REG78_REG_APCDIG_ERC_LOOPGAIN_2_0_MASK, value);

    return SUCCESS;
}

int32  rtl8291_apcErcLoopGain_get(uint32 devId, uint32 *pValue)
{
    rtl8291_getRegBits(devId, RTL8291_WB_REG78_ADDR, RTL8291_WB_REG78_REG_APCDIG_ERC_LOOPGAIN_2_0_MASK, pValue);

    return SUCCESS;
}

int32  rtl8291_apcErcChopperEn_set(uint32 devId, uint32 enable)
{
    if (enable>0x1)
      return FAILED;

    rtl8291_setRegBit(devId, RTL8291_WB_REG87_ADDR, RTL8291_WB_REG87_REG_APCDIG_ERC_CHOP_EN_OFFSET, enable);
    //The following is not defined in 6097 TCL    
    //rtl8291_setRegBits(devId, RTL8290C_WB_REG5C_ADDR, RTL8290C_WB_REG5C_REG_APCDIG_ERC_CHOP_SEL_1_0_MASK, enable);
    //rtl8291_setRegBit(devId, RTL8290C_WB_REG5C_ADDR, RTL8290C_WB_REG5C_REG_APCDIG_ERC_CAL_SEL_OFFSET, enable);

    return SUCCESS;
}

int32  rtl8291_apcErcChopperEn_get(uint32 devId, uint32 *pEnable)
{
    rtl8291_getRegBit(devId, RTL8291_WB_REG87_ADDR, RTL8291_WB_REG87_REG_APCDIG_ERC_CHOP_EN_OFFSET, pEnable);

    return SUCCESS;
}

//1: Enable APC crossing compensation
int32  rtl8291_apcCrossEn_set(uint32 devId, uint32 enable)
{
    if (enable>0x1)
      return FAILED;

    rtl8291_setRegBit(devId, RTL8291_WB_REG67_ADDR, RTL8291_WB_REG67_APC_EYEX_LEVEL_ENL_OFFSET, enable);
    return SUCCESS;
}

int32  rtl8291_apcCrossEn_get(uint32 devId, uint32 *pEnable)
{
    rtl8291_getRegBit(devId, RTL8291_WB_REG67_ADDR, RTL8291_WB_REG67_APC_EYEX_LEVEL_ENL_OFFSET, pEnable);
    return SUCCESS;
}


//Eyes crossing coarse adjustment direction 0: up 1: down
//Eyes crossing coarse adjustment level control 00: 4ps 01: 8ps 10: 12ps 11: 16ps
int32  rtl8291_apcCrossAdj_set(uint32 devId, uint32 sign, uint32 str)
{
    if ((sign>0x1)||(str>0x3))
      return FAILED;

    rtl8291_setRegBit(devId, RTL8291_WB_REG67_ADDR, RTL8291_WB_REG67_APC_EYEX_LEVEL_CROSS_DIR_L_OFFSET, sign);
    rtl8291_setRegBits(devId, RTL8291_WB_REG67_ADDR, RTL8291_WB_REG67_APC_EYEX_LEVEL_CROSS_CTRL_L_1_0_MASK, str);

    return SUCCESS;
}

int32  rtl8291_apcCrossAdj_get(uint32 devId, uint32 *pSign, uint32 *pStr)
{
    rtl8291_getRegBit(devId, RTL8291_WB_REG67_ADDR, RTL8291_WB_REG67_APC_EYEX_LEVEL_CROSS_DIR_L_OFFSET, pSign);
    rtl8291_getRegBits(devId, RTL8291_WB_REG67_ADDR, RTL8291_WB_REG67_APC_EYEX_LEVEL_CROSS_CTRL_L_1_0_MASK, pStr);

    return SUCCESS;
}

//Max MOD current
//MOD_MAX in MOD<11:4>, LSB=0.3906mA, max=100mA  (0x40=25mA, 0x80=50mA)
//eg. 0x40=25mA, 0x80=50mA
int32  rtl8291_apcModMaxEn_set(uint32 devId, uint32 enable)
{
    if (enable>0x1)
      return FAILED;

    rtl8291_setRegBit(devId, RTL8291_WB_REG84_ADDR, RTL8291_WB_REG84_REG_APCDIG_MOD_MAX_EN_OFFSET, enable);
    return SUCCESS;
}

int32  rtl8291_apcModMaxEn_get(uint32 devId, uint32 *pEnable)
{
    rtl8291_getRegBit(devId, RTL8291_WB_REG84_ADDR, RTL8291_WB_REG84_REG_APCDIG_MOD_MAX_EN_OFFSET, pEnable);
    return SUCCESS;
}


//TX_IMOD<10:3>
int32  rtl8291_apcModMax_set(uint32 devId, uint32 value)
{
    uint32 data1;

    if (value>0xFF)
      return FAILED;

    printk(KERN_EMERG " rtl8291_apcModMax_set: value = 0x%x\n", value);

    rtl8291_setRegBit(devId, RTL8291_WB_REG84_ADDR, RTL8291_WB_REG84_REG_APCDIG_MOD_MAX_LOAD_OFFSET, 0);
    rtl8291_setRegBits(devId, RTL8291_WB_REG80_ADDR, RTL8291_WB_REG80_REG_APCDIG_MOD_MAX_7_0_MASK, value);
    rtl8291_setRegBit(devId, RTL8291_WB_REG84_ADDR, RTL8291_WB_REG84_REG_APCDIG_MOD_MAX_LOAD_OFFSET, 1);

    rtl8291_getReg(devId, RTL8291_WB_REG80_ADDR, &data1);
    printk(KERN_EMERG " rtl8291_apcModMax_set: W82 = 0x%x\n", data1);

    return SUCCESS;
}

int32  rtl8291_apcModMax_get(uint32 devId, uint32 *pValue)
{
    rtl8291_getRegBits(devId, RTL8291_WB_REG80_ADDR, RTL8291_WB_REG80_REG_APCDIG_MOD_MAX_7_0_MASK, pValue);

    return SUCCESS;
}
//Min MOD current
//MOD_MIN in MOD<11:4>, LSB=0.3906mA, max=100mA  (0x40=25mA, 0x80=50mA)
//eg. 0x40=25mA, 0x80=50mA
int32  rtl8291_apcModMinEn_set(uint32 devId, uint32 enable)
{
    if (enable>0x1)
      return FAILED;

    rtl8291_setRegBit(devId, RTL8291_WB_REG84_ADDR, RTL8291_WB_REG84_REG_APCDIG_MOD_MIN_EN_OFFSET, enable);
    return SUCCESS;
}

int32  rtl8291_apcModMinEn_get(uint32 devId, uint32 *pEnable)
{
    rtl8291_getRegBit(devId, RTL8291_WB_REG84_ADDR, RTL8291_WB_REG84_REG_APCDIG_MOD_MIN_EN_OFFSET, pEnable);
    return SUCCESS;
}


//TX_IMOD<10:3>
int32  rtl8291_apcModMin_set(uint32 devId, uint32 value)
{
    uint32 data1;

    if (value>0xFF)
      return FAILED;
    
    printk(KERN_EMERG " rtl8291_apcModMin_set: value = 0x%x\n", value);

    rtl8291_setRegBit(devId, RTL8291_WB_REG84_ADDR, RTL8291_WB_REG84_REG_APCDIG_MOD_MIN_LOAD_OFFSET, 0);
    rtl8291_setRegBits(devId, RTL8291_WB_REG81_ADDR, RTL8291_WB_REG81_REG_APCDIG_MOD_MIN_7_0_MASK, value);
    rtl8291_setRegBit(devId, RTL8291_WB_REG84_ADDR, RTL8291_WB_REG84_REG_APCDIG_MOD_MIN_LOAD_OFFSET, 1);

    rtl8291_getReg(devId, RTL8291_WB_REG81_ADDR, &data1);
    printk(KERN_EMERG " rtl8291_apcModMin_set: W82 = 0x%x\n", data1);

    return SUCCESS;
}

int32  rtl8291_apcModMin_get(uint32 devId, uint32 *pValue)
{
    rtl8291_getRegBits(devId, RTL8291_WB_REG81_ADDR, RTL8291_WB_REG81_REG_APCDIG_MOD_MIN_7_0_MASK, pValue);

    return SUCCESS;
}

//Max BIAS current
//BIAS_MAX in BIAS<11:4>, , LSB=0.3906mA, max=100mA  (0x40=25mA, 0x80=50mA)
//eg. 0x40=25mA, 0x80=50mA
int32  rtl8291_apcBiasMaxEn_set(uint32 devId, uint32 enable)
{
    if (enable>0x1)
      return FAILED;

    rtl8291_setRegBit(devId, RTL8291_WB_REG84_ADDR, RTL8291_WB_REG84_REG_APCDIG_BIAS_MAX_EN_OFFSET, enable);
    return SUCCESS;
}

int32  rtl8291_apcBiasMaxEn_get(uint32 devId, uint32 *pEnable)
{
    rtl8291_getRegBit(devId, RTL8291_WB_REG84_ADDR, RTL8291_WB_REG84_REG_APCDIG_BIAS_MAX_EN_OFFSET, pEnable);
    return SUCCESS;
}

//TX_BIAS<10:3>
int32  rtl8291_apcBiasMax_set(uint32 devId, uint32 value)
{
    uint32 data1;

    if (value>0xFF)
      return FAILED;

    printk(KERN_EMERG " rtl8291_apcBiasMax_set: value = 0x%x\n", value);

    rtl8291_setRegBit(devId, RTL8291_WB_REG84_ADDR, RTL8291_WB_REG84_REG_APCDIG_BIAS_MAX_LOAD_OFFSET, 0);
    rtl8291_setRegBits(devId, RTL8291_WB_REG82_ADDR, RTL8291_WB_REG82_REG_APCDIG_BIAS_MAX_7_0_MASK, value);
    rtl8291_setRegBit(devId, RTL8291_WB_REG84_ADDR, RTL8291_WB_REG84_REG_APCDIG_BIAS_MAX_LOAD_OFFSET, 1);

    rtl8291_getReg(devId, RTL8291_WB_REG82_ADDR, &data1);
    printk(KERN_EMERG " rtl8291_apcBiasMax_set: W82 = 0x%x\n", data1);

    return SUCCESS;
}

int32  rtl8291_apcBiasMax_get(uint32 devId, uint32 *pValue)
{
    rtl8291_getRegBits(devId, RTL8291_WB_REG82_ADDR, RTL8291_WB_REG82_REG_APCDIG_BIAS_MAX_7_0_MASK, pValue);

    return SUCCESS;
}


//Min BIAS current
//BIAS_MIN<9:2> in BIAS<11:0>, LSB=0.097mA, max=24mA  (0x40=6.208mA, 0x80=12.416mA)
//(0x40=6.208mA, 0x80=12.416mA)
int32  rtl8291_apcBiasMinEn_set(uint32 devId, uint32 enable)
{
    if (enable>0x1)
      return FAILED;

    rtl8291_setRegBit(devId, RTL8291_WB_REG84_ADDR, RTL8291_WB_REG84_REG_APCDIG_BIAS_MIN_EN_OFFSET, enable);
    return SUCCESS;
}

int32  rtl8291_apcBiasMinEn_get(uint32 devId, uint32 *pEnable)
{
    rtl8291_getRegBit(devId, RTL8291_WB_REG84_ADDR, RTL8291_WB_REG84_REG_APCDIG_BIAS_MIN_EN_OFFSET, pEnable);
    return SUCCESS;
}

int32  rtl8291_apcBiasMin_set(uint32 devId, uint32 value)
{
    uint32 data1;

    if (value>0xFF)
      return FAILED;

    printk(KERN_EMERG " rtl8291_apcBiasMin_set: value = 0x%x\n", value);

    rtl8291_setRegBit(devId, RTL8291_WB_REG84_ADDR, RTL8291_WB_REG84_REG_APCDIG_BIAS_MIN_LOAD_OFFSET, 0);
    rtl8291_setRegBits(devId, RTL8291_WB_REG83_ADDR, RTL8291_WB_REG83_REG_APCDIG_BIAS_MIN_7_0_MASK, value);
    rtl8291_setRegBit(devId, RTL8291_WB_REG84_ADDR, RTL8291_WB_REG84_REG_APCDIG_BIAS_MIN_LOAD_OFFSET, 1);

    rtl8291_getReg(devId, RTL8291_WB_REG83_ADDR, &data1);
    printk(KERN_EMERG " rtl8291_apcBiasMin_set: W83 = 0x%x\n", data1);

    return SUCCESS;
}

int32  rtl8291_apcBiasMin_get(uint32 devId, uint32 *pValue)
{
    rtl8291_getRegBits(devId, RTL8291_WB_REG83_ADDR, RTL8291_WB_REG83_REG_APCDIG_BIAS_MIN_7_0_MASK, pValue);

    return SUCCESS;
}

//Max ER restriction when power leveling
//0: no restriction,  1:10dB,  2:11dB,  3:12dB
int32  rtl8291_apcPwrLvlMaxEr_set(uint32 devId, uint32 value)
{
    if (value>0x3)
        return FAILED;
    
    rtl8291_setRegBits(devId, RTL8291_WB_REG85_ADDR, RTL8291_WB_REG85_REG_APCDIG_POWER_LEVEL_ER_MAX_1_0_MASK, value);
    
    return SUCCESS;
}
    
int32  rtl8291_apcPwrLvlMaxEr_get(uint32 devId, uint32 *pValue)
{
    rtl8291_getRegBits(devId, RTL8291_WB_REG85_ADDR, RTL8291_WB_REG85_REG_APCDIG_POWER_LEVEL_ER_MAX_1_0_MASK, pValue);
    
    return SUCCESS;
}

//APC working time at DCL
//00: 2.048us 01: 4.096us 10: 8.192us 11: 16.384us
int32  rtl8291_apcApcTimer_set(uint32 devId, uint32 value)
{
    if (value>0x3)
      return FAILED;

    rtl8291_setRegBits(devId, RTL8291_WB_REG77_ADDR, RTL8291_WB_REG77_REG_APCDIG_APC_TIMER_1_0_MASK, value);

    return SUCCESS;
}

int32  rtl8291_apcApcTimer_get(uint32 devId, uint32 *pValue)
{
    rtl8291_getRegBits(devId, RTL8291_WB_REG77_ADDR, RTL8291_WB_REG77_REG_APCDIG_APC_TIMER_1_0_MASK, pValue);

    return SUCCESS;
}

//ERC working time at DCL
//00: 2.048us 01: 4.096us 10: 8.912us 11: 16.384us
int32  rtl8291_apcErcTimer_set(uint32 devId, uint32 value)
{
    if (value>0x3)
      return FAILED;

    rtl8291_setRegBits(devId, RTL8291_WB_REG77_ADDR, RTL8291_WB_REG77_REG_APCDIG_ERC_TIMER_1_0_MASK, value);

    return SUCCESS;
}

int32  rtl8291_apcErcTimer_get(uint32 devId, uint32 *pValue)
{
    rtl8291_getRegBits(devId, RTL8291_WB_REG77_ADDR, RTL8291_WB_REG77_REG_APCDIG_ERC_TIMER_1_0_MASK, pValue);

    return SUCCESS;
}

/*
 * ModCurrent: 120mA max for 11bits DAC, input is uA
 * BiasCurrent:120mA max for 11bits DAC, input is uA
 */
int32  rtl8291_tx_mod_bias_set(uint32 devId, uint32 mod, uint32 bias)
{
    uint32 value_mod, value_bias;
    uint32 mod_sys;
    uint32 bias_sys;

    mod_sys = (mod * 2047) / (120*1000); 
    value_mod = mod_sys&0x7FF;

    bias_sys = (bias * 2047) / (120*1000);   
    value_bias = bias_sys&0x7FF;

    rtl8291_apcImod_biasInitCode_set(devId, value_mod, value_bias);
    return SUCCESS;
}

//1: Enable FSU (rising edge to start FSU)
int32  rtl8291_fsuEnable_set(uint32 devId, uint32 enable)
{
    if (enable>0x1)
      return FAILED;

    rtl8291_setRegBit(devId, RTL8291_WB_REG7E_ADDR, RTL8291_WB_REG7E_REG_APCDIG_FSU_EN_OFFSET, enable);
    return SUCCESS;
}
    
int32  rtl8291_fsuEnable_get(uint32 devId, uint32 *pEnable)
{
    rtl8291_getRegBit(devId, RTL8291_WB_REG7E_ADDR, RTL8291_WB_REG7E_REG_APCDIG_FSU_EN_OFFSET, pEnable);
    return SUCCESS;
}

//Fast-start-up mode  1: APC&ERC,   0: APC only
int32  rtl8291_fsuMode_set(uint32 devId, uint32 mode)
{
    if (mode>0x1)
      return FAILED;
        
    rtl8291_setRegBit(devId, RTL8291_WB_REG7E_ADDR, RTL8291_WB_REG7E_REG_APCDIG_FSU_MODE_OFFSET, mode);
    return SUCCESS;
}
        
int32  rtl8291_fsuMode_get(uint32 devId, uint32 *pMode)
{
    rtl8291_getRegBit(devId, RTL8291_WB_REG7E_ADDR, RTL8291_WB_REG7E_REG_APCDIG_FSU_MODE_OFFSET, pMode);
    return SUCCESS;
}

//Fast start up mode APC digital loop filter gain
//0: 2^-1,  1:2^-2,  2:2^-3,  3:2^-4
int32  rtl8291_fsuApcLoopGain_set(uint32 devId, uint32 value)
{
    if (value>0x3)
        return FAILED;
    
    rtl8291_setRegBits(devId, RTL8291_WB_REG7E_ADDR, RTL8291_WB_REG7E_REG_APCDIG_FSU_APC_LOOPGAIN_1_0_MASK, value);
        
    return SUCCESS;
}
        
int32  rtl8291_fsuApcLoopGain_get(uint32 devId, uint32 *pValue)
{
    rtl8291_getRegBits(devId, RTL8291_WB_REG7E_ADDR, RTL8291_WB_REG7E_REG_APCDIG_FSU_APC_LOOPGAIN_1_0_MASK, pValue);        
    return SUCCESS;
}

//Fast start up mode APC ramp stage, Ibias adjust step option 
//0:128LSB,  1:64LSB,  2:32LSB,  3:16LSB
int32  rtl8291_fsuApcRampb_set(uint32 devId, uint32 value)
{
    if (value>0x3)
        return FAILED;
        
    rtl8291_setRegBits(devId, RTL8291_WB_REG7E_ADDR, RTL8291_WB_REG7E_REG_APCDIG_FSU_APC_RAMPB_1_0_MASK, value);
        
    return SUCCESS;
}
            
int32  rtl8291_fsuApcRampb_get(uint32 devId, uint32 *pValue)
{
    rtl8291_getRegBits(devId, RTL8291_WB_REG7E_ADDR, RTL8291_WB_REG7E_REG_APCDIG_FSU_APC_RAMPB_1_0_MASK, pValue);        
    return SUCCESS;
}
    
//Fast start up mode APC ramp stage, Imod adjust step option (ratio of Imod ramp to Ibias ramp)
//0:0x,  1:0.25x,  2:0.5x,  3:0.75x,  Ibias_ramp
int32  rtl8291_fsuApcRampm_set(uint32 devId, uint32 value)
{
    if (value>0x3)
        return FAILED;        
    rtl8291_setRegBits(devId, RTL8291_WB_REG7E_ADDR, RTL8291_WB_REG7E_REG_APCDIG_FSU_APC_RAMPM_1_0_MASK, value);        
    return SUCCESS;
}
        
int32  rtl8291_fsuApcRampm_get(uint32 devId, uint32 *pValue)
{
    rtl8291_getRegBits(devId, RTL8291_WB_REG7E_ADDR, RTL8291_WB_REG7E_REG_APCDIG_FSU_APC_RAMPM_1_0_MASK, pValue);     
    return SUCCESS;
}
                        
//Fast start up mode, APC/ERC reset period
//0:40ns,  1:48ns
int32  rtl8291_fsuRstCount_set(uint32 devId, uint32 mode)
{
    if (mode>0x1)
      return FAILED;
    
    rtl8291_setRegBit(devId, RTL8291_WB_REG7F_ADDR, RTL8291_WB_REG7F_REG_APCDIG_FSU_RST_CLK_COUNT_OFFSET, mode);
    return SUCCESS;
}
            
int32  rtl8291_fsuRstCount_get(uint32 devId, uint32 *pMode)
{
    rtl8291_getRegBit(devId, RTL8291_WB_REG7F_ADDR, RTL8291_WB_REG7F_REG_APCDIG_FSU_RST_CLK_COUNT_OFFSET, pMode);
    return SUCCESS;
}

//Fast start up mode,  TX LASER ON settling period
//0:16ns,  1:32ns,  2:64ns,  3:128ns
int32  rtl8291_fsuSettleCount_set(uint32 devId, uint32 value)
{
    if (value>0x3)
        return FAILED;        
    rtl8291_setRegBits(devId, RTL8291_WB_REG7F_ADDR, RTL8291_WB_REG7F_REG_APCDIG_FSU_SETTLE_CLK_COUNT_1_0_MASK, value);        
    return SUCCESS;
}
        
int32  rtl8291_fsuSettleCount_get(uint32 devId, uint32 *pValue)
{
    rtl8291_getRegBits(devId, RTL8291_WB_REG7F_ADDR, RTL8291_WB_REG7F_REG_APCDIG_FSU_SETTLE_CLK_COUNT_1_0_MASK, pValue);     
    return SUCCESS;
}

//"Fast start up mode ERC digital loop filter gain
//0: 2^-1,  1:2^-2,  2:2^-3,  3:2^-4"
int32  rtl8291_fsuErcLoopGain_set(uint32 devId, uint32 value)
{
    if (value>0x3)
        return FAILED;        
    rtl8291_setRegBits(devId, RTL8291_WB_REG7F_ADDR, RTL8291_WB_REG7F_REG_APCDIG_FSU_ERC_LOOPGAIN_1_0_MASK, value);        
    return SUCCESS;
}
        
int32  rtl8291_fsuErcLoopGain_get(uint32 devId, uint32 *pValue)
{
    rtl8291_getRegBits(devId, RTL8291_WB_REG7F_ADDR, RTL8291_WB_REG7F_REG_APCDIG_FSU_ERC_LOOPGAIN_1_0_MASK, pValue);     
    return SUCCESS;
}

//Fast start up mode ERC ramp stage, IMOD adjust step option (2 Imod, -1Ibias)
//0:64LSB,  1:32LSB,  2:16LSB,  3:8LSB
int32  rtl8291_fsuErcRampm_set(uint32 devId, uint32 value)
{
    if (value>0x3)
        return FAILED;        
    rtl8291_setRegBits(devId, RTL8291_WB_REG7F_ADDR, RTL8291_WB_REG7F_REG_APCDIG_FSU_ERC_RAMPM_1_0_MASK, value);        
    return SUCCESS;
}
        
int32  rtl8291_fsuErcRampm_get(uint32 devId, uint32 *pValue)
{
    rtl8291_getRegBits(devId, RTL8291_WB_REG7F_ADDR, RTL8291_WB_REG7F_REG_APCDIG_FSU_ERC_RAMPM_1_0_MASK, pValue);     
    return SUCCESS;
}

//1: Swap RXOUT P/N polarity    
int32 rtl8291_rxOutPolSwap_set(uint32 devId, uint32 state)
{
    if (state>0x1)
      return FAILED;
        
    rtl8291_setRegBit(devId, RTL8291_WB_REG20_ADDR, RTL8291_WB_REG20_REG_RX_SG_SWAP_L_OFFSET, state);
    return SUCCESS;
}
        
int32  rtl8291_rxOutPolSwap_get(uint32 devId, uint32 *pState)
{
    rtl8291_getRegBit(devId, RTL8291_WB_REG20_ADDR, RTL8291_WB_REG20_REG_RX_SG_SWAP_L_OFFSET, pState);
    return SUCCESS;
}

//RX AMP's bias current value control.
int32  rtl8291_rxOutSwing_set(uint32 devId, uint32 value)
{
    if (value>0x3)
        return FAILED;        
    rtl8291_setRegBits(devId,RTL8291_WB_REG21_ADDR, RTL8291_WB_REG21_REG_RX_IAMP_SEL_L_2_0_MASK, value);        
    return SUCCESS;
}
        
int32  rtl8291_rxOutSwing_get(uint32 devId, uint32 *pValue)
{
    rtl8291_getRegBits(devId, RTL8291_WB_REG21_ADDR, RTL8291_WB_REG21_REG_RX_IAMP_SEL_L_2_0_MASK, pValue);     
    return SUCCESS;
}

//RX offset cancellation's enable pin. 0 is disable and 1 is enable.
//When DIS_OFFCAN=1, RX_OSEN=0
int32  rtl8291_rxDisOffcan_set(uint32 devId, uint32 disable)
{
    if (disable>0x1)
      return FAILED;
        
    rtl8291_setRegBit(devId, RTL8291_WB_REG23_ADDR, RTL8291_WB_REG23_REG_RX_OSEN_L_OFFSET, disable);
    return SUCCESS;
}
        
int32  rtl8291_rxDisOffcan_get(uint32 devId, uint32 *pDisable)
{
    rtl8291_getRegBit(devId, RTL8291_WB_REG23_ADDR, RTL8291_WB_REG23_REG_RX_OSEN_L_OFFSET, pDisable);
    return SUCCESS;
}

//1:TXSD by IMPD,  0: TXSD by TX voltage (NOT IMPLEMENTED)
int32  rtl8291_txsdMode_set(uint32 devId, uint32 mode)
{
    if (mode>0x1)
      return FAILED;
        
    rtl8291_setRegBit(devId, RTL8291_WB_REG79_ADDR, RTL8291_WB_REG79_REG_APCDIG_TXSD_MODE_OFFSET, mode);
    return SUCCESS;
}
        
int32  rtl8291_txsdMode_get(uint32 devId, uint32 *pMode)
{
    rtl8291_getRegBit(devId, RTL8291_WB_REG79_ADDR, RTL8291_WB_REG79_REG_APCDIG_TXSD_MODE_OFFSET, pMode);
    return SUCCESS;
}

//0:20%,    1:17.5%,  2:15%,  3:12.5%,    4:10%,    5:7.5%,  6:5%,    7:2.5%
int32  rtl8291_txsdTh_set(uint32 devId, uint32 value)
{
    if (value>0x7)
        return FAILED;        
    rtl8291_setRegBits(devId, RTL8291_WB_REG7A_ADDR, RTL8291_WB_REG7A_REG_APCDIG_TXSD_ITH_2_0_MASK, value);        
    return SUCCESS;
}
            
int32  rtl8291_txsdTh_get(uint32 devId, uint32 *pValue)
{
    rtl8291_getRegBits(devId, RTL8291_WB_REG7A_ADDR, RTL8291_WB_REG7A_REG_APCDIG_TXSD_ITH_2_0_MASK, pValue);     
    return SUCCESS;
}

//TXSD TIA transimpedance. 000:750 ohms  001:1500 ohms  010:3000 ohms  011:6000 ohms  1xx:375 ohms ( test mode)    
int32  rtl8291_txsdTiaGain_set(uint32 devId, uint32 state)
{
    if (state>0x7)
      return FAILED;
            
    rtl8291_setRegBits(devId, RTL8291_WB_REG79_ADDR, RTL8291_WB_REG79_REG_APCDIG_TXSD_TIA_GAIN_2_0_MASK, state);
    return SUCCESS;
}
            
int32  rtl8291_txsdTiaGain_get(uint32 devId, uint32 *pState)
{
    rtl8291_getRegBits(devId, RTL8291_WB_REG79_ADDR, RTL8291_WB_REG79_REG_APCDIG_TXSD_TIA_GAIN_2_0_MASK, pState);
    return SUCCESS;
}


//0:2^-2,  1:2^-3    
int32  rtl8291_txsdHighLoopGain_set(uint32 devId, uint32 gain)
{
    if (gain>0x1)
      return FAILED;
                
    rtl8291_setRegBit(devId, RTL8291_WB_REG79_ADDR, RTL8291_WB_REG79_REG_APCDIG_TXSD_HIGH_LOOPGAIN_OFFSET, gain);
    return SUCCESS;
}
                
int32  rtl8291_txsdHighLoopGain_get(uint32 devId, uint32 *pGain)
{
    rtl8291_getRegBit(devId, RTL8291_WB_REG79_ADDR, RTL8291_WB_REG79_REG_APCDIG_TXSD_HIGH_LOOPGAIN_OFFSET, pGain);
    return SUCCESS;
}

// 0:2^-2 1:2^-3
int32  rtl8291_txsdLowLoopGain_set(uint32 devId, uint32 gain)
{
    if (gain>0x1)
      return FAILED;
                
    rtl8291_setRegBit(devId, RTL8291_WB_REG79_ADDR, RTL8291_WB_REG79_REG_APCDIG_TXSD_LOW_LOOPGAIN_OFFSET, gain);
    return SUCCESS;
}
                    
int32  rtl8291_txsdLowLoopGain_get(uint32 devId, uint32 *pGain)
{
    rtl8291_getRegBit(devId, RTL8291_WB_REG79_ADDR, RTL8291_WB_REG79_REG_APCDIG_TXSD_LOW_LOOPGAIN_OFFSET, pGain);
    return SUCCESS;
}
    

//0:8ns,    1:16ns,  2:24ns,  3:32ns    
int32  rtl8291_txsdOnRstCount_set(uint32 devId, uint32 value)
{
    if (value>0x3)
        return FAILED;
            
    rtl8291_setRegBits(devId, RTL8291_WB_REG7A_ADDR, RTL8291_WB_REG7A_REG_APCDIG_TXSD_ON_RST_COUNT_1_0_MASK, value);
        
    return SUCCESS;
}
                
int32  rtl8291_txsdOnRstCount_get(uint32 devId, uint32 *pValue)
{
    rtl8291_getRegBits(devId, RTL8291_WB_REG7A_ADDR, RTL8291_WB_REG7A_REG_APCDIG_TXSD_ON_RST_COUNT_1_0_MASK, pValue);     
    return SUCCESS;
}

//0:8ns,  1:16ns,  2:24ns,  3:32ns
int32  rtl8291_txsdOffRstCount_set(uint32 devId, uint32 value)
{
    if (value>0x3)
        return FAILED;            
    rtl8291_setRegBits(devId, RTL8291_WB_REG7A_ADDR, RTL8291_WB_REG7A_REG_APCDIG_TXSD_OFF_RST_COUNT_1_0_MASK, value);            
    return SUCCESS;
}
                    
int32  rtl8291_txsdOffRstCount_get(uint32 devId, uint32 *pValue)
{
    rtl8291_getRegBits(devId, RTL8291_WB_REG7A_ADDR, RTL8291_WB_REG7A_REG_APCDIG_TXSD_OFF_RST_COUNT_1_0_MASK, pValue);     
    return SUCCESS;
}

//if fault condition remain longer than this timer, TXSD Fault asserted.
//0: 130us, 1: 500us, 2: 1ms, 3: 10ms, 4: normal mode (no timer)
int32  rtl8291_txsdFaultTimer_set(uint32 devId, uint32 value)
{
    if (value>0x4)
        return FAILED;            
    rtl8291_setRegBits(devId, RTL8291_REG_EXTRA_ADDITION0_ADDR, RTL8291_REG_EXTRA_ADDITION0_CFG_TX_SD_FAULT_TMR_LEN_MASK, value);            
    return SUCCESS;
}
                    
int32  rtl8291_txsdFaultTimer_get(uint32 devId, uint32 *pValue)
{
    rtl8291_getRegBits(devId, RTL8291_REG_EXTRA_ADDITION0_ADDR, RTL8291_REG_EXTRA_ADDITION0_CFG_TX_SD_FAULT_TMR_LEN_MASK, pValue);     
    return SUCCESS;
}


    
//TX_DIS control
//0: dis tx & apc,  1: dis tx only,  2: dis apc only,  3: interrupt only
//A4/84[1:0] TX_DIS_CTL 
//A5/193 REG_EXTRA_CONTROL1 [7] cfg_tx_save_power_ex_mode    0: non-cfg_tx_dis_release when cfg_tx_dis_ctl=2b'01 (hw-auto)    1: use cfg_tx_dis_release[5] cfg_vdd_ldx_ctl_by_tx_dis    1: vdd ldx is controlled by tx dis    0: vdd ldx is not controlled by tx dis 
int32  rtl8291_txdisCtrl_set(uint32 devId, uint32 value)
{
    if (value>0x3)
        return FAILED;  

    //rtl8291_setRegBit(devId, RTL8290C_REG_EXTRA_CONTROL1_ADDR, RTL8290C_REG_EXTRA_CONTROL1_CFG_TX_SAVE_POWER_EX_MODE_OFFSET, 1);         
    //if (value == 0x3)
   // {
   //     rtl8291_setRegBit(devId, RTL8290B_REG_M1_ADDR, RTL8290B_REG_M1_VDD_LDX_CTRL_BY_TX_DIS_OFFSET, 0);           
    //}
    //else
    //{
    //    rtl8291_setRegBit(devId, RTL8290B_REG_M1_ADDR, RTL8290B_REG_M1_VDD_LDX_CTRL_BY_TX_DIS_OFFSET, 1);           
    //}
    rtl8291_setRegBits(devId, RTL8291_REG_CONTROL2_ADDR, RTL8291_REG_CONTROL2_TX_DIS_CTL_MASK, value);         

    return SUCCESS;
}                            
                           
int32  rtl8291_txdisCtrl_get(uint32 devId, uint32 *pValue)
{
    rtl8291_getRegBits(devId, RTL8291_REG_CONTROL2_ADDR, RTL8291_REG_CONTROL2_TX_DIS_CTL_MASK, pValue);    
    return SUCCESS;
}

//TX_DIS pin enable
//0: disable TX_DIS pin,  1: enable TX_DIS pin
int32  rtl8291_txDisPinEn_set(uint32 devId, uint32 enable)
{
    if (enable>0x1)
      return FAILED;

    if (1==enable)
    {
        rtl8291_setRegBit(devId, RTL8291_REG_ANA_EXTRA_CTL_1_ADDR, RTL8291_REG_ANA_EXTRA_CTL_1_TX_DIS_PIN_DIS_OFFSET, 0);
    }
    else
    {
        //printk("\n%s: %s\n",__FUNCTION__, __LINE__); 
        rtl8291_setRegBit(devId, RTL8291_REG_ANA_EXTRA_CTL_1_ADDR, RTL8291_REG_ANA_EXTRA_CTL_1_TX_DIS_PIN_DIS_OFFSET, 1);
    }
    
    return SUCCESS;
}
                    
int32  rtl8291_txDisPinEn_get(uint32 devId, uint32 *pEnable)
{
    uint32 data;
    rtl8291_getRegBit(devId, RTL8291_REG_ANA_EXTRA_CTL_1_ADDR, RTL8291_REG_ANA_EXTRA_CTL_1_TX_DIS_PIN_DIS_OFFSET, &data);

    if (data == 1)
        *pEnable = 0;
    else 
        *pEnable = 1;
    
    return SUCCESS;
}

//TX SD pin enable
//0: disable TX_SD pin,  1: enable TX_SD pin
int32  rtl8291_txSdPinEn_set(uint32 devId, uint32 enable)
{
    if (enable>0x1)
      return FAILED;

    rtl8291_setRegBit(devId, RTL8291_REG_CONTROL2_ADDR, RTL8291_REG_CONTROL2_TX_SD_PIN_ENA_OFFSET, enable);

    return SUCCESS;
}
                    
int32  rtl8291_txSdPinEn_get(uint32 devId, uint32 *pEnable)
{
    rtl8291_getRegBit(devId, RTL8291_REG_CONTROL2_ADDR, RTL8291_REG_CONTROL2_TX_SD_PIN_ENA_OFFSET, pEnable);

    return SUCCESS;
}


//0:6.4us  1:12.8us  2:25.6us  3:51.2us    W40[5:4] REG_SAMPLE_SEL_L<1:0>         
int32  rtl8291_rxlosSampleSel_set(uint32 devId, uint32 value)
{
    if (value>0x3)
        return FAILED;
    rtl8291_setRegBits(devId, RTL8291_WB_REG2C_ADDR, RTL8291_WB_REG2C_REG_RXLOS_SAMP_SEL_L_1_0_MASK, value);   
        return SUCCESS;
}
       
int32  rtl8291_rxlosSampleSel_get(uint32 devId, uint32 *pValue)
{
    rtl8291_getRegBits(devId, RTL8291_WB_REG2C_ADDR, RTL8291_WB_REG2C_REG_RXLOS_SAMP_SEL_L_1_0_MASK, pValue);
    return SUCCESS;
}
        
//Refrence DAC range selector0: 64mVpp  1: 128mVpp
//W38[3] REG_RANGE_SELL
int32  rtl8291_rxlosRangSel_set(uint32 devId, uint32 value)
{
    if (value>0x1)
        return FAILED;
    rtl8291_setRegBit(devId, RTL8291_WB_REG2D_ADDR, RTL8291_WB_REG2D_REG_RXLOS_RANGEX2_L_OFFSET, value);
    
    return SUCCESS;
}
        
int32  rtl8291_rxlosRangSel_get(uint32 devId, uint32 *pValue)
{
    rtl8291_getRegBit(devId, RTL8291_WB_REG2D_ADDR, RTL8291_WB_REG2D_REG_RXLOS_RANGEX2_L_OFFSET, pValue);
    
    return SUCCESS;
}

#if 0
// 0: LA  1: LA stage 2
//W38[5] REG_INPUT_SEL
int32  rtl8291_rxlosInputSel_set(uint32 devId, uint32 value)
{
    if (value>0x1)
        return FAILED;
           
    rtl8291_setRegBit(devId, RTL8290B_REG_W38_ADDR, RTL8290B_REG_W38_REG_INPUT_SELL_OFFSET, value);   
        return SUCCESS;
}
   
int32  rtl8291_rxlosInputSel_get(uint32 devId, uint32 *pValue)
{
    rtl8291_getRegBit(devId, RTL8290B_REG_W38_ADDR, RTL8290B_REG_W38_REG_INPUT_SELL_OFFSET, pValue);
    return SUCCESS;
}
#endif
    
//0: 50us,  1:100us,      2:150us,    3: no-debounce
//A4/87 REG_EXTRA_ADDITION_2[7:6] cfg_los_debne_length
int32  rtl8291_rxlosDebounceSel_set(uint32 devId, uint32 value)
{
    if (value>0x3)
        return FAILED;
    rtl8291_setRegBits(devId, RTL8291_REG_EXTRA_ADDITION2_ADDR, RTL8291_REG_EXTRA_ADDITION2_CFG_LOS_DEBNE_LENGTH_MASK, value);     
    return SUCCESS;
}
       
int32  rtl8291_rxlosDebounceSel_get(uint32 devId, uint32 *pValue)
{
    rtl8291_getRegBits(devId, RTL8291_REG_EXTRA_ADDITION2_ADDR, RTL8291_REG_EXTRA_ADDITION2_CFG_LOS_DEBNE_LENGTH_MASK, pValue);
    return SUCCESS;
}

//0: debounce for boht LOS assert & de-assert, 1: de-bounce for LOS de-assert only
int32  rtl8291_rxlosDebounceOpt_set(uint32 devId, uint32 value)
{
    if (value>0x1)
        return FAILED;
    rtl8291_setRegBit(devId, RTL8291_REG_ANA_EXTRA_CTL_1_ADDR, RTL8291_REG_ANA_EXTRA_CTL_1_CFG_LOS_SINGLE_DEBNE_OFFSET, value);     
    return SUCCESS;
}
       
int32  rtl8291_rxlosDebounceOpt_get(uint32 devId, uint32 *pValue)
{
    rtl8291_getRegBit(devId, RTL8291_REG_ANA_EXTRA_CTL_1_ADDR, RTL8291_REG_ANA_EXTRA_CTL_1_CFG_LOS_SINGLE_DEBNE_OFFSET, pValue);
    return SUCCESS;
}

//0: disable RXOUT output when LOS assert, 1: RXOUT normal when LOS assert
int32  rtl8291_rxlosSquelch_set(uint32 devId, uint32 value)
{
    if (value>0x1)
        return FAILED;
    rtl8291_setRegBit(devId, RTL8291_REG_ANA_EXTRA_CTL_1_ADDR, RTL8291_REG_ANA_EXTRA_CTL_1_CFG_RX_SQUELCH_LOS_OFFSET, value);

    return SUCCESS;
}
       
int32  rtl8291_rxlosSquelch_get(uint32 devId, uint32 *pValue)
{
    rtl8291_getRegBit(devId, RTL8291_REG_ANA_EXTRA_CTL_1_ADDR, RTL8291_REG_ANA_EXTRA_CTL_1_CFG_RX_SQUELCH_LOS_OFFSET, pValue);
    return SUCCESS;
}

#if 0    
//0:1T, 1:2T, 2:4T, 3:8T      
//W40[7:6] REG_CHOP_SEL<1:0>
int32  rtl8291_rxlosChopperFreq_set(uint32 devId, uint32 value)
{
    if (value>0x3)
        return FAILED;
               
    rtl8291_setRegBits(devId, RTL8290C_WB_REG33_ADDR, RTL8290C_WB_REG33_REG_CHOP_SEL_L_1_0_MASK, value);    
        return SUCCESS;
}
       
int32  rtl8291_rxlosChopperFreq_get(uint32 devId, uint32 *pValue)
{
    rtl8291_getRegBits(devId, RTL8290C_WB_REG33_ADDR, RTL8290C_WB_REG33_REG_CHOP_SEL_L_1_0_MASK, pValue);
    return SUCCESS;
}
          
//0:disable DAC reference voltage x0.8 for LA magnitude compensation, 1:enable   
//W41[2] REG_REFx0p8_EN
int32  rtl8291_rxlosLaMagComp_set(uint32 devId, uint32 value)
{
    if (value>0x1)
        return FAILED;
    rtl8291_setRegBit(devId, RTL8290C_WB_REG30_ADDR, RTL8290C_WB_REG30_REG_REFX0P8_ENL_OFFSET, value);     
    return SUCCESS;
}
           
int32  rtl8291_rxlosLaMagComp_get(uint32 devId, uint32 *pValue)
{
    rtl8291_getRegBit(devId, RTL8290C_WB_REG30_ADDR, RTL8290C_WB_REG30_REG_REFX0P8_ENL_OFFSET, pValue);
    return SUCCESS;
}

//0:disable DAC buffer auto-zero, 1:enable auto zero  
// W41[1] REG_DIS_BUF_OS_RESET
int32 rtl8291_rxlosBufAutozero_set(uint32 devId, uint32 value)
{
    if (value>0x1)
        return FAILED;
    rtl8291_setRegBit(devId, RTL8290C_WB_REG31_ADDR, RTL8290C_WB_REG31_REG_DIS_BUF_OS_RESETL_OFFSET, value);
        
    return SUCCESS;
}
            
int32  rtl8291_rxlosBufAutozero_get(uint32 devId, uint32 *pValue)
{
    rtl8291_getRegBit(devId, RTL8290C_WB_REG31_ADDR, RTL8290C_WB_REG31_REG_DIS_BUF_OS_RESETL_OFFSET, pValue);        
    return SUCCESS;
}
#endif

//0:reset, 1:set (default=1)    
//W26[1] REG_RSTB_MANL    
int32  rtl8291_rxlosResetb_set(uint32 devId, uint32 value)
{
    if (value>0x1)
        return FAILED;
               
    rtl8291_setRegBit(devId, RTL8291_WB_REG2B_ADDR, RTL8291_WB_REG2B_REG_RXLOS_CKRSTB_MAN_L_OFFSET, value);    
    return SUCCESS;    
}
int32  rtl8291_rxlosResetb_get(uint32 devId, uint32 *pValue)
{
    rtl8291_getRegBit(devId, RTL8291_WB_REG2B_ADDR, RTL8291_WB_REG2B_REG_RXLOS_CKRSTB_MAN_L_OFFSET, pValue);
    return SUCCESS;
}
    
//RXLOS input clock selection pin. 0 is fCKIN and 1 is 0.5*fCKIN.
int32  rtl8291_rxlosClkMode_set(uint32 devId, uint32 value)
{
    if (value>0x1)
        return FAILED;
    rtl8291_setRegBit(devId, RTL8291_WB_REG2B_ADDR, RTL8291_WB_REG2B_REG_RXLOS_CK_SEL_L_OFFSET, value);     
    return SUCCESS;
}
           
int32  rtl8291_rxlosClkMode_get(uint32 devId, uint32 *pValue)
{
    rtl8291_getRegBit(devId, RTL8291_WB_REG2B_ADDR, RTL8291_WB_REG2B_REG_RXLOS_CK_SEL_L_OFFSET, pValue);
    return SUCCESS;
}

//0:normal mode (assert/de-assert by top digital control), 1:manual mode      
//W49[1] REG_ASSERT_TESTMODE
int32  rtl8291_rxlosTestMode_set(uint32 devId, uint32 value)
{
    if (value>0x1)
        return FAILED;

    rtl8291_setRegBit(devId, RTL8291_WB_REG2B_ADDR, RTL8291_WB_REG2B_REG_RXLOS_ASSERT_TESTMD_L_OFFSET, value);    

    return SUCCESS;
}
           
int32  rtl8291_rxlosTestMode_get(uint32 devId, uint32 *pValue)
{
    rtl8291_getRegBit(devId, RTL8291_WB_REG2B_ADDR, RTL8291_WB_REG2B_REG_RXLOS_ASSERT_TESTMD_L_OFFSET, pValue);
    return SUCCESS;
}
  
//"manual assert/de-assert selection0:de-assert, 1:assert"    
// W49[0] REG_ASSERT_SELL
int32 rtl8291_rxlosAssertSel_set(uint32 devId, uint32 value)
{
    if (value>0x1)
        return FAILED;
    rtl8291_setRegBit(devId, RTL8291_WB_REG2B_ADDR, RTL8291_WB_REG2B_REG_RXLOS_ASSERT_SET_L_OFFSET, value);
        
    return SUCCESS;
}
            
int32  rtl8291_rxlosAssertSel_get(uint32 devId, uint32 *pValue)
{
    rtl8291_getRegBit(devId, RTL8291_WB_REG2B_ADDR, RTL8291_WB_REG2B_REG_RXLOS_ASSERT_SET_L_OFFSET, pValue);        
    return SUCCESS;
}
      
//"LOSS pin polarity 0: RXSD,      1:LOS"    
//A4/94 REG_POLARITY[0] LOS_SD_POLARITY_CTL
int32  rtl8291_rxlosPolarity_set(uint32 devId, uint32 value)
{
    if (value>0x1)
        return FAILED;
               
    rtl8291_setRegBit(devId, RTL8291_REG_POLARITY_ADDR, RTL8291_REG_POLARITY_LOS_SD_POLARITY_CTL_OFFSET, value);    
    return SUCCESS;    
}
int32  rtl8291_rxlosPolarity_get(uint32 devId, uint32 *pValue)
{
    rtl8291_getRegBit(devId, RTL8291_REG_POLARITY_ADDR, RTL8291_REG_POLARITY_LOS_SD_POLARITY_CTL_OFFSET, pValue);
    return SUCCESS;
}
    
//LOSS (RX SD) pin enable
//0: disable RXLOS pin,  1: enable RXLOS pin
int32  rtl8291_rxlosPinEn_set(uint32 devId, uint32 enable)
{
    if (enable>0x1)
      return FAILED;
                
    rtl8291_setRegBit(devId, RTL8291_REG_CONTROL2_ADDR, RTL8291_REG_CONTROL2_LOS_PIN_NO_TRI_OFFSET, enable);
    return SUCCESS;
}
                    
int32  rtl8291_rxlosPinEn_get(uint32 devId, uint32 *pEnable)
{
    rtl8291_getRegBit(devId, RTL8291_REG_CONTROL2_ADDR, RTL8291_REG_CONTROL2_LOS_PIN_NO_TRI_OFFSET, pEnable);
    return SUCCESS;
}                             
                 

//0: Disable RX_SLEEP pin
//A4/87 REG_EXTRA_ADDITION_2 [0] cfg_sda_m_funchg=1: sda_m pin as RX_SLEEP pin
int32  rtl8291_rxSleepPin_set(uint32 devId, uint32 enable)
{
    if (enable>0x1)
      return FAILED;
        
    rtl8291_setRegBit(devId, RTL8291_REG_EXTRA_ADDITION2_ADDR, RTL8291_REG_EXTRA_ADDITION2_CFG_SDA_M_FUNCHG_OFFSET, enable);
    return SUCCESS;
}
        
int32  rtl8291_rxSleepPin_get(uint32 devId, uint32 *pEnable)
{
    rtl8291_getRegBit(devId, RTL8291_REG_EXTRA_ADDITION2_ADDR, RTL8291_REG_EXTRA_ADDITION2_CFG_SDA_M_FUNCHG_OFFSET, pEnable);
    return SUCCESS;
}

//1: Force LOS assert during RX sleep (LOS pin state) (default=0)
int32  rtl8291_rxSleepForceLos_set(uint32 devId, uint32 enable)
{
    if (enable>0x1)
      return FAILED;
        
    rtl8291_setRegBit(devId, RTL8291_REG_EXTRA_ADDITION2_ADDR, RTL8291_REG_EXTRA_ADDITION2_CFG_RX_SLEEP_FORCE_LOS_OFFSET, enable);
    return SUCCESS;
}
        
int32  rtl8291_rxSleepForceLos_get(uint32 devId, uint32 *pEnable)
{
    rtl8291_getRegBit(devId, RTL8291_REG_EXTRA_ADDITION2_ADDR, RTL8291_REG_EXTRA_ADDITION2_CFG_RX_SLEEP_FORCE_LOS_OFFSET, pEnable);
    return SUCCESS;
}

//1: Power down L.A during RX sleep (default=1)
int32  rtl8291_rxSleepPdLa_set(uint32 devId, uint32 enable)
{
    if (enable>0x1)
      return FAILED;
        
    rtl8291_setRegBit(devId, RTL8291_REG_EXTRA_ADDITION2_ADDR, RTL8291_REG_EXTRA_ADDITION2_CFG_RX_SLEEP_PD_LA_OFFSET, enable);
    return SUCCESS;
}
        
int32  rtl8291_rxSleepPdLa_get(uint32 devId, uint32 *pEnable)
{
    rtl8291_getRegBit(devId, RTL8291_REG_EXTRA_ADDITION2_ADDR, RTL8291_REG_EXTRA_ADDITION2_CFG_RX_SLEEP_PD_LA_OFFSET, pEnable);
    return SUCCESS;
}

//1: Power down Booster during RX sleep  (default=0)
int32  rtl8291_rxSleepPdBooster_set(uint32 devId, uint32 enable)
{
    if (enable>0x1)
      return FAILED;
        
    rtl8291_setRegBit(devId, RTL8291_REG_EXTRA_ADDITION2_ADDR, RTL8291_REG_EXTRA_ADDITION2_CFG_RX_SLEEP_PD_BOOSTER_OFFSET, enable);
    return SUCCESS;
}
        
int32  rtl8291_rxSleepPdBooster_get(uint32 devId, uint32 *pEnable)
{
    rtl8291_getRegBit(devId, RTL8291_REG_EXTRA_ADDITION2_ADDR, RTL8291_REG_EXTRA_ADDITION2_CFG_RX_SLEEP_PD_BOOSTER_OFFSET, pEnable);
    return SUCCESS;
}

//1: Power down RSSI during RX sleep (default=1)
int32  rtl8291_rxSleepPdRssi_set(uint32 devId, uint32 enable)
{
    if (enable>0x1)
      return FAILED;
        
    rtl8291_setRegBit(devId, RTL8291_REG_EXTRA_ADDITION2_ADDR, RTL8291_REG_EXTRA_ADDITION2_CFG_RX_SLEEP_PD_RSSI_OFFSET, enable);
    return SUCCESS;
}
        
int32  rtl8291_rxSleepPdRssi_get(uint32 devId, uint32 *pEnable)
{
    rtl8291_getRegBit(devId, RTL8291_REG_EXTRA_ADDITION2_ADDR, RTL8291_REG_EXTRA_ADDITION2_CFG_RX_SLEEP_PD_RSSI_OFFSET, pEnable);
    return SUCCESS;
}

//1: Power down RX VCM during RX sleep (default=0)
int32  rtl8291_rxSleepPdVcm_set(uint32 devId, uint32 enable)
{
    if (enable>0x1)
      return FAILED;
        
    rtl8291_setRegBit(devId, RTL8291_REG_ANA_EXTRA_CTL_1_ADDR, RTL8291_REG_ANA_EXTRA_CTL_1_CFG_RX_SLEEP_PD_VCM_OFFSET, enable);
    return SUCCESS;
}
        
int32  rtl8291_rxSleepPdVcm_get(uint32 devId, uint32 *pEnable)
{
    rtl8291_getRegBit(devId, RTL8291_REG_ANA_EXTRA_CTL_1_ADDR, RTL8291_REG_ANA_EXTRA_CTL_1_CFG_RX_SLEEP_PD_VCM_OFFSET, pEnable);
    return SUCCESS;
}

//1: Enable ""Auto Power Leveling""set Tx power to level 1 (normal -3 dB) when RX power is higher/lower to certain thresholds
//0: Disable ""Auto Power Leveling""
int32  rtl8291_autoPowerLeveling_set(uint32 devId, uint32 enable)
{
    if (enable>0x1)
      return FAILED;
        
    rtl8291_setRegBit(devId, RTL8291_REG_ANA_EXTRA_CTL_1_ADDR, RTL8291_REG_ANA_EXTRA_CTL_1_CFG_AUTO_POWER_LEVEL_HW_AUTO_OFFSET, enable);
    return SUCCESS;
}
        
int32  rtl8291_autoPowerLeveling_get(uint32 devId, uint32 *pEnable)
{
    rtl8291_getRegBit(devId, RTL8291_REG_ANA_EXTRA_CTL_1_ADDR, RTL8291_REG_ANA_EXTRA_CTL_1_CFG_AUTO_POWER_LEVEL_HW_AUTO_OFFSET, pEnable);
    return SUCCESS;
}

//High RSSI threshold to trigger assertion of Auto Power Leveling.
int32  rtl8291_autoPowerLevelingHth_set(uint32 devId, uint32 value)
{
    if (value>0xFFF)
      return FAILED;

    rtl8291_setRegBits(devId, RTL8291_REG_EXTRA_ADDITION4_ADDR, RTL8291_REG_EXTRA_ADDITION4_CFG_AUTO_POWER_LEVEL_HTH_H_MASK, (value&0xFF0)>>4);
    rtl8291_setRegBits(devId, RTL8291_REG_EXTRA_ADDITION6_ADDR, RTL8291_REG_EXTRA_ADDITION6_CFG_AUTO_POWER_LEVEL_HTH_L_MASK, (value&0xF));

    return SUCCESS;
}

int32  rtl8291_autoPowerLevelingHth_get(uint32 devId, uint32 *pValue)
{
    uint32 data1,data2;
    
    rtl8291_getRegBits(devId, RTL8291_REG_EXTRA_ADDITION4_ADDR, RTL8291_REG_EXTRA_ADDITION4_CFG_AUTO_POWER_LEVEL_HTH_H_MASK, &data1);
    rtl8291_getRegBits(devId, RTL8291_REG_EXTRA_ADDITION6_ADDR, RTL8291_REG_EXTRA_ADDITION6_CFG_AUTO_POWER_LEVEL_HTH_L_MASK, &data2);
    *pValue = ((data1&0xFF)<<4)|(data2&0xF);

    return SUCCESS;
}

//Low RSSI threshold to trigger de-assertion of Auto Power Leveling.
int32  rtl8291_autoPowerLevelingLth_set(uint32 devId, uint32 value)
{
    if (value>0xFFF)
      return FAILED;

    rtl8291_setRegBits(devId, RTL8291_REG_EXTRA_ADDITION5_ADDR, RTL8291_REG_EXTRA_ADDITION5_CFG_AUTO_POWER_LEVEL_LTH_H_MASK, (value&0xFF0)>>4);
    rtl8291_setRegBits(devId, RTL8291_REG_EXTRA_ADDITION6_ADDR, RTL8291_REG_EXTRA_ADDITION6_CFG_AUTO_POWER_LEVEL_LTH_L_MASK, (value&0xF));

    return SUCCESS;
}

int32  rtl8291_autoPowerLevelingLth_get(uint32 devId, uint32 *pValue)
{
    uint32 data1,data2;
    
    rtl8291_getRegBits(devId, RTL8291_REG_EXTRA_ADDITION5_ADDR, RTL8291_REG_EXTRA_ADDITION5_CFG_AUTO_POWER_LEVEL_LTH_H_MASK, &data1);
    rtl8291_getRegBits(devId, RTL8291_REG_EXTRA_ADDITION6_ADDR, RTL8291_REG_EXTRA_ADDITION6_CFG_AUTO_POWER_LEVEL_LTH_L_MASK, &data2);
    *pValue = ((data1&0xFF)<<4)|(data2&0xF);

    return SUCCESS;
}

int32  rtl8291_ddmiTxpwrLatchSel_set(uint32 devId, uint32 value)
{
    if (value>0x3)
      return FAILED;

    rtl8291_setRegBits(devId, RTL8291_REG_EXTRA_ADDITION1_ADDR, RTL8291_REG_EXTRA_ADDITION1_CFG_BEN_OFF_OT_LEN_MASK, value);

    return SUCCESS;
}

int32  rtl8291_ddmiTxpwrLatchSel_get(uint32 devId, uint32 *pValue)
{    
    rtl8291_getRegBits(devId, RTL8291_REG_EXTRA_ADDITION1_ADDR, RTL8291_REG_EXTRA_ADDITION1_CFG_BEN_OFF_OT_LEN_MASK, pValue);

    return SUCCESS;
}


void _temp_bubble_sort(uint32 iarr[], uint8 num)
{
    uint8  i, j;
    //uint8 k;
    uint32 temp;

    //printk(KERN_EMERG "\nUnsorted Data:");
    //for (k = 0; k < num; k++) {
    //  printk(KERN_EMERG " %d ", iarr[k]);
   //}

    for (i = 1; i < num; i++)
    {
        for (j = 0; j < num - 1; j++)
        {
            if (iarr[j] > iarr[j + 1])
            {
                temp = iarr[j];
                iarr[j] = iarr[j + 1];
                iarr[j + 1] = temp;
            }
        }
    }

   // printk("\nAfter pass %d : ", i);
    //for (k = 0; k < num; k++)
    //{
         //printk(" %d ", iarr[k]);
    //}

}

//TCL: dump_sdadc_current_temp
//ct_out<18:0> is unsigned number, 9 bits integer, 10 bits fraction.
//Output is 19bit data, unit is K.
int32  rtl8291_temperature_get(uint32 devId, uint32 *temp)
{
    int32 ret = 0;
    uint32 code_data,  tmp1, tmp2;
    
    ret = rtl8291_getReg(devId, RTL8291_STD_DDMI_TEMP_H_ADDR, &tmp1);
    if (ret!=0)
        return ret;
    
    ret = rtl8291_getReg(devId, RTL8291_STD_DDMI_TEMP_L_ADDR, &tmp2);
    if (ret!=0)
        return ret;

    code_data = ((tmp1&0xFF)<<8)|(tmp2&0xFF);
    *temp = code_data;

#if 0 //From 8290C
    uint32 code_data, b_raw, tmp1, tmp2, tmp3, tmp4;
    uint32 temp_sign, temp_raw;
    int32  signed_temp;

    rtl8291_sdadc_code_get(devId, RTL8291_CODE_TEM, &code_data);
    
    rtl8291_getRegBits(devId, RTL8291_WB_REG41_ADDR, RTL8291_WB_REG41_B_21_17_MASK, &tmp1);
    rtl8291_getRegBits(devId, RTL8291_WB_REG42_ADDR, RTL8291_WB_REG42_B_16_9_MASK, &tmp2);
    rtl8291_getRegBits(devId, RTL8291_WB_REG43_ADDR, RTL8291_WB_REG43_B_8_1_MASK, &tmp3);
    rtl8291_getRegBits(devId, RTL8291_WB_REG44_ADDR, RTL8291_WB_REG44_B_0_MASK, &tmp4);
    b_raw = (tmp1<<17)|(tmp2<<9)|(tmp3<<1)|(tmp4>>7);

    if (b_raw == 0)//unit is K
    {
       //bit 0~18 is valid data, bit 10~18 is integer, bit 0~9 is fraction.
       *temp = (code_data&RTL8291_TEMP_MASK);
    }
    else //unit is C
    {
        //2's complement   
        temp_sign = (code_data & 0x40000)>>18;
        temp_raw = (code_data & 0x7FFFF);
        if (temp_sign!=0)
        {
            tmp1 = 0xFFF80000|temp_raw;
            signed_temp = (int)tmp1;
        }
        else
        {
            signed_temp = (int)(temp_raw&0x7FFFF);
        }
        //Change unit from C to K
        signed_temp = signed_temp+RTL8291_TEMP_ZERO;
        
        *temp = (uint32)signed_temp;
        //printk("\nTemperature = 0x%x , temp_raw = 0x%x ", *temp, temp_raw);        
    }
    
    //printk("\nrtl8291_temperature_get *temp = 0x%x , temp_raw = 0x%x b_raw = 0x%x\n", *temp, temp_raw, b_raw); 
#endif

    return SUCCESS;
}

int32  rtl8291_initProcFaultAction_set(uint32 devId, uint32 enable)
{
    if (enable>0x1)
      return FAILED;

    if (enable == 1 )
    {
        printk("\nExecute Startup Fault Procedure !!!\n");        
        rtl8291_setRegBit(devId, RTL8291_TX_POWER_CTL_ADDR, RTL8291_TX_POWER_CTL_TX_POWL_OFFSET, 0);       
    }
    
    return SUCCESS;
}

void rtl8291_fault_status(uint32 devId) 
{
    uint32 status,status2;
    
    /*get REG FAULT_ACTION bit*/      
    rtl8291_getRegBits(devId, RTL8291_REG_FAULT_STATUS_ADDR,0xFF,&status);
    rtl8291_getRegBits(devId, RTL8291_REG_FAULT_STATUS2_ADDR,0xFF,&status2);

    printk(KERN_EMERG ">>>> ");    

    if (status !=0) //Fault Status 1
    {          
        if(status & RTL8291_REG_FAULT_STATUS_TX_FAULT_STATUS_MASK)
        {
            //uint16 r30, r31;
            printk(KERN_EMERG "TX_FAULT\n");    

            //rtl8291_getRegBits(devId, RTL8290B_REG_R30_ADDR,0xFF,&r30);
            //rtl8291_getRegBits(devId, RTL8290B_REG_R31_ADDR,0xFF,&r31);
            //printk(KERN_EMERG "   R30=0x%02x R31=0x%02x\n", r30, r31);
        }
    
        if(status & RTL8291_REG_FAULT_STATUS_TX_SD_FAULT_STATUS_MASK)
        {
            printk(KERN_EMERG "TX_SD_FAULT\n");    
        }
        
        //20230426 TBD
        //if(status & RTL8291_REG_FAULT_STATUS_WDOG_TIMEOUT_FAULT_STATUS_MASK)
        //{
        //    printk(KERN_EMERG "WDOG_TIMEOUT_FAULT\n");    
        //}
    
        if(status & RTL8291_REG_FAULT_STATUS_BEN_TOO_LONG_FAULT_STATUS_MASK)
        {
            printk(KERN_EMERG "BEN_TOO_LONG_FAULT\n");    
        }
        
        if(status & RTL8291_REG_FAULT_STATUS_TX_LV_FAULT_STATUS_MASK)
        {
            printk(KERN_EMERG "TX_LV_FAULT\n");    
        }
    
        if(status & RTL8291_REG_FAULT_STATUS_OVER_IMPD_FAULT_STATUS_MASK)
        {
            printk(KERN_EMERG "OVER_IMPD_FAULT\n");    
        }
        if(status & RTL8291_REG_FAULT_STATUS_OVER_VOL_FAULT_STATUS_MASK)
        {
            printk(KERN_EMERG "OVER_VOL_FAULT\n");    
        }
    
        if(status & RTL8291_REG_FAULT_STATUS_OVER_TEMP_FAULT_STATUS_MASK)
        {
            printk(KERN_EMERG "OVER_TEMP_FAULT\n");    
        }        
    }

    if (status2 !=0) //Fault Status 2
    {          
        if(status & RTL8291_REG_FAULT_STATUS2_SW_FAULT_STATUS_MASK)
        {
            printk(KERN_EMERG "SW_FAULT\n");    
        }
    }
    
    printk(KERN_EMERG "\n");    
}


int32 rtl8291_txCross_set(uint32 devId, rtk_enable_t enable, uint32 sign, uint32 str)
{
    int32 ret;
    uint32 data;

    if (enable>0x1)
      return FAILED;

    if (enable == 0)
    {
        data = 0;
    }
    else
    {
        data = 1;
    }

    ret = rtl8291_txCrossEn_set(devId, data);
    if (ret<0)
        return FAILED;

    ret = rtl8291_txCrossAdj_set(devId, sign, str);
    if (ret<0)
        return FAILED;

    return SUCCESS;
}

int32 rtl8291_txCross_get(uint32 devId, rtk_enable_t *pEnable, uint32 *pSign, uint32 *pStr)
{
    int32 ret;
    uint32 data;

    if ((pEnable==NULL)||(pSign==NULL)||(pStr==NULL))
      return FAILED;

    ret = rtl8291_txCrossEn_get(devId, &data);
    if (ret<0)
        return FAILED;

    if (data == 0)
    {
        *pEnable = DISABLED;
    }
    else
    {
        *pEnable = ENABLED;
    }

    ret = rtl8291_txCrossAdj_get(devId, pSign, pStr);
    if (ret<0)
        return FAILED;

    return SUCCESS;
}

int32  rtl8291_calTrigger_set(uint32 devId)
{
    rtl8291_setRegBit(devId, RTL8291_WB_REG7B_ADDR, RTL8291_WB_REG7B_REG_APCDIG_CAL_EN_OFFSET, 0);
    rtl8291_setRegBit(devId, RTL8291_WB_REG7B_ADDR, RTL8291_WB_REG7B_REG_APCDIG_CAL_EN_OFFSET, 1);

    return SUCCESS;    
}

//Eye shaping - emphasis 
//W62<6> TX_EYEADJ_EMPHASIS_AMP_VG_SEL_MODEL,  0 proportional to MOD DAC current, 1 absolute voltage mode 
int32 rtl8291_txEsEmpVgMode_set(uint32 devId, uint32 enable)
{
    int32 ret=0;
    
    if (enable>0x1)
      return FAILED;
    
    ret = rtl8291_setRegBit(devId, RTL8291_WB_REG62_ADDR, RTL8291_WB_REG62_TX_EYEADJ_EMPHASIS_AMP_VG_SEL_MODEL_OFFSET, enable);

    return ret;
}

int32 rtl8291_txEsEmpVgMode_get(uint32 devId, uint32 *pEnable)
{
    int32 ret=0;
    
    ret = rtl8291_getRegBit(devId, RTL8291_WB_REG62_ADDR, RTL8291_WB_REG62_TX_EYEADJ_EMPHASIS_AMP_VG_SEL_MODEL_OFFSET, pEnable);

    return ret;
}

//Eye shaping - emphasis 
//W62<5:3> TX_EYEADJ_EMPHASIS_VG_DRIVING_STRL<2:0>, 000 min, 111 max
int32 rtl8291_txEsEmpVgStr_set(uint32 devId, uint32 str)
{
    int32 ret=0;

    if (str>0x7)
      return FAILED;
    
    ret = rtl8291_setRegBits(devId, RTL8291_WB_REG62_ADDR, RTL8291_WB_REG62_TX_EYEADJ_EMPHASIS_VG_DRIVING_STRL_2_0_MASK, str);

    return ret;
}

int32 rtl8291_txEsEmpVgStr_get(uint32 devId, uint32 *pStr)
{
    int32 ret=0;
    
    ret = rtl8291_getRegBits(devId, RTL8291_WB_REG62_ADDR, RTL8291_WB_REG62_TX_EYEADJ_EMPHASIS_VG_DRIVING_STRL_2_0_MASK, pStr);

    return ret;
}

//Eye shaping - emphasis 
//W62<2:0> TX_EYEADJ_EMPHASIS_LEVELL<2:0>, 000 no emphasis, 111 max level
int32 rtl8291_txEsEmpLevel_set(uint32 devId, uint32 level)
{
    int32 ret=0;

    if (level>0x7)
      return FAILED;

    ret = rtl8291_setRegBits(devId, RTL8291_WB_REG62_ADDR, RTL8291_WB_REG62_TX_EYEADJ_EMPHASIS_LEVELL_2_0_MASK, level);

    return ret;
}

int32 rtl8291_txEsEmpLevel_get(uint32 devId, uint32 *plevel)
{
    int32 ret=0;
    
    ret = rtl8291_getRegBits(devId, RTL8291_WB_REG62_ADDR, RTL8291_WB_REG62_TX_EYEADJ_EMPHASIS_LEVELL_2_0_MASK, plevel);

    return ret;
}

//Eye shaping - emphasis 
//W63<7:4> TX_EYEADJ_EMPHASIS_AMP_VG_SELL<3:0>
int32 rtl8291_txEsEmpVgAmp_set(uint32 devId, uint32 vg_amp)
{
    int32 ret=0;

    if (vg_amp>0xF)
      return FAILED;

    ret = rtl8291_setRegBits(devId, RTL8291_WB_REG63_ADDR, RTL8291_WB_REG63_TX_EYEADJ_EMPHASIS_AMP_VG_SELL_3_0_MASK, vg_amp);

    return ret;
}

int32 rtl8291_txEsEmpVgAmp_get(uint32 devId, uint32 *pVg_amp)
{
    int32 ret=0;
    
    ret = rtl8291_getRegBits(devId, RTL8291_WB_REG63_ADDR, RTL8291_WB_REG63_TX_EYEADJ_EMPHASIS_AMP_VG_SELL_3_0_MASK, pVg_amp);

    return ret;
}

//Eye shaping - delay 
//W64<7:0> TX_EYEADJ_DATA_DELAY_CTRLL<7:0>    <7:4> rising edge, <3:0> falling edge
int32 rtl8291_txEsEmpDelay_set(uint32 devId, uint32 rising, uint32 falling)
{
    int32 ret=0;

    if ((rising>0xF)||(falling>0xF))
      return FAILED;
    
    ret = rtl8291_setRegBits(devId, RTL8291_WB_REG64_ADDR, RTL8291_WB_REG64_TX_EYEADJ_DELAY_RISING_3_0_MASK, rising);
    ret = rtl8291_setRegBits(devId, RTL8291_WB_REG64_ADDR, RTL8291_WB_REG64_TX_EYEADJ_DELAY_FALLING_3_0_MASK, falling);

    return ret;
}

int32 rtl8291_txEsEmpDelay_get(uint32 devId, uint32 *pRising, uint32 *pFalling)
{
    int32 ret=0;
    
    ret = rtl8291_getRegBits(devId, RTL8291_WB_REG64_ADDR, RTL8291_WB_REG64_TX_EYEADJ_DELAY_RISING_3_0_MASK, pRising);
    ret = rtl8291_getRegBits(devId, RTL8291_WB_REG64_ADDR, RTL8291_WB_REG64_TX_EYEADJ_DELAY_FALLING_3_0_MASK, pFalling);

    return ret;
}

int32 rtl8291_txEsAdjEn_set(uint32 devId, uint32 enable)
{
    int32 ret;

    if (enable>0x3)
      return FAILED;

    ret = rtl8291_setRegBit(devId, RTL8291_WB_REG63_ADDR, RTL8291_WB_REG63_TX_EYEADJ_DATA_DELAY_ENL_OFFSET, enable);
    if (ret<0)
        return FAILED;

    return SUCCESS;
}

int32 rtl8291_txEsAdjEn_get(uint32 devId, uint32 *pEnable)
{
    int32 ret;
    
    if (pEnable==NULL)
        return FAILED;
    
    ret = rtl8291_getRegBit(devId, RTL8291_WB_REG63_ADDR, RTL8291_WB_REG63_TX_EYEADJ_DATA_DELAY_ENL_OFFSET, pEnable);

    return ret;
}


int32 rtl8291_txEsConfig_set(uint32 devId, uint32 mode, uint32 value)
{
    int32 ret=0;
    uint32 data1, data2;
    
    switch(mode)
    {
        case LDD_TXES_VG_MODE:
            ret = rtl8291_txEsEmpVgMode_set(devId, value);
            break;
        case LDD_TXES_VG_STR:
            ret = rtl8291_txEsEmpVgStr_set(devId, value);
            break;
        case LDD_TXES_VG_AMP:
            ret = rtl8291_txEsEmpVgAmp_set(devId, value);
            break;
        case LDD_TXES_EMP_LEVEL:
            ret = rtl8291_txEsEmpLevel_set(devId, value);
            break;
        case LDD_TXES_DELY_RISING:
            ret = rtl8291_txEsEmpDelay_get(devId, &data1, &data2);            
            ret = rtl8291_txEsEmpDelay_set(devId, value, data2);
            break;
        case LDD_TXES_DELY_FALLING:
            ret = rtl8291_txEsEmpDelay_get(devId, &data1, &data2);    
            ret = rtl8291_txEsEmpDelay_set(devId, data1, value);   
            break;
        case LDD_TXES_ADJ_ENABLE:
            ret = rtl8291_txEsAdjEn_set(devId, value);
            break;
        case LDD_TXES_TEST_MODE:
            ret = rtl8291_test_init(devId, value);
            break;            
        default:
            break;
    }

    return ret;
}

int32 rtl8291_txEsConfig_get(uint32 devId, uint32 mode, uint32 *pValue)
{
    int32 ret=0;
    
    uint32 data1, data2;
    
    switch(mode)
    {
        case LDD_TXES_VG_MODE:
            ret = rtl8291_txEsEmpVgMode_get(devId, pValue);
            break;
        case LDD_TXES_VG_STR:
            ret = rtl8291_txEsEmpVgStr_get(devId, pValue);
            break;
        case LDD_TXES_VG_AMP:
            ret = rtl8291_txEsEmpVgAmp_get(devId, pValue);
            break;
        case LDD_TXES_EMP_LEVEL:
            ret = rtl8291_txEsEmpLevel_get(devId, pValue);
            break;
        case LDD_TXES_DELY_RISING:
            ret = rtl8291_txEsEmpDelay_get(devId, &data1, &data2);            
            *pValue = data1;
            break;
        case LDD_TXES_DELY_FALLING:
            ret = rtl8291_txEsEmpDelay_get(devId, &data1, &data2);    
            *pValue = data2;
            break;
        case LDD_TXES_ADJ_ENABLE:
            ret = rtl8291_txEsAdjEn_get(devId, pValue);            
        default:
            break;
    }

    return ret;
}


//RTL8291_VHVL_GEN_REF_OPTION
int32 rtl8291_vhvlGenRefOpt_set(uint32 devId, uint32 value)
{
    int32 ret;

    if (value>0x3)
      return FAILED;

    ret = rtl8291_setRegBit(devId, RTL8291_WB_REG63_ADDR, RTL8291_WB_REG63_LS_DM2_L_2_OFFSET, (value&0x1));
    if (ret<0)
        return FAILED;

    ret = rtl8291_setRegBit(devId, RTL8291_WB_REG65_ADDR, RTL8291_WB_REG65_LS_DM2_L_3_OFFSET, (value&0x2)>>1);
    if (ret<0)
        return FAILED;

    return SUCCESS;
}

int32 rtl8291_vhvlGenRefOpt_get(uint32 devId, uint32 *pValue)
{
    int32 ret;
    uint32 opt1, opt2;

    if (pValue==NULL)
      return FAILED;

    ret = rtl8291_getRegBit(devId, RTL8291_WB_REG63_ADDR, RTL8291_WB_REG63_LS_DM2_L_2_OFFSET, &opt1);
    if (ret<0)
        return FAILED;

    ret = rtl8291_getRegBit(devId, RTL8291_WB_REG65_ADDR, RTL8291_WB_REG65_LS_DM2_L_3_OFFSET, &opt2);
    if (ret<0)
        return FAILED;

    *pValue = ((opt2&0x1)<<1)|(opt1&0x1);

    return SUCCESS;
}

//RTL8291_CML_NCAP_SEL_OFFSET_ADDR
int32 rtl8291_cmlNcapSel_set(uint32 devId, uint32 value)
{
    int32 ret;

    if (value>0xF)
      return FAILED;

    ret = rtl8291_setRegBits(devId, RTL8291_WB_REG56_ADDR, RTL8291_WB_REG56_CML_NCAP_SELL_3_0_MASK, value);

    return ret;
}

int32 rtl8291_cmlNcapSel_get(uint32 devId, uint32 *pValue)
{
    int32 ret;

    if (pValue==NULL)
      return FAILED;

    ret = rtl8291_getRegBits(devId, RTL8291_WB_REG56_ADDR, RTL8291_WB_REG56_CML_NCAP_SELL_3_0_MASK, pValue);

    return ret;
}


//------------------------------------------------------------------------------
// rtl8291_roData_get 
//    page   : RO_DATA page, 0 or 1
//    datasel: RO_DATA sel, 0 ~ 15
// RL6907 version
//------------------------------------------------------------------------------
int32 rtl8291_roData_get(uint32 devId, uint32 page, uint32 datasel, uint32 *pRo_data) 
{
    if (page >1)
    {
        return FAILED;
    }

    if (datasel>15)
    {
        return FAILED;
    }


    rtl8291_setRegBit(devId, RTL8291_WB_REG88_ADDR,RTL8291_WB_REG88_REG_APCDIG_RO_PAGE_OFFSET, page);
    rtl8291_setRegBits(devId, RTL8291_WB_REG88_ADDR,RTL8291_WB_REG88_REG_APCDIG_RO_DATA_SEL_3_0_MASK, datasel);

    rtl8291_getReg(devId, RTL8291_RB_REG0A_ADDR, pRo_data);

  return SUCCESS;
}


//set default IBX code
//set WB_REG0A[7:2> RBG_BIT_L<5:0> to 5B'10011
int32 rtl8291_ibx_set(uint32 devId) 
{
    rtl8291_setRegBits(devId, RTL8291_WB_REG0A_ADDR, RTL8291_WB_REG0A_RBG_BIT_L_5_0_MASK, 0x13);
    return SUCCESS;
}

#if 0
// WB_REG3D<7>    RK_DIG    RK_FORCE_EN_L    0
// WB_REG3D<6:4>    RK_DIG    RK_RXO_FORCE_CAL_L<2:0>    100
// WB_REG3D<3:2>    RK_DIG    RK_R400_FORCE_CAL_L<1:0>    10
int32 rtl8291_forceRk_set(uint32 devId, uint32 rx_rxo_l, uint32 rx_rk400l, uint32 rk_force_en_l)
{
    rtl8291_setRegBits(devId, RTL8290C_WB_REG3D_ADDR, RTL8290C_WB_REG3D_RK_R400_FORCE_CAL_L_1_0_MASK, rx_rk400l);
    rtl8291_setRegBits(devId, RTL8290C_WB_REG3D_ADDR, RTL8290C_WB_REG3D_RK_RXO_FORCE_CAL_L_2_0_MASK, rx_rxo_l);
    rtl8291_setRegBit(devId, RTL8290C_WB_REG3D_ADDR, RTL8290C_WB_REG3D_RK_FORCE_EN_L_OFFSET, rk_force_en_l);
    return SUCCESS; 
}
#endif

int32  rtl8291_ercComp_set(uint32 devId, uint32 step)
{
    uint32 erc_cal_dac_bits, erc_cal_dac_sgn;
    int32 cal_data;
    uint32 value;
    
    // APCDIG_ERC_CAL_BIT: Page1, DataSel=8 [6:0]
    rtl8291_roData_get(devId, 1, 8, &erc_cal_dac_bits); 
    //printk(KERN_EMERG "APCDIG_ERC_CAL_BIT[6:0] = 0x%X\n", erc_cal_dac_bits);
    // APCDIG_ERC_CAL_DAC_SGN[1]: Page1, DataSel=10 [0]
    rtl8291_roData_get(devId, 1, 10, &erc_cal_dac_sgn); 
    //printk(KERN_EMERG "APCDIG_ERC_CAL_DAC_SGN[1] = 0x%X\n", erc_cal_dac_sgn&0x1);

    if ((step>0)&&(step<=3))
    {
        printk(KERN_EMERG "Do ERC ...\n");     
        if (erc_cal_dac_bits > 0x7F)
        {
            printk(KERN_EMERG "APCDIG_ERC_CAL_BIT[6:0] over flow!\n", erc_cal_dac_bits); 
        }
        
        cal_data = erc_cal_dac_bits&0x7F;
    
        if ((erc_cal_dac_sgn&0x01)==0) //APCDIG_ERC_CAL_DAC_SGN[1] 1: positive 0: for negative
        {
            cal_data = 0 - cal_data;
        }    
        printk(KERN_EMERG "CAL value = %d\n", cal_data); 
    
        cal_data = cal_data - 15*step;
        if (cal_data<=-127)
        {
            printk(KERN_EMERG "ERC Minimum ...\n");           
            cal_data = -127;
        }
        
        value = abs(cal_data);
        //printk(KERN_EMERG "APC value = 0x%X, Comp CAL value = %d\n", value, cal_data);     
        if (cal_data > 0)
        {
             value = 0x80 | value;
        }

        rtl8291_setRegBit(devId, RTL8291_WB_REG7B_ADDR, RTL8291_WB_REG7B_REG_APCDIG_ERC_CAL_DAC_LOAD_OFFSET, 0);         
        rtl8291_setReg(devId, RTL8291_WB_REG7D_ADDR, value); 
        rtl8291_setRegBit(devId, RTL8291_WB_REG7B_ADDR, RTL8291_WB_REG7B_REG_APCDIG_ERC_CAL_DAC_LOAD_OFFSET, 1); 
     
    }
    
    return SUCCESS;    
}

#if 0
int32 rtl8291_rssiK_get(uint32 devId, uint32 *value)
{
    uint32 data1, data2;

    rtl8291_getReg(devId, RTL8290C_RSSI_K_MSB_ADDR, &data1);
    rtl8291_getRegBits(devId, RTL8290C_RSSI_K_MPD_K_LSB_ADDR, 0xF0, &data2);
    *value = (data1<<4)|(data2&0xF);
    return SUCCESS;
}

//Range of R6400 is 6000 ~ 7700
int32 rtl8291_rssiR6400_get(uint32 devId, uint32 *value)
{
    uint32 data1, data2;
    uint32 temp1;
    int    temp2;

    rtl8291_efuse_get(devId, 18, &data1);
    rtl8291_efuse_get(devId, 19, &data2);

    temp1 = ((data1&0xFF)<<4)|((data2&0xF0)>>4);
    if (temp1&0x800)
    {
        temp1 = 0xFFFFF|temp1;
    }
    temp2 = 6400 + (int)temp1;
    *value = (uint32)temp2;
    return SUCCESS;
}

int32 rtl8291_mpdK_get(uint32 devId, uint32 *value)
{
    uint32 data1, data2;

    rtl8291_getReg(devId, RTL8290C_MPD_K_MSB_ADDR, &data1);
    rtl8291_getRegBits(devId, RTL8290C_RSSI_K_MPD_K_LSB_ADDR, 0xF, &data2);
    *value = (data1<<4)|(data2&0xF);
    return SUCCESS;
}

//Range of R500 is 373 ~ 672
int32 rtl8291_mpdR500_get(uint32 devId, uint32 *value)
{
    uint32 data1;
    char   temp1;
    int    temp2;

    rtl8291_efuse_get(devId, 33, &data1);
    temp1 = data1&0xFF;
    temp2 = 500 + (int)temp1;
    *value = (uint32)temp2;
    return SUCCESS;
}
#endif
#endif



