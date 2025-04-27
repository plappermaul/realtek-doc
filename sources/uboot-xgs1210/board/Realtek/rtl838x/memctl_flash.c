/*
 * Include Files
 */
#include <common.h>
#include <linux/ctype.h>
#include <spi_flash.h>
#include <soc.h>
#include <pblr.h>
#include "flash_spi.h"
#include "memctl_err_handle.h"

extern void spi_flash_init(void);
extern unsigned int spi_flash_sectorsize(void);

static unsigned long flash_size;
/* Definitions for memory test reset mode */
enum RESET_MODE{
	NO_RESET = 0,	
	UBOOT_RESET = 1,	
	WHOLE_CHIP_RESET = 2,
};
static u32_t g_err_handle_block_mode=0; 
static u32_t g_reset_flag=NO_RESET;
static u32_t src_data_addr;
static struct spi_flash *sf;
extern ulong uboot_end;
#define	FLASH_BASE_ADDRESS	CONFIG_SYS_FLASH_BASE
#define _cache_flush	pblr_dc_flushall
#define	FLASH_TOP_ADDRESS	    (FLASH_BASE_ADDRESS+flash_size-1)
#define	TEST_SIZE_PER_PATTREN	(0x10000)  //64KB
static unsigned int src_data_addr,reserved_size;
#define	FLASH_BACKUP_ADDR	(flash_backup_addr)
#define	SRC_DATA_ADDR		(src_data_addr)
#define RESERVED_UBOOT_DRAM_SIZE ((((unsigned int)&uboot_end-CONFIG_SYS_TEXT_BASE) & ~(0x100000-1))+0x100000)
#define TEST_SPACE			(flash_size+TEST_SIZE_PER_PATTREN+RESERVED_UBOOT_DRAM_SIZE)
#define SIZE_1MB (0x100000)
#define SIZE_2MB (0x200000)
#define SIZE_3MB (0x300000)
static u32_t flash_backup_addr;
#define RSV_SPACE  		SIZE_3MB
u32_t assign_test_range = 0;
/**************************
  * Command Parsing
  *************************/ 
typedef struct {
	u8_t test_loops;
    u32_t mmio_addr;    //0xB4xxxxxx or 0xBDxxxxxx
    u32_t mt_size; //Unit is byte
} spif_cmd_parsing_info_t;


extern unsigned long simple_strtoul(const char *cp,char **endp,unsigned int base);

/*
 * Data Declaration
 */
DECLARE_GLOBAL_DATA_PTR;

static u32_t bootcode_size;  
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
	u32_t sector_size,sector=0, size = addr_last - addr_first;
	
	if(!sf) {
        sf = spi_flash_probe(0, 0, 0, 0);
	    if(!sf) {
	        printf("spi flash probe failed\n");
	        return 0;
	    }
	}
	
		sector_size = spi_flash_sectorsize();
		
    if (size > sector_size) {
        sector = size / sector_size;
        if (size % sector_size)
            sector++;
    }    
    else
    	sector=1;
    
	return spi_flash_erase(sf, (addr_first-FLASH_BASE_ADDRESS), sector * sector_size);
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
	if(!sf) {
        sf = spi_flash_probe(0, 0, 0, 0);
        if(!sf) {
            printf("spi flash probe failed\n");
            return 0;
        }
    }
	return spi_flash_write(sf, (addr-FLASH_BASE_ADDRESS), cnt, src);
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
		src_start = (u32_t *)src_data_addr;
		for(j=0; j < test_size_per_pattern; j=j+4)
		{
			*src_start = flash_patterns[i];
			src_start++;
		}
		
		_cache_flush();
		
		/* check data */
		src_start = (u32_t *)src_data_addr;
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
		printf("Flash:pattern[%d](0x%x) setting pass\n", i, flash_patterns[i]);
		
		src_start = (u32_t *)src_data_addr;
		flash_start = flash_start_addr + ((i*test_size_per_pattern)%flash_test_size);
		flash_sect_erase(flash_start, (flash_start+test_size_per_pattern-1));
		flash_write((char *)src_start, flash_start, test_size_per_pattern);
		
		/* check flash data sequentially. Uncached address */
		src_start = (u32_t *)(TO_UNCACHED_ADDR((u32_t)flash_start));
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
		src_start = (u32_t *)(TO_UNCACHED_ADDR((u32_t)flash_start));
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
		src_start = (u32_t *)(TO_CACHED_ADDR((u32_t)flash_start));
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
		src_start = (u32_t *)(TO_CACHED_ADDR((u32_t)flash_start));
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

		printf("Flash: pattern[%d](0x%x) 0x%x pass\n", i, flash_patterns[i], flash_start);
		printf("pattern[%d](0x%x) completed\n", i, flash_patterns[i]);
	}
	printf("%s test succeed.\n", __FUNCTION__);
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
		src_start = (u32_t *)src_data_addr;
		for(j=0; j < test_size_per_pattern; j=j+4)
		{
			*src_start = walk_pattern;
			src_start++;
		}
		
		_cache_flush();
		
		/* check data */  
		src_start = (u32_t *)src_data_addr;
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
		printf("Flash: pattern[%d](0x%x) setting passed\n", i, walk_pattern);

		src_start = (u32_t *)src_data_addr;
		flash_start = flash_start_addr + ((i*test_size_per_pattern)%flash_test_size);
		flash_sect_erase(flash_start, (flash_start+test_size_per_pattern-1));
		flash_write((char *)src_start, flash_start, test_size_per_pattern);


		/* check data */  
		src_start = (u32_t *)(TO_UNCACHED_ADDR((u32_t)flash_start));
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
		printf("Flash: pattern[%d](0x%x) 0x%x passed\n", i, walk_pattern, flash_start);
		printf("pattern[%d](0x%x) completed\n", i, walk_pattern);
	}
	printf("%s test succeed.\n", __FUNCTION__);
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
		src_start = (u32_t *)src_data_addr;
		for(j=0; j < test_size_per_pattern; j=j+4)
		{
			*src_start = walk_pattern;
			src_start++;
		}
		
		_cache_flush();
		
		/* check data */  
		src_start = (u32_t *)src_data_addr;
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
		printf("Flash:pattern[%d](0x%x) setting passed\n", i, walk_pattern);
		
		src_start = (u32_t *)src_data_addr;
		flash_start = flash_start_addr + ((i*test_size_per_pattern)%flash_test_size);
		flash_sect_erase(flash_start, (flash_start+test_size_per_pattern-1));
		flash_write((char *)src_start, flash_start, test_size_per_pattern);

		/* check data */  
		src_start = (u32_t *)(TO_UNCACHED_ADDR((u32_t)flash_start));
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
		printf("Flash:pattern[%d](0x%x) 0x%x passed\n", i, walk_pattern, flash_start);
		printf("pattern[%d](0x%x) completed\n", i, walk_pattern);
	}
	printf("%s test succeed.\n", __FUNCTION__);
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
		src_start = (u32_t *)src_data_addr;
		for(j=0; j < test_size_per_pattern; j=j+4)
		{
			*src_start = ((u32_t)src_start << i);
			src_start++;
		}
		
		_cache_flush();
		
		/* check data */  
		src_start = (u32_t *)src_data_addr;
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
		printf("Flash: rotate %d setting passed\n", i);
		

		src_start = (u32_t *)src_data_addr;
		flash_start = flash_start_addr + ((i*test_size_per_pattern)%flash_test_size);
		flash_sect_erase(flash_start, (flash_start+test_size_per_pattern-1));
		flash_write((char *)src_start, flash_start, test_size_per_pattern);

		/* check data */  
		_dram_start = (u32_t *) src_data_addr;
		src_start = (u32_t *)(TO_UNCACHED_ADDR((u32_t)flash_start));
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
		printf("Flash: rotate %d 0x%x passed\n", i, flash_start);
		
		printf("rotate %d completed\n", i);
	}
	printf("%s test succeed.\n", __FUNCTION__);
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
		src_start = (u32_t *)src_data_addr;
		for(j=0; j < test_size_per_pattern; j=j+4)
		{
			*src_start = ~(((u32_t)src_start) << i);
			src_start++;
		}
		
		_cache_flush();
		
		/* check data */  
		src_start = (u32_t *)src_data_addr;
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
		printf("Flash: ~rotate %d setting passed\n", i);
		
		_cache_flush();
		
		src_start = (u32_t *)src_data_addr;
		flash_start = flash_start_addr + ((i*test_size_per_pattern)%flash_test_size);
		flash_sect_erase(flash_start, (flash_start+test_size_per_pattern-1));
		flash_write((char *)src_start, flash_start, test_size_per_pattern);
		
		/* check data */  
		_dram_start = (u32_t *) src_data_addr;
		src_start = (u32_t *)(TO_UNCACHED_ADDR((u32_t)flash_start));
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
		printf("Flash: ~rotate %d 0x%x passed\n", i, flash_start);
		
		printf("~rotate %d completed\n", i);
	}
	printf("%s test succeed.\n", __FUNCTION__);
	return MT_SUCCESS;
}

int _flash_test(u32_t flash_start_addr, u32_t test_size_per_pattern, u32_t flash_test_size)
{
	/* partial range */
	if(MT_SUCCESS != flash_normal_patterns( flash_start_addr, test_size_per_pattern, flash_test_size)){
		HANDLE_FAIL;
	}
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
	info->test_loops    = 1;

	/* Parse the environment parameter for mt (non-)blocking error mode */
	g_err_handle_block_mode = getenv_ulong("mt_block_e", 10, 0);

	/* Parse command flag for test range / test loops / mt error (non-)blocking mode */
	for(i=1 ; i<argc ;)	{
		if('-' != *argv[i])	ILL_CMD;

		if((strcmp(argv[i],"-loops") == 0) || (strcmp(argv[i],"-l") == 0)){
			if(((i+1) >= argc) || (isxdigit(*argv[i+1])==0))ILL_CMD;
			info->test_loops = simple_strtoul(argv[i+1], NULL, 10);
			info->test_loops = (info->test_loops==0)?1:(info->test_loops);
			i = i+2;
                }else if((strcmp (argv[i], "-block_e") == 0) || (strcmp (argv[i], "-b") == 0)){
                        g_err_handle_block_mode = 1;
                        i = i+1;
		}else if(strcmp(argv[i],"-reset") == 0){
			g_reset_flag = UBOOT_RESET;
			i = i+1;
		}else if(strcmp(argv[i],"-reset_all") == 0){
			g_reset_flag = WHOLE_CHIP_RESET;
			i = i+1;
        }else if((strcmp (argv[i], "-range") == 0) || (strcmp (argv[i], "-r") == 0)){
			if(((i+1) >= argc) || (isxdigit(*argv[i+1])==0) || ((i+2) >= argc) || (isxdigit(*argv[i+2])==0))ILL_CMD;
            info->mmio_addr = simple_strtoul(argv[i+1], NULL, 10);
            info->mmio_addr = CADDR(info->mmio_addr);
            info->mt_size= simple_strtoul(argv[i+2], NULL, 10);
			i = i+3;
            assign_test_range =1;
		}else{
			ILL_CMD;
		}
	}
	return MT_SUCCESS;
}

int back_up_spif_data(u32_t flash_addr_to_be_backup, u32_t backup_size)
{
	volatile u32_t *bootcode;
	volatile u32_t *bk_buffer;
    u32_t i;

    bk_buffer = (u32_t *)FLASH_BACKUP_ADDR;
    bootcode = (u32_t *)flash_addr_to_be_backup;
    printf("Backing up %dMB flash data: (0x%08x -> 0x%08x)... ",(backup_size/SIZE_1MB),(u32_t)bootcode,(u32_t)bk_buffer);
    for(i=0; i<backup_size; i=i+4){
        *bk_buffer = *bootcode;
        bk_buffer++;
        bootcode++;
    }
    
    bk_buffer = (u32_t *)FLASH_BACKUP_ADDR;
    bootcode = (u32_t *)flash_addr_to_be_backup;
    for(i=0; i<backup_size; i=i+4){
        if(*bk_buffer != *bootcode){
            printf("#Back up flash data error: bk_addr(0x%08x):0x%08x != flash_addr(0x%08x):0x%08x\n",\
            (u32_t)bk_buffer, *bk_buffer, (u32_t)bootcode, *bootcode);
            return MT_FAIL;            
        }
        bk_buffer++;
        bootcode++;
    }
    puts("done\n");
    return MT_SUCCESS;
}

int restore_spif_data(u32_t flash_addr_to_be_backup, u32_t backup_size)
{
	volatile u32_t *bootcode;
	volatile u32_t *bk_buffer;
    int i;
    ulong addr = (ulong)flash_addr_to_be_backup;
    ulong size = (ulong)backup_size;

    printf("Recover %dMB flash data (0x%08x -> 0x%08x) ... ", (backup_size/SIZE_1MB), (u32_t)FLASH_BACKUP_ADDR,(u32_t)addr);
    flash_sect_erase(addr, (addr+size-1));
    flash_write((char *)FLASH_BACKUP_ADDR, addr, size);
       
    bk_buffer = (u32_t *)FLASH_BACKUP_ADDR;
    bootcode = (u32_t *)addr;
    for(i=0; i<backup_size; i=i+4){
        if(*bk_buffer != *bootcode){
        printf("#Recover flash data error: bk_addr(0x%08x):0x%08x != flash_addr(0x%08x):0x%08x\n",\
            (u32_t)bk_buffer, *bk_buffer, (u32_t)bootcode, *bootcode);
            return MT_FAIL;
        }
        bk_buffer++;
        bootcode++;
    }
    if(i == backup_size){
        puts("Verify OK.\n");
        return MT_SUCCESS;
    }
    return MT_FAIL;
}

int _common_flash_test(u32_t flash_addr_to_be_test, u32_t test_size)
{
    if(MT_FAIL == back_up_spif_data(flash_addr_to_be_test, test_size)) return MT_FAIL;
//    spi_disable_message(); 
    if(MT_FAIL == _flash_test(flash_addr_to_be_test, TEST_SIZE_PER_PATTREN, test_size)){
//        spi_enable_message();
        return MT_FAIL;
    }

    if(MT_FAIL == restore_spif_data(flash_addr_to_be_test, test_size)){
        printf("[ERROR] No recover data\n");
        return MT_FAIL;        
    }

//    spi_enable_message();
    return MT_SUCCESS;
}

int memctl_flash_test(u32_t offset, u32_t size, int testLoops)
{
	volatile u32_t *bootcode;
	volatile u32_t *bk_buffer;
	int32 retcode = 0;
	u32_t i;
  u32_t l, flash_backup_addr,bootcode_end;
  u32_t test_addr,test_size,sector_size,temp,tmp_addr;

	test_addr = FLASH_BASE_ADDRESS + offset;

	if(parameters.flash_init_result == INI_RES_UNINIT) {
		spi_flash_init();
    }
	flash_size = 1 << para_flash_info.size_per_chip;
    
	if(!sf) {
        sf = spi_flash_probe(0, 0, 0, 0);
        if(!sf) {
            printf("spi flash probe failed\n");
            return 0;
        }
    }
		test_size = size;    

		sector_size = spi_flash_sectorsize();
		
		test_size += (test_addr & (sector_size-1));
		test_addr = test_addr & (~(sector_size-1));
		test_size = (((test_size-1)/sector_size)+1)*sector_size;
		
		if (test_addr > (FLASH_BASE_ADDRESS+flash_size) || (test_addr+test_size) > (FLASH_BASE_ADDRESS+flash_size))
		{
			printf("\n argument error, flash size = 0x%x \n",(u32_t)flash_size);
		}        
		bootcode_size = CONFIG_ENV_OFFSET + ((((CONFIG_ENV_SECT_SIZE-1)/sector_size)+1) * sector_size);
		bootcode_end = FLASH_BASE_ADDRESS + bootcode_size;
		reserved_size = RESERVED_UBOOT_DRAM_SIZE;
		
    if ((FLASH_TOP_ADDRESS-CONFIG_SYS_TEXT_BASE) >= TEST_SPACE) {
  		flash_backup_addr = CONFIG_SYS_TEXT_BASE+reserved_size;
        src_data_addr = flash_backup_addr + TEST_SPACE;
    } else if((UBOOT_RESERVED_START-CONFIG_SYS_TEXT_BASE) >= TEST_SPACE) {
	    flash_backup_addr = UBOOT_RESERVED_START-TEST_SPACE;
        src_data_addr = flash_backup_addr - TEST_SPACE;
    } else {
		printf("[Error] No enough space for backup Flash data.\n");
        return 0;
    }
        

	/* 0. Back up and verify whole flash data. */
	bk_buffer = (u32_t *)flash_backup_addr;
	bootcode = (u32_t *)test_addr;
   	printf("Back up flash data (0x%08x -> 0x%08x) ... ", (u32_t)bootcode, (u32_t)bk_buffer);
	for(i=0; i<test_size; i=i+4){
		*bk_buffer = *bootcode;
		bk_buffer++;
		bootcode++;
	}
	bk_buffer = (u32_t *)flash_backup_addr;
	bootcode = (u32_t *)test_addr;
	for(i=0; i<test_size; i=i+4){
		if(*bk_buffer != *bootcode){
			printf("failed\n#Back up flash data error: bk_addr(0x%08x):0x%08x != flash_addr(0x%08x):0x%08x\n"
				, (u32_t)bk_buffer, *bk_buffer, (u32_t)bootcode, *bootcode);
		}

		bk_buffer++;
		bootcode++;
	}
	puts("done\n");
    
	/* protect off flash data. */
	for(l=1; l<=testLoops; l++) {   
    if(testLoops>1) printf("[Round %d]\n", l);
		/* 1. Non Boot loader area, in case of unrecoverable operation. */
		if ((test_addr+test_size) > bootcode_end)
		{
			if (test_addr > bootcode_end)
			{
				tmp_addr = test_addr;
				temp = test_size;
			}
			else
			{
				tmp_addr = bootcode_end;
				temp = test_size-(bootcode_end-test_addr);
			}	
			retcode = _flash_test(tmp_addr, \
				TEST_SIZE_PER_PATTREN, temp);
			if(retcode < 0){
				printf("flash_test Failed-1.(0x%08x -> 0x%08x) ... ", (u32_t)tmp_addr, (u32_t)temp);
				goto no_recover;
			}
		}
		/* 2. Boot loader area. */
		if ((test_addr) < bootcode_end)
		{
			if ((test_addr+test_size) > bootcode_end)
				temp = (bootcode_end-test_addr);
			else
			{
				temp = test_size-test_addr;
			}	
			retcode = _flash_test((test_addr), TEST_SIZE_PER_PATTREN, temp);
			if(retcode < 0){
				printf("flash_test Failed-2.(0x%08x -> 0x%08x) ... ", (u32_t)test_addr, (u32_t)temp);
				goto no_recover;
			}
		}
    puts("\n");
	}

	/* 
	 * 3. Copy back and verify data into the flash. 
	 */

	/* 3.1 code */
	printf("\nRecover flash data (0x%08x -> 0x%08x) ... ", (u32_t)flash_backup_addr, (u32_t)test_addr);
	flash_sect_erase(test_addr, test_addr+(test_size-1));
	flash_write((char *)flash_backup_addr, test_addr, test_size);


	/* 3.3 Verify data */
	printf("done.\nVerify recovered data ... ");
	bk_buffer = (u32_t *)flash_backup_addr;
	bootcode = (u32_t *)test_addr;
	for(i=0; i<test_size; i=i+4){
		if(*bk_buffer != *bootcode){
			printf("failed: bk_addr(0x%08x):0x%08x != flash_addr(0x%08x):0x%08x\n"\
				, (u32_t)bk_buffer, *bk_buffer, (u32_t)bootcode, *bootcode);
			goto no_recover;
		}

		bk_buffer++;
		bootcode++;
	}
	if(i==test_size)
		printf("done.\n");

no_recover:
	return retcode;

}

int flash_test(int flag, int argc, char * const argv[])
{
	int retcode=MT_SUCCESS;
	u32_t i;
	spif_cmd_parsing_info_t cmd_info;
        u32_t mt_size;
        u32_t mt_start;
        u32_t backup_size;

	if(MT_FAIL == spif_cmd_parsing(argc, argv, &cmd_info)) return MT_FAIL;

	if(parameters.flash_init_result == INI_RES_UNINIT){
		spi_flash_init();
    }
	flash_size = 1 << para_flash_info.size_per_chip;
    printf("[mflash_test]: flash_size is %2dMB\n",(int)(flash_size/SIZE_1MB));

    if(assign_test_range){
        mt_size = cmd_info.mt_size;
        mt_start= cmd_info.mmio_addr;
        backup_size = mt_size;
        printf("               backup_size is %2d MB\n",backup_size/SIZE_1MB);
        printf("               test_size   is %2d MB\n",mt_size/SIZE_1MB);

        if((initdram(0)-(CONFIG_SYS_TEXT_BASE&0x1FFFFFFF)) > (mt_size+TEST_SIZE_PER_PATTREN+RSV_SPACE)){
            flash_backup_addr = CONFIG_SYS_TEXT_BASE+RSV_SPACE;
        }else if((CONFIG_SYS_TEXT_BASE&0x1FFFFFFF) > (mt_size+TEST_SIZE_PER_PATTREN+RSV_SPACE)){
            flash_backup_addr = CONFIG_SYS_TEXT_BASE-mt_size-TEST_SIZE_PER_PATTREN-RSV_SPACE;
        }else if((mt_size == flash_size) && (mt_size >= initdram(0))){
            backup_size = 16*1024*1024;
            printf(" Warning: Assigned test size is %d MB, DRAM_Size is %d MB => Only backup 16MB flash content\n", (mt_size/SIZE_1MB), (initdram(0)/SIZE_1MB)); 
        }else{
            printf("[Error] No enough space to back up the SPI-Flash data:\n");    
            return MT_FAIL;
        }
        src_data_addr = flash_backup_addr + backup_size;


        /* Step1: Back up and verify the assigned spif area data. */
        back_up_spif_data(mt_start, backup_size);   

//        spi_disable_message();
        for(i=0;i<cmd_info.test_loops;i++){
            /* Step2: Do mflash_test */
            if(MT_FAIL == _flash_test(mt_start, TEST_SIZE_PER_PATTREN, mt_size)){
                printf("[ERROR] No recover other data over the Assigned Area!\n");
//                spi_enable_message();
                return MT_FAIL;
            }
            printf("\n[mflash_test] %d runs OK\n",i);       
        }

        /* Step3: Copy back and verify data into the flash. */
        retcode = restore_spif_data(mt_start, backup_size);
        if(MT_SUCCESS == retcode) goto restore_setting;          

    }else{ //(!assign_test_range)
        u32_t flash_size_in_MB = flash_size/SIZE_1MB;
        mt_size = SIZE_2MB;

        if((initdram(0)-(CONFIG_SYS_TEXT_BASE&0x1FFFFFFF)) > (mt_size+TEST_SIZE_PER_PATTREN+RSV_SPACE)){
            flash_backup_addr = CONFIG_SYS_TEXT_BASE+RSV_SPACE;
        }else if((CONFIG_SYS_TEXT_BASE&0x1FFFFFFF) > (mt_size+TEST_SIZE_PER_PATTREN+RSV_SPACE)){
            flash_backup_addr = CONFIG_SYS_TEXT_BASE-mt_size-TEST_SIZE_PER_PATTREN-RSV_SPACE;
        }else{
            printf("[Error] No enough space to back up the SPI-Flash data:\n");    
            return MT_FAIL;
        }
        src_data_addr = flash_backup_addr + mt_size;


        for(i=0;i<cmd_info.test_loops;i++){
            u32_t part_num = 0;
            switch(flash_size_in_MB){
                case 64:
                    mt_start = FLASHBASE + (64*SIZE_1MB-mt_size);
                    printf("\nRegion(%d): 0x%08x ~ 0x%08x\n",part_num++, mt_start, (mt_start+mt_size));
                    if(MT_FAIL == _common_flash_test(mt_start, mt_size)) return MT_FAIL;

                    mt_start = FLASHBASE + (48*SIZE_1MB-mt_size);
                    printf("\nRegion(%d): 0x%08x ~ 0x%08x\n",part_num++, mt_start, (mt_start+mt_size));
                    if(MT_FAIL == _common_flash_test(mt_start, mt_size)) return MT_FAIL;

                case 32:
                    mt_start = FLASHBASE + (32*SIZE_1MB-mt_size);
                    printf("\nRegion(%d): 0x%08x ~ 0x%08x\n",part_num++, mt_start, (mt_start+mt_size));
                    if(MT_FAIL == _common_flash_test(mt_start, mt_size)) return MT_FAIL;

                    mt_start = FLASHBASE + (18*SIZE_1MB-mt_size);
                    printf("\nRegion(%d): 0x%08x ~ 0x%08x\n",part_num++, mt_start, (mt_start+mt_size));
                    if(MT_FAIL == _common_flash_test(mt_start, mt_size)) return MT_FAIL;

                case 16:
                    mt_start = FLASHBASE + (16*SIZE_1MB-mt_size);
                    printf("\nRegion(%d): 0x%08x ~ 0x%08x\n",part_num++, mt_start, (mt_start+mt_size));
                    if(MT_FAIL == _common_flash_test(mt_start, mt_size)) return MT_FAIL;

                case 8:
                    mt_start = FLASHBASE + (8*SIZE_1MB-mt_size);
                    printf("\nRegion(%d): 0x%08x ~ 0x%08x\n",part_num++, mt_start, (mt_start+mt_size));
                    if(MT_FAIL == _common_flash_test(mt_start, mt_size)) return MT_FAIL;

                default:
                    mt_start = FLASHBASE + (2*SIZE_1MB-mt_size);
                    printf("\nRegion(%d): 0x%08x ~ 0x%08x\n",part_num++, mt_start, (mt_start+mt_size));
                    if(MT_FAIL == _common_flash_test(mt_start, mt_size)) return MT_FAIL;                 
                    break;
            }
            printf("\n[mflash_test] %d runs OK\n",i+1);       
        }
    }

restore_setting:
//    spi_enable_message();
    assign_test_range = 0;

		/* Reset if the command is sent from the command line */
		if(UBOOT_RESET == g_reset_flag){
			do_reset (NULL, 0, 0, NULL);
		}else if(WHOLE_CHIP_RESET == g_reset_flag){	
			SYSTEM_RESET();
		}
	return retcode;
}

