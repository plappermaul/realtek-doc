/*
 * Copyright (C) 2009-2015 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Define the utility macro and function in the SDK.
 *
 * Feature : SDK common utility (time)
 *
 */

/*
 * Include Files
 */
#include <common/rt_autoconf.h>
#include <common/error.h>
#include <osal/print.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/debug/rt_log.h>
#include <drv/tc/tc.h>
#include <hwp/hw_profile.h>
#include <osal/isr.h>
#include <common/util/rt_util_time.h>
#include <rtcore/rtcore.h>
#include <dev_config.h>

/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */
static rt_timeval_t rt_util_hpt_recorded_tv[RT_UTIL_TIME_RECORD_MAX];
static int rt_util_hpt_record_max=0;

#define RT_UTIL_HPT_TcDataInitValue     (0x17D7840)
#define RT_UTIL_HPT_TIMER_UNIT          40 /* unit in nanosecond */
#define RT_UTIL_HPT_US_TO_COUNTER       25 /* change microsecond to counter in data register, here it is (1000/RT_UTIL_HPT_TIMER_UNIT)=25 */
/*
 * Macro Definition
 */


/*
 * Function Declaration
 */

/* Function Name:
 *      rt_util_hpt_whichTC_get
 * Description:
 *      Assign which Timer/Counter to be debug one
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
void rt_util_hpt_whichTC_get(uint32 unit, uint32 *tc_id)
{
    *tc_id = RT_UTIL_TIME_TC_ID;
}



/* Function Name:
 *      rt_util_hpt_init
 * Description:
 *      Init the HPT (high precision time) module, now using TC
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *
 */
int32
rt_util_hpt_init(void)
{
    uint32 tc_id;

    RT_INIT_MSG("    TC util init (unit %u)\n", HWP_MY_UNIT_ID());

    rt_util_hpt_whichTC_get(HWP_MY_UNIT_ID(),&tc_id);

    drv_tc_mode_set(HWP_MY_UNIT_ID(), tc_id, TC_MODE_TIMER);


    /* set TC to 40ns resolution */
    if(HWP_9300_FAMILY_ID(HWP_MY_UNIT_ID()))
    {
        drv_tc_divFactor_set(HWP_MY_UNIT_ID(), tc_id, 7);
    }
    else
    {
        drv_tc_divFactor_set(HWP_MY_UNIT_ID(), tc_id, 8);
    }

    drv_tc_dataInitValue_set(HWP_MY_UNIT_ID(), tc_id, RT_UTIL_HPT_TcDataInitValue);

    drv_tc_intEnable_set(HWP_MY_UNIT_ID(), tc_id,ENABLED);
    drv_tc_enable_set(HWP_MY_UNIT_ID(), tc_id, ENABLED);

    return RT_ERR_OK;
}




/* Function Name:
 *      rt_util_hpt_get
 * Description:
 *      Get the current timer tick of TC
 * Input:
 *      None
 * Output:
 *      *tv     - the value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *
 */
int32
rt_util_hpt_get(rt_timeval_t *tv)
{
    uint32 value;
    uint32 tc_id;

    rt_util_hpt_whichTC_get(HWP_MY_UNIT_ID(),&tc_id);

    drv_tc_counterValue_get(HWP_MY_UNIT_ID(), tc_id, &value);
    tv->sec = rtk_uk_sharedMem->rt_tv1.sec;
    tv->nsec = (value*RT_UTIL_HPT_TIMER_UNIT);

    return RT_ERR_OK;
}


int32
rt_util_hpt_udelay_withSec(uint32 usec)
{
    rt_timeval_t t_init, t_now;
    int cnt=0;
    uint32 delta_us;

    rt_util_hpt_get(&t_init);

    while(1){
        rt_util_hpt_get(&t_now);
        cnt++;
        delta_us = (t_now.sec-t_init.sec)*1000000 + (t_now.nsec/1000)-(t_init.nsec/1000);

        if(delta_us >= usec)
        {
            break;
        }
    }

    return RT_ERR_OK;
}


int32
rt_util_hpt_udelay_underSec(uint32 usec)
{
    uint32 tc_id;
    uint32 delta_cnt, init_cnt, now_cnt;

    rt_util_hpt_whichTC_get(HWP_MY_UNIT_ID(),&tc_id);

    drv_tc_counterValue_get(HWP_MY_UNIT_ID(), tc_id, &init_cnt);

	while(1){
        drv_tc_counterValue_get(HWP_MY_UNIT_ID(), tc_id, &now_cnt);

        if (now_cnt >= init_cnt)
            delta_cnt = now_cnt - init_cnt;
        else
            delta_cnt = (RT_UTIL_HPT_TcDataInitValue - init_cnt) + now_cnt;

        if((delta_cnt*RT_UTIL_HPT_TIMER_UNIT) >= (usec*1000))
        {
            break;
        }
    }

    return RT_ERR_OK;
}



int32
rt_util_hpt_udelay(uint32 usec)
{
    if(usec < 1000000)
        rt_util_hpt_udelay_underSec(usec);
    else
        rt_util_hpt_udelay_withSec(usec);

    return RT_ERR_OK;
}


/* Function Name:
 *      rt_util_hpt_record
 * Description:
 *      Get the current timer tick of TC, and record it into a variable. Later it can be printed out by rt_util_hptRecord_print().
 *      Just call it with a interger as its first parameter. Like:
 *          int i=o;
 *          i=rt_util_hpt_record(i, __FUNCTION__,__LINE__);
 *          .....
 *          i=rt_util_hpt_record(i, __FUNCTION__,__LINE__);
 * Input:
 *      int i               - user should declare a integer for this parameter (init as 0), the function will return "next" i for next time calling
 *      func_name      - put __FUNCTION__
 *      line                - put __LINE__
 * Output:
 *      None
 * Return:
 *      next i
 * Note:
 *
 */
int
rt_util_hpt_record(int i, const char *func_name, int line)
{
    if(i>=RT_UTIL_TIME_RECORD_MAX)
        return RT_UTIL_TIME_RECORD_MAX;

    rt_util_hpt_get(&rt_util_hpt_recorded_tv[i]);
    osal_snprintf((char *)rt_util_hpt_recorded_tv[i].str,RT_UTIL_TIME_RECORD_STR_MAX,"%s(%d)",func_name,line);
    rt_util_hpt_record_max = i;
    return i+1;
}

/* Function Name:
 *      rt_util_hpt_print
 * Description:
 *      Print out rt_timeval_t contents, with user's string "str".
 * Input:
 *      usr_str            - any user's string which will prepend to the printing message.
 *      tv                  - the rt_timeval_t to print
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
void
rt_util_hpt_print(uint8 *usr_str,rt_timeval_t *tv)
{
    osal_printf("%s%s(%dsec,%dns)\n",tv->str,usr_str,tv->sec,tv->nsec);
}


/* Function Name:
 *      rt_util_hptRecord_print
 * Description:
 *      Print the contents that rt_util_hpt_record() recorded.
 * Input:
 *      usr_str            - any user's string which will insert to the printing message.
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
void
rt_util_hptRecord_print(char *usr_str)
{
    int i;
    for(i=0;i<=rt_util_hpt_record_max;i++)
    {
        osal_printf("%d.%09d sec <--%s %s\n",
                                              rt_util_hpt_recorded_tv[i].sec,
                                              rt_util_hpt_recorded_tv[i].nsec,
                                              rt_util_hpt_recorded_tv[i].str,
                                              usr_str);
        if(i!=0)
        {
            uint64 x1, x2, x3;

            x1= rt_util_hpt_recorded_tv[i-1].sec*1000000 + rt_util_hpt_recorded_tv[i-1].nsec/1000;
            x2= rt_util_hpt_recorded_tv[i].sec*1000000 + rt_util_hpt_recorded_tv[i].nsec/1000;
            x3 = x2-x1;
            osal_printf("  (%d us)\n",(uint32)x3);
        }
    }
    rt_util_hpt_record_max = 0;
}


