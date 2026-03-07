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
 * $Revision: 98479 $
 * $Date: 2019-07-09 16:53:06 +0800 (Tue, 09 Jul 2019) $
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

#ifdef CMD_SERDES_DUMP
/**
 * serdes dump
 */
#define SDS_MAX_SDS 25
#define SDS_MAX_PAGES (0x31)
#define SDS_MAX_REGS (0x20)
cparser_result_t
cparser_cmd_serdes_dump(cparser_context_t *context)
{
    uint32 unit;
    uint32 data;
    int sds, page, reg;
    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();
    for (sds = 0; sds < SDS_MAX_SDS; sds++) {
        for (page = 0; page < SDS_MAX_PAGES; page++) {
            for (reg = 0; reg < SDS_MAX_REGS; reg++) {
                hal_serdes_reg_get(unit, sds, page, reg, &data);
                diag_util_printf("%d,%d,0x%x,0x%x,0x%x\n",sds, page, page, reg, data);
            }
        }
    }
    return CPARSER_OK;
}
#endif /* CMD_SERDES_DUMP */

#ifdef CMD_SERDES_DUMPOWRT_ID_SDSID
#define RTSDS_DBG_PAGE_NAMES	48

#define RTSDS_SUBPAGE(page)		(page % 64)
#define RTSDS_REG_CNT			32

#define RTSDS_931X_SDS_CNT		14
#define RTSDS_931X_PAGE_CNT		192

static const char * const rtsds_page_name[RTSDS_DBG_PAGE_NAMES] = {
	[0] = "SDS",		[1] = "SDS_EXT",
	[2] = "FIB",		[3] = "FIB_EXT",
	[4] = "DTE",		[5] = "DTE_EXT",
	[6] = "TGX",		[7] = "TGX_EXT",
	[8] = "ANA_RG",		[9] = "ANA_RG_EXT",
	[10] = "ANA_TG",	[11] = "ANA_TG_EXT",
	[31] = "ANA_WDIG",
	[32] = "ANA_MISC",	[33] = "ANA_COM",
	[34] = "ANA_SP",	[35] = "ANA_SP_EXT",
	[36] = "ANA_1G",	[37] = "ANA_1G_EXT",
	[38] = "ANA_2G",	[39] = "ANA_2G_EXT",
	[40] = "ANA_3G",	[41] = "ANA_3G_EXT",
	[42] = "ANA_5G",	[43] = "ANA_5G_EXT",
	[44] = "ANA_6G",	[45] = "ANA_6G_EXT",
	[46] = "ANA_10G",	[47] = "ANA_10G_EXT",
};

static int rtsds_get_backing_sds(int sds, int page)
{
	int map[] = { 0, 1, 2, 3, 6, 7, 10, 11, 14, 15, 18, 19, 22, 23 };
	int backsds;

	/* non-RTL931x and first two RTL931x SerDes have 1:1 frontend/backend mapping */
	if (CONFIG_SDK_RTL9310 || sds < 2)
		return sds;

	backsds = map[sds];
	if (sds & 1)
		backsds += (page >> 6); /* distribute "odd" to 3 background SerDes */
	else
		backsds += (page >> 7); /* distribute "even" to 2 background SerDes */

	return backsds;
}

/**
 * serdes dumpowrt id <UINT:sdsId>
 */
cparser_result_t
cparser_cmd_serdes_dumpowrt_id_sdsId(cparser_context_t *context, uint32_t *sdsId_ptr)
{
    uint32 unit;
    uint32 data;
    int regnum, subpage, page = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (!CONFIG_SDK_RTL9310) {
        diag_util_printf("Not supported!");
        return CPARSER_OK;
    }

    do {
        subpage = RTSDS_SUBPAGE(page);
        if (!subpage) {
            diag_util_printf("Back SDS %02d:", rtsds_get_backing_sds(*sdsId_ptr, page));
            for (regnum = 0; regnum < RTSDS_REG_CNT; regnum++)
                diag_util_printf("   %02X", regnum);
            diag_util_printf("\n");
        }

        if (subpage < RTSDS_DBG_PAGE_NAMES && rtsds_page_name[subpage])
            diag_util_printf("%*s: ", -11, rtsds_page_name[subpage]);
        else
            diag_util_printf("PAGE %02X    : ", page);

        for (regnum = 0; regnum < RTSDS_REG_CNT; regnum++) {
            hal_serdes_reg_get(unit, *sdsId_ptr, page, regnum, &data);
            diag_util_printf("%04X ", data);
        }
        diag_util_printf("\n");
    } while (++page < RTSDS_931X_PAGE_CNT);

    return CPARSER_OK;
}
#endif /* CMD_SERDES_DUMPOWRT */

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

    diag_util_printf(" latch_blk_lock: %u\n", info.latch_blk_lock);
    diag_util_printf(" latch_hiber: %u\n", info.latch_hiber);
    diag_util_printf(" ber: %u\n", info.ber);
    diag_util_printf(" blk_err: 0x%04X\n", info.blk_err);

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
    uint32  unit, leq;
    int32   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_sds_leq_get(unit, *sdsId_ptr, &leq), ret);

    diag_util_printf("SDS %u LEQ = %u\n", *sdsId_ptr, leq);

    return CPARSER_OK;
}   /* end of cparser_cmd_serdes_get_sdsId_testmode_cnt */
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

