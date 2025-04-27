#include "../../include/rtk_soc_common.h"
#include "./idmem_test.h"

int _idmemTest(int caseNo)
{
	int32_t retval;
	
	retval = 0;

	switch (caseNo){
		case 1:
		/* IMEM0~L2MEM, size 4bytes ~ sizeof(MEM), normal pattern */
			if(iDMemTestCase1() == FAILED) {
				printf("iDMemTestCase1 fail!!\n");
				retval = -1;	
			}
			else{
				printf("iDMemTestCase1 Passed!!\n");
			}
			break;
		case 2:
		/* IMEM0~L2MEM, size 4bytes ~ sizeof(MEM), address rotation pattern */
			if(iDMemTestCase2() == FAILED) {
				printf("iDMemTestCase2 fail!!\n");
				retval = -1;	
			}
			else{
				printf("iDMemTestCase2 Passed!!\n");
			}
			break;
		case 3:
		/* IMEM0~L2MEM, size 4bytes ~ sizeof(MEM), ~address rotation pattern */
			if(iDMemTestCase3() == FAILED) {
				printf("iDMemTestCase3 fail!!\n");
				retval = -1;	
			}
			else{
				printf("iDMemTestCase3 Passed!!\n");
			}
			break;
		case 4:
		/* IMEM0~L2MEM, size 4bytes ~ sizeof(MEM), walking of 1 pattern */
			if(iDMemTestCase4() == FAILED) {
				printf("iDMemTestCase4 fail!!\n");
				retval = -1;	
			}
			else{
				printf("iDMemTestCase4 Passed!!\n");
			}
			break;
		case 5:
		/* IMEM0~L2MEM,  4bytes ~ sizeof(MEM), walking of 0 pattern */
			if(iDMemTestCase5() == FAILED) {
				printf("iDMemTestCase5 fail!!\n");
				retval = -1;	
			}
			else{
				printf("iDMemTestCase5 Passed!!\n");
			}
			break;
		case 6:
		/* IMEM0~L2MEM, size sizeof(MEM), random data pattern */
			if(iDMemTestCase6() == FAILED) {
				printf("iDMemTestCase6 fail!!\n");
				retval = -1;	
			}
			else{
				printf("iDMemTestCase6 Passed!!\n");
			}
			break;
		case 7:
		/* Pin pon running IMEM0/IMEM1 and DMA DMEM0/DMEM1 */
			if(iDMemTestCase7() == FAILED) {
				printf("iDMemTestCase7 fail!!\n");
				retval = -1;	
			}
			else{
				printf("iDMemTestCase7 Passed!!\n");
			}
			break;
		case 8:
		/* performance mesurement */
			if(iDMemTestCase8() == FAILED) {
				printf("iDMemTestCase8 fail!!\n");
				retval = -1;	
			}
			else{
				printf("iDMemTestCase8 Passed!!\n");
			}
			break;
		case 9:
		/* performance mesurement */
			if(iDMemTestCase9() == FAILED) {
				printf("iDMemTestCase9 fail!!\n");
				retval = -1;	
			}
			else{
				printf("iDMemTestCase9 Passed!!\n");
			}
			break;
		case 10:
		/* performance mesurement */
			if(iDMemTestCase10() == FAILED) {
				printf("iDMemTestCase10 fail!!\n");
				retval = -1;	
			}
			else{
				printf("iDMemTestCase10 Passed!!\n");
			}
			break;
		case 11:
		/* performance mesurement */
			if(iDMemTestCase11() == FAILED) {
				printf("iDMemTestCase11 fail!!\n");
				retval = -1;	
			}
			else{
				printf("iDMemTestCase11 Passed!!\n");
			}
			break;
		case 12:
		/* performance mesurement */
			if(iDMemTestCase12() == FAILED) {
				printf("iDMemTestCase12 fail!!\n");
				retval = -1;	
			}
			else{
				printf("iDMemTestCase12 Passed!!\n");
			}
			break;
		case 13:
			if(iDMemTestCase13() == FAILED) {
				printf("iDMemTestCase13 fail!!\n");
				retval = -1;	
			}
			else{
				printf("iDMemTestCase13 Passed!!\n");
			}
			break;
		default:
			break;
		}

	return (retval);
}

int idmem_test (int flag, int argc, char *argv[])
{
	int ret_code;

	ret_code = _idmemTest(1);
	if(ret_code < 0)
		return ret_code;
	ret_code = _idmemTest(2);
	if(ret_code < 0)
		return ret_code;
	ret_code = _idmemTest(3);
	if(ret_code < 0)
		return ret_code;
	ret_code = _idmemTest(4);
	if(ret_code < 0)
		return ret_code;
	ret_code = _idmemTest(5);
	if(ret_code < 0)
		return ret_code;
	ret_code = _idmemTest(6);
	if(ret_code < 0)
		return ret_code;
	ret_code = _idmemTest(7);
	if(ret_code < 0)
		return ret_code;
}
