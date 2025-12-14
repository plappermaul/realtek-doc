#ifndef RTL_IPSEC_H
#define RTL_IPSEC_H

#define SYSTEM_BASE		0xB8000000
#define CLK_MANAGE		0xB8000010
#define CRYPTO_BASE		(SYSTEM_BASE+0xC000)	/* 0xB801C000 */
#define IPSSDAR			(CRYPTO_BASE+0x00)	/* IPSec Source Descriptor Starting Address Register */
#define IPSDDAR			(CRYPTO_BASE+0x04)	/* IPSec Destination Descriptor Starting Address Register */
#define IPSCSR			(CRYPTO_BASE+0x08)	/* IPSec Command/Status Register */
#define IPSCTR			(CRYPTO_BASE+0x0C)	/* IPSec Control Register */

/* IPSec Command/Status Register */
#define IPS_SDUEIP		(1<<15)				/* Source Descriptor Unavailable Error Interrupt Pending */
#define IPS_SDLEIP		(1<<14)				/* Source Descriptor Length Error Interrupt Pending */
#define IPS_DDUEIP		(1<<13)				/* Destination Descriptor Unavailable Error Interrupt Pending */
#define IPS_DDOKIP		(1<<12)				/* Destination Descriptor OK Interrupt Pending */
#define IPS_DABFIP		(1<<11)				/* Data Address Buffer Interrupt Pending */
#define IPS_POLL		(1<<1)				/* Descriptor Polling. Set 1 to kick crypto engine to fetch source descriptor. */
#define IPS_SRST		(1<<0)				/* Software reset, write 1 to reset */

/* IPSec Control Register */
#define IPS_SDUEIE		(1<<15)				/* Source Descriptor Unavailable Error Interrupt Enable */
#define IPS_SDLEIE		(1<<14)				/* Source Descriptor Length Error Interrupt Enable */
#define IPS_DDUEIE		(1<<13)				/* Destination Descriptor Unavailable Error Interrupt Enable */
#define IPS_DDOKIE		(1<<12)				/* Destination Descriptor OK Interrupt Enable */
#define IPS_DABFIE		(1<<11)				/* Data Address Buffer Interrupt Enable */
#define IPS_LBKM		(1<<8)				/* Loopback mode enable */
#define IPS_SAWB		(1<<7)				/* Source Address Write Back */
#define IPS_CKE			(1<<6)				/* Clock enable */
#define IPS_DMBS_MASK	(0x7<<3)			/* Mask for Destination DMA Maximum Burst Size */
#define IPS_DMBS_16		(0x0<<3)			/* 16 Bytes */
#define IPS_DMBS_32		(0x1<<3)			/* 32 Bytes */
#define IPS_DMBS_64		(0x2<<3)			/* 64 Bytes */
#define IPS_DMBS_128	(0x3<<3)			/* 128 Bytes */
#define IPS_SMBS_MASK	(0x7<<0)			/* Mask for SourceDMA Maximum Burst Size */
#define IPS_SMBS_16		(0x0<<0)			/* 16 Bytes */
#define IPS_SMBS_32		(0x1<<0)			/* 32 Bytes */
#define IPS_SMBS_64		(0x2<<0)			/* 64 Bytes */
#define IPS_SMBS_128	(0x3<<0)			/* 128 Bytes */

// modeCrypto
#define _MD_NOCRYPTO 			((uint32)-1)
#define _MD_CBC					(0)
#define _MD_ECB					(1<<1)
#define _MD_CTR					((1<<0)|(1<<1))
#define _MASK_CRYPTOTHENAUTH	(1<<2)
#define _MASK_CRYPTOAES			(1<<5)
#define _MASK_CRYPTODECRYPTO	(1<<2)
#define _MASK_CRYPTO3DESDES		(1<<0)
#define _MASK_CBCECBCTR			((1<<0)|(1<<1))
#define _MASK_ECBCBC			(1<<1)

// modeAuth
#define _MD_NOAUTH				((uint32)-1)
#define _MASK_AUTHSHA1MD5		(1<<0)
#define _MASK_AUTHHMAC			(1<<1)
#define _MASK_AUTH_IOPAD_READY	(1<<2)

#define MAX_PKTLEN (1<<14)

//Bit 0: 0:DES 1:3DES
//Bit 1: 0:CBC 1:ECB
//Bit 2: 0:Decrypt 1:Encrypt
#define DECRYPT_CBC_DES					0x00
#define DECRYPT_CBC_3DES				0x01
#define DECRYPT_ECB_DES					0x02
#define DECRYPT_ECB_3DES				0x03
#define ENCRYPT_CBC_DES					0x04
#define ENCRYPT_CBC_3DES				0x05
#define ENCRYPT_ECB_DES					0x06
#define ENCRYPT_ECB_3DES				0x07
#define RTL8651_CRYPTO_NON_BLOCKING		0x08
#define RTL8651_CRYPTO_GENERIC_DMA		0x10
#define DECRYPT_CBC_AES					0x20
#define DECRYPT_ECB_AES					0x22
#define DECRYPT_CTR_AES					0x23
#define ENCRYPT_CBC_AES					0x24
#define ENCRYPT_ECB_AES					0x26
#define ENCRYPT_CTR_AES					0x27

//Bit 0: 0:MD5 1:SHA1
//Bit 1: 0:Hash 1:HMAC
#define HASH_MD5		0x00
#define HASH_SHA1		0x01
#define HMAC_MD5		0x02
#define HMAC_SHA1		0x03

#define REG32(reg)				(*((volatile uint32 *)(reg)))
#define READ_MEM32(reg)			REG32(reg)
#define WRITE_MEM32(reg,val)	REG32(reg)=(val)

// address macro
#define KUSEG           0x00000000
#define KSEG0           0x80000000
#define KSEG1           0xa0000000
#define KSEG2           0xc0000000
#define KSEG3           0xe0000000

#define CPHYSADDR(a)        ((int)(a) & 0x1fffffff)
#define CKSEG0ADDR(a)       (CPHYSADDR(a) | KSEG0)
#define CKSEG1ADDR(a)       (CPHYSADDR(a) | KSEG1)
#define CKSEG2ADDR(a)       (CPHYSADDR(a) | KSEG2)
#define CKSEG3ADDR(a)       (CPHYSADDR(a) | KSEG3)

#define PHYSICAL_ADDRESS(x) CPHYSADDR(x)
#define UNCACHED_ADDRESS(x) CKSEG1ADDR(x)
#define CACHED_ADDRESS(x)	CKSEG0ADDR(x)

#define likely(x)	__builtin_expect(!!(x), 1)
#define unlikely(x)	__builtin_expect(!!(x), 0)


#if 0
#ifdef RTL_IPSEC_DEBUG
#define assert(expr) \
	do { \
		if(!(expr)) { \
			printk( "%s:%d: assert(%s)\n", \
			__FILE__,__LINE__,#expr); \
	        } \
	} while (0)
#else
#define assert(expr) do {} while (0)
#endif
#endif

/*
 *  ipsec engine supports scatter list: your data can be stored in several segments those are not continuous.
 *  Each scatter points to one segment of data.
 */
typedef struct rtl_ipsecScatter_s {
	int32 len;
	void* ptr;
} rtl_ipsecScatter_t;

enum RTL_IPSEC_OPTION
{
	RTL_IPSOPT_LBKM, /* loopback mode */
	RTL_IPSOPT_SAWB, /* Source Address Write Back */
	RTL_IPSOPT_DMBS, /* Dest Max Burst Size */
	RTL_IPSOPT_SMBS, /* Source Max Burst Size */
};

/**************************************************************************
 * Data Structure for Descriptor
 **************************************************************************/
typedef struct rtl_ipsec_source_s
{
	uint32 own:1;
	uint32 eor:1;
	uint32 fs:1;
	uint32 resv1:1;
	uint32 ms:2;
	uint32 kam:3;
	uint32 aeskl:2;
	uint32 trides:1;
	uint32 cbc:1;
	uint32 ctr:1;
	uint32 hmac:1;
	uint32 md5:1;
	uint32 resv2:2;
	uint32 sbl:14;
	uint32 resv3:8;
	uint32 a2eo:8;
	uint32 resv4:2;
	uint32 enl:14;
	uint32 resv5:8;
	uint32 apl:8;
	uint32 resv6:16;
	uint32 sdbp;
} rtl_ipsec_source_t;

typedef struct rtl_ipsec_dest_s
{
	uint32 own:1;
	uint32 eor:1;
	uint32 resv1:16;
	uint32 dbl:14;
	uint32 ddbp;
	uint32 resv2;
	uint32 icv[5];
} rtl_ipsec_dest_t;

int32 rtl_ipsecEngine_init(uint32 descNum, int8 mode32Bytes);
int32 rtl_ipsecEngine_exit(void);
int32 rtl_ipsecEngine_reset(void);
int32 rtl_ipsecEngine_descNum(void);
int32 rtl_ipsecEngine_dma_mode(void);
int32 rtl_ipsecEngine_sawb(void);

int32 rtl_ipsecEngine(uint32 modeCrypto, uint32 modeAuth, 
	uint32 cntScatter, rtl_ipsecScatter_t *scatter, void *pCryptResult,
	uint32 lenCryptoKey, void* pCryptoKey, 
	uint32 lenAuthKey, void* pAuthKey, 
	void* pIv, void* pPad, void* pDigest,
	uint32 a2eo, uint32 enl);

int32 rtl_ipsecGetOption(enum RTL_IPSEC_OPTION option, uint32* value);
int32 rtl_ipsecSetOption(enum RTL_IPSEC_OPTION option, uint32 value);

rtl_ipsec_source_t *get_rtl_ipsec_ipssdar(void);
rtl_ipsec_dest_t *get_rtl_ipsec_ipsddar(void);

void rtl_ipsec_info(void);

extern int g_rtl_ipsec_dbg;

#endif
