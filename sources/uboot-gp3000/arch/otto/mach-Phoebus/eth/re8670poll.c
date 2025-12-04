/* re8670poll.c - RealTek re8670 Fast Ethernet interface header */

#include <malloc.h>
#include <common.h>
#include "re8670poll.h"
#include <misc_setting.h>

unsigned char descriptor_tx[TX_DESC_NUM*sizeof(NIC_TXFD_T)+256];
unsigned char descriptor_rx[RX_DESC_NUM*sizeof(NIC_RXFD_T)+256];

NIC_TXFD_T *pTxBDPtr;
NIC_RXFD_T *pRxBDPtr;
unsigned int	txBDhead = 0;	// index for system to release buffer
unsigned int	txBDtail = 0;	// index for system to set buf to BD
unsigned int	rxBDtail = 0;	// index for system to set buf to BD
unsigned int regbase = 0xB8012000;


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
#define MAX_NUM_OF_PCS_STATE_CHECK  0xff

//#define CONFIG_DCACHE_WB_INV_ALL

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
	if ((OTTO_SID == PLR_SID_APRO) || (OTTO_SID == PLR_SID_APRO_GEN2)) {
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
	if ((OTTO_SID == PLR_SID_APRO_GEN2) || (OTTO_SID == PLR_SID_APRO_GEN2)) {
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
#ifdef CONFIG_DCACHE_WB_INV_ALL
		_cache_flush();
#endif
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

extern void flush_dcache_range(ulong start_addr, ulong stop);
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
#ifdef CONFIG_DCACHE_WB_INV_ALL
	_cache_flush();
#else
	flush_dcache_range((ulong)buff, (ulong)buff+length);
#endif
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

	// Flush cache data to memory. This can avoid cache write back into rx desc buffer and broken tftp image data.
	_cache_flush();

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

#ifndef OTTO_GMAC_FPGA
#define PHY_K_PORT_NUM			4

#define OTP_GPHY_HW_ENABLE  	0x106
#define OTP_GPHY_SW_CHECK    	0x107
#define OTP_PORT0_RC_CAL        	0x110
#define OTP_PORT0_AMP_CAL     	0x111
#define OTP_PORT0_ADO_CAL      	0x112
#define OTP_PORT0_R_CAL          	0x113

#define OTP_GDACIB_10M     0x120
#define OTP_GDACIB_100M   0x122
#define OTP_GDACIB_1000M 0x124


#define GDAC_PORT0_10M 0xBB01F068
#define GDAC_PORT1_10M 0xBB01F06C
#define GDAC_PORT2_10M 0xBB01F070
#define GDAC_PORT3_10M 0xBB01F074

#define GDAC_PORT0_100M 0xBB01F07C
#define GDAC_PORT1_100M 0xBB01F080
#define GDAC_PORT2_100M 0xBB01F084
#define GDAC_PORT3_100M 0xBB01F088

#define GDAC_PORT0_1000M 0xBB01F090
#define GDAC_PORT1_1000M 0xBB01F094
#define GDAC_PORT2_1000M 0xBB01F098
#define GDAC_PORT3_1000M 0xBB01F09C


#define GDACK_MASK 0xfffc0000

#define HW_ENABLE_GPHY_CAL_CMPLE   	0x0010
#define HW_ENABLE_GPHY_SW_PATCH   	0x0100
#define HW_ENABLE_GPHY_EN_ADO_CAL  	0x0001
#define HW_ENABLE_GPHY_EN_RC_CAL  	0x0002
#define HW_ENABLE_GPHY_EN_R_CAL  		0x0004
#define HW_ENABLE_GPHY_EN_AMP_CAL  	0x0008
#define HW_ENABLE_GPHY_EN_DIS_500M  	0x0010
#define HW_ENABLE_EN_GDAC_IB_10M  	0x0020
#define HW_ENABLE_EN_GDAC_IB_100M  	0x0040
#define HW_ENABLE_EN_GDAC_IB_1000M  	0x0080

extern void efuse_write_entry(unsigned short entry, unsigned short value);
extern unsigned short efuse_read_entry(unsigned short entry);

static int Lan_READGPHY_apro(unsigned int phyid, unsigned short OCPAddr, unsigned short *pData)
{
	unsigned int value;
	int ret;

	MACReg(0x4, (1<<21)|((phyid&0x1F)<<16)|(OCPAddr<<0));
	if((ret = check_busy()))
	{
		printf("read timeout\r\n");
		return -1;
	}

	value = (unsigned short)(MACRegR(0x8)&0xffff);
	*pData = value;
    return 0;
}

static int Lan_WRITEGPHY_apro(unsigned int phyid, unsigned short OCPAddr, unsigned short data)
{
    int ret;

	MACReg(0x0, (REG32(0xbb000000)&0xFFFF0000)|data);
	MACReg(0x4, (3<<21)|((phyid&0x1F)<<16)|(OCPAddr<<0));
	if((ret = check_busy()))
	{
		printf("write timeout\r\n");
		return -1;
	}

	return 0;
}

static int Lan_READMIIM_apro(unsigned int port, unsigned int page, unsigned int phyReg, unsigned short *pData)
{
	unsigned int phyid;
    unsigned int ocpAddr;
	int ret;

	phyid = port;

	if(phyReg < 8)
    {
        ocpAddr = 0xa400 + phyReg * 2;
    }
    else if(phyReg < 16)
    {
        ocpAddr = 0xa400 + phyReg * 2;
    }
    else if(phyReg < 24)
    {
        ocpAddr = ((page & 0xfff) << 4) + (phyReg - 16) * 2;
    }
    else if(phyReg < 30)
    {
        ocpAddr = 0xa400 + phyReg * 2;
    }
    else
    {
        return -1;
    }

	ret = Lan_READGPHY_apro(phyid, ocpAddr, pData);
	///--printf("\r\n %s-%d: phyid=%d ocpAddr=0x%x *pData=0x%x", __func__, __LINE__, phyid, ocpAddr, *pData);
	return ret;
}

static int Lan_WRITEMIIM_apro(unsigned int port, unsigned int page, unsigned int phyReg, unsigned short data)
{
	unsigned int phyid;
    unsigned int ocpAddr;
	int ret;

	phyid = port;

	if(phyReg < 8)
    {
        ocpAddr = 0xa400 + phyReg * 2;
    }
    else if(phyReg < 16)
    {
        ocpAddr = 0xa400 + phyReg * 2;
    }
    else if(phyReg < 24)
    {
        ocpAddr = ((page & 0xfff) << 4) + (phyReg - 16) * 2;
    }
    else if(phyReg < 30)
    {
        ocpAddr = 0xa400 + phyReg * 2;
    }
    else
    {
        return -1;
    }

	ret = Lan_WRITEGPHY_apro(phyid, ocpAddr, data);
	///--printf("\r\n %s-%d: phyid=%d ocpAddr=0x%x data=0x%x", __func__, __LINE__, phyid, ocpAddr, data);
	return ret;
}

static int lan_eth_check_pcs_state(unsigned int port)
{
    int i, ret;
    unsigned short miim_data=0;

	for(i = 0 ; i<MAX_NUM_OF_PCS_STATE_CHECK ; i++)
    {
        ret = Lan_READMIIM_apro(port, 0xA60, 16, &miim_data);
		//printf("\r\n %s-%d: ret=%d miim_data=0x%x", __func__, __LINE__, ret, miim_data);
        if(!ret && 0x1 == (miim_data & (0xff)))
        {
            return 0;
        }
        __udelay(100);
    }

    /* Timeout */
    return 1;
}

static int Lan_PHYCAL_check_apro_gen2(void)
{
#if 0
	unsigned short data;
	int port;

	for (port=0 ; port<PHY_K_PORT_NUM ; port++)
	{
		if(Lan_READGPHY_apro(port, 0xbcfc, &data) == 0)
			printf("\n port %d ado=%x   \n",port, data);
		if(Lan_READGPHY_apro(port, 0xbcdc, &data) == 0)
			printf(" port %d RCc=%x   \n",port, data);
		if(Lan_READGPHY_apro(port, 0xbcde, &data) == 0)
			printf(" port %d RCe=%x   \n",port, data);
		if(Lan_READGPHY_apro(port, 0xbce0, &data) == 0)
			printf(" port %d R0=%x   \n",port, data);
		if(Lan_READGPHY_apro(port, 0xbce2, &data) == 0)
			printf(" port %d R2=%x   \n",port, data);
		if(Lan_READGPHY_apro(port, 0xbcac, &data) == 0)
			printf(" port %d AMP=%x \n  ",port, data);
	}
#endif

	return 0;
}

static int Lan_PHYCAL_load_apro_gen2(void)
{
	int port = 0;
	//uint8_t  phy_dev = 0;
	//struct ca_otp_cfg_s otp_cfg;
	//uint16_t adoadj, rlen_val, tapbinrx_pm, ibadj;
	unsigned short tapbinrx_pm,phy_dis_500M;
	unsigned short val_OTP_GPHY_HW_ENABLE,val_OTP_GPHY_SW_CHECK;
	unsigned int gdac_read, gdac_read_high;


	val_OTP_GPHY_HW_ENABLE=efuse_read_entry(OTP_GPHY_HW_ENABLE);
	val_OTP_GPHY_SW_CHECK=efuse_read_entry(OTP_GPHY_SW_CHECK);

	//printf("hw_en=%x sw_check=%x\n",val_OTP_GPHY_HW_ENABLE,val_OTP_GPHY_SW_CHECK);

	if ((val_OTP_GPHY_HW_ENABLE & HW_ENABLE_GPHY_CAL_CMPLE) !=HW_ENABLE_GPHY_CAL_CMPLE)
	{
		//printf("no gphy cal\n");
		return -1;
	}

	if ((val_OTP_GPHY_HW_ENABLE & HW_ENABLE_GPHY_SW_PATCH) != 0)
	{
		//printf("sw_patch!=0\n");
		return -1;
	}

	for (port=0 ; port<PHY_K_PORT_NUM ; port++)
	{
		//printf("OTP OTP_GPHY_SW_CHECK %x=%x\n",OTP_GPHY_SW_CHECK,val_OTP_GPHY_SW_CHECK);
		if((val_OTP_GPHY_SW_CHECK & HW_ENABLE_GPHY_EN_ADO_CAL) == HW_ENABLE_GPHY_EN_ADO_CAL)
		{
			//printf("enable OTP_PORT0_ADO_CAL\n");
			//adoadj=Giga_Eth_ReadPhyMdio_byOCP(port, 0xbcfc);
			Lan_WRITEGPHY_apro(port, 0xbcfc,efuse_read_entry(OTP_PORT0_ADO_CAL+port*4) );
			//printf("port %d adok=%x\n",port,efuse_read_entry(OTP_PORT0_ADO_CAL+port*4) );
		}

		if((val_OTP_GPHY_SW_CHECK & HW_ENABLE_GPHY_EN_RC_CAL) == HW_ENABLE_GPHY_EN_RC_CAL)
		{
			//printf("enable HW_ENABLE_GPHY_EN_RC_CAL\n");
			//len_val=Giga_Eth_ReadPhyMdio_byOCP(port, 0xbcdc);
			//rlen_val=Giga_Eth_ReadPhyMdio_byOCP(port, 0xbcde);
			Lan_WRITEGPHY_apro(port, 0xbcdc,efuse_read_entry(OTP_PORT0_RC_CAL+port*4) );
			Lan_WRITEGPHY_apro(port, 0xbcde,efuse_read_entry(OTP_PORT0_RC_CAL+port*4) );
			//printf("port %d rck=%x\n",port,efuse_read_entry(OTP_PORT0_RC_CAL+port*4) );
		}

		if((val_OTP_GPHY_SW_CHECK & HW_ENABLE_GPHY_EN_R_CAL) == HW_ENABLE_GPHY_EN_R_CAL)
		{
			//printf("enable HW_ENABLE_GPHY_EN_R_CAL\n");
			//tapbintx=Giga_Eth_ReadPhyMdio_byOCP(port, 0xbce0);
			//tapbinrx_pm=Giga_Eth_ReadPhyMdio_byOCP(port, 0xbce2);
			tapbinrx_pm=efuse_read_entry(OTP_PORT0_R_CAL+port*4);
#if 0
			/*for new R cal*/
			tapbinrx_pm|= (tapbinrx_pm << 4)|(tapbinrx_pm << 8)|(tapbinrx_pm << 12);
#endif
			Lan_WRITEGPHY_apro(port, 0xbce0, tapbinrx_pm);
			Lan_WRITEGPHY_apro(port, 0xbce2, tapbinrx_pm);
			//printf("port %d tapbinrx_pm=%x\n",port,tapbinrx_pm);
		}

		if((val_OTP_GPHY_SW_CHECK & HW_ENABLE_GPHY_EN_AMP_CAL) == HW_ENABLE_GPHY_EN_AMP_CAL)
		{
			//printf("enable HW_ENABLE_GPHY_EN_AMP_CAL\n");
			//Giga_Eth_SetPhyMdio_byOCP(port, 0xbcac,ibadj);
			Lan_WRITEGPHY_apro(port, 0xbcac, efuse_read_entry(OTP_PORT0_AMP_CAL+port*4));
			//printf("port %d amp=%x\n",port, efuse_read_entry(OTP_PORT0_AMP_CAL+port*4));
		}

		//printf("0xa4a2=%x\n", phy_dis_500M);
		if((val_OTP_GPHY_SW_CHECK & HW_ENABLE_GPHY_EN_DIS_500M) == HW_ENABLE_GPHY_EN_DIS_500M)
		{
			//printf("enable 	DIS_500M\n");
			if(Lan_READGPHY_apro(port,0xa4a2, &phy_dis_500M) == 0)
			{
				Lan_WRITEGPHY_apro(port, 0xa4a2, phy_dis_500M & 0xfeff);
			}
			if(Lan_READGPHY_apro(port,0xa428, &phy_dis_500M) == 0)
			{
				Lan_WRITEGPHY_apro(port, 0xa428, phy_dis_500M & 0xfdff);
			}
		}
		else
		{
			//printf("enable 	Not DIS_500M\n");
			if(Lan_READGPHY_apro(port,0xa4a2, &phy_dis_500M) == 0)
			{
				Lan_WRITEGPHY_apro(port, 0xa4a2, phy_dis_500M | 0x0100);
			}
			if(Lan_READGPHY_apro(port,0xa428, &phy_dis_500M) == 0)
			{
				Lan_WRITEGPHY_apro(port, 0xa428, phy_dis_500M | 0x0200);
			}
		}
#if 0
		if(Lan_READGPHY_apro(port,0xa4a2, &phy_dis_500M) == 0)
		{
			printf("0xa4a2=%x\n", phy_dis_500M);
		}
		if(Lan_READGPHY_apro(port,0xa428, &phy_dis_500M) == 0)
		{
			printf("0xa428=%x\n", phy_dis_500M);
		}
#endif

		//phy_dis_500M=phy_ocpread(port,0xa4a2);
		//printf("0xa4a2=%x\n",phy_ocpread(port,0xa4a2) );
		//add for GDAC
		if ((val_OTP_GPHY_SW_CHECK & HW_ENABLE_EN_GDAC_IB_10M) == HW_ENABLE_EN_GDAC_IB_10M)
		{
			//printf("valid 10M patch\n");
			gdac_read=REG32(GDAC_PORT0_10M+port*4);
			gdac_read&=GDACK_MASK;
			gdac_read_high=efuse_read_entry(OTP_GDACIB_10M+1)<<16;
			gdac_read =gdac_read+ efuse_read_entry(OTP_GDACIB_10M)+ gdac_read_high;
			REG32(GDAC_PORT0_10M+port*4)=gdac_read;
			//printf("port %d 10M patch	%x=%x\n",port,GDAC_PORT0_10M+port*4,REG32(GDAC_PORT0_10M+port*4));
		}

		if ((val_OTP_GPHY_SW_CHECK & HW_ENABLE_EN_GDAC_IB_100M) == HW_ENABLE_EN_GDAC_IB_100M)
		{
			//printf("valid 100M patch\n");
			gdac_read=REG32(GDAC_PORT0_100M+port*4);
			gdac_read_high=efuse_read_entry(OTP_GDACIB_100M+1)<<16;
			//printf("valid 100M patch gdac_read=%x\n",gdac_read);
			gdac_read&=GDACK_MASK;
			gdac_read =gdac_read+ efuse_read_entry(OTP_GDACIB_100M)+gdac_read_high;
			REG32(GDAC_PORT0_100M+port*4)=gdac_read;
			//printf("port %d 100M patch %x=%x\n",port,GDAC_PORT0_100M+port*4,REG32(GDAC_PORT0_100M+port*4));
		}

		if ((val_OTP_GPHY_SW_CHECK & HW_ENABLE_EN_GDAC_IB_1000M) == HW_ENABLE_EN_GDAC_IB_1000M)
		{
			//printf("valid 1000M patch\n");
			gdac_read=REG32(GDAC_PORT0_1000M+port*4);
			gdac_read_high=efuse_read_entry(OTP_GDACIB_1000M+1)<<16;
			gdac_read&=GDACK_MASK;
			gdac_read =gdac_read+ efuse_read_entry(OTP_GDACIB_1000M)+gdac_read_high;
			REG32(GDAC_PORT0_1000M+port*4)=gdac_read;
			//printf("port %d 1000M patch %x=%x\n",port,GDAC_PORT0_1000M+port*4,REG32(GDAC_PORT0_1000M+port*4));
		}
		//end of GDAC
	}

	return 0;
}

static int Lan_eth_change_default_apro_gen2(void)
{
	unsigned int port;
	unsigned short miim_data=0;
	int ret;

	///--printf("\r\n Lan_eth_change_default_apro_gen2 enter");

	// (A1) set SOC ready
	///--printf("\r\n (A1) set SOC ready");
	REG32(0xB8000044) |=  (1<<0);

	///-- Check result
#if 0
	printf("\r\n REG32(0xB8000044)=0x%x", REG32(0xB8000044));
	printf("\r\n");
#endif

	// (A2) patch GPHY parameter  (in lock main state)
	///--printf("\r\n (A2) patch GPHY parameter");
	// <A2-begin> set lock main state
	///--printf("\r\n <A2-begin> set lock main state");
	for (port=0 ; port<PHY_K_PORT_NUM ; port++)
	{
		ret = Lan_READMIIM_apro(port, 0xA42, 16, &miim_data);
		if (!ret)
		{
			// When Px_phy_status = 0x2 (EXT_INI), no need to set lock main, because we cannot set it successfully.
			if ((miim_data&0x7) != 0x2)
			{
				ret = Lan_READMIIM_apro(port, 0xA46, 21, &miim_data);
				if (!ret)
				{
					// set lock main
					miim_data &= ~(1<<1);
					miim_data |= (1<<1);
					ret = Lan_WRITEMIIM_apro(port, 0xA46, 21, miim_data);
					if (ret)
					{
						printf("\r\n %s-%d: Lan_WRITEMIIM_apro ERROR!", __func__, __LINE__);
						goto ERROR;
					}
				}
				else
				{
					printf("\r\n %s-%d: Lan_READMIIM_apro ERROR!", __func__, __LINE__);
					goto ERROR;
				}

				///-- Check result
#if 0
				ret = Lan_READMIIM_apro(port, 0xA46, 21, &miim_data);
				if (!ret) {
					printf("\r\n Lan_READMIIM_apro(%d, 0xA46, 21)=0x%x", port, miim_data);
				}
#endif

				// check & wait pcs_state = 0x1
				if((ret = lan_eth_check_pcs_state(port)))
				{
					printf("\r\n %s-%d: lan_eth_check_pcs_state timeout.", __func__, __LINE__);
					goto ERROR;
				}
			}
		}
	}

	// (A2-0) enable ADC_bias_bootAutoK
	///--printf("\r\n (A2-0) enable ADC_bias_bootAutoK");
#if 0
	for (port=0 ; port<PHY_K_PORT_NUM ; port++)
	{
		ret = Lan_READMIIM_apro(port, 0xA43, 27, &miim_data);
		if (!ret)
		{
			// SRAM address = 0x8016
			miim_data = 0x8016;
			ret = Lan_WRITEMIIM_apro(port, 0xA43, 27, miim_data);
			if (ret)
			{
				printf("\r\n %s-%d: Lan_WRITEMIIM_apro ERROR!", __func__, __LINE__);
				goto ERROR;
			}
		}
		else
		{
			printf("\r\n %s-%d: Lan_READMIIM_apro ERROR!", __func__, __LINE__);
			goto ERROR;
		}

		ret = Lan_READMIIM_apro(port, 0xA43, 28, &miim_data);
		if (!ret)
		{
			// REG: SRAM address 0x8016 ,bit[10] =1
			miim_data &= ~(1<<10);
			miim_data |= (1<<10);
			ret = Lan_WRITEMIIM_apro(port, 0xA43, 28, miim_data);
			if (ret)
			{
				printf("\r\n %s-%d: Lan_WRITEMIIM_apro ERROR!", __func__, __LINE__);
				goto ERROR;
			}
		}
		else
		{
			printf("\r\n %s-%d: Lan_READMIIM_apro ERROR!", __func__, __LINE__);
			goto ERROR;
		}
	}
#endif

	///-- Check result
#if 0
	for (port=0 ; port<PHY_K_PORT_NUM ; port++)
	{
		ret = Lan_READMIIM_apro(port, 0xA43, 27, &miim_data);
		if (!ret) {
			printf("\r\n Lan_READMIIM_apro(%d, 0xA43, 27)=0x%x", port, miim_data);
		}

		ret = Lan_READMIIM_apro(port, 0xA43, 28, &miim_data);
		if (!ret) {
			printf("\r\n Lan_READMIIM_apro(%d, 0xA43, 28)=0x%x", port, miim_data);
		}
	}
#endif

	// <A2-1> GPHY init setting
	// ## <1>,<2> : disable green , R-tune .
	///--printf("\r\n <A2-1> GPHY init setting");
	for (port=0 ; port<PHY_K_PORT_NUM ; port++)
	{
		ret = Lan_READMIIM_apro(port, 0xA43, 27, &miim_data);
		if (!ret)
		{
			// SRAM address = 0x8011
			miim_data = 0x8011;
			ret = Lan_WRITEMIIM_apro(port, 0xA43, 27, miim_data);
			if (ret)
			{
				printf("\r\n %s-%d: Lan_WRITEMIIM_apro ERROR!", __func__, __LINE__);
				goto ERROR;
			}
		}
		else
		{
			printf("\r\n %s-%d: Lan_READMIIM_apro ERROR!", __func__, __LINE__);
			goto ERROR;
		}

		ret = Lan_READMIIM_apro(port, 0xA43, 28, &miim_data);
		if (!ret)
		{
			// sram_rg01[7]/0x8011 disable green,  sram_rg01[3]/0x8011 disable R-tune.
			miim_data &= ~(1<<15);
			miim_data &= ~(1<<11);
			ret = Lan_WRITEMIIM_apro(port, 0xA43, 28, miim_data);
			if (ret)
			{
				printf("\r\n %s-%d: Lan_WRITEMIIM_apro ERROR!", __func__, __LINE__);
				goto ERROR;
			}
		}
		else
		{
			printf("\r\n %s-%d: Lan_READMIIM_apro ERROR!", __func__, __LINE__);
			goto ERROR;
		}
	}

	///-- Check result
#if 0
	for (port=0 ; port<PHY_K_PORT_NUM ; port++)
	{
		ret = Lan_READMIIM_apro(port, 0xA43, 27, &miim_data);
		if (!ret) {
			printf("\r\n Lan_READMIIM_apro(%d, 0xA43, 27)=0x%x", port, miim_data);
		}

		ret = Lan_READMIIM_apro(port, 0xA43, 28, &miim_data);
		if (!ret) {
			printf("\r\n Lan_READMIIM_apro(%d, 0xA43, 28)=0x%x", port, miim_data);
		}
	}
#endif

	// ##  <3> enable  echo cancellor ec_o_x2 autotune function
	for (port=0 ; port<PHY_K_PORT_NUM ; port++)
	{
		ret = Lan_READMIIM_apro(port, 0xA42, 22, &miim_data);
		if (!ret)
		{
			// enable  echo cancellor ec_o_x2 autotune function 
			miim_data |= (1<<4);
			ret = Lan_WRITEMIIM_apro(port, 0xA42, 22, miim_data);
			if (ret)
			{
				printf("\r\n %s-%d: Lan_WRITEMIIM_apro ERROR!", __func__, __LINE__);
				goto ERROR;
			}
		}
		else
		{
			printf("\r\n %s-%d: Lan_READMIIM_apro ERROR!", __func__, __LINE__);
			goto ERROR;
		}
	}

	///-- Check result
#if 0
	for (port=0 ; port<PHY_K_PORT_NUM ; port++)
	{
		ret = Lan_READMIIM_apro(port, 0xA42, 22, &miim_data);
		if (!ret) {
			printf("\r\n Lan_READMIIM_apro(%d, 0xA42, 22)=0x%x", port, miim_data);
		}
	}
#endif

	// <A2-2> RL6831_patch_AFE_parameter
	///--printf("\r\n <A2-2> RL6831_patch_AFE_parameter");
	for (port=0 ; port<PHY_K_PORT_NUM ; port++)
	{
		ret = Lan_READMIIM_apro(port, 0xBCC, 16, &miim_data);
		if (!ret)
		{
			// 1.   vcm :          pg 0xbcc  / rg 16 / bit[13:12] =0x2
			miim_data &= ~(0x3<<12);
			miim_data |= (0x2<<12);
			ret = Lan_WRITEMIIM_apro(port, 0xBCC, 16, miim_data);
			if (ret)
			{
				printf("\r\n %s-%d: Lan_WRITEMIIM_apro ERROR!", __func__, __LINE__);
				goto ERROR;
			}
		}
		else
		{
			printf("\r\n %s-%d: Lan_READMIIM_apro ERROR!", __func__, __LINE__);
			goto ERROR;
		}

		ret = Lan_READMIIM_apro(port, 0xBCA, 23, &miim_data);
		if (!ret)
		{
			// 2.   ref_ldo:       pg 0xbca  / rg 23 / bit[14:12] =0x4
			miim_data &= ~(0x7<<12);
			miim_data |= (0x4<<12);
			ret = Lan_WRITEMIIM_apro(port, 0xBCA, 23, miim_data);
			if (ret)
			{
				printf("\r\n %s-%d: Lan_WRITEMIIM_apro ERROR!", __func__, __LINE__);
				goto ERROR;
			}
		}
		else
		{
			printf("\r\n %s-%d: Lan_READMIIM_apro ERROR!", __func__, __LINE__);
			goto ERROR;
		}

		ret = Lan_READMIIM_apro(port, 0xBCC, 16, &miim_data);
		if (!ret)
		{
			// 3.   reg_ref:       pg 0xbcc  / rg 16 / bit[10:8]  =0x4
			miim_data &= ~(0x7<<8);
			miim_data |= (0x4<<8);
			ret = Lan_WRITEMIIM_apro(port, 0xBCC, 16, miim_data);
			if (ret)
			{
				printf("\r\n %s-%d: Lan_WRITEMIIM_apro ERROR!", __func__, __LINE__);
				goto ERROR;
			}
		}
		else
		{
			printf("\r\n %s-%d: Lan_READMIIM_apro ERROR!", __func__, __LINE__);
			goto ERROR;
		}

		ret = Lan_READMIIM_apro(port, 0xBCA, 23, &miim_data);
		if (!ret)
		{
			// 4.   adc_sarduty:   pg 0xbca  / rg 23 / bit[1:0]   =0x0
			miim_data &= ~(0x3<<0);
			ret = Lan_WRITEMIIM_apro(port, 0xBCA, 23, miim_data);
			if (ret)
			{
				printf("\r\n %s-%d: Lan_WRITEMIIM_apro ERROR!", __func__, __LINE__);
				goto ERROR;
			}
		}
		else
		{
			printf("\r\n %s-%d: Lan_READMIIM_apro ERROR!", __func__, __LINE__);
			goto ERROR;
		}

		ret = Lan_READMIIM_apro(port, 0xBCA, 23, &miim_data);
		if (!ret)
		{
			// 5.   adc_duty75:    pg 0xbca  / rg 23 / bit[8]     =0x0
			miim_data &= ~(0x1<<8);
			ret = Lan_WRITEMIIM_apro(port, 0xBCA, 23, miim_data);
			if (ret)
			{
				printf("\r\n %s-%d: Lan_WRITEMIIM_apro ERROR!", __func__, __LINE__);
				goto ERROR;
			}
		}
		else
		{
			printf("\r\n %s-%d: Lan_READMIIM_apro ERROR!", __func__, __LINE__);
			goto ERROR;
		}

		ret = Lan_READMIIM_apro(port, 0xBCC, 23, &miim_data);
		if (!ret)
		{
			// 6.   adc_ibset_dn:  pg 0xbcc  / rg 23 / bit[4]     =0x0
			miim_data &= ~(0x1<<4);
			ret = Lan_WRITEMIIM_apro(port, 0xBCC, 23, miim_data);
			if (ret)
			{
				printf("\r\n %s-%d: Lan_WRITEMIIM_apro ERROR!", __func__, __LINE__);
				goto ERROR;
			}
		}
		else
		{
			printf("\r\n %s-%d: Lan_READMIIM_apro ERROR!", __func__, __LINE__);
			goto ERROR;
		}

		ret = Lan_READMIIM_apro(port, 0xBCD, 19, &miim_data);
		if (!ret)
		{
			// 7.   adc_ibset:     pg 0xbcd  / rg 19 / bit[2:0]   =0x1
			miim_data &= ~(0x7<<0);
			miim_data |= (0x1<<0);
			ret = Lan_WRITEMIIM_apro(port, 0xBCD, 19, miim_data);
			if (ret)
			{
				printf("\r\n %s-%d: Lan_WRITEMIIM_apro ERROR!", __func__, __LINE__);
				goto ERROR;
			}
		}
		else
		{
			printf("\r\n %s-%d: Lan_READMIIM_apro ERROR!", __func__, __LINE__);
			goto ERROR;
		}
	}

	///-- Check result
#if 0
	for (port=0 ; port<PHY_K_PORT_NUM ; port++)
	{
		ret = Lan_READMIIM_apro(port, 0xBCC, 16, &miim_data);
		if (!ret) {
			printf("\r\n Lan_READMIIM_apro(%d, 0xBCC, 16)=0x%x", port, miim_data);
		}

		ret = Lan_READMIIM_apro(port, 0xBCA, 23, &miim_data);
		if (!ret) {
			printf("\r\n Lan_READMIIM_apro(%d, 0xBCA, 23)=0x%x", port, miim_data);
		}

		ret = Lan_READMIIM_apro(port, 0xBCA, 23, &miim_data);
		if (!ret) {
			printf("\r\n Lan_READMIIM_apro(%d, 0xBCA, 23)=0x%x", port, miim_data);
		}

		ret = Lan_READMIIM_apro(port, 0xBCC, 23, &miim_data);
		if (!ret) {
			printf("\r\n Lan_READMIIM_apro(%d, 0xBCC, 23)=0x%x", port, miim_data);
		}

		ret = Lan_READMIIM_apro(port, 0xBCD, 19, &miim_data);
		if (!ret) {
			printf("\r\n Lan_READMIIM_apro(%d, 0xBCD, 19)=0x%x", port, miim_data);
		}
	}
#endif

	// <A2-3> RL6831_patch_MP_cal
	///--printf("\r\n <A2-3> RL6831_patch_MP_cal");
	for (port=0 ; port<PHY_K_PORT_NUM ; port++)
	{
		//AMP-cal:
		ret = Lan_READMIIM_apro(port, 0xBCA, 22, &miim_data);
		if (!ret)
		{
			// ibadj (modify)
			miim_data = 0x9999;
			ret = Lan_WRITEMIIM_apro(port, 0xBCA, 22, miim_data);
			if (ret)
			{
				printf("\r\n %s-%d: Lan_WRITEMIIM_apro ERROR!", __func__, __LINE__);
				goto ERROR;
			}
		}
		else
		{
			printf("\r\n %s-%d: Lan_READMIIM_apro ERROR!", __func__, __LINE__);
			goto ERROR;
		}
#if 0
		// RC-cal :
		ret = Lan_READMIIM_apro(port, 0xBCD, 22, &miim_data);
		if (!ret)
		{
			// len
			miim_data = 0x8888;
			ret = Lan_WRITEMIIM_apro(port, 0xBCD, 22, miim_data);
			if (ret)
			{
				printf("\r\n %s-%d: Lan_WRITEMIIM_apro ERROR!", __func__, __LINE__);
				goto ERROR;
			}
		}
		else
		{
			printf("\r\n %s-%d: Lan_READMIIM_apro ERROR!", __func__, __LINE__);
			goto ERROR;
		}

		ret = Lan_READMIIM_apro(port, 0xBCD, 23, &miim_data);
		if (!ret)
		{
			// rlen
			miim_data = 0x8888;
			ret = Lan_WRITEMIIM_apro(port, 0xBCD, 23, miim_data);
			if (ret)
			{
				printf("\r\n %s-%d: Lan_WRITEMIIM_apro ERROR!", __func__, __LINE__);
				goto ERROR;
			}
		}
		else
		{
			printf("\r\n %s-%d: Lan_READMIIM_apro ERROR!", __func__, __LINE__);
			goto ERROR;
		}

		// ## R-cal  :
		ret = Lan_READMIIM_apro(port, 0xBCE, 16, &miim_data);
		if (!ret)
		{
			// tapbin
			miim_data = 0x8888;
			ret = Lan_WRITEMIIM_apro(port, 0xBCE, 16, miim_data);
			if (ret)
			{
				printf("\r\n %s-%d: Lan_WRITEMIIM_apro ERROR!", __func__, __LINE__);
				goto ERROR;
			}
		}
		else
		{
			printf("\r\n %s-%d: Lan_READMIIM_apro ERROR!", __func__, __LINE__);
			goto ERROR;
		}

		ret = Lan_READMIIM_apro(port, 0xBCE, 17, &miim_data);
		if (!ret)
		{
			// tapbin_pm
			miim_data = 0x8888;
			ret = Lan_WRITEMIIM_apro(port, 0xBCE, 17, miim_data);
			if (ret)
			{
				printf("\r\n %s-%d: Lan_WRITEMIIM_apro ERROR!", __func__, __LINE__);
				goto ERROR;
			}
		}
		else
		{
			printf("\r\n %s-%d: Lan_READMIIM_apro ERROR!", __func__, __LINE__);
			goto ERROR;
		}

		// ADC biase -cal:
		ret = Lan_READMIIM_apro(port, 0xBCF, 22, &miim_data);
		if (!ret)
		{
			// adcioffset
			miim_data = 0x8888;
			ret = Lan_WRITEMIIM_apro(port, 0xBCF, 22, miim_data);
			if (ret)
			{
				printf("\r\n %s-%d: Lan_WRITEMIIM_apro ERROR!", __func__, __LINE__);
				goto ERROR;
			}
		}
		else
		{
			printf("\r\n %s-%d: Lan_READMIIM_apro ERROR!", __func__, __LINE__);
			goto ERROR;
		}
#endif
	}

	///-- Check result
#if 0
	for (port=0 ; port<PHY_K_PORT_NUM ; port++)
	{
		ret = Lan_READMIIM_apro(port, 0xBCA, 22, &miim_data);
		if (!ret) {
			printf("\r\n Lan_READMIIM_apro(%d, 0xBCA, 22)=0x%x", port, miim_data);
		}
#if 0
		ret = Lan_READMIIM_apro(port, 0xBCD, 22, &miim_data);
		if (!ret) {
			printf("\r\n Lan_READMIIM_apro(%d, 0xBCD, 22)=0x%x", port, miim_data);
		}

		ret = Lan_READMIIM_apro(port, 0xBCD, 23, &miim_data);
		if (!ret) {
			printf("\r\n Lan_READMIIM_apro(%d, 0xBCD, 23)=0x%x", port, miim_data);
		}

		ret = Lan_READMIIM_apro(port, 0xBCE, 16, &miim_data);
		if (!ret) {
			printf("\r\n Lan_READMIIM_apro(%d, 0xBCE, 16)=0x%x", port, miim_data);
		}

		ret = Lan_READMIIM_apro(port, 0xBCE, 17, &miim_data);
		if (!ret) {
			printf("\r\n Lan_READMIIM_apro(%d, 0xBCE, 17)=0x%x", port, miim_data);
		}

		ret = Lan_READMIIM_apro(port, 0xBCF, 22, &miim_data);
		if (!ret) {
			printf("\r\n Lan_READMIIM_apro(%d, 0xBCF, 22)=0x%x", port, miim_data);
		}
	}
#endif
#endif

#if 0
	efuse_write_entry(0x106, 0x0010);
	efuse_write_entry(0x107, 0x001f);
	efuse_write_entry(0x110, 0x8888);
	efuse_write_entry(0x111, 0x9aaa);
	efuse_write_entry(0x112, 0x8698);
	efuse_write_entry(0x113, 0x0006);
	efuse_write_entry(0x114, 0x8888);
	efuse_write_entry(0x115, 0x98a9);
	efuse_write_entry(0x116, 0x8688);
	efuse_write_entry(0x117, 0x0006);
	efuse_write_entry(0x118, 0x8888);
	efuse_write_entry(0x119, 0x8998);
	efuse_write_entry(0x11a, 0x8889);
	efuse_write_entry(0x11b, 0x0006);
	efuse_write_entry(0x11c, 0x8888);
	efuse_write_entry(0x11d, 0x7889);
	efuse_write_entry(0x11e, 0x9888);
	efuse_write_entry(0x11f, 0x0006);
#endif

	// <A2-4> RL6831_patch_MP_cal_OTP_reload : c-code
	ret=Lan_PHYCAL_load_apro_gen2();

	///-- Check result
#if 0
	if (ret == 0)
		printf("\r\n %s-%d: Lan_PHYCAL_load_apro SUCCESS!", __func__, __LINE__);
	else
		printf("\r\n %s-%d: Lan_PHYCAL_load_apro FAIL!", __func__, __LINE__);
#endif
	Lan_PHYCAL_check_apro_gen2();

	// <A2-5> RL6831_patch_INRX_parameter
	for (port=0 ; port<PHY_K_PORT_NUM ; port++)
	{
		ret = Lan_READMIIM_apro(port, 0xA43, 27, &miim_data);
		if (!ret)
		{
			// Giga master sram addr = 0x809b
			miim_data = 0x809b;
			ret = Lan_WRITEMIIM_apro(port, 0xA43, 27, miim_data);
			if (ret)
			{
				printf("\r\n %s-%d: Lan_WRITEMIIM_apro ERROR!", __func__, __LINE__);
				goto ERROR;
			}
		}
		else
		{
			printf("\r\n %s-%d: Lan_READMIIM_apro ERROR!", __func__, __LINE__);
			goto ERROR;
		}

		ret = Lan_READMIIM_apro(port, 0xA43, 28, &miim_data);
		if (!ret)
		{
			// INRX delta_a from default 0xD to 0x11.
			miim_data &= ~(0x1f<<11);
			miim_data |= (0x11<<11);
			ret = Lan_WRITEMIIM_apro(port, 0xA43, 28, miim_data);
			if (ret)
			{
				printf("\r\n %s-%d: Lan_WRITEMIIM_apro ERROR!", __func__, __LINE__);
				goto ERROR;
			}
		}
		else
		{
			printf("\r\n %s-%d: Lan_READMIIM_apro ERROR!", __func__, __LINE__);
			goto ERROR;
		}
	}

	///-- Check result
#if 0
	for (port=0 ; port<PHY_K_PORT_NUM ; port++)
	{
		ret = Lan_READMIIM_apro(port, 0xA43, 27, &miim_data);
		if (!ret) {
			printf("\r\n Lan_READMIIM_apro(%d, 0xA43, 27)=0x%x", port, miim_data);
		}

		ret = Lan_READMIIM_apro(port, 0xA43, 28, &miim_data);
		if (!ret) {
			printf("\r\n Lan_READMIIM_apro(%d, 0xA43, 28)=0x%x", port, miim_data);
		}
	}
#endif

	// <A2-end> release lock main state
	///--printf("\r\n <A2-end> release lock main state");
	for (port=0 ; port<PHY_K_PORT_NUM ; port++)
	{
		ret = Lan_READMIIM_apro(port, 0xA46, 21, &miim_data);
		if (!ret)
		{
			if (miim_data & (1<<1))
			{
				// set lock main 
				miim_data &= ~(1<<1);
				ret = Lan_WRITEMIIM_apro(port, 0xA46, 21, miim_data);
				if (ret)
				{
					printf("\r\n %s-%d: Lan_WRITEMIIM_apro ERROR!", __func__, __LINE__);
					goto ERROR;
				}
			}
		}
		else
		{
			printf("\r\n %s-%d: Lan_READMIIM_apro ERROR!", __func__, __LINE__);
			goto ERROR;
		}
	}

	///-- Check result
#if 0
	for (port=0 ; port<PHY_K_PORT_NUM ; port++)
	{
		ret = Lan_READMIIM_apro(port, 0xA46, 21, &miim_data);
		if (!ret) {
			printf("\r\n Lan_READMIIM_apro(%d, 0xA46, 21)=0x%x", port, miim_data);
		}
	}
#endif

	// (A3) set GPHY patch done bit
	///--printf("\r\n (A3) set GPHY patch done bit");
	REG32(0xBB000114) |=  (1<<0);

	///-- Check result
#if 0
	printf("\r\n REG32(0xBB000114)=0x%x", REG32(0xBB000114));
	printf("\r\n");
#endif

	// (A4) all ports reset PHY and restart AN
	///--printf("\r\n (A4) all ports reset PHY and restart AN");
	for (port=0 ; port<PHY_K_PORT_NUM ; port++)
	{
		ret = Lan_READMIIM_apro(port, 0xA40, 0, &miim_data);
		if (!ret)
		{
			// set lock main
			miim_data &= ~((1<<15)|(1<<12)|(1<<9));
			miim_data |= ((1<<15)|(1<<12)|(1<<9));
			ret = Lan_WRITEMIIM_apro(port, 0xA40, 0, miim_data);
			if (ret)
			{
				printf("\r\n %s-%d: Lan_WRITEMIIM_apro ERROR!", __func__, __LINE__);
				goto ERROR;
			}
		}
		else
		{
			printf("\r\n %s-%d: Lan_READMIIM_apro ERROR!", __func__, __LINE__);
			goto ERROR;
		}
	}

	///-- Check result
#if 0
	for (port=0 ; port<PHY_K_PORT_NUM ; port++)
	{
		ret = Lan_READMIIM_apro(port, 0xA40, 0, &miim_data);
		if (!ret) {
			printf("\r\n Lan_READMIIM_apro(%d, 0xA40, 0)=0x%x", port, miim_data);
		}
	}
	printf("\r\n");
#endif

	mdelay(20);
	// (others) Link down all GPHY
	for (port=0 ; port<PHY_K_PORT_NUM ; port++)
	{
		ret = Lan_READMIIM_apro(port, 0, 0, &miim_data);
		if (!ret)
		{
			// set lock main
			miim_data |= (1<<11);
			ret = Lan_WRITEMIIM_apro(port, 0, 0, miim_data);
			if (ret)
			{
				printf("\r\n %s-%d: Lan_WRITEMIIM_apro ERROR!", __func__, __LINE__);
				goto ERROR;
			}
		}
		else
		{
			printf("\r\n %s-%d: Lan_READMIIM_apro ERROR!", __func__, __LINE__);
			goto ERROR;
		}
	}

	return 0;

ERROR:
	return -1;
}

int Lan_eth_change_default(void)
{
	int ret=0;

	switch(OTTO_SID){
		case PLR_SID_APRO:
		case PLR_SID_9603CVD:
			break;
		case PLR_SID_APRO_GEN2:
			//Lan_RXENABLE();
			ret=Lan_eth_change_default_apro_gen2();
			break;
	}

	return ret;
}
#else
int Lan_eth_change_default(void)
{
	return 0;
}
#endif
