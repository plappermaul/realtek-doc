/*
 * Copyright (C) 2009 Realtek Semiconductor Corp. 
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated, 
 * modified or distributed under the authorized license from Realtek. 
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER 
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED. 
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

#if defined(CONFIG_SDK_KERNEL_LINUX)
#include <linux/module.h>
#endif

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
uint32 rt_bitop_numberOfSetBits(uint32 value)
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
 *      value       - the word to be handled
 *      arraySize   - Size of Array
 * Output:
 * 
 * Return:
 *      number bits are set
 * Note:
 *      None
 */
uint32 bitop_numberOfSetBitsInArray(uint32 *pArray, uint32 arraySize)
{
    uint32  index;
    uint32  value;
    uint32  numOfBits;
    
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
        numOfBits =+ (((((value + (value >> 4)) & 0xF0F0F0F)) * 0x1010101) >> 24);
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
int32 rt_bitop_findFirstBit(uint32 value)
{
    uint32  temp;
    
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
int32 rt_bitop_findFirstBitInAaray(uint32 *pArray, uint32 arraySize)
{
    uint32  temp, index;
    
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
                           
        return ((index * 32) + (rt_bitop_numberOfSetBits(temp) - 1));
    }
    
    /* not found, return -1 */
    return -1;
} /* end of rt_bitop_findFirstBitInAaray */

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
int32 rt_bitop_findLastBitInAaray(uint32 *pArray, uint32 arraySize)
{
    uint32  temp;
    int32   index, i;
    
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
        for (i = 31; i >= 0; i--)
        {
            if (temp & (1 << i))
                return ((index * 32) + i);
        }
    }
    
    /* not found, return -1 */
    return -1;
} /* end of rt_bitop_findLastBitInAaray */

#if defined(CONFIG_SDK_KERNEL_LINUX)
EXPORT_SYMBOL(rt_bitop_numberOfSetBits);
EXPORT_SYMBOL(bitop_numberOfSetBitsInArray);
EXPORT_SYMBOL(rt_bitop_findFirstBit);
EXPORT_SYMBOL(rt_bitop_findFirstBitInAaray);
EXPORT_SYMBOL(rt_bitop_findLastBitInAaray);
#endif
