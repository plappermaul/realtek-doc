/*
 * Include Files
 */
#include <common.h>
#include <asm/io.h>
#include <spi_flash.h>
#include "serial_nor.h"

static struct spi_flash *g_mnor_test;


#define _cache_flush()


/* Definitions for memory test error handing manner */
#define MT_SUCCESS    (0)
#define MT_SKIP       (-1)
#define MT_FAIL       (1)
#define ARY_LEN(x)    (sizeof(x)/sizeof(x[0]))


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

#define HANDLE_FAIL \
({ \
    printf("%s (%d) test failed.\n", __FUNCTION__,__LINE__);\
    return MT_FAIL; \
})

static uint32_t flash_backup_addr = 0x00000000;
static uint32_t src_patt_addr = 0x01000000;
static uint32_t dst_data_addr = 0x02000000;

#define SIZE_4KB  (0x1000)
#define SIZE_64KB (0x10000)
#define SIZE_1MB  (0x100000)
#define SIZE_2MB  (0x200000)
#define SIZE_3MB  (0x300000)
#define SIZE_4MB  (0x400000)
#define TEST_SIZE_PER_PATTREN (SIZE_64KB)
#define RSV_SPACE             (SIZE_4MB)
#define SRC_PATT_ADDR         (src_patt_addr)
#define DST_DATA_ADDR         (dst_data_addr)
#define FLASH_BACKUP_ADDR     (flash_backup_addr)
uint32_t assign_test_range = 0;

/**************************
  * Command Parsing
  *************************/
typedef struct {
    uint8_t test_loops;
    uint32_t mt_offset; //Offset: 0 ~ 16MB ~ 32MB ~ 64MB
    uint32_t mt_size;   //Unit is byte
    uint32_t test_case; //0xCE: chip_erase
} spif_cmd_parsing_info_t;



extern unsigned long simple_strtoul(const char *cp,char **endp,unsigned int base);

/*
 * Data Declaration
 */
DECLARE_GLOBAL_DATA_PTR;

const uint32_t flash_patterns[] = {
    0x5a5aa5a5,
    0xa5a5a5a5,
    0x55555555,
    0xaaaaaaaa,
    0x00000000,
    0xff00ff00,
    0x00ff00ff,
    0x0000ffff,
    0xffff0000,
    0xffffffff,
};

/*
 * Function Declaration
 */
int flash_sect_erase (ulong addr_first, ulong cnt)
{
    uint32_t offset = addr_first-CONFIG_SYS_FLASH_BASE;
    return spi_flash_erase(g_mnor_test, offset, cnt);
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
 *            (only some targets require alignment)
 */
int
flash_write (char *src, ulong addr, ulong cnt)
{
    uint32_t offset = (addr-CONFIG_SYS_FLASH_BASE);
    return spi_flash_write(g_mnor_test, offset, cnt, src);
}

int flash_read (char *src, ulong addr, ulong cnt)
{
    uint32_t offset = (addr-CONFIG_SYS_FLASH_BASE);
#if 1
return spi_flash_read(g_mnor_test, offset, cnt, src);
#else /* For checking JIRA issue: https://jira.realtek.com/browse/VENUSII-1558 */
    uint32_t i;
    uint32_t bytes_per_read = 12;
    uint32_t remain = cnt;

    for(i=bytes_per_read ; i<cnt ; i+=bytes_per_read){
        spi_flash_read(g_mnor_test, offset, bytes_per_read, src);
        offset  += bytes_per_read;
        src  += bytes_per_read;
        remain = remain - bytes_per_read;
    }
    spi_flash_read(g_mnor_test, offset, remain, src);
    return 0;
#endif
}

/* Function Name:
 *     nor_normal_patterns
 * Descripton:
 *
 * Input:
 *    None
 * Output:
 *     None
 * Return:
 *      None
 */
int flash_normal_patterns(uint32_t flash_start_addr, uint32_t test_size_per_pattern, uint32_t flash_test_size)
{
    int i, j;
    uint32_t read_value, flash_start;
    volatile uint32_t *dram_addr;
    uint32_t re_idx=0;
    uint32_t remain_size=flash_test_size;
    char _err_msg[] = {"\nEE: addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n"};

    printf("=======start %s test / 0x%08x~0x%08x (%dMB/%dKB)=======\n", __FUNCTION__, flash_start_addr, flash_start_addr+flash_test_size-1, flash_test_size/SIZE_1MB, flash_test_size/1024);
    for (i=0; i < (sizeof(flash_patterns)/sizeof(uint32_t)); i++, remain_size=flash_test_size, re_idx=0){
        /* write pattern*/
        _cache_flush();
        dram_addr = (uint32_t *)(uintptr_t)SRC_PATT_ADDR;
        for(j=0; j < test_size_per_pattern; j=j+4) {
            *dram_addr++ = flash_patterns[i];
        }

        /* check pattern */
        _cache_flush();
        dram_addr = (uint32_t *)(uintptr_t)SRC_PATT_ADDR;
        for(j=0; j < test_size_per_pattern; j=j+4){
            read_value = (*dram_addr);
            if(read_value != flash_patterns[i]){
                printf(_err_msg,(uint32_t)(uintptr_t)dram_addr,read_value,flash_patterns[i],__FUNCTION__,__LINE__);
                HANDLE_FAIL;
            }
            dram_addr++;
        }
        printf("\rSPIF: pattern[%d](0x%x) setting pass", i, flash_patterns[i]);


        while(remain_size >= test_size_per_pattern){
            dram_addr = (uint32_t *)(uintptr_t)SRC_PATT_ADDR;
            flash_start = flash_start_addr + ((re_idx*test_size_per_pattern)%flash_test_size);
            flash_sect_erase(flash_start, test_size_per_pattern);
            flash_write((char *)dram_addr, flash_start, test_size_per_pattern);

            /* check flash data sequentially. */
            dram_addr = (uint32_t *)(uintptr_t)DST_DATA_ADDR;
            flash_read((char *)dram_addr, flash_start, test_size_per_pattern);
            for(j=0; j < test_size_per_pattern; j=j+4){
                read_value = (*dram_addr);
                if(read_value != flash_patterns[i]){
                    printf(_err_msg,(uint32_t)(uintptr_t)dram_addr,read_value,flash_patterns[i],__FUNCTION__,__LINE__);
                    HANDLE_FAIL;
                }
                dram_addr++;
            }
            re_idx++;
            remain_size -= test_size_per_pattern;
            printf("\rSPIF: pat[%d](0x%x) 0x%x pass (remain 0x%x)", i, flash_patterns[i], flash_start, remain_size);
        }
    }
    printf("\n\r%s test succeed.                              \n", __FUNCTION__);
    return MT_SUCCESS;
}


int flash_walking_of_1(uint32_t flash_start_addr, uint32_t test_size_per_pattern, uint32_t flash_test_size)
{
    int i;
    int j;
    uint32_t walk_pattern;
    uint32_t start_value;
    uint32_t flash_start;
    uint32_t re_idx=0;
    uint32_t remain_size = flash_test_size;
    volatile uint32_t *dram_addr;

    printf("\n=======start %s test / 0x%08x~0x%08x (%dMB/%dKB)=======\n", __FUNCTION__, flash_start_addr, flash_start_addr+flash_test_size-1, flash_test_size/SIZE_1MB, flash_test_size/1024);
    while(remain_size >= test_size_per_pattern){
        for (i=0; i < 32; i++){
            /* generate pattern */
            walk_pattern = (1 << i);

            /* write pattern*/
            _cache_flush();
            dram_addr = (uint32_t *)(uintptr_t)SRC_PATT_ADDR;
            for(j=0; j < test_size_per_pattern; j=j+4){
                *dram_addr++ = walk_pattern;
            }

            /* check pattern */
            _cache_flush();
            dram_addr = (uint32_t *)(uintptr_t)SRC_PATT_ADDR;
            for(j=0; j < test_size_per_pattern; j=j+4){
                start_value = (*dram_addr);
                if(start_value != walk_pattern){
                    printf("EE: addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
                    (uint32_t)(uintptr_t)dram_addr , start_value, walk_pattern,  __FUNCTION__, __LINE__);
                    HANDLE_FAIL;
                }
                dram_addr++;
            }
            printf("\rFlash: pattern[%d](0x%x) setting passed", i, walk_pattern);

            dram_addr = (uint32_t *)(uintptr_t)SRC_PATT_ADDR;
            flash_start = flash_start_addr + (((re_idx++)*test_size_per_pattern)%flash_test_size);
            flash_sect_erase(flash_start, test_size_per_pattern);
            flash_write((char *)dram_addr, flash_start, test_size_per_pattern);

            /* check data */
            dram_addr = (uint32_t *)(uintptr_t)DST_DATA_ADDR;
            flash_read((char *)dram_addr, flash_start, test_size_per_pattern);
            for(j=0; j < test_size_per_pattern; j=j+4){
                start_value = (*dram_addr);
                if(start_value != walk_pattern){
                    printf("\nEE: addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
                    (uint32_t)(uintptr_t)dram_addr , start_value, walk_pattern,  __FUNCTION__, __LINE__);
                    HANDLE_FAIL;
                }
                dram_addr++;
            }
            remain_size -= test_size_per_pattern;
            printf("\rFlash: pattern[%d](0x%x) 0x%x passed (remain 0x%x)", i, walk_pattern, flash_start, remain_size);
            if(0 == remain_size) goto end;
        }
    }
end:
    printf("\r%s test succeed.            \n", __FUNCTION__);
    return MT_SUCCESS;
}

int flash_walking_of_0(uint32_t flash_start_addr, uint32_t test_size_per_pattern, uint32_t flash_test_size)
{
    int i;
    int j;
    uint32_t read_value;
    uint32_t walk_pattern;
    uint32_t flash_start;
    uint32_t re_idx=0;
    uint32_t remain_size = flash_test_size;
    volatile uint32_t *dram_addr;

    printf("\n=======start %s test / 0x%08x~0x%08x (%dMB/%dKB)=======\n", __FUNCTION__, flash_start_addr, flash_start_addr+flash_test_size-1, flash_test_size/SIZE_1MB, flash_test_size/1024);
    while(remain_size >= test_size_per_pattern){
        for (i=0; i < 32; i++){
            /* generate pattern */
            walk_pattern = ~(1 << i);

            /* write pattern*/
            _cache_flush();
            dram_addr = (uint32_t *)(uintptr_t)SRC_PATT_ADDR;
            for(j=0; j < test_size_per_pattern; j=j+4){
                *dram_addr++ = walk_pattern;
            }

            /* check data */
            _cache_flush();
            dram_addr = (uint32_t *)(uintptr_t)SRC_PATT_ADDR;
            for(j=0; j < test_size_per_pattern; j=j+4){
                read_value = (*dram_addr);
                if(read_value != walk_pattern){
                    printf("EE: addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
                    (uint32_t)(uintptr_t)dram_addr , read_value, walk_pattern,  __FUNCTION__, __LINE__);
                    HANDLE_FAIL;
                }
                dram_addr++;
            }
            printf("\rFlash:pattern[%d](0x%x) setting passed", i, walk_pattern);

            dram_addr = (uint32_t *)(uintptr_t)SRC_PATT_ADDR;
            flash_start = flash_start_addr + (((re_idx++)*test_size_per_pattern)%flash_test_size);
            flash_sect_erase(flash_start, test_size_per_pattern);
            flash_write((char *)dram_addr, flash_start, test_size_per_pattern);

            /* check data */
            dram_addr = (uint32_t *)(uintptr_t)DST_DATA_ADDR;
            flash_read((char *)dram_addr, flash_start, test_size_per_pattern);
            for(j=0; j < test_size_per_pattern; j=j+4){
                read_value = (*dram_addr);
                if(read_value != walk_pattern){
                    printf("\nEE: addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
                    (uint32_t)(uintptr_t)dram_addr , read_value, walk_pattern,  __FUNCTION__, __LINE__);
                    HANDLE_FAIL;
                }
                dram_addr++;
            }
            remain_size -= test_size_per_pattern;
            printf("\rFlash:pattern[%d](0x%x) 0x%x passed (remain 0x%x)", i, walk_pattern, flash_start, remain_size);
            if(0 == remain_size) goto end;
        }
    }
end:
    printf("\r%s test succeed.            \n", __FUNCTION__);
    return MT_SUCCESS;
}

int flash_addr_rot(uint32_t flash_start_addr, uint32_t test_size_per_pattern, uint32_t flash_test_size)
{
    int i;
    int j;
    uint32_t read_value;
    uint32_t flash_start;
    uint32_t pattern;
    uint32_t re_idx=0;
    uint32_t remain_size = flash_test_size;
    volatile uint32_t *dram_addr;
    volatile uint32_t *check_addr;

    printf("\n=======start %s test / 0x%08x~0x%08x (%dMB/%dKB)=======\n", __FUNCTION__, flash_start_addr, flash_start_addr+flash_test_size-1, flash_test_size/SIZE_1MB, flash_test_size/1024);
    while(remain_size >= test_size_per_pattern){
        for (i=0; i < 32; i=i+4){
            /* write pattern*/
            _cache_flush();
            dram_addr = (uint32_t *)(uintptr_t)SRC_PATT_ADDR;
            for(j=0; j < test_size_per_pattern; j=j+4){
                pattern = ((uint32_t)(uintptr_t)dram_addr << i);
                *dram_addr++ = pattern;
            }

            /* check data */
            _cache_flush();
            dram_addr = (uint32_t *)(uintptr_t)SRC_PATT_ADDR;
            for(j=0; j < test_size_per_pattern; j=j+4){
                pattern = ((uint32_t)(uintptr_t)dram_addr << i);
                read_value = (*dram_addr);
                if(read_value != pattern){
                    printf("\nEE: addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
                          (uint32_t)(uintptr_t)dram_addr , read_value, pattern,  __FUNCTION__, __LINE__);
                    HANDLE_FAIL;
                }
                dram_addr++;
            }
            printf("\rFlash: rotate %d setting passed", i);

            dram_addr = (uint32_t *)(uintptr_t)SRC_PATT_ADDR;
            flash_start = flash_start_addr + (((re_idx++)*test_size_per_pattern)%flash_test_size);
            flash_sect_erase(flash_start, test_size_per_pattern);
            flash_write((char *)dram_addr, flash_start, test_size_per_pattern);

            /* check data */
            check_addr = (uint32_t *)(uintptr_t)DST_DATA_ADDR;
            flash_read((char *)check_addr, flash_start, test_size_per_pattern);
            for(j=0; j < test_size_per_pattern; j=j+4){
                pattern = ((uint32_t)(uintptr_t)dram_addr << i);
                read_value = (*check_addr);
                if(read_value != pattern){
                    printf("\nEE: addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
                    (uint32_t)(uintptr_t)check_addr , read_value, pattern,  __FUNCTION__, __LINE__);
                    HANDLE_FAIL;
                }
                check_addr++;
                dram_addr++;
            }
            remain_size -= test_size_per_pattern;
            printf("\rFlash: rotate %d 0x%x passed (remain 0x%x)", i, flash_start, remain_size);
            if(0 == remain_size) goto end;
        }
    }
end:
    printf("\r%s test succeed.            \n", __FUNCTION__);
    return MT_SUCCESS;
}

int flash_com_addr_rot(uint32_t flash_start_addr, uint32_t test_size_per_pattern, uint32_t flash_test_size)
{
    int i;
    int j;
    uint32_t start_value;
    uint32_t flash_start;
    uint32_t re_idx=0;
    uint32_t remain_size = flash_test_size;
    volatile uint32_t *dram_addr;
    volatile uint32_t *check_addr;

    printf("\n=======start %s test / 0x%08x~0x%08x(%dMB/%dKB)=======\n", __FUNCTION__, flash_start_addr, flash_start_addr+flash_test_size-1, flash_test_size/SIZE_1MB, flash_test_size/1024);
    while(remain_size >= test_size_per_pattern){
        for (i=0; i < 32; i=i+4){
            /* write pattern*/
            _cache_flush();
            dram_addr = (uint32_t *)(uintptr_t)SRC_PATT_ADDR;
            for(j=0; j < test_size_per_pattern; j=j+4){
                *dram_addr = ~(((uint32_t)(uintptr_t)dram_addr) << i);
                dram_addr++;
            }

            /* check data */
            _cache_flush();
            dram_addr = (uint32_t *)(uintptr_t)SRC_PATT_ADDR;
            for(j=0; j < test_size_per_pattern; j=j+4){
                start_value = (*dram_addr);
                if(start_value != (~((uint32_t)(uintptr_t)dram_addr << i))){
                     printf("EE: addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
                            (uint32_t)(uintptr_t)dram_addr , start_value, ~(((uint32_t)(uintptr_t)dram_addr) << i),  __FUNCTION__, __LINE__);
                    HANDLE_FAIL;
                }
                dram_addr++;
            }
            printf("\rFlash: ~rotate %d setting passed", i);

               _cache_flush();
            dram_addr = (uint32_t *)(uintptr_t)SRC_PATT_ADDR;
            flash_start = flash_start_addr + (((re_idx++)*test_size_per_pattern)%flash_test_size);
            flash_sect_erase(flash_start, test_size_per_pattern);
            flash_write((char *)dram_addr, flash_start, test_size_per_pattern);

            /* check data */
            dram_addr = (uint32_t *)(uintptr_t) SRC_PATT_ADDR;
            check_addr = (uint32_t *)(uintptr_t)DST_DATA_ADDR;
            flash_read((char *)check_addr, flash_start, test_size_per_pattern);
            for(j=0; j < test_size_per_pattern; j=j+4){
                start_value = (*check_addr);
                if(start_value != (~((uint32_t)(uintptr_t)dram_addr << i))){
                    printf("\nEE: addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
                    (uint32_t)(uintptr_t)check_addr , start_value, ~(((uint32_t)(uintptr_t)dram_addr) << i),  __FUNCTION__, __LINE__);
                    HANDLE_FAIL;
                }
                dram_addr++;
                check_addr++;
            }

            remain_size -= test_size_per_pattern;
            printf("\rFlash: ~rotate %d 0x%x passed (remain 0x%x)", i, flash_start,remain_size);
            if(0 == remain_size) goto end;
        }
    }
end:
    printf("\r%s test succeed.            \n", __FUNCTION__);
    return MT_SUCCESS;
}

int flash_chip_erase_test(void)
{
    uint32_t i, offset;
    uint32_t *dram_addr = (uint32_t *)(uintptr_t)SRC_PATT_ADDR;

    printf("\n=======start chip_erase test... erase");
    flash_sect_erase(CONFIG_SYS_FLASH_BASE, g_mnor_test->size);

    printf(" ok... check erased data ");
    for(offset=0; offset < g_mnor_test->size; offset=offset+RSV_SPACE){
        printf("0x%08x\b\b\b\b\b\b\b\b\b\b",offset);

        flash_read((char *)dram_addr, CONFIG_SYS_FLASH_BASE, RSV_SPACE);
        for(i=0; i < RSV_SPACE; i=i+4){
            if(*dram_addr != 0xFFFFFFFF){
                printf("EE: Erase fail at offset(0x%x+0x%x) addr(0x%x):0x%x \n",offset,i,(uint32_t)(uintptr_t)dram_addr,*dram_addr);
                HANDLE_FAIL;
            }
            dram_addr++;
        }
    }
    printf(" ok=======\n");
    return 0;

}

int _flash_test(uint32_t flash_start_addr, uint32_t test_size_per_pattern, uint32_t flash_test_size) {
    uint32_t i;
    typedef int (norsf_test_t)(uint32_t, uint32_t, uint32_t);
    norsf_test_t *norsf_tf_list[] = {
        flash_normal_patterns,
        flash_walking_of_1,
        flash_walking_of_0,
        flash_addr_rot,
        flash_com_addr_rot,
    };

    for (i=0; i<ARY_LEN(norsf_tf_list); i++) {
        if (MT_SUCCESS !=
            norsf_tf_list[i](flash_start_addr, test_size_per_pattern, flash_test_size)) {
            HANDLE_FAIL;
        }
    }

    return MT_SUCCESS;
}

int spif_cmd_parsing(int argc, char * const argv[], spif_cmd_parsing_info_t *info)
{
    uint32_t i;

    #define ILL_CMD \
    ({ \
        printf("ERR: Illegal command (%d).\n",__LINE__);\
        return MT_SKIP;\
    })

    /* Initialize the memory test parameters..... */
    info->test_loops    = 1;


    /* Parse command flag for test range / test loops / mt error (non-)blocking mode */
    for(i=1 ; i<argc ;)    {
        if('-' != *argv[i])    ILL_CMD;

        if((strcmp(argv[i],"-loops") == 0) || (strcmp(argv[i],"-l") == 0)){
            if((i+1) >= argc) ILL_CMD;
            info->test_loops = simple_strtoul(argv[i+1], NULL, 10);
            info->test_loops = (info->test_loops==0)?1:(info->test_loops);
            i = i+2;
        }else if((strcmp (argv[i], "-range") == 0) || (strcmp (argv[i], "-r") == 0)){
            if(((i+1) >= argc) || ((i+2) >= argc)) ILL_CMD;
            info->mt_offset = simple_strtoul(argv[i+1], NULL, 16);
            info->mt_size= simple_strtoul(argv[i+2], NULL, 16);
            if(info->mt_size < SIZE_4KB) ILL_CMD;
            i = i+3;
            assign_test_range =1;
        }else if((strcmp (argv[i], "-ce") == 0) || (strcmp (argv[i], "-chip_erase") == 0)){
            info->test_case = 0xCE;
            i++;
        }else{
            ILL_CMD;
        }
    }
    return MT_SUCCESS;
}

int back_up_spif_data(uint32_t flash_addr_to_be_backup, uint32_t backup_size)
{
    volatile uint32_t *bootcode;
    volatile uint32_t *bk_buffer;

    bk_buffer = (uint32_t *)(uintptr_t)FLASH_BACKUP_ADDR;
    bootcode = (uint32_t *)(uintptr_t)flash_addr_to_be_backup;
    printf("Backing up %dMB/%dKB flash data: (0x%08x -> 0x%08x)... ",(backup_size/SIZE_1MB), backup_size/1024, (uint32_t)(uintptr_t)bootcode,(uint32_t)(uintptr_t)bk_buffer);
    flash_read((char *)bk_buffer, (ulong)bootcode, (ulong)backup_size);
    puts("done\n");
    return MT_SUCCESS;
}

int restore_spif_data(uint32_t flash_addr_to_be_backup, uint32_t backup_size)
{
    volatile uint32_t *bootcode;
    volatile uint32_t *bk_buffer;
    int i;
    ulong addr = (ulong)flash_addr_to_be_backup;
    ulong size = (ulong)backup_size;

    printf("Recover %dMB/%dKB flash data (0x%08x -> 0x%08x) ... ", (backup_size/SIZE_1MB), backup_size/1024, (uint32_t)(uintptr_t)FLASH_BACKUP_ADDR,(uint32_t)(uintptr_t)addr);
    flash_sect_erase(addr, backup_size);
    flash_write((char *)(uintptr_t)FLASH_BACKUP_ADDR, addr, size);

    bk_buffer = (uint32_t *)(uintptr_t)FLASH_BACKUP_ADDR;

    bootcode = (uint32_t *)(uintptr_t)DST_DATA_ADDR;
    flash_read((char *)bootcode, addr, backup_size);
    for(i=0; i<backup_size; i=i+4){
        if(*bk_buffer != *bootcode){
        printf("#Recover flash data error: bk_addr(0x%08x):0x%08x != flash_addr(0x%08x):0x%08x\n",\
            (uint32_t)(uintptr_t)bk_buffer, *bk_buffer, (uint32_t)(uintptr_t)bootcode, *bootcode);
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

int _common_flash_test(uint32_t flash_addr_to_be_test, uint32_t test_size)
{
    if(MT_FAIL == back_up_spif_data(flash_addr_to_be_test, test_size)) return MT_FAIL;
    if(MT_FAIL == _flash_test(flash_addr_to_be_test, TEST_SIZE_PER_PATTREN, test_size)){
        return MT_FAIL;
    }

    if(MT_FAIL == restore_spif_data(flash_addr_to_be_test, test_size)){
        printf("[ERROR] No recover data\n");
        return MT_FAIL;
    }

    return MT_SUCCESS;
}
int do_serial_nor_test (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    int retcode=MT_SUCCESS;
    uint32_t i;
    spif_cmd_parsing_info_t cmd_info;
    uint32_t mt_size;
    uint32_t mt_start;
    uint32_t backup_size;
    uint32_t max_dram_size = readl(0x111100c)*1024*1024 - RSV_SPACE;

    g_mnor_test = spi_flash_probe(0, 0, 0, 0);
    if (!g_mnor_test) {
        printf("spi flash 0 probe failed\n");
        return 0;
    }

    assign_test_range=0;

    if(MT_FAIL == spif_cmd_parsing(argc, argv, &cmd_info)) return MT_SKIP;

    printf("[mnor_test]: spi nor chip size is %2dMB/%dKB\n",(uint32_t)(uintptr_t)(g_mnor_test->size/SIZE_1MB),(uint32_t)(uintptr_t)(g_mnor_test->size/1024));

    if(cmd_info.test_case == 0xCE) {
        return flash_chip_erase_test();
    }

    if(assign_test_range){
        mt_size = cmd_info.mt_size;
        mt_start= cmd_info.mt_offset + CONFIG_SYS_FLASH_BASE;
        backup_size = mt_size;
        printf("             backup_size is %2d MB / %d KB\n",backup_size/SIZE_1MB,backup_size/1024);
        printf("             test_size   is %2d MB / %d KB\n",mt_size/SIZE_1MB,mt_size/1024);

        if (mt_size >= max_dram_size) {
            backup_size = 16*1024*1024;
            printf("[Warning] Assigned test size is %d MB, DRAM_Size is %d MB => Only backup 16MB flash content\n", (mt_size/SIZE_1MB), (max_dram_size/SIZE_1MB));
        }


        /* Step1: Back up and verify the assigned spif area data. */
        back_up_spif_data(mt_start, backup_size);

        /* Step2: Do mflash_test */
        for(i=0;i<cmd_info.test_loops;i++){
            if(MT_FAIL == _flash_test(mt_start, TEST_SIZE_PER_PATTREN, mt_size)){
                printf("[ERROR] No recover other data over the Assigned Area!\n");
                return MT_FAIL;
            }
            printf("\n[mnor_test] %d runs OK\n",i+1);
        }

        /* Step3: Copy back and verify data into the flash. */
        retcode = restore_spif_data(mt_start, backup_size);
        if(MT_SUCCESS == retcode) goto restore_setting;

    }else{ //(!assign_test_range)
        mt_size = SIZE_2MB;

        for (i=0; i<cmd_info.test_loops; i++) {
            uint32_t mt_start_mb_list[] = {64*SIZE_1MB, 48*SIZE_1MB, 32*SIZE_1MB,
                                           18*SIZE_1MB, 16*SIZE_1MB, 8*SIZE_1MB,
                                            4*SIZE_1MB};
            uint32_t msl_idx;
            uint32_t part_num = 0;

            for (msl_idx=0; msl_idx < ARY_LEN(mt_start_mb_list); msl_idx++) {
                if (mt_start_mb_list[msl_idx] > g_mnor_test->size) {
                    continue;
                }

                mt_start = CONFIG_SYS_FLASH_BASE + mt_start_mb_list[msl_idx] - mt_size;
                printf("\nRegion(%d): 0x%08x ~ 0x%08x\n", part_num++, mt_start, (mt_start+mt_size));

                if (MT_FAIL == _common_flash_test(mt_start, mt_size)) {
                    return MT_FAIL;
                }
            }

            printf("\n[mnor_test] %d runs OK\n", i);
        }
    }

restore_setting:
    assign_test_range = 0;

    return retcode;
}

U_BOOT_CMD(
    mnor_test, 10, 1, do_serial_nor_test,
    "mnor_test  - do spi-nor flash engineer command. ",
    "-r/-range <start_offset> <test_size (64KB alignment)> [-l/-loops <test loops>]\n"\
    "-ce/-chip_erase: doing chip erase and check erased data as ALL-ONE\n"\
);

