/*
 * Copyright(c) Realtek Semiconductor Corporation, 2008
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Definition of DAL test APIs in the SDK
 *
 * Feature : DAL test APIs
 *
 */

#ifndef __UNIT_TEST_UTIL_H__
#define __UNIT_TEST_UTIL_H__

#include <common/rt_type.h>
#include <common/rt_error.h>
#include <osal/print.h>
#include <hal/mac/mac_probe.h>


#if defined(CONFIG_SDK_KERNEL_LINUX) && defined(__KERNEL__) 
  #include <linux/random.h>    /* for Kernel Space */
#else
  #include <stdlib.h>           /* for User Space */
#endif


#if defined(CONFIG_SDK_KERNEL_LINUX) && defined(__KERNEL__) 
#if defined(CONFIG_KERNEL_2_6_30)
#define ut_rand     random32    /* for Kernel Space */
#else
#define ut_rand 	prandom_u32  /* for Kernel Space */
#endif
#else
#define ut_rand     rand        /* for User Space */
#endif

#define TEST_SCAN_MODE  1
#define TEST_NORMAL_MODE  0

extern int32 unit_test_mode;

#define IS_TEST_SCAN_MODE() ((unit_test_mode == TEST_SCAN_MODE) ? 1:0)

#define RT_TEST_IS_EQUAL_INT(fmt, v1, v2, args...) \
    do{if ((v1) != (v2)) { \
        osal_printf("\t%s(%u): %d(0x%08x), %d(0x%08x)\n\t" fmt "\n", __FUNCTION__, __LINE__, v1, v1,v2,v2, ## args); \
        return RT_ERR_FAILED; \
    }}while(0)


#define RT_TEST_IS_NOT_EQUAL_INT(fmt, v1, v2, args...) \
    do{if ((v1) == (v2)) { \
        osal_printf("\t%s(%u): %d(0x%08x), %d(0x%08x)\n\t" fmt "\n", __FUNCTION__, __LINE__, v1, v1,v2,v2, ## args); \
        return RT_ERR_FAILED; \
    }}while(0)

#define RT_TEST_IS_EQUAL_INT64(fmt, v1, v2, args...) \
    do{if ((v1) != (v2)) { \
        osal_printf("\t%s(%u): %lld(0x%16llx), %lld(0x%16llx)\n\t" fmt "\n", __FUNCTION__, __LINE__, v1, v1,v2,v2, ## args); \
        return RT_ERR_FAILED; \
    }}while(0)


#define RT_TEST_IS_NOT_EQUAL_INT64(fmt, v1, v2, args...) \
    do{if ((v1) == (v2)) { \
        osal_printf("\t%s(%u): %lld(0x%16llx), %lld(0x%16llx)\n\t" fmt "\n", __FUNCTION__, __LINE__, v1, v1,v2,v2, ## args); \
        return RT_ERR_FAILED; \
    }}while(0)



/* for continouse test */
#define ASSIGN_EXPECT_RESULT(var, lower_bound, upper_bound, result) \
        if (var < lower_bound || var > upper_bound)\
        {\
            result = 0;\
        }\
        else\
        {\
            result = 1;\
        }\

 /* less than the value */ 

#define L_VALUE(value, less_value)\
    ((((less_value) = (value)) - 1) < (value) ? (less_value)--: (value))

#define G_VALUE(value, great_value)\
    ((((great_value) = (value)) + 1) > (value) ? (great_value)++: (value))

/* for random test */
#define UNITTEST_MAX_TEST_VALUE     5
#define UNITTEST_TEST_VALUE_ASSIGN(upper, lower, value, result, last_index)\
do {\
    (last_index) = 0;\
    (value)[(last_index)] = (lower);\
    (result)[(last_index)] = 1;\
    if ((upper) != (lower))\
    {\
        (last_index)++;\
        (value)[(last_index)] = (upper);\
        (result)[(last_index)] = 1;\
    }\
    if ((upper) >= (lower) + 2)\
    {\
        (last_index)++;\
        (value)[(last_index)] = ((upper) - (lower) - 1);\
        (value)[(last_index)] = ut_rand()%(value)[(last_index)] + (lower) + 1;\
        ((result))[(last_index)] = 1;\
    }\
    (value)[(last_index) + 1] = (lower);\
    if (((value)[(last_index) + 1] - 1) < (value)[(last_index) + 1])\
    {\
        (last_index)++;\
        (value)[(last_index)] = (lower) - 1;\
        (result)[(last_index)] = 0;\
    }\
    (value)[(last_index) + 1] = (upper);\
    if (((value)[(last_index) + 1] + 1) > (value)[(last_index) + 1])\
    {\
        (last_index)++;\
        (value)[(last_index)] = (upper) + 1;\
        (result)[(last_index)] = 0;\
    }\
} while (0)\


extern uint32 unittest_util_chip_id_get(void);

#define UNITTEST_UTIL_CHIP_TYPE             (unittest_util_chip_id_get())


#endif  /*__UNIT_TEST_UTIL_H__*/

