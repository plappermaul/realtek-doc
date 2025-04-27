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
 * Purpose : Definition the common utilities in the SDK.
 *
 * Feature : type definition
 *
 */

#ifndef __RT_UTIL_TIME_H__
#define __RT_UTIL_TIME_H__



/*
 * Symbol Definition
 */
#define RT_UTIL_TIME_RECORD_MAX     32
#define RT_UTIL_TIME_RECORD_STR_MAX 64

#define RT_UTIL_TIME_TC_ID          TC_ID1


typedef struct rt_timeval_s {
    uint8  str[RT_UTIL_TIME_RECORD_STR_MAX];
    uint32 sec;
    uint32 nsec;
} rt_timeval_t;

typedef struct rt_util_timeTcIsr_db_s
{
    uint32 tc_id;

}rt_util_timeTcIsr_db_t;

/*
 * Data Declaration
 */

/*
 * Macro Definition
 */

/* the caller must give a integer with zero before 1st call */


/*
 * Function Declaration
 */



int32 rt_util_hpt_init(void);
int32 rt_util_hpt_get(rt_timeval_t *tv);
int rt_util_hpt_record(int i, const char *func_name, int line);
void rt_util_hpt_print(uint8 *str,rt_timeval_t *tv);
void rt_util_hptRecord_print(char *str);
int32 rt_util_hpt_isrRegister(void);
void rt_util_hpt_whichTC_get(uint32 unit, uint32 *tc_id);
int32 rt_util_hpt_udelay(uint32 usec);



#endif /* __RT_UTIL_TIME_H__ */

