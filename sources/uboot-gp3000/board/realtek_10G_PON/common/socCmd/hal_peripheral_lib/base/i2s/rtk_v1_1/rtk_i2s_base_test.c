/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#include "basic_types.h"
#include "diag.h"
#include <common.h>
#include <asm/cache.h>
//#include "rtk_hal_i2s.h"
//#include "rtk_i2s_base.h"
#include "peripheral.h"
#include "section_config.h"
#include "rtk_i2s_base_test.h"

//#include <linux/types.h>
//#include <common.h>
//#include <asm/armv7.h>
//#include <asm/utils.h>


#ifndef ARCH_DMA_MINALIGN
#define ARCH_DMA_MINALIGN 64
#endif

#if defined(CONFIG_KERNEL) && !TASK_SCHEDULER_DISABLED
  //#define I2S_FREE_RTOS_TASK_LOOPBACK_TEST 1
#endif


#define DATA_SIZE   2048

// I2S test config start

//#define SOC_TYPE_8195A 1
//#define SOC_TYPE_8198E 1
//#define SOC_TYPE_8196F 1
//#define SOC_TYPE_8198F 1
#define SOC_TYPE_8277C 1


#ifdef SOC_TYPE_8198E
  //#define I2S_IRQ (32+36) //old bit file
  #define I2S_IRQ (32+25)
#endif

#ifdef SOC_TYPE_8196F
#define I2S_IRQ 26
#endif
#ifdef SOC_TYPE_8198F
#define I2S_IRQ (107+32)
#endif
#ifdef SOC_TYPE_8277C
#define I2S_IRQ (32+32)
#endif

#define FLYSKY_DEBUG 0
#define I2S_DBG_LVL 3 //range 0~3
//#define LOOPBACK_SIMPLE_PAT 1  //use lrclk(word selec) as data input (when NO ALC5621_OR_ALC5651)

#define I2S_TEST_TIME_SEC 30
//#define I2S_TEST_TIME_SEC (43200)

#define I2S_CODEC_ALC5621_OR_ALC5651 0

#ifdef I2S_CODEC_ALC5621_OR_ALC5651
//#define I2S_PLAY_TONE_TEST 1
#define I2S_MIC_LOOPBACK_SPEAKER_TEST 1
#endif

//#define I2S_IN_FPGA 1 // fpga is slow, check less sample

//lock bus before disable i2s
//#define LOCK_LX_BUS 0

#if I2S_CODEC_ALC5621_OR_ALC5651
//#define I2S_INTERNAL_LOOPBACK 1
#define I2S_INTERNAL_LOOPBACK 0
#else
#define I2S_INTERNAL_LOOPBACK 0
#endif
// test delay tx/rx
#define I2S_8277C_TEST_MODE 0
#define I2S_LR_CH_LOOP 1

//select slave mode or master mode
//#define I2S_SLAVE_MODE 1

//select enable endian swap or not
#define I2S_ENDIAN_SWAP_ENABLE 0
//#define I2S_ENDIAN_SWAP_ENABLE 1

//select enable edge swap or not
#define I2S_EDGE_SWAP_ENABLE 0
//#define I2S_EDGE_SWAP_ENABLE 1

#define COMPARE_EVERY_INT
#define COMPARE_OFFSET

#define I2S_BURST_SIZE 0x7

//#define I2S_CLK_22P579MHZ 1 //using 44.1khz i2s sample rate

#ifdef SOC_TYPE_8195A
#define I2S_INTERFACE_NUM 2
#elif defined(SOC_TYPE_8198E) || defined(SOC_TYPE_8196F)
#define I2S_INTERFACE_NUM 1
#else //default, 8198F
#define I2S_INTERFACE_NUM 1
#endif

#if defined (SOC_TYPE_8198E)
#define LOCK_LX_CONTROL_REGISTER 0xF8000090
#define LOCK_LX_STATUS_REGISTER 0xF8000090
#define LOCK_LX1_BUS	(1<<3)
#define LOCK_LX1_OK	(1<<13)
#elif defined (SOC_TYPE_8196F)
#define LOCK_LX_CONTROL_REGISTER 0xB8000090
#define LOCK_LX_STATUS_REGISTER 0xB8000090
#define LOCK_LX1_BUS	(1<<3)
#define LOCK_LX1_OK	(1<<13)
#define VIR_TO_PHY(x) (x & (~0xa0000000))
#endif
// I2S test config end


#define I2S_PAGE_TIME	100	// unit 0.1ms, test case 10, 25, 50 100, 200, 400

#if defined(SOC_TYPE_8195A)
  #define I2S_PAGE_SIZE	(96*I2S_PAGE_TIME*2/10) // max 96kHz, max 2channel, 24bit wordlength
#elif defined(I2S_CODEC_ALC5621_OR_ALC5651) && defined(I2S_PLAY_TONE_TEST)
  #define I2S_PAGE_SIZE (384*I2S_PAGE_TIME*6/10) // max 96kHz, max 5.1channel, 24bit wordlength
#else
  #define I2S_PAGE_SIZE (384*I2S_PAGE_TIME*6/10) // max 96kHz, max 2channel, 24bit wordlength
#endif

#if (I2S_PAGE_SIZE>4096)
  #undef I2S_PAGE_SIZE
  #define I2S_PAGE_SIZE 4096
  //#error "max page size supported is 4096"
#endif

#define MAX_I2S_PAGE_NUM 4
#define I2S_PAGE_NUM     4

#define DATABUFSIZE (10*I2S_PAGE_SIZE*4)
#if defined(SOC_TYPE_8195A)
#define DMA_FROM_SECTION SRAM_BF_DATA_SECTION
#define DMA_TO_SECTION SRAM_BF_DATA_SECTION
// SDRAM_DATA_SECTION, SRAM_BD_DATA_SECTION, SRAM_BF_DATA_SECTION
#elif defined(SOC_TYPE_8198F)
#define DMA_FROM_SECTION 
#define DMA_TO_SECTION 
#define DMA_SELECT_SDRAM 1 //select sdram for test
#elif defined(SOC_TYPE_8277C)
#define DMA_FROM_SECTION
#define DMA_TO_SECTION
#define DMA_SELECT_SDRAM 1 //select sdram for test
#else
#define DMA_FROM_SECTION 
#define DMA_TO_SECTION 

#define DMA_SELECT_SDRAM 1 //select sdram for test
//#define DMA_SELECT_SDRAM 0 //select sram for test
//#define DMA_SELECT_ROM 1 //select sram for test


#endif

#if 1
DMA_FROM_SECTION
static s16 i2s_tx_buf[MAX_I2S_PAGE_NUM*I2S_PAGE_SIZE*2+32]__attribute__ ((aligned (0x40)));

DMA_TO_SECTION
static s16 i2s_rx_buf[MAX_I2S_PAGE_NUM*I2S_PAGE_SIZE*2+32]__attribute__ ((aligned (0x40)));
#else
static s16 *i2s_tx_buf = 0xc0000000;
static s16 *i2s_rx_buf = 0xc0004000;
#endif

#if I2S_8277C_TEST_MODE
u32 i2s_8277c_inc_value;
s32 t_oldold_value;
s32 t_old_value;
s32 t_value;
s32 t_num;
s32 myrx_buffer[100][5];
u32 i2s_rxpage_cnt;
#endif

u32 i2s_test_time_sec  = I2S_TEST_TIME_SEC;
struct i2s_pkt {
    s16  *payload;
    s32    *payload_24bit;
    s32    *payload_32bit;
};
static struct i2s_pkt i2s_tx[I2S_INTERFACE_NUM];
static struct i2s_pkt i2s_rx[I2S_INTERFACE_NUM];

#if defined(I2S_CODEC_ALC5621_OR_ALC5651) && defined(I2S_MIC_LOOPBACK_SPEAKER_TEST)
SRAM_BF_DATA_SECTION
static u8 databuf[DATABUFSIZE+512];
u32 tx_read_index, rx_write_index;
#endif

#if 1//!defined(I2S_CODEC_ALC5621_OR_ALC5651)
//SRAM_BF_DATA_SECTION
static short i2s_comp[I2S_PAGE_NUM*I2S_PAGE_SIZE*2+32];
//SRAM_BF_DATA_SECTION
static int i2s_comp_24bit[I2S_PAGE_NUM*I2S_PAGE_SIZE+16];
//SRAM_BF_DATA_SECTION
static int i2s_comp_32bit[I2S_PAGE_NUM*I2S_PAGE_SIZE+16];
#endif

s32 i2s_rate;
u32 i2s_page_size;
u32 i2s_cnt;
u32 i2s_isr_cnt;
u32 i2s_err_cnt;
u32 i2s_page_time;

static s32 shift=0;

#ifdef I2S_PLAY_TONE_TEST
static s32 tone_phase = 0;
static s32 tone_phase_ad;
static s32 play_channel;
static s32 play_channel_now;
static s32 sample_count;
static s32 i2s_allchannel;

/*
  sine wave 500hz matlab command (sample rate 8000hz, 500/8000=1/16)
  16bit (max 32767 0dB)
 [32767*sin(2*pi*0/16) 32767*sin(2*pi*1/16) 32767*sin(2*pi*2/16) 32767*sin(2*pi*3/16)
 32767*sin(2*pi*4/16) 32767*sin(2*pi*5/16) 32767*sin(2*pi*6/16) 32767*sin(2*pi*7/16)
 32767*sin(2*pi*8/16) 32767*sin(2*pi*9/16) 32767*sin(2*pi*10/16) 32767*sin(2*pi*11/16)
 32767*sin(2*pi*12/16) 32767*sin(2*pi*13/16) 32767*sin(2*pi*14/16) 32767*sin(2*pi*15/16)]

  24bit (max 4194304 -12dB)
 [4194304*sin(2*pi*0/16) 4194304*sin(2*pi*1/16) 4194304*sin(2*pi*2/16) 4194304*sin(2*pi*3/16)
 4194304*sin(2*pi*4/16) 4194304*sin(2*pi*5/16) 4194304*sin(2*pi*6/16) 4194304*sin(2*pi*7/16)
 4194304*sin(2*pi*8/16) 4194304*sin(2*pi*9/16) 4194304*sin(2*pi*10/16) 4194304*sin(2*pi*11/16)
 4194304*sin(2*pi*12/16) 4194304*sin(2*pi*13/16) 4194304*sin(2*pi*14/16) 4194304*sin(2*pi*15/16)]

  32bit (max 536870912  2^31/4)
[536870912*sin(2*pi*0/16) 536870912*sin(2*pi*1/16) 536870912*sin(2*pi*2/16) 536870912*sin(2*pi*3/16)         
536870912*sin(2*pi*4/16) 536870912*sin(2*pi*5/16) 536870912*sin(2*pi*6/16) 536870912*sin(2*pi*7/16)          
536870912*sin(2*pi*8/16) 536870912*sin(2*pi*9/16) 536870912*sin(2*pi*10/16) 536870912*sin(2*pi*11/16)        
536870912*sin(2*pi*12/16) 536870912*sin(2*pi*13/16) 536870912*sin(2*pi*14/16) 536870912*sin(2*pi*15/16)]
*/

s16 i2s_vpat[16]={0, 12539, 23170, 30273, 32767, 30273, 23170, 12539,
                  0, -12539, -23170, -30273, -32767, -30273, -23170, -12539};
s16 i2s_vpat_8[16]={0, 12539/8, 23170/8, 30273/8, 32767/8, 30273/8, 23170/8, 12539/8,
                  0, -12539/8, -23170/8, -30273/8, -32767/8, -30273/8, -23170/8, -12539/8};

s32 i2s_vpat24bit[16]={0, 1605090, 2965820, 3875031, 4194304, 3875031, 2965820, 1605090,
                       0, -1605090, -2965820, -3875031, -4194304, -3875031, -2965820, -1605090};
s32 i2s_vpat32bit[16]={0, 205451603, 379625062, 496004047, 536870912, 496004047, 379625062, 205451603,
                      0, -205451603, -379625062, -496004047, -536870912, -496004047, -379625062, -205451603};
#define I2S_TONEGEN_TABLE 1 //select tone gen by table or function
#endif

//extern void *memset( void *s, int c, SIZE_T n );
//extern void *memcpy( void *s1, const void *s2, SIZE_T n );

typedef struct _I2S_ADAPTER_ {
    PHAL_I2S_OP pHalI2sOp;
    PHAL_I2S_ADAPTER pHalI2sAdapter;
}I2S_ADAPTER, *PI2S_ADAPTER;

typedef struct _I2S_VERIFY_PARAMETER_ {
    //u8  GdmaAutoTest;
    u32  I2sTxMemoryTestType:1;
    u32  I2sRxMemoryTestType:1;
    u32  I2sWordLength:2;//16bits or 24bits or 32bits
    u32  I2sMode:2;//I2S, Left justified, Right justified
    u32  I2sLRSwap:1;
    u32  I2sIndex:1;
    u32  I2sTestMode:4;//play tone or internal loopback or external loopback
    u32  I2sCHNum:2;//i2s channel number mono or stereo or 5.1ch
    u32 I2s_TRX_Act:2;//rx, tx or tx+rx
    u32 I2sPageSize:12;
    u32 I2sPageNum:2;
    u32 I2sSampleRate:5;
    u32 I2sVerifyLoop;
    s16  *pTxPage; 
    s16  *pRxPage;
    s32  *pTxPage_24bit; 
    s32  *pRxPage_24bit;
    s32  *pTxPage_32bit; 
    s32  *pRxPage_32bit;
}I2S_VERIFY_PARAMETER, *PI2S_VERIFY_PARAMETER;

s32 i2s_ch_channelnum_tab1[]={1, 2, 6};


I2S_ADAPTER            TestI2sAdapte;
I2S_VERIFY_PARAMETER   I2sVerifyPere;
#if 0
SRAM_BD_DATA_SECTION
u8  BDTX[DATA_SIZE];
SRAM_BD_DATA_SECTION
u8  BDRX[DATA_SIZE];

SRAM_BF_DATA_SECTION
u8  BFTX[DATA_SIZE];
SRAM_BF_DATA_SECTION
u8  BFRX[DATA_SIZE];
#endif
HAL_I2S_OP HalI2sOp;
HAL_I2S_ADAPTER HalI2sAdapter;

SRAM_BF_DATA_SECTION
PI2S_VERI_PARA pI2SVeriAdp;

SRAM_BF_DATA_SECTION
volatile u32 I2SVeriGCnt;

SRAM_BF_DATA_SECTION
volatile u32 I2SVeriErrCnt;

SRAM_BF_DATA_SECTION
volatile u32 I2SVeriShift;


static u32 I2SChanTxPage[4] = {I2sTxP0OK, I2sTxP1OK, I2sTxP2OK, I2sTxP3OK};
static u32 I2SChanRxPage[4] = {I2sRxP0OK, I2sRxP1OK, I2sRxP2OK, I2sRxP3OK};

static u32 i2s_txpage[I2S_INTERFACE_NUM];
static u32 i2s_rxpage[I2S_INTERFACE_NUM];

VOID
I2SRegisterDump
(
    IN VOID     *Data
)
{
#if 1//FLYSKY_DEBUG
#if (I2S_DBG_LVL>=2)
    PI2S_ADAPTER pI2sAdapter = (PI2S_ADAPTER) Data;
    PHAL_I2S_ADAPTER pHalI2sAdapter = pI2sAdapter->pHalI2sAdapter;
    u8 I2sIndex = pHalI2sAdapter->I2sIndex;

    printf("I2sIndex = %d\n", I2sIndex);
    printf("REG_I2S_CTL (offset : 0x%x) = %x\n",REG_I2S_CTL, HAL_I2S_READ32(I2sIndex, REG_I2S_CTL));
    printf("REG_I2S_TX_PAGE_PTR (offset : 0x%x) = %x\n",REG_I2S_TX_PAGE_PTR, HAL_I2S_READ32(I2sIndex, REG_I2S_TX_PAGE_PTR));
    printf("REG_I2S_RX_PAGE_PTR (offset : 0x%x) = %x\n",REG_I2S_RX_PAGE_PTR, HAL_I2S_READ32(I2sIndex, REG_I2S_RX_PAGE_PTR));
    printf("REG_I2S_SETTING (offset : 0x%x) = %x\n",REG_I2S_SETTING, HAL_I2S_READ32(I2sIndex, REG_I2S_SETTING));
    printf("REG_I2S_TX_MASK_INT (offset : 0x%x) = %x\n",REG_I2S_TX_MASK_INT, HAL_I2S_READ32(I2sIndex, REG_I2S_TX_MASK_INT));
    printf("REG_I2S_TX_STATUS_INT (offset : 0x%x) = %x\n",REG_I2S_TX_STATUS_INT, HAL_I2S_READ32(I2sIndex, REG_I2S_TX_STATUS_INT));
    printf("REG_I2S_RX_MASK_INT (offset : 0x%x) = %x\n",REG_I2S_RX_MASK_INT, HAL_I2S_READ32(I2sIndex, REG_I2S_RX_MASK_INT));
    printf("REG_I2S_RX_STATUS_INT (offset : 0x%x) = %x\n",REG_I2S_RX_STATUS_INT, HAL_I2S_READ32(I2sIndex, REG_I2S_RX_STATUS_INT));
    printf("REG_I2S_TX_PAGE0_OWN (offset : 0x%x) = %x\n",REG_I2S_TX_PAGE0_OWN, HAL_I2S_READ32(I2sIndex, REG_I2S_TX_PAGE0_OWN));
    printf("REG_I2S_TX_PAGE1_OWN (offset : 0x%x) = %x\n",REG_I2S_TX_PAGE1_OWN, HAL_I2S_READ32(I2sIndex, REG_I2S_TX_PAGE1_OWN));
    printf("REG_I2S_TX_PAGE2_OWN (offset : 0x%x) = %x\n",REG_I2S_TX_PAGE2_OWN, HAL_I2S_READ32(I2sIndex, REG_I2S_TX_PAGE2_OWN));
    printf("REG_I2S_TX_PAGE3_OWN (offset : 0x%x) = %x\n",REG_I2S_TX_PAGE3_OWN, HAL_I2S_READ32(I2sIndex, REG_I2S_TX_PAGE3_OWN));
    printf("REG_I2S_RX_PAGE0_OWN (offset : 0x%x) = %x\n",REG_I2S_RX_PAGE0_OWN, HAL_I2S_READ32(I2sIndex, REG_I2S_RX_PAGE0_OWN));
    printf("REG_I2S_RX_PAGE1_OWN (offset : 0x%x) = %x\n",REG_I2S_RX_PAGE1_OWN, HAL_I2S_READ32(I2sIndex, REG_I2S_RX_PAGE1_OWN));
    printf("REG_I2S_RX_PAGE2_OWN (offset : 0x%x) = %x\n",REG_I2S_RX_PAGE2_OWN, HAL_I2S_READ32(I2sIndex, REG_I2S_RX_PAGE2_OWN));
    printf("REG_I2S_RX_PAGE3_OWN (offset : 0x%x) = %x\n",REG_I2S_RX_PAGE3_OWN, HAL_I2S_READ32(I2sIndex, REG_I2S_RX_PAGE3_OWN));
    printf("REG_I2S_VERSION_ID (offset : 0x%x) = %x\n",REG_I2S_VERSION_ID, HAL_I2S_READ32(I2sIndex, REG_I2S_VERSION_ID));
#endif
#endif
}


VOID
I2SLoopBackIrqHandle
(
    IN VOID      *Data
);
#ifdef LOCK_LX_BUS

VOID
LockLx1Bus
(
    VOID    
)
{
	long i, temp;

	temp = *((volatile unsigned long *)LOCK_LX_CONTROL_REGISTER);
	(*(volatile unsigned long *)LOCK_LX_CONTROL_REGISTER) = temp | LOCK_LX1_BUS;
	for (i=0 ; i<8000 ; i++) {
		temp = *((volatile unsigned long *)LOCK_LX_STATUS_REGISTER);
		if (temp & LOCK_LX1_OK)
			break;
	}        
	if (i>3500)
		DBG_8195A_DRIVER("lock time out=%d\n", i);
	else
		DBG_8195A_DRIVER("lock time =%d\n", i);
}

VOID
UnlockLx1Bus
(
    VOID    
)
{
	long temp;

	temp = *((volatile unsigned long *)LOCK_LX_CONTROL_REGISTER);
	(*(volatile unsigned long *)LOCK_LX_CONTROL_REGISTER) = temp&(~LOCK_LX1_BUS);
}
#endif
VOID
InitI2sTestCore
(
    IN VOID      *Data
)
{
    PI2S_VERIFY_PARAMETER pI2sVerifyPara = (PI2S_VERIFY_PARAMETER)Data;
    u32 i;

#if I2S_8277C_TEST_MODE
    i2s_8277c_inc_value=0;
#endif


    if (pI2sVerifyPara->I2sWordLength == 1) { // word length 24bit
#if DMA_SELECT_SDRAM
        i2s_tx[pI2sVerifyPara->I2sIndex].payload_24bit = (s32 *)i2s_tx_buf;
        i2s_rx[pI2sVerifyPara->I2sIndex].payload_24bit = (s32 *)i2s_rx_buf;
#elif (DMA_SELECT_ROM && SOC_TYPE_8196F)
	i2s_tx[pI2sVerifyPara->I2sIndex].payload_24bit = (s32 *)(0xbfd00000+(0<<10)); //4kword per page, 16kword/4page. 64kbyte
	i2s_rx[pI2sVerifyPara->I2sIndex].payload_24bit = (s32 *)(0xbfd00000+(32<<10));
#else
	#if defined(SOC_TYPE_8198E)
	i2s_tx[pI2sVerifyPara->I2sIndex].payload_24bit = (s32 *)(0xd0000000+(0<<10)); //4kword per page, 16kword/4page. 64kbyte
	i2s_rx[pI2sVerifyPara->I2sIndex].payload_24bit = (s32 *)(0xd0000000+(64<<10));
	#elif defined(SOC_TYPE_8196F)
	i2s_tx[pI2sVerifyPara->I2sIndex].payload_24bit = (s32 *)(0xbfe00000+(0<<10)); //4kword per page, 16kword/4page. 64kbyte
	i2s_rx[pI2sVerifyPara->I2sIndex].payload_24bit = (s32 *)(0xbfe00000+(8<<10));	
	#endif
#endif
#if I2S_8277C_TEST_MODE
        for (i=0 ; i< ((pI2sVerifyPara->I2sPageSize+1)*(pI2sVerifyPara->I2sPageNum+1)) ; i++) {
                i2s_tx[pI2sVerifyPara->I2sIndex].payload_24bit[i] = i2s_8277c_inc_value;
                i2s_8277c_inc_value+= 0x100;
        }
#else
#if !I2S_CODEC_ALC5621_OR_ALC5651
        for (i=0 ; i< ((pI2sVerifyPara->I2sPageSize+1)*(pI2sVerifyPara->I2sPageNum+1)+16) ; i++) {
            s32 raw_pcm;
            raw_pcm = ((i+101)*1705-67)&0xffffff;
            if (i%2 == 1)
                raw_pcm ^= 0x800000;
                i2s_tx[pI2sVerifyPara->I2sIndex].payload_24bit[i] = raw_pcm;
                i2s_comp_24bit[i]=raw_pcm;
        }

#endif
#endif
#if (I2S_CODEC_ALC5621_OR_ALC5651)
        memset( i2s_tx[pI2sVerifyPara->I2sIndex].payload_24bit, 0, ((pI2sVerifyPara->I2sPageSize+1)*(pI2sVerifyPara->I2sPageNum+1)+16)*4);
  #if defined(I2S_PLAY_TONE_TEST)
    #ifdef I2S_TONEGEN_TABLE
        for (i=0 ; i< ((pI2sVerifyPara->I2sPageSize+1)*(pI2sVerifyPara->I2sPageNum+1)) ; i++) {
            if (sample_count==play_channel_now) {
                i2s_tx[pI2sVerifyPara->I2sIndex].payload_24bit[i] = i2s_vpat24bit[tone_phase];
                tone_phase++;
                if (tone_phase == 16)
                    tone_phase = 0;
            } else
                i2s_tx[pI2sVerifyPara->I2sIndex].payload_24bit[i] = 0;
            sample_count++;
            if (sample_count == i2s_allchannel)
                sample_count = 0;
        }
    #else
        I2SToneGens24bit((pI2sVerifyPara->I2sPageSize+1)*(pI2sVerifyPara->I2sPageNum+1), i2s_tx[pI2sVerifyPara->I2sIndex].payload_24bit);
    #endif
  #endif

#endif
#if 1 //FLYSKY_DEBUG
#if (I2S_DBG_LVL>=3)
        u32 print_size;
        print_size = (pI2sVerifyPara->I2sPageSize+1>270)? 270: pI2sVerifyPara->I2sPageSize+1;
        printf("i2s tx page data:\n");
        for (i=0 ; i<(print_size*(pI2sVerifyPara->I2sPageNum+1))+16; i++) {
            if ((i%8) == 7)
                printf(" %x\n", i2s_tx[pI2sVerifyPara->I2sIndex].payload_24bit[i]);
            else
                printf(" %x ", i2s_tx[pI2sVerifyPara->I2sIndex].payload_24bit[i]);
        }
#endif
#endif

        memset( i2s_rx[pI2sVerifyPara->I2sIndex].payload_24bit, 0, ((pI2sVerifyPara->I2sPageSize+1)*(pI2sVerifyPara->I2sPageNum+1)+16)*4);

        pI2sVerifyPara->pTxPage_24bit = i2s_tx[pI2sVerifyPara->I2sIndex].payload_24bit;
        pI2sVerifyPara->pRxPage_24bit = i2s_rx[pI2sVerifyPara->I2sIndex].payload_24bit;
        #if defined(SOC_TYPE_8196F)
        HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_TX_PAGE_PTR, (VIR_TO_PHY((u32)pI2sVerifyPara->pTxPage_24bit)));
        HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_RX_PAGE_PTR, (VIR_TO_PHY((u32)pI2sVerifyPara->pRxPage_24bit)));
        #else
        HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_TX_PAGE_PTR, (u32)pI2sVerifyPara->pTxPage_24bit);
        HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_RX_PAGE_PTR, (u32)pI2sVerifyPara->pRxPage_24bit);
        #endif
    flush_dcache_range(((unsigned long)i2s_tx[pI2sVerifyPara->I2sIndex].payload_24bit)&(~(ARCH_DMA_MINALIGN-1)), (((unsigned long)i2s_tx[pI2sVerifyPara->I2sIndex].payload_24bit)+(MAX_I2S_PAGE_NUM*I2S_PAGE_SIZE*2+32)*2)&(~(ARCH_DMA_MINALIGN-1)));
    flush_dcache_range(((unsigned long)i2s_rx[pI2sVerifyPara->I2sIndex].payload_24bit)&(~(ARCH_DMA_MINALIGN-1)), (((unsigned long)i2s_rx[pI2sVerifyPara->I2sIndex].payload_24bit)+(MAX_I2S_PAGE_NUM*I2S_PAGE_SIZE*2+32)*2)&(~(ARCH_DMA_MINALIGN-1)));
    invalidate_dcache_range(((unsigned long)i2s_rx[pI2sVerifyPara->I2sIndex].payload_24bit)&(~(ARCH_DMA_MINALIGN-1)), (((unsigned long)i2s_rx[pI2sVerifyPara->I2sIndex].payload_24bit)+(MAX_I2S_PAGE_NUM*I2S_PAGE_SIZE*2+32)*2)&(~(ARCH_DMA_MINALIGN-1)));
    } else if (pI2sVerifyPara->I2sWordLength == 2) { // word length 32bit
#if DMA_SELECT_SDRAM
	#if 1
        i2s_tx[pI2sVerifyPara->I2sIndex].payload_32bit = (s32 *)i2s_tx_buf;
        i2s_rx[pI2sVerifyPara->I2sIndex].payload_32bit = (s32 *)i2s_rx_buf;
	#else
	i2s_tx[pI2sVerifyPara->I2sIndex].payload_32bit = (s32 *)(0x09000000+(0<<10));
	i2s_rx[pI2sVerifyPara->I2sIndex].payload_32bit = (s32 *)(0x09000000+(16<<10));
	#endif
#elif (DMA_SELECT_ROM && SOC_TYPE_8196F)
	i2s_tx[pI2sVerifyPara->I2sIndex].payload_32bit = (s32 *)(0xbfd00000+(0<<10)); //4kword per page, 16kword/4page. 64kbyte
	i2s_rx[pI2sVerifyPara->I2sIndex].payload_32bit = (s32 *)(0xbfd00000+(32<<10));
#else
	#if defined(SOC_TYPE_8198E)
	i2s_tx[pI2sVerifyPara->I2sIndex].payload_32bit = (s32 *)(0xd0000000+(0<<10)); //4kword per page, 16kword/4page. 64kbyte
	i2s_rx[pI2sVerifyPara->I2sIndex].payload_32bit = (s32 *)(0xd0000000+(64<<10));
	#elif defined(SOC_TYPE_8196F)
	i2s_tx[pI2sVerifyPara->I2sIndex].payload_32bit = (s32 *)(0xbfe00000+(0<<10)); //4kword per page, 16kword/4page. 64kbyte
	i2s_rx[pI2sVerifyPara->I2sIndex].payload_32bit = (s32 *)(0xbfe00000+(8<<10));	
	#endif
#endif
#if I2S_8277C_TEST_MODE
        for (i=0 ; i< ((pI2sVerifyPara->I2sPageSize+1)*(pI2sVerifyPara->I2sPageNum+1)) ; i++) {

                i2s_tx[pI2sVerifyPara->I2sIndex].payload_32bit[i] = i2s_8277c_inc_value;
                i2s_8277c_inc_value+=0x8000;
        }

#else
#if !I2S_CODEC_ALC5621_OR_ALC5651
        for (i=0 ; i< ((pI2sVerifyPara->I2sPageSize+1)*(pI2sVerifyPara->I2sPageNum+1)+16) ; i++) {
            s32 raw_pcm;
            raw_pcm = ((i+101)*103705-67);
            if (i%2 == 1)
                raw_pcm ^= 0x80000000;
                i2s_tx[pI2sVerifyPara->I2sIndex].payload_32bit[i] = raw_pcm;
                i2s_comp_32bit[i]=raw_pcm;
        }

#endif
#endif
#if (I2S_CODEC_ALC5621_OR_ALC5651)
        memset( i2s_tx[pI2sVerifyPara->I2sIndex].payload_32bit, 0, ((pI2sVerifyPara->I2sPageSize+1)*(pI2sVerifyPara->I2sPageNum+1)+16)*4);
  #if defined(I2S_PLAY_TONE_TEST)
    #ifdef I2S_TONEGEN_TABLE
        for (i=0 ; i< ((pI2sVerifyPara->I2sPageSize+1)*(pI2sVerifyPara->I2sPageNum+1)) ; i++) {
            if (sample_count==play_channel_now) {
                i2s_tx[pI2sVerifyPara->I2sIndex].payload_32bit[i] = i2s_vpat32bit[tone_phase];
                tone_phase++;
                if (tone_phase == 16)
                    tone_phase = 0;
            } else
                i2s_tx[pI2sVerifyPara->I2sIndex].payload_32bit[i] = 0;
            sample_count++;
            if (sample_count == i2s_allchannel)
                sample_count = 0;
        }
    #else
        I2SToneGens32bit((pI2sVerifyPara->I2sPageSize+1)*(pI2sVerifyPara->I2sPageNum+1), i2s_tx[pI2sVerifyPara->I2sIndex].payload_32bit);
    #endif
  #endif

#endif
#if 1//FLYSKY_DEBUG
#if (I2S_DBG_LVL>=3)
        u32 print_size;
        print_size = (pI2sVerifyPara->I2sPageSize+1>270)? 270: pI2sVerifyPara->I2sPageSize+1;
        printf("i2s tx page data:\n");
        for (i=0 ; i<(print_size*(pI2sVerifyPara->I2sPageNum+1))+16; i++) {
            if ((i%8) == 7)
                printf(" %x\n", i2s_tx[pI2sVerifyPara->I2sIndex].payload_32bit[i]);
            else
                printf(" %x ", i2s_tx[pI2sVerifyPara->I2sIndex].payload_32bit[i]);
        }
#endif
#endif

        memset( i2s_rx[pI2sVerifyPara->I2sIndex].payload_32bit, 0, ((pI2sVerifyPara->I2sPageSize+1)*(pI2sVerifyPara->I2sPageNum+1)+16)*4);

        pI2sVerifyPara->pTxPage_32bit = i2s_tx[pI2sVerifyPara->I2sIndex].payload_32bit;
        pI2sVerifyPara->pRxPage_32bit = i2s_rx[pI2sVerifyPara->I2sIndex].payload_32bit;
        #if (SOC_TYPE_8196F)
        HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_TX_PAGE_PTR, (VIR_TO_PHY((u32)pI2sVerifyPara->pTxPage_32bit)));
        HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_RX_PAGE_PTR, (VIR_TO_PHY((u32)pI2sVerifyPara->pRxPage_32bit)));
        #else
        HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_TX_PAGE_PTR, (u32)pI2sVerifyPara->pTxPage_32bit);
        HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_RX_PAGE_PTR, (u32)pI2sVerifyPara->pRxPage_32bit);
        #endif
    flush_dcache_range(((unsigned long)i2s_tx[pI2sVerifyPara->I2sIndex].payload_32bit)&(~(ARCH_DMA_MINALIGN-1)), (((unsigned long)i2s_tx[pI2sVerifyPara->I2sIndex].payload_32bit)+(MAX_I2S_PAGE_NUM*I2S_PAGE_SIZE*2+32)*2)&(~(ARCH_DMA_MINALIGN-1)));
    flush_dcache_range(((unsigned long)i2s_rx[pI2sVerifyPara->I2sIndex].payload_32bit)&(~(ARCH_DMA_MINALIGN-1)), (((unsigned long)i2s_rx[pI2sVerifyPara->I2sIndex].payload_32bit)+(MAX_I2S_PAGE_NUM*I2S_PAGE_SIZE*2+32)*2)&(~(ARCH_DMA_MINALIGN-1)));
    invalidate_dcache_range(((unsigned long)i2s_rx[pI2sVerifyPara->I2sIndex].payload_32bit)&(~(ARCH_DMA_MINALIGN-1)), (((unsigned long)i2s_rx[pI2sVerifyPara->I2sIndex].payload_32bit)+(MAX_I2S_PAGE_NUM*I2S_PAGE_SIZE*2+32)*2)&(~(ARCH_DMA_MINALIGN-1)));
    } else {
#if DMA_SELECT_SDRAM
	#if 1
       i2s_tx[pI2sVerifyPara->I2sIndex].payload = i2s_tx_buf;
       i2s_rx[pI2sVerifyPara->I2sIndex].payload = i2s_rx_buf;
	#else
	i2s_tx[pI2sVerifyPara->I2sIndex].payload = (s16 *)(0x09f8a080+(0<<10));
	i2s_rx[pI2sVerifyPara->I2sIndex].payload = (s16 *)(0x09f9a0c0+(16<<10));
	#endif
#elif (DMA_SELECT_ROM && SOC_TYPE_8196F)
	i2s_tx[pI2sVerifyPara->I2sIndex].payload = (s32 *)(0xbfd00000+(0<<10)); //4kword per page, 16kword/4page. 64kbyte
	i2s_rx[pI2sVerifyPara->I2sIndex].payload = (s32 *)(0xbfd00000+(32<<10));

#else
	#if defined(SOC_TYPE_8198E)
        i2s_tx[pI2sVerifyPara->I2sIndex].payload = (s32 *)(0xd0000000+(0<<10)); //4kword per page, 16kword/4page. 64kbyte
        i2s_rx[pI2sVerifyPara->I2sIndex].payload = (s32 *)(0xd0000000+(64<<10));
	#elif defined(SOC_TYPE_8196F)
        i2s_tx[pI2sVerifyPara->I2sIndex].payload = (s32 *)(0xbfe00000+(0<<10)); //4kword per page, 16kword/4page. 64kbyte
        i2s_rx[pI2sVerifyPara->I2sIndex].payload = (s32 *)(0xbfe00000+(8<<10));	
	#endif
#endif
#if I2S_8277C_TEST_MODE
        for (i=0 ; i< ((pI2sVerifyPara->I2sPageSize+1)*(pI2sVerifyPara->I2sPageNum+1)*2) ; i++) {
            i2s_tx[pI2sVerifyPara->I2sIndex].payload[i] = i2s_8277c_inc_value;
            i2s_8277c_inc_value++;
        }

#else
#if !I2S_CODEC_ALC5621_OR_ALC5651
        for (i=0 ; i< ((pI2sVerifyPara->I2sPageSize+1)*(pI2sVerifyPara->I2sPageNum+1)*2+32) ; i++) {
            s16 raw_pcm;
            raw_pcm = (i+101)*30-67;
            if (i%2 == 1)
               raw_pcm ^= 0x8000;
            i2s_tx[pI2sVerifyPara->I2sIndex].payload[i] = raw_pcm;
            i2s_comp[i] = raw_pcm;
        }
#endif
#endif
#if (I2S_CODEC_ALC5621_OR_ALC5651)
        memset( i2s_tx[pI2sVerifyPara->I2sIndex].payload, 0, ((pI2sVerifyPara->I2sPageSize+1)*(pI2sVerifyPara->I2sPageNum+1)+16)*4);
  #if defined(I2S_PLAY_TONE_TEST)
    #if I2S_TONEGEN_TABLE
        for (i=0 ; i< ((pI2sVerifyPara->I2sPageSize+1)*(pI2sVerifyPara->I2sPageNum+1)*2) ; i++) {
            if (sample_count==play_channel_now) {
                i2s_tx[pI2sVerifyPara->I2sIndex].payload[i] = i2s_vpat[tone_phase];
                tone_phase++;
                if (tone_phase == 16)
                    tone_phase = 0;
            } else
                i2s_tx[pI2sVerifyPara->I2sIndex].payload[i] = 0;
            sample_count++;
            if (sample_count == i2s_allchannel)
                sample_count = 0;
        }
    #else
        I2SToneGens((pI2sVerifyPara->I2sPageSize+1)*(pI2sVerifyPara->I2sPageNum+1)*2, i2s_tx[pI2sVerifyPara->I2sIndex].payload);
    #endif
  #endif
#endif
#if 1//FLYSKY_DEBUG
#if (I2S_DBG_LVL>=3)
        u32 print_size;
        print_size = (pI2sVerifyPara->I2sPageSize+1>270)? 270: pI2sVerifyPara->I2sPageSize+1;
        printf("i2s tx page data:\n");
        for (i=0 ; i<(print_size*(pI2sVerifyPara->I2sPageNum+1))*2+32; i++) {
     	    if (i%((pI2sVerifyPara->I2sPageSize+1)*2) == 0)
                printf("============================\n");
            if ((i%8) == 7)
                printf(" %x\n", i2s_tx[pI2sVerifyPara->I2sIndex].payload[i]);
            else
                printf(" %x ", i2s_tx[pI2sVerifyPara->I2sIndex].payload[i]);


        }
#endif
#endif
        memset( i2s_rx[pI2sVerifyPara->I2sIndex].payload, 0, ((pI2sVerifyPara->I2sPageSize+1)*(pI2sVerifyPara->I2sPageNum+1)+16)*4);

        pI2sVerifyPara->pTxPage = i2s_tx[pI2sVerifyPara->I2sIndex].payload;
        pI2sVerifyPara->pRxPage = i2s_rx[pI2sVerifyPara->I2sIndex].payload;
        #if (SOC_TYPE_8196F)
        HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_TX_PAGE_PTR, (VIR_TO_PHY((u32)pI2sVerifyPara->pTxPage)));
        HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_RX_PAGE_PTR, (VIR_TO_PHY((u32)pI2sVerifyPara->pRxPage)));
        #else
        HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_TX_PAGE_PTR, (u32)pI2sVerifyPara->pTxPage);
        HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_RX_PAGE_PTR, (u32)pI2sVerifyPara->pRxPage);
        #endif
    flush_dcache_range(((unsigned long)i2s_tx[pI2sVerifyPara->I2sIndex].payload)&(~(ARCH_DMA_MINALIGN-1)), (((unsigned long)i2s_tx[pI2sVerifyPara->I2sIndex].payload)+(MAX_I2S_PAGE_NUM*I2S_PAGE_SIZE*2+32)*2)&(~(ARCH_DMA_MINALIGN-1)));
    flush_dcache_range(((unsigned long)i2s_rx[pI2sVerifyPara->I2sIndex].payload)&(~(ARCH_DMA_MINALIGN-1)), (((unsigned long)i2s_rx[pI2sVerifyPara->I2sIndex].payload)+(MAX_I2S_PAGE_NUM*I2S_PAGE_SIZE*2+32)*2)&(~(ARCH_DMA_MINALIGN-1)));
    invalidate_dcache_range(((unsigned long)i2s_rx[pI2sVerifyPara->I2sIndex].payload)&(~(ARCH_DMA_MINALIGN-1)), (((unsigned long)i2s_rx[pI2sVerifyPara->I2sIndex].payload)+(MAX_I2S_PAGE_NUM*I2S_PAGE_SIZE*2+32)*2)&(~(ARCH_DMA_MINALIGN-1)));
    }


    HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_TX_PAGE0_OWN, 0x80000000);
    HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_TX_PAGE1_OWN, 0x80000000);
    HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_TX_PAGE2_OWN, 0x80000000);
    HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_TX_PAGE3_OWN, 0x80000000);

#if defined(I2S_PLAY_TONE_TEST)
    HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_RX_PAGE0_OWN, 0);
    HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_RX_PAGE1_OWN, 0);
    HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_RX_PAGE2_OWN, 0);
    HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_RX_PAGE3_OWN, 0);
#else
    HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_RX_PAGE0_OWN, 0x80000000);
    HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_RX_PAGE1_OWN, 0x80000000);
    HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_RX_PAGE2_OWN, 0x80000000);
    HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_RX_PAGE3_OWN, 0x80000000);
#endif

	// tx/rx int 
	HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_TX_STATUS_INT, 0x1ff);
	HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_TX_MASK_INT, 0x01ff);
	HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_RX_STATUS_INT, 0x1ff);
	HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_RX_MASK_INT, 0x01ff);

#ifdef I2S_MIC_LOOPBACK_SPEAKER_TEST

        memset( databuf, 0, DATABUFSIZE);
        rx_write_index = 0;
        tx_read_index = i2s_page_size*4;
#endif

        i2s_txpage[pI2sVerifyPara->I2sIndex]=0;
        i2s_rxpage[pI2sVerifyPara->I2sIndex]=0;
        i2s_isr_cnt = 0;
        i2s_err_cnt = 0;

#if I2S_8277C_TEST_MODE
    t_num=0;
	i2s_rxpage_cnt=0;
	t_old_value=0;
	t_value=0;

#endif

}

s32 i2s_rate_tab[]={8000, 12000, 16000, 24000, 32000, 48000, 64000, 96000, 
                    192000, 384000, 0, 0, 0, 0, 0, 0,
                    7350, 11025, 14700, 22050, 29400, 44100, 58800, 88200,
                    175400, 0,      0, 0, 0, 0, 0, 0 };

VOID
I2sTestMode0(
    IN  VOID      *Data
)
{
    PI2S_VERIFY_PARAMETER pI2sVerifyPara = (PI2S_VERIFY_PARAMETER)Data;

    u32 control_setting;

#if I2S_INTERNAL_LOOPBACK
    control_setting = BIT_(7) | BIT_CTRL_CTLX_I2S_ENDIAN_SWAP(I2S_ENDIAN_SWAP_ENABLE);//internal loopback
    control_setting = control_setting | BIT_CTRL_CTLX_I2S_EDGE_SWAP(I2S_EDGE_SWAP_ENABLE);
#else
    control_setting = BIT_CTRL_CTLX_I2S_ENDIAN_SWAP(I2S_ENDIAN_SWAP_ENABLE);//external loopback
    control_setting = control_setting | BIT_CTRL_CTLX_I2S_EDGE_SWAP(I2S_EDGE_SWAP_ENABLE);
#endif
    if (pI2sVerifyPara->I2sWordLength == 1)
        control_setting |= BIT_(29);//control_setting |= BIT_(6);
    else if (pI2sVerifyPara->I2sWordLength == 2)
        control_setting |= BIT_(30);
#ifdef I2S_SLAVE_MODE
    control_setting |= BIT_(28);  //clock slave mode
#endif
#if 1 //TODO : burst length , default bl = 4
    control_setting |= BIT_CTRL_CTLX_I2S_BURST_LENGTH(I2S_BURST_SIZE);

#endif
#ifdef I2S_CLK_22P579MHZ
    //control_setting |= BIT_(30);  //44.1khz clock input
#endif
    //if (pI2sVerifyPara->I2sSampleRate == 7) // notes need change for 8198e i2s
    //    control_setting |= BIT_(30);  //44.1khz clock input

    //reset i2s
    HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_CTL, BIT_(31)|BIT_(26)|BIT_(25)|control_setting);
    HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_CTL, BIT_(31)|0|control_setting);
    HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_CTL, BIT_(31)|BIT_(26)|BIT_(25)|control_setting);

    HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_TX_STATUS_INT, 0x1ff);//clear pending interrupt
    HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_RX_STATUS_INT, 0x1ff);

    i2s_rate = i2s_rate_tab[pI2sVerifyPara->I2sSampleRate];
#if 1//FLYSKY_DEBUG
    printf("i2s_rate=%d\n", i2s_rate);
#endif
#if 0
    i2s_page_size = i2s_rate/1000*I2S_PAGE_TIME*(pI2sVerifyPara->I2sCHNum+1)*(1<<pI2sVerifyPara->I2sWordLength)/20;
    DiagPrintf("pI2sVerifyPara->I2sCHNum=%d, pI2sVerifyPara->I2sWordLengt=%d, i2s_page_size=%d\n", pI2sVerifyPara->I2sCHNum, pI2sVerifyPara->I2sWordLength, i2s_page_size);
    if (i2s_page_size>4096)
        i2s_page_size = 4096;
    pI2sVerifyPara->I2sPageSize = i2s_page_size-1;
#else
    i2s_page_size = pI2sVerifyPara->I2sPageSize+1;
    i2s_page_time = (20000*i2s_page_size)/i2s_rate/(i2s_ch_channelnum_tab1[pI2sVerifyPara->I2sCHNum])/(1<<(pI2sVerifyPara->I2sWordLength>0));
#endif
    if (pI2sVerifyPara->I2sSampleRate >= 16)
        HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_SETTING, BIT_(18)|BIT_CTRL_SETTING_I2S_SAMPLE_RATE(15&pI2sVerifyPara->I2sSampleRate) |
                                                                BIT_CTRL_SETTING_I2S_PAGE_NUM(pI2sVerifyPara->I2sPageNum) |
                                                                BIT_CTRL_SETTING_I2S_PAGE_SZ(pI2sVerifyPara->I2sPageSize));
    else
        HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_SETTING, BIT_CTRL_SETTING_I2S_SAMPLE_RATE(pI2sVerifyPara->I2sSampleRate) |
                                                                BIT_CTRL_SETTING_I2S_PAGE_NUM(pI2sVerifyPara->I2sPageNum) |
                                                                BIT_CTRL_SETTING_I2S_PAGE_SZ(pI2sVerifyPara->I2sPageSize));

    if (pI2sVerifyPara->I2sCHNum == 0) {
        control_setting |= BIT_CTRL_CTLX_I2S_CH_NUM(2);
#ifdef I2S_PLAY_TONE_TEST
        i2s_allchannel = 1;
#endif
    } else if (pI2sVerifyPara->I2sCHNum == 2) {
        control_setting |= BIT_CTRL_CTLX_I2S_CH_NUM(1);
#ifdef I2S_PLAY_TONE_TEST
        i2s_allchannel = 6;
#endif
    } else {
#ifdef I2S_PLAY_TONE_TEST
        i2s_allchannel = 2;
#endif
    }
#ifdef I2S_PLAY_TONE_TEST
    control_setting |= BIT_CTRL_CTLX_I2S_TRX_ACT(1);
#else
    if (pI2sVerifyPara->I2sCHNum == 2) {
        control_setting |= BIT_CTRL_CTLX_I2S_TRX_ACT(1);
    } else {
        control_setting |= BIT_CTRL_CTLX_I2S_TRX_ACT(2);
    }
#endif
#if 1//FLYSKY_DEBUG
#if (I2S_DBG_LVL >= 1)
    DBG_8195A_DRIVER("==============>> i2s start test << ===================\n");
    printf("[test count:%d]\n", i2s_cnt++);
  #if !I2S_CODEC_ALC5621_OR_ALC5651
    DBG_8195A_DRIVER("test mode:loopback test\n");
  #else
    #ifdef I2S_PLAY_TONE_TEST
    printf("test mode: play tone, play_channel=%d\n", play_channel);
    sample_count = 0;
    tone_phase = 0;
      #ifdef I2S_TONEGEN_TABLE
    play_channel_now = play_channel++;
    if (play_channel == i2s_allchannel)
        play_channel = 0;
      #endif
    #else
    DBG_8195A_DRIVER("test mode: mic loopback speaker\n");
    #endif
  #endif
    printf("sampling rate=%dHz, internal loopback=%s, \n", i2s_rate, (control_setting & BIT_(7))?"ON":"OFF");
    printf("allchannel=%d, word length=%d-%s, RAM-type:%s\n", i2s_ch_channelnum_tab1[pI2sVerifyPara->I2sCHNum], pI2sVerifyPara->I2sWordLength, pI2sVerifyPara->I2sWordLength? "24/32bit" : "16bit", DMA_SELECT_SDRAM?"SDRAM":"SRAM");
    printf("slave mode=%s, sck inv=%s, enidan swap=%s\n", (control_setting & BIT_(28))?"ON":"OFF", (control_setting & BIT_(11))?"ON":"OFF", (control_setting & BIT_(12))?"ON":"OFF");
    printf("clock inpute=%sMhz\n", (pI2sVerifyPara->I2sSampleRate >= 16)?"45.1584":"98.304");
    //DBG_8195A_DRIVER("PAGE_TIME=%d, page_size=%d, max_page_size=%d, page_num=%d\n", I2S_PAGE_TIME, pI2sVerifyPara->I2sPageSize+1, I2S_PAGE_SIZE, pI2sVerifyPara->I2sPageNum+1);
    printf("page_time=%d, page_size=%d, max_page_size=%d, page_num=%d\n", i2s_page_time, pI2sVerifyPara->I2sPageSize+1, I2S_PAGE_SIZE, pI2sVerifyPara->I2sPageNum+1);
#endif
#endif
    InitI2sTestCore(pI2sVerifyPara);
    I2SRegisterDump(&TestI2sAdapte);
#if 1//FLYSKY_DEBUG
#if (I2S_DBG_LVL >= 1)
    printf("i2scr = %x\n", BIT_(31)|BIT_(26)|BIT_(25)|BIT_(24)|BIT_(23)| control_setting | BIT_(0));
#endif
#endif
    HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_CTL, BIT_(31)|BIT_(26)|BIT_(25)|BIT_(24)|BIT_(23) | control_setting | BIT_(0));
}

//test 24bit or 16bit
#define I2S_WL24BIT 1

#ifdef I2S_WL24BIT
#define I2S_WL_SEL BIT6
#else
#define I2S_WL_SEL 0
#endif

//#define I2S_LOOPBACK_SEL BIT7 //enable internal loopback
#define I2S_LOOPBACK_SEL 0

#define I2S_TRACT_SEL BIT2 //tx+rx
//#define I2S_TRACT_SEL 0 //rx only


//#define SLAVE_SEL (BIT_(29)|BIT_(11)) // 0 or bit29
#define SLAVE_SEL (BIT_(6)|BIT_(2)) // 0 or bit29
//#define SLAVE_SEL (BIT_(2)|BIT_(11)|BIT_(2)) // 0 or bit29
//#define SLAVE_SEL (0) // 0 or bit29

#define TEST_SEL (I2S_WL_SEL|I2S_LOOPBACK_SEL|I2S_TRACT_SEL)

VOID
I2sTestMode3(
    IN  VOID      *Data
)
{
	PI2S_VERIFY_PARAMETER pI2sVerifyPara = (PI2S_VERIFY_PARAMETER)Data;
	s32* pI2sRxPtr=(int *)0xa001f000;
	s32* pI2sTxPtr=(int *)0xa001e000;
	s16* pI2sTxPtr16=(s16 *)0xa001e000;
	s16* pI2sRxPtr16=(s16 *)0xa001f000;
	u32 i;
#if FLYSKY_DEBUG
	DBG_8195A_DRIVER("test i2s rx left/right channel, please short rx and ws\n");
#endif
	pI2sVerifyPara->I2sIndex = 1;
	// reset i2s + stereo 24bit rx
	HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, 0, BIT_(31)|TEST_SEL);
	HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, 0, 0|TEST_SEL);
        // stereo 24bit tx, endian swap
	//HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, 0, BIT_(31)|0x1042);
        // stereo 24bit tx, sck rising edge, endian swap
	//HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, 0, BIT_(31)|0x1842);
        // stereo 16bit tx, endian swap
	HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, 0, BIT_(31)|TEST_SEL|0x1000);
        // stereo 16bit tx, sck rising edge, endian swap
	//HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, 0, BIT_(31)|0x1802);

	// tx/rx page addr/size - 16byte * 2 page * 8khz
	HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, 4, 0xa001e000);
	HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, 8, 0xa001f000);
	HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, 0xc, 0x17007);//48khz
	//HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, 0xc, 0x304f);//8khz

	// tx/rx int 
	HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_TX_STATUS_INT, 0x3f);
	HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_TX_MASK_INT, 0x00);
	HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_RX_STATUS_INT, 0x3f);
	HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_RX_MASK_INT, 0x00);

	// own bit
//#if (I2S_TRACT_SEL!=0)
#if 1
	HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_TX_PAGE0_OWN+0*REG_I2S_PAGE_OWN_OFF, BIT_(31));
	HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_TX_PAGE0_OWN+1*REG_I2S_PAGE_OWN_OFF, BIT_(31));
	HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_TX_PAGE0_OWN+2*REG_I2S_PAGE_OWN_OFF, BIT_(31));
	HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_TX_PAGE0_OWN+3*REG_I2S_PAGE_OWN_OFF, BIT_(31));
#else
	HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_TX_PAGE0_OWN+0*REG_I2S_PAGE_OWN_OFF, 0);
	HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_TX_PAGE0_OWN+1*REG_I2S_PAGE_OWN_OFF, 0);
	HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_TX_PAGE0_OWN+2*REG_I2S_PAGE_OWN_OFF, 0);
	HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_TX_PAGE0_OWN+3*REG_I2S_PAGE_OWN_OFF, 0);

#endif
	HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_RX_PAGE0_OWN+0*REG_I2S_PAGE_OWN_OFF, BIT_(31));
	HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_RX_PAGE0_OWN+1*REG_I2S_PAGE_OWN_OFF, BIT_(31));
	HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_RX_PAGE0_OWN+2*REG_I2S_PAGE_OWN_OFF, BIT_(31));
	HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_RX_PAGE0_OWN+3*REG_I2S_PAGE_OWN_OFF, BIT_(31));

	//prepaer tx buffer
//	for(i=0 ; i<8 ; i++) {
//		pI2sTxPtr[2*i]=0;
//		pI2sTxPtr[2*i+1]=0xffffff00;
//	}
#ifndef I2S_WL24BIT
  #if 0
		pI2sTxPtr16[0]=0x0000ffff;
		pI2sTxPtr16[1]=0x0000aa55;
		pI2sTxPtr16[2]=0x0000ffff;
		pI2sTxPtr16[3]=0x0000aa55;
		pI2sTxPtr16[4]=0x0000ffff;
		pI2sTxPtr16[5]=0x0000aa55;
		pI2sTxPtr16[6]=0x0000ffff;
		pI2sTxPtr16[7]=0x0000aa55;
		pI2sTxPtr16[8]=0x0000ffff;
		pI2sTxPtr16[9]=0x0000aa55;
		pI2sTxPtr16[10]=0x0000ffff;
		pI2sTxPtr16[11]=0x0000aa55;
		pI2sTxPtr16[12]=0x0000ffff;
		pI2sTxPtr16[13]=0x0000aa55;
	//	pI2sTxPtr16[14]=0x0000ffff;
	//	pI2sTxPtr16[15]=0x0000aa55;
		pI2sTxPtr16[14]=0x00000010;
		pI2sTxPtr16[15]=0x00000001;
  #else
	for(i=0 ; i<33 ; i++) {
		pI2sTxPtr16[2*i]=0x0001;
		pI2sTxPtr16[2*i+1]=0xfffe;
	}

  #endif
#else
   #if 0
		pI2sTxPtr[0]=0x0000000f;
		pI2sTxPtr[1]=0x000000ff;
		pI2sTxPtr[2]=0x00000fff;
		pI2sTxPtr[3]=0x0000ffff;
		pI2sTxPtr[4]=0x000fffff;
		pI2sTxPtr[5]=0x00ffffff;
		pI2sTxPtr[6]=0x00ffffff;
		pI2sTxPtr[7]=0x00ffffff;
		pI2sTxPtr[8]=0x00ffffff;
   #else
		pI2sTxPtr[0]=0x00000000;
		pI2sTxPtr[1]=0x00ffffff;
		pI2sTxPtr[2]=0x00000000;
		pI2sTxPtr[3]=0x00ffffff;
		pI2sTxPtr[4]=0x00000000;
		pI2sTxPtr[5]=0x00ffffff;
		pI2sTxPtr[6]=0x00000000;
		pI2sTxPtr[7]=0x00ffffff;
		pI2sTxPtr[8]=0x00000000;
		pI2sTxPtr[9]=0x00ffffff;
		pI2sTxPtr[10]=0x00000000;
		pI2sTxPtr[11]=0x00ffffff;
		pI2sTxPtr[12]=0x00000000;
		pI2sTxPtr[13]=0x00ffffff;
		pI2sTxPtr[14]=0x00000000;
		pI2sTxPtr[15]=0x00ffffff;
		pI2sTxPtr[16]=0x00000000;
		pI2sTxPtr[17]=0x00ffffff;
		pI2sTxPtr[18]=0x00000000;
		pI2sTxPtr[19]=0x00ffffff;
		pI2sTxPtr[20]=0x00000000;
		pI2sTxPtr[21]=0x00ffffff;
		pI2sTxPtr[22]=0x00000000;
		pI2sTxPtr[23]=0x00ffffff;
		pI2sTxPtr[24]=0x00000000;
		pI2sTxPtr[25]=0x00ffffff;
		pI2sTxPtr[26]=0x00000000;
		pI2sTxPtr[27]=0x00ffffff;
		pI2sTxPtr[28]=0x00000000;
		pI2sTxPtr[29]=0x00ffffff;
		pI2sTxPtr[30]=0x00000000;
		pI2sTxPtr[31]=0x00ffffff;
		pI2sTxPtr[32]=0x00000000;
		pI2sTxPtr[33]=0x00ffffff;

   #endif
#endif
	//clean rx buffer
	for(i=0 ; i<34 ; i++) {
		pI2sRxPtr[i]=0xa000a;
	}

	//enable
	HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, 0, BIT_(31)|TEST_SEL|0x1001);

	//add delay;
	for(i=0 ; i<32768 ; i++) {
		HAL_I2S_READ32(pI2sVerifyPara->I2sIndex, 0);
	}
	
	//stop
	HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, 0, BIT_(31)|TEST_SEL|0x1000);


	//check offset
#if FLYSKY_DEBUG
#ifdef I2S_WL24BIT
	DBG_8195A_DRIVER("sample 0-3: %x %x %x %x \n", pI2sRxPtr[0], pI2sRxPtr[1], pI2sRxPtr[2], pI2sRxPtr[3]);
	DBG_8195A_DRIVER("sample 4-7: %x %x %x %x \n", pI2sRxPtr[4], pI2sRxPtr[5], pI2sRxPtr[6], pI2sRxPtr[7]);
	DBG_8195A_DRIVER("sample 8-11: %x %x %x %x \n", pI2sRxPtr[8], pI2sRxPtr[9], pI2sRxPtr[10], pI2sRxPtr[11]);
	DBG_8195A_DRIVER("sample 12-15: %x %x %x %x \n", pI2sRxPtr[12], pI2sRxPtr[13], pI2sRxPtr[14], pI2sRxPtr[15]);
	DBG_8195A_DRIVER("sample 16-19: %x %x %x %x \n", pI2sRxPtr[16], pI2sRxPtr[17], pI2sRxPtr[18], pI2sRxPtr[19]);
	DBG_8195A_DRIVER("sample 28-31: %x %x %x %x \n", pI2sRxPtr[28], pI2sRxPtr[29], pI2sRxPtr[30], pI2sRxPtr[31]);
	DBG_8195A_DRIVER("sample 32-35: %x %x %x %x \n", pI2sRxPtr[32], pI2sRxPtr[33], pI2sRxPtr[34], pI2sRxPtr[35]);
	if (pI2sRxPtr[0]==0x0) {
		DBG_8195A_DRIVER("first sample is Left channel(0)\n\n\n");
	} else if (pI2sRxPtr[0]==0xffffff) {
		DBG_8195A_DRIVER("first sample is Right channel(1)\n\n\n");
	} else {
		DBG_8195A_DRIVER("first sample is UNknown channel(-1)\n\n\n");
	}
#else
	DBG_8195A_DRIVER("sample  0- 3: %x %x %x %x \n", pI2sRxPtr16[ 0], pI2sRxPtr16[ 1], pI2sRxPtr16[ 2], pI2sRxPtr16[ 3]);
	DBG_8195A_DRIVER("sample 56-59: %x %x %x %x \n", pI2sRxPtr16[56], pI2sRxPtr16[57], pI2sRxPtr16[58], pI2sRxPtr16[59]);
	DBG_8195A_DRIVER("sample 60-63: %x %x %x %x \n", pI2sRxPtr16[60], pI2sRxPtr16[61], pI2sRxPtr16[62], pI2sRxPtr16[63]);
	DBG_8195A_DRIVER("sample 64-67: %x %x %x %x \n", pI2sRxPtr16[64], pI2sRxPtr16[65], pI2sRxPtr16[66], pI2sRxPtr16[67]);

#endif
#endif
}

#ifdef I2S_FREE_RTOS_TASK_LOOPBACK_TEST

VOID
I2STaskIrqHandle
(
    IN  VOID        *Data
)
{
    PI2S_ADAPTER pI2sAdapter = (PI2S_ADAPTER) Data;
    PHAL_I2S_ADAPTER pHalI2sAdapter = pI2sAdapter->pHalI2sAdapter;
    u8 I2sIndex = pHalI2sAdapter->I2sIndex;
    u32 I2sPageNum;
    u32 I2sPageSize;
    u32 i, j;

    u32 I2sTxIsr, I2sRxIsr;

    I2sTxIsr = HAL_I2S_READ32(I2sIndex, REG_I2S_TX_STATUS_INT);
    I2sRxIsr = HAL_I2S_READ32(I2sIndex, REG_I2S_RX_STATUS_INT);

//DBG_8195A("c%x\n", I2sTxIsr*16+I2sRxIsr);
    HAL_I2S_WRITE32(I2sIndex, REG_I2S_TX_STATUS_INT, I2sTxIsr);
    HAL_I2S_WRITE32(I2sIndex, REG_I2S_RX_STATUS_INT, I2sRxIsr);

    I2sPageNum = ((HAL_I2S_READ32(I2sIndex, REG_I2S_SETTING)>>12)&0x3) +1;
    I2sPageSize = (HAL_I2S_READ32(I2sIndex, REG_I2S_SETTING)&0xfff) +1;

    for (i=0 ; i<I2sPageNum ; i++) { // page0, page1, page2, page3
        if ( I2sTxIsr & I2SChanTxPage[i2s_txpage[I2sIndex]] ){
            for (j=0 ; j<I2sPageSize*2 ; j++) {
	    	i2s_tx[I2sIndex].payload[j+i2s_txpage[I2sIndex]*480*2] = 0xaaaa+(0x300*(i2s_isr_cnt+4))+j;
	    	
	    }
            i2s_isr_cnt++;
            HAL_I2S_WRITE32(I2sIndex, REG_I2S_TX_PAGE0_OWN+i2s_txpage[I2sIndex]*REG_I2S_PAGE_OWN_OFF, BIT31);
            I2sTxIsr &= ~I2SChanTxPage[i2s_txpage[I2sIndex]];
            i2s_txpage[I2sIndex] += 1;
            if (i2s_txpage[I2sIndex] == I2sPageNum )
                i2s_txpage[I2sIndex] = 0;
            if ((i2s_isr_cnt % (10000/i2s_page_time)) == (10000/i2s_page_time-1)) {
                //DiagPrintf(",");
            }
            if ((i2s_isr_cnt % (600000/i2s_page_time)) == (600000/i2s_page_time-1)) {
                //DiagPrintf("\n");
            }
        } // end of tx

        if ( I2sRxIsr & I2SChanRxPage[i2s_rxpage[I2sIndex]] ){

            for (j=0 ; j<I2sPageSize*2 ; j++) {
		if ((I2SVeriGCnt == 0) && (j<6) ) {
			if ((i2s_rx[I2sIndex].payload[j+480*2*i2s_rxpage[I2sIndex]] == (short)0xaaaa)) {
				I2SVeriShift = j;
			}
			if ((i2s_rx[I2sIndex].payload[j+480*2*i2s_rxpage[I2sIndex]] == 0x0) ||
                            (i2s_rx[I2sIndex].payload[j+480*2*i2s_rxpage[I2sIndex]] == (short)(0xaaaa+j-I2SVeriShift))) {
				;//pass
			} else {
				DiagPrintf("(%x,%d,%d,%d)", i2s_rx[I2sIndex].payload[j+480*2*i2s_rxpage[I2sIndex]], I2SVeriGCnt, i2s_rxpage[I2sIndex], j);
				I2SVeriErrCnt++;
				break;
			}

		} else {
			if (j<I2SVeriShift) {
				if ((i2s_rx[I2sIndex].payload[j+480*2*i2s_rxpage[I2sIndex]] == (short)(0xaaaa+(0x300*(I2SVeriGCnt-1)+480*2-I2SVeriShift)+j))) {
					;//pass
				} else {
					DiagPrintf("(%x,%d,%d,%d)", i2s_rx[I2sIndex].payload[j+480*2*i2s_rxpage[I2sIndex]], I2SVeriGCnt, i2s_rxpage[I2sIndex], j);
					I2SVeriErrCnt++;
					break;
				}
			
			
			
			} else {
				if ((i2s_rx[I2sIndex].payload[j+480*2*i2s_rxpage[I2sIndex]] == (short)(0xaaaa+(0x300*(I2SVeriGCnt)-I2SVeriShift)+j))) {
					;//pass
				} else {
					DiagPrintf("(%x,%d,%d,%d)", i2s_rx[I2sIndex].payload[j+480*2*i2s_rxpage[I2sIndex]], I2SVeriGCnt, i2s_rxpage[I2sIndex], j);
					I2SVeriErrCnt++;
					break;
				}
			}
		}
            }

            HAL_I2S_WRITE32(I2sIndex, REG_I2S_RX_PAGE0_OWN+i2s_rxpage[I2sIndex]*REG_I2S_PAGE_OWN_OFF, BIT31);
	    I2SVeriGCnt ++;
            I2sRxIsr &= ~I2SChanRxPage[i2s_rxpage[I2sIndex]];
            i2s_rxpage[I2sIndex] += 1;
            if (i2s_rxpage[I2sIndex] == I2sPageNum )
                i2s_rxpage[I2sIndex] = 0;
           //DiagPrintf("*");
        } // end of rx

    }


    //I2S_ISR(pI2sAdapter, I2sTxIsr & 0x0F, I2sRxIsr & 0x0F);
    //if (I2sTxIsr&0x10)
    //    DiagPrintf("TBU");
    //if (I2sRxIsr&0x10)
    //    DiagPrintf("RBU");

}


VOID
I2STestProc(
    IN  VOID    *I2STestData
)
{
	u32 i2sTest;
	u32 i2stemp;
	u32 i2sIndex = 0;
	u32 i2sPinmuxSelect = 0;
	u32 i, j;

	PI2S_ADAPTER	pI2sAdapter;
	IRQ_HANDLE	I2SIrqHandle;
	PI2S_VERIFY_PARAMETER pI2sVerifyPara = &I2sVerifyPere;

	PI2S_VERI_PARA pI2SVeriPara = (PI2S_VERI_PARA) I2STestData;

	pI2sAdapter = &TestI2sAdapte;
	pI2sVerifyPara->I2sIndex = i2sIndex;

	HalI2SOpInit(&HalI2sOp);
	HalI2sOp.HalI2SInit(&HalI2sAdapter);

	pI2sAdapter->pHalI2sAdapter=&HalI2sAdapter;
	pI2sAdapter->pHalI2sAdapter->I2sIndex=pI2sVerifyPara->I2sIndex;


	I2SIrqHandle.Data = (u32) (pI2sAdapter);
#if defined(SOC_TYPE_8195A)
	I2SIrqHandle.IrqNum = I2S0_PCM0_IRQ + pI2sVerifyPara->I2sIndex;
#elif defined(SOC_TYPE_8198E)
	I2SIrqHandle.IrqNum = I2S_IRQ + pI2sVerifyPara->I2sIndex;
#elif defined(SOC_TYPE_8196F)
	I2SIrqHandle.IrqNum = I2S_IRQ;
#else
  #error "no soc type match"
#endif
	I2SIrqHandle.IrqFun = (IRQ_FUN) I2STaskIrqHandle;
	I2SIrqHandle.Priority = 0;
#if 0
	InterruptRegister(&I2SIrqHandle);
	InterruptEn(&I2SIrqHandle);
#endif
	//DiagPrintf("(*40000074=%x, *40000210=%x, *40000218=%x)\n", *((volatile unsigned int*)0x40000074), *((volatile unsigned int*)0x40000210), *((volatile unsigned int*)0x40000218));
	//DiagPrintf("(*40000238=%x, *400002c0=%x, *4000028c=%x)\n", *(unsigned int*)0x40000238, *(unsigned int*)0x400002c0, *(unsigned int*)0x4000028c);
	// enable system pll (24.576Mhz & 22.5792Mhz)
	i2stemp = HAL_READ32(SYSTEM_CTRL_BASE, REG_SOC_SYS_SYSPLL_CTRL1) | (1<<9) | (1<<10);
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SOC_SYS_SYSPLL_CTRL1, i2stemp);
	

	//4 Turn off shutdown mode
	ACTCK_I2S_CCTRL(ON);
	LXBUS_FCTRL(ON); // enable lx bus for i2s 
	if (i2sIndex == 0) {
		PinCtrl(I2S0, i2sPinmuxSelect, ON);
		if (i2sPinmuxSelect == 0) {
			JTAG_PIN_FCTRL(OFF);
			SPI_FLASH_PIN_FCTRL(OFF);
		}
		I2S0_MCK_CTRL(ON);
		I2S0_PIN_CTRL(ON);
		I2S0_FCTRL(ON);
	} else {
		PinCtrl(I2S1, i2sPinmuxSelect, ON);
		I2S1_MCK_CTRL(ON);
		I2S1_PIN_CTRL(ON);
		I2S0_FCTRL(ON); //i2s 1 is control by bit 24 BIT_PERI_I2S0_EN
		I2S1_FCTRL(ON);
	}
	//DiagPrintf("(*40000074=%x, *40000210=%x, *40000218=%x)\n", *(unsigned int*)0x40000074, *(unsigned int*)0x40000210, *(unsigned int*)0x40000218);
	//DiagPrintf("(*40000238=%x, *400002c0=%x, *4000028c=%x)\n", *(unsigned int*)0x40000238, *(unsigned int*)0x400002c0, *(unsigned int*)0x4000028c);

	pI2sVerifyPara->I2sSampleRate = 5; // 48khz
	pI2sVerifyPara->I2sPageNum = 3;
	pI2sVerifyPara->I2sPageSize = 480-1; //10ms=480 samples, L+R channel =32bit
	pI2sVerifyPara->I2sCHNum = 1;
	pI2sVerifyPara->I2sWordLength = 0;
	

	//4 Reset I2s IP
	HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_CTL, BIT_(31)|BIT_CTRL_CTLX_I2S_ENDIAN_SWAP(1));
	HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_CTL, 0|BIT_CTRL_CTLX_I2S_ENDIAN_SWAP(1));
	HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_CTL, BIT_(31)|BIT_CTRL_CTLX_I2S_ENDIAN_SWAP(1));

	HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_TX_STATUS_INT, 0x1ff);//clear pending interrupt
	HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_RX_STATUS_INT, 0x1ff);

	HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_SETTING, BIT_CTRL_SETTING_I2S_SAMPLE_RATE(pI2sVerifyPara->I2sSampleRate) |
	                                                        BIT_CTRL_SETTING_I2S_PAGE_NUM(pI2sVerifyPara->I2sPageNum) |
	                                                        BIT_CTRL_SETTING_I2S_PAGE_SZ(pI2sVerifyPara->I2sPageSize));
	i2s_page_time = (20000*(pI2sVerifyPara->I2sPageSize+1))/48000/(i2s_ch_channelnum_tab1[pI2sVerifyPara->I2sCHNum])/(1<<(pI2sVerifyPara->I2sWordLength>0));
	DiagPrintf("(page_time=%d)", i2s_page_time);
        i2s_tx[pI2sVerifyPara->I2sIndex].payload = i2s_tx_buf;
        i2s_rx[pI2sVerifyPara->I2sIndex].payload = i2s_rx_buf;
    for (j=0 ; j< ((pI2sVerifyPara->I2sPageNum+1)*2) ; j++) {
        for (i=0 ; i< ((pI2sVerifyPara->I2sPageSize+1)*2) ; i++) {
            i2s_tx[pI2sVerifyPara->I2sIndex].payload[i+j*((pI2sVerifyPara->I2sPageSize+1)*2)] = 0xaaaa+0x300*j+i;
            i2s_rx[pI2sVerifyPara->I2sIndex].payload[i+j*((pI2sVerifyPara->I2sPageSize+1)*2)] = 0;
        }
    }
        //HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_TX_PAGE_PTR, (u32)pI2sVerifyPara->pTxPage);
        //HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_RX_PAGE_PTR, (u32)pI2sVerifyPara->pRxPage);
        HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_TX_PAGE_PTR, (u32)i2s_tx[pI2sVerifyPara->I2sIndex].payload);
        HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_RX_PAGE_PTR, (u32)i2s_rx[pI2sVerifyPara->I2sIndex].payload);


    HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_TX_PAGE0_OWN, 0x80000000);
    HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_TX_PAGE1_OWN, 0x80000000);
    HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_TX_PAGE2_OWN, 0x80000000);
    HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_TX_PAGE3_OWN, 0x80000000);


    HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_RX_PAGE0_OWN, 0x80000000);
    HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_RX_PAGE1_OWN, 0x80000000);
    HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_RX_PAGE2_OWN, 0x80000000);
    HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_RX_PAGE3_OWN, 0x80000000);

	HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_TX_STATUS_INT, 0x3f);
	HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_TX_MASK_INT, 0x0f);
	HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_RX_STATUS_INT, 0x3f);
	HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_RX_MASK_INT, 0x0f);

        i2s_txpage[pI2sVerifyPara->I2sIndex]=0;
        i2s_rxpage[pI2sVerifyPara->I2sIndex]=0;

    I2SVeriErrCnt = 0;
    I2SVeriGCnt = 0;
    I2SVeriShift = 0;
    i2s_isr_cnt = 0;

	//InterruptRegister(&I2SIrqHandle);
	//InterruptEn(&I2SIrqHandle);

	// internal loopback
	//HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_CTL, BIT_(31) | BIT_CTRL_CTLX_I2S_ENDIAN_SWAP(1) | BIT_(7)
	//                                                        | BIT_CTRL_CTLX_I2S_TRX_ACT(2) | BIT_CTRL_CTLX_I2S_CH_NUM(0));
	//HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_CTL, BIT_(31) | BIT_CTRL_CTLX_I2S_ENDIAN_SWAP(1) | BIT_(0) | BIT_(7)
	//                                                        | BIT_CTRL_CTLX_I2S_TRX_ACT(2) | BIT_CTRL_CTLX_I2S_CH_NUM(0));
	// external loopback
	HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_CTL, BIT_(31) | BIT_CTRL_CTLX_I2S_ENDIAN_SWAP(1)
	                                                        | BIT_CTRL_CTLX_I2S_TRX_ACT(2) | BIT_CTRL_CTLX_I2S_CH_NUM(0));
	HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_CTL, BIT_(31) | BIT_CTRL_CTLX_I2S_ENDIAN_SWAP(1) | BIT_(0)
	                                                        | BIT_CTRL_CTLX_I2S_TRX_ACT(2) | BIT_CTRL_CTLX_I2S_CH_NUM(0));
	for (;;)
	{
		//4 waiting for command
		RtlDownSema(&(pI2SVeriPara->VeriSema));
		i2sTest = pI2SVeriPara->VeriItem;

		switch (i2sTest)
		{
			case I2S_TEST_N_DEINT:

			default:
				goto ENDI2STASK;
				break;

		}

	}


ENDI2STASK:

	HAL_I2S_WRITE32(pI2sVerifyPara->I2sIndex, REG_I2S_CTL, BIT_(31) | BIT_CTRL_CTLX_I2S_ENDIAN_SWAP(1) |
	                                             BIT_CTRL_CTLX_I2S_TRX_ACT(2) | BIT_CTRL_CTLX_I2S_CH_NUM(0));

	vTaskDelete(NULL);

}

#endif

static u32 i2s_init=0;
VOID
I2STestApp(
    IN  VOID      *Data
)
{

#ifdef I2S_FREE_RTOS_TASK_LOOPBACK_TEST
	PI2S_VERI_PARA pI2SVerParaTmp = (PI2S_VERI_PARA)Data;
	switch (pI2SVerParaTmp->VeriProcCmd)
	{
		//4 I2S_TEST_INIT
		case I2S_TEST_INIT:
			I2SVeriGCnt = 0;
			u32 I2STmpSts;
			pI2SVeriAdp = NULL;
			pI2SVeriAdp = (PI2S_VERI_PARA)RtlZmalloc(sizeof(I2S_VERI_PARA));
			RtlInitSema(&(pI2SVeriAdp->VeriSema), 0);
			pI2SVeriAdp->VeriStop = _FALSE;
			I2STmpSts = xTaskCreate( I2STestProc, (const signed char *)"I2SVERI_TASK",
			                ((1024*4)/sizeof(portBASE_TYPE)), (void *)pI2SVeriAdp, 1, pI2SVeriAdp->I2STask);
			break;

		case I2S_TEST_SWCH:
			break;

		case I2S_TEST_DEINIT:
			//4 force the task goes to end
			pI2SVeriAdp->VeriItem = I2S_TEST_N_DEINT;
 			RtlUpSema(&(pI2SVeriAdp->VeriSema));
                
			//4 free the task semaphore
			RtlFreeSema(&(pI2SVeriAdp->VeriSema));
			pI2SVeriAdp->VeriSema = NULL;

			//4 free the adaptor
			RtlMfree((u8 *)pI2SVeriAdp,sizeof(I2S_VERI_PARA));
			pI2SVeriAdp = NULL;
			break;

		case I2S_TEST_GETCNT:
#if FLYSKY_DEBUG
			DBG_8195A("   [i2s_DBG]:Total Verification Loop:%x, Error%x, Shift=%d\n", I2SVeriGCnt, I2SVeriErrCnt, I2SVeriShift);
#endif
			break;

		default:
			break;
	}

#else
	u32 i;
	PI2S_VERIFY_PARAMETER pTempPara = (PI2S_VERIFY_PARAMETER)Data;
	PI2S_VERIFY_PARAMETER pI2sVerifyPara = &I2sVerifyPere;

	PI2S_ADAPTER         pI2sAdapter;
	IRQ_HANDLE            I2SIrqHandle;

	pI2sAdapter = &TestI2sAdapte;
	pI2sVerifyPara->I2sTestMode = pTempPara->I2sTestMode;
	pI2sVerifyPara->I2sWordLength = pTempPara->I2sWordLength;
	pI2sVerifyPara->I2sMode = pTempPara->I2sMode;
	pI2sVerifyPara->I2sLRSwap = pTempPara->I2sLRSwap;
	pI2sVerifyPara->I2sIndex = pTempPara->I2sIndex;
	pI2sVerifyPara->I2sCHNum = pTempPara->I2sCHNum;
	pI2sVerifyPara->I2sPageSize = pTempPara->I2sPageSize;
               if( pI2sVerifyPara->I2sCHNum == 2)
               {
                        //aligned page size to  6 multiply
                        pI2sVerifyPara->I2sPageSize = (((((pTempPara->I2sPageSize + 1)*4 + 5)/6)*6)/4) - 1;
               }
	pI2sVerifyPara->I2sPageNum = pTempPara->I2sPageNum;
	pI2sVerifyPara->I2sSampleRate = pTempPara->I2sSampleRate;
	//pI2sVerifyPara-> = pTempPara->;
#if FLYSKY_DEBUG
	printf("test i2s mode =%x\n", pI2sVerifyPara->I2sTestMode);
	printf("test i2s WordLength =%x\n", pI2sVerifyPara->I2sWordLength);
	printf("test i2s Mode =%x\n", pI2sVerifyPara->I2sMode);
	printf("test i2s LRSwap =%x\n", pI2sVerifyPara->I2sLRSwap);
	printf("test i2s Index =%x\n", pI2sVerifyPara->I2sIndex);
	printf("test i2s CHNum =%x\n", pI2sVerifyPara->I2sCHNum);
	printf("test i2s PageSize =%x\n", pI2sVerifyPara->I2sPageSize);
	printf("test i2s PageNum =%x\n", pI2sVerifyPara->I2sPageNum);
	printf("test i2s SampleRate =%x\n", pI2sVerifyPara->I2sSampleRate);
#endif
	//init hal 123
	HalI2SOpInit(&HalI2sOp);
	//HalI2sOp.HalI2SInit(&HalI2sAdapter);
	
	pI2sAdapter->pHalI2sAdapter=&HalI2sAdapter;
	pI2sAdapter->pHalI2sAdapter->I2sIndex=pI2sVerifyPara->I2sIndex;
       // init bus, pinmux
#if defined(SOC_TYPE_8196F)
	*(volatile unsigned long *)(0xb8000010) = ((*(volatile unsigned long *)(0xb8000010)) & 0xFFBFFFFF) | (0x1 << 22) | (0x1 << 12) | (0x1 << 13);
	*(volatile unsigned long *)(0xb8000024) = ((*(volatile unsigned long *)(0xb8000024)) & 0xFFDFFFFF) | (0x3 << 20);
	#if 1
	//pinmux
	*(volatile unsigned long *)(0xb8000848) = 0x2223000;
	//*(volatile unsigned long *)(0xb8000820) = 0x43111000; // 5.1 channel
	*(volatile unsigned long *)(0xb8000820) = 0x44111000;
	#else

	//set GPIO B[1], P0_RXC   to I2S_SD1_I //0x808h, [19:16] 0111 (IIS) //0110:IISV, 0111:IIS, 1000:IISA
	*(volatile unsigned long *)(0xB8000808) = ((*(volatile unsigned long *)(0xB8000808)) & 0xFFF0FFFF) | (6<<16);

	//set GPIO B[2], P0_RXD3  to I2S_MCLK  //0x804h, [19:16] 0101
	//set GPIO B[3], P0_RXD2  to I2S_SCLK  //0x804h, [23:20] 0101
	//set GPIO B[4], P0_RXD1  to I2S_WS    //0x804h, [27:24] 0101
	//set GPIO B[5], P0_RXD0  to I2S_SD1_O //0x804h, [31:28] 0100
	*(volatile unsigned long *)(0xB8000804) = ((*(volatile unsigned long *)(0xB8000804)) & 0x0000FFFF) | 5<<16 | 5<<20 | 5<<24 | 4<<28;
	#endif
#elif defined(SOC_TYPE_8198F)  
	*(volatile unsigned long *)(0xF43204B4 ) = ((*(volatile unsigned long *)(0xF43204B4)) & 0x3FFFFFFF) | (0x1 << 30) | (0x1 << 31);
	*(volatile unsigned long *)(0xF43203AC ) = ((*(volatile unsigned long *)(0xF43203AC)) & 0xFFFFE7FF) | (0x1 << 12) | (0x1 << 11);
	*(volatile unsigned long *)(0xF4322000 ) = 0x0;  //set sram non secure

                //set pin mux for 8198FH QA board
                //*(volatile unsigned long *)(0xF4320474) = 0x36;
                *(volatile unsigned long *)(0xF432048c) = 0x32220;
                *(volatile unsigned long *)(0xF4320470) = 0x40000;//0x30300; //0x40000 for sd1_in, 0x30000 fir sd2_o
                //*(volatile unsigned long *)(0xF4320474) = 0x36; 
	// TODO : added platform independent function such as platform_init_i2s , move all platform depended setting to extend folder, 
	//platform_init_i2s();
#elif defined(SOC_TYPE_8277C_ASIC)
	*(volatile unsigned long *)(0xF43204B4 ) = ((*(volatile unsigned long *)(0xF43204B4)) & 0x3FFFFFFF) | (0x1 << 30) | (0x1 << 31);
	*(volatile unsigned long *)(0xF43203AC ) = ((*(volatile unsigned long *)(0xF43203AC)) & 0xFFFFE7FF) | (0x1 << 12) | (0x1 << 11);
	*(volatile unsigned long *)(0xF4322000 ) = 0x0;  //set sram non secure

	//set pin mux for 8277C QA board
	//*(volatile unsigned long *)(0xF432048c) = 0x32220;
	//*(volatile unsigned long *)(0xF4320470) = 0x40000;//0x30300; //0x40000 for sd1_in, 0x30000 fir sd2_o
#endif
	I2SIrqHandle.Data = (u32) (pI2sAdapter);
#if defined(SOC_TYPE_8195A)
	I2SIrqHandle.IrqNum = I2S0_PCM0_IRQ + pI2sVerifyPara->I2sIndex;
#elif defined(SOC_TYPE_8198E)
	I2SIrqHandle.IrqNum = I2S_IRQ + pI2sVerifyPara->I2sIndex;
#elif defined(SOC_TYPE_8196F) || defined(SOC_TYPE_8198F)
	I2SIrqHandle.IrqNum = I2S_IRQ;
#elif defined(SOC_TYPE_8277C)
	I2SIrqHandle.IrqNum = I2S_IRQ;
#else
  #error "no soc type match" 
#endif
	//I2SIrqHandle.IrqNum = 26;
	I2SIrqHandle.IrqFun = (IRQ_FUN) I2SLoopBackIrqHandle;
	I2SIrqHandle.Priority = 0;
//DiagPrintf("a\n");
//        DiagPrintf("(%d)", i2s_init);
	if (i2s_init!=1 ) {
		#if defined(SOC_TYPE_8196F)
			//DiagPrintf("I2S IRQNUM : %x\n",I2SIrqHandle.IrqNum);
			irq_install_handler(I2SIrqHandle.IrqNum,I2SIrqHandle.IrqFun, (u32)(I2SIrqHandle.Data));
	               //InterruptRegister(&I2SIrqHandle);
	    	 	//DiagPrintf("A\n");
	    		//InterruptEn(&I2SIrqHandle);
	    	#elif defined(SOC_TYPE_8198F)
			irq_install_handler(I2SIrqHandle.IrqNum,I2SIrqHandle.IrqFun, (u32)(I2SIrqHandle.Data));
			//irq_init_i2s(I2SIrqHandle.IrqNum,I2SIrqHandle.IrqFun, (u32)(I2SIrqHandle.Data));
		#elif defined(SOC_TYPE_8277C)
			irq_install_handler(I2SIrqHandle.IrqNum,I2SIrqHandle.IrqFun, (u32)(I2SIrqHandle.Data));
		#else
            		InterruptRegister(&I2SIrqHandle);
			DiagPrintf("A\n");
	              InterruptEn(&I2SIrqHandle);
	              i2s_init = 1;
		#endif
        }
//DiagPrintf("b\n");
        //i2s_txpage[pI2sAdapter->pHalI2sAdapter->I2sIndex]=0;
        //i2s_rxpage[pI2sAdapter->pHalI2sAdapter->I2sIndex]=0;
  #if 1
	if (pI2sVerifyPara->I2sTestMode == 3) {//rx only left right channel check.
		//for (i=0 ; i<9128 ; i++)
		for (i=0 ; i<1 ; i++)
			I2sTestMode3(pI2sVerifyPara);
	} else if (pI2sVerifyPara->I2sTestMode == 0) { //loopback test
		I2sTestMode0(pI2sVerifyPara);
	}
  #endif
#endif
printf("Pass\n");
}

VOID
i2s_isr_rx_test(IN  VOID *Data,IN  u32 pindex)
{
        PI2S_ADAPTER pI2sAdapter = (PI2S_ADAPTER) Data;
        PHAL_I2S_ADAPTER pHalI2sAdapter = pI2sAdapter->pHalI2sAdapter;
        u8 I2sIndex = pHalI2sAdapter->I2sIndex;
        u32 i, j;
        u32 i2s_wordlength;
        u32 I2sPageNum=I2sVerifyPere.I2sPageNum+1;
        u32 I2sCHNum=i2s_ch_channelnum_tab1[I2sVerifyPere.I2sCHNum];
        i2s_wordlength = I2sVerifyPere.I2sWordLength;
        if (i2s_wordlength == 1) 
        {
                flush_dcache_range((unsigned long)(i2s_rx[I2sIndex].payload_24bit + i2s_page_size*pindex)&(~(ARCH_DMA_MINALIGN-1)), (unsigned long)(i2s_rx[I2sIndex].payload_24bit + i2s_page_size*(pindex+1)+(ARCH_DMA_MINALIGN/4-1))&(~(ARCH_DMA_MINALIGN-1)));
                invalidate_dcache_range((unsigned long)(i2s_rx[I2sIndex].payload_24bit + i2s_page_size*pindex)&(~(ARCH_DMA_MINALIGN-1)), (unsigned long)(i2s_rx[I2sIndex].payload_24bit + i2s_page_size*(pindex+1)+(ARCH_DMA_MINALIGN/4-1))&(~(ARCH_DMA_MINALIGN-1)));
        } 
        else if (i2s_wordlength == 2) 
        {
                flush_dcache_range((unsigned long)(i2s_rx[I2sIndex].payload_32bit + i2s_page_size*pindex)&(~(ARCH_DMA_MINALIGN-1)), (unsigned long)(i2s_rx[I2sIndex].payload_32bit + i2s_page_size*(pindex+1)+(ARCH_DMA_MINALIGN/4-1))&(~(ARCH_DMA_MINALIGN-1)));
                invalidate_dcache_range((unsigned long)(i2s_rx[I2sIndex].payload_32bit + i2s_page_size*pindex)&(~(ARCH_DMA_MINALIGN-1)), (unsigned long)(i2s_rx[I2sIndex].payload_32bit + i2s_page_size*(pindex+1) + (ARCH_DMA_MINALIGN/4-1))&(~(ARCH_DMA_MINALIGN-1)));
        } 
        else 
        {
                flush_dcache_range((unsigned long)(i2s_rx[I2sIndex].payload + 2*i2s_page_size*pindex)&(~(ARCH_DMA_MINALIGN-1)), (unsigned long)(i2s_rx[I2sIndex].payload + 2*i2s_page_size*(pindex+1)+(ARCH_DMA_MINALIGN/2-1))&(~(ARCH_DMA_MINALIGN-1)));
                invalidate_dcache_range((unsigned long)(i2s_rx[I2sIndex].payload + 2*i2s_page_size*pindex)&(~(ARCH_DMA_MINALIGN-1)), (unsigned long)(i2s_rx[I2sIndex].payload + 2*i2s_page_size*(pindex+1)+(ARCH_DMA_MINALIGN/2-1))&(~(ARCH_DMA_MINALIGN-1)));
        }
#if !I2S_CODEC_ALC5621_OR_ALC5651
        j=0;
        for (i=0 ; i<i2s_page_size*(i2s_wordlength?1:2) ; i++) 
        {
                u32 rx;
                u32 cmp1=0, cmp2=0, cmp3=0;
                if (i2s_wordlength == 1) 
                {
                        rx = *(i2s_rx[I2sIndex].payload_24bit + i2s_page_size*pindex + i);
                        if (i2s_isr_cnt < (9997/i2s_page_time*i2s_test_time_sec-8)) 
                        {
                                // 20141006 jwsyu notes: if page size is not equal = cache line size.
                                // the page 0 final word and page 1 start word are share cache line.
                                // write the page 0 final word will error. the cache line will dirty.
                                if ((i2s_page_size&0xf) && (i > (i2s_page_size-16)))
                                        ;//let the this page final word and next page start word cache line clean;
                                else
                                        *(i2s_rx[I2sIndex].payload_24bit + i2s_page_size*pindex + i) = 0xa5a55a;
                        }
#if I2S_8277C_TEST_MODE
                        t_oldold_value = t_old_value;
						t_old_value = t_value;
						t_value = rx;
                        if ((t_value == 0) && (t_old_value == 0) && (t_oldold_value == 0) ) {
							
						} else if ((t_value-t_oldold_value ==0x200) && (t_value - t_old_value ==0x100) ) {
							
						} else {
							myrx_buffer[t_num][0] = i2s_rxpage_cnt;
							myrx_buffer[t_num][1] = i;
							myrx_buffer[t_num][2] = t_value;
							myrx_buffer[t_num][3] = t_old_value;
							myrx_buffer[t_num][4] = t_oldold_value;
							t_num ++;
							if (t_num>99) {
								t_num=99;
							}
						}
#elif I2S_LR_CH_LOOP

#else
                        #ifdef COMPARE_EVERY_INT
                        if (i<2 && pindex==0) 
                        {
                                if (rx==0)
                                        cmp1=0;
                                else
                                        cmp1=i2s_comp_24bit[i2s_page_size*pindex+i];
                                #ifdef COMPARE_OFFSET
                                cmp2=i2s_comp_24bit[i2s_page_size*I2sPageNum+i-2];
                                #endif
                        } 
                        else 
                        {
                                cmp1=i2s_comp_24bit[i2s_page_size*pindex+i];
                                #ifdef COMPARE_OFFSET
                                cmp2=i2s_comp_24bit[i2s_page_size*pindex+i-2];
                                #endif
                        }
                        #ifdef COMPARE_OFFSET
                        if ( (rx != cmp1) && (rx != cmp2) ) 
                        #else
                        if ( (rx != cmp1) ) 
                        #endif
                        {
                                j++;
                                i2s_err_cnt++;
                                if (j<8) 
                                {
                                        DiagPrintf("\n%d-%d-%d,%x!=%x", i2s_isr_cnt, pindex, i, rx, cmp1);
                                }
                        }
                        #endif
#endif
                } 
                else if (i2s_wordlength == 2) 
                {
                        rx = *(i2s_rx[I2sIndex].payload_32bit + i2s_page_size*pindex + i);
                        if (i2s_isr_cnt < (9997/i2s_page_time*i2s_test_time_sec-8)) 
                        {
                                // 20141006 jwsyu notes: if page size is not equal = cache line size.
                                // the page 0 final word and page 1 start word are share cache line.
                                // write the page 0 final word will error. the cache line will dirty.
                                if ((i2s_page_size&0xf) && (i > (i2s_page_size-16)))
                                        ;//let the this page final word and next page start word cache line clean;
                                else
                                        *(i2s_rx[I2sIndex].payload_32bit + i2s_page_size*pindex + i) = 0x86a5a55a;
                        }
#if I2S_8277C_TEST_MODE
                        t_oldold_value = t_old_value;
						t_old_value = t_value;
						t_value = rx;
                        if ((t_value == 0) && (t_old_value == 0) && (t_oldold_value == 0) ) {
							
						} else if ((t_value-t_oldold_value ==0x10000) && (t_value - t_old_value ==0x8000) ) {
							
						} else {
							myrx_buffer[t_num][0] = i2s_rxpage_cnt;
							myrx_buffer[t_num][1] = i;
							myrx_buffer[t_num][2] = t_value;
							myrx_buffer[t_num][3] = t_old_value;
							myrx_buffer[t_num][4] = t_oldold_value;
							t_num ++;
							if (t_num>99) {
								t_num=99;
							}						}
#elif I2S_LR_CH_LOOP
#else

                        #ifdef COMPARE_EVERY_INT
                        if (i<2 && pindex==0) 
                        {
                                if (rx==0)
                                        cmp1=0;
                                else
                                        cmp1=i2s_comp_32bit[i2s_page_size*pindex+i];
                                #ifdef COMPARE_OFFSET
                                cmp2=i2s_comp_32bit[i2s_page_size*I2sPageNum+i-2];
                                #endif
                        } 
                        else 
                        {
                                cmp1=i2s_comp_32bit[i2s_page_size*pindex+i];
                                #ifdef COMPARE_OFFSET
                                cmp2=i2s_comp_32bit[i2s_page_size*pindex+i-2];
                                #endif
                        }
                        #ifdef COMPARE_OFFSET
                        if ( (rx != cmp1) && (rx != cmp2) ) 
                        #else
                        if ( (rx != cmp1) )
                        #endif
                        {
                                j++;
                                i2s_err_cnt++;
                                if (j<8) 
                                {
                                        DiagPrintf("\n%d-%d-%d,%x!=%x  (addr : 0x%x)", i2s_isr_cnt, pindex, i, rx, cmp1, (unsigned long)(i2s_rx[I2sIndex].payload_32bit + i2s_page_size*pindex + i));
                                }
                        }
                        #endif
#endif
                } 
                else 
                {
                        rx = *(i2s_rx[I2sIndex].payload + i2s_page_size*2*pindex + i);
                        if (i2s_isr_cnt < (9992/i2s_page_time*i2s_test_time_sec-8)) 
                        {
                                // 20141006 jwsyu notes: if page size is not equal = cache line size.
                                // the page 0 final word and page 1 start word are share cache line.
                                // write the page 0 final word will error. the cache line will dirty.
                                if ((i2s_page_size&0xf) && (i > (i2s_page_size*2-32)))
                                        ;//let the this page final word and next page start word cache line clean;
                                else
                                        *(i2s_rx[I2sIndex].payload + i2s_page_size*2*pindex + i)= 0xa55a;
                        }
#if I2S_8277C_TEST_MODE
                        t_oldold_value = t_old_value;
						t_old_value = t_value;
						t_value = rx;
                        if ((t_value == 0) && (t_old_value == 0) && (t_oldold_value == 0) ) {
							
						} else if ((t_value-t_oldold_value ==2) && (t_value - t_old_value ==1) ) {
							
						} else {
							myrx_buffer[t_num][0] = i2s_rxpage_cnt;
							myrx_buffer[t_num][1] = i;
							myrx_buffer[t_num][2] = t_value;
							myrx_buffer[t_num][3] = t_old_value;
							myrx_buffer[t_num][4] = t_oldold_value;
							t_num ++;
							if (t_num>99) {
								t_num=99;
							}
						}
#elif I2S_LR_CH_LOOP
#else
#ifdef COMPARE_EVERY_INT
                        if (i<2 && pindex==0) 
                        {
                                if (rx==0)
                                        cmp1=0;
                                else
                                        cmp1=i2s_comp[i2s_page_size*2*pindex+i];
                                #ifdef COMPARE_OFFSET
                                cmp2=i2s_comp[i2s_page_size*2*I2sPageNum*2+i-2];
                                #endif
                        } 
                        else 
                        {
                                cmp1=i2s_comp[i2s_page_size*2*pindex+i];
                                #ifdef COMPARE_OFFSET
                                cmp2=i2s_comp[i2s_page_size*2*pindex+i-2];
                                #endif
                        }
                        #ifdef COMPARE_OFFSET
                        if (I2sCHNum==1) 
                        {
                                if (i<1 && pindex==0)
                                        cmp3 = i2s_comp[i2s_page_size*2*I2sPageNum+i-1];
                                else
                                        cmp3 = i2s_comp[i2s_page_size*2*pindex+i-1];
                        } 
                        else
                                cmp3 = cmp1;
                        #endif
                        #ifdef COMPARE_OFFSET
                        if ( (rx != cmp1) && (rx != cmp2) && (rx != cmp3) ) 
                        #else
                        if ( (rx != cmp1) ) 
                        #endif
                        {
                                //if (((i2s_isr_cnt&0x3ff) == 0) || ((i2s_isr_cnt&0x3ff) == 0x101) || ((i2s_isr_cnt&0x3ff)== 0x202 ) || ((i2s_isr_cnt&0x3ff)== 0x303 )) { //temp reduce print
                                j++;
                                i2s_err_cnt++;
                                if (j<8) 
                                {
                                        DiagPrintf("\n%d-%d-%d,%x!=%x (addr : 0x%x)", i2s_isr_cnt, pindex, i, rx, cmp1, (unsigned long)(i2s_rx[I2sIndex].payload + i2s_page_size*2*pindex + i));
                                }
                                //}
                        }
                        #endif
#endif
                }
#if !I2S_8277C_TEST_MODE
#ifdef I2S_IN_FPGA
                //check less sample
                if (I2sVerifyPere.I2sSampleRate > 4) 
                {
                        i += I2sVerifyPere.I2sSampleRate*2;
                }
#endif
#endif
        }

        if (i2s_wordlength == 1) 
        {
                flush_dcache_range((unsigned long)(i2s_rx[I2sIndex].payload_24bit + i2s_page_size*pindex)&(~(ARCH_DMA_MINALIGN-1)), (unsigned long)(i2s_rx[I2sIndex].payload_24bit + i2s_page_size*(pindex+1)+(ARCH_DMA_MINALIGN/4-1))&(~(ARCH_DMA_MINALIGN-1)));
                invalidate_dcache_range((unsigned long)(i2s_rx[I2sIndex].payload_24bit + i2s_page_size*pindex)&(~(ARCH_DMA_MINALIGN-1)), (unsigned long)(i2s_rx[I2sIndex].payload_24bit + i2s_page_size*(pindex+1)+(ARCH_DMA_MINALIGN/4-1))&(~(ARCH_DMA_MINALIGN-1)));
        } 
        else if (i2s_wordlength == 2) 
        {
                flush_dcache_range((unsigned long)(i2s_rx[I2sIndex].payload_32bit + i2s_page_size*pindex)&(~(ARCH_DMA_MINALIGN-1)), (unsigned long)(i2s_rx[I2sIndex].payload_32bit + i2s_page_size*(pindex+1)+(ARCH_DMA_MINALIGN/4-1))&(~(ARCH_DMA_MINALIGN-1)));
                invalidate_dcache_range((unsigned long)(i2s_rx[I2sIndex].payload_32bit + i2s_page_size*pindex)&(~(ARCH_DMA_MINALIGN-1)), (unsigned long)(i2s_rx[I2sIndex].payload_32bit + i2s_page_size*(pindex+1) + (ARCH_DMA_MINALIGN/4-1))&(~(ARCH_DMA_MINALIGN-1)));
        } 
        else 
        {
                flush_dcache_range((unsigned long)(i2s_rx[I2sIndex].payload + 2*i2s_page_size*pindex)&(~(ARCH_DMA_MINALIGN-1)), (unsigned long)(i2s_rx[I2sIndex].payload + 2*i2s_page_size*(pindex+1)+(ARCH_DMA_MINALIGN/2-1))&(~(ARCH_DMA_MINALIGN-1)));
                invalidate_dcache_range((unsigned long)(i2s_rx[I2sIndex].payload + 2*i2s_page_size*pindex)&(~(ARCH_DMA_MINALIGN-1)), (unsigned long)(i2s_rx[I2sIndex].payload + 2*i2s_page_size*(pindex+1)+(ARCH_DMA_MINALIGN/2-1))&(~(ARCH_DMA_MINALIGN-1)));
        }


#endif
#ifdef I2S_MIC_LOOPBACK_SPEAKER_TEST
        u32 cut_size, buf_addr;
        if (i2s_wordlength == 1) 
        {
                if((rx_write_index+i2s_page_size*4) > DATABUFSIZE)
                {
                        cut_size =  DATABUFSIZE - rx_write_index;
                        buf_addr = (u32)(i2s_rx[I2sIndex].payload_24bit + i2s_page_size*pindex);
                        memcpy(&databuf[rx_write_index], (void *)buf_addr, cut_size); 
                        buf_addr = (u32)(i2s_rx[I2sIndex].payload_24bit + i2s_page_size*pindex) + cut_size;
                        cut_size = (rx_write_index+i2s_page_size*4) - DATABUFSIZE;
                        memcpy(&databuf[0], (void *)buf_addr, cut_size); 
                        
                }
                else
                {
                        memcpy(&databuf[rx_write_index], i2s_rx[I2sIndex].payload_24bit + i2s_page_size*pindex, 
                        i2s_page_size*4);
                }
        } 
        else if (i2s_wordlength == 2) 
        {
                if((rx_write_index+i2s_page_size*4) > DATABUFSIZE)
                {
                        cut_size =  DATABUFSIZE - rx_write_index;
                        buf_addr = (u32)(i2s_rx[I2sIndex].payload_32bit + i2s_page_size*pindex);
                        memcpy(&databuf[rx_write_index], (void *)buf_addr, cut_size); 
                        buf_addr = (u32)(i2s_rx[I2sIndex].payload_32bit + i2s_page_size*pindex) + cut_size;
                        cut_size = (rx_write_index+i2s_page_size*4) - DATABUFSIZE;
                        memcpy(&databuf[0], (void *)buf_addr, cut_size);       
                }
                else
                {
                        memcpy(&databuf[rx_write_index], i2s_rx[I2sIndex].payload_32bit + i2s_page_size*pindex, 
                        i2s_page_size*4);
                }
        } 
        else 
        {
                if((rx_write_index+i2s_page_size*4) > DATABUFSIZE)
                {
                        cut_size =  DATABUFSIZE - rx_write_index;
                        buf_addr = (u32)(i2s_rx[I2sIndex].payload + i2s_page_size*pindex*2);
                        memcpy(&databuf[rx_write_index], (void *)buf_addr, cut_size); 
                        buf_addr = (u32)(i2s_rx[I2sIndex].payload + i2s_page_size*pindex*2) + cut_size;
                        cut_size = (rx_write_index+i2s_page_size*4) - DATABUFSIZE;
                        memcpy(&databuf[0], (void *)buf_addr, cut_size);   
                }
                else
                {
                        memcpy(&databuf[rx_write_index], i2s_rx[I2sIndex].payload + i2s_page_size*pindex*2, 
                        i2s_page_size*4);
                }
        }
        rx_write_index = (rx_write_index+i2s_page_size*4)%DATABUFSIZE;
#endif
//printf("RX Pass\n");
}

VOID
i2s_isr_tx_test(IN  VOID *Data,IN  u32 pindex)
{
        PI2S_ADAPTER pI2sAdapter = (PI2S_ADAPTER) Data;
        PHAL_I2S_ADAPTER pHalI2sAdapter = pI2sAdapter->pHalI2sAdapter;
        u8 I2sIndex = pHalI2sAdapter->I2sIndex;
        u32 i, j;
        u32 i2s_wordlength;
        u32 I2sPageNum=I2sVerifyPere.I2sPageNum+1;
        u32 I2sCHNum=i2s_ch_channelnum_tab1[I2sVerifyPere.I2sCHNum];
        i2s_wordlength = I2sVerifyPere.I2sWordLength;
#ifdef I2S_PLAY_TONE_TEST
#ifdef I2S_TONEGEN_TABLE
        if (i2s_wordlength == 1) 
        {
                for (i=0 ; i< ((I2sVerifyPere.I2sPageSize+1)) ; i++) 
                {
                        if (sample_count==play_channel_now) 
                        {
                                i2s_tx[I2sVerifyPere.I2sIndex].payload_24bit[i+i2s_page_size*pindex] = i2s_vpat24bit[tone_phase];
                                tone_phase++;
                                if (tone_phase == 16)
                                        tone_phase = 0;
                        } 
                        else
                                i2s_tx[I2sVerifyPere.I2sIndex].payload_24bit[i+i2s_page_size*pindex] = 0;
                        sample_count++;
                        if (sample_count == i2s_allchannel)
                                sample_count = 0;
                }
        } 
        else if (i2s_wordlength == 2) 
        {
                for (i=0 ; i< ((I2sVerifyPere.I2sPageSize+1)) ; i++) 
                {
                        if (sample_count==play_channel_now) 
                        {
                                i2s_tx[I2sVerifyPere.I2sIndex].payload_32bit[i+i2s_page_size*pindex] = i2s_vpat32bit[tone_phase];
                        tone_phase++;
                        if (tone_phase == 16)
                                tone_phase = 0;
                        } 
                        else
                                i2s_tx[I2sVerifyPere.I2sIndex].payload_32bit[i+i2s_page_size*pindex] = 0;
                        sample_count++;
                        if (sample_count == i2s_allchannel)
                                sample_count = 0;
                }
        } 
        else 
        {
                for (i=0 ; i< ((I2sVerifyPere.I2sPageSize+1)*2) ; i++) 
                {
                        if (sample_count==play_channel_now) 
                        {
                                i2s_tx[I2sVerifyPere.I2sIndex].payload[i+i2s_page_size*pindex*2] = i2s_vpat[tone_phase];
                                tone_phase++;
                                if (tone_phase == 16)
                                        tone_phase = 0;
                        } 
                        else
                                i2s_tx[I2sVerifyPere.I2sIndex].payload[i+i2s_page_size*pindex*2] = 0;
                        sample_count++;
                        if (sample_count == i2s_allchannel)
                                sample_count = 0;
                }
        }
#else
if (i2s_wordlength == 1)
        I2SToneGens24bit(i2s_page_size, i2s_tx[I2sIndex].payload_24bit + i2s_page_size*pindex);
        else if (i2s_wordlength == 2)
        I2SToneGens32bit(i2s_page_size, i2s_tx[I2sIndex].payload_32bit + i2s_page_size*pindex);
        else
        I2SToneGens(i2s_page_size*2, i2s_tx[I2sIndex].payload + i2s_page_size*pindex*2);
#endif
#endif
#ifdef I2S_MIC_LOOPBACK_SPEAKER_TEST
        u32 cut_size, buf_addr;
        if (i2s_wordlength == 1)
        {
                if((tx_read_index+i2s_page_size*4) > DATABUFSIZE)
                {
                        cut_size = DATABUFSIZE - tx_read_index;
                        buf_addr = (u32)(i2s_tx[I2sIndex].payload_24bit + i2s_page_size*pindex);
                        memcpy((void*) buf_addr, &databuf[tx_read_index], cut_size);
                        buf_addr = (u32)(i2s_tx[I2sIndex].payload_24bit + i2s_page_size*pindex) + cut_size;
                        cut_size = (tx_read_index+i2s_page_size*4) - DATABUFSIZE;
                        memcpy(buf_addr, &databuf[0], cut_size);
                }
                else
                {
                        memcpy(i2s_tx[I2sIndex].payload_24bit + i2s_page_size*pindex, &databuf[tx_read_index],
                               i2s_page_size*4);
                }
        }
        else if (i2s_wordlength == 2)
        {
                if((tx_read_index+i2s_page_size*4) > DATABUFSIZE)
                {
                        cut_size = DATABUFSIZE - tx_read_index;
                        buf_addr = (u32)(i2s_tx[I2sIndex].payload_32bit + i2s_page_size*pindex);
                        memcpy((void*) buf_addr, &databuf[tx_read_index], cut_size);
                        buf_addr = (u32)(i2s_tx[I2sIndex].payload_32bit + i2s_page_size*pindex) + cut_size;
                        cut_size = (tx_read_index+i2s_page_size*4) - DATABUFSIZE;
                        memcpy(buf_addr, &databuf[0], cut_size);
                }
                else
                {
                        memcpy(i2s_tx[I2sIndex].payload_32bit + i2s_page_size*pindex, &databuf[tx_read_index],
                               i2s_page_size*4);
                }
        }
        else
        {
                if((tx_read_index+i2s_page_size*4) > DATABUFSIZE)
                {
                        cut_size = DATABUFSIZE - tx_read_index;
                        buf_addr = (u32)(i2s_tx[I2sIndex].payload + i2s_page_size*pindex*2);
                        memcpy((void*) buf_addr, &databuf[tx_read_index], cut_size);
                        buf_addr = (u32)(i2s_tx[I2sIndex].payload + i2s_page_size*pindex*2) + cut_size;
                        cut_size = (tx_read_index+i2s_page_size*4) - DATABUFSIZE;
                        memcpy(buf_addr, &databuf[0], cut_size);
                }
                else
                {
                        memcpy(i2s_tx[I2sIndex].payload + i2s_page_size*pindex*2, &databuf[tx_read_index],
                               i2s_page_size*4);
                }
        }
        tx_read_index = (tx_read_index+i2s_page_size*4)%DATABUFSIZE;       
#endif
#if I2S_8277C_TEST_MODE
        if (i2s_wordlength == 1) 
        {
                for (i=0 ; i< ((I2sVerifyPere.I2sPageSize+1)) ; i++) 
                {
                                i2s_tx[I2sVerifyPere.I2sIndex].payload_24bit[i+i2s_page_size*pindex] = i2s_8277c_inc_value;
                                i2s_8277c_inc_value+=0x100;
                }
        } 
        else if (i2s_wordlength == 2) 
        {
                for (i=0 ; i< ((I2sVerifyPere.I2sPageSize+1)) ; i++) 
                {
                        i2s_tx[I2sVerifyPere.I2sIndex].payload_32bit[i+i2s_page_size*pindex] = i2s_8277c_inc_value;
                        i2s_8277c_inc_value+=0x8000;
                }
        } 
        else 
        {
                for (i=0 ; i< ((I2sVerifyPere.I2sPageSize+1)*2) ; i++) 
                {
                                i2s_tx[I2sVerifyPere.I2sIndex].payload[i+i2s_page_size*pindex*2] = i2s_8277c_inc_value;
                                i2s_8277c_inc_value++;
                }
        }
#endif
        if (i2s_wordlength == 1) 
        {
                flush_dcache_range((unsigned long)(i2s_tx[I2sIndex].payload_24bit + i2s_page_size*pindex)&(~(ARCH_DMA_MINALIGN-1)), (unsigned long)(i2s_tx[I2sIndex].payload_24bit + i2s_page_size*(pindex+1)+15)&(~(ARCH_DMA_MINALIGN-1)));
        } 
        else if (i2s_wordlength == 2) 
        {
                flush_dcache_range((unsigned long)(i2s_tx[I2sIndex].payload_32bit + i2s_page_size*pindex)&(~(ARCH_DMA_MINALIGN-1)), (unsigned long)(i2s_tx[I2sIndex].payload_32bit + i2s_page_size*(pindex+1)+15)&(~(ARCH_DMA_MINALIGN-1)));
        } 
        else 
        {
                flush_dcache_range((unsigned long)(i2s_tx[I2sIndex].payload + 2*i2s_page_size*pindex)&(~(ARCH_DMA_MINALIGN-1)), (unsigned long)(i2s_tx[I2sIndex].payload + 2*i2s_page_size*(pindex+1)+63)&(~(ARCH_DMA_MINALIGN-1)));
        }
//printf("TX Pass\n");
}

VOID
I2S_ISR(
    IN  VOID        *Data,
    IN  u32         I2sTxIsr,
    IN  u32         I2sRxIsr
)
{
        PI2S_ADAPTER pI2sAdapter = (PI2S_ADAPTER) Data;
        PHAL_I2S_ADAPTER pHalI2sAdapter = pI2sAdapter->pHalI2sAdapter;
        u8 I2sIndex = pHalI2sAdapter->I2sIndex;
        u32 I2sPageNum;
        u32 I2sPageSize;
        u32 i, j;

        I2sPageNum = ((HAL_I2S_READ32(I2sIndex, REG_I2S_SETTING)>>12)&0x3) +1;
        I2sPageSize = (HAL_I2S_READ32(I2sIndex, REG_I2S_SETTING)&0xfff) +1;


        for (i=0 ; i<I2sPageNum ; i++) 
        { // page0, page1, page2, page3
                if ( I2sTxIsr & I2SChanTxPage[i2s_txpage[I2sIndex]] )
                {
                        i2s_isr_tx_test( pI2sAdapter, i2s_txpage[I2sIndex]);
                        i2s_isr_cnt++;
                        HAL_I2S_WRITE32(I2sIndex, REG_I2S_TX_PAGE0_OWN+i2s_txpage[I2sIndex]*REG_I2S_PAGE_OWN_OFF, BIT31);
                        I2sTxIsr &= ~I2SChanTxPage[i2s_txpage[I2sIndex]];
                        i2s_txpage[I2sIndex] += 1;
                        if (i2s_txpage[I2sIndex] == I2sPageNum )
                        i2s_txpage[I2sIndex] = 0;
                        //DiagPrintf(".");
                        if ((i2s_isr_cnt % (10000/i2s_page_time)) == (10000/i2s_page_time-1)) 
                        {
                                DiagPrintf(",");
                        }
                        if ((i2s_isr_cnt % (600000/i2s_page_time)) == (600000/i2s_page_time-1)) 
                        {
                                DiagPrintf("\n");
                        }
                } // end of tx

                if ( I2sRxIsr & I2SChanRxPage[i2s_rxpage[I2sIndex]] )
                {
#if !I2S_CODEC_ALC5621_OR_ALC5651
                        //if (I2sVerifyPere.I2sSampleRate < 8)
#endif
                        i2s_isr_rx_test( pI2sAdapter, i2s_rxpage[I2sIndex]);
#if I2S_8277C_TEST_MODE
						i2s_rxpage_cnt ++;
#endif
                        HAL_I2S_WRITE32(I2sIndex, REG_I2S_RX_PAGE0_OWN+i2s_rxpage[I2sIndex]*REG_I2S_PAGE_OWN_OFF, BIT31);
                        I2sRxIsr &= ~I2SChanRxPage[i2s_rxpage[I2sIndex]];
                        i2s_rxpage[I2sIndex] += 1;
                        if (i2s_rxpage[I2sIndex] == I2sPageNum )
                        i2s_rxpage[I2sIndex] = 0;
                        //DiagPrintf("*");
                } // end of rx

        }

        //i2s stop check.
        //if (i2s_isr_cnt > (10000/I2S_PAGE_TIME*i2s_test_time_sec)) {
        
        if ((i2s_isr_cnt > (10000/i2s_page_time*i2s_test_time_sec)) || (i2s_err_cnt>800)) 
        {
#if 1//FLYSKY_DEBUG
                printf("\n i2s_isr_cnt = %d, i2s_err_cnt = %d \n", i2s_isr_cnt, i2s_err_cnt);
#endif
                HAL_I2S_WRITE32(I2sIndex, REG_I2S_TX_MASK_INT, 0);
                HAL_I2S_WRITE32(I2sIndex, REG_I2S_RX_MASK_INT, 0);
#if 0   //delay and stop i2s let own bit page clean by i2s controller
                i=2100000;
                while (i) {
                i--;
                asm volatile ("nop\n\t");
                }
#endif
#ifdef LOCK_LX_BUS
                LockLx1Bus();	
#endif
                i = HAL_I2S_READ32(I2sIndex, REG_I2S_CTL);
                //i &= (~(BIT31|BIT0));
				i &= (~(BIT26|BIT25|BIT24|BIT23|BIT0));
                HAL_I2S_WRITE32(I2sIndex, REG_I2S_CTL, i);		
#ifdef LOCK_LX_BUS
                UnlockLx1Bus();	
#endif
#if (I2S_CODEC_ALC5621_OR_ALC5651) && defined(I2S_PLAY_TONE_TEST)
                if (I2sVerifyPere.I2sWordLength == 1) 
                {
                        u32 print_size;
                        print_size = (I2sPageSize>270)? 270: I2sPageSize;
#if FLYSKY_DEBUG
                        DiagPrintf("i2s tx page data:\n");
                        for (i=0 ; i<(print_size*I2sPageNum+1)+16; i++) 
                        {
                                if ((i%8) == 7)
                                        DiagPrintf(" %x\n", i2s_tx[I2sIndex].payload_24bit[i]);
                                else
                                        DiagPrintf(" %x ", i2s_tx[I2sIndex].payload_24bit[i]);
                        }
#endif
                } 
                else if (I2sVerifyPere.I2sWordLength == 2) 
                {
                        u32 print_size;
                        print_size = (I2sPageSize>270)? 270: I2sPageSize;
#if FLYSKY_DEBUG
                        DiagPrintf("i2s tx page data:\n");
                        for (i=0 ; i<(print_size*I2sPageNum+1)+16; i++) 
                        {
                                if ((i%8) == 7)
                                        DiagPrintf(" %x\n", i2s_tx[I2sIndex].payload_32bit[i]);
                                else
                                        DiagPrintf(" %x ", i2s_tx[I2sIndex].payload_32bit[i]);
                        }
#endif
                } 
                else 
                {
                        u32 print_size;
                        print_size = (I2sPageSize>270)? 270: I2sPageSize;
#if FLYSKY_DEBUG
                        DiagPrintf("i2s tx page data:\n");
                        for (i=0 ; i<(print_size*I2sPageNum*2)+16; i++) 
                        {
                                if ((i%8) == 7)
                                        DiagPrintf(" %x\n", i2s_tx[I2sIndex].payload[i]);
                                else
                                        DiagPrintf(" %x ", i2s_tx[I2sIndex].payload[i]);
                        }
#endif
                } 
#else
                if (I2sVerifyPere.I2sWordLength == 1) 
                {
                        u32 print_size;
                        print_size = (I2sPageSize>270)? 270: I2sPageSize;
#if 1//FLYSKY_DEBUG
                        printf("i2s rx page data:\n");
                        for (i=0 ; i<(print_size*I2sPageNum+1)+16; i++) 
                        {
                                if ((i%8) == 7)
                                        printf(" %x\n", i2s_rx[I2sIndex].payload_24bit[i]);
                                else
                                        printf(" %x ", i2s_rx[I2sIndex].payload_24bit[i]);
                        }
#endif
                } 
                else if (I2sVerifyPere.I2sWordLength == 2) 
                {
                        u32 print_size;
                        print_size = (I2sPageSize>270)? 270: I2sPageSize;
#if 1//FLYSKY_DEBUG
                        printf("i2s rx page data:\n");
                        for (i=0 ; i<(print_size*I2sPageNum+1)+16; i++) 
                        {
                                if ((i%8) == 7)
                                        printf(" %x\n", i2s_rx[I2sIndex].payload_32bit[i]);
                                else
                                        printf(" %x ", i2s_rx[I2sIndex].payload_32bit[i]);
                        }
#endif
                } 
                else 
                {
                        u32 print_size;
                        print_size = (I2sPageSize>270)? 270: I2sPageSize;
#if 1//FLYSKY_DEBUG
                        DiagPrintf("i2s rx page data:\n");
                        for (i=0 ; i<(print_size*I2sPageNum*2)+16; i++) 
                        {
                                if (i%((print_size)*2) == 0)
                                DiagPrintf("============================\n");

                                if ((i%8) == 7)
                                        printf(" %x\n", i2s_rx[I2sIndex].payload[i]);
                                else
                                        printf(" %x ", i2s_rx[I2sIndex].payload[i]);
                        }
#endif
                } 

#endif
#if !I2S_CODEC_ALC5621_OR_ALC5651
                shift = 0;
                if (I2sVerifyPere.I2sWordLength == 1) 
                {
                        if (i2s_rx[I2sIndex].payload_24bit[0] == i2s_tx[I2sIndex].payload_24bit[0] ) 
                        {
                                DiagPrintf("\noffset=0\n"); shift=0;
                        } 
                        else if (i2s_rx[I2sIndex].payload_24bit[1] == i2s_tx[I2sIndex].payload_24bit[0] ) 
                        {
                                DiagPrintf("\noffset=1\n"); shift=1;
                        } 
                        else if (i2s_rx[I2sIndex].payload_24bit[2] == i2s_tx[I2sIndex].payload_24bit[0] ) 
                        {
                                DiagPrintf("\noffset=2\n"); shift=2;
                        } 
                        else if (i2s_rx[I2sIndex].payload_24bit[3] == i2s_tx[I2sIndex].payload_24bit[0] ) 
                        {
                                DiagPrintf("\noffset=3\n"); shift=3;
                        } 
                        else if (i2s_rx[I2sIndex].payload_24bit[4] == i2s_tx[I2sIndex].payload_24bit[0] ) 
                        {
                                DiagPrintf("\noffset=4\n"); shift=4;
                        } 
                        else
                                DiagPrintf("\noffset error\n");
                } 
                else if (I2sVerifyPere.I2sWordLength == 2) 
                {
                        if (i2s_rx[I2sIndex].payload_32bit[0] == i2s_tx[I2sIndex].payload_32bit[0] ) 
                        {
                                DiagPrintf("\noffset=0\n"); shift=0;
                        } 
                        else if (i2s_rx[I2sIndex].payload_32bit[1] == i2s_tx[I2sIndex].payload_32bit[0] ) 
                        {
                                DiagPrintf("\noffset=1\n"); shift=1;
                        } 
                        else if (i2s_rx[I2sIndex].payload_32bit[2] == i2s_tx[I2sIndex].payload_32bit[0] ) 
                        {
                                DiagPrintf("\noffset=2\n"); shift=2;
                        } 
                        else if (i2s_rx[I2sIndex].payload_32bit[3] == i2s_tx[I2sIndex].payload_32bit[0] ) 
                        {
                                DiagPrintf("\noffset=3\n"); shift=3;
                        } 
                        else if (i2s_rx[I2sIndex].payload_32bit[4] == i2s_tx[I2sIndex].payload_32bit[0] ) 
                        {
                                DiagPrintf("\noffset=4\n"); shift=4;
                        } 
                        else
                                DiagPrintf("\noffset error\n");
                } 
                else 
                {
                        if (i2s_rx[I2sIndex].payload[0] == i2s_tx[I2sIndex].payload[0] ) 
                        {
                                DiagPrintf("\noffset=0\n"); shift=0;
                        } 
                        else if (i2s_rx[I2sIndex].payload[1] == i2s_tx[I2sIndex].payload[0] ) 
                        {
                                DiagPrintf("\noffset=1\n"); shift=1;
                        } 
                        else if (i2s_rx[I2sIndex].payload[2] == i2s_tx[I2sIndex].payload[0] ) 
                        {
                                DiagPrintf("\noffset=2\n"); shift=2;
                        } 
                        else if (i2s_rx[I2sIndex].payload[3] == i2s_tx[I2sIndex].payload[0] ) 
                        {
                                DiagPrintf("\noffset=3\n"); shift=3;
                        } 
                        else if (i2s_rx[I2sIndex].payload[4] == i2s_tx[I2sIndex].payload[0] ) 
                        {
                                DiagPrintf("\noffset=4\n"); shift=4;
                        } 
                        else
                                DiagPrintf("\noffset error\n");
                }
                j=0;
                if (I2sVerifyPere.I2sWordLength == 1) 
                {
                        for (i=0 ; i<(I2sPageSize*I2sPageNum-shift) ; i++) 
                        {
                                if (i2s_rx[I2sIndex].payload_24bit[i+shift] != i2s_tx[I2sIndex].payload_24bit[i]) 
                                {
#if FLYSKY_DEBUG
                                        DiagPrintf(" %d, %x!=%x\n", i, i2s_rx[I2sIndex].payload_24bit[i+shift], i2s_tx[I2sIndex].payload_24bit[i]);
#endif
                                        j++;
                                }
                                if (j > 30)
                                        break;
                        }
                }
                else if (I2sVerifyPere.I2sWordLength == 2) 
                {
                        for (i=0 ; i<(I2sPageSize*I2sPageNum-shift) ; i++) 
                        {
                                if (i2s_rx[I2sIndex].payload_32bit[i+shift] != i2s_tx[I2sIndex].payload_32bit[i]) 
                                {
#if FLYSKY_DEBUG
                                        DiagPrintf(" %d, %x!=%x\n", i, i2s_rx[I2sIndex].payload_32bit[i+shift], i2s_tx[I2sIndex].payload_32bit[i]);
#endif
                                        j++;
                                }
                                if (j > 30)
                                        break;
                        }
                }
                else 
                {
                        for (i=0 ; i<(I2sPageSize*I2sPageNum*2-shift) ; i++) 
                        {
                                if (i2s_rx[I2sIndex].payload[i+shift] != i2s_tx[I2sIndex].payload[i]) 
                                {
#if FLYSKY_DEBUG
                                        DiagPrintf(" %d, %x!=%x\n", i, i2s_rx[I2sIndex].payload[i+shift], i2s_tx[I2sIndex].payload[i]);
#endif
                                        j++;
                                }
                                if (j > 30)
                                        break;
                        }

                }
#if FLYSKY_DEBUG
                DBG_8195A_DRIVER("check error number=%d\n", j);
#endif
#endif

#if I2S_8277C_TEST_MODE

                if (I2sVerifyPere.I2sWordLength == 1) 
                {
                        u32 print_size;
                        print_size = (I2sPageSize>270)? 270: I2sPageSize;
#if 1
                        printf("i2s tx page data:\n");
                        for (i=0 ; i<(print_size*I2sPageNum+1)+16; i++) 
                        {
                                if ((i%8) == 7)
                                        printf(" %x\n", i2s_tx[I2sIndex].payload_24bit[i]);
                                else
                                        printf(" %x ", i2s_tx[I2sIndex].payload_24bit[i]);
                        }
#endif
                } 
                else if (I2sVerifyPere.I2sWordLength == 2) 
                {
                        u32 print_size;
                        print_size = (I2sPageSize>270)? 270: I2sPageSize;
#if 1
                        printf("i2s tx page data:\n");
                        for (i=0 ; i<(print_size*I2sPageNum+1)+16; i++) 
                        {
                                if ((i%8) == 7)
                                        printf(" %x\n", i2s_tx[I2sIndex].payload_32bit[i]);
                                else
                                        printf(" %x ", i2s_tx[I2sIndex].payload_32bit[i]);
                        }
#endif
                } 
                else 
                {
                        u32 print_size;
                        print_size = (I2sPageSize>270)? 270: I2sPageSize;
#if 1
                        printf("i2s tx page data:\n");
                        for (i=0 ; i<(print_size*I2sPageNum*2)+16; i++) 
                        {
                                if ((i%8) == 7)
                                        printf(" %x\n", i2s_tx[I2sIndex].payload[i]);
                                else
                                        printf(" %x ", i2s_tx[I2sIndex].payload[i]);
                        }
#endif
                } 

                    printf("----------- t_num=%d\n", t_num);
                    for (i=0 ; i<(t_num) ; i++) 
                    {
                        printf(" %d, %d, %x, [%x, %x, %x]\n", i, myrx_buffer[i][0], myrx_buffer[i][1], myrx_buffer[i][2], myrx_buffer[i][3], myrx_buffer[i][4]);
                    }

#endif


                I2SRegisterDump(pI2sAdapter);
                DBG_8195A_DRIVER("==============>> i2s end test   << ===================\n");
//printf("ISR Pass\n");
        }
}


VOID
I2SLoopBackIrqHandle
(
    IN  VOID        *Data
)
{
    PI2S_ADAPTER pI2sAdapter = (PI2S_ADAPTER) Data;
    PHAL_I2S_ADAPTER pHalI2sAdapter = pI2sAdapter->pHalI2sAdapter;
    u8 I2sIndex = pHalI2sAdapter->I2sIndex;

    u32 I2sTxIsr, I2sRxIsr;

    I2sTxIsr = HAL_I2S_READ32(I2sIndex, REG_I2S_TX_STATUS_INT);
    I2sRxIsr = HAL_I2S_READ32(I2sIndex, REG_I2S_RX_STATUS_INT);

//DiagPrintf("c%x", I2sTxIsr*16+I2sRxIsr);
    HAL_I2S_WRITE32(I2sIndex, REG_I2S_TX_STATUS_INT, I2sTxIsr);
    HAL_I2S_WRITE32(I2sIndex, REG_I2S_RX_STATUS_INT, I2sRxIsr);

    I2S_ISR(pI2sAdapter, I2sTxIsr & 0x0F, I2sRxIsr & 0x0F);
#if 0
if (I2sTxIsr&0xf)
	DiagPrintf("%d T 0x%x\n", i2s_isr_cnt,I2sTxIsr);
if (I2sRxIsr&0xf)
	DiagPrintf("%d R 0x%x\n", i2s_isr_cnt,I2sRxIsr);
#endif
#if FLYSKY_DEBUG
if (I2sTxIsr&0xf0)
	DiagPrintf("%d TBU 0x%x\n", i2s_isr_cnt,I2sTxIsr);
if (I2sRxIsr&0xf0)
	DiagPrintf("%d RBU 0x%x\n", i2s_isr_cnt,I2sRxIsr);
if (I2sTxIsr&0x100)
	DiagPrintf("%d TE 0x%x\n", i2s_isr_cnt, I2sTxIsr);
if (I2sRxIsr&0x100)
	DiagPrintf("%d RF 0x%x\n", i2s_isr_cnt, I2sRxIsr);
#endif




}
#ifdef I2S_PLAY_TONE_TEST
s16
i2s_sinus
(
    IN  s32    x
)
{
    s16 i;
    s32 x2;      //Q15
    s32 q;
    s32 res;
    s16 coef[5] = { (s16)0x3240, (s16)0x0054, (s16)0xaacc,
                    (s16)0x08b7, (s16)0x1cce }; //Q12

    if (x > 0x00008000L)
        x2 = x - 0x00008000L;
    else
        x2 = x;

    if (x2 > 0x00004000L)
        x2 = 0x00008000L - x2;
    q = x2;
    res = 0;
    for (i=0 ; i<5 ; i++) {
        res += coef[i] * x2;                  // Q27
        x2 *= q;                              // Q30
        x2 >>= 15;                            // Q15
    }
    res >>= 12;  /* back to 0x0000-0xffff */   // Q15
    if (x > 0x00008000L)
        res = -res;
    if (res > 32767)
        res = 32767;
    else if (res < -32768)
        res = -32768;

    return (s16) res;
}

VOID
I2SToneGens
(
    IN  u32    sample_num,
    IN  s16   *buffPtr
)
{
    u32 i;

    tone_phase_ad = (1014 * 16777) >> 11;    // 65535/8000 in Q11

    if (play_channel >= i2s_allchannel) {
        if (i2s_allchannel == 1)
            play_channel_now = 0;
        else
            play_channel_now = play_channel % i2s_allchannel;
    } else
        play_channel_now = play_channel;

    for (i=0 ; i<sample_num ; i++) {
        if (sample_count == play_channel_now) {
            *buffPtr = i2s_sinus(tone_phase)>>2;
            //*buffPtr = 0xffff;
            tone_phase += tone_phase_ad;
            if (tone_phase > 65535)
                tone_phase -= 65535;
        } else {
            *buffPtr = 0;
        }
        sample_count ++;
        if (sample_count == i2s_allchannel)
            sample_count = 0;
        buffPtr++;
    }
}

VOID
I2SToneGens24bit
(
    IN  u32    sample_num,
    IN  s32   *buffPtr
)
{
    u32 i;

    tone_phase_ad = (1014 * 16777) >> 11;      //65535/8000 in Q11

    if (play_channel >= i2s_allchannel) {
        if (i2s_allchannel == 1)
            play_channel_now = 0;
        else
            play_channel_now = play_channel % i2s_allchannel;
    } else
        play_channel_now = play_channel;

    for (i=0 ; i<sample_num ; i++) {
        if (sample_count == play_channel_now) {
            *buffPtr = ((s32)(i2s_sinus(tone_phase))<<6) & 0x00ffffff;
            tone_phase += tone_phase_ad;
            if (tone_phase > 65535)
                tone_phase -= 65535;
        } else {
            *buffPtr = 0;
        }
        sample_count++;
        if (sample_count == i2s_allchannel)
            sample_count = 0;
        buffPtr++;
    }
}

VOID
I2SToneGens32bit
(
    IN  u32    sample_num,
    IN  s32   *buffPtr
)
{
    u32 i;

    tone_phase_ad = (1014 * 16777) >> 11;      //65535/8000 in Q11

    if (play_channel >= i2s_allchannel) {
        if (i2s_allchannel == 1)
            play_channel_now = 0;
        else
            play_channel_now = play_channel % i2s_allchannel;
    } else
        play_channel_now = play_channel;

    for (i=0 ; i<sample_num ; i++) {
        if (sample_count == play_channel_now) {
            *buffPtr = ((s32)(i2s_sinus(tone_phase))<<14);
            tone_phase += tone_phase_ad;
            if (tone_phase > 65535)
                tone_phase -= 65535;
        } else {
            *buffPtr = 0;
        }
        sample_count++;
        if (sample_count == i2s_allchannel)
            sample_count = 0;
        buffPtr++;
    }
}
#endif

