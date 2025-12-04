//#include <stdio.h>
//#include "lx_common.h"
//#include "lx_macro.h"
//#include "rle0272_defs.h"

#include "include/DW_common.h"
#include "include/spi_flash_public.h"
#include "include/spi_flash_private.h"

void spi_flash_enable(struct dw_device *dev)
{
   struct spi_flash_portmap *spi_flash_map;
   //int info;

   spi_flash_map = dev->base_address;

   spi_flash_map-> ssienr =1;

}

void spi_flash_disable(struct dw_device *dev)
{
   struct spi_flash_portmap *spi_flash_map;
   //int info;

   spi_flash_map = dev->base_address;
   spi_flash_map-> ssienr =0;
}

uint32_t spi_flash_setser(struct dw_device *dev, uint32_t ser_num)
{
   struct spi_flash_portmap *spi_flash_map;
   struct spi_flash_param *spi_flash_para;

   //int info;

   spi_flash_map = dev->base_address;
   spi_flash_para = dev->comp_param;

   // Disable SPI_FLASH
   spi_flash_map-> ssienr =0;

   #if C_DEBUG
   printf("slaves_num = %d\n", spi_flash_para->spi_flash_num_slaves);
   #endif

   if(ser_num >= (1<< (spi_flash_para->spi_flash_num_slaves) ))
     return DW_EINVAL;
   else {
     spi_flash_map-> ser = 1 << ser_num;
     return 0;
   }
}


uint32_t spi_flash_getidr(struct dw_device *dev)
{
  struct spi_flash_portmap *spi_flash_map;
  struct spi_flash_param *spi_flash_para;

  int info;

  spi_flash_map = dev-> base_address;
  spi_flash_para = dev-> comp_param;

  info =  spi_flash_map-> idr;
  #if C_DEBUG
  printf("Read spi_flash_ID= 0x%08x\n", info);
  #endif

  if(info != spi_flash_para->spi_flash_idr )
  #if C_DEBUG
  printf ("spi_flash_ID is not expect number.\n");
  #endif

   return 0;
}

/***** spi_flash.functions/spi_flash_set_eeprom_mode()
 * DESCRIPTION
 *  This function is used to set the control register.
 *  ARGUMENTS
 *  dev         -- DMA controller device handle
 * RETURN VALUE
 *  -
 * SEE ALSO
 *  dw_device
 * SOURCE
 */
void spi_flash_set_eeprom_mode(struct dw_device *dev)
{
   struct spi_flash_portmap *spi_flash_map;
   int info;

   spi_flash_map = dev->base_address;

   // Disable SPI_FLASH
   spi_flash_map-> ssienr =0;

   info = spi_flash_map-> ctrlr0;

   info = info | 0x00000300;

   spi_flash_map-> ctrlr0 = info;

}

/***** spi_flash.functions/spi_flash_set_issuemode()
 * DESCRIPTION
 *  This function is used to set the control register.
 *  ARGUMENTS
 *  dev         -- DMA controller device handle
 * RETURN VALUE
 * -
 * SEE ALSO
 *  dw_device
 * SOURCE
 */
void spi_flash_set_tx_mode(struct dw_device *dev)
{
   struct spi_flash_portmap *spi_flash_map;
   int info;

   spi_flash_map = dev->base_address;

   // Disable SPI_FLASH
   spi_flash_map-> ssienr =0;

   info = spi_flash_map-> ctrlr0;

   info = info & 0xffffff3f;

   spi_flash_map-> ctrlr0 = 0x0000;

}

/***** spi_flash.functions/spi_flash_set_receive_mode()
 * DESCRIPTION
 *  This function is used to set the control register.
 *  ARGUMENTS
 *  dev         -- DMA controller device handle
 * RETURN VALUE
 *  -
 * SEE ALSO
 *  dw_device
 * SOURCE
 */
void spi_flash_set_rx_mode(struct dw_device *dev)
{
   struct spi_flash_portmap *spi_flash_map;
   int info;

   spi_flash_map = dev->base_address;

   // Disable SPI_FLASH
   spi_flash_map-> ssienr =0;

   info = spi_flash_map-> ctrlr0;

   info = info | 0x0300;

   spi_flash_map-> ctrlr0 = info;

}


/***** spi_flash.functions/dw_set_multi_ch()
 * DESCRIPTION
 *  This function is used to set the Ctrlr0 controller.
 *  ARGUMENTS
 *  dev         -- DMA controller device handle
 * RETURN VALUE
 *  -
 * SEE ALSO
 *  dw_device
 * SOURCE
 */
void spi_flash_set_multi_ch(struct dw_device *dev, uint32_t data_ch, uint32_t addr_ch)
{
   struct spi_flash_portmap *spi_flash_map;
   uint32_t temp;

   spi_flash_map= dev->base_address;

   // Disable SPI_FLASH
   spi_flash_map-> ssienr =0;

   temp = ((data_ch & 0x00000003)<<2) | (addr_ch& 0x00000003);

   DW_BITS_SET_VAL(spi_flash_map->ctrlr0, bfoSPI_FLASH_CTRLR0_ADDR_CH,
                   temp, bfwSPI_FLASH_CTRLR0_ADDR_CH);
}

/***** spi_flash.functions/spi_flash_setctrlr1()
 * DESCRIPTION
 *  This function is used to set the ctrlr1 controller.
 *  ARGUMENTS
 *  dev         -- DMA controller device handle
 * RETURN VALUE
 *  0           -- if successful
 *  -DW_EINVAL  -- if num_frame is out of range.
 * SEE ALSO
 *  dw_device
 * SOURCE
 */
uint32_t spi_flash_setctrlr1(struct dw_device *dev, uint32_t num_frame)
{
   struct spi_flash_portmap *spi_flash_map;

   spi_flash_map= dev->base_address;

   if (num_frame>0x00010000)
     return DW_ENODATA;
   else
   {
     spi_flash_map-> ctrlr1 = num_frame;
     #if C_DEBUG
     //printf("spi_flash_setctrlr1 = 0x%08x\n", spi_flash_map->ctrlr1);
     #endif
     return 0;
   }
}
/*****/

/***** spi_flash.functions/spi_flash_setdr()
 * DESCRIPTION
 *  This function is used to set the ctrlr1 controller.
 *  ARGUMENTS
 *  dev         -- DMA controller device handle
 * RETURN VALUE
 *  0           -- if successful
 *  -DW_ECHRNG  -- if dr_number is out the range
 *  -DW_EINVAL  -- if byte_num argument isn't available
 * SEE ALSO
 *  dw_device
 * SOURCE
 */
uint32_t spi_flash_setdr(struct dw_device *dev, enum spi_flash_dr_number dr_num,
                    uint32_t data, enum spi_flash_byte_num byte_num)
{
   struct spi_flash_portmap *spi_flash_map;
   uint32_t wr_data;

   spi_flash_map= dev->base_address;

   #if BIG_ENDIAN
     if (byte_num == DATA_BYTE) {
        wr_data = (data & 0x000000ff);
     }
     else if (byte_num == DATA_HALF) {
        wr_data = ((data & 0x000000ff) <<8) |
                  ((data & 0x0000ff00) >>8) ;
     }
     else if (byte_num == DATA_WORD) {
       wr_data = ((data & 0x000000ff) <<24) |
                 ((data & 0x0000ff00) <<8 ) |
                 ((data & 0x00ff0000) >>8 ) |
                 ((data & 0xff000000) >>16) ;
     }
   #else
        wr_data = data;//little endian for ARM
   #endif

   if (dr_num > DR31)
     return DW_EINVAL;
   else {
     if (byte_num == DATA_BYTE) {
       spi_flash_map->dr[dr_num].byte = wr_data;
     }
     else if (byte_num == DATA_HALF) {
         spi_flash_map->dr[dr_num].half = wr_data;
     }
     else if (byte_num == DATA_WORD) {
         spi_flash_map->dr[dr_num].word = wr_data;
     }
     else {
         return DW_EINVAL;
     }
   }

  return 0;

}
/*****/

/***** spi_flash.functions/dw_setbaudr()
 * DESCRIPTION
 *  This function is used to set the Baudr controller.
 *  ARGUMENTS
 *  dev         -- DMA controller device handle
 * RETURN VALUE
 *  0           -- if successful
 *  -DW_EINVAL  -- if baudrate is out the range
 * SEE ALSO
 *  dw_device
 * SOURCE
 */

#define SYS_FRQ_MHz 200

uint32_t spi_flash_setbaudr(struct dw_device *dev, uint32_t baudrate)
{
   struct spi_flash_portmap *spi_flash_map;

   spi_flash_map= dev->base_address;

   // Disable SPI_FLASH
   spi_flash_map-> ssienr =0;

   if ((baudrate & 0xffff0000) || (baudrate ==0 ))
     return DW_ENODATA;
   else
   {
     spi_flash_map->baudr = baudrate;
     #if 1//PRINTF_ON
     printf("spi_flash Baud rate setting= %d MHZ\n", ((SYS_FRQ_MHz)/ (spi_flash_map->baudr *2)));
      //printf("spi_flash Baud rate setting= %d \n", ((CONFIG_LEXRA_CLOCK*1000000)/ (spi_flash_map->baudr *2)));
     #endif
     return 0;
   }

}
/*****/

/***** spi_flash.functions/dw_set_dummy_cycle()
 * DESCRIPTION
 *  This function is used to set the Baudr controller.
 *  ARGUMENTS
 *  dev         -- DMA controller device handle
 * RETURN VALUE
 *  0           -- if successful
 *  -DW_EINVAL  -- if dum_cycle is out the range
 * SEE ALSO
 *  dw_device
 * SOURCE
 */
uint32_t spi_flash_set_dummy_cycle(struct dw_device *dev, uint32_t dum_cycle)
{
   struct spi_flash_portmap *spi_flash_map;
   struct device_info *dev_info;
   uint32_t cycle;

   spi_flash_map= dev->base_address;
   dev_info = dev-> instance;

   // Disable SPI_FLASH
   spi_flash_map-> ssienr =0;

   // if using fast_read baud_rate
   if (((spi_flash_map-> ctrlr0) & 0x00100000))
   {
     cycle = (spi_flash_map->fbaudr);
   }
   else
   {
     cycle = (spi_flash_map->baudr);
   }


   //printf("SPI: pre_cycle = %d\n", cycle);
   cycle = (cycle *dum_cycle* 2)+ dev_info->cmd_info-> tunning_dummy;
   //cycle = (cycle *dum_cycle* 2);

   if (cycle > 0x10000)
     return DW_ECHRNG;

  #if CONFIG_SPI_DW_BITS_SET_VAL
   DW_BITS_SET_VAL(spi_flash_map->auto_length, bfoSPI_FLASH_AUTO_LEN_DUM,
                    cycle, bfwSPI_FLASH_AUTO_LEN_DUM);
  #endif

   #if 0//C_DEBUG
     printf("\nspi_flash_map->auto_length = 0x%x\n", spi_flash_map->auto_length);
     printf("SPI: cycle = %d\n", cycle);
     printf("SPI: dum_cycle = %d\n", dum_cycle);
   #endif

   return 0;
}
/*****/

/***** spi_flash.functions/spi_flash_getbaudr()
 * DESCRIPTION
 *  This function is used to read the Baudr controller.
 *  ARGUMENTS
 *  dev         -- DMA controller device handle
 * RETURN VALUE
 *  -uint32_t   -- ruten baudrate value
 *
 * SEE ALSO
 *  dw_device
 * SOURCE
 */

uint32_t spi_flash_getbaudr(struct dw_device *dev)
{
   struct spi_flash_portmap *spi_flash_map;
   uint32_t info;
   spi_flash_map= dev->base_address;

   info = (SYS_FRQ_MHz*1000000)/(spi_flash_map->baudr *2);
   #if PRINTF_ON
   printf("spi_flash Baud rate = 0x%08x\n", info);
   #endif
   return info;

}
/*****/

/***** spi_flash.functions/spi_flash_getdr()
 * DESCRIPTION
 *  This function is used to read the ctrlr1 controller.
 *  ARGUMENTS
 *  dev         -- DMA controller device handle
 * RETURN VALUE
 *  uint32_t    -- data popping from FIFO
 * SEE ALSO
 *  dw_device
 * SOURCE
 */
uint32_t spi_flash_getdr(struct dw_device *dev, enum spi_flash_dr_number dr_num,
                         enum spi_flash_byte_num byte_num)
{
   struct spi_flash_portmap *spi_flash_map;
   uint32_t data/*, rd_data*/;

   spi_flash_map= dev->base_address;

   if (dr_num > DR31)
     return DW_ECHRNG;
   else
   {
     #if BIG_ENDIAN
       if (byte_num == DATA_BYTE) {
         data = spi_flash_map->dr[dr_num].byte & 0x000000ff;
       }
       else if (byte_num == DATA_HALF) {
         rd_data = spi_flash_map->dr[dr_num].half & 0x0000ffff;
         data = ((rd_data & 0x000000ff) <<8) |
                ((rd_data & 0x0000ff00) >>8) ;
       }
       else if (byte_num == DATA_WORD) {
         rd_data =  spi_flash_map->dr[dr_num].word;
         data = ((rd_data & 0x000000ff) <<24) |
                ((rd_data & 0x0000ff00) <<8 ) |
                ((rd_data & 0x00ff0000) >>8 ) |
                ((rd_data & 0xff000000) >>24) ;
       }
     #else
       if (byte_num == DATA_BYTE) {
         data = spi_flash_map->dr[dr_num].byte & 0x000000ff;
       }
       else if (byte_num == DATA_HALF) {
         data = spi_flash_map->dr[dr_num].half & 0x0000ffff;
       }
       else if (byte_num == DATA_WORD) {
         data =  spi_flash_map->dr[dr_num].word;
       }
     #endif
       else
         return DW_EIO;
   }

   return data;

}
/*****/

/***** spi_flash.functions/spi_flash_isstatus_busy()
 * DESCRIPTION
 *  This function is used to set the ctrlr1 controller.
 *  ARGUMENTS
 *  dev         -- DMA controller device handle
 * RETURN VALUE
 *  0           -- if device is not busy
 *  -DW_EBUSY   -- if device is on busy
 * SEE ALSO
 *  dw_device
 * SOURCE
 */
uint32_t spi_flash_isstatus_busy(struct dw_device *dev)
{
   struct spi_flash_portmap *spi_flash_map;

   spi_flash_map= dev->base_address;

   if (DW_BIT_GET_UNSHIFTED(spi_flash_map->sr, bfoSPI_FLASH_SR_BUSY))
     return DW_EBUSY;
   else
   {
     return 0;
   }
}

/*****/
/***** spi_flash.functions/spi_flash_wait_nobusy()
 * DESCRIPTION
 *  This function is used to wait the spi_flash is not at busy state.
 *  ARGUMENTS
 *  dev         -- DMA controller device handle
 * SEE ALSO
 *  dw_device
 * SOURCE
 */
void spi_flash_wait_busy(struct dw_device *dev)
{
   struct spi_flash_portmap *spi_flash_map;

   spi_flash_map= dev->base_address;

   //printf("\nspi_flash_wait_busy(): spi_flash_map=0x%x \n",spi_flash_map);

   while (1)
   {
     if (DW_BIT_GET_UNSHIFTED(spi_flash_map->sr, bfoSPI_FLASH_SR_TXE))
     {
       #if C_DEBUG
           printf("spi_flash: transfer error. \n");
	 #endif

       break;
     }
     else
     {
       if ( (!DW_BIT_GET_UNSHIFTED(spi_flash_map->sr, bfoSPI_FLASH_SR_BUSY)))      // not busy
       {
        //  printf("\nspi_flash_wait_busy,SPIC not busy ,go\n");
         break;
       }
	// else//JSW
	// {
	 	//  printf("\nspi_flash_wait_busy,SPIC busy ,delay 10ns\n");
		//  udelay (10);	/* Delay 10ns  */
	// }
     }
   }
}
/*****/

