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
 * $Revision:
 * $Date: 2013-05-03 17:35:27 +0800 $
 *
 * Purpose : Definition those public Interrupt APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *           (1) Interrupt parameter settings
 */


/*
 * Include Files
 */
#include <common/rt_type.h>
#include <dal/rtl9602c/dal_rtl9602c.h>
#include <dal/rtl9602c/dal_rtl9602c_intr.h>
#include <rtk/ppstod.h>
#include <dal/rtl9602c/dal_rtl9602c_ppstod.h>
#include <rtk/irq.h>
#include <rtk/intr.h>
#include "module/intr_bcaster/intr_bcaster.h"
#if defined(CONFIG_SDK_KERNEL_LINUX)&&defined(CONFIG_RTK_SWITCH_ISSUE_LOG)
#include "module/switch_issue_log/switch_issue_log_main.h"
#endif

/*
 * Symbol Definition
 */
#define IS_RTL9602C_VALID_INTERRUPT_TYPE(type) \
    (((INTR_TYPE_LINK_CHANGE != type) && \
     (INTR_TYPE_METER_EXCEED != type) && \
     (INTR_TYPE_LEARN_OVER != type) && \
     (INTR_TYPE_SPEED_CHANGE != type) && \
     (INTR_TYPE_SPECIAL_CONGEST != type) && \
     (INTR_TYPE_LOOP_DETECTION != type) && \
     (INTR_TYPE_DEBUG != type) && \
     (INTR_TYPE_ACL_ACTION != type) && \
     (INTR_TYPE_GPHY != type) && \
     (INTR_TYPE_SERDES != type) && \
     (INTR_TYPE_GPON != type) && \
     (INTR_TYPE_EPON != type) && \
     (INTR_TYPE_PTP != type) && \
     (INTR_TYPE_DYING_GASP != type) && \
     (INTR_TYPE_THERMAL != type) && \
     (INTR_TYPE_SMARTCARD != type) && \
     (INTR_TYPE_SWITCH_ERROR != type) && \
     (INTR_TYPE_SFP != type) && \
     (INTR_TYPE_TOD != type) && \
     (INTR_TYPE_ALL != type)) ? 0 : 1)

/*
 * Data Declaration
 */
static uint32     intr_init = {INIT_NOT_COMPLETED};

/* The order of ims/imr here should be the same as in intr.h
 * There should be no array indexing to dummy entries
 * which should be blocked by IS_RTL9602C_VALID_INTERRUPT_TYPE
 */
static uint32   intr_reg_ims_field[] = {
    RTL9602C_IMS_LINK_CHGf,
    RTL9602C_IMS_METER_EXCEEDf,
    RTL9602C_IMS_L2_LRN_OVERf,
    RTL9602C_IMS_SPE_CHGf,
    RTL9602C_IMS_SPE_CONGESTf,
    RTL9602C_IMS_LOOPf,
    RTL9602C_IMS_LOOPf, /* Dummy entry */
    RTL9602C_IMS_ACLf,
    RTL9602C_IMS_GPHYf,
    RTL9602C_IMS_SERDESf,
    RTL9602C_IMS_GPONf,
    RTL9602C_IMS_EPONf,
    RTL9602C_IMS_PTP_1_SECf,
    RTL9602C_IMS_DYING_GASPf,
    RTL9602C_IMS_THERMAL_ALARMf,
    RTL9602C_IMS_THERMAL_ALARMf, /* Dummy entry */
    RTL9602C_IMS_THERMAL_ALARMf, /* Dummy entry */
    RTL9602C_IMS_THERMAL_ALARMf, /* Dummy entry */
    RTL9602C_IMS_THERMAL_ALARMf, /* Dummy entry */
    RTL9602C_IMS_DBGOf,
    RTL9602C_IMS_SCf,
    RTL9602C_IMS_CRASHf,
    RTL9602C_IMS_SFPf,
    RTL9602C_IMS_SFPf, /* Dummy entry */
    RTL9602C_IMS_TODf,
};

static uint32   intr_reg_ims_bitPos[] = {
    0,  /* RTL9602C_IMS_LINK_CHGf */
    1,  /* RTL9602C_IMS_METER_EXCEEDf */
    2,  /* RTL9602C_IMS_L2_LRN_OVERf */
    3,  /* RTL9602C_IMS_SPE_CHGf */
    4,  /* RTL9602C_IMS_SPE_CONGESTf */
    5,  /* RTL9602C_IMS_LOOPf */
    5,  /* RTL9602C_IMS_LOOPf */ /* Dummy entry */
    7,  /* RTL9602C_IMS_ACLf */
    8,  /* RTL9602C_IMS_GPHYf */
    9,  /* RTL9602C_IMS_SERDESf */
    10, /* RTL9602C_IMS_GPONf */
    11, /* RTL9602C_IMS_EPONf */
    12, /* RTL9602C_IMR_PTP_1_SECf */
    13, /* RTL9602C_IMS_DYING_GASPf */
    14, /* RTL9602C_IMS_THERMAL_ALARMf */
    14, /* RTL9602C_IMS_THERMAL_ALARMf */ /* Dummy entry */
    14, /* RTL9602C_IMS_THERMAL_ALARMf */ /* Dummy entry */
    14, /* RTL9602C_IMS_THERMAL_ALARMf */ /* Dummy entry */
    14, /* RTL9602C_IMS_THERMAL_ALARMf */ /* Dummy entry */
    6,  /* RTL9602C_IMS_DBGOf */
    15, /* RTL9602C_IMS_SCf */
    16, /* RTL9602C_IMS_CRASHf */
    17, /* RTL9602C_IMS_SFP */
    17, /* RTL9602C_IMS_SFP */ /* Dummy entry */
    18, /* RTL9602C_IMS_TOD */
};

static uint32   intr_reg_imr_field[] = {
    RTL9602C_IMR_LINK_CHGf,
    RTL9602C_IMR_METER_EXCEEDf,
    RTL9602C_IMR_L2_LRN_OVERf,
    RTL9602C_IMR_SPE_CHGf,
    RTL9602C_IMR_SPE_CONGESTf,
    RTL9602C_IMR_LOOPf,
    RTL9602C_IMR_LOOPf, /* Dummy entry */
    RTL9602C_IMR_ACLf,
    RTL9602C_IMR_GPHYf,
    RTL9602C_IMR_SERDESf,
    RTL9602C_IMR_GPONf,
    RTL9602C_IMR_EPONf,
    RTL9602C_IMR_PTP_1_SECf,
    RTL9602C_IMR_DYING_GASPf,
    RTL9602C_IMR_THERMAL_ALARMf,
    RTL9602C_IMR_THERMAL_ALARMf, /* Dummy entry */
    RTL9602C_IMR_THERMAL_ALARMf, /* Dummy entry */
    RTL9602C_IMR_THERMAL_ALARMf, /* Dummy entry */
    RTL9602C_IMR_THERMAL_ALARMf, /* Dummy entry */
    RTL9602C_IMR_DBGOf,
    RTL9602C_IMR_SCf,
    RTL9602C_IMR_CRASHf,
    RTL9602C_IMR_SFPf,
    RTL9602C_IMR_SFPf, /* Dummy entry */
    RTL9602C_IMR_TODf,
};

/*
 * Function Declaration
 */

/* Function Name:
 *      dal_rtl9602c_intr_init
 * Description:
 *      Initialize interrupt module.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize interrupt module before calling any interrupt APIs.
 */
int32
dal_rtl9602c_intr_init(void)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_INTR),"%s",__FUNCTION__);
    intr_init = INIT_COMPLETED;

    /*reset interrupt mask register*/
    if((ret=dal_rtl9602c_intr_imr_set(INTR_TYPE_ALL,DISABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_INTR),"");
        intr_init = INIT_NOT_COMPLETED;
        return ret;
    }

    /*clear all of interrupt status*/
    if((ret=dal_rtl9602c_intr_ims_clear(INTR_TYPE_ALL)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_INTR),"");
        intr_init = INIT_NOT_COMPLETED;
        return ret;
    }

    /*clear port speed shange status*/
    if((ret=dal_rtl9602c_intr_speedChangeStatus_clear()) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_INTR),"");
        intr_init = INIT_NOT_COMPLETED;
        return ret;
    }

    /*clear port linkup status*/
    if((ret=dal_rtl9602c_intr_linkupStatus_clear()) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_INTR),"");
        intr_init = INIT_NOT_COMPLETED;
        return ret;
    }

    /*clear port linkdown status*/
    if((ret=dal_rtl9602c_intr_linkdownStatus_clear()) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_INTR),"");
        intr_init = INIT_NOT_COMPLETED;
        return ret;
    }

    /*clear gphy status*/
    if((ret=dal_rtl9602c_intr_gphyStatus_clear()) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_INTR),"");
        intr_init = INIT_NOT_COMPLETED;
        return ret;
    }
    return RT_ERR_OK;
} /* end of dal_rtl9602c_intr_init */


/* Function Name:
 *      dal_rtl9602c_intr_polarity_set
 * Description:
 *      Set interrupt polarity mode
 * Input:
 *      mode - Interrupt polarity mode.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - OK
 *      RT_ERR_FAILED   - Failed
 *      RT_ERR_INPUT    - Invalid input parameters.
 * Note:
 *      The API can set Interrupt polarity mode.
 *      The modes that can be set as following:
 *      - INTR_POLAR_HIGH
 *      - INTR_POLAR_LOW
 */
int32
dal_rtl9602c_intr_polarity_set(rtk_intr_polarity_t polar)
{
    int32   ret;
    uint32 ipolar;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_INTR),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(intr_init);

    /* parameter check */
    RT_PARAM_CHK((INTR_POLAR_END <= polar), RT_ERR_OUT_OF_RANGE);

    ipolar = polar;

    if((ret = reg_field_write(RTL9602C_INTR_CTRLr,RTL9602C_INTR_POLARITYf, &ipolar) ) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_INTR),"");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_intr_polarity_set */


/* Function Name:
 *      dal_rtl9602c_intr_polarity_mode_get
 * Description:
 *      Get Interrupt polarity mode
 * Input:
 *      None
 * Output:
 *      pMode - Interrupt polarity mode
 * Return:
 *      RT_ERR_OK       - OK
 *      RT_ERR_FAILED   - Failed
 *      RT_ERR_INPUT    - Invalid input parameters.
 * Note:
 *      The API can get Interrupt polarity mode.
 *    The modes that can be got as following:
 *      - INTR_POLAR_HIGH
 *      - INTR_POLAR_LOW
 */
int32
dal_rtl9602c_intr_polarity_get(rtk_intr_polarity_t *pPolar)
{
    int32   ret;
    uint32 ipolar;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_INTR),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(intr_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPolar), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9602C_INTR_CTRLr, RTL9602C_INTR_POLARITYf, &ipolar)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_INTR), "");
        return ret;
    }

    *pPolar = ipolar;

    return RT_ERR_OK;
} /* end of dal_rtl9602c_intr_polarity_get */


/* Function Name:
 *      dal_rtl9602c_intr_imr_set
 * Description:
 *      Set interrupt mask.
 * Input:
 *      intr          - interrupt type
 *      enable      - interrupt state
 *
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_OUT_OF_RANGE
 * Note:
 *     None.
 */
int32
dal_rtl9602c_intr_imr_set(rtk_intr_type_t intr, rtk_enable_t enable)
{
    int32   ret,i;
    uint32  regValue;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_INTR),"%s",__FUNCTION__);
    RT_INIT_CHK(intr_init);

    /* parameter check */
    RT_PARAM_CHK((INTR_TYPE_END <= intr), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(!IS_RTL9602C_VALID_INTERRUPT_TYPE(intr), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_OUT_OF_RANGE);


    if(INTR_TYPE_ALL == intr)
    {
        regValue = 0;
        for(i = 0; i < INTR_TYPE_ALL; i++)
        {
            if(!IS_RTL9602C_VALID_INTERRUPT_TYPE(i))
                continue;

            regValue |= ((enable&0x1)<<i);
        }
        if ((ret = reg_write(RTL9602C_INTR_IMRr, &regValue)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_INTR), "");
            return ret;
        }
    }
    else
    {
        regValue = enable;
        if ((ret = reg_field_write(RTL9602C_INTR_IMRr, intr_reg_imr_field[intr], &regValue)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_INTR), "");
            return ret;
        }
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_intr_imr_set */


/* Function Name:
 *      dal_rtl9602c_intr_imr_get
 * Description:
 *      Get interrupt mask.
 * Input:
 *      intr            - interrupt type
 *      pEnable      - pointer of return status
 *
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_INPUT
 *      RT_ERR_OUT_OF_RANGE
 * Note:
 *      None.
 */
 extern int32
dal_rtl9602c_intr_imr_get(rtk_intr_type_t intr, rtk_enable_t *pEnable){

    uint32 ret;
    uint32 regValue;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_INTR),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(intr_init);

    /* parameter check */
    RT_PARAM_CHK((INTR_TYPE_END <= intr), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(!IS_RTL9602C_VALID_INTERRUPT_TYPE(intr), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if(INTR_TYPE_ALL == intr)
    {
         if ((ret = reg_read(RTL9602C_INTR_IMRr, &regValue)) != RT_ERR_OK)
         {
             RT_ERR(ret, (MOD_DAL|MOD_INTR), "");
             return ret;
         }
    }
    else
    {
        if ((ret = reg_field_read(RTL9602C_INTR_IMRr, intr_reg_imr_field[intr], &regValue)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_INTR), "");
            return ret;
        }
    }

    *pEnable = regValue;

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rtl9602c_intr_ims_get
 * Description:
 *      Get interrupt status.
 * Input:
 *      intr            - interrupt type
 *      pState        - pointer of return status
 *
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_INPUT
 *      RT_ERR_OUT_OF_RANGE
 * Note:
 *      None.
 */
int32
dal_rtl9602c_intr_ims_get(rtk_intr_type_t intr, rtk_enable_t *pState)
{
    int32   ret;
    uint32  regValue;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_INTR),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(intr_init);

    /* parameter check */
    RT_PARAM_CHK((INTR_TYPE_END <= intr), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(!IS_RTL9602C_VALID_INTERRUPT_TYPE(intr), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    if(INTR_TYPE_ALL == intr)
    {
        if ((ret = reg_read(RTL9602C_INTR_IMSr, &regValue)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_INTR), "");
            return ret;
        }
    }
    else
    {
        if ((ret = reg_field_read(RTL9602C_INTR_IMSr, intr_reg_ims_field[intr], &regValue)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_INTR), "intr=%d",intr);
            return ret;
        }
    }

    *pState = regValue;

    return RT_ERR_OK;
} /* end of dal_rtl9602c_intr_ims_get */


/* Function Name:
 *      dal_rtl9602c_intr_ims_clear
 * Description:
 *      Clear interrupt status.
 * Input:
 *      intr            - interrupt type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_OUT_OF_RANGE
 * Note:
 *      None.
 */
int32
dal_rtl9602c_intr_ims_clear(rtk_intr_type_t intr)
{
    int32   ret,i;
    uint32  regValue;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_INTR),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(intr_init);

    /* parameter check */
    RT_PARAM_CHK((INTR_TYPE_END <= intr), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(!IS_RTL9602C_VALID_INTERRUPT_TYPE(intr), RT_ERR_INPUT);

    if(INTR_TYPE_ALL == intr)
    {
        regValue = 0;
        for(i = 0; i < INTR_TYPE_ALL; i++)
        {
            if(!IS_RTL9602C_VALID_INTERRUPT_TYPE(i))
                continue;

            regValue |= (1<<intr_reg_ims_bitPos[i]);
        }
    }
    else
    {
        regValue = (1<<intr_reg_ims_bitPos[intr]);
    }

    if ((ret = reg_write(RTL9602C_INTR_IMSr, &regValue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_INTR), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_intr_ims_clear */


/* Function Name:
 *      dal_rtl9602c_intr_speedChangeStatus_get
 * Description:
 *      Get interrupt status of speed change.
 * Input:
 *      pPortMask          - pointer of return status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None.
 */
int32
dal_rtl9602c_intr_speedChangeStatus_get(rtk_portmask_t *pPortMask)
{
    int32   ret;
    uint32  regValue;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_INTR),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(intr_init);

    /* parameter check */
    RT_PARAM_CHK((NULL==pPortMask), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9602C_INTR_STATr, RTL9602C_INTR_STAT_PORT_CHANGEf, &regValue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_INTR), "");
        return ret;
    }

    RTK_PORTMASK_FROM_UINT_PORTMASK(pPortMask,&regValue);

    return RT_ERR_OK;
} /* end of dal_rtl9602c_intr_speedChangeStatus_get */


/* Function Name:
 *      dal_rtl9602c_intr_speedChangeStatus_clear
 * Description:
 *      Clear interrupt status of port speed change.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
int32
dal_rtl9602c_intr_speedChangeStatus_clear(void)
{
    int32   ret;
    uint32  regValue;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_INTR),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(intr_init);

    regValue = INTR_PORT_MASK;

    if ((ret = reg_field_write(RTL9602C_INTR_STATr,RTL9602C_INTR_STAT_PORT_CHANGEf,&regValue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_INTR), "");
        return ret;
    }


    return RT_ERR_OK;
} /* end of dal_rtl9602c_intr_speedChangeStatus_clear */


/* Function Name:
 *      dal_rtl9602c_intr_linkupStatus_get
 * Description:
 *      Get interrupt status of linkup.
 * Input:
 *      pPortMask          - pointer of return status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None.
 */
int32
dal_rtl9602c_intr_linkupStatus_get(rtk_portmask_t *pPortMask)
{
    int32   ret;
    uint32  regValue;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_INTR),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(intr_init);


    /* parameter check */
    RT_PARAM_CHK((NULL == pPortMask), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9602C_INTR_STATr, RTL9602C_INTR_STAT_PORT_LINKUPf, &regValue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_INTR), "");
        return ret;
    }
    RTK_PORTMASK_FROM_UINT_PORTMASK(pPortMask,&regValue);

    return RT_ERR_OK;
} /* end of dal_rtl9602c_intr_linkupStatus_get */


/* Function Name:
 *      dal_rtl9602c_intr_linkupStatus_clear
 * Description:
 *      Clear interrupt status of linkup.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
int32
dal_rtl9602c_intr_linkupStatus_clear(void)
{
    int32   ret;
    uint32  regValue;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_INTR),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(intr_init);

    regValue = INTR_PORT_MASK;

    if((ret=reg_field_write(RTL9602C_INTR_STATr,RTL9602C_INTR_STAT_PORT_LINKUPf,&regValue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_INTR),"");
        return ret;
    }


    return RT_ERR_OK;
} /* end of dal_rtl9602c_intr_linkupStatus_clear */


/* Function Name:
 *      dal_rtl9602c_intr_linkupPortStatus_clear
 * Description:
 *      Clear interrupt status of linkup.
 * Input:
 *      portMask - port mask to be cleared
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
int32
dal_rtl9602c_intr_linkupPortStatus_clear(rtk_portmask_t portMask)
{
    int32   ret;
    uint32  regValue;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_INTR),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(intr_init);

    regValue = INTR_PORT_MASK & (RTK_PORTMASK_TO_UINT_PORTMASK((&portMask)));

    if((ret=reg_field_write(RTL9602C_INTR_STATr,RTL9602C_INTR_STAT_PORT_LINKUPf,&regValue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_INTR),"");
        return ret;
    }


    return RT_ERR_OK;
} /* end of dal_rtl9602c_intr_linkupPortStatus_clear */


/* Function Name:
 *      dal_rtl9602c_intr_linkdownStatus_get
 * Description:
 *      Get interrupt status of linkdown.
 * Input:
 *      pPortMask          - pointer of return status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None.
 */
int32
dal_rtl9602c_intr_linkdownStatus_get(rtk_portmask_t *pPortMask)
{
    int32   ret;
    uint32  regValue;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_INTR),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(intr_init);


    /* parameter check */
    RT_PARAM_CHK((NULL == pPortMask), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9602C_INTR_STATr, RTL9602C_INTR_STAT_PORT_LINKDOWNf, &regValue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_INTR), "");
        return ret;
    }
    RTK_PORTMASK_FROM_UINT_PORTMASK(pPortMask,&regValue);

    return RT_ERR_OK;
} /* end of dal_rtl9602c_intr_linkdownStatus_get */


/* Function Name:
 *      dal_rtl9602c_intr_linkdownStatus_clear
 * Description:
 *      Clear interrupt status of linkdown.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
int32
dal_rtl9602c_intr_linkdownStatus_clear(void)
{
    int32   ret;
    uint32  regValue;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_INTR),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(intr_init);

    regValue = INTR_PORT_MASK;

    if((ret=reg_field_write(RTL9602C_INTR_STATr,RTL9602C_INTR_STAT_PORT_LINKDOWNf,&regValue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_INTR),"");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_intr_linkdownStatus_clear */


/* Function Name:
 *      dal_rtl9602c_intr_linkdownPortStatus_clear
 * Description:
 *      Clear interrupt status of linkdown.
 * Input:
 *      portMask - port mask to be cleared
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
int32
dal_rtl9602c_intr_linkdownPortStatus_clear(rtk_portmask_t portMask)
{
    int32   ret;
    uint32  regValue;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_INTR),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(intr_init);

    regValue = INTR_PORT_MASK & (RTK_PORTMASK_TO_UINT_PORTMASK((&portMask)));

    if((ret=reg_field_write(RTL9602C_INTR_STATr,RTL9602C_INTR_STAT_PORT_LINKDOWNf,&regValue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_INTR),"");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_intr_linkdownPortStatus_clear */


/* Function Name:
 *      dal_rtl9602c_intr_gphyStatus_get
 * Description:
 *      Get interrupt status of GPHY.
 * Input:
 *      pPortMask          - pointer of return status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None.
 */
int32
dal_rtl9602c_intr_gphyStatus_get(rtk_portmask_t *pPortMask)
{
    int32   ret;
    uint32  regValue;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_INTR),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(intr_init);


    /* parameter check */
    RT_PARAM_CHK((NULL == pPortMask), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9602C_INTR_STATr, RTL9602C_INTR_STAT_GPHYf, &regValue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_INTR), "");
        return ret;
    }
    RTK_PORTMASK_FROM_UINT_PORTMASK(pPortMask,&regValue);

    return RT_ERR_OK;
} /* end of dal_rtl9602c_intr_gphyStatus_get */


/* Function Name:
 *      dal_rtl9602c_intr_sts_gphy_clear
 * Description:
 *      Clear interrupt status of GPHY.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
int32
dal_rtl9602c_intr_gphyStatus_clear(void)
{
    int32   ret;
    uint32  regValue;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_INTR),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(intr_init);

    regValue = INTR_PORT_MASK;

    if((ret=reg_field_write(RTL9602C_INTR_STATr,RTL9602C_INTR_STAT_GPHYf,&regValue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_INTR),"");
        return ret;
    }


    return RT_ERR_OK;
} /* end of dal_rtl9602c_intr_gphyStatus_clear */


/* Function Name:
 *      dal_rtl9602c_intr_imr_restore
 * Description:
 *     set imr mask from input value
 * Input:
 *      imrValue: imr value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
int32
dal_rtl9602c_intr_imr_restore(uint32 imrValue)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_INTR),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(intr_init);

    /* check input value*/
    RT_PARAM_CHK((INTR_IMR_MASK < imrValue), RT_ERR_OUT_OF_RANGE);

    if((ret=reg_write(RTL9602C_INTR_IMRr,&imrValue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_INTR),"");
        return ret;
    }


    return RT_ERR_OK;

}

static uint32 _meterExceedIntCnt=0;
static uint32 _learnOverIntCnt=0;
static uint32 _speedChangeIntCnt=0;
static uint32 _specialCongestIntCnt=0;
static uint32 _thermalIntCnt=0;
static uint32 _debugIntCnt=0;
static uint32 _crashIntCnt=0;
static uint32 _intrGphyCnt = 0;
static rtk_enable_t _meterExceedInEnable=DISABLED;
static rtk_enable_t _learnOverIntEnable=DISABLED;
static rtk_enable_t _speedChangeIntEnable=DISABLED;
static rtk_enable_t _specialCongestIntEnable=DISABLED;
static rtk_enable_t _thermalIntEnable=DISABLED;
static rtk_enable_t _debugIntEnable=DISABLED;
static rtk_enable_t _ptp1ppsIntEnable=DISABLED;
static rtk_enable_t _crashIntEnable=DISABLED;
static rtk_enable_t _ppstodIntEnable=DISABLED;
static rtk_enable_t _gphyIntEnable = DISABLED;

void dal_rtl9602c_meter_exceed_isr_entry(void)
{
    int32  ret;
    
    _meterExceedIntCnt ++;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_INTR), "counter=%d",_meterExceedIntCnt);

    /* switch interrupt clear state */
    if((ret=dal_rtl9602c_intr_ims_clear(INTR_TYPE_METER_EXCEED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_INTR), "");
        return ;
    }

    return ; 
}

int32 dal_rtl9602c_meter_exceed_isr_counter_get(uint32 *counter)
{
    /* check Init status */
    RT_INIT_CHK(intr_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == counter), RT_ERR_NULL_POINTER);

    /* function body */
    *counter = _meterExceedIntCnt;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_INTR), "counter=%d",*counter);
    
    /*reset counter*/
    _meterExceedIntCnt=0;

    return RT_ERR_OK;
}

void dal_rtl9602c_learn_over_isr_entry(void)
{
    int32  ret;
    
    _learnOverIntCnt ++;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_INTR), "counter=%d",_learnOverIntCnt);
    
    /* switch interrupt clear state */
    if((ret=dal_rtl9602c_intr_ims_clear(INTR_TYPE_LEARN_OVER)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_INTR), "");
        return ;
    }

    return ; 
}

int32 dal_rtl9602c_learn_over_isr_counter_get(uint32 *counter)
{
    /* check Init status */
    RT_INIT_CHK(intr_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == counter), RT_ERR_NULL_POINTER);

    /* function body */
    *counter = _learnOverIntCnt;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_INTR), "counter=%d",*counter);

    /*reset counter*/
    _learnOverIntCnt=0;

    return RT_ERR_OK;
}

void dal_rtl9602c_speed_change_isr_entry(void)
{
    int32  ret;
    
    _speedChangeIntCnt ++;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_INTR), "counter=%d",_speedChangeIntCnt);
    
    /* switch interrupt clear state */
    if((ret=dal_rtl9602c_intr_ims_clear(INTR_TYPE_SPEED_CHANGE)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_INTR), "");
        return ;
    }

    return ; 
}

int32 dal_rtl9602c_speed_change_isr_counter_get(uint32 *counter)
{
    /* check Init status */
    RT_INIT_CHK(intr_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == counter), RT_ERR_NULL_POINTER);

    /* function body */
    *counter = _speedChangeIntCnt;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_INTR), "counter=%d",*counter);
    
    /*reset counter*/
    _speedChangeIntCnt=0;

    return RT_ERR_OK;
}

void dal_rtl9602c_special_congest_isr_entry(void)
{
    int32  ret;
    
    _specialCongestIntCnt ++;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_INTR), "counter=%d",_specialCongestIntCnt);
    
    /* switch interrupt clear state */
    if((ret=dal_rtl9602c_intr_ims_clear(INTR_TYPE_SPECIAL_CONGEST)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_INTR), "");
        return ;
    }

    return ; 
}

int32 dal_rtl9602c_special_congest_isr_counter_get(uint32 *counter)
{
    /* check Init status */
    RT_INIT_CHK(intr_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == counter), RT_ERR_NULL_POINTER);

    /* function body */
    *counter = _specialCongestIntCnt;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_INTR), "counter=%d",*counter);
    
    /*reset counter*/
    _specialCongestIntCnt=0;

    return RT_ERR_OK;
}

void
dal_rtl9602c_gphy_Isr_entry (
    void )
{
#if defined (CONFIG_SDK_KERNEL_LINUX)
    uint32 ret;
#endif

    if (_intrGphyCnt < 0xFFFFFFFF) {
        _intrGphyCnt++;
    }

#if defined (CONFIG_SDK_KERNEL_LINUX)
    GPHY_ISR_EXIT:

    if ((ret = dal_rtl9602c_intr_gphyStatus_clear()) != RT_ERR_OK) {
        RT_ERR(ret, (MOD_DAL | MOD_INTR), "");
    }
    /*clear gphy's ims*/
    ret = dal_rtl9602c_intr_ims_clear(INTR_TYPE_GPHY);
    if (ret != RT_ERR_OK) {
        RT_ERR(ret, (MOD_DAL | MOD_INTR), "");
    }
#endif
}

int32
dal_rtl9602c_gphy_isr_counter_get (
    uint32* counter )
{
    /* check Init status */
    RT_INIT_CHK(intr_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == counter), RT_ERR_NULL_POINTER);

    /* function body */
    *counter = _intrGphyCnt;

    /*reset counter*/
    _intrGphyCnt = 0;

    return (RT_ERR_OK);
}

void dal_rtl9602c_thermal_isr_entry(void)
{
    int32  ret;
    
    _thermalIntCnt ++;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_INTR), "counter=%d",_thermalIntCnt);

    /* switch interrupt clear state */
    if((ret=dal_rtl9602c_intr_ims_clear(INTR_TYPE_THERMAL)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_INTR), "");
        return ;
    }

    return ; 
}

int32 dal_rtl9602c_thermal_isr_counter_get(uint32 *counter)
{
    /* check Init status */
    RT_INIT_CHK(intr_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == counter), RT_ERR_NULL_POINTER);

    /* function body */
    *counter = _thermalIntCnt;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_INTR), "counter=%d",*counter);
    
    /*reset counter*/
    _thermalIntCnt=0;

    return RT_ERR_OK;
}

void dal_rtl9602c_debug_isr_entry(void)
{
    int32  ret;
    
    _debugIntCnt ++;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_INTR), "counter=%d",_debugIntCnt);
    
    /* switch interrupt clear state */
    if((ret=dal_rtl9602c_intr_ims_clear(INTR_TYPE_DEBUG)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_INTR), "");
        return ;
    }

    return ; 
}

int32 dal_rtl9602c_debug_isr_counter_get(uint32 *counter)
{
    /* check Init status */
    RT_INIT_CHK(intr_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == counter), RT_ERR_NULL_POINTER);

    /* function body */
    *counter = _debugIntCnt;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_INTR), "counter=%d",*counter);
    
    /*reset counter*/
    _debugIntCnt=0;

    return RT_ERR_OK;
}

void dal_rtl9602c_crash_isr_entry(void)
{
    int32  ret;
    
#if defined(CONFIG_SDK_KERNEL_LINUX)&&defined(CONFIG_RTK_SWITCH_ISSUE_LOG)
    switch_issue_log_isr();
#endif

    _crashIntCnt ++;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_INTR), "counter=%d",_crashIntCnt);
    
    /* switch interrupt clear state */
    if((ret=dal_rtl9602c_intr_ims_clear(INTR_TYPE_SWITCH_ERROR)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_INTR), "");
        return ;
    }

    return ; 
}

int32 dal_rtl9602c_crash_isr_counter_get(uint32 *counter)
{
    /* check Init status */
    RT_INIT_CHK(intr_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == counter), RT_ERR_NULL_POINTER);

    /* function body */
    *counter = _crashIntCnt;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_INTR), "counter=%d",*counter);

    /*reset counter*/
    _crashIntCnt=0;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl9602c_intr_isr_set
 * Description:
 *     set isr to register or deregister
 * Input:
 *      intr          - interrupt type
 *      enable      - interrupt status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
int32 
dal_rtl9602c_intr_isr_set(rtk_intr_type_t type, rtk_enable_t enable)
{
    int32  ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_INTR), "type=%d,enable=%d",type, enable);

    /* check Init status */
    RT_INIT_CHK(intr_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    /* function body */

    switch(type)
    {
        case INTR_TYPE_METER_EXCEED:
            if(enable == ENABLED)
            {
#if defined(CONFIG_SDK_KERNEL_LINUX)
                if((ret = rtk_irq_isr_register(INTR_TYPE_METER_EXCEED,dal_rtl9602c_meter_exceed_isr_entry)) != RT_ERR_OK)
                {
                    RT_ERR(ret,(MOD_DAL|MOD_INTR), "");
                    return ret;
                }
#endif
                _meterExceedInEnable=ENABLED;
            }
            else
            {
#if defined(CONFIG_SDK_KERNEL_LINUX)
                if((ret = rtk_irq_isr_unregister(INTR_TYPE_METER_EXCEED)) != RT_ERR_OK)
                {
                    RT_ERR(ret,(MOD_DAL|MOD_INTR), "");
                    return ret;
                }
#endif
                _meterExceedInEnable=DISABLED;
            }
            break;
        case INTR_TYPE_LEARN_OVER:
            if(enable == ENABLED)
            {
#if defined(CONFIG_SDK_KERNEL_LINUX)
                if((ret = rtk_irq_isr_register(INTR_TYPE_LEARN_OVER,dal_rtl9602c_learn_over_isr_entry)) != RT_ERR_OK)
                {
                    RT_ERR(ret,(MOD_DAL|MOD_INTR), "");
                    return ret;
                }
#endif
                _learnOverIntEnable=ENABLED;
            }
            else
            {
#if defined(CONFIG_SDK_KERNEL_LINUX)
                if((ret = rtk_irq_isr_unregister(INTR_TYPE_LEARN_OVER)) != RT_ERR_OK)
                {
                    RT_ERR(ret,(MOD_DAL|MOD_INTR), "");
                    return ret;
                }
#endif
                _learnOverIntEnable=DISABLED;
            }
            break;
        case INTR_TYPE_SPEED_CHANGE:
            if(enable == ENABLED)
            {
#if defined(CONFIG_SDK_KERNEL_LINUX)
                if((ret = rtk_irq_isr_register(INTR_TYPE_SPEED_CHANGE,dal_rtl9602c_speed_change_isr_entry)) != RT_ERR_OK)
                {
                    RT_ERR(ret,(MOD_DAL|MOD_INTR), "");
                    return ret;
                }
#endif
                _speedChangeIntEnable=ENABLED;
            }
            else
            {
#if defined(CONFIG_SDK_KERNEL_LINUX)
                if((ret = rtk_irq_isr_unregister(INTR_TYPE_SPEED_CHANGE)) != RT_ERR_OK)
                {
                    RT_ERR(ret,(MOD_DAL|MOD_INTR), "");
                    return ret;
                }
#endif
                _speedChangeIntEnable=DISABLED;
            }
            break;
        case INTR_TYPE_SPECIAL_CONGEST:
            if(enable == ENABLED)
            {
#if defined(CONFIG_SDK_KERNEL_LINUX)
                if((ret = rtk_irq_isr_register(INTR_TYPE_SPECIAL_CONGEST,dal_rtl9602c_special_congest_isr_entry)) != RT_ERR_OK)
                {
                    RT_ERR(ret,(MOD_DAL|MOD_INTR), "");
                    return ret;
                }
#endif
                _specialCongestIntEnable=ENABLED;
            }
            else
            {
#if defined(CONFIG_SDK_KERNEL_LINUX)
                if((ret = rtk_irq_isr_unregister(INTR_TYPE_SPECIAL_CONGEST)) != RT_ERR_OK)
                {
                    RT_ERR(ret,(MOD_DAL|MOD_INTR), "");
                    return ret;
                }
#endif
                _specialCongestIntEnable=DISABLED;
            }
            break;
        case INTR_TYPE_THERMAL:
            if(enable == ENABLED)
            {
#if defined(CONFIG_SDK_KERNEL_LINUX)
                if((ret = rtk_irq_isr_register(INTR_TYPE_THERMAL,dal_rtl9602c_thermal_isr_entry)) != RT_ERR_OK)
                {
                    RT_ERR(ret,(MOD_DAL|MOD_INTR), "");
                    return ret;
                }
#endif
                _thermalIntEnable=ENABLED;
            }
            else
            {
#if defined(CONFIG_SDK_KERNEL_LINUX)
                if((ret = rtk_irq_isr_unregister(INTR_TYPE_THERMAL)) != RT_ERR_OK)
                {
                    RT_ERR(ret,(MOD_DAL|MOD_INTR), "");
                    return ret;
                }
#endif
                _thermalIntEnable=DISABLED;
            }
            break;
        case INTR_TYPE_DEBUG:
            if(enable == ENABLED)
            {
#if defined(CONFIG_SDK_KERNEL_LINUX)
                if((ret = rtk_irq_isr_register(INTR_TYPE_DEBUG,dal_rtl9602c_debug_isr_entry)) != RT_ERR_OK)
                {
                    RT_ERR(ret,(MOD_DAL|MOD_INTR), "");
                    return ret;
                }
#endif
                _debugIntEnable=ENABLED;
            }
            else
            {
#if defined(CONFIG_SDK_KERNEL_LINUX)
                if((ret = rtk_irq_isr_unregister(INTR_TYPE_DEBUG)) != RT_ERR_OK)
                {
                    RT_ERR(ret,(MOD_DAL|MOD_INTR), "");
                    return ret;
                }
#endif
                _debugIntEnable=DISABLED;
            }
            break;
        case INTR_TYPE_PTP:
            if(enable == ENABLED)
            {
#if defined(CONFIG_SDK_KERNEL_LINUX)
                if((ret = rtk_irq_isr_register(INTR_TYPE_PTP,dal_rtl9602c_ptp1pps_isr_entry)) != RT_ERR_OK)
                {
                    RT_ERR(ret,(MOD_DAL|MOD_INTR), "");
                    return ret;
                }
#endif
                _ptp1ppsIntEnable=ENABLED;
            }
            else
            {
#if defined(CONFIG_SDK_KERNEL_LINUX)
                if((ret = rtk_irq_isr_unregister(INTR_TYPE_PTP)) != RT_ERR_OK)
                {
                    RT_ERR(ret,(MOD_DAL|MOD_INTR), "");
                    return ret;
                }
#endif
                _ptp1ppsIntEnable=DISABLED;
            }
            break;
        case INTR_TYPE_SWITCH_ERROR:
            if(enable == ENABLED)
            {
#if defined(CONFIG_SDK_KERNEL_LINUX)
                if((ret = rtk_irq_isr_register(INTR_TYPE_SWITCH_ERROR,dal_rtl9602c_crash_isr_entry)) != RT_ERR_OK)
                {
                    RT_ERR(ret,(MOD_DAL|MOD_INTR), "");
                    return ret;
                }
#endif
                _crashIntEnable=ENABLED;
            }
            else
            {
#if defined(CONFIG_SDK_KERNEL_LINUX)
                if((ret = rtk_irq_isr_unregister(INTR_TYPE_SWITCH_ERROR)) != RT_ERR_OK)
                {
                    RT_ERR(ret,(MOD_DAL|MOD_INTR), "");
                    return ret;
                }
#endif
                _crashIntEnable=DISABLED;
            }
            break;
        case INTR_TYPE_TOD:
            if(enable == ENABLED)
            {
#if defined(CONFIG_SDK_KERNEL_LINUX)
                if((ret = rtk_irq_isr_register(INTR_TYPE_TOD,dal_rtl9602c_ppstod_isr_entry)) != RT_ERR_OK)
                {
                    RT_ERR(ret,(MOD_DAL|MOD_INTR), "");
                    return ret;
                }
#endif
                _ppstodIntEnable=ENABLED;
            }
            else
            {
#if defined(CONFIG_SDK_KERNEL_LINUX)
                if((ret = rtk_irq_isr_unregister(INTR_TYPE_TOD)) != RT_ERR_OK)
                {
                    RT_ERR(ret,(MOD_DAL|MOD_INTR), "");
                    return ret;
                }
#endif
                _ppstodIntEnable=DISABLED;
            }
            break;
        case INTR_TYPE_GPHY:
            if (enable == ENABLED) {
#if defined(CONFIG_SDK_KERNEL_LINUX)
                if ((ret = rtk_irq_isr_register(INTR_TYPE_GPHY, dal_rtl9602c_gphy_Isr_entry)) != RT_ERR_OK) {
                    RT_ERR(ret, (MOD_DAL | MOD_INTR), "");
                    return (ret);
                }
#endif
                _gphyIntEnable = ENABLED;
            } else {
#if defined(CONFIG_SDK_KERNEL_LINUX)
                if ((ret = rtk_irq_isr_unregister(INTR_TYPE_GPHY)) != RT_ERR_OK) {
                    RT_ERR(ret, (MOD_DAL | MOD_INTR), "");
                    return (ret);
                }
#endif
                _gphyIntEnable = DISABLED;
            }
            break;
        default:
            ret = RT_ERR_CHIP_NOT_SUPPORTED;
            RT_ERR(ret,(MOD_DAL|MOD_INTR), "not support interrupt type=%d",type);
            return ret;
            break;
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9602c_intr_isr_set */

/* Function Name:
 *      dal_rtl9602c_intr_isr_get
 * Description:
 *     get isr register status
 * Input:
 *      intr          - interrupt type
 * Output:
 *      enable      - interrupt status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
int32 
dal_rtl9602c_intr_isr_get(rtk_intr_type_t type, rtk_enable_t *enable)
{
    int32  ret;
    
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_INTR), "type=%d",type);

    /* check Init status */
    RT_INIT_CHK(intr_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == enable), RT_ERR_NULL_POINTER);

    /* function body */
    switch(type)
    {
        case INTR_TYPE_METER_EXCEED:
            *enable=_meterExceedInEnable;
            break;
        case INTR_TYPE_LEARN_OVER:
            *enable=_learnOverIntEnable;
            break;
        case INTR_TYPE_SPEED_CHANGE:
            *enable=_speedChangeIntEnable;
            break;
        case INTR_TYPE_SPECIAL_CONGEST:
            *enable=_specialCongestIntEnable;
            break;
        case INTR_TYPE_THERMAL:
            *enable=_thermalIntEnable;
            break;
        case INTR_TYPE_DEBUG:
            *enable=_debugIntEnable;
            break;
        case INTR_TYPE_PTP:
            *enable=_ptp1ppsIntEnable;
            break;
        case INTR_TYPE_SWITCH_ERROR:
            *enable=_crashIntEnable;
            break;
        case INTR_TYPE_TOD:
            *enable=_ppstodIntEnable;
            break;
        case INTR_TYPE_GPHY:
            *enable = _gphyIntEnable;
            break;
        default:
            ret = RT_ERR_CHIP_NOT_SUPPORTED;
            RT_ERR(ret,(MOD_DAL|MOD_INTR), "not support interrupt type=%d",type);
            return ret;
            break;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9602c_intr_isr_get */

/* Function Name:
 *      dal_rtl9602c_intr_imr_restore
 * Description:
 *     set imr mask from input value
 * Input:
 *      intr          - interrupt type
 * Output:
 *      counter      - counter of interrupt
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
int32 
dal_rtl9602c_intr_isr_counter_dump(rtk_intr_type_t type, uint32 *counter)
{
    int32  ret;
    
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_INTR), "type=%d",type);

    /* check Init status */
    RT_INIT_CHK(intr_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == counter), RT_ERR_NULL_POINTER);

    /* function body */
    switch(type)
    {
        case INTR_TYPE_LINK_CHANGE:
#if defined(CONFIG_SDK_KERNEL_LINUX)
            if((ret = link_change_isr_counter_get(counter)) != RT_ERR_OK)
            {
                RT_ERR(ret,(MOD_DAL|MOD_INTR), "");
                return ret;
            }
#endif
            break;
        case INTR_TYPE_METER_EXCEED:
            if((ret = dal_rtl9602c_meter_exceed_isr_counter_get(counter)) != RT_ERR_OK)
            {
                RT_ERR(ret,(MOD_DAL|MOD_INTR), "");
                return ret;
            }
            break;
        case INTR_TYPE_LEARN_OVER:
            if((ret = dal_rtl9602c_learn_over_isr_counter_get(counter)) != RT_ERR_OK)
            {
                RT_ERR(ret,(MOD_DAL|MOD_INTR), "");
                return ret;
            }
            break;
        case INTR_TYPE_SPEED_CHANGE:
            if((ret = dal_rtl9602c_speed_change_isr_counter_get(counter)) != RT_ERR_OK)
            {
                RT_ERR(ret,(MOD_DAL|MOD_INTR), "");
                return ret;
            }
            break;
        case INTR_TYPE_SPECIAL_CONGEST:
            if((ret = dal_rtl9602c_special_congest_isr_counter_get(counter)) != RT_ERR_OK)
            {
                RT_ERR(ret,(MOD_DAL|MOD_INTR), "");
                return ret;
            }
            break;
        case INTR_TYPE_DYING_GASP:
#if defined(CONFIG_SDK_KERNEL_LINUX)
            if((ret = dying_gasp_isr_counter_get(counter)) != RT_ERR_OK)
            {
                RT_ERR(ret,(MOD_DAL|MOD_INTR), "");
                return ret;
            }
#endif
            break;
        case INTR_TYPE_THERMAL:
            if((ret = dal_rtl9602c_thermal_isr_counter_get(counter)) != RT_ERR_OK)
            {
                RT_ERR(ret,(MOD_DAL|MOD_INTR), "");
                return ret;
            }
            break;
        case INTR_TYPE_DEBUG:
            if((ret = dal_rtl9602c_debug_isr_counter_get(counter)) != RT_ERR_OK)
            {
            RT_ERR(ret,(MOD_DAL|MOD_INTR), "");
            return ret;
            }
            break;
        case INTR_TYPE_PTP:
            if((ret = dal_rtl9602c_ptp1pps_isr_counter_get(counter)) != RT_ERR_OK)
            {
            RT_ERR(ret,(MOD_DAL|MOD_INTR), "");
            return ret;
            }
            break;
        case INTR_TYPE_SWITCH_ERROR:
            if((ret = dal_rtl9602c_crash_isr_counter_get(counter)) != RT_ERR_OK)
            {
                RT_ERR(ret,(MOD_DAL|MOD_INTR), "");
                return ret;
            }
            break;
        case INTR_TYPE_TOD:
            if((ret = dal_rtl9602c_ppstod_isr_counter_get(counter)) != RT_ERR_OK)
            {
                RT_ERR(ret,(MOD_DAL|MOD_INTR), "");
                return ret;
            }
            break;
        case INTR_TYPE_GPHY:
            if ((ret = dal_rtl9602c_gphy_isr_counter_get (counter)) != RT_ERR_OK) {
                RT_ERR(ret, (MOD_DAL | MOD_INTR), "");
                return (ret);
            }
            break;
        default:
            ret = RT_ERR_CHIP_NOT_SUPPORTED;
            RT_ERR(ret,(MOD_DAL|MOD_INTR), "not support interrupt type=%d",type);
            return ret;
            break;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9602c_intr_isr_counter_dump */


