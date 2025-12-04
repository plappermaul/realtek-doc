#ifndef SPI_FLASH_DEFS
#define SPI_FLASH_DEFS

#define ps_DW_APB_SPI_FLASH_BASE BSP_PS_I_SSI_BASE

#define ps_CC_SPI_FLASH_NUM_SLAVES            1
#define ps_CC_SPI_FLASH_HAS_DMA               0
#define ps_CC_SPI_FLASH_DMA_TX_SGL_STATUS     1
#define ps_CC_SPI_FLASH_DMA_RX_SGL_STATUS     1
#define ps_CC_SPI_FLASH_RX_FIFO_DEPTH         0x20
#define ps_CC_SPI_FLASH_TX_FIFO_DEPTH         0x20
#define ps_CC_SPI_FLASH_RX_ABW                5
#define ps_CC_SPI_FLASH_TX_ABW                5
#define ps_CC_SPI_FLASH_INTR_POL              1
#define ps_CC_SPI_FLASH_INTR_IO               1
#define ps_CC_SPI_FLASH_INDIVIDUAL            0
#define ps_CC_SPI_FLASH_ID                    0x0
#define ps_CC_SPI_FLASH_HC_FRF                0
#define ps_CC_SPI_FLASH_DFLT_FRF              0x0
#define ps_CC_SPI_FLASH_DFLT_SCPOL            0x0
#define ps_CC_SPI_FLASH_DFLT_SCPH             0x0
#define ps_CC_SPI_FLASH_CLK_PERIOD            400
#define ps_CC_SPI_FLASH_VERSION_ID            0x28100617
#endif
