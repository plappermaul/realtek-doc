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
 * Purpose : Definition of Classifyication API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) CPU tag functions set/get
 */



/*
 * Include Files
 */
#include <common/rt_type.h>
#include <dal/apollomp/dal_apollomp.h>
#include <rtk/cpu.h>
#include <dal/apollomp/dal_apollomp_cpu.h>

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
 *      dal_apollomp_cpu_init
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
dal_apollomp_cpu_init(void)
{
    rtk_portmask_t port_mask;
    int32 ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_CPU),"%s",__FUNCTION__);

    cpu_init = INIT_COMPLETED;

    if ((ret = dal_apollomp_cpu_trapInsertTag_set(ENABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CPU|MOD_DAL), "");
        return ret;
    }

    if ((ret = dal_apollomp_cpu_tagFormat_set(CPU_TAG_FMT_APOLLO)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CPU|MOD_DAL), "");
        return ret;
    }

    osal_memset(&port_mask, 0, sizeof(rtk_portmask_t));
    RTK_PORTMASK_PORT_SET(port_mask, HAL_GET_CPU_PORT());
    if ((ret = dal_apollomp_cpu_awarePortMask_set(port_mask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CPU|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_cpu_init */

/* Function Name:
 *      dal_apollomp_cpu_awarePortMask_set
 * Description:
 *      Set CPU awared port mask.
 * Input:
 *      port_mask   - CPU awared port mask
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
dal_apollomp_cpu_awarePortMask_set(rtk_portmask_t port_mask)
{
    uint32 enable;
    uint32 port;
    int32  ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_CPU),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(cpu_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORTMASK_VALID(port_mask), RT_ERR_INPUT);

    for(port=0; port<HAL_GET_PORTNUM(); port++)
    {
        if(RTK_PORTMASK_IS_PORT_SET(port_mask, port))
        {
            enable = 1;
            if ((ret = reg_array_field_write(APOLLOMP_MAC_CPU_TAG_AWARE_CTRLr,port,REG_ARRAY_INDEX_NONE,APOLLOMP_ENf,&enable)) != RT_ERR_OK)
                RT_ERR(ret, (MOD_DAL|MOD_CPU), "");
        }
        else
        {
            enable = 0;
            if ((ret = reg_array_field_write(APOLLOMP_MAC_CPU_TAG_AWARE_CTRLr,port,REG_ARRAY_INDEX_NONE,APOLLOMP_ENf,&enable)) != RT_ERR_OK)
                RT_ERR(ret, (MOD_DAL|MOD_CPU), "");
        }
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_cpu_awarePortMask_set */

/* Function Name:
 *      dal_apollomp_cpu_awarePortMask_get
 * Description:
 *      Get CPU awared port mask.
 * Input:
 *      pPort_mask   - the pointer of CPU awared port mask
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
dal_apollomp_cpu_awarePortMask_get(rtk_portmask_t *pPort_mask)
{
    uint32 enable;
    uint32 port;
    int32  ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_CPU),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(cpu_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPort_mask), RT_ERR_NULL_POINTER);

    osal_memset(pPort_mask, 0x0, sizeof(rtk_portmask_t));
    for(port=0; port<HAL_GET_PORTNUM(); port++)
    {
        if ((ret = reg_array_field_read(APOLLOMP_MAC_CPU_TAG_AWARE_CTRLr,port,REG_ARRAY_INDEX_NONE,APOLLOMP_ENf,&enable))
            != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_CPU), "");
        }

        if(enable)
        {
            RTK_PORTMASK_PORT_SET(*pPort_mask, port);
        }
        else
        {
            RTK_PORTMASK_PORT_CLEAR(*pPort_mask, port);
        }
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_cpu_awarePortMask_get */

/* Function Name:
 *      dal_apollomp_cpu_tagFormat_set
 * Description:
 *      Set CPU tag format.
 * Input:
 *      mode    - CPU tag format mode
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
dal_apollomp_cpu_tagFormat_set(rtk_cpu_tag_fmt_t mode)
{
    uint32 data;
    int32  ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_CPU),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(cpu_init);

    /* parameter check */
    RT_PARAM_CHK((CPU_TAG_FMT_END <= mode), RT_ERR_INPUT);

    data = mode;
    if ((ret = reg_field_write(APOLLOMP_MAC_CPU_TAG_CTRLr, APOLLOMP_TAG_FORMATf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_CPU), "");
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_cpu_tagFormat_set */

/* Function Name:
 *      dal_apollomp_cpu_tagFormat_get
 * Description:
 *      Get CPU tag format.
 * Input:
 *      pMode    - the pointer of CPU tag format mode
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
dal_apollomp_cpu_tagFormat_get(rtk_cpu_tag_fmt_t *pMode)
{
    uint32 data;
    int32  ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_CPU),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(cpu_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pMode), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_MAC_CPU_TAG_CTRLr, APOLLOMP_TAG_FORMATf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_CPU), "");
    }
    *pMode = data;

    return RT_ERR_OK;
} /* end of dal_apollomp_cpu_tagFormat_get */

/* Function Name:
 *      dal_apollomp_cpu_trapInsertTag_set
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
dal_apollomp_cpu_trapInsertTag_set(rtk_enable_t state)
{
    uint32 data;
    int32  ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_CPU),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(cpu_init);

    /* parameter check */
    RT_PARAM_CHK((state>=RTK_ENABLE_END), RT_ERR_OUT_OF_RANGE);

    data = state;
    if ((ret = reg_field_write(APOLLOMP_MAC_CPU_TAG_CTRLr, APOLLOMP_TRAP_TAGET_INSERT_ENf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_CPU), "");
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_cpu_trapInsertTag_set */

/* Function Name:
 *      dal_apollomp_cpu_trapInsertTag_get
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
dal_apollomp_cpu_trapInsertTag_get(rtk_enable_t *pState)
{
    uint32 data;
    int32  ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_CPU),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(cpu_init);

    /* parameter check */
    RT_PARAM_CHK((NULL==pState), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_MAC_CPU_TAG_CTRLr, APOLLOMP_TRAP_TAGET_INSERT_ENf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_CPU), "");
    }
    *pState = data;

    return RT_ERR_OK;
} /* end of dal_apollomp_cpu_trapInsertTag_get */



