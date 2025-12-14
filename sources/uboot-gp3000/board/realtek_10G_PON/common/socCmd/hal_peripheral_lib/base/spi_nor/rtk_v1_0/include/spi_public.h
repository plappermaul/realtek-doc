#include "DW_common.h"

/* --------------------------------------------------------------------
**
** File     : SPI_public.h
** Created  : $Date: 2008/10/30 $
** Abstract : This is the public header file for the SPI
**            low-level-driver
**
** --------------------------------------------------------------------
*/

/****h* drivers.SPI/SPI.api
 * NAME
 *  SPI API overview
 * DESCRIPTION
 *  This section gives an overview of the SPI software driver
 *  Application Programming Interface (API).
 * SEE ALSO
 *  dmac.data, dmac.functions
 ***/

/****h* SPI.api/SPI.data_types
 * NAME
 *  DW_ahb_SPI data types and definitions
 * DESCRIPTION
 *  The data types below are used as function arguments for the DMA
 *  Controller API. Users of this driver must pass the relevant data
 *  types below to the API function that is being used.
 *
 *    - enum spi_command
 *    - struct dw_dmac_channel_config
 *
 * SEE ALSO
 *  SPI.api, dmac.configuration, dmac.command, dmac.status,
 *  SPI.interrupt
 ***/

/****h* SPI.api/dmac.configuration
 * NAME
 *  SPI configuration functions
 * DESCRIPTION
 *  SPI driver are listed
 *  below:
 *
 *
 * SEE ALSO
 *  SPI.api, dmac.data_types, dmac.command, dmac.status,
 *  SPI.interrupt
 ***/

/****h* SPI.api/spi.command
 * NAME
 *  SPI command functions
 * DESCRIPTION
 *  An API command function causes some functional behavior
 *  driver are list below:
 *    
 *    - spi_chip_erase()
 *    - spi_se_erase()
 *    - spi_set_status()
 *    - spi_en_write()
 *    - spi_dis_write()
 *    - spi_set_protect_mem()
 *    - spi_set_write_disable()
 *    - spi_set_write_enable()
 *    //- spi_set_en4k()
 *    //- spi_set_ex4k()
 *    - spi_enter_dp()
 *    - spi_release_dp()
 *    - spi_write_byte()
 *    - spi_read_byte()
 *    - spi_write_word()
 *    - spi_read_word()
 *    - spi_fastread_byte()
 *    - spi_fastread_word()
 *
 * SEE ALSO
 *  dmac.api, dmac.data_types, dmac.configuration, dmac.status,
 *  dmac.interrupt
 ***/

/****h* SPI.api/SPI.status
 * NAME
 *  SPI status functions
 * DESCRIPTION
 *  An API status function returns the status information about
 *  the current state of the SPI controller and/or the driver. 
 *  The driver are listed below:
 *   
 *    - spi_init() 
 *    - spi_get_status()
 *    - spi_read_id()
 *    - spi_read_ect_id()
 *    - spi_read_mf_id()
 *    - spi_wait_busy()
  *  
 * SEE ALSO
 *  dmac.api, dmac.data_types, dmac.configuration, dmac.command,
 *  dmac.interrupt
 ***/


/****d* spi.data/spi_command
 * DESCRIPTION
 *  This data type is used to describe the SPI controller's command
 *  number.  This data type is used by many of the API functions in
 *  the driver.
 */
enum spi_command {
      WRSR          = 0x01,
      PP            = 0x02,
      READ          = 0x03,
      WRDI          = 0x04,
      RDSR          = 0x05,
      WREN          = 0x06,
      FAST_READ     = 0x0b,
      Parallel_mode = 0x55,
      REMS          = 0x90, 
      RDID          = 0x9f,
      EN4K          = 0Xa5,
      RES           = 0xab,
      EX4K          = 0xb5,
      DP            = 0xb9,
      BE            = 0xc7,
      SE            = 0xd8
};
/*****/

/***** spi.data/spi_protect_mem
 * DESCRIPTION
 *  This data type is used to describe the protect memory size
 */
enum spi_protect_mem {
      SEC_NONE      = 0x00,
      SEC_64        = 0x01,
      SEC_63_62     = 0x02,
      SEC_63_60     = 0x03,
      SEC_63_56     = 0x04,
      SEC_63_48     = 0x05,
      SEC_63_32     = 0x06,
      SEC_ALL       = 0x07
};
/*****/



/***** spi.functions/spi_chip_erase() in spi_driver.s
 * DESCRIPTION
 *  This function is used to read the SPI controller. 
 *  ARGUMENTS
 *  dev         -- DMA controller device handle
 * SEE ALSO
 *  dw_device
 * SOURCE
 */
void spi_chip_erase(struct dw_device *dev);
/*****/

/***** spi.functions/spi_se_erase() 
 * DESCRIPTION
 *  This function is used to read the SPI controller. 
 *  ARGUMENTS
 *  dev         -- DMA controller device handle
 *  addr        -- erase address
 * SEE ALSO
 *  dw_device
 * SOURCE
 */
void spi_se_erase(struct dw_device *dev, uint32_t addr);
/*****/
/***** spi.functions/spi_set_status() in spi_driver.s
 * DESCRIPTION
 *  This function is used to read the SPI controller. 
 *  ARGUMENTS
 *  dev         -- DMA controller device handle
 *  data        -- the {1'b, program, BP2, BP1, BP0, WEL, WIP}
 * RETURN VALUE
 *  -DW_EIO     -- if an error occurred
 * SEE ALSO
 *  dw_device
 * SOURCE
 */
void spi_set_status(struct dw_device *dev, int data);
/*****/

/***** spi.functions/spi_dis_write() in spi_driver.s
 * DESCRIPTION
 *  This function is used to read the SPI controller. 
 *  ARGUMENTS
 *  dev         -- DMA controller device handle
 *  data        -- the {1'b, program, BP2, BP1, BP0, WEL, WIP}
 * RETURN VALUE
 *  -DW_EIO     -- if an error occurred
 * SEE ALSO
 *  dw_device
 * SOURCE
 */
void spi_dis_write(struct dw_device *dev);
/*****/

/***** spi.functions/spi_enable_write() in spi_driver.s
 * DESCRIPTION
 *  This function is used to read the SPI controller. 
 *  ARGUMENTS
 *  dev         -- DMA controller device handle
 *  data        -- the {1'b, program, BP2, BP1, BP0, WEL, WIP}
 * RETURN VALUE
 *  -DW_EIO     -- if an error occurred
 * SEE ALSO
 *  dw_device
 * SOURCE
 */
void spi_enable_write(struct dw_device *dev);
/*****/

/***** spi.functions/spi_set_protect_mem()
 * DESCRIPTION
 *  This function is used to read the SPI controller. 
 *  ARGUMENTS
 *  dev               -- DMA controller device handle
 *  spi_protect_mem   -- enum data 
 * RETURN VALUE
 *  -DW_EIO     -- if an error occurred
 * SEE ALSO
 *  dw_device
 * SOURCE
 */
void spi_set_protect_mem(struct dw_device *dev, enum spi_protect_mem size);
/*****/

/***** spi.functions/spi_enter_dp()
 * DESCRIPTION
 *  This function is used to read the SPI controller. 
 *  ARGUMENTS
 *  dev         -- DMA controller device handle
 * SEE ALSO
 *  dw_device
 * SOURCE
 */
void spi_enter_dp();
/*****/

/***** spi.functions/spi_set_write_disable()
 * DESCRIPTION
 *  This function is used to disable SPI write. 
 *  ARGUMENTS
 *  dev         -- DMA controller device handle
 * SEE ALSO
 *  dw_device
 * SOURCE
 */
void spi_set_write_disable(struct dw_device *dev);
/*****/

/***** spi.functions/spi_set_write_enable()
 * DESCRIPTION
 *  This function is used to enable SPI write enable.
 *  ARGUMENTS
 *  dev         -- DMA controller device handle
 * SEE ALSO
 *  dw_device
 * SOURCE
 */
void spi_set_write_enable(struct dw_device *dev);
/*****/

/***** spi.functions/spi_set_en4k()
 * DESCRIPTION
 *  This function is used to setv SPI EN4K command.
 *  ARGUMENTS
 *  dev         -- DMA controller device handle
 * SEE ALSO
 *  dw_device
 * SOURCE
 */
//void spi_set_en4k(struct dw_device *dev);

/***** spi.functions/spi_set_ex4k()
 * DESCRIPTION
 *  This function is used to setv SPI EX4K command.
 *  ARGUMENTS
 *  dev         -- DMA controller device handle
 * SEE ALSO
 *  dw_device
 * SOURCE
 */
//void spi_set_ex4k(struct dw_device *dev);

/***** spi.functions/spi_release_dp()
 * DESCRIPTION
 *  This function is used to read the SPI controller. 
 *  ARGUMENTS
 *  dev         -- DMA controller device handle
 * RETURN VALUE
 *  0           -- if successful
 *  -DW_EIO     -- if an error occurred
 * SEE ALSO
 *  dw_device
 * SOURCE
 */
void spi_release_dp(struct dw_device *dev);
/*****/

/***** spi.functions/spi_init() in spi_driver.s
 * DESCRIPTION
 *  This function is used to init SPI controller.
 *  for wirte control signal and sec_prot signals for write data. 
 *
 *  ARGUMENTS
 *  dev         -- DMA controller device handle
 * RETURN VALUE
 *  none 
 */
void spi_init(struct dw_device *dev);
/*****/

/***** spi.functions/spi_get_status() in spi_driver.s
 * DESCRIPTION
 *  This function is used to read the SPI controller. 
 *  ARGUMENTS
 *  dev         -- DMA controller device handle
 * RETURN VALUE
 *  int         -- value of status regrister(SR:0x28)
 */
int spi_get_stataus(struct dw_device *dev);
/*****/


/***** spi.functions/spi_read_id()
 * DESCRIPTION
 *  This function is used to read the SPI controller. 
 *  ARGUMENTS
 *  dev         -- DMA controller device handle
 * RETURN VALUE
 *  int         -- the ID of SPI
 * SEE ALSO
 *  dw_device
 * SOURCE
 */
int spi_read_id(struct dw_device *dev);
/*****/

/***** spi.functions/spi_read_ect_id()
 * DESCRIPTION
 *  This function is used to read the electronic id. 
 *  ARGUMENTS
 *  dev         -- DMA controller device handle
 * RETURN VALUE
 *  int         -- the ID of SPI
 * SEE ALSO
 *  dw_device
 * SOURCE
 */
int spi_read_ect_id(struct dw_device *dev);
/*****/

/***** spi.functions/spi_read_mf_id()
 * DESCRIPTION
 *  This function is used to read the electronic manufacturer
 *  & device ID.
 *  ARGUMENTS
 *  dev         -- DMA controller device handle
 * RETURN VALUE
 *  int         -- the ID of SPI
 * SEE ALSO
 *  dw_device
 * SOURCE
 */
int spi_read_mf_id(struct dw_device *dev, int form);
/*****/

/***** spi.functions/spi_wait_busy()
 * DESCRIPTION
 *  This function is used to read the SPI controller. 
 *  ARGUMENTS
 *  dev         -- DMA controller device handle
 * SEE ALSO
 *  dw_device
 * SOURCE
 */
void spi_wait_busy(struct dw_device *dev);
/*****/

/***** spi.functions/spi_write_byte();
 * DESCRIPTION
 *  This function is used to write data to SPI. 
 *  ARGUMENTS
 *  dev         -- DMA controller device handle
 * SEE ALSO
 *  dw_device
 * SOURCE
 */
void spi_write_byte(struct dw_device *dev, uint32_t addr, uint8_t wdata);
/*****/

/***** spi.functions/spi_read_byte();
 * DESCRIPTION
 *  This function is used to read data from SPI. 
 *  ARGUMENTS
 *  dev         -- DMA controller device handle
 * SEE ALSO
 *  dw_device
 * SOURCE
 */
int spi_read_byte(struct dw_device *dev, uint32_t addr);
/*****/

/***** spi.functions/spi_write_word();
 * DESCRIPTION
 *  This function is used to write data to SPI. 
 *  ARGUMENTS
 *  dev         -- DMA controller device handle
 * SEE ALSO
 *  dw_device
 * SOURCE
 */
void spi_write_word(struct dw_device *dev, uint32_t addr, 
                    int wdata, uint32_t le);
/*****/

/***** spi.functions/spi_read_word();
 * DESCRIPTION
 *  This function is used to read data from SPI. 
 *  ARGUMENTS
 *  dev         -- DMA controller device handle
 * SEE ALSO
 *  dw_device
 * SOURCE
 */
int spi_read_word(struct dw_device *dev, uint32_t addr,uint32_t le);
/*****/

/***** spi.functions/spi_fastread_byte();
 * DESCRIPTION
 *  This function is used to read data from SPI. 
 *  ARGUMENTS
 *  dev         -- DMA controller device handle
 * SEE ALSO
 *  dw_device
 * SOURCE
 */
int spi_fastread_byte(struct dw_device *dev, uint32_t addr);
/*****/

/***** spi.functions/spi_fastread_word();
 * DESCRIPTION
 *  This function is used to read data from SPI. 
 *  ARGUMENTS
 *  dev         -- DMA controller device handle
 * SEE ALSO
 *  dw_device
 * SOURCE
 */
int spi_fastread_word(struct dw_device *dev, uint32_t addr, uint32_t le );
/*****/


