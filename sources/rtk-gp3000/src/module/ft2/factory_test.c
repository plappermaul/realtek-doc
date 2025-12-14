/* * Copyright(c) Realtek Semiconductor Corporation, 2008 
* All rights reserved. 
*
* Author : kevin.chung
*
* ============================================================================
*  Ver   Date(YYYY/MM/DD)  Description 
* ============================================================================ 
*  0.1     2009/06/01      Create draft version - For RTL8672 EQC board
* ============================================================================ 
*
* Purpose : PCM feature test for RTL8672 EQC board
*           
* Feature : For Linux-2.6.19 
*
* Description : 
*	1. Support 8 independent channels for VOIP application 
*	2. Lexra bus master
*	3. Flexible timing control 
*	4. Support A law/u law companding and 16bit linear mode * */

#include <linux/interrupt.h>
#include <linux/delay.h>
#include <asm/io.h>
#include <linux/mm.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/dma-mapping.h>
#include <rtk/gpio.h>
#include <rtk/switch.h>
#include <dal/apollomp/dal_apollomp_switch.h>
#include <linux/kthread.h>



//extern unsigned int	eqc_pass_count;
extern unsigned int	check_ft2_ready;
void ft2_write_mode(char *filename,char *mode);

static int log = 0;

#define TEST_REULST "/var/ft2_result"
#define printk_test(fmt,args...)  if(log) { printk(fmt, ##args); } 
#define printk_result(str,fmt,args...) { sprintf(str,fmt,##args); ft2_write_mode(TEST_REULST,str) ; printk(fmt, ##args); } 

int result[4]={0,0,0,0};
int prbs = 0;


/*****************************************************
*
* PCM Test Related fuction
*
*****************************************************/


#define PCM_BASE      	0xb8008000
#define PCM_PGCR      	(PCM_BASE + 0x00)     //pcm control register
#define PCM_PCSCR   	(PCM_BASE + 0x04)     //pcm channel specific control register
#define PCM_PTSCR     	(PCM_BASE + 0x08)     //pcm time slot assignment register
#define PCM_PBSIZE    	(PCM_BASE + 0x0C)     //pcm channel buffer size register

#define PCM_PCSCR47   	(PCM_BASE + 0x38)     //pcm channel specific control register
#define PCM_PTSCR47		(PCM_BASE + 0x3C)     //pcm time slot assignment register
#define PCM_PBSIZE47	(PCM_BASE + 0x40)     //pcm channel buffer size register

#define PCM_CH0TXBSA  	(PCM_BASE + 0x10)     //ch0_txbsa  tx buffer start_address
#define PCM_CH0RXBSA  	(PCM_BASE + 0x20)     //ch0_txbsa  tx buffer start_address
#define PCM_CH1TXBSA  	(PCM_BASE + 0x14)     //ch1_txbsa  tx buffer start_address
#define PCM_CH1RXBSA  	(PCM_BASE + 0x24)     //ch1_txbsa  tx buffer start_address
#define PCM_CH2TXBSA  	(PCM_BASE + 0x18)     //ch2_txbsa  tx buffer start_address
#define PCM_CH2RXBSA  	(PCM_BASE + 0x28)     //ch2_txbsa  tx buffer start_address
#define PCM_CH3TXBSA  	(PCM_BASE + 0x1C)     //ch3_txbsa  tx buffer start_address
#define PCM_CH3RXBSA  	(PCM_BASE + 0x2C)     //ch3_txbsa  tx buffer start_address   
#define PCM_PIMR  		(PCM_BASE + 0x30)     //pcm channel interrupt mask register
#define PCM_PISR  		(PCM_BASE + 0x34)     //pcm channel interrupt status register        

#define RTL_W32(reg, value)			(*(volatile u32*)(reg)) = (u32)value
#define RTL_R32(reg)				(*(volatile u32*)(reg))

#define PCM_BUF_SIZE 20


#if 1
enum PCM_REGS{
	/*PCM General Control Register , Addr : 0xB8008000*/
	C0ILBE		= 1<<14,	//Internal loop back enable.
	LINEAR_MODE	= 1<<13,	//Linear mode enable.
	PCM_AE		= 1<<12,	//PCM interface A Enable.
	PCM_CLK		= 1<<11,	//CLK and FS signel source of interface PCM A.
	FSINV		= 1<<9,		//PCM interface A Frame synchronization invert.

	/*PCM Ch0-3 Specific Control Register , Addr : 0xB8008004*/	
	CH0uA		= 1<<26,	//Channel 0 u law/A law select, 1:A law
	CH0TE		= 1<<25,	//Channel 0 Transmitter Enable.
	CH0RE		= 1<<24,	//Channel 0 Receiver Enable.

	CH1uA		= 1<<18,	//Channel 1 u law/A law select, 1:A law
	CH1TE		= 1<<17,	//Channel 1 Transmitter Enable.
	CH1RE		= 1<<16,	//Channel 1 Receiver Enable.

	CH2uA		= 1<<10,	//Channel 2 u law/A law select, 1:A law
	CH2TE		= 1<<9,		//Channel 2 Transmitter Enable.
	CH2RE		= 1<<8,		//Channel 2 Receiver Enable.

	CH3uA		= 1<<2,		//Channel 3 u law/A law select, 1:A law
	CH3TE		= 1<<1,		//Channel 3 Transmitter Enable.
	CH3RE		= 1<<0,		//Channel 3 Receiver Enable.	
	
	/*PCM Ch4-7 Specific Control Register , Addr : 0xB8008038*/	
	CH4uA		= 1<<26,	//Channel 4 u law/A law select, 1:A law
	CH4TE		= 1<<25,	//Channel 4 Transmitter Enable.
	CH4RE		= 1<<24,	//Channel 4 Receiver Enable.

	CH5uA		= 1<<18,	//Channel 5 u law/A law select, 1:A law
	CH5TE		= 1<<17,	//Channel 5 Transmitter Enable.
	CH5RE		= 1<<16,	//Channel 5 Receiver Enable.

	CH6uA		= 1<<10,	//Channel 6 u law/A law select, 1:A law
	CH6TE		= 1<<9,		//Channel 6 Transmitter Enable.
	CH6RE		= 1<<8,		//Channel 6 Receiver Enable.

	CH7uA		= 1<<2,		//Channel 7 u law/A law select, 1:A law
	CH7TE		= 1<<1,		//Channel 7 Transmitter Enable.
	CH7RE		= 1<<0,		//Channel 7 Receiver Enable.	
};
#endif


struct pcm_device {
	unsigned		active_channel;
	unsigned 		ch03_scr;
	//unsigned short	ch4_scr,ch5_scr,ch6_scr,ch7_scr;
	unsigned 		ch0_tsar,ch1_tsar,ch2_tsar,ch3_tsar;
	//unsigned 		ch4_tsar,ch5_tsar,ch6_tsar,ch7_tsar;
	unsigned		ch0_bsize,ch1_bsize,ch2_bsize,ch3_bsize;
	//unsigned		ch4_bsize,ch5_bsize,ch6_bsize,ch7_bsize;
	volatile unsigned long *ch0atxbsa,*ch1atxbsa,*ch2atxbsa,*ch3atxbsa;
	volatile unsigned long *ch0arxbsa,*ch1arxbsa,*ch2arxbsa,*ch3arxbsa;
	dma_addr_t 		tx0_dma , tx1_dma , tx2_dma , tx3_dma;
	dma_addr_t 		rx0_dma , rx1_dma , rx2_dma , rx3_dma;
};


static void re8670_pcm_init_hw (struct pcm_device *cp);
static void pcm_free_buf (struct pcm_device *cp);
static void re8670_pcm_close (struct pcm_device *cp);
static int pcm_buf_alloc(struct pcm_device *cp);


static void dump_pcm_register(void)
{
	printk_test("0xb8008000 : %X \n" , RTL_R32(0xb8008000));
	printk_test("0xb8008004 : %X \n" , RTL_R32(0xb8008004));
	printk_test("0xb8008008 : %X \n" , RTL_R32(0xb8008008));
	printk_test("0xb800800C : %X \n" , RTL_R32(0xb800800C));
	printk_test("0xb8008010 : %X \n" , RTL_R32(0xb8008010));
	printk_test("0xb8008014 : %X \n" , RTL_R32(0xb8008014));
	printk_test("0xb8008018 : %X \n" , RTL_R32(0xb8008018));
	printk_test("0xb800801C : %X \n" , RTL_R32(0xb800801C));
	printk_test("0xb8008020 : %X \n" , RTL_R32(0xb8008020));
	printk_test("0xb8008024 : %X \n" , RTL_R32(0xb8008024));
	printk_test("0xb8008028 : %X \n" , RTL_R32(0xb8008028));
	printk_test("0xb800802C : %X \n" , RTL_R32(0xb800802C));
	printk_test("0xb8008030 : %X \n" , RTL_R32(0xb8008030));
	printk_test("0xb8008034 : %X \n" , RTL_R32(0xb8008034));
	printk_test("0xb8008038 : %X \n" , RTL_R32(0xb8008038));
	printk_test("0xb800803C : %X \n" , RTL_R32(0xb800803C));
}

static void dump_txrx_data(struct pcm_device *cp ,int ch)
{
	volatile unsigned long *tx_tmp = NULL, *rx_tmp = NULL;
	int i;
	if(ch == 0)
	{
		tx_tmp = cp->ch0atxbsa;
		rx_tmp = cp->ch0arxbsa;
	}
	else if(ch == 1)
	{
		tx_tmp = cp->ch1atxbsa;
		rx_tmp = cp->ch1arxbsa;
	}
	else if(ch == 2)
	{
		tx_tmp = cp->ch2atxbsa;
		rx_tmp = cp->ch2arxbsa;
	}
	else if(ch == 3)
	{
		tx_tmp = cp->ch3atxbsa;
		rx_tmp = cp->ch3arxbsa;
	}
	
	for(i=0;i<PCM_BUF_SIZE;i++)
	{
		printk_test("tx%d_data[%d]=0x%08lx , rx%d_data[%d]=0x%08lx \n",ch , i,*(tx_tmp+i) ,ch ,i, *(rx_tmp+i));
	}
}

static int compare_txrx_data(struct pcm_device *cp ,int ch)
{
	volatile unsigned long *tx_tmp = NULL , *rx_tmp = NULL;
	int i;
	if(ch == 0)
	{
		tx_tmp = cp->ch0atxbsa;
		rx_tmp = cp->ch0arxbsa;
	}
	else if(ch == 1)
	{
		tx_tmp = cp->ch1atxbsa;
		rx_tmp = cp->ch1arxbsa;
	}
	else if(ch == 2)
	{
		tx_tmp = cp->ch2atxbsa;
		rx_tmp = cp->ch2arxbsa;
	}
	else if(ch == 3)
	{
		tx_tmp = cp->ch3atxbsa;
		rx_tmp = cp->ch3arxbsa;
	}
	
	for(i=0;i<PCM_BUF_SIZE;i++)
	{
		if(*(tx_tmp+i)!=*(rx_tmp+i))
		{
		   	printk_test("F_PCM\n CH : %d , i = %d  TX : %lX , RX : %lX \n",ch , i,*(tx_tmp+i) ,  *(rx_tmp+i)); 
			dump_txrx_data(cp , ch);
			dump_pcm_register();
		   	return 0;
		}
	}
	return 1;
}

//int re8670_pcm_test (struct pcm_device * cp){  
int re8670_pcm_test (int ch_sel)
{  
	struct pcm_device *pcm_dev;
   	unsigned pcm_ch=0,i,tx_data_temp=0x5a;
   	u32 pcm_tx_rx_enable=0;
   	volatile unsigned long *xmit_tmp;
	
	pcm_dev=(struct pcm_device *)kmalloc(sizeof(struct pcm_device),GFP_KERNEL);
	pcm_dev->active_channel=ch_sel;
	pcm_ch=pcm_dev->active_channel;
	re8670_pcm_init_hw(pcm_dev);
	
	if(pcm_buf_alloc(pcm_dev)){
		printk_test("memory allocate sucessful\n");
	}
	else{
		return 0;
	}

	switch(pcm_ch)
	{	
		case 0:
			xmit_tmp=pcm_dev->ch0atxbsa;
			printk_test("xmit_tmp=%p ,pcm_dev->ch0atxbsa=%p \n",xmit_tmp,pcm_dev->ch0atxbsa);
			RTL_W32(PCM_CH0TXBSA,(pcm_dev->tx0_dma|0x3));
			printk_test("PCM_CH0TXBSA = 0x%x   \n",RTL_R32(PCM_CH0TXBSA));
			for (i=0;i<PCM_BUF_SIZE;i++)
			{
				 *(xmit_tmp+i)= (tx_data_temp << (i%28))&0xFFFEFFFE; //external loop back has to ignore bit 0 and bit 16
				  //*(xmit_tmp+i)= ()&0xFFFEFFFE; //external loop back has to ignore bit 0 and bit 16
				//print_ft2("tx_data[%d]=0x%08x \n",i,*(xmit_tmp+i));
			}
			xmit_tmp=pcm_dev->ch0arxbsa;
			RTL_W32(PCM_CH0RXBSA,(pcm_dev->rx0_dma|0x3));			
			printk_test("PCM_CH0RXBSA = 0x%x   \n",RTL_R32(PCM_CH0RXBSA));
			pcm_tx_rx_enable |= (3<<24);
		break;
		case 1:
			xmit_tmp=pcm_dev->ch1atxbsa;
			printk_test("xmit_tmp=%p ,pcm_dev->ch1atxbsa=%p \n",xmit_tmp,pcm_dev->ch1atxbsa);
			RTL_W32(PCM_CH1TXBSA,(pcm_dev->tx1_dma|0x3));
			printk_test("PCM_CH1TXBSA = 0x%x   \n",RTL_R32(PCM_CH1TXBSA));
			for (i=0;i<PCM_BUF_SIZE;i++)
			{
				 *(xmit_tmp+i)= (tx_data_temp << (i%28))&0xFFFEFFFE; //external loop back has to ignore bit 0 and bit 16
				  //*(xmit_tmp+i)= ()&0xFFFEFFFE; //external loop back has to ignore bit 0 and bit 16
				//print_ft2("tx_data[%d]=0x%08x \n",i,*(xmit_tmp+i));
			}
			xmit_tmp=pcm_dev->ch1arxbsa;
			RTL_W32(PCM_CH1RXBSA,(pcm_dev->rx1_dma|0x3));			
			printk_test("PCM_CH1RXBSA = 0x%x   \n",RTL_R32(PCM_CH1RXBSA));
			pcm_tx_rx_enable |= (3<<16);
		break;
		case 2:
			xmit_tmp=pcm_dev->ch2atxbsa;
			printk_test("xmit_tmp=%p ,pcm_dev->ch0atxbsa=%p \n",xmit_tmp,pcm_dev->ch2atxbsa);
			RTL_W32(PCM_CH2TXBSA,(pcm_dev->tx2_dma|0x3));
			printk_test("PCM_CH2TXBSA = 0x%x   \n",RTL_R32(PCM_CH2TXBSA));
			for (i=0;i<PCM_BUF_SIZE;i++)
			{
				 *(xmit_tmp+i)= (tx_data_temp << (i%28))&0xFFFEFFFE; //external loop back has to ignore bit 0 and bit 16
				  //*(xmit_tmp+i)= ()&0xFFFEFFFE; //external loop back has to ignore bit 0 and bit 16
				//print_ft2("tx_data[%d]=0x%08x \n",i,*(xmit_tmp+i));
			}
			xmit_tmp=pcm_dev->ch2arxbsa;
			RTL_W32(PCM_CH2RXBSA,(pcm_dev->rx2_dma|0x3));			
			printk_test("PCM_CH2RXBSA = 0x%x   \n",RTL_R32(PCM_CH2RXBSA));
			pcm_tx_rx_enable |= (3<<8);
		break;
		case 3:
			xmit_tmp=pcm_dev->ch3atxbsa;
			printk_test("xmit_tmp=%p ,pcm_dev->ch0atxbsa=%p \n",xmit_tmp,pcm_dev->ch3atxbsa);
			RTL_W32(PCM_CH3TXBSA,(pcm_dev->tx3_dma|0x3));
			printk_test("PCM_CH3TXBSA = 0x%x   \n",RTL_R32(PCM_CH3TXBSA));
			for (i=0;i<PCM_BUF_SIZE;i++)
			{
				 *(xmit_tmp+i)= (tx_data_temp << (i%28))&0xFFFEFFFE; //external loop back has to ignore bit 0 and bit 16
				  //*(xmit_tmp+i)= ()&0xFFFEFFFE; //external loop back has to ignore bit 0 and bit 16
				//print_ft2("tx_data[%d]=0x%08x \n",i,*(xmit_tmp+i));
			}
			xmit_tmp=pcm_dev->ch3arxbsa;
			RTL_W32(PCM_CH3RXBSA,(pcm_dev->rx3_dma|0x3));			
			printk_test("PCM_CH3RXBSA = 0x%x   \n",RTL_R32(PCM_CH3RXBSA));
			pcm_tx_rx_enable |= (3);
		break;
	}
	
	dump_txrx_data(pcm_dev,pcm_dev->active_channel); 
	/*PCM TX/RX enable*/
	wmb();
	//RTL_W32(PCM_PCSCR,0x03000000);
	RTL_W32(PCM_PCSCR,pcm_tx_rx_enable);
	switch(pcm_ch)
	{							//check page0 and page1 owned by CPU
		case 0:
			while(1)
				if((*(volatile unsigned int *)PCM_CH0RXBSA&0x3)==0)
					break;
			while(1)
				if((*(volatile unsigned int *)PCM_CH0TXBSA&0x3)==0)
					break;
		break;
		case 1:
			while(1)
				if((*(volatile unsigned int *)PCM_CH1RXBSA&0x3)==0)
					break;
			while(1)
				if((*(volatile unsigned int *)PCM_CH1TXBSA&0x3)==0)
					break;
		break;
		case 2:
			while(1)
				if((*(volatile unsigned int *)PCM_CH2RXBSA&0x3)==0)
					break;
			while(1)
				if((*(volatile unsigned int *)PCM_CH2TXBSA&0x3)==0)
					break;
		break;
		case 3:
			while(1)
				if((*(volatile unsigned int *)PCM_CH3RXBSA&0x3)==0)
					break;
			while(1)
				if((*(volatile unsigned int *)PCM_CH3TXBSA&0x3)==0)
					break;
		break;
	}	
	
	wmb();
	/*compare Tx/Rx data*/

	dump_txrx_data(pcm_dev,pcm_dev->active_channel); 
	if( compare_txrx_data(pcm_dev , pcm_dev->active_channel) == 1)
	{
		printk_test("P_PCM%d\n",pcm_dev->active_channel);
		result[pcm_dev->active_channel] = 1;
	}
	else
	{
		printk_test("F_PCM%d\n",pcm_dev->active_channel);
		result[pcm_dev->active_channel] = 0;

	}

   	re8670_pcm_close(pcm_dev);						//PCM Reset default and disable module  
   	return 1;


	
}

static void re8670_pcm_init_hw (struct pcm_device *cp)
{
	u32 regValue;
	/*enable PCM module */
	regValue = RTL_R32(0xbb023018);
	regValue |= (1<<18);
	RTL_W32(0xbb023018 , regValue);
	RTL_W32(0xB8000600 , RTL_R32(0xB8000600) | (1<<2));
	/*Disable PCM TX/RX*/
	RTL_W32(PCM_PCSCR , 0);
	/*PCM General Control Register: linear mode,enable PCM*/
	RTL_W32(PCM_PGCR,( LINEAR_MODE | PCM_AE ));
	/*PCM interface Channel0-3 Time slot Assignment Register*/
	RTL_W32(PCM_PTSCR,0x00020406);
	/*PCM interface Buffer Size Register*/
	RTL_W32(PCM_PBSIZE , (0xfe << 24) | (0xfe << 16) | (0xfe << 8) | 0xfe);

}

static void re8670_pcm_close (struct pcm_device *cp)
{
	u32 regValue;	

	regValue = RTL_R32(0xbb023018) & ~((u32)1 << 18);		//disable PCM module.
	RTL_W32(0xbb023018,regValue);	
	//regValue = RTL_R32(0xb8000300) & ~((u32)1 << 4);		//de-select PCM function.
	//RTL_W32(0xb8000300,regValue);
	RTL_W32(PCM_PGCR,0);
	RTL_W32(PCM_PCSCR,0);
	RTL_W32(PCM_PTSCR,0);
	RTL_W32(PCM_PBSIZE,0);	
	kfree(cp);
	pcm_free_buf(cp);	
}

static int pcm_buf_alloc(struct pcm_device *cp)
{
	unsigned	pcm_ch=0;
	int i;
	pcm_ch=cp->active_channel;

	if(pcm_ch==0)
	{
		cp->ch0arxbsa = dma_alloc_coherent(NULL , PCM_BUF_SIZE*sizeof(long) , &(cp->rx0_dma), GFP_KERNEL);
		cp->ch0atxbsa = dma_alloc_coherent(NULL , PCM_BUF_SIZE*sizeof(long) , &(cp->tx0_dma), GFP_KERNEL);
		for(i=0;i<PCM_BUF_SIZE;i++)
		{
			cp->ch0arxbsa[i] = 0xffffffff;
			cp->ch0atxbsa[i] = 0x0;
		}
	}
	else if(pcm_ch==1)
	{
		cp->ch1arxbsa = dma_alloc_coherent(NULL , PCM_BUF_SIZE*sizeof(long) , &(cp->rx1_dma), GFP_KERNEL);
		cp->ch1atxbsa = dma_alloc_coherent(NULL , PCM_BUF_SIZE*sizeof(long) , &(cp->tx1_dma), GFP_KERNEL);
		for(i=0;i<PCM_BUF_SIZE;i++)
		{
			cp->ch1arxbsa[i] = 0xffffffff;
			cp->ch1atxbsa[i] = 0x0;
		}
	}
	else if(pcm_ch==2)
	{
		cp->ch2arxbsa = dma_alloc_coherent(NULL , PCM_BUF_SIZE*sizeof(long) , &(cp->rx2_dma), GFP_KERNEL);
		cp->ch2atxbsa = dma_alloc_coherent(NULL , PCM_BUF_SIZE*sizeof(long) , &(cp->tx2_dma), GFP_KERNEL);
		for(i=0;i<PCM_BUF_SIZE;i++)
		{
			cp->ch2arxbsa[i] = 0xffffffff;
			cp->ch2atxbsa[i] = 0x0;
		}
	}
	else if(pcm_ch==3)
	{
		cp->ch3arxbsa = dma_alloc_coherent(NULL , PCM_BUF_SIZE*sizeof(long) , &(cp->rx3_dma), GFP_KERNEL);
		cp->ch3atxbsa = dma_alloc_coherent(NULL , PCM_BUF_SIZE*sizeof(long) , &(cp->tx3_dma), GFP_KERNEL);
		for(i=0;i<PCM_BUF_SIZE;i++)
		{
			cp->ch3arxbsa[i] = 0xffffffff;
			cp->ch3atxbsa[i] = 0x0;
		}
	}
	return (1);
}

static void pcm_free_buf (struct pcm_device *cp)
{
	//print_ft2("free allocate memory .......\n");
	unsigned	pcm_ch=0;
	pcm_ch=cp->active_channel;
	if(pcm_ch==0)
	{	
		dma_free_coherent(NULL , PCM_BUF_SIZE*sizeof(long) , (void *)cp->ch0atxbsa , cp->tx0_dma);
		cp->ch0atxbsa = NULL;
		dma_free_coherent(NULL , PCM_BUF_SIZE*sizeof(long) , (void *)cp->ch0arxbsa , cp->rx0_dma);
		cp->ch0arxbsa = NULL;
	}
	else if(pcm_ch==1)
	{	
		dma_free_coherent(NULL , PCM_BUF_SIZE*sizeof(long) , (void *)cp->ch1atxbsa , cp->tx1_dma);
		cp->ch1atxbsa = NULL;
		dma_free_coherent(NULL , PCM_BUF_SIZE*sizeof(long) , (void *)cp->ch1arxbsa , cp->rx1_dma);
		cp->ch1arxbsa = NULL;
	}	
	else if(pcm_ch==2)
	{	
		dma_free_coherent(NULL , PCM_BUF_SIZE*sizeof(long) , (void *)cp->ch2atxbsa , cp->tx2_dma);
		cp->ch2atxbsa = NULL;
		dma_free_coherent(NULL , PCM_BUF_SIZE*sizeof(long) , (void *)cp->ch2arxbsa , cp->rx2_dma);
		cp->ch2arxbsa = NULL;
	}	
	else if(pcm_ch==3)
	{	
		dma_free_coherent(NULL , PCM_BUF_SIZE*sizeof(long) , (void *)cp->ch3atxbsa , cp->tx3_dma);
		cp->ch3atxbsa = NULL;
		dma_free_coherent(NULL , PCM_BUF_SIZE*sizeof(long) , (void *)cp->ch3arxbsa , cp->rx3_dma);
		cp->ch3arxbsa = NULL;
	}		
}

static int pcm_test_read_proc(char *page, char **start, off_t off,
				int count, int *eof, void *data)
{
	int i=0,pcm_result=0;
	char str[128]="";
	uint32 chipId, rev, subType;

	rtk_switch_version_get(&chipId, &rev, &subType);
	switch(subType)
	{
	case APPOLOMP_CHIP_SUB_TYPE_RTL9602B:
		for(i=0;i<2;i++){
			if(!result[i]){
				pcm_result = -1;
			}
		}
		break;
	default:
		for(i=0;i<4;i++){
			if(!result[i]){
				pcm_result = -1;
			}
		}
		break;
	}

	if(pcm_result < 0){
		printk_result(str,"PCM Test: FAILED\n");

	}else{
		printk_result(str,"PCM Test: PASS\n");
	}

	return 0;
}


static int pcm_test_write_proc(struct file *file, const char *buffer,
				unsigned long count, void *data)
{
	char flag;
	uint32 chipId, rev, subType;

	if (count < 2)
		return -EFAULT;

	
	if (buffer && !copy_from_user(&flag, buffer, 1)) {
		if (flag == '1')
		{
			rtk_switch_version_get(&chipId, &rev, &subType);
			switch(subType)
			{	
			case APPOLOMP_CHIP_SUB_TYPE_RTL9602B:
				re8670_pcm_test(0);
				re8670_pcm_test(1);
				break;
			default:
    				re8670_pcm_test(0);
	    			re8670_pcm_test(1);
    				re8670_pcm_test(2);
    				re8670_pcm_test(3);
			break;
			}
		}	
		else if(flag == '2')
		{
			int i;
			dma_addr_t tx_dma , rx_dma;
			unsigned char *txbuf , *rxbuf;
			/*enable PCM module */
			RTL_W32(0xB8000600 , RTL_R32(0xB8000600) | (1<<2));
			/*Disable PCM TX/RX*/
			RTL_W32(PCM_PCSCR , 0);
			/*PCM General Control Register: linear mode,enable PCM*/
			RTL_W32(PCM_PGCR,( LINEAR_MODE | PCM_AE | C0ILBE));
			/*PCM interface Channel0-3 Time slot Assignment Register*/
			RTL_W32(PCM_PTSCR,0x00020406);
			/*PCM interface Buffer Size Register*/
			RTL_W32(PCM_PBSIZE , (0x30) << 24);
			txbuf = dma_alloc_coherent(NULL , 0x30 , &tx_dma, GFP_KERNEL);
			rxbuf = dma_alloc_coherent(NULL , 0x30 , &rx_dma, GFP_KERNEL);
			printk_test("txmda = %x , rxdma = %x \n" , tx_dma , rx_dma);
			for(i=0;i<0x30;i++)
			{
				txbuf[i] = i;
				rxbuf[i] = 0xff;
				printk_test("tx[%d] = %X , rx[%d] = %X \n" ,i, txbuf[i] , i,rxbuf[i]);
			}
			RTL_W32(PCM_CH0TXBSA,((((int)txbuf& 0x1fffffff))|0x3));
			RTL_W32(PCM_CH0RXBSA,((((int)rxbuf & 0x1fffffff))|0x3));
			wmb();
			RTL_W32(PCM_PCSCR,0x03000000);
			while(1)
			{
				if((*(volatile unsigned int *)PCM_CH0RXBSA&0x3)==0)
				{
					break;
				}
			}
			for(i=0;i<0x30;i++)
			{
				printk_test("tx[%d] = %X , rx[%d] = %X \n", i , txbuf[i] , i , rxbuf[i]);
			}
			dump_pcm_register();
			dma_free_coherent(NULL, 0x30 , txbuf , tx_dma);
			dma_free_coherent(NULL, 0x30 , rxbuf , rx_dma);
		}
		
		return count;
	}
	else
		return -EFAULT;
	
}


#define SWITCH_BASE 0xbb000000

/*
reg set 0x00014C 0x1
reg set 0x000120 0x20
reg set 0x0000A4 0x76
reg set 0x4 0x8
reg set 0x8 0x1
reg set 0x23010 0x2560000
*/

void rgPortInit(void)
{
	RTL_W32(SWITCH_BASE | 0x00014C,0x1);
	RTL_W32(SWITCH_BASE | 0x000120,0x20);
	RTL_W32(SWITCH_BASE | 0x0000A4,0x76);
	RTL_W32(SWITCH_BASE | 0x4,0x8);
	RTL_W32(SWITCH_BASE | 0x8,0x1);
	RTL_W32(SWITCH_BASE | 0x23010,0x2560000);
}


/*****************************************************
*
* PRBS Test Related fuction
*
*****************************************************/

#define PRBS_STATUS 0x22108
#define PRBS_ERR	 0x22104
#define PRBS_EN		 0x22100
#define PRBS_SEL	 0x220fc

static int prbs_test_read_proc(char *page, char **start, off_t off,
				int count, int *eof, void *data)
{
	unsigned int status,err ;
	char str[128]="";
	err = SWITCH_BASE | PRBS_ERR;
	status = SWITCH_BASE | PRBS_STATUS;
	//printk_test("status %x, err %x\n", RTL_R32(status),RTL_R32(err));
	if(prbs < 0 || RTL_R32(err)!=0){
		printk_result(str,"PRBS Test: FAILED\n");
	}else{
		printk_result(str,"PRBS Test: PASS\n");
	}
	return 0;
}


static int prbs_set(unsigned int value){

	unsigned int addr;
	unsigned int data;
	unsigned int status,err ;
	err = SWITCH_BASE | PRBS_ERR;
	status = SWITCH_BASE | PRBS_STATUS;

	addr = SWITCH_BASE | PRBS_EN;
	RTL_W32(addr,0x1);
	data = 0xc000 | value; 
	addr = SWITCH_BASE | PRBS_SEL;
	RTL_W32(addr,data);
	msleep(100);
	data = 0x4000 | value; 
	RTL_W32(addr,data);
	addr = SWITCH_BASE | PRBS_STATUS;

	if(RTL_R32(addr)==0){
		return -1;
	}
	/*clear & restart*/
	data = 0xc000 | value; 
	addr = SWITCH_BASE | PRBS_SEL;
	RTL_W32(addr,data);
	msleep(100);
	data = 0x4000 | value; 
	RTL_W32(addr,data);
	printk_test("status %x, err %x\n", RTL_R32(status),RTL_R32(err));
	return 0;
}


static int prbs_test_write_proc(struct file *file, const char *buffer,
				unsigned long count, void *data)
{
	char flag[64];
	int ret,sel=0;

	if (count < 2)
		return -EFAULT;

	 prbs = 0;
	if (buffer && !copy_from_user(&flag, buffer, sizeof(flag))) {
		
		sel = simple_strtol(flag, NULL, 0);
		
		printk_test("Test PRBS %d\n",sel);
		switch(sel){
		case 3:
		{
			/*for prbs test*/
			if((ret = prbs_set(0x66)) <0)
			{
				goto prbs_err;
			}
		}	
		break;
		case 7:
		{
			/*for prbs test*/
			if((ret = prbs_set(0x77)) <0)
			{
				goto prbs_err;
			}
		
		}
		break;
		case 15:
		{
			/*for prbs test*/
			if((ret = prbs_set(0x88)) <0)
			{
				goto prbs_err;
			}
		
		}
		break;
		case 23:
		{
			/*for prbs test*/
			if((ret = prbs_set(0x99)) <0)
			{
				goto prbs_err;
			}
		
		}
		break;
		case 31:		
		{
			/*for prbs test*/
			if((ret = prbs_set(0xaa)) <0)
			{
				goto prbs_err;
			}
		
		}
		break;
		default:
			goto prbs_err;
		break;
		}
		return count;
	}
	else
	{
prbs_err:
		printk_test("prbs set failed!\n");
		prbs = -1;
		return -EFAULT;
	}
}

/*****************************************************
*
* UTP Test Related fuction
*
*****************************************************/
#define SPG_GLB_CTRL 		SWITCH_BASE | 0x2320c
#define SPG_PORT_TX_CTRL 	SWITCH_BASE | 0x20050
#define SPG_PORT_STS		SWITCH_BASE | 0x20054
#define SPG_P_TX_GRP_CTRL   SWITCH_BASE | 0x20058
#define SPG_P_LEN_CTRL		SWITCH_BASE | 0x2005c
#define SPG_P_TX_COUNT      SWITCH_BASE | 0x20060
#define SPG_P_TX_SA     		SWITCH_BASE | 0x20064
#define SPG_P_TX_DA     		SWITCH_BASE | 0x2006C
#define SPG_P_RX_CHECK       SWITCH_BASE | 0x326ac


static int utpPktCount = 100000;
static int utpState=0;

static const int mdiOpenLpArray[230][2]=
{{0x00010,0x0020a420},  
{0x00010,0x0021a420},   
{0x00010,0x0022a420},   
{0x00010,0x0023a420},   
{0x00010,0x0024a420},   
{0x00088,0x00000001},   
{0x00010,0x0020a420},   
{0x00010,0x0021a420},   
{0x00010,0x0022a420},   
{0x00010,0x0023a420},   
{0x00010,0x0024a420},   
{0x00170,0x00000002},   
{0x2d8f8,0xffffffff},   
{0x2d8fc,0xffffffff},   
{0x0014c,0x00000001},   
{0x00004,0x00000004},   
{0x13100,0x0000007f},   
{0x2de44,0x00000001},   
{0x2dd08,0x00000001},   
{0x2006c,0x00000010},   
{0x20070,0x00000000},   
{0x20064,0x00000000},   
{0x20068,0x00000000},   
{0x2046c,0x00000011},   
{0x20470,0x00000000},   
{0x20464,0x00000001},   
{0x20468,0x00000000},   
{0x2086c,0x00000012},   
{0x20870,0x00000000},   
{0x20864,0x00000002},   
{0x20868,0x00000000},   
{0x20c6c,0x00000013},   
{0x20c70,0x00000000},   
{0x20c64,0x00000003},   
{0x20c68,0x00000000},   
{0x2106c,0x00000014},   
{0x21070,0x00000000},   
{0x21064,0x00000004},   
{0x21068,0x00000000},   
{0x2146c,0x00000015},   
{0x21470,0x00000000},   
{0x21464,0x00000005},   
{0x21468,0x00000000},   
{0x2005c,0x00400040},   
{0x2045c,0x00400040},   
{0x2085c,0x00400040},   
{0x20c5c,0x00400040},   
{0x2105c,0x00400040},   
{0x2145c,0x00400040},   
{0x0000c,0x00000033},   
{0x00010,0x0060a4a6},   
{0x0000c,0x00000033},   
{0x00010,0x0061a4a6},   
{0x0000c,0x00000033},   
{0x00010,0x0062a4a6},   
{0x0000c,0x00000033},   
{0x00010,0x0063a4a6},   
{0x0000c,0x00000033},   
{0x00010,0x0064a4a6},   
{0x0000c,0x000040e4},   
{0x00010,0x0060a586},   
{0x0000c,0x000040e4},   
{0x00010,0x0061a586},   
{0x0000c,0x000040e4},   
{0x00010,0x0062a586},   
{0x0000c,0x000040e4},   
{0x00010,0x0063a586},   
{0x0000c,0x000040e4},   
{0x00010,0x0064a586},   
{0x0000c,0x0000809d},   
{0x00010,0x0060a436},   
{0x0000c,0x0000809d},   
{0x00010,0x0061a436},   
{0x0000c,0x0000809d},   
{0x00010,0x0062a436},   
{0x0000c,0x0000809d},   
{0x00010,0x0063a436},   
{0x0000c,0x0000809d},   
{0x00010,0x0064a436},   
{0x0000c,0x00000a08},   
{0x00010,0x0060a438},   
{0x0000c,0x00000a08},   
{0x00010,0x0061a438},   
{0x0000c,0x00000a08},   
{0x00010,0x0062a438},   
{0x0000c,0x00000a08},   
{0x00010,0x0063a438},   
{0x0000c,0x00000a08},   
{0x00010,0x0064a438},   
{0x0000c,0x000080a6},   
{0x00010,0x0060a436},   
{0x0000c,0x000080a6},   
{0x00010,0x0061a436},   
{0x0000c,0x000080a6},   
{0x00010,0x0062a436},   
{0x0000c,0x000080a6},   
{0x00010,0x0063a436},   
{0x0000c,0x000080a6},   
{0x00010,0x0064a436},   
{0x0000c,0x00000a08},   
{0x00010,0x0060a438},   
{0x0000c,0x00000a08},   
{0x00010,0x0061a438},   
{0x0000c,0x00000a08},   
{0x00010,0x0062a438},   
{0x0000c,0x00000a08},   
{0x00010,0x0063a438},   
{0x0000c,0x00000a08},   
{0x00010,0x0064a438},   
{0x0000c,0x000080af},   
{0x00010,0x0060a436},   
{0x0000c,0x000080af},   
{0x00010,0x0061a436},   
{0x0000c,0x000080af},   
{0x00010,0x0062a436},   
{0x0000c,0x000080af},   
{0x00010,0x0063a436},   
{0x0000c,0x000080af},   
{0x00010,0x0064a436},   
{0x0000c,0x00000a08},   
{0x00010,0x0060a438},   
{0x0000c,0x00000a08},   
{0x00010,0x0061a438},   
{0x0000c,0x00000a08},   
{0x00010,0x0062a438},   
{0x0000c,0x00000a08},   
{0x00010,0x0063a438},   
{0x0000c,0x00000a08},   
{0x00010,0x0064a438},   
{0x0000c,0x000008c3},   
{0x00010,0x0060a80e},   
{0x0000c,0x000008c3},   
{0x00010,0x0061a80e},   
{0x0000c,0x000008c3},   
{0x00010,0x0062a80e},   
{0x0000c,0x000008c3},   
{0x00010,0x0063a80e},   
{0x0000c,0x000008c3},   
{0x00010,0x0064a80e},   
{0x0000c,0x0000a40d},   
{0x00010,0x0060bcc0},   
{0x0000c,0x0000a40d},   
{0x00010,0x0061bcc0},   
{0x0000c,0x0000a40d},   
{0x00010,0x0062bcc0},   
{0x0000c,0x0000a40d},   
{0x00010,0x0063bcc0},   
{0x0000c,0x0000a40d},   
{0x00010,0x0064bcc0},   
{0x0000c,0x0000118b},   
{0x00010,0x0060a430},   
{0x0000c,0x0000118b},   
{0x00010,0x0061a430},   
{0x0000c,0x0000118b},   
{0x00010,0x0062a430},   
{0x0000c,0x0000118b},   
{0x00010,0x0063a430},   
{0x0000c,0x0000118b},   
{0x00010,0x0064a430},   
{0x0000c,0x00000e00},   
{0x00010,0x0060a412},   
{0x0000c,0x00000e00},   
{0x00010,0x0061a412},   
{0x0000c,0x00000e00},   
{0x00010,0x0062a412},   
{0x0000c,0x00000e00},   
{0x00010,0x0063a412},   
{0x0000c,0x00000e00},   
{0x00010,0x0064a412},   
{0x0000c,0x000040a0},   
{0x00010,0x0060bce8},   
{0x0000c,0x000040a0},   
{0x00010,0x0061bce8},   
{0x0000c,0x000040a0},   
{0x00010,0x0062bce8},   
{0x0000c,0x000040a0},   
{0x00010,0x0063bce8},   
{0x0000c,0x000040a0},   
{0x00010,0x0064bce8},   
{0x0000c,0x0000158b},   
{0x00010,0x0060a430},   
{0x0000c,0x0000158b},   
{0x00010,0x0061a430},   
{0x0000c,0x0000158b},   
{0x00010,0x0062a430},   
{0x0000c,0x0000158b},   
{0x00010,0x0063a430},   
{0x0000c,0x0000158b},   
{0x00010,0x0064a430},   
{0x0000c,0x00008011},   
{0x00010,0x0060a436},   
{0x0000c,0x00008011},   
{0x00010,0x0061a436},   
{0x0000c,0x00008011},   
{0x00010,0x0062a436},   
{0x0000c,0x00008011},   
{0x00010,0x0063a436},   
{0x0000c,0x00008011},   
{0x00010,0x0064a436},   
{0x0000c,0x0000e37f},   
{0x00010,0x0060a438},   
{0x0000c,0x0000e37f},   
{0x00010,0x0061a438},   
{0x0000c,0x0000e37f},   
{0x00010,0x0062a438},   
{0x0000c,0x0000e37f},   
{0x00010,0x0063a438},   
{0x0000c,0x0000e37f},   
{0x00010,0x0064a438},   
{0x0000c,0x0000178b},   
{0x00010,0x0060a430},   
{0x0000c,0x0000178b},   
{0x00010,0x0061a430},   
{0x0000c,0x0000178b},   
{0x00010,0x0062a430},   
{0x0000c,0x0000178b},   
{0x00010,0x0063a430},   
{0x0000c,0x0000178b},   
{0x00010,0x0064a430},   
{0x0000c,0x00001240},   
{0x00010,0x0060a400},   
{0x0000c,0x00001240},   
{0x00010,0x0061a400},   
{0x0000c,0x00001240},   
{0x00010,0x0062a400},   
{0x0000c,0x00001240},   
{0x00010,0x0063a400},   
{0x0000c,0x00001240},   
{0x00010,0x0064a400}} ;


static openLoopBackEnable(void)
{
	int i,j;
	unsigned int addr;

	for(i=0;i<230;i++)
	{
		addr = SWITCH_BASE | mdiOpenLpArray[i][0];
		RTL_W32(addr,mdiOpenLpArray[i][1]);
	}
}

static void utp_foce_tx_set(int state,unsigned int packetCount)
{
	int i;
	unsigned int addr;
	/*SPG_GLB_CTRL*/
	switch(state){
	case 2: /*continue test, for burning test*/
		/*enable open loopback*/
		openLoopBackEnable();
		/*continue mode set*/
		RTL_W32(SPG_GLB_CTRL,0);
		/*per port setting*/
		for(i=0;i<6;i++)
		{
			addr = SPG_P_TX_GRP_CTRL |  (0x400*i);
			RTL_W32(addr,0x10);
			addr = SPG_P_LEN_CTRL |  (0x400*i);
			RTL_W32(addr,0x5EE0040);
			addr = SPG_PORT_TX_CTRL |  (0x400*i);
			RTL_W32(addr,0x1);
			addr = SPG_P_TX_GRP_CTRL |  (0x400*i);
			RTL_W32(addr,0xD35);
			addr = SPG_P_TX_SA | (0x400*i);
			RTL_W32(addr,0xffffffff);
			addr+=4;
			RTL_W32(addr,0xffffffff);
			addr = SPG_P_TX_DA | (0x400*i);
			RTL_W32(addr,0xffffffff);
			addr+=4;
			RTL_W32(addr,0xffffffff);
		}
		RTL_W32(SPG_GLB_CTRL,0x1);
	break;
	case 1: /*packet count test, for function test*/

		RTL_W32(SPG_GLB_CTRL,0);
		/*per port setting*/
		for(i=0;i<6;i++)
		{
			addr = SPG_P_TX_GRP_CTRL |  (0x400*i);
			RTL_W32(addr,0x10);
			addr = SPG_P_LEN_CTRL 	 |  (0x400*i);
			RTL_W32(addr,0);
			addr = SPG_PORT_TX_CTRL  |  (0x400*i);
			RTL_W32(addr,0);
	
		}
		/*for rg port tx packet count*/
		addr = SPG_P_LEN_CTRL | 0x400*5;
		RTL_W32(addr,0x5EE0040);
		addr = SPG_PORT_TX_CTRL | 0x400*5;
		RTL_W32(addr,0x1);
		addr = SPG_P_TX_COUNT | 0x400*5;
		RTL_W32(addr,packetCount);
		addr = SPG_P_TX_GRP_CTRL | 0x400*5;
		RTL_W32(addr,0xD31);	
		addr = SPG_P_TX_SA | (0x400*5);
		RTL_W32(addr,0xffff);
		addr+=4;
		RTL_W32(addr,0xffffffff);
		addr = SPG_P_TX_DA | (0x400*5);
		RTL_W32(addr,0xffff);
		addr+=4;
		RTL_W32(addr,0xffffffff);
		addr = SPG_GLB_CTRL;
		RTL_W32(addr,0x1);
	break;
	case 0: /*normal*/
		RTL_W32(SPG_P_LEN_CTRL,0);
		/*per port setting*/
		for(i=0;i<6;i++)
		{
			addr = SPG_P_TX_GRP_CTRL | (0x400*i);
			RTL_W32(addr,0x10);
			addr = SPG_P_LEN_CTRL    |  (0x400*i);
			RTL_W32(addr,0);
			addr = SPG_PORT_TX_CTRL |  (0x400*i);
			RTL_W32(addr,0);
	
		}
	break;
	default:	
		printk_test("gpio set failed!\n");
	break;
	}
	
}


static int utp_foce_rx_get(int port,int pktCounter)
{
	unsigned int data;
	unsigned int addr = SPG_P_RX_CHECK;

	data = RTL_R32(addr);

	if(data==pktCounter)
		return 1;
	else
		return 0;	
}


static int utp_test_read_proc(char *page, char **start, off_t off,
				int count, int *eof, void *data)
{
	unsigned result;
	char str[128]="";
	result = utp_foce_rx_get(5,utpPktCount);

	if(result == 1 || utpState!=1){
		printk_result(str,"UTP Test: PASS\n");
	}else
	{
		printk_result(str,"UTP Test: FAILED\n");
	}
	return 0;
}

static int utp_test_write_proc(struct file *file, const char *buffer,
				unsigned long count, void *data)
{
	char flag[64];
	int state;
	if (count < 2)
		return -EFAULT;

	
	if (buffer && !copy_from_user(&flag, buffer, sizeof(flag))) {
		utpState = simple_strtol(flag,NULL, 0);
		utp_foce_tx_set(utpState,utpPktCount);
		return count;
	}
	return 0;
}

/*****************************************************
*
* GPIO Test Related fuction
*
*****************************************************/

/*HW should mapping group 0 and group 1*/
#define  GPIONUM_PERGROUP  8;
static int currState = 0; 

unsigned int gpioGroup[2][8] = {{22,30,25,29,31,58,37,36},{21,26,28,27,60,59,38,35}};


static void gpio_group_set(int groupId, rtk_gpio_mode_t mode,rtk_enable_t state)
{
	int i;
	int gpioId;
	for(i=0;i<8;i++)
	{
		gpioId = gpioGroup[groupId][i];
		rtk_gpio_mode_set(gpioId,mode);	
		rtk_gpio_state_set(gpioId,state);
		if(mode == GPIO_OUTPUT && state == ENABLED){
			rtk_gpio_databit_set(gpioId,1);
		}
	}
}

static void gpio_group_get(int groupId,uint32 *pData)
{
	int i;
	int gpioId;
	for(i=0;i<8;i++)
	{
		gpioId = gpioGroup[groupId][i];
		rtk_gpio_databit_get(gpioId,&pData[i]);
	}
}

static int gpio_test_read_proc(char *page, char **start, off_t off,
				int count, int *eof, void *data)
{
	int i,gpioId;
	unsigned int result[8];
	int isfailed=0;
	char str[128]="";
	memset(result,0,sizeof(unsigned int)*8);

	switch(currState){
	case 0:
		/*do nothing*/
		goto gpio_pass;
	break;
	case 1:		
		gpio_group_get(1,result);
	break;
	case 2:
		gpio_group_get(0,result);
	default:
	break;
	}
gpio_check:

	for(i=0;i<8;i++)
	{
		if(!result[i]){
			isfailed = 1;
			printk_result(str,"GPIO %d Test: FAILED\n",gpioGroup[currState-1][i]);
		}
	}
gpio_pass:
	if(!isfailed){
		printk_result(str,"GPIO Test: PASS\n");
	}
	return 0;
}

static int gpio_test_write_proc(struct file *file, const char *buffer,
				unsigned long count, void *data)
{
	char flag[64];
	
	if (count < 2)
		return -EFAULT;

	
	if (buffer && !copy_from_user(&flag, buffer, sizeof(flag))) {
		currState = simple_strtol(flag,NULL, 0);
		
		switch(currState)
		{
		case 0: /*default setting*/
			gpio_group_set(0,GPIO_INPUT,DISABLED);			
			gpio_group_set(1,GPIO_INPUT,DISABLED);
		break;
		case 1: /*group 0 output, group input*/
			gpio_group_set(0,GPIO_OUTPUT,ENABLED);			
			gpio_group_set(1,GPIO_INPUT,ENABLED);
		break;
		case 2: /*group 0 input, group output*/
			gpio_group_set(0,GPIO_INPUT,ENABLED);			
			gpio_group_set(1,GPIO_OUTPUT,ENABLED);
		break;
		default:
			printk_test("gpio set failed!\n");
		break;
		}
		
		return count;
	}
	return 0;
}


/*****************************************************
*
* CPU Test Related function
*
******************************************************/
static struct task_struct *cpu_tsk;
static int cpuState = 0;

#define DYING_GASP_REG 		SWITCH_BASE | 0x17c
#define DYING_GASP_LED_ON 	RTL_R32(DYING_GASP_REG) | 0x6
#define DYING_GASP_LED_OFF 	RTL_R32(DYING_GASP_REG) | 0x2


static void cpu_gpio_set(int data)
{
	rtk_gpio_mode_set(1,GPIO_OUTPUT);	
	rtk_gpio_state_set(1,ENABLED);
	rtk_gpio_databit_set(1,data);
}

static int cpu_handle(void *arg)
{
	unsigned int timeout;
	int state = 0;
	printk("Start CPU Test\n");
	while(1){
		
		set_current_state(TASK_INTERRUPTIBLE);
		if (kthread_should_stop()) break;
		
		if(state){
			//RTL_W32(DYING_GASP_REG,DYING_GASP_LED_ON);
			cpu_gpio_set(1);
			msleep(100);			
			state = 0;
			
		}else{
			RTL_W32(DYING_GASP_REG,DYING_GASP_LED_OFF);
			cpu_gpio_set(0);
			msleep(100);
			state = 1;
		}
	}	
	printk("Stop CPU Test\n");
	return 0;
}

static int  cpu_state_set(int state)
{
	int ret;

	if(state)
	{
		cpu_tsk = kthread_create(cpu_handle,NULL, "cpu_test");
		if (IS_ERR(cpu_tsk)) {
			ret = PTR_ERR(cpu_tsk);
			cpu_tsk = NULL;
			goto out;
		}
		wake_up_process(cpu_tsk);
	}else{
		kthread_stop(cpu_tsk);
	}

	return 0;

out:
	return ret;
}

static int cpu_test_read_proc(char *page, char **start, off_t off,
				int count, int *eof, void *data)
{

	char str[128]="";
	printk_result(str,"CPU Test State: %s\n",cpuState ? "ENABLE" : "DISABLE" );
	return 0;
}

static int cpu_test_write_proc(struct file *file, const char *buffer,
				unsigned long count, void *data)
{
	char flag[64];
	
	if (count < 2)
		return -EFAULT;

	
	if (buffer && !copy_from_user(&flag, buffer, sizeof(flag))) {
		cpuState = simple_strtol(flag,NULL, 0);
		
		cpu_state_set(cpuState);
		
		return count;
	}
	return 0;
}

/*****************************************************
*
* Read/Write File API
*
******************************************************/
#define FT2_MODE_FILE "/var/ft2Mode"
static struct task_struct *reboot_tsk;

static mm_segment_t oldfs; 


static void ft2_kfsEnvInit(void){ 
   oldfs = get_fs(); 
   set_fs(KERNEL_DS); 
} 

static void ft2_kfsEnvDeinit(){
   set_fs(oldfs); 
}

static struct file *ft2_openFile(char *path,int flag,int mode){ 
   struct file *fp; 
	
   fp=filp_open(path, flag, 0); 
   if (fp) return fp; 
   else return NULL; 
}

static int ft2_writeFile(struct file *fp,char *buf,int readlen) { 
   if (fp->f_op && fp->f_op->read) 
	  return fp->f_op->write(fp,buf,readlen, &fp->f_pos); 
   else 
	  return -1; 
} 

static int ft2_readFile(struct file *fp,char *buf,int readlen) 
{ 
   if (fp->f_op && fp->f_op->read) 
	  return fp->f_op->read(fp,buf,readlen, &fp->f_pos); 
   else 
	  return -1; 
} 

static int ft2_closeFile(struct file *fp) { 
   filp_close(fp,NULL); 
   return 0; 
}

void ft2_write_mode(char *filename,char *mode){
   struct file *fp; 

   ft2_kfsEnvInit(); 
   
   /*write to file*/
   fp = ft2_openFile(filename, O_CREAT | O_WRONLY | O_APPEND, 0); 
   if (fp!= NULL) { 
	  ft2_writeFile(fp, mode, 64);
   }

   ft2_closeFile(fp); 
   
   ft2_kfsEnvDeinit();

}


static int reboot_handle(void *arg)
{
	unsigned int timeout;
	int *timer = (int*)arg;
	printk("Start Reboot Timer\n");
	while(1){
		
		set_current_state(TASK_INTERRUPTIBLE);
		if (kthread_should_stop()) break;
		
		msleep(*timer);
		/*reboot*/
		RTL_W32(0xbb000074,0xff);
	}	
	printk("Stop Reboot Test\n");
	return 0;
}

static int  reboot_timer_start(int timer)
{
	int ret;


	reboot_tsk = kthread_create(reboot_handle,&timer, "reboot_timer");
	if (IS_ERR(reboot_tsk)) {
		ret = PTR_ERR(reboot_tsk);
		reboot_tsk = NULL;
		goto out;
	}
	wake_up_process(reboot_tsk);

	return 0;

out:
	return ret;
}


static int mode_read_proc(char *page, char **start, off_t off,
				int count, int *eof, void *data)
{
	unsigned int addr = 0xb800231c, value;
	char mode[64]="";
	value = RTL_R32(addr);
	value = ( value >> 24 ) & 0xff;
	sprintf(mode,"%s",value==0xff ? "OLT" : "FT");
	printk("Mode: %s,value=%x\n",mode,value);	
	ft2_write_mode(FT2_MODE_FILE,mode);
	return 0;
}

static int mode_write_proc(struct file *file, const char *buffer,
				unsigned long count, void *data)
{
	char flag[64];	
	int timer;
	if (count < 2)
		return -EFAULT;

	
	if (buffer && !copy_from_user(&flag, buffer, sizeof(flag))) {
		timer = simple_strtol(flag,NULL, 0);
		reboot_timer_start(timer);
		return count;
	}
	return 0;
}






/*****************************************************
*
* Others 
*
*****************************************************/




static int log_test_read_proc(char *page, char **start, off_t off,
				int count, int *eof, void *data)
{
	printk("log: %d\n",log);
	return 0;
}

static int log_test_write_proc(struct file *file, const char *buffer,
				unsigned long count, void *data)
{
	char flag[64];

	if (count < 2)
		return -EFAULT;

	
	if (buffer && !copy_from_user(&flag, buffer, sizeof(flag))) {
		log = simple_strtol(flag,NULL, 0);
		return count;
	}
	return 0;
}




int factory_test_init(void)
{

	struct proc_dir_entry *dir = NULL, *pcm = NULL, *prbs=NULL, *log=NULL;	
	struct proc_dir_entry *gpio = NULL, *utp = NULL;
	struct proc_dir_entry *cpu = NULL, *mode=NULL;
	
	printk_test("\n\n factory_test_init \n\n");
	
	dir = proc_mkdir("ft2",NULL);
	
	pcm = create_proc_entry("pcm_test", 0, dir);
	if (pcm) {
		pcm->read_proc = pcm_test_read_proc;
		pcm->write_proc = pcm_test_write_proc;
	}
	else {
		printk_test("pcm_test, create proc failed!\n");
	}

	prbs = create_proc_entry("prbs_test",0,dir);
	if(prbs){
		prbs->read_proc = prbs_test_read_proc;
		prbs->write_proc = prbs_test_write_proc;
	}

	log = create_proc_entry("log",0,dir);	
	if(log){
		log->read_proc = log_test_read_proc;
		log->write_proc = log_test_write_proc;
	}

	utp = create_proc_entry("utp",0,dir);	
	if(utp){
		utp->read_proc = utp_test_read_proc;
		utp->write_proc = utp_test_write_proc;
	}

	gpio = create_proc_entry("gpio",0,dir);	
	if(gpio){
		gpio->read_proc = gpio_test_read_proc;
		gpio->write_proc = gpio_test_write_proc;
	}

	cpu = create_proc_entry("cpu",0,dir);	
	if(cpu){
		cpu->read_proc = cpu_test_read_proc;
		cpu->write_proc =cpu_test_write_proc;
	}
	mode = create_proc_entry("mode",0,dir);	
	if(mode){
		mode->read_proc = mode_read_proc;
		mode->write_proc =mode_write_proc;
	}

	

	rgPortInit();

	return -EFAULT;
}



int factory_test_exit(void)
{
	printk_test("%s(): exist",__FUNCTION__);
	return 0;
}


module_init(factory_test_init);
module_exit(factory_test_exit);

