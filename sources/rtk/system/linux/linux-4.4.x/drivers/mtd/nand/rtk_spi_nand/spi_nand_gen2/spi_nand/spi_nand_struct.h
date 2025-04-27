#ifndef SPI_NAND_STRUCT_H
#define SPI_NAND_STRUCT_H

#include <ecc/ecc_ctrl.h>

/* Version Info. */
#define SYM_SPI_NAND_VER    0x534E0B02

/***********************************************
  * Driver Section Definitions
  ***********************************************/
#ifndef SECTION_SPI_NAND
    #define SECTION_SPI_NAND
#endif
#ifndef SECTION_SPI_NAND_DATA
    #define SECTION_SPI_NAND_DATA
#endif


struct  spi_nand_flash_info_s;
typedef struct spi_nand_flash_info_s spi_nand_flash_info_t;
/*********************************************
  * SPI NAND FLASH PARAMETERS
  * The latest general spi nand flash & ecc driver
  *********************************************/
typedef void (spi_nand_pio_read_t)(spi_nand_flash_info_t *info, void *addr, u32_t wr_bytes, u32_t blk_pge_addr, u32_t col_addr);
typedef s32_t (spi_nand_pio_write_t)(spi_nand_flash_info_t *info, void *addr, u32_t wr_bytes, u32_t blk_pge_addr, u32_t col_addr);
typedef void (spi_nand_page_read_t)(spi_nand_flash_info_t *info, void *dma_addr, u32_t blk_pge_addr);
typedef s32_t (spi_nand_page_write_t)(spi_nand_flash_info_t *info, void *dma_addr, u32_t blk_pge_addr);
typedef s32_t (spi_nand_page_read_write_ecc_t)(spi_nand_flash_info_t *info, void *dma_addr, u32_t blk_pge_addr, void *p_eccbuf);
typedef s32_t (spi_nand_erase_block_t) (spi_nand_flash_info_t *info, u32_t blk_pge_addr);
typedef s32_t (spi_nand_chk_eccStatus_t)(void);
typedef spi_nand_flash_info_t* (spi_nand_probe_t) (void);
typedef u32_t (spi_nand_get_feature_reg_t) (u32_t cs, u32_t feature_addr);
typedef void (spi_nand_set_feature_reg_t) (u32_t cs, u32_t feature_addr, u32_t setting);
typedef u32_t (spi_nand_read_id_t) (u32_t cs, u32_t man_addr, u32_t w_io_len, u32_t r_io_len);

typedef struct spi_nand_cmd_info_s{
    u8_t w_cmd;
    u8_t w_addr_io;
    u8_t w_data_io;
    u8_t r_cmd;
    u8_t r_addr_io;
    u8_t r_data_io;
    u8_t r_dummy_cycles;
}spi_nand_cmd_info_t;

typedef struct spi_nand_model_info_s{
    spi_nand_pio_read_t            *_pio_read;
    spi_nand_pio_write_t           *_pio_write;
    spi_nand_page_read_t           *_page_read;
    spi_nand_page_write_t          *_page_write;
    spi_nand_page_read_write_ecc_t *_page_read_ecc;
    spi_nand_page_read_write_ecc_t *_page_write_ecc;
    spi_nand_erase_block_t         *_block_erase;
    fpv_u32_t                      *_wait_spi_nand_ready;
}spi_nand_model_info_t;

struct spi_nand_flash_info_s {
    u16_t        man_id;               //Manufacture id
    u16_t        dev_id;               //Device id
    u32_t        _num_block:3;         //num_block = 512*(1<<_num_block); (512~8192)
    u32_t        _num_page_per_block:3;//num_page_per_block = 32*(1<<_num_page_per_block+1); (32~1024)
    u32_t        _page_size:2;         //page_size = 1024*(1<<_page_size); (1024~8192)
    u32_t        _spare_size:3;        //spare_size = 32*(1<<_spare_size); (32~1024)
    u32_t        _oob_size:7;          //oob_size = 4*(_oob_size+1); (4~512)
    u32_t        _ecc_ability:4;       //bch_ability = 6*(_ecc_ablility+1); (6~96)
    u32_t        mbz:10;               //reserved between gen1 and gen2: 1+2+2+5
    ecc_encode_t *_ecc_encode;
    ecc_decode_t *_ecc_decode;
    fpv_u32_t    *_reset;
    spi_nand_cmd_info_t    *_cmd_info;
    spi_nand_model_info_t  *_model_info;
};

#define UNIT_NUM_BLK    (512)
#define UNIT_NUM_PAGE   (32)
#define UNIT_PAGE_SIZE  (1024)
#define UNIT_SPARE_SIZE (32)
#define UNIT_OOB_SIZE   (4)
#define UNIT_ECC_BITS   (6)

#define SNAF_MODEL_NUM_BLK_512          (0) //512*(1<<0)
#define SNAF_MODEL_NUM_BLK_1024         (1) //512*(1<<1)
#define SNAF_MODEL_NUM_BLK_2048         (2) //512*(1<<2)
#define SNAF_MODEL_NUM_BLK_4096         (3) //512*(1<<3)
#define SNAF_MODEL_NUM_BLK_8192         (4) //512*(1<<3)

#define SNAF_MODEL_NUM_PAGE_32          (0) // 32*(1<<0)
#define SNAF_MODEL_NUM_PAGE_64          (1) // 32*(1<<1)
#define SNAF_MODEL_NUM_PAGE_128         (2) // 32*(1<<2)
#define SNAF_MODEL_NUM_PAGE_256         (3) // 32*(1<<3)
#define SNAF_MODEL_NUM_PAGE_512         (4) // 32*(1<<4)
#define SNAF_MODEL_NUM_PAGE_1024        (5) // 32*(1<<5)

#define SNAF_MODEL_PAGE_SIZE_1024B      (0) //1024*(1<<0)
#define SNAF_MODEL_PAGE_SIZE_2048B      (1) //1024*(1<<1)
#define SNAF_MODEL_PAGE_SIZE_4096B      (2) //1024*(1<<2)
#define SNAF_MODEL_PAGE_SIZE_8192B      (3) //1024*(1<<2)

#define SNAF_MODEL_SPARE_SIZE_32B       (0) //  32*(1<<0)
#define SNAF_MODEL_SPARE_SIZE_64B       (1) //  32*(1<<1)
#define SNAF_MODEL_SPARE_SIZE_128B      (2) //  32*(1<<2)
#define SNAF_MODEL_SPARE_SIZE_256B      (3) //  32*(1<<3)
#define SNAF_MODEL_SPARE_SIZE_512B      (4) //  32*(1<<4)
#define SNAF_MODEL_SPARE_SIZE_1024B     (5) //  32*(1<<5)

#define SNAF_MODEL_OOB_SIZE(oob_byte)   (((oob_byte)/UNIT_OOB_SIZE)-1)

#define SNAF_NUM_OF_PAGE_PER_BLK(info)  ((1<<info->_num_page_per_block) * UNIT_NUM_PAGE)
#define SNAF_NUM_OF_BLOCK(info)         ((1<<info->_num_block)* UNIT_NUM_BLK)
#define SNAF_PAGE_SIZE(info)            ((1<<info->_page_size) * UNIT_PAGE_SIZE)
#define SNAF_SPARE_SIZE(info)           ((1<<info->_spare_size) * UNIT_SPARE_SIZE)
#define SNAF_OOB_SIZE(info)             ((info->_oob_size+1) * UNIT_OOB_SIZE)
#define SNAF_RW_CMD(info)               info->_cmd_info

#define ECC_MODEL_6T                    ((6/UNIT_ECC_BITS)-1)
#define ECC_MODEL_12T                   ((12/UNIT_ECC_BITS)-1)
#define ECC_MODEL_18T                   ((18/UNIT_ECC_BITS)-1)
#define ECC_MODEL_24T                   ((24/UNIT_ECC_BITS)-1)
#define ECC_CORRECT_BITS(info)          ((info->_ecc_ability+1) * UNIT_ECC_BITS)

#define PAGE_SHF                        (6)
#define ELEMENT_OF_SNAF_INFO(info)       sizeof(info)/sizeof(spi_nand_flash_info_t)

#endif //SPI_NAND_STRUCT_H
