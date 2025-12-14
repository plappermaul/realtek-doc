/*
* Copyright c                  Realtek Semiconductor Corporation, 2006  
* All rights reserved.
*/

#ifndef _SPI_H_
#define _SPI_H_



typedef unsigned char uint8;
typedef unsigned int uint32;
typedef int int32;
// ------------------------------------------------------------------------------
// SPI dev context definition 
// ------------------------------------------------------------------------------

#define SPI_IP_VERSION		0x05	// 0.5 
#define SPI_BASE_ADDR		0xB8009000

// 0x00: SPI Control Register 
#define SPICNR_ADDR			( SPI_BASE_ADDR + 0x00 )
#define pSPICNR				( ( volatile uint32 * )SPICNR_ADDR )
	#define SPICNR_CSTIME			0x04000000	// bit 26 
	#define SPICNR_CSTIME_8BIT		0x00000000
	#define SPICNR_CSTIME_16BIT		SPICNR_CSTIME
	#define SPICNR_WRIE				0x02000000	// bit 25
	#define SPICNR_RDIE				0x01000000	// bit 24
	#define SPICNR_ADD_MASK			0x00FF0000	// bit 23:16
	#define SPICNR_ADD_SHIFT		16
	#define SPICNR_CTRL_MASK		0x0000FF00	// bit 15:8
	#define SPICNR_CTRL_SHIFT		8
	#define SPICNR_LSB				0x00000080	// bit 7
	#define SPICNR_MSB				0x00000000
	#define SPICNR_CMD				0x00000020	// bit 5
	#define SPICNR_CMD_READ			0x00000000
	#define SPICNR_CMD_WRITE		SPICNR_CMD
	#define SPICNR_START			0x00000010	// bit 4
	#define SPICNR_SCLK_TYPE		0x00000008	// bit 3
	#define SPICNR_SCLK_TYPE_I		0x00000000
	#define SPICNR_SCLK_TYPE_II		SPICNR_SCLK_TYPE
	#define SPICNR_CSP				0x00000002	// bit 1
	#define SPICNR_CSP_LOW_ACTIVE	0x00000000
	#define SPICNR_CSP_HIGH_ACTIVE	SPICNR_CSP
// 0x04: SPI Status Register 
#define SPISTR_ADDR			( SPI_BASE_ADDR + 0x04 )
#define pSPISTR				( ( volatile uint32 * )SPISTR_ADDR )
	#define SPISTR_RDIP				0x80000000	// bit 31
	#define SPISTR_WDIP				0x40000000	// bit 30
	#define SPISTR_MSIF_LOCK		0x3F000000	// bit 29:24
// 0x08: SPI Clock Divisor Register 
#define SPICKDIV_ADDR		( SPI_BASE_ADDR + 0x08 )
#define pSPICKDIV			( ( volatile uint32 * )SPICKDIV_ADDR )
	#define SPICKDIV_DIV			0xFF000000	// bit 31:24
	#define SPICKDIV_DIV_SHIFT		24
// 0x0C: SPI Read Data Register 
#define SPIRDR_ADDR			( SPI_BASE_ADDR + 0x0C )
#define pSPIRDR				( ( volatile uint32 * )SPIRDR_ADDR )
	#define SPIRDR_RD0R				0xFF000000	// bit 31:24
	#define SPIRDR_RD0R_SHIFT		24
	#define SPIRDR_RD1R				0x00FF0000	// bit 23:16
	#define SPIRDR_RD1R_SHIFT		16
// 0x0C: SPI Write Data Register 
#define SPITDR_ADDR			( SPI_BASE_ADDR + 0x0C )
#define pSPITDR				( ( volatile uint32 * )SPITDR_ADDR )
	#define SPITDR_TD0R				0xFF000000	// bit 31:24
	#define SPIRDR_TD0R_SHIFT		24
	#define SPITDR_TD1R				0x00FF0000	// bit 23:16
	#define SPIRDR_TD1R_SHIFT		16
// 0x10: SPI Transaction Configuration Register 
#define SPITCR_ADDR			( SPI_BASE_ADDR + 0x10 )
#define pSPITCR				( ( volatile uint32 * )SPITCR_ADDR )
	#define SPITCR_SEL_CS0			0x80000000	// bit 31
	#define SPITCR_SEL_CS1			0x40000000	// bit 30
	#define SPITCR_SEL_CS2			0x20000000	// bit 29
	#define SPITCR_SEL_CS3			0x10000000	// bit 28
	#define SPITCR_SEL_CS4			0x08000000	// bit 27
	#define SPITCR_SEL_CS5			0x04000000	// bit 26
	#define SPITCR_SEL_CS( n )		( ( 0x80000000UL >> ( n ) ) & SPITCR_SEL_CS_MASK )
	#define SPITCR_SEL_CS_MASK		0xFC000000	// bit 31:26
	#define SPITCR_CTL_EN			0x00800000	// bit 23
	#define SPITCR_ADD_EN			0x00400000	// bit 22
	#define SPITCR_D0_EN			0x00200000	// bit 21
	#define SPITCR_D1_EN			0x00100000	// bit 20
	#define SPITCR_CSFL				0x00010000	// bit 16
	#define SPITCR_DCS_CNT_UNIT		0x0000FF00	// bit 15:8
	#define SPITCR_DCS_CNT_UNIT_SHIFT	8
	#define SPITCR_DCS_DEF_CNT		0x000000FF	// bit 7:0
	#define SPITCR_DCS_DEF_CNT_SHIFT	0
// 0x14: SPI CS Deselect Time Counter Register 0
#define SPICDTCR0_ADDR		( SPI_BASE_ADDR + 0x14 )
#define pSPICDTCR0			( ( volatile uint32 * )SPICDTCR0_ADDR )
	#define SPICDTCR0_CS0_DES_CN	0xFF000000	// bit 31:24
	#define SPICDTCR0_CS1_DES_CN	0x00FF0000	// bit 23:16
	#define SPICDTCR0_CS2_DES_CN	0x0000FF00	// bit 15:8
	#define SPICDTCR0_CS3_DES_CN	0x000000FF	// bit 7:0
// 0x18: SPI CS Deselect Time Counter Register 1
#define SPICDTCR1_ADDR		( SPI_BASE_ADDR + 0x18 )
#define pSPICDTCR1			( ( volatile uint32 * )SPICDTCR1_ADDR )
	#define SPICDTCR1_CS4_DES_CN	0xFF000000	// bit 31:24
	#define SPICDTCR1_CS5_DES_CN	0x00FF0000	// bit 23:16
// 0x1c: SPI Timing Calibration Register 
#define SPITCALR_ADDR		( SPI_BASE_ADDR + 0x1c )
#define pSPITCALR			( ( volatile uint32 * )SPITCALR_ADDR )
	#define SPITCALR_CS_SETUP_CAL		0xFF000000	// bit 31:24
	#define SPITCALR_CS_SETUP_CAL_SHIEF	24
	#define SPITCALR_CS_HOLD_CAL		0x00FF0000	// bit 23:16
	#define SPITCALR_CS_HOLD_CAL_SHIEF	16
	#define SPITCALR_SD0_SETUP_CAL		0x0000FF00	// bit 15:8
	#define SPITCALR_SD0_SETUP_CAL_SHIEF	8

typedef enum {
	SPI_TYPE_SW,	// software SPI (GPIO)
	SPI_TYPE_HW,	// hardware SPI fully (SPI CS)
	SPI_TYPE_HW2,	// hardware SPI + GPIO CS 
	SPI_TYPE_HW3,	// hardware SPI + GPIO for CSEN
} spi_type_t;

typedef struct rtl_spi_dev_s
{
	spi_type_t type;	// hardware or software 

#ifndef __UC_EDITOR__		// help editor to parse C code 
	union {
#endif
		struct {	// type == SPI_TYPE_SW
			uint32 gpioSCLK;
			uint32 gpioCS_;
			uint32 gpioSDI;
			uint32 gpioSDO;
#ifdef CONFIG_RTK_VOIP_DECT_SPI_SUPPORT
			uint32 gpioINT;
#endif
		} sw;
		
		struct {	// type == SPI_TYPE_HW
			uint32 spiCS_;	// corresponding to SPITCR bit 31:26 
		} hw;
		
		struct {	// type == SPI_TYPE_HW2
			uint32 gpioCS_;
		} hw2;

		struct {	// type == SPI_TYPE_HW3
			uint32 spiCS_;	// corresponding to SPITCR bit 31:26
			uint32 gpioCSEN_;
			uint32 CSMode_;
		} hw3;		
#ifndef __UC_EDITOR__		// help editor to parse C code 
	};
#endif

	// backward compatible variables (going to remove)
#if defined(CONFIG_RTK_VOIP_DRIVERS_SLIC_ZARLINK) && defined(CONFIG_RTK_VOIP_8676_SHARED_SPI)
	uint32 SPI_SEL_CS;
#endif

	//uint32 SClkDelayLoop;
} rtl_spi_dev_t;

// ------------------------------------------------------------------------------
// SPI interfaces 
// ------------------------------------------------------------------------------

// SPI context initialization 
//extern int32 _init_rtl_spi_dev_type_hw( rtl_spi_dev_t* pDev, uint32 spiCS_ );

// SPI raw (1 phase) read/write 
//extern int32 _rtl_spi_rawRead( rtl_spi_dev_t* pDev, void* pData, int32 bits );
//extern int32 _rtl_spi_rawWrite( rtl_spi_dev_t* pDev, void* pData, int32 bits );

#ifndef SUCCESS
#define SUCCESS 	0
#endif
#ifndef FAILED
#define FAILED 		-1
#endif

#endif //_SPI_H_
