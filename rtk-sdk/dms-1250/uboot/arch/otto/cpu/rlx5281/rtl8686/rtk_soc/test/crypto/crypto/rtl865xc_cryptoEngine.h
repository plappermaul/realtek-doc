/*
* --------------------------------------------------------------------
* Copyright c                  Realtek Semiconductor Corporation, 2003  
* All rights reserved.
* 
* Program : 8651B crypto engine driver header
* Abstract : 
* $Id: rtl865xc_cryptoEngine.h,v 1.2 2006/01/19 05:19:24 yjlou Exp $
* $Log: rtl865xc_cryptoEngine.h,v $
* Revision 1.2  2006/01/19 05:19:24  yjlou
* *: move rtl865xc_crypto_source_t and rtl865xc_crypto_dest_t to crypto.h
* *: little fix for IPSEC
* *: modify definition of KSEG0_ADDRESS and KSEG1_ADDRESS
*
* Revision 1.1  2005/11/01 03:22:38  yjlou
* *** empty log message ***
*
* Revision 1.6  2005/01/26 08:21:10  cfliu
* +:Commit Generic DMA engine code
*
* Revision 1.5  2004/12/27 01:43:31  yjlou
* +: support RTL8651_CRYPTO_NON_BLOCKING
*
* Revision 1.4  2004/07/13 07:08:10  danwu
* + document of API
*
* Revision 1.3  2004/06/23 15:36:12  yjlou
* *: removed duplicated code, and implemented original functions with the generic function:
*    rtl8651b_authEngine_md5(), rtl8651b_authEngine_sha1(),
*    rtl8651b_authEngine_hmacMd5(), rtl8651b_authEngine_hmacSha1(),
*    rtl8651b_cryptoEngine_ecb_encrypt(), rtl8651b_cryptoEngine_3des_ecb_encrypt(),
*    rtl8651b_cryptoEngine_cbc_encrypt(), rtl8651b_cryptoEngine_3des_cbc_encrypt()
* -: remove rtl8651b_cryptoEngine_cbc_encryptEmbIV() and rtl8651b_cryptoEngine_ede_cbc_encryptEmbIV()
*
* Revision 1.2  2004/06/23 10:15:45  yjlou
* *: convert DOS format to UNIX format
*
* Revision 1.1  2004/06/23 09:18:57  yjlou
* +: support 865xB CRYPTO Engine
*   +: CONFIG_RTL865XB_EXP_CRYPTOENGINE
*   +: basic encry/decry functions (DES/3DES/SHA1/MAC)
*   +: old-fashion API (should be removed in next version)
*   +: batch functions (should be removed in next version)
*
* Revision 1.5  2003/09/30 12:24:28  jzchen
* Add interrupt counter get api
*
* Revision 1.4  2003/09/23 02:23:46  jzchen
* Add batch API
*
* Revision 1.3  2003/09/09 11:36:49  jzchen
* Add generic crypto function, which use mode to avoid 6 combination
*
* Revision 1.2  2003/09/08 04:40:39  jzchen
* Add initial function definition
*
* Revision 1.1  2003/08/28 02:52:07  jzchen
* Initial 8651b crypto engine driver code
*
*
* --------------------------------------------------------------------
*/

#ifndef RTL8651B_CRYPTOENGINE_H
#define RTL8651B_CRYPTOENGINE_H

//#include <rtl_types.h>
#include "crypto.h"

/*
@func void	| rtl8651b_cryptoEngine_des	| envoke crypto engine
@parm uint32 	| mode		| Crypto mode
@parm int8 * 	| data	| Pointer to input data
@parm uint32 	| dataLen		| Length of input data
@parm int8 *		| key	| Pointer to key
@parm int8 *		| iv	| Pointer to iv
@rdesc None
@rvalue SUCCESS		| 	Encryption/decryption done as requested
@rvalue FAILED	| 	Invalid parameter
@comm User does not need to reverse the sequence of keys when applying for 3DES.
That is, when data is encrypted with keys A/B/C, decrypt it with A/B/C, instead of C/B/A.
 */
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


//data, key and iv does not have 4-byte alignment limitatiuon
int32 rtl8651b_cryptoEngine_des(uint32 mode, int8 *data, uint32 len, int8 *key, int8 *iv );

#define rtl8651b_cryptoEngine_ecb_encrypt(input,len,key,encrypt) rtl8651b_cryptoEngine_des(encrypt?ENCRYPT_ECB_DES:DECRYPT_ECB_DES,input,len,key,NULL)
#define rtl8651b_cryptoEngine_3des_ecb_encrypt(input,len,key,encrypt) rtl8651b_cryptoEngine_des(encrypt?ENCRYPT_ECB_3DES:DECRYPT_ECB_3DES,input,len,key,NULL)
#define rtl8651b_cryptoEngine_cbc_encrypt(input,len,key,iv,encrypt) rtl8651b_cryptoEngine_des(encrypt?ENCRYPT_CBC_DES:DECRYPT_CBC_DES,input,len,key,iv)
#define rtl8651b_cryptoEngine_3des_cbc_encrypt(input,len,key,iv,encrypt) rtl8651b_cryptoEngine_des(encrypt?ENCRYPT_CBC_3DES:DECRYPT_CBC_3DES,input,len,key,iv)


#endif

