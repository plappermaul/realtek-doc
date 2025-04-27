/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 73171 $
 * $Date: 2016-11-09 17:03:46 +0800 (Wed, 09 Nov 2016) $
 *
 * Purpose : Definition of OSAL test APIs in the SDK
 *
 * Feature : OSAL test APIs
 *
 */

/*
 * Include Files
 */
#include <osal/osal_test_case.h>
#include <private/drv/swcore/swcore.h>
#include <hwp/hw_profile.h>

#define VERI_ISR_TIMES    (8)

static volatile int global_run_times = 0;
static volatile int even_fail = 0;
uint32 g_family_id, g_chip_id, g_chip_rev_id;

osal_isrret_t isr_handler(uint32 unit, void *data){
    /* How much interrupt occurd */
    global_run_times++;
    SHOWMESSAGE("Hello: %d\n", global_run_times);

    /* Clear TC1 pending flag */
    if(HWP_8390_50_FAMILY(unit))
        REG32(RTL8390MP_TC1INT) |= RTL8390MP_TCIP;
    if(HWP_8380_30_FAMILY(unit))
        REG32(RTL8380MP_TC1INT) |= RTL8380MP_TCIP;


    return RT_ERR_OK;
}

void remove_interrupt_handler(uint32 unit)
{
    /* unregister isr */
    if(HWP_8390_50_FAMILY(unit))
    {
        REG32(RTL8390MP_TC1CTL) &= ~(RTL8390MP_TCEN | RTL8390MP_TCMODE_TIMER);
        REG32(RTL8390MP_TC1INT) &= ~RTL8390MP_TCIE;
    }
    if(HWP_8380_30_FAMILY(unit))
    {
        REG32(RTL8380MP_TC1CTL) &= ~(RTL8380MP_TCEN | RTL8380MP_TCMODE_TIMER);
        REG32(RTL8380MP_TC1INT) &= ~RTL8380MP_TCIE;
    }
        REG32(GIMR) &= ~(0x10000000);    /* TC1IE */
        osal_isr_unregister(RTK_DEV_TC1);
        SHOWMESSAGE("Unregister TC1 ISR\n");

}

int32 osal_isr_test(uint32 caseNo, uint32 unit)
{
    char name[32];
    volatile int checking;

    RT_LOG(LOG_FUNC_ENTER, MOD_UNITTEST, "[%s] Running ISR test cases", __FUNCTION__);

    global_run_times = 0;
    even_fail = 0;


    if(HWP_8390_50_FAMILY(unit))
    {
        /* Set trigger period */
        REG32(RTL8390MP_TC1CTL) = 0; /* disable timer before setting CDBR */
        REG32(RTL8390MP_TC1DATA) = (20000);
        REG32(RTL8390MP_TC1CTL) = RTL8390MP_TCEN | RTL8390MP_TCMODE_TIMER | DIVISOR_RTL8390 ;
        REG32(RTL8390MP_TC1INT) = RTL8390MP_TCIE;
    }

    if(HWP_8380_30_FAMILY(unit))
    {
        /* Set trigger period */
        REG32(RTL8380MP_TC1CTL) = 0; /* disable timer before setting CDBR */
        REG32(RTL8380MP_TC1DATA) = (20000);
        REG32(RTL8380MP_TC1CTL) = RTL8380MP_TCEN | RTL8380MP_TCMODE_TIMER | DIVISOR ;
        REG32(RTL8380MP_TC1INT) = RTL8380MP_TCIE;
    }

    /* Install ISR */
    osal_isr_register(RTK_DEV_TC1, isr_handler, name);
    REG32(GIMR) |= 0x10000000;    //TC1IE

    /* Wait 5 secs for increasing global_run_times */
    osal_time_sleep(5);
    if( 0==global_run_times ){
        even_fail++;
        goto isr_fail;
    }
    while( VERI_ISR_TIMES>global_run_times );

    /* Un-install ISR */
    remove_interrupt_handler(unit);

    checking = global_run_times;

    /* The ISR handler is unregisted, and the
     * global_run_times must be equal to VERI_ISR_TIMES
     */
    osal_time_sleep(5);
    if( checking != global_run_times ){
        even_fail++;
    }

isr_fail:
    if( 0==even_fail )
    {
        osal_printf("[OSAL ISR Test Pass]\n");
        return RT_ERR_OK;
    }
    else
    {
        osal_printf("[OSAL ISR Test Fail]\n");
        return RT_ERR_FAILED;
    }
}
