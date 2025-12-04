#include <osal/lib.h>
#include <common/debug/rt_log.h>
#include <dal/apollo/raw/apollo_raw_intr.h>
#include <dal/raw/apollo/dal_raw_intr_test_case.h>
#include <rtk/intr.h>

/* Define symbol used for test input */


int32 dal_raw_intr_imr_test(uint32 caseNo)
{
    int32 ret;
    rtk_intr_type_t type;
    rtk_enable_t enable, tmp_enable;
    uint32 mask;

    /*error input check*/
    /*out of range*/
    if(RT_ERR_OK == apollo_raw_intr_imr_set(INTR_TYPE_END, DISABLED))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(RT_ERR_OK == apollo_raw_intr_imr_set(INTR_TYPE_LINK_CHANGE, RTK_ENABLE_END))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(RT_ERR_OK == apollo_raw_intr_imr_get(INTR_TYPE_END, &mask))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*NULL pointer*/
    if(RT_ERR_OK == apollo_raw_intr_imr_get(INTR_TYPE_LINK_CHANGE,NULL))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }


    /* get/set test*/
    for(type = 0; type < INTR_TYPE_ALL; type++)
    {
        for(enable = 0; enable < RTK_ENABLE_END; enable++)
        {
            if(RT_ERR_OK != apollo_raw_intr_imr_set(type, enable))
            {
                osal_printf("\napollo_raw_intr_imr_set: type %d, enable %d\n",type, enable);
                return RT_ERR_FAILED;
            }
            if(RT_ERR_OK != apollo_raw_intr_imr_get(type, &tmp_enable))
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            if(enable != tmp_enable)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
        }
    }

    /* 'all' case */
    enable = DISABLED;
    if(RT_ERR_OK != apollo_raw_intr_imr_set(INTR_TYPE_ALL, enable))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if(RT_ERR_OK != apollo_raw_intr_imr_get(INTR_TYPE_ALL, &mask))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if(mask != 0x0)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    enable = ENABLED;
    if(RT_ERR_OK != apollo_raw_intr_imr_set(INTR_TYPE_ALL, enable))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if(RT_ERR_OK != apollo_raw_intr_imr_get(INTR_TYPE_ALL, &mask))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if(mask != 0xFFFF)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32 dal_raw_intr_ims_test(uint32 caseNo)
{
    int32 ret;
    rtk_intr_type_t type;
    uint32 status;

    /*error input check*/
    /*out of range*/
    if(RT_ERR_OK == apollo_raw_intr_ims_get(INTR_TYPE_END, &status))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(RT_ERR_OK == apollo_raw_intr_ims_clear(INTR_TYPE_END))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }


    /*NULL pointer*/
    if(RT_ERR_OK == apollo_raw_intr_ims_get(INTR_TYPE_LINK_CHANGE,NULL))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* clear and get test */
    for(type = 0; type < INTR_TYPE_END; type++)
    {
        if(RT_ERR_OK != apollo_raw_intr_ims_clear(type))
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if(RT_ERR_OK != apollo_raw_intr_ims_get(type, &status))
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if(status != 0x0)
        {
            osal_printf("\n %s %d: type %d, status %d\n",__FUNCTION__,__LINE__,type, status);
            return RT_ERR_FAILED;
        }
    }

    return RT_ERR_OK;
}

int32 dal_raw_intr_sts_test(uint32 caseNo)
{
    int32 ret;
    rtk_intr_type_t type;
    uint32 status;

    /*error input check*/
    /*NULL pointer*/
    if(RT_ERR_OK == apollo_raw_intr_sts_speed_change_get(NULL))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if(RT_ERR_OK == apollo_raw_intr_sts_linkup_get(NULL))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if(RT_ERR_OK == apollo_raw_intr_sts_linkdown_get(NULL))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if(RT_ERR_OK == apollo_raw_intr_sts_gphy_get(NULL))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* clear and get test */
    if(RT_ERR_OK != apollo_raw_intr_sts_speed_change_clear())
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if(RT_ERR_OK != apollo_raw_intr_sts_speed_change_get(&status))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if(status != 0x0)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(RT_ERR_OK != apollo_raw_intr_sts_linkup_clear())
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if(RT_ERR_OK != apollo_raw_intr_sts_linkup_get(&status))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if(status != 0x0)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(RT_ERR_OK != apollo_raw_intr_sts_linkdown_clear())
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if(RT_ERR_OK != apollo_raw_intr_sts_linkdown_get(&status))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if(status != 0x0)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(RT_ERR_OK != apollo_raw_intr_sts_gphy_clear())
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if(RT_ERR_OK != apollo_raw_intr_sts_gphy_get(&status))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if(status != 0x0)
    {
        osal_printf("\n %s %d: status %d\n",__FUNCTION__,__LINE__, status);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32 dal_raw_intr_polarity_test(uint32 caseNo)
{
    int32 ret;
    rtk_intr_polarity_t polar, tmp_polar;

    /*error input check*/
    /*out of range*/
    if(RT_ERR_OK == apollo_raw_intr_polarity_set(INTR_POLAR_END))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*NULL pointer*/
    if(RT_ERR_OK == apollo_raw_intr_polarity_get(NULL))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* get/set test*/
    for(polar = 0; polar < INTR_POLAR_END; polar++)
    {
        if(RT_ERR_OK != apollo_raw_intr_polarity_set(polar))
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if(RT_ERR_OK != apollo_raw_intr_polarity_get(&tmp_polar))
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if(polar != tmp_polar)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }

    return RT_ERR_OK;
}

