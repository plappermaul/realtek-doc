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
 * Purpose : Define the chip utility in the SDK.
 *
 * Feature : SDK common utility (chip)
 *
 */
#ifndef __RT_UTIL_CHIP_H__
#define __RT_UTIL_CHIP_H__



/* define a index (from 0) for chip family, for defining array data structure for each chip family and can index by this */
typedef enum rt_chip_family_index_e
{
    RT_CHIP_FAMILY_INDEX_8380 = 0,      /* for RTL8380 family */
    RT_CHIP_FAMILY_INDEX_8390,          /* for RTL8390 family */
    RT_CHIP_FAMILY_INDEX_9300,          /* for RTL9300 family */
    RT_CHIP_FAMILY_INDEX_9310,          /* for RTL9310 family */
    RT_CHIP_FAMILY_INDEX_9310HRA,       /* for remote stacking 9310 via Hardware-Remote-Access */
    RT_CHIP_FAMILY_INDEX_RPC,           /* for remote stacking chip */
    RT_CHIP_FAMILY_INDEX_END,
}rt_chip_family_index_t;


/* Function Name:
 *      rt_chip_familyIndex_get
 * Description:
 *      Get rt_chip_family_index_t of this unit
 * Input:
 *      unit    - the unit ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - Successed
 *      RT_ERR_FAILED   - Failed
 * Note:
 *      None
 */
extern int32
rt_chip_familyIndex_get(uint32 unit, rt_chip_family_index_t *index);


#endif /* __RT_UTIL_CHIP_H__ */

