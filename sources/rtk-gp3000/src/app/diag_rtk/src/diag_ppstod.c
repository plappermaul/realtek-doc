#include <stdio.h>
#include <string.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <diag_util.h>
#include <diag_str.h>
#include <parser/cparser_priv.h>
#include <rtk/ppstod.h>


/*
 * ppstod init
 */
cparser_result_t
cparser_cmd_ppstod_init(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_ppstod_init(), ret);
    
    return CPARSER_OK;
}    /* end of cparser_cmd_ppstod_init */

/*
 * ppstod get delay
 */
cparser_result_t
cparser_cmd_ppstod_get_delay(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint8 delay;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_ppstod_delay_get(&delay), ret);

    diag_util_mprintf("delay: %d ms\n", delay);

    return CPARSER_OK;
}    /* end of cparser_cmd_ppstod_get_delay */

/*
 * ppstod set delay <UINT:delay>
 */
cparser_result_t
cparser_cmd_ppstod_set_delay_delay(
    cparser_context_t *context,
    uint32_t  *delay_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint8 delay;
    
    DIAG_UTIL_PARAM_CHK();

    delay=*delay_ptr;
    
    DIAG_UTIL_ERR_CHK(rtk_ppstod_delay_set(delay), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_ppstod_set_delay_delay */

/*
 * ppstod get pulse-width
 */
cparser_result_t
cparser_cmd_ppstod_get_pulse_width(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint8 width;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_ppstod_pulseWidth_get(&width), ret);

    diag_util_mprintf("width: %d 10ms\n",width);

    return CPARSER_OK;
}    /* end of cparser_cmd_ppstod_get_pulse_width */

/*
 * ppstod set pulse-width <UINT:width>
 */
cparser_result_t
cparser_cmd_ppstod_set_pulse_width_width(
    cparser_context_t *context,
    uint32_t  *width_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint8 width;
    
    DIAG_UTIL_PARAM_CHK();
    
    width=*width_ptr;
    
    DIAG_UTIL_ERR_CHK(rtk_ppstod_pulseWidth_set(width), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_ppstod_set_pulse_width_width */

/*
 * ppstod get baud-rate
 */
cparser_result_t
cparser_cmd_ppstod_get_baud_rate(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 rate;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_ppstod_baudrate_get(&rate), ret);

    diag_util_mprintf("baudrate: %d bps\n",rate);

    return CPARSER_OK;
}    /* end of cparser_cmd_ppstod_get_baud_rate */

/*
 * ppstod set baud-rate ( 115200 | 19200 | 9600 | 4800 | 2400 )
 */
cparser_result_t
cparser_cmd_ppstod_set_baud_rate_115200_19200_9600_4800_2400(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 rate;
    
    DIAG_UTIL_PARAM_CHK();

    if ('1' == TOKEN_CHAR(3, 0))
    {
        if ('1' == TOKEN_CHAR(3, 1))
        {
            rate=115200;
        }
        else if ('9' == TOKEN_CHAR(3, 1))
        {
            rate=19200;
        }
        else
        {
            return CPARSER_NOT_OK;
        }
    }
    else if ('9' == TOKEN_CHAR(3, 0))
    {
        rate=9600;
    }
    else if ('4' == TOKEN_CHAR(3, 0))
    {
        rate=4800;
    }
    else if ('2' == TOKEN_CHAR(3, 0))
    {
        rate=2400;
    }
    else
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_ppstod_baudrate_set(rate), ret);
    
    return CPARSER_OK;
}    /* end of cparser_cmd_ppstod_set_baud_rate_115200_19200_9600_4800_2400 */

/*
 * ppstod get mode
 */
cparser_result_t
cparser_cmd_ppstod_get_mode(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_ppstod_manualMode_t mode;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_ppstod_mode_get(&mode), ret);

    diag_util_mprintf("mode: %s\n",(mode==PPSTOD_MANUAL_MODE_HW)?"hardware":"software");

    return CPARSER_OK;
}    /* end of cparser_cmd_ppstod_get_mode */

/*
 * ppstod set mode ( hardware | software )
 */
cparser_result_t
cparser_cmd_ppstod_set_mode_hardware_software(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_ppstod_manualMode_t mode;

    DIAG_UTIL_PARAM_CHK();

    if ('h' == TOKEN_CHAR(3, 0))
    {
        mode=PPSTOD_MANUAL_MODE_HW;
    }
    else if ('s' == TOKEN_CHAR(3, 0))
    {
        mode=PPSTOD_MANUAL_MODE_SW;
    }
    else
    {
        return CPARSER_NOT_OK;
    }
    
    DIAG_UTIL_ERR_CHK(rtk_ppstod_mode_set(mode), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_ppstod_set_mode_hardware_software */

/*
 * ppstod get sarp-gps-week
 */
cparser_result_t
cparser_cmd_ppstod_get_sarp_gps_week(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint16 week;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_ppstod_sarpGpsWeek_get(&week), ret);

    diag_util_mprintf("sarp gps week: %d\n",week);

    return CPARSER_OK;
}    /* end of cparser_cmd_ppstod_get_sarp_gps_week */

/*
 * ppstod set sarp-gps-week <UINT:week>
 */
cparser_result_t
cparser_cmd_ppstod_set_sarp_gps_week_week(
    cparser_context_t *context,
    uint32_t  *week_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint16 week;

    DIAG_UTIL_PARAM_CHK();

    week=*week_ptr;
    
    DIAG_UTIL_ERR_CHK(rtk_ppstod_sarpGpsWeek_set(week), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_ppstod_set_sarp_gps_week_week */

/*
 * ppstod get sarp-utc-sec
 */
cparser_result_t
cparser_cmd_ppstod_get_sarp_utc_sec(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 sec;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_ppstod_sarpUtcSec_get(&sec), ret);

    diag_util_mprintf("sarp utc second: %u\n",sec);


    return CPARSER_OK;
}    /* end of cparser_cmd_ppstod_get_sarp_utc_sec */

/*
 * ppstod set sarp-utc-sec <UINT:sec>
 */
cparser_result_t
cparser_cmd_ppstod_set_sarp_utc_sec_sec(
    cparser_context_t *context,
    uint32_t  *sec_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 sec;

    DIAG_UTIL_PARAM_CHK();

    sec=*sec_ptr;
    
    DIAG_UTIL_ERR_CHK(rtk_ppstod_sarpUtcSec_set(sec), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_ppstod_set_sarp_utc_sec_sec */

/*
 * ppstod get frame len
 */
cparser_result_t
cparser_cmd_ppstod_get_frame_len(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint8 len;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_ppstod_frameLen_get(&len), ret);

    diag_util_mprintf("frame len: %d\n",len);


    return CPARSER_OK;
}    /* end of cparser_cmd_ppstod_get_frame_len */

/*
 * ppstod set frame len <UINT:len>
 */
cparser_result_t
cparser_cmd_ppstod_set_frame_len_len(
    cparser_context_t *context,
    uint32_t  *len_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint8 len;

    DIAG_UTIL_PARAM_CHK();

    len=*len_ptr;
    
    DIAG_UTIL_ERR_CHK(rtk_ppstod_frameLen_set(len), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_ppstod_set_frame_len_len */

/*
 * ppstod get frame data offset <UINT:offset>
 */
cparser_result_t
cparser_cmd_ppstod_get_frame_data_offset_offset(
    cparser_context_t *context,
    uint32_t  *offset_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint8 offset;
    uint8 value;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    offset=*offset_ptr;

    DIAG_UTIL_ERR_CHK(rtk_ppstod_frameData_get(offset,&value), ret);

    diag_util_mprintf("frame offset %d: 0x%02x\n",offset,value);


    return CPARSER_OK;
}    /* end of cparser_cmd_ppstod_get_frame_data_offset_offset */

/*
 * ppstod set frame data offset <UINT:offset> value <UINT:value>
 */
cparser_result_t
cparser_cmd_ppstod_set_frame_data_offset_offset_value_value(
    cparser_context_t *context,
    uint32_t  *offset_ptr,
    uint32_t  *value_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint8 offset;
    uint8 value;

    DIAG_UTIL_PARAM_CHK();

    offset=*offset_ptr;
    value=*value_ptr;

    DIAG_UTIL_ERR_CHK(rtk_ppstod_frameData_set(offset,value), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_ppstod_set_frame_data_offset_offset_value_value */

/*
 * ppstod dump frame
 */
cparser_result_t
cparser_cmd_ppstod_dump_frame(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint8 offset;
    uint8 value;
    
    DIAG_UTIL_PARAM_CHK();


    for(offset=0;offset<32;offset++)
    {
        DIAG_UTIL_ERR_CHK(rtk_ppstod_frameData_get(offset,&value), ret);
        
        diag_util_mprintf("frame offset %d: 0x%02x\n",offset,value);
    }
    
    return CPARSER_OK;
}    /* end of cparser_cmd_ppstod_dump_frame */

