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
#ifndef __RTK_PPSTOD_H__
#define __RTK_PPSTOD_H__

typedef enum rtk_ppstod_manualMode_e
{
    PPSTOD_MANUAL_MODE_HW = 0,
    PPSTOD_MANUAL_MODE_SW,
    PPSTOD_MANUAL_MODE_END
} rtk_ppstod_manualMode_t;


/* Function Name:
 *      rtk_ppstod_init
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
extern int32
rtk_ppstod_init(void);

/* Function Name:
 *      rtk_ppstod_delay_get
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
extern int32
rtk_ppstod_delay_get(uint8 *delay);

/* Function Name:
 *      rtk_ppstod_todDelay_set
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
extern int32
rtk_ppstod_delay_set(uint8 delay);

/* Function Name:
 *      rtk_ppstod_pulseWidth_get
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
extern int32
rtk_ppstod_pulseWidth_get(uint8 *width);

/* Function Name:
 *      rtk_ppstod_pulseWidth_set
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
extern int32
rtk_ppstod_pulseWidth_set(uint8 width);

/* Function Name:
 *      rtk_ppstod_mode_get
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
extern int32
rtk_ppstod_mode_get(rtk_ppstod_manualMode_t *mode);

/* Function Name:
 *      rtk_ppstod_mode_set
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
extern int32
rtk_ppstod_mode_set(rtk_ppstod_manualMode_t mode);



/* Function Name:
 *      rtk_ppstod_sarpGpsWeek_get
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
extern int32
rtk_ppstod_sarpGpsWeek_get(uint16 *week);

/* Function Name:
 *      rtk_ppstod_sarpGpsWeek_set
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
extern int32
rtk_ppstod_sarpGpsWeek_set(uint16 week);

/* Function Name:
 *      rtk_ppstod_sarpUtcSec_get
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
extern int32
rtk_ppstod_sarpUtcSec_get(uint32 *sec);

/* Function Name:
 *      rtk_ppstod_sarpUtcSec_set
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
extern int32
rtk_ppstod_sarpUtcSec_set(uint32 sec);

/* Function Name:
 *      rtk_ppstod_frameLen_get
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
extern int32
rtk_ppstod_frameLen_get(uint8 *len);

/* Function Name:
 *      rtk_ppstod_frameLen_set
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
extern int32
rtk_ppstod_frameLen_set(uint8 len);

/* Function Name:
 *      rtk_ppstod_frameData_get
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
extern int32
rtk_ppstod_frameData_get(uint8 offset,uint8 *val);

/* Function Name:
 *      rtk_ppstod_frameData_set
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
extern int32
rtk_ppstod_frameData_set(uint8 offset,uint8 val);

/* Function Name:
 *      rtk_ppstod_baudrate_get
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
extern int32
rtk_ppstod_baudrate_get(uint32 *rate);

/* Function Name:
 *      rtk_ppstod_baudrate_set
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
extern int32
rtk_ppstod_baudrate_set(uint32 rate);

 
#endif /* __RTK_PPSTOD_H__ */
