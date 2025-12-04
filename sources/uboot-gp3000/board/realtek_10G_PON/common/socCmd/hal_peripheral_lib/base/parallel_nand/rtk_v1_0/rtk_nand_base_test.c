/******************************************************************************
 * $Id: rtk_nand.c 337905 2010-10-18 01:32:27Z alexchang2131 $
 * drivers/mtd/nand/rtk_nand.c
 * Overview: Realtek NAND Flash Controller Driver
 * Copyright (c) 2008 Realtek Semiconductor Corp. All Rights Reserved.
 * Modification History:
 *    #000 2010-07-02 AlexChang   create file
 *
 *
 *******************************************************************************/

#include <common.h>
#include "basic_types.h"
#include "rtk_nand.h"
#include "p_mtd.h"
//#include <asm/cache.h>
//#include <linux/mtd/partitions.h>
//#include <linux/slab.h>
//#include <linux/sysctl.h>
//#include <linux/sched.h>
//#include <linux/list.h>
//#include <linux/pm.h>
//#include <asm/io.h>
//#include <linux/mtd/rtk_nand_reg.h>
//#include <linux/mtd/rtk_nand.h>
//#include <linux/bitops.h>
//#include <mtd/mtd-abi.h>
//#include <asm/r4kcache.h>
//#include <linux/jiffies.h>
//#include <linux/delay.h>
//#include <linux/time.h>
//#include <linux/proc_fs.h>
//#include <linux/string.h>

//#include <asm/mach-rtk_dmp/platform.h>
#include "nand_reg.h"
//#include <asm/arch/platform_lib/board/gpio.h>

//#include <linux/platform_device.h>
//#include <linux/interrupt.h>


//#include <stdlib.h>
//#include <linux/mtd/rtk_cp_reg.h>
//#include <linux/mtd/rtk_mcp_reg.h>
//#define RTK_NAND_INTERRUPT (0)


#define BANNER  "Realtek NAND Flash Driver"
#define VERSION  "$Id: rtk_nand.c 337905 2010-10-18 01:32:27Z alexchang2131 $"
#define RTK_NAND_INTERRUPT	(0)
#define MTDSIZE	(sizeof (struct mtd_info) + sizeof (struct nand_chip))
#define MAX_PARTITIONS	16
#define BOOTCODE	16*1024*1024	//16MB
#define FACT_PARAM_SIZE 4*1024*1024 //4MB for TV factory parameter use.
#define DRIVER_NAME "rtkNF"
#define NF_INIT	(0)
#define NF_READ_TRIGGER 	(1)
#define NF_WRITE_TRIGGER 	(2)
#define NF_ERASE_TRIGGER 	(3)
#define NF_POLL_STATUS 	(4)
#define NF_WAKE_UP	(5)

#define NF_AUTO_IRQ_MASK	(0x08)
#define NF_DMA_IRQ_MASK		(0x10)
#define NF_POLL_IRQ_MASK	(0x80)
#define NF_XFER_IRQ_MASK	(0x04)
#define NF_IRQ_MASK	(NF_XFER_IRQ_MASK|NF_AUTO_IRQ_MASK|NF_DMA_IRQ_MASK|NF_POLL_IRQ_MASK)

#define NF_RESET_IRQ	REG_WRITE_U32(REG_NAND_ISREN,(0xff<<1|0x00));\
						REG_WRITE_U32(REG_NAND_ISR,(0xff<<1|0x00))
#define VIR_TO_PHY(x) (x & (~0xa0000000))

//#define NF_RESET_IRQ	NULL

#define NF_ENABLE_IRQ(mask)	REG_WRITE_U32(REG_NAND_ISREN,(mask|0x01))


static int g_nfFlag = NF_INIT ;

#define CLR_REG_NAND_ISR REG_WRITE_U32(REG_NAND_ISR,(0xFF<<1))

#define ISR_EN_READ (0x10)
#define ISR_EN_WRITE (0x08)

#define MODULE_AUTHOR(x)	/* x */
#define MODULE_DESCRIPTION(x)	/* x */

#define OTP_K_S                 2048
#define OTP_K_H					2304
#define OTP_K_N                 2560

//static struct semaphore sem_NF_CARDREADER;

static int g_id_chain = 0;
static int g_enReadRetrial = 0;
static unsigned char regVal1=0;
static unsigned char regVal2=0;
static unsigned char regVal3=0;
static unsigned char regVal4=0;

static int read_retry_toshiba_cnt[]={0x00, 0x00, 0x04, 0x7c, 0x78, 0x74, 0x08};

static int read_retry_toshiba_cnt_new[8][5]={
        {0x00, 0x00, 0x00, 0x00, 0x00},
        {0x04, 0x04, 0x7c, 0x7e, 0x00},
        {0x00, 0x7c, 0x78, 0x78, 0x00},
        {0x7c, 0x76, 0x74, 0x72, 0x00},
        {0x08, 0x08, 0x00, 0x00, 0x00},
        {0x0b, 0x7e, 0x76, 0x74, 0x00},
        {0x10, 0x76, 0x72, 0x70, 0x00},
        {0x02, 0x7c, 0x7e, 0x70, 0x00}
};

static unsigned char nRun=0;
static unsigned char nInc=0;
volatile unsigned int monflg=0, g_ecc_select = 0;

#define NAND_BUFFER_SRAM

#ifdef NAND_BUFFER_SRAM
u8* nand_write_buf_rtk = 0xbfe02000;
u8* nand_read_buf_rtk = 0xbfe00000;
#else
u8 nand_write_buf_rtk[4096]__attribute__ ((aligned (0x10)));
#endif

//#define dbg_parallel_nand printf
#define dbg_parallel_nand(...)


int  rtk_nand_init (void);
static void rtk_xfer_GetParameter(void);
static void rtk_xfer_SetParameter(unsigned char val1,unsigned char val2,unsigned char val3,unsigned char val4);
static void rtk_set_feature_micron(int P1);

//static void rtk_SetVal( char val1, char val2, char val3, char val4);
static void rtk_SetReadRetrialMode(unsigned int run);
static int rtk_read_ecc_page_reTrial (struct mtd_info *mtd, u16 chipnr, unsigned int page, u_char *data_buf, u_char *oob_buf);

extern int pcb_mgr_get_enum_info_byname(char *enum_name, unsigned long long *value);
//extern platform_info_t platform_info;

extern int add_mtd_partitions(struct mtd_info *master, const struct mtd_partition *parts, int nbparts);
extern int parse_mtd_partitions(struct mtd_info *master, const char **types, struct mtd_partition **pparts, struct mtd_part_parser_data *data);
extern int add_mtd_device(struct mtd_info *mtd);

#ifdef CONFIG_CMD_KEY_BURNING
extern int OTP_Get_Byte(int offset, unsigned char *ptr, unsigned int cnt);
#endif
static int swap_endian(unsigned int input);
//extern int del_mtd_partitions(struct mtd_info *master);
#if RTK_NAND_INTERRUPT
static wait_queue_head_t	g_irq_waitq;

//#define RTK_WAIT_EVENT wait_event_interruptible(g_host->irq_waitq,g_nfFlag==NF_WAKE_UP)
#define RTK_WAIT_EVENT wait_event_interruptible(g_irq_waitq,g_nfFlag==NF_WAKE_UP)

#define CHECK_IRQ(flag,reg,mask,value) ((g_nfFlag==flag)&&((REG_READ_U32(reg)&mask)==value))
#define CHECK_REG(reg,mask,value) (REG_READ_U32(reg)&mask==value)
//#define CLEAR_IRQ(reg,mask)	(REG_WRITE_U32(reg,(REG_READ_U32(reg)|mask)&0xfffffffe))
#define CLEAR_IRQ(reg,mask)	(REG_WRITE_U32(reg,mask))
#define NF_AUTO_TRIG_ISR	(0x08)
#define NF_DMA_ISR	(0x10)
#define NF_POLL_STATUS_ISR	(0x80)
#endif

#define RETRY_COUNT	(0x20)

//Add for GPIO Setting
#define BIT(x) (1 << (x))
#define SETBITS(x,y) ((x) |= (y))
#define CLEARBITS(x,y) ((x) &= (~(y)))
#define SETBIT(x,y) SETBITS((x), (BIT((y))))
#define CLEARBIT(x,y) CLEARBITS((x), (BIT((y))))
static unsigned int g_regGPIODirBase = 0;
static unsigned int g_regGPIOOutBase = 0;
static struct rtk_nand_host *g_host;

static unsigned int g_NFWP_no = 0;
static unsigned int g_NFWP_index = 0;

static unsigned int g_NFWP_value_INV = 0;
static unsigned int g_NFWP_value_en = 0;
static unsigned int g_NFWP_value_dis = 0;



#define NF_GPIO_OUT	(1)
#define NF_GPIO_IN	(0)
//#define NF_WP_ENABLE	(0)
//#define NF_WP_DISABLE	(1)

static unsigned int g_BootcodeSize = 0;
static unsigned int g_Factory_param_size = 0;
static int g_WP_en = 0;
unsigned int g_NF_pageSize = 0;
unsigned char g_isRandomize = 0;

#define NF_OOB_SIZE 512

const char *ptypes[] = {"cmdlinepart", NULL};

/* nand driver low-level functions */
static int rtk_read_oob(struct mtd_info *mtd, u16 chipnr, int page, int len, u_char *buf);
static int rtk_read_ecc_page_ori(struct mtd_info *mtd, u16 chipnr, unsigned int page,
			u_char *data, u_char *oob_buf, u16 cp_mode, u16 cp_first, size_t cp_len);
static int rtk_write_oob(struct mtd_info *mtd, u16 chipnr, int page, int len, const u_char *buf);
static int rtk_write_ecc_page_ori(struct mtd_info *mtd, u16 chipnr, unsigned int page,
			const u_char *data, const u_char *oob_buf, int isBBT);
static int rtk_erase_block_ori(struct mtd_info *mtd, u16 chipnr, int page);

static int rtk_read_ecc_page(struct mtd_info *mtd, u16 chipnr, unsigned int page,
			u_char *data, u_char *oob_buf);
static int rtk_write_ecc_page(struct mtd_info *mtd, u16 chipnr, unsigned int page,
			const u_char *data, const u_char *oob_buf);
static int rtk_write_ecc_512(struct mtd_info *mtd, u16 chipnr, unsigned int page,
			const u_char *data, const u_char *oob_buf);

static int rtk_erase_block(struct mtd_info *mtd, u16 chipnr, int page);


/* Global Variables */
//u8 g_rtk_mtd[MTDSIZE];
struct mtd_info *rtk_mtd;// = g_rtk_mtd;

static DECLARE_MUTEX (sem);


static int page_size, oob_size, ppb;
static int RBA_PERCENT = 5;
static char is_NF_CP_Enable_read = 0;
static char is_NF_CP_Enable_write = 0;

static unsigned int u32CP_mode = 0;
static unsigned int cpSel = 0;
static char u8regKey0[4];
static char u8regKey1[4];
static char u8regKey2[4];
static char u8regKey3[4];
static unsigned int *regKey0;
static unsigned int *regKey1;
static unsigned int *regKey2;
static unsigned int *regKey3;

//DECLARE_MUTEX (sem_NF_CARDREADER);
//EXPORT_SYMBOL(sem_NF_CARDREADER);

#ifdef SPECIAL_TEST_FOR_65BIT_ECC
u8 ecc_65[115] = {0xff,  0xff, 0xe2,0xf5, 0x01 ,0x85 ,0x7d ,0x1b ,0xb2 ,0x87 ,0xbe ,0x4d ,0x66 ,0x66 ,0xb3 ,0xf9
 ,0x74 ,0x45 ,0xe6 ,0xc5 ,0x38 ,0x2c ,0xf2 ,0x87 ,0x1c ,0x98 ,0x98 ,0xb2 ,0x98 ,0xe8 ,0x9a ,0x0f
 ,0x57 ,0x6c ,0x1a ,0x6a ,0x0d ,0x15 ,0xef ,0xb0 ,0xa6 ,0xc8 ,0xbc ,0xab ,0x2c ,0x86 ,0x24 ,0xfc
 ,0xa1 ,0x30 ,0x28 ,0xed ,0xfe ,0xc8 ,0x6c ,0x6e ,0x16 ,0xe9 ,0xde ,0xc7 ,0x1d ,0x97 ,0x5f ,0x88
 ,0xbb ,0xe2 ,0xd6 ,0x68 ,0xc2 ,0xfe ,0x0c ,0x51 ,0xe0 ,0x67 ,0xd8 ,0xa8 ,0xc3 ,0x56 ,0x14 ,0xb5
 ,0x46 ,0x62 ,0x84 ,0x22 ,0xdf ,0x31 ,0xbf ,0x25 ,0x3f ,0x23 ,0xc4 ,0x71 ,0xd6 ,0x46 ,0xe5 ,0xf0
 ,0x3b ,0xba ,0x04 ,0xd3 ,0x45 ,0x41 ,0x7d ,0x48 ,0x7e ,0x1c ,0x92 ,0x96 ,0x4a ,0x57 ,0xbf ,0x25
 ,0xa3 ,0xa8 ,0xb0 };
#endif

#if RTK_ARD_ALGORITHM //Variable declartion
	#define TOTAL_BLK_NUM	8192
	const unsigned int g_RecBlkStart = 64;
	const unsigned int g_RecBlkEnd   = 7389;

	const unsigned int g_PorcWindowSize = 64;
	const unsigned int g_ReadCntThrshld = 0x200000;
	//const unsigned int g_ReadCntThrshld = 0x400000;

	const unsigned int g_MaxMinDist = 8192;
	const unsigned int g_DistInc = 1;

	const unsigned int g_WinTrigThrshld = 8192;
	static unsigned int g_WinTrigCnt = 0;

	static unsigned int g_RecArray[8192]={0};

	static unsigned int g_u32WinStart = 0;
	static unsigned int g_u32WinEnd = 0;

	int whichBlk= 0;
	int pagePerBlk = 0;

#endif
struct rtk_nand_host {
	struct mtd_info		mtd;
	struct nand_chip	nand;
	struct mtd_partition	*parts;
	struct device		*dev;

	void			*spare0;
	void			*main_area0;
	void			*main_area1;

	void __iomem		*base;
	void __iomem		*regs;
	int			status_request;
	struct clk		*clk;
	int			clk_act;
	int			irq;

#if RTK_NAND_INTERRUPT
	wait_queue_head_t	irq_waitq;
#endif
	uint8_t			*data_buf;
	unsigned int		buf_start;
	int			spare_len;
};

static void rtk_nand_printf_pp_buffer(u32 region)
{
	u_char data_buf[512];
	u32 id_chain,i;
	switch(region)
	{
		case 0:
			break;
		case 1:
			break;
		case 2:
			break;
		case 3:
			break;
		case 4:
			//print ecc region 2049~2304 Bytes regio
			//Set PP
			REG_WRITE_U32(REG_READ_BY_PP,NF_READ_BY_PP_read_by_pp(0x0));
			
			REG_WRITE_U32(REG_PP_CTL0,NF_PP_CTL0_pp_enable(0x01));
			REG_WRITE_U32(REG_PP_CTL1,NF_PP_CTL1_pp_start_addr(0));

			REG_WRITE_U32(REG_PP_CTL0,NF_PP_CTL0_pp_reset(1));
			REG_WRITE_U32(REG_SRAM_CTL,NF_SRAM_CTL_map_sel(1)|NF_SRAM_CTL_access_en(1)|NF_SRAM_CTL_mem_region(0x4));
			for(i = 0; i < 256; i +=4)
			{
				id_chain = REG_READ_U32(REG_ND_PA0+i);
				data_buf[i+0] = id_chain & 0xff;
				data_buf[i+1] = (id_chain >> 8) & 0xff;
				data_buf[i+2] = (id_chain >> 16) & 0xff;
				data_buf[i+3] = (id_chain >> 24) & 0xff;
			}		
			printk("\n================PRINT ECC=====================\n");
			for(i = 0; i < 256; i++)
			{
				if(!(i %16))
					printf("\n");
				printk(" %02x",  data_buf[i]);				
			}
			printk("\n================PRINT ECC=====================\n");

			break;
		default:
			break;
	}
	REG_WRITE_U32(REG_SRAM_CTL,0x0);
	return 0;
}

static void rtk_nand_reset(void)
{
	
	REG_WRITE_U32(REG_SPR_DDR_CTL, NF_SPR_DDR_CTL_spare_ddr_ena(1) | NF_SPR_DDR_CTL_per_2k_spr_ena(1)| NF_SPR_DDR_CTL_spare_dram_sa(0)); //set spare2ddr func. 4=>0.5k spe2ddr_ena at A000F000

	REG_WRITE_U32( REG_PD,~(0x1 << 0) );
	REG_WRITE_U32( REG_ND_CMD,0xFF/*CMD_RESET*/ ); //stevie

	//if(is_jupiter_cpu()||is_macarthur_cpu()||is_nike_cpu())
	//	REG_WRITE_U32( REG_ND_CTL,(0x80 | 0x00));
	//else
	//	REG_WRITE_U32( REG_ND_CTL,(NF_ND_CTL_xfer(0x01) | NF_ND_CTL_tran_mode(0)) );
	REG_WRITE_U32( REG_ND_CTL,(NF_ND_CTL_xfer(0x01) | NF_ND_CTL_tran_mode(0)) );

	WAIT_DONE(REG_ND_CTL,0x80,0);
	WAIT_DONE(REG_ND_CTL,0x40,0x40);

	REG_WRITE_U32( REG_MULTI_CHNL_MODE,0x00);
	REG_WRITE_U32( REG_TIME_PARA1,NF_TIME_PARA1_T1(0x0));
	REG_WRITE_U32( REG_TIME_PARA2,NF_TIME_PARA2_T2(0x0));
	REG_WRITE_U32( REG_TIME_PARA3,NF_TIME_PARA3_T3(0x0));

	//if(!is_jupiter_cpu()||!is_saturn_cpu()||!is_darwin_cpu()||!is_macarthur_cpu()||is_nike_cpu())
	//	REG_WRITE_U32( REG_DELAY_CTL,NF_DELAY_CTL_T_WHR_ADL(0x09) );
	REG_WRITE_U32( REG_DELAY_CTL,NF_DELAY_CTL_T_WHR_ADL(0x09) );

	REG_WRITE_U32( REG_MULTI_CHNL_MODE,NF_MULTI_CHNL_MODE_edo(0x01));
	REG_WRITE_U32( REG_ECC_STOP,NF_ECC_STOP_ecc_n_stop(0x01));

	REG_WRITE_U32( REG_ND_CA0,0);
	REG_WRITE_U32( REG_ND_CA1,0);
}

static void rtk_nand_auto_mode_case_2(void)
{
	// analysis wave form by LA or scope, and we send 0xFF  command
	REG_WRITE_U32(REG_CMD3,0xFF);
	//Enable Auto mode
	REG_WRITE_U32( REG_AUTO_TRIG,NF_AUTO_TRIG_auto_trig(1)|NF_AUTO_TRIG_spec_auto_case(2));
	WAIT_DONE(REG_AUTO_TRIG,0x80,0);
	return;
}

// this is for test auto mode 101 CAD write, using LA to check waveform
static void rtk_nand_auto_mode_case_3(struct mtd_info *mtd, u16 chipnr, unsigned int page,
			const u_char *data_buf, const  u_char *oob_buf)
{
	unsigned int ppb = mtd->erasesize/mtd->oobblock;
	struct nand_chip *this = (struct nand_chip *) mtd->priv;
	int rc = 0, i;
	uint8_t auto_trigger_mode = 6;
	uint8_t addr_mode = 6;

	unsigned int page_len, dram_sa, dma_len, spare_dram_sa;
	unsigned char oob_1stB;

	unsigned char nf_oob_buf[oob_size];
	unsigned int chip_section = 0;
	unsigned int section = 0;
	unsigned int index = 0;
	unsigned int retryCnt = RETRY_COUNT;
#ifdef RTK_ROM_CODE
	unsigned char temp_oob_buf[oob_size];
	this->g_oobbuf = oob_buf = temp_oob_buf;
#endif

	printk("[%s] entry... \n", __FUNCTION__);
	this->select_chip(mtd, chipnr);

	if(((uint32_t)data_buf&0x7)!=0) {
			printk("[%s]data_buf must 8 byte alignmemt!!\n",__FUNCTION__);
			BUG();
	}
	if(((uint32_t)oob_buf&0x7)!=0) {
			printk("[%s]oob_buf must 8 byte alignmemt!!\n",__FUNCTION__);
			BUG();
	}
#if 0       //stevie
	if ( chipnr == 0 && page < g_BootcodeSize/page_size ){
		if ( isBBT && page<3*ppb ){
			printk("[%s] Updating BBT %d page=%d\n", __FUNCTION__, page/ppb, page%ppb);
		}
		else {
			printk("[%s] You have no permission to write page %d\n", __FUNCTION__, page);
			//up (&sem_NF_CARDREADER);
			return -2;
		}
		mtd->isCPdisable_W = 1;
	}
	if ( chipnr == 0 && page >= g_BootcodeSize/page_size){
		if(page<(g_BootcodeSize+g_Factory_param_size)/page_size+ppb)
		{
			  mtd->isCPdisable_W = 1;
		}
	}
#endif
	//if(is_darwin_cpu()||is_macarthur_cpu()||is_nike_cpu())//Enable NF_WP pin (Write Protect Pin)
	//{
	//	if(g_WP_en)
	//	{
	//		setGPIOBit(g_regGPIODirBase,g_NFWP_no,NF_GPIO_OUT);//Set Driection
	//		setGPIOBit(g_regGPIOOutBase,g_NFWP_no,g_NFWP_value_dis);//Set Value
	//	}
	//}
#if 0       //stevie
	if ( page == ppb || page == ppb+1|| page == ppb*2 || page == ppb*2+1 )
		oob_1stB = BBT_TAG;
	else if(( page>=g_BootcodeSize/page_size )&&(page<(g_BootcodeSize+g_Factory_param_size)/page_size))
		oob_1stB = TAG_FACTORY_PARAM;
	else
		oob_1stB = 0xFF;
#endif
	oob_1stB = 0xFF;
	/*
	printk("[nand DBG]data_buf :  0x%x, g_oobbuf : 0x%x, g_databuf : 0x%x", (u32)data_buf, (u32)this->g_oobbuf, (u32)this->g_databuf);
		for(i=0; i < 512; i ++)
		{
			if(!(i %16))
				printf("\n");
			printf(" %02x",  data_buf[i]);
		}
	*/
	this->g_oobbuf[0x0] = oob_1stB;
	this->g_oobbuf[0x10] = oob_1stB;
	this->g_oobbuf[0x20] = oob_1stB;
	this->g_oobbuf[0x30] = oob_1stB;
	
	this->g_oobbuf[0x8] = oob_1stB;
	this->g_oobbuf[0x18] = oob_1stB;
	this->g_oobbuf[0x28] = oob_1stB;
	this->g_oobbuf[0x38] = oob_1stB;
	if(oob_size >= 0x78)
	{
		this->g_oobbuf[0x40] = oob_1stB;
		this->g_oobbuf[0x50] = oob_1stB;
		this->g_oobbuf[0x60] = oob_1stB;
		this->g_oobbuf[0x70] = oob_1stB;

		this->g_oobbuf[0x48] = oob_1stB;
		this->g_oobbuf[0x58] = oob_1stB;
		this->g_oobbuf[0x68] = oob_1stB;
		this->g_oobbuf[0x78] = oob_1stB;
	}

#if 0
			int i,id_chain;
			REG_WRITE_U32(REG_DATA_TL0,NF_DATA_TL0_length0(0x10));
			REG_WRITE_U32(REG_DATA_TL1,NF_DATA_TL1_access_mode(0x01) |NF_DATA_TL1_length1(0));
		
			//Set PP
			REG_WRITE_U32(REG_READ_BY_PP,NF_READ_BY_PP_read_by_pp(0x0));
		
			REG_WRITE_U32(REG_PP_CTL0,NF_PP_CTL0_pp_enable(0x01));
			REG_WRITE_U32(REG_PP_CTL1,NF_PP_CTL1_pp_start_addr(0));
		
			//Set command
			REG_WRITE_U32(REG_ND_CMD,NF_ND_CMD_cmd(CMD_PG_WRITE_C1));
			REG_WRITE_U32(REG_ND_CTL,NF_ND_CTL_xfer(0x01));
			WAIT_DONE(REG_ND_CTL,0x80,0);
		
			//Set address
			REG_WRITE_U32(REG_ND_PA0, page&0xff);
			REG_WRITE_U32(REG_ND_PA1, (page>>8)&0xff);
			REG_WRITE_U32(REG_ND_PA2,NF_ND_PA2_addr_mode(0x01) | (page>>16)&0x1f);
		
			REG_WRITE_U32(REG_ND_CTL,NF_ND_CTL_xfer(1)|NF_ND_CTL_tran_mode(1));
			WAIT_DONE(REG_ND_CTL,0x80,0);

			//Reset PP
			REG_WRITE_U32(REG_PP_CTL0,NF_PP_CTL0_pp_reset(1));
			//Move data to SRAM  from  DRAM

			// flush cache.
			RTK_FLUSH_CACHE(data_buf, page_size);
			RTK_FLUSH_CACHE(this->g_oobbuf, oob_size);
			/*
			dram_sa = ( (uint32_t)data_buf >> 3);
			REG_WRITE_U32(REG_DMA_CTL1,NF_DMA_CTL1_dram_sa(dram_sa));
			dma_len = page_size >> 9;
			REG_WRITE_U32(REG_DMA_CTL2,NF_DMA_CTL2_dma_len(dma_len));	
			
			//if (oob_buf)
			//	spare_dram_sa = ( (uint32_t)nf_oob_buf >> 3);
			//else
				spare_dram_sa = ( (uint32_t)this->g_oobbuf >> 3);
			REG_WRITE_U32(REG_SPR_DDR_CTL,NF_SPR_DDR_CTL_spare_ddr_ena(1)|NF_SPR_DDR_CTL_per_2k_spr_ena(1)|NF_SPR_DDR_CTL_spare_dram_sa(spare_dram_sa));
			
			REG_WRITE_U32(REG_DMA_CTL3,NF_DMA_CTL3_ddr_wr(0)|NF_DMA_CTL3_dma_xfer(1));
			//REG_WRITE_U32( REG_AUTO_TRIG,NF_AUTO_TRIG_auto_trig(1)|NF_AUTO_TRIG_spec_auto_case(0)| NF_AUTO_TRIG_auto_case(auto_trigger_mode));
*/
			
			REG_WRITE_U32(REG_SRAM_CTL,NF_SRAM_CTL_map_sel(1)|NF_SRAM_CTL_access_en(1)|NF_SRAM_CTL_mem_region(0));
			
			for(i = 0; i < 512; i +=4)
			{
				id_chain = data_buf[i+0]  & 0xff;
				id_chain |= (data_buf[i+1] << 8) & 0xff00;
				id_chain |= (data_buf[i+2] << 16) & 0xff0000;
				id_chain |= (data_buf[i+3] << 24) & 0xff000000;
				REG_WRITE_U32(REG_ND_PA0+i, id_chain);
			}
			REG_WRITE_U32(REG_SRAM_CTL,0x0);	//# no omitted


			//Enable XFER mode
			REG_WRITE_U32(REG_ND_CTL,NF_ND_CTL_xfer(1)| NF_ND_CTL_ecc_enable(0) |NF_ND_CTL_tran_mode(3));
			WAIT_DONE(REG_ND_CTL,0x80,0);
	
			//while ( (REG_READ_U32(REG_ND_CTL) & 0x80) != 0 )
			//{
			//	asm("nop");
			//}
			//Set write command 2
			REG_WRITE_U32(REG_ND_CMD,NF_ND_CMD_cmd(CMD_PG_WRITE_C2));
			REG_WRITE_U32(REG_ND_CTL,NF_ND_CTL_xfer(0x01));
			WAIT_DONE(REG_ND_CTL,0x80,0);	
			//REG_WRITE_U32(REG_POLL_FSTS,NF_POLL_FSTS_bit_sel(6)|NF_POLL_FSTS_trig_poll(1) );
			//WAIT_DONE(REG_POLL_FSTS,0x01,0x0);

			WAIT_DONE(REG_ND_CTL,0x40,0x40);

			return 0;
#else
	//REG_WRITE_U32(REG_SRAM_CTL,0x00);//add by alexchang 0208-2010
	REG_WRITE_U32(REG_DATA_TL0,NF_DATA_TL0_length0(0));
	if(this->ecc_select >= 0x18)
	{
		// enable randomizer
		REG_WRITE_U32(REG_RMZ_CTRL, 1);
		
		page_len = page_size >> 10;
		REG_WRITE_U32(REG_DATA_TL1,NF_DATA_TL1_length1(4));
	}
	else
	{
		// set random write
		REG_WRITE_U32(REG_RND_EN, 0);
		REG_WRITE_U32(REG_RND_CMD1, 0x85);
		REG_WRITE_U32(REG_RND_DATA_STR_COL_H, 0);	// data start address MSB (always 0)
		REG_WRITE_U32(REG_RND_SPR_STR_COL_H, page_size >> 8);	// spare start address MSB
		REG_WRITE_U32(REG_RND_SPR_STR_COL_L, page_size & 0xff); // spare start address LSB
		
		page_len = page_size >> 9;
		REG_WRITE_U32(REG_DATA_TL1,NF_DATA_TL1_length1(2));
	}

	REG_WRITE_U32( REG_PAGE_LEN,NF_PAGE_LEN_page_len(page_len));

	//Set PP
	REG_WRITE_U32(REG_READ_BY_PP,NF_READ_BY_PP_read_by_pp(0));//add by alexchang 0208-2010
	REG_WRITE_U32(REG_PP_CTL1,NF_PP_CTL1_pp_start_addr(0));//add by alexchang 0208-2010
	REG_WRITE_U32(REG_PP_CTL0,0);//add by alexchang 0208-2010

	//Set command
	REG_WRITE_U32(REG_ND_CMD,NF_ND_CMD_cmd(0x85));
	//REG_WRITE_U32(REG_CMD2,NF_CMD2_cmd2(CMD_PG_WRITE_C2));
	//REG_WRITE_U32(REG_CMD3,NF_CMD3_cmd3(CMD_PG_WRITE_C3));

	//Set address
	REG_WRITE_U32( REG_ND_PA0,NF_ND_PA0_page_addr0(page));
	REG_WRITE_U32( REG_ND_PA1,NF_ND_PA1_page_addr1(page>>8));
	REG_WRITE_U32( REG_ND_PA2,NF_ND_PA2_addr_mode(addr_mode)|NF_ND_PA2_page_addr2(page>>16));
	REG_WRITE_U32( REG_ND_PA3,NF_ND_PA3_page_addr3((page>>21)&0x7));
	REG_WRITE_U32(REG_ND_CA0, 0);
	REG_WRITE_U32(REG_ND_CA1, 0);

	//Set ECC
	REG_WRITE_U32(REG_MULTI_CHNL_MODE,NF_MULTI_CHNL_MODE_edo(1));//add by alexchang0205-2010

	// no ECC NF_MULTI_CHNL_MODE_ecc_pass(1)
	if(monflg == 5566)
	{
		printk("Set write ECC PASS \n");
		REG_WRITE_U32(REG_MULTI_CHNL_MODE,NF_MULTI_CHNL_MODE_edo(1)|NF_MULTI_CHNL_MODE_ecc_pass(1));
	}
	REG_WRITE_U32( REG_ECC_STOP,NF_ECC_STOP_ecc_n_stop(0x01));

	switch(this->ecc_select) {
		case 6:
			REG_WRITE_U32(REG_ECC_SEL, 0x0);
			break;
		case 12:
			REG_WRITE_U32(REG_ECC_SEL, 0x1);
			break;
		case 16:
			REG_WRITE_U32(REG_ECC_SEL, 0xe);
			break;
		case 24:
			REG_WRITE_U32(REG_ECC_SEL, 0xa);
			break;
		case 40:
			REG_WRITE_U32(REG_ECC_SEL, 0x2);
			break;
		case 43:
			REG_WRITE_U32(REG_ECC_SEL, 0x4);
			break;
		case 65:
			REG_WRITE_U32(REG_ECC_SEL, 0x6);
			break;
		case 72:
			REG_WRITE_U32(REG_ECC_SEL, 0x8);
			break;
		default:
			REG_WRITE_U32(REG_ECC_SEL, 0x0);
			break;
	}

	// flush cache.
	RTK_FLUSH_CACHE(data_buf, page_size);
	RTK_FLUSH_CACHE(this->g_oobbuf, oob_size);
	
	dram_sa = ( VIR_TO_PHY((uint32_t)data_buf) >> 3);
	REG_WRITE_U32(REG_DMA_CTL1,NF_DMA_CTL1_dram_sa(dram_sa));
	dma_len = page_size >> 9;
	REG_WRITE_U32(REG_DMA_CTL2,NF_DMA_CTL2_dma_len(dma_len));	
	
	//if (oob_buf)
	//	spare_dram_sa = ( (uint32_t)nf_oob_buf >> 3);
	//else
		spare_dram_sa = ( (uint32_t)this->g_oobbuf >> 3);
	REG_WRITE_U32(REG_SPR_DDR_CTL,NF_SPR_DDR_CTL_spare_ddr_ena(1)|NF_SPR_DDR_CTL_per_2k_spr_ena(1)|NF_SPR_DDR_CTL_spare_dram_sa(spare_dram_sa));

	REG_WRITE_U32(REG_DMA_CTL3,NF_DMA_CTL3_ddr_wr(0)|NF_DMA_CTL3_dma_xfer(1));
	REG_WRITE_U32( REG_AUTO_TRIG,NF_AUTO_TRIG_auto_trig(1)|NF_AUTO_TRIG_spec_auto_case(0)| NF_AUTO_TRIG_auto_case(auto_trigger_mode));


//#if RTK_NAND_INTERRUPT
//	RTK_WAIT_EVENT;
//	NF_RESET_IRQ;
//	NF_ENABLE_IRQ(NF_POLL_IRQ_MASK);
//	g_nfFlag = NF_WRITE_TRIGGER;
//	REG_WRITE_U32(REG_POLL_FSTS,NF_POLL_FSTS_bit_sel(6)|NF_POLL_FSTS_trig_poll(1) );
//	RTK_WAIT_EVENT;
//#else
	WAIT_DONE(REG_AUTO_TRIG,0x80,0);
	WAIT_DONE(REG_DMA_CTL3,0x01,0);
	//REG_WRITE_U32(REG_POLL_FSTS,NF_POLL_FSTS_bit_sel(6)|NF_POLL_FSTS_trig_poll(1) );
	//WAIT_DONE(REG_POLL_FSTS,0x01,0x0);
	WAIT_DONE(REG_ND_CTL,0x40,0x40);//add by alexchang 0416-2010
//#endif
	if ( REG_READ_U32(REG_ND_DAT) & 0x01 ){
		//up (&sem_NF_CARDREADER);
		//up_write(&rw_sem);
		printk("[%s] write is not completed at page %d\n", __FUNCTION__, page);
		//padunlock(PAD_NAND);	 // unlock nand・s pad
		return -1;
	}

	//while (1);
	return 0;
#endif
}



//----------------------------------------------------------------------------
static void rtk_read_parameter_page(struct mtd_info *mtd,u_char para[256])
{
#define XFER_MODE_1
	int id_chain;

#ifdef XFER_MODE_1

	REG_WRITE_U32(REG_DATA_TL0,NF_DATA_TL0_length0(0x00));
	REG_WRITE_U32(REG_DATA_TL1,NF_DATA_TL1_access_mode(0x01) | NF_DATA_TL1_length1(1));

	//Set PP
	REG_WRITE_U32(REG_READ_BY_PP,NF_READ_BY_PP_read_by_pp(0x0));

	REG_WRITE_U32(REG_PP_CTL0,NF_PP_CTL0_pp_enable(0x01));
	REG_WRITE_U32(REG_PP_CTL1,NF_PP_CTL1_pp_start_addr(0));

	//Set command
	REG_WRITE_U32(REG_ND_CMD,NF_ND_CMD_cmd(CMD_READ_PARAMETER));
	REG_WRITE_U32(REG_ND_CTL,NF_ND_CTL_xfer(0x01));
	WAIT_DONE(REG_ND_CTL,0x80,0);

	//Set address
	REG_WRITE_U32(REG_ND_PA0, 0);
	REG_WRITE_U32(REG_ND_PA1, 0);
	REG_WRITE_U32(REG_ND_PA2,NF_ND_PA2_addr_mode(0x07));

	REG_WRITE_U32(REG_ND_CTL,NF_ND_CTL_xfer(1)|NF_ND_CTL_tran_mode(1));
	WAIT_DONE(REG_ND_CTL,0x80,0);
	WAIT_DONE(REG_ND_CTL,0x40,0x40);
	//Enable XFER mode
	REG_WRITE_U32(REG_ND_CTL,NF_ND_CTL_xfer(1)|NF_ND_CTL_tran_mode(4));
	WAIT_DONE(REG_ND_CTL,0x80,0);

	//Reset PP
	REG_WRITE_U32(REG_PP_CTL0,NF_PP_CTL0_pp_reset(1));

	//Move data to DRAM from SRAM
	REG_WRITE_U32(REG_SRAM_CTL,NF_SRAM_CTL_map_sel(1)|NF_SRAM_CTL_access_en(1)|NF_SRAM_CTL_mem_region(0));
#if 0
	id_chain = REG_READ_U32(REG_ND_PA0);
	//printk("id_chain 1 = 0x%x \n", id_chain);
	para[0] = id_chain & 0xff;
	para[1] = (id_chain >> 8) & 0xff;
	para[2] = (id_chain >> 16) & 0xff;
	para[3] = (id_chain >> 24) & 0xff;
#endif
	int i = 0;
	for(i = 0; i < 256; i +=4)
	{
		//read 256 byte
		id_chain = REG_READ_U32(REG_ND_PA0 + i);
		para[i+0] = id_chain & 0xff;
		para[i+1] = (id_chain >> 8) & 0xff;
		para[i+2] = (id_chain >> 16) & 0xff;
		para[i+3] = (id_chain >> 24) & 0xff;
	}
	g_id_chain = id_chain;
	//if(id_chain==0xDA94D7AD)
	//{
		//printk("Detect H27UBG8T2B and enable read-retrial mode...\n");
		//g_enReadRetrial = 1;
		//g_isRandomize = 1;
		//printk("Enable Read-retrial mode...\n");
		//printk("Enable Randomized mechanism\n");
	//}
	printf("Parameter page signature : 0x%x%x%x%x \n", (u32)para[0], (u32)para[1],(u32)para[2],(u32)para[3]);
#if 0
	id_chain = REG_READ_U32(REG_ND_PA1);
	//printk("id_chain 2 = 0x%x \n", id_chain);
	para[4] = id_chain & 0xff;
	para[5] = (id_chain >> 8) & 0xff;
#endif
	REG_WRITE_U32(REG_SRAM_CTL,0x0);	//# no omitted
#else
	REG_WRITE_U32(REG_PP_CTL0,NF_PP_CTL0_pp_reset(0x01));
	REG_WRITE_U32(REG_DATA_TL0,NF_DATA_TL0_length0(0x00));
	REG_WRITE_U32(REG_DATA_TL1,NF_DATA_TL1_access_mode(0x01)|NF_DATA_TL1_length1(0x2));

	//Set page length at auto mode 
	REG_WRITE_U32(REG_PAGE_LEN, 0x1);
	REG_WRITE_U32(REG_READ_BY_PP,NF_READ_BY_PP_read_by_pp(0x1));

	REG_WRITE_U32(REG_PP_CTL0,NF_PP_CTL0_pp_enable(0x01));
	REG_WRITE_U32(REG_PP_CTL1,NF_PP_CTL1_pp_start_addr(0));

	//Set command
	REG_WRITE_U32(REG_ND_CMD,NF_ND_CMD_cmd(CMD_READ_PARAMETER));

	//Set address
	REG_WRITE_U32(REG_ND_PA0, 0);
	REG_WRITE_U32(REG_ND_PA1, 0);
	REG_WRITE_U32(REG_ND_PA2,NF_ND_PA2_addr_mode(0x07));

	//Set ECC	
	REG_WRITE_U32(REG_MULTI_CHNL_MODE, NF_MULTI_CHNL_MODE_ecc_no_check(1) | NF_MULTI_CHNL_MODE_ecc_pass(1));	//Set HW no check ECC	
	REG_WRITE_U32(REG_ECC_STOP, NF_ECC_STOP_ecc_n_stop(0x1));	//Set ECC no stop
	//Set DMA	
	REG_WRITE_U32(REG_DMA_CTL1, para);	//Set DRAM start address
	REG_WRITE_U32(REG_DMA_CTL2, 0x1);	//Transfer length (Unit = 512B) 
	REG_WRITE_U32(REG_DMA_CTL3, NF_DMA_CTL3_ddr_wr(1) | NF_DMA_CTL3_dma_xfer(1));	//Set DMA direction and enable DMA transfer 
	//Enable Auto mode
	REG_WRITE_U32(REG_AUTO_TRIG,NF_AUTO_TRIG_auto_trig(1)|NF_AUTO_TRIG_spec_auto_case(1)| NF_AUTO_TRIG_auto_case(0));
	WAIT_DONE(REG_AUTO_TRIG,0x80,0);
	//Enable Auto mode
	REG_WRITE_U32(REG_AUTO_TRIG,NF_AUTO_TRIG_auto_trig(1)|NF_AUTO_TRIG_spec_auto_case(0)| NF_AUTO_TRIG_auto_case(4));
	WAIT_DONE(REG_AUTO_TRIG,0x80,0);
	//Wait DMA done 
	WAIT_DONE(REG_DMA_CTL3, 0x01, 0x00);
	
	printf("Parameter page signature : 0x%x%x%x%x \n", (u32)para[0], (u32)para[1],(u32)para[2],(u32)para[3]);

#endif
}

//----------------------------------------------------------------------------
static void rtk_read_oob_from_SRAM(struct mtd_info *mtd, __u8 *r_oobbuf)
{
	unsigned int reg_oob, reg_num;
	int i;
	//printk("mtd->ecctype 0x%x\n",mtd->ecctype);
//	if ( mtd->ecctype == MTD_ECC_NONE )
	if ( mtd->ecctype == MTD_ECC_NONE ||(mtd->ecctype!=MTD_ECC_RTK_HW))
	{
		reg_num = REG_NF_BASE_ADDR;
		reg_oob = REG_READ_U32(reg_num);
		r_oobbuf[0] = reg_oob & 0xff;
		r_oobbuf[1] = (reg_oob >> 8) & 0xff;
		r_oobbuf[2] = (reg_oob >> 16) & 0xff;
		r_oobbuf[3] = (reg_oob >> 24) & 0xff;

		reg_num = REG_NF_BASE_ADDR+4;
		reg_oob = REG_READ_U32(reg_num);
		r_oobbuf[4] = reg_oob & 0xff;

		reg_num = REG_NF_BASE_ADDR+16;
		reg_oob = REG_READ_U32(reg_num);
		r_oobbuf[5] = reg_oob & 0xff;
		r_oobbuf[6] = (reg_oob >> 8) & 0xff;
		r_oobbuf[7] = (reg_oob >> 16) & 0xff;
		r_oobbuf[8] = (reg_oob >> 24) & 0xff;

		reg_num = REG_NF_BASE_ADDR+16*2;
		reg_oob = REG_READ_U32(reg_num);
		r_oobbuf[9] = reg_oob & 0xff;
		r_oobbuf[10] = (reg_oob >> 8) & 0xff;
		r_oobbuf[11] = (reg_oob >> 16) & 0xff;
		r_oobbuf[12] = (reg_oob >> 24) & 0xff;

		reg_num = REG_NF_BASE_ADDR+16*3;
		reg_oob = REG_READ_U32(reg_num);
		r_oobbuf[13] = reg_oob & 0xff;
		r_oobbuf[14] = (reg_oob >> 8) & 0xff;
		r_oobbuf[15] = (reg_oob >> 16) & 0xff;
		r_oobbuf[16] = (reg_oob >> 24) & 0xff;

	}
	else
	{
		for ( i=0; i < 16; i++)
			{
				reg_num = REG_NF_BASE_ADDR + i*4;
				reg_oob = REG_READ_U32(reg_num);
				r_oobbuf[i*4+0] = reg_oob & 0xff;
				r_oobbuf[i*4+1] = (reg_oob >> 8) & 0xff;
				r_oobbuf[i*4+2] = (reg_oob >> 16) & 0xff;
				r_oobbuf[i*4+3] = (reg_oob >> 24) & 0xff;
			}
	}
}

//----------------------------------------------------------------------------

static int rtk_read_oob (struct mtd_info *mtd, u16 chipnr, int page, int len, u_char *oob_buf)
{
	struct nand_chip *this = (struct nand_chip *) mtd->priv;
	int rc = 0;
	int dram_sa, dma_len, spare_dram_sa;
	int blank_all_one=0;
	int page_len;
	unsigned int chip_section = 0;
	unsigned int section = 0;
	unsigned int index = 0;
	unsigned int retryCnt = RETRY_COUNT;
	uint8_t auto_trigger_mode = 2;
	uint8_t addr_mode = 1;
	uint8_t	bChkAllOne = 0;
	
	page_size = mtd->oobblock;
	oob_size = mtd->oobsize;
	ppb = mtd->erasesize/mtd->oobblock;

	if ( len>oob_size || !oob_buf ){
		printk("[%s] error: len>oob_size OR oob_buf is NULL\n", __FUNCTION__);
		//up (&sem_NF_CARDREADER);
		return -1;
	}
	if ( chipnr == 0 && page >= 0 && page < g_BootcodeSize/page_size )
		mtd->isCPdisable_R = 1;

	while (1)
	{
		REG_WRITE_U32(REG_DATA_TL0,NF_DATA_TL0_length0(0));
		if(this->ecc_select >= 0x18)
		{
			if (bChkAllOne) {
				// enable randomizer
				REG_WRITE_U32(REG_RMZ_CTRL, 0);
				REG_WRITE_U32(REG_BLANK_CHK,NF_BLANK_CHK_read_ecc_xnor_ena(1));
			}
			else {
				// enable randomizer
				REG_WRITE_U32(REG_RMZ_CTRL, 1);
				REG_WRITE_U32(REG_BLANK_CHK,NF_BLANK_CHK_read_ecc_xnor_ena(0));
			}
			page_len = page_size >> 10;
			REG_WRITE_U32(REG_DATA_TL1,NF_DATA_TL1_access_mode(1)|NF_DATA_TL1_length1(4));
		}
		else
		{
			// set random read			
			REG_WRITE_U32(REG_RND_EN, 1);
			REG_WRITE_U32(REG_RND_CMD1, 0x5);
			REG_WRITE_U32(REG_RND_CMD2, 0xe0);
			REG_WRITE_U32(REG_RND_DATA_STR_COL_H, 0);	// data start address MSB (always 0)
			REG_WRITE_U32(REG_RND_SPR_STR_COL_H, page_size >> 8);	// spare start address MSB
			REG_WRITE_U32(REG_RND_SPR_STR_COL_L, page_size & 0xff);	// spare start address LSB
		
			page_len = page_size >> 9;
			REG_WRITE_U32(REG_DATA_TL1,NF_DATA_TL1_access_mode(1)|NF_DATA_TL1_length1(2));
		}
		
		REG_WRITE_U32(REG_PAGE_LEN,NF_PAGE_LEN_page_len(page_len));

		//Set PP
		REG_WRITE_U32(REG_READ_BY_PP,NF_READ_BY_PP_read_by_pp(1));
		REG_WRITE_U32(REG_PP_CTL1,NF_PP_CTL1_pp_start_addr(0));
		REG_WRITE_U32(REG_PP_CTL0,0);

		// enable blank check
		REG_WRITE_U32(REG_BLANK_CHK,NF_BLANK_CHK_blank_ena(1) );

		//Set command
		REG_WRITE_U32(REG_ND_CMD,NF_ND_CMD_cmd(CMD_PG_READ_C1));
		REG_WRITE_U32(REG_CMD2,NF_CMD2_cmd2(CMD_PG_READ_C2));
		REG_WRITE_U32(REG_CMD3,NF_CMD3_cmd3(CMD_PG_READ_C3));
		
		//Set address
		REG_WRITE_U32( REG_ND_PA0,NF_ND_PA0_page_addr0( 0xff&page ));
		REG_WRITE_U32( REG_ND_PA1,NF_ND_PA1_page_addr1( 0xff&(page>>8) ));
		REG_WRITE_U32( REG_ND_PA2,NF_ND_PA2_addr_mode(addr_mode)|NF_ND_PA2_page_addr2( 0x1f&(page>>16) ));
		REG_WRITE_U32( REG_ND_PA3,NF_ND_PA3_page_addr3( 0x7&(page>>21) ));

		REG_WRITE_U32(REG_ND_CA0, 0);
		REG_WRITE_U32(REG_ND_CA1, 0);

		//Set ECC
		REG_WRITE_U32( REG_MULTI_CHNL_MODE,NF_MULTI_CHNL_MODE_edo(1));//add by alexchang0205-2010
		REG_WRITE_U32( REG_ECC_STOP,NF_ECC_STOP_ecc_n_stop(0x01));

		switch(this->ecc_select) {
			case 6:
				REG_WRITE_U32(REG_ECC_SEL, 0x0);
				break;
			case 12:
				REG_WRITE_U32(REG_ECC_SEL, 0x1);
				break;
			case 16:
				REG_WRITE_U32(REG_ECC_SEL, 0xe);
				break;
			case 24:
				REG_WRITE_U32(REG_ECC_SEL, 0xa);
				break;
			case 40:
				REG_WRITE_U32(REG_ECC_SEL, 0x2);
				break;
			case 43:
				REG_WRITE_U32(REG_ECC_SEL, 0x4);
				break;
			case 65:
				REG_WRITE_U32(REG_ECC_SEL, 0x6);
				break;
			case 72:
				REG_WRITE_U32(REG_ECC_SEL, 0x8);
				break;
			default:
				REG_WRITE_U32(REG_ECC_SEL, 0x0);
				break;
		}

		dram_sa = ( (uint32_t)this->g_databuf >> 3);
		REG_WRITE_U32(REG_DMA_CTL1,NF_DMA_CTL1_dram_sa(dram_sa));
		dma_len = page_size >> 9;
		REG_WRITE_U32(REG_DMA_CTL2,NF_DMA_CTL2_dma_len(dma_len));

		spare_dram_sa = ( (uint32_t)oob_buf >> 3);
		REG_WRITE_U32( REG_SPR_DDR_CTL,NF_SPR_DDR_CTL_spare_ddr_ena(1)|NF_SPR_DDR_CTL_per_2k_spr_ena(0)|NF_SPR_DDR_CTL_spare_dram_sa(spare_dram_sa));

		// flush cache.
		RTK_FLUSH_CACHE(oob_buf, oob_size);

		REG_WRITE_U32(REG_DMA_CTL3,NF_DMA_CTL3_ddr_wr(1)|NF_DMA_CTL3_dma_xfer(1));
		REG_WRITE_U32( REG_AUTO_TRIG,NF_AUTO_TRIG_auto_trig(1)|NF_AUTO_TRIG_spec_auto_case(0)| NF_AUTO_TRIG_auto_case(auto_trigger_mode));
		WAIT_DONE(REG_AUTO_TRIG,0x80,0);

		WAIT_DONE(REG_AUTO_TRIG,0x80,0);
		WAIT_DONE(REG_DMA_CTL3,0x01,0);

		if(oob_buf)	{
			REG_WRITE_U32(REG_READ_BY_PP,0x00);
			REG_WRITE_U32(REG_SRAM_CTL, 0x30 | 0x04);
			rtk_read_oob_from_SRAM(mtd, oob_buf);
			REG_WRITE_U32(REG_SRAM_CTL, 0x00);
			REG_WRITE_U32(REG_READ_BY_PP,0x80);
		}

		// return OK if all data bit is 1 (page is not written yet)
		if (REG_READ_U32(REG_BLANK_CHK) & 0x2){
			//printk("[%s] page(0x%x) data all one\n", __FUNCTION__, page);
			return 0;
		}
		else if (REG_READ_U32(REG_ND_ECC) & 0x8) {
			if (!bChkAllOne && this->ecc_select>=0x18) {
				bChkAllOne = 1;
				continue;
			}
			printk("[%s] page(0x%x) ecc error\n", __FUNCTION__, page);
			return -1;	// ECC not correctable
		}
		else {
			//printk("[%s] page(0x%x) oob= 0x%x\n", __FUNCTION__, page, *oob_buf);
			return 0;
		}
	}
	return rc;
}
//
static int rtk_read_ecc_512 (struct mtd_info *mtd, u16 chipnr, unsigned int page, u_char *data_buf, u_char *oob_buf)
{
//static unsigned int eccReadCnt = 0;
	struct nand_chip *this = NULL;
	int rc = 0;
	int dram_sa, dma_len, spare_dram_sa;
	int blank_all_one = 0;
	int page_len;

	unsigned int chip_section = 0;
	unsigned int section = 0;
	unsigned int index = 0;
	unsigned int retryCnt = RETRY_COUNT;
	uint8_t auto_trigger_mode = 3;
	uint8_t addr_mode = 1;
	uint8_t bChkAllOne = 0;
	uint8_t read_retry_cnt = 0;
	uint8_t max_read_retry_cnt = 0;

	printk("rtk_read_ecc_page (chipnr=%d, page=0x%x, data_buf addr=0x%x, oob_buf addr=0x%x)\n",
		chipnr, page, data_buf, oob_buf);
	
	this = (struct nand_chip *) mtd->priv;
#ifdef RTK_ROM_CODE
	unsigned char temp_oob_buf[oob_size];
	this->g_oobbuf = oob_buf = temp_oob_buf;
#endif

	page_size = mtd->oobblock;
	oob_size = mtd->oobsize;
	ppb = mtd->erasesize/mtd->oobblock;

		volatile unsigned int data;

		unsigned char ks[16];
		unsigned char kh[16];
		unsigned char dest[16];
		unsigned int *intPTR;

	if (((uint32_t)data_buf&0x7)!=0) {
			printk("[%s]data_buf must 8 byte alignmemt!!\n",__FUNCTION__);
			BUG();
	}

	if (g_enReadRetrial) {
		switch(g_id_chain)
		{
			case 0x4b44642c:
				max_read_retry_cnt = 7;
				break;
			case 0x4b44442c:
				max_read_retry_cnt = 7;
				break;
			case 0x9384d798:
				max_read_retry_cnt = 6;
				break;
					   case 0x9384de98:
				max_read_retry_cnt = 6;
				break;
					   case 0x9394de98:
				max_read_retry_cnt = 7;
				break;			  
			default:
				break;
				
		}
	}
	//rtk_nand_reset();
	this->select_chip(mtd, chipnr);
#if 1
	while (1) 
	{					
		REG_WRITE_U32(REG_DATA_TL0,NF_DATA_TL0_length0(0));
		if(this->ecc_select >= 0x18)
		{
			if (bChkAllOne) {
				// enable randomizer
				REG_WRITE_U32(REG_RMZ_CTRL, 0);
				REG_WRITE_U32(REG_BLANK_CHK,NF_BLANK_CHK_blank_ena(1)|NF_BLANK_CHK_read_ecc_xnor_ena(1));
								//printk("[DBG]ecc error, set xnor and blank to 1, REG_BLANK_CHK reg: 0x%x\n", REG_READ_U32(REG_BLANK_CHK));
			}
			else {
				// enable randomizer
				REG_WRITE_U32(REG_RMZ_CTRL, 1);
				REG_WRITE_U32(REG_BLANK_CHK,NF_BLANK_CHK_blank_ena(1)|NF_BLANK_CHK_read_ecc_xnor_ena(0));
								//printk("[DBG]set xnor to 0, REG_BLANK_CHK reg: 0x%x\n", REG_READ_U32(REG_BLANK_CHK));
			}
			
			page_len = page_size >> 10;
			REG_WRITE_U32(REG_DATA_TL1,NF_DATA_TL1_access_mode(1)|NF_DATA_TL1_length1(4));
		}
		else
		{
			// set random read
			REG_WRITE_U32(REG_RND_EN, 1);
			REG_WRITE_U32(REG_RND_CMD1, 0x5);
			REG_WRITE_U32(REG_RND_CMD2, 0xe0);
			REG_WRITE_U32(REG_RND_DATA_STR_COL_H, 0);	// data start address MSB (always 0)
			REG_WRITE_U32(REG_RND_SPR_STR_COL_H, page_size >> 8);	// spare start address MSB
			REG_WRITE_U32(REG_RND_SPR_STR_COL_L, page_size & 0xff); // spare start address LSB
			
			page_len = page_size >> 9;
			REG_WRITE_U32(REG_DATA_TL1,NF_DATA_TL1_access_mode(1)|NF_DATA_TL1_length1(2));
		}
		
		REG_WRITE_U32(REG_PAGE_LEN,NF_PAGE_LEN_page_len(page_len));

		//Set PP
		REG_WRITE_U32(REG_READ_BY_PP,NF_READ_BY_PP_read_by_pp(1));
		REG_WRITE_U32(REG_PP_CTL1,NF_PP_CTL1_pp_start_addr(0));
		REG_WRITE_U32(REG_PP_CTL0,0);

		// enable blank check
		//REG_WRITE_U32(REG_BLANK_CHK,NF_BLANK_CHK_blank_ena(1) );

		//Set command
		REG_WRITE_U32(REG_ND_CMD,NF_ND_CMD_cmd(CMD_PG_READ_C1));
		REG_WRITE_U32(REG_CMD2,NF_CMD2_cmd2(CMD_PG_READ_C2));
		REG_WRITE_U32(REG_CMD3,NF_CMD3_cmd3(CMD_PG_READ_C3));
		
		//Set address
		REG_WRITE_U32( REG_ND_PA0,NF_ND_PA0_page_addr0( 0xff&page ));
		REG_WRITE_U32( REG_ND_PA1,NF_ND_PA1_page_addr1( 0xff&(page>>8) ));
		REG_WRITE_U32( REG_ND_PA2,NF_ND_PA2_addr_mode(addr_mode)|NF_ND_PA2_page_addr2( 0x1f&(page>>16) ));
		REG_WRITE_U32( REG_ND_PA3,NF_ND_PA3_page_addr3( 0x7&(page>>21) ));

		REG_WRITE_U32(REG_ND_CA0, 0);
		REG_WRITE_U32(REG_ND_CA1, 0);

		//Set ECC
		REG_WRITE_U32( REG_MULTI_CHNL_MODE,NF_MULTI_CHNL_MODE_edo(1));//add by alexchang0205-2010.
		if(monflg == 7788)
		{
			printk("set ECC NO CHECK read \n");
			
			REG_WRITE_U32( REG_MULTI_CHNL_MODE,NF_MULTI_CHNL_MODE_edo(1)|NF_MULTI_CHNL_MODE_ecc_no_check(1));//add by alexchang0205-2010.
		}
		REG_WRITE_U32( REG_ECC_STOP,NF_ECC_STOP_ecc_n_stop(0x01));
		switch(this->ecc_select) {
			case 6:
				REG_WRITE_U32(REG_ECC_SEL, 0x0);
				break;
			case 12:
				REG_WRITE_U32(REG_ECC_SEL, 0x1);
				break;
			case 16:
				REG_WRITE_U32(REG_ECC_SEL, 0xe);
				break;
			case 24:
				REG_WRITE_U32(REG_ECC_SEL, 0xa);
				break;
			case 40:
				REG_WRITE_U32(REG_ECC_SEL, 0x2);
				break;
			case 43:
				REG_WRITE_U32(REG_ECC_SEL, 0x4);
				break;
			case 65:
				REG_WRITE_U32(REG_ECC_SEL, 0x6);
				break;
			case 72:
				REG_WRITE_U32(REG_ECC_SEL, 0x8);
				break;
			default:
				REG_WRITE_U32(REG_ECC_SEL, 0x0);
				break;
		}

		if (oob_buf) {
			spare_dram_sa = ( (uint32_t)oob_buf >> 3);
		}
		else {
			spare_dram_sa = ( (uint32_t)this->g_oobbuf >> 3);
		}
		REG_WRITE_U32( REG_SPR_DDR_CTL,NF_SPR_DDR_CTL_spare_ddr_ena(1)|NF_SPR_DDR_CTL_per_2k_spr_ena(0)|NF_SPR_DDR_CTL_spare_dram_sa(spare_dram_sa));



		dram_sa = ( (uint32_t)data_buf >> 3);
		REG_WRITE_U32(REG_DMA_CTL1,NF_DMA_CTL1_dram_sa(dram_sa));
		dma_len = page_size >> 9;
		REG_WRITE_U32(REG_DMA_CTL2,NF_DMA_CTL2_dma_len(dma_len));
		//REG_WRITE_U32(REG_DMA_CTL3,NF_DMA_CTL3_ddr_wr(1)|NF_DMA_CTL3_dma_xfer(1));
#if 0 // no cp for now
				if(cp_mode==CP_NF_AES_CBC_128){
					if(cp_first == 1)
						REG_WRITE_U32(REG_DMA_CTL3,NF_DMA_CTL3_cp_enable(1)|NF_DMA_CTL3_cp_first(1)|NF_DMA_CTL3_ddr_wr(1)|NF_DMA_CTL3_dma_xfer(1));
					else
				REG_WRITE_U32(REG_DMA_CTL3,NF_DMA_CTL3_cp_enable(1)|NF_DMA_CTL3_cp_first(0)|NF_DMA_CTL3_ddr_wr(1)|NF_DMA_CTL3_dma_xfer(1));
				}else if(cp_mode == CP_NF_AES_ECB_128){
						REG_WRITE_U32(REG_DMA_CTL3,NF_DMA_CTL3_cp_enable(1)|NF_DMA_CTL3_cp_first(1)|NF_DMA_CTL3_ddr_wr(1)|NF_DMA_CTL3_dma_xfer(1));
				}else
#endif
				{
						REG_WRITE_U32(REG_DMA_CTL3,NF_DMA_CTL3_ddr_wr(1)|NF_DMA_CTL3_dma_xfer(1));
				}
		
		// flush cache.
		RTK_FLUSH_CACHE(data_buf, page_size);
		RTK_FLUSH_CACHE(oob_buf, oob_size);

		//Enable Auto mode
		REG_WRITE_U32( REG_AUTO_TRIG,NF_AUTO_TRIG_auto_trig(1)|NF_AUTO_TRIG_spec_auto_case(0)| NF_AUTO_TRIG_auto_case(auto_trigger_mode));

		WAIT_DONE(REG_AUTO_TRIG,0x80,0);
		WAIT_DONE(REG_DMA_CTL3,0x01,0);

	#if 0
		if(oob_buf) {
			//syncPageRead(); //stevie
			data = (REG_READ_U32(REG_SPR_DDR_CTL) & 0x0fffffff) << 3;	// physical address
			 //printk("data==> 0x%x\n",data);
			 data |= 0x80000000;
			oob_buf[0] = REG_READ_U32(data + 0x0) & 0xff;
			//printk("oob_buf[0]==> 0x%x, page=0x%x\n",oob_buf[0],page);		   
		}
	#endif

		//rtk_hexdump("data_buf : ", data_buf, page_size);
		//rtk_hexdump("oob_buf : ", oob_buf, oob_size);
		//if(REG_READ_U32(REG_ND_ECC) & 0x4)
		//printk("page: 0x%x, MAX_ECC_NUM: 0x%x\n", page, REG_MAX_ECC_NUM_max_ecc_num(REG_READ_U32(REG_MAX_ECC_NUM)));

		//syncPageRead();
		// return OK if all data bit is 1 (page is not written yet)
		if (REG_READ_U32(REG_BLANK_CHK) & 0x2){
			printk("data all one \n");
			REG_WRITE_U32(REG_BLANK_CHK,NF_BLANK_CHK_blank_ena(1)|NF_BLANK_CHK_read_ecc_xnor_ena(0));
			return 0;
		}
		else if (REG_READ_U32(REG_ND_ECC) & 0x8) {
			if (!bChkAllOne && this->ecc_select>=0x18) {
				//printk("[DBG]bChkAllOne this->ecc_select: 0x%x\n", this->ecc_select);
				bChkAllOne = 1;
				continue;
			}
			if (g_enReadRetrial) {
				bChkAllOne = 0;
				if(read_retry_cnt == 0){	//set pre condition
					switch(g_id_chain)
					{
					#if 0  //stevie
						case 0x9384d798:
							printk("TC58TEG5DCJT read-retrial rtk_set_pre_condition_toshiba\n");
							rtk_set_pre_condition_toshiba();
							break;
										case 0x9384de98:
							printk("TC58TEG6DCJT read-retrial rtk_set_pre_condition_toshiba\n");
							rtk_set_pre_condition_toshiba();
							break;		 
												case 0x9394de98:
							printk("TC58TEG6DDK read-retrial rtk_set_pre_condition_toshiba\n");
							rtk_set_pre_condition_toshiba();
							break;	  
					#endif
						default:
							break;							
					}
								}			   
				read_retry_cnt ++;
				if (read_retry_cnt <= max_read_retry_cnt) {
					switch(g_id_chain)
					{
					 //stevie
						case 0x4b44642c:
							printk("MT29F64G08CBABA read-retrial rtk_set_feature_micron ...read_retry_cnt: %d\n", read_retry_cnt);
							rtk_set_feature_micron(read_retry_cnt);
							break;
					#if 0
						case 0x4b44442c:
							printk("MT29F32G08CBADA read-retrial rtk_set_feature_micron ...read_retry_cnt: %d\n", read_retry_cnt);
							rtk_set_feature_micron(read_retry_cnt);
							break;	
						case 0x9384d798:
							printk("TC58TEG5DCJT read-retrial rtk_set_feature_toshiba ...read_retry_cnt: %d, data: 0x%x\n", read_retry_cnt, read_retry_toshiba_cnt[read_retry_cnt]);
							rtk_set_feature_toshiba(read_retry_toshiba_cnt[read_retry_cnt]);
							break;
										case 0x9384de98:
							printk("TC58TEG6DCJT read-retrial rtk_set_feature_toshiba ...read_retry_cnt: %d, data: 0x%x\n", read_retry_cnt, read_retry_toshiba_cnt[read_retry_cnt]);
							rtk_set_feature_toshiba(read_retry_toshiba_cnt[read_retry_cnt]);
											break;
										case 0x9394de98:
							printk("TC58TEG6DDK read-retrial rtk_set_feature_toshiba ...read_retry_cnt: %d\n", read_retry_cnt);
							rtk_set_feature_toshiba_new(read_retry_cnt);
							break;
                    			#endif
						default:
							break;
							
					}
					continue;
				}
			}
			printk("[DBG]ecc error... page=0x%x, REG_BLANK_CHK reg: 0x%x \n", page, REG_READ_U32(REG_BLANK_CHK));
			if (g_enReadRetrial) {
				if (read_retry_cnt !=0) {
					switch(g_id_chain)
					{
					#if 0  //stevie
						case 0x4b44642c:
							printk("MT29F64G08CBABA read-retrial rtk_set_feature_micron ...reset to 0\n");
							rtk_set_feature_micron(0);
							break;
										case 0x4b44442c:
							printk("MT29F32G08CBADA read-retrial rtk_set_feature_micron ...reset to 0\n");
							rtk_set_feature_micron(0);
							break;
						case 0x9384d798:
							printk("TC58TEG5DCJT read-retrial rtk_set_feature_toshiba ...reset to 0\n");
							rtk_set_feature_toshiba(0);
														rtk_set_terminate_toshiba();
							break;
										case 0x9384de98:
							printk("TC58TEG6DCJT read-retrial rtk_set_feature_toshiba ...reset to 0\n");
							rtk_set_feature_toshiba(0);
														rtk_set_terminate_toshiba();
							break;
												case 0x9394de98:
							printk("TC58TEG6DDK read-retrial rtk_set_feature_toshiba ...reset to 0\n");
							rtk_set_feature_toshiba_new(0);
														rtk_set_terminate_toshiba();
							break;
					#endif
						default:
							
							break;
							
					}
				}
			}
			REG_WRITE_U32(REG_BLANK_CHK,NF_BLANK_CHK_blank_ena(1)|NF_BLANK_CHK_read_ecc_xnor_ena(0));
			return -1;	// ECC not correctable
		}
		else {
			if (g_enReadRetrial) {
				if (read_retry_cnt !=0) {
					switch(g_id_chain)
					{
					#if 0 //stevie
						case 0x4b44642c:
							printk("MT29F64G08CBABA read-retrial rtk_set_feature_micron ...reset to 0\n");
							rtk_set_feature_micron(0);
							break;
										case 0x4b44442c:
							printk("MT29F32G08CBADA read-retrial rtk_set_feature_micron ...reset to 0\n");
							rtk_set_feature_micron(0);
							break;
										case 0x9384d798:
							printk("TC58TEG5DCJT read-retrial rtk_set_feature_toshiba ...reset to 0\n");
							rtk_set_feature_toshiba(0);
														rtk_set_terminate_toshiba();
							break;
										case 0x9384de98:
							printk("TC58TEG6DCJT read-retrial rtk_set_feature_toshiba ...reset to 0\n");
							rtk_set_feature_toshiba(0);
														rtk_set_terminate_toshiba();
							break;
												case 0x9394de98:
							printk("TC58TEG6DDK read-retrial rtk_set_feature_toshiba ...reset to 0\n");
							rtk_set_feature_toshiba_new(0);
														rtk_set_terminate_toshiba();
							break;
					#endif
						default:
							break;
							
					}
				}
			}			 
			REG_WRITE_U32(REG_BLANK_CHK,NF_BLANK_CHK_blank_ena(1)|NF_BLANK_CHK_read_ecc_xnor_ena(0));
			return 0;
		}
	}

	return rc;
#endif
}

static int rtk_read_diff_ecc (struct mtd_info *mtd, u16 chipnr, unsigned int page, unsigned int column, u_char *data_buf, u_char *oob_buf, u32 ecc_select)
{
//static unsigned int eccReadCnt = 0;
	struct nand_chip *this = NULL;
	int rc = 0;
	int dram_sa, dma_len, spare_dram_sa;
	int blank_all_one = 0;
	int page_len;

	unsigned int chip_section = 0;
	unsigned int section = 0;
	unsigned int index = 0, i = 0;
	unsigned int retryCnt = RETRY_COUNT;
	uint8_t auto_trigger_mode = 2;
	uint8_t addr_mode = 1;
	uint8_t bChkAllOne = 0;
	uint8_t read_retry_cnt = 0;
	uint8_t max_read_retry_cnt = 0;

	printk("rtk_read_ecc_page (chipnr=%d, page=0x%x, data_buf addr=0x%x, oob_buf addr=0x%x)\n",
		chipnr, page, data_buf, oob_buf);
	
	this = (struct nand_chip *) mtd->priv;
#ifdef RTK_ROM_CODE
	unsigned char temp_oob_buf[oob_size];
	this->g_oobbuf = oob_buf = temp_oob_buf;
	for(i = 0; i < oob_size; i++)
	{
		this->g_oobbuf[i] = 0;
	}

#endif

	page_size = mtd->oobblock;
	oob_size = mtd->oobsize;
	ppb = mtd->erasesize/mtd->oobblock;

		volatile unsigned int data;


	if (((uint32_t)data_buf&0x7)!=0) {
			printk("[%s]data_buf must 8 byte alignmemt!!\n",__FUNCTION__);
			BUG();
	}

	if (g_enReadRetrial) {
		switch(g_id_chain)
		{
			case 0x4b44642c:
				max_read_retry_cnt = 7;
				break;
			case 0x4b44442c:
				max_read_retry_cnt = 7;
				break;
			case 0x9384d798:
				max_read_retry_cnt = 6;
				break;
			case 0x9384de98:
				max_read_retry_cnt = 6;
				break;
			case 0x9394de98:
				max_read_retry_cnt = 7;
				break;			  
			default:
				break;
				
		}
	}
	//rtk_nand_reset();
	this->select_chip(mtd, chipnr);
#if 0
		int i,id_chain;
		REG_WRITE_U32(REG_DATA_TL0,NF_DATA_TL0_length0(0x00));
		REG_WRITE_U32(REG_DATA_TL1,NF_DATA_TL1_access_mode(0x01) |NF_DATA_TL1_length1(2));
	
		//Set PP
		REG_WRITE_U32(REG_READ_BY_PP,NF_READ_BY_PP_read_by_pp(0x0));
	
		REG_WRITE_U32(REG_PP_CTL0,NF_PP_CTL0_pp_enable(0x01));
		REG_WRITE_U32(REG_PP_CTL1,NF_PP_CTL1_pp_start_addr(0));
	
		//Set command
		REG_WRITE_U32(REG_ND_CMD,NF_ND_CMD_cmd(CMD_PG_READ_C1));
		REG_WRITE_U32(REG_ND_CTL,NF_ND_CTL_xfer(0x01));
		WAIT_DONE(REG_ND_CTL,0x80,0);
	
		//Set address
		REG_WRITE_U32(REG_ND_PA0, page&0xff);
		REG_WRITE_U32(REG_ND_PA1, (page>>8)&0xff);
		REG_WRITE_U32(REG_ND_PA2,NF_ND_PA2_addr_mode(0x01) | (page>>16)&0x1f);
	
		REG_WRITE_U32(REG_ND_CTL,NF_ND_CTL_xfer(1)|NF_ND_CTL_tran_mode(1));
		WAIT_DONE(REG_ND_CTL,0x80,0);
		
		//Set read command 2
		REG_WRITE_U32(REG_ND_CMD,NF_ND_CMD_cmd(CMD_PG_READ_C2));
		REG_WRITE_U32(REG_ND_CTL,NF_ND_CTL_xfer(0x01));
		WAIT_DONE(REG_ND_CTL,0x80,0);	
		WAIT_DONE(REG_ND_CTL,0x40,0x40);
		
		//Enable XFER mode
		REG_WRITE_U32(REG_ND_CTL,NF_ND_CTL_xfer(1)| NF_ND_CTL_ecc_enable(0) |NF_ND_CTL_tran_mode(4));
		WAIT_DONE(REG_ND_CTL,0x80,0);
		//while ( (REG_READ_U32(REG_ND_CTL) & 0x80) != 0 )
		//{
		//	asm("nop");
		//}

		//Reset PP
		REG_WRITE_U32(REG_PP_CTL0,NF_PP_CTL0_pp_reset(1));
	
		//Move data to DRAM from SRAM
		REG_WRITE_U32(REG_SRAM_CTL,NF_SRAM_CTL_map_sel(1)|NF_SRAM_CTL_access_en(1)|NF_SRAM_CTL_mem_region(0));
		
		for(i = 0; i < 512; i +=4)
		{
			id_chain = REG_READ_U32(REG_ND_PA0+i);
			data_buf[i+0] = id_chain & 0xff;
			data_buf[i+1] = (id_chain >> 8) & 0xff;
			data_buf[i+2] = (id_chain >> 16) & 0xff;
			data_buf[i+3] = (id_chain >> 24) & 0xff;
		}
		REG_WRITE_U32(REG_SRAM_CTL,0x0);	//# no omitted
		return 0;
#else
	while (1) 
	{					
		REG_WRITE_U32(REG_DATA_TL0,NF_DATA_TL0_length0(0));
		if(ecc_select >= 0x18)
		{

				REG_WRITE_U32(REG_BLANK_CHK,NF_BLANK_CHK_blank_ena(1)|NF_BLANK_CHK_read_ecc_xnor_ena(0));
								//printk("[DBG]set xnor to 0, REG_BLANK_CHK reg: 0x%x\n", REG_READ_U32(REG_BLANK_CHK));
			
			page_len = 1;//page_size >> 10;
			REG_WRITE_U32(REG_DATA_TL1,NF_DATA_TL1_access_mode(1)|NF_DATA_TL1_length1(4));
		}
		else
		{
			// set random read

			page_len = 1;//page_size >> 9;
			REG_WRITE_U32(REG_DATA_TL1,NF_DATA_TL1_access_mode(1)|NF_DATA_TL1_length1(2));
		}
		REG_WRITE_U32(REG_RND_EN, 0);
		REG_WRITE_U32(REG_PAGE_LEN,NF_PAGE_LEN_page_len(page_len));
#if 1//RANDOMIZER_ECC_CHECK
	if(monflg == 6789)
	{
		//REG_WRITE_U32(REG_RND_EN, 1);
		REG_WRITE_U32(REG_RMZ_CTRL, 1);
		//REG_WRITE_U32(REG_RMZ_SEED_L, 0x0);
		//REG_WRITE_U32(REG_RMZ_SEED_H, 0x0);
		REG_WRITE_U32(REG_RMZ_SEED_CTRL, 0x20);
		//if(monflg == 5566)
		//{
		//	REG_WRITE_U32(REG_RMZ_CTRL, 0);			
		//	REG_WRITE_U32(REG_RMZ_SEED_CTRL, 0x0);
		//}

		printk("[nand test] print seed H : 0x%x, L : 0x%x \n", REG_READ_U32(REG_RMZ_SEED_H), REG_READ_U32(REG_RMZ_SEED_L));
	}
	else
	{
		REG_WRITE_U32(REG_RMZ_CTRL, 0); 		
		REG_WRITE_U32(REG_RMZ_SEED_CTRL, 0x0);
	}
#endif

		//Set PP
		REG_WRITE_U32(REG_READ_BY_PP,NF_READ_BY_PP_read_by_pp(1));
		REG_WRITE_U32(REG_PP_CTL1,NF_PP_CTL1_pp_start_addr(0));
		REG_WRITE_U32(REG_PP_CTL0,NF_PP_CTL0_pp_start_addr(0));

		// enable blank check
		//REG_WRITE_U32(REG_BLANK_CHK,NF_BLANK_CHK_blank_ena(1) );

		//Set command
		REG_WRITE_U32(REG_ND_CMD,NF_ND_CMD_cmd(CMD_PG_READ_C1));
		REG_WRITE_U32(REG_CMD2,NF_CMD2_cmd2(CMD_PG_READ_C2));
		REG_WRITE_U32(REG_CMD3,NF_CMD3_cmd3(CMD_PG_READ_C3));
		
		//Set address
		REG_WRITE_U32( REG_ND_PA0,NF_ND_PA0_page_addr0( 0xff&page ));
		REG_WRITE_U32( REG_ND_PA1,NF_ND_PA1_page_addr1( 0xff&(page>>8) ));
		REG_WRITE_U32( REG_ND_PA2,NF_ND_PA2_addr_mode(addr_mode)|NF_ND_PA2_page_addr2( 0x1f&(page>>16) ));
		REG_WRITE_U32( REG_ND_PA3,NF_ND_PA3_page_addr3( 0x7&(page>>21) ));

		REG_WRITE_U32(REG_ND_CA0, NF_ND_CA0_col_addr0(column));
		REG_WRITE_U32(REG_ND_CA1, NF_ND_CA1_col_addr1(column>>8));

		//Set ECC
		REG_WRITE_U32( REG_MULTI_CHNL_MODE,NF_MULTI_CHNL_MODE_edo(1));//add by alexchang0205-2010.
		if(monflg == 5566)
		{
			printk("set ECC NO CHECK read \n"); 			
			REG_WRITE_U32( REG_MULTI_CHNL_MODE,NF_MULTI_CHNL_MODE_edo(1)|NF_MULTI_CHNL_MODE_ecc_no_check(1) | NF_MULTI_CHNL_MODE_ecc_pass(1));//add by alexchang0205-2010.
			//REG_WRITE_U32(REG_RMZ_CTRL, 1);
		}
		//REG_WRITE_U32( REG_ECC_STOP,NF_ECC_STOP_ecc_n_stop(0x01));
		switch(ecc_select) {
			case 6:
				REG_WRITE_U32(REG_ECC_SEL, 0x0);
				dma_len = 1;
				break;
			case 12:
				REG_WRITE_U32(REG_ECC_SEL, 0x1);
				dma_len = 1;
				break;
			case 16:
				REG_WRITE_U32(REG_ECC_SEL, 0xe);
				dma_len = 2;
				break;
			case 24:
				REG_WRITE_U32(REG_ECC_SEL, 0xa);
				dma_len = 2;
				break;
			case 40:
				REG_WRITE_U32(REG_ECC_SEL, 0x2);
				dma_len = 2;
				break;
			case 43:
				REG_WRITE_U32(REG_ECC_SEL, 0x4);
				dma_len = 2;
				break;
			case 65:
				REG_WRITE_U32(REG_ECC_SEL, 0x6);
				dma_len = 2;
				break;
			case 72:
				REG_WRITE_U32(REG_ECC_SEL, 0x8);
				dma_len = 2;
				break;
			default:
				REG_WRITE_U32(REG_ECC_SEL, 0x0);
				dma_len = 1;
				break;
		}

		if (oob_buf) {
			spare_dram_sa = ( VIR_TO_PHY((uint32_t)oob_buf) >> 3);
		}
		else {
			spare_dram_sa = ( VIR_TO_PHY((uint32_t)this->g_oobbuf) >> 3);
		}
		if(monflg == 5566)
			REG_WRITE_U32( REG_SPR_DDR_CTL,NF_SPR_DDR_CTL_spare_ddr_ena(0)|NF_SPR_DDR_CTL_per_2k_spr_ena(0)|NF_SPR_DDR_CTL_spare_dram_sa(spare_dram_sa));
		else			
			REG_WRITE_U32( REG_SPR_DDR_CTL,NF_SPR_DDR_CTL_spare_ddr_ena(1)|NF_SPR_DDR_CTL_per_2k_spr_ena(0)|NF_SPR_DDR_CTL_spare_dram_sa(spare_dram_sa));



		dram_sa = ( VIR_TO_PHY((uint32_t)data_buf) >> 3);
		REG_WRITE_U32(REG_DMA_CTL1,NF_DMA_CTL1_dram_sa(dram_sa));
		//dma_len = 1;//page_size >> 9;
		REG_WRITE_U32(REG_DMA_CTL2,NF_DMA_CTL2_dma_len(dma_len));
		REG_WRITE_U32(REG_DMA_CTL3,NF_DMA_CTL3_ddr_wr(1)|NF_DMA_CTL3_dma_xfer(1));
#if 0 // no cp for now
				if(cp_mode==CP_NF_AES_CBC_128){
					if(cp_first == 1)
						REG_WRITE_U32(REG_DMA_CTL3,NF_DMA_CTL3_cp_enable(1)|NF_DMA_CTL3_cp_first(1)|NF_DMA_CTL3_ddr_wr(1)|NF_DMA_CTL3_dma_xfer(1));
					else
				REG_WRITE_U32(REG_DMA_CTL3,NF_DMA_CTL3_cp_enable(1)|NF_DMA_CTL3_cp_first(0)|NF_DMA_CTL3_ddr_wr(1)|NF_DMA_CTL3_dma_xfer(1));
				}else if(cp_mode == CP_NF_AES_ECB_128){
						REG_WRITE_U32(REG_DMA_CTL3,NF_DMA_CTL3_cp_enable(1)|NF_DMA_CTL3_cp_first(1)|NF_DMA_CTL3_ddr_wr(1)|NF_DMA_CTL3_dma_xfer(1));
				}else
#endif
				{
						REG_WRITE_U32(REG_DMA_CTL3,NF_DMA_CTL3_ddr_wr(1)|NF_DMA_CTL3_dma_xfer(1));
				}
		
		// flush cache.
		RTK_FLUSH_CACHE(data_buf, page_size);
		RTK_FLUSH_CACHE(oob_buf, oob_size);

		//Enable Auto mode
		REG_WRITE_U32( REG_AUTO_TRIG,NF_AUTO_TRIG_auto_trig(1)|NF_AUTO_TRIG_spec_auto_case(0)| NF_AUTO_TRIG_auto_case(auto_trigger_mode));

		WAIT_DONE(REG_AUTO_TRIG,0x80,0);
		WAIT_DONE(REG_DMA_CTL3,0x01,0);

	#if 0
		if(oob_buf) {
			//syncPageRead(); //stevie
			data = (REG_READ_U32(REG_SPR_DDR_CTL) & 0x0fffffff) << 3;	// physical address
			 //printk("data==> 0x%x\n",data);
			 data |= 0x80000000;
			oob_buf[0] = REG_READ_U32(data + 0x0) & 0xff;
			//printk("oob_buf[0]==> 0x%x, page=0x%x\n",oob_buf[0],page);		   
		}
	#endif

		//rtk_hexdump("data_buf : ", data_buf, page_size);
		//rtk_hexdump("oob_buf : ", oob_buf, oob_size);
		//if(REG_READ_U32(REG_ND_ECC) & 0x4)
		//printk("page: 0x%x, MAX_ECC_NUM: 0x%x\n", page, REG_MAX_ECC_NUM_max_ecc_num(REG_READ_U32(REG_MAX_ECC_NUM)));

		//syncPageRead();
		// return OK if all data bit is 1 (page is not written yet)
		if (REG_READ_U32(REG_BLANK_CHK) & 0x2){
			printk("data all one \n");
			#if 0
			//print OOB data
			//----------------------------
			printk("\n================PRINT OOB=====================\n");
			for(i = 0; i < oob_size; i++)
			{
				if(!(i %16))
					printf("\n");
				printk(" %02x",  oob_buf[i]);				
			}
			printk("\n================PRINT OOB=====================\n");
			rtk_nand_printf_pp_buffer(4);
			#endif
			REG_WRITE_U32(REG_BLANK_CHK,NF_BLANK_CHK_blank_ena(1)|NF_BLANK_CHK_read_ecc_xnor_ena(0));
			return 0;
		}
		else if (REG_READ_U32(REG_ND_ECC) & 0x8) {
			if (!bChkAllOne && this->ecc_select>=0x18) {
				//printk("[DBG]bChkAllOne this->ecc_select: 0x%x\n", this->ecc_select);
				bChkAllOne = 1;
				continue;
			}
			if (g_enReadRetrial) {
				bChkAllOne = 0;
				if(read_retry_cnt == 0){	//set pre condition
					switch(g_id_chain)
					{
					#if 0  //stevie
						case 0x9384d798:
							printk("TC58TEG5DCJT read-retrial rtk_set_pre_condition_toshiba\n");
							rtk_set_pre_condition_toshiba();
							break;
										case 0x9384de98:
							printk("TC58TEG6DCJT read-retrial rtk_set_pre_condition_toshiba\n");
							rtk_set_pre_condition_toshiba();
							break;		 
												case 0x9394de98:
							printk("TC58TEG6DDK read-retrial rtk_set_pre_condition_toshiba\n");
							rtk_set_pre_condition_toshiba();
							break;	  
					#endif
						default:
							break;							
					}
								}			   
				read_retry_cnt ++;
				if (read_retry_cnt <= max_read_retry_cnt) {
					switch(g_id_chain)
					{
					#if 0 //stevie
						case 0x4b44642c:
							printk("MT29F64G08CBABA read-retrial rtk_set_feature_micron ...read_retry_cnt: %d\n", read_retry_cnt);
							rtk_set_feature_micron(read_retry_cnt);
							break;
						case 0x4b44442c:
							printk("MT29F32G08CBADA read-retrial rtk_set_feature_micron ...read_retry_cnt: %d\n", read_retry_cnt);
							rtk_set_feature_micron(read_retry_cnt);
							break;	
						case 0x9384d798:
							printk("TC58TEG5DCJT read-retrial rtk_set_feature_toshiba ...read_retry_cnt: %d, data: 0x%x\n", read_retry_cnt, read_retry_toshiba_cnt[read_retry_cnt]);
							rtk_set_feature_toshiba(read_retry_toshiba_cnt[read_retry_cnt]);
							break;
										case 0x9384de98:
							printk("TC58TEG6DCJT read-retrial rtk_set_feature_toshiba ...read_retry_cnt: %d, data: 0x%x\n", read_retry_cnt, read_retry_toshiba_cnt[read_retry_cnt]);
							rtk_set_feature_toshiba(read_retry_toshiba_cnt[read_retry_cnt]);
											break;
										case 0x9394de98:
							printk("TC58TEG6DDK read-retrial rtk_set_feature_toshiba ...read_retry_cnt: %d\n", read_retry_cnt);
							rtk_set_feature_toshiba_new(read_retry_cnt);
							break;
                    			#endif
						default:
							break;
							
					}
					continue;
				}
			}
			printk("[DBG]ecc error... page=0x%x, REG_BLANK_CHK reg: 0x%x \n", page, REG_READ_U32(REG_BLANK_CHK));
			#if 0
						//print OOB data
			//----------------------------
			printk("\n================PRINT OOB=====================\n");
			for(i = 0; i < oob_size; i++)
			{
				if(!(i %16))
					printf("\n");
				printk(" %02x",  oob_buf[i]);				
			}
			printk("\n================PRINT OOB=====================\n");
			rtk_nand_printf_pp_buffer(4);
			#endif
			if (g_enReadRetrial) {
				if (read_retry_cnt !=0) {
					switch(g_id_chain)
					{
					#if 0  //stevie
						case 0x4b44642c:
							printk("MT29F64G08CBABA read-retrial rtk_set_feature_micron ...reset to 0\n");
							rtk_set_feature_micron(0);
							break;
										case 0x4b44442c:
							printk("MT29F32G08CBADA read-retrial rtk_set_feature_micron ...reset to 0\n");
							rtk_set_feature_micron(0);
							break;
						case 0x9384d798:
							printk("TC58TEG5DCJT read-retrial rtk_set_feature_toshiba ...reset to 0\n");
							rtk_set_feature_toshiba(0);
														rtk_set_terminate_toshiba();
							break;
										case 0x9384de98:
							printk("TC58TEG6DCJT read-retrial rtk_set_feature_toshiba ...reset to 0\n");
							rtk_set_feature_toshiba(0);
														rtk_set_terminate_toshiba();
							break;
												case 0x9394de98:
							printk("TC58TEG6DDK read-retrial rtk_set_feature_toshiba ...reset to 0\n");
							rtk_set_feature_toshiba_new(0);
														rtk_set_terminate_toshiba();
							break;
					#endif
						default:
							
							break;
							
					}
				}
			}
			REG_WRITE_U32(REG_BLANK_CHK,NF_BLANK_CHK_blank_ena(1)|NF_BLANK_CHK_read_ecc_xnor_ena(0));
			return -1;	// ECC not correctable
		}
		else {
			if (g_enReadRetrial) {
				if (read_retry_cnt !=0) {
					switch(g_id_chain)
					{
					#if 0 //stevie
						case 0x4b44642c:
							printk("MT29F64G08CBABA read-retrial rtk_set_feature_micron ...reset to 0\n");
							rtk_set_feature_micron(0);
							break;
										case 0x4b44442c:
							printk("MT29F32G08CBADA read-retrial rtk_set_feature_micron ...reset to 0\n");
							rtk_set_feature_micron(0);
							break;
										case 0x9384d798:
							printk("TC58TEG5DCJT read-retrial rtk_set_feature_toshiba ...reset to 0\n");
							rtk_set_feature_toshiba(0);
														rtk_set_terminate_toshiba();
							break;
										case 0x9384de98:
							printk("TC58TEG6DCJT read-retrial rtk_set_feature_toshiba ...reset to 0\n");
							rtk_set_feature_toshiba(0);
														rtk_set_terminate_toshiba();
							break;
												case 0x9394de98:
							printk("TC58TEG6DDK read-retrial rtk_set_feature_toshiba ...reset to 0\n");
							rtk_set_feature_toshiba_new(0);
														rtk_set_terminate_toshiba();
							break;
					#endif
						default:
							break;
							
					}
				}
			}			 
			REG_WRITE_U32(REG_BLANK_CHK,NF_BLANK_CHK_blank_ena(1)|NF_BLANK_CHK_read_ecc_xnor_ena(0));
			#if 0
			//print OOB data
			//----------------------------
			printk("\n================PRINT OOB=====================\n");
			for(i = 0; i < oob_size; i++)
			{
				if(!(i %16))
					printf("\n");
				printk(" %02x",  oob_buf[i]);				
			}
			printk("\n================PRINT OOB=====================\n");
			rtk_nand_printf_pp_buffer(4);
			#endif
			return 0;
		}
	}

	return rc;
#endif
}

//#define PRINT_OOB
//----------------------------------------------------------------------------
static int rtk_read_ecc_page (struct mtd_info *mtd, u16 chipnr, unsigned int page, u_char *data_buf, u_char *oob_buf)
{
//static unsigned int eccReadCnt = 0;
	struct nand_chip *this = NULL;
	int rc = 0;
	int dram_sa, dma_len, spare_dram_sa;
	int blank_all_one = 0;
	int page_len;

	unsigned int chip_section = 0;
	unsigned int section = 0;
	unsigned int index = 0, i = 0;
	unsigned int retryCnt = RETRY_COUNT;
	uint8_t auto_trigger_mode = 2;
	uint8_t addr_mode = 1;
	uint8_t bChkAllOne = 0;
	uint8_t read_retry_cnt = 0;
	uint8_t max_read_retry_cnt = 0;
	unsigned int is_sync_mode;

	printk("rtk_read_ecc_page (chipnr=%d, page=0x%x, data_buf addr=0x%x, oob_buf addr=0x%x)\n",
		chipnr, page, data_buf, oob_buf);
	
	this = (struct nand_chip *) mtd->priv;
#ifdef RTK_ROM_CODE
	unsigned char temp_oob_buf[oob_size];
	this->g_oobbuf = oob_buf = temp_oob_buf;
	for(i = 0; i < oob_size; i++)
	{
		this->g_oobbuf[i] = 0;
	}

#endif

	page_size = mtd->oobblock;
	oob_size = mtd->oobsize;
	ppb = mtd->erasesize/mtd->oobblock;

		volatile unsigned int data;


	if (((uint32_t)data_buf&0x7)!=0) {
			printk("[%s]data_buf must 8 byte alignmemt!!\n",__FUNCTION__);
			BUG();
	}

	if (g_enReadRetrial) {
		switch(g_id_chain)
		{
			case 0x4b44642c:
				max_read_retry_cnt = 7;
				break;
			case 0x4b44442c:
				max_read_retry_cnt = 7;
				break;
			case 0x9384d798:
				max_read_retry_cnt = 6;
				break;
			case 0x9384de98:
				max_read_retry_cnt = 6;
				break;
			case 0x9394de98:
				max_read_retry_cnt = 7;
				break;			  
			default:
				break;
				
		}
	}
	//rtk_nand_reset();
	//this->select_chip(mtd, chipnr);
#if 0
		int id_chain;
		REG_WRITE_U32(REG_DATA_TL0,NF_DATA_TL0_length0(0x00));
		REG_WRITE_U32(REG_DATA_TL1,NF_DATA_TL1_access_mode(0x01) |NF_DATA_TL1_length1(2));
	
		//Set PP
		REG_WRITE_U32(REG_READ_BY_PP,NF_READ_BY_PP_read_by_pp(0x0));
	
		REG_WRITE_U32(REG_PP_CTL0,NF_PP_CTL0_pp_enable(0x01));
		REG_WRITE_U32(REG_PP_CTL1,NF_PP_CTL1_pp_start_addr(0));
	
		//Set command
		REG_WRITE_U32(REG_ND_CMD,NF_ND_CMD_cmd(CMD_PG_READ_C1));
		REG_WRITE_U32(REG_ND_CTL,NF_ND_CTL_xfer(0x01));
		WAIT_DONE(REG_ND_CTL,0x80,0);
	
		//Set address
		REG_WRITE_U32(REG_ND_PA0, page&0xff);
		REG_WRITE_U32(REG_ND_PA1, (page>>8)&0xff);
		REG_WRITE_U32(REG_ND_PA2,NF_ND_PA2_addr_mode(0x01) | (page>>16)&0x1f);
	
		REG_WRITE_U32(REG_ND_CTL,NF_ND_CTL_xfer(1)|NF_ND_CTL_tran_mode(1));
		WAIT_DONE(REG_ND_CTL,0x80,0);
		
		//Set read command 2
		REG_WRITE_U32(REG_ND_CMD,NF_ND_CMD_cmd(CMD_PG_READ_C2));
		REG_WRITE_U32(REG_ND_CTL,NF_ND_CTL_xfer(0x01));
		WAIT_DONE(REG_ND_CTL,0x80,0);	
		WAIT_DONE(REG_ND_CTL,0x40,0x40);
		
		//Enable XFER mode
		REG_WRITE_U32(REG_ND_CTL,NF_ND_CTL_xfer(1)| NF_ND_CTL_ecc_enable(0) |NF_ND_CTL_tran_mode(4));
		WAIT_DONE(REG_ND_CTL,0x80,0);
		//while ( (REG_READ_U32(REG_ND_CTL) & 0x80) != 0 )
		//{
		//	asm("nop");
		//}

		//Reset PP
		REG_WRITE_U32(REG_PP_CTL0,NF_PP_CTL0_pp_reset(1));
	
		//Move data to DRAM from SRAM
		REG_WRITE_U32(REG_SRAM_CTL,NF_SRAM_CTL_map_sel(1)|NF_SRAM_CTL_access_en(1)|NF_SRAM_CTL_mem_region(0));
		
		for(i = 0; i < 512; i +=4)
		{
			id_chain = REG_READ_U32(REG_ND_PA0+i);
			data_buf[i+0] = id_chain & 0xff;
			data_buf[i+1] = (id_chain >> 8) & 0xff;
			data_buf[i+2] = (id_chain >> 16) & 0xff;
			data_buf[i+3] = (id_chain >> 24) & 0xff;
		}
		REG_WRITE_U32(REG_SRAM_CTL,0x0);	//# no omitted
		return 0;
#else
	while (1) 
	{					
		REG_WRITE_U32(REG_DATA_TL0,NF_DATA_TL0_length0(0));
		if(this->ecc_select >= 0x18)
		{
			if (bChkAllOne) {
				// enable randomizer
				REG_WRITE_U32(REG_RMZ_CTRL, 0);
				REG_WRITE_U32(REG_BLANK_CHK,NF_BLANK_CHK_blank_ena(1)|NF_BLANK_CHK_read_ecc_xnor_ena(1));
								//printk("[DBG]ecc error, set xnor and blank to 1, REG_BLANK_CHK reg: 0x%x\n", REG_READ_U32(REG_BLANK_CHK));
			}
			else {
				// enable randomizer
				REG_WRITE_U32(REG_RMZ_CTRL, 1);
				REG_WRITE_U32(REG_BLANK_CHK,NF_BLANK_CHK_blank_ena(1)|NF_BLANK_CHK_read_ecc_xnor_ena(0));
								//printk("[DBG]set xnor to 0, REG_BLANK_CHK reg: 0x%x\n", REG_READ_U32(REG_BLANK_CHK));
			}
			
			page_len = page_size >> 10;
			REG_WRITE_U32(REG_DATA_TL1,NF_DATA_TL1_access_mode(1)|NF_DATA_TL1_length1(4));
		}
		else
		{
			// set random read

			REG_WRITE_U32(REG_RND_EN, 1);
			REG_WRITE_U32(REG_RND_CMD1, 0x5);
			REG_WRITE_U32(REG_RND_CMD2, 0xe0);
			REG_WRITE_U32(REG_RND_DATA_STR_COL_H, 0);	// data start address MSB (always 0)
			REG_WRITE_U32(REG_RND_SPR_STR_COL_H, page_size >> 8);	// spare start address MSB
			REG_WRITE_U32(REG_RND_SPR_STR_COL_L, page_size & 0xff); // spare start address LSB
			REG_WRITE_U32(REG_RMZ_CTRL, 0);
			
			page_len = page_size >> 9;
			REG_WRITE_U32(REG_DATA_TL1,NF_DATA_TL1_access_mode(1)|NF_DATA_TL1_length1(2));
		}
		
		REG_WRITE_U32(REG_PAGE_LEN,NF_PAGE_LEN_page_len(page_len));

		//Set PP
		REG_WRITE_U32(REG_READ_BY_PP,NF_READ_BY_PP_read_by_pp(1));
		REG_WRITE_U32(REG_PP_CTL1,NF_PP_CTL1_pp_start_addr(0));
		REG_WRITE_U32(REG_PP_CTL0,0);

		// enable blank check
		//REG_WRITE_U32(REG_BLANK_CHK,NF_BLANK_CHK_blank_ena(1) );

		//Set command
		REG_WRITE_U32(REG_ND_CMD,NF_ND_CMD_cmd(CMD_PG_READ_C1));
		REG_WRITE_U32(REG_CMD2,NF_CMD2_cmd2(CMD_PG_READ_C2));
		REG_WRITE_U32(REG_CMD3,NF_CMD3_cmd3(CMD_PG_READ_C3));
		
		//Set address
		REG_WRITE_U32( REG_ND_PA0,NF_ND_PA0_page_addr0( 0xff&page ));
		REG_WRITE_U32( REG_ND_PA1,NF_ND_PA1_page_addr1( 0xff&(page>>8) ));
		REG_WRITE_U32( REG_ND_PA2,NF_ND_PA2_addr_mode(addr_mode)|NF_ND_PA2_page_addr2( 0x1f&(page>>16) ));
		REG_WRITE_U32( REG_ND_PA3,NF_ND_PA3_page_addr3( 0x7&(page>>21) ));

		REG_WRITE_U32(REG_ND_CA0, 0);
		REG_WRITE_U32(REG_ND_CA1, 0);

		
		is_sync_mode = (REG_READ_U32(REG_NF_MODE) & 0x2);
		//Set ECC
		if(monflg == 5566)
		{
			printk("set ECC NO CHECK read \n");				
			REG_WRITE_U32( REG_MULTI_CHNL_MODE,NF_MULTI_CHNL_MODE_edo(1)|NF_MULTI_CHNL_MODE_ecc_no_check(1) | NF_MULTI_CHNL_MODE_ecc_pass(1));//add by alexchang0205-2010.
			//REG_WRITE_U32(REG_RMZ_CTRL, 1);
		}
		else
			REG_WRITE_U32( REG_MULTI_CHNL_MODE,NF_MULTI_CHNL_MODE_edo(1));//add by alexchang0205-2010.
		if(is_sync_mode == 0x2)
			REG_WRITE_U32(REG_MULTI_CHNL_MODE,REG_READ_U32(REG_MULTI_CHNL_MODE) & (~(1<<5)));//sync mode doesn't support edo

		//REG_WRITE_U32( REG_ECC_STOP,NF_ECC_STOP_ecc_n_stop(0x01));
		switch(this->ecc_select) {
			case 6:
				REG_WRITE_U32(REG_ECC_SEL, 0x0);
				break;
			case 12:
				REG_WRITE_U32(REG_ECC_SEL, 0x1);
				break;
			case 16:
				REG_WRITE_U32(REG_ECC_SEL, 0xe);
				break;
			case 24:
				REG_WRITE_U32(REG_ECC_SEL, 0xa);
				break;
			case 40:
				REG_WRITE_U32(REG_ECC_SEL, 0x2);
				break;
			case 43:
				REG_WRITE_U32(REG_ECC_SEL, 0x4);
				break;
			case 65:
				REG_WRITE_U32(REG_ECC_SEL, 0x6);
				break;
			case 72:
				REG_WRITE_U32(REG_ECC_SEL, 0x8);
				break;
			default:
				REG_WRITE_U32(REG_ECC_SEL, 0x0);
				break;
		}

		if (oob_buf) {
			spare_dram_sa = ( VIR_TO_PHY((uint32_t)oob_buf) >> 3);
		}
		else {
			spare_dram_sa = ( VIR_TO_PHY((uint32_t)this->g_oobbuf) >> 3);
		}
		if(monflg == 5566)
			REG_WRITE_U32( REG_SPR_DDR_CTL,NF_SPR_DDR_CTL_spare_ddr_ena(0)|NF_SPR_DDR_CTL_per_2k_spr_ena(0)|NF_SPR_DDR_CTL_spare_dram_sa(spare_dram_sa));
		else			
			REG_WRITE_U32( REG_SPR_DDR_CTL,NF_SPR_DDR_CTL_spare_ddr_ena(1)|NF_SPR_DDR_CTL_per_2k_spr_ena(0)|NF_SPR_DDR_CTL_spare_dram_sa(spare_dram_sa));



		dram_sa = ( VIR_TO_PHY((uint32_t)data_buf) >> 3);
		REG_WRITE_U32(REG_DMA_CTL1,NF_DMA_CTL1_dram_sa(dram_sa));
		dma_len = page_size >> 9;
		REG_WRITE_U32(REG_DMA_CTL2,NF_DMA_CTL2_dma_len(dma_len));
		REG_WRITE_U32(REG_DMA_CTL3,NF_DMA_CTL3_ddr_wr(1)|NF_DMA_CTL3_dma_xfer(1));
#if 0 // no cp for now
		if(cp_mode==CP_NF_AES_CBC_128){
			if(cp_first == 1)
				REG_WRITE_U32(REG_DMA_CTL3,NF_DMA_CTL3_cp_enable(1)|NF_DMA_CTL3_cp_first(1)|NF_DMA_CTL3_ddr_wr(1)|NF_DMA_CTL3_dma_xfer(1));
			else
		REG_WRITE_U32(REG_DMA_CTL3,NF_DMA_CTL3_cp_enable(1)|NF_DMA_CTL3_cp_first(0)|NF_DMA_CTL3_ddr_wr(1)|NF_DMA_CTL3_dma_xfer(1));
		}else if(cp_mode == CP_NF_AES_ECB_128){
				REG_WRITE_U32(REG_DMA_CTL3,NF_DMA_CTL3_cp_enable(1)|NF_DMA_CTL3_cp_first(1)|NF_DMA_CTL3_ddr_wr(1)|NF_DMA_CTL3_dma_xfer(1));
		}else
#endif
		{
				REG_WRITE_U32(REG_DMA_CTL3,NF_DMA_CTL3_ddr_wr(1)|NF_DMA_CTL3_dma_xfer(1));
		}
		
		// flush cache.
		RTK_FLUSH_CACHE(data_buf, page_size);
		RTK_FLUSH_CACHE(oob_buf, oob_size);

		//Enable Auto mode
		REG_WRITE_U32( REG_AUTO_TRIG,NF_AUTO_TRIG_auto_trig(1)|NF_AUTO_TRIG_spec_auto_case(0)| NF_AUTO_TRIG_auto_case(auto_trigger_mode));

		WAIT_DONE(REG_AUTO_TRIG,0x80,0);
		WAIT_DONE(REG_DMA_CTL3,0x01,0);

		#if 0
		if(oob_buf) {
			//syncPageRead(); //stevie
			data = (REG_READ_U32(REG_SPR_DDR_CTL) & 0x0fffffff) << 3;	// physical address
			 //printk("data==> 0x%x\n",data);
			 data |= 0x80000000;
			oob_buf[0] = REG_READ_U32(data + 0x0) & 0xff;
			//printk("oob_buf[0]==> 0x%x, page=0x%x\n",oob_buf[0],page);		   
		}
		#endif

		//rtk_hexdump("data_buf : ", data_buf, page_size);
		//rtk_hexdump("oob_buf : ", oob_buf, oob_size);
		//if(REG_READ_U32(REG_ND_ECC) & 0x4)
		//printk("page: 0x%x, MAX_ECC_NUM: 0x%x\n", page, REG_MAX_ECC_NUM_max_ecc_num(REG_READ_U32(REG_MAX_ECC_NUM)));

		//syncPageRead();
		// return OK if all data bit is 1 (page is not written yet)
		if (REG_READ_U32(REG_BLANK_CHK) & 0x2){
			printk("data all one \n");
			#ifdef PRINT_OOB
			//print OOB data
			//----------------------------
			printk("\n================PRINT OOB=====================\n");
			for(i = 0; i < oob_size; i++)
			{
				if(!(i %16))
					printf("\n");
				printk(" %02x",  oob_buf[i]);				
			}
			printk("\n================PRINT OOB=====================\n");
			#endif
			REG_WRITE_U32(REG_BLANK_CHK,NF_BLANK_CHK_blank_ena(1)|NF_BLANK_CHK_read_ecc_xnor_ena(0));
			return 0;
		}
		else if (REG_READ_U32(REG_ND_ECC) & 0x8) {
			if (!bChkAllOne && this->ecc_select>=0x18) {
				//printk("[DBG]bChkAllOne this->ecc_select: 0x%x\n", this->ecc_select);
				bChkAllOne = 1;
				continue;
			}
			if (g_enReadRetrial) {
				bChkAllOne = 0;
				if(read_retry_cnt == 0){	//set pre condition
					switch(g_id_chain)
					{
						#if 0  //stevie
						case 0x9384d798:
							printk("TC58TEG5DCJT read-retrial rtk_set_pre_condition_toshiba\n");
							rtk_set_pre_condition_toshiba();
							break;
										case 0x9384de98:
							printk("TC58TEG6DCJT read-retrial rtk_set_pre_condition_toshiba\n");
							rtk_set_pre_condition_toshiba();
							break;		 
												case 0x9394de98:
							printk("TC58TEG6DDK read-retrial rtk_set_pre_condition_toshiba\n");
							rtk_set_pre_condition_toshiba();
							break;	  
						#endif
						default:
							break;							
					}
								}			   
				read_retry_cnt ++;
				if (read_retry_cnt <= max_read_retry_cnt) {
					switch(g_id_chain)
					{
						
						case 0x4b44642c:
							printk("MT29F64G08CBABA read-retrial rtk_set_feature_micron ...read_retry_cnt: %d\n", read_retry_cnt);
							rtk_set_feature_micron(read_retry_cnt);
							break;
						#if 0 //stevie
						case 0x4b44442c:
							printk("MT29F32G08CBADA read-retrial rtk_set_feature_micron ...read_retry_cnt: %d\n", read_retry_cnt);
							rtk_set_feature_micron(read_retry_cnt);
							break;	
						case 0x9384d798:
							printk("TC58TEG5DCJT read-retrial rtk_set_feature_toshiba ...read_retry_cnt: %d, data: 0x%x\n", read_retry_cnt, read_retry_toshiba_cnt[read_retry_cnt]);
							rtk_set_feature_toshiba(read_retry_toshiba_cnt[read_retry_cnt]);
							break;
										case 0x9384de98:
							printk("TC58TEG6DCJT read-retrial rtk_set_feature_toshiba ...read_retry_cnt: %d, data: 0x%x\n", read_retry_cnt, read_retry_toshiba_cnt[read_retry_cnt]);
							rtk_set_feature_toshiba(read_retry_toshiba_cnt[read_retry_cnt]);
											break;
										case 0x9394de98:
							printk("TC58TEG6DDK read-retrial rtk_set_feature_toshiba ...read_retry_cnt: %d\n", read_retry_cnt);
							rtk_set_feature_toshiba_new(read_retry_cnt);
							break;
                        			#endif
						default:
							break;
							
					}
					continue;
				}
			}
			printk("[DBG]ecc error... page=0x%x, REG_BLANK_CHK reg: 0x%x \n", page, REG_READ_U32(REG_BLANK_CHK));
			#ifdef PRINT_OOB
						//print OOB data
			//----------------------------
			printk("\n================PRINT OOB=====================\n");
			for(i = 0; i < oob_size; i++)
			{
				if(!(i %16))
					printf("\n");
				printk(" %02x",  oob_buf[i]);				
			}
			printk("\n================PRINT OOB=====================\n");
			#endif
			if (g_enReadRetrial) {
				if (read_retry_cnt !=0) {
					switch(g_id_chain)
					{
						
						case 0x4b44642c:
							printk("MT29F64G08CBABA read-retrial rtk_set_feature_micron ...reset to 0\n");
							rtk_set_feature_micron(0);
							break;
						#if 0  //stevie
						case 0x4b44442c:
							printk("MT29F32G08CBADA read-retrial rtk_set_feature_micron ...reset to 0\n");
							rtk_set_feature_micron(0);
							break;
						case 0x9384d798:
							printk("TC58TEG5DCJT read-retrial rtk_set_feature_toshiba ...reset to 0\n");
							rtk_set_feature_toshiba(0);
														rtk_set_terminate_toshiba();
							break;
										case 0x9384de98:
							printk("TC58TEG6DCJT read-retrial rtk_set_feature_toshiba ...reset to 0\n");
							rtk_set_feature_toshiba(0);
														rtk_set_terminate_toshiba();
							break;
												case 0x9394de98:
							printk("TC58TEG6DDK read-retrial rtk_set_feature_toshiba ...reset to 0\n");
							rtk_set_feature_toshiba_new(0);
														rtk_set_terminate_toshiba();
							break;
						#endif
						default:
							
							break;
							
					}
				}
			}
			REG_WRITE_U32(REG_BLANK_CHK,NF_BLANK_CHK_blank_ena(1)|NF_BLANK_CHK_read_ecc_xnor_ena(0));
			return -1;	// ECC not correctable
		}
		else {
			if (g_enReadRetrial) {
				if (read_retry_cnt !=0) {
					switch(g_id_chain)
					{
						
						case 0x4b44642c:
							printk("MT29F64G08CBABA read-retrial rtk_set_feature_micron ...reset to 0\n");
							rtk_set_feature_micron(0);
							break;
						#if 0 //stevie
										case 0x4b44442c:
							printk("MT29F32G08CBADA read-retrial rtk_set_feature_micron ...reset to 0\n");
							rtk_set_feature_micron(0);
							break;
										case 0x9384d798:
							printk("TC58TEG5DCJT read-retrial rtk_set_feature_toshiba ...reset to 0\n");
							rtk_set_feature_toshiba(0);
														rtk_set_terminate_toshiba();
							break;
										case 0x9384de98:
							printk("TC58TEG6DCJT read-retrial rtk_set_feature_toshiba ...reset to 0\n");
							rtk_set_feature_toshiba(0);
														rtk_set_terminate_toshiba();
							break;
												case 0x9394de98:
							printk("TC58TEG6DDK read-retrial rtk_set_feature_toshiba ...reset to 0\n");
							rtk_set_feature_toshiba_new(0);
														rtk_set_terminate_toshiba();
							break;
						#endif
						default:
							break;
							
					}
				}
			}			 
			REG_WRITE_U32(REG_BLANK_CHK,NF_BLANK_CHK_blank_ena(1)|NF_BLANK_CHK_read_ecc_xnor_ena(0));
			#ifdef PRINT_OOB
			//print OOB data
			//----------------------------
			printk("\n================PRINT OOB=====================\n");
			for(i = 0; i < oob_size; i++)
			{
				if(!(i %16))
					printf("\n");
				printk(" %02x",  oob_buf[i]);				
			}
			printk("\n================PRINT OOB=====================\n");
			#endif
			return 0;
		}
	}

	return rc;
#endif
}


//----------------------------------------------------------------------------
static int rtk_read_ecc_page_ori (struct mtd_info *mtd, u16 chipnr, unsigned int page, u_char *data_buf, u_char *oob_buf, u16 cp_mode, u16 cp_first, size_t cp_len)
{
//static unsigned int eccReadCnt = 0;
	struct nand_chip *this = NULL;
	int rc = 0;
	int dram_sa, dma_len, spare_dram_sa;
	int blank_all_one = 0;
	int page_len;

	unsigned int chip_section = 0;
	unsigned int section = 0;
	unsigned int index = 0;
	unsigned int retryCnt = RETRY_COUNT;
	uint8_t	auto_trigger_mode = 2;
	uint8_t	addr_mode = 1;
	uint8_t	bChkAllOne = 0;
	uint8_t read_retry_cnt = 0;
	uint8_t max_read_retry_cnt = 0;

	//printk("rtk_read_ecc_page (chipnr=%d, page=0x%x, data_buf addr=0x%x, oob_buf addr=0x%x)\n",
	//	chipnr, page, data_buf, oob_buf);
	
	this = (struct nand_chip *) mtd->priv;
	page_size = mtd->oobblock;
	oob_size = mtd->oobsize;
	ppb = mtd->erasesize/mtd->oobblock;

        volatile unsigned int data;

        unsigned char ks[16];
        unsigned char kh[16];
        unsigned char dest[16];
        unsigned int *intPTR;

    if (((uint32_t)data_buf&0x7)!=0) {
            printk("[%s]data_buf must 8 byte alignmemt!!\n",__FUNCTION__);
            BUG();
    }

	if (g_enReadRetrial) {
		switch(g_id_chain)
		{
			case 0x4b44642c:
				max_read_retry_cnt = 7;
				break;
			case 0x4b44442c:
				max_read_retry_cnt = 7;
				break;
			case 0x9384d798:
                                max_read_retry_cnt = 6;
				break;
                       case 0x9384de98:
                                max_read_retry_cnt = 6;
				break;
                       case 0x9394de98:
                                max_read_retry_cnt = 7;
				break;            
			default:
				break;
				
		}
	}

	while (1) 
	{                   
	#if 0  //stevie
			if (g_enReadRetrial) {
				if (read_retry_cnt !=0) {   //set extra feature before retry read page. ex. toshiba 0x26&0x5D
					switch(g_id_chain)
					{
						case 0x9384d798:
							printk("TC58TEG5DCJT read-retrial rtk_set_ext_feature_toshiba ...read_retry_cnt: %d\n", read_retry_cnt);
                                                        rtk_set_ext_feature_toshiba();
							break;
                            			case 0x9384de98:
							printk("TC58TEG6DCJT read-retrial rtk_set_ext_feature_toshiba ...read_retry_cnt: %d\n", read_retry_cnt);
                                                        rtk_set_ext_feature_toshiba();
							break;
                                                case 0x9394de98:
							printk("TC58TEG6DDK read-retrial rtk_set_ext_feature_toshiba ...read_retry_cnt: %d\n", read_retry_cnt);
                                                        rtk_set_ext_feature_toshiba();
							break;
						default:
							break;
							
					}
				}
			}
    	#endif
		REG_WRITE_U32(REG_DATA_TL0,NF_DATA_TL0_length0(0));
		if(this->ecc_select >= 0x18)
		{
			if (bChkAllOne) {
				// enable randomizer
				REG_WRITE_U32(REG_RMZ_CTRL, 0);
				REG_WRITE_U32(REG_BLANK_CHK,NF_BLANK_CHK_blank_ena(1)|NF_BLANK_CHK_read_ecc_xnor_ena(1));
                                //printk("[DBG]ecc error, set xnor and blank to 1, REG_BLANK_CHK reg: 0x%x\n", REG_READ_U32(REG_BLANK_CHK));
			}
			else {
				// enable randomizer
				REG_WRITE_U32(REG_RMZ_CTRL, 1);
				REG_WRITE_U32(REG_BLANK_CHK,NF_BLANK_CHK_blank_ena(1)|NF_BLANK_CHK_read_ecc_xnor_ena(0));
                                //printk("[DBG]set xnor to 0, REG_BLANK_CHK reg: 0x%x\n", REG_READ_U32(REG_BLANK_CHK));
			}
			
			page_len = page_size >> 10;
			REG_WRITE_U32(REG_DATA_TL1,NF_DATA_TL1_access_mode(1)|NF_DATA_TL1_length1(4));
		}
		else
		{
			// set random read
			REG_WRITE_U32(REG_RND_EN, 1);
			REG_WRITE_U32(REG_RND_CMD1, 0x5);
			REG_WRITE_U32(REG_RND_CMD2, 0xe0);
			REG_WRITE_U32(REG_RND_DATA_STR_COL_H, 0);	// data start address MSB (always 0)
			REG_WRITE_U32(REG_RND_SPR_STR_COL_H, page_size >> 8);	// spare start address MSB
			REG_WRITE_U32(REG_RND_SPR_STR_COL_L, page_size & 0xff);	// spare start address LSB
			
			page_len = page_size >> 9;
			REG_WRITE_U32(REG_DATA_TL1,NF_DATA_TL1_access_mode(1)|NF_DATA_TL1_length1(2));
		}
		
		REG_WRITE_U32(REG_PAGE_LEN,NF_PAGE_LEN_page_len(page_len));

		//Set PP
		REG_WRITE_U32(REG_READ_BY_PP,NF_READ_BY_PP_read_by_pp(1));
		REG_WRITE_U32(REG_PP_CTL1,NF_PP_CTL1_pp_start_addr(0));
		REG_WRITE_U32(REG_PP_CTL0,0);

		// enable blank check
		//REG_WRITE_U32(REG_BLANK_CHK,NF_BLANK_CHK_blank_ena(1) );

#if 0 //stevie
                if(cp_mode == CP_NF_AES_ECB_128||(cp_mode==CP_NF_AES_CBC_128 && cp_first==1)){
#ifdef CONFIG_NAND_ON_THE_FLY_TEST_KEY

                    if(cp_mode==CP_NF_AES_CBC_128){
            		    REG_WRITE_U32(CP_NF_INI_KEY_0,0x0);
			    REG_WRITE_U32(CP_NF_INI_KEY_1,0x0);
			    REG_WRITE_U32(CP_NF_INI_KEY_2,0x0);
			    REG_WRITE_U32(CP_NF_INI_KEY_3,0x0);
			}

			REG_WRITE_U32(CP_NF_KEY_0,0xad0d8175);
			REG_WRITE_U32(CP_NF_KEY_1,0xa0d732c0);
			REG_WRITE_U32(CP_NF_KEY_2,0xe56ef350);
			REG_WRITE_U32(CP_NF_KEY_3,0xc53ce48b);
        	        // set CP register.

                        if(cp_mode == CP_NF_AES_ECB_128){
        	                REG_WRITE_U32(CP_NF_SET, 0x200);				// sel=0, cw_entry=0, bcm=1, aes_mode=0. Its ECB mode.
                        }
                        else{
                                REG_WRITE_U32(CP_NF_SET, 0x0);				// sel=0, cw_entry=0, bcm=0, aes_mode=0. Its CBC mode.
                        }       
            
#else
        	        // set CP register.

                        if(cp_mode == CP_NF_AES_ECB_128){
        	                //REG_WRITE_U32(CP_NF_SET, 0x200);				// sel=0, cw_entry=0, bcm=1, aes_mode=0. Its ECB mode.
                                    if (!OTP_JUDGE_BIT(OTP_BIT_SECUREBOOT)) 
                                      REG_WRITE_U32(CP_NF_SET, 0x202); // key from kn
                                    else if (OTP_JUDGE_BIT(OTP_BIT_SECUREBOOT)){

#ifdef CONFIG_CMD_KEY_BURNING
                                        OTP_Get_Byte(OTP_K_S, ks, 16);
                                        OTP_Get_Byte(OTP_K_H, kh, 16);
#endif
                                        AES_ECB_encrypt(ks, 16, dest, kh);

					intPTR = dest;
					intPTR[0] = swap_endian(intPTR[0]);
					intPTR[1] = swap_endian(intPTR[1]);
					intPTR[2] = swap_endian(intPTR[2]);
					intPTR[3] = swap_endian(intPTR[3]);

                                        writeNFKey(dest);

                                        memset(ks, 0, 16);
                                        memset(kh, 0, 16);
                                        memset(dest, 0, 16);

                                        REG_WRITE_U32(CP_NF_SET, 0x200);				// sel=0, cw_entry=0, bcm=1, aes_mode=0. Its ECB mode.
                                      //REG_WRITE_U32(CP_NF_SET, 0x2202); // key from kh

                                    }
                        }
                        else{
                                //REG_WRITE_U32(CP_NF_SET, 0x0);				// sel=0, cw_entry=0, bcm=0, aes_mode=0. Its CBC mode.
                                    if (!OTP_JUDGE_BIT(OTP_BIT_SECUREBOOT)) 
                                      REG_WRITE_U32(CP_NF_SET, 0x002); // key from kn
                                    else if (OTP_JUDGE_BIT(OTP_BIT_SECUREBOOT)){

                            		REG_WRITE_U32(CP_NF_INI_KEY_0,0x0);
                			REG_WRITE_U32(CP_NF_INI_KEY_1,0x0);
                			REG_WRITE_U32(CP_NF_INI_KEY_2,0x0);
                			REG_WRITE_U32(CP_NF_INI_KEY_3,0x0);

#ifdef CONFIG_CMD_KEY_BURNING
                                        OTP_Get_Byte(OTP_K_S, ks, 16);
                                        OTP_Get_Byte(OTP_K_H, kh, 16);
#endif
                                        AES_ECB_encrypt(ks, 16, dest, kh);
					
					intPTR = dest;
					intPTR[0] = swap_endian(intPTR[0]);
					intPTR[1] = swap_endian(intPTR[1]);
					intPTR[2] = swap_endian(intPTR[2]);
					intPTR[3] = swap_endian(intPTR[3]);

                                        writeNFKey(dest);

                                        memset(ks, 0, 16);
                                        memset(kh, 0, 16);
                                        memset(dest, 0, 16);

                                        REG_WRITE_U32(CP_NF_SET, 0x0);				// sel=0, cw_entry=0, bcm=0, aes_mode=0. Its CBC mode.
                                      //REG_WRITE_U32(CP_NF_SET, 0x2002); // key from kh

                                    }

                        }        
#endif                          
                        //printk("[DBG]set cp mode...............cp_len: 0x%x\n", cp_len);

                        if(cp_mode == CP_NF_AES_CBC_128 )
                            REG_WRITE_U32(REG_CP_LEN, (cp_len / 0x200) << 9);		// integer multiple of dma_len.
                        else
                            REG_WRITE_U32(REG_CP_LEN, (page_size / 0x200) << 9);		// integer multiple of dma_len.
                            
                        syncPageRead();
                        //printk("[DBG]set cp mode...............REG_READ_U32(REG_CP_LEN): 0x%x\n", REG_READ_U32(REG_CP_LEN));
                }
#endif
		//Set command
		REG_WRITE_U32(REG_ND_CMD,NF_ND_CMD_cmd(CMD_PG_READ_C1));
		REG_WRITE_U32(REG_CMD2,NF_CMD2_cmd2(CMD_PG_READ_C2));
		REG_WRITE_U32(REG_CMD3,NF_CMD3_cmd3(CMD_PG_READ_C3));
		
		//Set address
		REG_WRITE_U32( REG_ND_PA0,NF_ND_PA0_page_addr0( 0xff&page ));
		REG_WRITE_U32( REG_ND_PA1,NF_ND_PA1_page_addr1( 0xff&(page>>8) ));
		REG_WRITE_U32( REG_ND_PA2,NF_ND_PA2_addr_mode(addr_mode)|NF_ND_PA2_page_addr2( 0x1f&(page>>16) ));
		REG_WRITE_U32( REG_ND_PA3,NF_ND_PA3_page_addr3( 0x7&(page>>21) ));

		REG_WRITE_U32(REG_ND_CA0, 0);
		REG_WRITE_U32(REG_ND_CA1, 0);

		//Set ECC
		REG_WRITE_U32( REG_MULTI_CHNL_MODE,NF_MULTI_CHNL_MODE_edo(1));//add by alexchang0205-2010
		REG_WRITE_U32( REG_ECC_STOP,NF_ECC_STOP_ecc_n_stop(0x01));

		switch(this->ecc_select) {
			case 6:
				REG_WRITE_U32(REG_ECC_SEL, 0x0);
				break;
			case 12:
				REG_WRITE_U32(REG_ECC_SEL, 0x1);
				break;
			case 16:
				REG_WRITE_U32(REG_ECC_SEL, 0xe);
				break;
			case 24:
				REG_WRITE_U32(REG_ECC_SEL, 0xa);
				break;
			case 40:
				REG_WRITE_U32(REG_ECC_SEL, 0x2);
				break;
			case 43:
				REG_WRITE_U32(REG_ECC_SEL, 0x4);
				break;
			case 65:
				REG_WRITE_U32(REG_ECC_SEL, 0x6);
				break;
			case 72:
				REG_WRITE_U32(REG_ECC_SEL, 0x8);
				break;
			default:
				REG_WRITE_U32(REG_ECC_SEL, 0x0);
				break;
		}

		if (oob_buf) {
			spare_dram_sa = ( (uint32_t)oob_buf >> 3);
		}
		else {
			spare_dram_sa = ( (uint32_t)this->g_oobbuf >> 3);
		}

		REG_WRITE_U32( REG_SPR_DDR_CTL,NF_SPR_DDR_CTL_spare_ddr_ena(1)|NF_SPR_DDR_CTL_per_2k_spr_ena(0)|NF_SPR_DDR_CTL_spare_dram_sa(spare_dram_sa));



		dram_sa = ( (uint32_t)data_buf >> 3);
		REG_WRITE_U32(REG_DMA_CTL1,NF_DMA_CTL1_dram_sa(dram_sa));
		dma_len = page_size >> 9;
		REG_WRITE_U32(REG_DMA_CTL2,NF_DMA_CTL2_dma_len(dma_len));
		//REG_WRITE_U32(REG_DMA_CTL3,NF_DMA_CTL3_ddr_wr(1)|NF_DMA_CTL3_dma_xfer(1));

                if(cp_mode==CP_NF_AES_CBC_128){
                    if(cp_first == 1)
                        REG_WRITE_U32(REG_DMA_CTL3,NF_DMA_CTL3_cp_enable(1)|NF_DMA_CTL3_cp_first(1)|NF_DMA_CTL3_ddr_wr(1)|NF_DMA_CTL3_dma_xfer(1));
                    else
		        REG_WRITE_U32(REG_DMA_CTL3,NF_DMA_CTL3_cp_enable(1)|NF_DMA_CTL3_cp_first(0)|NF_DMA_CTL3_ddr_wr(1)|NF_DMA_CTL3_dma_xfer(1));
                }else if(cp_mode == CP_NF_AES_ECB_128){
                        REG_WRITE_U32(REG_DMA_CTL3,NF_DMA_CTL3_cp_enable(1)|NF_DMA_CTL3_cp_first(1)|NF_DMA_CTL3_ddr_wr(1)|NF_DMA_CTL3_dma_xfer(1));
                }else{
                        REG_WRITE_U32(REG_DMA_CTL3,NF_DMA_CTL3_ddr_wr(1)|NF_DMA_CTL3_dma_xfer(1));
                }
		
		// flush cache.
		RTK_FLUSH_CACHE(data_buf, page_size);
		RTK_FLUSH_CACHE(oob_buf, oob_size);


		//Enable Auto mode
		REG_WRITE_U32( REG_AUTO_TRIG,NF_AUTO_TRIG_auto_trig(1)|NF_AUTO_TRIG_spec_auto_case(0)| NF_AUTO_TRIG_auto_case(auto_trigger_mode));

		WAIT_DONE(REG_AUTO_TRIG,0x80,0);
		WAIT_DONE(REG_DMA_CTL3,0x01,0);

		if(oob_buf)	{
#if 0
			REG_WRITE_U32(REG_READ_BY_PP,0x00);
			REG_WRITE_U32(REG_SRAM_CTL, 0x30 | 0x04);
			rtk_read_oob_from_SRAM(mtd, oob_buf);
			REG_WRITE_U32(REG_SRAM_CTL, 0x00);
			REG_WRITE_U32(REG_READ_BY_PP,0x80);
#endif
                        //syncPageRead(); //stevie

    		        data = (REG_READ_U32(REG_SPR_DDR_CTL) & 0x0fffffff) << 3;	// physical address
    		         //printk("data==> 0x%x\n",data);
		        oob_buf[0] = REG_READ_U32(data + 0x0) & 0xff;
                        //printk("oob_buf[0]==> 0x%x, page=0x%x\n",oob_buf[0],page);           
		}

		//rtk_hexdump("data_buf : ", data_buf, page_size);
		//rtk_hexdump("oob_buf : ", oob_buf, oob_size);
                //if(REG_READ_U32(REG_ND_ECC) & 0x4)
                    //printk("page: 0x%x, MAX_ECC_NUM: 0x%x\n", page, REG_MAX_ECC_NUM_max_ecc_num(REG_READ_U32(REG_MAX_ECC_NUM)));

                //syncPageRead();
		// return OK if all data bit is 1 (page is not written yet)
		if (REG_READ_U32(REG_BLANK_CHK) & 0x2){
			//printk("data all one \n");
			if (g_enReadRetrial) {
				if (read_retry_cnt !=0) {
					switch(g_id_chain)
					{
					#if 0 //stevie
						case 0x4b44642c:
							printk("MT29F64G08CBABA read-retrial rtk_set_feature_micron ...reset to 0\n");
							rtk_set_feature_micron(0);
							break;
                            			case 0x4b44442c:
							printk("MT29F32G08CBADA read-retrial rtk_set_feature_micron ...reset to 0\n");
							rtk_set_feature_micron(0);
							break;
						case 0x9384d798:
							printk("TC58TEG5DCJT read-retrial rtk_set_feature_toshiba ...reset to 0\n");
							rtk_set_feature_toshiba(0);
                                                        rtk_set_terminate_toshiba();
							break;
                            			case 0x9384de98:
							printk("TC58TEG6DCJT read-retrial rtk_set_feature_toshiba ...reset to 0\n");
							rtk_set_feature_toshiba(0);
                                                        rtk_set_terminate_toshiba();
							break;
                                                case 0x9394de98:
							printk("TC58TEG6DDK read-retrial rtk_set_feature_toshiba ...reset to 0\n");
							rtk_set_feature_toshiba_new(0);
                                                        rtk_set_terminate_toshiba();
							break;
                            
						default:
							break;
					#endif
					}
				}
			}
			REG_WRITE_U32(REG_BLANK_CHK,NF_BLANK_CHK_blank_ena(1)|NF_BLANK_CHK_read_ecc_xnor_ena(0));
			return 0;
		}
		else if (REG_READ_U32(REG_ND_ECC) & 0x8) {
			if (!bChkAllOne && this->ecc_select>=0x18) {
                                //printk("[DBG]bChkAllOne this->ecc_select: 0x%x\n", this->ecc_select);
				bChkAllOne = 1;
				continue;
			}
			if (g_enReadRetrial) {
				bChkAllOne = 0;
                                if(read_retry_cnt == 0){    //set pre condition
					switch(g_id_chain)
					{
					#if 0  //stevie
						case 0x9384d798:
							printk("TC58TEG5DCJT read-retrial rtk_set_pre_condition_toshiba\n");
							rtk_set_pre_condition_toshiba();
							break;
                            			case 0x9384de98:
							printk("TC58TEG6DCJT read-retrial rtk_set_pre_condition_toshiba\n");
							rtk_set_pre_condition_toshiba();
							break;       
                                                case 0x9394de98:
							printk("TC58TEG6DDK read-retrial rtk_set_pre_condition_toshiba\n");
							rtk_set_pre_condition_toshiba();
							break;    
					#endif
						default:
							break;							
					}
                                }              
				read_retry_cnt ++;
				if (read_retry_cnt <= max_read_retry_cnt) {
					switch(g_id_chain)
					{
					#if 0 //stevie
						case 0x4b44642c:
							printk("MT29F64G08CBABA read-retrial rtk_set_feature_micron ...read_retry_cnt: %d\n", read_retry_cnt);
							rtk_set_feature_micron(read_retry_cnt);
							break;
						case 0x4b44442c:
							printk("MT29F32G08CBADA read-retrial rtk_set_feature_micron ...read_retry_cnt: %d\n", read_retry_cnt);
							rtk_set_feature_micron(read_retry_cnt);
							break;	
						case 0x9384d798:
							printk("TC58TEG5DCJT read-retrial rtk_set_feature_toshiba ...read_retry_cnt: %d, data: 0x%x\n", read_retry_cnt, read_retry_toshiba_cnt[read_retry_cnt]);
							rtk_set_feature_toshiba(read_retry_toshiba_cnt[read_retry_cnt]);
							break;
                            			case 0x9384de98:
							printk("TC58TEG6DCJT read-retrial rtk_set_feature_toshiba ...read_retry_cnt: %d, data: 0x%x\n", read_retry_cnt, read_retry_toshiba_cnt[read_retry_cnt]);
							rtk_set_feature_toshiba(read_retry_toshiba_cnt[read_retry_cnt]);
                            				break;
                            			case 0x9394de98:
							printk("TC58TEG6DDK read-retrial rtk_set_feature_toshiba ...read_retry_cnt: %d\n", read_retry_cnt);
							rtk_set_feature_toshiba_new(read_retry_cnt);
							break;
                            		#endif
						default:
							break;
							
					}
					continue;
				}
			}
			printk("[DBG]ecc error... page=0x%x, REG_BLANK_CHK reg: 0x%x \n", page, REG_READ_U32(REG_BLANK_CHK));
			if (g_enReadRetrial) {
				if (read_retry_cnt !=0) {
					switch(g_id_chain)
					{
					#if 0  //stevie
						case 0x4b44642c:
							printk("MT29F64G08CBABA read-retrial rtk_set_feature_micron ...reset to 0\n");
							rtk_set_feature_micron(0);
							break;
                            			case 0x4b44442c:
							printk("MT29F32G08CBADA read-retrial rtk_set_feature_micron ...reset to 0\n");
							rtk_set_feature_micron(0);
							break;
						case 0x9384d798:
							printk("TC58TEG5DCJT read-retrial rtk_set_feature_toshiba ...reset to 0\n");
							rtk_set_feature_toshiba(0);
                                                        rtk_set_terminate_toshiba();
							break;
                            			case 0x9384de98:
							printk("TC58TEG6DCJT read-retrial rtk_set_feature_toshiba ...reset to 0\n");
							rtk_set_feature_toshiba(0);
                                                        rtk_set_terminate_toshiba();
							break;
                                                case 0x9394de98:
							printk("TC58TEG6DDK read-retrial rtk_set_feature_toshiba ...reset to 0\n");
							rtk_set_feature_toshiba_new(0);
                                                        rtk_set_terminate_toshiba();
							break;
					#endif
						default:
                            
							break;
							
					}
				}
			}
                        REG_WRITE_U32(REG_BLANK_CHK,NF_BLANK_CHK_blank_ena(1)|NF_BLANK_CHK_read_ecc_xnor_ena(0));
                        return -1;	// ECC not correctable
		}
		else {
			if (g_enReadRetrial) {
				if (read_retry_cnt !=0) {
					switch(g_id_chain)
					{
					#if 0 //stevie
						case 0x4b44642c:
							printk("MT29F64G08CBABA read-retrial rtk_set_feature_micron ...reset to 0\n");
							rtk_set_feature_micron(0);
							break;
                            			case 0x4b44442c:
							printk("MT29F32G08CBADA read-retrial rtk_set_feature_micron ...reset to 0\n");
							rtk_set_feature_micron(0);
							break;
                            			case 0x9384d798:
							printk("TC58TEG5DCJT read-retrial rtk_set_feature_toshiba ...reset to 0\n");
							rtk_set_feature_toshiba(0);
                                                        rtk_set_terminate_toshiba();
							break;
                            			case 0x9384de98:
							printk("TC58TEG6DCJT read-retrial rtk_set_feature_toshiba ...reset to 0\n");
							rtk_set_feature_toshiba(0);
                                                        rtk_set_terminate_toshiba();
							break;
                                                case 0x9394de98:
							printk("TC58TEG6DDK read-retrial rtk_set_feature_toshiba ...reset to 0\n");
							rtk_set_feature_toshiba_new(0);
                                                        rtk_set_terminate_toshiba();
							break;
					#endif
						default:
							break;
							
					}
				}
			}            
                        REG_WRITE_U32(REG_BLANK_CHK,NF_BLANK_CHK_blank_ena(1)|NF_BLANK_CHK_read_ecc_xnor_ena(0));
			return 0;
		}
	}

	return rc;
}

//----------------------------------------------------------------------------
static void rtk_nand_read_id2(struct mtd_info *mtd, u_char id[512])
{
//#define XFER_MODE
	int id_chain;
	printk("%s \n", __FUNCTION__);
	//Set SRAM path and access mode
	if(((uint32_t)id&0x7)!=0) {
		printk("%s, buffer address not 8 byte aligned \n", __FUNCTION__);
	}
		
	REG_WRITE_U32(REG_PP_CTL0,NF_PP_CTL0_pp_reset(0x01));
	REG_WRITE_U32(REG_DATA_TL0,NF_DATA_TL0_length0(0x00));
	REG_WRITE_U32(REG_DATA_TL1,NF_DATA_TL1_access_mode(0x01)|NF_DATA_TL1_length1(0x2));

	//Set page length at auto mode 
	REG_WRITE_U32(REG_PAGE_LEN, 0x1);
	REG_WRITE_U32(REG_READ_BY_PP,NF_READ_BY_PP_read_by_pp(0x1));

	REG_WRITE_U32(REG_PP_CTL0,NF_PP_CTL0_pp_enable(0x1));
	REG_WRITE_U32(REG_PP_CTL1,NF_PP_CTL1_pp_start_addr(0));

	//Set command
	REG_WRITE_U32(REG_ND_CMD,NF_ND_CMD_cmd(CMD_READ_ID));

	//Set address
	REG_WRITE_U32(REG_ND_PA0, 0);
	REG_WRITE_U32(REG_ND_PA1, 0);
	REG_WRITE_U32(REG_ND_PA2,NF_ND_PA2_addr_mode(0x07));
	
	//Set ECC	
	REG_WRITE_U32(REG_MULTI_CHNL_MODE, NF_MULTI_CHNL_MODE_ecc_no_check(1) | NF_MULTI_CHNL_MODE_ecc_pass(1));	//Set HW no check ECC	
	REG_WRITE_U32(REG_ECC_STOP, NF_ECC_STOP_ecc_n_stop(0x1));	//Set ECC no stop
	//Set DMA	
	REG_WRITE_U32(REG_DMA_CTL1, VIR_TO_PHY((uint32_t)id)>>3);	//Set DRAM start address
	REG_WRITE_U32(REG_DMA_CTL2, 0x1);	//Transfer length (Unit = 512B)	
	REG_WRITE_U32(REG_DMA_CTL3, NF_DMA_CTL3_ddr_wr(1) | NF_DMA_CTL3_dma_xfer(1));	//Set DMA direction and enable DMA transfer 
	//Enable Auto mode
	REG_WRITE_U32(REG_AUTO_TRIG,NF_AUTO_TRIG_auto_trig(1)|NF_AUTO_TRIG_spec_auto_case(1)| NF_AUTO_TRIG_auto_case(0));
	WAIT_DONE(REG_AUTO_TRIG,0x80,0);
	//Enable Auto mode
	REG_WRITE_U32(REG_AUTO_TRIG,NF_AUTO_TRIG_auto_trig(1)|NF_AUTO_TRIG_spec_auto_case(0)| NF_AUTO_TRIG_auto_case(4));
	WAIT_DONE(REG_AUTO_TRIG,0x80,0);
	//Wait DMA done	
	WAIT_DONE(REG_DMA_CTL3, 0x01, 0x00);	

}



//----------------------------------------------------------------------------
static void rtk_nand_read_id(struct mtd_info *mtd, u_char id[6])
{
#define XFER_MODE
	int id_chain;
	printk("%s \n", __FUNCTION__);

	//printk("rtk_nand_read_id \n");
	//Set SRAM path and access mode
	//REG_WRITE_U32( REG_TIME_PARA1,0x1);
	//REG_WRITE_U32( REG_TIME_PARA2,0x1);
	//REG_WRITE_U32( REG_TIME_PARA3,0x1);

#ifdef XFER_MODE
	REG_WRITE_U32(REG_DATA_TL0,NF_DATA_TL0_length0(0x06));
	REG_WRITE_U32(REG_DATA_TL1,NF_DATA_TL1_access_mode(0x01));

	//Set PP
	REG_WRITE_U32(REG_READ_BY_PP,NF_READ_BY_PP_read_by_pp(0x0));

	REG_WRITE_U32(REG_PP_CTL0,NF_PP_CTL0_pp_enable(0x01));
	REG_WRITE_U32(REG_PP_CTL1,NF_PP_CTL1_pp_start_addr(0));

	//Set command
	REG_WRITE_U32(REG_ND_CMD,NF_ND_CMD_cmd(CMD_READ_ID));
	REG_WRITE_U32(REG_ND_CTL,NF_ND_CTL_xfer(0x01));
	WAIT_DONE(REG_ND_CTL,0x80,0);

	//Set address
	REG_WRITE_U32(REG_ND_PA0, 0);
	REG_WRITE_U32(REG_ND_PA1, 0);
	REG_WRITE_U32(REG_ND_PA2,NF_ND_PA2_addr_mode(0x07));

	REG_WRITE_U32(REG_ND_CTL,NF_ND_CTL_xfer(1)|NF_ND_CTL_tran_mode(1));
	WAIT_DONE(REG_ND_CTL,0x80,0);
	//Enable XFER mode
	#if 1
	REG_WRITE_U32(REG_ND_CTL,NF_ND_CTL_xfer(1)|NF_ND_CTL_tran_mode(4));
	WAIT_DONE(REG_ND_CTL,0x80,0);

	//Reset PP
	REG_WRITE_U32(REG_PP_CTL0,NF_PP_CTL0_pp_reset(1));

	//Move data to DRAM from SRAM
	REG_WRITE_U32(REG_SRAM_CTL,NF_SRAM_CTL_map_sel(1)|NF_SRAM_CTL_access_en(1)|NF_SRAM_CTL_mem_region(0));

	id_chain = REG_READ_U32(REG_ND_PA0);
	//printk("id_chain 1 = 0x%x \n", id_chain);
	id[0] = id_chain & 0xff;
	id[1] = (id_chain >> 8) & 0xff;
	id[2] = (id_chain >> 16) & 0xff;
	id[3] = (id_chain >> 24) & 0xff;

	g_id_chain = id_chain;
	//if(id_chain==0xDA94D7AD)
	//{
		//printk("Detect H27UBG8T2B and enable read-retrial mode...\n");
		//g_enReadRetrial = 1;
		//g_isRandomize = 1;
		//printk("Enable Read-retrial mode...\n");
		//printk("Enable Randomized mechanism\n");
	//}
	if(id_chain == 0x4b44642c)
	{
		printk("Detect MT29F64G08CBABA and enable read-retrial mode...\n");
		g_enReadRetrial = 1;
	}else if(id_chain == 0x4b44442c)
	{
		printk("Detect MT29F32G08CBADA and enable read-retrial mode\n");
		g_enReadRetrial = 1;
	}else if(id_chain == 0x9384d798){
		printk("Detect TC58TEG5DCJT and enable read-retrial mode...\n");
		g_enReadRetrial = 1;
        }else if(id_chain == 0x9384de98){
		printk("Detect TC58TEG6DCJT and enable read-retrial mode...\n");
		g_enReadRetrial = 1;
        }else if(id_chain == 0x9394de98){
		printk("Detect TC58TEG6DDK and enable read-retrial mode...\n");
		g_enReadRetrial = 1;
        }


	id_chain = REG_READ_U32(REG_ND_PA1);
	//printk("id_chain 2 = 0x%x \n", id_chain);
	id[4] = id_chain & 0xff;
	id[5] = (id_chain >> 8) & 0xff;

	REG_WRITE_U32(REG_SRAM_CTL,0x0);	//# no omitted
	#else
	u32 i;
	for(i = 0; i < 6; i ++)
	{
		REG_WRITE_U32(REG_ND_CTL,NF_ND_CTL_xfer(1)|NF_ND_CTL_tran_mode(2));
		WAIT_DONE(REG_ND_CTL,0x80,0);

		id_chain = REG_READ_U32(REG_ND_DAT);
		id[i] = (id_chain) & 0xff;
	}
	//Reset PP
	REG_WRITE_U32(REG_PP_CTL0,NF_PP_CTL0_pp_reset(1));


	#endif
#else
	REG_WRITE_U32(REG_PP_CTL0,NF_PP_CTL0_pp_reset(0x01));
	REG_WRITE_U32(REG_DATA_TL0,NF_DATA_TL0_length0(0x00));
	REG_WRITE_U32(REG_DATA_TL1,NF_DATA_TL1_access_mode(0x01)|NF_DATA_TL1_length1(0x2));

	//Set page length at auto mode 
	REG_WRITE_U32(REG_PAGE_LEN, 0x1);
	REG_WRITE_U32(REG_READ_BY_PP,NF_READ_BY_PP_read_by_pp(0x1));

	REG_WRITE_U32(REG_PP_CTL0,NF_PP_CTL0_pp_enable(0x01));
	REG_WRITE_U32(REG_PP_CTL1,NF_PP_CTL1_pp_start_addr(0));

	//Set command
	REG_WRITE_U32(REG_ND_CMD,NF_ND_CMD_cmd(CMD_READ_ID));

	//Set address
	REG_WRITE_U32(REG_ND_PA0, 0);
	REG_WRITE_U32(REG_ND_PA1, 0);
	REG_WRITE_U32(REG_ND_PA2,NF_ND_PA2_addr_mode(0x07));
	
	//Set ECC	
	REG_WRITE_U32(REG_MULTI_CHNL_MODE, NF_MULTI_CHNL_MODE_ecc_no_check(1) | NF_MULTI_CHNL_MODE_ecc_pass(1));	//Set HW no check ECC	
	REG_WRITE_U32(REG_ECC_STOP, NF_ECC_STOP_ecc_n_stop(0x1));	//Set ECC no stop
	//Set DMA	
	REG_WRITE_U32(REG_DMA_CTL1, id);	//Set DRAM start address
	REG_WRITE_U32(REG_DMA_CTL2, 0x1);	//Transfer length (Unit = 512B)	
	REG_WRITE_U32(REG_DMA_CTL3, NF_DMA_CTL3_ddr_wr(1) | NF_DMA_CTL3_dma_xfer(1));	//Set DMA direction and enable DMA transfer 
	//Enable Auto mode
	REG_WRITE_U32(REG_AUTO_TRIG,NF_AUTO_TRIG_auto_trig(1)|NF_AUTO_TRIG_spec_auto_case(1)| NF_AUTO_TRIG_auto_case(0));
	WAIT_DONE(REG_AUTO_TRIG,0x80,0);
	//Enable Auto mode
	REG_WRITE_U32(REG_AUTO_TRIG,NF_AUTO_TRIG_auto_trig(1)|NF_AUTO_TRIG_spec_auto_case(0)| NF_AUTO_TRIG_auto_case(4));
	WAIT_DONE(REG_AUTO_TRIG,0x80,0);
	//Wait DMA done	
	WAIT_DONE(REG_DMA_CTL3, 0x01, 0x00);	
#endif

}
//
static int rtk_write_ecc_512 (struct mtd_info *mtd, u16 chipnr, unsigned int page,
			const u_char *data_buf, const  u_char *oob_buf)
{
	unsigned int ppb = mtd->erasesize/mtd->oobblock;
	struct nand_chip *this = (struct nand_chip *) mtd->priv;
	int rc = 0, i;
	uint8_t auto_trigger_mode = 0;
	uint8_t addr_mode = 1;

	unsigned int page_len, dram_sa, dma_len, spare_dram_sa;
	unsigned char oob_1stB;

	unsigned char nf_oob_buf[oob_size];
	unsigned int chip_section = 0;
	unsigned int section = 0;
	unsigned int index = 0;
	unsigned int retryCnt = RETRY_COUNT;
#ifdef RTK_ROM_CODE
	unsigned char temp_oob_buf[oob_size];
	this->g_oobbuf = oob_buf = temp_oob_buf;
#endif

	printk("[%s] entry... \n", __FUNCTION__);
	this->select_chip(mtd, chipnr);

	if(((uint32_t)data_buf&0x7)!=0) {
			printk("[%s]data_buf must 8 byte alignmemt!!\n",__FUNCTION__);
			BUG();
	}
	if(((uint32_t)oob_buf&0x7)!=0) {
			printk("[%s]oob_buf must 8 byte alignmemt!!\n",__FUNCTION__);
			BUG();
	}
#if 0       //stevie
	if ( chipnr == 0 && page < g_BootcodeSize/page_size ){
		if ( isBBT && page<3*ppb ){
			printk("[%s] Updating BBT %d page=%d\n", __FUNCTION__, page/ppb, page%ppb);
		}
		else {
			printk("[%s] You have no permission to write page %d\n", __FUNCTION__, page);
			//up (&sem_NF_CARDREADER);
			return -2;
		}
		mtd->isCPdisable_W = 1;
	}
	if ( chipnr == 0 && page >= g_BootcodeSize/page_size){
		if(page<(g_BootcodeSize+g_Factory_param_size)/page_size+ppb)
		{
			  mtd->isCPdisable_W = 1;
		}
	}
#endif
	//if(is_darwin_cpu()||is_macarthur_cpu()||is_nike_cpu())//Enable NF_WP pin (Write Protect Pin)
	//{
	//	if(g_WP_en)
	//	{
	//		setGPIOBit(g_regGPIODirBase,g_NFWP_no,NF_GPIO_OUT);//Set Driection
	//		setGPIOBit(g_regGPIOOutBase,g_NFWP_no,g_NFWP_value_dis);//Set Value
	//	}
	//}
#if 0       //stevie
	if ( page == ppb || page == ppb+1|| page == ppb*2 || page == ppb*2+1 )
		oob_1stB = BBT_TAG;
	else if(( page>=g_BootcodeSize/page_size )&&(page<(g_BootcodeSize+g_Factory_param_size)/page_size))
		oob_1stB = TAG_FACTORY_PARAM;
	else
		oob_1stB = 0xFF;
#endif
	oob_1stB = 0xFF;
	/*
	printk("[nand DBG]data_buf :  0x%x, g_oobbuf : 0x%x, g_databuf : 0x%x", (u32)data_buf, (u32)this->g_oobbuf, (u32)this->g_databuf);
		for(i=0; i < 512; i ++)
		{
			if(!(i %16))
				printf("\n");
			printf(" %02x",  data_buf[i]);
		}
	*/
	this->g_oobbuf[0x0] = oob_1stB;
	this->g_oobbuf[0x10] = oob_1stB;
	this->g_oobbuf[0x20] = oob_1stB;
	this->g_oobbuf[0x30] = oob_1stB;
	
	this->g_oobbuf[0x8] = oob_1stB;
	this->g_oobbuf[0x18] = oob_1stB;
	this->g_oobbuf[0x28] = oob_1stB;
	this->g_oobbuf[0x38] = oob_1stB;
	if(oob_size >= 0x78)
	{
		this->g_oobbuf[0x40] = oob_1stB;
		this->g_oobbuf[0x50] = oob_1stB;
		this->g_oobbuf[0x60] = oob_1stB;
		this->g_oobbuf[0x70] = oob_1stB;

		this->g_oobbuf[0x48] = oob_1stB;
		this->g_oobbuf[0x58] = oob_1stB;
		this->g_oobbuf[0x68] = oob_1stB;
		this->g_oobbuf[0x78] = oob_1stB;
	}

#if 0
			int i,id_chain;
			REG_WRITE_U32(REG_DATA_TL0,NF_DATA_TL0_length0(0x10));
			REG_WRITE_U32(REG_DATA_TL1,NF_DATA_TL1_access_mode(0x01) |NF_DATA_TL1_length1(0));
		
			//Set PP
			REG_WRITE_U32(REG_READ_BY_PP,NF_READ_BY_PP_read_by_pp(0x0));
		
			REG_WRITE_U32(REG_PP_CTL0,NF_PP_CTL0_pp_enable(0x01));
			REG_WRITE_U32(REG_PP_CTL1,NF_PP_CTL1_pp_start_addr(0));
		
			//Set command
			REG_WRITE_U32(REG_ND_CMD,NF_ND_CMD_cmd(CMD_PG_WRITE_C1));
			REG_WRITE_U32(REG_ND_CTL,NF_ND_CTL_xfer(0x01));
			WAIT_DONE(REG_ND_CTL,0x80,0);
		
			//Set address
			REG_WRITE_U32(REG_ND_PA0, page&0xff);
			REG_WRITE_U32(REG_ND_PA1, (page>>8)&0xff);
			REG_WRITE_U32(REG_ND_PA2,NF_ND_PA2_addr_mode(0x01) | (page>>16)&0x1f);
		
			REG_WRITE_U32(REG_ND_CTL,NF_ND_CTL_xfer(1)|NF_ND_CTL_tran_mode(1));
			WAIT_DONE(REG_ND_CTL,0x80,0);

			//Reset PP
			REG_WRITE_U32(REG_PP_CTL0,NF_PP_CTL0_pp_reset(1));
			//Move data to SRAM  from  DRAM

			// flush cache.
			RTK_FLUSH_CACHE(data_buf, page_size);
			RTK_FLUSH_CACHE(this->g_oobbuf, oob_size);
			/*
			dram_sa = ( (uint32_t)data_buf >> 3);
			REG_WRITE_U32(REG_DMA_CTL1,NF_DMA_CTL1_dram_sa(dram_sa));
			dma_len = page_size >> 9;
			REG_WRITE_U32(REG_DMA_CTL2,NF_DMA_CTL2_dma_len(dma_len));	
			
			//if (oob_buf)
			//	spare_dram_sa = ( (uint32_t)nf_oob_buf >> 3);
			//else
				spare_dram_sa = ( (uint32_t)this->g_oobbuf >> 3);
			REG_WRITE_U32(REG_SPR_DDR_CTL,NF_SPR_DDR_CTL_spare_ddr_ena(1)|NF_SPR_DDR_CTL_per_2k_spr_ena(1)|NF_SPR_DDR_CTL_spare_dram_sa(spare_dram_sa));
			
			REG_WRITE_U32(REG_DMA_CTL3,NF_DMA_CTL3_ddr_wr(0)|NF_DMA_CTL3_dma_xfer(1));
			//REG_WRITE_U32( REG_AUTO_TRIG,NF_AUTO_TRIG_auto_trig(1)|NF_AUTO_TRIG_spec_auto_case(0)| NF_AUTO_TRIG_auto_case(auto_trigger_mode));
*/
			
			REG_WRITE_U32(REG_SRAM_CTL,NF_SRAM_CTL_map_sel(1)|NF_SRAM_CTL_access_en(1)|NF_SRAM_CTL_mem_region(0));
			
			for(i = 0; i < 512; i +=4)
			{
				id_chain = data_buf[i+0]  & 0xff;
				id_chain |= (data_buf[i+1] << 8) & 0xff00;
				id_chain |= (data_buf[i+2] << 16) & 0xff0000;
				id_chain |= (data_buf[i+3] << 24) & 0xff000000;
				REG_WRITE_U32(REG_ND_PA0+i, id_chain);
			}
			REG_WRITE_U32(REG_SRAM_CTL,0x0);	//# no omitted


			//Enable XFER mode
			REG_WRITE_U32(REG_ND_CTL,NF_ND_CTL_xfer(1)| NF_ND_CTL_ecc_enable(0) |NF_ND_CTL_tran_mode(3));
			WAIT_DONE(REG_ND_CTL,0x80,0);
	
			//while ( (REG_READ_U32(REG_ND_CTL) & 0x80) != 0 )
			//{
			//	asm("nop");
			//}
			//Set write command 2
			REG_WRITE_U32(REG_ND_CMD,NF_ND_CMD_cmd(CMD_PG_WRITE_C2));
			REG_WRITE_U32(REG_ND_CTL,NF_ND_CTL_xfer(0x01));
			WAIT_DONE(REG_ND_CTL,0x80,0);	
			//REG_WRITE_U32(REG_POLL_FSTS,NF_POLL_FSTS_bit_sel(6)|NF_POLL_FSTS_trig_poll(1) );
			//WAIT_DONE(REG_POLL_FSTS,0x01,0x0);

			WAIT_DONE(REG_ND_CTL,0x40,0x40);

			return 0;
#else
	//REG_WRITE_U32(REG_SRAM_CTL,0x00);//add by alexchang 0208-2010
	REG_WRITE_U32(REG_DATA_TL0,NF_DATA_TL0_length0(0));
	if(this->ecc_select >= 0x18)
	{
		// enable randomizer
		REG_WRITE_U32(REG_RMZ_CTRL, 1);
		
		page_len = page_size >> 10;
		REG_WRITE_U32(REG_DATA_TL1,NF_DATA_TL1_length1(4));
	}
	else
	{
		// set random write
		REG_WRITE_U32(REG_RND_EN, 1);
		REG_WRITE_U32(REG_RND_CMD1, 0x85);
		REG_WRITE_U32(REG_RND_DATA_STR_COL_H, 0);	// data start address MSB (always 0)
		REG_WRITE_U32(REG_RND_SPR_STR_COL_H, page_size >> 8);	// spare start address MSB
		REG_WRITE_U32(REG_RND_SPR_STR_COL_L, page_size & 0xff); // spare start address LSB
		
		page_len = page_size >> 9;
		REG_WRITE_U32(REG_DATA_TL1,NF_DATA_TL1_length1(2));
	}

	REG_WRITE_U32( REG_PAGE_LEN,NF_PAGE_LEN_page_len(page_len));

	//Set PP
	REG_WRITE_U32(REG_READ_BY_PP,NF_READ_BY_PP_read_by_pp(0));//add by alexchang 0208-2010
	REG_WRITE_U32(REG_PP_CTL1,NF_PP_CTL1_pp_start_addr(0));//add by alexchang 0208-2010
	REG_WRITE_U32(REG_PP_CTL0,0);//add by alexchang 0208-2010

	//Set command
	REG_WRITE_U32(REG_ND_CMD,NF_ND_CMD_cmd(CMD_PG_WRITE_C1));
	REG_WRITE_U32(REG_CMD2,NF_CMD2_cmd2(CMD_PG_WRITE_C2));
	REG_WRITE_U32(REG_CMD3,NF_CMD3_cmd3(CMD_PG_WRITE_C3));

	//Set address
	REG_WRITE_U32( REG_ND_PA0,NF_ND_PA0_page_addr0(page));
	REG_WRITE_U32( REG_ND_PA1,NF_ND_PA1_page_addr1(page>>8));
	REG_WRITE_U32( REG_ND_PA2,NF_ND_PA2_addr_mode(addr_mode)|NF_ND_PA2_page_addr2(page>>16));
	REG_WRITE_U32( REG_ND_PA3,NF_ND_PA3_page_addr3((page>>21)&0x7));
	REG_WRITE_U32(REG_ND_CA0, 0);
	REG_WRITE_U32(REG_ND_CA1, 0);

	//Set ECC
	REG_WRITE_U32(REG_MULTI_CHNL_MODE,NF_MULTI_CHNL_MODE_edo(1));//add by alexchang0205-2010

	// no ECC NF_MULTI_CHNL_MODE_ecc_pass(1)
	if(monflg == 5566)
	{
		printk("Set write ECC PASS \n");
		REG_WRITE_U32(REG_MULTI_CHNL_MODE,NF_MULTI_CHNL_MODE_edo(1)|NF_MULTI_CHNL_MODE_ecc_pass(1));
	}
	REG_WRITE_U32( REG_ECC_STOP,NF_ECC_STOP_ecc_n_stop(0x01));

	switch(this->ecc_select) {
		case 6:
			REG_WRITE_U32(REG_ECC_SEL, 0x0);
			break;
		case 12:
			REG_WRITE_U32(REG_ECC_SEL, 0x1);
			break;
		case 16:
			REG_WRITE_U32(REG_ECC_SEL, 0xe);
			break;
		case 24:
			REG_WRITE_U32(REG_ECC_SEL, 0xa);
			break;
		case 40:
			REG_WRITE_U32(REG_ECC_SEL, 0x2);
			break;
		case 43:
			REG_WRITE_U32(REG_ECC_SEL, 0x4);
			break;
		case 65:
			REG_WRITE_U32(REG_ECC_SEL, 0x6);
			break;
		case 72:
			REG_WRITE_U32(REG_ECC_SEL, 0x8);
			break;
		default:
			REG_WRITE_U32(REG_ECC_SEL, 0x0);
			break;
	}

	// flush cache.
	RTK_FLUSH_CACHE(data_buf, page_size);
	RTK_FLUSH_CACHE(this->g_oobbuf, oob_size);
	
	dram_sa = ( (uint32_t)data_buf >> 3);
	REG_WRITE_U32(REG_DMA_CTL1,NF_DMA_CTL1_dram_sa(dram_sa));
	dma_len = page_size >> 9;
	REG_WRITE_U32(REG_DMA_CTL2,NF_DMA_CTL2_dma_len(dma_len));	
	
	//if (oob_buf)
	//	spare_dram_sa = ( (uint32_t)nf_oob_buf >> 3);
	//else
		spare_dram_sa = ( (uint32_t)this->g_oobbuf >> 3);
	REG_WRITE_U32(REG_SPR_DDR_CTL,NF_SPR_DDR_CTL_spare_ddr_ena(1)|NF_SPR_DDR_CTL_per_2k_spr_ena(1)|NF_SPR_DDR_CTL_spare_dram_sa(spare_dram_sa));

	REG_WRITE_U32(REG_DMA_CTL3,NF_DMA_CTL3_ddr_wr(0)|NF_DMA_CTL3_dma_xfer(1));
	REG_WRITE_U32( REG_AUTO_TRIG,NF_AUTO_TRIG_auto_trig(1)|NF_AUTO_TRIG_spec_auto_case(0)| NF_AUTO_TRIG_auto_case(auto_trigger_mode));


//#if RTK_NAND_INTERRUPT
//	RTK_WAIT_EVENT;
//	NF_RESET_IRQ;
//	NF_ENABLE_IRQ(NF_POLL_IRQ_MASK);
//	g_nfFlag = NF_WRITE_TRIGGER;
//	REG_WRITE_U32(REG_POLL_FSTS,NF_POLL_FSTS_bit_sel(6)|NF_POLL_FSTS_trig_poll(1) );
//	RTK_WAIT_EVENT;
//#else
	WAIT_DONE(REG_AUTO_TRIG,0x80,0);
	WAIT_DONE(REG_DMA_CTL3,0x01,0);
	REG_WRITE_U32(REG_POLL_FSTS,NF_POLL_FSTS_bit_sel(6)|NF_POLL_FSTS_trig_poll(1) );
	WAIT_DONE(REG_POLL_FSTS,0x01,0x0);
	WAIT_DONE(REG_ND_CTL,0x40,0x40);//add by alexchang 0416-2010
//#endif
	if ( REG_READ_U32(REG_ND_DAT) & 0x01 ){
		//up (&sem_NF_CARDREADER);
		//up_write(&rw_sem);
		printk("[%s] write is not completed at page %d\n", __FUNCTION__, page);
		//padunlock(PAD_NAND);	 // unlock nand・s pad
		return -1;
	}

	//while (1);
	return 0;
#endif
}
static int rtk_write_diff_ecc (struct mtd_info *mtd, u16 chipnr, unsigned int page, unsigned int column,
			const u_char *data_buf, const  u_char *oob_buf, u32 ecc_select)
{
	unsigned int ppb = mtd->erasesize/mtd->oobblock;
	struct nand_chip *this = (struct nand_chip *) mtd->priv;
	int rc = 0, i;
	uint8_t auto_trigger_mode = 1;
	uint8_t addr_mode = 1;

	unsigned int page_len, dram_sa, dma_len, spare_dram_sa;
	unsigned char oob_1stB;

	unsigned char nf_oob_buf[oob_size];
	unsigned int chip_section = 0;
	unsigned int section = 0;
	unsigned int index = 0;
	unsigned int retryCnt = RETRY_COUNT;
#ifdef RTK_ROM_CODE
	unsigned char temp_oob_buf[oob_size];
	this->g_oobbuf = oob_buf = temp_oob_buf;
#endif

	dbg_parallel_nand("[%s] entry... page : %d\n", __FUNCTION__, page);
	this->select_chip(mtd, chipnr);
	//printk("00[%s]Ran 0x%x, scr 0x%x, flag 0x%x\n",__FUNCTION__,g_isRandomize,mtd->isScramble,mtd->isCPdisable_W);
	//while (down_interruptible (&sem_NF_CARDREADER)) {
	//	printk("%s : Retry [Address: 0x%x]\n",__FUNCTION__,page);
	//	if(retryCnt<=0) {
	//		printk("%s : Retry fail [ADDR 0x%x]\n",__FUNCTION__,page);
	//		return -ERESTARTSYS;
	//	}
	//	retryCnt--;
	//}
	if(((uint32_t)data_buf&0x7)!=0) {
			printk("[%s]data_buf must 8 byte alignmemt!!\n",__FUNCTION__);
			BUG();
	}
	if(((uint32_t)this->g_oobbuf&0x7)!=0) {
			printk("[%s]oob_buf must 8 byte alignmemt!!\n",__FUNCTION__);
			BUG();
	}
#if 0       //stevie
	if ( chipnr == 0 && page < g_BootcodeSize/page_size ){
		if ( isBBT && page<3*ppb ){
			printk("[%s] Updating BBT %d page=%d\n", __FUNCTION__, page/ppb, page%ppb);
		}
		else {
			printk("[%s] You have no permission to write page %d\n", __FUNCTION__, page);
			//up (&sem_NF_CARDREADER);
			return -2;
		}
		mtd->isCPdisable_W = 1;
	}
	if ( chipnr == 0 && page >= g_BootcodeSize/page_size){
		if(page<(g_BootcodeSize+g_Factory_param_size)/page_size+ppb)
		{
			  mtd->isCPdisable_W = 1;
		}
	}
#endif
	//if(is_darwin_cpu()||is_macarthur_cpu()||is_nike_cpu())//Enable NF_WP pin (Write Protect Pin)
	//{
	//	if(g_WP_en)
	//	{
	//		setGPIOBit(g_regGPIODirBase,g_NFWP_no,NF_GPIO_OUT);//Set Driection
	//		setGPIOBit(g_regGPIOOutBase,g_NFWP_no,g_NFWP_value_dis);//Set Value
	//	}
	//}
#if 0       //stevie
	if ( page == ppb || page == ppb+1|| page == ppb*2 || page == ppb*2+1 )
		oob_1stB = BBT_TAG;
	else if(( page>=g_BootcodeSize/page_size )&&(page<(g_BootcodeSize+g_Factory_param_size)/page_size))
		oob_1stB = TAG_FACTORY_PARAM;
	else
		oob_1stB = 0xFF;
#endif


	//REG_WRITE_U32(REG_SRAM_CTL,0x00);//add by alexchang 0208-2010
	REG_WRITE_U32(REG_DATA_TL0,NF_DATA_TL0_length0(0));
	if(ecc_select >= 16)
	{
		// enable randomizer
		//REG_WRITE_U32(REG_RMZ_CTRL, 1);
		
		page_len = 1;//page_size >> 10;
		REG_WRITE_U32(REG_DATA_TL1,NF_DATA_TL1_length1(4));
	}
	else
	{
		// set random write
		REG_WRITE_U32(REG_RND_EN, 1);
		REG_WRITE_U32(REG_RND_CMD1, 0x5);
		REG_WRITE_U32(REG_RND_CMD2, 0xe0);
		REG_WRITE_U32(REG_RND_DATA_STR_COL_H, 0);	// data start address MSB (always 0)
		REG_WRITE_U32(REG_RND_SPR_STR_COL_H, page_size >> 8);	// spare start address MSB
		REG_WRITE_U32(REG_RND_SPR_STR_COL_L, page_size & 0xff); // spare start address LSB
		REG_WRITE_U32(REG_RMZ_CTRL, 0);

		page_len = 1;//page_size >> 9;
		REG_WRITE_U32(REG_DATA_TL1,NF_DATA_TL1_length1(2));
	}
	REG_WRITE_U32(REG_RND_EN, 0);
	#if 1//RANDOMIZER_ECC_CHECK
	//REG_WRITE_U32(REG_RND_EN, 1);
	if(monflg == 6789)
	{
		REG_WRITE_U32(REG_RMZ_CTRL, 1);
		REG_WRITE_U32(REG_RMZ_SEED_L, 0xff);
		REG_WRITE_U32(REG_RMZ_SEED_H,0x7f);
		REG_WRITE_U32(REG_RMZ_SEED_CTRL,0x20);
	}
	else
	{
		REG_WRITE_U32(REG_RMZ_CTRL, 0);
		REG_WRITE_U32(REG_RMZ_SEED_CTRL, 0);
	}
	#endif


	REG_WRITE_U32( REG_PAGE_LEN,NF_PAGE_LEN_page_len(page_len));

	//Set PP
	REG_WRITE_U32(REG_READ_BY_PP,NF_READ_BY_PP_read_by_pp(0));//add by alexchang 0208-2010
	REG_WRITE_U32(REG_PP_CTL1,NF_PP_CTL1_pp_start_addr(0));//add by alexchang 0208-2010
	REG_WRITE_U32(REG_PP_CTL0,0);//add by alexchang 0208-2010

	//Set command
	REG_WRITE_U32(REG_ND_CMD,NF_ND_CMD_cmd(CMD_PG_WRITE_C1));
	REG_WRITE_U32(REG_CMD2,NF_CMD2_cmd2(CMD_PG_WRITE_C2));
	REG_WRITE_U32(REG_CMD3,NF_CMD3_cmd3(CMD_PG_WRITE_C3));

	//Set address
	REG_WRITE_U32( REG_ND_PA0,NF_ND_PA0_page_addr0(page));
	REG_WRITE_U32( REG_ND_PA1,NF_ND_PA1_page_addr1(page>>8));
	REG_WRITE_U32( REG_ND_PA2,NF_ND_PA2_addr_mode(addr_mode)|NF_ND_PA2_page_addr2(page>>16));
	REG_WRITE_U32( REG_ND_PA3,NF_ND_PA3_page_addr3((page>>21)&0x7));
	REG_WRITE_U32(REG_ND_CA0, NF_ND_CA0_col_addr0(column));
	REG_WRITE_U32(REG_ND_CA1, NF_ND_CA1_col_addr1(column>>8));

	//Set ECC

	// no ECC NF_MULTI_CHNL_MODE_ecc_pass(1)
	if(monflg == 5566)
	{
		printk("Set write ECC PASS \n");
		REG_WRITE_U32(REG_MULTI_CHNL_MODE,NF_MULTI_CHNL_MODE_edo(1)|NF_MULTI_CHNL_MODE_ecc_pass(1) | NF_MULTI_CHNL_MODE_ecc_no_check(1));
		//REG_WRITE_U32(REG_RMZ_CTRL, 1);
	}
	else
		REG_WRITE_U32(REG_MULTI_CHNL_MODE,NF_MULTI_CHNL_MODE_edo(1));//add by alexchang0205-2010
	REG_WRITE_U32( REG_ECC_STOP,NF_ECC_STOP_ecc_n_stop(0x01));

	switch(ecc_select) {
		case 6:
			REG_WRITE_U32(REG_ECC_SEL, 0x0);
			dma_len = 1;
			break;
		case 12:
			REG_WRITE_U32(REG_ECC_SEL, 0x1);
			dma_len = 1;
			break;
		case 16:
			REG_WRITE_U32(REG_ECC_SEL, 0xe);
			dma_len = 2;
			break;
		case 24:
			REG_WRITE_U32(REG_ECC_SEL, 0xa);
			dma_len = 2;
			break;
		case 40:
			REG_WRITE_U32(REG_ECC_SEL, 0x2);
			dma_len = 2;
			break;
		case 43:
			REG_WRITE_U32(REG_ECC_SEL, 0x4);
			dma_len = 2;
			break;
		case 65:
			REG_WRITE_U32(REG_ECC_SEL, 0x6);
			dma_len = 2;
			break;
		case 72:
			REG_WRITE_U32(REG_ECC_SEL, 0x8);
			dma_len = 2;
			break;
		default:
			REG_WRITE_U32(REG_ECC_SEL, 0x0);
			dma_len = 1;
			break;
	}

	// flush cache.
	RTK_FLUSH_CACHE(data_buf, page_size);
	RTK_FLUSH_CACHE(this->g_oobbuf, oob_size);
	
	dram_sa = ( VIR_TO_PHY((uint32_t)data_buf) >> 3);
	REG_WRITE_U32(REG_DMA_CTL1,NF_DMA_CTL1_dram_sa(dram_sa));
	//dma_len = 1;//page_size >> 9;
	REG_WRITE_U32(REG_DMA_CTL2,NF_DMA_CTL2_dma_len(dma_len));	
	
	//if (oob_buf)
	//	spare_dram_sa = ( (uint32_t)nf_oob_buf >> 3);
	//else
		spare_dram_sa = ( VIR_TO_PHY((uint32_t)this->g_oobbuf) >> 3);
	if(monflg == 5566)
		REG_WRITE_U32(REG_SPR_DDR_CTL,NF_SPR_DDR_CTL_spare_ddr_ena(0)|NF_SPR_DDR_CTL_per_2k_spr_ena(0)|NF_SPR_DDR_CTL_spare_dram_sa(spare_dram_sa));
	else		
		REG_WRITE_U32(REG_SPR_DDR_CTL,NF_SPR_DDR_CTL_spare_ddr_ena(1)|NF_SPR_DDR_CTL_per_2k_spr_ena(0)|NF_SPR_DDR_CTL_spare_dram_sa(spare_dram_sa));

	REG_WRITE_U32(REG_DMA_CTL3,NF_DMA_CTL3_ddr_wr(0)|NF_DMA_CTL3_dma_xfer(1));
	REG_WRITE_U32( REG_AUTO_TRIG,NF_AUTO_TRIG_auto_trig(1)|NF_AUTO_TRIG_spec_auto_case(0)| NF_AUTO_TRIG_auto_case(auto_trigger_mode));


//#if RTK_NAND_INTERRUPT
//	RTK_WAIT_EVENT;
//	NF_RESET_IRQ;
//	NF_ENABLE_IRQ(NF_POLL_IRQ_MASK);
//	g_nfFlag = NF_WRITE_TRIGGER;
//	REG_WRITE_U32(REG_POLL_FSTS,NF_POLL_FSTS_bit_sel(6)|NF_POLL_FSTS_trig_poll(1) );
//	RTK_WAIT_EVENT;
//#else
	WAIT_DONE(REG_AUTO_TRIG,0x80,0);
	WAIT_DONE(REG_DMA_CTL3,0x01,0);
	REG_WRITE_U32(REG_POLL_FSTS,NF_POLL_FSTS_bit_sel(6)|NF_POLL_FSTS_trig_poll(1) );
	WAIT_DONE(REG_POLL_FSTS,0x01,0x0);
	WAIT_DONE(REG_ND_CTL,0x40,0x40);//add by alexchang 0416-2010
//#endif
	if ( REG_READ_U32(REG_ND_DAT) & 0x01 ){
		//up (&sem_NF_CARDREADER);
		//up_write(&rw_sem);
		printk("[%s] write is not completed at page %d\n", __FUNCTION__, page);
		//padunlock(PAD_NAND);	 // unlock nand・s pad
		return -1;
	}

	//while (1);
	return 0;
}


//----------------------------------------------------------------------------
//char bbbbb[4096];
static int rtk_write_ecc_page (struct mtd_info *mtd, u16 chipnr, unsigned int page,
			const u_char *data_buf, const  u_char *oob_buf)
{
	unsigned int ppb = mtd->erasesize/mtd->oobblock;
	struct nand_chip *this = (struct nand_chip *) mtd->priv;
	int rc = 0, i;
	uint8_t auto_trigger_mode = 1;
	uint8_t addr_mode = 1;

	unsigned int page_len, dram_sa, dma_len, spare_dram_sa;
	unsigned char oob_1stB;

	//unsigned char nf_oob_buf[oob_size];
	unsigned int chip_section = 0;
	unsigned int section = 0;
	unsigned int index = 0;
	unsigned int retryCnt = RETRY_COUNT;
#ifdef RTK_ROM_CODE
	unsigned char temp_oob_buf[oob_size];
	this->g_oobbuf = oob_buf = temp_oob_buf;
#endif
	unsigned int is_sync_mode;
	printk("[%s] entry... page : %d\n", __FUNCTION__, page);
	//this->select_chip(mtd, chipnr);
	//printk("00[%s]Ran 0x%x, scr 0x%x, flag 0x%x\n",__FUNCTION__,g_isRandomize,mtd->isScramble,mtd->isCPdisable_W);
	//while (down_interruptible (&sem_NF_CARDREADER)) {
	//	printk("%s : Retry [Address: 0x%x]\n",__FUNCTION__,page);
	//	if(retryCnt<=0) {
	//		printk("%s : Retry fail [ADDR 0x%x]\n",__FUNCTION__,page);
	//		return -ERESTARTSYS;
	//	}
	//	retryCnt--;
	//}
	if(((uint32_t)data_buf&0x7)!=0) {
			printk("[%s]data_buf must 8 byte alignmemt!!\n",__FUNCTION__);
			BUG();
	}
	if(((uint32_t)this->g_oobbuf&0x7)!=0) {
			printk("[%s]oob_buf must 8 byte alignmemt!!\n",__FUNCTION__);
			BUG();
	}
#if 0       //stevie
	if ( chipnr == 0 && page < g_BootcodeSize/page_size ){
		if ( isBBT && page<3*ppb ){
			printk("[%s] Updating BBT %d page=%d\n", __FUNCTION__, page/ppb, page%ppb);
		}
		else {
			printk("[%s] You have no permission to write page %d\n", __FUNCTION__, page);
			//up (&sem_NF_CARDREADER);
			return -2;
		}
		mtd->isCPdisable_W = 1;
	}
	if ( chipnr == 0 && page >= g_BootcodeSize/page_size){
		if(page<(g_BootcodeSize+g_Factory_param_size)/page_size+ppb)
		{
			  mtd->isCPdisable_W = 1;
		}
	}
#endif
	//if(is_darwin_cpu()||is_macarthur_cpu()||is_nike_cpu())//Enable NF_WP pin (Write Protect Pin)
	//{
	//	if(g_WP_en)
	//	{
	//		setGPIOBit(g_regGPIODirBase,g_NFWP_no,NF_GPIO_OUT);//Set Driection
	//		setGPIOBit(g_regGPIOOutBase,g_NFWP_no,g_NFWP_value_dis);//Set Value
	//	}
	//}
#if 0       //stevie
	if ( page == ppb || page == ppb+1|| page == ppb*2 || page == ppb*2+1 )
		oob_1stB = BBT_TAG;
	else if(( page>=g_BootcodeSize/page_size )&&(page<(g_BootcodeSize+g_Factory_param_size)/page_size))
		oob_1stB = TAG_FACTORY_PARAM;
	else
		oob_1stB = 0xFF;
#endif
	oob_1stB = 0xFF;
	/*
	printk("[nand DBG]data_buf :  0x%x, g_oobbuf : 0x%x, g_databuf : 0x%x", (u32)data_buf, (u32)this->g_oobbuf, (u32)this->g_databuf);
		for(i=0; i < 512; i ++)
		{
			if(!(i %16))
				printf("\n");
			printf(" %02x",  data_buf[i]);
		}
	*/
	
	this->g_oobbuf[0x0] = oob_1stB;
	this->g_oobbuf[0x10] = oob_1stB;
	this->g_oobbuf[0x20] = oob_1stB;
	this->g_oobbuf[0x30] = oob_1stB;
	
	this->g_oobbuf[0x8] = oob_1stB;
	this->g_oobbuf[0x18] = oob_1stB;
	this->g_oobbuf[0x28] = oob_1stB;
	this->g_oobbuf[0x38] = oob_1stB;
	if(oob_size >= 0x78)
	{
		this->g_oobbuf[0x40] = oob_1stB;
		this->g_oobbuf[0x50] = oob_1stB;
		this->g_oobbuf[0x60] = oob_1stB;
		this->g_oobbuf[0x70] = oob_1stB;

		this->g_oobbuf[0x48] = oob_1stB;
		this->g_oobbuf[0x58] = oob_1stB;
		this->g_oobbuf[0x68] = oob_1stB;
		this->g_oobbuf[0x78] = oob_1stB;
	}
	
	//REG_WRITE_U32(REG_SRAM_CTL,0x00);//add by alexchang 0208-2010
	REG_WRITE_U32(REG_DATA_TL0,NF_DATA_TL0_length0(0));
	if(this->ecc_select >= 0x18)
	{
		// enable randomizer
		REG_WRITE_U32(REG_RMZ_CTRL, 1);
		
		page_len = page_size >> 10;
		REG_WRITE_U32(REG_DATA_TL1,NF_DATA_TL1_length1(4));
	}
	else
	{
		// set random write
		REG_WRITE_U32(REG_RND_EN, 1);
		REG_WRITE_U32(REG_RND_CMD1, 0x85);
		REG_WRITE_U32(REG_RND_DATA_STR_COL_H, 0);	// data start address MSB (always 0)
		REG_WRITE_U32(REG_RND_SPR_STR_COL_H, page_size >> 8);	// spare start address MSB
		REG_WRITE_U32(REG_RND_SPR_STR_COL_L, page_size & 0xff); // spare start address LSB
		
		page_len = page_size >> 9;
		REG_WRITE_U32(REG_DATA_TL1,NF_DATA_TL1_length1(2));
	}

	REG_WRITE_U32( REG_PAGE_LEN,NF_PAGE_LEN_page_len(page_len));

	//Set PP
	REG_WRITE_U32(REG_READ_BY_PP,NF_READ_BY_PP_read_by_pp(0));//add by alexchang 0208-2010
	REG_WRITE_U32(REG_PP_CTL1,NF_PP_CTL1_pp_start_addr(0));//add by alexchang 0208-2010
	REG_WRITE_U32(REG_PP_CTL0,0);//add by alexchang 0208-2010

	//Set command
	REG_WRITE_U32(REG_ND_CMD,NF_ND_CMD_cmd(CMD_PG_WRITE_C1));
	REG_WRITE_U32(REG_CMD2,NF_CMD2_cmd2(CMD_PG_WRITE_C2));
	REG_WRITE_U32(REG_CMD3,NF_CMD3_cmd3(CMD_PG_WRITE_C3));

	//Set address
	REG_WRITE_U32( REG_ND_PA0,NF_ND_PA0_page_addr0(page));
	REG_WRITE_U32( REG_ND_PA1,NF_ND_PA1_page_addr1(page>>8));
	REG_WRITE_U32( REG_ND_PA2,NF_ND_PA2_addr_mode(addr_mode)|NF_ND_PA2_page_addr2(page>>16));
	REG_WRITE_U32( REG_ND_PA3,NF_ND_PA3_page_addr3((page>>21)&0x7));
	REG_WRITE_U32(REG_ND_CA0, 0);
	REG_WRITE_U32(REG_ND_CA1, 0);

	//Set ECC

	// no ECC NF_MULTI_CHNL_MODE_ecc_pass(1)
	is_sync_mode = (REG_READ_U32(REG_NF_MODE) & 0x2);
	if(monflg == 5566)
	{
		printk("Set write ECC PASS \n");
		REG_WRITE_U32(REG_MULTI_CHNL_MODE,NF_MULTI_CHNL_MODE_edo(1)|NF_MULTI_CHNL_MODE_ecc_pass(1) | NF_MULTI_CHNL_MODE_ecc_no_check(1));
		//REG_WRITE_U32(REG_RMZ_CTRL, 1);
	}
	else
		REG_WRITE_U32(REG_MULTI_CHNL_MODE,NF_MULTI_CHNL_MODE_edo(1));//add by alexchang0205-2010

	if(is_sync_mode == 0x2)
		REG_WRITE_U32(REG_MULTI_CHNL_MODE,REG_READ_U32(REG_MULTI_CHNL_MODE) & (~(1<<5)));//sync mode doesn't support edo

	REG_WRITE_U32( REG_ECC_STOP,NF_ECC_STOP_ecc_n_stop(0x01));

	switch(this->ecc_select) {
		case 6:
			REG_WRITE_U32(REG_ECC_SEL, 0x0);
			break;
		case 12:
			REG_WRITE_U32(REG_ECC_SEL, 0x1);
			break;
		case 16:
			REG_WRITE_U32(REG_ECC_SEL, 0xe);
			break;
		case 24:
			REG_WRITE_U32(REG_ECC_SEL, 0xa);
			break;
		case 40:
			REG_WRITE_U32(REG_ECC_SEL, 0x2);
			break;
		case 43:
			REG_WRITE_U32(REG_ECC_SEL, 0x4);
			break;
		case 65:
			REG_WRITE_U32(REG_ECC_SEL, 0x6);
			break;
		case 72:
			REG_WRITE_U32(REG_ECC_SEL, 0x8);
			break;
		default:
			REG_WRITE_U32(REG_ECC_SEL, 0x0);
			break;
	}

	// flush cache.
	RTK_FLUSH_CACHE(data_buf, page_size);
	RTK_FLUSH_CACHE(this->g_oobbuf, oob_size);
	#if 0
	for(i = 0; i < page_size; i++)
	{
	if(!(i %16))
		printf("\n");
	printf(" %02x",  data_buf[i]);
	}
	#endif
	dram_sa = ( VIR_TO_PHY((uint32_t)data_buf) >> 3);
	REG_WRITE_U32(REG_DMA_CTL1,NF_DMA_CTL1_dram_sa(dram_sa));
	dma_len = page_size >> 9;
	REG_WRITE_U32(REG_DMA_CTL2,NF_DMA_CTL2_dma_len(dma_len));	
	
	//if (oob_buf)
	//	spare_dram_sa = ( (uint32_t)nf_oob_buf >> 3);
	//else
		spare_dram_sa = ( VIR_TO_PHY((uint32_t)this->g_oobbuf) >> 3);
	if(monflg == 5566)
		REG_WRITE_U32(REG_SPR_DDR_CTL,NF_SPR_DDR_CTL_spare_ddr_ena(0)|NF_SPR_DDR_CTL_per_2k_spr_ena(0)|NF_SPR_DDR_CTL_spare_dram_sa(spare_dram_sa));
	else		
		REG_WRITE_U32(REG_SPR_DDR_CTL,NF_SPR_DDR_CTL_spare_ddr_ena(1)|NF_SPR_DDR_CTL_per_2k_spr_ena(1)|NF_SPR_DDR_CTL_spare_dram_sa(spare_dram_sa));

	REG_WRITE_U32(REG_DMA_CTL3,NF_DMA_CTL3_ddr_wr(0)|NF_DMA_CTL3_dma_xfer(1));
	REG_WRITE_U32( REG_AUTO_TRIG,NF_AUTO_TRIG_auto_trig(1)|NF_AUTO_TRIG_spec_auto_case(0)| NF_AUTO_TRIG_auto_case(auto_trigger_mode));


//#if RTK_NAND_INTERRUPT
//	RTK_WAIT_EVENT;
//	NF_RESET_IRQ;
//	NF_ENABLE_IRQ(NF_POLL_IRQ_MASK);
//	g_nfFlag = NF_WRITE_TRIGGER;
//	REG_WRITE_U32(REG_POLL_FSTS,NF_POLL_FSTS_bit_sel(6)|NF_POLL_FSTS_trig_poll(1) );
//	RTK_WAIT_EVENT;
//#else
	WAIT_DONE(REG_AUTO_TRIG,0x80,0);
	WAIT_DONE(REG_DMA_CTL3,0x01,0);
	REG_WRITE_U32(REG_POLL_FSTS,NF_POLL_FSTS_bit_sel(6)|NF_POLL_FSTS_trig_poll(1) );
	WAIT_DONE(REG_POLL_FSTS,0x01,0x0);
	WAIT_DONE(REG_ND_CTL,0x40,0x40);//add by alexchang 0416-2010
//#endif
	return 0;

	if ( REG_READ_U32(REG_ND_DAT) & 0x01 ){
		//up (&sem_NF_CARDREADER);
		//up_write(&rw_sem);
		printk("[%s] write is not completed at page %d\n", __FUNCTION__, page);
		//padunlock(PAD_NAND);   // unlock nand・s pad
		return -1;
	}

	//while (1);
	return 0;
}

static int rtk_write_ecc_page_ori (struct mtd_info *mtd, u16 chipnr, unsigned int page,
			const u_char *data_buf, const  u_char *oob_buf, int isBBT)
{
	unsigned int ppb = mtd->erasesize/mtd->oobblock;
	struct nand_chip *this = (struct nand_chip *) mtd->priv;
	int rc = 0;
	uint8_t	auto_trigger_mode = 1;
	uint8_t	addr_mode = 1;

	unsigned int page_len, dram_sa, dma_len, spare_dram_sa;
	unsigned char oob_1stB;

	unsigned char nf_oob_buf[oob_size];
	unsigned int chip_section = 0;
	unsigned int section = 0;
	unsigned int index = 0;
	unsigned int retryCnt = RETRY_COUNT;

	printk("[%s] entry... \n", __FUNCTION__);
	//printk("00[%s]Ran 0x%x, scr 0x%x, flag 0x%x\n",__FUNCTION__,g_isRandomize,mtd->isScramble,mtd->isCPdisable_W);
	//while (down_interruptible (&sem_NF_CARDREADER)) {
	//	printk("%s : Retry [Address: 0x%x]\n",__FUNCTION__,page);
	//	if(retryCnt<=0) {
	//		printk("%s : Retry fail [ADDR 0x%x]\n",__FUNCTION__,page);
	//		return -ERESTARTSYS;
	//	}
	//	retryCnt--;
	//}
    if(((uint32_t)data_buf&0x7)!=0) {
            printk("[%s]data_buf must 8 byte alignmemt!!\n",__FUNCTION__);
            BUG();
    }
    if(((uint32_t)oob_buf&0x7)!=0) {
            printk("[%s]oob_buf must 8 byte alignmemt!!\n",__FUNCTION__);
            BUG();
    }

	if ( chipnr == 0 && page < g_BootcodeSize/page_size ){
		if ( isBBT && page<3*ppb ){
			printk("[%s] Updating BBT %d page=%d\n", __FUNCTION__, page/ppb, page%ppb);
		}
		else {
			printk("[%s] You have no permission to write page %d\n", __FUNCTION__, page);
			//up (&sem_NF_CARDREADER);
			return -2;
		}
		mtd->isCPdisable_W = 1;
	}
	if ( chipnr == 0 && page >= g_BootcodeSize/page_size){
		if(page<(g_BootcodeSize+g_Factory_param_size)/page_size+ppb)
		{
			  mtd->isCPdisable_W = 1;
		}
	}

	//if(is_darwin_cpu()||is_macarthur_cpu()||is_nike_cpu())//Enable NF_WP pin (Write Protect Pin)
	//{
	//	if(g_WP_en)
	//	{
	//		setGPIOBit(g_regGPIODirBase,g_NFWP_no,NF_GPIO_OUT);//Set Driection
	//		setGPIOBit(g_regGPIOOutBase,g_NFWP_no,g_NFWP_value_dis);//Set Value
	//	}
	//}

	if ( page == ppb || page == ppb+1|| page == ppb*2 || page == ppb*2+1 )
		oob_1stB = BBT_TAG;
	else if(( page>=g_BootcodeSize/page_size )&&(page<(g_BootcodeSize+g_Factory_param_size)/page_size))
		oob_1stB = TAG_FACTORY_PARAM;
	else
		oob_1stB = 0xFF;

	//REG_WRITE_U32(0xb801032c,0x01);	//Enable NAND/CardReader arbitrator add by alexchang 0303-2010

	/*
	if ( oob_buf )	{
		int i,j;
		//memset(nf_oob_buf,0xff,oob_size);
		nf_oob_buf[0] = oob_1stB;

		if(this->ecc_select == 0x18)
		{
			for(i=1,j=0; i<8; i++)
			{
				nf_oob_buf[8*i] = oob_buf[j];
				nf_oob_buf[8*i+1] = oob_buf[j+1];
				nf_oob_buf[8*i+2] = oob_buf[j+2];
				nf_oob_buf[8*i+3] = oob_buf[j+3];
				j+=4;
			}
		}
		else
		{
			for(i=0; i<4; i++)
				nf_oob_buf[1+i] = oob_buf[i];
			for(i=2,j=4; i<8; i+=2)
			{
				nf_oob_buf[4*i] = oob_buf[j];
				nf_oob_buf[4*i+1] = oob_buf[j+1];
				nf_oob_buf[4*i+2] = oob_buf[j+2];
				nf_oob_buf[4*i+3] = oob_buf[j+3];
				j+=4;
			}
		}
	}else
		nf_oob_buf[0] = oob_1stB;
	*/
	this->g_oobbuf[0x0] = oob_1stB;
	this->g_oobbuf[0x10] = oob_1stB;
	this->g_oobbuf[0x20] = oob_1stB;
	this->g_oobbuf[0x30] = oob_1stB;

    	this->g_oobbuf[0x8] = oob_1stB;
	this->g_oobbuf[0x18] = oob_1stB;
	this->g_oobbuf[0x28] = oob_1stB;
	this->g_oobbuf[0x38] = oob_1stB;

    	this->g_oobbuf[0x40] = oob_1stB;
	this->g_oobbuf[0x50] = oob_1stB;
	this->g_oobbuf[0x60] = oob_1stB;
	this->g_oobbuf[0x70] = oob_1stB;

    	this->g_oobbuf[0x48] = oob_1stB;
	this->g_oobbuf[0x58] = oob_1stB;
	this->g_oobbuf[0x68] = oob_1stB;
	this->g_oobbuf[0x78] = oob_1stB;

	//REG_WRITE_U32(REG_SRAM_CTL,0x00);//add by alexchang 0208-2010
	REG_WRITE_U32(REG_DATA_TL0,NF_DATA_TL0_length0(0));
 	if(this->ecc_select >= 0x18)
	{
		// enable randomizer
		REG_WRITE_U32(REG_RMZ_CTRL, 1);
		
		page_len = page_size >> 10;
		REG_WRITE_U32(REG_DATA_TL1,NF_DATA_TL1_length1(4));
	}
	else
	{
		// set random write
		REG_WRITE_U32(REG_RND_EN, 1);
		REG_WRITE_U32(REG_RND_CMD1, 0x85);
		REG_WRITE_U32(REG_RND_DATA_STR_COL_H, 0);	// data start address MSB (always 0)
		REG_WRITE_U32(REG_RND_SPR_STR_COL_H, page_size >> 8);	// spare start address MSB
		REG_WRITE_U32(REG_RND_SPR_STR_COL_L, page_size & 0xff);	// spare start address LSB
		
		page_len = page_size >> 9;
		REG_WRITE_U32(REG_DATA_TL1,NF_DATA_TL1_length1(2));
	}

	REG_WRITE_U32( REG_PAGE_LEN,NF_PAGE_LEN_page_len(page_len));

	//Set PP
	REG_WRITE_U32(REG_READ_BY_PP,NF_READ_BY_PP_read_by_pp(0));//add by alexchang 0208-2010
	REG_WRITE_U32(REG_PP_CTL1,NF_PP_CTL1_pp_start_addr(0));//add by alexchang 0208-2010
	REG_WRITE_U32(REG_PP_CTL0,0);//add by alexchang 0208-2010

	//Set command
 	REG_WRITE_U32(REG_ND_CMD,NF_ND_CMD_cmd(CMD_PG_WRITE_C1));
	REG_WRITE_U32(REG_CMD2,NF_CMD2_cmd2(CMD_PG_WRITE_C2));
	REG_WRITE_U32(REG_CMD3,NF_CMD3_cmd3(CMD_PG_WRITE_C3));

	//Set address
	REG_WRITE_U32( REG_ND_PA0,NF_ND_PA0_page_addr0(page));
	REG_WRITE_U32( REG_ND_PA1,NF_ND_PA1_page_addr1(page>>8));
	REG_WRITE_U32( REG_ND_PA2,NF_ND_PA2_addr_mode(addr_mode)|NF_ND_PA2_page_addr2(page>>16));
	REG_WRITE_U32( REG_ND_PA3,NF_ND_PA3_page_addr3((page>>21)&0x7));
	REG_WRITE_U32(REG_ND_CA0, 0);
	REG_WRITE_U32(REG_ND_CA1, 0);

	//Set ECC
	REG_WRITE_U32(REG_MULTI_CHNL_MODE,NF_MULTI_CHNL_MODE_edo(1));//add by alexchang0205-2010
	REG_WRITE_U32( REG_ECC_STOP,NF_ECC_STOP_ecc_n_stop(0x01));

	switch(this->ecc_select) {
		case 6:
			REG_WRITE_U32(REG_ECC_SEL, 0x0);
			break;
		case 12:
			REG_WRITE_U32(REG_ECC_SEL, 0x1);
			break;
		case 16:
			REG_WRITE_U32(REG_ECC_SEL, 0xe);
			break;
		case 24:
			REG_WRITE_U32(REG_ECC_SEL, 0xa);
			break;
		case 40:
			REG_WRITE_U32(REG_ECC_SEL, 0x2);
			break;
		case 43:
			REG_WRITE_U32(REG_ECC_SEL, 0x4);
			break;
		case 65:
			REG_WRITE_U32(REG_ECC_SEL, 0x6);
			break;
		case 72:
			REG_WRITE_U32(REG_ECC_SEL, 0x8);
			break;
		default:
			REG_WRITE_U32(REG_ECC_SEL, 0x0);
			break;
	}

	// flush cache.
	RTK_FLUSH_CACHE(data_buf, page_size);
	RTK_FLUSH_CACHE(this->g_oobbuf, oob_size);
	
	dram_sa = ( (uint32_t)data_buf >> 3);
	REG_WRITE_U32(REG_DMA_CTL1,NF_DMA_CTL1_dram_sa(dram_sa));
	dma_len = page_size >> 9;
	REG_WRITE_U32(REG_DMA_CTL2,NF_DMA_CTL2_dma_len(dma_len));	
	
	//if (oob_buf)
	//	spare_dram_sa = ( (uint32_t)nf_oob_buf >> 3);
	//else
		spare_dram_sa = ( (uint32_t)this->g_oobbuf >> 3);
	REG_WRITE_U32(REG_SPR_DDR_CTL,NF_SPR_DDR_CTL_spare_ddr_ena(1)|NF_SPR_DDR_CTL_per_2k_spr_ena(1)|NF_SPR_DDR_CTL_spare_dram_sa(spare_dram_sa));

#if 0
#if RTK_NAND_INTERRUPT
	NF_RESET_IRQ;
	NF_ENABLE_IRQ(NF_AUTO_IRQ_MASK);
	g_nfFlag = NF_WRITE_TRIGGER;
#endif
	//printk("11[%s]Ran 0x%x, scr 0x%x, flag 0x%x\n",__FUNCTION__,g_isRandomize,mtd->isScramble,mtd->isCPdisable_W);

	if(g_isRandomize&&(!mtd->isCPdisable_W))
	{
//		if(mtd->isScramble!=MTD_SCRAMBLE_DISABLE)
//		{
			//printk("$");
			if(u32CP_mode==0)//CBC initial key setting.
			{
					REG_WRITE_U32( REG_CP_NF_INI_KEY,page);
			}
			REG_WRITE_U32(REG_CP_NF_KEY,0x12345678);
			REG_WRITE_U32((REG_CP_NF_KEY+0x4),0x12345678);
			REG_WRITE_U32((REG_CP_NF_KEY+0x8),0x12345678);
			REG_WRITE_U32((REG_CP_NF_KEY+0xc),0x12345678);

			REG_WRITE_U32(REG_CP_NF_SET,u32CP_mode|cpSel);
			REG_WRITE_U32(REG_CP_LEN,NF_CP_LEN_cp_length(page_size));
			RTK_FLUSH_CACHE((unsigned long) data_buf, page_size);
			RTK_FLUSH_CACHE((unsigned long) nf_oob_buf, oob_size);
			REG_WRITE_U32(REG_DMA_CTL3,NF_DMA_CTL3_cp_enable(0x1)|NF_DMA_CTL3_cp_first(0x1)|NF_DMA_CTL3_ddr_wr(0)|NF_DMA_CTL3_dma_xfer(0x1));
//		}
//		else
//		{//printk("=");
//			RTK_FLUSH_CACHE((unsigned long) data_buf, page_size);
//			RTK_FLUSH_CACHE((unsigned long) nf_oob_buf, oob_size);
//			REG_WRITE_U32(REG_DMA_CTL3,NF_DMA_CTL3_cp_enable(0)|NF_DMA_CTL3_cp_first(0)|NF_DMA_CTL3_ddr_wr(0)|NF_DMA_CTL3_dma_xfer(0x1));
//		}
	}
	else
	{
		if(is_NF_CP_Enable_write&&(!mtd->isCPdisable_W))
		{//printk("+");
			REG_WRITE_U32(REG_CP_NF_KEY,*regKey0);
			REG_WRITE_U32((REG_CP_NF_KEY+0x4),*regKey1);
			REG_WRITE_U32((REG_CP_NF_KEY+0x8),*regKey2);
			REG_WRITE_U32((REG_CP_NF_KEY+0xc),*regKey3);

			REG_WRITE_U32(REG_CP_NF_SET,u32CP_mode|cpSel);
			REG_WRITE_U32(REG_CP_LEN,NF_CP_LEN_cp_length(page_size));
			RTK_FLUSH_CACHE((unsigned long) data_buf, page_size);
			RTK_FLUSH_CACHE((unsigned long) nf_oob_buf, oob_size);
			REG_WRITE_U32(REG_DMA_CTL3,NF_DMA_CTL3_cp_enable(0x1)|NF_DMA_CTL3_cp_first(0x1)|NF_DMA_CTL3_ddr_wr(0)|NF_DMA_CTL3_dma_xfer(0x1));
		}
		else
		{//printk("-");
			RTK_FLUSH_CACHE((unsigned long) data_buf, page_size);
			RTK_FLUSH_CACHE((unsigned long) nf_oob_buf, oob_size);
			REG_WRITE_U32(REG_DMA_CTL3,NF_DMA_CTL3_cp_enable(0)|NF_DMA_CTL3_cp_first(0)|NF_DMA_CTL3_ddr_wr(0)|NF_DMA_CTL3_dma_xfer(0x1));
		}
	}

#endif
	REG_WRITE_U32(REG_DMA_CTL3,NF_DMA_CTL3_ddr_wr(0)|NF_DMA_CTL3_dma_xfer(1));
	REG_WRITE_U32( REG_AUTO_TRIG,NF_AUTO_TRIG_auto_trig(1)|NF_AUTO_TRIG_spec_auto_case(0)| NF_AUTO_TRIG_auto_case(auto_trigger_mode));


//#if RTK_NAND_INTERRUPT
//	RTK_WAIT_EVENT;
//	NF_RESET_IRQ;
//	NF_ENABLE_IRQ(NF_POLL_IRQ_MASK);
//	g_nfFlag = NF_WRITE_TRIGGER;
//	REG_WRITE_U32(REG_POLL_FSTS,NF_POLL_FSTS_bit_sel(6)|NF_POLL_FSTS_trig_poll(1) );
//	RTK_WAIT_EVENT;
//#else
	WAIT_DONE(REG_AUTO_TRIG,0x80,0);
	WAIT_DONE(REG_DMA_CTL3,0x01,0);
	REG_WRITE_U32(REG_POLL_FSTS,NF_POLL_FSTS_bit_sel(6)|NF_POLL_FSTS_trig_poll(1) );
	WAIT_DONE(REG_POLL_FSTS,0x01,0x0);
	WAIT_DONE(REG_ND_CTL,0x40,0x40);//add by alexchang 0416-2010
//#endif

	//while (1);
	
	return 0;

	if(is_darwin_cpu()||is_macarthur_cpu()||is_nike_cpu())//Enable NF_WP pin (Write Protect Pin)
	{
		if(g_WP_en)
		{
			setGPIOBit(g_regGPIODirBase,g_NFWP_no,NF_GPIO_OUT);//Set Driection
			setGPIOBit(g_regGPIOOutBase,g_NFWP_no,g_NFWP_value_en);//Set Value
		}
	}

	if((is_NF_CP_Enable_write&&(!mtd->isCPdisable_W))||g_isRandomize)//Clear register
	{
			//REG_WRITE_U32(REG_CP_NF_SET,0);
	}


	if ( REG_READ_U32(REG_ND_DAT) & 0x01 ){
		//up (&sem_NF_CARDREADER);
		printk("[%s] write is not completed at page %d\n", __FUNCTION__, page);
		return -1;
	}

	chip_section = (chipnr * this->page_num) >> 5;
	section = page >> 5;
	index = page & (32-1);

	if (this->erase_page_flag)
		this->erase_page_flag[chip_section+section] &= ~(1 << index);
	mtd->isCPdisable_W = 0;
	//printk("[AT]W block : %u , page: %u \n",page/ppb,page%ppb);
	//up (&sem_NF_CARDREADER);
#if 0
	char *ppptr = (unsigned long)bbbbb-(unsigned long)bbbbb%8+8;
	rtk_read_ecc_page (mtd, chipnr, page, ppptr, oob_buf);
	if(memcmp(ppptr,data_buf, 2048))
		{
			int i, count=0;
			for(i=0;i<2048;i++)
		 		if(ppptr[i] != data_buf[i])
				 count++;
				printk("Data compare error! %d %x %x %x %p\n", count, ppptr[0],
				ppptr[1], ppptr[2], ppptr); //BUG();
				}
	else
		printk("write check OK!\n");
#endif



	return rc;
}

//----------------------------------------------------------------------------
static int rtk_write_oob(struct mtd_info *mtd, u16 chipnr, int page, int len, const u_char *oob_buf)
{
	struct nand_chip *this = (struct nand_chip *) mtd->priv;
	int rc = 0;
	uint8_t	auto_trigger_mode = 1;
	uint8_t	addr_mode = 1;
	unsigned int page_len, dram_sa, dma_len, spare_dram_sa;
	unsigned char oob_1stB;
	unsigned int chip_section = 0;
	unsigned int section =0;
	unsigned int index =0;
	unsigned int retryCnt = RETRY_COUNT;
	if (this->erase_page_flag)
		this->erase_page_flag[chip_section+section] &= ~(1 << index);
	memset(this->g_databuf, 0xff, page_size);

	page_size = mtd->oobblock;
	oob_size = mtd->oobsize;
	ppb = mtd->erasesize/mtd->oobblock;

	//while (down_interruptible (&sem_NF_CARDREADER)) {
	//	printk("%s : Retry [Address: 0x%x]\n",__FUNCTION__,page);
	//	if(retryCnt<=0) {
	//		printk("%s : Retry fail [ADDR 0x%x]\n",__FUNCTION__,page);
	//		return -ERESTARTSYS;
	//	}
	//	retryCnt--;
	//}
	if ( chipnr == 0 && page >= 0 && page < g_BootcodeSize/page_size ){
		printk("[%s] You have no permission to write this page %d\n", __FUNCTION__, page);
		//up (&sem_NF_CARDREADER);
		return -2;
	}

	if ( len>oob_size || !oob_buf ){
		printk("[%s] error: len>oob_size OR oob_buf is NULL\n", __FUNCTION__);
		//up (&sem_NF_CARDREADER);
		return -1;
	}

	//if(is_darwin_cpu()||is_macarthur_cpu()||is_nike_cpu())//Enable NF_WP pin (Write Protect Pin)
	//{
	//	if(g_WP_en)
	//	{
	//		setGPIOBit(g_regGPIODirBase,g_NFWP_no,NF_GPIO_OUT);//Set Driection
	//		setGPIOBit(g_regGPIOOutBase,g_NFWP_no,g_NFWP_value_dis);//Set Value
	//	}
	//}

	if ( page == ppb || page == ppb+1|| page == ppb*2 || page == ppb*2+1 )
		oob_1stB = BBT_TAG;
	else if(( page>=g_BootcodeSize/page_size )&&(page<(g_BootcodeSize+g_Factory_param_size)/page_size))
		oob_1stB = TAG_FACTORY_PARAM;
	else
		oob_1stB = 0xFF;
//end of alexchang 0208-2010
	//REG_WRITE_U32(0x01,0xb801032c);	//Enable NAND/CardReader arbitrator add by alexchang 0303-2010
	REG_WRITE_U32(REG_SRAM_CTL,0x00);//add by alexchang 0208-2010
	REG_WRITE_U32(REG_DATA_TL0,NF_DATA_TL0_length0(0));
 	if(this->ecc_select >= 0x18)
	{
		page_len = page_size >> 10;
		REG_WRITE_U32(REG_DATA_TL1,NF_DATA_TL1_length1(4));
	}
	else
	{
		page_len = page_size >> 9;
		REG_WRITE_U32(REG_DATA_TL1,NF_DATA_TL1_length1(2));
	}

	REG_WRITE_U32(REG_READ_BY_PP,NF_READ_BY_PP_read_by_pp(0));//add by alexchang 0208-2010
	REG_WRITE_U32(REG_PP_CTL1,NF_PP_CTL1_pp_start_addr(0));//add by alexchang 0208-2010
	REG_WRITE_U32(REG_PP_CTL0,0);//add by alexchang 0208-2010

	REG_WRITE_U32( REG_PAGE_LEN,NF_PAGE_LEN_page_len(page_len));

 	REG_WRITE_U32(REG_ND_CMD,NF_ND_CMD_cmd(CMD_PG_WRITE_C1));
	REG_WRITE_U32(REG_CMD2,NF_CMD2_cmd2(CMD_PG_WRITE_C2));
	REG_WRITE_U32(REG_CMD3,NF_CMD3_cmd3(CMD_PG_WRITE_C3));

	REG_WRITE_U32( REG_ND_PA0,NF_ND_PA0_page_addr0(page));
	REG_WRITE_U32( REG_ND_PA1,NF_ND_PA1_page_addr1(page>>8));
	REG_WRITE_U32( REG_ND_PA2,NF_ND_PA2_addr_mode(addr_mode)|NF_ND_PA2_page_addr2(page>>16));
	REG_WRITE_U32( REG_ND_PA3,NF_ND_PA3_page_addr3((page>>21)&0x7));
	REG_WRITE_U32(REG_MULTI_CHNL_MODE,NF_MULTI_CHNL_MODE_edo(1));//add by alexchang0205-2010

	switch(this->ecc_select) {
		case 6:
			REG_WRITE_U32(REG_ECC_SEL, 0x0);
			break;
		case 12:
			REG_WRITE_U32(REG_ECC_SEL, 0x1);
			break;
		case 16:
			REG_WRITE_U32(REG_ECC_SEL, 0xe);
			break;
		case 24:
			REG_WRITE_U32(REG_ECC_SEL, 0xa);
			break;
		case 40:
			REG_WRITE_U32(REG_ECC_SEL, 0x2);
			break;
		case 43:
			REG_WRITE_U32(REG_ECC_SEL, 0x4);
			break;
		case 65:
			REG_WRITE_U32(REG_ECC_SEL, 0x6);
			break;
		case 72:
			REG_WRITE_U32(REG_ECC_SEL, 0x8);
			break;
		default:
			REG_WRITE_U32(REG_ECC_SEL, 0x0);
			break;
	}

	dram_sa = ( (uint32_t)this->g_databuf >> 3);
	REG_WRITE_U32(REG_DMA_CTL1,NF_DMA_CTL1_dram_sa(dram_sa));
	dma_len = page_size >> 9;
	REG_WRITE_U32(REG_DMA_CTL2,NF_DMA_CTL2_dma_len(dma_len));	
	REG_WRITE_U32(REG_DMA_CTL3,NF_DMA_CTL3_ddr_wr(1)|NF_DMA_CTL3_dma_xfer(1));

	spare_dram_sa = ( (uint32_t)oob_buf >> 3);
	REG_WRITE_U32( REG_SPR_DDR_CTL,NF_SPR_DDR_CTL_spare_ddr_ena(1)|NF_SPR_DDR_CTL_per_2k_spr_ena(1)|NF_SPR_DDR_CTL_spare_dram_sa(spare_dram_sa));
#if 0	
#if RTK_NAND_INTERRUPT
	NF_RESET_IRQ;
	NF_ENABLE_IRQ(NF_AUTO_IRQ_MASK);
	g_nfFlag = NF_WRITE_TRIGGER;
#endif
	if(g_isRandomize&&(!mtd->isCPdisable_W))
	{
		if(mtd->isScramble!=MTD_SCRAMBLE_DISABLE)
		{
			//printk("&");
			if(u32CP_mode==0)//CBC initial key setting.
			{
					REG_WRITE_U32( REG_CP_NF_INI_KEY,page);
			}
			REG_WRITE_U32(REG_CP_NF_KEY,0x12345678);
			REG_WRITE_U32((REG_CP_NF_KEY+0x4),0x12345678);
			REG_WRITE_U32((REG_CP_NF_KEY+0x8),0x12345678);
			REG_WRITE_U32((REG_CP_NF_KEY+0xc),0x12345678);


			REG_WRITE_U32(REG_CP_NF_SET,u32CP_mode|cpSel);
			REG_WRITE_U32(REG_CP_LEN,NF_CP_LEN_cp_length(page_size));
			//add by alexchang 0208-2010
			RTK_FLUSH_CACHE((unsigned long) this->g_databuf, page_size);
			if ( oob_buf )
				RTK_FLUSH_CACHE((unsigned long) oob_buf, oob_size);
			REG_WRITE_U32(REG_DMA_CTL3,NF_DMA_CTL3_cp_enable(0x1)|NF_DMA_CTL3_cp_first(0x1)|NF_DMA_CTL3_ddr_wr(0)|NF_DMA_CTL3_dma_xfer(0x1));
		}
		else
		{
			//printk("%");
			RTK_FLUSH_CACHE((unsigned long) this->g_databuf, page_size);
			if ( oob_buf )
				RTK_FLUSH_CACHE((unsigned long) oob_buf, oob_size);
			REG_WRITE_U32(REG_DMA_CTL3,NF_DMA_CTL3_cp_enable(0)|NF_DMA_CTL3_cp_first(0)|NF_DMA_CTL3_ddr_wr(0)|NF_DMA_CTL3_dma_xfer(0x1));
		}
	}
	else
	{
		if(is_NF_CP_Enable_write&&(!mtd->isCPdisable_W))
		{//printk("#");
			REG_WRITE_U32(REG_CP_NF_KEY,*regKey0);
			REG_WRITE_U32((REG_CP_NF_KEY+0x4),*regKey1);
			REG_WRITE_U32((REG_CP_NF_KEY+0x8),*regKey2);
			REG_WRITE_U32((REG_CP_NF_KEY+0xc),*regKey3);


			REG_WRITE_U32(REG_CP_NF_SET,u32CP_mode|cpSel);
			REG_WRITE_U32(REG_CP_LEN,NF_CP_LEN_cp_length(page_size));
			//add by alexchang 0208-2010
			RTK_FLUSH_CACHE((unsigned long) this->g_databuf, page_size);
			if ( oob_buf )
				RTK_FLUSH_CACHE((unsigned long) oob_buf, oob_size);
			REG_WRITE_U32(REG_DMA_CTL3,NF_DMA_CTL3_cp_enable(0x1)|NF_DMA_CTL3_cp_first(0x1)|NF_DMA_CTL3_ddr_wr(0)|NF_DMA_CTL3_dma_xfer(0x1));
		}
		else
		{//printk("@");
			//add by alexchang 0208-2010
			RTK_FLUSH_CACHE((unsigned long) this->g_databuf, page_size);
			if ( oob_buf )
				RTK_FLUSH_CACHE((unsigned long) oob_buf, oob_size);
			REG_WRITE_U32(REG_DMA_CTL3,NF_DMA_CTL3_cp_enable(0)|NF_DMA_CTL3_cp_first(0)|NF_DMA_CTL3_ddr_wr(0)|NF_DMA_CTL3_dma_xfer(0x1));
		}
	}
#endif
	REG_WRITE_U32( REG_AUTO_TRIG,NF_AUTO_TRIG_auto_trig(1)|NF_AUTO_TRIG_spec_auto_case(0)| NF_AUTO_TRIG_auto_case(auto_trigger_mode));

//#if RTK_NAND_INTERRUPT
//	RTK_WAIT_EVENT;
//	NF_RESET_IRQ;
//	NF_ENABLE_IRQ(NF_POLL_IRQ_MASK);
//	g_nfFlag = NF_WRITE_TRIGGER;
//	REG_WRITE_U32(REG_POLL_FSTS,NF_POLL_FSTS_bit_sel(6)|NF_POLL_FSTS_trig_poll(1) );
//	RTK_WAIT_EVENT;
//#else
	WAIT_DONE(REG_AUTO_TRIG,0x80,0);
	WAIT_DONE(REG_DMA_CTL3,0x01,0);
	REG_WRITE_U32(REG_POLL_FSTS,NF_POLL_FSTS_bit_sel(6)|NF_POLL_FSTS_trig_poll(1) );
	WAIT_DONE(REG_POLL_FSTS,0x01,0x0);
	WAIT_DONE(REG_ND_CTL,0x40,0x40);//add by alexchang 0416-2010
//#endif

	return 0;

	if(is_darwin_cpu()||is_macarthur_cpu()||is_nike_cpu())//Enable NF_WP pin (Write Protect Pin)
	{
		if(g_WP_en)
		{
			setGPIOBit(g_regGPIODirBase,g_NFWP_no,NF_GPIO_OUT);//Set Driection
			setGPIOBit(g_regGPIOOutBase,g_NFWP_no,g_NFWP_value_en);//Set Value
		}
	}

	if((is_NF_CP_Enable_write&&(!mtd->isCPdisable_W))||g_isRandomize)//Clear register
	{
		REG_WRITE_U32(REG_CP_NF_SET,0);
	}

	if ( REG_READ_U32(REG_ND_DAT) & 0x01 ){
		//up (&sem_NF_CARDREADER);
		printk("[%s] write oob is not completed at page %d\n", __FUNCTION__, page);

		return -1;
	}

	chip_section = (chipnr * this->page_num) >> 5;
	section = page >> 5;
	index = page & (32-1);
	if (this->erase_page_flag)
		this->erase_page_flag[chip_section+section] &= ~(1 << index);
	mtd->isCPdisable_W = 0;
	//printk("[AT]WO block : %u ,page : %d  \n",page/ppb,page%ppb);
	//up (&sem_NF_CARDREADER);
//int k= 0;
//for(k=0;k<32;k++)
//printk("[WO][%u,%u][%d]0x%x\n",page/ppb,page%ppb,k,oob_buf[k]);
//printk("----------------------------\n");

	return rc;
}

//----------------------------------------------------------------------------
static int rtk_erase_block(struct mtd_info *mtd, u16 chipnr, int page)
{
	struct nand_chip *this = NULL;
	unsigned int chip_section = 0;
	unsigned int section = 0;
	unsigned int retryCnt = RETRY_COUNT;
	page_size = mtd->oobblock;
	oob_size = mtd->oobsize;
	ppb = mtd->erasesize/mtd->oobblock;

	this = (struct nand_chip *) mtd->priv;
//	printk("AT[%s]:page %d, ppb %d, mtd->erasesize %u, mtd->oobblock %u\n",__FUNCTION__,page,ppb,mtd->erasesize,mtd->oobblock);
	if ( page & (ppb-1) ){
		printk("%s: page %d is not block alignment !!\n", __FUNCTION__, page);
		//up (&sem_NF_CARDREADER);
		return -1;
	}

	this->select_chip(mtd, chipnr);
#if 0
			int i,id_chain;
			REG_WRITE_U32(REG_DATA_TL0,NF_DATA_TL0_length0(0x00));
			REG_WRITE_U32(REG_DATA_TL1,NF_DATA_TL1_access_mode(0x01));
		
			//Set PP
			REG_WRITE_U32(REG_READ_BY_PP,NF_READ_BY_PP_read_by_pp(0x0));
		
			REG_WRITE_U32(REG_PP_CTL0,NF_PP_CTL0_pp_enable(0x01));
			REG_WRITE_U32(REG_PP_CTL1,NF_PP_CTL1_pp_start_addr(0));
		
			//Set command
			REG_WRITE_U32(REG_ND_CMD,NF_ND_CMD_cmd(CMD_BLK_ERASE_C1));
			REG_WRITE_U32(REG_ND_CTL,NF_ND_CTL_xfer(0x01));
			WAIT_DONE(REG_ND_CTL,0x80,0);
		
			//Set address
			REG_WRITE_U32(REG_ND_PA0, page&0xff);
			REG_WRITE_U32(REG_ND_PA1, (page>>8)&0xff);
			REG_WRITE_U32(REG_ND_PA2,NF_ND_PA2_addr_mode(0x04) | (page>>16)&0x1f);
		
			REG_WRITE_U32(REG_ND_CTL,NF_ND_CTL_xfer(1)|NF_ND_CTL_tran_mode(1));
			WAIT_DONE(REG_ND_CTL,0x80,0);
			//Set read command 2
			REG_WRITE_U32(REG_ND_CMD,NF_ND_CMD_cmd(CMD_BLK_ERASE_C2));
			REG_WRITE_U32(REG_ND_CTL,NF_ND_CTL_xfer(0x01));
			WAIT_DONE(REG_ND_CTL,0x80,0);	
			WAIT_DONE(REG_ND_CTL,0x40,0x40);
#endif

#if 0// RTK_NAND_INTERRUPT
	g_nfFlag = NF_ERASE_TRIGGER;
	NF_RESET_IRQ;
#endif

#if 1
	//REG_WRITE_U32(0x01,0xb801032c);	//Enable NAND/CardReader arbitrator add by alexchang 0303-2010
	REG_WRITE_U32( REG_MULTI_CHNL_MODE,NF_MULTI_CHNL_MODE_no_wait_busy(1)|NF_MULTI_CHNL_MODE_edo(1));

	REG_WRITE_U32( REG_ND_CMD,NF_ND_CMD_cmd(CMD_BLK_ERASE_C1) );
	REG_WRITE_U32( REG_CMD2,NF_CMD2_cmd2(CMD_BLK_ERASE_C2) );
	REG_WRITE_U32( REG_CMD3,NF_CMD3_cmd3(CMD_BLK_ERASE_C3) );

	REG_WRITE_U32( REG_ND_PA0,NF_ND_PA0_page_addr0(page) );
	REG_WRITE_U32( REG_ND_PA1,NF_ND_PA1_page_addr1(page>>8) );
	REG_WRITE_U32( REG_ND_PA2,NF_ND_PA2_addr_mode(0x04)|NF_ND_PA2_page_addr2(page>>16) );
	REG_WRITE_U32( REG_ND_PA3,NF_ND_PA3_page_addr3((page>> 21)&0x7) );

	REG_WRITE_U32( REG_AUTO_TRIG,NF_AUTO_TRIG_auto_trig(1)|NF_AUTO_TRIG_spec_auto_case(1)|NF_AUTO_TRIG_auto_case(2) );
	WAIT_DONE(REG_AUTO_TRIG,0x80,0);
#if  RTK_NAND_INTERRUPT
	NF_RESET_IRQ;
	NF_ENABLE_IRQ(NF_POLL_IRQ_MASK);
	g_nfFlag = NF_ERASE_TRIGGER;
	REG_WRITE_U32(REG_POLL_FSTS,NF_POLL_FSTS_bit_sel(6)|NF_POLL_FSTS_trig_poll(1) );
	RTK_WAIT_EVENT;
	NF_RESET_IRQ;
	NF_ENABLE_IRQ(NF_IRQ_MASK);
#else
	REG_WRITE_U32(REG_POLL_FSTS,NF_POLL_FSTS_bit_sel(6)|NF_POLL_FSTS_trig_poll(1) );
	WAIT_DONE(REG_POLL_FSTS,0x01,0x0);
	WAIT_DONE(REG_ND_CTL,0x40,0x40);//add by alexchang 0416-2010
#endif

	if ( REG_READ_U32(REG_ND_DAT) & 0x01 ){
		//up (&sem_NF_CARDREADER);
		printk("[%s] erasure is not completed at block %d\n", __FUNCTION__, page/ppb);
		if ( chipnr == 0 && page >= 0 && page < g_BootcodeSize/page_size )
			return 0;
		else
			return -1;
	}

	chip_section = (chipnr * this->page_num) >> 5;
	section = page >> 5;
	if(this->erase_page_flag)
		memset ( (__u32 *)(this->erase_page_flag)+ chip_section + section, 0xff, ppb>>3);
	//printk("[AT]Erase block : %u	\n",page/ppb);
	//up (&sem_NF_CARDREADER);
#if 0// RTK_ARD_ALGORITHM
	g_RecArray[page/ppb]=0;
//	printk("[AT]Erase block : %u  \n",page/ppb);
#endif
#endif
	printk("[AT]Erase block : %u  \n",page/ppb);

	return 0;
}



//----------------------------------------------------------------------------
static int rtk_erase_block_ori(struct mtd_info *mtd, u16 chipnr, int page)
{
	struct nand_chip *this = NULL;
	unsigned int chip_section = 0;
	unsigned int section = 0;
	unsigned int retryCnt = RETRY_COUNT;
	page_size = mtd->oobblock;
	oob_size = mtd->oobsize;
	ppb = mtd->erasesize/mtd->oobblock;
//printk("erasesize 0x%x,oobblock 0x%x\n",mtd->erasesize,mtd->oobblock);
//printk("AT[%s]:show chipnr %d\n",__FUNCTION__,chipnr);

	//while (down_interruptible (&sem_NF_CARDREADER)) {
	//	printk("%s : Retry [Address: 0x%x]\n",__FUNCTION__,page);
	//	if(retryCnt<=0)
	//	{
	//		printk("%s : Retry fail [ADDR 0x%x]\n",__FUNCTION__,page);
	//		return -ERESTARTSYS;
	//	}
	//	retryCnt--;
	//}
	this = (struct nand_chip *) mtd->priv;
//	printk("AT[%s]:page %d, ppb %d, mtd->erasesize %u, mtd->oobblock %u\n",__FUNCTION__,page,ppb,mtd->erasesize,mtd->oobblock);
	if ( page & (ppb-1) ){
		printk("%s: page %d is not block alignment !!\n", __FUNCTION__, page);
		//up (&sem_NF_CARDREADER);
		return -1;
	}

	//if(is_darwin_cpu()||is_macarthur_cpu()||is_nike_cpu())//Disable NF_WP pin (Write Protect Pin)
	//{
	//	if(g_WP_en)
	//	{
	//		setGPIOBit(g_regGPIODirBase,g_NFWP_no,NF_GPIO_OUT);//Set Driection
	//		setGPIOBit(g_regGPIOOutBase,g_NFWP_no,g_NFWP_value_dis);//Set Value
	//	}
	//}

#if RTK_NAND_INTERRUPT
	g_nfFlag = NF_ERASE_TRIGGER;
	NF_RESET_IRQ;
#endif

	//REG_WRITE_U32(0x01,0xb801032c);	//Enable NAND/CardReader arbitrator add by alexchang 0303-2010
	REG_WRITE_U32( REG_MULTI_CHNL_MODE,NF_MULTI_CHNL_MODE_no_wait_busy(1)|NF_MULTI_CHNL_MODE_edo(1));

	REG_WRITE_U32( REG_ND_CMD,NF_ND_CMD_cmd(CMD_BLK_ERASE_C1) );
	REG_WRITE_U32( REG_CMD2,NF_CMD2_cmd2(CMD_BLK_ERASE_C2) );
	REG_WRITE_U32( REG_CMD3,NF_CMD3_cmd3(CMD_BLK_ERASE_C3) );

	REG_WRITE_U32( REG_ND_PA0,NF_ND_PA0_page_addr0(page) );
	REG_WRITE_U32( REG_ND_PA1,NF_ND_PA1_page_addr1(page>>8) );
	REG_WRITE_U32( REG_ND_PA2,NF_ND_PA2_addr_mode(0x04)|NF_ND_PA2_page_addr2(page>>16) );
	REG_WRITE_U32( REG_ND_PA3,NF_ND_PA3_page_addr3((page>> 21)&0x7) );

	REG_WRITE_U32( REG_AUTO_TRIG,NF_AUTO_TRIG_auto_trig(1)|NF_AUTO_TRIG_spec_auto_case(1)|NF_AUTO_TRIG_auto_case(2) );
	WAIT_DONE(REG_AUTO_TRIG,0x80,0);

#if RTK_NAND_INTERRUPT
	NF_RESET_IRQ;
	NF_ENABLE_IRQ(NF_POLL_IRQ_MASK);
	g_nfFlag = NF_ERASE_TRIGGER;
	REG_WRITE_U32(REG_POLL_FSTS,NF_POLL_FSTS_bit_sel(6)|NF_POLL_FSTS_trig_poll(1) );
	RTK_WAIT_EVENT;
	NF_RESET_IRQ;
	NF_ENABLE_IRQ(NF_IRQ_MASK);
#else
	REG_WRITE_U32(REG_POLL_FSTS,NF_POLL_FSTS_bit_sel(6)|NF_POLL_FSTS_trig_poll(1) );
	WAIT_DONE(REG_POLL_FSTS,0x01,0x0);
	WAIT_DONE(REG_ND_CTL,0x40,0x40);//add by alexchang 0416-2010
#endif

	//if(is_darwin_cpu()||is_macarthur_cpu()||is_nike_cpu())//Enable NF_WP pin (Write Protect Pin)
	//{
	//	if(g_WP_en)
	//	{
	//		setGPIOBit(g_regGPIODirBase,g_NFWP_no,NF_GPIO_OUT);//Set Driection
	//		setGPIOBit(g_regGPIOOutBase,g_NFWP_no,g_NFWP_value_en);//Set Value
	//	}
	//}

	if ( REG_READ_U32(REG_ND_DAT) & 0x01 ){
		//up (&sem_NF_CARDREADER);
		printk("[%s] erasure is not completed at block %d\n", __FUNCTION__, page/ppb);
		if ( chipnr == 0 && page >= 0 && page < g_BootcodeSize/page_size )
			return 0;
		else
			return -1;
	}

	chip_section = (chipnr * this->page_num) >> 5;
	section = page >> 5;
	if(this->erase_page_flag)
		memset ( (__u32 *)(this->erase_page_flag)+ chip_section + section, 0xff, ppb>>3);
	//printk("[AT]Erase block : %u  \n",page/ppb);
	//up (&sem_NF_CARDREADER);
#if RTK_ARD_ALGORITHM
	g_RecArray[page/ppb]=0;
//	printk("[AT]Erase block : %u  \n",page/ppb);
#endif
	return 0;
}

//----------------------------------------------------------------------------
unsigned int rtkNF_getRBAPercent(void)
{
    unsigned char *param;
	unsigned int retRbaPercent = -1 ;
//    param=(char *)rtkNF_parse_token(platform_info.system_parameters,"rba_percent");

    if(param)
		retRbaPercent = simple_strtoul(param, (char**)&retRbaPercent, 16);
    return retRbaPercent;
}


//----------------------------------------------------------------------------
static int rtk_nand_profile (void)
{
	int maxchips = 4;
	char *ptype;
	int pnum = 0;
	struct mtd_partition *mtd_parts;
	struct nand_chip *this = (struct nand_chip *)rtk_mtd->priv;
	int retRba=0;

#if 0
#ifdef CONFIG_MTD_NAND_RTK_RBA_PERCENT
	this->RBA_PERCENT = CONFIG_MTD_NAND_RTK_RBA_PERCENT;
	printk("[Customize]RBA percentage: %d%\n",this->RBA_PERCENT);

#else
	this->RBA_PERCENT = RBA_PERCENT;
	printk("[Default]RBA percentage: %d'%'\n",this->RBA_PERCENT);
#endif
//#else
		retRba = rtkNF_getRBAPercent();
	if(retRba == -1)
	{
		rtk_mtd->u32RBApercentage = this->RBA_PERCENT = RBA_PERCENT;
		printk("[**Default**]RBA percentage: %d%\n",this->RBA_PERCENT);
	}
	else
	{
		rtk_mtd->u32RBApercentage = this->RBA_PERCENT = retRba;
		printk("[**Customize**]RBA percentage: %d%\n",this->RBA_PERCENT);
	}
#endif

	if (rtk_nand_scan (rtk_mtd, maxchips) < 0 || rtk_mtd->size == 0){
		printk("%s: Error, cannot do nand_scan(on-board)\n", __FUNCTION__);
		return -ENODEV;
	}
	if ( !(rtk_mtd->oobblock&(0x200-1)) )
		REG_WRITE_U32( REG_PAGE_LEN,rtk_mtd->oobblock >> 9);
	else{
		printk("Error: pagesize is not 512B Multiple");
		return -1;
	}
#if 0 //stevie
#ifdef CONFIG_MTD_CMDLINE_PARTS
	ptype = (char *)ptypes[0];
	pnum = parse_mtd_partitions (rtk_mtd, ptypes, &mtd_parts, NULL);
#endif
	if (pnum <= 0) {
		printk(KERN_NOTICE "RTK: using the whole nand as a partitoin\n");
		if(add_mtd_device(rtk_mtd)) {
			printk(KERN_WARNING "RTK: Failed to register new nand device\n");
			return -EAGAIN;
		}
	}else{
		printk(KERN_NOTICE "RTK: using dynamic nand partition\n");
		if (add_mtd_partitions (rtk_mtd, mtd_parts, pnum)) {
			printk("%s: Error, cannot add %s device\n",
					__FUNCTION__, rtk_mtd->name);
			rtk_mtd->size = 0;
			return -EAGAIN;
		}
	}
#endif
	return 0;
}

unsigned int rtkNF_getBootcodeSize(void)
{
    unsigned char *param;
	unsigned int retBootcodeSize = -1 ;
    //param=(char *)rtkNF_parse_token(platform_info.system_parameters,"nand_boot_size");

    if(param)
		retBootcodeSize = simple_strtoul(param, (char**)&retBootcodeSize, 16);
    return retBootcodeSize;
}
//----------------------------------------------------------------------------
int is_jupiter_cpu(void)
{
	return 0;
}
//----------------------------------------------------------------------------
int is_saturn_cpu(void)
{
	return 1;
}

//----------------------------------------------------------------------------
int is_darwin_cpu(void)
{
	return 0;
}

//----------------------------------------------------------------------------

int is_macarthur_cpu(void)
{
	return 0;
}

//----------------------------------------------------------------------------
int is_nike_cpu(void)
{
	return 0;
}

//----------------------------------------------------------------------------
int is_venus_cpu(void)
{
	return 0;
}

//----------------------------------------------------------------------------
int is_neptune_cpu(void)
{
	return 0;
}

//----------------------------------------------------------------------------
int is_mars_cpu(void)
{
	return 0;
}
//----------------------------------------------------------------------------

void WAIT_DONE(unsigned int addr, unsigned int mask, unsigned int value)
{
        int timeout = 0;

	while ( (REG_READ_U32(addr) & mask) != value )
	{

                if(timeout++>100000){
                    printk("[%s]timeout..%d\n",__FUNCTION__, timeout);
                    return;
                 }
    
		asm("nop");
	}
}

//----------------------------------------------------------------------------
int  rtk_nand_init (void)
{
	struct nand_chip *this=NULL;
	int rc = 0;
	//unsigned long long pin_WP_info;
	//if ( is_venus_cpu() || is_neptune_cpu()||is_mars_cpu())
	//	return -1;
#if 0 //stevie
	if ( REG_READ_U32(REG_CLOCK_ENABLE1) & 0x00800000 ){
		display_version();
	}else{
		printk(KERN_ERR "Nand Flash Clock is NOT Open. Installing fails.\n");
		return -1;
	}
#endif

#if 1//defined(SOC_TYPE_8196F)
		*(volatile unsigned long *)(0xb8000014) = ((*(volatile unsigned long *)(0xb8000014)) & 0xFFBFFFFF) | (0x1 << 21);
		//pinmux
		*(volatile unsigned long *)(0xb8000800) = 0x81110000;
		*(volatile unsigned long *)(0xb8000804) = 0x12220000;
		*(volatile unsigned long *)(0xb8000808) = 0x1121000;
		*(volatile unsigned long *)(0xb800083c) = 0x44440000;
#endif

    //if(is_macarthur_cpu())
    //{
    //        if((REG_READ_U32(0xb8060008)&0x01)!=1)
    //        {
    //                printk("[%s]Doesn't support NAND boot for macathur\n",__FUNCTION__);
    //                return -1;
    //        }
    //}
#if 0
	#if RTK_NAND_INTERRUPT
		//if(is_darwin_cpu()||is_macarthur_cpu()||is_nike_cpu())
		//{
		//	printk("Enable INT for nand flash driver...\n");
		//	REG_WRITE_U32(0xb801a640,(REG_READ_U32(0xb801a640)|0x8));//Enable INT of NAND flash
		//}
		printk("(V)Enable interrupt mechanism of NAND driver.[1011-2011 14:30]\n");
		platform_driver_probe(&rtkNF_device, rtkNF_probe);
		REG_WRITE_U32(REG_NAND_ISREN,(NF_IRQ_MASK|0x01));
	#else
		printk("(X)Enable interrupt mechanism of NAND driver.\n");
	#endif

	g_BootcodeSize = -1;//rtkNF_getBootcodeSize(); //stevie
	g_Factory_param_size = -1;//rtkNF_getFactParaSize(); //stevie

	if(g_BootcodeSize == -1)
	{
		if(is_darwin_cpu()||is_saturn_cpu()||is_macarthur_cpu()||is_nike_cpu())
			g_BootcodeSize = 12*1024*1024;
		else
			g_BootcodeSize = 16*1024*1024;
	}
	if(g_Factory_param_size == -1)
		g_Factory_param_size = 4*1024*1024;

	//g_WP_en = rtkNF_getWPSettingval();
	#if 0 //stevie
	if(is_darwin_cpu()||is_macarthur_cpu()||is_nike_cpu())
	{
		g_regGPIODirBase = 0x1801bc00;
		g_regGPIOOutBase = 0x1801bc14;

		#if  defined(CONFIG_REALTEK_PCBMGR)
		if(pcb_mgr_get_enum_info_byname("PIN_FLASH_WP",&pin_WP_info)!=0)
		{
			g_WP_en = 0;
			printk("PIN_FLASH_WP is not defined\n");
		}
		else
		{
			 g_WP_en = 1;
			printk("PIN_FLASH_WP defined value 0x%llx\n",pin_WP_info);

			g_NFWP_no = ((unsigned char) ((pin_WP_info >> 8) & 0xFF));
			g_NFWP_value_INV = ((unsigned char) ((pin_WP_info >> 24) & 0xFF));
			if(is_darwin_cpu())
			{
				if(g_NFWP_value_INV==0)
				{
					g_NFWP_value_en = 1;
					g_NFWP_value_dis = 0;
				}
				else
				{
					g_NFWP_value_en = 0;
					g_NFWP_value_dis = 1;
				}

			}
			else
			{
				if(g_NFWP_value_INV==0)
				{
					g_NFWP_value_en = 0;
					g_NFWP_value_dis = 1;
				}
				else
				{
					g_NFWP_value_en = 1;
					g_NFWP_value_dis = 0;
				}
			}
		}
		#else
		 printk("Force disable FLASH_WP!!\n");
		 g_WP_en=0;
		#endif

		printk("g_NFWP_no = %d\n",g_NFWP_no);
		printk("g_NFWP_value_en = %d\n",g_NFWP_value_en);
		printk("g_NFWP_value_dis = %d\n",g_NFWP_value_dis);

	}
	#endif
	//nRun = nInc = 0;
	printk("Get parameter from register...\n");
	printk("[%s]Bootcode size 0x%x\n",__FUNCTION__,g_BootcodeSize);
	printk("[%s]Factory parameter size 0x%x\n",__FUNCTION__,g_Factory_param_size);
#endif

#if	RTK_ARD_ALGORITHM
 	g_u32WinStart = g_RecBlkStart;
 	g_u32WinEnd = g_u32WinStart+g_PorcWindowSize-1;
	g_WinTrigCnt = 0;
	//printk("Start ARD Algorithm Test : ");
	//printk("\n");
#endif

#if 0
	if(platform_info.secure_boot)
	{
		memcpy(&u8regKey0,&platform_info.AES_IMG_KEY[0],4);
		memcpy(&u8regKey1,&platform_info.AES_IMG_KEY[4],4);
		memcpy(&u8regKey2,&platform_info.AES_IMG_KEY[8],4);
		memcpy(&u8regKey3,&platform_info.AES_IMG_KEY[12],4);
        regKey0 = (unsigned int*)&u8regKey0;
		regKey1 = (unsigned int*)&u8regKey1;
		regKey2 = (unsigned int*)&u8regKey2;
		regKey3 = (unsigned int*)&u8regKey3;
		endian_swap(regKey0);
		endian_swap(regKey1);
		endian_swap(regKey2);
		endian_swap(regKey3);

		printk("[%s]Enable NFCP_",__FUNCTION__);
		is_NF_CP_Enable_read = 1;
		is_NF_CP_Enable_write = 0;//When secure boot enable, disable write encryption.

		cpSel = 0;//0:register, 1:CW, 2:E-Fuse

		u32CP_mode = 0x200;//0 for CBC mode, 0x200 for ECB mode
		if(u32CP_mode == 0)
			printk("CBC.\n");
		else if(u32CP_mode == 0x200)
			printk("ECB.\n");
		else
			printk("Unknown.\n");

	}
	else

	{
		printk("[%s]Disable NFCP...\n",__FUNCTION__);
		is_NF_CP_Enable_write = is_NF_CP_Enable_read = 0;
	}
#endif

#if 0 //stevie
	//REG_WRITE_U32( 0xb801032c,0x01);	//Enable NAND/CardReader arbitrator
	REG_WRITE_U32( REG_CLOCK_ENABLE1,REG_READ_U32(REG_CLOCK_ENABLE1)& (~0x00800000));
	
	//if (is_jupiter_cpu())
	//	REG_WRITE_U32( REG_NF_CKSEL,0x02 );
	//else if(is_saturn_cpu()||is_darwin_cpu()||is_macarthur_cpu()||is_nike_cpu())
	//	REG_WRITE_U32( REG_NF_CKSEL,0x02 );
	REG_WRITE_U32( REG_NF_CKSEL,0x04 );

	REG_WRITE_U32( REG_CLOCK_ENABLE1,REG_READ_U32(REG_CLOCK_ENABLE1)| (0x00800000));
#endif
	//if ( is_jupiter_cpu()||is_macarthur_cpu()||is_nike_cpu() )
	//	REG_WRITE_U32( REG_SPR_DDR_CTL,0x7<<26);
	//else
	//	REG_WRITE_U32(REG_SPR_DDR_CTL,NF_SPR_DDR_CTL_cr_nf_hw_pinmux_ena(1) | NF_SPR_DDR_CTL_spare_ddr_ena(1) | NF_SPR_DDR_CTL_per_2k_spr_ena(1)| NF_SPR_DDR_CTL_spare_dram_sa(0)); //set spare2ddr func. 4=>0.5k spe2ddr_ena at A000F000
	REG_WRITE_U32(REG_SPR_DDR_CTL, NF_SPR_DDR_CTL_spare_ddr_ena(1) | NF_SPR_DDR_CTL_per_2k_spr_ena(1)| NF_SPR_DDR_CTL_spare_dram_sa(0)); //set spare2ddr func. 4=>0.5k spe2ddr_ena at A000F000

	rtk_mtd = kmalloc (MTDSIZE, GFP_KERNEL);
	if ( !rtk_mtd ){
		printk("%s: Error, no enough memory for rtk_mtd\n",__FUNCTION__);
		rc = -ENOMEM;
		goto EXIT;
	}
	memset ( (char *)rtk_mtd, 0, MTDSIZE);
	rtk_mtd->priv = this = (struct nand_chip *)(rtk_mtd+1);
#if 1
	REG_WRITE_U32( REG_PD,~(0x1 << 0) );
	REG_WRITE_U32( REG_ND_CMD,0xFF/*CMD_RESET*/ ); //stevie

	//if(is_jupiter_cpu()||is_macarthur_cpu()||is_nike_cpu())
	//	REG_WRITE_U32( REG_ND_CTL,(0x80 | 0x00));
	//else
	//	REG_WRITE_U32( REG_ND_CTL,(NF_ND_CTL_xfer(0x01) | NF_ND_CTL_tran_mode(0)) );
	REG_WRITE_U32( REG_ND_CTL,(NF_ND_CTL_xfer(0x01) | NF_ND_CTL_tran_mode(0)) );

	WAIT_DONE(REG_ND_CTL,0x80,0);
	WAIT_DONE(REG_ND_CTL,0x40,0x40);

	REG_WRITE_U32( REG_MULTI_CHNL_MODE,0x00);
	REG_WRITE_U32( REG_TIME_PARA1,NF_TIME_PARA1_T1(0x01));
	REG_WRITE_U32( REG_TIME_PARA2,NF_TIME_PARA2_T2(0x01));
	REG_WRITE_U32( REG_TIME_PARA3,NF_TIME_PARA3_T3(0x01));

	//if(!is_jupiter_cpu()||!is_saturn_cpu()||!is_darwin_cpu()||!is_macarthur_cpu()||is_nike_cpu())
	//	REG_WRITE_U32( REG_DELAY_CTL,NF_DELAY_CTL_T_WHR_ADL(0x09) );
	REG_WRITE_U32( REG_DELAY_CTL,NF_DELAY_CTL_T_WHR_ADL(0x09) );

	REG_WRITE_U32( REG_MULTI_CHNL_MODE,NF_MULTI_CHNL_MODE_edo(0x00));
	REG_WRITE_U32( REG_ECC_STOP,NF_ECC_STOP_ecc_n_stop(0x01));

	REG_WRITE_U32( REG_ND_CA0,0);
	REG_WRITE_U32( REG_ND_CA1,0);
#endif
	this->read_id		= rtk_nand_read_id;
	this->read_ecc_page 	= NULL;//rtk_read_ecc_page;
	this->read_oob 		= rtk_read_oob;
	this->write_ecc_page	= NULL;//rtk_write_ecc_page;
	this->write_oob		= rtk_write_oob;

	this->erase_block 	= rtk_erase_block;
	//this->suspend		= rtk_nand_suspend; //stevie
	//this->resume		= rtk_nand_resume;  //stevie
	this->read_parameter   = rtk_read_parameter_page; //stevie
	this->sync		= NULL;
	
	//unsigned char id[512];
	//rtk_nand_read_id2(rtk_mtd, id);
	//printk("[XXXXXXXXXXXX]read id : 0x%x %x %x %x \n", id[0], id[1],id[2],id[3]);

	if( rtk_nand_profile() < 0 ){
		rc = -1;
		goto EXIT;
	}
	#if 0
	//rtk_read_ecc_page(rtk_mtd, 0, 0, this->g_databuf, this->g_oobbuf);
	if(g_isRandomize)
	{
		printk("[%s]Randomized enabled \n ",__FUNCTION__);
		cpSel = 0;//0:register, 1:CW, 2:E-Fuse
        u32CP_mode = 0x0;//0 for CBC mode, 0x200 for ECB mode
	}
	#endif


	g_NF_pageSize = page_size = rtk_mtd->oobblock;
	oob_size = rtk_mtd->oobsize;
	ppb = (rtk_mtd->erasesize)/(rtk_mtd->oobblock);

	dbg_parallel_nand("[%s]Ecc bit select %u\n",__FUNCTION__,this->ecc_select);
	//create_proc_read_entry("nandinfo", 0, NULL, rtk_read_proc_nandinfo, NULL);  //stevie

EXIT:
	if (rc < 0) {
		if (rtk_mtd){
			//del_mtd_partitions (rtk_mtd);
			#ifndef RTK_ROM_CODE
			if (this->g_databuf)
				kfree(this->g_databuf);
			if(this->g_oobbuf)
				kfree(this->g_oobbuf);
			#endif
			if (this->erase_page_flag){
				unsigned int flag_size =  (this->numchips * this->page_num) >> 3;
				//unsigned int mempage_order = get_order(flag_size);
				//free_pages((unsigned long)this->erase_page_flag, mempage_order);
			}
			kfree(rtk_mtd);
		}
		//remove_proc_entry("nandinfo", NULL);
	}else
		printk(KERN_INFO "Realtek Nand Flash Driver is successfully installing.\n");

	return rc;
}

VOID rtk_nand_set_monflg(IN  VOID *Data)
{
	u32*  InPutData;
	InPutData = (u32*)Data;

	monflg = InPutData[0];
	g_ecc_select = InPutData[1];
	printk("monflg = %d, g_ecc_select = %d \n", monflg, g_ecc_select);
	return;

}

static void rtk_nand_auto_mode_test(u32 test_mode)
{
	u32 id[512];
	int i,page;
	u16 chipnr;
	//u_char *nand_write_buf_rtk;
	//u_char *nand_read_buf;
	struct nand_chip *this ;
	printk("Entry %s \n", __FUNCTION__);
	switch(test_mode)
	{
		case 0:
			#if 0
			page = monflg;
			chipnr = 0;
			//fill data into write buffer
			for(i=0; i < page_size; i++)
			{
				nand_write_buf_rtk[i] = (u8)(i & 0xff);
				/*
				if(!(i %16))
					printf("\n");
				printf(" %02x",  nand_write_buf_rtk[i]);
				*/
			}

			if(rtk_mtd != NULL)
			{
				this = (struct nand_chip *)rtk_mtd->priv;
				rtk_write_ecc_512(rtk_mtd, chipnr,  page, nand_write_buf_rtk, this->g_oobbuf);
			}
			#endif
			break;
		case 1:
			//read on page 
			printk("[nand test] read %s, %d \n",__FUNCTION__,__LINE__);
			page =  monflg;
			chipnr = 0;
			if ( !this->g_databuf ){
				printk("%s: Error, no enough memory for nand_read_buf\n",__FUNCTION__);
				return ;
			}
			printk("[nand test] read %s, %d \n",__FUNCTION__,__LINE__);

			if(rtk_mtd != NULL)
			{
				this = (struct nand_chip *)rtk_mtd->priv;
				//this->g_databuf = nand_read_buf;
				//clean buffer
				for(i=0; i < page_size; i ++)
				{
					this->g_databuf[i] = 0;
				}
				
				printk("[nand test] read %s, %d \n",__FUNCTION__,__LINE__);
				rtk_read_ecc_512(rtk_mtd, chipnr, page, this->g_databuf, this->g_oobbuf);
			}
			//print data buffer
			for(i=0; i < page_size; i ++)
			{
				if(!(i %16))
					printf("\n");
				printf(" %02x",  this->g_databuf[i]);
			}

			break;
		case 2:
			rtk_nand_read_id2(rtk_mtd,id);
			break;
		case 3:
			rtk_nand_auto_mode_case_2();
			break;
		case 4:
			page = monflg;
			chipnr = 0;
			//fill data into write buffer
			for(i=0; i < page_size; i++)
			{
				nand_write_buf_rtk[i] = (u8)(i & 0xff);
				/*
				if(!(i %16))
					printf("\n");
				printf(" %02x",  nand_write_buf_rtk[i]);
				*/
			}

			if(rtk_mtd != NULL)
			{
				this = (struct nand_chip *)rtk_mtd->priv;
				rtk_nand_auto_mode_case_3(rtk_mtd, chipnr,  page, nand_write_buf_rtk, this->g_oobbuf);
			}
			break;
		default:
			break;
	}
}

static void rtk_nand_open_ecc_write(u32 page)
{
	printk("[nand test] %s,page %d \n",__FUNCTION__,page);
	//page = InPutData[1];
	int xfer_size, ecc_select, chipnr, i, check_sum;
	struct nand_chip *this ;
	ecc_select = g_ecc_select;
	if(ecc_select >= 0x18)
		xfer_size = 1024;
	else
		xfer_size = 512;
	chipnr= check_sum = 0;
	//fill data into write buffer

	for(i=0; i < xfer_size; i++)
	{
		nand_write_buf_rtk[i] = (u8)(i&0xff);
		check_sum += (u32)nand_write_buf_rtk[i];
	}
	if(rtk_mtd != NULL)
	{
		this = (struct nand_chip *)rtk_mtd->priv;
		rtk_write_diff_ecc(rtk_mtd, chipnr,  page,0, nand_write_buf_rtk, this->g_oobbuf, ecc_select);
	}
	printk("[nand test]  %s, %d, checksum = %d \n\n",__FUNCTION__,__LINE__, check_sum);
	//if(monflg == 5566)
	//	rtk_nand_printf_pp_buffer(4);
	return;

}

static void rtk_nand_close_ecc_read(u32 page)
{
	//read on page 
	printk("[nand test] %s,page : %d \n",__FUNCTION__,page);
	//page = InPutData[1];
	int chipnr, ecc_select, xfer_size, i, montemp, check_sum;
	struct nand_chip *this ;
	u8* read_buf;
	chipnr = check_sum = 0;
	ecc_select = g_ecc_select;
	if(ecc_select >= 0x18)
		xfer_size = 1024;
	else
		xfer_size = 512;
	if(rtk_mtd != NULL)
	{
		this = (struct nand_chip *)rtk_mtd->priv;
		if ( !this->g_databuf ){
			printk("%s: Error, no enough memory for nand_read_buf\n",__FUNCTION__);
			return ;
		}
		#ifdef NAND_BUFFER_SRAM
		read_buf = nand_read_buf_rtk;
		#else
		read_buf = this->g_databuf;
		#endif

		//clean buffer
		for(i=0; i < page_size; i ++)
		{
			read_buf[i] = 0;
		}		
		montemp = monflg;
		monflg = 5566;  // close ecc
		rtk_read_diff_ecc(rtk_mtd, chipnr, page,0, read_buf, this->g_oobbuf, ecc_select);
		monflg = montemp;
	}
	//print data buffer
	
	for(i=0; i < xfer_size; i ++)
	{
		//if(!(i %16))
		//	printf("\n");
		//printf(" %02x",  read_buf[i]);
		check_sum += (u32)read_buf[i];

	}
	
	printk("\n[nand test]  %s, %d, checksum = %d \n\n",__FUNCTION__,__LINE__, check_sum);
	return;
}

static void rtk_nand_close_ecc_write(u32 page)
{
	printk("[nand test] %s,page %d \n",__FUNCTION__,page);
	//page = InPutData[1];
	int xfer_size, ecc_select, chipnr, i, check_sum, montemp;
	ecc_select = g_ecc_select;
	struct nand_chip *this ;
	if(ecc_select >= 0x18)
		xfer_size = 1024;
	else
		xfer_size = 512;
	chipnr= check_sum = 0;
	//fill data into write buffer

	for(i=0; i < xfer_size; i++)
	{
		nand_write_buf_rtk[i] = (u8)(i&0xff);
		check_sum += (u32)nand_write_buf_rtk[i];
	}
	montemp = monflg;
	monflg = 5566; //close ecc
#if 1
	if(monflg == 5566)
	{
		printk("[nand test] ecc check write insertion error %d bits\n", ecc_select);
		switch(ecc_select)
		{
			case 6:
				nand_write_buf_rtk[0] = 0xf; // 4bit
				nand_write_buf_rtk[1] = 0x0; // 1 bit
				nand_write_buf_rtk[2] = 0x0; // 1 bit
				break;
			case 12:
				nand_write_buf_rtk[0] = 0xff; // 8 bit
				nand_write_buf_rtk[1] = 0x0f; // 3 bit 
				nand_write_buf_rtk[2] = 0x0;  // 1 bit
				break;
			case 24:
				nand_write_buf_rtk[0] = 0xff; // 8 bit
				nand_write_buf_rtk[1] = 0xfe; // 8 bit 
				nand_write_buf_rtk[2] = 0xfd; // 8 bit
				break;
			case 40:
				nand_write_buf_rtk[0] = 0xff; // 8 bit
				nand_write_buf_rtk[1] = 0xfe; // 8 bit 
				nand_write_buf_rtk[2] = 0xfd; // 8 bit
				nand_write_buf_rtk[3] = 0xfc; // 8 bit
				nand_write_buf_rtk[4] = 0xfb; // 8 bit
				break;
			case 43:
				nand_write_buf_rtk[0] = 0xff; // 8 bit
				nand_write_buf_rtk[1] = 0xfe; // 8 bit 
				nand_write_buf_rtk[2] = 0xfd; // 8 bit
				nand_write_buf_rtk[3] = 0xfc; // 8 bit
				nand_write_buf_rtk[4] = 0xfb; // 8 bit
				nand_write_buf_rtk[5] = 0x75; // 3 bit
				break;
			case 65:
				/*
				nand_write_buf_rtk[0] = 0xff; // 8 bit
				nand_write_buf_rtk[0x10] = 0xEF; // 8 bit 
				nand_write_buf_rtk[0x20] = 0xDF; // 8 bit
				nand_write_buf_rtk[0x30] = 0xCF; // 8 bit
				nand_write_buf_rtk[0x40] = 0xBF; // 8 bit
				nand_write_buf_rtk[0x50] = 0xAF; // 8 bit						
				nand_write_buf_rtk[0x60] = 0x9F; // 8 bit
				nand_write_buf_rtk[0x70] = 0x8F; // 8 bit
				//nand_write_buf_rtk[8] = 0x09; // 1 bit
				*/
				for(i = 0; i < 63; i++) // the ecc code will be rewrite 2 bit , so we can only insert 63 error, or you may rewrite pp buffer to fix ecc code
				{
					nand_write_buf_rtk[i*8] ^=0x1;
				}
				
				break;
			case 72:
				nand_write_buf_rtk[0] = 0xff; // 8 bit
				nand_write_buf_rtk[1] = 0xfe; // 8 bit 
				nand_write_buf_rtk[2] = 0xfd; // 8 bit
				nand_write_buf_rtk[3] = 0xfc; // 8 bit
				nand_write_buf_rtk[4] = 0xfb; // 8 bit
				nand_write_buf_rtk[5] = 0xfa; // 8 bit						
				nand_write_buf_rtk[6] = 0xf9; // 8 bit
				nand_write_buf_rtk[7] = 0xf8; // 8 bit
				nand_write_buf_rtk[8] = 0xf7; // 8 bit
				break;					
			default :
				break;
		}
	}
#endif
	if(rtk_mtd != NULL)
	{
		this = (struct nand_chip *)rtk_mtd->priv;
		rtk_write_diff_ecc(rtk_mtd, chipnr,  page,0, nand_write_buf_rtk, this->g_oobbuf, ecc_select);
	}
	monflg = montemp;
	printk("[nand test]  %s, %d, checksum = %d \n",__FUNCTION__,__LINE__, check_sum);
	return;

}

static void rtk_nand_open_ecc_read(u32 page)
{
	//read on page 
	printk("[nand test] %s,page : %d \n",__FUNCTION__,page);
	//page = InPutData[1];
	int chipnr, ecc_select, xfer_size, i, montemp, check_sum;
	u8* read_buf;
	struct nand_chip *this ;
	chipnr = check_sum = 0;
	ecc_select = g_ecc_select;
	if(ecc_select >= 0x18)
		xfer_size = 1024;
	else
		xfer_size = 512;
	if(rtk_mtd != NULL)
	{
		this = (struct nand_chip *)rtk_mtd->priv;
		if ( !this->g_databuf ){
			printk("%s: Error, no enough memory for nand_read_buf\n",__FUNCTION__);
			return ;
		}
	#ifdef NAND_BUFFER_SRAM
		read_buf = nand_read_buf_rtk;
	#else
		read_buf = this->g_databuf;
	#endif

		//clean buffer
		for(i=0; i < page_size; i ++)
		{
			read_buf[i] = 0;
		}		
		rtk_read_diff_ecc(rtk_mtd, chipnr, page,0, read_buf, this->g_oobbuf, ecc_select);
	}
	for(i=0; i < xfer_size; i++)
	{
		nand_write_buf_rtk[i] = (u8)(i&0xff);
		check_sum += (u32)nand_write_buf_rtk[i];
	}

	//print data buffer
	for(i=0; i < xfer_size; i ++)
	{
		if(read_buf[i] != nand_write_buf_rtk[i])
		{
			printf(" error read_buf  != write_buf (%x, %x) \n",  read_buf[i], nand_write_buf_rtk[i]);
			return;
		}
		check_sum += (u32)read_buf[i];

	}
	printk("\nECC function test pass, correct ecc : %d \n", REG_READ_U32(REG_MAX_ECC_NUM) & 0xFF);
	return;
}

static void rtk_nand_open_randomizer_ecc_write(u32 page)
{
	//randomizer seed ecc verify
	int chipnr, i, montemp;	
	struct nand_chip *this ;
	chipnr = 0;
	for(i=0; i < page_size; i ++)
	{
		//this->g_databuf[i] = 0;
		nand_write_buf_rtk[i] = i&0xff;
	}
	if(rtk_mtd != NULL)
	{
		this = (struct nand_chip *)rtk_mtd->priv;
		montemp = monflg;
		monflg = 6789; // open randomizer write
		rtk_write_diff_ecc(rtk_mtd, chipnr,  page, 510, nand_write_buf_rtk, this->g_oobbuf, 0);
		monflg = montemp;
	}
	return;
}

static void rtk_nand_close_randomizer_ecc_write(u32 page)
{
	
	int chipnr, i, montemp;
	struct nand_chip *this ;
	chipnr = 0;
	for(i=0; i < page_size; i ++)
	{
		//this->g_databuf[i] = 0;
		nand_write_buf_rtk[i] = 0;
	}
	if(rtk_mtd != NULL)
	{
		this = (struct nand_chip *)rtk_mtd->priv;
		montemp = monflg;
		monflg = 5566; // open randomizer write
		rtk_write_diff_ecc(rtk_mtd, chipnr,  page, 0, nand_write_buf_rtk, this->g_oobbuf, 0);
		monflg = montemp;
	}
	return;
}
#define REG_PAD_CTRL_4 0xb800085c
#define REG_PAD_CTRL_8 0xb800086c

#define GPIO_PABCD_CNR 0xb8003500
#define GPIO_PABCD_DIR 0xb8003508
#define GPIO_PABCD_DAT 0xb800350c

static void WP_HIGH(void)
{
	REG_WRITE_U32(GPIO_PABCD_CNR, (REG_READ_U32(GPIO_PABCD_CNR) & 0xFFFFBFFF) | (0<<14)); //set GPIOB[6] as GPIO pin
	REG_WRITE_U32(GPIO_PABCD_DIR, (REG_READ_U32(GPIO_PABCD_DIR) & 0xFFFFBFFF) | (1<<14)); //set as output pin
	REG_WRITE_U32(GPIO_PABCD_DAT, (REG_READ_U32(GPIO_PABCD_DAT) & 0xFFFFBFFF) | (1<<14)); //set high
}

static void WP_LOW(void)
{
	REG_WRITE_U32(GPIO_PABCD_CNR, (REG_READ_U32(GPIO_PABCD_CNR) & 0xFFFFBFFF) | (0<<14)); //set GPIOB[6] as GPIO pin
	REG_WRITE_U32(GPIO_PABCD_DIR, (REG_READ_U32(GPIO_PABCD_DIR) & 0xFFFFBFFF) | (1<<14)); //set as output pin
	REG_WRITE_U32(GPIO_PABCD_DAT, (REG_READ_U32(GPIO_PABCD_DAT) & 0xFFFFBFFF) | (0<<14)); //set low

}

static void rtk_nand_open_randomizer_ecc_read(u32 page)
{
	int chipnr, i, montemp, check_sum;
	struct nand_chip *this ;
	u8* read_buf;
	chipnr = check_sum = 0;
	if(rtk_mtd != NULL)
	{
		this = (struct nand_chip *)rtk_mtd->priv;
		#ifdef NAND_BUFFER_SRAM
		read_buf = nand_read_buf_rtk;
		#else
		read_buf = this->g_databuf;
		#endif

	}

	for(i=0; i < page_size; i ++)
	{
		//this->g_databuf[i] = 0;
		read_buf[i] = 0;
	}
	if(rtk_mtd != NULL)
	{
		this = (struct nand_chip *)rtk_mtd->priv;
		montemp = monflg;
		monflg = 6789; // open randomizer write
		rtk_read_diff_ecc(rtk_mtd, chipnr, page, 510, read_buf, this->g_oobbuf, 0);
		monflg = montemp;
	}
	for(i=0; i < 512; i ++)
	{
		if(!(i %16))
			printf("\n");
		//printf(" %02x",  this->g_databuf[i]);
		//check_sum += (u32)this->g_databuf[i];
		printf(" %02x",  read_buf[i]);
		check_sum += (u32)read_buf[i];
	
	}
	//kfree(nand_read_buf);
	printk("\n[nand test]  %s, %d, checksum = %d \n",__FUNCTION__,__LINE__, check_sum);
	printk("\n[nand test]  ECC correct = %d,RMZ_ECC_NUM : %d  \n",REG_READ_U32(REG_MAX_ECC_NUM) & 0xff, REG_READ_U32(REG_RMZ_ECC_NUM) & 0x7f);
	return;
}

VOID rtk_nand_test(IN  VOID *Data)
{
	u32*  InPutData;
	InPutData = (u32*)Data;
	int i,page_tmp,page, ecc_select, xfer_size;
	u16 chipnr;
	unsigned char id[512];
	u8* read_buf;
	//struct timeval start_time;
	//struct timeval end_time;
	u32  wr_exe_time;
	u32 data_size;
	u32 data_block_size, data_page_size;
	unsigned char para[256];


	u32 check_sum = 0;
	struct nand_chip *this ;
	switch(InPutData[0])
	{
		case 0:
			//CAUTION :  init memory and parameter , make sure this command have been excuted before any others
			rtk_nand_init();
			break;
		case 1:
			//write
			printk("[nand test] write %s, %d \n",__FUNCTION__,__LINE__);
			page = InPutData[1];
			chipnr= check_sum = 0;
			
			if ( !nand_write_buf_rtk ){
				printk("%s: Not alloc write buffer for NAND\n",__FUNCTION__);
				return ;
			}
			for(i=0; i < page_size; i++) //fill data into write buffer
			{
				nand_write_buf_rtk[i] = (u8)(i & 0xff);
				check_sum += (u32)nand_write_buf_rtk[i];
				/*
				if(!(i %16))
					printf("\n");
				printf(" %02x",  nand_write_buf_rtk[i]);
				*/
			}
			#ifdef SPECIAL_TEST_FOR_65BIT_ECC
			//JUST FOR ECC 65 
			for(i = 0; i < 65; i++)
			{
				nand_write_buf_rtk[i*8] ^=0x1;
			}
			for(i = 0; i < (page_size -1024); i ++)
			{
				if(i <= 114)
					nand_write_buf_rtk[1024+i] = ecc_65[i];
				else
					nand_write_buf_rtk[1024+i] = 0xFF;
			}
			#endif
			if(rtk_mtd != NULL)
			{
				WP_HIGH();
				this = (struct nand_chip *)rtk_mtd->priv;
				rtk_write_ecc_page(rtk_mtd, chipnr,  page, nand_write_buf_rtk, this->g_oobbuf);
				WP_LOW();
			}
			printk("[nand test]  %s, %d, checksum = %d \n",__FUNCTION__,__LINE__, check_sum);
			break;
		case 2:
			//read one page 
			printk("[nand test] read %s, %d \n",__FUNCTION__,__LINE__);
			page = InPutData[1];
			chipnr = check_sum = 0;

			if(rtk_mtd != NULL)
			{
				this = (struct nand_chip *)rtk_mtd->priv;
				if ( !this->g_databuf ){
					printk("%s: Error, no enough memory for nand_read_buf\n",__FUNCTION__);
					return ;
				}
				#ifdef NAND_BUFFER_SRAM
				read_buf = nand_read_buf_rtk;
				#else
				read_buf = this->g_databuf;
				#endif
				
				//clean buffer
				for(i=0; i < page_size; i ++)
				{					
					read_buf[i] = 0;
				}				
				rtk_read_ecc_page(rtk_mtd, chipnr, page, read_buf, this->g_oobbuf);		
						
			}
			//print data buffer
			for(i=0; i < page_size; i ++)
			{
				if(monflg != 1234)
				{
					if(!(i %16))
						printf("\n");
					printf(" %02x",  read_buf[i]);
				}
				else
				{
					if(read_buf[i] != read_buf[i])
						printk("[nand test] read error byte %d, read : 0x%x, write : 0x%x \n", i,(u32)read_buf[i], (u32)nand_write_buf_rtk[i]);
				}
				check_sum += (u32)read_buf[i];
			}
			//kfree(nand_read_buf);
			printk("\n[nand test]  %s, %d, checksum = %d \n",__FUNCTION__,__LINE__, check_sum);
			break;
		case 3:
			//erase
			chipnr = 0;
			page = InPutData[1];
			if(rtk_mtd != NULL)
			{
				rtk_erase_block(rtk_mtd, chipnr, page);
			}
			break;
		case 4:
			//rtk_nand_reset();
			//rtk_nand_read_id(rtk_mtd, id);
			//printk("read id : 0x%x %x %x %x \n", id[0], id[1],id[2],id[3]);
			
#if 1
			
			rtk_read_parameter_page(rtk_mtd, para);		
	//		this->read_parameter(mtd, para);		
	//		this->read_parameter(mtd, para);
			
			printk("[Phoenix]Number of data bytes per page:0x%02x %02x %02x %02x\n",para[83],para[82],para[81],para[80]);
			printk("[Phoenix]Number of spare bytes per page:0x%02x %02x\n",para[85],para[84]);
			printk("[Phoenix]Number of pages per block:0x%02x %02x %02x %02x\n",para[95],para[94],para[93],para[92]);
			printk("[Phoenix]Number of blocks per logical unit (LUN):0x%02x %02x %02x %02x\n",para[99],para[98],para[97],para[96]);
			printk("[Phoenix]Number of logical units (LUNs):0x%x\n",para[100]);
			printk("[Phoenix]Number of address cycles:0x%x\n",para[101]);
			printk("[Phoenix]Number of bits per cell:0x%x\n",para[102]);
			printk("[Phoenix]Number of bits ECC correctability:0x%x\n",para[112]);
			//this->read_parameter(mtd, para);
#endif
			break;
		case 5:
			//page = InPutData[1];
			//rtk_nand_auto_mode_test(page);
			break;
		case 6:
			printk("[nand test] ecc check write %s, %d \n",__FUNCTION__,__LINE__);
			page = InPutData[1];
			ecc_select = g_ecc_select;
			if(ecc_select >= 0x18)
				xfer_size = 1024;
			else
				xfer_size = 512;
			chipnr= check_sum = 0;
			//fill data into write buffer
			if ( !nand_write_buf_rtk ){
				printk("%s: Error, no enough memory for nand_write_buf_rtk\n",__FUNCTION__);
				return ;
			}
			for(i=0; i < xfer_size; i++)
			{
				nand_write_buf_rtk[i] = (u8)(i&0xff);
				check_sum += (u32)nand_write_buf_rtk[i];
			}
			if(monflg == 5566)
			{
				printk("[nand test] ecc check write insertion error %d bits\n", ecc_select);
				switch(ecc_select)
				{
					case 6:
						nand_write_buf_rtk[0] = 0xf; // 4bit
						nand_write_buf_rtk[1] = 0x0; // 1 bit
						nand_write_buf_rtk[2] = 0x0; // 1 bit
						break;
					case 12:
						nand_write_buf_rtk[0] = 0xff; // 8 bit
						nand_write_buf_rtk[1] = 0x0f; // 3 bit 
						nand_write_buf_rtk[2] = 0x0;  // 1 bit
						break;
					case 16:
						nand_write_buf_rtk[255] = 0x0; // 8 bit
						nand_write_buf_rtk[1] = 0xff; // 7 bit 
						nand_write_buf_rtk[256] = 0x0;  // 1 bit
						break;
					case 24:
						nand_write_buf_rtk[0] = 0xff; // 8 bit
						nand_write_buf_rtk[1] = 0xfe; // 8 bit 
						nand_write_buf_rtk[2] = 0xfd; // 8 bit
						break;
					case 40:
						nand_write_buf_rtk[0] = 0xff; // 8 bit
						nand_write_buf_rtk[1] = 0xfe; // 8 bit 
						nand_write_buf_rtk[2] = 0xfd; // 8 bit
						nand_write_buf_rtk[3] = 0xfc; // 8 bit
						nand_write_buf_rtk[4] = 0xfb; // 8 bit
						break;
					case 43:
						nand_write_buf_rtk[0] = 0xff; // 8 bit
						nand_write_buf_rtk[1] = 0xfe; // 8 bit 
						nand_write_buf_rtk[2] = 0xfd; // 8 bit
						nand_write_buf_rtk[3] = 0xfc; // 8 bit
						nand_write_buf_rtk[4] = 0xfb; // 8 bit
						nand_write_buf_rtk[5] = 0x75; // 3 bit
						break;
					case 65:
						/*
						nand_write_buf_rtk[0] = 0xff; // 8 bit
						nand_write_buf_rtk[0x10] = 0xEF; // 8 bit 
						nand_write_buf_rtk[0x20] = 0xDF; // 8 bit
						nand_write_buf_rtk[0x30] = 0xCF; // 8 bit
						nand_write_buf_rtk[0x40] = 0xBF; // 8 bit
						nand_write_buf_rtk[0x50] = 0xAF; // 8 bit						
						nand_write_buf_rtk[0x60] = 0x9F; // 8 bit
						nand_write_buf_rtk[0x70] = 0x8F; // 8 bit
						//nand_write_buf_rtk[8] = 0x09; // 1 bit
						*/
						for(i = 0; i < 65; i++)
						{
							nand_write_buf_rtk[i*8] ^=0x1;
						}
						
						break;
					case 72:
						nand_write_buf_rtk[0] = 0xff; // 8 bit
						nand_write_buf_rtk[1] = 0xfe; // 8 bit 
						nand_write_buf_rtk[2] = 0xfd; // 8 bit
						nand_write_buf_rtk[3] = 0xfc; // 8 bit
						nand_write_buf_rtk[4] = 0xfb; // 8 bit
						nand_write_buf_rtk[5] = 0xfa; // 8 bit						
						nand_write_buf_rtk[6] = 0xf9; // 8 bit
						nand_write_buf_rtk[7] = 0xf8; // 8 bit
						nand_write_buf_rtk[8] = 0xf7; // 8 bit
						break;					
					default :
						break;
				}
			}

			if(rtk_mtd != NULL)
			{
				this = (struct nand_chip *)rtk_mtd->priv;
				rtk_write_diff_ecc(rtk_mtd, chipnr,  page,0, nand_write_buf_rtk, this->g_oobbuf, ecc_select);
			}
			printk("[nand test]  %s, %d, checksum = %d \n",__FUNCTION__,__LINE__, check_sum);
			if(monflg == 5566)
				rtk_nand_printf_pp_buffer(4);
			break;
		case 7:
			//read on page 
			printk("[nand test] read by ecc select %s, %d \n",__FUNCTION__,__LINE__);
			page = InPutData[1];
			chipnr = check_sum = 0;
			ecc_select = g_ecc_select;
			if(ecc_select >= 0x18)
				xfer_size = 1024;
			else
				xfer_size = 512;
			if(rtk_mtd != NULL)
			{
				this = (struct nand_chip *)rtk_mtd->priv;
				if ( !this->g_databuf ){
					printk("%s: Error, no enough memory for nand_read_buf\n",__FUNCTION__);
					return ;
				}
				#ifdef NAND_BUFFER_SRAM
				read_buf = nand_read_buf_rtk;
				#else
				read_buf = this->g_databuf;
				#endif

				//clean buffer
				for(i=0; i < page_size; i ++)
				{
					read_buf[i] = 0;
				}				
				rtk_read_diff_ecc(rtk_mtd, chipnr, page,0, read_buf, this->g_oobbuf, ecc_select);
			}
			//print data buffer
			for(i=0; i < xfer_size; i ++)
			{
				if(!(i %16))
					printf("\n");
				printf(" %02x",  read_buf[i]);
				check_sum += (u32)read_buf[i];

			}
			//kfree(nand_read_buf);
			printk("\n[nand test]  %s, %d, checksum = %d \n",__FUNCTION__,__LINE__, check_sum);
			if(monflg != 5566)
			{
				printk("\n[nand test]  ECC correct = %d,RMZ_ECC_NUM : %d  \n",REG_READ_U32(REG_MAX_ECC_NUM) & 0xFF, REG_READ_U32(REG_RMZ_ECC_NUM) & 0x7F);
			}
			break;
		case 8:
			//randomizer seed ecc verify
			page = InPutData[1];
			chipnr = check_sum = 0;
			if(monflg == 5566)
			{
				for(i=0; i < page_size; i ++)
				{
					//this->g_databuf[i] = 0;
					nand_write_buf_rtk[i] = 0;
				}
				if(rtk_mtd != NULL)
				{
					this = (struct nand_chip *)rtk_mtd->priv;
					rtk_write_diff_ecc(rtk_mtd, chipnr,  page, 0, nand_write_buf_rtk, this->g_oobbuf, 0);
				}

			}
			else
			{
				for(i=0; i < page_size; i ++)
				{
					//this->g_databuf[i] = 0;
					nand_write_buf_rtk[i] = i&0xff;
				}
				if(rtk_mtd != NULL)
				{
					this = (struct nand_chip *)rtk_mtd->priv;
					rtk_write_diff_ecc(rtk_mtd, chipnr,  page, 510, nand_write_buf_rtk, this->g_oobbuf, 0);
				}
			}
			
			break;
		case 9:
				//randomizer seed ecc verify
				page = InPutData[1];
				chipnr = check_sum = 0;
				if(rtk_mtd != NULL)
				{
					this = (struct nand_chip *)rtk_mtd->priv;
					#ifdef NAND_BUFFER_SRAM
					read_buf = nand_read_buf_rtk;
					#else
					read_buf = this->g_databuf;
					#endif

				}

				for(i=0; i < page_size; i ++)
				{
					//this->g_databuf[i] = 0;
					read_buf[i] = 0;
				}
				if(rtk_mtd != NULL)
				{
					this = (struct nand_chip *)rtk_mtd->priv;
					rtk_read_diff_ecc(rtk_mtd, chipnr, page, 510, read_buf, this->g_oobbuf, 0);
				}
				for(i=0; i < page_size; i ++)
				{
					if(!(i %16))
						printf("\n");
					//printf(" %02x",  this->g_databuf[i]);
					//check_sum += (u32)this->g_databuf[i];
					printf(" %02x",  read_buf[i]);
					check_sum += (u32)read_buf[i];
				
				}
				//kfree(nand_read_buf);
				printk("\n[nand test]  %s, %d, checksum = %d \n",__FUNCTION__,__LINE__, check_sum);
				if(monflg != 5566)
				{
					printk("\n[nand test]  ECC correct = %d,RMZ_ECC_NUM : %d  \n",REG_READ_U32(REG_MAX_ECC_NUM) & 0xff, REG_READ_U32(REG_RMZ_ECC_NUM) & 0x7f);
				}

			break;
		case 10:
			printk("[nand test]%d bit ecc circuit verify \n", g_ecc_select);
			page = InPutData[1];
			rtk_nand_open_ecc_write(page);
			udelay(100);
			rtk_nand_close_ecc_read(page);
			udelay(100);
			rtk_nand_close_ecc_write(page+1);
			udelay(100);
			rtk_nand_open_ecc_read(page+1);
			break;
		case 11:
			printk("[nand test]16 bit randomizer ecc verify \n");
			page = InPutData[1];
			rtk_nand_open_randomizer_ecc_write(page);
			udelay(100);
			rtk_nand_close_randomizer_ecc_write(page);
			udelay(100);
			rtk_nand_open_randomizer_ecc_read(page);
			break;
		case 12:

			//write
			data_size = 4,842,585;
			data_block_size = data_size/(64*2048) + 1;
			data_page_size = data_size/2048 + 1;

			printk("[nand test] 26MB write, 208 block,  13312 pages %d \n",__LINE__);
			page_tmp = page = InPutData[1];
			chipnr= check_sum = 0;
			
			nand_write_buf_rtk = 0xa0000000;

			//erase 208 block
			for (i = 0; i < data_block_size; i++)
			{
				page = i*64;
				rtk_erase_block(rtk_mtd, chipnr, page);		
			}
			//gettimeofday(&start_time, NULL);
			if(rtk_mtd != NULL)
			{
				page = page_tmp;
				for (i = 0; i < data_page_size; i++)
				{
					page = i;
					this = (struct nand_chip *)rtk_mtd->priv;
					rtk_write_ecc_page(rtk_mtd, chipnr,  page, nand_write_buf_rtk, this->g_oobbuf);
					nand_write_buf_rtk = nand_write_buf_rtk + 2048;
				}
			}
			//gettimeofday(&end_time, NULL);
			//wr_exe_time = 1000000 * (end_time.tv_sec - start_time.tv_sec) + ((end_time.tv_usec - start_time.tv_usec));
			printk("[nand test]  %s, %d, checksum = %d \n",__FUNCTION__,__LINE__, check_sum);
			break;
		case 13:
			//read one page 
			data_size = 4,842,585;
			data_block_size = data_size/(64*2048) + 1;
			data_page_size = data_size/2048 + 1;

			printk("[nand test] read %s, %d \n",__FUNCTION__,__LINE__);
			page = InPutData[1];
			chipnr = check_sum = 0;
		
			if(rtk_mtd != NULL)
			{
				this = (struct nand_chip *)rtk_mtd->priv;
				if ( !this->g_databuf ){
					printk("%s: Error, no enough memory for nand_read_buf\n",__FUNCTION__);
					return ;
				}
				read_buf = 0xa2000000;
				
				//clean buffer
				for(i=0; i < page_size; i++)
				{					
					read_buf[i] = 0;
				}				
				for (i = 0; i < data_page_size; i++)
				{
					page = i;
					rtk_read_ecc_page(rtk_mtd, chipnr, page, read_buf, this->g_oobbuf); 	
					read_buf += 2048;
				}
						
			}

			break;
		default:
			//default
			rtk_nand_reset();
			break;
	}
	return;
}

//----------------------------------------------------------------------------
// add read-retry for Micron
void rtk_set_feature_micron(int P1){

	printk("[%s] enter \n", __FUNCTION__);
	
	REG_WRITE_U32(REG_DATA_TL0, 0x4);	//Set data transfer count[7:0]
	REG_WRITE_U32(REG_DATA_TL1, NF_DATA_TL1_access_mode(0x1) | NF_DATA_TL1_length1(0x0));	//Set data transfer count[13:8], SRAM path and access mode 

	//Set PP
	REG_WRITE_U32(REG_READ_BY_PP, NF_READ_BY_PP_read_by_pp(0)); //Data read to 	DRAM from NAND through PP		
	REG_WRITE_U32(REG_PP_CTL0, NF_PP_CTL0_pp_start_addr(0x0) | NF_PP_CTL0_pp_reset(0x0) | NF_PP_CTL0_pp_enable(0x1));	//Set PP starting address[9:8], PP reset and PP enable
	REG_WRITE_U32(REG_PP_CTL1, 0x0);	//Set PP starting assdress[7:0]

	//Set command
	REG_WRITE_U32(REG_ND_CMD, NF_ND_CMD_cmd(0xef));	//Set CMD1
	REG_WRITE_U32(REG_ND_CTL, NF_ND_CTL_xfer(1) | NF_ND_CTL_ecc_enable(0x0) | NF_ND_CTL_tran_mode(0x0));	//Enable Xfer, eanble ECC and set transfer mode 
	WAIT_DONE(REG_ND_CTL, 0x80, 0x00);	//Wait xfer done

	//Set address, 
	REG_WRITE_U32(REG_ND_PA0, 0x89);	//Set PA[7:0]
	REG_WRITE_U32(REG_ND_PA2, NF_ND_PA2_addr_mode(0x7));	//Set address mode & PA[20:16]		
	REG_WRITE_U32(REG_ND_CTL, NF_ND_CTL_xfer(1) | NF_ND_CTL_ecc_enable(0x0) | NF_ND_CTL_tran_mode(0x1));	//Enable Xfer, eanble ECC and set transfer mode 
	WAIT_DONE(REG_ND_CTL, 0x80, 0x00);	//Wait xfer done
	
	//Reset PP
	REG_WRITE_U32(REG_READ_BY_PP, NF_READ_BY_PP_read_by_pp(0)); //Data read to 	DRAM from NAND through PP		
	REG_WRITE_U32(REG_PP_CTL0, NF_PP_CTL0_pp_start_addr(0x0) | NF_PP_CTL0_pp_reset(0x1) | NF_PP_CTL0_pp_enable(0x0));	//Set PP starting address[9:8], PP reset and PP enable

	//Set data
	REG_WRITE_U32(REG_SRAM_CTL,NF_SRAM_CTL_map_sel(1)|NF_SRAM_CTL_access_en(0x1) | NF_SRAM_CTL_mem_region(0x0));	//Enable direct access SRAM
	REG_WRITE_U32(REG_ND_PA0, P1);	//Set data
	REG_WRITE_U32(REG_SRAM_CTL, NF_SRAM_CTL_access_en(0x0));	//Disable direct access SRAM
	//Set PP
	REG_WRITE_U32(REG_PP_CTL0, NF_PP_CTL0_pp_start_addr(0x0) | NF_PP_CTL0_pp_reset(0) | NF_PP_CTL0_pp_enable(1));	//Set PP starting address[9:8], PP reset and PP enable
	REG_WRITE_U32(REG_PP_CTL0, NF_PP_CTL0_pp_start_addr(0x0));				//Set PP starting address[9:8]
	REG_WRITE_U32(REG_ND_CTL, NF_ND_CTL_xfer(1) | NF_ND_CTL_ecc_enable(0) | NF_ND_CTL_tran_mode(0x3));	//Enable Xfer, eanble ECC and set transfer mode 	
	WAIT_DONE(REG_ND_CTL, 0xc0, 0x40);	//Wait xfer done
}	


#if 0
void syncPageRead(void)
{
    CP15DMB;
    //REG32(0x1801a020) = 0x0;
    REG_WRITE_U32(0x1801a020, 0x0);
    CP15DMB;
}

static void writeNFKey(unsigned int data[4])
{

	REG_WRITE_U32(CP_NF_KEY_0, data[0]);
	REG_WRITE_U32(CP_NF_KEY_1, data[1]);
	REG_WRITE_U32(CP_NF_KEY_2, data[2]);
	REG_WRITE_U32(CP_NF_KEY_3, data[3]);

}

static unsigned int OTP_JUDGE_BIT(unsigned int offset)
{
    unsigned int div_n=0, rem_n=0;
    unsigned int align_n=0, align_rem_n=0, real_n=0;

    rem_n = offset%8;
    div_n = offset/8;

    align_n = div_n & ~0x3;
    align_rem_n = div_n & 0x3;

    real_n = REG32(OTP_REG_BASE + align_n);
    return(((real_n >>(align_rem_n*8)) >> rem_n)&1);
}

//----------------------------------------------------------------------------

void setGPIOBit(unsigned int nGPIOBase, unsigned int nGPIOnum, unsigned int uBit)
{
        unsigned int whichReg = 0;
        unsigned int whichBit = 0;
        whichReg = nGPIOBase+4*(nGPIOnum/32);
        whichBit = nGPIOnum%32;
        if(uBit)
                REG_WRITE_U32(whichReg,SETBIT(REG_READ_U32(whichReg),whichBit));
        else
                REG_WRITE_U32(whichReg,CLEARBIT(REG_READ_U32(whichReg),whichBit));
}
//----------------------------------------------------------------------------
unsigned int getGPIOBit(unsigned int nGPIOBase, unsigned int nGPIOnum)
{
        unsigned int whichReg = 0;
        unsigned int whichBit = 0;
        unsigned int ret=0;
        whichReg = nGPIOBase+4*(nGPIOnum/32);
        whichBit = nGPIOnum%32;
        ret = REG_READ_U32(whichReg);
        ret>>=whichBit;
        ret&=(0x1);
        return ret;
}
//----------------------------------------------------------------------------

unsigned int getGPIORegVal(unsigned int nGPIOBase, unsigned int nGPIOnum)
{
        unsigned int whichReg = 0;
//        unsigned int whichBit = 0;
        whichReg = nGPIOBase+4*(nGPIOnum/32);
		//printk("\tgetRegister 0x%x\n",whichReg);
        return REG_READ_U32(whichReg);
}

//----------------------------------------------------------------------------
int is_jupiter_cpu(void)
{
	return 0;
}
//----------------------------------------------------------------------------
int is_saturn_cpu(void)
{
	return 1;
}

//----------------------------------------------------------------------------
int is_darwin_cpu(void)
{
	return 0;
}

//----------------------------------------------------------------------------

int is_macarthur_cpu(void)
{
	return 0;
}

//----------------------------------------------------------------------------
int is_nike_cpu(void)
{
	return 0;
}

//----------------------------------------------------------------------------
int is_venus_cpu(void)
{
	return 0;
}

//----------------------------------------------------------------------------
int is_neptune_cpu(void)
{
	return 0;
}

//----------------------------------------------------------------------------
int is_mars_cpu(void)
{
	return 0;
}

//----------------------------------------------------------------------------










#if RTK_ARD_ALGORITHM //function declartion
//----------------------------------------------------------------------------
void RTK_ARD_DumpProcWin(int nStartBlk, int nSize)
{
	int i=0;
	unsigned int nSysStartBlk = g_RecBlkStart;
	for(i=0;i<nSize;i++)
	{
		if(nStartBlk<=g_RecBlkEnd)
		{
			printk("[%u] %u \n",nStartBlk,g_RecArray[nStartBlk]);
			nStartBlk++;
		}
		else
		{
			printk("[%u] %u \n",nSysStartBlk,g_RecArray[nSysStartBlk]);
			nSysStartBlk++;
		}
	}
}
//----------------------------------------------------------------------------
void slideProcWindowPtr()
{
	int tmp=0;
	g_u32WinStart = g_u32WinEnd;
	if(((g_u32WinEnd+g_PorcWindowSize-1)>g_RecBlkEnd))//reverse
	{
		tmp =  g_RecBlkEnd - g_u32WinEnd+1;
		g_u32WinEnd = g_RecBlkStart + (g_PorcWindowSize - tmp)-1;
	}
	else
	{
		g_u32WinEnd+=(g_PorcWindowSize-1);
	}
}
//----------------------------------------------------------------------------
void resetBlock(int nBlkNo)
{
	static unsigned int tmpCnt = 0;
//Reser block....TBD...
////////////////////////////
	tmpCnt++;

printk("\n");
printk("\t[AT]Reset block [%u] : %u\n",nBlkNo,g_RecArray[nBlkNo]);
printk("\t[AT]Total reset %u blocks\n",tmpCnt);
	g_RecArray[nBlkNo]=0;

}
//----------------------------------------------------------------------------
void travelProcWindow()//When WinTrigCnt > WinTrigThrshld
{
	int maxVal[2];
	int minVal[2];
	int i=0;
	int idx=g_u32WinStart;

	if(g_RecArray[g_u32WinEnd] > g_RecArray[g_u32WinStart])
	{
		minVal[0]=g_u32WinStart;
		minVal[1]=g_RecArray[g_u32WinStart];
		maxVal[0]=g_u32WinEnd;
		maxVal[1]=g_RecArray[g_u32WinEnd];
	}
	else
	{
		maxVal[0]=g_u32WinStart;
		maxVal[1]=g_RecArray[g_u32WinStart];
		minVal[0]=g_u32WinEnd;
		minVal[1]=g_RecArray[g_u32WinEnd];
	}

	printk("++++Before Travel++++\n");
	printk("minVal[%u] %u, maxVal[%u] %u\n",minVal[0],minVal[1],maxVal[0],maxVal[1]);
	RTK_ARD_DumpProcWin(g_u32WinStart,g_PorcWindowSize);
	for(i=0;i<g_PorcWindowSize;i++)
	{
		if(g_RecArray[idx]>=g_ReadCntThrshld)
			resetBlock(idx);
		if(idx<=g_RecBlkEnd)
		{
			if(g_RecArray[idx]<minVal[1])//Process minimun value
			{
				minVal[0]=idx;
				minVal[1]=g_RecArray[idx];
			}
			if(g_RecArray[idx]>maxVal[1])//Process maximun value
			{
				maxVal[0]=idx;
				maxVal[1]=g_RecArray[idx];
			}
			if(idx==g_RecBlkEnd)
				idx = g_RecBlkStart;
			else
				idx++;
		}

	}
	printk("----After Travel w/o Grouping ----\n");
	printk("minVal[%u] %u, maxVal[%u] %u\n",minVal[0],minVal[1],maxVal[0],maxVal[1]);

	//Grouping
	g_RecArray[minVal[0]] += g_DistInc;
	if((maxVal[1]-minVal[1]) < g_MaxMinDist)
	{
		g_RecArray[maxVal[0]] += g_MaxMinDist;
	}

	RTK_ARD_DumpProcWin(g_u32WinStart,g_PorcWindowSize);
	slideProcWindowPtr();
	g_WinTrigCnt = 0; //Reset windows trigger count
}
#endif
//----------------------------------------------------------------------------

/*
	parse_token can parse a string and extract the value of designated token.
		parsed_string: The string to be parsed.
		token: the name of the token
		return value: If return value is NULL, it means that the token is not found.
			If return value is "non zero", it means that the token is found, and return value will be a string that contains the value of that token.
			If the token doesn't have a value, return value will be a string that contains empty string ("\0").
			If return value is "non zero", "BE SURE" TO free it after you donot need it.

		Exp:
			char *value=parse_token("A1 A2=222 A3=\"333 333\"", "A0");
				=> value=NULL
			char *value=parse_token("A1 A2=222 A3=\"333 333\"", "A1");
				=> value points to a string of "\0"
			char *value=parse_token("A1 A2=222 A3=\"333 333\"", "A2");
				=> value points to a string of "222"
			char *value=parse_token("A1 A2=222 A3=\"333 333\"", "A3");
				=> value points to a string of "333 333"
*/
 char *rtkNF_parse_token(const char *parsed_string, const char *token)
{
	const char *ptr = parsed_string;
	const char *start, *end, *value_start, *value_end;
	char *ret_str;

	while(1) {
		value_start = value_end = 0;
		for(;*ptr == ' ' || *ptr == '\t'; ptr++)	;
		if(*ptr == '\0')	break;
		start = ptr;
		for(;*ptr != ' ' && *ptr != '\t' && *ptr != '=' && *ptr != '\0'; ptr++) ;
		end = ptr;
		if(*ptr == '=') {
			ptr++;
			if(*ptr == '"') {
				ptr++;
				value_start = ptr;
				for(; *ptr != '"' && *ptr != '\0'; ptr++)	;
				if(*ptr != '"' || (*(ptr+1) != '\0' && *(ptr+1) != ' ' && *(ptr+1) != '\t')) {
					printk("system_parameters error! Check your parameters.");
					break;
				}
			} else {
				value_start = ptr;
				for(;*ptr != ' ' && *ptr != '\t' && *ptr != '\0' && *ptr != '"'; ptr++) ;
				if(*ptr == '"') {
					printk("system_parameters error! Check your parameters.");
					break;
				}
			}
			value_end = ptr;
		}

		if(!strncmp(token, start, end-start)) {
			if(value_start) {
				ret_str = kmalloc(value_end-value_start+1, GFP_KERNEL);
				if(ret_str)
				{
					strncpy(ret_str, value_start, value_end-value_start);
					ret_str[value_end-value_start] = '\0';
				}
				return ret_str;
			} else {
				if(ret_str)
				{
					ret_str = kmalloc(1, GFP_KERNEL);
					strcpy(ret_str, "");
				}
				return ret_str;
			}
		}
	}

	return (char*)NULL;
}

//----------------------------------------------------------------------------
unsigned int rtkNF_getWPSettingval(void)
{
    unsigned char *param;
	unsigned int retWPval = 0 ;
//    param=(char *)rtkNF_parse_token(platform_info.system_parameters,"WP_PIN_EN");

    if(param)
		retWPval = simple_strtoul(param, (char**)&retWPval, 16);
	else
		retWPval = 0;
    return retWPval;
}
//----------------------------------------------------------------------------

unsigned int rtkNF_getBootcodeSize(void)
{
    unsigned char *param;
	unsigned int retBootcodeSize = -1 ;
    //param=(char *)rtkNF_parse_token(platform_info.system_parameters,"nand_boot_size");

    if(param)
		retBootcodeSize = simple_strtoul(param, (char**)&retBootcodeSize, 16);
    return retBootcodeSize;
}
//----------------------------------------------------------------------------
unsigned int rtkNF_getFactParaSize(void)
{
    unsigned char *param;
	unsigned int retFactParaSize = -1 ;
  //  param=(char *)rtkNF_parse_token(platform_info.system_parameters,"factory_size");

    if(param)
		retFactParaSize = simple_strtoul(param, (char**)&retFactParaSize, 16);
    return retFactParaSize;
}

//----------------------------------------------------------------------------
unsigned int rtkNF_getRBAPercent(void)
{
    unsigned char *param;
	unsigned int retRbaPercent = -1 ;
//    param=(char *)rtkNF_parse_token(platform_info.system_parameters,"rba_percent");

    if(param)
		retRbaPercent = simple_strtoul(param, (char**)&retRbaPercent, 16);
    return retRbaPercent;
}

//----------------------------------------------------------------------------

void WAIT_DONE(unsigned int addr, unsigned int mask, unsigned int value)
{
        int timeout = 0;

	while ( (REG_READ_U32(addr) & mask) != value )
	{

                if(timeout++>100000){
                    //printk("[%s]timeout..%d\n",__FUNCTION__, timeout);
                    return;
                 }
    
		asm("nop");
	}
}
//----------------------------------------------------------------------------

static void rtk_nand_resumeReg(void)
{
	//REG_WRITE_U32( 0xb801032c,0x01);	//Enable NAND/CardReader arbitrator
	REG_WRITE_U32( REG_CLOCK_ENABLE1,REG_READ_U32(REG_CLOCK_ENABLE1)& (~0x00800000));
	//if(is_jupiter_cpu()||is_macarthur_cpu()||is_nike_cpu())
	//	REG_WRITE_U32( REG_NF_CKSEL,0x02 );
	//else if(is_saturn_cpu()||is_darwin_cpu())
	//	REG_WRITE_U32( REG_NF_CKSEL,0x02 );
	REG_WRITE_U32( REG_NF_CKSEL,0x04 );
	REG_WRITE_U32( REG_CLOCK_ENABLE1,REG_READ_U32(REG_CLOCK_ENABLE1)| (0x00800000));

	if(is_jupiter_cpu()||is_macarthur_cpu()||is_nike_cpu())
		REG_WRITE_U32( REG_SPR_DDR_CTL,0x7<<26);
	else
		REG_WRITE_U32(REG_SPR_DDR_CTL,NF_SPR_DDR_CTL_cr_nf_hw_pinmux_ena(1) | NF_SPR_DDR_CTL_spare_ddr_ena(1) | NF_SPR_DDR_CTL_per_2k_spr_ena(1)| NF_SPR_DDR_CTL_spare_dram_sa(0)); //set spare2ddr func. 4=>0.5k spe2ddr_ena at A000F000

	REG_WRITE_U32( REG_PD,~(0x1 << 0) );
	REG_WRITE_U32( REG_ND_CMD,CMD_RESET );

	if(is_jupiter_cpu()||is_macarthur_cpu()||is_nike_cpu())
		REG_WRITE_U32( REG_ND_CTL,(0x80 | 0x00));
	else
		REG_WRITE_U32( REG_ND_CTL,(NF_ND_CTL_xfer(0x01) | NF_ND_CTL_tran_mode(0)) );

	WAIT_DONE(REG_ND_CTL,0x80,0);
	WAIT_DONE(REG_ND_CTL,0x40,0x40);

	REG_WRITE_U32( REG_TIME_PARA1,NF_TIME_PARA1_T1(0x0));
	REG_WRITE_U32( REG_TIME_PARA2,NF_TIME_PARA2_T2(0x0));
	REG_WRITE_U32( REG_TIME_PARA3,NF_TIME_PARA3_T3(0x0));

	if(!is_jupiter_cpu()||!is_saturn_cpu()||!is_darwin_cpu()||!is_macarthur_cpu()||!is_nike_cpu())
		REG_WRITE_U32( REG_DELAY_CTL,NF_DELAY_CTL_T_WHR_ADL(0x09) );

	REG_WRITE_U32( REG_MULTI_CHNL_MODE,NF_MULTI_CHNL_MODE_edo(0x01));
	REG_WRITE_U32( REG_ECC_STOP,NF_ECC_STOP_ecc_n_stop(0x01));

}
//----------------------------------------------------------------------------

/*
 * RTK NAND suspend:
 */
static void rtk_nand_suspend (struct mtd_info *mtd)
{
	printk("[%s]Enter..\n",__FUNCTION__);
	//if(g_enReadRetrial)
	if (0)
	{
		rtk_xfer_SetParameter(regVal1,regVal2,regVal3,regVal4);
	}
	WAIT_DONE(REG_AUTO_TRIG,0x80,0);
	WAIT_DONE(REG_DMA_CTL3,0x01,0);
	printk("[%s]Exit..\n",__FUNCTION__);

}
//----------------------------------------------------------------------------
static void rtk_nand_resume (struct mtd_info *mtd)
{
	printk("[%s]\n",__FUNCTION__);
	//if(g_enReadRetrial)
	if (0)
	{
		if((regVal1==0)&&(regVal2==0)&&(regVal3==0)&&(regVal4==0))
			rtk_xfer_GetParameter();
	}
	rtk_nand_resumeReg();
	printk("[%s]Exit..\n",__FUNCTION__);

}
//----------------------------------------------------------------------------
static void rtk_read_oob_from_SRAM(struct mtd_info *mtd, __u8 *r_oobbuf)
{
	unsigned int reg_oob, reg_num;
	int i;
	//printk("mtd->ecctype 0x%x\n",mtd->ecctype);
//	if ( mtd->ecctype == MTD_ECC_NONE )
	if ( mtd->ecctype == MTD_ECC_NONE ||(mtd->ecctype!=MTD_ECC_RTK_HW))
	{
		reg_num = REG_NF_BASE_ADDR;
		reg_oob = REG_READ_U32(reg_num);
		r_oobbuf[0] = reg_oob & 0xff;
		r_oobbuf[1] = (reg_oob >> 8) & 0xff;
		r_oobbuf[2] = (reg_oob >> 16) & 0xff;
		r_oobbuf[3] = (reg_oob >> 24) & 0xff;

		reg_num = REG_NF_BASE_ADDR+4;
		reg_oob = REG_READ_U32(reg_num);
		r_oobbuf[4] = reg_oob & 0xff;

		reg_num = REG_NF_BASE_ADDR+16;
		reg_oob = REG_READ_U32(reg_num);
		r_oobbuf[5] = reg_oob & 0xff;
		r_oobbuf[6] = (reg_oob >> 8) & 0xff;
		r_oobbuf[7] = (reg_oob >> 16) & 0xff;
		r_oobbuf[8] = (reg_oob >> 24) & 0xff;

		reg_num = REG_NF_BASE_ADDR+16*2;
		reg_oob = REG_READ_U32(reg_num);
		r_oobbuf[9] = reg_oob & 0xff;
		r_oobbuf[10] = (reg_oob >> 8) & 0xff;
		r_oobbuf[11] = (reg_oob >> 16) & 0xff;
		r_oobbuf[12] = (reg_oob >> 24) & 0xff;

		reg_num = REG_NF_BASE_ADDR+16*3;
		reg_oob = REG_READ_U32(reg_num);
		r_oobbuf[13] = reg_oob & 0xff;
		r_oobbuf[14] = (reg_oob >> 8) & 0xff;
		r_oobbuf[15] = (reg_oob >> 16) & 0xff;
		r_oobbuf[16] = (reg_oob >> 24) & 0xff;

	}
	else
	{
		for ( i=0; i < 16; i++)
			{
				reg_num = REG_NF_BASE_ADDR + i*4;
				reg_oob = REG_READ_U32(reg_num);
				r_oobbuf[i*4+0] = reg_oob & 0xff;
				r_oobbuf[i*4+1] = (reg_oob >> 8) & 0xff;
				r_oobbuf[i*4+2] = (reg_oob >> 16) & 0xff;
				r_oobbuf[i*4+3] = (reg_oob >> 24) & 0xff;
			}
	}
}

//----------------------------------------------------------------------------
static void rtk_nand_read_id(struct mtd_info *mtd, u_char id[6])
{
#define XFER_MODE
	int id_chain;
	//printk("rtk_nand_read_id \n");
	//Set SRAM path and access mode
#ifdef XFER_MODE
	REG_WRITE_U32(REG_DATA_TL0,NF_DATA_TL0_length0(0x06));
	REG_WRITE_U32(REG_DATA_TL1,NF_DATA_TL1_access_mode(0x01));

	//Set PP
	REG_WRITE_U32(REG_READ_BY_PP,NF_READ_BY_PP_read_by_pp(0x0));

	REG_WRITE_U32(REG_PP_CTL0,NF_PP_CTL0_pp_enable(0x01));
	REG_WRITE_U32(REG_PP_CTL1,NF_PP_CTL1_pp_start_addr(0));

	//Set command
	REG_WRITE_U32(REG_ND_CMD,NF_ND_CMD_cmd(CMD_READ_ID));
	REG_WRITE_U32(REG_ND_CTL,NF_ND_CTL_xfer(0x01));
	WAIT_DONE(REG_ND_CTL,0x80,0);

	//Set address
	REG_WRITE_U32(REG_ND_PA0, 0);
	REG_WRITE_U32(REG_ND_PA1, 0);
	REG_WRITE_U32(REG_ND_PA2,NF_ND_PA2_addr_mode(0x07));

	REG_WRITE_U32(REG_ND_CTL,NF_ND_CTL_xfer(1)|NF_ND_CTL_tran_mode(1));
	WAIT_DONE(REG_ND_CTL,0x80,0);
	//Enable XFER mode
	REG_WRITE_U32(REG_ND_CTL,NF_ND_CTL_xfer(1)|NF_ND_CTL_tran_mode(4));
	WAIT_DONE(REG_ND_CTL,0x80,0);

	//Reset PP
	REG_WRITE_U32(REG_PP_CTL0,NF_PP_CTL0_pp_reset(1));

	//Move data to DRAM from SRAM
	REG_WRITE_U32(REG_SRAM_CTL,NF_SRAM_CTL_map_sel(1)|NF_SRAM_CTL_access_en(1)|NF_SRAM_CTL_mem_region(0));

	id_chain = REG_READ_U32(REG_ND_PA0);
	//printk("id_chain 1 = 0x%x \n", id_chain);
	id[0] = id_chain & 0xff;
	id[1] = (id_chain >> 8) & 0xff;
	id[2] = (id_chain >> 16) & 0xff;
	id[3] = (id_chain >> 24) & 0xff;

	g_id_chain = id_chain;
	//if(id_chain==0xDA94D7AD)
	//{
		//printk("Detect H27UBG8T2B and enable read-retrial mode...\n");
		//g_enReadRetrial = 1;
		//g_isRandomize = 1;
		//printk("Enable Read-retrial mode...\n");
		//printk("Enable Randomized mechanism\n");
	//}
	if(id_chain == 0x4b44642c)
	{
		printk("Detect MT29F64G08CBABA and enable read-retrial mode...\n");
		g_enReadRetrial = 1;
	}else if(id_chain == 0x4b44442c)
	{
		printk("Detect MT29F32G08CBADA and enable read-retrial mode\n");
		g_enReadRetrial = 1;
	}else if(id_chain == 0x9384d798){
		printk("Detect TC58TEG5DCJT and enable read-retrial mode...\n");
		g_enReadRetrial = 1;
        }else if(id_chain == 0x9384de98){
		printk("Detect TC58TEG6DCJT and enable read-retrial mode...\n");
		g_enReadRetrial = 1;
        }else if(id_chain == 0x9394de98){
		printk("Detect TC58TEG6DDK and enable read-retrial mode...\n");
		g_enReadRetrial = 1;
        }


	id_chain = REG_READ_U32(REG_ND_PA1);
	//printk("id_chain 2 = 0x%x \n", id_chain);
	id[4] = id_chain & 0xff;
	id[5] = (id_chain >> 8) & 0xff;

	REG_WRITE_U32(REG_SRAM_CTL,0x0);	//# no omitted
#else
	REG_WRITE_U32(REG_PP_CTL0,NF_PP_CTL0_pp_reset(0x01));
	REG_WRITE_U32(REG_DATA_TL0,NF_DATA_TL0_length0(0x00));
	REG_WRITE_U32(REG_DATA_TL1,NF_DATA_TL1_access_mode(0x01)|NF_DATA_TL1_length1(0x2));

	//Set page length at auto mode 
	REG_WRITE_U32(REG_PAGE_LEN, 0x1);
	REG_WRITE_U32(REG_READ_BY_PP,NF_READ_BY_PP_read_by_pp(0x1));

	REG_WRITE_U32(REG_PP_CTL0,NF_PP_CTL0_pp_enable(0x01));
	REG_WRITE_U32(REG_PP_CTL1,NF_PP_CTL1_pp_start_addr(0));

	//Set command
	REG_WRITE_U32(REG_ND_CMD,NF_ND_CMD_cmd(CMD_READ_ID));

	//Set address
	REG_WRITE_U32(REG_ND_PA0, 0);
	REG_WRITE_U32(REG_ND_PA1, 0);
	REG_WRITE_U32(REG_ND_PA2,NF_ND_PA2_addr_mode(0x07));
	
	//Set ECC	
	REG_WRITE_U32(REG_MULTI_CHNL_MODE, NF_MULTI_CHNL_MODE_ecc_no_check(1) | NF_MULTI_CHNL_MODE_ecc_pass(1));	//Set HW no check ECC	
	REG_WRITE_U32(REG_ECC_STOP, NF_ECC_STOP_ecc_n_stop(0x1));	//Set ECC no stop
	//Set DMA	
	REG_WRITE_U32(REG_DMA_CTL1, id);	//Set DRAM start address
	REG_WRITE_U32(REG_DMA_CTL2, 0x1);	//Transfer length (Unit = 512B)	
	REG_WRITE_U32(REG_DMA_CTL3, NF_DMA_CTL3_ddr_wr(1) | NF_DMA_CTL3_dma_xfer(1));	//Set DMA direction and enable DMA transfer 
	//Enable Auto mode
	REG_WRITE_U32(REG_AUTO_TRIG,NF_AUTO_TRIG_auto_trig(1)|NF_AUTO_TRIG_spec_auto_case(1)| NF_AUTO_TRIG_auto_case(0));
	WAIT_DONE(REG_AUTO_TRIG,0x80,0);
	//Enable Auto mode
	REG_WRITE_U32(REG_AUTO_TRIG,NF_AUTO_TRIG_auto_trig(1)|NF_AUTO_TRIG_spec_auto_case(0)| NF_AUTO_TRIG_auto_case(4));
	WAIT_DONE(REG_AUTO_TRIG,0x80,0);
	//Wait DMA done	
	WAIT_DONE(REG_DMA_CTL3, 0x01, 0x00);	
#endif

}
//----------------------------------------------------------------------------
#if 1
static void reverse_to_Tags(char *r_oobbuf, int eccBits)
{
	int k,j;

	if(is_jupiter_cpu())
	{
		for ( k=0; k<4; k++ )
				r_oobbuf[5+k] = r_oobbuf[16+k];

			memcpy(&r_oobbuf[9],&r_oobbuf[32],4);//add by alexchang for improve nand read 0225-2010
			memcpy(&r_oobbuf[13],&r_oobbuf[48],4);//add by alexchang for improve nand read 0225-2010
	}

	else
	{

		if(eccBits==0)
		{
	    for ( k=0; k<4; k++ )
			r_oobbuf[5+k] = r_oobbuf[8+k];

		memcpy(&r_oobbuf[9],&r_oobbuf[16],4);//add by alexchang for improve nand read 0225-2010
		memcpy(&r_oobbuf[13],&r_oobbuf[24],4);//add by alexchang for improve nand read 0225-2010
		}
		else if(eccBits==0x18)
		{
			for(k=0,j=1;k<28;k+=4,j++)
				memcpy(&r_oobbuf[1+k],&r_oobbuf[8*j],4);
		}
	}
}
#endif
//----------------------------------------------------------------------------
static int rtk_Process_Buf(unsigned char* oob_buf)
{
	int j=0,k=0;
	unsigned int reg_oob, reg_num;

	REG_WRITE_U32(REG_READ_BY_PP,0x00);
	REG_WRITE_U32(REG_SRAM_CTL, 0x30 | 0x04);

	for ( j=0; j < 12; j++)
	{
		reg_num = REG_NF_BASE_ADDR + j*4;
		reg_oob = REG_READ_U32(reg_num);
		oob_buf[j*4+0] = reg_oob & 0xff;
		oob_buf[j*4+1] = (reg_oob >> 8) & 0xff;
		if(j==11)
			break;
		oob_buf[j*4+2] = (reg_oob >> 16) & 0xff;
		oob_buf[j*4+3] = (reg_oob >> 24) & 0xff;
	}

	for ( j=16,k=0; j < 28; j++,k++)
	{
		reg_num = REG_NF_BASE_ADDR + j*4;
		reg_oob = REG_READ_U32(reg_num);
		oob_buf[46+k*4+0] = reg_oob & 0xff;
		oob_buf[46+k*4+1] = (reg_oob >> 8) & 0xff;
		if(j==27)
			break;
		oob_buf[46+k*4+2] = (reg_oob >> 16) & 0xff;
		oob_buf[46+k*4+3] = (reg_oob >> 24) & 0xff;
	}

	REG_WRITE_U32(REG_SRAM_CTL, 0x00);
	REG_WRITE_U32(REG_READ_BY_PP,0x80);
	return 0;
}
//----------------------------------------------------------------------------

static int rtk_read_regTrigger(int isCPdisable,int page_size,int triggerMode)
{
#if RTK_NAND_INTERRUPT
	NF_RESET_IRQ;
	NF_ENABLE_IRQ(NF_DMA_IRQ_MASK);
	g_nfFlag = NF_READ_TRIGGER;
#endif
	if(is_NF_CP_Enable_read&&(!isCPdisable))
	{
		REG_WRITE_U32(REG_CP_NF_KEY,*regKey0);
		REG_WRITE_U32((REG_CP_NF_KEY+0x4),*regKey1);
		REG_WRITE_U32((REG_CP_NF_KEY+0x8),*regKey2);
		REG_WRITE_U32((REG_CP_NF_KEY+0xc),*regKey3);

		REG_WRITE_U32(REG_CP_NF_SET,u32CP_mode|cpSel);
		REG_WRITE_U32(REG_CP_LEN,NF_CP_LEN_cp_length(page_size));
		REG_WRITE_U32(REG_DMA_CTL3,NF_DMA_CTL3_cp_enable(0x1)|NF_DMA_CTL3_cp_first(0x1)|NF_DMA_CTL3_ddr_wr(0x1)|NF_DMA_CTL3_dma_xfer(0x1));
	}
	else
		REG_WRITE_U32(REG_DMA_CTL3,NF_DMA_CTL3_cp_enable(0)|NF_DMA_CTL3_cp_first(0)|NF_DMA_CTL3_ddr_wr(0x1)|NF_DMA_CTL3_dma_xfer(0x1));
	REG_WRITE_U32( REG_AUTO_TRIG,NF_AUTO_TRIG_auto_trig(1)|NF_AUTO_TRIG_spec_auto_case(0)| NF_AUTO_TRIG_auto_case(triggerMode));

#if RTK_NAND_INTERRUPT
	RTK_WAIT_EVENT;
#else
	WAIT_DONE(REG_AUTO_TRIG,0x80,0);
	WAIT_DONE(REG_DMA_CTL3,0x01,0);
#endif
	if(is_NF_CP_Enable_read&&(!isCPdisable))//Clear register
	{
		REG_WRITE_U32(REG_CP_NF_SET,0);
	}
	return 0;
}


//----------------------------------------------------------------------------
static int rtk_read_ecc_page_withLargeECC (struct mtd_info *mtd, u16 chipnr, unsigned int page, u_char *data_buf, u_char *oob_buf)
{

	struct nand_chip *this = NULL;
	int rc = 0;
	int dram_sa, dma_len, spare_dram_sa;
	int blank_all_one = 0;
	int page_len;
	int r_unit = 2048;
	int i=0,cnt=0;
//	unsigned int reg_oob, reg_num;
	unsigned char *ptr_oob;
	unsigned int  *ptr_data;

	unsigned int chip_section = 0;
	unsigned int section = 0;
	unsigned int index = 0;
	uint8_t	auto_trigger_mode = 2;
	uint8_t	addr_mode = 1;

	printk("[%s] enter \n",__FUNCTION__);

	ptr_oob = kmalloc(128,GFP_KERNEL);
	ptr_data = kmalloc(2048,GFP_KERNEL);
//printk("ptr_oob 0x%x\n",ptr_oob);
//printk("ptr_data 0x%x\n",ptr_data);

	this = (struct nand_chip *) mtd->priv;
	page_size = mtd->oobblock;
	oob_size = mtd->oobsize;
	ppb = mtd->erasesize/mtd->oobblock;
	cnt = page_size/r_unit;
    if(page_size%r_unit)
    {
		printk("[%s]ERROR page_size not alignment...\n",__FUNCTION__);
		return -1;
    }

	page_size = 2048;

	REG_WRITE_U32(REG_BLANK_CHK,NF_BLANK_CHK_blank_ena(1) );
	REG_WRITE_U32(REG_DATA_TL0,NF_DATA_TL0_length0(0));

	if(this->ecc_select >= 0x18)
	{
		page_len = page_size >> 10;
		REG_WRITE_U32(REG_DATA_TL1,NF_DATA_TL1_access_mode(1)|NF_DATA_TL1_length1(4));
	}
	else
	{
		page_len = page_size >> 9;
		REG_WRITE_U32(REG_DATA_TL1,NF_DATA_TL1_access_mode(1)|NF_DATA_TL1_length1(2));
	}
	REG_WRITE_U32(REG_PAGE_LEN,NF_PAGE_LEN_page_len(page_len));

	REG_WRITE_U32(REG_READ_BY_PP,NF_READ_BY_PP_read_by_pp(1));
	REG_WRITE_U32(REG_PP_CTL1,NF_PP_CTL1_pp_start_addr(0));
	REG_WRITE_U32(REG_PP_CTL0,0);

	REG_WRITE_U32(REG_ND_CMD,NF_ND_CMD_cmd(CMD_PG_READ_C1));
	REG_WRITE_U32(REG_CMD2,NF_CMD2_cmd2(CMD_PG_READ_C2));
	REG_WRITE_U32(REG_CMD3,NF_CMD3_cmd3(CMD_PG_READ_C3));

	REG_WRITE_U32( REG_ND_PA0,NF_ND_PA0_page_addr0(page));
	REG_WRITE_U32( REG_ND_PA1,NF_ND_PA1_page_addr1(page>>8));
	REG_WRITE_U32( REG_ND_PA2,NF_ND_PA2_addr_mode(addr_mode)|NF_ND_PA2_page_addr2(page>>16));
	REG_WRITE_U32( REG_ND_PA3,NF_ND_PA3_page_addr3((page>>21)&0x7));
	REG_WRITE_U32(REG_MULTI_CHNL_MODE,NF_MULTI_CHNL_MODE_edo(1));//add by alexchang0205-2010
	REG_WRITE_U32( REG_ECC_STOP,NF_ECC_STOP_ecc_n_stop(0x01));

	switch(this->ecc_select) {
		case 6:
			REG_WRITE_U32(REG_ECC_SEL, 0x0);
			break;
		case 12:
			REG_WRITE_U32(REG_ECC_SEL, 0x1);
			break;
		case 16:
			REG_WRITE_U32(REG_ECC_SEL, 0xe);
			break;
		case 24:
			REG_WRITE_U32(REG_ECC_SEL, 0xa);
			break;
		case 40:
			REG_WRITE_U32(REG_ECC_SEL, 0x2);
			break;
		case 43:
			REG_WRITE_U32(REG_ECC_SEL, 0x4);
			break;
		case 65:
			REG_WRITE_U32(REG_ECC_SEL, 0x6);
			break;
		case 72:
			REG_WRITE_U32(REG_ECC_SEL, 0x8);
			break;
		default:
			REG_WRITE_U32(REG_ECC_SEL, 0x0);
			break;
	}

	dram_sa = ( (uint32_t)data_buf >> 3);
	REG_WRITE_U32(REG_DMA_CTL1,NF_DMA_CTL1_dram_sa(dram_sa));
	dma_len = page_size >> 9;
	REG_WRITE_U32(REG_DMA_CTL2,NF_DMA_CTL2_dma_len(dma_len));
	REG_WRITE_U32(REG_DMA_CTL3,NF_DMA_CTL3_ddr_wr(1)|NF_DMA_CTL3_dma_xfer(1));

	if (ptr_oob)
		spare_dram_sa = ( (uint32_t)ptr_oob >> 3);
	else
		spare_dram_sa = ( (uint32_t)this->g_oobbuf >> 3);
	REG_WRITE_U32( REG_SPR_DDR_CTL,NF_SPR_DDR_CTL_spare_ddr_ena(1)|NF_SPR_DDR_CTL_per_2k_spr_ena(1)|NF_SPR_DDR_CTL_spare_dram_sa(spare_dram_sa));

	rtk_read_regTrigger(mtd->isCPdisable_R, page_size,auto_trigger_mode);

	rtk_Process_Buf(oob_buf);
	for(;i<cnt-1;i++)
	{
		data_buf+=2048;
		oob_buf+=92;
		dram_sa = ( (uint32_t)data_buf >> 3);

		if (ptr_oob)
			spare_dram_sa = ( (uint32_t)ptr_oob >> 3);
		else
			spare_dram_sa = ( (uint32_t)this->g_oobbuf >> 3);

		REG_WRITE_U32( REG_SPR_DDR_CTL,NF_SPR_DDR_CTL_spare_ddr_ena(1)|NF_SPR_DDR_CTL_per_2k_spr_ena(1)|NF_SPR_DDR_CTL_spare_dram_sa(spare_dram_sa));
		REG_WRITE_U32(REG_DMA_CTL1,NF_DMA_CTL1_dram_sa(dram_sa));
		rtk_read_regTrigger(mtd->isCPdisable_R, page_size,0x04);
		rtk_Process_Buf(oob_buf);
	}

	if(ptr_oob)
	{
		kfree(ptr_oob);
		ptr_oob=NULL;
	}
	if(ptr_data)
	{
		kfree(ptr_data);
		ptr_data=NULL;
	}

	chip_section = (chipnr * this->page_num) >> 5;
	section = page >> 5;
	index = page & (32-1);
	blank_all_one = (REG_READ_U32(REG_BLANK_CHK)>>1) & 0x01;

	mtd->nEccMaxValue = REG_MAX_ECC_NUM_max_ecc_num(REG_READ_U32(REG_MAX_ECC_NUM));
	if (blank_all_one)
	{
		if(this->erase_page_flag)
			this->erase_page_flag[chip_section+section] =  (1<< index);
	}

	if (REG_READ_U32(REG_ND_ECC) & 0x0C){
		if(!this->erase_page_flag)
		{
			if (REG_READ_U32(REG_ND_ECC) & 0x08){
				if ( chipnr == 0 && (page < (g_BootcodeSize/page_size)) )
				{
					return 0;
				}
				else
				{
					return -1;
				}
			}
		}
		else
		{
			if (this->erase_page_flag[chip_section+section] & (1<< index) ){
				;
			}else
			{
				if (REG_READ_U32(REG_ND_ECC) & 0x08){
					if ( chipnr == 0 && (page < (g_BootcodeSize/page_size)) )
					{
						return 0;
					}
					else
					{
						return -1;
					}
				}
			}
		}
	}
#if RTK_ARD_ALGORITHM
	g_WinTrigCnt++;
	whichBlk = page / ppb;
	g_RecArray[whichBlk]++;
	if(g_WinTrigCnt > g_WinTrigThrshld)
		travelProcWindow();
#endif
	mtd->isCPdisable_R = 0;
	return rc;
}

//----------------------------------------------------------------------------
// add read-retry for TOSHIBA
void rtk_set_terminate_toshiba(void){


	printk("[%s] enter\n", __FUNCTION__);

	//Set command
	REG_WRITE_U32(REG_ND_CMD, NF_ND_CMD_cmd(0xff));	//Set CMD1
	REG_WRITE_U32(REG_ND_CTL, NF_ND_CTL_xfer(1) | NF_ND_CTL_ecc_enable(0x0) | NF_ND_CTL_tran_mode(0x0));	//Enable Xfer, eanble ECC and set transfer mode 
	WAIT_DONE(REG_ND_CTL, 0x80, 0x00);	//Wait xfer done

}

//----------------------------------------------------------------------------
// add read-retry for TOSHIBA
void rtk_set_pre_condition_toshiba(void){


	printk("[%s] enter\n", __FUNCTION__);

	//Set command
	REG_WRITE_U32(REG_ND_CMD, NF_ND_CMD_cmd(0x5C));	//Set CMD1
	REG_WRITE_U32(REG_ND_CTL, NF_ND_CTL_xfer(1) | NF_ND_CTL_ecc_enable(0x0) | NF_ND_CTL_tran_mode(0x0));	//Enable Xfer, eanble ECC and set transfer mode 
	WAIT_DONE(REG_ND_CTL, 0x80, 0x00);	//Wait xfer done

	//Set command
	REG_WRITE_U32(REG_ND_CMD, NF_ND_CMD_cmd(0xC5));	//Set CMD2
	REG_WRITE_U32(REG_ND_CTL, NF_ND_CTL_xfer(1) | NF_ND_CTL_ecc_enable(0x0) | NF_ND_CTL_tran_mode(0x0));	//Enable Xfer, eanble ECC and set transfer mode 
	WAIT_DONE(REG_ND_CTL, 0x80, 0x00);	//Wait xfer done


}

//----------------------------------------------------------------------------
// add read-retry for TOSHIBA
void rtk_set_ext_feature_toshiba(void){


	printk("[%s] enter\n", __FUNCTION__);

	//Set command
	REG_WRITE_U32(REG_ND_CMD, NF_ND_CMD_cmd(0x26));	//Set CMD1
	REG_WRITE_U32(REG_ND_CTL, NF_ND_CTL_xfer(1) | NF_ND_CTL_ecc_enable(0x0) | NF_ND_CTL_tran_mode(0x0));	//Enable Xfer, eanble ECC and set transfer mode 
	WAIT_DONE(REG_ND_CTL, 0x80, 0x00);	//Wait xfer done

	//Set command
	REG_WRITE_U32(REG_ND_CMD, NF_ND_CMD_cmd(0x5d));	//Set CMD2
	REG_WRITE_U32(REG_ND_CTL, NF_ND_CTL_xfer(1) | NF_ND_CTL_ecc_enable(0x0) | NF_ND_CTL_tran_mode(0x0));	//Enable Xfer, eanble ECC and set transfer mode 
	WAIT_DONE(REG_ND_CTL, 0x80, 0x00);	//Wait xfer done


}

//----------------------------------------------------------------------------
// add read-retry for TOSHIBA
void rtk_set_feature_toshiba(int P1){

        int i = 0;

	printk("[%s] set slow timer, cnt=0x%x\n", __FUNCTION__, P1);

	REG_WRITE_U32(0x18000038, 0xf);		//432/(N+1)	
	REG_WRITE_U32(REG_TIME_PARA3,  NF_TIME_PARA3_T3(0x1));			//Set flash timming T3	
	REG_WRITE_U32(REG_TIME_PARA2,  NF_TIME_PARA2_T2(0x1));			//Set flash timming T2	
	REG_WRITE_U32(REG_TIME_PARA1,  NF_TIME_PARA1_T1(0x1));			//Set flash timming T1

    for(i=4;i<8;i++){
        printk("[%s] enter set REG_DATA_TL0 to 0x1, addr=0x%x\n", __FUNCTION__, i);
	//REG_WRITE_U32(REG_DATA_TL0, 0x1);	//Set data transfer count[7:0]
	//REG_WRITE_U32(REG_DATA_TL1, NF_DATA_TL1_access_mode(0x1) | NF_DATA_TL1_length1(0x0));	//Set data transfer count[13:8], SRAM path and access mode 

	//Set PP
	//REG_WRITE_U32(REG_READ_BY_PP, NF_READ_BY_PP_read_by_pp(0)); //Data read to 	DRAM from NAND through PP		
	//REG_WRITE_U32(REG_PP_CTL0, NF_PP_CTL0_pp_start_addr(0x0) | NF_PP_CTL0_pp_reset(0x0) | NF_PP_CTL0_pp_enable(0x1));	//Set PP starting address[9:8], PP reset and PP enable
	//REG_WRITE_U32(REG_PP_CTL1, 0x0);	//Set PP starting assdress[7:0]

	//Set command
	REG_WRITE_U32(REG_ND_CMD, NF_ND_CMD_cmd(0x55));	//Set CMD1
	REG_WRITE_U32(REG_ND_CTL, NF_ND_CTL_xfer(1) | NF_ND_CTL_ecc_enable(0x0) | NF_ND_CTL_tran_mode(0x0));	//Enable Xfer, eanble ECC and set transfer mode 
	WAIT_DONE(REG_ND_CTL, 0x80, 0x00);	//Wait xfer done

	//Set address, 
	REG_WRITE_U32(REG_ND_PA0, i);	//Set PA[7:0]
	REG_WRITE_U32(REG_ND_PA2, NF_ND_PA2_addr_mode(0x7));	//Set address mode & PA[20:16]		
	REG_WRITE_U32(REG_ND_CTL, NF_ND_CTL_xfer(1) | NF_ND_CTL_ecc_enable(0x0) | NF_ND_CTL_tran_mode(0x1));	//Enable Xfer, eanble ECC and set transfer mode 
	WAIT_DONE(REG_ND_CTL, 0x80, 0x00);	//Wait xfer done
	
	//Reset PP
	REG_WRITE_U32(REG_READ_BY_PP, NF_READ_BY_PP_read_by_pp(0)); //Data read to 	DRAM from NAND through PP		
	REG_WRITE_U32(REG_PP_CTL0, NF_PP_CTL0_pp_start_addr(0x0) | NF_PP_CTL0_pp_reset(0x1) | NF_PP_CTL0_pp_enable(0x0));	//Set PP starting address[9:8], PP reset and PP enable

	//Set data
	REG_WRITE_U32(REG_SRAM_CTL,NF_SRAM_CTL_map_sel(1)|NF_SRAM_CTL_access_en(0x1) | NF_SRAM_CTL_mem_region(0x0));	//Enable direct access SRAM
	REG_WRITE_U32(REG_ND_PA0, P1);	//Set data
	REG_WRITE_U32(REG_SRAM_CTL, NF_SRAM_CTL_access_en(0x0));	//Disable direct access SRAM
	//Set PP
	REG_WRITE_U32(REG_DATA_TL0, 0x1);	//Set data transfer count[7:0]
	REG_WRITE_U32(REG_DATA_TL1, NF_DATA_TL1_access_mode(0x1) | NF_DATA_TL1_length1(0x0));	//Set data transfer count[13:8], SRAM path and access mode 
	REG_WRITE_U32(REG_PP_CTL0, NF_PP_CTL0_pp_start_addr(0x0) | NF_PP_CTL0_pp_reset(0) | NF_PP_CTL0_pp_enable(1));	//Set PP starting address[9:8], PP reset and PP enable
	//REG_WRITE_U32(REG_PP_CTL0, NF_PP_CTL0_pp_start_addr(0x0));				//Set PP starting address[9:8]
	REG_WRITE_U32(REG_ND_CTL, NF_ND_CTL_xfer(1) | NF_ND_CTL_ecc_enable(0) | NF_ND_CTL_tran_mode(0x3));	//Enable Xfer, eanble ECC and set transfer mode 	
	WAIT_DONE(REG_ND_CTL, 0xc0, 0x40);	//Wait xfer done
        }

        printk("[%s] close slow timer, cnt=0x%x\n", __FUNCTION__, P1);

	REG_WRITE_U32(0x18000038, 0x4);		//432/(N+1)	
	REG_WRITE_U32(REG_TIME_PARA3,  NF_TIME_PARA3_T3(0x0));			//Set flash timming T3	
	REG_WRITE_U32(REG_TIME_PARA2,  NF_TIME_PARA2_T2(0x0));			//Set flash timming T2	
	REG_WRITE_U32(REG_TIME_PARA1,  NF_TIME_PARA1_T1(0x0));			//Set flash timming T1
    
}	

//----------------------------------------------------------------------------
// add read-retry for TOSHIBA
void rtk_set_feature_toshiba_new(int P1){

        int i = 0;

	printk("[%s] set slow timer, cnt=0x%x\n", __FUNCTION__, P1);

	REG_WRITE_U32(0x18000038, 0xf);		//432/(N+1)	
	REG_WRITE_U32(REG_TIME_PARA3,  NF_TIME_PARA3_T3(0x1));			//Set flash timming T3	
	REG_WRITE_U32(REG_TIME_PARA2,  NF_TIME_PARA2_T2(0x1));			//Set flash timming T2	
	REG_WRITE_U32(REG_TIME_PARA1,  NF_TIME_PARA1_T1(0x1));			//Set flash timming T1

    for(i=4;i<9;i++){
        printk("[%s] enter set REG_DATA_TL0 to 0x1, addr=0x%x, data: 0x%x\n", __FUNCTION__, i,read_retry_toshiba_cnt_new[P1][i-4]);
	//REG_WRITE_U32(REG_DATA_TL0, 0x1);	//Set data transfer count[7:0]
	//REG_WRITE_U32(REG_DATA_TL1, NF_DATA_TL1_access_mode(0x1) | NF_DATA_TL1_length1(0x0));	//Set data transfer count[13:8], SRAM path and access mode 

	//Set PP
	//REG_WRITE_U32(REG_READ_BY_PP, NF_READ_BY_PP_read_by_pp(0)); //Data read to 	DRAM from NAND through PP		
	//REG_WRITE_U32(REG_PP_CTL0, NF_PP_CTL0_pp_start_addr(0x0) | NF_PP_CTL0_pp_reset(0x0) | NF_PP_CTL0_pp_enable(0x1));	//Set PP starting address[9:8], PP reset and PP enable
	//REG_WRITE_U32(REG_PP_CTL1, 0x0);	//Set PP starting assdress[7:0]

	//Set command
	REG_WRITE_U32(REG_ND_CMD, NF_ND_CMD_cmd(0x55));	//Set CMD1
	REG_WRITE_U32(REG_ND_CTL, NF_ND_CTL_xfer(1) | NF_ND_CTL_ecc_enable(0x0) | NF_ND_CTL_tran_mode(0x0));	//Enable Xfer, eanble ECC and set transfer mode 
	WAIT_DONE(REG_ND_CTL, 0x80, 0x00);	//Wait xfer done

	//Set address, 

        if(i==8)
            REG_WRITE_U32(REG_ND_PA0, 0x0d);	//Set PA[7:0]
        else
	    REG_WRITE_U32(REG_ND_PA0, i);	//Set PA[7:0]
	REG_WRITE_U32(REG_ND_PA2, NF_ND_PA2_addr_mode(0x7));	//Set address mode & PA[20:16]		
	REG_WRITE_U32(REG_ND_CTL, NF_ND_CTL_xfer(1) | NF_ND_CTL_ecc_enable(0x0) | NF_ND_CTL_tran_mode(0x1));	//Enable Xfer, eanble ECC and set transfer mode 
	WAIT_DONE(REG_ND_CTL, 0x80, 0x00);	//Wait xfer done
	
	//Reset PP
	REG_WRITE_U32(REG_READ_BY_PP, NF_READ_BY_PP_read_by_pp(0)); //Data read to 	DRAM from NAND through PP		
	REG_WRITE_U32(REG_PP_CTL0, NF_PP_CTL0_pp_start_addr(0x0) | NF_PP_CTL0_pp_reset(0x1) | NF_PP_CTL0_pp_enable(0x0));	//Set PP starting address[9:8], PP reset and PP enable

	//Set data
	REG_WRITE_U32(REG_SRAM_CTL,NF_SRAM_CTL_map_sel(1)|NF_SRAM_CTL_access_en(0x1) | NF_SRAM_CTL_mem_region(0x0));	//Enable direct access SRAM
	REG_WRITE_U32(REG_ND_PA0, read_retry_toshiba_cnt_new[P1][i-4]);	//Set data
	REG_WRITE_U32(REG_SRAM_CTL, NF_SRAM_CTL_access_en(0x0));	//Disable direct access SRAM
	//Set PP
	REG_WRITE_U32(REG_DATA_TL0, 0x1);	//Set data transfer count[7:0]
	REG_WRITE_U32(REG_DATA_TL1, NF_DATA_TL1_access_mode(0x1) | NF_DATA_TL1_length1(0x0));	//Set data transfer count[13:8], SRAM path and access mode 
	REG_WRITE_U32(REG_PP_CTL0, NF_PP_CTL0_pp_start_addr(0x0) | NF_PP_CTL0_pp_reset(0) | NF_PP_CTL0_pp_enable(1));	//Set PP starting address[9:8], PP reset and PP enable
	//REG_WRITE_U32(REG_PP_CTL0, NF_PP_CTL0_pp_start_addr(0x0));				//Set PP starting address[9:8]
	REG_WRITE_U32(REG_ND_CTL, NF_ND_CTL_xfer(1) | NF_ND_CTL_ecc_enable(0) | NF_ND_CTL_tran_mode(0x3));	//Enable Xfer, eanble ECC and set transfer mode 	
	WAIT_DONE(REG_ND_CTL, 0xc0, 0x40);	//Wait xfer done
        }

        printk("[%s] close slow timer, cnt=0x%x\n", __FUNCTION__, P1);

	REG_WRITE_U32(0x18000038, 0x4);		//432/(N+1)	
	REG_WRITE_U32(REG_TIME_PARA3,  NF_TIME_PARA3_T3(0x0));			//Set flash timming T3	
	REG_WRITE_U32(REG_TIME_PARA2,  NF_TIME_PARA2_T2(0x0));			//Set flash timming T2	
	REG_WRITE_U32(REG_TIME_PARA1,  NF_TIME_PARA1_T1(0x0));			//Set flash timming T1

        if(P1==4){
        	printk("[%s] enter 4th retry send 0xb3 command\n", __FUNCTION__);

        	//Set command
        	REG_WRITE_U32(REG_ND_CMD, NF_ND_CMD_cmd(0xb3));	//Set CMD1
        	REG_WRITE_U32(REG_ND_CTL, NF_ND_CTL_xfer(1) | NF_ND_CTL_ecc_enable(0x0) | NF_ND_CTL_tran_mode(0x0));	//Enable Xfer, eanble ECC and set transfer mode 
        	WAIT_DONE(REG_ND_CTL, 0x80, 0x00);	//Wait xfer done
        }
    
}	

//----------------------------------------------------------------------------
// add read-retry for Micron
void rtk_set_feature_micron(int P1){

	printk("[%s] enter \n", __FUNCTION__);
	
	REG_WRITE_U32(REG_DATA_TL0, 0x4);	//Set data transfer count[7:0]
	REG_WRITE_U32(REG_DATA_TL1, NF_DATA_TL1_access_mode(0x1) | NF_DATA_TL1_length1(0x0));	//Set data transfer count[13:8], SRAM path and access mode 

	//Set PP
	REG_WRITE_U32(REG_READ_BY_PP, NF_READ_BY_PP_read_by_pp(0)); //Data read to 	DRAM from NAND through PP		
	REG_WRITE_U32(REG_PP_CTL0, NF_PP_CTL0_pp_start_addr(0x0) | NF_PP_CTL0_pp_reset(0x0) | NF_PP_CTL0_pp_enable(0x1));	//Set PP starting address[9:8], PP reset and PP enable
	REG_WRITE_U32(REG_PP_CTL1, 0x0);	//Set PP starting assdress[7:0]

	//Set command
	REG_WRITE_U32(REG_ND_CMD, NF_ND_CMD_cmd(0xef));	//Set CMD1
	REG_WRITE_U32(REG_ND_CTL, NF_ND_CTL_xfer(1) | NF_ND_CTL_ecc_enable(0x0) | NF_ND_CTL_tran_mode(0x0));	//Enable Xfer, eanble ECC and set transfer mode 
	WAIT_DONE(REG_ND_CTL, 0x80, 0x00);	//Wait xfer done

	//Set address, 
	REG_WRITE_U32(REG_ND_PA0, 0x89);	//Set PA[7:0]
	REG_WRITE_U32(REG_ND_PA2, NF_ND_PA2_addr_mode(0x7));	//Set address mode & PA[20:16]		
	REG_WRITE_U32(REG_ND_CTL, NF_ND_CTL_xfer(1) | NF_ND_CTL_ecc_enable(0x0) | NF_ND_CTL_tran_mode(0x1));	//Enable Xfer, eanble ECC and set transfer mode 
	WAIT_DONE(REG_ND_CTL, 0x80, 0x00);	//Wait xfer done
	
	//Reset PP
	REG_WRITE_U32(REG_READ_BY_PP, NF_READ_BY_PP_read_by_pp(0)); //Data read to 	DRAM from NAND through PP		
	REG_WRITE_U32(REG_PP_CTL0, NF_PP_CTL0_pp_start_addr(0x0) | NF_PP_CTL0_pp_reset(0x1) | NF_PP_CTL0_pp_enable(0x0));	//Set PP starting address[9:8], PP reset and PP enable

	//Set data
	REG_WRITE_U32(REG_SRAM_CTL,NF_SRAM_CTL_map_sel(1)|NF_SRAM_CTL_access_en(0x1) | NF_SRAM_CTL_mem_region(0x0));	//Enable direct access SRAM
	REG_WRITE_U32(REG_ND_PA0, P1);	//Set data
	REG_WRITE_U32(REG_SRAM_CTL, NF_SRAM_CTL_access_en(0x0));	//Disable direct access SRAM
	//Set PP
	REG_WRITE_U32(REG_PP_CTL0, NF_PP_CTL0_pp_start_addr(0x0) | NF_PP_CTL0_pp_reset(0) | NF_PP_CTL0_pp_enable(1));	//Set PP starting address[9:8], PP reset and PP enable
	REG_WRITE_U32(REG_PP_CTL0, NF_PP_CTL0_pp_start_addr(0x0));				//Set PP starting address[9:8]
	REG_WRITE_U32(REG_ND_CTL, NF_ND_CTL_xfer(1) | NF_ND_CTL_ecc_enable(0) | NF_ND_CTL_tran_mode(0x3));	//Enable Xfer, eanble ECC and set transfer mode 	
	WAIT_DONE(REG_ND_CTL, 0xc0, 0x40);	//Wait xfer done
}	

//----------------------------------------------------------------------------
//Add read retrial mechanism
//static int nRun,regVal1,regVal2,regVal3,regVal4,nInc;
static void rtk_xfer_SetParameter(unsigned char val1,unsigned char val2,unsigned char val3,unsigned char val4)
{
#if RTK_NAND_INTERRUPT
			NF_RESET_IRQ;
			CLR_REG_NAND_ISR;
			g_nfFlag = NF_INIT;
#endif


	//rtk_xfer_GetParameter();
	REG_WRITE_U32(REG_DATA_TL0, 0x1);				//Set data transfer count[7:0]
	REG_WRITE_U32(REG_DATA_TL1, NF_DATA_TL1_access_mode(1) | NF_DATA_TL1_sram_path(0) | NF_DATA_TL1_length1(0x0));				//Set data transfer count[13:8], SRAM path and access mode

	//Transfer command
	REG_WRITE_U32(REG_ND_CMD, 0x36);				//Set CMD1
	REG_WRITE_U32(REG_ND_CTL, NF_ND_CTL_xfer(1) | NF_ND_CTL_ecc_enable(0) | NF_ND_CTL_tran_mode(0x0));				//Enable Xfer, eanble ECC and set transfer mode
	WAIT_DONE(REG_ND_CTL, 0x80, 0x00);					//Wait xfer done

	//Transfer Reg#1 address
	REG_WRITE_U32(REG_ND_PA0, 0xa7);				//Set PA[7:0]
	REG_WRITE_U32(REG_ND_PA2, NF_ND_PA2_addr_mode(0x7) | NF_ND_PA2_page_addr2(0x0));				//Set address mode & PA[20:16]
	REG_WRITE_U32(REG_ND_CTL, NF_ND_CTL_xfer(1) | NF_ND_CTL_ecc_enable(0) | NF_ND_CTL_tran_mode(0x1));				//Enable Xfer, eanble ECC and set transfer mode
	WAIT_DONE(REG_ND_CTL, 0x80, 0x00);					//Wait xfer done

	//Reset PP
	REG_WRITE_U32(REG_READ_BY_PP, NF_READ_BY_PP_read_by_pp(0));	//Data read to DRAM from NAND through PP
	REG_WRITE_U32(REG_PP_CTL0, NF_PP_CTL0_pp_start_addr(0x0) | NF_PP_CTL0_pp_reset(0x1) | NF_PP_CTL0_pp_enable(0x0));				//Set PP starting address[9:8], PP reset and PP enable

	//Set data
	REG_WRITE_U32(REG_SRAM_CTL,NF_SRAM_CTL_map_sel(1)|NF_SRAM_CTL_access_en(0x1) | NF_SRAM_CTL_mem_region(0x0));	//Enable direct access SRAM
	REG_WRITE_U32(REG_ND_PA0, val1);				//Set data
	REG_WRITE_U32(REG_SRAM_CTL,NF_SRAM_CTL_access_en(0x0));	//Disable direct access SRAM
	//Set PP
	REG_WRITE_U32(REG_PP_CTL0, NF_PP_CTL0_pp_start_addr(0x0) | NF_PP_CTL0_pp_reset(0) | NF_PP_CTL0_pp_enable(1));				//Set PP starting address[9:8], PP reset and PP enable
	REG_WRITE_U32(REG_PP_CTL0, NF_PP_CTL0_pp_start_addr(0x0));				//Set PP starting address[9:8]
	REG_WRITE_U32(REG_ND_CTL, NF_ND_CTL_xfer(1) | NF_ND_CTL_ecc_enable(0) | NF_ND_CTL_tran_mode(0x3));				//Enable Xfer, eanble ECC and set transfer mode
	WAIT_DONE(REG_ND_CTL, 0x80, 0x00);					//Wait xfer done
	//Transfer Reg#2 address
	REG_WRITE_U32(REG_ND_PA0, 0xad);				//Set PA[7:0]
	REG_WRITE_U32(REG_ND_CTL, NF_ND_CTL_xfer(1) | NF_ND_CTL_ecc_enable(0) | NF_ND_CTL_tran_mode(0x1));				//Enable Xfer, eanble ECC and set transfer mode
	WAIT_DONE(REG_ND_CTL, 0x80, 0x00);					//Wait xfer done

	//Reset PP
	REG_WRITE_U32(REG_PP_CTL0, NF_PP_CTL0_pp_start_addr(0x0) | NF_PP_CTL0_pp_reset(0x1) | NF_PP_CTL0_pp_enable(0x0));				//Set PP starting address[9:8], PP reset and PP enable
	//Set data
	REG_WRITE_U32(REG_SRAM_CTL,NF_SRAM_CTL_map_sel(1)|NF_SRAM_CTL_access_en(0x1) | NF_SRAM_CTL_mem_region(0x0));	//Enable direct access SRAM
	REG_WRITE_U32(REG_ND_PA0, val2);				//Set data
	REG_WRITE_U32(REG_SRAM_CTL,NF_SRAM_CTL_access_en(0x0));	//Disable direct access SRAM
	//Set PP
	REG_WRITE_U32(REG_PP_CTL0, NF_PP_CTL0_pp_start_addr(0x0) | NF_PP_CTL0_pp_reset(0) | NF_PP_CTL0_pp_enable(1));				//Set PP starting address[9:8], PP reset and PP enable
	REG_WRITE_U32(REG_PP_CTL0, NF_PP_CTL0_pp_start_addr(0x0));				//Set PP starting address[9:8]
	REG_WRITE_U32(REG_ND_CTL, NF_ND_CTL_xfer(1) | NF_ND_CTL_ecc_enable(0) | NF_ND_CTL_tran_mode(0x3));				//Enable Xfer, eanble ECC and set transfer mode
	WAIT_DONE(REG_ND_CTL, 0x80, 0x00);					//Wait xfer done


	//Transfer Reg#3 address
	REG_WRITE_U32(REG_ND_PA0, 0xae);				//Set PA[7:0]
	REG_WRITE_U32(REG_ND_CTL, NF_ND_CTL_xfer(1) | NF_ND_CTL_ecc_enable(0) | NF_ND_CTL_tran_mode(0x1));				//Enable Xfer, eanble ECC and set transfer mode
	WAIT_DONE(REG_ND_CTL, 0x80, 0x00);					//Wait xfer done

	//Reset PP
	REG_WRITE_U32(REG_PP_CTL0, NF_PP_CTL0_pp_start_addr(0x0) | NF_PP_CTL0_pp_reset(0x1) | NF_PP_CTL0_pp_enable(0x0));				//Set PP starting address[9:8], PP reset and PP enable
	//Set data
	REG_WRITE_U32(REG_SRAM_CTL,NF_SRAM_CTL_map_sel(1)|NF_SRAM_CTL_access_en(0x1) | NF_SRAM_CTL_mem_region(0x0));	//Enable direct access SRAM
	REG_WRITE_U32(REG_ND_PA0, val3);				//Set data
	REG_WRITE_U32(REG_SRAM_CTL,NF_SRAM_CTL_access_en(0x0));	//Disable direct access SRAM
	//Set PP
	REG_WRITE_U32(REG_PP_CTL0, NF_PP_CTL0_pp_start_addr(0x0) | NF_PP_CTL0_pp_reset(0) | NF_PP_CTL0_pp_enable(1));				//Set PP starting address[9:8], PP reset and PP enable
	REG_WRITE_U32(REG_PP_CTL0, NF_PP_CTL0_pp_start_addr(0x0));				//Set PP starting address[9:8]
	REG_WRITE_U32(REG_ND_CTL, NF_ND_CTL_xfer(1) | NF_ND_CTL_ecc_enable(0) | NF_ND_CTL_tran_mode(0x3));				//Enable Xfer, eanble ECC and set transfer mode
	WAIT_DONE(REG_ND_CTL, 0x80, 0x00);					//Wait xfer done


	//Transfer Reg#4 address
	REG_WRITE_U32(REG_ND_PA0, 0xaf);				//Set PA[7:0]
	REG_WRITE_U32(REG_ND_CTL, NF_ND_CTL_xfer(1) | NF_ND_CTL_ecc_enable(0) | NF_ND_CTL_tran_mode(0x1));				//Enable Xfer, eanble ECC and set transfer mode
	WAIT_DONE(REG_ND_CTL, 0x80, 0x00);					//Wait xfer done

	//Reset PP
	REG_WRITE_U32(REG_PP_CTL0, NF_PP_CTL0_pp_start_addr(0x0) | NF_PP_CTL0_pp_reset(0x1) | NF_PP_CTL0_pp_enable(0x0));				//Set PP starting address[9:8], PP reset and PP enable
	//Set data
	REG_WRITE_U32(REG_SRAM_CTL,NF_SRAM_CTL_map_sel(1)|NF_SRAM_CTL_access_en(0x1) | NF_SRAM_CTL_mem_region(0x0));	//Enable direct access SRAM
	REG_WRITE_U32(REG_ND_PA0, val4);				//Set data
	REG_WRITE_U32(REG_SRAM_CTL,NF_SRAM_CTL_access_en(0x0));	//Disable direct access SRAM
	//Set PP
	REG_WRITE_U32(REG_PP_CTL0, NF_PP_CTL0_pp_start_addr(0x0) | NF_PP_CTL0_pp_reset(0) | NF_PP_CTL0_pp_enable(1));				//Set PP starting address[9:8], PP reset and PP enable
	REG_WRITE_U32(REG_PP_CTL0, NF_PP_CTL0_pp_start_addr(0x0));				//Set PP starting address[9:8]
	REG_WRITE_U32(REG_ND_CTL, NF_ND_CTL_xfer(1) | NF_ND_CTL_ecc_enable(0) | NF_ND_CTL_tran_mode(0x3));				//Enable Xfer, eanble ECC and set transfer mode
	WAIT_DONE(REG_ND_CTL, 0x80, 0x00);					//Wait xfer done

	//Transfer command
	REG_WRITE_U32(REG_ND_CMD, 0x16);				//Set CMD1
	REG_WRITE_U32(REG_ND_CTL, NF_ND_CTL_xfer(1) | NF_ND_CTL_ecc_enable(0) | NF_ND_CTL_tran_mode(0x0));				//Enable Xfer, eanble ECC and set transfer mode
	WAIT_DONE(REG_ND_CTL, 0x80, 0x00);					//Wait xfer done
	//printk("Set and Get value...\n");
	//rtk_xfer_GetParameter();
//	printk("[%s]%d,%d,%d,%d\n",__FUNCTION__,val1,val2,val3,val4);
#if RTK_NAND_INTERRUPT
			NF_RESET_IRQ;
			CLR_REG_NAND_ISR;
			g_nfFlag = NF_INIT;
#endif

}
//----------------------------------------------------------------------------
static void rtk_xfer_GetParameter()
{
#if RTK_NAND_INTERRUPT
			NF_RESET_IRQ;
			CLR_REG_NAND_ISR;
			g_nfFlag = NF_INIT;
#endif

#if 0 //Get parameter from DRAM

	regVal1=(REG_READ_U32(0xb8007660)>>24)&0x000000ff ;
    regVal2=(REG_READ_U32(0xb8007660)>>16)&0x000000ff ;
    regVal3=(REG_READ_U32(0xb8007660)>>8)&0x000000ff ;
    regVal4=REG_READ_U32(0xb8007660)&0x000000ff ;


#else

	REG_WRITE_U32(REG_DATA_TL0, 0x1);				//Set data transfer count[7:0]
	REG_WRITE_U32(REG_DATA_TL1, NF_DATA_TL1_access_mode(1) | NF_DATA_TL1_sram_path(0) | NF_DATA_TL1_length1(0x0));				//Set data transfer count[13:8], SRAM path and access mode

	//Set PP
	REG_WRITE_U32(REG_READ_BY_PP, NF_READ_BY_PP_read_by_pp(0));	//Data read to DRAM from NAND through PP
	REG_WRITE_U32(REG_PP_CTL0, NF_PP_CTL0_pp_start_addr(0x0) | NF_PP_CTL0_pp_reset(0) | NF_PP_CTL0_pp_enable(1));				//Set PP starting address[9:8], PP reset and PP enable
	REG_WRITE_U32(REG_PP_CTL1, 0x0);				//Set PP starting assdress[7:0]
	//Set table sram
	//REG_WRITE_U32(NF_TABLE_CTL_reg, NF_TABLE_CTL_table_start_addr(0x0) | NF_TABLE_CTL_table_enable(0));				//Set table SRAM starting assdress ( 0 ~ A) and enable table SRAM

	//Transfer command
	REG_WRITE_U32(REG_ND_CMD, 0x37);				//Set CMD1
	REG_WRITE_U32(REG_ND_CTL, NF_ND_CTL_xfer(1) | NF_ND_CTL_ecc_enable(0) | NF_ND_CTL_tran_mode(0x0));				//Enable Xfer, eanble ECC and set transfer mode
	WAIT_DONE(REG_ND_CTL, 0x80, 0x00);					//Wait xfer done
//test hang

	//Transfer Reg#1 address
	REG_WRITE_U32(REG_ND_PA0, 0xa7);
	REG_WRITE_U32(REG_ND_PA2, NF_ND_PA2_addr_mode(0x7) | NF_ND_PA2_page_addr2(0x0));	//Set reg#1
	REG_WRITE_U32(REG_ND_CTL, NF_ND_CTL_xfer(1) | NF_ND_CTL_ecc_enable(0) | NF_ND_CTL_tran_mode(0x1));				//Enable Xfer, eanble ECC and set transfer mode
	WAIT_DONE(REG_ND_CTL, 0x80, 0x00);					//Wait xfer done
//test ok
	//Get data
	REG_WRITE_U32(REG_ND_CTL, NF_ND_CTL_xfer(1) | NF_ND_CTL_ecc_enable(0) | NF_ND_CTL_tran_mode(0x2));				//Enable Xfer, eanble ECC and set transfer mode
	WAIT_DONE(REG_ND_CTL, 0x80,0x00);					//Wait xfer done
	REG_WRITE_U32((unsigned long)&regVal1, REG_READ_U32(REG_ND_DAT));
//test err


	//Transfer Reg#2 address
	REG_WRITE_U32(REG_ND_PA0, 0xad);				//Set PA[7:0]
	REG_WRITE_U32(REG_ND_CTL, NF_ND_CTL_xfer(1) | NF_ND_CTL_ecc_enable(0) | NF_ND_CTL_tran_mode(0x1));				//Enable Xfer, eanble ECC and set transfer mode
	WAIT_DONE(REG_ND_CTL, 0x80, 0x00);					//Wait xfer done

	//Get data
	REG_WRITE_U32(REG_ND_CTL, NF_ND_CTL_xfer(1) | NF_ND_CTL_ecc_enable(0) | NF_ND_CTL_tran_mode(0x2));				//Enable Xfer, eanble ECC and set transfer mode
	WAIT_DONE(REG_ND_CTL, 0x80, 0x00);					//Wait xfer done
	REG_WRITE_U32((unsigned long)&regVal2, REG_READ_U32(REG_ND_DAT));

	//Transfer Reg#3 address
	REG_WRITE_U32(REG_ND_PA0, 0xae);				//Set PA[7:0]
	REG_WRITE_U32(REG_ND_CTL, NF_ND_CTL_xfer(1) | NF_ND_CTL_ecc_enable(0) | NF_ND_CTL_tran_mode(0x1));				//Enable Xfer, eanble ECC and set transfer mode
	WAIT_DONE(REG_ND_CTL, 0x80, 0x00);					//Wait xfer done

	//Get data
	REG_WRITE_U32(REG_ND_CTL, NF_ND_CTL_xfer(1) | NF_ND_CTL_ecc_enable(0) | NF_ND_CTL_tran_mode(0x2));				//Enable Xfer, eanble ECC and set transfer mode
	WAIT_DONE(REG_ND_CTL, 0x80,0x00);					//Wait xfer done
	REG_WRITE_U32((unsigned long)&regVal3, REG_READ_U32(REG_ND_DAT));


	//Transfer Reg#4 address
	REG_WRITE_U32(REG_ND_PA0, 0xaf);				//Set PA[7:0]
	REG_WRITE_U32(REG_ND_CTL, NF_ND_CTL_xfer(1) | NF_ND_CTL_ecc_enable(0) | NF_ND_CTL_tran_mode(0x1));				//Enable Xfer, eanble ECC and set transfer mode
	WAIT_DONE(REG_ND_CTL, 0x80, 0x00);					//Wait xfer done

	//Get data
	REG_WRITE_U32(REG_ND_CTL, NF_ND_CTL_xfer(1) | NF_ND_CTL_ecc_enable(0) | NF_ND_CTL_tran_mode(0x2));				//Enable Xfer, eanble ECC and set transfer mode
	WAIT_DONE(REG_ND_CTL, 0x80, 0x00);					//Wait xfer done
	REG_WRITE_U32((unsigned long)&regVal4, REG_READ_U32(REG_ND_DAT));

	//Reset PP
	REG_WRITE_U32(REG_PP_CTL0, NF_PP_CTL0_pp_start_addr(0x0) | NF_PP_CTL0_pp_reset(0x1) | NF_PP_CTL0_pp_enable(0x0)); 			//Set PP starting address[9:8], PP reset and PP enable
	#endif

//	printk("[%s]%d,%d,%d,%d\n",__FUNCTION__,regVal1,regVal2,regVal3,regVal4);
#if RTK_NAND_INTERRUPT
			NF_RESET_IRQ;
			CLR_REG_NAND_ISR;

			g_nfFlag = NF_INIT;
#endif

}
//----------------------------------------------------------------------------
#if 0
static void rtk_SetVal( char val1, char val2, char val3, char val4)
{
	rtk_xfer_GetParameter();
	regVal1+=val1;
	regVal2+=val2;
	regVal3+=val3;
	regVal4+=val4;
	rtk_xfer_SetParameter(regVal1,regVal2,regVal3,regVal4);
}
#endif
//----------------------------------------------------------------------------
static void rtk_SetReadRetrialMode(unsigned int run)
{

	if((regVal1==0)&&(regVal2==0)&&(regVal3==0)&&(regVal4==0))
		rtk_xfer_GetParameter();
	run%=7;

	if(run!=nRun)
		printk("\n\n$$run %d,[Run %d]\n",run,nRun);
	switch(run)
	{

	#if 1
		case 1:
			//rtk_SetVal(0x00,0x06,0x0A,0x06);
			rtk_xfer_SetParameter(regVal1,regVal2+0x06,regVal3+0x0A,regVal4+0x06);

		break;

		case 2:
			//rtk_SetVal(0xff,-0x03,-0x07,-0x08);
			rtk_xfer_SetParameter(0x00,regVal2-0x03,regVal3-0x07,regVal4-0x08);

		break;

		case 3:
			//rtk_SetVal(0xff,-0x06,-0x0D,-0x0F);
			rtk_xfer_SetParameter(0x00,regVal2-0x06,regVal3-0x0D,regVal4-0x0F);

		break;

		case 4:
			//rtk_SetVal(0xff,-0x09,-0x14,-0x17);
			rtk_xfer_SetParameter(0x00,regVal2-0x09,regVal3-0x14,regVal4-0x17);
		break;

		case 5:
			//rtk_SetVal(0xff,0xff,-0x1A,-0x1E);
			rtk_xfer_SetParameter(0x00,0x00,regVal3-0x1A,regVal4-0x1E);

		break;

		case 6:

			//rtk_SetVal(0xff,0xff,-0x20,-0x25);
			rtk_xfer_SetParameter(0x00,0x00,regVal3-0x20,regVal4-0x25);

		break;
#endif
		default:
			//rtk_xfer_SetParameter(regVal1,regVal2,regVal3,regVal4);
		break;
	}
}
//----------------------------------------------------------------------------
static int rtk_read_ecc_page_reTrial (struct mtd_info *mtd, u16 chipnr, unsigned int page, u_char *data_buf, u_char *oob_buf)
{

	struct nand_chip *this = NULL;
	int rc = 0;
	int dram_sa, dma_len, spare_dram_sa;
	int blank_all_one = 0;
	int page_len;

	unsigned int chip_section = 0;
	unsigned int section = 0;
	unsigned int index = 0;
	uint8_t	auto_trigger_mode = 2;
	uint8_t	addr_mode = 1;
	
	printk("[%s] enter \n",__FUNCTION__);
	
	#if RTK_NAND_INTERRUPT
		NF_RESET_IRQ;
		NF_ENABLE_IRQ(NF_DMA_IRQ_MASK);
		g_nfFlag = NF_READ_TRIGGER;
#endif
	nInc=0;
	while(nInc<7)
	{


		rtk_SetReadRetrialMode(nInc+nRun);

		this = (struct nand_chip *) mtd->priv;
		page_size = mtd->oobblock;
		oob_size = mtd->oobsize;
		ppb = mtd->erasesize/mtd->oobblock;

		if ( chipnr == 0 && page < (((g_BootcodeSize+g_Factory_param_size)/page_size)+ppb))
		{
			mtd->isCPdisable_R = 1;
			mtd->isScramble=MTD_SCRAMBLE_DISABLE;
		}
		else
			mtd->isScramble=1;

		if((mtd->ecctype==MTD_ECC_RTK_HW)&&(this->ecc_select==0x18||this->ecc_select==0x0c))
		{
			if(!oob_buf)
			{
				rc = rtk_read_ecc_page_withLargeECC(mtd,chipnr,page,data_buf,this->g_oobbuf);
			}
			else
				rc = rtk_read_ecc_page_withLargeECC(mtd,chipnr,page,data_buf,oob_buf);
			if(rc==0)
			{
				nRun+=nInc;
				nRun%=7;
				return rc;
			}
			else
			{
				nInc++;
				rc=-1;
				continue;
			}
		}

		//REG_WRITE_U32(0x01,0xb801032c);	//Enable NAND/CardReader arbitrator add by alexchang 0303-2010
		REG_WRITE_U32(REG_BLANK_CHK,NF_BLANK_CHK_blank_ena(1) );
		REG_WRITE_U32(REG_DATA_TL0,NF_DATA_TL0_length0(0));

		if(this->ecc_select >= 0x18)
		{
			page_len = page_size >> 10;
			REG_WRITE_U32(REG_DATA_TL1,NF_DATA_TL1_access_mode(1)|NF_DATA_TL1_length1(4));
		}
		else
		{
			page_len = page_size >> 9;
			REG_WRITE_U32(REG_DATA_TL1,NF_DATA_TL1_access_mode(1)|NF_DATA_TL1_length1(2));
		}
		REG_WRITE_U32(REG_PAGE_LEN,NF_PAGE_LEN_page_len(page_len));

		REG_WRITE_U32(REG_READ_BY_PP,NF_READ_BY_PP_read_by_pp(1));
		REG_WRITE_U32(REG_PP_CTL1,NF_PP_CTL1_pp_start_addr(0));
		REG_WRITE_U32(REG_PP_CTL0,0);

		REG_WRITE_U32(REG_ND_CMD,NF_ND_CMD_cmd(CMD_PG_READ_C1));
		REG_WRITE_U32(REG_CMD2,NF_CMD2_cmd2(CMD_PG_READ_C2));
		REG_WRITE_U32(REG_CMD3,NF_CMD3_cmd3(CMD_PG_READ_C3));

		REG_WRITE_U32( REG_ND_PA0,NF_ND_PA0_page_addr0(page));
		REG_WRITE_U32( REG_ND_PA1,NF_ND_PA1_page_addr1(page>>8));
		REG_WRITE_U32( REG_ND_PA2,NF_ND_PA2_addr_mode(addr_mode)|NF_ND_PA2_page_addr2(page>>16));
		REG_WRITE_U32( REG_ND_PA3,NF_ND_PA3_page_addr3((page>>21)&0x7));
		REG_WRITE_U32(REG_MULTI_CHNL_MODE,NF_MULTI_CHNL_MODE_edo(1));//add by alexchang0205-2010
		REG_WRITE_U32( REG_ECC_STOP,NF_ECC_STOP_ecc_n_stop(0x01));

		switch(this->ecc_select) {
			case 6:
				REG_WRITE_U32(REG_ECC_SEL, 0x0);
				break;
			case 12:
				REG_WRITE_U32(REG_ECC_SEL, 0x1);
				break;
			case 16:
				REG_WRITE_U32(REG_ECC_SEL, 0xe);
				break;
			case 24:
				REG_WRITE_U32(REG_ECC_SEL, 0xa);
				break;
			case 40:
				REG_WRITE_U32(REG_ECC_SEL, 0x2);
				break;
			case 43:
				REG_WRITE_U32(REG_ECC_SEL, 0x4);
				break;
			case 65:
				REG_WRITE_U32(REG_ECC_SEL, 0x6);
				break;
			case 72:
				REG_WRITE_U32(REG_ECC_SEL, 0x8);
				break;
			default:
				REG_WRITE_U32(REG_ECC_SEL, 0x0);
				break;
		}

		dram_sa = ( (uint32_t)data_buf >> 3);
		REG_WRITE_U32(REG_DMA_CTL1,NF_DMA_CTL1_dram_sa(dram_sa));
		dma_len = page_size >> 9;
		REG_WRITE_U32(REG_DMA_CTL2,NF_DMA_CTL2_dma_len(dma_len));
		REG_WRITE_U32(REG_DMA_CTL3,NF_DMA_CTL3_ddr_wr(1)|NF_DMA_CTL3_dma_xfer(1));
		
		if (oob_buf)
			spare_dram_sa = ( (uint32_t)oob_buf >> 3);
		else
			spare_dram_sa = ( (uint32_t)this->g_oobbuf >> 3);

		REG_WRITE_U32( REG_SPR_DDR_CTL,NF_SPR_DDR_CTL_spare_ddr_ena(1)|NF_SPR_DDR_CTL_per_2k_spr_ena(1)|NF_SPR_DDR_CTL_spare_dram_sa(spare_dram_sa));
			


#if RTK_NAND_INTERRUPT
		NF_RESET_IRQ;
		NF_ENABLE_IRQ(NF_DMA_IRQ_MASK);
		g_nfFlag = NF_READ_TRIGGER;
#endif
		//printk("Retry[%s]Ran 0x%x, scr 0x%x, flag 0x%x\n",__FUNCTION__,g_isRandomize,mtd->isScramble,mtd->isCPdisable_R);
		//printk("R blk:%u, page:%u\n",page/ppb,page%ppb);

		if((g_isRandomize&&(!mtd->isCPdisable_R))||(mtd->isScramble!=MTD_SCRAMBLE_DISABLE))
		{
			//printk("Ran 0x%x\n",g_isRandomize);
			//printk("isCPdisable_R 0x%x\n",mtd->isCPdisable_R);
			//printk("isScramble 0x%x\n",mtd->isScramble);
	                //printk("page 0x%x\n",page);
			//printk("@");
			if(u32CP_mode==0)//CBC initial key setting.
			{
				REG_WRITE_U32( REG_CP_NF_INI_KEY,page);
			}
			REG_WRITE_U32(REG_CP_NF_KEY,0x12345678);
			REG_WRITE_U32((REG_CP_NF_KEY+0x4),0x12345678);
			REG_WRITE_U32((REG_CP_NF_KEY+0x8),0x12345678);
			REG_WRITE_U32((REG_CP_NF_KEY+0xc),0x12345678);

			REG_WRITE_U32(REG_CP_NF_SET,u32CP_mode|cpSel);
			REG_WRITE_U32(REG_CP_LEN,NF_CP_LEN_cp_length(page_size));
			RTK_FLUSH_CACHE((unsigned long) data_buf, page_size);
			if ( oob_buf )
				RTK_FLUSH_CACHE((unsigned long) oob_buf, oob_size);
			else
				RTK_FLUSH_CACHE((unsigned long) this->g_oobbuf, oob_size);
			REG_WRITE_U32(REG_DMA_CTL3,NF_DMA_CTL3_cp_enable(0x1)|NF_DMA_CTL3_cp_first(0x1)|NF_DMA_CTL3_ddr_wr(0x1)|NF_DMA_CTL3_dma_xfer(0x1));
		}
		else
		{

			if(is_NF_CP_Enable_read&&(!mtd->isCPdisable_R))
			{
				//printk("X");
				REG_WRITE_U32(REG_CP_NF_KEY,*regKey0);
				REG_WRITE_U32((REG_CP_NF_KEY+0x4),*regKey1);
				REG_WRITE_U32((REG_CP_NF_KEY+0x8),*regKey2);
				REG_WRITE_U32((REG_CP_NF_KEY+0xc),*regKey3);

				REG_WRITE_U32(REG_CP_NF_SET,u32CP_mode|cpSel);
				REG_WRITE_U32(REG_CP_LEN,NF_CP_LEN_cp_length(page_size));
				RTK_FLUSH_CACHE((unsigned long) data_buf, page_size);
				if ( oob_buf )
					RTK_FLUSH_CACHE((unsigned long) oob_buf, oob_size);
				else
					RTK_FLUSH_CACHE((unsigned long) this->g_oobbuf, oob_size);
				REG_WRITE_U32(REG_DMA_CTL3,NF_DMA_CTL3_cp_enable(0x1)|NF_DMA_CTL3_cp_first(0x1)|NF_DMA_CTL3_ddr_wr(0x1)|NF_DMA_CTL3_dma_xfer(0x1));
			}
			else
			{//printk("/");
				RTK_FLUSH_CACHE((unsigned long) data_buf, page_size);
				if ( oob_buf )
					RTK_FLUSH_CACHE((unsigned long) oob_buf, oob_size);
				else
					RTK_FLUSH_CACHE((unsigned long) this->g_oobbuf, oob_size);
				REG_WRITE_U32(REG_DMA_CTL3,NF_DMA_CTL3_cp_enable(0)|NF_DMA_CTL3_cp_first(0)|NF_DMA_CTL3_ddr_wr(0x1)|NF_DMA_CTL3_dma_xfer(0x1));
			}
		}

		REG_WRITE_U32( REG_AUTO_TRIG,NF_AUTO_TRIG_auto_trig(1)|NF_AUTO_TRIG_spec_auto_case(0)| NF_AUTO_TRIG_auto_case(auto_trigger_mode));

#if RTK_NAND_INTERRUPT
		RTK_WAIT_EVENT;
#else
		WAIT_DONE(REG_AUTO_TRIG,0x80,0);
		WAIT_DONE(REG_DMA_CTL3,0x01,0);
#endif


		if((is_NF_CP_Enable_read&&(!mtd->isCPdisable_R))||g_isRandomize)//Clear register
		{
			REG_WRITE_U32(REG_CP_NF_SET,0);
		}

		if(oob_buf)
		{
			if(this->ecc_select==0x18||this->ecc_select==0x0c)
				reverse_to_Tags(oob_buf,this->ecc_select);
			else
			{
				REG_WRITE_U32(REG_READ_BY_PP,0x00);
				if(is_jupiter_cpu()||is_macarthur_cpu()||is_nike_cpu())
					REG_WRITE_U32(REG_SRAM_CTL, 0x30 | 0x02);
				else
					REG_WRITE_U32(REG_SRAM_CTL, 0x30 | 0x04);
				rtk_read_oob_from_SRAM(mtd, oob_buf);
				REG_WRITE_U32(REG_SRAM_CTL, 0x00);
				REG_WRITE_U32(REG_READ_BY_PP,0x80);
			}
		}

		chip_section = (chipnr * this->page_num) >> 5;
		section = page >> 5;
		index = page & (32-1);
		blank_all_one = (REG_READ_U32(REG_BLANK_CHK)>>1) & 0x01;


		// add by alexchang 01-28-2011
		mtd->nEccMaxValue = REG_MAX_ECC_NUM_max_ecc_num(REG_READ_U32(REG_MAX_ECC_NUM));
		if (blank_all_one)
		{
			if(this->erase_page_flag)
			this->erase_page_flag[chip_section+section] =  (1<< index);
		}

		if (REG_READ_U32(REG_ND_ECC) & 0x0C){
					if(!this->erase_page_flag)
					{
						if (REG_READ_U32(REG_ND_ECC) & 0x08){
							if ( chipnr == 0 && page < g_BootcodeSize/page_size )
							{
								nRun+=nInc;
								nRun%=7;
							    return 0;
							}
							else
							{
								//printk("0[%s]nInc %d,nRun %d\n",__FUNCTION__,nInc,nRun);
								nInc++;
								rc=-1;
								printk("!!!Read Fail page NO. 0x%x\n",page);
								continue;
							}
						}
					}
					else
					{
						if (this->erase_page_flag[chip_section+section] & (1<< index) ){
							;
						}else
						{
							if (REG_READ_U32(REG_ND_ECC) & 0x08){
								if ( chipnr == 0 && page < g_BootcodeSize/page_size )
								{
									nRun+=nInc;
									nRun%=7;
									return 0;
								}
								else
								{
									//printk("1[%s]nInc %d,nRun %d\n",__FUNCTION__,nInc,nRun);
									nInc++;
									rc=-1;
									printk("!!!Read Fail page NO. 0x%x\n",page);
									continue;
								}
							}
						}
					}
				}

#if RTK_ARD_ALGORITHM
		whichBlk = page / ppb;
		g_RecArray[whichBlk]++;
		if(g_WinTrigCnt > g_WinTrigThrshld)
			travelProcWindow();
#endif
		mtd->isCPdisable_R = 0;
		rc=0;
		break;

	}
	//printk("return rc %d\n",rc);
	nRun+=nInc;
	nRun%=7;

	return rc;
}


//----------------------------------------------------------------------------


static int rtk_erase_block(struct mtd_info *mtd, u16 chipnr, int page)
{
	struct nand_chip *this = NULL;
	unsigned int chip_section = 0;
	unsigned int section = 0;
	unsigned int retryCnt = RETRY_COUNT;
	page_size = mtd->oobblock;
	oob_size = mtd->oobsize;
	ppb = mtd->erasesize/mtd->oobblock;
//printk("erasesize 0x%x,oobblock 0x%x\n",mtd->erasesize,mtd->oobblock);
//printk("AT[%s]:show chipnr %d\n",__FUNCTION__,chipnr);

	//while (down_interruptible (&sem_NF_CARDREADER)) {
	//	printk("%s : Retry [Address: 0x%x]\n",__FUNCTION__,page);
	//	if(retryCnt<=0)
	//	{
	//		printk("%s : Retry fail [ADDR 0x%x]\n",__FUNCTION__,page);
	//		return -ERESTARTSYS;
	//	}
	//	retryCnt--;
	//}
	this = (struct nand_chip *) mtd->priv;
//	printk("AT[%s]:page %d, ppb %d, mtd->erasesize %u, mtd->oobblock %u\n",__FUNCTION__,page,ppb,mtd->erasesize,mtd->oobblock);
	if ( page & (ppb-1) ){
		printk("%s: page %d is not block alignment !!\n", __FUNCTION__, page);
		//up (&sem_NF_CARDREADER);
		return -1;
	}

	//if(is_darwin_cpu()||is_macarthur_cpu()||is_nike_cpu())//Disable NF_WP pin (Write Protect Pin)
	//{
	//	if(g_WP_en)
	//	{
	//		setGPIOBit(g_regGPIODirBase,g_NFWP_no,NF_GPIO_OUT);//Set Driection
	//		setGPIOBit(g_regGPIOOutBase,g_NFWP_no,g_NFWP_value_dis);//Set Value
	//	}
	//}

#if RTK_NAND_INTERRUPT
	g_nfFlag = NF_ERASE_TRIGGER;
	NF_RESET_IRQ;
#endif

	//REG_WRITE_U32(0x01,0xb801032c);	//Enable NAND/CardReader arbitrator add by alexchang 0303-2010
	REG_WRITE_U32( REG_MULTI_CHNL_MODE,NF_MULTI_CHNL_MODE_no_wait_busy(1)|NF_MULTI_CHNL_MODE_edo(1));

	REG_WRITE_U32( REG_ND_CMD,NF_ND_CMD_cmd(CMD_BLK_ERASE_C1) );
	REG_WRITE_U32( REG_CMD2,NF_CMD2_cmd2(CMD_BLK_ERASE_C2) );
	REG_WRITE_U32( REG_CMD3,NF_CMD3_cmd3(CMD_BLK_ERASE_C3) );

	REG_WRITE_U32( REG_ND_PA0,NF_ND_PA0_page_addr0(page) );
	REG_WRITE_U32( REG_ND_PA1,NF_ND_PA1_page_addr1(page>>8) );
	REG_WRITE_U32( REG_ND_PA2,NF_ND_PA2_addr_mode(0x04)|NF_ND_PA2_page_addr2(page>>16) );
	REG_WRITE_U32( REG_ND_PA3,NF_ND_PA3_page_addr3((page>> 21)&0x7) );

	REG_WRITE_U32( REG_AUTO_TRIG,NF_AUTO_TRIG_auto_trig(1)|NF_AUTO_TRIG_spec_auto_case(1)|NF_AUTO_TRIG_auto_case(2) );
	WAIT_DONE(REG_AUTO_TRIG,0x80,0);

#if RTK_NAND_INTERRUPT
	NF_RESET_IRQ;
	NF_ENABLE_IRQ(NF_POLL_IRQ_MASK);
	g_nfFlag = NF_ERASE_TRIGGER;
	REG_WRITE_U32(REG_POLL_FSTS,NF_POLL_FSTS_bit_sel(6)|NF_POLL_FSTS_trig_poll(1) );
	RTK_WAIT_EVENT;
	NF_RESET_IRQ;
	NF_ENABLE_IRQ(NF_IRQ_MASK);
#else
	REG_WRITE_U32(REG_POLL_FSTS,NF_POLL_FSTS_bit_sel(6)|NF_POLL_FSTS_trig_poll(1) );
	WAIT_DONE(REG_POLL_FSTS,0x01,0x0);
	WAIT_DONE(REG_ND_CTL,0x40,0x40);//add by alexchang 0416-2010
#endif

	//if(is_darwin_cpu()||is_macarthur_cpu()||is_nike_cpu())//Enable NF_WP pin (Write Protect Pin)
	//{
	//	if(g_WP_en)
	//	{
	//		setGPIOBit(g_regGPIODirBase,g_NFWP_no,NF_GPIO_OUT);//Set Driection
	//		setGPIOBit(g_regGPIOOutBase,g_NFWP_no,g_NFWP_value_en);//Set Value
	//	}
	//}

	if ( REG_READ_U32(REG_ND_DAT) & 0x01 ){
		//up (&sem_NF_CARDREADER);
		printk("[%s] erasure is not completed at block %d\n", __FUNCTION__, page/ppb);
		if ( chipnr == 0 && page >= 0 && page < g_BootcodeSize/page_size )
			return 0;
		else
			return -1;
	}

	chip_section = (chipnr * this->page_num) >> 5;
	section = page >> 5;
	if(this->erase_page_flag)
		memset ( (__u32 *)(this->erase_page_flag)+ chip_section + section, 0xff, ppb>>3);
	//printk("[AT]Erase block : %u  \n",page/ppb);
	//up (&sem_NF_CARDREADER);
#if RTK_ARD_ALGORITHM
	g_RecArray[page/ppb]=0;
//	printk("[AT]Erase block : %u  \n",page/ppb);
#endif
	return 0;
}
//----------------------------------------------------------------------------
unsigned char *malloc_aligned(size_t size,int alignment)
{
   unsigned char *ptr = kmalloc(size + alignment-1,GFP_KERNEL);
   if (!ptr) return NULL;
   ptr = (unsigned char *)(((unsigned int)(ptr) + alignment-1) & ~alignment);
   return ptr;
}
//----------------------------------------------------------------------------

static int rtk_read_oob (struct mtd_info *mtd, u16 chipnr, int page, int len, u_char *oob_buf)
{
	struct nand_chip *this = (struct nand_chip *) mtd->priv;
	int rc = 0;
	int dram_sa, dma_len, spare_dram_sa;
	int blank_all_one=0;
	int page_len;
	unsigned int chip_section = 0;
	unsigned int section = 0;
	unsigned int index = 0;
	unsigned int retryCnt = RETRY_COUNT;
	uint8_t auto_trigger_mode = 2;
	uint8_t addr_mode = 1;
	uint8_t	bChkAllOne = 0;
	
	page_size = mtd->oobblock;
	oob_size = mtd->oobsize;
	ppb = mtd->erasesize/mtd->oobblock;

	if ( len>oob_size || !oob_buf ){
		printk("[%s] error: len>oob_size OR oob_buf is NULL\n", __FUNCTION__);
		//up (&sem_NF_CARDREADER);
		return -1;
	}
	if ( chipnr == 0 && page >= 0 && page < g_BootcodeSize/page_size )
		mtd->isCPdisable_R = 1;

	while (1)
	{
		REG_WRITE_U32(REG_DATA_TL0,NF_DATA_TL0_length0(0));
		if(this->ecc_select >= 0x18)
		{
			if (bChkAllOne) {
				// enable randomizer
				REG_WRITE_U32(REG_RMZ_CTRL, 0);
				REG_WRITE_U32(REG_BLANK_CHK,NF_BLANK_CHK_read_ecc_xnor_ena(1));
			}
			else {
				// enable randomizer
				REG_WRITE_U32(REG_RMZ_CTRL, 1);
				REG_WRITE_U32(REG_BLANK_CHK,NF_BLANK_CHK_read_ecc_xnor_ena(0));
			}
			page_len = page_size >> 10;
			REG_WRITE_U32(REG_DATA_TL1,NF_DATA_TL1_access_mode(1)|NF_DATA_TL1_length1(4));
		}
		else
		{
			// set random read			
			REG_WRITE_U32(REG_RND_EN, 1);
			REG_WRITE_U32(REG_RND_CMD1, 0x5);
			REG_WRITE_U32(REG_RND_CMD2, 0xe0);
			REG_WRITE_U32(REG_RND_DATA_STR_COL_H, 0);	// data start address MSB (always 0)
			REG_WRITE_U32(REG_RND_SPR_STR_COL_H, page_size >> 8);	// spare start address MSB
			REG_WRITE_U32(REG_RND_SPR_STR_COL_L, page_size & 0xff);	// spare start address LSB
		
			page_len = page_size >> 9;
			REG_WRITE_U32(REG_DATA_TL1,NF_DATA_TL1_access_mode(1)|NF_DATA_TL1_length1(2));
		}
		
		REG_WRITE_U32(REG_PAGE_LEN,NF_PAGE_LEN_page_len(page_len));

		//Set PP
		REG_WRITE_U32(REG_READ_BY_PP,NF_READ_BY_PP_read_by_pp(1));
		REG_WRITE_U32(REG_PP_CTL1,NF_PP_CTL1_pp_start_addr(0));
		REG_WRITE_U32(REG_PP_CTL0,0);

		// enable blank check
		REG_WRITE_U32(REG_BLANK_CHK,NF_BLANK_CHK_blank_ena(1) );

		//Set command
		REG_WRITE_U32(REG_ND_CMD,NF_ND_CMD_cmd(CMD_PG_READ_C1));
		REG_WRITE_U32(REG_CMD2,NF_CMD2_cmd2(CMD_PG_READ_C2));
		REG_WRITE_U32(REG_CMD3,NF_CMD3_cmd3(CMD_PG_READ_C3));
		
		//Set address
		REG_WRITE_U32( REG_ND_PA0,NF_ND_PA0_page_addr0( 0xff&page ));
		REG_WRITE_U32( REG_ND_PA1,NF_ND_PA1_page_addr1( 0xff&(page>>8) ));
		REG_WRITE_U32( REG_ND_PA2,NF_ND_PA2_addr_mode(addr_mode)|NF_ND_PA2_page_addr2( 0x1f&(page>>16) ));
		REG_WRITE_U32( REG_ND_PA3,NF_ND_PA3_page_addr3( 0x7&(page>>21) ));

		REG_WRITE_U32(REG_ND_CA0, 0);
		REG_WRITE_U32(REG_ND_CA1, 0);

		//Set ECC
		REG_WRITE_U32( REG_MULTI_CHNL_MODE,NF_MULTI_CHNL_MODE_edo(1));//add by alexchang0205-2010
		REG_WRITE_U32( REG_ECC_STOP,NF_ECC_STOP_ecc_n_stop(0x01));

		switch(this->ecc_select) {
			case 6:
				REG_WRITE_U32(REG_ECC_SEL, 0x0);
				break;
			case 12:
				REG_WRITE_U32(REG_ECC_SEL, 0x1);
				break;
			case 16:
				REG_WRITE_U32(REG_ECC_SEL, 0xe);
				break;
			case 24:
				REG_WRITE_U32(REG_ECC_SEL, 0xa);
				break;
			case 40:
				REG_WRITE_U32(REG_ECC_SEL, 0x2);
				break;
			case 43:
				REG_WRITE_U32(REG_ECC_SEL, 0x4);
				break;
			case 65:
				REG_WRITE_U32(REG_ECC_SEL, 0x6);
				break;
			case 72:
				REG_WRITE_U32(REG_ECC_SEL, 0x8);
				break;
			default:
				REG_WRITE_U32(REG_ECC_SEL, 0x0);
				break;
		}

		dram_sa = ( (uint32_t)this->g_databuf >> 3);
		REG_WRITE_U32(REG_DMA_CTL1,NF_DMA_CTL1_dram_sa(dram_sa));
		dma_len = page_size >> 9;
		REG_WRITE_U32(REG_DMA_CTL2,NF_DMA_CTL2_dma_len(dma_len));

		spare_dram_sa = ( (uint32_t)oob_buf >> 3);
		REG_WRITE_U32( REG_SPR_DDR_CTL,NF_SPR_DDR_CTL_spare_ddr_ena(1)|NF_SPR_DDR_CTL_per_2k_spr_ena(0)|NF_SPR_DDR_CTL_spare_dram_sa(spare_dram_sa));

		// flush cache.
		RTK_FLUSH_CACHE(oob_buf, oob_size);

		REG_WRITE_U32(REG_DMA_CTL3,NF_DMA_CTL3_ddr_wr(1)|NF_DMA_CTL3_dma_xfer(1));
		REG_WRITE_U32( REG_AUTO_TRIG,NF_AUTO_TRIG_auto_trig(1)|NF_AUTO_TRIG_spec_auto_case(0)| NF_AUTO_TRIG_auto_case(auto_trigger_mode));
		WAIT_DONE(REG_AUTO_TRIG,0x80,0);

		WAIT_DONE(REG_AUTO_TRIG,0x80,0);
		WAIT_DONE(REG_DMA_CTL3,0x01,0);

		if(oob_buf)	{
			REG_WRITE_U32(REG_READ_BY_PP,0x00);
			REG_WRITE_U32(REG_SRAM_CTL, 0x30 | 0x04);
			rtk_read_oob_from_SRAM(mtd, oob_buf);
			REG_WRITE_U32(REG_SRAM_CTL, 0x00);
			REG_WRITE_U32(REG_READ_BY_PP,0x80);
		}

		// return OK if all data bit is 1 (page is not written yet)
		if (REG_READ_U32(REG_BLANK_CHK) & 0x2){
			//printk("[%s] page(0x%x) data all one\n", __FUNCTION__, page);
			return 0;
		}
		else if (REG_READ_U32(REG_ND_ECC) & 0x8) {
			if (!bChkAllOne && this->ecc_select>=0x18) {
				bChkAllOne = 1;
				continue;
			}
			printk("[%s] page(0x%x) ecc error\n", __FUNCTION__, page);
			return -1;	// ECC not correctable
		}
		else {
			//printk("[%s] page(0x%x) oob= 0x%x\n", __FUNCTION__, page, *oob_buf);
			return 0;
		}
	}
	return rc;
}
//----------------------------------------------------------------------------
static int rtk_read_ecc_page (struct mtd_info *mtd, u16 chipnr, unsigned int page, u_char *data_buf, u_char *oob_buf, u16 cp_mode, u16 cp_first, size_t cp_len)
{
//static unsigned int eccReadCnt = 0;
	struct nand_chip *this = NULL;
	int rc = 0;
	int dram_sa, dma_len, spare_dram_sa;
	int blank_all_one = 0;
	int page_len;

	unsigned int chip_section = 0;
	unsigned int section = 0;
	unsigned int index = 0;
	unsigned int retryCnt = RETRY_COUNT;
	uint8_t	auto_trigger_mode = 2;
	uint8_t	addr_mode = 1;
	uint8_t	bChkAllOne = 0;
	uint8_t read_retry_cnt = 0;
	uint8_t max_read_retry_cnt = 0;

	//printk("rtk_read_ecc_page (chipnr=%d, page=0x%x, data_buf addr=0x%x, oob_buf addr=0x%x)\n",
	//	chipnr, page, data_buf, oob_buf);
	
	this = (struct nand_chip *) mtd->priv;
	page_size = mtd->oobblock;
	oob_size = mtd->oobsize;
	ppb = mtd->erasesize/mtd->oobblock;

        volatile unsigned int data;

        unsigned char ks[16];
        unsigned char kh[16];
        unsigned char dest[16];
        unsigned int *intPTR;

    if (((uint32_t)data_buf&0x7)!=0) {
            printk("[%s]data_buf must 8 byte alignmemt!!\n",__FUNCTION__);
            BUG();
    }

	if (g_enReadRetrial) {
		switch(g_id_chain)
		{
			case 0x4b44642c:
				max_read_retry_cnt = 7;
				break;
			case 0x4b44442c:
				max_read_retry_cnt = 7;
				break;
			case 0x9384d798:
                                max_read_retry_cnt = 6;
				break;
                       case 0x9384de98:
                                max_read_retry_cnt = 6;
				break;
                       case 0x9394de98:
                                max_read_retry_cnt = 7;
				break;            
			default:
				break;
				
		}
	}

	while (1) 
	{                   
			if (g_enReadRetrial) {
				if (read_retry_cnt !=0) {   //set extra feature before retry read page. ex. toshiba 0x26&0x5D
					switch(g_id_chain)
					{
						case 0x9384d798:
							printk("TC58TEG5DCJT read-retrial rtk_set_ext_feature_toshiba ...read_retry_cnt: %d\n", read_retry_cnt);
                                                        rtk_set_ext_feature_toshiba();
							break;
                            			case 0x9384de98:
							printk("TC58TEG6DCJT read-retrial rtk_set_ext_feature_toshiba ...read_retry_cnt: %d\n", read_retry_cnt);
                                                        rtk_set_ext_feature_toshiba();
							break;
                                                case 0x9394de98:
							printk("TC58TEG6DDK read-retrial rtk_set_ext_feature_toshiba ...read_retry_cnt: %d\n", read_retry_cnt);
                                                        rtk_set_ext_feature_toshiba();
							break;
						default:
							break;
							
					}
				}
			}
    
		REG_WRITE_U32(REG_DATA_TL0,NF_DATA_TL0_length0(0));
		if(this->ecc_select >= 0x18)
		{
			if (bChkAllOne) {
				// enable randomizer
				REG_WRITE_U32(REG_RMZ_CTRL, 0);
				REG_WRITE_U32(REG_BLANK_CHK,NF_BLANK_CHK_blank_ena(1)|NF_BLANK_CHK_read_ecc_xnor_ena(1));
                                //printk("[DBG]ecc error, set xnor and blank to 1, REG_BLANK_CHK reg: 0x%x\n", REG_READ_U32(REG_BLANK_CHK));
			}
			else {
				// enable randomizer
				REG_WRITE_U32(REG_RMZ_CTRL, 1);
				REG_WRITE_U32(REG_BLANK_CHK,NF_BLANK_CHK_blank_ena(1)|NF_BLANK_CHK_read_ecc_xnor_ena(0));
                                //printk("[DBG]set xnor to 0, REG_BLANK_CHK reg: 0x%x\n", REG_READ_U32(REG_BLANK_CHK));
			}
			
			page_len = page_size >> 10;
			REG_WRITE_U32(REG_DATA_TL1,NF_DATA_TL1_access_mode(1)|NF_DATA_TL1_length1(4));
		}
		else
		{
			// set random read
			REG_WRITE_U32(REG_RND_EN, 1);
			REG_WRITE_U32(REG_RND_CMD1, 0x5);
			REG_WRITE_U32(REG_RND_CMD2, 0xe0);
			REG_WRITE_U32(REG_RND_DATA_STR_COL_H, 0);	// data start address MSB (always 0)
			REG_WRITE_U32(REG_RND_SPR_STR_COL_H, page_size >> 8);	// spare start address MSB
			REG_WRITE_U32(REG_RND_SPR_STR_COL_L, page_size & 0xff);	// spare start address LSB
			
			page_len = page_size >> 9;
			REG_WRITE_U32(REG_DATA_TL1,NF_DATA_TL1_access_mode(1)|NF_DATA_TL1_length1(2));
		}
		
		REG_WRITE_U32(REG_PAGE_LEN,NF_PAGE_LEN_page_len(page_len));

		//Set PP
		REG_WRITE_U32(REG_READ_BY_PP,NF_READ_BY_PP_read_by_pp(1));
		REG_WRITE_U32(REG_PP_CTL1,NF_PP_CTL1_pp_start_addr(0));
		REG_WRITE_U32(REG_PP_CTL0,0);

		// enable blank check
		//REG_WRITE_U32(REG_BLANK_CHK,NF_BLANK_CHK_blank_ena(1) );


                if(cp_mode == CP_NF_AES_ECB_128||(cp_mode==CP_NF_AES_CBC_128 && cp_first==1)){
#ifdef CONFIG_NAND_ON_THE_FLY_TEST_KEY

                    if(cp_mode==CP_NF_AES_CBC_128){
            		    REG_WRITE_U32(CP_NF_INI_KEY_0,0x0);
			    REG_WRITE_U32(CP_NF_INI_KEY_1,0x0);
			    REG_WRITE_U32(CP_NF_INI_KEY_2,0x0);
			    REG_WRITE_U32(CP_NF_INI_KEY_3,0x0);
			}

			REG_WRITE_U32(CP_NF_KEY_0,0xad0d8175);
			REG_WRITE_U32(CP_NF_KEY_1,0xa0d732c0);
			REG_WRITE_U32(CP_NF_KEY_2,0xe56ef350);
			REG_WRITE_U32(CP_NF_KEY_3,0xc53ce48b);
        	        // set CP register.

                        if(cp_mode == CP_NF_AES_ECB_128){
        	                REG_WRITE_U32(CP_NF_SET, 0x200);				// sel=0, cw_entry=0, bcm=1, aes_mode=0. Its ECB mode.
                        }
                        else{
                                REG_WRITE_U32(CP_NF_SET, 0x0);				// sel=0, cw_entry=0, bcm=0, aes_mode=0. Its CBC mode.
                        }       
            
#else
        	        // set CP register.

                        if(cp_mode == CP_NF_AES_ECB_128){
        	                //REG_WRITE_U32(CP_NF_SET, 0x200);				// sel=0, cw_entry=0, bcm=1, aes_mode=0. Its ECB mode.
                                    if (!OTP_JUDGE_BIT(OTP_BIT_SECUREBOOT)) 
                                      REG_WRITE_U32(CP_NF_SET, 0x202); // key from kn
                                    else if (OTP_JUDGE_BIT(OTP_BIT_SECUREBOOT)){

#ifdef CONFIG_CMD_KEY_BURNING
                                        OTP_Get_Byte(OTP_K_S, ks, 16);
                                        OTP_Get_Byte(OTP_K_H, kh, 16);
#endif
                                        AES_ECB_encrypt(ks, 16, dest, kh);

					intPTR = dest;
					intPTR[0] = swap_endian(intPTR[0]);
					intPTR[1] = swap_endian(intPTR[1]);
					intPTR[2] = swap_endian(intPTR[2]);
					intPTR[3] = swap_endian(intPTR[3]);

                                        writeNFKey(dest);

                                        memset(ks, 0, 16);
                                        memset(kh, 0, 16);
                                        memset(dest, 0, 16);

                                        REG_WRITE_U32(CP_NF_SET, 0x200);				// sel=0, cw_entry=0, bcm=1, aes_mode=0. Its ECB mode.
                                      //REG_WRITE_U32(CP_NF_SET, 0x2202); // key from kh

                                    }
                        }
                        else{
                                //REG_WRITE_U32(CP_NF_SET, 0x0);				// sel=0, cw_entry=0, bcm=0, aes_mode=0. Its CBC mode.
                                    if (!OTP_JUDGE_BIT(OTP_BIT_SECUREBOOT)) 
                                      REG_WRITE_U32(CP_NF_SET, 0x002); // key from kn
                                    else if (OTP_JUDGE_BIT(OTP_BIT_SECUREBOOT)){

                            		REG_WRITE_U32(CP_NF_INI_KEY_0,0x0);
                			REG_WRITE_U32(CP_NF_INI_KEY_1,0x0);
                			REG_WRITE_U32(CP_NF_INI_KEY_2,0x0);
                			REG_WRITE_U32(CP_NF_INI_KEY_3,0x0);

#ifdef CONFIG_CMD_KEY_BURNING
                                        OTP_Get_Byte(OTP_K_S, ks, 16);
                                        OTP_Get_Byte(OTP_K_H, kh, 16);
#endif
                                        AES_ECB_encrypt(ks, 16, dest, kh);
					
					intPTR = dest;
					intPTR[0] = swap_endian(intPTR[0]);
					intPTR[1] = swap_endian(intPTR[1]);
					intPTR[2] = swap_endian(intPTR[2]);
					intPTR[3] = swap_endian(intPTR[3]);

                                        writeNFKey(dest);

                                        memset(ks, 0, 16);
                                        memset(kh, 0, 16);
                                        memset(dest, 0, 16);

                                        REG_WRITE_U32(CP_NF_SET, 0x0);				// sel=0, cw_entry=0, bcm=0, aes_mode=0. Its CBC mode.
                                      //REG_WRITE_U32(CP_NF_SET, 0x2002); // key from kh

                                    }

                        }        
#endif                          
                        //printk("[DBG]set cp mode...............cp_len: 0x%x\n", cp_len);

                        if(cp_mode == CP_NF_AES_CBC_128 )
                            REG_WRITE_U32(REG_CP_LEN, (cp_len / 0x200) << 9);		// integer multiple of dma_len.
                        else
                            REG_WRITE_U32(REG_CP_LEN, (page_size / 0x200) << 9);		// integer multiple of dma_len.
                            
                        syncPageRead();
                        //printk("[DBG]set cp mode...............REG_READ_U32(REG_CP_LEN): 0x%x\n", REG_READ_U32(REG_CP_LEN));
                }

		//Set command
		REG_WRITE_U32(REG_ND_CMD,NF_ND_CMD_cmd(CMD_PG_READ_C1));
		REG_WRITE_U32(REG_CMD2,NF_CMD2_cmd2(CMD_PG_READ_C2));
		REG_WRITE_U32(REG_CMD3,NF_CMD3_cmd3(CMD_PG_READ_C3));
		
		//Set address
		REG_WRITE_U32( REG_ND_PA0,NF_ND_PA0_page_addr0( 0xff&page ));
		REG_WRITE_U32( REG_ND_PA1,NF_ND_PA1_page_addr1( 0xff&(page>>8) ));
		REG_WRITE_U32( REG_ND_PA2,NF_ND_PA2_addr_mode(addr_mode)|NF_ND_PA2_page_addr2( 0x1f&(page>>16) ));
		REG_WRITE_U32( REG_ND_PA3,NF_ND_PA3_page_addr3( 0x7&(page>>21) ));

		REG_WRITE_U32(REG_ND_CA0, 0);
		REG_WRITE_U32(REG_ND_CA1, 0);

		//Set ECC
		REG_WRITE_U32( REG_MULTI_CHNL_MODE,NF_MULTI_CHNL_MODE_edo(1));//add by alexchang0205-2010
		REG_WRITE_U32( REG_ECC_STOP,NF_ECC_STOP_ecc_n_stop(0x01));

		switch(this->ecc_select) {
			case 6:
				REG_WRITE_U32(REG_ECC_SEL, 0x0);
				break;
			case 12:
				REG_WRITE_U32(REG_ECC_SEL, 0x1);
				break;
			case 16:
				REG_WRITE_U32(REG_ECC_SEL, 0xe);
				break;
			case 24:
				REG_WRITE_U32(REG_ECC_SEL, 0xa);
				break;
			case 40:
				REG_WRITE_U32(REG_ECC_SEL, 0x2);
				break;
			case 43:
				REG_WRITE_U32(REG_ECC_SEL, 0x4);
				break;
			case 65:
				REG_WRITE_U32(REG_ECC_SEL, 0x6);
				break;
			case 72:
				REG_WRITE_U32(REG_ECC_SEL, 0x8);
				break;
			default:
				REG_WRITE_U32(REG_ECC_SEL, 0x0);
				break;
		}

		if (oob_buf) {
			spare_dram_sa = ( (uint32_t)oob_buf >> 3);
		}
		else {
			spare_dram_sa = ( (uint32_t)this->g_oobbuf >> 3);
		}

		REG_WRITE_U32( REG_SPR_DDR_CTL,NF_SPR_DDR_CTL_spare_ddr_ena(1)|NF_SPR_DDR_CTL_per_2k_spr_ena(0)|NF_SPR_DDR_CTL_spare_dram_sa(spare_dram_sa));



		dram_sa = ( (uint32_t)data_buf >> 3);
		REG_WRITE_U32(REG_DMA_CTL1,NF_DMA_CTL1_dram_sa(dram_sa));
		dma_len = page_size >> 9;
		REG_WRITE_U32(REG_DMA_CTL2,NF_DMA_CTL2_dma_len(dma_len));
		//REG_WRITE_U32(REG_DMA_CTL3,NF_DMA_CTL3_ddr_wr(1)|NF_DMA_CTL3_dma_xfer(1));

                if(cp_mode==CP_NF_AES_CBC_128){
                    if(cp_first == 1)
                        REG_WRITE_U32(REG_DMA_CTL3,NF_DMA_CTL3_cp_enable(1)|NF_DMA_CTL3_cp_first(1)|NF_DMA_CTL3_ddr_wr(1)|NF_DMA_CTL3_dma_xfer(1));
                    else
		        REG_WRITE_U32(REG_DMA_CTL3,NF_DMA_CTL3_cp_enable(1)|NF_DMA_CTL3_cp_first(0)|NF_DMA_CTL3_ddr_wr(1)|NF_DMA_CTL3_dma_xfer(1));
                }else if(cp_mode == CP_NF_AES_ECB_128){
                        REG_WRITE_U32(REG_DMA_CTL3,NF_DMA_CTL3_cp_enable(1)|NF_DMA_CTL3_cp_first(1)|NF_DMA_CTL3_ddr_wr(1)|NF_DMA_CTL3_dma_xfer(1));
                }else{
                        REG_WRITE_U32(REG_DMA_CTL3,NF_DMA_CTL3_ddr_wr(1)|NF_DMA_CTL3_dma_xfer(1));
                }
		
		// flush cache.
		RTK_FLUSH_CACHE(data_buf, page_size);
		RTK_FLUSH_CACHE(oob_buf, oob_size);


		//Enable Auto mode
		REG_WRITE_U32( REG_AUTO_TRIG,NF_AUTO_TRIG_auto_trig(1)|NF_AUTO_TRIG_spec_auto_case(0)| NF_AUTO_TRIG_auto_case(auto_trigger_mode));

		WAIT_DONE(REG_AUTO_TRIG,0x80,0);
		WAIT_DONE(REG_DMA_CTL3,0x01,0);

		if(oob_buf)	{
#if 0
			REG_WRITE_U32(REG_READ_BY_PP,0x00);
			REG_WRITE_U32(REG_SRAM_CTL, 0x30 | 0x04);
			rtk_read_oob_from_SRAM(mtd, oob_buf);
			REG_WRITE_U32(REG_SRAM_CTL, 0x00);
			REG_WRITE_U32(REG_READ_BY_PP,0x80);
#endif
                        syncPageRead();

    		        data = (REG_READ_U32(REG_SPR_DDR_CTL) & 0x0fffffff) << 3;	// physical address
    		         //printk("data==> 0x%x\n",data);
		        oob_buf[0] = REG_READ_U32(data + 0x0) & 0xff;
                        //printk("oob_buf[0]==> 0x%x, page=0x%x\n",oob_buf[0],page);           
		}

		//rtk_hexdump("data_buf : ", data_buf, page_size);
		//rtk_hexdump("oob_buf : ", oob_buf, oob_size);
                //if(REG_READ_U32(REG_ND_ECC) & 0x4)
                    //printk("page: 0x%x, MAX_ECC_NUM: 0x%x\n", page, REG_MAX_ECC_NUM_max_ecc_num(REG_READ_U32(REG_MAX_ECC_NUM)));

                //syncPageRead();
		// return OK if all data bit is 1 (page is not written yet)
		if (REG_READ_U32(REG_BLANK_CHK) & 0x2){
			//printk("data all one \n");
			if (g_enReadRetrial) {
				if (read_retry_cnt !=0) {
					switch(g_id_chain)
					{
						case 0x4b44642c:
							printk("MT29F64G08CBABA read-retrial rtk_set_feature_micron ...reset to 0\n");
							rtk_set_feature_micron(0);
							break;
                            			case 0x4b44442c:
							printk("MT29F32G08CBADA read-retrial rtk_set_feature_micron ...reset to 0\n");
							rtk_set_feature_micron(0);
							break;
						case 0x9384d798:
							printk("TC58TEG5DCJT read-retrial rtk_set_feature_toshiba ...reset to 0\n");
							rtk_set_feature_toshiba(0);
                                                        rtk_set_terminate_toshiba();
							break;
                            			case 0x9384de98:
							printk("TC58TEG6DCJT read-retrial rtk_set_feature_toshiba ...reset to 0\n");
							rtk_set_feature_toshiba(0);
                                                        rtk_set_terminate_toshiba();
							break;
                                                case 0x9394de98:
							printk("TC58TEG6DDK read-retrial rtk_set_feature_toshiba ...reset to 0\n");
							rtk_set_feature_toshiba_new(0);
                                                        rtk_set_terminate_toshiba();
							break;
                            
						default:
							break;
							
					}
				}
			}
			REG_WRITE_U32(REG_BLANK_CHK,NF_BLANK_CHK_blank_ena(1)|NF_BLANK_CHK_read_ecc_xnor_ena(0));
			return 0;
		}
		else if (REG_READ_U32(REG_ND_ECC) & 0x8) {
			if (!bChkAllOne && this->ecc_select>=0x18) {
                                //printk("[DBG]bChkAllOne this->ecc_select: 0x%x\n", this->ecc_select);
				bChkAllOne = 1;
				continue;
			}
			if (g_enReadRetrial) {
				bChkAllOne = 0;
                                if(read_retry_cnt == 0){    //set pre condition
					switch(g_id_chain)
					{
						case 0x9384d798:
							printk("TC58TEG5DCJT read-retrial rtk_set_pre_condition_toshiba\n");
							rtk_set_pre_condition_toshiba();
							break;
                            			case 0x9384de98:
							printk("TC58TEG6DCJT read-retrial rtk_set_pre_condition_toshiba\n");
							rtk_set_pre_condition_toshiba();
							break;       
                                                case 0x9394de98:
							printk("TC58TEG6DDK read-retrial rtk_set_pre_condition_toshiba\n");
							rtk_set_pre_condition_toshiba();
							break;    
						default:
							break;							
					}
                                }              
				read_retry_cnt ++;
				if (read_retry_cnt <= max_read_retry_cnt) {
					switch(g_id_chain)
					{
						case 0x4b44642c:
							printk("MT29F64G08CBABA read-retrial rtk_set_feature_micron ...read_retry_cnt: %d\n", read_retry_cnt);
							rtk_set_feature_micron(read_retry_cnt);
							break;
						case 0x4b44442c:
							printk("MT29F32G08CBADA read-retrial rtk_set_feature_micron ...read_retry_cnt: %d\n", read_retry_cnt);
							rtk_set_feature_micron(read_retry_cnt);
							break;	
						case 0x9384d798:
							printk("TC58TEG5DCJT read-retrial rtk_set_feature_toshiba ...read_retry_cnt: %d, data: 0x%x\n", read_retry_cnt, read_retry_toshiba_cnt[read_retry_cnt]);
							rtk_set_feature_toshiba(read_retry_toshiba_cnt[read_retry_cnt]);
							break;
                            			case 0x9384de98:
							printk("TC58TEG6DCJT read-retrial rtk_set_feature_toshiba ...read_retry_cnt: %d, data: 0x%x\n", read_retry_cnt, read_retry_toshiba_cnt[read_retry_cnt]);
							rtk_set_feature_toshiba(read_retry_toshiba_cnt[read_retry_cnt]);
                            				break;
                            			case 0x9394de98:
							printk("TC58TEG6DDK read-retrial rtk_set_feature_toshiba ...read_retry_cnt: %d\n", read_retry_cnt);
							rtk_set_feature_toshiba_new(read_retry_cnt);
							break;
                            
						default:
							break;
							
					}
					continue;
				}
			}
			printk("[DBG]ecc error... page=0x%x, REG_BLANK_CHK reg: 0x%x \n", page, REG_READ_U32(REG_BLANK_CHK));
			if (g_enReadRetrial) {
				if (read_retry_cnt !=0) {
					switch(g_id_chain)
					{
						case 0x4b44642c:
							printk("MT29F64G08CBABA read-retrial rtk_set_feature_micron ...reset to 0\n");
							rtk_set_feature_micron(0);
							break;
                            			case 0x4b44442c:
							printk("MT29F32G08CBADA read-retrial rtk_set_feature_micron ...reset to 0\n");
							rtk_set_feature_micron(0);
							break;
						case 0x9384d798:
							printk("TC58TEG5DCJT read-retrial rtk_set_feature_toshiba ...reset to 0\n");
							rtk_set_feature_toshiba(0);
                                                        rtk_set_terminate_toshiba();
							break;
                            			case 0x9384de98:
							printk("TC58TEG6DCJT read-retrial rtk_set_feature_toshiba ...reset to 0\n");
							rtk_set_feature_toshiba(0);
                                                        rtk_set_terminate_toshiba();
							break;
                                                case 0x9394de98:
							printk("TC58TEG6DDK read-retrial rtk_set_feature_toshiba ...reset to 0\n");
							rtk_set_feature_toshiba_new(0);
                                                        rtk_set_terminate_toshiba();
							break;
						default:
                            
							break;
							
					}
				}
			}
                        REG_WRITE_U32(REG_BLANK_CHK,NF_BLANK_CHK_blank_ena(1)|NF_BLANK_CHK_read_ecc_xnor_ena(0));
                        return -1;	// ECC not correctable
		}
		else {
			if (g_enReadRetrial) {
				if (read_retry_cnt !=0) {
					switch(g_id_chain)
					{
						case 0x4b44642c:
							printk("MT29F64G08CBABA read-retrial rtk_set_feature_micron ...reset to 0\n");
							rtk_set_feature_micron(0);
							break;
                            			case 0x4b44442c:
							printk("MT29F32G08CBADA read-retrial rtk_set_feature_micron ...reset to 0\n");
							rtk_set_feature_micron(0);
							break;
                            			case 0x9384d798:
							printk("TC58TEG5DCJT read-retrial rtk_set_feature_toshiba ...reset to 0\n");
							rtk_set_feature_toshiba(0);
                                                        rtk_set_terminate_toshiba();
							break;
                            			case 0x9384de98:
							printk("TC58TEG6DCJT read-retrial rtk_set_feature_toshiba ...reset to 0\n");
							rtk_set_feature_toshiba(0);
                                                        rtk_set_terminate_toshiba();
							break;
                                                case 0x9394de98:
							printk("TC58TEG6DDK read-retrial rtk_set_feature_toshiba ...reset to 0\n");
							rtk_set_feature_toshiba_new(0);
                                                        rtk_set_terminate_toshiba();
							break;
						default:
							break;
							
					}
				}
			}            
                        REG_WRITE_U32(REG_BLANK_CHK,NF_BLANK_CHK_blank_ena(1)|NF_BLANK_CHK_read_ecc_xnor_ena(0));
			return 0;
		}
	}

	return rc;
}
//----------------------------------------------------------------------------
static int rtk_write_oob(struct mtd_info *mtd, u16 chipnr, int page, int len, const u_char *oob_buf)
{
	struct nand_chip *this = (struct nand_chip *) mtd->priv;
	int rc = 0;
	uint8_t	auto_trigger_mode = 1;
	uint8_t	addr_mode = 1;
	unsigned int page_len, dram_sa, dma_len, spare_dram_sa;
	unsigned char oob_1stB;
	unsigned int chip_section = 0;
	unsigned int section =0;
	unsigned int index =0;
	unsigned int retryCnt = RETRY_COUNT;
	if (this->erase_page_flag)
		this->erase_page_flag[chip_section+section] &= ~(1 << index);
	memset(this->g_databuf, 0xff, page_size);

	page_size = mtd->oobblock;
	oob_size = mtd->oobsize;
	ppb = mtd->erasesize/mtd->oobblock;

	//while (down_interruptible (&sem_NF_CARDREADER)) {
	//	printk("%s : Retry [Address: 0x%x]\n",__FUNCTION__,page);
	//	if(retryCnt<=0) {
	//		printk("%s : Retry fail [ADDR 0x%x]\n",__FUNCTION__,page);
	//		return -ERESTARTSYS;
	//	}
	//	retryCnt--;
	//}
	if ( chipnr == 0 && page >= 0 && page < g_BootcodeSize/page_size ){
		printk("[%s] You have no permission to write this page %d\n", __FUNCTION__, page);
		//up (&sem_NF_CARDREADER);
		return -2;
	}

	if ( len>oob_size || !oob_buf ){
		printk("[%s] error: len>oob_size OR oob_buf is NULL\n", __FUNCTION__);
		//up (&sem_NF_CARDREADER);
		return -1;
	}

	//if(is_darwin_cpu()||is_macarthur_cpu()||is_nike_cpu())//Enable NF_WP pin (Write Protect Pin)
	//{
	//	if(g_WP_en)
	//	{
	//		setGPIOBit(g_regGPIODirBase,g_NFWP_no,NF_GPIO_OUT);//Set Driection
	//		setGPIOBit(g_regGPIOOutBase,g_NFWP_no,g_NFWP_value_dis);//Set Value
	//	}
	//}

	if ( page == ppb || page == ppb+1|| page == ppb*2 || page == ppb*2+1 )
		oob_1stB = BBT_TAG;
	else if(( page>=g_BootcodeSize/page_size )&&(page<(g_BootcodeSize+g_Factory_param_size)/page_size))
		oob_1stB = TAG_FACTORY_PARAM;
	else
		oob_1stB = 0xFF;
//end of alexchang 0208-2010
	//REG_WRITE_U32(0x01,0xb801032c);	//Enable NAND/CardReader arbitrator add by alexchang 0303-2010
	REG_WRITE_U32(REG_SRAM_CTL,0x00);//add by alexchang 0208-2010
	REG_WRITE_U32(REG_DATA_TL0,NF_DATA_TL0_length0(0));
 	if(this->ecc_select >= 0x18)
	{
		page_len = page_size >> 10;
		REG_WRITE_U32(REG_DATA_TL1,NF_DATA_TL1_length1(4));
	}
	else
	{
		page_len = page_size >> 9;
		REG_WRITE_U32(REG_DATA_TL1,NF_DATA_TL1_length1(2));
	}

	REG_WRITE_U32(REG_READ_BY_PP,NF_READ_BY_PP_read_by_pp(0));//add by alexchang 0208-2010
	REG_WRITE_U32(REG_PP_CTL1,NF_PP_CTL1_pp_start_addr(0));//add by alexchang 0208-2010
	REG_WRITE_U32(REG_PP_CTL0,0);//add by alexchang 0208-2010

	REG_WRITE_U32( REG_PAGE_LEN,NF_PAGE_LEN_page_len(page_len));

 	REG_WRITE_U32(REG_ND_CMD,NF_ND_CMD_cmd(CMD_PG_WRITE_C1));
	REG_WRITE_U32(REG_CMD2,NF_CMD2_cmd2(CMD_PG_WRITE_C2));
	REG_WRITE_U32(REG_CMD3,NF_CMD3_cmd3(CMD_PG_WRITE_C3));

	REG_WRITE_U32( REG_ND_PA0,NF_ND_PA0_page_addr0(page));
	REG_WRITE_U32( REG_ND_PA1,NF_ND_PA1_page_addr1(page>>8));
	REG_WRITE_U32( REG_ND_PA2,NF_ND_PA2_addr_mode(addr_mode)|NF_ND_PA2_page_addr2(page>>16));
	REG_WRITE_U32( REG_ND_PA3,NF_ND_PA3_page_addr3((page>>21)&0x7));
	REG_WRITE_U32(REG_MULTI_CHNL_MODE,NF_MULTI_CHNL_MODE_edo(1));//add by alexchang0205-2010

	switch(this->ecc_select) {
		case 6:
			REG_WRITE_U32(REG_ECC_SEL, 0x0);
			break;
		case 12:
			REG_WRITE_U32(REG_ECC_SEL, 0x1);
			break;
		case 16:
			REG_WRITE_U32(REG_ECC_SEL, 0xe);
			break;
		case 24:
			REG_WRITE_U32(REG_ECC_SEL, 0xa);
			break;
		case 40:
			REG_WRITE_U32(REG_ECC_SEL, 0x2);
			break;
		case 43:
			REG_WRITE_U32(REG_ECC_SEL, 0x4);
			break;
		case 65:
			REG_WRITE_U32(REG_ECC_SEL, 0x6);
			break;
		case 72:
			REG_WRITE_U32(REG_ECC_SEL, 0x8);
			break;
		default:
			REG_WRITE_U32(REG_ECC_SEL, 0x0);
			break;
	}

	dram_sa = ( (uint32_t)this->g_databuf >> 3);
	REG_WRITE_U32(REG_DMA_CTL1,NF_DMA_CTL1_dram_sa(dram_sa));
	dma_len = page_size >> 9;
	REG_WRITE_U32(REG_DMA_CTL2,NF_DMA_CTL2_dma_len(dma_len));	
	REG_WRITE_U32(REG_DMA_CTL3,NF_DMA_CTL3_ddr_wr(1)|NF_DMA_CTL3_dma_xfer(1));

	spare_dram_sa = ( (uint32_t)oob_buf >> 3);
	REG_WRITE_U32( REG_SPR_DDR_CTL,NF_SPR_DDR_CTL_spare_ddr_ena(1)|NF_SPR_DDR_CTL_per_2k_spr_ena(1)|NF_SPR_DDR_CTL_spare_dram_sa(spare_dram_sa));
#if 0	
#if RTK_NAND_INTERRUPT
	NF_RESET_IRQ;
	NF_ENABLE_IRQ(NF_AUTO_IRQ_MASK);
	g_nfFlag = NF_WRITE_TRIGGER;
#endif
	if(g_isRandomize&&(!mtd->isCPdisable_W))
	{
		if(mtd->isScramble!=MTD_SCRAMBLE_DISABLE)
		{
			//printk("&");
			if(u32CP_mode==0)//CBC initial key setting.
			{
					REG_WRITE_U32( REG_CP_NF_INI_KEY,page);
			}
			REG_WRITE_U32(REG_CP_NF_KEY,0x12345678);
			REG_WRITE_U32((REG_CP_NF_KEY+0x4),0x12345678);
			REG_WRITE_U32((REG_CP_NF_KEY+0x8),0x12345678);
			REG_WRITE_U32((REG_CP_NF_KEY+0xc),0x12345678);


			REG_WRITE_U32(REG_CP_NF_SET,u32CP_mode|cpSel);
			REG_WRITE_U32(REG_CP_LEN,NF_CP_LEN_cp_length(page_size));
			//add by alexchang 0208-2010
			RTK_FLUSH_CACHE((unsigned long) this->g_databuf, page_size);
			if ( oob_buf )
				RTK_FLUSH_CACHE((unsigned long) oob_buf, oob_size);
			REG_WRITE_U32(REG_DMA_CTL3,NF_DMA_CTL3_cp_enable(0x1)|NF_DMA_CTL3_cp_first(0x1)|NF_DMA_CTL3_ddr_wr(0)|NF_DMA_CTL3_dma_xfer(0x1));
		}
		else
		{
			//printk("%");
			RTK_FLUSH_CACHE((unsigned long) this->g_databuf, page_size);
			if ( oob_buf )
				RTK_FLUSH_CACHE((unsigned long) oob_buf, oob_size);
			REG_WRITE_U32(REG_DMA_CTL3,NF_DMA_CTL3_cp_enable(0)|NF_DMA_CTL3_cp_first(0)|NF_DMA_CTL3_ddr_wr(0)|NF_DMA_CTL3_dma_xfer(0x1));
		}
	}
	else
	{
		if(is_NF_CP_Enable_write&&(!mtd->isCPdisable_W))
		{//printk("#");
			REG_WRITE_U32(REG_CP_NF_KEY,*regKey0);
			REG_WRITE_U32((REG_CP_NF_KEY+0x4),*regKey1);
			REG_WRITE_U32((REG_CP_NF_KEY+0x8),*regKey2);
			REG_WRITE_U32((REG_CP_NF_KEY+0xc),*regKey3);


			REG_WRITE_U32(REG_CP_NF_SET,u32CP_mode|cpSel);
			REG_WRITE_U32(REG_CP_LEN,NF_CP_LEN_cp_length(page_size));
			//add by alexchang 0208-2010
			RTK_FLUSH_CACHE((unsigned long) this->g_databuf, page_size);
			if ( oob_buf )
				RTK_FLUSH_CACHE((unsigned long) oob_buf, oob_size);
			REG_WRITE_U32(REG_DMA_CTL3,NF_DMA_CTL3_cp_enable(0x1)|NF_DMA_CTL3_cp_first(0x1)|NF_DMA_CTL3_ddr_wr(0)|NF_DMA_CTL3_dma_xfer(0x1));
		}
		else
		{//printk("@");
			//add by alexchang 0208-2010
			RTK_FLUSH_CACHE((unsigned long) this->g_databuf, page_size);
			if ( oob_buf )
				RTK_FLUSH_CACHE((unsigned long) oob_buf, oob_size);
			REG_WRITE_U32(REG_DMA_CTL3,NF_DMA_CTL3_cp_enable(0)|NF_DMA_CTL3_cp_first(0)|NF_DMA_CTL3_ddr_wr(0)|NF_DMA_CTL3_dma_xfer(0x1));
		}
	}
#endif
	REG_WRITE_U32( REG_AUTO_TRIG,NF_AUTO_TRIG_auto_trig(1)|NF_AUTO_TRIG_spec_auto_case(0)| NF_AUTO_TRIG_auto_case(auto_trigger_mode));

//#if RTK_NAND_INTERRUPT
//	RTK_WAIT_EVENT;
//	NF_RESET_IRQ;
//	NF_ENABLE_IRQ(NF_POLL_IRQ_MASK);
//	g_nfFlag = NF_WRITE_TRIGGER;
//	REG_WRITE_U32(REG_POLL_FSTS,NF_POLL_FSTS_bit_sel(6)|NF_POLL_FSTS_trig_poll(1) );
//	RTK_WAIT_EVENT;
//#else
	WAIT_DONE(REG_AUTO_TRIG,0x80,0);
	WAIT_DONE(REG_DMA_CTL3,0x01,0);
	REG_WRITE_U32(REG_POLL_FSTS,NF_POLL_FSTS_bit_sel(6)|NF_POLL_FSTS_trig_poll(1) );
	WAIT_DONE(REG_POLL_FSTS,0x01,0x0);
	WAIT_DONE(REG_ND_CTL,0x40,0x40);//add by alexchang 0416-2010
//#endif

	return 0;

	if(is_darwin_cpu()||is_macarthur_cpu()||is_nike_cpu())//Enable NF_WP pin (Write Protect Pin)
	{
		if(g_WP_en)
		{
			setGPIOBit(g_regGPIODirBase,g_NFWP_no,NF_GPIO_OUT);//Set Driection
			setGPIOBit(g_regGPIOOutBase,g_NFWP_no,g_NFWP_value_en);//Set Value
		}
	}

	if((is_NF_CP_Enable_write&&(!mtd->isCPdisable_W))||g_isRandomize)//Clear register
	{
		REG_WRITE_U32(REG_CP_NF_SET,0);
	}

	if ( REG_READ_U32(REG_ND_DAT) & 0x01 ){
		//up (&sem_NF_CARDREADER);
		printk("[%s] write oob is not completed at page %d\n", __FUNCTION__, page);

		return -1;
	}

	chip_section = (chipnr * this->page_num) >> 5;
	section = page >> 5;
	index = page & (32-1);
	if (this->erase_page_flag)
		this->erase_page_flag[chip_section+section] &= ~(1 << index);
	mtd->isCPdisable_W = 0;
	//printk("[AT]WO block : %u ,page : %d  \n",page/ppb,page%ppb);
	//up (&sem_NF_CARDREADER);
//int k= 0;
//for(k=0;k<32;k++)
//printk("[WO][%u,%u][%d]0x%x\n",page/ppb,page%ppb,k,oob_buf[k]);
//printk("----------------------------\n");

	return rc;
}

//----------------------------------------------------------------------------
//char bbbbb[4096];

static int rtk_write_ecc_page (struct mtd_info *mtd, u16 chipnr, unsigned int page,
			const u_char *data_buf, const  u_char *oob_buf, int isBBT)
{
	unsigned int ppb = mtd->erasesize/mtd->oobblock;
	struct nand_chip *this = (struct nand_chip *) mtd->priv;
	int rc = 0;
	uint8_t	auto_trigger_mode = 1;
	uint8_t	addr_mode = 1;

	unsigned int page_len, dram_sa, dma_len, spare_dram_sa;
	unsigned char oob_1stB;

	unsigned char nf_oob_buf[oob_size];
	unsigned int chip_section = 0;
	unsigned int section = 0;
	unsigned int index = 0;
	unsigned int retryCnt = RETRY_COUNT;

	printk("[%s] entry... \n", __FUNCTION__);
	//printk("00[%s]Ran 0x%x, scr 0x%x, flag 0x%x\n",__FUNCTION__,g_isRandomize,mtd->isScramble,mtd->isCPdisable_W);
	//while (down_interruptible (&sem_NF_CARDREADER)) {
	//	printk("%s : Retry [Address: 0x%x]\n",__FUNCTION__,page);
	//	if(retryCnt<=0) {
	//		printk("%s : Retry fail [ADDR 0x%x]\n",__FUNCTION__,page);
	//		return -ERESTARTSYS;
	//	}
	//	retryCnt--;
	//}
    if(((uint32_t)data_buf&0x7)!=0) {
            printk("[%s]data_buf must 8 byte alignmemt!!\n",__FUNCTION__);
            BUG();
    }
    if(((uint32_t)oob_buf&0x7)!=0) {
            printk("[%s]oob_buf must 8 byte alignmemt!!\n",__FUNCTION__);
            BUG();
    }

	if ( chipnr == 0 && page < g_BootcodeSize/page_size ){
		if ( isBBT && page<3*ppb ){
			printk("[%s] Updating BBT %d page=%d\n", __FUNCTION__, page/ppb, page%ppb);
		}
		else {
			printk("[%s] You have no permission to write page %d\n", __FUNCTION__, page);
			//up (&sem_NF_CARDREADER);
			return -2;
		}
		mtd->isCPdisable_W = 1;
	}
	if ( chipnr == 0 && page >= g_BootcodeSize/page_size){
		if(page<(g_BootcodeSize+g_Factory_param_size)/page_size+ppb)
		{
			  mtd->isCPdisable_W = 1;
		}
	}

	//if(is_darwin_cpu()||is_macarthur_cpu()||is_nike_cpu())//Enable NF_WP pin (Write Protect Pin)
	//{
	//	if(g_WP_en)
	//	{
	//		setGPIOBit(g_regGPIODirBase,g_NFWP_no,NF_GPIO_OUT);//Set Driection
	//		setGPIOBit(g_regGPIOOutBase,g_NFWP_no,g_NFWP_value_dis);//Set Value
	//	}
	//}

	if ( page == ppb || page == ppb+1|| page == ppb*2 || page == ppb*2+1 )
		oob_1stB = BBT_TAG;
	else if(( page>=g_BootcodeSize/page_size )&&(page<(g_BootcodeSize+g_Factory_param_size)/page_size))
		oob_1stB = TAG_FACTORY_PARAM;
	else
		oob_1stB = 0xFF;

	//REG_WRITE_U32(0xb801032c,0x01);	//Enable NAND/CardReader arbitrator add by alexchang 0303-2010

	/*
	if ( oob_buf )	{
		int i,j;
		//memset(nf_oob_buf,0xff,oob_size);
		nf_oob_buf[0] = oob_1stB;

		if(this->ecc_select == 0x18)
		{
			for(i=1,j=0; i<8; i++)
			{
				nf_oob_buf[8*i] = oob_buf[j];
				nf_oob_buf[8*i+1] = oob_buf[j+1];
				nf_oob_buf[8*i+2] = oob_buf[j+2];
				nf_oob_buf[8*i+3] = oob_buf[j+3];
				j+=4;
			}
		}
		else
		{
			for(i=0; i<4; i++)
				nf_oob_buf[1+i] = oob_buf[i];
			for(i=2,j=4; i<8; i+=2)
			{
				nf_oob_buf[4*i] = oob_buf[j];
				nf_oob_buf[4*i+1] = oob_buf[j+1];
				nf_oob_buf[4*i+2] = oob_buf[j+2];
				nf_oob_buf[4*i+3] = oob_buf[j+3];
				j+=4;
			}
		}
	}else
		nf_oob_buf[0] = oob_1stB;
	*/
	this->g_oobbuf[0x0] = oob_1stB;
	this->g_oobbuf[0x10] = oob_1stB;
	this->g_oobbuf[0x20] = oob_1stB;
	this->g_oobbuf[0x30] = oob_1stB;

    	this->g_oobbuf[0x8] = oob_1stB;
	this->g_oobbuf[0x18] = oob_1stB;
	this->g_oobbuf[0x28] = oob_1stB;
	this->g_oobbuf[0x38] = oob_1stB;

    	this->g_oobbuf[0x40] = oob_1stB;
	this->g_oobbuf[0x50] = oob_1stB;
	this->g_oobbuf[0x60] = oob_1stB;
	this->g_oobbuf[0x70] = oob_1stB;

    	this->g_oobbuf[0x48] = oob_1stB;
	this->g_oobbuf[0x58] = oob_1stB;
	this->g_oobbuf[0x68] = oob_1stB;
	this->g_oobbuf[0x78] = oob_1stB;

	//REG_WRITE_U32(REG_SRAM_CTL,0x00);//add by alexchang 0208-2010
	REG_WRITE_U32(REG_DATA_TL0,NF_DATA_TL0_length0(0));
 	if(this->ecc_select >= 0x18)
	{
		// enable randomizer
		REG_WRITE_U32(REG_RMZ_CTRL, 1);
		
		page_len = page_size >> 10;
		REG_WRITE_U32(REG_DATA_TL1,NF_DATA_TL1_length1(4));
	}
	else
	{
		// set random write
		REG_WRITE_U32(REG_RND_EN, 1);
		REG_WRITE_U32(REG_RND_CMD1, 0x85);
		REG_WRITE_U32(REG_RND_DATA_STR_COL_H, 0);	// data start address MSB (always 0)
		REG_WRITE_U32(REG_RND_SPR_STR_COL_H, page_size >> 8);	// spare start address MSB
		REG_WRITE_U32(REG_RND_SPR_STR_COL_L, page_size & 0xff);	// spare start address LSB
		
		page_len = page_size >> 9;
		REG_WRITE_U32(REG_DATA_TL1,NF_DATA_TL1_length1(2));
	}

	REG_WRITE_U32( REG_PAGE_LEN,NF_PAGE_LEN_page_len(page_len));

	//Set PP
	REG_WRITE_U32(REG_READ_BY_PP,NF_READ_BY_PP_read_by_pp(0));//add by alexchang 0208-2010
	REG_WRITE_U32(REG_PP_CTL1,NF_PP_CTL1_pp_start_addr(0));//add by alexchang 0208-2010
	REG_WRITE_U32(REG_PP_CTL0,0);//add by alexchang 0208-2010

	//Set command
 	REG_WRITE_U32(REG_ND_CMD,NF_ND_CMD_cmd(CMD_PG_WRITE_C1));
	REG_WRITE_U32(REG_CMD2,NF_CMD2_cmd2(CMD_PG_WRITE_C2));
	REG_WRITE_U32(REG_CMD3,NF_CMD3_cmd3(CMD_PG_WRITE_C3));

	//Set address
	REG_WRITE_U32( REG_ND_PA0,NF_ND_PA0_page_addr0(page));
	REG_WRITE_U32( REG_ND_PA1,NF_ND_PA1_page_addr1(page>>8));
	REG_WRITE_U32( REG_ND_PA2,NF_ND_PA2_addr_mode(addr_mode)|NF_ND_PA2_page_addr2(page>>16));
	REG_WRITE_U32( REG_ND_PA3,NF_ND_PA3_page_addr3((page>>21)&0x7));
	REG_WRITE_U32(REG_ND_CA0, 0);
	REG_WRITE_U32(REG_ND_CA1, 0);

	//Set ECC
	REG_WRITE_U32(REG_MULTI_CHNL_MODE,NF_MULTI_CHNL_MODE_edo(1));//add by alexchang0205-2010
	REG_WRITE_U32( REG_ECC_STOP,NF_ECC_STOP_ecc_n_stop(0x01));

	switch(this->ecc_select) {
		case 6:
			REG_WRITE_U32(REG_ECC_SEL, 0x0);
			break;
		case 12:
			REG_WRITE_U32(REG_ECC_SEL, 0x1);
			break;
		case 16:
			REG_WRITE_U32(REG_ECC_SEL, 0xe);
			break;
		case 24:
			REG_WRITE_U32(REG_ECC_SEL, 0xa);
			break;
		case 40:
			REG_WRITE_U32(REG_ECC_SEL, 0x2);
			break;
		case 43:
			REG_WRITE_U32(REG_ECC_SEL, 0x4);
			break;
		case 65:
			REG_WRITE_U32(REG_ECC_SEL, 0x6);
			break;
		case 72:
			REG_WRITE_U32(REG_ECC_SEL, 0x8);
			break;
		default:
			REG_WRITE_U32(REG_ECC_SEL, 0x0);
			break;
	}

	// flush cache.
	RTK_FLUSH_CACHE(data_buf, page_size);
	RTK_FLUSH_CACHE(this->g_oobbuf, oob_size);
	
	dram_sa = ( (uint32_t)data_buf >> 3);
	REG_WRITE_U32(REG_DMA_CTL1,NF_DMA_CTL1_dram_sa(dram_sa));
	dma_len = page_size >> 9;
	REG_WRITE_U32(REG_DMA_CTL2,NF_DMA_CTL2_dma_len(dma_len));	
	
	//if (oob_buf)
	//	spare_dram_sa = ( (uint32_t)nf_oob_buf >> 3);
	//else
		spare_dram_sa = ( (uint32_t)this->g_oobbuf >> 3);
	REG_WRITE_U32(REG_SPR_DDR_CTL,NF_SPR_DDR_CTL_spare_ddr_ena(1)|NF_SPR_DDR_CTL_per_2k_spr_ena(1)|NF_SPR_DDR_CTL_spare_dram_sa(spare_dram_sa));

#if 0
#if RTK_NAND_INTERRUPT
	NF_RESET_IRQ;
	NF_ENABLE_IRQ(NF_AUTO_IRQ_MASK);
	g_nfFlag = NF_WRITE_TRIGGER;
#endif
	//printk("11[%s]Ran 0x%x, scr 0x%x, flag 0x%x\n",__FUNCTION__,g_isRandomize,mtd->isScramble,mtd->isCPdisable_W);

	if(g_isRandomize&&(!mtd->isCPdisable_W))
	{
//		if(mtd->isScramble!=MTD_SCRAMBLE_DISABLE)
//		{
			//printk("$");
			if(u32CP_mode==0)//CBC initial key setting.
			{
					REG_WRITE_U32( REG_CP_NF_INI_KEY,page);
			}
			REG_WRITE_U32(REG_CP_NF_KEY,0x12345678);
			REG_WRITE_U32((REG_CP_NF_KEY+0x4),0x12345678);
			REG_WRITE_U32((REG_CP_NF_KEY+0x8),0x12345678);
			REG_WRITE_U32((REG_CP_NF_KEY+0xc),0x12345678);

			REG_WRITE_U32(REG_CP_NF_SET,u32CP_mode|cpSel);
			REG_WRITE_U32(REG_CP_LEN,NF_CP_LEN_cp_length(page_size));
			RTK_FLUSH_CACHE((unsigned long) data_buf, page_size);
			RTK_FLUSH_CACHE((unsigned long) nf_oob_buf, oob_size);
			REG_WRITE_U32(REG_DMA_CTL3,NF_DMA_CTL3_cp_enable(0x1)|NF_DMA_CTL3_cp_first(0x1)|NF_DMA_CTL3_ddr_wr(0)|NF_DMA_CTL3_dma_xfer(0x1));
//		}
//		else
//		{//printk("=");
//			RTK_FLUSH_CACHE((unsigned long) data_buf, page_size);
//			RTK_FLUSH_CACHE((unsigned long) nf_oob_buf, oob_size);
//			REG_WRITE_U32(REG_DMA_CTL3,NF_DMA_CTL3_cp_enable(0)|NF_DMA_CTL3_cp_first(0)|NF_DMA_CTL3_ddr_wr(0)|NF_DMA_CTL3_dma_xfer(0x1));
//		}
	}
	else
	{
		if(is_NF_CP_Enable_write&&(!mtd->isCPdisable_W))
		{//printk("+");
			REG_WRITE_U32(REG_CP_NF_KEY,*regKey0);
			REG_WRITE_U32((REG_CP_NF_KEY+0x4),*regKey1);
			REG_WRITE_U32((REG_CP_NF_KEY+0x8),*regKey2);
			REG_WRITE_U32((REG_CP_NF_KEY+0xc),*regKey3);

			REG_WRITE_U32(REG_CP_NF_SET,u32CP_mode|cpSel);
			REG_WRITE_U32(REG_CP_LEN,NF_CP_LEN_cp_length(page_size));
			RTK_FLUSH_CACHE((unsigned long) data_buf, page_size);
			RTK_FLUSH_CACHE((unsigned long) nf_oob_buf, oob_size);
			REG_WRITE_U32(REG_DMA_CTL3,NF_DMA_CTL3_cp_enable(0x1)|NF_DMA_CTL3_cp_first(0x1)|NF_DMA_CTL3_ddr_wr(0)|NF_DMA_CTL3_dma_xfer(0x1));
		}
		else
		{//printk("-");
			RTK_FLUSH_CACHE((unsigned long) data_buf, page_size);
			RTK_FLUSH_CACHE((unsigned long) nf_oob_buf, oob_size);
			REG_WRITE_U32(REG_DMA_CTL3,NF_DMA_CTL3_cp_enable(0)|NF_DMA_CTL3_cp_first(0)|NF_DMA_CTL3_ddr_wr(0)|NF_DMA_CTL3_dma_xfer(0x1));
		}
	}

#endif
	REG_WRITE_U32(REG_DMA_CTL3,NF_DMA_CTL3_ddr_wr(0)|NF_DMA_CTL3_dma_xfer(1));
	REG_WRITE_U32( REG_AUTO_TRIG,NF_AUTO_TRIG_auto_trig(1)|NF_AUTO_TRIG_spec_auto_case(0)| NF_AUTO_TRIG_auto_case(auto_trigger_mode));


//#if RTK_NAND_INTERRUPT
//	RTK_WAIT_EVENT;
//	NF_RESET_IRQ;
//	NF_ENABLE_IRQ(NF_POLL_IRQ_MASK);
//	g_nfFlag = NF_WRITE_TRIGGER;
//	REG_WRITE_U32(REG_POLL_FSTS,NF_POLL_FSTS_bit_sel(6)|NF_POLL_FSTS_trig_poll(1) );
//	RTK_WAIT_EVENT;
//#else
	WAIT_DONE(REG_AUTO_TRIG,0x80,0);
	WAIT_DONE(REG_DMA_CTL3,0x01,0);
	REG_WRITE_U32(REG_POLL_FSTS,NF_POLL_FSTS_bit_sel(6)|NF_POLL_FSTS_trig_poll(1) );
	WAIT_DONE(REG_POLL_FSTS,0x01,0x0);
	WAIT_DONE(REG_ND_CTL,0x40,0x40);//add by alexchang 0416-2010
//#endif

	//while (1);
	
	return 0;

	if(is_darwin_cpu()||is_macarthur_cpu()||is_nike_cpu())//Enable NF_WP pin (Write Protect Pin)
	{
		if(g_WP_en)
		{
			setGPIOBit(g_regGPIODirBase,g_NFWP_no,NF_GPIO_OUT);//Set Driection
			setGPIOBit(g_regGPIOOutBase,g_NFWP_no,g_NFWP_value_en);//Set Value
		}
	}

	if((is_NF_CP_Enable_write&&(!mtd->isCPdisable_W))||g_isRandomize)//Clear register
	{
			//REG_WRITE_U32(REG_CP_NF_SET,0);
	}


	if ( REG_READ_U32(REG_ND_DAT) & 0x01 ){
		//up (&sem_NF_CARDREADER);
		printk("[%s] write is not completed at page %d\n", __FUNCTION__, page);
		return -1;
	}

	chip_section = (chipnr * this->page_num) >> 5;
	section = page >> 5;
	index = page & (32-1);

	if (this->erase_page_flag)
		this->erase_page_flag[chip_section+section] &= ~(1 << index);
	mtd->isCPdisable_W = 0;
	//printk("[AT]W block : %u , page: %u \n",page/ppb,page%ppb);
	//up (&sem_NF_CARDREADER);
#if 0
	char *ppptr = (unsigned long)bbbbb-(unsigned long)bbbbb%8+8;
	rtk_read_ecc_page (mtd, chipnr, page, ppptr, oob_buf);
	if(memcmp(ppptr,data_buf, 2048))
		{
			int i, count=0;
			for(i=0;i<2048;i++)
		 		if(ppptr[i] != data_buf[i])
				 count++;
				printk("Data compare error! %d %x %x %x %p\n", count, ppptr[0],
				ppptr[1], ppptr[2], ppptr); //BUG();
				}
	else
		printk("write check OK!\n");
#endif



	return rc;
}
//----------------------------------------------------------------------------
static int rtk_nand_profile (void)
{
	int maxchips = 4;
	char *ptype;
	int pnum = 0;
	struct mtd_partition *mtd_parts;
	struct nand_chip *this = (struct nand_chip *)rtk_mtd->priv;
	int retRba=0;

#if 0
#ifdef CONFIG_MTD_NAND_RTK_RBA_PERCENT
	this->RBA_PERCENT = CONFIG_MTD_NAND_RTK_RBA_PERCENT;
	printk("[Customize]RBA percentage: %d%\n",this->RBA_PERCENT);

#else
	this->RBA_PERCENT = RBA_PERCENT;
	printk("[Default]RBA percentage: %d'%'\n",this->RBA_PERCENT);
#endif
#else
		retRba = rtkNF_getRBAPercent();
	if(retRba == -1)
	{
		rtk_mtd->u32RBApercentage = this->RBA_PERCENT = RBA_PERCENT;
		printk("[**Default**]RBA percentage: %d%\n",this->RBA_PERCENT);
	}
	else
	{
		rtk_mtd->u32RBApercentage = this->RBA_PERCENT = retRba;
		printk("[**Customize**]RBA percentage: %d%\n",this->RBA_PERCENT);
	}
#endif

	if (rtk_nand_scan (rtk_mtd, maxchips) < 0 || rtk_mtd->size == 0){
		printk("%s: Error, cannot do nand_scan(on-board)\n", __FUNCTION__);
		return -ENODEV;
	}
	if ( !(rtk_mtd->oobblock&(0x200-1)) )
		REG_WRITE_U32( REG_PAGE_LEN,rtk_mtd->oobblock >> 9);
	else{
		printk("Error: pagesize is not 512B Multiple");
		return -1;
	}

#ifdef CONFIG_MTD_CMDLINE_PARTS
	ptype = (char *)ptypes[0];
	pnum = parse_mtd_partitions (rtk_mtd, ptypes, &mtd_parts, NULL);
#endif
	if (pnum <= 0) {
		printk(KERN_NOTICE "RTK: using the whole nand as a partitoin\n");
		if(add_mtd_device(rtk_mtd)) {
			printk(KERN_WARNING "RTK: Failed to register new nand device\n");
			return -EAGAIN;
		}
	}else{
		printk(KERN_NOTICE "RTK: using dynamic nand partition\n");
		if (add_mtd_partitions (rtk_mtd, mtd_parts, pnum)) {
			printk("%s: Error, cannot add %s device\n",
					__FUNCTION__, rtk_mtd->name);
			rtk_mtd->size = 0;
			return -EAGAIN;
		}
	}
	return 0;
}
//----------------------------------------------------------------------------
int rtk_read_proc_nandinfo(char *buf, char **start, off_t offset, int len, int *eof, void *data)
{
	struct nand_chip *this = (struct nand_chip *) rtk_mtd->priv;
	int wlen = 0;
	wlen += sprintf(buf+wlen,"nand_PartNum:%s\n", rtk_mtd->PartNum);
	wlen += sprintf(buf+wlen,"nand_size:%llu\n", (uint64_t)this->device_size);
	wlen += sprintf(buf+wlen,"chip_size:%llu\n", (uint64_t)this->chipsize);
	wlen += sprintf(buf+wlen,"block_size:%u\n", rtk_mtd->erasesize);
	wlen += sprintf(buf+wlen,"page_size:%u\n", rtk_mtd->oobblock);
	wlen += sprintf(buf+wlen,"oob_size:%u\n", rtk_mtd->oobsize);
	wlen += sprintf(buf+wlen,"ppb:%u\n", rtk_mtd->erasesize/rtk_mtd->oobblock);
	wlen += sprintf(buf+wlen,"RBA:%u\n", this->RBA);
	wlen += sprintf(buf+wlen,"BBs:%u\n", this->BBs);
	return wlen;
}
//----------------------------------------------------------------------------
static void display_version (void)
{
	const __u8 *revision;
	const __u8 *date;
	char *running = (__u8 *)VERSION;
	strsep(&running, " ");
	strsep(&running, " ");
	revision = strsep(&running, " ");
	date = strsep(&running, " ");
	printk(BANNER " Rev:%s (%s)\n", revision, date);
}
//----------------------------------------------------------------------------
#if 0//!RTK_CP_DISABLE
static int rtk_set_CP_KEY(struct CP_key_info *cp_keyInfo)
{
	#if 0
	REG_WRITE_U32(cp_keyInfo->CP_INI_KEY_0,CP_NF_INI_KEY_0_reg);
	REG_WRITE_U32(cp_keyInfo->CP_INI_KEY_1,CP_NF_INI_KEY_1_reg);
	REG_WRITE_U32(cp_keyInfo->CP_INI_KEY_2,CP_NF_INI_KEY_2_reg);
	REG_WRITE_U32(cp_keyInfo->CP_INI_KEY_3,CP_NF_INI_KEY_3_reg);
	REG_WRITE_U32(cp_keyInfo->CP_KEY_0,CP_NF_KEY_0_reg);
	REG_WRITE_U32(cp_keyInfo->CP_KEY_1,CP_NF_KEY_1_reg);
	REG_WRITE_U32(cp_keyInfo->CP_KEY_2,CP_NF_KEY_2_reg);
	REG_WRITE_U32(cp_keyInfo->CP_KEY_3,CP_NF_KEY_3_reg);
	#endif
	return 0;
}
#endif
#if 1
void endian_swap(unsigned int* x)
{
	*x = (*x>>24) |((*x<<8) & 0x00FF0000) |((*x>>8) & 0x0000FF00) |(*x<<24);
}
#endif
//----------------------------------------------------------------------------

#if 0
static struct platform_driver rtkNF_device = {
    .driver     = {
        .name   = DRIVER_NAME,
        .owner  = THIS_MODULE,
    },
    .remove     = __exit_p(rtk_nand_exit),
    //.suspend    = rtk_nand_suspend,
    //.resume     = rtk_nand_resume,
    .suspend    = NULL,
    .resume     = NULL,
};
#endif

//----------------------------------------------------------------------------
#if RTK_NAND_INTERRUPT
static irqreturn_t rtknf_irq(int irq, void *dev)
{
	//printk("++++f:%d,0x%x\n",g_nfFlag,REG_READ_U32(REG_NAND_ISR));
	//Read isr handler
	//printk("&g_irq_waitq 0x%x\n",&g_irq_waitq);
	if(CHECK_IRQ(NF_READ_TRIGGER,REG_NAND_ISR,NF_DMA_ISR,NF_DMA_ISR))
	{
		CLEAR_IRQ(REG_NAND_ISR,NF_DMA_ISR);
		g_nfFlag=NF_WAKE_UP;
		//wake_up_interruptible(&g_host->irq_waitq);
		wake_up_interruptible(&g_irq_waitq);

		return IRQ_HANDLED;
	}
	else if(CHECK_IRQ(NF_READ_TRIGGER,REG_NAND_ISR,NF_AUTO_TRIG_ISR,NF_AUTO_TRIG_ISR))
	{
		CLEAR_IRQ(REG_NAND_ISR,NF_AUTO_TRIG_ISR);
		return IRQ_HANDLED;
	}

	//Write isr handler
	if(CHECK_IRQ(NF_WRITE_TRIGGER,REG_NAND_ISR,NF_AUTO_TRIG_ISR,NF_AUTO_TRIG_ISR))
	{
		CLEAR_IRQ(REG_NAND_ISR,NF_AUTO_TRIG_ISR);
		g_nfFlag=NF_WAKE_UP;
		//wake_up_interruptible(&g_host->irq_waitq);
		wake_up_interruptible(&g_irq_waitq);
		return IRQ_HANDLED;
	}
	if(CHECK_IRQ(NF_WRITE_TRIGGER,REG_NAND_ISR,NF_POLL_STATUS_ISR,NF_POLL_STATUS_ISR))
	{
		CLEAR_IRQ(REG_NAND_ISR,NF_POLL_STATUS_ISR);
		g_nfFlag=NF_WAKE_UP;
		//wake_up_interruptible(&g_host->irq_waitq);
		wake_up_interruptible(&g_irq_waitq);
		return IRQ_HANDLED;
	}
	else if(CHECK_IRQ(NF_WRITE_TRIGGER,REG_NAND_ISR,NF_DMA_ISR,NF_DMA_ISR))
	{
		CLEAR_IRQ(REG_NAND_ISR,NF_DMA_ISR);
		return IRQ_HANDLED;
	}

	//Erase isr handler
	if(CHECK_IRQ(NF_ERASE_TRIGGER,REG_NAND_ISR,NF_POLL_STATUS_ISR,NF_POLL_STATUS_ISR))
	{
		CLEAR_IRQ(REG_NAND_ISR,NF_POLL_STATUS_ISR);
		g_nfFlag=NF_WAKE_UP;
		//wake_up_interruptible(&g_host->irq_waitq);
		wake_up_interruptible(&g_irq_waitq);
		return IRQ_HANDLED;
	}
	else if(CHECK_IRQ(NF_ERASE_TRIGGER,REG_NAND_ISR,NF_AUTO_TRIG_ISR,NF_AUTO_TRIG_ISR))
	{
		CLEAR_IRQ(REG_NAND_ISR,NF_AUTO_TRIG_ISR);
		return IRQ_HANDLED;
	}

	if((REG_READ_U32(REG_NAND_ISR)&NF_IRQ_MASK))
	{
		if((g_nfFlag==NF_ERASE_TRIGGER)||(g_nfFlag==NF_READ_TRIGGER)||(g_nfFlag==NF_READ_TRIGGER)||(g_nfFlag=NF_WAKE_UP))
		{
			//printk("+++++f:%d,0x%x\n",g_nfFlag,REG_READ_U32(REG_NAND_ISR));
			CLR_REG_NAND_ISR;
			return IRQ_HANDLED;
		}
	}
	//printk("xxxxxf:%d,0x%x\n",g_nfFlag,REG_READ_U32(REG_NAND_ISR));
	 return IRQ_NONE;
}

//----------------------------------------------------------------------------
static int __init rtkNF_probe(struct platform_device *pdev)
{
	struct resource *r;
	int nf_irq,ret;
	/* Allocate memory for MTD device structure and private data */
	g_host = kzalloc(sizeof(struct rtk_nand_host), GFP_KERNEL);
	if (!g_host)
		return -ENOMEM;

	r = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	//init_waitqueue_head(&g_host->irq_waitq);
	init_waitqueue_head(&g_irq_waitq);




	nf_irq = platform_get_irq(pdev, 0);
	ret = request_irq(nf_irq, rtknf_irq, IRQF_SHARED, DRIVER_NAME, g_host);   //rtkcr_interrupt
    if (ret)
    {
        printk(KERN_ERR "%s: cannot assign irq %d\n", DRIVER_NAME, nf_irq);
		return -1;
    }
	return 0;

}
#endif

int board_nand_init(struct nand_chip *nand)
{
	display_version();

	REG_WRITE_U32( REG_CLOCK_ENABLE1,REG_READ_U32(REG_CLOCK_ENABLE1)& (~0x00800000));
	REG_WRITE_U32( REG_NF_CKSEL,0x04 );
	REG_WRITE_U32( REG_CLOCK_ENABLE1,REG_READ_U32(REG_CLOCK_ENABLE1)| (0x00800000));

	// set pinmux to nand.
	REG_WRITE_U32(SYS_muxpad0, 0x55555554);

	// controller init.
	REG_WRITE_U32(REG_PD, 0x1E);
	REG_WRITE_U32(REG_TIME_PARA3, 0x1);
	REG_WRITE_U32(REG_TIME_PARA2, 0x1);
	REG_WRITE_U32(REG_TIME_PARA1, 0x1);

	REG_WRITE_U32(REG_MULTI_CHNL_MODE, 0x20);
	REG_WRITE_U32(REG_READ_BY_PP, 0x0);
	
	//REG32(REG_SPR_DDR_CTL) = 0x30000000 | (PHYS(SPARE_DMA_ADDR) >> 3);

	//REG32(REG_RMZ_SEED_L) = 0x6e;
	//REG32(REG_RMZ_SEED_H) = 0xa3;

        //REG_WRITE_U32(0x1801013c, REG_READ_U32(0x1801013c) & 0xffffffef);

	// reset nand.
	REG_WRITE_U32(REG_ND_CMD, 0xff);
	REG_WRITE_U32(REG_ND_CTL, 0x80);

	//printk("\n\n");
	//printk("0x1800011c = 0x%x \n", REG_READ_U32(0x1800011c));
	//printk("0x18000128 = 0x%x \n", REG_READ_U32(0x18000128));
	//printk("0x18000124 = 0x%x \n", REG_READ_U32(0x18000124));
	
	//printk("SYS_muxpad0:0x%x = 0x%x \n", SYS_muxpad0, REG_READ_U32(SYS_muxpad0));
	//printk("REG_PD:0x%x = 0x%x \n", REG_PD, REG_READ_U32(REG_PD));
	
	//printk("trigger... \n");
	WAIT_DONE(REG_ND_CTL,0x80,0);
	//printk("trigger xfer.\n");
	WAIT_DONE(REG_ND_CTL,0x40,0x40);
	//printk("nand ready. \n");
	//printk("\n\n");
	
	nand->read_id			= rtk_nand_read_id;
	nand->read_ecc_page 	= rtk_read_ecc_page;
	nand->read_oob 			= rtk_read_oob;
	nand->write_ecc_page	= rtk_write_ecc_page;
	nand->write_oob			= rtk_write_oob;

	nand->erase_block 	= rtk_erase_block;
	//nand->suspend		= rtk_nand_suspend;
	//nand->resume		= rtk_nand_resume;
	//nand->sync			= NULL;

	nand->RBA_PERCENT = RBA_PERCENT;
	printk("[Default]RBA percentage: %d'%'\n",nand->RBA_PERCENT);

	return 0;
}

//----------------------------------------------------------------------------
static int  rtk_nand_init (void)
{
	struct nand_chip *this=NULL;
	int rc = 0;
	unsigned long long pin_WP_info;
	//if ( is_venus_cpu() || is_neptune_cpu()||is_mars_cpu())
	//	return -1;

	if ( REG_READ_U32(REG_CLOCK_ENABLE1) & 0x00800000 ){
		display_version();
	}else{
		printk(KERN_ERR "Nand Flash Clock is NOT Open. Installing fails.\n");
		return -1;
	}
	
    //if(is_macarthur_cpu())
    //{
    //        if((REG_READ_U32(0xb8060008)&0x01)!=1)
    //        {
    //                printk("[%s]Doesn't support NAND boot for macathur\n",__FUNCTION__);
    //                return -1;
    //        }
    //}
    
	#if RTK_NAND_INTERRUPT
		//if(is_darwin_cpu()||is_macarthur_cpu()||is_nike_cpu())
		//{
		//	printk("Enable INT for nand flash driver...\n");
		//	REG_WRITE_U32(0xb801a640,(REG_READ_U32(0xb801a640)|0x8));//Enable INT of NAND flash
		//}
		printk("(V)Enable interrupt mechanism of NAND driver.[1011-2011 14:30]\n");
		platform_driver_probe(&rtkNF_device, rtkNF_probe);
		REG_WRITE_U32(REG_NAND_ISREN,(NF_IRQ_MASK|0x01));
	#else
		printk("(X)Enable interrupt mechanism of NAND driver.\n");
	#endif

	g_BootcodeSize = rtkNF_getBootcodeSize();
	g_Factory_param_size = rtkNF_getFactParaSize();

	if(g_BootcodeSize == -1)
	{
		if(is_darwin_cpu()||is_saturn_cpu()||is_macarthur_cpu()||is_nike_cpu())
			g_BootcodeSize = 12*1024*1024;
		else
			g_BootcodeSize = 16*1024*1024;
	}
	if(g_Factory_param_size == -1)
		g_Factory_param_size = 4*1024*1024;

	//g_WP_en = rtkNF_getWPSettingval();
	if(is_darwin_cpu()||is_macarthur_cpu()||is_nike_cpu())
	{
		g_regGPIODirBase = 0x1801bc00;
		g_regGPIOOutBase = 0x1801bc14;

		#if  defined(CONFIG_REALTEK_PCBMGR)
		if(pcb_mgr_get_enum_info_byname("PIN_FLASH_WP",&pin_WP_info)!=0)
		{
			g_WP_en = 0;
			printk("PIN_FLASH_WP is not defined\n");
		}
		else
		{
			 g_WP_en = 1;
			printk("PIN_FLASH_WP defined value 0x%llx\n",pin_WP_info);

			g_NFWP_no = ((unsigned char) ((pin_WP_info >> 8) & 0xFF));
			g_NFWP_value_INV = ((unsigned char) ((pin_WP_info >> 24) & 0xFF));
			if(is_darwin_cpu())
			{
				if(g_NFWP_value_INV==0)
				{
					g_NFWP_value_en = 1;
					g_NFWP_value_dis = 0;
				}
				else
				{
					g_NFWP_value_en = 0;
					g_NFWP_value_dis = 1;
				}

			}
			else
			{
				if(g_NFWP_value_INV==0)
				{
					g_NFWP_value_en = 0;
					g_NFWP_value_dis = 1;
				}
				else
				{
					g_NFWP_value_en = 1;
					g_NFWP_value_dis = 0;
				}
			}
		}
		#else
		 printk("Force disable FLASH_WP!!\n");
		 g_WP_en=0;
		#endif

		printk("g_NFWP_no = %d\n",g_NFWP_no);
		printk("g_NFWP_value_en = %d\n",g_NFWP_value_en);
		printk("g_NFWP_value_dis = %d\n",g_NFWP_value_dis);

	}

	//nRun = nInc = 0;
	printk("Get parameter from register...\n");
	printk("[%s]Bootcode size 0x%x\n",__FUNCTION__,g_BootcodeSize);
	printk("[%s]Factory parameter size 0x%x\n",__FUNCTION__,g_Factory_param_size);

#if	RTK_ARD_ALGORITHM
 	g_u32WinStart = g_RecBlkStart;
 	g_u32WinEnd = g_u32WinStart+g_PorcWindowSize-1;
	g_WinTrigCnt = 0;
	//printk("Start ARD Algorithm Test : ");
	//printk("\n");
#endif

#if 0
	if(platform_info.secure_boot)
	{
		memcpy(&u8regKey0,&platform_info.AES_IMG_KEY[0],4);
		memcpy(&u8regKey1,&platform_info.AES_IMG_KEY[4],4);
		memcpy(&u8regKey2,&platform_info.AES_IMG_KEY[8],4);
		memcpy(&u8regKey3,&platform_info.AES_IMG_KEY[12],4);
        regKey0 = (unsigned int*)&u8regKey0;
		regKey1 = (unsigned int*)&u8regKey1;
		regKey2 = (unsigned int*)&u8regKey2;
		regKey3 = (unsigned int*)&u8regKey3;
		endian_swap(regKey0);
		endian_swap(regKey1);
		endian_swap(regKey2);
		endian_swap(regKey3);

		printk("[%s]Enable NFCP_",__FUNCTION__);
		is_NF_CP_Enable_read = 1;
		is_NF_CP_Enable_write = 0;//When secure boot enable, disable write encryption.

		cpSel = 0;//0:register, 1:CW, 2:E-Fuse

		u32CP_mode = 0x200;//0 for CBC mode, 0x200 for ECB mode
		if(u32CP_mode == 0)
			printk("CBC.\n");
		else if(u32CP_mode == 0x200)
			printk("ECB.\n");
		else
			printk("Unknown.\n");

	}
	else
#endif
	{
		printk("[%s]Disable NFCP...\n",__FUNCTION__);
		is_NF_CP_Enable_write = is_NF_CP_Enable_read = 0;
	}

	//REG_WRITE_U32( 0xb801032c,0x01);	//Enable NAND/CardReader arbitrator
	REG_WRITE_U32( REG_CLOCK_ENABLE1,REG_READ_U32(REG_CLOCK_ENABLE1)& (~0x00800000));
	
	//if (is_jupiter_cpu())
	//	REG_WRITE_U32( REG_NF_CKSEL,0x02 );
	//else if(is_saturn_cpu()||is_darwin_cpu()||is_macarthur_cpu()||is_nike_cpu())
	//	REG_WRITE_U32( REG_NF_CKSEL,0x02 );
	REG_WRITE_U32( REG_NF_CKSEL,0x04 );

	REG_WRITE_U32( REG_CLOCK_ENABLE1,REG_READ_U32(REG_CLOCK_ENABLE1)| (0x00800000));

	//if ( is_jupiter_cpu()||is_macarthur_cpu()||is_nike_cpu() )
	//	REG_WRITE_U32( REG_SPR_DDR_CTL,0x7<<26);
	//else
	//	REG_WRITE_U32(REG_SPR_DDR_CTL,NF_SPR_DDR_CTL_cr_nf_hw_pinmux_ena(1) | NF_SPR_DDR_CTL_spare_ddr_ena(1) | NF_SPR_DDR_CTL_per_2k_spr_ena(1)| NF_SPR_DDR_CTL_spare_dram_sa(0)); //set spare2ddr func. 4=>0.5k spe2ddr_ena at A000F000
	REG_WRITE_U32(REG_SPR_DDR_CTL, NF_SPR_DDR_CTL_spare_ddr_ena(1) | NF_SPR_DDR_CTL_per_2k_spr_ena(1)| NF_SPR_DDR_CTL_spare_dram_sa(0)); //set spare2ddr func. 4=>0.5k spe2ddr_ena at A000F000

	rtk_mtd = kmalloc (MTDSIZE, GFP_KERNEL);
	if ( !rtk_mtd ){
		printk("%s: Error, no enough memory for rtk_mtd\n",__FUNCTION__);
		rc = -ENOMEM;
		goto EXIT;
	}
	memset ( (char *)rtk_mtd, 0, MTDSIZE);
	rtk_mtd->priv = this = (struct nand_chip *)(rtk_mtd+1);

	REG_WRITE_U32( REG_PD,~(0x1 << 0) );
	REG_WRITE_U32( REG_ND_CMD,CMD_RESET );

	//if(is_jupiter_cpu()||is_macarthur_cpu()||is_nike_cpu())
	//	REG_WRITE_U32( REG_ND_CTL,(0x80 | 0x00));
	//else
	//	REG_WRITE_U32( REG_ND_CTL,(NF_ND_CTL_xfer(0x01) | NF_ND_CTL_tran_mode(0)) );
	REG_WRITE_U32( REG_ND_CTL,(NF_ND_CTL_xfer(0x01) | NF_ND_CTL_tran_mode(0)) );

	WAIT_DONE(REG_ND_CTL,0x80,0);
	WAIT_DONE(REG_ND_CTL,0x40,0x40);

	REG_WRITE_U32( REG_MULTI_CHNL_MODE,0x00);
	REG_WRITE_U32( REG_TIME_PARA1,NF_TIME_PARA1_T1(0x0));
	REG_WRITE_U32( REG_TIME_PARA2,NF_TIME_PARA2_T2(0x0));
	REG_WRITE_U32( REG_TIME_PARA3,NF_TIME_PARA3_T3(0x0));

	//if(!is_jupiter_cpu()||!is_saturn_cpu()||!is_darwin_cpu()||!is_macarthur_cpu()||is_nike_cpu())
	//	REG_WRITE_U32( REG_DELAY_CTL,NF_DELAY_CTL_T_WHR_ADL(0x09) );
	REG_WRITE_U32( REG_DELAY_CTL,NF_DELAY_CTL_T_WHR_ADL(0x09) );

	REG_WRITE_U32( REG_MULTI_CHNL_MODE,NF_MULTI_CHNL_MODE_edo(0x01));
	REG_WRITE_U32( REG_ECC_STOP,NF_ECC_STOP_ecc_n_stop(0x01));

	REG_WRITE_U32( REG_ND_CA0,0);
	REG_WRITE_U32( REG_ND_CA1,0);
	this->read_id		= rtk_nand_read_id;
	this->read_ecc_page 	= rtk_read_ecc_page;
	this->read_oob 		= rtk_read_oob;
	this->write_ecc_page	= rtk_write_ecc_page;
	this->write_oob		= rtk_write_oob;

	this->erase_block 	= rtk_erase_block;
	this->suspend		= rtk_nand_suspend;
	this->resume		= rtk_nand_resume;
	this->sync		= NULL;

	if( rtk_nand_profile() < 0 ){
		rc = -1;
		goto EXIT;
	}

	if(g_isRandomize)
	{
		printk("[%s]Randomized enabled \n ",__FUNCTION__);
		cpSel = 0;//0:register, 1:CW, 2:E-Fuse
        u32CP_mode = 0x0;//0 for CBC mode, 0x200 for ECB mode
	}



	g_NF_pageSize = page_size = rtk_mtd->oobblock;
	oob_size = rtk_mtd->oobsize;
	ppb = (rtk_mtd->erasesize)/(rtk_mtd->oobblock);

	printk("[%s]Ecc bit select %u\n",__FUNCTION__,this->ecc_select);
	create_proc_read_entry("nandinfo", 0, NULL, rtk_read_proc_nandinfo, NULL);

EXIT:
	if (rc < 0) {
		if (rtk_mtd){
			//del_mtd_partitions (rtk_mtd);
			if (this->g_databuf)
				kfree(this->g_databuf);
			if(this->g_oobbuf)
				kfree(this->g_oobbuf);
			if (this->erase_page_flag){
				unsigned int flag_size =  (this->numchips * this->page_num) >> 3;
				//unsigned int mempage_order = get_order(flag_size);
				//free_pages((unsigned long)this->erase_page_flag, mempage_order);
			}
			kfree(rtk_mtd);
		}
		//remove_proc_entry("nandinfo", NULL);
	}else
		printk(KERN_INFO "Realtek Nand Flash Driver is successfully installing.\n");

	return rc;
}
//----------------------------------------------------------------------------
void  rtk_nand_exit (void)
{
	struct nand_chip *this = NULL;
	//free_irq(g_host->irq, g_host);

	if (rtk_mtd){
		//del_mtd_partitions (rtk_mtd);
		this = (struct nand_chip *)rtk_mtd->priv;
		if (this->g_databuf)
			kfree(this->g_databuf);
		if(this->g_oobbuf)
			kfree(this->g_oobbuf);
		if (this->erase_page_flag){
			unsigned int flag_size =  (this->numchips * this->page_num) >> 3;
			//unsigned int mempage_order = get_order(flag_size);
			//free_pages((unsigned long)this->erase_page_flag, mempage_order);
		}
		kfree(rtk_mtd);

	}
	//remove_proc_entry("nandinfo", NULL);
}
//----------------------------------------------------------------------------
module_init(rtk_nand_init);
module_exit(rtk_nand_exit);
MODULE_AUTHOR("AlexChang <alexchang2131@realtek.com>");
MODULE_DESCRIPTION("Realtek NAND Flash Controller Driver");
#endif




