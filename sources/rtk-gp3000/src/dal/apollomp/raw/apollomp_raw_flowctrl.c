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

#include <dal/apollomp/raw/apollomp_raw_flowctrl.h>
#include <dal/apollomp/dal_apollomp_ponmac.h>
#include <dal/apollomp/dal_apollomp_switch.h>
#include <osal/time.h>


/* Function Name:
 *      apollomp_raw_flowctrl_type_set
 * Description:
 *      Set system flow control type
 * Input:
 *      type         - flow control type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 apollomp_raw_flowctrl_type_set(apollomp_raw_flowctrl_type_t type)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((APOLLOMP_FLOWCTRL_TYPE_END <= type), RT_ERR_INPUT);

    if ((ret = reg_field_write(APOLLOMP_FC_CTRLr, APOLLOMP_FC_TYPEf, &type)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_type_set */

/* Function Name:
 *      apollomp_raw_flowctrl_type_get
 * Description:
 *      Get system flow control type
 * Input:
 *      None
 * Output:
 *      pType         - flow control type
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 apollomp_raw_flowctrl_type_get(apollomp_raw_flowctrl_type_t* pType)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pType == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_FC_CTRLr, APOLLOMP_FC_TYPEf, pType)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_type_get */

/* Function Name:
 *      apollomp_raw_flowctrl_jumboMode_set
 * Description:
 *      Set flow control jumbo mode
 * Input:
 *      enable         - enable jumbo mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 apollomp_raw_flowctrl_jumboMode_set(rtk_enable_t enable)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    if ((ret = reg_field_write(APOLLOMP_FC_CTRLr, APOLLOMP_FC_JUMBO_MODEf, &enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_jumboMode_set */

/* Function Name:
 *      apollomp_raw_flowctrl_jumboMode_get
 * Description:
 *      Get flow control jumbo mode
 * Input:
 *      None
 * Output:
 *      pEnable         - enable jumbo mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 apollomp_raw_flowctrl_jumboMode_get(rtk_enable_t *pEnable)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pEnable == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_FC_CTRLr, APOLLOMP_FC_JUMBO_MODEf, pEnable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_jumboMode_get */

/* Function Name:
 *      apollomp_raw_flowctrl_jumboSize_set
 * Description:
 *      Set system flow control jumbo frame size
 * Input:
 *      size         - jumbo frame size
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 apollomp_raw_flowctrl_jumboSize_set(apollomp_raw_flowctrl_jumbo_size_t size)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((APOLLOMP_FLOWCTRL_JUMBO_END <= size), RT_ERR_INPUT);

    if ((ret = reg_field_write(APOLLOMP_FC_CTRLr, APOLLOMP_FC_JUMBO_SIZEf, &size)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_jumboSize_set */

/* Function Name:
 *      apollomp_raw_flowctrl_jumboSize_get
 * Description:
 *      Get system flow control jumbo frame size
 * Input:
 *      None
 * Output:
 *      pSize         - jumbo frame size
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 apollomp_raw_flowctrl_jumboSize_get(apollomp_raw_flowctrl_jumbo_size_t* pSize)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pSize == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_FC_CTRLr, APOLLOMP_FC_JUMBO_SIZEf, pSize)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_jumboSize_get */

/* Function Name:
 *      apollomp_raw_flowctrl_dropAllThreshold_set
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
int32 apollomp_raw_flowctrl_dropAllThreshold_set(uint32 threshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < threshold), RT_ERR_INPUT);

    if ((ret = reg_field_write(APOLLOMP_FC_DROP_ALL_THr, APOLLOMP_THf, &threshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_dropAllThreshold_set */





/* Function Name:
 *      apollomp_raw_flowctrl_dropAllThreshold_get
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
int32 apollomp_raw_flowctrl_dropAllThreshold_get(uint32 *pThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pThreshold == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_FC_DROP_ALL_THr, APOLLOMP_THf, pThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_dropAllThreshold_get */

/* Function Name:
 *      apollomp_raw_flowctrl_pauseAllThreshold_set
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
int32 apollomp_raw_flowctrl_pauseAllThreshold_set(uint32 threshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < threshold), RT_ERR_INPUT);

    if ((ret = reg_field_write(APOLLOMP_FC_PAUSE_ALL_THr, APOLLOMP_THf, &threshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_pauseAllThreshold_set */

/* Function Name:
 *      apollomp_raw_flowctrl_pauseAllThreshold_get
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
int32 apollomp_raw_flowctrl_pauseAllThreshold_get(uint32 *pThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pThreshold == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_FC_PAUSE_ALL_THr, APOLLOMP_THf, pThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_pauseAllThreshold_get */

/* Function Name:
 *      apollomp_raw_flowctrl_globalFcoffHighThreshold_set
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
int32 apollomp_raw_flowctrl_globalFcoffHighThreshold_set(uint32 onThreshold, uint32 offThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < onThreshold), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < offThreshold), RT_ERR_INPUT);

    if ((ret = reg_field_write(APOLLOMP_FC_GLB_FCOFF_HI_THr, APOLLOMP_ON_THf, &onThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_write(APOLLOMP_FC_GLB_FCOFF_HI_THr, APOLLOMP_OFF_THf, &offThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_globalFcoffHighThreshold_set */

/* Function Name:
 *      apollomp_raw_flowctrl_globalFcoffHighThreshold_get
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
int32 apollomp_raw_flowctrl_globalFcoffHighThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pOnThreshold == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pOffThreshold == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_FC_GLB_FCOFF_HI_THr, APOLLOMP_ON_THf, pOnThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_read(APOLLOMP_FC_GLB_FCOFF_HI_THr, APOLLOMP_OFF_THf, pOffThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_globalFcoffHighThreshold_get */

/* Function Name:
 *      apollomp_raw_flowctrl_globalFcoffLowThreshold_set
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
int32 apollomp_raw_flowctrl_globalFcoffLowThreshold_set(uint32 onThreshold, uint32 offThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < onThreshold), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < offThreshold), RT_ERR_INPUT);

    if ((ret = reg_field_write(APOLLOMP_FC_GLB_FCOFF_LO_THr, APOLLOMP_ON_THf, &onThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_write(APOLLOMP_FC_GLB_FCOFF_LO_THr, APOLLOMP_OFF_THf, &offThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_globalFcoffLowThreshold_set */

/* Function Name:
 *      apollomp_raw_flowctrl_globalFcoffLowThreshold_get
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
int32 apollomp_raw_flowctrl_globalFcoffLowThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pOnThreshold == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pOffThreshold == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_FC_GLB_FCOFF_LO_THr, APOLLOMP_ON_THf, pOnThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_read(APOLLOMP_FC_GLB_FCOFF_LO_THr, APOLLOMP_OFF_THf, pOffThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_globalFcoffLowThreshold_get */





/* Function Name:
 *      apollomp_raw_flowctrl_globalFconHighThreshold_set
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
int32 apollomp_raw_flowctrl_globalFconHighThreshold_set(uint32 onThreshold, uint32 offThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < onThreshold), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < offThreshold), RT_ERR_INPUT);

    if ((ret = reg_field_write(APOLLOMP_FC_GLB_HI_THr, APOLLOMP_ON_THf, &onThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_write(APOLLOMP_FC_GLB_HI_THr, APOLLOMP_OFF_THf, &offThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_globalFconHighThreshold_set */

/* Function Name:
 *      apollomp_raw_flowctrl_globalFconHighThreshold_get
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
int32 apollomp_raw_flowctrl_globalFconHighThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pOnThreshold == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pOffThreshold == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_FC_GLB_HI_THr, APOLLOMP_ON_THf, pOnThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_read(APOLLOMP_FC_GLB_HI_THr, APOLLOMP_OFF_THf, pOffThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_globalFconHighThreshold_get */


/* Function Name:
 *      apollomp_raw_flowctrl_globalFconLowThreshold_set
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
int32 apollomp_raw_flowctrl_globalFconLowThreshold_set(uint32 onThreshold, uint32 offThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < onThreshold), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < offThreshold), RT_ERR_INPUT);

    if ((ret = reg_field_write(APOLLOMP_FC_GLB_LO_THr, APOLLOMP_ON_THf, &onThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_write(APOLLOMP_FC_GLB_LO_THr, APOLLOMP_OFF_THf, &offThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_globalFconLowThreshold_set */

/* Function Name:
 *      apollomp_raw_flowctrl_globalFconLowThreshold_get
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
int32 apollomp_raw_flowctrl_globalFconLowThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pOnThreshold == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pOffThreshold == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_FC_GLB_LO_THr, APOLLOMP_ON_THf, pOnThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_read(APOLLOMP_FC_GLB_LO_THr, APOLLOMP_OFF_THf, pOffThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_globalFconLowThreshold_get */

/* Function Name:
 *      apollomp_raw_flowctrl_portFconHighThreshold_set
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
int32 apollomp_raw_flowctrl_portFconHighThreshold_set(uint32 onThreshold, uint32 offThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < onThreshold), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < offThreshold), RT_ERR_INPUT);

    if ((ret = reg_field_write(APOLLOMP_FC_P_HI_THr, APOLLOMP_ON_THf, &onThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_write(APOLLOMP_FC_P_HI_THr, APOLLOMP_OFF_THf, &offThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_portFconHighThreshold_set */

/* Function Name:
 *      apollomp_raw_flowctrl_portFconHighThreshold_get
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
int32 apollomp_raw_flowctrl_portFconHighThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pOnThreshold == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pOffThreshold == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_FC_P_HI_THr, APOLLOMP_ON_THf, pOnThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_read(APOLLOMP_FC_P_HI_THr, APOLLOMP_OFF_THf, pOffThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_portFconHighThreshold_get */

/* Function Name:
 *      apollomp_raw_flowctrl_globalFconLowThreshold_set
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
int32 apollomp_raw_flowctrl_portFconLowThreshold_set(uint32 onThreshold, uint32 offThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < onThreshold), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < offThreshold), RT_ERR_INPUT);

    if ((ret = reg_field_write(APOLLOMP_FC_P_LO_THr, APOLLOMP_ON_THf, &onThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_write(APOLLOMP_FC_P_LO_THr, APOLLOMP_OFF_THf, &offThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_portFconLowThreshold_set */

/* Function Name:
 *      apollomp_raw_flowctrl_portFconLowThreshold_get
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
int32 apollomp_raw_flowctrl_portFconLowThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pOnThreshold == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pOffThreshold == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_FC_P_LO_THr, APOLLOMP_ON_THf, pOnThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_read(APOLLOMP_FC_P_LO_THr, APOLLOMP_OFF_THf, pOffThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_portFconLowThreshold_get */



/* Function Name:
 *      apollomp_raw_flowctrl_portFcoffHighThreshold_set
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
int32 apollomp_raw_flowctrl_portFcoffHighThreshold_set(uint32 onThreshold, uint32 offThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < onThreshold), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < offThreshold), RT_ERR_INPUT);

    if ((ret = reg_field_write(APOLLOMP_FC_P_FCOFF_HI_THr, APOLLOMP_ON_THf, &onThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_write(APOLLOMP_FC_P_FCOFF_HI_THr, APOLLOMP_OFF_THf, &offThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_portFcoffHighThreshold_set */

/* Function Name:
 *      apollomp_raw_flowctrl_portFcoffHighThreshold_get
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
int32 apollomp_raw_flowctrl_portFcoffHighThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pOnThreshold == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pOffThreshold == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_FC_P_FCOFF_HI_THr, APOLLOMP_ON_THf, pOnThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_read(APOLLOMP_FC_P_FCOFF_HI_THr, APOLLOMP_OFF_THf, pOffThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_portFcoffHighThreshold_get */


/* Function Name:
 *      apollomp_raw_flowctrl_portFcoffLowThreshold_set
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
int32 apollomp_raw_flowctrl_portFcoffLowThreshold_set(uint32 onThreshold, uint32 offThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < onThreshold), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < offThreshold), RT_ERR_INPUT);

    if ((ret = reg_field_write(APOLLOMP_FC_P_FCOFF_LO_THr, APOLLOMP_ON_THf, &onThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_write(APOLLOMP_FC_P_FCOFF_LO_THr, APOLLOMP_OFF_THf, &offThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_portFcoffLowThreshold_set */

/* Function Name:
 *      apollomp_raw_flowctrl_portFcoffLowThreshold_get
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
int32 apollomp_raw_flowctrl_portFcoffLowThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pOnThreshold == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pOffThreshold == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_FC_P_FCOFF_LO_THr, APOLLOMP_ON_THf, pOnThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_read(APOLLOMP_FC_P_FCOFF_LO_THr, APOLLOMP_OFF_THf, pOffThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_portFcoffLowThreshold_get */

/* Function Name:
 *      apollomp_raw_flowctrl_jumboGlobalHighThreshold_set
 * Description:
 *      Set jumbo mode per port high on/off threshold
 * Input:
 *      onThreshold         - per port jumbo mode high on threshold
 *      offThreshold         - per port jumbo mode high off threshold
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 apollomp_raw_flowctrl_jumboGlobalHighThreshold_set(uint32 onThreshold, uint32 offThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < onThreshold), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < offThreshold), RT_ERR_INPUT);

    if ((ret = reg_field_write(APOLLOMP_FC_JUMBO_GLB_HI_THr, APOLLOMP_ON_THf, &onThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_write(APOLLOMP_FC_JUMBO_GLB_HI_THr, APOLLOMP_OFF_THf, &offThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_jumboGlobalHighThreshold_set */

/* Function Name:
 *      apollomp_raw_flowctrl_jumboGlobalHighThreshold_get
 * Description:
 *      Get per jumbo mode port high on/off.
 * Input:
 *      None
 * Output:
 *      pOnThreshold         - per port jumbo mode high on threshold
 *      pOffThreshold         - per port jumbo mode high off threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 apollomp_raw_flowctrl_jumboGlobalHighThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pOnThreshold == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pOffThreshold == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_FC_JUMBO_GLB_HI_THr, APOLLOMP_ON_THf, pOnThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_read(APOLLOMP_FC_JUMBO_GLB_HI_THr, APOLLOMP_OFF_THf, pOffThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_jumboGlobalHighThreshold_get */


/* Function Name:
 *      apollomp_raw_flowctrl_jumboGlobalLowThreshold_set
 * Description:
 *      Set per port jumbo mode low on/off threshold.
 * Input:
 *      onThreshold         - per port jumbo mode low on threshold
 *      offThreshold         - per port jumbo mode low off threshold
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 apollomp_raw_flowctrl_jumboGlobalLowThreshold_set(uint32 onThreshold, uint32 offThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < onThreshold), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < offThreshold), RT_ERR_INPUT);

    if ((ret = reg_field_write(APOLLOMP_FC_JUMBO_GLB_LO_THr, APOLLOMP_ON_THf, &onThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_write(APOLLOMP_FC_JUMBO_GLB_LO_THr, APOLLOMP_OFF_THf, &offThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_jumboGlobalLowThreshold_set */

/* Function Name:
 *      apollomp_raw_flowctrl_jumboGlobalLowThreshold_get
 * Description:
 *      Get per port jumbo mode low on/off threshold.
 * Input:
 *      None
 * Output:
 *      pOnThreshold         - per port jumbo mode high on threshold
 *      pOffThreshold         - per port jumbo mode high off threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 apollomp_raw_flowctrl_jumboGlobalLowThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pOnThreshold == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pOffThreshold == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_FC_JUMBO_GLB_LO_THr, APOLLOMP_ON_THf, pOnThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_read(APOLLOMP_FC_JUMBO_GLB_LO_THr, APOLLOMP_OFF_THf, pOffThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_jumboGlobalLowThreshold_get */


/* Function Name:
 *      apollomp_raw_flowctrl_jumboPortHighThreshold_set
 * Description:
 *      Set jumbo mode per port high on/off threshold
 * Input:
 *      onThreshold         - per port jumbo mode high on threshold
 *      offThreshold         - per port jumbo mode high off threshold
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 apollomp_raw_flowctrl_jumboPortHighThreshold_set(uint32 onThreshold, uint32 offThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < onThreshold), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < offThreshold), RT_ERR_INPUT);

    if ((ret = reg_field_write(APOLLOMP_FC_JUMBO_P_HI_THr, APOLLOMP_ON_THf, &onThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_write(APOLLOMP_FC_JUMBO_P_HI_THr, APOLLOMP_OFF_THf, &offThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_jumboPortHighThreshold_set */

/* Function Name:
 *      apollomp_raw_flowctrl_jumboPortHighThreshold_get
 * Description:
 *      Get per jumbo mode port high on/off.
 * Input:
 *      None
 * Output:
 *      pOnThreshold         - per port jumbo mode high on threshold
 *      pOffThreshold         - per port jumbo mode high off threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 apollomp_raw_flowctrl_jumboPortHighThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pOnThreshold == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pOffThreshold == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_FC_JUMBO_P_HI_THr, APOLLOMP_ON_THf, pOnThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_read(APOLLOMP_FC_JUMBO_P_HI_THr, APOLLOMP_OFF_THf, pOffThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_jumboPortHighThreshold_get */


/* Function Name:
 *      apollomp_raw_flowctrl_jumboPortLowThreshold_set
 * Description:
 *      Set per port jumbo mode low on/off threshold.
 * Input:
 *      onThreshold         - per port jumbo mode low on threshold
 *      offThreshold         - per port jumbo mode low off threshold
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 apollomp_raw_flowctrl_jumboPortLowThreshold_set(uint32 onThreshold, uint32 offThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < onThreshold), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < offThreshold), RT_ERR_INPUT);

    if ((ret = reg_field_write(APOLLOMP_FC_JUMBO_P_LO_THr, APOLLOMP_ON_THf, &onThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_write(APOLLOMP_FC_JUMBO_P_LO_THr, APOLLOMP_OFF_THf, &offThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_jumboPortLowThreshold_set */

/* Function Name:
 *      apollomp_raw_flowctrl_jumboPortLowThreshold_get
 * Description:
 *      Get per port jumbo mode low on/off threshold.
 * Input:
 *      None
 * Output:
 *      pOnThreshold         - per port jumbo mode high on threshold
 *      pOffThreshold         - per port jumbo mode high off threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 apollomp_raw_flowctrl_jumboPortLowThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pOnThreshold == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pOffThreshold == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_FC_JUMBO_P_LO_THr, APOLLOMP_ON_THf, pOnThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_read(APOLLOMP_FC_JUMBO_P_LO_THr, APOLLOMP_OFF_THf, pOffThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_jumboPortLowThreshold_get */

/* Function Name:
 *      apollomp_raw_flowctrl_queueEegressDropThreshold_set
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
int32 apollomp_raw_flowctrl_queueEegressDropThreshold_set(uint32 queue, uint32 threshold)
{
    int32   ret;

    RT_PARAM_CHK((HAL_QUEUE_ID_MAX()  < queue), RT_ERR_QUEUE_ID);

    /*parameter check*/
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < threshold), RT_ERR_INPUT);

    if ((ret = reg_array_field_write(APOLLOMP_FC_Q_EGR_DROP_THr, REG_ARRAY_INDEX_NONE, queue, APOLLOMP_THf, &threshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_queueEegressDropThreshold_set */

/* Function Name:
 *      apollomp_raw_flowctrl_queueEegressDropThreshold_get
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
int32 apollomp_raw_flowctrl_queueEegressDropThreshold_get(uint32 queue, uint32 *pThreshold)
{
    int32   ret;

    RT_PARAM_CHK((HAL_QUEUE_ID_MAX()  < queue), RT_ERR_QUEUE_ID);

    /*parameter check*/
    RT_PARAM_CHK((pThreshold == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(APOLLOMP_FC_Q_EGR_DROP_THr, REG_ARRAY_INDEX_NONE, queue, APOLLOMP_THf, pThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_queueEegressDropThreshold_get */


/* Function Name:
 *      apollomp_raw_flowctrl_portEegressDropThreshold_set
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
int32 apollomp_raw_flowctrl_portEegressDropThreshold_set(uint32 port, uint32 threshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < threshold), RT_ERR_INPUT);

    if ((ret = reg_array_field_write(APOLLOMP_FC_P_EGR_DROP_THr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_THf, &threshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_portEegressDropThreshold_set */

/* Function Name:
 *      apollomp_raw_flowctrl_portEegressDropThreshold_get
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
int32 apollomp_raw_flowctrl_portEegressDropThreshold_get(uint32 port, uint32 *pThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((pThreshold == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(APOLLOMP_FC_P_EGR_DROP_THr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_THf, pThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_portEegressDropThreshold_get */


/* Function Name:
 *      apollomp_raw_flowctrl_queueEegressGapThreshold_set
 * Description:
 *      Set queue flow control egress drop threshold gap
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
int32 apollomp_raw_flowctrl_queueEegressGapThreshold_set(uint32 threshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < threshold), RT_ERR_INPUT);

    if ((ret = reg_field_write(APOLLOMP_FC_Q_EGR_GAP_THr, APOLLOMP_THf, &threshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_queueEegressGapThreshold_set */

/* Function Name:
 *      apollomp_raw_flowctrl_queueEegressGapThreshold_get
 * Description:
 *      Get queue flow control egress drop threshold gap
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
int32 apollomp_raw_flowctrl_queueEegressGapThreshold_get(uint32 *pThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pThreshold == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_FC_Q_EGR_GAP_THr, APOLLOMP_THf, pThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_queueEegressGapThreshold_get */


/* Function Name:
 *      apollomp_raw_flowctrl_portEegressGapThreshold_set
 * Description:
 *      Set port flow control egress drop threshold gap
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
int32 apollomp_raw_flowctrl_portEegressGapThreshold_set(uint32 threshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < threshold), RT_ERR_INPUT);

    if ((ret = reg_field_write(APOLLOMP_FC_P_EGR_GAP_THr, APOLLOMP_THf, &threshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_portEegressGapThreshold_set */

/* Function Name:
 *      apollomp_raw_flowctrl_portEegressGapThreshold_get
 * Description:
 *      Get port flow control egress drop threshold gap
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
int32 apollomp_raw_flowctrl_portEegressGapThreshold_get(uint32 *pThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pThreshold == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_FC_P_EGR_GAP_THr, APOLLOMP_THf, pThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_portEegressGapThreshold_get */

/* Function Name:
 *      apollomp_raw_flowctrl_egressDropEnable_set
 * Description:
 *      Set enable to per queue egress drop
 * Input:
 *      port                   - Physical port number (0~6)
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
int32 apollomp_raw_flowctrl_egressDropEnable_set(uint32 port, uint32 queue, rtk_enable_t enable)
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

    if ((ret = reg_array_field_write(APOLLOMP_FC_P_Q_EGR_DROP_ENr, port, queue, APOLLOMP_THf, &reg)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_egressDropEnable_set */

/* Function Name:
 *      apollomp_raw_flowctrl_egressDropEnable_get
 * Description:
 *      Get  per queue egress drop status
 * Input:
 *      port                   - Physical port number (0~6)
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
int32 apollomp_raw_flowctrl_egressDropEnable_get(uint32 port, uint32 queue, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32 reg;

    /*parameter check*/
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((HAL_QUEUE_ID_MAX()  < queue), RT_ERR_QUEUE_ID);

    RT_PARAM_CHK((pEnable == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(APOLLOMP_FC_P_Q_EGR_DROP_ENr, port, queue, APOLLOMP_THf, &reg)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL|MOD_DAL), "");
        return ret;
    }

    if (ENABLED == reg)
        *pEnable = 0;
    else
        *pEnable = 1;


    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_egressDropEnable_get */


/* Function Name:
 *      apollomp_raw_flowctrl_ponGlobalHighThreshold_set
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
int32 apollomp_raw_flowctrl_ponGlobalHighThreshold_set(uint32 onThreshold, uint32 offThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < onThreshold), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < offThreshold), RT_ERR_INPUT);

    if ((ret = reg_field_write(APOLLOMP_FC_PON_GLB_HI_THr, APOLLOMP_ON_THf, &onThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_write(APOLLOMP_FC_PON_GLB_HI_THr, APOLLOMP_OFF_THf, &offThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_ponGlobalHighThreshold_set */

/* Function Name:
 *      apollomp_raw_flowctrl_ponGlobalHighThreshold_get
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
int32 apollomp_raw_flowctrl_ponGlobalHighThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pOnThreshold == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pOffThreshold == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_FC_PON_GLB_HI_THr, APOLLOMP_ON_THf, pOnThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_read(APOLLOMP_FC_PON_GLB_HI_THr, APOLLOMP_OFF_THf, pOffThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_ponGlobalHighThreshold_get */


/* Function Name:
 *      apollomp_raw_flowctrl_ponGlobalLowThreshold_set
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
int32 apollomp_raw_flowctrl_ponGlobalLowThreshold_set(uint32 onThreshold, uint32 offThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < onThreshold), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < offThreshold), RT_ERR_INPUT);

    if ((ret = reg_field_write(APOLLOMP_FC_PON_GLB_LO_THr, APOLLOMP_ON_THf, &onThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_write(APOLLOMP_FC_PON_GLB_LO_THr, APOLLOMP_OFF_THf, &offThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_ponGlobalLowThreshold_set */

/* Function Name:
 *      apollomp_raw_flowctrl_ponGlobalLowThreshold_get
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
int32 apollomp_raw_flowctrl_ponGlobalLowThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pOnThreshold == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pOffThreshold == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_FC_PON_GLB_LO_THr, APOLLOMP_ON_THf, pOnThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_read(APOLLOMP_FC_PON_GLB_LO_THr, APOLLOMP_OFF_THf, pOffThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_ponGlobalHighThreshold_get */

/* Function Name:
 *      apollomp_raw_flowctrl_ponPortHighThreshold_set
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
int32 apollomp_raw_flowctrl_ponPortHighThreshold_set(uint32 onThreshold, uint32 offThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < onThreshold), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < offThreshold), RT_ERR_INPUT);

    if ((ret = reg_field_write(APOLLOMP_FC_PON_P_HI_THr, APOLLOMP_ON_THf, &onThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_write(APOLLOMP_FC_PON_P_HI_THr, APOLLOMP_OFF_THf, &offThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_ponPortHighThreshold_set */

/* Function Name:
 *      apollomp_raw_flowctrl_ponPortHighThreshold_get
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
int32 apollomp_raw_flowctrl_ponPortHighThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pOnThreshold == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pOffThreshold == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_FC_PON_P_HI_THr, APOLLOMP_ON_THf, pOnThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_read(APOLLOMP_FC_PON_P_HI_THr, APOLLOMP_OFF_THf, pOffThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_ponPortHighThreshold_get */


/* Function Name:
 *      apollomp_raw_flowctrl_ponPortLowThreshold_set
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
int32 apollomp_raw_flowctrl_ponPortLowThreshold_set(uint32 onThreshold, uint32 offThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < onThreshold), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < offThreshold), RT_ERR_INPUT);

    if ((ret = reg_field_write(APOLLOMP_FC_PON_P_LO_THr, APOLLOMP_ON_THf, &onThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_write(APOLLOMP_FC_PON_P_LO_THr, APOLLOMP_OFF_THf, &offThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_ponPortLowThreshold_set */

/* Function Name:
 *      apollomp_raw_flowctrl_ponPortLowThreshold_get
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
int32 apollomp_raw_flowctrl_ponPortLowThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pOnThreshold == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pOffThreshold == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_FC_PON_P_LO_THr, APOLLOMP_ON_THf, pOnThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_read(APOLLOMP_FC_PON_P_LO_THr, APOLLOMP_OFF_THf, pOffThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_ponPortLowThreshold_get */


/* Function Name:
 *      apollomp_raw_flowctrl_ponQueueEegressDropIndex_set
 * Description:
 *      Set queue flow control egress drop threshold index
 * Input:
 *      queue               - queue id
 *      index           - drop all threshold index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 apollomp_raw_flowctrl_ponQueueEegressDropIndex_set(uint32 queue, uint32 index)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((HAL_MAX_NUM_OF_PON_QUEUE() < queue), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((APOLLOMP_PON_QUEUE_INDEX_MAX < index), RT_ERR_INPUT);

    if ((ret = reg_array_field_write(APOLLOMP_FC_PON_Q_EGR_DROP_IDXr, REG_ARRAY_INDEX_NONE, queue, APOLLOMP_IDXf, &index)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_ponQueueEegressDropIndex_set */

/* Function Name:
 *      apollomp_raw_flowctrl_ponQueueEegressDropIndex_get
 * Description:
 *      Get queue flow control egress drop threshold index
 * Input:
 *      queue               - queue id
 * Output:
 *      pIndex         - drop all threshold index
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 apollomp_raw_flowctrl_ponQueueEegressDropIndex_get(uint32 queue, uint32 *pIndex)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((HAL_MAX_NUM_OF_PON_QUEUE() < queue), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((NULL == pIndex), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(APOLLOMP_FC_PON_Q_EGR_DROP_IDXr, REG_ARRAY_INDEX_NONE, queue, APOLLOMP_IDXf, pIndex)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_ponQueueEegressDropIndex_get */


/* Function Name:
 *      apollomp_raw_flowctrl_ponQueueEegressDropThreshold_set
 * Description:
 *      Set PON MAC queue flow control egress drop threshold
 * Input:
 *      index               - index
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
int32 apollomp_raw_flowctrl_ponQueueEegressDropThreshold_set(uint32 index, uint32 threshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((APOLLOMP_PON_QUEUE_INDEX_MAX < index), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < threshold), RT_ERR_INPUT);

    if ((ret = reg_array_field_write(APOLLOMP_FC_PON_Q_EGR_DROP_THr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_THf, &threshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_ponQueueEegressDropThreshold_set */

/* Function Name:
 *      apollomp_raw_flowctrl_ponQueueEegressDropThreshold_get
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
int32 apollomp_raw_flowctrl_ponQueueEegressDropThreshold_get(uint32 index, uint32 *pThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((APOLLOMP_PON_QUEUE_INDEX_MAX < index), RT_ERR_INPUT);
    RT_PARAM_CHK((pThreshold == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(APOLLOMP_FC_PON_Q_EGR_DROP_THr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_THf, pThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_ponQueueEegressDropThreshold_get */

/* Function Name:
 *      apollomp_raw_flowctrl_ponQueueEegressGapThreshold_set
 * Description:
 *      Set PON MAC queue flow control egress drop threshold gap
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
int32 apollomp_raw_flowctrl_ponQueueEegressGapThreshold_set(uint32 threshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < threshold), RT_ERR_INPUT);

    if ((ret = reg_field_write(APOLLOMP_FC_PON_Q_EGR_GAP_THr, APOLLOMP_THf, &threshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_ponQueueEegressGapThreshold_set */

/* Function Name:
 *      apollomp_raw_flowctrl_ponQueueEegressGapThreshold_get
 * Description:
 *      Get PON MAC queue flow control egress drop threshold gap
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
int32 apollomp_raw_flowctrl_ponQueueEegressGapThreshold_get(uint32 *pThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pThreshold == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_FC_PON_Q_EGR_GAP_THr, APOLLOMP_THf, pThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_ponQueueEegressGapThreshold_get */


/* Function Name:
 *      apollomp_raw_flowctrl_debugCtrl_set
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
int32 apollomp_raw_flowctrl_debugCtrl_set(uint32 port, uint32 portClear, rtk_bmp_t queueMaskClear)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= portClear), RT_ERR_INPUT);
    RT_PARAM_CHK((queueMaskClear.bits[0] > APOLLOMP_QUEUE_MASK), RT_ERR_PORT_MASK);

    if ((ret = reg_field_write(APOLLOMP_FC_DBG_CTRLr, APOLLOMP_PORT_NOf, &port)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_write(APOLLOMP_FC_DBG_CTRLr, APOLLOMP_CLR_PE_MAX_PAGE_CNTf, &portClear)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_write(APOLLOMP_FC_DBG_CTRLr, APOLLOMP_CLR_Q_MAX_PAGE_CNTf, &queueMaskClear.bits[0])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_debugCtrl_set */

/* Function Name:
 *      apollomp_raw_flowctrl_clrTotalPktCnt_set
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
int32 apollomp_raw_flowctrl_clrTotalPktCnt_set(rtk_enable_t clear)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((RTK_ENABLE_END <= clear), RT_ERR_INPUT);

    if ((ret = reg_field_write(APOLLOMP_FC_DBG_CTRLr, APOLLOMP_CLR_TOTAL_PKTCNTf, &clear)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_clrTotalPktCnt_set */


/* Function Name:
 *      apollomp_raw_flowctrl_clrMaxUsedPageCnt_set
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
int32 apollomp_raw_flowctrl_clrMaxUsedPageCnt_set(uint32 clear)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((RTK_ENABLE_END <= clear), RT_ERR_INPUT);

    if ((ret = reg_field_write(APOLLOMP_CLR_MAX_USED_PAGE_CNTr, APOLLOMP_CLR_MAX_USED_PAGE_CNTf, &clear)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_clrMaxUsedPageCnt_set */

/* Function Name:
 *      apollomp_raw_flowctrl_totalPageCnt_get
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
int32 apollomp_raw_flowctrl_totalPageCnt_get(uint32 *pCount)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pCount == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_FC_TOTAL_PAGE_CNTr, APOLLOMP_TOTAL_PAGE_CNTf, pCount)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_totalUsedPageCnt_get */

/* Function Name:
 *      apollomp_raw_flowctrl_totalUsedPageCnt_get
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
int32 apollomp_raw_flowctrl_totalUsedPageCnt_get(uint32 *pCurrent, uint32 *pMaximum)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pCurrent == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pMaximum == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_FC_TL_USED_PAGE_CNTr, APOLLOMP_TL_USED_PAGE_CNTf, pCurrent)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL|MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_field_read(APOLLOMP_FC_TL_USED_PAGE_CNTr, APOLLOMP_TL_MAX_USED_PAGE_CNTf, pMaximum)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_totalUsedPageCnt_get */

/* Function Name:
 *      apollomp_raw_flowctrl_publicUsedPageCnt_get
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
int32 apollomp_raw_flowctrl_publicUsedPageCnt_get(uint32 *pCurrent, uint32 *pMaximum)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pCurrent == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pMaximum == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_FC_PUB_USED_PAGE_CNTr, APOLLOMP_PUB_USED_PAGE_CNTf, pCurrent)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL|MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_field_read(APOLLOMP_FC_PUB_USED_PAGE_CNTr, APOLLOMP_PUB_MAX_USED_PAGE_CNTf, pMaximum)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_publicUsedPageCnt_get */

/* Function Name:
 *      apollomp_raw_flowctrl_publicFcoffUsedPageCnt_get
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
int32 apollomp_raw_flowctrl_publicFcoffUsedPageCnt_get(uint32 *pCurrent, uint32 *pMaximum)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pCurrent == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pMaximum == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_FC_PUB_FCOFF_USED_PAGE_CNTr, APOLLOMP_PUB_FCOFF_USED_PAGE_CNTf, pCurrent)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL|MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_field_read(APOLLOMP_FC_PUB_FCOFF_USED_PAGE_CNTr, APOLLOMP_PUB_FCOFF_MAX_USED_PAGE_CNTf, pMaximum)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_publicFcoffUsedPageCnt_get */

/* Function Name:
 *      apollomp_raw_flowctrl_publicJumboUsedPageCnt_get
 * Description:
 *      Get public ingress used page count for jumbo threshold
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
int32 apollomp_raw_flowctrl_publicJumboUsedPageCnt_get(uint32 *pCurrent, uint32 *pMaximum)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pCurrent == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pMaximum == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_FC_PUB_JUMBO_USED_PAGE_CNTr, APOLLOMP_PUB_JUMBO_USED_PAGE_CNTf, pCurrent)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL|MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_field_read(APOLLOMP_FC_PUB_JUMBO_USED_PAGE_CNTr, APOLLOMP_PUB_JUMBO_MAX_USED_PAGE_CNTf, pMaximum)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_publicJumboUsedPageCnt_get */

/* Function Name:
 *      apollomp_raw_flowctrl_portUsedPageCnt_get
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
int32 apollomp_raw_flowctrl_portUsedPageCnt_get(uint32 port, uint32 *pCurrent, uint32 *pMaximum)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((pCurrent == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pMaximum == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(APOLLOMP_FC_P_USED_PAGE_CNTr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_P_USED_PAGE_CNTf, pCurrent)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL|MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(APOLLOMP_FC_P_USED_PAGE_CNTr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_P_MAX_USED_PAGE_CNTf, pMaximum)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_portUsedPageCnt_get */

/* Function Name:
 *      apollomp_raw_flowctrl_portPacketPageCnt_get
 * Description:
 *      Get ingress port used page count
 * Input:
 *      port                   - Physical port number (0~6)
 * Output:
 *      pCount         -  Packet page count
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 apollomp_raw_flowctrl_portPacketPageCnt_get(uint32 port, uint32 *pCount)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((pCount == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(APOLLOMP_FC_P_DBG_PKT_PAGE_CNTr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_PKT_PAGE_CNTf, pCount)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_portPacketPageCnt_get */


/* Function Name:
 *      apollomp_raw_flowctrl_portEgressUsedPageCnt_get
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
int32 apollomp_raw_flowctrl_portEgressUsedPageCnt_get(uint32 port, uint32 *pCurrent, uint32 *pMaximum)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((pCurrent == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pMaximum == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(APOLLOMP_FC_PE_USED_PAGE_CNTr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_PE_USED_PAGE_CNTf, pCurrent)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL|MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(APOLLOMP_FC_PE_USED_PAGE_CNTr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_PE_MAX_USED_PAGE_CNTf, pMaximum)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_portEgressUsedPageCnt_get */

/* Function Name:
 *      apollomp_raw_flowctrl_queueEgressUsedPageCnt_get
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
int32 apollomp_raw_flowctrl_queueEgressUsedPageCnt_get(uint32 port, uint32 queue, uint32 *pCurrent, uint32 *pMaximum)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((HAL_QUEUE_ID_MAX()  < queue), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((pCurrent == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pMaximum == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(APOLLOMP_FC_Q_USED_PAGE_CNTr, port, queue, APOLLOMP_Q_USED_PAGE_CNTf, pCurrent)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL|MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(APOLLOMP_FC_Q_USED_PAGE_CNTr, port, queue, APOLLOMP_Q_MAX_USED_PAGE_CNTf, pMaximum)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_queueEgressUsedPageCnt_get */

/* Function Name:
 *      apollomp_raw_flowctrl_ponUsedPageCtrl_set
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
int32 apollomp_raw_flowctrl_ponUsedPageCtrl_set( uint32 clear)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((RTK_ENABLE_END <= clear), RT_ERR_INPUT);

    if ((ret = reg_field_write(APOLLOMP_FC_PON_Q_USED_PAGE_CTRLr, APOLLOMP_CLR_MAX_PAGE_CNTf, &clear)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_ponUsedPageCtrl_set */

/* Function Name:
 *      apollomp_raw_flowctrl_ponUsedPage_get
 * Description:
 *      Set flow control tuning debug control
 * Input:
 *      queue                   - PON port queue number
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
int32 apollomp_raw_flowctrl_ponUsedPage_get(uint32 queue, uint32 *pCurrent, uint32 *pMaximum)
{
    int32   ret;
    uint32 qidTmp;

    /*parameter check*/
    RT_PARAM_CHK((HAL_MAX_NUM_OF_PON_QUEUE() < queue), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((NULL == pCurrent), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pMaximum), RT_ERR_INPUT);


    if(queue)
        qidTmp = 0;
    else
        qidTmp = 1;

    /*switch to other queue at the first*/
    if ((ret = reg_field_write(APOLLOMP_QUEUE_SEL_INDr, APOLLOMP_QCNT_SEL_INDf, &qidTmp)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_write(APOLLOMP_QUEUE_SEL_INDr, APOLLOMP_QCNT_SEL_INDf, &queue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    /*No use, the above register is for queue control*/
    /*if ((ret = reg_field_write(FC_PON_Q_USED_PAGE_CTRLr, QIDf, &queue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }*/

    if ((ret = reg_field_read(APOLLOMP_FC_PON_Q_USED_PAGE_CNTr, APOLLOMP_Q_USED_PAGE_CNTf, pCurrent)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_read(APOLLOMP_FC_PON_Q_USED_PAGE_CNTr, APOLLOMP_Q_MAX_USED_PAGE_CNTf, pMaximum)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_ponUsedPageCtrl_get */


/* Function Name:
 *      apollomp_raw_flowctrl_txPrefet_set
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
int32 apollomp_raw_flowctrl_txPrefet_set(uint32 threshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((APOLLOMP_FLOWCTRL_PREFET_THRESHOLD_MAX < threshold), RT_ERR_INPUT);

    if ((ret = reg_field_write(APOLLOMP_TH_TX_PREFETr, APOLLOMP_CFG_TH_TX_PREFETf, &threshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_txPrefet_set */

/* Function Name:
 *      apollomp_raw_flowctrl_txPrefet_get
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
int32 apollomp_raw_flowctrl_txPrefet_get(uint32 *pThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((NULL == pThreshold), RT_ERR_INPUT);

    if ((ret = reg_field_read(APOLLOMP_TH_TX_PREFETr, APOLLOMP_CFG_TH_TX_PREFETf, pThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_txPrefet_get */

/* Function Name:
 *      apollomp_raw_flowctrl_lowQueuelThreshold_set
 * Description:
 *      Set low queue over threshold for EEE usage
 * Input:
 *      threshold         - low queue over threshold
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 apollomp_raw_flowctrl_lowQueueThreshold_set(uint32 threshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < threshold), RT_ERR_INPUT);

    if ((ret = reg_field_write(APOLLOMP_LOW_QUEUE_THr, APOLLOMP_LOW_QUEUE_THf, &threshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_lowQueuelThreshold_set */


/* Function Name:
 *      apollomp_raw_flowctrl_lowQueuelThreshold_get
 * Description:
 *      Get low queue over threshold for EEE usage
 * Input:
 *      None
 * Output:
 *      pThreshold         -  low queue over threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 apollomp_raw_flowctrl_lowQueueThreshold_get(uint32 *pThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pThreshold == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_LOW_QUEUE_THr, APOLLOMP_LOW_QUEUE_THf, pThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_lowQueuelThreshold_get */

/* Function Name:
 *      apollomp_raw_flowctrl_highQueueMask_set
 * Description:
 *      Set per port high queue mask
 * Input:
 *      port      - port id
 *      mask   	- high queue mask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_DSL_VC
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND
 * Note:
 */
int32 apollomp_raw_flowctrl_highQueueMask_set(rtk_port_t port, rtk_bmp_t mask)
{
    int32 ret;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((mask.bits[0] >= (1<<HAL_MAX_NUM_OF_QUEUE())), RT_ERR_PORT_MASK);
    if ((ret = reg_array_field_write(APOLLOMP_HIGH_QUEUE_MSKr,  port, REG_ARRAY_INDEX_NONE, APOLLOMP_HIGH_QUEUE_MSKf, mask.bits)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }
	return RT_ERR_OK;

} /* end of apollomp_raw_flowctrl_highQueueMask_set */

/* Function Name:
 *      apollomp_raw_flowctrl_highQueueMask_get
 * Description:
 *      Get per port high queue mask
 * Input:
 *      port      - port id
 * Output:
 *      pMask   	- high queue mask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 */
int32 apollomp_raw_flowctrl_highQueueMask_get(rtk_port_t port, rtk_bmp_t  *pMask)
{
    int32 ret;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pMask), RT_ERR_NULL_POINTER);

     if ((ret = reg_array_field_read(APOLLOMP_HIGH_QUEUE_MSKr,  port, REG_ARRAY_INDEX_NONE,APOLLOMP_HIGH_QUEUE_MSKf, pMask->bits)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }
	return RT_ERR_OK;

} /* end of apollomp_raw_flowctrl_highQueueMask_get */

/* Function Name:
 *      apollomp_raw_flowctrl_ponThreshold_set
 * Description:
 *      Set flowcontrol thresholds for GPON/EPON mode
 * Input:
 *      none
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 */
int32 apollomp_raw_flowctrl_ponThreshold_set(void)
{
    uint32 data;
    uint32 port;
    uint32 queue;
    int32  ret;

    /* flowctrl ingress */
    if ((ret = apollomp_raw_flowctrl_type_set(APOLLOMP_FLOWCTRL_TYPE_INGRESS)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* drop all 7117 */
    if ((ret = apollomp_raw_flowctrl_dropAllThreshold_set(7117)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* pause all 6781 */
    if ((ret = apollomp_raw_flowctrl_pauseAllThreshold_set(6781)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    {
        uint32 chipId, rev, subtype;
        uint32 high_on, high_off, low_on, low_off;
        dal_apollomp_switch_version_get(&chipId, &rev, &subtype);
    
        switch (rev) 
        {
            case CHIP_REV_ID_0:
            case CHIP_REV_ID_A:
            case CHIP_REV_ID_B:
            case CHIP_REV_ID_C:
            case CHIP_REV_ID_D:
            case CHIP_REV_ID_E:
            case CHIP_REV_ID_F:
                high_on = 4495;
                high_off = 4062;
                low_on = 2764;
                low_off = 2487;
            break;
            case CHIP_REV_ID_G:
            default:
                high_on = 5600;
                high_off = 5500;
                low_on = 4350;
                low_off = 4250;
            break;
        }
        /* system fc-on high threshold: on 4495, off 4062 */
        if ((ret = apollomp_raw_flowctrl_globalFconHighThreshold_set(high_on, high_off)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
            return ret;
        }

        /* system fc-off high threshold: on 4495, off 4062 */
        if ((ret = apollomp_raw_flowctrl_globalFcoffHighThreshold_set(high_on, high_off)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
            return ret;
        }

        /* system fc-on low threshold: on 2764, off 2487 */
        if ((ret = apollomp_raw_flowctrl_globalFconLowThreshold_set(low_on, low_off)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
            return ret;
        }

        /* system fc-off low threshold: on 2764, off 2487 */
        if ((ret = apollomp_raw_flowctrl_globalFcoffLowThreshold_set(low_on, low_off)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
            return ret;
        }
    }

    /* Port fc-on high threshold: on 563, off 539 */
    if ((ret = apollomp_raw_flowctrl_portFconHighThreshold_set(563, 539)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* Port fc-off high threshold: on 563, off 539 */
    if ((ret = apollomp_raw_flowctrl_portFcoffHighThreshold_set(563, 539)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* Port fc-on low threshold: on 346, off 334 */
    if ((ret = apollomp_raw_flowctrl_portFconLowThreshold_set(346, 334)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* Port fc-off low threshold: on 346, off 334 */
    if ((ret = apollomp_raw_flowctrl_portFcoffLowThreshold_set(346, 334)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* ingress flowctrl egress-drop, port-gap 350 */
    if ((ret = apollomp_raw_flowctrl_portEegressGapThreshold_set(350)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* ingress flowctrl egress-drop, PON port threshold 6000, LAN port 1300 */
    HAL_SCAN_ALL_PORT(port)
    {
        if(HAL_IS_PON_PORT(port))
            data=6000;
        else
            data=1300;
        if ((ret = apollomp_raw_flowctrl_portEegressDropThreshold_set(port, data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
            return ret;
        }
    }

    /* ingress flowctrl egress-drop, queue-gap 160 */
    if ((ret = apollomp_raw_flowctrl_queueEegressGapThreshold_set(160)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* ingress flowctrl egress-drop, queue threshold 320 */
    for(queue = 0; queue < HAL_MAX_NUM_OF_QUEUE(); queue++)
    {
        if ((ret = apollomp_raw_flowctrl_queueEegressDropThreshold_set(queue, 320)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
            return ret;
        }
    }

    /* Jumbo enable */
    if ((ret = apollomp_raw_flowctrl_jumboMode_set(ENABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* Jumbo size 6K */
    if ((ret = apollomp_raw_flowctrl_jumboSize_set(APOLLOMP_FLOWCTRL_JUMBO_6K)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* ingress Jumbo global high threshold, on 900 off 810 */
    if ((ret = apollomp_raw_flowctrl_jumboGlobalHighThreshold_set(900, 810)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* ingress Jumbo global low threshold, on 900 off 810 */
    if ((ret = apollomp_raw_flowctrl_jumboGlobalLowThreshold_set(900, 810)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* ingress Jumbo port high threshold, on 432 off 216 */
    if ((ret = apollomp_raw_flowctrl_jumboPortHighThreshold_set(432, 216)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* ingress Jumbo port low threshold, on 432 off 216 */
    if ((ret = apollomp_raw_flowctrl_jumboPortLowThreshold_set(432, 216)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* ingress PON port fc-off high threshold, on 4495 off 4062 */
    if ((ret = apollomp_raw_flowctrl_ponGlobalHighThreshold_set(563, 539)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* ingress PON port fc-off low threshold, on 2764 off 2487 */
    if ((ret = apollomp_raw_flowctrl_ponGlobalLowThreshold_set(346, 334)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* ingress PON port fc-on high threshold, on 2048 off 1952 */
    if ((ret = apollomp_raw_flowctrl_ponPortHighThreshold_set(563, 539)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* ingress PON port fc-on low threshold, on 375 off 327 */
    if ((ret = apollomp_raw_flowctrl_ponPortLowThreshold_set(346, 334)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* ingress PON flowctrl egress-drop, threshold idx 0 threshold 150 */
    if ((ret = apollomp_raw_flowctrl_ponQueueEegressDropThreshold_set(0, 150)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* ingress PON flowctrl egress-drop, queue threshold idx 0 */
    for(queue = 0; queue < HAL_CLASSIFY_SID_NUM(); queue++)
    {
        if ((ret = apollomp_raw_flowctrl_ponQueueEegressDropIndex_set(queue, 0)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
            return ret;
        }
    }

    /* ingress PON flowctrl egress-drop, queue gap 35 */
    for(queue = 0; queue < HAL_CLASSIFY_SID_NUM(); queue++)
    {
        if ((ret = apollomp_raw_flowctrl_ponQueueEegressGapThreshold_set(35)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
            return ret;
        }
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      apollomp_raw_flowctrl_fiberThreshold_set
 * Description:
 *      Set flowcontrol thresholds for fiber mode
 * Input:
 *      none
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 */
int32 apollomp_raw_flowctrl_fiberThreshold_set(void)
{
    uint32 port;
    uint32 queue;
    int32  ret;

    /* flowctrl ingress */
    if ((ret = apollomp_raw_flowctrl_type_set(APOLLOMP_FLOWCTRL_TYPE_INGRESS)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* drop all 7117 */
    if ((ret = apollomp_raw_flowctrl_dropAllThreshold_set(7117)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* pause all 6781 */
    if ((ret = apollomp_raw_flowctrl_pauseAllThreshold_set(6781)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* system fc-on high threshold: on 4495, off 4062 */
    if ((ret = apollomp_raw_flowctrl_globalFconHighThreshold_set(4495, 4062)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* system fc-off high threshold: on 4495, off 4062 */
    if ((ret = apollomp_raw_flowctrl_globalFcoffHighThreshold_set(4495, 4062)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* system fc-on low threshold: on 2764, off 2487 */
    if ((ret = apollomp_raw_flowctrl_globalFconLowThreshold_set(2764, 2487)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* system fc-off low threshold: on 2764, off 2487 */
    if ((ret = apollomp_raw_flowctrl_globalFcoffLowThreshold_set(2764, 2487)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* Port fc-on high threshold: on 563, off 539 */
    if ((ret = apollomp_raw_flowctrl_portFconHighThreshold_set(563, 539)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* Port fc-off high threshold: on 563, off 539 */
    if ((ret = apollomp_raw_flowctrl_portFcoffHighThreshold_set(563, 539)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* Port fc-on low threshold: on 346, off 334 */
    if ((ret = apollomp_raw_flowctrl_portFconLowThreshold_set(346, 334)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* Port fc-off low threshold: on 346, off 334 */
    if ((ret = apollomp_raw_flowctrl_portFcoffLowThreshold_set(346, 334)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* ingress flowctrl egress-drop, port-gap 350 */
    if ((ret = apollomp_raw_flowctrl_portEegressGapThreshold_set(350)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* ingress flowctrl egress-drop, port threshold 6000 */
    HAL_SCAN_ALL_PORT(port)
    {
        if ((ret = apollomp_raw_flowctrl_portEegressDropThreshold_set(port, 6000)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
            return ret;
        }
    }

    /* ingress flowctrl egress-drop, queue-gap 160 */
    if ((ret = apollomp_raw_flowctrl_queueEegressGapThreshold_set(160)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* ingress flowctrl egress-drop, queue threshold 320 */
    for(queue = 0; queue < HAL_MAX_NUM_OF_QUEUE(); queue++)
    {
        if ((ret = apollomp_raw_flowctrl_queueEegressDropThreshold_set(queue, 320)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
            return ret;
        }
    }

    /* Jumbo enable */
    if ((ret = apollomp_raw_flowctrl_jumboMode_set(ENABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* Jumbo size 6K */
    if ((ret = apollomp_raw_flowctrl_jumboSize_set(APOLLOMP_FLOWCTRL_JUMBO_6K)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* ingress Jumbo global high threshold, on 900 off 810 */
    if ((ret = apollomp_raw_flowctrl_jumboGlobalHighThreshold_set(900, 810)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* ingress Jumbo global low threshold, on 900 off 810 */
    if ((ret = apollomp_raw_flowctrl_jumboGlobalLowThreshold_set(900, 810)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* ingress Jumbo port high threshold, on 432 off 216 */
    if ((ret = apollomp_raw_flowctrl_jumboPortHighThreshold_set(432, 216)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* ingress Jumbo port low threshold, on 432 off 216 */
    if ((ret = apollomp_raw_flowctrl_jumboPortLowThreshold_set(432, 216)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* ingress PON port fc-off high threshold, on 4495 off 4062 */
    if ((ret = apollomp_raw_flowctrl_ponGlobalHighThreshold_set(563, 539)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* ingress PON port fc-off low threshold, on 2764 off 2487 */
    if ((ret = apollomp_raw_flowctrl_ponGlobalLowThreshold_set(346, 334)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* ingress PON port fc-on high threshold, on 2048 off 1952 */
    if ((ret = apollomp_raw_flowctrl_ponPortHighThreshold_set(563, 539)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* ingress PON port fc-on low threshold, on 375 off 327 */
    if ((ret = apollomp_raw_flowctrl_ponPortLowThreshold_set(346, 334)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* ingress PON flowctrl egress-drop, threshold idx 0 threshold 320 */
    if ((ret = apollomp_raw_flowctrl_ponQueueEegressDropThreshold_set(0, 320)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* ingress PON flowctrl egress-drop, queue threshold idx 0 */
    for(queue = 0; queue < HAL_CLASSIFY_SID_NUM(); queue++)
    {
        if ((ret = apollomp_raw_flowctrl_ponQueueEegressDropIndex_set(queue, 0)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
            return ret;
        }
    }

    /* ingress PON flowctrl egress-drop, queue gap 120 */
    for(queue = 0; queue < HAL_CLASSIFY_SID_NUM(); queue++)
    {
        if ((ret = apollomp_raw_flowctrl_ponQueueEegressGapThreshold_set(120)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
            return ret;
        }
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      apollomp_raw_flowctrl_defaultThreshold_set
 * Description:
 *      Set flowcontrol thresholds for default mode
 * Input:
 *      none
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 */
int32 apollomp_raw_flowctrl_defaultThreshold_set(void)
{
    uint32 data;
    uint32 port;
    uint32 queue;
    int32  ret;

    /* flowctrl ingress */
    if ((ret = apollomp_raw_flowctrl_type_set(APOLLOMP_FLOWCTRL_TYPE_INGRESS)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* drop all 4030 */
    if ((ret = apollomp_raw_flowctrl_dropAllThreshold_set(4030)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* pause all 3706 */
    if ((ret = apollomp_raw_flowctrl_pauseAllThreshold_set(3706)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* system fc-on high threshold: on 2400, off 2000 */
    if ((ret = apollomp_raw_flowctrl_globalFconHighThreshold_set(2400, 2000)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* system fc-off high threshold: on 2400, off 2000 */
    if ((ret = apollomp_raw_flowctrl_globalFcoffHighThreshold_set(2400, 2000)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* system fc-on low threshold: on 1400, off 1300 */
    if ((ret = apollomp_raw_flowctrl_globalFconLowThreshold_set(1400, 1300)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* system fc-off low threshold: on 1400, off 1300 */
    if ((ret = apollomp_raw_flowctrl_globalFcoffLowThreshold_set(1400, 1300)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* Port fc-on high threshold: on 384, off 360 */
    if ((ret = apollomp_raw_flowctrl_portFconHighThreshold_set(384, 360)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* Port fc-off high threshold: on 384, off 360 */
    if ((ret = apollomp_raw_flowctrl_portFcoffHighThreshold_set(384, 360)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* Port fc-on low threshold: on 64, off 52 */
    if ((ret = apollomp_raw_flowctrl_portFconLowThreshold_set(64, 52)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* Port fc-off low threshold: on 64, off 52 */
    if ((ret = apollomp_raw_flowctrl_portFcoffLowThreshold_set(64, 52)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* ingress flowctrl egress-drop, port-gap 200 */
    if ((ret = apollomp_raw_flowctrl_portEegressGapThreshold_set(200)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* ingress flowctrl egress-drop, pon port threshold 4000,  other port 1000 */
    HAL_SCAN_ALL_PORT(port)
    {
        if(HAL_IS_PON_PORT(port))
            data=4000;
        else
            data=1000;
        if ((ret = apollomp_raw_flowctrl_portEegressDropThreshold_set(port, data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
            return ret;
        }
    }

    /* ingress flowctrl egress-drop, queue-gap 72 */
    if ((ret = apollomp_raw_flowctrl_queueEegressGapThreshold_set(72)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* ingress flowctrl egress-drop, queue threshold 180 */
    for(queue = 0; queue < HAL_MAX_NUM_OF_QUEUE(); queue++)
    {
        if ((ret = apollomp_raw_flowctrl_queueEegressDropThreshold_set(queue, 180)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
            return ret;
        }
    }

    /* Jumbo enable */
    if ((ret = apollomp_raw_flowctrl_jumboMode_set(ENABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* Jumbo size 6K */
    if ((ret = apollomp_raw_flowctrl_jumboSize_set(APOLLOMP_FLOWCTRL_JUMBO_6K)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* ingress Jumbo global high threshold, on 1073 off 966 */
    if ((ret = apollomp_raw_flowctrl_jumboGlobalHighThreshold_set(1073, 966)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* ingress Jumbo global low threshold, on 1073 off 966 */
    if ((ret = apollomp_raw_flowctrl_jumboGlobalLowThreshold_set(1073, 966)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* ingress Jumbo port high threshold, on 432 off 216 */
    if ((ret = apollomp_raw_flowctrl_jumboPortHighThreshold_set(432, 216)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* ingress Jumbo port low threshold, on 432 off 216 */
    if ((ret = apollomp_raw_flowctrl_jumboPortLowThreshold_set(432, 216)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* ingress PON port fc-off high threshold, on 384 off 360 */
    if ((ret = apollomp_raw_flowctrl_ponGlobalHighThreshold_set(384, 360)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* ingress PON port fc-off low threshold, on 64 off 52 */
    if ((ret = apollomp_raw_flowctrl_ponGlobalLowThreshold_set(64, 52)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* ingress PON port fc-on high threshold, on 384 off 360 */
    if ((ret = apollomp_raw_flowctrl_ponPortHighThreshold_set(384, 360)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* ingress PON port fc-on low threshold, on 64 off 52 */
    if ((ret = apollomp_raw_flowctrl_ponPortLowThreshold_set(64, 52)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* ingress PON flowctrl egress-drop, threshold idx 0 threshold 320 */
    if ((ret = apollomp_raw_flowctrl_ponQueueEegressDropThreshold_set(0, 72)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* ingress PON flowctrl egress-drop, queue threshold idx 0 */
    for(queue = 0; queue < HAL_CLASSIFY_SID_NUM(); queue++)
    {
        if ((ret = apollomp_raw_flowctrl_ponQueueEegressDropIndex_set(queue, 0)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
            return ret;
        }
    }

    /* ingress PON flowctrl egress-drop, queue gap 160 */
    for(queue = 0; queue < HAL_CLASSIFY_SID_NUM(); queue++)
    {
        if ((ret = apollomp_raw_flowctrl_ponQueueEegressGapThreshold_set(180)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
            return ret;
        }
    }

    return RT_ERR_OK;
}

static int32 _patch_sw_rest(void)
{
    uint32 data;
    int32  ret;

    /* software reset */
    data = 0x1;
    if ((ret = reg_field_write(APOLLOMP_SOFTWARE_RSTr, APOLLOMP_SW_RSTf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}

static int32 _patch_35m_gpon(void)
{
    uint32 data;
    int32  ret;

    /* buffer mode, bit0 (0=2.0Mb, 1=3.5Mb) */
    data = 0x1;
    if ((ret = reg_write(APOLLOMP_SYS_PKT_BUF_CTRLr, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* software reset */
    if ((ret = _patch_sw_rest()) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* Configure thresholds */
    return apollomp_raw_flowctrl_ponThreshold_set();
}

static int32 _patch_35m_fiber(void)
{
    uint32 data;
    int32  ret;

    /* buffer mode, bit0 (0=2.0Mb, 1=3.5Mb) */
    data = 0x1;
    if ((ret = reg_write(APOLLOMP_SYS_PKT_BUF_CTRLr, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* software reset */
    if ((ret = _patch_sw_rest()) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* Configure thresholds */
    return apollomp_raw_flowctrl_fiberThreshold_set();
}

static int32 _patch_20m(void)
{
    uint32 data;
    int32  ret;

    /* buffer mode, bit0 (0=2.0Mb, 1=3.5Mb) */
    data = 0x0;
    if ((ret = reg_write(APOLLOMP_SYS_PKT_BUF_CTRLr, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* software reset */
    if ((ret = _patch_sw_rest()) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

    /* Configure thresholds */
    return apollomp_raw_flowctrl_defaultThreshold_set();
}

/* Function Name:
 *      apollomp_raw_flowctrl_patch
 * Description:
 *      patch the flow control parameter by type
 * Input:
 *      patch_type      - 3.5Mb GPON, 3.5Mb Fiber, 2.0Mb.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_CHIP_NOT_SUPPORTED
 * Note:
 */
int32 apollomp_raw_flowctrl_patch(rtk_flowctrl_patch_t patch_type)
{
    int32  ret=RT_ERR_OK;

    switch(patch_type)
    {
        case FLOWCTRL_PATCH_20M:
            ret = _patch_20m();
            break;

        case FLOWCTRL_PATCH_35M_FIBER:
            ret = _patch_35m_fiber();
            break;

        case FLOWCTRL_PATCH_35M_GPON:
            ret = _patch_35m_gpon();
            break;
        default:
            ret = RT_ERR_CHIP_NOT_SUPPORTED;
            break;
    }

    return ret;
}

/* Function Name:
 *      apollomp_raw_flowctrl_ponLatchMaxUsedPage_set
 * Description:
 *      Set PON queue to latch maxumum used page count
 * Input:
 *      enable           -        latch/dis-latch maximum used page count
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 apollomp_raw_flowctrl_ponLatchMaxUsedPage_set( rtk_enable_t enable)
{
    int32   ret;
    uint32 tmp;
    /*parameter check*/
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    tmp = !enable;
    if ((ret = reg_field_write(APOLLOMP_PON_MISC_CFGr, APOLLOMP_PON_LATCH_MAX_USEDPAGEf, &tmp)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_ponLatchMaxUsedPage_get */

/* Function Name:
 *      apollomp_raw_flowctrl_ponLatchMaxUsedPage_set
 * Description:
 *      Get PON queue to latch maxumum used page count
 * Input:
 *      None
 * Output:
 *      pEnable           -        latch/dis-latch maximum used page count
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 apollomp_raw_flowctrl_ponLatchMaxUsedPage_get( rtk_enable_t *pEnable)
{
    int32   ret;
    uint32 tmp;

    if ((ret = reg_field_read(APOLLOMP_PON_MISC_CFGr, APOLLOMP_PON_LATCH_MAX_USEDPAGEf, &tmp)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL|MOD_DAL), "");
        return ret;
    }

    *pEnable = !tmp;

    return RT_ERR_OK;
} /* end of apollomp_raw_flowctrl_ponLatchMaxUsedPage_get */

