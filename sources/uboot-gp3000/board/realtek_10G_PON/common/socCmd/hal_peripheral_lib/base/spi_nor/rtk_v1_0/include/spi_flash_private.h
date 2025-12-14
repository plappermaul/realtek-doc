/* --------------------------------------------------------------------
**
**
** File     : flash_private.h
** Created  : $Date: 2008/10/30 $
** Abstract : This is the private header file for the flash
**            low-level-driver
**
** --------------------------------------------------------------------
*/
#define on_RLX5181

#ifndef on_RLX5181
#define on_TAROKO
#endif

#ifndef SPI_FLASH_PRIVATE_H
#define SPI_FLASH_PRIVATE_H

/***********************************************************
 * Indicate using which vender to define device information
***********************************************************/
#ifndef  SPI_FLASH_VENDER_H
#define  SPI_FLASH_VENDER_H
//#define VENDER_ATMEL
#define VENDER_MXIC 
//#define VENDER_NUMONYX
//#define VENDER_SPANSION
//#define VENDER_SST
//#define VENDER_WINBOND
#endif //SPI_FLASH_VENDER_H


#include "device_info.h"
#include <linux/types.h>
#include "RTL8195A_spic_conf.h"
//#include "spi_flash/rle0591_defs.h"




/* Peripheral Subsystem SPIC */
//#define BSP_PS_I_SPIC_BASE            0xBFB09000
//#define BSP_PS_I_SPIC_BASE            0xf8143000 //For 8198E ARM
//#define FLASH_BASE 0xe0000000  //For 8198E ARM

#define BSP_PS_I_SPIC_BASE            0xB8143000 //For 8196F MIPS24K
#define FLASH_BASE 0xB0000000  //For 8196F MIPS24K






//#define FLASH_SIZE 0x00200000
#define FLASH_SIZE 0x00200
#define DATA_CH    2
#define ADDR_CH    0
//#define FLASH_SIZE 0x00000008

#define SPIC_SSIENR 0x8
#define SPIC_SER 0x10
#define SPIC_FLASH_SIZE 0x124

//*********  sdr_init_test define *********
//#define SPIC_BASE   0xBFB09000
#if 0
#define SPIC_BASE   0xf8143000
#define GOLD_ADDR   0xe0000000
#define GOLD_VALUE  0XFFFFFFFF
#endif

#define MAX_BAUDR   0x4  // >=1
#define MAX_DUMMY   0x4
#define SPIC_FRQ    25
//*****************************************

/* Register Macro */
#ifndef REG32
#define REG32(reg)      (*(volatile u32 *)(reg))
#endif
#ifndef REG16
#define REG16(reg)      (*(volatile u16 *)(reg))
#endif
#ifndef REG8
#define REG8(reg)       (*(volatile u8  *)(reg))
#endif




//*********  auto mode control register bit define (0x120/ VALID_CMD) *********
#define EN_CHECK_SPI_FLASH_STATUS_FUNCTION (1<<10)
#define WR_BLOCKING (1<<9)

#define EN_WR_QUAD_II (1<<8)
#define EN_WR_QUAD_I (1<<7)
#define EN_WR_DUAL_II (1<<6)
#define EN_WR_DUAL_I (1<<5)

#define EN_RD_QUAD_IO (1<<4)
#define EN_RD_QUAD_O (1<<3)
#define EN_RD_DUAL_IO (1<<2)
#define EN_RD_DUAL_I  (1<<1)

#define EN_FRD_SINGLE  (1<<0)


//*****************************************



/***** spi_flash.api/spi_flash.bfws
 * DESCRIPTION
 *  Used in conjunction with bitops.h to access register bitfields.
 *  They are defined as bit offset/mask pairs for each DMA register
 *  bitfield.
 * EXAMPLE
 *  int_status = BIT_GET(INP(portmap->status_int_l),
 *               SPI_FLASH_STATUSINT_L_DSTTRAN);
 * NOTES
 *  bfo is the offset of the bitfield with respect to LSB;
 *  bfw is the width of the bitfield
 * SEE ALSO
 *  dw_common_bitops.h
 * SOURCE
 */
#define bfoSPI_FLASH_CTRLR0_FAST_RD         ((uint32_t)   20)
#define bfwSPI_FLASH_CTRLR0_FAST_RD         ((uint32_t)    1)
#define bfoSPI_FLASH_CTRLR0_DATA_CH         ((uint32_t)   18)
#define bfwSPI_FLASH_CTRLR0_DATA_CH         ((uint32_t)    2)
#define bfoSPI_FLASH_CTRLR0_ADDR_CH         ((uint32_t)   16)
#define bfwSPI_FLASH_CTRLR0_ADDR_CH         ((uint32_t)    2)
#define bfoSPI_FLASH_CTRLR0_CFS             ((uint32_t)   12)
#define bfwSPI_FLASH_CTRLR0_CFS             ((uint32_t)    3)
#define bfoSPI_FLASH_CTRLR0_SRL             ((uint32_t)   11)
#define bfwSPI_FLASH_CTRLR0_SRL             ((uint32_t)    1)
#define bfoSPI_FLASH_CTRLR0_SLV_OE          ((uint32_t)   10)
#define bfwSPI_FLASH_CTRLR0_SLV_OE          ((uint32_t)    1)
#define bfoSPI_FLASH_CTRLR0_TMOD            ((uint32_t)    8)
#define bfwSPI_FLASH_CTRLR0_TMOD            ((uint32_t)    2)
#define bfoSPI_FLASH_CTRLR0_SCPOL           ((uint32_t)    7)
#define bfwSPI_FLASH_CTRLR0_SCPOL           ((uint32_t)    1)
#define bfoSPI_FLASH_CTRLR0_FRF             ((uint32_t)    4)
#define bfwSPI_FLASH_CTRLR0_FRF             ((uint32_t)    2)
#define bfoSPI_FLASH_CTRLR0_DFS             ((uint32_t)    0)
#define bfwSPI_FLASH_CTRLR0_DFS             ((uint32_t)    4)

#define bfoSPI_FLASH_CTRLR1_NDF             ((uint32_t)    0)
#define bfwSPI_FLASH_CTRLR1_NDF             ((uint32_t)   16)

#define bfoSPI_FLASH_SSIENR_SSI_EN          ((uint32_t)    0)
#define bfwSPI_FLASH_SSIENR_SSI_EN          ((uint32_t)    1)

#define bfoSPI_FLASH_MWCR_MHS               ((uint32_t)    2)
#define bfwSPI_FLASH_MWCR_MHS               ((uint32_t)    1)
#define bfoSPI_FLASH_MWCR_MDD               ((uint32_t)    1)
#define bfwSPI_FLASH_MWCR_MDD               ((uint32_t)    1)
#define bfoSPI_FLASH_MWCR_MWMOD             ((uint32_t)    0)
#define bfwSPI_FLASH_MWCR_MWMOD             ((uint32_t)    1)

#define bfoSPI_FLASH_SER                    ((uint32_t)    0)
#define bfwSPI_FLASH_SER                    ((uint32_t)    4)

#define bfoSPI_FLASH_BAUDR_SCKDV            ((uint32_t)    0)
#define bfwSPI_FLASH_BAUDR_SCKDV            ((uint32_t)   16)

#define bfoSPI_FLASH_TXFTLR_TFT             ((uint32_t)    0)
#define bfwSPI_FLASH_TXFTLR_TFT             ((uint32_t)    3)

#define bfoSPI_FLASH_RXFTLR_RFT             ((uint32_t)    0)
#define bfwSPI_FLASH_RXFTLR_RFT             ((uint32_t)    3)

#define bfoSPI_FLASH_TXFLR_TXTFL            ((uint32_t)    0)
#define bfwSPI_FLASH_TXFLR_TXTFL            ((uint32_t)    3)

#define bfoSPI_FLASH_RXFLR_RXTFL            ((uint32_t)    0)
#define bfwSPI_FLASH_RXFLR_RXTFL            ((uint32_t)    3)

#define bfoSPI_FLASH_SR_BUSY                ((uint32_t)    0)
#define bfwSPI_FLASH_SR_BUSY                ((uint32_t)    1)
#define bfoSPI_FLASH_SR_TFNF                ((uint32_t)    1)
#define bfwSPI_FLASH_SR_TFNF                ((uint32_t)    1)
#define bfoSPI_FLASH_SR_TFE                 ((uint32_t)    2)
#define bfwSPI_FLASH_SR_TFE                 ((uint32_t)    1)
#define bfoSPI_FLASH_SR_RFNE                ((uint32_t)    3)
#define bfwSPI_FLASH_SR_RFNE                ((uint32_t)    1)
#define bfoSPI_FLASH_SR_RFF                 ((uint32_t)    4)
#define bfwSPI_FLASH_SR_RFF                 ((uint32_t)    1)
#define bfoSPI_FLASH_SR_TXE                 ((uint32_t)    5)
#define bfwSPI_FLASH_SR_TXE                 ((uint32_t)    1)
#define bfoSPI_FLASH_SR_DCOL                ((uint32_t)    6)
#define bfwSPI_FLASH_SR_DCOL                ((uint32_t)    1)

#define bfoSPI_FLASH_IMR_TXEIM              ((uint32_t)     0)
#define bfwSPI_FLASH_IMR_TXEIM              ((uint32_t)     1)
#define bfoSPI_FLASH_IMR_TXOIM              ((uint32_t)     1)
#define bfwSPI_FLASH_IMR_TXOIM              ((uint32_t)     1)
#define bfoSPI_FLASH_IMR_RXUIM              ((uint32_t)     2)
#define bfwSPI_FLASH_IMR_RXUIM              ((uint32_t)     1)
#define bfoSPI_FLASH_IMR_RXOIM              ((uint32_t)     3)
#define bfwSPI_FLASH_IMR_RXOIM              ((uint32_t)     1)
#define bfoSPI_FLASH_IMR_RXFIM              ((uint32_t)     4)
#define bfwSPI_FLASH_IMR_RXFIM              ((uint32_t)     1)
#define bfoSPI_FLASH_IMR_FSEIM              ((uint32_t)     5)
#define bfwSPI_FLASH_IMR_FSEIM              ((uint32_t)     1)
#define bfoSPI_FLASH_IMR_WBEIM              ((uint32_t)     6)
#define bfwSPI_FLASH_IMR_WBEIM              ((uint32_t)     1)
#define bfoSPI_FLASH_IMR_BYEIM              ((uint32_t)     7)
#define bfwSPI_FLASH_IMR_BYEIM              ((uint32_t)     1)
#define bfoSPI_FLASH_IMR_ACTIM              ((uint32_t)     8)
#define bfwSPI_FLASH_IMR_ACTIM              ((uint32_t)     1)
#define bfoSPI_FLASH_IMR_TXEIM_PEND         ((uint32_t)     9)
#define bfwSPI_FLASH_IMR_TXEIM_PEND         ((uint32_t)     1)

#define bfoSPI_FLASH_ISR_TXEIS              ((uint32_t)     0)
#define bfwSPI_FLASH_ISR_TXEIS              ((uint32_t)     1)
#define bfoSPI_FLASH_ISR_TXOIS              ((uint32_t)     1)
#define bfwSPI_FLASH_ISR_TXOIS              ((uint32_t)     1)
#define bfoSPI_FLASH_ISR_RXUIS              ((uint32_t)     2)
#define bfwSPI_FLASH_ISR_RXUIS              ((uint32_t)     1)
#define bfoSPI_FLASH_ISR_RXOIS              ((uint32_t)     3)
#define bfwSPI_FLASH_ISR_RXOIS              ((uint32_t)     1)
#define bfoSPI_FLASH_ISR_RXFIS              ((uint32_t)     4)
#define bfwSPI_FLASH_ISR_RXFIS              ((uint32_t)     1)
#define bfoSPI_FLASH_ISR_FSEIS              ((uint32_t)     5)
#define bfwSPI_FLASH_ISR_FSEIS              ((uint32_t)     1)
#define bfoSPI_FLASH_ISR_WBEIS              ((uint32_t)     6)
#define bfwSPI_FLASH_ISR_WBEIS              ((uint32_t)     1)
#define bfoSPI_FLASH_ISR_BYEIS              ((uint32_t)     7)
#define bfwSPI_FLASH_ISR_BYEIS              ((uint32_t)     1)
#define bfoSPI_FLASH_ISR_ACTIS              ((uint32_t)     8)
#define bfwSPI_FLASH_ISR_ACTIS              ((uint32_t)     1)
#define bfoSPI_FLASH_ISR_TXEIS_PEND         ((uint32_t)     9)
#define bfwSPI_FLASH_ISR_TXEIS_PEND         ((uint32_t)     1)

#define bfoSPI_FLASH_RISR_TXEIR             ((uint32_t)     0)
#define bfwSPI_FLASH_RISR_TXEIR             ((uint32_t)     1)
#define bfoSPI_FLASH_RISR_TXOIR             ((uint32_t)     1)
#define bfwSPI_FLASH_RISR_TXOIR             ((uint32_t)     1)
#define bfoSPI_FLASH_RISR_RXUIR             ((uint32_t)     2)
#define bfwSPI_FLASH_RISR_RXUIR             ((uint32_t)     1)
#define bfoSPI_FLASH_RISR_RXOIR             ((uint32_t)     3)
#define bfwSPI_FLASH_RISR_RXOIR             ((uint32_t)     1)
#define bfoSPI_FLASH_RISR_RXFIR             ((uint32_t)     4)
#define bfwSPI_FLASH_RISR_RXFIR             ((uint32_t)     1)
#define bfoSPI_FLASH_RISR_FSEIR             ((uint32_t)     5)
#define bfwSPI_FLASH_RISR_FSEIR             ((uint32_t)     1)
#define bfoSPI_FLASH_RISR_WBEIR             ((uint32_t)     6)
#define bfwSPI_FLASH_RISR_WBEIR             ((uint32_t)     1)
#define bfoSPI_FLASH_RISR_BYEIR             ((uint32_t)     7)
#define bfwSPI_FLASH_RISR_BYEIR             ((uint32_t)     1)
#define bfoSPI_FLASH_RISR_ACTIR             ((uint32_t)     8)
#define bfwSPI_FLASH_RISR_ACTIR             ((uint32_t)     1)

#define bfoSPI_FLASH_TXOICR_TXOICR          ((uint32_t)     0)
#define bfwSPI_FLASH_TXOICR_TXOICR          ((uint32_t)     1)

#define bfoSPI_FLASH_RXOICR_RXOICR          ((uint32_t)     0)
#define bfwSPI_FLASH_RXOICR_RXOICR          ((uint32_t)     1)

#define bfoSPI_FLASH_RXUICR_RXUICR          ((uint32_t)     0)
#define bfwSPI_FLASH_RXUICR_RXUICR          ((uint32_t)     1)

#define bfoSPI_FLASH_MSTICR_MSTICR          ((uint32_t)     0)
#define bfwSPI_FLASH_MSTICR_MSTICR          ((uint32_t)     1)

#define bfoSPI_FLASH_ICR_ICR                ((uint32_t)     0)
#define bfwSPI_FLASH_ICR_ICR                ((uint32_t)     1)

#define bfoSPI_FLASH_DMACR_RDMAE            ((uint32_t)     0)
#define bfwSPI_FLASH_DMACR_RDMAE            ((uint32_t)     1)
#define bfoSPI_FLASH_DMACR_TDMAE            ((uint32_t)     1)
#define bfwSPI_FLASH_DMACR_TDMAE            ((uint32_t)     1)
                                       
#define bfoSPI_FLASH_DMATDLR_DMATDL         ((uint32_t)     0)
#define bfwSPI_FLASH_DMATDLR_DMATDL         ((uint32_t)     3)

#define bfoSPI_FLASH_DMARDLR_DMARDL         ((uint32_t)     0)
#define bfwSPI_FLASH_DMARDLR_DMARDL         ((uint32_t)     3)

#define bfoSPI_FLASH_DR0_dr0                ((uint32_t)     0)
#define bfwSPI_FLASH_DR0_dr0                ((uint32_t)    16)
#define bfoSPI_FLASH_DR1_dr1                ((uint32_t)     0)
#define bfwSPI_FLASH_DR1_dr1                ((uint32_t)    16)
#define bfoSPI_FLASH_DR2_dr2                ((uint32_t)     0)
#define bfwSPI_FLASH_DR2_dr2                ((uint32_t)    16)
#define bfoSPI_FLASH_DR3_dr3                ((uint32_t)     0)
#define bfwSPI_FLASH_DR3_dr3                ((uint32_t)    16)
#define bfoSPI_FLASH_DR4_dr4                ((uint32_t)     0)
#define bfwSPI_FLASH_DR4_dr4                ((uint32_t)    16)
#define bfoSPI_FLASH_DR5_dr5                ((uint32_t)     0)
#define bfwSPI_FLASH_DR5_dr5                ((uint32_t)    16)
#define bfoSPI_FLASH_DR6_dr6                ((uint32_t)     0)
#define bfwSPI_FLASH_DR6_dr6                ((uint32_t)    16)
#define bfoSPI_FLASH_DR7_dr7                ((uint32_t)     0)
#define bfwSPI_FLASH_DR7_dr7                ((uint32_t)    16)
#define bfoSPI_FLASH_DR8_dr8                ((uint32_t)     0)
#define bfwSPI_FLASH_DR8_dr8                ((uint32_t)    16)
#define bfoSPI_FLASH_DR9_dr9                ((uint32_t)     0)
#define bfwSPI_FLASH_DR9_dr9                ((uint32_t)    16)
#define bfoSPI_FLASH_DR10_dr10              ((uint32_t)     0)
#define bfwSPI_FLASH_DR10_dr10              ((uint32_t)    16)
#define bfoSPI_FLASH_DR11_dr11              ((uint32_t)     0)
#define bfwSPI_FLASH_DR11_dr11              ((uint32_t)    16)
#define bfoSPI_FLASH_DR12_dr12              ((uint32_t)     0)
#define bfwSPI_FLASH_DR12_dr12              ((uint32_t)    16)
#define bfoSPI_FLASH_DR13_dr13              ((uint32_t)     0)
#define bfwSPI_FLASH_DR13_dr13              ((uint32_t)    16)
#define bfoSPI_FLASH_DR14_dr14              ((uint32_t)     0)
#define bfwSPI_FLASH_DR14_dr14              ((uint32_t)    16)
#define bfoSPI_FLASH_DR15_dr15              ((uint32_t)     0)
#define bfwSPI_FLASH_DR15_dr15              ((uint32_t)    16)

#define bfoSPI_FLASH_AUTO_LEN_ADDR          ((uint32_t)    16)
#define bfwSPI_FLASH_AUTO_LEN_ADDR          ((uint32_t)     2)
#define bfoSPI_FLASH_AUTO_LEN_DUM           ((uint32_t)     0)
#define bfwSPI_FLASH_AUTO_LEN_DUM           ((uint32_t)    16)


/***** SPI_Flash macros/DW_CC_DEFINE_SPI_FLASH_PARAMS
 * DESCRIPTION
 *  This macro is used to initialize a spi_flash_param structure.  To use
 *  this macro, the relevant C header file must also be included.  This
 *  is generated when a DesignWare device is synthesized.
 * NOTES
 *  The relevant uart coreConsultant C header must be included before
 *  this macro can be used.
 * SOURCE
 */
#define CC_DEFINE_SPI_FLASH_PARAMS(prefix) { \
    prefix ## CC_SPI_FLASH_NUM_SLAVES            ,\
    prefix ## CC_SPI_FLASH_TX_FIFO_DEPTH         ,\
    prefix ## CC_SPI_FLASH_RX_FIFO_DEPTH         ,\
    prefix ## CC_SPI_FLASH_ID                    ,\
    prefix ## CC_SPI_FLASH_DFLT_SCPOL            ,\
    prefix ## CC_SPI_FLASH_DFLT_SCPH             ,\
    prefix ## CC_SPI_FLASH_CLK_PERIOD            ,\
    prefix ## CC_SPI_FLASH_VERSION_ID            \
}

#define CC_DEFINE_FLASH_CMD(prefix) { \
    prefix ## _WRSR                 ,\
    prefix ## _PP                   ,\
    prefix ## _PPX2                 ,\
    prefix ## _PPX4                 ,\
    prefix ## _READ                 ,\
    prefix ## _READX2               ,\
    prefix ## _READX4               ,\
    prefix ## _WRDI                 ,\
    prefix ## _RDSR                 ,\
    prefix ## _WREN                 ,\
    prefix ## _FAST_READ            ,\
    prefix ## _FAST_READX2          ,\
    prefix ## _FAST_READX4          ,\
    prefix ## _RDID                 ,\
    prefix ## _DP                   ,\
    prefix ## _BE                   ,\
    prefix ## _SE                   \
}

//***** flash.data/flash_ID     
// * DESCRIPTION
// *  This data type is used to describe the manufacturer identifiation of Flash 
// */
struct flash_info {
  uint8_t flash_id;
  char    vender_name[16];
  struct  flash_cmd *cmd;
};

/***** flash.data/flash_rd_multi_type
 * DESCRIPTION
 *  This data type is used to describe read type with multi_channel
 * SEE ALSO
 *  flash.data
 */
enum flash_rd_multi_type {
      RD_MULTI_NONE = 0x00,
      RD_DUAL_O     = 0x01,
      RD_DUAL_IO    = 0x02,
      RD_QUAD_O     = 0x03,
      RD_QUAD_IO    = 0x04
};


/***** flash.data/flash_wr_multi_type
 * DESCRIPTION
 *  This data type is used to describe write type with multi_channel
 * SEE ALSO
 *  flash.data
 */
enum flash_wr_multi_type {
      WR_MULTI_NONE = 0x00,
      WR_DUAL_I     = 0x01,
      WR_DUAL_II    = 0x02,
      WR_QUAD_I     = 0x03,
      WR_QUAD_II    = 0x04
};


struct device_info {
    uint32_t  device_id;
    struct    flash_cmd    *cmd;
    struct    command_info *cmd_info;
};


/*command set in flash_vender.h*/
struct   flash_cmd {
  volatile uint8_t  wrsr       ;    // flash_cmd; write status  
  volatile uint8_t  pp         ;    // flash_cmd; write 
  volatile uint8_t  pp_4b    ;   // flash_cmd; 4 byte address write
  volatile uint8_t  ppx2_i     ;    // flash_cmd; write dual channels
  volatile uint8_t  ppx2_ii    ;    // flash_cmd; write dual channels
  volatile uint8_t  ppx4_i     ;    // flash_cmd; wirte quad channels
  volatile uint8_t  ppx4_ii    ;    // flash_cmd; wirte quad channels
  volatile uint8_t  read_4b    ;    //flash_cmd; 4 byte address read 
  volatile uint8_t  readx2_o   ;    // flash_cmd; read  dual channels
  volatile uint8_t  readx2_io  ;    // flash_cmd; read  dual channels
  volatile uint8_t  readx4_o   ;    // flash_cmd; read  quad channels
  volatile uint8_t  readx4_io  ;    // flash_cmd; read  quad channels
  volatile uint8_t  wrdi       ;    // flash_cmd; write disable
  volatile uint8_t  rdsr       ;    // flash_cmd; read status 
  volatile uint8_t  wren       ;    // flash_cmd; wren enable 
  volatile uint8_t  fast_read  ;    // flash_cmd; fast read
  volatile uint8_t  rdid       ;    // flash_cmd; read ID
  volatile uint8_t  ce         ;    // flash_cmd; chip erase
  volatile uint8_t  se         ;    // flash_cmd; sector erase
} ;

struct command_info {
    enum flash_rd_multi_type rd_dual_type; 
    enum flash_rd_multi_type rd_quad_type; 
    enum flash_wr_multi_type wr_dual_type; 
    enum flash_wr_multi_type wr_quad_type; 
    uint32_t  rd_dual_dummy;
    uint32_t  rd_quad_dummy;
    uint32_t  fast_rd_dummy;
    uint32_t  tunning_dummy; 
    uint32_t  wr_block_bound;
};


struct spi_flash_param {
    uint32_t spi_flash_num_slaves ;    // slaves number           
    uint32_t spi_flash_tx_fifo_depth;  // TX fifo depth number
    uint32_t spi_flash_rx_fifo_depth;  // TX fifo depth number
    uint32_t spi_flash_idr        ;    // ID code 
    uint32_t spi_flash_scpol      ;    // Serial clock polarity     
    uint32_t spi_flash_scph       ;    // Serial clock phase        
    uint32_t spi_flash_clk_period ;    // serial clock period
    uint32_t spi_flash_version_id ;    // spi flash ID
   };


/***** spi_flash.api/portmap
 * DESCRIPTION
 *  This is the structure used for accessing the spi_flash register
 *  portmap.
 * EXAMPLE
 *  struct spi_flash_portmap *portmap;
 *  portmap = (struct spi_flash_portmap *) spi_flash_BASE;
 *  foo = INP(portmap->ctrlr0 );
 * SOURCE
 */
struct spi_flash_portmap {
//typedef uint32_t unsigned int;

  /* Channel registers                                    */
  /* The offset address for each of the channel registers */
  /*  is shown for channel 0. For other channel numbers   */
  /*  use the following equation.                         */
  /*                                                      */
  /*    offset = (channel_num * 0x058) + channel_0 offset */
  /*                                                      */
  struct {
      volatile uint32_t ctrlr0;    /* Control Reg 0           (0x000) */
      volatile uint32_t ctrlr1;
      volatile uint32_t ssienr;    /* SPIC enable Reg1        (0x008) */
      volatile uint32_t mwcr; 
      volatile uint32_t ser;       /* Slave enable Reg        (0x010) */
      volatile uint32_t baudr;  
      volatile uint32_t txftlr;    /* TX_FIFO threshold level (0x018) */
      volatile uint32_t rxftlr;
      volatile uint32_t txflr;     /* TX_FIFO threshold level (0x020) */
      volatile uint32_t rxflr;  
      volatile uint32_t sr;        /* Destination Status Reg  (0x028) */
      volatile uint32_t imr;       
      volatile uint32_t isr;       /* Interrupt Stauts Reg    (0x030) */
      volatile uint32_t risr;      
      volatile uint32_t txoicr;    /* TX_FIFO overflow_INT clear (0x038) */
      volatile uint32_t rxoicr;    
      volatile uint32_t rxuicr;    /* RX_FIFO underflow_INT clear (0x040) */
      volatile uint32_t msticr;
      volatile uint32_t icr;       /* Interrupt clear Reg     (0x048) */ 
      volatile uint32_t dmacr;    
      volatile uint32_t dmatdlr;   /* DMA TX_data level       (0x050) */
      volatile uint32_t dmardlr;  
      volatile uint32_t idr;       /* Identiation Scatter Reg (0x058) */
      volatile uint32_t spi_flash_version;   
      //volatile uint32_t dr[32];    /* Data Reg          (0x060~0xdc)*/
      union{
           volatile uint8_t  byte;
           volatile uint16_t half;
           volatile uint32_t word;
      } dr[32];
      volatile uint32_t rd_fast_single; 
      volatile uint32_t rd_dual_o; /* Read dual data cmd Reg  (0x0e4) */
      volatile uint32_t rd_dual_io; 
      volatile uint32_t rd_quad_o; /* Read quad data cnd Reg  (0x0ec) */
      volatile uint32_t rd_quad_io;     
      volatile uint32_t wr_single; /* write single cmd Reg    (0x0f4) */
      volatile uint32_t wr_dual_i; 
      volatile uint32_t wr_dual_ii;/* write dual addr/data cmd(0x0fc) */
      volatile uint32_t wr_quad_i; 
      volatile uint32_t wr_quad_ii;/* write quad addr/data cnd(0x104) */
      volatile uint32_t wr_enable; 
      volatile uint32_t rd_status; /* read status cmd Reg     (0x10c) */
      volatile uint32_t ctrlr2;    
      volatile uint32_t fbaudr;    /* fast baud rate Reg      (0x114) */
      volatile uint32_t addr_length;
      volatile uint32_t auto_length; /* Auto addr length Reg  (0x11c) */
      volatile uint32_t valid_cmd;
      volatile uint32_t flash_size; /* Flash size Reg         (0x124) */
      volatile uint32_t flush_fifo; 
  };

  
};
/*****/


#endif /* SPI_FLASH_PRIVATE_H */

