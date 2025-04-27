#include "concur_test.h"
#include "concur_idmem.h"

static unsigned char *dmem0_base;
static unsigned char *dmem1_base;
static unsigned char *imem0_base;
static unsigned char *imem1_base;
static unsigned char *l2mem_base;
static unsigned int src[0x2000];
static unsigned int dist[0x2000];
static unsigned int data_patterns[] = { 0xFFFFFFFF, 0x00000000, 0xAAAAAAAA, 0x55555555, 0x01234567, 0x89ABCDEF};



#define INIT_BYTE_VALUE (0xCA)
#define INIT_WORD_VALUE (0xCACACACA)
#define CHECK_INIT_VALUE
//#undef CHECK_INIT_VALUE
//#define INIT_BYTE_VALUE (0x00)
//#define INIT_WORD_VALUE (0x00000000)
extern void init_Env(unsigned int flag);
static char *targetName[] = {"IMEM0", "IMEM1", "DMEM0", "DMEM1", "L2MEM"};
static unsigned int targetSize[] = { IMEM0_SIZE,  IMEM1_SIZE,  DMEM0_SIZE,  DMEM1_SIZE, L2MEM_SIZE };
unsigned int IDMEM_cases_setting(unsigned int test_mode, unsigned int target, unsigned int test_word_size,\
				unsigned int direction, unsigned int pattern)
{
	int j;
	volatile unsigned int *uc_src;
	volatile unsigned int *uc_dist;
	volatile unsigned int *pMem;
	unsigned int tmp;


	/* Initialize the environment */

	uc_src = (unsigned int *)TO_UNCACHED_ADDR(&src) ;
	uc_dist = (unsigned int *)TO_UNCACHED_ADDR(&dist) ;
	
//	printf("uc_src = 0x%x, uc_dist = 0x%x\n", uc_src, uc_dist);

	init_Env(0);
#if 0
	if(test_mode==0)
		printf("mode: store and forward \n");
	else
		printf("mode: Acceleration\n");
		/* init src and dist */
#endif
	write_back_invalidate_dcache();
	for(j=0; j<(sizeof(src)/4); j++)
	{
		uc_dist[j] = INIT_WORD_VALUE;
		uc_src[j] = INIT_WORD_VALUE;
	}
	//printf("\n target: %s, size: 0x%08x\n", targetName[target], targetSize[target]);	
		
	//printf("\t\n Pattern: 0x%08x\n", pattern);	

	write_back_invalidate_dcache();

	if(direction == DIR_SDRAM_TO_IMEMDMEM){
		/* Set source data */
		for(j=0; j<test_word_size; j++)
		{
			uc_src[j] = pattern;
		}
		/* flush OCP pending write, including buffered data in memory controller */
		write_back_invalidate_dcache();
		tmp = uc_src[j-1];
		/* DMA src into MEM */
		return setImemDmemDMA((unsigned int)uc_src, CACHED_TARGET_DMEM0_ADDR, CACHED_TARGET_DMEM0_ADDR, \
				test_word_size*4, DIR_SDRAM_TO_IMEMDMEM, target, test_mode);
	}
	else{
		if(target == SEL_IMEM0){
			enable_IMEM0();
			pMem = (unsigned int *)CACHED_TARGET_IMEM0_ADDR;
			for(j=0; j<test_word_size; j = j + 2)
				write_imem_8bytes(pattern, pattern, &pMem[j]);
		}
		else if(target == SEL_IMEM1){
			enable_IMEM1();
			pMem = (unsigned int *)CACHED_TARGET_IMEM1_ADDR;
			for(j=0; j<test_word_size; j = j + 2)
				write_imem_8bytes(pattern, pattern, &pMem[j]);
		}
		else {
			switch (target){
				case SEL_DMEM0:
					enable_DMEM0();
					pMem = (unsigned int *)CACHED_TARGET_DMEM0_ADDR;
					break;
				case SEL_DMEM1:
					enable_DMEM1();
					pMem = (unsigned int *)CACHED_TARGET_DMEM1_ADDR;
					break;
				case SEL_L2MEM:
					enable_L2MEM();
					pMem = (unsigned int *)CACHED_TARGET_L2MEM_ADDR;
					break;
			}
			for(j=0; j<test_word_size; j++)
				pMem[j] = pattern;
		}
		/* DMA Data out form MEM into distination */
		return setImemDmemDMA((unsigned int)uc_dist, CACHED_TARGET_DMEM0_ADDR, CACHED_TARGET_DMEM0_ADDR, \
			test_word_size*4, DIR_IMEMDMEM_TO_SDRAM, target, test_mode);
	}
}


int IDMEM_cases_checking(unsigned int target, unsigned int test_word_size, \
				unsigned int direction, unsigned int pattern)
{
	
	int j;
	volatile unsigned int *pMem;
	volatile unsigned int *uc_src;
	volatile unsigned int *uc_dist;
	unsigned int imem_read_buff[2];
	unsigned int *chk_data;

	uc_src = (unsigned int *)TO_UNCACHED_ADDR(&src) ;
	uc_dist = (unsigned int *)TO_UNCACHED_ADDR(&dist) ;

	if(direction == DIR_SDRAM_TO_IMEMDMEM)
		chk_data = &uc_src[0];
	else
		chk_data = &uc_dist[0];

#ifdef CHECK_SOURCE
	/* check src data first */
	for(j=0; j<test_word_size; j++)
	{
		if(chk_data[j] != pattern)
		{
			printf("\n\t%s %d:src[%d](0x%x) != pattern (0x%x)\n", \
				__FUNCTION__, __LINE__, j, chk_data[j], pattern);
			retcode = FAILED;
		}
	}
#endif
		
	if((target==SEL_DMEM0) || (target==SEL_DMEM1) || (target == SEL_L2MEM))
	{
		/* Enable  DMEM */
		write_back_invalidate_dcache();
		/* Compare src and dmem */
		if(target==SEL_DMEM0)
		{
			enable_DMEM0();
			pMem = (unsigned int *)CACHED_TARGET_DMEM0_ADDR;
		}
		else if (target==SEL_DMEM1)
		{
			enable_DMEM1();
			pMem = (unsigned int *)CACHED_TARGET_DMEM1_ADDR;
		}
		else
		{
			enable_L2MEM();
			pMem = (unsigned int *)CACHED_TARGET_L2MEM_ADDR;
		}
		/* check data in MEM */
		for(j=0; j<(targetSize[target]/4); j++)
		{
			if(chk_data[j] != *pMem)
			{
				printf("\n\t%s %d:src[%d](0x%x) != MEM 0x%x (0x%x)\n", \
					__FUNCTION__, __LINE__, j, chk_data[j], pMem, *pMem);
				goto test_fail;
			}
			pMem++;
		}
		if(target==SEL_DMEM0)
			disable_DMEM0();
		else if (target==SEL_DMEM1)
			disable_DMEM1();
		else
			disable_L2MEM();
	}
	else
	{
		if(target==SEL_IMEM0)
		{
			enable_IMEM0();
			pMem = (unsigned int *)CACHED_TARGET_IMEM0_ADDR;
		}
		else
		{
			enable_IMEM1();
			pMem = (unsigned int *)CACHED_TARGET_IMEM1_ADDR;
		}

		/* Check data in IMEM by cache instruction */
		for(j=0; j<test_word_size; j = j + 2)
		{
			if((test_word_size - j) < 2) /*only compare low 4 bytes*/
			{
				read_imem_8bytes(pMem , &imem_read_buff[0]);
				if(chk_data[j] != imem_read_buff[0])
				{
					printf(\
					"\n\t%s %d:src[%d](0x%x) != MEM 0x%x (0x%x)\n"\
					, __FUNCTION__, __LINE__, j, chk_data[j], pMem, \
					imem_read_buff[0]);
					goto test_fail;
				}
			}
			else
			{
				if(chk_data[j] != pattern || chk_data[j+1] != pattern){
					printf("chk_data = 0x%x\n", chk_data);
					printf(\
					"\n\t%s %d: chk[%d](0x%x)(%x) != pattern (0x%x)\n"\
					, __FUNCTION__, __LINE__, j,  chk_data[j],chk_data[j+1], pattern);
					goto test_fail;
				}
				read_imem_8bytes(pMem , &imem_read_buff[0]);
				if(chk_data[j] != imem_read_buff[0])
				{
					printf(\
					"\n\t%s %d:src[%d](0x%x) != MEM 0x%x (0x%x)\n"\
					, __FUNCTION__, __LINE__, j, chk_data[j], pMem, \
					imem_read_buff[0]);
					goto test_fail;
				}
				if(chk_data[j+1] != imem_read_buff[1])
				{
					printf(\
					"\n\t%s %d:src[%d](0x%x) != MEM 0x%x (0x%x)\n"\
					, __FUNCTION__, __LINE__, j+1, chk_data[j+1], \
					pMem+1, imem_read_buff[1]);
					goto test_fail;
				}
			}
			pMem = pMem + 2;
		}

		if(target==SEL_IMEM0)
			disable_IMEM0();
		else
			disable_IMEM1();

	}
	return CHECKING_PASS;

test_fail:
	disable_IMEM0();
	disable_IMEM1();
	disable_DMEM0();
	disable_DMEM1();
	disable_L2MEM();

	return CHECKING_FAIL;

}

/* Pollng IDMEM DMA status */
int IDMEM_nonb_polling(void)
{
        if ( (REG(DMDMA_CTL_REG)&IMEMDMEM_START) ){
                return IP_BUSY;
        }
        else
                return IP_IDLE;
}

int IDMEM_checking(int caseNo)
{
        switch (caseNo)
        {
		case 0 ... 5:
			return IDMEM_cases_checking( SEL_IMEM0, IMEM0_SIZE/4, \
					DIR_SDRAM_TO_IMEMDMEM ,data_patterns[caseNo]);
			break;
		case 6 ... 11:
			return IDMEM_cases_checking( SEL_IMEM1, IMEM1_SIZE/4, \
					DIR_SDRAM_TO_IMEMDMEM ,data_patterns[caseNo-6]);
			break;
		case 12 ... 17:
			return IDMEM_cases_checking( SEL_DMEM0, DMEM0_SIZE/4, \
					DIR_SDRAM_TO_IMEMDMEM ,data_patterns[caseNo-12]);
			break;
		case 18 ... 23:
			return IDMEM_cases_checking( SEL_DMEM1, DMEM1_SIZE/4, \
					DIR_SDRAM_TO_IMEMDMEM ,data_patterns[caseNo-18]);
			break;
		case 24 ... 29:
			return IDMEM_cases_checking( SEL_L2MEM, L2MEM_SIZE/4, \
					DIR_SDRAM_TO_IMEMDMEM ,data_patterns[caseNo-24]);
			break;
		case 30 ... 35:
			return IDMEM_cases_checking( SEL_IMEM0, IMEM0_SIZE/4, \
					DIR_IMEMDMEM_TO_SDRAM ,data_patterns[caseNo-30]);
		case 36 ... 41:
			return IDMEM_cases_checking( SEL_IMEM1, IMEM1_SIZE/4, \
					DIR_IMEMDMEM_TO_SDRAM ,data_patterns[caseNo-36]);
			break;
		case 42 ... 47:
			return IDMEM_cases_checking( SEL_DMEM0, DMEM0_SIZE/4, \
					DIR_IMEMDMEM_TO_SDRAM ,data_patterns[caseNo-42]);
			break;
		case 48 ... 53:
			return IDMEM_cases_checking( SEL_DMEM1, DMEM1_SIZE/4, \
					DIR_IMEMDMEM_TO_SDRAM ,data_patterns[caseNo-48]);
			break;
		case 54 ... 59:
			return IDMEM_cases_checking( SEL_L2MEM, L2MEM_SIZE/4, \
					DIR_IMEMDMEM_TO_SDRAM ,data_patterns[caseNo-54]);
			break;
                default:
                        return NO_SUCH_CASE;
                        break;
        }
        return NO_SUCH_CASE;
}

unsigned int IDMEM_setting(int caseNo)
{
        switch (caseNo)
        {
		case 0 ... 5:
			return IDMEM_cases_setting( STORE_FORWARD_MODE, SEL_IMEM0, DMEM0_SIZE/4, \
					DIR_SDRAM_TO_IMEMDMEM ,data_patterns[caseNo]);
			break;
		case 6 ... 11:
			return IDMEM_cases_setting( STORE_FORWARD_MODE, SEL_IMEM1, IMEM1_SIZE/4, \
					DIR_SDRAM_TO_IMEMDMEM ,data_patterns[caseNo-6]);
			break;
		case 12 ... 17:
			return IDMEM_cases_setting( STORE_FORWARD_MODE, SEL_DMEM0, DMEM0_SIZE/4, \
					DIR_SDRAM_TO_IMEMDMEM ,data_patterns[caseNo-12]);
			break;
		case 18 ... 23:
			return IDMEM_cases_setting( STORE_FORWARD_MODE, SEL_DMEM1, DMEM1_SIZE/4, \
					DIR_SDRAM_TO_IMEMDMEM ,data_patterns[caseNo-18]);
			break;
		case 24 ... 29:
			return IDMEM_cases_setting( STORE_FORWARD_MODE, SEL_L2MEM, L2MEM_SIZE/4, \
					DIR_SDRAM_TO_IMEMDMEM ,data_patterns[caseNo-24]);
			break;
		case 30 ... 35:
			return IDMEM_cases_setting( STORE_FORWARD_MODE, SEL_IMEM0, IMEM0_SIZE/4, \
					DIR_IMEMDMEM_TO_SDRAM ,data_patterns[caseNo-30]);
			break;
		case 36 ... 41:
			return IDMEM_cases_setting( STORE_FORWARD_MODE, SEL_IMEM1, IMEM1_SIZE/4, \
					DIR_IMEMDMEM_TO_SDRAM ,data_patterns[caseNo-36]);
			break;
		case 42 ... 47:
			return IDMEM_cases_setting( STORE_FORWARD_MODE, SEL_DMEM0, DMEM0_SIZE/4, \
					DIR_IMEMDMEM_TO_SDRAM ,data_patterns[caseNo-42]);
			break;
		case 48 ... 53:
			return IDMEM_cases_setting( STORE_FORWARD_MODE, SEL_DMEM1, DMEM1_SIZE/4, \
					DIR_IMEMDMEM_TO_SDRAM ,data_patterns[caseNo-48]);
			break;
		case 54 ... 59:
			return IDMEM_cases_setting( STORE_FORWARD_MODE, SEL_L2MEM, L2MEM_SIZE/4, \
					DIR_IMEMDMEM_TO_SDRAM ,data_patterns[caseNo-54]);
			break;
                default:
                        return NO_SUCH_CASE;
                        break;
        }
        return NO_SUCH_CASE;
}
