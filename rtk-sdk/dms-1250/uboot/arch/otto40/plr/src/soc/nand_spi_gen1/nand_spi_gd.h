#ifndef NAND_SPI_GD_H
#define NAND_SPI_GD_H

/***********************************************
  * Driver Section Definitions
  ***********************************************/
#ifndef SECTION_NAND_SPI
    #define SECTION_NAND_SPI 
#endif
#ifndef SECTION_NAND_SPI_DATA
    #define SECTION_NAND_SPI_DATA 
#endif


/***********************************************
  *  GD's ID definition
  ***********************************************/
#define MID_GD         (0xC8)
#define DID_GD5F1GQ4UAYIG (0xF1)
#define DID_GD5F1GQ4UBYIG (0xD1)


/********************************************
  Export Functions
  ********************************************/
nand_spi_flash_info_t *probe_gd_spi_nand_chip(void);

#endif //#ifndef NAND_SPI_GD_H

