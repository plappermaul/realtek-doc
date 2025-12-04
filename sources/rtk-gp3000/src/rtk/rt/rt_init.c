/*
 * Copyright (C) 2021 Realtek Semiconductor Corp.
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
#include <rtk/rt/rt_init.h>
#include <rtk/rt/rt_switch.h>
#include <rtk/rt/rt_stat.h>
#include <rtk/rt/rt_sec.h>
#include <rtk/rt/rt_rate.h>
#include <rtk/rt/rt_qos.h>
#include <rtk/rt/rt_port.h>
#include <rtk/rt/rt_l2.h>
#include <rtk/rt/rt_intr.h>
#include <rtk/rt/rt_intr.h>
#include <rtk/rt/rt_i2c.h>
#include <rtk/rt/rt_mirror.h>
#include <rtk/rt/rt_trap.h>
#include <rtk/rt/rt_time.h>


#ifdef CONFIG_LUNA_G3_SERIES
/*no need*/
#else
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
#endif

#include <osal/print.h>

/* Function Name:
 *      rt_init_without_pon
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
rt_init_without_pon(void)
{
    int32 ret = RT_ERR_FAILED;

    ret = RT_ERR_OK;

#ifdef CONFIG_LUNA_G3_SERIES
    /*no need init rtk api*/
#else
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

    if ((ret = rtk_gpio_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_gpio_init Failed!!");
osal_printf("%s %d\n",__FUNCTION__,__LINE__);
        return ret;
    }

    if ((ret = rtk_time_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_time_init Failed!!");
osal_printf("%s %d\n",__FUNCTION__,__LINE__);
        return ret;
    }

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

#endif
 
    if ((ret = rt_switch_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rt_switch_init Failed!!");
osal_printf("%s %d %d\n",__FUNCTION__,__LINE__, ret);
        return ret;
    }

    if ((ret = rt_stat_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rt_stat_init Failed!!");
osal_printf("%s %d %d\n",__FUNCTION__,__LINE__, ret);
        return ret;
    }

    if ((ret = rt_sec_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rt_sec_init Failed!!");
osal_printf("%s %d %d\n",__FUNCTION__,__LINE__, ret);
        return ret;
    }

    if ((ret = rt_rate_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rt_rate_init Failed!!");
osal_printf("%s %d %d\n",__FUNCTION__,__LINE__, ret);
        return ret;
    }

    if ((ret = rt_qos_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rt_qos_init Failed!!");
osal_printf("%s %d %d\n",__FUNCTION__,__LINE__, ret);
        return ret;
    }

    if ((ret = rt_port_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rt_port_init Failed!!");
osal_printf("%s %d %d\n",__FUNCTION__,__LINE__, ret);
        return ret;
    }

    if ((ret = rt_l2_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rt_l2_init Failed!!");
osal_printf("%s %d %d\n",__FUNCTION__,__LINE__, ret);
        return ret;
    }

#ifdef CONFIG_LUNA_G3_SERIES
    if ((ret = rt_intr_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rt_intr_init Failed!!");
osal_printf("%s %d %d\n",__FUNCTION__,__LINE__, ret);
        return ret;
    }
#endif

    if ((ret = rt_cls_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rt_cls_init Failed!!");
osal_printf("%s %d %d\n",__FUNCTION__,__LINE__, ret);
        return ret;
    }

    if ((ret = rt_i2c_init(0)) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rt_i2c_init Failed!!");
osal_printf("%s %d %d\n",__FUNCTION__,__LINE__, ret);
        return ret;
    }

    if ((ret = rt_mirror_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rt_mirror_init Failed!!");
osal_printf("%s %d %d\n",__FUNCTION__,__LINE__, ret);
//        return ret;
    }

    if ((ret = rt_trap_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rt_trap_init Failed!!");
osal_printf("%s %d %d\n",__FUNCTION__,__LINE__, ret);
        return ret;
    }
    
    if ((ret = rt_time_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rt_time_init Failed!!");
osal_printf("%s %d %d\n",__FUNCTION__,__LINE__, ret);
        return ret;
    }

    if ((ret = rt_switch_mode_set(RT_SWITCH_MODE_HYBRID)) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rt_switch_mode_set Failed!!");
osal_printf("%s %d %d\n",__FUNCTION__,__LINE__, ret);
        return ret;
    }

    return ret;
} /* end of rt_init_without_pon */
