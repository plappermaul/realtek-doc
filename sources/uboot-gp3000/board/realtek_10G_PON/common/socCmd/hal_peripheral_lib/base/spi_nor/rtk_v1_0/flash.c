/*
 * Realtek Semiconductor Corp.
 *
 */

#include <common.h>

//Add for RXI-310 SPI_Flash_Controller
#include <malloc.h>
//#include "SPIC_deliver/spi_flash/spi_flash_rtk.h"
#include "include/DW_common.h"
#include "include/spi_flash_public.h"
#include "include/spi_flash_private.h"
#include "include/snor_flash.h"

/* FLASH chips info */
flash_info_t flash_info[CONFIG_SYS_MAX_FLASH_BANKS];


#define CONFIG_SPI_STD_MODE 1


#define MTD_SPI_DEBUG 2
//#include <linux/autoconf2.h>
//#include "spi_common.h"

#ifndef SPI_KERNEL
// ****** spi flash driver in bootcode

//#include <asm/rtl8196x.h>
//#include <rtl_types.h>
#if (MTD_SPI_DEBUG == 0)
//0
#define NDEBUG(args...) printf(args)
#define KDEBUG(args...) printf(args)
#define LDEBUG(args...) printf(args)
#endif
//1
#if (MTD_SPI_DEBUG == 1)
#define NDEBUG(args...) printf(args)
#define KDEBUG(args...) printf(args)
#define LDEBUG(args...)
#endif
//2
#if (MTD_SPI_DEBUG == 2)
#define NDEBUG(args...) printf(args)
#define KDEBUG(args...)
#define LDEBUG(args...)
#endif
//3
#if (MTD_SPI_DEBUG == 3)
#define NDEBUG(args...)
#define KDEBUG(args...)
#define LDEBUG(args...)
#endif

#else
// ****** spi flash driver in kernel
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/vmalloc.h>
#define malloc	vmalloc
#define free	vfree
//0
#if (MTD_SPI_DEBUG == 0)
#define NDEBUG(args...) printk(args)
#define KDEBUG(args...) printk(args)
#define LDEBUG(args...) printk(args)
#endif
//1
#if (MTD_SPI_DEBUG == 1)
#define NDEBUG(args...) printk(args)
#define KDEBUG(args...) printk(args)
#define LDEBUG(args...)
#endif
//2
#if (MTD_SPI_DEBUG == 2)
#define NDEBUG(args...) printk(args)
#define KDEBUG(args...)
#define LDEBUG(args...)
#endif
//3
#if (MTD_SPI_DEBUG == 3)
#define NDEBUG(args...)
#define KDEBUG(args...)
#define LDEBUG(args...)
#endif

#endif


#define SIZEN_01M	0x14
#define SIZEN_02M	0x15
#define SIZEN_04M	0x16
#define SIZEN_08M	0x17
#define SIZEN_16M	0x18
#define SIZEN_32M	0x19
#define SIZEN_64M	0x20
#define SIZEN_CAL	0xff
#define SIZE_256B	0x100
#define SIZE_004K	0x1000
#define SIZE_064K	0x10000


/* Spanson Flash */
#define SPANSION		0x00010000		/*factory id*/
#define SPANSION_F		0x00010200		/*memory_type*/
#define S25FL004A		0x00010212
#define S25FL016A		0x00010214
#define S25FL032A		0x00010215
#define S25FL064A		0x00010216		/*supposed support*/
#define S25FL128P		0x00012018		/*only S25FL128P0XMFI001, Uniform 64KB secotr*/
										/*not support S25FL128P0XMFI011, Uniform 256KB secotr*/
										/*because #define SPI_BLOCK_SIZE 65536 */
#define S25FL032P		0x00010215


/* MICRONIX Flash */
#define MACRONIX		0x00C20000		/*factory_id*/
#define MACRONIX_05		0x00C22000		/*memory_type*/
#define MX25L4005		0x00C22013
#define MX25L1605D		0x00C22015
#define MX25L3205D		0x00C22016		/*supposed support*/
#define MX25L6405D		0x00C22017
#define MX25L12805D		0x00C22018

#define MACRONIX_Q1		0x00C22400		/*memory_type*/
#define MACRONIX_Q2		0x00C25E00		/*memory_type*/
#define MX25L1635D		0x00C22415
#define MX25L3235D		0x00C25E16
#define MX25L6445E		0x00C22017
#define MX25L12845E		0x00C22018

/* SST Flash */
#define SST				0x00BF0000		/*factory_id*/
#define SST_25			0x00BF2500		/*memory_type*/
#define SST_26			0x00BF2600		/*memory_tyep*/
#define SST25VF032B		0x00BF254A		//4MB
#define SST26VF016		0x00BF2601
#define SST26VF032		0x00BF2602

/* WinBond Flash */
#define WINBOND   0X00EF0000  /*factory_id*/
#define WINBOND_Q  0x00EF4000  /*memory_type*/
#define W25Q80   0x00EF4014 //1MB
#define W25Q16   0x00EF4015 //2MB
#define W25Q32   0x00EF4016 //4MB
#define W25Q64   0x00EF4017//8MB
#define W25Q128   0x00EF4018 //16MB
#define W25Q256   0x00EF4019 //32MB



/* Eon Flash */
#define EON				0x001c0000		/*factory_id*/
#define EON_F			0x001c3100		/*memory_type*/
#define EON_Q			0x001c3000		/*memory_type*/
#define EN25F32			0x001c3116
#define EN25F16			0x001c3115
#define EN25Q32			0x001c3016
#define EN25Q16			0x001c3015

/* GigaDevice Flash */
#define GIGADEVICE		0x00c80000		/*factory_id*/
#define GD_Q			0x00c84000		/*memory_type*/
#define GD25Q16			0x00c84015
#define GD25Q32			0x00c84016

/* Atmel Flash */
#define ATMEL			0x001f0000		/*factory_id*/
#define AT25DF161		0x001f4602


/* ESMT Flash */
#define ESMTDEVICE		0x008c0000		/*factory_id*/
#define ESMT_Q			0x008c4000		/*memory_type*/
#define F25L08			0x008c4014  /*Supposed OK , 8Mb*/
#define F25L16			0x008c4015  /*Supposed OK , 16Mb*/
#define F25L32			0x008c4116  /*20130708 Bootcode Verified OK 32Mb*/
#define F25L64			0x008c4117 /*20130708 Bootcode Verified OK 64Mb*/
#define F25L128			0x008c4118  /*No sample ,Supposed OK*/



#if 0
 /*
  * 	Structure Declaration
  */
 struct spi_flash_type
 {
	unsigned char maker_id;
	unsigned char type_id;
	unsigned char capacity_id;
	unsigned char device_size;		  // 2 ^ N (bytes)
	unsigned int 	sector_cnt; //enlarge sector_cnt, 16MB spi flash = 256 sectors
 };

struct spi_flash_db
{
   	unsigned char maker_id;
  	 unsigned char type_id;
	 signed char size_shift;
};



/* Known SPI Flash type */
const struct spi_flash_db	spi_flash_known[] =
{
   {0xC2, 0x20,   0}, /* MXIC */
   {0x01, 0x02,   1}, /* Spansion */
   {0x1C, 0x31,  0}, /* EON */
   {0x1C, 0x30,  0}, /* EON */
   {0x8C, 0x20,  0}, /* F25L016A */
   {0xEF, 0x30,  0}, /* W25X16 */
   {0x1F, 0x46,  0}, /* AT26DF161 */
   {0xBF, 0x25,  0}, /* 25VF016B-50-4c-s2AF */
   {0xC8, 0x40, 0}, /*GigaDevice GD25Q16*/
   {0xEF, 0x40, 0}, /*SPANSION S25FL016K*/
};
/* SPI Flash Info */
struct spi_flash_type	spi_flash_info;


#else
struct spi_flash_type
{
	unsigned int	chip_id;
	unsigned char	mfr_id;
	unsigned char	dev_id;

	unsigned char	capacity_id;
	unsigned char	size_shift;

	unsigned char	device_size;        // 2 ^ N (bytes)
	unsigned int	chip_size;

	unsigned int	block_size;
	unsigned int	block_cnt;

	unsigned int	sector_size;
	unsigned int	sector_cnt;

	unsigned int	page_size;
	unsigned int	page_cnt;
	unsigned int	chip_clk;
	char*			chip_name;

	unsigned int	uiClk;
#if 0
	FUNC_ERASE		pfErase;
	FUNC_WRITE		pfWrite;
	FUNC_READ		pfRead;
	FUNC_SETQEBIT	pfQeBit;
	FUNC_PAGEWRITE	pfPageWrite;
#endif
};

struct spi_flash_known
{
	unsigned int	uiChipId;
	unsigned int	uiDistinguish;
	unsigned int	uiCapacityId;
	unsigned int	uiBlockSize;
	unsigned int	uiSectorSize;
	unsigned int	uiPageSize;
	char*			pcChipName;
	unsigned int	uiClk;
#ifndef CONFIG_SPI_STD_MODE
	FUNC_ERASE		pfErase;
	FUNC_READ		pfRead;
	FUNC_SETQEBIT	pfQeBit;
	FUNC_PAGEWRITE	pfPageWrite;
#endif
};


#endif


struct spi_flash_type spi_flash_info[];

/*SPI lists*/
struct spi_flash_known spi_flash_registed[] = {
	/****************************************** Micronix Flash ******************************************/
//#define MX25L1605D		0x00C22015
{
0x00C22015, 0x00, SIZEN_02M, SIZE_064K, SIZE_004K, SIZE_256B, "MX25L1605D", 86
#ifndef CONFIG_SPI_STD_MODE
, ComSrlCmd_SE, mxic_cmd_read_s1, ComSrlCmd_NoneQeBit, mxic_cmd_write_s1
#endif
},
//#define MX25L3205D		0x00C22016
{0x00C22016, 0x00, SIZEN_04M, SIZE_064K, SIZE_004K, SIZE_256B, "MX25L3205D", 86
#ifndef CONFIG_SPI_STD_MODE
, ComSrlCmd_SE, mxic_cmd_read_s1, ComSrlCmd_NoneQeBit, mxic_cmd_write_s1
#endif
},
//#define MX25L6405D		0x00C22017
{0x00C22017, 0x00, SIZEN_08M, SIZE_064K, SIZE_004K, SIZE_256B, "MX25L6405D", 86
#ifndef CONFIG_SPI_STD_MODE
, ComSrlCmd_SE, mxic_cmd_read_s1, ComSrlCmd_NoneQeBit, mxic_cmd_write_s1
#endif
},
//#define MX25L12805D		0x00C22018
{0x00C22018, 0x00, SIZEN_16M, SIZE_064K, SIZE_004K, SIZE_256B, "MX25L12805D", 86
#ifndef CONFIG_SPI_STD_MODE
, ComSrlCmd_SE, mxic_cmd_read_s1, ComSrlCmd_NoneQeBit, mxic_cmd_write_s1
#endif
},
#if 1 //Initial 3 byte , need to issue "EN4B" command first
//#define MX25L25635E		0x00C22019
{0x00C22019, 0x00, SIZEN_32M, SIZE_064K, SIZE_004K, SIZE_256B, "MX25L25635E", 80
#ifndef CONFIG_SPI_STD_MODE
, ComSrlCmd_SE, mxic_cmd_read_s1, ComSrlCmd_NoneQeBit, mxic_cmd_write_s1
#endif
},
#endif
/*
*/
//#define MX25L25735F		0x00C22019
//#define MX25L1635D		0x00C22415
{0x00C22415, 0x00, SIZEN_02M, SIZE_064K, SIZE_004K, SIZE_256B, "MX25L1635D", 75
#ifndef CONFIG_SPI_STD_MODE
, ComSrlCmd_SE, mxic_cmd_read_q1, mxic_spi_setQEBit, mxic_cmd_write_q1
#endif
},
//#define MX25L3235D		0x00C25E16
{0x00C25E16, 0x00, SIZEN_04M, SIZE_064K, SIZE_004K, SIZE_256B, "MX25L3235D", 86
#ifndef CONFIG_SPI_STD_MODE
, ComSrlCmd_SE, mxic_cmd_read_q1, mxic_spi_setQEBit, mxic_cmd_write_q1
#endif
},
//#define MX25L6445E		0x00C22017
//#define MX25L12845E		0x00C22018
//#define MX25L4005			0x00C22013

/****************************************** Spanson Flash ******************************************/
//#define S25FL016A		0x00010214
{0x00010214, 0x00, SIZEN_02M, SIZE_064K, SIZE_064K, SIZE_256B, "S25FL016A", 50
#ifndef CONFIG_SPI_STD_MODE
, ComSrlCmd_BE, span_cmd_read_s1, ComSrlCmd_NoneQeBit, span_cmd_write_s1
#endif
},
//#define S25FL032A		0x00010215
//#define S25FL032P		0x00010215
{0x00010215, 0x00, SIZEN_04M, SIZE_064K, SIZE_064K, SIZE_256B, "S25FL032A", 50
#ifndef CONFIG_SPI_STD_MODE
, ComSrlCmd_BE, span_cmd_read_q0, span_spi_setQEBit, span_cmd_write_q0
#endif
},
//#define S25FL004A		0x00010212
//#define S25FL064A		0x00010216
{0x00010216, 0x00, SIZEN_08M, SIZE_064K, SIZE_064K, SIZE_256B, "S25FL064P", 80
#ifndef CONFIG_SPI_STD_MODE
, ComSrlCmd_BE, span_cmd_read_q0, span_spi_setQEBit, span_cmd_write_q0
#endif
},
//#define S25FL128P		0x00012018

/****************************************** Eon Flash ******************************************/
//#define EN25F16			0x001c3115
{0x001c3115, 0x00, SIZEN_02M, SIZE_064K, SIZE_004K, SIZE_256B, "EN25F16", 50
#ifndef CONFIG_SPI_STD_MODE
, ComSrlCmd_SE, eon_cmd_read_s1, ComSrlCmd_NoneQeBit, eon_cmd_write_s1
#endif
},
//#define EN25F32			0x001c3116
{0x001c3116, 0x00, SIZEN_04M, SIZE_064K, SIZE_004K, SIZE_256B, "EN25F32", 104
#ifndef CONFIG_SPI_STD_MODE
, ComSrlCmd_SE, eon_cmd_read_s1, ComSrlCmd_NoneQeBit, eon_cmd_write_s1
#endif
},
//#define EN25Q16			0x001c3015
{0x001c3015, 0x00, SIZEN_02M, SIZE_064K, SIZE_004K, SIZE_256B, "EN25Q16", 80
#ifndef CONFIG_SPI_STD_MODE
, ComSrlCmd_SE, eon_cmd_read_q1, ComSrlCmd_NoneQeBit, eon_cmd_write_s1
#endif
},
//#define EN25Q32			0x001c3016
{0x001c3016, 0x00, SIZEN_04M, SIZE_064K, SIZE_004K, SIZE_256B, "EN25Q32", 80
#ifndef CONFIG_SPI_STD_MODE
, ComSrlCmd_SE, eon_cmd_read_q1, ComSrlCmd_NoneQeBit, eon_cmd_write_s1
#endif
},

#ifndef CONFIG_SPI_STD_MODE
/****************************************** SST Flash ******************************************/
//#define SST25VF032B		0x00BF254A
{0x00BF254A, 0x00, SIZEN_04M, SIZE_064K, SIZE_004K, SIZE_256B, "SST25VF032B", 40
#ifndef CONFIG_SPI_STD_MODE
, ComSrlCmd_SE, sst_cmd_read_s1, ComSrlCmd_NoneQeBit, sst_cmd_write_s1
#endif
},
//#define SST26VF032		0x00BF2602
//#define SST26VF016		0x00BF2601
#endif

/****************************************** GigaDevice Flash ******************************************/
//#define GD25Q16			0x00c84015
{0x00c84015, 0x00, SIZEN_02M, SIZE_064K, SIZE_004K, SIZE_256B, "GD25Q16", 120
#ifndef CONFIG_SPI_STD_MODE
, ComSrlCmd_SE, gd_cmd_read_q0, gd_spi_setQEBit, gd_cmd_write_s1
#endif
},
//#define GD25Q32			0x00c84016
{0x00c84016, 0x00, SIZEN_04M, SIZE_064K, SIZE_004K, SIZE_256B, "GD25Q32", 120
#ifndef CONFIG_SPI_STD_MODE
, ComSrlCmd_SE, gd_cmd_read_q0, gd_spi_setQEBit, gd_cmd_write_s1
#endif
},

/****************************************** WinBond Flash ******************************************/
//#define W25Q16			0x00EF4015
{0x00EF4015, 0x00, SIZEN_02M, SIZE_064K, SIZE_004K, SIZE_256B, "W25Q16", 104
#ifndef CONFIG_SPI_STD_MODE
, ComSrlCmd_SE, wb_cmd_read_q0, wb_spi_setQEBit, wb_cmd_write_q0
#endif
},
//#define W25Q32			0x00EF4016
{0x00EF4016, 0x00, SIZEN_04M, SIZE_064K, SIZE_004K, SIZE_256B, "W25Q32", 104
#ifndef CONFIG_SPI_STD_MODE
, ComSrlCmd_SE, wb_cmd_read_q0, wb_spi_setQEBit, wb_cmd_write_q0
#endif
},
//#define W25Q64			0x00EF4017
{0x00EF4017, 0x00, SIZEN_08M, SIZE_064K, SIZE_004K, SIZE_256B, "W25Q64", 104
#ifndef CONFIG_SPI_STD_MODE
, ComSrlCmd_SE, wb_cmd_read_q0, wb_spi_setQEBit, wb_cmd_write_q0
#endif
},
#ifndef CONFIG_SPI_3to4BYTES_ADDRESS_SUPPORT
//#define W25Q128			0x00EF4018
{0x00EF4018, 0x00, SIZEN_16M, SIZE_064K, SIZE_004K, SIZE_256B, "W25Q128", 104
#ifndef CONFIG_SPI_STD_MODE
, ComSrlCmd_SE, wb_cmd_read_q0, wb_spi_setQEBit, wb_cmd_write_q0
#endif
},
#else
//#define W25Q256			0x00EF4019
{0x00EF4019, 0x00, SIZEN_32M, SIZE_064K, SIZE_004K, SIZE_256B, "W25Q256", 104
#ifndef CONFIG_SPI_STD_MODE
, ComSrlCmd_SE, wb_cmd_read_q0, wb_spi_setQEBit, wb_cmd_write_q0
#endif
},
#endif
//#define W25X16			0x00EF3015
{0x00EF3015, 0x00, SIZEN_02M, SIZE_064K, SIZE_004K, SIZE_256B, "W25X16", 75
#ifndef CONFIG_SPI_STD_MODE
, ComSrlCmd_SE, at_cmd_read_d0, ComSrlCmd_NoneQeBit, mxic_cmd_write_s1
#endif
},
//#define W25X32			0x00EF3016
{0x00EF3016, 0x00, SIZEN_04M, SIZE_064K, SIZE_004K, SIZE_256B, "W25X32", 75
#ifndef CONFIG_SPI_STD_MODE
, ComSrlCmd_SE, at_cmd_read_d0, ComSrlCmd_NoneQeBit, mxic_cmd_write_s1
#endif
},
//#define W25X64			0x00EF3017
{0x00EF3016, 0x00, SIZEN_08M, SIZE_064K, SIZE_004K, SIZE_256B, "W25X64", 75
#ifndef CONFIG_SPI_STD_MODE
, ComSrlCmd_SE, at_cmd_read_d0, ComSrlCmd_NoneQeBit, mxic_cmd_write_s1
#endif
},
/****************************************** ATMEL Flash ******************************************/
//#define AT25DF161		0x001f4602
{0x001f4602, 0x00, SIZEN_02M, SIZE_064K, SIZE_004K, SIZE_256B, "AT25DF161", 85
#ifndef CONFIG_SPI_STD_MODE
, ComSrlCmd_SE, at_cmd_read_d0, ComSrlCmd_NoneQeBit, at_cmd_write_d0
#endif
},
};



//printf ("RXI-310 SPI_Flash_Controller flash_init\n");
// printf("\r\nspi_flash_map :0x%x\r\n",spi_flash_map );

/*-----------------------------------------------------------------------
 * flash_init()
 *
 * sets up flash_info and returns size of FLASH (bytes)
 */



  struct dw_device dev;
  struct spi_flash_portmap *spi_flash_map;
  struct device_info dev_info;
  struct spi_flash_param ps_para= CC_DEFINE_SPI_FLASH_PARAMS(ps_);




 //USER MODE TEST
unsigned long flash_init_rtk(void)
{
  //unsigned int uiCount ;

  uint32_t size=0;

  //int i,j,offset;

  //unsigned int flashbase = FLASH_BASE; //define in "spi_flash/spi_flash_private.h"

   dev.base_address = (struct dw_device *) BSP_PS_I_SPIC_BASE;
  spi_flash_map = (struct dw_device *) BSP_PS_I_SPIC_BASE;
  dev.comp_param = &ps_para;
  dev.instance = &dev_info;

  spi_flash_map = (struct spi_flash_portmap *) BSP_PS_I_SPIC_BASE;

  //********************************************************
  //*******  iniitialize Flash_Device_information  *********
  //********************************************************


   printf("\r\nspi_flash_map :0x%x\r\n",spi_flash_map );


  memset(&flash_info, 0, sizeof(flash_info));
  memset(&spi_flash_info, 0, sizeof(struct spi_flash_type));


  spi_regist(0);
  // Normal:RDID
  //info = flash_init(&dev, 0x9f); //"0"=pass , "1"=something wrong


 size=1<<(REG32(BSP_PS_I_SPIC_BASE+SPIC_FLASH_SIZE)+12);

 return size; //bytes

#if 0
  spi_flash_info.maker_id = (info >> 16) & 0xFF;
  spi_flash_info.type_id = (info >> 8) & 0xFF;
  spi_flash_info.capacity_id = (info ) & 0xFF;


  uiCount = sizeof(spi_flash_registed) / sizeof(struct spi_flash_known);



	/* Iterate Each Maker ID/Type ID Pair */
//	for (i = 0; i < sizeof(spi_flash_known) / sizeof(struct spi_flash_db); i++)
  for (i = 0; i < uiCount; i++)
	{
		if ( (spi_flash_info.maker_id == spi_flash_known[i].maker_id) &&
			(spi_flash_info.type_id == spi_flash_known[i].type_id) )
		{
			spi_flash_info.device_size = (unsigned char)((signed char)spi_flash_info.capacity_id + spi_flash_known[i].size_shift);
			//prnFlashInfo(ucChip, spi_flash_info[ucChip]);
			prnFlashInfo(i, spi_flash_known[i]);
			break;
		}
	}




	spi_flash_info.sector_cnt = ((1<<spi_flash_info.device_size)>>16);

	for(i=0;i<CONFIG_SYS_MAX_FLASH_BANKS;i++){
		flash_info[i].size = (1<<(spi_flash_info.device_size));
		flash_info[i].sector_count = spi_flash_info.sector_cnt;
		flash_info[i].flash_id = ((spi_flash_info.maker_id<<16) | (spi_flash_info.type_id<<8));
		size += flash_info[i].size;

		printf("\nflash_info[%d].size=0x%x bytes\n",i,flash_info[i].size);

		for(j=0, offset=0;offset<(flash_info[i].size);j++, offset+=SPI_BLOCK_SIZE){
			flash_info[i].start[j] = flashbase+offset;
		}
	}


	printf("\nSPI flash size (from ID info) =0x%x bytes\n",size);



	 if(flash_info[0].size > CONFIG_SYS_MONITOR_LEN){
    		flash_info[0].protect[ CONFIG_SYS_MONITOR_LEN/SPI_BLOCK_SIZE ] = 1;
    	}

  	return size;
  #endif
}



#if 1 //original
int write_buff(flash_info_t *info, uchar *src, ulong addr, ulong cnt)
{
	printf ("write_buff not implemented\n");
	return (-1);
}

#endif

// Calculate chip capacity shift bit
unsigned char calShift(unsigned char ucCapacityId, unsigned char ucChipSize)
{
	unsigned int ui;
	if(ucChipSize > ucCapacityId)
	{
		ui = ucChipSize - ucCapacityId;
	}
	else
	{
		ui = ucChipSize + 0x100 -ucCapacityId;
	}
	LDEBUG("calShift: ucCapacityId=%x; ucChipSize=%x; ucReturnVal=%x\n", ucCapacityId, ucChipSize, ui);
	return (unsigned char)ui;
}



// Print spi_flash_type
void prnFlashInfo(unsigned char ucChip, struct spi_flash_type sftInfo)
{

	unsigned short baudrate_divider_ocp;//SPI flash clock = OCP/baudrate_divider_ocp

	baudrate_divider_ocp=6;//should be auto-calculated by using system register's OCP clock info
	 /*Set baud_rate for SPI flash controller*/


#ifndef CONFIG_SPI_STD_MODE
	NDEBUG("\n********************************************************************************\n");
	NDEBUG("*\n");
	NDEBUG("* chip__no chip__id mfr___id dev___id cap___id size_sft dev_size chipSize\n");
	NDEBUG("* %7xh %7xh %7xh %7xh %7xh %7xh %7xh %7xh\n", ucChip, sftInfo.chip_id, sftInfo.mfr_id, sftInfo.dev_id, sftInfo.capacity_id, sftInfo.size_shift, sftInfo.device_size, sftInfo.chip_size);
	NDEBUG("* blk_size blk__cnt sec_size sec__cnt pageSize page_cnt chip_clk chipName\n");
	NDEBUG("* %7xh %7xh %7xh %7xh %7xh %7xh %7xh %s\n", sftInfo.block_size, sftInfo.block_cnt, sftInfo.sector_size, sftInfo.sector_cnt, sftInfo.page_size, sftInfo.page_cnt, sftInfo.chip_clk, sftInfo.chip_name);
	NDEBUG("* \n");
	NDEBUG("********************************************************************************\n");
#else
	NDEBUG("\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
	NDEBUG("@\n");
	NDEBUG("@ chip__no chip__id mfr___id dev___id cap___id size_sft dev_size chipSize\n");
	NDEBUG("@ %7xh %7xh %7xh %7xh %7xh %7xh %7xh %7xh\n", ucChip, sftInfo.chip_id, sftInfo.mfr_id, sftInfo.dev_id, sftInfo.capacity_id, sftInfo.size_shift, sftInfo.device_size, sftInfo.chip_size);
	NDEBUG("@ blk_size blk__cnt sec_size sec__cnt pageSize page_cnt chip_clk chipName\n");
	NDEBUG("@ %7xh %7xh %7xh %7xh %7xh %7xh %7xh %s\n", sftInfo.block_size, sftInfo.block_cnt, sftInfo.sector_size, sftInfo.sector_cnt, sftInfo.page_size, sftInfo.page_cnt, sftInfo.chip_clk, sftInfo.chip_name);
	NDEBUG("@ \n");
	NDEBUG("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
#endif


      spi_flash_setbaudr(&dev, baudrate_divider_ocp);

	/*Config FLASH_SIZE(0x124) for SPIC auto-mode , will active CS1 when access over its size
	   FLASH_SIZE[3:0] , fomula = Exponent of 2(Flash size) -12 (Smallest flash size is 4KB)
	   ex:4MB (2^22) , set FLASH_SIZE[3:0]=22-12=10
	   	 8MB (2^23) , set FLASH_SIZE[3:0]=23-12=11
	   	 16MB (2^24) , set FLASH_SIZE[3:0]=24-12=12
	   	 32MB (2^25) , set FLASH_SIZE[3:0]=25-12=13

	*/
	//sftInfo.chip_size
	REG32(BSP_PS_I_SPIC_BASE+SPIC_SSIENR)&=~(1<<0);//SSIENR bit[0]=0


	switch (sftInfo.chip_size)
	{
		case 0x00200000: //2MB
			REG32(BSP_PS_I_SPIC_BASE+SPIC_FLASH_SIZE)=9 ;
			break;
		case 0x00400000: //4MB
			REG32(BSP_PS_I_SPIC_BASE+SPIC_FLASH_SIZE)=10 ;
			break;
		case 0x00800000: //8MB
			REG32(BSP_PS_I_SPIC_BASE+SPIC_FLASH_SIZE)=11 ;
			break;
		case 0x01000000: //16MB
			REG32(BSP_PS_I_SPIC_BASE+SPIC_FLASH_SIZE)=12 ;
			break;

		case 0x02000000: //32MB
			REG32(BSP_PS_I_SPIC_BASE+SPIC_FLASH_SIZE)=13 ;
			break;

		case 0x04000000: //64MB
			REG32(BSP_PS_I_SPIC_BASE+SPIC_FLASH_SIZE)=14 ;
			break;

		case 0x08000000: //128MB
			REG32(BSP_PS_I_SPIC_BASE+SPIC_FLASH_SIZE)=15 ;
			break;


		default:
			printf ("Error: No match size to config auto-mode of SPI flash!\n");

	}
	//REG32(BSP_PS_I_SPIC_BASE+SPIC_FLASH_SIZE)=((sftInfo.chip_size/0x00100000)-19) ;

	printf("\nSPIC(SPIC_FLASH_SIZE_OFFSET_0x%x)=0x%x \n",BSP_PS_I_SPIC_BASE+SPIC_FLASH_SIZE,REG32(BSP_PS_I_SPIC_BASE+SPIC_FLASH_SIZE));



}
// print when writing


// spi flash probe
void spi_regist(unsigned char ucChip)
{
	unsigned int ui, i, uiCount;
	unsigned char pucBuffer[4];



	 spi_flash_setser(&dev, ucChip);//ucChip=chip select


 	 ui=flash_device_init(&dev, 0x9f);


 	 printf("\r\nSPI FLASH ID info=0x%x\n ", ui);


       #if 0 //for 8196F FPGA debug only
	   	spi_flash_setbaudr(&dev, 1);

       	return; // 20141111: JSW :For 8196F Uboot debug
	#endif


	uiCount = sizeof(spi_flash_registed) / sizeof(struct spi_flash_known);

	for (i = 0; i < uiCount; i++)
	{
		if((spi_flash_registed[i].uiChipId == ui) && (spi_flash_registed[i].uiDistinguish == 0x00))
		{
			break;
		}
		else
		{
			printf("spi_probe dbg004: spi_regist(0, 1)\n");
		}
	}
	//printf("spi_probe dbg004: spi_regist(0, 1)\n");
	if(i == uiCount)
	{
		// default setting
		//setFSCR(ucChip, 40, 1, 1, 31);
		//set_flash_info(ucChip, ui, SIZEN_16M, SIZE_064K, SIZE_004K, SIZE_256B, "UNKNOWN", ComSrlCmd_SE, mxic_cmd_read_s1, ComSrlCmd_NoneQeBit, mxic_cmd_write_s1);
		set_flash_info(ucChip, ui, SIZEN_16M, SIZE_064K, SIZE_004K, SIZE_256B, "UNKNOWN");

	}
	else
	{
		// have registed
		//setFSCR(ucChip, spi_flash_registed[i].uiClk, 1, 1, 31);
#if 0//ndef CONFIG_SPI_STD_MODE
		set_flash_info(ucChip, ui, spi_flash_registed[i].uiCapacityId, spi_flash_registed[i].uiBlockSize, spi_flash_registed[i].uiSectorSize, spi_flash_registed[i].uiPageSize, spi_flash_registed[i].pcChipName, spi_flash_registed[i].pfErase, spi_flash_registed[i].pfRead, spi_flash_registed[i].pfQeBit, spi_flash_registed[i].pfPageWrite);
#else
		if((ui & 0x00ffff00) == SPANSION_F)
		{
			//set_flash_info(ucChip, ui, spi_flash_registed[i].uiCapacityId, spi_flash_registed[i].uiBlockSize, spi_flash_registed[i].uiSectorSize, spi_flash_registed[i].uiPageSize, spi_flash_registed[i].pcChipName, ComSrlCmd_BE, mxic_cmd_read_s1, ComSrlCmd_NoneQeBit, mxic_cmd_write_s1);
			set_flash_info(ucChip, ui, spi_flash_registed[i].uiCapacityId, spi_flash_registed[i].uiBlockSize, spi_flash_registed[i].uiSectorSize, spi_flash_registed[i].uiPageSize, spi_flash_registed[i].pcChipName);
		}
		else
		{
			//set_flash_info(ucChip, ui, spi_flash_registed[i].uiCapacityId, spi_flash_registed[i].uiBlockSize, spi_flash_registed[i].uiSectorSize, spi_flash_registed[i].uiPageSize, spi_flash_registed[i].pcChipName, ComSrlCmd_SE, mxic_cmd_read_s1, ComSrlCmd_NoneQeBit, mxic_cmd_write_s1);
			set_flash_info(ucChip, ui, spi_flash_registed[i].uiCapacityId, spi_flash_registed[i].uiBlockSize, spi_flash_registed[i].uiSectorSize, spi_flash_registed[i].uiPageSize, spi_flash_registed[i].pcChipName);
		}

#endif
	}
	//spi_flash_info[ucChip].pfQeBit(ucChip);

	prnFlashInfo(ucChip, spi_flash_info[ucChip]);
	//ui = spi_flash_info[ucChip].pfRead(ucChip, 0x00, 4, pucBuffer);
	//printf("spi_regist: ucChip=%x; i=%x; uiCount=%x\n", ucChip, i, uiCount);


}




// set spi_flash_info struction content
//void set_flash_info(unsigned char ucChip, unsigned int chip_id, unsigned int device_cap, unsigned int block_size, unsigned int sector_size, unsigned int page_size, char* chip_name, FUNC_ERASE pfErase, FUNC_READ pfRead, FUNC_SETQEBIT pfQeBit, FUNC_PAGEWRITE pfPageWrite)
void set_flash_info(unsigned char ucChip, unsigned int chip_id, unsigned int device_cap, unsigned int block_size, unsigned int sector_size, unsigned int page_size, char* chip_name)
{
	unsigned int ui = 1 << device_cap;
	spi_flash_info[ucChip].chip_id = chip_id;
	spi_flash_info[ucChip].mfr_id = (chip_id >> 16) & 0xff;
	spi_flash_info[ucChip].dev_id = (chip_id >> 8) & 0xff;
	spi_flash_info[ucChip].capacity_id = (chip_id) & 0xff;
	spi_flash_info[ucChip].size_shift = calShift(spi_flash_info[ucChip].capacity_id, device_cap);
	spi_flash_info[ucChip].device_size = device_cap;			// 2 ^ N (bytes)
	spi_flash_info[ucChip].chip_size =  ui;
	spi_flash_info[ucChip].block_size = block_size;
	spi_flash_info[ucChip].block_cnt = ui / block_size;
	spi_flash_info[ucChip].sector_size = sector_size;
	spi_flash_info[ucChip].sector_cnt = ui / sector_size;
	spi_flash_info[ucChip].page_size = page_size;
	spi_flash_info[ucChip].page_cnt = sector_size / page_size;
	spi_flash_info[ucChip].chip_name = chip_name;
	#if 0
	spi_flash_info[ucChip].pfErase = pfErase;
	spi_flash_info[ucChip].pfWrite = ComSrlCmd_ComWriteData;
	spi_flash_info[ucChip].pfRead = pfRead;
	spi_flash_info[ucChip].pfQeBit = pfQeBit;
	spi_flash_info[ucChip].pfPageWrite = pfPageWrite;
	#endif
	//SPI_REG_LOAD(SFCR2, 0x0bb08000);
	//printf("set_flash_info: ucChip=%x; chip_id=%x; device_cap=%x; block_size=%x; sector_size=%x; page_size=%x; chip_name=%s\n", ucChip, chip_id, device_cap, block_size, sector_size, page_size, chip_name);
}




