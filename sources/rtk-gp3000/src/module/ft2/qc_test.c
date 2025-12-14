#include <linux/init.h>
#include <linux/module.h>
#include <linux/kthread.h>

#include <linux/interrupt.h>
#include <linux/delay.h>
#include <asm/io.h>
#include <linux/mm.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/dma-mapping.h>
#include "module/ft2/pcm.h"
#include "module/ft2/spi.h"

#include <rtk/port.h>
#include <rtk/acl.h>
#include <rtk/gpio.h>
#include <common/error.h>
#include <ioal/mac_debug.h>
#include <rtk/rtusr/rtusr_pkt.h>

#if defined(CONFIG_RTL9602C_SERIES)
#define RTL9602C 1
#elif defined(CONFIG_SDK_RTL9601B)
#define RTL9601B 1
#elif defined(CONFIG_RTL8685S)
#define RTL8685S 1
#elif defined(CONFIG_RTL8685)
#define RTL8685 1
#else
#define RTL8696 1
#endif

#if 0 
#define THREAD_TEST
#endif
#ifdef THREAD_TEST
static struct task_struct *pmclb_tsk;
static int data;
static int kpcmlbtest(void *arg);
#endif


static int log = 0;
#define printk_test(fmt,args...)  if(log) { printk(fmt, ##args); } 



//#define ZSI 1
//#define ISI 1
static void init_pcm (void);
static void init_zsi (void);
static void init_isi (void);
static inline void printk_off( const char *fmt, ... ) {}
static rtl_spi_dev_t            spi_dev[1];
#define VP890_DEVTYPE_RD            0x73
#define VP890_DEVTYPE_LEN           0x03    /**< RCN = 1st Byte, PCN = 2nd Byte */
#define VP890_EC_CH1            0x01
#define SPI_PRINTK              printk_off
#define SPI2_PRINTK              printk_off
#define QC_PRINTK              printk_off
#define PCM_BUF_SIZE 20
#define REG32(reg) (*(volatile unsigned int *)(reg))

#define IP_ENABLE 0xb8000600
#define PCMEN   (1<<2)
#define VOIP_PERI   (1<<0)

#if defined(RTL8696) || defined(RTL9601B) || defined(RTL9602C)

#if defined(RTL8696) || defined(RTL9602C)
#define IO_MODE_EN  0xbb023018
#define SLIC_PCM_EN (1<<18)
#define SLIC_SPI_EN (1<<17)
#define SLIC_ZSI_EN (1<<16)
#define SLIC_ISI_EN (1<<15)

#if defined(RTL8696)
#define SLIC_INSEL_CTRL 0xbb000174
#define SLIC_EN (1<<2)
#endif // defined(RTL8696)

#elif defined(RTL9601B)
#define PLLControl 0xb8000204
#define IO_MODE_EN  0xbb023004
#endif // defined(RTL8696) || defined(RTL9602C)

#endif // defined(RTL8696) || defined(RTL9601B) || defined(RTL9602C)

#ifdef RTL8685S
#define PINMUX  ( ( volatile unsigned int * )0xB800010C )
#define FRACENREG       ( ( volatile unsigned int * )0xB8000228 )
#define PLL_CLOCK_CONTROL       ( ( volatile unsigned int * )0xB8000204 )
//#define IP_ENABLE                ( ( volatile unsigned int * )0xB8000600 )
#define FRACEN  (1<<3)
#define BIT27   (1<<27)
#define BIT26   (1<<26)
#define BIT19   (1<<19)
#endif



#ifdef RTL9602C
// 0676 : 9602C
#define REG_SDS_CFG     0xBB0001D0
#define FIB_1G          BIT(2)

#define REG_FRC         0xBB022500
#define FRC_RX_EN_ON    BIT(4)
#define FRC_RX_EN_VAL   BIT(5)
#define FRC_CMU_EN_ON   BIT(10)
#define FRC_CMU_EN_VAL  BIT(11)

#define REG_REG42       0xBB0226A8
#define REG_PCM_CMU_EN  BIT(2)
#endif


static void print_reg(void)
{
    QC_PRINTK("PCMCR= 0x%x\n", pcm_inl(PCMCR));
    QC_PRINTK("PCMCHCNR= 0x%x\n", pcm_inl(PCMCHCNR));
    QC_PRINTK("PCMBSIZE= 0x%x\n", pcm_inl(PCMBSIZE));
    QC_PRINTK("CH0TXBSA= 0x%x\n", pcm_inl(CH0TXBSA));
    QC_PRINTK("CH0RXBSA= 0x%x\n", pcm_inl(CH0RXBSA));
    QC_PRINTK("CH1TXBSA= 0x%x\n", pcm_inl(CH1TXBSA));
    QC_PRINTK("CH1RXBSA= 0x%x\n", pcm_inl(CH1RXBSA));
    QC_PRINTK("CH2TXBSA= 0x%x\n", pcm_inl(CH2TXBSA));
    QC_PRINTK("CH2RXBSA= 0x%x\n", pcm_inl(CH2RXBSA));
    QC_PRINTK("CH3TXBSA= 0x%x\n", pcm_inl(CH3TXBSA));
    QC_PRINTK("CH3RXBSA= 0x%x\n", pcm_inl(CH3RXBSA));   
    QC_PRINTK("PCMTSR= 0x%x\n", pcm_inl(PCMTSR));
    QC_PRINTK("PCMIMR= 0x%x\n", pcm_inl(PCMIMR));
    QC_PRINTK("PCMISR= 0x%x\n", pcm_inl(PCMISR));   
    QC_PRINTK("PCMCHCNR47= 0x%x\n", pcm_inl(PCMCHCNR47));
    QC_PRINTK("PCMBSIZE47= 0x%x\n", pcm_inl(PCMBSIZE47));
    QC_PRINTK("CH4TXBSA= 0x%x\n", pcm_inl(CH4TXBSA));
    QC_PRINTK("CH4RXBSA= 0x%x\n", pcm_inl(CH4RXBSA));
    QC_PRINTK("CH5TXBSA= 0x%x\n", pcm_inl(CH5TXBSA));
    QC_PRINTK("CH5RXBSA= 0x%x\n", pcm_inl(CH5RXBSA));
    QC_PRINTK("CH6TXBSA= 0x%x\n", pcm_inl(CH6TXBSA));
    QC_PRINTK("CH6RXBSA= 0x%x\n", pcm_inl(CH6RXBSA));
    QC_PRINTK("CH7TXBSA= 0x%x\n", pcm_inl(CH7TXBSA));
    QC_PRINTK("CH7RXBSA= 0x%x\n", pcm_inl(CH7RXBSA));   
    QC_PRINTK("PCMTSR47= 0x%x\n", pcm_inl(PCMTSR47));
    QC_PRINTK("PCMIMR47= 0x%x\n", pcm_inl(PCMIMR47));
    QC_PRINTK("PCMISR47= 0x%x\n", pcm_inl(PCMISR47));
    QC_PRINTK("SPICNR   (%p)=%08X\n", pSPICNR, *pSPICNR );
    QC_PRINTK("SPISTR   (%p)=%08X\n", pSPISTR, *pSPISTR );
    QC_PRINTK("SPICKDIV (%p)=%08X\n", pSPICKDIV, *pSPICKDIV );
    QC_PRINTK("SPIRDR   (%p)=%08X\n", pSPIRDR, *pSPIRDR );
    QC_PRINTK("SPITDR   (%p)=%08X\n", pSPITDR, *pSPITDR );
    QC_PRINTK("SPITCR   (%p)=%08X\n", pSPITCR, *pSPITCR );
    QC_PRINTK("SPICDTCR0(%p)=%08X\n", pSPICDTCR0, *pSPICDTCR0 );
    QC_PRINTK("SPICDTCR1(%p)=%08X\n", pSPICDTCR1, *pSPICDTCR1 );
    QC_PRINTK("SPITCALR (%p)=%08X\n", pSPITCALR, *pSPITCALR );  
}

static inline int32 __rtl_hw_spi_chip_select( rtl_spi_dev_t* pDev, int select )
{
    SPI2_PRINTK("[%s][%d] pDev ->type = [%x]\n", __FUNCTION__, __LINE__, pDev ->type);
    switch( pDev ->type ) {
    case SPI_TYPE_HW:   // use SPI CS 
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
        QC_PRINTK( "unknown spi type=%d\n", pDev ->type );
        return FAILED;
        break;
    }
    
    return SUCCESS;
}

static inline int32 _rtl_hw_spi_chip_select( rtl_spi_dev_t* pDev )
{
    return __rtl_hw_spi_chip_select( pDev, 1 ); // chip select 
}

static inline int32 _rtl_hw_spi_chip_deselect( rtl_spi_dev_t* pDev )
{
    return __rtl_hw_spi_chip_select( pDev, 0 ); // chip deselect 
}

static inline void __rtl_hw_spi_phase_select( rtl_spi_dev_t* pDev, int phase )
{
    SPI2_PRINTK("[%s][%d] phase = [%x]\n", __FUNCTION__, __LINE__,phase);
    // 1 or 3 phase 
    switch( phase ) {
    case 1:     // Zarlink SLIC 
        // 1 phase use 'D0' only 
        SPI2_PRINTK("[%s][%d]\n", __FUNCTION__, __LINE__);
        *pSPITCR &= ~( SPITCR_CTL_EN | SPITCR_ADD_EN | SPITCR_D0_EN | SPITCR_D1_EN );
        *pSPITCR |= SPITCR_D0_EN;
        break;
        
    case 3:     // Silab SLIC 
        // 3 phases use 'CTL', 'ADD' and 'D0' 
        SPI2_PRINTK("[%s][%d]\n", __FUNCTION__, __LINE__);
        *pSPITCR &= ~( SPITCR_CTL_EN | SPITCR_ADD_EN | SPITCR_D0_EN | SPITCR_D1_EN );
        *pSPITCR |= SPITCR_CTL_EN | SPITCR_ADD_EN | SPITCR_D0_EN;
        break;
        
    default:
        QC_PRINTK( "unknown spi phase=%d\n", phase );
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
    *pSPIRDR = 0xDE << SPIRDR_RD0R_SHIFT;   // use magic number instead of 0 
    
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
    *pSPICKDIV = ( 0x7 << SPICKDIV_DIV_SHIFT ); // 7.8M
    //*pSPICKDIV = ( 0x13 << SPICKDIV_DIV_SHIFT );  // 3.125M
    
    // transaction configuration register 
    *pSPITCR =  ( SPITCR_D0_EN ) |
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
static void spi_test_read(int type)
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
            QC_PRINTK("cmd = [%x]\n", cmd);
            _rtl_spi_rawWrite(( rtl_spi_dev_t * )spi_dev, &cmd, 8);
        }
        else if ( type == 1 )
        {
            cmd = 0xf2;
            data = 0x50;
            QC_PRINTK("cmd = [%x], data = [%x]\n", cmd, data);
            _rtl_spi_rawWrite(( rtl_spi_dev_t * )spi_dev, &cmd, 8);
            _rtl_spi_rawWrite(( rtl_spi_dev_t * )spi_dev, &data, 8);
        }

        if ( type == 2 )
        {   
            cmd = 0xfd;
            QC_PRINTK("cmd = [%x]\n", cmd);
            _rtl_spi_rawWrite(( rtl_spi_dev_t * )spi_dev, &cmd, 8);
            _rtl_spi_rawWrite( ( rtl_spi_dev_t * )spi_dev, &ecVal, 8);

            for(byteCnt=0; byteCnt < 4; byteCnt++){
                _rtl_spi_rawRead( ( rtl_spi_dev_t * )spi_dev, &devType[byteCnt], 8);
            }
            for ( byteCnt = 0 ; byteCnt < 4 ; byteCnt ++ )
                QC_PRINTK("[%02x]", devType[byteCnt]);
        }
            
        QC_PRINTK("\n");
    }   
#if 0
    QC_PRINTK("\nsend read id command..\n");
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

        QC_PRINTK("read id..\n");
        for(byteCnt=0; byteCnt < VP890_DEVTYPE_LEN; byteCnt++){
            _rtl_spi_rawRead( ( rtl_spi_dev_t * )spi_dev, &devType[byteCnt], 8);;
        }
        QC_PRINTK("id : ");
        for ( byteCnt = 0 ; byteCnt < VP890_DEVTYPE_LEN ; byteCnt ++ )
            QC_PRINTK("[%02x]", devType[byteCnt]);
            
        QC_PRINTK("\n");
    }

    if ( type <= 2 )
    {
        for ( i = 0 ; i < 5 ; i ++ )
        {
            cmd = 0x4d;
            QC_PRINTK("write 0x4d..\n");
            _rtl_spi_rawWrite(( rtl_spi_dev_t * )spi_dev, &cmd, 8);

            QC_PRINTK("read 0x4d..\n");
            for(byteCnt=0; byteCnt < 2; byteCnt++){
                _rtl_spi_rawRead( ( rtl_spi_dev_t * )spi_dev, &devType[byteCnt], 8);            
            }
            for ( byteCnt = 0 ; byteCnt < VP890_DEVTYPE_LEN ; byteCnt ++ )
                QC_PRINTK("[%02x]", devType[byteCnt]);      
        }
        QC_PRINTK("\n");
        
        for ( i = 0 ; i < 5 ; i ++ )
        {
            cmd = 0x47;
            QC_PRINTK("write 0x47..\n");
            _rtl_spi_rawWrite(( rtl_spi_dev_t * )spi_dev, &cmd, 8);

            QC_PRINTK("read 0x47..\n");
            for(byteCnt = 0; byteCnt < 1; byteCnt++){
                _rtl_spi_rawRead( ( rtl_spi_dev_t * )spi_dev, &devType[byteCnt], 8);            
            }
            for ( byteCnt = 0 ; byteCnt < 1 ; byteCnt ++ )
                QC_PRINTK("[%02x]", devType[byteCnt]);      
        }
    }
#endif
    QC_PRINTK("\n");    
    print_reg();
}


#if defined(RTL9601B) || defined(RTL9602C) || defined(RTL8685) || defined(RTL8685S)

static int eqc_comparedata2(int type, unsigned long *rxbuf)
{
    unsigned long *rxtmpbuf;
    int i = 0;
    unsigned long rxtmpbufISI[PCM_BUF_SIZE] = {0x0, 0x5a0000, 0xb40000, 0x1680000, 
                                               0x2d00000, 0x5a00000, 0xb400000, 0x16800000, 
                                               0x2d000000, 0x5a000000, 0xb4000000, 0x68000002, 
                                               0xd0000004, 0xa000000a, 0x40000016, 0x8000002c,
                                               0x5a, 0xb4, 0x168, 0x2d0};
                                               
    unsigned long rxtmpbufZSI[PCM_BUF_SIZE] = {0x0, 0x2d0000, 0x5a0000, 0xb40000, 
                                            0x1680000, 0x2d00000, 0x5a00000, 0xb400000, 
                                            0x16800000, 0x2d000000, 0x5a000000, 0x34000001, 
                                            0x68000002, 0x50000005, 0x2000000b, 0x40000016, 
                                            0x2d, 0x5a, 0xb4, 0x168};               

    if ( type == 0 )
        rxtmpbuf = &rxtmpbufISI[0];
    else if ( type == 1 )
        rxtmpbuf = &rxtmpbufZSI[0];
    
    for(i=0;i<PCM_BUF_SIZE;i++)
    {
        if ( (rxbuf[i]&0xFFFEFFFE) != rxtmpbuf[i] ){
            for(i=0;i<PCM_BUF_SIZE;i++)
            {
                QC_PRINTK("rxbuf[%d] = %08x , rxtmpbuf[%d] = %08x \n", i , (rxbuf[i]&0xFFFEFFFE) , i , rxtmpbuf[i]);
            }           
            return 0;
        }
    }
    return 1;                                   
}

static int eqc_comparedata(int type, unsigned long *txbuf, unsigned long *rxbuf)
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
            //QC_PRINTK("11 : tx[%d] = %x , rx[%d] = %x \n", i , txbuf[i] , i , rxbuf[i]);
            if ( txbuf[i] != (rxbuf[i]&0xFFFEFFFE) ){
                if ( eqc_comparedata2(0, rxbuf) == 0 )
                    return 0;
            }else
                continue;
        }
    }else if ( type == 1 ){
        for ( i = 0 ; i < PCM_BUF_SIZE ; i ++ )
        {
            //if ( (rxbuf[i]&0xFFFEFFFE) != rxtmpbuf[i] ) {
            if ( rxbuf[i] != rxtmpbuf[i] ) {
                if ( eqc_comparedata2(1, rxbuf) == 0 )
                    return 0;
            }else
                continue;
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
    int i, ret = 0;
    dma_addr_t tx_dma , rx_dma;
    volatile unsigned long *txbuf , *rxbuf, *xmit_tmp;
    unsigned int pcm_tx_rx_enable=0;
    unsigned char tx_data_temp=0x5a;
    QC_PRINTK("test channel number : [%d], type = [%d]\n", chid, type);
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
        //QC_PRINTK("type = [%d] : PCMCR,( ISILBE | LINEAR_MODE | PCM_AE )\n", type);
    }else if ( type == 15 ){
        init_zsi();
        RTL_W32(PCMCR,( LINEAR_MODE | PCM_AE ));
        //QC_PRINTK("type = [%d] : PCMCR,( LINEAR_MODE | PCM_AE )\n", type);
    }else if ( type == 11 ){
        init_pcm();
        RTL_W32(PCMCR,( LINEAR_MODE | PCM_AE ));
    }

    /*PCM interface Buffer Size Register*/
    txbuf = dma_alloc_coherent(NULL , PCM_BUF_SIZE*sizeof(long)  , &tx_dma, GFP_KERNEL);
    rxbuf = dma_alloc_coherent(NULL , PCM_BUF_SIZE*sizeof(long)  , &rx_dma, GFP_KERNEL);
    //QC_PRINTK("txmda = %x , rxdma = %x \n" , tx_dma , rx_dma);
    for( i=0; i<PCM_BUF_SIZE; i++)
    {
        rxbuf[i] = 0xffffffff;
        txbuf[i] = 0x0;
    }
    
    switch ( chid ){
        case 0:
            xmit_tmp = txbuf;
            QC_PRINTK("xmit_tmp=%p ,txbuf=%p \n", xmit_tmp, txbuf);
            // set data pointer and change owner to pcm controller
            RTL_W32(CH0TXBSA,tx_dma|0x3);
            QC_PRINTK("CH0TXBSA = 0x%x   \n",RTL_R32(CH0TXBSA));
            for ( i=0; i<PCM_BUF_SIZE; i++)
            {
                //external loop back has to ignore bit 0 and bit 16
                 *(xmit_tmp+i)= (tx_data_temp << (i%28))&0xFFFEFFFE; 
            }           
            //wmb();
            xmit_tmp = rxbuf;
            RTL_W32(CH0RXBSA, rx_dma|0x3);
            QC_PRINTK("CH0RXBSA = 0x%x   \n",RTL_R32(CH0RXBSA));
            pcm_tx_rx_enable |= 3<<24;
            wmb();
            RTL_W32(PCMCHCNR, pcm_tx_rx_enable);            
            break;
        case 1:     
            xmit_tmp = txbuf;
            QC_PRINTK("xmit_tmp=%p ,txbuf=%p \n", xmit_tmp, txbuf);
            // set data pointer and change owner to pcm controller
            RTL_W32(CH1TXBSA,tx_dma|0x3);
            QC_PRINTK("CH1TXBSA = 0x%x   \n",RTL_R32(CH1TXBSA));
            for ( i=0; i<PCM_BUF_SIZE; i++)
            {
                //external loop back has to ignore bit 0 and bit 16
                 *(xmit_tmp+i)= (tx_data_temp << (i%28))&0xFFFEFFFE; 
            }           
            //wmb();
            xmit_tmp = rxbuf;
            RTL_W32(CH1RXBSA, rx_dma|0x3);
            QC_PRINTK("CH1RXBSA = 0x%x   \n",RTL_R32(CH1RXBSA));
            pcm_tx_rx_enable |= 3<<16;
    
            break;
        case 2: 
            xmit_tmp = txbuf;
            QC_PRINTK("xmit_tmp=%p ,txbuf=%p \n", xmit_tmp, txbuf);
            // set data pointer and change owner to pcm controller
            RTL_W32(CH2TXBSA,tx_dma|0x3);
            QC_PRINTK("CH2TXBSA = 0x%x   \n",RTL_R32(CH2TXBSA));
            for ( i=0; i<PCM_BUF_SIZE; i++)
            {
                //external loop back has to ignore bit 0 and bit 16
                 *(xmit_tmp+i)= (tx_data_temp << (i%28))&0xFFFEFFFE; 
            }           
            //wmb();
            xmit_tmp = rxbuf;
            RTL_W32(CH2RXBSA, rx_dma|0x3);
            QC_PRINTK("CH2RXBSA = 0x%x   \n",RTL_R32(CH2RXBSA));
            pcm_tx_rx_enable |= 3<<8;
            wmb();
            RTL_W32(PCMCHCNR, pcm_tx_rx_enable);    
            break;  
        case 3:     
            xmit_tmp = txbuf;
            QC_PRINTK("xmit_tmp=%p ,txbuf=%p \n", xmit_tmp, txbuf);
            // set data pointer and change owner to pcm controller
            RTL_W32(CH3TXBSA,tx_dma|0x3);
            QC_PRINTK("CH3TXBSA = 0x%x   \n",RTL_R32(CH3TXBSA));
            for ( i=0; i<PCM_BUF_SIZE; i++)
            {
                //external loop back has to ignore bit 0 and bit 16
                 *(xmit_tmp+i)= (tx_data_temp << (i%28))&0xFFFEFFFE; 
            }           
            //wmb();
            xmit_tmp = rxbuf;
            RTL_W32(CH3RXBSA, rx_dma|0x3);
            QC_PRINTK("CH3RXBSA = 0x%x   \n",RTL_R32(CH3RXBSA));
            pcm_tx_rx_enable |= 3;
            wmb();
            RTL_W32(PCMCHCNR, pcm_tx_rx_enable);    
            break;  
        case 4:     
            xmit_tmp = txbuf;
            QC_PRINTK("xmit_tmp=%p ,txbuf=%p \n", xmit_tmp, txbuf);
            // set data pointer and change owner to pcm controller
            RTL_W32(CH4TXBSA,tx_dma|0x3);
            QC_PRINTK("CH4TXBSA = 0x%x   \n",RTL_R32(CH4TXBSA));
            for ( i=0; i<PCM_BUF_SIZE; i++)
            {
                //external loop back has to ignore bit 0 and bit 16
                 *(xmit_tmp+i)= (tx_data_temp << (i%28))&0xFFFEFFFE; 
            }           
            //wmb();
            xmit_tmp = rxbuf;
            RTL_W32(CH4RXBSA, rx_dma|0x3);
            QC_PRINTK("CH4RXBSA = 0x%x   \n",RTL_R32(CH4RXBSA));
            pcm_tx_rx_enable |= 3<<24;
            break;  
        case 5:
            xmit_tmp = txbuf;
            QC_PRINTK("xmit_tmp=%p ,txbuf=%p \n", xmit_tmp, txbuf);
            // set data pointer and change owner to pcm controller
            RTL_W32(CH5TXBSA,tx_dma|0x3);
            QC_PRINTK("CH5TXBSA = 0x%x   \n",RTL_R32(CH5TXBSA));
            for ( i=0; i<PCM_BUF_SIZE; i++)
            {
                //external loop back has to ignore bit 0 and bit 16
                 *(xmit_tmp+i)= (tx_data_temp << (i%28))&0xFFFEFFFE; 
            }           
            //wmb();
            xmit_tmp = rxbuf;
            RTL_W32(CH5RXBSA, rx_dma|0x3);
            QC_PRINTK("CH5RXBSA = 0x%x   \n",RTL_R32(CH5RXBSA));
            pcm_tx_rx_enable |= 3<<16;
            break;  
        case 6:         
            xmit_tmp = txbuf;
            QC_PRINTK("xmit_tmp=%p ,txbuf=%p \n", xmit_tmp, txbuf);
            // set data pointer and change owner to pcm controller
            RTL_W32(CH6TXBSA,tx_dma|0x3);
            QC_PRINTK("CH6TXBSA = 0x%x   \n",RTL_R32(CH6TXBSA));
            for ( i=0; i<PCM_BUF_SIZE; i++)
            {
                //external loop back has to ignore bit 0 and bit 16
                 *(xmit_tmp+i)= (tx_data_temp << (i%28))&0xFFFEFFFE; 
            }           
            //wmb();
            xmit_tmp = rxbuf;
            RTL_W32(CH6RXBSA, rx_dma|0x3);
            QC_PRINTK("CH6RXBSA = 0x%x   \n",RTL_R32(CH6RXBSA));
            pcm_tx_rx_enable |= 3<<8;
            break;  
        case 7:
            xmit_tmp = txbuf;
            QC_PRINTK("xmit_tmp=%p ,txbuf=%p \n", xmit_tmp, txbuf);
            // set data pointer and change owner to pcm controller
            RTL_W32(CH7TXBSA,tx_dma|0x3);
            QC_PRINTK("CH7TXBSA = 0x%x   \n",RTL_R32(CH7TXBSA));
            for ( i=0; i<PCM_BUF_SIZE; i++)
            {
                //external loop back has to ignore bit 0 and bit 16
                 *(xmit_tmp+i)= (tx_data_temp << (i%28))&0xFFFEFFFE; 
            }           
            //wmb();
            xmit_tmp = rxbuf;
            RTL_W32(CH7RXBSA, rx_dma|0x3);
            QC_PRINTK("CH7RXBSA = 0x%x   \n",RTL_R32(CH7RXBSA));
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
    QC_PRINTK("compare data\n");
    for(i=0;i<PCM_BUF_SIZE;i++)
    {
        QC_PRINTK("tx[%d] = %x , rx[%d] = %x \n", i , txbuf[i] , i , (rxbuf[i]&0xFFFEFFFE));
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

    u32 regValue;
    if ( type == 13 ){ // ISI
        ret = eqc_comparedata(0, txbuf, rxbuf);
    }else if ( type == 15 ){    // ZSI
        ret = eqc_comparedata(1, txbuf, rxbuf);
    }       
    
    regValue = RTL_R32(0xbb0231c0);
    
    if ( !ret ){
        //QC_PRINTK("0 : 0xbb0231c0 = [%x]\n", regValue);
        RTL_W32(0xbb0231c0 , 0x0);
    }else{
        //QC_PRINTK("1 : 0xbb0231c0 = [%x]\n", regValue);   
        if ( type == 13 )   // ISI
            RTL_W32(0xbb0231c0 , 0x3f);
        else if ( type == 15 )  // ZSI
            RTL_W32(0xbb0231c0 , 0x3e);
    }
    regValue = RTL_R32(0xbb0231c0);
    QC_PRINTK("0xbb0231c0 = [%x]\n", regValue); 
#elif defined (RTL9602C)
    if ( type == 13 || type == 11){ // ISI
        ret = eqc_comparedata(0, txbuf, rxbuf);
    }else if ( type == 15 ){    // ZSI
        ret = eqc_comparedata(1, txbuf, rxbuf);
    }       
    if ( !ret ){
        //QC_PRINTK("0 : 0xbb0231c0 = [%x]\n", regValue);
        //RTL_W32(0xbb0231c0 , 0x0);
        printk_test("Fail\n");   
    }else{
        printk_test("Pass\n");   
    }
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
    regValue = RTL_R32(0xb8000204);
    regValue = 0x4000000a;
    RTL_W32(0xb8000204 , regValue);
#if defined(ZSI)
    regValue = RTL_R32(0xb8000600);
    regValue |= (1<<26);// 0xc600ffff;
    //regValue = 0xc400ffff;
    RTL_W32(0xb8000600 , regValue);

    regValue = RTL_R32(0xbb023004);
    regValue = 0x2aaa72;
    RTL_W32(0xbb023004 , regValue); 
#elif defined(ISI)
    regValue = RTL_R32(0xb8000600);
    regValue |= (1<<26);// 0xc600ffff;
    regValue |= (1<<25);// 0xc600ffff;
    RTL_W32(0xb8000600 , regValue);

    regValue = RTL_R32(0xbb023004);
    regValue = 0x155572;
    RTL_W32(0xbb023004 , regValue); 
    
#endif


#elif defined(RTL8685S)
    regValue = *FRACENREG;
    regValue &= ~BIT19;
    *FRACENREG = regValue;

    regValue = *FRACENREG;
    regValue |= BIT19;
    *FRACENREG = regValue;

    regValue = *PLL_CLOCK_CONTROL;
    regValue |= FRACEN;
    *PLL_CLOCK_CONTROL = regValue;
    
#elif defined(RTL8685)
    // for RLE6318 E version, reset divider circuit
    unsigned int reg_tmp = 0;
    REG32(0xbb804004) = 0x841f0bc0;
    mdelay(1);
    REG32(0xbb804004) = 0x04110000;
    mdelay(1);
    reg_tmp = (REG32(0xbb804008)&0x0FFFF)|(1<<3)|0x84110000;

    mdelay(1);
    REG32(0xbb804004) = reg_tmp;
    mdelay(2);
    REG32(0xbb804004) = 0x04110000;
    mdelay(1);

#elif defined(RTL9602C)
#if 0
    regValue = RTL_R32(REG_SDS_CFG);
    regValue = FIB_1G;
    RTL_W32(REG_SDS_CFG, regValue);

    regValue = RTL_R32(REG_FRC);
    regValue &= ~(FRC_RX_EN_VAL|FRC_RX_EN_VAL);
    regValue |= (FRC_RX_EN_ON|FRC_CMU_EN_ON);
    RTL_W32(REG_FRC, regValue);

//  REG32(REG_FRC) &= ~(FRC_RX_EN_VAL|FRC_RX_EN_VAL);
//  REG32(REG_FRC) |= (FRC_RX_EN_ON|FRC_CMU_EN_ON);
    regValue = RTL_R32(REG_REG42);
    regValue |= (REG_PCM_CMU_EN);
    RTL_W32(REG_REG42, regValue);
    
    regValue = RTL_R32(REG_REG42);
    regValue &= ~(REG_PCM_CMU_EN);
    RTL_W32(REG_REG42, regValue);

    regValue = RTL_R32(REG_REG42);
    regValue |= (REG_PCM_CMU_EN);
    RTL_W32(REG_REG42, regValue);
//  REG32(REG_REG42) |= (REG_PCM_CMU_EN);
//  REG32(REG_REG42) &= ~(REG_PCM_CMU_EN);
//  REG32(REG_REG42) |= (REG_PCM_CMU_EN);
#endif
#endif

    /*PCM General Control Register: linear mode,enable PCM*/
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
    QC_PRINTK("PCMCR = [%x]\n", regValue);  
    regValue = RTL_R32(0xbb023018);
    QC_PRINTK("0xbb023018 = [%x]\n", regValue);
    regValue = RTL_R32(0xB8000600);
    QC_PRINTK("0xB8000600 = [%x]\n", regValue);
#if defined(RTL8696)
    regValue = RTL_R32(0xbb000174);
    QC_PRINTK("0xbb000174 = [%x]\n", regValue); 
#endif
}

static void init_pcm (void)
{
    u32 regValue;
    
    // init SPI
    init_rtl_hw_spi_IP();
    //_init_rtl_spi_dev_type_hw(&spi_dev[0], 0);    

#if defined(RTL8685S)
    regValue = RTL_R32(FRACENREG);
    regValue &= ~BIT19;
    RTL_W32(FRACENREG, regValue);
    
    regValue = RTL_R32(FRACENREG);
    regValue |= BIT19;
    RTL_W32(FRACENREG, regValue);   

    regValue = RTL_R32(PLL_CLOCK_CONTROL);
    regValue |= FRACEN;
    RTL_W32(PLL_CLOCK_CONTROL, regValue);
    
    regValue = RTL_R32(IP_ENABLE);
    regValue &= 0xf3ffffff;
    regValue |= (PCMEN|VOIP_PERI);
    RTL_W32(IP_ENABLE , regValue);
#elif defined(RTL8685)
    // for RLE6318 E version, reset divider circuit
    unsigned int reg_tmp = 0;
    REG32(0xbb804004) = 0x841f0bc0;
    mdelay(1);
    REG32(0xbb804004) = 0x04110000;
    mdelay(1);
    reg_tmp = (REG32(0xbb804008)&0x0FFFF)|(1<<3)|0x84110000;

    mdelay(1);
    REG32(0xbb804004) = reg_tmp;
    mdelay(2);
    REG32(0xbb804004) = 0x04110000;
    mdelay(1);  
    
    // enable PCM IP
    regValue = RTL_R32(IP_ENABLE);
    regValue &= 0xf3ffffff;
    regValue |= (PCMEN|VOIP_PERI);
    RTL_W32(IP_ENABLE , regValue);      
#elif defined(RTL9602C)
#if 0
    regValue = RTL_R32(REG_SDS_CFG);
    regValue = FIB_1G;
    RTL_W32(REG_SDS_CFG, regValue);

    regValue = RTL_R32(REG_FRC);
    regValue &= ~(FRC_RX_EN_VAL|FRC_RX_EN_VAL);
    regValue |= (FRC_RX_EN_ON|FRC_CMU_EN_ON);
    RTL_W32(REG_FRC, regValue);

//  REG32(REG_FRC) &= ~(FRC_RX_EN_VAL|FRC_RX_EN_VAL);
//  REG32(REG_FRC) |= (FRC_RX_EN_ON|FRC_CMU_EN_ON);
    regValue = RTL_R32(REG_REG42);
    regValue |= (REG_PCM_CMU_EN);
    RTL_W32(REG_REG42, regValue);
    
    regValue = RTL_R32(REG_REG42);
    regValue &= ~(REG_PCM_CMU_EN);
    RTL_W32(REG_REG42, regValue);

    regValue = RTL_R32(REG_REG42);
    regValue |= (REG_PCM_CMU_EN);
    RTL_W32(REG_REG42, regValue);
//  REG32(REG_REG42) |= (REG_PCM_CMU_EN);
//  REG32(REG_REG42) &= ~(REG_PCM_CMU_EN);
//  REG32(REG_REG42) |= (REG_PCM_CMU_EN);
#endif
#endif
    
#if defined(RTL9601B) || defined(RTL8696) || defined(RTL9602C)  
    regValue = RTL_R32(IP_ENABLE );
    regValue &= 0xf9ffffff; // clear bit [26:25]
    regValue |= (PCMEN|VOIP_PERI);
    RTL_W32(IP_ENABLE  , regValue);
#if defined(RTL8696) || defined(RTL9602C)
    /*enable PCM module */
    regValue = RTL_R32(IO_MODE_EN);
    regValue = regValue & 0xfff87fff;       // clear bit [18:15]
    regValue |= (SLIC_PCM_EN|SLIC_SPI_EN);
    RTL_W32(IO_MODE_EN , regValue);
#elif defined(RTL9601B)
    regValue = RTL_R32(IO_MODE_EN);
    regValue = 0x2aaa72;
    RTL_W32(IO_MODE_EN , regValue); 
#endif
#endif

    /*PCM General Control Register: linear mode,enable PCM*/
    RTL_W32(PCMCR,( LINEAR_MODE | PCM_AE ));
    
    /*PCM interface Channel0-3 , 4-7 Time slot Assignment Register*/
    RTL_W32(PCMTSR,0x00020406);
    RTL_W32(PCMTSR47,0x00020406);
    
    /*PCM interface Buffer Size Register*/
    RTL_W32(PCMBSIZE , (0xfe << 24) | (0xfe << 16) | (0xfe << 8) | 0xfe);
    RTL_W32(PCMBSIZE47 , (0xfe << 24) | (0xfe << 16) | (0xfe << 8) | 0xfe);

    regValue = RTL_R32(PCMCR);
    QC_PRINTK("PCMCR = [%x]\n", regValue);  
#if defined(RTL9601B) || defined(RTL8696) || defined(RTL9602C)      
    regValue = RTL_R32(0xbb023018);
    QC_PRINTK("0xbb023018 = [%x]\n", regValue);
#endif  
    regValue = RTL_R32(0xB8000600);
    QC_PRINTK("0xB8000600 = [%x]\n", regValue);
}

static void init_zsi (void)
{
    u32 regValue;
    
    // init SPI
    init_rtl_hw_spi_IP();
    //_init_rtl_spi_dev_type_hw(&spi_dev[0], 0);    
#if defined(RTL8685S)
    regValue = RTL_R32(FRACENREG);
    regValue &= ~BIT19;
    RTL_W32(FRACENREG, regValue);
    
    regValue = RTL_R32(FRACENREG);
    regValue |= BIT19;
    RTL_W32(FRACENREG, regValue);   

    regValue = RTL_R32(PLL_CLOCK_CONTROL);
    regValue |= FRACEN;
    RTL_W32(PLL_CLOCK_CONTROL, regValue);

    regValue = RTL_R32(IP_ENABLE);
    regValue |= ((BIT27)|(PCMEN)|(VOIP_PERI));
    RTL_W32(IP_ENABLE, regValue);

#elif defined(RTL8685)
    // for RLE6318 E version, reset divider circuit
    unsigned int reg_tmp = 0;
    REG32(0xbb804004) = 0x841f0bc0;
    mdelay(1);
    REG32(0xbb804004) = 0x04110000;
    mdelay(1);
    reg_tmp = (REG32(0xbb804008)&0x0FFFF)|(1<<3)|0x84110000;

    mdelay(1);
    REG32(0xbb804004) = reg_tmp;
    mdelay(2);
    REG32(0xbb804004) = 0x04110000;
    mdelay(1);  
    
    // enable ZSI IP
    regValue = RTL_R32(IP_ENABLE);
    regValue &= 0xf3ffffff;
    regValue |= ((1<<27)|PCMEN|VOIP_PERI);
    RTL_W32(IP_ENABLE, regValue);   
#elif defined(RTL9602C)
#if 0
    regValue = RTL_R32(REG_SDS_CFG);
    regValue = FIB_1G;
    RTL_W32(REG_SDS_CFG, regValue);

    regValue = RTL_R32(REG_FRC);
    regValue &= ~(FRC_RX_EN_VAL|FRC_RX_EN_VAL);
    regValue |= (FRC_RX_EN_ON|FRC_CMU_EN_ON);
    RTL_W32(REG_FRC, regValue);

//  REG32(REG_FRC) &= ~(FRC_RX_EN_VAL|FRC_RX_EN_VAL);
//  REG32(REG_FRC) |= (FRC_RX_EN_ON|FRC_CMU_EN_ON);
    regValue = RTL_R32(REG_REG42);
    regValue |= (REG_PCM_CMU_EN);
    RTL_W32(REG_REG42, regValue);
    
    regValue = RTL_R32(REG_REG42);
    regValue &= ~(REG_PCM_CMU_EN);
    RTL_W32(REG_REG42, regValue);

    regValue = RTL_R32(REG_REG42);
    regValue |= (REG_PCM_CMU_EN);
    RTL_W32(REG_REG42, regValue);
//  REG32(REG_REG42) |= (REG_PCM_CMU_EN);
//  REG32(REG_REG42) &= ~(REG_PCM_CMU_EN);
//  REG32(REG_REG42) |= (REG_PCM_CMU_EN);
#endif  
#endif  
    
    /*enable PCM module */
#if defined(RTL9601B) || defined(RTL8696) || defined(RTL9602C)  
    regValue = RTL_R32(IP_ENABLE );
    regValue &= 0xf9ffffff; // clear bit [26:25]
    regValue |= ((1<<26)|PCMEN|VOIP_PERI); //0xc400ffff;
    RTL_W32(IP_ENABLE, regValue);

#if defined(RTL8696) || defined(RTL9602C)   
    regValue = RTL_R32(IO_MODE_EN);
    regValue = regValue & 0xfff87fff;       // clear bit[18:15]
    regValue |= SLIC_ZSI_EN;
    RTL_W32(IO_MODE_EN , regValue);
    
#if defined(RTL8696)
    regValue = RTL_R32(SLIC_INSEL_CTRL);
    regValue |= SLIC_EN;
    RTL_W32(IO_MODE_EN , regValue);
#endif // defined(RTL8696)

#elif defined(RTL9601B)
    regValue = RTL_R32(IO_MODE_EN);
    regValue = 0x2aaa72;
    RTL_W32(IO_MODE_EN , regValue); 
#endif // defined(RTL8696) || defined(RTL9602C) 

#endif // defined(RTL9601B) || defined(RTL8696) || defined(RTL9602C)    

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
    QC_PRINTK("PCMCR = [%x]\n", regValue);  
    regValue = RTL_R32(0xbb023018);
    QC_PRINTK("0xbb023018 = [%x]\n", regValue);
    regValue = RTL_R32(0xB8000600);
    QC_PRINTK("0xB8000600 = [%x]\n", regValue);
}

static void init_isi (void)
{
    u32 regValue;
    
    // init SPI
    init_rtl_hw_spi_IP();
    //_init_rtl_spi_dev_type_hw(&spi_dev[0], 0);    
#if defined(RTL8685S)
    regValue = RTL_R32(FRACENREG);
    regValue &= ~BIT19;
    RTL_W32(FRACENREG, regValue);
    
    regValue = RTL_R32(FRACENREG);
    regValue |= BIT19;
    RTL_W32(FRACENREG, regValue);   

    regValue = RTL_R32(PLL_CLOCK_CONTROL);
    regValue |= FRACEN;
    RTL_W32(PLL_CLOCK_CONTROL, regValue);

    regValue = RTL_R32(IP_ENABLE);
    regValue |= ((BIT27)|(BIT26)|(PCMEN)|(VOIP_PERI));
    RTL_W32(IP_ENABLE, regValue);
#elif defined(RTL8685)
    regValue = RTL_R32(IP_ENABLE);
    regValue &= 0xf3ffffff;
    regValue |= ((1<<27)|(1<<26)|PCMEN|VOIP_PERI);
    RTL_W32(IP_ENABLE , regValue);  
#elif defined(RTL9602C)
#if 0
    regValue = RTL_R32(REG_SDS_CFG);
    regValue = FIB_1G;
    RTL_W32(REG_SDS_CFG, regValue);

    regValue = RTL_R32(REG_FRC);
    regValue &= ~(FRC_RX_EN_VAL|FRC_RX_EN_VAL);
    regValue |= (FRC_RX_EN_ON|FRC_CMU_EN_ON);
    RTL_W32(REG_FRC, regValue);

//  REG32(REG_FRC) &= ~(FRC_RX_EN_VAL|FRC_RX_EN_VAL);
//  REG32(REG_FRC) |= (FRC_RX_EN_ON|FRC_CMU_EN_ON);
    regValue = RTL_R32(REG_REG42);
    regValue |= (REG_PCM_CMU_EN);
    RTL_W32(REG_REG42, regValue);
    
    regValue = RTL_R32(REG_REG42);
    regValue &= ~(REG_PCM_CMU_EN);
    RTL_W32(REG_REG42, regValue);

    regValue = RTL_R32(REG_REG42);
    regValue |= (REG_PCM_CMU_EN);
    RTL_W32(REG_REG42, regValue);
//  REG32(REG_REG42) |= (REG_PCM_CMU_EN);
//  REG32(REG_REG42) &= ~(REG_PCM_CMU_EN);
//  REG32(REG_REG42) |= (REG_PCM_CMU_EN);
#endif  
#endif  
    
#if defined(RTL9601B) || defined(RTL8696) || defined(RTL9602C)  
    /*enable PCM module */
    regValue = RTL_R32(IP_ENABLE);
    regValue &= 0xf9ffffff; //0xc600ffff;
    regValue |= ((1<<26)|(1<<25)|PCMEN|VOIP_PERI);
    RTL_W32(IP_ENABLE , regValue);
    
#if defined(RTL8696) || defined(RTL9602C)       
    regValue = RTL_R32(IO_MODE_EN);
    regValue = regValue & 0xfff87fff;       // clear bit[18:15]
    regValue |= SLIC_ISI_EN;
    RTL_W32(IO_MODE_EN, regValue);
#if defined(RTL8696)
    regValue = RTL_R32(SLIC_INSEL_CTRL);
    regValue |= SLIC_EN;
    RTL_W32(IO_MODE_EN , regValue);
#endif // defined(RTL8696)

#elif defined(RTL9601B)
    regValue = RTL_R32(IO_MODE_EN);
    regValue = 0x155572;
    RTL_W32(IO_MODE_EN , regValue); 
#endif  // defined(RTL8696) || defined(RTL9602C)        

#endif // defined(RTL9601B) || defined(RTL8696) || defined(RTL9602C)    
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
    QC_PRINTK("PCMCR = [%x]\n", regValue);  
    regValue = RTL_R32(0xbb023018);
    QC_PRINTK("0xbb023018 = [%x]\n", regValue);
    regValue = RTL_R32(0xB8000600);
    QC_PRINTK("0xB8000600 = [%x]\n", regValue);
}


static int pcm_test_read_proc(char *page, char **start, off_t off,
                int count, int *eof, void *data)
{
    QC_PRINTK("enter read proc..\n");
    
    return 0;
}

static int pcm_test_write_proc(struct file *file, const char *buffer,
                unsigned long count, void *data)
{
    char tmp[128];
    int t1 = 0;
    if (count < 1)
        return -EFAULT;
    QC_PRINTK("enter write proc..\n");

    //_init_rtl_spi_dev_type_hw(&spi_dev[0], 0);    
    
    //re8670_pcm_init_hw();
    
    if (buffer && !copy_from_user(tmp, buffer, 128)) {
        sscanf(tmp, "%d", &t1);
        QC_PRINTK("test input : %d\n", t1); 
        RTL_W32(PCMCHCNR47, 0x00000000);
        if ( (t1 > 0) && (t1 < 10))  // spi test
        {
            RTL_W32(PCMCHCNR, 0x03000000);
            spi_test_read(t1);          
        }else if ( t1 >= 10 ) // pcm test
        {
            pcm_loopback(t1, 0);
        }
#ifdef THREAD_TEST      
        else if ( t1 == 0 )
            wake_up_process(pmclb_tsk);
#endif      

    }
    else
        return -EFAULT;
        
    
    return count;
    
}

/*****************************************************
*
* PRBS Test Related fuction
*
*****************************************************/
static int prbs = 0;

#define SWITCH_BASE 0xbb000000

#if defined(RTL9602C)
    #define PRBS_STATUS  0x220a0
    #define PRBS_ERR     0x2209c
    #define PRBS_EN      0x22098
    #define PRBS_SEL     0x22094
#elif defined(RTL9601B)
    #define PRBS_STATUS  0x22040
    #define PRBS_ERR     0x2203c
    #define PRBS_EN      0x22038
    #define PRBS_SEL     0x22034
#else
    #define PRBS_STATUS  0x22108
    #define PRBS_ERR     0x22104
    #define PRBS_EN      0x22100
    #define PRBS_SEL     0x220fc
#endif


static int prbs_test_read_proc(char *page, char **start, off_t off,
                int count, int *eof, void *data)
{
    unsigned int status,err ;
    err = SWITCH_BASE | PRBS_ERR;
    status = SWITCH_BASE | PRBS_STATUS;
    //printk_test("status %x, err %x\n", RTL_R32(status),RTL_R32(err));
    if(prbs < 0 || RTL_R32(err)!=0){
        printk_test("PRBS Test: FAILED\n");
    }else{
        printk_test("PRBS Test: PASS\n");
    }
    return count;
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
        case 32:        
        {
            /*for prbs test*/
            if((ret = prbs_set(0xff)) <0)
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
* Ethernet Test Related fuction
*
*****************************************************/

typedef struct qc_test_phy_data_s
{
    uint16  phy;
    uint16  page;
    uint16  reg;
    uint16  data;
} qc_test_phy_data_t;

#if defined(RTL9602C)
    static qc_test_phy_data_t  phyPatchArray[] = {
        {0x1, 0xa4a, 0x13, 0x0033},
        {0x1, 0xa58, 0x13, 0x40e4},
        {0x1, 0xa43, 0x13, 0x809d},
        {0x1, 0xa43, 0x14, 0x0a08},
        {0x1, 0xa43, 0x13, 0x80a6},
        {0x1, 0xa43, 0x14, 0x0a08},
        {0x1, 0xa43, 0x13, 0x80af},
        {0x1, 0xa43, 0x14, 0x0a08},
        {0x1, 0xa80, 0x17, 0x08c3},
        {0x1, 0xbcc, 0x10, 0xa40d},
        {0x1, 0xa43, 0x10, 0x118b},
        {0x1, 0xa41, 0x19, 0x0e00},
        {0x1, 0xbce, 0x14, 0x40a0},
        {0x1, 0xa43, 0x10, 0x158b},
        {0x1, 0xa43, 0x13, 0x8011},
        {0x1, 0xa43, 0x14, 0xe37f},
        {0x1, 0xa43, 0x10, 0x178b},
        {0x1, 0xa40, 0x10, 0x1240},
    };
#elif defined(RTL9601B)
    static qc_test_phy_data_t  phyPatchArray[] = {};
#else
    static qc_test_phy_data_t  phyPatchArray[] = {};
#endif


static int ether_test_read_proc(char *page, char **start, off_t off,
                int count, int *eof, void *data)
{
    return count;
}


static int ether_test_write_proc(struct file *file, const char *buffer,
                unsigned long count, void *data)
{
    int i;
    int ret;
    qc_test_phy_data_t *patchArray;
    uint32   patchSize;
    rtk_portmask_t portmask;
    rtk_portmask_t extPortmask;
    struct pkt_dbg_tx_info cpuTag;
    uint8 payload[150];
    char flag[64];
    int pkt_cnt=0;

    if (count < 2)
        return -EFAULT;

    if (buffer && !copy_from_user(&flag, buffer, sizeof(flag))) {
        
        pkt_cnt = simple_strtol(flag, NULL, 0);
        
        printk_test("Test Ethernet %d\n",pkt_cnt);

        /* isolation */
        memset(&portmask, 0x0, sizeof(rtk_portmask_t));
        memset(&extPortmask, 0x0, sizeof(rtk_portmask_t));
        portmask.bits[0]= 0x4;
        for(i=0; i<2; i++) 
        {
            if((ret=rtk_port_isolationEntry_set(RTK_PORT_ISO_CFG_0, i, &portmask, &extPortmask))!=RT_ERR_OK)
            {
                return -EFAULT;
            }
        }
        /* acl init */
        if((ret=rtk_acl_init())!=RT_ERR_OK)
        {
            return -EFAULT;
        }
        patchArray = phyPatchArray;
        patchSize = sizeof(phyPatchArray)/sizeof(qc_test_phy_data_t);
        /*phy lookpack*/
        for(i=0 ; i<patchSize ; i++)
        {
            if((ret=rtk_port_phyReg_set(patchArray[i].phy,
                                  patchArray[i].page,
                                  patchArray[i].reg,
                                  patchArray[i].data))!=RT_ERR_OK)
            {
                return -EFAULT;
            }
        }
        /* CPU send packet */
        memset(&cpuTag, 0x0, sizeof(cpuTag));
        cpuTag.opts1.bit.dislrn = 1;
        cpuTag.opts3.bit.tx_portmask = 0x3;
        memset(payload, 0x0, sizeof(payload));
        payload[0]=0x00;
        payload[1]=0x11;
        payload[2]=0x22;
        payload[3]=0x33;
        payload[4]=0x44;
        payload[5]=0x56;
        payload[6]=0x00;
        payload[7]=0x33;
        payload[8]=0x44;
        payload[9]=0x55;
        payload[10]=0x66;
        payload[11]=0x78;

        if(pkt_cnt > 0) 
        {
            mac_debug_txPkt_send(payload, 128, &cpuTag, pkt_cnt);
        }
        else if(pkt_cnt == 0) 
        {
            mac_debug_txPktCont_send(payload, 128, &cpuTag, 0);
        }
        else if(pkt_cnt < 0)
        {
            mac_debug_txPktCont_send(payload, 128, &cpuTag, 1);
        }
        else
        {}
    }
    return count;
}


/*****************************************************
*
* GPIO Test Related fuction
*
*****************************************************/


#if defined(RTL9602C)
    //uint32 gpio_array[2][16] = {{12,13,15,36,31,27,26,40,3,7,1,33,37,28,39,21},{11,14,0,35,30,43,41,2,6,10,34,38,29,32,19,11}}; /*remove console bit pin17 for test*/
    uint32 gpio_array[12][16] = {{12,13,15,36,31,27,26,40,3,7,1,33,37,28,39,21},{11,14,0,35,30,43,41,2,6,10,34,38,29,32,17,19}};
#elif defined(RTL9601B)
    uint32 gpio_array[][] = {};
#else
    uint32 gpio_array[][] = {};
#endif


static int gpio_test_read_proc(char *page, char **start, off_t off,
                int count, int *eof, void *data)
{
    return count;
}


static int gpio_test_write_proc(struct file *file, const char *buffer,
                unsigned long count, void *data)
{
    int i, j;
    int ret;
    uint32 *patchArray;
    uint32  patchSize;
    char flag[64];
    int sel=0;

    if (count < 2)
        return -EFAULT;

    if (buffer && !copy_from_user(&flag, buffer, sizeof(flag))) {
        
        sel = simple_strtol(flag, NULL, 0);
        
        printk_test("Test GPIO %d\n",sel);

        if(sel == 0 || sel == 1) 
        {
            for(i=0; i<2; i++) 
            {
                patchArray = gpio_array[i];
                patchSize = sizeof(gpio_array[i])/sizeof(uint32);

                printk_test("Test GPIO array[%d]=%d, no=%d\n",i,i^sel, patchSize);

                if((ret=rtk_gpio_init())!=RT_ERR_OK)
                {
                    return -EFAULT;
                }
                for(j=0; j<patchSize; j++) 
                {
                    if((ret=rtk_gpio_state_set(gpio_array[i][j], ENABLED))!=RT_ERR_OK)
                    {
                        return -EFAULT;
                    }
                    if((ret=rtk_gpio_mode_set(gpio_array[i][j], GPIO_OUTPUT))!=RT_ERR_OK)
                    {
                        return -EFAULT;
                    }
                    if((ret=rtk_gpio_databit_set(gpio_array[i][j],i^sel))!=RT_ERR_OK)
                    {
                        return -EFAULT;
                    }
                }
            }
        }
        else
        {}
    }

    return count;
}

/*****************************************************
*
* USB Test Related fuction
*
*****************************************************/




static int usb_test_read_proc(char *page, char **start, off_t off,
                int count, int *eof, void *data)
{
    return count;
}


static int usb_test_write_proc(struct file *file, const char *buffer,
                unsigned long count, void *data)
{
    uint32 val, addr;
    char flag[64];
    int sel=0;
	uint32 cnt=0;


    if (count < 2)
        return -EFAULT;

    if (buffer && !copy_from_user(&flag, buffer, sizeof(flag))) {
        
        sel = simple_strtol(flag, NULL, 0);
        
        printk_test("Test USB %d\n",sel);

        if(sel == 1) 
        {
#if defined(RTL9602C)
			addr = 0xb8021010;
			val = RTL_R32(addr) & ~(0x1<<24);
			RTL_W32(addr,val);
			printk_test("Test USB addr=0x%x, val=0x%x, reg=0x%x\n",addr, val, RTL_R32(addr));
			addr = 0xb8021014;
			while (((RTL_R32(addr) & (0x1<<20)) == 0) && (cnt < 100))
			{
				cnt++;
				msleep(100);
			}
			if(cnt >= 100) 
			{
				printk_test("Test USB fail, 0x%x\n",RTL_R32(addr));
				return -EFAULT;
			}

			addr = 0xb8021054;
			val = (RTL_R32(addr) & ~(0xf<<8)) | (0x4<<8);
			RTL_W32(addr,val);
#elif defined(RTL9601B)
#else
#endif
        }
		else if(sel == 0) 
		{
		}
        else
        {}
    }

    return count;
}


static int __init qc_test_module_init(void)
{
    struct proc_dir_entry *dir = NULL;
    struct proc_dir_entry *pcm=NULL;
    struct proc_dir_entry *prbs=NULL;
    struct proc_dir_entry *ether=NULL;
    struct proc_dir_entry *gpio=NULL;
	struct proc_dir_entry *usb=NULL;
    
    dir = proc_mkdir("qc_test",NULL);

    pcm = create_proc_entry("pcm", 0, dir);

    if (pcm) {
        pcm->read_proc = pcm_test_read_proc;
        pcm->write_proc = pcm_test_write_proc;
    }
    else {
        QC_PRINTK("pcm_test, create proc failed!\n");
    }

    prbs = create_proc_entry("prbs", 0, dir);

    if (prbs) {
        prbs->read_proc = prbs_test_read_proc;
        prbs->write_proc = prbs_test_write_proc;
    }
    else {
        QC_PRINTK("prbs_test, create proc failed!\n");
    }

    ether = create_proc_entry("ether", 0, dir);

    if (ether) {
        ether->read_proc = ether_test_read_proc;
        ether->write_proc = ether_test_write_proc;
    }
    else {
        QC_PRINTK("ether_test, create proc failed!\n");
    }

    gpio = create_proc_entry("gpio", 0, dir);

    if (gpio) {
        gpio->read_proc = gpio_test_read_proc;
        gpio->write_proc = gpio_test_write_proc;
    }
    else {
        QC_PRINTK("gpio_test, create proc failed!\n");
    }

	usb = create_proc_entry("usb", 0, dir);

    if (usb) {
        usb->read_proc = usb_test_read_proc;
        usb->write_proc = usb_test_write_proc;
    }
    else {
        QC_PRINTK("gpio_test, create proc failed!\n");
    }

    return 0;
}

static void __exit qc_test_module_exit(void)
{
    QC_PRINTK("\n\n qc_test_module_exit \n\n");
    //return 0;
}


module_init(qc_test_module_init);
module_exit(qc_test_module_exit);



#ifdef THREAD_TEST
static int kpcmlbtest(void *arg)
{
    unsigned int timeout;

    for(;;) {
        if (kthread_should_stop()) break;
        pcm_loopback(11, 0);
        do {
            set_current_state(TASK_INTERRUPTIBLE);
            timeout = schedule_timeout(100 * HZ);

        } while(timeout);
    }
    QC_PRINTK("break\n");

    return 0;
}

static int __init init_modules(void)
{
    int ret;

    pmclb_tsk = kthread_create(kpcmlbtest, &data, "kpcmlbtest");
    if (IS_ERR(pmclb_tsk)) {
        ret = PTR_ERR(pmclb_tsk);
        pmclb_tsk = NULL;
        goto out;
    }


    return 0;

out:
    return ret;
}

static void __exit exit_modules(void)
{
    kthread_stop(pmclb_tsk);
}

module_init(init_modules);
module_exit(exit_modules);
#endif
