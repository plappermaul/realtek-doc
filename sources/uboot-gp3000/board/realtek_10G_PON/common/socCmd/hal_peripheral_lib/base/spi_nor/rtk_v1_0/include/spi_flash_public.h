

#ifndef SPI_FLASH_PUBLIC_H
#define SPI_FLASH_PUBLIC_H


//#include "common/1.01a/DW_common.h"
#include "spi_flash_private.h"

#define PRINTF_INFO 1   // support printf() to display
#define PRINTF_DEBUG 0  // printf more information to debug
#define BIG_ENDIAN 0    // system endian , 

/* 
  * SPI Flash size definition
  */
#define SPI_BLOCK_SIZE 	65536  	/* 64KB */
#define SPI_SECTOR_SIZE 	4096  	/*  4KB */

/*For 8198E ARM*/
//#define SPI_FLASH_BASE_ADDR   0xE0000000

/*For 8197F MIPS*/
#define SPI_FLASH_BASE_ADDR   0xB0000000

#if PRINTF_DEBUG
  #define C_DEBUG 1
#else
  #define C_DEBUG 1
#endif

#if PRINTF_INFO 
  #define PRINTF_ON 1
#else
  #define PRINTF_ON 1
#endif

 
/* --------------------------------------------------------------------
**
** File     : SPI_public.h
** Created  : $Date: 2008/10/30 $
** Abstract : This is the public header file for the SPI
**            low-level-driver
**
** --------------------------------------------------------------------
*/

/****** drivers.SPI_FLASH/SPI_FLASH.api
 * NAME
 *  SPI API overview
 * DESCRIPTION
 *  This section gives an overview of the SPI software driver
 *  Application Programming Interface (API).
 * SEE ALSO
 *  SPI_FLASH.data, SPI_FLASH.functions
 ***/

/****** SPI_FLASH.api/SPI_FLASH.data_types
 * NAME
 *  DW_ahb_SPI data types and definitions
 * DESCRIPTION
 *  The data types below are used as function arguments for the DMA
 *  Controller API. Users of this driver must pass the relevant data
 *  types below to the API function that is being used.
 *
 *    - enum spi_flash_dr_number
 *
 * SEE ALSO
 *  SPI_FLASH.api, SPI_FLASH.configuration, SPI_FLASH.command, SPI_FLASH.status,
 ***/

/****** SPI_FLASH.api/SPI_FLASH.function     
 * NAME
 *  SPI configuration functions
 * DESCRIPTION
 *  SPI_FLASH driver are listed
 *  below:
 *    - spi_flash_enable()
 *    - spi_flash_disable()
 *    - spi_flash_set_eeprom_mode()/ spi_flash_set_rx_mode/ 
 *    - spi_flash_set_tx_mode()
 *    - spi_flash_setser()
 *    - spi_flash_setctrlr1()
 *    - spi_flash_setdr()
 *    - spi_flash_setbaudr()
 *    - spi_flash_getbaudr()
 *    - spi_flash_getdr()
 *    - spi_flash_getidr()
 *    - spi_flash_set_multi_ch()
 *    - spi_flash_set_dummy_cycle()
 *    - spi_flash_isStatus_busy()
 *    - spi_flash_wait_busy() 
 *
 *
 * SEE ALSO
 *  SPI_FLASH.api, SPI_FLASH.data_types, SPI_FLASH.command, SPI_FLASH.status,
 ***/

enum spi_flash_byte_num {
    DATA_BYTE         = 0,
    DATA_HALF         = 1,
    DATA_WORD         = 2        
};

enum spi_flash_dr_number {
    DR0               = 0 ,  
    DR1               = 1 ,
    DR2               = 2 ,
    DR3               = 3 ,
    DR4               = 4 ,
    DR5               = 5 ,
    DR6               = 6 ,
    DR7               = 7 ,
    DR8               = 8 ,
    DR9               = 9 ,
    DR10              = 10,
    DR11              = 11,
    DR12              = 12,
    DR13              = 13,
    DR14              = 14,
    DR15              = 15,
    DR16              = 16,
    DR17              = 17,
    DR18              = 18,
    DR19              = 19,
    DR20              = 20,
    DR21              = 21,
    DR22              = 22,
    DR23              = 23,
    DR24              = 24,
    DR25              = 25,
    DR26              = 26,
    DR27              = 27,
    DR28              = 28,
    DR29              = 29,
    DR30              = 30,
    DR31              = 31
};

/***** spi_flash.functions/spi_flash_enable()
 * DESCRIPTION
 *  This function is used to read the spi_flash controller. 
 *  ARGUMENTS
 *  dev         -- SPI_FLASH controller device handle
 * SEE ALSO
 *  spi_flash.function
 */
void spi_flash_enable(struct dw_device *dev);
/*****/

/***** spi_flash.functions/spi_flash_disable()
 * DESCRIPTION
 *  This function is used to disable SSIENR register. 
 *  ARGUMENTS
 *  dev         -- SPI_FLASH controller device handle
 * SEE ALSO
 *  dw_device
 * SOURCE
 */
void spi_flash_disable(struct dw_device *dev);
/*****/

/***** spi_flash.functions/spi_flash_geatidr()
 * DESCRIPTION
 *  This function is used to read IDR register
 *  ARGUMENTS
 *  dev         -- SPI_FLASH controller device handle
 * RETURN VALUE
 *  0           -- if successful
 *  -DW_EIO     -- if an error occurred
 * SEE ALSO
 *  spi_flash.function
 */
uint32_t spi_flash_getidr(struct dw_device *dev);
/*****/

/***** spi_flash.functions/spi_flash_set_eeprom_mode()
 * DESCRIPTION
 *   This function is used to set CTrlr0: TMOD register.
 *  ARGUMENTS
 *  dev         -- SPI_FLASH controller device handle
 * RETURN VALUE
 *  -
 * SEE ALSO
 *  spi_flash.function
 */
void spi_flash_set_eeprom_mode(struct dw_device *dev);
/*****/

/***** spi_flash.functions/spi_flash_set_transmit_mode()
 * DESCRIPTION
 *  This function is used to set CTrlr0: TMOD register.
 *  ARGUMENTS
 *  dev         -- SPI_FLASH controller device handle
 * RETURN VALUE
 * -
 * SEE ALSO
 *  spi_flash.function
 */
void spi_flash_set_tx_mode(struct dw_device *dev);
/*****/

/***** spi_flash.functions/spi_flash_set_receive_mode()
 * DESCRIPTION
 *  This function is used to set Ctrlr0: TMOD register.
 *  ARGUMENTS
 *  dev         -- SPI_FLASH controller device handle
 * RETURN VALUE
 *  -
 * SEE ALSO
 *  spi_flash.function
 */
void spi_flash_set_rx_mode(struct dw_device *dev);
/*****/

/***** spi_flash.functions/spi_flash_set_multi_ch()
 * DESCRIPTION
 *  This function is used to set the ctrlr0: {DATA_CH, ADDR_CH} register. 
 *  ARGUMENTS
 *  dev         -- DMA controller device handle
 * RETURN VALUE
 *  -
 * SEE ALSO
 *  spi_flash.function
 */
void spi_flash_set_multi_ch(struct dw_device *dev, uint32_t data_ch, uint32_t addr_ch);
/*****/

/***** spi_flash.functions/spi_flash_setctrlr1()
 * DESCRIPTION
 *  This function is used to set the ctrlr1 register. 
 *  ARGUMENTS
 *  dev         -- SPI_FLASH controller device handle
 * RETURN VALUE
 *  0           -- if successful
 *  -DW_EINVAL  -- if an error occurred
 * SEE ALSO
 *  spi_flash.function
 */
uint32_t spi_flash_setctrlr1(struct dw_device *dev, uint32_t num_frame);
/*****/

/***** spi_flash.functions/spi_flash_setdr()
 * DESCRIPTION
 *  This function is used to set the data to FIFO. 
 *  ARGUMENTS
 *  dev         -- SPI_FLASH controller device handle
 * RETURN VALUE
 *  0           -- if successful
 *  -DW_EINVAL  -- if num_frame is out the range
 * SEE ALSO
 *  spi_flash.function
 */
uint32_t spi_flash_setdr(struct dw_device *dev, enum spi_flash_dr_number dr_num,
                 uint32_t data, enum spi_flash_byte_num byte_num);
/*****/

/***** spi_flash.functions/spi_flash_setbaudr()
 * DESCRIPTION
 *  This function is used to set the Baudrate register. 
 *  ARGUMENTS
 *  dev         -- SPI_FLASH controller device handle
 * RETURN VALUE
 *  0           -- if successful
 *  -DW_EINVAL  -- if baud rate is out the range
 * SEE ALSO
 *  spi_flash.function
 */
uint32_t spi_flash_setbaudr(struct dw_device *dev, uint32_t baudrate);
/*****/

/***** spi_flash.functions/spi_flash_setser()
 * DESCRIPTION
 *  This function is used to set Ser register. 
 *  ARGUMENTS
 *  dev         -- SPI_FLASH controller device handle
 * RETURN VALUE
 *  0           -- if successful
 *  -DW_EINVAL  -- if baud rate is out the range
 * SEE ALSO
 *  spi_flash.function
 */
uint32_t spi_flash_setser(struct dw_device *dev, uint32_t ser_num);
/*****/

/***** spi_flash.functions/spi_flash_set_dummy_cycle()
 * DESCRIPTION
 *  This function is used to set the Auto_length controller. 
 *  ARGUMENTS
 *  dev         -- DMA controller device handle
 * RETURN VALUE
 *  0           -- if successful
 *  -DW_EINVAL  -- if dum_cycle is out the range
 * SEE ALSO
 *  spi_flash.function
 */
uint32_t spi_flash_set_dummy_cycle(struct dw_device *dev, uint32_t dum_cycle);
/*****/

/***** spi_flash.functions/spi_flash_getbaudr()
 * DESCRIPTION
 *  This function is used to get the baud_rate controller. 
 *  ARGUMENTS
 *  dev         -- SPI_FLASH controller device handle
 * RETURN VALUE
 *  -uint32_t   -- if successful
 * SEE ALSO
 *  spi_flash.function
 */
uint32_t spi_flash_getbaudr(struct dw_device *dev);
/*****/

/***** spi_flash.functions/spi_flash_getdr()
 * DESCRIPTION
 *  This function is used to read Data from RX FIFO. 
 *  ARGUMENTS
 *  dr_num      -- dr[0]~ dr[32] point to FIFO[0]
 *  byte_num    -- recevie data tyep: byte/ half/ word 
 * RETURN VALUE
 *  uint32_t    -- if successful
 * SEE ALSO
 *  spi_flash.function
 */
uint32_t spi_flash_getdr(struct dw_device *dev, enum spi_flash_dr_number dr_num, 
                        enum spi_flash_byte_num byte_num);
/*****/

/***** spi_flash.functions/spi_flash_isstatus_busy()
 * DESCRIPTION
 *  This function is used to read SR register:Busy. 
 *  ARGUMENTS
 *  dev         -- SPI_FLASH controller device handle
 * RETURN VALUE
 *  0           -- if device is not busy
 *  -DW_EBUSY   -- if device is on busy
 * SEE ALSO
 *  spi_flash.function
 */
uint32_t spi_flash_isstatus_busy(struct dw_device *dev);
/*****/

/***** spi_flash.functions/spi_flash_wait_nobusy()
 * DESCRIPTION
 *  This function is used to wait the SSI is not at busy state.
 *  ARGUMENTS
 *  dev         -- SPI_FLASH controller device handle
 * SEE ALSO
 *  spi_flash.function
 */
void spi_flash_wait_busy(struct dw_device *dev);
/*****/

/****** flash.api/flash.command
 * NAME
 *  FLASH command functions
 * DESCRIPTION
 *  An API command function causes some functional behavior
 *  driver are list below:
 *   
 *    - flash_tx_cmd()
 *    - flash_rx_cmd() 
 *    - flash_chip_erase()
 *    - flash_se_erase()
 *    - flash_set_status()
 *    - flash_enable_write()
 *    - flash_disable_write()
 *    //- flash_set_protect_mem()
 *    //- spi_set_en4k()
 *    //- spi_set_ex4k()
 *    //- flash_enter_dp()
 *    //- flash_release_dp()
 *    - flash_get_status()
 *    - flash_read_id()
 *    - flash_read()
 *    - flash_write()
 *    - flash_fastread()
 *    - flash_readx2()
 *    - flash_writex2()
 *    - flash_readx4()
 *    - flash_writex4()
 *
 * SEE ALSO
 *  SPI_FLASH.api, SPI_FLASH.data_types, SPI_FLASH.configuration, SPI_FLASH.status,
 ***/

/****** flash.api/flash.status
 * NAME
 *  SPI status functions
 * DESCRIPTION
 *  An API function is combined from complex flash.command and it's easy to use for users.
 *  The driver are listed below:
 *   
 *    - flash_init() 
 *    - flash_device_init()
 *    - flash_wait_busy()
 *  
 * SEE ALSO
 *  SPI_FLASH.api, SPI_FLASH.data_types, SPI_FLASH.configuration, SPI_FLASH.command,
 *  FLASH.command
 ***/


/***** flash.data/flash_protect_mem
 * DESCRIPTION
 *  This data type is used to describe the protect memory size
 * SEE ALSO
 *  flash.data
 */

//enum flash_protect_mem {
//      SEC_NONE      = 0x00,
//      SEC_64        = 0x01,
//      SEC_63_62     = 0x02,
//      SEC_63_60     = 0x03,
//      SEC_63_56     = 0x04,
//      SEC_63_48     = 0x05,
//      SEC_63_32     = 0x06,
//      SEC_ALL       = 0x07
//};
///*****/

/*****
 * The follow functions are in flash_driver.c
 ****/

/***** flash.functions/flash_tx_cmd()
 * DESCRIPTION
 *  This function is general used to write operation of flash(WRSR, WRDI, PP, ) 
 *  Users can use the function to operate flash 
 *  if the operation is not supporting in spi.function 
 *  ARGUMENTS
 *  dev         -- SPI_FLASH controller device handle
 *  cmd         -- Flash command 
 * SEE ALSO
 *  flash.function
 */
void flash_tx_cmd(struct dw_device *dev, uint8_t cmd);
/*****/

/***** flash.functions/flash_rx_cmd()
 * DESCRIPTION
 *  This function is general used to read operation of flash (read, rdid...)
 *  Users can use the function to operate flash and users need get data by userselves.
 *  if the operation is not supporting in spi.function 
 *  ARGUMENTS
 *  dev         -- SPI_FLASH controller device handle
 *  cmd         -- Flash command 
 * SEE ALSO
 *  flash.function
 */
void flash_rx_cmd(struct dw_device *dev, uint8_t cmd);
/*****/

/***** flash.functions/flash_chip_erase()
 * DESCRIPTION
 *  This function is used to earse the flash. 
 *  ARGUMENTS
 *  dev         -- SPI_FLASH controller device handle
 * SEE ALSO
 *  flash.function
 */
void flash_chip_erase(struct dw_device *dev);
/*****/

/***** flash.functions/flash_se_erase() 
 * DESCRIPTION
 *  This function is used section erase command to earse flash
 *  ARGUMENTS
 *  dev         -- SPI_FLASH controller device handle
 *  addr        -- erase address
 * SEE ALSO
 *  flash.function
 */
void flash_se_erase(struct dw_device *dev, uint32_t addr);
/*****/

/***** flash.functions/flash_set_status() 
 * DESCRIPTION
 *  This function is used to read the status of Flash
 *  ARGUMENTS
 *  dev         -- SPI_FLASH controller device handle
 *  data        -- the {1'b, program, BP2, BP1, BP0, WEL, WIP}
 *  byt_num     -- DATA_BYTE/ DATA_HALF/ DATA_WORD
 * RETURN VALUE
 *  -
 * SEE ALSO
 *  flash.function
 */
void flash_set_status(struct dw_device *dev, uint32_t data, 
                      enum spi_flash_byte_num byte_num);
/*****/

/***** flash.functions/flash_disable_write() 
 * DESCRIPTION
 *  This function is used to disable write of Flash. 
 *  ARGUMENTS
 *  dev         -- SPI_FLASH controller device handle
 * SEE ALSO
 *  flash.function
 */
void flash_disable_write(struct dw_device *dev);
/*****/

/***** flash.functions/flash_enable_write() 
 * DESCRIPTION
 * This function is used to enable write of Flash. 
 *  dev         -- SPI_FLASH controller device handle
 * RETURN VALUE
 *  -
 * SEE ALSO
 *  flash.function
 */
void flash_enable_write(struct dw_device *dev);
/*****/

/***** flash.functions/flash_read_id()
 * DESCRIPTION
 *  This function is used to read ID of flash. 
 *  ARGUMENTS
 *  dev         -- SPI_FLASH controller device handle
 * RETURN VALUE
 *  uint32_t    -- the ID of SPI
 * SEE ALSO
 *  flash.function
 */
uint32_t flash_read_id(struct dw_device *dev);
//uint32_t flash_read_id(struct dw_device *dev,unsigned int dummy_cycle );
/*****/


///***** flash.functions/flash_set_protect_mem()
// * DESCRIPTION
// *  This function is used to set the protect area of flash.
// *  ARGUMENTS
// *  dev         -- SPI_FLASH controller device handle
// *  spi_protect_mem   -- enum data 
// * RETURN VALUE
// *  -
// * SEE ALSO
// *  flash.function
// */
//void flash_set_protect_mem(struct dw_device *dev, enum flash_protect_mem size);
///*****/

//***** flash.functions/flash_enter_dp()
//* DESCRIPTION
//*  This function is used to enter power dwon mode of flash.
//*  ARGUMENTS
//*  dev         -- SPI_FLASH controller device handle
//*  cmd         -- Flash deep power down command
//* RETURN VALUE
//*  -
//* SEE ALSO
//*  flash.function
//*/
//oid flash_enter_dp(struct dw_device *dev, uint8_t cmd);
//*****/

/***** flash.functions/flash_set_en4k()
 * DESCRIPTION
 *  This function is used to setv SPI EN4K command.
 *  ARGUMENTS
 *  -
 *  dev         -- SPI_FLASH controller device handle

 * RETURN VALUE
 *  -
 * SEE ALSO
 *  flash.function
 */
//void flash_set_en4k(struct dw_device *dev);

/***** flash.functions/flash_set_ex4k()
 * DESCRIPTION
 *  This function is used to setv SPI EX4K command.
 *  ARGUMENTS
 *  -
 *  dev         -- SPI_FLASH controller device handle

 * RETURN VALUE
 *  -
 * SEE ALSO
 *  flash.function
 */
//void flash_set_ex4k(struct dw_device *dev);

///***** flash.functions/flash_release_dp()
// * DESCRIPTION
// *  This function is used to release power down mode of flash. 
// *  ARGUMENTS
// *  dev         -- SPI_FLASH controller device handle
// * RETURN VALUE
// *  -
// * RETURN VALUE
// *  -
// * SEE ALSO
// *  flash.function
// */
//void flash_release_dp(struct dw_device *dev);
///*****/

/***** spi_flash.functions/flash_device_init()
 * DESCRIPTION
 *  This function is used to read_id the spi_flash controller and 
 *  initialize the device infomation. 
 *  ARGUMENTS
 *  dev         -- SPI_FLASH controller device handle
 *  DW_ENODEV   -- No Device infomation 
 * SEE ALSO
 *  spi_flash.function
 */
uint32_t flash_device_init(struct dw_device *dev, uint8_t cmd_rdid);
/*****/

/***** flash.functions/flash_get_status()
 * DESCRIPTION
 *  This function is used to read status of flash. 
 *  ARGUMENTS
 *  dev         -- SPI_FLASH controller device handle
 * RETURN VALUE
 *  uint8_t    -- value of status regrister(SR:0x28)
 * SEE ALSO
 *  flash.function
 */
uint8_t flash_get_status(struct dw_device *dev);




int spic_init_auto_mode(struct dw_device *dev);

#if CONFIG_SPI_3to4BYTES_ADDRESS_SUPPORT
void flash_EN4B(struct dw_device *dev);
#endif

/*****/


/***** flash.functions/flash_init() 
 * DESCRIPTION
 *  This function is used to initize flash.
 *  * set write disable
 *  * set status =0
 *  for wirte control signal and sec_prot signals for write data. 
 *
 *  ARGUMENTS
 *  -
 *  dev         -- SPI_FLASH controller device handle
 * RETURN VALUE
 *  DW_ENODEV   -- No Device infomation 
 * SEE ALSO
 *  flash.function
 */
uint32_t flash_init(struct dw_device *dev, uint8_t cmd_rdid);
/*****/

/***** flash.functions/flash_wait_busy()
 * DESCRIPTION
 *  This function is used to read the status of flash and 
 *  check the flash is in wirte progress or not
 *  ARGUMENTS
 *  dev         -- SPI_FLASH controller device handle
 * SEE ALSO
 *  dw_device
 * SOURCE
 */

void flash_wait_busy(struct dw_device *dev);
/*****/

/***** flash.functions/flash_write();
 * DESCRIPTION
 *  This function is used to write byte to flash.
 *  ARGUMENTS
 *  dev         -- SPI_FLASH controller device handle
 *  byte_num    -- recevie data tyep: byte/ half/ word 
 * SEE ALSO
 * SOURCE
 */
//void flash_write(struct dw_device *dev, uint32_t addr, uint32_t data, enum spi_flash_byte_num byte_num);
/*****/

/***** flash.functions/flash_read();
 * DESCRIPTION
 *  This function is used to read bute from flash. 
 *  ARGUMENTS
 *  dev         -- SPI_FLASH controller device handle
 *  byte_num    -- recevie data tyep: byte/ half/ word 
 * SEE ALSO
 *  -uint32_t   -- if successful
 *  -DW_EPERM   -- if the device doesn't support readx2
 * SOURCE
 */
uint32_t flash_read(struct dw_device *dev, uint32_t addr,
                    enum spi_flash_byte_num byte_num);

/*****/

/***** flash.functions/flash_write_words();
 * DESCRIPTION
 *  This function is used to write byte to flash.
 *  ARGUMENTS
 *  dev         -- SPI_FLASH controller device handle
 *  target_addr -- write address to SPI Flash
 *  source addr -- read address of source data
 *  data_num    -- push serial words data into FIFO
 * SEE ALSO
 * SOURCE
 */
void flash_write_words(struct dw_device *dev, uint32_t target_addr, 
                       uint32_t source_addr, uint32_t data_num);
/*****/

/***** flash.functions/flash_read_bytes();
 * DESCRIPTION
 *  This function is used to set control register to read bytes data 
 *  from SPI FLash. User should read data bytes byte userself after 
 *  implementing flash_worte_bytes function.
 *  User also be careful that it's still in RX_mode after implementing 
 *  the function. Because reading these data needs in RX_mode or it would
 *  cause error to pop data.
 * 
 *  ARGUMENTS
 *  dev         -- SPI_FLASH controller device handle
 *  addr        -- read address of SPI Flash  
 *  data_num    -- read serial bytes data into FIFO
 * SEE ALSO
 * SOURCE
 */
void flash_read_bytes(struct dw_device *dev, uint32_t addr, 
                      uint32_t data_num);
/*****/


/***** flash.functions/flash_fastread();
 * DESCRIPTION
 *  This function is used fastread_cmd to read byte from flash. 
 *  ARGUMENTS
 *  dev         -- SPI_FLASH controller device handle
 *  byte_num    -- recevie data tyep: byte/ half/ word 
 * SEE ALSO
 *  -uint32_t   -- if successful
 *  -DW_EPERM   -- if the device doesn't support readx2
 * SOURCE
 */
uint32_t flash_fastread(struct dw_device *dev, uint32_t addr,
                       enum spi_flash_byte_num byte_num);
/*****/

/***** flash.functions/flash_writex2();
 * DESCRIPTION
 *  This function is used dual_write cmd to wirte to flash.
 *  ARGUMENTS
 *  dev         -- SPI_FLASH controller device handle
 *  byte_num    -- recevie data tyep: byte/ half/ word 
 * SEE ALSO
 *  -uint32_t   -- if successful
 *  -DW_EPERM   -- if the device doesn't support readx2
 * SOURCE
 */
uint32_t flash_writex2(struct dw_device *dev, uint32_t addr, uint32_t data, 
                 enum spi_flash_byte_num byte_num);
/*****/

/***** flash.functions/flash_readx2();
 * DESCRIPTION
 *  This function is used to dual_read_cmd to read flash. 
 *  ARGUMENTS
 *  dev         -- SPI_FLASH controller device handle
 *  byte_num    -- recevie data tyep: byte/ half/ word 
 * SEE ALSO
 *  -uint32_t   -- if successful
 *  -DW_EPERM   -- if the device doesn't support readx2
 * SOURCE
 */
uint32_t flash_readx2(struct dw_device *dev, uint32_t addr,
                    enum spi_flash_byte_num byte_num);

/***** flash.functions/flash_writex4();
 * DESCRIPTION
 *  This function is used quad_write cmd to wirte to flash.
 *  ARGUMENTS
 *  dev         -- SPI_FLASH controller device handle
 *  byte_num    -- recevie data tyep: byte/ half/ word 
 * SEE ALSO
 *  0           -- if successful
 *  -DW_EPERM   -- if the device doesn't support readx2
 * SOURCE
 */
uint32_t flash_writex4(struct dw_device *dev, uint32_t addr, uint32_t data, 
                 enum spi_flash_byte_num byte_num);
/*****/

/***** flash.functions/flash_readx4()
 * DESCRIPTION
 *  This function is used to quad_read_cmd to read flash. 
 *  ARGUMENTS
 *  dev         -- SPI_FLASH controller device handle
 *  byte_num    -- recevie data tyep: byte/ half/ word 
 * SEE ALSO
 *  0           -- if successful
 *  -DW_EPERM   -- if the device doesn't support readx2
 * SOURCE
 */
uint32_t flash_readx4(struct dw_device *dev, uint32_t addr,
                    enum spi_flash_byte_num byte_num);

/***** flash.functions/flash_write_block()
 * DESCRIPTION
 *  This function is used to wirte block data to flash. 
 *  ARGUMENTS
 *  dev         -- SPI_FLASH controller device handle
 *  source_addr -- source address (source addre[1:0] should be equal target address [1:0])
 *  byte_num    -- recevie data tyep: byte/ half/ word 
 *  target_addr -- target address (target addre[1:0] should be equal source address [1:0])
 * SEE ALSO
 *  0           -- if successful
 *  -DW_EIO     -- if tx_threshold > TX_FIFO_DEPTH      
 * SOURCE
 */
uint32_t flash_write_block(struct dw_device *dev, uint32_t source_addr,
                       uint32_t byte_size, uint32_t target_addr);

#endif /*SPI_FLASH_PUBLIC_H*/ 
