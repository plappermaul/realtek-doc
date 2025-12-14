#ifdef CONFIG_RTL8291_FEATURE

#include <osal/memory.h>
#include <osal/sem.h>
#include <osal/time.h>
//#include <common/debug/rt_log.h>

#include <linux/kernel.h>
#include <linux/delay.h>

#include <rtk/gpio.h>
#include <rtk/irq.h>

#include "include/europa.h"
#include "include/common.h"
#include "include/utility.h"
#include "include/apc_enable.h"
#include "include/tx_enable.h"
#include "include/init.h"
#include "include/laser_drv.h"
#include "include/ddmi.h"
#include "include/rtl8290_api.h"
#include "include/rtl8290b_api.h"

extern uint8 flash_data[];

extern uint8 loopmode;

extern uint16 europa_chip_type;


void laser_drv_fault_status(void);

void laser_drv_intrEnable(uint16 mask1, uint16 mask2) 
{
    //uint16 mask;

    //printk(KERN_EMERG "laser_drv_intrEnable !!!\n");    
    /*only enable europa interrupt*/
    
    //mask = 0;
    /*turn on all mask for interrupt 1*/
   // setReg(EUROPA_REG_INT_MASK1_ADDR, 0x20);
    //mask =(~(EUROPA_REG_INT_MASK1_TEMP_CHANGE_INT_MASK_MASK|EUROPA_REG_INT_MASK1_RSSI_ER_INT_MASK_MASK|EUROPA_REG_INT_MASK1_LOST_INT_MASK_MASK|EUROPA_REG_INT_MASK1_FAULT_INT_MASK_MASK|EUROPA_REG_INT_MASK1_FAULT_RELEASE_INT_MASK_MASK|EUROPA_REG_INT_MASK1_DDMI_TX_DIS_INT_MASK_MASK|EUROPA_REG_INT_MASK1_TX_DIS_INT_MASK_MASK))&0xff;
    //mask = 0x20;
    //getReg(EUROPA_DEBUG_INT_MASK1_REG)
    setReg(EUROPA_REG_INT_MASK1_ADDR, mask1);
    //printk(KERN_EMERG "EUROPA_REG_INT_MASK1_ADDR = 0x%x !!!\n", mask1);
    
    /*turn on all mask for interrupt 2*/
   // mask = 0x05;
    setReg(EUROPA_REG_INT_MASK2_ADDR, mask2);    
    //printk(KERN_EMERG "EUROPA_REG_INT_MASK2_ADDR = 0x%x !!!\n", mask2);    
}

void laser_drv_intrDisable(void) 
{
    //printk(KERN_EMERG "laser_drv_intrDisable !!!\n");        
    setReg(EUROPA_REG_INT_MASK1_ADDR, 0xff);      
    setReg(EUROPA_REG_INT_MASK2_ADDR, 0xff);      
}



/*enable or disable laser control finction
  for:
  TX
  VDD_LDX
  Booster
  RX
  RSSI
  SDAD 
  DAC
  APC
*/
void laser_drv_functionCtrl(europa_laser_fun_t fun, uint16 enable) 
{
    //uint16 tmp;
    
    switch(fun)
    {
        case EUROPA_LASER_FUN_TX:
            if(enable == ENABLE)
                drv_setAsicRegBit(EUROPA_REG_CONTROL2_ADDR,EUROPA_REG_CONTROL2_TX_POW_CTL_OFFSET,1);    
            else
                drv_setAsicRegBit(EUROPA_REG_CONTROL2_ADDR,EUROPA_REG_CONTROL2_TX_POW_CTL_OFFSET,0);                
            /*if(enable == ENABLE)
                drv_setAsicRegBit(EUROPA_REG_CONTROL2_ADDR,EUROPA_REG_CONTROL2_TX_DIS_CTL_OFFSET,1);    
            else
                drv_setAsicRegBit(EUROPA_REG_CONTROL2_ADDR,EUROPA_REG_CONTROL2_TX_DIS_CTL_OFFSET,0);*/    
            break;    
        case EUROPA_LASER_FUN_VDD_LDX:
            if(enable == ENABLE)
                drv_setAsicRegBit(EUROPA_REG_CONTROL2_ADDR,EUROPA_REG_CONTROL2_ENLD_L_OFFSET,1);    
            else
                drv_setAsicRegBit(EUROPA_REG_CONTROL2_ADDR,EUROPA_REG_CONTROL2_ENLD_L_OFFSET,0);    
            break;    

        case EUROPA_LASER_FUN_BOOSTER:
            if(enable == ENABLE)
        {
        /* disable LOS pin during booster turn on */
        drv_setAsicRegBits(EUROPA_REG_CONTROL2_ADDR, EUROPA_REG_CONTROL2_LOS_PIN_TRI_MASK, 0x0);
        drv_setAsicRegBit(EUROPA_REG_W4_ADDR, EUROPA_REG_W4_EN_L_OFFSET, 1);
        mdelay(200);
        drv_setAsicRegBits(EUROPA_REG_CONTROL2_ADDR, EUROPA_REG_CONTROL2_LOS_PIN_TRI_MASK, 0x1);
        }
            else
        {
        /* disable LOS pin during booster turn off */
        drv_setAsicRegBits(EUROPA_REG_CONTROL2_ADDR, EUROPA_REG_CONTROL2_LOS_PIN_TRI_MASK, 0x0);
        drv_setAsicRegBit(EUROPA_REG_W4_ADDR, EUROPA_REG_W4_EN_L_OFFSET, 1);
        mdelay(200);
        drv_setAsicRegBits(EUROPA_REG_CONTROL2_ADDR, EUROPA_REG_CONTROL2_LOS_PIN_TRI_MASK, 0x1);
        }
            break;   
        case EUROPA_LASER_FUN_RX:
            if(enable == ENABLE)
            {
                drv_setAsicRegBit(EUROPA_REG_W41_ADDR,4,0);  
                //busy_loop(0x50);
        udelay(5);
                drv_setAsicRegBit(EUROPA_REG_W41_ADDR,5,1);    
            }
            else
            {
                drv_setAsicRegBit(EUROPA_REG_W41_ADDR,4,1);    
                //busy_loop(0x50);
        udelay(5);
                drv_setAsicRegBit(EUROPA_REG_W41_ADDR,5,0);    
            }        
        
            break;
        case EUROPA_LASER_FUN_RSSI:
            if(enable == ENABLE)
            {
                drv_setAsicRegBit(EUROPA_REG_W40_ADDR,4,0);    
                //busy_loop(0x50);
        udelay(5);
                drv_setAsicRegBit(EUROPA_REG_W40_ADDR,4,1);    
            }
            else
            {
                drv_setAsicRegBit(EUROPA_REG_W40_ADDR,4,1);    
                //busy_loop(0x50);
        udelay(5);
                drv_setAsicRegBit(EUROPA_REG_W40_ADDR,4,0);    
            }        
            break;   
        case EUROPA_LASER_FUN_SDADC:
            if(enable == ENABLE)
                drv_setAsicRegBit(EUROPA_REG_W34_ADDR,0,1);    
            else
                drv_setAsicRegBit(EUROPA_REG_W34_ADDR,0,0);    
            break;
        case EUROPA_LASER_FUN_DAC:
            if(enable == ENABLE)
                drv_setAsicRegBit(EUROPA_REG_W36_ADDR,2,1);    
            else
                drv_setAsicRegBit(EUROPA_REG_W36_ADDR,2,0);    
            break;   
        case EUROPA_LASER_FUN_APC:
            if(enable == ENABLE)
            {
                drv_setAsicRegBit(EUROPA_REG_W57_ADDR,EUROPA_REG_W57_APCDIG_BIAS_POW_OFFSET,1);  
                //busy_loop(0x50);
        udelay(5);
                drv_setAsicRegBit(EUROPA_REG_W53_ADDR,EUROPA_REG_W53_APC_ENFD_MPD_HIGH_OFFSET,1);    
                drv_setAsicRegBit(EUROPA_REG_W53_ADDR,EUROPA_REG_W53_APC_ENFD_MPD_LOW_OFFSET,1);    
            }
            else
            {
                drv_setAsicRegBit(EUROPA_REG_W57_ADDR,EUROPA_REG_W57_APCDIG_BIAS_POW_OFFSET,0); 
                //busy_loop(0x50);
        udelay(5);
                drv_setAsicRegBit(EUROPA_REG_W53_ADDR,EUROPA_REG_W53_APC_ENFD_MPD_HIGH_OFFSET,0);    
                drv_setAsicRegBit(EUROPA_REG_W53_ADDR,EUROPA_REG_W53_APC_ENFD_MPD_LOW_OFFSET,0);    
            }         
            break;
               
        default:
            break;        
    }
}

void laser_drv_faultReleaseIntrHandle(uint16 status) 
{
    //uint16 status;
    /*get REG FAULT_ACTION bit*/    
    
    //drv_getAsicRegBits(EUROPA_REG_FAULT_CTL_ADDR,EUROPA_REG_FAULT_CTL_FAULT_ACTION_MASK,&status);

    switch(status)
    {
        case EUROPA_FAULT_INTR_8051_ONLY:
            laser_drv_functionCtrl(EUROPA_LASER_FUN_TX,ENABLE);    
            laser_drv_functionCtrl(EUROPA_LASER_FUN_VDD_LDX,ENABLE);              
            laser_drv_functionCtrl(EUROPA_LASER_FUN_BOOSTER,ENABLE);    
            return;    
        case EUROPA_FAULT_INTR_8051_TX_VDD_LDX:
            laser_drv_functionCtrl(EUROPA_LASER_FUN_TX,DISABLE);    
            laser_drv_functionCtrl(EUROPA_LASER_FUN_VDD_LDX,DISABLE);    
            break;    
        case EUROPA_FAULT_INTR_8051_TX_VDD_LDX_BOOSTER:
            laser_drv_functionCtrl(EUROPA_LASER_FUN_TX,DISABLE);    
            laser_drv_functionCtrl(EUROPA_LASER_FUN_VDD_LDX,DISABLE);              
            laser_drv_functionCtrl(EUROPA_LASER_FUN_BOOSTER,DISABLE);    
            laser_drv_functionCtrl(EUROPA_LASER_FUN_RX,DISABLE);
            break;            
        case EUROPA_FAULT_INTR_8051_TX_VDD_LDX_BOOSTER_RSSI_SDADC_DAC:
            drv_setAsicRegBit(EUROPA_REG_RESET_ADDR, EUROPA_REG_RESET_TOTAL_CHIP_RESET_OFFSET,1);              
            break;     
        default :
            return;    
        
    }
    
    drv_setAsicRegBit(EUROPA_REG_CONTROL3_ADDR,EUROPA_REG_CONTROL3_UNDER_FAULT_RELEASE_MASK,1);
    //busy_loop(0x500);
    udelay(500); // FIXME  is 500us feasible value?
    drv_setAsicRegBit(EUROPA_REG_CONTROL3_ADDR,EUROPA_REG_CONTROL3_UNDER_FAULT_RELEASE_MASK,0);
    
    switch(status)
    {
        case EUROPA_FAULT_INTR_8051_TX_VDD_LDX:
            laser_drv_functionCtrl(EUROPA_LASER_FUN_TX,ENABLE);    
            laser_drv_functionCtrl(EUROPA_LASER_FUN_VDD_LDX,ENABLE);    
            break;    
        case EUROPA_FAULT_INTR_8051_TX_VDD_LDX_BOOSTER:
            laser_drv_functionCtrl(EUROPA_LASER_FUN_TX,ENABLE);    
            laser_drv_functionCtrl(EUROPA_LASER_FUN_VDD_LDX,ENABLE);              
            laser_drv_functionCtrl(EUROPA_LASER_FUN_BOOSTER,ENABLE);    
            laser_drv_functionCtrl(EUROPA_LASER_FUN_RX,ENABLE);
            break;                 
        default :
            return;    
        
    }


}

void laser_drv_underTxDisFaultReleaseIntrHandle(void) 
{
    uint16 status,status2;
    /*get REG FAULT_ACTION bit*/    
    
    drv_getAsicRegBits(EUROPA_REG_FAULT_STATUS_ADDR,0xFF,&status);
    drv_getAsicRegBits(EUROPA_REG_FAULT_STATUS2_ADDR,0xFF,&status2);

    if ((status ==0) && (status2 == 0)) //No Fault
    {
        drv_getAsicRegBits(EUROPA_REG_CONTROL2_ADDR,EUROPA_REG_CONTROL2_TX_DIS_CTL_MASK,&status);
        drv_getAsicRegBit(EUROPA_REG_M1_ADDR,7,&status2);
    
        if ((EUROPA_UNDER_TX_FAULT_INTR_TX_ONLY != status)|| (status2!=0x1))
        {
            drv_setAsicRegBit(EUROPA_REG_CONTROL3_ADDR, EUROPA_REG_CONTROL3_UNDER_TX_DIS_RELEASE_OFFSET,0);
          //busy_loop(0x50);
          udelay(5);
            drv_setAsicRegBit(EUROPA_REG_CONTROL3_ADDR, EUROPA_REG_CONTROL3_UNDER_TX_DIS_RELEASE_OFFSET,1);
          //busy_loop(0x50);
          udelay(5);
            drv_setAsicRegBit(EUROPA_REG_CONTROL3_ADDR, EUROPA_REG_CONTROL3_UNDER_TX_DIS_RELEASE_OFFSET,0);            
        }
      //busy_loop(0x500);
      udelay(500); // FIXME  is 500us feasible value?

        drv_getAsicRegBit(EUROPA_REG_M2_ADDR,4,&status);
        //drv_getAsicRegBit(EUROPA_REG_FAULT_STATUS_ADDR,EUROPA_REG_FAULT_STATUS_TX_SD_FAULT_STATUS_OFFSET,&status);
        
        if (status !=1)
        {
            laser_drv_functionCtrl(EUROPA_LASER_FUN_TX,DISABLE); 
            laser_drv_functionCtrl(EUROPA_LASER_FUN_VDD_LDX,DISABLE);         
            //SW user define fault!! 
            //drv_setAsicRegBit(EUROPA_REG_SW_FAULT_STATUS1_ADDR, 0,1);
        }    
    }
    else
    {
        if (status !=0) //Fault Status 1
        {          
            if(status & EUROPA_REG_FAULT_STATUS_TX_FAULT_STATUS_MASK)
            {
                printk(KERN_EMERG "EUROPA_REG_FAULT_STATUS_TX_FAULT_STATUS_MASK\n");    
            }
        
            if(status & EUROPA_REG_FAULT_STATUS_TX_SD_FAULT_STATUS_MASK)
            {
                printk(KERN_EMERG "EUROPA_REG_FAULT_STATUS_TX_SD_FAULT_STATUS_MASK\n");    
            }
    
            if(status & EUROPA_REG_FAULT_STATUS_WDOG_TIMEOUT_FAULT_STATUS_MASK)
            {
                printk(KERN_EMERG "EUROPA_REG_FAULT_STATUS_WDOG_TIMEOUT_FAULT_STATUS_MASK\n");    
            }
        
            if(status & EUROPA_REG_FAULT_STATUS_BEN_TOO_LONG_FAULT_STATUS_MASK)
            {
                printk(KERN_EMERG "EUROPA_REG_FAULT_STATUS_BEN_TOO_LONG_FAULT_STATUS_MASK\n");    
            }
            
            if(status & EUROPA_REG_FAULT_STATUS_TX_LV_FAULT_STATUS_MASK)
            {
                printk(KERN_EMERG "EUROPA_REG_FAULT_STATUS_TX_LV_FAULT_STATUS_MASK\n");    
            }
        
            if(status & EUROPA_REG_FAULT_STATUS_OVER_IMPD_FAULT_STATUS_MASK)
            {
                printk(KERN_EMERG "EUROPA_REG_FAULT_STATUS_OVER_IMPD_FAULT_STATUS_MASK\n");    
            }
            if(status & EUROPA_REG_FAULT_STATUS_OVER_VOL_FAULT_STATUS_MASK)
            {
                printk(KERN_EMERG "EUROPA_REG_FAULT_STATUS_OVER_VOL_FAULT_STATUS_MASK\n");    
            }
        
            if(status & EUROPA_REG_FAULT_STATUS_OVER_TEMP_FAULT_STATUS_MASK)
            {
                printk(KERN_EMERG "EUROPA_REG_FAULT_STATUS_OVER_TEMP_FAULT_STATUS_MASK\n");    
            }        
        }
    
        if (status2 !=0) //Fault Status 2
        {          
            if(status & EUROPA_REG_FAULT_STATUS2_SW_FAULT_STATUS_MASK)
            {
                printk(KERN_EMERG "EUROPA_REG_FAULT_STATUS2_SW_FAULT_STATUS_MASK\n");    
            }
        }    
    }
    
}    

#if 0
void laser_drv_underRxOverPowerFaultReleaseIntrHandle(void) 
{
    uint16 status;
    /*get RX_OVER_POWER_ACTION bit*/    
    
    drv_getAsicRegBits(EUROPA_REG_CONTROL2_ADDR,EUROPA_REG_CONTROL2_RX_OVER_POWER_ACTION_MASK,&status);

    switch(status)
    {
        case EUROPA_RX_OVER_FAULT_INTR_BOOSTER:
            laser_drv_functionCtrl(EUROPA_LASER_FUN_BOOSTER,DISABLE);    
        case EUROPA_RX_OVER_FAULT_INTR_BOOSTER_RX:
            laser_drv_functionCtrl(EUROPA_LASER_FUN_RX,DISABLE);    
            break;    

        default :
            return;    
    }  

    drv_setAsicRegBit(EUROPA_REG_CONTROL3_ADDR,EUROPA_REG_CONTROL3_UNDER_RX_OVER_POWER_RELEASE_MASK,1);
    
    switch(status)
    {
        case EUROPA_RX_OVER_FAULT_INTR_BOOSTER:
            laser_drv_functionCtrl(EUROPA_LASER_FUN_BOOSTER,ENABLE);    
        case EUROPA_RX_OVER_FAULT_INTR_BOOSTER_RX:
            laser_drv_functionCtrl(EUROPA_LASER_FUN_RX,ENABLE);    
            break;    

        default :
            return;    
    }  
    
}
#endif

void rssi_err_intrHandle(void) 
{

    drv_setAsicRegBit(EUROPA_REG_M3_ADDR, 0x3, 0x1);
}

void rssi_err_intrRelease(void) 
{
    uint16 status;

      status = getReg(EUROPA_REG_INT_STATUS1_ADDR);

      if ((status & EUROPA_REG_INT_STATUS1_RSSI_ER_INT_STATUS_MASK) ==0)
      {
        drv_setAsicRegBit(EUROPA_REG_M3_ADDR, 0x3, 0);      
      }
}

void laser_drv_ddmi_tx_dis_intrHandle(uint16 status2) 
{
    uint16 tmp, tmp2;

    //A4/85[4](DDMI_TX_DIS_HW_ENABLE)=1 )
    drv_getAsicRegBit(EUROPA_REG_CONTROL3_ADDR, EUROPA_REG_CONTROL3_DDMI_TX_DIS_HW_ENABLE_OFFSET, &tmp);

    if (tmp == 1)
    {
        //read TX_DIS_Status form DDMI(A2/110[6])
        drv_getAsicRegBit(0x16E, 6, &tmp2);
        //Write the value  to DDMI_TX_DIS (A4/85[3])
        drv_setAsicRegBit(EUROPA_REG_CONTROL3_ADDR, EUROPA_REG_CONTROL3_DDMI_TX_DIS_OFFSET, tmp2);    

        if (tmp2 == 1)
        {
            //Make sure TX_SD =0(A5/194[4])
            drv_getAsicRegBit(EUROPA_REG_M2_ADDR, 4, &tmp2);
            if (tmp2 == 1)
            {
                laser_drv_functionCtrl(EUROPA_LASER_FUN_TX,DISABLE); 
                laser_drv_functionCtrl(EUROPA_LASER_FUN_VDD_LDX,DISABLE); 
            }
        }
        else
        {
             if(status2 & EUROPA_REG_INT_STATUS2_UNDER_TX_DIS_RELEASE_INT_MASK)
             {
                 laser_drv_underTxDisFaultReleaseIntrHandle();    
             }
        }

    }
      
      
}

void laser_drv_tx_dis_intrHandle(uint16 status2) 
{
    uint16 tmp, tmp2;

    /*Check TX_DIS_PIN=1(A5/131[3]) */
    drv_getAsicRegBit(EUROPA_REG_STATUS_2_ADDR, EUROPA_REG_STATUS_2_TX_DIS_STATUS_OFFSET, &tmp);    

    if (tmp == 1)
    {
        /*Make sure TX_SD =0(A5/194[4])*/
        drv_getAsicRegBit(EUROPA_REG_M2_ADDR, 4, &tmp2);
        if (tmp2 == 1)
        {
            laser_drv_functionCtrl(EUROPA_LASER_FUN_TX,DISABLE); 
            laser_drv_functionCtrl(EUROPA_LASER_FUN_VDD_LDX,DISABLE); 
        }
    }
    else
    {
         if(status2 & EUROPA_REG_INT_STATUS2_UNDER_TX_DIS_RELEASE_INT_MASK)
         {
             printk(KERN_EMERG "UNDER_TX_DIS_RELEASE_INT\n");             
              laser_drv_underTxDisFaultReleaseIntrHandle();    
         }
    }
      
}

void laser_drv_intrHandle(void) 
{
    uint16 status1, status2;
    uint16 tmp, mode;
    uint16 mask1, mask2; 
    uint16 bosa_type;

    status1 = getReg(EUROPA_REG_INT_STATUS1_ADDR); 
    status2 = getReg(EUROPA_REG_INT_STATUS2_ADDR);
    //printk(KERN_EMERG "laser_drv_intrHandle: status1 = 0x%x, status2 = 0x%x \n", status1, status2);	
    if ((status1 ==0)&&(status2 ==0))
        return;
	
    /* save original interrupt mask */
    mask1 = getReg(EUROPA_REG_INT_MASK1_ADDR);  
    mask2 = getReg(EUROPA_REG_INT_MASK2_ADDR);  
    //printk(KERN_EMERG "laser_drv_intrHandle: mask1 = 0x%x, mask2 = 0x%x \n", mask1, mask2);	  

    laser_drv_intrDisable();   

    /** REG_INT_STATUS1 handlers */
    //if (europa_debug_level_get())
    //printk(KERN_EMERG "europa_interrupt: status1 = 0x%X \n", status1);    

    if(status1 & EUROPA_REG_INT_STATUS1_TX_DIS_INT_STATUS_MASK)
    {
        printk(KERN_EMERG "TX_DIS_INT\n");    
        //laser_drv_tx_dis_intrHandle(status2);
    }

    if(status1 & EUROPA_REG_INT_STATUS1_DDMI_TX_DIS_INT_STATUS_MASK)
    {
        printk(KERN_EMERG "DDMI_TX_DIS_INT\n");    
        //laser_drv_ddmi_tx_dis_intrHandle(status2);
    }

    if(status1 & EUROPA_REG_INT_STATUS1_DDMI_WRITE_INT_STAUS_MASK)
    {
        printk(KERN_EMERG "DDMI_WRITE_INT\n");        
        tmp = getReg(EUROPA_REG_DDMI_INT_STATUS_ADDR); 
        //ddmi_intr_handle(tmp);
    }
    
    if(status1 & EUROPA_REG_INT_STATUS1_FAULT_RELEASE_INT_STATUS_MASK)
    {
        printk(KERN_EMERG "FAULT_RELEASE_INT\n");            
        /*handle fault release interrupt*/
        //drv_getAsicRegBits(EUROPA_REG_FAULT_CTL_ADDR,EUROPA_REG_FAULT_CTL_FAULT_ACTION_MASK,&tmp);        
        //laser_drv_faultReleaseIntrHandle(tmp);      
    }
    
    if(status1 & EUROPA_REG_INT_STATUS1_FAULT_INT_STATUS_MASK)
    {
        printk(KERN_EMERG "FAULT_INT !!!!\n");            
        //laser_drv_functionCtrl(EUROPA_LASER_FUN_TX,DISABLE); 
        //laser_drv_functionCtrl(EUROPA_LASER_FUN_VDD_LDX,DISABLE);         
        //laser_drv_functionCtrl(EUROPA_LASER_FUN_BOOSTER,DISABLE); 
        
        //laser_drv_fault_status();        
    }
    
    if(status1 & EUROPA_REG_INT_STATUS1_LOST_INT_STATUS_MASK)
    {
        printk(KERN_EMERG "LOST_INT\n");    
    /* TODO Lost Of Signal */
    }
    
    if(status1 & EUROPA_REG_INT_STATUS1_RSSI_ER_INT_STATUS_MASK)
    {
        printk(KERN_EMERG "RSSI_ER_INT\n");         
        //rssi_err_intrHandle();
        //rssi_err_intrRelease();        
    }

    //if(status1 & EUROPA_REG_INT_STATUS1_TEMP_CHANGE_INT_STATUS_MASK)
    //20200428: Also check mask for enable/disable rtl8290b_laserLut_set
    if((status1 & EUROPA_REG_INT_STATUS1_TEMP_CHANGE_INT_STATUS_MASK)&&((mask1 & EUROPA_REG_INT_STATUS1_TEMP_CHANGE_INT_STATUS_MASK)==0))
    {
        //printk(KERN_EMERG "TEMP_CHANGE_INT\n");          

#ifdef EUROPA_ENABLE_OFFK_DAEMON
    kick_OffKd();
#endif

    if ( !calibration_state_get())
    {
        if (EUROPA_CHIP_RTL8290 == europa_chip_type)
        {            
            /* For DOL/SCL mode, update BIAS and/or MOD DAC per Laser LUT */
            rtl8290_apdLut_set(&flash_data[EUROPA_APD_LUT_ADDR]);  			
            rtl8290_laserLut_set(loopmode, &flash_data[EUROPA_LASER_LUT_ADDR]);
        }
        else if (EUROPA_CHIP_RTL8290B == europa_chip_type)
        {
            rtl8290b_apcLoopMode_get(&mode);
            if (mode != 1)
            {
                //printk(KERN_EMERG "mode = 0x%x", mode); 
            }
            rtl8290b_laserLut_set(mode, &flash_data[RTL8290B_LASER_LUT_ADDR]);			
            bosa_type = flash_data[RTL8290B_PARAM_ADDR_BASE]; 
            //printk(KERN_EMERG "%s: europa_chip_type = 0x%x bosa_type = 0x%x\n", __FUNCTION__, europa_chip_type, bosa_type);  			
            if (RTL8290B_BOSA_APD == bosa_type) 
            {
                rtl8290b_apdLut_set(&flash_data[EUROPA_APD_LUT_ADDR]);             
            }
        }
        else
        {
            printk(KERN_EMERG "%s: europa_chip_type = 0x%x\n", __FUNCTION__, europa_chip_type);    
        }
    }
    }


    /** REG_INT_STATUS2 handlers */
    //if (europa_debug_level_get())
    //printk(KERN_EMERG "europa_interrupt: status2 = 0x%X \n", status2);    
    
    if(status2 & EUROPA_REG_INT_STATUS2_TX_SD_TOGGLE_INT_MASK)
    {
        printk(KERN_EMERG "TX_SD_TOGGLE_INT\n");             
    }

    if(status2 & EUROPA_REG_INT_STATUS2_UNDER_TX_DIS_RELEASE_INT_MASK)
    {
        printk(KERN_EMERG "UNDER_TX_DIS_RELEASE_INT\n");             
        //laser_drv_underTxDisFaultReleaseIntrHandle();    
    }

    if(status2 & EUROPA_REG_INT_STATUS2_UNDER_RX_OVER_POWER_RELEASE_INT_MASK)
    {
        printk(KERN_EMERG "UNDER_RX_OVER_POWER_RELEASE_INT\n");    
        //laser_drv_underRxOverPowerFaultReleaseIntrHandle();    
    }
    
    if(status2 & EUROPA_REG_INT_STATUS2_WDOG_TIMEOUT_FAULT_INT_STAUS_MASK)
    {
        printk(KERN_EMERG "WDOG_TIMEOUT_FAULT_INT\n");     
        //TODO //reset_8051();
    }
    
    if(status2 & EUROPA_REG_INT_STATUS2_RX_OVER_POWER_INT_STATUS_MASK)
    {
        printk(KERN_EMERG "RX_OVER_POWER_INT\n");    
        //laser_drv_functionCtrl(EUROPA_LASER_FUN_BOOSTER,DISABLE); 
    }
    
    if(status2 & EUROPA_REG_INT_STATUS2_SW_REQ_INT_STATUS_MASK)
    {
        printk(KERN_EMERG "SW_REQ_INT\n");
    }

    /* restore original interrupt mask */
    laser_drv_intrEnable(mask1, mask2) ;
}


void laser_drv_fault_status(void) 
{
    uint16 status,status2;
    
    /*get REG FAULT_ACTION bit*/      
    drv_getAsicRegBits(EUROPA_REG_FAULT_STATUS_ADDR,0xFF,&status);
    drv_getAsicRegBits(EUROPA_REG_FAULT_STATUS2_ADDR,0xFF,&status2);

    printk(KERN_EMERG ">>>> ");    

    if (status !=0) //Fault Status 1
    {          
        if(status & EUROPA_REG_FAULT_STATUS_TX_FAULT_STATUS_MASK)
        {
            uint16 r30, r31;
            printk(KERN_EMERG "TX_FAULT\n");    

            drv_getAsicRegBits(EUROPA_REG_R30_ADDR,0xFF,&r30);
            drv_getAsicRegBits(EUROPA_REG_R31_ADDR,0xFF,&r31);
            printk(KERN_EMERG "   R30=0x%02x R31=0x%02x\n", r30, r31);    

        }
    
        if(status & EUROPA_REG_FAULT_STATUS_TX_SD_FAULT_STATUS_MASK)
        {
            printk(KERN_EMERG "TX_SD_FAULT\n");    
        }

        if(status & EUROPA_REG_FAULT_STATUS_WDOG_TIMEOUT_FAULT_STATUS_MASK)
        {
            printk(KERN_EMERG "WDOG_TIMEOUT_FAULT\n");    
        }
    
        if(status & EUROPA_REG_FAULT_STATUS_BEN_TOO_LONG_FAULT_STATUS_MASK)
        {
            printk(KERN_EMERG "BEN_TOO_LONG_FAULT\n");    
        }
        
        if(status & EUROPA_REG_FAULT_STATUS_TX_LV_FAULT_STATUS_MASK)
        {
            printk(KERN_EMERG "TX_LV_FAULT\n");    
        }
    
        if(status & EUROPA_REG_FAULT_STATUS_OVER_IMPD_FAULT_STATUS_MASK)
        {
            printk(KERN_EMERG "OVER_IMPD_FAULT\n");    
        }
        if(status & EUROPA_REG_FAULT_STATUS_OVER_VOL_FAULT_STATUS_MASK)
        {
            printk(KERN_EMERG "OVER_VOL_FAULT\n");    
        }
    
        if(status & EUROPA_REG_FAULT_STATUS_OVER_TEMP_FAULT_STATUS_MASK)
        {
            printk(KERN_EMERG "OVER_TEMP_FAULT\n");    
        }        
    }

    if (status2 !=0) //Fault Status 2
    {          
        if(status & EUROPA_REG_FAULT_STATUS2_SW_FAULT_STATUS_MASK)
        {
            printk(KERN_EMERG "SW_FAULT\n");    
        }
    }
    
    printk(KERN_EMERG "\n");    
}

#endif

