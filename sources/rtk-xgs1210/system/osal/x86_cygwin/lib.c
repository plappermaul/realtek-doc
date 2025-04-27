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
 * $Revision: 95661 $
 * $Date: 2019-03-11 09:59:41 +0800 (周一, 11 3月 2019) $
 *
 * Purpose : If the RTOS (Real-time Operation System) is not supported those kind
 *           of library, you can use the library functions.
 *
 * Feature : 1)Library for OS independent part
 *
 */

/*
 * Include Files
 */

#include <common/debug/rt_log.h>
#include <osal/lib.h>

/*
 * Function Declaration
 */
#if !defined(OS_LIB)
 /* Function Name:
  *      osal_strlen
  * Description:
  *      Count the length of a string, the terminating '\0' character isn't included.
  * Input:
  *      pS - The pointer of the given string.
  * Output:
  *      None
  * Return:
  *      The number of characters in the given string pS.
  * Note:
  *      return 0 when pS is NULL.
  */
 unsigned int
 osal_strlen(const char *pS)
 {
     unsigned int len;

     RT_INTERNAL_PARAM_CHK((NULL == pS), 0);

     len = 0;
     while (0 != *pS)
     {
         pS++;
         len++;
     }

     return len;
 } /* end of osal_strlen */

 /* Function Name:
  *      osal_strcmp
  * Description:
  *      compare pS1 string and pS2 string.
  * Input:
  *      pS1 - the pointer of the strings to be compared.
  *      pS2 - the pointer of the strings to be compared.
  * Output:
  *      None
  * Return:
  *      0 - pS1 and pS2 match.
  *      1 - pS1 and pS2 mismatch, pS1 is greater than pS2
  *     -1 - pS1 and pS2 mismatch, pS1 is less than pS2
  *     -2 - pS1 or pS2 is NULL
  * Note:
  *      None
  */
 int
 osal_strcmp(const char *pS1, const char *pS2)
 {
     RT_INTERNAL_PARAM_CHK((NULL == pS1) || (NULL == pS2), -2);

     do
     {
         if (*pS1 < *pS2)
         {
             /* pS1 and pS2 mismatch. */
             return -1;
         }
         else if (*pS1 > *pS2)
         {
             /* pS1 and pS2 mismatch. */
             return 1;
         }
         else
         {
             pS1++;
         }
     } while (*pS2++);

     /* pS1 and pS2 match. */
     return 0;
 } /* end of osal_strcmp */

 /* Function Name:
  *      osal_strcpy
  * Description:
  *      Copy the string pointed to by pSrc to the array pointed to by pDst
  * Input:
  *      pDst  - the pointer of the distination string.
  *      pSrc  - the pointer of the source string.
  * Output:
  *      None
  * Return:
  *      NULL   - pSrc or pDst is NULL.
  *      Others - The pointer to the destination string.
  * Note:
  *      The strings may not overlap and the string pointed by dst must
  *      large enough.
  */
 char *
 osal_strcpy(char *pDst, const char *pSrc)
 {
     RT_INTERNAL_PARAM_CHK((NULL == pDst) || (NULL == pSrc), NULL);

     while ('\0' != (*pSrc))
     {
         *pDst = *pSrc;
         pDst++;
         pSrc++;
     }
     *pDst = '\0';

     return pDst;
 } /* end of osal_strcpy */


 /* Function Name:
  *      osal_memset
  * Description:
  *      fill memory with a constant byte with value of val.
  * Input:
  *      pDst - the pointer of the destination memory area.
  *      val   - the constant byte to be set.
  *      len   - number of bytes to be set.
  * Output:
  *      None
  * Return:
  *      NULL   - pDst is NULL.
  *      Others - The pointer to the destination memory area.
  * Note:
  *      None
  */
 void *
 osal_memset(void *pDst, int val, unsigned int len)
 {
     unsigned char *pTem_dst;  /* used for cast pDst */

     RT_INTERNAL_PARAM_CHK((NULL == dst), NULL);

     pTem_dst = (unsigned char *)pDst;
     while (len)
     {
         *pTem_dst++ = (unsigned char)val;
         len--;
     }

     return (void *)pTem_dst;
 } /* end of osal_memset */


 /* Function Name:
  *      osal_memcpy
  * Description:
  *      copy memory area
  * Input:
  *      pDst  - A pointer to the distination memory area.
  *      pSrc  - A pointer to the source memory area.
  *      len    - Number of bytes to be copied.
  * Output:
  *      None
  * Return:
  *      NULL   - pDst or pSrc is NULL.
  *      Others - A pointer to pDst.
  * Note:
  *      None
  */
 void *
 osal_memcpy(void *pDst, const void *pSrc, unsigned int len)
 {
     unsigned char *pTem_dst;  /* used for cast pDst*/
     unsigned char *pTem_src;  /* used for cast pSrc*/

     RT_INTERNAL_PARAM_CHK((NULL == pDst) || (NULL == pSrc), NULL);

     pTem_dst = (unsigned char *)pDst;
     pTem_src = (unsigned char *)pSrc;

     while (0 < len)
     {
         *pTem_dst = *pTem_src;
         pTem_dst++;
         pTem_src++;
         len--;
     }

     return (void *)pTem_dst;
 } /* end of osal_memcpy */


 /* Function Name:
  *      osal_memcmp
  * Description:
  *      Compare first len bytes of memory areas pMem1 and pMem2.
  * Input:
  *      pMem1 - The pointer points to one of the target memory areas.
  *      pMem2 - The pointer points to the other target memory area.
  *      len    - number of bytes to be compared.
  * Output:
  *      None
  * Return:
  *      0 - pMem1 and pMem2 match
  *      1 - pMem1 and pMem2 mismatch, the first n bytes of pMem1 is greater than
  *          the first n bytes of pMem2, n = len
  *     -1 - pMem1 and pMem2 mismatch, the first n bytes of pMem1 is less than
  *          the first n bytes of pMem2, n = len
  *     -2 - pMem1 or pMem2 is NULL
  * Note:
  *      None
  */
 int
 osal_memcmp(const void *pMem1, const void *pMem2, unsigned int len)
 {
     unsigned char *pTem_mem1; /* used for cast pMem1 */
     unsigned char *pTem_mem2; /* used for cast pMem2 */

     RT_INTERNAL_PARAM_CHK((NULL == pMem1) || (NULL == pMem2), -2);

     pTem_mem1 = (unsigned char *)pMem1;
     pTem_mem2 = (unsigned char *)pMem2;

     while (0 != len)
     {
         if (*pTem_mem1 < *pTem_mem2)
         {
             /* pMem1 and pMem2 mismatch. */
             return -1;
         }
         else if (*pTem_mem1 > *pTem_mem2)
         {
             /* pMem1 and pMem2 mismatch. */
             return 1;
         }
         pTem_mem1++;
         pTem_mem2++;
         len--;
     }

     /* pMem1 and pMem2 match. */
     return 0;
 } /* end of osal_memcmp */
#endif

