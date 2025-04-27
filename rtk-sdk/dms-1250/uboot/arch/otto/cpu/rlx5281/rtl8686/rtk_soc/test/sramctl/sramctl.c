
/*
 * Include Files
 */
#include <common.h>
#include <exports.h>
#include "../../include/sramctl.h"
#include "../../include/memctl.h"


/*
 * Data Declaration
 */
unsigned int sram_patterns[] =	{
					0x00000000,
					0xffffffff,
					0x55555555,
					0xaaaaaaaa,
					0x01234567,
					0x76543210,
					0x89abcdef,
					0xfedcba98,
					0xA5A5A5A5,
					0x5A5A5A5A,
					0xF0F0F0F0,
					0x0F0F0F0F,
					0xFF00FF00,
					0x00FF00FF,
					0x0000FFFF,
					0xFFFF0000,
					0x00FFFF00,
					0xFF0000FF,
					0x5A5AA5A5,
					0xA5A55A5A
				};

unsigned int sram_line_toggle_pattern[] = {
					0xF0F0F0F0,
					0x0F0F0F0F,
					0xFF00FF00,
					0x00FF00FF,
					0x00000000,
					0xFFFFFFFF,
					0x5A5A5A5A,
					0xA5A5A5A5
				};

unsigned int sram_toggle_pattern[] = {
					0xA5A5A5A5,
					0x5A5A5A5A,
					0xF0F0F0F0,
					0x0F0F0F0F,
					0xFF00FF00,
					0x00FF00FF,
					0x0000FFFF,
					0xFFFF0000,
					0x00FFFF00,
					0xFF0000FF,
					0x5A5AA5A5,
					0xA5A55A5A
				};

/*
 * Function Declaration
 */


/* Function Name: 
 * 	sram_test
 * Descripton:
 *	Test cases for the SRAM controller.
 * Input:
 *	case_no
 *	 if 0 <= *case_no < total test case number
 *	 	- IP_TEST_CASE_ALL: Run all test cases in the sram_test function.
 *	 	- Others	  : Run single test case which the case number is case_no.
 *	 else
 *		*case_no = *case_no - total test case number
 *
 *	ip_base_addr	   
 *		- The base address of the SRAM controller configuration register.
 *	kick_value	   
 *		- The value to kick the IP hardware.
 *	burst_length	   
 *		- The burst length of the IP hardware.
 *	op_state	   
 *	 	- The operation flags of the test case. 
 *	 	  IP_TEST_OP_CONFIGURE: Configure the test pattern.
 *	 	  IP_TEST_OP_KICKOFF  : Kick off the hardware.
 *	  	  IP_TEST_OP_STATE_CHECKING: Check the result.
 *	src_start_addr	   
		- The start address of the source data buffer.
 *	src_byte_len	   
		- The number of bytes of the source data buffer.
 *	dist_start_addr	   
		- the start address of the distination data buffer.
 *	dist_byte_len	   
		- The number of bytes of the distination data buffer.
 * Output:
 * 	None
 * Return:
 *  	The number of completed cases.
 */
#define IP_TEST_OP_STATE_CONFIGURE	(0x00000001)
#define IP_TEST_OP_STATE_KICKOFF	(0x00000002)
#define IP_TEST_OP_STATE_CHECKING	(0x00000004)

#define IP_TEST_STATUS_PASS		(0x00000000)
#define IP_TEST_STATUS_FAIL		(0x00000001)
#define IP_TEST_ALL_CASE		(0xFFFFFFFF)

unsigned int _sramctl_size_number(unsigned int num_bytes){

	switch (num_bytes)
	{
		case 0x100:
				return SRAM_SIZE_256B;
		case 0x200:
				return SRAM_SIZE_512B;
		case 0x400:
				return SRAM_SIZE_1KB;
		case 0x800:
				return SRAM_SIZE_2KB;
		case 0x1000:
				return SRAM_SIZE_4KB;
		case 0x2000:
				return SRAM_SIZE_8KB;
		case 0x4000:
				return SRAM_SIZE_16KB;
		case 0x8000:
				return SRAM_SIZE_32KB;
		case 0x10000:
				return SRAM_SIZE_64KB;
		case 0x20000:
				return SRAM_SIZE_128KB;
		case 0x40000:
				return SRAM_SIZE_256KB;
		case 0x80000:
				return SRAM_SIZE_512KB;
		case 0x100000:
				return SRAM_SIZE_1MB;
		default:
				return 0;

	}
}
unsigned int _sram_test_single_segment_walking_of_1( unsigned int *case_no, unsigned int *test_status,\
	      unsigned int ip_base_addr, unsigned int *kick_value,\
	      unsigned int burst_length, unsigned int op_state,\
	      unsigned int src_start_addr, unsigned int src_byte_len,\
	      unsigned int dist_start_addr, unsigned int dist_byte_len)
{

	unsigned int comp_cases;
	unsigned int start_case, end_case, i, j;
	unsigned int walk_pattern, total_case_num;
	unsigned int start_value, seg_no;
	volatile unsigned int *read_start;
	volatile unsigned int *start;
	int retcode;

	/* Compute total case */
	total_case_num = 32;

	comp_cases = 0;

	if(*case_no == IP_TEST_ALL_CASE){	/* Test all cases */
		start_case = 0;
		end_case = total_case_num-1;
	}else if(*case_no >= total_case_num){	/* Not our test case number */
		*case_no = *case_no - total_case_num;
		*test_status = IP_TEST_STATUS_PASS;
		return comp_cases;
	}else{					/* Single test case */
		start_case = *case_no;
		end_case = *case_no;
	}

	for(i = start_case; i <= end_case; i++){
		/* Configure the test case */
		walk_pattern = 1<<i;
		seg_no = 0;
		if(op_state & IP_TEST_OP_STATE_CONFIGURE){
			/* Configure SRAM segment */
			if( 0 > sram_mapping(seg_no, src_start_addr, 0, _sramctl_size_number(src_byte_len))){
				goto go_out;
			}
			if( 0 > memctl_unmapping(seg_no, src_start_addr, _sramctl_size_number(src_byte_len))){
				sram_unmapping(seg_no);
				goto go_out;

			}
       			/* Write data */
        		start = (unsigned int *)(src_start_addr);
        		read_start = (unsigned int *)(TO_UNCACHED_ADDR((unsigned int)start+src_byte_len-4));
        		for(j=0; j < src_byte_len; j=j+4)
        		{
            			*start = walk_pattern;
            			start++;
        		}
		}
	
		/* Kick it off */
		if(op_state & IP_TEST_OP_STATE_KICKOFF){
			_cache_flush();
		}

		/* Checking the result. */
		if(op_state & IP_TEST_OP_STATE_CHECKING){
       			/* check data */
			retcode = 0;
        		for(j=0; j < src_byte_len; j=j+4)
        		{
	    			start_value = (*read_start);
            			if(start_value != walk_pattern)
            			{
                			printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
                        			read_start , start_value, walk_pattern,  __FUNCTION__, __LINE__);
                 			retcode = -1;
            			}
            			read_start--;
        		}
			
			/* Assigned *test_status with IP_TEST_STATUS_PASS if the checking is ok. */
			if(retcode != -1){
				*test_status = IP_TEST_STATUS_PASS;
				comp_cases++;
				sram_unmapping(seg_no);
				memctl_unmapping_disable(seg_no);
				printf("pattern[%d](0x%x) 0x%x passed\r", i, walk_pattern, start);
			}else{
				*test_status = IP_TEST_STATUS_FAIL;
				goto go_out;
			}

		}
	}
go_out:
	return comp_cases;
}

#define TEST_AREA_SIZE (memctlc_dram_size() - 0x800000)
int sram_test(int flag, int argc, char *argv[])
{
	unsigned int case_no;
	unsigned int kick_value;
	unsigned int test_status;
	unsigned int op_state;
	unsigned int complete_case;
	unsigned int start_address;
	unsigned int test_size;

	case_no = IP_TEST_ALL_CASE;
	op_state = (IP_TEST_OP_STATE_CONFIGURE|IP_TEST_OP_STATE_KICKOFF|IP_TEST_OP_STATE_CHECKING);

	for(test_size=0x100; test_size<=0x100000; test_size = test_size<<1){
		printf("SRAM Size = 0x%08x\n", test_size);
		for(start_address=0xa0000000; start_address < (0xa0000000 + TEST_AREA_SIZE); start_address = start_address+test_size){
			complete_case = _sram_test_single_segment_walking_of_1(&case_no, &test_status, \
              			0xB8004000, &kick_value, 0, op_state, start_address, test_size, start_address, test_size);
	
			printf("\n");
		}
	}
	printf("\nsram test completed.\n");
	return 0;
}


#if 0
int sram_test(unsigned int start_case_no, unsigned int end_case_no, unsigned int *test_status\
              unsigned int ip_base_addr, unsigned int *kick_value,\
              unsigned int burst_length,
              unsigned int src_start_addr, unsigned int src_byte_len,\
              unsigned int dist_start_addr, unsigned int dist_byte_len)
{
	unsigned int test_status;
	unsigned int kick_value;
	unsigned int op_state;
	unsigned int total_case_no;

	/* 1. count the test case number */
	total_case_no = _sram_test_single_segment(IP_TEST_ALL_CASE, test_status, ip_base_addr, \
				  kick_value, burst_length, IP_TEST_OP_STATE_COUNT_CASE, src_start_addr, \
				  src_byte_len, dist_start_addr, dist_byte_len);


	/* 2. single case */
	for(case_no = start_case_no; case_no <= end_case_no; case_no++){
		if(0 == _sram_test_single_segment(case_no, test_status, ip_base_addr, \
                                  kick_value, burst_length, IP_TEST_OP_CONFIGURE|IP_TEST_OP_KICKOFF|IP_TEST_OP_STATE_CHECKING, \
				  src_start_addr, src_byte_len, dist_start_addr, dist_byte_len)){
			/* Fail no case_no case */
			IP_TEST_PROCESS_FAIL(case_no);
		}
	}

	return 0;
}
#endif

