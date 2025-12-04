#ifdef CONFIG_RTL8291_FEATURE
//#include <stdio.h>
//
#include <linux/sched.h>   /* for schedule_timeout() */
#include <linux/kernel.h>
#include <linux/delay.h>
#include <asm-generic/div64.h>

#include "include/ddmi.h"
#include "include/common.h"
#include "include/rtl8291_reg_acc.h"
#include "include/rtl8291_api.h"
#include "include/rtl8291_init.h"
#include "include/rtl8291_reg_definition.h"

void update_ddmi_8290c_temperature(uint32 devId, rtl8291_param_t *ldd_param, uint32 *temp_sys) 
{
    int temp32;
    uint32 temp1, temp2;
    int32 temp_i;
    uint32 temp_f;
    uint32 dev_idx=0;

    rtl8291_temperature_get(devId, &temp1);  
    temp32 = (int)temp1 - RTL8290C_TEMP_ZERO;
    
    //Original data is 10bit fraction, reduce to 8bits, 
    //if temp32 over 16bits, that means some error...
#if 0    
    temp32 = temp32>>2;
    *temp_sys = (uint32)temp32;
#endif    
    //printk("temp32 = %d (0x%x), temp_sys = 0x%x", temp32, temp32, *temp_sys);         
    dev_idx = devId-5;
    temp_i = temp32 >>10;
    temp_i = temp_i - (int)ldd_param->temp_offset;
    temp_f = (temp32 & 0x3FF)>>2;
    *temp_sys = (temp_i<<8)|temp_f;
    //printk("temp32 = %d , temp_i = %d, temp_offset = %d \n", temp32>>10, temp_i, (int)ldd_param->temp_offset);	
    return;
}

void ddmi_8290c_diagnostic_temperature(uint32 devId, uint32 temp_sys) 
{
    uint32 status, status2;
    int16 temp_ddmi, temp_now;
    
    //set temperature to DDMI
    rtl8291_setReg(devId, DDMI_A2_REG_START_ADDRESS + 96, (temp_sys >> 8)&0xff);
    rtl8291_setReg(devId, DDMI_A2_REG_START_ADDRESS + 97, temp_sys & 0xFF);
    temp_now = (int16)(temp_sys&0xFFFF);

    rtl8291_getReg(devId, DDMI_A2_REG_START_ADDRESS + 0, &status); //temperature high alarm
    rtl8291_getReg(devId, DDMI_A2_REG_START_ADDRESS + 1, &status2);
    temp_ddmi = (status << 8) + status2;
    if(temp_sys > temp_ddmi)
    { //A2[112] bit7
        rtl8291_setRegBit(devId, DDMI_A2_REG_START_ADDRESS + 112, 7, 1);
        //drv_setAsicRegBit(EUROPA_REG_SW_FAULT_STATUS1_ADDR, EUROPA_REG_SW_FAULT_STATUS1_OVER_HTEMP_FAULT_OFFSET, 1);        
    }

    rtl8291_getReg(devId, DDMI_A2_REG_START_ADDRESS + 2, &status); //temperature low alarm
    rtl8291_getReg(devId, DDMI_A2_REG_START_ADDRESS + 3, &status2);
    temp_ddmi = (status << 8) + status2;
    if(temp_sys < temp_ddmi)
    { //A2[112] bit6
        rtl8291_setRegBit(devId, DDMI_A2_REG_START_ADDRESS + 112, 6, 1);
        //drv_setAsicRegBit(EUROPA_REG_SW_FAULT_STATUS1_ADDR, EUROPA_REG_SW_FAULT_STATUS1_OVER_LTEMP_FAULT_OFFSET, 1);            
    }

    rtl8291_getReg(devId, DDMI_A2_REG_START_ADDRESS + 4, &status); //temperature high warning
    rtl8291_getReg(devId, DDMI_A2_REG_START_ADDRESS + 5, &status2);
    temp_ddmi = (status << 8) + status2;
    if(temp_sys > temp_ddmi)
    { //A2[116] bit7
        rtl8291_setRegBit(devId, DDMI_A2_REG_START_ADDRESS + 116, 7, 1);
    }
    
    rtl8291_getReg(devId, DDMI_A2_REG_START_ADDRESS + 6, &status); //temperature low warning
    rtl8291_getReg(devId, DDMI_A2_REG_START_ADDRESS + 7, &status2);    
    temp_ddmi = (status << 8) + status2;    
    if(temp_sys < temp_ddmi)
    { //A2[116] bit6
        rtl8291_setRegBit(devId, DDMI_A2_REG_START_ADDRESS + 116, 6, 1);
    }

}

void ddmi_8290c_temperature(uint32 devId, rtl8291_param_t *ldd_param) 
{
    uint32 temp_sys;

    update_ddmi_8290c_temperature(devId, ldd_param, &temp_sys);
    ddmi_8290c_diagnostic_temperature(devId, temp_sys);
}

void update_ddmi_8290c_vcc(uint32 devId, uint32 *vcc) 
{
    uint32 volt_sys;

    //unit is uV
    rtl8291_vdd_get(devId, &volt_sys);
    *vcc = volt_sys/100;
    
    //printk("volt_sys = %d (0x%x), vcc = 0x%x", volt_sys, volt_sys, *vcc); 

    return;
}

void ddmi_8290c_diagnostic_vcc(uint32 devId, uint32 volt_sys) 
{
    uint32 regData1, regData2;
    uint32 volt_ddmi;
    
    rtl8291_setReg(devId, DDMI_A2_REG_START_ADDRESS + 98, (volt_sys >> 8)& 0xff);
    rtl8291_setReg(devId, DDMI_A2_REG_START_ADDRESS + 99, (volt_sys) & 0xff);
    
    rtl8291_getReg(devId, DDMI_A2_REG_START_ADDRESS + 8, &regData1); //voltage high alarm
    rtl8291_getReg(devId, DDMI_A2_REG_START_ADDRESS + 9, &regData2);
    
    volt_ddmi = (regData1 << 8) + regData2;
    if(volt_sys > volt_ddmi)
    { //A2[112] bit5
        rtl8291_setRegBit(devId, DDMI_A2_REG_START_ADDRESS + 112, 5, 1);
        //drv_setAsicRegBit(EUROPA_REG_SW_FAULT_STATUS1_ADDR, EUROPA_REG_SW_FAULT_STATUS1_OVER_HVOL_FAULT_OFFSET, 1);                
    }

    rtl8291_getReg(devId, DDMI_A2_REG_START_ADDRESS + 10, &regData1); //voltage low alarm
    rtl8291_getReg(devId, DDMI_A2_REG_START_ADDRESS + 11, &regData2);
    volt_ddmi = (regData1 << 8) + regData2;
    if(volt_sys < volt_ddmi)
    { //A2[112] bit4
        rtl8291_setRegBit(devId, DDMI_A2_REG_START_ADDRESS + 112, 4, 1);        
    }

    rtl8291_getReg(devId, DDMI_A2_REG_START_ADDRESS + 12, &regData1); //voltage high warning
    rtl8291_getReg(devId, DDMI_A2_REG_START_ADDRESS + 13, &regData2);
    volt_ddmi = (regData1 << 8) + regData2;
    if(volt_sys > volt_ddmi)
    { //A2[116] bit5
        rtl8291_setRegBit(devId, DDMI_A2_REG_START_ADDRESS + 116, 5, 1);
    }
     
    rtl8291_getReg(devId, DDMI_A2_REG_START_ADDRESS + 14, &regData1); //voltage low warning
    rtl8291_getReg(devId, DDMI_A2_REG_START_ADDRESS + 15, &regData2);
    volt_ddmi = (regData1 << 8) + regData2;
    if(volt_sys < volt_ddmi)
    { //A2[116] bit4
        rtl8291_setRegBit(devId, DDMI_A2_REG_START_ADDRESS + 116, 4, 1);
    }

}

void ddmi_8290c_vcc(uint32 devId) 
{
    uint32 volt;

    update_ddmi_8290c_vcc(devId, &volt);
    ddmi_8290c_diagnostic_vcc(devId, volt);
}

void update_ddmi_8290c_tx_bias(uint32 devId, uint32 *bias_sys) 
{
    uint32 bias;

    rtl8291_tx_bias_get(devId, &bias);

    //Transfer from uA to 2uA
    *bias_sys = bias/2;   
   
    return;
}

void ddmi_8290c_diagnostic_tx_bias(uint32 devId, uint32 bias_sys) 
{
    uint32 regData1, regData2;
    uint32 bias_ddmi;

    rtl8291_setReg(devId, DDMI_A2_REG_START_ADDRESS + 100, (bias_sys >> 8)& 0xff);
    rtl8291_setReg(devId, DDMI_A2_REG_START_ADDRESS + 101, (bias_sys) & 0xff);    
    
    rtl8291_getReg(devId, DDMI_A2_REG_START_ADDRESS + 16, &regData1); //tx bias high alarm
    rtl8291_getReg(devId, DDMI_A2_REG_START_ADDRESS + 17, &regData2);    
    bias_ddmi = (regData1 << 8) + regData2;
    if(bias_sys > bias_ddmi)
    { //A2[112] bit3
        rtl8291_setRegBit(devId, DDMI_A2_REG_START_ADDRESS + 112, 3, 1);
        //drv_setAsicRegBit(EUROPA_REG_SW_FAULT_STATUS1_ADDR, EUROPA_REG_SW_FAULT_STATUS1_OVER_HBIAS_FAULT_OFFSET, 1);                        
    }

    rtl8291_getReg(devId, DDMI_A2_REG_START_ADDRESS + 18, &regData1); //tx bias low alarm
    rtl8291_getReg(devId, DDMI_A2_REG_START_ADDRESS + 19, &regData2);
    bias_ddmi = (regData1 << 8) + regData2;
    if(bias_sys < bias_ddmi)
    { //A2[112] bit2
        rtl8291_setRegBit(devId, DDMI_A2_REG_START_ADDRESS + 112, 2, 1);
        //drv_setAsicRegBit(EUROPA_REG_SW_FAULT_STATUS1_ADDR, EUROPA_REG_SW_FAULT_STATUS1_OVER_LBIAS_FAULT_OFFSET, 1);            
    }    

    rtl8291_getReg(devId, DDMI_A2_REG_START_ADDRESS + 20, &regData1); //tx bias high warning
    rtl8291_getReg(devId, DDMI_A2_REG_START_ADDRESS + 21, &regData2);
    bias_ddmi = (regData1 << 8) + regData2;
    if(bias_sys > bias_ddmi)
    { //A2[116] bit3
        rtl8291_setRegBit(devId, DDMI_A2_REG_START_ADDRESS + 116, 3, 1);
    }

    rtl8291_getReg(devId, DDMI_A2_REG_START_ADDRESS + 22, &regData1); //tx bias low warning
    rtl8291_getReg(devId, DDMI_A2_REG_START_ADDRESS + 23, &regData2);
    bias_ddmi = (regData1 << 8) + regData2;
    if(bias_sys < bias_ddmi)
    { //A2[116] bit2
        rtl8291_setRegBit(devId, DDMI_A2_REG_START_ADDRESS + 116, 2, 1);
    }    

}

void ddmi_8290c_tx_bias(uint32 devId) 
{
    uint32 bias_sys;
    
    update_ddmi_8290c_tx_bias(devId, &bias_sys);     
    ddmi_8290c_diagnostic_tx_bias(devId, bias_sys);
}

void update_ddmi_8290c_tx_power(uint32 devId, rtl8291_param_t *ldd_param, uint32 *tx_power) 
{
    int32 i;
    uint32 sum_i=0, avg_i;
    int32 temp_a, temp_b, temp_c, sum;    
    uint16 cnt;
    rtl8291_txpwr_t ldd_txpwr;    
    
    memset(&ldd_txpwr, 0, sizeof(rtl8291_txpwr_t));
    ldd_txpwr.tx_lsb = ldd_param->tx_lsb;
    ldd_txpwr.tx_code_250u = ldd_param->tx_code_250u;
    ldd_txpwr.tx_r_mpd = ldd_param->tx_r_mpd;
    for (i=0;i<DDMI_TX_POWER_NUM;i++) 
    {
        rtl8291_txMpdCurrent_get(devId, &ldd_txpwr);    
        sum_i += ldd_txpwr.i_mpd;              
    }
    avg_i = sum_i/DDMI_TX_POWER_NUM;    
	ldd_param->mpd_i = avg_i;
    memset(&ldd_txpwr, 0, sizeof(rtl8291_rxpwr_t));
    ldd_txpwr.i_mpd = avg_i;    
    ldd_txpwr.tx_a = ldd_param->tx_a;
    ldd_txpwr.tx_b = ldd_param->tx_b;
    ldd_txpwr.tx_c = ldd_param->tx_c;
    ldd_txpwr.data_a = ldd_param->tx_shft_a;
    ldd_txpwr.data_b = ldd_param->tx_shft_b;
    ldd_txpwr.data_c = ldd_param->tx_shft_c;
    _rtl8291_txCurr2Pwr_get(devId, &ldd_txpwr);
    ldd_param->tx_power = ldd_txpwr.tx_power;
    *tx_power = ldd_txpwr.tx_power;
    //printk(KERN_EMERG "update_tx_power: sum_i = %d, avg_i = %d, tx_power = 0x%x \n", sum_i, avg_i, *tx_power);
    return;
}

void ddmi_8290c_diagnostic_tx_power(uint32 devId, uint32 power_sys) 
{
    uint32 regData1, regData2;
    uint32 power_ddmi;
   
    rtl8291_setReg(devId, DDMI_A2_REG_START_ADDRESS + 102, (power_sys >> 8)& 0xff);
    rtl8291_setReg(devId, DDMI_A2_REG_START_ADDRESS + 103, (power_sys) & 0xff);    
   
    rtl8291_getReg(devId, DDMI_A2_REG_START_ADDRESS + 24, &regData1); //tx power high alarm
    rtl8291_getReg(devId, DDMI_A2_REG_START_ADDRESS + 25, &regData2);
    power_ddmi = (regData1 << 8) + regData2;
    if(power_sys > power_ddmi)
    { //A2[112] bit1
        rtl8291_setRegBit(devId, DDMI_A2_REG_START_ADDRESS + 112, 1, 1);
        //drv_setAsicRegBit(EUROPA_REG_SW_FAULT_STATUS1_ADDR, EUROPA_REG_SW_FAULT_STATUS1_OVER_HTXPWR_FAULT_OFFSET, 1);            
    }

    rtl8291_getReg(devId, DDMI_A2_REG_START_ADDRESS + 26, &regData1); //tx power low alarm
    rtl8291_getReg(devId, DDMI_A2_REG_START_ADDRESS + 27, &regData2);
    power_ddmi = (regData1 << 8) + regData2;
    if(power_sys < power_ddmi)
    { //A2[112] bit0
        rtl8291_setRegBit(devId, DDMI_A2_REG_START_ADDRESS + 112, 0, 1);
        //drv_setAsicRegBit(EUROPA_REG_SW_FAULT_STATUS1_ADDR, EUROPA_REG_SW_FAULT_STATUS1_OVER_LTXPWR_FAULT_OFFSET, 1);            
    }
	
    rtl8291_getReg(devId, DDMI_A2_REG_START_ADDRESS + 28, &regData1); //tx power high warning
    rtl8291_getReg(devId, DDMI_A2_REG_START_ADDRESS + 29, &regData2);
    power_ddmi = (regData1 << 8) + regData2;
    if(power_sys > power_ddmi)
    { //A2[116] bit1
        rtl8291_setRegBit(devId, DDMI_A2_REG_START_ADDRESS + 116, 1, 1);
    }

    rtl8291_getReg(devId, DDMI_A2_REG_START_ADDRESS + 30, &regData1); //tx power low warning
    rtl8291_getReg(devId, DDMI_A2_REG_START_ADDRESS + 31, &regData2);	
    power_ddmi = (regData1 << 8) + regData2;
    if(power_sys < power_ddmi)
    { //A2[116] bit0
        rtl8291_setRegBit(devId, DDMI_A2_REG_START_ADDRESS + 116, 0, 1);    
    }

}



void ddmi_8290c_tx_power(uint32 devId, rtl8291_param_t *ldd_param) 
{
    uint32 txpwr_sys;

    update_ddmi_8290c_tx_power(devId, ldd_param, &txpwr_sys);
    ddmi_8290c_diagnostic_tx_power(devId, txpwr_sys);
}


void update_ddmi_8290c_rx_power(uint32 devId, rtl8291_param_t *ldd_param, uint32 *rx_power) 
{
    uint32 i, sum_i=0, avg_i;
    int32 temp_a, temp_b, temp_c, sum;    
    uint16 cnt;
    rtl8291_rxpwr_t ldd_rxpwr;    
    
    memset(&ldd_rxpwr, 0, sizeof(rtl8291_rxpwr_t));
    ldd_rxpwr.rx_lsb = ldd_param->rx_lsb;
    ldd_rxpwr.rx_code_200u = ldd_param->rx_code_200u;
    ldd_rxpwr.rx_r6400 = ldd_param->rx_r6400;	
    //printk(KERN_EMERG "update_rx_power: rx_lsb = %d, rx_code_200u = %d, rx_r6400 = %d\n", ldd_rxpwr.rx_lsb, ldd_rxpwr.rx_code_200u, ldd_rxpwr.rx_r6400);
    for (i=0;i<DDMI_RX_POWER_NUM;i++) 
    {
        rtl8291_rxRssiCurrent_get(devId, &ldd_rxpwr);    
        sum_i += ldd_rxpwr.i_rssi;              
    }
    avg_i = (sum_i+DDMI_RX_POWER_NUM/2)/DDMI_RX_POWER_NUM; 
    //printk(KERN_EMERG "update_rx_power: sum_i = %d, avg_i = %d \n", sum_i, avg_i);
    ldd_param->rssi_i = avg_i;
    memset(&ldd_rxpwr, 0, sizeof(rtl8291_rxpwr_t));
    ldd_rxpwr.i_rssi = avg_i;    
    ldd_rxpwr.rx_a = ldd_param->rx_a;
    ldd_rxpwr.rx_b = ldd_param->rx_b;
    ldd_rxpwr.rx_c = ldd_param->rx_c;
    ldd_rxpwr.data_a = ldd_param->rx_shft_a;
    ldd_rxpwr.data_b = ldd_param->rx_shft_b;
    ldd_rxpwr.data_c = ldd_param->rx_shft_c;
    _rtl8291_rxCurr2Pwr_get(devId, &ldd_rxpwr);
    ldd_param->rx_power = ldd_rxpwr.rx_power;
    *rx_power = ldd_rxpwr.rx_power;
}

void ddmi_8290c_diagnostic_rx_power(uint32 devId, uint32 power_sys) 
{
    uint32 regData1, regData2;
    uint32 power_ddmi;
    //printk(KERN_EMERG "rx_power: power_sys = 0x%x\n", power_sys); 
    rtl8291_setReg(devId, DDMI_A2_REG_START_ADDRESS + 104, (power_sys >> 8)& 0xff);
    rtl8291_setReg(devId, DDMI_A2_REG_START_ADDRESS + 105, (power_sys) & 0xff);    
    
    rtl8291_getReg(devId, DDMI_A2_REG_START_ADDRESS + 32, &regData1); //rx power high alarm
    rtl8291_getReg(devId, DDMI_A2_REG_START_ADDRESS + 33, &regData2);
    power_ddmi = (regData1 << 8) + regData2;
    if(power_sys > power_ddmi)
    { //A2[113] bit7
        rtl8291_setRegBit(devId, DDMI_A2_REG_START_ADDRESS + 113, 7, 1);
        //drv_setAsicRegBit(EUROPA_REG_SW_FAULT_STATUS2_ADDR, EUROPA_REG_SW_FAULT_STATUS2_OVER_HRXPWR_FAULT_OFFSET, 1);            
    }
	
    rtl8291_getReg(devId, DDMI_A2_REG_START_ADDRESS + 34, &regData1); //rx power low alarm
    rtl8291_getReg(devId, DDMI_A2_REG_START_ADDRESS + 35, &regData2);
    power_ddmi = (regData1 << 8) + regData2;
    if(power_sys < power_ddmi)
    { //A2[113] bit6
        rtl8291_setRegBit(devId, DDMI_A2_REG_START_ADDRESS + 113, 6, 1);    
        //drv_setAsicRegBit(EUROPA_REG_SW_FAULT_STATUS2_ADDR, EUROPA_REG_SW_FAULT_STATUS2_OVER_LRXPWR_FAULT_OFFSET, 1);            
    }

    rtl8291_getReg(devId, DDMI_A2_REG_START_ADDRESS + 36, &regData1); //rx power high warning
    rtl8291_getReg(devId, DDMI_A2_REG_START_ADDRESS + 37, &regData2);
    power_ddmi = (regData1 << 8) + regData2;
    if(power_sys > power_ddmi)
    { //A2[117] bit7
        rtl8291_setRegBit(devId, DDMI_A2_REG_START_ADDRESS + 117, 7, 1);   
    }

    rtl8291_getReg(devId, DDMI_A2_REG_START_ADDRESS + 38, &regData1); //rx power low warning
    rtl8291_getReg(devId, DDMI_A2_REG_START_ADDRESS + 39, &regData2);    
    power_ddmi = (regData1 << 8) + regData2;
    if(power_sys < power_ddmi)
    { //A2[117] bit6
        rtl8291_setRegBit(devId, DDMI_A2_REG_START_ADDRESS + 117, 6, 1);    
    }
   
}

void ddmi_8290c_rx_power(uint32 devId, rtl8291_param_t *ldd_param) 
{
    uint32 rxpwr_sys = 0;
    update_ddmi_8290c_rx_power(devId, ldd_param, &rxpwr_sys);     
    //printk(KERN_EMERG "ddmi_8290c_rx_power: rxpwr_sys = 0x%x\n", rxpwr_sys); 
    ddmi_8290c_diagnostic_rx_power(devId, rxpwr_sys);	
}

void update_8290c_ddmi(uint32 devId, rtl8291_param_t *ldd_param)
{
    uint32 freeze;
    static int count = 0; 
    uint32 Addr, Data;

    /* check Digital REG_CONTROL1[4]: FREEZE_DDMI_UPDATE */    
    rtl8291_getRegBit(devId, RTL8290C_REG_CONTROL1_ADDR, RTL8290C_REG_CONTROL1_FREEZE_DDMI_UPDATE_OFFSET, &freeze);

    if (freeze)
    {
        return;   /* do not update DDMI */
    }
    //printk(KERN_EMERG "update_8290c_ddmi: freeze = 0x%x\n", freeze);     
    
    //for (count=0;count<0x100;count++)
    //{
    //    Addr = 0x280+count;
    //    rtl8291_getReg(devId, Addr, &Data);
    //}

    if (0==count)
    {
         /* Update DDMI TX Bias */
         //printk("[DDMI 8290C TX Bias]\n");
         ddmi_8290c_tx_bias(devId);

         /* Update DDMI temperature */
         //printk("[DDMI 8290C Temperature]\n");
         ddmi_8290c_temperature(devId, ldd_param);
    }

    if (1==count)  
    {
        //printk("[DDMI 8290C VCC]\n");    
        ddmi_8290c_vcc(devId);
    }

    if (++count==2)  /* temperature, bias, bias updated in less freq */
    {
        count=0;
    } 

    //printk("[DDMI RX Power]\n");
    ddmi_8290c_rx_power(devId, ldd_param);

    //printk("[DDMI TX Power]\n");
    ddmi_8290c_tx_power(devId, ldd_param);
   
}

#endif

