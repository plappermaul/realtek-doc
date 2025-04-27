/*
 * Copyright(c) Realtek Semiconductor Corporation, 2008
 * All rights reserved.
 *
 * $Revision: 74309 $
 * $Date: 2016-12-14 16:27:16 +0800 (Wed, 14 Dec 2016) $
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

#if defined(CONFIG_SDK_KERNEL_LINUX) && defined(__KERNEL__)
  #include <linux/version.h>
  #include <linux/random.h>    /* for Kernel Space */
#else
  #include <stdlib.h>           /* for User Space */
#endif


#if defined(CONFIG_SDK_KERNEL_LINUX) && defined(__KERNEL__)
    #if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,19))
      #define ut_rand     prandom_u32
    #else
      #define ut_rand     random32    /* for Kernel Space */
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

void UNITTEST_RANDOM_PORTMASK_ASSIGN(uint32 unit,int32 index, rtk_portmask_t *portmask);

#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL9300)
#define UNITTEST_PORTMASK_PORT_REMOVE(port, portmask)\
do{\
    RTK_PORTMASK_PORT_CLEAR(portmask, port); \
} while(0)

#define UNITTEST_PORTMASK_PORT_ADD(port, portmask)\
    do{\
        RTK_PORTMASK_PORT_SET(portmask, port); \
    } while(0)

#endif

#define UNITTEST_TEST_EXISTED_PORT_ASSIGN(upper, lower, unit, port)\
do {\
    if ((upper) >= (lower) + 2)\
    {\
        do {\
            port = (ut_rand() % ((upper) - (lower) + 1)) + lower;\
        } while (!HWP_PORT_EXIST(unit, port));\
    }\
} while (0)

#define UNITTEST_TEST_EXISTED_PORT_WO_CPU_ASSIGN(upper, lower, unit, port)\
        do {\
            if ((upper) >= (lower) + 2)\
            {\
                do {\
                    port = (ut_rand() % ((upper) - (lower) + 1)) + lower;\
                } while (!HWP_PORT_EXIST(unit, port)|| HWP_IS_CPU_PORT(unit, port));\
            }\
        } while (0)


#define UNITTEST_TEST_NEXT_EXISTED_PORT_ASSIGN(upper, lower, unit, cur_port, next_port)\
do {\
    if ((upper) >= (lower) + 2)\
    {\
        next_port = cur_port;\
        do {\
            next_port++;\
            if (next_port > upper)\
                next_port = lower;\
        } while (!HWP_PORT_EXIST(unit, next_port));\
    }\
} while (0)

#define UNITTEST_TEST_EXISTED_PORT_CHECK(unit, port, result)\
do {\
    if(HWP_PORT_EXIST(unit, port))\
        result = 1;\
    else\
        result = 0;\
} while (0)

#define UNITTEST_TEST_EXISTED_PORT_CHECK_WO_CPU(unit, port, result)\
do {\
    if(!HWP_IS_CPU_PORT(unit, port))\
    {\
        if(HWP_PORT_EXIST(unit, port))\
            result = 1;\
        else\
            result = 0;\
    }\
} while (0)

extern int32 sdktest_mode_get(int32 *pMode);
extern int32 sdktest_mode_set(int32 mode);

#endif  /*__UNIT_TEST_UTIL_H__*/

