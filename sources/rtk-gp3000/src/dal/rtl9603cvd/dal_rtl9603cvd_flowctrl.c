/*
 * Copyright (C) 2011 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * Purpose : switch asic-level Flow Control API
 * Feature : Flow Control related functions
 *
 */

#include <dal/rtl9603cvd/dal_rtl9603cvd_flowctrl.h>
#include <osal/time.h>


/* Function Name:
 *      rtl9603cvd_raw_flowctrl_dropAllThreshold_set
 * Description:
 *      Set system flow control drop all threshold
 * Input:
 *      threshold         - drop all threshold
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_dropAllThreshold_set(uint32 threshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < threshold), RT_ERR_INPUT);

    if ((ret = reg_field_write(RTL9603CVD_FC_DROP_ALL_THr, RTL9603CVD_THf, &threshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_dropAllThreshold_set */





/* Function Name:
 *      rtl9603cvd_raw_flowctrl_dropAllThreshold_get
 * Description:
 *      Get system flow control drop all threshold
 * Input:
 *      None
 * Output:
 *      pThreshold         - drop all threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_dropAllThreshold_get(uint32 *pThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pThreshold == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9603CVD_FC_DROP_ALL_THr, RTL9603CVD_THf, pThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_dropAllThreshold_get */

/* Function Name:
 *      rtl9603cvd_raw_flowctrl_pauseAllThreshold_set
 * Description:
 *      Set system flow control pause all threshold
 * Input:
 *      threshold         - pause all threshold
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_pauseAllThreshold_set(uint32 threshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < threshold), RT_ERR_INPUT);

    if ((ret = reg_field_write(RTL9603CVD_FC_PAUSE_ALL_THr, RTL9603CVD_THf, &threshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_pauseAllThreshold_set */

/* Function Name:
 *      rtl9603cvd_raw_flowctrl_pauseAllThreshold_get
 * Description:
 *      Get system flow control pause all threshold
 * Input:
 *      None
 * Output:
 *      pThreshold         - pause all threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_pauseAllThreshold_get(uint32 *pThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pThreshold == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9603CVD_FC_PAUSE_ALL_THr, RTL9603CVD_THf, pThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_pauseAllThreshold_get */

/* Function Name:
 *      rtl9603cvd_raw_flowctrl_globalFcoffHighThreshold_set
 * Description:
 *      Set global high on/off threshold when flow contrl off.
 * Input:
 *      onThreshold         - global fc-off high on threshold
 *      offThreshold         - global fc-off high off threshold
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_globalFcoffHighThreshold_set(uint32 onThreshold, uint32 offThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < onThreshold), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < offThreshold), RT_ERR_INPUT);

    if ((ret = reg_field_write(RTL9603CVD_FC_GLB_FCOFF_HI_THr, RTL9603CVD_ON_THf, &onThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_write(RTL9603CVD_FC_GLB_FCOFF_HI_THr, RTL9603CVD_OFF_THf, &offThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_globalFcoffHighThreshold_set */

/* Function Name:
 *      rtl9603cvd_raw_flowctrl_globalFcoffHighThreshold_get
 * Description:
 *      Get global high on/off threshold when flow contrl off.
 * Input:
 *      None
 * Output:
 *      pOnThreshold         - global fc-off high on threshold
 *      pOffThreshold         - global fc-off high off threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_globalFcoffHighThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pOnThreshold == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pOffThreshold == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9603CVD_FC_GLB_FCOFF_HI_THr, RTL9603CVD_ON_THf, pOnThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_read(RTL9603CVD_FC_GLB_FCOFF_HI_THr, RTL9603CVD_OFF_THf, pOffThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_globalFcoffHighThreshold_get */

/* Function Name:
 *      rtl9603cvd_raw_flowctrl_globalFcoffLowThreshold_set
 * Description:
 *      Set global low on/off threshold when flow contrl off.
 * Input:
 *      onThreshold         - global fc-off low on threshold
 *      offThreshold         - global fc-off lowoff threshold
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_globalFcoffLowThreshold_set(uint32 onThreshold, uint32 offThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < onThreshold), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < offThreshold), RT_ERR_INPUT);

    if ((ret = reg_field_write(RTL9603CVD_FC_GLB_FCOFF_LO_THr, RTL9603CVD_ON_THf, &onThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_write(RTL9603CVD_FC_GLB_FCOFF_LO_THr, RTL9603CVD_OFF_THf, &offThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_globalFcoffLowThreshold_set */

/* Function Name:
 *      rtl9603cvd_raw_flowctrl_globalFcoffLowThreshold_get
 * Description:
 *      Get global low on/off threshold when flow contrl off.
 * Input:
 *      None
 * Output:
 *      pOnThreshold         - global fc-off low on threshold
 *      pOffThreshold         - global fc-off low off threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_globalFcoffLowThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pOnThreshold == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pOffThreshold == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9603CVD_FC_GLB_FCOFF_LO_THr, RTL9603CVD_ON_THf, pOnThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_read(RTL9603CVD_FC_GLB_FCOFF_LO_THr, RTL9603CVD_OFF_THf, pOffThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_globalFcoffLowThreshold_get */





/* Function Name:
 *      rtl9603cvd_raw_flowctrl_globalFconHighThreshold_set
 * Description:
 *      Set global high on/off threshold when flow contrl on.
 * Input:
 *      onThreshold         - global fc-on high on threshold
 *      offThreshold         - global fc-on high off threshold
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_globalFconHighThreshold_set(uint32 onThreshold, uint32 offThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < onThreshold), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < offThreshold), RT_ERR_INPUT);

    if ((ret = reg_field_write(RTL9603CVD_FC_GLB_HI_THr, RTL9603CVD_ON_THf, &onThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_write(RTL9603CVD_FC_GLB_HI_THr, RTL9603CVD_OFF_THf, &offThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_globalFconHighThreshold_set */

/* Function Name:
 *      rtl9603cvd_raw_flowctrl_globalFconHighThreshold_get
 * Description:
 *      Get global high on/off threshold when flow contrl on.
 * Input:
 *      None
 * Output:
 *      pOnThreshold         - global fc-on high on threshold
 *      pOffThreshold         - global fc-on high off threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_globalFconHighThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pOnThreshold == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pOffThreshold == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9603CVD_FC_GLB_HI_THr, RTL9603CVD_ON_THf, pOnThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_read(RTL9603CVD_FC_GLB_HI_THr, RTL9603CVD_OFF_THf, pOffThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_globalFconHighThreshold_get */


/* Function Name:
 *      rtl9603cvd_raw_flowctrl_globalFconLowThreshold_set
 * Description:
 *      Set global low on/off threshold when flow contrl on.
 * Input:
 *      onThreshold         - global fc-on low on threshold
 *      offThreshold         - global fc-on low off threshold
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_globalFconLowThreshold_set(uint32 onThreshold, uint32 offThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < onThreshold), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < offThreshold), RT_ERR_INPUT);

    if ((ret = reg_field_write(RTL9603CVD_FC_GLB_LO_THr, RTL9603CVD_ON_THf, &onThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_write(RTL9603CVD_FC_GLB_LO_THr, RTL9603CVD_OFF_THf, &offThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_globalFconLowThreshold_set */

/* Function Name:
 *      rtl9603cvd_raw_flowctrl_globalFconLowThreshold_get
 * Description:
 *      Get global low on/off threshold when flow contrl on.
 * Input:
 *      None
 * Output:
 *      pOnThreshold         - global fc-on low on threshold
 *      pOffThreshold         - global fc-on low off threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_globalFconLowThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pOnThreshold == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pOffThreshold == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9603CVD_FC_GLB_LO_THr, RTL9603CVD_ON_THf, pOnThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_read(RTL9603CVD_FC_GLB_LO_THr, RTL9603CVD_OFF_THf, pOffThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_globalFconLowThreshold_get */

/* Function Name:
 *      rtl9603cvd_raw_flowctrl_portFconHighThreshold_set
 * Description:
 *      Set per port high on/off threshold when flow contrl on.
 * Input:
 *      onThreshold         - per port fc-on high on threshold
 *      offThreshold         - per port fc-on high off threshold
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_portFconHighThreshold_set(uint32 onThreshold, uint32 offThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < onThreshold), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < offThreshold), RT_ERR_INPUT);

    if ((ret = reg_field_write(RTL9603CVD_FC_P_HI_THr, RTL9603CVD_ON_THf, &onThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_write(RTL9603CVD_FC_P_HI_THr, RTL9603CVD_OFF_THf, &offThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_portFconHighThreshold_set */

/* Function Name:
 *      rtl9603cvd_raw_flowctrl_portFconHighThreshold_get
 * Description:
 *      Get per port high on/off threshold when flow contrl on.
 * Input:
 *      None
 * Output:
 *      pOnThreshold         - per port fc-on high on threshold
 *      pOffThreshold         - per port fc-on high off threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_portFconHighThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pOnThreshold == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pOffThreshold == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9603CVD_FC_P_HI_THr, RTL9603CVD_ON_THf, pOnThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_read(RTL9603CVD_FC_P_HI_THr, RTL9603CVD_OFF_THf, pOffThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_portFconHighThreshold_get */

/* Function Name:
 *      rtl9603cvd_raw_flowctrl_globalFconLowThreshold_set
 * Description:
 *      Set per port low on/off threshold when flow contrl on.
 * Input:
 *      onThreshold         - per port fc-on low on threshold
 *      offThreshold         - per port fc-on low off threshold
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_portFconLowThreshold_set(uint32 onThreshold, uint32 offThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < onThreshold), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < offThreshold), RT_ERR_INPUT);

    if ((ret = reg_field_write(RTL9603CVD_FC_P_LO_THr, RTL9603CVD_ON_THf, &onThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_write(RTL9603CVD_FC_P_LO_THr, RTL9603CVD_OFF_THf, &offThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_portFconLowThreshold_set */

/* Function Name:
 *      rtl9603cvd_raw_flowctrl_portFconLowThreshold_get
 * Description:
 *      Get per port low on/off threshold when flow contrl on.
 * Input:
 *      None
 * Output:
 *      pOnThreshold         - per port fc-on high on threshold
 *      pOffThreshold         - per port fc-on high off threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_portFconLowThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pOnThreshold == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pOffThreshold == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9603CVD_FC_P_LO_THr, RTL9603CVD_ON_THf, pOnThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_read(RTL9603CVD_FC_P_LO_THr, RTL9603CVD_OFF_THf, pOffThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_portFconLowThreshold_get */



/* Function Name:
 *      rtl9603cvd_raw_flowctrl_portFcoffHighThreshold_set
 * Description:
 *      Set per port high on/off threshold when flow contrl off.
 * Input:
 *      onThreshold         - per port fc-off high on threshold
 *      offThreshold         - per port fc-off high off threshold
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_portFcoffHighThreshold_set(uint32 onThreshold, uint32 offThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < onThreshold), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < offThreshold), RT_ERR_INPUT);

    if ((ret = reg_field_write(RTL9603CVD_FC_P_FCOFF_HI_THr, RTL9603CVD_ON_THf, &onThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_write(RTL9603CVD_FC_P_FCOFF_HI_THr, RTL9603CVD_OFF_THf, &offThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_portFcoffHighThreshold_set */

/* Function Name:
 *      rtl9603cvd_raw_flowctrl_portFcoffHighThreshold_get
 * Description:
 *      Get per port high on/off threshold when flow contrl off.
 * Input:
 *      None
 * Output:
 *      pOnThreshold         - per port fc-off high on threshold
 *      pOffThreshold         - per port fc-off high off threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_portFcoffHighThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pOnThreshold == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pOffThreshold == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9603CVD_FC_P_FCOFF_HI_THr, RTL9603CVD_ON_THf, pOnThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_read(RTL9603CVD_FC_P_FCOFF_HI_THr, RTL9603CVD_OFF_THf, pOffThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_portFcoffHighThreshold_get */


/* Function Name:
 *      rtl9603cvd_raw_flowctrl_portFcoffLowThreshold_set
 * Description:
 *      Set per port low on/off threshold when flow contrl off.
 * Input:
 *      onThreshold         - per port fc-off low on threshold
 *      offThreshold         - per port fc-off low off threshold
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_portFcoffLowThreshold_set(uint32 onThreshold, uint32 offThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < onThreshold), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < offThreshold), RT_ERR_INPUT);

    if ((ret = reg_field_write(RTL9603CVD_FC_P_FCOFF_LO_THr, RTL9603CVD_ON_THf, &onThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_write(RTL9603CVD_FC_P_FCOFF_LO_THr, RTL9603CVD_OFF_THf, &offThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_portFcoffLowThreshold_set */

/* Function Name:
 *      rtl9603cvd_raw_flowctrl_portFcoffLowThreshold_get
 * Description:
 *      Get per port low on/off threshold when flow contrl off.
 * Input:
 *      None
 * Output:
 *      pOnThreshold         - per port fc-off high on threshold
 *      pOffThreshold         - per port fc-off high off threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_portFcoffLowThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pOnThreshold == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pOffThreshold == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9603CVD_FC_P_FCOFF_LO_THr, RTL9603CVD_ON_THf, pOnThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_read(RTL9603CVD_FC_P_FCOFF_LO_THr, RTL9603CVD_OFF_THf, pOffThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_portFcoffLowThreshold_get */


/* Function Name:
 *      rtl9603cvd_raw_flowctrl_queueEegressDropThreshold_set
 * Description:
 *      Set queue flow control egress drop threshold
 * Input:
 *      queue               - queue id
 *      threshold           - drop all threshold
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_queueEegressDropThreshold_set(uint32 queue, uint32 threshold)
{
    int32   ret;

    RT_PARAM_CHK((HAL_QUEUE_ID_MAX()  < queue), RT_ERR_QUEUE_ID);

    /*parameter check*/
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < threshold), RT_ERR_INPUT);

    if ((ret = reg_array_field_write(RTL9603CVD_FC_Q_EGR_DROP_THr, REG_ARRAY_INDEX_NONE, queue, RTL9603CVD_THf, &threshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_queueEegressDropThreshold_set */

/* Function Name:
 *      rtl9603cvd_raw_flowctrl_queueEegressDropThreshold_get
 * Description:
 *      Get queue flow control egress drop threshold
 * Input:
 *      queue               - queue id
 * Output:
 *      pThreshold         - drop all threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_queueEegressDropThreshold_get(uint32 queue, uint32 *pThreshold)
{
    int32   ret;

    RT_PARAM_CHK((HAL_QUEUE_ID_MAX()  < queue), RT_ERR_QUEUE_ID);

    /*parameter check*/
    RT_PARAM_CHK((pThreshold == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(RTL9603CVD_FC_Q_EGR_DROP_THr, REG_ARRAY_INDEX_NONE, queue, RTL9603CVD_THf, pThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_queueEegressDropThreshold_get */


/* Function Name:
 *      rtl9603cvd_raw_flowctrl_portEegressDropThreshold_set
 * Description:
 *      Set port flow control egress drop threshold
 * Input:
 *      port                   - Physical port number (0~6)
 *      threshold            - drop all threshold
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_portEegressDropThreshold_set(uint32 port, uint32 threshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < threshold), RT_ERR_INPUT);

    if ((ret = reg_array_field_write(RTL9603CVD_FC_P_EGR_DROP_THr, port, REG_ARRAY_INDEX_NONE, RTL9603CVD_THf, &threshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_portEegressDropThreshold_set */

/* Function Name:
 *      rtl9603cvd_raw_flowctrl_portEegressDropThreshold_get
 * Description:
 *      Get port flow control egress drop threshold
 * Input:
 *      port                   - Physical port number (0~6)
 * Output:
 *      pThreshold         - drop all threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_portEegressDropThreshold_get(uint32 port, uint32 *pThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((pThreshold == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(RTL9603CVD_FC_P_EGR_DROP_THr, port, REG_ARRAY_INDEX_NONE, RTL9603CVD_THf, pThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_portEegressDropThreshold_get */

/* Function Name:
 *      rtl9603cvd_raw_flowctrl_egressDropEnable_set
 * Description:
 *      Set enable to per queue egress drop
 * Input:
 *      port                   - Physical port number (0~2)
 *      queue                 - queue id (0~7)
 *      enable                - enable per queue egress drop
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_egressDropEnable_set(uint32 port, uint32 queue, rtk_enable_t enable)
{
    int32   ret;
    uint32 reg;

    /*parameter check*/
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((HAL_QUEUE_ID_MAX()  < queue), RT_ERR_QUEUE_ID);

    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    if (ENABLED == enable)
        reg = 0;
    else
        reg = 1;

    if ((ret = reg_array_field_write(RTL9603CVD_FC_P_Q_EGR_FC_ENr, port, queue, RTL9603CVD_ENf, &reg)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_egressDropEnable_set */

/* Function Name:
 *      rtl9603cvd_raw_flowctrl_egressDropEnable_get
 * Description:
 *      Get  per queue egress drop status
 * Input:
 *      port                   - Physical port number (0~2)
 *      queue                 - queue id (0~7)
 * Output:
 *      pEnable                - per queue egress drop status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_egressDropEnable_get(uint32 port, uint32 queue, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32 reg;

    /*parameter check*/
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((HAL_QUEUE_ID_MAX()  < queue), RT_ERR_QUEUE_ID);

    RT_PARAM_CHK((pEnable == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(RTL9603CVD_FC_P_Q_EGR_FC_ENr, port, queue, RTL9603CVD_ENf, &reg)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL|MOD_DAL), "");
        return ret;
    }

    if (ENABLED == reg)
        *pEnable = 0;
    else
        *pEnable = 1;


    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_egressDropEnable_get */

/* Function Name:
 *      rtl9603cvd_raw_flowctrl_ponEgressDropEnable_set
 * Description:
 *      Set enable to per queue egress drop for packet from PON port
 * Input:
 *      port                  - Physical port number (0~2)
 *      queue                 - queue id (0~7)
 *      enable                - enable per queue egress drop
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_ponEgressDropEnable_set(uint32 port, uint32 queue, rtk_enable_t enable)
{
    int32   ret;
    uint32 reg;

    /*parameter check*/
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((HAL_QUEUE_ID_MAX()  < queue), RT_ERR_QUEUE_ID);

    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    if (ENABLED == enable)
        reg = 0;
    else
        reg = 1;

    if ((ret = reg_array_field_write(RTL9603CVD_PON_FC_P_Q_EGR_FC_ENr, port, queue, RTL9603CVD_FC_ENf, &reg)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9602c_raw_flowctrl_ponEgressDropEnable_set */

/* Function Name:
 *      rtl9603cvd_raw_flowctrl_ponEgressDropEnable_get
 * Description:
 *      Get  per queue egress drop status for packet from PON port
 * Input:
 *      port                   - Physical port number (0~2)
 *      queue                 - queue id (0~7)
 * Output:
 *      pEnable                - per queue egress drop status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_ponEgressDropEnable_get(uint32 port, uint32 queue, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32 reg;

    /*parameter check*/
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((HAL_QUEUE_ID_MAX()  < queue), RT_ERR_QUEUE_ID);

    RT_PARAM_CHK((pEnable == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(RTL9603CVD_PON_FC_P_Q_EGR_FC_ENr, port, queue, RTL9603CVD_FC_ENf, &reg)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL|MOD_DAL), "");
        return ret;
    }

    if (ENABLED == reg)
        *pEnable = 0;
    else
        *pEnable = 1;


    return RT_ERR_OK;
} /* end of rtl9602c_raw_flowctrl_ponEgressDropEnable_get */

/* Function Name:
 *      rtl9603cvd_raw_flowctrl_ponGlobalHighThreshold_set
 * Description:
 *      Set PON MAC global high on/off threshold when flow contrl off.
 * Input:
 *      onThreshold         - global fc-off high on threshold
 *      offThreshold         - global fc-off high off threshold
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_ponGlobalHighThreshold_set(uint32 onThreshold, uint32 offThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < onThreshold), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < offThreshold), RT_ERR_INPUT);

    if ((ret = reg_field_write(RTL9603CVD_FC_PON_GLB_HI_THr, RTL9603CVD_ON_THf, &onThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_write(RTL9603CVD_FC_PON_GLB_HI_THr, RTL9603CVD_OFF_THf, &offThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_ponGlobalHighThreshold_set */

/* Function Name:
 *      rtl9603cvd_raw_flowctrl_ponGlobalHighThreshold_get
 * Description:
 *      Get PON MAC global high on/off threshold when flow contrl off.
 * Input:
 *      None
 * Output:
 *      pOnThreshold         - global fc-off high on threshold
 *      pOffThreshold         - global fc-off high off threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_ponGlobalHighThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pOnThreshold == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pOffThreshold == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9603CVD_FC_PON_GLB_HI_THr, RTL9603CVD_ON_THf, pOnThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_read(RTL9603CVD_FC_PON_GLB_HI_THr, RTL9603CVD_OFF_THf, pOffThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_ponGlobalHighThreshold_get */


/* Function Name:
 *      rtl9603cvd_raw_flowctrl_ponGlobalLowThreshold_set
 * Description:
 *      Set PON MAC global Low on/off threshold when flow contrl off.
 * Input:
 *      onThreshold         - global fc-off high on threshold
 *      offThreshold         - global fc-off high off threshold
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_ponGlobalLowThreshold_set(uint32 onThreshold, uint32 offThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < onThreshold), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < offThreshold), RT_ERR_INPUT);

    if ((ret = reg_field_write(RTL9603CVD_FC_PON_GLB_LO_THr, RTL9603CVD_ON_THf, &onThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_write(RTL9603CVD_FC_PON_GLB_LO_THr, RTL9603CVD_OFF_THf, &offThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_ponGlobalLowThreshold_set */

/* Function Name:
 *      rtl9603cvd_raw_flowctrl_ponGlobalLowThreshold_get
 * Description:
 *      Get PON MAC global Low on/off threshold when flow contrl off.
 * Input:
 *      None
 * Output:
 *      pOnThreshold         - global fc-off high on threshold
 *      pOffThreshold         - global fc-off high off threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_ponGlobalLowThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pOnThreshold == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pOffThreshold == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9603CVD_FC_PON_GLB_LO_THr, RTL9603CVD_ON_THf, pOnThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_read(RTL9603CVD_FC_PON_GLB_LO_THr, RTL9603CVD_OFF_THf, pOffThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_ponGlobalHighThreshold_get */

/* Function Name:
 *      rtl9603cvd_raw_flowctrl_ponPortHighThreshold_set
 * Description:
 *      Set PON MAC port high on/off threshold when flow contrl off.
 * Input:
 *      onThreshold         - global fc-off high on threshold
 *      offThreshold         - global fc-off high off threshold
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_ponPortHighThreshold_set(uint32 onThreshold, uint32 offThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < onThreshold), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < offThreshold), RT_ERR_INPUT);

    if ((ret = reg_field_write(RTL9603CVD_FC_PON_P_HI_THr, RTL9603CVD_ON_THf, &onThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_write(RTL9603CVD_FC_PON_P_HI_THr, RTL9603CVD_OFF_THf, &offThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_ponPortHighThreshold_set */

/* Function Name:
 *      rtl9603cvd_raw_flowctrl_ponPortHighThreshold_get
 * Description:
 *      Get PON MAC global high on/off threshold when flow contrl off.
 * Input:
 *      None
 * Output:
 *      pOnThreshold         - global fc-off high on threshold
 *      pOffThreshold         - global fc-off high off threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_ponPortHighThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pOnThreshold == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pOffThreshold == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9603CVD_FC_PON_P_HI_THr, RTL9603CVD_ON_THf, pOnThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_read(RTL9603CVD_FC_PON_P_HI_THr, RTL9603CVD_OFF_THf, pOffThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_ponPortHighThreshold_get */


/* Function Name:
 *      rtl9603cvd_raw_flowctrl_ponPortLowThreshold_set
 * Description:
 *      Set PON MAC port Low on/off threshold when flow contrl off.
 * Input:
 *      onThreshold         - global fc-off high on threshold
 *      offThreshold         - global fc-off high off threshold
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_ponPortLowThreshold_set(uint32 onThreshold, uint32 offThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < onThreshold), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < offThreshold), RT_ERR_INPUT);

    if ((ret = reg_field_write(RTL9603CVD_FC_PON_P_LO_THr, RTL9603CVD_ON_THf, &onThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_write(RTL9603CVD_FC_PON_P_LO_THr, RTL9603CVD_OFF_THf, &offThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_ponPortLowThreshold_set */

/* Function Name:
 *      rtl9603cvd_raw_flowctrl_ponPortLowThreshold_get
 * Description:
 *      Get PON MAC global Low on/off threshold when flow contrl off.
 * Input:
 *      None
 * Output:
 *      pOnThreshold         - global fc-off high on threshold
 *      pOffThreshold         - global fc-off high off threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_ponPortLowThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pOnThreshold == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pOffThreshold == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9603CVD_FC_PON_P_LO_THr, RTL9603CVD_ON_THf, pOnThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_read(RTL9603CVD_FC_PON_P_LO_THr, RTL9603CVD_OFF_THf, pOffThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_ponPortLowThreshold_get */

/* Function Name:
 *      rtl9603cvd_raw_flowctrl_ponGlobalDropHighThreshold_set
 * Description:
 *      Set PON MAC global drop high on/off threshold when downstream pbo drop.
 * Input:
 *      onThreshold         - global drop high on threshold
 *      offThreshold         - global drop high off threshold
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_ponGlobalDropHighThreshold_set(uint32 onThreshold, uint32 offThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < onThreshold), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < offThreshold), RT_ERR_INPUT);

    if ((ret = reg_field_write(RTL9603CVD_FC_PON_GLB_DROP_HI_THr, RTL9603CVD_ON_THf, &onThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_write(RTL9603CVD_FC_PON_GLB_DROP_HI_THr, RTL9603CVD_OFF_THf, &offThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_ponGlobalDropHighThreshold_set */

/* Function Name:
 *      rtl9603cvd_raw_flowctrl_ponGlobalDropHighThreshold_get
 * Description:
 *      Get PON MAC global drop high on/off threshold when downstream pbo drop.
 * Input:
 *      None
 * Output:
 *      pOnThreshold         - global drop high on threshold
 *      pOffThreshold         - global drop high off threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_ponGlobalDropHighThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pOnThreshold == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pOffThreshold == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9603CVD_FC_PON_GLB_DROP_HI_THr, RTL9603CVD_ON_THf, pOnThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_read(RTL9603CVD_FC_PON_GLB_DROP_HI_THr, RTL9603CVD_OFF_THf, pOffThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_ponGlobalDropHighThreshold_get */


/* Function Name:
 *      rtl9603cvd_raw_flowctrl_ponGlobalDropLowThreshold_set
 * Description:
 *      Set PON MAC global drop Low on/off threshold when downstream pbo drop.
 * Input:
 *      onThreshold         - global drop low on threshold
 *      offThreshold         - global drop low off threshold
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_ponGlobalDropLowThreshold_set(uint32 onThreshold, uint32 offThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < onThreshold), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < offThreshold), RT_ERR_INPUT);

    if ((ret = reg_field_write(RTL9603CVD_FC_PON_GLB_DROP_LO_THr, RTL9603CVD_ON_THf, &onThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_write(RTL9603CVD_FC_PON_GLB_DROP_LO_THr, RTL9603CVD_OFF_THf, &offThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_ponGlobalDropLowThreshold_set */

/* Function Name:
 *      rtl9603cvd_raw_flowctrl_ponGlobalDropLowThreshold_get
 * Description:
 *      Get PON MAC global drop Low on/off threshold when downstream pbo drop.
 * Input:
 *      None
 * Output:
 *      pOnThreshold         - global drop low on threshold
 *      pOffThreshold        - global drop low off threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_ponGlobalDropLowThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pOnThreshold == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pOffThreshold == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9603CVD_FC_PON_GLB_DROP_LO_THr, RTL9603CVD_ON_THf, pOnThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_read(RTL9603CVD_FC_PON_GLB_DROP_LO_THr, RTL9603CVD_OFF_THf, pOffThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_ponGlobalDropLowThreshold_get */

/* Function Name:
 *      rtl9603cvd_raw_flowctrl_ponPortDropHighThreshold_set
 * Description:
 *      Set PON MAC port drop high on/off threshold when downstream pbo drop.
 * Input:
 *      onThreshold         - port drop high on threshold
 *      offThreshold        - port drop high off threshold
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_ponPortDropHighThreshold_set(uint32 onThreshold, uint32 offThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < onThreshold), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < offThreshold), RT_ERR_INPUT);

    if ((ret = reg_field_write(RTL9603CVD_FC_PON_P_DROP_HI_THr, RTL9603CVD_ON_THf, &onThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_write(RTL9603CVD_FC_PON_P_DROP_HI_THr, RTL9603CVD_OFF_THf, &offThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_ponPortDropHighThreshold_set */

/* Function Name:
 *      rtl9603cvd_raw_flowctrl_ponPortDropHighThreshold_get
 * Description:
 *      Get PON MAC global drop high on/off threshold when downstream pbo drop.
 * Input:
 *      None
 * Output:
 *      pOnThreshold         - global drop high on threshold
 *      pOffThreshold        - global drop high off threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_ponPortDropHighThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pOnThreshold == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pOffThreshold == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9603CVD_FC_PON_P_DROP_HI_THr, RTL9603CVD_ON_THf, pOnThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_read(RTL9603CVD_FC_PON_P_DROP_HI_THr, RTL9603CVD_OFF_THf, pOffThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_ponPortDropHighThreshold_get */


/* Function Name:
 *      rtl9603cvd_raw_flowctrl_ponPortDropLowThreshold_set
 * Description:
 *      Set PON MAC port drop Low on/off threshold when downstream pbo drop.
 * Input:
 *      onThreshold         - port drop low on threshold
 *      offThreshold        - port drop low off threshold
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_ponPortDropLowThreshold_set(uint32 onThreshold, uint32 offThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < onThreshold), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < offThreshold), RT_ERR_INPUT);

    if ((ret = reg_field_write(RTL9603CVD_FC_PON_P_DROP_LO_THr, RTL9603CVD_ON_THf, &onThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_write(RTL9603CVD_FC_PON_P_DROP_LO_THr, RTL9603CVD_OFF_THf, &offThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_ponPortDropLowThreshold_set */

/* Function Name:
 *      rtl9603cvd_raw_flowctrl_ponPortDropLowThreshold_get
 * Description:
 *      Get PON MAC port drop Low on/off threshold when downstream pbo drop.
 * Input:
 *      None
 * Output:
 *      pOnThreshold         - port drop low on threshold
 *      pOffThreshold        - port drop low off threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_ponPortDropLowThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pOnThreshold == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pOffThreshold == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9603CVD_FC_PON_P_DROP_LO_THr, RTL9603CVD_ON_THf, pOnThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_read(RTL9603CVD_FC_PON_P_DROP_LO_THr, RTL9603CVD_OFF_THf, pOffThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_ponPortDropLowThreshold_get */

/* Function Name:
 *      rtl9603cvd_raw_flowctrl_ponSidState_set
 * Description:
 *      Set flowctrl state per sid
 * Input:
 *      sid                   - stream id (0~32)
 *      state                 - enable or disable per sid
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_ponSidState_set(uint32 sid, rtk_enable_t state)
{
    int32   ret;
    uint32 reg;

    /*parameter check*/
    RT_PARAM_CHK((HAL_CLASSIFY_SID_NUM() < sid), RT_ERR_INPUT);
    RT_PARAM_CHK((state >= RTK_ENABLE_END), RT_ERR_INPUT);

    reg = (ENABLED == state) ? 1 : 0;

    if ((ret = reg_array_field_write(RTL9603CVD_FC_PON_Q_EGR_FC_ENr, REG_ARRAY_INDEX_NONE, sid, RTL9603CVD_ENf, &reg)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_ponSidState_set */

/* Function Name:
 *      rtl9603cvd_raw_flowctrl_ponSidState_get
 * Description:
 *      Get flowctrl state per sid
 * Input:
 *      sid                    - stream id (0~32)
 * Output:
 *      pState                - per stream id flowctrl status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_ponSidState_get(uint32 sid, rtk_enable_t *pState)
{
    int32   ret;
    uint32 reg;

    /*parameter check*/
    RT_PARAM_CHK((HAL_CLASSIFY_SID_NUM() < sid), RT_ERR_INPUT);
    RT_PARAM_CHK((pState == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(RTL9603CVD_FC_PON_Q_EGR_FC_ENr, REG_ARRAY_INDEX_NONE, sid, RTL9603CVD_ENf, &reg)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL|MOD_DAL), "");
        return ret;
    }

    *pState = (ENABLED == reg) ? 1 : 0;

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_ponSidState_get */

/* Function Name:
 *      rtl9603cvd_raw_flowctrl_debugCtrl_set
 * Description:
 *      Set flow control tuning debug control
 * Input:
 *      port                   - Physical port number (0~6)
 *      portClear         - clear maximum used page count
 *      queueClear         - clear maximum used page count
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_debugCtrl_set(uint32 port, uint32 portClear, rtk_bmp_t queueMaskClear)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= portClear), RT_ERR_INPUT);
    RT_PARAM_CHK((queueMaskClear.bits[0] > RTL9603CVD_QUEUE_MASK), RT_ERR_PORT_MASK);

    if ((ret = reg_field_write(RTL9603CVD_FC_DBG_CTRLr, RTL9603CVD_PORT_NOf, &port)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_write(RTL9603CVD_FC_DBG_CTRLr, RTL9603CVD_CLR_PE_MAX_PAGE_CNTf, &portClear)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_write(RTL9603CVD_FC_DBG_CTRLr, RTL9603CVD_CLR_Q_MAX_PAGE_CNTf, &queueMaskClear.bits[0])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_debugCtrl_set */

/* Function Name:
 *      rtl9603cvd_raw_flowctrl_clrTotalPktCnt_set
 * Description:
 *      Set total packet count clear
 * Input:
 *      clear                   - total packet count clear
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_clrTotalPktCnt_set(rtk_enable_t clear)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((RTK_ENABLE_END <= clear), RT_ERR_INPUT);

    if ((ret = reg_field_write(RTL9603CVD_FC_DBG_CTRLr, RTL9603CVD_CLR_TOTAL_PKTCNTf, &clear)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_clrTotalPktCnt_set */


/* Function Name:
 *      rtl9603cvd_raw_flowctrl_clrMaxUsedPageCnt_set
 * Description:
 *      Set maximum used page count clear
 * Input:
 *      clear         - clear maximum used page count
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_clrMaxUsedPageCnt_set(uint32 clear)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((RTK_ENABLE_END <= clear), RT_ERR_INPUT);

    if ((ret = reg_field_write(RTL9603CVD_CLR_MAX_USED_PAGE_CNTr, RTL9603CVD_CLR_MAX_USED_PAGE_CNTf, &clear)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_clrMaxUsedPageCnt_set */

/* Function Name:
 *      rtl9603cvd_raw_flowctrl_totalPageCnt_get
 * Description:
 *      Get total ingress packet used page count
 * Input:
 *      None
 * Output:
 *      pCount         - page count
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_totalPageCnt_get(uint32 *pCount)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pCount == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9603CVD_FC_TOTAL_PAGE_CNTr, RTL9603CVD_TOTAL_PAGE_CNTf, pCount)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_totalUsedPageCnt_get */

/* Function Name:
 *      rtl9603cvd_raw_flowctrl_totalUsedPageCnt_get
 * Description:
 *      Get total ingress used page count
 * Input:
 *      None
 * Output:
 *      pCurrent         - Current page count
 *      pMaximum         - Maximum page count
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_totalUsedPageCnt_get(uint32 *pCurrent, uint32 *pMaximum)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pCurrent == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pMaximum == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9603CVD_FC_TL_USED_PAGE_CNTr, RTL9603CVD_TL_USED_PAGE_CNTf, pCurrent)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL|MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_field_read(RTL9603CVD_FC_TL_USED_PAGE_CNTr, RTL9603CVD_TL_MAX_USED_PAGE_CNTf, pMaximum)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_totalUsedPageCnt_get */

/* Function Name:
 *      rtl9603cvd_raw_flowctrl_publicUsedPageCnt_get
 * Description:
 *      Get public ingress used page count
 * Input:
 *      None
 * Output:
 *      pCurrent         - Current page count
 *      pMaximum         - Maximum page count
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_publicUsedPageCnt_get(uint32 *pCurrent, uint32 *pMaximum)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pCurrent == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pMaximum == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9603CVD_FC_PUB_USED_PAGE_CNTr, RTL9603CVD_PUB_USED_PAGE_CNTf, pCurrent)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL|MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_field_read(RTL9603CVD_FC_PUB_USED_PAGE_CNTr, RTL9603CVD_PUB_MAX_USED_PAGE_CNTf, pMaximum)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_publicUsedPageCnt_get */

/* Function Name:
 *      rtl9603cvd_raw_flowctrl_publicFcoffUsedPageCnt_get
 * Description:
 *      Get public ingress used page count when flow control off
 * Input:
 *      None
 * Output:
 *      pCurrent         - Current page count
 *      pMaximum         - Maximum page count
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_publicFcoffUsedPageCnt_get(uint32 *pCurrent, uint32 *pMaximum)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pCurrent == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pMaximum == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9603CVD_FC_PUB_FCOFF_USED_PAGE_CNTr, RTL9603CVD_PUB_FCOFF_USED_PAGE_CNTf, pCurrent)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL|MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_field_read(RTL9603CVD_FC_PUB_FCOFF_USED_PAGE_CNTr, RTL9603CVD_PUB_FCOFF_MAX_USED_PAGE_CNTf, pMaximum)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_publicFcoffUsedPageCnt_get */

/* Function Name:
 *      rtl9603cvd_raw_flowctrl_portUsedPageCnt_get
 * Description:
 *      Get ingress port used page count
 * Input:
 *      port                   - Physical port number (0~6)
 * Output:
 *      pCurrent         - Current page count
 *      pMaximum         - Maximum page count
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_portUsedPageCnt_get(uint32 port, uint32 *pCurrent, uint32 *pMaximum)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((pCurrent == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pMaximum == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(RTL9603CVD_FC_P_USED_PAGE_CNTr, port, REG_ARRAY_INDEX_NONE, RTL9603CVD_P_USED_PAGE_CNTf, pCurrent)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL|MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(RTL9603CVD_FC_P_USED_PAGE_CNTr, port, REG_ARRAY_INDEX_NONE, RTL9603CVD_P_MAX_USED_PAGE_CNTf, pMaximum)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_portUsedPageCnt_get */


/* Function Name:
 *      rtl9603cvd_raw_flowctrl_portEgressUsedPageCnt_get
 * Description:
 *      Get iegress port used page count
 * Input:
 *      port                   - Physical port number (0~6)
 * Output:
 *      pCurrent         - Current page count
 *      pMaximum         - Maximum page count
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_portEgressUsedPageCnt_get(uint32 port, uint32 *pCurrent, uint32 *pMaximum)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((pCurrent == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pMaximum == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(RTL9603CVD_FC_PE_USED_PAGE_CNTr, port, REG_ARRAY_INDEX_NONE, RTL9603CVD_PE_USED_PAGE_CNTf, pCurrent)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL|MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(RTL9603CVD_FC_PE_USED_PAGE_CNTr, port, REG_ARRAY_INDEX_NONE, RTL9603CVD_PE_MAX_USED_PAGE_CNTf, pMaximum)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_portEgressUsedPageCnt_get */

/* Function Name:
 *      rtl9603cvd_raw_flowctrl_queueEgressUsedPageCnt_get
 * Description:
 *      Get iegress queue used page count
 * Input:
 *      port                   - Physical port number (0~6)
 *      queue                 - queue id (0~7)
 * Output:
 *      pCurrent         - Current page count
 *      pMaximum         - Maximum page count
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_queueEgressUsedPageCnt_get(uint32 port, uint32 queue, uint32 *pCurrent, uint32 *pMaximum)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((HAL_QUEUE_ID_MAX()  < queue), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((pCurrent == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pMaximum == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(RTL9603CVD_FC_Q_USED_PAGE_CNTr, port, queue, RTL9603CVD_Q_USED_PAGE_CNTf, pCurrent)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL|MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(RTL9603CVD_FC_Q_USED_PAGE_CNTr, port, queue, RTL9603CVD_Q_MAX_USED_PAGE_CNTf, pMaximum)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_queueEgressUsedPageCnt_get */


/* Function Name:
 *      rtl9603cvd_raw_flowctrl_txPrefet_set
 * Description:
 *      Set total packet count clear
 * Input:
 *      threshold         - prefet threshold
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_txPrefet_set(uint32 threshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((RTL9603CVD_FLOWCTRL_PREFET_THRESHOLD_MAX < threshold), RT_ERR_INPUT);

    if ((ret = reg_field_write(RTL9603CVD_TH_TX_PREFETr, RTL9603CVD_CFG_TH_TX_PREFETf, &threshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_txPrefet_set */

/* Function Name:
 *      rtl9603cvd_raw_flowctrl_txPrefet_get
 * Description:
 *      Set total packet count clear
 * Input:
 *      None
 * Output:
 *      pThreshold         - prefet threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_txPrefet_get(uint32 *pThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((NULL == pThreshold), RT_ERR_INPUT);

    if ((ret = reg_field_read(RTL9603CVD_TH_TX_PREFETr, RTL9603CVD_CFG_TH_TX_PREFETf, pThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_txPrefet_get */

/* Function Name:
 *      rtl9603cvd_raw_flowctrl_ponUsedPageCtrl_set
 * Description:
 *      Set flow control tuning debug control
 * Input:
 *      port                   - Physical port number (0~6)
 *      clear           -        clear maximum used page count
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_ponUsedPageCtrl_set(uint32 sid, uint32 clear)
{
    int32   ret;
    uint32  data;
    uint32  retry;

    /*parameter check*/
    RT_PARAM_CHK((HAL_MAX_NUM_OF_PON_QUEUE() < sid), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= clear), RT_ERR_INPUT);

    /*set sid_no*/
    if ((ret = reg_field_write(RTL9603CVD_PONIP_DBG_CTRL_USr, RTL9603CVD_SID_NOf, &sid)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    /*trigger clear*/
    data = 1;
    if ((ret = reg_field_write(RTL9603CVD_PONIP_DBG_CTRL_USr, RTL9603CVD_CLR_SID_MAX_PAGE_CNTf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    /*wait complete*/
    retry = 0;
    do
    {
        if ((ret = reg_field_read(RTL9603CVD_PONIP_DBG_CTRL_USr, RTL9603CVD_SIDCNT_ACC_BUSYf, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
            return ret;
        }
        retry++;
    }while((1 == data) && (retry < 1000));

    if(retry >= 1000)
        return RT_ERR_BUSYWAIT_TIMEOUT;
    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_ponUsedPageCtrl_set */

/* Function Name:
 *      rtl9603cvd_raw_flowctrl_ponUsedPage_get
 * Description:
 *      Set flow control tuning debug control
 * Input:
 *      sid                   - PON port sid number
 * Output:
 *      pCurrent         - Current page count
 *      pMaximum         - Maximum page count
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_ponUsedPage_get(uint32 sid, uint32 *pCurrent, uint32 *pMaximum)
{
    int32   ret;
    uint32  data;
    uint32  retry;

    /*parameter check*/
    RT_PARAM_CHK((HAL_MAX_NUM_OF_PON_QUEUE() < sid), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((NULL == pCurrent), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pMaximum), RT_ERR_INPUT);

    /*set sid_no*/
    if ((ret = reg_field_write(RTL9603CVD_PONIP_DBG_CTRL_USr, RTL9603CVD_SID_NOf, &sid)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    /*trigger read*/
    data = 1;
    if ((ret = reg_field_write(RTL9603CVD_PONIP_DBG_CTRL_USr, RTL9603CVD_RD_SID_MAX_PAGE_CNTf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    /*wait complete*/
    retry = 0;
    do
    {
        if ((ret = reg_field_read(RTL9603CVD_PONIP_DBG_CTRL_USr, RTL9603CVD_SIDCNT_ACC_BUSYf, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
            return ret;
        }
        retry++;
    }while((1 == data) && (retry < 1000));

    if(retry >= 1000)
        return RT_ERR_BUSYWAIT_TIMEOUT;

    /*read data*/
    if ((ret = reg_field_read(RTL9603CVD_PONIP_SID_USED_PAGE_CNT_USr, RTL9603CVD_USED_PAGE_CNTf, pCurrent)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_read(RTL9603CVD_PONIP_SID_USED_PAGE_CNT_USr, RTL9603CVD_MAX_PAGE_CNTf, pMaximum)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_ponUsedPage_get */

static int32 _patch_pon(void)
{

    int32  ret;
#if 0 //TBD RTL9603CVD_FC_PON_DROP_CTRLr REG Remove
    uint32 data;
    uint32 i;

    /* turn on option for DS PBO issue, drop TH and pause TH use different setting.*/
    data=0;
    if ((ret = reg_field_write(RTL9603CVD_FC_PON_DROP_CTRLr, RTL9603CVD_FC_DROP_SELf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
#endif
#if 0 /* no need to set value, the chip default value already fix it. */
    /* Configure the PON system flow control thresholds 16 less than switchs' */
    /* Switch PON port system hign on 512 - 16 off 500 - 16 */
    if ((ret = rtl9603cvd_raw_flowctrl_ponGlobalHighThreshold_set(0x200 - 16, 0x1F4 - 16)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* Switch PON port system low on 486 - 16 off 471 - 16 */
    if ((ret = rtl9603cvd_raw_flowctrl_ponGlobalLowThreshold_set(0x1E6 - 16, 0x1D7 - 16)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* Switch PON port private high on 96 off 90 */
    if ((ret = rtl9603cvd_raw_flowctrl_ponPortHighThreshold_set(0x60, 0x5A)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* Switch PON port private high on 32 off 12 */
    if ((ret = rtl9603cvd_raw_flowctrl_ponPortLowThreshold_set(0x20, 0xC)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* Switch ingress egress drop all queue 36 */
    for(i = 0;i < HAL_MAX_NUM_OF_QUEUE();i ++)
    {
        if ((ret = rtl9603cvd_raw_flowctrl_queueEegressDropThreshold_set(i, 36)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
            return ret;
        }
    }

    /* Switch ingress egress drop queue gap 8 */
#endif
    /* Switch to MAC X threshold to 150 */
    if ((ret = rtl9603cvd_raw_flowctrl_swpboEgressDropThreshold_set(150)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;
}

static int32 _patch_default(void)
{
#if 0 /* no need to set value, the chip default value already fix it. */
    uint32 i;
    int32  ret;

    /* Switch PON port system hign on 235 off 220 */
    if ((ret = rtl9603cvd_raw_flowctrl_ponGlobalHighThreshold_set(0xEB, 0xDC)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* Switch PON port system low on 230 off 215 */
    if ((ret = rtl9603cvd_raw_flowctrl_ponGlobalLowThreshold_set(0xE6, 0xD7)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* Switch PON port private high on 96 off 90 */
    if ((ret = rtl9603cvd_raw_flowctrl_ponPortHighThreshold_set(0x60, 0x5A)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* Switch PON port private high on 32 off 12 */
    if ((ret = rtl9603cvd_raw_flowctrl_ponPortLowThreshold_set(0x20, 0xC)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* Switch ingress egress drop all queue 36 */
    for(i = 0;i < HAL_MAX_NUM_OF_QUEUE();i ++)
    {
        if ((ret = rtl9603cvd_raw_flowctrl_queueEegressDropThreshold_set(i, 36)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
            return ret;
        }
    }

    /* Switch ingress egress drop queue gap 8 */
#endif
    return RT_ERR_OK;
}

/* Function Name:
 *      rtl9603cvd_raw_flowctrl_patch
 * Description:
 *      patch the flow control parameter by type
 * Input:
 *      patch_type      - 3.5Mb GPON, 3.5Mb Fiber, 2.0Mb. default
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_CHIP_NOT_SUPPORTED
 * Note:
 */
int32 rtl9603cvd_raw_flowctrl_patch(rtk_flowctrl_patch_t patch_type)
{
    int32  ret=RT_ERR_OK;

    switch(patch_type)
    {
        case FLOWCTRL_PATCH_20M:
            break;

        case FLOWCTRL_PATCH_35M_FIBER:
            break;

        case FLOWCTRL_PATCH_35M_GPON:
            /* GPON/EPON using the same config in RTL9603CVD */
            _patch_pon();
            break;

        case FLOWCTRL_PATCH_DEFAULT:
            _patch_default();
            break;

        default:
            ret = RT_ERR_CHIP_NOT_SUPPORTED;
            break;
    }

    return ret;
}

/* Function Name:
 *      rtl9603cvd_raw_flowctrl_portBackPressureEnable_set
 * Description:
 *      Set the back pressure enable status of the specific port
 * Input:
 *      port   - port id
 *      enable - enable status of back pressure
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 */
int32
rtl9603cvd_raw_flowctrl_portBackPressureEnable_set(uint32 port, rtk_enable_t enable)
{
    int32   ret;
    uint32  data;

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    data = (uint32)enable;
    if((ret = reg_array_field_write(RTL9603CVD_P_MISCr, (int32)port, REG_ARRAY_INDEX_NONE, RTL9603CVD_CFG_ENBKPRESSf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_portBackPressureEnable_set */

/* Function Name:
 *      rtl9603cvd_raw_flowctrl_portBackPressureEnable_get
 * Description:
 *      Get the back pressure enable status of the specific port
 * Input:
 *      port    - port id
 * Output:
 *      pEnable - pointer to the enable status of back pressure
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_NULL_POINTER     - invalid input parameter
 */
int32
rtl9603cvd_raw_flowctrl_portBackPressureEnable_get(uint32 port, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  data;

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((!pEnable), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(RTL9603CVD_P_MISCr, (int32)port, REG_ARRAY_INDEX_NONE, RTL9603CVD_CFG_ENBKPRESSf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL|MOD_DAL), "");
        return ret;
    }

    *pEnable = (rtk_enable_t)data ;

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_portBackPressureEnable_get */

/* Function Name:
 *      rtl9603cvd_raw_flowctrl_queueEegressGapThreshold_set
 * Description:
 *      Set egress queue gap page count
 * Input:
 *      threshold         - egress queue gap page count
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_queueEegressGapThreshold_set(uint32 threshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < threshold), RT_ERR_INPUT);

    if ((ret = reg_field_write(RTL9603CVD_HYS_GAP_OUTQr, RTL9603CVD_GAP_OQ_OVERf, &threshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_egressQueueGap_set */

/* Function Name:
 *      rtl9603cvd_raw_flowctrl_queueEegressGapThreshold_get
 * Description:
 *      Get egress queue gap page count
 * Input:
 *      None
 * Output:
 *      pThreshold         - egress queue gap page count
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_queueEegressGapThreshold_get(uint32 *pThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pThreshold == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9603CVD_HYS_GAP_OUTQr, RTL9603CVD_GAP_OQ_OVERf, pThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_queueEegressGapThreshold_get */

/* Function Name:
 *      rtl9603cvd_raw_flowctrl_portEegressGapThreshold_set
 * Description:
 *      Set egress port gap page count
 * Input:
 *      threshold         - egress port gap page count
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_portEegressGapThreshold_set(uint32 threshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < threshold), RT_ERR_INPUT);

    if ((ret = reg_field_write(RTL9603CVD_HYS_GAP_OUTQr, RTL9603CVD_GAP_EP_OVERf, &threshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_portEegressGapThreshold_set */

/* Function Name:
 *      rtl9603cvd_raw_flowctrl_portEegressGapThreshold_get
 * Description:
 *      Get egress port gap page count
 * Input:
 *      None
 * Output:
 *      pThreshold         - egress port gap page count
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_portEegressGapThreshold_get(uint32 *pThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pThreshold == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9603CVD_HYS_GAP_OUTQr, RTL9603CVD_GAP_EP_OVERf, pThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_portEegressGapThreshold_get */

/* Function Name:
 *      rtl9603cvd_raw_flowctrl_rsvPageBeforeFcDrop_set
 * Description:
 *      Set reserve page before triggering flow control drop
 * Input:
 *      threshold         - reserve page count
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_rsvPageBeforeFcDrop_set(uint32 threshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < threshold), RT_ERR_INPUT);

    if ((ret = reg_field_write(RTL9603CVD_CFG_MAC_MISCr, RTL9603CVD_RSVPG_BEFORE_FCDROPf, &threshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_rsvPageBeforeFcDrop_set */

/* Function Name:
 *      rtl9603cvd_raw_flowctrl_rsvPageBeforeFcDrop_get
 * Description:
 *      Get reserve page before triggering flow control drop
 * Input:
 *      None
 * Output:
 *      pThreshold         - reserve page count
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_rsvPageBeforeFcDrop_get(uint32 *pThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pThreshold == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9603CVD_CFG_MAC_MISCr, RTL9603CVD_RSVPG_BEFORE_FCDROPf, pThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_rsvPageBeforeFcDrop_get */

/* Function Name:
 *      rtl9603cvd_raw_flowctrl_changeDuplexEnable_set
 * Description:
 *      Set status for changeing duplex function
 * Input:
 *      enable           - enable or disable change duplex function
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_changeDuplexEnable_set(rtk_enable_t enable)
{
    int32   ret;
    uint32  reg;

    /*parameter check*/
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    reg = (ENABLED == enable) ? 1 : 0;

    if ((ret = reg_field_write(RTL9603CVD_CHANGE_DUPLEX_CTRLr, RTL9603CVD_CFG_CHG_DUP_ENf, &reg)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_changeDuplexEnable_set */

/* Function Name:
 *      rtl9603cvd_raw_flowctrl_changeDuplexEnable_get
 * Description:
 *      Get reserve page before triggering flow control drop
 * Input:
 *      None
 * Output:
 *      pEnable         - status for changeing duplex function
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_changeDuplexEnable_get(rtk_enable_t *pEnable)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pEnable == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9603CVD_CHANGE_DUPLEX_CTRLr, RTL9603CVD_CFG_CHG_DUP_ENf, pEnable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_changeDuplexEnable_get */

/* Function Name:
 *      rtl9603cvd_raw_flowctrl_swpboGlobalHighThreshold_set
 * Description:
 *      Set SWPBO MAC global high on/off threshold.
 * Input:
 *      onThreshold         - global fc-on high on threshold
 *      offThreshold        - global fc-on high off threshold
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_swpboGlobalHighThreshold_set(uint32 onThreshold, uint32 offThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < onThreshold), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < offThreshold), RT_ERR_INPUT);

    if ((ret = reg_field_write(RTL9603CVD_FC_SWPBO_GLB_HI_THr, RTL9603CVD_ON_THf, &onThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_write(RTL9603CVD_FC_SWPBO_GLB_HI_THr, RTL9603CVD_OFF_THf, &offThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_swpboGlobalHighThreshold_set */

/* Function Name:
 *      rtl9603cvd_raw_flowctrl_swpboGlobalHighThreshold_get
 * Description:
 *      Get SWPBO MAC global high on/off threshold.
 * Input:
 *      None
 * Output:
 *      pOnThreshold         - global fc-on high on threshold
 *      pOffThreshold        - global fc-on high off threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_swpboGlobalHighThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pOnThreshold == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pOffThreshold == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9603CVD_FC_SWPBO_GLB_HI_THr, RTL9603CVD_ON_THf, pOnThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_read(RTL9603CVD_FC_SWPBO_GLB_HI_THr, RTL9603CVD_OFF_THf, pOffThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_swpboGlobalHighThreshold_get */


/* Function Name:
 *      rtl9603cvd_raw_flowctrl_swpboGlobalLowThreshold_set
 * Description:
 *      Set SWPBO MAC global Low on/off threshold.
 * Input:
 *      onThreshold         - global fc-on high on threshold
 *      offThreshold        - global fc-on high off threshold
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_swpboGlobalLowThreshold_set(uint32 onThreshold, uint32 offThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < onThreshold), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < offThreshold), RT_ERR_INPUT);

    if ((ret = reg_field_write(RTL9603CVD_FC_SWPBO_GLB_LO_THr, RTL9603CVD_ON_THf, &onThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_write(RTL9603CVD_FC_SWPBO_GLB_LO_THr, RTL9603CVD_OFF_THf, &offThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_swpboGlobalLowThreshold_set */

/* Function Name:
 *      rtl9603cvd_raw_flowctrl_swpboGlobalLowThreshold_get
 * Description:
 *      Get SWPBO MAC global Low on/off threshold.
 * Input:
 *      None
 * Output:
 *      pOnThreshold         - global fc-on high on threshold
 *      pOffThreshold        - global fc-on high off threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_swpboGlobalLowThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pOnThreshold == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pOffThreshold == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9603CVD_FC_SWPBO_GLB_LO_THr, RTL9603CVD_ON_THf, pOnThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_read(RTL9603CVD_FC_SWPBO_GLB_LO_THr, RTL9603CVD_OFF_THf, pOffThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_swpboGlobalHighThreshold_get */

/* Function Name:
 *      rtl9603cvd_raw_flowctrl_swpboPortHighThreshold_set
 * Description:
 *      Set SWPBO MAC port high on/off threshold.
 * Input:
 *      onThreshold         - global fc-on high on threshold
 *      offThreshold        - global fc-on high off threshold
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_swpboPortHighThreshold_set(uint32 onThreshold, uint32 offThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < onThreshold), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < offThreshold), RT_ERR_INPUT);

    if ((ret = reg_field_write(RTL9603CVD_FC_SWPBO_P_HI_THr, RTL9603CVD_ON_THf, &onThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_write(RTL9603CVD_FC_SWPBO_P_HI_THr, RTL9603CVD_OFF_THf, &offThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_swpboPortHighThreshold_set */

/* Function Name:
 *      rtl9603cvd_raw_flowctrl_swpboPortHighThreshold_get
 * Description:
 *      Get SWPBO MAC global high on/off threshold.
 * Input:
 *      None
 * Output:
 *      pOnThreshold         - global fc-on high on threshold
 *      pOffThreshold        - global fc-on high off threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_swpboPortHighThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pOnThreshold == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pOffThreshold == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9603CVD_FC_SWPBO_P_HI_THr, RTL9603CVD_ON_THf, pOnThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_read(RTL9603CVD_FC_SWPBO_P_HI_THr, RTL9603CVD_OFF_THf, pOffThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_swpboPortHighThreshold_get */


/* Function Name:
 *      rtl9603cvd_raw_flowctrl_swpboPortLowThreshold_set
 * Description:
 *      Set SWPBO MAC port Low on/off threshold.
 * Input:
 *      onThreshold         - global fc-on high on threshold
 *      offThreshold        - global fc-on high off threshold
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_swpboPortLowThreshold_set(uint32 onThreshold, uint32 offThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < onThreshold), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < offThreshold), RT_ERR_INPUT);

    if ((ret = reg_field_write(RTL9603CVD_FC_SWPBO_P_LO_THr, RTL9603CVD_ON_THf, &onThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_write(RTL9603CVD_FC_SWPBO_P_LO_THr, RTL9603CVD_OFF_THf, &offThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_swpboPortLowThreshold_set */

/* Function Name:
 *      rtl9603cvd_raw_flowctrl_swpboPortLowThreshold_get
 * Description:
 *      Get SWPBO MAC global Low on/off threshold.
 * Input:
 *      None
 * Output:
 *      pOnThreshold         - global fc-on high on threshold
 *      pOffThreshold        - global fc-on high off threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_swpboPortLowThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pOnThreshold == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pOffThreshold == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9603CVD_FC_SWPBO_P_LO_THr, RTL9603CVD_ON_THf, pOnThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_read(RTL9603CVD_FC_SWPBO_P_LO_THr, RTL9603CVD_OFF_THf, pOffThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_swpboPortLowThreshold_get */

/* Function Name:
 *      rtl9603cvd_raw_flowctrl_swpboEgressUsedPage_get
 * Description:
 *      Get swpbo egress used page count
 * Input:
 *      None
 * Output:
 *      pCurrent         - Current page count
 *      pMaximum         - Maximum page count
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_swpboEgressUsedPage_get(uint32 *pCurrent, uint32 *pMaximum)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pCurrent == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pMaximum == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9603CVD_FC_SWPBO_USED_PAGE_CNTr, RTL9603CVD_PE_USED_PAGE_CNTf, pCurrent)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL|MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_field_read(RTL9603CVD_FC_SWPBO_USED_PAGE_CNTr, RTL9603CVD_PE_MAX_USED_PAGE_CNTf, pMaximum)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_swpboEgressUsedPage_get */

/* Function Name:
 *      rtl9603cvd_raw_flowctrl_swpboEgressQueueUsedPage_get
 * Description:
 *      Get swpbo egress queue used page count
 * Input:
 *      None
 * Output:
 *      pCurrent         - Current page count
 *      pMaximum         - Maximum page count
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_swpboEgressQueueUsedPage_get(uint32 *pCurrent, uint32 *pMaximum)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pCurrent == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pMaximum == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9603CVD_FC_SWPBO_Q_USED_PAGE_CNTr, RTL9603CVD_Q_USED_PAGE_CNTf, pCurrent)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL|MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_field_read(RTL9603CVD_FC_SWPBO_Q_USED_PAGE_CNTr, RTL9603CVD_Q_MAX_USED_PAGE_CNTf, pMaximum)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_swpboEgressQueueUsedPage_get */

/* Function Name:
 *      rtl9603cvd_raw_flowctrl_swpboPortUsedUsedPage_get
 * Description:
 *      Get swpbo ingress port used page count
 * Input:
 *      None
 * Output:
 *      pCurrent         - Current page count
 *      pMaximum         - Maximum page count
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_swpboPortUsedUsedPage_get(uint32 *pCurrent, uint32 *pMaximum)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pCurrent == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pMaximum == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9603CVD_FC_P_USED_PAGE_CNT_Xr, RTL9603CVD_P_USED_PAGE_CNT_Xf, pCurrent)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL|MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_field_read(RTL9603CVD_FC_P_USED_PAGE_CNT_Xr, RTL9603CVD_P_MAX_USED_PAGE_CNT_Xf, pMaximum)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_swpboPortUsedUsedPage_get */


/* Function Name:
 *      rtl9603cvd_raw_flowctrl_swpboEgressDropThreshold_set
 * Description:
 *      Set swpbo flow control egress drop threshold
 * Input:
 *      threshold            - drop threshold
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_swpboEgressDropThreshold_set(uint32 threshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < threshold), RT_ERR_INPUT);

    if ((ret = reg_field_write(RTL9603CVD_FC_SWPBO_P_EGR_DROP_THr, RTL9603CVD_THf, &threshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_swpboEgressDropThreshold_set */

/* Function Name:
 *      rtl9603cvd_raw_flowctrl_swpboEgressDropThreshold_get
 * Description:
 *      Get swpbo flow control egress drop threshold
 * Input:
 * Output:
 *      pThreshold         - drop threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9603cvd_raw_flowctrl_swpboEgressDropThreshold_get(uint32 *pThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pThreshold == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9603CVD_FC_SWPBO_P_EGR_DROP_THr, RTL9603CVD_THf, pThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL|MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;
} /* end of rtl9603cvd_raw_flowctrl_swpboEgressDropThreshold_get */

