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
 *
 *
 * $Revision$
 * $Date$
 *
 * Purpose : GMac Driver FSM Processor
 *
 * Feature : GMac Driver FSM Processor
 *
 */

#include <module/gpon/gpon_defs.h>
#include <module/gpon/gpon_fsm.h>
#include <module/gpon/gpon_res.h>
#include <module/gpon/gpon_debug.h>
#include <rtk/ponmac.h>
#include <hal/common/halctrl.h>

static int32 gLosClrO6 = 0;

typedef void (*GMac_fsm_event_handler_t)(gpon_dev_obj_t *obj);

GMac_fsm_event_handler_t g_gmac_fsm_eventhandler[GPON_STATE_O7][GPON_FSM_EVENT_MAX];

static void gmac_fsm_o1_los_clear(gpon_dev_obj_t *obj)
{
    int32 ret;
    rtk_port_macAbility_t mac_ability;

    obj->status = GPON_STATE_O2;

    if((ret = rtk_gpon_onuState_set(obj->status))!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_GPON), "");
        return ;
    }
#if 0 /* do re lock tx PLL in ponmac_mode_set */
    /* re lock tx PLL. MP2099 pull RX_SD by power on in high optic (> -12dbm), it casuse tx Pll lock wrong clock rate */
    if((ret = rtk_ponmac_txPll_relock())!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_GPON), "");
        return ;
    }
#endif
#if 1 /* keep pon port link down until pon clock is steady */
    if((ret = rtk_port_macForceAbility_get(HAL_GET_PON_PORT(), &mac_ability)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_GPON), "");
        return ;
    }
    if(PORT_LINKDOWN == mac_ability.linkStatus)
    {
        mac_ability.linkStatus = PORT_LINKUP;
        if((ret = rtk_port_macForceAbility_set(HAL_GET_PON_PORT(), mac_ability)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_GPON), "");
            return ;
        }
    }
#endif
#if 1 /* set the AES key to 0 */
    osal_memset(&obj->aes_key,0x0,sizeof(rtk_gpon_aes_key_t));
    obj->aes_key_switch_time = 0x0;
    if((ret = rtk_gpon_aesKeyWordActive_set(obj->aes_key.key)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_GPON), "");
        return ;
    }
    if((ret = rtk_gpon_aesKeySwitch_set(obj->aes_key_switch_time)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_GPON), "");
        return ;
    }
#endif
}

static void gmac_fsm_o2_upstream(gpon_dev_obj_t *obj)
{
    int32 ret;
    if(obj->timer)
    {
        GPON_OS_Log(GPON_LOG_LEVEL_DEBUG,"stop timer %p [baseaddr:%p]",obj->timer,obj->base_addr);
        GPON_OS_StopTimer(obj->timer);
        obj->timer = 0;
    }
    obj->timer = GPON_OS_StartTimer(obj->parameter.onu.to1_timer,FALSE,0,gpon_fsm_to1_expire);
    GPON_OS_Log(GPON_LOG_LEVEL_DEBUG,"start TO1 timer %p [baseaddr:%p]",obj->timer,obj->base_addr);
    obj->status = GPON_STATE_O3;

	obj->eqd = 0;

    if((ret = rtk_gpon_onuState_set(obj->status))!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_GPON), "");
        return;
    }
}

static void gmac_fsm_o2_los_detect(gpon_dev_obj_t *obj)
{
    int32 ret;
    obj->status = GPON_STATE_O1;

    if((ret = rtk_gpon_onuState_set(obj->status))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_GPON), "%s, return failed ret value = %x",__FUNCTION__,ret);
        return;
    }
}

static void gmac_fsm_o2_disable(gpon_dev_obj_t *obj)
{
    int32 ret;
    /* shut down optical transmit power immediately. */
    //TBD: use gpio to control laser off

    obj->status = GPON_STATE_O7;

    if((ret = rtk_gpon_onuState_set(obj->status))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_GPON), "%s, return failed ret value = %x",__FUNCTION__,ret);
        return;
    }

    /* set TX disable when OTL set ONT to disable */
    if((ret = gpon_dev_rogueOntDisTx_set(obj, DISABLED))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_GPON), "%s, return failed ret value = %x",__FUNCTION__,ret);
        return;
    }
}

static void gmac_fsm_o3_onuid(gpon_dev_obj_t *obj)
{
    int32 ret;
    obj->status = GPON_STATE_O4;

    if((ret = rtk_gpon_onuState_set(obj->status))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_GPON), "%s, return failed ret value = %x",__FUNCTION__,ret);
        return;
    }
}

static void gmac_fsm_o3_to1_expire(gpon_dev_obj_t *obj)
{
    int32 ret;
    obj->status = GPON_STATE_O2;
    if((ret = rtk_gpon_onuState_set(obj->status))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_GPON), "%s, return failed ret value = %x",__FUNCTION__,ret);
        return;
    }
}

static void gmac_fsm_o3_los_detect(gpon_dev_obj_t *obj)
{
    int32 ret;
    if(obj->timer)
    {
        GPON_OS_Log(GPON_LOG_LEVEL_DEBUG,"stop timer %p [baseaddr:%p]",obj->timer,obj->base_addr);
        GPON_OS_StopTimer(obj->timer);
        obj->timer = 0;
    }
    obj->status = GPON_STATE_O1;

    if((ret = rtk_gpon_onuState_set(obj->status))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_GPON), "%s, return failed ret value = %x",__FUNCTION__,ret);
        return;
    }

    if((ret = gpon_dev_onuid_set(obj,GPON_DEV_DEFAULT_ONU_ID))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_GPON), "%s, return failed ret value = %x",__FUNCTION__,ret);
        return;
    }
}

static void gmac_fsm_o3_disable(gpon_dev_obj_t *obj)
{
    int32 ret;
    /* shut down optical transmit power immediately. */
    //TBD: use gpio to control laser off

    if(obj->timer)
    {
        GPON_OS_Log(GPON_LOG_LEVEL_DEBUG,"stop timer %p [baseaddr:%p]",obj->timer,obj->base_addr);
        GPON_OS_StopTimer(obj->timer);
        obj->timer = 0;
    }
    obj->status = GPON_STATE_O7;

    if((ret = rtk_gpon_onuState_set(obj->status))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_GPON), "%s, return failed ret value = %x",__FUNCTION__,ret);
        return;
    }

    /* set TX disable when OTL set ONT to disable */
    if((ret = gpon_dev_rogueOntDisTx_set(obj, DISABLED))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_GPON), "%s, return failed ret value = %x",__FUNCTION__,ret);
        return;
    }
}

static void gmac_fsm_o4_eqd(gpon_dev_obj_t *obj)
{
    int32 ret;
    if(obj->timer)
    {
        GPON_OS_Log(GPON_LOG_LEVEL_DEBUG,"stop timer %p [baseaddr:%p]",obj->timer,obj->base_addr);
        GPON_OS_StopTimer(obj->timer);
        obj->timer = 0;
    }
    obj->status = GPON_STATE_O5;

    if((ret = gpon_dev_burstHead_ranged_set(obj))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_GPON), "%s, return failed ret value = %x",__FUNCTION__,ret);
        return;
    }
    if((ret = rtk_gpon_usPloamBuf_flush())!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_GPON), "%s, return failed ret value = %x",__FUNCTION__,ret);
        return;
    }
    if((ret = rtk_gpon_onuState_set(obj->status))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_GPON), "%s, return failed ret value = %x",__FUNCTION__,ret);
        return;
    }
#if 0 /* this function is for power saving, it is not verified yet. */
    if((ret = rtk_gpon_autoDisTx_set(ENABLED))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_GPON), "%s, return failed ret value = %x",__FUNCTION__,ret);
        return;
    }
#endif
    /*set pon port to mac fore mode*/
    if((ret = gpon_dev_portMacForceMode_set(obj, PORT_LINKUP))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_GPON), "%s, return failed ret value = %x",__FUNCTION__,ret);
        return;
    }
}

static void gmac_fsm_o4_to1_expire(gpon_dev_obj_t *obj)
{
    int32 ret;
    obj->status = GPON_STATE_O2;

    if((ret = rtk_gpon_onuState_set(obj->status))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_GPON), "%s, return failed ret value = %x",__FUNCTION__,ret);
        return;
    }
    if((ret = gpon_dev_onuid_set(obj,GPON_DEV_DEFAULT_ONU_ID))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_GPON), "%s, return failed ret value = %x",__FUNCTION__,ret);
        return;
    }
}

static void gmac_fsm_o4_deactivate(gpon_dev_obj_t *obj)
{
    int32 ret;
    if(obj->timer)
    {
        GPON_OS_Log(GPON_LOG_LEVEL_DEBUG,"stop timer %p [baseaddr:%p]",obj->timer,obj->base_addr);
        GPON_OS_StopTimer(obj->timer);
        obj->timer = 0;
    }
    obj->status = GPON_STATE_O2;

    if((ret = rtk_gpon_onuState_set(obj->status))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_GPON), "%s, return failed ret value = %x",__FUNCTION__,ret);
        return;
    }
    if((ret = gpon_dev_onuid_set(obj,GPON_DEV_DEFAULT_ONU_ID))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_GPON), "%s, return failed ret value = %x",__FUNCTION__,ret);
        return;
    }
}

static void gmac_fsm_o4_los_detect(gpon_dev_obj_t *obj)
{
    int32 ret;
    if(obj->timer)
    {
        GPON_OS_Log(GPON_LOG_LEVEL_DEBUG,"stop timer %p [baseaddr:%p]",obj->timer,obj->base_addr);
        GPON_OS_StopTimer(obj->timer);
        obj->timer = 0;
    }
    obj->status = GPON_STATE_O1;

    if((ret = rtk_gpon_onuState_set(obj->status))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_GPON), "%s, return failed ret value = %x",__FUNCTION__,ret);
        return;
    }
    if((ret = gpon_dev_onuid_set(obj,GPON_DEV_DEFAULT_ONU_ID))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_GPON), "%s, return failed ret value = %x",__FUNCTION__,ret);
        return;
    }
}

static void gmac_fsm_o4_disable(gpon_dev_obj_t *obj)
{
    int32 ret;
    /* shut down optical transmit power immediately. */
    //TBD: use gpio to control laser off

    if(obj->timer)
    {
        GPON_OS_Log(GPON_LOG_LEVEL_DEBUG,"stop timer %p [baseaddr:%p]",obj->timer,obj->base_addr);
        GPON_OS_StopTimer(obj->timer);
        obj->timer = 0;
    }
    obj->status = GPON_STATE_O7;

    if((ret = rtk_gpon_onuState_set(obj->status))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_GPON), "%s, return failed ret value = %x",__FUNCTION__,ret);
        return;
    }
    if((ret = gpon_dev_onuid_set(obj,GPON_DEV_DEFAULT_ONU_ID))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_GPON), "%s, return failed ret value = %x",__FUNCTION__,ret);
        return;
    }

    /* set TX disable when OTL set ONT to disable */
    if((ret = gpon_dev_rogueOntDisTx_set(obj, DISABLED))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_GPON), "%s, return failed ret value = %x",__FUNCTION__,ret);
        return;
    }
}

static void gmac_fsm_o5_deactivate(gpon_dev_obj_t *obj)
{
    int32 ret;
    obj->status = GPON_STATE_O2;

    /*set pon port to mac fore mode*/
    if((ret = gpon_dev_portMacForceMode_set(obj, PORT_LINKDOWN))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_GPON), "%s, return failed ret value = %x",__FUNCTION__,ret);
        return;
    }

#if 0 /* this function is for power saving, it is not verified yet. */
    if((ret = rtk_gpon_autoDisTx_set(DISABLED))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_GPON), "%s, return failed ret value = %x",__FUNCTION__,ret);
        return;
    }
#endif
    if((ret = rtk_gpon_onuState_set(obj->status))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_GPON), "%s, return failed ret value = %x",__FUNCTION__,ret);
        return;
    }
    if(obj->ber_timer)
    {
         GPON_OS_StopTimer(obj->ber_timer);
         obj->ber_timer = 0;
    }

    if(obj->signal_callback)
    {
        (*obj->signal_callback)(GPON_SIGNAL_DEACTIVE);
    }
    if((ret = gpon_dev_onuid_set(obj,GPON_DEV_DEFAULT_ONU_ID))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_GPON), "%s, return failed ret value = %x",__FUNCTION__,ret);
        return;
    }

    /*drain-out default queue first*/
    if((ret = rtk_gpon_drainOutDefaultQueue_set())!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_GPON), "%s, return failed ret value = %x",__FUNCTION__,ret);
        return;
    }
}

static void gmac_fsm_o5_los_detect(gpon_dev_obj_t *obj)
{
    int32 ret;
    if(obj->timer)
    {
        GPON_OS_Log(GPON_LOG_LEVEL_DEBUG,"stop timer %p [baseaddr:%p]",obj->timer,obj->base_addr);
        GPON_OS_StopTimer(obj->timer);
        obj->timer = 0;
    }
    obj->timer = GPON_OS_StartTimer(obj->parameter.onu.to2_timer,FALSE,0,gpon_fsm_to2_expire);
    GPON_OS_Log(GPON_LOG_LEVEL_DEBUG,"start TO2 timer %p [baseaddr:%p]",obj->timer,obj->base_addr);
    obj->status = GPON_STATE_O6;
#if 0 /* this function is for power saving, it is not verified yet. */
    if((ret = rtk_gpon_autoDisTx_set(DISABLED))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_GPON), "%s, return failed ret value = %x",__FUNCTION__,ret);
        return;
    }
#endif

    if((ret = rtk_gpon_onuState_set(obj->status))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_GPON), "%s, return failed ret value = %x",__FUNCTION__,ret);
        return;
    }

#if 0 /* move to O6 handle */
    /*drain-out default queue first*/
    if((ret = rtk_gpon_drainOutDefaultQueue_set())!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_GPON), "%s, return failed ret value = %x",__FUNCTION__,ret);
        return;
    }
#endif
}

static void gmac_fsm_o5_disable(gpon_dev_obj_t *obj)
{
    int32 ret;
    /*set pon port to mac fore mode*/
    if((ret = gpon_dev_portMacForceMode_set(obj, PORT_LINKDOWN))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_GPON), "%s, return failed ret value = %x",__FUNCTION__,ret);
        return;
    }

    /* shut down optical transmit power immediately. */
    //TBD: use gpio to control laser off

    obj->status = GPON_STATE_O7;

#if 0 /* this function is for power saving, it is not verified yet. */
    if((ret = rtk_gpon_autoDisTx_set(DISABLED))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_GPON), "%s, return failed ret value = %x",__FUNCTION__,ret);
        return;
    }
#endif
    if((ret = rtk_gpon_onuState_set(obj->status))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_GPON), "%s, return failed ret value = %x",__FUNCTION__,ret);
        return;
    }
    if(obj->ber_timer)
    {
         GPON_OS_StopTimer(obj->ber_timer);
         obj->ber_timer = 0;
    }
    if(obj->signal_callback)
    {
        (*obj->signal_callback)(GPON_SIGNAL_DEACTIVE);
    }
    if((ret = gpon_dev_onuid_set(obj,GPON_DEV_DEFAULT_ONU_ID))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_GPON), "%s, return failed ret value = %x",__FUNCTION__,ret);
        return;
    }

    /*drain-out default queue first*/
    if((ret = rtk_gpon_drainOutDefaultQueue_set())!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_GPON), "%s, return failed ret value = %x",__FUNCTION__,ret);
        return;
    }

    /* set TX disable when OTL set ONT to disable */
    if((ret = gpon_dev_rogueOntDisTx_set(obj, DISABLED))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_GPON), "%s, return failed ret value = %x",__FUNCTION__,ret);
        return;
    }
}

static void gmac_fsm_o6_deactivate(gpon_dev_obj_t *obj)
{
    int32 ret;
    /*set pon port to mac fore mode*/
    if((ret = gpon_dev_portMacForceMode_set(obj, PORT_LINKDOWN))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_GPON), "%s, return failed ret value = %x",__FUNCTION__,ret);
        return;
    }

    if(obj->timer)
    {
        GPON_OS_Log(GPON_LOG_LEVEL_DEBUG,"stop timer %p [baseaddr:%p]",obj->timer,obj->base_addr);
        GPON_OS_StopTimer(obj->timer);
        obj->timer = 0;
    }
    obj->status = GPON_STATE_O2;

    if((ret = rtk_gpon_onuState_set(obj->status))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_GPON), "%s, return failed ret value = %x",__FUNCTION__,ret);
        return;
    }

    if(obj->ber_timer)
    {
         GPON_OS_StopTimer(obj->ber_timer);
         obj->ber_timer = 0;
    }
    if(obj->signal_callback)
    {
        (*obj->signal_callback)(GPON_SIGNAL_DEACTIVE);
    }
    if((ret = gpon_dev_onuid_set(obj,GPON_DEV_DEFAULT_ONU_ID))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_GPON), "%s, return failed ret value = %x",__FUNCTION__,ret);
        return;
    }

    /*drain-out default queue first*/
    if((ret = rtk_gpon_drainOutDefaultQueue_set())!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_GPON), "%s, return failed ret value = %x",__FUNCTION__,ret);
        return;
    }
}

static void gmac_fsm_o6_broadcast_popup(gpon_dev_obj_t *obj)
{
    int32 ret;
    if(obj->timer)
    {
        GPON_OS_Log(GPON_LOG_LEVEL_DEBUG,"stop timer %p [baseaddr:%p]",obj->timer,obj->base_addr);
        GPON_OS_StopTimer(obj->timer);
        obj->timer = 0;
    }

    /*drain-out default queue first*/
    if((ret = rtk_gpon_drainOutDefaultQueue_set())!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_GPON), "%s, return failed ret value = %x",__FUNCTION__,ret);
        return;
    }

    obj->timer = GPON_OS_StartTimer(obj->parameter.onu.to1_timer,FALSE,0,gpon_fsm_to1_expire);
    GPON_OS_Log(GPON_LOG_LEVEL_DEBUG,"start TO1 timer %p [baseaddr:%p]",obj->timer,obj->base_addr);
    obj->status = GPON_STATE_O4;

    if((ret = gpon_dev_burstHead_preRanged_set(obj))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_GPON), "%s, return failed ret value = %x",__FUNCTION__,ret);
        return;
    }
    if(obj->ber_timer)
    {
         GPON_OS_StopTimer(obj->ber_timer);
         obj->ber_timer = 0;
    }
    /* set EQD to pre_eqd */
    if((ret = rtk_gpon_eqd_set(obj->pre_eqd,obj->eqd_offset))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_GPON), "%s, return failed ret value = %x",__FUNCTION__,ret);
        return;
    }
    if((ret = rtk_gpon_onuState_set(obj->status))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_GPON), "%s, return failed ret value = %x",__FUNCTION__,ret);
        return;
    }

    /*
    * O6 popup should not set this flag
    */
    gLosClrO6 = 0;
}

static void gmac_fsm_o6_directed_popup(gpon_dev_obj_t *obj)
{
    int32 ret;
    if(obj->timer)
    {
        GPON_OS_Log(GPON_LOG_LEVEL_DEBUG,"stop timer %p [baseaddr:%p]",obj->timer,obj->base_addr);
        GPON_OS_StopTimer(obj->timer);
        obj->timer = 0;
    }
    obj->status = GPON_STATE_O5;

    if((ret = rtk_gpon_onuState_set(obj->status))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_GPON), "%s, return failed ret value = %x",__FUNCTION__,ret);
        return;
    }

    /*
    * O6 popup should not set this flag
    */
    gLosClrO6 = 0;
}

static void gmac_fsm_o6_to2_expire(gpon_dev_obj_t *obj)
{
    int32 ret;
    /*set pon port to mac fore mode*/
    if((ret = gpon_dev_portMacForceMode_set(obj, PORT_LINKDOWN))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_GPON), "%s, return failed ret value = %x",__FUNCTION__,ret);
        return;
    }

    obj->status = GPON_STATE_O1;

    if((ret = rtk_gpon_onuState_set(obj->status))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_GPON), "%s, return failed ret value = %x",__FUNCTION__,ret);
        return;
    }
    if(obj->signal_callback)
    {
        (*obj->signal_callback)(GPON_SIGNAL_DEACTIVE);
    }
    if((ret = gpon_dev_onuid_set(obj,GPON_DEV_DEFAULT_ONU_ID))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_GPON), "%s, return failed ret value = %x",__FUNCTION__,ret);
        return;
    }
    if(obj->ber_timer)
    {
         GPON_OS_StopTimer(obj->ber_timer);
     obj->ber_timer = 0;
    }
    /*
    * to2 set to 10s
    * plug fiber quickly,pon miss los intr in o6
    * when to2 expire,ont in o1 state,can't range
    */
    if(gLosClrO6 == 1)
    {
        osal_time_mdelay(10);
        gmac_fsm_o1_los_clear(obj);
        gLosClrO6 = 0;
    }

    /*drain-out default queue first*/
    if((ret = rtk_gpon_drainOutDefaultQueue_set())!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_GPON), "%s, return failed ret value = %x",__FUNCTION__,ret);
        return;
    }
}

static void gmac_fsm_o6_disable(gpon_dev_obj_t *obj)
{
    int32 ret;
    /* shut down optical transmit power immediately. */
    //TBD: use gpio to control laser off

    /*set pon port to mac fore mode*/
    if((ret = gpon_dev_portMacForceMode_set(obj, PORT_LINKDOWN))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_GPON), "%s, return failed ret value = %x",__FUNCTION__,ret);
        return;
    }

    if(obj->timer)
    {
        GPON_OS_Log(GPON_LOG_LEVEL_DEBUG,"stop timer %p [baseaddr:%p]",obj->timer,obj->base_addr);
        GPON_OS_StopTimer(obj->timer);
        obj->timer = 0;
    }
    obj->status = GPON_STATE_O7;

    if((ret = rtk_gpon_onuState_set(obj->status))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_GPON), "%s, return failed ret value = %x",__FUNCTION__,ret);
        return;
    }

    if(obj->ber_timer)
    {
         GPON_OS_StopTimer(obj->ber_timer);
         obj->ber_timer = 0;
    }
    if(obj->signal_callback)
    {
        (*obj->signal_callback)(GPON_SIGNAL_DEACTIVE);
    }
    if((ret = gpon_dev_onuid_set(obj,GPON_DEV_DEFAULT_ONU_ID))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_GPON), "%s, return failed ret value = %x",__FUNCTION__,ret);
        return;
    }

    /* set TX disable when OTL set ONT to disable */
    if((ret = gpon_dev_rogueOntDisTx_set(obj, DISABLED))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_GPON), "%s, return failed ret value = %x",__FUNCTION__,ret);
        return;
    }

    /*drain-out default queue first*/
    if((ret = rtk_gpon_drainOutDefaultQueue_set())!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_GPON), "%s, return failed ret value = %x",__FUNCTION__,ret);
        return;
    }
}

static void gmac_fsm_o6_los_clear(gpon_dev_obj_t *obj)
{
    if(obj){}

    gLosClrO6 = 1;
}

static void gmac_fsm_o6_los_detect(gpon_dev_obj_t *obj)
{
    if(obj){}

    gLosClrO6 = 0;
}

static void gmac_fsm_o7_enable(gpon_dev_obj_t *obj)
{
    int32 ret;
    /* turn on optical transmit power immediately. */
    //TBD: use gpio to control laser on

    obj->status = GPON_STATE_O2;

    if((ret = rtk_gpon_onuState_set(obj->status))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_GPON), "%s, return failed ret value = %x",__FUNCTION__,ret);
        return;
    }

    /* set TX enable when OTL set ONT to enable */
    if((ret = gpon_dev_rogueOntDisTx_set(obj, ENABLED))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_GPON), "%s, return failed ret value = %x",__FUNCTION__,ret);
        return;
    }
}

void gpon_fsm_init(void)
{
    uint8 i,j;

    /* clear the table */
    for(i=0;i<GPON_STATE_O7;i++)
    {
        for(j=0;j<GPON_FSM_EVENT_MAX;j++)
        {
            g_gmac_fsm_eventhandler[i][j] = NULL;
        }
    }

    /* initialize the table */
    g_gmac_fsm_eventhandler[GPON_STATE_O1-1][GPON_FSM_EVENT_LOS_CLEAR] = gmac_fsm_o1_los_clear;

    g_gmac_fsm_eventhandler[GPON_STATE_O2-1][GPON_FSM_EVENT_RX_UPSTREAM] = gmac_fsm_o2_upstream;
    g_gmac_fsm_eventhandler[GPON_STATE_O2-1][GPON_FSM_EVENT_LOS_DETECT] = gmac_fsm_o2_los_detect;
    g_gmac_fsm_eventhandler[GPON_STATE_O2-1][GPON_FSM_EVENT_RX_DISABLE] = gmac_fsm_o2_disable;

    g_gmac_fsm_eventhandler[GPON_STATE_O3-1][GPON_FSM_EVENT_RX_ONUID] = gmac_fsm_o3_onuid;
    g_gmac_fsm_eventhandler[GPON_STATE_O3-1][GPON_FSM_EVENT_TO1_EXPIRE] = gmac_fsm_o3_to1_expire;
    g_gmac_fsm_eventhandler[GPON_STATE_O3-1][GPON_FSM_EVENT_LOS_DETECT] = gmac_fsm_o3_los_detect;
    g_gmac_fsm_eventhandler[GPON_STATE_O3-1][GPON_FSM_EVENT_RX_DISABLE] = gmac_fsm_o3_disable;

    g_gmac_fsm_eventhandler[GPON_STATE_O4-1][GPON_FSM_EVENT_RX_EQD] = gmac_fsm_o4_eqd;
    g_gmac_fsm_eventhandler[GPON_STATE_O4-1][GPON_FSM_EVENT_TO1_EXPIRE] = gmac_fsm_o4_to1_expire;
    g_gmac_fsm_eventhandler[GPON_STATE_O4-1][GPON_FSM_EVENT_RX_DEACTIVATE] = gmac_fsm_o4_deactivate;
    g_gmac_fsm_eventhandler[GPON_STATE_O4-1][GPON_FSM_EVENT_LOS_DETECT] = gmac_fsm_o4_los_detect;
    g_gmac_fsm_eventhandler[GPON_STATE_O4-1][GPON_FSM_EVENT_RX_DISABLE] = gmac_fsm_o4_disable;

    g_gmac_fsm_eventhandler[GPON_STATE_O5-1][GPON_FSM_EVENT_RX_DEACTIVATE] = gmac_fsm_o5_deactivate;
    g_gmac_fsm_eventhandler[GPON_STATE_O5-1][GPON_FSM_EVENT_LOS_DETECT] = gmac_fsm_o5_los_detect;
    g_gmac_fsm_eventhandler[GPON_STATE_O5-1][GPON_FSM_EVENT_RX_DISABLE] = gmac_fsm_o5_disable;

    g_gmac_fsm_eventhandler[GPON_STATE_O6-1][GPON_FSM_EVENT_RX_DEACTIVATE] = gmac_fsm_o6_deactivate;
    g_gmac_fsm_eventhandler[GPON_STATE_O6-1][GPON_FSM_EVENT_RX_BC_POPUP] = gmac_fsm_o6_broadcast_popup;
    g_gmac_fsm_eventhandler[GPON_STATE_O6-1][GPON_FSM_EVENT_RX_DIRECT_POPUP] = gmac_fsm_o6_directed_popup;
    g_gmac_fsm_eventhandler[GPON_STATE_O6-1][GPON_FSM_EVENT_TO2_EXPIRE] = gmac_fsm_o6_to2_expire;
    g_gmac_fsm_eventhandler[GPON_STATE_O6-1][GPON_FSM_EVENT_RX_DISABLE] = gmac_fsm_o6_disable;
    g_gmac_fsm_eventhandler[GPON_STATE_O6-1][GPON_FSM_EVENT_LOS_CLEAR] = gmac_fsm_o6_los_clear;
    g_gmac_fsm_eventhandler[GPON_STATE_O6-1][GPON_FSM_EVENT_LOS_DETECT] = gmac_fsm_o6_los_detect;

    g_gmac_fsm_eventhandler[GPON_STATE_O7-1][GPON_FSM_EVENT_RX_ENABLE] = gmac_fsm_o7_enable;
}

void gpon_fsm_event(gpon_dev_obj_t *obj, rtk_gpon_fsm_event_t event)
{
    rtk_gpon_fsm_status_t oldstate = obj->status;

    GPON_OS_Log(GPON_LOG_LEVEL_NORMAL,"FSM State %s receive event %s [baseaddr:%p]",gpon_dbg_fsm_status_str(obj->status),gpon_dbg_fsm_event_str(event),obj->base_addr);
    osal_printf("FSM State %s receive event %s \n\r",gpon_dbg_fsm_status_str(obj->status),gpon_dbg_fsm_event_str(event));
    if(g_gmac_fsm_eventhandler[obj->status-1][event])
    {
        (*g_gmac_fsm_eventhandler[obj->status-1][event])(obj);
    }

    if(obj->status!=oldstate)
    {
        GPON_OS_Log(GPON_LOG_LEVEL_NORMAL,"FSM State changed: from %s to %s [baseaddr:%p]",gpon_dbg_fsm_status_str(oldstate),gpon_dbg_fsm_status_str(obj->status),obj->base_addr);
        if(obj->state_change_callback)
        {
            (*obj->state_change_callback)(event,obj->status,oldstate);  // modified by haoship,reason:support func "rtk_handle_state_change()"in rtk_drv_callback.c
        }
    }
}

extern gpon_drv_obj_t *g_gponmac_drv;
void gpon_fsm_to1_expire(uint32 data)
{
    /*for compiler warning*/
    if(data){}

    if(g_gponmac_drv->dev->timer)
    {
        GPON_OS_StopTimer(g_gponmac_drv->dev->timer);
        g_gponmac_drv->dev->timer = 0;
    }
    GPON_OS_Lock(g_gponmac_drv->lock);
    gpon_fsm_event(g_gponmac_drv->dev,GPON_FSM_EVENT_TO1_EXPIRE);
    GPON_OS_Unlock(g_gponmac_drv->lock);
    GPON_OS_Log(GPON_LOG_LEVEL_NORMAL,"FSM TO1 expire");
}

void gpon_fsm_to2_expire(uint32 data)
{
    /*for compiler warning*/
    if(data){}

    if(g_gponmac_drv->dev->timer)
    {
        GPON_OS_StopTimer(g_gponmac_drv->dev->timer);
        g_gponmac_drv->dev->timer = 0;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);
    gpon_fsm_event(g_gponmac_drv->dev,GPON_FSM_EVENT_TO2_EXPIRE);
    GPON_OS_Unlock(g_gponmac_drv->lock);
    GPON_OS_Log(GPON_LOG_LEVEL_NORMAL,"FSM TO2 expire");
}

