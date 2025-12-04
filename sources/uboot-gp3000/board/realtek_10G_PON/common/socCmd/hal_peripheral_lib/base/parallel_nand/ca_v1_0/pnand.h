#ifndef _PNAND_H_
#define _PNAND_H_

#define	NF_CNT_512P_OOB    		0x0F
#define	NF_CNT_2kP_OOB    		0x3F
#define	NF_CNT_4kP_OOB    		0x7F
#define	NF_CNT_M4kP_OOB    		0xdF
#define	NF_CNT_EMPTY_OOB    		0x3FF

#define	NF_CNT_DATA_1    		0x0
#define	NF_CNT_DATA_2    		0x1
#define	NF_CNT_DATA_3    		0x2
#define	NF_CNT_DATA_4    		0x3
#define	NF_CNT_DATA_5    		0x4
#define	NF_CNT_DATA_6    		0x5
#define	NF_CNT_DATA_7    		0x6
#define	NF_CNT_DATA_8    		0x7
#define	NF_CNT_512P_DATA    		0x1FF
#define	NF_CNT_2kP_DATA    		0x7FF
#define	NF_CNT_4kP_DATA    		0xFFF
#define	NF_CNT_EMPTY_DATA    		0x3FFF

#define	NF_CNT_ADDR_1	    		0x0
#define	NF_CNT_ADDR_2	    		0x1
#define	NF_CNT_ADDR_3	    		0x2
#define	NF_CNT_ADDR_4	    		0x3
#define	NF_CNT_ADDR_5	    		0x4
#define	NF_CNT_EMPTY_ADDR    		0x7

#define	NF_CNT_CMD_1    		0x0
#define	NF_CNT_CMD_2    		0x1
#define	NF_CNT_CMD_3    		0x2
#define	NF_CNT_EMPTY_CMD    		0x3

#define	NF_ACCESS_WiDTH8              	0x0	//BYTE
#define	NF_ACCESS_WiDTH16             	0x1	//WORD
#define	NF_ACCESS_WiDTH32             	0x2	//DOUBLE_WORD

#define NF_ACCESS_CHIP0_EN            	0x0
#define NF_ACCESS_CHIP1_EN            	0x1

#define	ECC_DONE             		0x1	//ECC generation complete
#define	NF_RESET             		0x1	//ECC generation complete
#define	FIFO_CLR             		0x1	//ECC generation complete
#define	ECC_CLR             		0x1	//ECC generation complete

// ECC comparison result,
#define	ECC_NO_ERR             		0x0
#define	ECC_1BIT_DATA_ERR       	0x1
#define	ECC_1BIT_ECC_ERR        	0x2
#define	ECC_UNCORRECTABLE       	0x3

#define ECC_DISABLE			0x0	//ECC disable
#define	ECC_ENABLE           		0x1	//ECC generation complete

#define	ECC_GEN_256          		0x0	//NAND Flash ECC Generation Mode 256
#define	ECC_GEN_512          		0x1	//NAND Flash ECC Generation Mode 512

#define	ECC_PAUSE_EN            	0x1

#define	FLASH_GO             		0x1
#define	FLASH_RD             		0x2	//read
#define	FLASH_WT             		0x3	//Write

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

// jenfeng8K
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

//#define BCH_ERR_CAP_8_512		0x0
//#define BCH_ERR_CAP_12_512		0x1

#define BCH_ERR_CAP_8			0x0
#define BCH_ERR_CAP_16			0x1
#define BCH_ERR_CAP_24			0x2
#define BCH_ERR_CAP_40			0x3

#define BCH_GF_PARAM_M			14
#define BCH_DATA_UNIT			1024

#define FIFO_CMD_READ			2
#define FIFO_CMD_WRITE			3

#define	FLASH_CLR_FIFO             	0x8000

#define	STS_WP             		0x80
#define	STS_READY             		0x40
#define	STS_TRUE_READY             	0x40

#define FDMA_DEPTH			3
#define FDMA_DESC_NUM			(1<<FDMA_DEPTH)

#define	NF_STS_PCNT			200
#define	NF_STS_MSK			0xF00
#define	NF_STS_VAL			0

#ifdef TEST_ON_G3_ASIC_SIM
#define NF_START_BCNT   10
#else
// wait 2000us in  flash_reset for reg_wait, too long to run on ASIC simulator
#define	NF_START_BCNT	2000
#endif

#define NF_ECC_HM_BCH			1

 typedef enum
{
	ECC_HM_256B_3B	= 0,
	ECC_HM_512B_3B,
	ECC_BCH_8,
	ECC_BCH_16,
	ECC_BCH_24,
	ECC_BCH_40,
} ecc_type_list;

#define	PAGE512_SIZE              	0x200
#define	PAGE512_OOB_SIZE              	0x10
#define	PAGE2K_SIZE              	0x800


//extern void g2_nand_init(void);



typedef struct tx_descriptor_t {
	unsigned int buf_adr;	/* data buffer address */
	union tx_word1_t
	{
  		struct {
#ifdef CS_BIG_ENDIAN
    			unsigned int own        	:  1 ;  /* bits 31:31 */
    			unsigned int share_rsrvd        :  1 ;  /* bits 30 */
    			unsigned int cache_rsrvd 	:  1 ;  /* bits 29 */
    			unsigned int sof_eof_rsrvd   	:  2 ;  /* bits 28:27 */
    			unsigned int sgm_rsrvd   	:  5 ;  /* bits 26:22 */
    			unsigned int desccnt     	:  6 ;  /* bits 21:16 */
    			unsigned int buf_size           :  16 ; /* bits 15:0 */
#else /* CS_LITTLE_ENDIAN */
    			unsigned int buf_adr_hi           :  8 ; /* bits 7:0 */
    			unsigned int buf_len     		:  16 ;  /* bits 23:8 */
    			unsigned int sgm   			:  1 ;  /* bits 24 */
    			unsigned int rsrvd   		:  6 ;  /* bits 30:25 */
    			unsigned int own        	:  1 ;  /* bits 31:31 */
#endif
  		} bf ;
  		unsigned int     wrd ;
	} word1;

} DMA_SSP_TX_DESC_T;


typedef struct rx_descriptor_t {
	unsigned int buf_adr;	/* data buffer address */
	union rx_word1_t
	{
  		struct {
#ifdef CS_BIG_ENDIAN
    			unsigned int own        	:  1 ;  /* bits 31 */
    			unsigned int share_rsrvd        :  1 ;  /* bits 30 */
    			unsigned int cache_rsrvd 	:  1 ;  /* bits 29 */
    			unsigned int rqsts_rsrvd   	:  7 ;  /* bits 28:22 */
    			unsigned int desccnt     	:  6 ;  /* bits 21:16 */
    			unsigned int buf_size           :  16 ; /* bits 15:0 */
#else /* CS_LITTLE_ENDIAN */
    			unsigned int buf_adr_hi           :  8 ; /* bits 7:0 */
    			unsigned int buf_len     		:  16 ;  /* bits 23:8 */
    			unsigned int rsrvd   		:  7 ;  /* bits 30:24 */
    			unsigned int own        	:  1 ;  /* bits 31:31 */
#endif
  		} bf ;
  		unsigned int     wrd ;
	} word1;


} DMA_SSP_RX_DESC_T ;

#endif /* _PARA_NAND_H_ */
