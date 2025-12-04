/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#include "platform_autoconf.h" 
#include "diag.h"
#include "hal_api.h"
#include "rtk_efuse_base.h"
//#include "rtk_hal_efuse.h"
#include "peripheral.h"

void efuse_rw_test(void)
{
        write_efuse_byte(0x12345678, 0x12345678, 0, 0);
        read_efuse_byte(0);
}


u8 write_efuse_byte(u32 data, u32 mask, u32 dataPort, u32 dataIdx)
{
        u32 cnt = 0;
        u32 cnt_max = 1000;

        REG32(REG_EFUSE_CONFIG) =  REG32(REG_EFUSE_CONFIG) | BIT_EFUSE_CONFIG_PWR_GATE_EN | BIT_EFUSE_CONFIG_BURN_CHECK;

        // suggest delay 1 us on spec.
        udelay(1);

        // clear all mask
        REG32(REG_EFUSE_WRITE_DATA_BIT_MASK_0) = 0;
        REG32(REG_EFUSE_WRITE_DATA_BIT_MASK_1) = 0;
        REG32(REG_EFUSE_WRITE_DATA_BIT_MASK_2) = 0;
        REG32(REG_EFUSE_WRITE_DATA_BIT_MASK_3) = 0;

        REG32(REG_EFUSE_WRITE_DATA_0 + dataIdx * 0x4) = data;
        REG32(REG_EFUSE_WRITE_DATA_BIT_MASK_0 + dataIdx * 0x4) = mask;

        REG32(REG_EFUSE_CMD) = (REG32(REG_EFUSE_CMD) & (~0x7)) | BIT_EFUSE_CMD_ADDR(dataPort) | BIT_EFUSE_RW_CTRL;

        mdelay(10);
        
        do {
                if ((REG32(REG_EFUSE_CMD) & BIT_EFUSE_CTRL_STATE) == BIT_EFUSE_CTRL_STATE) {
                        printf("%s(%d): 0x%x, cnt:0x%02x \n", __FUNCTION__, __LINE__, REG32(REG_EFUSE_CMD), cnt);
                        break;
                }
                cnt++;
                mdelay(10);
        } while(cnt < cnt_max);

        if (cnt >= cnt_max) {
                printf("Efuse write fail (timeout) !!!\n");
                return STATUS_EFUSE_TIMEOUT;
        } else {
                u32 resultFlag = REG32(REG_EFUSE_BURN_CHECK_FAIL_0) | REG32(REG_EFUSE_BURN_CHECK_FAIL_1) |
                REG32(REG_EFUSE_BURN_CHECK_FAIL_2) | REG32(REG_EFUSE_BURN_CHECK_FAIL_3);

                if (resultFlag == 0) {
                        printf("Efuse write data done. p[%d]_[%d] = 0x%x & 0x%x, 0x%02x\n", dataPort, dataIdx, data, mask, cnt);
                        return STATUS_EFUSE_SUCCESS;
                } else {
                        printf("Efuse write fail (0x%x) !!!\n", resultFlag);
                        return STATUS_EFUSE_FAIL;
                }
        }

}

u8 load_efuse_data_to_reg(void)
{
        u32 cnt = 0;
        u32 cnt_max = 5000;

        REG32(REG_EFUSE_CONFIG) = REG32(REG_EFUSE_CONFIG) & (~BIT_EFUSE_CONFIG_PWR_GATE_EN);
        REG32(REG_EFUSE_CMD) = REG32(REG_EFUSE_CMD) & (~BIT_EFUSE_RW_CTRL);

        udelay(1);
        
        do {
                if ((REG32(REG_EFUSE_CMD) & BIT_EFUSE_CTRL_STATE) == BIT_EFUSE_CTRL_STATE) {
                    break;
                }
                cnt++;
                udelay(1);
        } while(cnt < cnt_max);

        if (cnt < cnt_max) {
                printf("load efuse ok\n");
                return STATUS_EFUSE_SUCCESS;
        } else {
                printf("load efuse fail. 0x%x(0x%x) \n", REG_EFUSE_CMD, REG32(REG_EFUSE_CMD));
                return STATUS_EFUSE_FAIL;
        }

}

u32 read_efuse_byte(u32 idx)
{
        // efuse controller only support read 4 bytes operation. 
        u32 addr  = (REG_EFUSE_P0_0 + idx) & (~0x3);
        u32 value = REG32(addr);
        u8 retval = 0;

        switch(idx%4) {
                case 0:
                        retval = value & 0xFF;
                break;
                case 1:
                        retval = (value >> 8) & 0xFF;
                break;
                case 2:
                        retval = (value >> 16) & 0xFF;
                break;
                case 3:
                        retval = (value >> 24) & 0xFF;
                break;
                default:
                ;
        }

        return retval;

}

void dump_efuse_reg_data(void)
{
        u32 max_num = EFUSE_PORT_MAX*EFUSE_PORT_IDX_MAX;
        u32 i, reg;
        for(i = 0; i < EFUSE_PORT_MAX*EFUSE_PORT_IDX_MAX && i < max_num; i++) {
                reg = REG_EFUSE_P0_0 + i*4;
                printf("0x%x: 0x%x\n", reg, REG32(reg));
        }
}

static void efuse_irq_handler(void)
{
        printf("%s(%d)\n", __func__, __LINE__);
        printf("0x%x, 0x%x, 0x%x\n", REG32(REG_EFUSE_CONFIG), REG32(0xB8003000), REG32(0xB8003004));
        REG32(0xB8003004) = REG32(0xB8003004);
        REG32(0xB8000704) = REG32(0xB8000704);
        printf("%s(%d)\n", __func__, __LINE__);
        printf("0x%x, 0x%x, 0x%x\n", REG32(REG_EFUSE_CONFIG), REG32(0xB8003000), REG32(0xB8003004));
}

void efuse_isr_verification(void)
{
	u32             value32;
	IRQ_HANDLE      EfuseIrqHandle;
	IRQ_FUN         IrqFun;
        IRQn_Type       IrqNum;

        IrqFun = (IRQ_FUN)efuse_irq_handler;
        IrqNum = EFUSE_CTRL_IP_IRQ;

        printf("%s(%d)\n", __func__, __LINE__);
        printf("0x%x, 0x%x, 0x%x\n", REG32(REG_EFUSE_CONFIG), REG32(0xB8003000), REG32(0xB8003004));

	// register IRQ
	REG32(REG_EFUSE_CONFIG) = (REG32(REG_EFUSE_CONFIG)&(~BIT_EFUSE_CONFIG_INT_MASK)) | BIT_EFUSE_CONFIG_INT_STATUS;

        printf("0x%x, 0x%x, 0x%x\n", REG32(REG_EFUSE_CONFIG), REG32(0xB8003000), REG32(0xB8003004));

        
        EfuseIrqHandle.Data = (u32)NULL;
        EfuseIrqHandle.IrqNum = IrqNum;
        EfuseIrqHandle.IrqFun = IrqFun;
        EfuseIrqHandle.Priority = 0;

#if 1
        irq_install_handler(EfuseIrqHandle.IrqNum, EfuseIrqHandle.IrqFun, (u32)(EfuseIrqHandle.Data));
#else   
        InterruptRegister(&EfuseIrqHandle);
        InterruptEn(&EfuseIrqHandle);
#endif

        printf("%s(%d)\n", __func__, __LINE__);
        printf("0x%x, 0x%x, 0x%x\n", REG32(REG_EFUSE_CONFIG), REG32(0xB8003000), REG32(0xB8003004));

}


