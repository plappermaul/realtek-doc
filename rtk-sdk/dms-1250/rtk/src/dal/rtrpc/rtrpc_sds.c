/*
 * Copyright(c) Realtek Semiconductor Corporation, 2008
 * All rights reserved.
 *
 * $Revision: 81532 $
 * $Date: 2017-08-18 16:50:57 +0800 (Fri, 18 Aug 2017) $
 *
 * Purpose : Realtek Switch SDK rtrpc API Module
 *
 * Feature : The file have include the following module and sub-modules
 *           1) port
 *
 */

#include <common/rt_type.h>
#include <rtk/sds.h>
#include <dal/rtrpc/rtrpc_sds.h>
#include <dal/rtrpc/rtrpc_msg.h>
#include <rtdrv/rtdrv_netfilter.h>

int32
rtrpc_sds_symErr_get(uint32 unit, uint32 sds, rtk_sds_symErr_t *info)
{
    rtdrv_sdsCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    memset(&cfg, 0, sizeof(rtdrv_sdsCfg_t));
    memcpy(&cfg.unit, &unit, sizeof(uint32));
    memcpy(&cfg.sds, &sds, sizeof(uint32));
    GETSOCKOPT(RTDRV_SERDES_SYMERR_GET, &cfg, rtdrv_sdsCfg_t, 1);
    memcpy(info, &cfg.symErr, sizeof(rtk_sds_symErr_t));

    return RT_ERR_OK;
}   /* end of rtk_port_sdsSymErr_get */

int32
rtrpc_sds_symErr_clear(uint32 unit, uint32 sds)
{
    rtdrv_sdsCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    memset(&cfg, 0, sizeof(rtdrv_sdsCfg_t));
    memcpy(&cfg.unit, &unit, sizeof(uint32));
    memcpy(&cfg.sds, &sds, sizeof(uint32));
    SETSOCKOPT(RTDRV_SERDES_SYMERR_CLEAR, &cfg, rtdrv_sdsCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_port_sdsSymErr_clear */

int32
rtrpc_sds_linkSts_get(uint32 unit, uint32 sds, rtk_sds_linkSts_t *info)
{
    rtdrv_sdsCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    memset(&cfg, 0, sizeof(rtdrv_sdsCfg_t));
    memcpy(&cfg.unit, &unit, sizeof(uint32));
    memcpy(&cfg.sds, &sds, sizeof(uint32));
    GETSOCKOPT(RTDRV_SERDES_LINKSTS_GET, &cfg, rtdrv_sdsCfg_t, 1);
    memcpy(info, &cfg.linkSts, sizeof(rtk_sds_linkSts_t));

    return RT_ERR_OK;
}   /* end of rtk_port_sdsLinkSts_get */

int32
rtrpc_sds_testModeCnt_get(uint32 unit, uint32 sds, uint32 *cnt)
{
    rtdrv_sdsCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == cnt), RT_ERR_NULL_POINTER);

    /* function body */
    memset(&cfg, 0, sizeof(rtdrv_sdsCfg_t));
    memcpy(&cfg.unit, &unit, sizeof(uint32));
    memcpy(&cfg.sds, &sds, sizeof(uint32));
    GETSOCKOPT(RTDRV_SDS_TESTMODECNT_GET, &cfg, rtdrv_sdsCfg_t, 1);
    memcpy(cnt, &cfg.cnt, sizeof(uint32));

    return RT_ERR_OK;
}   /* end of rtk_sds_testModeCnt_get */

int32
rtrpc_sds_testMode_set(uint32 unit, uint32 sds, rtk_sds_testMode_t mode)
{
    rtdrv_sdsCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    memset(&cfg, 0, sizeof(rtdrv_sdsCfg_t));
    memcpy(&cfg.unit, &unit, sizeof(uint32));
    memcpy(&cfg.sds, &sds, sizeof(uint32));
    memcpy(&cfg.mode, &mode, sizeof(rtk_sds_testMode_t));
    SETSOCKOPT(RTDRV_SDS_TESTMODE_SET, &cfg, rtdrv_sdsCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_sds_testMode_set */

int32
rtrpc_sds_rx_rst(uint32 unit, uint32 sds)
{
    rtdrv_sdsCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&cfg, 0, sizeof(rtdrv_sdsCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.sds, &sds, sizeof(uint32));
    SETSOCKOPT(RTDRV_SDS_RX_RST, &cfg, rtdrv_sdsCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_sds_rx_rst */

int32
rtrpc_sds_leq_get(uint32 unit, uint32 sds, rtk_sds_leq_t *pLeq)
{
    rtdrv_sdsCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&cfg, 0, sizeof(rtdrv_sdsCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.sds, &sds, sizeof(uint32));
    GETSOCKOPT(RTDRV_SDS_LEQ_GET, &cfg, rtdrv_sdsCfg_t, 1);
    osal_memcpy(pLeq, &cfg.leq, sizeof(rtk_sds_leq_t));

    return RT_ERR_OK;
}   /* end of rtk_sds_leq_get */

int32
rtrpc_sds_leq_set(uint32 unit, uint32 sds, rtk_sds_leq_t *pLeq)
{
    rtdrv_sdsCfg_t cfg;
    uint32 master_view_unit = unit;

    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pLeq), RT_ERR_NULL_POINTER);

    /* function body */
    memset(&cfg, 0, sizeof(rtdrv_sdsCfg_t));
    memcpy(&cfg.unit, &unit, sizeof(uint32));
    memcpy(&cfg.sds, &sds, sizeof(uint32));
    memcpy(&cfg.leq, pLeq, sizeof(rtk_sds_leq_t));
    SETSOCKOPT(RTDRV_SDS_LEQ_SET, &cfg, rtdrv_sdsCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtrpc_sds_leq_set */

int32
rtrpc_sds_leq_adapt(uint32 unit, uint32 sds)
{
    rtdrv_sdsCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&cfg, 0, sizeof(rtdrv_sdsCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.sds, &sds, sizeof(uint32));
    SETSOCKOPT(RTDRV_SDS_LEQ_ADAPT, &cfg, rtdrv_sdsCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_sds_leq_adapt */

int32
rtrpc_sds_xsgNwayEn_set(uint32 unit, uint32 sds, rtk_enable_t en)
{
    rtdrv_sdsCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&cfg, 0, sizeof(rtdrv_sdsCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.sds, &sds, sizeof(uint32));
    osal_memcpy(&cfg.en, &en, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_SDS_XSGNWAYEN_SET, &cfg, rtdrv_sdsCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_sds_xsgNwayEn_set */

int32
rtrpc_sds_cmuBand_get(uint32 unit, uint32 sds, uint32 *band)
{
    rtdrv_sdsCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&cfg, 0, sizeof(rtdrv_sdsCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.sds, &sds, sizeof(uint32));
    GETSOCKOPT(RTDRV_SDS_CMUBAND_GET, &cfg, rtdrv_sdsCfg_t, 1);
    osal_memcpy(band, &cfg.cnt, sizeof(uint32));

    return RT_ERR_OK;
}   /* end of rtk_sds_cmuBand_get */

int32
rtrpc_sds_cmuBand_set(uint32 unit, uint32 sds, rtk_enable_t en, uint32 val)
{
    rtdrv_sdsCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&cfg, 0, sizeof(rtdrv_sdsCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.sds, &sds, sizeof(uint32));
    osal_memcpy(&cfg.en, &en, sizeof(rtk_enable_t));
    osal_memcpy(&cfg.cnt, &val, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_SDS_CMUBAND_SET, &cfg, rtdrv_sdsCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_sds_cmuBand_set */

int32
rtrpc_sds_eyeMonitor_start(uint32 unit, uint32 sds, uint32 frameNum)
{
    rtdrv_sdsCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    memset(&cfg, 0, sizeof(rtdrv_sdsCfg_t));
    memcpy(&cfg.unit, &unit, sizeof(uint32));
    memcpy(&cfg.sds, &sds, sizeof(uint32));
    memcpy(&cfg.frameNum, &frameNum, sizeof(uint32));
    SETSOCKOPT(RTDRV_SDS_EYEMONITOR_START, &cfg, rtdrv_sdsCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_sds_eyeMonitor_start */

/* Function Name:
 *      rtk_sds_eyeParam_get
 * Description:
 *      Get SerDes eye parameter
 * Input:
 *      unit - unit id
 *      sds  - SerDes id
 * Output:
 *      pParam - eye parameter
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
int32
rtrpc_sds_eyeParam_get(uint32 unit, uint32 sds, rtk_sds_eyeParam_t  *pParam)
{
    rtdrv_sdsCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pParam), RT_ERR_NULL_POINTER);

    /* function body */
    memcpy(&cfg.unit, &unit, sizeof(uint32));
    memcpy(&cfg.sds, &sds, sizeof(uint32));
    GETSOCKOPT(RTDRV_SDS_EYEPARAM_GET, &cfg, rtdrv_sdsCfg_t, 1);
    memcpy(pParam, &cfg.param, sizeof(rtk_sds_eyeParam_t));

    return RT_ERR_OK;
}   /* end of rtk_sds_eyeParam_get */

/* Function Name:
 *      rtk_sds_eyeParam_set
 * Description:
 *      Set SerDes eye parameters
 * Input:
 *      unit - unit id
 *      sds  - SerDes id
 *      param  - eye parameter value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
int32
rtrpc_sds_eyeParam_set(uint32 unit, uint32 sds, rtk_sds_eyeParam_t  param)
{
    rtdrv_sdsCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    memcpy(&cfg.unit, &unit, sizeof(uint32));
    memcpy(&cfg.sds, &sds, sizeof(uint32));
    memcpy(&cfg.param, &param, sizeof(rtk_sds_eyeParam_t));
    SETSOCKOPT(RTDRV_SDS_EYEPARAM_SET, &cfg, rtdrv_sdsCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_sds_eyeParam_set */


int32
rtrpc_sds_rxCaliConf_get(uint32 unit, uint32 sds, rtk_sds_rxCaliConf_t *pConf)
{
    rtdrv_sdsCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    memset(&cfg, 0, sizeof(rtdrv_sdsCfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pConf), RT_ERR_NULL_POINTER);

    memcpy(&cfg.unit, &unit, sizeof(uint32));
    memcpy(&cfg.sds, &sds, sizeof(uint32));
    GETSOCKOPT(RTDRV_SDS_RXCALICONF_GET, &cfg, rtdrv_sdsCfg_t, 1);
    memcpy(pConf, &cfg.conf, sizeof(rtk_sds_rxCaliConf_t));

    return RT_ERR_OK;
}   /* end of rtrpc_sds_rxCaliConf_get */

int32
rtrpc_sds_rxCaliConf_set(uint32 unit, uint32 sds, rtk_sds_rxCaliConf_t conf)
{
    rtdrv_sdsCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    memset(&cfg, 0, sizeof(rtdrv_sdsCfg_t));
    memcpy(&cfg.unit, &unit, sizeof(uint32));
    memcpy(&cfg.sds, &sds, sizeof(uint32));
    memcpy(&cfg.conf, &conf, sizeof(rtk_sds_rxCaliConf_t));
    SETSOCKOPT(RTDRV_SDS_RXCALICONF_SET, &cfg, rtdrv_sdsCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtrpc_sds_rxCaliConf_set */

int32
rtrpc_sdsMapper_init(dal_mapper_t *pMapper)
{
    pMapper->sds_symErr_get = rtrpc_sds_symErr_get;
    pMapper->sds_symErr_clear = rtrpc_sds_symErr_clear;
    pMapper->sds_linkSts_get = rtrpc_sds_linkSts_get;
    pMapper->sds_testModeCnt_get = rtrpc_sds_testModeCnt_get;
    pMapper->sds_testMode_set = rtrpc_sds_testMode_set;
    pMapper->sds_rx_rst = rtrpc_sds_rx_rst;
    pMapper->sds_leq_adapt = rtrpc_sds_leq_adapt;
    pMapper->sds_leq_get = rtrpc_sds_leq_get;
    pMapper->sds_xsgNwayEn_set = rtrpc_sds_xsgNwayEn_set;
    pMapper->sds_cmuBand_get = rtrpc_sds_cmuBand_get;
    pMapper->sds_cmuBand_set = rtrpc_sds_cmuBand_set;
    pMapper->sds_eyeMonitor_start = rtrpc_sds_eyeMonitor_start;
    pMapper->sds_eyeParam_get = rtrpc_sds_eyeParam_get;
    pMapper->sds_eyeParam_set = rtrpc_sds_eyeParam_set;
    pMapper->sds_eyeMonitorInfo_get = rtrpc_sds_eyeMonitorInfo_get;
    pMapper->sds_info_get = rtrpc_sds_info_get;
    pMapper->sds_info_get = rtrpc_sds_info_get;
    pMapper->sds_loopback_get = rtrpc_sds_loopback_get;
    pMapper->sds_loopback_set = rtrpc_sds_loopback_set;
    pMapper->sds_ctrl_get = rtrpc_sds_ctrl_get;
    pMapper->sds_ctrl_set = rtrpc_sds_ctrl_set;
    return RT_ERR_OK;
}

int32
rtrpc_sds_eyeMonitorInfo_get(uint32 unit, uint32 sds, uint32 frameNum, rtk_sds_eyeMonInfo_t *pInfo)
{
    rtdrv_sdsCfg_t cfg;
    uint32 master_view_unit = unit;

    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pInfo), RT_ERR_NULL_POINTER);

    /* function body */
    memset(&cfg, 0, sizeof(rtdrv_sdsCfg_t));
    memcpy(&cfg.unit, &unit, sizeof(uint32));
    memcpy(&cfg.sds, &sds, sizeof(uint32));
    memcpy(&cfg.frameNum, &frameNum, sizeof(uint32));
    GETSOCKOPT(RTDRV_SDS_EYEMONITORINFO_GET, &cfg, rtdrv_sdsCfg_t, 1);
    memcpy(pInfo, &cfg.eyeInfo, sizeof(rtk_sds_eyeMonInfo_t));

    return RT_ERR_OK;
}   /* end of rtrpc_sds_eyeMonitorInfo_get */


int32
rtrpc_sds_info_get(uint32 unit, uint32 sds, rtk_sds_info_t *pInfo)
{
    rtdrv_sdsCfg_t cfg;
    uint32 master_view_unit = unit;

    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pInfo), RT_ERR_NULL_POINTER);

    /* function body */
    memset(&cfg, 0, sizeof(rtdrv_sdsCfg_t));
    memcpy(&cfg.unit, &unit, sizeof(uint32));
    memcpy(&cfg.sds, &sds, sizeof(uint32));
    GETSOCKOPT(RTDRV_SDS_INFO_GET, &cfg, rtdrv_sdsCfg_t, 1);
    memcpy(pInfo, &cfg.info, sizeof(rtk_sds_info_t));

    return RT_ERR_OK;
}   /* end of rtrpc_sds_info_get */


int32
rtrpc_sds_loopback_get(uint32 unit, uint32 sds, rtk_enable_t *pStatus)
{
    rtdrv_sdsCfg_t cfg;
    uint32 master_view_unit = unit;

    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pStatus), RT_ERR_NULL_POINTER);

    /* function body */
    memset(&cfg, 0, sizeof(rtdrv_sdsCfg_t));
    memcpy(&cfg.unit, &unit, sizeof(uint32));
    memcpy(&cfg.sds, &sds, sizeof(uint32));
    GETSOCKOPT(RTDRV_SDS_LOOPBACK_GET, &cfg, rtdrv_sdsCfg_t, 1);
    memcpy(pStatus, &cfg.en, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtrpc_sds_loopback_get */

int32
rtrpc_sds_loopback_set(uint32 unit, uint32 sds, rtk_enable_t status)
{
    rtdrv_sdsCfg_t cfg;
    uint32 master_view_unit = unit;

    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    /* function body */
    memset(&cfg, 0, sizeof(rtdrv_sdsCfg_t));
    memcpy(&cfg.unit, &unit, sizeof(uint32));
    memcpy(&cfg.sds, &sds, sizeof(uint32));
    memcpy(&cfg.en, &status, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_SDS_LOOPBACK_SET, &cfg, rtdrv_sdsCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtrpc_sds_loopback_set */

int32
rtrpc_sds_ctrl_get(uint32 unit, uint32 sds, rtk_sds_ctrl_t ctrlType,
    uint32 *pValue)
{
    rtdrv_sdsCfg_t cfg;
    uint32 master_view_unit = unit;

    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pValue), RT_ERR_NULL_POINTER);

    /* function body */
    memset(&cfg, 0, sizeof(rtdrv_sdsCfg_t));
    memcpy(&cfg.unit, &unit, sizeof(uint32));
    memcpy(&cfg.sds, &sds, sizeof(uint32));
    memcpy(&cfg.ctrlType, &ctrlType, sizeof(rtk_sds_ctrl_t));
    GETSOCKOPT(RTDRV_SDS_CTRL_GET, &cfg, rtdrv_sdsCfg_t, 1);
    memcpy(pValue, &cfg.value, sizeof(uint32));

    return RT_ERR_OK;
}   /* end of rtrpc_sds_ctrl_get */

int32
rtrpc_sds_ctrl_set(uint32 unit, uint32 sds, rtk_sds_ctrl_t ctrlType,
    uint32 value)
{
    rtdrv_sdsCfg_t cfg;
    uint32 master_view_unit = unit;

    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    /* function body */
    memset(&cfg, 0, sizeof(rtdrv_sdsCfg_t));
    memcpy(&cfg.unit, &unit, sizeof(uint32));
    memcpy(&cfg.sds, &sds, sizeof(uint32));
    memcpy(&cfg.ctrlType, &ctrlType, sizeof(rtk_sds_ctrl_t));
    memcpy(&cfg.value, &value, sizeof(uint32));
    SETSOCKOPT(RTDRV_SDS_CTRL_SET, &cfg, rtdrv_sdsCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtrpc_sds_ctrl_set */

