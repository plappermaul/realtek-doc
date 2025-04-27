/*
* --------------------------------------------------------------------
* Copyright c                  Realtek Semiconductor Corporation, 2003  
* All rights reserved.
* 
* Program : 8651B authentication engine driver API definition
* Abstract : 
* $Id: rtl865xc_authEngine.h,v 1.2 2006/01/23 06:32:11 yjlou Exp $
* $Log: rtl865xc_authEngine.h,v $
* Revision 1.2  2006/01/23 06:32:11  yjlou
* *: modified for FPGA (HMAC MD5/SHA1 verified)
*
* Revision 1.1  2005/11/01 03:22:38  yjlou
* *** empty log message ***
*
* Revision 1.6  2004/12/27 01:44:45  yjlou
* +: support RTL8651_AUTH_NON_BLOCKING
* *: use RTL8651_AUTH_IOPAD_READY, instead of IOPAD_READY.
*
* Revision 1.5  2004/12/22 11:38:08  yjlou
* +: support IOPAD_READY to pre-compute ipad and opad.
*
* Revision 1.4  2004/07/13 07:07:53  danwu
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
* Revision 1.4  2003/09/30 12:24:44  jzchen
* Add interrupt counter get api and remove data dump api
*
* Revision 1.3  2003/09/29 09:03:19  jzchen
* Add authentication batch command
*
* Revision 1.2  2003/09/23 02:23:57  jzchen
* Add generic API
*
* Revision 1.1  2003/09/09 07:12:20  jzchen
* Add first version of authentication engine driver
*
*
* --------------------------------------------------------------------
*/

#ifndef RTL8651B_AUTHENGINE_H
#define RTL8651B_AUTHENGINE_H

#include "rtl_types.h"
#include "crypto.h"

#define RTL8651B_MD5_DIGEST_LENGTH 		16
#define RTL8651B_SHA1_DIGEST_LENGTH		20

int32 rtl8651b_authEngine_init(uint32 descNum, int8 mode32Bytes);
int32 rtl8651b_authEngine_exit(void);
int32 rtl8651b_authEngine_alloc(uint32 descNum);
int32 rtl8651b_authEngine_free(void);

/*
@func void	| rtl8651b_authEngine	| envoke authentication engine
@parm uint32 	| mode		| Authentication mode
@parm uint8 * 	| data	| Pointer to input data
@parm uint32 	| dataLen		| Length of input data
@parm uint8 *		| key	| Pointer to key
@parm uint32		| keyLen	| Length of key
@parm uint8 *		| digest	| Pointer to output digest
@rdesc None
@rvalue SUCCESS		| 	Authentication done as requested
@rvalue FAILED	| 	Invalid parameter
@comm This function will pad the data if necessary. User should input length of data without padding.
However, user must make sure that there is enough room for padding at the end of data.
 */
//Bit 0: 0:MD5 1:SHA1
//Bit 1: 0:Hash 1:HMAC
#define HASH_MD5		0x00
#define HASH_SHA1		0x01
#define HMAC_MD5		0x02
#define HMAC_SHA1		0x03
#define RTL8651_AUTH_NON_BLOCKING	0x08
#define RTL8651_AUTH_IOPAD_READY	0x00000100
int32 rtl8651b_authEngine(uint32 mode, uint8 * data, uint32 dataLen, uint8 * key, uint32 keyLen, uint8 * digest);

#define rtl8651b_authEngine_md5(data,dataLen,digest) rtl8651b_authEngine(HASH_MD5,data,dataLen,NULL,0,digest)
#define rtl8651b_authEngine_sha1(data,dataLen,digest) rtl8651b_authEngine(HASH_SHA1,data,dataLen,NULL,0,digest)
#define rtl8651b_authEngine_hmacMd5(data,dataLen,key,keyLen,digest) rtl8651b_authEngine(HMAC_MD5,data,dataLen,key,keyLen,digest)
#define rtl8651b_authEngine_hmacSha1(data,dataLen,key,keyLen,digest) rtl8651b_authEngine(HMAC_SHA1,data,dataLen,key,keyLen,digest)

#define RTL8651B_MAX_MD_CBLOCK 		64

//Management information counter
void rtl8651b_authEngineGetIntCounter(uint32 * doneCounter, uint32 * allDoneCounter);

#endif

