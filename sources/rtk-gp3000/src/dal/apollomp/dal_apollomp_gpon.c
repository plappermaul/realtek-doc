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
 * Purpose : GMac Driver API
 *
 * Feature : Provide the APIs to access GPON MAC
 *
 */

#include <osal/print.h>
#include <dal/apollo/gpon/gpon_defs.h>
#include <dal/apollo/gpon/gpon_res.h>
#include <dal/apollo/gpon/gpon_fsm.h>
#include <dal/apollo/gpon/gpon_ploam.h>
#include <dal/apollo/gpon/gpon_omci.h>
#include <dal/apollo/gpon/gpon_alm.h>
#include <dal/apollo/gpon/gpon_debug.h>
#include <dal/apollo/gpon/gpon_pm.h>
#include <dal/apollo/gpon/gpon_int.h>
#include <dal/apollomp/dal_apollomp.h>
#include <dal/apollomp/dal_apollomp_gpon.h>
#include <dal/apollomp/raw/apollomp_raw_flowctrl.h>
#include <dal/apollomp/raw/apollomp_raw_port.h>
#include <dal/apollomp/dal_apollomp_ponmac.h>
#include <dal/apollomp/raw/apollomp_raw_ponmac.h>
#include <dal/apollomp/dal_apollomp_switch.h>
#include <ioal/mem32.h>

#if (defined(CONFIG_SDK_APOLLOMP) && !defined(CONFIG_SDK_APOLLO))
gpon_drv_obj_t *g_gponmac_drv = NULL;
#else
extern gpon_drv_obj_t *g_gponmac_drv;
#endif

int32 dal_apollomp_gpon_driver_initialize(void)
{
    uint32 data;
    int32  ret;
    apollomp_raw_port_ability_t mac_ability;

    if(g_gponmac_drv)
    {
       dal_apollomp_gpon_device_deInitialize();
       dal_apollomp_gpon_driver_deInitialize();
    }

    g_gponmac_drv = (gpon_drv_obj_t*)GPON_OS_Malloc(sizeof(gpon_drv_obj_t));

    g_gponmac_drv->status = GPON_DRV_STATUS_INIT;
    g_gponmac_drv->dev = NULL;

    /* FSM Handler Initialization */
    gpon_fsm_init();

    /* Start the GMAC thread here */
    g_gponmac_drv->lock = GPON_OS_CreateLock();

	/*linkdown pon port*/
	{
		   osal_memset(&mac_ability, 0x00, sizeof(apollomp_raw_port_ability_t));
		   mac_ability.speed		   = PORT_SPEED_1000M;
		   mac_ability.duplex		   = PORT_FULL_DUPLEX;
		   mac_ability.linkFib1g	   = DISABLED;
		   mac_ability.linkStatus	   = PORT_LINKDOWN;
		   mac_ability.txFc 		   = DISABLED;
		   mac_ability.rxFc 		   = DISABLED;
		   mac_ability.nwayAbility	   = DISABLED;
		   mac_ability.masterMod	   = DISABLED;
		   mac_ability.nwayFault	   = DISABLED;
		   mac_ability.lpi_100m 	   = DISABLED;
		   mac_ability.lpi_giga 	   = DISABLED;
		   if((ret = apollomp_raw_port_ForceAbility_set(HAL_GET_PON_PORT(), &mac_ability)) != RT_ERR_OK)
		   {
			   RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
			   return ret;
		   }

		   if((ret = reg_field_read(APOLLOMP_ABLTY_FORCE_MODEr, APOLLOMP_ABLTY_FORCE_MODEf, &data)) != RT_ERR_OK)
		   {
			   RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
			   return ret;
		   }

		   data |= (0x01 << HAL_GET_PON_PORT());

		   if((ret = reg_field_write(APOLLOMP_ABLTY_FORCE_MODEr, APOLLOMP_ABLTY_FORCE_MODEf, &data)) != RT_ERR_OK)
		   {
			   RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
			   return ret;
		   }
	}


#if 1 /* it should be determined by strap pin */
    /* set the GPON mode enable */
    data = 1;
    if ((ret = reg_write(APOLLOMP_PON_MODE_CFGr, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
        return ret;
    }
#endif

    /* set the serdes mode to GPON mode */
    data = 0x8;
    if ((ret = reg_field_write(APOLLOMP_SDS_CFGr, APOLLOMP_CFG_SDS_MODEf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
        return ret;
    }

    data = 0x0;
    /* PON MAC select phy 4 enable/disable */
    if ((ret = reg_field_write(APOLLOMP_CHIP_INF_SELr, APOLLOMP_PHY4_ENf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

#if 1
    /* reset serdes including digital and analog */
    /* no force sds */
    data = 0x0;
    if ((ret = reg_write(APOLLOMP_WSDS_DIG_01r, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
        return ret;
    }

    /* reset serdes including digital and analog, and GPON MAC */
    data = 0x1;
    if ((ret = reg_field_write(APOLLOMP_SOFTWARE_RSTr, APOLLOMP_CMD_SDS_RST_PSf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
        return ret;
    }
    osal_time_mdelay(10);

    /* force sds for not reset GPON MAC when SD down */
    data = 0xc;
    if ((ret = reg_write(APOLLOMP_WSDS_DIG_01r, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
        return ret;
    }
#endif

    data = 0x0;
    if ((ret = reg_write(APOLLOMP_WSDS_ANA_1Cr, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
        return ret;
    }

    /* Serdes amplitude 500 mv */
    data = 0x1164;
    if ((ret = reg_write(APOLLOMP_WSDS_ANA_0Er, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
        return ret;
    }

    /* BEN on */
    data = 0x1;
    if ((ret = reg_field_write(APOLLOMP_WSDS_DIG_18r, APOLLOMP_BEN_OEf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
        return ret;
    }

    /* DBRu period */
    data = 1;
    if ((ret = reg_field_write(APOLLOMP_GPON_DPRU_RPT_PRDr, APOLLOMP_RPT_PRDf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
        return ret;
    }

    /* pon port allow undersize packet for  some OLT will send 60 byte packet after removing VLAN tag */
    data = 1;
    if ((ret = reg_array_field_write(APOLLOMP_P_MISCr, HAL_GET_PON_PORT(), REG_ARRAY_INDEX_NONE, APOLLOMP_RX_SPCf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
        return ret;
    }

    /* turn off 'serdes_tx_dis when ben=0' */
    data = 0;
    if ((ret = reg_field_write(APOLLOMP_WSDS_DIG_06r, APOLLOMP_CFG_DMY1f, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}

int32 dal_apollomp_gpon_driver_deInitialize(void)
{
    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(g_gponmac_drv->status!=GPON_DRV_STATUS_INIT || g_gponmac_drv->dev)
    {
        return RT_ERR_GPON_ONU_ALREADY_REGISTERED;
    }

    if(g_gponmac_drv->lock)
    {
        GPON_OS_DestroyLock(g_gponmac_drv->lock);
    }

    GPON_OS_Free(g_gponmac_drv);
    g_gponmac_drv = NULL;
    return RT_ERR_OK;
}

int32 dal_apollomp_gpon_device_initialize(void)
{
    int32 ret;
    uint32 addr;
    uint32 data;
    uint32 speed;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(g_gponmac_drv->status!=GPON_DRV_STATUS_INIT || g_gponmac_drv->dev)
    {
        return RT_ERR_GPON_ONU_ALREADY_REGISTERED;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    g_gponmac_drv->dev = (gpon_dev_obj_t*)GPON_OS_Malloc(sizeof(gpon_dev_obj_t));

    ret = gpon_dev_initialize(g_gponmac_drv->dev,0);
    if(ret==RT_ERR_OK)
    {
        g_gponmac_drv->status = GPON_DRV_STATUS_READY;
    }
    else
    {
        GPON_OS_Free(g_gponmac_drv->dev);
        g_gponmac_drv->dev = NULL;
        g_gponmac_drv->status=GPON_DRV_STATUS_INIT;

    }

    /* reset GPON data-path */
    data = 0;
    if ((ret = reg_field_write(APOLLOMP_WSDS_DIG_00r, APOLLOMP_CFG_SFT_RSTB_GPONf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
        return ret;
    }
    data = 1;
    if ((ret = reg_field_write(APOLLOMP_WSDS_DIG_00r, APOLLOMP_CFG_SFT_RSTB_GPONf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
        return ret;
    }

    /* set threshold for HEC error patch , GPON MAC page 6 offset 0x98 */
    addr = (6 << 12) | (0x98)*4 | 0x700000;
    data = 40;
    if ((ret = ioal_mem32_write(addr, data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
        return ret;
    }

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}

int32 dal_apollomp_gpon_device_deInitialize(void)
{
    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    if(g_gponmac_drv->status==GPON_DRV_STATUS_ACTIVATE)
    {
        return RT_ERR_GPON_ONU_ALREADY_ACTIVATED;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    if(g_gponmac_drv->status==GPON_DRV_STATUS_READY)
    {
        gpon_dev_deInitialize(g_gponmac_drv->dev);
        g_gponmac_drv->status = GPON_DRV_STATUS_INIT;
    }

    GPON_OS_Free(g_gponmac_drv->dev);
    g_gponmac_drv->dev = NULL;

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return RT_ERR_OK;
}

int32 dal_apollomp_gpon_eventHandler_stateChange_reg(rtk_gpon_eventHandleFunc_stateChange_t func)
{
    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);
    g_gponmac_drv->dev->state_change_callback = func;
    GPON_OS_Unlock(g_gponmac_drv->lock);

    return RT_ERR_OK;
}

int32 dal_apollomp_gpon_eventHandler_dsFecChange_reg(rtk_gpon_eventHandleFunc_fecChange_t func)
{
    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);
    g_gponmac_drv->dev->dsfec_change_callback = func;
    GPON_OS_Unlock(g_gponmac_drv->lock);

    return RT_ERR_OK;
}

int32 dal_apollomp_gpon_eventHandler_usFecChange_reg(rtk_gpon_eventHandleFunc_fecChange_t func)
{
    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);
    g_gponmac_drv->dev->usfec_change_callback = func;
    GPON_OS_Unlock(g_gponmac_drv->lock);

    return RT_ERR_OK;
}

int32 dal_apollomp_gpon_eventHandler_usPloamUrgEmpty_reg(rtk_gpon_eventHandleFunc_usPloamEmpty_t func)
{
    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);
    g_gponmac_drv->dev->usploam_urg_epty_callback = func;
    GPON_OS_Unlock(g_gponmac_drv->lock);

    return RT_ERR_OK;
}

int32 dal_apollomp_gpon_eventHandler_usPloamNrmEmpty_reg(rtk_gpon_eventHandleFunc_usPloamEmpty_t func)
{
    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);
    g_gponmac_drv->dev->usploam_nrm_epty_callback = func;
    GPON_OS_Unlock(g_gponmac_drv->lock);

    return RT_ERR_OK;
}

int32 dal_apollomp_gpon_eventHandler_ploam_reg(rtk_gpon_eventHandleFunc_ploam_t func)
{
    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);
    g_gponmac_drv->dev->ploam_callback = func;
    GPON_OS_Unlock(g_gponmac_drv->lock);

    return RT_ERR_OK;
}

int32 dal_apollomp_gpon_eventHandler_omci_reg(rtk_gpon_eventHandleFunc_omci_t func)
{
    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);
    g_gponmac_drv->dev->omci_callback = func;
    GPON_OS_Unlock(g_gponmac_drv->lock);

    return RT_ERR_OK;
}

int32 dal_apollomp_gpon_callback_queryAesKey_reg(rtk_gpon_callbackFunc_queryAesKey_t func)
{
    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);
    g_gponmac_drv->dev->aeskey_callback = func;
    GPON_OS_Unlock(g_gponmac_drv->lock);

    return RT_ERR_OK;
}

int32 dal_apollomp_gpon_eventHandler_alarm_reg(rtk_gpon_alarm_type_t alarmType, rtk_gpon_eventHandleFunc_fault_t func)
{
    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    if(alarmType>=RTK_GPON_ALARM_MAX)
    {
        return RT_ERR_OUT_OF_RANGE;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);
    g_gponmac_drv->dev->fault_callback[alarmType] = func;
    GPON_OS_Unlock(g_gponmac_drv->lock);

    return RT_ERR_OK;
}

int32 dal_apollomp_gpon_serialNumber_set(rtk_gpon_serialNumber_t *sn)
{
    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    if(g_gponmac_drv->status==GPON_DRV_STATUS_ACTIVATE)
    {
        return RT_ERR_GPON_ONU_ALREADY_ACTIVATED;
    }

    if(!sn)
    {
        return RT_ERR_NULL_POINTER;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    gpon_dev_sn_set(g_gponmac_drv->dev,sn);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return RT_ERR_OK;
}

int32 dal_apollomp_gpon_serialNumber_get(rtk_gpon_serialNumber_t *sn)
{
    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!sn)
    {
        return RT_ERR_NULL_POINTER;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    gpon_dev_sn_get(g_gponmac_drv->dev,sn);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return RT_ERR_OK;
}

int32 dal_apollomp_gpon_password_set(rtk_gpon_password_t *pwd)
{
    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    if(g_gponmac_drv->status==GPON_DRV_STATUS_ACTIVATE)
    {
        return RT_ERR_GPON_ONU_ALREADY_ACTIVATED;
    }

    if(!pwd)
    {
        return RT_ERR_NULL_POINTER;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    gpon_dev_pwd_set(g_gponmac_drv->dev,pwd);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return RT_ERR_OK;
}

int32 dal_apollomp_gpon_password_get(rtk_gpon_password_t *pwd)
{
    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!pwd)
    {
        return RT_ERR_NULL_POINTER;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    gpon_dev_pwd_get(g_gponmac_drv->dev,pwd);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return RT_ERR_OK;
}

int32 dal_apollomp_gpon_parameter_set(rtk_gpon_patameter_type_t type, void *pPara)
{
    if(type>=RTK_GPON_PARA_TYPE_MAX)
    {
        return RT_ERR_OUT_OF_RANGE;
    }

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    if(g_gponmac_drv->status==GPON_DRV_STATUS_ACTIVATE)
    {
        return RT_ERR_GPON_ONU_ALREADY_ACTIVATED;
    }

    if(type>=RTK_GPON_PARA_TYPE_MAX)
    {
        return RT_ERR_OUT_OF_RANGE;
    }

    if(!pPara)
    {
        return RT_ERR_NULL_POINTER;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    switch(type)
    {
        case RTK_GPON_PARA_TYPE_ONU_ACTIVATION:
        {
            gpon_dev_para_onu_set(g_gponmac_drv->dev,(rtk_gpon_onu_activation_para_t*)pPara);
            break;
        }
        case RTK_GPON_PARA_TYPE_LASER:
        {
            gpon_dev_para_laser_set(g_gponmac_drv->dev,(rtk_gpon_laser_para_t*)pPara);
            break;
        }
        case RTK_GPON_PARA_TYPE_DS_PHY:
        {
            gpon_dev_para_dsPhy_set(g_gponmac_drv->dev,(rtk_gpon_ds_physical_para_t*)pPara);
            break;
        }
        case RTK_GPON_PARA_TYPE_DS_PLOAM:
        {
            gpon_dev_para_dsPloam_set(g_gponmac_drv->dev,(rtk_gpon_ds_ploam_para_t*)pPara);
            break;
        }
        case RTK_GPON_PARA_TYPE_DS_BWMAP:
        {
            gpon_dev_para_dsBwmap_set(g_gponmac_drv->dev,(rtk_gpon_ds_bwMap_para_t*)pPara);
            break;
        }
        case RTK_GPON_PARA_TYPE_DS_GEM:
        {
            gpon_dev_para_dsGem_set(g_gponmac_drv->dev,(rtk_gpon_ds_gem_para_t*)pPara);
            break;
        }
        case RTK_GPON_PARA_TYPE_DS_ETH:
        {
            gpon_dev_para_dsEth_set(g_gponmac_drv->dev,(rtk_gpon_ds_eth_para_t*)pPara);
            break;
        }
        case RTK_GPON_PARA_TYPE_DS_OMCI:
        {
            gpon_dev_para_dsOmci_set(g_gponmac_drv->dev,(rtk_gpon_ds_omci_para_t*)pPara);
            break;
        }
#if 0
        case RTK_GPON_PARA_TYPE_DS_TDM:
        {
            gpon_dev_para_dsTdm_set(g_gponmac_drv->dev,(rtk_gpon_ds_tdm_para_t*)pPara);
            break;
        }
#endif
        case RTK_GPON_PARA_TYPE_US_PHY:
        {
            gpon_dev_para_usPhy_set(g_gponmac_drv->dev,(rtk_gpon_us_physical_para_t*)pPara);
            break;
        }
        case RTK_GPON_PARA_TYPE_US_PLOAM:
        {
            gpon_dev_para_usPloam_set(g_gponmac_drv->dev,(rtk_gpon_us_ploam_para_t*)pPara);
            break;
        }
        case RTK_GPON_PARA_TYPE_US_DBR:
        {
            gpon_dev_para_usDbr_set(g_gponmac_drv->dev,(rtk_gpon_us_dbr_para_t*)pPara);
            break;
        }
#if 0 /* page US_FRAG is removed */
        case RTK_GPON_PARA_TYPE_US_GEM:
        {
            gpon_dev_para_usGem_set(g_gponmac_drv->dev,(rtk_gpon_us_gem_para_t*)pPara);
            break;
        }
        case RTK_GPON_PARA_TYPE_US_ETH:
        {
            gpon_dev_para_usEth_set(g_gponmac_drv->dev,(rtk_gpon_us_eth_para_t*)pPara);
            break;
        }
#endif
#if 0 /* GEM US omci register is removed */
        case RTK_GPON_PARA_TYPE_US_OMCI:
        {
            gpon_dev_para_usOmci_set(g_gponmac_drv->dev,(rtk_gpon_us_omci_para_t*)pPara);
            break;
        }
#endif
#if 0 /* GPON_GEM_US_OPT_CFG is removed in GPON_MAC_SWIO_r1.1 */
        case RTK_GPON_PARA_TYPE_US_SAV:
        {
            gpon_dev_para_optPwrSav_set(g_gponmac_drv->dev,(rtk_gpon_us_opt_pwr_sav_t*)pPara);
            break;
        }
#endif
        default:
        {
            break;
        }
    }

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return RT_ERR_OK;
}

int32 dal_apollomp_gpon_parameter_get(rtk_gpon_patameter_type_t type, void *pPara)
{
    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    if(type>=RTK_GPON_PARA_TYPE_MAX)
    {
        return RT_ERR_OUT_OF_RANGE;
    }

    if(!pPara)
    {
        return RT_ERR_NULL_POINTER;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    switch(type)
    {
        case RTK_GPON_PARA_TYPE_ONU_ACTIVATION:
        {
            gpon_dev_para_onu_get(g_gponmac_drv->dev,(rtk_gpon_onu_activation_para_t*)pPara);
            break;
        }
        case RTK_GPON_PARA_TYPE_LASER:
        {
            gpon_dev_para_laser_get(g_gponmac_drv->dev,(rtk_gpon_laser_para_t*)pPara);
            break;
        }
        case RTK_GPON_PARA_TYPE_DS_PHY:
        {
            gpon_dev_para_dsPhy_get(g_gponmac_drv->dev,(rtk_gpon_ds_physical_para_t*)pPara);
            break;
        }
        case RTK_GPON_PARA_TYPE_DS_PLOAM:
        {
            gpon_dev_para_dsPloam_get(g_gponmac_drv->dev,(rtk_gpon_ds_ploam_para_t*)pPara);
            break;
        }
        case RTK_GPON_PARA_TYPE_DS_BWMAP:
        {
            gpon_dev_para_dsBwmap_get(g_gponmac_drv->dev,(rtk_gpon_ds_bwMap_para_t*)pPara);
            break;
        }
        case RTK_GPON_PARA_TYPE_DS_GEM:
        {
            gpon_dev_para_dsGem_get(g_gponmac_drv->dev,(rtk_gpon_ds_gem_para_t*)pPara);
            break;
        }
        case RTK_GPON_PARA_TYPE_DS_ETH:
        {
            gpon_dev_para_dsEth_get(g_gponmac_drv->dev,(rtk_gpon_ds_eth_para_t*)pPara);
            break;
        }
        case RTK_GPON_PARA_TYPE_DS_OMCI:
        {
            gpon_dev_para_dsOmci_get(g_gponmac_drv->dev,(rtk_gpon_ds_omci_para_t*)pPara);
            break;
        }
#if 0
        case RTK_GPON_PARA_TYPE_DS_TDM:
        {
            gpon_dev_para_dsTdm_get(g_gponmac_drv->dev,(rtk_gpon_ds_tdm_para_t*)pPara);
            break;
        }
#endif
        case RTK_GPON_PARA_TYPE_US_PHY:
        {
            gpon_dev_para_usPhy_get(g_gponmac_drv->dev,(rtk_gpon_us_physical_para_t*)pPara);
            break;
        }
        case RTK_GPON_PARA_TYPE_US_PLOAM:
        {
            gpon_dev_para_usPloam_get(g_gponmac_drv->dev,(rtk_gpon_us_ploam_para_t*)pPara);
            break;
        }
        case RTK_GPON_PARA_TYPE_US_DBR:
        {
            gpon_dev_para_usDbr_get(g_gponmac_drv->dev,(rtk_gpon_us_dbr_para_t*)pPara);
            break;
        }
        case RTK_GPON_PARA_TYPE_ONUID:
        {
            gpon_dev_para_onuId_get(g_gponmac_drv->dev,(uint8*)pPara);
            break;
        }
        case RTK_GPON_PARA_TYPE_DRV_STATUS:
        {
            gpon_dev_para_drvStatus_get(g_gponmac_drv,(gpon_drv_status_t*)pPara);
            break;
        }
#if 0 /* page US_FRAG is removed */
        case RTK_GPON_PARA_TYPE_US_GEM:
        {
            gpon_dev_para_usGem_get(g_gponmac_drv->dev,(rtk_gpon_us_gem_para_t*)pPara);
            break;
        }
        case RTK_GPON_PARA_TYPE_US_ETH:
        {
            gpon_dev_para_usEth_get(g_gponmac_drv->dev,(rtk_gpon_us_eth_para_t*)pPara);
            break;
        }
#endif
#if 0 /* GEM US omci register is removed */
        case RTK_GPON_PARA_TYPE_US_OMCI:
        {
            gpon_dev_para_usOmci_get(g_gponmac_drv->dev,(rtk_gpon_us_omci_para_t*)pPara);
            break;
        }
#endif
#if 0 /* GPON_GEM_US_OPT_CFG is removed in GPON_MAC_SWIO_r1.1 */
        case RTK_GPON_PARA_TYPE_US_SAV:
        {
            gpon_dev_para_optPwrSav_get(g_gponmac_drv->dev,(rtk_gpon_us_opt_pwr_sav_t*)pPara);
            break;
        }
#endif
        default:
        {
            break;
        }
    }

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return RT_ERR_OK;
}

int32 dal_apollomp_gpon_activate(rtk_gpon_initialState_t initState)
{
    int ret ;
    uint32 data;
    apollomp_raw_port_ability_t mac_ability;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    if(g_gponmac_drv->status==GPON_DRV_STATUS_ACTIVATE)
    {
        return RT_ERR_GPON_ONU_ALREADY_ACTIVATED;
    }

    if(initState>RTK_GPONMAC_INIT_STATE_O7)
    {
        return RT_ERR_OUT_OF_RANGE;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    /*drain-out default queue first*/
    if((ret = apollomp_raw_ponMacQueueDrainOutState_set(127))!=RT_ERR_OK)
    {
    	 RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
        return ret;
    }
    if((ret = apollomp_raw_ponMacQueueDrainOutState_set(0))!=RT_ERR_OK)
    {
    	 RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
        return ret;
    }
    /* PON port set as force mode */
    {
        osal_memset(&mac_ability, 0x00, sizeof(apollomp_raw_port_ability_t));
        mac_ability.speed           = PORT_SPEED_1000M;
        mac_ability.duplex          = PORT_FULL_DUPLEX;
        mac_ability.linkFib1g       = DISABLED;
        mac_ability.linkStatus      = PORT_LINKDOWN;
        mac_ability.txFc            = DISABLED;
        mac_ability.rxFc            = DISABLED;
        mac_ability.nwayAbility     = DISABLED;
        mac_ability.masterMod       = DISABLED;
        mac_ability.nwayFault       = DISABLED;
        mac_ability.lpi_100m        = DISABLED;
        mac_ability.lpi_giga        = DISABLED;
        if((ret = apollomp_raw_port_ForceAbility_set(HAL_GET_PON_PORT(), &mac_ability)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
            return ret;
        }

        mac_ability.linkStatus      = PORT_LINKUP;
        if((ret = apollomp_raw_port_ForceAbility_set(HAL_GET_PON_PORT(), &mac_ability)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
            return ret;
        }

        if((ret = reg_field_read(APOLLOMP_ABLTY_FORCE_MODEr, APOLLOMP_ABLTY_FORCE_MODEf, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
            return ret;
        }

        data |= (0x01 << HAL_GET_PON_PORT());

        if((ret = reg_field_write(APOLLOMP_ABLTY_FORCE_MODEr, APOLLOMP_ABLTY_FORCE_MODEf, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
            return ret;
        }
    }

    g_gponmac_drv->status = GPON_DRV_STATUS_ACTIVATE;
    gpon_dev_activate(g_gponmac_drv->dev,initState);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return RT_ERR_OK;
}

int32 dal_apollomp_gpon_deActivate(void)
{
    int ret ;
    uint32 data;
    apollomp_raw_port_ability_t mac_ability;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    if(g_gponmac_drv->status==GPON_DRV_STATUS_READY)
    {
        return RT_ERR_GPON_ONU_NOT_ACTIVATED;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    /*drain-out default queue first*/
    if((ret = apollomp_raw_ponMacQueueDrainOutState_set(127))!=RT_ERR_OK)
    {
    	 RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
        return ret;
    }
    if((ret = apollomp_raw_ponMacQueueDrainOutState_set(0))!=RT_ERR_OK)
    {
    	 RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
        return ret;
    }
    /* PON port set as force mode */
    {
        osal_memset(&mac_ability, 0x00, sizeof(apollomp_raw_port_ability_t));
        if((ret = apollomp_raw_port_ForceAbility_get(HAL_GET_PON_PORT(), &mac_ability)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
            return ret;
        }

        mac_ability.linkStatus      = PORT_LINKDOWN;
        if((ret = apollomp_raw_port_ForceAbility_set(HAL_GET_PON_PORT(), &mac_ability)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
            return ret;
        }

        if((ret = reg_field_read(APOLLOMP_ABLTY_FORCE_MODEr, APOLLOMP_ABLTY_FORCE_MODEf, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
            return ret;
        }

        data |= (0x01 << HAL_GET_PON_PORT());

        if((ret = reg_field_write(APOLLOMP_ABLTY_FORCE_MODEr, APOLLOMP_ABLTY_FORCE_MODEf, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
            return ret;
        }
    }

    gpon_dev_deactivate(g_gponmac_drv->dev);
    g_gponmac_drv->status = GPON_DRV_STATUS_READY;

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return RT_ERR_OK;
}

/*
 * GPON MAC ISR entry
 * Should be called in interrupt process or a polling thread
 */
void dal_apollomp_gpon_isr_entry(void)
{
    gpon_isr_entry();
}

int32 dal_apollomp_gpon_ponStatus_get(rtk_gpon_fsm_status_t* status)
{
    if(!status)
    {
        return RT_ERR_NULL_POINTER;
    }

    *status = RTK_GPONMAC_FSM_STATE_UNKNOWN;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    if(g_gponmac_drv->status==GPON_DRV_STATUS_READY)
    {
        return RT_ERR_GPON_ONU_NOT_ACTIVATED;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    gpon_dev_get_status(g_gponmac_drv->dev,status);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return RT_ERR_OK;
}

int32 dal_apollomp_gpon_tcont_create(rtk_gpon_tcont_ind_t* ind, rtk_gpon_tcont_attr_t* attr)
{
    int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    if((!ind)||(!attr))
    {
        return RT_ERR_NULL_POINTER;
    }

    if(ind->alloc_id>GPON_DEV_MAX_ALLOC_ID)
    {
        return RT_ERR_OUT_OF_RANGE;
    }

    if(ind->type>RTK_GPON_TCONT_TYPE_5 || ind->type<RTK_GPON_TCONT_TYPE_1)
    {
        return RT_ERR_OUT_OF_RANGE;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_dev_tcont_physical_add(g_gponmac_drv->dev,ind->alloc_id);
    if(ret == RT_ERR_OK)
    ret = gpon_dev_tcont_logical_add(g_gponmac_drv->dev,ind,attr);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}

int32 dal_apollomp_gpon_tcont_destroy(rtk_gpon_tcont_ind_t* ind)
{
    int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    if(!ind)
    {
        return RT_ERR_NULL_POINTER;
    }

    if(ind->alloc_id>GPON_DEV_MAX_ALLOC_ID)
    {
        return RT_ERR_OUT_OF_RANGE;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_dev_tcont_logical_del(g_gponmac_drv->dev,ind);
    ret = gpon_dev_tcont_physical_del(g_gponmac_drv->dev,ind->alloc_id);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}

int32 dal_apollomp_gpon_tcont_get(rtk_gpon_tcont_ind_t* ind, rtk_gpon_tcont_attr_t* attr)
{
    int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    if((!ind)||(!attr))
    {
        return RT_ERR_NULL_POINTER;
    }

    if(ind->alloc_id>GPON_DEV_MAX_ALLOC_ID)
    {
        return RT_ERR_OUT_OF_RANGE;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_dev_tcont_logical_get(g_gponmac_drv->dev,ind,attr);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}

int32 dal_apollomp_gpon_dsFlow_set(uint32 flowId, rtk_gpon_dsFlow_attr_t* attr)
{
    int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    if(!attr)
    {
        return RT_ERR_NULL_POINTER;
    }

    if(flowId>=GPON_DEV_MAX_FLOW_NUM)
    {
        return RT_ERR_OUT_OF_RANGE;
    }

    if(attr->gem_port_id>GPON_DEV_MAX_GEMPORT_ID && attr->gem_port_id!=RTK_GPON_GEMPORT_ID_NOUSE)
    {
        return RT_ERR_OUT_OF_RANGE;
    }
    else if(attr->gem_port_id<=GPON_DEV_MAX_GEMPORT_ID)
    {
        if(attr->type>RTK_GPON_FLOW_TYPE_TDM)
        {
            return RT_ERR_OUT_OF_RANGE;
        }
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    if(attr->gem_port_id!=RTK_GPON_GEMPORT_ID_NOUSE)
    {
        ret = gpon_dev_dsFlow_add(g_gponmac_drv->dev,flowId,attr);
    }
    else
    {
        ret = gpon_dev_dsFlow_del(g_gponmac_drv->dev,flowId);
    }

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}

int32 dal_apollomp_gpon_dsFlow_get(uint32 flowId, rtk_gpon_dsFlow_attr_t* attr)
{
    int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    if(!attr)
    {
        return RT_ERR_NULL_POINTER;
    }

    if(flowId>=GPON_DEV_MAX_FLOW_NUM)
    {
        return RT_ERR_OUT_OF_RANGE;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_dev_dsFlow_get(g_gponmac_drv->dev,flowId, attr);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}

int32 dal_apollomp_gpon_usFlow_set(uint32 flowId, rtk_gpon_usFlow_attr_t* attr)
{
    int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    if(!attr)
    {
        return RT_ERR_NULL_POINTER;
    }

    if(flowId>=GPON_DEV_MAX_FLOW_NUM)
    {
        return RT_ERR_OUT_OF_RANGE;
    }

    if(attr->gem_port_id>GPON_DEV_MAX_GEMPORT_ID && attr->gem_port_id!=RTK_GPON_GEMPORT_ID_NOUSE)
    {
        return RT_ERR_OUT_OF_RANGE;
    }
    else if(attr->gem_port_id<=GPON_DEV_MAX_GEMPORT_ID)
    {
        if(attr->type>RTK_GPON_FLOW_TYPE_TDM)
        {
            return RT_ERR_OUT_OF_RANGE;
        }

        if(attr->tcont_id>=GPON_DEV_MAX_TCONT_NUM)
        {
            return RT_ERR_OUT_OF_RANGE;
        }

        if(attr->type==RTK_GPON_FLOW_TYPE_TDM)
        {
            if(attr->channel>=GPON_DEV_MAX_TDM_CHANNEL_NUM)
            {
                return RT_ERR_OUT_OF_RANGE;
            }
        }
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    if(attr->gem_port_id!=RTK_GPON_GEMPORT_ID_NOUSE)
    {
        ret = gpon_dev_usFlow_add(g_gponmac_drv->dev,flowId,attr);
    }
    else
    {
        ret = gpon_dev_usFlow_del(g_gponmac_drv->dev,flowId);
    }

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}

int32 dal_apollomp_gpon_usFlow_get(uint32 flowId, rtk_gpon_usFlow_attr_t* attr)
{
    int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_OUT_OF_RANGE;
    }

    if(!attr)
    {
        return RT_ERR_NULL_POINTER;
    }

    if(flowId>=GPON_DEV_MAX_FLOW_NUM)
    {
        return RT_ERR_OUT_OF_RANGE;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_dev_usFlow_get(g_gponmac_drv->dev,flowId,attr);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}

int32 dal_apollomp_gpon_ploam_send(int32 urgent, rtk_gpon_ploam_t* ploam)
{
    int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    if(!ploam)
    {
        return RT_ERR_NULL_POINTER;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_ploam_tx(g_gponmac_drv->dev,urgent,ploam);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}

#if 0 /* The feature is not support now. */
int32 dal_apollomp_gpon_omci_send(int32        urgent, rtk_gpon_omci_msg_t* omci)
{
    int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    if(!omci)
    {
        return RT_ERR_NULL_POINTER;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_dev_omci_tx(g_gponmac_drv->dev,urgent,omci);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}
#endif

int32 dal_apollomp_gpon_broadcastPass_set(int32 mode)
{
    int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_dev_bcastPass_set(g_gponmac_drv->dev,mode);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}

int32 dal_apollomp_gpon_broadcastPass_get(int32 *mode)
{
    int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    if(!mode)
    {
        return RT_ERR_NULL_POINTER;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_dev_bcastPass_get(g_gponmac_drv->dev, mode);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}

int32 dal_apollomp_gpon_nonMcastPass_set(int32 mode)
{
    int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_dev_nonMcastPass_set(g_gponmac_drv->dev,mode);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}

int32 dal_apollomp_gpon_nonMcastPass_get(int32 *mode)
{
    int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    if(!mode)
    {
        return RT_ERR_NULL_POINTER;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_dev_nonMcastPass_get(g_gponmac_drv->dev, mode);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}

#if 0 /* wellknown addr is removed in GPON_MAC_SWIO_r1.1 */
int32 dal_apollomp_gpon_wellKnownAddr_set(int32 mode, uint32 addr)
{
    int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_dev_wellKnownFwdAddr_set(g_gponmac_drv->dev,mode,addr);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}

int32 dal_apollomp_gpon_wellKnownAddr_get(int32       * mode, uint32* addr)
{
    int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    if(!mode || !addr)
    {
        return RT_ERR_NULL_POINTER;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_dev_wellKnownFwdAddr_get(g_gponmac_drv->dev,mode,addr);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}
#endif

int32 dal_apollomp_gpon_multicastAddrCheck_set(uint32 ipv4_pattern, uint32 ipv6_pattern)
{
    int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_dev_mcastCheck_set(g_gponmac_drv->dev,ipv4_pattern,ipv6_pattern);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}

int32 dal_apollomp_gpon_multicastAddrCheck_get(uint32* ipv4_pattern, uint32* ipv6_pattern)
{
    int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    if(!ipv4_pattern || !ipv6_pattern)
    {
        return RT_ERR_NULL_POINTER;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_dev_mcastCheck_get(g_gponmac_drv->dev,ipv4_pattern,ipv6_pattern);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}

int32 dal_apollomp_gpon_macFilterMode_set(rtk_gpon_macTable_exclude_mode_t mode)
{
    int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    if(mode>RTK_GPON_MACTBL_MODE_EXCLUDE)
    {
        return RT_ERR_OUT_OF_RANGE;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_dev_macFilterMode_set(g_gponmac_drv->dev,mode);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}

int32 dal_apollomp_gpon_macFilterMode_get(rtk_gpon_macTable_exclude_mode_t* mode)
{
    int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    if(!mode)
    {
        return RT_ERR_NULL_POINTER;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_dev_macFilterMode_get(g_gponmac_drv->dev, mode);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}

int32 dal_apollomp_gpon_mcForceMode_set(rtk_gpon_mc_force_mode_t ipv4, rtk_gpon_mc_force_mode_t ipv6)
{
    int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_dev_mcForceMode_set(g_gponmac_drv->dev,ipv4,ipv6);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}

int32 dal_apollomp_gpon_mcForceMode_get(rtk_gpon_mc_force_mode_t *ipv4, rtk_gpon_mc_force_mode_t *ipv6)
{
    int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_dev_mcForceMode_get(g_gponmac_drv->dev, ipv4, ipv6);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}

int32 dal_apollomp_gpon_macEntry_add(rtk_gpon_macTable_entry_t* entry)
{
    int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    if(!entry)
    {
        return RT_ERR_NULL_POINTER;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_dev_macTableEntry_add(g_gponmac_drv->dev,entry);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}

int32 dal_apollomp_gpon_macEntry_del(rtk_gpon_macTable_entry_t* entry)
{
    int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    if(!entry)
    {
        return RT_ERR_NULL_POINTER;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_dev_macTableEntry_del(g_gponmac_drv->dev,entry);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}

int32 dal_apollomp_gpon_macEntry_get(uint32 index, rtk_gpon_macTable_entry_t* entry)
{
    int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    if(!entry)
    {
        return RT_ERR_NULL_POINTER;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_dev_macTableEntry_get(g_gponmac_drv->dev,index,entry);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}

#if 0 /* page GEM_FCB is removed */
int32 dal_apollomp_gpon_chunkHeader_set(rtk_gpon_chunkHeader_t* chunk)
{
    int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    if(!chunk)
    {
        return RT_ERR_NULL_POINTER;
    }

    if(chunk->head_size>0x07)
    {
        return RT_ERR_OUT_OF_RANGE;
    }

    if(chunk->task_ind>0x0F)
    {
        return RT_ERR_OUT_OF_RANGE;
    }

    if(chunk->err_code>0x0F)
    {
        return RT_ERR_OUT_OF_RANGE;
    }

    if(chunk->src>0x0F)
    {
        return RT_ERR_OUT_OF_RANGE;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_dev_chunk_set(g_gponmac_drv->dev,chunk);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}

int32 dal_apollomp_gpon_chunkHeader_get(rtk_gpon_chunkHeader_t* chunk)
{
    int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    if(!chunk)
    {
        return RT_ERR_NULL_POINTER;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_dev_chunk_get(g_gponmac_drv->dev,chunk);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}

int32 dal_apollomp_gpon_gemFcbShaper_set(rtk_gpon_shaper_t* shaper)
{
    int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    if(!shaper)
    {
        return RT_ERR_NULL_POINTER;
    }

    if(shaper->inc_size>0xFF)
    {
        return RT_ERR_OUT_OF_RANGE;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_dev_shaper_set(g_gponmac_drv->dev,shaper);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}

int32 dal_apollomp_gpon_gemFcbShaper_get(rtk_gpon_shaper_t* shaper)
{
    int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    if(!shaper)
    {
        return RT_ERR_NULL_POINTER;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_dev_shaper_get(g_gponmac_drv->dev,shaper);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}

int32 dal_apollomp_gpon_gemFcbReqEn_set(int32        enable)
{
    int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_dev_reqEn_set(g_gponmac_drv->dev,enable);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}

int32 dal_apollomp_gpon_gemFcbReqEn_get(int32       * enable)
{
    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    if(!enable)
    {
        return RT_ERR_NULL_POINTER;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    *enable = gpon_dev_reqEn_get(g_gponmac_drv->dev);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return RT_ERR_OK;
}
#endif /* page GEM_FCB is removed */

int32 dal_apollomp_gpon_rdi_set(int32        enable)
{
    int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_dev_rdi_set(g_gponmac_drv->dev,enable);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}

int32 dal_apollomp_gpon_rdi_get(int32       * enable)
{
    int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    if(!enable)
    {
        return RT_ERR_NULL_POINTER;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_dev_rdi_get(g_gponmac_drv->dev, enable);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}

#if 0 /* The feature is removed */
int32 dal_apollomp_gpon_tdm_set(int32        enable)
{
    int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_dev_tdmEn_set(g_gponmac_drv->dev,enable);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}

int32 dal_apollomp_gpon_tdm_get(int32       * enable)
{
    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    if(!enable)
    {
        return RT_ERR_NULL_POINTER;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    *enable = gpon_dev_tdmEn_get(g_gponmac_drv->dev);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return RT_ERR_OK;
}
#endif

int32 dal_apollomp_gpon_powerLevel_set(uint8 level)
{
    int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_dev_powerLevel_set(g_gponmac_drv->dev,level);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}

int32 dal_apollomp_gpon_powerLevel_get(uint8 *level)
{
    int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    if(!level)
    {
        return RT_ERR_NULL_POINTER;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_dev_powerLevel_get(g_gponmac_drv->dev, level);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}

#if 0 /* the feature is removed */
int32 dal_apollomp_gpon_plouDbr_set(uint8 plou)
{
    int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_dev_dbruPlou_send(g_gponmac_drv->dev,plou);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}

int32 dal_apollomp_gpon_piggybackDBR_set(uint32 idx, rtk_gpon_piggyback_dbr_data_t *value)
{
    int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    if(idx>=GPON_DEV_MAX_TCONT_NUM)
    {
        return RT_ERR_OUT_OF_RANGE;
    }

    if(!value)
    {
        return RT_ERR_NULL_POINTER;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_dev_dbruPiggyback_send(g_gponmac_drv->dev,idx,value);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}

int32 dal_apollomp_gpon_wholeDBR_set(CIG_U16BIT len, uint8 *value)
{
    int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    if(!value)
    {
        return RT_ERR_NULL_POINTER;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_dev_dbruWholeDba_send(g_gponmac_drv->dev,len,value);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}
#endif /* the feature is removed */

int32 dal_apollomp_gpon_alarmStatus_get(rtk_gpon_alarm_type_t alarm, int32 *pStatus)
{
    int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    if(alarm>=RTK_GPON_ALARM_MAX)
    {
        return RT_ERR_OUT_OF_RANGE;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_alarm_status_get(g_gponmac_drv->dev,alarm,pStatus);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}

int32 dal_apollomp_gpon_globalCounter_get (rtk_gpon_global_performance_type_t type, rtk_gpon_global_counter_t *pPara)
{
    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    if(type>=RTK_GPON_PMTYPE_MAX)
    {
        return RT_ERR_OUT_OF_RANGE;
    }

    if(!pPara)
    {
        return RT_ERR_NULL_POINTER;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    switch(type)
    {
        case RTK_GPON_PMTYPE_ONU_ACTIVATION:
        {
            gpon_pm_onuActivation_get(g_gponmac_drv->dev,&pPara->activate);
            break;
        }
        case RTK_GPON_PMTYPE_DS_PHY:
        {
            gpon_pm_dsPhy_get(g_gponmac_drv->dev,&pPara->dsphy);
            break;
        }
        case RTK_GPON_PMTYPE_DS_PLOAM:
        {
            gpon_pm_dsPloam_get(g_gponmac_drv->dev,&pPara->dsploam);
            break;
        }
        case RTK_GPON_PMTYPE_DS_BWMAP:
        {
            gpon_pm_dsBwMap_get(g_gponmac_drv->dev,&pPara->dsbwmap);
            break;
        }
        case RTK_GPON_PMTYPE_DS_GEM:
        {
            gpon_pm_dsGem_get(g_gponmac_drv->dev,&pPara->dsgem);
            break;
        }
        case RTK_GPON_PMTYPE_DS_ETH:
        {
            gpon_pm_dsEth_get(g_gponmac_drv->dev,&pPara->dseth);
            break;
        }
        case RTK_GPON_PMTYPE_DS_OMCI:
        {
            gpon_pm_dsOmci_get(g_gponmac_drv->dev,&pPara->dsomci);
            break;
        }
        case RTK_GPON_PMTYPE_US_PHY:
        {
            gpon_pm_usPhy_get(g_gponmac_drv->dev,&pPara->usphy);
            break;
        }
        case RTK_GPON_PMTYPE_US_DBR:
        {
            gpon_pm_usDbr_get(g_gponmac_drv->dev,&pPara->usdbr);
            break;
        }
        case RTK_GPON_PMTYPE_US_PLOAM:
        {
            gpon_pm_usPloam_get(g_gponmac_drv->dev,&pPara->usploam);
            break;
        }
        case RTK_GPON_PMTYPE_US_GEM:
        {
            gpon_pm_usGem_get(g_gponmac_drv->dev,&pPara->usgem);
            break;
        }
        case RTK_GPON_PMTYPE_US_ETH:
        {
            gpon_pm_usEth_get(g_gponmac_drv->dev,&pPara->useth);
            break;
        }
        case RTK_GPON_PMTYPE_US_OMCI:
        {
            gpon_pm_usOmci_get(g_gponmac_drv->dev,&pPara->usomci);
            break;
        }
        default:
            break;
    }

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return RT_ERR_OK;
}

int32 dal_apollomp_gpon_tcontCounter_get (uint32 tcontId, rtk_gpon_tcont_performance_type_t type, rtk_gpon_tcont_counter_t *pPara)
{
    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    if(type>=RTK_GPON_PMTYPE_TCONT_MAX)
    {
        return RT_ERR_OUT_OF_RANGE;
    }

    if(!pPara)
    {
        return RT_ERR_NULL_POINTER;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    switch(type)
    {
        case RTK_GPON_PMTYPE_TCONT_GEM:
        {
            gpon_pm_tcontGem_get(g_gponmac_drv->dev, tcontId,&pPara->gem);
            break;
        }
        case RTK_GPON_PMTYPE_TCONT_ETH:
        {
            gpon_pm_tcontEth_get(g_gponmac_drv->dev, tcontId,&pPara->eth);
            break;
        }
        case RTK_GPON_PMTYPE_TCONT_IDLE:
        {
            gpon_pm_tcontIdle_get(g_gponmac_drv->dev, tcontId,&pPara->idle);
            break;
        }
        default:
            break;
    }

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return RT_ERR_OK;
}

int32 dal_apollomp_gpon_flowCounter_get (uint32 flowId, rtk_gpon_flow_performance_type_t type, rtk_gpon_flow_counter_t *pPara)
{
    int32 rsl = RT_ERR_OK;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    if(type>=RTK_GPON_PMTYPE_FLOW_MAX)
    {
        return RT_ERR_OUT_OF_RANGE;
    }

    if(!pPara)
    {
        return RT_ERR_NULL_POINTER;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    switch(type)
    {
        case RTK_GPON_PMTYPE_FLOW_DS_GEM:
        {
            rsl = gpon_pm_dsFlowGem_get(g_gponmac_drv->dev, flowId, &pPara->dsgem);
            break;
        }
        case RTK_GPON_PMTYPE_FLOW_DS_ETH:
        {
            rsl = gpon_pm_dsFlowEth_get(g_gponmac_drv->dev, flowId, &pPara->dseth);
            break;
        }
        case RTK_GPON_PMTYPE_FLOW_US_GEM:
        {
            rsl = gpon_pm_usFlowGem_get(g_gponmac_drv->dev, flowId, &pPara->usgem);
            break;
        }
        case RTK_GPON_PMTYPE_FLOW_US_ETH:
        {
            rsl = gpon_pm_usFlowEth_get(g_gponmac_drv->dev, flowId, &pPara->useth);
            break;
        }
        default:
            break;
    }

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return rsl;
}

int32 dal_apollomp_gpon_version_get(rtk_gpon_device_ver_t* hver, rtk_gpon_driver_ver_t* sver)
{
#if 0 /* design_id is removed in GPON_MAC_SWIO_v1.1 */
    hver->design_id = 0xFF;
#endif
    hver->gmac_ver  = 0xFF;

    sver->release   = 0xFF;
    sver->load      = 0xFF;
    sver->build     = 0xFFFF;

    if(g_gponmac_drv)
    {
        sver->release   = GPON_DRIVER_SW_VERSION_RELEASE;
        sver->load      = GPON_DRIVER_SW_VERSION_LOAD;
        sver->build     = GPON_DRIVER_SW_VERSION_BUILD;

        if(g_gponmac_drv->dev)
        {
            GPON_OS_Lock(g_gponmac_drv->lock);
            gpon_dev_versoin_get(g_gponmac_drv->dev,hver);
            GPON_OS_Unlock(g_gponmac_drv->lock);
        }
    }

    return RT_ERR_OK;
}


#if 0 /* gemloop is removed in GPON_MAC_SWIO_v1.1 */
int32 dal_apollomp_gpon_gemLoop_set(int32 loop)
{
    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    gpon_dbg_gemLoop_set(g_gponmac_drv->dev,loop);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return RT_ERR_OK;
}

int32 dal_apollomp_gpon_gemLoop_get(int32 *pLoop)
{
    int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    if(!pLoop)
    {
        return RT_ERR_NULL_POINTER;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_dbg_gemLoop_get(g_gponmac_drv->dev, pLoop);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}
#endif
#if 0 /* the feature is removed */
int32 dal_apollomp_gpon_gemPktDropHis_get(rtk_gpon_dropHisStatus_t* history)
{
    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    if(!history)
    {
        return RT_ERR_NULL_POINTER;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    GMac_Dbg_get_gemport_drop_his(g_gponmac_drv->dev,history);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return RT_ERR_OK;
}
#endif

int32 dal_apollomp_gpon_txForceLaser_set(rtk_gpon_laser_status_t status)
{
    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    if(status>RTK_GPON_LASER_STATUS_FORCE_OFF)
    {
        return RT_ERR_OUT_OF_RANGE;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    gpon_dbg_forceLaser_set(g_gponmac_drv->dev,status);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return RT_ERR_OK;
}

int32 dal_apollomp_gpon_txForceLaser_get(rtk_gpon_laser_status_t *pStatus)
{
    int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    if(!pStatus)
    {
        return RT_ERR_NULL_POINTER;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_dbg_forceLaser_get(g_gponmac_drv->dev,pStatus);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}

int32 dal_apollomp_gpon_txForceIdle_set(int32 on)
{
    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    gpon_dbg_forceIdle_set(g_gponmac_drv->dev,on);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return RT_ERR_OK;
}

int32 dal_apollomp_gpon_txForceIdle_get(int32 *pOn)
{
    int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    if(!pOn)
    {
        return RT_ERR_NULL_POINTER;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_dbg_forceIdle_get(g_gponmac_drv->dev, pOn);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}

#if 0
int32 dal_apollomp_gpon_txForcePRBS_set(int32 on)
{
    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    gpon_dbg_forcePrbs_set(g_gponmac_drv->dev,on);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return RT_ERR_OK;
}

int32 dal_apollomp_gpon_txForcePRBS_get(int32 *pOn)
{
    int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    if(!pOn)
    {
        return RT_ERR_NULL_POINTER;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_dbg_forcePrbs_get(g_gponmac_drv->dev,pOn);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}
#endif

int32 dal_apollomp_gpon_dsFecSts_get(int32 *pEn)
{
    int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    if(!pEn)
    {
        return RT_ERR_NULL_POINTER;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_dbg_dsFec_get(g_gponmac_drv->dev,pEn);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}

void dal_apollomp_gpon_version_show(void)
{
    rtk_gpon_device_ver_t hver;
#if defined(FPGA_DEFINED)
    uint16 date, time, revision;
#endif
    if(!g_gponmac_drv)
    {
        osal_printf("GPON MAC is not started.\r\n");
        return;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        osal_printf("GPON MAC Device is not initialized.\r\n");
        return;
    }
    gpon_dev_versoin_get(g_gponmac_drv->dev,&hver);

    osal_printf("============================================================\r\n");
    osal_printf("    GPON ONU MAC Device Version\r\n");
#if 0 /* design_id is removed in GPON_MAC_SWIO_v1.1 */
    osal_printf("Device    Design: 0x%x\r\n",hver.design_id);
#endif
#if defined(FPGA_DEFINED)
    osal_printf("GPON FPGA date: 0x%04x, time: 0x%04x, revision: 0x%04x\r\n",date, time, revision);
#endif
    osal_printf("GPON MAC Version: 0x%x\r\n",hver.gmac_ver);
    osal_printf("Software Version: %d.%02d.%04d\r\n",GPON_DRIVER_SW_VERSION_RELEASE,GPON_DRIVER_SW_VERSION_LOAD,GPON_DRIVER_SW_VERSION_BUILD);
    osal_printf("Building    Time: %s, %s\r\n",__TIME__,__DATE__);
    osal_printf("============================================================\r\n");
}

void dal_apollomp_gpon_devInfo_show(void)
{
    if(!g_gponmac_drv)
    {
        osal_printf("GPON MAC is not started.\r\n");
        return;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        osal_printf("GPON MAC Device is not initialized.\r\n");
        return;
    }

    osal_printf("============================================================\r\n");
    osal_printf("    GPON ONU MAC Driver Info\r\n");
    osal_printf("Software Base Addr: %p\r\n",g_gponmac_drv);
    osal_printf("Driver Status: %s\r\n",gpon_dbg_drv_status_str(g_gponmac_drv->status));
    osal_printf("Semaphore ID: %p\r\n",(void *)g_gponmac_drv->lock);
    osal_printf("Dev Pointer: %p\r\n",g_gponmac_drv->dev);
    osal_printf("Device Base Addr: 0x%lx\r\n",(long unsigned int)g_gponmac_drv->dev->base_addr);
    osal_printf("Device Timer: %p\r\n",(void *)g_gponmac_drv->dev->timer);
    osal_printf("============================================================\r\n");
}

void dal_apollomp_gpon_gtc_show(void)
{
    uint32 i;
    if(!g_gponmac_drv)
    {
        osal_printf("GPON MAC is not started.\r\n");
        return;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        osal_printf("GPON MAC Device is not initialized.\r\n");
        return;
    }

    osal_printf("============================================================\r\n");
    osal_printf("    GPON ONU MAC GTC Status\r\n");
    osal_printf("GPON MAC Status: %s\r\n",gpon_dbg_fsm_status_str(g_gponmac_drv->dev->status));
    osal_printf("ONU ID: %u\r\n",g_gponmac_drv->dev->onuid);
    osal_printf("Vendor ID: %02x %02x %02x %02x\r\n",g_gponmac_drv->dev->serial_number.vendor[0],g_gponmac_drv->dev->serial_number.vendor[1],g_gponmac_drv->dev->serial_number.vendor[2],g_gponmac_drv->dev->serial_number.vendor[3]);
    osal_printf("Specific: %02x %02x %02x %02x\r\n",g_gponmac_drv->dev->serial_number.specific[0],g_gponmac_drv->dev->serial_number.specific[1],g_gponmac_drv->dev->serial_number.specific[2],g_gponmac_drv->dev->serial_number.specific[3]);
    osal_printf("Password: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\r\n",g_gponmac_drv->dev->password.password[0],g_gponmac_drv->dev->password.password[1],g_gponmac_drv->dev->password.password[2],g_gponmac_drv->dev->password.password[3],
        g_gponmac_drv->dev->password.password[4],g_gponmac_drv->dev->password.password[5],g_gponmac_drv->dev->password.password[6],g_gponmac_drv->dev->password.password[7],g_gponmac_drv->dev->password.password[8],g_gponmac_drv->dev->password.password[9]);
    osal_printf("Power Level: %u\r\n",g_gponmac_drv->dev->power_level);
    osal_printf("TO1 timer: %lu ms, TO2 timer: %lu ms\r\n",(long unsigned int)g_gponmac_drv->dev->parameter.onu.to1_timer,(long unsigned int)g_gponmac_drv->dev->parameter.onu.to2_timer);
    osal_printf("Laser: Optical EN: %d, Optical Polarity: %d, CDR EN: %d, CDR Polarity: %d, LOS Filter %d\r\n",g_gponmac_drv->dev->parameter.laser.laser_optic_los_en,g_gponmac_drv->dev->parameter.laser.laser_optic_los_polar,g_gponmac_drv->dev->parameter.laser.laser_cdr_los_en,g_gponmac_drv->dev->parameter.laser.laser_cdr_los_polar,g_gponmac_drv->dev->parameter.laser.laser_los_filter_en);
    osal_printf("Laser: Laser On: 0x%x, Laser Off: 0x%x\r\n",g_gponmac_drv->dev->parameter.laser.laser_us_on,g_gponmac_drv->dev->parameter.laser.laser_us_off);
    osal_printf("D/S Physical: Scramble EN: %d, FEC Bypass: %d, FEC Thrd: %d\r\n",g_gponmac_drv->dev->parameter.dsphy.ds_scramble_en,g_gponmac_drv->dev->parameter.dsphy.ds_fec_bypass,g_gponmac_drv->dev->parameter.dsphy.ds_fec_thrd);
/* ploam_crc_check is removed in GPON_MAC_SWIO_v1.1 */
    osal_printf("D/S PLOAM: ONU ID  Filter: %d, Broadcast Accept: %d, Drop CRC error %d\r\n",g_gponmac_drv->dev->parameter.dsploam.ds_ploam_onuid_filter,g_gponmac_drv->dev->parameter.dsploam.ds_ploam_broadcast_accpt,g_gponmac_drv->dev->parameter.dsploam.ds_ploam_drop_crc_err);
    osal_printf("D/S BWMAP: CRC Check EN: %d, ONU ID filter EN: %d, PLENd mode: %d\r\n",g_gponmac_drv->dev->parameter.dsbwmap.ds_bwmap_crc_chk,g_gponmac_drv->dev->parameter.dsbwmap.ds_bwmap_onuid_filter,g_gponmac_drv->dev->parameter.dsbwmap.ds_bwmap_plend_mode);
    osal_printf("D/S GEM: Assemble Timer: %d\r\n",g_gponmac_drv->dev->parameter.dsgem.assemble_timer);
    osal_printf("D/S Eth: FCS Check EN: %d, PTI mask: 0x%1x, PTI Pattern: 0x%1x\r\n",g_gponmac_drv->dev->parameter.dseth.ds_eth_crc_chk,g_gponmac_drv->dev->parameter.dseth.ds_eth_pti_mask,g_gponmac_drv->dev->parameter.dseth.ds_eth_pti_ptn);
    osal_printf("D/S OMCI: PTI mask: 0x%1x, PTI Pattern: 0x%1x\r\n",g_gponmac_drv->dev->parameter.dsomci.ds_omci_pti_mask,g_gponmac_drv->dev->parameter.dsomci.ds_omci_pti_ptn);
#if 0 /* TDM PTI is removed in GPON_MAC_SWIO_r1.1 */
    osal_printf("D/S TDM: PTI mask: 0x%1x, PTI Pattern: 0x%1x\r\n",g_gponmac_drv->dev->parameter.dstdm.ds_tdm_pti_mask,g_gponmac_drv->dev->parameter.dstdm.ds_tdm_pti_ptn);
#endif
    osal_printf("U/S Physical: Scramble EN: %d, Burst Enable Polarity: %d, Small SStart: %d, Suppress Laser: %d\r\n",g_gponmac_drv->dev->parameter.usphy.us_scramble_en,g_gponmac_drv->dev->parameter.usphy.us_burst_en_polar,g_gponmac_drv->dev->parameter.usphy.small_sstart_en,g_gponmac_drv->dev->parameter.usphy.suppress_laser_en);
    osal_printf("U/S PLOAM: PLOAMu EN: %d\r\n",g_gponmac_drv->dev->parameter.usploam.us_ploam_en);
    osal_printf("U/S DBR: DBRu EN: %d\r\n",g_gponmac_drv->dev->parameter.usdbr.us_dbru_en);
#if 0 /* page US_FRAG is removed */
    osal_printf("U/S GEM: Max Packet Length: %d\r\n",g_gponmac_drv->dev->parameter.usgem.us_max_pdu);
    osal_printf("U/S Eth: Frag EN: %d, FCS Insert EN: %d\r\n",g_gponmac_drv->dev->parameter.useth.us_frag_en,g_gponmac_drv->dev->parameter.useth.us_eth_fcs_insert_en);
#endif
#if 0 /* GEM US omci register is removed */
    osal_printf("U/S OMCI: Frag EN: %d, PTI: 0x%1x\r\n",g_gponmac_drv->dev->parameter.usomci.us_omci_frag_en,g_gponmac_drv->dev->parameter.usomci.us_omci_pti);
#endif
    osal_printf("Chunk: head size %d, task ind %d, err code 0x%x, src 0x%x\r\n",g_gponmac_drv->dev->chunk_header.head_size,g_gponmac_drv->dev->chunk_header.task_ind,g_gponmac_drv->dev->chunk_header.err_code,g_gponmac_drv->dev->chunk_header.src);
    osal_printf("Shaper: tick %d, inc size %d, thrd high 0x%04x, thrd low 0x%04x\r\n",g_gponmac_drv->dev->shaper.tick,g_gponmac_drv->dev->shaper.inc_size,g_gponmac_drv->dev->shaper.thrd_high,g_gponmac_drv->dev->shaper.thrd_low);
    osal_printf("Mac Table: Broadcast Pass: %d, Non Multicast Pass: %d, Multicast Check Pattern for Ipv4: %08lx, Multicast Check Pattern for Ipv6: %08lx, Multicast Filter Mode: %s\r\n",
                    g_gponmac_drv->dev->mactbl.bc_pass,
                    g_gponmac_drv->dev->mactbl.non_mc_pass,
                    (long unsigned int)g_gponmac_drv->dev->mactbl.ipv4_mc_check_ptn,
                    (long unsigned int)g_gponmac_drv->dev->mactbl.ipv6_mc_check_ptn,
                    gpon_dbg_mac_filter_mode_str(g_gponmac_drv->dev->mactbl.op_mode));
    osal_printf("Request and Data Output En: %d\r\n",g_gponmac_drv->dev->req_en);
    osal_printf("RDI Indicator: %d\r\n",g_gponmac_drv->dev->rdi);
    osal_printf("TDM Module En: %d\r\n",g_gponmac_drv->dev->tdm_en);
    osal_printf("DEC Status: D/S %d, U/S %d\r\n",g_gponmac_drv->dev->us_ploam_req_urg,g_gponmac_drv->dev->us_ploam_req_nrm);
    osal_printf("D/S FEC: %d, U/S FEC: %d\r\n",g_gponmac_drv->dev->ds_fec,g_gponmac_drv->dev->us_fec);
    osal_printf("U/S OMCI Flow ID: %d\r\n",g_gponmac_drv->dev->us_omci_flow);
    osal_printf("AES Key Switch Time: 0x%lx\r\n",(long unsigned int)g_gponmac_drv->dev->aes_key_switch_time);
    osal_printf("AES Key[%d]: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\r\n",g_gponmac_drv->dev->key_index,g_gponmac_drv->dev->aes_key.key[0],g_gponmac_drv->dev->aes_key.key[1],g_gponmac_drv->dev->aes_key.key[2],g_gponmac_drv->dev->aes_key.key[3],
        g_gponmac_drv->dev->aes_key.key[4],g_gponmac_drv->dev->aes_key.key[5],g_gponmac_drv->dev->aes_key.key[6],g_gponmac_drv->dev->aes_key.key[7],g_gponmac_drv->dev->aes_key.key[8],g_gponmac_drv->dev->aes_key.key[9],g_gponmac_drv->dev->aes_key.key[10],g_gponmac_drv->dev->aes_key.key[11],
        g_gponmac_drv->dev->aes_key.key[12],g_gponmac_drv->dev->aes_key.key[13],g_gponmac_drv->dev->aes_key.key[14],g_gponmac_drv->dev->aes_key.key[15]);
    osal_printf("Burst Overhead: guard bits %d, t1 bits %d, t2 bits %d, t3 pattern 0x%02x\r\n",g_gponmac_drv->dev->burst_overhead.guard_bits,g_gponmac_drv->dev->burst_overhead.type1_bits,g_gponmac_drv->dev->burst_overhead.type2_bits,g_gponmac_drv->dev->burst_overhead.type3_ptn);
    osal_printf("Burst Overhead: Delimiter %02x%02x%02x\r\n",g_gponmac_drv->dev->burst_overhead.delimiter[0],g_gponmac_drv->dev->burst_overhead.delimiter[1],g_gponmac_drv->dev->burst_overhead.delimiter[2]);
    osal_printf("Burst Overhead: t3 pre-range %d, t3 ranged %d\r\n",g_gponmac_drv->dev->burst_overhead.type3_preranged,g_gponmac_drv->dev->burst_overhead.type3_ranged);
    osal_printf("Burst Overhead: Total Length: pre-range %d, ranged %d\r\n",g_gponmac_drv->dev->burst_overhead.boh_len_preranged,g_gponmac_drv->dev->burst_overhead.boh_len_ranged);
    osal_printf("Burst Overhead: Pattern: valid %d, repeat byte %d\r\n",g_gponmac_drv->dev->burst_overhead.boh_valid,g_gponmac_drv->dev->burst_overhead.boh_repeat);
    if(g_gponmac_drv->dev->burst_overhead.boh_valid)
    {
        osal_printf("Burst Overhead: Pattern:");
        for(i=0;i<g_gponmac_drv->dev->burst_overhead.boh_valid;i++)
        {
            osal_printf(" 0x%02x",g_gponmac_drv->dev->burst_overhead.bursthead[i]);
        }
        osal_printf("\r\n");
    }
    for(i=0;i<RTK_GPON_ALARM_MAX;i++)
    {
        osal_printf("Alarm [%s]: %d\r\n",gpon_dbg_alarm_str((rtk_gpon_alarm_type_t)i),g_gponmac_drv->dev->alarm_tbl[i]);
    }
    osal_printf("Callback: State Change: %p\r\n",g_gponmac_drv->dev->state_change_callback);
    osal_printf("Callback: D/S FEC Change: %p\r\n",g_gponmac_drv->dev->dsfec_change_callback);
    osal_printf("Callback: U/S FEC Change: %p\r\n",g_gponmac_drv->dev->usfec_change_callback);
    osal_printf("Callback: U/S PLOAM Urgent Q Empty: %p\r\n",g_gponmac_drv->dev->usploam_urg_epty_callback);
    osal_printf("Callback: U/S PLOAM Normal Q Empty: %p\r\n",g_gponmac_drv->dev->usploam_nrm_epty_callback);
    osal_printf("Callback: PLOAM: %p\r\n",g_gponmac_drv->dev->ploam_callback);
    osal_printf("Callback: OMCI: %p\r\n",g_gponmac_drv->dev->omci_callback);
    osal_printf("Callback: AES Key: %p\r\n",g_gponmac_drv->dev->aeskey_callback);
    for(i=0;i<RTK_GPON_ALARM_MAX;i++)
    {
        osal_printf("Callback: Alarm [%s]: %p\r\n",gpon_dbg_alarm_str((rtk_gpon_alarm_type_t)i),g_gponmac_drv->dev->fault_callback[i]);
    }
    osal_printf("============================================================\r\n");
}

void dal_apollomp_gpon_tcont_show(uint32 tcont)
{
    uint32 i;
    int32        found = FALSE;
    if(!g_gponmac_drv)
    {
        osal_printf("GPON MAC is not started.\r\n");
        return;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        osal_printf("GPON MAC Device is not initialized.\r\n");
        return;
    }

    for(i=0;i<GPON_DEV_MAX_TCONT_NUM;i++)
    {
        if(g_gponmac_drv->dev->p_tcont[i])
        {
            if(tcont==0xFFFF || tcont==i)
            {
                if(!found)
                {
                    osal_printf("============================================================\r\n");
                    osal_printf("     GPON ONU MAC TCONT Status\r\n");
                    osal_printf("TCont ID | Alloc ID | Physical | Logical | Type \r\n");
                    found = TRUE;
                }
                osal_printf("    %4d |     %4d |        %1s |       %1s |    %1d \r\n",g_gponmac_drv->dev->p_tcont[i]->tcont_id,
                    g_gponmac_drv->dev->p_tcont[i]->alloc_id,g_gponmac_drv->dev->p_tcont[i]->ploam_en?"*":" ",g_gponmac_drv->dev->p_tcont[i]->omci_en?"*":" ",
                    g_gponmac_drv->dev->p_tcont[i]->type);
            }
        }
    }
    if(found)
    {
        osal_printf("============================================================\r\n");
    }
    else
    {
        if(tcont!=0xFFFF)
        {
            osal_printf("The TCont is not created.\r\n");
        }
    }
}

void dal_apollomp_gpon_dsFlow_show(uint32 flow)
{
    uint32 i;
    int32        found = FALSE;
    if(!g_gponmac_drv)
    {
        osal_printf("GPON MAC is not started.\r\n");
        return;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        osal_printf("GPON MAC Device is not initialized.\r\n");
        return;
    }

    for(i=0;i<GPON_DEV_MAX_FLOW_NUM;i++)
    {
        if(g_gponmac_drv->dev->p_dsflow[i])
        {
            if(flow==0xFFFF || flow==i)
            {
                if(!found)
                {
                    osal_printf("============================================================\r\n");
                    osal_printf("     GPON ONU MAC D/S Flow Status\r\n");
                    osal_printf("Flow ID | GEM Port | Type | Multicast | AES\r\n");
                    found = TRUE;
                }
                osal_printf("   %4d |     %4d | %4s |         %1s |   %1s\r\n",g_gponmac_drv->dev->p_dsflow[i]->flow_id,
                    g_gponmac_drv->dev->p_dsflow[i]->gem_port,gpon_dbg_flow_type_str(g_gponmac_drv->dev->p_dsflow[i]->type),g_gponmac_drv->dev->p_dsflow[i]->multicast?"*":" ",
                    g_gponmac_drv->dev->p_dsflow[i]->aes_en?"*":" ");
            }
        }
    }
    if(found)
    {
        osal_printf("============================================================\r\n");
    }
    else
    {
        if(flow!=0xFFFF)
        {
            osal_printf("The D/S Flow is not created.\r\n");
        }
    }
}

void dal_apollomp_gpon_usFlow_show(uint32 flow)
{
    uint32 i;
    int32        found = FALSE;
    if(!g_gponmac_drv)
    {
        osal_printf("GPON MAC is not started.\r\n");
        return;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        osal_printf("GPON MAC Device is not initialized.\r\n");
        return;
    }

    for(i=0;i<GPON_DEV_MAX_FLOW_NUM;i++)
    {
        if(g_gponmac_drv->dev->p_usflow[i])
        {
            if(flow==0xFFFF || flow==i)
            {
                if(!found)
                {
                    osal_printf("============================================================\r\n");
                    osal_printf("    GPON ONU MAC U/S Flow Status\r\n");
                    osal_printf("Flow ID | GEM Port | Type | TCont | Channel | OMCI\r\n");
                    found = TRUE;
                }
                osal_printf("   %4d |     %4d | %4s |  %4d |     %3d |    %1s\r\n",g_gponmac_drv->dev->p_usflow[i]->flow_id,
                    g_gponmac_drv->dev->p_usflow[i]->gem_port,gpon_dbg_flow_type_str(g_gponmac_drv->dev->p_usflow[i]->type),g_gponmac_drv->dev->p_usflow[i]->tcont,
                    g_gponmac_drv->dev->p_usflow[i]->channel,
                    (g_gponmac_drv->dev->p_usflow[i]->flow_id==g_gponmac_drv->dev->us_omci_flow)?"*":" ");
            }
        }
    }
    if(found)
    {
        osal_printf("============================================================\r\n");
    }
    else
    {
        if(flow!=0xFFFF)
        {
            osal_printf("The U/S Flow is not created.\r\n");
        }
    }
}

void dal_apollomp_gpon_macTable_show(void)
{
    uint32      i;
    int32       found = FALSE;
    if(!g_gponmac_drv)
    {
        osal_printf("GPON MAC is not started.\r\n");
        return;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        osal_printf("GPON MAC Device is not initialized.\r\n");
        return;
    }

    for(i=0;i<GPON_DEV_MAX_MACTBL_NUM;i++)
    {
        if(g_gponmac_drv->dev->p_mac_tbl[i])
        {
            if(!found)
            {
                osal_printf("============================================================\r\n");
                osal_printf("     GPON ONU MAC Multicast MAC Filter Table\r\n");
                osal_printf("Index | MAC Address\r\n");
                found = TRUE;
            }
            osal_printf("  %3d | %02x-%02x-%02x-%02x-%02x-%02x\r\n",i,g_gponmac_drv->dev->p_mac_tbl[i]->addr[0],
                g_gponmac_drv->dev->p_mac_tbl[i]->addr[1],g_gponmac_drv->dev->p_mac_tbl[i]->addr[2],g_gponmac_drv->dev->p_mac_tbl[i]->addr[3],
                g_gponmac_drv->dev->p_mac_tbl[i]->addr[4],g_gponmac_drv->dev->p_mac_tbl[i]->addr[5]);
        }
    }
    if(found)
    {
        osal_printf("============================================================\r\n");
    }
    else
    {
        osal_printf("The MAC Table is empty.\r\n");
    }
}

void dal_apollomp_gpon_globalCounter_show(rtk_gpon_global_performance_type_t type)
{
    rtk_gpon_global_counter_t counter;
    if(!g_gponmac_drv)
    {
        osal_printf("GPON MAC is not started.\r\n");
        return;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        osal_printf("GPON MAC Device is not initialized.\r\n");
        return;
    }

    if(dal_apollomp_gpon_globalCounter_get(type,&counter)!=RT_ERR_OK)
    {
        osal_printf("Read Counter fail\r\n");
        return;
    }

    switch(type)
    {
        case RTK_GPON_PMTYPE_ONU_ACTIVATION:
        {
            osal_printf("============================================================\r\n");
            osal_printf("     GPON ONU MAC Device Counter: Activation\r\n");
            osal_printf("SN Req          : %u\r\n",counter.activate.rx_sn_req);
            osal_printf("Ranging Req     : %u\r\n",counter.activate.rx_ranging_req);
            osal_printf("============================================================\r\n");
            break;
        }
        case RTK_GPON_PMTYPE_DS_PHY:
        {
            osal_printf("============================================================\r\n");
            osal_printf("     GPON ONU MAC Device Counter: DS PHY\r\n");
            osal_printf("BIP Error bits  : %lu\r\n",(long unsigned int)counter.dsphy.rx_bip_err_bit);
            osal_printf("BIP Error blocks: %lu\r\n",(long unsigned int)counter.dsphy.rx_bip_err_block);
            osal_printf("FEC Correct bits: %lu\r\n",(long unsigned int)counter.dsphy.rx_fec_correct_bit);
            osal_printf("FEC Correct bytes: %lu\r\n",(long unsigned int)counter.dsphy.rx_fec_correct_byte);
            osal_printf("FEC Correct codewords: %lu\r\n",(long unsigned int)counter.dsphy.rx_fec_correct_cw);
            osal_printf("FEC codewords Uncor: %lu\r\n",(long unsigned int)counter.dsphy.rx_fec_uncor_cw);
            osal_printf("Superframe LOS  : %u\r\n",counter.dsphy.rx_lom);
            osal_printf("PLEN fail       : %u\r\n",counter.dsphy.rx_plen_err);
            osal_printf("============================================================\r\n");
            break;
        }
        case RTK_GPON_PMTYPE_DS_PLOAM:
        {
            osal_printf("============================================================\r\n");
            osal_printf("     GPON ONU MAC Device Counter: DS PLOAM\r\n");
            osal_printf("Total RX PLOAMd    : %u\r\n",counter.dsploam.rx_ploam_cnt);
            osal_printf("CRC Err RX PLOAM   : %u\r\n",counter.dsploam.rx_ploam_err);
            osal_printf("Corrected RX PLOAMd: %u\r\n",counter.dsploam.rx_ploam_correctted);
            osal_printf("Proc RX PLOAMd     : %u\r\n",counter.dsploam.rx_ploam_proc);
            osal_printf("Overflow Rx PLOAM  : %u\r\n",counter.dsploam.rx_ploam_overflow);
            osal_printf("============================================================\r\n");
            break;
        }
        case RTK_GPON_PMTYPE_DS_BWMAP:
        {
            osal_printf("============================================================\r\n");
            osal_printf("     GPON ONU MAC Device Counter: DS BWMAP\r\n");
            osal_printf("Total RX BwMap  : %lu\r\n",(long unsigned int)counter.dsbwmap.rx_bwmap_cnt);
            osal_printf("CRC Err RX BwMap: %u\r\n",counter.dsbwmap.rx_bwmap_crc_err);
            osal_printf("Overflow BwMap  : %u\r\n",counter.dsbwmap.rx_bwmap_overflow);
            osal_printf("Invalid BwMap 0 : %u\r\n",counter.dsbwmap.rx_bwmap_inv0);
            osal_printf("Invalid BwMap 1 : %u\r\n",counter.dsbwmap.rx_bwmap_inv1);
            osal_printf("============================================================\r\n");
            break;
        }
        case RTK_GPON_PMTYPE_DS_GEM:
        {
            osal_printf("============================================================\r\n");
            osal_printf("     GPON ONU MAC Device Counter: DS GEM\r\n");
            osal_printf("D/S GEM LOS     : %lu\r\n",(long unsigned int)counter.dsgem.rx_gem_los);
            osal_printf("D/S GEM Idle    : %lu\r\n",(long unsigned int)counter.dsgem.rx_gem_idle);
            osal_printf("D/S GEM Non Idle: %lu\r\n",(long unsigned int)counter.dsgem.rx_gem_non_idle);
            osal_printf("D/S HEC correct : %lu\r\n",(long unsigned int)counter.dsgem.rx_hec_correct);
            osal_printf("Over Interleave : %lu\r\n",(long unsigned int)counter.dsgem.rx_over_interleaving);
            osal_printf("Mis GEM Pkt Len : %u\r\n",counter.dsgem.rx_gem_len_mis);
            osal_printf("Multi Flow Match: %u\r\n",counter.dsgem.rx_match_multi_flow);
            osal_printf("============================================================\r\n");
            break;
        }
        case RTK_GPON_PMTYPE_DS_ETH:
        {
            osal_printf("============================================================\r\n");
            osal_printf("     GPON ONU MAC Device Counter: DS ETH\r\n");
            osal_printf("Total Unicast   : %lu\r\n",(long unsigned int)counter.dseth.rx_eth_unicast);
            osal_printf("Total Multicast : %lu\r\n",(long unsigned int)counter.dseth.rx_eth_multicast);
            osal_printf("Fwd Multicast   : %lu\r\n",(long unsigned int)counter.dseth.rx_eth_multicast_fwd);
            osal_printf("Leak Multicast  : %lu\r\n",(long unsigned int)counter.dseth.rx_eth_multicast_leak);
            osal_printf("FCS Error       : %lu\r\n",(long unsigned int)counter.dseth.rx_eth_fcs_err);
            osal_printf("============================================================\r\n");
            break;
        }
        case RTK_GPON_PMTYPE_DS_OMCI:
        {
            osal_printf("============================================================\r\n");
            osal_printf("     GPON ONU MAC Device Counter: DS OMCI\r\n");
            osal_printf("Total RX OMCI   : %lu\r\n",(long unsigned int)counter.dsomci.rx_omci);
            osal_printf("CRC Error OMCI  : %lu\r\n",(long unsigned int)counter.dsomci.rx_omci_crc_err);
            osal_printf("Processed OMCI  : %lu\r\n",(long unsigned int)counter.dsomci.rx_omci_proc);
            osal_printf("Dropped OMCI    : %lu\r\n",(long unsigned int)counter.dsomci.rx_omci_drop);
            osal_printf("============================================================\r\n");
            break;
        }
        case RTK_GPON_PMTYPE_US_PHY:
        {
            osal_printf("============================================================\r\n");
            osal_printf("     GPON ONU MAC Device Counter: US PHY\r\n");
            osal_printf("TX BOH          : %u\r\n",counter.usphy.tx_boh_cnt);
            osal_printf("============================================================\r\n");
            break;
        }
        case RTK_GPON_PMTYPE_US_DBR:
        {
            osal_printf("============================================================\r\n");
            osal_printf("     GPON ONU MAC Device Counter: US DBR\r\n");
            osal_printf("TX DBRu         : %u\r\n",counter.usdbr.tx_dbru_cnt);
            osal_printf("============================================================\r\n");
            break;
        }
        case RTK_GPON_PMTYPE_US_PLOAM:
        {
            osal_printf("============================================================\r\n");
            osal_printf("     GPON ONU MAC Device Counter: US PLOAM\r\n");
            osal_printf("Total TX PLOAM  : %u\r\n",counter.usploam.tx_ploam_cnt);
            osal_printf("Process TX PLOAM: %u\r\n",counter.usploam.tx_ploam_proc);
            osal_printf("TX Urgent PLOAM : %u\r\n",counter.usploam.tx_ploam_urg);
            osal_printf("Proc Urg PLOAM  : %u\r\n",counter.usploam.tx_ploam_urg_proc);
            osal_printf("TX Normal PLOAM : %u\r\n",counter.usploam.tx_ploam_nor);
            osal_printf("Proc Nrm PLOAM  : %u\r\n",counter.usploam.tx_ploam_nor_proc);
            osal_printf("TX S/N PLOAM    : %u\r\n",counter.usploam.tx_ploam_sn);
            osal_printf("TX NoMsg PLOAM  : %u\r\n",counter.usploam.tx_ploam_nomsg);
            osal_printf("============================================================\r\n");
            break;
        }
        case RTK_GPON_PMTYPE_US_GEM:
        {
            osal_printf("============================================================\r\n");
            osal_printf("     GPON ONU MAC Device Counter: US GEM\r\n");
            osal_printf("TX GEM Blocks   : %u\r\n",counter.usgem.tx_gem_cnt);
            osal_printf("TX GEM Bytes    : %lu\r\n",(long unsigned int)counter.usgem.tx_gem_byte);
            osal_printf("============================================================\r\n");
            break;
        }
        case RTK_GPON_PMTYPE_US_ETH:
        {
            osal_printf("============================================================\r\n");
            osal_printf("     GPON ONU MAC Device Counter: US ETH\r\n");
            osal_printf("Aborted Packet  : %lu\r\n",(long unsigned int)counter.useth.tx_eth_abort_ebb);
            osal_printf("============================================================\r\n");
            break;
        }
        case RTK_GPON_PMTYPE_US_OMCI:
        {
            osal_printf("============================================================\r\n");
            osal_printf("     GPON ONU MAC Device Counter: US OMCI\r\n");
            osal_printf("Process OMCI    : %lu\r\n",(long unsigned int)counter.usomci.tx_omci_proc);
            osal_printf("============================================================\r\n");
            break;
        }
        default:
        {
            osal_printf("Unknown Counter Type\r\n");
            break;
        }
    }
}

void dal_apollomp_gpon_tcontCounter_show(uint32 idx, rtk_gpon_tcont_performance_type_t type)
{
    rtk_gpon_tcont_counter_t counter;
    if(!g_gponmac_drv)
    {
        osal_printf("GPON MAC is not started.\r\n");
        return;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        osal_printf("GPON MAC Device is not initialized.\r\n");
        return;
    }

    if(dal_apollomp_gpon_tcontCounter_get(idx,type,&counter)!=RT_ERR_OK)
    {
        osal_printf("Read Counter fail[idx %d, type %d]\r\n",idx,type);
        return;
    }

    switch(type)
    {
        case RTK_GPON_PMTYPE_TCONT_GEM:
        {
            osal_printf("============================================================\r\n");
            osal_printf("     GPON ONU MAC Device Counter: TCont %d Gem\r\n",idx);
            osal_printf("U/S sent GEM    : %lu\r\n",(long unsigned int)counter.gem.gem_pkt);
            osal_printf("============================================================\r\n");
            break;
        }
        case RTK_GPON_PMTYPE_TCONT_ETH:
        {
            osal_printf("============================================================\r\n");
            osal_printf("     GPON ONU MAC Device Counter: TCont %d Eth\r\n",idx);
            osal_printf("U/S sent Eth Pkt: %lu\r\n",(long unsigned int)counter.eth.eth_pkt);
            osal_printf("============================================================\r\n");
            break;
        }
        case RTK_GPON_PMTYPE_TCONT_IDLE:
        {
            osal_printf("============================================================\r\n");
            osal_printf("     GPON ONU MAC Device Counter: TCont %d Idle\r\n",idx);
            osal_printf("U/S sent Idle Byte: %llu\r\n",counter.idle.idle_byte);
            osal_printf("============================================================\r\n");
            break;
        }
        default:
        {
            osal_printf("Unknown Counter Type\r\n");
            break;
        }
    }
}

void dal_apollomp_gpon_flowCounter_show(uint32 idx, rtk_gpon_flow_performance_type_t type)
{
    rtk_gpon_flow_counter_t counter;
    if(!g_gponmac_drv)
    {
        osal_printf("GPON MAC is not started.\r\n");
        return;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        osal_printf("GPON MAC Device is not initialized.\r\n");
        return;
    }

    if(dal_apollomp_gpon_flowCounter_get(idx,type,&counter)!=RT_ERR_OK)
    {
        osal_printf("Read Counter fail[idx %d, type %d]\r\n",idx,type);
        return;
    }

    switch(type)
    {
        case RTK_GPON_PMTYPE_FLOW_DS_GEM:
        {
            osal_printf("============================================================\r\n");
            osal_printf("     GPON ONU MAC Device Counter: D/S Flow %d Gem\r\n",idx);
            osal_printf("D/S GEM packets : %lu\r\n",(long unsigned int)counter.dsgem.gem_block);
            osal_printf("D/S GEM bytes   : %lu\r\n",(long unsigned int)counter.dsgem.gem_byte);
            osal_printf("============================================================\r\n");
            break;
        }
        case RTK_GPON_PMTYPE_FLOW_DS_ETH:
        {
            osal_printf("============================================================\r\n");
            osal_printf("     GPON ONU MAC Device Counter: D/S Flow %d Eth\r\n",idx);
            osal_printf("RX Eth packetts : %lu\r\n",(long unsigned int)counter.dseth.eth_pkt_rx);
            osal_printf("Fwd Eth packets : %lu\r\n",(long unsigned int)counter.dseth.eth_pkt_fwd);
            osal_printf("============================================================\r\n");
            break;
        }
        case RTK_GPON_PMTYPE_FLOW_US_GEM:
        {
            osal_printf("============================================================\r\n");
            osal_printf("     GPON ONU MAC Device Counter: U/S Flow %d Gem\r\n",idx);
            osal_printf("U/S GEM counts  : %lu\r\n",(long unsigned int)counter.usgem.gem_block);
            osal_printf("U/S GEM bytes   : %llu\r\n",counter.usgem.gem_byte);
            osal_printf("============================================================\r\n");
            break;
        }
        case RTK_GPON_PMTYPE_FLOW_US_ETH:
        {
            osal_printf("============================================================\r\n");
            osal_printf("     GPON ONU MAC Device Counter: U/S Flow %d Eth\r\n",idx);
            osal_printf("U/S Eth packets   : %lu\r\n",(long unsigned int)counter.useth.eth_cnt);
            osal_printf("============================================================\r\n");
            break;
        }
        default:
        {
            osal_printf("Unknown Counter Type\r\n");
            break;
        }
    }
}

int32 dal_apollomp_gpon_omci_tx(rtk_gpon_omci_msg_t* omci)
{
    int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_omci_tx(g_gponmac_drv->dev, omci);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}

int32 dal_apollomp_gpon_omci_rx(rtk_gpon_omci_msg_t* omci)
{
    int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!g_gponmac_drv->dev || g_gponmac_drv->status==GPON_DRV_STATUS_INIT)
    {
        return RT_ERR_GPON_ONU_NOT_REGISTERED;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_omci_rx(g_gponmac_drv->dev, omci);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}

int32 dal_apollomp_gpon_auto_tcont_set(int32 state)
{
    int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_dev_auto_tcont_set(g_gponmac_drv->dev,state);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}

int32 dal_apollomp_gpon_auto_tcont_get(int32 *pState)
{
    int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!pState)
    {
        return RT_ERR_NULL_POINTER;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_dev_auto_tcont_get(g_gponmac_drv->dev,pState);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}

int32 dal_apollomp_gpon_auto_boh_set(int32 state)
{
    int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_dev_auto_boh_set(g_gponmac_drv->dev,state);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}

int32 dal_apollomp_gpon_auto_boh_get(int32 *pState)
{
    int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!pState)
    {
        return RT_ERR_NULL_POINTER;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_dev_auto_boh_get(g_gponmac_drv->dev,pState);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}

int32 dal_apollomp_gpon_eqd_offset_set(int32 offset)
{
    int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_dev_eqd_offset_set(g_gponmac_drv->dev,offset);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}

int32 dal_apollomp_gpon_eqd_offset_get(int32 *pOffset)
{
    int32 ret;

    if(!g_gponmac_drv)
    {
        return RT_ERR_GPON_DRV_NOT_STARTED;
    }

    if(!pOffset)
    {
        return RT_ERR_NULL_POINTER;
    }

    GPON_OS_Lock(g_gponmac_drv->lock);

    ret = gpon_dev_eqd_offset_get(g_gponmac_drv->dev,pOffset);

    GPON_OS_Unlock(g_gponmac_drv->lock);

    return ret;
}

int32 dal_apollomp_gpon_autoDisTx_set(rtk_enable_t state)
{
    uint32 data;
    uint32 addr;
    int32 ret;

    addr = (5 << 12) | (0x9b)*4 | 0x700000;
    if(ENABLED == state)
    {
        data=0;
        if ((ret = ioal_mem32_write(addr, data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
            return ret;
        }

        data=1;
        if ((ret = reg_field_write(APOLLOMP_IO_MODE_ENr, APOLLOMP_OEM_ENf, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
            return ret;
        }

        data=0;
        if ((ret = reg_field_write(APOLLOMP_WSDS_DIG_18r, APOLLOMP_CFG_FRC_TX_DISABLE_OPTICf, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
            return ret;
        }

        data=1;
        if ((ret = reg_field_write(APOLLOMP_WSDS_DIG_18r, APOLLOMP_TX_DISABLE_OPTIC_OEf, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
            return ret;
        }

        data=0;
        if ((ret = reg_array_field_write(APOLLOMP_GPIO_CTRL_2r, REG_ARRAY_INDEX_NONE, 34, APOLLOMP_EN_GPIOf,&data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
            return ret;
        }
    }
    else
    {
        data=0;
        if ((ret = ioal_mem32_write(addr, data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
            return ret;
        }

        data=0;
        if ((ret = reg_field_write(APOLLOMP_IO_MODE_ENr, APOLLOMP_OEM_ENf, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
            return ret;
        }

        data=0;
        if ((ret = reg_field_write(APOLLOMP_WSDS_DIG_18r, APOLLOMP_CFG_FRC_TX_DISABLE_OPTICf, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
            return ret;
        }

        data=0;
        if ((ret = reg_field_write(APOLLOMP_WSDS_DIG_18r, APOLLOMP_TX_DISABLE_OPTIC_OEf, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
            return ret;
        }

        data=1;
        if ((ret = reg_array_field_write(APOLLOMP_GPIO_CTRL_2r, REG_ARRAY_INDEX_NONE, 34, APOLLOMP_EN_GPIOf,&data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
            return ret;
        }

    }

    return RT_ERR_OK;
}

