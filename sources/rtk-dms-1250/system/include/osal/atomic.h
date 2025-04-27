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
 * $Revision: 71708 $
 * $Date: 2016-09-19 11:31:17 +0800 (Mon, 19 Sep 2016) $
 *
 * Purpose : Definition those APIs interface for separating OS depend system call.
 *           Let the RTK SDK call the layer and become OS independent SDK package.
 *
 * Feature : Atomic relative API
 *
 */

#ifndef __OSAL_ATOMIC_H__
#define __OSAL_ATOMIC_H__

/*
 * Include Files
 */
#include <common/type.h>


/*
 * Symbol Definition
 */
#if defined(__KERNEL__)
  #include <linux/sched.h>
  #define osal_atomic_t  atomic_t
#else
  typedef int osal_atomic_t;
#endif

#if defined(CONFIG_SDK_KERNEL_LINUX)
#define OSAL_ATOMIC_INIT(value)     ATOMIC_INIT(value)
#else /* CONFIG_SDK_OS_KERNEL_OTHER */
#define OSAL_ATOMIC_INIT(value)     (value)
#endif


/*
 * Function Declaration
 */

/* Function Name:
 *      osal_atomic_inc
 * Description:
 *      Increase the atomic counter.
 * Input:
 *      None
 * Output:
 *      pAtomic - pointer buffer of atomic counter.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern int32
osal_atomic_inc(osal_atomic_t *pAtomic);

/* Function Name:
 *      osal_atomic_add_return
 * Description:
 *      Atomically increments atomic counter by specified value and returns the result.
 * Input:
 *      val     - the amount to increase.
 *      pAtomic - pointer buffer of atomic counter.
 * Output:
 *      pAtomic - pointer buffer of atomic counter.
 * Return:
 *      returns the result of atomic counter
 * Note:
 *      None
 */
extern int32
osal_atomic_add_return(int32 val, osal_atomic_t *pAtomic);

/* Function Name:
 *      osal_atomic_set
 * Description:
 *      Set the atomic counter.
 * Input:
 *      counter - atomic counter to be configured
 * Output:
 *      pAtomic - pointer buffer of atomic counter.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern int32
osal_atomic_set(osal_atomic_t *pAtomic, int32 counter);

/* Function Name:
 *      osal_atomic_dec_return
 * Description:
 *      Atomically decreases atomic counter by 1 and returns the result.
 * Input:
 *      pAtomic - pointer buffer of atomic counter.
 * Output:
 *      pAtomic - pointer buffer of atomic counter.
 * Return:
 *      returns the result of atomic counter
 * Note:
 *      None
 */
extern int32
osal_atomic_dec_return(osal_atomic_t *pAtomic);

/* Function Name:
 *      osal_atomic_sub_return
 * Description:
 *      Atomically decreases atomic counter by specified value and returns the result.
 * Input:
 *      val     - the value to decrease.
 *      pAtomic - pointer buffer of atomic counter.
 * Output:
 *      pAtomic - pointer buffer of atomic counter.
 * Return:
 *      returns the result of atomic counter
 * Note:
 *      None
 */
extern int32
osal_atomic_sub_return(int32 val, osal_atomic_t *pAtomic);

/* Function Name:
 *      osal_atomic_read
 * Description:
 *      Atomically reads the atomic counter.
 * Input:
 *      pAtomic - pointer buffer of atomic counter.
 * Output:
 *      None
 * Return:
 *      returns the read value of atomic counter
 * Note:
 *      None
 */
extern int32
osal_atomic_read(osal_atomic_t *pAtomic);

extern int64
osal_atomic_test_and_set(osal_atomic_t *pAtomic, int32 counter);
#endif /* __OSAL_ATOMIC_H__ */
