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
 * Purpose : GMac Driver Interrupt Processor
 *
 * Feature : GMac Driver Interrupt Processor
 *
 */

#include <module/gpon/gpon_defs.h>
#include <module/gpon/gpon_int.h>
#include <module/gpon/gpon_fsm.h>
#include <module/gpon/gpon_ploam.h>
#include <module/gpon/gpon_omci.h>
#include <module/gpon/gpon_alm.h>
#include <module/gpon/gpon_res.h>
#include <rtk/irq.h>

extern gpon_drv_obj_t *g_gponmac_drv;


static int32 gpon_isr_dsGtc_check( gpon_dev_obj_t* obj)
{

    uint32 value=0;
    rtk_enable_t enable=DISABLED;
    uint32 los=0, lof=0, lom=0, ds_fec=0;
    int32 ret;

    if((ret = rtk_gpon_gtcDsIntrDlt_get(GPON_GTC_DS_INTR_ALL,&value))!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_GPON), "");
        return ret;
    }

    GPON_OS_Log(GPON_LOG_LEVEL_INFO,"GTC D/S Interrupt status %04x",value);

    if((ret = rtk_gpon_gtcDsIntrDlt_check(GPON_PLM_BUF_REQ,value,&enable))==RT_ERR_OK)
    {
        if(enable)
        {
            if((ret = gpon_ploam_rx(obj))!=RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_GPON), "%s,%d,ret=0x%x",__FUNCTION__,__LINE__,ret);
            }
        }
    }
    
    if((ret = rtk_gpon_gtcDsIntrDlt_check(GPON_LOS_DLT,value,&enable))==RT_ERR_OK)
    {
        if(enable)
        {
            if((ret = rtk_gpon_gtcDsIntr_get(GPON_LOS_DLT,&los))==RT_ERR_OK)
            {
                if((ret = gpon_alarm_event(obj,RTK_GPON_ALARM_LOS,los))!=RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_GPON), "%s,%d,ret=0x%x",__FUNCTION__,__LINE__,ret);
                }
            }
        }
    }
    
    if((ret= rtk_gpon_gtcDsIntrDlt_check(GPON_LOF_DLT,value,&enable))==RT_ERR_OK)
    {
        if(enable)
        {
            if((ret = rtk_gpon_gtcDsIntr_get(GPON_LOF_DLT,&lof))==RT_ERR_OK)
            {
                if((ret = gpon_alarm_event(obj,RTK_GPON_ALARM_LOF,lof))!=RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_GPON), "%s,%d,ret=0x%x",__FUNCTION__,__LINE__,ret);
                }
            }
        }
    }
    
    if((ret = rtk_gpon_gtcDsIntrDlt_check(GPON_SN_REQ_HIS,value,&enable))==RT_ERR_OK)
    {
        if(enable)
        {
            gpon_fsm_event(obj,GPON_FSM_EVENT_RX_SN_REQ);
        }
    }
    
    if((ret = rtk_gpon_gtcDsIntrDlt_check(GPON_RNG_REQ_HIS,value,&enable))==RT_ERR_OK)
    {
        if(enable)
        {
            gpon_fsm_event(obj,GPON_FSM_EVENT_RX_RANGING_REQ);
        }
    }
    
    if((ret = rtk_gpon_gtcDsIntrDlt_check(GPON_LOM_DLT,value,&enable))==RT_ERR_OK)
    {
        if(enable)
        {
            if((ret = rtk_gpon_gtcDsIntr_get(GPON_LOM_DLT,&lom))==RT_ERR_OK)
            {
                if((ret = gpon_alarm_event(obj,RTK_GPON_ALARM_LOM,lom))!=RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_GPON), "%s,%d,ret=0x%x",__FUNCTION__,__LINE__,ret);
                }
            }
        }
    }
    
    if((ret = rtk_gpon_gtcDsIntrDlt_check(GPON_DS_FEC_STS_DLT,value,&enable))==RT_ERR_OK)
    {
        if(enable)
        {
            if((ret = rtk_gpon_gtcDsIntr_get(GPON_DS_FEC_STS_DLT,&ds_fec))==RT_ERR_OK)
            {
                if((ret = gpon_dev_dsFec_report(obj,ds_fec))!=RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_GPON), "%s,%d,ret=0x%x",__FUNCTION__,__LINE__,ret);
                }
            }
        }
    }
    
    /*pps is not supported in all chip types*/
    if((ret = rtk_gpon_gtcDsIntrDlt_check(GPON_PPS_DLT,value,&enable))==RT_ERR_OK)
    {
        if(enable)
        {
            if((ret = gpon_dev_pps_fire(obj))!=RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_GPON), "%s,%d,ret=0x%x",__FUNCTION__,__LINE__,ret);
            }
        }
    }

    return RT_ERR_OK;
}


static int32 gpon_isr_usGtc_check(gpon_dev_obj_t* obj)
{

    uint32 value=0;
    rtk_enable_t enable=DISABLED;
    uint32 us_fec=0;
    int32 ret;
    uint32 mask=0;

    if((ret = rtk_gpon_gtcUsIntrDlt_get(GPON_GTC_US_INTR_ALL,&value))!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_GPON), "");
        return ret;
    }
    GPON_OS_Log(GPON_LOG_LEVEL_INFO,"GTC U/S Interrupt status %04x",value);

    if((ret = rtk_gpon_gtcUsIntrDlt_check(GPON_US_FEC_STS,value,&enable))==RT_ERR_OK)
    {
        if(enable)
        {
            if((ret = rtk_gpon_gtcUsIntr_get(GPON_US_FEC_STS,&us_fec))==RT_ERR_OK)
            {
                if((ret = gpon_dev_usFec_report(obj,us_fec))!=RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_GPON), "%s,%d,ret=0x%x",__FUNCTION__,__LINE__,ret);
                }
            }
        }
    }
    
    if((ret = rtk_gpon_gtcUsIntrDlt_check(GPON_PLM_URG_EMPTY,value,&enable))==RT_ERR_OK)
    {
        if(enable)
        {
            gpon_ploam_usUrgPloamQ_empty_report(obj);
        }
    }
    
    if((ret = rtk_gpon_gtcUsIntrDlt_check(GPON_PLM_NRM_EMPTY,value,&enable))==RT_ERR_OK)
    {
        if(enable)
        {
            gpon_ploam_usNrmPloamQ_Empty_report(obj);
        }
    }
    
    if((ret = rtk_gpon_gtcUsIntrDlt_check(GPON_OPTIC_SD_MISM,value,&enable))==RT_ERR_OK)
    {
        if(enable)
        {
            if((ret = rtk_gpon_gtcUsIntrMask_get(GPON_OPTIC_SD_MISM,&mask))==RT_ERR_OK)
            {
                if(mask) 
                {
                    obj->sd_mismatch_cnt++;
                    obj->alarm_tbl[RTK_GPON_ALARM_SD_MISMATCH] = TRUE;
                    if((ret = gpon_dev_rogueOntDisTx_set(obj, DISABLED))!=RT_ERR_OK)
                    {
                        RT_ERR(ret, (MOD_GPON), "%s,%d,ret=0x%x",__FUNCTION__,__LINE__,ret);
                    }
                }
            }
        }
    }

    if((ret = rtk_gpon_gtcUsIntrDlt_check(GPON_OPTIC_SD_TOOLONG,value,&enable))==RT_ERR_OK)
    {
        if(enable)
        {
            if((ret = rtk_gpon_gtcUsIntrMask_get(GPON_OPTIC_SD_TOOLONG,&mask))==RT_ERR_OK)
            {
                if(mask) 
                {
                    obj->sd_toolong_cnt++;
                    obj->alarm_tbl[RTK_GPON_ALARM_SD_TOOLONG] = TRUE;
                    if((ret = gpon_dev_rogueOntDisTx_set(obj, DISABLED))!=RT_ERR_OK)
                    {
                        RT_ERR(ret, (MOD_GPON), "%s,%d,ret=0x%x",__FUNCTION__,__LINE__,ret);
                    }
                }
            }
        }
    }
    
    return RT_ERR_OK;
}


static int32 gpon_isr_usGem_check(gpon_dev_obj_t* obj)
{

    uint32 value=0;
    rtk_enable_t enable=DISABLED;
    int32 ret;

    if(obj==NULL)
        return RT_ERR_NULL_POINTER;

    if((ret = rtk_gpon_gemUsIntr_get(GPON_GEM_US_INTR_ALL,&value))!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_GPON), "");
        return ret;
    }
    GPON_OS_Log(GPON_LOG_LEVEL_INFO,"GEM U/S Interrupt status %04x",value);

    if((ret=rtk_gpon_gemUsIntr_get(GPON_SD_VALID_LONG,&enable))==RT_ERR_OK)
    {
        if(enable)
        {
            GPON_OS_Log(GPON_LOG_LEVEL_WARNING,"U/S SD Long interrupt");
        }
    }
    
    if((ret=rtk_gpon_gemUsIntr_get(GPON_SD_DIFF_HUGE,&enable))==RT_ERR_OK)
    {
        if(enable)
        {
            GPON_OS_Log(GPON_LOG_LEVEL_WARNING,"U/S SD Diff interrupt");
        }
    }
    
    if((ret=rtk_gpon_gemUsIntr_get(GPON_REQUEST_DELAY,&enable))==RT_ERR_OK)
    {
        if(enable)
        {
            GPON_OS_Log(GPON_LOG_LEVEL_WARNING,"U/S request delay interrupt");
        }
    }
    
    if((ret=rtk_gpon_gemUsIntr_get(GPON_BC_LESS6,&enable))==RT_ERR_OK)
    {
        if(enable)
        {
            GPON_OS_Log(GPON_LOG_LEVEL_WARNING,"U/S byte count less than 6 interrupt");
        }
    }
    
    if((ret=rtk_gpon_gemUsIntr_get(GPON_ERR_PLI,&enable))==RT_ERR_OK)
    {
        if(enable)
        {
            GPON_OS_Log(GPON_LOG_LEVEL_WARNING,"U/S error PLI interrupt");
        }
    }
    
    if((ret=rtk_gpon_gemUsIntr_get(GPON_BURST_TM_LARGER_GTC,&enable))==RT_ERR_OK)
    {
        if(enable)
        {
            GPON_OS_Log(GPON_LOG_LEVEL_WARNING,"U/S burst large interrupt");
        }
    }
    
    if((ret=rtk_gpon_gemUsIntr_get(GPON_BANK_TOO_MUCH_AT_END,&enable))==RT_ERR_OK)
    {
        if(enable)
        {
            GPON_OS_Log(GPON_LOG_LEVEL_WARNING,"U/S bank too much interrupt");
        }
    }
    
    if((ret=rtk_gpon_gemUsIntr_get(GPON_BANK_REMAIN_AFRD,&enable))==RT_ERR_OK)
    {
        if(enable)
        {
            GPON_OS_Log(GPON_LOG_LEVEL_WARNING,"U/S bank remain interrupt");
        }
    }
    
    if((ret=rtk_gpon_gemUsIntr_get(GPON_BANK_OVERFL,&enable))==RT_ERR_OK)
    {
        if(enable)
        {
            obj->us_gem_overflow_cnt++;
            GPON_OS_Log(GPON_LOG_LEVEL_WARNING,"U/S bank overflow interrupt");
        }
    }
    
    if((ret=rtk_gpon_gemUsIntr_get(GPON_BANK_UNDERFL,&enable))==RT_ERR_OK)
    {
        if(enable)
        {
            obj->us_gem_underflow_cnt++;
            GPON_OS_Log(GPON_LOG_LEVEL_WARNING,"U/S bank underflow interrupt");
        }
    }

    return RT_ERR_OK;
}

void gpon_isr_entry(void)
{
    uint32 mask,stat;
    rtk_enable_t enable;
    gpon_dev_obj_t* obj;
    int32 ret;

    GPON_OS_Log(GPON_LOG_LEVEL_INFO,"dal_apollo_gpon_isr_entry");

    if(!g_gponmac_drv || !g_gponmac_drv->dev || g_gponmac_drv->status!=GPON_DRV_STATUS_ACTIVATE)
    {
        return;
    }

#if 0 /* interrupt mask is handle by switch_interrupt_bh, it is no need to be set here */
    /* switch interrupt disable GPON mask */
    if((ret=rtk_intr_imr_set(INTR_TYPE_GPON,DISABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
        return ;
    }
#endif

    obj = g_gponmac_drv->dev;

    /* disable top mask */
    GPON_OS_Lock(g_gponmac_drv->lock);
    if((ret = rtk_gpon_topIntrMask_get(GPON_INTR_ALL,&mask))!=RT_ERR_OK)
    {
        GPON_OS_Unlock(g_gponmac_drv->lock);
        GPON_OS_Log(GPON_LOG_LEVEL_INFO,"%s,%d,ret=0x%x",__FUNCTION__,__LINE__,ret);
        return ;
    }
    
    if((ret = rtk_gpon_topIntrMask_set(GPON_INTR_ALL,0))!=RT_ERR_OK)
    {
        GPON_OS_Unlock(g_gponmac_drv->lock);
        GPON_OS_Log(GPON_LOG_LEVEL_INFO,"%s,%d,ret=0x%x",__FUNCTION__,__LINE__,ret);
        return ;
    }
    GPON_OS_Unlock(g_gponmac_drv->lock);

    GPON_OS_Log(GPON_LOG_LEVEL_INFO,"Top Interrupt mask 0");

    /* read top int status */
    if((ret = rtk_gpon_topIntr_get(GPON_INTR_ALL,&stat))!=RT_ERR_OK)
    {
        GPON_OS_Log(GPON_LOG_LEVEL_INFO,"%s,%d,ret=0x%x",__FUNCTION__,__LINE__,ret);
        return ;
    }
    GPON_OS_Log(GPON_LOG_LEVEL_INFO,"Top Interrupt status %04x",stat);

    /* check int status */
    if((ret = rtk_gpon_topIntr_get(GPON_INTR_GTC_DS,&enable))==RT_ERR_OK)
    {
        /*dsGtc interrupt handler*/
        GPON_OS_Log(GPON_LOG_LEVEL_INFO,"Check GTC_DS is %d",enable);
        if(enable)
        {
            if((ret = gpon_isr_dsGtc_check(obj))!=RT_ERR_OK)
            {
                GPON_OS_Log(GPON_LOG_LEVEL_INFO,"%s,%d,ret=0x%x",__FUNCTION__,__LINE__,ret);
            }
        }
    }

    /*check gtc us interrupt status*/
    if((ret =rtk_gpon_topIntr_get(GPON_INTR_GTC_US,&enable))==RT_ERR_OK)
    {
        /*usGtc interrupt handler*/
        GPON_OS_Log(GPON_LOG_LEVEL_INFO,"Check GTC_US is %d",enable);
        if(enable)
        {
            if((ret = gpon_isr_usGtc_check(obj))!=RT_ERR_OK)
            {
                GPON_OS_Log(GPON_LOG_LEVEL_INFO,"%s,%d,ret=0x%x",__FUNCTION__,__LINE__,ret);
            }
        }
    }
    
    /*check gem us interrupt status*/
    if((ret =rtk_gpon_topIntr_get(GPON_INTR_GEM_US,&enable))==RT_ERR_OK)
    {
        /*usGem interrupt handler*/
        GPON_OS_Log(GPON_LOG_LEVEL_INFO,"Check GEM_US is %d",enable);
        if(enable)
        {
            if((ret = gpon_isr_usGem_check(obj))!=RT_ERR_OK)
            {
                GPON_OS_Log(GPON_LOG_LEVEL_INFO,"%s,%d,ret=0x%x",__FUNCTION__,__LINE__,ret);
            }
        }
    }
    
    GPON_OS_Lock(g_gponmac_drv->lock);
    /* enable top mask */
    rtk_gpon_topIntrMask_set(GPON_INTR_ALL,mask);
    GPON_OS_Unlock(g_gponmac_drv->lock);
    GPON_OS_Log(GPON_LOG_LEVEL_INFO,"Restore Top Interrupt mask 0x%x",mask);

    /* switch interrupt clear GPON state */
    if((ret=rtk_intr_ims_clear(INTR_TYPE_GPON)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
        return ;
    }

#if 0 /* interrupt mask is handle by switch_interrupt_bh, it is no need to be set here */
    /* switch interrupt enable GPON mask */
    if((ret=rtk_intr_imr_set(INTR_TYPE_GPON,ENABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
        return ;
    }
#endif
}



