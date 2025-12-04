#include <osal/lib.h>
#include <osal/print.h>
#include <ioal/mem32.h>
#include <hal/common/halctrl.h>
#include <common/error.h>
#include <common/unittest_util.h>
#include <rtk/cpu.h>
#include <dal/dal_cpu_test_case.h>

/* Define symbol used for test input */


int32 dal_cpu_port_test(uint32 caseNo)
{
    int32 ret;
    rtk_port_t cpu_port, tmp_port;
    rtk_portmask_t port_mask, tmp_mask;
    rtk_cpu_tag_fmt_t mode, tmp_mode;
    rtk_enable_t state, tmp_state;

    rtk_cpu_init();

    /*error input check*/
    /*out of range*/
#if defined(CONFIG_SDK_APOLLO) || defined(CONFIG_SDK_APOLLOMP) || defined(CONFIG_SDK_RTL9601B)
    port_mask.bits[0] = 0xFF;
    if(RT_ERR_OK == rtk_cpu_awarePortMask_set(port_mask))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
#endif

#if defined(CONFIG_SDK_APOLLO) || defined(CONFIG_SDK_APOLLOMP)
    if(RT_ERR_OK == rtk_cpu_tagFormat_set(CPU_TAG_FMT_END))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
#endif

    if(RT_ERR_OK == rtk_cpu_trapInsertTag_set(RTK_ENABLE_END))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

#if defined(CONFIG_SDK_RTL9602C)
    if(RT_ERR_OK == rtk_cpu_tagAware_set(RTK_ENABLE_END))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
#endif

    /*NULL pointer*/
#if defined(CONFIG_SDK_APOLLO) || defined(CONFIG_SDK_APOLLOMP) || defined(CONFIG_SDK_RTL9601B)
    if(RT_ERR_OK == rtk_cpu_awarePortMask_get(NULL))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
#endif

#if defined(CONFIG_SDK_APOLLO) || defined(CONFIG_SDK_APOLLOMP)
    if(RT_ERR_OK == rtk_cpu_tagFormat_get(NULL))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
#endif

    if(RT_ERR_OK == rtk_cpu_trapInsertTag_get(NULL))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

#if defined(CONFIG_SDK_RTL9602C)
    if(RT_ERR_OK == rtk_cpu_tagAware_get(NULL))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
#endif


    /* get/set test*/
    /* aware port mask */
#if defined(CONFIG_SDK_APOLLO) || defined(CONFIG_SDK_APOLLOMP) || defined(CONFIG_SDK_RTL9601B)
    for(port_mask.bits[0] = 0; port_mask.bits[0] < 0x7F; port_mask.bits[0]++)
    {
        if(RT_ERR_OK != rtk_cpu_awarePortMask_set(port_mask))
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if(RT_ERR_OK != rtk_cpu_awarePortMask_get(&tmp_mask))
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if(port_mask.bits[0] != tmp_mask.bits[0])
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }
#endif

    /* tag format */
#if defined(CONFIG_SDK_APOLLO) || defined(CONFIG_SDK_APOLLOMP)
    for(mode = 0; mode <CPU_TAG_FMT_END; mode++)
    {
        if(RT_ERR_OK != rtk_cpu_tagFormat_set(mode))
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if(RT_ERR_OK != rtk_cpu_tagFormat_get(&tmp_mode))
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if(mode != tmp_mode)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }
#endif

    switch(UNITTEST_UTIL_CHIP_TYPE)
    {
        case RTL9603CVD_CHIP_ID:
	    /* trap insert tag */
	    for(state = 0; state < RTK_ENABLE_END; state++)
	    {
		if(RT_ERR_OK != rtk_cpu_trapInsertTag_set(state))
		{
		    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
		    return RT_ERR_FAILED;
		}
		if(RT_ERR_OK != rtk_cpu_trapInsertTag_get(&tmp_state))
		{
		    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
		    return RT_ERR_FAILED;
		}
		if(state != tmp_state)
		{
		    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
		    return RT_ERR_FAILED;
		}
	    }

	    /* tag aware */
	    for(state = 0; state < RTK_ENABLE_END; state++)
	    {
		if(RT_ERR_OK != rtk_cpu_tagAware_set(state))
		{
		    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
		    return RT_ERR_FAILED;
		}
		if(RT_ERR_OK != rtk_cpu_tagAware_get(&tmp_state))
		{
		    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
		    return RT_ERR_FAILED;
		}
		if(state != tmp_state)
		{
		    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
		    return RT_ERR_FAILED;
		}
	    }
            break;
        case RTL9607C_CHIP_ID:
	    /* trap insert tag */
	    for(state = 0; state < RTK_ENABLE_END; state++)
	    {
		if(RT_ERR_OK != rtk_cpu_trapInsertTagByPort_set(9,state))
		{
		    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
		    return RT_ERR_FAILED;
		}
		if(RT_ERR_OK != rtk_cpu_trapInsertTagByPort_get(9,&tmp_state))
		{
		    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
		    return RT_ERR_FAILED;
		}
		if(state != tmp_state)
		{
		    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
		    return RT_ERR_FAILED;
		}

		if(RT_ERR_OK != rtk_cpu_trapInsertTagByPort_set(10,state))
		{
		    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
		    return RT_ERR_FAILED;
		}
		if(RT_ERR_OK != rtk_cpu_trapInsertTagByPort_get(10,&tmp_state))
		{
		    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
		    return RT_ERR_FAILED;
		}
		if(state != tmp_state)
		{
		    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
		    return RT_ERR_FAILED;
		}

		if(RT_ERR_OK != rtk_cpu_trapInsertTagByPort_set(7,state))
		{
		    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
		    return RT_ERR_FAILED;
		}
		if(RT_ERR_OK != rtk_cpu_trapInsertTagByPort_get(7,&tmp_state))
		{
		    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
		    return RT_ERR_FAILED;
		}
		if(state != tmp_state)
		{
		    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
		    return RT_ERR_FAILED;
		}
	    }
	    /* tag aware */
	    for(state = 0; state < RTK_ENABLE_END; state++)
	    {
		if(RT_ERR_OK != rtk_cpu_tagAwareByPort_set(9,state))
		{
		    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
		    return RT_ERR_FAILED;
		}
		if(RT_ERR_OK != rtk_cpu_tagAwareByPort_get(9,&tmp_state))
		{
		    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
		    return RT_ERR_FAILED;
		}
		if(state != tmp_state)
		{
		    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
		    return RT_ERR_FAILED;
		}

		if(RT_ERR_OK != rtk_cpu_tagAwareByPort_set(10,state))
		{
		    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
		    return RT_ERR_FAILED;
		}
		if(RT_ERR_OK != rtk_cpu_tagAwareByPort_get(10,&tmp_state))
		{
		    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
		    return RT_ERR_FAILED;
		}
		if(state != tmp_state)
		{
		    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
		    return RT_ERR_FAILED;
		}

		if(RT_ERR_OK != rtk_cpu_tagAwareByPort_set(7,state))
		{
		    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
		    return RT_ERR_FAILED;
		}
		if(RT_ERR_OK != rtk_cpu_tagAwareByPort_get(7,&tmp_state))
		{
		    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
		    return RT_ERR_FAILED;
		}
		if(state != tmp_state)
		{
		    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
		    return RT_ERR_FAILED;
		}
	    }               
            break;
        default:
             break;
    }

#if defined(CONFIG_SDK_RTL9602C)
    /* trap insert tag */
    for(state = 0; state < RTK_ENABLE_END; state++)
    {
        if(RT_ERR_OK != rtk_cpu_trapInsertTag_set(state))
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if(RT_ERR_OK != rtk_cpu_trapInsertTag_get(&tmp_state))
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if(state != tmp_state)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }

    /* tag aware */
    for(state = 0; state < RTK_ENABLE_END; state++)
    {
        if(RT_ERR_OK != rtk_cpu_tagAware_set(state))
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if(RT_ERR_OK != rtk_cpu_tagAware_get(&tmp_state))
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if(state != tmp_state)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }
#endif

    return RT_ERR_OK;
}

