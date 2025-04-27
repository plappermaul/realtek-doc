/*
 * Copyright (C) 2009-2015 Realtek Semiconductor Corp.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * $Revision: 63714 $
 * $Date: 2015-11-30 10:01:17 +0800 (Mon, 30 Nov 2015) $
 *
 */

#ifndef _RTK_FLASH_SPI_H
#define _RTK_FLASH_SPI_H


#define SPI_VENDOR_MASK     0xFFFF0000
#define SPI_VENDOR_SPANSION 0x00010000
#define SPI_VENDOR_MXIC     0x00C20000
#define SPI_VENDOR_SST      0x00BF0000
#define SPI_VENDOR_WINDBOND 0x00EF0000
#define SPI_VENDOR_DEVICETYPE_MASK     0x00FFFF00
#define SPI_VENDOR_SST_QIO       0x00BF2600
#define SPI_VENDOR_WINDBOND_QIO  0x00EF4000
#define SPI_VENDOR_MXIC_QIO      0x00C25E00
#define SPI_VENDOR_SPANSION_QIO  0x00012000

/*   List of supported single I/O chip    */
/*  Spanson Flash  */
#define S25FL004A 0x00010212
#define S25FL016A 0x00010214
#define S25FL064A 0x00010216  /*supposed support*/
#define S25FL128P 0x00012018  /*only S25FL128P0XMFI001, Uniform  64KB secotr*/
                       /*not support S25FL128P0XMFI011, Uniform 256KB secotr*/
                       /*because #define SPI_BLOCK_SIZE 65536  */
#define S25FL256S 0x00010219

/*Micron*/
#define MT25QL512AB 0x0020BA20

/*  MXIC Flash  */
#define MX25L4005   0x00C22013
#define MX25L1605D  0x00C22015
#define MX25L3205D  0x00C22016  /*supposed support*/
//#define MX25L6405D  0x00C22017 /* phase out*/
//#define MX25L12805D 0x00C22018 /* phase out*/
#define MX25L8035EM 0x00C22014
#define MX25L6406E  (0x00C22017)
#define MX25L3233F  0x00C22016  /*supposed support*/
#define MX25L51245G 0x00C2201A

/*  SST Flash  */
#define SST25VF032B 0x00BF254A

/*   List of supported Multi I/O chip    */
/*  Spanson Flash   */
/*  MXIC Flash      */
#define MX25L1635D  0x00C22415  /*supposed support*/
#define MX25L3235D  0x00C25E16  /*supposed support*/
#define MX25L12845E 0x00C22018  /*supposed support*/
#define MX25L25635F (0x00C22019)
#define MX25L25635E  (0x00C22019)
#define MX25L25635EF (0x00C22019)
#define MX25L25635E_REMS2 (0xC218)
/*  SST Flash       */
#define SST26VF016  0x00BF2601
#define SST26VF032  0x00BF2602
/*  WindBond Flash  */
#define W25Q80      0x00EF4014
#define W25Q16      0x00EF4015
#define W25Q32      0x00EF4016
#define W25Q128     0x00EF4018
#define W25Q256FV     0x00ef4019
#define W25Q512JV     0x00ef4020
#define W25Q64JV    0x00ef4017
/*  Numonyx  */
#define M25P128     0x00202018
/* Spansion Flash*/
#define S25FL032P   0x00010215

/*GD SPI flash*/
#define GD25Q256C   0x00c84019
#define GD25Q64C    0x00c84017
#define GD25B512M   0x00c8471A


#ifndef __ASSEMBLY__
typedef unsigned int    uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;
typedef int int32;
#endif /*__ASSEMBLY__*/



#define SPI_REG(reg) *((volatile uint32 *)(reg))
#define SPI_BLOCK_SIZE 65536  /* 64KB */
#define SPI_SECTOR_SIZE 4096  /*  4KB */
#define ENABLE_SPI_FLASH_FORMAL_READ


//#ifdef CONFIG_RTL8328
    #define SFRB   0xB8001200       /*SPI Flash Register Base*/
    #define SFCR   (SFRB)           /*SPI Flash Configuration Register*/
        #define SFCR_CLK_DIV(val)   ((val)<<29)
        #define SFCR_EnableRBO      (1<<28)
        #define SFCR_EnableWBO      (1<<27)
        #define SFCR_SPI_TCS(val)   ((val)<<23) /*4 bit, 1111 */
    #define SFCR2  (SFRB+0x04)      /*For memory mapped I/O */
        #define SFCR2_SFCMD(val)    ((val)<<24) /*8 bit, 1111_1111 */
        #define SFCR2_SIZE(val)     ((val)<<21) /*3 bit, 111 */
        #define SFCR2_RDOPT         (1<<20)
        #define SFCR2_CMDIO(val)    ((val)<<18) /*2 bit, 11 */
        #define SFCR2_ADDRIO(val)   ((val)<<16) /*2 bit, 11 */
        #define SFCR2_DUMMYCYCLE(val)   ((val)<<13) /*3 bit, 111 */
        #define SFCR2_DATAIO(val)   ((val)<<11) /*2 bit, 11 */
        #define SFCR2_HOLD_TILL_SFDR2  (1<<10)
        #define SFCR2_GETSIZE(x)    (((x)&0x00E00000)>>21)



    #define SFCSR  (SFRB+0x08)   /*SPI Flash Control&Status Register*/
        #define SFCSR_SPI_CSB0      (1<<31)
        #define SFCSR_SPI_CSB1      (1<<30)
        #define SFCSR_LEN(val)      ((val)<<28)  /*2 bits*/
        #define SFCSR_SPI_RDY       (1<<27)
        #define SFCSR_IO_WIDTH(val) ((val)<<25)  /*2 bits*/
        #define SFCSR_CHIP_SEL      (1<<24)
        #define SFCSR_CMD_BYTE(val) ((val)<<16)  /*8 bit, 1111_1111 */

    #define SFDR   (SFRB+0x0C) /*SPI Flash Data Register*/
    #define SFDR2  (SFRB+0x10) /*SPI Flash Data Register - for post SPI bootup setting*/

    //#define SPI_CS_INIT    (SFCSR_SPI_CSB0 | SFCSR_SPI_CSB1 | SPI_LEN1 | SFCSR_SPI_RDY)
    #define SPI_CS_INIT    (SFCSR_SPI_CSB0 | SFCSR_SPI_CSB1 | SPI_LEN1)
    #define SPI_CS0    SFCSR_SPI_CSB0
    #define SPI_CS1    SFCSR_SPI_CSB1
    //#define SPI_eCS0  ((SFCSR_SPI_CSB1) | SFCSR_SPI_RDY) /*and SFCSR to active CS0*/
    //#define SPI_eCS1  ((SFCSR_SPI_CSB0) | SFCSR_SPI_RDY) /*and SFCSR to active CS1*/
    #define SPI_eCS0  ((SFCSR_SPI_CSB1)) /*and SFCSR to active CS0*/
    #define SPI_eCS1  ((SFCSR_SPI_CSB0)) /*and SFCSR to active CS1*/

    #define SPI_WIP (1)   /* Write In Progress */
    #define SPI_WEL (1<<1)   /* Write Enable Latch*/
    #define SPI_SST_QIO_WIP (1<<7)   /* SST QIO Flash Write In Progress */
    #define SPI_LEN_INIT 0xCFFFFFFF /* and SFCSR to init   */
    #define SPI_LEN4    0x30000000     /* or SFCSR to set */
    #define SPI_LEN3    0x20000000     /* or SFCSR to set */
    #define SPI_LEN2    0x10000000     /* or SFCSR to set */
    #define SPI_LEN1    0x00000000     /* or SFCSR to set */
    #define SPI_SETLEN(val) do {        \
        SPI_REG(SFCSR) &= 0xCFFFFFFF;   \
        SPI_REG(SFCSR) |= (val-1)<<28;  \
        }while(0)
//#endif /*CONFIG_RTL8328*/

#define SPI_MAX_TRANSFER_SIZE 256

#define CHECK_READY while( !(SPI_REG(SFCSR)&SFCSR_SPI_RDY) );

#define FLASHBASE CFG_FLASH_BASE
#define MAX_SPI_FLASH_CHIPS 2

#define ENABLE_SPI_FLASH_READ
#define ENABLE_SPI_FLASH_PIO_READ

#ifndef CONFIG_SPI_WBO
    #define CONFIG_SPI_WBO 1
#endif

#ifndef CONFIG_SPI_RBO
    #define CONFIG_SPI_RBO 1
#endif

#ifndef __ASSEMBLY__
typedef enum{
/*  RBO=0x01,
    WBO=0x02, */
    R_MODE=0x04,
    CS0=(1<<4), /*CS: bit4 ~ bit7*/
    CS1=(1<<5),
    CS2=(1<<6),
    CS3=(1<<7),
}spi_flags;

typedef enum{
    IO1=(1<<0), /*IO#: chip supports IO width*/
    IO2=(1<<1),
    IO4=(1<<2),
    CIO1=(1<<3),/*IO#: current IO width mode*/
    CIO2=(1<<4),
    CIO4=(1<<5),
    CMD_IO1=((1)<<6),
    CMD_IO2=((2)<<6),
    CMD_IO4=((3)<<6),
    CMD_IO_Reserved=(1<<8),
    R_ADDR_IO1=((1)<<9),
    R_ADDR_IO2=((2)<<9),
    R_ADDR_IO4=((3)<<9),
    R_ADDR_IO_Reserved=(1<<11),
    W_ADDR_IO1=((1)<<12),
    W_ADDR_IO2=((2)<<12),
    W_ADDR_IO4=((3)<<12),
    W_ADDR_IO_Reserved=(1<<14),
    R_DATA_IO1=((1)<<15),
    R_DATA_IO2=((2)<<15),
    R_DATA_IO4=((3)<<15),
    R_DATA_IO_Reserved=(1<<17),
    W_DATA_IO1=((1)<<18),
    W_DATA_IO2=((2)<<18),
    W_DATA_IO4=((3)<<18),
    CADDR_4BYTES=(1<<20),
    HAVE_EQ_CMD=(1<<29),
    QE_BIT=(1<<30),
    MODE_EN=(1<<31)    //enhance mode enable
}spi_iostatus;
#endif /*__ASSEMBLY__*/


#define IOSTATUS_CIO_8xMASK (IO4 | CMD_IO4 | R_ADDR_IO4 | W_ADDR_IO4 | R_DATA_IO4 | W_DATA_IO4 | QE_BIT)
#define IOSTATUS_CIO_8xDaul (IO2 | CMD_IO1 | R_ADDR_IO2 | W_ADDR_IO1 | R_DATA_IO2 | W_DATA_IO1)
#define IOSTATUS_CIO_MASK (0x00000038)
#define CMD_IO_MASK CMD_IO4
#define CMD_R_ADDR_MASK R_ADDR_IO4
#define CMD_W_ADDR_MASK W_ADDR_IO4
#define CMD_R_DATA_MASK R_DATA_IO4
#define CMD_W_DATA_MASK W_DATA_IO4
#define GET_CMD_IO(val)    (((val)&CMD_IO_MASK)>>6)
#define GET_R_ADDR_IO(val) (((val)&CMD_R_ADDR_MASK)>>9)
#define GET_W_ADDR_IO(val) (((val)&CMD_W_ADDR_MASK)>>12)
#define GET_R_DATA_IO(val)   (((val)&CMD_R_DATA_MASK)>>15)
#define GET_W_DATA_IO(val)   (((val)&CMD_W_DATA_MASK)>>18)

#define SST_WRITE_UNLOCK 0
#define SST_WRITE_LOCK 1

#ifndef __ASSEMBLY__
typedef struct spi_chip_info_s{
    uint32  chip_id;
    uint32  chip_size;
    uint32  io_status;
    uint8   *chip_name;
    uint8   dio_read;     //dio read command
    uint8   dio_mode;     //enhance mode format
    uint8   dio_read_dummy;//dummy bytes(dummy_cycle/2)

    uint8   qio_read;     //qio read command
    uint8   qio_mode;     //enhance mode format
    uint8   qio_read_dummy; //dummy bytes(dummy_cycle/2)
    uint8   qio_pp;       //page program command
    uint8   qio_eq;       //enter quad mode command
    uint8   qio_eq_dummy; //enter quad mode dummy
    uint8   qio_es;       //enter serial mode command

    uint8   qio_wqe_cmd;  //write quad enable bit command
    uint8   qio_qeb_loc;  //quad enable bit location
    uint8   qio_status_len; // bytes of status register which owns quad enable bit
}spi_chip_info_t;
#endif /*__ASSEMBLY__*/

#ifndef __ASSEMBLY__
typedef enum{
    SPI_C_QPP = 0xfa,    /*pseudo command, Quad Page Program*/
    SPI_C_MREAD = 0xfb,  /*pseudo command, Multi-IO Read*/
    SPI_C_EMIO = 0xfd,   /*pseudo command, enter Multi-IO Mode*/
    SPI_C_RSTQIO = 0xFF,  /*SST Reset Quad IO*/
    SPI_C_READ = 0x03,
    SPI_C_FREAD = 0x0B,
    SPI_C_FREAD4B = 0x0C,
    SPI_C_RDID = 0x9F,
    SPI_C_REMS2 = 0xEF,
    SPI_C_4RDID = 0xAF,  /*SST Quad ReadID*/
    SPI_C_WREN = 0x06,
    SPI_C_WRDI = 0x04,
    SPI_C_BE = 0xD8,
    SPI_C_BE4B = 0xDC,
    SPI_C_CE = 0xC7,
    SPI_C_SE = 0x20,
    SPI_C_SE4B = 0x21,
    SPI_C_PP = 0x02,
    SPI_C_PP4B = 0x12,
    SPI_C_RDSR = 0x05,
    SPI_C_WRSR = 0x01,
    SPI_C_WBPR = 0x42,  /*SST Write Block-Protection Register*/
    SPI_C_DP = 0xB9,
    SPI_C_EN4B = 0xB7,
    SPI_C_EX4B = 0xE9,
    SPI_C_RES = 0xAB,
    SPI_C_AAI = 0xAD   /*Auto Address Increment*/
}spi_cmdType_t;


typedef struct rtk_spi_data{
    const char* Type;
    uint32 flags;    /*Chip Select*/
    uint32 ChipSize;    /* total size of this flash chip, ex: 0x100000 for 1MB ... */
    uint32 BlockBase;   /* the start address of this block, ex: 0xbfc00000, 0xbfe00000 ... */
    uint32 NumOfBlock;  /* number of sector */
}spi_dev_t;

typedef struct spi_request_s{
    spi_cmdType_t cmd_t;
    uint32 address;
    uint8   *buf;               /*request buffer*/
    uint32 size;                /*request IO size*/
}spi_request_t;
#endif /*__ASSEMBLY__*/

#ifdef __UBOOT__
#define SPI_SHOW_PROGRESS 1
#else
#include <linux/mtd/rtk_flash_common.h>


#define CONFIG_MTD_RTK_SPI 1

#undef FLASHBASE
#define FLASHBASE FLASH_BASE
typedef struct RTK_SPI_MTD_s{
    uint32 BlockBase;   /* the start address of this block, ex: 0xbfc00000, 0xbfe00000 ... */
    uint32 flags;    /*Chip Select*/
    ulong flash_id;
#if defined(CONFIG_MTD_RTK_SPI)
    ulong   io_status;
    uint8   dio_read;     //dio read command
    uint8   dio_mode;     //enhance mode format
    uint8   dio_read_dummy;//dummy bytes(dummy_cycle/2)
//  uint8   dio_pp;       //all pp do not need pp_dummy on 2008.06.25
//  uint8   dio_pp_dummy; //all pp do not need pp_dummy on 2008.06.25

    uint8   qio_read;     //qio read command
    uint8   qio_mode;     //enhance mode format
    uint8   qio_read_dummy; //dummy bytes(dummy_cycle/2)
    uint8   qio_pp;       //page program command
//  uint8   qio_pp_dummy; //all pp do not need pp_dummy on 2008.06.25

    uint8   qio_eq;       //enter quad mode command
    uint8   qio_eq_dummy; //enter quad mode dummy
    uint8   qio_es;       //enter serial mode command
    uint8   qio_wqe_cmd;  //write quad enable bit command
    uint8   qio_qeb_loc;  //quad enable bit location
    uint8   qio_status_len; // bytes of status register which owns quad enable bit
#endif /*CONFIG_FLASH_SPI*/
    struct semaphore    lock;
    struct mtd_info     mtd;
}RTK_SPI_MTD;
#endif /*!__UBOOT__*/

/* Function Name:
 *      drv_swcore_cid_get
 * Description:
 *      Get chip id and chip revision id.
 * Input:
 *      unit           - unit id
 * Output:
 *      pChip_id       - pointer buffer of chip id
 *      pChip_rev_id   - pointer buffer of chip revision id
 * Return:
 *      RT_ERR_OK      - OK
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      None
 */
extern int32
drv_swcore_cid_get(uint32 unit, uint32 *pChip_id, uint32 *pChip_rev_id);

/* Function Name:
 *      drv_swcore_chipFamilyId_get
 * Description:
 *      Get chip family id
 * Input:
 *      chip_id         - chip id (e.g. RTL9301_CHIP_ID,...)
 * Output:
 *      None
 * Return:
 *      chip family ID
 */
extern uint32
drv_swcore_chipFamilyId_get(uint32 chip_id);

#endif /*_FLASH_SPI_H*/

