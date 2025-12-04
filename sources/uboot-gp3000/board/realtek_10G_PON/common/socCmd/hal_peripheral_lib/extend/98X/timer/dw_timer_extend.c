/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2014 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "common.h"
#include "dw_timer_extend.h"

//TIMER_ROM_TEXT_SECTION
BOOL
HalTimerIrqRegister98F(
    IN  VOID    *Data
)
{
    PTIMER_ADAPTER      pHalTimerAdap = (PTIMER_ADAPTER) Data;
    PIRQ_HANDLE         pTimerIrqHandle;
    IRQ_FUN             BackUpIrqFun = NULL;
    
    pTimerIrqHandle = &pHalTimerAdap->IrqHandle;

    if (pHalTimerAdap->TimerId > 7) {
        //DBG_8195A_DRIVER("Error: No Support Timer ID!!!!!!!\n");
        return _FALSE;
    }
    else {
		/*
        if (pHalTimerAdap->TimerId > 1) {

            pTimerIrqHandle->IrqNum = TIMER2_7_IRQ;

            BackUpIrqFun = (IRQ_FUN) pTimerIrqHandle->IrqFun;

            Timer2To7VectorTable[pHalTimerAdap->TimerId-2] = 
                (IRQ_FUN) pTimerIrqHandle->IrqFun;
            pTimerIrqHandle->IrqFun = (IRQ_FUN) HalTimerIrq2To7Handle;

        }
        else {

            pTimerIrqHandle->IrqNum = (pHalTimerAdap->TimerId ? TIMER1_IRQ : TIMER0_IRQ);

        }
        pTimerIrqHandle->Data = (u32)pHalTimerAdap;
        InterruptRegister(pTimerIrqHandle);

        if (BackUpIrqFun) {
            pTimerIrqHandle->IrqFun = BackUpIrqFun;
        }
		*/
		//GTIMER_IRQ is not declared, temporary marked
		pTimerIrqHandle->IrqNum = DW_apb_timers_top;
		//pTimerIrqHandle->IrqFun = (IRQ_FUN) HalTimerIrqHandle;
		pTimerIrqHandle->Data = (u32)pHalTimerAdap;
        InterruptRegister(pTimerIrqHandle);

        //arm
        //irq_install_handler(pTimerIrqHandle->IrqNum, pTimerIrqHandle->IrqFun, pTimerIrqHandle->Data);
		printf("register interrupt\n");
		
    }
    
    return _TRUE;
}

void 
HalWatchdogMode96F(IN void *Data)
{
	PWATCHDOG_ADAPTER pWatchdogAdapter = (PWATCHDOG_ADAPTER)Data;
	u32 Mode = pWatchdogAdapter->Mode;
	u32 OVSEL = pWatchdogAdapter->TimerValue;
	u32 value32;
	u32 counter,divide_factor, baseClock = 16*1024;
	
	if((OVSEL < TIMER_OVSEL_2_15) || (OVSEL > TIMER_OVSEL_2_24) )
	{
		printf("\nERROR: OVSEL should be 0~3!\n");
		return;
	}

	printf("\nBefore setting, WDTCNR_REG(b800311c)=%x\n", HAL_READ32_MEM(TIMER_CONTROL_REG_BASE + REG_TIMER_WDTCNR_REG));
	switch(Mode)
	{
		case WD_TEST_MODE_INIT:
			/* system clock = lx clock = 1/4 * cpu clock */
			divide_factor = (CONFIG_OCP_CLOCK_FREQ/4)/baseClock;
			
			HAL_WRITE32_MEM(TIMER_CONTROL_REG_BASE + REG_TIMER_CDBR_REG, BIT_CTRL_TIMER_DIV_FACTOR(divide_factor));
			HAL_WRITE32_MEM(TIMER_CONTROL_REG_BASE + REG_TIMER_WDTCNR_REG, (BIT_CTRL_TIMER_WDTE(TIMER_WDTE_DEFAULT) | BIT_CTRL_TIMER_WDTCLR(1) | BIT_CTRL_TIMER_OVSEL(OVSEL)) );

			printf("\nAfter setting, WDTCNR_REG(b8003118)=%x\n", HAL_READ32_MEM(TIMER_CONTROL_REG_BASE + REG_TIMER_CDBR_REG));
			printf("\nAfter setting, WDTCNR_REG(b800311c)=%x\n", HAL_READ32_MEM(TIMER_CONTROL_REG_BASE + REG_TIMER_WDTCNR_REG));
			break;
		case WD_TEST_MODE_START_AND_RESET:
			value32 = HAL_READ32_MEM(TIMER_CONTROL_REG_BASE + REG_TIMER_WDTCNR_REG );
			value32 = value32 & (~(BIT_CTRL_TIMER_WDTE(BIT_MASK_TIMER_WDTE)));
			HAL_WRITE32_MEM(TIMER_CONTROL_REG_BASE + REG_TIMER_WDTCNR_REG, value32 | BIT_CTRL_TIMER_OVSEL(OVSEL) );
			printf("\nAfter setting, WDTCNR_REG(b800311c)=%x\n", HAL_READ32_MEM(TIMER_CONTROL_REG_BASE + REG_TIMER_WDTCNR_REG));

			printf("Watchdog reset will be triggered after %d seconds!\n", (1<<(OVSEL+1)));
			counter = 0;
			while(1)
			{
				mdelay(250);
				counter++;
				printf("Counter: %d\n", counter);
			}
			break;
		case WD_TEST_MODE_START_AND_CLEAR_COUNTER:
			value32 = HAL_READ32_MEM(TIMER_CONTROL_REG_BASE + REG_TIMER_WDTCNR_REG );
			value32 = value32 & (~(BIT_CTRL_TIMER_WDTE(BIT_MASK_TIMER_WDTE)));
			HAL_WRITE32_MEM(TIMER_CONTROL_REG_BASE + REG_TIMER_WDTCNR_REG, value32 | BIT_CTRL_TIMER_OVSEL(OVSEL) );
			printf("\nAfter setting, WDTCNR_REG(b800311c)=%x\n", HAL_READ32_MEM(TIMER_CONTROL_REG_BASE + REG_TIMER_WDTCNR_REG));

			value32 = HAL_READ32_MEM(TIMER_CONTROL_REG_BASE + REG_TIMER_WDTCNR_REG );
			value32 = value32 | BIT_CTRL_TIMER_WDTCLR(1);
			counter = 0;
			while(1)
			{
				mdelay(250);
				counter++;
				printf("Counter: %d\n", counter);
				HAL_WRITE32_MEM(TIMER_CONTROL_REG_BASE + REG_TIMER_WDTCNR_REG, value32 | BIT_CTRL_TIMER_OVSEL(OVSEL) );
			}
			break;
		case WD_TEST_MODE_STOP:
			value32 = HAL_READ32_MEM(TIMER_CONTROL_REG_BASE + REG_TIMER_WDTCNR_REG );
			value32 = value32 | BIT_CTRL_TIMER_WDTE(TIMER_WDTE_DEFAULT);
			HAL_WRITE32_MEM(TIMER_CONTROL_REG_BASE + REG_TIMER_WDTCNR_REG, value32 | BIT_CTRL_TIMER_OVSEL(OVSEL) );
			
			printf("\nAfter setting, WDTCNR_REG(b800311c)=%x\n", HAL_READ32_MEM(TIMER_CONTROL_REG_BASE + REG_TIMER_WDTCNR_REG));
			break;
		default:
			printf("\nERROR: Mode should be 0~3!\n");
			break;
	}
	
	return;
}

#define REG32(reg)      (*(volatile unsigned int*)(reg))

VOID
CpuWakeUpIrqHandle
(
    IN  VOID        *Data
)
{
    printf("%s(%d): 0x%x, 0x%x \n", __FUNCTION__, __LINE__, REG32(SYSTEM_REG_BASE + REG_INT_PENDING_STATUS), REG32(SYSTEM_REG_BASE + REG_HW_STRAP));
    // clear interrupt BIT_INT_CPUWAKE in 0xB8000004[1]
    REG32(SYSTEM_REG_BASE + REG_INT_PENDING_STATUS) = REG32(SYSTEM_REG_BASE + REG_INT_PENDING_STATUS) & ~BIT_INT_CPUWAKE;
}

void 
HalChangeCpuFreqMode96F(IN void *Data)
{
    u32 counter = 0;
	PCHANGE_CPU_FREQ_ADAPTER pChangeCpuFreqAdapter = (PCHANGE_CPU_FREQ_ADAPTER)Data;
	u32 Mode = pChangeCpuFreqAdapter->Mode;

    IRQ_HANDLE cpuIrqHandle;
    cpuIrqHandle.Data = (u32) (Data);
    cpuIrqHandle.IrqNum = 0;//CPU_WAKE_IP_IRQ;
    cpuIrqHandle.IrqFun = (IRQ_FUN) CpuWakeUpIrqHandle;
    cpuIrqHandle.Priority = 0;

    // clear interrupt BIT_INT_CPUWAKE in 0xB8000004[1]
    REG32(SYSTEM_REG_BASE + REG_INT_PENDING_STATUS) = REG32(SYSTEM_REG_BASE + REG_INT_PENDING_STATUS) & ~BIT_INT_CPUWAKE;
    
    irq_install_handler(cpuIrqHandle.IrqNum, cpuIrqHandle.IrqFun, cpuIrqHandle.Data);

    if ((Mode & BIT0) == BIT0) {
        // Change Cpu Frequency...
        // REG32(0xb8000008)[18:15]: BIT_CK_CPU_FREQ_SEL
        // REG32(0xb8000008):  0x18203 => 0x10203
        REG32(SYSTEM_REG_BASE + REG_HW_STRAP) = 0x10203;
        
        printf("%s(%d): 0x%x, 0x%x \n", __FUNCTION__, __LINE__, REG32(SYSTEM_REG_BASE + REG_INT_PENDING_STATUS), REG32(SYSTEM_REG_BASE + REG_HW_STRAP));
        //__asm__("wait"); //this is mips
        printf("%s(%d): 0x%x, 0x%x \n", __FUNCTION__, __LINE__, REG32(SYSTEM_REG_BASE + REG_INT_PENDING_STATUS), REG32(SYSTEM_REG_BASE + REG_HW_STRAP));
    }

    if ((Mode & BIT1) == BIT1) {
        // set watchdog reset
        REG32(TIMER_CONTROL_REG_BASE + REG_TIMER_CDBR_REG) = 0x17d0000;
        REG32(TIMER_CONTROL_REG_BASE + REG_TIMER_WDTCNR_REG) = 0xa5a00000;
        REG32(TIMER_CONTROL_REG_BASE + REG_TIMER_WDTCNR_REG) = 0x200000;

        counter = 0;
        while(1) {
            mdelay(250);
            counter++;
            printf("Counter: %d\n", counter);
        }
    }
}


VOID
EnableTimerIP98F(
){


    //enable timer
    HAL_WRITE32(0xf43203ac, 0, 0xF805E47C);
    

}



