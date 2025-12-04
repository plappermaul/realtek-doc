/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#include "rtl8195a.h"


//#include "basic_types.h"
//#include "diag.h"

#include "cryptoSim.h"

//#include "section_config.h"
//#include "FreeRTOS.h"
//#include "hal_timer.h"


extern HAL_TIMER_OP HalTimerOp;


#if 1 
// TODO: 
// will modify to see how to do assertion and print out in the future
// +++++++++++++++++++++++++++++++++++++++++


#define memDump rtl_memDump

#define DBG_CRYPTO_INFO(...)     do {\
    if (unlikely(ConfigDebugInfo & _DBG_CRYPTO_)) \
        _DbgDump("\r"IPSEC_INFO_PREFIX __VA_ARGS__);\
}while(0)


#define dbg_printk(fmt, args...) \
	     	DBG_CRYPTO_INFO("%s():%d : " fmt "\n", __FUNCTION__, __LINE__, ##args); 

#define dbg_mem_dump(start, size, str_header) \
	if ( ConfigDebugInfo & _DBG_CRYPTO_ ) { \
	    	printk("%s():%d : memdump : address: %08x, size: %d\n", __FUNCTION__, __LINE__, start, size); \
	    	memDump((const u8*)start, size, (char*)str_header); \
	}

#if 0
#define rtl_assert(x)  do { \
					if((x) == 0) {\
							err_printk("\n\rAssert(" #x ") failed"); \
                            rtl_halt();\
						} \
					} while(0)

							
        

#else 

#define rtl_assert(x) 
#endif


#define err_printk(fmt, args...) \
			DBG_CRYPTO_ERR("%s():%d : ERROR : " fmt "\n", __FUNCTION__, __LINE__, ##args)
					
#define rtl_halt(x) \
							err_printk("\n\r HALT! \n\r"); \
							for(;;);

#endif


#define Crypto_CMD	"CRYPTO"



u8 _asic_authkey[1024] 								__attribute__ ((aligned (8)));


u8 _asic_cryptkey[1024] 							__attribute__ ((aligned (8)));


u8 _asic_iv[1024]									__attribute__ ((aligned (8)));


u8 _asic_orig[32 + CRYPTO_MAX_MSG_LENGTH + 8 + 32] __attribute__ ((aligned (8)));


u8 _asic_enc[32 + CRYPTO_MAX_MSG_LENGTH + 32] 		__attribute__ ((aligned (8)));


u8 _sw_orig[CRYPTO_MAX_MSG_LENGTH] 					__attribute__ ((aligned (8)));


u8 _sw_enc[32 + CRYPTO_MAX_MSG_LENGTH + 32]			__attribute__ ((aligned (8)));


u8 _asic_dec[32 + CRYPTO_MAX_MSG_LENGTH + 32]		__attribute__ ((aligned (8)));



u8 _sw_dec[32 + CRYPTO_MAX_MSG_LENGTH + 32]			__attribute__ ((aligned (8)));


u8 _AsicDigest[32+CRYPTO_SHA2_DIGEST_LENGTH+32] 	__attribute__ ((aligned (4)));


u8 sw_digest[CRYPTO_SHA2_DIGEST_LENGTH]				__attribute__ ((aligned (4)));

/******************************************************************************
 * External Function
 ******************************************************************************/

extern u32 
Strtoul(
    IN  const u8 *nptr,
    IN  u8 **endptr,
    IN  u32 base
);


/******************************************************************************
 * Functions
 ******************************************************************************/





int rtl_crypto_auth_sim(const u32 authtype, 
	const u8* pKey, const u32 keylen, const u8* pText, const u32 textlen, u8* pDigest, const u32 digestlen)
{
    int     ret;

 
	ret = authSim(authtype, pText, textlen, pKey, keylen, pDigest, digestlen);
            

    return ret;
}



int rtl_crypto_esp_sim(const u32 modeCrypto, 
	const u8* pKey, const u32 keylen, const u8* pIv, const u32 ivLen, const u8* pText, const u32 textlen, u8* pResult)
{
    int     ret = SUCCESS;

    

	dbg_printk("modeCrypto=0x%x, pText=%p, pResult=%p, keylen=%d, pKey=%p, pIv=%p, ivLen=%d, pText=%p textlen=%d\n", 
		modeCrypto, pText, pResult, keylen, pKey, pIv, ivLen, pText, textlen);

	dbg_mem_dump(pText, textlen, "Text :");
	dbg_mem_dump(pKey, keylen, "Key :");
	dbg_mem_dump(pIv, ivLen, "IV :");

	if ( (modeCrypto & 0x30) == 0x30 ) { // chacha20
		CRYPTO_sim_chacha_20(pResult, pText, textlen, pKey, pIv, 0);
	} else if ( (modeCrypto & 0x20) == 0x20 ) { // aes
		ret = aesSim_aes(modeCrypto, pText,
			textlen, keylen, pKey, pIv, pResult);
	} else { // 3des / des
		ret = desSim_des(modeCrypto, pText,
			textlen, pKey, pIv, pResult);
	}

    return ret;
}





void rtl_crypto_usage(char *type)
{

    if (type == NULL)
    {
        //printk("Usage: crypto <init|rand|throughput|asic|info|debug|vector|config>\n");
        printk("Usage: crypto <init|vector|test>\n");
    }
    else if (strcmp(type, "init") == 0)
    {        
        printk("Usage:" Crypto_CMD "init\n");
        printk("  max_desc:  default: 8 descriptors\n");
        printk("  dma_burst: default: 2 (64 bytes)\n");
    }
    else if (strcmp(type, "debug") == 0)
    {
        printk("Usage:" Crypto_CMD "debug [mode]\n");
        printk("  mode:  0: turn off, 1: turn on \n");
    }
    else if (strcmp(type, "vector") == 0)
    {
		//
		//		vector 0 16 -1 -> 3DES-CBC : 0x10
		//		vector 0 18 -1 -> 3DES-ECB : 0x12 
		//		vector 0 32 -1 -> AES-CBC :   0x20
		//		vector 0 34 -1 -> AES-ECB  :  0x22
		//		vector 0 35 -1 -> AES-CTR  : 0x23
		//
		//		vector 1 -1 0 -> SHA1
		//		vector 1 -1 1 -> SHA2
		//		vector 1 -1 2 -> MD5
		//		vector 1 -1 4 -> HMAC-SHA1
		//		vector 1 -1 5 -> HMAC-SHA2
		//		vector 1 -1 6 -> HMAC-MD5
		//           vector 1 -1 7 -> Poly1305

        printk("Usage:" Crypto_CMD "vector <mode> <cipher_type> <auth_type> \n");
        printk("  mode: 0: Auth, 1: Cipher\n");
        printk("  cipher_type: -1 : None, 16: 3DES-CBC, 18: 3DES-ECB, 32: AES_CBC, 34: AES-ECB, 35: AES-CTR \n");
        printk("  auth_type: -1: None, 0: SHA1, 1: SHA2, 2: MD5, 4:HMAC-SHA1, 5: HMAC-SHA2, 6: HMAC-MD5 \n");
    }
    else if (strcmp(type, "test") == 0)
    {
        printk("Usage:" Crypto_CMD "test\n");
		printk("  will do random test automatically\n"); 
//        printk("  seed: random seed\n");
//        printk("  round: test round, default: 100\n");
//        printk("  crypto_mode: 0x00:CBC_DES,0x01:CBC_3DES,0x02:ECB_DES,0x03:ECB_3DES \n");
//        printk("             : 0x20:CBC_AES,0x22:ECB_AES,0x23:CTR_AES\n");
//        printk("             : 0xffffffff: no crypto, default: random \n");
//        printk("  auth_mode: 0:HASH_MD5,1:HASH_SHA1,2:HMAC_MD5,3:HMAC_SHA1\n");
//        printk("             : 0xffffffff: no auth, default: random\n");
//        printk("  cntScatter: 1..MAX_SCATTER: count scatter, -1: random, default: -1\n");
    }
#if 0
    else if (strcmp(type, "throughput") == 0)
    {
        printk("Usage:" Crypto_CMD "throughput <asic> <round> <crypto_start> <crypto_end>\n");
        printk("                         <auth_start> <auth_end> <ckey_len>\n");
        printk("                         <akey_len> <pkt_len> <a2eo> <cntScatter>\n");
        printk("  asic: 0: sw, 1: asic\n");
        printk("  round: test round\n");
        printk("  crypto: 0x00:CBC_DES,0x01:CBC_3DES,0x02:ECB_DES,0x03:ECB_3DES \n");
        printk("        : 0x20:CBC_AES,0x22:ECB_AES,0x23:CTR_AES\n");
        printk("        : 0xffffffff: no crypto\n");
        printk("  auth: 0:HASH_MD5,1:HASH_SHA1,2:HMAC_MD5,3:HMAC_SHA1\n");
        printk("      : 0xffffffff: no auth\n");
        printk("  ckey_len: 16: AES128, 24: AES192, 32: AES256\n");
        printk("  akey_len: authentication length\n");
        printk("  pkt_len: packet length\n");
        printk("  a2eo: authentication to encryption offset\n");
        printk("  cntScatter: 1..MAX_SCATTER: count scatter\n");
    }
    else if (strcmp(type, "asic") == 0)
    {
        printk("Usage:" Crypto_CMD "asic <round> <len_start> <len_end>\n");
        printk("                   <off_start> <off_end>\n");
        printk("                   <crypto_start> <crypto_end>\n");
        printk("                   <auth_start> <auth_end>\n");
        printk("                   <ckeylen_start> <ckeylen_end>\n");
        printk("                   <akeylen_start> <akeylen_start>\n");
        printk("                   <a2eo_start> <a2eo_end>\n");
        printk("                   <cntScatter>\n");
    }
    else if (strcmp(type, "debug") == 0)
    {
        printk("Usage:" Crypto_CMD " debug <level>\n");
        printk("  level: 0: show error, 1: show success, 2: show skip, 3: show all, default: 0\n");
    }
    else if (strcmp(type, "config") == 0)
    {
        printk("Usage:" Crypto_CMD "config <config_name> <config_value>\n");
        printk("  config_name: sawb, config_value: 0 = disabled, 1 = enabled\n");
        printk("  config_name: fr, config_value: 0 = failed exit, 1 = do failed reset\n");
        printk("  config_name: seed, config_value: seed_value\n");
    }
#endif
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



// 3DES - ECB : IP_SEC vector 0 18 ( 18 = 0x12)

__attribute__ ((aligned (4)))
static const unsigned char des3_test_keys[24] =
{
    0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
    0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0x01,
    0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0x01, 0x23
};



__attribute__ ((aligned (4)))
static const unsigned char des3_test_iv[8] =
{
    0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF,
};


//__attribute__ ((aligned (4)))
static const unsigned char des3_test_buf[64] =
{
		0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x21, 0x21, 0x21,
		0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x21, 0x21, 0x21,
		0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x21, 0x21, 0x21,
		0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x21, 0x21, 0x21,
		0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x21, 0x21, 0x21,
		0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x21, 0x21, 0x21,
		0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x21, 0x21, 0x21,
		0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x21, 0x21, 0x21,
};
//	"Hello!!!"

// 3DES-SHA MtE

__attribute__ ((aligned (4)))
static const unsigned char des3_test_buf_encoded[8] =
{
	0x8a, 0x2f, 0xe9, 0xb3, 0x4a, 0x4c, 0x2f, 0x47,
};


// http://en.wikipedia.org/wiki/MD5
// MD5("") = d41d8cd98f00b204e9800998ecf8427e	
//char *_md5_plaintext = "";
//char *_md5_digest =  "\xd4\x1d\x8c\xd9\x8f\x00\xb2\x04"
//					 "\xe9\x80\x09\x98\xec\xf8\x42\x7e";

// MD5("The quick brown fox jumps over the lazy dog") =
//  9e107d9d372bb6826bd81d3542a419d6

//__attribute__ ((aligned (4)))
static const u8 _md5_plaintext[] 	= "The quick brown fox jumps over the lazy dog";


//__attribute__ ((aligned (4)))
static const u8 _md5_digest[] 		=  "\x9e\x10\x7d\x9d\x37\x2b\xb6\x82"
					 			"\x6b\xd8\x1d\x35\x42\xa4\x19\xd6";


// http://en.wikipedia.org/wiki/SHA-1
// SHA1("") = da39a3ee5e6b4b0d3255bfef95601890afd80709
// char *_sha1_plaintext = "";
// char *_sha1_digest =  "\xda\x39\xa3\xee\x5e\x6b\x4b\x0d"
//					  "\x32\x55\xbf\xef\x95\x60\x18\x90"
//					  "\xaf\xd8\x07\x09";

// SHA1("The quick brown fox jumps over the lazy dog")
//   = 2fd4e1c67a2d28fced849ee1bb76e7391b93eb12

//__attribute__ ((aligned (4)))
static const u8 _sha1_plaintext[] 	= "The quick brown fox jumps over the lazy dog";


//__attribute__ ((aligned (4)))
static const u8 _sha1_digest[] 	=  "\x2f\xd4\xe1\xc6\x7a\x2d\x28\xfc"
					  			"\xed\x84\x9e\xe1\xbb\x76\xe7\x39"
					  			"\x1b\x93\xeb\x12";

//char *_hmac_md5_key = "\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b";
//char *_hmac_md5_plaintext = "Hi There";
//char *_hmac_md5_digest =  "\x92\x94\x72\x7a\x36\x38\xbb\x1c"
//            "\x13\xf4\x8e\xf8\x15\x8b\xfc\x9d";


// http://en.wikipedia.org/wiki/Hash-based_message_authentication_code
// HMAC_MD5("", "") = 0x74e6f7298a9c2d168935f58c001bad88
//char *_hmac_md5_key = "";
//char *_hmac_md5_plaintext = "";
//char *_hmac_md5_digest =  "\x74\xe6\xf7\x29\x8a\x9c\x2d\x16"
//    "\x89\x35\xf5\x8c\x00\x1b\xad\x88";

// HMAC_MD5("key", "The quick brown fox jumps over the lazy dog") = 0x80070713463e7749b90c2dc24911e275


__attribute__ ((aligned (4)))
static const u8 _hmac_md5_key[] 		= "key";


//__attribute__ ((aligned (4)))
static const u8 _hmac_md5_plaintext[] 	= "The quick brown fox jumps over the lazy dog";


//__attribute__ ((aligned (4)))
static const u8 _hmac_md5_digest[] 	=  "\x80\x07\x07\x13\x46\x3e\x77\x49"
                                                "\xb9\x0c\x2d\xc2\x49\x11\xe2\x75";


// SHA-1 : 160 bit : 20 bytes
//
// http://en.wikipedia.org/wiki/Hash-based_message_authentication_code
// HMAC_SHA1("", "") = 0xfbdb1d1b18aa6c08324b7d64b71fb76370690e1d
//char *_hmac_sha1_key = "";
//char *_hmac_sha1_plaintext = "";
//char *_hmac_sha1_digest =  "\xfb\xdb\x1d\x1b\x18\xaa\x6c\x08"
//    "\x32\x4b\x7d\x64\xb7\x1f\xb7\x63"
//    "\x70\x69\x0e\x1d";

// HMAC_SHA1("key", "The quick brown fox jumps over the lazy dog") = 0xde7c9b85b8b78aa6bc8a7a36f70a90701c9db4d9


__attribute__ ((aligned (4)))
static const u8 _hmac_sha1_key[] = "key";


//__attribute__ ((aligned (4)))
static const u8 _hmac_sha1_plaintext[] = "The quick brown fox jumps over the lazy dog";


//__attribute__ ((aligned (4)))
static const u8 _hmac_sha1_digest[] =  "\xde\x7c\x9b\x85\xb8\xb7\x8a\xa6"
                                                "\xbc\x8a\x7a\x36\xf7\x0a\x90\x70"
                                                "\x1c\x9d\xb4\xd9";


// SHA-2 : 224 bit: 28 bytes;
//
// http://en.wikipedia.org/wiki/SHA-2
// 
// SHA224("The quick brown fox jumps over the lazy dog") =
//   0x 730e109bd7a8a32b1cb9d9a09aa2325d2430587ddbc0c38bad911525


//__attribute__ ((aligned (4)))
static const u8 _sha2_plaintext[] = "The quick brown fox jumps over the lazy dog";
//char *_sha2_digest =  "\x73\x0e\x10\x9b\xd7\xa8\xa3\x2b"
//					   "\x1c\xb9\xd9\xa0\x9a\xa2\x32\x5d"
//					   "\x24\x30\x58\x7d\xdb\xc0\xc3\x8b"
//					   "\xad\x91\x15\x25";

// SHA256("The quick brown fox jumps over the lazy dog") =
//    0x d7a8fbb307d7809469ca9abcb0082e4f8d5651e46d3cdb762d02d0bf37c9e592


//__attribute__ ((aligned (4)))
static const u8 _sha2_digest[] =  "\xd7\xa8\xfb\xb3\x07\xd7\x80\x94"
						"\x69\xca\x9a\xbc\xb0\x08\x2e\x4f"
						"\x8d\x56\x51\xe4\x6d\x3c\xdb\x76"
						"\x2d\x02\xd0\xbf\x37\xc9\xe5\x92";



// SHA384("The quick brown fox jumps over the lazy dog")
//   0x ca737f1014a48f4c0b6dd43cb177b0afd9e5169367544c494011e3317dbf9a509cb1e5dc1e85a941bbee3d7f2afbc9b1

//char *_sha2_digest =  "\xca\x73\x7f\x10\x14\xa4\x8f\x4c"
//						"\x0b\x6d\xd4\x3c\xb1\x77\xb0\xaf"
//						"\xd9\xe5\x16\x93\x67\x54\x4c\x49"
//						"\x40\x11\xe3\x31\x7d\xbf\x9a\x50"
//						"\x9c\xb1\xe5\xdc\x1e\x85\xa9\x41"
//						"\xbb\xee\x3d\x7f\x2a\xfb\xc9\xb1";

// SHA512("The quick brown fox jumps over the lazy dog") =
//   0x 07e547d9586f6a73f73fbac0435ed76951218fb7d0c8d788a309d785436bbb642e93a252a954f23912547d1e8a3b5ed6e1bfd7097821233fa0538f3db854fee6

//char *_sha2_digest =  
//	"\x07\xe5\x47\xd9\x58\x6f\x6a\x73"
//	"\xf7\x3f\xba\xc0\x43\x5e\xd7\x69"
//	"\x51\x21\x8f\xb7\xd0\xc8\xd7\x88"
//	"\xa3\x09\xd7\x85\x43\x6b\xbb\x64"
//	"\x2e\x93\xa2\x52\xa9\x54\xf2\x39"
//	"\x12\x54\x7d\x1e\x8a\x3b\x5e\xd6"
//	"\xe1\xbf\xd7\x09\x78\x21\x23\x3f"
//	"\xa0\x53\x8f\x3d\xb8\x54\xfe\xe6";


// 
// http://en.wikipedia.org/wiki/SHA-2
//
// HMAC_SHA256("", "") = 0xb613679a0814d9ec772f95d778c35fc5ff1697c493715653c6c712144292c5ad
//
// HMAC_SHA256("key", "The quick brown fox jumps over the lazy dog") = 
//    0xf7bc83f430538424b13298e6aa6fb143ef4d59a14946175997479dbc2d1a3cd8
//

__attribute__ ((aligned (4)))
static const u8 _hmac_sha256_key[] 		= "key";


//__attribute__ ((aligned (4)))
static const u8 _hmac_sha256_plaintext[] 	= "The quick brown fox jumps over the lazy dog";


//__attribute__ ((aligned (4)))
static const u8 _hmac_sha256_digest[] 		=  "\xf7\xbc\x83\xf4\x30\x53\x84\x24"
							 			"\xb1\x32\x98\xe6\xaa\x6f\xb1\x43"
							 			"\xef\x4d\x59\xa1\x49\x46\x17\x59"
							 			"\x97\x47\x9d\xbc\x2d\x1a\x3c\xd8";


//
// http://tools.ietf.org/html/draft-nir-cfrg-chacha20-poly1305-06 
// Poly1305 vector : Test vector #2


__attribute__ ((aligned (4)))
static const u8 poly1305_key[32] 		= {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x36, 0xe5, 0xf6, 0xb5, 0xc5, 0xe0, 0x60, 0x70,
	0xf0, 0xef, 0xca, 0x96, 0x22, 0x7a, 0x86, 0x3e
};

//__attribute__ ((aligned (4)))
static const u8 poly1305_plaintext[] 	= 
	"Any submission to the IETF intended by the Contributor for publication "
	"as all or part of an IETF Internet-Draft or RFC and any statement made "
	"within the context of an IETF activity is considered an \"IETF "
	"Contribution\". Such statements include oral statements in IETF "
	"sessions, as well as written and electronic communications made at "
	"any time or place, which are addressed to";


//__attribute__ ((aligned (4)))
static const u8 poly1305_digest[] 		=  
	{ 0x36, 0xe5, 0xf6, 0xb5, 0xc5, 0xe0, 0x60, 0x70, 
	  0xf0, 0xef, 0xca, 0x96, 0x22, 0x7a, 0x86, 0x3e };


//
// authtype : 0: SHA1, 1: SHA2, 2: MD5; 4: HMAC-SHA1, 5: HMAC-SHA2, 6: HMAC-MD5
//

int vector_test_auth(int authtype)
{
	int ret;
    const u8* key;
	u32 keylen;
    const u8 *plaintext;
	u32 textlen;
	u8 *message;
    const u8 *digest;
    //u8 *pAsicDigest = &_AsicDigest[32];
	// force not 4 byte alignment
    u8 *pAsicDigest = &_AsicDigest[33];
	int digestlen = 0;


	// force not 4-byte alignment
	message = &_asic_orig[1];
	
	switch (authtype) {
   		case 0: // SHA1
			key = NULL;
			keylen = 0;
			rtl_memcpyb(message, _sha1_plaintext, sizeof(_sha1_plaintext));
			plaintext = message;
			textlen = strlen(plaintext);
			digest = _sha1_digest;
			digestlen = 20;
			ret = rtl_crypto_sha1(plaintext, strlen(plaintext), pAsicDigest);
			break;
			
		case 1: // SHA2
			key = NULL;
			keylen = 0;
			rtl_memcpyb(message, _sha2_plaintext, sizeof(_sha2_plaintext));
			plaintext = message;
			textlen = strlen(plaintext);
			digest = _sha2_digest;
			digestlen = 32;
			ret = rtl_crypto_sha2(SHA2_256, plaintext, strlen(plaintext), pAsicDigest);
			break;
			
        case 2: // MD5
            key = NULL;
			keylen = 0;
			rtl_memcpyb(message, _md5_plaintext, sizeof(_md5_plaintext));
			plaintext = message;
			textlen = strlen(plaintext);
            digest = _md5_digest;            
			digestlen = 16;
			ret = rtl_crypto_md5(plaintext, strlen(plaintext), pAsicDigest);
            break;
			
		case 4: // HMAC-SHA1
			key = _hmac_sha1_key;
			keylen = strlen(_hmac_sha1_key);
			rtl_memcpyb(message, _hmac_sha1_plaintext, sizeof(_hmac_sha1_plaintext));
			plaintext = message;
			textlen = strlen(_hmac_sha1_plaintext);
			digest = _hmac_sha1_digest;
			digestlen = 20;
			ret = rtl_crypto_hmac_sha1(plaintext, textlen, key, keylen, pAsicDigest);
			break;
			
		case 5: // HMAC-SHA256
			//digestlen = SHA2_DIGEST_LENGTH;
			key = _hmac_sha256_key;
			keylen = strlen(key);
			rtl_memcpyb(message, _hmac_sha256_plaintext, sizeof(_hmac_sha256_plaintext));
			plaintext = message;
			textlen = strlen(plaintext);
			digest = _hmac_sha256_digest;
			digestlen = 32;
			ret = rtl_crypto_hmac_sha2(SHA2_256, plaintext, textlen, key, keylen, pAsicDigest);
			break;
		
        case 6: // HMAC-MD5
            key = _hmac_md5_key;
			keylen = strlen(key);
			rtl_memcpyb(message, _hmac_md5_plaintext, sizeof(_hmac_md5_plaintext));
			plaintext = message;
			textlen = strlen(plaintext);
            digest = _hmac_md5_digest;
			digestlen = 16;
			ret = rtl_crypto_hmac_md5(plaintext, strlen(plaintext), key, keylen, pAsicDigest);
            break;

		case 7: 
			key = poly1305_key;
			keylen = sizeof(poly1305_key);
			plaintext = poly1305_plaintext;
			textlen = strlen(plaintext);
			digest = poly1305_digest;
			digestlen = 16;
			ret = SUCCESS;
			break;
			
        default:
            err_printk("authtype(%d) : no this type\n", authtype);
            return FAIL;
  }

   
	if ( rtl_memcmpb(pAsicDigest, digest, digestlen) == 0)
	{
			dbg_mem_dump(pAsicDigest, digestlen, "test OK - digest:");
	    return SUCCESS;
	} else {
	    err_printk("test failed!\n");
	    dbg_mem_dump(pAsicDigest, digestlen, "asic digest");
	}

    ret = rtl_crypto_auth_sim(authtype, key, keylen, plaintext, textlen, sw_digest, digestlen);
    if (ret != SUCCESS)
    {
        err_printk("ipsecSim failed!\n");
        return ret;
    }

    dbg_mem_dump(sw_digest, digestlen, "sw digest");

    if ( rtl_memcmpb(sw_digest, digest, digestlen) == 0)
    {
        printk("the same as sw_digest\n");
    }
    else
    {
        printk("IPSec failed!totally different\n");
        dbg_mem_dump(pAsicDigest, digestlen, "asic digest");
		dbg_mem_dump(sw_digest, digestlen, "sw digest");
		dbg_mem_dump(digest, digestlen, "correct digest");		
    }
	
    return FAIL;
}



int vector_test_auth_rand(void)
{

    int ret = SUCCESS;

    u8 *pAsicDigest = &_AsicDigest[32];

    u32 authtype;
    u32 keylen;
    u8 *key;
    u32 msglen;
    u8 *message;
    int i;

	u32 this_rtlseed;
//	HAL_CIPHER_ADAPTER *pIE;
	u32 digestlen = 0;

	this_rtlseed = rtl_random();
	//dbg_printk("seed=0x%x\n", this_rtlseed);


//	pIE = &g_rtl_IPSec_Engine;
	
	// 0 : SHA1, 1: SHA2, 2:MD5, 4:HMAC-SHA1, 5: HMAC-SHA2, 6:HMAC-MD5
	authtype = ( (rtl_random() % 2 )== 1)?
				 (rtl_random()%3) :
				 (rtl_random()%3)+4 ;

	// Suppose keylen = 0~ BLOCK(64)
	if ( authtype < 3 ) { // no HMAC
		keylen = 0;
		key = NULL;
	} else {
	  keylen = ( rtl_random() % 63 )+1;
	  key = &_asic_authkey[0];
	  for (i=0; i<keylen; i++)
	      key[i] = rtl_random() & 0xFF;
	  key[i] = '\0';
	}


	// Suppose msglen = 1~16000
	msglen = (rtl_random() % 16000)+1;

	//message = &_asic_orig[32];
	// force not 4byte alignment
	message = &_asic_orig[33];

	for (i=0; i<msglen; i++)
	  message[i] = rtl_random() & 0xFF;
	message[i] = '\0';


	switch (authtype) {
	case 2: 			
			digestlen = CRYPTO_MD5_DIGEST_LENGTH;
			ret = rtl_crypto_md5(message, msglen, pAsicDigest);
			if (ret != SUCCESS)
			{
				err_printk("failed!\n");
				return ret; 				
			}
			
		break;
	case 6:
			digestlen = CRYPTO_MD5_DIGEST_LENGTH;
			ret = rtl_crypto_hmac_md5(message, msglen, key, keylen, pAsicDigest);
			if (ret != SUCCESS)
			{
				err_printk("failed!\n");
				return ret; 				
			}
		break;
	case 0:
			digestlen = CRYPTO_SHA1_DIGEST_LENGTH;
			ret = rtl_crypto_sha1(message, msglen, pAsicDigest);
			if (ret != SUCCESS)
			{
				err_printk("failed!\n");
				return ret; 				
			}
			
		break;
	case 4:
			digestlen = CRYPTO_SHA1_DIGEST_LENGTH;
			ret = rtl_crypto_hmac_sha1(message, msglen, key, keylen, pAsicDigest);
			if (ret != SUCCESS)
			{
				err_printk("failed!\n");
				return ret; 				
			}
			
		break;
	case 1: 		
			digestlen = CRYPTO_SHA2_DIGEST_LENGTH;
			ret = rtl_crypto_sha2(SHA2_256, message, msglen, pAsicDigest);
			if (ret != SUCCESS)
			{
				err_printk("failed!\n");
				return ret; 				
			}
		break;
	case 5:
			digestlen = CRYPTO_SHA2_DIGEST_LENGTH;
			ret = rtl_crypto_hmac_sha2(SHA2_256, message, msglen, key, keylen, pAsicDigest);
			if (ret != SUCCESS)
			{
				err_printk("failed!\n");
				return ret; 				
			}
			
		break;
	default:
		err_printk("authtype(%d) : no this type\n", authtype);
		ret = FAIL;
	}



	ret = rtl_crypto_auth_sim(authtype, key, keylen, message, msglen, sw_digest, digestlen);
	if (ret != SUCCESS)
	{
	    err_printk("ipsecSim failed!\n");
	    ret = FAIL;
	    goto ret_vector_test_auth_rand;
	}

	if ( rtl_memcmpb(pAsicDigest, sw_digest, digestlen) == 0)
	{
	    dbg_printk("IPSec ok\n");
	    ret = SUCCESS;
	}
	else
	{
	    ret = FAIL;
	}

ret_vector_test_auth_rand:

	if ( ret == FAIL ) {
	    printk("IPSec failed!\n");
		err_printk("rtl_seed = 0x%x\n", this_rtlseed);
		err_printk("auth_type = 0x%x\n", authtype);
		err_printk("keylen = 0x%x\n", keylen);
		err_printk("msglen = 0x%x\n", msglen);
		err_printk("digestlen = 0x%x\n", digestlen);

	    memDump(key, keylen, "key: ");
	    memDump(message, msglen, "message: ");
	    memDump(pAsicDigest, digestlen, "asic digest: ");
	    memDump(sw_digest, digestlen, "sw digest: ");
	}

	return ret;
}



int vector_test_cipher_rand(void)
{
    int ret = FAIL;
	
	u32 cipher_type;
    u32 keylen, ivlen;
    u8 *key, *pIv;
    u32 msglen;
    u8 *message;
	u8 *message_sw;

    int i;

	u8 *pResult_asic;
	u8 *pResult_sw;

	u32 enl;
	u32 this_rtlseed;

	
	this_rtlseed = rtl_random();
//	this_rtlseed = 0xaa9b6229;
//	rtl_srandom(this_rtlseed);
	//	dbg_printk("seed=0x%x\n", this_rtlseed);

	switch (rtl_random() % 7) {
	    case 0:
			cipher_type = 0x00; // DES-CBC
			keylen = 8;			
			ivlen = keylen;
			break;
		case 1:
			cipher_type = 0x02; // DES-ECB
			keylen = 8;			
			ivlen = keylen;
			break;
		case 2:
			cipher_type = 0x10; // 3DES-CBC
			keylen = 24;
			ivlen = 8;
			break;
		case 3:
			cipher_type = 0x12; // 3DES-ECB
			keylen = 24;
			ivlen  = 8;
			break;
		case 4:
			cipher_type = 0x20; // AES-CBC, 128, 192, 256
			keylen = ((rtl_random() %3)+2)*8;
			ivlen = 16;
			break;
		case 5:
			cipher_type = 0x22; // AES-ECB, 128, 192, 256
			keylen = ((rtl_random() %3)+2)*8;
			ivlen = 0;
			break;
		case 6:
			cipher_type = 0x23; // AES-CTR 128, 192, 256
			keylen = ((rtl_random() %3)+2)*8;
			ivlen = 8;
			break;
	    default:
	        err_printk("no this cipher type\n");
	        return ret;
	}

	//printk("cipher_type=0x%x\n", cipher_type);


	key = &_asic_cryptkey[0];

	for (i=0; i<keylen; i++)
	      key[i] = rtl_random() & 0xFF;
	key[keylen] = '\0';
	

	// Suppose msglen = 1~16000
	msglen = ((rtl_random() % 1000)+1)*16;

	//message = &_asic_orig[32];
	// force not 4-byte alignment
	message = &_asic_orig[33];
	
	//memcpy(message, des3_test_buf, msglen);
	for (i=0; i<msglen; i++)
	  message[i] = rtl_random() & 0xFF;
	message[msglen] = '\0';

	message_sw = _sw_orig;
	rtl_memcpyb(message_sw, message, msglen);
	message_sw[msglen] = '\0';

	pIv = _asic_iv;
	//memcpy(pIv, des3_test_iv, 8);
	for (i=0; i<ivlen; i++)
		pIv[i] = rtl_random() & 0xFF;

	//pResult_asic = _asic_enc;		  
	// force result not 4-byte alignment	
	//pResult_asic = &_asic_enc[1];		
	// force result is message
	pResult_asic = message;		  
	pResult_sw	 = _sw_enc;

   
	// a2eo = 0;
	enl = msglen;

	switch ( cipher_type) {
		case 0x0: // DES-CBC
			ret = rtl_crypto_des_cbc_init(key,keylen);
			if ( ret != SUCCESS ) return ret;
			ret = rtl_crypto_des_cbc_encrypt(message, msglen, pIv, ivlen, pResult_asic);
			break;

		
		case 0x2: // DES-ECB
			ret = rtl_crypto_des_ecb_init(key,keylen);
			if ( ret != SUCCESS ) return ret;

			ret = rtl_crypto_des_ecb_encrypt(message, msglen, pIv, ivlen, pResult_asic);
			break;

		case 0x10: // 3DES-CBC
			ret = rtl_crypto_3des_cbc_init(key,keylen);
			if ( ret != SUCCESS ) return ret;
			
			ret = rtl_crypto_3des_cbc_encrypt(message, msglen, pIv, ivlen, pResult_asic);
			break;

		
		case 0x12: // 3DES-ECB
			ret = rtl_crypto_3des_ecb_init(key,keylen);
			if ( ret != SUCCESS ) return ret;

			ret = rtl_crypto_3des_ecb_encrypt(message, msglen, pIv, ivlen, pResult_asic);
			break;

		case 0x20: // AES-CBC
			ret = rtl_crypto_aes_cbc_init(key,keylen);
			if ( ret != SUCCESS ) return ret;
			
			ret = rtl_crypto_aes_cbc_encrypt(message, msglen, pIv, ivlen, pResult_asic);
			break;
			
		case 0x22: // AES-ECB
			ret = rtl_crypto_aes_ecb_init(key,keylen);
			if ( ret != SUCCESS ) return ret;
			
			ret = rtl_crypto_aes_ecb_encrypt(message, msglen, pIv, ivlen, pResult_asic);
			break;

		case 0x23: // AES-CTR
			ret = rtl_crypto_aes_ctr_init(key,keylen);
			if ( ret != SUCCESS ) return ret;
			
			ret = rtl_crypto_aes_ctr_encrypt(message, msglen, pIv, ivlen, pResult_asic);
			break;
			

		default : 
			err_printk("no this crypto_type=0x%x\n", cipher_type);
			break;
	}

	if (ret != SUCCESS)
	{
	   err_printk("ipsec encrypt failed!, ret=%d\n", ret);
	   ret = FAIL;
	   goto ret_vector_test_esp_rand;
	}
   
	ret = rtl_crypto_esp_sim(cipher_type |0x80, key, keylen, pIv, ivlen, message_sw, msglen, pResult_sw);
	if (ret != SUCCESS)
	{
	   err_printk("ipsecSim encrypt failed!\n");
	   memDump(pResult_asic, msglen, "ASIC enc msg: ");
	   ret = FAIL;
	   goto ret_vector_test_esp_rand;
	}
	   
	if ( rtl_memcmpb(pResult_asic, pResult_sw, enl) == 0 )
	{
	   dbg_mem_dump(pResult_asic, msglen, "test OK - enc:");
	} else {
	   err_printk("test failed!\n");
	   memDump(pResult_asic, msglen, "ASIC enc msg: ");
	   memDump(pResult_sw, msglen, "SW enc msg: ");
	   ret = FAIL;
	   goto ret_vector_test_esp_rand;
	}
   
	// Decryption

	message = pResult_asic;
	
	rtl_memcpyb(message_sw, message, msglen);
	message_sw[msglen] = '\0';

	//pResult_asic = _asic_dec;		 
	// force result not 4-byte alignment
	//pResult_asic = &_asic_dec[1];		 
	// force result is message
	pResult_asic = message;
	
	pResult_sw	= _sw_dec;

	switch ( cipher_type) {
		case 0x0: // DES-CBC
			ret = rtl_crypto_des_cbc_decrypt(message, msglen, pIv, ivlen, pResult_asic);
			break;

		
		case 0x2: // DES-ECB
			ret = rtl_crypto_des_ecb_decrypt(message, msglen, pIv, ivlen, pResult_asic);
			break;

		case 0x10: // 3DES-CBC
			ret = rtl_crypto_3des_cbc_decrypt(message, msglen, pIv, ivlen, pResult_asic);
			break;

		
		case 0x12: // 3DES-ECB
			ret = rtl_crypto_3des_ecb_decrypt(message, msglen, pIv, ivlen, pResult_asic);
			break;

		case 0x20: // AES-CBC
			ret = rtl_crypto_aes_cbc_decrypt(message, msglen, pIv, ivlen, pResult_asic);
			break;
			
		case 0x22: // AES-ECB
			ret = rtl_crypto_aes_ecb_decrypt(message, msglen, pIv, ivlen, pResult_asic);
			break;

		case 0x23: // AES-CTR
			ret = rtl_crypto_aes_ctr_decrypt(message, msglen, pIv, ivlen, pResult_asic);
			break;
			

		default : 
			err_printk("no this crypto_type=0x%x\n", cipher_type);
			break;
	}
		
	if (ret != SUCCESS)
	{
	   err_printk("ipsec decrypt failed!\n");
	   goto ret_vector_test_esp_rand;
	}


	ret = rtl_crypto_esp_sim(cipher_type, key, keylen, pIv, ivlen, message_sw, msglen, pResult_sw);
	if (ret != SUCCESS)
	{
	   err_printk("ipsecSim decrypt failed!\n");
	   memDump(pResult_asic, msglen, "ASIC dec msg: ");
	   goto ret_vector_test_esp_rand;
	}

	if ( rtl_memcmpb(pResult_asic, pResult_sw, msglen) == 0 )
	{
	   dbg_mem_dump(pResult_asic, msglen, "test OK - dec:");
	   ret = SUCCESS;
	} else {
	   err_printk("test failed!\n");
	   
	   memDump(pResult_asic, msglen, "ASIC dec msg: ");
	   
	   ret = FAIL;
	   goto ret_vector_test_esp_rand;
	}

	return SUCCESS;

ret_vector_test_esp_rand: 
	
	err_printk("message (%d) : \n", msglen);
	memDump(message, msglen, "MSG:");
	
	err_printk("this seed=0x%x\n", this_rtlseed);
	err_printk("cipher_type=0x%x\n", cipher_type);
	
	err_printk("key (%d) : \n", keylen);
	memDump(key, keylen, "key:");
	
	err_printk("IV (%d) : \n", ivlen);
	memDump(pIv, ivlen, "IV:");

	return ret;
}


// Single DES Encryption in ECB mode
// 
//char *crypto_msg = "Now is the time for all ";
//int crypto_msglen = 24;
//char *crypto_key = "0123456789ABCDEF";
//int crypto_keylen = 16;
//char *crypto_iv =  "1234567890ABCDEF";



//char *crypto_msg = "The quick brown fox jumped over the lazy dog";
//char *crypto_key = "1234567890123456ABCDEFGH";
//int crypto_keylen = 24;
//char *crypto_iv =  "";
//int crypto_iv_len = 0;


//char *crypto_msg = "Now is the time for all ";
//int crypto_msglen = 24;
//char *crypto_key = "0123456789ABCDEF";
//int crypto_keylen = 16;
//char *crypto_iv =  "1234567890ABCDEF";


// AES 128

//char *crypto_msgkey = "\x06\xa9\x21\x40\x36\xb8\xa1\x5b"
//	  "\x51\x2e\x03\xd5\x34\x12\x00\x06";

//char *crypto_iv = "\x3d\xaf\xba\x42\x9d\x9e\xb4\x30"
//	  "\xb4\x22\xda\x80\x2c\x9f\xac\x41";

//char *crypto_encoded = "\xe3\x53\x77\x9c\x10\x79\xae\xb8"
//	  "\x27\x08\x94\x2d\xbe\x77\x18\x1a";

//char *crypto_text = "Single block msg";

//u8 _asic_enc[32 + 128 + 32];


int vector_test_cipher_3DES(u32 cipher_type)
{
    const u8 *key, *pIv;
	u32 keylen, ivlen;
    u8 *message;
    u8 *pResult_asic;
	u8 *pResult_sw;
	//const u8 *pResult_correct;
    int ret;
	u32 msglen, enl;
	//	u32 a2eo;
	//	u32 modeSelect;
	//	HAL_CIPHER_ADAPTER *pIE;


	
	// Encryption

	key = des3_test_keys;
	keylen = 24;
	//message = &_asic_orig[32];
	// force not 4-byte alignmnet
	message = &_asic_orig[33];
	msglen = sizeof(des3_test_buf);
	rtl_memcpyb(message, des3_test_buf, msglen);
    pIv = des3_test_iv;
	ivlen = 8;

	// force not 4-byte alignment
	//pResult_asic = _asic_enc;		  
	pResult_asic = &_asic_enc[1];		  
	pResult_sw   = _sw_enc;
	//pResult_correct = des3_test_buf_encoded;

	//modeSelect = _MS_CRYPTO;

//   Note: a2eo is only for athentication to encryption, can not set a2eo in only crypto
//	a2eo = 32;
//   message = _asic_orig;
	//a2eo = 0;
	enl = msglen;


 	switch ( cipher_type) {
		case 0x10: // 3DES-CBC
			rtl_crypto_3des_cbc_init(key,keylen);
			ret = rtl_crypto_3des_cbc_encrypt(message, msglen, pIv, ivlen, pResult_asic);
			break;

		
		case 0x12 : // 3DES-ECB
			rtl_crypto_3des_ecb_init(key,keylen);
			ret = rtl_crypto_3des_ecb_encrypt(message, msglen, pIv, ivlen, pResult_asic);
			break;
			

		default : 
			err_printk("no this crypto_type=0x%x\n", cipher_type);
			ret = _ERRNO_CRYPTO_CIPHER_TYPE_NOT_MATCH;
			break;
	}

	if (ret != SUCCESS)
	{
		err_printk("ipsec encrypt failed!\n");
		return ret;
	}
 

 	ret = rtl_crypto_esp_sim(cipher_type |0x80, key, keylen, pIv, ivlen, message, msglen, pResult_sw);
	if (ret != SUCCESS)
	{
		err_printk("ipsecSim encrypt failed!\n");
		return ret;
	}
 	
	if ( rtl_memcmpb(pResult_asic, pResult_sw, enl) == 0 )
	{
		memDump(pResult_asic, msglen, "test OK - enc:");
	} else {
		err_printk("test failed!\n");
		dbg_mem_dump(pResult_asic, msglen, "ASIC enc msg: ");
		dbg_mem_dump(pResult_sw, msglen, "SW enc msg: ");
		return FAIL;
	}

	// Decryption

    message = pResult_asic;

	//pResult_asic = _asic_dec;		  
	// force not 4-byte alignment
	pResult_asic = &_asic_dec[1];		  
	pResult_sw   = _sw_dec;
	//pResult_correct = des3_test_buf;

	switch ( cipher_type) {
		case 0x10: // 3DES-CBC
			ret = rtl_crypto_3des_cbc_decrypt(message, msglen, pIv, ivlen, pResult_asic);
			break;

		
		case 0x12 : // 3DES-ECB
			ret = rtl_crypto_3des_ecb_decrypt(message, msglen, pIv, ivlen, pResult_asic);
			break;
			

		default : 
			err_printk("no this crypto_type=0x%x\n", cipher_type);
			break;
	}

	if (ret != SUCCESS)
	{
		err_printk("ipsec encrypt failed!\n");
		return ret;
	}

	ret = rtl_crypto_esp_sim(cipher_type, key, keylen, pIv, ivlen, message, msglen, pResult_sw);
	if (ret != SUCCESS)
	{
		err_printk("ipsecSim encrypt failed!\n");
		return ret;
	}
	
	if ( rtl_memcmpb(pResult_asic, pResult_sw, msglen) == 0 )
	{
		dbg_mem_dump(pResult_asic, msglen, "test OK - dec:");
		return SUCCESS;
	} else {
		err_printk("test failed!\n");
		dbg_mem_dump(pResult_asic, msglen, "ASIC dec msg: ");
		dbg_mem_dump(pResult_sw, msglen, "SW dec msg: ");
		return FAIL;
	}


}


//
// http://www.inconteam.com/software-development/41-encryption/55-aes-test-vectors
// 


// 128 bit - CBC

__attribute__ ((aligned (4)))
static const unsigned char aes_test_key[16] =
{
	0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
	0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
} ;


__attribute__ ((aligned (4)))
static const unsigned char aes_test_iv_1[16] =
{
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 
	0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
};



__attribute__ ((aligned (4)))
static const unsigned char aes_test_buf[16] =
{
	0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
	0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a
};



__attribute__ ((aligned (4)))
static const unsigned char aes_test_res_128[16] =
{
	0x76, 0x49, 0xab, 0xac, 0x81, 0x19, 0xb2, 0x46,
	0xce, 0xe9, 0x8e, 0x9b, 0x12, 0xe9, 0x19, 0x7d
};


// 192 bit - CBC 

__attribute__ ((aligned (4)))
static const unsigned char aes_test_key_192[24] =
{
	0x8e, 0x73, 0xb0, 0xf7, 0xda, 0x0e, 0x64, 0x52,
	0xc8, 0x10, 0xf3, 0x2b, 0x80, 0x90, 0x79, 0xe5,
	0x62, 0xf8, 0xea, 0xd2, 0x52, 0x2c, 0x6b, 0x7b
};



__attribute__ ((aligned (4)))
static const unsigned char aes_test_res_192_cbc[16] =
{
	0x4f, 0x02, 0x1d, 0xb2, 0x43, 0xbc, 0x63, 0x3d,
	0x71, 0x78, 0x18, 0x3a, 0x9f, 0xa0, 0x71, 0xe8
};


// 256 bit - CBC

__attribute__ ((aligned (4)))
static const unsigned char aes_test_key_256[32] =
{
	0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe,
	0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81,
	0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7,
	0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4
};



__attribute__ ((aligned (4)))
static const unsigned char aes_test_res_256_cbc[16] =
{
	0xf5, 0x8c, 0x4c, 0x04, 0xd6, 0xe5, 0xf1, 0xba,
	0x77, 0x9e, 0xab, 0xfb, 0x5f, 0x7b, 0xfb, 0xd6
};


// ECB

__attribute__ ((aligned (4)))
static const unsigned char aes_test_res_128_ECB[16] =
{
	0x3a, 0xd7, 0x7b, 0xb4, 0x0d, 0x7a, 0x36, 0x60,
	0xa8, 0x9e, 0xca, 0xf3, 0x24, 0x66, 0xef, 0x97
};


__attribute__ ((aligned (4)))
static const unsigned char aes_test_res_192_ECB[16] =
{
	0xbd, 0x33, 0x4f, 0x1d, 0x6e, 0x45, 0xf2, 0x5f,
	0xf7, 0x12, 0xa2, 0x14, 0x57, 0x1f, 0xa5, 0xcc
};


__attribute__ ((aligned (4)))
static const unsigned char aes_test_res_256_ECB[16] =
{
	0xf3, 0xee, 0xd1, 0xbd, 0xb5, 0xd2, 0xa0, 0x3c,
	0x06, 0x4b, 0x5a, 0x7e, 0x3d, 0xb1, 0x81, 0xf8
};



// CTR

__attribute__ ((aligned (4)))
static const unsigned char aes_test_iv[16] =
{
	0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
	0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
};


__attribute__ ((aligned (4)))
static const unsigned char aes_test_res_128_CTR[16] =
{
	0x87, 0x4d, 0x61, 0x91, 0xb6, 0x20, 0xe3, 0x26,
	0x1b, 0xef, 0x68, 0x64, 0x99, 0x0d, 0xb6, 0xce
};


__attribute__ ((aligned (4)))
static const unsigned char aes_test_res_192_CTR[16] =
{
	0x1a, 0xbc, 0x93, 0x24, 0x17, 0x52, 0x1c, 0xa2,
	0x4f, 0x2b, 0x04, 0x59, 0xfe, 0x7e, 0x6e, 0x0b
};



int vector_test_cipher_AES(u32 cipher_type)
{
    const u8 *key, *pIv;
	u32 keylen= 0;
	u32 ivlen = 0;
    u8 *message;
    u8 *pResult_asic;
	u8 *pResult_sw;
	//const u8 *pResult_correct = aes_test_res_128;
    int ret;
	u32 msglen, enl;
	//u32 a2eo
//	u32 modeSelect;
//	HAL_CIPHER_ADAPTER *pIE;

//	pIE = &g_rtl_IPSec_Engine;
	
	// Encryption

	key = aes_test_key;
	keylen = sizeof(aes_test_key);
	//message = &_asic_orig[32];
	// force message not 4-byte alignment
	message = &_asic_orig[33];
	msglen = sizeof(aes_test_buf);
	rtl_memcpyb(message, aes_test_buf, msglen);
    pIv = aes_test_iv;
//	ivlen = ( (cipher_type & _MASK_CBCECBCTR) == 0x2 ) ? 0 : sizeof(aes_test_iv);
	

	//pResult_asic = _asic_enc;		  
	// force not 4-byte alignment
	pResult_asic = &_asic_enc[1];		  
	pResult_sw   = _sw_enc;

//	modeSelect = _MS_CRYPTO;

//	a2eo = 32;
//   message = _asic_orig;
	//a2eo = 0;
	enl = msglen;



	switch ( cipher_type) {
		//case CIPHER_TYPE_3DES_CBC: // 3DES-CBC
		//case CIPHER_TYPE_3DES_ECB : // 3DES-ECB
			//ret = vector_test_esp_3DES(crypto_type);
			//break;
			
		case 0x20: // AES-CBC
			pIv = aes_test_iv_1;
			ivlen = sizeof(aes_test_iv);
			ret = rtl_crypto_aes_cbc_init(key,keylen);
			if ( ret != SUCCESS ) return ret;
			ret = rtl_crypto_aes_cbc_encrypt(message, msglen, pIv, ivlen, pResult_asic);
			//pResult_correct = aes_test_res_128;
			break;
			
		case 0x22 : // AES-ECB
			ret = rtl_crypto_aes_ecb_init(key,keylen);
			if ( ret != SUCCESS ) return ret;
			ivlen = 0;
			ret = rtl_crypto_aes_ecb_encrypt(message, msglen, pIv, ivlen, pResult_asic);
			//pResult_correct = aes_test_res_128_ECB;
			break;

		case 0x23 : // AES-CTR
			ret = rtl_crypto_aes_ctr_init(key,keylen);
			if ( ret != SUCCESS ) return ret;
			ivlen = sizeof(aes_test_iv);
			ret = rtl_crypto_aes_ctr_encrypt(message, msglen, pIv, ivlen, pResult_asic);
			//pResult_correct = aes_test_res_128_CTR;
			break;

		case 0x24 : // AES-CFB 
			//ret = rtl_crypto_aes_cfb_init(key,keylen);
			//if ( ret != SUCCESS ) return ret;
			pIv = aes_test_iv_1;
			ivlen = sizeof(aes_test_iv_1);
//			ret = rtl_crypto_aes_cfb_encrypt(message, msglen, pIv, ivlen, pResult_asic);
			ret = SUCCESS;
			// AES-CFB
			// enc: 3b 3f d9 2e b7 2d ad 20   33 34 49 f8 e8 3c fb 4a 
			break;

		case 0x25 : // AES-OFB
			pIv = aes_test_iv_1;
			ivlen = sizeof(aes_test_iv_1);
			ret = SUCCESS;
			// AES-CFB
			// enc: 3b 3f d9 2e b7 2d ad 20   33 34 49 f8 e8 3c fb 4a 
			// AES-OFB
			// enc: 3b 3f d9 2e b7 2d ad 20   33 34 49 f8 e8 3c fb 4a 
			break;

			
		default : 
			err_printk("no this crypto_type=0x%x\n", cipher_type);
			ret = _ERRNO_CRYPTO_CIPHER_TYPE_NOT_MATCH;
			break;
	}
	
	if (ret != SUCCESS)
	{
		err_printk("ipsec encrypt failed!, ret=%d\n", ret);
		return ret;
	}


#if 1
	ret = rtl_crypto_esp_sim(cipher_type |0x80, key, keylen, pIv, ivlen, message, msglen, pResult_sw);
	if (ret != SUCCESS)
	{
		err_printk("ipsecSim encrypt failed!\n");
		return ret;
	}
#endif
	
	if ( rtl_memcmpb(pResult_asic, pResult_sw, enl) == 0 )
	{
		dbg_mem_dump(pResult_asic, msglen, "test OK - enc:");
	} else {
		err_printk("test failed!\n");
		dbg_mem_dump(pResult_asic, msglen, "ASIC enc msg: ");
		dbg_mem_dump(pResult_sw, msglen, "SW enc msg: ");
		return FAIL;
	}

	// Decryption

    message = pResult_asic;

	//pResult_asic = _asic_dec;		  
	// force not 4-byte alignment
	pResult_asic = &_asic_dec[1];		  
	pResult_sw   = _sw_dec;
	//pResult_correct = aes_test_buf;


	switch ( cipher_type) {
		//case CIPHER_TYPE_3DES_CBC: // 3DES-CBC
		//case CIPHER_TYPE_3DES_ECB : // 3DES-ECB
			//ret = vector_test_esp_3DES(crypto_type);
			//break;
			
		case 0x20: // AES-CBC
			ret = rtl_crypto_aes_cbc_decrypt(message, msglen, pIv,ivlen, pResult_asic);
			break;
			
		case 0x22 : // AES-ECB
			ret = rtl_crypto_aes_ecb_decrypt(message, msglen, pIv,ivlen, pResult_asic);
			break;

		case 0x23 : // AES-CTR
//			ret = rtl_crypto_aes_ctr_decrypt(message, msglen, pIv,ivlen, pResult_asic);
			break;

		case 0x24 : // AES-CFB 
//			ret = rtl_crypto_aes_cfb_decrypt(message, msglen, pIv, ivlen, pResult_asic);
			break;
			
		default : 
			err_printk("no this crypto_type=0x%x\n", cipher_type);
			break;
	}

	if (ret != SUCCESS)
	{
		err_printk("ipsec decrypt failed!\n");
		return ret;
	}


	

#if 1
	ret = rtl_crypto_esp_sim(cipher_type, key, keylen, pIv, ivlen, message, msglen, pResult_sw);
	if (ret != SUCCESS)
	{
		err_printk("ipsecSim encrypt failed!\n");
		return ret;
	}
#endif
	
	if ( rtl_memcmpb(pResult_asic, pResult_sw, msglen) == 0 )
	{
		dbg_mem_dump(pResult_asic, msglen, "test OK - dec:");
		return SUCCESS;
	} else {
		err_printk("test failed!\n");
		dbg_mem_dump(pResult_asic, msglen, "ASIC dec msg: ");
		dbg_mem_dump(pResult_sw, msglen, "SW dec msg: ");
		return FAIL;
	}


}



//
// http://tools.ietf.org/html/draft-nir-cfrg-chacha20-poly1305-06
//
// 2.4.1 example and test vector 
//



__attribute__ ((aligned (4)))
static const unsigned char chacha_test_key[32] = { 0 };
static const unsigned char chacha_test_buf[64] = { 0 };
static const unsigned char chacha_test_iv[8] = { 0 };

static const unsigned char chacha_test_res[64] ={

0x76, 0xb8, 0xe0, 0xad, 0xa0, 0xf1, 0x3d, 0x90,
0x40, 0x5d, 0x6a, 0xe5, 0x53, 0x86, 0xbd, 0x28, 
0xbd, 0xd2, 0x19, 0xb8, 0xa0, 0x8d, 0xed, 0x1a,
0xa8, 0x36, 0xef, 0xcc, 0x8b, 0x77, 0x0d, 0xc7,

0xda, 0x41, 0x59, 0x7c, 0x51, 0x57, 0x48, 0x8d,
0x77, 0x24, 0xe0, 0x3f, 0xb8, 0xd8, 0x4a, 0x37,
0x6a, 0x43, 0xb8, 0xf4, 0x15, 0x18, 0xa1, 0x1c,
0xc3, 0x87, 0xb6, 0x69, 0xb2, 0xee, 0x65, 0x86 };


int vector_test_cipher_chacha(u32 cipher_type)
{
    const u8 *key, *pIv;
//	u32 keylen= 0;
//	u32 ivlen = 0;
    u8 *message;
//    u8 *pResult_asic;
	u8 *pResult_sw;
    int ret;
	u32 msglen, enl;
	
	// Encryption

	key = chacha_test_key;
//	keylen = sizeof(chacha_test_key);
	message = &_asic_orig[32];
	msglen = sizeof(chacha_test_buf);
	rtl_memcpyb(message, chacha_test_buf, msglen);
    pIv = chacha_test_iv;
//	ivlen = sizeof(chacha_test_iv);	

//	pResult_asic = _asic_enc;		  
	pResult_sw   = _sw_enc;

	enl = msglen;

	switch ( cipher_type) {
		case 0x30: // chacha
			ret = SUCCESS;
			// chacha
			break;

			
		default : 
			err_printk("no this crypto_type=0x%x\n", cipher_type);
			ret = _ERRNO_CRYPTO_CIPHER_TYPE_NOT_MATCH;
			break;
	}
	
	if (ret != SUCCESS)
	{
		err_printk("ipsec encrypt failed!, ret=%d\n", ret);
		return ret;
	}


#if 1
	CRYPTO_sim_chacha_20(pResult_sw, message, msglen, key, pIv, 0);

	//ret = rtl_crypto_esp_sim(cipher_type |0x80, key, keylen, pIv, ivlen, message, msglen, pResult_sw);
#endif
	
	if ( rtl_memcmpb(chacha_test_res, pResult_sw, enl) == 0 )
	{
		printk("test OK\n");
		dbg_mem_dump(pResult_sw, msglen, "test OK - enc:");
		ret = SUCCESS;
	} else {
		err_printk("test failed!\n");
		dbg_mem_dump(pResult_sw, msglen, "SW enc msg: ");
		return FAIL;
	}

	// Decryption

#if 0
    message = pResult_asic;

	pResult_asic = _asic_dec;		  
	pResult_sw   = _sw_dec;
	//pResult_correct = aes_test_buf;


	switch ( cipher_type) {

		case 0x30 : // chacha 
			ret = SUCCESS;
			break;
			
		default : 
			err_printk("no this crypto_type=0x%x\n", cipher_type);
			break;
	}

	if (ret != SUCCESS)
	{
		err_printk("ipsec decrypt failed!\n");
		return ret;
	}


	

#if 1
	ret = rtl_crypto_esp_sim(cipher_type, key, keylen, pIv, ivlen, message, msglen, pResult_sw);
	if (ret != SUCCESS)
	{
		err_printk("ipsecSim encrypt failed!\n");
		return ret;
	}
#endif
	
	if ( rtl_memcmpb(pResult_asic, pResult_sw, msglen) == 0 )
	{
		dbg_mem_dump(pResult_asic, msglen, "test OK - dec:");
		return SUCCESS;
	} else {
		err_printk("test failed!\n");
		dbg_mem_dump(pResult_asic, msglen, "ASIC dec msg: ");
		dbg_mem_dump(pResult_sw, msglen, "SW dec msg: ");
		return FAIL;
	}
#endif

	return ret;

}


// DES-CBC   : 0x00
// DES-ECB   :  0x02
// 3DES-CBC : 0x10
// 3DES-ECB  : 0x12
//

int vector_test_cipher(u32 cipher_type)
{
	int ret = FAIL;

	switch ( cipher_type) {
		case 0x10: // 3DES-CBC
		case 0x12 : // 3DES-ECB
			ret = vector_test_cipher_3DES(cipher_type);
			break;
		case 0x20: // AES-CBC
		case 0x22 : // AES-ECB
		case 0x23 : // AES-CTR
		case 0x24 : // AES-CFB
		case 0x25 : // AES-OFB
			ret = vector_test_cipher_AES(cipher_type);
			break;
		case 0x30 : // Chacha
			ret = vector_test_cipher_chacha(cipher_type);
			break;		
		default : 
			err_printk("no this crypto_type=0x%x\n", cipher_type);
			break;
	}

	return ret;
		
}


#if 0
static rtl_ipsecScatter_t scatter_th[1];
static u8 _cryptoKey_th[32 + 32 + 32]; // 32 for AES-256

//		throughput 0 0 -1 -> DES-CBC : 0x0
//		throughput 0 2 -1 -> DES-ECB : 0x2
//		throughput 0 16 -1 -> 3DES-CBC : 0x10
//		throughput 0 18 -1 -> 3DES-ECB : 0x12 
//		throughput 0 32 -1 -> AES-CBC :   0x20
//		throughput 0 34 -1 -> AES-ECB  :  0x22
//		throughput 0 35 -1 -> AES-CTR  : 0x23

#endif 

int throughput_test_cipher(u32 cipher_type, 
    void* key, u32 keylen, u8* iv, int ivlen, 
    u32 digestlen, u8 *message, int msglen, 
    void *pResult, u32 resultLen, int loops) 
{
	int loop, ret;
//	u32 start_ticks, end_ticks, spend_ticks, spend_time, throughput;
    u32 start_count, current_count, spend_time, spend_ticks, throughput;
	
	printk("%s start\n", __FUNCTION__);
	//start_ticks = xTaskGetTickCount();
	start_count = HalTimerOp.HalTimerReadCount(1);

	switch (cipher_type) {
		case 0x0: // DES-CBC				
			for (loop=0; loop<loops; loop++) {
				ret = rtl_crypto_des_cbc_init(key, keylen); 
				if (ret != SUCCESS)
				{
					err_printk("init failed!\n");
					return ret; 				
				}

				ret = rtl_crypto_des_cbc_encrypt(message, msglen, iv, ivlen, pResult);
				if (ret != SUCCESS)
				{
					err_printk("failed!\n");
					return ret;					
				}
			}
			break;
			
		case 0x2: // DES-ECB
			for (loop=0; loop<loops; loop++) {
				ret = rtl_crypto_des_ecb_init(key, keylen); 
				if (ret != SUCCESS)
				{
					err_printk("init failed!\n");
					return ret; 				
				}

				ret = rtl_crypto_des_ecb_encrypt(message, msglen, iv, ivlen, pResult);
				if (ret != SUCCESS)
				{
					err_printk("failed!\n");
					return ret;					
				}
			}
			break;

		case 0x10: // 3DES-CBC
			for (loop=0; loop<loops; loop++) {
				ret = rtl_crypto_3des_cbc_init(key, keylen); 
				if (ret != SUCCESS)
				{
					err_printk("init failed!\n");
					return ret; 				
				}

				ret = rtl_crypto_3des_cbc_encrypt(message, msglen, iv, ivlen, pResult);
				if (ret != SUCCESS)
				{
					err_printk("failed!\n");
					return ret;					
				}
			}
			break;
			
		case 0x12: // 3DES-ECB
			for (loop=0; loop<loops; loop++) {
				ret = rtl_crypto_3des_ecb_init(key, keylen); 
				if (ret != SUCCESS)
				{
					err_printk("init failed!\n");
					return ret; 				
				}

				ret = rtl_crypto_3des_ecb_encrypt(message, msglen, iv, ivlen, pResult);
				if (ret != SUCCESS)
				{
					err_printk("failed!\n");
					return ret;					
				}
			}
			break;


		case 0x20: // AES-CBC
			
			for (loop=0; loop<loops; loop++) {
				ret = rtl_crypto_aes_cbc_init(key, keylen); 
				if (ret != SUCCESS)
				{
					err_printk("init failed!\n");
					return ret; 				
				}

				ret = rtl_crypto_aes_cbc_encrypt(message, msglen, iv, ivlen, pResult);
				if (ret != SUCCESS)
				{
					err_printk("failed!\n");
					return ret;					
				}
			}
			break;
			
		case 0x22: // AES-ECB
			for (loop=0; loop<loops; loop++) {
				ret = rtl_crypto_aes_ecb_init(key, keylen); 
				if (ret != SUCCESS)
				{
					err_printk("init failed!\n");
					return ret; 				
				}

				ret = rtl_crypto_aes_ecb_encrypt(message, msglen, iv, ivlen, pResult);
				if (ret != SUCCESS)
				{
					err_printk("failed! ret=%d\n", ret);
					return ret;					
				}
			}
			break;

		case 0x23: // AES-CTR
			for (loop=0; loop<loops; loop++) {
				ret = rtl_crypto_aes_ctr_init(key, keylen); 
				if (ret != SUCCESS)
				{
					err_printk("init failed!\n");
					return ret; 				
				}

				ret = rtl_crypto_aes_ctr_encrypt(message, msglen, iv, ivlen, pResult);
				if (ret != SUCCESS)
				{
					err_printk("failed!\n");
					return ret;					
				}
			}
			break;

		case 0x24 : // AES-CFB 
			for (loop=0; loop<loops; loop++) {
				//ret = rtl_crypto_aes_cfb_init(key, keylen); 
				if (ret != SUCCESS)
				{
					err_printk("init failed!\n");
					return ret; 				
				}

				//ret = rtl_crypto_aes_cfb_encrypt(message, msglen, iv, ivlen, pResult);
				if (ret != SUCCESS)
				{
					err_printk("failed!\n");
					return ret;					
				}
			}
			break;

		case 0x25 : // AES-OFB
		case 0x40 : // CHACHA20
			ret = SUCCESS;
			break;

		
		default:
			err_printk("authtype(%d) : no this type\n", cipher_type);
			ret = FAIL;
	}

		
	//end_ticks = xTaskGetTickCount();
	current_count = HalTimerOp.HalTimerReadCount(1);	
	if ( start_count < current_count) {
        spend_ticks =  (0xFFFFFFFF - current_count) + start_count;
    }
    else {
        spend_ticks = start_count - current_count;            
    }
    

	
	//spend_ticks = end_ticks - start_ticks;
	//spend_time = spend_ticks * portTICK_RATE_MS;
	spend_time = spend_ticks * 30 / loops;
	throughput = (msglen*8) / spend_time;
	printk("HW spend time each:%d, ticks:%d, throughput: %d Mbps\n", 
		spend_time, spend_ticks, throughput);

	//
	// run SW simulation
	//
	//start_ticks = xTaskGetTickCount();
	start_count = HalTimerOp.HalTimerReadCount(1);

	for (loop=0; loop<loops; loop++) {
		ret = rtl_crypto_esp_sim(cipher_type |0x80, key, keylen, iv, ivlen, message, msglen, pResult);
	}
	
	//end_ticks = xTaskGetTickCount();
	current_count = HalTimerOp.HalTimerReadCount(1);	
	if ( start_count < current_count) {
        spend_ticks =  (0xFFFFFFFF - current_count) + start_count;
    }
    else {
        spend_ticks = start_count - current_count;            
    }

	//spend_ticks = end_ticks - start_ticks;
	//spend_time = spend_ticks * portTICK_RATE_MS;
	
	spend_time = spend_ticks * 30 / loops;
	throughput = (msglen*8) / spend_time;
	printk("SW spend time each:%d, ticks:%d, throughput: %d Mbps\n", 
		spend_time, spend_ticks, throughput);

	return ret;
}


int throughput_test_auth(u32 authtype, 
    void* key, u32 keylen, 
    u32 digestlen, u8 *message, int msglen, 
    void *pResult, u32 resultLen, int loops) 
{
	int loop, ret = SUCCESS;
	//u32 start_ticks, end_ticks, spend_ticks, spend_time, throughput;
    u32 start_count, current_count, spend_time, spend_ticks, throughput;

	
	printk("%s start\n", __FUNCTION__);
	//start_ticks = xTaskGetTickCount();
	start_count = HalTimerOp.HalTimerReadCount(1);

		switch (authtype) {
			case 2:				
				for (loop=0; loop<loops; loop++) {
					ret = rtl_crypto_md5_init();
					if (ret != SUCCESS)
					{
						err_printk("init failed!\n");
						return ret; 				
					}
					

					ret = rtl_crypto_md5_process(message, msglen, pResult);
					if (ret != SUCCESS)
					{
						err_printk("failed!\n");
						return ret;					
					}
				}
				break;
				
			case 6:
				for (loop=0; loop<loops; loop++) {
					ret = rtl_crypto_hmac_md5_init(key, keylen);
					if (ret != SUCCESS)
					{
						err_printk("init failed!, ret=%d\n", ret);
						return ret; 				
					}
					

					ret = rtl_crypto_hmac_md5_process(message, msglen, pResult);
					if (ret != SUCCESS)
					{
						err_printk("failed!, ret = %d\n", ret);
						return ret;					
					}
				}
				break;

			case 0:
				
				for (loop=0; loop<loops; loop++) {
					ret = rtl_crypto_sha1_init();
					if (ret != SUCCESS)
					{
						err_printk("init failed! ret=%d\n", ret);
						return ret; 				
					}

					ret = rtl_crypto_sha1_process(message, msglen, pResult);
					if (ret != SUCCESS)
					{
						err_printk("failed!, ret=%d\n", ret);
						return ret;					
					}
				}
				break;
				
			case 4:
				
				for (loop=0; loop<loops; loop++) {
					ret = rtl_crypto_hmac_sha1_init(key, keylen);
					if (ret != SUCCESS)
					{
						err_printk("init failed!\n");
						return ret; 				
					}
					
					ret = rtl_crypto_hmac_sha1_process(message, msglen, pResult);
					if (ret != SUCCESS)
					{
						err_printk("failed!\n");
						return ret;					
					}
				}
				break;
				
			case 1: 		
				for (loop=0; loop<loops; loop++) {
					ret = rtl_crypto_sha2_init(SHA2_256);
					if (ret != SUCCESS)
					{
						err_printk("init failed!\n");
						return ret; 				
					}
					
					ret = rtl_crypto_sha2_process(message, msglen, pResult);
					if (ret != SUCCESS)
					{
						err_printk("failed!\n");
						return ret;					
					}
				}
				break;
				
			case 5:
				for (loop=0; loop<loops; loop++) {
					ret = rtl_crypto_hmac_sha2_init(SHA2_256, key, keylen);
					if (ret != SUCCESS)
					{
						err_printk("init failed!\n");
						return ret; 				
					}
					
					
					ret = rtl_crypto_hmac_sha2_process(message, msglen, pResult);
					if (ret != SUCCESS)
					{
						err_printk("failed!\n");
						return ret;					
					}
				}
				break;

			case 7 : //poly1305
				ret = SUCCESS;
				break;
				
			default:
				err_printk("authtype(%d) : no this type\n", authtype);
				ret = FAIL;
		}
		
	//end_ticks = xTaskGetTickCount();
	current_count = HalTimerOp.HalTimerReadCount(1);	
	if ( start_count < current_count) {
        spend_ticks =  (0xFFFFFFFF - current_count) + start_count;
    }
    else {
        spend_ticks = start_count - current_count;            
    }
	
	//spend_ticks = end_ticks - start_ticks;
	//spend_time = spend_ticks * portTICK_RATE_MS;
	
	spend_time = spend_ticks * 30 / loops;
	throughput = (msglen*8) / spend_time;
	printk("HW spend time each:%d, ticks:%d, throughput: %d Mbps\n", 
		spend_time, spend_ticks, throughput);
	

	//
	// test sofware 
	//

	//start_ticks = xTaskGetTickCount();
	start_count = HalTimerOp.HalTimerReadCount(1);
	
	for (loop=0; loop<loops; loop++) {
	    ret = rtl_crypto_auth_sim(authtype, key, keylen, message, msglen, pResult, digestlen);
		if (ret != SUCCESS)
		{
			err_printk("failed!\n");
			return ret; 				
		}
	}
	//end_ticks = xTaskGetTickCount();
	current_count = HalTimerOp.HalTimerReadCount(1);	
	if ( start_count < current_count) {
        spend_ticks =  (0xFFFFFFFF - current_count) + start_count;
    }
    else {
        spend_ticks = start_count - current_count;            
    }
	
	//spend_ticks = end_ticks - start_ticks;
	//spend_time = spend_ticks * portTICK_RATE_MS;
	spend_time = spend_ticks * 30 / loops;
	throughput = (msglen*8) / spend_time;
	printk("SW spend time each:%d, ticks:%d, throughput: %d Mbps\n", 
		spend_time, spend_ticks, throughput);


	return ret;
}


int throughput_test(u32 modetype, u32 cipher_type, u32 authtype, u32 loops)
{
//	u8 *pAsicDigest = &_AsicDigest[32];
	u32 digestlen;

	u32 auth_keylen, crypt_keylen, ivlen;
	u8 *auth_key, *crypt_key, *pIv;
	u32 msglen;
	u8 *message;
	int i;
	int ret = FAIL;

	u8 *pResult_asic;
//	u8 *pResult_sw;

	//int loop;
	//u32 start_ticks, end_ticks, spend_ticks, spend_time, throughput;
	//u32 this_rtlseed;

	//u32 a2eo, enl;

	

	dbg_printk("modetype=0x%x, cipher_type=0x%x, authtype=0x%x\n", 
		modetype, cipher_type, authtype);


	ivlen = 8;
	switch (cipher_type) {
		case _MD_NOCRYPTO:
			crypt_keylen = 0;
			break;
	    case 0x00:
			crypt_keylen = 8;
			break;
		case 0x02:
			crypt_keylen = 8;
			break;
		case 0x10:
			crypt_keylen = 24;
			break;
		case 0x12:
			crypt_keylen = 24;
			break;
		case 0x20:
		case 0x24:
		case 0x25:
			crypt_keylen = 32;
			ivlen = 16;
			break;
		case 0x22:
			crypt_keylen = 32;
			ivlen = 0;
			break;
		case 0x23:
			crypt_keylen = 32; 
			ivlen = 8;
			break;
		case 0x30: // chacha20
			crypt_keylen = 32; 
			ivlen = 8;
			break;
	    default:
	        err_printk("no this cipher type\n");
			return FAIL;
	}

	switch (authtype) {
		case _MD_NOAUTH:
			digestlen = 0;
			break;
	    case 2:
	    case 6:
	        digestlen = 16;
	        break;
	    case 0:
	    case 4:
	        digestlen = 20;
	        break;
		case 1:			
		case 5:
			digestlen = 32;
			break;
		case 7: // poly1305
			digestlen = 16;
			break;
			
	    default:
	        err_printk("authtype(%d) : no this type\n", authtype);
	        return FAIL;
	}


	crypt_key = &_asic_cryptkey[0];
	for (i=0; i<crypt_keylen; i++)
	      crypt_key[i] = rtl_random() & 0xFF;
	crypt_key[i] = '\0';



	// Suppose keylen = 0~ BLOCK(64)
	if ( (digestlen == 0) || (authtype < 3) ) { // no HMAC
		auth_keylen = 0;
		auth_key = NULL;
	} else {
	  auth_keylen = ( rtl_random() % 63 )+1;
	  auth_key = &_asic_authkey[0];
	  for (i=0; i<auth_keylen; i++)
	      auth_key[i] = rtl_random() & 0xFF;
	  auth_key[i] = '\0';
	}

	if ( cipher_type != _MD_NOCRYPTO ) {
		pIv = _asic_iv;
		for (i=0; i<ivlen; i++)
			pIv[i] = rtl_random() & 0xFF;
	} else {
		pIv = NULL;
		ivlen = 0;
	}
	

	// 32 byte * 50 = 1600
	msglen = 1600;

	message = &_asic_orig[32];

	for (i=0; i<msglen; i++)
	  message[i] = rtl_random() & 0xFF;
	message[i] = '\0';


	//a2eo = 0;
	//enl = msglen;

	pResult_asic = _asic_enc;		  

#if 0

	//
	// verify function first;
	// 

	scatter_th[0].len = msglen;
	scatter_th[0].ptr = message;

	pResult_sw	 = _sw_enc;


	ret = rtl_ipsecEngine(&g_rtl_IPSec_Engine, modetype, cipher_type|0x80, authtype, 
	   crypt_key, crypt_keylen, 
	   auth_key,auth_keylen,
	   pIv, ivlen, digestlen,
	   1, scatter_th, 
	   a2eo, enl, 
	   pResult_asic, sizeof(_asic_enc));

	rtl_assert( ret == SUCCESS );


	ret = rtl_ipsec_mixed_sim(modetype, cipher_type|0x80, authtype, 
		crypt_key, crypt_keylen, auth_key, auth_keylen, pIv, ivlen, 
		message, a2eo+enl, pResult_sw, a2eo, enl, digestlen);
	if (ret != SUCCESS)
	{
	    err_printk("ipsec_mixed_sim failed!\n");
	    goto ret_throughput_test;
	}

	if (memcmp(pResult_asic, pResult_sw, a2eo+enl) == 0)
	{
	    dbg_printk("IPSec ok\n");
	    ret = SUCCESS;
	}
	else
	{
	    ret = FAIL;
		goto ret_throughput_test;
	}
#endif

	if ( modetype == _MS_TYPE_AUTH ) {
		
			throughput_test_auth(authtype,
				auth_key, auth_keylen, digestlen, message, msglen, pResult_asic, sizeof(_asic_enc), loops);

			
	} 
	else {

			throughput_test_cipher(cipher_type,
				crypt_key, crypt_keylen, pIv, ivlen, digestlen, message, msglen, pResult_asic, sizeof(_asic_enc), loops);

	}

#if 0 
		//
		// 
		// 
		printk("Start looping to calculate HW throughput\n");
		start_ticks = xTaskGetTickCount();
		for (loop=0; loop<loops; loop++) {

			ret = rtl_ipsecEngine(&g_rtl_IPSec_Engine, modetype, cipher_type|0x80, authtype, 
			   crypt_key, crypt_keylen, 
			   auth_key,auth_keylen,
			   pIv, ivlen, digestlen,
			   1, scatter_th, 
			   a2eo, enl, 
			   pResult_asic, sizeof(_asic_enc));
			
			if (ret != SUCCESS)
			{
				err_printk("failed!\n");
				goto ret_throughput_test;
			}
		}
		end_ticks = xTaskGetTickCount();
		spend_ticks = end_ticks - start_ticks;
		spend_time = spend_ticks * portTICK_RATE_MS;
		throughput = (loops*(msglen*8)) / (spend_time*1000);
		printk("HW spend time:%d, ms:%d, throughput: %d Mbps, idea : %d MPS\n", 
			spend_ticks, spend_time, throughput, throughput * 40000000/SYSTEM_CLK);
	//
	//
	//
	printk("Start looping to calculate SW throughput\n");
	start_ticks = xTaskGetTickCount();
	for (loop=0; loop<loops; loop++) {

		ret = rtl_ipsec_mixed_sim(modetype, cipher_type|0x80, authtype, 
			crypt_key, crypt_keylen, auth_key, auth_keylen, pIv, ivlen, 
			message, a2eo+enl, pResult_sw, a2eo, enl, digestlen);
		
		if (ret != SUCCESS)
		{
			err_printk("failed!\n");
			goto ret_throughput_test;
		}
	}
	end_ticks = xTaskGetTickCount();
	spend_ticks = end_ticks - start_ticks;
	spend_time = spend_ticks * portTICK_RATE_MS;
	throughput = (loops*(msglen*8)) / (spend_time*1000);
	printk("SW spend time:%d, ms:%d, throughput: %d Mbps, idea : %d MPS\n", 
		spend_ticks, spend_time, throughput, throughput * 40000000/SYSTEM_CLK);

#endif
	
	ret = SUCCESS;
	

	return ret;


}


extern int rtl_crypto_test(IN u16 argc, IN u8 *argv[]);


int rtl_crypto_test(u16 argc, u8 *argv[])
{
	int ret;
	
	dbg_printk("argc=%d\n", argc);
	dbg_printk("argv[0]=%s\n", argv[0]);
    if (argc < 1)
    {
        rtl_crypto_usage(NULL);
        return _FALSE;
    }

    if (strcmp(argv[0], "init") == 0)
    {
		dbg_printk("doing init\n");
		ret = rtl_cryptoEngine_init();
		if(  ret == SUCCESS ) {
			printk("Crypto Engine Init Success!!!\n");
			return _TRUE;
		} else {
			printk("Crypto Engine Init FAILED, ret=%d\n", ret);
			return _FALSE;
		}
    }
    else if (strcmp(argv[0], "info") == 0)
    {
		dbg_printk("Hello info\n");
        rtl_cryptoEngine_info();
    }
    else if (strcmp(argv[0], "test") == 0)
    {
        int loop=1;
		int ret; 

		
		printk("Hello test \n");
		rtl_srandom(0xA0A0A0A0);
        for (;;) {
			if ( (loop % 100) == 0 )  
 			    printk("Round %d\n", loop);
			if ( (loop % 2) == 0 ) 
				ret = vector_test_cipher_rand();
			else 
				ret = vector_test_auth_rand();
            if ( ret != SUCCESS ) {
				printk("failed\n");
				printk("Round %d\n", loop);
				break;
            }
            loop++;
        }
        
    }
    else if (strcmp(argv[0], "debug") == 0)
    {
    	u8 mode_select;
		
        if (argc != 2)
        {
            rtl_crypto_usage("debug");
            return _FALSE;
        }

		mode_select = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 10);

		switch ( mode_select ) {
			case 0 : 
				DBG_INFO_MSG_OFF(_DBG_CRYPTO_);
				printk("Crypto Debug Info turn OFF\n");
				break;

			case 1 : 
				DBG_INFO_MSG_ON(_DBG_CRYPTO_);
				printk("Crypto Debug Info turn ON\n");
				break;

			default : 
				rtl_crypto_usage("debug");
				break;
		}
    	
    }
	
#if 1

//
//	Usage : 
//
//
//		throughput 0 0 -1 -> DES-CBC : 0x0
//		throughput 0 2 -1 -> DES-ECB : 0x2
//		throughput 0 16 -1 -> 3DES-CBC : 0x10
//		throughput 0 18 -1 -> 3DES-ECB : 0x12 
//		throughput 0 32 -1 -> AES-CBC :   0x20
//		throughput 0 34 -1 -> AES-ECB  :  0x22
//		throughput 0 35 -1 -> AES-CTR  : 0x23
//		throughput 0 36 -1 -> AES-CFB  : 0x24
//		throughput 0 37 -1 -> AES-OFB  : 0x25
//
//		throughput 1 -1 0 -> SHA1
//		throughput 1 -1 1 -> SHA2
//		throughput 1 -1 2 -> MD5
//		throughput 1 -1 4 -> HMAC-SHA1
//		throughput 1 -1 5 -> HMAC-SHA2
//		throughput 1 -1 6 -> HMAC-MD5
//
    
	else if (strcmp(argv[0], "throughput") == 0)
	{
		u32 modetype, crypttype, authtype, loops;


        modetype = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 10);
        crypttype = Strtoul((const u8*)(argv[2]), (u8 **)NULL, 10);
        authtype = Strtoul((const u8*)(argv[3]), (u8 **)NULL, 10);
        loops = Strtoul((const u8*)(argv[4]), (u8 **)NULL, 10);

		switch ( modetype ) {
			case _MS_CRYPTO : 
				authtype = _MD_NOAUTH ; 
				break;
			case _MS_AUTH : 
				crypttype = _MD_NOCRYPTO;
				break;
		}
				
		if ( throughput_test(modetype, crypttype, authtype, loops) != SUCCESS ) {
			printk("FAIL! \n");
		}
		
	}	
#endif // temp
//
//	Usage : 
//
//		vector 0 16 -1 -> 3DES-CBC : 0x10
//		vector 0 18 -1 -> 3DES-ECB : 0x12 
//		vector 0 32 -1 -> AES-CBC :   0x20
//		vector 0 34 -1 -> AES-ECB  :  0x22
//		vector 0 35 -1 -> AES-CTR  : 0x23
//          vector 0 36 -1 -> AES-CFB   : 0x24 
//          vector 0 37 -1 -> AES-OFB   : 0x25
//		vector 0 48 -1 -> Chacha20 : 0x30
//
//		vector 1 -1 0 -> SHA1
//		vector 1 -1 1 -> SHA2
//		vector 1 -1 2 -> MD5
//		vector 1 -1 4 -> HMAC-SHA1
//		vector 1 -1 5 -> HMAC-SHA2
//		vector 1 -1 6 -> HMAC-MD5
//	       vector 1 -1 7 -> Poly1305
//
//		vector 2 16 0 -> 3DES-CCB-SHA1 : MtE (MAC-then-Encrypt, for SSL)
//		MtE : HMAC(message) ->> Encrypt(message + HMAC )
//
    else if (strcmp(argv[0], "vector") == 0)
    {
        int ret; 
		u32 mode_select, cipher_type, auth_type;

        if (argc != 4)
        {
            rtl_crypto_usage("vector");
            return _FALSE;
        }

        mode_select = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 10);
        cipher_type = Strtoul((const u8*)(argv[2]), (u8 **)NULL, 10);
        auth_type = Strtoul((const u8*)(argv[3]), (u8 **)NULL, 10);

		switch (mode_select) {
			case 0 : // ESP algorithm
		        ret = vector_test_cipher(cipher_type);
				break;
			case 1 : // Auth algorithm
		        ret = vector_test_auth(auth_type);
				break;
			default:
				err_printk("No this mode_select type:%d\n", mode_select);
				ret = _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH;
				break;
		}

        printk("vector test done. ret=%d\n", ret);
        return _TRUE;
    }
    else
    {
        rtl_crypto_usage(NULL);
		return _FALSE;
    }

	return _TRUE;
}

