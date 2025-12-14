/*
* --------------------------------------------------------------------
* Copyright c                  Realtek Semiconductor Corporation, 2003  
* All rights reserved.
* 
* Program : Simulate DES behavior and providing final DES driver interface
* Abstract : 
* $Id: aesSim.c,v 1.2 2006-07-13 16:03:06 chenyl Exp $
*/

#include "crypto_types.h"
#include "aesSim.h"
#include "aes.h"

int32 aesSim_aes(uint32 mode, int8 *input, int8 *output, uint32 len, uint32 lenKey, int8 *key, int8 *iv )
{
	AES_KEY aesKey;
	int done;
	uint8 _iv[16]; /* Since soft_AES_cbc_encrypt() will modify iv, we need to allocate one for it. */
	uint8 counter[AES_BLOCK_SIZE];
	uint8 ecount_buf[AES_BLOCK_SIZE];
	uint32 num;

#if 0
	printf("===== %s: enter =====\n");
	printf("mode=%x\n", mode);
	memDump(input, len, "input");
	memDump(key, lenKey, "key");
	memDump(iv, sizeof(_iv), "iv");
#endif
	switch ( mode )
	{
		case SWDECRYPT_CBC_AES:
			memcpy( _iv, iv, sizeof(_iv) );
			AES_set_decrypt_key( (const uint8 *) key, lenKey*8, &aesKey );
			//memDump(&aesKey, lenKey*8, "aeskey");
			soft_AES_cbc_encrypt( (const uint8 *) input, (uint8 *) output, len, &aesKey, _iv, FALSE );
			//memDump(output, len, "output");
			break;
			
		case SWDECRYPT_ECB_AES:
			AES_set_decrypt_key( (const uint8 *) key, lenKey*8, &aesKey );
			for( done = 0; done < len; done += 16 )
				AES_ecb_encrypt( (const uint8 *) &input[done], (uint8 *) &output[done], &aesKey, AES_DECRYPT );
			break;
			
		case SWDECRYPT_CTR_AES:
			AES_set_encrypt_key( (const uint8 *) key, lenKey*8, &aesKey );
			memcpy( counter, iv, sizeof(counter) );
			memset( ecount_buf, 0, sizeof(ecount_buf) );
			num = 0;
			AES_ctr128_encrypt( (const uint8 *) input, (uint8 *) output, len, &aesKey, counter, ecount_buf, &num );
			break;
			
		case SWENCRYPT_CBC_AES:
			memcpy( _iv, iv, sizeof(_iv) );
			AES_set_encrypt_key( (const uint8 *) key, lenKey*8, &aesKey );
			soft_AES_cbc_encrypt( (const uint8 *) input, (uint8 *) output, len, &aesKey, _iv, TRUE );
			break;
			
		case SWENCRYPT_ECB_AES:
			AES_set_encrypt_key( (const uint8 *) key, lenKey*8, &aesKey );
			for( done = 0; done < len; done += 16 )
				AES_ecb_encrypt( (const uint8 *) &input[done], (uint8 *) &output[done], &aesKey, AES_ENCRYPT );
			break;
			
		case SWENCRYPT_CTR_AES:
			AES_set_encrypt_key( (const uint8 *) key, lenKey*8, &aesKey );
			memcpy( counter, iv, sizeof(counter) );
			memset( ecount_buf, 0, sizeof(ecount_buf) );
			num = 0;
			AES_ctr128_encrypt( (const uint8 *) input, (uint8 *) output, len, &aesKey, counter, ecount_buf, &num );
			break;
	}
	return SUCCESS;
}

