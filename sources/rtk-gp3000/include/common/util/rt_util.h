/*
 * Copyright (C) 2011 Realtek Semiconductor Corp. 
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated, 
 * modified or distributed under the authorized license from Realtek. 
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER 
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED. 
 * Purpose : Define the utility macro and function in the SDK.
 *
 * Feature : SDK common utility
 *
 */

#ifndef __RT_UTIL_H__
#define __RT_UTIL_H__


/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/util.h>
#include <common/util/rt_bitop.h>
#include <osal/lib.h>

/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */


/*
 * Macro Definition
 */
#define RTK_PORTMASK_TO_UINT_PORTMASK(pPortmask)                (pPortmask->bits[0])
#define RTK_PORTMASK_FROM_UINT_PORTMASK(pPortmask, pUintmask)   (pPortmask->bits[0] = *pUintmask)
#define RTK_PORTMASK_PORT_SET(portmask, port) \
do {\
    if ((port) <= RTK_MAX_PORT_ID) {BITMAP_SET((portmask).bits, (port));}\
} while (0);\

#define RTK_PORTMASK_PORT_CLEAR(portmask, port) \
do {\
    if ((port) <= RTK_MAX_PORT_ID) {BITMAP_CLEAR((portmask).bits, (port));}\
} while (0);\

#define RTK_PORTMASK_WORD_SET(portmask, word, value) \
    ((portmask).bits[(word)] = value)
#define RTK_PORTMASK_WORD_GET(portmask, word) \
    ((portmask).bits[(word)])

#define RTK_PORTMASK_IS_PORT_SET(portmask, port) \
    (((port) <= RTK_MAX_PORT_ID)?BITMAP_IS_SET((portmask).bits, (port)): 0)

#define RTK_LSTMASK_IS_MAMBER_SET(portmask, port, max) \
    (((port) <= max)?BITMAP_IS_SET((portmask).bits, (port)): 0)

#define RTK_PORTMASK_IS_PORT_CLEAR(portmask, port) \
    (((port) <= RTK_MAX_PORT_ID)?BITMAP_IS_CLEAR((portmask).bits, (port)): 0)

#define RTK_PORTMASK_ASSIGN(dstPortmask, srcPortmask) \
    BITMAP_ASSIGN((dstPortmask).bits, (srcPortmask).bits, RTK_TOTAL_NUM_OF_WORD_FOR_1BIT_PORT_LIST)
#define RTK_PORTMASK_RESET(portmask) \
    BITMAP_RESET((portmask).bits, RTK_TOTAL_NUM_OF_WORD_FOR_1BIT_PORT_LIST)
#define RTK_PORTMASK_COMPARE(dstPortmask, srcPortmask) \
    BITMAP_COMPARE((dstPortmask).bits, (srcPortmask).bits, RTK_TOTAL_NUM_OF_WORD_FOR_1BIT_PORT_LIST)

#define RTK_PORTMASK_GET_FIRST_PORT(portmask) \
    rt_bitop_findFirstBitInAaray((portmask).bits, RTK_TOTAL_NUM_OF_WORD_FOR_1BIT_PORT_LIST)
#define RTK_PORTMASK_GET_LAST_PORT(portmask) \
    rt_bitop_findLastBitInAaray((portmask).bits, RTK_TOTAL_NUM_OF_WORD_FOR_1BIT_PORT_LIST)
#define RTK_PORTMASK_GET_PORT_COUNT(portmask)\
    (bitop_numberOfSetBitsInArray((portmask).bits, RTK_TOTAL_NUM_OF_WORD_FOR_1BIT_PORT_LIST))

#define RTK_PORTMASK_OR(dstPortmask, srcPortmask) \
    BITMAP_OR((dstPortmask).bits, (srcPortmask).bits, RTK_TOTAL_NUM_OF_WORD_FOR_1BIT_PORT_LIST)
#define RTK_PORTMASK_AND(dstPortmask, srcPortmask) \
    BITMAP_AND((dstPortmask).bits, (srcPortmask).bits, RTK_TOTAL_NUM_OF_WORD_FOR_1BIT_PORT_LIST)
#define RTK_PORTMASK_XOR(dstPortmask, srcPortmask) \
    BITMAP_XOR((dstPortmask).bits, (srcPortmask).bits, RTK_TOTAL_NUM_OF_WORD_FOR_1BIT_PORT_LIST)

/*RT definition*/
#define RT_PORTMASK_PORT_CLEAR      RTK_PORTMASK_PORT_CLEAR
#define RT_PORTMASK_PORT_SET        RTK_PORTMASK_PORT_SET
#define RT_PORTMASK_RESET           RTK_PORTMASK_RESET
#define RT_PORTMASK_IS_PORT_SET     RTK_PORTMASK_IS_PORT_SET
#define RT_PORTMASK_IS_PORT_CLEAR   RTK_PORTMASK_IS_PORT_CLEAR
#define RT_PORTMASK_COMPARE         RTK_PORTMASK_COMPARE
/*RT definition*/

/*
 * Function Declaration
 */

/* Function Name:
 *      rt_util_macCmp
 * Description:
 *      Compare two mac address
 * Input:
 *      mac1    - mac address 1
 *      mac2    - mac address 2
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - two address is same
 *      RT_ERR_FAILED       - two address is different
 * Note:
 */
extern int32
rt_util_macCmp(const uint8 *mac1, const uint8 *mac2);
#ifndef CONFIG_SDK_KERNEL_LINUX
#include <rtk/ponmac.h>

extern double __log10_subfunction(double cz, int n);
extern double __log10(double z);
extern void _get_data_by_type(rtk_transceiver_parameter_type_t type, rtk_transceiver_data_t *pSrcData, rtk_transceiver_data_t *pDstData);
#endif
#endif /* __RT_UTIL_H__ */

