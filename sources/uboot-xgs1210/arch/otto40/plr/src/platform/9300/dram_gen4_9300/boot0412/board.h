/*
 * Board.h - System and memory specific configurations and 
 *           parameters.
 *
 * History:
 *  2006/10/27  SH, Lee    Added Big model's support
 *  2008/08/01  SH	Add definition of SCCR register for Chip version detection
 */

//#define SUPPORT_8672_PLATFORM
#define SUPPORT_8676_PLATFORM

/* System Bus Clock configuration  */
#define SYS_CLOCK_175MHZ //chip_detect() in loader/main.c will set the proper system clock, just set 175 as default value for boot/romInit.s

/* SDRAM Clock configuration, undefined 116Mhz and 166Mhz is set to 133Mhz */
#define SDRAM_CLOCK_166MHZ

/* CPU (OCP) Clock Configuration, undefined 340Mhz is set as 400Mhz */
//#define CPU_CLOCK_340MHZ

#if 0 // mask the definitions, chip_detect will set system clk
#ifdef SYS_CLOCK_200MHZ
#define CLOCKGEN_FREQ	200000000
#elif defined(SYS_CLOCK_175MHZ)
#define CLOCKGEN_FREQ	175000000
#elif defined(SYS_CLOCK_180MHZ)
#define CLOCKGEN_FREQ	180000000
#else
#define CLOCKGEN_FREQ	175000000
#endif
#endif
#ifdef SDRAM_AUTO_DETECT
#else
//#define CONFIG_64M_SDRAM
#undef CONFIG_32M_SDRAM
#define CONFIG_16M_SDRAM

/*ql:20080927 START: modify flash layout*/
//#define NEW_FLASH_LAYOUT//ql: I have moved it to config.mk
/*ql:20080927 END*/


//tylo, 8672
#ifdef CONFIG_64M_SDRAM
#define MCR0_VAL    0x54880000
#elif CONFIG_32M_SDRAM
#define MCR0_VAL	0x54480000
#elif defined(CONFIG_16M_SDRAM)
#define MCR0_VAL	0x52480000	//16M
#else
#define MCR0_VAL 0x52080000	//8M
#endif

#endif //ifndef SDRAM_AUTO_DETECT


//#define MCR0_VAL	0xe2800000	//8M/16M
//#define MCR0_VAL	0x52800000	//2M/8M
//#define MCR0_VAL	0x62800000	//2M/16M
//#define MCR0_VAL	0xA2800000	//4M/16M
//#define MCR0_VAL	0x02800000	//512/8M
//#define MCR0_VAL	0x42800000	
#define MCR1_VAL	0x1818ffff
//#define MCR1_VAL	0xe825000a
#define MCR2_VAL	0x00000CC9


/* System interface */
/*internal phy*/
//#define SICR_VAL	0xb7053a19    //for external phy & PCI
#define SICR_VAL	0xb7053819    //for internal phy & PCI

/* Lexra Bus Arbitor */
#define CPUC_VAL	0x0c0100FF    //for device bus weighting

#define SCCR		0xb8003200

#ifdef CONFIG_RTL8685_PERI
#define Timer_Control_Base 0xB8003200
#define Timer_WDTCTRLR     (Timer_Control_Base+0x68)  //watchdog timer control!
#define Timer_WDT_E        (1<<31)
#else /*CONFIG_RTL8685_PERI*/
//ccwei
#define Timer_Control_Base 0xB8003100
#define Timer_CDBR         (Timer_Control_Base+0x18)  //watchdog timer Clock division base!
#define Timer_WDTCNR       (Timer_Control_Base+0x1C)  //watchdog timer control!
//end-ccwei
#endif /*CONFIG_RTL8685_PERI*/

#define NAND_CTRL_BASE  0xB801A000
#define NACFR  (NAND_CTRL_BASE + 0x0)

#if defined(CONFIG_RTL8685) || defined(CONFIG_RTL8685S)
#define Boot_Select				0xB8001000
	#ifdef CONFIG_SPI_NAND_FLASH
	#define Boot_NF 			(1<<24)
	#define NAFC_NF (1<<28) //define in NAND_CTRL_BASE
	#else
	#define Boot_NF 			(1<<25)
	#define NAFC_NF	(1<<27) //define in NAND_CTRL_BASE
	#endif
#else
#define NAFC_NF	(1<<28) //define in NAND_CTRL_BASE
#define Boot_Select				0xB8000304
#define Boot_NF					(1<<6)
#define NAND_ADDR_MASK			(3<<7)
#endif
#define IS_BOOT_NAND 	((((*(volatile unsigned int *)((Boot_Select)) & Boot_NF) == Boot_NF) ? 1:0))
#define IS_0412		(*((volatile unsigned int *)0xB8000224) >> 16 == 0x0412)
#define IS_6239		(*((volatile unsigned int *)0xB8000224) >> 16 == 0x6239)
#define IS_RTL8676	(IS_6239 || IS_0412)
//tctsai
#define IS_0562		(*((volatile unsigned int *)0xB8000224) >> 16 == 0x0562)
#define IS_6333		(*((volatile unsigned int *)0xB8000224) >> 16 == 0x6333)
#define IS_RTL8676S	(IS_6333 || IS_0562)
//end-tctsai
#define IS_RL6318A			((*((volatile unsigned int *)0xB8000000) == 0x000e0561)?1:0)
#define IS_RL6318B			((*((volatile unsigned int *)0xB8000000) == 0x000c0561)?1:0)
#define IS_RL6318C			((*((volatile unsigned int *)0xB8000000) == 0x00080561)?1:0)
#define IS_RTL8685			(IS_RL6318A || IS_RL6318B || IS_RL6318C)
#define IS_RTL8685_256PIN	((*((volatile unsigned int *)0xB8000100)>>27) & 1)
#define IS_SDS_IF				((*((volatile unsigned int *)0xB8000100)>>14) & 1)

/* RTL8685S, including RLE0705A, RLE0705B, RL6405 */
#define IS_RLE0705			((*((volatile unsigned int *)0xB8000000) == 0x00000705)?1:0)
#define IS_RL6405A			((*((volatile unsigned int *)0xB8000000) == 0x00006405)?1:0)
#define IS_RL6405B			((*((volatile unsigned int *)0xB8000000) == 0x00016405)?1:0)
#define IS_RTL8685S			(IS_RLE0705 || IS_RL6405A || IS_RL6405B)

/* RTL8685SB, including RLE0797 */
#define IS_RLE0797			((*((volatile unsigned int *)0xB8000000) == 0x00000797)?1:0)
#define IS_RTL8685SB			(IS_RLE0797)

//end-ccwei
#ifndef _ASMLANGUAGE
#define N_MAC	2
#define BOARD_BOOTLINE_SZ	8
struct board_param_s
{
	char	id[8];						/* ID, 			offset 0x0000 */
	char	bootline[BOARD_BOOTLINE_SZ];/* BootLine, 	offset 0x0008 */
	char	mac[N_MAC][8];				/* MACs,		offset 0x0010 */
	unsigned long	entry;				/* entry point, offset 0x0020 */
	unsigned long	load;				/* load buffer, offset 0x0024 */
	unsigned long	app;				/* application, offset 0x0028 */
	unsigned long	ip;					/* IP,			offset 0x002C */

	unsigned long flash_size;			/* Flash size,    offset 0x0030, we support 1M/2M/4M/8M*/
	unsigned long mem_size;				/* Memory size,	offset 0x0034, we support 4M/8M/16M*/
	unsigned long MII_select;			/* MII select,	offset 0x0038, 0x00 Internal PHY  0x01: external PHY*/
	unsigned long	reserved[1];		/* Reserved,	offset 0x003C */
#ifdef CONFIG_RTL8685S_DYNAMIC_FREQ
	unsigned long	sysclk_ctrl;		/* Value of system clock register, offset 0x4000 */
	unsigned long	mckg_freq_div;		/* Value of mckg frequency register, offset 0x4004 */
	unsigned long lx_pll_sel;			/* Value of LX pll sel register, offset 0x4008 */
	unsigned long	cmu_ctrl;			/* Value of CMU control register, offset 0x400c */
#endif /* CONFIG_RTL8685S_DYNAMIC_FREQ */

};

typedef struct board_param_s board_param_t, *board_param_p;

#endif	//#ifndef _ASMLANGUAGE

#define BOARD_PARAM_SZ		sizeof(board_param_t)
#define BOARD_PARAM_ID		"Berlin"
#define BOARD_PARAM_BOOT	"file"
#define BOARD_PARAM_BOOT2	"re0"
#define BOARD_PARAM_MAC		"\x00\x23\x79\x11\x22\x33"
#define BOARD_PARAM_ENTRY	0x80000000
#define BOARD_PARAM_LOAD	0x80000000


#ifdef CONFIG_SPANSION_16M_FLASH

#ifdef NEW_FLASH_LAYOUT
#ifdef SUPPORT_Z_NEW_LAYOUT
#define BOARD_PARAM_APP		0xBD0C0000
#else
#ifdef IMAGE_DOUBLE_BACKUP_SUPPORT
#define BOARD_PARAM_APP		0xBD100000
#endif
#ifdef IMAGE_SINGLE_BACKUP_SUPPORT
#define BOARD_PARAM_APP		0xBD300000
#endif
#endif	// SUPPORT_Z_NEW_LAYOUT
#else	// NEW_FLASH_LAYOUT
#define BOARD_PARAM_APP		0xBFC80000
#endif	// NEW_FLASH_LAYOUT
//#else

//#define BOARD_PARAM_APP		0xBFC20000
#endif // end CONFIG_SPANSION_16M_FLASH

#define BOARD_PARAM_BAUD	115200
#define BOARD_PARAM_IP		0xc0a80101 //192.168.1.1

#define BOARD_PARAM_FLASHSIZE	0xfffe8000 //2M  0xfffc8000 //4M 0xfffe8000      //16M
#if 0
#ifdef CONFIG_32M_SDRAM
#define BOARD_PARAM_MEMSIZE		0x54480000
#elif defined(CONFIG_16M_SDRAM)
#define BOARD_PARAM_MEMSIZE		0x52480000
#else
#define BOARD_PARAM_MEMSIZE		0x52080000
#endif
#else
#define BOARD_PARAM_MEMSIZE MCR0_VAL
#endif
#define BOARD_PARAM_MIISEL		0	// Using Int. PHY
#define FLASH_TIMEOUT 		1000 // ms

//for web server
#define DRAM_MAP_DLOAD_BUF_ADDR             0x80200000//0x80400000
//#define DRAM_MAP_DLOAD_BUF_ADDR				0x80A00000


#define FLASH_START_4M_SPACE		0xbfc00000
#define CPU_REBOOT_ADDR				FLASH_START_4M_SPACE
#ifdef SUPPORT_DIRECT_16M_ADDR
#define FLASH_BASE_ADDR				0xbd000000	
#else		
#define FLASH_BASE_ADDR				(FLASH_START) //0xbfc00000
#endif

#ifdef CONFIG_NFBI_SLAVE
#define CONFIG_BOOT_SIZE_32K
#else
//#define CONFIG_BOOT_SIZE_64K
#define CONFIG_BOOT_SIZE_128K
#endif
#ifdef CONFIG_BOOT_SIZE_128K
#define CONFIG_BOOT_SIZE		0x20000
#elif defined(CONFIG_BOOT_SIZE_32K)
#define CONFIG_BOOT_SIZE		0x8000
#else
#define CONFIG_BOOT_SIZE		0x10000
#endif /*CONFIG_BOOT_SIZE_128K*/

#ifdef CONFIG_SPI_FLASH
#ifdef CONFIG_BOOT_SIZE_128K
#define _PARM_BACK_START_spi	0x10000
#else
#define _PARM_BACK_START_spi	0x0
#endif /*CONFIG_BOOT_SIZE_128K*/
#define _PARM_BACK_SZ_spi		0x10000
#endif /*CONFIG_SPI_FLASH*/

#ifdef CONFIG_SPANSION_16M_FLASH
#define _PARM_SZ				0x100
#else
#ifdef CONFIG_RTL8685S_DYNAMIC_FREQ
#define _PARM_SZ				0x50
#else
#define _PARM_SZ				0x40
#endif /* CONFIG_RTL8685S_DYNAMIC_FREQ */
#endif /*CONFIG_SPANSION_16M_FLASH*/
#define _PARM_END				(FLASH_BASE_ADDR+CONFIG_BOOT_SIZE)
#define _PARM_START				(_PARM_END-_PARM_SZ)
#ifdef CONFIG_SPANSION_16M_FLASH
#define _PARM_BACK_START		0x0
#define _PARM_BACK_SZ			0x20000
#else
#ifdef CONFIG_BOOT_SIZE_128K
#define _PARM_BACK_START		0x10000
#define _PARM_BACK_SZ			0x10000
#else
#define _PARM_BACK_START		0x8000
#define _PARM_BACK_SZ			0x8000
#endif /*CONFIG_BOOT_SIZE_128K*/
#endif /*CONFIG_SPANSION_16M_FLASH*/

#ifndef BOARD_PARAM_APP
#define BOARD_PARAM_APP			(FLASH_BASE_ADDR+CONFIG_BOOT_SIZE+0x20000)
#endif /*BOARD_PARAM_APP*/


#define CONFIG_OSK_APP_SUPPORT 	1

#ifdef IMAGE_DOUBLE_BACKUP_SUPPORT
#ifdef NEW_FLASH_LAYOUT
//#define DRAM_TEMP_LOAD_ADDR			(0x80500000)
#define DRAM_TEMP_LOAD_ADDR			(0x80800000)
#ifndef SUPPORT_DIRECT_16M_ADDR
#define IMAGE_LOCATION_FLAG_ADDR	(0xBFCA0000)
#define FIRST_IMAGE_ADDR				(0xBFD00000)
#define SECOND_IMAGE_ADDR			(0xC0200000)
#else 
#ifdef SUPPORT_Z_NEW_LAYOUT
#define IMAGE_LOCATION_FLAG_ADDR	(0xBD020000-4)
#define FIRST_IMAGE_ADDR			(0xBD0C0000)
#define SECOND_IMAGE_ADDR			(0xBD660000)
#else
#define IMAGE_LOCATION_FLAG_ADDR	(0xBD0A0000)
#define FIRST_IMAGE_ADDR			(0xBD100000)
#define SECOND_IMAGE_ADDR			(0xBD600000)
#endif
#endif
#define IMAGE_MAX_SIZE 				(0x500000)
#else
#define DRAM_TEMP_LOAD_ADDR		(0x80000000)
#define   IMAGE_LOCATION_FLAG_ADDR  (0xC0020000)
#define FIRST_IMAGE_ADDR			  (0xbfc80000)
#define SECOND_IMAGE_ADDR                   (0xc0080000)
#define IMAGE_MAX_SIZE                          (0x380000)
//#define HW_SETTING_ADDR                       (0xBFC05000)
//#define HW_SETTING_SIZE                         (0x1000)
#endif
#elif IMAGE_SINGLE_BACKUP_SUPPORT
#define E8_SMALL_IMAGE_ADDR			0xbd100000
#ifdef NEW_FLASH_LAYOUT
#define DRAM_TEMP_LOAD_ADDR			(0x80800000)
#define IMAGE_MAX_SIZE                          (0x500000)
#else
#define DRAM_TEMP_LOAD_ADDR			(0x80000000)
#define IMAGE_MAX_SIZE                          (0x380000)
#endif
#else
#define DRAM_TEMP_LOAD_ADDR		(0x80000000)
#define IMAGE_MAX_SIZE                          (0x380000)
#endif

#ifndef REG32
	#define REG32(reg)	(*(volatile unsigned int *)(reg))
#endif
#ifndef REG16
  	#define REG16(reg)	(*(volatile unsigned short *)(reg))
#endif
#ifndef REG8
	#define REG8(reg)	(*(volatile unsigned char *)(reg))
#endif
#ifndef WRITE_MEM32
	#define WRITE_MEM32(addr, val)   	(*(volatile unsigned int *)   (addr)) = (val)
#endif
#ifndef READ_MEM32
	#define READ_MEM32(addr)         		(*(volatile unsigned int *)   (addr))
#endif
#ifndef WRITE_MEM16
	#define WRITE_MEM16(addr, val)   	(*(volatile unsigned short *) (addr)) = (val)
#endif
#ifndef READ_MEM16
	#define READ_MEM16(addr)         		(*(volatile unsigned short *) (addr))
#endif
#ifndef WRITE_MEM8
	#define WRITE_MEM8(addr, val)    	(*(volatile unsigned char *)  (addr)) = (val)
#endif
#ifndef READ_MEM8
	#define READ_MEM8(addr)          		(*(volatile unsigned char *)  (addr))
#endif

#ifndef _ASMLANGUAGE
#ifdef CONFIG_NFBI_SLAVE
#define BOOTCONF_MAGIC (('b'<<24) | ('t'<<16) | ('c'<<8) | ('f')) //0x62746366
#define BOOTCONF_SIZE  0x40
#define BOOTCONF_START (_PARM_START-BOOTCONF_SIZE)
struct bootconf
{
	unsigned long	magic;
	unsigned char	mac[6];
	unsigned short	flag;
	unsigned long	ip;
	unsigned long	ipmask;
	unsigned long	serverip;
	unsigned char	filename[24];
	unsigned char	res[16];
};
typedef struct bootconf bootconf_t, *bootconf_p;
#endif /*CONFIG_NFBI_SLAVE*/
#endif /*!_ASMLANGUAGE*/

//#endif
