#include <soc.h>
#include <plr_sections.h>
#include <cli/cli_access.h>
#include <string.h>

#ifndef SECTION_CLI_UTIL
    #define SECTION_CLI_UTIL
#endif

/***************************
  * Macros for DRAM memory test
  ***************************/
#define CLI_MT_CASE_ADR_ROT_TEST (1<<0)
#define CLI_MT_CASE_ULS_TEST     (1<<1)
#define CLI_MT_CASE_MDRAM_TEST   (1<<2)
#define CLI_MT_PASS              (0)
#define CLI_MT_FAIL              (1)
#define HANDLE_FAIL \
({ \
        printf("%s (%d) test failed.\n", __FUNCTION__,__LINE__);\
        return CLI_MT_FAIL;\
})

#define _DCache_flush_invalidate()  _soc.bios.dcache_writeback_invalidate_all()

extern void plat_memctl_dcache_flush(void);

/***************************
  * Macros for DRAM memory test
  ***************************/ 
//#define _DCache_flush_invalidate         plat_memctl_dcache_flush
#define WORD_SIZE	    (sizeof(u32_t))
#define CFG_DCACHE_SIZE (0x10)
#define PAT32_MASK      (0x1FFFFFFF)

/*********************************
  * DRAM prefetch machanism declaration
  * (DDR1/2/3 Combo PHY Controller)
  *********************************/ 
#define MCR_PREFETCH_INS_SIDE    (0x2)
#define MCR_PREFETCH_DATA_SIDE   (0x1)
#define MCR_PREFETCH_DIS_IMASK   (0xFF7FFFFF)
#define MCR_PREFETCH_DIS_DMASK   (0xFFBFFFFF)
#define MCR_PREFETCH_ENABLE_INS  (0x00800000)
#define MCR_PREFETCH_ENABLE_DATA (0x00400000)

/*********************
  * Memory Test structures
  *********************/ 
typedef struct {
	u8_t  patternNum;
	u32_t writeAddr;
	u32_t readAddr;
	u32_t testSize;
} memory_test_info_t;
enum CASE_NUMBER
{
	MT_CASE_BYTE=8,
	MT_CASE_HALF=16,
	MT_CASE_WORD=32,
};
static u32_t mt_patterns[] =
{
	0x89abcdef,
	0x5a5aa5a5,
	0x76543210, 
	0xa5a5a5a5,
	0x55555555, 
	0xaaaaaaaa,
	0x01234567, 
	0xfedcba98,
	0xffffffff,
	0xff00ff00,
	0x00ff00ff,
	0x0000ffff,
	0xffff0000,
};
const unsigned int line_toggle_pattern[] = {
	0xF0F0F0F0,
	0x0F0F0F0F,
	0xFF00FF00,
	0x00FF00FF,
	0x00000000,
	0xFFFFFFFF,
	0x5A5A5A5A,
	0xA5A5A5A5
};

const unsigned int toggle_pattern[] = {
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
	0xA5A55A5A,
};

SECTION_CLI_UTIL void 
error_printf(enum CASE_NUMBER err, u32_t pat,u32_t patTrans32, u32_t addr, u32_t readval)
{
	switch(err)
	{
		case MT_CASE_BYTE:
			puts("\n>>>>>Byte ERROR<<<<<\n");
			break;
		case MT_CASE_HALF:
			puts("\n>>>>>Half ERROR<<<<<\n");
			break;
		case MT_CASE_WORD:
			puts("\n>>>>>Word ERROR<<<<<\n");
			break;
	}
	printf("  mt_patterns=0x%x\n",pat);
	printf("  patTrans32=0x%x\n",patTrans32);
	printf("  *(0x%x)=0x%x\n",addr,readval);
}

SECTION_CLI_UTIL u32_t 
rotate_1bit(u32_t v) 
{
	return (((v) << 1) | ((v & 0x80000000)>>31));
}

SECTION_CLI_UTIL int 
word_access_rotate_test(memory_test_info_t *info)
{
	u8_t  patIdx;
	u32_t mt_pat32,patTrans32, i;
	volatile u32_t *test_addr_word;

	for(patIdx=0; patIdx<info->patternNum ; patIdx++)
	{
		mt_pat32 = mt_patterns[patIdx];
		_DCache_flush_invalidate();

		/*Write Word*/
		test_addr_word = (volatile u32_t *)(info->writeAddr);
		for(i=0;i<info->testSize/WORD_SIZE;i++)
		{
			patTrans32 = (((u32_t)test_addr_word & PAT32_MASK) ^ rotate_1bit(mt_pat32));
			*(test_addr_word++) = patTrans32;
		}
		
		/*Read Word*/
		test_addr_word = (volatile u32_t *)(info->readAddr);
		for(i=0;i<info->testSize/WORD_SIZE;i++)
		{
			patTrans32 = (((u32_t)test_addr_word & PAT32_MASK) ^ rotate_1bit(mt_pat32));
			if(*(test_addr_word) != patTrans32)
			{
				error_printf(MT_CASE_WORD,mt_pat32,patTrans32,(u32_t)test_addr_word,*(test_addr_word));			 
				HANDLE_FAIL;
			}
			test_addr_word++;
		}
	}
	puts("Word_OK ");
	return CLI_MT_PASS;
}
SECTION_CLI_UTIL int 
half_access_rotate_test(memory_test_info_t *info)
{
	u8_t  patIdx;
	u32_t patTrans32, mt_pat32,i;
	u16_t wr16_1, wr16_2, rd16_1, rd16_2;
	volatile u16_t *test_addr_half;

	for(patIdx=0; patIdx<info->patternNum ; patIdx++)
	{		
		mt_pat32 = mt_patterns[patIdx];
		_DCache_flush_invalidate();

		/*Write half-word*/
		test_addr_half = (volatile u16_t *)(info->writeAddr);
		for(i=0;i<info->testSize/WORD_SIZE;i++)
		{
			patTrans32 = (((u32_t)test_addr_half & PAT32_MASK) ^ rotate_1bit(mt_pat32));
			wr16_1 = (patTrans32 & 0xffff);
			wr16_2 = (patTrans32>>16) & 0xffff;
			*(test_addr_half++) = wr16_1;
			*(test_addr_half++)= wr16_2;
		}
		
		/*Read half-word*/
		test_addr_half = (volatile u16_t *)(info->readAddr);
		for(i=0;i<info->testSize/WORD_SIZE;i++)
		{
			patTrans32 = (((u32_t)test_addr_half & PAT32_MASK) ^ rotate_1bit(mt_pat32));
			wr16_1 = (patTrans32 & 0xffff);
			wr16_2 = (patTrans32>>16) & 0xffff;
			
			rd16_1 = *(test_addr_half);
			if(rd16_1 != wr16_1)
			{
				error_printf(MT_CASE_HALF,mt_pat32,wr16_1,(u32_t)test_addr_half,rd16_1);
				HANDLE_FAIL;
			}			
			rd16_2 = *(++test_addr_half);
			if(rd16_2 != wr16_2)
			{
				error_printf(MT_CASE_HALF,mt_pat32,wr16_2,(u32_t)test_addr_half,rd16_2);
				HANDLE_FAIL;
			}
			test_addr_half++;
		}
	}
	puts("Half_OK ");
	return CLI_MT_PASS;
}
SECTION_CLI_UTIL int 
byte_access_rotate_test(memory_test_info_t *info)
{
	u8_t  patIdx;
	u32_t patTrans32,mt_pat32,i;
	u8_t  wr8_1,wr8_2,wr8_3,wr8_4,rd8_1,rd8_2,rd8_3,rd8_4;
	volatile u8_t *test_addr_byte;

	for(patIdx=0; patIdx<info->patternNum ; patIdx++)
	{	
		mt_pat32 = mt_patterns[patIdx];
		_DCache_flush_invalidate();
		
		/*Write Byte*/
		test_addr_byte = (volatile u8_t *)(info->writeAddr);
		for(i=0 ; i<info->testSize/WORD_SIZE; i++)
		{
			patTrans32 = (((u32_t)test_addr_byte & PAT32_MASK) ^ rotate_1bit(mt_pat32));
			wr8_1 = (patTrans32>>0) & 0xffff;
			wr8_2 = (patTrans32>>8) & 0xffff;
			wr8_3 = (patTrans32>>16)& 0xffff;
			wr8_4 = (patTrans32>>24)& 0xffff;
			*(test_addr_byte++) = wr8_1;
			*(test_addr_byte++) = wr8_2;
			*(test_addr_byte++) = wr8_3;
			*(test_addr_byte++) = wr8_4;
		}
		
		/*Read Byte*/
		test_addr_byte = (volatile u8_t *)(info->readAddr);
		for(i=0 ; i<info->testSize/WORD_SIZE; i++)
		{
			patTrans32 = (((u32_t)test_addr_byte & PAT32_MASK) ^ rotate_1bit(mt_pat32));
			wr8_1 = (patTrans32>>0) & 0xffff;
			wr8_2 = (patTrans32>>8) & 0xffff;
			wr8_3 = (patTrans32>>16)& 0xffff;
			wr8_4 = (patTrans32>>24)& 0xffff;

			rd8_1 = *(test_addr_byte);
			if(rd8_1 != wr8_1)
			{
				error_printf(MT_CASE_BYTE, mt_pat32,wr8_1,(u32_t)test_addr_byte,rd8_1); 	
				HANDLE_FAIL;
			}		
			rd8_2 = *(++test_addr_byte);
			if(rd8_2 != wr8_2)
			{
				error_printf(MT_CASE_BYTE, mt_pat32,wr8_2,(u32_t)test_addr_byte,rd8_2);
				HANDLE_FAIL;
			}			
			rd8_3 = *(++test_addr_byte);
			if(rd8_3 != wr8_3)
			{
				error_printf(MT_CASE_BYTE, mt_pat32,wr8_3,(u32_t)test_addr_byte,rd8_3);
				HANDLE_FAIL;
			}			
			rd8_4 = *(++test_addr_byte);
			if(rd8_4 != wr8_4)
			{
				error_printf(MT_CASE_BYTE, mt_pat32,wr8_4,(u32_t)test_addr_byte,rd8_4);
				HANDLE_FAIL;
			}
			test_addr_byte++;
		}
	}
	puts("Byte_OK\n");
	return CLI_MT_PASS;
}
SECTION_CLI_UTIL void 
disable_DRAM_prefech(u32_t side_id)
{
	volatile u32_t *reg_mcr;

	reg_mcr = (volatile u32_t *)0xB8001000;

	if( side_id & MCR_PREFETCH_INS_SIDE )
		*reg_mcr =*reg_mcr & ((unsigned int)MCR_PREFETCH_DIS_IMASK);

	if( side_id & MCR_PREFETCH_DATA_SIDE)
		*reg_mcr =*reg_mcr & ((unsigned int)MCR_PREFETCH_DIS_DMASK);
}

SECTION_CLI_UTIL void 
enable_DRAM_prefech(u32_t side_id)
{
	volatile u32_t *reg_mcr;

	reg_mcr = (volatile u32_t *)0xB8001000;

	if( side_id & MCR_PREFETCH_INS_SIDE )
	{
		disable_DRAM_prefech(MCR_PREFETCH_INS_SIDE);
		*reg_mcr = *reg_mcr | MCR_PREFETCH_ENABLE_INS;
	}
	
	if( side_id & MCR_PREFETCH_DATA_SIDE )
	{
		disable_DRAM_prefech(MCR_PREFETCH_DATA_SIDE);
		*reg_mcr = *reg_mcr | MCR_PREFETCH_ENABLE_DATA;
	}
}

const char *char_wr_cache_case[] = {
	"[Uncache-W/Uncache-R]",
	"[Uncache-W/Cache-R  ]",
	"[Cache-W  /Cache-R  ]",
};
SECTION_CLI_UTIL int 
dram_adr_rotate_test(u32_t startAddr, u32_t size)
{
	memory_test_info_t mt_info;
	u8_t  caseIdx;
	u32_t dram_write_addr[3];
	u32_t dram_read_addr[3];

	dram_write_addr[0] = dram_write_addr[1] = dram_read_addr[0] = (startAddr&PAT32_MASK)|0xA0000000;
	dram_write_addr[2] = dram_read_addr[1] = dram_read_addr[2] = (startAddr&PAT32_MASK)|0x80000000;
	mt_info.patternNum = 2;
	mt_info.testSize = size;

	for(caseIdx=0 ; caseIdx<3 ; caseIdx++)
	/* Case0: Uncached-W / Uncached-R
	  * Case1: Uncached-W / Cached-R
	  * Case2: Cached-W / Cached-R
	  */		
	{
		printf("%s ",char_wr_cache_case[caseIdx]);
		mt_info.writeAddr = dram_write_addr[caseIdx];
		mt_info.readAddr  = dram_read_addr[caseIdx];
		if(CLI_MT_FAIL == word_access_rotate_test(&mt_info))
			return CLI_MT_FAIL;
		if(CLI_MT_FAIL == half_access_rotate_test(&mt_info))
			return CLI_MT_FAIL;
		if(CLI_MT_FAIL == byte_access_rotate_test(&mt_info))
			return CLI_MT_FAIL;
		if(caseIdx==2)
		{
			/*Data Prefetch*/
			puts("(Data Prefetch) ");
			enable_DRAM_prefech(MCR_PREFETCH_DATA_SIDE);
			if(CLI_MT_FAIL == word_access_rotate_test(&mt_info))
				return CLI_MT_FAIL;
			if(CLI_MT_FAIL == half_access_rotate_test(&mt_info))
				return CLI_MT_FAIL;
			if(CLI_MT_FAIL == byte_access_rotate_test(&mt_info))
				return CLI_MT_FAIL;
			disable_DRAM_prefech( MCR_PREFETCH_INS_SIDE | MCR_PREFETCH_DATA_SIDE ); 	

			/*Instuction Prefetch*/
			puts("(Instuction Prefetch) ");
			enable_DRAM_prefech(MCR_PREFETCH_INS_SIDE);
			if(CLI_MT_FAIL == word_access_rotate_test(&mt_info))
				return CLI_MT_FAIL;
			if(CLI_MT_FAIL == half_access_rotate_test(&mt_info))
				return CLI_MT_FAIL;
			if(CLI_MT_FAIL == byte_access_rotate_test(&mt_info))
				return CLI_MT_FAIL;
			disable_DRAM_prefech(MCR_PREFETCH_INS_SIDE|MCR_PREFETCH_DATA_SIDE);
			
			/*Data & Instuction Prefetch*/
			puts("(Data & Instuction Prefetch) ");
			enable_DRAM_prefech(MCR_PREFETCH_DATA_SIDE|MCR_PREFETCH_INS_SIDE);
			if(CLI_MT_FAIL == word_access_rotate_test(&mt_info))
				return CLI_MT_FAIL;
			if(CLI_MT_FAIL == half_access_rotate_test(&mt_info))
				return CLI_MT_FAIL;
			if(CLI_MT_FAIL == byte_access_rotate_test(&mt_info))
				return CLI_MT_FAIL;
			disable_DRAM_prefech(MCR_PREFETCH_INS_SIDE|MCR_PREFETCH_DATA_SIDE);
		}
	}
	return CLI_MT_PASS;
}

#pragma pack(1)
/* Data structures used for testing unaligned load/store operations. */
typedef struct{
	unsigned char c1;
	unsigned int w;
} t_off_1;

typedef struct{
	unsigned char c1;
	unsigned char c2;
	unsigned int w;
} t_off_2;
typedef struct{
	unsigned char c1;
	unsigned char c2;
	unsigned char c3;
	unsigned int w;
} t_off_3;

SECTION_CLI_UTIL int 
unaligned_test(u32_t startAddr, u32_t area_size)
{
	u32_t i, j;
	volatile t_off_1 *off1;
	volatile t_off_2 *off2;
	volatile t_off_3 *off3;
	u32_t sizePerPattern=0x10000;

	off1 = (t_off_1 *)startAddr;
	off2 = (t_off_2 *)startAddr;
	off3 = (t_off_3 *)startAddr;

	/* patterns loop */
	for(j=0; j<sizeof(mt_patterns)/sizeof(u32_t);j++)
	{
		_DCache_flush_invalidate();

		/* offset 1 bytes */
         	off1 = (t_off_1 *)(startAddr + (j*sizePerPattern)%(area_size));
		/* set value */
		for(i=0;i<(sizePerPattern/sizeof(t_off_1));i++){
			off1[i].c1 = 0xcc;
			off1[i].w = mt_patterns[j];
		}
		/* check data */
		for(i=0;i<(sizePerPattern/sizeof(t_off_1));i++){
			if((off1[i].w != mt_patterns[j]) || (off1[i].c1 != 0xcc)){
				printf("\noffset 1 error:addr(0x%x) write 0x%x, read 0x%x, c1(%02x)\n", (u32_t)&off1[i], mt_patterns[j], off1[i].w, off1[i].c1);
				HANDLE_FAIL;
			}
		}
	
		_DCache_flush_invalidate();
		/* offset 2 bytes */
         	off2 = (t_off_2 *)(startAddr + (j*sizePerPattern)%(area_size));
		/* set value */
		for(i=0;i<(sizePerPattern/sizeof(t_off_2));i++){
			off2[i].c1 = 0xcc;
			off2[i].c2 = 0xdd;
			off2[i].w = mt_patterns[j];
		}
		/* check data */
		for(i=0;i<(sizePerPattern/sizeof(t_off_2));i++){
			if(off2[i].w != mt_patterns[j] || (off2[i].c1 != 0xcc) || (off2[i].c2 != 0xdd)){
				printf("\noffset 2 error:addr(0x%x) write 0x%x, read 0x%x, c1(0x%x), c2(0x%x)\n",(u32_t)&off2[i], mt_patterns[j], off2[i].w, off2[i].c1, off2[i].c2);
				HANDLE_FAIL;
			}
		}

		_DCache_flush_invalidate();
		/* offset 3 bytes */
         	off3 = (t_off_3 *)(startAddr + (j*sizePerPattern)%(area_size));
		/* set value */
		for(i=0;i<(sizePerPattern/sizeof(t_off_3));i++){
			off3[i].c1 = 0xcc;
			off3[i].c2 = 0xdd;
			off3[i].c3 = 0xee;
			off3[i].w = mt_patterns[j];
		}
		/* check data */
		for(i=0;i<(sizePerPattern/sizeof(t_off_3));i++){
			if(off3[i].w != mt_patterns[j] ||(off3[i].c1 != 0xcc) || (off3[i].c2 != 0xdd)\
			 || (off3[i].c3 != 0xee)){
				printf("\noffset 3 error:addr(0x%x) write 0x%x, "\
					"read 0x%x, c1(%02x), c2(%02x), c3(%02x)\n", \
					(u32_t)&off1[i], mt_patterns[j], off3[i].w, off3[i].c1, \
					off3[i].c2, off3[i].c3);
				HANDLE_FAIL;
			}
		}
		printf("pattern[%d](0x%x) 0x%x pass\r", j, mt_patterns[j], (u32_t)off3);
	}
   	 printf("%s test passed.\r", __FUNCTION__);
	 return CLI_MT_PASS;
}


SECTION_CLI_UTIL int 
dram_normal_patterns(u32_t dram_start, u32_t dram_size, u32_t area_size)
{
    int i;
    int j;
    u32_t start_value;
    volatile u32_t *read_start;
    volatile u32_t *start;


    printf("=======start %s test=======\r", __FUNCTION__);
    for (i=0; i < (sizeof(mt_patterns)/sizeof(u32_t)); i++)
    {
		_DCache_flush_invalidate();

        /* write pattern*/
        start = (u32_t *)(dram_start + (i*dram_size)%(area_size));
        read_start = (u32_t *)(UADDR((u32_t)start+dram_size-4));
        for(j=0; j < dram_size; j=j+4)
        {
            *start = mt_patterns[i];
            start++;
        }

		_DCache_flush_invalidate();

        /* check data */  
        for(j=0; j < dram_size; j=j+4)
        {
	    start_value = (*read_start);
            if(start_value != mt_patterns[i])
            {
                 printf("\naddr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
                        (u32_t)read_start , start_value, mt_patterns[i],  __FUNCTION__, __LINE__);
                 HANDLE_FAIL;
            }
            read_start--;
        }
		printf("pattern[%d](0x%x) 0x%x pass\r", i, mt_patterns[i], (u32_t)start);
    }
    printf("%s test completed.\r", __FUNCTION__);
    return CLI_MT_PASS;
}


SECTION_CLI_UTIL int 
dram_com_addr_rot(u32_t dram_start, u32_t dram_size, u32_t area_size)
{
    int i;
    int j;
    u32_t start_value;
    u32_t read_start_addr;
    volatile u32_t *start;
    volatile u32_t *read_start;

    printf("=======start %s test=======\r", __FUNCTION__);
    for (i=0; i < 32; i=i+4)
    {
		_DCache_flush_invalidate();
        /* write pattern*/
        start		= (u32_t *)(dram_start + ((i/4)*dram_size)%(area_size));
        read_start 	= (u32_t *)((u32_t)start + dram_size-4);
        read_start_addr = ((u32_t)start);
        start = (u32_t *)(UADDR(((u32_t)start)));
        for(j=0; j < dram_size; j=j+4)
        {
            *start = ~(read_start_addr << i);
            start++;
            read_start_addr = read_start_addr + 4 ;
        }

		_DCache_flush_invalidate();
		read_start_addr = ((u32_t)read_start);
        /* check data reversing order */  
        for(j=0; j < dram_size; j=j+4)
        {
            start_value = (*read_start);
            if(start_value != (~(read_start_addr << i)))
            {
                 printf("\ndecr addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
                        (u32_t)read_start , start_value, ~((read_start_addr) << i), \
                        __FUNCTION__, __LINE__);
                 HANDLE_FAIL;
            }
            read_start_addr = read_start_addr - 4;
            read_start--;
        }

		read_start_addr += 4;
        read_start++;

        /* check data sequential order */  
        for(j=0; j < dram_size; j=j+4)
        {
            start_value = (*read_start);
            if(start_value != (~(read_start_addr << i)))
            {
                 printf("\nseq addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
                        (u32_t)read_start , start_value, ~((read_start_addr) << i), \
                        __FUNCTION__, __LINE__);
                 HANDLE_FAIL;
            }
            read_start_addr = read_start_addr + 4;
            read_start++;
        }
		printf("~rotate %d 0x%x passed\r", i, (u32_t)start);
    }
    printf("%s test completed.\r", __FUNCTION__);
    return CLI_MT_PASS;
}

SECTION_CLI_UTIL int 
dram_walking_of_1(u32_t dram_start, u32_t dram_size, u32_t area_size)
{
	int i;
	int j;
	u32_t walk_pattern;
	u32_t start_value;
	volatile u32_t *read_start;
	volatile u32_t *start;

	printf("=======start %s test=======\r", __FUNCTION__);
	for (i=0; i < 32; i++)
	{
		_DCache_flush_invalidate();

		/* generate pattern */
		walk_pattern = (1 << i);

		/* write pattern*/
		start = (u32_t *)(dram_start + (i*dram_size)%(area_size));
		read_start = (u32_t *)(UADDR((u32_t)start+dram_size-4));
		for(j=0; j < dram_size; j=j+4)
		{
			*start = walk_pattern;
			start++;
		}

		_DCache_flush_invalidate();

		/* check data */  
		for(j=0; j < dram_size; j=j+4)
		{
		start_value = (*read_start);
			if(start_value != walk_pattern)
			{
				 printf("\naddr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
						(u32_t)read_start , start_value, walk_pattern,	__FUNCTION__, __LINE__);
				 HANDLE_FAIL;
			}
			read_start--;
		}
		printf("pattern[%d](0x%x) 0x%x passed\r", i, walk_pattern, (u32_t)start);
	}
	printf("%s test completed.\r", __FUNCTION__);
	return CLI_MT_PASS;
}

SECTION_CLI_UTIL int 
dram_walking_of_0(u32_t dram_start, u32_t dram_size, u32_t area_size)
{
	int i;
	int j;
	u32_t start_value;
	u32_t walk_pattern;
	volatile u32_t *start;
	volatile u32_t *read_start;

	printf("=======start %s test=======\r", __FUNCTION__);
	for (i=0; i < 32; i++)
	{
		_DCache_flush_invalidate();

		/* generate pattern */
		walk_pattern = ~(1 << i);

		/* write pattern*/
		start = (u32_t *)(dram_start + (i*dram_size)%(area_size));
		read_start = (u32_t *)(UADDR(((u32_t)start)+dram_size-4));
		for(j=0; j < dram_size; j=j+4)
		{
			*start = walk_pattern;
			start++;
		}

		_DCache_flush_invalidate();

		/* check data */  
		for(j=0; j < dram_size; j=j+4)
		{
			start_value = (*read_start);
			if(start_value != walk_pattern)
			{
				 printf("\naddr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
						(u32_t)read_start , start_value, walk_pattern,	__FUNCTION__, __LINE__);
				 HANDLE_FAIL;
			}
			read_start--;
		}
		printf("pattern[%d](0x%x) 0x%x passed\r", i, walk_pattern, (u32_t)start);
	}
	printf("%s test completed.\r", __FUNCTION__);
	return CLI_MT_PASS;
}

SECTION_CLI_UTIL int 
dram_addr_rot(u32_t dram_start, u32_t dram_size, u32_t area_size)
{
	int i;
	int j;
	u32_t start_value;
	u32_t read_start_addr;
	volatile u32_t *start;
	volatile u32_t *read_start;

	printf("=======start %s test=======\r", __FUNCTION__);
	for (i=0; i < 32; i=i+4)
	{
		_DCache_flush_invalidate();
		/* write pattern*/
		start		= (u32_t *)(dram_start + ((i/4)*dram_size)%(area_size));
		read_start	= (u32_t *)((u32_t)start + dram_size-4);
		read_start_addr = ((u32_t)start);
		start = (u32_t *)(UADDR(((u32_t)start)));
		for(j=0; j < dram_size; j=j+4)
		{
			*start = (read_start_addr << i);
			start++;
			read_start_addr = read_start_addr + 4 ;
		}

		_DCache_flush_invalidate();
		read_start_addr = ((u32_t)read_start);
		/* check data reversing order */  
		for(j=0; j < dram_size; j=j+4)
		{
			start_value = (*read_start);
			if(start_value != ((read_start_addr) << i))
			{
				 printf("\ndecr addr(0x%x): 0x%x != pattern(0x%x) %s, %d i=%d j=0x%x\n",\
						(u32_t)read_start , start_value, ((read_start_addr) << i),\
						__FUNCTION__, __LINE__, i,j);
				 HANDLE_FAIL;
			}
			read_start_addr = read_start_addr - 4;
			read_start--;
		}

		read_start_addr += 4;
		read_start++;

		/* check data sequential order */  
		for(j=0; j < dram_size; j=j+4)
		{
			start_value = (*read_start);
			if(start_value != ((read_start_addr) << i))
			{
				 printf("\nseq addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
						(u32_t)read_start , start_value, ((read_start_addr) << i), \
						__FUNCTION__, __LINE__);
				 HANDLE_FAIL;
			}
			read_start_addr = read_start_addr + 4;
			read_start++;
		}
		printf("rotate %d 0x%x passed\r", i, (u32_t)start);
	}
	printf("%s test completed.\r", __FUNCTION__);
	return CLI_MT_PASS;
}


/*
 * write two half-words and read word.
 */
SECTION_CLI_UTIL int 
dram_half_word_access(u32_t dram_start, u32_t dram_size, u32_t area_size)
{
    int i;
    int j;
    u16_t h_word;
    u32_t start_value;
    volatile u16_t *start_h;
    volatile u32_t *start_w;

    printf("=======start %s test=======\r", __FUNCTION__);
    for (i=0; i < (sizeof(mt_patterns)/sizeof(u32_t)); i++)
    {
	_DCache_flush_invalidate();

        /* write half-word pattern*/
        start_h = (u16_t *)(dram_start+ (i*dram_size)%(area_size));
        start_w = (u32_t *)(UADDR(dram_start+ (i*dram_size)%(area_size)+dram_size-4));
        for(j=0; j < dram_size; j=j+4)
        {
            h_word = (u16_t)(mt_patterns[i]);
            *(start_h+1) = h_word;
            h_word = (u16_t)(mt_patterns[i] >> 16);
            *start_h = h_word;
            start_h+=2;
        }

		_DCache_flush_invalidate();

        /* read word and check data */  
        for(j=0; j < dram_size; j=j+4)
        {
            start_value = (*start_w);
            if(start_value != mt_patterns[i])
            {
                 printf("\naddr:0x%x(0x%x) != pattern(0x%x) %s, %d\n",\
                       (u32_t)start_w, start_value, mt_patterns[i],  __FUNCTION__, __LINE__);
                 HANDLE_FAIL;
            }
            start_w--;
        }
		printf("pattern[%d](0x%x) 0x%x pass\r", i, mt_patterns[i], (u32_t)start_h);
    }
    printf("%s test completed.\r", __FUNCTION__);
    return CLI_MT_PASS;
}

SECTION_CLI_UTIL int 
dram_byte_access(u32_t dram_start, u32_t dram_size, u32_t area_size)
{
    int i;
    int j;
    u8_t byte;
    u32_t start_value;
    volatile u8_t *start_b;
    volatile u32_t *start_w;

    printf("=======start %s test=======\r", __FUNCTION__);
    for (i=0; i < (sizeof(mt_patterns)/sizeof(u32_t)); i++)
    {
		_DCache_flush_invalidate();

        /* write byte pattern*/
        start_w = (u32_t *)(UADDR(dram_start+(i*dram_size)%(area_size)+dram_size-4));
        start_b = (u8_t *)(dram_start+(i*dram_size)%(area_size));
        for(j=0; j < dram_size; j=j+4)
        {
            byte = (u8_t)(mt_patterns[i]);
            *(start_b+3) = byte;
            byte = (u8_t)(mt_patterns[i] >> 8);
            *(start_b+2) = byte;
            byte = (u8_t)(mt_patterns[i] >> 16);
            *(start_b+1) = byte;
            byte = (u8_t)(mt_patterns[i] >> 24);
            *(start_b) = byte;
            start_b+=4;
        }

		_DCache_flush_invalidate();

        /* read word and check data */  
        for (j=0; j < dram_size; j=j+4)
        {
            start_value = *start_w;
            if (start_value != mt_patterns[i])
            {
                 printf("\naddr:0x%x(0x%x) != pattern(0x%x) %s, %d\n",\
                       (u32_t)start_w, start_value, mt_patterns[i],  __FUNCTION__, __LINE__);
                 HANDLE_FAIL;
            }
            start_w--;
        }
		printf("pattern[%d](0x%x) 0x%x pass\r", i, mt_patterns[i], (u32_t)start_b);
    }
    printf("%s test completed.\r", __FUNCTION__);
    return CLI_MT_PASS;
}

SECTION_CLI_UTIL int 
mdram_test(u32_t start_addr, u32_t size)
{
	int retcode = CLI_MT_PASS;
	retcode = dram_normal_patterns(start_addr,(u32_t)0x10000,size);
	/*retcode = dram_com_addr_rot(start_addr,(u32_t)0x10000,size);
	retcode = dram_walking_of_0(start_addr,(u32_t)0x10000,size);
	retcode = dram_walking_of_1(start_addr,(u32_t)0x10000,size);
	retcode = dram_addr_rot(start_addr,(u32_t)0x10000,size);
	retcode = dram_byte_access(start_addr,(u32_t)0x10000,size);
	retcode = dram_half_word_access(start_addr,(u32_t)0x10000,size);*/
	//retcode = memcpy_test(start_addr,(u32_t)0x10000,size);
	//retcode = cache_flush_test(start_addr,(u32_t)0x10000,size);
	if(CLI_MT_FAIL == retcode)
	{
		printf("%s Failed!!!\n",__FUNCTION__);
	}
	return retcode;
}

SECTION_CLI_UTIL cli_cmd_ret_t
cli_std_mt(const void *user, u32_t argc, const char *argv[]) {
    if (argc<3) return CCR_INCOMPLETE_CMD;

    u32_t addr=atoi(argv[1]);
    u32_t size=atoi(argv[2]);
    mdram_test(addr, size);
    return CCR_OK;
}

cli_top_node(mt, cli_std_mt);
    cli_add_help(mt, "mt <addr> <len>");

