/* Copyright c                  Realtek Semiconductor Corporation, 2006
* All rights reserved.
* 
* Program : Test Code Header File For IC-FT2 Testing
* Abstract : 
* Author : Michael Huang (michaelhuang@realtek.com.tw)               
*/

#ifndef _FT2_TEST_CODE_
#define _FT2_TEST_CODE_

#include "rtl_types.h"
int32 ft2_testCPU_MIPS(uint32 caseNo);
int32 ft2_testCPU_Arithmetic(uint32 caseNo);
int32 ft2_testImem(uint32 caseNo);
int32 ft2_testDmem(uint32 caseNo);
int32 ft2_testSram_BIST_Status(uint32 caseNo);
int32 ft2_testSram_BIST_RLMRAM(uint32 caseNo);
int32 ft2_testSram_BIST_MIBRAM(uint32 caseNo);
int32 ft2_testSram_BIST_AMI(uint32 caseNo);
int32 ft2_testSram_BIST_HTRAM(uint32 caseNo);
int32 ft2_testSram_BIST_L2RAM(uint32 caseNo);
int32 ft2_testSram_BIST_L4RAM(uint32 caseNo);
int32 ft2_testSram_BIST_ACLRAM126(uint32 caseNo);
int32 ft2_testSram_BIST_ACLRAM125(uint32 caseNo);
int32 ft2_testSram_BIST_NICTXRAM(uint32 caseNo);
int32 ft2_testSram_BIST_NICRXRAM(uint32 caseNo);
int32 ft2_testSram_BIST_V4KRAM(uint32 caseNo);
int32 ft2_testSram_BIST_OQRAM(uint32 caseNo);
int32 ft2_testSram_BIST_IQRAM(uint32 caseNo);
int32 ft2_testSram_BIST_HSARAM125(uint32 caseNo);
int32 ft2_testSram_BIST_HSARAM121(uint32 caseNo);
int32 ft2_testSram_BIST_PBRAM(uint32 caseNo);
int32 ft2_testPCI_S0(uint32 caseNo);
int32 ft2_testUSB_P0_Detect(uint32 caseNo);
int32 ft2_testUSB_P0_Loopback(uint32 caseNo);
int32 ft2_testUSB_P1_Detect(uint32 caseNo);
int32 ft2_testUSB_P1_Loopback(uint32 caseNo);
int32 ft2_testUSB_PowerOn(uint32 caseNo);
int32 ft2_testUSB_PowerOff(uint32 caseNo);
int32 ft2_testUSB_IncreaseVoltage(uint32 caseNo);
int32 ft2_testUSB_DecreaseVoltage(uint32 caseNo);


#endif	/* _FT2_TEST_CODE_ */
