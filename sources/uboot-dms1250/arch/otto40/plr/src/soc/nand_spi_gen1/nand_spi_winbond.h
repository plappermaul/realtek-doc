#ifndef NAND_SPI_WINBOND_H
#define NAND_SPI_WINBOND_H

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
  *  Winbond's ID definition
  ***********************************************/
#define MID_WINBOND         (0xEF)
#define DID_W25N01GV      (0xAA21)


/********************************************
  Export functions
  ********************************************/
nand_spi_flash_info_t *probe_winbond_spi_nand_chip(void);

#endif //#ifdef NAND_SPI_WINBOND_H

