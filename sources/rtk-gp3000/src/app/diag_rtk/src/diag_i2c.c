/*
 * Copyright (C) 2012 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Definition those CPU command and APIs in the SDK diagnostic shell.
 *
 * Feature : The file have include the following module and sub-modules
 *
 */

/*
 * Include Files
 */
#include <stdio.h>
#include <string.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <diag_util.h>
#include <diag_str.h>
#include <rtk/i2c.h>

#define DIAGSHELL_I2C_WRITE_BUFFSIZE    512

static uint32 *i2c_write_buffer = NULL;

/*
 * i2c init port <UINT:port>
 */
cparser_result_t
cparser_cmd_i2c_init_port_port(
    cparser_context_t *context,
    uint32_t  *port_ptr)
{
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_i2c_init(*port_ptr), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_i2c_init_port_port */

/*
 * i2c set state port <UINT:port> ( enable | disable )
 */
cparser_result_t
cparser_cmd_i2c_set_state_port_port_enable_disable(
    cparser_context_t *context,
    uint32_t  *port_ptr)
{
    rtk_enable_t enable=ENABLED;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    if('e' == TOKEN_CHAR(5,0))
        enable = ENABLED;
    else if('d' == TOKEN_CHAR(5,0))
        enable = DISABLED;

    DIAG_UTIL_ERR_CHK(rtk_i2c_enable_set(*port_ptr, enable), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_i2c_set_state_port_port_enable_disable */

/*
 * i2c get state port <UINT:port>
 */
cparser_result_t
cparser_cmd_i2c_get_state_port_port(
    cparser_context_t *context,
    uint32_t  *port_ptr)
{
    int32 ret;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_i2c_enable_get(*port_ptr, &enable), ret);

    diag_util_printf("I2C state: %s\n\r", diagStr_enable[enable]);

    return CPARSER_OK;
}    /* end of cparser_cmd_i2c_get_state_port_port */

/*
 * i2c set width port <UINT:port> ( 8-bit | 16-bit | 24-bit | 32-bit )
 */
cparser_result_t
cparser_cmd_i2c_set_width_port_port_8_bit_16_bit_24_bit_32_bit(
    cparser_context_t *context,
    uint32_t  *port_ptr)
{
    rtk_i2c_width_t width;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    if('8' == TOKEN_CHAR(5,0))
        width = I2C_WIDTH_8bit;
    else if('1' == TOKEN_CHAR(5,0))
        width = I2C_WIDTH_16bit;
    else if('2' == TOKEN_CHAR(5,0))
        width = I2C_WIDTH_24bit;
    else if('3' == TOKEN_CHAR(5,0))
        width = I2C_WIDTH_32bit;
    else
        width = I2C_WIDTH_8bit;



    DIAG_UTIL_ERR_CHK(rtk_i2c_width_set(*port_ptr, width), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_i2c_set_width_port_port_8_bit_16_bit_24_bit_32_bit */

/*
 * i2c get width port <UINT:port>
 */
cparser_result_t
cparser_cmd_i2c_get_width_port_port(
    cparser_context_t *context,
    uint32_t  *port_ptr)
{
    int32 ret;
    rtk_i2c_width_t width;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_i2c_width_get(*port_ptr, &width), ret);

    diag_util_printf("I2C width: %s\n\r", diagStr_i2cWidth[width]);

    return CPARSER_OK;
}    /* end of cparser_cmd_i2c_get_width_port_port */

/*
 * i2c set port <UINT:port> dev <UINT:dev_id> reg <UINT:register> data <UINT:data>
 */
cparser_result_t
cparser_cmd_i2c_set_port_port_dev_dev_id_reg_register_data_data(
    cparser_context_t *context,
    uint32_t  *port_ptr,
    uint32_t  *dev_id_ptr,
    uint32_t  *register_ptr,
    uint32_t  *data_ptr)
{
    rtk_i2c_width_t width;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_i2c_write(*port_ptr, *dev_id_ptr, *register_ptr, *data_ptr), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_i2c_set_port_port_dev_dev_id_reg_register_data_data */

/*
 * i2c set port <UINT:port> dev <UINT:dev_id> reg <UINT:register> write-buffer count <UINT:count>
 */
cparser_result_t
cparser_cmd_i2c_set_port_port_dev_dev_id_reg_register_write_buffer_count_count(
    cparser_context_t *context,
    uint32_t  *port_ptr,
    uint32_t  *dev_id_ptr,
    uint32_t  *register_ptr,
    uint32_t  *count_ptr)
{
    rtk_i2c_width_t width;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    if(*count_ptr <= DIAGSHELL_I2C_WRITE_BUFFSIZE)
    {
        DIAG_UTIL_ERR_CHK(rtk_i2c_seq_write(*port_ptr, *dev_id_ptr, *register_ptr, i2c_write_buffer, *count_ptr), ret);
    }
    else
    {
        diag_util_printf("diagshell only support %d byte write\n", DIAGSHELL_I2C_WRITE_BUFFSIZE);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_i2c_set_port_port_dev_dev_id_reg_register_data_data */

/*
 * i2c set write-buffer pos <UINT:pos> data <STRING:data>
 */
cparser_result_t
cparser_cmd_i2c_set_write_buffer_pos_pos_data_data(
    cparser_context_t *context,
    uint32_t  *pos_ptr,
    char * *data_ptr)
{
    int32 i, j, ret;
    char tmp[3];
    char *pPtr;
    uint16 input_length;
    uint16 payload_length;

    DIAG_UTIL_PARAM_CHK();

    if(NULL == i2c_write_buffer)
    {
        i2c_write_buffer = (uint32 *) malloc(DIAGSHELL_I2C_WRITE_BUFFSIZE * sizeof(uint32));
        if(NULL == i2c_write_buffer)
        {
            diag_util_printf("allocate failed\n");
            return CPARSER_NOT_OK;
        }
        osal_memset(i2c_write_buffer, 0, DIAGSHELL_I2C_WRITE_BUFFSIZE * sizeof(uint32));
    }

    /* Syntax check of the input string */
    input_length = strlen(*data_ptr);
    if(input_length & 0x01)
    {
        diag_util_printf("payload syntax error\n");
        return CPARSER_NOT_OK;
    }
    for(i = 0, pPtr = *data_ptr; i < input_length; i++)
    {
        if((pPtr[i] >= 'a') && (pPtr[i] <= 'f'))
            continue;
        else if((pPtr[i] >= 'A') && (pPtr[i] <= 'F'))
            continue;
        else if((pPtr[i] >= '0') && (pPtr[i] <= '9'))
            continue;

        diag_util_printf("payload syntax error at %d\n", i);
        return CPARSER_NOT_OK;
    }

    /* Parse the string */
    payload_length = input_length >> 1;
    tmp[2] = '\0';
    for(i = *pos_ptr, j = 0, pPtr = *data_ptr; j < payload_length && i < DIAGSHELL_I2C_WRITE_BUFFSIZE; i++, j++, pPtr += 2)
    {
        tmp[0] = pPtr[0];
        tmp[1] = pPtr[1];
        i2c_write_buffer[i] = strtoul(&tmp[0], NULL, 16);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_i2c_set_write_buffer_pos_pos_data_data */

/*
 * i2c clear write-buffer
 */
cparser_result_t
cparser_cmd_i2c_clear_write_buffer(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    if(NULL != i2c_write_buffer)
    {
        free(i2c_write_buffer);
        i2c_write_buffer = NULL;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_i2c_clear_write_buffer */

/*
 * i2c show write-buffer
 */
cparser_result_t
cparser_cmd_i2c_show_write_buffer(
    cparser_context_t *context)
{
    int32 i;

    DIAG_UTIL_PARAM_CHK();

    if(NULL != i2c_write_buffer)
    {
        diag_util_printf("     00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f");
        for(i = 0 ; i < DIAGSHELL_I2C_WRITE_BUFFSIZE ; i ++)
        {
            if(0 == (i % 16))
            {
                diag_util_printf("\n%04x ", i / 16);
            }

            diag_util_printf("%02x ", i2c_write_buffer[i]);
        }
        diag_util_printf("\n");
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_i2c_show_write_buffer */

/*
 * i2c get port <UINT:port> dev <UINT:dev_id> reg <UINT:register>
 */
cparser_result_t
cparser_cmd_i2c_get_port_port_dev_dev_id_reg_register(
    cparser_context_t *context,
    uint32_t  *port_ptr,
    uint32_t  *dev_id_ptr,
    uint32_t  *register_ptr)
{
    uint32 data;
    int32  ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_i2c_read(*port_ptr, *dev_id_ptr, *register_ptr, &data), ret);

    diag_util_printf("  dev: 0x%x, reg: 0x%x, data: 0x%x \n\r", *dev_id_ptr, *register_ptr, data);

    return CPARSER_OK;
}    /* end of cparser_cmd_i2c_get_port_port_dev_dev_id_reg_register */

/*
 * i2c get port <UINT:port> dev <UINT:dev_id> reg <UINT:register> count <UINT:count>
 */
cparser_result_t
cparser_cmd_i2c_get_port_port_dev_dev_id_reg_register_count_count(
    cparser_context_t *context,
    uint32_t  *port_ptr,
    uint32_t  *dev_id_ptr,
    uint32_t  *register_ptr,
    uint32_t  *count_ptr)
{
    uint32 i, *data;
    int32  ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    data = (uint32 *) malloc(sizeof(uint32) * *count_ptr);
    if(NULL == data)
    {
        DIAG_UTIL_ERR_CHK(RT_ERR_NULL_POINTER, ret);
    }
    ret = rtk_i2c_seq_read(*port_ptr, *dev_id_ptr, *register_ptr, data, *count_ptr);
    if(RT_ERR_OK != ret)
    {
        free(data);
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_printf("start with dev: 0x%x, reg: 0x%x", *dev_id_ptr, *register_ptr);
    for(i = 0 ; i < *count_ptr ; i++)
    {
        if(i % 8 == 0)
        {
            if(i % 16 == 0)
            {
                diag_util_printf("\n%04x ", i / 16);
            }
            else
            {
                diag_util_printf("\n     ");
            }
            diag_util_printf("-----------------------------------------------------------------------");
            diag_util_printf("\n     %08x %08x %08x %08x %08x %08x %08x %08x\n",
                i % 16, i % 16 + 1, i % 16 + 2, i % 16 + 3, i % 16 + 4, i % 16 + 5, i % 16 + 6, i % 16 + 7);
            diag_util_printf("     ");
        }
        diag_util_printf("%08x ", *(data+i));
    }
    diag_util_printf("\n");
    free(data);

    return CPARSER_OK;
}    /* end of cparser_cmd_i2c_get_port_port_dev_dev_id_reg_register */

/*
 * i2c get port <UINT:port> dev <UINT:dev_id>
 */
cparser_result_t
cparser_cmd_i2c_get_port_port_dev_dev_id(
    cparser_context_t *context,
    uint32_t  *port_ptr,
    uint32_t  *dev_id_ptr)
{
    uint32 data;
    int32  ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_i2c_curr_read(*port_ptr, *dev_id_ptr, &data), ret);

    diag_util_printf("  dev: 0x%x, data: 0x%x \n\r", *dev_id_ptr, data);

    return CPARSER_OK;
}    /* end of cparser_cmd_i2c_get_port_port_dev_dev_id */

/*
 * i2c get port <UINT:port> dev <UINT:dev_id> count <UINT:count>
 */
cparser_result_t
cparser_cmd_i2c_get_port_port_dev_dev_id_count_count(
    cparser_context_t *context,
    uint32_t  *port_ptr,
    uint32_t  *dev_id_ptr,
    uint32_t  *count_ptr)
{
    uint32 i, *data;
    int32  ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    data = (uint32 *) malloc(sizeof(uint32) * *count_ptr);
    if(NULL == data)
    {
        DIAG_UTIL_ERR_CHK(RT_ERR_NULL_POINTER, ret);
    }
    ret = rtk_i2c_seqCurr_read(*port_ptr, *dev_id_ptr, data, *count_ptr);
    if(RT_ERR_OK != ret)
    {
        free(data);
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_printf("start with dev: 0x%x", *dev_id_ptr);
    for(i = 0 ; i < *count_ptr ; i++)
    {
        if(i % 8 == 0)
        {
            if(i % 16 == 0)
            {
                diag_util_printf("\n%04x ", i / 16);
            }
            else
            {
                diag_util_printf("\n     ");
            }
            diag_util_printf("-----------------------------------------------------------------------");
            diag_util_printf("\n     %08x %08x %08x %08x %08x %08x %08x %08x\n",
                i % 16, i % 16 + 1, i % 16 + 2, i % 16 + 3, i % 16 + 4, i % 16 + 5, i % 16 + 6, i % 16 + 7);
            diag_util_printf("     ");
        }
        diag_util_printf("%08x ", *(data+i));
    }
    diag_util_printf("\n");
    free(data);

    return CPARSER_OK;
}    /* end of cparser_cmd_i2c_get_port_port_dev_dev_id_count_count */

/*
 * i2c set clock port <UINT:port> <UINT:khz>
 */
cparser_result_t
cparser_cmd_i2c_set_clock_port_port_khz(
    cparser_context_t *context,
    uint32_t  *port_ptr,
    uint32_t  *khz_ptr)
{
    rtk_i2c_width_t width;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_i2c_clock_set(*port_ptr, *khz_ptr), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_i2c_set_clock_port_port_khz */

/*
 * i2c get clock port <UINT:port>
 */
cparser_result_t
cparser_cmd_i2c_get_clock_port_port(
    cparser_context_t *context,
    uint32_t  *port_ptr)
{
    int32 ret;
    uint32 clock;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_i2c_clock_get(*port_ptr, &clock), ret);

    diag_util_printf("I2C clock: %d KHz\n\r", clock);

    return CPARSER_OK;
}    /* end of cparser_cmd_i2c_get_clock_port_port */

/*
 * i2c set eeprom-mirror dev <UINT:dev_id> addr-width ( 8-bit | 16-bit | 24-bit | 32-bit ) addr-ext-mode ( mode0 | mode1 | mode2 | mode3 )
 */
cparser_result_t
cparser_cmd_i2c_set_eeprom_mirror_dev_dev_id_addr_width_8_bit_16_bit_24_bit_32_bit_addr_ext_mode_mode0_mode1_mode2_mode3(
    cparser_context_t *context,
    uint32_t  *dev_id_ptr)
{
    rtk_i2c_eeprom_mirror_t eepMirrirCfg;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    eepMirrirCfg.devId = *dev_id_ptr;

    if('8' == TOKEN_CHAR(6,0))
        eepMirrirCfg.addrWidth = I2C_WIDTH_8bit;
    else if('1' == TOKEN_CHAR(6,0))
        eepMirrirCfg.addrWidth = I2C_WIDTH_16bit;
    else if('2' == TOKEN_CHAR(6,0))
        eepMirrirCfg.addrWidth = I2C_WIDTH_24bit;
    else if('3' == TOKEN_CHAR(6,0))
        eepMirrirCfg.addrWidth = I2C_WIDTH_32bit;
    else
        eepMirrirCfg.addrWidth = I2C_WIDTH_8bit;

    if('0' == TOKEN_CHAR(8,4))
        eepMirrirCfg.addrExtMode = I2C_ADDR_EXT_MODE_0;
    else if('1' == TOKEN_CHAR(8,4))
        eepMirrirCfg.addrExtMode = I2C_ADDR_EXT_MODE_1;
    else if('2' == TOKEN_CHAR(8,4))
        eepMirrirCfg.addrExtMode = I2C_ADDR_EXT_MODE_2;
    else if('3' == TOKEN_CHAR(8,4))
        eepMirrirCfg.addrExtMode = I2C_ADDR_EXT_MODE_3;
    else
        eepMirrirCfg.addrExtMode = I2C_ADDR_EXT_MODE_0;


    DIAG_UTIL_ERR_CHK(rtk_i2c_eepMirror_set(eepMirrirCfg), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_i2c_set_eeprom_mirror_dev_dev_id_addr_width_8_bit_16_bit_24_bit_32_bit_addr_ext_mode_mode0_mode1_mode2_mode3 */

/*
 * i2c get eeprom-mirror
 */
cparser_result_t
cparser_cmd_i2c_get_eeprom_mirror(
    cparser_context_t *context)
{
    int32 ret;
    rtk_i2c_eeprom_mirror_t eepMirrirCfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_i2c_eepMirror_get(&eepMirrirCfg), ret);

    diag_util_printf("I2C EEPROM Mirror: \n\r");
    diag_util_printf("    device ID     0x%x \n\r", eepMirrirCfg.devId);
    diag_util_printf("    addr width    %s \n\r", diagStr_i2cWidth[eepMirrirCfg.addrWidth]);
    diag_util_printf("    addr ext mode %s \n\r", diagStr_i2cEepMirrorAddrExtMode[eepMirrirCfg.addrExtMode]);

    return CPARSER_OK;
}    /* end of cparser_cmd_i2c_get_eeprom_mirror */

/*
 * i2c set eeprom-mirror-reg <UINT:register> data <UINT:data>
 */
cparser_result_t
cparser_cmd_i2c_set_eeprom_mirror_reg_register_data_data(
    cparser_context_t *context,
    uint32_t  *register_ptr,
    uint32_t  *data_ptr)
{
    rtk_i2c_width_t width;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_i2c_eepMirror_write(*register_ptr, *data_ptr), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_i2c_set_eeprom_mirror_reg_register_data_data */

/*
 * i2c get eeprom-mirror-reg <UINT:register>
 */
cparser_result_t
cparser_cmd_i2c_get_eeprom_mirror_reg_register(
    cparser_context_t *context,
    uint32_t  *register_ptr)
{
    uint32 data;
    int32  ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_i2c_eepMirror_read(*register_ptr, &data), ret);

    diag_util_printf("  reg: 0x%x, data: 0x%x \n\r",*register_ptr, data);

    return CPARSER_OK;
}    /* end of cparser_cmd_i2c_get_eeprom_mirror_reg_register */

/*
 * i2c set addr-width port <UINT:port> ( 8-bit | 16-bit | 24-bit | 32-bit )
 */
cparser_result_t
cparser_cmd_i2c_set_addr_width_port_port_8_bit_16_bit_24_bit_32_bit(
    cparser_context_t *context,
    uint32_t  *port_ptr)
{
    rtk_i2c_width_t width;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    if('8' == TOKEN_CHAR(5,0))
        width = I2C_WIDTH_8bit;
    else if('1' == TOKEN_CHAR(5,0))
        width = I2C_WIDTH_16bit;
    else if('2' == TOKEN_CHAR(5,0))
        width = I2C_WIDTH_24bit;
    else if('3' == TOKEN_CHAR(5,0))
        width = I2C_WIDTH_32bit;
    else
        width = I2C_WIDTH_8bit;


    DIAG_UTIL_ERR_CHK(rtk_i2c_addrWidth_set(*port_ptr, width), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_i2c_set_addr_width_port_port_8_bit_16_bit_24_bit_32_bit */

/*
 * i2c get addr-width port <UINT:port>
 */
cparser_result_t
cparser_cmd_i2c_get_addr_width_port_port(
    cparser_context_t *context,
    uint32_t  *port_ptr)
{
    int32 ret;
    rtk_i2c_width_t width;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_i2c_addrWidth_get(*port_ptr, &width), ret);

    diag_util_printf("I2C width: %s\n\r", diagStr_i2cWidth[width]);

    return CPARSER_OK;
}    /* end of cparser_cmd_i2c_get_addr_width_port_port */


/*
 * i2c set data-width port <UINT:port> ( 8-bit | 16-bit | 24-bit | 32-bit )
 */
cparser_result_t
cparser_cmd_i2c_set_data_width_port_port_8_bit_16_bit_24_bit_32_bit(
    cparser_context_t *context,
    uint32_t  *port_ptr)
{
    rtk_i2c_width_t width;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    if('8' == TOKEN_CHAR(5,0))
        width = I2C_WIDTH_8bit;
    else if('1' == TOKEN_CHAR(5,0))
        width = I2C_WIDTH_16bit;
    else if('2' == TOKEN_CHAR(5,0))
        width = I2C_WIDTH_24bit;
    else if('3' == TOKEN_CHAR(5,0))
        width = I2C_WIDTH_32bit;
    else
        width = I2C_WIDTH_8bit;


    DIAG_UTIL_ERR_CHK(rtk_i2c_dataWidth_set(*port_ptr, width), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_i2c_set_data_width_port_port_8_bit_16_bit_24_bit_32_bit */

/*
 * i2c get data-width port <UINT:port>
 */
cparser_result_t
cparser_cmd_i2c_get_data_width_port_port(
    cparser_context_t *context,
    uint32_t  *port_ptr)
{
    int32 ret;
    rtk_i2c_width_t width;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_i2c_dataWidth_get(*port_ptr, &width), ret);

    diag_util_printf("I2C width: %s\n\r", diagStr_i2cWidth[width]);

    return CPARSER_OK;
}    /* end of cparser_cmd_i2c_get_data_width_port_port */

