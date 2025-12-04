/* SPDX-License-Identifier: GPL-2.0+ */

/*
 * Copyright (C) 2015-2020, Cortina-Access Incorporation.
 * Author: Jason Li <jason.li@cortina-access.com>
 *
 * Parallel NAND Controller Driver for CAxxxx based SoCs
 */

#ifndef _CA_NAND_H_
#define _CA_NAND_H_

#define	NCNT_EMPTY_OOB    		0x3FF
#define	NCNT_512P_OOB    		0x0F
#define	NCNT_2kP_OOB    		0x3F
#define	NCNT_4kP_OOB    		0x7F
#define	NCNT_M4kP_OOB    		0xdF
#define	NCNT_EMPTY_DATA    		0x3FFF
#define	NCNT_512P_DATA    		0x1FF
#define	NCNT_2kP_DATA    		0x7FF
#define	NCNT_4kP_DATA    		0xFFF
#define	NCNT_DATA_1    			0x0
#define	NCNT_DATA_2    			0x1
#define	NCNT_DATA_3    			0x2
#define	NCNT_DATA_4    			0x3
#define	NCNT_DATA_5    			0x4
#define	NCNT_DATA_6    			0x5
#define	NCNT_DATA_7    			0x6
#define	NCNT_DATA_8    			0x7

#define	NCNT_EMPTY_ADDR    		0x7
#define	NCNT_ADDR_5	    		0x4
#define	NCNT_ADDR_4	    		0x3
#define	NCNT_ADDR_3	    		0x2
#define	NCNT_ADDR_2	    		0x1
#define	NCNT_ADDR_1	    		0x0
#define	NCNT_EMPTY_CMD    		0x3
#define	NCNT_CMD_3    			0x2
#define	NCNT_CMD_2    			0x1
#define	NCNT_CMD_1    			0x0

#define	NFLASH_WiDTH8              	0x0
#define	NFLASH_WiDTH16             	0x1
#define	NFLASH_WiDTH32             	0x2
#define NFLASH_CHIP0_EN            	0x0
#define NFLASH_CHIP1_EN            	0x1
//#define       NFLASH_DIRECT              0x00004000
//#define       NFLASH_INDIRECT            0x00000000

#define	ECC_DONE             		0x1	//ECC generation complete
#define	NF_RESET             		0x1	//ECC generation complete
#define	FIFO_CLR             		0x1	//ECC generation complete
#define	ECC_CLR             		0x1	//ECC generation complete
#define	FLASH_GO             		0x1	//ECC generation complete

// ECC comparison result,
#define	ECC_UNCORRECTABLE       	0x3
#define	ECC_1BIT_DATA_ERR       	0x1
#define	ECC_1BIT_ECC_ERR        	0x2
#define	ECC_NO_ERR             		0x0

#define	ECC_ENABLE           		0x1	//ECC generation complete
#define	ECC_GEN_256          		0x0	//NAND Flash ECC Generation Mode 256
#define	ECC_GEN_512          		0x1	//NAND Flash ECC Generation Mode 512
#define	ECC_PAUSE_EN            	0x1

#define	FLASH_RD             		0x2
#define	FLASH_WT             		0x3

#define	FLASH_SERIAL  	           	0x0	//Serial Flash
#define	FLASH_PARALLEL             	0x1	//Parallel Flash
#define	FLASH_NAND_512P            	0x4	//NAND Flash with 512B page size
#define	FLASH_NAND_2KP             	0x5	//NAND Flash with 2KB page size
#define	FLASH_NAND_4KP             	0x6	//Samsung NAND Flash with 4KB page size
#define	FLASH_NAND_M4KP            	0x7	//Micron NAND Flash with 4KB page size
#define	FLASH_NAND_8KP             	0x7	//Samsung NAND Flash with 8KB page size

#define	FLASH_WIDTH8_STM          	0x0	//8 bits width of flash for NAND and Parallel flash, but STMicroelectronic-compatible for Serial flash
#define	FLASH_WIDTH16_AT          	0x1	//16 bits width of flash for NAND and Parallel flash, but Atmel-Compatible for Serial flash

#define	FLASH_SIZE_STM16MB          	0x0
#define	FLASH_SIZE_STM32MB          	0x1

#define	FLASH_SIZE_AT1MB 	       	0x0
#define	FLASH_SIZE_AT2MB	       	0x1
#define	FLASH_SIZE_AT8MB	       	0x2
#define	FLASH_SIZE_AT16MB	       	0x3

#define	FLASH_SIZE_NP512_32MB        	0x0
#define	FLASH_SIZE_NP512_64MB        	0x1
#define	FLASH_SIZE_NP512_128MB       	0x2

#define	FLASH_SIZE_NP2K_128MB        	0x0
#define	FLASH_SIZE_NP2K_256MB        	0x1
#define	FLASH_SIZE_NP2K_512MB       	0x2
#define	FLASH_SIZE_NP2K_1GMB	       	0x3

#define	FLASH_SIZE_NP4K_1GMB        	0x0
#define	FLASH_SIZE_NP4K_2GMB        	0x1
#define	FLASH_SIZE_NP4K_4GMB	       	0x2

#define FLASH_SIZE_NP8K_CONF		0x4

//bch
#define	BCH_ENABLE	       		0x1
#define	BCH_DISABLE	       		0x0
#define	BCH_DECODE	       		0x1
#define	BCH_ENCODE	       		0x0
// BCH ECC comparison result,
#define	BCH_UNCORRECTABLE       	0x3
#define	BCH_CORRECTABLE_ERR         	0x2
#define	BCH_NO_ERR             		0x1
#define	BCH_ING		             	0x0

//#define BCH_ERR_CAP_8_512             0x0
//#define BCH_ERR_CAP_12_512            0x1

#define BCH_ERR_CAP_8		0x0
#define BCH_ERR_CAP_16		0x1
#define BCH_ERR_CAP_24		0x2
#define BCH_ERR_CAP_40		0x3

#define BCH_GF_PARAM_M		14
#define BCH_DATA_UNIT		1024

#define FIFO_CMD_READ	2
#define FIFO_CMD_WRITE	3

#define	FLASH_CLR_FIFO             	0x8000

#define	STS_WP             		0x80
#define	STS_READY             		0x40
#define	STS_TRUE_READY             	0x40
#define	NFLASH_ENABLE             	0x00000004

#define FDMA_DEPTH			3
#define FDMA_DESC_NUM			(1<<FDMA_DEPTH)


#if !defined(__ASSEMBLER__) && !defined(__ASSEMBLY__)

typedef struct tx_descriptor_t {
	unsigned int buf_adr;	/* data buffer address */
	union tx_word1_t {
		struct {
#ifdef CS_BIG_ENDIAN
			unsigned int own:1;	/* bits 31:31 */
			unsigned int share_rsrvd:1;	/* bits 30 */
			unsigned int cache_rsrvd:1;	/* bits 29 */
			unsigned int sof_eof_rsrvd:2;	/* bits 28:27 */
			unsigned int sgm_rsrvd:5;	/* bits 26:22 */
			unsigned int desccnt:6;	/* bits 21:16 */
			unsigned int buf_size:16;	/* bits 15:0 */
#else				/* CS_LITTLE_ENDIAN */
			unsigned int buf_adr_hi:8;	/* bits 7:0 */
			unsigned int buf_len:16;	/* bits 23:8 */
			unsigned int sgm:1;	/* bits 24 */
			unsigned int rsrvd:6;	/* bits 30:25 */
			unsigned int own:1;	/* bits 31:31 */
#endif
		} bf;
		unsigned int wrd;
	} word1;

} DMA_SSP_TX_DESC_T;

typedef struct rx_descriptor_t {
	unsigned int buf_adr;	/* data buffer address */
	union rx_word1_t {
		struct {
#ifdef CS_BIG_ENDIAN
			unsigned int own:1;	/* bits 31 */
			unsigned int share_rsrvd:1;	/* bits 30 */
			unsigned int cache_rsrvd:1;	/* bits 29 */
			unsigned int rqsts_rsrvd:7;	/* bits 28:22 */
			unsigned int desccnt:6;	/* bits 21:16 */
			unsigned int buf_size:16;	/* bits 15:0 */
#else				/* CS_LITTLE_ENDIAN */
			unsigned int buf_adr_hi:8;	/* bits 7:0 */
			unsigned int buf_len:16;	/* bits 23:8 */
			unsigned int rsrvd:7;	/* bits 30:24 */
			unsigned int own:1;	/* bits 31:31 */
#endif
		} bf;
		unsigned int wrd;
	} word1;

} DMA_SSP_RX_DESC_T;
#endif				/* !__ASSEMBLER__ */

#endif /* _CA_NAND_H_ */
