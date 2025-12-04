#ifdef CONFIG_RTL8291_FEATURE
#include <linux/module.h>  /* Needed by all modules */
#include <linux/kernel.h>  /* Needed for KERN_ALERT */
#include <linux/delay.h>
#include <linux/spinlock.h>
#include <linux/gpio.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/version.h>

#include <linux/proc_fs.h>
#include <linux/fs.h>      // Needed by filp
#include <asm/uaccess.h>   // Needed by segment descriptors
//#include <osal/sem.h>

#include "include/common.h"
#include "include/rtl8291_reg_acc.h"
#include "include/rtl8291_init.h"
#include "include/rtl8291_api.h"
#include "include/rtl8291_reg_acc.h"

#include <rtk/i2c.h>
#if defined(CONFIG_COMMON_RT_API)
#include <rtk/rt/rt_i2c.h>
#endif
#include <rtk/switch.h>
#include <hal/chipdef/apollomp/rtk_apollomp_reg_struct.h>
#include <hal/mac/reg.h>
#include <common/error.h>
//#include <rtk/gpio.h>
//#include <rtk/irq.h>

//Should be removed and change to RL6591C version in future
#include "include/rtl8290c_reg_definition.h"
//#include "include/rtl8290b_api.h"
#include "include/rtl8291_reg_definition.h"

#define RTL8291_TXLUT_SZ 128

/*
 * Data Declaration
 */

static uint32 romcode_cfg_96_nctl_data[40] =
{
0x02, 0x10, 0x0,  0xc, 0x65, 0x14, 0x2,  0xc,
0x80, 0x14, 0x4,  0xc, 0x65, 0x12, 0x7,  0xc,
0xa5, 0x12, 0xc0, 0xc, 0xba, 0x12, 0xe4, 0xc, 
0x0b, 0x13, 0xef, 0xc, 0x13, 0x13, 0x13, 0xd, 
0x60, 0x13, 0xa7, 0xd, 0x9c, 0x14, 0xca, 0xd
};

func_i2c_write _ldd_i2c_write = NULL;
func_i2c_read _ldd_i2c_read = NULL;

uint8  internal;

uint8 ldd_pon_mode;

extern rtl8291_param_t ldd_param[LDD_DEVICE_NUM];

extern uint8 flash_data[LDD_FLASH_DATA_SIZE];

extern uint8 I2C_PORT;

rtk_ldd_mapper_t rtl8291_ldd_mapper =
{
    .ldd_i2c_init = rtl8291_i2c_init,
    .ldd_gpio_init = NULL,
    .ldd_reset = NULL,
    .ldd_calibration_state_set = NULL,
    .ldd_parameter_set = NULL,
    .ldd_parameter_get = NULL,
    .ldd_loop_mode_set = NULL,
    .ldd_laser_lut_set = NULL,
    .ldd_apd_lut_set = NULL,
    .ldd_apc_enable_set = NULL,
    .ldd_tx_enable_set = NULL,
    .ldd_rx_enable_set = NULL,
    .ldd_power_on_get = NULL,
    .ldd_tx_power_get = NULL,
    .ldd_rx_power_get = NULL,
    .ldd_rssi_voltage_get =NULL,
    .ldd_rssi_v0_get = NULL,
    .ldd_vdd_get = NULL,
    .ldd_mpd0_get = NULL,
    .ldd_temperature_get = NULL,
    .ldd_tx_bias_get = NULL,
    .ldd_tx_mod_get = NULL,
    .ldd_tx_bias_set = NULL,
    .ldd_tx_mod_set = NULL,
    .ldd_driver_version_get = rtl8291_driver_version_get,
    .ldd_steering_mode_set = NULL,
    .ldd_integration_mode_set = NULL,
    .ldd_steering_mode_fixup = NULL,
    .ldd_integration_mode_fixup = NULL,
    .ldd_config_refresh = NULL,
    .ldd_txCross_set = NULL,
    .ldd_txCross_get = NULL,
    .ldd_apcIavg_set= NULL,
    .ldd_apcIavg_get= NULL,
    .ldd_apcEr_set= NULL,
    .ldd_apcEr_get= NULL,
    .ldd_apcErTrim_set= NULL, 
    .ldd_apcErTrim_get= NULL,
    .ldd_rxlosRefDac_set= NULL,
    .ldd_rxlosRefDac_get= NULL,
    .ldd_rxlosHystSel_set=  NULL,      
    .ldd_rxlosHystSel_get= NULL,
    .ldd_chip_init = NULL,
    .ldd_api_test = NULL,    
    .ldd_reset2 = rtl8291_reset,
    .ldd_parameter2_set = rtl8291_parameter_set,
    .ldd_parameter2_get = rtl8291_parameter_get,
    .ldd_loop_mode2_set = rtl8291_apcLoopMode_set,
    .ldd_laserLut2_set = rtl8291_laserLut_set,
    .ldd_laserLut2_get = rtl8291_laserLut_get,    
    .ldd_apdLut2_set = rtl8291_apdLut_set,
    .ldd_apdLut2_get = rtl8291_apdLut_get,    
    .ldd_mpd02_get = NULL,
    .ldd_temperature2_get = rtl8291_temperature_get,
    .ldd_tx_bias2_get = rtl8291_tx_bias_get,
    .ldd_tx_mod2_get = rtl8291_tx_mod_get,  
    .ldd_tx_bias2_set = rtl8291_tx_bias_set,
    .ldd_tx_mod2_set = rtl8291_tx_mod_set,
    .ldd_txCross2_set = rtl8291_txCross_set,
    .ldd_txCross2_get = rtl8291_txCross_get,
    .ldd_apcIavg2_set= rtl8291_apcIavg_set,
    .ldd_apcIavg2_get= rtl8291_apcIavg_get,
    .ldd_apcEr2_set= rtl8291_apcEr_set,
    .ldd_apcEr2_get= rtl8291_apcEr_get,
    .ldd_apcErTrim2_set= NULL, 
    .ldd_apcErTrim2_get= NULL,
    .ldd_rxlosRefDac2_set= rtl8291_rxlosRefDac_set,
    .ldd_rxlosRefDac2_get= rtl8291_rxlosRefDac_get,
    .ldd_rxlosHystSel2_set=  rtl8291_rxlosHystSel_set,      
    .ldd_rxlosHystSel2_get= rtl8291_rxlosHystSel_get,
    .ldd_chip_init2 = rtl8291_chip_init,
    .ldd_rx_power2_get = rtl8291_rxPower_get,
    .ldd_tx_power2_get = rtl8291_txPower_get,
    .ldd_sdadc_code2_get = rtl8291_sdadc_code_get,
    .ldd_efuse2_set= rtl8291_efuse_set,
    .ldd_efuse2_get= rtl8291_efuse_get, 
    .ldd_vdd2_get = rtl8291_vdd_get,    
    .ldd_config2_refresh = rtl8291_config_refresh,
    .ldd_regBit_set = rtl8291_setRegBit,
    .ldd_regBit_get = rtl8291_getRegBit, 
    .ldd_regBits_set = rtl8291_setRegBits,
    .ldd_regBits_get = rtl8291_getRegBits, 
    .ldd_txEsConfig_set = rtl8291_txEsConfig_set,
    .ldd_txEsConfig_get = rtl8291_txEsConfig_get,
    .ldd_dac_set = rtl8291_dac_set,
    .ldd_dac_get = rtl8291_dac_get, 
#if 0
    .ldd_apcErTrim2_set= rtl8291_apcErTrim_set, 
    .ldd_apcErTrim2_get= rtl8291_apcErTrim_get,
#endif
};



void ldd_mapper_init(uint16 europa_chip_type)
{
    int32 ret =0;
    if (EUROPA_CHIP_RTL8291 == europa_chip_type)
    {
        printk(KERN_EMERG "Run rtl8291_ldd_mapper\n");
        ret = rtk_lddMapper_attach(&rtl8291_ldd_mapper);
        if(ret != 0)
        {
            printk(KERN_EMERG "rtk_lddMapper_attach fail  retval=%d \n",ret);
        }    
    }
#if 0
    if (EUROPA_CHIP_RTL8290 == europa_chip_type)
    {
        printk(KERN_EMERG "Run rtl8290_ldd_mapper\n");
        rtk_lddMapper_attach(&rtl8290_ldd_mapper);

    }
    else if (EUROPA_CHIP_RTL8290B == europa_chip_type)
    {
        
        printk(KERN_EMERG "Run rtl8290b_ldd_mapper\n");
        rtk_lddMapper_attach(&rtl8290b_ldd_mapper);


    }
#endif
    else
    {
        printk(KERN_EMERG "Mapper Fail!!! No such chip type!!!\n");        
    }

}

void ldd_mapper_exit(void)
{
    rtk_lddMapper_deattach();
}




int32  rtl8291_i2c_init(rtk_i2c_port_t i2cPort)
{
    rtk_enable_t enable;
    rtk_i2c_width_t width;
    int32 retval;
    uint32 clock;
    uint32 chipId, rev, subType;

#if defined(CONFIG_COMMON_RT_API)

    printk(KERN_EMERG "rtl8291_i2c_init: use RT_API, i2cPort = %d  \n", i2cPort);
    retval = rt_i2c_init(i2cPort);
    if(retval != 0)
    {
        printk(KERN_EMERG "rt_i2c_init fail  retval=%d \n",retval);
        return FAILED;
    }
    retval = rt_i2c_enable_set(i2cPort,ENABLED);
    if(retval != 0)
    {
        printk(KERN_EMERG "rt_i2c_enable_set fail    retval=%d \n",retval);
        return FAILED;
    }
    retval = rt_i2c_enable_get(i2cPort,&enable);
    if(retval != 0)
    {
        printk(KERN_EMERG "rt_i2c_enable_get fail    retval=%d \n",retval);
        return FAILED;
    }

    _ldd_i2c_write = rt_i2c_write;
    _ldd_i2c_read = rt_i2c_read;    

    retval = rt_i2c_enable_get(i2cPort,&enable);
    if(retval != 0)
    {
        printk(KERN_EMERG "rt_i2c_enable_get fail    retval=%d \n",retval);
        return FAILED;
    }

    /*retval = rt_i2c_clock_get(i2cPort,&clock);
    if(retval != 0)
    {
        printk(KERN_EMERG "rtk_i2c_clock_get fail  retval=%d \n",retval);
        return FAILED;
    }
    
    printk(KERN_EMERG "rtl8291_i2c_init  enable = %d clock = %d \n",enable, clock);*/
    
#else

    printk(KERN_EMERG "rtl8291_i2c_init: use RTK_API, i2cPort = %d  \n", i2cPort);

    retval = rtk_i2c_init(i2cPort);
    if(retval != 0)
    {
        printk(KERN_EMERG "rtk_i2c_init fail  retval=%d \n",retval);
        return FAILED;
    }
    retval = rtk_i2c_enable_set(i2cPort,ENABLED);
    if(retval != 0)
    {
        printk(KERN_EMERG "rtk_i2c_enable_set fail  retval=%d \n",retval);
        return FAILED;
    }
    retval = rtk_i2c_enable_get(i2cPort,&enable);
    if(retval != 0)
    {
        printk(KERN_EMERG "rtk_i2c_enable_get fail  retval=%d \n",retval);
        return FAILED;
    }

    if((retval = rtk_switch_version_get(&chipId, &rev, &subType)) != 0)
    {
        return retval;
    }

#ifdef RTL8290B_I2C_NOWAIT          
    if (RTL9601B_CHIP_ID == chipId)
    {
        _ldd_i2c_write = rtk_i2c_noWait_write;
    }
    else
    {
        _ldd_i2c_write = rtk_i2c_write;
    }
#else
    _ldd_i2c_write = rtk_i2c_write;
#endif
    _ldd_i2c_read = rtk_i2c_read;
    
#endif

#ifndef CONFIG_LUNA_G3_SERIES //Original config. 
    retval = rtk_i2c_width_set(i2cPort,I2C_WIDTH_8bit);
    if(retval != 0)
    {
        printk(KERN_EMERG "rtk_i2c_width_set fail  retval=%d \n",retval);
        return FAILED;
    }
    //Set to 800KHz for 8290C (1MHz failed!!!)
    retval = rtk_i2c_clock_set(i2cPort,800);
    if(retval != 0)
    {
        printk(KERN_EMERG "rtk_i2c_clock_set fail  retval=%d \n",retval);
        return FAILED;
     }
    retval = rtk_i2c_width_get(i2cPort,&width);
    if(retval != 0)
    {
        printk(KERN_EMERG "rtk_i2c_width_get fail  retval=%d \n",retval);
        return FAILED;
    }     
#else //For 9607DQ ARM based SDK
    //Set to 400KHz in diagshell mode
    retval = rtk_i2c_clock_set(i2cPort,400);
    if(retval != 0)
    {
        printk(KERN_EMERG "rtk_i2c_clock_set fail  retval=%d \n",retval);
        return FAILED;
    }
#endif

    retval = rtk_i2c_enable_get(i2cPort,&enable);
    if(retval != 0)
    {
        printk(KERN_EMERG "rtk_i2c_enable_get fail  retval=%d \n",retval);
        return FAILED;
    }

    retval = rtk_i2c_clock_get(i2cPort,&clock);
    if(retval != 0)
    {
        printk(KERN_EMERG "rtk_i2c_clock_get fail  retval=%d \n",retval);
        return FAILED;
    }

    printk(KERN_EMERG "rtl8291_i2c_init  enable = %d clock = %d \n",enable, clock);

    return SUCCESS;
}  


int _rtl8291_load_config_file(uint32 devId, uint8 *flash_data)
{
    struct file *f;
    //struct inode *inode;
    //off_t fsize;
    mm_segment_t fs;
    //uint32 value, value2;
    loff_t pos;    

    if (devId == RTL8291_DEFAULT_DEVID)
    {
        f = filp_open(RTL8291_DEVICE_0_FILE, O_RDONLY, 0);
        printk(KERN_EMERG "Open file: %s\n",RTL8291_DEVICE_0_FILE);
    }
    else if (devId == 1)
    {
        f = filp_open(RTL8291_DEVICE_1_FILE, O_RDONLY, 0);
        printk(KERN_EMERG "Open file: %s\n", RTL8291_DEVICE_1_FILE);
    }
    else if (devId == 2)
    {
        f = filp_open(RTL8291_DEVICE_2_FILE, O_RDONLY, 0);
        printk(KERN_EMERG "Open file: %s\n",RTL8291_DEVICE_2_FILE);
    }
    else if (devId == 3)
    {
        f = filp_open(RTL8291_DEVICE_3_FILE, O_RDONLY, 0);
        printk(KERN_EMERG "Open file: %s\n", RTL8291_DEVICE_3_FILE);
    }    
    else if (devId == 4)
    {
        f = filp_open(RTL8291_DEVICE_4_FILE, O_RDONLY, 0);
        printk(KERN_EMERG "Open file: %s\n", RTL8291_DEVICE_4_FILE);
    }    
    else
    {
        printk(KERN_EMERG "Devic ID overflow ID = %d \n", devId);    
        return 0;    
    }
    
    printk(KERN_EMERG "Read flash config data...\n");    

    if( f != NULL)
    {
        memset(flash_data,0,RTL8291_FLASH_DATA_SIZE*sizeof(uint8));

#if LINUX_VERSION_CODE < KERNEL_VERSION(5,0,0)
        // Get current segment descriptor
        printk(KERN_EMERG "Get current segment descriptor....\n");          
        fs = get_fs();
        // Set segment descriptor associated to kernel space
        printk(KERN_EMERG "Set segment descriptor associated to kernel space....\n");    
        set_fs(get_ds());
        
#elif LINUX_VERSION_CODE < KERNEL_VERSION(5,10,0)
        fs = get_fs();//Save the current FS segment
        set_fs(KERNEL_DS);
#else
        fs = force_uaccess_begin();
#endif
    
        // Read the file
#if LINUX_VERSION_CODE <= KERNEL_VERSION(4,13,0)
        pos =0;
        vfs_read(f,flash_data, RTL8291_FLASH_DATA_SIZE, &pos);         
        printk(KERN_EMERG "Read the file(under 4.13)....\n");
#else
        pos =0;
        kernel_read(f,flash_data, RTL8291_FLASH_DATA_SIZE, &pos);
        printk(KERN_EMERG "Read the file(over 4.13)....\n");
#endif


        // Restore segment descriptor
        printk(KERN_EMERG "Restore segment descriptor....\n");            
        filp_close(f,NULL);

#if LINUX_VERSION_CODE < KERNEL_VERSION(5,10,0)
        set_fs(fs);
#else
        force_uaccess_end(fs);
#endif

        printk(KERN_EMERG "Loaded data finish.\n");

    }
    else
    {
        printk(KERN_EMERG "filp_open error!!.\n");
    }

    return 0;
}


void _rtl8291_ddmi_a0_init(uint32 devId, uint8 *flash_data)
{
    uint16 addr;
    uint8 *ddmi_a0_reg;
    uint32 regData;

    ddmi_a0_reg = flash_data;

    //printk("Update DDMI A0....,  addr = 0x%x\n ", (unsigned int)flash_data);

    for(addr=0; addr <= 0xFF ; addr++)
    {
        rtl8291_setReg(devId, addr, ddmi_a0_reg[addr]);
    }
}

void _rtl8291_ddmi_a2_init(uint32 devId, uint8 *flash_data)
{
    uint16 addr;
    uint8 *ddmi_a2_reg;
    uint32 regData;

    ddmi_a2_reg = flash_data;

    //printk("Update DDMI A2...., addr = 0x%x\n ", (unsigned int)flash_data);

    for(addr=RTL8291_CFG_TEM_HI_ALAR_TH_HB_ADDR; addr <= RTL8291_A2L_REG7E_ADDR ; addr++)
    {
        regData =  (uint32)ddmi_a2_reg[addr];   
        rtl8291_setReg(devId, addr, ddmi_a2_reg[addr]);
#if 0
        if (addr%16==0)
            printk(KERN_EMERG " addr = 0x%04X  ", addr);

         printk(KERN_EMERG"%02X ", regData);

         if(addr%8 == 7)
            printk(KERN_EMERG"   ");
         if(addr%16 == 15)
            printk(KERN_EMERG "\n");
#endif
    }
}


void _rtl8291_apd_lut_init(uint32 devId, uint8 *flash_data)
{
    uint16 addr;
    uint8 *apd_lut_reg;
    uint32 regData;

    apd_lut_reg = flash_data;

    //printk("Update APD LUT...., addr = 0x%x\n ", (unsigned int)flash_data);

    for(addr=RTL8291_APD_LUT_REG00_ADDR; addr <= RTL8291_APD_LUT_REG7F_ADDR ; addr++)
    {
        regData =  (uint32)apd_lut_reg[addr];
        rtl8291_setReg(devId, addr, regData);
#if 0
        if (addr%16==0)
            printk(" addr = %d  ", addr);

        printk("%02X ",apd_lut_reg[addr] );

        if(addr%8 == 7)
            printk("   ");
        if(addr%16 == 15)
            printk( "\n");
#endif
    }
}

void _rtl8291_laser_lut_init(uint32 devId, uint8 *flash_data)
{
    uint16 addr;
    uint8 *laser_lut_reg;
    uint32 regData;

    laser_lut_reg = flash_data;

    //printk("Update Laser LUT...., addr = 0x%x\n ", (unsigned int)flash_data);

    for(addr=RTL8291_TX_LUT_REG00_ADDR; addr <= RTL8291_TX_LUT_REG7F_ADDR ; addr++)
    {
        regData =  (uint32)laser_lut_reg[addr];    
        rtl8291_setReg(devId, addr, regData);
#if 0
        if (addr%16==0)
            printk(KERN_EMERG " addr = 0x%04X  ", addr);

        printk(KERN_EMERG "%02X ", regData);

        if(addr%8 == 7)
            printk(KERN_EMERG "   ");
        if(addr%16 == 15)
            printk(KERN_EMERG "\n");
#endif
    }
}

void rtl8291_default_init(uint32 devId, uint8 *flash_data)
{
    printk(KERN_EMERG "Update default A0/A2/APDLUT/TXLUT value... \n");    
    _rtl8291_ddmi_a0_init(devId, flash_data);
    _rtl8291_ddmi_a2_init(devId, flash_data);
    _rtl8291_apd_lut_init(devId, flash_data);
    _rtl8291_laser_lut_init(devId, flash_data);
    return;
}

void _8291_data_trans(uint8 *ptr_data, uint32 length, uint32 *value)
{
    uint8  i, temp8;
    uint32 temp32;

    if ((length==0)||(length>4))
    {
         printk(KERN_EMERG "Data Length Error!!!!!!!\n");
         return;
    }
    temp32 = 0;
    for(i=0;i<length;i++)
    {
        temp8 = *ptr_data;
        temp32 = temp32 | ((uint32)temp8<<(8*((length-1)-i)));
        ptr_data++;
    }

    //printk(KERN_EMERG "%s = 0x%x!!!!!!!\n", __FUNCTION__, temp32);

    *value = temp32;
    return;
}

void rtl8291_romcode_patch(uint32 devId)
{
    uint32 i=0, data=0;
    uint32 cnt=0, status=0;

    // set bit-2 is 1
    rtl8291_setReg(devId, RTL8291_REG_CONTROL1_ADDR, 0x4);
    //printk(KERN_EMERG "SET_DREG $REG_CONTROL1  0x4\n");    
    // set bit-3 is 1
    rtl8291_setReg(devId, RTL8291_REG_IIC_S_ADDR, 0x1C);
    //printk(KERN_EMERG "SET_DREG $REG_IIC_S   0x1C\n");    

    for (i=0;i<RTL8291_ROMCODE_105_PATCH_SIZE;i++)
    {
        rtl8291_setReg(devId, RTL8291_REG_INDACC_ADDR_LB_ADDR, romcode_105_data[i][0]);
        //printk(KERN_EMERG "SET_DREG $REG_INDACC_ADDR_LB  0x%02x \n", romcode_105_data[i][0]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_ADDR_HB_ADDR, romcode_105_data[i][1]);
        //printk(KERN_EMERG "SET_DREG $REG_INDACC_ADDR_HB  0x%02x \n", romcode_105_data[i][1]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_0_ADDR, romcode_105_data[i][2]);
        //printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_0  0x%02x \n", romcode_105_data[i][2]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_1_ADDR, romcode_105_data[i][3]);
        //printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_1  0x%02x \n", romcode_105_data[i][3]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_2_ADDR, romcode_105_data[i][4]);
        //printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_2  0x%02x \n", romcode_105_data[i][4]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_3_ADDR, romcode_105_data[i][5]);
        //printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_3  0x%02x \n", romcode_105_data[i][5]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_4_ADDR, romcode_105_data[i][6]);
        //printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_4  0x%02x \n", romcode_105_data[i][6]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_5_ADDR, romcode_105_data[i][7]);
        //printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_5  0x%02x \n", romcode_105_data[i][7]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_6_ADDR, romcode_105_data[i][8]);
        //printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_6  0x%02x \n", romcode_105_data[i][8]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_7_ADDR, romcode_105_data[i][9]);
        //printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_7  0x%02x \n", romcode_105_data[i][9]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_8_ADDR, romcode_105_data[i][10]);
        //printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_8  0x%02x \n", romcode_105_data[i][10]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_9_ADDR, romcode_105_data[i][11]);
        //printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_9  0x%02x \n", romcode_105_data[i][11]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_10_ADDR, romcode_105_data[i][12]);
        //printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_10  0x%02x \n", romcode_105_data[i][12]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_11_ADDR, romcode_105_data[i][13]);
        //printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_11  0x%02x \n", romcode_105_data[i][13]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_12_ADDR, romcode_105_data[i][14]);
        //printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_12  0x%02x \n", romcode_105_data[i][14]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_13_ADDR, romcode_105_data[i][15]);
        //printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_13  0x%02x \n", romcode_105_data[i][15]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_14_ADDR, romcode_105_data[i][16]);
        //printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_14  0x%02x \n", romcode_105_data[i][16]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_15_ADDR, romcode_105_data[i][17]);
        //printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_15  0x%02x \n", romcode_105_data[i][17]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_CONTROL_ADDR, romcode_105_data[i][18]);
        //printk(KERN_EMERG "SET_DREG $REG_INDACC_CONTROL  0x%02x  ;# // set 16-byte write\n\n", romcode_105_data[i][18]);
        //Poll NCTRL busy bit
        cnt=0;
        while(cnt<1000) //check NCTRL BUSY
        {
            rtl8291_getRegBit(devId, RTL8291_REG_INDACC_CONTROL_ADDR, RTL8291_REG_INDACC_CONTROL_INDACC_CMD_OFFSET, &status);
            if(status == 0)
            {
                break;
            }
            cnt++;
        }
        rtl8291_getReg(devId, RTL8291_REG_INDACC_CONTROL_ADDR, &status);
        //printk(KERN_EMERG "idx = %4d REG_INDACC_CONTROL = 0x%x Busy Cnt= %d \n\n", i, status, cnt);
    }

    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P00_L_ADDR, 0x02);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P00_H_ADDR, 0x10);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P00_L_ADDR, 0x0);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P00_H_ADDR, 0xc);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P01_L_ADDR, 0x65);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P01_H_ADDR, 0x14);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P01_L_ADDR, 0x2);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P01_H_ADDR, 0xc);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P02_L_ADDR, 0x80);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P02_H_ADDR, 0x14);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P02_L_ADDR, 0x4);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P02_H_ADDR, 0xc);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P03_L_ADDR, 0x65);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P03_H_ADDR, 0x12);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P03_L_ADDR, 0x7);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P03_H_ADDR, 0xc);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P04_L_ADDR, 0xa5);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P04_H_ADDR, 0x12);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P04_L_ADDR, 0xbd);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P04_H_ADDR, 0xc);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P05_L_ADDR, 0xba);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P05_H_ADDR, 0x12);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P05_L_ADDR, 0xe1);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P05_H_ADDR, 0xc);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P06_L_ADDR, 0x0b);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P06_H_ADDR, 0x13);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P06_L_ADDR, 0xec);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P06_H_ADDR, 0xc);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P07_L_ADDR, 0x13);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P07_H_ADDR, 0x13);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P07_L_ADDR, 0x10);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P07_H_ADDR, 0xd);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P08_L_ADDR, 0x60);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P08_H_ADDR, 0x13);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P08_L_ADDR, 0xa0);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P08_H_ADDR, 0xd);
    // set bit-3 is 0
    rtl8291_setReg(devId, RTL8291_REG_IIC_S_ADDR, 0x15);     
    // set bit-2 is 0
    rtl8291_setReg(devId, RTL8291_REG_CONTROL1_ADDR, 0);   

    return;
}

void rtl8291_romcode_96_patch(uint32 devId, int dbg_en)
{
    uint32 i=0, data=0;
    uint32 cnt=0, status=0;

    printk(KERN_EMERG "rtl8291_romcode_96_patch...\n"); 


    // set bit-2 is 1
    rtl8291_setReg(devId, RTL8291_REG_CONTROL1_ADDR, 0x4);
    if (dbg_en>0)
    {    
        printk(KERN_EMERG "SET_DREG $REG_CONTROL1  0x4\n"); 
    }
    // set bit-3 is 1
    rtl8291_setReg(devId, RTL8291_REG_IIC_S_ADDR, 0x1C);
    if (dbg_en>0)
    {    
        printk(KERN_EMERG "SET_DREG $REG_IIC_S   0x1C\n");    
    }
    for (i=0;i<RTL8291_ROMCODE_96_PATCH_SIZE;i++)
    {
        if (i<(RTL8291_ROMCODE_96_PATCH_SIZE-1))
        {
        rtl8291_setReg(devId, RTL8291_REG_INDACC_ADDR_LB_ADDR, romcode_96_data[i][0]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_ADDR_HB_ADDR, romcode_96_data[i][1]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_0_ADDR, romcode_96_data[i][2]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_1_ADDR, romcode_96_data[i][3]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_2_ADDR, romcode_96_data[i][4]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_3_ADDR, romcode_96_data[i][5]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_4_ADDR, romcode_96_data[i][6]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_5_ADDR, romcode_96_data[i][7]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_6_ADDR, romcode_96_data[i][8]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_7_ADDR, romcode_96_data[i][9]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_8_ADDR, romcode_96_data[i][10]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_9_ADDR, romcode_96_data[i][11]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_10_ADDR, romcode_96_data[i][12]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_11_ADDR, romcode_96_data[i][13]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_12_ADDR, romcode_96_data[i][14]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_13_ADDR, romcode_96_data[i][15]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_14_ADDR, romcode_96_data[i][16]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_15_ADDR, romcode_96_data[i][17]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_CONTROL_ADDR, romcode_96_data[i][18]);
        if (dbg_en>0)
        {
            printk(KERN_EMERG "SET_DREG $REG_INDACC_ADDR_LB  0x%02x \n", romcode_96_data[i][0]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_ADDR_HB  0x%02x \n", romcode_96_data[i][1]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_0  0x%02x \n", romcode_96_data[i][2]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_1  0x%02x \n", romcode_96_data[i][3]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_2  0x%02x \n", romcode_96_data[i][4]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_3  0x%02x \n", romcode_96_data[i][5]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_4  0x%02x \n", romcode_96_data[i][6]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_5  0x%02x \n", romcode_96_data[i][7]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_6  0x%02x \n", romcode_96_data[i][8]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_7  0x%02x \n", romcode_96_data[i][9]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_8  0x%02x \n", romcode_96_data[i][10]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_9  0x%02x \n", romcode_96_data[i][11]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_10  0x%02x \n", romcode_96_data[i][12]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_11  0x%02x \n", romcode_96_data[i][13]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_12  0x%02x \n", romcode_96_data[i][14]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_13  0x%02x \n", romcode_96_data[i][15]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_14  0x%02x \n", romcode_96_data[i][16]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_15  0x%02x \n", romcode_96_data[i][17]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_CONTROL  0x%02x  ;# // set 16-byte write\n\n", romcode_96_data[i][18]);
        }        
        //Poll NCTRL busy bit
        cnt=0;
        while(cnt<1000) //check NCTRL BUSY
        {
            rtl8291_getRegBit(devId, RTL8291_REG_INDACC_CONTROL_ADDR, RTL8291_REG_INDACC_CONTROL_INDACC_CMD_OFFSET, &status);
            if(status == 0)
            {
                break;
            }
            cnt++;
        }
        rtl8291_getReg(devId, RTL8291_REG_INDACC_CONTROL_ADDR, &status);
        if (dbg_en>0)
        {        
            printk(KERN_EMERG "REG_INDACC_CONTROL = 0x%x Busy Cnt= %d \n\n", status, cnt);
        }
        }
        else
        {
            rtl8291_setReg(devId, RTL8291_REG_INDACC_ADDR_LB_ADDR, romcode_96_data[i][0]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_ADDR_HB_ADDR, romcode_96_data[i][1]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_0_ADDR, romcode_96_data[i][2]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_1_ADDR, romcode_96_data[i][3]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_2_ADDR, romcode_96_data[i][4]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_3_ADDR, romcode_96_data[i][5]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_4_ADDR, romcode_96_data[i][6]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_5_ADDR, romcode_96_data[i][7]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_6_ADDR, romcode_96_data[i][8]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_7_ADDR, romcode_96_data[i][9]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_CONTROL_ADDR, romcode_96_data[i][18]);
            if (dbg_en>0)
            {
                printk(KERN_EMERG "SET_DREG $REG_INDACC_ADDR_LB  0x%02x \n", romcode_96_data[i][0]);
                printk(KERN_EMERG "SET_DREG $REG_INDACC_ADDR_HB  0x%02x \n", romcode_96_data[i][1]);
                printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_0  0x%02x \n", romcode_96_data[i][2]);
                printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_1  0x%02x \n", romcode_96_data[i][3]);
                printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_2  0x%02x \n", romcode_96_data[i][4]);
                printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_3  0x%02x \n", romcode_96_data[i][5]);
                printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_4  0x%02x \n", romcode_96_data[i][6]);
                printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_5  0x%02x \n", romcode_96_data[i][7]);
                printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_6  0x%02x \n", romcode_96_data[i][8]);
                printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_7  0x%02x \n", romcode_96_data[i][9]);
                printk(KERN_EMERG "SET_DREG $REG_INDACC_CONTROL  0x%02x  ;# // set 16-byte write\n\n", romcode_96_data[i][18]);
            }                    
            //Poll NCTRL busy bit
            cnt=0;
            while(cnt<1000) //check NCTRL BUSY
            {
                rtl8291_getRegBit(devId, RTL8291_REG_INDACC_CONTROL_ADDR, RTL8291_REG_INDACC_CONTROL_INDACC_CMD_OFFSET, &status);
                if(status == 0)
                {
                    break;
                }
                cnt++;
            }
            rtl8291_getReg(devId, RTL8291_REG_INDACC_CONTROL_ADDR, &status);
            if (dbg_en>0)
            {            
                printk(KERN_EMERG "REG_INDACC_CONTROL = 0x%x Busy Cnt= %d \n\n", status, cnt);
            }
        }
    }

    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P00_L_ADDR, romcode_cfg_96_nctl_data[0]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P00_H_ADDR, romcode_cfg_96_nctl_data[1]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P00_L_ADDR, romcode_cfg_96_nctl_data[2]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P00_H_ADDR, romcode_cfg_96_nctl_data[3]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P01_L_ADDR, romcode_cfg_96_nctl_data[4]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P01_H_ADDR, romcode_cfg_96_nctl_data[5]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P01_L_ADDR, romcode_cfg_96_nctl_data[6]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P01_H_ADDR, romcode_cfg_96_nctl_data[7]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P02_L_ADDR, romcode_cfg_96_nctl_data[8]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P02_H_ADDR, romcode_cfg_96_nctl_data[9]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P02_L_ADDR, romcode_cfg_96_nctl_data[10]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P02_H_ADDR, romcode_cfg_96_nctl_data[11]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P03_L_ADDR, romcode_cfg_96_nctl_data[12]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P03_H_ADDR, romcode_cfg_96_nctl_data[13]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P03_L_ADDR, romcode_cfg_96_nctl_data[14]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P03_H_ADDR, romcode_cfg_96_nctl_data[15]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P04_L_ADDR, romcode_cfg_96_nctl_data[16]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P04_H_ADDR, romcode_cfg_96_nctl_data[17]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P04_L_ADDR, romcode_cfg_96_nctl_data[18]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P04_H_ADDR, romcode_cfg_96_nctl_data[19]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P05_L_ADDR, romcode_cfg_96_nctl_data[20]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P05_H_ADDR, romcode_cfg_96_nctl_data[21]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P05_L_ADDR, romcode_cfg_96_nctl_data[22]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P05_H_ADDR, romcode_cfg_96_nctl_data[23]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P06_L_ADDR, romcode_cfg_96_nctl_data[24]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P06_H_ADDR, romcode_cfg_96_nctl_data[25]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P06_L_ADDR, romcode_cfg_96_nctl_data[26]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P06_H_ADDR, romcode_cfg_96_nctl_data[27]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P07_L_ADDR, romcode_cfg_96_nctl_data[28]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P07_H_ADDR, romcode_cfg_96_nctl_data[29]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P07_L_ADDR, romcode_cfg_96_nctl_data[30]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P07_H_ADDR, romcode_cfg_96_nctl_data[31]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P08_L_ADDR, romcode_cfg_96_nctl_data[32]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P08_H_ADDR, romcode_cfg_96_nctl_data[33]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P08_L_ADDR, romcode_cfg_96_nctl_data[34]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P08_H_ADDR, romcode_cfg_96_nctl_data[35]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P09_L_ADDR, romcode_cfg_96_nctl_data[36]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P09_H_ADDR, romcode_cfg_96_nctl_data[37]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P09_L_ADDR, romcode_cfg_96_nctl_data[38]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P09_H_ADDR, romcode_cfg_96_nctl_data[39]);
     if (dbg_en>0)
    {
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P00_L  0x%x \n", romcode_cfg_96_nctl_data[0]);
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P00_H  0x%x \n", romcode_cfg_96_nctl_data[1]);
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P00_L  0x%x \n", romcode_cfg_96_nctl_data[2]);
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P00_H  0x%x \n", romcode_cfg_96_nctl_data[3]);
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P01_L  0x%x \n", romcode_cfg_96_nctl_data[4]);
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P01_H  0x%x \n", romcode_cfg_96_nctl_data[5]);
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P01_L  0x%x \n", romcode_cfg_96_nctl_data[6]);
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P01_H  0x%x \n", romcode_cfg_96_nctl_data[7]);
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P02_L  0x%x \n", romcode_cfg_96_nctl_data[8]);
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P02_H  0x%x \n", romcode_cfg_96_nctl_data[9]);
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P02_L  0x%02x \n", romcode_cfg_96_nctl_data[10]);
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P02_H  0x%x \n", romcode_cfg_96_nctl_data[11]);
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P03_L  0x%x \n", romcode_cfg_96_nctl_data[12]);
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P03_H  0x%x \n", romcode_cfg_96_nctl_data[13]);
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P03_L  0x%x \n", romcode_cfg_96_nctl_data[14]);
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P03_H  0x%x \n", romcode_cfg_96_nctl_data[15]);
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P04_L  0x%x \n", romcode_cfg_96_nctl_data[16]);
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P04_H  0x%x \n", romcode_cfg_96_nctl_data[17]);
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P04_L  0x%x \n", romcode_cfg_96_nctl_data[18]);
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P04_H  0x%x \n", romcode_cfg_96_nctl_data[19]);
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P05_L  0x%x \n", romcode_cfg_96_nctl_data[20]);
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P05_H  0x%x \n", romcode_cfg_96_nctl_data[21]);
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P05_L  0x%02x \n", romcode_cfg_96_nctl_data[22]);
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P05_H  0x%x \n", romcode_cfg_96_nctl_data[23]);
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P06_L  0x%x \n", romcode_cfg_96_nctl_data[24]);
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P06_H  0x%x \n", romcode_cfg_96_nctl_data[25]);
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P06_L  0x%x \n", romcode_cfg_96_nctl_data[26]);
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P06_H  0x%x \n", romcode_cfg_96_nctl_data[27]);
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P07_L  0x%x \n", romcode_cfg_96_nctl_data[28]);
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P07_H  0x%x \n", romcode_cfg_96_nctl_data[29]);
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P07_L  0x%x \n", romcode_cfg_96_nctl_data[30]);
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P07_H  0x%x \n", romcode_cfg_96_nctl_data[31]);
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P08_L  0x%x \n", romcode_cfg_96_nctl_data[32]);
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P08_H  0x%x \n", romcode_cfg_96_nctl_data[33]);
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P08_L  0x%02x \n", romcode_cfg_96_nctl_data[34]);
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P08_H  0x%x \n", romcode_cfg_96_nctl_data[35]);
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P09_L  0x%x \n", romcode_cfg_96_nctl_data[36]);
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P09_H  0x%x \n", romcode_cfg_96_nctl_data[37]);
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P09_L  0x%x \n", romcode_cfg_96_nctl_data[38]);
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P09_H  0x%x \n", romcode_cfg_96_nctl_data[39]);
    }

    // set bit-3 is 0
    rtl8291_setReg(devId, RTL8291_REG_IIC_S_ADDR, 0x15);
     if (dbg_en>0)
    {    
        printk(KERN_EMERG "SET_DREG $REG_IIC_S   0x15\n");
     }
    // set bit-2 is 0
    rtl8291_setReg(devId, RTL8291_REG_CONTROL1_ADDR, 0);
    if (dbg_en>0)
    {
        printk(KERN_EMERG "SET_DREG $REG_CONTROL1  0\n");    
    }
    return;
}

void rtl8291_romcode_100_patch(uint32 devId, int dbg_en)
{
    uint32 i=0, data=0;
    uint32 cnt=0, status=0;

    printk(KERN_EMERG "rtl8291_romcode_100_patch...\n"); 
 
    // set bit-2 is 1
    rtl8291_setReg(devId, RTL8291_REG_CONTROL1_ADDR, 0x4);
    if (dbg_en>0)
    {    
        printk(KERN_EMERG "SET_DREG $REG_CONTROL1  0x4\n");
    }
    // set bit-3 is 1
    rtl8291_setReg(devId, RTL8291_REG_IIC_S_ADDR, 0x1C);
    if (dbg_en>0)
    {
        printk(KERN_EMERG "SET_DREG $REG_IIC_S   0x1C\n");    
    }
    for (i=0;i<RTL8291_ROMCODE_100_PATCH_SIZE;i++)
    {
        if (i<(RTL8291_ROMCODE_100_PATCH_SIZE-1))
        {
            rtl8291_setReg(devId, RTL8291_REG_INDACC_ADDR_LB_ADDR, romcode_100_data[i][0]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_ADDR_HB_ADDR, romcode_100_data[i][1]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_0_ADDR, romcode_100_data[i][2]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_1_ADDR, romcode_100_data[i][3]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_2_ADDR, romcode_100_data[i][4]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_3_ADDR, romcode_100_data[i][5]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_4_ADDR, romcode_100_data[i][6]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_5_ADDR, romcode_100_data[i][7]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_6_ADDR, romcode_100_data[i][8]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_7_ADDR, romcode_100_data[i][9]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_8_ADDR, romcode_100_data[i][10]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_9_ADDR, romcode_100_data[i][11]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_10_ADDR, romcode_100_data[i][12]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_11_ADDR, romcode_100_data[i][13]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_12_ADDR, romcode_100_data[i][14]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_13_ADDR, romcode_100_data[i][15]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_14_ADDR, romcode_100_data[i][16]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_15_ADDR, romcode_100_data[i][17]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_CONTROL_ADDR, romcode_100_data[i][18]);

        if (dbg_en>0)
        {
            printk(KERN_EMERG "SET_DREG $REG_INDACC_ADDR_LB  0x%02x \n", romcode_100_data[i][0]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_ADDR_HB  0x%02x \n", romcode_100_data[i][1]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_0  0x%02x \n", romcode_100_data[i][2]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_1  0x%02x \n", romcode_100_data[i][3]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_2  0x%02x \n", romcode_100_data[i][4]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_3  0x%02x \n", romcode_100_data[i][5]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_4  0x%02x \n", romcode_100_data[i][6]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_5  0x%02x \n", romcode_100_data[i][7]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_6  0x%02x \n", romcode_100_data[i][8]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_7  0x%02x \n", romcode_100_data[i][9]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_8  0x%02x \n", romcode_100_data[i][10]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_9  0x%02x \n", romcode_100_data[i][11]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_10  0x%02x \n", romcode_100_data[i][12]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_11  0x%02x \n", romcode_100_data[i][13]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_12  0x%02x \n", romcode_100_data[i][14]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_13  0x%02x \n", romcode_100_data[i][15]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_14  0x%02x \n", romcode_100_data[i][16]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_15  0x%02x \n", romcode_100_data[i][17]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_CONTROL  0x%02x  ;# // set 16-byte write\n\n", romcode_100_data[i][18]);
        }

        
        //Poll NCTRL busy bit
        cnt=0;
        while(cnt<1000) //check NCTRL BUSY
        {
            rtl8291_getRegBit(devId, RTL8291_REG_INDACC_CONTROL_ADDR, RTL8291_REG_INDACC_CONTROL_INDACC_CMD_OFFSET, &status);
            if(status == 0)
            {
                break;
            }
            cnt++;
        }
        rtl8291_getReg(devId, RTL8291_REG_INDACC_CONTROL_ADDR, &status);
        if (dbg_en>0)
        {        
            printk(KERN_EMERG "REG_INDACC_CONTROL = 0x%x Busy Cnt= %d \n\n", status, cnt);
        }
        }
        else
        {
            rtl8291_setReg(devId, RTL8291_REG_INDACC_ADDR_LB_ADDR, romcode_100_data[i][0]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_ADDR_HB_ADDR, romcode_100_data[i][1]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_0_ADDR, romcode_100_data[i][2]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_1_ADDR, romcode_100_data[i][3]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_2_ADDR, romcode_100_data[i][4]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_3_ADDR, romcode_100_data[i][5]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_4_ADDR, romcode_100_data[i][6]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_5_ADDR, romcode_100_data[i][7]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_6_ADDR, romcode_100_data[i][8]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_7_ADDR, romcode_100_data[i][9]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_CONTROL_ADDR, romcode_100_data[i][18]);
            if (dbg_en>0)
            {
                printk(KERN_EMERG "SET_DREG $REG_INDACC_ADDR_LB  0x%02x \n", romcode_100_data[i][0]);
                printk(KERN_EMERG "SET_DREG $REG_INDACC_ADDR_HB  0x%02x \n", romcode_100_data[i][1]);
                printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_0  0x%02x \n", romcode_100_data[i][2]);
                printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_1  0x%02x \n", romcode_100_data[i][3]);
                printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_2  0x%02x \n", romcode_100_data[i][4]);
                printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_3  0x%02x \n", romcode_100_data[i][5]);
                printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_4  0x%02x \n", romcode_100_data[i][6]);
                printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_5  0x%02x \n", romcode_100_data[i][7]);
                printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_6  0x%02x \n", romcode_100_data[i][8]);
                printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_7  0x%02x \n", romcode_100_data[i][9]);
                printk(KERN_EMERG "SET_DREG $REG_INDACC_CONTROL  0x%02x  ;# // set 16-byte write\n\n", romcode_100_data[i][18]);
            }        
            //Poll NCTRL busy bit
            cnt=0;
            while(cnt<1000) //check NCTRL BUSY
            {
                rtl8291_getRegBit(devId, RTL8291_REG_INDACC_CONTROL_ADDR, RTL8291_REG_INDACC_CONTROL_INDACC_CMD_OFFSET, &status);
                if(status == 0)
                {
                    break;
                }
                cnt++;
            }
            rtl8291_getReg(devId, RTL8291_REG_INDACC_CONTROL_ADDR, &status);
            if (dbg_en>0)
            {
                printk(KERN_EMERG "REG_INDACC_CONTROL = 0x%x Busy Cnt= %d \n\n", status, cnt);
            }
        }
    }

    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P00_L_ADDR, 0x02);   
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P00_H_ADDR, 0x10);   
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P00_L_ADDR, 0x0);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P00_H_ADDR, 0xc);  
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P01_L_ADDR, 0x65);  
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P01_H_ADDR, 0x14);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P01_L_ADDR, 0x2);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P01_H_ADDR, 0xc);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P02_L_ADDR, 0x80);  
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P02_H_ADDR, 0x14);   
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P02_L_ADDR, 0x4); 
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P02_H_ADDR, 0xc);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P03_L_ADDR, 0x65);  
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P03_H_ADDR, 0x12);  
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P03_L_ADDR, 0x7);  
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P03_H_ADDR, 0xc);  
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P04_L_ADDR, 0xa5);  
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P04_H_ADDR, 0x12); 
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P04_L_ADDR, 0xbd);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P04_H_ADDR, 0xc);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P05_L_ADDR, 0xba);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P05_H_ADDR, 0x12);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P05_L_ADDR, 0xe1);  
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P05_H_ADDR, 0xc); 
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P06_L_ADDR, 0x0b);   
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P06_H_ADDR, 0x13); 
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P06_L_ADDR, 0xec);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P06_H_ADDR, 0xc); 
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P07_L_ADDR, 0x13); 
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P07_H_ADDR, 0x13);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P07_L_ADDR, 0x10);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P07_H_ADDR, 0xd);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P08_L_ADDR, 0x60); 
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P08_H_ADDR, 0x13);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P08_L_ADDR, 0xa0);  
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P08_H_ADDR, 0xd);
    if (dbg_en>0)
    {    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P00_L  0x%x \n", 0x02);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P00_H  0x%x \n", 0x10);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P00_L  0x%x \n", 0x0);
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P00_H  0x%x \n", 0xc);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P01_L  0x%x \n", 0x65);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P01_H  0x%x \n", 0x14);
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P01_L  0x%x \n", 0x2);
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P01_H  0x%x \n", 0xc);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P02_L  0x%x \n", 0x80);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P02_H  0x%x \n", 0x14);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P02_L  0x%x \n", 0x4);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P02_H  0x%x \n", 0xc);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P03_L  0x%x \n", 0x65);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P03_H  0x%x \n", 0x12);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P03_L  0x%x \n", 0x7);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P03_H  0x%x \n", 0xc);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P04_L  0x%x \n", 0xa5);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P04_H  0x%x \n", 0x12);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P04_L  0x%x \n", 0xbd);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P04_H  0x%x \n", 0xc);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P05_L  0x%x \n", 0xba);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P05_H  0x%x \n", 0x12);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P05_L  0x%x \n", 0xe1);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P05_H  0x%x \n", 0xc);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P06_L  0x%x \n", 0x0b);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P06_H  0x%x \n", 0x13);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P06_L  0x%x \n", 0xec);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P06_H  0x%x \n", 0xc);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P07_L  0x%x \n", 0x13);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P07_H  0x%x \n", 0x13);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P07_L  0x%x \n", 0x10);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P07_H  0x%x \n", 0xd);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P08_L  0x%x \n", 0x60);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P08_H  0x%x \n", 0x13);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P08_L  0x%x \n", 0xa0);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P08_H  0x%x \n", 0xd);
    }

    // set bit-3 is 0
    rtl8291_setReg(devId, RTL8291_REG_IIC_S_ADDR, 0x15);
    if (dbg_en>0)
    {     
        printk(KERN_EMERG "SET_DREG $REG_IIC_S   0x15\n"); 
    }
    // set bit-2 is 0
    rtl8291_setReg(devId, RTL8291_REG_CONTROL1_ADDR, 0);
    if (dbg_en>0)
    {     
        printk(KERN_EMERG "SET_DREG $REG_CONTROL1  0\n"); 
    }

    return;
}

void rtl8291_romcode_103_patch(uint32 devId, int dbg_en)
{
    uint32 i=0, data=0;
    uint32 cnt=0, status=0;

    printk(KERN_EMERG "rtl8291_romcode_103_patch...\n"); 
 
    // set bit-2 is 1
    rtl8291_setReg(devId, RTL8291_REG_CONTROL1_ADDR, 0x4);
    if (dbg_en>0)
    {    
        printk(KERN_EMERG "SET_DREG $REG_CONTROL1  0x4\n");
    }
    // set bit-3 is 1
    rtl8291_setReg(devId, RTL8291_REG_IIC_S_ADDR, 0x1C);
    if (dbg_en>0)
    {    
        printk(KERN_EMERG "SET_DREG $REG_IIC_S   0x1C\n");    
    }
    for (i=0;i<RTL8291_ROMCODE_103_PATCH_SIZE;i++)
    {
        if (i<(RTL8291_ROMCODE_103_PATCH_SIZE-1))
        {
        rtl8291_setReg(devId, RTL8291_REG_INDACC_ADDR_LB_ADDR, romcode_103_data[i][0]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_ADDR_HB_ADDR, romcode_103_data[i][1]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_0_ADDR, romcode_103_data[i][2]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_1_ADDR, romcode_103_data[i][3]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_2_ADDR, romcode_103_data[i][4]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_3_ADDR, romcode_103_data[i][5]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_4_ADDR, romcode_103_data[i][6]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_5_ADDR, romcode_103_data[i][7]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_6_ADDR, romcode_103_data[i][8]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_7_ADDR, romcode_103_data[i][9]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_8_ADDR, romcode_103_data[i][10]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_9_ADDR, romcode_103_data[i][11]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_10_ADDR, romcode_103_data[i][12]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_11_ADDR, romcode_103_data[i][13]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_12_ADDR, romcode_103_data[i][14]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_13_ADDR, romcode_103_data[i][15]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_14_ADDR, romcode_103_data[i][16]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_15_ADDR, romcode_103_data[i][17]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_CONTROL_ADDR, romcode_103_data[i][18]);
        if (dbg_en>0)
        {
            printk(KERN_EMERG "SET_DREG $REG_INDACC_ADDR_LB  0x%02x \n", romcode_103_data[i][0]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_ADDR_HB  0x%02x \n", romcode_103_data[i][1]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_0  0x%02x \n", romcode_103_data[i][2]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_1  0x%02x \n", romcode_103_data[i][3]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_2  0x%02x \n", romcode_103_data[i][4]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_3  0x%02x \n", romcode_103_data[i][5]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_4  0x%02x \n", romcode_103_data[i][6]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_5  0x%02x \n", romcode_103_data[i][7]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_6  0x%02x \n", romcode_103_data[i][8]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_7  0x%02x \n", romcode_103_data[i][9]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_8  0x%02x \n", romcode_103_data[i][10]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_9  0x%02x \n", romcode_103_data[i][11]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_10  0x%02x \n", romcode_103_data[i][12]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_11  0x%02x \n", romcode_103_data[i][13]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_12  0x%02x \n", romcode_103_data[i][14]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_13  0x%02x \n", romcode_103_data[i][15]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_14  0x%02x \n", romcode_103_data[i][16]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_15  0x%02x \n", romcode_103_data[i][17]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_CONTROL  0x%02x  ;# // set 16-byte write\n\n", romcode_103_data[i][18]);
        }
        //Poll NCTRL busy bit
        cnt=0;
        while(cnt<1000) //check NCTRL BUSY
        {
            rtl8291_getRegBit(devId, RTL8291_REG_INDACC_CONTROL_ADDR, RTL8291_REG_INDACC_CONTROL_INDACC_CMD_OFFSET, &status);
            if(status == 0)
            {
                break;
            }
            cnt++;
        }
        rtl8291_getReg(devId, RTL8291_REG_INDACC_CONTROL_ADDR, &status);
        if (dbg_en>0)
        {
            printk(KERN_EMERG "REG_INDACC_CONTROL = 0x%x Busy Cnt= %d \n\n", status, cnt);
        }
        }
        else
        {
            rtl8291_setReg(devId, RTL8291_REG_INDACC_ADDR_LB_ADDR, romcode_103_data[i][0]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_ADDR_HB_ADDR, romcode_103_data[i][1]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_0_ADDR, romcode_103_data[i][2]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_1_ADDR, romcode_103_data[i][3]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_CONTROL_ADDR, romcode_103_data[i][18]);
            if (dbg_en>0)
            {
                printk(KERN_EMERG "SET_DREG $REG_INDACC_ADDR_LB  0x%02x \n", romcode_103_data[i][0]);
                printk(KERN_EMERG "SET_DREG $REG_INDACC_ADDR_HB  0x%02x \n", romcode_103_data[i][1]);
                printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_0  0x%02x \n", romcode_103_data[i][2]);
                printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_1  0x%02x \n", romcode_103_data[i][3]);
                printk(KERN_EMERG "SET_DREG $REG_INDACC_CONTROL  0x%02x  ;# // set 16-byte write\n\n", romcode_103_data[i][18]);
            }

            
            //Poll NCTRL busy bit
            cnt=0;
            while(cnt<1000) //check NCTRL BUSY
            {
                rtl8291_getRegBit(devId, RTL8291_REG_INDACC_CONTROL_ADDR, RTL8291_REG_INDACC_CONTROL_INDACC_CMD_OFFSET, &status);
                if(status == 0)
                {
                    break;
                }
                cnt++;
            }            
            rtl8291_getReg(devId, RTL8291_REG_INDACC_CONTROL_ADDR, &status);
            if (dbg_en>0)
            {            
                printk(KERN_EMERG "REG_INDACC_CONTROL = 0x%x Busy Cnt= %d \n\n", status, cnt);
            }
        }
    }

    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P00_L_ADDR, 0x02);  
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P00_H_ADDR, 0x10);  
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P00_L_ADDR, 0x0);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P00_H_ADDR, 0xc);   
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P01_L_ADDR, 0x65);   
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P01_H_ADDR, 0x14);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P01_L_ADDR, 0x2);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P01_H_ADDR, 0xc);  
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P02_L_ADDR, 0x80);  
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P02_H_ADDR, 0x14);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P02_L_ADDR, 0x4);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P02_H_ADDR, 0xc);  
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P03_L_ADDR, 0x65);   
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P03_H_ADDR, 0x12);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P03_L_ADDR, 0x7); 
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P03_H_ADDR, 0xc);  
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P04_L_ADDR, 0xa5);  
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P04_H_ADDR, 0x12);   
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P04_L_ADDR, 0xbd); 
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P04_H_ADDR, 0xc);   
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P05_L_ADDR, 0xba);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P05_H_ADDR, 0x12); 
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P05_L_ADDR, 0xe1);  
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P05_H_ADDR, 0xc); 
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P06_L_ADDR, 0x0b);  
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P06_H_ADDR, 0x13); 
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P06_L_ADDR, 0xec);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P06_H_ADDR, 0xc);   
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P07_L_ADDR, 0x13);  
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P07_H_ADDR, 0x13);   
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P07_L_ADDR, 0x10);   
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P07_H_ADDR, 0xd);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P08_L_ADDR, 0x60); 
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P08_H_ADDR, 0x13);   
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P08_L_ADDR, 0xa0);   
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P08_H_ADDR, 0xd);
    if (dbg_en>0)
    {    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P00_L  0x%x \n", 0x02);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P00_H  0x%x \n", 0x10);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P00_L  0x%x \n", 0x0);
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P00_H  0x%x \n", 0xc);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P01_L  0x%x \n", 0x65);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P01_H  0x%x \n", 0x14);
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P01_L  0x%x \n", 0x2);
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P01_H  0x%x \n", 0xc);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P02_L  0x%x \n", 0x80);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P02_H  0x%x \n", 0x14);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P02_L  0x%x \n", 0x4);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P02_H  0x%x \n", 0xc);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P03_L  0x%x \n", 0x65);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P03_H  0x%x \n", 0x12);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P03_L  0x%x \n", 0x7);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P03_H  0x%x \n", 0xc);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P04_L  0x%x \n", 0xa5);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P04_H  0x%x \n", 0x12);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P04_L  0x%x \n", 0xbd);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P04_H  0x%x \n", 0xc);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P05_L  0x%x \n", 0xba);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P05_H  0x%x \n", 0x12);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P05_L  0x%x \n", 0xe1);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P05_H  0x%x \n", 0xc);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P06_L  0x%x \n", 0x0b);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P06_H  0x%x \n", 0x13);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P06_L  0x%x \n", 0xec);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P06_H  0x%x \n", 0xc);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P07_L  0x%x \n", 0x13);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P07_H  0x%x \n", 0x13);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P07_L  0x%x \n", 0x10);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P07_H  0x%x \n", 0xd);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P08_L  0x%x \n", 0x60);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P08_H  0x%x \n", 0x13);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P08_L  0x%x \n", 0xa0);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P08_H  0x%x \n", 0xd);
    }
    // set bit-3 is 0
    rtl8291_setReg(devId, RTL8291_REG_IIC_S_ADDR, 0x15);
    if (dbg_en>0)
    {    
        printk(KERN_EMERG "SET_DREG $REG_IIC_S   0x15\n");
    }
    // set bit-2 is 0
    rtl8291_setReg(devId, RTL8291_REG_CONTROL1_ADDR, 0);
    if (dbg_en>0)
    {    
        printk(KERN_EMERG "SET_DREG $REG_CONTROL1  0\n");
    }

    return;
}

void rtl8291_romcode_105_patch(uint32 devId, int dbg_en)
{
    uint32 i=0, data=0;
    uint32 cnt=0, status=0;

    printk(KERN_EMERG "rtl8291_romcode_105_patch...\n"); 
 
    // set bit-2 is 1
    rtl8291_setReg(devId, RTL8291_REG_CONTROL1_ADDR, 0x4);
    if (dbg_en>0)
    {    
        printk(KERN_EMERG "SET_DREG $REG_CONTROL1  0x4\n");
    }
    // set bit-3 is 1
    rtl8291_setReg(devId, RTL8291_REG_IIC_S_ADDR, 0x1C);
    if (dbg_en>0)
    {    
        printk(KERN_EMERG "SET_DREG $REG_IIC_S   0x1C\n");    
    }
    for (i=0;i<RTL8291_ROMCODE_105_PATCH_SIZE;i++)
    {
        if (i<(RTL8291_ROMCODE_105_PATCH_SIZE-1))
        {
        rtl8291_setReg(devId, RTL8291_REG_INDACC_ADDR_LB_ADDR, romcode_105_data[i][0]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_ADDR_HB_ADDR, romcode_105_data[i][1]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_0_ADDR, romcode_105_data[i][2]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_1_ADDR, romcode_105_data[i][3]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_2_ADDR, romcode_105_data[i][4]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_3_ADDR, romcode_105_data[i][5]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_4_ADDR, romcode_105_data[i][6]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_5_ADDR, romcode_105_data[i][7]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_6_ADDR, romcode_105_data[i][8]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_7_ADDR, romcode_105_data[i][9]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_8_ADDR, romcode_105_data[i][10]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_9_ADDR, romcode_105_data[i][11]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_10_ADDR, romcode_105_data[i][12]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_11_ADDR, romcode_105_data[i][13]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_12_ADDR, romcode_105_data[i][14]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_13_ADDR, romcode_105_data[i][15]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_14_ADDR, romcode_105_data[i][16]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_15_ADDR, romcode_105_data[i][17]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_CONTROL_ADDR, romcode_105_data[i][18]);
        if (dbg_en>0)
        {
            printk(KERN_EMERG "SET_DREG $REG_INDACC_ADDR_LB  0x%02x \n", romcode_105_data[i][0]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_ADDR_HB  0x%02x \n", romcode_105_data[i][1]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_0  0x%02x \n", romcode_105_data[i][2]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_1  0x%02x \n", romcode_105_data[i][3]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_2  0x%02x \n", romcode_105_data[i][4]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_3  0x%02x \n", romcode_105_data[i][5]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_4  0x%02x \n", romcode_105_data[i][6]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_5  0x%02x \n", romcode_105_data[i][7]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_6  0x%02x \n", romcode_105_data[i][8]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_7  0x%02x \n", romcode_105_data[i][9]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_8  0x%02x \n", romcode_105_data[i][10]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_9  0x%02x \n", romcode_105_data[i][11]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_10  0x%02x \n", romcode_105_data[i][12]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_11  0x%02x \n", romcode_105_data[i][13]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_12  0x%02x \n", romcode_105_data[i][14]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_13  0x%02x \n", romcode_105_data[i][15]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_14  0x%02x \n", romcode_105_data[i][16]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_15  0x%02x \n", romcode_105_data[i][17]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_CONTROL  0x%02x  ;# // set 16-byte write\n\n", romcode_105_data[i][18]);
        }
        //Poll NCTRL busy bit
        cnt=0;
        while(cnt<1000) //check NCTRL BUSY
        {
            rtl8291_getRegBit(devId, RTL8291_REG_INDACC_CONTROL_ADDR, RTL8291_REG_INDACC_CONTROL_INDACC_CMD_OFFSET, &status);
            if(status == 0)
            {
                break;
            }
            cnt++;
        }
        rtl8291_getReg(devId, RTL8291_REG_INDACC_CONTROL_ADDR, &status);
        if (dbg_en>0)
        {
            printk(KERN_EMERG "REG_INDACC_CONTROL = 0x%x Busy Cnt= %d \n\n", status, cnt);
        }
        }
        else
        {
            rtl8291_setReg(devId, RTL8291_REG_INDACC_ADDR_LB_ADDR, romcode_105_data[i][0]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_ADDR_HB_ADDR, romcode_105_data[i][1]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_0_ADDR, romcode_105_data[i][2]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_1_ADDR, romcode_105_data[i][3]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_2_ADDR, romcode_105_data[i][4]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_3_ADDR, romcode_105_data[i][5]);            
            rtl8291_setReg(devId, RTL8291_REG_INDACC_CONTROL_ADDR, romcode_105_data[i][18]);
            if (dbg_en>0)
            {
                printk(KERN_EMERG "SET_DREG $REG_INDACC_ADDR_LB  0x%02x \n", romcode_105_data[i][0]);
                printk(KERN_EMERG "SET_DREG $REG_INDACC_ADDR_HB  0x%02x \n", romcode_105_data[i][1]);
                printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_0  0x%02x \n", romcode_105_data[i][2]);
                printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_1  0x%02x \n", romcode_105_data[i][3]);
                printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_2  0x%02x \n", romcode_105_data[i][4]);
                printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_3  0x%02x \n", romcode_105_data[i][5]);
                printk(KERN_EMERG "SET_DREG $REG_INDACC_CONTROL  0x%02x  ;# // set 16-byte write\n\n", romcode_105_data[i][18]);
            }         
            //Poll NCTRL busy bit
            cnt=0;
            while(cnt<1000) //check NCTRL BUSY
            {
                rtl8291_getRegBit(devId, RTL8291_REG_INDACC_CONTROL_ADDR, RTL8291_REG_INDACC_CONTROL_INDACC_CMD_OFFSET, &status);
                if(status == 0)
                {
                    break;
                }
                cnt++;
            }            
            rtl8291_getReg(devId, RTL8291_REG_INDACC_CONTROL_ADDR, &status);
            if (dbg_en>0)
            {            
                printk(KERN_EMERG "REG_INDACC_CONTROL = 0x%x Busy Cnt= %d \n\n", status, cnt);
            }
        }
    }

    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P00_L_ADDR, 0x02);  
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P00_H_ADDR, 0x10);  
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P00_L_ADDR, 0x0);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P00_H_ADDR, 0xc);   
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P01_L_ADDR, 0x65);   
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P01_H_ADDR, 0x14);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P01_L_ADDR, 0x2);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P01_H_ADDR, 0xc);  
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P02_L_ADDR, 0x80);  
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P02_H_ADDR, 0x14);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P02_L_ADDR, 0x4);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P02_H_ADDR, 0xc);  
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P03_L_ADDR, 0x65);   
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P03_H_ADDR, 0x12);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P03_L_ADDR, 0x7); 
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P03_H_ADDR, 0xc);  
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P04_L_ADDR, 0xa5);  
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P04_H_ADDR, 0x12);   
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P04_L_ADDR, 0xbd); 
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P04_H_ADDR, 0xc);   
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P05_L_ADDR, 0xba);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P05_H_ADDR, 0x12); 
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P05_L_ADDR, 0xe1);  
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P05_H_ADDR, 0xc); 
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P06_L_ADDR, 0x0b);  
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P06_H_ADDR, 0x13); 
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P06_L_ADDR, 0xec);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P06_H_ADDR, 0xc);   
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P07_L_ADDR, 0x13);  
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P07_H_ADDR, 0x13);   
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P07_L_ADDR, 0x10);   
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P07_H_ADDR, 0xd);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P08_L_ADDR, 0x60); 
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P08_H_ADDR, 0x13);   
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P08_L_ADDR, 0xa0);   
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P08_H_ADDR, 0xd);
    if (dbg_en>0)
    {    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P00_L  0x%x \n", 0x02);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P00_H  0x%x \n", 0x10);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P00_L  0x%x \n", 0x0);
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P00_H  0x%x \n", 0xc);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P01_L  0x%x \n", 0x65);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P01_H  0x%x \n", 0x14);
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P01_L  0x%x \n", 0x2);
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P01_H  0x%x \n", 0xc);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P02_L  0x%x \n", 0x80);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P02_H  0x%x \n", 0x14);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P02_L  0x%x \n", 0x4);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P02_H  0x%x \n", 0xc);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P03_L  0x%x \n", 0x65);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P03_H  0x%x \n", 0x12);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P03_L  0x%x \n", 0x7);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P03_H  0x%x \n", 0xc);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P04_L  0x%x \n", 0xa5);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P04_H  0x%x \n", 0x12);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P04_L  0x%x \n", 0xbd);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P04_H  0x%x \n", 0xc);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P05_L  0x%x \n", 0xba);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P05_H  0x%x \n", 0x12);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P05_L  0x%x \n", 0xe1);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P05_H  0x%x \n", 0xc);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P06_L  0x%x \n", 0x0b);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P06_H  0x%x \n", 0x13);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P06_L  0x%x \n", 0xec);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P06_H  0x%x \n", 0xc);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P07_L  0x%x \n", 0x13);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P07_H  0x%x \n", 0x13);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P07_L  0x%x \n", 0x10);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P07_H  0x%x \n", 0xd);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P08_L  0x%x \n", 0x60);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P08_H  0x%x \n", 0x13);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P08_L  0x%x \n", 0xa0);    
        printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P08_H  0x%x \n", 0xd);
    }
    // set bit-3 is 0
    rtl8291_setReg(devId, RTL8291_REG_IIC_S_ADDR, 0x15);
    if (dbg_en>0)
    {    
        printk(KERN_EMERG "SET_DREG $REG_IIC_S   0x15\n");
    }
    // set bit-2 is 0
    rtl8291_setReg(devId, RTL8291_REG_CONTROL1_ADDR, 0);
    if (dbg_en>0)
    {    
        printk(KERN_EMERG "SET_DREG $REG_CONTROL1  0\n");
    }

    return;
}


void rtl8291_user_parameter_init(uint32 devId, uint8 *flash_data, rtl8291_param_t *ldd_param)
{
    uint32 addr, temp32;
    uint32 rssi_k=0, mpd_k=0;
    uint32 rssi_r1=0, mpd_r500=0;
    uint32 rssi_ical=0, rssi_ir1=0;
    uint32 data0=0, data1=0;

    //printk(KERN_EMERG "Update RTL8290C user parameters ...\n");

    // setup temperature interrupt scale +
    {
    uint8 temp_scale;
    
    temp_scale = flash_data[RTL8291_TEMP_INTR_SCALE_ADDR];

    if (0<temp_scale && 8>temp_scale)
    {
       rtl8291_setReg(devId, RTL8291_REG_TEMP_INT_OFFSET_ADDR, temp_scale);
    }
    
    memcpy(&ldd_param->temp_offset,  &flash_data[RTL8291_TEMP_OFFSET_ADDR], sizeof(uint8));
    //printk(KERN_EMERG "toffset = %d \n", (int)ldd_param->temp_offset);
    
    }

    ldd_param->rx_lsb = 4000000;
    rtl8291_getReg(devId, RTL8291_RSSI_K1_MSB_ADDR, &data0); 
    rtl8291_getRegBits(devId, RTL8291_RSSI_K1_MPD_K_LSB_ADDR, RTL8291_RSSI_K1_MPD_K_LSB_RSSI_K1_LSB_MASK, &data1);
    rssi_k = (data0&0xFF)<<4|(data1&0xF);
    if (rssi_k!=0)
    {
        ldd_param->rx_code_200u = rssi_k;
    }
    else
    {
        ldd_param->rx_code_200u = RTL8291_RSSI_K_DEFAULT;
        rtl8291_setReg(devId, RTL8291_RSSI_K1_MSB_ADDR, (ldd_param->rx_code_200u&0xFF0)>>4); 
        rtl8291_setRegBits(devId, RTL8291_RSSI_K1_MPD_K_LSB_ADDR, RTL8291_RSSI_K1_MPD_K_LSB_RSSI_K1_LSB_MASK, (ldd_param->rx_code_200u&0xF));        
    }

    //rtl8291_getReg(devId, RTL8291_RSSI_R1_MSB_ADDR, &data0); 
    //rtl8291_getRegBits(devId, RTL8291_RSSI_R1_LSB_ADDR, RTL8291_RSSI_R1_LSB_RSSI_R1_LSB_MASK, &data1);
    //rssi_r1 = (data0&0xFF)<<4|(data1&0xF);
    rtl8291_rssiR6400_get(devId, &rssi_r1);    
    //if (rssi_r1!=0)
    //{
        ldd_param->rx_r6400 = rssi_r1;
    //}
    //else
    //{
    //    ldd_param->rx_r6400 = RTL8291_RSSI_R1_DEFAULT;
    //    rtl8291_setReg(devId, RTL8291_RSSI_R1_MSB_ADDR, (ldd_param->rx_r6400&0xFF0)>>4); 
    //    rtl8291_setRegBits(devId, RTL8291_RSSI_R1_LSB_ADDR, RTL8291_RSSI_R1_LSB_RSSI_R1_LSB_MASK, (ldd_param->rx_r6400&0xF));
    //}

    //rssi_ical = flash_data[RTL8291_RSSI_ICAL_OFFSET_ADDR];
    rtl8291_getReg(devId, RTL8291_RSSI_ICAL1_ADDR, &rssi_ical);    
    if (rssi_ical==0)
    {
        rssi_ical = RTL8291_RSSI_ICAL_DEFAULT;
        rtl8291_setReg(devId, RTL8291_RSSI_ICAL1_ADDR, rssi_ical);		
    }
    
    //rssi_ir1 = flash_data[RTL8291_RSSI_IR1_OFFSET_ADDR];
    rtl8291_getReg(devId, RTL8291_RSSI_IR1_ADDR, &rssi_ir1);     
    if (rssi_ir1==0)
    {
        rssi_ir1 = RTL8291_RSSI_IR1_DEFAULT;
        rtl8291_setReg(devId, RTL8291_RSSI_IR1_ADDR, rssi_ir1);		
    }

    printk(KERN_EMERG "RX: LSB = %d, CODE_200u(RSSI_K) = %d, RSSI_R1 = %d, Ical = %d, IR1 = %d\n", ldd_param->rx_lsb, ldd_param->rx_code_200u, ldd_param->rx_r6400, rssi_ical, rssi_ir1);

    addr = RTL8291_RX_A_ADDR;
    _8291_data_trans(&flash_data[addr], 2, &temp32);
    ldd_param->rx_a = (int16)temp32;
    rtl8291_setReg(devId, RTL8291_CFG_RX_POWER_SLOPE2_H_ADDR, (temp32&0xFF00)>>8); 
    rtl8291_setReg(devId, RTL8291_CFG_RX_POWER_SLOPE2_L_ADDR, temp32&0xFF);     
    addr = RTL8291_RX_B_ADDR;
    _8291_data_trans(&flash_data[addr], 2, &temp32);
    ldd_param->rx_b = (int16)temp32;
    rtl8291_setReg(devId, RTL8291_CFG_RX_POWER_SLOPE1_H_ADDR, (temp32&0xFF00)>>8); 
    rtl8291_setReg(devId, RTL8291_CFG_RX_POWER_SLOPE1_L_ADDR, temp32&0xFF); 
    addr = RTL8291_RX_C_ADDR;
    _8291_data_trans(&flash_data[addr], 2, &temp32);
    ldd_param->rx_c = (int16)temp32;
    rtl8291_setReg(devId, RTL8291_CFG_RX_POWER_OFFSET_H_ADDR, (temp32&0xFF00)>>8); 
    rtl8291_setReg(devId, RTL8291_CFG_RX_POWER_OFFSET_L_ADDR, temp32&0xFF);     
    addr = RTL8291_SHIFT_RX_A_ADDR;
    _8291_data_trans(&flash_data[addr], 1, &temp32);
    ldd_param->rx_shft_a = (int32)temp32;
    rtl8291_setReg(devId, RTL8291_CFG_RX_POWER_SHIFT0_ADDR, temp32);     
    addr = RTL8291_SHIFT_RX_B_ADDR;
    _8291_data_trans(&flash_data[addr], 1, &temp32);
    ldd_param->rx_shft_b = (int32)temp32;
    rtl8291_setRegBits(devId, RTL8291_CFG_RX_POWER_SHIFT1_ADDR, RTL8291_CFG_RX_POWER_SHIFT1_CFG_RX_POWER_SLOPE1_SHIFT_MASK, temp32);
    addr = RTL8291_SHIFT_RX_C_ADDR;
    _8291_data_trans(&flash_data[addr], 1, &temp32);
    ldd_param->rx_shft_c = (int32)temp32;
    rtl8291_setRegBits(devId, RTL8291_CFG_RX_POWER_SHIFT1_ADDR, RTL8291_CFG_RX_POWER_SHIFT1_CFG_RX_POWER_OFFSET_SHIFT_MASK, temp32);

    printk(KERN_EMERG "RX A = %d, RX B = %d, RX C = %d\n", ldd_param->rx_a, ldd_param->rx_b, ldd_param->rx_c);
    printk(KERN_EMERG "RX SHFT_A = %d, RX SHFT_B = %d, RX SHFT_C = %d\n", ldd_param->rx_shft_a, ldd_param->rx_shft_b, ldd_param->rx_shft_c);

    ldd_param->tx_lsb = 4000000;
    rtl8291_getReg(devId, RTL8291_MPD_K_MSB_ADDR, &data0); 
    rtl8291_getRegBits(devId, RTL8291_RSSI_K1_MPD_K_LSB_ADDR, RTL8291_RSSI_K1_MPD_K_LSB_MPD_K_LSB_MASK, &data1);
    mpd_k = (data0&0xFF)<<4|(data1&0xF);
    if (mpd_k!=0)
    {
        ldd_param->tx_code_250u = mpd_k;
    }
    else
    {
        ldd_param->tx_code_250u = RTL8291_MPD_K_DEFAULT;
        rtl8291_setReg(devId, RTL8291_MPD_K_MSB_ADDR, (ldd_param->tx_code_250u&0xFF0)>>4); 
        rtl8291_setRegBits(devId, RTL8291_RSSI_K1_MPD_K_LSB_ADDR, RTL8291_RSSI_K1_MPD_K_LSB_MPD_K_LSB_MASK, (ldd_param->tx_code_250u&0xF));
    }
    rtl8291_getReg(devId, RTL8291_MPD_R0_LB_ADDR, &data1); 
    rtl8291_getRegBits(devId, RTL8291_MPD_R_HB_ADDR, RTL8291_MPD_R_HB_MPD_R0_HB_MASK, &data0);
    mpd_r500 = (data0&0xF)<<8|(data1&0xFF);
    if (mpd_r500!=0)
    {
        ldd_param->tx_r_mpd = mpd_r500;
    }
    else
    {
        //ldd_param->tx_r_mpd = RTL8291_MPD_R500_DEFAULT;
        ldd_param->tx_r_mpd = RTL8291_MPD_CODE_STANDARD;
        rtl8291_setReg(devId, RTL8291_MPD_R0_LB_ADDR, (ldd_param->tx_r_mpd&0xFF)); 
        rtl8291_setRegBits(devId, RTL8291_MPD_R_HB_ADDR, RTL8291_MPD_R_HB_MPD_R0_HB_MASK, (ldd_param->tx_r_mpd&0xF00)>>8);
    }
    printk(KERN_EMERG "TX: LSB = %d, CODE_250u(MPD_K) = %d, R_MPD = %d\n", ldd_param->tx_lsb, ldd_param->tx_code_250u, ldd_param->tx_r_mpd);

    addr = RTL8291_TX_A_ADDR;
    _8291_data_trans(&flash_data[addr], 2, &temp32);
    ldd_param->tx_a = (int16)temp32;
    rtl8291_setReg(devId, RTL8291_CFG_TX_POWER_SLOPE_H_ADDR, (temp32&0xFF00)>>8); 
    rtl8291_setReg(devId, RTL8291_CFG_TX_POWER_SLOPE_L_ADDR, temp32&0xFF);      
    addr = RTL8291_TX_B_ADDR;
    _8291_data_trans(&flash_data[addr], 2, &temp32);
    ldd_param->tx_b = (int16)temp32;
    rtl8291_setReg(devId, RTL8291_CFG_TX_POWER_OFFSET_H_ADDR, (temp32&0xFF00)>>8); 
    rtl8291_setReg(devId, RTL8291_CFG_TX_POWER_OFFSET_L_ADDR, temp32&0xFF);      
    addr = RTL8291_TX_C_ADDR;
    _8291_data_trans(&flash_data[addr], 2, &temp32);
    ldd_param->tx_c = (int16)temp32;
    addr = RTL8291_SHIFT_TX_A_ADDR;
    _8291_data_trans(&flash_data[addr], 1, &temp32);
    ldd_param->tx_shft_a = (int32)temp32;
    rtl8291_setRegBits(devId, RTL8291_CFG_TX_POWER_SHIFT_ADDR, RTL8291_CFG_TX_POWER_SHIFT_CFG_TX_POWER_SLOPE_SHIFT_MASK, temp32);    
    addr = RTL8291_SHIFT_TX_B_ADDR;
    _8291_data_trans(&flash_data[addr], 1, &temp32);
    ldd_param->tx_shft_b = (int32)temp32;
    rtl8291_setRegBits(devId, RTL8291_CFG_TX_POWER_SHIFT_ADDR, RTL8291_CFG_TX_POWER_SHIFT_CFG_TX_POWER_OFFSET_SHIFT_MASK, temp32);        
    addr = RTL8291_SHIFT_TX_C_ADDR;
    _8291_data_trans(&flash_data[addr], 1, &temp32);
    ldd_param->tx_shft_c = (int32)temp32;    
   
    printk(KERN_EMERG "TX A = %d, TX B = %d, TX C = %d\n", ldd_param->tx_a, ldd_param->tx_b, ldd_param->tx_c);
    printk(KERN_EMERG "TX SHFT_A = %d, TX SHFT_B = %d, TX SHFT_C = %d\n", ldd_param->tx_shft_a, ldd_param->tx_shft_b, ldd_param->tx_shft_c);

}

//Cal for APC & ERC
//    puts "CalMode=0, cal for APC only." 
//    puts "This mode is *NOT AVAILABLE* in RL6907!"
int32  rtl8291_apcCalibration_set(uint32 devId, uint32 erc_en)
{
    uint32 ro_data_p1s10, ro_data_p1s7, ro_data_p1s8;
    
    /*** W5C[6] ERC_CAL_SEL: 0=rectifier, 1=integator. MUST BE 0 in RL6907 ****/
    rtl8291_setRegBit(devId, RTL8291_WB_REG87_ADDR, RTL8291_WB_REG87_REG_APCDIG_ERC_CAL_SEL_OFFSET, 0);  
    rtl8291_apcErcChopperEn_set(devId, erc_en); 
    rtl8291_setRegBit(devId, RTL8291_WB_REG7C_ADDR, RTL8291_WB_REG7C_REG_APCDIG_ERC_CAL_DAC_EN_OFFSET, 1);  

    rtl8291_calTrigger_set(devId);

    rtl8291_roData_get(devId, 1, 10, &ro_data_p1s10);
    rtl8291_roData_get(devId, 1, 7, &ro_data_p1s7);
    rtl8291_roData_get(devId, 1, 8, &ro_data_p1s8);
    //printk(KERN_EMERG "APC_CAL: APC_CAL_SIGN = %d  APC_CAL_BITS = 0x%x\n", ((ro_data_p1s10&0x04)>>2), ro_data_p1s7);
    //printk(KERN_EMERG "ERC_CAL: ERC_CAL_SIGN = %d  ERC_CAL_BITS = 0x%x\n", (ro_data_p1s10&0x01), (ro_data_p1s8&0x7F));
    
    return SUCCESS;    
}

int32  rtl8291_powerOnStatus_get(uint32 devId, uint32 *result)
{
   uint32 status;
   uint32 cnt=0;

   *result = 0xFFFF;

    while(1)
    {
        rtl8291_getRegBits(devId, RTL8291_REG_STATUS_2_ADDR,0xFF,&status);
        if(((RTL8291_REG_STATUS_2_TEMP_VALID_MASK&status)!=0) && ((RTL8291_REG_STATUS_2_LVCMP_LV_L_MASK&status)!=0) &&
        ((RTL8291_REG_STATUS_2_DEBUG_MODE_STATUS_MASK&status)==0))
        {
            break;
        }
        cnt++;
        if (cnt>=2000)
        {
             printk(KERN_EMERG "RTL8291_REG_STATUS_2_ADDR check over 2000 times!!!!!\n");
             *result = 0;
             printk(KERN_EMERG " rtl8291_powerOnStatus_get: status 2 =0x %x!!!!!\n", status);
             if ((RTL8291_REG_STATUS_2_TEMP_VALID_MASK&status)!=0)
             {
                 printk(KERN_EMERG " RTL8291_REG_STATUS_2_TEMP_VALID = 1 !!!!!\n");
             }
             if ((RTL8291_REG_STATUS_2_LVCMP_LV_L_MASK&status)!=0)
             {
                 printk(KERN_EMERG " RTL8291_REG_STATUS_2_LVCMP_LV_L = 1 !!!!!\n");
             }             
             if ((RTL8291_REG_STATUS_2_DEBUG_MODE_STATUS_MASK&status)!=0)
             {
                 printk(KERN_EMERG " RTL8291_REG_STATUS_2_DEBUG_MODE_STATUS = 1 !!!!!\n");
             }             
             return -1;
        }
    }

 #if 0   
    rtl8291_getRegBit(devId, RTL8291_RB_REG07_ADDR, RTL8290C_RB_REG07_PORB33V_L_OFFSET, &status);
    //printk(KERN_EMERG "RTL8290C_RB_REG07_PORB33V_L = 0x%x \n", status);
    rtl8291_getRegBit(devId, RTL8290C_RB_REG07_ADDR, RTL8290C_RB_REG07_PORSTB_L_OFFSET, &status);
    //printk(KERN_EMERG "RTL8290C_RB_REG07_PORSTB_L = 0x%x \n", status);
    rtl8291_getRegBit(devId, RTL8290C_RB_REG07_ADDR, RTL8290C_RB_REG07_POR12V_VTH_L_OFFSET, &status);
    //printk(KERN_EMERG "RTL8290C_RB_REG07_POR12V_VTH_L = 0x%x \n", status);
    rtl8291_getRegBits(devId, RTL8290C_RB_REG07_ADDR, RTL8290C_RB_REG07_POR33V_VTH_L_1_0_MASK, &status);
    //printk(KERN_EMERG "RTL8290C_RB_REG07_POR33V_VTH_L = 0x%x \n", status);
    rtl8291_getRegBit(devId, RTL8290C_RB_REG07_ADDR, RTL8290C_RB_REG07_LVCMP_LV_L_OFFSET, &status);
    //printk(KERN_EMERG "RTL8290C_RB_REG07_LVCMP_LV_L = 0x%x \n", status);
    rtl8291_getRegBit(devId, RTL8290C_RB_REG07_ADDR, RTL8290C_RB_REG07_LVCMP_RXLA_L_OFFSET, &status);
    //printk(KERN_EMERG "RTL8290C_RB_REG07_LVCMP_RXLA_L = 0x%x \n", status);
    rtl8291_getRegBit(devId, RTL8290C_RB_REG07_ADDR, RTL8290C_RB_REG07_LVCMP_TX_L_OFFSET, &status);
    //printk(KERN_EMERG "RTL8290C_RB_REG07_LVCMP_TX_L_OFFSET = 0x%x \n", status);
#endif

    *result = 0;
    return SUCCESS;
}

int32  rtl8291_ready_check(uint32 devId)
{
    uint32 tmp, cnt;
 
    cnt = 0;
    tmp = 0;
    //---- check ready_Bit status---- (bit 2 == 1)
    while(1)
    {
        rtl8291_getRegBit(devId, RTL8291_REG_STATUS_1_ADDR, RTL8291_REG_STATUS_1_READY_STATUS_OFFSET, &tmp);
        //if(tmp  == 0x1)
        //      break;

        if((tmp  == 1)||cnt>=2000)
        {
            if (cnt>=2000)
            {
                printk("rtl8291_ready_check(0x384): over 2000 times!!!!!");
            }
            break;
        }
        cnt++;
        mdelay(1);
    }
    return SUCCESS;
}

void  rtl8291_tx_info(uint32 devId)
{
    uint32 bias, mod;

    mdelay(20);  /* debug.... */
    rtl8291_tx_bias_get(devId, &bias);
    rtl8291_tx_mod_get(devId, &mod);    
    printk(KERN_EMERG "Bias = %d, Mod = %d\n", bias, mod);
    return;
}

int32  rtl8291_laserLut_init(uint32 devId, uint8 *flash_data)
{
    int32 i;
    uint32 addr_base, addr, data;
    uint8 *ptr;
    
    if (NULL == flash_data)
    {
        return FAILED;
    }

    ptr = flash_data;
    addr_base  = RTL8291_TX_LUT_REG00_ADDR;

    for (i=0;i<RTL8291_TXLUT_SZ;i++)
    {
        addr = addr_base+i;
        data = *ptr;
        rtl8291_setReg(devId, addr, data);
        //printk(KERN_EMERG " rtl8291_laserLut_init: addr = 0x%x, data = 0x%x\n", addr, data);
        ptr++;
    }

    return SUCCESS;
}


void rtl8291_mcu_init(uint32 devId) 
{
    uint32 chip_ready=0;
    //puts " \[Check Chip Init Ready\] "
    //set CHIP_INIT_RDY [lindex [GET_DREG 0x686] end];
    //set st_chip_init_rdy [expr ($CHIP_INIT_RDY & 0x01)]  
    //puts "st_chip_init_rdy=$st_chip_init_rdy"
    rtl8291_getRegBit(devId, RTL8291_CHIP_INIT_RDY_ADDR, RTL8291_CHIP_INIT_RDY_CHIP_INIT_RDY_OFFSET, &chip_ready);
    printk(KERN_EMERG "Check Chip Init Ready: chip_ready = 0x%x\n", chip_ready); 
    
    //puts "set CFG_SYSTEM_MODE.CFG_MCUMD_INIT_DONE flag"
    //set CFG_SYSTEM_MODE [lindex [GET_DREG 0x699] end];
    //set regVal [expr ($CFG_SYSTEM_MODE | 0x01)]
    //SET_DREG 0x699 $regVal
    printk(KERN_EMERG "Set CFG_SYSTEM_MODE.CFG_MCUMD_INIT_DONE flag\n"); 
    rtl8291_setRegBit(devId, RTL8291_CFG_SYSTEM_MODE_ADDR, RTL8291_CFG_SYSTEM_MODE_CFG_MCUMD_INIT_DONE_OFFSET, 1);
          
    mdelay(10);          

    //puts " \[Check Chip Init Ready\] "
    rtl8291_getRegBit(devId, RTL8291_CHIP_INIT_RDY_ADDR, RTL8291_CHIP_INIT_RDY_CHIP_INIT_RDY_OFFSET, &chip_ready);
    printk(KERN_EMERG "Check Chip Init Ready: chip_ready = 0x%x\n", chip_ready); 
    
    return;
}



int32  rtl8291_apc_init(uint32 devId, uint8 *flash_data)
{
    uint32 value, value2, i;
    //uint8 ibim_sel;
    int32 step;

    printk(KERN_EMERG " APC Init....\n");

#if 0
    for(i=0;i<0x100;i++)
    { 
        if(i%16 == 0)
            printk(KERN_EMERG "%04d  ", i);    
        
        printk(KERN_EMERG "0x%02x ", flash_data[i]);
        if(i%8 == 7)
            printk(KERN_EMERG "   ");
        if(i%16 == 15)
            printk(KERN_EMERG "\n");                     
    }
#endif
    //20240522: Add DAC Offset, DAC Code use default value
    //rtl8291_dac_get(devId, &value2, &value);
    value = (flash_data[RTL8291_APD_DAC_CODE_OFFSET_ADDR]<<8)|flash_data[RTL8291_APD_DAC_CODE_OFFSET_ADDR+1];
    rtl8291_dac_set(devId, flash_data[RTL8291_DAC_OFFSET_OFFSET_ADDR], value);     
    printk(KERN_EMERG " DAC Offset = 0x%x, DAC Code = 0x%x \n", flash_data[RTL8291_DAC_OFFSET_OFFSET_ADDR], value);
    rtl8291_getReg(devId, RTL8291_WB_REG01_ADDR,&value); 
    printk(KERN_EMERG "Get WB_REG01(0x%x): data = 0x%x\n", RTL8291_WB_REG01_ADDR, value); 

#if 0 //From RL6733 TCL    
    //#-- step 11. set MOD DAC VBN2x and VBN1x
    //# W50<6> BEN_EN
    //SET_WREG 0x50 0xE0
    rtl8291_setReg(devId, RTL8291_WB_REG50_ADDR, 0xE0);
    //# W5B<3:0> Predriver Vout
    //SET_WREG 0x5B 0xE3
    rtl8291_setReg(devId, RTL8291_WB_REG5B_ADDR, 0xE3);

    //TCL: # 20220128 KCT comments for APC register settings
    //# -- begin --
    //SET_WREG 0x6F 0x64
    rtl8291_setReg(devId, RTL8291_WB_REG6F_ADDR, 0x64);
    
    //# 0xBA (w76<7>: REG_APCDID_CODE_LOADIN)
    //SET_WREG 0x76 0xBA
    rtl8291_setRegBit(devId, RTL8291_WB_REG76_ADDR, RTL8291_WB_REG76_REG_APCDIG_CODE_LOADIN_OFFSET, 1);
    
    //# 0xF8 (APC/ERC working time from 2.048us to 16.384us, APC digital loop gain from 2^-4(256ns) to 2^-7 (2.048us))
    //SET_WREG 0x77 0xF8
    rtl8291_setRegBits(devId, RTL8291_WB_REG77_ADDR, RTL8291_WB_REG77_REG_APCDIG_APC_LOOPGAIN_2_0_MASK, 0x4);
    
    //# 0x90 (ERC digital loop gain  from 2^-4(256ns) to 2^-7 (2.048us))
    //SET_WREG 0x78 0x90
    rtl8291_setRegBits(devId, RTL8291_WB_REG78_ADDR, RTL8291_WB_REG78_REG_APCDIG_ERC_LOOPGAIN_2_0_MASK, 0x4);    

    //SET_WREG 0x79 0x58
    rtl8291_setReg(devId, RTL8291_WB_REG79_ADDR, 0x58);        

    //SET_WREG 0x8A 0x90
    rtl8291_setReg(devId, RTL8291_WB_REG8A_ADDR, 0x90);    
    
    //# APC_CROSSING W67=0xBF (Down 16ps) 
    //SET_WREG 0x67 0xBF    
    rtl8291_setReg(devId, RTL8291_WB_REG67_ADDR, 0xBF);    
#endif

    //Flash parameter config
    rtl8291_txsdMode_set(devId, flash_data[RTL8291_TXSD_MODE_OFFSET_ADDR]);

    value =  flash_data[RTL8291_TXSD_VTH_OFFSET_ADDR];
    rtl8291_setRegBits(devId, RTL8291_WB_REG66_ADDR, RTL8291_WB_REG66_TXSD_VTH_SELL_2_0_MASK, value); 
    value =  flash_data[RTL8291_TXSD_VMODE_OFFSET_ADDR];
    rtl8291_setRegBit(devId, RTL8291_WB_REG66_ADDR, RTL8291_WB_REG66_TXSD_VMODE_SELL_OFFSET, value); 

    rtl8291_getReg(devId, RTL8291_WB_REG66_ADDR,&value); 
    printk(KERN_EMERG "Get WB_REG66(0x%x): data = 0x%x\n", RTL8291_WB_REG66_ADDR, value);

    rtl8291_txsdTh_set(devId, flash_data[RTL8291_TXSD_TH_OFFSET_ADDR]);

    rtl8291_apcIavg_set(devId, flash_data[RTL8291_APC_IAVG_SET_OFFSET_ADDR]);

    //if (ldd_pon_mode == RTL8291_DUAL_EPON_MODE)
    //{
    //    rtl8291_apcEr_set(devId, flash_data[RTL8291_EPON_APC_ER_SET_OFFSET_ADDR]);     
    //}
    //else
    {
        rtl8291_apcEr_set(devId, flash_data[RTL8291_APC_ER_SET_OFFSET_ADDR]); 
    }

    rtl8291_apcApcTimer_set(devId, flash_data[RTL8291_APC_TIMER_OFFSET_ADDR]);    
    rtl8291_apcErcTimer_set(devId, flash_data[RTL8291_ERC_TIMER_OFFSET_ADDR]);    
     
    //if (ldd_pon_mode == RTL8291_DUAL_EPON_MODE)
    //{
    //    rtl8291_apcLpfBw_set(devId, flash_data[RTL8291_EPON_APC_LPF_BW_OFFSET_ADDR]);     
    //}    
    //else
    {
        rtl8291_apcLpfBw_set(devId, flash_data[RTL8291_APC_LPF_BW_OFFSET_ADDR]);   
    }

    rtl8291_apcCalibration_set(devId, flash_data[RTL8291_ERC_CHOPPER_EN_OFFSET_ADDR]);

    //if (ldd_pon_mode == RTL8291_DUAL_EPON_MODE)
    //{
    //    step = flash_data[RTL8291_EPON_ERC_COMP_OFFSET_ADDR];
    //}
    //else
    //{
        step = flash_data[RTL8291_ERC_COMP_OFFSET_ADDR];
    //}
    
    //20240517: Removed    
    //rtl8291_ercComp_set(devId, step);

    value =  flash_data[RTL8291_INIT_IBIM_SEL_OFFSET_ADDR];
    rtl8291_setRegBit(devId, RTL8291_REG_LUT_CONTROL1_ADDR, RTL8291_REG_LUT_CONTROL1_CFG_TXAPD_LUT_START_OFFSET, value); 
    //20230704: Move from TX init to APC init. Before loopmode.
    rtl8291_apdLutEnable_set(devId, flash_data[RTL8291_APD_LUT_EN_OFFSET_ADDR]); 
    rtl8291_txLutEnable_set(devId, flash_data[RTL8291_TX_LUT_EN_OFFSET_ADDR]); 

#if 0
    value = flash_data[RTL8291_APC_IBIAS_INIT_OFFSET_ADDR];
    rtl8291_apcIbiasInitCode_set(devId, value);
    value = flash_data[RTL8291_APC_IMOD_INIT_OFFSET_ADDR];
    rtl8291_apcImodInitCode_set(devId, value);
#endif
    
    value = flash_data[RTL8291_APC_IMOD_INIT_OFFSET_ADDR];
    value2 = flash_data[RTL8291_APC_IBIAS_INIT_OFFSET_ADDR]; 
    value = (value<<4)|flash_data[RTL8291_APC_IMOD_INIT2_OFFSET_ADDR];	
    rtl8291_apcImod_biasInitCode_set(devId, value, value2);

    rtl8291_apcLoopMode_set(devId, flash_data[RTL8291_APC_LOOP_MODE_OFFSET_ADDR]);
    rtl8291_apcLoopMode_get(devId, &value);
    //printk(KERN_EMERG "Set Loop Mode to 0x%x\n", value);    

    rtl8291_apcLaserOnDelay_set(devId, flash_data[RTL8291_APC_LASER_ON_DELAY_OFFSET_ADDR]);

    rtl8291_apcSettleCnt_set(devId, flash_data[RTL8291_APC_SETTLE_COUNT_OFFSET_ADDR]);

    rtl8291_apcApcLoopGain_set(devId, flash_data[RTL8291_APC_LOOP_GAIN_OFFSET_ADDR]);
    rtl8291_apcErcLoopGain_set(devId, flash_data[RTL8291_ERC_LOOP_GAIN_OFFSET_ADDR]);

    //if (ldd_pon_mode == RTL8291_DUAL_EPON_MODE)
    //{
    //    rtl8291_apcCmpd_set(devId, flash_data[RTL8291_EPON_APC_CMPD_OFFSET_ADDR]);    
    //}
    //else
    {
        rtl8291_apcCmpd_set(devId, flash_data[RTL8291_APC_CMPD_OFFSET_ADDR]);
    }    
 
    //rtl8291_fsuApcRampb_set(devId, flash_data[RTL8291_FSU_APC_RAMPB_OFFSET_ADDR]);    
    //rtl8291_fsuApcRampm_set(devId, flash_data[RTL8291_FSU_APC_RAMPM_OFFSET_ADDR]);
    //rtl8291_fsuErcRampm_set(devId, flash_data[RTL8291_FSU_ERC_RAMPM_OFFSET_ADDR]);
    rtl8291_fsuEnable_set(devId, flash_data[RTL8291_FSU_ENABLE_OFFSET_ADDR]);
   
    rtl8291_apcModMax_set(devId, flash_data[RTL8291_APC_MOD_MAX_OFFSET_ADDR]);
    rtl8291_apcModMin_set(devId, flash_data[RTL8291_APC_MOD_MIN_OFFSET_ADDR]);
    rtl8291_apcBiasMax_set(devId, flash_data[RTL8291_APC_BIAS_MAX_OFFSET_ADDR]);
    rtl8291_apcBiasMin_set(devId, flash_data[RTL8291_APC_BIAS_MIN_OFFSET_ADDR]);
    rtl8291_apcModMaxEn_set(devId, flash_data[RTL8291_APC_MOD_MAX_EN_OFFSET_ADDR]);
    rtl8291_apcModMinEn_set(devId, flash_data[RTL8291_APC_MOD_MIN_EN_OFFSET_ADDR]);
    rtl8291_apcBiasMaxEn_set(devId, flash_data[RTL8291_APC_BIAS_MAX_EN_OFFSET_ADDR]);
    rtl8291_apcBiasMinEn_set(devId, flash_data[RTL8291_APC_BIAS_MIN_EN_OFFSET_ADDR]);
    
    //if (ldd_pon_mode == RTL8291_DUAL_EPON_MODE)
    //{
    //    rtl8291_apcCrossAdj_set(devId, flash_data[RTL8291_EPON_APC_CROSS_SIGN_OFFSET_ADDR], flash_data[RTL8291_EPON_APC_CROSS_STR_OFFSET_ADDR]);
    //    rtl8291_apcCrossEn_set(devId, flash_data[RTL8291_EPON_APC_CROSS_EN_OFFSET_ADDR]);  
    //}
    //else
    {
        rtl8291_apcCrossAdj_set(devId, flash_data[RTL8291_APC_CROSS_SIGN_OFFSET_ADDR], flash_data[RTL8291_APC_CROSS_STR_OFFSET_ADDR]);
        rtl8291_apcCrossEn_set(devId, flash_data[RTL8291_APC_CROSS_EN_OFFSET_ADDR]);
    } 

    return SUCCESS;
}

int32  rtl8291_tx_init(uint32 devId, uint8 *flash_data)
{
    uint32 value, value2;

    printk(KERN_EMERG " TX enable....\n");

    rtl8291_cmlNcapSel_set(devId, flash_data[RTL8291_CML_NCAP_SEL_OFFSET_ADDR]);

    rtl8291_vhvlGenRefOpt_set(devId, flash_data[RTL8291_VHVL_GEN_REF_OPTION_OFFSET_ADDR]);

    //W50<6> BEN_EN
    rtl8291_setRegBit(devId, RTL8291_WB_REG50_ADDR, RTL8291_WB_REG50_BEN_ENL_OFFSET, 0x1);

    rtl8291_getReg(devId, RTL8291_WB_REG50_ADDR,&value); 
    printk(KERN_EMERG "Get WB_REG50(0x%x): data = 0x%x\n", RTL8291_WB_REG50_ADDR, value);

    //W5B<3:0> Predriver Vout
    rtl8291_setReg(devId, RTL8291_WB_REG5B_ADDR, 0xE3);

#if 0 //From RL6733 TCL      
    //Set MOD DAC VBN2x and VBN1x
    //# W5E<5:3> MOD_DAC_VBN2P,  <2:0> MOD_DAC_VBN2N, 
    //SET_WREG 0x5E 0x1B
    rtl8291_setReg(devId, RTL8291_WB_REG5E_ADDR, 0x1B);
    //# W5F<7:5> MOD_DAC_VBN1P,  <4:2> MOD_DAC_VBN1N, 
    //SET_WREG 0x5F 0x6E
    rtl8291_setReg(devId, RTL8291_WB_REG5F_ADDR, 0x6E);

    //TCL: ENABLE_TX2
    rtl8291_setReg(devId, RTL8291_WB_REG5B_ADDR, 0x63);    
    rtl8291_setReg(devId, RTL8291_WB_REG54_ADDR, 0x8E);  
    rtl8291_setReg(devId, RTL8291_WB_REG56_ADDR, 0);  
    rtl8291_setReg(devId, RTL8291_WB_REG58_ADDR, 0xAA);  
    rtl8291_setReg(devId, RTL8291_WB_REG5E_ADDR, 0x1B);  
    rtl8291_setReg(devId, RTL8291_WB_REG5F_ADDR, 0x6E);  
    rtl8291_setReg(devId, RTL8291_WB_REG61_ADDR, 0x55);  

    //TCL: ENABLE_TX3
    rtl8291_setReg(devId, RTL8291_WB_REG5C_ADDR, 0x90);    
    rtl8291_setReg(devId, RTL8291_WB_REG51_ADDR, 0xF3);  
    rtl8291_setReg(devId, RTL8291_WB_REG52_ADDR, 0xB3);  
    rtl8291_setReg(devId, RTL8291_WB_REG5D_ADDR, 0x70);  
    rtl8291_setReg(devId, RTL8291_WB_REG58_ADDR, 0x88);  
    rtl8291_setReg(devId, RTL8291_WB_REG54_ADDR, 0x7E);  
    rtl8291_setReg(devId, RTL8291_WB_REG56_ADDR, 0xE0); 
    rtl8291_setReg(devId, RTL8291_WB_REG57_ADDR, 0x60);    
    rtl8291_setReg(devId, RTL8291_WB_REG5E_ADDR, 0x36);  
    rtl8291_setReg(devId, RTL8291_WB_REG5F_ADDR, 0x2);  
    rtl8291_setReg(devId, RTL8291_WB_REG61_ADDR, 0x28);  
    rtl8291_setReg(devId, RTL8291_WB_REG59_ADDR, 0x3C);  
    rtl8291_setReg(devId, RTL8291_WB_REG62_ADDR, 0xD9);  
    rtl8291_setReg(devId, RTL8291_WB_REG63_ADDR, 0x48);
    rtl8291_setReg(devId, RTL8291_WB_REG54_ADDR, 0x36);    
    rtl8291_setReg(devId, RTL8291_WB_REG5A_ADDR, 0x27);  
    rtl8291_setReg(devId, RTL8291_WB_REG5B_ADDR, 0xE3);  
    rtl8291_setReg(devId, RTL8291_WB_REG61_ADDR, 0x6);  
    rtl8291_setReg(devId, RTL8291_WB_REG5E_ADDR, 0x36);  
    rtl8291_setReg(devId, RTL8291_WB_REG62_ADDR, 0x59);  
    rtl8291_setReg(devId, RTL8291_WB_REG63_ADDR, 0x40); 
    rtl8291_setReg(devId, RTL8291_WB_REG54_ADDR, 0x36);    

#endif

    //Flash parameter config  
    rtl8291_txInPolSwap_set(devId, flash_data[RTL8291_TX_IN_POL_SWAP_OFFSET_ADDR]);

    rtl8291_txOutPolSwap_set(devId, flash_data[RTL8291_TX_OUT_POL_SWAP_OFFSET_ADDR]);    
    //rtl8291_txInterface_set(devId, flash_data[RTL8291_TX_INTERFACE_OFFSET_ADDR]); 

    //rtl8291_txDcdCalEn_set(devId, flash_data[RTL8290B_TX_DCD_CAL_EN_OFFSET_ADDR]);    

    //value = flash_data[RTL8291_TX_BACK_TERM_TYPE_OFFSET_ADDR];
    //value2 = flash_data[RTL8291_TX_BACK_TERM_VAL_OFFSET_ADDR];
    //rtl8291_txBackTerm_set(devId, value, value2);

    rtl8291_txCrossEn_set(devId, flash_data[RTL8291_TX_CROSS_EN_OFFSET_ADDR]);
    value2 = flash_data[RTL8291_TX_CROSS_STR_OFFSET_ADDR];;
    value = flash_data[RTL8291_TX_CROSS_SIGN_OFFSET_ADDR];
    rtl8291_txCrossAdj_set(devId, value, value2);

    value = flash_data[RTL8291_TX_ES_EMP_VG_MODE_OFFSET_ADDR];
    rtl8291_txEsConfig_set(devId, LDD_TXES_VG_MODE, value);
    value = flash_data[RTL8291_TX_ES_EMP_VG_STR_OFFSET_ADDR];
    rtl8291_txEsConfig_set(devId, LDD_TXES_VG_STR, value);
    value = flash_data[RTL8291_TX_ES_EMP_LEVEL_OFFSET_ADDR];
    rtl8291_txEsConfig_set(devId, LDD_TXES_EMP_LEVEL, value);
    value = flash_data[RTL8291_TX_ES_EMP_VG_AMP_OFFSET_ADDR];
    rtl8291_txEsConfig_set(devId, LDD_TXES_VG_AMP, value);
    value = flash_data[RTL8291_TX_ES_DELAY_RISE_OFFSET_ADDR];
    rtl8291_txEsConfig_set(devId, LDD_TXES_DELY_RISING, value);
    value = flash_data[RTL8291_TX_ES_DELAY_FALL_OFFSET_ADDR];
    rtl8291_txEsConfig_set(devId, LDD_TXES_DELY_FALLING, value);
    value = flash_data[RTL8291_TX_ES_ADJ_ENABLE_OFFSET_ADDR];
    rtl8291_txEsConfig_set(devId, LDD_TXES_ADJ_ENABLE, value);

    return SUCCESS;
}

int32  rtl8291_rx_init(uint32 devId, uint8 *flash_data)
{
    uint32 value;
    //uint32 mask;

    printk(KERN_EMERG " RX enable....\n");     
#if 0
    //Set pre-emphasis  (SET_WREG 0x24 0xDE)
    rtl8291_setReg(devId, RTL8291_WB_REG24_ADDR, 0xDE);      
    //Set Tr/Tf symmetrical (SET_WREG 0x25 0xA2)
    rtl8291_setReg(devId, RTL8291_WB_REG25_ADDR, 0xA2);     
    //Set DE-ISI to reduce ISI jitter (SET_WREG 0x26 0xA0)
    rtl8291_setReg(devId, RTL8291_WB_REG26_ADDR, 0xA0);     
    //Compensate amplitude reduced by de-emphasis (SET_WREG 0x29 0xA0)
    rtl8291_setReg(devId, RTL8291_WB_REG29_ADDR, 0xA0);    
    //RX EQ (SET_WREG 0x22 0x94)
    rtl8291_setReg(devId, RTL8291_WB_REG22_ADDR, 0x94);

    //TCL: ENABLE_RX2
    //SET_WREG 0x21 0x20
    rtl8291_setReg(devId, RTL8291_WB_REG21_ADDR, 0x20);
    //SET_WREG 0x22 0x92
    rtl8291_setReg(devId, RTL8291_WB_REG22_ADDR, 0x92);
    //SET_WREG 0x23 0xCC
    rtl8291_setReg(devId, RTL8291_WB_REG23_ADDR, 0xCC);
    //SET_WREG 0x24 0x00
    rtl8291_setReg(devId, RTL8291_WB_REG24_ADDR, 0);    
    //SET_WREG 0x25 0xA2
    rtl8291_setReg(devId, RTL8291_WB_REG25_ADDR, 0xA2);    
    //SET_WREG 0x26 0xC8
    rtl8291_setReg(devId, RTL8291_WB_REG26_ADDR, 0xC8);    
    //SET_WREG 0x27 0x6C
    rtl8291_setReg(devId, RTL8291_WB_REG27_ADDR, 0x6C);    
    //SET_WREG 0x28 0xAC
    rtl8291_setReg(devId, RTL8291_WB_REG28_ADDR, 0xAC);    
    //SET_WREG 0x29 0x84
    rtl8291_setReg(devId, RTL8291_WB_REG29_ADDR, 0x84);    
    //SET_WREG 0x25 0x02
    rtl8291_setReg(devId, RTL8291_WB_REG25_ADDR, 0x02);    
    //SET_WREG 0x24 0xe0
    rtl8291_setReg(devId, RTL8291_WB_REG24_ADDR, 0xE0);        
    //SET_WREG 0x26 0xb0
    rtl8291_setReg(devId, RTL8291_WB_REG26_ADDR, 0xB0);        
    //SET_WREG 0x22 0x98
    rtl8291_setReg(devId, RTL8291_WB_REG22_ADDR, 0x98);        

    //TCL: ENABLE_RX3
    //SET_WREG 0x21 0x10 ( decrease AMP's current default value)
    rtl8291_setReg(devId, RTL8291_WB_REG21_ADDR, 0x10);
    //SET_WREG 0x22 0x92
    rtl8291_setReg(devId, RTL8291_WB_REG22_ADDR, 0x92);
    //SET_WREG 0x23 0x9c (Change OS's current default value)
    rtl8291_setReg(devId, RTL8291_WB_REG23_ADDR, 0x9C);    
    //SET_WREG 0x24 0x00(if turn on pre-emphasis, set W24=0xC8)
     rtl8291_setReg(devId, RTL8291_WB_REG24_ADDR, 0);   
    //SET_WREG 0x25 0x90 (decrease delay,  Try to not change DVCM's offset current)
    rtl8291_setReg(devId, RTL8291_WB_REG25_ADDR, 0x90);        
    //SET_WREG 0x26 0x94 (Change DeISI's default value)
    rtl8291_setReg(devId, RTL8291_WB_REG26_ADDR, 0x94);            
    //SET_WREG 0x27 0x2c (Change LDAMP's default value)
    rtl8291_setReg(devId, RTL8291_WB_REG27_ADDR, 0x2C);        
    //SET_WREG 0x28 0xac (Change LDDP's default value)
    rtl8291_setReg(devId, RTL8291_WB_REG28_ADDR, 0xAC);        
    //SET_WREG 0x28 0xa0 ( Change LDDP's default value, to save power)
    rtl8291_setReg(devId, RTL8291_WB_REG28_ADDR, 0xA0);        
    //SET_WREG 0x29 0x60 ([4:0] to set amplitude)
    rtl8291_setReg(devId, RTL8291_WB_REG29_ADDR, 0x60);        
    //SET_WREG 0x2a 0x00
    rtl8291_setReg(devId, RTL8291_WB_REG2A_ADDR, 0);    

    //Jitter optimization
    //SET_WREG 0x21 0x30 (increase amplifier current)
    rtl8291_setReg(devId, RTL8291_WB_REG21_ADDR, 0x30);
    //SET_WREG 0x27 0x6c (increase LDO voltage   NOTE: to avoid overstress, LDO voltage must be set after power on  (PoR is 011))
    rtl8291_setReg(devId, RTL8291_WB_REG27_ADDR, 0x6C);

#endif

    //TCL: ENABLE_RXLOS
    //Enable RXLOS clock
    //set W2B_val [ expr ($W2B_val | 0x80) 
    //SET_WREG 0x2B $W2B_val      
    rtl8291_setRegBit(devId, RTL8291_WB_REG2B_ADDR, RTL8291_WB_REG2B_REG_RXLOS_CKGEN_EN_L_OFFSET, 1);

    //Flash parameter config 
    rtl8291_rxOutPolSwap_set(devId, flash_data[RTL8291_RX_OUT_POL_SWAP_OFFSET_ADDR]);     
    rtl8291_rxOutSwing_set(devId, flash_data[RTL8291_RX_OUT_SWING_OFFSET_ADDR]);
    rtl8291_rxDisOffcan_set(devId, flash_data[RTL8291_RX_DIS_OFFCAN_OFFSET_ADDR]);

    rtl8291_rxlosResetb_set(devId, flash_data[RTL8291_RXLOS_RESETB_OFFSET_ADDR]);
    rtl8291_rxlosClkMode_set(devId, flash_data[RTL8291_RXLOS_CLK_MODE_OFFSET_ADDR]);
    rtl8291_rxlosHystSel_set(devId, flash_data[RTL8291_RXLOS_HYST_SEL_OFFSET_ADDR]);
    rtl8291_rxlosRangSel_set(devId, flash_data[RTL8291_RXLOS_RANGE_SEL_OFFSET_ADDR]);   
    rtl8291_rxlosRefDac_set(devId, flash_data[RTL8291_RXLOS_REF_DAC_OFFSET_ADDR]);
    
    //rtl8291_rxlosChopperFreq_set(devId, flash_data[RTL8291_RXLOS_CHOPPER_FREQ_OFFSET_ADDR]);
    //rtl8291_rxlosSampleSel_set(devId, flash_data[RTL8291_RXLOS_SAMPLE_SEL_OFFSET_ADDR]);
    //rtl8291_rxlosChopperEn_set(devId, flash_data[RTL8291_RXLOS_CHOPPER_EN_OFFSET_ADDR]);
    //rtl8291_rxlosLaMagComp_set(devId, flash_data[RTL8291_RXLOS_LA_MAG_COMP_OFFSET_ADDR]);
    //rtl8291_rxlosBufAutozero_set(devId, flash_data[RTL8291_RXLOS_BUF_AUTOZERO_OFFSET_ADDR]);
 
    //rtl8291_rxlosTestMode_set(devId, flash_data[RTL8291_RXLOS_TESTMODE_OFFSET_ADDR]);
    rtl8291_rxlosAssertSel_set(devId, flash_data[RTL8291_RXLOS_ASSERT_SEL_OFFSET_ADDR]);

    rtl8291_rxlosDebounceSel_set(devId, flash_data[RTL8291_RXLOS_DEBOUNCE_SEL_OFFSET_ADDR]);
    rtl8291_rxlosDebounceOpt_set(devId, flash_data[RTL8291_RXLOS_DEBOUNCE_OPT_OFFSET_ADDR]);
    rtl8291_rxlosSquelch_set(devId, flash_data[RTL8291_RXLOS_SQUELCH_OFFSET_ADDR]);
   
    rtl8291_rxlosPolarity_set(devId, flash_data[RTL8291_RXLOS_POLARITY_OFFSET_ADDR]);
    rtl8291_rxlosPinEn_set(devId, flash_data[RTL8291_RXLOS_PIN_EN_OFFSET_ADDR]);

    rtl8291_txsdFaultTimer_set(devId, flash_data[RTL8291_TXSD_FAULT_TIMER_OFFSET_ADDR]);

    return SUCCESS;
}

//used to load 8051 code
void rtl8291_up_init(void)
{

    return;
}


void rtl8291_init(uint32 devId, uint8 *flash_data, rtl8291_param_t *ldd_param, uint32 version)
{
    uint32 value, value2;
    uint32 status;
    uint32 status2;
    //uint32 v_mpd;
    int i;
    uint32 addr=0;

    if (flash_data==NULL)
    {
        printk(KERN_EMERG "rtl8291_init FAIL: flash_data==NULL \n");    
        return;
    }
    
    //Check Chip ID    
    rtl8291_chipInfo_get(devId, &value, &value2);             
    if (value != 0x8291)
    {
        printk(KERN_EMERG "rtl8291_init: Error RTL NUM = 0x%04X I2C_PORT = %d!!! \n", value, I2C_PORT);    
        return;
    }

    rtl8291_reset(devId,LDD_RESET_CHIP);

    mdelay(20);  /* Modify to 20ms */
    
    rtl8291_default_init(devId, flash_data);

    if (version==0)
    {
        rtl8291_romcode_patch(devId);
    }
    else if (version==96)
    {
        rtl8291_romcode_96_patch(devId, 0);
    }
    else if (version==100)
    {
        rtl8291_romcode_100_patch(devId, 0);
    }
    else if (version==103)
    {
        rtl8291_romcode_103_patch(devId, 0);
    }
    else if (version==105)
    {
        rtl8291_romcode_105_patch(devId, 0);
    }    
#if 1
    rtl8291_apc_init(devId, &flash_data[RTL8291_PARAMETER_BASE_ADDR]);
  
    rtl8291_tx_init(devId, &flash_data[RTL8291_PARAMETER_BASE_ADDR]);

    rtl8291_rx_init(devId, &flash_data[RTL8291_PARAMETER_BASE_ADDR]);
#endif
   
    rtl8291_mcu_init(devId);

    //20240521: VDD 2.4V issue patch
    rtl8291_setRegBit(devId, RTL8291_WB_REG4A_ADDR, 2, 1);
    //rtl8291_setRegBit(devId, RTL8291_CFG_DDMI_EXTRA_CTL_ADDR, RTL8291_CFG_DDMI_EXTRA_CTL_CFG_DDMI_BASE_SEL_OFFSET, 1);

    //20240618: If IBX = 0, set IBX to 0x13
    rtl8291_getRegBits(devId, RTL8291_WB_REG0A_ADDR, RTL8291_WB_REG0A_RBG_BIT_L_5_0_MASK, &value);   
    if (value ==0)
    {
        rtl8291_setRegBits(devId, RTL8291_WB_REG0A_ADDR, RTL8291_WB_REG0A_RBG_BIT_L_5_0_MASK, 0x13);
    }

#if 1
    //----unmask all interrupt----  
    rtl8291_setReg(devId, RTL8291_REG_INT_MASK1_ADDR, 0);
    rtl8291_setReg(devId, RTL8291_REG_INT_MASK2_ADDR, 0);    
         
    rtl8291_powerOnStatus_get(devId, &status);
    if (status !=0)
    {
        printk("rtl8291_powerOnStatus_get error!!!!!!!status = 0x%x \n", status);
        return;
    }

    rtl8291_ready_check(devId); 

    //Fault enable
    //rtl8291_setReg(devId, RTL8290C_WB_REG6B_ADDR, 0xF8);
    addr = RTL8291_PARAMETER_BASE_ADDR+RTL8291_FAULT_ACTION_OFFSET_ADDR;   
    rtl8291_setRegBits(devId, RTL8291_REG_FAULT_CTL_ADDR, RTL8291_REG_FAULT_CTL_FAULT_ACTION_MASK, flash_data[addr]);
    addr = RTL8291_PARAMETER_BASE_ADDR+RTL8291_FAULT_INHIBIT_ENABLE_OFFSET_ADDR;
    rtl8291_setRegBits(devId, RTL8291_REG_FAULT_INHIBIT_ADDR, 0xff, flash_data[addr]);

    //20240704 Add
    addr = RTL8291_FAULT_CFG1_OFFSET_ADDR+RTL8291_PARAMETER_BASE_ADDR;
    value = flash_data[addr];
    //WB_REG16[4] DIG_VDD fault detection power on
    rtl8291_setRegBit(devId, RTL8291_WB_REG16_ADDR, 4, value&0x1);
    //WB_REG65[7] TX_VDD fault detection power on
    //WB_REG65[6] TX_VDD too high detection enable
    //WB_REG65[5] TX_VDD too low detection enable
    //WB_REG65[4] TX_LASERPN fault detection power on
    //WB_REG65[3] TX_LASERPN fault detection enable
    //WB_REG65[2] TX_LASERPN rogue onu detection enable 
    //WB_REG65[1] TX_LASERPN too low detection enable
    rtl8291_getReg(devId, RTL8291_WB_REG65_ADDR, &value2);
    value2 = (value2&0x1)|(value&0xFE);
    rtl8291_setReg(devId, RTL8291_WB_REG65_ADDR, value2);

    addr = RTL8291_FAULT_CFG2_OFFSET_ADDR+RTL8291_PARAMETER_BASE_ADDR;
    value = flash_data[addr];
    //WB_REG20[1] RX_VDD fault detection power on
    rtl8291_setRegBit(devId, RTL8291_WB_REG20_ADDR, 1, value&0x1);
    //WB_REG66[7] TX_BIASP fault detection power on
    //WB_REG66[6] TX_BIASP fault detection enable
    //WB_REG66[5] TX_BIASP rogue onu detection enable 
    //WB_REG66[4] TX_BIASP too low detection enable
    rtl8291_getReg(devId, RTL8291_WB_REG66_ADDR, &value2);
    value2 = (value2&0xF)|(value&0xF0);
    rtl8291_setReg(devId, RTL8291_WB_REG66_ADDR, value2);


    addr = RTL8291_FAULT_CFG3_OFFSET_ADDR+RTL8291_PARAMETER_BASE_ADDR;
    value = flash_data[addr];
    //WB_REG86[5] APC fault detection enable 
    //WB_REG86[4] APC MPD too high detection enable 
    //WB_REG86[3] APC MPD too low detection enable 
    //WB_REG86[2] APC_VDD fault detection enable 
    //WB_REG86[1] APC_VDD too high detection enable 
    //WB_REG86[0] APC_VDD too low detection enable 
    rtl8291_getReg(devId, RTL8291_WB_REG86_ADDR, &value2);
    value2 = (value2&0xC0)|(value&0x3F);
    rtl8291_setReg(devId, RTL8291_WB_REG86_ADDR, value2);


    /* wait no fault condition */
    rtl8291_getReg(devId, RTL8291_REG_FAULT_STATUS_ADDR, &status2);
    if(status2 != 0)
    {
        printk("\n!!! FAULT STATUS in Startup Procedure !!!\n");

        rtl8291_fault_status(devId);

        rtl8291_getRegBits(devId, RTL8291_REG_STATUS_2_ADDR,0xFF, &status2);
        if((RTL8291_REG_STATUS_2_FAULT_STATUS_MASK&status2)!=0 )
        {
            //Stop init procedure, close TX, LDX, booster...
            rtl8291_initProcFaultAction_set(devId, flash_data[RTL8291_PARAMETER_BASE_ADDR+RTL8291_INIT_PROC_FAULT_ACTION_OFFSET_ADDR]);
        }
    }
#endif

    rtl8291_getReg(devId, RTL8291_WB_REG4A_ADDR, &value);
    printk(KERN_EMERG "Get WB_REG4A(0x%x): data = 0x%x\n", RTL8291_WB_REG4A_ADDR, value); 
    rtl8291_getReg(devId, RTL8291_CFG_DDMI_EXTRA_CTL_ADDR, &value2);
    printk(KERN_EMERG "Get CFG_DDMI_EXTRA_CTL(0x%x): data = 0x%x\n", RTL8291_CFG_DDMI_EXTRA_CTL_ADDR, value2); 
    rtl8291_getReg(devId, RTL8291_WB_REG0A_ADDR, &value);
    printk(KERN_EMERG "Get WB_REG0A(0x%x): data = 0x%x\n", RTL8291_WB_REG0A_ADDR, value);
    rtl8291_getReg(devId, RTL8291_WB_REG01_ADDR,&value); 
    printk(KERN_EMERG "Get WB_REG01(0x%x): data = 0x%x\n", RTL8291_WB_REG01_ADDR, value);
    rtl8291_getReg(devId, RTL8291_WB_REG79_ADDR,&value); 
    printk(KERN_EMERG "Get WB_REG79(0x%x): data = 0x%x\n", RTL8291_WB_REG79_ADDR, value);     


    //printk("[ Enable up      ]\n");
    //Keep the shell, needs to update 8051 code....
    rtl8291_up_init();
}

void rtl8291_init_test(uint32 devId, uint8 *flash_data, rtl8291_param_t *ldd_param)
{
    uint32 value, value2;
    uint32 status;
    uint32 status2;
    //uint32 v_mpd;
    int i;
    //uint16 addr;

    if (flash_data==NULL)
    {
        printk(KERN_EMERG "rtl8291_init FAIL: flash_data==NULL \n");    
        return;
    }
    
    //Check Chip ID    
    rtl8291_chipInfo_get(devId, &value, &value2);             
    if (value != 0x8291)
    {
        printk(KERN_EMERG "rtl8291_init: Error RTL NUM = 0x%04X I2C_PORT = %d!!! \n", value, I2C_PORT);    
        return;
    }

#if 0 
    rtl8291_reset(devId,LDD_RESET_CHIP);

    mdelay(20);  /* Modify to 20ms */
   
    rtl8291_default_init(devId, flash_data);
#endif

    //rtl8291_setRegBit(devId, RTL8291_CFG_DDMI_EXTRA_CTL_ADDR, RTL8291_CFG_DDMI_EXTRA_CTL_CFG_DDMI_BASE_SEL_OFFSET, 1);

    rtl8291_romcode_patch(devId);

#if 0
    rtl8291_apc_init(devId, &flash_data[RTL8291_PARAMETER_BASE_ADDR]);

    rtl8291_getReg(devId, RTL8291_WB_REG50_ADDR,&value); 
    printk(KERN_EMERG "Get WB_REG50(0x%x): data = 0x%x\n", RTL8291_WB_REG50_ADDR, value);


    rtl8291_tx_init(devId, &flash_data[RTL8291_PARAMETER_BASE_ADDR]);

    rtl8291_getReg(devId, RTL8291_WB_REG50_ADDR,&value); 
    printk(KERN_EMERG "Get WB_REG50(0x%x): data = 0x%x\n", RTL8291_WB_REG50_ADDR, value);

    rtl8291_rx_init(devId, &flash_data[RTL8291_PARAMETER_BASE_ADDR]);

    rtl8291_getReg(devId, RTL8291_WB_REG50_ADDR,&value); 
    printk(KERN_EMERG "Get WB_REG50(0x%x): data = 0x%x\n", RTL8291_WB_REG50_ADDR, value);

    rtl8291_mcu_init(devId);

    //20240521: VDD 2.4V issue patch
    rtl8291_setRegBit(devId, RTL8291_WB_REG4A_ADDR, 2, 1);

    //20240618: If IBX = 0, set IBX to 0x13
    rtl8291_getRegBits(devId, RTL8291_WB_REG0A_ADDR, RTL8291_WB_REG0A_RBG_BIT_L_5_0_MASK, &value);   
    if (value ==0)
    {
        rtl8291_setRegBits(devId, RTL8291_WB_REG0A_ADDR, RTL8291_WB_REG0A_RBG_BIT_L_5_0_MASK, 0x13);
    }

    rtl8291_getReg(devId, RTL8291_WB_REG50_ADDR,&value); 
    printk(KERN_EMERG "Get WB_REG50(0x%x): data = 0x%x\n", RTL8291_WB_REG50_ADDR, value);


    rtl8291_getReg(devId, RTL8291_WB_REG79_ADDR,&value); 
    printk(KERN_EMERG "Get WB_REG79(0x%x): data = 0x%x\n", RTL8291_WB_REG79_ADDR, value);
#endif

#if 0
    //----unmask all interrupt----  
    rtl8291_setReg(devId, RTL8291_REG_INT_MASK1_ADDR, 0);
    rtl8291_setReg(devId, RTL8291_REG_INT_MASK2_ADDR, 0);    
         
    rtl8291_powerOnStatus_get(devId, &status);
    if (status !=0)
    {
        printk("rtl8291_powerOnStatus_get error!!!!!!!status = 0x%x \n", status);
        return;
    }

    rtl8291_ready_check(devId); 

    //Fault enable
    //rtl8291_setReg(devId, RTL8290C_WB_REG6B_ADDR, 0xF8);
    rtl8291_setReg(devId, RTL8291_REG_FAULT_CTL_ADDR, 0);
    rtl8291_setRegBits(devId, RTL8291_REG_FAULT_INHIBIT_ADDR, 0xff, 0);

    /* wait no fault condition */
    rtl8291_getReg(devId, RTL8291_REG_FAULT_STATUS_ADDR, &status2);
    if(status2 != 0)
    {
        printk("\n!!! FAULT STATUS in Startup Procedure !!!\n");

        rtl8291_fault_status(devId);

        rtl8291_getRegBits(devId, RTL8291_REG_STATUS_2_ADDR,0xFF, &status2);
        if((RTL8291_REG_STATUS_2_FAULT_STATUS_MASK&status2)!=0 )
        {
            //Stop init procedure, close TX, LDX, booster...
            rtl8291_initProcFaultAction_set(devId, flash_data[RTL8291_PARAMETER_BASE_ADDR+RTL8291_INIT_PROC_FAULT_ACTION_OFFSET_ADDR]);
        }
    }
#endif

    //rtl8291_getReg(devId, RTL8291_WB_REG4A_ADDR, &value);
    //printk(KERN_EMERG "Get WB_REG4A(0x%x): data = 0x%x\n", RTL8291_WB_REG4A_ADDR, value); 
    //rtl8291_getReg(devId, RTL8291_CFG_DDMI_EXTRA_CTL_ADDR, &value2);
    //printk(KERN_EMERG "Get CFG_DDMI_EXTRA_CTL(0x%x): data = 0x%x\n", RTL8291_CFG_DDMI_EXTRA_CTL_ADDR, value2); 
    //rtl8291_getReg(devId, RTL8291_WB_REG0A_ADDR, &value);
    //printk(KERN_EMERG "Get WB_REG0A(0x%x): data = 0x%x\n", RTL8291_WB_REG0A_ADDR, value);
    //rtl8291_getReg(devId, RTL8291_WB_REG01_ADDR,&value); 
    //printk(KERN_EMERG "Get WB_REG01(0x%x): data = 0x%x\n", RTL8291_WB_REG01_ADDR, value);     
    //rtl8291_getReg(devId, RTL8291_WB_REG79_ADDR,&value); 
    //printk(KERN_EMERG "Get WB_REG79(0x%x): data = 0x%x\n", RTL8291_WB_REG79_ADDR, value);

    //printk("[ Enable up      ]\n");
    //Keep the shell, needs to update 8051 code....
    rtl8291_up_init();
}

//For FT test only!!!!! Mapping to RTK API
void rtl8291_chip_init(uint32 devId)
{
    uint32 dev_idx;

#if 0 
    printk(KERN_EMERG "IROM Patch...\n");        
    rtl8291_romcode_patch(devId);

    //rtl8291_apc_init(devId, &flash_data[RTL8291_PARAMETER_BASE_ADDR]);

    //rtl8291_tx_init(devId, &flash_data[RTL8291_PARAMETER_BASE_ADDR]);

    //rtl8291_rx_init(devId, &flash_data[RTL8291_PARAMETER_BASE_ADDR]);

    //printk(KERN_EMERG "MCU Init...\n");
    //rtl8291_mcu_init(devId); 
#endif

#if 1
    printk(KERN_EMERG "Init RTL8291 module devId = %d...\n", devId);
    dev_idx = devId - 5;
    //printk(KERN_EMERG "Read Flash Data...\n");  
    //_rtl8291_load_config_file(devId, flash_data);

    rtl8291_init(devId, flash_data, &ldd_param[dev_idx], 105);
    //rtl8291_user_parameter_init(devId, &flash_data[RTL8291_PARAMETER_BASE_ADDR], &ldd_param[dev_idx]);
#endif
    printk(KERN_EMERG "RTL8291 Init Finish.\n");

}


int32 rtl8291_test_init(uint32 devId, uint32 value)
{
    int ret=0;
    uint32 dev_idx;
    uint32 value1, value2, addr, i;
    uint32 status;
    uint32 status2;

    dev_idx    = devId - 5;

    //Check Chip ID    
    rtl8291_chipInfo_get(devId, &value1, &value2);             
    if (value1 != 0x8291)
    {
        printk(KERN_EMERG "rtl8291_init: Error RTL NUM = 0x%04X!!! \n", value1);    
        return 0;
    }    
    printk(KERN_EMERG "RTL NUM = 0x%04X Test Value = %d \n", value1, value);    

    if (value==0)
    {
        printk(KERN_EMERG "0.Load Flash Data...\n");  
        _rtl8291_load_config_file(devId, flash_data);
    }
    if (value==1)
    {
        printk(KERN_EMERG "1. ROM Patch (latest version)...\n");      
        rtl8291_romcode_patch(devId);
    }
    else if (value==2)
    {
        printk(KERN_EMERG "2. APC init...\n");     
        rtl8291_apc_init(devId, &flash_data[RTL8291_PARAMETER_BASE_ADDR]);   
    }
    else if (value==3)
    {
        printk(KERN_EMERG "3. TX init...\n");      
        rtl8291_tx_init(devId, &flash_data[RTL8291_PARAMETER_BASE_ADDR]);
    }
    else if (value==4)
    {
         printk(KERN_EMERG "4. RX init...\n");   
        rtl8291_rx_init(devId, &flash_data[RTL8291_PARAMETER_BASE_ADDR]);
    }    
    else if (value==5)
    {
        printk(KERN_EMERG "5. MCU init...\n");    
        rtl8291_mcu_init(devId);
    }    
    else if (value ==6)
    {
         printk(KERN_EMERG "6. VDD & IBX patch...\n");     
        //20240521: VDD 2.4V issue patch
        rtl8291_setRegBit(devId, RTL8291_WB_REG4A_ADDR, 2, 1);
        //rtl8291_setRegBit(devId, RTL8291_CFG_DDMI_EXTRA_CTL_ADDR, RTL8291_CFG_DDMI_EXTRA_CTL_CFG_DDMI_BASE_SEL_OFFSET, 1);
        
        //20240618: If IBX = 0, set IBX to 0x13
        rtl8291_getRegBits(devId, RTL8291_WB_REG0A_ADDR, RTL8291_WB_REG0A_RBG_BIT_L_5_0_MASK, &value);     
        if (value ==0)
        {
            rtl8291_setRegBits(devId, RTL8291_WB_REG0A_ADDR, RTL8291_WB_REG0A_RBG_BIT_L_5_0_MASK, 0x13);
        }
    }
    else if (value ==7)
    {
        printk(KERN_EMERG "7. Checking Stage...\n");      
        //----unmask all interrupt----    
        rtl8291_setReg(devId, RTL8291_REG_INT_MASK1_ADDR, 0);
        addr = RTL8291_PARAMETER_BASE_ADDR+RTL8291_FAULT_INHIBIT_ENABLE_OFFSET_ADDR;
        rtl8291_setRegBits(devId, RTL8291_REG_FAULT_INHIBIT_ADDR, 0xff, flash_data[addr]);
             
        rtl8291_powerOnStatus_get(devId, &status);
        if (status !=0)
        {
            printk("rtl8291_powerOnStatus_get error!!!!!!!status = 0x%x \n", status);
            return status;
        }
    
        rtl8291_ready_check(devId); 

        //Fault enable
        //rtl8291_setReg(devId, RTL8290C_WB_REG6B_ADDR, 0xF8);
        addr = RTL8291_PARAMETER_BASE_ADDR+RTL8291_FAULT_ACTION_OFFSET_ADDR;   
        rtl8291_setRegBits(devId, RTL8291_REG_FAULT_CTL_ADDR, RTL8291_REG_FAULT_CTL_FAULT_ACTION_MASK, flash_data[addr]);
        addr = RTL8291_PARAMETER_BASE_ADDR+RTL8291_FAULT_INHIBIT_ENABLE_OFFSET_ADDR;
        rtl8291_setRegBits(devId, RTL8291_REG_FAULT_INHIBIT_ADDR, 0xff, flash_data[addr]);

        //20240704 Add
        addr = RTL8291_FAULT_CFG1_OFFSET_ADDR+RTL8291_PARAMETER_BASE_ADDR;
        value1 = flash_data[addr];
        //WB_REG16[4] DIG_VDD fault detection power on
        rtl8291_setRegBit(devId, RTL8291_WB_REG16_ADDR, 4, value1&0x1);
        //WB_REG65[7] TX_VDD fault detection power on
        //WB_REG65[6] TX_VDD too high detection enable
        //WB_REG65[5] TX_VDD too low detection enable
        //WB_REG65[4] TX_LASERPN fault detection power on
        //WB_REG65[3] TX_LASERPN fault detection enable
        //WB_REG65[2] TX_LASERPN rogue onu detection enable 
        //WB_REG65[1] TX_LASERPN too low detection enable
        rtl8291_getReg(devId, RTL8291_WB_REG65_ADDR, &value2);
        value2 = (value2&0x1)|(value1&0xFE);
        rtl8291_setReg(devId, RTL8291_WB_REG65_ADDR, value2);
        
        addr = RTL8291_FAULT_CFG2_OFFSET_ADDR+RTL8291_PARAMETER_BASE_ADDR;
        value1 = flash_data[addr];
        //WB_REG20[1] RX_VDD fault detection power on
        rtl8291_setRegBit(devId, RTL8291_WB_REG20_ADDR, 1, value1&0x1);
        //WB_REG66[7] TX_BIASP fault detection power on
        //WB_REG66[6] TX_BIASP fault detection enable
        //WB_REG66[5] TX_BIASP rogue onu detection enable 
        //WB_REG66[4] TX_BIASP too low detection enable
        rtl8291_getReg(devId, RTL8291_WB_REG66_ADDR, &value2);
        value2 = (value2&0xF)|(value1&0xF0);
        rtl8291_setReg(devId, RTL8291_WB_REG66_ADDR, value2);
                
        addr = RTL8291_FAULT_CFG3_OFFSET_ADDR+RTL8291_PARAMETER_BASE_ADDR;
        value1 = flash_data[addr];
        //WB_REG86[5] APC fault detection enable 
        //WB_REG86[4] APC MPD too high detection enable 
        //WB_REG86[3] APC MPD too low detection enable 
        //WB_REG86[2] APC_VDD fault detection enable 
        //WB_REG86[1] APC_VDD too high detection enable 
        //WB_REG86[0] APC_VDD too low detection enable 
        rtl8291_getReg(devId, RTL8291_WB_REG86_ADDR, &value2);
        value2 = (value2&0xC0)|(value1&0x3F);
        rtl8291_setReg(devId, RTL8291_WB_REG86_ADDR, value2);
            
        /* wait no fault condition */
        rtl8291_getReg(devId, RTL8291_REG_FAULT_STATUS_ADDR, &status2);
        if(status2 != 0)
        {
            printk("\n!!! FAULT STATUS in Startup Procedure !!!\n");
        
            rtl8291_fault_status(devId);
    
            rtl8291_getRegBits(devId, RTL8291_REG_STATUS_2_ADDR,0xFF, &status2);
            if((RTL8291_REG_STATUS_2_FAULT_STATUS_MASK&status2)!=0 )
            {
                //Stop init procedure, close TX, LDX, booster...
                rtl8291_initProcFaultAction_set(devId, flash_data[RTL8291_PARAMETER_BASE_ADDR+RTL8291_INIT_PROC_FAULT_ACTION_OFFSET_ADDR]);
            }
        }
    }  
    else if (value ==8)
    {
        printk(KERN_EMERG "8. A0/A2/LUT init...\n"); 
        rtl8291_default_init(devId, flash_data);
    }
    else if (value ==9)
    {
        printk(KERN_EMERG "9. User Parameter init...\n"); 
        rtl8291_user_parameter_init(devId, &flash_data[RTL8291_PARAMETER_BASE_ADDR], &ldd_param[dev_idx]);
    }
    else if (value==10)
    {
        printk(KERN_EMERG "10. ROM Patch (96 version)...\n");      
        rtl8291_romcode_96_patch(devId, 1);
    }
    else if (value==11)
    {
        printk(KERN_EMERG "11. ROM Patch (100 version)...\n");      
        rtl8291_romcode_100_patch(devId, 1);
    }    
    else if (value==12)
    {
        printk(KERN_EMERG "12. ROM Patch (103 version)...\n");      
        rtl8291_romcode_103_patch(devId, 1);
    } 
    else if (value==13)
    {
        printk(KERN_EMERG "13. Use ROM Patch 96 to init...\n"); 
        printk(KERN_EMERG "Init RTL8291 module devId = %d...\n", devId);        
        printk(KERN_EMERG "Load Flash Data...\n");  
        _rtl8291_load_config_file(devId, flash_data);
        rtl8291_init(devId, flash_data, &ldd_param[dev_idx], 96);    
        rtl8291_user_parameter_init(devId, &flash_data[RTL8291_PARAMETER_BASE_ADDR], &ldd_param[dev_idx]);
    }
    else if (value==14)
    {
        printk(KERN_EMERG "13. Use ROM Patch 100 to init...\n"); 
        printk(KERN_EMERG "Init RTL8291 module devId = %d...\n", devId);        
        printk(KERN_EMERG "Load Flash Data...\n");  
        _rtl8291_load_config_file(devId, flash_data);
        rtl8291_init(devId, flash_data, &ldd_param[dev_idx], 100);    
        rtl8291_user_parameter_init(devId, &flash_data[RTL8291_PARAMETER_BASE_ADDR], &ldd_param[dev_idx]);
    }
    else if (value==15)
    {
        printk(KERN_EMERG "15. Use ROM Patch 103 to init...\n"); 
        printk(KERN_EMERG "Init RTL8291 module devId = %d...\n", devId);        
        printk(KERN_EMERG "Load Flash Data...\n");  
        _rtl8291_load_config_file(devId, flash_data);
        rtl8291_init(devId, flash_data, &ldd_param[dev_idx], 103);    
        rtl8291_user_parameter_init(devId, &flash_data[RTL8291_PARAMETER_BASE_ADDR], &ldd_param[dev_idx]);
    } 
    else if (value==16)
    {
        printk(KERN_EMERG "15. Use ROM Patch 105 to init...\n"); 
        printk(KERN_EMERG "Init RTL8291 module devId = %d...\n", devId);        
        printk(KERN_EMERG "Load Flash Data...\n");  
        _rtl8291_load_config_file(devId, flash_data);
        rtl8291_init(devId, flash_data, &ldd_param[dev_idx], 105);    
        rtl8291_user_parameter_init(devId, &flash_data[RTL8291_PARAMETER_BASE_ADDR], &ldd_param[dev_idx]);
    }     
    else if (value==0xFF)
    {
        printk(KERN_EMERG "16. Dump Flash Parameter\n");
        addr = RTL8291_PARAMETER_BASE_ADDR;
        for(i=0;i<0x100;i++)
        { 
            if(i%16 == 0)
                printk(KERN_EMERG "%04d  ", addr+i);    
            
            printk(KERN_EMERG "0x%02x ", flash_data[addr+i]);
            if(i%8 == 7)
                printk(KERN_EMERG "   ");
            if(i%16 == 15)
                printk(KERN_EMERG "\n");                     
        }
    }
    printk(KERN_EMERG "RTL8291 Test Init Finish.\n");

    return ret;
}

#if 0
void rtl8291_romcode_96_patch(uint32 devId)
{
    uint32 i=0, data=0;
    uint32 cnt=0, status=0;

    printk(KERN_EMERG "rtl8291_romcode_96_patch...\n"); 

 
    // set bit-2 is 1
    rtl8291_setReg(devId, RTL8291_REG_CONTROL1_ADDR, 0x4);
    printk(KERN_EMERG "SET_DREG $REG_CONTROL1  0x4\n");    
    // set bit-3 is 1
    rtl8291_setReg(devId, RTL8291_REG_IIC_S_ADDR, 0x1C);
    printk(KERN_EMERG "SET_DREG $REG_IIC_S   0x1C\n");    

    for (i=0;i<RTL8291_ROMCODE_96_PATCH_SIZE;i++)
    {
        if (i<(RTL8291_ROMCODE_96_PATCH_SIZE-1))
        {
        rtl8291_setReg(devId, RTL8291_REG_INDACC_ADDR_LB_ADDR, romcode_96_data[i][0]);
        printk(KERN_EMERG "SET_DREG $REG_INDACC_ADDR_LB  0x%02x \n", romcode_96_data[i][0]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_ADDR_HB_ADDR, romcode_96_data[i][1]);
        printk(KERN_EMERG "SET_DREG $REG_INDACC_ADDR_HB  0x%02x \n", romcode_96_data[i][1]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_0_ADDR, romcode_96_data[i][2]);
        printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_0  0x%02x \n", romcode_96_data[i][2]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_1_ADDR, romcode_96_data[i][3]);
        printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_1  0x%02x \n", romcode_96_data[i][3]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_2_ADDR, romcode_96_data[i][4]);
        printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_2  0x%02x \n", romcode_96_data[i][4]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_3_ADDR, romcode_96_data[i][5]);
        printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_3  0x%02x \n", romcode_96_data[i][5]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_4_ADDR, romcode_96_data[i][6]);
        printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_4  0x%02x \n", romcode_96_data[i][6]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_5_ADDR, romcode_96_data[i][7]);
        printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_5  0x%02x \n", romcode_96_data[i][7]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_6_ADDR, romcode_96_data[i][8]);
        printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_6  0x%02x \n", romcode_96_data[i][8]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_7_ADDR, romcode_96_data[i][9]);
        printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_7  0x%02x \n", romcode_96_data[i][9]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_8_ADDR, romcode_96_data[i][10]);
        printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_8  0x%02x \n", romcode_96_data[i][10]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_9_ADDR, romcode_96_data[i][11]);
        printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_9  0x%02x \n", romcode_96_data[i][11]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_10_ADDR, romcode_96_data[i][12]);
        printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_10  0x%02x \n", romcode_96_data[i][12]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_11_ADDR, romcode_96_data[i][13]);
        printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_11  0x%02x \n", romcode_96_data[i][13]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_12_ADDR, romcode_96_data[i][14]);
        printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_12  0x%02x \n", romcode_96_data[i][14]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_13_ADDR, romcode_96_data[i][15]);
        printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_13  0x%02x \n", romcode_96_data[i][15]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_14_ADDR, romcode_96_data[i][16]);
        printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_14  0x%02x \n", romcode_96_data[i][16]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_15_ADDR, romcode_96_data[i][17]);
        printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_15  0x%02x \n", romcode_96_data[i][17]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_CONTROL_ADDR, romcode_96_data[i][18]);
        printk(KERN_EMERG "SET_DREG $REG_INDACC_CONTROL  0x%02x  ;# // set 16-byte write\n\n", romcode_96_data[i][18]);
        //Poll NCTRL busy bit
        cnt=0;
        while(cnt<1000) //check NCTRL BUSY
        {
            rtl8291_getRegBit(devId, RTL8291_REG_INDACC_CONTROL_ADDR, RTL8291_REG_INDACC_CONTROL_INDACC_CMD_OFFSET, &status);
            if(status == 0)
            {
                break;
            }
            cnt++;
        }
        rtl8291_getReg(devId, RTL8291_REG_INDACC_CONTROL_ADDR, &status);
        printk(KERN_EMERG "REG_INDACC_CONTROL = 0x%x Busy Cnt= %d \n\n", status, cnt);
        }
        else
        {
            rtl8291_setReg(devId, RTL8291_REG_INDACC_ADDR_LB_ADDR, romcode_96_data[i][0]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_ADDR_LB  0x%02x \n", romcode_96_data[i][0]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_ADDR_HB_ADDR, romcode_96_data[i][1]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_ADDR_HB  0x%02x \n", romcode_96_data[i][1]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_0_ADDR, romcode_96_data[i][2]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_0  0x%02x \n", romcode_96_data[i][2]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_1_ADDR, romcode_96_data[i][3]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_1  0x%02x \n", romcode_96_data[i][3]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_2_ADDR, romcode_96_data[i][4]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_2  0x%02x \n", romcode_96_data[i][4]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_3_ADDR, romcode_96_data[i][5]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_3  0x%02x \n", romcode_96_data[i][5]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_4_ADDR, romcode_96_data[i][6]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_4  0x%02x \n", romcode_96_data[i][6]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_5_ADDR, romcode_96_data[i][7]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_5  0x%02x \n", romcode_96_data[i][7]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_6_ADDR, romcode_96_data[i][8]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_6  0x%02x \n", romcode_96_data[i][8]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_7_ADDR, romcode_96_data[i][9]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_7  0x%02x \n", romcode_96_data[i][9]);
#if 0
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_8_ADDR, romcode_96_data[i][10]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_8  0x%02x \n", romcode_96_data[i][10]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_9_ADDR, romcode_96_data[i][11]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_9  0x%02x \n", romcode_96_data[i][11]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_10_ADDR, romcode_96_data[i][12]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_10  0x%02x \n", romcode_96_data[i][12]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_11_ADDR, romcode_96_data[i][13]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_11  0x%02x \n", romcode_96_data[i][13]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_12_ADDR, romcode_96_data[i][14]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_12  0x%02x \n", romcode_96_data[i][14]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_13_ADDR, romcode_96_data[i][15]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_13  0x%02x \n", romcode_96_data[i][15]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_14_ADDR, romcode_96_data[i][16]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_14  0x%02x \n", romcode_96_data[i][16]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_15_ADDR, romcode_96_data[i][17]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_15  0x%02x \n", romcode_96_data[i][17]);
#endif
            rtl8291_setReg(devId, RTL8291_REG_INDACC_CONTROL_ADDR, romcode_96_data[i][18]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_CONTROL  0x%02x  ;# // set 16-byte write\n\n", romcode_96_data[i][18]);
            //Poll NCTRL busy bit
            cnt=0;
            while(cnt<1000) //check NCTRL BUSY
            {
                rtl8291_getRegBit(devId, RTL8291_REG_INDACC_CONTROL_ADDR, RTL8291_REG_INDACC_CONTROL_INDACC_CMD_OFFSET, &status);
                if(status == 0)
                {
                    break;
                }
                cnt++;
            }
            rtl8291_getReg(devId, RTL8291_REG_INDACC_CONTROL_ADDR, &status);
            printk(KERN_EMERG "REG_INDACC_CONTROL = 0x%x Busy Cnt= %d \n\n", status, cnt);
        }
    }

    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P00_L_ADDR, romcode_cfg_96_nctl_data[0]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P00_L  0x%x \n", romcode_cfg_96_nctl_data[0]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P00_H_ADDR, romcode_cfg_96_nctl_data[1]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P00_H  0x%x \n", romcode_cfg_96_nctl_data[1]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P00_L_ADDR, romcode_cfg_96_nctl_data[2]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P00_L  0x%x \n", romcode_cfg_96_nctl_data[2]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P00_H_ADDR, romcode_cfg_96_nctl_data[3]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P00_H  0x%x \n", romcode_cfg_96_nctl_data[3]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P01_L_ADDR, romcode_cfg_96_nctl_data[4]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P01_L  0x%x \n", romcode_cfg_96_nctl_data[4]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P01_H_ADDR, romcode_cfg_96_nctl_data[5]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P01_H  0x%x \n", romcode_cfg_96_nctl_data[5]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P01_L_ADDR, romcode_cfg_96_nctl_data[6]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P01_L  0x%x \n", romcode_cfg_96_nctl_data[6]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P01_H_ADDR, romcode_cfg_96_nctl_data[7]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P01_H  0x%x \n", romcode_cfg_96_nctl_data[7]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P02_L_ADDR, romcode_cfg_96_nctl_data[8]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P02_L  0x%x \n", romcode_cfg_96_nctl_data[8]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P02_H_ADDR, romcode_cfg_96_nctl_data[9]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P02_H  0x%x \n", romcode_cfg_96_nctl_data[9]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P02_L_ADDR, romcode_cfg_96_nctl_data[10]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P02_L  0x%02x \n", romcode_cfg_96_nctl_data[10]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P02_H_ADDR, romcode_cfg_96_nctl_data[11]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P02_H  0x%x \n", romcode_cfg_96_nctl_data[11]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P03_L_ADDR, romcode_cfg_96_nctl_data[12]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P03_L  0x%x \n", romcode_cfg_96_nctl_data[12]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P03_H_ADDR, romcode_cfg_96_nctl_data[13]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P03_H  0x%x \n", romcode_cfg_96_nctl_data[13]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P03_L_ADDR, romcode_cfg_96_nctl_data[14]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P03_L  0x%x \n", romcode_cfg_96_nctl_data[14]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P03_H_ADDR, romcode_cfg_96_nctl_data[15]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P03_H  0x%x \n", romcode_cfg_96_nctl_data[15]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P04_L_ADDR, romcode_cfg_96_nctl_data[16]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P04_L  0x%x \n", romcode_cfg_96_nctl_data[16]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P04_H_ADDR, romcode_cfg_96_nctl_data[17]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P04_H  0x%x \n", romcode_cfg_96_nctl_data[17]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P04_L_ADDR, romcode_cfg_96_nctl_data[18]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P04_L  0x%x \n", romcode_cfg_96_nctl_data[18]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P04_H_ADDR, romcode_cfg_96_nctl_data[19]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P04_H  0x%x \n", romcode_cfg_96_nctl_data[19]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P05_L_ADDR, romcode_cfg_96_nctl_data[20]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P05_L  0x%x \n", romcode_cfg_96_nctl_data[20]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P05_H_ADDR, romcode_cfg_96_nctl_data[21]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P05_H  0x%x \n", romcode_cfg_96_nctl_data[21]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P05_L_ADDR, romcode_cfg_96_nctl_data[22]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P05_L  0x%02x \n", romcode_cfg_96_nctl_data[22]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P05_H_ADDR, romcode_cfg_96_nctl_data[23]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P05_H  0x%x \n", romcode_cfg_96_nctl_data[23]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P06_L_ADDR, romcode_cfg_96_nctl_data[24]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P06_L  0x%x \n", romcode_cfg_96_nctl_data[24]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P06_H_ADDR, romcode_cfg_96_nctl_data[25]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P06_H  0x%x \n", romcode_cfg_96_nctl_data[25]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P06_L_ADDR, romcode_cfg_96_nctl_data[26]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P06_L  0x%x \n", romcode_cfg_96_nctl_data[26]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P06_H_ADDR, romcode_cfg_96_nctl_data[27]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P06_H  0x%x \n", romcode_cfg_96_nctl_data[27]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P07_L_ADDR, romcode_cfg_96_nctl_data[28]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P07_L  0x%x \n", romcode_cfg_96_nctl_data[28]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P07_H_ADDR, romcode_cfg_96_nctl_data[29]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P07_H  0x%x \n", romcode_cfg_96_nctl_data[29]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P07_L_ADDR, romcode_cfg_96_nctl_data[30]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P07_L  0x%x \n", romcode_cfg_96_nctl_data[30]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P07_H_ADDR, romcode_cfg_96_nctl_data[31]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P07_H  0x%x \n", romcode_cfg_96_nctl_data[31]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P08_L_ADDR, romcode_cfg_96_nctl_data[32]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P08_L  0x%x \n", romcode_cfg_96_nctl_data[32]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P08_H_ADDR, romcode_cfg_96_nctl_data[33]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P08_H  0x%x \n", romcode_cfg_96_nctl_data[33]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P08_L_ADDR, romcode_cfg_96_nctl_data[34]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P08_L  0x%02x \n", romcode_cfg_96_nctl_data[34]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P08_H_ADDR, romcode_cfg_96_nctl_data[35]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P08_H  0x%x \n", romcode_cfg_96_nctl_data[35]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P09_L_ADDR, romcode_cfg_96_nctl_data[36]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P09_L  0x%x \n", romcode_cfg_96_nctl_data[36]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P09_H_ADDR, romcode_cfg_96_nctl_data[37]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P09_H  0x%x \n", romcode_cfg_96_nctl_data[37]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P09_L_ADDR, romcode_cfg_96_nctl_data[38]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P09_L  0x%x \n", romcode_cfg_96_nctl_data[38]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P09_H_ADDR, romcode_cfg_96_nctl_data[39]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P09_H  0x%x \n", romcode_cfg_96_nctl_data[39]);

    // set bit-3 is 0
    rtl8291_setReg(devId, RTL8291_REG_IIC_S_ADDR, 0x15);
    printk(KERN_EMERG "SET_DREG $REG_IIC_S   0x15\n");    
    // set bit-2 is 0
    rtl8291_setReg(devId, RTL8291_REG_CONTROL1_ADDR, 0);
    printk(KERN_EMERG "SET_DREG $REG_CONTROL1  0\n");    

    return;
}

void rtl8291_romcode_100_patch(uint32 devId)
{
    uint32 i=0, data=0;
    uint32 cnt=0, status=0;

    printk(KERN_EMERG "rtl8291_romcode_100_patch...\n"); 

 
    // set bit-2 is 1
    rtl8291_setReg(devId, RTL8291_REG_CONTROL1_ADDR, 0x4);
    printk(KERN_EMERG "SET_DREG $REG_CONTROL1  0x4\n");    
    // set bit-3 is 1
    rtl8291_setReg(devId, RTL8291_REG_IIC_S_ADDR, 0x1C);
    printk(KERN_EMERG "SET_DREG $REG_IIC_S   0x1C\n");    

    for (i=0;i<RTL8291_ROMCODE_100_PATCH_SIZE;i++)
    {
        if (i<(RTL8291_ROMCODE_100_PATCH_SIZE-1))
        {
        rtl8291_setReg(devId, RTL8291_REG_INDACC_ADDR_LB_ADDR, romcode_100_data[i][0]);
        printk(KERN_EMERG "SET_DREG $REG_INDACC_ADDR_LB  0x%02x \n", romcode_100_data[i][0]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_ADDR_HB_ADDR, romcode_100_data[i][1]);
        printk(KERN_EMERG "SET_DREG $REG_INDACC_ADDR_HB  0x%02x \n", romcode_100_data[i][1]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_0_ADDR, romcode_100_data[i][2]);
        printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_0  0x%02x \n", romcode_100_data[i][2]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_1_ADDR, romcode_100_data[i][3]);
        printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_1  0x%02x \n", romcode_100_data[i][3]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_2_ADDR, romcode_100_data[i][4]);
        printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_2  0x%02x \n", romcode_100_data[i][4]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_3_ADDR, romcode_100_data[i][5]);
        printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_3  0x%02x \n", romcode_100_data[i][5]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_4_ADDR, romcode_100_data[i][6]);
        printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_4  0x%02x \n", romcode_100_data[i][6]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_5_ADDR, romcode_100_data[i][7]);
        printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_5  0x%02x \n", romcode_100_data[i][7]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_6_ADDR, romcode_100_data[i][8]);
        printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_6  0x%02x \n", romcode_100_data[i][8]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_7_ADDR, romcode_100_data[i][9]);
        printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_7  0x%02x \n", romcode_100_data[i][9]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_8_ADDR, romcode_100_data[i][10]);
        printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_8  0x%02x \n", romcode_100_data[i][10]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_9_ADDR, romcode_100_data[i][11]);
        printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_9  0x%02x \n", romcode_100_data[i][11]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_10_ADDR, romcode_100_data[i][12]);
        printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_10  0x%02x \n", romcode_100_data[i][12]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_11_ADDR, romcode_100_data[i][13]);
        printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_11  0x%02x \n", romcode_100_data[i][13]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_12_ADDR, romcode_100_data[i][14]);
        printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_12  0x%02x \n", romcode_100_data[i][14]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_13_ADDR, romcode_100_data[i][15]);
        printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_13  0x%02x \n", romcode_100_data[i][15]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_14_ADDR, romcode_100_data[i][16]);
        printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_14  0x%02x \n", romcode_100_data[i][16]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_15_ADDR, romcode_100_data[i][17]);
        printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_15  0x%02x \n", romcode_100_data[i][17]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_CONTROL_ADDR, romcode_100_data[i][18]);
        printk(KERN_EMERG "SET_DREG $REG_INDACC_CONTROL  0x%02x  ;# // set 16-byte write\n\n", romcode_100_data[i][18]);
        //Poll NCTRL busy bit
        cnt=0;
        while(cnt<1000) //check NCTRL BUSY
        {
            rtl8291_getRegBit(devId, RTL8291_REG_INDACC_CONTROL_ADDR, RTL8291_REG_INDACC_CONTROL_INDACC_CMD_OFFSET, &status);
            if(status == 0)
            {
                break;
            }
            cnt++;
        }
        rtl8291_getReg(devId, RTL8291_REG_INDACC_CONTROL_ADDR, &status);
        printk(KERN_EMERG "REG_INDACC_CONTROL = 0x%x Busy Cnt= %d \n\n", status, cnt);
        }
        else
        {
            rtl8291_setReg(devId, RTL8291_REG_INDACC_ADDR_LB_ADDR, romcode_100_data[i][0]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_ADDR_LB  0x%02x \n", romcode_100_data[i][0]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_ADDR_HB_ADDR, romcode_100_data[i][1]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_ADDR_HB  0x%02x \n", romcode_100_data[i][1]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_0_ADDR, romcode_100_data[i][2]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_0  0x%02x \n", romcode_100_data[i][2]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_1_ADDR, romcode_100_data[i][3]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_1  0x%02x \n", romcode_100_data[i][3]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_2_ADDR, romcode_100_data[i][4]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_2  0x%02x \n", romcode_100_data[i][4]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_3_ADDR, romcode_100_data[i][5]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_3  0x%02x \n", romcode_100_data[i][5]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_4_ADDR, romcode_100_data[i][6]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_4  0x%02x \n", romcode_100_data[i][6]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_5_ADDR, romcode_100_data[i][7]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_5  0x%02x \n", romcode_100_data[i][7]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_6_ADDR, romcode_100_data[i][8]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_6  0x%02x \n", romcode_100_data[i][8]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_7_ADDR, romcode_100_data[i][9]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_7  0x%02x \n", romcode_100_data[i][9]);
#if 0
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_8_ADDR, romcode_96_data[i][10]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_8  0x%02x \n", romcode_96_data[i][10]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_9_ADDR, romcode_96_data[i][11]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_9  0x%02x \n", romcode_96_data[i][11]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_10_ADDR, romcode_96_data[i][12]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_10  0x%02x \n", romcode_96_data[i][12]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_11_ADDR, romcode_96_data[i][13]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_11  0x%02x \n", romcode_96_data[i][13]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_12_ADDR, romcode_96_data[i][14]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_12  0x%02x \n", romcode_96_data[i][14]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_13_ADDR, romcode_96_data[i][15]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_13  0x%02x \n", romcode_96_data[i][15]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_14_ADDR, romcode_96_data[i][16]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_14  0x%02x \n", romcode_96_data[i][16]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_15_ADDR, romcode_96_data[i][17]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_15  0x%02x \n", romcode_96_data[i][17]);
#endif
            rtl8291_setReg(devId, RTL8291_REG_INDACC_CONTROL_ADDR, romcode_96_data[i][18]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_CONTROL  0x%02x  ;# // set 16-byte write\n\n", romcode_96_data[i][18]);
            //Poll NCTRL busy bit
            cnt=0;
            while(cnt<1000) //check NCTRL BUSY
            {
                rtl8291_getRegBit(devId, RTL8291_REG_INDACC_CONTROL_ADDR, RTL8291_REG_INDACC_CONTROL_INDACC_CMD_OFFSET, &status);
                if(status == 0)
                {
                    break;
                }
                cnt++;
            }
            rtl8291_getReg(devId, RTL8291_REG_INDACC_CONTROL_ADDR, &status);
            printk(KERN_EMERG "REG_INDACC_CONTROL = 0x%x Busy Cnt= %d \n\n", status, cnt);
        }
    }

    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P00_L_ADDR, 0x02);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P00_L  0x%x \n", 0x02);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P00_H_ADDR, 0x10);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P00_H  0x%x \n", 0x10);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P00_L_ADDR, 0x0);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P00_L  0x%x \n", 0x0);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P00_H_ADDR, 0xc);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P00_H  0x%x \n", 0xc);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P01_L_ADDR, 0x65);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P01_L  0x%x \n", 0x65);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P01_H_ADDR, 0x14);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P01_H  0x%x \n", 0x14);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P01_L_ADDR, 0x2);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P01_L  0x%x \n", 0x2);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P01_H_ADDR, 0xc);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P01_H  0x%x \n", 0xc);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P02_L_ADDR, 0x80);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P02_L  0x%x \n", 0x80);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P02_H_ADDR, 0x14);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P02_H  0x%x \n", 0x14);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P02_L_ADDR, 0x4);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P02_L  0x%x \n", 0x4);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P02_H_ADDR, 0xc);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P02_H  0x%x \n", 0xc);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P03_L_ADDR, 0x65);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P03_L  0x%x \n", 0x65);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P03_H_ADDR, 0x12);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P03_H  0x%x \n", 0x12);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P03_L_ADDR, 0x7);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P03_L  0x%x \n", 0x7);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P03_H_ADDR, 0xc);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P03_H  0x%x \n", 0xc);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P04_L_ADDR, 0xa5);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P04_L  0x%x \n", 0xa5);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P04_H_ADDR, 0x12);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P04_H  0x%x \n", 0x12);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P04_L_ADDR, 0xbd);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P04_L  0x%x \n", 0xbd);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P04_H_ADDR, 0xc);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P04_H  0x%x \n", 0xc);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P05_L_ADDR, 0xba);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P05_L  0x%x \n", 0xba);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P05_H_ADDR, 0x12);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P05_H  0x%x \n", 0x12);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P05_L_ADDR, 0xe1);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P05_L  0x%x \n", 0xe1);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P05_H_ADDR, 0xc);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P05_H  0x%x \n", 0xc);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P06_L_ADDR, 0x0b);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P06_L  0x%x \n", 0x0b);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P06_H_ADDR, 0x13);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P06_H  0x%x \n", 0x13);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P06_L_ADDR, 0xec);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P06_L  0x%x \n", 0xec);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P06_H_ADDR, 0xc);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P06_H  0x%x \n", 0xc);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P07_L_ADDR, 0x13);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P07_L  0x%x \n", 0x13);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P07_H_ADDR, 0x13);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P07_H  0x%x \n", 0x13);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P07_L_ADDR, 0x10);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P07_L  0x%x \n", 0x10);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P07_H_ADDR, 0xd);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P07_H  0x%x \n", 0xd);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P08_L_ADDR, 0x60);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P08_L  0x%x \n", 0x60);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P08_H_ADDR, 0x13);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P08_H  0x%x \n", 0x13);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P08_L_ADDR, 0xa0);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P08_L  0x%x \n", 0xa0);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P08_H_ADDR, 0xd);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P08_H  0x%x \n", 0xd);

    // set bit-3 is 0
    rtl8291_setReg(devId, RTL8291_REG_IIC_S_ADDR, 0x15);
    printk(KERN_EMERG "SET_DREG $REG_IIC_S   0x15\n");     
    // set bit-2 is 0
    rtl8291_setReg(devId, RTL8291_REG_CONTROL1_ADDR, 0);
    printk(KERN_EMERG "SET_DREG $REG_CONTROL1  0\n");       

#if 0
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P00_L_ADDR, romcode_cfg__nctl_data[0]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P00_L  0x%x \n", romcode_cfg__nctl_data[0]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P00_H_ADDR, romcode_cfg__nctl_data[1]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P00_H  0x%x \n", romcode_cfg__nctl_data[1]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P00_L_ADDR, romcode_cfg__nctl_data[2]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P00_L  0x%x \n", romcode_cfg__nctl_data[2]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P00_H_ADDR, romcode_cfg__nctl_data[3]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P00_H  0x%x \n", romcode_cfg__nctl_data[3]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P01_L_ADDR, romcode_cfg__nctl_data[4]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P01_L  0x%x \n", romcode_cfg__nctl_data[4]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P01_H_ADDR, romcode_cfg__nctl_data[5]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P01_H  0x%x \n", romcode_cfg__nctl_data[5]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P01_L_ADDR, romcode_cfg__nctl_data[6]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P01_L  0x%x \n", romcode_cfg__nctl_data[6]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P01_H_ADDR, romcode_cfg__nctl_data[7]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P01_H  0x%x \n", romcode_cfg__nctl_data[7]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P02_L_ADDR, romcode_cfg__nctl_data[8]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P02_L  0x%x \n", romcode_cfg__nctl_data[8]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P02_H_ADDR, romcode_cfg__nctl_data[9]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P02_H  0x%x \n", romcode_cfg__nctl_data[9]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P02_L_ADDR, romcode_cfg__nctl_data[10]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P02_L  0x%02x \n", romcode_cfg__nctl_data[10]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P02_H_ADDR, romcode_cfg__nctl_data[11]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P02_H  0x%x \n", romcode_cfg__nctl_data[11]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P03_L_ADDR, romcode_cfg__nctl_data[12]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P03_L  0x%x \n", romcode_cfg__nctl_data[12]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P03_H_ADDR, romcode_cfg__nctl_data[13]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P03_H  0x%x \n", romcode_cfg__nctl_data[13]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P03_L_ADDR, romcode_cfg__nctl_data[14]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P03_L  0x%x \n", romcode_cfg__nctl_data[14]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P03_H_ADDR, romcode_cfg__nctl_data[15]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P03_H  0x%x \n", romcode_cfg__nctl_data[15]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P04_L_ADDR, romcode_cfg__nctl_data[16]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P04_L  0x%x \n", romcode_cfg__nctl_data[16]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P04_H_ADDR, romcode_cfg__nctl_data[17]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P04_H  0x%x \n", romcode_cfg__nctl_data[17]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P04_L_ADDR, romcode_cfg__nctl_data[18]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P04_L  0x%x \n", romcode_cfg__nctl_data[18]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P04_H_ADDR, romcode_cfg__nctl_data[19]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P04_H  0x%x \n", romcode_cfg__nctl_data[19]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P05_L_ADDR, romcode_cfg__nctl_data[20]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P05_L  0x%x \n", romcode_cfg__nctl_data[20]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P05_H_ADDR, romcode_cfg__nctl_data[21]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P05_H  0x%x \n", romcode_cfg__nctl_data[21]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P05_L_ADDR, romcode_cfg__nctl_data[22]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P05_L  0x%02x \n", romcode_cfg__nctl_data[22]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P05_H_ADDR, romcode_cfg__nctl_data[23]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P05_H  0x%x \n", romcode_cfg__nctl_data[23]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P06_L_ADDR, romcode_cfg__nctl_data[24]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P06_L  0x%x \n", romcode_cfg__nctl_data[24]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P06_H_ADDR, romcode_cfg__nctl_data[25]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P06_H  0x%x \n", romcode_cfg__nctl_data[25]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P06_L_ADDR, romcode_cfg__nctl_data[26]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P06_L  0x%x \n", romcode_cfg__nctl_data[26]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P06_H_ADDR, romcode_cfg__nctl_data[27]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P06_H  0x%x \n", romcode_cfg__nctl_data[27]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P07_L_ADDR, romcode_cfg__nctl_data[28]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P07_L  0x%x \n", romcode_cfg__nctl_data[28]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P07_H_ADDR, romcode_cfg__nctl_data[29]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P07_H  0x%x \n", romcode_cfg__nctl_data[29]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P07_L_ADDR, romcode_cfg__nctl_data[30]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P07_L  0x%x \n", romcode_cfg__nctl_data[30]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P07_H_ADDR, romcode_cfg__nctl_data[31]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P07_H  0x%x \n", romcode_cfg__nctl_data[31]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P08_L_ADDR, romcode_cfg__nctl_data[32]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P08_L  0x%x \n", romcode_cfg__nctl_data[32]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P08_H_ADDR, romcode_cfg__nctl_data[33]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P08_H  0x%x \n", romcode_cfg__nctl_data[33]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P08_L_ADDR, romcode_cfg__nctl_data[34]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P08_L  0x%02x \n", romcode_cfg__nctl_data[34]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P08_H_ADDR, romcode_cfg__nctl_data[35]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P08_H  0x%x \n", romcode_cfg__nctl_data[35]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P09_L_ADDR, romcode_cfg__nctl_data[36]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P09_L  0x%x \n", romcode_cfg__nctl_data[36]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P09_H_ADDR, romcode_cfg__nctl_data[37]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P09_H  0x%x \n", romcode_cfg__nctl_data[37]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P09_L_ADDR, romcode_cfg__nctl_data[38]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P09_L  0x%x \n", romcode_cfg__nctl_data[38]);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P09_H_ADDR, romcode_cfg__nctl_data[39]);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P09_H  0x%x \n", romcode_cfg__nctl_data[39]);

    // set bit-3 is 0
    rtl8291_setReg(devId, RTL8291_REG_IIC_S_ADDR, 0x15);
    printk(KERN_EMERG "SET_DREG $REG_IIC_S   0x15\n");    
    // set bit-2 is 0
    rtl8291_setReg(devId, RTL8291_REG_CONTROL1_ADDR, 0);
    printk(KERN_EMERG "SET_DREG $REG_CONTROL1  0\n");    
#endif
    return;
}


void rtl8291_romcode_103_patch(uint32 devId)
{
    uint32 i=0, data=0;
    uint32 cnt=0, status=0;

    printk(KERN_EMERG "rtl8291_romcode_103_patch...\n"); 

 
    // set bit-2 is 1
    rtl8291_setReg(devId, RTL8291_REG_CONTROL1_ADDR, 0x4);
    printk(KERN_EMERG "SET_DREG $REG_CONTROL1  0x4\n");    
    // set bit-3 is 1
    rtl8291_setReg(devId, RTL8291_REG_IIC_S_ADDR, 0x1C);
    printk(KERN_EMERG "SET_DREG $REG_IIC_S   0x1C\n");    

    for (i=0;i<RTL8291_ROMCODE_103_PATCH_SIZE;i++)
    {
        if (i<(RTL8291_ROMCODE_103_PATCH_SIZE-1))
        {
        rtl8291_setReg(devId, RTL8291_REG_INDACC_ADDR_LB_ADDR, romcode_103_data[i][0]);
        printk(KERN_EMERG "SET_DREG $REG_INDACC_ADDR_LB  0x%02x \n", romcode_103_data[i][0]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_ADDR_HB_ADDR, romcode_103_data[i][1]);
        printk(KERN_EMERG "SET_DREG $REG_INDACC_ADDR_HB  0x%02x \n", romcode_103_data[i][1]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_0_ADDR, romcode_103_data[i][2]);
        printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_0  0x%02x \n", romcode_103_data[i][2]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_1_ADDR, romcode_103_data[i][3]);
        printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_1  0x%02x \n", romcode_103_data[i][3]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_2_ADDR, romcode_103_data[i][4]);
        printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_2  0x%02x \n", romcode_103_data[i][4]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_3_ADDR, romcode_103_data[i][5]);
        printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_3  0x%02x \n", romcode_103_data[i][5]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_4_ADDR, romcode_103_data[i][6]);
        printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_4  0x%02x \n", romcode_103_data[i][6]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_5_ADDR, romcode_103_data[i][7]);
        printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_5  0x%02x \n", romcode_103_data[i][7]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_6_ADDR, romcode_103_data[i][8]);
        printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_6  0x%02x \n", romcode_103_data[i][8]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_7_ADDR, romcode_103_data[i][9]);
        printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_7  0x%02x \n", romcode_103_data[i][9]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_8_ADDR, romcode_103_data[i][10]);
        printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_8  0x%02x \n", romcode_103_data[i][10]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_9_ADDR, romcode_103_data[i][11]);
        printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_9  0x%02x \n", romcode_103_data[i][11]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_10_ADDR, romcode_103_data[i][12]);
        printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_10  0x%02x \n", romcode_103_data[i][12]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_11_ADDR, romcode_103_data[i][13]);
        printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_11  0x%02x \n", romcode_103_data[i][13]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_12_ADDR, romcode_103_data[i][14]);
        printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_12  0x%02x \n", romcode_103_data[i][14]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_13_ADDR, romcode_103_data[i][15]);
        printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_13  0x%02x \n", romcode_103_data[i][15]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_14_ADDR, romcode_103_data[i][16]);
        printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_14  0x%02x \n", romcode_103_data[i][16]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_15_ADDR, romcode_103_data[i][17]);
        printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_15  0x%02x \n", romcode_103_data[i][17]);
        rtl8291_setReg(devId, RTL8291_REG_INDACC_CONTROL_ADDR, romcode_103_data[i][18]);
        printk(KERN_EMERG "SET_DREG $REG_INDACC_CONTROL  0x%02x  ;# // set 16-byte write\n\n", romcode_103_data[i][18]);
        //Poll NCTRL busy bit
        cnt=0;
        while(cnt<1000) //check NCTRL BUSY
        {
            rtl8291_getRegBit(devId, RTL8291_REG_INDACC_CONTROL_ADDR, RTL8291_REG_INDACC_CONTROL_INDACC_CMD_OFFSET, &status);
            if(status == 0)
            {
                break;
            }
            cnt++;
        }
        rtl8291_getReg(devId, RTL8291_REG_INDACC_CONTROL_ADDR, &status);
        printk(KERN_EMERG "REG_INDACC_CONTROL = 0x%x Busy Cnt= %d \n\n", status, cnt);
        }
        else
        {
            rtl8291_setReg(devId, RTL8291_REG_INDACC_ADDR_LB_ADDR, romcode_103_data[i][0]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_ADDR_LB  0x%02x \n", romcode_103_data[i][0]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_ADDR_HB_ADDR, romcode_103_data[i][1]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_ADDR_HB  0x%02x \n", romcode_103_data[i][1]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_0_ADDR, romcode_103_data[i][2]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_0  0x%02x \n", romcode_103_data[i][2]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_1_ADDR, romcode_103_data[i][3]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_1  0x%02x \n", romcode_103_data[i][3]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_2_ADDR, romcode_103_data[i][4]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_2  0x%02x \n", romcode_103_data[i][4]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_3_ADDR, romcode_103_data[i][5]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_3  0x%02x \n", romcode_103_data[i][5]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_4_ADDR, romcode_103_data[i][6]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_4  0x%02x \n", romcode_103_data[i][6]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_5_ADDR, romcode_103_data[i][7]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_5  0x%02x \n", romcode_103_data[i][7]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_6_ADDR, romcode_103_data[i][8]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_6  0x%02x \n", romcode_103_data[i][8]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_7_ADDR, romcode_103_data[i][9]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_7  0x%02x \n", romcode_103_data[i][9]);
#if 0
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_8_ADDR, romcode_96_data[i][10]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_8  0x%02x \n", romcode_96_data[i][10]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_9_ADDR, romcode_96_data[i][11]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_9  0x%02x \n", romcode_96_data[i][11]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_10_ADDR, romcode_96_data[i][12]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_10  0x%02x \n", romcode_96_data[i][12]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_11_ADDR, romcode_96_data[i][13]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_11  0x%02x \n", romcode_96_data[i][13]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_12_ADDR, romcode_96_data[i][14]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_12  0x%02x \n", romcode_96_data[i][14]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_13_ADDR, romcode_96_data[i][15]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_13  0x%02x \n", romcode_96_data[i][15]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_14_ADDR, romcode_96_data[i][16]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_14  0x%02x \n", romcode_96_data[i][16]);
            rtl8291_setReg(devId, RTL8291_REG_INDACC_WDATA_15_ADDR, romcode_96_data[i][17]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_WDATA_15  0x%02x \n", romcode_96_data[i][17]);
#endif
            rtl8291_setReg(devId, RTL8291_REG_INDACC_CONTROL_ADDR, romcode_96_data[i][18]);
            printk(KERN_EMERG "SET_DREG $REG_INDACC_CONTROL  0x%02x  ;# // set 16-byte write\n\n", romcode_96_data[i][18]);
            //Poll NCTRL busy bit
            cnt=0;
            while(cnt<1000) //check NCTRL BUSY
            {
                rtl8291_getRegBit(devId, RTL8291_REG_INDACC_CONTROL_ADDR, RTL8291_REG_INDACC_CONTROL_INDACC_CMD_OFFSET, &status);
                if(status == 0)
                {
                    break;
                }
                cnt++;
            }
            rtl8291_getReg(devId, RTL8291_REG_INDACC_CONTROL_ADDR, &status);
            printk(KERN_EMERG "REG_INDACC_CONTROL = 0x%x Busy Cnt= %d \n\n", status, cnt);
        }
    }

    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P00_L_ADDR, 0x02);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P00_L  0x%x \n", 0x02);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P00_H_ADDR, 0x10);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P00_H  0x%x \n", 0x10);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P00_L_ADDR, 0x0);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P00_L  0x%x \n", 0x0);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P00_H_ADDR, 0xc);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P00_H  0x%x \n", 0xc);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P01_L_ADDR, 0x65);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P01_L  0x%x \n", 0x65);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P01_H_ADDR, 0x14);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P01_H  0x%x \n", 0x14);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P01_L_ADDR, 0x2);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P01_L  0x%x \n", 0x2);
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P01_H_ADDR, 0xc);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P01_H  0x%x \n", 0xc);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P02_L_ADDR, 0x80);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P02_L  0x%x \n", 0x80);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P02_H_ADDR, 0x14);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P02_H  0x%x \n", 0x14);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P02_L_ADDR, 0x4);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P02_L  0x%x \n", 0x4);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P02_H_ADDR, 0xc);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P02_H  0x%x \n", 0xc);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P03_L_ADDR, 0x65);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P03_L  0x%x \n", 0x65);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P03_H_ADDR, 0x12);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P03_H  0x%x \n", 0x12);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P03_L_ADDR, 0x7);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P03_L  0x%x \n", 0x7);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P03_H_ADDR, 0xc);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P03_H  0x%x \n", 0xc);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P04_L_ADDR, 0xa5);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P04_L  0x%x \n", 0xa5);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P04_H_ADDR, 0x12);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P04_H  0x%x \n", 0x12);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P04_L_ADDR, 0xbd);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P04_L  0x%x \n", 0xbd);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P04_H_ADDR, 0xc);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P04_H  0x%x \n", 0xc);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P05_L_ADDR, 0xba);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P05_L  0x%x \n", 0xba);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P05_H_ADDR, 0x12);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P05_H  0x%x \n", 0x12);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P05_L_ADDR, 0xe1);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P05_L  0x%x \n", 0xe1);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P05_H_ADDR, 0xc);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P05_H  0x%x \n", 0xc);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P06_L_ADDR, 0x0b);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P06_L  0x%x \n", 0x0b);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P06_H_ADDR, 0x13);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P06_H  0x%x \n", 0x13);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P06_L_ADDR, 0xec);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P06_L  0x%x \n", 0xec);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P06_H_ADDR, 0xc);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P06_H  0x%x \n", 0xc);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P07_L_ADDR, 0x13);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P07_L  0x%x \n", 0x13);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P07_H_ADDR, 0x13);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P07_H  0x%x \n", 0x13);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P07_L_ADDR, 0x10);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P07_L  0x%x \n", 0x10);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P07_H_ADDR, 0xd);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P07_H  0x%x \n", 0xd);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P08_L_ADDR, 0x60);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P08_L  0x%x \n", 0x60);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_ADDR_P08_H_ADDR, 0x13);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_ADDR_P08_H  0x%x \n", 0x13);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P08_L_ADDR, 0xa0);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P08_L  0x%x \n", 0xa0);    
    rtl8291_setReg(devId, RTL8291_CFG_NCTL_BP_TARG_P08_H_ADDR, 0xd);
    printk(KERN_EMERG "SET_DREG  $CFG_NCTL_BP_TARG_P08_H  0x%x \n", 0xd);

    // set bit-3 is 0
    rtl8291_setReg(devId, RTL8291_REG_IIC_S_ADDR, 0x15);
    printk(KERN_EMERG "SET_DREG $REG_IIC_S   0x15\n");     
    // set bit-2 is 0
    rtl8291_setReg(devId, RTL8291_REG_CONTROL1_ADDR, 0);
    printk(KERN_EMERG "SET_DREG $REG_CONTROL1  0\n");       

    return;
}

#endif




#endif

