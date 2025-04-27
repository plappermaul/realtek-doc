#include <asm/arch/rtk_soc_common.h>
#include <asm/arch/idmem_dma.h>
#include <asm/arch/rlx5281.h>
#include "idmem_test.h"

//#undef READ_FLUSH_OCP_WRITE
#define READ_FLUSH_OCP_WRITE
#define MEMCTL_DEBUG_PRINTF printf
//#define MEMCTL_DEBUG_PRINTF(...)


void init_Env(unsigned int flag);

unsigned char *dmem0_base;
unsigned char *dmem1_base;
unsigned char *imem0_base;
unsigned char *imem1_base;
unsigned char *l2mem_base;
unsigned int src[0x2000];
unsigned int dist[0x2000];
unsigned int idmem_test_sel = IDMEM_TEST_SEL;
//unsigned int data_patterns[] = { 0xFFFFFFFF, 0x00000000, 0xAAAAAAAA, 0x55555555, 0x01234567, 0x89ABCDEF};
unsigned int data_patterns[] = { 
				 0xCACACACA,
				 0xF0F0F0F0, 
				 0x0F0F0F0F, 
				 0xFF00FF00, 
				 0x00FF00FF,
				 0x00FFFF00,
				 0xFF0000FF, 
				 0xFFFF0000, 
				 0x0000FFFF, 
				 0xFFFFFFFF, 
				 0x00000000, 
				 0x5A5A5A5A,
				 0xA5A5A5A5,
				 0x5A5AA5A5,
				 0xA5A55A5A
				};


#define INIT_BYTE_VALUE (0xCA)
#define INIT_WORD_VALUE (0xCACACACA)
#define CHECK_INIT_VALUE
//#undef CHECK_INIT_VALUE
//#define INIT_BYTE_VALUE (0x00)
//#define INIT_WORD_VALUE (0x00000000)
void init_Env(unsigned int flag)
{
	unsigned int *pData;
	unsigned int i;

	memset((unsigned char *)src, INIT_BYTE_VALUE, sizeof(src));
	memset((unsigned char *)dist, INIT_BYTE_VALUE, sizeof(dist));
	enable_CP3();

	if(idmem_test_sel & DMEM0_MASK)
		load_dmem0Range(DMEM0_TOP_ADDR, DMEM0_BASE_ADDR);
	if(idmem_test_sel & DMEM1_MASK){
		load_dmem1Range(DMEM1_TOP_ADDR, DMEM1_BASE_ADDR);
		MEMCTL_DEBUG_PRINTF("DMEM1_TOP_ADDR = 0x%08x, DMEM1_BASE_ADDR = 0x%08x\n", DMEM1_TOP_ADDR, DMEM1_BASE_ADDR);
	}
	if(idmem_test_sel & IMEM0_MASK)
		load_imem0Range(IMEM0_TOP_ADDR, IMEM0_BASE_ADDR);
	if(idmem_test_sel & IMEM1_MASK)
		load_imem1Range(IMEM1_TOP_ADDR, IMEM1_BASE_ADDR);
	if(idmem_test_sel & L2MEM_MASK){
		load_l2memRange(L2MEM_TOP_ADDR, L2MEM_BASE_ADDR);
		MEMCTL_DEBUG_PRINTF("L2MEM_TOP_ADDR = 0x%08x, L2MEM_BASE_ADDR = 0x%08x\n", L2MEM_TOP_ADDR, L2MEM_BASE_ADDR);
	}
	
	/* Write back and invalidate dcache */	
	write_back_invalidate_dcache();
	if(idmem_test_sel & DMEM0_MASK){
		enable_DMEM0();
		memset((unsigned char *)CACHED_TARGET_DMEM0_ADDR, INIT_BYTE_VALUE, DMEM0_SIZE);
	}
	
	if(idmem_test_sel & DMEM1_MASK){
		enable_DMEM1();
		memset((unsigned char *)CACHED_TARGET_DMEM1_ADDR, INIT_BYTE_VALUE, DMEM1_SIZE);
	}

	if(idmem_test_sel & L2MEM_MASK){
		enable_L2MEM();
		memset((unsigned char *)CACHED_TARGET_L2MEM_ADDR, INIT_BYTE_VALUE, L2MEM_SIZE);
	}

	write_back_invalidate_dcache();

#ifdef CHECK_INIT_VALUE
	if(idmem_test_sel & DMEM0_MASK){
		pData = (unsigned int *)CACHED_TARGET_DMEM0_ADDR;
		/* varify data in DMEM0 */
		for(i=0;i<(DMEM0_SIZE/4); i++)
		{
			if(*pData!= INIT_WORD_VALUE)
			{
				printf("DMEM0 init error: (0x%08x) value 0x%08x != 0x%08x\n", \
					pData, *pData, INIT_WORD_VALUE);
			}
			pData++;
		}
	}

	if(idmem_test_sel & DMEM1_MASK){
		pData = (unsigned int *)CACHED_TARGET_DMEM1_ADDR;
		/* varify data in DMEM1 */
		for(i=0;i<(DMEM1_SIZE/4); i++)
		{
			if(*pData!= INIT_WORD_VALUE)
			{
				printf("DMEM1 init error: (0x%08x) value 0x%08x != 0x%08x\n", \
					pData, *pData, INIT_WORD_VALUE);
			}
			pData++;
		}
	}
	if(idmem_test_sel & L2MEM_MASK){
		pData = (unsigned int *)CACHED_TARGET_L2MEM_ADDR;
		/* varify data in L2MEM */
		for(i=0;i<(L2MEM_SIZE/4); i++)
		{
			if(*pData!= INIT_WORD_VALUE)
			{
				printf("L2MEM init error: (0x%08x) value 0x%08x != 0x%08x\n", \
					pData, *pData, INIT_WORD_VALUE);
			}
			pData++;
		}
	}
#endif

	if(idmem_test_sel & IMEM0_MASK)
		disable_IMEM0();
	if(idmem_test_sel & IMEM1_MASK)
		disable_IMEM1();
	if(idmem_test_sel & DMEM0_MASK)
		disable_DMEM0();
	if(idmem_test_sel & DMEM1_MASK)
		disable_DMEM1();
	if(idmem_test_sel & L2MEM_MASK)
		disable_L2MEM();
}

#define RLX5181_PID	(0xcf01)
#define RLX4181_PID	(0xcd01)
#define RLX5281_PID	(0xdc02)
#define RLX4281_PID	(0xdc01)
#define RLXCPU_PID_MASK (0xFFFF)

int is_rlxX281(void)
{
	unsigned int pid;

	pid = _read_cp0_pid();
	pid = pid & RLXCPU_PID_MASK;

	if( (pid == RLX5281_PID) || (pid == RLX4281_PID))
		return 1;
	else
		return 0;

}


char *targetName[] = {"IMEM0", "IMEM1", "DMEM0", "DMEM1", "L2MEM"};
unsigned int targetSize[] = { IMEM0_SIZE,  IMEM1_SIZE,  DMEM0_SIZE,  DMEM1_SIZE, L2MEM_SIZE };
int iDMemTestCase1(void)
{
	int j;
	int retcode;
	unsigned int i;
	unsigned int test_size;
	unsigned int target;
	unsigned int test_mode;
	volatile unsigned int k;
	volatile unsigned int *pMem;
	volatile unsigned int *uc_src;
	volatile unsigned int *uc_dist;
	unsigned int tmp;
	unsigned int imem_read_buff[2];


	/* Initialize the environment */

	retcode = SUCCESS;
	uc_src = (unsigned int *)TO_UNCACHED_ADDR(&src) ;
	uc_dist = (unsigned int *)TO_UNCACHED_ADDR(&dist) ;
	
	MEMCTL_DEBUG_PRINTF("uc_src = 0x%x, uc_dist = 0x%x\n", uc_src, uc_dist);

	for(test_mode=0; test_mode<=MODE_MAXNUM; test_mode++)
	{
		//init_Env(0);
		if(test_mode==0)
			MEMCTL_DEBUG_PRINTF("mode: store and forward \n");
		else
			MEMCTL_DEBUG_PRINTF("mode: Acceleration\n");

		for(target=SEL_IMEM0; target<=SEL_TARGET_MAX; target++)
		{
	
			if((idmem_test_sel & (((unsigned int) 0x1)<<target)) == 0)
				continue;
	
			init_Env(0);
			/* init src and dist */
			write_back_invalidate_dcache();
			for(j=0; j<(sizeof(src)/4); j++)
			{
				uc_dist[j] = INIT_WORD_VALUE;
				uc_src[j] = INIT_WORD_VALUE;
			}
			MEMCTL_DEBUG_PRINTF("\n target: %s, size: 0x%08x\n", targetName[target], targetSize[target]);	
		for(i=0; i<(sizeof(data_patterns)/4); i++)
		{
			
			//printf("\t\n Pattern[%d]: 0x%08x\n",i, data_patterns[i]);	
			MEMCTL_DEBUG_PRINTF("\t\n Pattern: 0x%08x\n", data_patterns[i]);	

			for(test_size=10; test_size<(targetSize[target]/4); test_size++)
			//for(test_size=(targetSize[target]/4)-1; test_size<(targetSize[target]/4); test_size++)
			{
				MEMCTL_DEBUG_PRINTF("\r\ttest size: %04d", test_size);
				write_back_invalidate_dcache();
				/* Set source data */
				for(j=0; j<test_size; j++)
				{
					uc_src[j] = data_patterns[i];
				}
#ifdef READ_FLUSH_OCP_WRITE /* flush OCP pending write */
				tmp = uc_src[j-1];
#else
				for(k=0;k<10000;k++);
				*((volatile unsigned int *)(0xB8001030)) = \
					*((volatile unsigned int *)(0xB8001030)) | 0x80000000;

				while(*((volatile unsigned int *)(0xB8001030)) != 0);
#endif

				/* DMA src into MEM */
				write_back_invalidate_dcache();
				imemDmemDMA((unsigned int)uc_src, CACHED_TARGET_DMEM0_ADDR, CACHED_TARGET_DMEM0_ADDR, \
						test_size*4, DIR_SDRAM_TO_IMEMDMEM, target, test_mode);

				/* DMA Data out form MEM into distination */
				imemDmemDMA((unsigned int)uc_dist, CACHED_TARGET_DMEM0_ADDR, CACHED_TARGET_DMEM0_ADDR, \
						test_size*4, DIR_IMEMDMEM_TO_SDRAM, target, test_mode);
#ifdef CHECK_SOURCE
				/* check src data first */
				for(j=0; j<test_size; j++)
				{
					if(uc_src[j] != data_patterns[i])
					{
						printf("\n\t%s %d:src[%d](0x%x) != pattern (0x%x)\n", \
							__FUNCTION__, __LINE__, j, uc_src[j], data_patterns[i]);
						retcode = FAILED;
						goto test_fail;
					}
				}
#endif
				/* Compare src and dist */
				for(j=0; j<(sizeof(src)/4); j++)
				{
						if((uc_src[j] != uc_dist[j]))
						{
							printf("\n\t%s %d:src[%d](0x%x) != dist[%d](0x%x)\n", \
								__FUNCTION__, __LINE__, j, uc_src[j], j, uc_dist[j]);
							retcode = FAILED;
							goto test_fail;
						}
				}
				
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
						if(uc_src[j] != *pMem)
						{
							printf("\n\t%s %d:src[%d](0x%x) != MEM 0x%x (0x%x)\n", \
							__FUNCTION__, __LINE__, j, uc_src[j], pMem, *pMem);
							retcode = FAILED;
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
					if(is_rlxX281()){
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
						for(j=0; j<test_size; j = j + 2)
						{
							if((test_size - j) < 2) /*only compare low 4 bytes*/
							{
								read_imem_8bytes(pMem , &imem_read_buff[0]);
								if(uc_src[j] != imem_read_buff[0])
								{
									printf(\
									"\n\t%s %d:src[%d](0x%x) != MEM 0x%x (0x%x)\n"\
									, __FUNCTION__, __LINE__, j, uc_src[j], pMem, \
									imem_read_buff[0]);
									retcode = FAILED;
									goto test_fail;
								}
							}
							else
							{
								read_imem_8bytes(pMem , &imem_read_buff[0]);
								if(uc_src[j] != imem_read_buff[0])
								{
									printf(\
									"\n\t%s %d:src[%d](0x%x) != MEM 0x%x (0x%x)\n"\
									, __FUNCTION__, __LINE__, j, uc_src[j], pMem, \
									imem_read_buff[0]);
									retcode = FAILED;
									goto test_fail;
								}
								if(uc_src[j+1] != imem_read_buff[1])
								{
									printf(\
									"\n\t%s %d:src[%d](0x%x) != MEM 0x%x (0x%x)\n"\
									, __FUNCTION__, __LINE__, j+1, uc_src[j+1], \
									pMem+1, imem_read_buff[1]);
									retcode = FAILED;
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
				}
			}
		}
		}
		MEMCTL_DEBUG_PRINTF("\n");
	}
	return retcode;

test_fail:
	if(idmem_test_sel & IMEM0_MASK)
		disable_IMEM0();
	if(idmem_test_sel & IMEM1_MASK)
		disable_IMEM1();
	if(idmem_test_sel & DMEM0_MASK)
		disable_DMEM0();
	if(idmem_test_sel & DMEM1_MASK)
		disable_DMEM1();
	if(idmem_test_sel & L2MEM_MASK)
		disable_L2MEM();

	return retcode;
}

/* address rotation */
int iDMemTestCase2(void)
{

	int j;
	int retcode;
	unsigned int i;
	unsigned int test_size;
	unsigned int target;
	unsigned int test_mode;
	unsigned int k;
	volatile unsigned int *pMem;
	volatile unsigned int *uc_src;
	volatile unsigned int *uc_dist;
	unsigned int tmp;
	unsigned int imem_read_buff[2];


	/* Initialize the environment */

	retcode = SUCCESS;
	uc_src = (unsigned int *)TO_UNCACHED_ADDR(&src) ;
	uc_dist = (unsigned int *)TO_UNCACHED_ADDR(&dist) ;
	
	//MEMCTL_DEBUG_PRINTF("uc_src = 0x%x, uc_dist = 0x%x\n", uc_src, uc_dist);

	for(test_mode=0; test_mode<=MODE_MAXNUM; test_mode++)
	{
		if(test_mode==0)
			MEMCTL_DEBUG_PRINTF("mode: store and forward \n");
		else
			MEMCTL_DEBUG_PRINTF("mode: Acceleration\n");
		for(target=SEL_IMEM0; target<=SEL_TARGET_MAX; target++)
		{

			if((idmem_test_sel & (((unsigned int) 0x1)<<target)) == 0)
				continue;

			init_Env(0);
			/* init src and dist */
			write_back_invalidate_dcache();
			for(j=0; j<(sizeof(src)/4); j++)
			{
				uc_dist[j] = INIT_WORD_VALUE;
				uc_src[j] = INIT_WORD_VALUE;
			}
			MEMCTL_DEBUG_PRINTF("\n target: %s, size: 0x%08x\n", targetName[target], targetSize[target]);
			for(i=0; i<32; i=i+4)
			{
				MEMCTL_DEBUG_PRINTF("\r Pattern: address rotate %d", i);	
				for(test_size=10; test_size<=(targetSize[target]/4); test_size=test_size*2)
				//for(test_size=(targetSize[target]/4)-1; test_size<=(targetSize[target]/4); test_size=test_size*2)
				{
					//write_back_invalidate_dcache();
					/* Set source data */
					for(j=0; j<test_size; j++)
					{
						uc_src[j] = ((unsigned int)(&uc_src[j])) << i;
					}
#ifdef READ_FLUSH_OCP_WRITE /* flush OCP pending write */
					tmp = uc_src[j-1];
#else
					*((volatile unsigned int *)(0xB8001030)) = \
						*((volatile unsigned int *)(0xB8001030)) | 0x80000000;

					while(*((volatile unsigned int *)(0xB8001030)) != 0);
#endif
	
					/* DMA src into DMEM */
					//write_back_invalidate_dcache();
					imemDmemDMA((unsigned int)uc_src, CACHED_TARGET_DMEM0_ADDR, CACHED_TARGET_DMEM0_ADDR, \
						test_size*4, DIR_SDRAM_TO_IMEMDMEM, target, test_mode);

					/* DMA Data out form DMEME into distination */
					imemDmemDMA((unsigned int)uc_dist, CACHED_TARGET_DMEM0_ADDR, CACHED_TARGET_DMEM0_ADDR, \
						test_size*4, DIR_IMEMDMEM_TO_SDRAM, target, test_mode);

					/* Compare src data first */
					for(j=0; j<test_size; j++)
					{
						if(uc_src[j] != (((unsigned int)(&uc_src[j])) << i))
						{
							printf("\n\t%s %d:src[%d](0x%x) != (0x%x)\n", \
								__FUNCTION__, __LINE__, j, uc_src[j], \
								((unsigned int)(&uc_src[j])) << i);
							retcode = FAILED;
							goto test_fail;
						}
					}

					/* Compare src and dist */
					for(j=0; j<(sizeof(src)/4); j++)
					{
						if(uc_src[j] != uc_dist[j])
						{
							printf("\n\t%s %d:src[%d](0x%x) != dist[%d](0x%x)\n", \
								__FUNCTION__, __LINE__, j, uc_src[j], j, uc_dist[j]);
							retcode = FAILED;
							goto test_fail;
						}
					}
				
					if((target==SEL_DMEM0) || (target==SEL_DMEM1) || (target == SEL_L2MEM))
					{
						/* Enable  DMEM */
						write_back_dcache();
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

						for(j=0; j<(targetSize[target]/4); j++)
						{
							if(uc_src[j] != *pMem)
							{
								printf("\n\t%s %d:0x%x src[%d](0x%x) != MEM 0x%x (0x%x)\n", \
								__FUNCTION__, __LINE__, &uc_src[j], j, uc_src[j], pMem, *pMem);
								retcode = FAILED;
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
						if(is_rlxX281()){
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
							for(j=0; j<test_size; j = j + 2)
							{
								if((test_size - j) < 2) /*only compare low 4 bytes*/
								{
									read_imem_8bytes(pMem , &imem_read_buff[0]);
									if(uc_src[j] != imem_read_buff[0])
									{
										printf(\
										"\n\t%s %d:src[%d](0x%x) != MEM 0x%x (0x%x)\n"\
										, __FUNCTION__, __LINE__, j, uc_src[j], pMem, \
										imem_read_buff[0]);
										retcode = FAILED;
										goto test_fail;
									}
								}
								else
								{
									read_imem_8bytes(pMem , &imem_read_buff[0]);
									if(uc_src[j] != imem_read_buff[0])
									{
										printf(\
										"\n\t%s %d:src[%d](0x%x) != MEM 0x%x (0x%x)\n"\
										, __FUNCTION__, __LINE__, j, uc_src[j], pMem, \
										imem_read_buff[0]);
										retcode = FAILED;
										goto test_fail;
									}
									if(uc_src[j+1] != imem_read_buff[1])
									{
										printf(\
										"\n\t%s %d:src[%d](0x%x) != MEM 0x%x (0x%x)\n"\
										, __FUNCTION__, __LINE__, j+1, uc_src[j+1], \
										pMem+1, imem_read_buff[1]);
										retcode = FAILED;
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
	
					}
				}
			}
		}
		MEMCTL_DEBUG_PRINTF("\n");
	}
	return retcode;

test_fail:
	if(idmem_test_sel & IMEM0_MASK)
		disable_IMEM0();
	if(idmem_test_sel & IMEM1_MASK)
		disable_IMEM1();
	if(idmem_test_sel & DMEM0_MASK)
		disable_DMEM0();
	if(idmem_test_sel & DMEM1_MASK)
		disable_DMEM1();
	if(idmem_test_sel & L2MEM_MASK)
		disable_L2MEM();
	return retcode;
}

/* ~ address rotattion */
int iDMemTestCase3(void)
{
	int j;
	int retcode;
	unsigned int i;
	unsigned int test_size;
	unsigned int target;
	unsigned int test_mode;
	unsigned int k;
	volatile unsigned int *pMem;
	volatile unsigned int *uc_src;
	volatile unsigned int *uc_dist;
	unsigned int tmp;
	unsigned int imem_read_buff[2];


	/* Initialize the environment */

	retcode = SUCCESS;
	uc_src = (unsigned int *)TO_UNCACHED_ADDR(&src) ;
	uc_dist = (unsigned int *)TO_UNCACHED_ADDR(&dist) ;
	
	//MEMCTL_DEBUG_PRINTF("uc_src = 0x%x, uc_dist = 0x%x\n", uc_src, uc_dist);

	for(test_mode=0; test_mode<=MODE_MAXNUM; test_mode++)
	{
		if(test_mode==0)
			MEMCTL_DEBUG_PRINTF("mode: store and forward \n");
		else
			MEMCTL_DEBUG_PRINTF("mode: Acceleration\n");
		for(target=SEL_IMEM0; target<=SEL_TARGET_MAX; target++)
		{

			if((idmem_test_sel & (((unsigned int) 0x1)<<target)) == 0)
				continue;

			init_Env(0);
			/* init src and dist */
			write_back_invalidate_dcache();
			for(j=0; j<(sizeof(src)/4); j++)
			{
				uc_dist[j] = INIT_WORD_VALUE;
				uc_src[j] = INIT_WORD_VALUE;
			}
			MEMCTL_DEBUG_PRINTF("\n target: %s, size: 0x%08x\n", targetName[target], targetSize[target]);
		for(i=0; i<32; i=i+4)
		{
			
			MEMCTL_DEBUG_PRINTF("\r Pattern: ~ address rotate %d", i);	
			for(test_size=10; test_size<=(targetSize[target]/4); test_size=test_size*2)
			//for(test_size=(targetSize[target]/4)-1; test_size<=(targetSize[target]/4); test_size=test_size*2)
			{
				write_back_invalidate_dcache();
				/* Set source data */
				for(j=0; j<test_size; j++)
				{
					uc_src[j] = ~((unsigned int)(&uc_src[j]) << i);
				}
#ifdef READ_FLUSH_OCP_WRITE /* flush OCP pending write */
				tmp = uc_src[j-1];
#else
				*((volatile unsigned int *)(0xB8001030)) = \
					*((volatile unsigned int *)(0xB8001030)) | 0x80000000;

				while(*((volatile unsigned int *)(0xB8001030)) != 0);
#endif

				/* DMA src into DMEM */
				write_back_invalidate_dcache();
				imemDmemDMA((unsigned int)uc_src, CACHED_TARGET_DMEM0_ADDR, CACHED_TARGET_DMEM0_ADDR, \
						test_size*4, DIR_SDRAM_TO_IMEMDMEM, target, test_mode);

				/* DMA Data out form DMEME into distination */
				imemDmemDMA((unsigned int)uc_dist, CACHED_TARGET_DMEM0_ADDR, CACHED_TARGET_DMEM0_ADDR, \
						test_size*4, DIR_IMEMDMEM_TO_SDRAM, target, test_mode);

				/* Compare src data first */
				for(j=0; j<test_size; j++)
				{
					if(uc_src[j] != ~(((unsigned int)(&uc_src[j])) << i))
					{
						printf("\n\t%s %d:src[%d](0x%x) != (0x%x)\n", \
							__FUNCTION__, __LINE__, j, uc_src[j], \
							~((unsigned int)(&uc_src[j])) << i);
						retcode = FAILED;
						goto test_fail;
					}
				}
				/* Compare src and dist */
				for(j=0; j<(sizeof(src)/4); j++)
				{
					if(uc_src[j] != uc_dist[j])
					{
						printf("\n\t%s %d:src[%d](0x%x) != dist[%d](0x%x)\n", \
							__FUNCTION__, __LINE__, j, uc_src[j], j, uc_dist[j]);
						retcode = FAILED;
						goto test_fail;
					}
				}
				
				if((target==SEL_DMEM0) || (target==SEL_DMEM1) || (target == SEL_L2MEM))
				{
					/* Enable  DMEM */
					write_back_dcache();
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

					for(j=0; j<(targetSize[target]/4); j++)
					{
						if(uc_src[j] != *pMem)
						{
							printf("\n\t%s %d:src[%d](0x%x) != MEM 0x%x (0x%x)\n", \
							__FUNCTION__, __LINE__, j, uc_src[j], pMem, *pMem);
							retcode = FAILED;
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
					if(is_rlxX281()){
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
						for(j=0; j<test_size; j = j + 2)
						{
							if((test_size - j) < 2) /*only compare low 4 bytes*/
							{
								read_imem_8bytes(pMem , &imem_read_buff[0]);
								if(uc_src[j] != imem_read_buff[0])
								{
									printf(\
									"\n\t%s %d:src[%d](0x%x) != MEM 0x%x (0x%x)\n"\
									, __FUNCTION__, __LINE__, j, uc_src[j], pMem, \
									imem_read_buff[0]);
									retcode = FAILED;
									goto test_fail;
								}
							}
							else
							{
								read_imem_8bytes(pMem , &imem_read_buff[0]);
								if(uc_src[j] != imem_read_buff[0])
								{
									printf(\
									"\n\t%s %d:src[%d](0x%x) != MEM 0x%x (0x%x)\n"\
									, __FUNCTION__, __LINE__, j, uc_src[j], pMem, \
									imem_read_buff[0]);
									retcode = FAILED;
									goto test_fail;
								}
								if(uc_src[j+1] != imem_read_buff[1])
								{
									printf(\
									"\n\t%s %d:src[%d](0x%x) != MEM 0x%x (0x%x)\n"\
									, __FUNCTION__, __LINE__, j+1, uc_src[j+1], \
									pMem+1, imem_read_buff[1]);
									retcode = FAILED;
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
				}
			}
		}
		}
		MEMCTL_DEBUG_PRINTF("\n");
	}
	return retcode;

test_fail:
	if(idmem_test_sel & IMEM0_MASK)
		disable_IMEM0();
	if(idmem_test_sel & IMEM1_MASK)
		disable_IMEM1();
	if(idmem_test_sel & DMEM0_MASK)
		disable_DMEM0();
	if(idmem_test_sel & DMEM1_MASK)
		disable_DMEM1();
	if(idmem_test_sel & L2MEM_MASK)
		disable_L2MEM();

	return retcode;
}

/* walking of 1 */
int iDMemTestCase4(void)
{
	int j;
	int retcode;
	unsigned int i;
	unsigned int test_size;
	unsigned int target;
	unsigned int test_mode;
	unsigned int k;
	volatile unsigned int *pMem;
	volatile unsigned int *uc_src;
	volatile unsigned int *uc_dist;
	unsigned int tmp;
	unsigned int imem_read_buff[2];


	/* Initialize the environment */

	retcode = SUCCESS;
	uc_src = (unsigned int *)TO_UNCACHED_ADDR(&src) ;
	uc_dist = (unsigned int *)TO_UNCACHED_ADDR(&dist) ;
	
	//MEMCTL_DEBUG_PRINTF("uc_src = 0x%x, uc_dist = 0x%x\n", uc_src, uc_dist);

	for(test_mode=0; test_mode<=MODE_MAXNUM; test_mode++)
	{
		if(test_mode==0)
			MEMCTL_DEBUG_PRINTF("mode: store and forward \n");
		else
			MEMCTL_DEBUG_PRINTF("mode: Acceleration\n");
		for(target=SEL_IMEM0; target<=SEL_TARGET_MAX; target++)
		{

			if((idmem_test_sel & (((unsigned int) 0x1)<<target)) == 0)
				continue;

			init_Env(0);
			/* init src and dist */
			write_back_invalidate_dcache();
			for(j=0; j<(sizeof(src)/4); j++)
			{
				uc_dist[j] = INIT_WORD_VALUE;
				uc_src[j] = INIT_WORD_VALUE;
			}
			MEMCTL_DEBUG_PRINTF("\n target: %s, size: 0x%08x\n", targetName[target], targetSize[target]);
		for(i=0; i<32; i++)
		{
			MEMCTL_DEBUG_PRINTF("\r Pattern: walking of 1: %08x", 1 << i);	
			for(test_size=10; test_size<=(targetSize[target]/4); test_size=test_size*2)
			//for(test_size=(targetSize[target]/4)-1; test_size<=(targetSize[target]/4); test_size=test_size*2)
			{
				write_back_invalidate_dcache();
				/* Set source data */
				for(j=0; j<test_size; j++)
				{
					uc_src[j] = (1 << i);
				}
#ifdef READ_FLUSH_OCP_WRITE /* flush OCP pending write */
				tmp = uc_src[j-1];
#else
				*((volatile unsigned int *)(0xB8001030)) = \
					*((volatile unsigned int *)(0xB8001030)) | 0x80000000;

				while(*((volatile unsigned int *)(0xB8001030)) != 0);
#endif

				/* DMA src into DMEM */
				write_back_invalidate_dcache();
				imemDmemDMA((unsigned int)uc_src, CACHED_TARGET_DMEM0_ADDR, CACHED_TARGET_DMEM0_ADDR, \
						test_size*4, DIR_SDRAM_TO_IMEMDMEM, target, test_mode);

				/* DMA Data out form DMEME into distination */
				imemDmemDMA((unsigned int)uc_dist, CACHED_TARGET_DMEM0_ADDR, CACHED_TARGET_DMEM0_ADDR, \
						test_size*4, DIR_IMEMDMEM_TO_SDRAM, target, test_mode);

				/* Compare src data first */
				for(j=0; j<test_size; j++)
				{
					if(uc_src[j] != (1 << i))
					{
						printf("\n\t%s %d:src[%d](0x%x) != (0x%x)\n", \
							__FUNCTION__, __LINE__, j, uc_src[j], \
							(1 << i));
						retcode = FAILED;
						goto test_fail;
					}
				}

				/* Compare src and dist */
				for(j=0; j<(sizeof(src)/4); j++)
				{
					if(uc_src[j] != uc_dist[j])
					{
						printf("\n\t%s %d:src[%d](0x%x) != dist[%d](0x%x)\n", \
							__FUNCTION__, __LINE__, j, uc_src[j], j, uc_dist[j]);
						retcode = FAILED;
						goto test_fail;
					}
				}
				
				if((target==SEL_DMEM0) || (target==SEL_DMEM1) || (target == SEL_L2MEM))
				{
					/* Enable  DMEM */
					write_back_dcache();
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

					for(j=0; j<(targetSize[target]/4); j++)
					{
						if(uc_src[j] != *pMem)
						{
							printf("\n\t%s %d:src[%d](0x%x) != MEM 0x%x (0x%x)\n", \
							__FUNCTION__, __LINE__, j, uc_src[j], pMem, *pMem);
							retcode = FAILED;
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
				if(is_rlxX281()){
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
					for(j=0; j<test_size; j = j + 2)
					{
						if((test_size - j) < 2) /*only compare low 4 bytes*/
						{
							read_imem_8bytes(pMem , &imem_read_buff[0]);
							if(uc_src[j] != imem_read_buff[0])
							{
								printf(\
								"\n\t%s %d:src[%d](0x%x) != MEM 0x%x (0x%x)\n"\
								, __FUNCTION__, __LINE__, j, uc_src[j], pMem, \
								imem_read_buff[0]);
								retcode = FAILED;
								goto test_fail;
							}
						}
						else
						{
							read_imem_8bytes(pMem , &imem_read_buff[0]);
							if(uc_src[j] != imem_read_buff[0])
							{
								printf(\
								"\n\t%s %d:src[%d](0x%x) != MEM 0x%x (0x%x)\n"\
								, __FUNCTION__, __LINE__, j, uc_src[j], pMem, \
								imem_read_buff[0]);
								retcode = FAILED;
								goto test_fail;
							}
							if(uc_src[j+1] != imem_read_buff[1])
							{
								printf(\
								"\n\t%s %d:src[%d](0x%x) != MEM 0x%x (0x%x)\n"\
								, __FUNCTION__, __LINE__, j+1, uc_src[j+1], \
								pMem+1, imem_read_buff[1]);
								retcode = FAILED;
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

				}
			}
		}
		}
		MEMCTL_DEBUG_PRINTF("\n");
	}
	return retcode;

test_fail:
	if(idmem_test_sel & IMEM0_MASK)
		disable_IMEM0();
	if(idmem_test_sel & IMEM1_MASK)
		disable_IMEM1();
	if(idmem_test_sel & DMEM0_MASK)
		disable_DMEM0();
	if(idmem_test_sel & DMEM1_MASK)
		disable_DMEM1();
	if(idmem_test_sel & L2MEM_MASK)
		disable_L2MEM();

	return retcode;
}

/* walking of 0 */
int iDMemTestCase5(void)
{
	int j;
	int retcode;
	unsigned int i;
	unsigned int test_size;
	unsigned int target;
	unsigned int test_mode;
	unsigned int k;
	volatile unsigned int *pMem;
	volatile unsigned int *uc_src;
	volatile unsigned int *uc_dist;
	unsigned int tmp;
	unsigned int imem_read_buff[2];


	/* Initialize the environment */

	retcode = SUCCESS;
	uc_src = (unsigned int *)TO_UNCACHED_ADDR(&src) ;
	uc_dist = (unsigned int *)TO_UNCACHED_ADDR(&dist) ;
	
	//MEMCTL_DEBUG_PRINTF("uc_src = 0x%x, uc_dist = 0x%x\n", uc_src, uc_dist);

	for(test_mode=0; test_mode<=MODE_MAXNUM; test_mode++)
	{
		if(test_mode==0)
			MEMCTL_DEBUG_PRINTF("\nmode: store and forward \n");
		else
			MEMCTL_DEBUG_PRINTF("\nmode: Acceleration\n");
		for(target=SEL_IMEM0; target<=SEL_TARGET_MAX; target++)
		{

			if((idmem_test_sel & (((unsigned int) 0x1)<<target)) == 0)
				continue;

			init_Env(0);
			/* init src and dist */
			write_back_invalidate_dcache();
			for(j=0; j<(sizeof(src)/4); j++)
			{
				uc_dist[j] = INIT_WORD_VALUE;
				uc_src[j] = INIT_WORD_VALUE;
			}
			MEMCTL_DEBUG_PRINTF("\n target: %s, size: 0x%08x\n", targetName[target], targetSize[target]);
		for(i=0; i<32; i++)
		{
			MEMCTL_DEBUG_PRINTF("\r Pattern: walking of 0: %08x", ~(1 << i));	
			for(test_size=10; test_size<=(targetSize[target]/4); test_size=test_size*2)
			//for(test_size=(targetSize[target]/4)-1; test_size<=(targetSize[target]/4); test_size=test_size*2)
			{
				write_back_invalidate_dcache();
				/* Set source data */
				for(j=0; j<test_size; j++)
				{
					uc_src[j] = ~(1 << i);
				}
#ifdef READ_FLUSH_OCP_WRITE /* flush OCP pending write */
				tmp = uc_src[j-1];
#else
				*((volatile unsigned int *)(0xB8001030)) = \
					*((volatile unsigned int *)(0xB8001030)) | 0x80000000;

				while(*((volatile unsigned int *)(0xB8001030)) != 0);
#endif

				/* DMA src into DMEM */
				write_back_invalidate_dcache();
				imemDmemDMA((unsigned int)uc_src, CACHED_TARGET_DMEM0_ADDR, CACHED_TARGET_DMEM0_ADDR, \
						test_size*4, DIR_SDRAM_TO_IMEMDMEM, target, test_mode);

				/* DMA Data out form DMEME into distination */
				imemDmemDMA((unsigned int)uc_dist, CACHED_TARGET_DMEM0_ADDR, CACHED_TARGET_DMEM0_ADDR, \
						test_size*4, DIR_IMEMDMEM_TO_SDRAM, target, test_mode);

				/* Compare src data first */
				for(j=0; j<test_size; j++)
				{
					if(uc_src[j] != ~(1 << i))
					{
						printf("\n\t%s %d:src[%d](0x%x) != (0x%x)\n", \
							__FUNCTION__, __LINE__, j, uc_src[j], \
							~(1 << i));
						retcode = FAILED;
						goto test_fail;
					}
				}

				/* Compare src and dist */
				for(j=0; j<(sizeof(src)/4); j++)
				{
					if(uc_src[j] != uc_dist[j])
					{
						printf("\n\t%s %d:src[%d](0x%x) != dist[%d](0x%x), 0x%08x \n", \
							__FUNCTION__, __LINE__, j, uc_src[j], j, uc_dist[j],&uc_dist[j]);
						retcode = FAILED;
						goto test_fail;
					}
				}
				
				if((target==SEL_DMEM0) || (target==SEL_DMEM1) || (target == SEL_L2MEM))
				{
					/* Enable  DMEM */
					write_back_dcache();
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

					for(j=0; j<(targetSize[target]/4); j++)
					{
						if(uc_src[j] != *pMem)
						{
							printf("\n\t%s %d:src[%d](0x%x) != MEM 0x%x (0x%x)\n", \
							__FUNCTION__, __LINE__, j, uc_src[j], pMem, *pMem);
							retcode = FAILED;
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
				if(is_rlxX281()){
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
					for(j=0; j<test_size; j = j + 2)
					{
						if((test_size - j) < 2) /*only compare low 4 bytes*/
						{
							read_imem_8bytes(pMem , &imem_read_buff[0]);
							if(uc_src[j] != imem_read_buff[0])
							{
								printf(\
								"\n\t%s %d:src[%d](0x%x) != MEM 0x%x (0x%x)\n"\
								, __FUNCTION__, __LINE__, j, uc_src[j], pMem, \
								imem_read_buff[0]);
								retcode = FAILED;
								goto test_fail;
							}
						}
						else
						{
							read_imem_8bytes(pMem , &imem_read_buff[0]);
							if(uc_src[j] != imem_read_buff[0])
							{
								printf(\
								"\n\t%s %d:src[%d](0x%x) != MEM 0x%x (0x%x)\n"\
								, __FUNCTION__, __LINE__, j, uc_src[j], pMem, \
								imem_read_buff[0]);
								retcode = FAILED;
								goto test_fail;
							}
							if(uc_src[j+1] != imem_read_buff[1])
							{
								printf(\
								"\n\t%s %d:src[%d](0x%x) != MEM 0x%x (0x%x)\n"\
								, __FUNCTION__, __LINE__, j+1, uc_src[j+1], \
								pMem+1, imem_read_buff[1]);
								retcode = FAILED;
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
				}
			}
		}
		}
		MEMCTL_DEBUG_PRINTF("\n");
	}
	return retcode;

test_fail:
	if(idmem_test_sel & IMEM0_MASK)
		disable_IMEM0();
	if(idmem_test_sel & IMEM1_MASK)
		disable_IMEM1();
	if(idmem_test_sel & DMEM0_MASK)
		disable_DMEM0();
	if(idmem_test_sel & DMEM1_MASK)
		disable_DMEM1();
	if(idmem_test_sel & L2MEM_MASK)
		disable_L2MEM();

	return retcode;
}

static unsigned int x = 123456789;
unsigned int rand2(void)
{
        static unsigned int y = 362436;
        static unsigned int z = 521288629;
        static unsigned int c = 7654321;
        unsigned long long t, a= 698769069;

        x = 69069 * x + 12345;
        y ^= (y << 13); y ^= (y >> 17); y ^= (y << 5);
        t = a * z + c; c = (t >> 32); z = t;

        return x + y + z;
}


/* Random data */
int iDMemTestCase6(void)
{
	int j;
	int retcode;
	unsigned int i;
	unsigned int test_size;
	unsigned int target;
	unsigned int test_mode;
	unsigned int k;
	volatile unsigned int *pMem;
	volatile unsigned int *uc_src;
	volatile unsigned int *uc_dist;
	volatile unsigned int tmp;
	unsigned int show;
	unsigned int imem_read_buff[2];
	int run_times;

	show = 0;
	/* Initialize the environment */

	retcode = SUCCESS;
	uc_src = (unsigned int *)TO_UNCACHED_ADDR(&src) ;
	uc_dist = (unsigned int *)TO_UNCACHED_ADDR(&dist) ;
	
	MEMCTL_DEBUG_PRINTF("uc_src = 0x%x, uc_dist = 0x%x\n", uc_src, uc_dist);

	for(test_mode=0; test_mode<=MODE_MAXNUM; test_mode++)
	{
		if(test_mode==0)
			MEMCTL_DEBUG_PRINTF("\nmode: store and forward \n");
		else
			MEMCTL_DEBUG_PRINTF("\nmode: Acceleration\n");

		for(target=SEL_IMEM0; target<=SEL_TARGET_MAX; target++)
		{

			if((idmem_test_sel & (((unsigned int) 0x1)<<target)) == 0)
				continue;

			run_times = 20;
			init_Env(0);
			//run_times = -1;
			/* init src and dist */
			write_back_invalidate_dcache();
			for(j=0; j<(sizeof(src)/4); j++)
			{
				uc_dist[j] = INIT_WORD_VALUE;
				uc_src[j] = INIT_WORD_VALUE;
			}
			if(run_times > 0)
			{
				MEMCTL_DEBUG_PRINTF("\n target: %s, size: 0x%08x\n", targetName[target], targetSize[target]);
			}
				MEMCTL_DEBUG_PRINTF("Pattern: Random data \n");	
		while(run_times != 0)
		{
			if(run_times > 0)
			{
				run_times--;
				MEMCTL_DEBUG_PRINTF("\rleft times: %08d", run_times);
			}

			else
			{
if(retcode != FAILED){
				target = (target+1)%5;
				test_mode = target%2;
}
				if(show==0)
				{
					MEMCTL_DEBUG_PRINTF("\r\\\\\\\\");
					show = (show+1)%4;
				}
				else if(show==1)
				{
					MEMCTL_DEBUG_PRINTF("\r||||");
					show = (show+1)%4;
				}
				else if(show==2)
				{
					MEMCTL_DEBUG_PRINTF("\r////");
					show = (show+1)%4;
				}
				else
				{
					MEMCTL_DEBUG_PRINTF("\r----");
					show = (show+1)%4;
				}
			}
			for(test_size=(targetSize[target]/4); test_size<=(targetSize[target]/4); test_size=test_size*2)
			{
				write_back_invalidate_dcache();
				/* Set source data */
if(retcode != FAILED)
{
				for(j=0; j<test_size; j++)
				{
					uc_src[j] = rand2();
				}
}

#ifdef READ_FLUSH_OCP_WRITE /* flush OCP pending write */
				tmp = uc_src[j-1];
#else
				*((volatile unsigned int *)(0xB8001030)) = \
					*((volatile unsigned int *)(0xB8001030)) | 0x80000000;

				while(*((volatile unsigned int *)(0xB8001030)) != 0);
#endif

				/* DMA src into DMEM */
				write_back_invalidate_dcache();
				imemDmemDMA((unsigned int)uc_src, CACHED_TARGET_DMEM0_ADDR, CACHED_TARGET_DMEM0_ADDR, \
						test_size*4, DIR_SDRAM_TO_IMEMDMEM, target, test_mode);

				/* DMA Data out form DMEME into distination */
				imemDmemDMA((unsigned int)uc_dist, CACHED_TARGET_DMEM0_ADDR, CACHED_TARGET_DMEM0_ADDR, \
						test_size*4, DIR_IMEMDMEM_TO_SDRAM, target, test_mode);

				/* Compare src and dist */
				for(j=0; j<(sizeof(src)/4); j++)
				{
					if(uc_src[j] != uc_dist[j])
					{
						printf("\n\t%s %d: %s mode(%d) src[%d](0x%x)" \
						"!= dist[%d](0x%x), 0x%08x \n", __FUNCTION__,\
						 __LINE__,targetName[target], test_mode, j, uc_src[j], j, \
						uc_dist[j],&uc_dist[j]);
						retcode = FAILED;
					//	goto test_fail;
					}
				}
				
				if((target==SEL_DMEM0) || (target==SEL_DMEM1) || (target == SEL_L2MEM))
				{
					/* Enable  DMEM */
					write_back_dcache();
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

					for(j=0; j<(targetSize[target]/4); j++)
					{
						if(uc_src[j] != *pMem)
						{
							printf("\n\t%s %d: %s mode(%d) src[%d](0x%x)" \
							"!= MEM 0x%x (0x%x)\n", __FUNCTION__, __LINE__,\
							 targetName[target],test_mode , j,uc_src[j], pMem, *pMem);
							retcode = FAILED;
							//goto test_fail;
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
				if(is_rlxX281()){
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
					for(j=0; j<test_size; j = j + 2)
					{
						if((test_size - j) < 2) /*only compare low 4 bytes*/
						{
							read_imem_8bytes(pMem , &imem_read_buff[0]);
							if(uc_src[j] != imem_read_buff[0])
							{
								printf(\
								"\n\t%s %d: %s mode(%d) src[%d](0x%x) "\
								"!= MEM 0x%x (0x%x)\n", __FUNCTION__, __LINE__,\
								 targetName[target], test_mode ,j,\
								 uc_src[j], pMem, imem_read_buff[0]);
								retcode = FAILED;
								//goto test_fail;
							}
						}
						else
						{
							read_imem_8bytes(pMem , &imem_read_buff[0]);
							if(uc_src[j] != imem_read_buff[0])
							{
								printf(\
								"\n\t%s %d: %s mode(%d) src[%d](0x%x)" \
								" != MEM 0x%x (0x%x)\n" , __FUNCTION__,\
								__LINE__, targetName[target], test_mode, j,\
								uc_src[j], pMem, imem_read_buff[0]);
								retcode = FAILED;
								//goto test_fail;
							}
							if(uc_src[j+1] != imem_read_buff[1])
							{
								printf(\
								"\n\t%s %d: %s mode(%d) src[%d](0x%x) "\
								"!= MEM 0x%x (0x%x)\n", __FUNCTION__,\
								 __LINE__, targetName[target], test_mode ,j+1, \
								uc_src[j+1], pMem+1, imem_read_buff[1]);
								retcode = FAILED;
								//goto test_fail;
							}
						}
						pMem = pMem + 2;
					}

					if(target==SEL_IMEM0)
						disable_IMEM0();
					else
						disable_IMEM1();
				}
				}
			}
		}
		}
		MEMCTL_DEBUG_PRINTF("\n");
	}
	return retcode;

test_fail:
	if(idmem_test_sel & IMEM0_MASK)
		disable_IMEM0();
	if(idmem_test_sel & IMEM1_MASK)
		disable_IMEM1();
	if(idmem_test_sel & DMEM0_MASK)
		disable_DMEM0();
	if(idmem_test_sel & DMEM1_MASK)
		disable_DMEM1();
	if(idmem_test_sel & L2MEM_MASK)
		disable_L2MEM();

	return retcode;


}

int iDMemTestCase7(void)
{
	unsigned int *ins_addr;
	unsigned int *uc_ins_addr;
	unsigned int *pMem;
	unsigned int choose;
	unsigned int imem_top;
	unsigned int imem_base;
	unsigned int i, tmp;
	unsigned int retcode;
	unsigned int show;
	
	int run_times; /* -1 for running forever */

	//run_times = -1;
	run_times = 0x100;
	show = 0;

	init_Env(0);

	retcode = SUCCESS;
	choose = 0; /* Start from trying IMEM0 */

	write_back_invalidate_dcache();
	ins_addr = (unsigned int*)iDMemTestCase7;
	ins_addr = (unsigned int*)((unsigned int)ins_addr & (0xFFFFC000));
	uc_ins_addr = (unsigned int*)TO_UNCACHED_ADDR(ins_addr);

	MEMCTL_DEBUG_PRINTF("ins_addr = 0x%08x, uc_ins_addr = 0x%08x\n", ins_addr, uc_ins_addr );

	if(run_times < 0)
		MEMCTL_DEBUG_PRINTF("run %s forever\n", __FUNCTION__);
	while(run_times != 0)
	{
		if(run_times>0)
		{
			MEMCTL_DEBUG_PRINTF("\rtimes = %d", run_times);
		}
		else
		{
			if(show==0)
			{
				MEMCTL_DEBUG_PRINTF("\r\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\");
				show = (show+1)%4;
			}
			else if(show==1)
			{
				MEMCTL_DEBUG_PRINTF("\r||||||||||||||||||||||||");
				show = (show+1)%4;
			}
			else if(show==2)
			{
				MEMCTL_DEBUG_PRINTF("\r////////////////////////");
				show = (show+1)%4;
			}
			else
			{
				MEMCTL_DEBUG_PRINTF("\r------------------------");
				show = (show+1)%4;
			}
		}
		/* Backup instructions and set IMEM top and base registers */
		if(choose == 0)
		{
			memcpy((void *)TARGET_BACKUP_ADDR, (void *)ins_addr , IMEM0_SIZE);
			imem_base = PHYSADDR((ins_addr));
			imem_top = PHYSADDR( imem_base + IMEM0_SIZE - 1 );
			//MEMCTL_DEBUG_PRINTF("load imem0 range: top (0x%08x), base(0x%08x)\n", imem_top, imem_base);
			load_imem0Range(imem_top, imem_base);
			IMEM0_refill();
			/* fill pattern into the mapped imem area in DRAM */
			for(i=0; i < IMEM0_SIZE/4; i++)
			{
				uc_ins_addr[i] = 0xFFFFFFFF;
			}
		}
		else
		{
			memcpy((void *)TARGET_BACKUP_ADDR, (void *)ins_addr , IMEM1_SIZE);
			imem_base = PHYSADDR( (ins_addr) );
			imem_top = PHYSADDR( imem_base + IMEM1_SIZE - 1 );
			//MEMCTL_DEBUG_PRINTF("load imem1 range: top (0x%08x), base(0x%08x)\n", imem_top, imem_base);
			load_imem1Range(imem_top, imem_base);
			IMEM1_refill();
			/* fill pattern into the mapped imem area in DRAM */
			for(i=0; i < IMEM1_SIZE/4; i++)
			{
				uc_ins_addr[i] = 0xFFFFFFFF;
			}
		}


#ifdef READ_FLUSH_OCP_WRITE /* flush OCP pending write */
		tmp = uc_ins_addr[i-1];
#else
		*((volatile unsigned int *)(0xB8001030)) = \
			*((volatile unsigned int *)(0xB8001030)) | 0x80000000;

		while(*((volatile unsigned int *)(0xB8001030)) != 0);
#endif

		invalidate_icache();
		write_back_invalidate_dcache();


		/* DMA src into DMEM */
		if(choose==0)
		{
			load_dmem0Range(DMEM0_TOP_ADDR, DMEM0_BASE_ADDR);
			imemDmemDMA((unsigned int)uc_ins_addr, (unsigned int)CACHED_TARGET_DMEM0_ADDR, \
					(unsigned int)CACHED_TARGET_DMEM0_ADDR, DMEM0_SIZE,\ 
					DIR_SDRAM_TO_IMEMDMEM, SEL_DMEM0, STORE_FORWARD_MODE);
			pMem = (unsigned int*)CACHED_TARGET_DMEM0_ADDR;
			enable_DMEM0();
			/* compare data in DMEM and DRAM */
               		for(i = 0; i < DMEM0_SIZE/4; i++)
               		{
                		if(uc_ins_addr[i] != *pMem)
                		{
                			printf("\n\t%s %d:src[%d](0x%x) != MEM 0x%x (0x%x)\n", \
                				__FUNCTION__, __LINE__, i, uc_ins_addr[i], pMem, *pMem);
                			retcode = FAILED;
                			goto test_fail;
                		}
                		pMem++;
               		}
		}
		else
		{
			load_dmem1Range(DMEM1_TOP_ADDR, DMEM1_BASE_ADDR);
			imemDmemDMA((unsigned int)uc_ins_addr, (unsigned int)CACHED_TARGET_DMEM1_ADDR, \
					(unsigned int)CACHED_TARGET_DMEM1_ADDR, DMEM1_SIZE, \
					 DIR_SDRAM_TO_IMEMDMEM, SEL_DMEM1, STORE_FORWARD_MODE);
			pMem = (unsigned int*)CACHED_TARGET_DMEM1_ADDR;
			enable_DMEM1();
			/* compare data in DMEM and DRAM */
               		for(i = 0; i < DMEM1_SIZE/4; i++)
               		{
                		if(uc_ins_addr[i] != *pMem)
                		{
                			printf("\n\t%s %d:src[%d](0x%x) != MEM 0x%x (0x%x)\n", \
                				__FUNCTION__, __LINE__, i, uc_ins_addr[i], pMem, *pMem);
                			retcode = FAILED;
                			goto test_fail;
                		}
                		pMem++;
               		}
		}


		write_back_invalidate_dcache();
		/* DMA IMEM data into DRAM */
		pMem = (unsigned int*)TO_UNCACHED_ADDR(TARGET_BACKUP_ADDR);
		if(choose==0)
		{
			imemDmemDMA((unsigned int)uc_ins_addr, (unsigned int)ins_addr, \
					(unsigned int)ins_addr, IMEM0_SIZE, DIR_IMEMDMEM_TO_SDRAM, \
					SEL_IMEM0, STORE_FORWARD_MODE);
			/* compare data with backup instructions */
               		for(i = 0; i < IMEM0_SIZE/4; i++)
                	{
                		if(uc_ins_addr[i] != *pMem)
                		{
                			printf("\n\t%s %d:src[%d](0x%x) != backup 0x%x (0x%x)\n", \
                			__FUNCTION__, __LINE__, i, uc_ins_addr[i], pMem, *pMem);
                			retcode = FAILED;
                			goto test_fail;
                		}
                		pMem++;
                	}
		}
		else
		{
			imemDmemDMA((unsigned int)uc_ins_addr, (unsigned int)ins_addr, \
					(unsigned int)ins_addr, IMEM1_SIZE, DIR_IMEMDMEM_TO_SDRAM, \
					SEL_IMEM1, STORE_FORWARD_MODE);
			/* compare data with backup instructions */
                	for(i = 0; i < IMEM1_SIZE/4; i++)
                	{
                		if(uc_ins_addr[i] != *pMem)
                		{
                			printf("\n\t%s %d:src[%d](0x%x) != backup 0x%x (0x%x)\n", \
                				__FUNCTION__, __LINE__, i, uc_ins_addr[i], pMem, *pMem);
                			retcode = FAILED;
                			goto test_fail;
                		}
                		pMem++;
                	}
		}



		if(run_times > 0)
			run_times--;

		/* flip flop */
		if(choose == 0)
		{
                	disable_DMEM0();
                	disable_IMEM0();
			choose = 1;
		}
		else
		{
                	disable_DMEM1();
                	disable_IMEM1();
			choose = 0;
		}
	}
	MEMCTL_DEBUG_PRINTF("\n");
test_fail:
	if(idmem_test_sel & IMEM0_MASK)
		disable_IMEM0();
	if(idmem_test_sel & IMEM1_MASK)
		disable_IMEM1();
	if(idmem_test_sel & DMEM0_MASK)
		disable_DMEM0();
	if(idmem_test_sel & DMEM1_MASK)
		disable_DMEM1();
	if(idmem_test_sel & L2MEM_MASK)
		disable_L2MEM();
	return retcode;


}



void test_banch(unsigned int *start, unsigned int *end, unsigned int rounds)
{
	unsigned int i;
	unsigned int tmp;
	volatile unsigned int *addr_start;

	for(i=0; i<=rounds; i++)
	{
		addr_start = start;
		while(addr_start <= end)
		{
			tmp = *addr_start;
			addr_start++;
		}
	}

}

/* Performance mesurement: CPU self-refill */
int iDMemTestCase8(void)
{
	unsigned int i;
	volatile unsigned int k;
	unsigned int imem_top;
	unsigned int imem_base;
	unsigned int couter[8];
	unsigned int ctr_value;

	/* Load ranges of IMEM0 and IMEM1. */
	enable_CP3();

	ctr_value = CP3_COUTER0(CYCLES)|CP3_COUTER1(INST_FEATCH)|\
                                        CP3_COUTER2(ICACHE_MISS)|CP3_COUTER3(ICACHE_MISS_CYCLE);
	//ctr_value = CP3_COUTER0(CYCLES)|CP3_COUTER1(LORDSTROE_INST)|\
                                        CP3_COUTER2(DCACHE_MISSE)|CP3_COUTER3(DCACHE_MISS_CYCLE);

	imem_base = PHYSADDR( ((unsigned int)CACHED_TARGET_IMEM0_ADDR)&((unsigned int)0xFFFFF000));

	disable_IMEM0();
	disable_IMEM1();

	MEMCTL_DEBUG_PRINTF("CPU self-refill\n");
	MEMCTL_DEBUG_PRINTF("size: cpu clock cycles\n");
	//for(i=16; i<=0x1000; i=i+16)
	for(i=0x400; i<=0x1000; i=i+0x400)
	{
		clear_CP3_counter();
		if(i==0)
			imem_top = PHYSADDR( imem_base);
		else	
			imem_top = PHYSADDR( imem_base + i - 1 );

		load_imem0Range(imem_top, imem_base);
		/* CPU self refill instructions */
		set_and_start_counter(ctr_value);
		IMEM0_refill();
		stop_counter();
		save_counter(&couter[0], &couter[2], &couter[4], &couter[6]);

		disable_IMEM0();
		disable_IMEM1();
		//MEMCTL_DEBUG_PRINTF("%04x: 0x%08x%08x\n",i ,couter[0], couter[1]);
		MEMCTL_DEBUG_PRINTF("%08x: %d\n", i,couter[1]);
		for(k=0; k<=100000; k++);
#if 1
//		MEMCTL_DEBUG_PRINTF("Cycles:            0x%08x%08x\n", couter[0], couter[1]);
//		MEMCTL_DEBUG_PRINTF("INST_FEATCH:       0x%08x%08x\n", couter[2], couter[3]);
//		MEMCTL_DEBUG_PRINTF("ICACHE_MISS:       0x%08x%08x\n", couter[4], couter[5]);
//		MEMCTL_DEBUG_PRINTF("ICACHE_MISS_CYCLE: 0x%08x%08x\n", couter[6], couter[7]);
#else
		MEMCTL_DEBUG_PRINTF("Cycles:            0x%08x%08x\n", couter[0], couter[1]);
		MEMCTL_DEBUG_PRINTF("LORDSTROE_INST:       0x%08x%08x\n", couter[2], couter[3]);
		MEMCTL_DEBUG_PRINTF("DCACHE_MISS:       0x%08x%08x\n", couter[4], couter[5]);
		MEMCTL_DEBUG_PRINTF("DCACHE_MISS_CYCLE: 0x%08x%08x\n", couter[6], couter[7]);
#endif
	}
	return SUCCESS;
}

char *modeName[] = {"STORE AND FORWARD", "ACCELERATION"};
/* Performance mesurement: IMEM/DMEM DMA: DMA into MEM */
int iDMemTestCase9(void)
{
	unsigned int i;
	volatile unsigned int k;
	volatile unsigned int *uc_src;
	unsigned int couter[8];
	unsigned int test_mode;
	unsigned int ctr_value;

	/* Load ranges of IMEM0 and IMEM1. */
	enable_CP3();

	ctr_value = CP3_COUTER0(CYCLES)|CP3_COUTER1(INST_FEATCH)|\
                                        CP3_COUTER2(ICACHE_MISS)|CP3_COUTER3(ICACHE_MISS_CYCLE);
	//ctr_value = CP3_COUTER0(CYCLES)|CP3_COUTER1(LORDSTROE_INST)|\
                                        CP3_COUTER2(DCACHE_MISSE)|CP3_COUTER3(DCACHE_MISS_CYCLE);

	uc_src = (unsigned int*)TARGET_IMEM0_ADDR;

	MEMCTL_DEBUG_PRINTF("IMEM/DMEM DMA Engine: DMA into IMEM0\n");
	for(test_mode=0; test_mode<=MODE_MAXNUM; test_mode++)
	{
		MEMCTL_DEBUG_PRINTF("mode: %s\n", modeName[test_mode]);
		MEMCTL_DEBUG_PRINTF("size: cpu clock cycles\n");
		for(i=0x400; i<=0x1000; i=i+0x400)
		{
			clear_CP3_counter();

			/* DMA src into DMEM */
			write_back_invalidate_dcache();
			set_and_start_counter(ctr_value);

			imemDmemDMA((unsigned int)uc_src, TARGET_IMEM0_ADDR, TARGET_IMEM0_ADDR, \
					i , DIR_SDRAM_TO_IMEMDMEM, SEL_IMEM0, test_mode);
			
			stop_counter();
			save_counter(&couter[0], &couter[2], &couter[4], &couter[6]);

			//MEMCTL_DEBUG_PRINTF("%04x: 0x%08x%08x\n",i ,couter[0], couter[1]);
			MEMCTL_DEBUG_PRINTF("%08x: %d\n", i,couter[1]);
			for(k=0; k<=100000; k++);
		}
	}
	return SUCCESS;

}

/* Performance mesurement: IMEM/DMEM DMA: DMA out into DRAM */
int iDMemTestCase10(void)
{
	unsigned int i;
	volatile unsigned int k;
	volatile unsigned int *uc_src;
	unsigned int couter[8];
	unsigned int test_mode;
	unsigned int ctr_value;

	/* Load ranges of IMEM0 and IMEM1. */
	enable_CP3();

	ctr_value = CP3_COUTER0(CYCLES)|CP3_COUTER1(INST_FEATCH)|\
                                        CP3_COUTER2(ICACHE_MISS)|CP3_COUTER3(ICACHE_MISS_CYCLE);
	//ctr_value = CP3_COUTER0(CYCLES)|CP3_COUTER1(LORDSTROE_INST)|\
                                        CP3_COUTER2(DCACHE_MISSE)|CP3_COUTER3(DCACHE_MISS_CYCLE);

	uc_src = (unsigned int*)TARGET_IMEM0_ADDR;

	MEMCTL_DEBUG_PRINTF("IMEM/DMEM DMA Engine: DMA out into DRAM\n");
	for(test_mode=0; test_mode<=MODE_MAXNUM; test_mode++)
	{
		MEMCTL_DEBUG_PRINTF("mode: %s\n", modeName[test_mode]);
		MEMCTL_DEBUG_PRINTF("size: cpu clock cycles\n");
		for(i=0x400; i<=0x1000; i=i+0x400)
		{
			clear_CP3_counter();

			/* DMA src into DMEM */
			write_back_invalidate_dcache();
			set_and_start_counter(ctr_value);

			imemDmemDMA((unsigned int)uc_src, TARGET_IMEM0_ADDR, TARGET_IMEM0_ADDR, \
					i , DIR_IMEMDMEM_TO_SDRAM, SEL_IMEM0, test_mode);
			
			stop_counter();
			save_counter(&couter[0], &couter[2], &couter[4], &couter[6]);

			//MEMCTL_DEBUG_PRINTF("%04x: 0x%08x%08x\n",i ,couter[0], couter[1]);
			MEMCTL_DEBUG_PRINTF("%08x: %d\n", i,couter[1]);
			for(k=0; k<=100000; k++);
		}
	}
	return SUCCESS;

}

int iDMemTestCase11(void)
{

	int j;
	int retcode;
	unsigned int i;
	unsigned int test_size;
	unsigned int target;
	unsigned int test_mode;
	unsigned int k;
	volatile unsigned int *pMem;
	volatile unsigned int *uc_src;
	volatile unsigned int *uc_dist;
	unsigned int tmp;
	unsigned int imem_read_buff[2];


	/* Initialize the environment */

	retcode = SUCCESS;
	uc_src = (unsigned int *)TO_UNCACHED_ADDR(0x81000000) ;
	uc_dist = (unsigned int *)TO_UNCACHED_ADDR(0x81100000) ;
	
	//MEMCTL_DEBUG_PRINTF("uc_src = 0x%x, uc_dist = 0x%x\n", uc_src, uc_dist);

	for(test_mode=0; test_mode<=MODE_MAXNUM; test_mode++)
	{
		init_Env(0);
#if 0		
		if(test_mode==0)
			MEMCTL_DEBUG_PRINTF("mode: store and forward \n");
		else
			MEMCTL_DEBUG_PRINTF("mode: Acceleration\n");
		for(target=SEL_IMEM0; target<=SEL_L2MEM; target++)
		{
			/* init src and dist */
			write_back_invalidate_dcache();
			for(j=0; j<(sizeof(src)/4); j++)
			{
				uc_dist[j] = 0;
				uc_src[j] = 0;
			}
			MEMCTL_DEBUG_PRINTF("\n target: %s, size: 0x%08x\n", targetName[target], targetSize[target]);
		for(i=0; i<32; i=i+4)
		{
			
			MEMCTL_DEBUG_PRINTF("\r Pattern: address rotate %d", i);	
			for(test_size=1; test_size<=(targetSize[target]/4); test_size=test_size*2)
			{
				write_back_invalidate_dcache();
				/* Set source data */
				for(j=0; j<test_size; j++)
				{
					uc_src[j] = ((unsigned int)(&uc_src[j])) << i;
				}
#ifdef READ_FLUSH_OCP_WRITE /* flush OCP pending write */
				tmp = uc_src[j-1];
#else
				*((volatile unsigned int *)(0xB8001030)) = \
					*((volatile unsigned int *)(0xB8001030)) | 0x80000000;

				while(*((volatile unsigned int *)(0xB8001030)) != 0);
#endif

				/* DMA src into DMEM */
				write_back_invalidate_dcache();
				imemDmemDMA((unsigned int)uc_src, CACHED_TARGET_DMEM0_ADDR, CACHED_TARGET_DMEM0_ADDR, \
						test_size*4, DIR_SDRAM_TO_IMEMDMEM, target, test_mode);

				/* DMA Data out form DMEME into distination */
				imemDmemDMA((unsigned int)uc_dist, CACHED_TARGET_DMEM0_ADDR, CACHED_TARGET_DMEM0_ADDR, \
						test_size*4, DIR_IMEMDMEM_TO_SDRAM, target, test_mode);

				/* Compare src and dist */
				for(j=0; j<(sizeof(src)/4); j++)
				{
					if(uc_src[j] != uc_dist[j])
					{
						printf("\n\t%s %d:src[%d](0x%x) != dist[%d](0x%x)\n", \
							__FUNCTION__, __LINE__, j, uc_src[j], j, uc_dist[j]);
						retcode = FAILED;
						goto test_fail;
					}
				}
				
			}
		}
		}
		MEMCTL_DEBUG_PRINTF("\n");
#endif
	}
	return retcode;

test_fail:
	disable_IMEM0();
	disable_IMEM1();
	disable_DMEM0();
	disable_DMEM1();
//#ifdef CONFIG_RTL8198
//	disable_L2MEM();
//#endif
	return retcode;
}


int iDMemTestCase12(void)
{
	volatile unsigned int* p_temp;
	volatile unsigned int  temp;

	enable_CP3();
	//MEMCTL_DEBUG_PRINTF("start case 12 test\n");
	//MEMCTL_DEBUG_PRINTF("CACHED_TARGET_BACKUP_ADDR = 0x%x\n", CACHED_TARGET_BACKUP_ADDR);
	//MEMCTL_DEBUG_PRINTF("TARGET_BACKUP_ADDR = 0x%x\n", TARGET_BACKUP_ADDR);
	//MEMCTL_DEBUG_PRINTF("DMEM0_TOP_ADDR = 0x%x\n", DMEM0_TOP_ADDR);
	//MEMCTL_DEBUG_PRINTF("DMEM0_BASE_ADDR = 0x%x\n", DMEM0_BASE_ADDR);

	/* load DMEM0 range. */
	load_dmem0Range(DMEM0_TOP_ADDR, DMEM0_BASE_ADDR);
	/* enable DMEM0. */
	enable_DMEM0();
	/* Init DMEM0 with 0xAAAAAAAA. */
while(1)
{
	MEMCTL_DEBUG_PRINTF("\r.");
	/* Init distination data with 0xDDDDDDDD. (with uncached address) */
	memset((unsigned char *)TARGET_BACKUP_ADDR, 0xDD, 64);
	memset((unsigned char *)CACHED_TARGET_DMEM0_ADDR, 0xAA, DMEM0_SIZE);
	/* Read distination data with cached address. (expected cache line state is clean.) */

	p_temp = (unsigned int*) (CACHED_TARGET_BACKUP_ADDR);
	temp = *p_temp;
//	MEMCTL_DEBUG_PRINTF("0x%08x: %x %x %x %x\n", CACHED_TARGET_BACKUP_ADDR,\ 
//					*((unsigned int*)(CACHED_TARGET_BACKUP_ADDR)),\
//					*((unsigned int*)(CACHED_TARGET_BACKUP_ADDR)+1), \
//					*((unsigned int*)(CACHED_TARGET_BACKUP_ADDR)+2), \
//					*((unsigned int*)(CACHED_TARGET_BACKUP_ADDR)+3));
//	MEMCTL_DEBUG_PRINTF("0x%08x: %x %x %x %x\n", CACHED_TARGET_BACKUP_ADDR+16,\
//					*((unsigned int*)(CACHED_TARGET_BACKUP_ADDR)+4), \
//					*((unsigned int*)(CACHED_TARGET_BACKUP_ADDR)+5), \
//					*((unsigned int*)(CACHED_TARGET_BACKUP_ADDR)+6), \
//					*((unsigned int*)(CACHED_TARGET_BACKUP_ADDR)+7));

	//MEMCTL_DEBUG_PRINTF("0x%08x: %x\n", (unsigned int)p_temp, temp);
	p_temp = (unsigned int*)TARGET_BACKUP_ADDR;
	/* DMA DMEM0 data into the distination in DRAM. */
	imemDmemDMA((unsigned int)TARGET_BACKUP_ADDR, (unsigned int)CACHED_TARGET_DMEM0_ADDR, \
			(unsigned int)CACHED_TARGET_DMEM0_ADDR, 32, DIR_IMEMDMEM_TO_SDRAM, \
			SEL_DMEM0, STORE_FORWARD_MODE);
	temp = *p_temp;

	/* Read distination data with uncached address. */
//	MEMCTL_DEBUG_PRINTF("0x%08x: %x %x %x %x\n", TARGET_BACKUP_ADDR, \
//					*((unsigned int*)(TARGET_BACKUP_ADDR)), \
//					*((unsigned int*)(TARGET_BACKUP_ADDR)+1), \
//					*((unsigned int*)(TARGET_BACKUP_ADDR)+2), \
//					*((unsigned int*)(TARGET_BACKUP_ADDR)+3));
//	MEMCTL_DEBUG_PRINTF("0x%08x: %x %x %x %x\n", TARGET_BACKUP_ADDR+16,\
//					*((unsigned int*)(TARGET_BACKUP_ADDR)+4), \
//					*((unsigned int*)(TARGET_BACKUP_ADDR)+5), \
//					*((unsigned int*)(TARGET_BACKUP_ADDR)+6), \
//					*((unsigned int*)(TARGET_BACKUP_ADDR)+7));

	//MEMCTL_DEBUG_PRINTF("0x%08x: %x\n", (unsigned int)p_temp, temp);
}
	return SUCCESS;
}


/* Test Case 13
 * Data Overwrite test: ( IMEM0, IMEM1, DMEM0, DMEM1) Memory(address rotating) */
/* 0xA0800000: IMEM0 (32KB)
 * 0xA0808000: IMEM1 (8KB)
 * 0xA0810000: DMEM0 (4KB)
 * 0xA0811000: DMEM1 (4KB)
 *
 * 0. Setting MEM top/base.
 * 1. Clear Data in MEM.
 * 2. Fill address data into MEM mapped area in DRAM .
 * 3. IDMEM DMA into MEM .
 * 5. Clear Data in MEM mapped area in DRAM and check clear area.
 * 4. Enable MEM and check data in MEM.
 * 6. Disable MEM and DMA out data into DRAM.
 * 7. Check data in MEM mapped area in DRAM.
 */
#define C13_IDMEM_MEM_BASE 	(0xA0800000)
#define C13_IMEM0_BASE		C13_IDMEM_MEM_BASE
#define C13_IMEM1_BASE		(C13_IMEM0_BASE+IMEM0_SIZE)
#define C13_DMEM0_BASE		(C13_IMEM1_BASE+IMEM1_SIZE)
#define C13_DMEM1_BASE		(C13_DMEM0_BASE+DMEM0_SIZE)
#define C13_TOTAL_MEMSIZE	(IMEM0_SIZE+IMEM1_SIZE+DMEM0_SIZE+DMEM1_SIZE)
int iDMemTestCase13(void)
{
	unsigned int *pMem;
	unsigned int *pDMem;
	unsigned int *pIMem;
	unsigned int i;
	unsigned int test_mode;
	unsigned int imem_refill;
	unsigned int imem_read_buff[2];
	int retcode;
	unsigned int run_times;
	unsigned int write_data;
	unsigned int pat_num;
	
	run_times = 0x1000;
	imem_refill = 0;
	MEMCTL_DEBUG_PRINTF("Data overwrite test: run 0x%x times\n", run_times);

	/* 0. setup MEM top/base. */
	enable_CP3();
	load_imem0Range( PHYSADDR(C13_IMEM0_BASE+IMEM0_SIZE-1), PHYSADDR(C13_IMEM0_BASE) );
	load_imem1Range( PHYSADDR(C13_IMEM1_BASE+IMEM1_SIZE-1), PHYSADDR(C13_IMEM1_BASE) );
	load_dmem0Range( PHYSADDR(C13_DMEM0_BASE+DMEM0_SIZE-1), PHYSADDR(C13_DMEM0_BASE) );
	load_dmem1Range( PHYSADDR(C13_DMEM1_BASE+DMEM1_SIZE-1), PHYSADDR(C13_DMEM1_BASE) );

	/* Disable MEM */	
	disable_IMEM0();
	disable_IMEM1();
	disable_DMEM0();
	disable_DMEM1();

	while(run_times--)
	{
		pat_num = (pat_num + 1) % ((sizeof(data_patterns)/4)+1);
		if(pat_num == (sizeof(data_patterns)/4))
			MEMCTL_DEBUG_PRINTF("\rLeft times: 0x%08x, pattern(addr_rot)",run_times);
		else
			MEMCTL_DEBUG_PRINTF("\rLeft times: 0x%08x, pattern(0x%08x)",run_times, data_patterns[pat_num]);
		for(test_mode = 0; test_mode <= MODE_MAXNUM; test_mode++)
		{
			/* 1. Clear Data in MEM */
			enable_DMEM0();
			enable_DMEM1();
			pMem = (unsigned int *)(C13_IDMEM_MEM_BASE & 0xDFFFFFFF);
			for(i=0; i<(C13_TOTAL_MEMSIZE/4); i++)
			{
				pMem[i] = (0x0);
			}
			IMEM0_refill();
			IMEM1_refill();
			disable_IMEM0();
			disable_IMEM1();
			disable_DMEM0();
			disable_DMEM1();


			/* 2. Setup data of MEM mapped in DRAM */
			pMem = (unsigned int *)C13_IDMEM_MEM_BASE;
			for(i=0; i<(C13_TOTAL_MEMSIZE/4); i++)
			{
				/* address rotating pattern */
				if(pat_num == (sizeof(data_patterns)/4))
				{
					write_data = (((unsigned int)(&pMem[i]))&0xDFFFFFFF);/*cached address*/
				}
				else /*nomal patterns */
					write_data = data_patterns[pat_num];
	
				pMem[i] = write_data;
			}
#ifdef READ_FLUSH_OCP_WRITE /* flush OCP pending write */
			i = pMem[i-1];
#else
			*((volatile unsigned int *)(0xB8001030)) = \
			*((volatile unsigned int *)(0xB8001030)) | 0x80000000;

			while(*((volatile unsigned int *)(0xB8001030)) != 0);
#endif
			/* 3. DMA data into MEM */
			/* IMEM0/IMEM1 */
			if(imem_refill == 1)
			{
				IMEM0_refill();
				IMEM1_refill();
				imem_refill = 0;
			}
			else{
				imem_refill = 1;
				imemDmemDMA((unsigned int)C13_IMEM0_BASE, C13_IMEM0_BASE, C13_IMEM0_BASE, \
					IMEM0_SIZE, DIR_SDRAM_TO_IMEMDMEM, SEL_IMEM0, test_mode);
				imemDmemDMA((unsigned int)C13_IMEM1_BASE, C13_IMEM1_BASE, C13_IMEM1_BASE, \
					IMEM1_SIZE, DIR_SDRAM_TO_IMEMDMEM, SEL_IMEM1, test_mode);
			}

			/* DMEM0 */
			imemDmemDMA((unsigned int)C13_DMEM0_BASE, C13_DMEM0_BASE, C13_DMEM0_BASE, \
				DMEM0_SIZE, DIR_SDRAM_TO_IMEMDMEM, SEL_DMEM0, test_mode);
			/* DMEM1 */
			imemDmemDMA((unsigned int)C13_DMEM1_BASE, C13_DMEM1_BASE, C13_DMEM1_BASE, \
				DMEM1_SIZE, DIR_SDRAM_TO_IMEMDMEM, SEL_DMEM1, test_mode);

			/* 4. Clear data in MEM mapped DRAM data. */
			for(i=0; i<(C13_TOTAL_MEMSIZE/4); i++)
			{
				pMem[i] = 0x0;
			}
				/* Check data */
			for(i=0; i<(C13_TOTAL_MEMSIZE/4); i++)
			{
				if(pMem[i]!= 0x0){
					MEMCTL_DEBUG_PRINTF("pMem[%d](0x%x) != 0\n", i, pMem[i]);	
				}
			}

			/* 5. Check data in MEM */
			pIMem = (unsigned int *)(C13_IMEM0_BASE & 0xDFFFFFFF);
			enable_IMEM0();
			write_data = data_patterns[pat_num];
			for(i=0; i<(IMEM0_SIZE/4); i = i + 2)
			{
				if(pat_num == (sizeof(data_patterns)/4))
					write_data = ((unsigned int)pIMem);

				read_imem_8bytes(pIMem , &imem_read_buff[0]);
				if( imem_read_buff[0] != write_data)
				{
					MEMCTL_DEBUG_PRINTF("\n\t%s %d:pIMem0[%d](0x%x) != MEM 0x%x \n"\
						, __FUNCTION__, __LINE__, i, write_data, \
						imem_read_buff[0]);
					retcode = FAILED;
					goto test_fail;
				}

                                if(pat_num == (sizeof(data_patterns)/4))
                                        write_data = ((unsigned int)(pIMem+1));

				if(((IMEM0_SIZE/4) - i ) >= 2) /*only compare low 4 bytes*/
				{
					if(write_data != imem_read_buff[1])
					{
						printf(\
							"\n\t%s %d:pIMem0[%d](0x%x) != MEM 0x%x \n"\
							, __FUNCTION__, __LINE__, i+1, write_data, \
							imem_read_buff[1]);
						retcode = FAILED;
						goto test_fail;
					}
				}
				pIMem = pIMem + 2;
			}

			pIMem = (unsigned int *)(C13_IMEM1_BASE & 0xDFFFFFFF);
			enable_IMEM1();
			for(i=0; i<(IMEM1_SIZE/4); i = i + 2)
			{
				if(pat_num == (sizeof(data_patterns)/4))
					write_data = ((unsigned int)pIMem);

				read_imem_8bytes(pIMem , &imem_read_buff[0]);
				if(write_data != imem_read_buff[0])
				{
					printf(\
						"\n\t%s %d:pIMem1[%d](0x%x) != MEM 0x%x \n"\
						, __FUNCTION__, __LINE__, i, write_data, \
						imem_read_buff[0]);
					retcode = FAILED;
					goto test_fail;
				}

                                if(pat_num == (sizeof(data_patterns)/4))
                                        write_data = ((unsigned int)(pIMem+1));

				if(((IMEM1_SIZE/4) - i ) >= 2) /*only compare low 4 bytes*/
				{
					if(write_data != imem_read_buff[1])
					{
						printf(\
							"\n\t%s %d:pIMem1[%d](0x%x) != MEM 0x%x \n"\
							, __FUNCTION__, __LINE__, i+1, write_data, \
							imem_read_buff[1]);
						retcode = FAILED;
						goto test_fail;
					}
				}
				pIMem = pIMem + 2;
			}
	
			enable_DMEM0();
			pDMem = (unsigned int*)(C13_DMEM0_BASE & 0xDFFFFFFF);
			for(i=0; i<(DMEM0_SIZE/4); i++)
			{
				if(pat_num == (sizeof(data_patterns)/4))
					write_data = ((unsigned int)&pDMem[i]);

				if(pDMem[i] != write_data)
				{
					printf("\n\t%s %d:pDMem0[%d](0x%x) != (0x%x)\n", \
						__FUNCTION__, __LINE__, i, pDMem[i], \
						write_data);
					retcode = FAILED;
					goto test_fail;
				}
			}
			enable_DMEM1();
			pDMem = (unsigned int*)(C13_DMEM1_BASE & 0xDFFFFFFF);
			for(i=0; i<(DMEM0_SIZE/4); i++)
			{
				if(pat_num == (sizeof(data_patterns)/4))
					write_data = ((unsigned int)&pDMem[i]);

				if(pDMem[i] != write_data)
				{
					printf("\n\t%s %d:pDMem1[%d](0x%x) != (0x%x)\n", \
						__FUNCTION__, __LINE__, i, pDMem[i], \
						write_data);
					retcode = FAILED;
					goto test_fail;
				}
			}
	
			/* Disable MEM */	
			disable_IMEM0();
			disable_IMEM1();
			disable_DMEM0();
			disable_DMEM1();


			/* 6. DMA back into DRAM */
			/* IMEM0 */
			imemDmemDMA((unsigned int)C13_IMEM0_BASE, C13_IMEM0_BASE, C13_IMEM0_BASE, \
				IMEM0_SIZE, DIR_IMEMDMEM_TO_SDRAM, SEL_IMEM0, test_mode);
			/* IMEM1 */
			imemDmemDMA((unsigned int)C13_IMEM1_BASE, C13_IMEM1_BASE, C13_IMEM1_BASE, \
				IMEM1_SIZE, DIR_IMEMDMEM_TO_SDRAM, SEL_IMEM1, test_mode);
			/* DMEM0 */
			imemDmemDMA((unsigned int)C13_DMEM0_BASE, C13_DMEM0_BASE, C13_DMEM0_BASE, \
				DMEM0_SIZE, DIR_IMEMDMEM_TO_SDRAM, SEL_DMEM0, test_mode);
			/* DMEM1 */
			imemDmemDMA((unsigned int)C13_DMEM1_BASE, C13_DMEM1_BASE, C13_DMEM1_BASE, \
				DMEM1_SIZE, DIR_IMEMDMEM_TO_SDRAM, SEL_DMEM1, test_mode);

			/* 7. Check data in DRAM mapped by MEM. */
			for(i=0; i<(C13_TOTAL_MEMSIZE/4); i++)
			{
				if(pat_num == (sizeof(data_patterns)/4))
					write_data = (((unsigned int)(&pMem[i])) & 0xDFFFFFFF);

				if(pMem[i] != write_data)
				{
					printf("\n\t%s %d:pMem[%d](0x%x) != (0x%x)\n", \
						__FUNCTION__, __LINE__, i, pMem[i], \
						write_data);
					retcode = FAILED;
					goto test_fail;
				}
			}
		}
	}
	retcode = SUCCESS;
test_fail:
	return retcode;
}

