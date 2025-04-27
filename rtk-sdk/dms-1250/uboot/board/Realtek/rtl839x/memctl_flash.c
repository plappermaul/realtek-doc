
/*
 * Include Files
 */
#include "config.h"
#define FLLOW_DRIVER
#ifdef FLLOW_DRIVER
#include <common.h>
#include <spi_flash.h>
#endif
#include <soc.h>
#include <pblr.h>
#include "flash_spi.h"
#define _cache_flush	pblr_dc_flushall

static unsigned long flash_size;
#ifdef FLLOW_DRIVER
static struct spi_flash *sf;
#endif

/*Platform independent parameter*/
#ifdef CONFIG_MT_ERR_HANDLE
#define HANDLE_FAIL \
({ \
	printf("%s (%d) test failed.\n", __FUNCTION__,__LINE__);\
	return -1;\
})
#else
#define HANDLE_FAIL \
({ \
	printf("%s (%d) test failed.\n", __FUNCTION__,__LINE__);\
	while(1);\
})
#endif

extern void spi_flash_init(void);
extern unsigned int spi_flash_sectorsize(void);
extern ulong uboot_end;
#define	FLASH_BASE_ADDRESS	CONFIG_SYS_FLASH_BASE
//#define	FLASH_SIZE         	(gd->bd->bi_flashsize)
#define	FLASH_TOP_ADDRESS	(FLASH_BASE_ADDRESS+flash_size-1)
#define	TEST_SIZE_PER_PATTREN	(0x10000) //64KB

static unsigned int src_data_addr,reserved_size;
#define	FLASH_BACKUP_ADDR	(flash_backup_addr)
#define	SRC_DATA_ADDR		(src_data_addr)
#define RESERVED_UBOOT_DRAM_SIZE ((((unsigned int)&uboot_end-CONFIG_SYS_TEXT_BASE) & ~(0x100000-1))+0x100000)
#define TEST_SPACE			(flash_size+TEST_SIZE_PER_PATTREN+RESERVED_UBOOT_DRAM_SIZE)


//#define HANDLE_FAIL  goto test_fail;
//#define SRC_DATA_ADDR (0x84000000)


/*
 * Data Declaration
 */
DECLARE_GLOBAL_DATA_PTR;

//extern FLASH_INFO_TYPE  flash_info[]; /* info for FLASH chips */
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
#ifndef FLLOW_DRIVER
static int flash_fill_sect_ranges(ulong addr_first, ulong addr_last, int *s_first, int *s_last, int *s_count )
{
	//FLASH_INFO_TYPE *info;
	ulong bank;
	int rcode = 0;

	*s_count = 0;

	for (bank=0; bank < CFG_MAX_FLASH_BANKS; ++bank) {
		s_first[bank] = -1;	/* first sector to erase	*/
		s_last [bank] = -1;	/* last  sector to erase	*/
	}

	for (bank=0,info=&flash_info[0];
	     (bank < CFG_MAX_FLASH_BANKS) && (addr_first <= addr_last);
	     ++bank, ++info) {
		ulong b_end;
		int sect;
		short s_end;

		if (info->flash_id == FLASH_UNKNOWN) {
			continue;
		}

		b_end = info->start[0] + info->size - 1;	/* bank end addr */
		s_end = info->sector_count - 1;			/* last sector   */


		for (sect=0; sect < info->sector_count; ++sect) {
			ulong end;	/* last address in current sect	*/

			end = (sect == s_end) ? b_end : info->start[sect + 1] - 1;

			if (addr_first > end)
				continue;
			if (addr_last < info->start[sect])
				continue;

			if (addr_first == info->start[sect]) {
				s_first[bank] = sect;
			}
			if (addr_last  == end) {
				s_last[bank]  = sect;
			}
		}
		if (s_first[bank] >= 0) {
			if (s_last[bank] < 0) {
				if (addr_last > b_end) {
					s_last[bank] = s_end;
				} else {
					printf("Error: end address"
						" not on sector boundary\n");
					rcode = 1;
					break;
				}
			}
			if (s_last[bank] < s_first[bank]) {
				printf("Error: end sector"
					" precedes start sector\n");
				rcode = 1;
				break;
			}
			sect = s_last[bank];
			addr_first = (sect == s_end) ? b_end + 1: info->start[sect + 1];
			(*s_count) += s_last[bank] - s_first[bank] + 1;
		} 
		else if (addr_first >= info->start[0] && addr_first < b_end) {
			printf("addr_first 0x%x\n", (u32_t)addr_first);
			printf("b_end 0x%x\n",(u32_t)b_end);
			printf("info->start[0] 0x%x\n",(u32_t)info->start[0]);
			printf("Error: start address not on sector boundary\n");
			rcode = 1;
			break;
		} else if (s_last[bank] >= 0) {
			printf("Error: cannot span across banks when they are"
			       " mapped in reverse order\n");
			rcode = 1;
			break;
		}
	}
	return rcode;
}
#endif

int flash_sect_erase (ulong addr_first, ulong addr_last)
{
#ifdef FLLOW_DRIVER
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
#else
	FLASH_INFO_TYPE *info;
	ulong bank;
	int s_first[CFG_MAX_FLASH_BANKS], s_last[CFG_MAX_FLASH_BANKS];
	int erased = 0;
	int planned;
	int rcode = 0;

	rcode = flash_fill_sect_ranges (addr_first, addr_last, s_first, s_last, &planned );

	if (planned && (rcode == 0)) 
	{
		for (bank=0,info=&flash_info[0];(bank<CFG_MAX_FLASH_BANKS) && (rcode == 0); ++bank, ++info)
		{
			if (s_first[bank]>=0)
			{
				erased += s_last[bank] - s_first[bank] + 1;
#if defined(SPI_SHOW_PROGRESS)                 
				printf("Erase Flash from 0x%08lx to 0x%08lx in Bank #%ld ",
					info->start[s_first[bank]],
					(s_last[bank] == (info->sector_count-1)) ?
					(info->start[0] + info->size - 1):(info->start[s_last[bank]+1] - 1),
					bank+1);
#endif	/*SPI_SHOW_PROGRESS*/
				rcode = flash_erase(info, s_first[bank], s_last[bank]);
			}
		}
#if defined(SPI_SHOW_PROGRESS)        
		printf("Erased %d sectors\n", erased);
#endif	/*SPI_SHOW_PROGRESS*/
	} 
	else if (rcode == 0)
	{
		printf("Error: start and/or end address"
			" not on sector boundary\n");
		rcode = 1;
	}
	return rcode;
#endif    
}

#ifndef FLLOW_DRIVER
/*-----------------------------------------------------------------------
 * Set protection status for monitor sectors
 *
 * The monitor is always located in the _first_ Flash bank.
 * If necessary you have to map the second bank at lower addresses.
 */
void flash_protect (int flag, ulong from, ulong to, FLASH_INFO_TYPE *info)
{
	ulong b_end = info->start[0] + info->size - 1;	/* bank end address */
	short s_end = info->sector_count - 1;	/* index of last sector */
	int i;

	/* Do nothing if input data is bad. */
	if (info->sector_count == 0 || info->size == 0 || to < from) {
		return;
	}

	printf("flash_protect %s: from 0x%08lX to 0x%08lX\n",
		(flag & FLAG_PROTECT_SET) ? "ON" :
			(flag & FLAG_PROTECT_CLEAR) ? "OFF" : "???",
		from, to);

	/* There is nothing to do if we have no data about the flash
	 * or the protect range and flash range don't overlap.
	 */
	if (info->flash_id == FLASH_UNKNOWN ||
	    to < info->start[0] || from > b_end) {
		return;
	}

	for (i=0; i<info->sector_count; ++i) {
		ulong end;		/* last address in current sect	*/

		end = (i == s_end) ? b_end : info->start[i + 1] - 1;

		/* Update protection if any part of the sector
		 * is in the specified range.
		 */
		if (from <= end && to >= info->start[i]) {
			if (flag & FLAG_PROTECT_CLEAR) {
				info->protect[i] = 0;
				//printf ("protect off %d\n", i);
			}
			else if (flag & FLAG_PROTECT_SET) {
				info->protect[i] = 1;
				printf("protect on %d\n", i);
			}
		}
	}
}

FLASH_INFO_TYPE *addr2info (ulong addr)
{
	FLASH_INFO_TYPE *info;
	int i;
	
	for (i=0, info=&flash_info[0]; i<CFG_MAX_FLASH_BANKS; ++i, ++info) {	
		if (info->flash_id != FLASH_UNKNOWN &&
		    addr >= info->start[0] &&
		    /* WARNING - The '- 1' is needed if the flash
		     * is at the end of the address space, since
		     * info->start[0] + info->size wraps back to 0.
		     * Please don't change this unless you understand this.
		     */
		    addr <= info->start[0] + info->size - 1) {
			return (info);
		}
	}
	return (FLASH_INFO_TYPE *)0;
}
#endif

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
#ifdef FLLOW_DRIVER
	if(!sf) {
        sf = spi_flash_probe(0, 0, 0, 0);
        if(!sf) {
            printf("spi flash probe failed\n");
            return 0;
        }
    }
	return spi_flash_write(sf, (addr-FLASH_BASE_ADDRESS), cnt, src);
#else
	int i;
	ulong         end        = addr + cnt - 1;
	FLASH_INFO_TYPE *info_first = addr2info (addr);
	FLASH_INFO_TYPE *info_last  = addr2info (end );
	FLASH_INFO_TYPE *info;

	if (cnt == 0) {
		return (ERR_OK);
	}

	if (!info_first || !info_last) {
		return (ERR_INVAL);
	}

	for (info = info_first; info <= info_last; ++info) {
		ulong b_end = info->start[0] + info->size;	/* bank end addr */
		short s_end = info->sector_count - 1;
		for (i=0; i<info->sector_count; ++i) {
			ulong e_addr = (i == s_end) ? b_end : info->start[i + 1];

			if ((end >= info->start[i]) && (addr < e_addr) &&
			    (info->protect[i] != 0) ) {
				return (ERR_PROTECTED);
			}
		}
	}

	/* finally write data to flash */
	for (info = info_first; info <= info_last && cnt>0; ++info) {
		ulong len;

		len = info->start[0] + info->size - addr;
        //printf("flash write from 0x%08x to 0x%08lx\n",
        //    (u32_t)src, (ulong)addr);
		if (len > cnt)
			len = cnt;
		if ((i = write_buff(info, (uchar *)src, addr, len)) != 0) {
			return (i);
		}
		cnt  -= len;
		addr += len;
		src  += len;
	}
	return (ERR_OK);
#endif
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
		printf("Flash: pattern[%d](0x%x) setting pass\n", i, flash_patterns[i]);
		
		src_start = (u32_t *)SRC_DATA_ADDR;
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
	return 0;
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
				printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",
				(u32_t)src_start , start_value, walk_pattern,  __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			src_start++;
		}
		printf("Flash: pattern[%d](0x%x) setting passed\n", i, walk_pattern);

		src_start = (u32_t *)SRC_DATA_ADDR;
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
	return 0;
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
		printf("Flash: pattern[%d](0x%x) setting passed\n", i, walk_pattern);
		
		src_start = (u32_t *)SRC_DATA_ADDR;
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
	return 0;
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
		printf("Flash: rotate %d setting passed\n", i);
		

		src_start = (u32_t *)SRC_DATA_ADDR;
		flash_start = flash_start_addr + ((i*test_size_per_pattern)%flash_test_size);
		flash_sect_erase(flash_start, (flash_start+test_size_per_pattern-1));
		flash_write((char *)src_start, flash_start, test_size_per_pattern);

		/* check data */  
		_dram_start = (u32_t *) SRC_DATA_ADDR;
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
	return 0;
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
		printf("Flash: ~rotate %d setting passed\n", i);
		
		_cache_flush();
		
		src_start = (u32_t *)SRC_DATA_ADDR;
		flash_start = flash_start_addr + ((i*test_size_per_pattern)%flash_test_size);
		flash_sect_erase(flash_start, (flash_start+test_size_per_pattern-1));
		flash_write((char *)src_start, flash_start, test_size_per_pattern);
		
		/* check data */  
		_dram_start = (u32_t *) SRC_DATA_ADDR;
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
	return 0 ;
}

int _flash_test(u32_t flash_start_addr, u32_t test_size_per_pattern, u32_t flash_test_size)
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

int memctl_flash_test(u32_t offset, u32_t size, int testLoops)
{
	volatile u32_t *bootcode;
	volatile u32_t *bk_buffer;
	int32 retcode = 0;
	u32_t i;
  u32_t l, flash_backup_addr,bootcode_end;
  u32_t test_addr,test_size,sector_size,temp,tmp_addr;

	test_addr = FLASH_BASE_ADDRESS + offset;

#ifdef FLLOW_DRIVER
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
#else
	flash_size = soc_flash_init();
#endif
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
	bk_buffer = (u32_t *)FLASH_BACKUP_ADDR;
	bootcode = (u32_t *)test_addr;
   	printf("Back up flash data (0x%08x -> 0x%08x) ... ", (u32_t)bootcode, (u32_t)bk_buffer);
	for(i=0; i<test_size; i=i+4){
		*bk_buffer = *bootcode;
		bk_buffer++;
		bootcode++;
	}
	bk_buffer = (u32_t *)FLASH_BACKUP_ADDR;
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
#ifndef FLLOW_DRIVER
	flash_protect( FLAG_PROTECT_CLEAR, FLASH_BASE_ADDRESS, \
			FLASH_TOP_ADDRESS(flash_size), addr2info(FLASH_BASE_ADDRESS));
#endif
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
	printf("\nRecover flash data (0x%08x -> 0x%08x) ... ", (u32_t)FLASH_BACKUP_ADDR, (u32_t)test_addr);
	flash_sect_erase(test_addr, test_addr+(test_size-1));
	flash_write((char *)FLASH_BACKUP_ADDR, test_addr, test_size);


	/* 3.3 Verify data */
	printf("done.\nVerify recovered data ... ");
	bk_buffer = (u32_t *)FLASH_BACKUP_ADDR;
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

int flash_test(int flag, int argc, void* argv[])
{
	volatile u32_t *bootcode;
	volatile u32_t *bk_buffer;
	int32 retcode = 0;
	u32_t i;
  u32_t l, testLoops, flash_backup_addr,bootcode_end;
  u32_t test_addr,test_size,sector_size,temp,tmp_addr;
    
	if(argc > 1)
	{
    test_addr = simple_strtoul(argv[1], NULL, 10);
    test_addr += FLASH_BASE_ADDRESS;
  }
  else
  	test_addr = FLASH_BASE_ADDRESS;
        
	if(argc > 3)
  	testLoops = simple_strtoul(argv[3], NULL, 10);
    else
        testLoops = 1;

#ifdef FLLOW_DRIVER
	if(parameters.flash_init_result == INI_RES_UNINIT) {
		spi_flash_init();
    }
	flash_size = 1 << para_flash_info.size_per_chip;
    printf("flash size = %ldMB\n", flash_size / (0x100000));
    
	if(!sf) {
        sf = spi_flash_probe(0, 0, 0, 0);
        if(!sf) {
            printf("spi flash probe failed\n");
            return 0;
        }
    }
#else
	flash_size = soc_flash_init();
#endif
		if(argc > 2)
        test_size = simple_strtoul(argv[2], NULL, 10);
    else
        test_size = flash_size;    

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
	bk_buffer = (u32_t *)FLASH_BACKUP_ADDR;
	bootcode = (u32_t *)test_addr;
   	printf("Back up flash data (0x%08x -> 0x%08x) ... ", (u32_t)bootcode, (u32_t)bk_buffer);
	for(i=0; i<test_size; i=i+4){
		*bk_buffer = *bootcode;
		bk_buffer++;
		bootcode++;
	}
	bk_buffer = (u32_t *)FLASH_BACKUP_ADDR;
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
#ifndef FLLOW_DRIVER
	flash_protect( FLAG_PROTECT_CLEAR, FLASH_BASE_ADDRESS, \
			FLASH_TOP_ADDRESS(flash_size), addr2info(FLASH_BASE_ADDRESS));
#endif
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
			printf("No recover other data\n");
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
		}
        puts("\n");
	}

	/* 
	 * 3. Copy back and verify data into the flash. 
	 */

	/* 3.1 code */
	printf("\nRecover flash data (0x%08x -> 0x%08x) ... ", (u32_t)FLASH_BACKUP_ADDR, (u32_t)test_addr);
	flash_sect_erase(test_addr, test_addr+(test_size-1));
	flash_write((char *)FLASH_BACKUP_ADDR, test_addr, test_size);


	/* 3.3 Verify data */
	printf("done.\nVerify recovered data ... ");
	bk_buffer = (u32_t *)FLASH_BACKUP_ADDR;
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


