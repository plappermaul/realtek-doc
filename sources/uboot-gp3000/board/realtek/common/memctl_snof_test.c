/*
 * Include Files
 */
#include <common.h>
#include <linux/ctype.h>
#include <spi_flash.h>
#include <soc.h>
#define _cache_flush	(((soc_t *)(0x9f000020))->bios).dcache_writeback_invalidate_all

#define FLASHBASE CONFIG_SYS_FLASH_BASE
static unsigned long flash_size;

/* Definitions for memory test error handing manner */
#define MT_SUCCESS    (0)
#define MT_FAIL       (-1)

/* Definitions for memory test reset mode */
enum RESET_MODE{
	NO_RESET = 0,
	UBOOT_RESET = 1,
	WHOLE_CHIP_RESET = 2,
};

//The default seting of memory test error handling manner is non-blocking
//Using "Environment parameter" or "command flag setting" can change this
//"Environment parameter": setenv mt_freeze_block debug
//"command flag setting": mdram_test/mflash_test -b/-mt_block
static u32_t g_err_handle_block_mode=0;
static u32_t g_reset_flag=NO_RESET;

#define HANDLE_FAIL \
({ \
	printf("%s (%d) test failed.\n", __FUNCTION__,__LINE__);\
	if(!g_err_handle_block_mode){\
		return MT_FAIL;\
	}else{ \
		while(1);\
	} \
})

#define SIZE_1MB (0x100000)
#define SIZE_3MB (0x300000)
#define FLASH_TOP_ADDRESS(flash_size)	    (FLASHBASE+flash_size-1)
#define TEST_SIZE_PER_PATTREN	(0x10000)  //64KB
static u32_t src_data_addr;
#define	FLASH_BACKUP_ADDR	(flash_backup_addr)
#define	SRC_DATA_ADDR		(src_data_addr)
#define RSV_SPACE  			(0x300000)


/**************************
  * Command Parsing
  *************************/
typedef struct {
	u8_t test_loops;
	u8_t test_all_case;
} spif_cmd_parsing_info_t;


void spi_disable_message(void){};
void spi_enable_message(void){};

extern unsigned long simple_strtoul(const char *cp,char **endp,unsigned int base);


/*
 * Data Declaration
 */
DECLARE_GLOBAL_DATA_PTR;

const u32_t flash_patterns[] = {
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
int flash_sect_erase (ulong addr_first, ulong addr_last)
{
	u32_t sector=0, size = addr_last - addr_first;
	u32_t offset = addr_first-FLASHBASE;
	u32_t cs = offset/flash_size;
    struct spi_flash *sf = spi_flash_probe(0, cs, 0, 0);

	if(!sf) {
        printf("spi flash %d probe failed\n", cs);
        return 0;
    }
    printf("\r");
    if (size > CONFIG_ENV_SECT_SIZE) {
        sector = size / CONFIG_ENV_SECT_SIZE;
        if (size % CONFIG_ENV_SECT_SIZE)
            sector++;
    }

	return spi_flash_erase(sf, offset, sector * CONFIG_ENV_SECT_SIZE);
}

/*-----------------------------------------------------------------------
 * Copy memory to flash.
 * Make sure all target addresses are within Flash bounds,
 * and no protected sectors are hit.
 * Returns:
 * ERR_OK          0 - OK
 * ERR_TIMOUT      1 - write timeout
 * ERR_NOT_ERASED  2 - Flash not erased
 * ERR_PROTECTED   4 - target range includes protected sectors
 * ERR_INVAL       8 - target address not in Flash memory
 * ERR_ALIGN       16 - target address not aligned on boundary
 *			(only some targets require alignment)
 */
int
flash_write (char *src, ulong addr, ulong cnt)
{
	u32_t offset = (addr-FLASHBASE);
	u32_t cs = offset/flash_size;
	struct spi_flash *sf = spi_flash_probe(0, cs, 0, 0);
    if(!sf) {
        printf("spi flash %d probe failed\n", cs);
        return 0;
	}
    printf("\r");
	return spi_flash_write(sf, offset, cnt, src);
}

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
int flash_normal_patterns(u32_t flash_start_addr, u32_t test_size_per_pattern, u32_t flash_test_size)
{
	int i;
	int j;
	u32_t start_value;
	u32_t flash_start;
	volatile u32_t *src_start;

	printf("=======start %s test=======\n", __FUNCTION__);
	for (i=0; i < (sizeof(flash_patterns)/sizeof(u32_t)); i++)
	{
		_cache_flush();

		/* write pattern*/
		src_start = (u32_t *)SRC_DATA_ADDR;
		for(j=0; j < test_size_per_pattern; j=j+4)
		{
			*src_start = flash_patterns[i];
			src_start++;
		}

		_cache_flush();

		/* check data */
		src_start = (u32_t *)SRC_DATA_ADDR;
		for(j=0; j < test_size_per_pattern; j=j+4)
		{
			start_value = (*src_start);
			if(start_value != flash_patterns[i])
			{
				printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
				(u32_t)src_start , start_value, flash_patterns[i],  __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			src_start++;
		}
		printf("Flash:pattern[%d](0x%x) setting pass\r", i, flash_patterns[i]);

		src_start = (u32_t *)SRC_DATA_ADDR;
		flash_start = flash_start_addr + ((i*test_size_per_pattern)%flash_test_size);
		flash_sect_erase(flash_start, (flash_start+test_size_per_pattern-1));
		flash_write((char *)src_start, flash_start, test_size_per_pattern);

		/* check flash data sequentially. Uncached address */
		src_start = (u32_t *)(UADDR((u32_t)flash_start));
		for(j=0; j < test_size_per_pattern; j=j+4)
		{
			start_value = (*src_start);
			if(start_value != flash_patterns[i])
			{
				printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
				(u32_t)src_start , start_value, flash_patterns[i],  __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			src_start++;
		}

		/* check flash data interlevelingly. Uncached address */
		src_start = (u32_t *)(UADDR((u32_t)flash_start));
		for(j=0; j < (test_size_per_pattern/2); j=j+4)
		{
			start_value = (*src_start);
			if(start_value != flash_patterns[i])
			{
				printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
					(u32_t)src_start , start_value, flash_patterns[i],  __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}

			start_value = *(src_start + ((unsigned int)test_size_per_pattern/8));
			if(start_value != flash_patterns[i])
			{
				printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
				      (u32_t)(src_start + ((unsigned int)test_size_per_pattern/8)) , start_value, flash_patterns[i],  __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			src_start++;
		}


		/* check flash data sequentially. Cached address */
		src_start = (u32_t *)(CADDR((u32_t)flash_start));
		for(j=0; j < test_size_per_pattern; j=j+4)
		{
			start_value = (*src_start);
			if(start_value != flash_patterns[i])
			{
				printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
				(u32_t)src_start , start_value, flash_patterns[i],  __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			src_start++;
		}

		/* check flash data interlevelingly. Uncached address */
		src_start = (u32_t *)(CADDR((u32_t)flash_start));
		for(j=0; j < (test_size_per_pattern/2); j=j+4)
		{
			start_value = (*src_start);
			if(start_value != flash_patterns[i])
			{
				printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
					(u32_t)src_start , start_value, flash_patterns[i],  __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}

			start_value = *(src_start + ((unsigned int)test_size_per_pattern/8));
			if(start_value != flash_patterns[i])
			{
				printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
				      (u32_t)(src_start + ((unsigned int)test_size_per_pattern/8)) , start_value, flash_patterns[i],  __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			src_start++;
		}
		printf("Flash: pattern[%d](0x%x) 0x%x passed\r", i, flash_patterns[i], flash_start);
	}
	printf("=======%s test succeed.=======\n", __FUNCTION__);
	return MT_SUCCESS;
}

int flash_walking_of_1(u32_t flash_start_addr, u32_t test_size_per_pattern, u32_t flash_test_size)
{
	int i;
	int j;
	u32_t walk_pattern;
	u32_t start_value;
	u32_t flash_start;
	volatile u32_t *src_start;

	printf("=======start %s test=======\n", __FUNCTION__);
	for (i=0; i < 32; i++)
	{
		_cache_flush();

		/* generate pattern */
		walk_pattern = (1 << i);

		/* write pattern*/
		src_start = (u32_t *)SRC_DATA_ADDR;
		for(j=0; j < test_size_per_pattern; j=j+4)
		{
			*src_start = walk_pattern;
			src_start++;
		}

		_cache_flush();

		/* check data */
		src_start = (u32_t *)SRC_DATA_ADDR;
		for(j=0; j < test_size_per_pattern; j=j+4)
		{
			start_value = (*src_start);
			if(start_value != walk_pattern)
			{
				printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
				(u32_t)src_start , start_value, walk_pattern,  __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			src_start++;
		}
		printf("Flash: pattern[%d](0x%x) setting passed\r", i, walk_pattern);

		src_start = (u32_t *)SRC_DATA_ADDR;
		flash_start = flash_start_addr + ((i*test_size_per_pattern)%flash_test_size);
		flash_sect_erase(flash_start, (flash_start+test_size_per_pattern-1));
		flash_write((char *)src_start, flash_start, test_size_per_pattern);


		/* check data */
		src_start = (u32_t *)(UADDR((u32_t)flash_start));
		for(j=0; j < test_size_per_pattern; j=j+4)
		{
			start_value = (*src_start);
			if(start_value != walk_pattern)
			{
				printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
				(u32_t)src_start , start_value, walk_pattern,  __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			src_start++;
		}
		printf("Flash: pattern[%d](0x%x) 0x%x passed\r", i, walk_pattern, flash_start);
	}
	printf("=======%s test succeed.=======\n", __FUNCTION__);
	return MT_SUCCESS;
}

int flash_walking_of_0(u32_t flash_start_addr, u32_t test_size_per_pattern, u32_t flash_test_size)
{
	int i;
	int j;
	u32_t start_value;
	u32_t walk_pattern;
	u32_t flash_start;
	volatile u32_t *src_start;

	printf("=======start %s test=======\n", __FUNCTION__);
	for (i=0; i < 32; i++)
	{
		_cache_flush();

		/* generate pattern */
		walk_pattern = ~(1 << i);

		/* write pattern*/
		src_start = (u32_t *)SRC_DATA_ADDR;
		for(j=0; j < test_size_per_pattern; j=j+4)
		{
			*src_start = walk_pattern;
			src_start++;
		}

		_cache_flush();

		/* check data */
		src_start = (u32_t *)SRC_DATA_ADDR;
		for(j=0; j < test_size_per_pattern; j=j+4)
		{
			start_value = (*src_start);
			if(start_value != walk_pattern)
			{
				printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
				(u32_t)src_start , start_value, walk_pattern,  __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			src_start++;
		}
		printf("Flash:pattern[%d](0x%x) setting passed\r", i, walk_pattern);

		src_start = (u32_t *)SRC_DATA_ADDR;
		flash_start = flash_start_addr + ((i*test_size_per_pattern)%flash_test_size);
		flash_sect_erase(flash_start, (flash_start+test_size_per_pattern-1));
		flash_write((char *)src_start, flash_start, test_size_per_pattern);

		/* check data */
		src_start = (u32_t *)(UADDR((u32_t)flash_start));
		for(j=0; j < test_size_per_pattern; j=j+4)
		{
			start_value = (*src_start);
			if(start_value != walk_pattern)
			{
				printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
				(u32_t)src_start , start_value, walk_pattern,  __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			src_start++;
		}
		printf("Flash:pattern[%d](0x%x) 0x%x passed\r", i, walk_pattern, flash_start);
	}
	printf("=======%s test succeed.=======\n", __FUNCTION__);
	return MT_SUCCESS;
}

int flash_addr_rot(u32_t flash_start_addr, u32_t test_size_per_pattern, u32_t flash_test_size)
{
	int i;
	int j;
	u32_t start_value;
	u32_t flash_start;
	volatile u32_t *_dram_start;
	volatile u32_t *src_start;

	printf("=======start %s test=======\n", __FUNCTION__);
	for (i=0; i < 32; i=i+4)
	{
		_cache_flush();

		/* write pattern*/
		src_start = (u32_t *)SRC_DATA_ADDR;
		for(j=0; j < test_size_per_pattern; j=j+4)
		{
			*src_start = ((u32_t)src_start << i);
			src_start++;
		}

		_cache_flush();

		/* check data */
		src_start = (u32_t *)SRC_DATA_ADDR;
		for(j=0; j < test_size_per_pattern; j=j+4)
		{
			start_value = (*src_start);
			if(start_value != (((u32_t)src_start) << i))
			{
				printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
				      (u32_t)src_start , start_value, (((u32_t)src_start) << i),  __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			src_start++;
		}
		printf("Flash: rotate %d setting passed\r", i);


		src_start = (u32_t *)SRC_DATA_ADDR;
		flash_start = flash_start_addr + ((i*test_size_per_pattern)%flash_test_size);
		flash_sect_erase(flash_start, (flash_start+test_size_per_pattern-1));
		flash_write((char *)src_start, flash_start, test_size_per_pattern);

		/* check data */
		_dram_start = (u32_t *) SRC_DATA_ADDR;
		src_start = (u32_t *)(UADDR((u32_t)flash_start));
		for(j=0; j < test_size_per_pattern; j=j+4)
		{
			start_value = (*src_start);
			if(start_value != (((u32_t)_dram_start) << i))
			{
				printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
				(u32_t)src_start , start_value, (((u32_t)_dram_start) << i),  __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			src_start++;
			_dram_start++;
		}
		printf("Flash: rotate %d 0x%x passed\r", i, flash_start);
	}
	printf("=======%s test succeed.=======\n", __FUNCTION__);
	return MT_SUCCESS;
}

int flash_com_addr_rot(u32_t flash_start_addr, u32_t test_size_per_pattern, u32_t flash_test_size)
{
	int i;
	int j;
	u32_t start_value;
	u32_t flash_start;
	volatile u32_t *_dram_start;
	volatile u32_t *src_start;

	printf("=======start %s test=======\n", __FUNCTION__);
	for (i=0; i < 32; i=i+4)
	{
		_cache_flush();

		/* write pattern*/
		src_start = (u32_t *)SRC_DATA_ADDR;
		for(j=0; j < test_size_per_pattern; j=j+4)
		{
			*src_start = ~(((u32_t)src_start) << i);
			src_start++;
		}

		_cache_flush();

		/* check data */
		src_start = (u32_t *)SRC_DATA_ADDR;
		for(j=0; j < test_size_per_pattern; j=j+4)
		{
			start_value = (*src_start);
			if(start_value != (~((u32_t)src_start << i)))
			{
				 printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
				        (u32_t)src_start , start_value, ~(((u32_t)src_start) << i),  __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			src_start++;
		}
		printf("Flash: ~rotate %d setting passed\r", i);

		_cache_flush();

		src_start = (u32_t *)SRC_DATA_ADDR;
		flash_start = flash_start_addr + ((i*test_size_per_pattern)%flash_test_size);
		flash_sect_erase(flash_start, (flash_start+test_size_per_pattern-1));
		flash_write((char *)src_start, flash_start, test_size_per_pattern);

		/* check data */
		_dram_start = (u32_t *) SRC_DATA_ADDR;
		src_start = (u32_t *)(UADDR((u32_t)flash_start));
		for(j=0; j < test_size_per_pattern; j=j+4)
		{
			start_value = (*src_start);
			if(start_value != (~((u32_t)_dram_start << i)))
			{
				printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
				(u32_t)src_start , start_value, ~(((u32_t)_dram_start) << i),  __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			src_start++;
			_dram_start++;
		}
		printf("Flash: ~rotate %d 0x%x passed\r", i, flash_start);
	}
	printf("=======%s test succeed.=======\n", __FUNCTION__);
	return MT_SUCCESS;
}

int _flash_test(u32_t flash_start_addr, u32_t test_size_per_pattern, u32_t flash_test_size, u8_t test_all_case)
{
	/* partial range */
	if(MT_SUCCESS != flash_normal_patterns( flash_start_addr, test_size_per_pattern, flash_test_size)){
		HANDLE_FAIL;
	}
    if(test_all_case) {
    	if(MT_SUCCESS != flash_walking_of_1( flash_start_addr, test_size_per_pattern, flash_test_size)){
    		HANDLE_FAIL;
    	}
    	if(MT_SUCCESS != flash_walking_of_0( flash_start_addr, test_size_per_pattern, flash_test_size)){
    		HANDLE_FAIL;
    	}
    	if(MT_SUCCESS != flash_addr_rot( flash_start_addr, test_size_per_pattern, flash_test_size)){
    		HANDLE_FAIL;
    	}
    	if(MT_SUCCESS != flash_com_addr_rot( flash_start_addr, flash_test_size, flash_test_size)){
    		HANDLE_FAIL;
    	}
    }
	return MT_SUCCESS;
}

int spif_cmd_parsing(int argc, char * const argv[], spif_cmd_parsing_info_t *info)
{
	u32_t i;

	#define ILL_CMD \
	({ \
		printf("ERR: Illegal command (%d).\n",__LINE__);\
		return MT_FAIL;\
	})

	/* Initialize the memory test parameters..... */
	g_err_handle_block_mode = 0;
	info->test_loops = 1;
	info->test_all_case = 0;

	/* Parse the environment parameter for mt (non-)blocking error mode */
	//g_err_handle_block_mode = env_get("mt_block_e");

	/* Parse command flag for test range / test loops / mt error (non-)blocking mode */
	for(i=1 ; i<argc ;)	{
		if('-' != *argv[i])	ILL_CMD;

		if((strcmp(argv[i],"-loops") == 0) || (strcmp(argv[i],"-l") == 0)){
			if(((i+1) >= argc) || (isxdigit(*argv[i+1])==0)) ILL_CMD;
			info->test_loops = simple_strtoul(argv[i+1], NULL, 10);
			info->test_loops = (info->test_loops==0)?1:(info->test_loops);
			i = i+2;
        }else if((strcmp (argv[i], "-all_case") == 0) || (strcmp (argv[i], "-ac") == 0)){
            info->test_all_case = 1;
            i = i+1;
		}else if((strcmp (argv[i], "-block_e") == 0) || (strcmp (argv[i], "-b") == 0)){
            g_err_handle_block_mode = 1;
            i = i+1;
		}else if(strcmp(argv[i],"-reset") == 0){
			g_reset_flag = UBOOT_RESET;
			i = i+1;
		}else if(strcmp(argv[i],"-reset_all") == 0){
			g_reset_flag = WHOLE_CHIP_RESET;
			i = i+1;
		}else{
			ILL_CMD;
		}
	}
	return MT_SUCCESS;
}

int do_snof_test(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	volatile u32_t *bootcode;
	volatile u32_t *bk_buffer;
	int retcode=MT_SUCCESS;
	u32_t i;
	u32_t flash_backup_addr;
	spif_cmd_parsing_info_t cmd_info;

	if(MT_FAIL == spif_cmd_parsing(argc, argv, &cmd_info))
		goto no_recover;

    u32_t cs = 0;
    struct spi_flash *sf = spi_flash_probe(0, cs, 0, 0);

	flash_size = sf->size;

	if((gd->ram_size-(CONFIG_SYS_TEXT_BASE&0x1FFFFFFF)) > (flash_size+TEST_SIZE_PER_PATTREN+RSV_SPACE)){
		flash_backup_addr = CONFIG_SYS_TEXT_BASE+RSV_SPACE;
	}else if((CONFIG_SYS_TEXT_BASE&0x1FFFFFFF) > (flash_size+TEST_SIZE_PER_PATTREN+RSV_SPACE)){
	    flash_backup_addr = CONFIG_SYS_TEXT_BASE-flash_size-TEST_SIZE_PER_PATTREN;
	}else{
		printf("[Error] No enough space to back up the SPI-Flash data:\n");
		printf("uBoot_Base=0x%x, DRAM_Size=%dMB\n",CONFIG_SYS_TEXT_BASE,(gd->ram_size/SIZE_1MB));
		return MT_FAIL;
	}
	src_data_addr = flash_backup_addr+flash_size;

    printf("flash size = %ldMB\n", (flash_size/SIZE_1MB));
	printf("FLASH_BACKUP_ADDR: 0x%x\n",flash_backup_addr);
	printf("SRC_DATA_ADDR: 0x%x\n",src_data_addr);


	/* 0. Back up and verify whole flash data. */
	bk_buffer = (u32_t *)FLASH_BACKUP_ADDR;
	bootcode = (u32_t *)FLASHBASE;
	printf("Backing up %ldMB flash data: (0x%08x -> 0x%08x)... ",(flash_size/SIZE_1MB),(u32_t)bootcode,(u32_t)bk_buffer);
	for(i=0; i<flash_size; i=i+4){
		*bk_buffer = *bootcode;
		bk_buffer++;
		bootcode++;
	}
	bk_buffer = (u32_t *)FLASH_BACKUP_ADDR;
	bootcode = (u32_t *)FLASHBASE;
	for(i=0; i<flash_size; i=i+4){
		if(*bk_buffer != *bootcode){
			printf("#Back up flash data error: bk_addr(0x%08x):0x%08x != flash_addr(0x%08x):0x%08x\n",\
			(u32_t)bk_buffer, *bk_buffer, (u32_t)bootcode, *bootcode);
        	goto no_recover;
		}
		bk_buffer++;
		bootcode++;
	}
	puts("done\n");
    spi_disable_message();

	for(i=0;i<cmd_info.test_loops;i++){
		/* 1. Non Boot loader area, in case of unrecoverable operation. */
		if(MT_FAIL == _flash_test((FLASHBASE+RSV_SPACE),TEST_SIZE_PER_PATTREN,(flash_size-RSV_SPACE),cmd_info.test_all_case)){
			printf("[ERROR] Non Boot loader area! No recover other data\n");
			goto no_recover;
		}

		/* 2. Boot loader area. */
        if(MT_FAIL == _flash_test((FLASHBASE), TEST_SIZE_PER_PATTREN, RSV_SPACE, cmd_info.test_all_case)){
			printf("[ERROR] Boot loader area! No recover other data\n");
			goto no_recover;
        }
		printf("== %d runs mflash_test==\n\n",i);
	}//for(i=0;i<cmd_info.test_loops;i++){

	spi_enable_message();

	/*
	 * 3. Copy back and verify data into the flash.
	 */
	/* 3.1 Loader code */
	printf("Recover flash loader data (0x%08x -> 0x%08x) ... \n", (u32_t)FLASH_BACKUP_ADDR,(u32_t)FLASHBASE);
	flash_sect_erase(FLASHBASE, (FLASHBASE+RSV_SPACE-1));
	flash_write((char *)FLASH_BACKUP_ADDR, FLASHBASE, RSV_SPACE);

	/* 3.2 Other data */
	printf("Recover data other than loader code:\n");
	flash_sect_erase((FLASHBASE+RSV_SPACE), FLASH_TOP_ADDRESS(flash_size));
	flash_write((char *)(FLASH_BACKUP_ADDR+RSV_SPACE), (FLASHBASE+RSV_SPACE), (flash_size-RSV_SPACE));

	/* 3.3 Verify data */
	printf("Verify recovered data: ");
	bk_buffer = (u32_t *)FLASH_BACKUP_ADDR;
	bootcode = (u32_t *)FLASHBASE;
	for(i=0; i<flash_size; i=i+4){
		if(*bk_buffer != *bootcode){
		printf("#Recover flash data error: bk_addr(0x%08x):0x%08x != flash_addr(0x%08x):0x%08x\n",\
			(u32_t)bk_buffer, *bk_buffer, (u32_t)bootcode, *bootcode);
			goto no_recover;
		}
		bk_buffer++;
		bootcode++;
	}
	if(i==flash_size){
		printf("Verify OK.\n");
	}
	goto restore_setting;

no_recover:
	retcode = MT_FAIL;
restore_setting:
    spi_enable_message();

	if(MT_SUCCESS== retcode){
		/* Reset if the command is sent from the command line */
		if(UBOOT_RESET == g_reset_flag){
			do_reset (NULL, 0, 0, NULL);
		}else if(WHOLE_CHIP_RESET == g_reset_flag){
            //WDOG whole chip reset
            REG32(0xb8003268) = 0;
            REG32(0xb8003268) = 0x80000000;
		}
	}
	return retcode;
}

U_BOOT_CMD(
    mflash_test, 10, 1, do_snof_test,
    "mflash_test  - do spi-nor flash test. ",
    "\n"
);

