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
 * Purpose : GPON MAC Driver Debug Functions
 *
 * Feature : GPON MAC Driver Debug Functions
 *
 */

#include <module/gpon/gpon_defs.h>
#include <module/gpon/gpon_debug.h>
#include <module/gpon/gpon.h>

static int32 gpon_dbg_flag=FALSE;
static uint32 gpon_dbg_level=0x0;

int32 gpon_dbg_para_laser_get(rtk_gpon_laser_para_t *pPara)
{
    int32 opten=0, optpolar=0, cdren=0, cdrpolar=0, filter=0;
    uint8 on=0, off=0;
    int32 ret;

    if((ret = rtk_gpon_losCfg_get(&opten,&optpolar,&cdren,&cdrpolar,&filter))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG,MOD_GPON,"apollo_raw_gpon_losCfg_read fail:0x%x\n\r",ret);
        return RT_ERR_FAILED;
    }
    pPara->laser_optic_los_en       = opten;
    pPara->laser_optic_los_polar    = optpolar;
    pPara->laser_cdr_los_en         = cdren;
    pPara->laser_cdr_los_polar      = cdrpolar;
    pPara->laser_los_filter_en      = filter;

    if((ret = rtk_gpon_laserTime_get(&on,&off))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG,MOD_GPON,"apollo_raw_gpon_laserTime_read fail:0x%x\n\r",ret);
        return RT_ERR_FAILED;
    }
    pPara->laser_us_on  = on;
    pPara->laser_us_off = off;

    return RT_ERR_OK;
}

int32 gpon_dbg_para_dsPhy_get(rtk_gpon_ds_physical_para_t *para)
{
    int32 ret;

    if((ret=rtk_gpon_dsScrambleState_get((rtk_enable_t*)&para->ds_scramble_en))!=RT_ERR_OK)
        return ret;
    if((ret=rtk_gpon_dsFecBypass_get((rtk_enable_t*)&para->ds_fec_bypass))!=RT_ERR_OK)
        return ret;
    return rtk_gpon_dsFecThrd_get(&para->ds_fec_thrd);
}

int32 gpon_dbg_para_dsPloam_get(rtk_gpon_ds_ploam_para_t *para)
{
    int32 ret;
    if((ret=rtk_gpon_dsPloamOnuIdFilterState_get((rtk_enable_t*)&para->ds_ploam_onuid_filter))!=RT_ERR_OK)
        return ret;
    if((ret=rtk_gpon_dsPloamBcAcceptState_get((rtk_enable_t*)&para->ds_ploam_broadcast_accpt))!=RT_ERR_OK)
        return ret;
    ret = rtk_gpon_dsPloamDropCrcState_get((rtk_enable_t*)&para->ds_ploam_drop_crc_err);

    return ret;
}

int32 gpon_dbg_para_dsBwmap_get(rtk_gpon_ds_bwMap_para_t *para)
{
    int32 ret;

    if((ret=rtk_gpon_dsBwmapCrcCheckState_get((rtk_enable_t*)&para->ds_bwmap_crc_chk))!=RT_ERR_OK)
        return ret;
    if((ret=rtk_gpon_dsBwmapFilterOnuIdState_get((rtk_enable_t*)&para->ds_bwmap_onuid_filter))!=RT_ERR_OK)
        return ret;

    ret = rtk_gpon_dsPlendStrictMode_get((rtk_enable_t*)&para->ds_bwmap_plend_mode);

    return ret;
}

int32 gpon_dbg_para_dsGem_get(rtk_gpon_ds_gem_para_t *para)
{
    uint32 timer=0;
    int32 ret;
    if((ret = rtk_gpon_dsGemPortFrameTimeOut_get(&timer))!=RT_ERR_OK)
    {
        return ret;
    }
    para->assemble_timer = timer;
    return RT_ERR_OK;
}

int32 gpon_dbg_para_dsEth_get(rtk_gpon_ds_eth_para_t *para)
{
    int32 ret;
    uint32 mask=0,ptn=0;

    if((ret = rtk_gpon_dsGemPortFcsCheckState_get((rtk_enable_t*)&para->ds_eth_crc_chk)) != RT_ERR_OK)
        return ret;

    if((ret = rtk_gpon_dsEthPti_get(&mask,&ptn))!=RT_ERR_OK)
    {
        return ret;
    }
    para->ds_eth_pti_mask = (uint8)mask;
    para->ds_eth_pti_ptn    = (uint8)ptn;
    return RT_ERR_OK;
}

int32 gpon_dbg_para_dsOmci_get(rtk_gpon_ds_omci_para_t *para)
{
    uint32 mask=0,ptn=0;
    int32 ret;
    if((ret = rtk_gpon_dsOmciPti_get(&mask,&ptn))!=RT_ERR_OK)
    {
        return ret;
    }
    para->ds_omci_pti_ptn = ptn;
    para->ds_omci_pti_mask = mask;
    return RT_ERR_OK;
}

int32 gpon_dbg_para_usPhy_get(rtk_gpon_us_physical_para_t *para)
{
    int32 ret;
    rtk_gpon_polarity_t polar;

    if((ret =  rtk_gpon_usScrambleState_get((rtk_enable_t*)&para->us_scramble_en)) != RT_ERR_OK)
        return ret;
    if((ret =  rtk_gpon_usBurstPolarity_get(&polar)) != RT_ERR_OK)
        return ret;
    para->us_burst_en_polar =(int32) polar;
    if((ret =  rtk_gpon_usSmalSstartProcState_get((rtk_enable_t*)&para->small_sstart_en)) != RT_ERR_OK)
        return ret;
    if((ret =  rtk_gpon_usSuppressLaserState_get((rtk_enable_t*)&para->suppress_laser_en)) != RT_ERR_OK)
        return ret;
    return ret;
}

int32 gpon_dbg_para_usPloam_get(rtk_gpon_us_ploam_para_t *para)
{
    return rtk_gpon_ploamState_get((rtk_enable_t*)&para->us_ploam_en);
}

int32 gpon_dbg_para_usDbr_get(rtk_gpon_us_dbr_para_t *para)
{
    return rtk_gpon_dbruState_get((rtk_enable_t*)&para->us_dbru_en);
}

int32 gpon_dbg_dsFlow_get(uint32 flowId, rtk_gpon_dsFlow_attr_t *attr)
{
    int32 ret;
    rtk_gem_cfg_t cfg={0};

    ret = rtk_gpon_dsGemPort_get(flowId,&cfg);
    if(ret == RT_ERR_OK)
    {
    attr->gem_port_id = cfg.gemPortId;
    attr->type = (cfg.isEthernet) ? RTK_GPON_FLOW_TYPE_ETH : RTK_GPON_FLOW_TYPE_OMCI;
    attr->multicast = (cfg.isMulticast) ?1:0;
    attr->aes_en = (cfg.enAES)?1:0;
    }
    return ret;
}

int32 gpon_dbg_usFlow_get(uint32 flowId, uint32 *gem_port)
{
    return rtk_gpon_gemUsPortCfg_get(flowId, gem_port);
}

int32 gpon_dbg_multicastAddrCheck_get(uint32 *ipv4_pattern, uint32 *ipv6_pattern)
{
    int32 ret;
    ret = rtk_gpon_ipv4McAddrPtn_get(ipv4_pattern);
    if(ret == RT_ERR_OK)
        ret = rtk_gpon_ipv6McAddrPtn_get(ipv6_pattern);
    return ret;
}

int32 gpon_dbg_broadcastPass_get(int32 *mode)
{
    int32 ret;
    rtk_enable_t state=DISABLED;
       if((ret = rtk_gpon_dsGemPortBcPassState_get(&state))!=RT_ERR_OK)
       {
        return ret;
       }
     *mode = (int32) state;
     return RT_ERR_OK;
}

int32 gpon_dbg_nonMcastPass_get(int32 *mode)
{
    int32 ret;
    rtk_enable_t state=DISABLED;
    if((ret = rtk_gpon_dsGemPortNonMcPassState_get(&state))!=RT_ERR_OK)
       {
        return ret;
       }
         *mode = (int32) state;
     return RT_ERR_OK;
}

int32 gpon_dbg_macFilterMode_get(rtk_gpon_macTable_exclude_mode_t *mode)
{
    return rtk_gpon_dsGemPortMacFilterMode_get(mode);
}

int32 gpon_dbg_mcForceMode_get(rtk_gpon_mc_force_mode_t *ipv4, rtk_gpon_mc_force_mode_t *ipv6)
{
    int32 ret;
    if((ret = rtk_gpon_dsGemPortMacForceMode_get(GPON_IP_VER_V4,ipv4))!=RT_ERR_OK)
    {
        return ret;
    }
    if((ret = rtk_gpon_dsGemPortMacForceMode_get(GPON_IP_VER_V6,ipv6))!=RT_ERR_OK)
    {
        return ret;
    }
    return RT_ERR_OK;
}

int32 gpon_dbg_macEntry_get(uint32 index, rtk_gpon_macTable_entry_t *pEntry)
{
    rtk_mac_t mac={{0}};
    int32 ret;

    RT_PARAM_CHK((index >= GPON_DEV_MAX_MACTBL_NUM), RT_ERR_OUT_OF_RANGE);

    ret = rtk_gpon_dsGemPortMacEntry_get(index, &mac);

    osal_memcpy(&pEntry->mac_addr[0],&mac.octet[0],ETHER_ADDR_LEN);

    return ret;
}

int32 gpon_dbg_rdi_get(int32 *pEnable)
{
    return rtk_gpon_usRdi_get(pEnable);
}

int32 gpon_dbg_forceLaser_set(gpon_dev_obj_t* obj, rtk_gpon_laser_status_t enable)
{
    if(NULL == obj)
        return RT_ERR_NULL_POINTER;

    GPON_OS_Log(GPON_LOG_LEVEL_DEBUG,"Set Force Laser %d [baseaddr:%p]",enable,obj->base_addr);
    rtk_gpon_forceLaser_set(enable);
    return RT_ERR_OK;
}

int32 gpon_dbg_forceLaser_get(gpon_dev_obj_t* obj, rtk_gpon_laser_status_t *pEnable)
{
    if(NULL == obj)
        return RT_ERR_NULL_POINTER;
    return rtk_gpon_forceLaser_get(pEnable);
}

int32 gpon_dbg_forceIdle_set(gpon_dev_obj_t* obj, int32 enable)
{
    if(NULL == obj)
        return RT_ERR_NULL_POINTER;
    GPON_OS_Log(GPON_LOG_LEVEL_DEBUG,"Set Force Idle %d [baseaddr:%p]",enable,obj->base_addr);
    rtk_gpon_gemUsForceIdleState_set(enable);
    return RT_ERR_OK;
}

int32 gpon_dbg_forceIdle_get(gpon_dev_obj_t* obj, int32 *pEnable)
{
    if(NULL == obj)
        return RT_ERR_NULL_POINTER;
    return rtk_gpon_gemUsForceIdleState_get((rtk_enable_t*)pEnable);
}

int32 gpon_dbg_forcePRBS_set(gpon_dev_obj_t* obj, rtk_gpon_prbs_t prbsCfg)
{
    if (NULL == obj)
        return RT_ERR_NULL_POINTER;

    GPON_OS_Log(GPON_LOG_LEVEL_DEBUG, "Set Force PRBS %d [baseaddr:%p]", prbsCfg, obj->base_addr);

    return rtk_gpon_forcePRBS_set(prbsCfg);
}

int32 gpon_dbg_forcePRBS_get(gpon_dev_obj_t* obj, rtk_gpon_prbs_t *pPrbsCfg)
{
    if (NULL == obj)
        return RT_ERR_NULL_POINTER;

    return rtk_gpon_forcePRBS_get(pPrbsCfg);
}

int32 gpon_dbg_dsFec_get(gpon_dev_obj_t* obj, int32 *pEnable)
{
    if(NULL == obj)
        return RT_ERR_NULL_POINTER;
    return rtk_gpon_gtcDsIntr_get(GPON_DS_FEC_STS_DLT,(rtk_enable_t*)pEnable);
}

const char* gpon_dbg_drv_status_str(gpon_drv_status_t status)
{
    switch(status)
    {
        case GPON_DRV_STATUS_INIT:
            return "Initial";
        case GPON_DRV_STATUS_READY:
            return "Ready";
        case GPON_DRV_STATUS_ACTIVATE:
            return "Active";
        default:
            return "Error";
    }
}

const char* gpon_dbg_fsm_status_str(rtk_gpon_fsm_status_t status)
{
    switch(status)
    {
        case GPON_STATE_UNKNOWN:
            return "Unknown";
        case GPON_STATE_O1:
            return "Initial State(O1)";
        case GPON_STATE_O2:
            return "Standby State(O2)";
        case GPON_STATE_O3:
            return "Serial Number State(O3)";
        case GPON_STATE_O4:
            return "Ranging State(O4)";
        case GPON_STATE_O5:
            return "Operation State(O5)";
        case GPON_STATE_O6:
            return "POPUP State(O6)";
        case GPON_STATE_O7:
            return "Emergency Stop State(O7)";
        default:
            return "Error";
    }
}

const char* gpon_dbg_fsm_event_str(rtk_gpon_fsm_event_t event)
{
    switch(event)
    {
        case GPON_FSM_EVENT_LOS_CLEAR:
            return "LOS Clear";
        case GPON_FSM_EVENT_RX_UPSTREAM:
            return "Rcv Upstream";
        case GPON_FSM_EVENT_RX_SN_REQ:
            return "Rcv SN Req";
        case GPON_FSM_EVENT_RX_ONUID:
            return "Assigned ONU ID";
        case GPON_FSM_EVENT_RX_RANGING_REQ:
            return "Rcv Ranging Req";
        case GPON_FSM_EVENT_RX_EQD:
            return "Rcv Ranging Time";
        case GPON_FSM_EVENT_TO1_EXPIRE:
            return "TO1 Expire";
        case GPON_FSM_EVENT_RX_DEACTIVATE:
            return "Deactive";
        case GPON_FSM_EVENT_LOS_DETECT:
            return "LOS Detected";
        case GPON_FSM_EVENT_RX_BC_POPUP:
            return "Broadcast POPUP";
        case GPON_FSM_EVENT_RX_DIRECT_POPUP:
            return "Direct POPUP";
        case GPON_FSM_EVENT_TO2_EXPIRE:
            return "TO2 Expire";
        case GPON_FSM_EVENT_RX_DISABLE:
            return "Disable";
        case GPON_FSM_EVENT_RX_ENABLE:
            return "Enable";
        default:
            return "Error";
    }
}

const char* gpon_dbg_mac_filter_mode_str(rtk_gpon_macTable_exclude_mode_t status)
{
    switch(status)
    {
        case RTK_GPON_MACTBL_MODE_EXCLUDE:
            return "Excluding";
        case RTK_GPON_MACTBL_MODE_INCLUDE:
            return "Including";
        default:
            return "Error";
    }
}

const char* gpon_dbg_mac_force_mode_str(rtk_gpon_mc_force_mode_t status)
{
    switch(status)
    {
        case RTK_GPON_MCFORCE_MODE_NORMAL:
            return "Normal";
        case RTK_GPON_MCFORCE_MODE_PASS:
            return "Force pass";
        case RTK_GPON_MCFORCE_MODE_DROP:
            return "Force drop";
        default:
            return "Error";
    }
}

const char* gpon_dbg_mac_force_laser_str(rtk_gpon_laser_status_t status)
{
    switch(status)
    {
        case RTK_GPON_LASER_STATUS_NORMAL:
            return "Normal";
        case RTK_GPON_LASER_STATUS_FORCE_ON:
            return "Force on";
        case RTK_GPON_LASER_STATUS_FORCE_OFF:
            return "Force off";
        default:
            return "Error";
    }
}

const char* gpon_dbg_alarm_str(rtk_gpon_alarm_type_t status)
{
    switch(status)
    {
        case RTK_GPON_ALARM_NONE:
            return "None";
        case RTK_GPON_ALARM_LOS:
            return "LOS";
        case RTK_GPON_ALARM_LOF:
            return "LOF";
        case RTK_GPON_ALARM_LOM:
            return "LOM";
        case RTK_GPON_ALARM_SF:
            return "SF";
        case RTK_GPON_ALARM_SD:
            return "SD";
        case RTK_GPON_ALARM_SD_TOOLONG:
            return "SD_LONG";
        case RTK_GPON_ALARM_SD_MISMATCH:
            return "SD_MISM";
        case RTK_GPON_ALARM_MAX:
            return "MAX";
        default:
            return "Error";
    }
}

const char* gpon_dbg_flow_type_str(rtk_gpon_flowType_t type)
{
    switch(type)
    {
        case RTK_GPON_FLOW_TYPE_OMCI:
            return "OMCI";
        case RTK_GPON_FLOW_TYPE_ETH:
            return "ETH";
        case RTK_GPON_FLOW_TYPE_TDM:
            return "TDM";
        default:
            return "----";
    }
}

int32 gpon_dbg_print(const uint32 level, const char *format, ...)
{
    /* determine the length of the output string */
    int32 result = RT_ERR_FAILED;
    static uint8 buf[LOG_BUFSIZE_DEFAULT];   /* init value will be given by RT_LOG_FORMATTED_OUTPUT */

    if(1 != gpon_dbg_flag)
        return RT_ERR_OK;

	if((gpon_dbg_level>>level)&1)
	{

		/* formatted output conversion */
		RT_LOG_FORMATTED_OUTPUT(buf, format, result);


		if (result < 0)
		{
			osal_printf("dbg_print: %d\n\r", result);
			return RT_ERR_FAILED;
		}

		/* start logging */
		osal_printf("%s\n\r", buf);
	}

    return RT_ERR_OK;
}


void
gpon_dbg_enable(int32 enable, uint32 level)
{
    gpon_dbg_flag = enable;
	if(enable == 1)
	{
		if(level == GPON_LOG_LEVEL_ALL)
			gpon_dbg_level = 0xffffffff;
		else
			gpon_dbg_level |= (1<<level);
	}
	else
		gpon_dbg_level = 0;
}

