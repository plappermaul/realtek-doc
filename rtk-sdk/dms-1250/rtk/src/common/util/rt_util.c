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

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/util/rt_util.h>
#include <common/rt_error.h>
#include <hwp/hw_profile.h>
#include <hal/chipdef/chipdef.h>
#include <hal/mac/mem.h>


/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */

/*
 * Macro Definition
 */

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
int32
rt_util_macCmp(const uint8 *mac1, const uint8 *mac2)
{
    if (osal_memcmp(mac1, mac2, ETHER_ADDR_LEN) == 0)
        return RT_ERR_OK;
    else
        return RT_ERR_FAILED;

    return RT_ERR_OK;
}

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
int32
rt_util_actListIndex_get(uint32 *pActList, uint32 actNum, uint32 *pIndex, uint32 action)
{
    uint32 idx;

    /* parameter check */
    RT_PARAM_CHK((NULL == pActList), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((0 == actNum), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pIndex), RT_ERR_NULL_POINTER);

    for (idx=0; idx<(actNum); idx++)
    {
        if (action == pActList[idx])
        {
            *pIndex = idx;

            return RT_ERR_OK;
        }
    }

    return RT_ERR_CHIP_NOT_SUPPORTED;
}

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
int32
rt_util_actListAction_get(uint32 *pActList, uint32 actNum, uint32 *pAction, uint32 value)
{
    /* parameter check */
    RT_PARAM_CHK((NULL == pActList), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((0 == actNum), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((value >= actNum), RT_ERR_CHIP_UNDEFINED_VALUE);

    *pAction = pActList[value];

    return RT_ERR_OK;
}

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
rtk_ip_addr_t
rt_util_ipLength2Mask_ret(uint32 length)
{
    rtk_ip_addr_t ip;

    if (length & ~(0x0000001FUL))
        return 0xFFFFFFFFUL;

    ip = (length)? (0xFFFFFFFFUL << (32 - length)) : 0;

    return ip;
}

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
rtk_ipv6_addr_t
rt_util_ip6Length2Mask_ret(uint32 length)
{
    rtk_ipv6_addr_t ip6;
    uint32 i = 0;

    osal_memset(&ip6, 0x00, sizeof(rtk_ipv6_addr_t));
    if (length > 128)   /* length must be in the range: 0~128 */
        length = 128;

    /* step by 1 byte */
    while ((length - i) > 7)
    {
        ip6.octet[(i/8)] = 0xFF;
        i += 8;
    }

    /* step by D-bit */
    if (i < length)
    {
        ip6.octet[(i/8)] = (0xFF << (8 - (length - i)));
    }

    return ip6;
}

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
uint32
rt_util_ipMask2Length_ret(rtk_ip_addr_t ip)
{
    uint32 length = 0;

    while ((length < 32) && ((ip << length) & 0x80000000))
    {
        length++;
    }

    return length;
}

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
uint32
rt_util_ip6Mask2Length_ret(rtk_ipv6_addr_t *pIp6)
{
    int32 byte_offset, bit_offset;
    uint32 length = 0;

    for (byte_offset=(IPV6_ADDR_LEN-1); byte_offset>=0; byte_offset--)
    {
        for (bit_offset=7; bit_offset>=0; bit_offset--)
        {
            if (pIp6->octet[byte_offset] & (0x1 << bit_offset))
            {
                length++;
            }
            else
            {
                break;
            }

        }
    }

    return length;
}

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
int32
rt_util_ipMaxMatchLength_ret(rtk_ip_addr_t ip1, rtk_ip_addr_t ip2, uint32 maxLen)
{
    int32   offset = 31, length = 0;

    /* maximum compare length is 32 */
    if (maxLen > 32)
        maxLen = 32;

    while ((length < maxLen) && \
           ((ip1 & (0x1 << offset)) == \
            (ip2 & (0x1 << offset))))
    {
        offset--;
        length++;
    }

    return length;
}

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
int32
rt_util_ipv6MaxMatchLength_ret(rtk_ipv6_addr_t *pIp1, rtk_ipv6_addr_t *pIp2, uint32 maxLen)
{
    int32   offset = 0, length = 0;

    /* maximum compare length is 128 (IPV6_ADDR_LEN * 8) */
    if (maxLen > (IPV6_ADDR_LEN * 8))
        maxLen = (IPV6_ADDR_LEN * 8);

    while ((length < maxLen) && \
           ((pIp1->octet[(offset >> 3)] & (0x1 << ( (((IPV6_ADDR_LEN * 8) - 1) - offset) & 0x7))) == \
            (pIp2->octet[(offset >> 3)] & (0x1 << ((((IPV6_ADDR_LEN * 8) - 1) - offset) & 0x7)))))
    {
        offset++;
        length++;
    }

    return length;
}

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
int32
rt_util_ipv6IsZero_ret(rtk_ipv6_addr_t *pIp6)
{
    int32 i;

    for (i=0; i<IPV6_ADDR_LEN; i++)
    {
        if (0 != pIp6->octet[i])
            return FALSE;
    }

    return TRUE;
}

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
int32
rt_util_ipv6IsAllOnes_ret(rtk_ipv6_addr_t *pIp6)
{
    int32 i;

    for (i=0; i<IPV6_ADDR_LEN; i++)
    {
        if (0xFF != pIp6->octet[i])
            return FALSE;
    }

    return TRUE;
}

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
int32
rt_util_ipv6Cmp(rtk_ipv6_addr_t *pIp1, rtk_ipv6_addr_t *pIp2)
{
    int32 i;

    for (i=0; i<IPV6_ADDR_LEN; i++)
    {
        if (pIp1->octet[i] != pIp2->octet[i])
            return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

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
uint32
rt_util_grayToBinary(uint32 gray_code)
{
    uint32  num = gray_code;

    num = num ^ (num >> 16);
    num = num ^ (num >> 8);
    num = num ^ (num >> 4);
    num = num ^ (num >> 2);
    num = num ^ (num >> 1);
    return num;
}

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
void
rt_util_upinkPort_mask2Reg(uint32 unit, rtk_portmask_t *pPortmask, uint32 *pRegVal)
{
    uint32  port, bit = 0;
    uint32  port_start,port_end;

    switch(HWP_CHIP_FAMILY_ID(unit))
    {
        case RTL9300_FAMILY_ID:
            port_start = RTL9300_UPLINK_PORT_START;
            port_end = RTL9300_UPLINK_PORT_END;
            break;

        case RTL9310_FAMILY_ID:
            port_start = RTL9310_UPLINK_PORT_START;
            port_end = RTL9310_UPLINK_PORT_END;
            break;
        default:
            return;
    }

    for (port = port_start; port <= port_end; port++, bit++)
    {
        if (RTK_PORTMASK_IS_PORT_SET((*pPortmask), port))
        {
            (*pRegVal) |= 0x1 << bit;
        }
        else
        {
            (*pRegVal) &= ~(0x1 << bit);
        }
    }/* end for */
}


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
void
rt_util_upinkPort_reg2Mask(uint32 unit, uint32 *pRegVal, rtk_portmask_t *pPortmask)
{
    uint32  port, bit = 0;
    uint32  port_start,port_end;

    RTK_PORTMASK_RESET(*pPortmask);

    switch(HWP_CHIP_FAMILY_ID(unit))
    {
        case RTL9300_FAMILY_ID:
            port_start = RTL9300_UPLINK_PORT_START;
            port_end = RTL9300_UPLINK_PORT_END;
            break;

        case RTL9310_FAMILY_ID:
            port_start = RTL9310_UPLINK_PORT_START;
            port_end = RTL9310_UPLINK_PORT_END;
            break;
        default:
            return;
    }

    for (port = port_start; port <= port_end; port++, bit++)
    {
        if ((*pRegVal) & (0x1 << bit))
        {
            RTK_PORTMASK_PORT_SET((*pPortmask), port);
        }
        else
        {
            RTK_PORTMASK_PORT_CLEAR((*pPortmask), port);
        }
    }/* end for */
}


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
void
rt_util_portmask_portReg2Portmask(uint32 *pRegVal, rtk_portmask_t *pPortmask, uint32 macIdStart, uint32 macIdEnd)
{
    uint32          port, bit = 0;

    for (port = macIdStart; port <= macIdEnd; port++, bit++)
    {
        if ((*pRegVal) & (0x1 << bit))
        {
            RTK_PORTMASK_PORT_SET((*pPortmask), port);
        }
        else
        {
            RTK_PORTMASK_PORT_CLEAR((*pPortmask), port);
        }
    }/* end for */
}


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
void
rt_util_portmask_portmask2PortReg(rtk_portmask_t *pPortmask, uint32 *pRegVal, uint32 macIdStart, uint32 macIdEnd)
{
    uint32          port, bit = 0;

    for (port = macIdStart; port <= macIdEnd; port++, bit++)
    {
        if (RTK_PORTMASK_IS_PORT_SET((*pPortmask), port))
        {
            (*pRegVal) |= 0x1 << bit;
        }
        else
        {
            (*pRegVal) &= ~(0x1 << bit);
        }
    }/* end for */
}


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
int32
rt_util_tblEntry2Field(uint32 unit, uint32 table, uint32 field, uint32 *pField, uint32 *pEntry)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID), RT_ERR_UNIT_ID);
    return table_field_get(unit, table, field, pField, pEntry);
}

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
int32
rt_util_field2TblEntry(uint32 unit, uint32 table, uint32 field, uint32 *pField, uint32 *pEntry)
{
   RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID), RT_ERR_UNIT_ID);
   return table_field_set(unit, table, field, pField, pEntry);
}




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
int32
rtk_portmask2bitPortmask_get(uint32 unit, rtk_portmask_t *portmask,
    uint8 *bitPortmask, uint32 byteSize)
{
    uint32  i, j;

    RT_PARAM_CHK(unit > RTK_MAX_UNIT_ID, RT_ERR_UNIT_ID);
    RT_PARAM_CHK((NULL == portmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == bitPortmask), RT_ERR_NULL_POINTER);

    for (i = 0; i < byteSize; i++)
    {
        j = byteSize - i - 1;
        #if defined(CONFIG_SDK_ENDIAN_LITTLE)
        bitPortmask[j] = (RTK_PORTMASK_WORD_GET((*portmask), i / 4) >> ((4 - (i % 4) - 1) * 8)) & 0xFF;
        #else
        bitPortmask[j] = (RTK_PORTMASK_WORD_GET((*portmask), i / 4) >> ((i % 4) * 8)) & 0xFF;
        #endif
    }

    return RT_ERR_OK;
}   /* end of rtk_portmask2bitPortmask_get */





