
/*
 * Include Files
 */
#include <common.h>
#include <exports.h>
#include <flash.h>
#include <asm/arch/memctl.h>

#ifndef CONFIG_RTL0371S 
#define	FLASH_BASE_ADDRESS	(0xBD000000)
#define	FLASH_SIZE         	(gd->bd->bi_flashsize)
#define	FLASH_TOP_ADDRESS	(FLASH_BASE_ADDRESS+FLASH_SIZE-1)
#define	SRC_DATA_ADDR		((0x80000000) + (memctlc_dram_size()- 0x100000) - (2*FLASH_SIZE))
#define	FLASH_BACKUP_ADDR	((0x80000000) + (memctlc_dram_size()- 0x100000) - FLASH_SIZE)
#define	TEST_SIZE_PER_PATTREN	(0x10000) //64KB
//#define HANDLE_FAIL  goto test_fail;
#define HANDLE_FAIL  while(1);
#define JFFS2_START  (0x60000)


/*
 * Data Declaration
 */
DECLARE_GLOBAL_DATA_PTR;

extern flash_info_t  flash_info[]; /* info for FLASH chips */

uint32 flash_patterns[] = {
					0x00000000,
					0xff00ff00,
					0x00ff00ff,
					0x0000ffff,
					0xffff0000,
					0xffffffff,
					0x5a5aa5a5,
					0xa5a5a5a5,
					0x55555555, 
					0xaaaaaaaa, 
					0x01234567, 
					0x76543210, 
					0x89abcdef,
					0xfedcba98,
			};

/*
 * Function Declaration
 */

/* Function Name: 
 * 	nor_normal_patterns
 * Descripton:
 *	
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *  	None
 */
int flash_normal_patterns(uint32 flash_start_addr, uint32 test_size_per_pattern, uint32 flash_test_size)
{
	int i;
	int j;
	uint32 start_value;
	uint32 flash_start;
	volatile uint32 *src_start;

	printf("=======start %s test=======\n", __FUNCTION__);
	for (i=0; i < (sizeof(flash_patterns)/sizeof(uint32)); i++)
	{
	
		_cache_flush();
		
		/* write pattern*/
		src_start = (uint32 *)SRC_DATA_ADDR;
		for(j=0; j < test_size_per_pattern; j=j+4)
		{
			*src_start = flash_patterns[i];
			src_start++;
		}
		
		_cache_flush();
		
		/* check data */
		src_start = (uint32 *)SRC_DATA_ADDR;
		for(j=0; j < test_size_per_pattern; j=j+4)
		{
			start_value = (*src_start);
			if(start_value != flash_patterns[i])
			{
				printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
				src_start , start_value, flash_patterns[i],  __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			src_start++;
		}
		printf("Flash:pattern[%d](0x%x) setting pass\n", i, flash_patterns[i]);
		
		src_start = (uint32 *)SRC_DATA_ADDR;
		flash_start = flash_start_addr + ((i*test_size_per_pattern)%flash_test_size);
		flash_sect_erase(flash_start, (flash_start+test_size_per_pattern-1));
		flash_write((char *)src_start, flash_start, test_size_per_pattern);
		
		/* check flash data sequentially. Uncached address */
		src_start = (uint32 *)(TO_UNCACHED_ADDR((uint32)flash_start));
		for(j=0; j < test_size_per_pattern; j=j+4)
		{
			start_value = (*src_start);
			if(start_value != flash_patterns[i])
			{
				printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
				src_start , start_value, flash_patterns[i],  __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			src_start++;
		}
		
		/* check flash data interlevelingly. Uncached address */
		src_start = (uint32 *)(TO_UNCACHED_ADDR((uint32)flash_start));
		for(j=0; j < (test_size_per_pattern/2); j=j+4)
		{
			start_value = (*src_start);
			if(start_value != flash_patterns[i])
			{
				printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
					src_start , start_value, flash_patterns[i],  __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			
			start_value = *(src_start + ((unsigned int)test_size_per_pattern/8));
			if(start_value != flash_patterns[i])
			{
				printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
				      (uint32)(src_start + ((unsigned int)test_size_per_pattern/8)) , start_value, flash_patterns[i],  __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			src_start++;
		}
		

		/* check flash data sequentially. Cached address */
		src_start = (uint32 *)(TO_CACHED_ADDR((uint32)flash_start));
		for(j=0; j < test_size_per_pattern; j=j+4)
		{
			start_value = (*src_start);
			if(start_value != flash_patterns[i])
			{
				printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
				src_start , start_value, flash_patterns[i],  __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			src_start++;
		}
		
		/* check flash data interlevelingly. Uncached address */
		src_start = (uint32 *)(TO_CACHED_ADDR((uint32)flash_start));
		for(j=0; j < (test_size_per_pattern/2); j=j+4)
		{
			start_value = (*src_start);
			if(start_value != flash_patterns[i])
			{
				printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
					src_start , start_value, flash_patterns[i],  __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			
			start_value = *(src_start + ((unsigned int)test_size_per_pattern/8));
			if(start_value != flash_patterns[i])
			{
				printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
				      (uint32)(src_start + ((unsigned int)test_size_per_pattern/8)) , start_value, flash_patterns[i],  __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			src_start++;
		}

		printf("Flash: pattern[%d](0x%x) 0x%x pass\n", i, flash_patterns[i], flash_start);
		
		printf("pattern[%d](0x%x) completed\n", i, flash_patterns[i]);
	}
	

	printf("%s test succeed.\n", __FUNCTION__);
	return 0;
test_fail:
	printf("%s test fails.\n", __FUNCTION__);
	return -1;

}

int flash_walking_of_1(uint32 flash_start_addr, uint32 test_size_per_pattern, uint32 flash_test_size)
{
	int i;
	int j;
	uint32 walk_pattern;
	uint32 start_value;
	uint32 flash_start;
	volatile uint32 *src_start;
	
	printf("=======start %s test=======\n", __FUNCTION__);
	for (i=0; i < 32; i++)
	{
		_cache_flush();
		
		/* generate pattern */
		walk_pattern = (1 << i);
		
		/* write pattern*/
		src_start = (uint32 *)SRC_DATA_ADDR;
		for(j=0; j < test_size_per_pattern; j=j+4)
		{
			*src_start = walk_pattern;
			src_start++;
		}
		
		_cache_flush();
		
		/* check data */  
		src_start = (uint32 *)SRC_DATA_ADDR;
		for(j=0; j < test_size_per_pattern; j=j+4)
		{
			start_value = (*src_start);
			if(start_value != walk_pattern)
			{
				printf("addr(0x%x): 0x%x% != pattern(0x%x) %s, %d\n",\
				src_start , start_value, walk_pattern,  __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			src_start++;
		}
		printf("Flash: pattern[%d](0x%x) setting passed\n", i, walk_pattern);

		src_start = (uint32 *)SRC_DATA_ADDR;
		flash_start = flash_start_addr + ((i*test_size_per_pattern)%flash_test_size);
		flash_sect_erase(flash_start, (flash_start+test_size_per_pattern-1));
		flash_write((char *)src_start, flash_start, test_size_per_pattern);


		/* check data */  
		src_start = (uint32 *)(TO_UNCACHED_ADDR((uint32)flash_start));
		for(j=0; j < test_size_per_pattern; j=j+4)
		{
			start_value = (*src_start);
			if(start_value != walk_pattern)
			{
				printf("addr(0x%x): 0x%x% != pattern(0x%x) %s, %d\n",\
				src_start , start_value, walk_pattern,  __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			src_start++;
		}
		printf("Flash: pattern[%d](0x%x) 0x%x passed\n", i, walk_pattern, flash_start);
		
		printf("pattern[%d](0x%x) completed\n", i, walk_pattern);
	}
	printf("%s test succeed.\n", __FUNCTION__);
	return 0;

test_fail:
	printf("%s test fails.\n", __FUNCTION__);
	return -1;
}

int flash_walking_of_0(uint32 flash_start_addr, uint32 test_size_per_pattern, uint32 flash_test_size)
{
	int i;
	int j;
	uint32 start_value;
	uint32 walk_pattern;
	uint32 flash_start;
	volatile uint32 *src_start;
	
	printf("=======start %s test=======\n", __FUNCTION__);
	for (i=0; i < 32; i++)
	{
		_cache_flush();
		
		/* generate pattern */
		walk_pattern = ~(1 << i);
		
		/* write pattern*/
		src_start = (uint32 *)SRC_DATA_ADDR;
		for(j=0; j < test_size_per_pattern; j=j+4)
		{
			*src_start = walk_pattern;
			src_start++;
		}
		
		_cache_flush();
		
		/* check data */  
		src_start = (uint32 *)SRC_DATA_ADDR;
		for(j=0; j < test_size_per_pattern; j=j+4)
		{
			start_value = (*src_start);
			if(start_value != walk_pattern)
			{
				printf("addr(0x%x): 0x%x% != pattern(0x%x) %s, %d\n",\
				src_start , start_value, walk_pattern,  __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			src_start++;
		}
		printf("Flash:pattern[%d](0x%x) setting passed\n", i, walk_pattern);
		
		src_start = (uint32 *)SRC_DATA_ADDR;
		flash_start = flash_start_addr + ((i*test_size_per_pattern)%flash_test_size);
		flash_sect_erase(flash_start, (flash_start+test_size_per_pattern-1));
		flash_write((char *)src_start, flash_start, test_size_per_pattern);

		/* check data */  
		src_start = (uint32 *)(TO_UNCACHED_ADDR((uint32)flash_start));
		for(j=0; j < test_size_per_pattern; j=j+4)
		{
			start_value = (*src_start);
			if(start_value != walk_pattern)
			{
				printf("addr(0x%x): 0x%x% != pattern(0x%x) %s, %d\n",\
				src_start , start_value, walk_pattern,  __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			src_start++;
		}
		printf("Flash:pattern[%d](0x%x) 0x%x passed\n", i, walk_pattern, flash_start);
		
		printf("pattern[%d](0x%x) completed\n", i, walk_pattern);
	}
	printf("%s test succeed.\n", __FUNCTION__);
	return 0;
test_fail:
	printf("%s test fails.\n", __FUNCTION__);
	return -1;

}

int flash_addr_rot(uint32 flash_start_addr, uint32 test_size_per_pattern, uint32 flash_test_size)
{
	int i;
	int j;
	uint32 start_value;
	uint32 flash_start;
	volatile uint32 *_dram_start;
	volatile uint32 *src_start;
	
	printf("=======start %s test=======\n", __FUNCTION__);
	for (i=0; i < 32; i=i+4)
	{
		_cache_flush();
		
		/* write pattern*/
		src_start = (uint32 *)SRC_DATA_ADDR;
		for(j=0; j < test_size_per_pattern; j=j+4)
		{
			*src_start = ((uint32)src_start << i);
			src_start++;
		}
		
		_cache_flush();
		
		/* check data */  
		src_start = (uint32 *)SRC_DATA_ADDR;
		for(j=0; j < test_size_per_pattern; j=j+4)
		{
			start_value = (*src_start);
			if(start_value != (((uint32)src_start) << i))
			{
				printf("addr(0x%x): 0x%x% != pattern(0x%x) %s, %d\n",\
				      src_start , start_value, (((uint32)src_start) << i),  __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			src_start++;
		}
		printf("Flash: rotate %d setting passed\n", i);
		

		src_start = (uint32 *)SRC_DATA_ADDR;
		flash_start = flash_start_addr + ((i*test_size_per_pattern)%flash_test_size);
		flash_sect_erase(flash_start, (flash_start+test_size_per_pattern-1));
		flash_write((char *)src_start, flash_start, test_size_per_pattern);

		/* check data */  
		_dram_start = (uint32 *) SRC_DATA_ADDR;
		src_start = (uint32 *)(TO_UNCACHED_ADDR((uint32)flash_start));
		for(j=0; j < test_size_per_pattern; j=j+4)
		{
			start_value = (*src_start);
			if(start_value != (((uint32)_dram_start) << i))
			{
				printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
				src_start , start_value, (((uint32)_dram_start) << i),  __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			src_start++;
			_dram_start++;
		}
		printf("Flash: rotate %d 0x%x passed\n", i, flash_start);
		
		printf("rotate %d completed\n", i);
	}
	printf("%s test succeed.\n", __FUNCTION__);
	return 0;
	
test_fail:
	printf("%s test fails.\n", __FUNCTION__);
	return -1;
}

int flash_com_addr_rot(uint32 flash_start_addr, uint32 test_size_per_pattern, uint32 flash_test_size)
{
	int i;
	int j;
	uint32 start_value;
	uint32 flash_start;
	volatile uint32 *_dram_start;
	volatile uint32 *src_start;
	
	printf("=======start %s test=======\n", __FUNCTION__);
	for (i=0; i < 32; i=i+4)
	{
		_cache_flush();
		
		/* write pattern*/
		src_start = (uint32 *)SRC_DATA_ADDR;
		for(j=0; j < test_size_per_pattern; j=j+4)
		{
			*src_start = ~(((uint32)src_start) << i);
			src_start++;
		}
		
		_cache_flush();
		
		/* check data */  
		src_start = (uint32 *)SRC_DATA_ADDR;
		for(j=0; j < test_size_per_pattern; j=j+4)
		{
			start_value = (*src_start);
			if(start_value != (~((uint32)src_start << i)))
			{
				 printf("addr(0x%x): 0x%x% != pattern(0x%x) %s, %d\n",\
				        src_start , start_value, ~(((uint32)src_start) << i),  __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			src_start++;
		}
		printf("Flash: ~rotate %d setting passed\n", i);
		
		_cache_flush();
		
		src_start = (uint32 *)SRC_DATA_ADDR;
		flash_start = flash_start_addr + ((i*test_size_per_pattern)%flash_test_size);
		flash_sect_erase(flash_start, (flash_start+test_size_per_pattern-1));
		flash_write((char *)src_start, flash_start, test_size_per_pattern);
		
		/* check data */  
		_dram_start = (uint32 *) SRC_DATA_ADDR;
		src_start = (uint32 *)(TO_UNCACHED_ADDR((uint32)flash_start));
		for(j=0; j < test_size_per_pattern; j=j+4)
		{
			start_value = (*src_start);
			if(start_value != (~((uint32)_dram_start << i)))
			{
				printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
				src_start , start_value, ~(((uint32)_dram_start) << i),  __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			src_start++;
			_dram_start++;
		}
		printf("Flash: ~rotate %d 0x%x passed\n", i, flash_start);
		
		printf("~rotate %d completed\n", i);
	}
	printf("%s test succeed.\n", __FUNCTION__);
	return 0 ;

test_fail:
	printf("%s test fails.\n", __FUNCTION__);
	return -1;

}

int _flash_test(uint32 flash_start_addr, uint32 test_size_per_pattern, uint32 flash_test_size)
{
	int retcode;

	/* partial range */
	retcode = flash_normal_patterns( flash_start_addr, test_size_per_pattern, flash_test_size);
	if(retcode != 0)
		return retcode;

	retcode = flash_walking_of_1( flash_start_addr, test_size_per_pattern, flash_test_size);
	if(retcode != 0)
		return retcode;

	retcode = flash_walking_of_0( flash_start_addr, test_size_per_pattern, flash_test_size);
	if(retcode != 0)
		return retcode;

	retcode = flash_addr_rot( flash_start_addr, test_size_per_pattern, flash_test_size);
	if(retcode != 0)
		return retcode;

	retcode = flash_com_addr_rot( flash_start_addr, flash_test_size, flash_test_size);
	if(retcode != 0)
		return retcode;

	return 0;
}


int flash_test(int flag, int argc, void* arg[])
{
	volatile uint32 *bootcode;
	volatile uint32 *bk_buffer;
	int32 retcode;
	uint32 i;
	

	/* 0. Back up and verify whole flash data. */
	bk_buffer = (uint32 *)FLASH_BACKUP_ADDR;
	bootcode = (uint32 *)FLASH_BASE_ADDRESS;
	for(i=0; i<FLASH_SIZE; i=i+4){
		*bk_buffer = *bootcode;
		bk_buffer++;
		bootcode++;
	}
	bk_buffer = (uint32 *)FLASH_BACKUP_ADDR;
	bootcode = (uint32 *)FLASH_BASE_ADDRESS;
	for(i=0; i<FLASH_SIZE; i=i+4){
		if(*bk_buffer != *bootcode){
			printf("#Back up flash data error: bk_addr(0x%08x):0x%08x != flash_addr(0x%08x):0x%08x\n"\
				, bk_buffer, *bk_buffer, bootcode, *bootcode);
		}

		bk_buffer++;
		bootcode++;
	}

	/* protect off flash data. */
	flash_protect( FLAG_PROTECT_CLEAR, FLASH_BASE_ADDRESS, \
			FLASH_TOP_ADDRESS, addr2info(FLASH_BASE_ADDRESS));
	/* 1. Non Boot loader area, in case of unrecoverable operation. */
	retcode = _flash_test((FLASH_BASE_ADDRESS+JFFS2_START), \
			TEST_SIZE_PER_PATTREN, (FLASH_SIZE - JFFS2_START));
	if(retcode < 0){
		printf("No recover other data\n");
		goto no_recover;
	}
	/* 2. Boot loader area. */
	retcode = _flash_test((FLASH_BASE_ADDRESS), TEST_SIZE_PER_PATTREN, JFFS2_START);


	/* 
	 * 3. Copy back and verify data into the flash. 
	 */

	/* 3.1 Loader code */
	printf("Recover loader code:\n");
	flash_sect_erase( FLASH_BASE_ADDRESS, (FLASH_BASE_ADDRESS+JFFS2_START-1));
	flash_write((char *)FLASH_BACKUP_ADDR, FLASH_BASE_ADDRESS, JFFS2_START);

	/* 3.2 Other data */
	printf("Recover data other than loader code:\n");
	flash_sect_erase((FLASH_BASE_ADDRESS+JFFS2_START), FLASH_TOP_ADDRESS);
	flash_write((char *)(FLASH_BACKUP_ADDR+JFFS2_START), (FLASH_BASE_ADDRESS+JFFS2_START), (FLASH_SIZE-JFFS2_START));

	/* 3.3 Verify data */
	printf("Verify recovered data: ");
	bk_buffer = (uint32 *)FLASH_BACKUP_ADDR;
	bootcode = (uint32 *)FLASH_BASE_ADDRESS;
	for(i=0; i<FLASH_SIZE; i=i+4){
		if(*bk_buffer != *bootcode){
			printf("#Recover flash data error: bk_addr(0x%08x):0x%08x != flash_addr(0x%08x):0x%08x\n"\
				, bk_buffer, *bk_buffer, bootcode, *bootcode);
			goto no_recover;
		}

		bk_buffer++;
		bootcode++;
	}
	if(i==FLASH_SIZE)
		printf("Verify OK.\n");

no_recover:
	return retcode;
}
#else

#endif

