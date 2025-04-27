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
 * Purpose : BIT operation utility
 *
 * Feature :
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/util/rt_bitop.h>

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
 *      rt_bitop_numberOfSetBits
 * Description:
 *      caculate how much bit is set in this word
 * Input:
 *      value       - the word to be handled
 * Output:
 *
 * Return:
 *      number bits are set
 * Note:
 *      None
 */
uint32 rt_bitop_numberOfSetBits(rtk_bitmap_t value)
{   /* Hamming Weight Algorithm */

    value = value - ((value >> 1) & 0x55555555);
    value = (value & 0x33333333) + ((value >> 2) & 0x33333333);
    return (((((value + (value >> 4)) & 0xF0F0F0F)) * 0x1010101) >> 24);
} /* end of rt_bitop_numberOfSetBits */

/* Function Name:
 *      bitop_numberOfSetBitsInArray
 * Description:
 *      caculate how much bit is set in this array
 * Input:
 *      pArray      - the array to be handled
 *      arraySize   - Size of Array
 * Output:
 *
 * Return:
 *      number bits are set
 * Note:
 *      None
 */
uint32 bitop_numberOfSetBitsInArray(rtk_bitmap_t *pArray, uint32 arraySize)
{
    uint32          index;
    rtk_bitmap_t    value;
    uint32          numOfBits;

    if (NULL == pArray)
    {/* if null pointer, return 0 bits */
        return 0;
    }

    numOfBits = 0;

    for (index = 0; index < arraySize; index++)
    {
        value = pArray[index];

        value = value - ((value >> 1) & 0x55555555);
        value = (value & 0x33333333) + ((value >> 2) & 0x33333333);
        numOfBits += (((((value + (value >> 4)) & 0xF0F0F0F)) * 0x1010101) >> 24);
    }

    return numOfBits;
} /* end of bitop_numberOfSetBitsInArray */


/* Function Name:
 *      rt_bitop_findFirstBit
 * Description:
 *      find first bits
 * Input:
 *      value       - the word to be handled
 * Output:
 *
 * Return:
 *          position of first bit
 *          if not bit is set, will return (-1)
 * Note:
 *      None
 */
int32 rt_bitop_findFirstBit(rtk_bitmap_t value)
{
    rtk_bitmap_t  temp;

    if (0 == value){
        return -1;
    }
    temp = (value) ^ (value - 1);
    return (rt_bitop_numberOfSetBits(temp) - 1);
} /* end of rt_bitop_findFirstBit */

/* Function Name:
 *      rt_bitop_findFirstBitInArray
 * Description:
 *      find first bits in Array
 * Input:
 *      pArray       - the array to be handled
 * Output:
 *
 * Return:
 *          position of first bit
 *          if not bit is set, will return (-1)
 * Note:
 *      None
 */
int32 rt_bitop_findFirstBitInAaray(rtk_bitmap_t *pArray, uint32 arraySize)
{
    rtk_bitmap_t    temp;
    uint32          index;

    if ((NULL == pArray)){
        return -1;
    }

    /* find first bit in Array */
    for (index = 0; index < arraySize; index++)
    {
        if (0 == pArray[index])
        { /* this word is 0, no need futher process */
            continue;
        }

        temp = (pArray[index]) ^ (pArray[index] - 1);

        return ((index * BITMAP_WIDTH) + (rt_bitop_numberOfSetBits(temp) - 1));
    }

    /* not found, return -1 */
    return -1;
} /* end of rt_bitop_findFirstBitInAaray */


/* Function Name:
 *      rt_bitop_findFirstZeroBitInAaray
 * Description:
 *      find first zero bit in Array
 * Input:
 *      pArray       - the array to be handled
 * Output:
 *
 * Return:
 *          position of first zero bit
 *          if not bit is set, will return (-1)
 * Note:
 *      None
 */
int32 rt_bitop_findFirstZeroBitInAaray(rtk_bitmap_t *pArray, uint32 arraySize)
{
    rtk_bitmap_t    temp, bmVal, bitmapAll=0xFFFFFFFF;
    uint32          index;

    if ((NULL == pArray)){
        return -1;
    }

    /* find first zero bit in Array */
    for (index = 0; index < arraySize; index++)
    {
        if (bitmapAll == pArray[index])
        { /* this word is all 1, no need futher process */
            continue;
        }

        bmVal = ~pArray[index];
        temp = (bmVal) ^ (bmVal - 1);

        return ((index * BITMAP_WIDTH) + (rt_bitop_numberOfSetBits(temp) - 1));
    }

    /* not found, return -1 */
    return -1;
} /* end of rt_bitop_findFirstZeroBitInAaray */

/* Function Name:
 *      rt_bitop_findLastBitInArray
 * Description:
 *      find last bits in Array
 * Input:
 *      pArray       - the array to be handled
 * Output:
 *
 * Return:
 *          position of last bit
 *          if not bit is set, will return (-1)
 * Note:
 *      None
 */
int32 rt_bitop_findLastBitInAaray(rtk_bitmap_t *pArray, uint32 arraySize)
{
    rtk_bitmap_t    temp;
    int32           index, i;

    if ((NULL == pArray)){
        return -1;
    }

    /* find last bit in Array */
    for (index = arraySize - 1; index >= 0; index--)
    {
        if (0 == pArray[index])
        { /* this word is 0, no need futher process */
            continue;
        }

        temp = (pArray[index]);
        for (i = (BITMAP_WIDTH-1); i >= 0; i--)
        {
            if (temp & (1 << i))
                return ((index * BITMAP_WIDTH) + i);
        }
    }

    /* not found, return -1 */
    return -1;
} /* end of rt_bitop_findLastBitInAaray */

/* Function Name:
 *      rt_bitop_findIdxBitInAaray
 * Description:
 *      find the idx bits in Array
 * Input:
 *      pArray       - the array to be handled
 * Output:
 *
 * Return:
 *          position of the idx bit
 *          if not bit is set, will return (-1)
 * Note:
 *      None
 */
int32 rt_bitop_findIdxBitInAaray(rtk_bitmap_t *pArray, uint32 idx, uint32 arraySize)
{
    rtk_bitmap_t    temp;
    uint32          index;
    uint32          i,j = 0;

    if ((NULL == pArray)){
        return -1;
    }

    /* find the idx bit in Array */
    for (index = 0; index < arraySize; index++)
    {
        if (0 == pArray[index])
        { /* this word is 0, no need futher process */
            continue;
        }

        temp = (pArray[index]);

        for(i = 0; i < BITMAP_WIDTH; i++)
        {
            if (temp & (1 << i))
            {
                if(idx == j)
                    return ((index * BITMAP_WIDTH) + i);
                else
                    j++;
            }
        }
    }

    /* not found, return -1 */
    return -1;
} /* end of rt_bitop_findFirstBitInAaray */
