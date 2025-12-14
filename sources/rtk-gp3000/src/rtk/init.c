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
 * Purpose : Definition of Init API
 *
 * Feature : Initialize All Layers of RTK Module
 *
 */

/*
 * Include Files
 */
#include <ioal/mem32.h>
#include <common/rt_error.h>
#include <common/debug/rt_log.h>
#include <ioal/ioal_init.h>
#include <hal/common/halctrl.h>
#include <dal/dal_mgmt.h>
#include <rtk/init.h>
#include <rtk/l34.h>
#include <rtk/stp.h>
#include <rtk/svlan.h>
#include <rtk/acl.h>
#include <rtk/led.h>
#include <rtk/mirror.h>
#include <rtk/trunk.h>
#include <rtk/port.h>
#include <rtk/vlan.h>
#include <rtk/mirror.h>
#include <rtk/cpu.h>
#include <rtk/trap.h>
#include <rtk/irq.h>
#include <rtk/i2c.h>
#include <rtk/rldp.h>
#include <rtk/time.h>
#include <rtk/ponmac.h>
#include <rtk/epon.h>

#include <osal/print.h>

#include <rtk/l34lite.h>

#if 0
#include <rtk/default.h>
#include <rtk/dot1x.h>
#include <rtk/filter.h>
#include <rtk/flowctrl.h>
#include <rtk/l2.h>
#include <rtk/qos.h>
#include <rtk/rate.h>
#include <rtk/stat.h>
#include <rtk/switch.h>
#endif

#ifdef CONFIG_COMMON_RT_API
#include <rtk/rt/rt_init.h>
#endif

#include <osal/sem.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
osal_mutex_t    rtk_api_mutex;
/*
 * Function Declaration
 */
int32 _rtk_soc_ready(void);


int32
rtk_core_init(void)
{
    int32 ret = RT_ERR_FAILED;
    RT_DBG(LOG_EVENT, MOD_INIT, "rtk_core_init Start!!");

    ioal_init();

    /* Initialize the hal layer */
    if ((ret = hal_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "hal_init Failed!!");
        return ret;
    }
    RT_DBG(LOG_EVENT, MOD_INIT, "hal_init Completed!!");

    if ((ret = dal_mgmt_initDevice()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "dal_mgmt_initDevice Failed!!");
        return ret;
    }

    return ret;
}


/* Function Name:
 *      rtk_init
 * Description:
 *      Initialize the driver, hook related driver
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      INIT must be initialized before using all of APIs in each modules
 */
int32
rtk_all_module_init(void)
{
    int32 ret = RT_ERR_FAILED;

    if ((ret = rtk_switch_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_switch_init Failed!!");
        return ret;
    }

    if ((ret = rtk_svlan_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_svlan_init Failed!!");
        return ret;
    }
    if ((ret = rtk_stp_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_stp_init Failed!!");
        return ret;
    }
    if ((ret = rtk_oam_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_oam_init Failed!!");
        return ret;
    }
    if ((ret = rtk_acl_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_acl_init Failed!!");
        return ret;
    }
    if ((ret = rtk_qos_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_qos_init Failed!!");
        return ret;
    }
    if ((ret = rtk_sec_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_sec_init Failed!!");
        return ret;
    }
    if ((ret = rtk_rate_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_rate_init Failed!!");
        return ret;
    }
#if defined(CONFIG_CLASSFICATION_FEATURE)
    if ((ret = rtk_classify_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_classify_init Failed!!");
        return ret;
    }
#endif
    if ((ret = rtk_stat_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_stat_init Failed!!");
        return ret;
    }
    if ((ret = rtk_trunk_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_trunk_init Failed!!");
        return ret;
    }
    if ((ret = rtk_l2_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_l2_init Failed!!");
        return ret;
    }
    if ((ret = rtk_vlan_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_vlan_init Failed!!");
        return ret;
    }
    if ((ret = rtk_port_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_port_init Failed!!");
        return ret;
    }
    if ((ret = rtk_mirror_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_mirror_init Failed!!");
        return ret;
    }
    if ((ret = rtk_cpu_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_mirror_init Failed!!");
        return ret;
    }
    if ((ret = rtk_rldp_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_rldp_init Failed!!");
        return ret;
    }
    if ((ret = rtk_trap_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_trap_init Failed!!");
        return ret;
    }
#if 1 /*mark for do not need to initial gpio in the first time*/
    if ((ret = rtk_gpio_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_gpio_init Failed!!");
        return ret;
    }
#endif
    if ((ret = rtk_time_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_time_init Failed!!");
        return ret;
    }

    if ((ret = rtk_ponmac_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_ponmac_init Failed!!");
        return ret;
    }

#ifdef CONFIG_RTK_L34_DEMO
    {
    	if((ret=rtk_l34_lite_init())!=RT_ERR_OK)
    	{
    		 RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_ponmac_init Failed!!");
        	return ret;
    	}
    }
#endif


    return ret;
}


static void dying_gasp_handler(void)
{
    int ret;
    osal_printf("\n\rDyingGasp Interrupt!!!\n\r");

    if((ret = rtk_switch_chip_reset())!=RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "switch reset failed!!");
        return;
    }
    osal_printf("\n\rDyingGasp isr done!!!\n\r");
}



int32
rtk_system_init(void)
{
    int32 ret = RT_ERR_OK;

    /* create RTK API lock mutex */
    rtk_api_mutex = osal_sem_mutex_create();

#if defined(CONFIG_LUNA_G3_SERIES)
    if ((ret = rtk_core_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_core_init Failed!!");
        return ret;
    }
#else
#if defined(CONFIG_SDK_KERNEL_LINUX)
    if ((ret = rtk_core_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_core_init Failed!!");
        return ret;
    }

    /*switch soc init*/
    /*SoC ready*/
    if ((ret = _rtk_soc_ready()) != RT_ERR_OK)
	{
		 RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "_rtk_soc_ready Failed!!");
        return ret;
	}

#if defined(CONFIG_SOC_DEPEND_FEATURE)
	if ((ret = rtk_intr_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_intr_init Failed!!");
        return ret;
    }
	if ((ret = rtk_irq_init()) != RT_ERR_OK)
	{
		 RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_switch_irq_init Failed!!");
       	 return ret;
	}
#if 0
    /* register DyingGasp interrupt */
	if((ret = rtk_irq_isr_register(INTR_TYPE_DYING_GASP, dying_gasp_handler)) != RT_ERR_OK)
	{
		 RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_irq_isr_register for DyingGasp Failed!!");
       	 return ret;
	}

    if((ret = rtk_intr_imr_set(INTR_TYPE_DYING_GASP, ENABLED)) != RT_ERR_OK)
	{
		 RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_intr_imr_set for DyingGasp Failed!!");
       	 return ret;
	}
#endif
#endif /*CONFIG_SOC_DEPEND_FEATURE*/
#endif /*CONFIG_SDK_KERNEL_LINUX*/

	if ((ret = rtk_init()) != RT_ERR_OK)
	{
		RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_init Failed!!");
        return ret;
	}
#endif
    return ret;

}


int32 _rtk_soc_ready(void)
{
    int32 ret = RT_ERR_OK;
    uint32   wData;

    /*SoC ready*/
    {
        if ((ret = ioal_socMem32_read(0xb8000044, &wData)) != RT_ERR_OK)
        {
            RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "get soc ready Failed!!");
            return ret;
        }
        wData = wData | 1;
        if ((ret = ioal_socMem32_write(0xb8000044, wData)) != RT_ERR_OK)
        {
            RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "set soc ready Failed!!");
            return ret;
        }
    }
    return ret;
}


/* Function Name:
 *      rtk_init
 * Description:
 *      Initialize the driver, hook related driver
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      INIT must be initialized before using all of APIs in each modules
 */
int32
rtk_init(void)
{
    int32 ret = RT_ERR_OK;

#if defined(RTL_RLX_IO)
   /*switch soc init*/
	if ((ret = _rtk_soc_ready()) != RT_ERR_OK)
	{
		 RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "_rtk_soc_ready Failed!!");
        return ret;
	}
#endif

#if !defined(CONFIG_PURE_HW_INIT)
	if ((ret = rtk_all_module_init()) != RT_ERR_OK)
	{
		 RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_all_module_init Failed!!");
        return ret;
	}
#endif
    return ret;
} /* end of rtk_init */

/* Function Name:
 *      rtk_deinit
 * Description:
 *      De-Initialize the driver, release irq
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      INIT must be initialized before using all of APIs in each modules
 */
int32
rtk_deinit(void)
{
    int32 ret = RT_ERR_FAILED;

    RT_DBG(LOG_EVENT, MOD_INIT, "rtk_deinit Start!!");

#if defined(CONFIG_SDK_KERNEL_LINUX)
#if defined(CONFIG_SOC_DEPEND_FEATURE)
    /* IRQ deinit */
    rtk_irq_exit();

#endif
#endif

    return ret;

} /* end of rtk_init */

/* Function Name:
 *      rtk_init_without_pon
 * Description:
 *      Initialize the driver, without pon related driver
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      INIT must be initialized before using all of APIs in each modules
 */
int32
rtk_init_without_pon(void)
{
    int32 ret = RT_ERR_FAILED;

#if 0 /* remove without pon */
    if ((ret = rtk_switch_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_switch_init Failed!!");
        return ret;
    }
#endif

    if ((ret = rtk_svlan_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_svlan_init Failed!!");
osal_printf("%s %d\n",__FUNCTION__,__LINE__);
        return ret;
    }
    if ((ret = rtk_stp_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_stp_init Failed!!");
osal_printf("%s %d\n",__FUNCTION__,__LINE__);
        return ret;
    }
#if 0 /* remove without pon */
    if ((ret = rtk_oam_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_oam_init Failed!!");
        return ret;
    }
#endif
    if ((ret = rtk_acl_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_acl_init Failed!!");
osal_printf("%s %d\n",__FUNCTION__,__LINE__);
        return ret;
    }
    if ((ret = rtk_qos_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_qos_init Failed!!");
osal_printf("%s %d\n",__FUNCTION__,__LINE__);
        return ret;
    }
    if ((ret = rtk_sec_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_sec_init Failed!!");
osal_printf("%s %d\n",__FUNCTION__,__LINE__);
        return ret;
    }
    if ((ret = rtk_rate_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_rate_init Failed!!");
osal_printf("%s %d\n",__FUNCTION__,__LINE__);
        return ret;
    }
#if defined(CONFIG_CLASSFICATION_FEATURE)
    if ((ret = rtk_classify_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_classify_init Failed!!");
osal_printf("%s %d\n",__FUNCTION__,__LINE__);
        return ret;
    }
#endif
    if ((ret = rtk_stat_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_stat_init Failed!!");
osal_printf("%s %d\n",__FUNCTION__,__LINE__);
        return ret;
    }
    if ((ret = rtk_trunk_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_trunk_init Failed!!");
osal_printf("%s %d\n",__FUNCTION__,__LINE__);
        return ret;
    }
    if ((ret = rtk_l2_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_l2_init Failed!!");
osal_printf("%s %d\n",__FUNCTION__,__LINE__);
        return ret;
    }
    if ((ret = rtk_vlan_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_vlan_init Failed!!");
osal_printf("%s %d\n",__FUNCTION__,__LINE__);
        return ret;
    }
#if 0 /* remove without pon */
    if ((ret = rtk_port_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_port_init Failed!!");
        return ret;
    }
#endif
    if ((ret = rtk_mirror_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_mirror_init Failed!!");
osal_printf("%s %d\n",__FUNCTION__,__LINE__);
        return ret;
    }
    if ((ret = rtk_cpu_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_mirror_init Failed!!");
osal_printf("%s %d\n",__FUNCTION__,__LINE__);
        return ret;
    }
    if ((ret = rtk_rldp_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_rldp_init Failed!!");
osal_printf("%s %d\n",__FUNCTION__,__LINE__);
        return ret;
    }
    if ((ret = rtk_trap_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_trap_init Failed!!");
osal_printf("%s %d\n",__FUNCTION__,__LINE__);
        return ret;
    }
#if 1 /*mark for do not need to initial gpio in the first time*/
    if ((ret = rtk_gpio_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_gpio_init Failed!!");
osal_printf("%s %d\n",__FUNCTION__,__LINE__);
        return ret;
    }
#endif
    if ((ret = rtk_time_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_time_init Failed!!");
osal_printf("%s %d\n",__FUNCTION__,__LINE__);
        return ret;
    }

#if 0 /* remove without pon */
    if ((ret = rtk_ponmac_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_ponmac_init Failed!!");
        return ret;
    }
#endif

#ifdef CONFIG_RTK_L34_DEMO
    {
    	if((ret=rtk_l34_lite_init())!=RT_ERR_OK)
    	{
    		 RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_ponmac_init Failed!!");
osal_printf("%s %d\n",__FUNCTION__,__LINE__);
        	return ret;
    	}
    }
#endif

#if defined(CONFIG_COMMON_RT_API) && defined(CONFIG_SDK_KERNEL_LINUX)
    if ((ret = rt_init_without_pon()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rt_init_without_pon Failed!!");
osal_printf("%s %d\n",__FUNCTION__,__LINE__);
        return ret;
    }
#endif

    return ret;
} /* end of rtk_init */

#if defined(CONFIG_LUNA_G3_SERIES) && defined(CONFIG_SDK_KERNEL_LINUX)
uint32 ca_rtk_debug = 0;
EXPORT_SYMBOL(ca_rtk_debug);
#endif 