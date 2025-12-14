/* re8670poll.c - RealTek re8670 Fast Ethernet interface header */

#include <malloc.h>
#include <common.h>
#include "re8670poll.h"

unsigned char descriptor_tx[TX_DESC_NUM*sizeof(NIC_TXFD_T)+256];
unsigned char descriptor_rx[RX_DESC_NUM*sizeof(NIC_RXFD_T)+256];

NIC_TXFD_T *pTxBDPtr;
NIC_RXFD_T *pRxBDPtr;
unsigned int	txBDhead = 0;	// index for system to release buffer
unsigned int	txBDtail = 0;	// index for system to set buf to BD
unsigned int	rxBDtail = 0;	// index for system to set buf to BD
unsigned int regbase = 0xB8012000;

extern int otto_is_apro(void);

int isEthDbgLv(unsigned int eth_dbg_lv){
	unsigned int current = env_get_ulong( "eth_dbg_lv", 16, 0);
	if(current & eth_dbg_lv){
		return 1;
	}
	else{
		return 0;
	}
}

#define SWITCH_BASE 0xbb000000

#define WRITE_MEM32(addr, val)   (*(volatile unsigned int *) (addr)) = (val)
#define WRITE_MEM16(addr, val)   (*(volatile unsigned short *) (addr)) = (val)
#define READ_MEM32(addr)         (*(volatile unsigned int *) (addr))
#define MACReg(offset, val)      (WRITE_MEM32(SWITCH_BASE + offset, val))
#define MACRegR(offset)          (READ_MEM32(SWITCH_BASE+offset))
#define MAX_NUM_OF_LAN_PORT    4
#define MAX_NUM_OF_BUSY_CHECK  0xffff

static int analog_patch_flag=0;
extern struct soc_reg_remap_t soc_reg_remap;

int check_busy(void)
{
    int i;
    unsigned int value;

    for(i = 0; i < MAX_NUM_OF_BUSY_CHECK ; i ++)
    {
        value = MACRegR(0x8);
        if(0 == (value & (1<<16)))
        {
            return 0;
        }
        __udelay(100);
    }

    /* Timeout */
    return 1;
}

static void Lan_RXENABLE_9603cvd(void)
{
    int flag;
    int i, ret;
    unsigned int disPort, value;
    if(0 == analog_patch_flag) {
        //printf("%s %d\n", __func__, __LINE__);
         printf("swcore_init\r\n");

        /* Check SoC register - System Status Register bit 1, RDY_FOR_PATHCH */
        do
        {
            flag = *((volatile unsigned int *)0xb8000044) & 0x2;
        } while (flag == 0);

#ifndef OTTO_GMAC_FPGA
        /* Patch PHY done */
        disPort = (MACRegR(0x23024) & 0x1e) >> 1;
        value = ((MACRegR(0x50) & (~(0xf << 5))) | (disPort << 5));
        MACReg(0x50, value);
        MACReg(0xEC, 0x1);
        __udelay(800000);

        /* MAC force for CPU port */
        MACReg(0x1ac, 0x96);
        MACReg(0x1f0, 0xffff);

        for(i = 0;i < MAX_NUM_OF_LAN_PORT;i ++)
        {
            if(disPort & (1 << i))
            {
                continue;
            }
            MACReg(0x4, 0x20a400 | (i << 16));
            if((ret = check_busy()))
            {
                printf("read timeout\r\n");
            }
            value = MACRegR(0x8);
            value = value & (~0x800);
            MACReg(0x0, value);
            MACReg(0x4, 0x60a400 | (i << 16));
            if((ret = check_busy()))
            {
                printf("write timeout\r\n");
            }
        }
#endif
        *((volatile unsigned int *)0xb8000044) = *((volatile unsigned int *)0xb8000044) | 1;

        analog_patch_flag = 1;
    }
    else
    {
#ifndef OTTO_GMAC_FPGA
        disPort = (MACRegR(0x23024) & 0x1e) >> 1;
        for(i = 0;i < MAX_NUM_OF_LAN_PORT;i ++)
        {
            if(disPort & (1 << i))
            {
                continue;
            }
            MACReg(0x4, 0x20a400 | (i << 16));
            if((ret = check_busy()))
            {
                printf("read timeout\r\n");
            }
            value = MACRegR(0x8);
            value = value & (~0x800);
            MACReg(0x0, value);
            MACReg(0x4, 0x60a400 | (i << 16));
            if((ret = check_busy()))
            {
                printf("write timeout\r\n");
            }
        }

        //cpu port set to force link up
        MACReg(0x1ac, 0x96);
#endif
    }
    /* PHY polling enable */
    MACReg(0xc, 0xa18);
}

void Lan_RXENABLE_apro(void)
{
    int flag;
    int i;
    unsigned int disPort, value;
    if(0 == analog_patch_flag) {
        //printf("%s %d\n", __func__, __LINE__);
         printf("swcore_init\r\n");

        /* Check SoC register - System Status Register bit 1, RDY_FOR_PATHCH */
        do
        {
            flag = *((volatile unsigned int *)0xb8000044) & 0x2;
        } while (flag == 0);

#ifndef OTTO_GMAC_FPGA
        /* Patch PHY done */
        disPort = (MACRegR(0x23024) & 0x3e) >> 1;
        value = ((MACRegR(0x4c) & (~(0x1f << 10))) | (disPort << 10));
        MACReg(0x4c, value);
        MACReg(0x114, 0x1);
        __udelay(800000);

        /* MAC force for CPU port */
        MACReg(0x1f0, 0x96);
        MACReg(0x25c, 0xffff);

        for(i = 0;i < 5;i ++)
        {
            if(disPort & (1 << i))
            {
                continue;
            }
            MACReg(0x0, 0x1140);
            MACReg(0x4, 0x60a400 | (i << 16));
        }

        /* Patch PHY done */
        MACReg(0x114, 0x1);
#endif
        *((volatile unsigned int *)0xb8000044) = *((volatile unsigned int *)0xb8000044) | 1;

        analog_patch_flag = 1;
    }
    else
    {
#ifndef OTTO_GMAC_FPGA
        disPort = (MACRegR(0x23024) & 0x3e) >> 1;
        for(i = 0;i < 5;i ++)
        {
            if(disPort & (1 << i))
            {
                continue;
            }
            MACReg(0x0, 0x1140);
            MACReg(0x4, 0x60a400 | (i << 16));
        }

        //cpu port set to force link up
        MACReg(0x1f0, 0x96);
#endif
    }
}

void Lan_RXENABLE(void) {
	if (otto_is_apro()) {
		Lan_RXENABLE_apro();
	} else {
		Lan_RXENABLE_9603cvd();
	}
	return;
}

void Lan_RXDISABLE_9603cvd(void)
{
#ifndef OTTO_GMAC_FPGA
    int i, ret;
    unsigned int disPort, value;

    disPort = (MACRegR(0x23024) & 0x1e) >> 1;
    for(i = 0;i < MAX_NUM_OF_LAN_PORT;i ++)
    {
        if(disPort & (1 << i))
        {
            continue;
        }
        MACReg(0x4, 0x20a400 | (i << 16));
        if((ret = check_busy()))
        {
            printf("read timeout\r\n");
        }
        value = MACRegR(0x8);
        value = value | 0x800;
        MACReg(0x0, value);
        MACReg(0x4, 0x60a400 | (i << 16));
        if((ret = check_busy()))
        {
            printf("write timeout\r\n");
        }
    }

    //cpu port set to force link down
    MACReg(0x1ac, 0x86);
#endif
}

static void Lan_RXDISABLE_apro(void)
{
#ifndef OTTO_GMAC_FPGA
    int i;
    unsigned int disPort;

    disPort = (MACRegR(0x23024) & 0x3e) >> 1;
    for(i = 0;i < 5;i ++)
    {
        if(disPort & (1 << i))
        {
            continue;
        }
        MACReg(0x0, 0x1940);
        MACReg(0x4, 0x60a400 | (i << 16));
    }

    //cpu port set to force link down
    MACReg(0x1f0, 0x86);
#endif
}

void Lan_RXDISABLE(void) {
	if (otto_is_apro()) {
		Lan_RXDISABLE_apro();
	} else {
		Lan_RXDISABLE_9603cvd();
	}
}
#define _cache_flush (((soc_t *)(0x9f000020))->bios).dcache_writeback_invalidate_all

int Lan_Receive(char** ppData, int* pLen)
{
	static int first_in = 1;
	if(isEthDbgLv(ETH_DBG_RX_IN)){
		printf("%s %d\n", __func__, __LINE__);
	}
	EISR = 0xffff;  //reset RDU flag to start rx again

	if(pRxBDPtr[rxBDtail].StsLen & OWN_BIT){
		//printf("%s %d\n", __func__, __LINE__);
		return -1;
	}

	if(isEthDbgLv(ETH_DBG_RX_DESC)){
		unsigned long tmp = (unsigned long)(&pRxBDPtr[rxBDtail]);
		printf("pRxBDPtr[rxBDtail] @ 0x%lx\n", tmp);
		print_buffer(tmp, (void*)tmp, 4, sizeof(NIC_RXFD_T), 4);
	}

	*ppData = (char*)pRxBDPtr[rxBDtail].DataPtr + RX_SHIFT;
	*pLen = (pRxBDPtr[rxBDtail].StsLen & 0xfff) - 4;
#if 1
	if(!first_in){
		unsigned int lastBDprocessed;
		if(rxBDtail ==0)
			lastBDprocessed =  RX_DESC_NUM -1;
		else
			lastBDprocessed = rxBDtail -1;
		pRxBDPtr[lastBDprocessed].StsLen &= ~0xfff;
		pRxBDPtr[lastBDprocessed].StsLen |= RX_DESC_BUFFER_SIZE;
		pRxBDPtr[lastBDprocessed].StsLen |= OWN_BIT;
		_cache_flush();
	}else
	{
		first_in = 0;

	}
#else	
	pRxBDPtr[rxBDtail].StsLen &= ~0xfff;
	pRxBDPtr[rxBDtail].StsLen |= RX_DESC_BUFFER_SIZE;

	pRxBDPtr[rxBDtail].StsLen |= OWN_BIT;
#endif	
	//EthrntRxCPU_Des_Num = rxBDtail;
	if(isEthDbgLv(ETH_DBG_RX_DATA)){
		unsigned long tmp = (unsigned long)(*ppData);
		printf("rx data @ 0x%lx, len %d\n", tmp, *pLen);
		print_buffer(tmp, (void*)tmp, 2, (*pLen)/2, 0);
	}
	rxBDtail++;
	rxBDtail %= RX_DESC_NUM;

	return 0;
}

int Lan_Transmit(void * buff, unsigned int length)
{
	if(isEthDbgLv(ETH_DBG_TX_IN)){
		printf("%s %d\n", __func__, __LINE__);
	}

	if(pTxBDPtr[txBDtail].StsLen & OWN_BIT){
		printf("%s %d\n", __func__, __LINE__);
		return -1;
	}

//#define _cache_flush (((soc_t *)(0x9f000020))->bios).dcache_writeback_invalidate_all
	_cache_flush();
	pTxBDPtr[txBDtail].DataPtr = (unsigned int)buff | UNCACHE_MASK;
	if(length < 60)
		length = 60;
	pTxBDPtr[txBDtail].StsLen &= ~0xfff;
	pTxBDPtr[txBDtail].StsLen |= length;

	// trigger to send
	pTxBDPtr[txBDtail].StsLen |= OWN_BIT|FS_BIT|LS_BIT|(1<<23);
	//tylo, for 8672 fpga, cache write-back
	__asm__ volatile(
			"mtc0 $0,$20\n\t"
			"nop\n\t"
			"nop\n\t"
			"li $8,512\n\t"
			"mtc0 $8,$20\n\t"
			"nop\n\t"
			"nop\n\t"
			"mtc0 $0,$20\n\t"
			"nop"
			: /* no output */
			: /* no input */
			);
	if(isEthDbgLv(ETH_DBG_TX_DESC)){
		unsigned long tmp = (unsigned long)(&pTxBDPtr[txBDtail]);
		printf("pTxBDPtr[txBDtail] @ 0x%lx\n", tmp);
		print_buffer(tmp, (void*)tmp, 4, sizeof(NIC_TXFD_T), 4);
	}

	if(isEthDbgLv(ETH_DBG_TX_DATA)){
		unsigned long tmp = (unsigned long)(buff);
		printf("tx data @ 0x%lx\n", tmp);
		print_buffer(tmp, (void*)tmp, 4, length, 4);
	}

	IO_CMD |= (1<<0);

	#if 0//really need this?
	mdelay(1);
	while(pTxBDPtr[txBDtail].StsLen & OWN_BIT)
		mdelay(1);
	#endif

	/* advance one */
	txBDtail++;
	txBDtail %= TX_DESC_NUM;

	return 0;
}

int Lan_Initialed = 0;

//11/09/05' hrchen, disable NIC
void Lan_Stop(void)
{
	//printf("%s %d\n", __func__, __LINE__);
	CR = 0x01;	 /* Reset */
	while ( CR & 0x1 );
	IO_CMD = 0;
	IO_CMD1 = 0;

	/* Interrupt Register, ISR, IMR */
	EIMR = 0;
	EISR = 0xffff;

	Lan_Initialed = 0;
}

void Lan_WriteMac(char* mac){
	// set MAC address
	unsigned long mac_reg0, mac_reg1;
	//mac_reg0 = (mac[0] << 24) | (mac[1] << 16) | (mac[2] << 8) | (mac[3] << 0);
	//mac_reg1 = (mac[4] << 24) | (mac[5] << 16);
	memcpy(&mac_reg0, mac, 4);
	memcpy(&mac_reg1, &mac[4], 2);
	NIC_ID0 = mac_reg0;
	NIC_ID1 = mac_reg1;
	//printf("%s %d: 0x%x 0x%x\n", __func__, __LINE__, NIC_ID0, NIC_ID1);
}

int Lan_Initialize(char *mac)
{
	int i;

	//printf("%s %d\n", __func__, __LINE__);

	if(Lan_Initialed)
		return 0;

	//printf("%s %d\n", __func__, __LINE__);

	//initIOB();

	Lan_Stop();

	pTxBDPtr = (NIC_TXFD_T *)((((unsigned int)(descriptor_tx+0xff))& 0xffffff00)|UNCACHE_MASK);
	pRxBDPtr = (NIC_RXFD_T *)((((unsigned int)(descriptor_rx+0xff))& 0xffffff00)|UNCACHE_MASK);

	/* setup descriptor */
	RxFDP = (unsigned int)pRxBDPtr;
	RxCDO = 0;
	TxFDP1 = (unsigned int)pTxBDPtr;
	TxCDO1 = 0;
	// init xmt BD
	for(i=0;i<TX_DESC_NUM;i++)
	{
		pTxBDPtr[i].StsLen = 0;
		pTxBDPtr[i].VLan = 0;
		pTxBDPtr[i].DataPtr = 0;
	}
	pTxBDPtr[TX_DESC_NUM-1].StsLen |= EOR_BIT;

	for(i=0;i<RX_DESC_NUM;i++)
	{
		char *pBuf;
		//if ( (pBuf = getIOB()) == 0 ) {
		if ( (pBuf = UNCACHED_MALLOC(RX_DESC_BUFFER_SIZE)) == 0 ) {
			printf("%s %d\n", __func__, __LINE__);
			return -1;
		}

		pRxBDPtr[i].StsLen = OWN_BIT + RX_DESC_BUFFER_SIZE;
		pRxBDPtr[i].VLan = 0;
		//pRxBDPtr[i].DataPtr = (unsigned long) IOB_PKT_PTR(pBuf);
		pRxBDPtr[i].DataPtr = (unsigned int)pBuf;
	}
	pRxBDPtr[RX_DESC_NUM-1].StsLen |= EOR_BIT;
#if 0
	// set MAC address
	{
		unsigned long mac_reg0, mac_reg1;
		//mac_reg0 = (mac[0] << 24) | (mac[1] << 16) | (mac[2] << 8) | (mac[3] << 0);
		//mac_reg1 = (mac[4] << 24) | (mac[5] << 16);
		memcpy(&mac_reg0, mac, 4);
		memcpy(&mac_reg1, &mac[4], 2);
		NIC_ID0 = mac_reg0;
		NIC_ID1 = mac_reg1;
	}
#endif
	/* RCR, don't accept error packet */
	RCR = NoErrAccept;
	//	RCR = NoErrPromiscAccept;

	/* TCR: IFG, Mode */
	TCR = (unsigned long)(TCR_IFG<<TCR_IFG_OFFSET)|(TCR_NORMAL<<TCR_MODE_OFFSET);

	Rx_Pse_Des_Thres = RX_FC_THRESHOLD;

	/* Rx descriptor Size */
	EthrntRxCPU_Des_Num = RX_DESC_NUM-1;

	//RxRingSize = 0x00;	// 16 descriptor
	RxRingSize = RX_DESC_NUM-1;	// 16 descriptor

	/* Flow Control */
	MSR &= ~(TXFCE | RXFCE);

	/* cpu tag control*/
	CPUtagCR = (CTEN_RX | 2<<CT_RSIZE_L | 3<<CT_TSIZE |CT_APPLO |CTPM_8370 | CTPV_8370);
	CPUtag1CR = 0;

	/* Ethernet IO CMD */
	IO_CMD1 = CMD_CONFIG1;
	IO_CMD = CMD_CONFIG;

#if defined(SUPPORT_MULT_UPGRADE) || defined(CONFIG_MULTICAST_UPGRADE)
	MAR0 = 0xffffffff;
	MAR4 = 0xffffffff;
#endif

	Lan_Initialed = 1;
	txBDtail = 0;	// index for system to set buf to BD
	rxBDtail = 0;	// index for system to set buf to BD

	return 0;
}

