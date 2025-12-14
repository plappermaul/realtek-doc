/* mbed Microcontroller Library
 *******************************************************************************
 * Copyright (c) 2014, Realtek Semiconductor Corp.
 * All rights reserved.
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *******************************************************************************
 */

#include "rtl8195a.h"
#ifdef CONFIG_EFUSE_EN

extern VOID  ReadEfuseContant2(OUT u8 *pContant);
extern u8 WriteEfuseContant2(IN u8 CodeWordNum, IN u8 WordEnable, IN u8 *pContant);


 /**
  * @brief  Read efuse contant of specified user 
  * @param  data: Specified the address to save the readback data.
  */
void  efuse_user_data_read2(uint8_t * data)
{
    ReadEfuseContant2(data);
}

/**
  * @brief  Write user's contant to efuse
  * @param  codewordnum: Specifies the number of the codeword to be programmed.
  * @param  worden: Specifies the word enable of the codeword.
  * @param  data: Specified the data to be programmed.
  * @retval   status: Success:1 or Failure: Others.
  */
int  efuse_user_data_write2(uint8_t codewordnum, uint8_t worden, uint8_t *data)
{
    return WriteEfuseContant2(codewordnum,worden,data);
}
#endif
