/*
 * Copyright (C) 2015 Realtek Semiconductor Corp.
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
 * Purpose : Definition of CPU tag API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) CPU tag functions set/get
 */


/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/cpu.h>
#include <dal/rtl9607c/dal_rtl9607c.h>
#include <dal/rtl9607c/dal_rtl9607c_cpu.h>

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
static int
_dal_rtl9607c_cpu_getGmacIdByPort(rtk_port_t port, uint32 *gmacId)
{
    /* parameter check */
    RT_PARAM_CHK((NULL == gmacId), RT_ERR_NULL_POINTER);

    switch (port)
    {
        case 9:
            *gmacId = CPU_GMAC_0;
            break;
        case 10:
            *gmacId = CPU_GMAC_1;
            break;
        case 7:
            *gmacId = CPU_GMAC_2;
            break;
        default:
            return RT_ERR_OUT_OF_RANGE;;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl9607c_cpu_init
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
dal_rtl9607c_cpu_init(void)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_CPU), "%s", __FUNCTION__);

    cpu_init = INIT_COMPLETED;

    if ((ret = dal_rtl9607c_cpu_trapInsertTagByPort_set(9, ENABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CPU|MOD_DAL), "");
        return ret;
    }

    if ((ret = dal_rtl9607c_cpu_trapInsertTagByPort_set(10, ENABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CPU|MOD_DAL), "");
        return ret;
    }

    if ((ret = dal_rtl9607c_cpu_trapInsertTagByPort_set(7, DISABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CPU|MOD_DAL), "");
        return ret;
    }

    if ((ret = dal_rtl9607c_cpu_tagAwareByPort_set(9, ENABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CPU|MOD_DAL), "");
        return ret;
    }

    if ((ret = dal_rtl9607c_cpu_tagAwareByPort_set(10, ENABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CPU|MOD_DAL), "");
        return ret;
    }

    if ((ret = dal_rtl9607c_cpu_tagAwareByPort_set(7, DISABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CPU|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607c_cpu_init */

/* Function Name:
 *      dal_rtl9607c_cpu_trapInsertTagByPort_set
 * Description:
 *      Set trap CPU insert tag state.
 * Input:
 *      port     - CPU port ID
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
dal_rtl9607c_cpu_trapInsertTagByPort_set(rtk_port_t port, rtk_enable_t state)
{
    int32   ret;
    uint32  gmacId;
    uint32  data;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_CPU), "%s", __FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(cpu_init);

    /* parameter check */
    RT_PARAM_CHK((state >= RTK_ENABLE_END), RT_ERR_OUT_OF_RANGE);

    if ((ret = _dal_rtl9607c_cpu_getGmacIdByPort(port, &gmacId)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_CPU), "");
        return ret;
    }

    data = state;
    if ((ret = reg_array_field_write(RTL9607C_CPU_TAG_INSERT_CTRLr,
            gmacId, REG_ARRAY_INDEX_NONE, RTL9607C_ENf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_CPU), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607c_cpu_trapInsertTagByPort_set */

/* Function Name:
 *      dal_rtl9607c_cpu_trapInsertTagByPort_get
 * Description:
 *      Get trap CPU insert tag state.
 * Input:
 *      port     - CPU port ID
 *      pState   - the pointer of insert CPU tag state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_OUT_OF_RANGE
 * Note:
 *      Must initialize cpu tag module before calling any cpu tag APIs.
 */
int32
dal_rtl9607c_cpu_trapInsertTagByPort_get(rtk_port_t port, rtk_enable_t *pState)
{
    int32   ret;
    uint32  gmacId;
    uint32  data;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_CPU), "%s", __FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(cpu_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    if ((ret = _dal_rtl9607c_cpu_getGmacIdByPort(port, &gmacId)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_CPU), "");
        return ret;
    }

    if ((ret = reg_array_field_read(RTL9607C_CPU_TAG_INSERT_CTRLr,
            gmacId, REG_ARRAY_INDEX_NONE, RTL9607C_ENf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_CPU), "");
        return ret;
    }

    if (data)
        *pState = ENABLED;
    else
        *pState = DISABLED;

    return RT_ERR_OK;
} /* end of dal_rtl9607c_cpu_trapInsertTagByPort_get */

/* Function Name:
 *      dal_rtl9607c_cpu_tagAwareByPort_set
 * Description:
 *      Set CPU tag aware state.
 * Input:
 *      port     - CPU port ID
 *      state    - CPU tag aware state
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
dal_rtl9607c_cpu_tagAwareByPort_set(rtk_port_t port, rtk_enable_t state)
{
    int32   ret;
    uint32  gmacId;
    uint32  data;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_CPU), "%s", __FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(cpu_init);

    /* parameter check */
    RT_PARAM_CHK((state >= RTK_ENABLE_END), RT_ERR_OUT_OF_RANGE);

    if ((ret = _dal_rtl9607c_cpu_getGmacIdByPort(port, &gmacId)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_CPU), "");
        return ret;
    }

    data = state;
    if ((ret = reg_array_field_write(RTL9607C_CPU_TAG_AWARE_CTRLr,
            gmacId, REG_ARRAY_INDEX_NONE, RTL9607C_ENf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_CPU), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607c_cpu_tagAwareByPort_set */

/* Function Name:
 *      dal_rtl9607c_cpu_tagAwareByPort_get
 * Description:
 *      Get CPU tag aware state.
 * Input:
 *      port     - CPU port ID
 *      pState   - the pointer of CPU tag aware state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_OUT_OF_RANGE
 * Note:
 *      Must initialize cpu tag module before calling any cpu tag APIs.
 */
int32
dal_rtl9607c_cpu_tagAwareByPort_get(rtk_port_t port, rtk_enable_t *pState)
{
    int32   ret;
    uint32  gmacId;
    uint32  data;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_CPU), "%s", __FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(cpu_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    if ((ret = _dal_rtl9607c_cpu_getGmacIdByPort(port, &gmacId)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_CPU), "");
        return ret;
    }

    if ((ret = reg_array_field_read(RTL9607C_CPU_TAG_AWARE_CTRLr,
            gmacId, REG_ARRAY_INDEX_NONE, RTL9607C_ENf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_CPU), "");
        return ret;
    }

    if (data)
        *pState = ENABLED;
    else
        *pState = DISABLED;

    return RT_ERR_OK;
} /* end of dal_rtl9607c_cpu_tagAwareByPort_get */
