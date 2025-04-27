/*
 * Crypto engine test cases.
 * Port test cases according to cryto engine test cases posted on Mantis server.
 * We can look up test case description on Mantis by selecting bug filter to 
 * be RTL8652_IC_VERI project and CryptoEngine type.
 * Notice that standalone application should cooperate with U-Boot jump table functions.
 * Make sure that the application use the same jump table information with U-Boot.
 * We could refer to doc/README.standalone documentation file for more details.
 */

//#include <common.h>
//#include <exports.h>
#include "../../include/rtk_soc_common.h"
#include "crypto/crypto.h"
#include "crypto/destest.h"
#include "crypto/authTest.h"
#include "crypto/rtl865xc_authEngine.h"
#include "crypto/aesTest.h"
#include "crypto/util.h"
#include "crypto/cryptoCmd.h"
#include "crypto/hmactest.h"

int cryptoTest(int argc, char *argv[])
{
	int i;
	int32_t retval;
	char caseNum;

	app_startup(argv);

	rtl8651b_cryptoEngine_initdata();

	if( argc<=0 ){
		printf("Please input crypto test case number!\n");
		return 0;	
	}

#ifdef CONFIG_RTL8316S
	*((volatile unsigned int *)(0xB8003A00)) = (*((volatile unsigned int *)(0xB8003A00))|0x1000);
#endif

	/* test only 1, 2, 3, 6 cases for 8316s */
	for(i=0; i<argc ; i++){
		caseNum =  stringToInt(argv[i]);
		switch (caseNum){
			case 1:
			/* Basic test case 1 */
				printf("Basic test case 1: descriptor num = 3, burst size = 16 bytes\n");
				printf("Run des simulator and 8651b crypto engine test\n");
				printf("Round %u Function %u - %u Len %u - %u Offset %u - %u\n", 1, 0, 1, 8, 16376, 0, 0);
				if(rtl8651b_cryptoEngine_init( 3, 0) == FAILED) {
					printf("crypto engine init fail!!\n");
				}
				retval = runDes8651bGeneralApiTest( 1, 0, 1, 8, 16376, 0, 0);
				//retval = runDes8651bGeneralApiTest( 1, 0, 1, 16300, 16376, 0, 0);
	
				if ( retval == SUCCESS ) {
					printf("Basic test case 1 Pass!\n\n");
				} else {
					printf("Basic test case 1 Fail!\n\n");
				}
				break;
			case 2:
				/* Basic test case 2 */
				printf("Basic test case 2: descriptor num = 128, burst size = 32 bytes\n");
				printf("Run des simulator and 8651b crypto engine test\n");
				printf("Round %u Function %u - %u Len %u - %u Offset %u - %u\n", 1, 2, 3, 8, 16376, 0, 0);
				if(rtl8651b_cryptoEngine_init(128, 1) == FAILED) {
					printf("crypto engine init fail!!\n");
				}
				retval = runDes8651bGeneralApiTest(1, 2, 3, 8, 16376, 0, 0);
	
				if ( retval == SUCCESS ) {
					printf("Basic test case 2 Crypto runDes8651bGeneralApiTest Pass! {P902}\n\n");
				} else {
					printf("Basic test case 2 Crypto runDes8651bGeneralApiTest Fail! {F902}\n\n");
				}
				break;
			case 3:
				/* Basic test case 3 */
				printf("Basic test case 3: descriptor num = 8, burst size = 64 bytes\n");
				printf("Run des simulator and 8651b crypto engine test\n");
				printf("Round %u Function %u - %u Len %u - %u Offset %u - %u\n", 1, 0, 3, 8, 16376, 0, 7);
				if(rtl8651b_cryptoEngine_init(8, 2) == FAILED) {
					printf("crypto engine init fail!!\n");
				}
				retval = runDes8651bGeneralApiTest(1, 0, 3, 8, 16376, 0, 7);
	
				if ( retval == SUCCESS ) {
					printf("Basic test case 3 Pass!\n\n");
				} else {
					printf("Basic test case 3 Fail!\n\n");
				}
				break;

			case 4:
				/* for crypto burst size > 16 bytes bug with DDR SDRAM. */
				printf("Basic test case 4: descriptor num = 8, burst size = 128 bytes\n");
				printf("Run des simulator and 8651b crypto engine test\n");
				printf("Round %u Function %u - %u Len %u - %u Offset %u - %u\n", 1, 0, 3, 8, 16376, 0, 7);
				if(rtl8651b_cryptoEngine_init(8, 3) == FAILED) {
					printf("crypto engine init fail!!\n");
				}
				retval = runDes8651bGeneralApiTest(1, 0, 3, 8, 16376, 0, 7);
	
				if ( retval == SUCCESS ) {
					printf("Basic test case 4 Pass!\n\n");
				} else {
					printf("Basic test case 4 Fail!\n\n");
				}
				break;


				break;

#if 0
			case 4:
				/* Basic test case 4 */
				printf("Basic test case 4: descriptor num = 128, burst size = 32 bytes\n");
				printf("Run auth simulator and 8651b auth engine test\n");
				printf("Round %u Function %u - %u Len %u - %u keyLen %u - %u Offset %u - %u \n", 1, 0, 3, 8, 16240, 16, 20, 0, 7);
				if(rtl8651b_authEngine_init(8, 2) == FAILED) {//cr au init 8 64
					printf("crypto engine init fail!!\n");
				}
				retval = runAuth8651bGeneralApiTest(1, 0, 3, 8, 16240, 16, 20, 0, 7);//cr au as 1 0 3 8 16240 16 20 0 7
	
				if ( retval == SUCCESS ) {
					printf("Basic test case 4 Pass!\n\n");
				} else {
					printf("Basic test case 4 Fail!\n\n");
				}
				break;
#endif
			case 5:
				/* Basic test case 5 */
				printf("Basic test case 5: descriptor num = 8, burst size = 64 bytes\n");
				printf("Run AES simulator and 8651b crypto engine test\n");
				if(rtl8651b_cryptoEngine_init(8, 2) == FAILED) {//cr cr init 8 64
					printf("crypto engine init fail!!\n");
				}
				retval = runAes8651bGeneralApiTest(1, 0, 3, 16, 16368, 0, 7);//cr ae as 1 0 3 16 16368 0 7
	
				if ( retval == SUCCESS ) {
					printf("Basic test case 5 Pass!\n\n");
				} else {
					printf("Basic test  case 5 Fail!\n\n");
				}
				break;
			case 6:
				/* Basic test case 6 */
				printf("Basic test case 6: descriptor num = 8, burst size = 128 bytes\n");
				printf("Run DES random test seed %u and %u rounds\n", 0x65432, 10000);
				//if(rtl8651b_cryptoEngine_init(8, 2) == FAILED) {//cr cr init 8 64
				if(rtl8651b_cryptoEngine_init(8, 3) == FAILED) {//cr cr init 8 64
					printf("crypto engine init fail!!\n");
				}
				//retval = runDes8651bGeneralApiRandTest(0x65432, 1000000);//cr cr ra 0x65432 1000000
				retval = runDes8651bGeneralApiRandTest(0x65432, 10000);//cr cr ra 0x65432 1000000
	
				if ( retval == SUCCESS ) {
					printf("Basic test case 6 pass!\n\n");
				} else {
					printf("Basic test case 6 fail!\n\n");
				}
				break;
			case 7:
				/* Mix test case 1 */
				printf("Mix test case 1: descriptor num = 16, burst size = 64 bytes\n");
				if(rtl8651b_cryptoEngine_init(16, 2) == FAILED) {//cr cr init 16 64
					printf("crypto engine init fail!!\n");
				}
				retval = _crypto_mixCmdAsic( 1, 0, 0x23, 2, 3, 8, 8, 16, 20, 0, 0, 0x1); //cr mi as 1 0 0x23 2 3 8 8 16 20 0 0 0x1
	
				if ( retval == SUCCESS ) {
					printf("Mix test case 1 pass!\n\n");
				} else {
					printf("Mix test case 1 fail!\n\n");
				}
				break;
			case 8:
				/* Mix test case 2*/
				printf("Mix test case 2: descriptor num = 16, burst size = 64 bytes\n");
				if(rtl8651b_cryptoEngine_init(16, 2) == FAILED) {//cr cr init 16 64
					printf("crypto engine init fail!!\n");
				}
				retval = _crypto_mixCmdAsic( 1, 0, 0x23, 2, 3, 8, 8, 16, 20, 8, 8, 0x2); //cr mi as 1 0 0x23 2 3 8 8 16 20 8 8 0x2
	
				if ( retval == SUCCESS ) {
					printf("Mix test case 2 pass!\n\n");
				} else {
					printf("Mix test case 2 fail!\n\n");
				}
				break;
			case 9:
				/* Mix test case 3 */
				printf("Mix test case 3: descriptor num = 16, burst size = 64 bytes\n");
				if(rtl8651b_cryptoEngine_init(16, 2) == FAILED) {//cr cr init 16 64
					printf("crypto engine init fail!!\n");
				}
				retval = _crypto_mixCmdAsic( 1, 0, 0x23, 2, 3, 8, 8, 16, 20, 0, 0, 0x4); //cr mi as 1 0 0x23 2 3 8 8 16 20 0 0 0x4
	
				if ( retval == SUCCESS ) {
					printf("Mix test case 3 pass!\n\n");
				} else {
					printf("Mix test case 3 fail!\n\n");
				}
				break;
			case 10:
				/* Mix test case 4 */
				printf("Mix test case 4: descriptor num = 16, burst size = 64 bytes\n");
				if(rtl8651b_cryptoEngine_init(16, 2) == FAILED) {//cr cr init 16 64
					printf("crypto engine init fail!!\n");
				}
				//retval = _crypto_mixCmdAsic( 1, 0, 0x23, 0, 3, 8, 8, 16, 20, 0, 0, 0x8); //cr mi as 1 0 0x23 0 3 8 8 16 20 0 0 0x8
				retval = _crypto_mixCmdAsic( 1, 0, 0x23, 2, 3, 8, 8, 16, 20, 0, 0, 0x8); //cr mi as 1 0 0x23 2 3 8 8 16 20 0 0 0x8
	
				if ( retval == SUCCESS ) {
					printf("Mix test case 4 pass!\n\n");
				} else {
					printf("Mix test case 4 fail!\n\n");
				}
				break;
			case 11:
				/* Mix test case 5 */
				printf("Mix test case 5: descriptor num = 16, burst size = 64 bytes\n");
				if(rtl8651b_cryptoEngine_init(16, 2) == FAILED) {//cr cr init 16 64
					printf("crypto engine init fail!!\n");
				}
				retval = _crypto_mixCmdAsic( 1, 0, 0x23, 2, 3, 8, 8, 16, 20, 8, 8, 0x10); //cr mi as 1 0 0x23 2 3 8 8 16 20 8 8 0x10 
	
				if ( retval == SUCCESS ) {
					printf("Mix test case 5 pass!\n\n");
				} else {
					printf("Mix test case 5 fail!\n\n");
				}
				break;
			case 12:
				/* Mix test case 6 */
				printf("Mix test case 6: descriptor num = 16, burst size = 64 bytes\n");
				if(rtl8651b_cryptoEngine_init(16, 2) == FAILED) {//cr cr init 16 64
					printf("crypto engine init fail!!\n");
				}
				retval = _crypto_mixCmdAsic( 1, 0, 0x23, 0, 3, 8, 8, 16, 20, 0, 0, 0x20); //cr mi as 1 0 0x23 0 3 8 8 16 20 0 0 0x20 
	
				if ( retval == SUCCESS ) {
					printf("Mix test case 6 pass!\n\n");
				} else {
					printf("Mix test case 6 fail!\n\n");
				}
				break;
			case 13:
				/* Mix test case 7 */
				printf("Mix test case 7: descriptor num = 16, burst size = 64 bytes\n");
				if(rtl8651b_cryptoEngine_init(16, 2) == FAILED) {//cr cr init 16 64
					printf("crypto engine init fail!!\n");
				}
				retval = _crypto_mixCmdAsic( 1, 0, 0x23, 0, 3, 8, 8, 16, 20, 0, 0, 0x20); //cr mi as 1 0 0x23 0 3 8 8 16 20 0 0 0x20 
	
				if ( retval == SUCCESS ) {
					printf("Mix test case 7 pass!\n\n");
				} else {
					printf("Mix test case 7 fail!\n\n");
				}
				break;
			case 14:
				/* Mix test case 8 */
				printf("Mix test case 8: descriptor num = 16, burst size = 64 bytes\n");
				if(rtl8651b_cryptoEngine_init(16, 2) == FAILED) {//cr cr init 16 64
					printf("crypto engine init fail!!\n");
				}
				retval = _crypto_mixCmdAsic( 1, 0, 0x23, -1, -1, 16, 16, 0, 0, 0, 0, 0x10); //cr mi as 1 0 0x23 -1 -1 16 16 0 0 0 0 0x10
	
				if ( retval == SUCCESS ) {
					printf("Mix test case 8 pass!\n\n");
				} else {
					printf("Mix test case 8 fail!\n\n");
				}
				break;
			case 15:
				/* Mix test case 9 */
				printf("Mix test case 9: descriptor num = 16, burst size = 64 bytes\n");
				if(rtl8651b_cryptoEngine_init(16, 2) == FAILED) {//cr cr init 16 64
					printf("crypto engine init fail!!\n");
				}
				retval = _crypto_mixCmdAsic( 1, 0, 0x23, -1, -1, 16, 16, 0, 0, 0, 0, 0x2); //cr mi as 1 0 0x23 -1 -1 16 16 0 0 0 0 0x2
	
				if ( retval == SUCCESS ) {
					printf("Mix test case 9 pass!\n\n");
				} else {
					printf("Mix test case 9 fail!\n\n");
				}
				break;
			case 16:
				/* Mix test case 10 */
				printf("Mix test case 10: descriptor num = 16, burst size = 64 bytes\n");
				if(rtl8651b_cryptoEngine_init(16, 2) == FAILED) {//cr cr init 16 64
					printf("crypto engine init fail!!\n");
				}
				retval = _crypto_mixCmdAsic( 1, 0, 0x23, -1, -1, 16, 16, 0, 0, 0, 0, 0x4); //cr mi as 1 0 0x23 -1 -1 16 16 0 0 0 0 0x4
	
				if ( retval == SUCCESS ) {
					printf("Mix test case 10 pass!\n\n");
				} else {
					printf("Mix test case 10 fail!\n\n");
				}
				break;
			case 17:
				/* Mix test case 11 */
				printf("Mix test case 11: descriptor num = 16, burst size = 64 bytes\n");
				if(rtl8651b_cryptoEngine_init(16, 2) == FAILED) {//cr cr init 16 64
					printf("crypto engine init fail!!\n");
				}
				retval = _crypto_mixCmdAsic( 1, 0, 0x23, -1, -1, 16, 16, 0, 0, 0, 0, 0x8); //cr mi as 1 0 0x23 -1 -1 16 16 0 0 0 0 0x8
	
				if ( retval == SUCCESS ) {
					printf("Mix test case 11 pass!\n\n");
				} else {
					printf("Mix test case 11 fail!\n\n");
				}
				break;
			case 18:
				/* Mix test case 12 */
				printf("Mix test case 12: descriptor num = 16, burst size = 64 bytes\n");
				if(rtl8651b_cryptoEngine_init(16, 2) == FAILED) {//cr cr init 16 64
					printf("crypto engine init fail!!\n");
				}
				retval = _crypto_mixCmdAsic( 1, 0, 0x23, -1, -1, 16, 16, 0, 0, 0, 0, 0x10); //cr mi as 1 0 0x23 -1 -1 16 16 0 0 0 0 0x10
	
				if ( retval == SUCCESS ) {
					printf("Mix test case 12 pass!\n\n");
				} else {
					printf("Mix test case 12 fail!\n\n");
				}
				break;
			case 19:
				/* Mix test case 13 */
				printf("Mix test case 13: descriptor num = 16, burst size = 64 bytes\n");
				if(rtl8651b_cryptoEngine_init(16, 2) == FAILED) {//cr cr init 16 64
					printf("crypto engine init fail!!\n");
				}
				retval = _crypto_mixCmdAsic( 1, 0, 0x23, -1, -1, 16, 16, 0, 0, 0, 0, 0x20); //cr mi as 1 0 0x23 -1 -1 16 16 0 0 0 0 0x20
	
				if ( retval == SUCCESS ) {
					printf("Mix test case 13 pass!\n\n");
				} else {
					printf("Mix test case 13 fail!\n\n");
				}
				break;
			case 20:
				/* Mix test case 14 */
				printf("Mix test case 14: descriptor num = 16, burst size = 64 bytes\n");
				if(rtl8651b_cryptoEngine_init(16, 2) == FAILED) {//cr cr init 16 64
					printf("crypto engine init fail!!\n");
				}
				retval = _crypto_mixCmdAsic( 1, -1, -1, 0, 3, 0, 0, 16, 20, 0, 0, 0x1); //cr mi as 1 -1 -1 0 3 0 0 16 20 0 0 0x1
	
				if ( retval == SUCCESS ) {
					printf("Mix test case 14 pass!\n\n");
				} else {
					printf("Mix test case 14 fail!\n\n");
				}
				break;
			case 21:
				/* Mix test case 15 */
				printf("Mix test case 15: descriptor num = 16, burst size = 64 bytes\n");
				if(rtl8651b_cryptoEngine_init(16, 2) == FAILED) {//cr cr init 16 64
					printf("crypto engine init fail!!\n");
				}
				retval = _crypto_mixCmdAsic( 1, -1, -1, 0, 3, 0, 0, 16, 20, 0, 0, 0x2); //cr mi as 1 -1 -1 0 3 0 0 16 20 0 0 0x2
	
				if ( retval == SUCCESS ) {
					printf("Mix test case 15 pass!\n\n");
				} else {
					printf("Mix test case 15 fail!\n\n");
				}
				break;
			case 22:
				/* Mix test case 16 */
				printf("Mix test case 16: descriptor num = 16, burst size = 64 bytes\n");
				if(rtl8651b_cryptoEngine_init(16, 2) == FAILED) {//cr cr init 16 64
					printf("crypto engine init fail!!\n");
				}
				retval = _crypto_mixCmdAsic( 1, -1, -1, 0, 3, 0, 0, 16, 20, 0, 0, 0x4); //cr mi as 1 -1 -1 0 3 0 0 16 20 0 0 0x4
	
				if ( retval == SUCCESS ) {
					printf("Mix test case 16 pass!\n\n");
				} else {
					printf("Mix test case 16 fail!\n\n");
				}
				break;
			case 23:
				/* Mix test case 17 */
				printf("Mix test case 17: descriptor num = 16, burst size = 64 bytes\n");
				if(rtl8651b_cryptoEngine_init(16, 2) == FAILED) {//cr cr init 16 64
					printf("crypto engine init fail!!\n");
				}
				retval = _crypto_mixCmdAsic( 1, -1, -1, 0, 3, 0, 0, 16, 20, 0, 0, 0x8); //cr mi as 1 -1 -1 0 3 0 0 16 20 0 0 0x8 
	
				if ( retval == SUCCESS ) {
					printf("Mix test case 17 pass!\n\n");
				} else {
					printf("Mix test case 17 fail!\n\n");
				}
				break;
			case 24:
				/* Mix test case 18 */
				printf("Mix test case 18: descriptor num = 16, burst size = 64 bytes\n");
				if(rtl8651b_cryptoEngine_init(16, 2) == FAILED) {//cr cr init 16 64
					printf("crypto engine init fail!!\n");
				}
				retval = _crypto_mixCmdAsic( 1, -1, -1, 0, 3, 0, 0, 16, 20, 0, 0, 0x10); //cr mi as 1 -1 -1 0 3 0 0 16 20 0 0 0x10
	
				if ( retval == SUCCESS ) {
					printf("Mix test case 18 pass!\n\n");
				} else {
					printf("Mix test case 18 fail!\n\n");
				}
				break;
			case 25:
				/* Mix test case 19 */
				printf("Mix test case 19: descriptor num = 16, burst size = 64 bytes\n");
				if(rtl8651b_cryptoEngine_init(16, 2) == FAILED) {//cr cr init 16 64
					printf("crypto engine init fail!!\n");
				}
				retval = _crypto_mixCmdAsic( 1, -1, -1, 0, 3, 0, 0, 16, 20, 0, 0, 0x20); //cr mi as 1 -1 -1 0 3 0 0 16 20 0 0 0x20
	
				if ( retval == SUCCESS ) {
					printf("Mix test case 19 pass!\n\n");
				} else {
					printf("Mix test case 19 fail!\n\n");
				}
				break;
			case 26:
				/* Mix test case 20 */
				printf("Mix test case 20: descriptor num = 16, burst size = 64 bytes\n");
				if(rtl8651b_cryptoEngine_init(16, 2) == FAILED) {//cr cr init 16 64
					printf("crypto engine init fail!!\n");
				}
				retval = _crypto_mixCmdAsic( 1, 0, 3, 2, 3, 8, 24, 16, 20, 16, 16, 0x80); //cr mi as 1 0 3 2 3 8 24 16 20 16 16 0x80
	
				if ( retval == SUCCESS ) {
					printf("Mix test case 20-1 pass!\n\n");
				} else {
					printf("Mix test case 20-1 fail!\n\n");
				}
				retval = _crypto_mixCmdAsic( 1, 0x20, 0x23, 2, 3, 16, 32, 16, 20, 24, 24, 0x80); //cr mi as 1 0x20 0x23 2 3 16 32 16 20 24 24 0x80
	
				if ( retval == SUCCESS ) {
					printf("Mix test case 20-2 pass!\n\n");
				} else {
					printf("Mix test case 20-2 fail!\n\n");
				}
				break;
			case 27:
				/* Individual test case 2: Test authentication*/
				printf("Individual test case 2: descriptor num = 16, burst size = 64 bytes\n");
				if(rtl8651b_cryptoEngine_init(16, 2) == FAILED) {//cr cr init 16 64
					printf("crypto engine init fail!!\n");
				}
				retval = _crypto_mixCmdAsic( 1, 0, 0, -1, -1, 8, 8, 16, 16, 0, 0, 0x4); //cr mi as 1 0 0 -1 -1 8 8 16 16 0 0 0x4
	
				if ( retval == SUCCESS ) {
					printf("Individual test case 2 pass!\n\n");
				} else {
					printf("Individual test case 2 fail!\n\n");
				}
				break;
			case 28:
				/* Individual test case 3 */
				printf("Individual test case 3: descriptor num = 128, burst size = 32 bytes\n");
				printf("Run auth simulator and 8651b auth engine test\n");
				printf("Round %u Function %u - %u Len %u - %u keyLen %u - %u Offset %u - %u \n", 1, 0, 3, 8, 16240, 48, 144, 0, 7);
                                if(rtl8651b_cryptoEngine_init(16, 2) == FAILED) {//cr cr init 16 64
                                        printf("crypto engine init fail!!\n");
                                }
				/*Why not use rtl8651b_authEngine_init ?*/
				//if(rtl8651b_authEngine_init(16, 2) == FAILED) {//cr au init 16 64
				//	printf("crypto engine init fail!!\n");
				//}
				retval = runAuth8651bGeneralApiTest(1, 0, 3, 8, 16240, 48, 144, 0, 7);//cr au as 1 0 3 8 16240 48 144 0 7
	
				if ( retval == SUCCESS ) {
					printf("Individual test case 3 Pass!\n\n");
				} else {
					printf("Individual test case 3 Fail!\n\n");
				}
				break;
			case 29:
				/* Individual test case 5 */
				printf("Individual test case 5: descriptor num = 128, burst size = 32 bytes\n");
                                if(rtl8651b_cryptoEngine_init(16, 2) == FAILED) {//cr cr init 16 64
                                        printf("crypto engine init fail!!\n");
                                }
				
				retval = hmacSimRandTest(10000);//cr au ra 0x1245 
                                if ( retval == SUCCESS ) {
                                        printf("Individual test case 5-1 pass!\n\n");
                                } else {
                                        printf("Individual test case 5-1 fail!\n\n");
                                }

				rtlglue_printf("Run AES random test seed %u and %u rounds\n", 0xdead, 1000);
				retval = runAes8651bGeneralApiRandTest(0xdead, 1000); // cr ae ra 0xdead 1000
                                if ( retval == SUCCESS ) {
                                        printf("Individual test case 5-2 pass!\n\n");
                                } else {
                                        printf("Individual test case 5-2 fail!\n\n");
                                }
				retval = runAuth8651bGeneralApiRandTest( 0x5566, 1000000);//cr au ra 0x5566 1000000 asic
				if ( retval == SUCCESS ) {
                                        printf("Individual test case 5-3 pass!\n\n");
                                } else {
                                        printf("Individual test case 5-3 fail!\n\n");
                                }

				retval = runDes8651bGeneralApiRandTest(0x5678, 1000000);//cr cr ra 0x5678 1000000
                                if ( retval == SUCCESS ) {
                                        printf("Individual test case 5-4 pass!\n\n");
                                } else {
                                        printf("Individual test case 5-4 fail!\n\n");
                                }
                                break;
			default:
				break;
			}
		}
	printf ("Hit any key to exit ... ");
	while (!tstc())
		;
	/* consume input */
	(void) getc();

	printf ("\n\n");
	return (0);
}
