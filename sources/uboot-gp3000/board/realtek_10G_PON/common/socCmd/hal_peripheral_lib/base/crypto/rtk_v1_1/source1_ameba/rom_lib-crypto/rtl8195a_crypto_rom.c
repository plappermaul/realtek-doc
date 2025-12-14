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
//#include "crypto_types.h"


HAL_ROM_TEXT_SECTION 
_LONG_CALL_
int __rtl_memcmpb_v1_00(const u8 *dst, const u8 *src, int bytes)
{
	int i;
	u8  val;

	for (i=0; i<bytes; i++) {
		val = *dst - *src;
		if ( val != 0 ) return val;
		dst++; src++;
	}
	return 0;
}


HAL_ROM_TEXT_SECTION 
_LONG_CALL_
u32 __rtl_random_v1_00(u32 rtl_seed)
{
    u32 hi32, lo32;

    hi32 = (rtl_seed>>16)*19;
    lo32 = (rtl_seed&0xffff)*19+37;
    hi32 = hi32^(hi32<<16);
    return ( hi32^lo32 );
}



HAL_ROM_TEXT_SECTION 
_LONG_CALL_
void __rtl_align_to_be32_v1_00(u32 *pData, int bytes4_num )
{
    int i;
    u32 *ptr;
	u32 data;

    ptr = pData;

    for (i=0; i<bytes4_num; i++ ) {
		data = *ptr;
        *ptr = __rtl_cpu_to_be32(data); 
        ptr++;
    }
}

HAL_ROM_TEXT_SECTION 
_LONG_CALL_
void __rtl_memsetw_v1_00(u32 *pData, u32 data, int bytes4_num )
{
	int i; 
	u32 *ptr;

	ptr = pData;

	for (i=0; i<bytes4_num; i++) {
		*ptr = data;
		ptr++;
	}
}


HAL_ROM_TEXT_SECTION 
_LONG_CALL_
void __rtl_memsetb_v1_00(u8 *pData, u8 data, int bytes4 )
{
	int i; 
	u8 *ptr;

	ptr = pData;

	for (i=0; i<bytes4; i++) {
		*ptr = data;
		ptr++;
	}
}


HAL_ROM_TEXT_SECTION 
_LONG_CALL_
void __rtl_memcpyw_v1_00(u32 *dst, u32 *src, int bytes4_num)
{
	int i; 
	u32 *srcptr, *dstptr;

	srcptr = src;
	dstptr = dst;

	for (i=0; i<bytes4_num; i++) {
		*dstptr = *srcptr;
		dstptr++;
		srcptr++;
	}
}


HAL_ROM_TEXT_SECTION 
_LONG_CALL_
void __rtl_memcpyb_v1_00(u8 *dst, const u8 *src, int bytes)
{
	int i; 
	const u8 *srcptr;
	u8 *dstptr;

	srcptr = src;
	dstptr = dst;

	for (i=0; i<bytes; i++) {
		*dstptr = *srcptr;
		dstptr++;
		srcptr++;
	}
}


HAL_ROM_TEXT_SECTION 
_LONG_CALL_
void __rtl_memDump_v1_00(const u8 *start, u32 size, char * strHeader)
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
    for (index = 0; index < row; index++, line += 16) 
    {
        buf = (u8*)line;

        max = (index == row - 1) ? column : 16;
        if ( max==0 ) break; /* If we need not dump this line, break it. */

        __printk ("\n[%08x] ", line);
        
        //Hex
        for (index2 = 0; index2 < max; index2++)
        {
            if (index2 == 8)
            __printk ("  ");
            __printk ("%02x ", (u8) buf[index2]);
        }

        if (max != 16)
        {
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

#undef __err_printk
#define __err_printk(fmt, args...) \
	     DBG_CRYPTO_ERR("%s(): " fmt "\n", "rtl_cryptoEngine_err", ##args); 


#undef __dbg_mem_dump
#define __dbg_mem_dump(start, size, str_header) \
			if ( ConfigDebugInfo & _DBG_CRYPTO_ ) { \
		    	__printk("%s(): memdump : address: %08x, size: %d\n", "rtl_cryptoEngine_dbg", start, size); \
		    	__memDump((const u8*)(start), size, (char*)(str_header)); \
			}

#endif


//
// Parameters
// 


HAL_ROM_DATA_SECTION
const u32 __AES_Te4[256] = {
    0x63636363U, 0x7c7c7c7cU, 0x77777777U, 0x7b7b7b7bU,
    0xf2f2f2f2U, 0x6b6b6b6bU, 0x6f6f6f6fU, 0xc5c5c5c5U,
    0x30303030U, 0x01010101U, 0x67676767U, 0x2b2b2b2bU,
    0xfefefefeU, 0xd7d7d7d7U, 0xababababU, 0x76767676U,
    0xcacacacaU, 0x82828282U, 0xc9c9c9c9U, 0x7d7d7d7dU,
    0xfafafafaU, 0x59595959U, 0x47474747U, 0xf0f0f0f0U,
    0xadadadadU, 0xd4d4d4d4U, 0xa2a2a2a2U, 0xafafafafU,
    0x9c9c9c9cU, 0xa4a4a4a4U, 0x72727272U, 0xc0c0c0c0U,
    0xb7b7b7b7U, 0xfdfdfdfdU, 0x93939393U, 0x26262626U,
    0x36363636U, 0x3f3f3f3fU, 0xf7f7f7f7U, 0xccccccccU,
    0x34343434U, 0xa5a5a5a5U, 0xe5e5e5e5U, 0xf1f1f1f1U,
    0x71717171U, 0xd8d8d8d8U, 0x31313131U, 0x15151515U,
    0x04040404U, 0xc7c7c7c7U, 0x23232323U, 0xc3c3c3c3U,
    0x18181818U, 0x96969696U, 0x05050505U, 0x9a9a9a9aU,
    0x07070707U, 0x12121212U, 0x80808080U, 0xe2e2e2e2U,
    0xebebebebU, 0x27272727U, 0xb2b2b2b2U, 0x75757575U,
    0x09090909U, 0x83838383U, 0x2c2c2c2cU, 0x1a1a1a1aU,
    0x1b1b1b1bU, 0x6e6e6e6eU, 0x5a5a5a5aU, 0xa0a0a0a0U,
    0x52525252U, 0x3b3b3b3bU, 0xd6d6d6d6U, 0xb3b3b3b3U,
    0x29292929U, 0xe3e3e3e3U, 0x2f2f2f2fU, 0x84848484U,
    0x53535353U, 0xd1d1d1d1U, 0x00000000U, 0xededededU,
    0x20202020U, 0xfcfcfcfcU, 0xb1b1b1b1U, 0x5b5b5b5bU,
    0x6a6a6a6aU, 0xcbcbcbcbU, 0xbebebebeU, 0x39393939U,
    0x4a4a4a4aU, 0x4c4c4c4cU, 0x58585858U, 0xcfcfcfcfU,
    0xd0d0d0d0U, 0xefefefefU, 0xaaaaaaaaU, 0xfbfbfbfbU,
    0x43434343U, 0x4d4d4d4dU, 0x33333333U, 0x85858585U,
    0x45454545U, 0xf9f9f9f9U, 0x02020202U, 0x7f7f7f7fU,
    0x50505050U, 0x3c3c3c3cU, 0x9f9f9f9fU, 0xa8a8a8a8U,
    0x51515151U, 0xa3a3a3a3U, 0x40404040U, 0x8f8f8f8fU,
    0x92929292U, 0x9d9d9d9dU, 0x38383838U, 0xf5f5f5f5U,
    0xbcbcbcbcU, 0xb6b6b6b6U, 0xdadadadaU, 0x21212121U,
    0x10101010U, 0xffffffffU, 0xf3f3f3f3U, 0xd2d2d2d2U,
    0xcdcdcdcdU, 0x0c0c0c0cU, 0x13131313U, 0xececececU,
    0x5f5f5f5fU, 0x97979797U, 0x44444444U, 0x17171717U,
    0xc4c4c4c4U, 0xa7a7a7a7U, 0x7e7e7e7eU, 0x3d3d3d3dU,
    0x64646464U, 0x5d5d5d5dU, 0x19191919U, 0x73737373U,
    0x60606060U, 0x81818181U, 0x4f4f4f4fU, 0xdcdcdcdcU,
    0x22222222U, 0x2a2a2a2aU, 0x90909090U, 0x88888888U,
    0x46464646U, 0xeeeeeeeeU, 0xb8b8b8b8U, 0x14141414U,
    0xdedededeU, 0x5e5e5e5eU, 0x0b0b0b0bU, 0xdbdbdbdbU,
    0xe0e0e0e0U, 0x32323232U, 0x3a3a3a3aU, 0x0a0a0a0aU,
    0x49494949U, 0x06060606U, 0x24242424U, 0x5c5c5c5cU,
    0xc2c2c2c2U, 0xd3d3d3d3U, 0xacacacacU, 0x62626262U,
    0x91919191U, 0x95959595U, 0xe4e4e4e4U, 0x79797979U,
    0xe7e7e7e7U, 0xc8c8c8c8U, 0x37373737U, 0x6d6d6d6dU,
    0x8d8d8d8dU, 0xd5d5d5d5U, 0x4e4e4e4eU, 0xa9a9a9a9U,
    0x6c6c6c6cU, 0x56565656U, 0xf4f4f4f4U, 0xeaeaeaeaU,
    0x65656565U, 0x7a7a7a7aU, 0xaeaeaeaeU, 0x08080808U,
    0xbabababaU, 0x78787878U, 0x25252525U, 0x2e2e2e2eU,
    0x1c1c1c1cU, 0xa6a6a6a6U, 0xb4b4b4b4U, 0xc6c6c6c6U,
    0xe8e8e8e8U, 0xddddddddU, 0x74747474U, 0x1f1f1f1fU,
    0x4b4b4b4bU, 0xbdbdbdbdU, 0x8b8b8b8bU, 0x8a8a8a8aU,
    0x70707070U, 0x3e3e3e3eU, 0xb5b5b5b5U, 0x66666666U,
    0x48484848U, 0x03030303U, 0xf6f6f6f6U, 0x0e0e0e0eU,
    0x61616161U, 0x35353535U, 0x57575757U, 0xb9b9b9b9U,
    0x86868686U, 0xc1c1c1c1U, 0x1d1d1d1dU, 0x9e9e9e9eU,
    0xe1e1e1e1U, 0xf8f8f8f8U, 0x98989898U, 0x11111111U,
    0x69696969U, 0xd9d9d9d9U, 0x8e8e8e8eU, 0x94949494U,
    0x9b9b9b9bU, 0x1e1e1e1eU, 0x87878787U, 0xe9e9e9e9U,
    0xcecececeU, 0x55555555U, 0x28282828U, 0xdfdfdfdfU,
    0x8c8c8c8cU, 0xa1a1a1a1U, 0x89898989U, 0x0d0d0d0dU,
    0xbfbfbfbfU, 0xe6e6e6e6U, 0x42424242U, 0x68686868U,
    0x41414141U, 0x99999999U, 0x2d2d2d2dU, 0x0f0f0f0fU,
    0xb0b0b0b0U, 0x54545454U, 0xbbbbbbbbU, 0x16161616U,
};

HAL_ROM_DATA_SECTION
const u32 __AES_rcon[] = {
    0x01000000, 0x02000000, 0x04000000, 0x08000000,
    0x10000000, 0x20000000, 0x40000000, 0x80000000,
    0x1B000000, 0x36000000, /* for 128-bit blocks, Rijndael never uses more than 10 rcon values */
};

// little-endian system
#undef __GETU32
#define __GETU32(pt) (((u32)(pt)[0] << 24) ^ ((u32)(pt)[1] << 16) ^ \
			((u32)(pt)[2] <<  8) ^ ((u32)(pt)[3]))








// AES functions
HAL_ROM_TEXT_SECTION
_LONG_CALL_
int __rtl_AES_set_encrypt_key(const u8 *userKey, const int bits,
			RTL_SW_AES_KEY *key) {

	u32 *rk;
	int i = 0;
	u32 temp;

	if (!userKey || !key)
		return -1;
	if (bits != 128 && bits != 192 && bits != 256)
		return -2;

	rk = key->rd_key;

	if (bits==128)
		key->rounds = 10;
	else if (bits==192)
		key->rounds = 12;
	else
		key->rounds = 14;

	rk[0] = __GETU32(userKey	   );
	rk[1] = __GETU32(userKey +  4);
	rk[2] = __GETU32(userKey +  8);
	rk[3] = __GETU32(userKey + 12);
	if (bits == 128) {
		for (;;) {
			temp  = rk[3];
			rk[4] = rk[0] ^
				(__AES_Te4[(temp >> 16) & 0xff] & 0xff000000) ^
				(__AES_Te4[(temp >>  8) & 0xff] & 0x00ff0000) ^
				(__AES_Te4[(temp	  ) & 0xff] & 0x0000ff00) ^
				(__AES_Te4[(temp >> 24) 	  ] & 0x000000ff) ^
				__AES_rcon[i];
			rk[5] = rk[1] ^ rk[4];
			rk[6] = rk[2] ^ rk[5];
			rk[7] = rk[3] ^ rk[6];
			if (++i == 10) {
				return 0;
			}
			rk += 4;
		}
	}
	rk[4] = __GETU32(userKey + 16);
	rk[5] = __GETU32(userKey + 20);
	if (bits == 192) {
		for (;;) {
			temp = rk[ 5];
			rk[ 6] = rk[ 0] ^
				(__AES_Te4[(temp >> 16) & 0xff] & 0xff000000) ^
				(__AES_Te4[(temp >>  8) & 0xff] & 0x00ff0000) ^
				(__AES_Te4[(temp	  ) & 0xff] & 0x0000ff00) ^
				(__AES_Te4[(temp >> 24) 	  ] & 0x000000ff) ^
				__AES_rcon[i];
			rk[ 7] = rk[ 1] ^ rk[ 6];
			rk[ 8] = rk[ 2] ^ rk[ 7];
			rk[ 9] = rk[ 3] ^ rk[ 8];
			if (++i == 8) {
				return 0;
			}
			rk[10] = rk[ 4] ^ rk[ 9];
			rk[11] = rk[ 5] ^ rk[10];
			rk += 6;
		}
	}
	rk[6] = __GETU32(userKey + 24);
	rk[7] = __GETU32(userKey + 28);
	if (bits == 256) {
		for (;;) {
			temp = rk[ 7];
			rk[ 8] = rk[ 0] ^
				(__AES_Te4[(temp >> 16) & 0xff] & 0xff000000) ^
				(__AES_Te4[(temp >>  8) & 0xff] & 0x00ff0000) ^
				(__AES_Te4[(temp	  ) & 0xff] & 0x0000ff00) ^
				(__AES_Te4[(temp >> 24) 	  ] & 0x000000ff) ^
				__AES_rcon[i];
			rk[ 9] = rk[ 1] ^ rk[ 8];
			rk[10] = rk[ 2] ^ rk[ 9];
			rk[11] = rk[ 3] ^ rk[10];
			if (++i == 7) {
				return 0;
			}
			temp = rk[11];
			rk[12] = rk[ 4] ^
				(__AES_Te4[(temp >> 24) 	  ] & 0xff000000) ^
				(__AES_Te4[(temp >> 16) & 0xff] & 0x00ff0000) ^
				(__AES_Te4[(temp >>  8) & 0xff] & 0x0000ff00) ^
				(__AES_Te4[(temp	  ) & 0xff] & 0x000000ff);
			rk[13] = rk[ 5] ^ rk[12];
			rk[14] = rk[ 6] ^ rk[13];
			rk[15] = rk[ 7] ^ rk[14];

			rk += 8;
			}
	}
	return 0;
}
	

HAL_ROM_TEXT_SECTION
_LONG_CALL_
int __rtl_cryptoEngine_AES_set_decrypt_key(HAL_CRYPTO_ADAPTER *pIE, const u8 *userKey, const int bits,
            const u8 *outputKey) 
{

	__rtl_AES_set_encrypt_key(userKey, bits, &(pIE->aes_key));

	switch (bits)
	{
		case 128:
			__rtl_memcpyw((u32 *) UNCACHED_ADDRESS(outputKey), &(pIE->aes_key).rd_key[4*10], 16/4);
			break;
			
		case 192:
			__rtl_memcpyw((u32 *) UNCACHED_ADDRESS(outputKey), &(pIE->aes_key).rd_key[4*12], 16/4);
			__rtl_memcpyw((u32 *) UNCACHED_ADDRESS(&outputKey[16]), &(pIE->aes_key).rd_key[4*11+2], 8/4);
			break;
			
		case 256:
			__rtl_memcpyw((u32 *) UNCACHED_ADDRESS(outputKey), &(pIE->aes_key).rd_key[4*14], 16/4);
			__rtl_memcpyw((u32 *) UNCACHED_ADDRESS(&outputKey[16]), &(pIE->aes_key).rd_key[4*13], 16/4);
			break;
			
		default:
			return FAIL;
	}

	return SUCCESS;
}


static inline 
int __rtl_cryptoEngine_get_cipher_type(HAL_CRYPTO_ADAPTER *pIE)
{
	return (pIE->cipher_type & CIPHER_TYPE_MASK_FUNC);
}




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
    *pRingAddr = UNCACHED_ADDRESS(rambuf);

    // set data field for the start and the end
    __rtl_memsetb((u8*)(*pRingAddr), 0, descNum * eachsize);

    return SUCCESS;
}

static inline 
int __rtl_cryptoEngine_srcDesp_init(HAL_CRYPTO_ADAPTER* pIE)
{
    int ret;
    RTL_CRYPTO_SOURCE_T *g_ipssdar;
    
    ret = __rtl_cryptoEngine_setDespRing((void**)&(pIE->g_ipssdar), 
                                                pIE->desc_num,  (void*)(&(pIE->g_src_buf[0])), sizeof(RTL_CRYPTO_SOURCE_T));
    if ( ret != SUCCESS ) {
        return ret;
    }

    pIE->idx_srcdesc = 0;
    (pIE->g_ipssdar)[pIE->desc_num -1].eor = 1;
	
    __rtl_align_to_be32((u32*)(&((pIE->g_ipssdar)[pIE->desc_num -1])), 
		sizeof(RTL_CRYPTO_SOURCE_T)/4);
	

    g_ipssdar = pIE->g_ipssdar;
    HAL_CRYPTO_WRITE32(REG_IPSSDAR, PHYSICAL_ADDRESS(g_ipssdar));

	return SUCCESS;
    
}
    
static inline 
int __rtl_cryptoEngine_dstDesp_init(HAL_CRYPTO_ADAPTER* pIE)
{
    int ret;
    RTL_CRYPTO_DEST_T *g_ipsddar;
    
    ret = __rtl_cryptoEngine_setDespRing((void**)&(pIE->g_ipsddar), 
                                                pIE->desc_num,  (void*)(&(pIE->g_dst_buf[0])), sizeof(RTL_CRYPTO_DEST_T));
    if ( ret != SUCCESS ) return ret;

    pIE->idx_dstdesc = 0;
    (pIE->g_ipsddar)[pIE->desc_num- 1].eor = 1;
	
    __rtl_align_to_be32((u32*)(&((pIE->g_ipsddar)[pIE->desc_num -1])), 
		sizeof(RTL_CRYPTO_DEST_T)/4);
	

    // set into HW Reg : IPSDDAR
    g_ipsddar = pIE->g_ipsddar;
    HAL_CRYPTO_WRITE32(REG_IPSDDAR, PHYSICAL_ADDRESS((u32) g_ipsddar));

	return SUCCESS;
    
}

//
// Neo : 2014.06.17 : Source Crypto Descriptor ---
//


static inline 
int __rtl_cryptoEngine_alloc(HAL_CRYPTO_ADAPTER* pIE, u32 descNum) 
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
int __rtl_cryptoEngine_free(HAL_CRYPTO_ADAPTER* pIE) 
{
    if (pIE == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
    
    if (pIE->g_ipssdar)
    {
        HAL_CRYPTO_WRITE32(REG_IPSSDAR, 0);
        pIE->g_ipssdar = NULL;
    }

    if (pIE->g_ipsddar)
    {
        HAL_CRYPTO_WRITE32(REG_IPSDDAR, 0);
        pIE->g_ipsddar = NULL;
    }

    return SUCCESS;
}



static inline  
RTL_CRYPTO_SOURCE_T* __rtl_cryptoEngine_get_new_srcDesc(HAL_CRYPTO_ADAPTER *pIE)
{
    int idx_srcdesc;
    RTL_CRYPTO_SOURCE_T *pSrcDesc;
    
    idx_srcdesc = pIE->idx_srcdesc;
    pSrcDesc = &(pIE->g_ipssdar[idx_srcdesc]);

    __rtl_memsetb((u8*)pSrcDesc, 0, sizeof(RTL_CRYPTO_SOURCE_T));

    if ( idx_srcdesc == (pIE->desc_num -1) ) { // The last one
        pSrcDesc->eor = 1;
    }
    return pSrcDesc;
}

static inline  
RTL_CRYPTO_DEST_T* __rtl_cryptoEngine_get_new_dstDesc(HAL_CRYPTO_ADAPTER *pIE)
{
    int idx_dstdesc;
    RTL_CRYPTO_DEST_T *pDstDesc;
    
    idx_dstdesc = pIE->idx_dstdesc;
    pDstDesc = &(pIE->g_ipsddar[idx_dstdesc]);

    __rtl_memsetb((u8*)pDstDesc, 0, sizeof(RTL_CRYPTO_DEST_T));

    if ( idx_dstdesc == (pIE->desc_num -1) ) { // The last one
        pDstDesc->eor = 1;
    }
    return pDstDesc;
}


static inline
void __rtl_cryptoEngine_set_digestlen(HAL_CRYPTO_ADAPTER *pIE, IN const u32 auth_type)
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
int __rtl_cryptoEngine_set_key(HAL_CRYPTO_ADAPTER *pIE, 
	IN const u8* pCipherKey, IN const int lenCipherKey, 
	IN const u8* pAuthKey, IN const int lenAuthKey ) 
{

	pIE->lenCipherKey = lenCipherKey;
	pIE->pCipherKey = pCipherKey;


	pIE->pAuthKey = pAuthKey;
	pIE->lenAuthKey = lenAuthKey;

	if ( lenAuthKey > 0 ) {
		int i;
		
		
		pIE->ipad = (u8 *) UNCACHED_ADDRESS(&(pIE->g_IOPAD[0]));
		pIE->opad = (u8 *) UNCACHED_ADDRESS(&(pIE->g_IOPAD[CRYPTO_PADSIZE]));
		
		__rtl_memsetw((u32*)(pIE->ipad), 0x36363636, CRYPTO_PADSIZE/4);
		__rtl_memsetw((u32*)(pIE->opad), 0x5c5c5c5c, CRYPTO_PADSIZE/4);
		
		for (i=0; i< lenAuthKey; i++) 
		{
			pIE->ipad[i] ^= ((u8*) pIE->pAuthKey)[i];
			pIE->opad[i] ^= ((u8*) pIE->pAuthKey)[i];
		}
	} else {
		pIE->ipad = 0;
		pIE->opad = 0;
	}


	switch ( __rtl_cryptoEngine_get_cipher_type(pIE) ) {
		case CIPHER_TYPE_FUNC_AES : 
	    {
			u32* pAESDecryptKey;
			int keylen;
			
			pAESDecryptKey = (u32 *) UNCACHED_ADDRESS(&(pIE->decryptKey[0]));
			keylen = pIE->lenCipherKey;

			__rtl_cryptoEngine_AES_set_decrypt_key(pIE, pIE->pCipherKey, keylen*8, (const u8*)pAESDecryptKey);

			__rtl_align_to_be32(pAESDecryptKey, keylen/4);
		}
			break;

		case CIPHER_TYPE_FUNC_3DES :
		{
			u8 *swapped;
			const u8* pk = pIE->pCipherKey; /* for compiler */
			
			swapped = (u8 *) UNCACHED_ADDRESS(&pIE->decryptKey[0]);
			/* for descryption, we need to swap K1 and K3 (K1,K2,K3)==>(K3,K2,K1). */
			__rtl_memcpyw((u32*)(&swapped[16]), (u32*)(&pk[0]), 8/4);
			__rtl_memcpyw((u32*)(&swapped[8]), (u32*)(&pk[8]), 8/4);
			__rtl_memcpyw((u32*)(&swapped[0]), (u32*)(&pk[16]), 8/4);
		}
			break;

		default:
			break;
	}

	return SUCCESS;

}



HAL_ROM_TEXT_SECTION
_LONG_CALL_
int __rtl_cryptoEngine_set_security_mode_v1_00(HAL_CRYPTO_ADAPTER *pIE, 
        IN const u32 mode_select, IN const u32 cipher_type, IN const u32 auth_type,
		IN const void* pCipherKey, IN const u32 lenCipherKey, 
        IN const void* pAuthKey, IN const u32 lenAuthKey
        )
{
    //int i;

	if ( pIE == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT;

	if ( lenAuthKey > 0 ) { // Authentication
		if ( pAuthKey == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
		if ( lenAuthKey > CRYPTO_AUTH_PADDING ) return _ERRNO_CRYPTO_KEY_OutRange;
	}

    pIE->mode_select = mode_select;


	// Crypto

	pIE->trides  = 0;
	pIE->aes		 = 0;
	pIE->des		 = 0;
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
		}

		pIE->isDecrypt  = ( cipher_type & CIPHER_TYPE_MODE_ENCRYPT )? 0: 1;
	} 
	


	// Auth
	
    pIE->auth_type = auth_type; 
	if ( auth_type != AUTH_TYPE_NO_AUTH){
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
	//dbg_printk("auth_type=0x%x, isHMAC=%d, isMD5=%d, isSHA1=%d, isSHA2=%d, sha2type=%d\n", 
	//		auth_type, pIE->isHMAC, pIE->isMD5, pIE->isSHA1, pIE->isSHA2, pIE->sha2type); 

	__rtl_cryptoEngine_set_digestlen(pIE, auth_type);

	__rtl_cryptoEngine_set_key(pIE, pCipherKey, lenCipherKey, pAuthKey, lenAuthKey);


	return SUCCESS;	
}


/*
 *  descNum -- number of descriptor to be allocated.
 *  mode32Bytes -- 0: 16word
 *                 1: 32word
 *                 2: 64word
 */
HAL_ROM_TEXT_SECTION
_LONG_CALL_
int __rtl_cryptoEngine_init_v1_00(HAL_CRYPTO_ADAPTER *pIE)
{
	int ret;
    u32 burstSize;

    int descNum;
    int mode32Bytes;
	u32 DDLEN_setting = IPS_DDLEN_12; 

    descNum = pIE->desc_num;
	if ( (descNum <=0) || (descNum > CRYPTO_MAX_DESP ) ) 
		return _ERRNO_CRYPTO_DESC_NUM_SET_OutRange;
	
    mode32Bytes = pIE->dma_mode;

    LXBUS_FCTRL(ON);
    SECURITY_FCTRL(ON);
    
    // enable IPSec related clock
    ACTCK_SEC_ENG_CCTRL(ON);
    SLPCK_SEC_ENG_CCTRL(ON);

    // mode32Bytes == 0 or 1 is backward-compatible 
    switch ( mode32Bytes ) {
		case 0 : 
	        burstSize = IPS_DMBS_16 | IPS_SMBS_16;
			break;
		case 1 : 
	        burstSize = IPS_DMBS_32 | IPS_SMBS_32;
			break;
		case 2 : 
	        burstSize = IPS_DMBS_64 | IPS_SMBS_64;
			break;
		case 3 : 
	        burstSize = IPS_DMBS_128 | IPS_SMBS_128;
			break;
		default : 
	        return _ERRNO_CRYPTO_BURST_NUM_SET_OutRange;
    }

    // Software Reset 
    HAL_CRYPTO_WRITE32(REG_IPSCSR, HAL_CRYPTO_READ32(REG_IPSCSR) | IPS_SRST);

    ret = __rtl_cryptoEngine_free(pIE);
	if ( ret != SUCCESS ) return ret;
	
    ret = __rtl_cryptoEngine_alloc(pIE, descNum);
	if ( ret != SUCCESS ) return ret;
    

    // We must delay a while after software reset. 
    if (pIE->sawb_mode)
        HAL_CRYPTO_WRITE32(REG_IPSCTR, 0 | IPS_SAWB | IPS_CKE | burstSize | DDLEN_setting );
    else
        HAL_CRYPTO_WRITE32(REG_IPSCTR, 0 | IPS_CKE | burstSize | DDLEN_setting );


    // write 1 to clear 
    HAL_CRYPTO_WRITE32(REG_IPSCSR, HAL_CRYPTO_READ32(REG_IPSCSR) | IPS_SDUEIP | IPS_SDLEIP | IPS_DDUEIP | IPS_DDOKIP);


    pIE->isInit = 1;
    
    return SUCCESS;
}


HAL_ROM_TEXT_SECTION
_LONG_CALL_
int __rtl_cryptoEngine_exit_v1_00(HAL_CRYPTO_ADAPTER *pIE)
{
    pIE->isInit = 0;

    // disable IPSec related clock
    ACTCK_SEC_ENG_CCTRL(OFF);
    SLPCK_SEC_ENG_CCTRL(OFF);

    // 
    SECURITY_FCTRL(OFF);
    

    __rtl_cryptoEngine_free(pIE);
    return SUCCESS;
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_
int __rtl_cryptoEngine_reset_v1_00(HAL_CRYPTO_ADAPTER *pIE)
{
    pIE->isInit = 0;

    // disable  related clocks
    ACTCK_SEC_ENG_CCTRL(OFF);
    SLPCK_SEC_ENG_CCTRL(OFF);

    // 
    SECURITY_FCTRL(OFF);

    //
    
    return __rtl_cryptoEngine_init_v1_00(pIE);
}




static inline 
RTL_CRYPTO_SOURCE_T* __rtl_cryptoEngine_set_and_next_srcDesc(HAL_CRYPTO_ADAPTER *pIE)
{
    RTL_CRYPTO_SOURCE_T *pSrcDesc;
   
    pSrcDesc = &(pIE->g_ipssdar[pIE->idx_srcdesc]);
    pSrcDesc->own = 1; //set ower to HW engine

    __rtl_align_to_be32((u32*)pSrcDesc, sizeof(RTL_CRYPTO_SOURCE_T)/4);

	__dbg_mem_dump(pSrcDesc, sizeof(RTL_CRYPTO_SOURCE_T), "src Desc:"); 

    pIE->idx_srcdesc = ( pIE->idx_srcdesc + 1 ) % pIE->desc_num;

    return __rtl_cryptoEngine_get_new_srcDesc(pIE);
}





static inline
void __rtl_cryptoEngine_srcDesc_generate_key_IV(HAL_CRYPTO_ADAPTER *pIE, 
	const u8* pIv, u32 ivLen)
{
    RTL_CRYPTO_SOURCE_T *srcDesc;
	u32 cipher_type;
	u32 block_mode;
	//u32 auth_type;
	u32 lenCipherKey;
	const u8*	pCipherKey;


	cipher_type = pIE->cipher_type;
	if ( cipher_type == CIPHER_TYPE_NO_CIPHER) return;
	
	//auth_type  = pIE->auth_type;
	lenCipherKey = pIE->lenCipherKey;

	// set key
	if ( pIE->isDecrypt && (pIE->trides || pIE->aes)) { // decrypt for 3DES or AES 
		pCipherKey = &((pIE->decryptKey)[0]);
	} else {
		pCipherKey = pIE->pCipherKey;
	}


	srcDesc = __rtl_cryptoEngine_get_new_srcDesc(pIE);

	srcDesc->fs = 1;
	srcDesc->ms = pIE->mode_select;


	// CBC / CTR;
	block_mode = cipher_type & CIPHER_TYPE_MASK_BLOCK;
	switch ( block_mode )
	{
		case CIPHER_TYPE_BLOCK_CBC: /* CBC mode */
			srcDesc->cbc = 1;
			break;
		case CIPHER_TYPE_BLOCK_ECB: /* ECB mode */
			srcDesc->cbc = 0;
			break;
		case CIPHER_TYPE_BLOCK_CTR: /* Counter mode */
			srcDesc->ctr = 1;
			break;
		default:
			//return FAIL;
			return;
	}

	// kam
	
	if ( pIE->trides) { // 3DES
		srcDesc->kam = ( pIE->isDecrypt )? 2 : 5;
		// TODO: set EDE for 3DES first
 	} else { // AES / DES
		srcDesc->kam = ( pIE->isDecrypt )? 0 : 7; 		
	}


	// AESKL
	if ( pIE->aes ) {// AES 
		u32 aesKeyLen;

		switch (lenCipherKey) {
			case 128/8:
				aesKeyLen = _CIPHER_AES_KEYLEN_128BIT;
				break;
			case 192/8:
				aesKeyLen = _CIPHER_AES_KEYLEN_192BIT;
				break;
			case 256/8:
				aesKeyLen = _CIPHER_AES_KEYLEN_256BIT;
				break;
			default:
				//return FAIL;
				return;
		}

 		srcDesc->aeskl = aesKeyLen;

	} else { // DES/3DES
		srcDesc->aeskl= 0;
	}

	// 3DES
	srcDesc->trides = pIE->trides;


	// HMAC
	srcDesc->hmac = pIE->isHMAC;

	// AH
	srcDesc->md5 = pIE->isMD5;

	/* KEY descriptor */
	srcDesc->sbl = lenCipherKey; /* Key Array Length */
	
	srcDesc->a2eo = pIE->a2eo;
	srcDesc->enl = pIE->enl;
	srcDesc->apl = pIE->apl;

	srcDesc->sdbp = (u32)PHYSICAL_ADDRESS(pCipherKey);

	__dbg_mem_dump(srcDesc->sdbp, srcDesc->sbl, "KEY: ");
	srcDesc = __rtl_cryptoEngine_set_and_next_srcDesc(pIE);


	/* IV descriptor */
	srcDesc->own = 1; /* Own by crypto */
	srcDesc->fs = 0;

	srcDesc->sbl = ivLen;
	
	srcDesc->sdbp = (u32)PHYSICAL_ADDRESS(pIv);
	__dbg_mem_dump(srcDesc->sdbp, srcDesc->sbl, "IV: ");

	srcDesc = __rtl_cryptoEngine_set_and_next_srcDesc(pIE);
  
}

static inline  
void __rtl_cryptoEngine_set_srcDesc_IOPad(HAL_CRYPTO_ADAPTER *pIE)
{

    int idx_srcdesc;
    RTL_CRYPTO_SOURCE_T *pSrcDesc;

    
    idx_srcdesc = pIE->idx_srcdesc;
    pSrcDesc = &(pIE->g_ipssdar[idx_srcdesc]);

    pSrcDesc->ms = pIE->mode_select;
	pSrcDesc->isSHA2 = pIE->isSHA2;
    pSrcDesc->md5 = pIE->isMD5;
    pSrcDesc->hmac = pIE->isHMAC;

	if (pSrcDesc->isSHA2) {
		switch ( pIE->digestlen ) {
			case 224/8 : // SHA-224
				pSrcDesc->sha2 = 0;
				break;
			case 256/8 : // SHA-256
				pSrcDesc->sha2 = 1;
				break;
			case 384/8 : // SHA-384
				pSrcDesc->sha2 = 2;
				break;
			case 512/8 : // SHA-512
				pSrcDesc->sha2 = 3;
				break;
			default:
				__err_printk("length(%d) for SHA2 is not correct", pIE->digestlen);
				break;
		}
	}

    if (pIE->isHMAC) {
        pSrcDesc->sbl = 128; /* PAD size */
    }
    else {
        pSrcDesc->sbl = 0;//128; /* PAD size */
    }

	// IO Pad has been generated when doing rtl_cryptoEngine_set_security_mode()
    pSrcDesc->sdbp = (u32)PHYSICAL_ADDRESS(pIE->ipad);
	
	__dbg_mem_dump(pSrcDesc->sdbp, pSrcDesc->sbl, "PAD: ");
	//dbg_printk("sbl=%d\n", pSrcDesc->sbl);
	//dbg_printk("sdbp=0x%x\n", pSrcDesc->sdbp);

}

static inline 
void __rtl_cryptoEngine_srcDesc_gen_PAD(HAL_CRYPTO_ADAPTER *pIE)
{
    RTL_CRYPTO_SOURCE_T *srcDesc;


	if ( pIE->auth_type == AUTH_TYPE_NO_AUTH ) return;

	srcDesc = __rtl_cryptoEngine_get_new_srcDesc(pIE);

    /* HMAC_PAD descriptor */
	if ( pIE->cipher_type == CIPHER_TYPE_NO_CIPHER )
        srcDesc->fs = 1; /* AuthOnly mode */

    __rtl_cryptoEngine_set_srcDesc_IOPad(pIE);
    srcDesc->a2eo = pIE->a2eo;
    srcDesc->enl = pIE->enl;
    srcDesc->apl = pIE->apl;

    srcDesc = __rtl_cryptoEngine_set_and_next_srcDesc(pIE);

}

static inline 
void __rtl_cryptoEngine_srcDesc_gen_HMACPadding(HAL_CRYPTO_ADAPTER *pIE)
{
    RTL_CRYPTO_SOURCE_T *srcDesc;
    u64 len64;
    u8 *u8Ptr = (u8 *)&len64;	
	u32 totalLen;
	u32 apl;
	int    i;
	u8 *pAuthPadding;



	if ( pIE->auth_type == AUTH_TYPE_NO_AUTH ) return;

	srcDesc = __rtl_cryptoEngine_get_new_srcDesc(pIE);
	
	apl = pIE->apl;
	totalLen = pIE->enl;

    /* build padding pattern */
    pAuthPadding = (u8 *) UNCACHED_ADDRESS(&(pIE->g_authPadding[0]));
    pAuthPadding[0] = 0x80; /* the first byte */
    for (i = 1; i < apl - 8; i++)
        pAuthPadding[i] = 0; /* zero bytes */

    /* final, length bytes */
    if (pIE->isHMAC)
        len64 = (totalLen + 64) << 3; /* First padding length is */
    else
        len64 = totalLen << 3; /* First padding length is */


    for (i=0; i<8; i++) 
    {
        if (pIE->isMD5)
            pAuthPadding[apl - i - 1] = u8Ptr[i];
        else
            pAuthPadding[apl - 8 + i] = u8Ptr[i];
    }

    u32 *swaptemp1,*swaptemp2, temp123;
    swaptemp1 = (u32*) (&pAuthPadding[apl-8]);            
    *(swaptemp1) = __rtl_cpu_to_be32(*(swaptemp1));
    temp123 = *(swaptemp1);
    swaptemp2 = (u32*) (&pAuthPadding[apl-4]);            
    *(swaptemp2) = __rtl_cpu_to_be32(*(swaptemp2));
    *(swaptemp1) = *(swaptemp2);
    *(swaptemp2) = temp123;



    /* AUTH_PAD descriptor */
    srcDesc->own = 1; /* Own by crypto */
    srcDesc->fs = 0;
    srcDesc->sbl = apl; /* PAD size */
    srcDesc->sdbp = (u32)PHYSICAL_ADDRESS(pAuthPadding);
	__dbg_mem_dump(srcDesc->sdbp, srcDesc->sbl, "Padding: ");

    srcDesc = __rtl_cryptoEngine_set_and_next_srcDesc(pIE);
}



static inline 
int __rtl_cryptoEngine_kickoff(HAL_CRYPTO_ADAPTER *pIE)
{
	u32 ips_errmask;
	u32 loopWait;

	ips_errmask = IPS_SDUEIP | IPS_SDLEIP | IPS_DDUEIP |IPS_DABFIP;


	// clear flags 
	HAL_CRYPTO_WRITE32(REG_IPSCSR, ips_errmask | IPS_DDOKIP );

	// start!
	HAL_CRYPTO_WRITE32(REG_IPSCSR, IPS_POLL);

	// wait until ipsec engine stop 
	loopWait = 1000000; /* hope long enough */
	while (( HAL_CRYPTO_READ32(REG_IPSCSR) & (ips_errmask | IPS_DDOKIP )) == 0)
	{
	    loopWait--;
	    if (loopWait == 0)
	    {
	        __err_printk("Wait Timeout. READ_MEM32(IPSCSR)=0x%08x.\n",
	            HAL_CRYPTO_READ32(REG_IPSCSR));

	        return FAIL; /* error occurs */
	    }
	}

	if ( HAL_CRYPTO_READ32(REG_IPSCSR) & ips_errmask)
	{
		__err_printk("IPS_SDLEIP or IPS_DABFI is ON. READ_MEM32(IPSCSR)=0x%08x.\n",
			HAL_CRYPTO_READ32( REG_IPSCSR));
		
		return FAIL; /* error occurs */
	}
	
	return SUCCESS;
}

static inline RTL_CRYPTO_DEST_T* __rtl_cryptoEngine_set_DstDesc_and_Kickoff(HAL_CRYPTO_ADAPTER *pIE)
{
    RTL_CRYPTO_DEST_T *pDstDesc;
   
    pDstDesc = &(pIE->g_ipsddar[pIE->idx_dstdesc]);
    pDstDesc->own = 1; //set ower to HW engine

    __rtl_align_to_be32((u32*)pDstDesc, sizeof(RTL_CRYPTO_DEST_T)/4); // only set the first 4 byte(32 bit) for dest descriptor

	__dbg_mem_dump(pDstDesc, sizeof(RTL_CRYPTO_DEST_T), " dst desc : ");

    if ( __rtl_cryptoEngine_kickoff(pIE)== SUCCESS ) {

		if (pDstDesc->own != 0) // IPSec Engine doesn't return the result
		{
			__err_printk("own bit error READ_MEM32(IPSCSR)=0x%08x.\n", 
				HAL_CRYPTO_READ32( REG_IPSCSR));
			return NULL; /* error occurs */
		}

	    return pDstDesc;
    } else {
    	__err_printk("IPSec Engine kick off FAIL! \n");
		return NULL;
    }
}

static inline 
void __rtl_cryptoEngine_dstDesc_move_next(HAL_CRYPTO_ADAPTER *pIE)
{
	pIE->idx_dstdesc = (pIE->idx_dstdesc + 1) % pIE->desc_num;	  	
}

static inline 
void __rtl_cryptoEngine_dstDesc_get_digest(HAL_CRYPTO_ADAPTER *pIE, void* pDigest)
{
    RTL_CRYPTO_DEST_T *pDstDesc;

    pDstDesc = &(pIE->g_ipsddar[pIE->idx_dstdesc]);
	__rtl_memcpyb((u8*)pDigest, (const u8*)(pDstDesc->icv), pIE->digestlen);
}



static inline 
void __rtl_cryptoEngine_set_length(HAL_CRYPTO_ADAPTER *pIE, 
	u32 a2eo, u32 enl, u32 apl)
{
	pIE->a2eo = a2eo;
	pIE->enl  = enl;
	pIE->apl  = apl;
}




static inline 
u32 __rtl_cryptoEngine_auth_calc_apl(HAL_CRYPTO_ADAPTER *pIE, IN const u32 totalLen)
{
	u32 auth_type;
	u32 apl;

	auth_type = pIE->auth_type;
	if (auth_type != AUTH_TYPE_NO_AUTH) {
		u32 authPadSpace;
		
		/* calculate APL */
		authPadSpace = ((totalLen + CRYPTO_AUTH_PADDING-1) &
			(~(CRYPTO_AUTH_PADDING-1))) - totalLen;


		if (authPadSpace > 8)
			// Yes, we have enough space to store 0x80 and dmaLen
			apl = authPadSpace; 
		else
			// We don't have enough space to store 0x80 and dmaLen. Allocate another 64 bytes.
			apl = authPadSpace + CRYPTO_AUTH_PADDING;
	} else {
		apl = 0;
	}

	return apl;
}


HAL_ROM_TEXT_SECTION
_LONG_CALL_
int __rtl_cryptoEngine_v1_00(HAL_CRYPTO_ADAPTER *pIE, 
    IN const u8 *message, 	IN const u32 msglen, 
    IN const u8 *pIv, 		IN const u32 ivlen,
    IN const u32 a2eo, 		OUT void *pResult) 
{
    RTL_CRYPTO_SOURCE_T *srcDesc;
	RTL_CRYPTO_DEST_T *dstDesc;

    u32 totalLen = 0;
	u32 digestlen;
	u32 enl;
    u32 apl;


	// Use only one scatter	
	enl = msglen;
    totalLen = a2eo + enl;

//
//   Set relative length data
//

	apl = __rtl_cryptoEngine_auth_calc_apl(pIE, totalLen);


	__rtl_cryptoEngine_set_length(pIE, a2eo, enl, apl);

	digestlen = pIE->digestlen;

    /***************************************
    prepare source data descriptor:

         ----------
        |Key array   |
         ----------
        |IV array     |
         ----------
        |PAD array  |
         ----------
        |Data 1       |
        |   .          |
        |   .          |
        |   .          |
        |Data N       |
         ----------
        |HMAC APL  |
         ----------
    
    *****************************************/

    /********************************************
     * step 1: prepare Key & IV array: 
     ********************************************/

	__rtl_cryptoEngine_srcDesc_generate_key_IV(pIE, pIv, ivlen);

    /********************************************
     * step 2: prepare PAD array:
     ********************************************/

	__rtl_cryptoEngine_srcDesc_gen_PAD(pIE);


    /********************************************
     * step 3: prepare Data1 ~ DataN
     ********************************************/
	srcDesc = __rtl_cryptoEngine_get_new_srcDesc(pIE);

    //for (i = 0; i < cntScatter; i++)
    {

        /* DATA descriptor */
        //srcDesc->own = 1;
        srcDesc->fs = 0;
        srcDesc->sbl = msglen; /* Data Length */
        srcDesc->sdbp = (u32)PHYSICAL_ADDRESS(message);

        __dbg_mem_dump(message, msglen, "data: ");

        srcDesc = __rtl_cryptoEngine_set_and_next_srcDesc(pIE);
    }

    /********************************************
     * step 4: prepare padding for MD5 and SHA-1
     ********************************************/
	__rtl_cryptoEngine_srcDesc_gen_HMACPadding(pIE);


    /********************************************
     * step 5: write software parameters to ASIC
     ********************************************/
    /********************************************
     * prepare destination descriptor.
     ********************************************/

 	dstDesc = __rtl_cryptoEngine_get_new_dstDesc(pIE);
	if ( pIE->cipher_type != CIPHER_TYPE_NO_CIPHER ) {
		dstDesc->dbl = totalLen+digestlen;
		dstDesc->ddbp = (u32)PHYSICAL_ADDRESS(pResult);
	} else {
		dstDesc->dbl = 0;
		dstDesc->ddbp = 0;
	}


    /********************************************
     * write prepared descriptors into ASIC
     ********************************************/

	dstDesc = __rtl_cryptoEngine_set_DstDesc_and_Kickoff(pIE);

	if ( pIE->mode_select == _MS_TYPE_AUTH ) {
		__rtl_cryptoEngine_dstDesc_get_digest(pIE, pResult); 
		__dbg_mem_dump(pResult, digestlen, "pResult: ");
	}
	
	__rtl_cryptoEngine_dstDesc_move_next(pIE);

    return SUCCESS;
}


//
// Cipher Functions
//


HAL_ROM_TEXT_SECTION
_LONG_CALL_
int __rtl_crypto_cipher_init_v1_00(HAL_CRYPTO_ADAPTER *pIE, 
				IN const u32 cipher_type, 
				IN const u8* key, IN const u32 keylen)
{
	u8* pAuthKey = NULL;
	u32 lenAuthKey = 0;
	
	return __rtl_cryptoEngine_set_security_mode_v1_00(pIE, 
		_MS_TYPE_CRYPT, cipher_type, AUTH_TYPE_NO_AUTH,
		key, keylen, pAuthKey, lenAuthKey);

}

static inline 
void __rtl_cryptoEngine_set_encrypt(HAL_CRYPTO_ADAPTER *pIE)
{
	pIE->cipher_type |= CIPHER_TYPE_MODE_ENCRYPT;
	pIE->isDecrypt = _FALSE;
}

static inline 
void __rtl_cryptoEngine_set_decrypt(HAL_CRYPTO_ADAPTER *pIE)
{
	pIE->cipher_type ^= CIPHER_TYPE_MODE_ENCRYPT; 
	pIE->isDecrypt = _TRUE;
}


HAL_ROM_TEXT_SECTION
_LONG_CALL_
int __rtl_crypto_cipher_encrypt_v1_00(HAL_CRYPTO_ADAPTER *pIE, 
			IN const u32 cipher_type, 	
			IN const u8* message, 	IN const u32 msglen, 
			IN const u8* piv, 		IN const u32 ivlen, 
			OUT u8* pResult)
{
	int ret;
	u32 a2eo = 0;

	//rtl_assert((msglen&0xF)==0);

	__rtl_cryptoEngine_set_encrypt(pIE);
	ret = __rtl_cryptoEngine_v1_00(pIE, message, msglen, 
		piv, ivlen, a2eo, pResult);
	

	return ret;
}


HAL_ROM_TEXT_SECTION
_LONG_CALL_
int __rtl_crypto_cipher_decrypt_v1_00(HAL_CRYPTO_ADAPTER *pIE, 
			IN const u32 cipher_type, 	IN const u8* message, 	IN const u32 msglen, 
			IN const u8* piv, 			IN const u32 ivlen, 	
			OUT u8* pResult )
{
	int ret;
	u32 a2eo = 0;

	//rtl_assert((msglen&0xF)==0);

	__rtl_cryptoEngine_set_decrypt(pIE);
	ret = __rtl_cryptoEngine_v1_00(pIE, message, msglen, piv, ivlen, a2eo, pResult);
	

	return ret;
}





