/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#include "rtl8195a.h"
#include "rtl8195a_crypto_rom.h"

#undef printk
#define printk 	DiagPrintf
//
// options setting
//

// LEXRA
#define LEXRA_BIG_ENDIAN	0


// macro function definitions


#define HAL_CRYPTO_WRITE32(addr, value32)  HAL_WRITE32(CRYPTO_REG_BASE, (addr), (value32))
#define HAL_CRYPTO_WRITE16(addr, value16)  HAL_WRITE16(CRYPTO_REG_BASE, (addr), (value16))
#define HAL_CRYPTO_WRITE8(addr, value8)    HAL_WRITE8(CRYPTO_REG_BASE, (addr), (value8))
#define HAL_CRYPTO_READ32(addr)            HAL_READ32(CRYPTO_REG_BASE, (addr))
#define HAL_CRYPTO_READ16(addr)            HAL_READ16(CRYPTO_REG_BASE, (addr))
#define HAL_CRYPTO_READ8(addr)             HAL_READ8(CRYPTO_REG_BASE, (addr))


// registers


#define REG_IPSSDSTS		(0x00)	// IPSec Source Descriptor Status
#define REG_IPSSD1			(0x04)	// IPSec Source Descriptor first world setting
#define REG_IPSSD2			(0x08)	// IPSec Source Descriptor second world setting


#define REG_IPSDDSTS		(0x1000)	// IPSec Source Descriptor Status
#define REG_IPSDD1			(0x1004)	// IPSec Source Descriptor first world setting
#define REG_IPSDD2			(0x1008)	// IPSec Source Descriptor second world setting


#define REG_IPSCSR_RESET_POLL_STATUS	(0x10) // IPSec Command/Status Register, for reset, poll status, 
#define REG_IPSCSR_INT_MASK				(0x14) // IPSec Command/Status Register, for interrupt mask
#define REG_IPSCSR_DEBUG				(0x18) // IPSec Command/Status Register, for debug
#define REG_IPSCSR_ERR_STATS			(0x1C) // IPSec Command/Status Register, for error status
#define REG_IPSCSR_SWAP_BURST 			(0x30) // IPSec Command/Status Register, for swap and burst number 

#define REG_CUR_SDSA	(0x4) //  
#define REG_CUR_DDSA	(0xC) // 
#define REG_SRC_DMA_ADDR	(0x38) // 
#define REG_DES_DMA_ADDR	(0x3C) // 
#define REG_A2EO_SUM	(0x20) // 
#define REG_ENC_SUM	(0x24) // 
#define REG_HPAD_SUM	(0x28) // 
#define REG_ENC_PAD_SUM	(0x2C) // 

#define REG_ENGINE_DBG	(0x40)
#define REG_DMA_LX_DBG	(0x44)
#define REG_DMA_RX_DBG	(0x48)
#define REG_DMA_TX_DBG	(0x4c)

#define REG_STA_SDES_CFG	(0x50)
#define REG_STA_SDES_PTR	(0x54)
#define REG_STA_SDES_CMD1	(0x58)
#define REG_STA_SDES_CMD2	(0x5C)
#define REG_STA_SDES_CMD3	(0x60)
#define REG_STA_DDES_CFG	(0x64)
#define REG_STA_DDES_PTR	(0x68)

// IPSec Source Descriptor Status
typedef union {
    struct {
        uint32_t fifo_empty_cnt:8;
        uint32_t swptr:8;
        uint32_t srptr:8;
        uint32_t src_fail:1;
        uint32_t src_fail_status:2;
        uint32_t src_fail_m:1;
        uint32_t rsvd1:2;
        uint32_t pk_up:1;
        uint32_t src_rst:1;
    } b;

    uint32_t w;

} rtl_crypto_srcdesc_status_t;

// IPSec Destination Descriptor Status
typedef union {
    struct {
        uint32_t fifo_empty_cnt:8;
        uint32_t dwptr:8;
        uint32_t drptr:8;
        uint32_t des_fail:1;
        uint32_t des_fail_status:2;
        uint32_t des_fail_m:1;
        uint32_t rsvd1:3;
        uint32_t src_rst:1;
    } b;

    uint32_t w;

} rtl_crypto_dstdesc_status_t;


typedef union {
    struct {
        uint32_t key_len:4;
        uint32_t iv_len:4;
        uint32_t keypad_len:8;
        uint32_t hash_iv_len:6;
        uint32_t ap:2;
        uint32_t cl:2;
        uint32_t priv_key:1;
        uint32_t otp_key:1;
        uint32_t ls:1;
        uint32_t fs:1;
        uint32_t rs:1;
        uint32_t rsvd:1;
    } b;

    struct {
        u32 apl:8;
        u32 a2eo:5;
        u32 zero:1;
        u32 enl:14;
        u32 ls:1;
        u32 fs:1;
        u32 rs:1;
        u32 rsvd:1;
    } d;

    uint32_t w;
} rtl_crypto_srcdesc_t;


typedef union {
    struct {
        u32 adl:8;
        u32 rsvd1:19;
        u32 enc:1;
        u32 ls:1;
        u32 fs:1;
        u32 wd:1;
        u32 rsvd2:1;
    } auth;

    struct {
        u32 enl:24;
        u32 rsvd1:3;
        u32 enc:1;
        u32 ls:1;
        u32 fs:1;
        u32 wd:1;
        u32 rsvd2:1;
    } cipher;

    uint32_t w;

} rtl_crypto_dstdesc_t;



// IPSec Command/Status Register : 0x10
#define IPS_10_SRST				(1<<0)
#define IPS_10_SRC_POLL			(1<<1)
#define IPS_10_DES_POLL			(1<<2)
#define IPS_10_CMD_OK			(1<<4)
#define IPS_10_SRC_IDLE			(1<<5)
#define IPS_10_DES_IDLE			(1<<6)
#define IPS_10_DES_MULTI_PKT 	(1<<24)
#define IPS_10_IPSEC_RST		(1<<31)


// IPSec Command/Status Register : 0x18
#define IPS_18_DMA_WAIT_CYCLE	(1<<0)
#define IPS_18_ARBITER_MODE		(1<<16)
#define IPS_18_DEBUG_PORT		(1<<20)
#define IPS_18_CLK_EN			(1<<24)
#define IPS_18_DEBUG_WB			(1<<31)


// IPSec Command/Status Register : 0x30
#define IPS_30_DES_SWAP			(1<<0)
#define IPS_30_KEY_IV_SWAP		(1<<1)
#define IPS_30_KEY_PAD_SWAP		(1<<2)
#define IPS_30_HASH_IV			(1<<3)
#define IPS_30_DI_ENDIAN		(1<<4)
#define IPS_30_TX_BYTE_SWAP		(1<<8)
#define IPS_30_DO_ENDIAN		(1<<9)
#define IPS_30_MAC_ENDIAN		(1<<10)
#define IPS_30_RXWD_SWAP		(1<<11)
#define IPS_30_TXWD_SWAP		(1<<12)
#define IPS_30_BURST_NUM_SHIFT	16


//
// crc
//
#define REG_CRC_RST			(0x100)	// CRC reset register
#define REG_CRC_OP			(0x104)	// CRC operation register
#define REG_CRC_POLY		(0x108)	// CRC polynomia register
#define REG_CRC_IV			(0x10C)	// CRC initial value register
#define REG_CRC_OXOR		(0x110)	// CRC output XOR register
#define REG_CRC_DATA		(0x114)	// CRC data register
#define REG_CRC_STAT		(0x118)	// CRC status register
#define REG_CRC_RES			(0x11C)	// CRC result register
#define REG_CRC_CNT			(0x120)	// CRC count register

#define CRC_104_ISWAP		(1<<0)
#define CRC_104_OSWAP		(1<<3)
#define CRC_104_DMA			(1<<7)

#define CRC_118_BUSY		(1<<0)
#define CRC_118_INTR		(1<<1)
#define CRC_118_INTRMSK		(1<<2)
#define CRC_118_LE			(1<<3)




//

static u8 padding[64] = { 0x0 };
static u8 ssl_padding[64] = { 0x0 };

//
// Functions
//

HAL_ROM_TEXT_SECTION
_LONG_CALL_
int __rtl_memcmpb(const u8 *dst, const u8 *src, int bytes)
{
    int i;
    u8  val;

    for (i=0; i<bytes; i++) {
        val = *dst - *src;
        if ( val != 0 ) return i+1;
        dst++;
        src++;
    }
    return 0;
}


HAL_ROM_TEXT_SECTION
_LONG_CALL_
uint32_t __rtl_random(uint32_t rtl_seed)
{
    uint32_t hi32, lo32;

    hi32 = (rtl_seed>>16)*19;
    lo32 = (rtl_seed&0xffff)*19+37;
    hi32 = hi32^(hi32<<16);
    return ( hi32^lo32 );
}



HAL_ROM_TEXT_SECTION
_LONG_CALL_
void __rtl_align_to_be32(uint32_t *pData, int bytes4_num )
{
    int i;
    uint32_t *ptr;
    uint32_t data;

    ptr = pData;

    for (i=0; i<bytes4_num; i++ ) {
        data = *ptr;
        *ptr = __rtl_cpu_to_be32(data);
        ptr++;
    }
}


HAL_ROM_TEXT_SECTION
_LONG_CALL_
void __rtl_memDump(const u8 *start, uint32_t size, char * strHeader)
{
    int row, column, index, index2, max;
    u8 *buf, *line;

    if(!start ||(size==0))
        return;

    line = (u8*)start;

    /*
    16 bytes per line
    */
    if (strHeader)
        __printk ("%s", strHeader);

    column = size % 16;
    row = (size / 16) + 1;
    for (index = 0; index < row; index++, line += 16) {
        buf = (u8*)line;

        max = (index == row - 1) ? column : 16;
        if ( max==0 ) break; /* If we need not dump this line, break it. */

        __printk ("\n[%08x] ", line);

        //Hex
        for (index2 = 0; index2 < max; index2++) {
            if (index2 == 8)
                __printk ("  ");
            __printk ("%02x ", (u8) buf[index2]);
        }

        if (max != 16) {
            if (max < 8)
                __printk ("  ");
            for (index2 = 16 - max; index2 > 0; index2--)
                __printk ("   ");
        }

    }

    __printk ("\n");
    return;
}





#if 1
// TODO:
// will modify to see how to do assertion and print out in the future
// +++++++++++++++++++++++++++++++++++++++++

#undef rtl_assert
#define rtl_assert(x)


#define __memDump	__rtl_memDump


#undef __dbg_printk
#define __dbg_printk(fmt, args...) \
	     DBG_CRYPTO_INFO("%s(): " fmt "\n", "crypto info: ", ##args);

#undef __err_printk
#define __err_printk(fmt, args...) \
	     DBG_CRYPTO_ERR("%s(): " fmt "\n", "crypto error: ", ##args);


#undef __dbg_mem_dump
#define __dbg_mem_dump(start, size, str_header) \
			if ( ConfigDebugInfo & _DBG_CRYPTO_ ) { \
		    	__printk("%s(): memdump : address: %08x, size: %d\n", "rtl_cryptoEngine_dbg", start, size); \
		    	__memDump((const u8*)(start), size, (char*)(str_header)); \
			}

#endif


static inline
int __rtl_crypto_srcdesc_fifo_cnt(void)
{
    rtl_crypto_srcdesc_status_t srcdesc_stats;

    srcdesc_stats.w = HAL_CRYPTO_READ32(REG_IPSSDSTS);
    return srcdesc_stats.b.fifo_empty_cnt;

}

static inline
void __rtl_crypto_set_srcdesc(uint32_t sd1, uint32_t sdpr)
{
    int cnt;

    if ( (cnt=__rtl_crypto_srcdesc_fifo_cnt()) > 0  ) {
        __dbg_printk("sd1=0x%x , sdpr=0x%x \r\n", sd1, sdpr);
        HAL_CRYPTO_WRITE32(REG_IPSSD1, sd1|0x80000000);
        HAL_CRYPTO_WRITE32(REG_IPSSD2, sdpr);
    } else {
        __err_printk("fifo_cnt is not correct: %d \r\n", cnt);
    }
}

static inline
int __rtl_crypto_dstdesc_fifo_cnt(void)
{
    rtl_crypto_srcdesc_status_t srcdesc_stats;

    srcdesc_stats.w = HAL_CRYPTO_READ32(REG_IPSDDSTS);
    return srcdesc_stats.b.fifo_empty_cnt;

}

static inline
void __rtl_crypto_set_dstdesc(uint32_t dd1, uint32_t ddpr)
{
    if ( __rtl_crypto_dstdesc_fifo_cnt() > 0  ) {
        __dbg_printk("dd1=0x%x , ddpr=0x%x \r\n", dd1, ddpr);
        HAL_CRYPTO_WRITE32(REG_IPSDD1, dd1|0x80000000);
        HAL_CRYPTO_WRITE32(REG_IPSDD2, ddpr);
    } else {
        __err_printk("fifo_cnt is not correct: %d \r\n", __rtl_crypto_dstdesc_fifo_cnt());
    }
}

static inline
void __rtl_crypto_start_packet_init(void)
{
    uint32_t ips_err;
    volatile uint32_t ips_10_status;


    // clear flags : write 1 to clear
    ips_err = 0x3FFF;
    HAL_CRYPTO_WRITE32(REG_IPSCSR_ERR_STATS, ips_err);
    HAL_CRYPTO_WRITE32(REG_IPSCSR_RESET_POLL_STATUS, IPS_10_CMD_OK);
}

#if 0



//
// Create a ring buffer for source descriptor
//
// Output
//      pRing_num
//      pRingAddr
// Input :
//      descNum
//      rambuf
//
static inline
int __rtl_cryptoEngine_setDespRing(void**pRingAddr, int descNum, void* rambuf, int eachsize)
{

    //
    // Check parameters
    //
    if ( rambuf == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
    if ( pRingAddr == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;


    // The number of ring is : 1~MAX_IPSEC_DESC
    //
    if ( (descNum <=0) || (descNum > CRYPTO_MAX_DESP) ) return _ERRNO_CRYPTO_DESC_NUM_SET_OutRange;

    //
    // Allocate src descriptor ring
    //

    // assign static RAM buffer to Ring
    *pRingAddr = rambuf;

    // set data field for the start and the end
    rtl_memset((u8*)(*pRingAddr), 0, descNum * eachsize);

    return SUCCESS;
}



static inline
int __rtl_cryptoEngine_srcDesp_init(hal_crypto_adapter_t* pIE)
{
    int ret;
    rtl_crypto_source_t *g_ipssdar;

    ret = __rtl_cryptoEngine_setDespRing((void**)&(pIE->g_ipssdar),
                                         pIE->desc_num,  (void*)(&(pIE->g_src_buf[0])), sizeof(rtl_crypto_source_t));
    if ( ret != SUCCESS ) {
        return ret;
    }

    pIE->idx_srcdesc = 0;
    (pIE->g_ipssdar)[pIE->desc_num -1].eor = 1;


    g_ipssdar = pIE->g_ipssdar;
    HAL_CRYPTO_WRITE32(REG_IPSSDAR, g_ipssdar);

    return SUCCESS;

}

static inline
int __rtl_cryptoEngine_dstDesp_init(hal_crypto_adapter_t* pIE)
{
    int ret;
    rtl_crypto_dest_t *g_ipsddar;

    ret = __rtl_cryptoEngine_setDespRing((void**)&(pIE->g_ipsddar),
                                         pIE->desc_num,  (void*)(&(pIE->g_dst_buf[0])), sizeof(rtl_crypto_dest_t));
    if ( ret != SUCCESS ) return ret;

    pIE->idx_dstdesc = 0;
    (pIE->g_ipsddar)[pIE->desc_num- 1].eor = 1;


    // set into HW Reg : IPSDDAR
    g_ipsddar = pIE->g_ipsddar;
    HAL_CRYPTO_WRITE32(REG_IPSDDAR, (u32)g_ipsddar);

    return SUCCESS;

}


//
// Neo : 2014.06.17 : Source Crypto Descriptor ---
//



static inline
int __rtl_cryptoEngine_alloc(hal_crypto_adapter_t* pIE, u32 descNum)
{
    int ret;

    pIE->desc_num= descNum;

    ret = __rtl_cryptoEngine_srcDesp_init(pIE);
    if ( ret != SUCCESS ) return ret;

    ret = __rtl_cryptoEngine_dstDesp_init(pIE);
    if ( ret != SUCCESS ) return ret;

    return SUCCESS;
}

static inline
int __rtl_cryptoEngine_free(hal_crypto_adapter_t* pIE)
{
    if (pIE == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

    if (pIE->g_ipssdar) {
        HAL_CRYPTO_WRITE32(REG_IPSSDAR, 0);
        pIE->g_ipssdar = NULL;
    }

    if (pIE->g_ipsddar) {
        HAL_CRYPTO_WRITE32(REG_IPSDDAR, 0);
        pIE->g_ipsddar = NULL;
    }

    return SUCCESS;
}


static inline
rtl_crypto_source_t* __rtl_cryptoEngine_get_new_srcDesc(hal_crypto_adapter_t *pIE)
{
    int idx_srcdesc;
    rtl_crypto_source_t *pSrcDesc;

    idx_srcdesc = pIE->idx_srcdesc;
    pSrcDesc = &(pIE->g_ipssdar[idx_srcdesc]);

    rtl_memset((u8*)pSrcDesc, 0, sizeof(rtl_crypto_source_t));

    if ( idx_srcdesc == (pIE->desc_num -1) ) { // The last one
        pSrcDesc->eor = 1;
    }
    return pSrcDesc;
}

static inline
rtl_crypto_dest_t* __rtl_cryptoEngine_get_new_dstDesc(hal_crypto_adapter_t *pIE)
{
    int idx_dstdesc;
    rtl_crypto_dest_t *pDstDesc;

    idx_dstdesc = pIE->idx_dstdesc;
    pDstDesc = &(pIE->g_ipsddar[idx_dstdesc]);

    rtl_memset((u8*)pDstDesc, 0, sizeof(rtl_crypto_dest_t));

    if ( idx_dstdesc == (pIE->desc_num -1) ) { // The last one
        pDstDesc->eor = 1;
    }
    return pDstDesc;
}

#endif


static inline
void __rtl_cryptoEngine_set_digestlen(hal_crypto_adapter_t *pIE, IN const uint32_t auth_type)
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
            digestlen = pIE->sha2type;
            break;

        default:
            digestlen = 0;
            break;
    }

    pIE->digestlen = digestlen;
}


static inline
int __rtl_cryptoEngine_set_key(hal_crypto_adapter_t *pIE,
                               IN const u8* pCipherKey, IN const int lenCipherKey,
                               IN const u8* pAuthKey, IN const int lenAuthKey )
{

    pIE->lenCipherKey = lenCipherKey;
    pIE->pCipherKey = pCipherKey;


    pIE->pAuthKey = pAuthKey;
    pIE->lenAuthKey = lenAuthKey;

    if ( lenAuthKey > 0 ) {
        int i;


        pIE->ipad = (u8 *)(&(pIE->g_IOPAD[0]));
        pIE->opad = (u8 *)(&(pIE->g_IOPAD[CRYPTO_PADSIZE]));

        rtl_memset((uint32_t*)(pIE->ipad), 0x36363636, CRYPTO_PADSIZE);
        rtl_memset((uint32_t*)(pIE->opad), 0x5c5c5c5c, CRYPTO_PADSIZE);

        for (i=0; i< lenAuthKey; i++) {
            pIE->ipad[i] ^= ((u8*) pIE->pAuthKey)[i];
            pIE->opad[i] ^= ((u8*) pIE->pAuthKey)[i];
        }
    } else {
        pIE->ipad = 0;
        pIE->opad = 0;
    }



    return SUCCESS;

}

//
// external functions
//
HAL_ROM_TEXT_SECTION
_LONG_CALL_
void __rtl_cryptoEngine_info(void)
{

    printk("\n REG_IPSSDSTS=%x, REG_IPSSD1=%x \n", HAL_CRYPTO_READ32(REG_IPSSDSTS), HAL_CRYPTO_READ32(REG_IPSSD1));
    printk(" REG_IPSSD2=%x, REG_IPSDDSTS=%x \n", HAL_CRYPTO_READ32(REG_IPSSD2), HAL_CRYPTO_READ32(REG_IPSDDSTS));
    printk(" REG_IPSDD1=%x, REG_IPSDD2=%x \n", HAL_CRYPTO_READ32(REG_IPSDD1), HAL_CRYPTO_READ32(REG_IPSDD2));
    printk(" REG_IPSCSR_RESET_POLL_STATUS=%x, REG_IPSCSR_DEBUG=%x \n", HAL_CRYPTO_READ32(REG_IPSCSR_RESET_POLL_STATUS), HAL_CRYPTO_READ32(REG_IPSCSR_DEBUG));
    printk(" REG_IPSCSR_ERR_STATS=%x, REG_IPSCSR_SWAP_BURST=%x \n\n", HAL_CRYPTO_READ32(REG_IPSCSR_ERR_STATS), HAL_CRYPTO_READ32(REG_IPSCSR_SWAP_BURST));
    printk(" REG_CUR_SDSA=%x, REG_CUR_DDSA=%x \n", HAL_CRYPTO_READ32(REG_CUR_SDSA), HAL_CRYPTO_READ32(REG_CUR_DDSA));
    printk(" REG_SRC_DMA_ADDR=%x, REG_DES_DMA_ADDR=%x \n\n", HAL_CRYPTO_READ32(REG_SRC_DMA_ADDR), HAL_CRYPTO_READ32(REG_DES_DMA_ADDR));
    printk(" REG_A2EO_SUM=%x, REG_ENC_SUM=%x \n", HAL_CRYPTO_READ32(REG_A2EO_SUM), HAL_CRYPTO_READ32(REG_ENC_SUM));
    printk(" REG_HPAD_SUM=%x, REG_ENC_PAD_SUM=%x \n\n", HAL_CRYPTO_READ32(REG_HPAD_SUM), HAL_CRYPTO_READ32(REG_ENC_PAD_SUM));
    printk(" REG_ENGINE_DBG=%x, REG_DMA_LX_DBG=%x \n", HAL_CRYPTO_READ32(REG_ENGINE_DBG), HAL_CRYPTO_READ32(REG_DMA_LX_DBG));
    printk(" REG_DMA_RX_DBG=%x, REG_DMA_TX_DBG=%x \n\n", HAL_CRYPTO_READ32(REG_DMA_RX_DBG), HAL_CRYPTO_READ32(REG_DMA_TX_DBG));
    printk(" REG_STA_SDES_CFG=%x, REG_STA_SDES_PTR=%x \n", HAL_CRYPTO_READ32(REG_STA_SDES_CFG), HAL_CRYPTO_READ32(REG_STA_SDES_PTR));
    printk(" REG_STA_SDES_CMD1=%x, REG_STA_SDES_CMD2=%x \n", HAL_CRYPTO_READ32(REG_STA_SDES_CMD1), HAL_CRYPTO_READ32(REG_STA_SDES_CMD2));
    printk(" REG_STA_SDES_CMD3=%x, REG_STA_DDES_CFG=%x \n", HAL_CRYPTO_READ32(REG_STA_SDES_CMD3), HAL_CRYPTO_READ32(REG_STA_DDES_CFG));
    printk(" REG_STA_DDES_PTR=%x\n\n", HAL_CRYPTO_READ32(REG_STA_DDES_PTR));



    return SUCCESS;
}



//
// external functions
//
HAL_ROM_TEXT_SECTION
_LONG_CALL_
int __rtl_cryptoEngine_init(hal_crypto_adapter_t *pIE)
{
    int ret;
    uint32_t burstSize;
    uint32_t value;

    int descNum;

    descNum = pIE->desc_num;
    if ( (descNum <=0) || (descNum > CRYPTO_MAX_DESP ) )
        return _ERRNO_CRYPTO_DESC_NUM_SET_OutRange;


    LXBUS_FCTRL(ON);
    SECURITY_FCTRL(ON);

    // enable IPSec related clock
    ACTCK_SEC_ENG_CCTRL(ON);
    SLPCK_SEC_ENG_CCTRL(ON);

#if (LEXRA_BIG_ENDIAN==0)
    // LEXRA big/little
    // [10]	R/W LX Bus slave read/write endian swap control: 1: big to little endian; 0: big endian
    // [9]	R/W LX Bus master read/write endian swap control: 1: big to little endian; 0: big endian
    value = HAL_READ32(PERI_ON_BASE,REG_PESOC_SOC_CTRL);
    value |= 0x600;
    HAL_WRITE32(PERI_ON_BASE,REG_PESOC_SOC_CTRL, value);
    // debug
    value = HAL_READ32(PERI_ON_BASE,REG_PESOC_SOC_CTRL);
    DiagPrintf("PERI ctrl=0x%x \r\n", value);
#endif

    // Crypto engine : Software Reset
    //HAL_CRYPTO_WRITE32(REG_IPSCSR_RESET_POLL_STATUS, HAL_CRYPTO_READ32(REG_IPSCSR_RESET_POLL_STATUS) | IPS_10_SRST);
    HAL_CRYPTO_WRITE32(REG_IPSCSR_RESET_POLL_STATUS, IPS_10_SRST);


    burstSize = pIE->dma_burst_num;
    // for debug
    burstSize = 0; // default
    DiagPrintf("burstSize=%d\r\n", burstSize);
    // KEY , IV swap for chacha only
#if (LEXRA_BIG_ENDIAN==1)
    HAL_CRYPTO_WRITE32(REG_IPSCSR_SWAP_BURST, (IPS_30_DES_SWAP | (burstSize<<IPS_30_BURST_NUM_SHIFT)) );
#else
    HAL_CRYPTO_WRITE32(REG_IPSCSR_SWAP_BURST, IPS_30_KEY_PAD_SWAP | IPS_30_KEY_IV_SWAP | IPS_30_HASH_IV | IPS_30_DI_ENDIAN | IPS_30_DO_ENDIAN | IPS_30_MAC_ENDIAN | (burstSize<<IPS_30_BURST_NUM_SHIFT) );
#endif


//Neo Jou : 0605
#if 0
    ret = __rtl_cryptoEngine_free(pIE);
    if ( ret != SUCCESS ) return ret;

    ret = __rtl_cryptoEngine_alloc(pIE, descNum);
    if ( ret != SUCCESS ) return ret;
#endif

    // Crypto Engine : DMA arbiter , clock enable, debug loopback
    HAL_CRYPTO_WRITE32(REG_IPSCSR_DEBUG, (IPS_18_ARBITER_MODE | IPS_18_CLK_EN ));

    pIE->isInit = 1;

    return SUCCESS;
}


HAL_ROM_TEXT_SECTION
_LONG_CALL_
int __rtl_cryptoEngine_set_security_mode(hal_crypto_adapter_t *pIE,
        IN const uint32_t mode_select, IN const uint32_t cipher_type, IN const uint32_t auth_type,
        IN const void* pCipherKey, IN const uint32_t lenCipherKey,
        IN const void* pAuthKey, IN const uint32_t lenAuthKey
                                        )
{
    //int i;

    if ( pIE == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
    if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT;

    if ( lenAuthKey > 0 ) { // Authentication
        if ( pAuthKey == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
        if ( lenAuthKey > CRYPTO_MAX_AUTH_KEY_LENGTH) return _ERRNO_CRYPTO_KEY_OutRange;
    }

    pIE->mode_select = mode_select;


    // Crypto

    pIE->trides  = 0;
    pIE->aes		 = 0;
    pIE->des		 = 0;
    pIE->chacha		 = 0;
    pIE->isDecrypt	 = 0;
    pIE->cipher_type = cipher_type;

    if ( cipher_type != CIPHER_TYPE_NO_CIPHER) {
        switch (cipher_type & CIPHER_TYPE_MASK_FUNC ) {
            case CIPHER_TYPE_FUNC_DES :
                pIE->des = 1;
                break;
            case CIPHER_TYPE_FUNC_3DES :
                pIE->trides = 1;
                break;
            case CIPHER_TYPE_FUNC_AES :
                pIE->aes = 1;
                break;
            case CIPHER_TYPE_FUNC_CHACHA :
                pIE->chacha = 1;
                break;
        }

        pIE->isDecrypt  = ( cipher_type & CIPHER_TYPE_MODE_ENCRYPT )? 0: 1;
    }



    // Auth

    pIE->auth_type = auth_type;
    if ( auth_type != AUTH_TYPE_NO_AUTH) {
        pIE->isHMAC = (auth_type & AUTH_TYPE_MASK_HMAC) ? 1 : 0;
        pIE->isMD5 = ( (auth_type & AUTH_TYPE_MASK_FUNC) == AUTH_TYPE_MD5 )? 1 : 0;
        pIE->isSHA1 = ( (auth_type & AUTH_TYPE_MASK_FUNC) == AUTH_TYPE_SHA1 )? 1 : 0;
        pIE->isSHA2 = ( (auth_type & AUTH_TYPE_MASK_FUNC) == AUTH_TYPE_SHA2 )? 1 : 0;
        if ( pIE->isSHA2 ) {
            switch ( auth_type & AUTH_TYPE_MASK_SHA2 ) {
                case AUTH_TYPE_SHA2_224 :
                    pIE->sha2type = SHA2_224;
                    break;
                case AUTH_TYPE_SHA2_256 :
                    pIE->sha2type = SHA2_256;
                    break;
                case AUTH_TYPE_SHA2_384 :
                    pIE->sha2type = SHA2_384;
                    break;
                case AUTH_TYPE_SHA2_512 :
                    pIE->sha2type = SHA2_512;
                    break;
                default :
                    __err_printk("No this auth_type(%d) for SHA2\n", auth_type);
                    pIE->sha2type = SHA2_NONE;
                    break;
            }
        } else {
            pIE->sha2type = SHA2_NONE;
        }
    } else {
        pIE->isMD5 = 0;
        pIE->isHMAC = 0;
        pIE->isSHA1=0;
        pIE->isSHA2=0;
        pIE->sha2type = SHA2_NONE;
    }

    __rtl_cryptoEngine_set_digestlen(pIE, auth_type);

    __rtl_cryptoEngine_set_key(pIE, pCipherKey, lenCipherKey, pAuthKey, lenAuthKey);

    if ( auth_type != AUTH_TYPE_NO_AUTH) {
        pIE->hmac_seq_hash_first = 1;
        pIE->hmac_seq_hash_last = 0;
        pIE->hmac_seq_hash_total_len = 0;
    }
    return SUCCESS;
}




HAL_ROM_TEXT_SECTION
_LONG_CALL_
int __rtl_cryptoEngine_exit(hal_crypto_adapter_t *pIE)
{
    pIE->isInit = 0;

    // disable IPSec related clock
    ACTCK_SEC_ENG_CCTRL(OFF);
    SLPCK_SEC_ENG_CCTRL(OFF);

    //
    SECURITY_FCTRL(OFF);


    //__rtl_cryptoEngine_free(pIE);
    return SUCCESS;
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_
int __rtl_cryptoEngine_reset(hal_crypto_adapter_t *pIE)
{
    pIE->isInit = 0;

    // disable  related clocks
    ACTCK_SEC_ENG_CCTRL(OFF);
    SLPCK_SEC_ENG_CCTRL(OFF);

    //
    SECURITY_FCTRL(OFF);

    //

    return __rtl_cryptoEngine_init(pIE);
}


#if 0
static inline void __rtl_cryptoEngine_set_srcDesc(hal_crypto_adapter_t *pIE)
{
    rtl_crypto_source_t *pSrcDesc;
    uint32_t desc_data_p;

    pSrcDesc = &(pIE->g_ipssdar[pIE->idx_srcdesc]);
    pSrcDesc->own = 1; //set ower to HW engine

    desc_data_p = pSrcDesc->sdbp;


    __dbg_mem_dump(pSrcDesc, sizeof(rtl_crypto_source_t), "src Desc:");
    __dbg_mem_dump(desc_data_p, 28, "src Desc Data:");

    pIE->idx_srcdesc = ( pIE->idx_srcdesc + 1 ) % pIE->desc_num;

}

static inline rtl_crypto_source_t* __rtl_cryptoEngine_set_and_next_srcDesc(hal_crypto_adapter_t *pIE)
{
    rtl_crypto_source_t *pSrcDesc;
    uint32_t desc_data_p;


    __rtl_cryptoEngine_set_srcDesc(pIE);

    return __rtl_cryptoEngine_get_new_srcDesc(pIE);
}

#endif

// TODO : create buffer temporarily for test only
static u8 cl_buffer[4*7];

static inline
void __rtl_cryptoEngine_setup_cl_buffer(hal_crypto_adapter_t *pIE)
{
    rtl_crypto_cl_t *pCL;
    uint32_t a2eo;
    uint32_t enl;
    uint32_t apl;

    a2eo = pIE->a2eo;
    enl = pIE->enl;
    apl = pIE->hash_apl;

    pCL = (rtl_crypto_cl_t *)&cl_buffer[0];

    memset((u8*)pCL, 0, sizeof(cl_buffer));
    if ((pIE->mode_select == CRYPTO_MS_TYPE_MIX1_sshenc_espdec) || (pIE->mode_select == CRYPTO_MS_TYPE_MIX2_sshdec_espenc) || (pIE->mode_select == CRYPTO_MS_TYPE_MIX3_sslenc)) {
        pCL->engine_mode = pIE->mode_select;

        pCL->hmac_seq_hash=0;
        pCL->hmac_seq_hash_first=0;
        pCL->hmac_seq_hash_last=0;

        //
        if ( pIE->isMD5 ) {
            pCL->habs=1;
            pCL->hibs=1;
            pCL->hobs=1;
            pCL->hkbs=1;
        } else if ( pIE->isSHA1 ) {
            pCL->hmac_mode = 1;
            pCL->habs=1;
        } else if ( pIE->sha2type != SHA2_NONE ) {
            pCL->hmac_mode = (pIE->sha2type == SHA2_224 )? 2 : 3 ; // currently only support SHA2_224 / SHA2_256
            pCL->habs=1;
        }

        if ( pIE->isHMAC ) {
            pCL->hmac_en = 1;
        }
        pCL->header_total_len=(a2eo+7)/8;
        pCL->aad_last_data_size = pIE->aad_last_data_size%8;


        pCL->hash_pad_len=(apl+7)/8;
        pCL->pad_last_data_size=pIE->pad_last_data_size;
        uint32_t cipher_type;
        uint32_t block_mode;

        // ECB / CBC / CTR;
        cipher_type = pIE->cipher_type;
        block_mode = cipher_type & CIPHER_TYPE_MASK_BLOCK;
        pCL->cipher_mode = block_mode;

        if ( pIE->aes ) {
            pCL->cipher_eng_sel = 0;
            switch ( pIE->lenCipherKey ) {
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

            pCL->enl = (enl+15)/16;
            pCL->enc_last_data_size = pIE->enc_last_data_size;

            //if ( block_mode ==  CIPHER_TYPE_BLOCK_GCM  ) {
            //pCL->header_total_len = (a2eo+7)/8;
            //pCL->aad_last_data_size = pIE->aad_last_data_size;
            //}

        } else if ( pIE->des ) {
            pCL->cipher_eng_sel = 1;
            pCL->des3_en = 0;
            pCL->enl = (enl+7)/8;
            pCL->enc_last_data_size = pIE->enc_last_data_size;
        } else if ( pIE->trides ) {
            pCL->cipher_eng_sel = 1;
            pCL->des3_en = 1;
            pCL->enl = (enl+7)/8;
            pCL->enc_last_data_size = pIE->enc_last_data_size;
        }// else if ( pIE->chacha ) {
        //   pCL->cipher_eng_sel = 2;
        //   pCL->enl = (enl+15)/16;
        //   pCL->enc_last_data_size = pIE->enc_last_data_size;

        //   pCL->header_total_len = (a2eo+15)/16;
        //   pCL->aad_last_data_size = pIE->aad_last_data_size;
        //}

        if (pIE->mode_select == CRYPTO_MS_TYPE_MIX3_sslenc) {
            pCL->ssl_apl = (pIE->data_padding_len+7)/8;
            pCL->enc_pad_last_data_size = pIE->enc_pad_last_data_size;
        }
        if ( pIE->isDecrypt == 0 )
            pCL->cipher_encrypt = 1;


        if ( pIE->chacha ) {
            pCL->ckws = 1;
            pCL->cabs = 1;
            pCL->ciws = 1;
            pCL->cibs = 1;
            pCL->cows = 1;
            pCL->cobs = 1;
            pCL->codws = 1;
            pCL->cidws = 1;
        } else {
            pCL->cabs = 1;
        }


    } else {
        if ( pIE->auth_type != AUTH_TYPE_NO_AUTH ) {
            pCL->engine_mode = 1; //hash only

            //
            pCL->hmac_seq_hash=1;

            pCL->hmac_seq_hash_first=0;
            if ( pIE->hmac_seq_hash_first == 1 ) {
                pCL->hmac_seq_hash_first=1;
                pIE->hmac_seq_hash_first=0;
            }

            //
            if ( pIE->isMD5 ) {
                pCL->habs=1;
                pCL->hibs=1;
                pCL->hobs=1;
                pCL->hkbs=1;
            } else if ( pIE->isSHA1 ) {
                pCL->hmac_mode = 1;
                pCL->habs=1;
            } else if ( pIE->sha2type != SHA2_NONE ) {
                pCL->hmac_mode = (pIE->sha2type == SHA2_224 )? 2 : 3 ; // currently only support SHA2_224 / SHA2_256
                pCL->habs=1;
            }

            if ( pIE->isHMAC ) {
                pCL->hmac_en = 1;
            }
            printk("(pIE->hmac_seq_hash_last=%d)\n", pIE->hmac_seq_hash_last);
            if ( pIE->hmac_seq_hash_last == 1 ) {
                //
                // always using auto padding
                printk("(%s-%d)\n", __func__, __LINE__);
                pCL->enl = (enl + 15)/16 ;
                pCL->enc_last_data_size = pIE->enc_last_data_size;
                pCL->ap0 = pIE->hmac_seq_hash_total_len*8;
                if ( pIE->isHMAC ) pCL->ap0 += 64*8;
            } else {
                pCL->enl = enl/64;

            }


            pCL->hmac_seq_hash_last=0;
            pCL->hmac_seq_hash_no_wb=1;
            if ( pIE->hmac_seq_hash_last == 1 ) {
                pCL->hmac_seq_hash_last=1;
                pCL->hmac_seq_hash_no_wb=0;
            }

        } else { // cipher - encryption / decryption

            uint32_t cipher_type;
            uint32_t block_mode;

            // ECB / CBC / CTR;
            cipher_type = pIE->cipher_type;
            block_mode = cipher_type & CIPHER_TYPE_MASK_BLOCK;
            pCL->cipher_mode = block_mode;


            if ( pIE->aes ) {
                pCL->cipher_eng_sel = 0;
                switch ( pIE->lenCipherKey ) {
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

                pCL->enl = (enl+15)/16;
                pCL->enc_last_data_size = pIE->enc_last_data_size;

                if ( block_mode ==  CIPHER_TYPE_BLOCK_GCM  ) {
                    pCL->header_total_len = (a2eo+15)/16;
                    pCL->aad_last_data_size = pIE->aad_last_data_size;
                }

            } else if ( pIE->des ) {
                pCL->cipher_eng_sel = 1;
                pCL->des3_en = 0;
                pCL->enl = enl/8;
                //pCL->enc_last_data_size = pIE->enc_last_data_size;
            } else if ( pIE->trides ) {
                pCL->cipher_eng_sel = 1;
                pCL->des3_en = 1;
                pCL->enl = enl/8;
                //pCL->enc_last_data_size = pIE->enc_last_data_size;
            } else if ( pIE->chacha ) {
                pCL->cipher_eng_sel = 2;
                pCL->enl = (enl+15)/16;
                pCL->enc_last_data_size = pIE->enc_last_data_size;

                pCL->header_total_len = (a2eo+15)/16;
                pCL->aad_last_data_size = pIE->aad_last_data_size;
            }

            if ( pIE->isDecrypt == 0 )
                pCL->cipher_encrypt = 1;


            if ( pIE->chacha ) {
                pCL->ckws = 1;
                pCL->cabs = 1;
                pCL->ciws = 1;
                pCL->cibs = 1;
                pCL->cows = 1;
                pCL->cobs = 1;
                pCL->codws = 1;
                pCL->cidws = 1;
            } else {
                pCL->cabs = 1;
            }
        }
    }
    if ( pIE->chacha ) {
        pCL->ckbs = 1;
    }

    if (pIE->mode_select == CRYPTO_MS_TYPE_MIX3_sslenc) {
        pCL->icv_total_length = pIE->digestlen; // for mix mode, but need to set a value 0x40
    } else {
        pCL->icv_total_length = 0x40; // for mix mode, but need to set a value 0x40
    }

}



static inline
void __rtl_cryptoEngine_srcDesc_generate_cl_key_IV(hal_crypto_adapter_t *pIE,
        const u8* p_iv, uint32_t ivLen)
{
    rtl_crypto_srcdesc_t src_desc;
    uint32_t val;


    src_desc.w = 0;

    // FS=1, CL=3
    src_desc.b.fs = 1;
    src_desc.b.cl= 3;

    if ((pIE->mode_select == CRYPTO_MS_TYPE_MIX1_sshenc_espdec) || (pIE->mode_select == CRYPTO_MS_TYPE_MIX2_sshdec_espenc) || (pIE->mode_select == CRYPTO_MS_TYPE_MIX3_sslenc)) {
        // disable auto padding in mix mode
        src_desc.b.ap = 0x00;

    } else {
        // auto padding
        if ( pIE->hmac_seq_hash_last )
            src_desc.b.ap = 0x01;
    }
    __rtl_cryptoEngine_setup_cl_buffer(pIE);

    __dbg_mem_dump((uint32_t)(&cl_buffer[0]), sizeof(cl_buffer), "Command Setting: ");

    __rtl_crypto_set_srcdesc(src_desc.w, (uint32_t)&cl_buffer[0]);


    // Set key
    if ( pIE->cipher_type != CIPHER_TYPE_NO_CIPHER) {
        uint32_t lenCipherKey;
        u8  *pCipherKey;

        lenCipherKey = pIE->lenCipherKey;
        pCipherKey = pIE->pCipherKey;

        src_desc.w = 0;
        src_desc.b.fs = 1;
        src_desc.b.key_len = lenCipherKey/4;

        __dbg_mem_dump((uint32_t)(pCipherKey), lenCipherKey, "KEY: ");

        __rtl_crypto_set_srcdesc(src_desc.w, (uint32_t)(pCipherKey));

    }


    // Set IV
    if ( p_iv != NULL && ivLen > 0 ) {
        src_desc.w = 0;
        src_desc.b.fs = 1;
        src_desc.b.iv_len = ivLen/4;

        __dbg_mem_dump((uint32_t)(p_iv), ivLen, "IV: ");

        __rtl_crypto_set_srcdesc(src_desc.w, (uint32_t)(p_iv));
    }


    // Set Pad
    if ( pIE->isHMAC ) {
        src_desc.w = 0;
        src_desc.b.fs = 1;
        src_desc.b.keypad_len = 128;

        __dbg_mem_dump((uint32_t)(&(pIE->g_IOPAD[0])), 128, "PAD: ");

        __rtl_crypto_set_srcdesc(src_desc.w, (uint32_t)(&(pIE->g_IOPAD[0])));

    }

}


#if 1
static inline
void __rtl_cryptoEngine_srcDesc_gen_HMACPadding(hal_crypto_adapter_t *pIE)
{
    rtl_crypto_srcdesc_t srcdesc;
    u64 len64;
    u8 *u8Ptr = (u8 *)&len64;
    u32 totalLen;
    uint32_t apl;
    int    i;
    u8 *pAuthPadding;

    if ( pIE->auth_type == AUTH_TYPE_NO_AUTH ) return;

    //
    apl = pIE->hash_apl;
    if ( apl == 0 ) return;

    totalLen = pIE->enl + pIE->a2eo;

    /* build padding pattern */
    pAuthPadding = (u8 *) (&(pIE->g_authPadding[0]));
    pAuthPadding[0] = 0x80; /* the first byte */
    for (i = 1; i < apl - 8; i++)
        pAuthPadding[i] = 0; /* zero bytes */

    /* final, length bytes */
    if (pIE->isHMAC)
        len64 = (totalLen + 64) << 3; /* First padding length is */
    else
        len64 = totalLen << 3; /* First padding length is */

    for (i=0; i<8; i++) {
        //the 8196f system is little endian
        //the md5 is little endian, the sha1 is big endian
        if (!pIE->isMD5)
            pAuthPadding[apl - i - 1] = u8Ptr[i];//(swap save totallen64)
        else
            pAuthPadding[apl - 8 + i] = u8Ptr[i];//(no swap save totallen64)
    }

    srcdesc.w = 0;

    /* AUTH_PAD descriptor */
    //src_desc.b.own = 1; /* Own by crypto */
    //src_desc.b.fs = 0;
    srcdesc.d.apl = apl; /* PAD size */
    //pSrcDataDesc->sdbp = 0; // the memory address which can be access

    __dbg_mem_dump(pAuthPadding, apl, "hash_apl: ");
    if ( (apl %8) == 0 ) {
        if (pIE->mode_select != CRYPTO_MS_TYPE_MIX3_sslenc)
            srcdesc.d.ls = 1;

        __rtl_crypto_set_srcdesc(srcdesc.w, pAuthPadding);

    } else {
        __rtl_crypto_set_srcdesc(srcdesc.w, pAuthPadding);
        srcdesc.w = 0;
        srcdesc.d.apl = 8-(apl%8);
        if (pIE->mode_select != CRYPTO_MS_TYPE_MIX3_sslenc)
            srcdesc.d.ls = 1;
        __dbg_mem_dump(padding, 8-(apl%8), "hash_apl padding ");
        __rtl_crypto_set_srcdesc(srcdesc.w, (uint32_t)padding);
    }
}
#endif


static inline
int __rtl_cryptoEngine_wait(hal_crypto_adapter_t *pIE)
{
    volatile uint32_t ips_err;
    volatile uint32_t ips_10_status;
    uint32_t loopWait;


    // wait until ipsec engine stop
    loopWait = 1000000; /* hope long enough */
    while (1) {
        ips_10_status = HAL_CRYPTO_READ32(REG_IPSCSR_RESET_POLL_STATUS);
        if ( ips_10_status & IPS_10_CMD_OK ) break;

        ips_err = HAL_CRYPTO_READ32(REG_IPSCSR_ERR_STATS);
        if ( ips_err ) {
            __err_printk("ips 0x1C err = 0x%08x\r\n", ips_err);
            return FAIL;
        }


        loopWait--;
        if (loopWait == 0) {
            ips_10_status = HAL_CRYPTO_READ32(REG_IPSCSR_RESET_POLL_STATUS);
            ips_err = HAL_CRYPTO_READ32(REG_IPSCSR_ERR_STATS);
            __err_printk("Wait Timeout ips status =0x%08x, ips err = 0x%08x\r\n", ips_10_status, ips_err);
            return FAIL; /* error occurs */
        }
    }
    return SUCCESS;
}

#if 0
static inline
void __rtl_cryptoEngine_dstDesc_move_next(hal_crypto_adapter_t *pIE)
{
    pIE->idx_dstdesc = (pIE->idx_dstdesc + 1) % pIE->desc_num;
}
#endif


static inline
void __rtl_cryptoEngine_auth_calc_apl(hal_crypto_adapter_t *pIE, IN const uint32_t a2eo, IN const uint32_t msglen)
{
    uint32_t auth_type;
    u32 apl;

    auth_type = pIE->auth_type;
    pIE->a2eo = a2eo;
    pIE->enl = msglen;

    pIE->enc_last_data_size = msglen % 16;
    pIE->apl = (16-pIE->enc_last_data_size) % 16;

    if ( pIE->des || pIE->trides ) {
        pIE->enc_last_data_size = msglen % 8;
        pIE->apl = (8-pIE->enc_last_data_size) % 8;
    }

    pIE->aad_last_data_size = a2eo % 16;
    pIE->apl_aad = (16-pIE->aad_last_data_size) % 16;

    if ((pIE->mode_select == CRYPTO_MS_TYPE_MIX1_sshenc_espdec) || (pIE->mode_select == CRYPTO_MS_TYPE_MIX2_sshdec_espenc) || (pIE->mode_select == CRYPTO_MS_TYPE_MIX3_sslenc)) {
        pIE->aad_last_data_size = a2eo % 8;
        pIE->apl_aad = (8-pIE->aad_last_data_size) % 8;
    }

    if (pIE->mode_select == CRYPTO_MS_TYPE_MIX3_sslenc) {
        if (pIE->aes)
            pIE->crypto_block_size=16;
        else
            pIE->crypto_block_size=8;
        pIE->data_padding_len = pIE->crypto_block_size-((pIE->enl+pIE->digestlen)%pIE->crypto_block_size);
        pIE->enc_pad_last_data_size = pIE->data_padding_len%8;
    }

    if (auth_type != AUTH_TYPE_NO_AUTH) {
        u32 authPadSpace;
        u32 g_CRYPTO_AUTH_PADDING;

        if ((pIE->sha2type==SHA2_384) || (pIE->sha2type==SHA2_512)) {
            g_CRYPTO_AUTH_PADDING = 128;
        } else {
            g_CRYPTO_AUTH_PADDING = 64;
        }

        /* calculate APL */
        authPadSpace = (((msglen+a2eo) + g_CRYPTO_AUTH_PADDING-1) &
                        (~(g_CRYPTO_AUTH_PADDING-1))) - (msglen+a2eo);


        if (authPadSpace > 8)
            // Yes, we have enough space to store 0x80 and dmaLen
            apl = authPadSpace;
        else
            // We don't have enough space to store 0x80 and dmaLen. Allocate another 64 bytes.
            apl = authPadSpace + g_CRYPTO_AUTH_PADDING;
    } else {
        apl = 0;
    }

    pIE->hash_apl = apl;
    pIE->pad_last_data_size = apl%8;

}


HAL_ROM_TEXT_SECTION
_LONG_CALL_
int __rtl_cryptoEngine(hal_crypto_adapter_t *pIE,
                       IN const u8 *message, 	IN const uint32_t msglen,
                       IN const u8 *pIv, 		IN const uint32_t ivlen,
                       IN const u8 *paad, 		IN const uint32_t aadlen,
                       OUT void *pResult, OUT void *pTag)
{
    rtl_crypto_srcdesc_t srcdesc;

    uint32_t a2eo;
    uint32_t enl;
    uint32_t digestlen;
    int ret;


    // Use only one scatter
    a2eo = aadlen;
    enl = msglen;

    __rtl_crypto_start_packet_init();

//
//   Set relative length data
//

    __rtl_cryptoEngine_auth_calc_apl(pIE, a2eo, enl);



    digestlen = pIE->digestlen;

    if ( pIE->auth_type != AUTH_TYPE_NO_AUTH) {

        pIE->hmac_seq_hash_total_len += msglen;

        if ( pResult != NULL ) {
            pIE->hmac_seq_hash_last = 1;
        }
    }

    /********************************************
      * step 1: prepare Key & IV array:
      ********************************************/

    __rtl_cryptoEngine_srcDesc_generate_cl_key_IV(pIE, pIv, ivlen);


    /********************************************
     * step 3: prepare Data1 ~ DataN
     ********************************************/

    srcdesc.w = 0;

    if ( paad != NULL ) {

        while ( a2eo >= 32 ) {
            srcdesc.d.a2eo = 31;
            __dbg_mem_dump(paad, 31, "AAD: ");
            __rtl_crypto_set_srcdesc(srcdesc.w, (uint32_t)paad);


            paad += 31;
            a2eo -= 31;
            srcdesc.w = 0;
        }

        if ( a2eo > 0 ) {
            srcdesc.d.a2eo = a2eo;
            __dbg_mem_dump(paad, a2eo, "AAD: ");
            __rtl_crypto_set_srcdesc(srcdesc.w, (uint32_t)paad);
            srcdesc.w = 0;
        }

        if ( pIE->apl_aad > 0 ) {
            srcdesc.d.a2eo = pIE->apl_aad;
            __dbg_mem_dump(padding, a2eo, "AAD padding ");
            __rtl_crypto_set_srcdesc(srcdesc.w, (uint32_t)padding);
            srcdesc.w = 0;
        }

    }

    //

    while ( enl >= 16384 ) {
        srcdesc.d.enl = 16383;
        __rtl_crypto_set_srcdesc(srcdesc.w, (uint32_t)message);

        message += 16383;
        enl -= 16383;
        srcdesc.w = 0;
    }
    srcdesc.d.enl = enl;
    __dbg_mem_dump(message, msglen, "data: ");

    if ( pIE->apl != 0 ) {
        __rtl_crypto_set_srcdesc(srcdesc.w, (uint32_t)message);
        srcdesc.w = 0;

        //
        srcdesc.d.enl = pIE->apl;
        srcdesc.d.ls = 1;
        __dbg_mem_dump(padding, srcdesc.d.enl, "data padding ");
        __rtl_crypto_set_srcdesc(srcdesc.w, (uint32_t)padding);
    } else {
        srcdesc.d.ls = 1;
        __rtl_crypto_set_srcdesc(srcdesc.w, (uint32_t)message);
    }


#if 0

    /********************************************
     * step 4: prepare padding for MD5 and SHA-1
     ********************************************/
    // auto padding , and padding above
    //__rtl_cryptoEngine_srcDesc_gen_HMACPadding(pIE);

#endif



    /********************************************
     * step 5: write software parameters to ASIC
     ********************************************/

    if ( pIE->auth_type != AUTH_TYPE_NO_AUTH ) {
        rtl_crypto_dstdesc_t dst_desc;

        if ( pResult != NULL ) {
            dst_desc.w = 0;
            dst_desc.auth.fs = 1;
            dst_desc.auth.ls = 1;
            dst_desc.auth.adl = digestlen;
            __rtl_crypto_set_dstdesc(dst_desc.w, (uint32_t)pResult);

        }

    } else { // cipher
        rtl_crypto_dstdesc_t  dst_auth_desc;
        rtl_crypto_dstdesc_t  dst_cipher_desc;

        if ( pResult != NULL ) {
            dst_cipher_desc.w = 0;
            dst_cipher_desc.cipher.fs = 1;
            dst_cipher_desc.cipher.enc = 1;
            dst_cipher_desc.cipher.enl = pIE->enl;
        }


        if ( pTag != NULL ) {
            if ( pResult != NULL ) {
                __rtl_crypto_set_dstdesc(dst_cipher_desc.w, (uint32_t)pResult);
            }
            dst_auth_desc.w = 0;
            dst_auth_desc.auth.enc = 0;
            dst_auth_desc.auth.adl = 16;
            dst_auth_desc.auth.ls = 1;
            __rtl_crypto_set_dstdesc(dst_auth_desc.w, (uint32_t)pTag);
        } else {
            if ( pResult != NULL ) {
                dst_cipher_desc.cipher.ls = 1;
                __rtl_crypto_set_dstdesc(dst_cipher_desc.w, (uint32_t)pResult);
            } else {
                __err_printk("pResult and pTag are all NULL \r\n");
            }
        }


    }



    /********************************************
     * write prepared descriptors into ASIC
     ********************************************/

    ret = __rtl_cryptoEngine_wait(pIE);

    return ret;
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_
int __rtl_cryptoEngineMix(hal_crypto_adapter_t *pIE,
                          IN const u8 *message, 	IN const uint32_t msglen,
                          IN const u8 *pIv, 		IN const uint32_t ivlen,
                          IN const u32 a2eo1,
                          OUT void *pResult, OUT void *pTag)
{
    rtl_crypto_srcdesc_t srcdesc;

    uint32_t a2eo=a2eo1;
    uint32_t enl;
    uint32_t digestlen;
    int ret;

    uint32_t crypto_block_size;
    uint32_t data_padding_len;

    // Use only one scatter
    a2eo = a2eo1;
    enl = msglen-a2eo1;
    printk("(%s-%d)\n", __func__, __LINE__);
    __rtl_crypto_start_packet_init();

//
//   Set relative length data
//

    __rtl_cryptoEngine_auth_calc_apl(pIE, a2eo, enl);
    //printk("(%s-%d)\n", __func__, __LINE__);


    digestlen = pIE->digestlen;

    if ( pIE->auth_type != AUTH_TYPE_NO_AUTH) {

        pIE->hmac_seq_hash_total_len += msglen;

        if ( pResult != NULL ) {
            pIE->hmac_seq_hash_last = 1;
        }
    }
    //printk("(%s-%d)\n", __func__, __LINE__);
    /********************************************
      * step 1: prepare Key & IV array:
      ********************************************/

    __rtl_cryptoEngine_srcDesc_generate_cl_key_IV(pIE, pIv, ivlen);

    //printk("(%s-%d)\n", __func__, __LINE__);
    /********************************************
     * step 3: prepare Data1 ~ DataN
     ********************************************/

    srcdesc.w = 0;

    if ( a2eo != 0 ) {

        while ( a2eo >= 32 ) {
            srcdesc.d.a2eo = 31;
            __dbg_mem_dump(message, 31, "AAD: ");
            __rtl_crypto_set_srcdesc(srcdesc.w, (uint32_t)message);


            message += 31;
            a2eo -= 31;
            srcdesc.w = 0;
        }

        if ( a2eo > 0 ) {
            srcdesc.d.a2eo = a2eo;
            __dbg_mem_dump(message, a2eo, "AAD: ");
            __rtl_crypto_set_srcdesc(srcdesc.w, (uint32_t)message);
            message += a2eo;
            srcdesc.w = 0;
        }

        if ( pIE->apl_aad > 0 ) {
            srcdesc.d.a2eo = pIE->apl_aad;
            __dbg_mem_dump(padding, pIE->apl_aad, "AAD padding ");
            __rtl_crypto_set_srcdesc(srcdesc.w, (uint32_t)padding);
            srcdesc.w = 0;
        }

    }

    //

    while ( enl >= 16384 ) {
        srcdesc.d.enl = 16383;
        __rtl_crypto_set_srcdesc(srcdesc.w, (uint32_t)message);

        message += 16383;
        enl -= 16383;
        srcdesc.w = 0;
    }
    srcdesc.d.enl = enl;
    __dbg_mem_dump(message, enl, "data: ");

    if ( pIE->apl != 0 ) {
        __rtl_crypto_set_srcdesc(srcdesc.w, (uint32_t)message);
        srcdesc.w = 0;

        //
        srcdesc.d.enl = pIE->apl;
        if ((pIE->mode_select == CRYPTO_MS_TYPE_MIX1_sshenc_espdec) || (pIE->mode_select == CRYPTO_MS_TYPE_MIX2_sshdec_espenc) || (pIE->mode_select == CRYPTO_MS_TYPE_MIX3_sslenc)) {
            srcdesc.d.ls = 0;
        } else {
            srcdesc.d.ls = 1;
        }
        __dbg_mem_dump(padding, srcdesc.d.enl, "data padding ");
        __rtl_crypto_set_srcdesc(srcdesc.w, (uint32_t)padding);
    } else {
        if ((pIE->mode_select == CRYPTO_MS_TYPE_MIX1_sshenc_espdec) || (pIE->mode_select == CRYPTO_MS_TYPE_MIX2_sshdec_espenc) || (pIE->mode_select == CRYPTO_MS_TYPE_MIX3_sslenc)) {
            srcdesc.d.ls = 0;
        } else {
            srcdesc.d.ls = 1;
        }
        __rtl_crypto_set_srcdesc(srcdesc.w, (uint32_t)message);
    }


#if 1

    /********************************************
     * step 4: prepare padding for MD5 and SHA-1
     ********************************************/
    // auto padding , and padding above
    if ((pIE->mode_select == CRYPTO_MS_TYPE_MIX1_sshenc_espdec) || (pIE->mode_select == CRYPTO_MS_TYPE_MIX2_sshdec_espenc) || (pIE->mode_select == CRYPTO_MS_TYPE_MIX3_sslenc)) {
        __rtl_cryptoEngine_srcDesc_gen_HMACPadding(pIE);
    }
#endif

    /********************************************
     * step 4.1: prepare padding for SSL ENC padding
     ********************************************/
    // prepare padding for SSL ENC padding
    if ((pIE->mode_select == CRYPTO_MS_TYPE_MIX3_sslenc)) {
        int i;
        for (i=0 ; i<pIE->data_padding_len ; i++) {
            ssl_padding[i]=pIE->data_padding_len-1;
        }
        srcdesc.w = 0;
        srcdesc.d.a2eo=pIE->data_padding_len; /* data_padding_len epl and a2eo same address */
        srcdesc.d.zero=1;
        __dbg_mem_dump(ssl_padding, pIE->data_padding_len, "ssl data padding ");
        if ((pIE->data_padding_len%8)==0) {
            srcdesc.d.ls = 1;
            __rtl_crypto_set_srcdesc(srcdesc.w, (uint32_t)ssl_padding);
        } else {
            __rtl_crypto_set_srcdesc(srcdesc.w, (uint32_t)ssl_padding);
            srcdesc.w = 0;
            srcdesc.d.a2eo=8-(pIE->data_padding_len%8);
            srcdesc.d.zero=1;
            srcdesc.d.ls = 1;
            __dbg_mem_dump(padding, srcdesc.d.a2eo, "ssl data padding padding");
            __rtl_crypto_set_srcdesc(srcdesc.w, (uint32_t)padding);
        }
    }


    /********************************************
     * step 5: write software parameters to ASIC
     ********************************************/
#if 0
    if ( pIE->auth_type != AUTH_TYPE_NO_AUTH ) {
        rtl_crypto_dstdesc_t dst_desc;

        if ( pResult != NULL ) {
            dst_desc.w = 0;
            dst_desc.auth.fs = 1;
            dst_desc.auth.ls = 1;
            dst_desc.auth.adl = digestlen;
            __rtl_crypto_set_dstdesc(dst_desc.w, (uint32_t)pResult);

        }

    } else { // cipher
        rtl_crypto_dstdesc_t  dst_auth_desc;
        rtl_crypto_dstdesc_t  dst_cipher_desc;

        if ( pResult != NULL ) {
            dst_cipher_desc.w = 0;
            dst_cipher_desc.cipher.fs = 1;
            dst_cipher_desc.cipher.enc = 1;
            dst_cipher_desc.cipher.enl = pIE->enl;
        }


        if ( pTag != NULL ) {
            if ( pResult != NULL ) {
                __rtl_crypto_set_dstdesc(dst_cipher_desc.w, (uint32_t)pResult);
            }
            dst_auth_desc.w = 0;
            dst_auth_desc.auth.enc = 0;
            dst_auth_desc.auth.adl = 16;
            dst_auth_desc.auth.ls = 1;
            __rtl_crypto_set_dstdesc(dst_auth_desc.w, (uint32_t)pTag);
        } else {
            if ( pResult != NULL ) {
                dst_cipher_desc.cipher.ls = 1;
                __rtl_crypto_set_dstdesc(dst_cipher_desc.w, (uint32_t)pResult);
            } else {
                __err_printk("pResult and pTag are all NULL \r\n");
            }
        }


    }
#else
    if ( pIE->mode_select == CRYPTO_MS_TYPE_AUTH ) {
        rtl_crypto_dstdesc_t dst_desc;

        if ( pTag != NULL ) {
            dst_desc.w = 0;
            dst_desc.auth.fs = 1;
            dst_desc.auth.ls = 1;
            dst_desc.auth.adl = digestlen;
            __rtl_crypto_set_dstdesc(dst_desc.w, (uint32_t)pTag);

        }

    } else { // cipher (CRYPTO_MS_TYPE_CIPHER, CRYPTO_MS_TYPE_MIX1_sshenc_espdec, CRYPTO_MS_TYPE_MIX2_sshdec_espenc, CRYPTO_MS_TYPE_MIX3_sslenc)
        rtl_crypto_dstdesc_t  dst_auth_desc;
        rtl_crypto_dstdesc_t  dst_cipher_desc;

        if ( pResult != NULL ) {
            dst_cipher_desc.w = 0;
            dst_cipher_desc.cipher.fs = 1;
            dst_cipher_desc.cipher.enc = 1;
            dst_cipher_desc.cipher.enl = pIE->enl;
            if (pIE->mode_select == CRYPTO_MS_TYPE_MIX3_sslenc) {
                printk("(%s-%d)\n", __func__, __LINE__);
                printk("%d-%d-%d", dst_cipher_desc.cipher.enl, pIE->data_padding_len, pIE->digestlen);
                dst_cipher_desc.cipher.enl += pIE->data_padding_len + pIE->digestlen;
                printk("-%d-\n", dst_cipher_desc.cipher.enl);
            }
        }


        if ( pTag != NULL ) {
            if ( pResult != NULL ) {
                __rtl_crypto_set_dstdesc(dst_cipher_desc.w, (uint32_t)pResult);
            }
            dst_auth_desc.w = 0;
            dst_auth_desc.auth.enc = 0;
            if ( pIE->auth_type != AUTH_TYPE_NO_AUTH ) {
                dst_auth_desc.auth.adl = digestlen;
            } else {
                dst_auth_desc.auth.adl = 16;
            }
            dst_auth_desc.auth.ls = 1;
            __rtl_crypto_set_dstdesc(dst_auth_desc.w, (uint32_t)pTag);
        } else {
            if ( pResult != NULL ) {
                dst_cipher_desc.cipher.ls = 1;
                __rtl_crypto_set_dstdesc(dst_cipher_desc.w, (uint32_t)pResult);
            } else {
                __err_printk("pResult and pTag are all NULL \r\n");
            }
        }


    }


#endif
    printk("(%s-%d)\n", __func__, __LINE__);

    /********************************************
     * write prepared descriptors into ASIC
     ********************************************/

    ret = __rtl_cryptoEngine_wait(pIE);
    printk("(%s-%d)\n", __func__, __LINE__);
    return ret;
}


//
// Cipher Functions
//


HAL_ROM_TEXT_SECTION
_LONG_CALL_
int __rtl_crypto_cipher_init(hal_crypto_adapter_t *pIE,
                             IN const uint32_t cipher_type,
                             IN const u8* key, IN const uint32_t keylen)
{
    u8* pAuthKey = NULL;
    uint32_t lenAuthKey = 0;

    return __rtl_cryptoEngine_set_security_mode(pIE,
            CRYPTO_MS_TYPE_CIPHER, cipher_type, AUTH_TYPE_NO_AUTH,
            key, keylen, pAuthKey, lenAuthKey);

}

static inline
void __rtl_cryptoEngine_set_encrypt(hal_crypto_adapter_t *pIE)
{
    pIE->cipher_type |= CIPHER_TYPE_MODE_ENCRYPT;
    pIE->isDecrypt = _FALSE;
}

static inline
void __rtl_cryptoEngine_set_decrypt(hal_crypto_adapter_t *pIE)
{
    pIE->cipher_type ^= CIPHER_TYPE_MODE_ENCRYPT;
    pIE->isDecrypt = _TRUE;
}


HAL_ROM_TEXT_SECTION
_LONG_CALL_
int __rtl_crypto_cipher_encrypt(hal_crypto_adapter_t *pIE,
                                IN const uint32_t cipher_type,
                                IN const u8* message, 	IN const uint32_t msglen,
                                IN const u8* piv, 		IN const uint32_t ivlen,
                                IN const u8* paad, 		IN const uint32_t aadlen,
                                OUT u8* pResult, OUT u8* pTag)
{
    int ret;
    uint32_t a2eo = 0;

    __rtl_cryptoEngine_set_encrypt(pIE);

    ret = __rtl_cryptoEngine(pIE, message, msglen,
                             piv, ivlen, paad, aadlen, pResult, pTag);


    return ret;
}



HAL_ROM_TEXT_SECTION
_LONG_CALL_
int __rtl_crypto_cipher_decrypt(
    hal_crypto_adapter_t *pIE,
    IN const uint32_t cipher_type,
    IN const u8* message, 	IN const uint32_t msglen,
    IN const u8* piv, 			IN const uint32_t ivlen,
    IN const u8* paad,			IN const uint32_t aadlen,
    OUT u8* pResult, OUT u8* pTag )
{
    int ret;
    uint32_t a2eo = 0;

    __rtl_cryptoEngine_set_decrypt(pIE);
    ret = __rtl_cryptoEngine(pIE, message, msglen, piv, ivlen, paad, aadlen, pResult, pTag);


    return ret;
}


//
// crc32
//

static inline int __rtl_crc_isBusy(void)
{
    volatile uint32_t regValue;

    regValue = HAL_CRYPTO_READ32(REG_CRC_STAT);
    regValue &= CRC_118_BUSY;

    return regValue;

}

static inline void __rtl_crc_set_DMA(u8 enabled)
{
    volatile uint32_t regValue;

    regValue = HAL_CRYPTO_READ32(REG_CRC_OP);
    if ( enabled ) {
        regValue |= CRC_104_DMA;
    } else {
        regValue &= ~((uint32_t)(CRC_104_DMA));

    }
    HAL_CRYPTO_WRITE32(REG_CRC_OP, regValue);

}

static inline void __rtl_crc_set_poly(uint32_t poly)
{
    HAL_CRYPTO_WRITE32(REG_CRC_POLY, poly);
}

static inline void __rtl_crc_set_iv(uint32_t init_value)
{
    HAL_CRYPTO_WRITE32(REG_CRC_IV, init_value);
}

static inline void __rtl_crc_set_oxor(uint32_t oxor)
{
    HAL_CRYPTO_WRITE32(REG_CRC_OXOR, oxor);
}

static inline void __rtl_crc_set_swap(u8 iswap_type, u8 oswap)
{
    volatile uint32_t regValue;

    regValue = HAL_CRYPTO_READ32(REG_CRC_OP);

    //
    regValue &= ~((uint32_t)(0xF));
    regValue |= ((uint32_t)(iswap_type)) & 0x7;
    regValue |= ((uint32_t)(oswap&0x1))<<3;
    //

    HAL_CRYPTO_WRITE32(REG_CRC_OP, regValue);
}

static inline void __rtl_crc_set_crc_type(u8 crc_data_type)
{
    volatile uint32_t regValue;

    regValue = HAL_CRYPTO_READ32(REG_CRC_OP);

    //
    regValue &= ~(((uint32_t)(0x7))<<4);
    regValue |= (crc_data_type&0x7)<<4;

    HAL_CRYPTO_WRITE32(REG_CRC_OP, regValue);
}

static inline void __rtl_crc_set_byte_type(u8 byte_type)
{
    volatile uint32_t regValue;

    regValue = HAL_CRYPTO_READ32(REG_CRC_OP);

    regValue &= ~(((uint32_t)(0x3))<<8);
    regValue |= (uint32_t)(byte_type&0x3)<<8;


    HAL_CRYPTO_WRITE32(REG_CRC_OP, regValue);
}

static inline void __rtl_crc_set_data_length(u16 data_len )
{
    volatile uint32_t regValue;

    regValue = HAL_CRYPTO_READ32(REG_CRC_OP);

    //
    regValue &= ~((uint32_t)0xFFFFF000);

    if ( data_len != 0 ) { // not last
        regValue |= data_len<<16;
    } else {
        regValue |= 0x1000;
    }

    HAL_CRYPTO_WRITE32(REG_CRC_OP, regValue);
}

static inline void __rtl_crc_start(void)
{
    //
    HAL_CRYPTO_WRITE32(REG_CRC_STAT, CRC_118_INTR | CRC_118_INTRMSK | CRC_118_LE );
    //HAL_CRYPTO_WRITE32(REG_CRC_STAT, CRC_118_INTR | CRC_118_INTRMSK  );

    HAL_CRYPTO_WRITE32(REG_CRC_RST, 0x1);

}

static inline void __rtl_crc_set_data(uint32_t data)
{
    HAL_CRYPTO_WRITE32(REG_CRC_DATA, data);
}


static inline int __rtl_crc_wait_done(void)
{
    volatile uint32_t regValue;
    uint32_t i;

    i=0;
    while (i<100000) {
        regValue = HAL_CRYPTO_READ32(REG_CRC_STAT);
        if ( regValue & CRC_118_INTR ) break;
        i++;
    }
    if ( i == 100000 ) {
        __err_printk("CRC wait done failed \r\n");
        return FAIL;
    } else {
        // DMA done clear
        HAL_CRYPTO_WRITE32(REG_CRC_STAT, regValue);
    }
    return SUCCESS;
}


static inline int __rtl_crc_get_result(uint32_t *pCrc)
{
    volatile uint32_t regValue;
    int ret;

    ret = __rtl_crc_wait_done();
    if ( ret != SUCCESS ) return ret;


    regValue = HAL_CRYPTO_READ32(REG_CRC_RES);

    *pCrc = regValue;

    return ret;

}



HAL_ROM_TEXT_SECTION
_LONG_CALL_
int __rtl_crypto_crc_setting(
    hal_crypto_adapter_t *pIE,
    IN int order, IN uint32_t polynom, IN u8 direct, IN uint32_t crcinit, IN uint32_t crcxor, IN uint32_t refin, IN uint32_t refout)
{
    u8 crc_data_type;
    u8 iswap, oswap;

    // check if busy
    if ( __rtl_crc_isBusy() ) {
        __err_printk("CRC engine is busy\r\n");
        return FAIL;
    }

    // order : crc type
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
            __err_printk("CRC order : %d not support \r\n", order);
            return FAIL;
    }
    __rtl_crc_set_crc_type(crc_data_type);

    //
    __rtl_crc_set_poly(polynom);

    // direct : not sure

    // crc_init
    __rtl_crc_set_iv(crcinit);

    // oxor
    __rtl_crc_set_oxor(crcxor);

    // refin , refout
    iswap = (refin)? 4 : 0;
    oswap = (refout)? 1 : 0;
    __rtl_crc_set_swap(iswap, oswap);

    return SUCCESS;

}

HAL_ROM_TEXT_SECTION
_LONG_CALL_
int __rtl_crypto_crc(
    hal_crypto_adapter_t *pIE,
    IN const u8* message,	IN const uint32_t msglen,
    OUT uint32_t* pCrc)
{
    int ret;

    // check if busy
    if ( __rtl_crc_isBusy() ) {
        __err_printk("CRC engine is busy\r\n");
        return FAIL;
    }

    // command mode
    __rtl_crc_set_DMA(1);


    // set data length
    __rtl_crc_set_data_length(msglen);


    __rtl_crc_set_data(message);
    __rtl_crc_start();

    ret = __rtl_crc_get_result(pCrc);

    return ret;


}


HAL_ROM_TEXT_SECTION
_LONG_CALL_
int __rtl_crypto_crc32(
    hal_crypto_adapter_t *pIE,
    IN const u8* message,	IN const uint32_t msglen,
    OUT uint32_t* pCrc)
{
    int i;
    int ret;
    uint32_t data;
    int msglen_4bytes;
    int msglen_rest;


    // setting
    ret = __rtl_crypto_crc_setting(pIE, 32, 0x4c11DB7, 1, 0xFFFFFFFF, 0xFFFFFFFF, 1, 1);
    if ( ret != SUCCESS ) return ret;


    // command mode
    __rtl_crc_set_DMA(1);


    // set data length
    __rtl_crc_set_data_length(msglen);


#if 1 // DMA mode
    __rtl_crc_set_data(message);
    __rtl_crc_start();

#else
    // reset crc engine
    __rtl_crc_start();

    //32 bit
    __rtl_crc_set_byte_type(0);

    msglen_4bytes = (msglen/4)*4;
    msglen_rest = msglen - msglen_4bytes;

    for (i=0; i<msglen_4bytes; i+=4) {
        data = (uint32_t)(message[i]);
        data |= (uint32_t)(message[i+1])<<8;
        data |= (uint32_t)(message[i+2])<<16;
        data |= (uint32_t)(message[i+3])<<24;
        __rtl_crc_set_data(data);
    }

    //8bit
    __rtl_crc_set_byte_type(1);
    for (; i<msglen; i++) {
        data = message[i];
        __rtl_crc_set_data(data);
    }
#endif

    ret = __rtl_crc_get_result(pCrc);

    return ret;

}


