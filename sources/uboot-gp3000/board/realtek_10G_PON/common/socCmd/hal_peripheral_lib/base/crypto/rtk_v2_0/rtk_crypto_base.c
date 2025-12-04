#include "rtk_crypto_base_common.h"
//#include "rtk_crypto_base_hal.h"
#include "rtk_crypto_base.h"
#include "rtk_crypto_base_type.h"
#include "rtk_crypto_base_ctrl.h"

/**
 * @addtogroup hs_hal_crypto CRYPTO
 * @{
 */

/// @cond DOXYGEN_ROM_HAL_API

#if defined(CONFIG_BUILD_NONSECURE) && (CONFIG_BUILD_NONSECURE == 1)
#undef CRYPTO_MODULE
#define CRYPTO_MODULE (CRYPTO_NS_MODULE)

#undef CRYPTO_REG_BASE
#define CRYPTO_REG_BASE (CRYPTO_NS_BASE)

#else

#undef CRYPTO_MODULE
#define CRYPTO_MODULE (CRYPTO_S_MODULE)

#undef CRYPTO_REG_BASE
#define CRYPTO_REG_BASE (CRYPTO_S_BASE)
#endif
// options
#define INIT_REG_NOONE                  0x0
#define INIT_REG_SOMEONE                0x1
#define CHECK_INIT_FROM_HAL             0x1

/// LEXRA bus is little endian
#define LEXRA_BIG_ENDIAN	        0
/// Timeout value of waiting available source descriptor FIFO nodes
#define FIFOCNT_TIMEOUT		        0x100000
#define DISABLE_PLATFORM            0x0
#define ENABLE_PLATFORM             0x1

//KeyStorage
//#define key_lock                    0x500704F0
//#define SK_OFFSET                   0x20
//#define SK_ADDR                     0x50070500

//
// crc
//
/// Timeout value of waiting CRC engine calculating a result interrupt
#define CRC_TIMEOUT  10000

/* Macros for hp crypto module CRC type */
/// CRC reset register
#define REG_CRC_RST			(0x100) //crc_rst_reg_b
/// CRC operation register
#define REG_CRC_OP			(0x104) //crc_op_reg_b
/// CRC polynomial register
#define REG_CRC_POLY		(0x108) //crc_poly_reg_b
/// CRC initial value register
#define REG_CRC_IV			(0x10C) //crc_iv_reg_b
/// CRC output XOR register
#define REG_CRC_OXOR		(0x110) //crc_oxor_reg_b
/// CRC data register
#define REG_CRC_DATA		(0x114) //crc_data_reg_b
/// CRC status register
#define REG_CRC_STAT		(0x118) //crc_stat_reg_b
/// CRC result register
#define REG_CRC_RES			(0x11C) //crc_result_reg_b
/// CRC count register
#define REG_CRC_CNT			(0x120) //crc_count_reg_b
/// CRC operation register DMA mode setting bit offset
#define CRC_104_DMA			(1<<7)
/// CRC status register checking CRC engine busy bit offset
#define CRC_118_BUSY		(1<<0)
/// CRC status register setting/checking CRC engine interrupt bit offset
#define CRC_118_INTR		(1<<1)
/// CRC status register setting/checking CRC engine interrupt mask bit offset
#define CRC_118_INTRMSK		(1<<2)
/// CRC status register setting/checking CRC engine little endian bit offset
#define CRC_118_LE			(1<<3)

/// @endcond
/** @} */ /* End of group hs_hal_crypto */

//static int crypto_debug_on = 0;
//
//
#define SECTION(_name) __attribute__ ((__section__(_name)))

#if defined(CONFIG_BUILD_NONSECURE) && (CONFIG_BUILD_NONSECURE == 1)
#define SECTION_CRYPTO_TEXT           SECTION(".rom.hal_crypto_ns.text")
#define SECTION_CRYPTO_DATA           SECTION(".rom.hal_crypto_ns.data")
#define SECTION_CRYPTO_RODATA         SECTION(".rom.hal_crypto_ns.rodata")
#define SECTION_CRYPTO_BSS            SECTION(".rom.hal_crypto_ns.bss")
#define SECTION_CRYPTO_STUBS          SECTION(".rom.hal_crypto_ns.stubs")

#else

#define SECTION_CRYPTO_TEXT           SECTION(".rom.hal_crypto_s.text")
#define SECTION_CRYPTO_DATA           SECTION(".rom.hal_crypto_s.data")
#define SECTION_CRYPTO_RODATA         SECTION(".rom.hal_crypto_s.rodata")
#define SECTION_CRYPTO_BSS            SECTION(".rom.hal_crypto_s.bss")
#define SECTION_CRYPTO_STUBS          SECTION(".rom.hal_crypto_s.stubs")
#endif

/**
 * @addtogroup hs_hal_crypto CRYPTO
 * @{
 */

/// @cond DOXYGEN_ROM_HAL_API

const u8 gcm_iv_tail[] __attribute__((aligned(64))) = {0x00, 0x00, 0x00, 0x01};

SECTION_CRYPTO_BSS
hal_crypto_adapter_t *pg_rtl_cryptoEngine;

SECTION_CRYPTO_BSS
volatile uint8_t reg_init;

#if 0
//#include "../rom_template/rtl8277c_rom_crypto_template.h"
extern void dump_for_one_bytes(u8 *pdata, u32 len);
/**  \brief     __dbg_mem_dump is only used to print out memory information of CRYPTO IP. \n
 *              Use \em enable to enable/disable this function which dumps \em size bytes of momery information
 *              from the \em start address.
 */
#undef __dbg_mem_dump
#define __dbg_mem_dump(enable, start, size, str_header) do{ \
    if(enable){ \
        DBG_CRYPTO_INFO(str_header "\r\n");\
        dump_for_one_bytes ((u8 *)start, size);}\
}while(0)
#endif

#if   defined (__CC_ARM)
#pragma push
#pragma anon_unions
#endif

/* Functions for crypto module */

/**
 * @addtogroup hs_hal_crypto_rom_func CRYPTO HAL ROM APIs.
 * @ingroup hs_hal_crypto
 * @{
 * @brief CRYPTO HAL ROM API. The user application(in RAM space) should not call these APIs directly.
 *        There is another set of CRYPTO HAL APIs in the RAM space is provided for the user application.
 */

/**
 *  \fn          u32 __rtl_cpu_to_be32(u32 val)
 *  \brief       Reverse \em val to byte order in a word.
 *  \param[in]   val The value to be reversed
 *  \return      value  Reverse result
 */
u32 __rtl_cpu_to_be32(u32 val)
{
    asm volatile(
        "rev %0, %0"
        : "=r" (val)
        : "0" (val));

    return val;
}

#if 0
/**
 *  \fn          void *rtlc_memset(void *dst, int c, size_t n)
 *  \brief       Sets the first \em c bytes of the block of memory pointed by \em dst to the specified \em value .
 *  \param[in]   dst Pointer to destination address
 *  \param[in]   c Value to be set
 *  \param[in]   n Number of bytes to be set to the \em value
 *  \return      value  destination address
 */
void *rtlc_memset(void *dst, int c, size_t n)
{
    if (n != 0) {
        char *d = dst;

        do {
            *d++ = c;
        } while (--n > 0);
    }
    return dst;
}

/**
 *  \fn          void *rtlc_memcpy( void *s1, const void *s2, size_t n )
 *  \brief       Copies the values of \em n bytes from the location pointed to by \em s2 directly to the memory block pointed to by \em s1 .
 *  \param[in]   s1 Pointer to the destination array where the content is to be copied
 *  \param[in]   s2 Pointer to the source of data to be copied
 *  \param[in]   n Number of bytes to copy
 *  \return      value  destination address
 */
void *rtlc_memcpy( void *s1, const void *s2, size_t n )
{
    char *dst = (char *) s1;
    const char *src = (const char *) s2;

    while (n--) {
        *dst++ = *src++;
    }
    return s1;
}
#endif

// cache functions
/**
 *  \fn          int isDCacheDisabled(void)
 *  \brief       Make sure Dcache state.
 *  \param       void
 *  \return      value 0   Dcache enable
 *  \return      value 1   Dcache disable
 */
#include <cpu_func.h>
int isDCacheDisabled(void)
{
	if( dcache_status()){
		return 0;
	}else{
		return 1;
	}
}

/**
 *  \fn          void arch_invalidate_dcache_by_size(hal_crypto_adapter_t *pcrypto_adapter, uint32_t start_address, uint32_t size)
 *  \brief       Invalidate the first \em size bytes of the block of memory pointed by \em start_address .
 *  \note        If programmer doesn't register an other invalidating dcache function, then it means use internal ram.
 *  \param[in]   pcrypto_adapter Pointer to crypto adapter data
 *  \param[in]   start_address Pointer to start address
 *  \param[in]   size Number of bytes to be clean
 *  \return      none
 */
void arch_invalidate_dcache_by_size(hal_crypto_adapter_t *pcrypto_adapter, uint32_t start_address, uint32_t size)
{
    if (isDCacheDisabled()) {
        //DBG_CRYPTO_ERR("Dcache disable\r\n");
        return;
    }
    if (pcrypto_adapter != NULL) {
        if (pcrypto_adapter->arch_invalidate_dcache_by_size != NULL) {
            //pcrypto_adapter->arch_invalidate_dcache_by_size(start_address, size);
            pcrypto_adapter->arch_invalidate_dcache_by_size(start_address, start_address + size);
        }
        return;
    }
}

/**
 *  \fn          void arch_clean_dcache_by_size(hal_crypto_adapter_t *pcrypto_adapter, uint32_t start_address, uint32_t size)
 *  \brief       Clean the first \em size bytes of the block of memory pointed by \em start_address .
 *  \note        If programmer doesn't register an other cleaning dcache function, then it means use internal ram.
 *  \param[in]   pcrypto_adapter Pointer to crypto adapter data
 *  \param[in]   start_address Pointer to start address
 *  \param[in]   size Number of bytes to be clean
 *  \return      none
 */
void arch_clean_dcache_by_size(hal_crypto_adapter_t *pcrypto_adapter, uint32_t start_address, uint32_t size)
{
    if (isDCacheDisabled()) {
        //DBG_CRYPTO_ERR("Dcache disable\r\n");
        return;
    }
    if (pcrypto_adapter != NULL) {
        if (pcrypto_adapter->arch_clean_dcache_by_size != NULL) {
            //pcrypto_adapter->arch_clean_dcache_by_size(start_address, size);
            pcrypto_adapter->arch_clean_dcache_by_size(start_address, start_address + size);
        }
        return;
    }
}

/**
 *  \fn          void hal_crypto_engine_set_digestlen_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter, IN const uint32_t auth_type)
 *  \brief       Set authentication digest length in crypto adapter.
 *  \param[in]   pcrypto_adapter  Pointer to crypto adapter data
 *  \param[in]   auth_type Authentication type ID
 *  \return      none
 */
void hal_crypto_engine_set_digestlen_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter, IN const uint32_t auth_type)
{
    int digestlen;

    switch (auth_type &  AUTH_TYPE_MASK_FUNC) {
        case AUTH_TYPE_MD5:
            digestlen = CRYPTO_MD5_DIGEST_LENGTH;
            break;

        case AUTH_TYPE_SHA1:
            digestlen = CRYPTO_SHA1_DIGEST_LENGTH;
            break;

        case AUTH_TYPE_SHA2:
            digestlen = pcrypto_adapter->sha2type;
            break;

        default:
            digestlen = 0;
            break;
    }

    pcrypto_adapter->digestlen = digestlen;
    DBG_CRYPTO_INFO("[Set digestlen] digestlen = %d\r\n",digestlen);
}

/**
 *  \fn          int hal_crypto_engine_set_key_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter,
                                                        IN const u8* pCipherKey, IN const int lenCipherKey,
                                                        IN const u8* pAuthKey, IN const int lenAuthKey )
 *  \brief       Set cipher key, authentication key information in crypto adapter.
 *  \param[in]   pcrypto_adapter  Pointer to crypto adapter data
 *  \param[in]   pCipherKey Pointer to crypto cipher key buffer
 *  \param[in]   lenCipherKey Crypto cipher key buffer length
 *  \param[in]   pAuthKey Pointer to authentication key buffer
 *  \param[in]   lenAuthKey Authentication key buffer length
 *  \return      none
 */
void hal_crypto_engine_set_key_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter,
                                        IN const u8 *pCipherKey, IN const int lenCipherKey,
                                        IN const u8 *pAuthKey,   IN const int lenAuthKey)
{
    DBG_CRYPTO_INFO("[Set Cipherkey/Authkey] lenCipherKey = %d,lenAuthKey = %d\r\n",lenCipherKey,lenAuthKey);

    pcrypto_adapter->lenCipherKey = lenCipherKey;
    pcrypto_adapter->pCipherKey   = pCipherKey;

    pcrypto_adapter->pAuthKey   = pAuthKey;
    pcrypto_adapter->lenAuthKey = lenAuthKey;

    if (lenAuthKey > 0) {
        int i;

        pcrypto_adapter->ipad = (u8 *)(&(pcrypto_adapter->g_IOPAD[0]));
        pcrypto_adapter->opad = (u8 *)(&(pcrypto_adapter->g_IOPAD[CRYPTO_PADSIZE]));

        rtlc_memset(pcrypto_adapter->ipad, 0x0, CRYPTO_PADSIZE); // 0x36 for old version RTL
        rtlc_memset(pcrypto_adapter->opad, 0x0, CRYPTO_PADSIZE); // 0x5c for old version RTL

        for (i = 0; i < lenAuthKey; i++) {
            pcrypto_adapter->ipad[i] ^= ((u8*) pcrypto_adapter->pAuthKey)[i];
            pcrypto_adapter->opad[i] ^= ((u8*) pcrypto_adapter->pAuthKey)[i];
        }
    } else {
        pcrypto_adapter->ipad = 0;
        pcrypto_adapter->opad = 0;
    }
}

/**
 *  \fn          int hal_crypto_engine_set_security_mode_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter,
                                                                  IN const uint32_t cipher_type, IN const uint32_t auth_type,
                                                                  IN const void* pCipherKey, IN const uint32_t lenCipherKey,
                                                                  IN const void* pAuthKey, IN const uint32_t lenAuthKey)
 *  \brief       Set basic crypto information in crypto adapter data.
 *  \param[in]   pcrypto_adapter  Pointer to crypto adapter data
 *  \param[in]   cipher_type Cipher type ID
 *  \param[in]   auth_type Authentication type ID
 *  \param[in]   pCipherKey Pointer to Crypto cipher key buffer
 *  \param[in]   lenCipherKey Crypto cipher key buffer length
 *  \param[in]   pAuthKey Pointer to Authentication key buffer
 *  \param[in]   lenAuthKey Authentication key buffer length
 *  \return      value == 0     success
 *  \return      value < 0      fail(error code)
 */
int hal_crypto_engine_set_security_mode_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter,
        IN const uint32_t cipher_type, IN const uint32_t auth_type,
        IN const void *pCipherKey,     IN const uint32_t lenCipherKey,
        IN const void *pAuthKey,       IN const uint32_t lenAuthKey)
{
    if (pcrypto_adapter == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT;
    }
    if (lenAuthKey > 0) { // Authentication
        if (pAuthKey == NULL) {
            return _ERRNO_CRYPTO_NULL_POINTER;
        }
        if (lenAuthKey > CRYPTO_AUTH_PADDING) {
            return _ERRNO_CRYPTO_KEY_OutRange;
        }
    }
    //pcrypto_adapter->mode_select = mode_select;

    // Crypto
    pcrypto_adapter->trides      = 0;
    pcrypto_adapter->aes		 = 0;
    pcrypto_adapter->des		 = 0;
    pcrypto_adapter->chacha		 = 0;
    pcrypto_adapter->isDecrypt	 = 0;
    pcrypto_adapter->cipher_type = cipher_type;

    if (cipher_type != CIPHER_TYPE_NO_CIPHER) {
        switch (cipher_type & CIPHER_TYPE_MASK_FUNC) {
            case CIPHER_TYPE_FUNC_DES :
                pcrypto_adapter->des = 1;
                break;
            case CIPHER_TYPE_FUNC_3DES :
                pcrypto_adapter->trides = 1;
                break;
            case CIPHER_TYPE_FUNC_AES :
                pcrypto_adapter->aes = 1;
                break;
        }
        pcrypto_adapter->isDecrypt = (cipher_type & CIPHER_TYPE_MODE_ENCRYPT)? 0: 1;
    }

    // Auth
    pcrypto_adapter->auth_type = auth_type;
    if (auth_type != AUTH_TYPE_NO_AUTH) {
        pcrypto_adapter->isHMAC = (auth_type & AUTH_TYPE_MASK_HMAC) ? 1 : 0;
        pcrypto_adapter->isMD5  = ((auth_type & AUTH_TYPE_MASK_FUNC) == AUTH_TYPE_MD5 )? 1 : 0;
        pcrypto_adapter->isSHA1 = ((auth_type & AUTH_TYPE_MASK_FUNC) == AUTH_TYPE_SHA1 )? 1 : 0;
        pcrypto_adapter->isSHA2 = ((auth_type & AUTH_TYPE_MASK_FUNC) == AUTH_TYPE_SHA2 )? 1 : 0;
        if (pcrypto_adapter->isSHA2) {
            switch (auth_type & AUTH_TYPE_MASK_SHA2) {
                case AUTH_TYPE_SHA2_224 :
                    pcrypto_adapter->sha2type = SHA2_224;
                    break;
                case AUTH_TYPE_SHA2_256 :
                    pcrypto_adapter->sha2type = SHA2_256;
                    break;
                default :
                    DBG_CRYPTO_ERR("No this auth_type(%d) for SHA2\n", auth_type);
                    pcrypto_adapter->sha2type = SHA2_NONE;
                    break;
            }
        } else {
            pcrypto_adapter->sha2type = SHA2_NONE;
        }
    } else {
        pcrypto_adapter->isMD5    = 0;
        pcrypto_adapter->isHMAC   = 0;
        pcrypto_adapter->isSHA1   = 0;
        pcrypto_adapter->isSHA2   = 0;
        pcrypto_adapter->sha2type = SHA2_NONE;
    }

    hal_crypto_engine_set_digestlen_rtl8277c(pcrypto_adapter, auth_type);
    hal_crypto_engine_set_key_rtl8277c(pcrypto_adapter, pCipherKey, lenCipherKey, pAuthKey, lenAuthKey);

    if (auth_type != AUTH_TYPE_NO_AUTH) {
        pcrypto_adapter->hmac_seq_hash_first = 1;
        pcrypto_adapter->hmac_seq_hash_last = 0;
        pcrypto_adapter->hmac_seq_hash_total_len = 0;
    }
    return SUCCESS;
}

/**
 *  \fn          int hal_crypto_engine_pre_exec_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter)
 *  \brief       If programmer has registered a pre_exec function, it indicates using interrupt handler. So it calls
 *               a registered function to initialize the notified flag before crypto calculating result interrupt triggers ISR.\n
 *               If programmer didn't register a function, it indicates using polling way to detect interrupt.
 *  \param[in]   pcrypto_adapter  Pointer to crypto adapter data
 *  \return      value == 0     success
 *  \return      value == -1    fail
 */
int hal_crypto_engine_pre_exec_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter)
{
    if ((pcrypto_adapter != NULL) && (pcrypto_adapter->pre_exec_func != NULL)) {
        DBG_CRYPTO_INFO("Use pcrypto_adapter->pre_exec_func!\r\n");
        return pcrypto_adapter->pre_exec_func(pcrypto_adapter);
    }
    return SUCCESS;
}

/**
 *  \fn          int hal_crypto_engine_wait_done_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter)
 *  \brief       If programmer has registered a wait_done function, it indicates using interrupt handler. So it calls
 *               a registered function waiting to be notified\n
 *               If programmer didn't register a function, it indicates using polling way. So it will busy-waiting the
 *               crypto calculating result interrupt arrives, or wait until timeout value count down to zero then return fail.
 *  \param[in]   pcrypto_adapter  Pointer to crypto adapter data
 *  \return      value == 0     success
 *  \return      value == -1    fail
 */
int hal_crypto_engine_wait_done_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter)
{
    volatile uint32_t ips_err;
    volatile uint32_t ips_10_status;
    uint32_t loopWait;
    CRYPTO_Type *pcrypto;

    if (pcrypto_adapter == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    pcrypto = pcrypto_adapter->base_addr;

    if (pcrypto_adapter->wait_done_func != NULL) {
        DBG_CRYPTO_INFO("Interrupt Handler way[Use pcrypto_adapter->wait_done_func]!\r\n");
        return pcrypto_adapter->wait_done_func(pcrypto_adapter);
    }

    // wait until ipsec engine stop
    loopWait = 1000000; /* hope long enough */
    while (1) {
        if (pcrypto->ipscsr_reset_isr_conf_reg_b.cmd_ok) {
            break;
        }

        ips_err = (pcrypto->ipscsr_err_stats_reg) & 0xffff;
        if (ips_err) {
            DBG_CRYPTO_ERR("ips 0x1C err = 0x%08x\r\n", ips_err);
            return FAIL;
        }

        loopWait--;
        if (loopWait == 0) {
            ips_10_status = (pcrypto->ipscsr_reset_isr_conf_reg);
            ips_err = (pcrypto->ipscsr_err_stats_reg);
            DBG_CRYPTO_ERR("Wait Timeout ips status =0x%08x, ips err = 0x%08x\r\n", ips_10_status, ips_err);
            return FAIL; /* error occurs */
        }
    }
    return SUCCESS;
}

/**
 *  \fn          void hal_crypto_start_packet_init_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter)
 *  \brief       Clear command ok interrupt and error interrupts.
 *  \param[in]   pcrypto_adapter  Pointer to crypto adapter data
 *  \return      none
 */
void hal_crypto_start_packet_init_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter)
{
    CRYPTO_Type *pcrypto;
    pcrypto = pcrypto_adapter->base_addr;
    DBG_CRYPTO_INFO("[Crypto_engine] start packet init\r\n");
    (pcrypto->ipscsr_err_stats_reg) = 0x3FFF;
    (pcrypto->ipscsr_reset_isr_conf_reg_b.cmd_ok) = 1;
}

/**
 *  \fn          void hal_crypto_engine_auth_calc_apl_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter,
                                                               IN const uint32_t a2eo, IN const uint32_t msglen,
                                                               IN const uint32_t hash_padlen, IN const uint32_t enc_padlen)
 *  \brief       Set crypto the last valid data size and padding data length in crypto adapter.
 *  \details     AES,Hash need 16 bytes alignment message buffer
 *  \param[in]   pcrypto_adapter  Pointer to crypto adapter data
 *  \param[in]   a2eo Additional authentication data buffer length
 *  \param[in]   msglen crypto message buffer length
 *  \param[in]   hash_padlen mix mode hash padding data buffer length
 *  \param[in]   enc_padlen  mix mode encryption padding data buffer length
 *  \return      none
 */
void hal_crypto_engine_auth_calc_apl_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter,
                                              IN const uint32_t a2eo, IN const uint32_t msglen,
                                              IN const uint32_t hash_padlen, IN const uint32_t enc_padlen)
{
    DBG_CRYPTO_INFO("[Crypto_engine] set engine needs relative length data\r\n");
    DBG_CRYPTO_INFO("[Crypto_engine] a2eo = %d, msglen = %d, hash_padlen = %d, enc_padlen = %d\r\n",
                                                                                a2eo, msglen, hash_padlen, enc_padlen);
    pcrypto_adapter->a2eo = a2eo;
    pcrypto_adapter->enl  = msglen;
    pcrypto_adapter->hash_padlen  = hash_padlen;
    pcrypto_adapter->enc_padlen   = enc_padlen;

    if (hash_padlen == 0) { // normal mode:HASH,AES msglen 16 byte alignment, AES_GCM aadlen 16 byte alignment
        // DES / 3DES : 8 byte alignment
        if ((pcrypto_adapter->des == 1) || (pcrypto_adapter->trides == 1)) {
            pcrypto_adapter->enc_last_data_size = msglen % 8;
            pcrypto_adapter->apl = (8 - pcrypto_adapter->enc_last_data_size) % 8;

            pcrypto_adapter->aad_last_data_size = a2eo % 8;
            pcrypto_adapter->apl_aad = (8 - pcrypto_adapter->aad_last_data_size) % 8;
        } else { // AES,Chacha,Hash : 16 byte alignment
            pcrypto_adapter->enc_last_data_size = msglen % 16;
            pcrypto_adapter->apl = (16 - pcrypto_adapter->enc_last_data_size) % 16;

            pcrypto_adapter->aad_last_data_size = a2eo % 16;
            pcrypto_adapter->apl_aad = (16 - pcrypto_adapter->aad_last_data_size) % 16;
        }
        #if 0
        pcrypto_adapter->enc_last_data_size = msglen % 16;
        pcrypto_adapter->apl = (16 - pcrypto_adapter->enc_last_data_size) % 16;

        pcrypto_adapter->aad_last_data_size = a2eo % 16;
        pcrypto_adapter->apl_aad = (16 - pcrypto_adapter->aad_last_data_size) % 16;
        #endif
    } else { // mix mode: a2eo 8 byte alignment, hash_padlen 8 byte alignment, enc_padlen 8 byte alignment
        pcrypto_adapter->enc_last_data_size = msglen % 16;
        pcrypto_adapter->apl = (16 - pcrypto_adapter->enc_last_data_size) % 16;

        pcrypto_adapter->aad_last_data_size = a2eo % 8;
        pcrypto_adapter->apl_aad = (8 - pcrypto_adapter->aad_last_data_size) % 8;

        pcrypto_adapter->hashpad_last_data_size = hash_padlen % 8;
        pcrypto_adapter->hashpad_pad = (8 - pcrypto_adapter->hashpad_last_data_size) % 8;

        pcrypto_adapter->encpad_last_data_size = enc_padlen % 8;
        pcrypto_adapter->encpad_pad = (8 - pcrypto_adapter->encpad_last_data_size) % 8;
    }
}

/**
 *  \fn          void hal_crypto_modify_enc_last_data_size_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter,
                                                                    IN const u8 *message,IN const uint32_t msglen)
 *  \brief       Only AES_GCM may modify enc_last_data_size in decryption to match with the tag value of encryption.
 *  \note        Observe if AES_GCM use non-16 bytes aligned plaintext as an input,
 *               it will generate a 16 bytes aligned ciphertext which is composed of ori_len(plaintext)+padding_len(0). \n
 *               And then use this 16 bytes aligned msglen of ciphertext as decryption input,
 *               this could recover back to the plaintext; however, the enc_last_data_size is different from encryption,
 *               so it leads to different tag value of decryption. That's why need to modify the enc_last_data_size same as encryption.
 *  \param[in]   pcrypto_adapter  Pointer to crypto adapter data
 *  \param[in]   message Pointer to Crypto cipher message buffer
 *  \param[in]   msglen Crypto cipher message buffer length
 *  \return      void
 */
void hal_crypto_modify_enc_last_data_size_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter,
                                                   IN const u8 *message,IN const uint32_t msglen){
    int vLaid_byte = 0;
    int find_zero = 0;
    // Because AES_GCM,Chacha_poly1305 only allow 16bytes aligned msglen to decrypt,
    // jump to the start_addr of the last 16 bytes msg.
    int offset_byte = (msglen - 16);
    u8 *pLast_start = (u8 *)message + offset_byte;

    while(vLaid_byte < 16){

        if(*pLast_start != 0x00){
            if(find_zero != 0){
                find_zero = 0;
            }
        }else{
            ++find_zero;
        }
        ++vLaid_byte;
        ++pLast_start;
    }

    if(16 == vLaid_byte){
        // The last one
        if(1 == find_zero){
            find_zero = 0;
        }

        vLaid_byte = 16 - find_zero;
    }

    DBG_CRYPTO_INFO("vLaid_byte = %d\r\n",vLaid_byte);
    pcrypto_adapter->enc_last_data_size = vLaid_byte;
}

/**
 *  \fn          uint32_t hal_crypto_dstdesc_fifo_cnt_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter)
 *  \brief       Know how many destination descripter fifo nodes are available to use.
 *  \param[in]   pcrypto_adapter  Pointer to crypto adapter data
 *  \return      value >= 0     fifo empty nodes count value
 */
uint32_t hal_crypto_dstdesc_fifo_cnt_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter)
{
    CRYPTO_Type *pcrypto;
    pcrypto = pcrypto_adapter->base_addr;
    return (pcrypto->dstdesc_status_reg_b.fifo_empty_cnt);
}

/**
 *  \fn          void hal_crypto_set_dstdesc_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter, uint32_t dd1, uint32_t ddpr)
 *  \brief       Check whether there are enough fifo nodes for destination descripter, if there're enough,
 *               then set destination descripter first/second words to crypto destination descripter registers.
 *  \param[in]   pcrypto_adapter  Pointer to crypto adapter data
 *  \param[in]   dd1 Destination descripter first word pointer value
 *  \param[in]   ddpr Destination descripter second word pointer value(data pointer value)
 *  \return      none
 */
void hal_crypto_set_dstdesc_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter, uint32_t dd1, uint32_t ddpr)
{
    CRYPTO_Type *pcrypto;
    pcrypto = pcrypto_adapter->base_addr;
    if (hal_crypto_dstdesc_fifo_cnt_rtl8277c(pcrypto_adapter) > 0) {
        DBG_CRYPTO_INFO("dd1=0x%08x , ddpr=0x%08x\r\n",dd1, ddpr);
        (pcrypto->ddfw_reg) = dd1;
        (pcrypto->ddsw_reg) = ddpr;
    } else {
        DBG_CRYPTO_ERR("dst fifo_cnt is not correct: %d \r\n", hal_crypto_dstdesc_fifo_cnt_rtl8277c(pcrypto_adapter));
    }
}

/**
 *  \fn          uint32_t hal_crypto_srcdesc_fifo_cnt_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter)
 *  \brief       Know how many source descripter fifo nodes are available to use.
 *  \param[in]   pcrypto_adapter  Pointer to crypto adapter data
 *  \return      value >= 0     fifo empty nodes count value
 */
uint32_t hal_crypto_srcdesc_fifo_cnt_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter)
{
    CRYPTO_Type *pcrypto;
    pcrypto = pcrypto_adapter->base_addr;
    return (pcrypto->srcdesc_status_reg_b.fifo_empty_cnt);
}

/**
 *  \fn          void hal_crypto_set_srcdesc_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter, uint32_t sd1, uint32_t sdpr)
 *  \brief       Check whether there are enough fifo nodes for source descripter, if there're enough,
 *               then set source descripter first/second words to crypto source descripter registers,
 *               or wait until timeout value count down to zero then break.
 *  \param[in]   pcrypto_adapter Pointer to crypto adapter data
 *  \param[in]   sd1 Source descripter first word pointer value
 *  \param[in]   sdpr Source descripter second word pointer value(data pointer value)
 *  \return      none
 */
void hal_crypto_set_srcdesc_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter, uint32_t sd1, uint32_t sdpr)
{
    uint32_t cnt, timeout;
    CRYPTO_Type *pcrypto;
    pcrypto = pcrypto_adapter->base_addr;
    timeout = FIFOCNT_TIMEOUT;

    while (1) {
        if ((cnt = hal_crypto_srcdesc_fifo_cnt_rtl8277c(pcrypto_adapter)) > 0) {
            DBG_CRYPTO_INFO("sd1=0x%08x , sdpr=0x%08x\r\n",sd1, sdpr);
            (pcrypto->sdfw_reg) = (sd1); // sd1 | 0x80000000
            (pcrypto->sdsw_reg) = (sdpr);
            break;
        }
        if (pcrypto_adapter->fifo_wait_func != NULL) {
            pcrypto_adapter->fifo_wait_func(pcrypto_adapter);
        } else {
            DBG_CRYPTO_ERR("src fifo_cnt is FULL: %d \r\n", cnt);
            timeout--;
            if (timeout == 0) {
                break;
            }
        }
    }
}

/**
 *  \fn          void hal_crypto_engine_setup_cl_buffer_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter)
 *  \brief       Use crypto adatper information to set crypto command setting buffer for normal mode.
 *  \param[in]   pcrypto_adapter  Pointer to crypto adapter data
 *  \return      none
 */
void hal_crypto_engine_setup_cl_buffer_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter)
{
    rtl_crypto_cl_t *pCL;
    uint32_t a2eo;
    uint32_t enl;

    DBG_CRYPTO_INFO("Set up src_desc command register\r\n");
    a2eo = pcrypto_adapter->a2eo;
    enl  = pcrypto_adapter->enl;

    //allocate a 32byte-aligned buffer and rtlc_memset
    pCL = (rtl_crypto_cl_t *)pcrypto_adapter->cl_buffer;
    rtlc_memset((u8*)pCL, 0, sizeof(pcrypto_adapter->cl_buffer));

    if (pcrypto_adapter->auth_type != AUTH_TYPE_NO_AUTH) {
        pCL->engine_mode = 1; //hash only

        //Enable seq_hash mechanism
        pCL->hmac_seq_hash = 1;

        //Check seq_hash first msg payload
        pCL->hmac_seq_hash_first = 0;
        if (pcrypto_adapter->hmac_seq_hash_first == 1) {
            pCL->hmac_seq_hash_first = 1;
            pcrypto_adapter->hmac_seq_hash_first = 0;
        }

        //Set mode parameters depend on what hash algorithm is
        if (pcrypto_adapter->isMD5) {
            pCL->habs = 1;
            pCL->hibs = 1;
            pCL->hobs = 1;
            pCL->hkbs = 1;
        } else if (pcrypto_adapter->isSHA1) {
            pCL->hmac_mode = 1;
            pCL->habs = 1;
        } else if (pcrypto_adapter->sha2type != SHA2_NONE) {
            pCL->hmac_mode = (pcrypto_adapter->sha2type == SHA2_224 )? 2 : 3 ; // currently only support SHA2_224 / SHA2_256
            pCL->habs = 1;
        }

        if (pcrypto_adapter->isHMAC) {
            pCL->hmac_en = 1;
        }

        if (pcrypto_adapter->hmac_seq_hash_last == 1) {

            // last msg payload always uses auto padding
            pCL->enl = (enl + 15)/16 ;
            pCL->enc_last_data_size = pcrypto_adapter->enc_last_data_size;
            pCL->ap0 = pcrypto_adapter->hmac_seq_hash_total_len * 8;
            DBG_CRYPTO_INFO("[Set src_desc_cmd_rst] enl = %d, enc_last_data_size = %d\r\n",pCL->enl,pcrypto_adapter->enc_last_data_size);
            DBG_CRYPTO_INFO("[Set src_desc_cmd_rst] hmac_seq_hash_total_len = %d\r\n",pcrypto_adapter->hmac_seq_hash_total_len);
            if (pcrypto_adapter->isHMAC) {
                pCL->ap0 += (64 * 8);
            }
        } else {
            pCL->enl = enl/64;
        }

        //Check seq_hash last msg payload. If it's the last msg payload,need to write back(Hash out result)
        pCL->hmac_seq_hash_last = 0;
        pCL->hmac_seq_hash_no_wb = 1;
        if (pcrypto_adapter->hmac_seq_hash_last == 1) {
            pCL->hmac_seq_hash_last = 1;
            pCL->hmac_seq_hash_no_wb = 0;
        }
    } else { // cipher - encryption / decryption

        uint32_t cipher_type;
        uint32_t block_mode;

        // ECB / CBC / CTR;
        cipher_type = pcrypto_adapter->cipher_type;
        block_mode = cipher_type & CIPHER_TYPE_MASK_BLOCK;
        pCL->cipher_mode = block_mode;
        DBG_CRYPTO_INFO("[Set src_desc_cmd_rst] cipher_type = 0x%x,cipher_mode = 0x%x\r\n",cipher_type,block_mode);

        if (pcrypto_adapter->aes) {
            pCL->cipher_eng_sel = 0;
            switch (pcrypto_adapter->lenCipherKey) {
                case 128/8 :
                    pCL->aes_key_sel = 0;
                    break;
                case 192/8 :
                    pCL->aes_key_sel = 1;
                    break;
                case 256/8 :
                    pCL->aes_key_sel = 2;
                    break;
                default:
                    break;
            }

            //AES,Chacha take 16bytes as a block to process data
            pCL->enl = (enl + 15)/16;
            pCL->enc_last_data_size = pcrypto_adapter->enc_last_data_size;

            if ((block_mode ==  CIPHER_TYPE_BLOCK_GCM) || (block_mode ==  CIPHER_TYPE_BLOCK_GMAC)) {
                pCL->header_total_len = (a2eo + 15)/16;
                pCL->aad_last_data_size = pcrypto_adapter->aad_last_data_size;
            }
        }
        else if ( pcrypto_adapter->des ) { //DES,3DES take 8bytes as a block to process data
            pCL->cipher_eng_sel = 1;
            pCL->des3_en = 0;
            pCL->enl = (enl + 7)/8;
            pCL->enc_last_data_size = pcrypto_adapter->enc_last_data_size;
        }
        else if (pcrypto_adapter->trides) {
            pCL->cipher_eng_sel = 1;
            pCL->des3_en = 1;
            pCL->enl = (enl + 7)/8;
            pCL->enc_last_data_size = pcrypto_adapter->enc_last_data_size;
        }

        DBG_CRYPTO_INFO("[Set src_desc_cmd_rst] enl = %d, enc_last_data_size = %d\r\n",pCL->enl,pCL->enc_last_data_size);
        DBG_CRYPTO_INFO("[Set src_desc_cmd_rst] header_total_len = %d, aad_last_data_size = %d\r\n",pCL->header_total_len,pCL->aad_last_data_size);

        if (pcrypto_adapter->isDecrypt == 0) {
            pCL->cipher_encrypt = 1;
        }
        if (pcrypto_adapter->aes) {
            pCL->cabs = 1;
        }
    }

    pCL->icv_total_length = 0x40; // for mix mode, but need to set a value 0x40
}

/**
 *  \fn          void hal_crypto_engine_mix_setup_cl_buffer_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter)
 *  \brief       Use crypto adatper information to set crypto command setting buffer for mix mode.
 *  \param[in]   pcrypto_adapter  Pointer to crypto adapter data
 *  \return      none
 */
void hal_crypto_engine_mix_setup_cl_buffer_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter)
{
    rtl_crypto_cl_t *pCL;
    uint32_t a2eo;
    uint32_t enl;
    uint32_t hash_padlen;
    uint32_t enc_padlen;

    DBG_CRYPTO_INFO("[Mix mode]Set up src_desc command register\r\n");
    a2eo = pcrypto_adapter->a2eo;
    enl  = pcrypto_adapter->enl;
    hash_padlen = pcrypto_adapter->hash_padlen;
    enc_padlen  = pcrypto_adapter->enc_padlen;

    //allocate a 32byte-aligned buffer and rtlc_memset
    pCL = (rtl_crypto_cl_t *)pcrypto_adapter->cl_buffer;
    rtlc_memset((u8*)pCL, 0, sizeof(pcrypto_adapter->cl_buffer));

    if ((pcrypto_adapter->auth_type != AUTH_TYPE_NO_AUTH) && (pcrypto_adapter->cipher_type != CIPHER_TYPE_NO_CIPHER)) {
        pCL->engine_mode = pcrypto_adapter->mix_mode_type; //SSH-enc/ESP-dec

        //Disable seq_hash mechanism
        pCL->hmac_seq_hash = 0;

        // Mix mode doesn't have the seq_hash mechanism
        pCL->hmac_seq_hash_first = 0;
        pCL->hmac_seq_hash_last = 0;
        pCL->hmac_seq_hash_no_wb = 0;
        pcrypto_adapter->hmac_seq_hash_first = 0;

        //Set mode parameters depend on what hash algorithm is
        if (pcrypto_adapter->isMD5) {
            pCL->habs = 1;
            pCL->hibs = 1;
            pCL->hobs = 1;
            pCL->hkbs = 1;
            pCL->hmac_mode = 0;
        } else if (pcrypto_adapter->isSHA1) {
            pCL->hmac_mode = 1;
            pCL->habs = 1;
        } else if (pcrypto_adapter->sha2type != SHA2_NONE) {
            pCL->hmac_mode = (pcrypto_adapter->sha2type == SHA2_224 )? 2 : 3 ; // currently only support SHA2_224 / SHA2_256
            pCL->habs = 1;
        }

        if (pcrypto_adapter->isHMAC) {
            pCL->hmac_en = 1;
        }

        //cipher
        uint32_t cipher_type;
        uint32_t block_mode;

        // ECB / CBC / CFB / OFB /CTR;
        cipher_type = pcrypto_adapter->cipher_type;
        block_mode = cipher_type & CIPHER_TYPE_MASK_BLOCK;
        pCL->cipher_mode = block_mode;
        DBG_CRYPTO_INFO("[Set src_desc_cmd_rst] cipher_type = 0x%x,cipher_mode = 0x%x\r\n",cipher_type,block_mode);

        if (pcrypto_adapter->aes) {
            pCL->cipher_eng_sel = 0;
            switch (pcrypto_adapter->lenCipherKey) {
                case 128/8 :
                    pCL->aes_key_sel = 0;
                    break;
                case 192/8 :
                    pCL->aes_key_sel = 1;
                    break;
                case 256/8 :
                    pCL->aes_key_sel = 2;
                    break;
                default:
                    break;
            }

            //AES take 16bytes as a block to process data
            pCL->enl = (enl + 15)/16;
            pCL->enc_last_data_size = pcrypto_adapter->enc_last_data_size;

            pCL->header_total_len = (a2eo + 7)/8;
            pCL->aad_last_data_size = pcrypto_adapter->aad_last_data_size;

            pCL->hash_pad_len = (hash_padlen + 7)/8;
            pCL->pad_last_data_size = pcrypto_adapter->hashpad_last_data_size;

            pCL->eptl = (enc_padlen + 7)/8;
            pCL->enc_pad_last_data_size = pcrypto_adapter->encpad_last_data_size;

        }

        DBG_CRYPTO_INFO("[Set src_desc_cmd_rst] enl = %d, enc_last_data_size = %d\r\n",pCL->enl,pCL->enc_last_data_size);
        DBG_CRYPTO_INFO("[Set src_desc_cmd_rst] header_total_len = %d, aad_last_data_size = %d\r\n",pCL->header_total_len,pCL->aad_last_data_size);
        DBG_CRYPTO_INFO("[Set src_desc_cmd_rst] hash_padlen = %d, pad_last_data_size = %d\r\n",pCL->hash_pad_len,pCL->pad_last_data_size);
        DBG_CRYPTO_INFO("[Set src_desc_cmd_rst] enc_padlen = %d, enc_pad_last_data_size = %d\r\n",pCL->eptl,pCL->enc_pad_last_data_size);

        if (pcrypto_adapter->isDecrypt == 0) {
            pCL->cipher_encrypt = 1;
        }
        if (pcrypto_adapter->aes) {
            pCL->cabs = 1;
        }
    }
    if (CRYPTO_ENGINE_SET_MIX_MODE_SSL_TLS_ENC == (pCL->engine_mode)) {
        pCL->icv_total_length = (pcrypto_adapter->digestlen); // For mix mode ssl_tls_enc, need to set less than or equal to digest length.
    } else {
        pCL->icv_total_length = 0x40; // For others(mix_mode ssh,esp/only cipher/only hash),this set max. digest length value = 64byts(0x40).
    }
}

/**
 *  \fn          void hal_crypto_engine_srcdesc_generate_cl_key_iv_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter,
 *                                                                          const u8* p_iv, uint32_t ivLen)
 *  \brief       Set command setting buffer, key buffer, initial vector buffer, HMAC padding buffer in source descripter.
 *  \param[in]   pcrypto_adapter  Pointer to crypto adapter data
 *  \param[in]   p_iv Pointer to crypto initial vector buffer
 *  \param[in]   ivLen Crypto initial vector buffer length
 *  \return      none
 */
void hal_crypto_engine_srcdesc_generate_cl_key_iv_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter,
                                                           const u8 *p_iv, uint32_t ivLen)
{
    rtl_crypto_srcdesc_t src_desc;
    //uint32_t val;

    src_desc.w = 0;

    // FS=1, CL=3
    src_desc.b.rs = 1;
    src_desc.b.fs = 1;
    src_desc.b.cl= 3;

#if 0 // Ameba version, key storage can be used for hmac
    if (pcrypto_adapter->isSecurekey == 1) {
        if (pcrypto_adapter->isHMAC == _TRUE) {
        DBG_CRYPTO_INFO("secure key for auth\r\n");
            src_desc.b.sk = 1;
            src_desc.b.key_len = pcrypto_adapter->securekeyIndex;
            //src_desc.b.keypad_len = 128/4; //HMAC keypad_len use 4bytes as a unit
        }
        if (pcrypto_adapter->isDecrypt == _TRUE || pcrypto_adapter->isDecrypt == _FALSE) {
            DBG_CRYPTO_INFO("secure key for cipher\r\n");
            src_desc.b.sk = 1;
            src_desc.b.key_len = pcrypto_adapter->securekeyIndex;
        }
    }
#else // 8277C version, key storage cannot be used for hmac (use key pad)
    if (pcrypto_adapter->isSecurekeypad == 1) {
        DBG_CRYPTO_INFO("secure keypad for auth\r\n");
        src_desc.b.pk = 1;
        src_desc.b.keypad_len = pcrypto_adapter->securekeypadIndex;
    }
    if (pcrypto_adapter->isSecurekey == 1) {
        DBG_CRYPTO_INFO("secure key for cipher\r\n");
        src_desc.b.sk = 1;
        src_desc.b.key_len = pcrypto_adapter->securekeyIndex;
    }
#endif

    if (pcrypto_adapter->isWriteback == 1){
        src_desc.b.wk = 1;
        src_desc.b.iv_len = pcrypto_adapter->writebackIndex;
        //DBG_CRYPTO_INFO("src_desc.b.wk = %d\n", src_desc.b.wk);
        //DBG_CRYPTO_INFO("src_desc.b.iv_len (writebackIndex) = %d\n", src_desc.b.iv_len);
    }

    if ((pcrypto_adapter->hash_padlen) == 0) { // Normal mode: enable auto-padding
        // auto padding[Hash algorithms need to set this]
        // It doesn't mean padding that makes up 16bytes aligned msglen]
        if (pcrypto_adapter->hmac_seq_hash_last) {
            src_desc.b.ap = 0x01;
        }
        hal_crypto_engine_setup_cl_buffer_rtl8277c(pcrypto_adapter);
    } else {  //Mix mode: disable auto-padding
        src_desc.b.ap = 0x00;
        hal_crypto_engine_mix_setup_cl_buffer_rtl8277c(pcrypto_adapter);
    }

    arch_clean_dcache_by_size(pcrypto_adapter, (size_t)(&(pcrypto_adapter->cl_buffer)[0]), 32);
    __dbg_mem_dump(pcrypto_adapter->isMemDump,(size_t)(&(pcrypto_adapter->cl_buffer)[0]), sizeof(pcrypto_adapter->cl_buffer), "Command Setting: ");
    hal_crypto_set_srcdesc_rtl8277c(pcrypto_adapter, src_desc.w, (size_t)(&(pcrypto_adapter->cl_buffer)[0]));

    // Set key
    if (pcrypto_adapter->cipher_type != CIPHER_TYPE_NO_CIPHER) {
        uint32_t lenCipherKey;
        u8  *pCipherKey;
        uint32_t cache_len;

        if (pcrypto_adapter->isSecurekey == 1) {
            DBG_CRYPTO_INFO("Do not need keylen\r\n");
        } else {
            lenCipherKey = (size_t) pcrypto_adapter->lenCipherKey;
            pCipherKey = (u8*) pcrypto_adapter->pCipherKey;
            DBG_CRYPTO_INFO("[Set src_desc_set_key] lenCipherKey = %d\r\n",lenCipherKey);

            src_desc.w = 0;
            src_desc.b.rs = 1;
            src_desc.b.fs = 1;
            src_desc.b.key_len = lenCipherKey/4;  //key_len use 4bytes as a unit

            cache_len = (lenCipherKey & 0x1F)? ((lenCipherKey/32) + 1) * 32: lenCipherKey;
            arch_clean_dcache_by_size(pcrypto_adapter, (size_t)(pCipherKey), cache_len);
            __dbg_mem_dump(pcrypto_adapter->isMemDump,(size_t)(pCipherKey), lenCipherKey, "KEY: ");
            hal_crypto_set_srcdesc_rtl8277c(pcrypto_adapter, src_desc.w, (size_t)(pCipherKey));
        }
    }

    // Set IV
    if (pcrypto_adapter->isWriteback == 1) {
        DBG_CRYPTO_INFO("Do not need ivlen\r\n");
    } else {
        if (p_iv != NULL && ivLen > 0) {
            DBG_CRYPTO_INFO("[Set src_desc_set_IV] ivLen = %d\r\n",ivLen);
            uint32_t cache_len;
            src_desc.w = 0;
            src_desc.b.rs = 1;
            src_desc.b.fs = 1;
            src_desc.b.iv_len = ivLen/4;  //iv_len use 4bytes as a unit

            if (pcrypto_adapter->isSecurekey == 1) {
                src_desc.b.sk = 1;
                src_desc.b.key_len = pcrypto_adapter->securekeyIndex;
            }

            if (ivLen & 0x1F) {
                cache_len = ((ivLen/32) + 1) * 32;
            } else {
                cache_len = ivLen;
            }
            arch_clean_dcache_by_size(pcrypto_adapter, (size_t)(p_iv), cache_len);
            __dbg_mem_dump(pcrypto_adapter->isMemDump,(size_t)(p_iv), ivLen, "IV: ");
            hal_crypto_set_srcdesc_rtl8277c(pcrypto_adapter, src_desc.w, (size_t)(p_iv));
        }
    }

    // Set Pad
    if (pcrypto_adapter->isSecurekeypad == 1) {
        DBG_CRYPTO_INFO("Do not need keypadlen\r\n");
    } else {
        if (pcrypto_adapter->isHMAC) {
            src_desc.w = 0;
            src_desc.b.rs = 1;
            src_desc.b.fs = 1;
            src_desc.b.keypad_len = 128/4; //HMAC keypad_len use 4bytes as a unit

            if (pcrypto_adapter->isWriteback == 1){
                src_desc.b.wk = 1;
                src_desc.b.iv_len = pcrypto_adapter->writebackIndex;
            }

            arch_clean_dcache_by_size(pcrypto_adapter, (size_t)(&(pcrypto_adapter->g_IOPAD[0])), 128);
            __dbg_mem_dump(pcrypto_adapter->isMemDump,(size_t)(&(pcrypto_adapter->g_IOPAD[0])), 128, "HMAC Key PAD: ");
            hal_crypto_set_srcdesc_rtl8277c(pcrypto_adapter, src_desc.w, (size_t)(&(pcrypto_adapter->g_IOPAD[0])));
        }
    }
}

/**
 *  \fn          int hal_crypto_engine_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter,
                                                IN const u8 *message, 	IN const uint32_t msglen,
                                                IN const u8 *pIv, 		IN const uint32_t ivlen,
                                                IN const u8 *paad, 		IN const uint32_t aadlen,
                                                OUT u8 *pResult, OUT u8 *pTag)
 *  \brief       Check input parameters, and set crypto engine registers, then make crypto engine handle this algorithm.
 *  \details     The executing order of crypto engine setting: \n
 *               - Initialize the notified mechanism.
 *               - Set essential buffers and buffer lengths of the cryptographic feature to crypto adapter
 *               - Setup desitination descriptor
 *               - Setup source descriptor
 *                  - Setup command setting buffer
 *                  - Setup key, initial vector, HMAC padding buffer
 *                  - Prepare Data1 ~ DataN
 *               - Wait to be notified after interrupt service routine processed over.
 *  \note        For normal mode(Seperate to calcualte each algorithm)
 *  \param[in]   pcrypto_adapter  Pointer to crypto adapter data
 *  \param[in]   message Pointer to Crypto cipher message buffer
 *  \param[in]   msglen Crypto cipher message buffer length
 *  \param[in]   pIv Pointer to Crypto cipher initial vector buffer
 *  \param[in]   ivlen Crypto cipher initial vector buffer length
 *  \param[in]   paad Pointer to Crypto cipher additional authentication data buffer
 *  \param[in]   aadlen Crypto cipher additional authentication data buffer length
 *  \param[out]  pResult Pointer to Crypto cipher result buffer
 *  \param[out]  pTag Pointer to Crypto cipher Tag buffer
 *  \return      value == 0     success
 *  \return      value < 0      fail(error code)
 */
int hal_crypto_engine_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter,
                               IN const u8 *message, 	IN const uint32_t msglen,
                               IN const u8 *pIv, 		IN const uint32_t ivlen,
                               IN const u8 *paad,     IN const uint32_t aadlen,
                               OUT u8 *pResult, OUT u8 *pTag)
{
    rtl_crypto_srcdesc_t srcdesc;

    uint32_t a2eo;
    uint32_t enl;
    uint32_t digestlen;
    uint32_t cipher_type = CIPHER_TYPE_NONE;
    uint32_t block_mode;
    volatile uint8_t *padding = NULL;
    int ret = SUCCESS;

    if (pcrypto_adapter == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    } else {
        padding = (&(pcrypto_adapter->padding[0]));
    }
    hal_crypto_engine_pre_exec_rtl8277c(pcrypto_adapter);

    if ((paad != NULL) && (aadlen > 0)) {
        uint32_t cache_len;
        if (aadlen & (cache_alinge-1)) {
            cache_len = ((aadlen/cache_alinge) + 1) *cache_alinge;
        } else {
            cache_len = aadlen;
        }
        arch_clean_dcache_by_size(pcrypto_adapter, (size_t)paad, cache_len);
    }

    // Use only one scatter
    a2eo = aadlen;
    enl = msglen;

    hal_crypto_start_packet_init_rtl8277c(pcrypto_adapter);
//
//   Set relative length data
//
    hal_crypto_engine_auth_calc_apl_rtl8277c(pcrypto_adapter, a2eo, enl, 0, 0);
    //Only AES_GCM may modify enc_last_data_size in decryption to match with the tag value of encryption
    if (pcrypto_adapter->isDecrypt){
        cipher_type = pcrypto_adapter->cipher_type;
        block_mode  = cipher_type & CIPHER_TYPE_MASK_BLOCK;
        if ((pcrypto_adapter->aes) && (block_mode ==  CIPHER_TYPE_BLOCK_GCM)) {
            hal_crypto_modify_enc_last_data_size_rtl8277c(pcrypto_adapter,message,enl);
        }
    }

    //Hash result length
    digestlen = pcrypto_adapter->digestlen;

    if (pcrypto_adapter->auth_type != AUTH_TYPE_NO_AUTH) {

        //Sum of seq_hash msglen
        pcrypto_adapter->hmac_seq_hash_total_len += msglen;

        //Make sure if it's the last seq_hash msg payload
        if (pResult != NULL) {
            pcrypto_adapter->hmac_seq_hash_last = 1;
        }
    }

    /********************************************
     * step 1: Setup desitination descriptor
     ********************************************/
    pcrypto_adapter->is_dst_first_cache_used = 0;
    pcrypto_adapter->is_dst_last_cache_used  = 0;

    if (pcrypto_adapter->auth_type != AUTH_TYPE_NO_AUTH) {
        rtl_crypto_dstdesc_t dst_desc;

        //It's the last seq_hash msg payload, so need to hash out the result
        if (pResult != NULL) {
            dst_desc.w = 0;
            dst_desc.auth.ws  = 1;
            dst_desc.auth.fs  = 1;
            dst_desc.auth.ls  = 1;
            dst_desc.auth.adl = digestlen;
            arch_invalidate_dcache_by_size(pcrypto_adapter, (size_t)pcrypto_adapter->digest_result, 32);
            hal_crypto_set_dstdesc_rtl8277c(pcrypto_adapter, dst_desc.w, (size_t)pcrypto_adapter->digest_result);
        }

    } else { // cipher(also need dst_auth_desc(such as AES_GCM or Chacha_poly1305) or poly1305)
        rtl_crypto_dstdesc_t  dst_auth_desc;
        rtl_crypto_dstdesc_t  dst_cipher_desc;
        int8_t first_cache_pos = 0;
        int8_t first_cache_len = 0;
        int8_t last_cache_pos  = 0;
        int8_t last_cache_len  = 0;
        int dst_len;

        if (pResult != NULL) {
            //Destination descriptor may change msglen, because of encryption padding
            dst_len = (pcrypto_adapter->enl + (pcrypto_adapter->apl));
            DBG_CRYPTO_INFO("[Set dest_desc] dst_len = %d,enl = %d,apl = %d\r\n",dst_len,(pcrypto_adapter->enl),(pcrypto_adapter->apl));

            first_cache_pos = (size_t)pResult & (cache_alinge-1);
            first_cache_len = (cache_alinge - first_cache_pos) % cache_alinge;
            if (first_cache_len >= dst_len) {
                //There's an enough space of first_cache_line for dst_len
                first_cache_len = dst_len;
                last_cache_len  = 0;
            } else {
                last_cache_pos = (size_t)(pResult + dst_len - 1) & (cache_alinge-1);
                last_cache_len = last_cache_pos + 1;
                if (last_cache_len == cache_alinge) {
                    last_cache_len = 0;
                }
            }
            DBG_CRYPTO_INFO("[Set dest_desc] f_cache_pos = %d,f_cache_len = %d\r\n",first_cache_pos,first_cache_len);
            DBG_CRYPTO_INFO("[Set dest_desc] l_cache_pos = %d,l_cache_len = %d\r\n",last_cache_pos,last_cache_len);

            dst_cipher_desc.w = 0;
            dst_cipher_desc.cipher.ws  = 1;
            dst_cipher_desc.cipher.fs  = 1;
            dst_cipher_desc.cipher.enc = 1;

            if (first_cache_len != 0) {
                pcrypto_adapter->is_dst_first_cache_used = 1;
                dst_cipher_desc.cipher.enl = first_cache_len;
                DBG_CRYPTO_INFO("[Set dest_desc] f_cache_line_enl = %d\r\n",first_cache_len);
                if ((first_cache_len >= dst_len) && (pTag == NULL)) {
                    dst_cipher_desc.cipher.ls = 1;
                }
                arch_clean_dcache_by_size(pcrypto_adapter, (size_t)pcrypto_adapter->dst_first_cache_line, cache_alinge);
                hal_crypto_set_dstdesc_rtl8277c(pcrypto_adapter, dst_cipher_desc.w, (size_t)pcrypto_adapter->dst_first_cache_line);

                dst_len -= first_cache_len;
                pResult += first_cache_len;

                dst_cipher_desc.w = 0;
                dst_cipher_desc.cipher.ws = 1;
                dst_cipher_desc.cipher.enc = 1;
            }

            if (dst_len > 0) {
                //Cut off valid msg of last_cache_line
                //It means leave 32 bytes alignment addr of msg_body
                dst_cipher_desc.cipher.enl = (dst_len - last_cache_len);

                if (last_cache_len != 0) {
                    pcrypto_adapter->is_dst_last_cache_used = 1;
                    if (dst_len > last_cache_len) { // body
                        arch_clean_dcache_by_size(pcrypto_adapter, (size_t)pResult, (dst_len - last_cache_len));
                        hal_crypto_set_dstdesc_rtl8277c(pcrypto_adapter, dst_cipher_desc.w, (size_t)pResult);
                    }
                    pResult += (dst_len - last_cache_len);
                    dst_len = last_cache_len;

                    dst_cipher_desc.w = 0;
                    dst_cipher_desc.cipher.ws  = 1;
                    dst_cipher_desc.cipher.enc = 1;
                    dst_cipher_desc.cipher.enl = last_cache_len;
                    DBG_CRYPTO_INFO("[Set dest_desc] l_cache_line_enl = %d\r\n",last_cache_len);
                    if (pTag == NULL) {
                        dst_cipher_desc.cipher.ls = 1;
                    }
                    arch_clean_dcache_by_size(pcrypto_adapter, (size_t)pcrypto_adapter->dst_last_cache_line, 32);
                    hal_crypto_set_dstdesc_rtl8277c(pcrypto_adapter, dst_cipher_desc.w, (size_t)pcrypto_adapter->dst_last_cache_line);
                } else {
                    if (pTag == NULL) {
                        dst_cipher_desc.cipher.ls = 1;
                    }
                    arch_clean_dcache_by_size(pcrypto_adapter, (size_t)pResult, dst_cipher_desc.cipher.enl);
                    hal_crypto_set_dstdesc_rtl8277c(pcrypto_adapter ,dst_cipher_desc.w, (size_t)pResult);
                }
            }
        }

        if (pTag != NULL) {
            //Calculate cipher Tag,use adl not enc!
            dst_auth_desc.w = 0;
            /*
            if (CIPHER_TYPE_POLY1305 == (cipher_type & CIPHER_TYPE_MASK_ALL)) {
                dst_auth_desc.auth.fs = 1;
            }
            */

            if ((CIPHER_TYPE_AES_GHASH == (cipher_type & CIPHER_TYPE_MASK_ALL)) ||
                (CIPHER_TYPE_AES_GMAC  == (cipher_type & CIPHER_TYPE_MASK_ALL))) {
                dst_auth_desc.auth.fs = 1;
            }
            dst_auth_desc.auth.ws  = 1;
            dst_auth_desc.auth.enc = 0;
            dst_auth_desc.auth.adl = 16;
            dst_auth_desc.auth.ls  = 1;
            arch_clean_dcache_by_size(pcrypto_adapter, (size_t)pcrypto_adapter->tag_result, 32);
            hal_crypto_set_dstdesc_rtl8277c(pcrypto_adapter, dst_auth_desc.w, (size_t)pcrypto_adapter->tag_result);
        }
    }

    /********************************************
     * step 2: Setup source descriptor
     ********************************************/
    /********************************************
      * step 2-1: prepare Key & IV array:
      ********************************************/

    hal_crypto_engine_srcdesc_generate_cl_key_iv_rtl8277c(pcrypto_adapter, pIv, ivlen);


    /********************************************
     * step 2-2: prepare Data1 ~ DataN
     ********************************************/

    srcdesc.w = 0;
    srcdesc.d.rs = 1;

    if (paad != NULL) {
        while (a2eo > 16) {
            srcdesc.d.a2eo = 16;

            __dbg_mem_dump(pcrypto_adapter->isMemDump,paad, 16, "AAD[while-loop]: ");
            hal_crypto_set_srcdesc_rtl8277c(pcrypto_adapter, srcdesc.w, (size_t)paad);

            paad += 16;
            a2eo -= 16;
            srcdesc.w = 0;
            srcdesc.d.rs = 1;
        }

        if (a2eo > 0) {
            DBG_CRYPTO_INFO("[Set src_desc_data] a2eo = %d\r\n",a2eo);
            srcdesc.d.a2eo = a2eo;
            __dbg_mem_dump(pcrypto_adapter->isMemDump,paad, a2eo, "AAD[rest]: ");
            hal_crypto_set_srcdesc_rtl8277c(pcrypto_adapter, srcdesc.w, (size_t)paad);
            srcdesc.w = 0;
            srcdesc.d.rs = 1;
        }

        //Handle padding issue
        if (pcrypto_adapter->apl_aad > 0) {
            DBG_CRYPTO_INFO("[Set src_desc_data] apl_aad = %d\r\n",pcrypto_adapter->apl_aad);
            srcdesc.d.a2eo = pcrypto_adapter->apl_aad;
            //rtl_printf("clean padding: 0x%x \r\n", padding);
            arch_clean_dcache_by_size(pcrypto_adapter, (size_t)padding, 64);
            __dbg_mem_dump(pcrypto_adapter->isMemDump,padding, pcrypto_adapter->apl_aad, "AAD padding ");
            hal_crypto_set_srcdesc_rtl8277c(pcrypto_adapter, srcdesc.w, (size_t)padding);
            srcdesc.w = 0;
            srcdesc.d.rs = 1;
        }

    }

    {
        int8_t first_cache_pos = 0;
        int8_t first_cache_len = 0;
        int8_t last_cache_pos  = 0;
        int8_t last_cache_len  = 0;

        first_cache_pos = (size_t) message & (cache_alinge-1);
        first_cache_len = (cache_alinge - first_cache_pos) % cache_alinge;
        if ((size_t)first_cache_len >= enl) { // the same one
            first_cache_len = enl;
            last_cache_len  = 0;
        } else {
            last_cache_pos = ((size_t)(message) + enl - 1) & (cache_alinge-1);
            last_cache_len = last_cache_pos + 1;
        }

        DBG_CRYPTO_INFO("[Set src_desc_data] f_cache_pos = %d,f_cache_len = %d\r\n",first_cache_pos,first_cache_len);
        DBG_CRYPTO_INFO("[Set src_desc_data] l_cache_pos = %d,l_cache_len = %d\r\n",last_cache_pos,last_cache_len);

        if (first_cache_len != 0) {
            rtlc_memcpy((void*)(&(pcrypto_adapter->src_first_cache_line[0])), (const void*)(message), first_cache_len);

            if (((size_t)first_cache_len >= enl) && (pcrypto_adapter->apl == 0)){
                srcdesc.d.ls = 1;
            }
            srcdesc.d.enl = first_cache_len;
            DBG_CRYPTO_INFO("[Set src_desc_data] f_cache_line_enl = %d\r\n",first_cache_len);
            __dbg_mem_dump(pcrypto_adapter->isMemDump,&(pcrypto_adapter->src_first_cache_line[0]), first_cache_len, "src_data[first_cache_line]: ");
            arch_clean_dcache_by_size(pcrypto_adapter, (size_t)(&(pcrypto_adapter->src_first_cache_line[0])), cache_alinge);
            hal_crypto_set_srcdesc_rtl8277c(pcrypto_adapter, srcdesc.w, (size_t)(&(pcrypto_adapter->src_first_cache_line[0])));
            message += first_cache_len;
            enl -= first_cache_len;
            srcdesc.w = 0;
            srcdesc.d.rs = 1;
        }

        // 16352 bytes is the max size of enl could set!
        while (enl > 16352) {
            arch_clean_dcache_by_size(pcrypto_adapter, (size_t)message, 16352);
            srcdesc.d.enl = 16352;
            hal_crypto_set_srcdesc_rtl8277c(pcrypto_adapter, srcdesc.w, (size_t)message);

            message += 16352;
            enl -= 16352;
            srcdesc.w = 0;
            srcdesc.d.rs = 1;
        }

        //Cut off valid msg of last_cache_line
        //It means leave 32 bytes alignment addr of msg_body
        // message aligned body
        if (enl > (size_t)(last_cache_len)) {
            uint32_t dst_len = enl - (size_t)(last_cache_len);

            // assert
            if ((dst_len & (cache_alinge-1)) != 0) {
                DBG_CRYPTO_ERR("Strange : enl(%d), last_cache_len(%d)", enl, last_cache_len);
                return _ERRNO_CRYPTO_CACHE_HANDLE;
            }

            DBG_CRYPTO_INFO("[Set src_desc_data] dst_len = %d\r\n",dst_len);
            srcdesc.d.enl = dst_len;
            arch_clean_dcache_by_size(pcrypto_adapter, (size_t)message, srcdesc.d.enl);
            __dbg_mem_dump(pcrypto_adapter->isMemDump,message, msglen, "src_data[msg_32align_body]: ");
            hal_crypto_set_srcdesc_rtl8277c(pcrypto_adapter, srcdesc.w, (size_t)message);
            message += dst_len;
            enl -= dst_len;

            srcdesc.w = 0;
            srcdesc.d.rs = 1;
        } else if (enl < (size_t)(last_cache_len)) {
            // can change to use assert function
            DBG_CRYPTO_ERR("Strange : enl(%d), last_cache_len(%d)", enl, last_cache_len);
            return _ERRNO_CRYPTO_CACHE_HANDLE;
        }

        if (last_cache_len == cache_alinge) {
            if (pcrypto_adapter->apl == 0) {
                srcdesc.d.ls = 1;
            }
            srcdesc.d.enl = cache_alinge;
            __dbg_mem_dump(pcrypto_adapter->isMemDump,message, 32, "src_data[last_cache_len_equal_32]: ");
            arch_clean_dcache_by_size(pcrypto_adapter, (size_t)message, cache_alinge);
            hal_crypto_set_srcdesc_rtl8277c(pcrypto_adapter, srcdesc.w, (size_t)message);
        } else if (last_cache_len != 0)  {
            rtlc_memcpy((void*)(pcrypto_adapter->src_last_cache_line), (const void*)(message), last_cache_len);
            DBG_CRYPTO_INFO("[Set src_desc_data] l_cache_len(below 32) = %d\r\n",last_cache_len);
            if (pcrypto_adapter->apl == 0) {
                srcdesc.d.ls = 1;
            }
            srcdesc.d.enl = last_cache_len;
            __dbg_mem_dump(pcrypto_adapter->isMemDump,pcrypto_adapter->src_last_cache_line, last_cache_len, "src_data[last_cache_len_below_32]: ");
            arch_clean_dcache_by_size(pcrypto_adapter, (size_t)pcrypto_adapter->src_last_cache_line, cache_alinge);
            hal_crypto_set_srcdesc_rtl8277c(pcrypto_adapter, srcdesc.w, (size_t)pcrypto_adapter->src_last_cache_line);
        }

        if (pcrypto_adapter->apl != 0) {
            DBG_CRYPTO_INFO("[Set src_desc_data] pcrypto_adapter->apl = %d\r\n",pcrypto_adapter->apl);
            srcdesc.w = 0;
            srcdesc.d.rs  = 1;
            srcdesc.d.enl = pcrypto_adapter->apl;
            srcdesc.d.ls  = 1;
            arch_clean_dcache_by_size(pcrypto_adapter, (size_t)padding, 64);
            __dbg_mem_dump(pcrypto_adapter->isMemDump,padding, srcdesc.d.enl, "src_data padding ");
            hal_crypto_set_srcdesc_rtl8277c(pcrypto_adapter, srcdesc.w, (size_t)padding);
        }
    }
    ret = hal_crypto_engine_wait_done_rtl8277c(pcrypto_adapter);

    return ret;
}

/**
 *  \fn          void hal_crypto_engine_get_result_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter, u8 *pDigest, u8 *pResult,
 *                                                          uint32_t len, u8 *pTag)
 *  \brief       Get crypto cipher result/authetication digest or crypto cipher tag.
 *  \note        For normal mode
 *  \param[in]   pcrypto_adapter  Pointer to crypto adapter data
 *  \param[in]   len Crypto cipher message buffer length
 *  \param[out]  pDigest Pointer to authentication digest buffer
 *  \param[out]  pResult Pointer to crypto cipher result buffer
 *  \param[out]  pTag Pointer to crypto cipher Tag buffer
 *  \return      none
 */
void hal_crypto_engine_get_result_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter, u8 *pDigest, u8 *pResult, uint32_t len, u8 *pTag)
{
    if (pcrypto_adapter->auth_type != AUTH_TYPE_NO_AUTH) {
        arch_invalidate_dcache_by_size(pcrypto_adapter, (size_t)(pcrypto_adapter->digest_result), 32);
        __dbg_mem_dump(pcrypto_adapter->isMemDump,pcrypto_adapter->digest_result, 32, "digest_result: ");
        rtlc_memcpy((void*)(pDigest), (const void*)(pcrypto_adapter->digest_result), pcrypto_adapter->digestlen);
    } else {

        uint32_t dst_len = 0;
        uint32_t first_cache_pos = 0;
        uint32_t first_cache_len = 0;
        uint32_t last_cache_pos  = 0;
        uint32_t last_cache_len  = 0;

        first_cache_pos = (size_t)(pResult) & (cache_alinge-1);
        first_cache_len = (cache_alinge - first_cache_pos) % cache_alinge;
        if (first_cache_len >= len) { // the same one
            first_cache_len = len;
            last_cache_len  = 0;
        } else {
            last_cache_pos = ((size_t)(pResult) + len - 1) & (cache_alinge-1);
            last_cache_len = last_cache_pos + 1;
            if (last_cache_len == cache_alinge) {
                last_cache_len = 0;
            }
        }

        DBG_CRYPTO_INFO("[Get cipher_result] f_cache_pos = %d,f_cache_len = %d\r\n",first_cache_pos,first_cache_len);
        DBG_CRYPTO_INFO("[Get cipher_result] l_cache_pos = %d,l_cache_len = %d\r\n",last_cache_pos,last_cache_len);

        if (pcrypto_adapter->is_dst_first_cache_used != 0) {
            arch_invalidate_dcache_by_size(pcrypto_adapter, (size_t)(pcrypto_adapter->dst_first_cache_line), cache_alinge);
            rtlc_memcpy((void*)(pResult), (const void*)(pcrypto_adapter->dst_first_cache_line), first_cache_len);
            dst_len += first_cache_len;
        }

        //32 byte aligned msg body which doesn't need rtlc_memcpy
        if (len > (dst_len + last_cache_len)) {
            DBG_CRYPTO_INFO("[Get cipher_result] msg_body_len = %d\r\n",(len - dst_len - last_cache_len));
            //rtl_printf("arch_invalidate - %d \r\n", (len-dst_len-last_cache_len));
            arch_invalidate_dcache_by_size(pcrypto_adapter, (size_t)(pResult) + dst_len, (len - dst_len - last_cache_len));
        }

        //Handle last_cache_line
        dst_len = len - last_cache_len;
        if (pcrypto_adapter->is_dst_last_cache_used != 0) {
            arch_invalidate_dcache_by_size(pcrypto_adapter, (size_t)(pcrypto_adapter->dst_last_cache_line), cache_alinge);
            rtlc_memcpy((void*)(pResult + dst_len), (const void*)(pcrypto_adapter->dst_last_cache_line), last_cache_len);
        }
    }
    if (pTag != NULL) {
        arch_invalidate_dcache_by_size(pcrypto_adapter, (size_t)(pcrypto_adapter->tag_result), sizeof(pcrypto_adapter->tag_result));
        rtlc_memcpy((void*)pTag, (const void*)(pcrypto_adapter->tag_result), 16);
    }
}

#if (CHIP_VER == CHIP_A_CUT)
/**
 *  \fn          int hal_crypto_send_seq_buf_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter, u8 *pDigest)
 *  \brief       A workaround for sequential hash to make sure crypto engine handle message buffer length as 64 bytes-aligned,
 *               except the last one is 16 bytes-aligned.
 *  \param[in]   pcrypto_adapter  Pointer to crypto adapter data
 *  \param[out]  pDigest Pointer to authentication digest buffer
 *  \return      value == 0     success
 *  \return      value < 0      fail(error code)
 */
// Note:
// This way can not handles re-used same ram buffer
// two buffers : last_message / seq_buf
//    last_message : store the previous message pointer
//    seq_buf : store the data less than 64
//
int hal_crypto_send_seq_buf_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter, u8 *pDigest)
{
    int ret = SUCCESS;
    //const u8 *pIV 	= NULL;
    //const u32 ivlen = 0;
    //const u32 a2eo 	= 0;

    int total_len  = pcrypto_adapter->hmac_seq_last_msglen;
    int buf_pos    = pcrypto_adapter->hmac_seq_buf_is_used_bytes;
    int rest_bytes = (64 - buf_pos);
    int bodylen, restlen;

    // There's an enough space of seq_buf for total_len
    if (total_len < rest_bytes) {
        rtlc_memcpy((void*)(&pcrypto_adapter->hmac_seq_buf[buf_pos]), (const void*)(pcrypto_adapter->hmac_seq_last_message), total_len);
        pcrypto_adapter->hmac_seq_buf_is_used_bytes += total_len;
    } else {
        /***********************************************************
         * send out all the hmac_seq_last_message or store into buf
         ***********************************************************/
        // Make up seq_buf
        rtlc_memcpy((void*)(&pcrypto_adapter->hmac_seq_buf[buf_pos]), (const void*)(pcrypto_adapter->hmac_seq_last_message), rest_bytes);
        ret = hal_crypto_engine_rtl8277c(pcrypto_adapter, (u8*)(pcrypto_adapter->hmac_seq_buf), 64, NULL, 0, NULL, 0, (total_len == rest_bytes)?pDigest:NULL, NULL);
        if (ret != SUCCESS) {
            return ret;
        }

        pcrypto_adapter->hmac_seq_buf_is_used_bytes = 0;
        buf_pos = 0;

        total_len -= rest_bytes;
        pcrypto_adapter->hmac_seq_last_msglen = total_len;
        pcrypto_adapter->hmac_seq_last_message += rest_bytes;

        // message(distinguish restlen[non_64bytes aligned] & bodylen[64bytes aligned])
        restlen = total_len & 0x3F;
        bodylen = total_len - restlen;
        if (bodylen > 0) { // there are 64x messages
            ret = hal_crypto_engine_rtl8277c(pcrypto_adapter, pcrypto_adapter->hmac_seq_last_message, bodylen, NULL, 0, NULL, 0, (restlen==0)?pDigest:NULL, NULL);
            if (ret != SUCCESS) {
                return ret;
            }
            pcrypto_adapter->hmac_seq_last_message += bodylen;
        }

        // backup the rest into seq_buf
        if (restlen > 0) {
            rtlc_memcpy((void*)(&pcrypto_adapter->hmac_seq_buf[0]), (const void*)(pcrypto_adapter->hmac_seq_last_message), restlen);
        }
        pcrypto_adapter->hmac_seq_buf_is_used_bytes = restlen;
    }

    if ((pDigest != NULL) &&
        (pcrypto_adapter->hmac_seq_buf_is_used_bytes > 0)) { // last one put into cryptoEngine, may need to pad 16bytes aligned msglen
        ret = hal_crypto_engine_rtl8277c(pcrypto_adapter, (u8*)(pcrypto_adapter->hmac_seq_buf), pcrypto_adapter->hmac_seq_buf_is_used_bytes, NULL, 0, NULL, 0, pDigest, NULL);
    }

    return ret;
}

#else
/**
 *  \fn          int hal_crypto_send_seq_buf_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter, u8 *pDigest)
 *  \brief       A workaround for sequential hash to make sure crypto engine handle message buffer length as 64 bytes-aligned,
 *               except the last one is 16 bytes-aligned.
 *  \param[in]   pcrypto_adapter  Pointer to crypto adapter data
 *  \param[out]  pDigest Pointer to authentication digest buffer
 *  \return      value == 0     success
 *  \return      value < 0      fail(error code)
 */
// Note:
// two buffers : last_message / seq_buf
//    last_message : store the previous message pointer
//    seq_buf : store the data less than 64
//
int hal_crypto_send_seq_buf_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter, u8 *pDigest)
{
    int ret = SUCCESS;
    //const u8 *pIV 	= NULL;
    //const u32 ivlen = 0;
    //const u32 a2eo 	= 0;

    int total_len  = pcrypto_adapter->hmac_seq_last_msglen;
    int buf_pos    = pcrypto_adapter->hmac_seq_buf_is_used_bytes;
    int rest_bytes = (64 - buf_pos);
    int bodylen, restlen;

    DBG_CRYPTO_INFO("[SEQ_BUF] total_len = %d, buf_pos = %d, rest_bytes = %d\r\n",total_len, buf_pos, rest_bytes);
    if (pDigest != NULL) { // Make sure it's called from final api
        if (pcrypto_adapter->hmac_seq_buf_is_used_bytes > 0) { // last one put into cryptoEngine, may need to pad 16bytes aligned msglen
            ret = hal_crypto_engine_rtl8277c(pcrypto_adapter, (u8*)(pcrypto_adapter->hmac_seq_buf), pcrypto_adapter->hmac_seq_buf_is_used_bytes, NULL, 0, NULL, 0, pDigest, NULL);
        }
        pcrypto_adapter->hmac_seq_buf_is_used_bytes = 0;
    } else {
        if (rest_bytes == 0) { // There's no enough space for seq_buf
            ret = hal_crypto_engine_rtl8277c(pcrypto_adapter, (u8*)(pcrypto_adapter->hmac_seq_buf), 64, NULL, 0, NULL, 0, NULL, NULL);
            if (ret != SUCCESS) {
                DBG_CRYPTO_ERR("Handle rest_bytes = 0 fail\r\n");
                return ret;
            }
            pcrypto_adapter->hmac_seq_buf_is_used_bytes = 0;
            buf_pos = 0;
            rest_bytes = (64 - buf_pos);
        }

        // There's an enough space of seq_buf for total_len
        if (total_len < rest_bytes) {
            rtlc_memcpy((void*)(&pcrypto_adapter->hmac_seq_buf[buf_pos]), (const void*)(pcrypto_adapter->hmac_seq_last_message), total_len);
            pcrypto_adapter->hmac_seq_buf_is_used_bytes += total_len;
        } else {
            /***********************************************************
             * send out all the hmac_seq_last_message or store into buf
             ***********************************************************/
            if ((total_len - rest_bytes) == 0) {
                // Make up seq_buf, leve next round to handle full data of seq_buf
                rtlc_memcpy((void*)(&pcrypto_adapter->hmac_seq_buf[buf_pos]), (const void*)(pcrypto_adapter->hmac_seq_last_message), rest_bytes);

                pcrypto_adapter->hmac_seq_buf_is_used_bytes = 64;
                total_len -= rest_bytes;
                if (total_len == 0) {
                    pcrypto_adapter->hmac_seq_last_msglen = total_len;
                    pcrypto_adapter->hmac_seq_last_message = NULL;
                } else {
                    DBG_CRYPTO_ERR("seq_hash wrong length 1: total_len = %d, rest_bytes = %d\r\n",total_len,rest_bytes);
                    return _ERRNO_CRYPTO_HASH_SEQUENTIAL_HASH_WORNG_LENGTH;
                }
            } else if ((total_len - rest_bytes) > 0) {
                 // Make up seq_buf
                rtlc_memcpy((void*)(&pcrypto_adapter->hmac_seq_buf[buf_pos]), (const void*)(pcrypto_adapter->hmac_seq_last_message), rest_bytes);
                ret = hal_crypto_engine_rtl8277c(pcrypto_adapter, (u8*)(pcrypto_adapter->hmac_seq_buf), 64, NULL, 0, NULL, 0, NULL, NULL);
                if (ret != SUCCESS) {
                    return ret;
                }

                pcrypto_adapter->hmac_seq_buf_is_used_bytes = 0;
                buf_pos = 0;

                total_len -= rest_bytes;
                pcrypto_adapter->hmac_seq_last_msglen = total_len;
                pcrypto_adapter->hmac_seq_last_message += rest_bytes;

                // message(distinguish restlen[non_64bytes aligned] & bodylen[64bytes aligned])
                restlen = total_len & 0x3F;
                bodylen = total_len - restlen;
                DBG_CRYPTO_INFO("[SEQ_BUF] total_len = %d, bodylen = %d, restlen = %d\r\n",total_len, bodylen, restlen);
                if ((bodylen == 0) && (restlen > 0)) { // there are only restlen, leave next round to handle seq_buf
                    rtlc_memcpy((void*)(&pcrypto_adapter->hmac_seq_buf[0]), (const void*)(pcrypto_adapter->hmac_seq_last_message), restlen);
                    total_len -= restlen;
                    if (total_len == 0) {
                        pcrypto_adapter->hmac_seq_last_msglen = total_len;
                        pcrypto_adapter->hmac_seq_last_message = NULL;
                        pcrypto_adapter->hmac_seq_buf_is_used_bytes = restlen;
                    } else {
                        DBG_CRYPTO_ERR("seq_hash wrong length 2: total_len = %d, bodylen = %d, restlen = %d\r\n",total_len, bodylen, restlen);
                        return _ERRNO_CRYPTO_HASH_SEQUENTIAL_HASH_WORNG_LENGTH;
                    }
                } else if ((bodylen > 0) && (restlen > 0)) { // there are 64x messages and restlen, leave next round to handle restlen
                    ret = hal_crypto_engine_rtl8277c(pcrypto_adapter, pcrypto_adapter->hmac_seq_last_message, bodylen, NULL, 0, NULL, 0, NULL, NULL);
                    if (ret != SUCCESS) {
                        return ret;
                    }
                    pcrypto_adapter->hmac_seq_last_message += bodylen;
                    total_len -= bodylen;

                    // backup the rest into seq_buf, leave next round to handle it
                    rtlc_memcpy((void*)(&pcrypto_adapter->hmac_seq_buf[0]), (const void*)(pcrypto_adapter->hmac_seq_last_message), restlen);
                    total_len -= restlen;
                    if (total_len == 0) {
                        pcrypto_adapter->hmac_seq_last_msglen = total_len;
                        pcrypto_adapter->hmac_seq_last_message = NULL;
                        pcrypto_adapter->hmac_seq_buf_is_used_bytes = restlen;
                    } else {
                        DBG_CRYPTO_ERR("seq_hash wrong length 3: total_len = %d, bodylen = %d, restlen = %d\r\n",total_len, bodylen, restlen);
                        return _ERRNO_CRYPTO_HASH_SEQUENTIAL_HASH_WORNG_LENGTH;
                    }
                } else if ((bodylen == 64) && (restlen == 0)) {
                    // there are only 64byte message, backup the bodylen into seq_buf, leave next round to handle it
                    rtlc_memcpy((void*)(&pcrypto_adapter->hmac_seq_buf[0]), (const void*)(pcrypto_adapter->hmac_seq_last_message), bodylen);
                    total_len -= bodylen;
                    if (total_len == 0) {
                        pcrypto_adapter->hmac_seq_last_msglen = total_len;
                        pcrypto_adapter->hmac_seq_last_message = NULL;
                        pcrypto_adapter->hmac_seq_buf_is_used_bytes = bodylen;
                    } else {
                        DBG_CRYPTO_ERR("seq_hash wrong length 4: total_len = %d, bodylen = %d, restlen = %d\r\n",total_len, bodylen, restlen);
                        return _ERRNO_CRYPTO_HASH_SEQUENTIAL_HASH_WORNG_LENGTH;
                    }
                } else if ((bodylen > 64) && (restlen == 0)) { // there are only 64x messages, leave next round to handle full data of seq_buf
                    ret = hal_crypto_engine_rtl8277c(pcrypto_adapter, pcrypto_adapter->hmac_seq_last_message, (bodylen - 64), NULL, 0, NULL, 0, NULL, NULL);
                    if (ret != SUCCESS) {
                        return ret;
                    }
                    pcrypto_adapter->hmac_seq_last_message += (bodylen - 64);

                    // backup the rest into seq_buf, leave next round to handle it
                    total_len -= (bodylen - 64);
                    pcrypto_adapter->hmac_seq_last_msglen = total_len;

                    if (total_len == 64){
                        rtlc_memcpy((void*)(&pcrypto_adapter->hmac_seq_buf[0]), (const void*)(pcrypto_adapter->hmac_seq_last_message), total_len);
                        pcrypto_adapter->hmac_seq_last_msglen = (total_len - 64);
                        pcrypto_adapter->hmac_seq_last_message = NULL;
                        pcrypto_adapter->hmac_seq_buf_is_used_bytes = 64;
                    } else {
                        DBG_CRYPTO_ERR("seq_hash wrong length 5: total_len = %d, bodylen = %d, restlen = %d\r\n",total_len, bodylen, restlen);
                        return _ERRNO_CRYPTO_HASH_SEQUENTIAL_HASH_WORNG_LENGTH;
                    }
                }
            }
        }
    }
    return ret;
}
#endif

/**
 *  \fn          void hal_crypto_engine_set_encrypt_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter)
 *  \brief       Set encrypt information in crypto adapter data.
 *  \param[in]   pcrypto_adapter  Pointer to crypto adapter data
 *  \return      none
 */
void hal_crypto_engine_set_encrypt_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter)
{
    pcrypto_adapter->cipher_type |= CIPHER_TYPE_MODE_ENCRYPT;
    pcrypto_adapter->isDecrypt = _FALSE;
}

/**
 *  \fn          void hal_crypto_engine_set_decrypt_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter)
 *  \brief       Set decrypt information in crypto adapter data.
 *  \param[in]   pcrypto_adapter  Pointer to crypto adapter data
 *  \return      none
 */
void hal_crypto_engine_set_decrypt_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter)
{
    pcrypto_adapter->cipher_type ^= CIPHER_TYPE_MODE_ENCRYPT;
    pcrypto_adapter->isDecrypt = _TRUE;
}

/**
 *  \fn          int hal_crypto_cipher_process_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter,
 *                                                      IN const u8 isEncrypt,  IN const u32 cipher_type,
 *                                                      IN const u8 *message, 	IN const u32 msglen,
 *                                                      IN const u8 *iv, 		IN const u32 ivlen,
 *                                                      IN const u8 *aad,		IN const u32 aadlen,
 *                                                      OUT u8 *pResult,        OUT u8 *pTag)
 *  \brief       Check input parameters which cipher needs, and make crypto engine encrypt the plaintext or decrypt the ciphertext.
 *               After that, get the cipher result.
 *  \param[in]   pcrypto_adapter  Pointer to crypto adapter data
 *  \param[in]   isEncrypt Indicate that crypto engine now is processing encryption or decryption
 *  \param[in]   cipher_type  Cipher type ID
 *  \param[in]   message Pointer to Crypto cipher message buffer
 *  \param[in]   msglen Crypto cipher message buffer length
 *  \param[in]   iv Pointer to Crypto cipher initial vector buffer
 *  \param[in]   ivlen Crypto cipher initial vector buffer length
 *  \param[in]   aad Pointer to Crypto cipher additional authentication buffer
 *  \param[in]   aadlen Crypto cipher additional authentication buffer length
 *  \param[out]  pResult Pointer to Crypto cipher result buffer
 *  \param[out]  pTag Pointer to Crypto cipher Tag buffer
 *  \return      value == 0     success
 *  \return      value < 0      fail(error code)
 */
int hal_crypto_cipher_process_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter,
    IN const u8 isEncrypt,  IN const u32 cipher_type,
    IN const u8 *message, 	IN const u32 msglen,
    IN const u8 *iv, 		IN const u32 ivlen,
    IN const u8 *aad,		IN const u32 aadlen,
    OUT u8 *pResult,        OUT u8 *pTag)
{
    int ret = SUCCESS;

    if (pcrypto_adapter == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (check_4byte_align((size_t)(iv))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if (check_4byte_align((size_t)(aad))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }

    do {
        if (pcrypto_adapter->isInit != _TRUE) {
            ret = _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
            break;
        }

        if (message == NULL) {
            DBG_CRYPTO_ERR("message is null pointer\r\n");
            ret = _ERRNO_CRYPTO_NULL_POINTER;
            break;
        }

        if (CIPHER_TYPE_FUNC_AES == (cipher_type & CIPHER_TYPE_MASK_FUNC)) { //AES
            if (CIPHER_TYPE_AES_GHASH == cipher_type) {
                if (pTag == NULL) {
                    DBG_CRYPTO_ERR("pTag is null pointer\r\n");
                    ret = _ERRNO_CRYPTO_NULL_POINTER;
                    break;
                }
            } else if (CIPHER_TYPE_AES_GMAC == cipher_type) {
                if (pTag == NULL) {
                    DBG_CRYPTO_ERR("pTag is null pointer\r\n");
                    ret = _ERRNO_CRYPTO_NULL_POINTER;
                    break;
                }
            } else {
                DBG_CRYPTO_INFO("msglen non_16_aligned %d bytes\r\n",(msglen % 16));
                if (((msglen % 16) != 0) && (!isEncrypt)) {
                    ret = _ERRNO_CRYPTO_AES_MSGLEN_NOT_16Byte_Aligned;
                    break;
                }
                if (pResult == NULL) {
                    DBG_CRYPTO_ERR("pResult is null pointer\r\n");
                    ret = _ERRNO_CRYPTO_NULL_POINTER;
                    break;
                }
                if (CIPHER_TYPE_AES_GCM == cipher_type) {
                    if (pTag == NULL) {
                        DBG_CRYPTO_ERR("pTag is null pointer\r\n");
                        ret = _ERRNO_CRYPTO_NULL_POINTER;
                        break;
                    }
                }
            }
        }

        if ((CIPHER_TYPE_FUNC_DES == (cipher_type & CIPHER_TYPE_MASK_FUNC)) || (CIPHER_TYPE_FUNC_3DES == (cipher_type & CIPHER_TYPE_MASK_FUNC)))
        {
            if (((msglen % 8) != 0) && (!isEncrypt)) {
                DBG_CRYPTO_ERR("msglen non_8_aligned %d bytes\r\n",(msglen % 8));
                ret = _ERRNO_CRYPTO_DES_MSGLEN_NOT_8Byte_Aligned;
                break;
            }
        }

        if (CIPHER_TYPE_AES_GCM == cipher_type) {
            rtlc_memcpy((void*)(pcrypto_adapter->gcm_iv), (const void*)(iv), 96/8);
            rtlc_memcpy((void*)(&(pcrypto_adapter->gcm_iv)[12]), (const void*)(gcm_iv_tail), 4);

            if (isEncrypt) {
                hal_crypto_engine_set_encrypt_rtl8277c(pcrypto_adapter);
                ret = hal_crypto_engine_rtl8277c(pcrypto_adapter, message, msglen,
                                                 (u8*)(pcrypto_adapter->gcm_iv), 16, aad, aadlen, pResult, pTag);
            } else {
                hal_crypto_engine_set_decrypt_rtl8277c(pcrypto_adapter);
                ret = hal_crypto_engine_rtl8277c(pcrypto_adapter, message, msglen,
                                                 (u8*)(pcrypto_adapter->gcm_iv), 16, aad, aadlen, pResult, pTag);
            }
        } else if (CIPHER_TYPE_AES_GCTR == cipher_type) {
            rtlc_memcpy((void*)(pcrypto_adapter->gcm_iv), (const void*)(iv), 96/8);
            rtlc_memcpy((void*)(&(pcrypto_adapter->gcm_iv)[12]), (const void*)(gcm_iv_tail), 4);

            if (isEncrypt) {
                hal_crypto_engine_set_encrypt_rtl8277c(pcrypto_adapter);
                ret = hal_crypto_engine_rtl8277c(pcrypto_adapter, message, msglen,
                                                 (u8*)(pcrypto_adapter->gcm_iv), 16, NULL, 0, pResult, NULL);
            } else {
                hal_crypto_engine_set_decrypt_rtl8277c(pcrypto_adapter);
                ret = hal_crypto_engine_rtl8277c(pcrypto_adapter, message, msglen,
                                                 (u8*)(pcrypto_adapter->gcm_iv), 16, NULL, 0, pResult, NULL);
            }
        } else if (CIPHER_TYPE_AES_GHASH == cipher_type) {
            hal_crypto_engine_set_decrypt_rtl8277c(pcrypto_adapter);
            ret = hal_crypto_engine_rtl8277c(pcrypto_adapter, message, msglen, iv, ivlen, NULL, 0, pResult, pTag);
        } else if (CIPHER_TYPE_AES_GMAC == cipher_type) {
            rtlc_memcpy((void*)(pcrypto_adapter->gcm_iv), (const void*)(iv), 96/8);
            rtlc_memcpy((void*)(&(pcrypto_adapter->gcm_iv)[12]), (const void*)(gcm_iv_tail), 4);
            hal_crypto_engine_set_decrypt_rtl8277c(pcrypto_adapter);
            ret = hal_crypto_engine_rtl8277c(pcrypto_adapter, message, msglen,
                                             (u8*)(pcrypto_adapter->gcm_iv), 16, aad, aadlen, NULL, pTag);
        } else {
            if (isEncrypt) {
                hal_crypto_engine_set_encrypt_rtl8277c(pcrypto_adapter);
            } else {
                hal_crypto_engine_set_decrypt_rtl8277c(pcrypto_adapter);
            }
            ret = hal_crypto_engine_rtl8277c(pcrypto_adapter, message, msglen, iv, ivlen, NULL, 0, pResult, NULL);
        }

        if (ret != SUCCESS) {
            break;
        }

        if (isEncrypt) {
            //Encryption may need to modify msglen of destination descriptor,because of padding length
            hal_crypto_engine_get_result_rtl8277c(pcrypto_adapter, NULL, pResult, (msglen + (pcrypto_adapter->apl)), pTag);
        } else {
            hal_crypto_engine_get_result_rtl8277c(pcrypto_adapter, NULL, pResult, msglen, pTag);
        }
    } while (0);

    return ret;
}

/**
 *  \fn          void hal_crypto_engine_reset_rtl8277c(phal_crypto_adapter_t pcrypto_adapter)
 *  \brief       Reset crypto engine DMA arbiter mode, clock, swap setting, endian setting, DMA burst length setting.
 *  \param[in]   pcrypto_adapter  Pointer to crypto adapter data
 *  \return      none
 */
void hal_crypto_engine_reset_rtl8277c(phal_crypto_adapter_t pcrypto_adapter)
{
    CRYPTO_Type *pcrypto;
    pcrypto = pcrypto_adapter->base_addr;

    // Crypto engine : Software Reset
    (pcrypto->ipscsr_reset_isr_conf_reg_b.soft_rst) = 1;

#if (LEXRA_BIG_ENDIAN == 1)
    {
        crypto_ipscsr_swap_burst_reg_t val;
        val.w = 0;
        val.b.set_swap = 1;
        //val.b.dma_burst_length = burstSize;
        val.b.dma_burst_length = 16;
        (pcrypto->ipscsr_swap_burst_reg) = val.w;
    }
#else
    {
        crypto_ipscsr_swap_burst_reg_t val;
        val.w = 0;
        val.b.key_iv_swap = 1;
        val.b.key_pad_swap = 1;
        val.b.hash_inital_value_swap = 1;
        val.b.dma_in_little_endian = 1;
        val.b.data_out_little_endian = 1;
        val.b.mac_out_little_endian = 1;
        val.b.dma_burst_length = 16;
        (pcrypto->ipscsr_swap_burst_reg) = val.w;
    }
#endif
	pcrypto->ipscsr_int_mask_reg = 0;
    // Crypto Engine : DMA arbiter , clock enable
    {
        crypto_ipscsr_debug_reg_t val;

        val.w = 0;
        val.b.arbiter_mode = 1;
        val.b.engine_clk_en = 1;
        (pcrypto->ipscsr_debug_reg) = val.w;
    }
}

/**
 *  \fn          void hal_crypto_en_ctrl_rtl8277c(int en)
 *  \brief       Enable/Disable the clock of IPsec.
 *  \param[in]   en Set the clock of IPsec state: 1=Enable, 0=Disable
 *  \return      none
 */
void hal_crypto_en_ctrl_rtl8277c(int en)
{
    // YR syson_soc_func_en_t platform_func_ctrl;
    // YR syson_pesoc_com_clk_ctrl1_t platform_clk_ctrl;

    // YR platform_func_ctrl.w = SYSON->soc_func_en;
    // YR platform_clk_ctrl.w  = SYSON->pesoc_com_clk_ctrl1;
    if (en) {
        DBG_CRYPTO_INFO("[Enable lxbus clk & hp_ipsec clk...]\r\n");
        //platform_ctrl.b.lxbus_en = 1;
        //platform_ctrl.b.lxbus_clk_en = 1;
        // YR platform_func_ctrl.b.soc_sec_eng_en = 1;
        // YR platform_clk_ctrl.b.soc_clk_security_eng_en = 1;
    } else {
        DBG_CRYPTO_INFO("[Disable lxbus clk & hp_ipsec clk...]\r\n");
        //platform_ctrl.b.lxbus_en = 0;
        //platform_ctrl.b.lxbus_clk_en = 0;
        // YR platform_func_ctrl.b.soc_sec_eng_en = 0;
        // YR platform_clk_ctrl.b.soc_clk_security_eng_en = 0;
    }
    // YR SYSON->soc_func_en = platform_func_ctrl.w;
    // YR SYSON->pesoc_com_clk_ctrl1 = platform_clk_ctrl.w;
}

/**
 *  \fn          int hal_crypto_engine_init_rtl8277c(phal_crypto_adapter_t pcrypto_adapter)
 *  \brief       Enable the clock and bus related IPsec and reset crypto engine.
 *  \param[in]   pcrypto_adapter  Pointer to crypto adapter data
 *  \return      value == 0     success
 *  \return      value < 0      fail(error code)
 */
int hal_crypto_engine_init_rtl8277c(phal_crypto_adapter_t pcrypto_adapter)
{
    int ret = SUCCESS;
    if (pcrypto_adapter == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    pcrypto_adapter->base_addr = CRYPTO_MODULE;
    rtlc_memset((u8 *)(&(pcrypto_adapter->padding[0])), 0x0, CRYPTO_PADSIZE);

    if (INIT_REG_SOMEONE == reg_init) {
        //CRYPTO_ADAPTER = pcrypto_adapter;
        pcrypto_adapter->isInit = 1;
        pcrypto_adapter->isIntMode = 0;
        ret = SUCCESS;
        DBG_CRYPTO_INFO("CryptoEngine initializes: isInit = %d, isIntMode = %d\r\n",pcrypto_adapter->isInit,pcrypto_adapter->isIntMode);
    } else {
        if (!((pcrypto_adapter->initmap) & CHECK_INIT_FROM_HAL)) {
            hal_crypto_en_ctrl_rtl8277c(ENABLE_PLATFORM);
        }
        hal_crypto_engine_reset_rtl8277c(pcrypto_adapter);
        pcrypto_adapter->isInit = 1;
        pcrypto_adapter->isIntMode = 0;
        DBG_CRYPTO_INFO("After CryptoEngine reset: isInit = %d, isIntMode = %d\r\n",pcrypto_adapter->isInit,pcrypto_adapter->isIntMode);
        reg_init = INIT_REG_SOMEONE;
    }
    DBG_CRYPTO_INFO("[Register someone...] reg_init = 0x%x\r\n",reg_init);
    DBG_CRYPTO_INFO("[ROM code Init CryptoEngine success...]\r\n");
    //__rtl_crypto_irq_enable(pcrypto_adapter, g_crypto_handler);

    return ret;
}

/**
 *  \fn          int hal_crypto_engine_deinit_rtl8277c(phal_crypto_adapter_t pcrypto_adapter)
 *  \brief       Disable crypto engine interrupt, the clock and bus related IPsec.
 *  \param[in]   pcrypto_adapter  Pointer to crypto adapter data
 *  \return      value == 0     success
 *  \return      value < 0      fail(error code)
 */
int hal_crypto_engine_deinit_rtl8277c(phal_crypto_adapter_t pcrypto_adapter)
{
    int ret = SUCCESS;
    CRYPTO_Type *pcrypto;
    if (pcrypto_adapter == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    pcrypto = pcrypto_adapter->base_addr;
    if (pcrypto_adapter->isInit == _TRUE) {
        if (INIT_REG_SOMEONE == reg_init) {
            //Disable Crypto interrupt(enable interrupt mask), no matter whether interrupt registered before
            (pcrypto->ipscsr_int_mask_reg_b.cmd_ok_m) = 1;

#if defined(CONFIG_BUILD_NONSECURE) && (CONFIG_BUILD_NONSECURE == 1)
            // YR hal_irq_disable_rtl8277c (Crypto_IRQn);
#else
            // YR hal_irq_disable_rtl8277c (SCrypto_IRQn);
#endif
            // YR __ISB();

            //Disable clock
            if (!((pcrypto_adapter->initmap) & CHECK_INIT_FROM_HAL)) {
                hal_crypto_en_ctrl_rtl8277c(DISABLE_PLATFORM);
            }
        }
        reg_init = INIT_REG_NOONE;
        pcrypto_adapter->isIntMode = 0;
        pcrypto_adapter->isInit = 0;
        pcrypto_adapter->base_addr = NULL;
        //CRYPTO_ADAPTER = NULL;
        DBG_CRYPTO_INFO("[Deregister someone...] reg_init = 0x%x\r\n",reg_init);
        DBG_CRYPTO_INFO("[ROM code DeInit CryptoEngine success...] isIntMode = %d, isInit = %d\r\n",pcrypto_adapter->isIntMode,pcrypto_adapter->isInit);
    }else {
        DBG_CRYPTO_WARN("[Crypto engine doesn't initialize...]\r\n");
    }
    return ret;
}


//AUTH
/**
 *  \fn          int hal_crypto_auth_init_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter, IN const u32 auth_type,
 *                                                 IN const u8 *pAuthKey, IN const u32 lenAuthKey)
 *  \brief       Check input parameters which authentication Hash/HMAC needs,
 *               and set related authentication Hash/HMAC initial values to crypto adapter.
 *  \note
 *               auth_type =
 *               {
 *                  AUTH_TYPE_MD5,
 *                  AUTH_TYPE_SHA1,
 *                  AUTH_TYPE_SHA2_224_ALL,
 *                  AUTH_TYPE_SHA2_256_ALL,
 *                  AUTH_TYPE_HMAC_MD5,
 *                  AUTH_TYPE_HMAC_SHA1,
 *                  AUTH_TYPE_HMAC_SHA2_224_ALL,
 *                  AUTH_TYPE_HMAC_SHA2_256_ALL
 *               }
 *  \param[in]   pcrypto_adapter  Pointer to crypto adapter data
 *  \param[in]   auth_type Hash/HMAC type ID
 *  \param[in]   pAuthKey Pointer to authentication key buffer
 *  \param[in]   lenAuthKey Authentication key buffer length
 *  \return      value == 0     success
 *  \return      value < 0      fail(error code)
 */
int hal_crypto_auth_init_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter, IN const u32 auth_type,
                                  IN const u8 *pAuthKey, IN const u32 lenAuthKey)
{
    int ret = SUCCESS;
    const u8 *pCipherKey 	= NULL;
    const u32 lenCipherKey 	= 0;

    if (pcrypto_adapter == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }

    // for sequential hash
    pcrypto_adapter->hmac_seq_is_recorded = 0;
    pcrypto_adapter->hmac_seq_buf_is_used_bytes = 0;

    if (auth_type & AUTH_TYPE_MASK_HMAC) {
        if (pAuthKey == NULL) {
            return _ERRNO_CRYPTO_NULL_POINTER;
        }
        if (check_4byte_align((size_t)(pAuthKey))) {
            return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
        }
        if (lenAuthKey == 0) {
            return _ERRNO_CRYPTO_KEY_OutRange;
        }
        if (lenAuthKey > CRYPTO_AUTH_PADDING) {
            return _ERRNO_CRYPTO_KEY_OutRange;
        }
        ret = hal_crypto_engine_set_security_mode_rtl8277c(pcrypto_adapter, CIPHER_TYPE_NO_CIPHER, auth_type,
                                                           pCipherKey, lenCipherKey, pAuthKey, lenAuthKey);
    } else {
        ret = hal_crypto_engine_set_security_mode_rtl8277c(pcrypto_adapter, CIPHER_TYPE_NO_CIPHER, auth_type,
                                                           pCipherKey, lenCipherKey, NULL, 0);
    }

    return ret;
}

/**
 *  \fn          int hal_crypto_auth_process_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter, IN const u32 auth_type,
 *                                                    IN const u8 *message, IN const u32 msglen, OUT u8 *pDigest)
 *  \brief       Check input parameters which authentication Hash/HMAC needs,
 *               and make crypto engine run Hash/HMAC algorithm. After that, get the hash digest.
 *  \param[in]   pcrypto_adapter  Pointer to crypto adapter data
 *  \param[in]   auth_type Hash/HMAC type ID
 *  \param[in]   message Pointer to authentication message buffer
 *  \param[in]   msglen Authentication message buffer length
 *  \param[out]  pDigest Pointer to authentication digest buffer
 *  \return      value == 0     success
 *  \return      value < 0      fail(error code)
 */
int hal_crypto_auth_process_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter, IN const u32 auth_type,
                                     IN const u8 *message, IN const u32 msglen, OUT u8 *pDigest)
{
    int ret = SUCCESS;

    if (pcrypto_adapter == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (pDigest == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if ((pcrypto_adapter->auth_type & AUTH_TYPE_MASK_FUNC_ALL) != (auth_type & AUTH_TYPE_MASK_FUNC_ALL)) {
        return _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH;
    }
    if (message == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (msglen == 0) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }
    //if ( msglen > CRYPTO_MAX_MSG_LENGTH ) return _ERRNO_CRYPTO_MSG_OutRange;

    ret = hal_crypto_engine_rtl8277c(pcrypto_adapter, message, msglen, NULL, 0, NULL, 0, pDigest, NULL);
    if (ret != SUCCESS) {
        return ret;
    }

    hal_crypto_engine_get_result_rtl8277c(pcrypto_adapter, pDigest, NULL, 0, NULL);
    return ret;
}

#if (CHIP_VER == CHIP_A_CUT)
/**
 *  \fn          int hal_crypto_auth_update_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter, IN const u32 auth_type,
 *                                                   IN const u8 *message, IN const u32 msglen)
 *  \brief       Check input parameters which authentication needs,and use a sequential buffer to help crypto engine
 *               to pre-process authentication message buffer.After that, update related sequential hash data in crypto adapter.
 *  \param[in]   pcrypto_adapter  Pointer to crypto adapter data
 *  \param[in]   auth_type Hash/HMAC type ID
 *  \param[in]   message Pointer to authentication message buffer
 *  \param[in]   msglen Authentication message buffer length
 *  \return      value == 0     success
 *  \return      value < 0      fail(error code)
 *
 *  \Note        This way can not handles re-used same ram buffer
 */
int hal_crypto_auth_update_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter, IN const u32 auth_type,
                                    IN const u8 *message, IN const u32 msglen)
{
    int ret = SUCCESS;
    if (pcrypto_adapter == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (message == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    //if ( msglen > CRYPTO_MAX_MSG_LENGTH ) return _ERRNO_CRYPTO_MSG_OutRange;

    if ((pcrypto_adapter->auth_type & AUTH_TYPE_MASK_FUNC_ALL) != (auth_type & AUTH_TYPE_MASK_FUNC_ALL)) {
        return _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH;
    }
    if (pcrypto_adapter->hmac_seq_is_recorded != 0) {
        ret = hal_crypto_send_seq_buf_rtl8277c(pcrypto_adapter, NULL);
        if (ret != SUCCESS) {
            return ret;
        }
    }
    pcrypto_adapter->hmac_seq_last_message = (u8*)message;
    pcrypto_adapter->hmac_seq_last_msglen  = msglen;
    pcrypto_adapter->hmac_seq_is_recorded  = 1;

    return ret;
}

#else
/**
 *  \fn          int hal_crypto_auth_update_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter, IN const u32 auth_type,
 *                                                   IN const u8 *message, IN const u32 msglen)
 *  \brief       Check input parameters which authentication needs,and use a sequential buffer to help crypto engine
 *               to pre-process authentication message buffer.After that, update related sequential hash data in crypto adapter.
 *  \param[in]   pcrypto_adapter  Pointer to crypto adapter data
 *  \param[in]   auth_type Hash/HMAC type ID
 *  \param[in]   message Pointer to authentication message buffer
 *  \param[in]   msglen Authentication message buffer length
 *  \return      value == 0     success
 *  \return      value < 0      fail(error code)
 */
int hal_crypto_auth_update_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter, IN const u32 auth_type,
                                    IN const u8 *message, IN const u32 msglen)
{
    int ret = SUCCESS;
    if (pcrypto_adapter == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (message == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    //if ( msglen > CRYPTO_MAX_MSG_LENGTH ) return _ERRNO_CRYPTO_MSG_OutRange;

    if ((pcrypto_adapter->auth_type & AUTH_TYPE_MASK_FUNC_ALL) != (auth_type & AUTH_TYPE_MASK_FUNC_ALL)) {
        return _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH;
    }

    {
        pcrypto_adapter->hmac_seq_last_message = (u8*)message;
        pcrypto_adapter->hmac_seq_last_msglen  = msglen;
        pcrypto_adapter->hmac_seq_is_recorded  = 1;

        ret = hal_crypto_send_seq_buf_rtl8277c(pcrypto_adapter, NULL);
    }
    return ret;
}
#endif

/**
 *  \fn          int hal_crypto_auth_final_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter, IN const u32 auth_type, OUT u8 *pDigest)
 *  \brief       Check input parameters which authentication needs,and process authentication last message buffer.
 *               After that, get the hash digest.
 *  \param[in]   pcrypto_adapter  Pointer to crypto adapter data
 *  \param[in]   auth_type Hash/HMAC type ID
 *  \param[out]  pDigest Pointer to authentication digest buffer
 *  \return      value == 0     success
 *  \return      value < 0      fail(error code)
 */
int hal_crypto_auth_final_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter, IN const u32 auth_type, OUT u8 *pDigest)
{
    int ret = SUCCESS;
    if (pcrypto_adapter == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if ((pcrypto_adapter->auth_type & AUTH_TYPE_MASK_FUNC_ALL) != (auth_type & AUTH_TYPE_MASK_FUNC_ALL)) {
        return _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH;
    }
    if (pDigest == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (pcrypto_adapter->hmac_seq_is_recorded == 0) {
        return _ERRNO_CRYPTO_HASH_FINAL_NO_UPDATE;
    }

    {
        ret = hal_crypto_send_seq_buf_rtl8277c(pcrypto_adapter, pDigest);
        if (ret != SUCCESS) {
            return ret;
        }
    }

    hal_crypto_engine_get_result_rtl8277c(pcrypto_adapter, pDigest, NULL, 0, NULL);
    pcrypto_adapter->hmac_seq_is_recorded = 0;

    return ret;
}

/**
 *  \fn          int hal_crypto_auth_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter, IN const u32 auth_type,
 *                                            IN const u8 *message, IN const u32 msglen,
 *                                            IN const u8 *pAuthKey, IN const u32 lenAuthKey, OUT u8 *pDigest)
 *  \brief       Make crypto engine run Hash/HMAC algorithm. After that, get the hash digest.
 *  \note        The maximum input buffer length can't be over 64k bytes.
 *  \param[in]   pcrypto_adapter  Pointer to crypto adapter data
 *  \param[in]   auth_type Hash/HMAC type ID
 *  \param[in]   message Pointer to authentication message buffer
 *  \param[in]   msglen Authentication message buffer length
 *  \param[in]   pAuthKey Pointer to authentication key buffer
 *  \param[in]   lenAuthKey Authentication key buffer length
 *  \param[out]  pDigest Pointer to authentication digest buffer
 *  \return      value == 0     success
 *  \return      value < 0      fail(error code)
 */
int hal_crypto_auth_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter, IN const u32 auth_type,
                             IN const u8 *message, IN const u32 msglen,
                             IN const u8 *pAuthKey, IN const u32 lenAuthKey, OUT u8 *pDigest)
{
    int ret = FAIL;
    do {
        ret = hal_crypto_auth_init_rtl8277c(pcrypto_adapter, auth_type, pAuthKey, lenAuthKey);
        if (ret != SUCCESS) {
            break;
        }
        ret = hal_crypto_auth_process_rtl8277c(pcrypto_adapter, auth_type, message, msglen, pDigest);
    } while (0);
    return ret;
}

/**
 *  \fn          int hal_crypto_cipher_init_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter, IN const u32 cipher_type,
 *                                                   IN const u8 *key, IN const u32 keylen)
 *  \brief       Check input parameters which cipher needs, and set related cipher initial values to crypto adapter.
 *  \note
 *               cipher_type =
 *               {
 *                  CIPHER_TYPE_AES_ECB,
 *                  CIPHER_TYPE_AES_CBC,
 *                  CIPHER_TYPE_AES_CFB,
 *                  CIPHER_TYPE_AES_OFB,
 *                  CIPHER_TYPE_AES_CTR,
 *                  CIPHER_TYPE_AES_GCTR,
 *                  CIPHER_TYPE_AES_GMAC,
 *                  CIPHER_TYPE_AES_GHASH,
 *                  CIPHER_TYPE_AES_GCM
 *               }
 *  \param[in]   pcrypto_adapter  Pointer to crypto adapter data
 *  \param[in]   cipher_type  Cipher type ID
 *  \param[in]   key Pointer to crypto cipher key buffer
 *  \param[in]   keylen Cipher key buffer length
 *  \return      value == 0     success
 *  \return      value < 0      fail(error code)
 */
int hal_crypto_cipher_init_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter, IN const u32 cipher_type, IN const u8 *key, IN const u32 keylen)
{
    int ret = SUCCESS;
    u8 *pAuthKey = NULL;
    u32 lenAuthKey = 0;
    if (pcrypto_adapter == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (key == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (check_4byte_align((size_t)(key))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
    }
    if (CIPHER_TYPE_AES_GHASH == cipher_type) {
        if ((128/8) != keylen ) {
            return _ERRNO_CRYPTO_KEY_OutRange;
        }
    } else if (CIPHER_TYPE_AES_GMAC == cipher_type) {
        if (((128/8) != keylen) && ((192/8) != keylen) && ((256/8) != keylen)) {
            return _ERRNO_CRYPTO_KEY_OutRange;
        }
    }
    //if ( (keylen != 128/8) && (keylen != 192/8) && (keylen != 256/8) ) return _ERRNO_CRYPTO_KEY_OutRange;
    ret = hal_crypto_engine_set_security_mode_rtl8277c(pcrypto_adapter, cipher_type, AUTH_TYPE_NO_AUTH,
                                                       key, keylen, pAuthKey, lenAuthKey);
    return ret;
}

/**
 *  \fn          int hal_crypto_cipher_encrypt_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter, IN const u32 cipher_type,
 *                                                      IN const u8 *message, IN const u32 msglen,
 *                                                      IN const u8 *iv, IN const u32 ivlen,
 *                                                      IN const u8 *aad, IN const u32 aadlen,
 *                                                      OUT u8 *pResult, OUT u8 *pTag)
 *  \brief       Check input parameters which cipher needs, and make crypto engine encrypt the plaintext.
 *               After that, get the ciphertext.
 *  \param[in]   pcrypto_adapter  Pointer to crypto adapter data
 *  \param[in]   cipher_type  Cipher type ID
 *  \param[in]   message Pointer to Crypto cipher message buffer
 *  \param[in]   msglen Crypto cipher message data length
 *  \param[in]   iv Pointer to Crypto cipher initial vector buffer
 *  \param[in]   ivlen Crypto cipher initial vector data length
 *  \param[in]   aad Pointer to Crypto cipher additional authentication buffer
 *  \param[in]   aadlen Crypto cipher additional authentication data length
 *  \param[out]  pResult Pointer to Crypto cipher result buffer
 *  \param[out]  pTag Pointer to Crypto cipher Tag buffer
 *  \return      value == 0     success
 *  \return      value < 0      fail(error code)
 */
int hal_crypto_cipher_encrypt_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter, IN const u32 cipher_type,
                                       IN const u8 *message, IN const u32 msglen,
                                       IN const u8 *iv, IN const u32 ivlen,
                                       IN const u8 *aad, IN const u32 aadlen,
                                       OUT u8 *pResult, OUT u8 *pTag)
{
    int ret = SUCCESS;
    if ((pcrypto_adapter->cipher_type & CIPHER_TYPE_MASK_ALL) != (cipher_type & CIPHER_TYPE_MASK_ALL)) {
        return _ERRNO_CRYPTO_CIPHER_TYPE_NOT_MATCH;
    }
    if (CIPHER_TYPE_FUNC_AES == (cipher_type & CIPHER_TYPE_MASK_FUNC)) {
        if (CIPHER_TYPE_AES_GCM == cipher_type) {
            ret = hal_crypto_cipher_process_rtl8277c(pcrypto_adapter, TYPE_ENCRYPT, cipher_type, message, msglen,
                                                     iv, 0, aad, aadlen, pResult, pTag);
        } else if (CIPHER_TYPE_AES_GCTR == cipher_type) {
            ret = hal_crypto_cipher_process_rtl8277c(pcrypto_adapter, TYPE_ENCRYPT, cipher_type, message, msglen,
                                                     iv, 0, NULL, 0, pResult, NULL);
        } else if (CIPHER_TYPE_AES_GHASH == cipher_type) {
            return ret; //do nothing, because aes_ghash only use decrypt function
        } else if (CIPHER_TYPE_AES_GMAC == cipher_type) {
            return ret; //do nothing, because aes_gmac only use decrypt function
        } else {
            ret = hal_crypto_cipher_process_rtl8277c(pcrypto_adapter, TYPE_ENCRYPT, cipher_type, message, msglen,
                                                     iv, ivlen, NULL, 0, pResult, NULL);
        }
    }
    else if ((CIPHER_TYPE_FUNC_DES == (cipher_type & CIPHER_TYPE_MASK_FUNC)) ||
        (CIPHER_TYPE_FUNC_3DES == (cipher_type & CIPHER_TYPE_MASK_FUNC))) {
        DBG_CRYPTO_INFO("hal_rom_crypto_cipher_encrypt2\r\n");
        ret = hal_crypto_cipher_process_rtl8277c(pcrypto_adapter, TYPE_ENCRYPT, cipher_type, message, msglen,
                                                     iv, ivlen, NULL, 0, pResult, NULL);
    }
    return ret;
}

/**
 *  \fn          int hal_crypto_cipher_decrypt_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter, IN const u32 cipher_type,
 *                                                      IN const u8 *message, IN const u32 msglen,
 *                                                      IN const u8 *iv, IN const u32 ivlen,
 *                                                      IN const u8 *aad, IN const u32 aadlen,
 *                                                      OUT u8 *pResult, OUT u8 *pTag)
 *  \brief       Check input parameters which cipher needs, and make crypto engine decrypt the ciphertext.
 *               After that, get the plaintext.
 *  \param[in]   pcrypto_adapter  Pointer to crypto adapter data
 *  \param[in]   cipher_type  Cipher type ID
 *  \param[in]   message Pointer to Crypto cipher message buffer
 *  \param[in]   msglen Crypto cipher message data length
 *  \param[in]   iv Pointer to Crypto cipher initial vector buffer
 *  \param[in]   ivlen Crypto cipher initial vector data length
 *  \param[in]   aad Pointer to Crypto cipher additional authentication buffer
 *  \param[in]   aadlen Crypto cipher additional authentication data length
 *  \param[out]  pResult Pointer to Crypto cipher result buffer
 *  \param[out]  pTag Pointer to Crypto cipher Tag buffer
 *  \return      value == 0     success
 *  \return      value < 0      fail(error code)
 */
int hal_crypto_cipher_decrypt_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter, IN const u32 cipher_type,
                                       IN const u8 *message, IN const u32 msglen,
                                       IN const u8 *iv, IN const u32 ivlen,
                                       IN const u8 *aad, IN const u32 aadlen,
                                       OUT u8 *pResult, OUT u8 *pTag)
{
    int ret = SUCCESS;
    if ((pcrypto_adapter->cipher_type & CIPHER_TYPE_MASK_ALL) != (cipher_type & CIPHER_TYPE_MASK_ALL)) {
        return _ERRNO_CRYPTO_CIPHER_TYPE_NOT_MATCH;
    }
    if (CIPHER_TYPE_FUNC_AES == (cipher_type & CIPHER_TYPE_MASK_FUNC)) {
        if (CIPHER_TYPE_AES_GCM == cipher_type) {
            ret = hal_crypto_cipher_process_rtl8277c(pcrypto_adapter, TYPE_DECRYPT, cipher_type, message, msglen,
                                                     iv, 0, aad, aadlen, pResult, pTag);
        } else if (CIPHER_TYPE_AES_GCTR == cipher_type) {
            ret = hal_crypto_cipher_process_rtl8277c(pcrypto_adapter, TYPE_DECRYPT, cipher_type, message, msglen,
                                                     iv, 0, NULL, 0, pResult, NULL);
        } else if (CIPHER_TYPE_AES_GHASH == cipher_type) {
            ret = hal_crypto_cipher_process_rtl8277c(pcrypto_adapter, TYPE_DECRYPT, cipher_type, message, msglen,
                                                     NULL, 0, NULL, 0, NULL, pTag);
        } else if (CIPHER_TYPE_AES_GMAC == cipher_type) {
            ret = hal_crypto_cipher_process_rtl8277c(pcrypto_adapter, TYPE_DECRYPT, cipher_type, message, msglen,
                                                     iv, 0, aad, aadlen, NULL, pTag);
        } else {
            ret = hal_crypto_cipher_process_rtl8277c(pcrypto_adapter, TYPE_DECRYPT, cipher_type, message, msglen,
                                                     iv, ivlen, NULL, 0, pResult, NULL);
        }
    }
    else if ((CIPHER_TYPE_FUNC_DES == (cipher_type & CIPHER_TYPE_MASK_FUNC)) ||
        (CIPHER_TYPE_FUNC_3DES == (cipher_type & CIPHER_TYPE_MASK_FUNC))) {
        ret = hal_crypto_cipher_process_rtl8277c(pcrypto_adapter, TYPE_DECRYPT, cipher_type, message, msglen,
                                                     iv, ivlen, NULL, 0, pResult, NULL);
    }
    return ret;
}

/**
 *  \fn          int hal_crypto_mix_mode_init_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter, IN const u32 cipher_type,
 *                                                     IN const u32 auth_type, IN const u8 *cipher_key,
 *                                                     IN const u32 cipher_keylen, IN const u8 *auth_key,
 *                                                     IN const u32 auth_keylen)
 *  \brief       Check input parameters which cipher & auth needs, and set related initial values to crypto adapter.
 *  \note
 *               mix_mode_type =
 *               {
 *                  CRYPTO_MIX_MODE_SSH_ENC,
 *                  CRYPTO_MIX_MODE_SSH_DEC,
 *                  CRYPTO_MIX_MODE_ESP_ENC,
 *                  CRYPTO_MIX_MODE_ESP_DEC,
 *                  CRYPTO_MIX_MODE_SSL_TLS_ENC
 *               }
 *  \param[in]   pcrypto_adapter  Pointer to crypto adapter data
 *  \param[in]   cipher_type  Cipher type ID
 *  \param[in]   auth_type Hash/HMAC type ID
 *  \param[in]   cipher_key Pointer to crypto cipher key buffer
 *  \param[in]   cipher_keylen Cipher key buffer length
 *  \param[in]   auth_key Pointer to authentication key buffer
 *  \param[in]   auth_keylen Authentication key buffer length
 *  \return      value == 0     success
 *  \return      value < 0      fail(error code)
 */
int hal_crypto_mix_mode_init_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter, IN const u32 cipher_type, IN const u32 auth_type,
                                      IN const u8 *cipher_key, IN const u32 cipher_keylen,
                                      IN const u8 *auth_key, IN const u32 auth_keylen)
{
    int ret = SUCCESS;
    if (pcrypto_adapter == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (cipher_key == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (check_4byte_align((size_t)(cipher_key))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
    }
    if ((cipher_keylen != 16) && (cipher_keylen != 24) && (cipher_keylen != 32)) {
        return _ERRNO_CRYPTO_KEY_OutRange;
    }
    if (auth_keylen != 0) {
        if (auth_keylen > CRYPTO_AUTH_PADDING) {
            return _ERRNO_CRYPTO_KEY_OutRange;
        }
        if (auth_key == NULL) {
            return _ERRNO_CRYPTO_NULL_POINTER;
        }
        if (check_4byte_align((size_t)(auth_key))) {
            return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
        }
    }

    ret = hal_crypto_engine_set_security_mode_rtl8277c(pcrypto_adapter, cipher_type, auth_type,
                                                       cipher_key, cipher_keylen, auth_key, auth_keylen);
    return ret;
}

/**
 *  \fn          int hal_crypto_engine_mix_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter,
                                                    IN const u8 *message, IN const uint32_t msglen,
                                                    IN const u8 *pIv, IN const uint32_t ivlen,
                                                    IN const u8 *paad, IN const uint32_t aadlen,
                                                    IN const u8 *phash_pad, IN const uint32_t hash_padlen,
                                                    IN const u8 *penc_pad, IN const uint32_t enc_padlen,
                                                    OUT u8 *pResult, OUT u8 *pTag)
 *  \brief       Check input parameters, and set crypto engine registers, then make crypto engine handle those algorithms
 *               in mix mode.
 *  \details     The executing order of crypto engine setting: \n
 *               - Initialize the notified mechanism.
 *               - Set essential buffers and buffer lengths of the cryptographic feature to crypto adapter
 *               - Setup desitination descriptor
 *               - Setup source descriptor
 *                  - Setup command setting buffer
 *                  - Setup key, initial vector, HMAC padding buffer
 *                  - Prepare Data1 ~ DataN
 *               - Wait to be notified after interrupt service routine processed over.
 *  \note        For mix mode(consider hash padding buffer)
 *  \param[in]   pcrypto_adapter  Pointer to crypto adapter data
 *  \param[in]   message Pointer to Crypto cipher message buffer
 *  \param[in]   msglen Crypto cipher message buffer length
 *  \param[in]   pIv Pointer to Crypto cipher initial vector buffer
 *  \param[in]   ivlen Crypto cipher initial vector buffer length
 *  \param[in]   paad Pointer to Crypto cipher additional authentication data buffer
 *  \param[in]   aadlen Crypto cipher additional authentication data buffer length
 *  \param[in]   phash_pad Pointer to mix mode hash padding data buffer
 *  \param[in]   hash_padlen Mix mode hash padding data buffer length
 *  \param[in]   penc_pad Pointer to mix mode encryption padding data buffer
 *  \param[in]   enc_padlen Mix mode encryption padding data buffer length
 *  \param[out]  pResult Pointer to Crypto cipher result buffer
 *  \param[out]  pTag Pointer to Crypto hash digest buffer
 *  \return      value == 0     success
 *  \return      value < 0      fail(error code)
 */
int hal_crypto_engine_mix_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter,
                                   IN const u8 *message, 	IN const uint32_t msglen,
                                   IN const u8 *pIv, 		IN const uint32_t ivlen,
                                   IN const u8 *paad,       IN const uint32_t aadlen,
                                   IN const u8 *phash_pad,  IN const uint32_t hash_padlen,
                                   IN const u8 *penc_pad,   IN const uint32_t enc_padlen,
                                   OUT u8 *pResult, OUT u8 *pTag)
{
    rtl_crypto_srcdesc_t srcdesc;

    uint32_t a2eo;
    uint32_t enl;
    uint32_t auth_padlen;
    uint32_t cipher_padlen;
    uint32_t digestlen;

    volatile uint8_t *padding = NULL;
    int ret = SUCCESS;

    if (pcrypto_adapter == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    } else {
        padding = (&(pcrypto_adapter->padding[0]));
    }
    hal_crypto_engine_pre_exec_rtl8277c(pcrypto_adapter);

    if ((paad != NULL) && (aadlen > 0)) {
        uint32_t cache_len;
        if (aadlen & 0x1F) {
            cache_len = ((aadlen/32) + 1) * 32;
        } else {
            cache_len = aadlen;
        }
        arch_clean_dcache_by_size(pcrypto_adapter, (size_t)paad, cache_len);
    }

    // Use only one scatter
    a2eo = aadlen;
    enl = msglen;
    auth_padlen = hash_padlen;
    cipher_padlen = enc_padlen;

    hal_crypto_start_packet_init_rtl8277c(pcrypto_adapter);
//
//   Set relative length data
//
    hal_crypto_engine_auth_calc_apl_rtl8277c(pcrypto_adapter, a2eo, enl, auth_padlen, cipher_padlen);

    //Hash result length
    digestlen = pcrypto_adapter->digestlen;

    /********************************************
     * step 1: Setup desitination descriptor
     ********************************************/
    pcrypto_adapter->is_dst_first_cache_used = 0;
    pcrypto_adapter->is_dst_last_cache_used  = 0;

    rtl_crypto_dstdesc_t  dst_auth_desc;
    rtl_crypto_dstdesc_t  dst_cipher_desc;
    int8_t first_cache_pos = 0;
    int8_t first_cache_len = 0;
    int8_t last_cache_pos  = 0;
    int8_t last_cache_len  = 0;
    int dst_len;

    if (pResult != NULL) {
        if ((CRYPTO_ENGINE_SET_MIX_MODE_SSH_ENC_ESP_DEC == (pcrypto_adapter->mix_mode_type)) ||
            (CRYPTO_ENGINE_SET_MIX_MODE_SSH_DEC_ESP_ENC == (pcrypto_adapter->mix_mode_type))) {
            //Destination descriptor may change msglen, because of encryption padding
            dst_len = (pcrypto_adapter->enl + (pcrypto_adapter->apl));
            DBG_CRYPTO_INFO("[Set dest_desc] dst_len = %d,enl = %d,apl = %d\r\n",dst_len,(pcrypto_adapter->enl),(pcrypto_adapter->apl));
        } else {
            //CRYPTO_ENGINE_SET_MIX_MODE_SSL_TLS_ENC
            dst_len = (pcrypto_adapter->enl + digestlen + pcrypto_adapter->enc_padlen);
            DBG_CRYPTO_INFO("[Set dest_desc] dst_len = %d,enl = %d,digestlen = %d,enc_padlen = %d\r\n",dst_len,(pcrypto_adapter->enl),
                                                                                            digestlen, (pcrypto_adapter->enc_padlen));
        }

        first_cache_pos = (size_t)pResult & 0x1F;
        first_cache_len = (32 - first_cache_pos) % 32;
        if (first_cache_len >= dst_len) {
            //There's an enough space of first_cache_line for dst_len
            first_cache_len = dst_len;
            last_cache_len  = 0;
        } else {
            last_cache_pos = (size_t)(pResult + dst_len - 1) & 0x1F;
            last_cache_len = last_cache_pos + 1;
            if (last_cache_len == 32) {
                last_cache_len = 0;
            }
        }
        DBG_CRYPTO_INFO("[Set dest_desc] f_cache_pos = %d,f_cache_len = %d\r\n",first_cache_pos,first_cache_len);
        DBG_CRYPTO_INFO("[Set dest_desc] l_cache_pos = %d,l_cache_len = %d\r\n",last_cache_pos,last_cache_len);

        dst_cipher_desc.w = 0;
        dst_cipher_desc.cipher.ws  = 1;
        dst_cipher_desc.cipher.fs  = 1;
        dst_cipher_desc.cipher.enc = 1;

        if (first_cache_len != 0) {
            pcrypto_adapter->is_dst_first_cache_used = 1;

            if (CRYPTO_ENGINE_SET_MIX_MODE_SSL_TLS_ENC == (pcrypto_adapter->mix_mode_type)) {
                if (dst_len == first_cache_len) {
                    dst_cipher_desc.cipher.ls  = 1;
                }
            }

            dst_cipher_desc.cipher.enl = first_cache_len;
            DBG_CRYPTO_INFO("[Set dest_desc] f_cache_line_enl = %d\r\n",first_cache_len);

            arch_clean_dcache_by_size(pcrypto_adapter, (size_t)pcrypto_adapter->dst_first_cache_line, 32);
            hal_crypto_set_dstdesc_rtl8277c(pcrypto_adapter, dst_cipher_desc.w, (size_t)pcrypto_adapter->dst_first_cache_line);

            dst_len -= first_cache_len;
            pResult += first_cache_len;

            dst_cipher_desc.w = 0;
            dst_cipher_desc.cipher.ws = 1;
            dst_cipher_desc.cipher.enc = 1;
        }

        if (dst_len > 0) {
            //Cut off valid msg of last_cache_line
            //It means leave 32 bytes alignment addr of msg_body
            dst_cipher_desc.cipher.enl = (dst_len - last_cache_len);

            if (last_cache_len != 0) {
                pcrypto_adapter->is_dst_last_cache_used = 1;
                if (dst_len > last_cache_len) { // body
                    arch_clean_dcache_by_size(pcrypto_adapter, (size_t)pResult, (dst_len - last_cache_len));
                    hal_crypto_set_dstdesc_rtl8277c(pcrypto_adapter, dst_cipher_desc.w, (size_t)pResult);
                }
                pResult += (dst_len - last_cache_len);
                dst_len = last_cache_len;

                dst_cipher_desc.w = 0;
                dst_cipher_desc.cipher.ws  = 1;
                dst_cipher_desc.cipher.enc = 1;
                if (CRYPTO_ENGINE_SET_MIX_MODE_SSL_TLS_ENC == (pcrypto_adapter->mix_mode_type)) {
                    dst_cipher_desc.cipher.ls  = 1;
                }
                dst_cipher_desc.cipher.enl = last_cache_len;
                DBG_CRYPTO_INFO("[Set dest_desc] l_cache_line_enl = %d\r\n",last_cache_len);

                arch_clean_dcache_by_size(pcrypto_adapter, (size_t)pcrypto_adapter->dst_last_cache_line, 32);
                hal_crypto_set_dstdesc_rtl8277c(pcrypto_adapter, dst_cipher_desc.w, (size_t)pcrypto_adapter->dst_last_cache_line);
            } else {
                if (CRYPTO_ENGINE_SET_MIX_MODE_SSL_TLS_ENC == (pcrypto_adapter->mix_mode_type)) {
                    dst_cipher_desc.cipher.ls  = 1;
                }
                arch_clean_dcache_by_size(pcrypto_adapter, (size_t)pResult, dst_cipher_desc.cipher.enl);
                hal_crypto_set_dstdesc_rtl8277c(pcrypto_adapter ,dst_cipher_desc.w, (size_t)pResult);
            }
        }
    }

    if (pTag != NULL) {
        //Calculate cipher Tag,use adl not enc!
        dst_auth_desc.w = 0;
        dst_auth_desc.auth.ws  = 1;
        dst_auth_desc.auth.ls  = 1;
        dst_auth_desc.auth.enc = 0;
        dst_auth_desc.auth.adl = digestlen;
        arch_clean_dcache_by_size(pcrypto_adapter, (size_t)pcrypto_adapter->tag_result, 32);
        hal_crypto_set_dstdesc_rtl8277c(pcrypto_adapter, dst_auth_desc.w, (size_t)pcrypto_adapter->tag_result);
    }

    /********************************************
     * step 2: Setup source descriptor
     ********************************************/
    /********************************************
     * step 2-1: prepare Key & IV array:
     ********************************************/

    hal_crypto_engine_srcdesc_generate_cl_key_iv_rtl8277c(pcrypto_adapter, pIv, ivlen);


    /********************************************
     * step 2-2: prepare Data1 ~ DataN
     ********************************************/

    srcdesc.w = 0;
    srcdesc.d.rs = 1;

    if (paad != NULL) {
        while (a2eo > 24) {
            srcdesc.d.a2eo = 24;

            __dbg_mem_dump(pcrypto_adapter->isMemDump,paad, 24, "MIX_AAD[while-loop]: ");
            hal_crypto_set_srcdesc_rtl8277c(pcrypto_adapter, srcdesc.w, (size_t)paad);

            paad += 24;
            a2eo -= 24;
            srcdesc.w = 0;
            srcdesc.d.rs = 1;
        }

        if (a2eo > 0) {
            DBG_CRYPTO_INFO("[Set src_desc_data] a2eo = %d\r\n",a2eo);
            srcdesc.d.a2eo = a2eo;
            __dbg_mem_dump(pcrypto_adapter->isMemDump,paad, a2eo, "AAD[rest]: ");
            hal_crypto_set_srcdesc_rtl8277c(pcrypto_adapter, srcdesc.w, (size_t)paad);
            srcdesc.w = 0;
            srcdesc.d.rs = 1;
        }

        //Handle padding issue
        if (pcrypto_adapter->apl_aad > 0) {
            DBG_CRYPTO_INFO("[Set src_desc_data] apl_aad = %d\r\n",pcrypto_adapter->apl_aad);
            srcdesc.d.a2eo = pcrypto_adapter->apl_aad;
            arch_clean_dcache_by_size(pcrypto_adapter, (size_t)padding, 64);
            __dbg_mem_dump(pcrypto_adapter->isMemDump,padding, pcrypto_adapter->apl_aad, "AAD padding ");
            hal_crypto_set_srcdesc_rtl8277c(pcrypto_adapter, srcdesc.w, (size_t)padding);
            srcdesc.w = 0;
            srcdesc.d.rs = 1;
        }

    }

    {
        int8_t first_cache_pos = 0;
        int8_t first_cache_len = 0;
        int8_t last_cache_pos  = 0;
        int8_t last_cache_len  = 0;

        first_cache_pos = (size_t) message & 0x1F;
        first_cache_len = (32 - first_cache_pos) % 32;
        if ((size_t)first_cache_len >= enl) { // the same one
            first_cache_len = enl;
            last_cache_len  = 0;
        } else {
            last_cache_pos = ((size_t)(message) + enl - 1) & 0x1F;
            last_cache_len = last_cache_pos + 1;
        }

        DBG_CRYPTO_INFO("[Set src_desc_data] f_cache_pos = %d,f_cache_len = %d\r\n",first_cache_pos,first_cache_len);
        DBG_CRYPTO_INFO("[Set src_desc_data] l_cache_pos = %d,l_cache_len = %d\r\n",last_cache_pos,last_cache_len);

        if (first_cache_len != 0) {
            rtlc_memcpy((void*)(&(pcrypto_adapter->src_first_cache_line[0])), (const void*)(message), first_cache_len);

            srcdesc.d.enl = first_cache_len;
            DBG_CRYPTO_INFO("[Set src_desc_data] f_cache_line_enl = %d\r\n",first_cache_len);
            __dbg_mem_dump(pcrypto_adapter->isMemDump,&(pcrypto_adapter->src_first_cache_line[0]), first_cache_len, "src_data[first_cache_line]: ");
            arch_clean_dcache_by_size(pcrypto_adapter, (size_t)(&(pcrypto_adapter->src_first_cache_line[0])), 32);
            hal_crypto_set_srcdesc_rtl8277c(pcrypto_adapter, srcdesc.w, (size_t)(&(pcrypto_adapter->src_first_cache_line[0])));
            message += first_cache_len;
            enl -= first_cache_len;
            srcdesc.w = 0;
            srcdesc.d.rs = 1;
        }

        // 16352 bytes is the max size of enl could set!
        while (enl > 16352) {
            arch_clean_dcache_by_size(pcrypto_adapter, (size_t)message, 16352);
            srcdesc.d.enl = 16352;
            hal_crypto_set_srcdesc_rtl8277c(pcrypto_adapter, srcdesc.w, (size_t)message);

            message += 16352;
            enl -= 16352;
            srcdesc.w = 0;
            srcdesc.d.rs = 1;
        }

        //Cut off valid msg of last_cache_line
        //It means leave 32 bytes alignment addr of msg_body
        // message aligned body
        if (enl > (size_t)(last_cache_len)) {
            uint32_t dst_len = enl - (size_t)(last_cache_len);

            // assert
            if ((dst_len & 0x1F) != 0) {
                DBG_CRYPTO_ERR("Strange : enl(%d), last_cache_len(%d)", enl, last_cache_len);
                return _ERRNO_CRYPTO_CACHE_HANDLE;
            }

            DBG_CRYPTO_INFO("[Set src_desc_data] dst_len = %d\r\n",dst_len);
            srcdesc.d.enl = dst_len;
            arch_clean_dcache_by_size(pcrypto_adapter, (size_t)message, srcdesc.d.enl);
            __dbg_mem_dump(pcrypto_adapter->isMemDump,message, msglen, "src_data[msg_32align_body]: ");
            hal_crypto_set_srcdesc_rtl8277c(pcrypto_adapter, srcdesc.w, (size_t)message);
            message += dst_len;
            enl -= dst_len;

            srcdesc.w = 0;
            srcdesc.d.rs = 1;
        } else if (enl < (size_t)(last_cache_len)) {

            DBG_CRYPTO_ERR("Strange : enl(%d), last_cache_len(%d)", enl, last_cache_len);
            return _ERRNO_CRYPTO_CACHE_HANDLE;
        }

        if (last_cache_len == 32) {

            srcdesc.d.enl = 32;
            __dbg_mem_dump(pcrypto_adapter->isMemDump,message, 32, "src_data[last_cache_len_equal_32]: ");
            arch_clean_dcache_by_size(pcrypto_adapter, (size_t)message, 32);
            hal_crypto_set_srcdesc_rtl8277c(pcrypto_adapter, srcdesc.w, (size_t)message);

        } else if (last_cache_len != 0)  {
            rtlc_memcpy((void*)(pcrypto_adapter->src_last_cache_line), (const void*)(message), last_cache_len);
            DBG_CRYPTO_INFO("[Set src_desc_data] l_cache_len(below 32) = %d\r\n",last_cache_len);

            srcdesc.d.enl = last_cache_len;
            __dbg_mem_dump(pcrypto_adapter->isMemDump,pcrypto_adapter->src_last_cache_line, last_cache_len, "src_data[last_cache_len_below_32]: ");
            arch_clean_dcache_by_size(pcrypto_adapter, (size_t)pcrypto_adapter->src_last_cache_line, 32);
            hal_crypto_set_srcdesc_rtl8277c(pcrypto_adapter, srcdesc.w, (size_t)pcrypto_adapter->src_last_cache_line);
        }

        if (pcrypto_adapter->apl != 0) {
            DBG_CRYPTO_INFO("[Set src_desc_data] pcrypto_adapter->apl = %d\r\n",pcrypto_adapter->apl);
            srcdesc.w = 0;
            srcdesc.d.rs  = 1;
            srcdesc.d.enl = pcrypto_adapter->apl;

            arch_clean_dcache_by_size(pcrypto_adapter, (size_t)padding, 64);
            __dbg_mem_dump(pcrypto_adapter->isMemDump,padding, srcdesc.d.enl, "src_data padding ");
            hal_crypto_set_srcdesc_rtl8277c(pcrypto_adapter, srcdesc.w, (size_t)padding);
        }

        if (phash_pad != NULL) {
            if ((phash_pad != NULL) && (auth_padlen > 0)) {
                uint32_t cache_len;
                if (auth_padlen & 0x1F) {
                    cache_len = ((auth_padlen/32) + 1) * 32;
                } else {
                    cache_len = auth_padlen;
                }
                arch_clean_dcache_by_size(pcrypto_adapter, (size_t)phash_pad, cache_len);
            }
            DBG_CRYPTO_INFO("[Set src_desc_data] pcrypto_adapter->hash_padlen = %d\r\n",pcrypto_adapter->hash_padlen);
            srcdesc.w = 0;
            srcdesc.d.rs  = 1;

            while (auth_padlen > 128) {
                srcdesc.d.apl = 128;

                __dbg_mem_dump(pcrypto_adapter->isMemDump,phash_pad, 128, "MIX_APL[while-loop]: ");
                hal_crypto_set_srcdesc_rtl8277c(pcrypto_adapter, srcdesc.w, (size_t)phash_pad);

                phash_pad += 128;
                auth_padlen -= 128;
                srcdesc.w = 0;
                srcdesc.d.rs = 1;
            }

            if (auth_padlen > 0) {
                DBG_CRYPTO_INFO("[Set src_desc_data] auth_padlen = %d\r\n",auth_padlen);
                if ((CRYPTO_ENGINE_SET_MIX_MODE_SSH_ENC_ESP_DEC == (pcrypto_adapter->mix_mode_type)) ||
                    (CRYPTO_ENGINE_SET_MIX_MODE_SSH_DEC_ESP_ENC == (pcrypto_adapter->mix_mode_type))) {
                    if (pcrypto_adapter->hashpad_pad == 0) {
                        srcdesc.d.ls  = 1;
                    }
                }

                srcdesc.d.apl = auth_padlen;
                __dbg_mem_dump(pcrypto_adapter->isMemDump,phash_pad, auth_padlen, "APL[rest]: ");
                hal_crypto_set_srcdesc_rtl8277c(pcrypto_adapter, srcdesc.w, (size_t)phash_pad);
                srcdesc.w = 0;
                srcdesc.d.rs = 1;
            }

            //Handle hash padding issue
            if (pcrypto_adapter->hashpad_pad > 0) {
                DBG_CRYPTO_INFO("[Set src_desc_data] hashpad_pad = %d\r\n",pcrypto_adapter->hashpad_pad);
                if ((CRYPTO_ENGINE_SET_MIX_MODE_SSH_ENC_ESP_DEC == (pcrypto_adapter->mix_mode_type)) ||
                    (CRYPTO_ENGINE_SET_MIX_MODE_SSH_DEC_ESP_ENC == (pcrypto_adapter->mix_mode_type))) {
                    srcdesc.d.ls  = 1;
                }
                srcdesc.d.apl = pcrypto_adapter->hashpad_pad;

                arch_clean_dcache_by_size(pcrypto_adapter, (size_t)padding, 64);
                __dbg_mem_dump(pcrypto_adapter->isMemDump,padding, pcrypto_adapter->hashpad_pad, "Hash pad padding ");
                hal_crypto_set_srcdesc_rtl8277c(pcrypto_adapter, srcdesc.w, (size_t)padding);
            }

        }

        // Only mix_mod ssl_tls_enc could match
        if ((cipher_padlen > 0) && (penc_pad != NULL)) {
            uint32_t cache_len;
            if (cipher_padlen & 0x1F) {
                cache_len = ((cipher_padlen/32) + 1) * 32;
            } else {
                cache_len = cipher_padlen;
            }
            arch_clean_dcache_by_size(pcrypto_adapter, (size_t)penc_pad, cache_len);
            DBG_CRYPTO_INFO("[Set src_desc_data] pcrypto_adapter->enc_padlen = %d\r\n",pcrypto_adapter->enc_padlen);
            srcdesc.w = 0;
            srcdesc.d.rs  = 1;
            srcdesc.d.zero = 1;

            while (cipher_padlen > 8) {
                srcdesc.d.a2eo = 8;

                __dbg_mem_dump(pcrypto_adapter->isMemDump,penc_pad, 8, "MIX_EPL[while-loop]: ");
                hal_crypto_set_srcdesc_rtl8277c(pcrypto_adapter, srcdesc.w, (size_t)penc_pad);

                penc_pad += 8;
                cipher_padlen -= 8;
                srcdesc.w = 0;
                srcdesc.d.rs = 1;
                srcdesc.d.zero = 1;
            }

            if (cipher_padlen > 0) {
                DBG_CRYPTO_INFO("[Set src_desc_data] cipher_padlen = %d\r\n",cipher_padlen);
                if (pcrypto_adapter->encpad_pad == 0) {
                    srcdesc.d.ls  = 1;
                }

                srcdesc.d.a2eo = cipher_padlen;
                __dbg_mem_dump(pcrypto_adapter->isMemDump,penc_pad, cipher_padlen, "EPL[rest]: ");
                hal_crypto_set_srcdesc_rtl8277c(pcrypto_adapter, srcdesc.w, (size_t)penc_pad);
                srcdesc.w = 0;
                srcdesc.d.rs = 1;
                srcdesc.d.zero = 1;
            }

            //Handle encryption padding issue
            if (pcrypto_adapter->encpad_pad > 0) {
                DBG_CRYPTO_INFO("[Set src_desc_data] encpad_pad = %d\r\n",pcrypto_adapter->encpad_pad);
                srcdesc.d.ls  = 1;
                srcdesc.d.a2eo = pcrypto_adapter->encpad_pad;

                arch_clean_dcache_by_size(pcrypto_adapter, (size_t)padding, 64);
                __dbg_mem_dump(pcrypto_adapter->isMemDump,padding, pcrypto_adapter->encpad_pad, "Enc pad padding ");
                hal_crypto_set_srcdesc_rtl8277c(pcrypto_adapter, srcdesc.w, (size_t)padding);
            }
        }
    }
    ret = hal_crypto_engine_wait_done_rtl8277c(pcrypto_adapter);

    return ret;
}

/**
 *  \fn          void hal_crypto_engine_mix_get_result_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter, u8 *pResult,
 *                                                              uint32_t len, u8 *pTag)
 *  \brief       Get crypto cipher result and authetication digest.
 *  \note        For mix mode
 *  \param[in]   pcrypto_adapter  Pointer to crypto adapter data
 *  \param[in]   len Crypto cipher message buffer length
 *  \param[out]  pResult Pointer to crypto cipher result buffer
 *  \param[out]  pTag Pointer to Crypto hash digest buffer
 *  \return      none
 */
void hal_crypto_engine_mix_get_result_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter, u8 *pResult, uint32_t len, u8 *pTag)
{
    if (pResult != NULL) {
        uint32_t dst_len = 0;
        uint32_t first_cache_pos = 0;
        uint32_t first_cache_len = 0;
        uint32_t last_cache_pos  = 0;
        uint32_t last_cache_len  = 0;

        first_cache_pos = (size_t)(pResult) & 0x1F;
        first_cache_len = (32 - first_cache_pos) % 32;
        if (first_cache_len >= len) { // the same one
            first_cache_len = len;
            last_cache_len  = 0;
        } else {
            last_cache_pos = ((size_t)(pResult) + len - 1) & 0x1F;
            last_cache_len = last_cache_pos + 1;
            if (last_cache_len == 32) {
                last_cache_len = 0;
            }
        }

        DBG_CRYPTO_INFO("[Get cipher_result] f_cache_pos = %d,f_cache_len = %d\r\n",first_cache_pos,first_cache_len);
        DBG_CRYPTO_INFO("[Get cipher_result] l_cache_pos = %d,l_cache_len = %d\r\n",last_cache_pos,last_cache_len);

        if (pcrypto_adapter->is_dst_first_cache_used != 0) {
            arch_invalidate_dcache_by_size(pcrypto_adapter, (size_t)(pcrypto_adapter->dst_first_cache_line), 32);
            rtlc_memcpy((void*)(pResult), (const void*)(pcrypto_adapter->dst_first_cache_line), first_cache_len);
            dst_len += first_cache_len;
        }

        //32 byte aligned msg body which doesn't need rtlc_memcpy
        if (len > (dst_len + last_cache_len)) {
            DBG_CRYPTO_INFO("[Get cipher_result] msg_body_len = %d\r\n",(len - dst_len - last_cache_len));
            arch_invalidate_dcache_by_size(pcrypto_adapter, (size_t)(pResult) + dst_len, (len - dst_len - last_cache_len));
        }

        //Handle last_cache_line
        dst_len = len - last_cache_len;
        if (pcrypto_adapter->is_dst_last_cache_used != 0) {
            arch_invalidate_dcache_by_size(pcrypto_adapter, (size_t)(pcrypto_adapter->dst_last_cache_line), 32);
            rtlc_memcpy((void*)(pResult + dst_len), (const void*)(pcrypto_adapter->dst_last_cache_line), last_cache_len);
        }
    }
    if (pTag != NULL) {
        arch_invalidate_dcache_by_size(pcrypto_adapter, (size_t)(pcrypto_adapter->tag_result), sizeof(pcrypto_adapter->tag_result));
        __dbg_mem_dump(pcrypto_adapter->isMemDump,pcrypto_adapter->tag_result, 32, "tag_result: ");
        rtlc_memcpy((void*)pTag, (const void*)(pcrypto_adapter->tag_result), pcrypto_adapter->digestlen);
    }
}


/**
 *  \fn          int hal_crypto_mix_process_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter,
                                                     IN const u8 isEncrypt, IN const u32 cipher_type,
                                                     IN const u8 *message, IN const u32 msglen,
                                                     IN const u8 *iv, IN const u32 ivlen,
                                                     IN const u8 *aad, IN const u32 aadlen,
                                                     IN const u32 auth_type,
                                                     IN const u8 *hash_pad, IN const u32 hash_padlen,
                                                     IN const u8 *enc_pad, IN const u32 enc_padlen,
                                                     OUT u8 *pResult, OUT u8 *pTag)
 *  \brief       Check input parameters which mix mode needs, and make crypto engine run mix mode algorithm.
 *               After that, get the cipher result and hash digest.
 *  \param[in]   pcrypto_adapter  Pointer to crypto adapter data
 *  \param[in]   isEncrypt Indicate that crypto engine now is processing encryption or decryption
 *  \param[in]   cipher_type  Cipher type ID
 *  \param[in]   message Pointer to Crypto cipher message buffer
 *  \param[in]   msglen Crypto cipher message buffer length
 *  \param[in]   iv Pointer to Crypto cipher initial vector buffer
 *  \param[in]   ivlen Crypto cipher initial vector buffer length
 *  \param[in]   aad Pointer to Crypto cipher additional authentication buffer
 *  \param[in]   aadlen Crypto cipher additional authentication buffer length
 *  \param[in]   auth_type  Hash/HMAC type ID
 *  \param[in]   hash_pad Pointer to mix mode hash padding data buffer
 *  \param[in]   hash_padlen Mix mode hash padding data buffer length
 *  \param[in]   enc_pad Pointer to mix mode encryption padding data buffer
 *  \param[in]   enc_padlen Mix mode encryption padding data buffer length
 *  \param[out]  pResult Pointer to Crypto cipher result data
 *  \param[out]  pTag Pointer to Crypto cipher Tag data
 *  \return      value == 0     success
 *  \return      value < 0      fail(error code)
 */
int hal_crypto_mix_process_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter,
                                    IN const u8 isEncrypt,  IN const u32 cipher_type,
                                    IN const u8 *message, 	IN const u32 msglen,
                                    IN const u8 *iv, 		IN const u32 ivlen,
                                    IN const u8 *aad,		IN const u32 aadlen,
                                    IN const u32 auth_type,
                                    IN const u8 *hash_pad,  IN const u32 hash_padlen,
                                    IN const u8 *enc_pad,   IN const u32 enc_padlen,
                                    OUT u8 *pResult,        OUT u8 *pTag) {

    int ret = SUCCESS;

    if (check_4byte_align((size_t)(iv))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if (check_4byte_align((size_t)(aad))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if (msglen == 0) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }

    do {
        if (message == NULL) {
            DBG_CRYPTO_ERR("message is null pointer\r\n");
            ret = _ERRNO_CRYPTO_NULL_POINTER;
            break;
        }

        if (CIPHER_TYPE_FUNC_AES == (cipher_type & CIPHER_TYPE_MASK_FUNC)) { //AES
            DBG_CRYPTO_INFO("msglen non_16_aligned %d bytes\r\n",(msglen % 16));
            if (((msglen % 16) != 0) && (!isEncrypt)) {
                ret = _ERRNO_CRYPTO_AES_MSGLEN_NOT_16Byte_Aligned;
                break;
            }
            if (pResult == NULL) {
                DBG_CRYPTO_ERR("pResult is null pointer\r\n");
                ret = _ERRNO_CRYPTO_NULL_POINTER;
                break;
            }
        }

        if (hash_pad == NULL) {
            DBG_CRYPTO_ERR("Hash_pad is null pointer\r\n");
            ret = _ERRNO_CRYPTO_MIX_MODE_HASH_PAD_NULL_POINTER;
            break;
        }

        if ((CRYPTO_ENGINE_SET_MIX_MODE_SSH_ENC_ESP_DEC == (pcrypto_adapter->mix_mode_type)) ||
            (CRYPTO_ENGINE_SET_MIX_MODE_SSH_DEC_ESP_ENC == (pcrypto_adapter->mix_mode_type))) {
            if (pTag == NULL) {
                DBG_CRYPTO_ERR("pTag is null pointer\r\n");
                ret = _ERRNO_CRYPTO_MIX_MODE_TAG_NULL_POINTER;
                break;
            }
        } else { // Mix_mode ssl_tls_enc
            if (enc_pad == NULL) {
                DBG_CRYPTO_ERR("ENC_pad is null pointer\r\n");
                ret = _ERRNO_CRYPTO_MIX_MODE_ENC_PAD_NULL_POINTER;
                break;
            }
        }

        if (isEncrypt) {
            hal_crypto_engine_set_encrypt_rtl8277c(pcrypto_adapter);
        } else {
            hal_crypto_engine_set_decrypt_rtl8277c(pcrypto_adapter);
        }
        ret = hal_crypto_engine_mix_rtl8277c(pcrypto_adapter, message, msglen, iv, ivlen, aad, aadlen, hash_pad, hash_padlen,
                                             enc_pad, enc_padlen, pResult, pTag);

        if (ret != SUCCESS) {
            break;
        }

        if (isEncrypt) {
            if ((CRYPTO_ENGINE_SET_MIX_MODE_SSH_ENC_ESP_DEC == (pcrypto_adapter->mix_mode_type)) ||
                (CRYPTO_ENGINE_SET_MIX_MODE_SSH_DEC_ESP_ENC == (pcrypto_adapter->mix_mode_type))) {
                //Encryption may need to modify msglen of destination descriptor,because of padding length
                hal_crypto_engine_mix_get_result_rtl8277c(pcrypto_adapter, pResult, (msglen + (pcrypto_adapter->apl)), pTag);
            } else {
                //CRYPTO_ENGINE_SET_MIX_MODE_SSL_TLS_ENC mode, because of new msglen (plaintext_len + digestlen + enc_padlen)
                hal_crypto_engine_mix_get_result_rtl8277c(pcrypto_adapter, pResult,
                                                         (msglen + (pcrypto_adapter->digestlen) + (pcrypto_adapter->enc_padlen)), NULL);
            }
        } else {
            hal_crypto_engine_mix_get_result_rtl8277c(pcrypto_adapter, pResult, msglen, pTag);
        }
    } while (0);

    return ret;
}

/**
 *  \fn          int hal_crypto_mix_mode_process_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter, IN const u8 mix_mode_type,
                                                          IN const u32 cipher_type,
                                                          IN const u8 *message, IN const u32 msglen,
                                                          IN const u8 *iv, IN const u32 ivlen,
                                                          IN const u8 *aad, IN const u32 aadlen,
                                                          IN const u32 auth_type,
                                                          IN const u8 *hash_pad, IN const u32 hash_padlen,
                                                          IN const u8 *enc_pad,  IN const u32 enc_padlen,
                                                          OUT u8 *pResult, OUT u8 *pTag)
 *  \brief       Check input parameters which mix mode needs(cipher_type,auth_type,mix_mode type and encryption/decryption).
 *  \param[in]   pcrypto_adapter  Pointer to crypto adapter data
 *  \param[in]   mix_mode_type Mix mode type ID
 *  \param[in]   cipher_type  Cipher type ID
 *  \param[in]   message Pointer to Crypto cipher message buffer
 *  \param[in]   msglen Crypto cipher message buffer length
 *  \param[in]   iv Pointer to Crypto cipher initial vector buffer
 *  \param[in]   ivlen Crypto cipher initial vector buffer length
 *  \param[in]   aad Pointer to Crypto cipher additional authentication buffer
 *  \param[in]   aadlen Crypto cipher additional authentication buffer length
 *  \param[in]   auth_type  Hash/HMAC type ID
 *  \param[in]   hash_pad Pointer to mix mode hash padding data buffer
 *  \param[in]   hash_padlen Mix mode hash padding data buffer length
 *  \param[in]   enc_pad Pointer to mix mode encryption padding data buffer
 *  \param[in]   enc_padlen Mix mode encryption padding data buffer length
 *  \param[out]  pResult Pointer to Crypto cipher result data
 *  \param[out]  pTag Pointer to Crypto cipher Tag data
 *  \return      value == 0     success
 *  \return      value < 0      fail(error code)
 */
int hal_crypto_mix_mode_process_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter, IN const u8 mix_mode_type,
                                         IN const u32 cipher_type,
                                         IN const u8 *message, IN const u32 msglen,
                                         IN const u8 *iv, IN const u32 ivlen,
                                         IN const u8 *aad, IN const u32 aadlen,
                                         IN const u32 auth_type,
                                         IN const u8 *hash_pad, IN const u32 hash_padlen,
                                         IN const u8 *enc_pad,  IN const u32 enc_padlen,
                                         OUT u8 *pResult, OUT u8 *pTag)
{
    int ret = SUCCESS;
    if (pcrypto_adapter == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if ((pcrypto_adapter->cipher_type & CIPHER_TYPE_MASK_ALL) != (cipher_type & CIPHER_TYPE_MASK_ALL)) {
        return _ERRNO_CRYPTO_CIPHER_TYPE_NOT_MATCH;
    }
    if ((pcrypto_adapter->auth_type & AUTH_TYPE_MASK_FUNC_ALL) != (auth_type & AUTH_TYPE_MASK_FUNC_ALL)) {
        return _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH;
    }
    pcrypto_adapter->mix_mode_type = (mix_mode_type & CRYPTO_MIX_MODE_MASK_TYPE);

    ret = hal_crypto_mix_process_rtl8277c(pcrypto_adapter, ((CRYPTO_MIX_MODE_MASK_FUNC & mix_mode_type) >> 1),
                                          cipher_type, message, msglen,
                                          iv, ivlen, aad, aadlen, auth_type, hash_pad, hash_padlen, enc_pad, enc_padlen,
                                          pResult, pTag);
    return ret;
}

int hal_crypto_key_storage_emptyspace_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter,
                                               IN u8 index,
                                               OUT u8 *result)
{
    int ret = SUCCESS;

    if (pcrypto_adapter == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT;
    }

    *result = pcrypto_adapter->securekey[index];

    return ret;
}

int hal_crypto_key_storage_write_lock_result_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter,
                                              IN const u8 index)
{
    u8 lock_result = 0;

    CRYPTO_Type *pcrypto;
    pcrypto = pcrypto_adapter->base_addr;

    switch (index) {
        case SKDR_0:
            lock_result = pcrypto->ipskey_lock_reg_b.key0_lock;
            break;
        case SKDR_1:
            lock_result = pcrypto->ipskey_lock_reg_b.key1_lock;
            break;
        case SKDR_2:
            lock_result = pcrypto->ipskey_lock_reg_b.key2_lock;
            break;
        case SKDR_3:
            lock_result = pcrypto->ipskey_lock_reg_b.key3_lock;
            break;
        case SKDR_4:
            lock_result = pcrypto->ipskey_lock_reg_b.key4_lock;
            break;
        case SKDR_5:
            lock_result = pcrypto->ipskey_lock_reg_b.key5_lock;
            break;
        case SKDR_6:
            lock_result = pcrypto->ipskey_lock_reg_b.key6_lock;
            break;
        case SKDR_7:
            lock_result = pcrypto->ipskey_lock_reg_b.key7_lock;
            break;
        case SKDR_8:
            lock_result = pcrypto->ipskey_lock_reg_b.key8_lock;
            break;
        case SKDR_9:
            lock_result = pcrypto->ipskey_lock_reg_b.key9_lock;
            break;
        case SKDR_10:
            lock_result = pcrypto->ipskey_lock_reg_b.key10_lock;
            break;
        case SKDR_11:
            lock_result = pcrypto->ipskey_lock_reg_b.key11_lock;
            break;
        default:
            break;
    }

    return lock_result;
}

int hal_crypto_key_storage_write_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter,
                                              IN const u8* keyin,
                                              IN const u8 index,
                                              IN const u8 keylock,
                                              IN const u32 keylen)
{
    u32 i;
    u8 keyinTmp[CRYPTO_MAX_KEY_LENGTH];

    CRYPTO_Type *pcrypto;
    pcrypto = pcrypto_adapter->base_addr;

    // set keyin to ipssk_reg
    rtlc_memset(keyinTmp, 0x0, CRYPTO_MAX_KEY_LENGTH);
    rtlc_memcpy(keyinTmp, keyin, keylen);

    //for (i = 0; i < CRYPTO_MAX_KEY_LENGTH; i++) {
    //    pcrypto->ipssk_reg[index][CRYPTO_MAX_KEY_LENGTH-i-1] = keyinTmp[i];
    //}

    for (i = 0; i < 8; i++) {
        pcrypto->ipssk_reg[index][i] = (((u32)(keyinTmp)[0+4*i] << 24) |
                                        ((u32)(keyinTmp)[1+4*i] << 16) |
                                        ((u32)(keyinTmp)[2+4*i] << 8)  |
                                        ((u32)(keyinTmp)[3+4*i] << 0));
        //hal_crypto_key_storage_readkey_rtl8277c(pcrypto_adapter, index);
        //DBG_CRYPTO_INFO("pcrypto->ipssk_reg[index][%d] = %x\n", i, pcrypto->ipssk_reg[index][i]);
    }

    // write lock ipssk_reg[index]
    if (keylock == 1) {
        pcrypto->ipskey_lock_reg |= (1 << index);
        //DBG_CRYPTO_INFO("pcrypto->ipskey_lock_reg = %x\n", pcrypto->ipskey_lock_reg);
    }
    pcrypto_adapter->securekey[index] = 1;

    return SUCCESS;
}

int hal_crypto_key_storage_usage_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter,
                                            IN const u8 index)
{
    pcrypto_adapter->isSecurekey = 1;
    pcrypto_adapter->securekeyIndex = index;

    //CRYPTO_Type *pcrypto;
    //pcrypto = pcrypto_adapter->base_addr;

    //pcrypto->ipskey_lock_reg_b.sk_wb_byte_swap = 1;
    //pcrypto->ipskey_lock_reg_b.sk_wb_word_swap = 1;

    return SUCCESS;
}

int hal_crypto_key_storage_deusage_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter)
{
    pcrypto_adapter->isSecurekey = 0;
    //pcrypto_adapter->securekeyIndex = 0;

    //CRYPTO_Type *pcrypto;
    //pcrypto = pcrypto_adapter->base_addr;

    //pcrypto->ipskey_lock_reg_b.sk_wb_byte_swap = 0;
    //pcrypto->ipskey_lock_reg_b.sk_wb_word_swap = 0;

    return SUCCESS;
}

int hal_crypto_key_storage_writeback_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter,
                                               IN const u8 index)
{
    pcrypto_adapter->isWriteback = 1;
    pcrypto_adapter->writebackIndex = index;

    return SUCCESS;
}

int hal_crypto_key_storage_dewriteback_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter)
{
    pcrypto_adapter->isWriteback = 0;
    //pcrypto_adapter->writebackIndex = 0;

    return SUCCESS;
}

int hal_crypto_keypad_storage_write_lock_result_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter,
                                              IN const u8 index)
{
    u8 lock_result = 0;

    CRYPTO_Type *pcrypto;
    pcrypto = pcrypto_adapter->base_addr;

    switch (index) {
        case SKPDR_0:
            lock_result = pcrypto->ipskeypad_locklen0_reg_b.keypad_lock;
            break;
        case SKPDR_1:
            lock_result = pcrypto->ipskeypad_locklen1_reg_b.keypad_lock;
            break;
        case SKPDR_2:
            lock_result = pcrypto->ipskeypad_locklen2_reg_b.keypad_lock;
            break;
        case SKPDR_3:
            lock_result = pcrypto->ipskeypad_locklen3_reg_b.keypad_lock;
            break;
        case SKPDR_4:
            lock_result = pcrypto->ipskeypad_locklen4_reg_b.keypad_lock;
            break;
        case SKPDR_5:
            lock_result = pcrypto->ipskeypad_locklen5_reg_b.keypad_lock;
            break;
        case SKPDR_6:
            lock_result = pcrypto->ipskeypad_locklen6_reg_b.keypad_lock;
            break;
        case SKPDR_7:
            lock_result = pcrypto->ipskeypad_locklen7_reg_b.keypad_lock;
            break;
        case SKPDR_8:
            lock_result = pcrypto->ipskeypad_locklen8_reg_b.keypad_lock;
            break;
        case SKPDR_9:
            lock_result = pcrypto->ipskeypad_locklen9_reg_b.keypad_lock;
            break;
        case SKPDR_10:
            lock_result = pcrypto->ipskeypad_locklen10_reg_b.keypad_lock;
            break;
        case SKPDR_11:
            lock_result = pcrypto->ipskeypad_locklen11_reg_b.keypad_lock;
            break;
    }

    return lock_result;
}

int hal_crypto_keypad_storage_write_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter,
                                              IN const u8* keypadin,
                                              IN const u8 index,
                                              IN const u8 keypadlock,
                                              IN const u32 keypadlen)
{
    u32 i;
    u8 keypadinTmp[CRYPTO_PADSIZE];

    CRYPTO_Type *pcrypto;
    pcrypto = pcrypto_adapter->base_addr;

    // set keypadin to ipsskp_reg
    rtlc_memset(keypadinTmp, 0x0, CRYPTO_PADSIZE);
    rtlc_memcpy(keypadinTmp, keypadin, keypadlen);

    for (i = 0; i < 16; i++) {
        pcrypto->ipsskp_reg[index][i] = (((u32)(keypadinTmp)[0+4*i] << 24) |
                                         ((u32)(keypadinTmp)[1+4*i] << 16) |
                                         ((u32)(keypadinTmp)[2+4*i] << 8)  |
                                         ((u32)(keypadinTmp)[3+4*i] << 0));
        //hal_crypto_keypad_storage_read_rtl8277c(pcrypto_adapter, index);
    }

    // write lock keypad_lock
    if (keypadlock == 1) {
        if (index == SKPDR_0) {
            pcrypto->ipskeypad_locklen0_reg_b.keypad_lock = 1;
        } else if (index == SKPDR_1) {
            pcrypto->ipskeypad_locklen1_reg_b.keypad_lock = 1;
        } else if (index == SKPDR_2) {
            pcrypto->ipskeypad_locklen2_reg_b.keypad_lock = 1;
        } else if (index == SKPDR_3) {
            pcrypto->ipskeypad_locklen3_reg_b.keypad_lock = 1;
        } else if (index == SKPDR_4) {
            pcrypto->ipskeypad_locklen4_reg_b.keypad_lock = 1;
        } else if (index == SKPDR_5) {
            pcrypto->ipskeypad_locklen5_reg_b.keypad_lock = 1;
        } else if (index == SKPDR_6) {
            pcrypto->ipskeypad_locklen6_reg_b.keypad_lock = 1;
        } else if (index == SKPDR_7) {
            pcrypto->ipskeypad_locklen7_reg_b.keypad_lock = 1;
        } else if (index == SKPDR_8) {
            pcrypto->ipskeypad_locklen8_reg_b.keypad_lock = 1;
        } else if (index == SKPDR_9) {
            pcrypto->ipskeypad_locklen9_reg_b.keypad_lock = 1;
        } else if (index == SKPDR_10) {
            pcrypto->ipskeypad_locklen10_reg_b.keypad_lock = 1;
        } else if (index == SKPDR_11) {
            pcrypto->ipskeypad_locklen11_reg_b.keypad_lock = 1;
        }
    }
    pcrypto_adapter->securekeypad[index] = 1;

    return SUCCESS;
}

int hal_crypto_keypad_storage_usage_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter,
                                            IN const u8 index,
                                            IN const u8 keypadlen)
{
    CRYPTO_Type *pcrypto;
    pcrypto = pcrypto_adapter->base_addr;

    u8 keypadlen_8byte_unit;

    if (keypadlen % 8 == 0) {
        keypadlen_8byte_unit = keypadlen / 8;
    } else {
        keypadlen_8byte_unit = (keypadlen / 8) + 1;
    }

    DBG_CRYPTO_INFO("keypadlen_8byte_unit = %d\n", keypadlen_8byte_unit);

    // set keypadlen (8-byte unit)
    switch (index) {
        case SKPDR_0:
            pcrypto->ipskeypad_locklen0_reg_b.keypad_len = keypadlen_8byte_unit;
            break;
        case SKPDR_1:
            pcrypto->ipskeypad_locklen1_reg_b.keypad_len = keypadlen_8byte_unit;
            break;
        case SKPDR_2:
            pcrypto->ipskeypad_locklen2_reg_b.keypad_len = keypadlen_8byte_unit;
            break;
        case SKPDR_3:
            pcrypto->ipskeypad_locklen3_reg_b.keypad_len = keypadlen_8byte_unit;
            break;
        case SKPDR_4:
            pcrypto->ipskeypad_locklen4_reg_b.keypad_len = keypadlen_8byte_unit;
            break;
        case SKPDR_5:
            pcrypto->ipskeypad_locklen5_reg_b.keypad_len = keypadlen_8byte_unit;
            break;
        case SKPDR_6:
            pcrypto->ipskeypad_locklen6_reg_b.keypad_len = keypadlen_8byte_unit;
            break;
        case SKPDR_7:
            pcrypto->ipskeypad_locklen7_reg_b.keypad_len = keypadlen_8byte_unit;
            break;
        case SKPDR_8:
            pcrypto->ipskeypad_locklen8_reg_b.keypad_len = keypadlen_8byte_unit;
            break;
        case SKPDR_9:
            pcrypto->ipskeypad_locklen9_reg_b.keypad_len = keypadlen_8byte_unit;
            break;
        case SKPDR_10:
            pcrypto->ipskeypad_locklen10_reg_b.keypad_len = keypadlen_8byte_unit;
            break;
        case SKPDR_11:
            pcrypto->ipskeypad_locklen11_reg_b.keypad_len = keypadlen_8byte_unit;
            break;
    }

    pcrypto_adapter->isSecurekeypad = 1;
    pcrypto_adapter->securekeypadIndex = index;

    DBG_CRYPTO_INFO("securekeypadIndex = %d\n", pcrypto_adapter->securekeypadIndex);

    return SUCCESS;
}

int hal_crypto_keypad_storage_deusage_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter,
                                            IN const u8 index)
{
    CRYPTO_Type *pcrypto;
    pcrypto = pcrypto_adapter->base_addr;

    u8 keypadlen_8byte_unit_default = 8;

    // set keypadlen (8-byte unit)
    switch (index) {
        case SKPDR_0:
            pcrypto->ipskeypad_locklen0_reg_b.keypad_len = keypadlen_8byte_unit_default;
            break;
        case SKPDR_1:
            pcrypto->ipskeypad_locklen1_reg_b.keypad_len = keypadlen_8byte_unit_default;
            break;
        case SKPDR_2:
            pcrypto->ipskeypad_locklen2_reg_b.keypad_len = keypadlen_8byte_unit_default;
            break;
        case SKPDR_3:
            pcrypto->ipskeypad_locklen3_reg_b.keypad_len = keypadlen_8byte_unit_default;
            break;
        case SKPDR_4:
            pcrypto->ipskeypad_locklen4_reg_b.keypad_len = keypadlen_8byte_unit_default;
            break;
        case SKPDR_5:
            pcrypto->ipskeypad_locklen5_reg_b.keypad_len = keypadlen_8byte_unit_default;
            break;
        case SKPDR_6:
            pcrypto->ipskeypad_locklen6_reg_b.keypad_len = keypadlen_8byte_unit_default;
            break;
        case SKPDR_7:
            pcrypto->ipskeypad_locklen7_reg_b.keypad_len = keypadlen_8byte_unit_default;
            break;
        case SKPDR_8:
            pcrypto->ipskeypad_locklen8_reg_b.keypad_len = keypadlen_8byte_unit_default;
            break;
        case SKPDR_9:
            pcrypto->ipskeypad_locklen9_reg_b.keypad_len = keypadlen_8byte_unit_default;
            break;
        case SKPDR_10:
            pcrypto->ipskeypad_locklen10_reg_b.keypad_len = keypadlen_8byte_unit_default;
            break;
        case SKPDR_11:
            pcrypto->ipskeypad_locklen11_reg_b.keypad_len = keypadlen_8byte_unit_default;
            break;
    }

    pcrypto_adapter->isSecurekeypad = 0;
    //pcrypto_adapter->securekeypadIndex = index;

    return SUCCESS;
}

#if defined(CONFIG_BUILD_NONSECURE) && (CONFIG_BUILD_NONSECURE == 1)
//
// CRC
//

//
// crc32
//
//#define CRC_DMA_MODE 1
//#define CRC_POLLING  0

/**
 *  \fn          uint8_t hal_crc_isbusy_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter)
 *  \brief       Check whether crc engine is busy.
 *  \param[in]   pcrypto_adapter  Pointer to crypto adapter data
 *  \return      regValue The value of the register
 */
uint8_t hal_crc_isbusy_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter)
{
    CRYPTO_Type *pcrypto;
    pcrypto = pcrypto_adapter->base_addr;
    volatile uint8_t regValue;

    regValue = pcrypto->crc_stat_reg_b.busy;

    return regValue;
}

/**
 *  \fn          void hal_crc_set_dma_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter, u8 enabled)
 *  \brief       Enable/Disable CRC engine DMA mode.
 *  \param[in]   pcrypto_adapter  Pointer to crypto adapter data
 *  \param[in]   enabled Enable/Disable CRC engine DMA mode: 1=enable, 0=disable
 *  \return      void
 */
void hal_crc_set_dma_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter, u8 enabled)
{
    CRYPTO_Type *pcrypto;
    pcrypto = pcrypto_adapter->base_addr;
    volatile uint32_t regValue;

    regValue = pcrypto->crc_op_reg;

    if (enabled) {
        regValue |= CRC_104_DMA;
    } else {
        regValue &= ~((size_t)(CRC_104_DMA));
    }

    pcrypto->crc_op_reg = regValue;
}

/**
 *  \fn          void hal_crc_set_swap_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter, u8 iswap_type, u8 oswap)
 *  \brief       Write the input swap and output swap value to the CRC operation register.
 *  \param[in]   pcrypto_adapter  Pointer to crypto adapter data
 *  \param[in]   iswap_type The input swap value that is ready to write
 *  \param[in]   oswap The output swap value that is ready to write
 *  \return      none
 */
void hal_crc_set_swap_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter, u8 iswap_type, u8 oswap)
{
    CRYPTO_Type *pcrypto;
    pcrypto = pcrypto_adapter->base_addr;
    volatile uint32_t regValue;

    regValue = pcrypto->crc_op_reg;

    //
    regValue &= ~((size_t)(0xF));
    regValue |= ((size_t)(iswap_type)) & 0x7;
    regValue |= ((size_t)(oswap & 0x1)) << 3;
    //

    pcrypto->crc_op_reg = regValue;
}

/**
 *  \fn          void hal_crc_set_crc_type_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter, u8 crc_data_type)
 *  \brief       Write the crc type to the CRC operation register.
 *  \param[in]   pcrypto_adapter  Pointer to crypto adapter data
 *  \param[in]   crc_data_type The crc type(depends on crc bit value)that is ready to write
 *               - 32 bits: 3'd0
 *               - 24 bits: 3'd1
 *               - 16 bits: 3'd2
 *               - 12 bits: 3'd3
 *               - 10 bits: 3'd4
 *               -  8 bits: 3'd5
 *               -  7 bits: 3'd6
 *               -  5 bits: 3'd7
 *  \return      void
 */
void hal_crc_set_crc_type_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter, u8 crc_data_type)
{
    CRYPTO_Type *pcrypto;
    pcrypto = pcrypto_adapter->base_addr;
    volatile uint32_t regValue;

    regValue = pcrypto->crc_op_reg;

    //
    regValue &= ~(((size_t)(0x7)) << 4);
    regValue |= (crc_data_type & 0x7) << 4;

    pcrypto->crc_op_reg = regValue;
}

/**
 *  \fn          void hal_crc_set_byte_type_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter, u8 byte_type)
 *  \brief       Write the crc type to the CRC operation register.
 *  \param[in]   pcrypto_adapter  Pointer to crypto adapter data
 *  \param[in]   byte_type The byte type that is ready to write
 *               - 32 bits: 2'd0
 *               - 24 bits: 2'd3
 *               - 16 bits: 2'd2
 *               - 8 bits:  2'd1
 *  \return      none
 */
void hal_crc_set_byte_type_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter, u8 byte_type)
{
    CRYPTO_Type *pcrypto;
    pcrypto = pcrypto_adapter->base_addr;
    volatile uint32_t regValue;

    regValue = pcrypto->crc_op_reg;

    regValue &= ~(((size_t)(0x3)) << 8);
    regValue |= (size_t)(byte_type & 0x3) << 8;

    pcrypto->crc_op_reg = regValue;
}

/**
 *  \fn          void hal_crc_set_data_length_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter, uint32_t data_len)
 *  \brief       Write the data length(bytes) to the CRC operation register.
 *  \param[in]   pcrypto_adapter  Pointer to crypto adapter data
 *  \param[in]   data_len The data length(bytes) that is ready to write
 *  \return      none
 */
void hal_crc_set_data_length_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter, uint32_t data_len)
{
    CRYPTO_Type *pcrypto;
    pcrypto = pcrypto_adapter->base_addr;
    volatile uint32_t regValue;

    regValue = pcrypto->crc_op_reg;

    regValue &= ~((size_t)0xFFFFF000);

    if (data_len != 0) { // not last
        regValue |= data_len << 16;
    } else {
        regValue |= 0x1000;
    }

    pcrypto->crc_op_reg = regValue;
}

#if 0
void check_registers(hal_crypto_adapter_t *pcrypto_adapter)
{
    CRYPTO_Type *pcrypto;
    pcrypto = pcrypto_adapter->base_addr;
    volatile uint32_t regValue;
    uint32_t crc_length;
    uint32_t crc_last;
    uint32_t crc_be;
    uint32_t crc_dma;
    uint32_t crc_sel;
    uint32_t crc_oswap;
    uint32_t crc_iswap;
    uint32_t crc_SDBP;
    uint32_t crc_oxor;
    uint32_t crc_poly;
    uint32_t crc_init;


    regValue = pcrypto->crc_op_reg;
    DBG_CRYPTO_INFO("[0x104] : 0x%x \r\n", regValue);
    crc_length = regValue >> 16;
    crc_last = (regValue >> 12) & 0x1;
    crc_be = (regValue >> 8) & 0x3;
    crc_dma = (regValue >> 7) & 0x1;
    crc_sel = (regValue >> 4) & 0x7;
    crc_oswap = (regValue >> 3) & 0x1;
    crc_iswap = (regValue ) & 0x7;


    crc_SDBP = pcrypto->crc_data_reg;
    crc_oxor = pcrypto->crc_oxor_reg;
    crc_poly = pcrypto->crc_poly_reg;;
    crc_init = pcrypto->crc_iv_reg;;

    DBG_CRYPTO_INFO("DMA: 0x%x \r\n", crc_dma);
    DBG_CRYPTO_INFO("SDBP: 0x%x \r\n", crc_SDBP);
    DBG_CRYPTO_INFO("len: 0x%x \r\n", crc_length);
    DBG_CRYPTO_INFO("last: 0x%x \r\n", crc_last);
    DBG_CRYPTO_INFO("be: 0x%x \r\n", crc_be);
    DBG_CRYPTO_INFO("oxor: 0x%x \r\n", crc_oxor);
    DBG_CRYPTO_INFO("poly: 0x%x \r\n", crc_poly);
    DBG_CRYPTO_INFO("init: 0x%x \r\n", crc_init);
    DBG_CRYPTO_INFO("iswap: 0x%x \r\n", crc_iswap);
    DBG_CRYPTO_INFO("oswap: 0x%x \r\n", crc_oswap);
    DBG_CRYPTO_INFO("sel: 0x%x \r\n", crc_sel);
}
#endif

/**
 *  \fn          void read_crc_setting_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter)
 *  \brief       Read the current crc setting values(\b Order, \b Refout, \b Refin, \b XOR, \b Initial_Value, \b Polynomial)
 *               from crc registers .
 *  \param[in]   pcrypto_adapter  Pointer to crypto adapter data
 *  \return      none
 */
void read_crc_setting_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter){
    CRYPTO_Type *pcrypto;
    pcrypto = pcrypto_adapter->base_addr;
    volatile uint32_t regValue;

    regValue = pcrypto->crc_op_reg;
    DBG_CRYPTO_INFO("[0x104] : 0x%x \r\n", regValue);

    pcrypto_adapter->crc_sel = (regValue >> 4) & 0x7;
    pcrypto_adapter->crc_oswap = (regValue >> 3) & 0x1;
    pcrypto_adapter->crc_iswap = (regValue ) & 0x7;

    pcrypto_adapter->crc_oxor = pcrypto->crc_oxor_reg;
    pcrypto_adapter->crc_init = pcrypto->crc_iv_reg;
    pcrypto_adapter->crc_poly = pcrypto->crc_poly_reg;
#if 1
    DBG_CRYPTO_INFO("sel: 0x%x \r\n", pcrypto_adapter->crc_sel);
    DBG_CRYPTO_INFO("poly: 0x%x \r\n", pcrypto_adapter->crc_poly);
    DBG_CRYPTO_INFO("init: 0x%x \r\n", pcrypto_adapter->crc_init);
    DBG_CRYPTO_INFO("oxor: 0x%x \r\n", pcrypto_adapter->crc_oxor);
    DBG_CRYPTO_INFO("iswap: 0x%x \r\n", pcrypto_adapter->crc_iswap);
    DBG_CRYPTO_INFO("oswap: 0x%x \r\n", pcrypto_adapter->crc_oswap);
#endif
}

/**
 *  \fn          int hal_crypto_crc_wait_done_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter)
 *  \brief       CRC command mode uses the polling way to check crc interrupt. If it waits over timeout value, then return FAIL.
 *  \param[in]   pcrypto_adapter  Pointer to crypto adapter data
 *  \return      value ==  0     success
 *  \return      value == -1     fail
 */
int hal_crypto_crc_wait_done_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter)
{
    CRYPTO_Type *pcrypto;
    pcrypto = pcrypto_adapter->base_addr;
    volatile uint32_t regValue;
    uint32_t i;

    i = 0;
    while (i < CRC_TIMEOUT) {
        regValue = pcrypto->crc_stat_reg;
        if (regValue & CRC_118_INTR) {
            break;
        }
        i++;
    }
    if (i == CRC_TIMEOUT) {
        DBG_CRYPTO_ERR("CRC wait done failed \r\n");
        return FAIL;
    } else {
        regValue = pcrypto->crc_result_reg;
        //DBG_CRYPTO_INFO("%s : 0x%x \r\n", __FUNCTION__, regValue);
        //clear interrupt
        //HAL_CRYPTO_WRITE32(REG_CRC_STAT, CRC_118_INTR);
        pcrypto->crc_stat_reg_b.crc_ok = 1;
    }
    return SUCCESS;
}

/**
 *  \fn          uint32_t  hal_crypto_crc_get_result_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter)
 *  \brief       Read crc result register, and return the crc result value.
 *  \param[in]   pcrypto_adapter  Pointer to crypto adapter data
 *  \return      regValue Get crc result value
 */
uint32_t  hal_crypto_crc_get_result_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter)
{
    CRYPTO_Type *pcrypto;
    pcrypto = pcrypto_adapter->base_addr;
    volatile uint32_t regValue;

    regValue = pcrypto->crc_result_reg;

    return regValue;
}

/**
 *  \fn          int hal_crypto_crc_setting_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter, IN int order,
 *                                                   IN uint32_t polynom, IN uint32_t crcinit, IN uint32_t crcxor,
 *                                                   IN uint32_t refin, IN uint32_t refout)
 *  \brief       Set the basic parameters that crc algorithm needs to crc registers.
 *  \param[in]   pcrypto_adapter  Pointer to crypto adapter data
 *  \param[in]   order CRC polynomial order
 *  \param[in]   polynom CRC polynomial coefficients
 *  \param[in]   crcinit CRC initial value
 *  \param[in]   crcxor CRC XOR output value
 *  \param[in]   refin CRC input swap value
 *  \param[in]   refout CRC output swap value
 *  \return      value ==  0     success
 *  \return      value == -1     fail
 */
int hal_crypto_crc_setting_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter,
    IN int order, IN uint32_t polynom, IN uint32_t crcinit, IN uint32_t crcxor, IN uint32_t refin, IN uint32_t refout)
{
    u8 crc_data_type;
    u8 iswap, oswap;
    CRYPTO_Type *pcrypto;
    volatile uint32_t regValue;

    if (pcrypto_adapter == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if ((pcrypto_adapter->base_addr) == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    } else {
        pcrypto = pcrypto_adapter->base_addr;
    }

    // check if busy
    if (hal_crc_isbusy_rtl8277c(pcrypto_adapter)) {
        DBG_CRYPTO_ERR("CRC engine is busy\r\n");
        return FAIL;
    }

    // check polynom
    if ((polynom & 0x1) != 0x1) {
        DBG_CRYPTO_ERR("CRC poly needs to be odd\r\n");
        return FAIL;
    }
    pcrypto->crc_poly_reg = polynom;
    regValue = pcrypto->crc_poly_reg;
    DBG_CRYPTO_INFO("REG_CRC_POLY: 0x%x \r\n", regValue);
    if (regValue == 0) {
        return FAIL;
    }

    // init
    pcrypto->crc_iv_reg = crcinit;

    // oxor
    pcrypto->crc_oxor_reg = crcxor;

    // refin , refout
    iswap = (refin)? 4 : 0;
    oswap = (refout)? 1 : 0;
    hal_crc_set_swap_rtl8277c(pcrypto_adapter, iswap, oswap);

    // sel
    // order : crc type
    pcrypto_adapter->crc_order = order;
    switch (order) {
        case 32:
            crc_data_type = 0;
            break;
        case 24:
            crc_data_type = 1;
            break;
        case 16:
            crc_data_type = 2;
            break;
        case 12:
            crc_data_type = 3;
            break;
        case 10:
            crc_data_type = 4;
            break;
        case 8:
            crc_data_type = 5;
            break;
        case 7:
            crc_data_type = 6;
            break;
        case 5:
            crc_data_type = 7;
            break;
        default:
            DBG_CRYPTO_ERR("CRC order : %d not support \r\n", order);
            return FAIL;
    }
    hal_crc_set_crc_type_rtl8277c(pcrypto_adapter, crc_data_type);

    return SUCCESS;
}

/**
 *  \fn          int hal_crypto_crc_cmd_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter, IN const u8* message, IN const uint32_t msglen)
 *  \brief       Use CRC command mode to calculate this crc message buffer.
 *  \param[in]   pcrypto_adapter  Pointer to crypto adapter data
 *  \param[in]   message Pointer to CRC message buffer
 *  \param[in]   msglen The crc message buffer length
 *  \return      value ==  0     success
 *  \return      value == -1     fail
 */
int hal_crypto_crc_cmd_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter, IN const u8* message, IN const uint32_t msglen)
{
    CRYPTO_Type *pcrypto;
    int ret = SUCCESS;

    if (pcrypto_adapter == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if ((pcrypto_adapter->base_addr) == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    } else {
        pcrypto = pcrypto_adapter->base_addr;
    }
    // check if busy
    if (hal_crc_isbusy_rtl8277c(pcrypto_adapter)) {
        DBG_CRYPTO_ERR("[CRC_CMD] CRC engine is busy\r\n");
        return FAIL;
    }

    uint32_t data;
    int msglen_4bytes;
    int i;

    hal_crc_set_dma_rtl8277c(pcrypto_adapter, 0);

    // set data length
    hal_crc_set_data_length_rtl8277c(pcrypto_adapter, msglen);

    //crc start polling
    // MSK : disable intr
    pcrypto->crc_stat_reg_b.crc_ok = 1;
    pcrypto->crc_stat_reg_b.crc_intr_mask = 1;
    pcrypto->crc_stat_reg_b.crc_little_endian = 1;

    // reset crc engine
    pcrypto->crc_rst_reg = 0x1;

    /***********************************************
     * Note:
     * Command mode need process 4bytes and 1 byte whether the msglen is only 4 times.
     *
     ************************************************/

    //32 bit
    hal_crc_set_byte_type_rtl8277c(pcrypto_adapter, 0);

    msglen_4bytes = (msglen / 4) * 4;
    //msglen_rest = msglen - msglen_4bytes;
    DBG_CRYPTO_INFO("[CRC_CMD] msglen_4bytes = %d\r\n",msglen_4bytes);

    for (i = 0; i < msglen_4bytes; i += 4) {
        data = (size_t)(message[i]);
        data |= (size_t)(message[i+1]) << 8;
        data |= (size_t)(message[i+2]) << 16;
        data |= (size_t)(message[i+3]) << 24;

        pcrypto->crc_data_reg = data;
    }

    DBG_CRYPTO_INFO("[CRC_CMD] msglen_rest = %d, count_byte = %d\r\n",(msglen - msglen_4bytes),i);
    if ((msglen - msglen_4bytes) > 0) {
        //8bit
        hal_crc_set_byte_type_rtl8277c(pcrypto_adapter, 1);
        for (; (size_t)(i) < msglen; i++) {
            data = message[i];
            pcrypto->crc_data_reg = data;
        }
    } else if ((msglen - msglen_4bytes) == 0) {
        //8bit
        hal_crc_set_byte_type_rtl8277c(pcrypto_adapter, 1);
        data = 0;
        pcrypto->crc_data_reg = data;
    }

    // polling way to check interrupt
    ret = hal_crypto_crc_wait_done_rtl8277c(pcrypto_adapter);
    return ret;
}

/**
 *  \fn          int hal_crypto_crc_dma_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter, IN const u8* message, IN const uint32_t msglen)
 *  \brief       Use CRC DMA mode to calculate this crc message buffer.
 *  \note        DMA mode need to check whether is 32bytes alignment, because of cache clean.
 *               - If it's not-32bytes alignment address,but the space in cache line is enough for message,
 *               then copy message to a 32bytes aligment address and jump to dma_process.
 *               - If it's not-32bytes alignment address,and the space in cache line isn't enough for message,
 *               then do cmd mode preprocess, after that jump to dma_process.
 *               - If it's 32bytes alignment address, do dma_proces directly.
 *  \param[in]   pcrypto_adapter  Pointer to crypto adapter data
 *  \param[in]   message Pointer to CRC message buffer
 *  \param[in]   msglen The crc message buffer length
 *  \return      value ==  0     success
 *  \return      value == -1     fail
 */
int hal_crypto_crc_dma_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter, IN const u8* message, IN const uint32_t msglen)
{
    CRYPTO_Type *pcrypto;
    volatile uint32_t regValue;
    int ret = SUCCESS;
    uint32_t cache_len = (msglen & 0x1F)?((msglen/32) + 1) * 32 : msglen;
    int8_t first_cache_pos = 0;
    int8_t first_cache_len = 0;
    int8_t is_cmdpreprocess = 0;
    uint32_t tmp_crcinit;
    uint32_t last_msglen;

    if (pcrypto_adapter == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if ((pcrypto_adapter->base_addr) == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    } else {
        pcrypto = pcrypto_adapter->base_addr;
    }

    // check if busy
    if (hal_crc_isbusy_rtl8277c(pcrypto_adapter)) {
        DBG_CRYPTO_ERR("[CRC_DMA] CRC engine is busy\r\n");
        return FAIL;
    }

    /***********************************************
    * Note:
    * DMA mode need to check whether is 32bytes alignment, because of cache clean.
    * - If not-32bytes alignment addr,but the space in cache line is enough for message,
    *   then copy message to a 32bytes aligment addr and jump to dma_process.
    * - If not-32bytes alignment addr,and the space in cache line isn't enough for message,
    *   then do cmd mode preporcess, after jump to dma_process.
    * - If 32bytes alignment addr, do dma_proces.
    *
    ************************************************/
    first_cache_pos = (size_t)message & 0x1F;
    first_cache_len = (32 - first_cache_pos) % 32;
    DBG_CRYPTO_INFO("[CRC_DMA] f_cache_pos = %d, f_cache_len = %d, msglen = %d\r\n", first_cache_pos, first_cache_len, msglen);

    if (first_cache_pos) {
        if (first_cache_len >= msglen) {
            DBG_CRYPTO_INFO("[CRC_DMA] msg_addr not 32 aligned & cache_len >= msglen\r\n");
            rtlc_memcpy((void *)&(pcrypto_adapter->crc_first_cache_line[0]),(const void *)message,msglen);
            message = (const u8 *)&(pcrypto_adapter->crc_first_cache_line[0]);
            goto dma_process;
        } else {
            //cmd mode preporcess
            is_cmdpreprocess = TRUE;
            read_crc_setting_rtl8277c(pcrypto_adapter);

            //reset_crc_setting for cmd(refout,crcxor)
            pcrypto->crc_oxor_reg = (size_t)0x0;
            hal_crc_set_swap_rtl8277c(pcrypto_adapter, pcrypto_adapter->crc_iswap, 0x0);
            regValue = pcrypto->crc_op_reg;
            DBG_CRYPTO_INFO("[0x104] : 0x%x \r\n", regValue);

            // check if busy
            if (hal_crc_isbusy_rtl8277c(pcrypto_adapter)) {
                DBG_CRYPTO_ERR("[CRC_DMA] After set CRC, CRC engine is busy\r\n");
                return FAIL;
            }

            ret = hal_crypto_crc_cmd_rtl8277c(pcrypto_adapter, message, first_cache_len);
            if (ret == SUCCESS) {
                tmp_crcinit = hal_crypto_crc_get_result_rtl8277c(pcrypto_adapter);
                message += first_cache_len;
                last_msglen = msglen;
                last_msglen -= first_cache_len;
                cache_len = (last_msglen & 0x1F)?((last_msglen/32)+1)*32:last_msglen;
                DBG_CRYPTO_INFO("[CRC_DMA] last_msglen =%d, cache_len = %d\r\n", last_msglen, cache_len);

                // check if busy
                if (hal_crc_isbusy_rtl8277c(pcrypto_adapter)) {
                    DBG_CRYPTO_ERR("[CRC_DMA] After CRC_CMD_preprocess, CRC engine is busy\r\n");
                    return FAIL;
                }
                //reset_crc_setting for dma(refout,crcxor,crcinit)
                DBG_CRYPTO_INFO("[CRC_DMA] last_crcinit = 0x%x \r\n", tmp_crcinit);
                //iv
                pcrypto->crc_iv_reg = tmp_crcinit;

                //oxor
                pcrypto->crc_oxor_reg = (pcrypto_adapter->crc_oxor);
                hal_crc_set_swap_rtl8277c(pcrypto_adapter, (pcrypto_adapter->crc_iswap), (pcrypto_adapter->crc_oswap));

                regValue = pcrypto->crc_op_reg;
                DBG_CRYPTO_INFO("[0x104] : 0x%x \r\n", regValue);

                goto dma_process;
            }else {
                DBG_CRYPTO_ERR("[CRC_DMA] CRC_CMD_preprocess fail!\r\n");
                return (ret);
            }
        }
    }else{

dma_process:

        //DMA mode use interrupt handler
        if (!(pcrypto_adapter->isIntMode)) {
            DBG_CRYPTO_ERR("[CRC_DMA] check interrupt handler fail!\r\n");
            return (FAIL);
        }

        //DBG_CRYPTO_INFO("DMA mode \r\n");
        hal_crc_set_dma_rtl8277c(pcrypto_adapter, 1);

        // set data length
        //DBG_CRYPTO_INFO("set data length\r\n");
        if (is_cmdpreprocess == TRUE) {
            DBG_CRYPTO_INFO("[CRC_DMA] last_msglen = %d\r\n",last_msglen);
            hal_crc_set_data_length_rtl8277c(pcrypto_adapter, last_msglen);
        } else {
            hal_crc_set_data_length_rtl8277c(pcrypto_adapter, msglen);
        }

        // Need 32 bytes alignment address
        arch_clean_dcache_by_size(pcrypto_adapter, (size_t)(message), cache_len);
        pcrypto->crc_data_reg = (size_t)(message);

        //DBG_CRYPTO_INFO("start\r\n");
        pcrypto->crc_stat_reg_b.crc_ok = 1;
        pcrypto->crc_stat_reg_b.crc_intr_mask = 0;
        pcrypto->crc_stat_reg_b.crc_little_endian = 1;
        pcrypto->crc_rst_reg = 0x1;

        //regValue = pcrypto->crc_stat_reg;
        //DBG_CRYPTO_INFO("[0x118] : 0x%x \r\n", regValue);
    }
    return ret;
}

/**
 *  \fn          int hal_crypto_crc32_cmd_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter, IN const u8* message, IN const uint32_t msglen)
 *  \brief       Use CRC command mode and crc32 type to calculate this crc message buffer.
 *  \details     It will call \ref hal_crypto_crc_setting_rtl8277c() to set crc32 type parameters,
 *               then call \ref hal_crypto_crc_cmd_rtl8277c() to calculate this crc message data via Command mode.
 *               crc32 type: \n
 *               - order: 32
 *               - polynomial: 0x4c11DB7
 *               - crcinit: 0xFFFFFFFF
 *               - crcxor: 0xFFFFFFFF
 *               - refin: true
 *               - refout: true
 *  \param[in]   pcrypto_adapter  Pointer to crypto adapter data
 *  \param[in]   message Pointer to CRC message buffer
 *  \param[in]   msglen The crc message buffer length
 *  \return      value ==  0     success
 *  \return      value == -1     fail
 */
int hal_crypto_crc32_cmd_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter, IN const u8* message, IN const uint32_t msglen)
{
    int ret;

    ret = hal_crypto_crc_setting_rtl8277c(pcrypto_adapter, 32, 0x4c11DB7, 0xFFFFFFFF, 0xFFFFFFFF, 1, 1);
    if (ret != SUCCESS) {
        return ret;
    }

    ret = hal_crypto_crc_cmd_rtl8277c(pcrypto_adapter, message, msglen);
    return ret;
}

/**
 *  \fn          int hal_crypto_crc32_dma_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter, IN const u8* message, IN const uint32_t msglen)
 *  \brief       Use CRC DMA mode and crc32 type to calculate this crc message buffer.
 *  \details     It will call \ref hal_crypto_crc_setting_rtl8277c() to set crc32 type parameters,
 *               then call \ref hal_crypto_crc_dma_rtl8277c() to calculate this crc message data via DMA mode.
 *               crc32 type: \n
 *               - order: 32
 *               - polynomial: 0x4c11DB7
 *               - crcinit: 0xFFFFFFFF
 *               - crcxor: 0xFFFFFFFF
 *               - refin: true
 *               - refout: true
 *  \param[in]   pcrypto_adapter  Pointer to crypto adapter data
 *  \param[in]   message Pointer to CRC message buffer
 *  \param[in]   msglen The crc message buffer length
 *  \return      value ==  0     success
 *  \return      value == -1     fail
 */
int hal_crypto_crc32_dma_rtl8277c(hal_crypto_adapter_t *pcrypto_adapter, IN const u8* message, IN const uint32_t msglen)
{
    int ret;
    ret = hal_crypto_crc_setting_rtl8277c(pcrypto_adapter, 32, 0x4c11DB7, 0xFFFFFFFF, 0xFFFFFFFF, 1, 1);
    if (ret != SUCCESS) {
        return ret;
    }

    ret = hal_crypto_crc_dma_rtl8277c(pcrypto_adapter, message, msglen);
    return ret;
}
#endif

#if defined(CONFIG_BUILD_NONSECURE) && (CONFIG_BUILD_NONSECURE == 1)

/**
 *   \brief  The data struct of CRYPTO non-secure stub functions. ROM code functions are accessed in RAM code through stub functions.
 */
SECTION_CRYPTO_STUBS
const hal_crypto_func_stubs_t hal_crypto_stubs_ns = {
    .hal_crypto_engine_init   = hal_crypto_engine_init_rtl8277c,
    .hal_crypto_engine_deinit = hal_crypto_engine_deinit_rtl8277c,
    .hal_crypto_engine_platform_en_ctrl = hal_crypto_en_ctrl_rtl8277c,

    .hal_crypto_auth         = hal_crypto_auth_rtl8277c,
    .hal_crypto_auth_init    = hal_crypto_auth_init_rtl8277c,
    .hal_crypto_auth_process = hal_crypto_auth_process_rtl8277c,
    .hal_crypto_auth_update  = hal_crypto_auth_update_rtl8277c,
    .hal_crypto_auth_final   = hal_crypto_auth_final_rtl8277c,

    .hal_crypto_cipher_init     = hal_crypto_cipher_init_rtl8277c,
    .hal_crypto_cipher_encrypt  = hal_crypto_cipher_encrypt_rtl8277c,
    .hal_crypto_cipher_decrypt  = hal_crypto_cipher_decrypt_rtl8277c,

    .hal_crypto_mix_mode_init     = hal_crypto_mix_mode_init_rtl8277c,
    .hal_crypto_mix_mode_process  = hal_crypto_mix_mode_process_rtl8277c,

    //.hal_crypto_key_storage_writekey  = hal_crypto_key_storage_writekey_rtl8277c,
    //.hal_crypto_key_storage_writeback = hal_crypto_key_storage_writeback_rtl8277c,
    //.hal_crypto_key_storage_readkey   = hal_crypto_key_storage_readkey_rtl8277c,

    .hal_crypto_crc_setting       = hal_crypto_crc_setting_rtl8277c,
    .hal_crypto_crc_cmd           = hal_crypto_crc_cmd_rtl8277c,
    .hal_crypto_crc_dma           = hal_crypto_crc_dma_rtl8277c,
    .hal_crypto_crc_get_result    = hal_crypto_crc_get_result_rtl8277c,
    .hal_crypto_crc32_cmd         = hal_crypto_crc32_cmd_rtl8277c,
    .hal_crypto_crc32_dma         = hal_crypto_crc32_dma_rtl8277c,

    .hal_crypto_engine            = hal_crypto_engine_rtl8277c,
    .hal_crypto_engine_get_result = hal_crypto_engine_get_result_rtl8277c,
    .hal_crypto_set_srcdesc       = hal_crypto_set_srcdesc_rtl8277c,
    .hal_crypto_set_dstdesc       = hal_crypto_set_dstdesc_rtl8277c,
    .hal_crypto_engine_setup_cl_buffer = hal_crypto_engine_setup_cl_buffer_rtl8277c,
};

#else

/**
 *   \brief  The data struct of CRYPTO secure stub functions. ROM code functions are accessed in RAM code through stub functions.
 */
SECTION_CRYPTO_STUBS
const hal_crypto_func_stubs_t hal_crypto_stubs_s = {
    .hal_crypto_engine_init   = hal_crypto_engine_init_rtl8277c,
    .hal_crypto_engine_deinit = hal_crypto_engine_deinit_rtl8277c,
    .hal_crypto_engine_platform_en_ctrl = hal_crypto_en_ctrl_rtl8277c,

    .hal_crypto_auth         = hal_crypto_auth_rtl8277c,
    .hal_crypto_auth_init    = hal_crypto_auth_init_rtl8277c,
    .hal_crypto_auth_process = hal_crypto_auth_process_rtl8277c,
    .hal_crypto_auth_update  = hal_crypto_auth_update_rtl8277c,
    .hal_crypto_auth_final   = hal_crypto_auth_final_rtl8277c,

    .hal_crypto_cipher_init     = hal_crypto_cipher_init_rtl8277c,
    .hal_crypto_cipher_encrypt  = hal_crypto_cipher_encrypt_rtl8277c,
    .hal_crypto_cipher_decrypt  = hal_crypto_cipher_decrypt_rtl8277c,

    .hal_crypto_mix_mode_init     = hal_crypto_mix_mode_init_rtl8277c,
    .hal_crypto_mix_mode_process  = hal_crypto_mix_mode_process_rtl8277c,

    //.hal_crypto_key_storage_writekey  = hal_crypto_key_storage_writekey_rtl8277c,
    //.hal_crypto_key_storage_writeback = hal_crypto_key_storage_writeback_rtl8277c,
    //.hal_crypto_key_storage_readkey   = hal_crypto_key_storage_readkey_rtl8277c,

    .hal_crypto_engine            = hal_crypto_engine_rtl8277c,
    .hal_crypto_engine_get_result = hal_crypto_engine_get_result_rtl8277c,
    .hal_crypto_set_srcdesc       = hal_crypto_set_srcdesc_rtl8277c,
    .hal_crypto_set_dstdesc       = hal_crypto_set_dstdesc_rtl8277c,
    .hal_crypto_engine_setup_cl_buffer = hal_crypto_engine_setup_cl_buffer_rtl8277c,
};
#endif

/** @} */ /* End of group hs_hal_crypto_rom_func */
/// @endcond
/** @} */ /* End of group hs_hal_crypto */
