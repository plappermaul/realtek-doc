/*
 * Crypto engine test cases.
 * Port test cases according to cryto engine test cases posted on Mantis server.
 * We can look up test case description on Mantis by selecting bug filter to 
 * be RTL8652_IC_VERI project and CryptoEngine type.
 * Notice that standalone application should cooperate with U-Boot jump table functions.
 * Make sure that the application use the same jump table information with U-Boot.
 * We could refer to doc/README.standalone documentation file for more details.
 */

#include <common.h>
#include <exports.h>
#include "gdma.h"
#include "gdma_utils.h"
#include "gdma_test.h"
#include "rtl_types.h"
#include "virtualMac.h"

//#define RUN_PATTERN_MATCH
#define RUN_MEMORY
//#define RUN_CHECKSUM
//#define RUN_SEQUENTIAL_TCAM
#define RUN_WIRELESS_MIC
//#define RUN_BASE64
//#define RUN_QUOTED_PRINTABLE
#define RUN_BYTE_SWAP
//#define RUN_ENUMBAL

int32 runPatternMatch(int32 caseNum);
int32 runMemory(int32 caseNum);
int32 runChecksum(int32 caseNum);
int32 runSequentialTCAM(int32 caseNum);
int32 runWirelessMIC(int32 caseNum);
int32 runBase64(int32 caseNum);
int32 runQuotedPrintable(int32 caseNum);
int32 runByteSwap(int32 caseNum);
int32 runEnumBAL(int32 caseNum);
extern uint32 internal_gdmacnr;
int32 internal_gdmacnr_value[4] = { 
					0x00000000, /* BURST SIZE = 4 words*/
					0x00000040, /* BURST SIZE = 8 words */
					0x00000080, /* BURST SIZE = 16 words */
					0x000000C0  /* BURST SIZE = 32 words */
				  };

#define PROCESS_FAIL 	while(1)
int gdma_test (int flag, int argc, char *argv[])
{
	int32 caseNum;
	int32 retval;
	int32 i;

	//model_setTestTargetMask(IC_TYPE_MODEL|IC_TYPE_REAL);
	for(caseNum=2; caseNum<16; caseNum++){

		//internal_gdmacnr = internal_gdmacnr_value[caseNum%(sizeof(internal_gdmacnr_value)/sizeof(uint32))];
		internal_gdmacnr = internal_gdmacnr_value[3];
		printf("Burst Size = %d\n", 4 << (caseNum%(sizeof(internal_gdmacnr_value)/sizeof(uint32))));

#ifdef RUN_PATTERN_MATCH
                _cache_flush();
		retval = runPatternMatch(caseNum);
		if(SUCCESS == retval)
			printf("PatternMatch case %d Success\n", caseNum);
		else{
			if(FAILED == retval){
				printf("PatternMatch case %d Fail\n", caseNum);
				PROCESS_FAIL;
			}
		}
#endif

#ifdef RUN_MEMORY
                _cache_flush();
		retval = runMemory(caseNum);
		if(SUCCESS == retval)
			printf("Memory case %d Success\n", caseNum);
		else{
			if(FAILED == retval){
				printf("Memory case %d Fail\n", caseNum);
				PROCESS_FAIL;
			}
		}
#endif


#ifdef RUN_CHECKSUM
                _cache_flush();
		retval = runChecksum(caseNum);
		if(SUCCESS == retval)
			printf("Checksum case %d Success\n", caseNum);
		else{
			if(FAILED == retval){
				printf("Checksum case %d Fail\n", caseNum);
				PROCESS_FAIL;
				return -1;
			}
		}
#endif

#ifdef RUN_SEQUENTIAL_TCAM
                _cache_flush();
		retval = runSequentialTCAM(caseNum);
		if(SUCCESS == retval)
			printf("SequentialTCAM case %d Success\n", caseNum);
		else{
			if(FAILED == retval){
				printf("SequentialTCAM case %d Fail\n", caseNum);
				PROCESS_FAIL;
			}
		}
#endif

#ifdef RUN_WIRELESS_MIC
                _cache_flush();
		printf("WirelessMIC case %d :\n", caseNum);
		retval = runWirelessMIC(caseNum);
		if(SUCCESS == retval)
			printf("WirelessMIC case %d Success\n", caseNum);
		else{
			if(FAILED == retval){
				printf("WirelessMIC case %d Fail\n", caseNum);
				PROCESS_FAIL;
			}
		}
#endif

#ifdef RUN_BASE64
                _cache_flush();
		retval = runBase64(caseNum);
		if(SUCCESS == retval)
			printf("Base64 case %d Success\n", caseNum);
		else{
			if(FAILED == retval){
				printf("Base64 case %d Fail\n", caseNum);
				PROCESS_FAIL;
			}
		}
#endif


#ifdef RUN_QUOTED_PRINTABLE
                _cache_flush();
		retval = runQuotedPrintable(caseNum);
		if(SUCCESS == retval)
			printf("QuotedPrintable case %d Success\n", caseNum);
		else{
			if(FAILED == retval){
				printf("QuotedPrintable case %d Fail\n", caseNum);
				PROCESS_FAIL;
			}
		}
#endif


#ifdef RUN_BYTE_SWAP
                _cache_flush();
		retval = runByteSwap(caseNum);
		if(SUCCESS == retval)
			printf("ByteSwap case %d Success\n", caseNum);
		else{
			if(FAILED == retval){
				printf("ByteSwap case %d Fail\n", caseNum);
				PROCESS_FAIL;
			}
		}
#endif


#ifdef RUN_ENUMBAL
                _cache_flush();
		retval = runEnumBAL(caseNum);
		if(SUCCESS == retval)
			printf("EnumBAL case %d Success\n", caseNum);
		else{
			if(FAILED == retval){
				printf("EnumBAL case %d Fail\n", caseNum);
				PROCESS_FAIL;
			}
		}
#endif
	}

	printf ("\n\n");
	return (0);
}

int32 runPatternMatch(int32 caseNum)
{
	if(caseNum > 15)
		return NOSUCHCASE;
	switch (caseNum){
		case 1:
			return testGdmaPatternMatch1(caseNum);
		case 2:
			return testGdmaPatternMatch2(caseNum);
		case 3:
			return testGdmaPatternMatch3(caseNum);
		case 4:
			return testGdmaPatternMatch4(caseNum);
		case 5:
			return testGdmaPatternMatch5(caseNum);
		case 6:
			return testGdmaPatternMatch6(caseNum);
		case 7:
			return NOSUCHCASE;
			//return testGdmaPatternMatch7(caseNum);
		case 8:
			return NOSUCHCASE;
			//return testGdmaPatternMatch8(caseNum);
		case 9:
			return NOSUCHCASE;
			//return testGdmaPatternMatch9(caseNum);
		case 10:
			return testGdmaPatternMatch10(caseNum);
		case 11:
			return testGdmaPatternMatch11(caseNum);
		case 12:
			return testGdmaPatternMatch12(caseNum);
		case 13:
			return testGdmaPatternMatch13(caseNum);
		case 14:
			return testGdmaPatternMatch14(caseNum);
		case 15:
			return testGdmaPatternMatch15(caseNum);
		default:
			return NOSUCHCASE;
	}
	return FAILED;
}

int32 runMemory(int32 caseNum)
{
	if(caseNum > 15)
		return NOSUCHCASE;
	switch (caseNum){
		case 1:
			return NOSUCHCASE;
			return testGdmaMemory1(caseNum);
		case 2:
			return testGdmaMemory2(caseNum);
		case 3:
			return testGdmaMemory3(caseNum);
		case 4:
			return testGdmaMemory4(caseNum);
		case 5:
			return testGdmaMemory5(caseNum);
		case 6:
			return NOSUCHCASE;
			return testGdmaMemory6(caseNum);
		case 7:
			return NOSUCHCASE;
			return testGdmaMemory7(caseNum);
		case 8:
			return testGdmaMemory8(caseNum);
		case 9:
			return NOSUCHCASE;
			return testGdmaMemory9(caseNum);
		case 10:
			return testGdmaMemory10(caseNum);
		case 11:
			return testGdmaMemory11(caseNum);
		case 12:
			return NOSUCHCASE;
			return testGdmaMemory12(caseNum);
		case 13:
			return testGdmaMemory13(caseNum);
		case 14:
			return testGdmaMemory14(caseNum);
		case 15:
			return NOSUCHCASE;
			return testGdmaMemory15(caseNum);
		default:
			return NOSUCHCASE;	
	}
	return FAILED;
}

int32 runChecksum(int32 caseNum)
{
	if(caseNum > 2)
		return NOSUCHCASE;
	switch (caseNum){
		case 1:
			return testGdmaChecksum1(caseNum);
		case 2:
			return testGdmaChecksum2(caseNum);
		default:
			return NOSUCHCASE;
	}
	return FAILED;
}

int32 runSequentialTCAM(int32 caseNum)
{
	if(caseNum > 2)
		return NOSUCHCASE;

	switch (caseNum){
		case 1:
			return testGdmaSequentialTCAM(caseNum);
		case 2:
			return testGdmaSequentialTCAM2(caseNum);
		default:
			return NOSUCHCASE;
	}
	return FAILED;
}

int32 runWirelessMIC(int32 caseNum)
{
	if(caseNum > 3)
		return NOSUCHCASE;
	switch (caseNum){
		case 1:
			return testGdmaWirelessMIC(caseNum);
		case 2:
			return testGdmaWirelessMIC2(caseNum);
		case 3:
			return testGdmaWirelessMIC3(caseNum);
		default:
			return NOSUCHCASE;
	}
	return FAILED;
}

int32 runBase64(int32 caseNum)
{
	if(caseNum > 7)
		return NOSUCHCASE;
	switch (caseNum){
		case 1:
			return testGdmaBase64Encode1(caseNum);
		case 2:
			return testGdmaBase64Encode2(caseNum);
		case 3:
			return testGdmaBase64Encode3(caseNum);
		case 4:
			return testGdmaBase64Decode1(caseNum);
		case 5:
			return testGdmaBase64Decode2(caseNum);
		case 6:
			return NOSUCHCASE;
			//return testGdmaBase64EncDec1(caseNum);
		case 7:
			return testGdmaBase64EncDec2(caseNum);
		defaut:
			return NOSUCHCASE;
	}
	return FAILED;
}

int32 runQuotedPrintable(int32 caseNum)
{
	if(caseNum > 4)
		return NOSUCHCASE;
	switch (caseNum){
		case 1:
			return testGdmaQuotedPrintableEncode1(caseNum);
		case 2:
			return testGdmaQuotedPrintableDecode1(caseNum);
		case 3:
			return testGdmaQuotedPrintableDecode2(caseNum);
		case 4:
			return testGdmaQuotedPrintableEncDec2(caseNum);
		default:
			return NOSUCHCASE;
	}
	return FAILED;
}

int32 runByteSwap(int32 caseNum)
{
	if(caseNum > 4)
		return NOSUCHCASE;
	switch (caseNum){
		case 1:
			return testGdmaByteSwap1(caseNum);
		case 2:
			return testGdmaByteSwap2(caseNum);
		case 3:
			return testGdmaByteSwap3(caseNum);
		case 4:
			return testGdmaByteSwap4(caseNum);
		default:
			return NOSUCHCASE;
	}
	return FAILED;
}

int32 runEnumBAL(int32 caseNum)
{
	if(caseNum > 1)
		return NOSUCHCASE;
	switch (caseNum){
		case 1: 
			return testGdmaEnumBAL(caseNum);
		default:
			return NOSUCHCASE;
	}
	return FAILED;
}
