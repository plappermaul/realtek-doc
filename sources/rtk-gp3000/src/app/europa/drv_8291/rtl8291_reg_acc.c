#include <linux/sched.h>   /* for schedule_timeout() */
#include <linux/kernel.h>  /* Needed for KERN_ALERT */
#include <linux/delay.h>

#include <rtk/i2c.h>
#if defined(CONFIG_COMMON_RT_API)
#include <rtk/rt/rt_i2c.h>
#endif

#include "include/common.h"

#include "include/rtl8291_reg_acc.h"

//Should be removed and change to RL6591C version in future
#include "include/rtl8290c_reg_definition.h"
#include "include/rtl8291_reg_definition.h"

extern uint8 I2C_PORT;

extern func_i2c_write _ldd_i2c_write;
extern func_i2c_read _ldd_i2c_read;

void _ldd_delay_loop(uint16 loopCnt)
{
    // FIXME  busy wait is not reliable on faster cpu (eg. 9602b)
    // FIXME  which is much faster than 9601b
#if 1
    uint16 i;
    for(i=0;i<loopCnt;i++);
#else
    mdelay( loopCnt>>4 );   // asume original 0x50 = 80 means 5us
#endif

}

void _ldd_delay(void)
{
    uint32 i;
    //set_current_state(TASK_UNINTERRUPTIBLE);  
    //schedule_timeout(HZ/100);
    for(i=0;i<1000000;i++);
    return;
}

int32 _rtl8291_setPage(uint32 devId, uint32 page)
{
    int ret=0;
    uint32 i2c_addr;

    if ((page > 0xFF)||(((page&0xF0) != 0x80)&&((page&0xF0) != 0xF0)&&(page != 0x2)))
    {
        printk(KERN_EMERG "_rtl8291_setPage page=0x%x, out of page range!!! \n", page);
        return FAILED;
    }
    //default devId is 0x51
    if ((devId&0x01)==0)
    {
        printk(KERN_EMERG "_rtl8291_setPage error devId = 0x%x \n", devId);
        return FAILED;
    }

    i2c_addr = RTL8291_PAGE_SELECT_ADDR - A2L_REG_BASE;
    ret=_ldd_i2c_write(I2C_PORT, devId, i2c_addr, page);
    
    return ret;
}

int32 _rtl8291_getPage(uint32 devId, uint32 *pPage)
{
    int ret=0;
    uint32 i2c_addr;

    //default devId is 0x51
    if ((devId&0x01)==0)
    {
        printk(KERN_EMERG "_rtl8291_getA2Reg error devId = 0x%x \n", devId);
        return ret;
    }

    i2c_addr = RTL8291_PAGE_SELECT_ADDR - A2L_REG_BASE;
    ret=_ldd_i2c_read(I2C_PORT, devId, i2c_addr, pPage);
    
    return ret;
}

int32 _rtl8291_setA2Reg(uint32 devId, uint32 addr, uint32 value)
{
    int ret=0;

    if ((addr > 0xFF)||(value > 0xFF))
    {
        printk(KERN_EMERG "_rtl8291_setA2Reg: addr = 0x%x value = 0x%x, out of range!!! \n", addr, value);
        return -1;
    }    
    //default devId is 0x51
    if ((devId&0x01)==0)
    {
        printk(KERN_EMERG "_rtl8291_setA2Reg error devId = 0x%x \n", devId);
        return -1;
    } 

    ret=_ldd_i2c_write(I2C_PORT, devId, addr, value);
    
    return ret;
}

int32 _rtl8291_getA2Reg(uint32 devId, uint32 addr, uint32 *pValue)
{
    int ret=0;

    if (addr > 0xFF)
    {
        printk(KERN_EMERG "_rtl8291_getA2Reg addr = 0x%x out of page range!!! \n", addr);
        return -1;
    }
    //default devId is 0x51
    if ((devId&0x01)==0)
    {
        printk(KERN_EMERG "_rtl8291_getA2Reg error devId = 0x%x \n", devId);
        return -1;
    }

    ret=_ldd_i2c_read(I2C_PORT, devId, addr, pValue);

    if (*pValue > 0xFF)
    {
        printk(KERN_EMERG "_rtl8291_getA2Reg error pValue = 0x%x \n", *pValue);
        return -1;
    }
    
    return ret;
}


int32 rtl8291_setReg(uint32 devId, uint32 address, uint32 value)
{
    int32 ret = 0;
    //uint32 eeprom_addr=0, i2c_addr = 0;
    //uint8 devId=0;
    //uint32 status;
    uint32 chId;
    uint32 page, baseAddr, i2c_addr;

    LDD_MUTEX_LOCK();

    if ((address >= 0)&&(address < 0x100)) 
    {
        //A0 registrer, default devId is 0x50
        chId = (devId<<4)&0xF0;
        i2c_addr = address;
        ret=_ldd_i2c_write(I2C_PORT, chId, i2c_addr, value); 
        LDD_MUTEX_UNLOCK(); 
		//printk(KERN_EMERG "rtl8291_setReg: addr = 0x%x, value = 0x%x\n", address, value);
        return ret;        
    }
    else
    {
        //A2 registrer, default devId is 0x51
        chId = ((devId<<4)&0xF0)|0x1;  

        if ((address >= 0x100)&&(address <= 0x17F)) 
        {
            //A2L
            page     = TABLE_A2PAGE0;
            baseAddr = A2L_REG_BASE;
            i2c_addr = address - baseAddr;
            ret =_rtl8291_setA2Reg(chId, i2c_addr, value);
            if (ret != 0)
            {
                printk(KERN_EMERG "rtl8291_setReg error: devId = 0x%x addr = 0x%x value = 0x%x\n", devId, address, value);
            }
            LDD_MUTEX_UNLOCK();
            //printk(KERN_EMERG "rtl8291_setReg: addr = 0x%x, value = 0x%x\n", address, value);
            return ret;                
        }
        else if ((address >= 0x180)&&(address <= 0x1FF)) 
        {
            //A2Page0
            page     = TABLE_A2PAGE0;
            baseAddr = A2_PAGE0_BASE;                
        }        
        else if ((address >= 0x200)&&(address <= 0x27F)) 
        {
            //A2Page1
            page     = TABLE_A2PAGE1;
            baseAddr = A2_PAGE1_BASE;
        }    
        else if ((address >= 0x280)&&(address < 0x300)) 
        {
            //WBREG00~WBREG7F
            page     = TABLE_WBREG00;
            baseAddr = WB_REG_BASE;
        }
        else if ((address>=0x380) && (address<=0x3FF)) 
        {
            //DIG
            page     = TABLE_DIGREG;
            baseAddr = DIG_REG_BASE;
        } 
        else if ((address>=0x400) && (address<=0x47F)) 
        {
            //LASER_LUT
            page     = TABLE_LASERLUT;
            baseAddr = LASERLUT_BASE;
        }
        else if ((address>=0x480) && (address<=0x4FF)) 
        {
            //APD_LUT
            page     = TABLE_APDLUT;
            baseAddr = APDLUT_BASE;
        }
        else if ((address>=0x500) && (address<=0x57F)) 
        {
            //Rsvd1_LUT
            page     = TABLE_RSVLUT1;
            baseAddr = RSVLUT1_BASE;
        }
        else if ((address>=0x580) && (address<=0x5FF)) 
        {
            //Rsvd2_LUT
            page     = TABLE_RSVLUT2;
            baseAddr = RSVLUT2_BASE;
    
        }
        else if ((address>=0x600) && (address<=0x67F)) 
        {
            //Ind_Acc
            page     = TABLE_INDACC;
            baseAddr = INDACC_BASE;      
        }
        else if ((address>=0x680) && (address<=0x6FF))
        {
            //Info
            page     = TABLE_INFO;
            baseAddr = INFO_BASE;        
        }    
        else if ((address>=0x700) && (address<=0x77F)) 
        {
            //DIG_EXT
            page     = TABLE_DIGEXTREG;
            baseAddr = DIGEX_REG_BASE;          
        }
        else if ((address>=0x780) && (address<=0x7FF)) 
        {
            //printk(KERN_EMERG "DIG_EXT2\n");             // 
            page     = TABLE_DIGEXT2REG;
            baseAddr = DIGEX2_REG_BASE;          
        }        
        else 
        {
            printk(KERN_EMERG "Invalid register address (should be 0~0x2FF, 0x380~0x67F, 0x700~0x77F)");
            LDD_MUTEX_UNLOCK();  
            return FAILED;
        }
    }

    //Use A2 register base to switch page and set register
    i2c_addr = address - baseAddr + COMMON_BASE;
    ret=_rtl8291_setPage(chId, page);
    if (ret != 0)
    {
        printk(KERN_EMERG "_rtl8291_setPage error: ret = 0x%x, devId = 0x%x addr = 0x%x value = 0x%x\n", ret, chId, page);
        LDD_MUTEX_UNLOCK();
        return ret;
    }
    ret = _rtl8291_setA2Reg(chId, i2c_addr, value);
    if (ret != 0)
    {
        printk(KERN_EMERG "rtl8291_setReg error: devId = 0x%x addr = 0x%x value = 0x%x\n", devId, address, value);
        LDD_MUTEX_UNLOCK();
        return ret;    
    }
    LDD_MUTEX_UNLOCK(); 
    
    //printk(KERN_EMERG "rtl8291_setReg: addr = 0x%x, value = 0x%x\n", address, value);   

    return ret;
}

int32 rtl8291_getReg(uint32 devId, uint32 address, uint32 *pValue)
{
    int32 ret = 0;
    //uint32 eeprom_addr=0, i2c_addr = 0;
    //uint8 devId=0;
    //uint32 status;
    uint32 chId;
    uint32 page, baseAddr, i2c_addr;

    LDD_MUTEX_LOCK();

    if ((address >= 0)&&(address < 0x100)) 
    {
        //printk(KERN_EMERG "A0 registrer, default devId is 0x50\n");   
        chId = (devId<<4)&0xF0;
        i2c_addr = address;
        ret=_ldd_i2c_read(I2C_PORT, chId, i2c_addr, pValue);  
        LDD_MUTEX_UNLOCK();
        //printk(KERN_EMERG "rtl8291_getReg: addr = 0x%x, pValue = 0x%x\n", address, *pValue); 
        return ret;        
    }
    else
    {
        //printk(KERN_EMERG "A2 registrer, default devId is 0x51, input devId = 0x%x\n", devId);   
        chId = ((devId<<4)&0xF0)|0x1;    

        if ((address >= 0x100)&&(address <= 0x17F)) 
        {
            //printk(KERN_EMERG "A2L\n");               
            page     = TABLE_A2PAGE0;
            baseAddr = A2L_REG_BASE;
            i2c_addr = address - baseAddr;
            ret = _rtl8291_getA2Reg(chId, i2c_addr, pValue);
            if (ret != 0)
            {
                printk(KERN_EMERG "rtl8291_getReg: devId = 0x%x addr = 0x%x, *pValue = 0x%x\n", devId, address, *pValue);
            }
            LDD_MUTEX_UNLOCK();
            //printk(KERN_EMERG "rtl8291_getReg: addr = 0x%x, pValue = 0x%x\n", address, *pValue); 
            return ret;                
        }
        else if ((address >= 0x180)&&(address <= 0x1FF)) 
        {
            //printk(KERN_EMERG "A2Page0\n");   
            page     = TABLE_A2PAGE0;
            baseAddr = A2_PAGE0_BASE;                
        }        
        else if ((address >= 0x200)&&(address <= 0x27F)) 
        {
            //printk(KERN_EMERG "A2Page1\n");
            page     = TABLE_A2PAGE1;
            baseAddr = A2_PAGE1_BASE;
        }    
        else if ((address >= 0x280)&&(address < 0x300)) 
        {
            //printk(KERN_EMERG "WBREG00~WBREG7F\n");            
            page     = TABLE_WBREG00;
            baseAddr = WB_REG_BASE;
        }
        else if ((address >= 0x300)&&(address < 0x380)) 
        {
            //printk(KERN_EMERG "RBREG00~RBREG7F\n");            
            page     = TABLE_RBREG00;
            baseAddr = RB_REG_BASE;
        }        
        else if ((address>=0x380) && (address<=0x3FF)) 
        {
            //printk(KERN_EMERG "DIG\n");            
            page     = TABLE_DIGREG;
            baseAddr = DIG_REG_BASE;
        } 
        else if ((address>=0x400) && (address<=0x47F)) 
        {
            //printk(KERN_EMERG "LASER_LUT\n");               
            page     = TABLE_LASERLUT;
            baseAddr = LASERLUT_BASE;
        }
        else if ((address>=0x480) && (address<=0x4FF)) 
        {
            //printk(KERN_EMERG "APD_LUT\n");             
            page     = TABLE_APDLUT;
            baseAddr = APDLUT_BASE;
        }
        else if ((address>=0x500) && (address<=0x57F)) 
        {
            //printk(KERN_EMERG "Rsvd1_LUT\n");
            page     = TABLE_RSVLUT1;
            baseAddr = RSVLUT1_BASE;
        }
        else if ((address>=0x580) && (address<=0x5FF)) 
        {
            //printk(KERN_EMERG "Rsvd2_LUT\n");
            page     = TABLE_RSVLUT2;
            baseAddr = RSVLUT2_BASE;
    
        }
        else if ((address>=0x600) && (address<=0x67F)) 
        {
            //printk(KERN_EMERG "Ind_Acc\n");
            page     = TABLE_INDACC;
            baseAddr = INDACC_BASE;      
        }
        else if ((address>=0x680) && (address<=0x6FF))
        {
            //printk(KERN_EMERG "Info\n");            
            page     = TABLE_INFO;
            baseAddr = INFO_BASE;        
        }    
        else if ((address>=0x700) && (address<=0x77F)) 
        {
            //printk(KERN_EMERG "DIG_EXT\n");             // 
            page     = TABLE_DIGEXTREG;
            baseAddr = DIGEX_REG_BASE;          
        }
        else if ((address>=0x780) && (address<=0x7FF)) 
        {
            //printk(KERN_EMERG "DIG_EXT2\n");             // 
            page     = TABLE_DIGEXT2REG;
            baseAddr = DIGEX2_REG_BASE;          
        }
        else 
        {
            printk(KERN_EMERG "Invalid register address (should be 0~0x2FF, 0x300~0x67F, 0x700~0x77F)");
            LDD_MUTEX_UNLOCK();
            return -1;
        }
    }
    //printk(KERN_EMERG "rtl8291_getReg: devId = 0x%x, chId = 0x%x\n", devId, chId);    
    //Use A2 register base to switch page and set register
    i2c_addr = address - baseAddr + COMMON_BASE;
    ret=_rtl8291_setPage(chId, page);
    if (ret != 0)
    {
        printk(KERN_EMERG "_rtl8291_setPage: chId = 0x%x page = 0x%x\n", chId, page);
        LDD_MUTEX_UNLOCK();
        return ret;
    } 
    ret = _rtl8291_getA2Reg(chId, i2c_addr, pValue);
    if (ret != 0)
    {
        printk(KERN_EMERG "rtl8291_getReg: devId = 0x%x addr = 0x%x, *pValue = 0x%x\n", devId, address, *pValue);
        LDD_MUTEX_UNLOCK();
        return ret;
    }
    LDD_MUTEX_UNLOCK();
    //printk(KERN_EMERG "rtl8291_getReg: addr = 0x%x, pValue = 0x%x\n", address, *pValue);   
   
    return ret;
}

int32 rtl8291_setRegBit(uint32 devId, uint32 reg, uint32 rBit, uint32 value)
{
    int32 ret = 0;

    uint32 regData=0;
    ret = rtl8291_getReg(devId, reg, &regData);
    if (ret!=0)
    {
        return ret;
    }

    if(value)
        regData = regData | (1 << rBit);
    else
        regData = regData & (~(1 << rBit));

    ret = rtl8291_setReg(devId,reg,regData);
    return ret;
}

int32 rtl8291_getRegBit(uint32 devId, uint32 reg, uint32 rBit, uint32 *pValue)
{
    int ret = 0;
    uint32 regData=0;

    //printk(KERN_EMERG "rtl8291_getRegBit: reg = 0x%x, rBit = 0x%x\n", reg, rBit);

    ret = rtl8291_getReg(devId, reg, &regData);

    *pValue = (regData & (0x1 << rBit)) >> rBit;

    //printk(KERN_EMERG "rtl8291_getRegBit: reg = 0x%x, rBit = 0x%x, value = 0x%x\n", reg, rBit, *pValue);
    return ret;
}

int32 rtl8291_setRegBits(uint32 devId, uint32 reg, uint32 rBits, uint32 value)
{
    int ret=0;
    uint32 regData=0;
    uint32 bitsShift;
    uint32 valueShifted;
    
    //printk(KERN_EMERG "rtl8291_setRegBits(1): reg = 0x%x, rBits = 0x%x, value = 0x%x\n", reg, rBits, value);   

    bitsShift = 0;
    while(!(rBits & (1 << bitsShift)))
    {
        bitsShift++;
    }

    valueShifted = value << bitsShift;

    //printk(KERN_EMERG "rtl8291_setRegBits(2): reg = 0x%x, valueShifted = 0x%x, bitsShift = 0x%x\n", reg, valueShifted, bitsShift);   

    ret = rtl8291_getReg(devId, reg, &regData);
    if (ret!=0)
    {
        return ret;
    }

    //printk(KERN_EMERG "rtl8291_setRegBits(3): reg = 0x%x, regData = 0x%x\n", reg, regData);   


    regData = regData & (~rBits);
    //printk(KERN_EMERG "rtl8291_setRegBits(4): regData = 0x%x, ~rBits = 0x%x\n", regData, ~rBits);     
    regData = regData | (valueShifted & rBits);
    //printk(KERN_EMERG "rtl8291_setRegBits(5): regData = 0x%x, rBits = 0x%x, valueShifted = 0x%x\n", regData, rBits, valueShifted); 

    ret = rtl8291_setReg(devId,reg,regData);
    
    return ret;
}


int32 rtl8291_getRegBits(uint32 devId, uint32 reg, uint32 rBits, uint32 *pValue)
{
    int ret=0;
    uint32 regData=0;
    uint32 bitsShift;

    bitsShift = 0;
    while(!(rBits & (1 << bitsShift)))
    {
        bitsShift++;
    }

    ret = rtl8291_getReg(devId, reg, &regData);

    *pValue = (regData & rBits) >> bitsShift;

    return ret;
}



