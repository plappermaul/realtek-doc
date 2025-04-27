/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Definition those Diagnostic command and APIs in the SDK diagnostic shell.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) Locol/Remote Loopback
 *           2) RTCT
 *
 */

/*
 * Include Files
 */
#include <common/debug/rt_log.h>
#include <common/rt_error.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <rtk/sds.h>
#include <diag_util.h>
#include <diag_om.h>
#include <diag_str.h>
#include <parser/cparser_priv.h>
#include <hal/mac/serdes.h>

#ifdef CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE
  #include <rtrpc/rtrpc_sds.h>
  #include <rtrpc/rtrpc_serdes.h>
#endif
#ifdef CMD_SERDES_GET_ID_SDSID_PAGE_PAGE_REG_REG
/*
 * serdes get id <UINT:sdsId> page <UINT:page> reg <UINT:reg>
 */
cparser_result_t
cparser_cmd_serdes_get_id_sdsId_page_page_reg_reg(cparser_context_t *context,
    uint32_t *sdsId_ptr,
    uint32_t *page_ptr,
    uint32_t *reg_ptr)
{
    uint32  unit;
    int32   ret;
    uint32  data;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(hal_serdes_reg_get(unit, *sdsId_ptr, *page_ptr, *reg_ptr, &data), ret);

    diag_util_printf("unit %u serdes %u page %u(0x%X) reg %u(0x%X)\n", unit, *sdsId_ptr, *page_ptr, *page_ptr, *reg_ptr, *reg_ptr);
    diag_util_printf("Data: 0x%04X\n", data);

    return CPARSER_OK;
}
#endif /* CMD_SERDES_GET_ID_SDSID_PAGE_PAGE_REG_REG */



#ifdef CMD_SERDES_SET_ID_SDSID_PAGE_PAGE_REG_REG_DATA_DATA
/*
 * serdes set id <UINT:sdsId> page <UINT:page> reg <UINT:reg> data <UINT:data>
 */
cparser_result_t
cparser_cmd_serdes_set_id_sdsId_page_page_reg_reg_data_data(cparser_context_t *context,
    uint32_t *sdsId_ptr,
    uint32_t *page_ptr,
    uint32_t *reg_ptr,
    uint32_t *data_ptr)
{
    uint32  unit;
    int32   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(hal_serdes_reg_set(unit, *sdsId_ptr, *page_ptr, *reg_ptr, *data_ptr), ret);
    diag_util_printf("unit %u serdes %u page %u(0x%X) reg %u(0x%X) set data 0x%04X\n",
        unit, *sdsId_ptr, *page_ptr, *page_ptr, *reg_ptr, *reg_ptr, *data_ptr);

    return CPARSER_OK;
}
#endif /* CMD_SERDES_SET_ID_SDSID_PAGE_PAGE_REG_REG_DATA_DATA */

#ifdef CMD_SERDES_GET_SDSID_LINK_STATUS
/*
 * serdes get <UINT:sdsId> link-status
 */
cparser_result_t
cparser_cmd_serdes_get_sdsId_link_status(
    cparser_context_t *context,
    uint32_t *sdsId_ptr)
{
    rtk_sds_linkSts_t   info;
    uint32              unit;
    int32               ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_sds_linkSts_get(unit, *sdsId_ptr, &info), ret);
    diag_util_printf("sds%d: sts1=0x%X sts0=0x%X, latch_sts1=0x%X latch_sts0=0x%X\n",
            *sdsId_ptr, info.sts1, info.sts, info.latch_sts1, info.latch_sts);

    return CPARSER_OK;
}   /* end of cparser_cmd_serdes_get_sdsId_link_status */
#endif

#ifdef CMD_SERDES_RESET_SDSID
/*
 * serdes reset <UINT:sdsId>
 */
cparser_result_t
cparser_cmd_serdes_reset_sdsId(
    cparser_context_t *context,
    uint32_t *sdsId_ptr)
{
    uint32  unit, ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(rtk_sds_rx_rst(unit, *sdsId_ptr), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_serdes_reset_sdsId */
#endif

#ifdef CMD_SERDES_GET_SDSID_RX_SYM_ERR
/*
 * serdes get <UINT:sdsId> rx-sym-err
 */
cparser_result_t
cparser_cmd_serdes_get_sdsId_rx_sym_err(
    cparser_context_t *context,
    uint32_t *sdsId_ptr)
{
    rtk_sds_symErr_t    info;
    uint32              unit, i;
    int32               ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_sds_symErr_get(unit, *sdsId_ptr, &info), ret);

    diag_util_printf("SDS %d:\n", *sdsId_ptr);
    for (i = 0; i < RTK_SDS_SYMERR_ALL_MAX; ++i)
    {
        diag_util_printf(" all%d: 0x%04X\n", i, info.all[i]);
    }

    for (i = 0; i < RTK_SDS_SYMERR_CHANNEL_MAX; ++i)
    {
        diag_util_printf(" CH%d: 0x%04X\n", i, info.ch[i]);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_serdes_get_sdsId_rx_sym_err */
#endif

#ifdef CMD_SERDES_CLEAR_SDSID_RX_SYM_ERR
/*
 * serdes clear <UINT:sdsId> rx-sym-err
 */
cparser_result_t
cparser_cmd_serdes_clear_sdsId_rx_sym_err(
    cparser_context_t *context,
    uint32_t *sdsId_ptr)
{
    uint32  unit;
    int32   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_sds_symErr_clear(unit, *sdsId_ptr), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_serdes_clear_sdsId_rx_sym_err */
#endif

#ifdef CMD_SERDES_GET_SDSID_TESTMODE_CNT
/*
 * serdes get <UINT:sdsId> testmode cnt
 */
cparser_result_t
cparser_cmd_serdes_get_sdsId_testmode_cnt(
    cparser_context_t *context,
    uint32_t *sdsId_ptr)
{
    uint32  unit, cnt;
    int32   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_sds_testModeCnt_get(unit, *sdsId_ptr, &cnt), ret);

    diag_util_printf("SDS %u rx_err_cnt %u\n", *sdsId_ptr, cnt);

    return CPARSER_OK;
}   /* end of cparser_cmd_serdes_get_sdsId_testmode_cnt */
#endif

#ifdef CMD_SERDES_SET_SDSID_10G_SQUARE_STATE_DISABLE_ENABLE
/*
 * serdes set <UINT:sdsId> 10g-square state ( disable | enable )
 */
cparser_result_t
cparser_cmd_serdes_set_sdsId_10g_square_state_disable_enable(
    cparser_context_t *context,
    uint32_t *sdsId_ptr)
{
    uint32  unit;
    int32   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('d' == TOKEN_CHAR(5, 0))
        DIAG_UTIL_ERR_CHK(rtk_sds_testMode_set(unit, *sdsId_ptr, RTK_SDS_TESTMODE_DISABLE), ret);
    else
        DIAG_UTIL_ERR_CHK(rtk_sds_testMode_set(unit, *sdsId_ptr, RTK_SDS_TESTMODE_SQUARE8), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_serdes_set_sdsId_10g_square_state_disable_enable */
#endif

#ifdef CMD_SERDES_SET_SDSID_10G_PRBS9_STATE_DISABLE_ENABLE
/*
 * serdes set <UINT:sdsId> 10g-prbs9 state ( disable | enable )
 */
cparser_result_t
cparser_cmd_serdes_set_sdsId_10g_prbs9_state_disable_enable(
    cparser_context_t *context,
    uint32_t *sdsId_ptr)
{
    uint32  unit;
    int32   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ('d' == TOKEN_CHAR(5, 0))
        DIAG_UTIL_ERR_CHK(rtk_sds_testMode_set(unit, *sdsId_ptr, RTK_SDS_TESTMODE_DISABLE), ret);
    else
        DIAG_UTIL_ERR_CHK(rtk_sds_testMode_set(unit, *sdsId_ptr, RTK_SDS_TESTMODE_PRBS9), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_serdes_set_sdsId_10g_prbs9_state_disable_enable */
#endif

#ifdef CMD_SERDES_SET_SDSID_10G_PRBS31_STATE_DISABLE_ENABLE
/*
 * serdes set <UINT:sdsId> 10g-prbs31 state ( disable | enable )
 */
cparser_result_t
cparser_cmd_serdes_set_sdsId_10g_prbs31_state_disable_enable(
    cparser_context_t *context,
    uint32_t *sdsId_ptr)
{
    uint32  unit;
    int32   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ('d' == TOKEN_CHAR(5, 0))
        DIAG_UTIL_ERR_CHK(rtk_sds_testMode_set(unit, *sdsId_ptr, RTK_SDS_TESTMODE_DISABLE), ret);
    else
        DIAG_UTIL_ERR_CHK(rtk_sds_testMode_set(unit, *sdsId_ptr, RTK_SDS_TESTMODE_PRBS31), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_serdes_set_sdsId_10g_prbs31_state_disable_enable */
#endif

#ifdef CMD_SERDES_SET_SDSID_LEQ_ADAPT
/*
 * serdes set <UINT:sdsId> leq adapt
 */
cparser_result_t
cparser_cmd_serdes_set_sdsId_leq_adapt(
    cparser_context_t *context,
    uint32_t *sdsId_ptr)
{
    uint32  unit;
    int32   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_sds_leq_adapt(unit, *sdsId_ptr), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_serdes_set_sdsId_leq_adapt */
#endif

#ifdef CMD_SERDES_GET_SDSID_LEQ
/*
 * serdes get <UINT:sdsId> leq
 */
cparser_result_t
cparser_cmd_serdes_get_sdsId_leq(
    cparser_context_t *context,
    uint32_t *sdsId_ptr)
{
    rtk_sds_leq_t   leq;
    uint32  unit;
    int32   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_sds_leq_get(unit, *sdsId_ptr, &leq), ret);

    diag_util_printf("SDS %u status = %s\n", *sdsId_ptr, (leq.manual? DIAG_STR_ENABLE: DIAG_STR_DISABLE));
    diag_util_printf("SDS %u LEQ = %u\n", *sdsId_ptr, leq.val);

    return CPARSER_OK;
}   /* end of cparser_cmd_serdes_get_sdsId_leq */
#endif

#ifdef CMD_SERDES_SET_SDSID_LEQ_MANUAL_ENABLE_DISABLE_VAL
/*
 * serdes set <UINT:sdsId> leq manual ( enable | disable ) { <UINT:val> }
 */
cparser_result_t
cparser_cmd_serdes_set_sdsId_leq_manual_enable_disable_val(
    cparser_context_t *context,
    uint32_t *sdsId_ptr,
    uint32_t *val_ptr)
{
    rtk_sds_leq_t   leq;
    uint32  unit;
    int32   ret;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_sds_leq_get(unit, *sdsId_ptr, &leq), ret);

    if ('e' == TOKEN_CHAR(5, 0))
        leq.manual = ENABLED;
    else
        leq.manual = DISABLED;

    if (val_ptr)
        leq.val = *val_ptr;

    DIAG_UTIL_ERR_CHK(rtk_sds_leq_set(unit, *sdsId_ptr, &leq), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_serdes_set_sdsId_leq_manual_enable_disable_val */
#endif

#ifdef CMD_SERDES_SET_SDSID_XSG_NWAY_STATE_DISABLE_ENABLE
/*
 * serdes set <UINT:sdsId> xsg-nway state ( disable | enable )
 */
cparser_result_t
cparser_cmd_serdes_set_sdsId_xsg_nway_state_disable_enable(
    cparser_context_t *context,
    uint32_t *sdsId_ptr)
{
    rtk_enable_t    en;
    uint32          unit;
    int32           ret;

    DIAG_UTIL_FUNC_INIT(unit);

    if ('d' == TOKEN_CHAR(5, 0))
        en = DISABLED;
    else
        en = ENABLED;

    DIAG_UTIL_ERR_CHK(rtk_sds_xsgNwayEn_set(unit, *sdsId_ptr, en), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_serdes_set_sdsId_xsg_nway_state_disable_enable */
#endif

#ifdef CMD_SERDES_GET_SDSID_CMU_BAND
/*
 * serdes get <UINT:sdsId> cmu-band
 */
cparser_result_t
cparser_cmd_serdes_get_sdsId_cmu_band(
    cparser_context_t *context,
    uint32_t *sdsId_ptr)
{
    uint32  unit, band;
    int32   ret;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_sds_cmuBand_get(unit, *sdsId_ptr, &band), ret);

    diag_util_mprintf("LC CMU Band = %d\n", band);

    return CPARSER_OK;
}   /* end of cparser_cmd_serdes_get_sdsId_cmu_band */
#endif

#ifdef CMD_SERDES_SET_SDSID_CMU_BAND_STATE_DISABLE_ENABLE_DATA_DATA
/*
 * serdes set <UINT:sdsId> cmu-band state ( disable | enable ) data <UINT:data>
 */
cparser_result_t
cparser_cmd_serdes_set_sdsId_cmu_band_state_disable_enable_data_data(
    cparser_context_t *context,
    uint32_t *sdsId_ptr,
    uint32_t *data_ptr)
{
    rtk_enable_t    en;
    uint32          unit;
    int32           ret;

    DIAG_UTIL_FUNC_INIT(unit);

    if ('d' == TOKEN_CHAR(5, 0))
        en = DISABLED;
    else
        en = ENABLED;

    DIAG_UTIL_ERR_CHK(rtk_sds_cmuBand_set(unit, *sdsId_ptr, en, *data_ptr), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_serdes_set_sdsId_cmu_band_state_disable_enable_data_data */

#endif

#ifdef CMD_SERDES_EYE_MONITOR_SDSID_FRAMENUM
/*
 * serdes eye-monitor <UINT:sdsId> <UINT:frameNum>
 */
cparser_result_t
cparser_cmd_serdes_eye_monitor_sdsId_frameNum(
    cparser_context_t *context,
    uint32_t *sdsId_ptr,
    uint32_t *frameNum_ptr)
{
    uint32  unit;
    int32   ret;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_sds_eyeMonitor_start(unit, *sdsId_ptr, *frameNum_ptr), ret);

    diag_util_printf("Done\n");

    return CPARSER_OK;
}   /* end of cparser_cmd_serdes_eye_monitor_sdsId_frameNum */
#endif

#ifdef CMD_SERDES_SET_SDSID_EYE_PARAM_PRE_STATE_DISABLE_ENABLE
/*
 * serdes set <UINT:sdsId> eye-param pre-state ( disable | enable )
 */
cparser_result_t
cparser_cmd_serdes_set_sdsId_eye_param_pre_state_disable_enable(
    cparser_context_t *context,
    uint32_t *sdsId_ptr)
{
    uint32  unit;
    int32   ret;
    rtk_sds_eyeParam_t param;
    rtk_enable_t    en;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('d' == TOKEN_CHAR(5, 0))
        en = DISABLED;
    else
        en = ENABLED;

    osal_memset(&param, 0, sizeof(rtk_sds_eyeParam_t));
    DIAG_UTIL_ERR_CHK(rtk_sds_eyeParam_get(unit, *sdsId_ptr, &param), ret);

    param.pre_en = en;
    DIAG_UTIL_ERR_CHK(rtk_sds_eyeParam_set(unit, *sdsId_ptr, param), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_serdes_set_sdsId_eye_param_pre_state_disable_enable */
#endif

#ifdef CMD_SERDES_SET_SDSID_EYE_PARAM_POST_STATE_DISABLE_ENABLE
/*
 * serdes set <UINT:sdsId> eye-param post-state ( disable | enable )
 */
cparser_result_t
cparser_cmd_serdes_set_sdsId_eye_param_post_state_disable_enable(
    cparser_context_t *context,
    uint32_t *sdsId_ptr)
{
    uint32  unit;
    int32   ret;
    rtk_sds_eyeParam_t param;
    rtk_enable_t    en;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('d' == TOKEN_CHAR(5, 0))
        en = DISABLED;
    else
        en = ENABLED;

    osal_memset(&param, 0, sizeof(rtk_sds_eyeParam_t));
    DIAG_UTIL_ERR_CHK(rtk_sds_eyeParam_get(unit, *sdsId_ptr, &param), ret);

    param.post_en = en;
    DIAG_UTIL_ERR_CHK(rtk_sds_eyeParam_set(unit, *sdsId_ptr, param), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_serdes_set_sdsId_eye_param_post_state_disable_enable */
#endif

#ifdef CMD_SERDES_SET_SDSID_EYE_PARAM_PRE_AMP_PRE_AMP
/*
 * serdes set <UINT:sdsId> eye-param pre-amp <UINT:pre_amp>
 */
cparser_result_t
cparser_cmd_serdes_set_sdsId_eye_param_pre_amp_pre_amp(
    cparser_context_t *context,
    uint32_t *sdsId_ptr,
    uint32_t *pre_amp_ptr)
{
    uint32  unit;
    int32   ret;
    rtk_sds_eyeParam_t param;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&param, 0, sizeof(rtk_sds_eyeParam_t));
    DIAG_UTIL_ERR_CHK(rtk_sds_eyeParam_get(unit, *sdsId_ptr, &param), ret);

    param.pre_amp = *pre_amp_ptr;
    DIAG_UTIL_ERR_CHK(rtk_sds_eyeParam_set(unit, *sdsId_ptr, param), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_serdes_set_sdsId_eye_param_pre_amp_pre_amp */
#endif

#ifdef CMD_SERDES_SET_SDSID_EYE_PARAM_MAIN_AMP_MAIN_AMP
/*
 * serdes set <UINT:sdsId> eye-param main-amp <UINT:main_amp>
 */
cparser_result_t
cparser_cmd_serdes_set_sdsId_eye_param_main_amp_main_amp(
    cparser_context_t *context,
    uint32_t *sdsId_ptr,
    uint32_t *main_amp_ptr)
{
    uint32  unit;
    int32   ret;
    rtk_sds_eyeParam_t param;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&param, 0, sizeof(rtk_sds_eyeParam_t));
    DIAG_UTIL_ERR_CHK(rtk_sds_eyeParam_get(unit, *sdsId_ptr, &param), ret);

    param.main_amp = *main_amp_ptr;
    DIAG_UTIL_ERR_CHK(rtk_sds_eyeParam_set(unit, *sdsId_ptr, param), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_serdes_set_sdsId_eye_param_main_amp_main_amp */
#endif

#ifdef CMD_SERDES_SET_SDSID_EYE_PARAM_POST_AMP_POST_AMP
/*
 * serdes set <UINT:sdsId> eye-param post-amp <UINT:post_amp>
 */
cparser_result_t
cparser_cmd_serdes_set_sdsId_eye_param_post_amp_post_amp(
    cparser_context_t *context,
    uint32_t *sdsId_ptr,
    uint32_t *post_amp_ptr)
{
    uint32  unit;
    int32   ret;
    rtk_sds_eyeParam_t param;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&param, 0, sizeof(rtk_sds_eyeParam_t));
    DIAG_UTIL_ERR_CHK(rtk_sds_eyeParam_get(unit, *sdsId_ptr, &param), ret);

    param.post_amp = *post_amp_ptr;
    DIAG_UTIL_ERR_CHK(rtk_sds_eyeParam_set(unit, *sdsId_ptr, param), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_serdes_set_sdsId_eye_param_post_amp_post_amp */
#endif

#ifdef CMD_SERDES_GET_SDSID_EYE_PARAM
/*
 * serdes get <UINT:sdsId> eye-param
 */
cparser_result_t
cparser_cmd_serdes_get_sdsId_eye_param(
    cparser_context_t *context,
    uint32_t *sdsId_ptr)
{
    uint32  unit;
    int32   ret;
    rtk_sds_eyeParam_t param;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&param, 0, sizeof(rtk_sds_eyeParam_t));
    DIAG_UTIL_ERR_CHK(rtk_sds_eyeParam_get(unit, *sdsId_ptr, &param), ret);

    diag_util_mprintf("Pre-AMP State:%s\n",(param.pre_en == ENABLED) ? "Enable" : "Disabled");
    diag_util_mprintf("Pre-AMP:%u\n",param.pre_amp);
    diag_util_mprintf("Main-AMP:%u\n",param.main_amp);
    diag_util_mprintf("Post-AMP State:%s\n",(param.post_en == ENABLED) ? "Enable" : "Disabled");
    diag_util_mprintf("Post-AMP:%u\n",param.post_amp);
    diag_util_mprintf("Impedance:%u\n",param.impedance);

    return CPARSER_OK;
}   /* end of cparser_cmd_serdes_get_sdsId_eye_param */
#endif

#ifdef CMD_SERDES_EYE_MONITOR_INFO_SDSID_FRAMENUM
/*
 * serdes eye-monitor info <UINT:sdsId> <UINT:frameNum>
 */
cparser_result_t
cparser_cmd_serdes_eye_monitor_info_sdsId_frameNum(
    cparser_context_t *context,
    uint32_t *sdsId_ptr,
    uint32_t *frameNum_ptr)
{
    rtk_sds_eyeMonInfo_t    info;
    uint32                  unit;
    int32                   ret;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_sds_eyeMonitorInfo_get(unit, *sdsId_ptr, *frameNum_ptr, &info), ret);

    diag_util_mprintf("max height: %u\n", info.height);
    diag_util_mprintf("max width: %u\n", info.width);

    return CPARSER_OK;
}   /* end of cparser_cmd_serdes_eye_monitor_info_sdsId_frameNum */
#endif

#ifdef CMD_SERDES_GET_SDSID_INFO
/*
 * serdes get <UINT:sdsId> info
 */
cparser_result_t
cparser_cmd_serdes_get_sdsId_info(
    cparser_context_t *context,
    uint32_t *sdsId_ptr)
{
    rtk_sds_info_t  info;
    uint32          unit;
    int32           ret;

    DIAG_UTIL_FUNC_INIT(unit);

    osal_memset(&info, 0, sizeof(rtk_sds_info_t));
    DIAG_UTIL_ERR_CHK(rtk_sds_info_get(unit, *sdsId_ptr, &info), ret);

    diag_util_mprintf("SDS %u information:\n", *sdsId_ptr);
    diag_util_mprintf("  FGCAL OFST: %d\n", info.fgcal_ofst);
    diag_util_mprintf("  VTH: %d\n", info.vth);
    diag_util_mprintf("  DFE TAP0: %d\n", info.dfe_tap0);
    diag_util_mprintf("  DFE TAP1 even: %d\n", info.dfe_tap1_even);
    diag_util_mprintf("  DFE TAP1 odd: %d\n", info.dfe_tap1_odd);
    diag_util_mprintf("  DFE TAP2 even: %d\n", info.dfe_tap2_even);
    diag_util_mprintf("  DFE TAP2 odd: %d\n", info.dfe_tap2_odd);
    diag_util_mprintf("  DFE TAP3 even: %d\n", info.dfe_tap3_even);
    diag_util_mprintf("  DFE TAP3 odd: %d\n", info.dfe_tap3_odd);
    diag_util_mprintf("  DFE TAP4 even: %d\n", info.dfe_tap4_even);
    diag_util_mprintf("  DFE TAP4 odd: %d\n", info.dfe_tap4_odd);

    return CPARSER_OK;
}   /* end of cparser_cmd_serdes_get_sdsId_info */
#endif

#ifdef CMD_SERDES_GET_SDSID_LOOPBACK
/*
 * serdes get <UINT:sdsId> loopback
 */
cparser_result_t
cparser_cmd_serdes_get_sdsId_loopback(
    cparser_context_t *context,
    uint32_t *sdsId_ptr)
{
    rtk_enable_t    status;
    uint32          unit;
    int32           ret;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_sds_loopback_get(unit, *sdsId_ptr, &status), ret);

    diag_util_mprintf("SerDes %d loopback: %s\n", *sdsId_ptr, (status? DIAG_STR_ENABLE: DIAG_STR_DISABLE));

    return CPARSER_OK;
}   /* end of cparser_cmd_serdes_get_sdsId_loopback */
#endif

#ifdef CMD_SERDES_SET_SDSID_LOOPBACK_DISABLE_ENABLE
/*
 * serdes set <UINT:sdsId> loopback ( disable | enable )
 */
cparser_result_t
cparser_cmd_serdes_set_sdsId_loopback_disable_enable(
    cparser_context_t *context,
    uint32_t *sdsId_ptr)
{
    rtk_enable_t    status;
    uint32          unit;
    int32           ret;

    DIAG_UTIL_FUNC_INIT(unit);

    if('e' == TOKEN_CHAR(4, 0))
    {
        status = ENABLED;
    }
    else
    {
        status = DISABLED;
    }

    DIAG_UTIL_ERR_CHK(rtk_sds_loopback_set(unit, *sdsId_ptr, status), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_serdes_set_sdsId_loopback_disable_enable */
#endif

