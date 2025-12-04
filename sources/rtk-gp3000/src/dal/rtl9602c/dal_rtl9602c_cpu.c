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
 * $Revision: 39101 $
 * $Date: 2013-05-03 17:35:27 +0800 (Fri, 03 May 2013) $
 *
 * Purpose : Definition of Classifyication API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) CPU tag functions set/get
 */



/*
 * Include Files
 */
#include <common/rt_type.h>
#include <dal/rtl9602c/dal_rtl9602c.h>
#include <rtk/cpu.h>
#include <dal/rtl9602c/dal_rtl9602c_cpu.h>

/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */
static uint32    cpu_init = INIT_NOT_COMPLETED;

/*
 * Macro Declaration
 */


/*
 * Function Declaration
 */
/* Function Name:
 *      dal_rtl9602c_cpu_init
 * Description:
 *      Initialize cpu tag module.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize classification module before calling any classification APIs.
 */
int32
dal_rtl9602c_cpu_init(void)
{
    int32 ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_CPU),"%s",__FUNCTION__);

    cpu_init = INIT_COMPLETED;

    if ((ret = dal_rtl9602c_cpu_trapInsertTag_set(ENABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CPU|MOD_DAL), "");
        return ret;
    }

    if ((ret = dal_rtl9602c_cpu_tagAware_set(ENABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CPU|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_cpu_init */

/* Function Name:
 *      dal_rtl9602c_cpu_trapInsertTag_set
 * Description:
 *      Set trap CPU insert tag state.
 * Input:
 *      state    - insert CPU tag state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_OUT_OF_RANGE
 * Note:
 *      Must initialize cpu tag module before calling any cpu tag APIs.
 */
int32
dal_rtl9602c_cpu_trapInsertTag_set(rtk_enable_t state)
{
    uint32 data;
    int32  ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_CPU),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(cpu_init);

    /* parameter check */
    RT_PARAM_CHK((state>=RTK_ENABLE_END), RT_ERR_OUT_OF_RANGE);

    data = state;
    if ((ret = reg_field_write(RTL9602C_MAC_CPU_TAG_CTRLr, RTL9602C_TRAP_TAGET_INSERT_ENf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_CPU), "");
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_cpu_trapInsertTag_set */

/* Function Name:
 *      dal_rtl9602c_cpu_trapInsertTag_get
 * Description:
 *      Get trap CPU insert tag state.
 * Input:
 *      pState    - the pointer of insert CPU tag state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      Must initialize cpu tag module before calling any cpu tag APIs.
 */
int32
dal_rtl9602c_cpu_trapInsertTag_get(rtk_enable_t *pState)
{
    uint32 data;
    int32  ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_CPU),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(cpu_init);

    /* parameter check */
    RT_PARAM_CHK((NULL==pState), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9602C_MAC_CPU_TAG_CTRLr, RTL9602C_TRAP_TAGET_INSERT_ENf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_CPU), "");
    }
    *pState = data;

    return RT_ERR_OK;
} /* end of dal_rtl9602c_cpu_trapInsertTag_get */

/* Function Name:
 *      dal_rtl9602c_cpu_tagAware_set
 * Description:
 *      Set CPU tag aware state.
 * Input:
 *      state    - CPU tag aware state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      Must initialize cpu tag module before calling any cpu tag APIs.
 */
int32
dal_rtl9602c_cpu_tagAware_set(rtk_enable_t state)
{
    uint32 data;
    int32  ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_CPU),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(cpu_init);

    /* parameter check */
    RT_PARAM_CHK((state>=RTK_ENABLE_END), RT_ERR_OUT_OF_RANGE);

    data = state;
    if ((ret = reg_field_write(RTL9602C_MAC_CPU_TAG_CTRLr, RTL9602C_TAG_AWAREf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_CPU), "");
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_cpu_tagAware_set */

/* Function Name:
 *      dal_rtl9602c_cpu_tagAware_get
 * Description:
 *      Get CPU tag aware state.
 * Input:
 *      pState    - the pointer of CPU tag aware state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      Must initialize cpu tag module before calling any cpu tag APIs.
 */
int32
dal_rtl9602c_cpu_tagAware_get(rtk_enable_t *pState)
{
    uint32 data;
    int32  ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_CPU),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(cpu_init);

    /* parameter check */
    RT_PARAM_CHK((NULL==pState), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9602C_MAC_CPU_TAG_CTRLr, RTL9602C_TAG_AWAREf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_CPU), "");
    }
    *pState = data;

    return RT_ERR_OK;
} /* end of dal_rtl9602c_cpu_tagAware_get */
