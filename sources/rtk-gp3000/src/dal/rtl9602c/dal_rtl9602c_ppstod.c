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
 * $Revision: 61228 $
 * $Date: 2016-03-16 14:49:14 +0800 (Thu, 16 Mar 2016) $
 *
 * Purpose : Definition of TIME API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) 1PPSToD
 *
 */

 /*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <dal/rtl9602c/dal_rtl9602c.h>
#include <rtk/ppstod.h>
#include <dal/rtl9602c/dal_rtl9602c_ppstod.h>
#include <rtk/irq.h>
#include <rtk/intr.h>
#include <dal/rtl9602c/dal_rtl9602c_intr.h>

#if defined(CONFIG_SDK_KERNEL_LINUX)&&defined(CONFIG_RTK_PPSTOD)
#include "module/ppstod/ppstod_drv_main.h"
#endif

/*

 * Symbol Definition

 */
#define PPSTOD_MAX_DELAY 63
#define PPSTOD_MAX_PULSE 63
#define PPSTOD_MAX_FRAME_LEN 32
#define PPSTOD_BAUDRATE_115200_DIV_VAL 0x43d
#define PPSTOD_BAUDRATE_19200_DIV_VAL 0x196e
#define PPSTOD_BAUDRATE_9600_DIV_VAL 0x32dd
#define PPSTOD_BAUDRATE_2400_DIV_VAL 0xcb73

#define GPS_UTC_DIFF_SEC 315964800
#define WEEK_SEC 604800
#define SARP_WEEK_TO_SEC(week,sec) sec=week*WEEK_SEC+GPS_UTC_DIFF_SEC
#define SARP_SEC_TO_WEEK(week,sec) week=(sec-GPS_UTC_DIFF_SEC)/WEEK_SEC

static uint32 ppstod_init = { INIT_NOT_COMPLETED } ;

static uint32 _ppstodIntCnt=0;
static uint32 _ptp1ppsIntCnt=0;

/* Function Name:
 *      dal_rtl9602c_ppstod_init
 * Description:
 *      Initialize Time module.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize 1PPSToD module before calling any Time APIs.
 */
int32
dal_rtl9602c_ppstod_init(void)
{
    int32   ret;
    uint32 sec;
    uint32 tmp;
    
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PPSTOD), "%s",__FUNCTION__);

    ppstod_init=INIT_COMPLETED;


    if ((ret = dal_rtl9602c_ppstod_delay_set(0xA)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PPSTOD), "");
        ppstod_init = INIT_NOT_COMPLETED;
        return ret;
    }

    if ((ret = dal_rtl9602c_ppstod_pulseWidth_set(0xA)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PPSTOD), "");
        ppstod_init = INIT_NOT_COMPLETED;
        return ret;
    }

    if ((ret = dal_rtl9602c_ppstod_frameLen_set(0x17)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PPSTOD), "");
        ppstod_init = INIT_NOT_COMPLETED;
        return ret;
    }

    if ((ret = dal_rtl9602c_ppstod_mode_set(PPSTOD_MANUAL_MODE_HW)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PPSTOD), "");
        ppstod_init = INIT_NOT_COMPLETED;
        return ret;
    }

    if ((ret = dal_rtl9602c_ppstod_sarpGpsWeek_set(0x760)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PPSTOD), "");
        ppstod_init = INIT_NOT_COMPLETED;
        return ret;
    }

    
    SARP_WEEK_TO_SEC(0x760,sec);
    if ((ret = dal_rtl9602c_ppstod_sarpUtcSec_set(sec)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PPSTOD), "");
        ppstod_init = INIT_NOT_COMPLETED;
        return ret;
    }
    
    if ((ret = dal_rtl9602c_ppstod_baudrate_set(9600)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PPSTOD), "");
        ppstod_init = INIT_NOT_COMPLETED;
        return ret;
    }

    tmp=0;
    if((ret = reg_write(RTL9602C_SOFTWARE_DEFINE_FRAME_0r,&tmp)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PPSTOD), "");
        ppstod_init = INIT_NOT_COMPLETED;
        return ret;
    }

    tmp=0;
    if((ret = reg_write(RTL9602C_SOFTWARE_DEFINE_FRAME_1r,&tmp)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PPSTOD), "");
        ppstod_init = INIT_NOT_COMPLETED;
        return ret;
    }

    tmp=0;
    if((ret = reg_write(RTL9602C_SOFTWARE_DEFINE_FRAME_2r,&tmp)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PPSTOD), "");
        ppstod_init = INIT_NOT_COMPLETED;
        return ret;
    }

    tmp=0;
    if((ret = reg_write(RTL9602C_SOFTWARE_DEFINE_FRAME_3r,&tmp)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PPSTOD), "");
        ppstod_init = INIT_NOT_COMPLETED;
        return ret;
    }

    tmp=0;
    if((ret = reg_write(RTL9602C_SOFTWARE_DEFINE_FRAME_4r,&tmp)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PPSTOD), "");
        ppstod_init = INIT_NOT_COMPLETED;
        return ret;
    }

    tmp=0;
    if((ret = reg_write(RTL9602C_SOFTWARE_DEFINE_FRAME_5r,&tmp)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PPSTOD), "");
        ppstod_init = INIT_NOT_COMPLETED;
        return ret;
    }

    tmp=0;
    if((ret = reg_write(RTL9602C_SOFTWARE_DEFINE_FRAME_6r,&tmp)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PPSTOD), "");
        ppstod_init = INIT_NOT_COMPLETED;
        return ret;
    }

    tmp=0;
    if((ret = reg_write(RTL9602C_SOFTWARE_DEFINE_FRAME_7r,&tmp)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PPSTOD), "");
        ppstod_init = INIT_NOT_COMPLETED;
        return ret;
    }

    if ((ret = dal_rtl9602c_ppstod_frameData_set(0,0x43)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PPSTOD), "");
        ppstod_init = INIT_NOT_COMPLETED;
        return ret;
    }

    if ((ret = dal_rtl9602c_ppstod_frameData_set(1,0x4D)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PPSTOD), "");
        ppstod_init = INIT_NOT_COMPLETED;
        return ret;
    }    

    if ((ret = dal_rtl9602c_ppstod_frameData_set(2,0x20)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PPSTOD), "");
        ppstod_init = INIT_NOT_COMPLETED;
        return ret;
    }    

    if ((ret = dal_rtl9602c_ppstod_frameData_set(3,0x01)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PPSTOD), "");
        ppstod_init = INIT_NOT_COMPLETED;
        return ret;
    }   

    if ((ret = dal_rtl9602c_ppstod_frameData_set(4,0x00)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PPSTOD), "");
        ppstod_init = INIT_NOT_COMPLETED;
        return ret;
    }  

    if ((ret = dal_rtl9602c_ppstod_frameData_set(5,0x10)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PPSTOD), "");
        ppstod_init = INIT_NOT_COMPLETED;
        return ret;
    }  

    if ((ret = dal_rtl9602c_ppstod_frameData_set(16,0x11)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PPSTOD), "");
        ppstod_init = INIT_NOT_COMPLETED;
        return ret;
    }

    if ((ret = dal_rtl9602c_ppstod_frameData_set(17,0x04)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PPSTOD), "");
        ppstod_init = INIT_NOT_COMPLETED;
        return ret;
    }

    if ((ret = dal_rtl9602c_ppstod_frameData_set(18,0xFF)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PPSTOD), "");
        ppstod_init = INIT_NOT_COMPLETED;
        return ret;
    }


    RT_INIT_CHK(ppstod_init);

    return RT_ERR_OK;
}   /* end of dal_rtl9602c_ppstod_init */

/* Function Name:
 *      dal_rtl9602c_ppstod_delay_get
 * Description:
 *      Get the ToD delay.
 * Input:
 *      None
 * Output:
 *      delay - ToD delay
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_ppstod_delay_get(uint8 *delay)
{
    uint32 ret;
    uint32 tmp;
    
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PPSTOD), "");

    /* check Init status */
    RT_INIT_CHK(ppstod_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == delay), RT_ERR_NULL_POINTER);

    /* function body */
    if ((ret = reg_field_read(RTL9602C_PPS_CTRLr, RTL9602C_TOD_DELAYf, &tmp)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_PPSTOD), "return failed ret value = %x",ret);
        return ret;
    }

    *delay = (uint8)tmp;

    return RT_ERR_OK;
}   /* end of dal_rtl9602c_ppstod_delay_get */

/* Function Name:
 *      dal_rtl9602c_ppstod_todDelay_set
 * Description:
 *      Set the ToD delay.
 * Input:
 *      delay - ToD delay
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      None
 */
int32
dal_rtl9602c_ppstod_delay_set(uint8 delay)
{
    uint32 ret;
    uint32 tmp;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PPSTOD), "delay=%d",delay);

    /* check Init status */
    RT_INIT_CHK(ppstod_init);

    /* parameter check */
    RT_PARAM_CHK((PPSTOD_MAX_DELAY < delay), RT_ERR_INPUT);

    /* function body */
    tmp=delay;
    if ((ret = reg_field_write(RTL9602C_PPS_CTRLr, RTL9602C_TOD_DELAYf, &tmp)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_PPSTOD), "return failed ret value = %x",ret);
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9602c_ppstod_delay_set */

/* Function Name:
 *      dal_rtl9602c_ppstod_pulseWidth_get
 * Description:
 *      Get the 1PPS pulse width
 * Input:
 *      None
 * Output:
 *      width - 1PPS pulse width
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_ppstod_pulseWidth_get(uint8 *width)
{
    uint32 ret;
    uint32 tmp;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PPSTOD), "");

    /* check Init status */
    RT_INIT_CHK(ppstod_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == width), RT_ERR_NULL_POINTER);

    /* function body */
    if ((ret = reg_field_read(RTL9602C_PPS_CTRLr, RTL9602C_PULSE_WIDTHf, &tmp)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_PPSTOD), "return failed ret value = %x",ret);
        return ret;
    }

    *width = (uint8)tmp;

    return RT_ERR_OK;
}   /* end of dal_rtl9602c_ppstod_pulseWidth_get */

/* Function Name:
 *      dal_rtl9602c_ppstod_pulseWidth_set
 * Description:
 *      Set the 1PPS pulse width
 * Input:
 *      width - 1PPS pulse width
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      None
 */
int32
dal_rtl9602c_ppstod_pulseWidth_set(uint8 width)
{
    uint32 ret;
    uint32 tmp;
    
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PPSTOD), "width=%d",width);

    /* check Init status */
    RT_INIT_CHK(ppstod_init);

    /* parameter check */
    RT_PARAM_CHK((PPSTOD_MAX_PULSE < width), RT_ERR_INPUT);

    /* function body */
    tmp=width;
    if ((ret = reg_field_write(RTL9602C_PPS_CTRLr, RTL9602C_PULSE_WIDTHf, &tmp)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_PPSTOD), "return failed ret value = %x",ret);
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9602c_ppstod_pulseWidth_set */

/* Function Name:
 *      dal_rtl9602c_ppstod_mode_get
 * Description:
 *      Get the configuration about software or hardware manual mode define ToD frame.
 * Input:
 *      None
 * Output:
 *      mode - 1PPS TOD manual frame mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_ppstod_mode_get(rtk_ppstod_manualMode_t *mode)
{
    uint32 ret;
    uint32 tmp;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PPSTOD), "");

    /* check Init status */
    RT_INIT_CHK(ppstod_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == mode), RT_ERR_NULL_POINTER);

    /* function body */
    if ((ret = reg_field_read(RTL9602C_TOD_FRAME_CONFIGr, RTL9602C_TOD_FRAME_MANUALf, &tmp)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_PPSTOD), "return failed ret value = %x",ret);
        return ret;
    }

    *mode=tmp;

    return RT_ERR_OK;
}   /* end of dal_rtl9602c_ppstod_mode_get */

/* Function Name:
 *      dal_rtl9602c_ppstod_mode_set
 * Description:
 *      Set the configuration about software or hardware manual define ToD frame.
 * Input:
 *      mode - 1PPS TOD manual framw mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
dal_rtl9602c_ppstod_mode_set(rtk_ppstod_manualMode_t mode)
{
    uint32 ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PPSTOD), "mode=%d",mode);

    /* check Init status */
    RT_INIT_CHK(ppstod_init);

    /* parameter check */
    RT_PARAM_CHK((PPSTOD_MANUAL_MODE_END<=mode), RT_ERR_INPUT);

    /* function body */
    if ((ret = reg_field_write(RTL9602C_TOD_FRAME_CONFIGr, RTL9602C_TOD_FRAME_MANUALf, &mode)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_PPSTOD), "return failed ret value = %x",ret);
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9602c_ppstod_mode_set */


/* Function Name:
 *      dal_rtl9602c_ppstod_sarpGpsWeek_get
 * Description:
 *      Get the software aid reference point GPS week.
 * Input:
 *      None
 * Output:
 *      week - software aid reference point GPS week
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_ppstod_sarpGpsWeek_get(uint16 *week)
{
    uint32 ret;
    uint32 tmp;
    
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PPSTOD), "");

    /* check Init status */
    RT_INIT_CHK(ppstod_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == week), RT_ERR_NULL_POINTER);

    /* function body */
    if ((ret = reg_field_read(RTL9602C_HARDWARE_DEFINE_FRAME_1r, RTL9602C_SARP_GPS_WEEKf, &tmp)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_PPSTOD), "return failed ret value = %x",ret);
        return ret;
    }

    *week=(uint16)tmp;
    
    return RT_ERR_OK;
}   /* end of dal_rtl9602c_ppstod_sarpGpsWeek_get */

/* Function Name:
 *      dal_rtl9602c_ppstod_sarpGpsWeek_set
 * Description:
 *      Set the software aid reference point GPS week.
 * Input:
 *      week - software aid reference point GPS week
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
dal_rtl9602c_ppstod_sarpGpsWeek_set(uint16 week)
{
    uint32 ret;
    uint32 tmp;
    uint32 sec;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PPSTOD), "week=%d",week);

    /* check Init status */
    RT_INIT_CHK(ppstod_init);

    /* parameter check */

    /* function body */
    tmp=week;
    if ((ret = reg_field_write(RTL9602C_HARDWARE_DEFINE_FRAME_1r, RTL9602C_SARP_GPS_WEEKf, &tmp)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_PPSTOD), "return failed ret value = %x",ret);
        return ret;
    }

    SARP_WEEK_TO_SEC(week,sec);
    if ((ret = dal_rtl9602c_ppstod_sarpUtcSec_set(sec)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_PPSTOD), "return failed ret value = %x",ret);
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9602c_ppstod_sarpGpsWeek_set */

/* Function Name:
 *      dal_rtl9602c_ppstod_sarpUtcSec_get
 * Description:
 *      Get the software aid reference point UTC time.
 * Input:
 *      None
 * Output:
 *      sec - software aid reference point UTC time
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_ppstod_sarpUtcSec_get(uint32 *sec)
{
    uint32 ret;
    uint32 tmp;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PPSTOD), "");

    /* check Init status */
    RT_INIT_CHK(ppstod_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == sec), RT_ERR_NULL_POINTER);

    /* function body */
    if ((ret = reg_field_read(RTL9602C_HARDWARE_DEFINE_FRAME_0r, RTL9602C_SARP_UTC_SECf, &tmp)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_PPSTOD), "return failed ret value = %x",ret);
        return ret;
    }

    *sec=tmp;

    return RT_ERR_OK;
}   /* end of dal_rtl9602c_ppstod_sarpUtcSec_get */

/* Function Name:
 *      dal_rtl9602c_ppstod_sarpUtcSec_set
 * Description:
 *      Set the software aid reference point UTC time.
 * Input:
 *      sec - software aid reference point UTC time
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
dal_rtl9602c_ppstod_sarpUtcSec_set(uint32 sec)
{
    uint32 ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PPSTOD), "sec=%d",sec);

    /* check Init status */
    RT_INIT_CHK(ppstod_init);

    /* parameter check */

    /* function body */
    if ((ret = reg_field_write(RTL9602C_HARDWARE_DEFINE_FRAME_0r, RTL9602C_SARP_UTC_SECf, &sec)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_PPSTOD), "return failed ret value = %x",ret);
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9602c_ppstod_sarpUtcSec_set */

/* Function Name:
 *      dal_rtl9602c_ppstod_frameLen_get
 * Description:
 *      Get the configuration about length of define ToD frame.
 * Input:
 *      None
 * Output:
 *      len - length of define ToD frame
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_ppstod_frameLen_get(uint8 *len)
{
    uint32 ret;
    uint32 tmp;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PPSTOD), "");

    /* check Init status */
    RT_INIT_CHK(ppstod_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == len), RT_ERR_NULL_POINTER);

    /* function body */
    if ((ret = reg_field_read(RTL9602C_TOD_FRAME_CONFIGr, RTL9602C_PACKET_LENGTHf, &tmp)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_PPSTOD), "return failed ret value = %x",ret);
        return ret;
    }

    *len=(uint8)tmp;

    return RT_ERR_OK;
}   /* end of dal_rtl9602c_ppstod_frameLen_get */

/* Function Name:
 *      dal_rtl9602c_ppstod_frameLen_set
 * Description:
 *      Set the configuration about length of define ToD frame.
 * Input:
 *      len - length of define ToD frame
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
dal_rtl9602c_ppstod_frameLen_set(uint8 len)
{
    uint32 ret;
    uint32 tmp;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PPSTOD), "len=%d",len);

    /* check Init status */
    RT_INIT_CHK(ppstod_init);

    /* parameter check */
    RT_PARAM_CHK((PPSTOD_MAX_FRAME_LEN < len), RT_ERR_INPUT);

    /* function body */
    tmp=len;
    if ((ret = reg_field_write(RTL9602C_TOD_FRAME_CONFIGr, RTL9602C_PACKET_LENGTHf, &tmp)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_PPSTOD), "return failed ret value = %x",ret);
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9602c_ppstod_frameLen_set */

/* Function Name:
 *      dal_rtl9602c_ppstod_frameData_get
 * Description:
 *      Get the software defined ToD frame.
 * Input:
 *      offset  - offset of frame
 * Output:
 *      val     - value of corresponding offset of frame
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - NULL pointer
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      None
 */
int32
dal_rtl9602c_ppstod_frameData_get(uint8 offset,uint8 *val)
{
    uint32 ret;
    uint32 tmp;
    uint32 reg;
    uint32 field;
    
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PPSTOD), "offset=%d",offset);

    /* check Init status */
    RT_INIT_CHK(ppstod_init);

    /* parameter check */
    RT_PARAM_CHK((32 <=offset), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == val), RT_ERR_NULL_POINTER);

    /* function body */
    switch(offset)
    {
        case 0:
            reg=RTL9602C_SOFTWARE_DEFINE_FRAME_0r;
            field=RTL9602C_SW_TOD_FRAME_0f;
            break;
        case 1:
            reg=RTL9602C_SOFTWARE_DEFINE_FRAME_0r;
            field=RTL9602C_SW_TOD_FRAME_1f;
            break;
        case 2:
            reg=RTL9602C_SOFTWARE_DEFINE_FRAME_0r;
            field=RTL9602C_SW_TOD_FRAME_2f;
            break;
        case 3:
            reg=RTL9602C_SOFTWARE_DEFINE_FRAME_0r;
            field=RTL9602C_SW_TOD_FRAME_3f;
            break;
        case 4:
            reg=RTL9602C_SOFTWARE_DEFINE_FRAME_1r;
            field=RTL9602C_SW_TOD_FRAME_4f;
            break;
        case 5:
            reg=RTL9602C_SOFTWARE_DEFINE_FRAME_1r;
            field=RTL9602C_SW_TOD_FRAME_5f;
            break;
        case 6:
            reg=RTL9602C_SOFTWARE_DEFINE_FRAME_1r;
            field=RTL9602C_SW_TOD_FRAME_6f;
            break;
        case 7:
            reg=RTL9602C_SOFTWARE_DEFINE_FRAME_1r;
            field=RTL9602C_SW_TOD_FRAME_7f;
            break;
        case 8:
            reg=RTL9602C_SOFTWARE_DEFINE_FRAME_2r;
            field=RTL9602C_SW_TOD_FRAME_8f;
            break;
        case 9:
            reg=RTL9602C_SOFTWARE_DEFINE_FRAME_2r;
            field=RTL9602C_SW_TOD_FRAME_9f;
            break;
        case 10:
            reg=RTL9602C_SOFTWARE_DEFINE_FRAME_2r;
            field=RTL9602C_SW_TOD_FRAME_10f;
            break;
        case 11:
            reg=RTL9602C_SOFTWARE_DEFINE_FRAME_2r;
            field=RTL9602C_SW_TOD_FRAME_11f;
            break;
        case 12:
            reg=RTL9602C_SOFTWARE_DEFINE_FRAME_3r;
            field=RTL9602C_SW_TOD_FRAME_12f;
            break;
        case 13:
            reg=RTL9602C_SOFTWARE_DEFINE_FRAME_3r;
            field=RTL9602C_SW_TOD_FRAME_13f;
            break;
        case 14:
            reg=RTL9602C_SOFTWARE_DEFINE_FRAME_3r;
            field=RTL9602C_SW_TOD_FRAME_14f;
            break;
        case 15:
            reg=RTL9602C_SOFTWARE_DEFINE_FRAME_3r;
            field=RTL9602C_SW_TOD_FRAME_15f;
            break;
        case 16:
            reg=RTL9602C_SOFTWARE_DEFINE_FRAME_4r;
            field=RTL9602C_SW_TOD_FRAME_16f;
            break;
        case 17:
            reg=RTL9602C_SOFTWARE_DEFINE_FRAME_4r;
            field=RTL9602C_SW_TOD_FRAME_17f;
            break;
        case 18:
            reg=RTL9602C_SOFTWARE_DEFINE_FRAME_4r;
            field=RTL9602C_SW_TOD_FRAME_18f;
            break;
        case 19:
            reg=RTL9602C_SOFTWARE_DEFINE_FRAME_4r;
            field=RTL9602C_SW_TOD_FRAME_19f;
            break;
        case 20:
            reg=RTL9602C_SOFTWARE_DEFINE_FRAME_5r;
            field=RTL9602C_SW_TOD_FRAME_20f;
            break;
        case 21:
            reg=RTL9602C_SOFTWARE_DEFINE_FRAME_5r;
            field=RTL9602C_SW_TOD_FRAME_21f;
            break;
        case 22:
            reg=RTL9602C_SOFTWARE_DEFINE_FRAME_5r;
            field=RTL9602C_SW_TOD_FRAME_22f;
            break;
        case 23:
            reg=RTL9602C_SOFTWARE_DEFINE_FRAME_5r;
            field=RTL9602C_SW_TOD_FRAME_23f;
            break;
        case 24:
            reg=RTL9602C_SOFTWARE_DEFINE_FRAME_6r;
            field=RTL9602C_SW_TOD_FRAME_24f;
            break;
        case 25:
            reg=RTL9602C_SOFTWARE_DEFINE_FRAME_6r;
            field=RTL9602C_SW_TOD_FRAME_25f;
            break;
        case 26:
            reg=RTL9602C_SOFTWARE_DEFINE_FRAME_6r;
            field=RTL9602C_SW_TOD_FRAME_26f;
            break;
        case 27:
            reg=RTL9602C_SOFTWARE_DEFINE_FRAME_6r;
            field=RTL9602C_SW_TOD_FRAME_27f;
            break;
        case 28:
            reg=RTL9602C_SOFTWARE_DEFINE_FRAME_7r;
            field=RTL9602C_SW_TOD_FRAME_28f;
            break;
        case 29:
            reg=RTL9602C_SOFTWARE_DEFINE_FRAME_7r;
            field=RTL9602C_SW_TOD_FRAME_29f;
            break;
        case 30:
            reg=RTL9602C_SOFTWARE_DEFINE_FRAME_7r;
            field=RTL9602C_SW_TOD_FRAME_30f;
            break;
        case 31:
            reg=RTL9602C_SOFTWARE_DEFINE_FRAME_7r;
            field=RTL9602C_SW_TOD_FRAME_31f;
            break;
        default:
            break;
    }   
    
    if ((ret = reg_field_read(reg, field, &tmp)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_PPSTOD), "return failed ret value = %x",ret);
        return ret;
    }

    *val=(uint8)tmp;

    return RT_ERR_OK;
}   /* end of dal_rtl9602c_ppstod_frameData_get */

/* Function Name:
 *      dal_rtl9602c_ppstod_frameData_set
 * Description:
 *      Set the software defined ToD frame.
 * Input:
 *      offset  - offset of frame
 *      val     - value of corresponding offset of frame
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - NULL pointer
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      None
 */
int32
dal_rtl9602c_ppstod_frameData_set(uint8 offset,uint8 val)
{
    uint32 ret;
    uint32 tmp;
    uint32 reg;
    uint32 field;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PPSTOD), "offset=%d,val=%d",offset, val);

    /* check Init status */
    RT_INIT_CHK(ppstod_init);

    /* parameter check */
    RT_PARAM_CHK((PPSTOD_MAX_FRAME_LEN <=offset), RT_ERR_INPUT);

    /* function body */
    switch(offset)
    {
        case 0:
            reg=RTL9602C_SOFTWARE_DEFINE_FRAME_0r;
            field=RTL9602C_SW_TOD_FRAME_0f;
            break;
        case 1:
            reg=RTL9602C_SOFTWARE_DEFINE_FRAME_0r;
            field=RTL9602C_SW_TOD_FRAME_1f;
            break;
        case 2:
            reg=RTL9602C_SOFTWARE_DEFINE_FRAME_0r;
            field=RTL9602C_SW_TOD_FRAME_2f;
            break;
        case 3:
            reg=RTL9602C_SOFTWARE_DEFINE_FRAME_0r;
            field=RTL9602C_SW_TOD_FRAME_3f;
            break;
        case 4:
            reg=RTL9602C_SOFTWARE_DEFINE_FRAME_1r;
            field=RTL9602C_SW_TOD_FRAME_4f;
            break;
        case 5:
            reg=RTL9602C_SOFTWARE_DEFINE_FRAME_1r;
            field=RTL9602C_SW_TOD_FRAME_5f;
            break;
        case 6:
            reg=RTL9602C_SOFTWARE_DEFINE_FRAME_1r;
            field=RTL9602C_SW_TOD_FRAME_6f;
            break;
        case 7:
            reg=RTL9602C_SOFTWARE_DEFINE_FRAME_1r;
            field=RTL9602C_SW_TOD_FRAME_7f;
            break;
        case 8:
            reg=RTL9602C_SOFTWARE_DEFINE_FRAME_2r;
            field=RTL9602C_SW_TOD_FRAME_8f;
            break;
        case 9:
            reg=RTL9602C_SOFTWARE_DEFINE_FRAME_2r;
            field=RTL9602C_SW_TOD_FRAME_9f;
            break;
        case 10:
            reg=RTL9602C_SOFTWARE_DEFINE_FRAME_2r;
            field=RTL9602C_SW_TOD_FRAME_10f;
            break;
        case 11:
            reg=RTL9602C_SOFTWARE_DEFINE_FRAME_2r;
            field=RTL9602C_SW_TOD_FRAME_11f;
            break;
        case 12:
            reg=RTL9602C_SOFTWARE_DEFINE_FRAME_3r;
            field=RTL9602C_SW_TOD_FRAME_12f;
            break;
        case 13:
            reg=RTL9602C_SOFTWARE_DEFINE_FRAME_3r;
            field=RTL9602C_SW_TOD_FRAME_13f;
            break;
        case 14:
            reg=RTL9602C_SOFTWARE_DEFINE_FRAME_3r;
            field=RTL9602C_SW_TOD_FRAME_14f;
            break;
        case 15:
            reg=RTL9602C_SOFTWARE_DEFINE_FRAME_3r;
            field=RTL9602C_SW_TOD_FRAME_15f;
            break;
        case 16:
            reg=RTL9602C_SOFTWARE_DEFINE_FRAME_4r;
            field=RTL9602C_SW_TOD_FRAME_16f;
            break;
        case 17:
            reg=RTL9602C_SOFTWARE_DEFINE_FRAME_4r;
            field=RTL9602C_SW_TOD_FRAME_17f;
            break;
        case 18:
            reg=RTL9602C_SOFTWARE_DEFINE_FRAME_4r;
            field=RTL9602C_SW_TOD_FRAME_18f;
            break;
        case 19:
            reg=RTL9602C_SOFTWARE_DEFINE_FRAME_4r;
            field=RTL9602C_SW_TOD_FRAME_19f;
            break;
        case 20:
            reg=RTL9602C_SOFTWARE_DEFINE_FRAME_5r;
            field=RTL9602C_SW_TOD_FRAME_20f;
            break;
        case 21:
            reg=RTL9602C_SOFTWARE_DEFINE_FRAME_5r;
            field=RTL9602C_SW_TOD_FRAME_21f;
            break;
        case 22:
            reg=RTL9602C_SOFTWARE_DEFINE_FRAME_5r;
            field=RTL9602C_SW_TOD_FRAME_22f;
            break;
        case 23:
            reg=RTL9602C_SOFTWARE_DEFINE_FRAME_5r;
            field=RTL9602C_SW_TOD_FRAME_23f;
            break;
        case 24:
            reg=RTL9602C_SOFTWARE_DEFINE_FRAME_6r;
            field=RTL9602C_SW_TOD_FRAME_24f;
            break;
        case 25:
            reg=RTL9602C_SOFTWARE_DEFINE_FRAME_6r;
            field=RTL9602C_SW_TOD_FRAME_25f;
            break;
        case 26:
            reg=RTL9602C_SOFTWARE_DEFINE_FRAME_6r;
            field=RTL9602C_SW_TOD_FRAME_26f;
            break;
        case 27:
            reg=RTL9602C_SOFTWARE_DEFINE_FRAME_6r;
            field=RTL9602C_SW_TOD_FRAME_27f;
            break;
        case 28:
            reg=RTL9602C_SOFTWARE_DEFINE_FRAME_7r;
            field=RTL9602C_SW_TOD_FRAME_28f;
            break;
        case 29:
            reg=RTL9602C_SOFTWARE_DEFINE_FRAME_7r;
            field=RTL9602C_SW_TOD_FRAME_29f;
            break;
        case 30:
            reg=RTL9602C_SOFTWARE_DEFINE_FRAME_7r;
            field=RTL9602C_SW_TOD_FRAME_30f;
            break;
        case 31:
            reg=RTL9602C_SOFTWARE_DEFINE_FRAME_7r;
            field=RTL9602C_SW_TOD_FRAME_31f;
            break;
        default:
            break;
    }   

    tmp=val;
    if ((ret = reg_field_write(reg, field, &tmp)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_PPSTOD), "return failed ret value = %x",ret);
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9602c_ppstod_frameData_set */

/* Function Name:
 *      dal_rtl9602c_ppstod_baudrate_get
 * Description:
 *      Get the baudrate.
 * Input:
 *      None
 * Output:
 *      rate - baudrate
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_ppstod_baudrate_get(uint32 *rate)
{
    uint32 ret;
    uint32 tmp;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PPSTOD), "");

    /* check Init status */
    RT_INIT_CHK(ppstod_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == rate), RT_ERR_NULL_POINTER);

    /* function body */
    if ((ret = reg_field_read(RTL9602C_TOD_BAUDRATEr, RTL9602C_DIVISOR_LATCH_VALUEf, &tmp)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_PPSTOD), "return failed ret value = %x",ret);
        return ret;
    }

    if(tmp==PPSTOD_BAUDRATE_115200_DIV_VAL)
    {
        *rate=115200;
    }
    else if(tmp==PPSTOD_BAUDRATE_19200_DIV_VAL)
    {
        *rate=19200;
    }
    else if(tmp==PPSTOD_BAUDRATE_9600_DIV_VAL)
    {
        *rate=9600;
    }
    else if(tmp==PPSTOD_BAUDRATE_2400_DIV_VAL)
    {
        *rate=2400;
    }
    else
    {
        *rate=125000000/tmp;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9602c_ppstod_baudrate_get */

/* Function Name:
 *      dal_rtl9602c_ppstod_baudrate_set
 * Description:
 *      Set the baudrate.
 * Input:
 *      rate - baudrate
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
dal_rtl9602c_ppstod_baudrate_set(uint32 rate)
{
    uint32 ret;
    uint32 tmp;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PPSTOD), "rate=%d",rate);

    /* check Init status */
    RT_INIT_CHK(ppstod_init);

    /* parameter check */

    /* function body */
    if(rate==115200)
    {
        tmp=PPSTOD_BAUDRATE_115200_DIV_VAL;
    }
    else if(rate==19200)
    {
        tmp=PPSTOD_BAUDRATE_19200_DIV_VAL;
    }
    else if(rate==9600)
    {
        tmp=PPSTOD_BAUDRATE_9600_DIV_VAL;
    }
    else if(rate==2400)
    {
        tmp=PPSTOD_BAUDRATE_2400_DIV_VAL;
    }
    else
    {
        tmp=125000000/rate;
    }
    
    if ((ret = reg_field_write(RTL9602C_TOD_BAUDRATEr, RTL9602C_DIVISOR_LATCH_VALUEf, &tmp)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_PPSTOD), "return failed ret value = %x",ret);
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9602c_ppstod_baudrate_set */

void dal_rtl9602c_ppstod_isr_entry(void)
{
    int32  ret;
    uint8 offset;
    uint8 val;
    
    if((ret = dal_rtl9602c_intr_imr_set(INTR_TYPE_TOD,DISABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret,(MOD_PPSTOD | MOD_DAL), "");
        return ;
    }

    _ppstodIntCnt ++;

#if defined(CONFIG_SDK_KERNEL_LINUX)&&defined(CONFIG_RTK_PPSTOD)
    ppstod_sw_interrupt();
#else
    for(offset=0;offset<32;offset++)
    {
        if ((ret = dal_rtl9602c_ppstod_frameData_get(offset,&val)) != RT_ERR_OK)
        {
            RT_ERR(ret,(MOD_PPSTOD | MOD_DAL), "");
            return ;
        }
        
        val++;
        
        if ((ret = dal_rtl9602c_ppstod_frameData_set(offset,val)) != RT_ERR_OK)
        {
            RT_ERR(ret,(MOD_PPSTOD | MOD_DAL), "");
            return ;
        }
    }  
#endif

    /* switch interrupt clear EPON state */
    if((ret=dal_rtl9602c_intr_ims_clear(INTR_TYPE_TOD)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PPSTOD | MOD_DAL), "");
        return ;
    }

    if((ret = dal_rtl9602c_intr_imr_set(INTR_TYPE_TOD,ENABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret,(MOD_PPSTOD | MOD_DAL), "");
        return ;
    }
    return ; 
    }

int32 dal_rtl9602c_ppstod_isr_counter_get(uint32 *counter)
{
    /* parameter check */
    RT_PARAM_CHK((NULL == counter), RT_ERR_NULL_POINTER);

    /* function body */
    *counter = _ppstodIntCnt;

    /*reset counter*/
    _ppstodIntCnt=0;

    return RT_ERR_OK; 
}

void dal_rtl9602c_ptp1pps_isr_entry(void)
{
    int32  ret;
    
    if((ret = dal_rtl9602c_intr_imr_set(INTR_TYPE_PTP,DISABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret,(MOD_PPSTOD | MOD_DAL), "");
        return ;
    }

    _ptp1ppsIntCnt ++;

#if defined(CONFIG_SDK_KERNEL_LINUX)&&defined(CONFIG_RTK_PPSTOD)
    ppstod_sw_interrupt();
#endif
    /* switch interrupt clear EPON state */
    if((ret=dal_rtl9602c_intr_ims_clear(INTR_TYPE_PTP)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PPSTOD | MOD_DAL), "");
        return ;
    }

    if((ret = dal_rtl9602c_intr_imr_set(INTR_TYPE_PTP,ENABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret,(MOD_PPSTOD | MOD_DAL), "");
        return ;
    }
    return ; 
}

int32 dal_rtl9602c_ptp1pps_isr_counter_get(uint32 *counter)
{
    /* parameter check */
    RT_PARAM_CHK((NULL == counter), RT_ERR_NULL_POINTER);

    /* function body */
    *counter = _ptp1ppsIntCnt;

    /*reset counter*/
    _ptp1ppsIntCnt=0;

    return RT_ERR_OK;
}

