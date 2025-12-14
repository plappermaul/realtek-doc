#include <linux/time.h>
#include "crypto_types.h"
#include "md5.h"
#include "sha1.h"
#include "hmac.h"
#include "sw_sim/authSim.h"
#include "sw_sim/desSim.h"
#include "sw_sim/aesSim.h"
#include "sw_sim/aes.h"
#include "rtl_ipsec.h"

// MAX_AUTH_KEY is SW limited
#define MAX_AUTH_KEY 1024

// max scatter
#define MAX_SCATTER 28 // 32 - 4 (KEY / IV / HMAC_PAD / AUTH_PAD)

//#define DISABLE_SW_CHECK

static uint32 rtl_seed = 0xDeadC0de;
uint32 dbg_level = 0; 
uint32 g_failed_reset = 0;
uint32 g_seed = 0;

unsigned int get_rand_seed(void)
{
	return READ_MEM32(0xb8003108); //TC0CNT
}

uint32 rtlglue_getmstime( uint32* pTime )
{
    struct timeval tm;

#if 0
    do_gettimeofday( &tm );
    tm.tv_sec = tm.tv_sec % 86400; /* to avoid overflow for 1000times, we wrap to the seconds in a day. */
    return *pTime = ( tm.tv_sec*1000 + tm.tv_usec/1000 );
#else
	*pTime = get_timer_jiffies() * 10;
	return *pTime;
#endif
}

void rtlglue_srandom( uint32 seed )
{
    rtl_seed = seed;
}

uint32 rtlglue_random( void )
{
    uint32 hi32, lo32;

    hi32 = (rtl_seed>>16)*19;
    lo32 = (rtl_seed&0xffff)*19+37;
    hi32 = hi32^(hi32<<16);
    return ( rtl_seed = (hi32^lo32) );
}

int32 ipsecSim(uint32 modeCrypto, uint32 modeAuth,
	uint8 *data, uint8 *pCryptResult,
	uint32 lenCryptoKey, uint8 *pCryptoKey, 
	uint32 lenAuthKey, uint8 *pAuthKey, 
	uint8 *pIv, uint8 *pPad, uint8 *pDigest,
	uint32 lenA2eo, uint32 lenEnl)
{
	if ((modeAuth != (uint32) -1) &&
		(modeCrypto != (uint32) -1))
	{
		if (modeCrypto & 0x04) // encrypt then auth
		{
			if (modeCrypto & 0x20) // aes 
				aesSim_aes(modeCrypto, data + lenA2eo, pCryptResult + lenA2eo,
					lenEnl, lenCryptoKey, pCryptoKey, pIv);
			else // des
				desSim_des(modeCrypto, data + lenA2eo, pCryptResult + lenA2eo,
					lenEnl,	pCryptoKey, pIv);

			memcpy(pCryptResult, data, lenA2eo);

			authSim(modeAuth, pCryptResult, lenA2eo + lenEnl,
				pAuthKey, lenAuthKey, pDigest);
		}
		else // auth then decrypt
		{
			authSim(modeAuth, data, lenA2eo + lenEnl,
				pAuthKey, lenAuthKey, pDigest);
		
			if (modeCrypto & 0x20) // aes
				aesSim_aes(modeCrypto, data + lenA2eo, pCryptResult + lenA2eo,
					lenEnl, lenCryptoKey, pCryptoKey, pIv);
			else // des
				desSim_des(modeCrypto, data + lenA2eo, pCryptResult + lenA2eo,
					lenEnl,	pCryptoKey, pIv);

			memcpy(pCryptResult, data, lenA2eo);
		}
	}
	else if (modeAuth != (uint32) -1) // auth only
	{
		authSim(modeAuth, data, lenA2eo + lenEnl,
			pAuthKey, lenAuthKey, pDigest);
	}
	else if (modeCrypto != (uint32) -1) // crypto only
	{
		if (modeCrypto & 0x20) // aes
			aesSim_aes(modeCrypto, data + lenA2eo, pCryptResult + lenA2eo,
				lenEnl, lenCryptoKey, pCryptoKey, pIv);
		else // des
			desSim_des(modeCrypto, data + lenA2eo, pCryptResult + lenA2eo,
				lenEnl,	pCryptoKey, pIv);
	}
	else
	{
		return FAILED;
	}

	return SUCCESS;
}

static int8 *cryptoModeString[] = {
	"CBC_DES",
	"CBC_3DES",
	"ECB_DES",
	"ECB_3DES",
	"CBC_AES",
	"NONE",
	"ECB_AES",
	"CTR_AES"
};

static int8 *authModeString[] = {
	"HASH_MD5",
	"HASH_SHA1",
	"HMAC_MD5",
	"HMAC_SHA1"
};

char *mixModeString(int32 cryptoIdx, int32 authIdx)
{
	int len;
	static char modestr[128];

	len = 0;
	memset(modestr, 0, sizeof(modestr));

	if (cryptoIdx == -1) // auth only
	{
		if (authIdx >= 0 && authIdx <= 3)
			len += sprintf(&modestr[len], "%s", authModeString[authIdx]);
	}
	else if (authIdx == -1) // encrypt/decrypt only
	{
		if (cryptoIdx >= 0x0 && cryptoIdx <= 0x3)
			len += sprintf(&modestr[len], "DECRYPT_%s", cryptoModeString[cryptoIdx]);
		else if (cryptoIdx >= 0x4 && cryptoIdx <= 0x7)
			len += sprintf(&modestr[len], "ENCRYPT_%s", cryptoModeString[cryptoIdx - 0x4]);
		else if (cryptoIdx >= 0x20 && cryptoIdx <= 0x23)
			len += sprintf(&modestr[len], "DECRYPT_%s", cryptoModeString[cryptoIdx - 0x20 + 4]);
		else if (cryptoIdx >= 0x24 && cryptoIdx <= 0x27)
			len += sprintf(&modestr[len], "ENCRYPT_%s", cryptoModeString[cryptoIdx - 0x24 + 4]);
	}
	else if (cryptoIdx & 0x4) // encode first, then auth
	{
		if (cryptoIdx >= 0x4 && cryptoIdx <= 0x7)
			len += sprintf(&modestr[len], "ENCRYPT_%s", cryptoModeString[cryptoIdx - 0x4]);
		else if (cryptoIdx >= 0x24 && cryptoIdx <= 0x27)
			len += sprintf(&modestr[len], "ENCRYPT_%s", cryptoModeString[cryptoIdx - 0x24 + 4]);

		if (authIdx >= 0 && authIdx <= 3)
			len += sprintf(&modestr[len], ", then %s", authModeString[authIdx]);
	}
	else // auth first, then decode
	{
		if (authIdx >= 0 && authIdx <= 3)
			len += sprintf(&modestr[len], "%s", authModeString[authIdx]);

		if (cryptoIdx >= 0x0 && cryptoIdx <= 0x3)
			len += sprintf(&modestr[len], ", then DECRYPT_%s", cryptoModeString[cryptoIdx]);
		else if (cryptoIdx >= 0x20 && cryptoIdx <= 0x23)
			len += sprintf(&modestr[len], ", then DECRYPT_%s", cryptoModeString[cryptoIdx - 0x20 + 4]);
	}

	return modestr;
}

char *scatter_show(uint32 cntScatter, rtl_ipsecScatter_t *scatter)
{
	static char scatter_str[256];
	int i, ilen;

	ilen = 0;
	for (i=0; i<cntScatter; i++)
	{
		if (i == cntScatter - 1)
			ilen += sprintf(&scatter_str[ilen], "%d", scatter[i].len);
		else
			ilen += sprintf(&scatter_str[ilen], "%d ", scatter[i].len);
	}
	return scatter_str;
}

/********************************************************************
 *          input
 *         |     |
 *         v     v
 *       SWenc HWenc
 *         |     |
 *         Compare
 *         |     |
 *         v     v
 *       SWdec HWdec
 *         |     |
 *         Compare
 *
 * notes: total length (in scatters) = lenA2eo + lenEnl
 ********************************************************************/
static int32 ipsec_GeneralApiTestItem(uint32 modeCrypto, uint32 modeAuth,
	uint32 cntScatter, rtl_ipsecScatter_t *scatter, void *pCryptResult,
	uint32 lenCryptoKey, void *pCryptoKey, 
	uint32 lenAuthKey, void *pAuthKey, 
	void *pIv, void *pPad, void *pDigest,
	uint32 lenA2eo, uint32 lenEnl)
{
	int i;
	int32 retval;
	uint8 *cur;
	uint32 data_len;
	static uint8 sw_orig[MAX_PKTLEN];
	static uint8 sw_enc[MAX_PKTLEN];
	static uint8 sw_dec[MAX_PKTLEN];
	static uint8 sw_digest[SHA_DIGEST_LENGTH];
	void *pDecryptoKey;
	uint32 sawb;

	sawb = rtl_ipsecEngine_sawb();
	if (!sawb && pCryptResult == NULL)
	{
		printk("no pCryptResult && no sawb!\n");
		return FAILED;
	}

	if (modeAuth == (uint32)-1 && modeCrypto == (uint32)-1)
	{
		// do nothing, skip it
		return 1;
	}

	// -1:NONE
	// 0:HASH_MD5,1:HASH_SHA1,2:HMAC_MD5,3:HMAC_SHA1
	if (modeAuth != (uint32)-1 && modeAuth > 3)
	{
		// auth type check failed
		return 2;
	}

	// -1:NONE
	// 0x00:CBC_DES,0x01:CBC_3DES,0x02:ECB_DES,0x03:ECB_3DES
	// 0x20:CBC_AES,0x22:ECB_AES,0x23:CTR_AES
	if (!(modeCrypto == (uint32)-1 ||
		modeCrypto==0x00 ||
		modeCrypto==0x01 ||
		modeCrypto==0x02 ||
		modeCrypto==0x03 ||
		modeCrypto==0x20 ||
		modeCrypto==0x22 ||
		modeCrypto==0x23))
	{
		// crypto type check failed
		return 3;
	}

	// 0,2: DES_CBC or DES_ECB
	// 1,3: 3DES_CBC or 3DES_ECB 
	// 20,22,23: AES_CBC or AES_ECB or AES_CTR
	if (((modeCrypto == 0 || modeCrypto == 2) && lenCryptoKey != 8) ||
		((modeCrypto == 1 || modeCrypto == 3) && lenCryptoKey != 24) ||
		((modeCrypto == 0x20 || modeCrypto == 0x22 || modeCrypto == 0x23) &&
			(lenCryptoKey < 16 || lenCryptoKey > 32)))
	{
		// crypto length check failed
		return 4;
	}

	// mix mode check
	if (modeCrypto != (uint32)-1 && modeAuth >= 0 && modeAuth <= 1)
	{
		// crypto + hash is not support
		return 5;
	}

	// enl length check
	if (modeCrypto != (uint32)-1)
	{
		if (modeCrypto & 0x20) /* AES, ENL is the times of 16 bytes. */
		{
			if (lenEnl & 0xf)
				return 6;
		}
		else /* DES, ENL is the times of 8 bytes. */
		{
			if (lenEnl & 0x7)
				return 7;
		}

		if ((int) lenEnl <= 0)
			return 8;
	}

	// a2eo check
	if (lenA2eo)
	{
		if ( lenA2eo & 3 )
		{
			/* A2EO must be 4-byte times */
			return 9;
		}
		else if (modeAuth == (uint32)-1)
		{
			/* A2EO not support if no AUTH enable */
			return 10;
		}
	}

	if (cntScatter == 0) // pkt len = 0
		return 11;

	// Mix Mode issue
	if (modeAuth != (uint32)-1 && modeCrypto != (uint32)-1)
	{
		int apl, authPadSpace;

		// lenA2eo limit
		if ((cntScatter == 1) ||
			(!rtl_ipsecEngine_sawb()))
		{
			int max_lenA2eo;

			switch (rtl_ipsecEngine_dma_mode())
			{
			case 0: // 16 bytes
				max_lenA2eo = 156 - 4;
				break;
			case 1: // 32 bytes
				max_lenA2eo = 172 - 4; // 172 = 156 + 16
				break;
			case 2: // 64 bytes
			case 3:
				max_lenA2eo = 204 - 4; // 204 = 172 + 32
				break;
			default:
				printk("unknown dma mode (%d)!\n", rtl_ipsecEngine_dma_mode());
				return FAILED;
			}

			// 201303: A2EO limit depends on DMA burst: it cause encryption failed
			if (lenA2eo > max_lenA2eo) 
				return 12;
		}
		else // multi-desc && use sawb
		{
			// 201303: A2EO limit if Multi-Desc & SAWB: it cause encryption failed
			if (lenA2eo > 136) // 136 = 152 - 16
				return 13;
		}

		// 201303: APL lenEnl limit -> encryption ok but hmac failed
		authPadSpace = 64 - ((lenA2eo + lenEnl) & 0x3f);
		apl = authPadSpace > 8 ? authPadSpace : authPadSpace + 64;
		if (apl == 68 && lenEnl < 64)
			return 14; 
		
		// 201303: Multi-Desc lenEnl limit -> 'sometimes' timeout if multi-desc
		if ((cntScatter > 1) && (lenEnl <= 48))
			return 15;
	}

	// 20121009: scatter >= 8 will be failed
	// 2013.3 scatter >= 8 issue is SAWB related, it may:
	// 	- IPS_SDLEIP or IPS_DABFI error: can be recover via re-init
	// 	- own bit failed: need hw reset
	if (cntScatter >= 8 && rtl_ipsecEngine_sawb())
		return 16;

	/*---------------------------------------------------------------------
	 *                                 SW ENC/Auth 
	 *---------------------------------------------------------------------*/

	// scatter to continuous buffer for ipsecSim
	data_len  = 0;
	for (i = 0; i < cntScatter; i++)
	{
		if (scatter[i].ptr == NULL)
		{
			printk("%s():%d Invalid scatter pointer: %p\n",
				__FUNCTION__, __LINE__, scatter[i].ptr);
			return FAILED;
		}

		memcpy(&sw_orig[data_len], scatter[i].ptr, scatter[i].len);
		data_len += scatter[i].len;
	}

#ifndef DISABLE_SW_CHECK
	retval = ipsecSim(modeCrypto|4/*enc*/, modeAuth, sw_orig, sw_enc,
		lenCryptoKey, pCryptoKey, lenAuthKey, pAuthKey,
		pIv, pPad, sw_digest, lenA2eo, lenEnl);

	if (retval != SUCCESS)
	{
		return FAILED; // IPSec Simulator Engine unable to encrypt is mandatory testing failed
	}
#endif

	/*---------------------------------------------------------------------
	 *                                 ASIC ENC/Auth 
	 *---------------------------------------------------------------------*/
	retval = rtl_ipsecEngine(modeCrypto|4/*enc*/, modeAuth,
		cntScatter, scatter, pCryptResult,
		lenCryptoKey, pCryptoKey, lenAuthKey, pAuthKey,
		pIv, pPad, pDigest, lenA2eo, lenEnl);

	if (retval != SUCCESS)
	{
		return FAILED; // IPSec Engine unable to encrypt is mandatory testing failed
	}
	
	/*---------------------------------------------------------------------
	 *                                 Compare ENC/Auth
	 *---------------------------------------------------------------------*/
#ifndef DISABLE_SW_CHECK
	if (modeCrypto != (uint32) -1)
	{
		if (sawb) // use sawb, ASIC write back to scatter buffer
		{
			cur = sw_enc;
			for (i = 0; i < cntScatter; i++)
			{
				if (memcmp(cur, scatter[i].ptr, scatter[i].len) != 0)
				{
					printk("ENC DIFF![%d]crypt:%x auth:%x CKey:%d AKey:%d lenA2eo:%d\n",
						i, modeCrypto, modeAuth, lenCryptoKey, lenAuthKey, lenA2eo);

					if (g_failed_reset)
					{
						// no need more debug if auto reset
						return FAILED;
					}
					else
					{
						memDump(cur, scatter[i].len, "Software");
						memDump(scatter[i].ptr, scatter[i].len, "ASIC");
						return FAILED;
					}
				}
				cur += scatter[i].len;
			}
		}
		else // not use sawb, ASIC write to continuous buffer pCryptResult
		{
			if (memcmp(sw_enc + lenA2eo, pCryptResult + lenA2eo, lenEnl) != 0)
			{
				printk("ENC DIFF![%d]crypt:%x auth:%x CKey:%d AKey:%d lenA2eo:%d\n",
					i, modeCrypto, modeAuth, lenCryptoKey, lenAuthKey, lenA2eo);

				if (g_failed_reset)
				{
					// no need more debug if auto reset
					return FAILED;
				}
				else
				{
					memDump(sw_enc + lenA2eo, lenEnl, "Software");
					memDump(pCryptResult + lenA2eo, lenEnl, "ASIC");
					return FAILED;
				}
			}
		}
	}

	if (modeAuth != (uint32) -1)
	{
		if (memcmp(sw_digest, pDigest, modeAuth&1?20/*SHA1*/:16/*MD5*/ ) != 0) 
		{
			printk("ENC DIGEST! crypt:%x auth:%x CKey:%d AKey:%d lenA2eo:%d\n",
				modeCrypto, modeAuth, lenCryptoKey, lenAuthKey, lenA2eo);

			if (g_failed_reset)
			{
				// no need more debug if auto reset
				return FAILED;
			}
			else
			{
				memDump(sw_digest, modeAuth&1?20/*SHA1*/:16/*MD5*/, "Software");
				memDump(pDigest, modeAuth&1?20/*SHA1*/:16/*MD5*/, "ASIC");

				memDump(sw_enc, lenA2eo + lenEnl, "sw_enc");
				memDump(pCryptResult, lenA2eo + lenEnl, "asic_enc");
				memDump(pAuthKey, lenAuthKey, "auth key");
				return FAILED;
			}
		}
	}
#endif

	/* Since no crypto employed, no need to hash again. */
	if (modeCrypto == (uint32) -1)
		return SUCCESS;

	/* AES_CTR mode need not verify. */
	if (modeCrypto == 0x23/*AES_CTR*/)
		return SUCCESS;

	/*---------------------------------------------------------------------
	 *                                 SW Auth/DEC
	 *---------------------------------------------------------------------*/
#ifndef DISABLE_SW_CHECK
	retval = ipsecSim(modeCrypto/*dec*/, modeAuth, sw_enc, sw_dec,
		lenCryptoKey, pCryptoKey, lenAuthKey, pAuthKey,
		pIv, pPad, sw_digest, lenA2eo, lenEnl);

	if (retval != SUCCESS)
	{
		return FAILED; // IPSec Simulator Engine unable to decrypt is mandatory testing failed
	}
#endif

	/*---------------------------------------------------------------------
	 *                                 ASIC Auth/DEC
	 *---------------------------------------------------------------------*/
	// create de-key for ASIC
	if (modeCrypto == 0x20/*AES_CBC*/ || modeCrypto == 0x22/*AES_ECB*/)
	{
		AES_KEY aes_key;
		static uint8 __key_sch[32 + 256/8 + 32];
		static uint8 *key_sch = &__key_sch[32];

		/* IC accept the de-key in reverse order. */
		AES_set_encrypt_key(pCryptoKey, lenCryptoKey*8, &aes_key);
		switch (lenCryptoKey)
		{
			case 128/8:
				memcpy((void *) UNCACHED_ADDRESS(key_sch), &aes_key.rd_key[4*10], 16);
				break;
			case 192/8:
				memcpy((void *) UNCACHED_ADDRESS(key_sch), &aes_key.rd_key[4*12], 16);
				memcpy((void *) UNCACHED_ADDRESS(&key_sch[16]), &aes_key.rd_key[4*11+2], 8);
				break;
			case 256/8:
				memcpy((void *) UNCACHED_ADDRESS(key_sch), &aes_key.rd_key[4*14], 16);
				memcpy((void *) UNCACHED_ADDRESS(&key_sch[16]), &aes_key.rd_key[4*13], 16);
				break;
			default:
				printk("%s():%d unknown lenCryptoKey = %d\n",
					__FUNCTION__, __LINE__, lenCryptoKey);
				return FAILED;
		}
		pDecryptoKey = (void *) UNCACHED_ADDRESS(key_sch);
	}
	else
	{
		pDecryptoKey = pCryptoKey;
	}

	// if sawb disabled,
	// create scatter buffer from continuous software buffer
	if (!sawb) 
	{
		cur = pCryptResult;
		for (i = 0; i < cntScatter; i++)
		{
			memcpy(scatter[i].ptr, cur, scatter[i].len);
			cur += scatter[i].len;
		}
	}

	retval = rtl_ipsecEngine(modeCrypto/*dec*/, modeAuth,
		cntScatter, scatter, pCryptResult,
		lenCryptoKey, pDecryptoKey, lenAuthKey, pAuthKey,
		pIv, pPad, pDigest, lenA2eo, lenEnl);

	if (retval != SUCCESS)
	{
		return FAILED; // IPSec Engine unable to encrypt is mandatory testing failed
	}
	
	/*---------------------------------------------------------------------
	 *                                 Compare Auth/DEC
	 *---------------------------------------------------------------------*/
#ifdef DISABLE_SW_CHECK
	if (modeAuth != (uint32) -1)
	{
		if (sawb) // use sawb, ASIC write back to scatter buffer
		{
			cur = sw_orig; // sw_orig backup orig asic input
			for (i = 0; i < cntScatter; i++)
			{
				if (memcmp(cur, scatter[i].ptr, scatter[i].len) != 0)
				{
					printk("DEC DIFF![%d]crypt:%x auth:%x CKey:%d AKey:%d lenA2eo:%d\n",
						i, modeCrypto, modeAuth, lenCryptoKey, lenAuthKey, lenA2eo);
					return FAILED;
				}
				cur += scatter[i].len;
			}
		}
		else // not use sawb, ASIC write to continuous buffer pCryptResult
		{
			if (memcmp(sw_orig + lenA2eo, pCryptResult + lenA2eo, lenEnl) != 0)
			{
				printk("DEC DIFF! crypt:%x auth:%x CKey:%d AKey:%d lenA2eo:%d\n",
					modeCrypto, modeAuth, lenCryptoKey, lenAuthKey, lenA2eo);
				return FAILED;
			}
		}
	}
#else
	if (modeAuth != (uint32) -1)
	{
		if (memcmp(sw_digest, pDigest, modeAuth&1?20/*SHA1*/:16/*MD5*/) != 0)
		{
			printk("DEC DIGEST! crypt:%x auth:%x CKey:%d AKey:%d lenA2eo:%d\n",
				modeCrypto, modeAuth, lenCryptoKey, lenAuthKey, lenA2eo);

			if (g_failed_reset)
			{
				// no need more debug if auto reset
				return FAILED;
			}
			else
			{
				memDump(sw_digest, modeAuth&1?20/*SHA1*/:16/*MD5*/, "Orig");
				memDump(pDigest, modeAuth&1?20/*SHA1*/:16/*MD5*/, "Software");
				return FAILED;
			}
		}
	}

	if (modeCrypto != (uint32) -1)
	{
		if (memcmp(sw_orig + lenA2eo, sw_dec + lenA2eo, lenEnl) != 0)
		{
			printk("DEC DIFF! crypt:%x auth:%x CKey:%d AKey:%d lenA2eo:%d\n",
				modeCrypto, modeAuth, lenCryptoKey, lenAuthKey, lenA2eo);

			if (g_failed_reset)
			{
				// no need more debug if auto reset
				return FAILED;
			}
			else
			{
				memDump(sw_orig + lenA2eo, lenEnl, "Orig");
				memDump(sw_dec + lenA2eo, lenEnl, "Software");
				return FAILED;
			}
		}

		if (sawb) // use sawb, ASIC write back to scatter buffer
		{
			cur = sw_dec;
			for (i = 0; i < cntScatter; i++)
			{
				if (memcmp(cur, scatter[i].ptr, scatter[i].len) != 0)
				{
					printk("DEC DIFF![%d]crypt:%x auth:%x CKey:%d AKey:%d lenA2eo:%d\n",
						i, modeCrypto, modeAuth, lenCryptoKey, lenAuthKey, lenA2eo);
			
					if (g_failed_reset)
					{
						// no need more debug if auto reset
						return FAILED;
					}
					else
					{
						memDump(cur, scatter[i].len, "Software");
						memDump(scatter[i].ptr, scatter[i].len, "ASIC");
						return FAILED;
					}
				}
				cur += scatter[i].len;
			}
		}
		else // not use sawb, ASIC write to continuous buffer pCryptResult
		{
			if (memcmp(sw_dec + lenA2eo, pCryptResult + lenA2eo, lenEnl) != 0)
			{
				printk("DEC DIFF! crypt:%x auth:%x CKey:%d AKey:%d lenA2eo:%d\n",
					modeCrypto, modeAuth, lenCryptoKey, lenAuthKey, lenA2eo);

				if (g_failed_reset)
				{
					// no need more debug if auto reset
					return FAILED;
				}
				else
				{
					memDump(sw_dec + lenA2eo, lenEnl, "Software");
					memDump(pCryptResult + lenA2eo, lenEnl, "ASIC");
					return FAILED;
				}
			}
		}
	}
#endif

	return SUCCESS;
}

int32 cryptoThroughput(int32 asic, int32 round, int32 cryptoStart, int32 cryptoEnd, int32 authStart, int32 authEnd,
	int32 CryptoKeyLen, int32 AuthKeyLen, int32 pktLen, int32 a2eoLen, int32 maxScatter)
{
	uint32 testRound, i, bps;
	uint32 sTime, eTime;
	int32 cryptoIdx, authIdx;
	uint8 *cur;
	int32 lenEnl;
	uint32 cntScatter = 0, len_unused;
	rtl_ipsecScatter_t src[MAX_SCATTER];
	static uint8 bufAsic[32+MAX_PKTLEN+32];
	static uint8 _cryptoKey[32+32+32];
	static uint8 _authKey[32+MAX_AUTH_KEY+32];
	static uint8 _iv[32+16+32]; /* 256-bits */
	static uint8 _pad[32+128+32]; /* ipad + opad */
	static uint8 _AsicDigest[32+20+32]; /* max digest length */
	static uint8 _AsicCrypt[32+MAX_PKTLEN+32];
	uint8 *pCryptoKey = (void *) UNCACHED_ADDRESS( &_cryptoKey[32] );
	uint8 *pAuthKey = (void *) UNCACHED_ADDRESS( &_authKey[32] );
	uint8 *pIv = (void *) UNCACHED_ADDRESS( &_iv[32] );
	uint8 *pPad = (void *) UNCACHED_ADDRESS( &_pad[32] );
	uint8 *pAsicDigest = &_AsicDigest[32];
	uint8 *pAsicCrypt = (void *) UNCACHED_ADDRESS(&_AsicCrypt[32]);
	static uint8 sw_orig[MAX_PKTLEN];
	static uint8 sw_enc[MAX_PKTLEN];
	static uint8 sw_digest[SHA_DIGEST_LENGTH];
	
	rtlglue_printf("Evaluate 8651b throughput(round=%d,cryptoStart=%d,cryptoEnd=%d,authStart=%d,authEnd=%d,"
		"CryptoKeyLen=%d,AuthKeyLen=%d,pktLen=%d,a2eoLen=%d)\n", round, cryptoStart, cryptoEnd, authStart, authEnd,
		CryptoKeyLen, AuthKeyLen, pktLen, a2eoLen);

	/*  <--lenValid-->
	 *  | A2EO | ENL | APL
	 */
	lenEnl = pktLen - a2eoLen;

	if ( lenEnl <= 0 )
	{
		rtlglue_printf( "[IGNORE,lenEnl<=0]\n" );
		return FAILED;
	}

	/* Simple pattern for debug */
	/* build keys and iv */
	for( i = 0; i < CryptoKeyLen; i++ )
		pCryptoKey[i] = i;
	for( i = 0; i < AuthKeyLen; i++ )
		pAuthKey[i] = i;
	for( i = 0; i < 16; i++ )
		pIv[i] = i;
	for( i = 0; i < 128; i++ )
		pPad[i] = i;

	/* fill the test data */
	cur = (void *) UNCACHED_ADDRESS( &bufAsic[0+32] );
	for( i = 0; i < pktLen; i++ )
	{
		cur[i] = 0xff;
	}

	memcpy(sw_orig, cur, pktLen);

//	rtlglue_srandom(g_seed++);
	rtlglue_srandom(get_rand_seed());

	len_unused = pktLen;
	for (cntScatter=0; cntScatter<maxScatter && len_unused > 0; cntScatter++)
	{
		if (cntScatter == maxScatter - 1) // last one?
		{
			src[cntScatter].len = len_unused;
			src[cntScatter].ptr = cur;
		}
		else
		{
			src[cntScatter].len = (rtlglue_random() % len_unused) + 1;
			src[cntScatter].ptr = cur;
			cur += src[cntScatter].len;
			len_unused -= src[cntScatter].len;
		}
	}

	for (cryptoIdx=cryptoStart; cryptoIdx<=cryptoEnd; cryptoIdx++) 
	{
		if ( cryptoIdx==-1 || // none
		     cryptoIdx==0x00 || cryptoIdx==0x01 || cryptoIdx==0x02 || cryptoIdx==0x03 || // des/3des decode
		     cryptoIdx==0x04 || cryptoIdx==0x05 || cryptoIdx==0x06 || cryptoIdx==0x07 || // des/3des encode
		     cryptoIdx==0x20 || cryptoIdx==0x22 || cryptoIdx==0x23 || // aes decode
		     cryptoIdx==0x24 || cryptoIdx==0x26 || cryptoIdx==0x27) // aes encode
		{
			/* valid mode, go on */
		}
		else
			continue;

		if ( cryptoIdx!=(uint32)-1 )
		{
			if ( cryptoIdx&0x20 )
			{ /* AES, ENL is the times of 16 bytes. */
				if ( lenEnl&0xf )
				{
					rtlglue_printf( "[IGNORE,ENL=0x%04x]\n", lenEnl );
					return FAILED;
				}
			}
			else
			{ /* DES, ENL is the times of 8 bytes. */
				if ( lenEnl&0x7 )
				{
					rtlglue_printf( "[IGNORE,ENL=0x%04x]\n", lenEnl );
					return FAILED;
				}
			}
		}

		if (cryptoIdx == 0x00 || cryptoIdx == 0x02 || cryptoIdx == 0x04 || cryptoIdx == 0x06)
			CryptoKeyLen = 8; // DES
		else if (cryptoIdx == 0x01 || cryptoIdx == 0x03 || cryptoIdx == 0x05 || cryptoIdx == 0x07)
			CryptoKeyLen = 24; // 3DES

		for (authIdx=authStart; authIdx<=authEnd; authIdx++) 
		{
#if 0				
			if ( authIdx!=(uint32)-1 )
			{
				if ( pktLen&3 )
				{
					/* Since A2EO and APL must be 4-byte times (ENL is 8/16-byte), lenValid must be 4-byte times.
					 * Otherwise, APL will be non-4-byte times. */
					rtlglue_printf( "[IGNORE,Valid=0x%04x]\n", pktLen );
					return FAILED;
				}
			}
#endif

			switch(asic)
			{
			case 0:
				rtlglue_getmstime(&sTime);
				for(testRound=0; testRound<=round; testRound++) 
				{
					if (ipsecSim(cryptoIdx|4/*enc*/, authIdx, sw_orig, sw_enc,
						CryptoKeyLen, pCryptoKey, AuthKeyLen, pAuthKey,
						pIv, pPad, sw_digest, a2eoLen, lenEnl) != SUCCESS)
					{
						rtlglue_printf("testRound=%d, rtl8651x_ipsecEngine(modeIdx=%d,authIdx=%d) failed... pktlen=%d\n",
							testRound, cryptoIdx, authIdx, pktLen);
						return FAILED;
					}
				}
				rtlglue_getmstime(&eTime);
				break;
			case 1:
				rtlglue_getmstime(&sTime);
				for(testRound=0; testRound<=round; testRound++) 
				{
					if (rtl_ipsecEngine(cryptoIdx, authIdx, cntScatter, src, pAsicCrypt,
						CryptoKeyLen, pCryptoKey, AuthKeyLen, pAuthKey, pIv, pPad, pAsicDigest, 
						a2eoLen, lenEnl) != SUCCESS)
					{
						rtlglue_printf("testRound=%d, rtl8651x_ipsecEngine(modeIdx=%d,authIdx=%d) failed... pktlen=%d\n",
							testRound, cryptoIdx, authIdx, pktLen);
						return FAILED;
					}
				}
				rtlglue_getmstime(&eTime);
				break;
			case 2:
				rtlglue_getmstime(&sTime);
				for(testRound=0; testRound<=round; testRound++) 
				{
					if (ipsec_GeneralApiTestItem(cryptoIdx, authIdx, cntScatter, src, pAsicCrypt,
							CryptoKeyLen, pCryptoKey, AuthKeyLen, pAuthKey, pIv, NULL/*pad*/, pAsicDigest,
							a2eoLen, lenEnl) != SUCCESS)
					{
						rtlglue_printf("testRound=%d, rtl8651x_ipsecEngine(modeIdx=%d,authIdx=%d) failed... pktlen=%d\n",
							testRound, cryptoIdx, authIdx, pktLen);
						return FAILED;
					}
				}
				rtlglue_getmstime(&eTime);
				break;
			}

			if ( eTime - sTime == 0 )
			{
				rtlglue_printf("round is too small to measure throughput, try larger round number!\n");
				return FAILED;
			}
			else
			{
				bps = pktLen*8*1000/((uint32)(eTime - sTime))*round;
				if(bps>1000000)
					rtlglue_printf("%s round %u len %u time %u throughput %u.%02u mbps\n",
						mixModeString(cryptoIdx, authIdx),
						round, pktLen, (uint32)(eTime - sTime),
						bps/1000000, (bps%1000000)/10000);
				else if(bps>1000)
					rtlglue_printf("%s round %u len %u time %u throughput %u.%02u kbps\n",
						mixModeString(cryptoIdx, authIdx),
						round, pktLen, (uint32)(eTime - sTime),
						bps/1000, (bps%1000)/10);
				else
					rtlglue_printf("%s round %u len %u time %u throughput %u bps\n",
						mixModeString(cryptoIdx, authIdx),
						round, pktLen, (uint32)(eTime - sTime),
						bps);			
			}
		}
	}

	return SUCCESS;
}

int32 ipsec_GeneralApiTest(uint32 round,
	int32 lenStart, int32 lenEnd,
	int32 offsetStart, int32 offsetEnd,
	int32 cryptoStart, int32 cryptoEnd,
	int32 authStart, int32 authEnd,
	int32 cryptoKeyStart, int32 cryptoKeyEnd,
	int32 authKeyStart, int32 authKeyEnd,
	int32 a2eoStart, int32 a2eoEnd,
	int32 maxScatter)
{
	int i;
	int32 roundIdx;
	int32 pktLen, offset;
	int32 modeCrypto, modeAuth;
	int32 lenCryptoKey, lenAuthKey;
	int32 lenA2eo;
	int32 ret = SUCCESS;
	static uint8 _cryptoKey[32 + 32 + 32]; // 32 for AES-256
	static uint8 _cryptoIv[32 + 16 + 32]; // 16 for AES
	static uint8 _authKey[32 + MAX_AUTH_KEY + 32];
	static uint8 _pad[32 + 128 + 32]; // 128 for ipad and opad 
	static uint8 _asic_orig[32 + MAX_PKTLEN + 8 + 32]; // +8 for max offset
	static uint8 _asic_enc[32 + MAX_PKTLEN + 32];
	static uint8 _asic_digest[32 + SHA_DIGEST_LENGTH + 32]; // 20 for SHA1
	static uint8 *cryptoKey, *cryptoIv, *authKey, *pad;
	static uint8 *asic_orig, *asic_enc, *asic_digest;
	rtl_ipsecScatter_t scatter[MAX_SCATTER];
	uint32 len_unused;
	uint8 *asic_buf;
	int cntScatter;
	int cnt_success, cnt_failed, cnt_ignore, cnt_scatters;

	cryptoKey = (void *) UNCACHED_ADDRESS(&_cryptoKey[32]);
	cryptoIv = (void *) UNCACHED_ADDRESS(&_cryptoIv[32]);
	authKey = (void *) UNCACHED_ADDRESS(&_authKey[32]);
	pad = (void *) UNCACHED_ADDRESS(&_pad[32]);
	asic_orig = (void *) UNCACHED_ADDRESS(&_asic_orig[32]);
	asic_enc = (void *) UNCACHED_ADDRESS(&_asic_enc[32]);
	asic_digest = (void *) UNCACHED_ADDRESS(&_asic_digest[32]);

	if (maxScatter < 1)
		maxScatter = 1; // have to 1 at least
	else if (maxScatter >= MAX_SCATTER)
		maxScatter = MAX_SCATTER;

	for(i=0; i<24; i++)
		cryptoKey[i] = 0x01;

	for(i=0; i<8; i++)
		cryptoIv[i] = 0x01;

	cnt_success = 0;
	cnt_failed = 0;
	cnt_ignore = 0;
	cnt_scatters = 0;
	for (roundIdx=0; roundIdx<round; roundIdx++)
	{
//		rtlglue_srandom(g_seed++);
		rtlglue_srandom(get_rand_seed());
		for (pktLen=lenStart; pktLen<=lenEnd; pktLen+=4) 
		{
			for (offset=offsetStart; offset<=offsetEnd; offset++) 
			{
				for (i=0; i<pktLen; i++)
					asic_orig[i + offset] = (i & 0xFF);

				len_unused = pktLen;
				asic_buf = &asic_orig[offset];
				for (cntScatter=0; cntScatter<maxScatter && len_unused > 0; cntScatter++)
				{
					if (cntScatter == maxScatter - 1) // last one?
					{
						scatter[cntScatter].len = len_unused;
						scatter[cntScatter].ptr = asic_buf;
					}
					else
					{
						scatter[cntScatter].len = (rtlglue_random() % len_unused) + 1;
						scatter[cntScatter].ptr = asic_buf;
						asic_buf += scatter[cntScatter].len;
						len_unused -= scatter[cntScatter].len;
					}
				}

				modeCrypto = cryptoStart;
				while (modeCrypto <= cryptoEnd)
				{
					for (modeAuth=authStart; modeAuth<=authEnd; modeAuth++)
					{
						for (lenCryptoKey=cryptoKeyStart; lenCryptoKey<=cryptoKeyEnd; lenCryptoKey+=8)
						{
							for (lenAuthKey=authKeyStart; lenAuthKey<=authKeyEnd && lenAuthKey < MAX_AUTH_KEY;
									lenAuthKey+=4)
							{
								for (i=0; i<lenAuthKey; i++)
									authKey[i] = (i & 0xff);

								for (lenA2eo=a2eoStart; lenA2eo<=a2eoEnd; lenA2eo+=4)
								{
									ret = ipsec_GeneralApiTestItem(modeCrypto, modeAuth,
										cntScatter, scatter, asic_enc,
										lenCryptoKey, cryptoKey,
										lenAuthKey, authKey,
										cryptoIv, NULL/*pad*/, asic_digest,
										lenA2eo, pktLen - lenA2eo);

									if (ret == FAILED)
									{
										printk("SAWB:%d DMA:%d Len:%d Off:%d Enc:0x%x Auth:0x%x CK:%d AK:%d A2EO:%d DATA:%d (%s) [FAILED]\n", rtl_ipsecEngine_sawb(), rtl_ipsecEngine_dma_mode(),	pktLen, offset, modeCrypto, modeAuth, lenCryptoKey, lenAuthKey, lenA2eo, cntScatter, scatter_show(cntScatter, scatter));

										if (g_failed_reset)
										{
											rtl_ipsecEngine_reset();
											cnt_failed++;
										}
										else
											return FAILED;
									}
									else if (ret > 0)
									{
										if (dbg_level >= 2)
											printk("SAWB:%d DMA:%d Len:%d Off:%d Enc:0x%x Auth:0x%x CK:%d AK:%d A2EO:%d DATA:%d (%s) [SKIP %d]\n", rtl_ipsecEngine_sawb(), rtl_ipsecEngine_dma_mode(),	pktLen, offset, modeCrypto, modeAuth, lenCryptoKey, lenAuthKey, lenA2eo, cntScatter, scatter_show(cntScatter, scatter), ret);

										cnt_ignore++;
									}
									else
									{
										if (dbg_level >= 1)
											printk("SAWB:%d DMA:%d Len:%d Off:%d Enc:0x%x Auth:0x%x CK:%d AK:%d A2EO:%d DATA:%d (%s) [SUCCESS]\n", rtl_ipsecEngine_sawb(), rtl_ipsecEngine_dma_mode(),	pktLen, offset, modeCrypto, modeAuth, lenCryptoKey, lenAuthKey, lenA2eo, cntScatter, scatter_show(cntScatter, scatter));

										cnt_success++;
										cnt_scatters += cntScatter;
									}
								}

								if (modeAuth == -1) // no auth, don't care auth key len
									break;
							}

							if (modeCrypto == -1) // no crypto, don't care crypto key len
								break;
						}
					}

					if (modeCrypto < 3) // no crypto or DES
						modeCrypto++;
					else if (modeCrypto == 3) // DES end
						modeCrypto = 0x20;
					else if (modeCrypto == 0x20) // skip 0x21 
						modeCrypto = 0x22;
					else
						modeCrypto++;
				}
			}
		}
	}
				
	if (g_failed_reset)
		printk("\nGeneral API Test Finished. (%d, %d, %d) %d\n", cnt_success, cnt_failed, cnt_ignore, cnt_scatters);
	else
		printk("\nGeneral API Test Finished. %d/%d, %d\n", cnt_success, cnt_success + cnt_ignore, cnt_scatters);

	return SUCCESS;	
}

/****************************************
 * By 'crypto mix rand' command
 *   random key/iv/mode/len/offset/data
 ****************************************/
int32 ipsec_GeneralApiRandTest(uint32 seed, uint32 round,
	int32 mode_crypto, int32 mode_auth, int32 maxScatter)
{
#define LEN_CRTPTO_VALS (sizeof(modeCrypto_val)/sizeof(modeCrypto_val[0]))
#define LEN_AUTH_VALS (sizeof(modeAuth_val)/sizeof(modeAuth_val[0]))
#define LEN_CRYPTOKEY_VALS (sizeof(CryptoKey_val)/sizeof(CryptoKey_val[0]))
	int i, j, k;
	int32 roundIdx;
	int32 pktLen, offset;
	int32 modeCrypto, modeAuth;
	int32 lenCryptoKey, lenAuthKey;
	int32 lenA2eo, lenEnl;
	int32 ret = SUCCESS;
	int32 modeCrypto_val[8] = {-1, 0x00, 0x01, 0x02, 0x03, 0x20, 0x22, 0x23};
	int32 modeAuth_val[5] = {-1, 0, 1, 2, 3};
	int32 CryptoKey_val[4] = {8, 16, 24, 32};
	static uint8 _cryptoKey[32 + 32 + 32]; // 32 for AES-256
	static uint8 _cryptoIv[32 + 16 + 32]; // 16 for AES
	static uint8 _authKey[32 + MAX_AUTH_KEY + 32];
	static uint8 _pad[32 + 128 + 32]; // 128 for ipad and opad 
	static uint8 _asic_orig[32 + MAX_PKTLEN + 8 + 32]; // +8 for max offset
	static uint8 _asic_enc[32 + MAX_PKTLEN + 32];
	static uint8 _asic_digest[32 + SHA_DIGEST_LENGTH + 32]; // 20 for SHA1
	static uint8 *cryptoKey, *cryptoIv, *authKey, *pad;
	static uint8 *asic_orig, *asic_enc, *asic_digest;
	rtl_ipsecScatter_t scatter[MAX_SCATTER];
	int32 cntScatter, randScatter = 0;
	uint32 len_unused;
	uint8 *asic_buf;
	int cnt_success, cnt_failed, cnt_ignore, cnt_scatters;
	int32 stat_tests[LEN_CRTPTO_VALS][LEN_AUTH_VALS][LEN_CRYPTOKEY_VALS];
	int idxCrypto, idxAuth, idxKeyLen, cnt;

	cryptoKey = (void *) UNCACHED_ADDRESS(&_cryptoKey[32]);
	cryptoIv = (void *) UNCACHED_ADDRESS(&_cryptoIv[32]);
	authKey = (void *) UNCACHED_ADDRESS(&_authKey[32]);
	pad = (void *) UNCACHED_ADDRESS(&_pad[32]);
	asic_orig = (void *) UNCACHED_ADDRESS(&_asic_orig[32]);
	asic_enc = (void *) UNCACHED_ADDRESS(&_asic_enc[32]);
	asic_digest = (void *) UNCACHED_ADDRESS(&_asic_digest[32]);

	if (maxScatter == 0xff)
		randScatter = 1;
	else if (maxScatter < 1)
		maxScatter = 1;
	else if (maxScatter >= MAX_SCATTER)
		maxScatter = MAX_SCATTER;

	cnt_success = 0;
	cnt_failed = 0;
	cnt_ignore = 0;
	cnt_scatters = 0;

	memset(stat_tests, 0, sizeof(stat_tests));

	if (mode_crypto == 0xff) 
		; // random case
	else if (mode_crypto >= 0 && mode_crypto <= 3)
		idxCrypto = mode_crypto + 1;
	else if (mode_crypto == 0x20)
		idxCrypto = 5;
	else if (mode_crypto >= 0x22 && mode_crypto <= 0x23)
		idxCrypto = mode_crypto - 0x22 + 6;
	else 
		idxCrypto = 0; // no crypto

	if (mode_auth == 0xff)
		; // random case
	else if (mode_auth >= 0 && mode_auth <= 3)
		idxAuth = mode_auth + 1;
	else 
		idxAuth = 0; // no auth

	for (roundIdx=0; roundIdx<round; roundIdx++) 
	{
//		rtlglue_srandom(seed++);
		rtlglue_srandom(get_rand_seed());

		// SAWB mode
		i = rtlglue_random() & 1;
		if ( i != rtl_ipsecEngine_sawb() )
		{
			rtl_ipsecSetOption(RTL_IPSOPT_SAWB, i);
		}

		// DMA burst mode
		i = rtlglue_random() & 3;
		if ( i != rtl_ipsecEngine_dma_mode() )
		{
			j = rtl_ipsecEngine_descNum();
			rtl_ipsecEngine_init(j, i);
		}

		if (mode_crypto == 0xff)
			idxCrypto = rtlglue_random() % LEN_CRTPTO_VALS;

		modeCrypto = modeCrypto_val[idxCrypto];

		if (mode_auth == 0xff)
			idxAuth = rtlglue_random() % LEN_AUTH_VALS;

		modeAuth = modeAuth_val[idxAuth];

		if (modeCrypto == 0x00 || modeCrypto == 0x02)
			idxKeyLen = 0; // DES
		else if (modeCrypto == 0x01 || modeCrypto == 0x03)
			idxKeyLen = 2; // 3DES
		else
			idxKeyLen = (rtlglue_random() % 3) + 1; // AES 128,192,256

		lenCryptoKey = CryptoKey_val[idxKeyLen];

		stat_tests[idxCrypto][idxAuth][idxKeyLen]++;

		lenAuthKey = rtlglue_random() & 0x3ff; // 0~1023
		if (lenAuthKey > MAX_AUTH_KEY)
			lenAuthKey = MAX_AUTH_KEY;

		for(i=0; i<24; i++)
			cryptoKey[i] = rtlglue_random() & 0xFF;
		for(i=0; i<8; i++)
			cryptoIv[i] = rtlglue_random() & 0xFF;

		if (randScatter)
		{
			i = MAX_SCATTER < 7 ? MAX_SCATTER : 7;
			if (rtl_ipsecEngine_sawb()) // max scatter is 7 if enable sawb
				maxScatter = (rtlglue_random() % i) + 1;
			else
				maxScatter = (rtlglue_random() % MAX_SCATTER) + 1;
		}

		if (modeCrypto == -1 && modeAuth == -1)
		{
			cnt_ignore++;
			continue;
		}
		else if (modeCrypto == -1) // auth only
		{
			lenA2eo = rtlglue_random() & 0xFC; // a2eo must 4 byte aligna, 0..252

			if (roundIdx % 10 > 1) // 80% case use 1..1518
				pktLen = rtlglue_random() % 1518 + 1;
			else if (modeAuth & 0x2) // HMAC
				pktLen = rtlglue_random() % (0x3FC0 - 9 - HMAC_MAX_MD_CBLOCK) + 1; // 1..16247
			else // HASH only
				pktLen = rtlglue_random() % (0x3FC0 - 9) + 1; // 1..16311
		}
		else if (modeAuth == -1) // crypto only
		{
			lenA2eo = 0; // lenA2eo must 0 if crypto only

			if (roundIdx % 10 > 1) // 80% case use 1..1518
			{
				pktLen = rtlglue_random() % 1518 + 1;
				pktLen = pktLen & ((modeCrypto & 0x20) ? 0x3FF0 : 0x3FF8);
			}
			else if (modeCrypto & 0x20) // aes
				pktLen = rtlglue_random() & 0x3FF0; /* 16 ~ 16368, 0 will be skipped later */
			else // des
				pktLen = rtlglue_random() & 0x3FF8; /* 8 ~ 16376, 0 will be skipped later */
		} 
		else // mix mode
		{
			int max_a2eo;

			modeAuth |= 0x2; // support hmac only if mix mode

			if ((cntScatter == 1) || (!rtl_ipsecEngine_sawb()))
			{
				int dma_bytes[4] = {16, 32, 64, 64};
				max_a2eo = 152 + dma_bytes[rtl_ipsecEngine_dma_mode()] - 16;
			}
			else 
			{
				max_a2eo = 152 - 16;
			}

			lenA2eo = (rtlglue_random() % max_a2eo) & 0xFC; // a2eo must 4 byte aligna, 0..252

			if (roundIdx % 10 > 1) // 80% case use 1..1518
				lenEnl = rtlglue_random() % (1518 - lenA2eo) + 1;
			else
				lenEnl = rtlglue_random() % (0x3FC0 - 9 - HMAC_MAX_MD_CBLOCK - lenA2eo) + 1; /* 1..16247 - a2eo*/

			if (modeCrypto & 0x20)	// aes
			{
				lenEnl = lenEnl & 0x3FF0;
				if (lenEnl <= 48)
					lenEnl = 64;
			}
			else // des
			{
				lenEnl = lenEnl & 0x3FF8;
				if (lenEnl <= 48)
					lenEnl = 56;
			}

			pktLen = lenEnl + lenA2eo;
		}

		offset = rtlglue_random() & 0x7;

		for(i=0; i<pktLen; i++) //8-byte for IV and 4-byte for DMA_addr (0-3) byte offset test
			asic_orig[i+offset] = rtlglue_random() & 0xFF;

		len_unused = pktLen;
		asic_buf = &asic_orig[offset];
		for (cntScatter=0; cntScatter<maxScatter && len_unused > 0; cntScatter++)
		{
			if (cntScatter == maxScatter - 1) // last one?
			{
				scatter[cntScatter].len = len_unused;
				scatter[cntScatter].ptr = asic_buf;
				cnt_scatters++;
			}
			else
			{
				scatter[cntScatter].len = (rtlglue_random() % len_unused) + 1;
				scatter[cntScatter].ptr = asic_buf;
				asic_buf += scatter[cntScatter].len;
				len_unused -= scatter[cntScatter].len;
				cnt_scatters++;
			}
		}

		for(i=0; i<lenAuthKey; i++)
			authKey[i] = (i & 0xff);

		ret = ipsec_GeneralApiTestItem(modeCrypto, modeAuth,
			cntScatter, scatter, asic_enc,
			lenCryptoKey, cryptoKey,
			lenAuthKey, authKey,
			cryptoIv, NULL/*pad*/, asic_digest,
			lenA2eo, pktLen - lenA2eo);

		if (ret == FAILED)
		{
			printk("SAWB:%d DMA:%d Len:%d Off:%d Enc:0x%x Auth:0x%x CK:%d AK:%d A2EO:%d DATA:%d (%s) [FAILED]\n",
				rtl_ipsecEngine_sawb(), rtl_ipsecEngine_dma_mode(),
				pktLen, offset, modeCrypto, modeAuth, lenCryptoKey, lenAuthKey, lenA2eo, cntScatter,
				scatter_show(cntScatter, scatter));

			if (g_failed_reset)
			{
				rtl_ipsecEngine_reset();
				cnt_failed++;
			}
			else
				return FAILED;
		}
		else if (ret > 0)
		{
			if (dbg_level >= 2)
				printk("SAWB:%d DMA:%d Len:%d Off:%d Enc:0x%x Auth:0x%x CK:%d AK:%d A2EO:%d DATA:%d (%s) [SKIP %d]\n",
					rtl_ipsecEngine_sawb(), rtl_ipsecEngine_dma_mode(),
					pktLen, offset, modeCrypto, modeAuth, lenCryptoKey, lenAuthKey, lenA2eo, cntScatter,
					scatter_show(cntScatter, scatter), ret);

			cnt_ignore++;
		}
		else
		{
			if (dbg_level >= 1)
				printk("SAWB:%d DMA:%d Len:%d Off:%d Enc:0x%x Auth:0x%x CK:%d AK:%d A2EO:%d DATA:%d (%s) [SUCCESS]\n",
					rtl_ipsecEngine_sawb(), rtl_ipsecEngine_dma_mode(),
					pktLen, offset, modeCrypto, modeAuth, lenCryptoKey, lenAuthKey, lenA2eo, cntScatter,
					scatter_show(cntScatter, scatter));

			cnt_success++;
		}
	}

	if (g_failed_reset)
		printk("\nGeneral API Test Finished. (%d, %d, %d) %d\n", cnt_success, cnt_failed, cnt_ignore, cnt_scatters);
	else
		printk("\nGeneral API Random Test Finished. %d/%d, %d\n", cnt_success, cnt_success + cnt_ignore, cnt_scatters);

	for (i=0; i<LEN_CRTPTO_VALS; i++)
	{
		for (j=0; j<LEN_AUTH_VALS; j++)
		{
			switch (modeCrypto_val[i])
			{
			case -1: // no crypto
				cnt = 0;
				for (k=0; k<LEN_CRYPTOKEY_VALS; k++)
					cnt += stat_tests[i][j][k];

				printk("no crypto: stat(0x%x, 0x%x) = %d\n",
					modeCrypto_val[i], modeAuth_val[j], cnt);
				break;
			case 0x00: // des cbc
			case 0x02: // des ecb
				for (k=0; k<LEN_CRYPTOKEY_VALS; k++)
				{
					if (k == 0)
					{
						printk("des: stat(0x%x, 0x%x) = %d\n",
							modeCrypto_val[i], modeAuth_val[j], stat_tests[i][j][0]);
						continue;
					}

					// check keylen != 8
					if (stat_tests[i][j][k])
						printk("!des: stat(0x%x, 0x%x, %d) = %d\n",
							modeCrypto_val[i], modeAuth_val[j], CryptoKey_val[k],
							stat_tests[i][j][k]);
				}
				break;
			case 0x01: // 3des cbc
			case 0x03: // 3des ecb
				for (k=0; k<LEN_CRYPTOKEY_VALS; k++)
				{
					if (k == 2)
					{
						printk("3des: stat(0x%x, 0x%x) = %d\n", 
							modeCrypto_val[i], modeAuth_val[j], stat_tests[i][j][2]);
						continue;
					}

					// check keylen != 24
					if (stat_tests[i][j][k])
						printk("!3des: stat(0x%x, 0x%x, %d) = %d\n",
							modeCrypto_val[i], modeAuth_val[j], CryptoKey_val[k],
							stat_tests[i][j][k]);
				}
				break;
			default: // aes cbc/ecb/ctr 128/192/256
				for (k=0; k<LEN_CRYPTOKEY_VALS; k++)
				{
					if (k)
					{
						printk("aes: stat(0x%x, 0x%x, %d) = %d\n",
							modeCrypto_val[i], modeAuth_val[j], CryptoKey_val[k],
							stat_tests[i][j][k]);
						continue;
					}

					// check keylen != 16/24/32
					if (stat_tests[i][j][k])
						printk("!aes: stat(0x%x, 0x%x, %d) = %d\n",
							modeCrypto_val[i], modeAuth_val[j], CryptoKey_val[k],
							stat_tests[i][j][k]);
				}
				break;
			}
		}
	}

	return SUCCESS;	
}

void aes_vector_test()
{
	static uint8 _cryptoKey[32 + 32 + 32]; // 32 for AES-256
	static uint8 _cryptoIv[32 + 16 + 32]; // 16 for AES
	static uint8 _asic_orig[32 + MAX_PKTLEN + 32];
	static uint8 _asic_enc[32 + MAX_PKTLEN + 32];
	static uint8 *cryptoKey, *cryptoIv;
	static uint8 *asic_orig, *asic_enc;
	rtl_ipsecScatter_t scatter[1];
	char *key = "\x06\xa9\x21\x40\x36\xb8\xa1\x5b"
		  "\x51\x2e\x03\xd5\x34\x12\x00\x06";
	int klen = 16;
	char *iv = "\x3d\xaf\xba\x42\x9d\x9e\xb4\x30"
		  "\xb4\x22\xda\x80\x2c\x9f\xac\x41";
	char *input = "\xe3\x53\x77\x9c\x10\x79\xae\xb8"
		  "\x27\x08\x94\x2d\xbe\x77\x18\x1a";
	int ilen = 16;
	char *result = "Single block msg";
	AES_KEY aes_key;
	int retval;
	static uint8 sw_orig[MAX_PKTLEN];
	static uint8 sw_enc[MAX_PKTLEN];

	cryptoKey = (void *) UNCACHED_ADDRESS(&_cryptoKey[32]);
	cryptoIv = (void *) UNCACHED_ADDRESS(&_cryptoIv[32]);
	asic_orig = (void *) UNCACHED_ADDRESS(&_asic_orig[32]);
	asic_enc = (void *) UNCACHED_ADDRESS(&_asic_enc[32]);

	AES_set_encrypt_key(key, klen*8, &aes_key);
	memcpy((void *) cryptoKey, &aes_key.rd_key[4*10], 16);
	if (g_rtl_ipsec_dbg) memDump(aes_key.rd_key, 11*4*4, "key");

	memcpy(cryptoIv, iv, 16);

	scatter[0].len = ilen;
#if 0
	scatter[0].ptr = input;
#else
	memcpy(asic_orig, input, ilen);
	scatter[0].ptr = asic_orig;
#endif

	rtl_ipsecEngine(0x20, -1, 1, scatter, _asic_enc,
		klen, cryptoKey,
		0, NULL,
		cryptoIv, NULL, NULL,
		0, ilen
	);

	if (memcmp(scatter[0].ptr, result, ilen) == 0)
	{
		if (g_rtl_ipsec_dbg)
			printk("aes test vector success!\n");
	}
	else
	{
		memcpy(sw_orig, input, ilen);
		retval = ipsecSim(0x20, -1, sw_orig, sw_enc,
			klen, key,
			0, NULL,
			cryptoIv, NULL, NULL,
			0, ilen
		);

		if (retval != SUCCESS)
		{
			printk("ipsecSim failed!\n");
			return;
		}

		if (memcmp(scatter[0].ptr, sw_enc, ilen) == 0)
		{
			memDump(scatter[0].ptr, ilen, "enc");
			memDump(result, ilen, "result");
			printk("aes test failed, but ipsecSim ok\n");
		}
		else
		{
			memDump(sw_enc, ilen, "sw_enc");
			memDump(result, ilen, "result");
			printk("ipsecSim test vector failed!\n");
		}
	}
}

void hmac_md5_vector_test()
{
	char *key = "\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b";
	int ksize = 16;
	char *plaintext = "Hi There";
	int psize = 8;
	char *digest = "\x92\x94\x72\x7a\x36\x38\xbb\x1c"
			"\x13\xf4\x8e\xf8\x15\x8b\xfc\x9d";
	rtl_ipsecScatter_t scatter[1];
	static uint8 _asic_orig[32 + MAX_PKTLEN + 32];
	static uint8 _authKey[32 + MAX_AUTH_KEY + 32];
	static uint8 _AsicDigest[32+20+32]; /* max digest length */
	uint8 *asic_orig = (void *) UNCACHED_ADDRESS(&_asic_orig[32]);
	uint8 *pAuthKey = (void *) UNCACHED_ADDRESS(&_authKey[32]);
	uint8 *pAsicDigest = &_AsicDigest[32];
	int retval;
	static uint8 sw_digest[SHA_DIGEST_LENGTH];
	
	scatter[0].len = psize;
#if 0
	scatter[0].ptr = plaintext;
#else
	memcpy(asic_orig, plaintext, psize);
	scatter[0].ptr = asic_orig;
#endif

	memcpy(pAuthKey, key, ksize);

	rtl_ipsecEngine(-1, 2, 
		1, scatter, NULL,
		0, NULL,
		ksize, pAuthKey,
		NULL, NULL, pAsicDigest,
		0, psize
	);

	if (memcmp(pAsicDigest, digest, MD5_DIGEST_LENGTH) == 0)
	{
		if (g_rtl_ipsec_dbg)
			printk("hmac-md5 test vector success!\n");
	}
	else
	{
		retval = ipsecSim(-1, 2,
			plaintext, NULL,
			0, NULL,
			ksize, key,
			NULL, NULL, sw_digest,
			0, psize
		);

		if (retval != SUCCESS)
		{
			printk("ipsecSim failed!\n");
			return;
		}

		if (memcmp(sw_digest, digest, MD5_DIGEST_LENGTH) == 0)
		{
			memDump(pAsicDigest, MD5_DIGEST_LENGTH, "asic digest");
			memDump(digest, MD5_DIGEST_LENGTH, "digest");
			printk("ipsec test failed, but ipsecSim ok\n");
		}
		else
		{
			memDump(sw_digest, MD5_DIGEST_LENGTH, "sw digest");
			memDump(digest, MD5_DIGEST_LENGTH, "digest");
			printk("ipsecSim test vector failed!\n");
		}
	}
}

void rtl_ipsec_usage(char *type)
{
	if (type == NULL)
	{
		prom_printf("Usage: crypto <init|rand|throughput|asic|info|debug|vector|config>\n");
	}
	else if (strcmp(type, "init") == 0)
	{
		prom_printf("Usage: crypto init [max_desc] [dma_burst]\n");
		prom_printf("  max_desc: 3..32, default: 8 descriptors\n");
		prom_printf("  dma_burst: 0..3, default: 2 (64 bytes)\n");
		prom_printf("             0: 16 bytes\n");
		prom_printf("             1: 32 bytes\n");
		prom_printf("             2: 64 bytes\n");
		prom_printf("             3: 128 bytes\n");
	}
	else if (strcmp(type, "rand") == 0)
	{
		prom_printf("Usage: crypto rand [<seed> <round>] [<crypto_mode> <auth_mode>] [cntScatter]\n");
		prom_printf("  seed: random seed\n");
		prom_printf("  round: test round, default: 100\n");
		prom_printf("  crypto_mode: 0x00:CBC_DES,0x01:CBC_3DES,0x02:ECB_DES,0x03:ECB_3DES \n");
		prom_printf("  			  : 0x20:CBC_AES,0x22:ECB_AES,0x23:CTR_AES\n");
		prom_printf("  			  : 0xffffffff: no crypto, default: random \n");
		prom_printf("  auth_mode: 0:HASH_MD5,1:HASH_SHA1,2:HMAC_MD5,3:HMAC_SHA1\n");
		prom_printf("             : 0xffffffff: no auth, default: random\n");
		prom_printf("  cntScatter: 1..MAX_SCATTER: count scatter, -1: random, default: -1\n");
	}
	else if (strcmp(type, "throughput") == 0)
	{
		prom_printf("Usage: crypto throughput <asic> <round> <crypto_start> <crypto_end>\n");
		prom_printf("                         <auth_start> <auth_end> <ckey_len>\n");
		prom_printf("                         <akey_len> <pkt_len> <a2eo> <cntScatter>\n");
		prom_printf("  asic: 0: sw, 1: asic\n");
		prom_printf("  round: test round\n");
		prom_printf("  crypto: 0x00:CBC_DES,0x01:CBC_3DES,0x02:ECB_DES,0x03:ECB_3DES \n");
		prom_printf("  		 : 0x20:CBC_AES,0x22:ECB_AES,0x23:CTR_AES\n");
		prom_printf("  		 : 0xffffffff: no crypto\n");
		prom_printf("  auth: 0:HASH_MD5,1:HASH_SHA1,2:HMAC_MD5,3:HMAC_SHA1\n");
		prom_printf("      : 0xffffffff: no auth\n");
		prom_printf("  ckey_len: 16: AES128, 24: AES192, 32: AES256\n");
		prom_printf("  akey_len: authentication length\n");
		prom_printf("  pkt_len: packet length\n");
		prom_printf("  a2eo: authentication to encryption offset\n");
		prom_printf("  cntScatter: 1..MAX_SCATTER: count scatter\n");
	}
	else if (strcmp(type, "asic") == 0)
	{
		prom_printf("Usage: crypto asic <round> <len_start> <len_end>\n");
		prom_printf("                   <off_start> <off_end>\n");
		prom_printf("                   <crypto_start> <crypto_end>\n");
		prom_printf("                   <auth_start> <auth_end>\n");
		prom_printf("                   <ckeylen_start> <ckeylen_end>\n");
		prom_printf("                   <akeylen_start> <akeylen_start>\n");
		prom_printf("                   <a2eo_start> <a2eo_end>\n");
		prom_printf("                   <cntScatter>\n");
	}
	else if (strcmp(type, "debug") == 0)
	{
		prom_printf("Usage: crypto debug <level>\n");
		prom_printf("  level: 0: show error, 1: show success, 2: show skip, 3: show all, default: 0\n");
	}
	else if (strcmp(type, "vector") == 0)
	{
		prom_printf("Usage: crypto vector <round> <mode>\n");
		prom_printf("  round: test round\n");
		prom_printf("  mode: 'aes128': AES 128 test vector\n");
		prom_printf("      : 'hmac-md5': HMAC-MD5 test vector\n");
	}
	else if (strcmp(type, "config") == 0)
	{
		prom_printf("Usage: crypto config <config_name> <config_value>\n");
		prom_printf("  config_name: sawb, config_value: 0 = disabled, 1 = enabled\n");
		prom_printf("  config_name: fr, config_value: 0 = failed exit, 1 = do failed reset\n");
		prom_printf("  config_name: seed, config_value: seed_value\n");
	}
}

void rtl_ipsec_test(int argc, char *argv[])
{
	if (argc < 1)
	{
		rtl_ipsec_usage(NULL);
		return;
	}

	if (strcmp(argv[0], "init") == 0)
	{
		int max_desc;
		int dma_burst;
		int sawb;

		if (argc == 1)
		{
			rtl_ipsecEngine_init(8, 2);
			return;
		}
		else if (argc == 3)
		{
			max_desc = strtoul((const char*)(argv[1]), (char **)NULL, 10);
			dma_burst = strtoul((const char*)(argv[2]), (char **)NULL, 10);

			if ((max_desc >= 3 && max_desc <= 32) &&
				(dma_burst >= 0) && (dma_burst <= 3))
			{
				rtl_ipsecEngine_init(max_desc, dma_burst);
				return;
			}
		}

		rtl_ipsec_usage("init");
		return;
	}
	else if (strcmp(argv[0], "config") == 0)
	{
		if ((argc == 3) &&
			((strcmp(argv[1], "sawb") == 0) || 
			(strcmp(argv[1], "fr") == 0) ||
			(strcmp(argv[1], "seed") == 0)))
		{
			int value;

			value = strtoul((const char*)(argv[2]), (char **)NULL, 10) ? 1 : 0;
			if (strcmp(argv[1], "sawb") == 0)
			{
				rtl_ipsecSetOption(RTL_IPSOPT_SAWB, value);
				return;
			}
			else if (strcmp(argv[1], "fr") == 0)
			{
				g_failed_reset = value;
				return;
			}
			else if (strcmp(argv[1], "seed") == 0)
			{
				g_seed = value;
				return;
			}
		}

		rtl_ipsec_usage("config");
		return;
	}
	else if (strcmp(argv[0], "rand") == 0)
	{
		static int seed = 0; 
		int round;
		int mode_crypto;
		int mode_auth;
		int cntScatter;

		// Random test
		if (argc == 1)
		{
			ipsec_GeneralApiRandTest(seed++, 100, 0xff, 0xff, 0xff); // 0xff for random
		}
		else if (argc == 3)
		{
			seed = strtoul((const char*)(argv[1]), (char **)NULL, 10);
			round = strtoul((const char*)(argv[2]), (char **)NULL, 10);
			ipsec_GeneralApiRandTest(seed, round, 0xff, 0xff, 0xff); // 0xff for random
		}
		else if (argc == 5 || argc == 6)
		{
		// CRYPTO:
		//   0x00:CBC_DES,0x01:CBC_3DES,0x02:ECB_DES,0x03:ECB_3DES
		//   0x20:CBC_AES,0x22:ECB_AES,0x23:CTR_AES
		// AUTH:
		//   0:HASH_MD5,1:HASH_SHA1,2:HMAC_MD5,3:HMAC_SHA1
			seed = strtoul((const char*)(argv[1]), (char **)NULL, 10);
			round = strtoul((const char*)(argv[2]), (char **)NULL, 10);
			mode_crypto = strtoul((const char*)(argv[3]), (char **)NULL, 16);
			mode_auth = strtoul((const char*)(argv[4]), (char **)NULL, 16);
			if (argc == 6)
				cntScatter = strtoul((const char*)(argv[5]), (char **)NULL, 16);
			else
				cntScatter = 0xff;

			ipsec_GeneralApiRandTest(seed, round, mode_crypto, mode_auth, cntScatter);
		}
		else
		{
			rtl_ipsec_usage("rand");
			return;
		}
	}
	else if (strcmp(argv[0], "asic") == 0)
	{
		int round;
		int len_start, len_end, cntScatter;
		int off_start, off_end;
		int crypto_start, crypto_end;
		int auth_start, auth_end;
		int ckeylen_start, ckeylen_end;
		int akeylen_start, akeylen_end;
		int a2eo_start, a2eo_end;

		if (argc != 17)
		{
			rtl_ipsec_usage("asic");
			return;
		}

		round = strtoul((const char*)(argv[1]), (char **)NULL, 10);
		len_start = strtoul((const char*)(argv[2]), (char **)NULL, 10);
		len_end = strtoul((const char*)(argv[3]), (char **)NULL, 10);
		off_start = strtoul((const char*)(argv[4]), (char **)NULL, 10);
		off_end = strtoul((const char*)(argv[5]), (char **)NULL, 10);
		crypto_start = strtoul((const char*)(argv[6]), (char **)NULL, 16);
		crypto_end = strtoul((const char*)(argv[7]), (char **)NULL, 16);
		auth_start = strtoul((const char*)(argv[8]), (char **)NULL, 16);
		auth_end = strtoul((const char*)(argv[9]), (char **)NULL, 16);
		ckeylen_start = strtoul((const char*)(argv[10]), (char **)NULL, 10);
		ckeylen_end = strtoul((const char*)(argv[11]), (char **)NULL, 10);
		akeylen_start = strtoul((const char*)(argv[12]), (char **)NULL, 10);
		akeylen_end = strtoul((const char*)(argv[13]), (char **)NULL, 10);
		a2eo_start = strtoul((const char*)(argv[14]), (char **)NULL, 10);
		a2eo_end = strtoul((const char*)(argv[15]), (char **)NULL, 10);
		cntScatter = strtoul((const char*)(argv[16]), (char **)NULL, 10);

		if (ipsec_GeneralApiTest(round,
			len_start, len_end,
			off_start, off_end,
			crypto_start, crypto_end,
			auth_start, auth_end,
			ckeylen_start, ckeylen_end,
			akeylen_start, akeylen_end,
			a2eo_start, a2eo_end,
			cntScatter) == SUCCESS)
		{
			printf("test success\n");
		}
		else
		{
			printf("test failed\n");
		}
	}
	else if (strcmp(argv[0], "throughput") == 0)
	{
		int asic;
		int round;
		int crypto_start, crypto_end;
		int auth_start, auth_end;
		int ckey_len, akey_len;
		int pkt_len;
		int a2eo;
		int cntScatter;

		if (argc != 12)
		{
			rtl_ipsec_usage("throughput");
			return;
		}

		asic = strtoul((const char*)(argv[1]), (char **)NULL, 10);
		round = strtoul((const char*)(argv[2]), (char **)NULL, 10);

		crypto_start = strtoul((const char*)(argv[3]), (char **)NULL, 16);
		crypto_end = strtoul((const char*)(argv[4]), (char **)NULL, 16);
		auth_start = strtoul((const char*)(argv[5]), (char **)NULL, 16);
		auth_end = strtoul((const char*)(argv[6]), (char **)NULL, 16);

		ckey_len = strtoul((const char*)(argv[7]), (char **)NULL, 10);
		akey_len = strtoul((const char*)(argv[8]), (char **)NULL, 10);

		pkt_len = strtoul((const char*)(argv[9]), (char **)NULL, 10);
		a2eo = strtoul((const char*)(argv[10]), (char **)NULL, 10);

		cntScatter = strtoul((const char*)(argv[11]), (char **)NULL, 10);

		//              all auth: round,   -1,   -1, -1, 3,  0, auth_len, pkt_len, a2eo
		// des & 3des + all auth: round,    0,    7, -1, 3,  0, auth_len, pkt_len(1400), a2eo
		// aes 128    + all auth: round, 0x20, 0x27, -1, 3, 16, auth_len, pkt_len(1408), a2eo
		// aes 192    + all auth: round, 0x20, 0x27, -1, 3, 24, auth_len, pkt_len(1408), a2eo
		// aes 256    + all auth: round, 0x20, 0x27, -1, 3, 32, auth_len, pkt_len(1408), a2eo
		cryptoThroughput(asic, round, crypto_start, crypto_end, auth_start,
			auth_end, ckey_len, akey_len, pkt_len, a2eo, cntScatter);
	}
	else if (strcmp(argv[0], "info") == 0)
	{
		rtl_ipsec_info();
	}
	else if (strcmp(argv[0], "debug") == 0)
	{
		int enable;

		if (argc != 2)
		{
			rtl_ipsec_usage("debug");
			return;
		}

		dbg_level = strtoul((const char*)(argv[1]), (char **)NULL, 10);
		if (dbg_level < 0 || dbg_level > 3)
			dbg_level = 0;

		g_rtl_ipsec_dbg = (dbg_level == 3);
	}
	else if (strcmp(argv[0], "vector") == 0)
	{
		int test, round;

		if (argc != 3)
		{
			rtl_ipsec_usage("vector");
			return;
		}

		round = strtoul((const char*)(argv[1]), (char **)NULL, 10);

		if (strcmp(argv[2], "aes128") == 0)
		{
			for (test=0; test<round; test++)
				aes_vector_test();

			prom_printf("vector test done.\n");
		}
		else if (strcmp(argv[2], "hmac-md5") == 0)
		{
			for (test=0; test<round; test++)
				hmac_md5_vector_test();

			prom_printf("vector test done.\n");
		}
		else
		{
			rtl_ipsec_usage("vector");
		}
	}
	else
	{
		rtl_ipsec_usage(NULL);
	}
}

