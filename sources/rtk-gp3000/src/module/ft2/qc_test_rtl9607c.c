#include <linux/init.h>
#include <linux/module.h>
#include <linux/kthread.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/version.h>

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
#include <rtk/ponmac.h>
#include <rtk/led.h>
#include <common/error.h>
#include <ioal/mac_debug.h>
#include <ioal/mem32.h>
#include <rtk/rtusr/rtusr_pkt.h>

#if defined(CONFIG_RTL9607C)
#include <dal/rtl9607c/dal_rtl9607c.h>
#include <dal/rtl9607c/dal_rtl9607c_switch.h>
#endif

#if defined(CONFIG_RTL9602C_SERIES)
#define RTL9602C 1
#elif defined(CONFIG_SDK_RTL9601B)
#define RTL9601B 1
#elif defined(CONFIG_RTL8685S)
#define RTL8685S 1
#elif defined(CONFIG_RTL8685)
#define RTL8685 1
#elif defined(CONFIG_RTL9607C)
#define RTL9607C
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

void qc_write_data(char *filename, char *data, uint32 len);


static int log = 1;
static int debug_log = 0;
#define printk_test(fmt,args...)  if(log) { printk(fmt, ##args); } 

static uint32 olt_mode = 0;

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
#define SPI2_PRINTK             printk_off
#define QC_PRINTK(fmt,args...)  if(debug_log) { printk(fmt, ##args); } 
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
#ifdef RTL9607C
#define IO_MODE_EN  0xbb023014
#endif
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


#if defined(RTL9601B) || defined(RTL9602C) || defined(RTL8685) || defined(RTL8685S) || defined(RTL9607C)

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
	
	if((type == 12) || (type == 14)) //for isi1 & zsi1
		RTL_W32(PCMCHCNR , 0x10000000);
    
    /*PCM General Control Register: linear mode,enable PCM*/
    //RTL_W32(PCMCR,( ZSILBE | LINEAR_MODE | PCM_AE | C0ILBE));

	if((type == 13) || (type == 12)){
        init_isi();
        RTL_W32(PCMCR,( ISILBE | LINEAR_MODE | PCM_AE ));       
        //QC_PRINTK("type = [%d] : PCMCR,( ISILBE | LINEAR_MODE | PCM_AE )\n", type);
	}else if ( (type == 15) || (type == 14)){
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
	if((type == 12) || (type == 14)){ //for isi1 & zsi1
		pcm_tx_rx_enable |= (1 << 28);
	}
	
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
	
	if((type == 12) || (type == 14)) //for isi1 & zsi1
		RTL_W32(PCMCHCNR , 0x10000000);

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
#elif defined(RTL9607C)
    if ( (type == 13) || (type == 12) || (type == 11)){ // ISI & PCM
        ret = eqc_comparedata(0, txbuf, rxbuf);
    }else if ( (type == 15) || (type == 14) ){    // ZSI
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
#ifdef RTL9607C
    regValue = RTL_R32(IP_ENABLE );
    regValue &= 0xf9ffffff; // clear bit [26:25]
	RTL_W32(IP_ENABLE  , regValue);
	
	regValue = RTL_R32(IO_MODE_EN);
	regValue = regValue & 0xfff87fff;// clear bit [18:15]
	regValue |= (1<<17);
	RTL_W32(IO_MODE_EN , regValue);
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

#ifdef RTL9607C
    regValue = RTL_R32(IP_ENABLE );
    regValue &= 0xf9ffffff; // clear bit [26:25]
	regValue |= (1 << 26);
	RTL_W32(IP_ENABLE  , regValue);
	
	regValue = RTL_R32(IO_MODE_EN);
	regValue = regValue & 0xfff87fff;// clear bit [18:15]
	regValue |= (1<<16);
	RTL_W32(IO_MODE_EN , regValue);
#endif

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
    
#if defined(RTL9601B) || defined(RTL8696) || defined(RTL9602C) || defined(RTL9607C)
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

#ifdef RTL9607C
    regValue = RTL_R32(IP_ENABLE );
    regValue &= 0xf9ffffff; // clear bit [26:25]
	regValue |= (1 << 26);
	regValue |= (1 << 25);
	RTL_W32(IP_ENABLE  , regValue);
	
	regValue = RTL_R32(IO_MODE_EN);
	regValue = regValue & 0xfff87fff;// clear bit [18:15]
	regValue |= (1<<15);
	RTL_W32(IO_MODE_EN , regValue);
#endif
  
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


static int pcm_test_read_proc(struct seq_file *seq, void *v)
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


static uint32 prbsResultGpon = 0;
static uint32 prbsResultEpon = 0;
static uint32 prbsResultFiber = 0;
static uint32 prbsResultHsg0 = 0;
static uint32 prbsResultHsg1 = 0;
static uint32 prbsSignalDetect = 0;

static int prbs_test_read_proc(struct seq_file *seq, void *v)
{
    seq_printf(seq, "GPON PRBS test %s\n", prbsResultGpon == 1 ? "passed" : "failed");
    seq_printf(seq, "EPON PRBS test %s\n", prbsResultEpon == 1 ? "passed" : "failed");
    seq_printf(seq, "Fiber PRBS test %s\n", prbsResultFiber == 1 ? "passed" : "failed");
    seq_printf(seq, "HSG0 PRBS test %s\n", prbsResultHsg0 == 1 ? "passed" : "failed");
    seq_printf(seq, "HSG1 PRBS test %s\n", prbsResultHsg1 == 1 ? "passed" : "failed");
    return 0;
}

static int prbs_gpon(unsigned int value)
{
    int ret;
    uint32 pattern, data, status, err;

    switch(value)
    {
    case 3:
        pattern = 0x66;
        break;
    case 7:
        pattern = 0x77;
        break;
    case 15:
        pattern = 0x88;
        break;
    case 23:
        pattern = 0x99;
        break;
    case 31:
        pattern = 0xaa;
        break;
    default:
        return -1;
    }

    if((ret = rtk_gpio_state_set(prbsSignalDetect, ENABLED))!=RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }
    if((ret = rtk_gpio_mode_set(prbsSignalDetect, GPIO_OUTPUT))!=RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }
    if((ret = rtk_gpio_databit_set(prbsSignalDetect, 1))!=RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }

    /* Configure GPON mode */
    if((ret = rtk_ponmac_mode_set(PONMAC_MODE_GPON))!=RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }
    /* Stop Tx/Rx clock reference for PRBS testing */
    data = 0x2;
    if((ret = reg_field_write(RTL9607C_SDS_ANA_GPON_REG38r, RTL9607C_REG_TX_CLK_SELf, &data)) != RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }

    data = 0x18000;
    if((ret = reg_write(RTL9607C_WSDS_DIG_1Dr, &data)) != RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }
    data = 0x1c000;
    if((ret = reg_write(RTL9607C_WSDS_DIG_1Dr, &data)) != RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }
    mdelay(200);

    data = 0x4000 | pattern;
    if((ret = reg_write(RTL9607C_WSDS_DIG_19r, &data)) != RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }
    data = 0x1;
    if((ret = reg_write(RTL9607C_WSDS_DIG_1Ar, &data)) != RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }
    data = 0xC000 | pattern;
    if((ret = reg_write(RTL9607C_WSDS_DIG_19r, &data)) != RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }
    data = 0x4000 | pattern;
    if((ret = reg_write(RTL9607C_WSDS_DIG_19r, &data)) != RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }
    mdelay(1000);
    if((ret = reg_read(RTL9607C_WSDS_DIG_1Br, &err)) != RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }
    if((ret = reg_read(RTL9607C_WSDS_DIG_1Cr, &status)) != RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }

    if(olt_mode)
    {
        /* Endless PRBS testing here... */
        while(1)
        {
            schedule_timeout(1000 * HZ);
        }
    }

    if(0 == err)
    {
        printk_test("GPON PRBS test passed\n");
        prbsResultGpon = 1;
    }
    else
    {
        printk_test("GPON PRBS failed, status 0x%x, err 0x%x\n", status, err);
        prbsResultGpon = 0;
    }

    data = 0;
    if((ret = reg_write(RTL9607C_WSDS_DIG_1Ar, &data)) != RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }
    if((ret = rtk_gpio_state_set(prbsSignalDetect, DISABLED)) != RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }
    data = 0;
    if((ret = reg_write(RTL9607C_SDS_ANA_MISC_REG01r, &data)) != RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }

    return 0;
}

static int prbs_epon(unsigned int value)
{
    int ret;
    uint32 pattern, data, err[2];

    switch(value)
    {
    case 7:
        pattern = 0x0;
        break;
    case 9:
        pattern = 0x1;
        break;
    case 11:
        pattern = 0x2;
        break;
    case 15:
        pattern = 0x3;
        break;
    default:
        return -1;
    }

    if((ret = rtk_gpio_state_set(prbsSignalDetect, ENABLED))!=RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }
    if((ret = rtk_gpio_mode_set(prbsSignalDetect, GPIO_OUTPUT))!=RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }
    if((ret = rtk_gpio_databit_set(prbsSignalDetect, 1))!=RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }

    /* Configure EPON mode */
    data = 0x1f;
    if((ret = reg_write(RTL9607C_SDS_CFGr, &data)) != RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }
    data = 0xf30;
    if((ret = reg_write(RTL9607C_WSDS_DIG_00r, &data)) != RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }
    if((ret = rtk_ponmac_mode_set(PONMAC_MODE_EPON))!=RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }
    /* Stop Tx/Rx clock reference for PRBS testing */
    data = 0x2;
    if((ret = reg_field_write(RTL9607C_SDS_ANA_EPON_REG38r, RTL9607C_REG_TX_CLK_SELf, &data)) != RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }

    data = 0xc;
    if((ret = reg_write(RTL9607C_SDS_CFGr, &data)) != RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }
    data = 0x71e0;
    if((ret = reg_write(RTL9607C_SDS_REG2r, &data)) != RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }
    data = 0x1140;
    if((ret = reg_write(RTL9607C_FIB_REG0r, &data)) != RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }
    data = 0x1401;
    if((ret = reg_write(RTL9607C_SDS_REG0r, &data)) != RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }
    data = 0x1403;
    if((ret = reg_write(RTL9607C_SDS_REG0r, &data)) != RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }
    mdelay(200);

    data = pattern << 8;
    if((ret = reg_write(RTL9607C_SDS_EXT_REG11r, &data)) != RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }
    data = 0x804;
    if((ret = reg_write(RTL9607C_SDS_EXT_REG12r, &data)) != RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }
    data = 0;
    if((ret = reg_write(RTL9607C_SDS_EXT_REG4r, &data)) != RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }
    data = 0;
    if((ret = reg_write(RTL9607C_SDS_EXT_REG5r, &data)) != RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }
    mdelay(1000);

    if((ret = reg_read(RTL9607C_SDS_EXT_REG4r, &err[0])) != RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }
    if((ret = reg_read(RTL9607C_SDS_EXT_REG4r, &err[1])) != RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }

    if(olt_mode)
    {
        /* Endless PRBS testing here... */
        while(1)
        {
            schedule_timeout(1000 * HZ);
        }
    }

    if((0 == err[0]) && (0 == err[1]))
    {
        printk_test("EPON PRBS test passed\n");
        prbsResultEpon = 1;
    }
    else
    {
        printk_test("EPON PRBS failed, err 0x%x\n", err[1] << 16 | err[0]);
        prbsResultEpon = 0;
    }

    data = 0x4;
    if((ret = reg_write(RTL9607C_SDS_EXT_REG12r, &data)) != RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }
    mdelay(200);

    if((ret = rtk_gpio_state_set(prbsSignalDetect, DISABLED)) != RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }

    return 0;
}

static int prbs_fiber(unsigned int value)
{
    int ret;
    uint32 pattern, data, err[2];

    switch(value)
    {
    case 7:
        pattern = 0x0;
        break;
    case 9:
        pattern = 0x1;
        break;
    case 11:
        pattern = 0x2;
        break;
    case 15:
        pattern = 0x3;
        break;
    default:
        return -1;
    }
    if((ret = rtk_gpio_state_set(prbsSignalDetect, ENABLED))!=RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }
    if((ret = rtk_gpio_mode_set(prbsSignalDetect, GPIO_OUTPUT))!=RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }
    if((ret = rtk_gpio_databit_set(prbsSignalDetect, 1))!=RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }
    if((ret = rtk_gpio_databit_set(prbsSignalDetect, 0))!=RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }

    /* Configure fiber mode */
    if((ret = rtk_ponmac_mode_set(PONMAC_MODE_FIBER_FORCE_1G))!=RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }
    data = 0x1140;
    if((ret = reg_write(RTL9607C_FIB_REG0r, &data)) != RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }
    mdelay(500);

    if((ret = reg_read(RTL9607C_SDS_EXT_REG29r, &data)) != RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }
    if((ret = reg_read(RTL9607C_SDS_EXT_REG29r, &data)) != RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }
    if(data != 0x111)
    {
        printk_test("FIB1G Link FAIL!\n");
        return 0;
    }

    data = pattern << 8;
    if((ret = reg_write(RTL9607C_SDS_EXT_REG11r, &data)) != RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }
    data = 0x804;
    if((ret = reg_write(RTL9607C_SDS_EXT_REG12r, &data)) != RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }
    data = 0;
    if((ret = reg_write(RTL9607C_SDS_EXT_REG4r, &data)) != RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }
    data = 0;
    if((ret = reg_write(RTL9607C_SDS_EXT_REG5r, &data)) != RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }
    mdelay(1000);

    if((ret = reg_read(RTL9607C_SDS_EXT_REG4r, &err[0])) != RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }
    if((ret = reg_read(RTL9607C_SDS_EXT_REG4r, &err[1])) != RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }

    if(olt_mode)
    {
        /* Endless PRBS testing here... */
        while(1)
        {
            schedule_timeout(1000 * HZ);
        }
    }

    if((0 == err[0]) && (0 == err[1]))
    {
        printk_test("fiber PRBS test passed\n");
        prbsResultFiber = 1;
    }
    else
    {
        printk_test("fiber PRBS failed, err 0x%x\n", err[1] << 16 | err[0]);
        prbsResultFiber = 0;
    }

    data = 0x4;
    if((ret = reg_write(RTL9607C_SDS_EXT_REG12r, &data)) != RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }
    if((ret = rtk_gpio_state_set(prbsSignalDetect, DISABLED))!= RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }
    mdelay(200);

    return 0;
}

static int prbs_hsg0(unsigned int value)
{
    int ret;
    uint32 pattern, data, err[2];

    switch(value)
    {
    case 7:
        pattern = 0x0;
        break;
    case 9:
        pattern = 0x1;
        break;
    case 11:
        pattern = 0x2;
        break;
    case 15:
        pattern = 0x3;
        break;
    default:
        return -1;
    }

    /* Configure HSG0 mode */
    if((ret = rtk_port_serdesMode_set(0, LAN_SDS_MODE_HSGMII_MAC)) != RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }
    mdelay(500);

    if(olt_mode)
    {
        /* For OLT testing, HSG0 is connected to HSG1 */
        /* Configure HSG1 mode */
        if((ret = rtk_port_serdesMode_set(1, LAN_SDS_MODE_HSGMII_MAC)) != RT_ERR_OK)
        {
            QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
            return -1;
        }
        mdelay(500);
    }

    if((ret = reg_read(RTL9607C_HSG0_SDS_EXT_REG30r, &data)) != RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }
    if((ret = reg_read(RTL9607C_HSG0_SDS_EXT_REG30r, &data)) != RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }
    if(data != 0x111)
    {
        printk_test("HSG0 Link FAIL!\n");
        return 0;
    }

    data = pattern << 8;
    if((ret = reg_write(RTL9607C_HSG0_SDS_EXT_REG11r, &data)) != RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }
    data = 0x804;
    if((ret = reg_write(RTL9607C_HSG0_SDS_EXT_REG12r, &data)) != RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }
    data = 0;
    if((ret = reg_write(RTL9607C_HSG0_SDS_EXT_REG4r, &data)) != RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }
    data = 0;
    if((ret = reg_write(RTL9607C_HSG0_SDS_EXT_REG5r, &data)) != RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }
    mdelay(1000);

    if(olt_mode)
    {
        data = pattern << 8;
        if((ret = reg_write(RTL9607C_HSG1_SDS_EXT_REG11r, &data)) != RT_ERR_OK)
        {
            QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
            return -1;
        }
        data = 0x804;
        if((ret = reg_write(RTL9607C_HSG1_SDS_EXT_REG12r, &data)) != RT_ERR_OK)
        {
            QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
            return -1;
        }
        data = 0;
        if((ret = reg_write(RTL9607C_HSG1_SDS_EXT_REG4r, &data)) != RT_ERR_OK)
        {
            QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
            return -1;
        }
        data = 0;
        if((ret = reg_write(RTL9607C_HSG1_SDS_EXT_REG5r, &data)) != RT_ERR_OK)
        {
            QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
            return -1;
        }
        /* Clear HSG0 counters again */
        data = 0;
        if((ret = reg_write(RTL9607C_HSG0_SDS_EXT_REG4r, &data)) != RT_ERR_OK)
        {
            QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
            return -1;
        }
        data = 0;
        if((ret = reg_write(RTL9607C_HSG0_SDS_EXT_REG5r, &data)) != RT_ERR_OK)
        {
            QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
            return -1;
        }
    }

    if((ret = reg_read(RTL9607C_HSG0_SDS_EXT_REG4r, &err[0])) != RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }
    if((ret = reg_read(RTL9607C_HSG0_SDS_EXT_REG4r, &err[1])) != RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }

    if(olt_mode)
    {
        /* Endless PRBS testing here... */
        while(1)
        {
            schedule_timeout(1000 * HZ);
        }
    }

    if((0 == err[0]) && (0 == err[1]))
    {
        printk_test("HSG0 PRBS test passed\n");
        prbsResultHsg0 = 1;
    }
    else
    {
        printk_test("HSG0 PRBS failed, err 0x%x\n", err[1] << 16 | err[0]);
        prbsResultHsg0 = 0;
    }

    data = 0x4;
    if((ret = reg_write(RTL9607C_HSG0_SDS_EXT_REG12r, &data)) != RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }
    mdelay(200);

    return 0;
}
static int prbs_hsg1(unsigned int value)
{
    int ret;
    uint32 pattern, data, err[2];

    switch(value)
    {
    case 7:
        pattern = 0x0;
        break;
    case 9:
        pattern = 0x1;
        break;
    case 11:
        pattern = 0x2;
        break;
    case 15:
        pattern = 0x3;
        break;
    default:
        return -1;
    }

    /* Configure HSG mode */
    if((ret = rtk_port_serdesMode_set(1, LAN_SDS_MODE_HSGMII_MAC)) != RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }
    mdelay(500);

    if((ret = reg_read(RTL9607C_HSG1_SDS_EXT_REG30r, &data)) != RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }
    if((ret = reg_read(RTL9607C_HSG1_SDS_EXT_REG30r, &data)) != RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }
    if(data != 0x111)
    {
        printk_test("HSG1 Link FAIL!\n");
        return 0;
    }

    data = pattern << 8;
    if((ret = reg_write(RTL9607C_HSG1_SDS_EXT_REG11r, &data)) != RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }
    data = 0x804;
    if((ret = reg_write(RTL9607C_HSG1_SDS_EXT_REG12r, &data)) != RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }
    data = 0;
    if((ret = reg_write(RTL9607C_HSG1_SDS_EXT_REG4r, &data)) != RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }
    data = 0;
    if((ret = reg_write(RTL9607C_HSG1_SDS_EXT_REG5r, &data)) != RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }
    mdelay(1000);

    if((ret = reg_read(RTL9607C_HSG1_SDS_EXT_REG4r, &err[0])) != RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }
    if((ret = reg_read(RTL9607C_HSG1_SDS_EXT_REG4r, &err[1])) != RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }

    if(olt_mode)
    {
        /* Endless PRBS testing here... */
        while(1)
        {
            schedule_timeout(1000 * HZ);
        }
    }

    if((0 == err[0]) && (0 == err[1]))
    {
        printk_test("HSG1 PRBS test passed\n");
        prbsResultHsg1 = 1;
    }
    else
    {
        printk_test("HSG1 PRBS failed, err 0x%x\n", err[1] << 16 | err[0]);
        prbsResultHsg1 = 0;
    }

    data = 0x4;
    if((ret = reg_write(RTL9607C_HSG1_SDS_EXT_REG12r, &data)) != RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }
    mdelay(200);

    return 0;
}

static int prbs_test_write_proc(struct file *file, const char *buffer, size_t count, loff_t *data)
{
    char flag[64];
    int ret, gponPrbs, eponPrbs, fiberPrbs, hsg0Prbs, hsg1Prbs;
    uint32  chipId,rev,subType  = 0;

    if (buffer && !copy_from_user(&flag, buffer, sizeof(flag)))
    {
        sscanf(flag, "%d %d %d %d %d", &gponPrbs, &eponPrbs, &fiberPrbs, &hsg0Prbs, &hsg1Prbs);

        rtk_switch_version_get(&chipId,&rev,&subType);
        if(olt_mode == 1)
        {
            prbsSignalDetect = 38;
        }
        else
        {
            switch(subType)
            {
            case RTL9607C_CHIP_SUB_TYPE_RTL9607CP:
            case RTL9607C_CHIP_SUB_TYPE_RTL9607EP:
            case RTL9607C_CHIP_SUB_TYPE_RTL9603CP:
                prbsSignalDetect = 31;
                break;
            case RTL9607C_CHIP_SUB_TYPE_RTL9603CT:
            case RTL9607C_CHIP_SUB_TYPE_RTL9607E_VA5:
            case RTL9607C_CHIP_SUB_TYPE_RTL9607C_VA5:
            case RTL9607C_CHIP_SUB_TYPE_RTL9607C_VA6:
            case RTL9607C_CHIP_SUB_TYPE_RTL9607C_VA7:
            case RTL9607C_CHIP_SUB_TYPE_RTL8198D_VE5:
            case RTL9607C_CHIP_SUB_TYPE_RTL8198D_VE6:
            case RTL9607C_CHIP_SUB_TYPE_RTL8198D_VE7:
            case RTL9607C_CHIP_SUB_TYPE_RTL9606C_VA5:
            case RTL9607C_CHIP_SUB_TYPE_RTL9606C_VA6:
            case RTL9607C_CHIP_SUB_TYPE_RTL9606C_VA7:
                prbsSignalDetect = 57;
                break;
            case RTL9607C_CHIP_SUB_TYPE_RTL9603C_VA4:
            case RTL9607C_CHIP_SUB_TYPE_RTL9603C_VA5:
            case RTL9607C_CHIP_SUB_TYPE_RTL9603C_VA6:
            case RTL9607C_CHIP_SUB_TYPE_RTL9603CW_VA6:
            case RTL9607C_CHIP_SUB_TYPE_RTL9603CW_VA7:
            case RTL9607C_CHIP_SUB_TYPE_RTL9603CE:
                prbsSignalDetect = 57;
                break;
            default:
                printk_test("unknown chip mode 0x%x\n", subType);
                goto prbs_err;
                break;
            }
        }

        switch(gponPrbs)
        {
        case 0:
            printk_test("GPON PRBS skipped!\n");
            break;
        case 3:
        case 7:
        case 15:
        case 23:
        case 31:
            if((ret = prbs_gpon(gponPrbs)) <0)
            {
                goto prbs_err;
            }
            break;
        default:
            goto prbs_err;
            break;
        }

        switch(eponPrbs)
        {
        case 0:
            printk_test("EPON PRBS skipped!\n");
            break;
        case 7:
        case 9:
        case 11:
        case 15:
            if((ret = prbs_epon(eponPrbs)) <0)
            {
                goto prbs_err;
            }
            break;
        default:
            goto prbs_err;
            break;
        }

        switch(fiberPrbs)
        {
        case 0:
            printk_test("Fiber PRBS skipped!\n");
            break;
        case 7:
        case 9:
        case 11:
        case 15:
            if((ret = prbs_fiber(fiberPrbs)) <0)
            {
                goto prbs_err;
            }
            break;
        default:
            goto prbs_err;
            break;
        }

        switch(hsg0Prbs)
        {
        case 0:
            printk_test("HSG0 PRBS skipped!\n");
            break;
        case 7:
        case 9:
        case 11:
        case 15:
            if((ret = prbs_hsg0(hsg0Prbs)) <0)
            {
                goto prbs_err;
            }
            break;
        default:
            goto prbs_err;
            break;
        }

        switch(hsg1Prbs)
        {
        case 0:
            printk_test("HSG1 PRBS skipped!\n");
            break;
        case 7:
        case 9:
        case 11:
        case 15:
            if((ret = prbs_hsg1(hsg1Prbs)) <0)
            {
                goto prbs_err;
            }
            break;
        default:
            goto prbs_err;
            break;
        }
    }
    else
    {
prbs_err:
        printk_test("prbs set failed!\n");
    }

    return count;
}

static int prbs_test_open_proc(struct inode *inode, struct file *file)
{
	return single_open(file, prbs_test_read_proc, NULL);
}
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
struct proc_ops prbs_fop = {
	.proc_open = prbs_test_open_proc,
	.proc_write = prbs_test_write_proc,
	.proc_read = seq_read,
	.proc_lseek = seq_lseek,
	.proc_release = single_release,
};
#else
struct file_operations prbs_fop = {
	.open = prbs_test_open_proc,
	.write = prbs_test_write_proc,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};
#endif


#if 0
/*****************************************************
*
* Ethernet Test Related fuction
*
*****************************************************/

static int ether_test_write_proc(struct file *file, const char *buffer, size_t count, loff_t *data)
{
    int ret;
    char flag[64];
    uint32 value[3];
    rtk_port_macAbility_t ability;
    int passed[2][4] = { {1, 1, 1, 1}, {1, 1, 1, 1}};

    if (buffer && !copy_from_user(&flag, buffer, sizeof(flag)))
    {
        /* Link down for port 9 & port 10 to prevent ingress packet from CPU */
        if((ret = rtk_port_macForceAbility_get(9, &ability)) != RT_ERR_OK)
        {
            return count;
        }
        ability.linkStatus = PORT_LINKDOWN;
        if((ret = rtk_port_macForceAbility_set(9, ability)) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_macForceAbility_get(10, &ability)) != RT_ERR_OK)
        {
            return count;
        }
        ability.linkStatus = PORT_LINKDOWN;
        if((ret = rtk_port_macForceAbility_set(10, ability)) != RT_ERR_OK)
        {
            return count;
        }

        /* Port 3 & 4 MDI loopback */
        if((ret = rtk_port_phyReg_set(3, 0xa43, 24, 0xd9c)) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_set(4, 0xa43, 24, 0xd9c)) != RT_ERR_OK)
        {
            return count;
        }
        /* Reset counter */
        if((ret = rtk_port_phyReg_set(1, 0xc80, 17, 0x0070)) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_set(2, 0xc80, 17, 0x0070)) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_set(3, 0xc80, 17, 0x0070)) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_set(4, 0xc80, 17, 0x0070)) != RT_ERR_OK)
        {
            return count;
        }
        /* Reset Tx/Rx */
        if((ret = rtk_port_phyReg_set(1, 0xc80, 17, 0x3)) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_set(2, 0xc80, 17, 0x3)) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_set(3, 0xc80, 17, 0x3)) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_set(4, 0xc80, 17, 0x3)) != RT_ERR_OK)
        {
            return count;
        }
        /* Set packet length */
        if((ret = rtk_port_phyReg_set(1, 0xc80, 19, 0x5ee)) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_set(2, 0xc80, 19, 0x5ee)) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_set(3, 0xc80, 19, 0x5ee)) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_set(4, 0xc80, 19, 0x5ee)) != RT_ERR_OK)
        {
            return count;
        }
        /* Set packet count */
        if((ret = rtk_port_phyReg_set(1, 0xc80, 20, 0x64)) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_set(1, 0xc80, 21, 0x0)) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_set(2, 0xc80, 20, 0x65)) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_set(2, 0xc80, 21, 0x0)) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_set(3, 0xc80, 20, 0x66)) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_set(3, 0xc80, 21, 0x0)) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_set(4, 0xc80, 20, 0x67)) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_set(4, 0xc80, 21, 0x0)) != RT_ERR_OK)
        {
            return count;
        }
        /* Select Rx/Tx & enable pkt gen */
        if((ret = rtk_port_phyReg_set(1, 0xc80, 16, 0x5a22)) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_set(2, 0xc80, 16, 0x5a22)) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_set(3, 0xc80, 16, 0x5a22)) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_set(4, 0xc80, 16, 0x5a22)) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_set(1, 0xc80, 16, 0x5a23)) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_set(2, 0xc80, 16, 0x5a23)) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_set(3, 0xc80, 16, 0x5a23)) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_set(4, 0xc80, 16, 0x5a23)) != RT_ERR_OK)
        {
            return count;
        }
        /* Wait for Tx/Rx complete */
        mdelay(1000);
        /* Disable pkt gen */
        if((ret = rtk_port_phyReg_set(1, 0xc80, 16, 0x5a00)) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_set(2, 0xc80, 16, 0x5a00)) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_set(3, 0xc80, 16, 0x5a00)) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_set(4, 0xc80, 16, 0x5a00)) != RT_ERR_OK)
        {
            return count;
        }
        /* Check Tx counters */
        if((ret = rtk_port_phyReg_get(1, 0xc80, 23, &value[0])) != RT_ERR_OK)
        {
            return count;
        }
        if(value[0] != 64)
        {
            passed[0][0] = 0;
        }
        if((ret = rtk_port_phyReg_get(2, 0xc80, 23, &value[0])) != RT_ERR_OK)
        {
            return count;
        }
        if(value[0] != 65)
        {
            passed[0][1] = 0;
        }
        if((ret = rtk_port_phyReg_get(3, 0xc80, 23, &value[0])) != RT_ERR_OK)
        {
            return count;
        }
        if(value[0] != 66)
        {
            passed[0][2] = 0;
        }
        if((ret = rtk_port_phyReg_get(4, 0xc80, 23, &value[0])) != RT_ERR_OK)
        {
            return count;
        }
        if(value[0] != 67)
        {
            passed[0][3] = 0;
        }
        /* Check Rx counters */
        if((ret = rtk_port_phyReg_get(1, 0xc81, 16, &value[0])) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_get(1, 0xc81, 17, &value[1])) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_get(1, 0xc81, 18, &value[2])) != RT_ERR_OK)
        {
            return count;
        }
        if((value[0]) != 65 || (value[1] != 0) || (value[2] != 0) )
        {
            passed[1][0] = 0;
        }
        if((ret = rtk_port_phyReg_get(2, 0xc81, 16, &value[0])) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_get(2, 0xc81, 17, &value[1])) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_get(2, 0xc81, 18, &value[2])) != RT_ERR_OK)
        {
            return count;
        }
        if((value[0]) != 64 || (value[1] != 0) || (value[2] != 0) )
        {
            passed[1][1] = 0;
        }
        if((ret = rtk_port_phyReg_get(3, 0xc81, 16, &value[0])) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_get(3, 0xc81, 17, &value[1])) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_get(3, 0xc81, 18, &value[2])) != RT_ERR_OK)
        {
            return count;
        }
        if((value[0]) != 66 || (value[1] != 0) || (value[2] != 0) )
        {
            passed[1][2] = 0;
        }
        if((ret = rtk_port_phyReg_get(4, 0xc81, 16, &value[0])) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_get(4, 0xc81, 17, &value[1])) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_get(4, 0xc81, 18, &value[2])) != RT_ERR_OK)
        {
            return count;
        }
        if((value[0]) != 67 || (value[1] != 0) || (value[2] != 0) )
        {
            passed[1][3] = 0;
        }
        /* Show result */
        printk("port 1 tx %s rx %s\n", passed[0][0] ? "PASS" : "FAIL", passed[1][0] ? "PASS" : "FAIL" );
        printk("port 2 tx %s rx %s\n", passed[0][1] ? "PASS" : "FAIL", passed[1][1] ? "PASS" : "FAIL" );
        printk("port 3 tx %s rx %s\n", passed[0][2] ? "PASS" : "FAIL", passed[1][2] ? "PASS" : "FAIL" );
        printk("port 4 tx %s rx %s\n", passed[0][3] ? "PASS" : "FAIL", passed[1][3] ? "PASS" : "FAIL" );
    }
    return count;
}

static int ether_test_open_proc(struct inode *inode, struct file *file)
{
	return single_open(file, ether_test_read_proc, NULL);
}
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
struct proc_ops ether_fop = {
	.proc_open = ether_test_open_proc,
	.proc_write = ether_test_write_proc,
	.proc_read = seq_read,
	.proc_lseek = seq_lseek,
	.proc_release = single_release,
};
#else
struct file_operations ether_fop = {
	.open = ether_test_open_proc,
	.write = ether_test_write_proc,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};
#endif
#endif

/*****************************************************
*
* GPIO Test Related fuction
*
*****************************************************/

#if 0
uint32 gpio_array0[2][20] = {
    { 0,  1,  3,  4,  5, 22,  8,  9, 10, 12, 13, 14, 32, 33, 35, 36, 37, 38, 39, 58},
    {30,  2, 69, 20, 21, 23, 24, 25, 26, 27, 28, 29, 56, 57, 59, 60, 61, 62, 63, 40}
};
#else
uint32 gpio_array0[2][21] = {
    { 0,  1,  2,  3,  4,  5,  6,  7,  8, 13, 14, 66, 33, 34, 35, 36, 37, 38, 39, 64, 67},
    {23, 17, 18, 19, 20, 21, 22, 30,  9, 26, 27, 69, 57, 58, 59, 60, 61, 62, 63, 40, 68}
};
#endif

uint32 gpio_array1[2][13] = {
    { 1,  0,  5,  4, 20, 21, 33,  6,  7, 19, 18, 64, 65},
    {69, 68, 67, 22, 37, 36, 39, 31, 34, 15, 16, 17, 66}
};

uint32 gpio_array2[2][13] = {
    {23, 28,  0,  3,  5, 62, 38, 35, 32, 20, 12,  9,  7},
    {30,  1,  2,  4, 56, 37, 36, 33, 21, 16, 10,  8,  6}
};

uint32 gpio_array3[2][14] = {
    {23, 28,  0,  3,  5, 66, 62, 38, 35, 32, 20, 12,  9, 29},
    {30,  1,  2,  4, 56, 39, 37, 36, 33, 21, 16, 10,  8, 17}
};

uint32 gpio_array4[2][5] = {
    { 5,  4, 20, 21, 33},
    {67, 22, 37, 36, 39}
};

uint32 gpio_array5[2][13] = {
    {23, 28,  0,  3,  5, 60, 62, 38, 35, 32, 20, 12,  9},
    {30,  1,  2,  4, 56, 39, 37, 36, 33, 21, 16, 10,  8}
};

static int gpio_test_read_proc(struct seq_file *seq, void *v)
{
    seq_printf(seq, "GPIO groups:\n");
    seq_printf(seq, "0 -> RTL9607CP QC\n");
    seq_printf(seq, "1 -> RTL9607CP OLT burn in\n");
    seq_printf(seq, "2 -> RTL9607C QC\n");
    seq_printf(seq, "3 -> RTL9603C QC\n");
    seq_printf(seq, "4 -> RTL9607CP OLT check\n");
    seq_printf(seq, "5 -> RTL9607CE QC\n");
   
    return 0;
}

static int gpio_test_write_proc(struct file *file, const char *buffer, size_t count, loff_t *data)
{
    int i, j;
    int ret;
    uint32 *testArray[2], testArraySize;
    uint32 readData;
    char flag[64];
    int groupSel = 0;
    int passed = 1;

    if (buffer && !copy_from_user(&flag, buffer, sizeof(flag)))
    {
        groupSel = simple_strtol(flag, NULL, 0);
        
        switch(groupSel)
        {
        case 0:
            testArray[0] = gpio_array0[0];
            testArray[1] = gpio_array0[1];
            testArraySize = sizeof(gpio_array0[0])/sizeof(uint32);
            break;
        case 1:
            testArray[0] = gpio_array1[0];
            testArray[1] = gpio_array1[1];
            testArraySize = sizeof(gpio_array1[0])/sizeof(uint32);
            break;
        case 2:
            testArray[0] = gpio_array2[0];
            testArray[1] = gpio_array2[1];
            testArraySize = sizeof(gpio_array2[0])/sizeof(uint32);
            break;
        case 3:
            testArray[0] = gpio_array3[0];
            testArray[1] = gpio_array3[1];
            testArraySize = sizeof(gpio_array3[0])/sizeof(uint32);
            break;
        case 4:
            testArray[0] = gpio_array4[0];
            testArray[1] = gpio_array4[1];
            testArraySize = sizeof(gpio_array4[0])/sizeof(uint32);
            break;
        case 5:
            testArray[0] = gpio_array5[0];
            testArray[1] = gpio_array5[1];
            testArraySize = sizeof(gpio_array5[0])/sizeof(uint32);
            break;
        default:
            printk_test("Invalid gpio group %d\n", groupSel);
            return count;
        }
        printk_test("Test GPIO group %d\n", groupSel);

        if((ret=rtk_gpio_init())!=RT_ERR_OK)
        {
            return count;
        }

        for(j = 0;j < testArraySize;j ++)
        {
            /* Enable both GPIOs */
            if((ret=rtk_gpio_state_set(testArray[0][j], ENABLED)) != RT_ERR_OK)
            {
                    return count;
            }
            if((ret=rtk_gpio_state_set(testArray[1][j], ENABLED)) != RT_ERR_OK)
            {
                    return count;
            }
            for(i = 0;i < 2;i ++)
            {
                /* Configure mode for both GPIOs */
                if((ret=rtk_gpio_mode_set(testArray[0][j], GPIO_OUTPUT))!=RT_ERR_OK)
                {
                    return count;
                }
                if((ret=rtk_gpio_mode_set(testArray[1][j], GPIO_INPUT))!=RT_ERR_OK)
                {
                    return count;
                }

                /* Configure data for output GPIO and read from the other one */
                if((ret=rtk_gpio_databit_set(testArray[0][j], i))!=RT_ERR_OK)
                {
                    return count;
                }
                if((ret=rtk_gpio_databit_get(testArray[1][j], &readData))!=RT_ERR_OK)
                {
                    return count;
                }
                /* Check value */
                if(readData != i)
                {
                    printk_test("Test GPIO failed @ gpio[%d]=>gpio[%d] != value %d\n", testArray[0][j], testArray[1][j], i);
                    passed = 0;
                }
            }

            for(i = 0;i < 2;i ++)
            {
                /* Configure mode for both GPIOs */
                if((ret=rtk_gpio_mode_set(testArray[1][j], GPIO_OUTPUT))!=RT_ERR_OK)
                {
                    return count;
                }
                if((ret=rtk_gpio_mode_set(testArray[0][j], GPIO_INPUT))!=RT_ERR_OK)
                {
                    return count;
                }

                /* Configure data for output GPIO and read from the other one */
                if((ret=rtk_gpio_databit_set(testArray[1][j], i))!=RT_ERR_OK)
                {
                    return count;
                }
                if((ret=rtk_gpio_databit_get(testArray[0][j], &readData))!=RT_ERR_OK)
                {
                    return count;
                }
                /* Check value */
                if(readData != i)
                {
                    printk_test("Test GPIO failed @ gpio[%d]=>gpio[%d] != value %d\n", testArray[1][j], testArray[0][j], i);
                    passed = 0;
                }
            }
            /* Disable both GPIOs */
            if((ret=rtk_gpio_state_set(testArray[0][j], DISABLED)) != RT_ERR_OK)
            {
                return count;
            }
            if((ret=rtk_gpio_state_set(testArray[1][j], DISABLED)) != RT_ERR_OK)
            {
                return count;
            }
        }
    }

    printk_test("%s!\n", (passed == 1) ? "passed" : "failed");

    return count;
}

static int gpio_test_open_proc(struct inode *inode, struct file *file)
{
	return single_open(file, gpio_test_read_proc, NULL);
}
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
struct proc_ops gpio_fop = {
	.proc_open = gpio_test_open_proc,
	.proc_write = gpio_test_write_proc,
	.proc_read = seq_read,
	.proc_lseek = seq_lseek,
	.proc_release = single_release,
};
#else
struct file_operations gpio_fop = {
	.open = gpio_test_open_proc,
	.write = gpio_test_write_proc,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};
#endif


/*****************************************************
*
* USB Test Related fuction
*
*****************************************************/
static int usb_test_read_proc(struct seq_file *seq, void *v)
{
    return 0;
}

static int usb_test_write_proc(struct file *file, const char *buffer, size_t count, loff_t *data)
{
    int ret;
    char flag[64];
    uint32 value;

    if (buffer && !copy_from_user(&flag, buffer, sizeof(flag)))
    {
        /* Start USB testing */
        value = 0x40;
        if ((ret = ioal_socMem32_write(0xB8060424, value)) != RT_ERR_OK)
        {
            QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
            return count;
        }
        value = 0x00200400;
        if ((ret = ioal_socMem32_write(0xB8021054, value)) != RT_ERR_OK)
        {
            QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
            return count;
        }
    }

    return count;
}

static int usb_test_open_proc(struct inode *inode, struct file *file)
{
	return single_open(file, usb_test_read_proc, NULL);
}
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
struct proc_ops usb_fop = {
	.proc_open = usb_test_open_proc,
	.proc_write = usb_test_write_proc,
	.proc_read = seq_read,
	.proc_lseek = seq_lseek,
	.proc_release = single_release,
};
#else
struct file_operations usb_fop = {
	.open = usb_test_open_proc,
	.write = usb_test_write_proc,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};
#endif


/*****************************************************
*
* ETH Test Related fuction
*
*****************************************************/
static int eth_test_read_proc(struct seq_file *seq, void *v)
{
    return 0;
}

static int eth_test_write_proc(struct file *file, const char *buffer, size_t count, loff_t *data)
{
    int ret;
    char flag[64];
    uint32 value;
    rtk_port_macAbility_t macAbility;

    if (buffer && !copy_from_user(&flag, buffer, sizeof(flag)))
    {
        /* DMP */
        value = 0x11a;
        if((ret = reg_write(RTL9607C_EN_FORCE_P_DMPr, &value)) != RT_ERR_OK)
        {
            QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
            return -1;
        }
        value = 1<<2;
        if((ret = reg_array_field_write(RTL9607C_FORCE_P_DMPr, 1, REG_ARRAY_INDEX_NONE, RTL9607C_FORCE_PROT_MASKf, &value)) != RT_ERR_OK)
        {
            QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
            return -1;
        }
        value = 1<<4;
        if((ret = reg_array_field_write(RTL9607C_FORCE_P_DMPr, 3, REG_ARRAY_INDEX_NONE, RTL9607C_FORCE_PROT_MASKf, &value)) != RT_ERR_OK)
        {
            QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
            return -1;
        }
        value = 1<<8;
        if((ret = reg_array_field_write(RTL9607C_FORCE_P_DMPr, 4, REG_ARRAY_INDEX_NONE, RTL9607C_FORCE_PROT_MASKf, &value)) != RT_ERR_OK)
        {
            QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
            return -1;
        }
        value = 1<<0;
        if((ret = reg_array_field_write(RTL9607C_FORCE_P_DMPr, 8, REG_ARRAY_INDEX_NONE, RTL9607C_FORCE_PROT_MASKf, &value)) != RT_ERR_OK)
        {
            QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
            return -1;
        }

        /* RGMII1 init */
        value = 0x1100;
        if((ret = reg_write(RTL9607C_CFG_RGMII_CTRL_0r, &value)) != RT_ERR_OK)
        {
            QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
            return -1;
        }
        value = 0x7f60000;
        if((ret = reg_write(RTL9607C_IOPAD_CFGr, &value)) != RT_ERR_OK)
        {
            QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
            return -1;
        }
        value = 0x1;
        if((ret = reg_field_write(RTL9607C_CFG_EXT_CTRL_0r, RTL9607C_EXT_MODEf, &value)) != RT_ERR_OK)
        {
            QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
            return -1;
        }
        osal_memset(&macAbility, 0x00, sizeof(rtk_port_macAbility_t));
        macAbility.speed           = PORT_SPEED_1000M;
        macAbility.duplex          = PORT_FULL_DUPLEX;
        macAbility.linkFib1g       = DISABLED;
        macAbility.linkStatus      = PORT_LINKUP;
        macAbility.txFc            = ENABLED;
        macAbility.rxFc            = ENABLED;
        macAbility.nwayAbility     = DISABLED;
        macAbility.masterMod       = DISABLED;
        macAbility.nwayFault       = DISABLED;
        macAbility.lpi_100m        = DISABLED;
        macAbility.lpi_giga        = DISABLED;
        if((ret = rtk_port_macForceAbility_set(8, macAbility)) != RT_ERR_OK)
        {
            QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
            return -1;
        }
        if((ret = rtk_port_macForceAbilityState_set(8, ENABLED)) != RT_ERR_OK)
        {
            QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
            return -1;
        }

        /* MDI loopback */
        if((ret = rtk_port_phyReg_set(4, 0xa43, 24, 0xd9c)) != RT_ERR_OK)
        {
            QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
            return -1;
        }

        /* Generate packet */
        if((ret = rtk_port_phyReg_set(0, 0xc80, 17, 0x0070)) != RT_ERR_OK)
        {
            QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
            return -1;
        }
        if((ret = rtk_port_phyReg_set(0, 0xc80, 17, 0x0003)) != RT_ERR_OK)
        {
            QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
            return -1;
        }
        if((ret = rtk_port_phyReg_set(0, 0xc80, 19, 0x05EE)) != RT_ERR_OK)
        {
            QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
            return -1;
        }
        if((ret = rtk_port_phyReg_set(0, 0xc80, 20, 0x0000)) != RT_ERR_OK)
        {
            QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
            return -1;
        }
        if((ret = rtk_port_phyReg_set(0, 0xc80, 21, 0x0000)) != RT_ERR_OK)
        {
            QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
            return -1;
        }
        if((ret = rtk_port_phyReg_set(0, 0xc80, 16, 0x5A23)) != RT_ERR_OK)
        {
            QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
            return -1;
        }

    }

    return count;
}

static int eth_test_open_proc(struct inode *inode, struct file *file)
{
	return single_open(file, eth_test_read_proc, NULL);
}
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
struct proc_ops eth_fop = {
	.proc_open = eth_test_open_proc,
	.proc_write = eth_test_write_proc,
	.proc_read = seq_read,
	.proc_lseek = seq_lseek,
	.proc_release = single_release,
};
#else
struct file_operations eth_fop = {
	.open = eth_test_open_proc,
	.write = eth_test_write_proc,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};
#endif

/*****************************************************
*
* Ethernet Test Related fuction
*
*****************************************************/

static int loopback_test_read_proc(struct seq_file *seq, void *v)
{
    return 0;
}

static int loopback_test_write_proc(struct file *file, const char *buffer, size_t count, loff_t *data)
{
    int ret;
    char flag[64];
    uint32 value[3];
    int passed[2][4] = { {1, 1, 1, 1}, {1, 1, 1, 1}};

    if (buffer && !copy_from_user(&flag, buffer, sizeof(flag)))
    {
        /* Port 3 & 4 MDI loopback */
        if((ret = rtk_port_phyReg_set(3, 0xa43, 24, 0xd9c)) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_set(4, 0xa43, 24, 0xd9c)) != RT_ERR_OK)
        {
            return count;
        }
        mdelay(6000);
        /* Reset counter */
        if((ret = rtk_port_phyReg_set(1, 0xc80, 17, 0x0070)) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_set(2, 0xc80, 17, 0x0070)) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_set(3, 0xc80, 17, 0x0070)) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_set(4, 0xc80, 17, 0x0070)) != RT_ERR_OK)
        {
            return count;
        }
        /* Reset Tx/Rx */
        if((ret = rtk_port_phyReg_set(1, 0xc80, 17, 0x3)) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_set(2, 0xc80, 17, 0x3)) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_set(3, 0xc80, 17, 0x3)) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_set(4, 0xc80, 17, 0x3)) != RT_ERR_OK)
        {
            return count;
        }
        /* Set packet length */
        if((ret = rtk_port_phyReg_set(1, 0xc80, 19, 0x5ee)) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_set(2, 0xc80, 19, 0x5ee)) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_set(3, 0xc80, 19, 0x5ee)) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_set(4, 0xc80, 19, 0x5ee)) != RT_ERR_OK)
        {
            return count;
        }
        /* Set packet count */
        if((ret = rtk_port_phyReg_set(1, 0xc80, 20, 0x64)) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_set(1, 0xc80, 21, 0x0)) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_set(2, 0xc80, 20, 0x65)) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_set(2, 0xc80, 21, 0x0)) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_set(3, 0xc80, 20, 0x66)) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_set(3, 0xc80, 21, 0x0)) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_set(4, 0xc80, 20, 0x67)) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_set(4, 0xc80, 21, 0x0)) != RT_ERR_OK)
        {
            return count;
        }
        /* Select Rx/Tx & enable pkt gen */
        if((ret = rtk_port_phyReg_set(1, 0xc80, 16, 0x5a22)) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_set(2, 0xc80, 16, 0x5a22)) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_set(3, 0xc80, 16, 0x5a22)) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_set(4, 0xc80, 16, 0x5a22)) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_set(1, 0xc80, 16, 0x5a23)) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_set(2, 0xc80, 16, 0x5a23)) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_set(3, 0xc80, 16, 0x5a23)) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_set(4, 0xc80, 16, 0x5a23)) != RT_ERR_OK)
        {
            return count;
        }
        /* Wait for Tx/Rx complete */
        mdelay(1000);
        /* Disable pkt gen */
        if((ret = rtk_port_phyReg_set(1, 0xc80, 16, 0x5a00)) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_set(2, 0xc80, 16, 0x5a00)) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_set(3, 0xc80, 16, 0x5a00)) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_set(4, 0xc80, 16, 0x5a00)) != RT_ERR_OK)
        {
            return count;
        }
        /* Check Tx counters */
        if((ret = rtk_port_phyReg_get(1, 0xc80, 22, &value[0])) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_get(1, 0xc80, 23, &value[1])) != RT_ERR_OK)
        {
            return count;
        }
        if((value[0] != 0x64) || (value[1] != 0))
        {
            passed[0][0] = 0;
        }
        if((ret = rtk_port_phyReg_get(2, 0xc80, 22, &value[0])) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_get(2, 0xc80, 23, &value[1])) != RT_ERR_OK)
        {
            return count;
        }
        if((value[0] != 0x65) || (value[1] != 0))
        {
            passed[0][1] = 0;
        }
        if((ret = rtk_port_phyReg_get(3, 0xc80, 22, &value[0])) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_get(3, 0xc80, 23, &value[1])) != RT_ERR_OK)
        {
            return count;
        }
        if((value[0] != 0x66) || (value[1] != 0))
        {
            passed[0][2] = 0;
        }
        if((ret = rtk_port_phyReg_get(4, 0xc80, 22, &value[0])) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_get(4, 0xc80, 23, &value[1])) != RT_ERR_OK)
        {
            return count;
        }
        if((value[0] != 0x67) || (value[1] != 0))
        {
            passed[0][3] = 0;
        }
        /* Check Rx counters */
        if((ret = rtk_port_phyReg_get(1, 0xc81, 16, &value[0])) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_get(1, 0xc81, 17, &value[1])) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_get(1, 0xc81, 18, &value[2])) != RT_ERR_OK)
        {
            return count;
        }
        if((value[0] != 0x65) || (value[1] != 0) || (value[2] != 0) )
        {
            passed[1][0] = 0;
            printk("%s:%d value %d %d %d\n", __FILE__, __LINE__, value[0], value[1], value[2]);
        }
        if((ret = rtk_port_phyReg_get(2, 0xc81, 16, &value[0])) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_get(2, 0xc81, 17, &value[1])) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_get(2, 0xc81, 18, &value[2])) != RT_ERR_OK)
        {
            return count;
        }
        if((value[0] != 0x64) || (value[1] != 0) || (value[2] != 0) )
        {
            passed[1][1] = 0;
            printk("%s:%d value %d %d %d\n", __FILE__, __LINE__, value[0], value[1], value[2]);
        }
        if((ret = rtk_port_phyReg_get(3, 0xc81, 16, &value[0])) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_get(3, 0xc81, 17, &value[1])) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_get(3, 0xc81, 18, &value[2])) != RT_ERR_OK)
        {
            return count;
        }
        if((value[0] != 0x66) || (value[1] != 0) || (value[2] != 0) )
        {
            passed[1][2] = 0;
            printk("%s:%d value %d %d %d\n", __FILE__, __LINE__, value[0], value[1], value[2]);
        }
        if((ret = rtk_port_phyReg_get(4, 0xc81, 16, &value[0])) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_get(4, 0xc81, 17, &value[1])) != RT_ERR_OK)
        {
            return count;
        }
        if((ret = rtk_port_phyReg_get(4, 0xc81, 18, &value[2])) != RT_ERR_OK)
        {
            return count;
        }
        if((value[0] != 0x67) || (value[1] != 0) || (value[2] != 0) )
        {
            passed[1][3] = 0;
            printk("%s:%d value %d %d %d\n", __FILE__, __LINE__, value[0], value[1], value[2]);
        }
        /* Show result */
        printk("port 1 tx %s rx %s\n", passed[0][0] ? "PASS" : "FAIL", passed[1][0] ? "PASS" : "FAIL" );
        printk("port 2 tx %s rx %s\n", passed[0][1] ? "PASS" : "FAIL", passed[1][1] ? "PASS" : "FAIL" );
        printk("port 3 tx %s rx %s\n", passed[0][2] ? "PASS" : "FAIL", passed[1][2] ? "PASS" : "FAIL" );
        printk("port 4 tx %s rx %s\n", passed[0][3] ? "PASS" : "FAIL", passed[1][3] ? "PASS" : "FAIL" );
    }
    return count;
}

static int loopback_test_open_proc(struct inode *inode, struct file *file)
{
	return single_open(file, loopback_test_read_proc, NULL);
}
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
struct proc_ops loopback_fop = {
	.proc_open = loopback_test_open_proc,
	.proc_write = loopback_test_write_proc,
	.proc_read = seq_read,
	.proc_lseek = seq_lseek,
	.proc_release = single_release,
};
#else
struct file_operations loopback_fop = {
	.open = loopback_test_open_proc,
	.write = loopback_test_write_proc,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};
#endif

/*****************************************************
*
* OLT Related fuction
*
*****************************************************/
#define QC_GPIO_SCRIPT "/tmp/qc_gpio_olt.sh"
static char gpioScript[] = 
"#!/bin/sh\n" \
"while [ 1 ] ; do\n" \
"   echo 1 > /proc/qc_test/gpio\n" \
"   sleep 1\n" \
"done\n";

#define QC_GPON_SCRIPT "/tmp/qc_gpon_prbs_olt.sh"
static char gponPrbsScript[] = 
"#!/bin/sh\n" \
"while [ 1 ] ; do\n" \
"   echo 31 0 0 0 0 > /proc/qc_test/prbs\n" \
"done\n";

#define QC_HSG0_SCRIPT "/tmp/qc_hsg0_prbs_olt.sh"
static char hsg0PrbsScript[] = 
"#!/bin/sh\n" \
"while [ 1 ] ; do\n" \
"   echo 0 0 0 15 0 > /proc/qc_test/prbs\n" \
"done\n";

#define QC_HSG1_SCRIPT "/tmp/qc_hsg1_prbs_olt.sh"
static char hsg1PrbsScript[] = 
"#!/bin/sh\n" \
"while [ 1 ] ; do\n" \
"   echo 0 0 0 0 15 > /proc/qc_test/prbs\n" \
"done\n";

#define QC_PCM_SCRIPT "/tmp/qc_pcm_olt.sh"
static char pcmScript[] = 
"#!/bin/sh\n" \
"while [ 1 ] ; do\n" \
"   echo 11 > /proc/qc_test/pcm\n" \
"   echo 12 > /proc/qc_test/pcm\n" \
"   echo 13 > /proc/qc_test/pcm\n" \
"   echo 14 > /proc/qc_test/pcm\n" \
"   echo 15 > /proc/qc_test/pcm\n" \
"done\n";

#define QC_ALL_SCRIPT "/tmp/qc_olt.sh"
static char allScript[] = 
"#!/bin/sh\n" \
"sleep 15\n" \
"echo 1 > /proc/qc_test/olt_init\n" \
"echo 1 > /proc/qc_test/usb &\n" \
QC_GPIO_SCRIPT " &\n" \
QC_GPON_SCRIPT " &\n" \
QC_HSG0_SCRIPT " &\n" \
QC_HSG1_SCRIPT " &\n" \
QC_PCM_SCRIPT " &\n" \
"echo 1 > /proc/qc_test/eth &\n" \
"sleep 3\n" \
"echo 2 > /proc/qc_test/olt_init\n";

static struct task_struct *olt_led_thread;

#define OLT_CONFIG_GPIO     40

static int olt_led_poll(void *arg)
{
    int ret;
    uint32 timeout;
    uint32 cpu_toogle = 0;
    uint32 value;

    /* Init GPIO for LED */
    if((ret = rtk_gpio_state_set(43, ENABLED)) != RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
    }
    if((ret = rtk_gpio_mode_set(43, GPIO_OUTPUT)) != RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
    }

    for(;;) {
        if (kthread_should_stop()) break;

        do {
            set_current_state(TASK_INTERRUPTIBLE);
            /* Polling every 100 ms */
            timeout = schedule_timeout(10 * HZ / 100);

            /* Toogle CPU LED(GPIO) to indicate still running */
            if((ret = rtk_gpio_databit_set(43, cpu_toogle == 0 ? 0 : 1 )) != RT_ERR_OK)
            {
                QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
            }
            cpu_toogle = !cpu_toogle;

            /* Check HSG0 link status */
            if((ret = reg_read(RTL9607C_HSG0_SDS_EXT_REG30r, &value)) != RT_ERR_OK)
            {
                QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
            }
            if(value != 0x100)
            {
                if((ret = rtk_led_modeForce_set(5, LED_FORCE_OFF)) != RT_ERR_OK)
                {
                    QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
                }
            }
            else
            {
                if((ret = rtk_led_modeForce_set(5, LED_FORCE_ON)) != RT_ERR_OK)
                {
                    QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
                }
            }

            /* Check HSG1 link status */
            if((ret = reg_read(RTL9607C_HSG1_SDS_EXT_REG30r, &value)) != RT_ERR_OK)
            {
                QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
            }
            if(value != 0x100)
            {
                if((ret = rtk_led_modeForce_set(4, LED_FORCE_OFF)) != RT_ERR_OK)
                {
                    QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
                }
            }
            else
            {
                if((ret = rtk_led_modeForce_set(4, LED_FORCE_ON)) != RT_ERR_OK)
                {
                    QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
                }
            }
        } while(timeout);
    }

    return 0;
}

static int olt_init_read_proc(struct seq_file *seq, void *v)
{
    int ret;
    uint32 data;
    
    if((ret=rtk_gpio_state_set(OLT_CONFIG_GPIO, ENABLED)) != RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }
    if((ret=rtk_gpio_mode_set(OLT_CONFIG_GPIO, GPIO_INPUT)) != RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }
    if((ret=rtk_gpio_databit_get(OLT_CONFIG_GPIO, &data)) != RT_ERR_OK)
    {
        QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
        return -1;
    }

    if(data == 0)
    {
        olt_mode = 1;
    }
    else
    {
        olt_mode = 0;
    }

    seq_printf(seq, "%u", olt_mode);
    
    return 0;
}

static int olt_init_write_proc(struct file *file, const char *buffer, size_t count, loff_t *data)
{
    int ret;
    char flag[64];
    uint32 init_state, value, cnt;
    rtk_led_config_t ledConfig;

    if (buffer && !copy_from_user(&flag, buffer, sizeof(flag)))
    {
        init_state = simple_strtol(flag, NULL, 0);
        switch(init_state)
        {
        case 0:            
            /* Prepare OLT scripts only */
            qc_write_data(QC_GPIO_SCRIPT, gpioScript, sizeof(gpioScript));
            qc_write_data(QC_GPON_SCRIPT, gponPrbsScript, sizeof(gponPrbsScript));
            qc_write_data(QC_HSG0_SCRIPT, hsg0PrbsScript, sizeof(hsg0PrbsScript));
            qc_write_data(QC_HSG1_SCRIPT, hsg1PrbsScript, sizeof(hsg1PrbsScript));
            qc_write_data(QC_PCM_SCRIPT, pcmScript, sizeof(pcmScript));
            qc_write_data(QC_ALL_SCRIPT, allScript, sizeof(allScript));
            break;
        case 1:
            /* disable log */
            log = 0;

            /* Init OLT testing */
            /* Init LED */
            rtk_led_operation_set(LED_OP_PARALLEL);
            rtk_led_parallelEnable_set(7,ENABLED);
            rtk_led_parallelEnable_set(6,ENABLED);
            rtk_led_parallelEnable_set(3,ENABLED);
            rtk_led_parallelEnable_set(2,ENABLED);
            rtk_led_parallelEnable_set(17,ENABLED);
            rtk_led_parallelEnable_set(15,ENABLED);
            rtk_led_parallelEnable_set(5,ENABLED);
            rtk_led_parallelEnable_set(4,ENABLED);

            memset(&ledConfig,0x0,sizeof(rtk_led_config_t));
            ledConfig.ledEnable[LED_CONFIG_SPD1000ACT] = ENABLED;
            ledConfig.ledEnable[LED_CONFIG_SPD500ACT] = ENABLED;
            ledConfig.ledEnable[LED_CONFIG_SPD100ACT] = ENABLED;
            ledConfig.ledEnable[LED_CONFIG_SPD10ACT] = ENABLED;
            ledConfig.ledEnable[LED_CONFIG_SPD1000] = ENABLED;
            ledConfig.ledEnable[LED_CONFIG_SPD500] = ENABLED;
            ledConfig.ledEnable[LED_CONFIG_SPD100] = ENABLED;
            ledConfig.ledEnable[LED_CONFIG_SPD10] = ENABLED;

            rtk_led_config_set(7,LED_TYPE_UTP0,&ledConfig);
            rtk_led_config_set(6,LED_TYPE_UTP1,&ledConfig);
            rtk_led_config_set(3,LED_TYPE_UTP2,&ledConfig);
            rtk_led_config_set(2,LED_TYPE_UTP3,&ledConfig);
            rtk_led_config_set(17,LED_TYPE_UTP4,&ledConfig);
            rtk_led_config_set(15,LED_TYPE_RGMII,&ledConfig);

            memset(&ledConfig,0x0,sizeof(rtk_led_config_t));
            ledConfig.ledEnable[LED_CONFIG_FORCE_MODE] = ENABLED;

            /* Phony type LED_TYPE_UTP0 */
            rtk_led_config_set(5,LED_TYPE_UTP0,&ledConfig);
            rtk_led_config_set(4,LED_TYPE_UTP0,&ledConfig);

            /* PCIE1 clock running */
            value = 0x8251;
            if((ret = reg_write(RTL9607C_WSDS_HSG1_DIG_02r, &value)) != RT_ERR_OK)
            {
                QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
                return count;
            }
            value = 0xc000;
            if((ret = reg_write(RTL9607C_WSDS_HSG1_DIG_01r, &value)) != RT_ERR_OK)
            {
                QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
                return count;
            }

            /* Remove GPIO 32 which is the same pin as LED 15 */
            rtk_gpio_state_set(32, DISABLED);

            /* Start OLT LED polling thread */
            olt_led_thread = kthread_create(olt_led_poll, &data, "olt_led_poll");
            if (IS_ERR(olt_led_thread)) {
                ret = PTR_ERR(olt_led_thread);
                olt_led_thread = NULL;
                QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
                return count;
            }
            wake_up_process(olt_led_thread);
            /* Link down CPU port to prevent packet sent to CPU */
            if((ret = rtk_port_macForceAbilityState_set(9, DISABLED)) != RT_ERR_OK)
            {
                QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
            }
            break;
        case 2:
            /* Some configure must be done after all test started */
            /* PCIE1 clock running */
            value = 0x8251;
            if((ret = reg_write(RTL9607C_WSDS_HSG1_DIG_02r, &value)) != RT_ERR_OK)
            {
                QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
                return count;
            }
            value = 0xc000;
            if((ret = reg_write(RTL9607C_WSDS_HSG1_DIG_01r, &value)) != RT_ERR_OK)
            {
                QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
                return count;
            }
            while(1)
            {
                if((ret = reg_field_read(RTL9607C_WSDS_HSG1_DIG_01r, RTL9607C_CFG_SDS_MDX_REQf, &value)) != RT_ERR_OK)
                {
                    QC_PRINTK("%s:%d %d\n", __FILE__, __LINE__, ret);
                    return count;
                }

                if(value == 0)
                {
                    break;
                }
                cnt --;
                if(cnt <= 0)
                {
                    QC_PRINTK("%s:%d\n", __FILE__, __LINE__);
                    return count;
                }
            }
            break;
        }
    }

    return count;
}

static int olt_init_open_proc(struct inode *inode, struct file *file)
{
	return single_open(file, olt_init_read_proc, NULL);
}
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
struct proc_ops olt_init_fop = {
	.proc_open = olt_init_open_proc,
	.proc_write = olt_init_write_proc,
	.proc_read = seq_read,
	.proc_lseek = seq_lseek,
	.proc_release = single_release,
};
#else
struct file_operations olt_init_fop = {
	.open = olt_init_open_proc,
	.write = olt_init_write_proc,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};
#endif

/*****************************************************
*
* Debug Related fuction
*
*****************************************************/

static int debug_read_proc(struct seq_file *seq, void *v)
{
    printk_test("debug = %d\n", debug_log);
    return 0;
}

static int debug_write_proc(struct file *file, const char *buffer, size_t count, loff_t *data)
{
    char flag[64];

    if (buffer && !copy_from_user(&flag, buffer, sizeof(flag)))
    {
        debug_log = simple_strtol(flag, NULL, 0);
    }

    return count;
}

static int debug_open_proc(struct inode *inode, struct file *file)
{
	return single_open(file, debug_read_proc, NULL);
}
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
struct proc_ops debug_fop = {
	.proc_open = debug_open_proc,
	.proc_write = debug_write_proc,
	.proc_read = seq_read,
	.proc_lseek = seq_lseek,
	.proc_release = single_release,
};
#else
struct file_operations debug_fop = {
	.open = debug_open_proc,
	.write = debug_write_proc,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};
#endif

static int pcm_test_open_proc(struct inode *inode, struct file *file)
{
	return single_open(file, pcm_test_read_proc, NULL);
}
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
struct proc_ops pcm_fop = {
	.proc_open = pcm_test_open_proc,
	.proc_write = pcm_test_write_proc,
	.proc_read = seq_read,
	.proc_lseek = seq_lseek,
	.proc_release = single_release,
};
#else
struct file_operations pcm_fop = {
	.open = pcm_test_open_proc,
	.write = pcm_test_write_proc,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};
#endif


/*****************************************************
*
* Read/Write File API
*
******************************************************/

static mm_segment_t oldfs; 

static void qc_kfsEnvInit(void){ 
   oldfs = get_fs(); 
   set_fs(KERNEL_DS); 
} 

static void qc_kfsEnvDeinit(void){
   set_fs(oldfs); 
}

static struct file *qc_openFile(char *path,int flag,int mode){ 
   struct file *fp; 
	
   fp=filp_open(path, flag, 0x775); 
   if (fp) return fp; 
   else return NULL; 
}

static int qc_writeFile(struct file *fp,char *buf,int readlen) { 
   if (fp->f_op && fp->f_op->write) 
	  return fp->f_op->write(fp,buf,readlen, &fp->f_pos); 
   else 
	  return -1; 
} 

static int qc_readFile(struct file *fp,char *buf,int readlen) 
{ 
   if (fp->f_op && fp->f_op->read) 
	  return fp->f_op->read(fp,buf,readlen, &fp->f_pos); 
   else 
	  return -1; 
} 

static int qc_closeFile(struct file *fp) { 
   filp_close(fp,NULL); 
   return 0; 
}

void qc_write_data(char *filename, char *data, uint32 len){
   struct file *fp; 

   qc_kfsEnvInit(); 

   /*write to file*/
   fp = qc_openFile(filename, O_CREAT | O_WRONLY | O_APPEND, 0); 
   if (fp!= NULL) { 
	  qc_writeFile(fp, data, len);
   }

   qc_closeFile(fp); 
   
   qc_kfsEnvDeinit();
}

static int __init qc_test_module_init(void)
{
    struct proc_dir_entry *dir = NULL;
    struct proc_dir_entry *pcm=NULL;
    struct proc_dir_entry *prbs=NULL;
    struct proc_dir_entry *ether=NULL;
    struct proc_dir_entry *gpio=NULL;
	struct proc_dir_entry *usb=NULL;
	struct proc_dir_entry *eth=NULL;
	struct proc_dir_entry *loopback=NULL;
	struct proc_dir_entry *olt_init=NULL;
	struct proc_dir_entry *debug=NULL;
#ifdef RTL9607C
	u32 regValue = 0;
#endif
    int ret;
    rtk_port_macAbility_t ability;

    dir = proc_mkdir("qc_test",NULL);

	pcm = proc_create("pcm", 0, dir, &pcm_fop);
	if (!pcm) {
		QC_PRINTK("proc pcm_test create fail\n");
	}

#ifdef RTL9607C
	regValue = RTL_R32(IP_ENABLE);
	regValue |= (1 << 0); //enable EN_PREI_VOIP
	regValue |= (1 << 2); //EN_PCM
	regValue |= (1 << 11); //EN_VOIPACC
	regValue |= (1 << 12); //EN_VOIPFFT
	RTL_W32(IP_ENABLE  , regValue);
#endif

	prbs = proc_create("prbs", 0, dir, &prbs_fop);
	if (!prbs) {
		QC_PRINTK("proc prbs_test create fail\n");
	}
	
#if 0	
	prbs = proc_create("ether", 0, dir, &ether_fop);
	if (!prbs) {
		QC_PRINTK("proc ether_test create fail\n");
	}
#endif

	gpio = proc_create("gpio", 0, dir, &gpio_fop);
	if (!gpio) {
		QC_PRINTK("proc gpio_test create fail\n");
	}

	usb = proc_create("usb", 0, dir, &usb_fop);
	if (!usb) {
		QC_PRINTK("proc usb_test create fail\n");
	}

	eth = proc_create("eth", 0, dir, &eth_fop);
	if (!eth) {
		QC_PRINTK("proc eth_test create fail\n");
	}

	loopback = proc_create("loopback", 0, dir, &loopback_fop);
	if (!loopback) {
		QC_PRINTK("proc loopback_fop create fail\n");
	}

	olt_init = proc_create("olt_init", 0, dir, &olt_init_fop);
	if (!olt_init) {
		QC_PRINTK("proc olt_init create fail\n");
	}

	debug = proc_create("debug", 0, dir, &debug_fop);
	if (!debug) {
		QC_PRINTK("proc debug create fail\n");
	}

    /* Link down for port 9 & port 10 to prevent ingress packet from CPU */
    if((ret = rtk_port_macForceAbility_get(9, &ability)) != RT_ERR_OK)
    {
        return 0;
    }
    ability.linkStatus = PORT_LINKDOWN;
    if((ret = rtk_port_macForceAbility_set(9, ability)) != RT_ERR_OK)
    {
        return 0;
    }
    if((ret = rtk_port_macForceAbility_get(10, &ability)) != RT_ERR_OK)
    {
        return 0;
    }
    ability.linkStatus = PORT_LINKDOWN;
    if((ret = rtk_port_macForceAbility_set(10, ability)) != RT_ERR_OK)
    {
        return 0;
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
