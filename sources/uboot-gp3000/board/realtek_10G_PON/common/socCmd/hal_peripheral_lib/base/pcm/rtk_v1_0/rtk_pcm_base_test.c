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
#include <asm/cache.h>
//#include "rtk_hal_pcm.h"
#include "section_config.h"
#include "g711.h"
#include "rtk_pcm_base_test.h"
#include "peripheral.h"

#ifndef ARCH_DMA_MINALIGN
#define ARCH_DMA_MINALIGN 64
#endif


#if defined(CONFIG_KERNEL) && !TASK_SCHEDULER_DISABLED
  #define PCM_FREE_RTOS_TASK_LOOPBACK_TEST 1
#endif

// PCM test config start
//#define SOC_TYPE_8195A 1
//#define SOC_TYPE_8198E 1
//#define SOC_TYPE_8196F 1
#define SOC_TYPE_8198F 1


#ifdef SOC_TYPE_8198E
  #define PCM_IRQ (32+24)
#elif SOC_TYPE_8196F
  #define PCM_IRQ (19)
#elif SOC_TYPE_8198F
  #define PCM_IRQ (108 + 32)
#endif

#define DATA_SIZE   2048

#define PCM_CH_NUM 16

//#define PKTSIZE		19 	// narrow band = 5 ms
//#define PKTSIZE		39 	// narrow band = 10 ms
//#define PKTSIZE		79	// narrow/wide band = 20/10 msec
#define PKTSIZE		159	// narrow/wide band = 40/20 msec

#define PCMPAGE_SIZE		((PKTSIZE+1)*4)

#define PCM_CODEC_ALC5621_OR_ALC5651 1

#ifdef PCM_CODEC_ALC5621_OR_ALC5651
//#define PCM_PLAY_TONE_TEST 1
#define PCM_MIC_LOOPBACK_SPEAKER_TEST 1
#endif

#ifndef PCM_CODEC_ALC5621_OR_ALC5651
//#define PCM_INTERNAL_LOOPBACK 1
#endif

#define PCM_TEST_TIME_SEC 30
//#define PCM_TEST_TIME_SEC 43200*2

//#define PCM_ENDIAN_SWAP 1

//select slave mode or master mode
//#define PCM_SLAVE_MODE 1

#if defined (SOC_TYPE_8198E)
#define LOCK_LX_CONTROL_REGISTER 0xF8000090
#define LOCK_LX_STATUS_REGISTER 0xF8000090
#define LOCK_LX1_BUS    (1<<3)
#define LOCK_LX1_OK     (1<<13)
#elif defined (SOC_TYPE_8196F)
#define LOCK_LX_CONTROL_REGISTER 0xB8000090
#define LOCK_LX_STATUS_REGISTER 0xB8000090
#define LOCK_LX1_BUS    (1<<3)
#define LOCK_LX1_OK     (1<<13)
#define VIR_TO_PHY(x) (x & (~0xa0000000))
#elif defined (SOC_TYPE_8198F)
//#define LOCK_LX_CONTROL_REGISTER 0xB8000090
//#define LOCK_LX_STATUS_REGISTER 0xB8000090
//#define LOCK_LX1_BUS    (1<<3)
//#define LOCK_LX1_OK     (1<<13)
#define VIR_TO_PHY(x) x

#endif

extern void *memset( void *s, int c, SIZE_T n );
extern void *memcpy( void *s1, const void *s2, SIZE_T n );

typedef struct _PCM_ADAPTER_ {
    PHAL_PCM_OP pHalPcmOp;
    PHAL_PCM_ADAPTER pHalPcmAdapter;
}PCM_ADAPTER, *PPCM_ADAPTER;

typedef struct _PCM_VERIFY_PARAMETER_ {
    //u8  GdmaAutoTest;
    u32  PcmTxMemoryTestType:1;
    u32  PcmRxMemoryTestType:1;
    u32  PcmWideband:1;
    u32  PcmLinearUA:2;//00: linear, 01:alaw, 10:mulaw, 11:mu+a mix law(c0:a, c1:mu)
    u32  PcmReserved3:1;
    u32  PcmIndex:1;
    u32  PcmTestMode:4;//play tone or internal loopback or external loopback
    u32  PcmReserved4:2;
    u32 PcmReserved5:2;
    u32 PcmPageSize:12;
    //u32 PcmReserved6:2;
    u32 PcmTimeSlot:5;
    u32 PcmVerifyLoop;
    //s16  *pTxPage; 
    //s16  *pRxPage;
    //s32  *pTxPage_24bit; 
    //s32  *pRxPage_24bit;
}PCM_VERIFY_PARAMETER, *PPCM_VERIFY_PARAMETER;


PCM_ADAPTER            TestPcmAdapte;
PCM_VERIFY_PARAMETER   PcmVerifyPere;
//SRAM_BD_DATA_SECTION
//u8  BDTX_PCM[DATA_SIZE];
//SRAM_BD_DATA_SECTION
//u8  BDRX_PCM[DATA_SIZE];

#if 0
SRAM_BF_DATA_SECTION
u8  BFTX_PCM[DATA_SIZE];
SRAM_BF_DATA_SECTION
u8  BFRX_PCM[DATA_SIZE];
#endif
HAL_PCM_OP HalPcmOp;
HAL_PCM_ADAPTER HalPcmAdapter;

SRAM_BF_DATA_SECTION
PPCM_VERI_PARA pPCMVeriAdp;

SRAM_BF_DATA_SECTION
volatile u32 PCMVeriGCnt;

SRAM_BF_DATA_SECTION
volatile u32 PCMVeriErrCnt;

SRAM_BF_DATA_SECTION
volatile u32 PCMVeriShift;

#define PCM_DATABUFSIZE (25*PCMPAGE_SIZE)
#if defined(SOC_TYPE_8195A)
#define DMA_FROM_SECTION SRAM_BF_DATA_SECTION
#define DMA_TO_SECTION SRAM_BF_DATA_SECTION
//SDRAM_DATA_SECTION, SRAM_BD_DATA_SECTION, SRAM_BF_DATA_SECTION
#else
#define DMA_FROM_SECTION
#define DMA_TO_SECTION
#define DMA_SELECT_SDRAM 1 //select sdram for test
#define DMA_RX_SRAM 0
//#define DMA_SELECT_SDRAM 0 //select sram for test
//#define PRINT_DATA
#endif

DMA_FROM_SECTION
u16 pcm_tx_buf[PCM_CH_NUM][(PCMPAGE_SIZE+66)&(~31)]__attribute__((aligned(64)));
DMA_TO_SECTION
u16 pcm_rx_buf[PCM_CH_NUM][(PCMPAGE_SIZE+66)&(~31)]__attribute__((aligned(64)));

struct pcm_pkt {
    s16 *payload;
};

static struct pcm_pkt pcm_tx[PCM_CH_NUM];
static struct pcm_pkt pcm_rx[PCM_CH_NUM];

#if defined(PCM_CODEC_ALC5621_OR_ALC5651) && defined(PCM_MIC_LOOPBACK_SPEAKER_TEST)
SRAM_BF_DATA_SECTION
static u8 pcm_databuf[PCM_DATABUFSIZE];
static u32 pcm_tx_read_index, pcm_rx_write_index;
#endif

static u32 pcm_txpage[2][16];
static u32 pcm_rxpage[2][16];

static s16 pcm_comp[PCM_CH_NUM][PCMPAGE_SIZE];


static u32 PCMChanTxPage[32] = {PcmCh0P0TOK, PcmCh0P1TOK, PcmCh1P0TOK, PcmCh1P1TOK, PcmCh2P0TOK, PcmCh2P1TOK, PcmCh3P0TOK, PcmCh3P1TOK,
                                PcmCh4P0TOK, PcmCh4P1TOK, PcmCh5P0TOK, PcmCh5P1TOK, PcmCh6P0TOK, PcmCh6P1TOK, PcmCh7P0TOK, PcmCh7P1TOK,
				PcmCh8P0TOK, PcmCh8P1TOK, PcmCh9P0TOK, PcmCh9P1TOK, PcmCh10P0TOK, PcmCh10P1TOK, PcmCh11P0TOK, PcmCh11P1TOK,
				PcmCh12P0TOK, PcmCh12P1TOK, PcmCh13P0TOK, PcmCh13P1TOK, PcmCh14P0TOK, PcmCh14P1TOK, PcmCh15P0TOK, PcmCh15P1TOK};
static u32 PCMChanRxPage[32] = {PcmCh0P0ROK, PcmCh0P1ROK, PcmCh1P0ROK, PcmCh1P1ROK, PcmCh2P0ROK, PcmCh2P1ROK, PcmCh3P0ROK, PcmCh3P1ROK,
                                PcmCh4P0ROK, PcmCh4P1ROK, PcmCh5P0ROK, PcmCh5P1ROK, PcmCh6P0ROK, PcmCh6P1ROK, PcmCh7P0ROK, PcmCh7P1ROK,
				PcmCh8P0ROK, PcmCh8P1ROK, PcmCh9P0ROK, PcmCh9P1ROK, PcmCh10P0ROK, PcmCh10P1ROK, PcmCh11P0ROK, PcmCh11P1ROK,
				PcmCh12P0ROK, PcmCh12P1ROK, PcmCh13P0ROK, PcmCh13P1ROK, PcmCh14P0ROK, PcmCh14P1ROK, PcmCh15P0ROK, PcmCh15P1ROK};

u32 pcm_isr_cnt;
u32 pcm_err_cnt;

typedef struct ts_reg_entry {
	unsigned int	pastar03;
	unsigned int	pastar47;
	unsigned int	pastar811;
	unsigned int	pastar1215;
} TS_REG_ENTRY_T;

TS_REG_ENTRY_T ts_tbl[] = {
#if 1
	{0x00020406,	0x080A0C0E, 0x10121416, 0X181A1C1E}, //linear
	{0x06080A0C,	0X0E101214, 0x16181A1C, 0x1E000204},
	{0x0C0E1012,	0X1416181A, 0x1C1E0002, 0x0406080A},
	{0X10121416,	0X181A1C1E, 0x00020406, 0x080A0C0E},
	{0X1E0E1012,	0X02040C0A, 0x00060814, 0x16181a1c},
	{0X021E041C,	0X06160818, 0x1a141210, 0x0e000c0a},
	{0X1C120800,	0X041E0A0C, 0x16021810, 0x140e1a06}, 

#endif
#if 1
	{0x00010203,	0x04050607, 0x08090a0b, 0x0c0d0e0f}, //a/u-Law
	{0x08090A0B,	0x0C0D0E0F, 0x10111213, 0x14151617},
	{0x10111213,	0x14151617, 0x18191a1b, 0x1c1d1e1f},
	{0x18191A1B,	0x1C1D1E1F, 0x00010203, 0x04050607},
	{0x07060504,	0x03020100, 0x0b0a0908, 0x0f0e0d0c},
	{0x1F1E1D1C,	0x1B1A1918, 0x17161514, 0x13121110},
	{0x03110919,	0x001F020C, 0x13011808, 0x100f121c},
	{0x11120F0A,	0x0C100E0D, 0x01021f1a, 0x1c001e1d},
	{0x1E001B09,	0x1907111C, 0x0e100b18, 0x0817010c},
	{0x07130B1F,	0x0F031519, 0x17021b0e, 0x1e120509},
#endif
#if 0
	{0x00010203,	0x04050607}, //u-law
	{0x08090A0B,	0x0C0D0E0F},
	{0x10111213,	0x14151617},
	{0x18191A1B,	0x1C1D1E1F},
	{0x07060504,	0x03020100},
	{0x1F1E1D1C,	0x1B1A1918},
	{0x03110919,	0x001F020C},
	{0x11120F0A,	0x0C100E0D},
	{0x1E001B09,	0x1907111C},
	{0x07130B1F,	0x0F031519}
#endif
};

typedef struct wts_reg_entry {
	unsigned int	pastar03;
	unsigned int	paswtar03;
	unsigned int	pastar47;
	unsigned int	paswtar47;
} WTS_REG_ENTRY_T;

WTS_REG_ENTRY_T wts_tbl[] = {
	{0x00020406,	0x10121416, 0x080a0c0e, 0X181A1C1E}, //linear
	{0x0004080c,	0X02060a0e, 0x1014181c, 0x12161a1e},
	{0x0e020a08,	0X1e121a18, 0x000c0604, 0x101c1614},
	{0x00010203,	0X04050607, 0x08090a0b, 0x0c0d0e0f}, // mu,alaw
	{0x08090a0b,	0X0c0d0e0f, 0x10111213, 0x14151617},
	{0x18191a1b,	0X1c1d1e1f, 0x00010203, 0x04050607},
	{0x01020304,	0X11121314, 0x090a0b0c, 0x191a1b1c},
};


#ifdef PCM_PLAY_TONE_TEST
static s32 tone_phase = 0;
static s32 tone_phase_ad;
static s32 play_channel;
static s32 play_channel_now;
static s32 sample_count;

/*
 * sine wave 500hz matlab command (sample rate 8000hz, 500/8000=1/16)
 * 16bit (max 32767 0dB)
 * [32767*sin(2*pi*0/16) 32767*sin(2*pi*1/16) 32767*sin(2*pi*2/16) 32767*sin(2*pi*3/16)
 * 32767*sin(2*pi*4/16) 32767*sin(2*pi*5/16) 32767*sin(2*pi*6/16) 32767*sin(2*pi*7/16)
 * 32767*sin(2*pi*8/16) 32767*sin(2*pi*9/16) 32767*sin(2*pi*10/16) 32767*sin(2*pi*11/16)
 * 32767*sin(2*pi*12/16) 32767*sin(2*pi*13/16) 32767*sin(2*pi*14/16) 32767*sin(2*pi*15/16)]
 *
 * 24bit (max 4194304 -12dB)
 * [4194304*sin(2*pi*0/16) 4194304*sin(2*pi*1/16) 4194304*sin(2*pi*2/16) 4194304*sin(2*pi*3/16)
 * 4194304*sin(2*pi*4/16) 4194304*sin(2*pi*5/16) 4194304*sin(2*pi*6/16) 4194304*sin(2*pi*7/16)
 * 4194304*sin(2*pi*8/16) 4194304*sin(2*pi*9/16) 4194304*sin(2*pi*10/16) 4194304*sin(2*pi*11/16)
 * 4194304*sin(2*pi*12/16) 4194304*sin(2*pi*13/16) 4194304*sin(2*pi*14/16) 4194304*sin(2*pi*15/16)]
 */

s16 pcm_vpat[16]={0, 12539, 23170, 30273, 32767, 30273, 23170, 12539,
                  0, -12539, -23170, -30273, -32767, -30273, -23170, -12539};

#define PCM_TONEGEN_TABLE 1 //select tone gen by table or function
#endif
extern int check_pcm_reg(int TestTimes);

VOID
PCMLoopBackIrqHandle
(
    IN VOID      *Data
);

VOID
print_pcm_regs
(
    IN u32      PcmIndex
)
{

    DBG_8195A_DRIVER("-------- PcmIndex = %d\n ctrl reg setting --------", PcmIndex);
    DBG_8195A_DRIVER("REG_PCM_CTL = %x\n", HAL_PCMX_READ32(PcmIndex, REG_PCM_CTL));
    DBG_8195A_DRIVER("REG_PCM_CHCNR03= %x\n", HAL_PCMX_READ32(PcmIndex, REG_PCM_CHCNR03));
    DBG_8195A_DRIVER("REG_PCM_CHCNR47= %x\n", HAL_PCMX_READ32(PcmIndex, REG_PCM_CHCNR47));
    DBG_8195A_DRIVER("REG_PCM_CHCNR811= %x\n", HAL_PCMX_READ32(PcmIndex, REG_PCM_CHCNR811));
    DBG_8195A_DRIVER("REG_PCM_CHCNR1215= %x\n\n", HAL_PCMX_READ32(PcmIndex, REG_PCM_CHCNR1215));

    DBG_8195A_DRIVER("------ slot setting ------\n");
    DBG_8195A_DRIVER("REG_PCM_TSR03= %x\n", HAL_PCMX_READ32(PcmIndex, REG_PCM_TSR03));
    DBG_8195A_DRIVER("REG_PCM_WTSR03= %x\n", HAL_PCMX_READ32(PcmIndex, REG_PCM_WTSR03));
    DBG_8195A_DRIVER("REG_PCM_TSR47= %x\n", HAL_PCMX_READ32(PcmIndex, REG_PCM_TSR47));
    DBG_8195A_DRIVER("REG_PCM_WTSR47= %x\n", HAL_PCMX_READ32(PcmIndex, REG_PCM_WTSR47));
    DBG_8195A_DRIVER("REG_PCM_TSR811= %x\n", HAL_PCMX_READ32(PcmIndex, REG_PCM_TSR811));
    DBG_8195A_DRIVER("REG_PCM_TSR1215= %x\n\n", HAL_PCMX_READ32(PcmIndex, REG_PCM_TSR1215));

    DBG_8195A_DRIVER("------ page size setting ------\n");
    DBG_8195A_DRIVER("REG_PCM_BSIZE03= %x\n", HAL_PCMX_READ32(PcmIndex, REG_PCM_BSIZE03));
    DBG_8195A_DRIVER("REG_PCM_BSIZE47= %x\n", HAL_PCMX_READ32(PcmIndex, REG_PCM_BSIZE47));
    DBG_8195A_DRIVER("REG_PCM_BSIZE811= %x\n", HAL_PCMX_READ32(PcmIndex, REG_PCM_BSIZE811));
    DBG_8195A_DRIVER("REG_PCM_BSIZE1215= %x\n\n", HAL_PCMX_READ32(PcmIndex, REG_PCM_BSIZE1215));

    DBG_8195A_DRIVER("------ buf addr setting ------\n");
    DBG_8195A_DRIVER("REG_PCM_CH0TXBSA= %x\n", HAL_PCMX_READ32(PcmIndex, REG_PCM_CH0TXBSA));
    DBG_8195A_DRIVER("REG_PCM_CH1TXBSA= %x\n", HAL_PCMX_READ32(PcmIndex, REG_PCM_CH1TXBSA));
    DBG_8195A_DRIVER("REG_PCM_CH2TXBSA= %x\n", HAL_PCMX_READ32(PcmIndex, REG_PCM_CH2TXBSA));
    DBG_8195A_DRIVER("REG_PCM_CH3TXBSA= %x\n", HAL_PCMX_READ32(PcmIndex, REG_PCM_CH3TXBSA));
    DBG_8195A_DRIVER("REG_PCM_CH0RXBSA= %x\n", HAL_PCMX_READ32(PcmIndex, REG_PCM_CH0RXBSA));
    DBG_8195A_DRIVER("REG_PCM_CH1RXBSA= %x\n", HAL_PCMX_READ32(PcmIndex, REG_PCM_CH1RXBSA));
    DBG_8195A_DRIVER("REG_PCM_CH2RXBSA= %x\n", HAL_PCMX_READ32(PcmIndex, REG_PCM_CH2RXBSA));
    DBG_8195A_DRIVER("REG_PCM_CH3RXBSA= %x\n", HAL_PCMX_READ32(PcmIndex, REG_PCM_CH3RXBSA));
    DBG_8195A_DRIVER("REG_PCM_CH4TXBSA= %x\n", HAL_PCMX_READ32(PcmIndex, REG_PCM_CH4TXBSA));
    DBG_8195A_DRIVER("REG_PCM_CH5TXBSA= %x\n", HAL_PCMX_READ32(PcmIndex, REG_PCM_CH5TXBSA));
    DBG_8195A_DRIVER("REG_PCM_CH6TXBSA= %x\n", HAL_PCMX_READ32(PcmIndex, REG_PCM_CH6TXBSA));
    DBG_8195A_DRIVER("REG_PCM_CH7TXBSA= %x\n", HAL_PCMX_READ32(PcmIndex, REG_PCM_CH7TXBSA));
    DBG_8195A_DRIVER("REG_PCM_CH4RXBSA= %x\n", HAL_PCMX_READ32(PcmIndex, REG_PCM_CH4RXBSA));
    DBG_8195A_DRIVER("REG_PCM_CH5RXBSA= %x\n", HAL_PCMX_READ32(PcmIndex, REG_PCM_CH5RXBSA));
    DBG_8195A_DRIVER("REG_PCM_CH6RXBSA= %x\n", HAL_PCMX_READ32(PcmIndex, REG_PCM_CH6RXBSA));
    DBG_8195A_DRIVER("REG_PCM_CH7RXBSA= %x\n", HAL_PCMX_READ32(PcmIndex, REG_PCM_CH7RXBSA));
    DBG_8195A_DRIVER("REG_PCM_CH8TXBSA= %x\n", HAL_PCMX_READ32(PcmIndex, REG_PCM_CH8TXBSA));
    DBG_8195A_DRIVER("REG_PCM_CH9TXBSA= %x\n", HAL_PCMX_READ32(PcmIndex, REG_PCM_CH9TXBSA));
    DBG_8195A_DRIVER("REG_PCM_CH10TXBSA= %x\n", HAL_PCMX_READ32(PcmIndex, REG_PCM_CH10TXBSA));
    DBG_8195A_DRIVER("REG_PCM_CH11TXBSA= %x\n", HAL_PCMX_READ32(PcmIndex, REG_PCM_CH11TXBSA));
    DBG_8195A_DRIVER("REG_PCM_CH8RXBSA= %x\n", HAL_PCMX_READ32(PcmIndex, REG_PCM_CH8RXBSA));
    DBG_8195A_DRIVER("REG_PCM_CH9RXBSA= %x\n", HAL_PCMX_READ32(PcmIndex, REG_PCM_CH9RXBSA));
    DBG_8195A_DRIVER("REG_PCM_CH10RXBSA= %x\n", HAL_PCMX_READ32(PcmIndex, REG_PCM_CH10RXBSA));
    DBG_8195A_DRIVER("REG_PCM_CH11RXBSA= %x\n", HAL_PCMX_READ32(PcmIndex, REG_PCM_CH11RXBSA));
    DBG_8195A_DRIVER("REG_PCM_CH12TXBSA= %x\n", HAL_PCMX_READ32(PcmIndex, REG_PCM_CH12TXBSA));
    DBG_8195A_DRIVER("REG_PCM_CH13TXBSA= %x\n", HAL_PCMX_READ32(PcmIndex, REG_PCM_CH13TXBSA));
    DBG_8195A_DRIVER("REG_PCM_CH14TXBSA= %x\n", HAL_PCMX_READ32(PcmIndex, REG_PCM_CH14TXBSA));
    DBG_8195A_DRIVER("REG_PCM_CH15TXBSA= %x\n", HAL_PCMX_READ32(PcmIndex, REG_PCM_CH15TXBSA));
    DBG_8195A_DRIVER("REG_PCM_CH12RXBSA= %x\n", HAL_PCMX_READ32(PcmIndex, REG_PCM_CH12RXBSA));
    DBG_8195A_DRIVER("REG_PCM_CH13RXBSA= %x\n", HAL_PCMX_READ32(PcmIndex, REG_PCM_CH13RXBSA));
    DBG_8195A_DRIVER("REG_PCM_CH14RXBSA= %x\n", HAL_PCMX_READ32(PcmIndex, REG_PCM_CH14RXBSA));
    DBG_8195A_DRIVER("REG_PCM_CH15RXBSA= %x\n\n", HAL_PCMX_READ32(PcmIndex, REG_PCM_CH15RXBSA));

    DBG_8195A_DRIVER("------ IMR/ISR setting ------\n");
    DBG_8195A_DRIVER("REG_PCM_IMR03= %x\n", HAL_PCMX_READ32(PcmIndex, REG_PCM_IMR03));
    DBG_8195A_DRIVER("REG_PCM_IMR47= %x\n", HAL_PCMX_READ32(PcmIndex, REG_PCM_IMR47));
    DBG_8195A_DRIVER("REG_PCM_IMR811= %x\n", HAL_PCMX_READ32(PcmIndex, REG_PCM_IMR811));
    DBG_8195A_DRIVER("REG_PCM_IMR1215= %x\n", HAL_PCMX_READ32(PcmIndex, REG_PCM_IMR1215));
    DBG_8195A_DRIVER("REG_PCM_ISR03= %x\n", HAL_PCMX_READ32(PcmIndex, REG_PCM_ISR03));
    DBG_8195A_DRIVER("REG_PCM_ISR47= %x\n", HAL_PCMX_READ32(PcmIndex, REG_PCM_ISR47));
    DBG_8195A_DRIVER("REG_PCM_ISR811= %x\n", HAL_PCMX_READ32(PcmIndex, REG_PCM_ISR811));
    DBG_8195A_DRIVER("REG_PCM_ISR1215= %x\n\n", HAL_PCMX_READ32(PcmIndex, REG_PCM_ISR1215));


    DBG_8195A_DRIVER("------ PCM Interrupt Mapping setting ------\n");
    DBG_8195A_DRIVER("REG_PCM_INTMAP= %x\n", HAL_PCMX_READ32(PcmIndex, REG_PCM_INTMAP));

    DBG_8195A_DRIVER("------ PCM Rx Buffer data overwrite indicated ------\n");
    DBG_8195A_DRIVER("REG_PCM_RX_BUFOW= %x\n", HAL_PCMX_READ32(PcmIndex, REG_PCM_RX_BUFOW));
}

SRAM_BF_DATA_SECTION
short mulaw2lineartab[]=
{   -32124,    -31100,    -30076,    -29052,    -28028,    -27004,    -25980,    -24956, 
   -23932,    -22908,    -21884,    -20860,    -19836,    -18812,    -17788,    -16764, 
   -15996,    -15484,    -14972,    -14460,    -13948,    -13436,    -12924,    -12412, 
   -11900,    -11388,    -10876,    -10364,     -9852,     -9340,     -8828,     -8316, 
    -7932,     -7676,     -7420,     -7164,     -6908,     -6652,     -6396,     -6140, 
    -5884,     -5628,     -5372,     -5116,     -4860,     -4604,     -4348,     -4092, 
    -3900,     -3772,     -3644,     -3516,     -3388,     -3260,     -3132,     -3004, 
    -2876,     -2748,     -2620,     -2492,     -2364,     -2236,     -2108,     -1980, 
    -1884,     -1820,     -1756,     -1692,     -1628,     -1564,     -1500,     -1436, 
    -1372,     -1308,     -1244,     -1180,     -1116,     -1052,      -988,      -924, 
     -876,      -844,      -812,      -780,      -748,      -716,      -684,      -652, 
     -620,      -588,      -556,      -524,      -492,      -460,      -428,      -396, 
     -372,      -356,      -340,      -324,      -308,      -292,      -276,      -260, 
     -244,      -228,      -212,      -196,      -180,      -164,      -148,      -132, 
     -120,      -112,      -104,       -96,       -88,       -80,       -72,       -64, 
      -56,       -48,       -40,       -32,       -24,       -16,        -8,         0, 
    32124,     31100,     30076,     29052,     28028,     27004,     25980,     24956,
    23932,     22908,     21884,     20860,     19836,     18812,     17788,     16764, 
    15996,     15484,     14972,     14460,     13948,     13436,     12924,     12412, 
    11900,     11388,     10876,     10364,      9852,      9340,      8828,      8316, 
     7932,      7676,      7420,      7164,      6908,      6652,      6396,      6140, 
     5884,      5628,      5372,      5116,      4860,      4604,      4348,      4092, 
     3900,      3772,      3644,      3516,      3388,      3260,      3132,      3004, 
     2876,      2748,      2620,      2492,      2364,      2236,      2108,      1980, 
     1884,      1820,      1756,      1692,      1628,      1564,      1500,      1436, 
     1372,      1308,      1244,      1180,      1116,      1052,       988,       924, 
      876,       844,       812,       780,       748,       716,       684,       652, 
      620,       588,       556,       524,       492,       460,       428,       396, 
      372,       356,       340,       324,       308,       292,       276,       260, 
      244,       228,       212,       196,       180,       164,       148,       132, 
      120,       112,       104,        96,        88,        80,        72,        64, 
       56,        48,        40,        32,        24,        16,         8,         0};

SRAM_BF_DATA_SECTION
short alaw2lineartab[]=
{    -5504,     -5248,     -6016,     -5760,     -4480,     -4224,     -4992,     -4736, 
    -7552,    -7296,    -8064,     -7808,     -6528,     -6272,     -7040,     -6784, 
    -2752,     -2624,     -3008,     -2880,     -2240,     -2112,     -2496,     -2368, 
    -3776,     -3648,     -4032,     -3904,     -3264,     -3136,     -3520,     -3392, 
   -22016,    -20992,    -24064,    -23040,    -17920,    -16896,    -19968,    -18944, 
   -30208,    -29184,    -32256,    -31232,    -26112,    -25088,    -28160,    -27136, 
   -11008,    -10496,    -12032,    -11520,     -8960,     -8448,     -9984,     -9472, 
   -15104,    -14592,    -16128,    -15616,    -13056,    -12544,    -14080,    -13568, 
     -344,      -328,      -376,      -360,      -280,      -264,      -312,      -296, 
     -472,      -456,      -504,      -488,      -408,      -392,      -440,      -424, 
      -88,       -72,      -120,      -104,       -24,        -8,       -56,       -40, 
     -216,      -200,      -248,      -232,      -152,      -136,      -184,      -168, 
    -1376,     -1312,     -1504,     -1440,     -1120,     -1056,     -1248,     -1184, 
    -1888,     -1824,     -2016,     -1952,     -1632,     -1568,     -1760,     -1696, 
     -688,      -656,      -752,      -720,      -560,      -528,      -624,      -592, 
     -944,      -912,     -1008,      -976,      -816,      -784,      -880,      -848, 
     5504,      5248,      6016,      5760,      4480,      4224,      4992,      4736, 
     7552,      7296,      8064,      7808,      6528,      6272,      7040,      6784, 
     2752,      2624,      3008,      2880,      2240,      2112,      2496,      2368, 
     3776,      3648,      4032,      3904,      3264,      3136,      3520,      3392, 
    22016,     20992,     24064,     23040,     17920,     16896,     19968,     18944, 
    30208,     29184,     32256,     31232,     26112,     25088,     28160,     27136, 
    11008,     10496,     12032,     11520,      8960,      8448,      9984,      9472, 
    15104,     14592,     16128,     15616,     13056,     12544,     14080,     13568, 
      344,       328,       376,       360,       280,       264,       312,       296, 
      472,       456,       504,       488,       408,       392,       440,       424, 
       88,        72,       120,       104,        24,         8,        56,        40, 
      216,       200,       248,       232,       152,       136,       184,       168, 
     1376,      1312,      1504,      1440,      1120,      1056,      1248,      1184, 
     1888,      1824,      2016,      1952,      1632,      1568,      1760,      1696, 
      688,       656,       752,       720,       560,       528,       624,       592, 
      944,       912,      1008,       976,       816,       784,       880,       848};


VOID
init_pcm_ch
(
    IN VOID      *Data,
    s32          chid,
    u32          mode
)
{
    PPCM_VERIFY_PARAMETER pPcmVerifyPara = (PPCM_VERIFY_PARAMETER)Data;
    u8 PcmIndex = pPcmVerifyPara->PcmIndex;
    u32 linear_test=1;
    u32 mu_alaw=0;//0:a-law 1:mu-law, 0xffff:all mulaw, 0x0: all alaw, 0xaaaa: mu+alaw
    s32 i, chid_tmp;
#if DMA_SELECT_SDRAM
    pcm_tx[chid].payload = (s16 *)&pcm_tx_buf[chid][0];
	#if DMA_RX_SRAM	
    pcm_rx[chid].payload = (s16 *)(0xbfe00000+(0<<10)+(chid<<10));
	#else
    pcm_rx[chid].payload = (s16 *)&pcm_rx_buf[chid][0];
	#endif
#else
    #if defined (SOC_TYPE_8198E)
        pcm_tx[chid].payload = (s16 *)(0xd0000000+(0<<10)+(chid<<11));
        pcm_rx[chid].payload = (s16 *)(0xd0000000+(64<<10)+(chid<<11));
    #else
        pcm_tx[chid].payload = (s16 *)(0xc0000000+(0<<10)+(chid<<9));
        pcm_rx[chid].payload = (s16 *)(0xc0000000+(8<<10)+(chid<<9));
    #endif
#endif

    if (pPcmVerifyPara->PcmLinearUA == 0) {
        linear_test=1;
    } else if (pPcmVerifyPara->PcmLinearUA == 1) {
        linear_test=0;
        mu_alaw=0;
    } else if (pPcmVerifyPara->PcmLinearUA == 2) {
        linear_test=0;
        mu_alaw=0xffff;
    } else {
        linear_test=0;
        mu_alaw=0xaaaa;
    }

//DiagPrintf("x%d\n", __LINE__);
    if (chid >= 8)
        chid_tmp = chid - 8;
    else
        chid_tmp = chid;

//DiagPrintf("x%d\n", __LINE__);
#ifndef PCM_CODEC_ALC5621_OR_ALC5651
    for (i=0 ; i<(pPcmVerifyPara->PcmPageSize+1)*4 ; i++) {
#ifdef PCM_INTERNAL_LOOPBACK 
        short raw_pcm = (((chid_tmp*100)+i+101)*30-67 );

#else
        short raw_pcm = (((chid_tmp*100)+i+101)*30-67 ) & 0xfffe;
        if (!linear_test) {
            if ((mu_alaw & (1<<chid)) == 0) {
                raw_pcm = alaw2lineartab[((i<<1)+chid*10)&0xff];
            } else {
                raw_pcm = mulaw2lineartab[((i<<1)+chid*10)&0xff];
            }
            if (i%2 == 1)
                raw_pcm ^= 0x8000;
        }
#endif
        if (i%2 == 1)
            raw_pcm ^= 0x8000;
        short back;
        unsigned char mid;
        if (linear_test) {
            back = raw_pcm;
        } else if ((mu_alaw & (1<<chid)) == 0) { // A-law
            mid = linear2alaw(raw_pcm);
            back = alaw2linear(mid);
        } else {
            mid = linear2ulaw(raw_pcm);
            back = ulaw2linear(mid);
        }
        pcm_tx[chid].payload[i] = raw_pcm;
        pcm_comp[chid][i]=back;
    }
    memset( pcm_rx[chid].payload, 0, (pPcmVerifyPara->PcmPageSize+1)*4*2);
#else
    memset( pcm_tx[chid].payload, 0, (pPcmVerifyPara->PcmPageSize+1)*4*2);
  #if defined(PCM_PLAY_TONE_TEST)
    if (chid == play_channel_now) {
    #if PCM_TONEGEN_TABLE
        for (i=0 ; i<((pPcmVerifyPara->PcmPageSize+1)*4); i++) {
            pcm_tx[chid].payload[i] = pcm_vpat[tone_phase]/8;
            tone_phase++;
            if (tone_phase == 16)
                tone_phase = 0;
        }
    #else
        PCMToneGens((pPcmVerifyPara->PcmPageSize+1)*(2)*2, &pcm_tx[chid].payload[0]);
    #endif
    }
  #endif
#endif
#ifdef PCM_CODEC_ALC5621_OR_ALC5651
#define PCM_SHOW_TX_DATA_CH 3
#else
#define PCM_SHOW_TX_DATA_CH 16
#endif

#ifdef PRINT_DATA
  if (chid < PCM_SHOW_TX_DATA_CH) {
    DiagPrintf("\nchid = %d\n", chid);
    DiagPrintf("tx page: \n");
    for (i=0 ; i<(pPcmVerifyPara->PcmPageSize+1)*4 ; i++) {
        if ((i%8) == 7)
            DiagPrintf(" %x\n", pcm_tx[chid].payload[i]);
        else
            DiagPrintf(" %x ", pcm_tx[chid].payload[i]);
    }
  }

#ifndef PCM_CODEC_ALC5621_OR_ALC5651
    DiagPrintf("comp:\n");
    for (i=0 ; i<(pPcmVerifyPara->PcmPageSize+1)*4 ; i++) {
        if ((i%8) == 7)
            DiagPrintf(" %x\n", pcm_comp[chid][i]);
        else
            DiagPrintf(" %x ", pcm_comp[chid][i]);
    }
#endif
#endif // PRINT_DATA
//DiagPrintf("x%d\n", __LINE__);
    //memset( pcm_tx[chid].payload, 0, 2*PCMPAGE_SIZE);

    if (chid < 4) {
        // allocate buffer address
        HAL_PCMX_WRITE32(PcmIndex, REG_PCM_CH0TXBSA+REG_PCM_TRXBSA_OFF*chid,VIR_TO_PHY((u32)pcm_tx[chid].payload));
        HAL_PCMX_WRITE32(PcmIndex, REG_PCM_CH0RXBSA+REG_PCM_TRXBSA_OFF*chid,VIR_TO_PHY((u32)pcm_rx[chid].payload));
        // set Tx Rx owned by PCM controller
        HAL_PCMX_WRITE32(PcmIndex, REG_PCM_CH0TXBSA+REG_PCM_TRXBSA_OFF*chid, HAL_PCMX_READ32(PcmIndex, REG_PCM_CH0TXBSA+REG_PCM_TRXBSA_OFF*chid)| 0x3);
#ifndef PCM_PLAY_TONE_TEST
        HAL_PCMX_WRITE32(PcmIndex, REG_PCM_CH0RXBSA+REG_PCM_TRXBSA_OFF*chid, HAL_PCMX_READ32(PcmIndex, REG_PCM_CH0RXBSA+REG_PCM_TRXBSA_OFF*chid)| 0x3);
#endif
        DBG_8195A_DRIVER("enable PCM-%d Channel-%d interrupt\n", PcmIndex, chid);
        HAL_PCMX_WRITE32(PcmIndex, REG_PCM_ISR03, 0xFF000000 >> (chid*8)); //reset pcm isr03
#ifndef PCM_PLAY_TONE_TEST
        HAL_PCMX_WRITE32(PcmIndex, REG_PCM_IMR03, HAL_PCMX_READ32(PcmIndex, REG_PCM_IMR03) | 0x30000000 >> (chid*8));
#else
        HAL_PCMX_WRITE32(PcmIndex, REG_PCM_IMR03, HAL_PCMX_READ32(PcmIndex, REG_PCM_IMR03) | 0xC0000000 >> (chid*8));
#endif
        HAL_PCMX_WRITE32(PcmIndex, REG_PCM_BSIZE03, HAL_PCMX_READ32(PcmIndex, REG_PCM_BSIZE03) | pPcmVerifyPara->PcmPageSize << (24 - (chid*8)));  // set page size
    } else if (chid < 8) {
        // allocate buffer address
        HAL_PCMX_WRITE32(PcmIndex, REG_PCM_CH4TXBSA+REG_PCM_TRXBSA_OFF*(chid&3),VIR_TO_PHY( (u32) pcm_tx[chid].payload));
        HAL_PCMX_WRITE32(PcmIndex, REG_PCM_CH4RXBSA+REG_PCM_TRXBSA_OFF*(chid&3),VIR_TO_PHY( (u32) pcm_rx[chid].payload));
        // set Tx Rx owned by PCM controller
        HAL_PCMX_WRITE32(PcmIndex, REG_PCM_CH4TXBSA+REG_PCM_TRXBSA_OFF*(chid&3), HAL_PCMX_READ32(PcmIndex, REG_PCM_CH4TXBSA+REG_PCM_TRXBSA_OFF*(chid&3))| 0x3);
#ifndef PCM_PLAY_TONE_TEST
        HAL_PCMX_WRITE32(PcmIndex, REG_PCM_CH4RXBSA+REG_PCM_TRXBSA_OFF*(chid&3), HAL_PCMX_READ32(PcmIndex, REG_PCM_CH4RXBSA+REG_PCM_TRXBSA_OFF*(chid&3))| 0x3);
#endif
        DBG_8195A_DRIVER("enable PCM-%d Channel-%d interrupt\n", PcmIndex, chid);
        HAL_PCMX_WRITE32(PcmIndex, REG_PCM_ISR47, 0xFF000000 >> ((chid&3)*8)); //reset pcm isr47
#ifndef PCM_PLAY_TONE_TEST
        HAL_PCMX_WRITE32(PcmIndex, REG_PCM_IMR47, HAL_PCMX_READ32(PcmIndex, REG_PCM_IMR47) | 0x30000000 >> ((chid&3)*8));
#else
        HAL_PCMX_WRITE32(PcmIndex, REG_PCM_IMR47, HAL_PCMX_READ32(PcmIndex, REG_PCM_IMR47) | 0xC0000000 >> ((chid&3)*8));
#endif
        HAL_PCMX_WRITE32(PcmIndex, REG_PCM_BSIZE47, HAL_PCMX_READ32(PcmIndex, REG_PCM_BSIZE47) | pPcmVerifyPara->PcmPageSize << (24 - ((chid&3)*8)));  // set page size
    } else if (chid < 12) {
        // allocate buffer address
        HAL_PCMX_WRITE32(PcmIndex, REG_PCM_CH8TXBSA+REG_PCM_TRXBSA_OFF*(chid&3),VIR_TO_PHY( (u32) pcm_tx[chid].payload));
        HAL_PCMX_WRITE32(PcmIndex, REG_PCM_CH8RXBSA+REG_PCM_TRXBSA_OFF*(chid&3),VIR_TO_PHY( (u32) pcm_rx[chid].payload));
        // set Tx Rx owned by PCM controller
        HAL_PCMX_WRITE32(PcmIndex, REG_PCM_CH8TXBSA+REG_PCM_TRXBSA_OFF*(chid&3), HAL_PCMX_READ32(PcmIndex, REG_PCM_CH8TXBSA+REG_PCM_TRXBSA_OFF*(chid&3))| 0x3);
#ifndef PCM_PLAY_TONE_TEST
        HAL_PCMX_WRITE32(PcmIndex, REG_PCM_CH8RXBSA+REG_PCM_TRXBSA_OFF*(chid&3), HAL_PCMX_READ32(PcmIndex, REG_PCM_CH8RXBSA+REG_PCM_TRXBSA_OFF*(chid&3))| 0x3);
#endif
        DBG_8195A_DRIVER("enable PCM-%d Channel-%d interrupt\n", PcmIndex, chid);
        HAL_PCMX_WRITE32(PcmIndex, REG_PCM_ISR811, 0xFF000000 >> ((chid&3)*8)); //reset pcm isr811
#ifndef PCM_PLAY_TONE_TEST
        HAL_PCMX_WRITE32(PcmIndex, REG_PCM_IMR811, HAL_PCMX_READ32(PcmIndex, REG_PCM_IMR811) | 0x30000000 >> ((chid&3)*8));
#else
        HAL_PCMX_WRITE32(PcmIndex, REG_PCM_IMR811, HAL_PCMX_READ32(PcmIndex, REG_PCM_IMR811) | 0xC0000000 >> ((chid&3)*8));
#endif
        HAL_PCMX_WRITE32(PcmIndex, REG_PCM_BSIZE811, HAL_PCMX_READ32(PcmIndex, REG_PCM_BSIZE811) | pPcmVerifyPara->PcmPageSize << (24 - ((chid&3)*8)));  // set page size
    } else if (chid < 16) {
        // allocate buffer address
        HAL_PCMX_WRITE32(PcmIndex, REG_PCM_CH12TXBSA+REG_PCM_TRXBSA_OFF*(chid&3),VIR_TO_PHY( (u32) pcm_tx[chid].payload));
        HAL_PCMX_WRITE32(PcmIndex, REG_PCM_CH12RXBSA+REG_PCM_TRXBSA_OFF*(chid&3),VIR_TO_PHY( (u32) pcm_rx[chid].payload));
        // set Tx Rx owned by PCM controller
        HAL_PCMX_WRITE32(PcmIndex, REG_PCM_CH12TXBSA+REG_PCM_TRXBSA_OFF*(chid&3), HAL_PCMX_READ32(PcmIndex, REG_PCM_CH12TXBSA+REG_PCM_TRXBSA_OFF*(chid&3))| 0x3);
#ifndef PCM_PLAY_TONE_TEST
        HAL_PCMX_WRITE32(PcmIndex, REG_PCM_CH12RXBSA+REG_PCM_TRXBSA_OFF*(chid&3), HAL_PCMX_READ32(PcmIndex, REG_PCM_CH12RXBSA+REG_PCM_TRXBSA_OFF*(chid&3))| 0x3);
#endif
        DBG_8195A_DRIVER("enable PCM-%d Channel-%d interrupt\n", PcmIndex, chid);
        HAL_PCMX_WRITE32(PcmIndex, REG_PCM_ISR1215, 0xFF000000 >> ((chid&3)*8)); //reset pcm isr1215
#ifndef PCM_PLAY_TONE_TEST
        HAL_PCMX_WRITE32(PcmIndex, REG_PCM_IMR1215, HAL_PCMX_READ32(PcmIndex, REG_PCM_IMR1215) | 0x30000000 >> ((chid&3)*8));
#else
        HAL_PCMX_WRITE32(PcmIndex, REG_PCM_IMR1215, HAL_PCMX_READ32(PcmIndex, REG_PCM_IMR1215) | 0xC0000000 >> ((chid&3)*8));
#endif
        HAL_PCMX_WRITE32(PcmIndex, REG_PCM_BSIZE1215, HAL_PCMX_READ32(PcmIndex, REG_PCM_BSIZE1215) | pPcmVerifyPara->PcmPageSize << (24 - ((chid&3)*8)));  // set page size
    }

#ifdef PCM_MIC_LOOPBACK_SPEAKER_TEST
    memset( pcm_databuf, 0, PCM_DATABUFSIZE);
    pcm_rx_write_index = 0;
    pcm_tx_read_index = (pPcmVerifyPara->PcmPageSize+1)*4;
#endif

    pcm_txpage[PcmIndex][chid]=0;
    pcm_rxpage[PcmIndex][chid]=0;

    flush_dcache_range(((unsigned long)pcm_tx[chid].payload)&(~(ARCH_DMA_MINALIGN-1)), (((unsigned long)pcm_tx[chid].payload)+(2*PCMPAGE_SIZE*2+32)*2)&(~(ARCH_DMA_MINALIGN-1)));
    invalidate_dcache_range(((unsigned long)pcm_tx[chid].payload)&(~(ARCH_DMA_MINALIGN-1)), (((unsigned long)pcm_tx[chid].payload)+(2*PCMPAGE_SIZE*2+32)*2)&(~(ARCH_DMA_MINALIGN-1)));
    flush_dcache_range(((unsigned long)pcm_rx[chid].payload)&(~(ARCH_DMA_MINALIGN-1)), (((unsigned long)pcm_rx[chid].payload)+(2*PCMPAGE_SIZE*2+32)*2)&(~(ARCH_DMA_MINALIGN-1)));
    invalidate_dcache_range(((unsigned long)pcm_rx[chid].payload)&(~(ARCH_DMA_MINALIGN-1)), (((unsigned long)pcm_rx[chid].payload)+(2*PCMPAGE_SIZE*2+32)*2)&(~(ARCH_DMA_MINALIGN-1)));

    pcm_isr_cnt=0;
    pcm_err_cnt=0;
//DiagPrintf("x%d\n", __LINE__);
}

VOID
init_pcm
(
    IN VOID      *Data
)
{
    PPCM_VERIFY_PARAMETER pPcmVerifyPara = (PPCM_VERIFY_PARAMETER)Data;
    u8 PcmIndex = pPcmVerifyPara->PcmIndex;
    u32 i;
    u32 timeslot_set=0;//0-6
    u32 pcm_ctl;
    u32 ua_mode=0;

    timeslot_set = pPcmVerifyPara->PcmTimeSlot;

#ifdef PCM_INTERNAL_LOOPBACK
    DBG_8195A_DRIVER("PCM in internal loopback\n");
  #ifdef PCM_ENDIAN_SWAP
    pcm_ctl=BIT_CTLX_LOOP_BACK(1)|BIT_CTLX_ENDIAN_SWAP(1);//internal loopback
    DBG_8195A_DRIVER("PCM endian swap enable\n");
  #else
    pcm_ctl=BIT_CTLX_LOOP_BACK(1);
    DBG_8195A_DRIVER("PCM endian swap disable\n");
  #endif
#else
  #ifdef PCM_CODEC_ALC5621_OR_ALC5651
    #ifdef PCM_PLAY_TONE_TEST
    DBG_8195A_DRIVER("PCM in Play tone test\n");
    #endif
    #ifdef PCM_MIC_LOOPBACK_SPEAKER_TEST
    DBG_8195A_DRIVER("PCM in mic loopback speaker test\n");
    #endif
  #else
    DBG_8195A_DRIVER("PCM in external loopback\n");
  #endif
  #ifdef PCM_ENDIAN_SWAP
    pcm_ctl=BIT_CTLX_ENDIAN_SWAP(1);//external loopback
    DBG_8195A_DRIVER("PCM endian swap enable\n");
  #else
    pcm_ctl=0;
    DBG_8195A_DRIVER("PCM endian swap disable\n");
  #endif
#endif

#ifdef PCM_SLAVE_MODE
    pcm_ctl |= BIT_CTLX_SLAVE_SEL(1);
    DBG_8195A_DRIVER("PCM clock is slave mode\n");
#else
    DBG_8195A_DRIVER("PCM clock is master mode\n");
#endif
    if (pPcmVerifyPara->PcmLinearUA == 0) {
        DBG_8195A_DRIVER("PCM in linear mode\n");
        pcm_ctl |= BIT_CTLX_LINEARMODE(1);
    } else {
        DBG_8195A_DRIVER("PCM in compender mode\n");
    }
    if (pPcmVerifyPara->PcmLinearUA == 1) {
        ua_mode = BIT_(26) | BIT_(18) | BIT_(10) | BIT_(2);
    } else if (pPcmVerifyPara->PcmLinearUA == 2) {
        ua_mode = 0;
    } else if (pPcmVerifyPara->PcmLinearUA == 3) {
        ua_mode = BIT_(26) | BIT_(10);
    }

    if (pPcmVerifyPara->PcmWideband)
        ua_mode |= 0x08080808;
    //reset pcm.
    HAL_PCMX_WRITE32(PcmIndex, REG_PCM_CTL, (1<<12) | pcm_ctl );
    i=1000000;
    while (i) {
        i--;
        asm volatile ("nop\n\t");
    }
    HAL_PCMX_WRITE32(PcmIndex, REG_PCM_CTL, 0 | pcm_ctl);
    i=1000000;
    while (i) {
        i--;
        asm volatile ("nop\n\t");
    }
    HAL_PCMX_WRITE32(PcmIndex, REG_PCM_CTL, 1<<12 | pcm_ctl);
    i=1000000;
    while (i) {
        i--;
        asm volatile ("nop\n\t");
    }

#ifdef PCM_PLAY_TONE_TEST
    tone_phase = 0;
    //#ifdef PCM_TONEGEN_TABLE
    play_channel_now = play_channel++;
    if (play_channel == 2)
        play_channel=0;
  #ifdef PCM_SLAVE_MODE
    //play_channel=0;
  #endif
    //#endif
#endif

    for (i=0 ; i<PCM_CH_NUM ; i++) {
        init_pcm_ch( Data, i, 0);
    }

//DiagPrintf("x%d\n", __LINE__);
    HAL_PCMX_WRITE32(PcmIndex, REG_PCM_TSR03, ts_tbl[timeslot_set].pastar03);
    HAL_PCMX_WRITE32(PcmIndex, REG_PCM_TSR47, ts_tbl[timeslot_set].pastar47);
    HAL_PCMX_WRITE32(PcmIndex, REG_PCM_TSR811, ts_tbl[timeslot_set].pastar811);
    HAL_PCMX_WRITE32(PcmIndex, REG_PCM_TSR1215, ts_tbl[timeslot_set].pastar1215);
    if (pPcmVerifyPara->PcmWideband) {
        HAL_PCMX_WRITE32(PcmIndex, REG_PCM_TSR03, wts_tbl[timeslot_set].pastar03);
        HAL_PCMX_WRITE32(PcmIndex, REG_PCM_WTSR03, wts_tbl[timeslot_set].paswtar03);
        HAL_PCMX_WRITE32(PcmIndex, REG_PCM_TSR47, wts_tbl[timeslot_set].pastar47);
        HAL_PCMX_WRITE32(PcmIndex, REG_PCM_WTSR47, wts_tbl[timeslot_set].paswtar47);
    }

//DiagPrintf("x%d\n", __LINE__);
    print_pcm_regs(PcmIndex);

//DiagPrintf("x%d\n", __LINE__);
#if !defined(PCM_PLAY_TONE_TEST) 
    HAL_PCMX_WRITE32(PcmIndex, REG_PCM_CHCNR03, BIT_CHCNR03_CH0TE(1) | BIT_CHCNR03_CH0RE(1) | ua_mode
                                               |BIT_CHCNR03_CH1TE(1) | BIT_CHCNR03_CH1RE(1)
                                               |BIT_CHCNR03_CH2TE(1) | BIT_CHCNR03_CH2RE(1)
                                               |BIT_CHCNR03_CH3TE(1) | BIT_CHCNR03_CH3RE(1));
    HAL_PCMX_WRITE32(PcmIndex, REG_PCM_CHCNR47, BIT_CHCNR03_CH0TE(1) | BIT_CHCNR03_CH0RE(1) | ua_mode
                                               |BIT_CHCNR03_CH1TE(1) | BIT_CHCNR03_CH1RE(1)
                                               |BIT_CHCNR03_CH2TE(1) | BIT_CHCNR03_CH2RE(1)
                                               |BIT_CHCNR03_CH3TE(1) | BIT_CHCNR03_CH3RE(1));
    if (!pPcmVerifyPara->PcmWideband) {
        HAL_PCMX_WRITE32(PcmIndex, REG_PCM_CHCNR811, BIT_CHCNR03_CH0TE(1) | BIT_CHCNR03_CH0RE(1) | ua_mode
                                               |BIT_CHCNR03_CH1TE(1) | BIT_CHCNR03_CH1RE(1)
                                               |BIT_CHCNR03_CH2TE(1) | BIT_CHCNR03_CH2RE(1)
                                               |BIT_CHCNR03_CH3TE(1) | BIT_CHCNR03_CH3RE(1));
        HAL_PCMX_WRITE32(PcmIndex, REG_PCM_CHCNR1215, BIT_CHCNR03_CH0TE(1) | BIT_CHCNR03_CH0RE(1) | ua_mode
                                               |BIT_CHCNR03_CH1TE(1) | BIT_CHCNR03_CH1RE(1)
                                               |BIT_CHCNR03_CH2TE(1) | BIT_CHCNR03_CH2RE(1)
                                               |BIT_CHCNR03_CH3TE(1) | BIT_CHCNR03_CH3RE(1));
    }
#else
    HAL_PCMX_WRITE32(PcmIndex, REG_PCM_CHCNR03, BIT_CHCNR03_CH0TE(1) | BIT_CHCNR03_CH0RE(0) | ua_mode
                                               |BIT_CHCNR03_CH1TE(1) | BIT_CHCNR03_CH1RE(0)
                                               |BIT_CHCNR03_CH2TE(0) | BIT_CHCNR03_CH2RE(0)
                                               |BIT_CHCNR03_CH3TE(0) | BIT_CHCNR03_CH3RE(0));
#endif

//DiagPrintf("x%d\n", __LINE__);
}

VOID
PcmTestMode0(
    IN  VOID      *Data
)
{
    
    DBG_8195A_DRIVER("PCM in test mode 0\n");
    init_pcm(Data);

}
//#define PCM_LOOPBACK_SEL BIT14 //enable internal loopback
#define PCM_LOOPBACK_SEL 0
//#define SLAVE_SEL (BIT_(29)|BIT_(11)) // 0 or bit29
//#define SLAVE_SEL (BIT_(8)) // 0 or bit8
//#define SLAVE_SEL (BIT_(2)|BIT_(11)|BIT_(2)) // 0 or bit29
#define SLAVE_SEL (0) // 0 or bit8

#define TEST_SEL (PCM_LOOPBACK_SEL|SLAVE_SEL)

VOID
PcmTestMode3(
    IN  VOID      *Data
)
{
#if 1
	PPCM_VERIFY_PARAMETER pPcmVerifyPara = (PPCM_VERIFY_PARAMETER)Data;
	int* pPcmRxPtr=(int *)0x2001f000;
	int* pPcmTxPtr=(int *)0x2001e000;
	s16* pPcmTxPtr16=(s16 *)0x2001e000;
	s16* pPcmRxPtr16=(s16 *)0x2001f000;
	int i;

	DBG_8195A_DRIVER("test Pcm rx left/right channel, please short rx and ws\n");
	pPcmVerifyPara->PcmIndex = 1;
	// reset pcm + pcm tx rx
	HAL_PCMX_WRITE32(pPcmVerifyPara->PcmIndex, 0, BIT_(12)|SLAVE_SEL);
	HAL_PCMX_WRITE32(pPcmVerifyPara->PcmIndex, 0, 0|SLAVE_SEL);
        // stereo 16bit tx, endian swap
	HAL_PCMX_WRITE32(pPcmVerifyPara->PcmIndex, 0, BIT_(12)|SLAVE_SEL|0x20000);

	// tx/rx page addr + own bit/size - 16byte * 2 page * 8khz
	HAL_PCMX_WRITE32(pPcmVerifyPara->PcmIndex, REG_PCM_CH0TXBSA, 0x2001e000|0x3);
	HAL_PCMX_WRITE32(pPcmVerifyPara->PcmIndex, REG_PCM_CH0RXBSA, 0x2001f000|0x3);
  
	HAL_PCMX_WRITE32(pPcmVerifyPara->PcmIndex, REG_PCM_BSIZE03, 0x4f000000);//page size

	// tx/rx int 
	HAL_PCMX_WRITE32(pPcmVerifyPara->PcmIndex, REG_PCM_ISR03, 0xffffffff);
	HAL_PCMX_WRITE32(pPcmVerifyPara->PcmIndex, REG_PCM_IMR03, 0x30000000);


	//prepaer tx buffer
//	for(i=0 ; i<8 ; i++) {
//		pPcmTxPtr[2*i]=0;
//		pPcmTxPtr[2*i+1]=0xffffff00;
//	}
#if 0
  #if 0
		pPcmTxPtr16[0]=0x0000ffff;
		pPcmTxPtr16[1]=0x0000aa55;
		pPcmTxPtr16[2]=0x0000ffff;
		pPcmTxPtr16[3]=0x0000aa55;
		pPcmTxPtr16[4]=0x0000ffff;
		pPcmTxPtr16[5]=0x0000aa55;
		pPcmTxPtr16[6]=0x0000ffff;
		pPcmTxPtr16[7]=0x0000aa55;
		pPcmTxPtr16[8]=0x0000ffff;
		pPcmTxPtr16[9]=0x0000aa55;
		pPcmTxPtr16[10]=0x0000ffff;
		pPcmTxPtr16[11]=0x0000aa55;
		pPcmTxPtr16[12]=0x0000ffff;
		pPcmTxPtr16[13]=0x0000aa55;
	//	pPcmTxPtr16[14]=0x0000ffff;
	//	pPcmTxPtr16[15]=0x0000aa55;
		pPcmTxPtr16[14]=0x00000010;
		pPcmTxPtr16[15]=0x00000001;
  #else
	for(i=0 ; i<33 ; i++) {
		pPcmTxPtr16[2*i]=0x0001;
		pPcmTxPtr16[2*i+1]=0xfffe;
	}

  #endif
#else
   #if 0
		pPcmTxPtr[0]=0x0000000f;
		pPcmTxPtr[1]=0x000000ff;
		pPcmTxPtr[2]=0x00000fff;
		pPcmTxPtr[3]=0x0000ffff;
		pPcmTxPtr[4]=0x000fffff;
		pPcmTxPtr[5]=0x00ffffff;
		pPcmTxPtr[6]=0x00ffffff;
		pPcmTxPtr[7]=0x00ffffff;
		pPcmTxPtr[8]=0x00ffffff;
   #else
		pPcmTxPtr[0]=0x00000000;
		pPcmTxPtr[1]=0x00ffffff;
		pPcmTxPtr[2]=0x00000000;
		pPcmTxPtr[3]=0x00ffffff;
		pPcmTxPtr[4]=0x00000000;
		pPcmTxPtr[5]=0x00ffffff;
		pPcmTxPtr[6]=0x00000000;
		pPcmTxPtr[7]=0x00ffffff;
		pPcmTxPtr[8]=0x00000000;
		pPcmTxPtr[9]=0x00ffffff;
		pPcmTxPtr[10]=0x00000000;
		pPcmTxPtr[11]=0x00ffffff;
		pPcmTxPtr[12]=0x00000000;
		pPcmTxPtr[13]=0x00ffffff;
		pPcmTxPtr[14]=0x00000000;
		pPcmTxPtr[15]=0x00ffffff;
		pPcmTxPtr[16]=0x00000000;
		pPcmTxPtr[17]=0x00ffffff;
		pPcmTxPtr[18]=0x00000000;
		pPcmTxPtr[19]=0x00ffffff;
		pPcmTxPtr[20]=0x00000000;
		pPcmTxPtr[21]=0x00ffffff;
		pPcmTxPtr[22]=0x00000000;
		pPcmTxPtr[23]=0x00ffffff;
		pPcmTxPtr[24]=0x00000000;
		pPcmTxPtr[25]=0x00ffffff;
		pPcmTxPtr[26]=0x00000000;
		pPcmTxPtr[27]=0x00ffffff;
		pPcmTxPtr[28]=0x00000000;
		pPcmTxPtr[29]=0x00ffffff;
		pPcmTxPtr[30]=0x00000000;
		pPcmTxPtr[31]=0x00ffffff;
		pPcmTxPtr[32]=0x00000000;
		pPcmTxPtr[33]=0x00ffffff;

   #endif
#endif
	//clean rx buffer
	for(i=0 ; i<33 ; i++) {
		pPcmRxPtr16[i]=0x000a;
	}

	//enable
	HAL_PCMX_WRITE32(pPcmVerifyPara->PcmIndex, REG_PCM_CHCNR03, BIT_(25)|BIT_(24));

	//add delay;
	for(i=0 ; i<13276800 ; i++) {
		HAL_PCMX_READ32(pPcmVerifyPara->PcmIndex, 0);
	}
	
	//stop
	HAL_PCMX_WRITE32(pPcmVerifyPara->PcmIndex, REG_PCM_CHCNR03, 0);



#endif
}

#ifdef PCM_FREE_RTOS_TASK_LOOPBACK_TEST

VOID
pcm_set_tx_own_bit(
    IN  u32         PcmIndex,
    IN  u32         PcmChid,
    IN  u32         PcmPageIndex
);

VOID
pcm_set_rx_own_bit(
    IN  u32         PcmIndex,
    IN  u32         PcmChid,
    IN  u32         PcmPageIndex
);


VOID
PCMTaskIrqHandle
(
    IN  VOID    *Data
)
{
    PPCM_ADAPTER pPcmAdapter = (PPCM_ADAPTER) Data;
    PHAL_PCM_ADAPTER pHalPcmAdapter = pPcmAdapter->pHalPcmAdapter;
    u8 PcmIndex = pHalPcmAdapter->PcmIndex;
    u32 PcmPageSize;
    u32 i, j;
    u32 tx_isrpage, rx_isrpage;
    u32 pcm_cnt_1sec;

    u32 PcmIsr03, PcmIsr47, PcmIsr811, PcmIsr1215;

    pcm_cnt_1sec = 4000/(39+1);

    PcmIsr03 = HAL_PCMX_READ32(PcmIndex, REG_PCM_ISR03);
    PcmIsr47 = HAL_PCMX_READ32(PcmIndex, REG_PCM_ISR47);
    PcmIsr811 = HAL_PCMX_READ32(PcmIndex, REG_PCM_ISR811);
    PcmIsr1215 = HAL_PCMX_READ32(PcmIndex, REG_PCM_ISR1215);

    HAL_PCMX_WRITE32(PcmIndex, REG_PCM_ISR03, PcmIsr03);
    HAL_PCMX_WRITE32(PcmIndex, REG_PCM_ISR47, PcmIsr47);
    HAL_PCMX_WRITE32(PcmIndex, REG_PCM_ISR811, PcmIsr811);
    HAL_PCMX_WRITE32(PcmIndex, REG_PCM_ISR1215, PcmIsr1215);

    //PcmPageSize = (HAL_PCMX_READ32(PcmIndex, REG_PCM_BSIZE03)>>24) + 1;
    //            DiagPrintf("(PcmPageSize=%d)", PcmPageSize);
    PcmPageSize = 40;


    for (i=0 ; i<2 ; i++) { // page0, page1
        tx_isrpage = 2*0 + pcm_txpage[PcmIndex][0];
        if (PcmIsr03 & PCMChanTxPage[tx_isrpage] ) {
            for (j=0 ; j<PcmPageSize*2 ; j++) {
                pcm_tx[0].payload[j+pcm_txpage[PcmIndex][0]*40*2] = 0xaaaa+0x300*(pcm_isr_cnt+2)+j*2;
            }
            pcm_isr_cnt++;
            if ( (pcm_isr_cnt%pcm_cnt_1sec) == (pcm_cnt_1sec-1)) {
                //DiagPrintf("~");
            }
            if ( (pcm_isr_cnt%(60*pcm_cnt_1sec)) == ((60*pcm_cnt_1sec)-1)) {
                //DiagPrintf("\n");
            }

            PcmIsr03 &= ~PCMChanTxPage[tx_isrpage];
            pcm_set_tx_own_bit(PcmIndex, 0, pcm_txpage[PcmIndex][0]);
            pcm_txpage[PcmIndex][0] ^= 1;
        } // end of tx

        rx_isrpage = 2*0 + pcm_rxpage[PcmIndex][0];
        if (PcmIsr03 & PCMChanRxPage[rx_isrpage] ) {
            for (j=0 ; j<PcmPageSize*2 ; j++) {
                if ((PCMVeriGCnt == 0) && (j<4) ) {
                    if ((pcm_rx[0].payload[j+40*2*pcm_rxpage[PcmIndex][0]] == (short)0xaaaa)) {
                        PCMVeriShift = j;
                    }
                    if ((pcm_rx[0].payload[j+40*2*pcm_rxpage[PcmIndex][0]] == (short)0x0) ||
                         (pcm_rx[0].payload[j+40*2*pcm_rxpage[PcmIndex][0]] == (short)(0xaaaa+j*2-PCMVeriShift*2))) {
                        //pass
                    } else {
                        DiagPrintf("(%x,%d,%d,%d)", pcm_rx[0].payload[j+40*2*pcm_rxpage[PcmIndex][0]], PCMVeriGCnt, pcm_rxpage[PcmIndex][0], j);
                        PCMVeriErrCnt++;
                        break;
                    }
                } else {
                    if (j<PCMVeriShift) {
                        if ((pcm_rx[0].payload[j+40*2*pcm_rxpage[PcmIndex][0]]) == (short)(0xaaaa+(0x300*(PCMVeriGCnt-1))+40*2*2+j*2-PCMVeriShift*2)) {
                            ;//pass
                        } else {
                            DiagPrintf("(%x,%x,%d,%d,%d)", pcm_rx[0].payload[j+40*2*pcm_rxpage[PcmIndex][0]], (short)(0xaaaa+(0x300*(PCMVeriGCnt-1))+40*2*2+j*2-PCMVeriShift*2),PCMVeriGCnt, pcm_rxpage[PcmIndex][0], j);
                            PCMVeriErrCnt++;
                            break;
                        }
                    } else {
                        if ((pcm_rx[0].payload[j+40*2*pcm_rxpage[PcmIndex][0]]) == (short)(0xaaaa+(0x300*PCMVeriGCnt)+j*2-PCMVeriShift*2)) {
                            ;//pass
                        } else {
                            DiagPrintf("(%x,%x,%d,%d,%d)", pcm_rx[0].payload[j+40*2*pcm_rxpage[PcmIndex][0]], (short)(0xaaaa+(0x300*PCMVeriGCnt)+j*2-PCMVeriShift*2), PCMVeriGCnt, pcm_rxpage[PcmIndex][0], j);
                            PCMVeriErrCnt++;
                            break;
                        }
                   }

                }
            }
            PCMVeriGCnt ++;

            PcmIsr03 &= ~PCMChanRxPage[rx_isrpage];
            pcm_set_rx_own_bit(PcmIndex, 0, pcm_rxpage[PcmIndex][0]);
            pcm_rxpage[PcmIndex][0] ^= 1;
        }
    }
}



VOID
PCMTestProc(
    IN  VOID    *PCMTestData
)
{
    u32 pcmTest;
    u32 pcmtemp;
    u32 pcmIndex = 1;
    u32 pcmPinmuxSelect = 1;
    u32 i, j;

    PPCM_ADAPTER    pPcmAdapter;
    IRQ_HANDLE      PCMIrqHandle;
    PPCM_VERIFY_PARAMETER pPcmVerifyPara = &PcmVerifyPere;

    PPCM_VERI_PARA pPCMVeriPara = (PPCM_VERI_PARA) PCMTestData;

    pPcmAdapter = &TestPcmAdapte;
    pPcmVerifyPara->PcmIndex = pcmIndex;

    HalPcmOpInit(&HalPcmOp);
    HalPcmOp.HalPcmInit(&HalPcmAdapter);

    pPcmAdapter->pHalPcmAdapter=&HalPcmAdapter;
    pPcmAdapter->pHalPcmAdapter->PcmIndex = pPcmVerifyPara->PcmIndex;

    PCMIrqHandle.Data = (u32) (pPcmAdapter);
#ifdef SOC_TYPE_8195A
    PCMIrqHandle.IrqNum = I2S0_PCM0_IRQ + pPcmVerifyPara->PcmIndex;
#elif defined(SOC_TYPE_8198E)
    PCMIrqHandle.IrqNum = PCM_IRQ;
#endif
    PCMIrqHandle.IrqFun = (IRQ_FUN) PCMTaskIrqHandle;
    PCMIrqHandle.Priority = 0;
	//DiagPrintf("(*40000074=%x, *40000210=%x, *40000218=%x)\n", *((volatile unsigned int*)0x40000074), *((volatile unsigned int*)0x40000210), *((volatile unsigned int*)0x40000218));
	//DiagPrintf("(*40000238=%x, *400002c0=%x, *4000028c=%x)\n", *(unsigned int*)0x40000238, *(unsigned int*)0x400002c0, *(unsigned int*)0x4000028c);

    // enable system pll
    pcmtemp = HAL_READ32(SYSTEM_CTRL_BASE, REG_SOC_SYS_SYSPLL_CTRL1) | (1<<9) | (1<<10);
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SOC_SYS_SYSPLL_CTRL1, pcmtemp);

    //DiagPrintf("a");
    //4 Turn off shutdown mode
    ACTCK_PCM_CCTRL(ON);
    LXBUS_FCTRL(ON); // enable lx bus for pcm
    if (pcmIndex == 0) {
        PinCtrl(PCM0, pcmPinmuxSelect, ON);
        PCM0_FCTRL(ON);
    } else {
        PinCtrl(PCM1, pcmPinmuxSelect, ON);
        PCM0_FCTRL(ON);//pcm 1 is control by bit 28 BIT_PERI_PCM0_EN
        PCM1_FCTRL(ON);
    }

    //DiagPrintf("b");
	//DiagPrintf("(*40000074=%x, *40000210=%x, *40000218=%x)\n", *((volatile unsigned int*)0x40000074), *((volatile unsigned int*)0x40000210), *((volatile unsigned int*)0x40000218));
	//DiagPrintf("(*40000238=%x, *400002c0=%x, *4000028c=%x)\n", *(unsigned int*)0x40000238, *(unsigned int*)0x400002c0, *(unsigned int*)0x4000028c);

    pPcmVerifyPara->PcmPageSize = 40-1; //10ms=80 samples, 1 sample = 16bits

    //4 Reset Pcm IP
    HAL_PCMX_WRITE32(pPcmVerifyPara->PcmIndex, REG_PCM_CTL, (1<<12)|BIT_CTLX_ENDIAN_SWAP(1)|BIT_CTLX_LINEARMODE(1));
    i=10000;
    while (i) {
        i--;
        asm volatile ("nop\n\t");
    }
    HAL_PCMX_WRITE32(pPcmVerifyPara->PcmIndex, REG_PCM_CTL, (0<<12)|BIT_CTLX_ENDIAN_SWAP(1)|BIT_CTLX_LINEARMODE(1));
    i=10000;
    while (i) {
        i--;
        asm volatile ("nop\n\t");
    }
    HAL_PCMX_WRITE32(pPcmVerifyPara->PcmIndex, REG_PCM_CTL, (1<<12)|BIT_CTLX_ENDIAN_SWAP(1)|BIT_CTLX_LINEARMODE(1));
    i=10000;
    while (i) {
        i--;
        asm volatile ("nop\n\t");
    }
    //DiagPrintf("c");
    
    HAL_PCMX_WRITE32(pPcmVerifyPara->PcmIndex, REG_PCM_ISR03, (0xff<<24));//clear pending interrupt

    HAL_PCMX_WRITE32(pPcmVerifyPara->PcmIndex, REG_PCM_TSR03, 0);
    HAL_PCMX_WRITE32(pPcmVerifyPara->PcmIndex, REG_PCM_BSIZE03, pPcmVerifyPara->PcmPageSize<<24);

    pcm_tx[0].payload = &pcm_tx_buf[0][0];
    pcm_rx[0].payload = &pcm_rx_buf[0][0];

    for (j=0 ; j<2 ; j++) {
        for (i=0 ; i<((pPcmVerifyPara->PcmPageSize+1)*2) ; i++) {
            pcm_tx[0].payload[i+j*((pPcmVerifyPara->PcmPageSize+1)*2)] = 0xaaaa+0x300*j+i*2;
            pcm_rx[0].payload[i+j*((pPcmVerifyPara->PcmPageSize+1)*2)] = 0;
        }
    }

    HAL_PCMX_WRITE32(pPcmVerifyPara->PcmIndex, REG_PCM_CH0TXBSA, (u32)pcm_tx[0].payload);
    HAL_PCMX_WRITE32(pPcmVerifyPara->PcmIndex, REG_PCM_CH0RXBSA, (u32)pcm_rx[0].payload);

    HAL_PCMX_WRITE32(pPcmVerifyPara->PcmIndex, REG_PCM_CH0TXBSA, (u32)pcm_tx[0].payload| 0x3);
    HAL_PCMX_WRITE32(pPcmVerifyPara->PcmIndex, REG_PCM_CH0RXBSA, (u32)pcm_rx[0].payload| 0x3);
    
    HAL_PCMX_WRITE32(pPcmVerifyPara->PcmIndex, REG_PCM_ISR03, (0xff<<24));
    HAL_PCMX_WRITE32(pPcmVerifyPara->PcmIndex, REG_PCM_IMR03, (0xf0<<24));

    pcm_txpage[pPcmVerifyPara->PcmIndex][0] = 0;
    pcm_rxpage[pPcmVerifyPara->PcmIndex][0] = 0;

    PCMVeriErrCnt = 0;
    PCMVeriGCnt = 0;
    PCMVeriShift = 0;
    pcm_isr_cnt = 0;
#if 1
    InterruptRegister(&PCMIrqHandle);
    InterruptEn(&PCMIrqHandle);
#endif
    // internal loopback
    // HAL_PCMX_WRITE32(pPcmVerifyPara->PcmIndex, REG_PCM_CTL, (1<<12)|BIT_CTLX_ENDIAN_SWAP(1)|BIT_CTLX_LINEARMODE(1)|BIT_(14));
    // HAL_PCMX_WRITE32(pPcmVerifyPara->PcmIndex, REG_PCM_CHCNR03, 0x3<<24);
    // external loopback
    HAL_PCMX_WRITE32(pPcmVerifyPara->PcmIndex, REG_PCM_CHCNR03, 0x3<<24);

    for (;;)
    {
#if 1
        //4 waiting for command
        RtlDownSema(&(pPCMVeriPara->VeriSema));
        pcmTest = pPCMVeriPara->VeriItem;

        switch (pcmTest)
        {
            case PCM_TEST_N_DEINT:

            default:
                goto ENDPCMTASK;
                break;
        }
#endif
    }

ENDPCMTASK:

    HAL_PCMX_WRITE32(pPcmVerifyPara->PcmIndex, REG_PCM_CHCNR03, 0);


    vTaskDelete(NULL);


}


#endif

static u32 pcm_init=0;
VOID
PCMTestApp(
    IN  VOID      *Data
)
{
#ifdef PCM_FREE_RTOS_TASK_LOOPBACK_TEST
	PPCM_VERI_PARA pPCMVerParaTmp = (PPCM_VERI_PARA)Data;

	switch (pPCMVerParaTmp->VeriProcCmd)
	{
		//4 PCM_TEST_INIT
		case PCM_TEST_INIT:
			PCMVeriGCnt = 0;
			u32 PCMTmpSts;
			pPCMVeriAdp = NULL;
			pPCMVeriAdp = (PPCM_VERI_PARA)RtlZmalloc(sizeof(PCM_VERI_PARA));
			RtlInitSema(&(pPCMVeriAdp->VeriSema), 0);
			pPCMVeriAdp->VeriStop = _FALSE;
			PCMTmpSts = xTaskCreate( PCMTestProc, (const signed char *)"PCMVERI_TASK",
			               ((1024*4)/sizeof(portBASE_TYPE)), (void *)pPCMVeriAdp, 1, pPCMVeriAdp->PCMTask);
			break;

		case PCM_TEST_SWCH:
			break;

		case PCM_TEST_DEINIT:
			//4 force the task goes to end
			pPCMVeriAdp->VeriItem = PCM_TEST_N_DEINT;
			RtlUpSema(&(pPCMVeriAdp->VeriSema));

			//4 free the adaptor
			RtlMfree((u8 *)pPCMVeriAdp, sizeof(PCM_VERI_PARA));
			pPCMVeriAdp = NULL;
			break;

		case PCM_TEST_GETCNT:
			DBG_8195A("   [pcm_DBG]:Total Verification Loop:%x, Error%x, Shift=%d\n", PCMVeriGCnt, PCMVeriErrCnt, PCMVeriShift);
			break;

		default:
			break;
	}
#else
	int i;
	PPCM_VERIFY_PARAMETER pTempPara = (PPCM_VERIFY_PARAMETER)Data;
	PPCM_VERIFY_PARAMETER pPcmVerifyPara = &PcmVerifyPere;

	PPCM_ADAPTER         pPcmAdapter;
	IRQ_HANDLE            PCMIrqHandle;


	pPcmAdapter = &TestPcmAdapte;
	pPcmVerifyPara->PcmTestMode = pTempPara->PcmTestMode;
        pPcmVerifyPara->PcmIndex = pTempPara->PcmIndex;
        pPcmVerifyPara->PcmWideband = pTempPara->PcmWideband;
        pPcmVerifyPara->PcmPageSize = pTempPara->PcmPageSize;
        pPcmVerifyPara->PcmLinearUA = pTempPara->PcmLinearUA;
        pPcmVerifyPara->PcmTimeSlot = pTempPara->PcmTimeSlot;

	DBG_8195A_DRIVER("test pcm mode =%x\n", pPcmVerifyPara->PcmTestMode);
	DBG_8195A_DRIVER("test pcm index =%x\n", pPcmVerifyPara->PcmIndex);
        DBG_8195A_DRIVER("test pcm PageSize = %x\n", pPcmVerifyPara->PcmPageSize);
        DBG_8195A_DRIVER("test pcm PcmLinearUA = %x\n", pPcmVerifyPara->PcmLinearUA);
        DBG_8195A_DRIVER("test pcm PcmWideband = %x\n", pPcmVerifyPara->PcmWideband);
        DBG_8195A_DRIVER("test pcm PcmTimeSlot = %x\n", pPcmVerifyPara->PcmTimeSlot);
#if defined(SOC_TYPE_8196F)
	*(volatile unsigned long *)(0xb8000010) = ((*(volatile unsigned long *)(0xb8000010)) & 0xFFBFFFFF) | (0x1 << 23) | (0x1 << 12) | (0x1 << 13);
	*(volatile unsigned long *)(0xb8000024) = ((*(volatile unsigned long *)(0xb8000024)) & 0xFFDFFFFF) | (0x3 << 20);
	//pinmux
	//*(volatile unsigned long *)(0xb8000848) = 0x2223000;
	*(volatile unsigned long *)(0xb8000820) = 0x20222000;

#elif defined(SOC_TYPE_8198F)
	*(volatile unsigned long *)(0xF43203A8 ) = ((*(volatile unsigned long *)(0xF43203A8)) & 0xD7FFFFFF) | (0x1 << 29);// | (0x1 << 27);	
	*(volatile unsigned long *)(0xF43204B4 ) = ((*(volatile unsigned long *)(0xF43204B4)) & 0x3FFFFFFF) | (0x1 << 30) | (0x1 << 31);
	*(volatile unsigned long *)(0xF43203AC ) = ((*(volatile unsigned long *)(0xF43203AC)) & 0xFFFFE6FF) | (0x1 << 12) | (0x1 << 11) | (0x1 << 8);	
#endif

	//init hal 123
	HalPcmOpInit(&HalPcmOp);
	HalPcmOp.HalPcmInit(&HalPcmAdapter);

	pPcmAdapter->pHalPcmAdapter=&HalPcmAdapter;
	pPcmAdapter->pHalPcmAdapter->PcmIndex=pPcmVerifyPara->PcmIndex;
#if 0
        check_pcm_reg(3);
#else
	PCMIrqHandle.Data = (u32) (pPcmAdapter);
#ifdef SOC_TYPE_8195A
	PCMIrqHandle.IrqNum = I2S0_PCM0_IRQ + pPcmVerifyPara->PcmIndex;
#elif defined(SOC_TYPE_8198E) || defined(SOC_TYPE_8196F) || defined(SOC_TYPE_8198F)
        PCMIrqHandle.IrqNum = PCM_IRQ;
#endif
	PCMIrqHandle.IrqFun = (IRQ_FUN) PCMLoopBackIrqHandle;
	PCMIrqHandle.Priority = 0;
//DiagPrintf("a\n");
        //DiagPrintf("(%d)", pcm_init);
	if (pcm_init!=1 ) {
            #if defined(SOC_TYPE_8196F) || defined(SOC_TYPE_8198F)
                //DiagPrintf("I2S IRQNUM : %x\n",I2SIrqHandle.IrqNum);
                irq_install_handler(PCMIrqHandle.IrqNum,PCMIrqHandle.IrqFun, (u32)(PCMIrqHandle.Data));
                //InterruptRegister(&I2SIrqHandle);
                //DiagPrintf("A\n");
                //InterruptEn(&I2SIrqHandle);
            #else
	        InterruptRegister(&PCMIrqHandle);
	        //DiagPrintf("A\n");
	        InterruptEn(&PCMIrqHandle);
                pcm_init = 1;
            #endif
        }
//DiagPrintf("b\n");
        //pcm_txpage[pPcmAdapter->pHalPcmAdapter->PcmIndex][0]=0;
        //pcm_rxpage[pPcmAdapter->pHalPcmAdapter->PcmIndex][0]=0;
	if (pPcmVerifyPara->PcmTestMode == 3) {//rx only left right channel check.
		//for (i=0 ; i<128 ; i++)
		for (i=0 ; i<1 ; i++)
			PcmTestMode3(pPcmVerifyPara);
	} else if (pPcmVerifyPara->PcmTestMode == 0) {
                PcmTestMode0(pPcmVerifyPara);
        }
#endif
#endif

}

VOID
pcm_set_tx_own_bit(
    IN  u32         PcmIndex,
    IN  u32         PcmChid,
    IN  u32         PcmPageIndex
)
{
    if (PcmChid < 4) {
        HAL_PCMX_WRITE32(PcmIndex, REG_PCM_CH0TXBSA + 4*PcmChid, HAL_PCMX_READ32(PcmIndex, REG_PCM_CH0TXBSA + 4*PcmChid)|BIT_(PcmPageIndex));
    } else if (PcmChid < 8){
        HAL_PCMX_WRITE32(PcmIndex, REG_PCM_CH4TXBSA + 4*(PcmChid&3), HAL_PCMX_READ32(PcmIndex, REG_PCM_CH4TXBSA + 4*(PcmChid&3))|BIT_(PcmPageIndex));
    } else if (PcmChid < 12){
        HAL_PCMX_WRITE32(PcmIndex, REG_PCM_CH8TXBSA + 4*(PcmChid&3), HAL_PCMX_READ32(PcmIndex, REG_PCM_CH8TXBSA + 4*(PcmChid&3))|BIT_(PcmPageIndex));
    } else if (PcmChid < 16){
        HAL_PCMX_WRITE32(PcmIndex, REG_PCM_CH12TXBSA + 4*(PcmChid&3), HAL_PCMX_READ32(PcmIndex, REG_PCM_CH12TXBSA + 4*(PcmChid&3))|BIT_(PcmPageIndex));
    }
}

VOID
pcm_set_rx_own_bit(
    IN  u32         PcmIndex,
    IN  u32         PcmChid,
    IN  u32         PcmPageIndex
)
{
    if (PcmChid < 4) {
        HAL_PCMX_WRITE32(PcmIndex, REG_PCM_CH0RXBSA + 4*PcmChid, HAL_PCMX_READ32(PcmIndex, REG_PCM_CH0RXBSA + 4*PcmChid)|BIT_(PcmPageIndex));
    } else if (PcmChid < 8){
        HAL_PCMX_WRITE32(PcmIndex, REG_PCM_CH4RXBSA + 4*(PcmChid&3), HAL_PCMX_READ32(PcmIndex, REG_PCM_CH4RXBSA + 4*(PcmChid&3))|BIT_(PcmPageIndex));
    } else if (PcmChid < 12){
        HAL_PCMX_WRITE32(PcmIndex, REG_PCM_CH8RXBSA + 4*(PcmChid&3), HAL_PCMX_READ32(PcmIndex, REG_PCM_CH8RXBSA + 4*(PcmChid&3))|BIT_(PcmPageIndex));
    } else if (PcmChid < 16){
        HAL_PCMX_WRITE32(PcmIndex, REG_PCM_CH12RXBSA + 4*(PcmChid&3), HAL_PCMX_READ32(PcmIndex, REG_PCM_CH12RXBSA + 4*(PcmChid&3))|BIT_(PcmPageIndex));
    }
}

VOID
pcm_loopback_test(
    IN  VOID   *Data,
    IN  u32    chid,
    IN  u32    pindex
)
{
    PPCM_ADAPTER pPcmAdapter = (PPCM_ADAPTER) Data;
    PHAL_PCM_ADAPTER pHalPcmAdapter = pPcmAdapter->pHalPcmAdapter;
    u8 PcmIndex = pHalPcmAdapter->PcmIndex;
    u32 i, j;
    
    flush_dcache_range(((unsigned long)(pcm_rx[chid].payload+(PcmVerifyPere.PcmPageSize+1)*2*pindex))&(~(ARCH_DMA_MINALIGN-1)), ((unsigned long)(pcm_rx[chid].payload+(PcmVerifyPere.PcmPageSize+1)*2*(pindex+1)+32))&(~(ARCH_DMA_MINALIGN-1)));
    invalidate_dcache_range(((unsigned long)(pcm_rx[chid].payload+(PcmVerifyPere.PcmPageSize+1)*2*pindex))&(~(ARCH_DMA_MINALIGN-1)), ((unsigned long)(pcm_rx[chid].payload+(PcmVerifyPere.PcmPageSize+1)*2*(pindex+1)+32))&(~(ARCH_DMA_MINALIGN-1)));
    j = 0;
    for (i=0 ; i<(PcmVerifyPere.PcmPageSize+1)*2 ; i++) {
        s16 rxi = *(pcm_rx[chid].payload + (PcmVerifyPere.PcmPageSize+1)*2*pindex + i);
        s16 cmp = pcm_comp[chid][(PcmVerifyPere.PcmPageSize+1)*2*pindex + i];
        if (pcm_isr_cnt < (3998/(PcmVerifyPere.PcmPageSize+1)*PCM_TEST_TIME_SEC-4)) {
            // 20141006 jwsyu notes: if page size is not equal = cache line size.
             // the page 0 final word and page 1 start word are share cache line.
             // write the page 0 final word will error. the cache line will dirty.
            if (((PcmVerifyPere.PcmPageSize+1)&0xf) && (i>(PcmVerifyPere.PcmPageSize*2-30)))
                ;//let the this page final word and next page start word cache line clean;
            else
                *(pcm_rx[chid].payload + (PcmVerifyPere.PcmPageSize+1)*2*pindex + i) = 0xa55a;//clear data
        }
        if (rxi != cmp) {
            j++;
            pcm_err_cnt++;
            if (j < 5) {
                DiagPrintf("\n%d-%d-%d,%x!=%x", chid, pindex, i, rxi, cmp);
            }
        }
	/*
	else if( (chid == 8) && (( (PcmVerifyPere.PcmPageSize+1)*2*pindex + i) == 163))
	{
		 DiagPrintf("(%d, %d) %04x ==%04x, %04x\n",chid, i, rxi, cmp, pcm_rx[8].payload[163]);
	}
	*/
    }
    flush_dcache_range(((unsigned long)(pcm_rx[chid].payload+(PcmVerifyPere.PcmPageSize+1)*2*pindex))&(~(ARCH_DMA_MINALIGN-1)), ((unsigned long)(pcm_rx[chid].payload+(PcmVerifyPere.PcmPageSize+1)*2*(pindex+1)+32))&(~(ARCH_DMA_MINALIGN-1)));
    invalidate_dcache_range(((unsigned long)(pcm_rx[chid].payload+(PcmVerifyPere.PcmPageSize+1)*2*pindex))&(~(ARCH_DMA_MINALIGN-1)), ((unsigned long)(pcm_rx[chid].payload+(PcmVerifyPere.PcmPageSize+1)*2*(pindex+1)+32))&(~(ARCH_DMA_MINALIGN-1)));
}



VOID
PCM_ISR(
    IN  VOID        *Data,
    IN  u32         PcmIsr03,
    IN  u32         PcmIsr47,
    IN  u32         PcmIsr811,
    IN  u32         PcmIsr1215
)
{
    PPCM_ADAPTER pPcmAdapter = (PPCM_ADAPTER) Data;
    PHAL_PCM_ADAPTER pHalPcmAdapter = pPcmAdapter->pHalPcmAdapter;
    u8 PcmIndex = pHalPcmAdapter->PcmIndex;
    u32 PcmPageNum;
    u32 chid = 0, i, j, k;
    u32 tx_isrpage, rx_isrpage;
    u32* pPcmIsr = 0;
    u32 pcm_cnt_1sec;
    
//DiagPrintf("d");
    pcm_cnt_1sec = 4000/(PcmVerifyPere.PcmPageSize+1)*(PcmVerifyPere.PcmWideband+1);

    for (i=0 ; i<2 ; i++) { // page0, page1
        chid = 0;
        for (j = 0; j < PCM_CH_NUM; j++, chid = j) {
            if (chid < 4)
	        pPcmIsr = &PcmIsr03;
	    else if (chid < 8)
	        pPcmIsr = &PcmIsr47;
            else if (chid < 12)
                pPcmIsr = &PcmIsr811;
            else
                pPcmIsr = &PcmIsr1215;

            tx_isrpage = 2*chid + pcm_txpage[PcmIndex][chid];
	    rx_isrpage = 2*chid + pcm_rxpage[PcmIndex][chid];

            ////// ISR - TX //////
	    if ( (*pPcmIsr) & PCMChanTxPage[tx_isrpage] ) {
                if (chid == 0) {
                    pcm_isr_cnt++;
                    if ( (pcm_isr_cnt%pcm_cnt_1sec) == (pcm_cnt_1sec-1)) {
                        DiagPrintf("~");
                    }
                    if ( (pcm_isr_cnt%(60*pcm_cnt_1sec)) == ((60*pcm_cnt_1sec)-1)) {
                        DiagPrintf("\n");
                    }
                    
                }
    invalidate_dcache_range(((unsigned long)(pcm_tx[chid].payload+(PcmVerifyPere.PcmPageSize+1)*2*pcm_txpage[PcmIndex][chid]))&(~(ARCH_DMA_MINALIGN-1)), ((unsigned long)(pcm_tx[chid].payload+(PcmVerifyPere.PcmPageSize+1)*2*(pcm_txpage[PcmIndex][chid]+1)+32))&(~(ARCH_DMA_MINALIGN-1)));
#ifdef PCM_PLAY_TONE_TEST
                if (chid == play_channel_now) {
                #if PCM_TONEGEN_TABLE
                    for (i=0 ; i<(PcmVerifyPere.PcmPageSize+1)*2; i++) {
                        pcm_tx[chid].payload[i+((PcmVerifyPere.PcmPageSize+1)*2*pcm_txpage[PcmIndex][chid])] = pcm_vpat[tone_phase]/8;
                        tone_phase++;
                        if (tone_phase == 16)
                            tone_phase = 0;
                    }
                #else
                    PCMToneGens((PcmVerifyPere.PcmPageSize+1)*2, &pcm_tx[chid].payload[((PcmVerifyPere.PcmPageSize+1)*2*pcm_txpage[PcmIndex][chid])]);
                #endif
                }
#endif
#ifdef PCM_MIC_LOOPBACK_SPEAKER_TEST
                if (chid == 0) {
                    memcpy(&pcm_tx[chid].payload[((PcmVerifyPere.PcmPageSize+1)*2*pcm_txpage[PcmIndex][chid])], &pcm_databuf[pcm_tx_read_index], (PcmVerifyPere.PcmPageSize+1)*4);
                    pcm_tx_read_index = (pcm_tx_read_index+(PcmVerifyPere.PcmPageSize+1)*4)%PCM_DATABUFSIZE;
                }
#endif
    flush_dcache_range(((unsigned long)(pcm_tx[chid].payload+(PcmVerifyPere.PcmPageSize+1)*2*pcm_txpage[PcmIndex][chid]))&(~(ARCH_DMA_MINALIGN-1)), ((unsigned long)(pcm_tx[chid].payload+(PcmVerifyPere.PcmPageSize+1)*2*(pcm_txpage[PcmIndex][chid]+1)+32))&(~(ARCH_DMA_MINALIGN-1)));
    invalidate_dcache_range(((unsigned long)(pcm_tx[chid].payload+(PcmVerifyPere.PcmPageSize+1)*2*pcm_txpage[PcmIndex][chid]))&(~(ARCH_DMA_MINALIGN-1)), ((unsigned long)(pcm_tx[chid].payload+(PcmVerifyPere.PcmPageSize+1)*2*(pcm_txpage[PcmIndex][chid]+1)+32))&(~(ARCH_DMA_MINALIGN-1)));
	        pcm_set_tx_own_bit(PcmIndex, chid, pcm_txpage[PcmIndex][chid]);
                *pPcmIsr &= ~PCMChanTxPage[tx_isrpage];
		pcm_txpage[PcmIndex][chid] ^= 1;
           //DiagPrintf("%d.", chid);
            } // end of tx

            if ( (*pPcmIsr) & PCMChanRxPage[rx_isrpage] ) {

#ifndef PCM_CODEC_ALC5621_OR_ALC5651
                if (1) {
                    pcm_loopback_test(Data, chid, pcm_rxpage[PcmIndex][chid]);
                } else {
                    //do playtone or mic loopback to spk
                }
#endif
#ifdef PCM_MIC_LOOPBACK_SPEAKER_TEST
    flush_dcache_range(((unsigned long)(pcm_rx[chid].payload+(PcmVerifyPere.PcmPageSize+1)*2*pcm_rxpage[PcmIndex][chid]))&(~(ARCH_DMA_MINALIGN-1)), ((unsigned long)(pcm_rx[chid].payload+(PcmVerifyPere.PcmPageSize+1)*2*(pcm_rxpage[PcmIndex][chid]+1)+32))&(~(ARCH_DMA_MINALIGN-1)));
    invalidate_dcache_range(((unsigned long)(pcm_rx[chid].payload+(PcmVerifyPere.PcmPageSize+1)*2*pcm_rxpage[PcmIndex][chid]))&(~(ARCH_DMA_MINALIGN-1)), ((unsigned long)(pcm_rx[chid].payload+(PcmVerifyPere.PcmPageSize+1)*2*(pcm_rxpage[PcmIndex][chid]+1)+32))&(~(ARCH_DMA_MINALIGN-1)));
                if (chid == 0) {
                    memcpy( &pcm_databuf[pcm_rx_write_index], &pcm_rx[chid].payload[((PcmVerifyPere.PcmPageSize+1)*2*pcm_rxpage[PcmIndex][chid])], (PcmVerifyPere.PcmPageSize+1)*4);
                    pcm_rx_write_index = (pcm_rx_write_index+(PcmVerifyPere.PcmPageSize+1)*4)%PCM_DATABUFSIZE;
                }
#endif
	        pcm_set_rx_own_bit(PcmIndex, chid, pcm_rxpage[PcmIndex][chid]);
                *pPcmIsr &= ~PCMChanRxPage[rx_isrpage];
		pcm_rxpage[PcmIndex][chid] ^= 1;
		//DiagPrintf("%d*", chid);
            }
        }
    }

    //pcm stop check
    if ((pcm_isr_cnt > (4000/(PcmVerifyPere.PcmPageSize+1)*PCM_TEST_TIME_SEC*(PcmVerifyPere.PcmWideband+1))) || (pcm_err_cnt>1800)) {
        // chang pcm_isr_cnt to print once
        pcm_isr_cnt = (3998/(PcmVerifyPere.PcmPageSize+1)*PCM_TEST_TIME_SEC);
	pcm_err_cnt = 0;
        HAL_PCMX_WRITE32(PcmIndex, REG_PCM_CHCNR03, 0xfcfcfcfc & HAL_PCMX_READ32(PcmIndex, REG_PCM_CHCNR03));
        HAL_PCMX_WRITE32(PcmIndex, REG_PCM_CHCNR47, 0xfcfcfcfc & HAL_PCMX_READ32(PcmIndex, REG_PCM_CHCNR47));
        HAL_PCMX_WRITE32(PcmIndex, REG_PCM_CHCNR811, 0xfcfcfcfc & HAL_PCMX_READ32(PcmIndex, REG_PCM_CHCNR811));
        HAL_PCMX_WRITE32(PcmIndex, REG_PCM_CHCNR1215, 0xfcfcfcfc & HAL_PCMX_READ32(PcmIndex, REG_PCM_CHCNR1215));
#if defined(PCM_PLAY_TONE_TEST)
        for ( chid=0 ; chid<3 ; chid++){
            DiagPrintf("\nchid = %d,", chid);
            DiagPrintf("pcm tx page data:\n");
            for (i=0 ; i<(PcmVerifyPere.PcmPageSize+1)*4 ; i++) {
                if ((i%8) == 7)
                    DiagPrintf(" %x\n", pcm_tx[chid].payload[i]);
                else
                    DiagPrintf(" %x ", pcm_tx[chid].payload[i]);
            }
        }

#else
	//DiagPrintf("before print rx: (8, 163) %04x ==%04x\n", pcm_rx[8].payload[163], pcm_comp[8][163]);
#ifdef PRINT_DATA
  #ifdef PCM_MIC_LOOPBACK_SPEAKER_TEST   
        for ( chid=0 ; chid<3 ; chid++){
  #else
        for ( chid=0 ; chid<PCM_CH_NUM ; chid++){
  #endif
            DiagPrintf("\nchid = %d,", chid);
            DiagPrintf("pcm rx page data:\n");
            for (i=0 ; i<(PcmVerifyPere.PcmPageSize+1)*4 ; i++) {
                if ((i%8) == 7)
                    DiagPrintf(" %x\n", pcm_rx[chid].payload[i]);
                else
                    DiagPrintf(" %x ", pcm_rx[chid].payload[i]);
            }
        }
#endif  //PRINT_DATA
#endif
#ifndef PCM_CODEC_ALC5621_OR_ALC5651
	//DiagPrintf("before compare rx: (8, 163) %04x ==%04x\n", pcm_rx[8].payload[163], pcm_comp[8][163]);
        int pcm_ch = PCM_CH_NUM;
        j=0;
        if (PcmVerifyPere.PcmWideband)
            pcm_ch = 8;

        for ( chid=0 ; chid<pcm_ch ; chid++)
        {
            for (i=0 ; i<(PcmVerifyPere.PcmPageSize+1)*4 ; i++) 
            {
                if (pcm_rx[chid].payload[i] != pcm_comp[chid][i]) 
                {
                    DiagPrintf("(%d, %d) %04x!=%04x\n",chid, i, pcm_rx[chid].payload[i], pcm_comp[chid][i]);
                    j++;
                }
                if (j > 30)
                    break;
            }
        }
	//DiagPrintf("after print rx: (8, 163) %04x ==%04x\n", pcm_rx[8].payload[163], pcm_comp[8][163]);
        DBG_8195A_DRIVER("check error number=%d\n", j);
#endif

        print_pcm_regs(PcmIndex);
        DBG_8195A_DRIVER("==============>> pcm end test   << ===================\n");
    }
}


VOID
PCMLoopBackIrqHandle
(
    IN  VOID        *Data
)
{
    PPCM_ADAPTER pPcmAdapter = (PPCM_ADAPTER) Data;
    PHAL_PCM_ADAPTER pHalPcmAdapter = pPcmAdapter->pHalPcmAdapter;
    u8 PcmIndex = pHalPcmAdapter->PcmIndex;

    u32 PcmIsr03, PcmIsr47, PcmIsr811, PcmIsr1215;
#if 1
    PcmIsr03 = HAL_PCMX_READ32(PcmIndex, REG_PCM_ISR03);
    PcmIsr47 = HAL_PCMX_READ32(PcmIndex, REG_PCM_ISR47);
    PcmIsr811 = HAL_PCMX_READ32(PcmIndex, REG_PCM_ISR811);
    PcmIsr1215 = HAL_PCMX_READ32(PcmIndex, REG_PCM_ISR1215);
    if(PcmIsr03&0xf0f0f0f) {
        DiagPrintf("g%xg", PcmIsr03);
    }
    HAL_PCMX_WRITE32(PcmIndex, REG_PCM_ISR03, PcmIsr03);
    HAL_PCMX_WRITE32(PcmIndex, REG_PCM_ISR47, PcmIsr47);
    HAL_PCMX_WRITE32(PcmIndex, REG_PCM_ISR811, PcmIsr811);
    HAL_PCMX_WRITE32(PcmIndex, REG_PCM_ISR1215, PcmIsr1215);

    PCM_ISR(pPcmAdapter, PcmIsr03 & 0xF0F0F0F0, PcmIsr47 & 0xF0F0F0F0, PcmIsr811 & 0xF0F0F0F0,
              PcmIsr1215 & 0xF0F0F0F0);
#endif

}

#ifdef PCM_PLAY_TONE_TEST
s16
pcm_sinus
(
    IN  s32    x
)
{
    s16 i;
    s32 x2;
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
        res += coef[i] * x2;               // Q27
        x2 *= q;                           // Q30
        x2 >>= 15;                         // Q15
    }
    res >>= 12; /* back to 0x0000-0xffff */   // Q15
    if (x > 0x00008000L)
        res = -res;
    if (res > 32767)
        res = 32767;
    else if (res < -32768)
        res = -32768;

    return (s16) res;
}

VOID
PCMToneGens
(
    IN u32    sample_num,
    IN s16   *buffPtr
)
{
    u32 i;

    tone_phase_ad = (1014 * 16777) >> 11;    // 65535/8000 in Q11

    for (i=0 ; i<sample_num ; i++) {
        *buffPtr = pcm_sinus(tone_phase)>>2;
        tone_phase += tone_phase_ad;
        if (tone_phase > 65535)
            tone_phase -= 65535;
        buffPtr ++;
    }

}

#endif

