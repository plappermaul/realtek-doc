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
 * $Revision$
 * $Date$
 *
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
/* rtk_portmask_t print fromat: "[1]00000000 [0]00000000" */
#define RTK_PORTMASK_PRINT_STRING_LEN           ((8+4) * RTK_TOTAL_NUM_OF_WORD_FOR_1BIT_PORT_LIST)

/*
 * Data Type Declaration
 */
typedef struct rtk_valRangeStep_s
{
    uint32 start;
    uint32 end;
    uint32 step;
}rt_valRangeStep_t;


/*
 * Data Declaration
 */


/*
 * Macro Definition
 */

/* print portmask to buffer. strBuf: string buffer, size should be RTK_PORTMASK_PRINT_STRING_LEN */
#define RTK_PORTMASK_SPRINTF(strBuf, portmask)                                                      \
    do {                                                                                            \
        int32       _j, _len = 0;                                                                   \
        strBuf[0] = 0;                                                                              \
        for (_j = (RTK_TOTAL_NUM_OF_WORD_FOR_1BIT_PORT_LIST-1); _j >= 0; _j--)                      \
        {                                                                                           \
            _len += osal_sprintf(&strBuf[_len], "[%d]%08x", _j, (portmask).bits[_j]);               \
            if (_j != 0)                                                                            \
                _len += osal_sprintf(&strBuf[_len], " ");                                           \
        }                                                                                           \
    }while(0)

/* print portmask format and arguments */
#define PMSK_FMT            "0x%08x 0x%08x"
#define PMSK_ARG(_m)        (_m).bits[0], (_m).bits[1]

#define RTK_PORTMASK_PORT_SET(portmask, port) \
do {\
    if ((port) <= RTK_MAX_PORT_ID_PER_UNIT) {BITMAP_SET((portmask).bits, (port));}\
} while (0)

#define RTK_PORTMASK_PORT_CLEAR(portmask, port) \
do {\
    if ((port) <= RTK_MAX_PORT_ID_PER_UNIT) {BITMAP_CLEAR((portmask).bits, (port));}\
} while (0)

#define RTK_PORTMASK_WORD_SET(portmask, word, value) \
    do {                                                                                            \
        if (word < RTK_TOTAL_NUM_OF_WORD_FOR_1BIT_PORT_LIST)                                        \
            (portmask).bits[(word)] = value;                                                        \
        else                                                                                        \
            osal_printf("%s,%d:RTK_PORTMASK_WORD_SET %s word%d exceed maximum number\n", __FUNCTION__, __LINE__, #portmask,(word));  \
    }while(0)

#define RTK_PORTMASK_WORD_GET(portmask, word) \
    ((word < RTK_TOTAL_NUM_OF_WORD_FOR_1BIT_PORT_LIST)?(portmask).bits[(word)]:0x5E5E5E5E)

#define RTK_PORTMASK_IS_PORT_SET(portmask, port) \
    (((port) <= RTK_MAX_PORT_ID_PER_UNIT)?BITMAP_IS_SET((portmask).bits, (port)): 0)
#define RTK_PORTMASK_IS_PORT_CLEAR(portmask, port) \
    (((port) <= RTK_MAX_PORT_ID_PER_UNIT)?BITMAP_IS_CLEAR((portmask).bits, (port)): 0)

#define RTK_PORTMASK_SCAN(portmask, port)       \
    for (port = 0; port <= RTK_MAX_NUM_OF_PORTS; port++) \
        if(RTK_PORTMASK_IS_PORT_SET(portmask, port))

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
#define RTK_PORTMASK_GET_IDX_PORT(portmask,idx) \
    rt_bitop_findIdxBitInAaray((portmask).bits, idx,RTK_TOTAL_NUM_OF_WORD_FOR_1BIT_PORT_LIST)
#define RTK_PORTMASK_GET_PORT_COUNT(portmask)\
    (bitop_numberOfSetBitsInArray((portmask).bits, RTK_TOTAL_NUM_OF_WORD_FOR_1BIT_PORT_LIST))

#define RTK_PORTMASK_OR(dstPortmask, srcPortmask) \
    BITMAP_OR((dstPortmask).bits, (srcPortmask).bits, RTK_TOTAL_NUM_OF_WORD_FOR_1BIT_PORT_LIST)
#define RTK_PORTMASK_AND(dstPortmask, srcPortmask) \
    BITMAP_AND((dstPortmask).bits, (srcPortmask).bits, RTK_TOTAL_NUM_OF_WORD_FOR_1BIT_PORT_LIST)
#define RTK_PORTMASK_XOR(dstPortmask, srcPortmask) \
    BITMAP_XOR((dstPortmask).bits, (srcPortmask).bits, RTK_TOTAL_NUM_OF_WORD_FOR_1BIT_PORT_LIST)
#define RTK_PORTMASK_REMOVE(dstPortmask, srcPortmask) \
    BITMAP_REMOVE((dstPortmask).bits, (srcPortmask).bits, RTK_TOTAL_NUM_OF_WORD_FOR_1BIT_PORT_LIST)
#define RTK_PORTMASK_REVERT(portmask) \
    BITMAP_REVERT((portmask).bits, RTK_TOTAL_NUM_OF_WORD_FOR_1BIT_PORT_LIST)

#define RTK_PORTMASK_IS_ALL_ZERO(portmask)      \
    ((bitop_numberOfSetBitsInArray((portmask).bits, RTK_TOTAL_NUM_OF_WORD_FOR_1BIT_PORT_LIST) == 0) ? 1: 0)

#define RTK_PORTMASK_FOR_EACH_WORDS(arrIdx) \
    for (arrIdx = 0; arrIdx < RTK_TOTAL_NUM_OF_WORD_FOR_1BIT_PORT_LIST; arrIdx++)

#define RTK_DSCPMASK_IS_DSCP_SET(dscpmask, dscp) \
    (((dscp) <= RTK_VALUE_OF_DSCP_MAX)?BITMAP_IS_SET((dscpmask).bits, (dscp)): 0)


#define RT_UTIL_ACTLIST_INDEX_GET(_actArray, _value, _action) \
    rt_util_actListIndex_get(_actArray, (sizeof(_actArray)/sizeof(uint32)), &(_value), _action)

#define RT_UTIL_ACTLIST_ACTION_GET(_actArray, _action, _value) \
    rt_util_actListAction_get(_actArray, (sizeof(_actArray)/sizeof(uint32)), &(_action), _value)


#define RTK_TRUNKMASK_IS_TRUNK_SET(trunkmask, trunk) \
    (((trunk) < RTK_MAX_TRUNK_PER_UNIT)?BITMAP_IS_SET((trunkmask).bits, (trunk)): 0)



/* Serdes mask utility */
#define SDS_MASK_BIT            (31)        /* for indicating the sds_idx is using the bit-mask format */
#define SDS_BIT(s)              (1 << s)
#define SBM(n)                  (SDS_BIT(SDS_MASK_BIT) | SDS_BIT(n))    /* mark the uint32 as serdes bit-mask format (set bit 31 as 1) and set n (serdes index) to corresponding bit as 1 */
#define SDS_IS_MASK(sds)        (sds & SDS_BIT(SDS_MASK_BIT))           /* check "sds" is serdes bit-mask format or serdes-index format. */
#define SDS_IS_BIT_SET(bm, s)   (bm & SDS_BIT(s))                       /* check "s" is set in the serdes bit-mask "bm" or not. */

/* WAIT_COMPLETE macros are using together to perform an for-loop with timeout time.
 * Timeout unit is microsecond.
 * For time that larger than 4294967295 (0xFFFFFFFF), use seconds version macro.
 * Usage example of polling a register with 100 microsecond timeout time:
 * int32 example_func()
 * {
 *       WAIT_COMPLETE_VAR();
 *
 *       WAIT_COMPLETE(100) {
 *           reg_read(x);
 *           if(x=0x5A)
 *               break;
 *       }
 *
 *       if(WAIT_COMPLETE_IS_TIMEOUT())
 *           return ERROR;
 *       . . .
 * }
 */
#if defined(CONFIG_SDK_RTKSIM)
  #define WAIT_COMPLETE_VAR()

  #define WAIT_COMPLETE(_timeout_us) \
      while(0)

  #define WAIT_COMPLETE_IS_TIMEOUT()   (0)
#else
  #define WAIT_COMPLETE_VAR() \
      osal_usecs_t    _t, _now, _t_wait=0, _timeout;  \
      int32           _chkCnt=0;

  #define WAIT_COMPLETE(_timeout_us)     \
      _timeout = _timeout_us;  \
      for(osal_time_usecs_get(&_t),osal_time_usecs_get(&_now),_t_wait=0,_chkCnt=0 ; \
          (_t_wait <= _timeout); \
          osal_time_usecs_get(&_now), _chkCnt++, _t_wait += ((_now >= _t) ? (_now - _t) : (0xFFFFFFFF - _t + _now)),_t = _now \
         )

  #define WAIT_COMPLETE_IS_TIMEOUT()   (_t_wait > _timeout)
#endif


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

/* Function Name:
 *      rt_util_actListIndex_get
 * Description:
 *      Get the index (value) of the action (uint32) according to a specified action (uint32) array.
 * Input:
 *      pActList - pointer to the array of the actions
 *      actNum   - number of actions (size of the action array)
 *      pIndex   - pointer to the buffer address of the value
 *      action   - object of action
 * Output:
 *      pValue - pointer to the returned value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER       - parameter is a null pointer
 *      RT_ERR_INPUT              - invalid parameter
 *      RT_ERR_CHIP_NOT_SUPPORTED - chip not support this action (action is not found)
 * Note:
 */
extern int32
rt_util_actListIndex_get(uint32 *pActList, uint32 actNum, uint32 *pIndex, uint32 action);

/* Function Name:
 *      rt_util_actListAction_get
 * Description:
 *      Get the mapped action (uint32) of an index (value) according to a specified action (uint32) array.
 * Input:
 *      pActList - pointer to the array of the actions
 *      actNum   - number of actions (size of the action array)
 *      pAction  - pointer to the buffer address of the action
 *      value    - object of value
 * Output:
 *      pValue - pointer to the returned value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER         - parameter is a null pointer
 *      RT_ERR_INPUT                - invalid parameter
 *      RT_ERR_CHIP_UNDEFINED_VALUE - input value has not been defined
 * Note:
 */
extern int32
rt_util_actListAction_get(uint32 *pActList, uint32 actNum, uint32 *pAction, uint32 value);

/* Function Name:
 *      rt_util_ipLength2Mask_ret
 * Description:
 *      Convert an IPv4 prefix length to the corresponding mask
 * Input:
 *      length - prefix length of IPv4
 * Output:
 *      None
 * Return:
 *      IPv4 address mask of the type of rtk_ip_addr_t
 * Note:
 */
extern rtk_ip_addr_t
rt_util_ipLength2Mask_ret(uint32 length);

/* Function Name:
 *      rt_util_ip6Length2Mask_ret
 * Description:
 *      Convert an IPv6 prefix length to the corresponding mask
 * Input:
 *      ipLength - prefix length of IPv6
 * Output:
 *      None
 * Return:
 *      IPv6 address mask of the type of rtk_ipv6_addr_t
 * Note:
 */
extern rtk_ipv6_addr_t
rt_util_ip6Length2Mask_ret(uint32 length);

/* Function Name:
 *      rt_util_ipMask2Length_ret
 * Description:
 *      Return the length of an IPv4 mask
 * Input:
 *      ip - IPv4 address
 * Output:
 *      None
 * Return:
 *      length in uint32 type
 * Note:
 */
extern uint32
rt_util_ipMask2Length_ret(rtk_ip_addr_t ip);

/* Function Name:
 *      rt_util_ip6Mask2Length_ret
 * Description:
 *      Return the length of an IPv6 mask
 * Input:
 *      pIp6 - pointer to IPv6 address
 * Output:
 *      None
 * Return:
 *      length in uint32 type
 * Note:
 */
extern uint32
rt_util_ip6Mask2Length_ret(rtk_ipv6_addr_t *pIp6);

/* Function Name:
 *      rt_util_ipMaxMatchLength_ret
 * Description:
 *      Return the longest match length of two IPv4 addresses
 * Input:
 *      ip1    - IPv4 address 1
 *      ip2    - IPv4 address 2
 *      maxLen - maximum comparing length
 * Output:
 *      None
 * Return:
 *      longest match length in int32 type
 * Note:
 */
extern int32
rt_util_ipMaxMatchLength_ret(rtk_ip_addr_t ip1, rtk_ip_addr_t ip2, uint32 maxLen);

/* Function Name:
 *      rt_util_ipv6MaxMatchLength_ret
 * Description:
 *      Return the longest match length of two IPv6 addresses
 * Input:
 *      pIp1   - pointer to IPv6 address 1
 *      pIp2   - pointer to IPv6 address 2
 *      maxLen - maximum comparing length
 * Output:
 *      None
 * Return:
 *      longest match length in int32 type
 * Note:
 */
extern int32
rt_util_ipv6MaxMatchLength_ret(rtk_ipv6_addr_t *pIp1, rtk_ipv6_addr_t *pIp2, uint32 maxLen);

/* Function Name:
 *      rt_util_ipv6IsZero_ret
 * Description:
 *      Check if the IPv6 address is zero or not.
 * Input:
 *      pIp6 - pointer to an IPv6 address
 * Output:
 *      None
 * Return:
 *      boolean (TRUE/FALSE)
 * Note:
 */
extern int32
rt_util_ipv6IsZero_ret(rtk_ipv6_addr_t *pIp6);

/* Function Name:
 *      rt_util_ipv6IsAllOnes_ret
 * Description:
 *      Check if the IPv6 address is all-1s or not.
 * Input:
 *      pIp6 - pointer to an IPv6 address
 * Output:
 *      None
 * Return:
 *      boolean (TRUE/FALSE)
 * Note:
 */
extern int32
rt_util_ipv6IsAllOnes_ret(rtk_ipv6_addr_t *pIp6);

/* Function Name:
 *      rt_util_ipv6Cmp
 * Description:
 *      Compare two IPv6 addresses
 * Input:
 *      pIp1 - pointer to the 1st IPv6 address
 *      pIp2 - pointer to the 2nd IPv6 address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - same
 *      RT_ERR_FAILED - different
 * Note:
 */
extern int32
rt_util_ipv6Cmp(rtk_ipv6_addr_t *pIp1, rtk_ipv6_addr_t *pIp2);

/* Function Name:
 *      rt_util_grayToBinary
 * Description:
 *      Converting Gray code to binary for 32 or fewer bits
 * Input:
 *      gray_code   - gray code
 * Output:
 *      None
 * Return:
 *      Binary number
 * Note:
 *      None
 */
extern uint32
rt_util_grayToBinary(uint32 gray_code);

/* Function Name:
 *      rt_util_upinkPort_mask2Reg
 * Description:
 *      Mapping RTK portmast structure to register port bitmap for uplink ports
 * Input:
 *      unit      - unit ID
 *      pPortmask - Address pointer RTK portmask
 * Output:
 *      pRegVal - Address pointer of register value
 * Return:
 *      None
 * Note:
 */
extern void
rt_util_upinkPort_mask2Reg(uint32 unit, rtk_portmask_t *pPortmask, uint32 *pRegVal);


/* Function Name:
 *      rt_util_upinkPort_reg2Mask
 * Description:
 *      Mapping register port bitmap to RTK portmast structure for uplink ports
 * Input:
 *      unit    - unit ID
 *      pRegVal - Address pointer of register value
 * Output:
 *      pPortmask - Address pointer RTK portmask
 * Return:
 *      None
 * Note:
 */
extern void
rt_util_upinkPort_reg2Mask(uint32 unit, uint32 *pRegVal, rtk_portmask_t *pPortmask);


/* Function Name:
 *      rt_util_portmask_portReg2Portmask
 * Description:
 *      Mapping register port bitmap to RTK portmast structure.
 * Input:
 *      pRegVal - Address pointer of register value
 *      macIdStart - start mac ID of the register value mapping to.
 *      macIdEnd - end mac ID of the portmask that are going to map to register.
 * Output:
 *      pPortmask - Address pointer RTK portmask
 * Return:
 *      None
 * Note:
 */
extern void
rt_util_portmask_portReg2Portmask(uint32 *pRegVal, rtk_portmask_t *pPortmask, uint32 macIdStart, uint32 macIdEnd);

/* Function Name:
 *      rt_util_portmask_portmask2PortReg
 * Description:
 *      Mapping RTK portmast structure to register port bitmap.
 * Input:
 *      pPortmask - Address pointer RTK portmask
 *      macIdStart - start mac ID of the portmask that are going to map to register.
 *      macIdEnd - end mac ID of the portmask that are going to map to register.
 * Output:
 *      pRegVal - Address pointer of register value
 * Return:
 *      None
 * Note:
 */
extern void
rt_util_portmask_portmask2PortReg(rtk_portmask_t *pPortmask, uint32 *pRegVal, uint32 macIdStart, uint32 macIdEnd);

/* Function Name:
 *      rtk_portmask2bitPortmask_get
 * Description:
 *      Transfer rtk_portmask_t to SPM/DPM
 * Input:
 *      unit        - unit id
 *      portmask    - RTK portmask structure
 *      byteSize    - byte of bitPortmask
 * Output:
 *      bitPortmask - SPM/DPM format
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390, 8380, 9300, 9310
 * Note:
 *
 */

/* Function Name:
 *      rt_util_tblEntry2Field
 * Description:
 *      Get the specified field from a table entry
 * Input:
 *      unit  - unit id
 *      table - table id
 *      field - field index
 *      pEntry  - pointer to entry data
 * Output:
 *      pField - pointer of field data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rt_util_tblEntry2Field(uint32 unit,uint32 table,uint32 field,uint32 * pField,uint32 * pEntry);

/* Function Name:
 *      rt_util_field2TblEntry
 * Description:
 *      Set the specified field to a table entry
 * Input:
 *      unit  - unit id
 *      table - table id
 *      field - field index
 *      pField - pointer of field data
 *      pEntry  - pointer to entry data
 * Output:
 *      pEntry  - pointer to entry data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rt_util_field2TblEntry(uint32 unit,uint32 table,uint32 field,uint32 * pField,uint32 * pEntry);


extern int32
rtk_portmask2bitPortmask_get(uint32 unit, rtk_portmask_t *portmask,
    uint8 *bitPortmask, uint32 byteSize);
#endif /* __RT_UTIL_H__ */

