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
 * Purpose : GMac Driver Alarm Management
 *
 * Feature : Provide the APIs of GMac Driver Alarm Management
 *
 */

#include <module/gpon/gpon_defs.h>
#include <module/gpon/gpon_fsm.h>
#include <module/gpon/gpon_alm.h>
#include <module/gpon/gpon_debug.h>
#include <hal/chipdef/chip.h>
#include <rtk/switch.h>
#include <rtk/ponmac.h>

int32 gpon_alarm_event(gpon_dev_obj_t* obj, rtk_gpon_alarm_type_t alarm, int32        status)
{
    int32        olds,news;
    int32        ret;
    if(alarm>=RTK_GPON_ALARM_MAX)
    {
        return RT_ERR_OUT_OF_RANGE;
    }

    if(obj->alarm_tbl[alarm]!=status)
    {
        GPON_OS_Log(GPON_LOG_LEVEL_WARNING,"Alarm %s Event %d [baseaddr:%p]",gpon_dbg_alarm_str(alarm),status,obj->base_addr);

        /* patch 0639 */
        if((RTK_GPON_ALARM_LOS==alarm) &&(0==status)&&(1==obj->alarm_tbl[alarm]))
        {
            int32 chipid=0, rev=0, dmy_type=0;
            if((ret = rtk_switch_version_get(&chipid, &rev, &dmy_type))==RT_ERR_OK)
            {
                if((RTL9601B_CHIP_ID==chipid) && (0==rev))
                {
                    if((ret = rtk_ponmac_serdesCdr_reset())!=RT_ERR_OK)
                    {
                        RT_ERR(ret, (MOD_GPON), "%s,%d,ret=0x%x",__FUNCTION__,__LINE__,ret);
                    }
                }
            }
        }

        olds = obj->alarm_tbl[RTK_GPON_ALARM_LOS] | obj->alarm_tbl[RTK_GPON_ALARM_LOF];
        obj->alarm_tbl[alarm] = status;
        if(alarm==RTK_GPON_ALARM_LOF || alarm==RTK_GPON_ALARM_LOS)
        {
            news = obj->alarm_tbl[RTK_GPON_ALARM_LOS] | obj->alarm_tbl[RTK_GPON_ALARM_LOF];

            if(olds!=news)
            {
                if(news)
                {
                    gpon_fsm_event(obj,GPON_FSM_EVENT_LOS_DETECT);
                }
                else
                {
                    gpon_fsm_event(obj,GPON_FSM_EVENT_LOS_CLEAR);
                }
            }
        }

        if(obj->fault_callback[alarm])
        {
            (*obj->fault_callback[alarm])(alarm, status);
        }
    }

    return RT_ERR_OK;
}

int32 gpon_alarm_status_get(gpon_dev_obj_t* obj, rtk_gpon_alarm_type_t alarm, int32 *pStatus)
{
    *pStatus = obj->alarm_tbl[alarm];
    return RT_ERR_OK;
}

void gpon_alarm_status_check(gpon_dev_obj_t* obj)
{
    int32 ret;
    uint32 los, lof, lom;

    if((ret = rtk_gpon_gtcDsIntr_get(GPON_LOS_DLT,&los))!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
        return ;
    }
    if((ret = rtk_gpon_gtcDsIntr_get(GPON_LOF_DLT,&lof))!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
        return ;
    }
    if((ret = rtk_gpon_gtcDsIntr_get(GPON_LOM_DLT,&lom))!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
        return ;
    }

    gpon_alarm_event(obj,RTK_GPON_ALARM_LOS,los);
    gpon_alarm_event(obj,RTK_GPON_ALARM_LOF,lof);
    gpon_alarm_event(obj,RTK_GPON_ALARM_LOM,lom);
}

void gpon_alarm_status_reset(gpon_dev_obj_t* obj)
{
    int32        olds;
    uint32 i;

    olds = obj->alarm_tbl[RTK_GPON_ALARM_LOS] | obj->alarm_tbl[RTK_GPON_ALARM_LOF];
    if(!olds)
    {
        gpon_fsm_event(obj,GPON_FSM_EVENT_LOS_DETECT);
    }

    for(i=0;i<RTK_GPON_ALARM_MAX;i++)
    {
        if(RTK_GPON_ALARM_SD == i || 
            RTK_GPON_ALARM_SF == i)
        {
            obj->alarm_tbl[i] = FALSE;
            continue;
        }
        if(!obj->alarm_tbl[i])
        {
            obj->alarm_tbl[i] = TRUE;
            if(obj->fault_callback[i])
            {
                (*obj->fault_callback[i])(i, TRUE);
            }
        }
    }
}

