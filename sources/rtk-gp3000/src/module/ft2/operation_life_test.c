#include <linux/interrupt.h>
#include <linux/delay.h>
#include <asm/io.h>
#include <linux/mm.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/dma-mapping.h>
#include <linux/kthread.h>
#include <linux/vmalloc.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include "module/ft2/pcm.h"
#include "module/ft2/spi.h"
#include <ioal/ioal.h>
#include "module/ft2/operation_life_test.h"

#include <rtk/l2.h>
#include <hal/mac/drv.h>

//#define RTL8696 1
#define RTL9601B 1

#define ZSI 1
//#define ISI 1
static void init_zsi (void);
static void init_isi (void);
static void init_pcm (void);
static inline void printk_off( const char *fmt, ... ) {}
static rtl_spi_dev_t            spi_dev[1];
#define VP890_DEVTYPE_RD            0x73
#define VP890_DEVTYPE_LEN           0x03    /**< RCN = 1st Byte, PCN = 2nd Byte */
#define VP890_EC_CH1            0x01
#define SPI_PRINTK              printk_off
#define SPI2_PRINTK              printk_off
#define PCM_BUF_SIZE 20


#ifdef RTL9601B
#define PLLControl 0xb8000204
#define IPEnable 0xb8000600
#define IO_MODE_EN	0xbb023004
#endif

static void print_reg(void)
{
	printk("PCMCR= 0x%x\n", pcm_inl(PCMCR));
	printk("PCMCHCNR= 0x%x\n", pcm_inl(PCMCHCNR));
	printk("PCMBSIZE= 0x%x\n", pcm_inl(PCMBSIZE));
	printk("CH0TXBSA= 0x%x\n", pcm_inl(CH0TXBSA));
	printk("CH0RXBSA= 0x%x\n", pcm_inl(CH0RXBSA));
	printk("CH1TXBSA= 0x%x\n", pcm_inl(CH1TXBSA));
	printk("CH1RXBSA= 0x%x\n", pcm_inl(CH1RXBSA));
	printk("CH2TXBSA= 0x%x\n", pcm_inl(CH2TXBSA));
	printk("CH2RXBSA= 0x%x\n", pcm_inl(CH2RXBSA));
	printk("CH3TXBSA= 0x%x\n", pcm_inl(CH3TXBSA));
	printk("CH3RXBSA= 0x%x\n", pcm_inl(CH3RXBSA));	
	printk("PCMTSR= 0x%x\n", pcm_inl(PCMTSR));
	printk("PCMIMR= 0x%x\n", pcm_inl(PCMIMR));
	printk("PCMISR= 0x%x\n", pcm_inl(PCMISR));	
	printk("PCMCHCNR47= 0x%x\n", pcm_inl(PCMCHCNR47));
	printk("PCMBSIZE47= 0x%x\n", pcm_inl(PCMBSIZE47));
	printk("CH4TXBSA= 0x%x\n", pcm_inl(CH4TXBSA));
	printk("CH4RXBSA= 0x%x\n", pcm_inl(CH4RXBSA));
	printk("CH5TXBSA= 0x%x\n", pcm_inl(CH5TXBSA));
	printk("CH5RXBSA= 0x%x\n", pcm_inl(CH5RXBSA));
	printk("CH6TXBSA= 0x%x\n", pcm_inl(CH6TXBSA));
	printk("CH6RXBSA= 0x%x\n", pcm_inl(CH6RXBSA));
	printk("CH7TXBSA= 0x%x\n", pcm_inl(CH7TXBSA));
	printk("CH7RXBSA= 0x%x\n", pcm_inl(CH7RXBSA));	
	printk("PCMTSR47= 0x%x\n", pcm_inl(PCMTSR47));
	printk("PCMIMR47= 0x%x\n", pcm_inl(PCMIMR47));
	printk("PCMISR47= 0x%x\n", pcm_inl(PCMISR47));
	printk("SPICNR   (%p)=%08X\n", pSPICNR, *pSPICNR );
	printk("SPISTR   (%p)=%08X\n", pSPISTR, *pSPISTR );
	printk("SPICKDIV (%p)=%08X\n", pSPICKDIV, *pSPICKDIV );
	printk("SPIRDR   (%p)=%08X\n", pSPIRDR, *pSPIRDR );
	printk("SPITDR   (%p)=%08X\n", pSPITDR, *pSPITDR );
	printk("SPITCR   (%p)=%08X\n", pSPITCR, *pSPITCR );
	printk("SPICDTCR0(%p)=%08X\n", pSPICDTCR0, *pSPICDTCR0 );
	printk("SPICDTCR1(%p)=%08X\n", pSPICDTCR1, *pSPICDTCR1 );
	printk("SPITCALR (%p)=%08X\n", pSPITCALR, *pSPITCALR );	
}

static inline int32 __rtl_hw_spi_chip_select( rtl_spi_dev_t* pDev, int select )
{
	SPI2_PRINTK("[%s][%d] pDev ->type = [%x]\n", __FUNCTION__, __LINE__, pDev ->type);
	switch( pDev ->type ) {
	case SPI_TYPE_HW:	// use SPI CS 
		// Transaction Configuration Register 
		if( select )
		{
			SPI2_PRINTK("[%s][%d]\n", __FUNCTION__, __LINE__);
			*pSPITCR |= SPITCR_SEL_CS( pDev ->hw.spiCS_ );
		}else
		{
			SPI2_PRINTK("[%s][%d]\n", __FUNCTION__, __LINE__);
			*pSPITCR &= ~SPITCR_SEL_CS( pDev ->hw.spiCS_ );
		}
		break;
	default:
		printk( "unknown spi type=%d\n", pDev ->type );
		return FAILED;
		break;
	}
	
	return SUCCESS;
}

static inline int32 _rtl_hw_spi_chip_select( rtl_spi_dev_t* pDev )
{
	return __rtl_hw_spi_chip_select( pDev, 1 );	// chip select 
}

static inline int32 _rtl_hw_spi_chip_deselect( rtl_spi_dev_t* pDev )
{
	return __rtl_hw_spi_chip_select( pDev, 0 );	// chip deselect 
}

static inline void __rtl_hw_spi_phase_select( rtl_spi_dev_t* pDev, int phase )
{
	SPI2_PRINTK("[%s][%d] phase = [%x]\n", __FUNCTION__, __LINE__,phase);
	// 1 or 3 phase 
	switch( phase ) {
	case 1: 	// Zarlink SLIC 
		// 1 phase use 'D0' only 
		SPI2_PRINTK("[%s][%d]\n", __FUNCTION__, __LINE__);
		*pSPITCR &= ~( SPITCR_CTL_EN | SPITCR_ADD_EN | SPITCR_D0_EN | SPITCR_D1_EN );
		*pSPITCR |= SPITCR_D0_EN;
		break;
		
	case 3:		// Silab SLIC 
		// 3 phases use 'CTL', 'ADD' and 'D0' 
		SPI2_PRINTK("[%s][%d]\n", __FUNCTION__, __LINE__);
		*pSPITCR &= ~( SPITCR_CTL_EN | SPITCR_ADD_EN | SPITCR_D0_EN | SPITCR_D1_EN );
		*pSPITCR |= SPITCR_CTL_EN | SPITCR_ADD_EN | SPITCR_D0_EN;
		break;
		
	default:
		printk( "unknown spi phase=%d\n", phase );
		break;
	}
}

static inline int32 _rtl_hw_spi_rawRead( rtl_spi_dev_t* pDev, void* pData)
{
	uint8* pch = pData;
	
	*pch = 0;

	if ( pData == NULL ) return FAILED;
	
	// chip select 
	_rtl_hw_spi_chip_select( pDev );
	
	// phase select 
	__rtl_hw_spi_phase_select( pDev, 1 );
	
	// clear reading data (D0)
	*pSPIRDR = 0xDE << SPIRDR_RD0R_SHIFT;	// use magic number instead of 0 
	
	// set ctrl register & start transfer 
	*pSPICNR = SPICNR_CSTIME_8BIT | SPICNR_MSB | SPICNR_CMD_READ | SPICNR_START | SPICNR_CSP_LOW_ACTIVE;
	
	// wait unitl finish
	while( *pSPICNR & SPICNR_START );
	
	// read data (D0)
	*pch = ( uint8 )( ( *pSPIRDR & SPIRDR_RD0R ) >> SPIRDR_RD0R_SHIFT );
	
	// chip deselect 
	_rtl_hw_spi_chip_deselect( pDev );
	
	SPI_PRINTK( "Raw Read 8 [1]: *data=%X *pch=%X *pSPIRDR=%08X\n", *( ( uint8 * )pData ), *pch, *pSPIRDR );
	
	return SUCCESS;
}

static inline int32 _rtl_hw_spi_rawWrite( rtl_spi_dev_t* pDev, void* pData)
{
	uint8* pch = pData;

	if ( pData == NULL ) return FAILED;
	
	// chip select 
	_rtl_hw_spi_chip_select( pDev );
	
	// phase select 
	__rtl_hw_spi_phase_select( pDev, 1 );
	
	// set writing data (D0)
	*pSPITDR = ( *pch ) << SPIRDR_TD0R_SHIFT;
	
	// set ctrl register 
	*pSPICNR = SPICNR_CSTIME_8BIT | SPICNR_MSB | SPICNR_CMD_WRITE | SPICNR_CSP_LOW_ACTIVE;
	
	// start transfer 
	*pSPICNR |= SPICNR_START;
	
	// wait unitl finish
	while( *pSPICNR & SPICNR_START );
	
	// chip deselect 
	_rtl_hw_spi_chip_deselect( pDev );
	
	return SUCCESS;
}

static inline int32 _init_rtl_spi_dev_type_hw( rtl_spi_dev_t* pDev, uint32 spiCS_ )
{
	pDev ->type = SPI_TYPE_HW;
	
	pDev ->hw.spiCS_ = spiCS_;
	
	return SUCCESS;
}

static void init_rtl_hw_spi_IP( void )
{
	// clock divisor register 
	*pSPICKDIV = ( 0x7 << SPICKDIV_DIV_SHIFT );	// 7.8M
	//*pSPICKDIV = ( 0x13 << SPICKDIV_DIV_SHIFT );	// 3.125M
	
	// transaction configuration register 
	*pSPITCR = 	( SPITCR_D0_EN ) |
				( ( 0x18 << SPITCR_DCS_CNT_UNIT_SHIFT ) & SPITCR_DCS_CNT_UNIT ) |
				( ( 0x23 << SPITCR_DCS_DEF_CNT_SHIFT ) & SPITCR_DCS_DEF_CNT );

	// timing calibration register
	*pSPITCALR = ( ( 0x04 << SPITCALR_CS_SETUP_CAL_SHIEF ) & SPITCALR_CS_SETUP_CAL) |
			( ( 0x04 << SPITCALR_CS_HOLD_CAL_SHIEF ) & SPITCALR_CS_HOLD_CAL) |
			( ( 0x00 << SPITCALR_SD0_SETUP_CAL_SHIEF ) & SPITCALR_SD0_SETUP_CAL);

	SPI_PRINTK( "*pSPICKDIV(%p)=%08X pSPITCR(%p)=%08X pSPITCALR(%p)=%08X\n",
			pSPICKDIV, *pSPICKDIV, 
			pSPITCR, *pSPITCR,
			pSPITCALR, *pSPITCALR);
}

static inline int32 _rtl_spi_rawRead( rtl_spi_dev_t* pDev, void* pData, int32 bits )
{
	SPI_PRINTK( "Raw Read 8 [0]: dev=%p, data=%p *data8=%02X\n",
								pDev, pData, *( ( uint8 * )pData ));
	
	switch( pDev ->type ) {
		case SPI_TYPE_HW:
			SPI2_PRINTK("pDev ->type = [%x]\n", pDev ->type);
			return _rtl_hw_spi_rawRead( pDev, pData);
			break;
		default:
			break;
	}
	
	return FAILED;
}

static inline int32 _rtl_spi_rawWrite( rtl_spi_dev_t* pDev, void* pData, int32 bits )
{
	SPI_PRINTK( "Raw Write 8 [0]: dev=%p, data=%p *data8=%02X\n",
								pDev, pData, *( ( uint8 * )pData ));
	
	switch( pDev ->type ) {
		case SPI_TYPE_HW:
			SPI2_PRINTK("pDev ->type = [%x]\n", pDev ->type);
			return _rtl_hw_spi_rawWrite( pDev, pData);
			break;
		default:
			break;
	}
	
	return FAILED;
}

// uint8 devType[VP890_DEVTYPE_LEN];
// VpMpiCmdWrapper(deviceId, VP890_EC_CH1, VP890_DEVTYPE_RD, VP890_DEVTYPE_LEN, devType);
// VpMpiCmd(deviceId, ecVal, mpiCmd, mpiCmdLen, dataBuffer);
void spi_test_read(int type)
{
	int i = 0, retry = 0;
	uint8 byteCnt;
	//uint8 isRead = (cmd & READ_COMMAND);
	uint8 devType[4];
	uint8 cmd = VP890_DEVTYPE_RD, data = 0x0;
	uint8 ecVal = 0x01;   //VP890_EC_CH1
	
	
	if ( type <= 2 )
		retry = 5;
	else if ( type >= 3 )
		retry = 10;

	for ( i = 0 ; i < 1 ; i ++ )
	{
		ecVal = 0x06;

		if ( type == 0 )
		{
			cmd = 0x04;
			printk("cmd = [%x]\n", cmd);
			_rtl_spi_rawWrite(( rtl_spi_dev_t * )spi_dev, &cmd, 8);
		}
		else if ( type == 1 )
		{
			cmd = 0xf2;
			data = 0x50;
			printk("cmd = [%x], data = [%x]\n", cmd, data);
			_rtl_spi_rawWrite(( rtl_spi_dev_t * )spi_dev, &cmd, 8);
			_rtl_spi_rawWrite(( rtl_spi_dev_t * )spi_dev, &data, 8);
		}

		if ( type == 2 )
		{	
			cmd = 0xfd;
			printk("cmd = [%x]\n", cmd);
			_rtl_spi_rawWrite(( rtl_spi_dev_t * )spi_dev, &cmd, 8);
			_rtl_spi_rawWrite( ( rtl_spi_dev_t * )spi_dev, &ecVal, 8);

			for(byteCnt=0; byteCnt < 4; byteCnt++){
				_rtl_spi_rawRead( ( rtl_spi_dev_t * )spi_dev, &devType[byteCnt], 8);
			}
			for ( byteCnt = 0 ; byteCnt < 4 ; byteCnt ++ )
				printk("[%02x]", devType[byteCnt]);
		}
			
		printk("\n");
	}	
#if 0
	printk("\nsend read id command..\n");
	for ( i = 0 ; i < retry ; i ++ )
	{
		if ( type == 0 ){
			ecVal = 0x4A;
		}else if ( type == 1 ){
			ecVal = 0x01;
		}else if ( type == 2 || type == 4){
			ecVal = 0x06;
		}
		_rtl_spi_rawWrite( ( rtl_spi_dev_t * )spi_dev, &ecVal, 8);
		_rtl_spi_rawWrite(( rtl_spi_dev_t * )spi_dev, &cmd, 8);

		printk("read id..\n");
		for(byteCnt=0; byteCnt < VP890_DEVTYPE_LEN; byteCnt++){
			_rtl_spi_rawRead( ( rtl_spi_dev_t * )spi_dev, &devType[byteCnt], 8);;
		}
		printk("id : ");
		for ( byteCnt = 0 ; byteCnt < VP890_DEVTYPE_LEN ; byteCnt ++ )
			printk("[%02x]", devType[byteCnt]);
			
		printk("\n");
	}

	if ( type <= 2 )
	{
		for ( i = 0 ; i < 5 ; i ++ )
		{
			cmd = 0x4d;
			printk("write 0x4d..\n");
			_rtl_spi_rawWrite(( rtl_spi_dev_t * )spi_dev, &cmd, 8);

			printk("read 0x4d..\n");
			for(byteCnt=0; byteCnt < 2; byteCnt++){
				_rtl_spi_rawRead( ( rtl_spi_dev_t * )spi_dev, &devType[byteCnt], 8);			
			}
			for ( byteCnt = 0 ; byteCnt < VP890_DEVTYPE_LEN ; byteCnt ++ )
				printk("[%02x]", devType[byteCnt]);		
		}
		printk("\n");
		
		for ( i = 0 ; i < 5 ; i ++ )
		{
			cmd = 0x47;
			printk("write 0x47..\n");
			_rtl_spi_rawWrite(( rtl_spi_dev_t * )spi_dev, &cmd, 8);

			printk("read 0x47..\n");
			for(byteCnt = 0; byteCnt < 1; byteCnt++){
				_rtl_spi_rawRead( ( rtl_spi_dev_t * )spi_dev, &devType[byteCnt], 8);			
			}
			for ( byteCnt = 0 ; byteCnt < 1 ; byteCnt ++ )
				printk("[%02x]", devType[byteCnt]);		
		}
	}
#endif
	printk("\n");	
	print_reg();
}


#if defined(RTL9601B)
int comparedata(int type, unsigned long *txbuf, unsigned long *rxbuf)
{
	int i = 0;
	unsigned long rxtmpbuf[PCM_BUF_SIZE] = {0x2d, 0x5a, 0xb4, 0x168, 
											0x2d0, 0x5a0, 0xb40, 0x1680, 
											0x2d00, 0x5a00, 0x3400, 0x16800, 
											0x25000, 0x52000, 0xb4000, 0x160000, 
											0x2d0000, 0x5a0000, 0xb40000, 0x1680000};
	if ( type == 0 ){
		for ( i = 0 ; i < PCM_BUF_SIZE ; i ++ )
		{	
			//printk("11 : tx[%d] = %x , rx[%d] = %x \n", i , txbuf[i] , i , rxbuf[i]);
			if ( txbuf[i] != rxbuf[i] )
				return 0;
		}
	}else if ( type == 1 ){
		for ( i = 0 ; i < PCM_BUF_SIZE ; i ++ )
		{
			if ( rxbuf[i] != rxtmpbuf[i] )
				return 0;
		}	
	}
	return 1;
}
#endif

/*
	chid : channel number
*/
static void pcm_loopback(int type, int chid)
{
	int i;
	dma_addr_t tx_dma , rx_dma;
	volatile unsigned long *txbuf , *rxbuf, *xmit_tmp;
	unsigned int pcm_tx_rx_enable=0;
	unsigned char tx_data_temp=0x5a;
	printk("test channel number : [%d], type = [%d]\n", chid, type);
	/*enable PCM module */
	//RTL_W32(0xB8000600 , RTL_R32(0xB8000600) | (1<<2) | (1<<16));
	
	/*Disable PCM TX/RX*/
	if (chid & 4) {
		RTL_W32(PCMCHCNR47 , 0);
	}else{
		RTL_W32(PCMCHCNR , 0);
	}
	
	/*PCM General Control Register: linear mode,enable PCM*/
	//RTL_W32(PCMCR,( ZSILBE | LINEAR_MODE | PCM_AE | C0ILBE));

	if ( type == 13 ){
		init_isi();
		RTL_W32(PCMCR,( ISILBE | LINEAR_MODE | PCM_AE ));		
		//printk("type = [%d] : PCMCR,( ISILBE | LINEAR_MODE | PCM_AE )\n", type);
	}else if ( type == 15 ){
		init_zsi();
		RTL_W32(PCMCR,( LINEAR_MODE | PCM_AE ));
		//printk("type = [%d] : PCMCR,( LINEAR_MODE | PCM_AE )\n", type);
	}else if ( type == 11 ){
		init_pcm();
		RTL_W32(PCMCR,( LINEAR_MODE | PCM_AE ));
	}	

	/*PCM interface Buffer Size Register*/

	txbuf = dma_alloc_coherent(NULL , PCM_BUF_SIZE*sizeof(long)  , &tx_dma, GFP_KERNEL);
	rxbuf = dma_alloc_coherent(NULL , PCM_BUF_SIZE*sizeof(long)  , &rx_dma, GFP_KERNEL);
	//printk("txmda = %x , rxdma = %x \n" , tx_dma , rx_dma);
	for( i=0; i<PCM_BUF_SIZE; i++)
	{
		rxbuf[i] = 0xffffffff;
		txbuf[i] = 0x0;
	}
	
	switch ( chid ){
		case 0:
			xmit_tmp = txbuf;
			printk("xmit_tmp=%p ,txbuf=%p \n", xmit_tmp, txbuf);
			// set data pointer and change owner to pcm controller
			RTL_W32(CH0TXBSA,tx_dma|0x3);
			printk("CH0TXBSA = 0x%x   \n",RTL_R32(CH0TXBSA));
			for ( i=0; i<PCM_BUF_SIZE; i++)
			{
				//external loop back has to ignore bit 0 and bit 16
				 *(xmit_tmp+i)= (tx_data_temp << (i%28))&0xFFFEFFFE; 
			}			
			//wmb();
			xmit_tmp = rxbuf;
			RTL_W32(CH0RXBSA, rx_dma|0x3);
			printk("CH0RXBSA = 0x%x   \n",RTL_R32(CH0RXBSA));
			pcm_tx_rx_enable |= 3<<24;
			wmb();
			RTL_W32(PCMCHCNR, pcm_tx_rx_enable);			
			break;
		case 1:		
			xmit_tmp = txbuf;
			printk("xmit_tmp=%p ,txbuf=%p \n", xmit_tmp, txbuf);
			// set data pointer and change owner to pcm controller
			RTL_W32(CH1TXBSA,tx_dma|0x3);
			printk("CH1TXBSA = 0x%x   \n",RTL_R32(CH1TXBSA));
			for ( i=0; i<PCM_BUF_SIZE; i++)
			{
				//external loop back has to ignore bit 0 and bit 16
				 *(xmit_tmp+i)= (tx_data_temp << (i%28))&0xFFFEFFFE; 
			}			
			//wmb();
			xmit_tmp = rxbuf;
			RTL_W32(CH1RXBSA, rx_dma|0x3);
			printk("CH1RXBSA = 0x%x   \n",RTL_R32(CH1RXBSA));
			pcm_tx_rx_enable |= 3<<16;
	
			break;
		case 2:	
			xmit_tmp = txbuf;
			printk("xmit_tmp=%p ,txbuf=%p \n", xmit_tmp, txbuf);
			// set data pointer and change owner to pcm controller
			RTL_W32(CH2TXBSA,tx_dma|0x3);
			printk("CH2TXBSA = 0x%x   \n",RTL_R32(CH2TXBSA));
			for ( i=0; i<PCM_BUF_SIZE; i++)
			{
				//external loop back has to ignore bit 0 and bit 16
				 *(xmit_tmp+i)= (tx_data_temp << (i%28))&0xFFFEFFFE; 
			}			
			//wmb();
			xmit_tmp = rxbuf;
			RTL_W32(CH2RXBSA, rx_dma|0x3);
			printk("CH2RXBSA = 0x%x   \n",RTL_R32(CH2RXBSA));
			pcm_tx_rx_enable |= 3<<8;
			wmb();
			RTL_W32(PCMCHCNR, pcm_tx_rx_enable);	
			break;	
		case 3:		
			xmit_tmp = txbuf;
			printk("xmit_tmp=%p ,txbuf=%p \n", xmit_tmp, txbuf);
			// set data pointer and change owner to pcm controller
			RTL_W32(CH3TXBSA,tx_dma|0x3);
			printk("CH3TXBSA = 0x%x   \n",RTL_R32(CH3TXBSA));
			for ( i=0; i<PCM_BUF_SIZE; i++)
			{
				//external loop back has to ignore bit 0 and bit 16
				 *(xmit_tmp+i)= (tx_data_temp << (i%28))&0xFFFEFFFE; 
			}			
			//wmb();
			xmit_tmp = rxbuf;
			RTL_W32(CH3RXBSA, rx_dma|0x3);
			printk("CH3RXBSA = 0x%x   \n",RTL_R32(CH3RXBSA));
			pcm_tx_rx_enable |= 3;
			wmb();
			RTL_W32(PCMCHCNR, pcm_tx_rx_enable);	
			break;	
		case 4:		
			xmit_tmp = txbuf;
			printk("xmit_tmp=%p ,txbuf=%p \n", xmit_tmp, txbuf);
			// set data pointer and change owner to pcm controller
			RTL_W32(CH4TXBSA,tx_dma|0x3);
			printk("CH4TXBSA = 0x%x   \n",RTL_R32(CH4TXBSA));
			for ( i=0; i<PCM_BUF_SIZE; i++)
			{
				//external loop back has to ignore bit 0 and bit 16
				 *(xmit_tmp+i)= (tx_data_temp << (i%28))&0xFFFEFFFE; 
			}			
			//wmb();
			xmit_tmp = rxbuf;
			RTL_W32(CH4RXBSA, rx_dma|0x3);
			printk("CH4RXBSA = 0x%x   \n",RTL_R32(CH4RXBSA));
			pcm_tx_rx_enable |= 3<<24;
			break;	
		case 5:
			xmit_tmp = txbuf;
			printk("xmit_tmp=%p ,txbuf=%p \n", xmit_tmp, txbuf);
			// set data pointer and change owner to pcm controller
			RTL_W32(CH5TXBSA,tx_dma|0x3);
			printk("CH5TXBSA = 0x%x   \n",RTL_R32(CH5TXBSA));
			for ( i=0; i<PCM_BUF_SIZE; i++)
			{
				//external loop back has to ignore bit 0 and bit 16
				 *(xmit_tmp+i)= (tx_data_temp << (i%28))&0xFFFEFFFE; 
			}			
			//wmb();
			xmit_tmp = rxbuf;
			RTL_W32(CH5RXBSA, rx_dma|0x3);
			printk("CH5RXBSA = 0x%x   \n",RTL_R32(CH5RXBSA));
			pcm_tx_rx_enable |= 3<<16;
			break;	
		case 6:			
			xmit_tmp = txbuf;
			printk("xmit_tmp=%p ,txbuf=%p \n", xmit_tmp, txbuf);
			// set data pointer and change owner to pcm controller
			RTL_W32(CH6TXBSA,tx_dma|0x3);
			printk("CH6TXBSA = 0x%x   \n",RTL_R32(CH6TXBSA));
			for ( i=0; i<PCM_BUF_SIZE; i++)
			{
				//external loop back has to ignore bit 0 and bit 16
				 *(xmit_tmp+i)= (tx_data_temp << (i%28))&0xFFFEFFFE; 
			}			
			//wmb();
			xmit_tmp = rxbuf;
			RTL_W32(CH6RXBSA, rx_dma|0x3);
			printk("CH6RXBSA = 0x%x   \n",RTL_R32(CH6RXBSA));
			pcm_tx_rx_enable |= 3<<8;
			break;	
		case 7:
			xmit_tmp = txbuf;
			printk("xmit_tmp=%p ,txbuf=%p \n", xmit_tmp, txbuf);
			// set data pointer and change owner to pcm controller
			RTL_W32(CH7TXBSA,tx_dma|0x3);
			printk("CH7TXBSA = 0x%x   \n",RTL_R32(CH7TXBSA));
			for ( i=0; i<PCM_BUF_SIZE; i++)
			{
				//external loop back has to ignore bit 0 and bit 16
				 *(xmit_tmp+i)= (tx_data_temp << (i%28))&0xFFFEFFFE; 
			}			
			//wmb();
			xmit_tmp = rxbuf;
			RTL_W32(CH7RXBSA, rx_dma|0x3);
			printk("CH7RXBSA = 0x%x   \n",RTL_R32(CH7RXBSA));
			pcm_tx_rx_enable |= 3;		
			break;
		default:
			break;
	}
	/*PCM TX/RX enable*/
	wmb();
	if (chid & 4) {
		RTL_W32(PCMCHCNR47, pcm_tx_rx_enable);	
	}else{
		RTL_W32(PCMCHCNR, pcm_tx_rx_enable);
	}	
	
	//print_reg();
	switch ( chid ){
		case 0:	
			while(1)
				if((*(volatile unsigned int *)CH0RXBSA&0x3)==0)
					break;
			while(1)
				if((*(volatile unsigned int *)CH0TXBSA&0x3)==0)
					break;						
		case 1:
			while(1)
				if((*(volatile unsigned int *)CH1RXBSA&0x3)==0)
					break;
			while(1)
				if((*(volatile unsigned int *)CH1TXBSA&0x3)==0)
					break;		
		case 2:
			while(1)
				if((*(volatile unsigned int *)CH2RXBSA&0x3)==0)
					break;
			while(1)
				if((*(volatile unsigned int *)CH2TXBSA&0x3)==0)
					break;
		case 3:
			while(1)
				if((*(volatile unsigned int *)CH3RXBSA&0x3)==0)
					break;
			while(1)
				if((*(volatile unsigned int *)CH3TXBSA&0x3)==0)
					break;
		case 4:
			while(1)
				if((*(volatile unsigned int *)CH4RXBSA&0x3)==0)
					break;
			while(1)
				if((*(volatile unsigned int *)CH4TXBSA&0x3)==0)
					break;
		case 5:
			while(1)
				if((*(volatile unsigned int *)CH5RXBSA&0x3)==0)
					break;
			while(1)
				if((*(volatile unsigned int *)CH5TXBSA&0x3)==0)
					break;
		case 6:
			while(1)
				if((*(volatile unsigned int *)CH6RXBSA&0x3)==0)
					break;
			while(1)
				if((*(volatile unsigned int *)CH6TXBSA&0x3)==0)
					break;
		case 7:
			while(1)
				if((*(volatile unsigned int *)CH7RXBSA&0x3)==0)
					break;
			while(1)
				if((*(volatile unsigned int *)CH7TXBSA&0x3)==0)
					break;
		default:
			break;
	}
		
	wmb();
	printk("compare data\n");
	for(i=0;i<PCM_BUF_SIZE;i++)
	{
		printk("tx[%d] = %x , rx[%d] = %x \n", i , txbuf[i] , i , rxbuf[i]);
	}
	//print_reg();
	dma_free_coherent(NULL, PCM_BUF_SIZE*sizeof(long) , txbuf , tx_dma);
	dma_free_coherent(NULL, PCM_BUF_SIZE*sizeof(long) , rxbuf , rx_dma);
	
	
	if (chid & 4) {
		RTL_W32(PCMCHCNR47, 0x0);	
	}else{
		RTL_W32(PCMCHCNR, 0x0);
	}		
	RTL_W32(PCMCR,0x00003000);


#if defined(RTL9601B)	
	int ret = 0;
	u32 regValue;
	if ( type == 13 ){ // ISI
		ret = comparedata(0, txbuf, rxbuf);
	}else if ( type == 15 ){	// ZSI
		ret = comparedata(1, txbuf, rxbuf);
	}		
	
	regValue = RTL_R32(0xbb0231c0);
	
	if ( !ret ){
		//printk("0 : 0xbb0231c0 = [%x]\n", regValue);
		RTL_W32(0xbb0231c0 , 0x0);
	}else{
		//printk("1 : 0xbb0231c0 = [%x]\n", regValue);	
		if ( type == 13 )	// ISI
			RTL_W32(0xbb0231c0 , 0x3f);
		else if ( type == 15 )	// ZSI
			RTL_W32(0xbb0231c0 , 0x3e);
	}
	regValue = RTL_R32(0xbb0231c0);
	printk("0xbb0231c0 = [%x]\n", regValue);	
#endif	
}

static void re8670_pcm_init_hw (void)
{
	u32 regValue;
	
	// init SPI
	init_rtl_hw_spi_IP();
	_init_rtl_spi_dev_type_hw(&spi_dev[0], 0);	
	
	/*enable PCM module */
	regValue = RTL_R32(0xbb023018);
	regValue = regValue & 0xfff9ffff;       // enable ZSI

	
#if defined(RTL8696)
#if defined(ZSI)	
	regValue |= (1<<16);       // enable ZSI
	regValue |= (1<<0);       // enable ZSI
	//regValue |= (1<<18);    // PCM need to enable ?
	RTL_W32(0xbb023018 , regValue);
	
	// SLIC_INSEL_CTRL
	regValue = RTL_R32(0xbb000174);
	regValue |= (1<<2);
	RTL_W32(0xbb000174 , regValue);
	
	RTL_W32(0xB8000600 , RTL_R32(0xB8000600) | (1<<26));
	/*Disable PCM TX/RX*/
	RTL_W32(PCMCHCNR , 0);
#elif defined(ISI)
	regValue |= (1<<15);       // enable ZSI
	regValue |= (1<<0);       // enable ZSI
	//regValue |= (1<<18);    // PCM need to enable ?
	RTL_W32(0xbb023018 , regValue);
	
	// SLIC_INSEL_CTRL
	regValue = RTL_R32(0xbb000174);
	regValue |= (1<<2);
	RTL_W32(0xbb000174 , regValue);
	
	RTL_W32(0xB8000600 , RTL_R32(0xB8000600) | (1<<26)| (1<<25));
	/*Disable PCM TX/RX*/
	RTL_W32(PCMCHCNR , 0);
#endif

#elif defined(RTL9601B)

//#define PLLControl 0xb8000204
//#define IPEnable 0xb8000600
//#define IO_MODE_EN	0xbb023004
	regValue = RTL_R32(0xb8000204);
	regValue = 0x4000000a;
	RTL_W32(0xb8000204 , regValue);
#if defined(ZSI)
	regValue = RTL_R32(0xb8000600);
	regValue |= (1<<26);// 0xc600ffff;
	//regValue = 0xc400ffff;
	RTL_W32(0xb8000600 , regValue);

	regValue = RTL_R32(0xbb023004);
	regValue |= ((0x2aaa)<<8);
	//regValue = 0x2aaa72;
	RTL_W32(0xbb023004 , regValue);	

#elif defined(ISI)
	regValue = RTL_R32(0xb8000600);
	regValue |= (1<<26);// 0xc600ffff;
	regValue |= (1<<25);// 0xc600ffff;
	RTL_W32(0xb8000600 , regValue);

	regValue = RTL_R32(0xbb023004);
	regValue |= ((0x1555)<<8);
	//regValue = 0x155572;
	RTL_W32(0xbb023004 , regValue);	
	
#endif

#endif


	/*PCM General Control Register: linear mode,enable PCM*/
	//RTL_W32(PCMCR,( ZSILBE | LINEAR_MODE | PCM_AE ));
	RTL_W32(PCMCR,( LINEAR_MODE | PCM_AE ));
	
	/*PCM interface Channel0-3 , 4-7 Time slot Assignment Register*/
	//RTL_W32(PCMTSR,0x00020406);
	RTL_W32(PCMTSR,0x02040600);
	//RTL_W32(PCMTSR47,0x00020406);
	RTL_W32(PCMTSR47,0x080a0c0e);
	
	
	/*PCM interface Buffer Size Register*/
	RTL_W32(PCMBSIZE , (0xfe << 24) | (0xfe << 16) | (0xfe << 8) | 0xfe);
	RTL_W32(PCMBSIZE47 , (0xfe << 24) | (0xfe << 16) | (0xfe << 8) | 0xfe);


	regValue = RTL_R32(PCMCR);
	printk("PCMCR = [%x]\n", regValue);	
	regValue = RTL_R32(0xbb023018);
	printk("0xbb023018 = [%x]\n", regValue);
	regValue = RTL_R32(0xB8000600);
	printk("0xB8000600 = [%x]\n", regValue);
	regValue = RTL_R32(0xbb000174);
	printk("0xbb000174 = [%x]\n", regValue);	
}

static void init_pcm (void)
{
	u32 regValue;
	
	regValue = RTL_R32(0xbb023004);
	//regValue = 0x3ffff72;
	regValue |= ((0x3ffff)<<8);
	RTL_W32(0xbb023004 , regValue);	
	
	regValue = RTL_R32(0xb8008000);
	regValue = 0x3800;
	RTL_W32(0xb8008000 , regValue);		
}

static void init_zsi (void)
{
	u32 regValue;
	
	// init SPI
	init_rtl_hw_spi_IP();
	_init_rtl_spi_dev_type_hw(&spi_dev[0], 0);	
	
	/*enable PCM module */
#if 0	
	regValue = RTL_R32(0xbb023018);
	regValue = regValue & 0xfff9ffff;       // enable ZSI
#endif

	regValue = RTL_R32(0xb8000600);
	regValue &= 0xf9ffffff;
	regValue |= (1<<26);
	RTL_W32(0xb8000600 , regValue);

	regValue = RTL_R32(0xbb023004);
	regValue |= ((0x2aaa)<<8);
	//regValue = 0x2aaa72;
	RTL_W32(0xbb023004 , regValue);	

	/*PCM General Control Register: linear mode,enable PCM*/
	//RTL_W32(PCMCR,( ZSILBE | LINEAR_MODE | PCM_AE ));
	RTL_W32(PCMCR,( LINEAR_MODE | PCM_AE ));
	
	/*PCM interface Channel0-3 , 4-7 Time slot Assignment Register*/
	RTL_W32(PCMTSR,0x00020406);
	RTL_W32(PCMTSR47,0x00020406);
	
	
	/*PCM interface Buffer Size Register*/
	RTL_W32(PCMBSIZE , (0xfe << 24) | (0xfe << 16) | (0xfe << 8) | 0xfe);
	RTL_W32(PCMBSIZE47 , (0xfe << 24) | (0xfe << 16) | (0xfe << 8) | 0xfe);


	regValue = RTL_R32(PCMCR);
	printk("PCMCR = [%x]\n", regValue);	
	regValue = RTL_R32(0xbb023018);
	printk("0xbb023018 = [%x]\n", regValue);
	regValue = RTL_R32(0xB8000600);
	printk("0xB8000600 = [%x]\n", regValue);
	regValue = RTL_R32(0xbb000174);
	printk("0xbb000174 = [%x]\n", regValue);		
}

static void init_isi (void)
{
	u32 regValue;
	
	// init SPI
	init_rtl_hw_spi_IP();
	_init_rtl_spi_dev_type_hw(&spi_dev[0], 0);	
	
#if 0	
	/*enable PCM module */
	regValue = RTL_R32(0xbb023018);
	regValue = regValue & 0xfff9ffff;       // enable ZSI
#endif	

	regValue = RTL_R32(0xb8000600);
	//regValue = 0xc600ffff;
	regValue |= (1<<26);// 0xc600ffff;
	regValue |= (1<<25);// 0xc600ffff;
	RTL_W32(0xb8000600 , regValue);

	regValue = RTL_R32(0xbb023004);
	//regValue = 0x155572;
	regValue |= ((0x1555)<<8);
	RTL_W32(0xbb023004 , regValue);	

	/*PCM General Control Register: linear mode,enable PCM*/
	//RTL_W32(PCMCR,( ZSILBE | LINEAR_MODE | PCM_AE ));
	RTL_W32(PCMCR,( LINEAR_MODE | PCM_AE ));
	
	/*PCM interface Channel0-3 , 4-7 Time slot Assignment Register*/
	RTL_W32(PCMTSR,0x00020406);
	RTL_W32(PCMTSR47,0x00020406);
	
	/*PCM interface Buffer Size Register*/
	RTL_W32(PCMBSIZE , (0xfe << 24) | (0xfe << 16) | (0xfe << 8) | 0xfe);
	RTL_W32(PCMBSIZE47 , (0xfe << 24) | (0xfe << 16) | (0xfe << 8) | 0xfe);

	regValue = RTL_R32(PCMCR);
	printk("PCMCR = [%x]\n", regValue);	
	regValue = RTL_R32(0xbb023018);
	printk("0xbb023018 = [%x]\n", regValue);
	regValue = RTL_R32(0xB8000600);
	printk("0xB8000600 = [%x]\n", regValue);
	regValue = RTL_R32(0xbb000174);
	printk("0xbb000174 = [%x]\n", regValue);	
}

static struct task_struct *pcm_task;
static int pcmdata;
static int kpcm(void *arg);
static int interfacetype = 2; // 1 : spi+pcm, 2 : zsi, 3 : isi 

static int kpcm(void *arg)
{
    unsigned int timeout;
    int *d = (int *) arg;

    for(;;) {
        if (kthread_should_stop()) break;
        //printk("%s %d\n", __FUNCTION__, (*d)++);
        do {
            set_current_state(TASK_INTERRUPTIBLE);
            timeout = schedule_timeout(50);
        } while(timeout);
		if ( interfacetype == 1 ){ // pcm 
			pcm_loopback(11, 0);			
			pcm_loopback(11, 1);			
			pcm_loopback(11, 2);			
			pcm_loopback(11, 3);			
		}else if ( interfacetype == 2 ){ // zsi
			pcm_loopback(15, 0);
			pcm_loopback(15, 1);
			pcm_loopback(15, 2);
			pcm_loopback(15, 3);
		}else if ( interfacetype == 3 ){ // isi
			pcm_loopback(13, 0);		
			pcm_loopback(13, 1);		
			pcm_loopback(13, 2);		
			pcm_loopback(13, 3);			
		}
			
    }
    printk("break\n");

    return 0;
}

static int pcm_test_read_proc(char *page, char **start, off_t off,
				int count, int *eof, void *data)
{
	printk("enter read proc..\n");
	
	return 0;
}

static int pcm_test_write_proc(struct file *file, const char *buffer,
				unsigned long count, void *data)
{
	char tmp[128];
	int t1 = 0;
	int ret = 0;
	
	if (count < 1)
		return -EFAULT;
	printk("enter write proc..\n");

	_init_rtl_spi_dev_type_hw(&spi_dev[0], 0);	
	
	re8670_pcm_init_hw();

	pcm_task = kthread_create(kpcm, &pcmdata, "pcm task");
    if (IS_ERR(pcm_task)) {
        ret = PTR_ERR(pcm_task);
        pcm_task = NULL;
        goto out;
    }
    wake_up_process(pcm_task);
	
	if (buffer && !copy_from_user(tmp, buffer, 128)) {
		sscanf(tmp, "%d", &t1);
		printk("test input : %d\n", t1);	
		RTL_W32(PCMCHCNR47, 0x00000000);
#if 0		
		if ( t1 < 10 )  // spi test
		{
			RTL_W32(PCMCHCNR, 0x03000000);
			spi_test_read(t1);			
		}else if ( t1 >= 10 ) // pcm test
		{
			//for ( i = 0 ; i < 8 ; i ++ )
				pcm_loopback(t1, 0);
		}
		//	RTL_W32(PCMCHCNR, 0xf3000000);
		
		//pcm_loopback(t1);
		//printk("Loopback test finish\n");
#else
		if ( t1 == 1 )
			interfacetype = 1;
		else if ( t1 == 3 )
			interfacetype = 2;
		else if ( t1 == 4 )
			interfacetype = 3;
				
#endif	
	}
	else
		return -EFAULT;
		
	
	return count;
	
out:
    return ret;	
	
}


static struct task_struct *gpio_tsk;

static int gpio_handle(void *arg)
{

	/*set pin use as GPIO*/
	RTL_W32(0xbb023004, 0x32);
	/*set gpio direction*/
	RTL_W32(0xb8003308, 0xffffffff);

	for(;;)
	{
		set_current_state(TASK_INTERRUPTIBLE);
		if (kthread_should_stop()) break;
		
		/*set GPO output value*/	
		RTL_W32(0xb800330c, 0xffffffff);
		/*delay 10us*/
		udelay(10);
		/*set GPO output value*/
		RTL_W32(0xb800330c, 0x0);
		/*delay 1ms*/
		msleep(1);
	}
	return 0;
}

static int  gpio_test_set(int state)
{
	int ret;

	if(state)
	{
		gpio_tsk = kthread_create(gpio_handle,NULL, "gpio_test");
		if (IS_ERR(gpio_tsk)) {
			ret = PTR_ERR(gpio_tsk);
			gpio_tsk = NULL;
			goto out;
		}
		wake_up_process(gpio_tsk);
	}
	else{
		kthread_stop(gpio_tsk);
	}

	return 0;

out:
	return ret;
}

static int gpio_test_read_proc(char *page, char **start, off_t off,
				int count, int *eof, void *data)
{
	printk("enter read proc..\n");
	
	return 0;
}

static int gpio_test_write_proc(struct file *file, const char *buffer,
				unsigned long count, void *data)
{
	char tmp[32] = "\0";
	int gpioState = 0;
	
	if (count < 1)
		return -EFAULT;
	
	printk("enter write proc..\n");
	
	if (buffer && !copy_from_user(tmp, buffer, count)) {
		gpioState = simple_strtol(tmp, NULL, 0);
		gpio_test_set(gpioState);
	}
	else
		return -EFAULT;
		
	
	return count;
	
}


static int  dram_test_set(int dramState)
{
	int ret;

	if(dramState > 0)
	{
		printk("start DRAM testing\n");
		
		/*Set Bus Traffic Generator to test memory*/
		IOAL_MEM32_WRITE(GDMACNR, 0x00000000);
		IOAL_MEM32_WRITE(GDMACNR, 0x10000000);
		IOAL_MEM32_WRITE(GDMACNR, 0x90000000);
		IOAL_MEM32_WRITE(GDMAIMR, 0x07300000);
		IOAL_MEM32_WRITE(GDMAISR, 0x07300000);
		IOAL_MEM32_WRITE(GDMAISR, 0x00000000);

		/*Write*/
		IOAL_MEM32_WRITE(BTGWCR, 0x00000000);
		IOAL_MEM32_WRITE(BTGWCR, 0x00100080);  /*Packet length 128 bytes*/
		IOAL_MEM32_WRITE(BTGWIR, 0x00000000);
		IOAL_MEM32_WRITE(BTGWRTR, 0x000000c8); /*200 cycle => 1us*/
		IOAL_MEM32_WRITE(BTGWPR, 0x000000c8);  /*200 cycle => 1us*/
		IOAL_MEM32_WRITE(BTGWBAR, 0x01A00000); /*start from 26M*/
		IOAL_MEM32_WRITE(BTGWAMR, 0x005fff00); /*range 6M - 255 bytes*/
		IOAL_MEM32_WRITE(BTGWGR, 0x00000080);  /*gap = 128 bytes*/
		
		IOAL_MEM32_WRITE(BTGWIV00, 0xa5a55a5a); /*BTG Write Input Vector*/
		IOAL_MEM32_WRITE(BTGWIV01, 0xffff0000);
		IOAL_MEM32_WRITE(BTGWIV02, 0x0000ffff);
		IOAL_MEM32_WRITE(BTGWIV03, 0xff00ff00);
		IOAL_MEM32_WRITE(BTGWIV04, 0x00ff00ff);
		IOAL_MEM32_WRITE(BTGWIV05, 0x5a5aa5a5);
		IOAL_MEM32_WRITE(BTGWIV06, 0x01234567);
		IOAL_MEM32_WRITE(BTGWIV07, 0x89abcdef);
		IOAL_MEM32_WRITE(BTGWIV08, 0xaaaa5555);
		IOAL_MEM32_WRITE(BTGWIV09, 0x5555aaaa);
		IOAL_MEM32_WRITE(BTGWIV10, 0xa5a55a5a);
		IOAL_MEM32_WRITE(BTGWIV11, 0xffff0000);
		IOAL_MEM32_WRITE(BTGWIV12, 0x0000ffff);
		IOAL_MEM32_WRITE(BTGWIV13, 0xff00ff00);
		IOAL_MEM32_WRITE(BTGWIV14, 0x00ff00ff);
		IOAL_MEM32_WRITE(BTGWIV15, 0xb801816c);

		IOAL_MEM32_WRITE(BTGWCR, 0x80100080);  /*Start to write, Packet length 128 bytes*/

		/*Read*/
		IOAL_MEM32_WRITE(BTGRCR, 0x00000000);
		IOAL_MEM32_WRITE(BTGRCR, 0x00100080);  /*Packet length 128 bytes*/
		IOAL_MEM32_WRITE(BTGRIR, 0x00000000);
		IOAL_MEM32_WRITE(BTGRRTR, 0x000000c8); /*200 cycle => 1us*/
		IOAL_MEM32_WRITE(BTGRPR, 0x000000c8);  /*200 cycle => 1us*/
		IOAL_MEM32_WRITE(BTGRBAR, 0x01A00000); /*start from 26M*/
		IOAL_MEM32_WRITE(BTGRAMR, 0x005fff00); /*range 6M - 255 bytes*/
		IOAL_MEM32_WRITE(BTGRGR, 0x00000080);  /*gap = 128 bytes*/
		
		IOAL_MEM32_WRITE(BTGRIV00, 0xa5a55a5a); /*BTG Write Input Vector*/
		IOAL_MEM32_WRITE(BTGRIV01, 0xffff0000);
		IOAL_MEM32_WRITE(BTGRIV02, 0x0000ffff);
		IOAL_MEM32_WRITE(BTGRIV03, 0xff00ff00);
		IOAL_MEM32_WRITE(BTGRIV04, 0x00ff00ff);
		IOAL_MEM32_WRITE(BTGRIV05, 0x5a5aa5a5);
		IOAL_MEM32_WRITE(BTGRIV06, 0x01234567);
		IOAL_MEM32_WRITE(BTGRIV07, 0x89abcdef);
		IOAL_MEM32_WRITE(BTGRIV08, 0xaaaa5555);
		IOAL_MEM32_WRITE(BTGRIV09, 0x5555aaaa);
		IOAL_MEM32_WRITE(BTGRIV10, 0xa5a55a5a);
		IOAL_MEM32_WRITE(BTGRIV11, 0xffff0000);
		IOAL_MEM32_WRITE(BTGRIV12, 0x0000ffff);
		IOAL_MEM32_WRITE(BTGRIV13, 0xff00ff00);
		IOAL_MEM32_WRITE(BTGRIV14, 0x00ff00ff);
		IOAL_MEM32_WRITE(BTGRIV15, 0xb801816c);

		IOAL_MEM32_WRITE(BTGRCR, 0x80100080);  /*Start to read, Packet length 128 bytes*/
		
	} 
	else{
		printk("stop DRAM testing\n");
		IOAL_MEM32_WRITE(BTGWCR, 0x00000000);
		IOAL_MEM32_WRITE(BTGRCR, 0x00000000);
	}

	return 0;
}


static int dram_test_read_proc(char *page, char **start, off_t off,
				int count, int *eof, void *data)
{
	printk("enter read proc..\n");
	
	return 0;
}

static int dram_test_write_proc(struct file *file, const char *buffer,
				unsigned long count, void *data)
{
	char tmp[32] = "\0";
	int dramState;
	
	
	if (count < 1)
		return -EFAULT;
	
	printk("enter write proc..\n");
	if (buffer && !copy_from_user(tmp, buffer, count)) {
		dramState = simple_strtol(tmp, NULL, 0);
		dram_test_set(dramState);
	}
	else
		return -EFAULT;
		
	
	return count;
	
}

static struct task_struct *gphy_tsk;

static int gphy_handle(void *arg)
{
	uint16 data;
	uint32 lastState;
	
	printk("Start check GPhy link\n");
	rtl9601b_ocpInterPhy_read(0, 0xa402, &data);
	printk("data = %x\n", data);
	while(1){
		
		set_current_state(TASK_INTERRUPTIBLE);
		if (kthread_should_stop()) break;
		
		rtl9601b_ocpInterPhy_read(0, 0xa402, &data);
		if((data & 0x4) && (lastState == 0)) /*linkdown to linkup*/
		{
			printk("linkdown to linkup\n");
			rtl9601b_ocpInterPhy_write(0, 0xc800, 0x5a23);
			lastState = 1;
		} else if (0 == (data & 0x4)){ /*linkdown*/
			lastState = 0;
		} else { /*linkup*/
			lastState = 1;
		}
		/*delay 1ms*/
		msleep(1);
	}	
	printk("Stop GPhy Test\n");
	return 0;
}



static int  gphy_test_set(int state)
{
	int ret;

	if(state)
	{
		gphy_tsk = kthread_create(gphy_handle, NULL, "gphy_test");
		if (IS_ERR(gphy_tsk)) {
			ret = PTR_ERR(gphy_tsk);
			gphy_tsk = NULL;
			goto out;
		}
		wake_up_process(gphy_tsk);
	}else{
		kthread_stop(gphy_tsk);
	}

	return 0;

out:
	return ret;
}
	
static int gphy_test_read_proc(char *page, char **start, off_t off,
				int count, int *eof, void *data)
{
	printk("enter read proc..\n");
	
	return 0;
}

static int gphy_test_write_proc(struct file *file, const char *buffer,
				unsigned long count, void *data)
{
	char flag[64];
	int state;
	
	if (count < 1)
		return -EFAULT;
	
	printk("enter write proc..\n");
	if (buffer && !copy_from_user(&flag, buffer, sizeof(flag))) {
		state = simple_strtol(flag,NULL, 0);
		gphy_test_set(state);
	}
	else
		return -EFAULT;

	return count;
	
}

static int __init operation_life_test_init(void)
{

	struct proc_dir_entry *dir = NULL;
	struct proc_dir_entry *pcm = NULL, *gpio = NULL, *dram = NULL, *gphy = NULL;
	
	printk("\n\n operation_life_test_init \n\n");

	dir = proc_mkdir("ol_test",NULL);

	pcm = create_proc_entry("pcm", 0, dir);
	if (pcm) {
		pcm->read_proc = pcm_test_read_proc;
		pcm->write_proc = pcm_test_write_proc;
	}
	else {
		printk("pcm_test, create proc failed!\n");
	}

	gpio = create_proc_entry("gpio", 0, dir);
	if (gpio) {
		gpio->read_proc = gpio_test_read_proc;
		gpio->write_proc = gpio_test_write_proc;
	}
	else {
		printk("gpio_test, create proc failed!\n");
	}

	dram = create_proc_entry("dram", 0, dir);
	if (dram) {
		dram->read_proc = dram_test_read_proc;
		dram->write_proc = dram_test_write_proc;
	}
	else {
		printk("dram_test, create proc failed!\n");
	}

	gphy = create_proc_entry("gphy", 0, dir);
	if (gphy) {
		gphy->read_proc = gphy_test_read_proc;
		gphy->write_proc = gphy_test_write_proc;
	}
	else {
		printk("gphy_test, create proc failed!\n");
	}

	return 0;
}


static int __exit operation_life_test_exit(void)
{
	printk("\n\n pcm_module_test_exit \n\n");
	return 0;
}


module_init(operation_life_test_init);
module_exit(operation_life_test_exit);

