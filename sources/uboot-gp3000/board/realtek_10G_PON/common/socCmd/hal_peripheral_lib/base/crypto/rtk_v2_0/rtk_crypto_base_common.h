#ifndef __RTK_CRYPTO_BASE_COMMON_H__
#define __RTK_CRYPTO_BASE_COMMON_H__

#include <stdio.h>
#include <malloc.h>
#include "common.h"
#include "basic_types.h"
#include "diag.h"
//#include "hal_irqn.h"
#include "hal_platform.h"
#include <asm/aarch64_sysreg_a55_tools.h>

/*--------------------Define --------------------------------------------*/
#ifdef __cplusplus
  #define   __I     volatile             /*!< Defines 'read only' permissions */
#else
  #define   __I     volatile const       /*!< Defines 'read only' permissions */
#endif
#define     __O     volatile             /*!< Defines 'write only' permissions */
#define     __IO    volatile             /*!< Defines 'read / write' permissions */

/* following defines should be used for structure members */
#define     __IM     volatile const      /*! Defines 'read only' structure member permissions */
#define     __OM     volatile            /*! Defines 'write only' structure member permissions */
#define     __IOM    volatile            /*! Defines 'read / write' structure member permissions */

//#define size_t uint32_t

#undef __printk
#define __printk DiagPrintf

// TODO: 
// will modify to see how to do assertion and print out in the future
// +++++++++++++++++++++++++++++++++++++++++

#undef rtl_assert
#define rtl_assert(x)

#define check_4byte_align(x)          ((u32)x & 0x3)

// #define __memDump	__rtl_memDump

#define ROM_ERR_LOG  1
#define ROM_WARN_LOG 0
#define ROM_INFO_LOG 0

#define SIM_ERR_LOG 1
#define SIM_DBG_LOG 0

#define VERIFY_ERR_LOG  1
#define VERIFY_DBG_LOG  0
#define VERIFY_INFO_LOG 1

//__printk
#undef DBG_CRYPTO_ERR
#define DBG_CRYPTO_ERR(fmt, args...) \
	     if (ROM_ERR_LOG) { \
	     	printf("%s(): " fmt " ", "rtl_crypto_err", ##args); \
	     }

#undef DBG_CRYPTO_WARN
#define DBG_CRYPTO_WARN(fmt, args...) \
	     if (ROM_WARN_LOG) { \
	     	printf("%s(): " fmt " ", "rtl_crypto_warn", ##args); \
	     }

#undef DBG_CRYPTO_INFO
#define DBG_CRYPTO_INFO(fmt, args...) \
	     if (ROM_INFO_LOG) { \
	     	printf("%s(): " fmt " ", "rtl_crypto_info", ##args); \
	     }


#undef __cryptoSim_err_printf
#define __cryptoSim_err_printf(fmt, args...) \
	     if (SIM_ERR_LOG) { \
	     	printf("%s(): " fmt " ", "rtl_cryptoSim_err", ##args); \
	     }

#undef __cryptoSim_dbg_printf
#define __cryptoSim_dbg_printf(fmt, args...) \
	     if (SIM_DBG_LOG) { \
	     	printf("%s(): " fmt " ", "rtl_cryptoSim_dbg", ##args); \
	     }

#undef __err_printf
#define __err_printf(fmt, args...) \
	     if (VERIFY_ERR_LOG) { \
	     	printf("%s(): " fmt " ", "err", ##args); \
	     }

#undef __dbg_printf
#define __dbg_printf(fmt, args...) \
	     if (VERIFY_DBG_LOG) { \
	     	printf("%s(): " fmt " ", "dbg", ##args); \
	     }

#undef __info_printf
#define __info_printf(fmt, args...) \
	     if (VERIFY_INFO_LOG) { \
	     	printf("%s(): " fmt " ", "info", ##args); \
	     }

#if 0
#define HAL_READ32(base, addr)            \
        rtk_le32_to_cpu(*((volatile u32*)(base + addr)))
    
#define HAL_WRITE32(base, addr, value32)  \
        ((*((volatile u32*)(base + addr))) = rtk_cpu_to_le32(value32))


#define HAL_READ16(base, addr)            \
        rtk_le16_to_cpu(*((volatile u16*)(base + addr)))
        
#define HAL_WRITE16(base, addr, value)  \
        ((*((volatile u16*)(base + addr))) = rtk_cpu_to_le16(value))
    

#define HAL_READ8(base, addr)            \
        (*((volatile u8*)(base + addr)))
            
#define HAL_WRITE8(base, addr, value)  \
        ((*((volatile u8*)(base + addr))) = value)
#endif

#if 0
#undef __dbg_mem_dump
#define __dbg_mem_dump(start, size, str_header) \
			if ( ConfigDebugInfo & _DBG_CRYPTO_ ) { \
		     	__printk("%s(): memdump : address: %08x, size: %d\n", "rtl_cryptoEngine_dbg", start, size); \
		    	__memDump((const u8*)(start), size, (char*)(str_header)); \
			}
#endif 

extern void dump_for_one_bytes(u8 *pdata, u32 len);

/**  \brief     __dbg_mem_dump is only used to print out memory information of CRYPTO IP. \n
 *              Use \em enable to enable/disable this function which dumps \em size bytes of momery information 
 *              from the \em start address.
 */ 
#undef __dbg_mem_dump
#define __dbg_mem_dump(enable, start, size, str_header) do{ \
    if(enable){ \
        printf(str_header "\r\n");\
        dump_for_one_bytes ((u8 *)start, size);}\
}while(0)

//
// Error index
//
#define _ERRNO_CRYPTO_DESC_NUM_SET_OutRange 		            -2
#define _ERRNO_CRYPTO_BURST_NUM_SET_OutRange		            -3
/// Pointer which points to NULL Error.
#define _ERRNO_CRYPTO_NULL_POINTER					            -4
/// Crypto doesn't initialize Error.
#define _ERRNO_CRYPTO_ENGINE_NOT_INIT				            -5
/// The starting address is not 4 byte-aligned Error.
#define _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned		            -6
/// Keylen out of range Error.
#define _ERRNO_CRYPTO_KEY_OutRange					            -7
/// Msglen out of range Error.
#define _ERRNO_CRYPTO_MSG_OutRange					            -8
/// Ivlen out of range Error.
#define _ERRNO_CRYPTO_IV_OutRange					            -9
/// AADlen out of range Error.
#define _ERRNO_CRYPTO_AAD_OutRange					            -10
#define _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH			            -11
#define _ERRNO_CRYPTO_CIPHER_TYPE_NOT_MATCH 		            -12
#define _ERRNO_CRYPTO_KEY_IV_LEN_DIFF				            -13
/// AES Msglen is not 16 byte-aligned Error.
#define _ERRNO_CRYPTO_AES_MSGLEN_NOT_16Byte_Aligned	            -14
/// Chahca Msglen is not 16 byte-aligned Error.
#define _ERRNO_CRYPTO_CHACHA_MSGLEN_NOT_16Byte_Aligned	        -15
/// DES Msglen is not 8 byte-aligned Error.
#define _ERRNO_CRYPTO_DES_MSGLEN_NOT_8Byte_Aligned	            -16
#define _ERRNO_CRYPTO_HASH_FINAL_NO_UPDATE		                -17
#define _ERRNO_CRYPTO_HASH_SEQUENTIAL_HASH_WORNG_LENGTH         -18
#define _ERRNO_CRYPTO_CACHE_HANDLE			                    -19
#define _ERRNO_CRYPTO_CIPHER_DECRYPT_MSGLEN_NOT_8Byte_Aligned	-20
#define _ERRNO_CRYPTO_MIX_MODE_HASH_PAD_NULL_POINTER            -21
#define _ERRNO_CRYPTO_MIX_MODE_TAG_NULL_POINTER					-22
#define _ERRNO_CRYPTO_MIX_MODE_ENC_PAD_NULL_POINTER             -23
#define _ERRNO_CRYPTO_KEY_STORAGE_WRONG_NUMBER                  -24
#define _ERRNO_CRYPTO_KEY_STORAGE_WRONG_LOCK                    -25
#define _ERRNO_CRYPTO_KEY_STORAGE_WRITE_LOCKED                  -26
#define _ERRNO_CRYPTO_KEYPAD_STORAGE_WRONG_NUMBER               -27
#define _ERRNO_CRYPTO_KEYPAD_STORAGE_WRONG_LOCK                 -28
#define _ERRNO_CRYPTO_KEYPAD_STORAGE_WRITE_LOCKED               -29

/*--------------------Define Enum---------------------------------------*/

/*--------------------Define MACRO--------------------------------------*/

/*--------------------Define Struct---------------------------------------*/

/*--------------------Export global variable-------------------------------*/

/*--------------------Function declaration---------------------------------*/

void *rtlc_memset(void *dst, int c, size_t n);
void *rtlc_memcpy(void *s1, const void *s2, size_t n);
int rtlc_memcmp(const void *s1, const void *s2, size_t n);

void rtlc_srandom(uint32_t seed);
uint32_t rtlc_random(void);
//void dump_for_one_bytes(u8 *pdata, u32 len);
//void dump_for_one_words(u8 *src, u32 len);

#endif  //__RTK_CRYPTO_BASE_COMMON_H__