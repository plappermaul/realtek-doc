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
 * Purpose : GPON MAC Driver Resource Management
 *
 * Feature : Provide the APIs to access GPON MAC
 *
 */

#include <module/gpon/gpon_defs.h>
#include <module/gpon/gpon_res.h>
#include <module/gpon/gpon_alm.h>
#include <module/gpon/gpon_ploam.h>
#include <module/gpon/gpon_omci.h>
#include <module/gpon/gpon_fsm.h>
#include <ioal/mem32.h>
#include <rtk/gponv2.h>
#include <rtk/gpio.h>
#include <rtk/i2c.h>
#include <rtk/ponmac.h>
#include <osal/time.h>
#include <hal/common/halctrl.h>


#define FPGA_PATCH 1

#define GPON_TEST_VAL 0x12345678


static void gpon_res_set_bits(uint8 *buf, uint8 start, uint8 stop, uint8 bit)
{
    uint8 by,bi,cur,ptn=bit&0x01;
    for(cur=start;cur<=stop;cur++)
    {
        by = cur/8;
        bi = 7-(cur%8);
        buf[by] &= ~(1<<bi);
        buf[by] |= ptn<<bi;
    }
}

int32 gpon_dev_versoin_get(gpon_dev_obj_t* obj, rtk_gpon_device_ver_t* ver)
{
    uint32 version,ret;

    if(NULL == obj)
        return RT_ERR_NULL_POINTER;

    GPON_UTIL_ERR_CHK(rtk_gpon_macVersion_get(&version),ret);

    ver->gmac_ver = version;

    return RT_ERR_OK;
}

int32 gpon_dev_initialize(gpon_dev_obj_t* obj, uint32 baseaddr)
{
    uint32  rd=0;
    uint32  i;
    uint32  cnt32;
    uint64  cnt64;
    int32 ret;
#ifdef FPGA_DEFINED
    uint32  reset_done;
#endif
    rtk_gpon_schedule_info_t *pScheInfo;
    uint32 max_tcont,max_flow;
    /*allocate tcont/ds/us flow*/
    pScheInfo = &obj->scheInfo;
    /*Get OMCI Info*/
    if((ret = rtk_gpon_scheInfo_get(pScheInfo))!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
        return ret;
    }
    max_tcont = pScheInfo->max_tcont;
    max_flow  = pScheInfo->max_flow;
    /* memory initialization */
    obj->base_addr = baseaddr;
    obj->status = GPON_STATE_O1;
    obj->timer = 0;
    obj->ber_timer = 0;
    obj->onuid = GPON_DEV_DEFAULT_ONU_ID;

    osal_memcpy(&obj->serial_number.vendor[0], "RTKG", RTK_GPON_VENDOR_ID_LEN);
    obj->serial_number.specific[0] = 0x11;
    obj->serial_number.specific[1] = 0x11;
    obj->serial_number.specific[2] = 0x11;
    obj->serial_number.specific[3] = 0x11;
    osal_memset(&obj->password.password[0], 'a' ,RTK_GPON_PASSWORD_LEN);

    obj->parameter.onu.to1_timer = 10000;  /* 10s */
    obj->parameter.onu.to2_timer = 100;     /* 100ms */
    obj->parameter.laser.laser_optic_los_en = TRUE;
    obj->parameter.laser.laser_optic_los_polar = TRUE;
    obj->parameter.laser.laser_cdr_los_en = FALSE;
    obj->parameter.laser.laser_cdr_los_polar = FALSE;
    obj->parameter.laser.laser_los_filter_en = FALSE;
#ifdef FPGA_DEFINED
    obj->parameter.laser.laser_us_on = 0x18;
    obj->parameter.laser.laser_us_off = 0x28;
#else
    if(rtk_gpon_usLaserDefault_get(&obj->parameter.laser.laser_us_on, &obj->parameter.laser.laser_us_off) != RT_ERR_OK) 
    {
        obj->parameter.laser.laser_us_on = 0x18;
        obj->parameter.laser.laser_us_off = 0x20;
    }
#endif
    obj->parameter.dsphy.ds_scramble_en = TRUE;
    obj->parameter.dsphy.ds_fec_bypass = FALSE;
    obj->parameter.dsphy.ds_fec_thrd = 0;
    obj->parameter.dsploam.ds_ploam_onuid_filter = TRUE;
    obj->parameter.dsploam.ds_ploam_broadcast_accpt = TRUE;
    obj->parameter.dsploam.ds_ploam_drop_crc_err = TRUE;
    obj->parameter.dsbwmap.ds_bwmap_crc_chk = TRUE;
    obj->parameter.dsbwmap.ds_bwmap_onuid_filter = TRUE;
    obj->parameter.dsbwmap.ds_bwmap_plend_mode = 0;
    obj->parameter.dsgem.assemble_timer = 16;
    obj->parameter.dseth.ds_eth_crc_chk = TRUE;
    obj->parameter.dseth.ds_eth_pti_mask = 0x01;
    obj->parameter.dseth.ds_eth_pti_ptn = 0x01;
    obj->parameter.dsomci.ds_omci_pti_mask = 0x01;
    obj->parameter.dsomci.ds_omci_pti_ptn = 0x01;
    obj->parameter.usphy.us_scramble_en = TRUE;
    obj->parameter.usphy.us_burst_en_polar = TRUE;
    obj->parameter.usphy.small_sstart_en = TRUE;
    obj->parameter.usphy.suppress_laser_en = FALSE;
    obj->parameter.usploam.us_ploam_en = TRUE;
    obj->parameter.usploam.us_ind_normal_ploam = TRUE;
    obj->parameter.usdbr.us_dbru_en = TRUE;
    obj->power_level = 0x01;
    osal_memset(&obj->aes_key,0x0,sizeof(rtk_gpon_aes_key_t));
    obj->chunk_header.head_size = 0x04;
    obj->chunk_header.task_ind = 0x00;
    obj->chunk_header.err_code = 0x02;
    obj->chunk_header.src = 0x05;
    obj->shaper.tick = 0;
    obj->shaper.inc_size = 0x80;
    obj->shaper.thrd_high = 0;
    obj->shaper.thrd_low = 0;
    obj->mactbl.op_mode = RTK_GPON_MACTBL_MODE_EXCLUDE;
    obj->mactbl.bc_pass = TRUE;
    obj->mactbl.non_mc_pass = TRUE;
    /* Add ipv4 force mode and ipv6 force mode in v1.2 */
    obj->mactbl.ipv4_force_mode = RTK_GPON_MCFORCE_MODE_NORMAL;
    obj->mactbl.ipv6_force_mode = RTK_GPON_MCFORCE_MODE_NORMAL;
    /* mc check pattern add IPv4 and IPv6 in GPON_MAC_SWIO_r1.1 */
    obj->mactbl.ipv4_mc_check_ptn = 0x01005E;
    obj->mactbl.ipv6_mc_check_ptn = 0x3333;
    obj->req_en = FALSE;
    obj->rdi = FALSE;
    obj->tdm_en = FALSE;
    obj->ds_fec = FALSE;
    obj->us_fec = FALSE;
    obj->us_ploam_req_urg = FALSE;
    obj->us_ploam_req_nrm = FALSE;
    obj->us_omci_flow = max_flow;
    obj->aes_key_switch_time = 0x0;
    obj->key_index = 0;
	obj->pre_eqd = 0;
    obj->eqd = 0;

    osal_memset(&obj->burst_overhead,0,sizeof(gpon_burst_oh_t));
    obj->p_tcont  = (gpon_tcont_obj_t**)GPON_OS_Malloc(sizeof(gpon_tcont_obj_t*)*max_tcont);
    if(obj->p_tcont == NULL)
        return RT_ERR_GPON_INITIAL_FAIL;
    obj->p_dsflow = (gpon_ds_flow_obj_t**)GPON_OS_Malloc(sizeof(gpon_ds_flow_obj_t*)*max_flow);
    if(obj->p_dsflow == NULL)
        return RT_ERR_GPON_INITIAL_FAIL;
    obj->p_usflow = (gpon_us_flow_obj_t**)GPON_OS_Malloc(sizeof(gpon_us_flow_obj_t*)*max_flow);
    if(obj->p_usflow == NULL)
        return RT_ERR_GPON_INITIAL_FAIL;

    for(i=0;i<max_tcont;i++)
    {
        obj->p_tcont[i] = NULL;
    }
    for(i=0;i<max_flow;i++)
    {
        obj->p_dsflow[i] = NULL;
        obj->p_usflow[i] = NULL;
    }
    for(i=0;i<GPON_DEV_MAX_MACTBL_NUM;i++)
    {
        obj->p_mac_tbl[i] = NULL;
    }
    for(i=0;i<RTK_GPON_ALARM_MAX;i++)
    {
        if (RTK_GPON_ALARM_LOS == i ||
            RTK_GPON_ALARM_LOF == i ||
            RTK_GPON_ALARM_LOM == i)
            obj->alarm_tbl[i] = TRUE;
        else
            obj->alarm_tbl[i] = FALSE;
    }

    obj->state_change_callback = NULL;
    obj->dsfec_change_callback = NULL;
    obj->usfec_change_callback = NULL;
    obj->usploam_urg_epty_callback = NULL;
    obj->usploam_nrm_epty_callback = NULL;
    obj->ploam_callback = NULL;
    obj->extMsgGet_callback = NULL;
    obj->extMsgSet_callback = NULL;
    #if 0
    obj->omci_callback = gpon_omci_cb;
    #endif
    obj->omci_callback = NULL;
    obj->aeskey_callback = NULL;
    for(i=0;i<RTK_GPON_ALARM_MAX;i++)
    {
        obj->fault_callback[i] = NULL;
    }
    obj->signal_callback = NULL;
    obj->cnt_boh_tx = 0;
    obj->cnt_ploamu_tx = 0;
    obj->cnt_dbru_tx = 0;
    obj->cnt_gem_tx = 0;

    obj->cnt_cpu_ploam_rx = 0;
    obj->cnt_cpu_omci_rx = 0;
    obj->cnt_cpu_ploam_tx = 0;
    obj->cnt_cpu_ploam_tx_nor = 0;
    obj->cnt_cpu_ploam_tx_urg = 0;
    obj->cnt_cpu_ploam_rx_unknown = 0;

    /* debug flag */
    obj->auto_tcont_state = 1;
    obj->auto_boh_state = 1;
    obj->eqd_offset = 0;

    /*BER timer */
    obj->ber_interval = 1000;

    obj->sf_threshold = 5;
    obj->sd_threshold = 9;
    obj->rdiMode = GPON_DRV_RDI_STD;
    /* DIS_TX GPIO*/
#if defined(CONFIG_SDK_KERNEL_LINUX)
    obj->gpio_disTx = CONFIG_TX_DISABLE_GPIO_PIN;
#endif

    obj->us_flow_eth_num = 0;
    obj->sd_mismatch_cnt = 0;
    obj->sd_toolong_cnt = 0;
    obj->pps_cnt = 0;

    obj->cnt_cpu_omci_tx =0;
    obj->cnt_cpu_omci_tx_nor =0;
    obj->cnt_cpu_omci_tx_urg =0;

    obj->us_gem_underflow_cnt = 0;
    obj->us_gem_overflow_cnt = 0;

#if 0 /* the reset is done in gpon_init */
    /* register reset */
    reset_done = 0;
    GPON_UTIL_ERR_CHK(rtk_gpon_resetState_set(ENABLED),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_resetState_set(DISABLED),ret);

    for(i=0;i<0xffff;i++)
    {
        rtk_gpon_resetDoneState_get(&rd);
        if(ENABLED == rd)
        {
            reset_done = 1;
            break;
        }
    }

    if(!reset_done)
    {
        osal_printf("gpon mac reset fail\n\r");
        GPON_OS_Log(GPON_LOG_LEVEL_ERROR,"register reset can't be finished [baseaddr:%p]",obj->base_addr);
        return RT_ERR_GPON_INITIAL_FAIL;
    }
#else /*FPGA_DEFINED*/
/* move the reset to dal_apollomp_gpon_driver_initialize */
#endif

    /* register test */
#if 1 /*!defined(CONFIG_SDK_ASICDRV_TEST)*/ /* for init ok, scott */
     GPON_UTIL_ERR_CHK(rtk_gpon_test_get(&rd),ret);

    if(rd!=GPON_TEST_VAL)
    {
        GPON_OS_Log(GPON_LOG_LEVEL_ERROR,"register read test fail (0x%04x) [baseaddr:%p]",rd,obj->base_addr);
        return RT_ERR_GPON_INITIAL_FAIL;
    }
#endif

#if 1 /* for init ok, scott */
     GPON_UTIL_ERR_CHK(rtk_gpon_test_set(0xEDCB),ret);

     GPON_UTIL_ERR_CHK(rtk_gpon_test_get(&rd),ret);


    if(rd!=0xEDCB)
    {
        GPON_OS_Log(GPON_LOG_LEVEL_ERROR,"register write test fail (0x%04x) [baseaddr:%p]",rd,obj->base_addr);
        return RT_ERR_GPON_INITIAL_FAIL;
    }
#endif

    GPON_UTIL_ERR_CHK(rtk_gpon_test_set(GPON_TEST_VAL),ret);

    /* register initialization */
    /* interrupt disable */
    GPON_UTIL_ERR_CHK(rtk_gpon_topIntr_disableAll(),ret);


    /* clean configurations, shoulc clean all tcont and flow of GPON MAC */
    for(i=0;i<GPON_DEV_MAX_TCONT_NUM;i++)
    {
       GPON_UTIL_ERR_CHK(rtk_gpon_usTcont_del(i),ret);
    }
    for(i=0;i<GPON_DEV_MAX_FLOW_NUM;i++)
    {
        GPON_UTIL_ERR_CHK(rtk_gpon_dsGemPort_del(i),ret);
    }
    for(i=0;i<GPON_DEV_MAX_MACTBL_NUM;i++)
    {
        GPON_UTIL_ERR_CHK(rtk_gpon_dsGemPortMacEntry_del(i),ret);
    }

    /* clear counters */
    for(i=0;i<max_flow;i++)
    {
        GPON_UTIL_ERR_CHK(rtk_gpon_dsGemPortPktCnt_get(i, &cnt32),ret);
        GPON_UTIL_ERR_CHK(rtk_gpon_dsGemPortByteCnt_get(i, &cnt32),ret);
        GPON_UTIL_ERR_CHK(rtk_gpon_dsGemPortEthRxCnt_get(i, &cnt32),ret);
        GPON_UTIL_ERR_CHK(rtk_gpon_dsGemPortEthFwdCnt_get(i, &cnt32),ret);
        GPON_UTIL_ERR_CHK(rtk_gpon_gemUsEthCnt_get(i, &cnt32),ret);
        GPON_UTIL_ERR_CHK(rtk_gpon_gemUsGemCnt_get(i, &cnt32),ret);
        GPON_UTIL_ERR_CHK(rtk_gpon_gemUsDataByteCnt_get(i, &cnt64),ret);
    }
    for(i=0;i<max_tcont;i++)
    {
        GPON_UTIL_ERR_CHK(rtk_gpon_gemUsIdleByteCnt_get(i, &cnt64),ret);
    }
    for(i=GPON_REG_DSGTCPMMISC_BIP_BLOCK_ERR;i<GPON_REG_DSGTCPMMISC_END;i++)
    {
        GPON_UTIL_ERR_CHK(rtk_gpon_dsGtcMiscCnt_get(i, &cnt32),ret);
    }
    for(i=GPON_REG_DSGEMPMMISC_MULTICAST_RX;i<GPON_REG_DSGEMPMMISC_END;i++)
    {
        GPON_UTIL_ERR_CHK(rtk_gpon_dsGemPortMiscCnt_get(i, &cnt32),ret);
    }
    for(i=GPON_REG_USGTCPMMISC_PLOAM_BOH_TX;i<GPON_REG_USGTCPMMISC_END;i++)
    {
        GPON_UTIL_ERR_CHK(rtk_gpon_usGtcMiscCnt_get(i, &cnt32),ret);
    }


    /* configuration initialization */
    GPON_UTIL_ERR_CHK(rtk_gpon_onuId_set(obj->onuid),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_onuState_set(obj->status),ret);

    GPON_UTIL_ERR_CHK(rtk_gpon_losCfg_set(obj->parameter.laser.laser_optic_los_en,obj->parameter.laser.laser_optic_los_polar,obj->parameter.laser.laser_cdr_los_en,obj->parameter.laser.laser_cdr_los_polar,obj->parameter.laser.laser_los_filter_en),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_laserTime_set(obj->parameter.laser.laser_us_on,obj->parameter.laser.laser_us_off),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_dsScrambleState_set(obj->parameter.dsphy.ds_scramble_en),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_dsFecBypass_set(obj->parameter.dsphy.ds_fec_bypass),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_dsFecThrd_set(obj->parameter.dsphy.ds_fec_thrd),ret);

    GPON_UTIL_ERR_CHK(rtk_gpon_dsPloamOnuIdFilterState_set(obj->parameter.dsploam.ds_ploam_onuid_filter),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_dsPloamBcAcceptState_set(obj->parameter.dsploam.ds_ploam_broadcast_accpt),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_dsPloamDropCrcState_set(obj->parameter.dsploam.ds_ploam_drop_crc_err),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_dsBwmapCrcCheckState_set(obj->parameter.dsbwmap.ds_bwmap_crc_chk),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_dsBwmapFilterOnuIdState_set(obj->parameter.dsbwmap.ds_bwmap_onuid_filter),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_dsPlendStrictMode_set(obj->parameter.dsbwmap.ds_bwmap_plend_mode),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_dsGemPortFrameTimeOut_set(obj->parameter.dsgem.assemble_timer),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_dsGemPortFcsCheckState_set(obj->parameter.dseth.ds_eth_crc_chk),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_dsEthPti_set(obj->parameter.dseth.ds_eth_pti_mask,obj->parameter.dseth.ds_eth_pti_ptn),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_dsOmciPti_set(obj->parameter.dsomci.ds_omci_pti_mask,obj->parameter.dsomci.ds_omci_pti_ptn),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_usScrambleState_set(obj->parameter.usphy.us_scramble_en),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_usBurstPolarity_set(obj->parameter.usphy.us_burst_en_polar),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_usSmalSstartProcState_set(obj->parameter.usphy.small_sstart_en),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_usSuppressLaserState_set(obj->parameter.usphy.suppress_laser_en),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_ploamState_set(obj->parameter.usploam.us_ploam_en),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_indNrmPloamState_set(obj->parameter.usploam.us_ind_normal_ploam),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_dbruState_set(obj->parameter.usdbr.us_dbru_en),ret);
    /* set the PTI value of OMCI and Ether.
       For ALU OLT, it only accept OMCI with NON_END_FRAG=0 and END_FRAG=1. */
    GPON_UTIL_ERR_CHK(rtk_gpon_gemUsPtiVector_set(0,1,0,1),ret);

    GPON_UTIL_ERR_CHK(rtk_gpon_dsGemPortBcPassState_set(obj->mactbl.bc_pass),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_dsGemPortNonMcPassState_set(obj->mactbl.non_mc_pass),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_ipv4McAddrPtn_set(obj->mactbl.ipv4_mc_check_ptn),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_ipv6McAddrPtn_set(obj->mactbl.ipv6_mc_check_ptn),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_dsGemPortMacFilterMode_set(obj->mactbl.op_mode),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_dsGemPortMacForceMode_set(GPON_IP_VER_V4,obj->mactbl.ipv4_force_mode),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_dsGemPortMacForceMode_set(GPON_IP_VER_V6,obj->mactbl.ipv6_force_mode),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_usRdi_set(obj->rdi),ret);
    /* default configurations */
    GPON_UTIL_ERR_CHK(rtk_gpon_forceLaser_set(RTK_GPON_LASER_STATUS_NORMAL),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_extraSnTxTimes_set(0),ret);

    GPON_UTIL_ERR_CHK(rtk_gpon_usPloamCrcGenState_set(TRUE),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_usPloamOnuIdFilterState_set(TRUE),ret);

    GPON_UTIL_ERR_CHK(rtk_gpon_aesKeyWordActive_set(obj->aes_key.key),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_aesKeySwitch_set(obj->aes_key_switch_time),ret);

    gpon_ploam_init(obj);

    /* for serial number set to ploamd */
    gpon_ploam_snPloam_set(obj);

#if !defined(FPGA_DEFINED)
    /* set ds_tx gpio to 0 */
    GPON_UTIL_ERR_CHK(rtk_gpio_databit_set(obj->gpio_disTx, 0), ret);
    /* set ds_tx gpio mode */
    GPON_UTIL_ERR_CHK(rtk_gpio_mode_set(obj->gpio_disTx, GPIO_OUTPUT), ret);
    /* enable ds_tx gpio */
    GPON_UTIL_ERR_CHK(rtk_gpio_state_set(obj->gpio_disTx, ENABLED), ret);
#endif

    GPON_OS_Log(GPON_LOG_LEVEL_WARNING,"GMac init success!");
    return RT_ERR_OK;
}

int32 gpon_dev_deInitialize(gpon_dev_obj_t* obj)
{
    int32 ret;
    uint32 i;
    uint32 max_tcont = obj->scheInfo.max_tcont;
    uint32 max_flow  = obj->scheInfo.max_flow;
    GPON_UTIL_ERR_CHK(rtk_gpon_topIntr_disableAll(),ret);
    if(obj->timer)
    {
        GPON_OS_Log(GPON_LOG_LEVEL_DEBUG,"stop timer %p [baseaddr:%p]",obj->timer,obj->base_addr);
        GPON_OS_StopTimer(obj->timer);
        obj->timer = 0;
    }
    for(i=0;i<max_tcont;i++)
    {
        GPON_OS_Free(obj->p_tcont[i]);
    }
    for(i=0;i<max_flow;i++)
    {
        GPON_OS_Free(obj->p_dsflow[i]);
        GPON_OS_Free(obj->p_usflow[i]);
    }
    return RT_ERR_OK;
}

int32 gpon_dev_sn_set(gpon_dev_obj_t* obj, rtk_gpon_serialNumber_t *sn)
{
    osal_memcpy(&obj->serial_number,sn,sizeof(rtk_gpon_serialNumber_t));

    return gpon_ploam_snPloam_set(obj);
}

int32 gpon_dev_sn_get(gpon_dev_obj_t* obj, rtk_gpon_serialNumber_t *sn)
{
    osal_memcpy(sn,&obj->serial_number,sizeof(rtk_gpon_serialNumber_t));
    return RT_ERR_OK;
}

int32 gpon_dev_pwd_set(gpon_dev_obj_t* obj, rtk_gpon_password_t *pwd)
{
    osal_memcpy(&obj->password,pwd,sizeof(rtk_gpon_password_t));
    return RT_ERR_OK;
}

int32 gpon_dev_pwd_get(gpon_dev_obj_t* obj, rtk_gpon_password_t *pwd)
{
    osal_memcpy(pwd,&obj->password,sizeof(rtk_gpon_password_t));
    return RT_ERR_OK;
}

int32 gpon_dev_para_onu_set(gpon_dev_obj_t* obj, rtk_gpon_onu_activation_para_t *para)
{
    if(obj->parameter.onu.to1_timer!=para->to1_timer)
    {
        obj->parameter.onu.to1_timer = para->to1_timer;
    }
    if(obj->parameter.onu.to2_timer!=para->to2_timer)
    {
        obj->parameter.onu.to2_timer = para->to2_timer;
    }
    return RT_ERR_OK;
}

int32 gpon_dev_para_onu_get(gpon_dev_obj_t* obj, rtk_gpon_onu_activation_para_t *para)
{
    osal_memcpy(para,&obj->parameter.onu,sizeof(rtk_gpon_onu_activation_para_t));
    return RT_ERR_OK;
}

int32 gpon_dev_para_laser_set(gpon_dev_obj_t* obj, rtk_gpon_laser_para_t *para)
{
    int32 ret;
    RT_PARAM_CHK(((para->laser_optic_los_en != 0) && (para->laser_optic_los_en != 1)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(((para->laser_optic_los_polar != 0) && (para->laser_optic_los_polar != 1)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(((para->laser_cdr_los_en != 0) && (para->laser_cdr_los_en != 1)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(((para->laser_cdr_los_polar != 0) && (para->laser_cdr_los_polar != 1)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(((para->laser_los_filter_en != 0) && (para->laser_los_filter_en != 1)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((para->laser_us_on > GPON_DEV_MAX_LASER_ON), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((para->laser_us_off > GPON_DEV_MAX_LASER_OFF), RT_ERR_OUT_OF_RANGE);

    if(obj->parameter.laser.laser_optic_los_en!=para->laser_optic_los_en
        || obj->parameter.laser.laser_optic_los_polar!=para->laser_optic_los_polar
        || obj->parameter.laser.laser_cdr_los_en!=para->laser_cdr_los_en
        || obj->parameter.laser.laser_cdr_los_polar!=para->laser_cdr_los_polar
        || obj->parameter.laser.laser_los_filter_en!=para->laser_los_filter_en)
    {
        obj->parameter.laser.laser_optic_los_en = para->laser_optic_los_en;
        obj->parameter.laser.laser_optic_los_polar = para->laser_optic_los_polar;
        obj->parameter.laser.laser_cdr_los_en = para->laser_cdr_los_en;
        obj->parameter.laser.laser_cdr_los_polar = para->laser_cdr_los_polar;
        obj->parameter.laser.laser_los_filter_en = para->laser_los_filter_en;
        GPON_UTIL_ERR_CHK(rtk_gpon_losCfg_set(obj->parameter.laser.laser_optic_los_en,obj->parameter.laser.laser_optic_los_polar,obj->parameter.laser.laser_cdr_los_en,obj->parameter.laser.laser_cdr_los_polar,obj->parameter.laser.laser_los_filter_en),ret);
    }
    if(obj->parameter.laser.laser_us_on!=para->laser_us_on || obj->parameter.laser.laser_us_off!=para->laser_us_off)
    {
        obj->parameter.laser.laser_us_on = para->laser_us_on;
        obj->parameter.laser.laser_us_off = para->laser_us_off;
        GPON_UTIL_ERR_CHK(rtk_gpon_laserTime_set(obj->parameter.laser.laser_us_on,obj->parameter.laser.laser_us_off),ret);
    }
    return RT_ERR_OK;
}

int32 gpon_dev_para_laser_get(gpon_dev_obj_t* obj, rtk_gpon_laser_para_t *para)
{
    para->laser_optic_los_en = obj->parameter.laser.laser_optic_los_en;
    para->laser_optic_los_polar = obj->parameter.laser.laser_optic_los_polar;
    para->laser_cdr_los_en = obj->parameter.laser.laser_cdr_los_en;
    para->laser_cdr_los_polar = obj->parameter.laser.laser_cdr_los_polar;
    para->laser_los_filter_en = obj->parameter.laser.laser_los_filter_en;
    para->laser_us_on = obj->parameter.laser.laser_us_on;
    para->laser_us_off = obj->parameter.laser.laser_us_off;

    return RT_ERR_OK;
}

int32 gpon_dev_para_dsPhy_set(gpon_dev_obj_t* obj, rtk_gpon_ds_physical_para_t *para)
{
    int32 ret;
    RT_PARAM_CHK(((para->ds_scramble_en != 0) && (para->ds_scramble_en != 1)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(((para->ds_fec_bypass != 0) && (para->ds_fec_bypass != 1)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((para->ds_fec_thrd > GPON_DEV_MAX_DS_FEC_DET_THRSH), RT_ERR_OUT_OF_RANGE);

    if(obj->parameter.dsphy.ds_scramble_en!=para->ds_scramble_en)
    {
        obj->parameter.dsphy.ds_scramble_en = para->ds_scramble_en;
        GPON_UTIL_ERR_CHK(rtk_gpon_dsScrambleState_set(obj->parameter.dsphy.ds_scramble_en),ret);
    }
    if(obj->parameter.dsphy.ds_fec_bypass!=para->ds_fec_bypass)
    {
        obj->parameter.dsphy.ds_fec_bypass = para->ds_fec_bypass;
        GPON_UTIL_ERR_CHK(rtk_gpon_dsFecBypass_set(obj->parameter.dsphy.ds_fec_bypass),ret);
    }
    if(obj->parameter.dsphy.ds_fec_thrd!=para->ds_fec_thrd)
    {
        obj->parameter.dsphy.ds_fec_thrd = para->ds_fec_thrd;
        GPON_UTIL_ERR_CHK(rtk_gpon_dsFecThrd_set(obj->parameter.dsphy.ds_fec_thrd),ret);
    }
    return RT_ERR_OK;
}

int gpon_dev_para_dsPhy_get(gpon_dev_obj_t* obj, rtk_gpon_ds_physical_para_t *para)
{
    para->ds_scramble_en = obj->parameter.dsphy.ds_scramble_en;
    para->ds_fec_bypass = obj->parameter.dsphy.ds_fec_bypass;
    para->ds_fec_thrd = obj->parameter.dsphy.ds_fec_thrd;

    return RT_ERR_OK;
}

int32 gpon_dev_para_dsPloam_set(gpon_dev_obj_t* obj, rtk_gpon_ds_ploam_para_t *para)
{
    int32 ret;
    RT_PARAM_CHK(((para->ds_ploam_onuid_filter != 0) && (para->ds_ploam_onuid_filter != 1)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(((para->ds_ploam_broadcast_accpt != 0) && (para->ds_ploam_broadcast_accpt != 1)), RT_ERR_OUT_OF_RANGE);
    /* Add in GPON_MAC_SWIO_v1.1 */
    RT_PARAM_CHK(((para->ds_ploam_drop_crc_err != 0) && (para->ds_ploam_drop_crc_err != 1)), RT_ERR_OUT_OF_RANGE);

    if(obj->parameter.dsploam.ds_ploam_onuid_filter!=para->ds_ploam_onuid_filter)
    {
        obj->parameter.dsploam.ds_ploam_onuid_filter = para->ds_ploam_onuid_filter;
        GPON_UTIL_ERR_CHK(rtk_gpon_dsPloamOnuIdFilterState_set(obj->parameter.dsploam.ds_ploam_onuid_filter),ret);
    }
    if(obj->parameter.dsploam.ds_ploam_broadcast_accpt!=para->ds_ploam_broadcast_accpt)
    {
        obj->parameter.dsploam.ds_ploam_broadcast_accpt = para->ds_ploam_broadcast_accpt;
        GPON_UTIL_ERR_CHK(rtk_gpon_dsPloamBcAcceptState_set(obj->parameter.dsploam.ds_ploam_broadcast_accpt),ret);
    }
    if(obj->parameter.dsploam.ds_ploam_drop_crc_err!=para->ds_ploam_drop_crc_err)
    {
        obj->parameter.dsploam.ds_ploam_drop_crc_err = para->ds_ploam_drop_crc_err;
        GPON_UTIL_ERR_CHK(rtk_gpon_dsPloamDropCrcState_set(obj->parameter.dsploam.ds_ploam_drop_crc_err),ret);
    }
    return RT_ERR_OK;
}

int32 gpon_dev_para_dsPloam_get(gpon_dev_obj_t* obj, rtk_gpon_ds_ploam_para_t *para)
{
    osal_memcpy(para,&obj->parameter.dsploam,sizeof(rtk_gpon_ds_ploam_para_t));
    return RT_ERR_OK;
}

int32 gpon_dev_para_dsBwmap_set(gpon_dev_obj_t* obj, rtk_gpon_ds_bwMap_para_t *para)
{
    int32 ret;
    RT_PARAM_CHK(((para->ds_bwmap_crc_chk != 0) && (para->ds_bwmap_crc_chk != 1)), RT_ERR_OUT_OF_RANGE);
    /* Added in GPON_MAC_SWIO_v1.1 */
    RT_PARAM_CHK(((para->ds_bwmap_onuid_filter != 0) && (para->ds_bwmap_onuid_filter != 1)), RT_ERR_OUT_OF_RANGE);

    RT_PARAM_CHK(((para->ds_bwmap_plend_mode != 0) && (para->ds_bwmap_plend_mode != 1)), RT_ERR_OUT_OF_RANGE);

    if(obj->parameter.dsbwmap.ds_bwmap_crc_chk!=para->ds_bwmap_crc_chk)
    {
        obj->parameter.dsbwmap.ds_bwmap_crc_chk = para->ds_bwmap_crc_chk;
        GPON_UTIL_ERR_CHK(rtk_gpon_dsBwmapCrcCheckState_set(obj->parameter.dsbwmap.ds_bwmap_crc_chk),ret);
    }
    if(obj->parameter.dsbwmap.ds_bwmap_onuid_filter!=para->ds_bwmap_onuid_filter)
    {
        obj->parameter.dsbwmap.ds_bwmap_onuid_filter = para->ds_bwmap_onuid_filter;
        GPON_UTIL_ERR_CHK(rtk_gpon_dsBwmapFilterOnuIdState_set(obj->parameter.dsbwmap.ds_bwmap_onuid_filter),ret);
    }
    if(obj->parameter.dsbwmap.ds_bwmap_plend_mode!=para->ds_bwmap_plend_mode)
    {
        obj->parameter.dsbwmap.ds_bwmap_plend_mode = para->ds_bwmap_plend_mode;
        GPON_UTIL_ERR_CHK(rtk_gpon_dsPlendStrictMode_set(obj->parameter.dsbwmap.ds_bwmap_plend_mode),ret);
    }
    return RT_ERR_OK;
}

int32 gpon_dev_para_dsBwmap_get(gpon_dev_obj_t* obj, rtk_gpon_ds_bwMap_para_t *para)
{
    osal_memcpy(para,&obj->parameter.dsbwmap,sizeof(rtk_gpon_ds_bwMap_para_t));
    return RT_ERR_OK;
}

int32 gpon_dev_para_dsGem_set(gpon_dev_obj_t* obj, rtk_gpon_ds_gem_para_t *para)
{
    int32 ret;
    RT_PARAM_CHK((para->assemble_timer > GPON_DEV_MAX_ASSM_TIMEOUT_FRM), RT_ERR_OUT_OF_RANGE);

    if(obj->parameter.dsgem.assemble_timer!=para->assemble_timer)
    {
        obj->parameter.dsgem.assemble_timer = para->assemble_timer;
        GPON_UTIL_ERR_CHK(rtk_gpon_dsGemPortFrameTimeOut_set(obj->parameter.dsgem.assemble_timer),ret);
    }
    return RT_ERR_OK;
}

int32 gpon_dev_para_dsGem_get(gpon_dev_obj_t* obj, rtk_gpon_ds_gem_para_t *para)
{
    para->assemble_timer = obj->parameter.dsgem.assemble_timer;

    return RT_ERR_OK;
}

int32 gpon_dev_para_dsEth_set(gpon_dev_obj_t* obj, rtk_gpon_ds_eth_para_t *para)
{

    int32 ret;
    RT_PARAM_CHK(((para->ds_eth_crc_chk != 0) && (para->ds_eth_crc_chk != 1)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((para->ds_eth_pti_mask > GPON_DEV_MAX_PTI), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((para->ds_eth_pti_ptn > GPON_DEV_MAX_PTI), RT_ERR_OUT_OF_RANGE);

    if(obj->parameter.dseth.ds_eth_crc_chk!=para->ds_eth_crc_chk)
    {
        obj->parameter.dseth.ds_eth_crc_chk = para->ds_eth_crc_chk;
        GPON_UTIL_ERR_CHK(rtk_gpon_dsGemPortFcsCheckState_set(obj->parameter.dseth.ds_eth_crc_chk),ret);
    }
    if(obj->parameter.dseth.ds_eth_pti_mask!=para->ds_eth_pti_mask || obj->parameter.dseth.ds_eth_pti_ptn!=para->ds_eth_pti_ptn)
    {
        obj->parameter.dseth.ds_eth_pti_mask = para->ds_eth_pti_mask;
        obj->parameter.dseth.ds_eth_pti_ptn = para->ds_eth_pti_ptn;
        GPON_UTIL_ERR_CHK(rtk_gpon_dsEthPti_set(obj->parameter.dseth.ds_eth_pti_mask,obj->parameter.dseth.ds_eth_pti_ptn),ret);
    }
    return RT_ERR_OK;
}

int32 gpon_dev_para_dsEth_get(gpon_dev_obj_t* obj, rtk_gpon_ds_eth_para_t *para)
{
    para->ds_eth_crc_chk = obj->parameter.dseth.ds_eth_crc_chk;
    para->ds_eth_pti_mask = obj->parameter.dseth.ds_eth_pti_mask;
    para->ds_eth_pti_ptn = obj->parameter.dseth.ds_eth_pti_ptn;

    return RT_ERR_OK;
}

int32 gpon_dev_para_dsOmci_set(gpon_dev_obj_t* obj, rtk_gpon_ds_omci_para_t *para)
{
    int32 ret;
    RT_PARAM_CHK((para->ds_omci_pti_mask > GPON_DEV_MAX_PTI), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((para->ds_omci_pti_ptn > GPON_DEV_MAX_PTI), RT_ERR_OUT_OF_RANGE);

    if(obj->parameter.dsomci.ds_omci_pti_mask!=para->ds_omci_pti_mask || obj->parameter.dsomci.ds_omci_pti_ptn!=para->ds_omci_pti_ptn)
    {
        obj->parameter.dsomci.ds_omci_pti_mask = para->ds_omci_pti_mask;
        obj->parameter.dsomci.ds_omci_pti_ptn = para->ds_omci_pti_ptn;
        GPON_UTIL_ERR_CHK(rtk_gpon_dsOmciPti_set(obj->parameter.dsomci.ds_omci_pti_mask,obj->parameter.dsomci.ds_omci_pti_ptn),ret);
    }
    return RT_ERR_OK;
}

int32 gpon_dev_para_dsOmci_get(gpon_dev_obj_t* obj, rtk_gpon_ds_omci_para_t *para)
{
    para->ds_omci_pti_mask = obj->parameter.dsomci.ds_omci_pti_mask;
    para->ds_omci_pti_ptn = obj->parameter.dsomci.ds_omci_pti_ptn;

    return RT_ERR_OK;
}

int32 gpon_dev_para_usPhy_set(gpon_dev_obj_t* obj, rtk_gpon_us_physical_para_t *para)
{
    int32 ret;
    RT_PARAM_CHK(((para->us_scramble_en != 0) && (para->us_scramble_en != 1)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(((para->us_burst_en_polar != 0) && (para->us_burst_en_polar != 1)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(((para->small_sstart_en != 0) && (para->small_sstart_en != 1)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(((para->suppress_laser_en != 0) && (para->suppress_laser_en != 1)), RT_ERR_OUT_OF_RANGE);

    if(obj->parameter.usphy.us_scramble_en!=para->us_scramble_en)
    {
        obj->parameter.usphy.us_scramble_en = para->us_scramble_en;
        GPON_UTIL_ERR_CHK(rtk_gpon_usScrambleState_set(obj->parameter.usphy.us_scramble_en),ret);
    }
    if(obj->parameter.usphy.us_burst_en_polar!=para->us_burst_en_polar)
    {
        obj->parameter.usphy.us_burst_en_polar = para->us_burst_en_polar;
        GPON_UTIL_ERR_CHK(rtk_gpon_usBurstPolarity_set(obj->parameter.usphy.us_burst_en_polar),ret);
    }
    if(obj->parameter.usphy.small_sstart_en!=para->small_sstart_en)
    {
        obj->parameter.usphy.small_sstart_en = para->small_sstart_en;
        GPON_UTIL_ERR_CHK(rtk_gpon_usSmalSstartProcState_set(obj->parameter.usphy.small_sstart_en),ret);
    }
    if(obj->parameter.usphy.suppress_laser_en!=para->suppress_laser_en)
    {
        obj->parameter.usphy.suppress_laser_en = para->suppress_laser_en;
        GPON_UTIL_ERR_CHK(rtk_gpon_usSuppressLaserState_set(obj->parameter.usphy.suppress_laser_en),ret);
    }
    return RT_ERR_OK;
}

int32 gpon_dev_para_usPhy_get(gpon_dev_obj_t* obj, rtk_gpon_us_physical_para_t *para)
{
    osal_memcpy(para,&obj->parameter.usphy,sizeof(rtk_gpon_us_physical_para_t));
    return RT_ERR_OK;
}

int32 gpon_dev_para_usPloam_set(gpon_dev_obj_t* obj, rtk_gpon_us_ploam_para_t *para)
{
    int32 ret;
    RT_PARAM_CHK(((para->us_ploam_en != 0) && (para->us_ploam_en != 1)), RT_ERR_OUT_OF_RANGE);

    if(obj->parameter.usploam.us_ploam_en!=para->us_ploam_en)
    {
        obj->parameter.usploam.us_ploam_en = para->us_ploam_en;
        GPON_UTIL_ERR_CHK(rtk_gpon_ploamState_set(obj->parameter.usploam.us_ploam_en),ret);
    }
    return RT_ERR_OK;
}

int32 gpon_dev_para_usPloam_get(gpon_dev_obj_t* obj, rtk_gpon_us_ploam_para_t *para)
{
    osal_memcpy(para,&obj->parameter.usploam,sizeof(rtk_gpon_us_ploam_para_t));
    return RT_ERR_OK;
}

int32 gpon_dev_para_usDbr_set(gpon_dev_obj_t* obj, rtk_gpon_us_dbr_para_t *para)
{
    int32 ret;
    RT_PARAM_CHK(((para->us_dbru_en != 0) && (para->us_dbru_en != 1)), RT_ERR_OUT_OF_RANGE);

    if(obj->parameter.usdbr.us_dbru_en!=para->us_dbru_en)
    {
        obj->parameter.usdbr.us_dbru_en = para->us_dbru_en;
        GPON_UTIL_ERR_CHK(rtk_gpon_dbruState_set(obj->parameter.usdbr.us_dbru_en),ret);
    }

    return RT_ERR_OK;
}

int32 gpon_dev_para_usDbr_get(gpon_dev_obj_t* obj, rtk_gpon_us_dbr_para_t *para)
{
    osal_memcpy(para,&obj->parameter.usdbr,sizeof(rtk_gpon_us_dbr_para_t));
    return RT_ERR_OK;
}

int32 gpon_dev_para_onuId_get(gpon_dev_obj_t* obj, uint8 *para)
{
    *para = obj->onuid;
    return RT_ERR_OK;
}

int32 gpon_dev_para_drvStatus_get(gpon_drv_obj_t* obj, gpon_drv_status_t *para)
{
    *para = obj->status;
    return RT_ERR_OK;
}


int32 gpon_dev_activate(gpon_dev_obj_t* obj, rtk_gpon_initialState_t initState)
{
    uint32 ds_fec, us_fec;
    int32 ret;
    GPON_OS_Log(GPON_LOG_LEVEL_WARNING,"Device Active %d [baseaddr:%p]",initState,obj->base_addr);

    obj->cnt_boh_tx = 0;
    obj->cnt_ploamu_tx = 0;
    obj->cnt_dbru_tx = 0;
    obj->cnt_gem_tx = 0;

    obj->cnt_cpu_ploam_rx = 0;
    obj->cnt_cpu_omci_rx = 0;
    obj->cnt_cpu_ploam_tx = 0;
    obj->cnt_cpu_ploam_tx_nor = 0;
    obj->cnt_cpu_ploam_tx_urg = 0;
    obj->cnt_cpu_ploam_rx_unknown = 0;

    if(initState==RTK_GPONMAC_INIT_STATE_O7)
    {
        obj->status = GPON_STATE_O7;

        /* set TX disable when initial state is O7 */
        GPON_UTIL_ERR_CHK(gpon_dev_rogueOntDisTx_set(obj, DISABLED),ret);
    }
    else
    {
        obj->status = GPON_STATE_O1;

        /* set TX enable when initial state is O1 */
        GPON_UTIL_ERR_CHK(gpon_dev_rogueOntDisTx_set(obj, ENABLED),ret);
    }

    /* Set State to chip */
    GPON_UTIL_ERR_CHK(rtk_gpon_onuId_set(obj->onuid),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_onuState_set(obj->status),ret);

    /* Check Alarm State */
    gpon_alarm_status_check(obj);

    /* Check FEC State */
    GPON_UTIL_ERR_CHK(rtk_gpon_gtcDsIntr_get(GPON_DS_FEC_STS_DLT,&ds_fec),ret);
    gpon_dev_dsFec_report(obj,ds_fec);

    GPON_UTIL_ERR_CHK(rtk_gpon_gtcUsIntr_get(GPON_US_FEC_STS,&us_fec),ret);
    gpon_dev_usFec_report(obj,us_fec);

    /* Enable INT */
    GPON_UTIL_ERR_CHK(rtk_gpon_gtcDsIntrMask_set(GPON_PLM_BUF_REQ, ENABLED),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_gtcDsIntrMask_set(GPON_RNG_REQ_HIS, ENABLED),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_gtcDsIntrMask_set(GPON_SN_REQ_HIS, ENABLED),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_gtcDsIntrMask_set(GPON_LOM_DLT, ENABLED),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_gtcDsIntrMask_set(GPON_DS_FEC_STS_DLT, ENABLED),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_gtcDsIntrMask_set(GPON_LOF_DLT, ENABLED),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_gtcDsIntrMask_set(GPON_LOS_DLT, ENABLED),ret);

    GPON_UTIL_ERR_CHK(rtk_gpon_gtcUsIntrMask_set(GPON_US_FEC_STS,ENABLED),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_gtcUsIntrMask_set(GPON_PLM_NRM_EMPTY,ENABLED),ret);
    GPON_UTIL_ERR_CHK(rtk_gpon_gtcUsIntrMask_set(GPON_PLM_URG_EMPTY,ENABLED),ret);

#ifdef CONFIG_GPON_ROGUE_SELF_DETECT /* The OE module should support TX SD pin */
    GPON_UTIL_ERR_CHK(rtk_gpon_gtcUsIntrMask_set(GPON_OPTIC_SD_MISM,ENABLED),ret);
    /*GPON_UTIL_ERR_CHK(rtk_gpon_gtcUsIntrMask_set(GPON_OPTIC_SD_TOOLONG,ENABLED),ret);*/
#endif

    return RT_ERR_OK;
}

int32 gpon_dev_deactivate(gpon_dev_obj_t* obj)
{
    int32 ret;
    /* Disable INT */
    GPON_UTIL_ERR_CHK(rtk_gpon_topIntr_disableAll(),ret);

    /* deinit ploam: send DyingGasp to OLT */
    /*gpon_ploam_deinit(obj);*/

    gpon_alarm_status_reset(obj);

    /* Check FEC State */
    GPON_UTIL_ERR_CHK(gpon_dev_dsFec_report(obj,FALSE),ret);
    GPON_UTIL_ERR_CHK(gpon_dev_usFec_report(obj,FALSE),ret);

    /* Reset status */
    if(obj->status!=GPON_STATE_O7)
    {
        obj->status = GPON_STATE_O1;
    }

    /* Stop Timer */
    if(obj->timer)
    {
        GPON_OS_Log(GPON_LOG_LEVEL_DEBUG,"stop timer %p [baseaddr:%p]",obj->timer,obj->base_addr);
        GPON_OS_StopTimer(obj->timer);
        obj->timer = 0;
    }
    if(obj->ber_timer)
    {
        GPON_OS_Log(GPON_LOG_LEVEL_DEBUG,"stop ber timer %p",obj->ber_timer);
        GPON_OS_StopTimer(obj->ber_timer);
        obj->ber_timer = 0;
    }

    /* Set State to chip */
    GPON_UTIL_ERR_CHK(rtk_gpon_onuState_set(obj->status),ret);

    /* Set ONU_ID to default ONU_ID 255 */
    GPON_UTIL_ERR_CHK(gpon_dev_onuid_set(obj,GPON_DEV_DEFAULT_ONU_ID),ret);

    if(obj->status!=GPON_STATE_O7)
    {
        gpon_fsm_event(obj, GPON_FSM_EVENT_LOS_DETECT);
    }

    GPON_OS_Log(GPON_LOG_LEVEL_WARNING,"Device DeActive [baseaddr:%p]",obj->base_addr);

    return RT_ERR_OK;
}

int32 gpon_dev_get_status(gpon_dev_obj_t* obj, rtk_gpon_fsm_status_t* status)
{
    *status = obj->status;
    return RT_ERR_OK;
}

int32 gpon_dev_get_fec_status(gpon_dev_obj_t* obj, int32 *ds, int32 *us)
{
    *ds = obj->ds_fec;
    *us = obj->us_fec;
    return RT_ERR_OK;
}

int32 gpon_dev_dsFec_report(gpon_dev_obj_t* obj, int32 status)
{
    if(obj->ds_fec!=status)
    {
        obj->ds_fec = status;
        GPON_OS_Log(GPON_LOG_LEVEL_WARNING,"D/S FEC Status Change Event: FEC %s [baseaddr:%p]",(obj->ds_fec)?"ON":"OFF",obj->base_addr);
        if(obj->dsfec_change_callback)
        {
            (*obj->dsfec_change_callback)(obj->ds_fec);
        }
    }
    if(status)
    {
        int32 ret;
        uint32 addr;
        /* get superframe, page 1, offset 0x12 */
        addr = (1 << 12) | (0x12)*4 | 0x700000;
        if((ret = ioal_mem32_read(addr, &obj->ds_fec_superframe)) != RT_ERR_OK)
        {
            osal_printf("gpon_dev_dsFec_report: get superframe fail! ret=%x\n",ret);
        }
        /*osal_printf("gpon_dev_dsFec_report: superframe = %d\n",obj->ds_fec_superframe);*/
    }
    else
    {
        obj->ds_fec_superframe = 0;
    }
    return RT_ERR_OK;
}

int32 gpon_dev_usFec_report(gpon_dev_obj_t* obj, int32 status)
{
    if(obj->us_fec!=status)
    {
        obj->us_fec = status;
        GPON_OS_Log(GPON_LOG_LEVEL_WARNING,"U/S FEC Status Change Event: FEC %s [baseaddr:%p]",(obj->us_fec)?"ON":"OFF",obj->base_addr);
        if(obj->usfec_change_callback)
        {
            (*obj->usfec_change_callback)(obj->us_fec);
        }
    }
    return RT_ERR_OK;
}

int32 gpon_dev_upstreamOverhead_calculate(gpon_dev_obj_t* obj)
{
    gpon_burst_oh_t* overhead = &obj->burst_overhead;

    osal_memset(overhead->bursthead,0xaa,GPON_DEV_BURST_OVERHEAD_LEN*sizeof(uint8));

    /* Set the max guard bits to 32. */
    if (overhead->guard_bits > 32)
    {
        overhead->guard_bits = 32;
    }
#if 0 /* type1_bits and type2_bit is no used now, and be added in to type3_len */
    gpon_res_set_bits(overhead->bursthead,overhead->guard_bits,overhead->guard_bits+overhead->type1_bits-1,1);
    overhead->boh_repeat = (overhead->guard_bits+overhead->type1_bits+overhead->type2_bits)/8;
#else
    overhead->boh_repeat = (uint8)((overhead->guard_bits)/8);
#endif
    overhead->bursthead[overhead->boh_repeat] = overhead->type3_ptn;
    overhead->bursthead[overhead->boh_repeat+1] = overhead->delimiter[0];
    overhead->bursthead[overhead->boh_repeat+2] = overhead->delimiter[1];
    overhead->bursthead[overhead->boh_repeat+3] = overhead->delimiter[2];

    if(overhead->type3_preranged)
    {
#if 0   /* type1_bits and type2_bit is no used now, and be added in to type3_len */
        overhead->boh_len_preranged = overhead->boh_repeat+overhead->type3_preranged+3;
#else
        overhead->boh_len_preranged = overhead->boh_repeat+overhead->type3_preranged+3 + (overhead->type1_bits+overhead->type2_bits)/8;
#endif
        if(overhead->boh_len_preranged > GPON_DEV_MAX_BOH_LEN)
        {
            overhead->boh_len_preranged = GPON_DEV_MAX_BOH_LEN;
        }
    }
    else
    {
        overhead->boh_len_preranged = GPON_DEV_TOTAL_OVERHEAD_BITS/8;
    }

    if(overhead->type3_ranged)
    {
#if 0   /* type1_bits and type2_bit is no used now, and be added in to type3_len */
        overhead->boh_len_ranged = overhead->boh_repeat+overhead->type3_ranged+3;
#else
        overhead->boh_len_ranged = overhead->boh_repeat+overhead->type3_ranged+3 + (overhead->type1_bits+overhead->type2_bits)/8;
#endif
        if(overhead->boh_len_ranged > GPON_DEV_MAX_BOH_LEN)
        {
            overhead->boh_len_ranged = GPON_DEV_MAX_BOH_LEN;
        }
    }
    else
    {
        overhead->boh_len_ranged = GPON_DEV_TOTAL_OVERHEAD_BITS/8;
    }
    return RT_ERR_OK;
}

int32 gpon_dev_burstHead_preRanged_set(gpon_dev_obj_t* obj)
{
    uint8 i,len;
    int32 ret;
    uint8 oh[GPON_DEV_BURST_OVERHEAD_LEN];

    if(obj->burst_overhead.boh_len_preranged>GPON_DEV_BURST_OVERHEAD_LEN)
    {
        len = GPON_DEV_BURST_OVERHEAD_LEN;
    }
    else
    {
        len = obj->burst_overhead.boh_len_preranged;
    }

    osal_memset(oh,0,GPON_DEV_BURST_OVERHEAD_LEN*sizeof(uint8));

    for(i=0;i<obj->burst_overhead.boh_repeat;i++)
    {
        oh[i] = obj->burst_overhead.bursthead[i];
    }
    for(;i<len-3;i++)
    {
        oh[i] = obj->burst_overhead.bursthead[obj->burst_overhead.boh_repeat];
    }
    for(;i<len;i++)
    {
        oh[i] = obj->burst_overhead.bursthead[obj->burst_overhead.boh_repeat+i+4-len];
    }

    if(1 == obj->auto_boh_state)
    {
        /*GPON_OS_Log(GPON_LOG_LEVEL_NORMAL,"Set BOH: repeat %d, length %d, %02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x [baseaddr:%p]",
            obj->burst_overhead.boh_repeat,obj->burst_overhead.boh_len_preranged,oh[0],oh[1],oh[2],oh[3],oh[4],oh[5],oh[6],oh[7],oh[8],oh[9],oh[10],oh[11],obj->base_addr);*/
        GPON_UTIL_ERR_CHK(rtk_gpon_burstOverhead_set(obj->burst_overhead.boh_repeat,obj->burst_overhead.boh_len_preranged,len,oh),ret);
    }

    return RT_ERR_OK;
}

int32 gpon_dev_burstHead_ranged_set(gpon_dev_obj_t* obj)
{
    uint8 i,len;
    int32 ret;
    uint8 oh[GPON_DEV_BURST_OVERHEAD_LEN];
    if(obj->burst_overhead.boh_len_ranged>GPON_DEV_BURST_OVERHEAD_LEN)
    {
        len = GPON_DEV_BURST_OVERHEAD_LEN;
    }
    else
    {
        len = obj->burst_overhead.boh_len_ranged;
    }

    osal_memset(oh,0,GPON_DEV_BURST_OVERHEAD_LEN*sizeof(uint8));

    for(i=0;i<obj->burst_overhead.boh_repeat;i++)
    {
        oh[i] = obj->burst_overhead.bursthead[i];
    }
    for(;i<len-3;i++)
    {
        oh[i] = obj->burst_overhead.bursthead[obj->burst_overhead.boh_repeat];
    }
    for(;i<len;i++)
    {
        oh[i] = obj->burst_overhead.bursthead[obj->burst_overhead.boh_repeat+i+4-len];
    }

    if(1 == obj->auto_boh_state)
    {
        /*GPON_OS_Log(GPON_LOG_LEVEL_NORMAL,"Set BOH: repeat %d, length %d, %02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x [baseaddr:%p]",
            obj->burst_overhead.boh_repeat,obj->burst_overhead.boh_len_ranged,oh[0],oh[1],oh[2],oh[3],oh[4],oh[5],oh[6],oh[7],oh[8],oh[9],oh[10],oh[11],obj->base_addr);*/
        GPON_UTIL_ERR_CHK(rtk_gpon_burstOverhead_set(obj->burst_overhead.boh_repeat,obj->burst_overhead.boh_len_ranged,len,oh),ret);
    }

    return RT_ERR_OK;
}

int32 gpon_dev_onuid_set(gpon_dev_obj_t* obj, uint8 onuid)
{
    int32 ret;
    if(GPON_DEV_DEFAULT_ONU_ID==onuid && obj->onuid!=GPON_DEV_DEFAULT_ONU_ID)
    {
        if((ret = gpon_dev_tcont_physical_reset(obj))!=RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_GPON), "");
        }
        if((ret = gpon_dev_dsFlow_del(obj, obj->scheInfo.omcc_flow))!=RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_GPON), "");
        }
        if((ret = gpon_dev_usFlow_del(obj, obj->scheInfo.omcc_flow))!=RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_GPON), "");
        }
    }

    obj->onuid = onuid;
    GPON_OS_Log(GPON_LOG_LEVEL_NORMAL,"set ONU ID %d [baseaddr:%p]",obj->onuid,obj->base_addr);
    GPON_UTIL_ERR_CHK(rtk_gpon_onuId_set(obj->onuid),ret);


    /* add default tcont */
    if(onuid!=GPON_DEV_DEFAULT_ONU_ID)
    {
        GPON_UTIL_ERR_CHK(gpon_dev_tcont_physical_add(obj,onuid,NULL),ret);
    }

    return RT_ERR_OK;
}

int32 gpon_dev_tcont_logical_add(gpon_dev_obj_t* obj, rtk_gpon_tcont_ind_t* ind, rtk_gpon_tcont_attr_t* attr)
{
    uint32 i;
    uint32 max_tcont = obj->scheInfo.max_tcont;
    uint32 idle = max_tcont;
    uint8 forbid_tcont[max_tcont];
    rtk_ponmac_aggregate_tcont_t aggIndex;
    rtk_enable_t state;

    /* for aggregate tcont check, if aggregate tcont enable, the slave tconts should not be added */
    osal_memset(forbid_tcont, 0x0, sizeof(uint8)*max_tcont);
    for(aggIndex=0; aggIndex<PONMAC_AGG_TCONT_END; aggIndex++)
    {
        if(rtk_ponmac_aggTcontState_get(aggIndex, &state) != RT_ERR_OK)
            continue;
        if(state == ENABLED)
        {
            for(i=aggIndex*4+1; i<=aggIndex*4+3; i++)
            {
                forbid_tcont[i] = 1;
            }
        }
    }

    /* search alloc id from table */
#if 0 /* the DBA TCONT(p_tcont[0]) is not support, the entry 0 can be used by normal tcont */
    for(i=1;i<max_tcont;i++)
#else
    for(i=0;i<max_tcont;i++)
#endif
    {
        /* Search an idle tcont */
        if(idle == max_tcont && !obj->p_tcont[i])
        {
            if(forbid_tcont[i] == 0)
            {
                idle = i;
            }
        }
        /* found it */
        if(obj->p_tcont[i] && obj->p_tcont[i]->alloc_id==ind->alloc_id)
        {
            break;
        }
    }

    if(i==max_tcont)    /* not found */
    {
        if(idle==max_tcont)    /* no idle tcont */
        {
            return RT_ERR_GPON_EXCEED_MAX_TCONT;
        }
        else    /* create new tcont */
        {
            obj->p_tcont[idle] = (gpon_tcont_obj_t*)GPON_OS_Malloc(sizeof(gpon_tcont_obj_t));
            if(obj->p_tcont[idle] == NULL)
                return RT_ERR_GPON_INVALID_HANDLE;
            obj->p_tcont[idle]->tcont_id = idle;
            obj->p_tcont[idle]->alloc_id = ind->alloc_id;
            obj->p_tcont[idle]->type = ind->type;
            obj->p_tcont[idle]->ploam_en = FALSE;
            obj->p_tcont[idle]->omci_en = TRUE;
            attr->tcont_id = obj->p_tcont[idle]->tcont_id;
            GPON_OS_Log(GPON_LOG_LEVEL_NORMAL,"Create Logical TCont %d: alloc %d, type %d [baseaddr:%p]",idle,obj->p_tcont[idle]->alloc_id,obj->p_tcont[idle]->type,obj->base_addr);
            return RT_ERR_OK;
        }
    }
    else    /* found */
    {
        if(forbid_tcont[i] == 1)
            return RT_ERR_GPON_FORBID_TCONT_ID;

        attr->tcont_id = obj->p_tcont[i]->tcont_id;
        if(obj->p_tcont[i]->omci_en)    /* added before */
        {
            return RT_ERR_INPUT;
        }
        else
        {
            obj->p_tcont[i]->omci_en = TRUE;
            obj->p_tcont[i]->type = ind->type;

            GPON_OS_Log(GPON_LOG_LEVEL_NORMAL,"Enable Logical TCont %d: alloc %d, type %d [baseaddr:%p]",i,obj->p_tcont[i]->alloc_id,obj->p_tcont[i]->type,obj->base_addr);
            return RT_ERR_OK;
        }
    }
}

int32 gpon_dev_tcont_logical_del(gpon_dev_obj_t* obj, rtk_gpon_tcont_ind_t* ind)
{
    uint32 i;
    uint32 max_tcont = obj->scheInfo.max_tcont;

    /* search alloc id from table */
#if 0 /* the DBA TCONT(p_tcont[0]) is not support, the entry 0 can be used by normal tcont */
    for(i=1;i<max_tcont;i++)
#else
    for(i=0;i<max_tcont;i++)
#endif
    {
        if(obj->p_tcont[i] && obj->p_tcont[i]->alloc_id==ind->alloc_id)
        {
            /* found it */
            break;
        }
    }

    if(i==max_tcont)    /* not found */
    {
        return RT_ERR_INPUT;
    }
    else    /* found */
    {
        if(!obj->p_tcont[i]->omci_en)    /* not added */
        {
            return RT_ERR_INPUT;
        }
        else
        {
            /* remove from logical */
            obj->p_tcont[i]->omci_en = FALSE;

            /* if removed from physical, free it */
            if(!obj->p_tcont[i]->ploam_en)
            {
                GPON_OS_Log(GPON_LOG_LEVEL_NORMAL,"Remove Logical TCont %d: alloc %d [baseaddr:%p]",i,obj->p_tcont[i]->alloc_id,obj->base_addr);
                GPON_OS_Free(obj->p_tcont[i]);
                obj->p_tcont[i] = NULL;
            }
            else
            {
                GPON_OS_Log(GPON_LOG_LEVEL_NORMAL,"Disable Logical TCont %d: alloc %d [baseaddr:%p]",i,obj->p_tcont[i]->alloc_id,obj->base_addr);
            }
            return RT_ERR_OK;
        }
    }
}

int32 gpon_dev_tcont_logical_get(gpon_dev_obj_t* obj, rtk_gpon_tcont_ind_t* ind, rtk_gpon_tcont_attr_t* attr)
{
    uint32 i;
    uint32 max_tcont = obj->scheInfo.max_tcont;

    /* search alloc id from table */
#if 0 /* the DBA TCONT(p_tcont[0]) is not support, the entry 0 can be used by normal tcont */
    for(i=1;i<max_tcont;i++)
#else
    for(i=0;i<max_tcont;i++)
#endif
    {
        if(obj->p_tcont[i] && obj->p_tcont[i]->alloc_id==ind->alloc_id)
        {
            /* found it */
            break;
        }
    }

    attr->tcont_id = max_tcont;

    if(i==max_tcont)    /* not found */
    {
        GPON_OS_Log(GPON_LOG_LEVEL_NORMAL,"Can't get alloc %d",ind->alloc_id);
        return RT_ERR_OUT_OF_RANGE;
    }
    else    /* found */
    {
        attr->tcont_id = obj->p_tcont[i]->tcont_id;
        if(!obj->p_tcont[i]->omci_en)    /* not added */
        {
            GPON_OS_Log(GPON_LOG_LEVEL_NORMAL,"Can't get logical alloc %d",ind->alloc_id);
            return RT_ERR_OUT_OF_RANGE;
        }
        else
        {
            GPON_OS_Log(GPON_LOG_LEVEL_NORMAL,"Get Logical TCont %d: alloc %d",i,obj->p_tcont[i]->alloc_id);
            return RT_ERR_OK;
        }
    }
}

int32 gpon_dev_tcont_physical_get(gpon_dev_obj_t* obj, rtk_gpon_tcont_ind_t* ind, rtk_gpon_tcont_attr_t* attr)
{
    uint32 i;
    uint32 max_tcont = obj->scheInfo.max_tcont;

    /* search alloc id from table */
#if 0 /* the DBA TCONT(p_tcont[0]) is not support, the entry 0 can be used by normal tcont */
    for(i=1;i<max_tcont;i++)
#else
    for(i=0;i<max_tcont;i++)
#endif
    {
        if(obj->p_tcont[i] && obj->p_tcont[i]->alloc_id==ind->alloc_id)
        {
            /* found it */
            break;
        }
    }

    attr->tcont_id = max_tcont;

    if(i==max_tcont)    /* not found */
    {
        GPON_OS_Log(GPON_LOG_LEVEL_NORMAL,"Can't get alloc %d",ind->alloc_id);
        return RT_ERR_OUT_OF_RANGE;
    }
    else    /* found */
    {
        attr->tcont_id = obj->p_tcont[i]->tcont_id;
        if(!obj->p_tcont[i]->ploam_en)    /* not added */
        {
            GPON_OS_Log(GPON_LOG_LEVEL_NORMAL,"Can't get physical alloc %d",ind->alloc_id);
            return RT_ERR_OUT_OF_RANGE;
        }
        else
        {
            GPON_OS_Log(GPON_LOG_LEVEL_NORMAL,"Get physical TCont %d: alloc %d",i,obj->p_tcont[i]->alloc_id);
            return RT_ERR_OK;
        }
    }
}

int32 gpon_dev_tcont_physical_add(gpon_dev_obj_t* obj, uint32 alloc,rtk_gpon_tcont_attr_t* attr)
{
    uint32 max_tcont = obj->scheInfo.max_tcont;
    uint32 i=max_tcont,ret, j;
    uint32 idle = max_tcont;
    uint8 isCreated = TRUE;
    uint8 forbid_tcont[max_tcont];
    rtk_ponmac_aggregate_tcont_t aggIndex;
    rtk_enable_t state;

    /* for aggregate tcont check, if aggregate tcont enable, the slave tconts should not be added */
    osal_memset(forbid_tcont, 0x0, sizeof(uint8)*max_tcont);
    for(aggIndex=0; aggIndex<PONMAC_AGG_TCONT_END; aggIndex++)
    {
        if(rtk_ponmac_aggTcontState_get(aggIndex, &state) != RT_ERR_OK)
            continue;
        if(state == ENABLED)
        {
            for(j=aggIndex*4+1; j<=aggIndex*4+3; j++)
            {
                forbid_tcont[j] = 1;
            }
        }
    }

    /* add for default tcont */
    if(alloc < GPON_DEV_DEFAULT_ONU_ID)
    {
       // GPON_UTIL_ERR_CHK(rtk_gpon_usTcont_get(GPON_OMCI_TCONT_ID, &read_alloc_id),ret);
       // if(read_alloc_id == alloc)
       //     return RT_ERR_OK;
        // OMCI directly assign tcontid
        idle = obj->scheInfo.omcc_tcont;
    }
    else /* normal case */
    {
        if(attr!=NULL && (attr->tcont_id < max_tcont) && !obj->p_tcont[attr->tcont_id])
        {
            idle = attr->tcont_id;
        }else
        {
            /* search alloc id from table */
            for(i=0;i<max_tcont;i++)
            {
                /* found it */
                if(obj->p_tcont[i] && obj->p_tcont[i]->alloc_id==alloc)
                {
                    isCreated = FALSE;
                    break;
                }
            }
        }
    }

    if(isCreated)    /* not found */
    {
        if(idle == max_tcont)
        {
            for(j = 0; j < max_tcont; j++)
            {
                /* Search an idle tcont */
                if(!obj->p_tcont[j] && (forbid_tcont[j] == 0))
                {
                    idle = j;
                    break;
                }
            }
        }

        if(idle==max_tcont)    /* no idle tcont */
        {
            return RT_ERR_GPON_EXCEED_MAX_TCONT;
        }
        else    /* create new tcont */
        {
            if(!obj->p_tcont[idle])
            {
                obj->p_tcont[idle] = (gpon_tcont_obj_t*)GPON_OS_Malloc(sizeof(gpon_tcont_obj_t));
                if(obj->p_tcont[idle] == NULL)
                    return RT_ERR_GPON_INVALID_HANDLE;
            }
            obj->p_tcont[idle]->tcont_id = idle;
            obj->p_tcont[idle]->alloc_id = alloc;
            obj->p_tcont[idle]->type = RTK_GPON_TCONT_TYPE_3;
            obj->p_tcont[idle]->ploam_en = TRUE;
            obj->p_tcont[idle]->omci_en = FALSE;

            /* write to chip */
            if((ret = rtk_gpon_usTcont_set(obj->p_tcont[idle]->tcont_id,obj->p_tcont[idle]->alloc_id)) != RT_ERR_OK)
            {
                osal_printf("rtk_gpon_usTcont_set ret=%x\n", ret);
                GPON_OS_Free(obj->p_tcont[idle]);
                obj->p_tcont[idle] = NULL;
                return ret;
            }

            GPON_OS_Log(GPON_LOG_LEVEL_NORMAL,"Create Physical TCont %d: alloc %d [baseaddr:%p]",idle,obj->p_tcont[idle]->alloc_id,obj->base_addr);
            return RT_ERR_OK;
        }
    }
    else    /* found */
    {
        if(forbid_tcont[i] == 1)
            return RT_ERR_GPON_FORBID_TCONT_ID;

        if(!obj->p_tcont[i]->ploam_en)
        {
            /* write to chip */
            GPON_UTIL_ERR_CHK(rtk_gpon_usTcont_set(obj->p_tcont[i]->tcont_id,obj->p_tcont[i]->alloc_id),ret);

            obj->p_tcont[i]->ploam_en = TRUE;

            GPON_OS_Log(GPON_LOG_LEVEL_NORMAL,"Enable Physical TCont %d: alloc %d [baseaddr:%p]",i,obj->p_tcont[i]->alloc_id,obj->base_addr);
        }
        return RT_ERR_OK;
    }
}

int32 gpon_dev_tcont_physical_del(gpon_dev_obj_t* obj, uint32 alloc)
{
    uint32 i,ret;
    uint32 max_tcont = obj->scheInfo.max_tcont;

    /* search alloc id from table */
#if 0 /* the DBA TCONT(p_tcont[0]) is not support, the entry 0 can be used by normal tcont */
    for(i=1;i<max_tcont;i++)
#else
    for(i=0;i<max_tcont;i++)
#endif
    {
        if(obj->p_tcont[i] && obj->p_tcont[i]->alloc_id==alloc)
        {
            /* found it */
            break;
        }
    }

    if(i==max_tcont)    /* not found */
    {
        return RT_ERR_INPUT;
    }
    else    /* found */
    {
        if(!obj->p_tcont[i]->ploam_en)    /* not added */
        {
            return RT_ERR_INPUT;
        }
        else
        {
            /* write to chip */
            GPON_UTIL_ERR_CHK(rtk_gpon_usTcont_del(obj->p_tcont[i]->tcont_id),ret);

            /* remove from physical */
            obj->p_tcont[i]->ploam_en = FALSE;

            /* if removed from physical, free it */
            if(!obj->p_tcont[i]->omci_en)
            {
                GPON_OS_Log(GPON_LOG_LEVEL_NORMAL,"Remove Physical TCont %d: alloc %d [baseaddr:%p]",i,obj->p_tcont[i]->alloc_id,obj->base_addr);
                GPON_OS_Free(obj->p_tcont[i]);
                obj->p_tcont[i] = NULL;
            }
            else
            {
                GPON_OS_Log(GPON_LOG_LEVEL_NORMAL,"Disable Physical TCont %d: alloc %d [baseaddr:%p]",i,obj->p_tcont[i]->alloc_id,obj->base_addr);
            }
            return RT_ERR_OK;
        }
    }
}

int32 gpon_dev_tcont_physical_reset(gpon_dev_obj_t* obj)
{
    uint32 i,ret;
    uint32 max_tcont = obj->scheInfo.max_tcont;

    /* search alloc id from table */
    for(i=0;i<max_tcont;i++)
    {
        if(obj->p_tcont[i])
        {
            if(obj->p_tcont[i]->ploam_en)    /* added */
            {
                /* write to chip */
                GPON_UTIL_ERR_CHK(rtk_gpon_usTcont_del(obj->p_tcont[i]->tcont_id),ret);

                /* remove from physical */
                obj->p_tcont[i]->ploam_en = FALSE;

                /* if removed from physical, free it */
                if(!obj->p_tcont[i]->omci_en)
                {
                    GPON_OS_Log(GPON_LOG_LEVEL_INFO,"Remove Physical TCont %d: alloc %d [baseaddr:%p]",i,obj->p_tcont[i]->alloc_id,obj->base_addr);
                    GPON_OS_Free(obj->p_tcont[i]);
                    obj->p_tcont[i] = NULL;
                }
                else
                {
                    GPON_OS_Log(GPON_LOG_LEVEL_INFO,"Disable Physical TCont %d: alloc %d [baseaddr:%p]",i,obj->p_tcont[i]->alloc_id,obj->base_addr);
                }
            }
        }
    }
    return RT_ERR_OK;
}

int32 gpon_dev_dsFlow_add(gpon_dev_obj_t* obj, uint32 flow_id, rtk_gpon_dsFlow_attr_t* attr)
{
    uint32 i;
    rtk_gem_cfg_t gemCfg;
    int32 ret;
    uint32 max_flow =  obj->scheInfo.max_flow;
    RT_PARAM_CHK((flow_id >= max_flow), RT_ERR_OUT_OF_RANGE);

    /* flow is used by other gem port */
    if(flow_id != obj->scheInfo.omcc_flow)
    {
        if(obj->p_dsflow[flow_id] && obj->p_dsflow[flow_id]->gem_port!=attr->gem_port_id)
        {
            return RT_ERR_INPUT;
        }
    }

    /* gem port is added to other flow */
    for(i=0;i<max_flow;i++)
    {
        if(obj->p_dsflow[i] && obj->p_dsflow[i]->flow_id!=flow_id && obj->p_dsflow[i]->gem_port==attr->gem_port_id)
        {
            /* found it */
            return RT_ERR_INPUT;
        }
    }

    //if(obj->p_dsflow[flow_id] && obj->p_dsflow[flow_id]->gem_port==attr->gem_port_id)
    if(obj->p_dsflow[flow_id])
    {
        GPON_OS_Log(GPON_LOG_LEVEL_NORMAL,"Update D/S Flow %d: GEM port %d type %d multicast %d AES %d [baseaddr:%p]",
            flow_id,attr->gem_port_id,attr->type,attr->multicast,attr->aes_en,obj->base_addr);
        GPON_OS_Free(obj->p_dsflow[flow_id]);
        obj->p_dsflow[flow_id] = NULL;
    }
    else
    {
        GPON_OS_Log(GPON_LOG_LEVEL_NORMAL,"Create D/S Flow %d: GEM port %d type %d multicast %d AES %d [baseaddr:%p]",
            flow_id,attr->gem_port_id,attr->type,attr->multicast,attr->aes_en,obj->base_addr);
    }

    obj->p_dsflow[flow_id] = (gpon_ds_flow_obj_t*)GPON_OS_Malloc(sizeof(gpon_ds_flow_obj_t));
    if(obj->p_dsflow[flow_id] == NULL)
        return RT_ERR_GPON_INVALID_HANDLE;
    obj->p_dsflow[flow_id]->flow_id = flow_id;
    obj->p_dsflow[flow_id]->gem_port = attr->gem_port_id;
    obj->p_dsflow[flow_id]->type = attr->type;
    obj->p_dsflow[flow_id]->multicast = attr->multicast;
    obj->p_dsflow[flow_id]->aes_en = attr->aes_en;


    osal_memset(&gemCfg,0,sizeof(rtk_gem_cfg_t));
    gemCfg.enAES = obj->p_dsflow[flow_id]->aes_en ? 1:0;

    /* write to chip */
    switch(obj->p_dsflow[flow_id]->type)
    {
        case RTK_GPON_FLOW_TYPE_OMCI:
        {
            gemCfg.isEthernet = DISABLED;
        gemCfg.isOMCI= ENABLED;

            break;
        }
        case RTK_GPON_FLOW_TYPE_ETH:
        {
                gemCfg.isEthernet = ENABLED;
        gemCfg.isOMCI= DISABLED;
            break;
        }
        default:
        {
            /* do nothing */
            break;
        }
    }
     gemCfg.gemPortId = obj->p_dsflow[flow_id]->gem_port;
     gemCfg.isMulticast = obj->p_dsflow[flow_id]->multicast ? 1:0;

     GPON_UTIL_ERR_CHK(rtk_gpon_dsGemPort_set(obj->p_dsflow[flow_id]->flow_id,gemCfg),ret);


    return RT_ERR_OK;
}

int32 gpon_dev_dsFlow_del(gpon_dev_obj_t* obj, uint32 flow_id)
{
    int32 ret;
    uint32 max_flow =  obj->scheInfo.max_flow;
    RT_PARAM_CHK((flow_id >= max_flow), RT_ERR_OUT_OF_RANGE);

    /* flow is not used */
    if(!obj->p_dsflow[flow_id])
    {
        return RT_ERR_INPUT;
    }

    GPON_OS_Log(GPON_LOG_LEVEL_NORMAL,"Remove D/S Flow %d: GEM port %d type %d [baseaddr:%p]",
        flow_id,obj->p_dsflow[flow_id]->gem_port,obj->p_dsflow[flow_id]->type,obj->base_addr);
    /* write to chip */
    GPON_UTIL_ERR_CHK( rtk_gpon_dsGemPort_del(obj->p_dsflow[flow_id]->flow_id),ret);

    GPON_OS_Free(obj->p_dsflow[flow_id]);
    obj->p_dsflow[flow_id] = NULL;

    return RT_ERR_OK;
}

int32 gpon_dev_dsFlow_get(gpon_dev_obj_t* obj, uint32 flow_id, rtk_gpon_dsFlow_attr_t* attr)
{
    int32 ret;
    rtk_gem_cfg_t gemCfg;

    RT_PARAM_CHK((flow_id >= obj->scheInfo.max_flow), RT_ERR_OUT_OF_RANGE);

    /* flow is not used */
    if(!obj->p_dsflow[flow_id])
    {
        return RT_ERR_INPUT;
    }

    attr->gem_port_id = obj->p_dsflow[flow_id]->gem_port;
    attr->type = obj->p_dsflow[flow_id]->type;
    attr->multicast = obj->p_dsflow[flow_id]->multicast;
    attr->aes_en = obj->p_dsflow[flow_id]->aes_en;

    GPON_UTIL_ERR_CHK(rtk_gpon_dsGemPort_get(flow_id, &gemCfg),ret);

    return RT_ERR_OK;
}

int32 gpon_dev_usFlow_add(gpon_dev_obj_t* obj, uint32 flow_id, rtk_gpon_usFlow_attr_t* attr)
{

    int32 ret;
    uint32 max_flow = obj->scheInfo.max_flow;
    uint32 adj_th = 0;

    RT_PARAM_CHK((flow_id >= max_flow), RT_ERR_OUT_OF_RANGE);

    /* check omci flow id */
    if(attr->type==RTK_GPON_FLOW_TYPE_OMCI)
        RT_PARAM_CHK((flow_id != obj->scheInfo.omcc_flow), RT_ERR_INPUT);
    else
        RT_PARAM_CHK((flow_id == obj->scheInfo.omcc_flow), RT_ERR_INPUT);

    /* flow is used by other gem port */
    if(flow_id != obj->scheInfo.omcc_flow)
    {
        if(obj->p_usflow[flow_id] && obj->p_usflow[flow_id]->gem_port!=attr->gem_port_id)
        {
            return RT_ERR_INPUT;
        }
    }

    /* The checking is removed because the Apollo spec allow many_flow-to-one_gem_port case. */

    /* check the OMCI should always be create, but no update */
    if(attr->type==RTK_GPON_FLOW_TYPE_OMCI)
    {
        if(obj->us_omci_flow!=max_flow)
        {
            return RT_ERR_INPUT;
        }
    }

    /* check the TDM */
    if(attr->type==RTK_GPON_FLOW_TYPE_TDM)
    {
        if(!obj->tdm_en)
        {
            return RT_ERR_GPON_INVALID_HANDLE;
        }
    }

    //if(obj->p_usflow[flow_id] && obj->p_usflow[flow_id]->gem_port==attr->gem_port_id)
    if(obj->p_usflow[flow_id])
    {
        GPON_OS_Log(GPON_LOG_LEVEL_NORMAL,"Update U/S Flow %d: GEM port %d type %d tcont %d channel %d [baseaddr:%p]",
            flow_id,attr->gem_port_id,attr->type,attr->tcont_id,attr->channel,obj->base_addr);
        GPON_OS_Free(obj->p_usflow[flow_id]);
        obj->p_usflow[flow_id] = NULL;
    }
    else
    {
        GPON_OS_Log(GPON_LOG_LEVEL_NORMAL,"Create U/S Flow %d: GEM port %d type %d tcont %d channel %d [baseaddr:%p]",
            flow_id,attr->gem_port_id,attr->type,attr->tcont_id,attr->channel,obj->base_addr);
        adj_th = 1;
    }

    obj->p_usflow[flow_id] = (gpon_us_flow_obj_t*)GPON_OS_Malloc(sizeof(gpon_us_flow_obj_t));
    if(obj->p_usflow[flow_id] == NULL)
        return RT_ERR_GPON_INVALID_HANDLE;
    obj->p_usflow[flow_id]->flow_id = flow_id;
    obj->p_usflow[flow_id]->gem_port = attr->gem_port_id;
    obj->p_usflow[flow_id]->type = attr->type;
    obj->p_usflow[flow_id]->tcont = attr->tcont_id;
    obj->p_usflow[flow_id]->channel = GPON_DEV_MAX_TDM_CHANNEL_NUM;

    /* write to chip */
    GPON_UTIL_ERR_CHK(rtk_gpon_gemUsPortCfg_set(obj->p_usflow[flow_id]->flow_id,obj->p_usflow[flow_id]->gem_port),ret);

    /* set pon SID valid to 1 */
    if((ret = rtk_ponmac_sidValid_set(obj->p_usflow[flow_id]->flow_id, 1))!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_GPON), "");
    }

    switch(obj->p_usflow[flow_id]->type)
    {
        case RTK_GPON_FLOW_TYPE_OMCI:
        {
            obj->us_omci_flow = flow_id;
            break;
        }
        case RTK_GPON_FLOW_TYPE_ETH:
        {
            if (adj_th) 
            {
                obj->us_flow_eth_num++;
                if((ret = rtk_gpon_flowctrl_adjust_byFlowNum(obj->us_flow_eth_num))!=RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_GPON), "%s,%d,ret=0x%x",__FUNCTION__,__LINE__,ret);
                }
            }
            break;
        }
        case RTK_GPON_FLOW_TYPE_TDM:
        {
            obj->p_usflow[flow_id]->channel = attr->channel;
            break;
        }
        default:
        {
            /* do nothing */
            break;
        }
    }

    return RT_ERR_OK;
}

int32 gpon_dev_usFlow_del(gpon_dev_obj_t* obj, uint32 flow_id)
{
    int32 ret;
    uint32 max_flow =  obj->scheInfo.max_flow;
    RT_PARAM_CHK((flow_id >= max_flow), RT_ERR_OUT_OF_RANGE);

    /* flow is not used */
    if(!obj->p_usflow[flow_id])
    {
        return RT_ERR_INPUT;
    }

    /* check the TDM */
    if(obj->p_usflow[flow_id]->type==RTK_GPON_FLOW_TYPE_TDM)
    {
        if(!obj->tdm_en)
        {
            return RT_ERR_GPON_INVALID_HANDLE;
        }
    }

    GPON_OS_Log(GPON_LOG_LEVEL_NORMAL,"Remove U/S Flow %d: GEM port %d type %d [baseaddr:%p]",
        flow_id,obj->p_usflow[flow_id]->gem_port,obj->p_usflow[flow_id]->type,obj->base_addr);
    /* write to chip */
    GPON_UTIL_ERR_CHK(rtk_gpon_gemUsPortCfg_set(obj->p_usflow[flow_id]->flow_id,0),ret);

    /* set pon SID valid to 0 */
    if((ret = rtk_ponmac_sidValid_set(obj->p_usflow[flow_id]->flow_id, 0))!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_GPON), "%s,%d,ret=0x%x",__FUNCTION__,__LINE__,ret);
    }

    switch(obj->p_usflow[flow_id]->type)
    {
        case RTK_GPON_FLOW_TYPE_OMCI:
        {
            obj->us_omci_flow = max_flow;
            break;
        }
        case RTK_GPON_FLOW_TYPE_ETH:
        {
            obj->us_flow_eth_num--;
            if((ret = rtk_gpon_flowctrl_adjust_byFlowNum(obj->us_flow_eth_num))!=RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_GPON), "%s,%d,ret=0x%x",__FUNCTION__,__LINE__,ret);
            }
            break;
        }
        case RTK_GPON_FLOW_TYPE_TDM:
        {
            break;
        }
        default:
        {
            /* do nothing */
            break;
        }
    }

    GPON_OS_Free(obj->p_usflow[flow_id]);
    obj->p_usflow[flow_id] = NULL;

    return RT_ERR_OK;
}

int32 gpon_dev_usFlow_get(gpon_dev_obj_t* obj, uint32 flow_id, rtk_gpon_usFlow_attr_t* attr)
{

    uint32 gemPortId,ret;
    uint32 max_flow =  obj->scheInfo.max_flow;
    RT_PARAM_CHK((flow_id >= max_flow), RT_ERR_OUT_OF_RANGE);

    /* flow is not used */
    if(!obj->p_usflow[flow_id])
    {
        return RT_ERR_INPUT;
    }

    attr->gem_port_id = obj->p_usflow[flow_id]->gem_port;
    attr->type = obj->p_usflow[flow_id]->type;
    attr->tcont_id = obj->p_usflow[flow_id]->tcont;
    attr->channel = obj->p_usflow[flow_id]->channel;

    GPON_UTIL_ERR_CHK(rtk_gpon_gemUsPortCfg_get(flow_id,&gemPortId),ret);
    return RT_ERR_OK;
}

int32 gpon_dev_bcastPass_set(gpon_dev_obj_t* obj, int32 status)
{
    int32 ret;
    RT_PARAM_CHK(((status != 0)&&(status != 1)), RT_ERR_OUT_OF_RANGE);

    if(status!=obj->mactbl.bc_pass)
    {
        obj->mactbl.bc_pass = status;
        GPON_UTIL_ERR_CHK(rtk_gpon_dsGemPortBcPassState_set(obj->mactbl.bc_pass),ret);
    }
    return RT_ERR_OK;
}

int32 gpon_dev_bcastPass_get(gpon_dev_obj_t* obj, int32 *pStatus)
{
    *pStatus = obj->mactbl.bc_pass;
    return RT_ERR_OK;
}

int32 gpon_dev_nonMcastPass_set(gpon_dev_obj_t* obj, int32 status)
{
    int32 ret;
    RT_PARAM_CHK(((status != 0)&&(status != 1)), RT_ERR_OUT_OF_RANGE);

    if(status!=obj->mactbl.non_mc_pass)
    {
        obj->mactbl.non_mc_pass = status;
        GPON_UTIL_ERR_CHK(rtk_gpon_dsGemPortNonMcPassState_set(obj->mactbl.non_mc_pass),ret);
    }
    return RT_ERR_OK;
}

int32 gpon_dev_nonMcastPass_get(gpon_dev_obj_t* obj, int32 *pStatus)
{
    *pStatus = obj->mactbl.non_mc_pass;
    return RT_ERR_OK;
}

int32 gpon_dev_mcastCheck_set(gpon_dev_obj_t* obj, uint32 ipv4_addr, uint32 ipv6_addr)
{
    int32 ret;
    if(ipv4_addr!=obj->mactbl.ipv4_mc_check_ptn)
    {
        obj->mactbl.ipv4_mc_check_ptn = ipv4_addr;
        GPON_UTIL_ERR_CHK(rtk_gpon_ipv4McAddrPtn_set(obj->mactbl.ipv4_mc_check_ptn),ret);
    }
    if(ipv6_addr!=obj->mactbl.ipv6_mc_check_ptn)
    {
        obj->mactbl.ipv6_mc_check_ptn = ipv6_addr;
        GPON_UTIL_ERR_CHK(rtk_gpon_ipv6McAddrPtn_set(obj->mactbl.ipv6_mc_check_ptn),ret);
    }
    return RT_ERR_OK;
}

int32 gpon_dev_mcastCheck_get(gpon_dev_obj_t* obj, uint32* ipv4_addr, uint32* ipv6_addr)
{
    *ipv4_addr = obj->mactbl.ipv4_mc_check_ptn;
    *ipv6_addr = obj->mactbl.ipv6_mc_check_ptn;
    return RT_ERR_OK;
}

int32 gpon_dev_macFilterMode_set(gpon_dev_obj_t* obj, rtk_gpon_macTable_exclude_mode_t mode)
{
    int32 ret;
    RT_PARAM_CHK((mode > RTK_GPON_MACTBL_MODE_EXCLUDE), RT_ERR_OUT_OF_RANGE);

    if(mode!=obj->mactbl.op_mode)
    {
        obj->mactbl.op_mode = mode;
        GPON_UTIL_ERR_CHK(rtk_gpon_dsGemPortMacFilterMode_set(obj->mactbl.op_mode),ret);
    }

    return RT_ERR_OK;
}

int32 gpon_dev_macFilterMode_get(gpon_dev_obj_t* obj, rtk_gpon_macTable_exclude_mode_t *pMode)
{
    *pMode = obj->mactbl.op_mode;
    return RT_ERR_OK;
}

int32 gpon_dev_mcForceMode_set(gpon_dev_obj_t* obj, rtk_gpon_mc_force_mode_t ipv4, rtk_gpon_mc_force_mode_t ipv6)
{
    int32 ret;
    RT_PARAM_CHK((ipv4 > RTK_GPON_MCFORCE_MODE_DROP), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((ipv6 > RTK_GPON_MCFORCE_MODE_DROP), RT_ERR_OUT_OF_RANGE);

    if((ipv4!=obj->mactbl.ipv4_force_mode) || (ipv6!=obj->mactbl.ipv6_force_mode))
    {
        obj->mactbl.ipv4_force_mode = ipv4;
        obj->mactbl.ipv6_force_mode = ipv6;
        GPON_UTIL_ERR_CHK(rtk_gpon_dsGemPortMacForceMode_set(GPON_IP_VER_V4, obj->mactbl.ipv4_force_mode),ret);
        GPON_UTIL_ERR_CHK(rtk_gpon_dsGemPortMacForceMode_set(GPON_IP_VER_V6, obj->mactbl.ipv6_force_mode),ret);
    }

    return RT_ERR_OK;
}

int32 gpon_dev_mcForceMode_get(gpon_dev_obj_t* obj, rtk_gpon_mc_force_mode_t *ipv4, rtk_gpon_mc_force_mode_t *ipv6)
{
    *ipv4 = obj->mactbl.ipv4_force_mode;
    *ipv6 = obj->mactbl.ipv6_force_mode;
    return RT_ERR_OK;
}

int32 gpon_dev_macTableEntry_add(gpon_dev_obj_t* obj, rtk_gpon_macTable_entry_t* entry)
{
    uint32 i,idle = GPON_DEV_MAX_MACTBL_NUM,ret;
    rtk_mac_t mac;
    for(i=0;i<GPON_DEV_MAX_MACTBL_NUM;i++)
    {
        if(obj->p_mac_tbl[i] && !osal_memcmp(obj->p_mac_tbl[i]->addr,entry->mac_addr,RTK_GPON_MACTABLE_ADDR_LEN*sizeof(uint8)))
        {
            return RT_ERR_INPUT;
        }
        if(idle==GPON_DEV_MAX_MACTBL_NUM && !obj->p_mac_tbl[i])
        {
            idle = i;
        }
    }

    if(idle==GPON_DEV_MAX_MACTBL_NUM)   /* MAC table full */
    {
        return RT_ERR_GPON_MAC_FILTER_TABLE_FULL;
    }
    else
    {
        obj->p_mac_tbl[idle] = (gpon_macTable_obj_t*)GPON_OS_Malloc(sizeof(gpon_macTable_obj_t));
        if(obj->p_mac_tbl[idle] == NULL)
            return RT_ERR_GPON_MAC_FILTER_TABLE_ALLOC_FAIL;

        osal_memcpy(obj->p_mac_tbl[idle]->addr,entry->mac_addr,RTK_GPON_MACTABLE_ADDR_LEN*sizeof(uint8));
        osal_memcpy(&mac.octet[0],&entry->mac_addr[0],ETHER_ADDR_LEN);
        GPON_UTIL_ERR_CHK(rtk_gpon_dsGemPortMacEntry_set(idle,mac),ret);
        return RT_ERR_OK;
    }
}

int32 gpon_dev_macTableEntry_del(gpon_dev_obj_t* obj, rtk_gpon_macTable_entry_t* entry)
{
    uint32 i,ret;
    for(i=0;i<GPON_DEV_MAX_MACTBL_NUM;i++)
    {
        if(obj->p_mac_tbl[i] && !osal_memcmp(obj->p_mac_tbl[i]->addr,entry->mac_addr,RTK_GPON_MACTABLE_ADDR_LEN*sizeof(uint8)))
        {
            break;
        }
    }

    if(i==GPON_DEV_MAX_MACTBL_NUM)   /* the entry is not exist */
    {
        return RT_ERR_GPON_TABLE_ENTRY_NOT_FOUND;
    }
    else
    {
        GPON_UTIL_ERR_CHK(rtk_gpon_dsGemPortMacEntry_del(i),ret);
        GPON_OS_Free(obj->p_mac_tbl[i]);
        obj->p_mac_tbl[i] = NULL;
        return RT_ERR_OK;
    }
}

int32 gpon_dev_macTableEntry_get(gpon_dev_obj_t* obj, uint32 index, rtk_gpon_macTable_entry_t* entry)
{
    RT_PARAM_CHK((index >= GPON_DEV_MAX_MACTBL_NUM), RT_ERR_OUT_OF_RANGE);

    if(!obj->p_mac_tbl[index])
    {
        return RT_ERR_GPON_TABLE_ENTRY_NOT_FOUND;
    }

    osal_memcpy(entry->mac_addr,obj->p_mac_tbl[index]->addr,RTK_GPON_MACTABLE_ADDR_LEN*sizeof(uint8));
    return RT_ERR_OK;
}

int32 gpon_dev_rdi_set(gpon_dev_obj_t* obj, int32 enable)
{
    int32 ret;
    RT_PARAM_CHK(((enable != 0)&&(enable != 1)), RT_ERR_OUT_OF_RANGE);

    if(obj->rdi!=enable)
    {
        obj->rdi = enable;
        GPON_UTIL_ERR_CHK(rtk_gpon_usRdi_set(obj->rdi),ret);
    }
    return RT_ERR_OK;
}

int32 gpon_dev_rdi_get(gpon_dev_obj_t* obj, int32 *pEnable)
{
    *pEnable = obj->rdi;
    return RT_ERR_OK;
}

int32 gpon_dev_powerLevel_set(gpon_dev_obj_t* obj, uint8 level)
{
    int32 ret;
    RT_PARAM_CHK((level > GPON_DEV_MAX_TX_PWR_LEVEL), RT_ERR_OUT_OF_RANGE);
    if(obj->power_level!=level)
    {
        obj->power_level = level;
        GPON_UTIL_ERR_CHK(gpon_ploam_snPloam_set(obj),ret);
    }
    return RT_ERR_OK;
}

int32 gpon_dev_powerLevel_get(gpon_dev_obj_t* obj, uint8 *pLevel)
{
    *pLevel = obj->power_level;
    return RT_ERR_OK;
}

int32 gpon_dev_auto_tcont_set(gpon_dev_obj_t *obj, int32 state)
{
    RT_PARAM_CHK((state > 1), RT_ERR_OUT_OF_RANGE);

    obj->auto_tcont_state = state;

    return RT_ERR_OK;
}

int32 gpon_dev_auto_tcont_get(gpon_dev_obj_t *obj, int32 *pState)
{
    *pState = obj->auto_tcont_state;

    return RT_ERR_OK;
}

int32 gpon_dev_auto_boh_set(gpon_dev_obj_t *obj, int32 state)
{
    RT_PARAM_CHK((state > 1), RT_ERR_OUT_OF_RANGE);

    obj->auto_boh_state = state;

    return RT_ERR_OK;
}

int32 gpon_dev_auto_boh_get(gpon_dev_obj_t *obj, int32 *pState)
{
    *pState = obj->auto_boh_state;

    return RT_ERR_OK;
}

int32 gpon_dev_eqd_offset_set(gpon_dev_obj_t *obj, int32 offset)
{
    obj->eqd_offset = offset;

    return RT_ERR_OK;
}

int32 gpon_dev_eqd_offset_get(gpon_dev_obj_t *obj, int32 *pOffset)
{
    *pOffset = obj->eqd_offset;

    return RT_ERR_OK;
}


int32 gpon_dev_berInterval_set(gpon_dev_obj_t* obj, uint32 interval)
{
    obj->ber_interval = interval;
    return RT_ERR_OK;
}

int32 gpon_dev_berInterval_get(gpon_dev_obj_t* obj, uint32 *interval)
{
    *interval = obj->ber_interval;
    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_gpon_rogueOntDisTx_set
 * Description:
 *      Enable or Disable Tx of rogue ONT function
 * Input:
 *     txState - enable or disable laser tx by gpio
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 gpon_dev_rogueOntDisTx_set(gpon_dev_obj_t* obj, rtk_enable_t txState)
{
#if !defined(FPGA_DEFINED)
    int32 ret;

    /* set gpio output value */
    if(ENABLED == txState)
    {
        ret = rtk_gpio_databit_set(obj->gpio_disTx, 0);

#ifdef CONFIG_OE_RESET_AFTER_TX_DIS_OFF
        if(ret == RT_ERR_OK)
        {
            rtk_transceiver_data_t dataCfg;
            /* check OE vender PN is NOG22-D6C-SD, need to do reset */
            if((ret=rtk_ponmac_transceiver_get(RTK_TRANSCEIVER_PARA_TYPE_VENDOR_PART_NUM, &dataCfg)) == RT_ERR_OK)
            {
                if(strncmp(dataCfg.buf, "NOG22-D6C-SD", 12) == 0)
                {
                    osal_printf("!!!Detect NOG22-D6C-SD, reset laser driver!!!\n\r");
                    if((ret=rtk_i2c_write(0, TRANSCEIVER_A2, 0x7f, 0x0))!= RT_ERR_OK)
                        return ret;
                    if((ret=rtk_i2c_write(0, TRANSCEIVER_A2, 0x7b, 0x53))!= RT_ERR_OK)
                        return ret;
                    if((ret=rtk_i2c_write(0, TRANSCEIVER_A2, 0x7c, 0x60))!= RT_ERR_OK)
                        return ret;
                    if((ret=rtk_i2c_write(0, TRANSCEIVER_A2, 0x7d, 0x67))!= RT_ERR_OK)
                        return ret;
                    if((ret=rtk_i2c_write(0, TRANSCEIVER_A2, 0x7e, 0x73))!= RT_ERR_OK)
                        return ret;
                    if((ret=rtk_i2c_write(0, TRANSCEIVER_A2, 0x7f, 0x4))!= RT_ERR_OK)
                        return ret;
                    if((ret=rtk_i2c_write(0, TRANSCEIVER_A2, 0x97, 0x2))!= RT_ERR_OK)
                        return ret;
                }
            }
        }
#endif
    }
    else
    {
        ret = rtk_gpio_databit_set(obj->gpio_disTx, 1);
    }

    return ret;
#else
    return RT_ERR_OK;
#endif
}   /* end of rtk_gpon_rogueOntDisTx_set */

int32 gpon_dev_signal_para_set(gpon_dev_obj_t* obj, rtk_gpon_sig_para_t *pPara)
{
    if(RTK_GPON_ALARM_SF == pPara->type)
        osal_memcpy(&obj->sf_threshold, &(pPara->threshold), sizeof(uint32));
    else
        osal_memcpy(&obj->sd_threshold, &(pPara->threshold), sizeof(uint32));

    return RT_ERR_OK;
}

int32 gpon_dev_RDIMODE_set(gpon_dev_obj_t* obj, uint32 rdiMode)
{
    obj->rdiMode = rdiMode;

    return RT_ERR_OK;
}

/* Function Name:
 *      gpon_dev_portMacForceMode_set
 * Description:
 *      Set MAC focre mode for PON port
 * Input:
 *       None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 gpon_dev_portMacForceMode_set(gpon_dev_obj_t* obj, rtk_port_linkStatus_t linkStatus)
{
    uint32 sid;
    rtk_gpon_usFlow_attr_t attr;
    int32 ret;

    if(PORT_LINKUP == linkStatus)
    {
        for(sid=0; sid<HAL_CLASSIFY_SID_NUM()-1; sid++)
        {
            if((gpon_dev_usFlow_get(obj, sid, &attr) == RT_ERR_OK) && ( attr.type == RTK_GPON_FLOW_TYPE_ETH))
            {
                if((ret = rtk_ponmac_sidValid_set(sid,1))!=RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_GPON), "%s,%d,ret=0x%x",__FUNCTION__,__LINE__,ret);
                }
            }
        }
    }
    else
    {
        for(sid=0; sid<HAL_CLASSIFY_SID_NUM()-1; sid++)
        {
            if((ret = rtk_ponmac_sidValid_set(sid,0))!=RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_GPON), "%s,%d,ret=0x%x",__FUNCTION__,__LINE__,ret);
            }
        }
    }

    if((ret = rtk_gpon_portMacForceMode_set(linkStatus))!=RT_ERR_OK)
    {
        RT_ERR(ret,(MOD_GPON | MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;
}

/* Function Name:
 *      gpon_dev_pps_fire
 * Description:
 *      PPS fire, it should be fire per second.
 * Input:
 *       None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 gpon_dev_pps_fire(gpon_dev_obj_t* obj)
{
    obj->pps_cnt++;
//osal_printf("gpon_dev_pps_fire cnt = %d\n\r", obj->pps_cnt);
    return RT_ERR_OK;
}


