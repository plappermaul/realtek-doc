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
 * Purpose : If the RTOS (Real-time Operation System) is not supported those kind
 *           of library, you can use the library functions.
 *
 * Feature : 1)Library for OS independent part
 *
 */

#ifndef __OSAL_LIB_H__
#define __OSAL_LIB_H__


/*
 * Include Files
 */
#ifdef __BOOTLOADER__
  #include <common.h>
  #define osal_strncpy  strncpy
  #define osal_memset   memset
  #define osal_memcpy   memcpy
  #define osal_memcmp   memcmp
  #define osal_strcmp   strcmp
  #define osal_strncmp  strncmp
  #define osal_strlen   strlen

#elif defined(CONFIG_SDK_KERNEL_LINUX)
  #if defined(__KERNEL__)
    #include <linux/ctype.h>    /* for Kernel Space */
    #include <linux/kernel.h>
    #include <linux/string.h>
  #else
    #include <ctype.h>          /* for User Space */
    #include <string.h>
  #endif

  #define osal_strlen   strlen
  #define osal_strcmp   strcmp
  #define osal_strcpy   strcpy
  #define osal_strncpy  strncpy
  #define osal_strcat   strcat
  #define osal_strchr   strchr
  #define osal_memset   memset
  #define osal_memcpy   memcpy
  #define osal_memcmp   memcmp
  #define osal_strdup   strdup
  #define osal_strncmp  strncmp
  #define osal_strstr   strstr
  #define osal_strtok   strtok
  #define osal_strtok_r   strtok_r
  #define osal_toupper  toupper

#elif defined(CONFIG_SDK_OS_KERNEL_OTHER)

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
extern unsigned int osal_strlen(const char *pS);

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
extern int osal_strcmp(const char *pS1, const char *pS2);

/* Function Name:
 *      osal_strcpy
 * Description:
 *      Copy the string pointed to by pSrc to the array pointed to by pDst
 * Input:
 *      pDst - the pointer of the distination string.
 *      pSrc - the pointer of the source string.
 * Output:
 *      None
 * Return:
 *      NULL   - pSrc or pDst is NULL.
 *      Others - The pointer to the destination string.
 * Note:
 *      The strings may not overlap and the string pointed by pDst must
 *      large enough.
 */
extern char *osal_strcpy(char *pDst, const char *pSrc);
extern char *osal_strncpy(char *dest, const char *src, unsigned int n);

/* Function Name:
 *      osal_memset
 * Description:
 *      fill memory with a constant byte with value of val.
 * Input:
 *      pDst  - the pointer of the destination memory area.
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
extern void *osal_memset(void *pDst, int val, unsigned int len);

/* Function Name:
 *      osal_memcpy
 * Description:
 *      copy memory area
 * Input:
 *      pDst  - A pointer to the distination memory area.
 *      pSrc  - A pointer to the source memory area.
 *      len   - Number of bytes to be copied.
 * Output:
 *      None
 * Return:
 *      NULL   - pDst or pSrc is NULL.
 *      Others - A pointer to pDst.
 * Note:
 *      None
 */
extern void *osal_memcpy(void *pDst, const void *pSrc, unsigned int len);

/* Function Name:
 *      osal_memcmp
 * Description:
 *      Compare first len bytes of memory areas pMem1 and pMem2.
 * Input:
 *      pMem1 - The pointer points to one of the target memory areas.
 *      pMem2 - The pointer points to the other target memory area.
 *      len   - number of bytes to be compared.
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
extern int osal_memcmp(const void *pMem1, const void *pMem2, unsigned int len);


extern int osal_strncmp(const char *s1, const char *s2, unsigned int n);

/* Function Name:
 *      osal_strstr
 * Description:
 *      Locate substring
 * Input:
 *      pStr1 - the pointer of the str1.
 *      pStr2 - the pointer of the str2.
 * Output:
 *      None
 * Return:
 *      NULL   - str2 not present in str1.
 *      Others - The to the first occurrence of str2 in str1.
 * Note:
 *      None
 */
extern char *osal_strstr(char *pStr1, const char *pStr2);

/* Function Name:
 *      osal_strtok
 * Description:
 *      Split string into tokens
 * Input:
 *      pStr - the pointer of the string to truncate.
 *      pDelimiters - the pointer of the string containing the delimiter characters.
 * Output:
 *      None
 * Return:
 *      NULL   - the end of the string (i.e., a null character) is reached in the string being scanned.
 *      Others - a pointer to the beginning of the token.
 * Note:
 *      None
 */
extern char *osal_strtok(char *pStr, const char *pDelimiters);

/* Function Name:
 *      osal_strcat
 * Description:
 *      Concatenate strings
 * Input:
 *      pDest - the pointer to the destination array, which should contain a string, and be large enough to contain the concatenated resulting string.
 *      pSrc - the pointer of the string to be appended.
 * Output:
 *      None
 * Return:
 *      the pointer to the destination is returned.
 * Note:
 *      None
 */
extern char *osal_strcat(char *pDest, const char * pSrc);
extern char *osal_strtok_r(char *pStr, const char *pDelimiters, char **pSave);

/* Function Name:
 *      osal_toupper
 * Description:
 *      Convert lowercase letter to uppercase
 * Input:
 *      c - Character to be converted, casted to an int, or EOF.
 * Output:
 *      None
 * Return:
 *      The uppercase equivalent to c, if such value exists, or c (unchanged) otherwise.
 * Note:
 *      None
 */
extern int osal_toupper(int c);

#else

#endif


#endif /* __OSAL_LIB_H__ */
