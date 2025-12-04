#include <osal/lib.h>
#include <osal/print.h>
#include <ioal/mem32.h>
#include <hal/common/halctrl.h>
#include <common/error.h>
#include <dal/dal_intr_test_case.h>
#include <rtk/intr.h>
#include <common/unittest_util.h>

int32 dal_intr_imr_test(uint32 caseNo)
{
    int32 ret;
    rtk_intr_type_t type;
    rtk_enable_t enable, tmp_enable;
    uint32 mask, allInterMask;

    switch(UNITTEST_UTIL_CHIP_TYPE)
    {
    case APOLLOMP_CHIP_ID:
        allInterMask = 0xefbf;
        break;
    case RTL9601B_CHIP_ID:
        allInterMask = 0xd2d9f;
        break;
    case RTL9602C_CHIP_ID:
        allInterMask = 0x7ffff;
        break;
    case RTL9607C_CHIP_ID:
        allInterMask = 0x1fffff;
        break;
    case RTL9603CVD_CHIP_ID:
        allInterMask = 0x1fffdf;
        break;
    default:
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*error input check*/
    /*out of range*/
    if(RT_ERR_OK == rtk_intr_imr_set(INTR_TYPE_END, DISABLED))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(RT_ERR_OK == rtk_intr_imr_set(INTR_TYPE_LINK_CHANGE, RTK_ENABLE_END))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(RT_ERR_OK == rtk_intr_imr_get(INTR_TYPE_END, &mask))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*NULL pointer*/
    if(RT_ERR_OK == rtk_intr_imr_get(INTR_TYPE_LINK_CHANGE,NULL))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Invalid types */
    for(type = 0; type < INTR_TYPE_END; type++)
    {
        if(IS_VALID_INTERRUPT_TYPE(type))
            continue;

        if(RT_ERR_OK == rtk_intr_imr_set(type, DISABLED))
        {
            osal_printf("\nrtk_intr_imr_set: type %d\n",type);
            return RT_ERR_FAILED;
        }
        if(RT_ERR_OK == rtk_intr_imr_get(type, &tmp_enable))
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }

    /* get/set test*/
    for(type = 0; type < INTR_TYPE_ALL; type++)
    {
        if(!IS_VALID_INTERRUPT_TYPE(type))
            continue;

        for(enable = 0; enable < RTK_ENABLE_END; enable++)
        {
            if(RT_ERR_OK != rtk_intr_imr_set(type, enable))
            {
                osal_printf("\n %s %d type %d, enable %d\n", __FUNCTION__, __LINE__, type, enable);
                return RT_ERR_FAILED;
            }
            if(RT_ERR_OK != rtk_intr_imr_get(type, &tmp_enable))
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
    if(RT_ERR_OK != rtk_intr_imr_set(INTR_TYPE_ALL, enable))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if(RT_ERR_OK != rtk_intr_imr_get(INTR_TYPE_ALL, &mask))
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
    if(RT_ERR_OK != rtk_intr_imr_set(INTR_TYPE_ALL, enable))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if(RT_ERR_OK != rtk_intr_imr_get(INTR_TYPE_ALL, &mask))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if(mask != allInterMask)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32 dal_intr_ims_test(uint32 caseNo)
{
    int32 ret;
    rtk_intr_type_t type;
    uint32 status;

    /*error input check*/
    /*out of range*/
    if(RT_ERR_OK == rtk_intr_ims_get(INTR_TYPE_END, &status))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(RT_ERR_OK == rtk_intr_ims_clear(INTR_TYPE_END))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*NULL pointer*/
    if(RT_ERR_OK == rtk_intr_ims_get(INTR_TYPE_LINK_CHANGE,NULL))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Invalid types */
    for(type = 0; type < INTR_TYPE_END; type++)
    {
        if(IS_VALID_INTERRUPT_TYPE(type))
            continue;

        if(RT_ERR_OK == rtk_intr_ims_clear(type))
        {
            osal_printf("\n %s %d type %d\n", __FUNCTION__, __LINE__, type);
            return RT_ERR_FAILED;
        }
        if(RT_ERR_OK == rtk_intr_ims_get(type, &status))
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }

    /* clear and get test */
    for(type = 0; type < INTR_TYPE_END; type++)
    {
        if(!IS_VALID_INTERRUPT_TYPE(type))
            continue;

        if(RT_ERR_OK != rtk_intr_ims_clear(type))
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if(RT_ERR_OK != rtk_intr_ims_get(type, &status))
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

int32 dal_intr_sts_test(uint32 caseNo)
{
    int32 ret;
    rtk_intr_type_t type;
    rtk_portmask_t portMask;

    /*error input check*/
    /*NULL pointer*/
    if(RT_ERR_OK == rtk_intr_speedChangeStatus_get(NULL))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if(RT_ERR_OK == rtk_intr_linkupStatus_get(NULL))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if(RT_ERR_OK == rtk_intr_linkdownStatus_get(NULL))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if(RT_ERR_OK == rtk_intr_gphyStatus_get(NULL))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* clear and get test */
    if(RT_ERR_OK != rtk_intr_speedChangeStatus_clear())
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if(RT_ERR_OK != rtk_intr_speedChangeStatus_get(&portMask))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if(RTK_PORTMASK_TO_UINT_PORTMASK((&portMask)) != 0x0)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(RT_ERR_OK != rtk_intr_linkupStatus_clear())
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if(RT_ERR_OK != rtk_intr_linkupStatus_get(&portMask))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if(RTK_PORTMASK_TO_UINT_PORTMASK((&portMask)) != 0x0)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(RT_ERR_OK != rtk_intr_linkdownStatus_clear())
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if(RT_ERR_OK != rtk_intr_linkdownStatus_get(&portMask))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if(RTK_PORTMASK_TO_UINT_PORTMASK((&portMask)) != 0x0)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(RT_ERR_OK != rtk_intr_gphyStatus_clear())
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if(RT_ERR_OK != rtk_intr_gphyStatus_get(&portMask))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if(RTK_PORTMASK_TO_UINT_PORTMASK((&portMask)) != 0x0)
    {
        osal_printf("\n %s %d: portMask %d\n",__FUNCTION__,__LINE__, RTK_PORTMASK_TO_UINT_PORTMASK((&portMask)));
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32 dal_intr_polarity_test(uint32 caseNo)
{
    int32 ret;
    rtk_intr_polarity_t polar, tmp_polar;

    /*error input check*/
    /*out of range*/
    if(RT_ERR_OK == rtk_intr_polarity_set(INTR_POLAR_END))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*NULL pointer*/
    if(RT_ERR_OK == rtk_intr_polarity_get(NULL))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* get/set test*/
    for(polar = 0; polar < INTR_POLAR_END; polar++)
    {
        if(RT_ERR_OK != rtk_intr_polarity_set(polar))
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if(RT_ERR_OK != rtk_intr_polarity_get(&tmp_polar))
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

