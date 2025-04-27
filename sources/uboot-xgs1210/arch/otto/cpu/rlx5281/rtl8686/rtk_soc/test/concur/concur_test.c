#include "concur_test.h"
#include "concur_gdma.h"
#include "concur_idmem.h"
#include "concur_cpu.h"
#include "../../include/sramctl.h"
#include "../../include/memctl.h"
#define TEST_TIMES (500)

#define MEMSET_BUF_SIZE (0x800)

//#define printf(...)

unsigned int _gdma_memset_sbuf[MEMSET_BUF_SIZE];
unsigned int _gdma_memset_dbuf[MEMSET_BUF_SIZE];

unsigned int _sgdma_memset_sbuf[MEMSET_BUF_SIZE];
unsigned int _sgdma_memset_dbuf[MEMSET_BUF_SIZE];

void config_sram(unsigned int config_no)
{
	volatile unsigned int *sram_swicther;


	switch (config_no){
		/* DRAM case */
		case 0: 
			printf("DRAM src/dist:\n");
			memctl_unmapping_disable(0);
			memctl_unmapping_disable(1);
			memctl_unmapping_disable(2);
			memctl_unmapping_disable(3);
			sram_unmapping(0);
			sram_unmapping(1);
			sram_unmapping(2);
			sram_unmapping(3);
			break;
		/* SRAM case */
		case 1:
			/* Swtich the 192KB SRAM to SOC */
			sram_swicther = (volatile unsigned int *)0xBB0001e0;
			*sram_swicther = 0;
			printf("SRAM src/dist:\n");
			sram_mapping(0, 0x80000000, 0x0, SRAM_SIZE_64KB);
			memctl_unmapping(0, 0x80000000, MEMCTL_UNMAP_SIZE_64KB);
			break;
		default:
			break;

	}

	return;

}
void unconfig_sram(unsigned int config_no)
{
	volatile unsigned int *sram_swicther;

	switch (config_no){
		/* DRAM case */
		case 0: 
			break;
		/* sram case */
		case 1: 
			memctl_unmapping_disable(0);
			memctl_unmapping_disable(1);
			memctl_unmapping_disable(2);
			memctl_unmapping_disable(3);
			sram_unmapping(0);
			sram_unmapping(1);
			sram_unmapping(2);
			sram_unmapping(3);

			/* Swtich the 192KB SRAM to Switch */
			sram_swicther = (volatile unsigned int *)0xBB0001e0;
			*sram_swicther = 1;
			break;
		default:
			break;

	}

}

int concur_test (int flag, int argc, char *argv[])
{
	int gdma_status, sgdma_status,idmem_status;
	int gdma_caseNo, sgdma_caseNo,idmem_caseNo, cpu_caseNo;
	unsigned int test_times, i;
	int gdma_max_case, sgdma_max_case, idmem_max_case, cpu_max_case;
	volatile unsigned int *gdma_memset_sbuf, *gdma_memset_dbuf, *sgdma_memset_sbuf, *sgdma_memset_dbuf;
	unsigned int gdma_memset_sbuf_size, gdma_memset_dbuf_size, sgdma_memset_sbuf_size, sgdma_memset_dbuf_size;
	unsigned int gdma_kick_value;
	unsigned int sgdma_kick_value;
	unsigned int idmem_kick_value;
	unsigned int idmem_complete;
	unsigned int gdma_complete;
	unsigned int sgdma_complete;
	unsigned int cpu_complete;
	unsigned int cpu_write_value;
	unsigned int gdma_base;
	unsigned int sgdma_base;
	unsigned int config_case;

	cpu_max_case 		= 7;
	gdma_max_case 		= 7;
	sgdma_max_case 		= 7;
	idmem_max_case 		= 5;
	gdma_base 		= 0xB800a000;
	sgdma_base		= 0xB8018000;
	gdma_memset_sbuf 	= (unsigned int *)0x80000000;
	gdma_memset_dbuf 	= (unsigned int *)0x80002000;
	sgdma_memset_sbuf 	= (unsigned int *)0x80004000;
	sgdma_memset_dbuf 	= (unsigned int *)0x80006000;
	gdma_memset_sbuf_size 	= sizeof(_gdma_memset_sbuf); 
	gdma_memset_dbuf_size 	= sizeof(_gdma_memset_dbuf);
	sgdma_memset_sbuf_size 	= sizeof(_sgdma_memset_sbuf);
	sgdma_memset_dbuf_size 	= sizeof(_sgdma_memset_dbuf);
	
//for(config_case = 0; config_case<2; config_case++){
for(config_case = 0; config_case<1; config_case++){
	config_sram(config_case);
	/* 
	 * Sequential access and blocking on polling 
	 */
	/* initialize the counter */
	idmem_complete = 0;
	gdma_complete = 0;
	sgdma_complete = 0;
	cpu_complete = 0;
	test_times = TEST_TIMES;	
	gdma_status = IP_IDLE;
	sgdma_status = IP_IDLE;
	idmem_status = IP_IDLE;
	gdma_caseNo  = 0;
	sgdma_caseNo  = 4;
	idmem_caseNo = 0;

	printf("Sequential access and blocking on polling\n");
	for(i=0; i< test_times; i++){
		printf("\r");
		/* CPU */
		printf("cpu: %05d, ", cpu_complete);
		cpu_write_value = CPU_setting(cpu_caseNo);
		CPU_KICKOFF(cpu_caseNo);
		cpu_complete++;
		if(CPU_checking(cpu_caseNo, cpu_write_value) == CHECKING_FAIL)
			return -1;
		cpu_caseNo = (cpu_caseNo + 1) % cpu_max_case;

#if 1
		/* GDMA */
		printf("gdma %05d, ", gdma_complete);
		gdma_kick_value = GDMA_setting(gdma_caseNo, gdma_base, (unsigned int)gdma_memset_sbuf, gdma_memset_sbuf_size, \
						(unsigned int)gdma_memset_dbuf, gdma_memset_dbuf_size);
		GDMA_KICKOFF(gdma_base, gdma_kick_value);
		while(GDMA_nonb_polling(gdma_base) == IP_BUSY);
		if(GDMA_checking(gdma_caseNo, gdma_base, (unsigned int)gdma_memset_sbuf, gdma_memset_sbuf_size, \
                                 (unsigned int)gdma_memset_dbuf, gdma_memset_dbuf_size) == CHECKING_FAIL)
			return -1;
		gdma_complete++;
		gdma_caseNo = (gdma_caseNo + 1) % gdma_max_case;
#endif
#if 1
		/* GDMA 2*/
		printf("sgdma %05d, ", sgdma_complete);
		sgdma_kick_value = GDMA_setting(sgdma_caseNo, sgdma_base, (unsigned int)sgdma_memset_sbuf, sgdma_memset_sbuf_size, \
						(unsigned int)sgdma_memset_dbuf, sgdma_memset_dbuf_size);
		GDMA_KICKOFF(sgdma_base, sgdma_kick_value);
		while(GDMA_nonb_polling(sgdma_base) == IP_BUSY);
		if(GDMA_checking(sgdma_caseNo, sgdma_base, (unsigned int)sgdma_memset_sbuf, sgdma_memset_sbuf_size, \
                                 (unsigned int)sgdma_memset_dbuf, sgdma_memset_dbuf_size) == CHECKING_FAIL)
			return -1;
		sgdma_complete++;
		sgdma_caseNo = (sgdma_caseNo + 1) % sgdma_max_case;
#endif
		/* IDMEM DMA */
		printf(" idmem %05d ", idmem_complete);
		idmem_kick_value = IDMEM_setting(idmem_caseNo);
		IDMEM_KICKOFF(idmem_kick_value);
		while(IDMEM_nonb_polling() == IP_BUSY);
		if(IDMEM_checking(idmem_caseNo) == CHECKING_FAIL)
			return -1;
		idmem_complete++;
		idmem_caseNo = (idmem_caseNo + 1) % idmem_max_case;
	}

	printf("\n");

	/* 
	 * Block on polling 
	 */
	/* initialize the counter */
	idmem_complete = 0;
	gdma_complete = 0;
	sgdma_complete = 0;
	cpu_complete = 0;
	test_times = TEST_TIMES;	
	gdma_status = IP_IDLE;
	sgdma_status = IP_IDLE;
	idmem_status = IP_IDLE;
	gdma_caseNo  = 0;
	sgdma_caseNo  = 0;
	idmem_caseNo = 0;
	cpu_caseNo = 0;

	printf("Concurrent access and blocking on polling\n");
	for(i=0; i< test_times; i++){

		printf("\rcpu %05d, gdma %05d, sgdma %05d, idmem %05d ", cpu_complete, gdma_complete, sgdma_complete, idmem_complete);

		cpu_write_value = CPU_setting(cpu_caseNo);
		gdma_kick_value = GDMA_setting(gdma_caseNo, gdma_base, (unsigned int)gdma_memset_sbuf, gdma_memset_sbuf_size, \
                                                (unsigned int)gdma_memset_dbuf, gdma_memset_dbuf_size);
		sgdma_kick_value = GDMA_setting(sgdma_caseNo, sgdma_base, (unsigned int)sgdma_memset_sbuf, sgdma_memset_sbuf_size, \
                                                (unsigned int)sgdma_memset_dbuf, sgdma_memset_dbuf_size);
		idmem_kick_value = IDMEM_setting(idmem_caseNo);

		GDMA_KICKOFF(gdma_base, gdma_kick_value);
		GDMA_KICKOFF(sgdma_base, sgdma_kick_value);
		IDMEM_KICKOFF(idmem_kick_value);
		CPU_KICKOFF(cpu_caseNo);

		while(GDMA_nonb_polling(gdma_base) == IP_BUSY);
		while(GDMA_nonb_polling(sgdma_base) == IP_BUSY);
		while(IDMEM_nonb_polling() == IP_BUSY);

		gdma_complete++;
		sgdma_complete++;
		if( GDMA_checking(gdma_caseNo, gdma_base, (unsigned int)gdma_memset_sbuf, gdma_memset_sbuf_size, \
                                 (unsigned int)gdma_memset_dbuf, gdma_memset_dbuf_size)==CHECKING_FAIL )
			return -1;
		gdma_caseNo = (gdma_caseNo + 1) % gdma_max_case;


		if( GDMA_checking(sgdma_caseNo, sgdma_base, (unsigned int)sgdma_memset_sbuf, sgdma_memset_sbuf_size, \
                                 (unsigned int)sgdma_memset_dbuf, sgdma_memset_dbuf_size)==CHECKING_FAIL )
			return -1;
		sgdma_caseNo = (sgdma_caseNo + 1) % sgdma_max_case;

		idmem_complete++;
		if( IDMEM_checking(idmem_caseNo) == CHECKING_FAIL )
			return -1;
		idmem_caseNo = (idmem_caseNo + 1) % idmem_max_case;
	
		cpu_complete++;
		if( CPU_checking(cpu_caseNo, cpu_write_value) == CHECKING_FAIL )
				return -1;
		cpu_caseNo = (cpu_caseNo + 1) % cpu_max_case;

	}

	printf("\n");

	/* 
	 * No blocking on polling 
	 */
	/* initialize the counter */
	idmem_complete = 0;
	gdma_complete = 0;
	sgdma_complete = 0;
	cpu_complete = 0;
	test_times = TEST_TIMES;	
	gdma_status = IP_IDLE;
	sgdma_status = IP_IDLE;
	idmem_status = IP_IDLE;
	gdma_caseNo  = 0;
	sgdma_caseNo  = 0;
	idmem_caseNo = 0;

	printf("Concurrent access and no blocking on polling\n");
	for(i=0; i< test_times; i++){
		printf("\rcpu %05d, gdma %05d, sgdma %05d, idmem %05d ", cpu_complete, gdma_complete, sgdma_complete, idmem_complete);

		if(gdma_status == IP_IDLE)
			gdma_kick_value = GDMA_setting(gdma_caseNo, gdma_base, (unsigned int)gdma_memset_sbuf, gdma_memset_sbuf_size, \
                                                (unsigned int)gdma_memset_dbuf, gdma_memset_dbuf_size);

		if(sgdma_status == IP_IDLE)
			sgdma_kick_value = GDMA_setting(sgdma_caseNo, sgdma_base, (unsigned int)sgdma_memset_sbuf, sgdma_memset_sbuf_size, \
                                                (unsigned int)sgdma_memset_dbuf, sgdma_memset_dbuf_size);

		if(idmem_status == IP_IDLE)
			idmem_kick_value = IDMEM_setting(idmem_caseNo);

		cpu_write_value = CPU_setting(cpu_caseNo);

		if(gdma_status == IP_IDLE)
			GDMA_KICKOFF(gdma_base, gdma_kick_value);
		if(sgdma_status == IP_IDLE)
			GDMA_KICKOFF(sgdma_base, sgdma_kick_value);
		if(idmem_status == IP_IDLE)
			IDMEM_KICKOFF(idmem_kick_value);
		CPU_KICKOFF(cpu_caseNo);

		gdma_status = GDMA_nonb_polling(gdma_base);
		sgdma_status = GDMA_nonb_polling(sgdma_base);
		idmem_status = IDMEM_nonb_polling();

		cpu_complete++;
		if(CPU_checking(cpu_caseNo, cpu_write_value) == CHECKING_FAIL)
			return -1;
		cpu_caseNo = (cpu_caseNo + 1) % cpu_max_case;

		if(gdma_status == IP_IDLE){
			gdma_complete++;
			if(GDMA_checking(gdma_caseNo, gdma_base, (unsigned int)gdma_memset_sbuf, gdma_memset_sbuf_size, \
                                 (unsigned int)gdma_memset_dbuf, gdma_memset_dbuf_size) == CHECKING_FAIL)
				return -1;
			gdma_caseNo = (gdma_caseNo + 1) % gdma_max_case;
		}

		if(sgdma_status == IP_IDLE){
			sgdma_complete++;
			if(GDMA_checking(sgdma_caseNo, sgdma_base, (unsigned int)sgdma_memset_sbuf, sgdma_memset_sbuf_size, \
                                 (unsigned int)sgdma_memset_dbuf, sgdma_memset_dbuf_size) == CHECKING_FAIL)
				return -1;
			sgdma_caseNo = (sgdma_caseNo + 1) % sgdma_max_case;
		}

		if(idmem_status == IP_IDLE){
			idmem_complete++;
			if(IDMEM_checking(idmem_caseNo) == CHECKING_FAIL)
				return -1;
			idmem_caseNo = (idmem_caseNo + 1) % idmem_max_case;
		}
	}
	printf("\n");
	unconfig_sram(config_case);
}
	return 0;
}


