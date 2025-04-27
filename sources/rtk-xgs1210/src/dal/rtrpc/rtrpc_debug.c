/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * $Revision: 84208 $
 * $Date: 2017-12-08 10:09:15 +0800 (Fri, 08 Dec 2017) $
 *
 * Purpose : Realtek Switch SDK Debug Module
 *
 * Feature : The file have include the following module and sub-modules
 *           1) SDK Debug Module for Linux User Mode
 *
 */

/*
 * Include Files
 */

#ifdef RTUSR
  #include <stdarg.h>
  #include <time.h>
#endif

#include <common/debug/rt_log.h>
#include <rtdrv/rtdrv_netfilter.h>
#include <dal/rtrpc/rtrpc_msg.h>
#if defined(CONFIG_SDK_RTL8295) || defined(CONFIG_SDK_RTL8295R) || defined(CONFIG_SDK_RTL8214QF)
  #include <hal/mac/rtl8295.h>
  #include <hal/phy/phy_rtl8295_patch.h>
#endif
#include <dal/rtrpc/rtrpc_debug.h>

/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */
#ifdef RTUSR
static uint32 log_init = INIT_NOT_COMPLETED;
#endif
uint8 *rtLogModuleName[] =
{
    RT_LOG_MODULE_STRING
};

/*
 * Macro Declaration
 */
#define RT_LOG_CONFIG_GET(type, lv, lv_mask,                    \
                          mod_mask, format, init)                     \
do {                                                            \
    rtdrv_logCfg_t cfg;                                         \
    if(RT_ERR_OK == rt_log_config_get((uint32 *) &cfg))         \
    {                                                           \
        type = cfg.log_level_type;                              \
        lv = cfg.log_level;                                     \
        lv_mask = cfg.log_level_mask;                           \
        mod_mask = cfg.log_module_mask;                         \
        format = cfg.log_format;                                \
        init = cfg.log_init;                                \
    }                                                           \
} while(0)

/*
 * Function Declaration
 */

#ifdef RTUSR
int32 rtrpc_rt_log_enable_get(rtk_enable_t *pEnabled)
{
    rtdrv_unitCfg_t unit_cfg;
    uint32 unit = RTRPC_UNIT_NOT_USED;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&unit_cfg, 0, sizeof(rtdrv_unitCfg_t));

    GETSOCKOPT(RTDRV_DEBUG_EN_LOG_GET, &unit_cfg, rtdrv_unitCfg_t, 1);
    *pEnabled = unit_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_rt_log_enable_set(rtk_enable_t enabled)
{
    rtdrv_unitCfg_t unit_cfg;
    uint32 unit = RTRPC_UNIT_NOT_USED;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&unit_cfg, 0, sizeof(rtdrv_unitCfg_t));

    unit_cfg.data = enabled;
    SETSOCKOPT(RTDRV_DEBUG_EN_LOG_SET, &unit_cfg, rtdrv_unitCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_rt_log_level_get(uint32 *pLv)
{
    rtdrv_unitCfg_t unit_cfg;
    uint32 unit = RTRPC_UNIT_NOT_USED;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&unit_cfg, 0, sizeof(rtdrv_unitCfg_t));

    GETSOCKOPT(RTDRV_DEBUG_LOGLV_GET, &unit_cfg, rtdrv_unitCfg_t, 1);
    *pLv = unit_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_rt_log_level_set(uint32 lv)
{
    rtdrv_unitCfg_t unit_cfg;
    uint32 unit = RTRPC_UNIT_NOT_USED;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&unit_cfg, 0, sizeof(rtdrv_unitCfg_t));

    unit_cfg.data = lv;
    SETSOCKOPT(RTDRV_DEBUG_LOGLV_SET, &unit_cfg, rtdrv_unitCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_rt_log_mask_get(uint32 *pMask)
{
    rtdrv_unitCfg_t unit_cfg;
    uint32 unit = RTRPC_UNIT_NOT_USED;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&unit_cfg, 0, sizeof(rtdrv_unitCfg_t));

    GETSOCKOPT(RTDRV_DEBUG_LOGLVMASK_GET, &unit_cfg, rtdrv_unitCfg_t, 1);
    *pMask = unit_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_rt_log_mask_set(uint32 mask)
{
    rtdrv_unitCfg_t unit_cfg;
    uint32 unit = RTRPC_UNIT_NOT_USED;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&unit_cfg, 0, sizeof(rtdrv_unitCfg_t));

    unit_cfg.data = mask;
    SETSOCKOPT(RTDRV_DEBUG_LOGLVMASK_SET, &unit_cfg, rtdrv_unitCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_rt_log_type_get(uint32 *pType)
{
    rtdrv_unitCfg_t unit_cfg;
    uint32 unit = RTRPC_UNIT_NOT_USED;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&unit_cfg, 0, sizeof(rtdrv_unitCfg_t));

    GETSOCKOPT(RTDRV_DEBUG_LOGTYPE_GET, &unit_cfg, rtdrv_unitCfg_t, 1);
    *pType = unit_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_rt_log_type_set(uint32 type)
{
    rtdrv_unitCfg_t unit_cfg;
    uint32 unit = RTRPC_UNIT_NOT_USED;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&unit_cfg, 0, sizeof(rtdrv_unitCfg_t));

    unit_cfg.data = type;
    SETSOCKOPT(RTDRV_DEBUG_LOGTYPE_SET, &unit_cfg, rtdrv_unitCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_rt_log_format_get(uint32 *pFormat)
{
    rtdrv_unitCfg_t unit_cfg;
    uint32 unit = RTRPC_UNIT_NOT_USED;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&unit_cfg, 0, sizeof(rtdrv_unitCfg_t));

    GETSOCKOPT(RTDRV_DEBUG_LOGFORMAT_GET, &unit_cfg, rtdrv_unitCfg_t, 1);
    *pFormat = unit_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_rt_log_format_set(uint32 format)
{
    rtdrv_unitCfg_t unit_cfg;
    uint32 unit = RTRPC_UNIT_NOT_USED;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&unit_cfg, 0, sizeof(rtdrv_unitCfg_t));

    unit_cfg.data = format;
    SETSOCKOPT(RTDRV_DEBUG_LOGFORMAT_SET, &unit_cfg, rtdrv_unitCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_rt_log_moduleMask_get(uint64 *pMask)
{
    rtdrv_unitCfg_t unit_cfg;
    uint32 unit = RTRPC_UNIT_NOT_USED;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&unit_cfg, 0, sizeof(rtdrv_unitCfg_t));

    GETSOCKOPT(RTDRV_DEBUG_MODMASK_GET, &unit_cfg, rtdrv_unitCfg_t, 1);
    *pMask = unit_cfg.data64;

    return RT_ERR_OK;
}

int32 rtrpc_rt_log_moduleMask_set(uint64 mask)
{
    rtdrv_unitCfg_t unit_cfg;
    uint32 unit = RTRPC_UNIT_NOT_USED;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&unit_cfg, 0, sizeof(rtdrv_unitCfg_t));

    unit_cfg.data64 = mask;
    SETSOCKOPT(RTDRV_DEBUG_MODMASK_SET, &unit_cfg, rtdrv_unitCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_rt_log_config_get(uint32 *pCfg)
{
    uint32 unit = RTRPC_UNIT_NOT_USED;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    GETSOCKOPT(RTDRV_DEBUG_LOGCFG_GET, pCfg, rtdrv_logCfg_t, 1);

    return RT_ERR_OK;
}

uint8** rt_log_moduleName_get(uint64 module)
{
    uint32 i;

    /* parameter check */
    RT_PARAM_CHK(!LOG_MOD_CHK(module), (rtLogModuleName + SDK_MOD_END));

    for (i = 0; i < SDK_MOD_END; i++)
        if ((module >> i) & 0x1) break;

    return (rtLogModuleName + i);
}

int32 rt_log(const int32 level, const int64 module, const char *format, ...)
{
    /* start logging, determine the length of the output string */
    uint32 log_type = 0, log_level = 0, log_mask = 0, log_module_mask = 0, log_format = 0;
    int32  result = RT_ERR_FAILED;
    static uint8  buf[LOG_BUFSIZE_DEFAULT]; /* init value will be given by RT_LOG_FORMATTED_OUTPUT */

    /* get the log config setting from the module */
    RT_LOG_CONFIG_GET(log_type, log_level, log_mask, log_module_mask, log_format, log_init);

    /* check log level and module */
    RT_LOG_PARAM_CHK(level, module);

    /* formatted output conversion */
    RT_LOG_FORMATTED_OUTPUT(buf, format, result);

    if (result < 0)
        return RT_ERR_FAILED;

    /* get the current time */
    time_t t = time(0);
    struct tm *pLt = localtime(&t);

    /* if that worked, print to console */
    if (LOG_FORMAT_DETAILED == log_format)
    {
        rt_log_printf("* ------------------------------------------------------------\n");
        rt_log_printf("* Level : %d\n", level);
        rt_log_printf("* Module: %s\n", *rt_log_moduleName_get(log_module_mask & module));
        rt_log_printf("* Date  : %04d-%02d-%02d\n", pLt->tm_year + 1900, pLt->tm_mon + 1, pLt->tm_mday);
        rt_log_printf("* Time  : %02d:%02d:%02d\n", pLt->tm_hour, pLt->tm_min, pLt->tm_sec);
        rt_log_printf("* Log   : %s\n", buf);
    }
    else
    {
        rt_log_printf("[%d][%s] %02d:%02d:%02d %s", level, *rt_log_moduleName_get(log_module_mask & module),
                      pLt->tm_hour, pLt->tm_min, pLt->tm_sec, buf);
    }

    return RT_ERR_OK;
}

int32 rtrpc_hal_dumpHsb(uint32 unit)
{
    rtdrv_unitCfg_t unit_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&unit_cfg, 0, sizeof(rtdrv_unitCfg_t));

    unit_cfg.unit = unit;
    SETSOCKOPT(RTDRV_DEBUG_HSB_DUMP, &unit_cfg, rtdrv_unitCfg_t, 1);
    return RT_ERR_OK;
}

int32 rtrpc_hal_dumpHsb_openflow(uint32 unit)
{
    rtdrv_unitCfg_t unit_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&unit_cfg, 0, sizeof(rtdrv_unitCfg_t));

    unit_cfg.unit = unit;
    SETSOCKOPT(RTDRV_DEBUG_HSB_OPENFLOW_DUMP, &unit_cfg, rtdrv_unitCfg_t, 1);
    return RT_ERR_OK;
}

int32 rtrpc_hal_dumpPpi(uint32 unit, uint32 index)
{
    rtdrv_unitCfg_t unit_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&unit_cfg, 0, sizeof(rtdrv_unitCfg_t));

    unit_cfg.unit = unit;
    unit_cfg.data = index;
    SETSOCKOPT(RTDRV_DEBUG_PPI_DUMP, &unit_cfg, rtdrv_unitCfg_t, 1);
    return RT_ERR_OK;
}

int32 rtrpc_hal_dumpPmi(uint32 unit)
{
    rtdrv_unitCfg_t unit_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&unit_cfg, 0, sizeof(rtdrv_unitCfg_t));

    unit_cfg.unit = unit;
    SETSOCKOPT(RTDRV_DEBUG_PMI_DUMP, &unit_cfg, rtdrv_unitCfg_t, 1);
    return RT_ERR_OK;
}

int32 rtrpc_hal_dumpHsa(uint32 unit)
{
    rtdrv_unitCfg_t unit_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&unit_cfg, 0, sizeof(rtdrv_unitCfg_t));

    unit_cfg.unit = unit;
    SETSOCKOPT(RTDRV_DEBUG_HSA_DUMP, &unit_cfg, rtdrv_unitCfg_t, 1);
    return RT_ERR_OK;
}

int32 rtrpc_hal_dumpHsa_openflow(uint32 unit)
{
    rtdrv_unitCfg_t unit_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&unit_cfg, 0, sizeof(rtdrv_unitCfg_t));

    unit_cfg.unit = unit;
    SETSOCKOPT(RTDRV_DEBUG_HSA_OPENFLOW_DUMP, &unit_cfg, rtdrv_unitCfg_t, 1);
    return RT_ERR_OK;
}

int32 rtrpc_hal_dumpHsm(uint32 unit)
{
    rtdrv_unitCfg_t unit_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&unit_cfg, 0, sizeof(rtdrv_unitCfg_t));

    unit_cfg.unit = unit;
    SETSOCKOPT(RTDRV_DEBUG_HSM_DUMP, &unit_cfg, rtdrv_unitCfg_t, 1);
    return RT_ERR_OK;
}

int32 rtrpc_hal_dumpHsm_openflow(uint32 unit)
{
    rtdrv_unitCfg_t unit_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&unit_cfg, 0, sizeof(rtdrv_unitCfg_t));

    unit_cfg.unit = unit;
    SETSOCKOPT(RTDRV_DEBUG_HSM_OPENFLOW_DUMP, &unit_cfg, rtdrv_unitCfg_t, 1);
    return RT_ERR_OK;
}

int32 rtrpc_hal_dumpHsmIdx(uint32 unit, uint32 index)
{
    rtdrv_unitCfg_t unit_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&unit_cfg, 0, sizeof(rtdrv_unitCfg_t));

    unit_cfg.unit = unit;
    unit_cfg.data = index;
    SETSOCKOPT(RTDRV_DEBUG_HSM_IDX_DUMP, &unit_cfg, rtdrv_unitCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_hal_getDbgCntr(uint32 unit, rtk_dbg_mib_dbgType_t type, uint32 *pCntr)
{
    rtdrv_unitCfg_t unit_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&unit_cfg, 0, sizeof(rtdrv_unitCfg_t));

    unit_cfg.unit = unit;
    unit_cfg.mibType = type;
    GETSOCKOPT(RTDRV_DEBUG_MIB_DBG_CNTR_GET, &unit_cfg, rtdrv_unitCfg_t, 1);
    *pCntr = unit_cfg.cntr;

    return RT_ERR_OK;
}

int32 rtrpc_hal_getFlowCtrlIgrPortUsedPageCnt(uint32 unit, rtk_port_t port, uint32 *pCntr, uint32 *pMaxCntr)
{
    rtdrv_unitCfg_t unit_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&unit_cfg, 0, sizeof(rtdrv_unitCfg_t));

    unit_cfg.unit = unit;
    unit_cfg.port = port;
    GETSOCKOPT(RTDRV_DEBUG_FLOWCTRL_PORT_USED_PAGE_CNT_INGRESS_GET, &unit_cfg, rtdrv_unitCfg_t, 1);
    *pCntr = unit_cfg.cntr;
    *pMaxCntr = unit_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_hal_getFlowCtrlEgrPortUsedPageCnt(uint32 unit, rtk_port_t port, uint32 *pCntr, uint32 *pMaxCntr)
{
    rtdrv_unitCfg_t unit_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&unit_cfg, 0, sizeof(rtdrv_unitCfg_t));

    unit_cfg.unit = unit;
    unit_cfg.port = port;
    GETSOCKOPT(RTDRV_DEBUG_FLOWCTRL_PORT_USED_PAGE_CNT_EGRESS_GET, &unit_cfg, rtdrv_unitCfg_t, 1);
    *pCntr = unit_cfg.cntr;
    *pMaxCntr = unit_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_hal_getFlowCtrlSystemUsedPageCnt(uint32 unit, uint32 *pCntr, uint32 *pMaxCntr)
{
    rtdrv_unitCfg_t unit_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&unit_cfg, 0, sizeof(rtdrv_unitCfg_t));

    unit_cfg.unit = unit;
    GETSOCKOPT(RTDRV_DEBUG_FLOWCTRL_SYSTEM_USED_PAGE_CNT_GET, &unit_cfg, rtdrv_unitCfg_t, 1);
    *pCntr = unit_cfg.cntr;
    *pMaxCntr = unit_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_hal_getFlowCtrlSystemPublicUsedPageCnt(uint32 unit, uint32 *pCntr, uint32 *pMaxCntr)
{
    rtdrv_unitCfg_t unit_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&unit_cfg, 0, sizeof(rtdrv_unitCfg_t));

    unit_cfg.unit = unit;
    GETSOCKOPT(RTDRV_DEBUG_FLOWCTRL_SYSTEM_PUB_USED_PAGE_CNT_GET, &unit_cfg, rtdrv_unitCfg_t, 1);
    *pCntr = unit_cfg.cntr;
    *pMaxCntr = unit_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_hal_getFlowCtrlSystemIgrQueueUsedPageCnt(uint32 unit, rtk_dbg_queue_usedPageCnt_t *pQCntr, rtk_dbg_queue_usedPageCnt_t *pQMaxCntr)
{
    rtdrv_unitCfg_t unit_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&unit_cfg, 0, sizeof(rtdrv_unitCfg_t));

    unit_cfg.unit = unit;
    GETSOCKOPT(RTDRV_DEBUG_FLOWCTRL_SYSTEM_IGR_QUEUE_USED_PAGE_CNT_GET, &unit_cfg, rtdrv_unitCfg_t, 1);
    osal_memcpy(pQCntr, &unit_cfg.qCntr, sizeof(rtk_dbg_queue_usedPageCnt_t));
    osal_memcpy(pQMaxCntr, &unit_cfg.qMaxCntr, sizeof(rtk_dbg_queue_usedPageCnt_t));

    return RT_ERR_OK;
}

int32 rtrpc_hal_getFlowCtrlPortIgrQueueUsedPageCnt(uint32 unit, rtk_port_t port, rtk_dbg_queue_usedPageCnt_t *pQCntr, rtk_dbg_queue_usedPageCnt_t *pQMaxCntr)
{
    rtdrv_unitCfg_t unit_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&unit_cfg, 0, sizeof(rtdrv_unitCfg_t));

    unit_cfg.unit = unit;
    unit_cfg.port = port;
    GETSOCKOPT(RTDRV_DEBUG_FLOWCTRL_PORT_QUEUE_USED_PAGE_CNT_INGRESS_GET, &unit_cfg, rtdrv_unitCfg_t, 1);
    osal_memcpy(pQCntr, &unit_cfg.qCntr, sizeof(rtk_dbg_queue_usedPageCnt_t));
    osal_memcpy(pQMaxCntr, &unit_cfg.qMaxCntr, sizeof(rtk_dbg_queue_usedPageCnt_t));

    return RT_ERR_OK;
}

int32 rtrpc_hal_getFlowCtrlSystemPublicResource_get(uint32 unit, uint32 *pCntr)
{
    rtdrv_unitCfg_t unit_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&unit_cfg, 0, sizeof(rtdrv_unitCfg_t));

    unit_cfg.unit = unit;
    GETSOCKOPT(RTDRV_DEBUG_FLOWCTRL_SYSTEM_PUB_RSRC_GET, &unit_cfg, rtdrv_unitCfg_t, 1);
    *pCntr = unit_cfg.cntr;

    return RT_ERR_OK;
}

int32 rtrpc_hal_getFlowCtrlPortQueueUsedPageCnt(uint32 unit, rtk_port_t port, rtk_dbg_queue_usedPageCnt_t *pQCntr, rtk_dbg_queue_usedPageCnt_t *pQMaxCntr)
{
    rtdrv_unitCfg_t unit_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&unit_cfg, 0, sizeof(rtdrv_unitCfg_t));

    unit_cfg.unit = unit;
    unit_cfg.port = port;
    GETSOCKOPT(RTDRV_DEBUG_FLOWCTRL_PORT_QUEUE_USED_PAGE_CNT_GET, &unit_cfg, rtdrv_unitCfg_t, 1);
    osal_memcpy(pQCntr, &unit_cfg.qCntr, sizeof(rtk_dbg_queue_usedPageCnt_t));
    osal_memcpy(pQMaxCntr, &unit_cfg.qMaxCntr, sizeof(rtk_dbg_queue_usedPageCnt_t));

    return RT_ERR_OK;
}

int32 rtrpc_hal_getFlowCtrlRepctQueueCntrInfo(uint32 unit, rtk_dbg_repctQ_CntrInfo_t *pRepctCntr)
{
    rtdrv_unitCfg_t unit_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&unit_cfg, 0, sizeof(rtdrv_unitCfg_t));

    unit_cfg.unit = unit;
    GETSOCKOPT(RTDRV_DEBUG_FLOWCTRL_RPECT_QUEUE_CNTR_GET, &unit_cfg, rtdrv_unitCfg_t, 1);
    osal_memcpy(pRepctCntr, &unit_cfg.repctCntr, sizeof(rtk_dbg_repctQ_CntrInfo_t));

    return RT_ERR_OK;
}

int32 rtrpc_hal_resetFlowCtrlIgrPortUsedPageCnt(uint32 unit, rtk_port_t port)
{
    rtdrv_unitCfg_t unit_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&unit_cfg, 0, sizeof(rtdrv_unitCfg_t));

    unit_cfg.unit = unit;
    unit_cfg.port = port;
    SETSOCKOPT(RTDRV_DEBUG_FLOWCTRL_PORT_USED_PAGE_CNT_INGRESS_RESET, &unit_cfg, rtdrv_unitCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_hal_resetFlowCtrlEgrPortUsedPageCnt(uint32 unit, rtk_port_t port)
{
    rtdrv_unitCfg_t unit_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&unit_cfg, 0, sizeof(rtdrv_unitCfg_t));

    unit_cfg.unit = unit;
    unit_cfg.port = port;
    SETSOCKOPT(RTDRV_DEBUG_FLOWCTRL_PORT_USED_PAGE_CNT_EGRESS_RESET, &unit_cfg, rtdrv_unitCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_hal_resetFlowCtrlSystemUsedPageCnt(uint32 unit)
{
    rtdrv_unitCfg_t unit_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&unit_cfg, 0, sizeof(rtdrv_unitCfg_t));

    unit_cfg.unit = unit;
    SETSOCKOPT(RTDRV_DEBUG_FLOWCTRL_SYSTEM_USED_PAGE_CNT_RESET, &unit_cfg, rtdrv_unitCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_hal_resetFlowCtrlRepctQueueUsedPageCnt(uint32 unit)
{
    rtdrv_unitCfg_t unit_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&unit_cfg, 0, sizeof(rtdrv_unitCfg_t));

    unit_cfg.unit = unit;
    SETSOCKOPT(RTDRV_DEBUG_FLOWCTRL_RPECT_QUEUE_USED_PAGE_CNT_RESET, &unit_cfg, rtdrv_unitCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_hal_repctQueueEmptyStatus_get(uint32 unit, rtk_enable_t *pEnable)
{
    rtdrv_unitCfg_t unit_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&unit_cfg, 0, sizeof(rtdrv_unitCfg_t));

    unit_cfg.unit = unit;
    GETSOCKOPT(RTDRV_DEBUG_REPCTQ_EMPTY_GET, &unit_cfg, rtdrv_unitCfg_t, 1);
    *pEnable = unit_cfg.enable;

    return RT_ERR_OK;
}

int32 rtrpc_hal_repctQueueStickEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    rtdrv_unitCfg_t unit_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&unit_cfg, 0, sizeof(rtdrv_unitCfg_t));

    unit_cfg.unit = unit;
    GETSOCKOPT(RTDRV_DEBUG_REPCTQ_STICK_ENABLE_GET, &unit_cfg, rtdrv_unitCfg_t, 1);
    *pEnable = unit_cfg.enable;

    return RT_ERR_OK;
}

int32 rtrpc_hal_repctQueueStickEnable_set(uint32 unit, rtk_enable_t enable)
{
    rtdrv_unitCfg_t unit_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&unit_cfg, 0, sizeof(rtdrv_unitCfg_t));

    unit_cfg.unit = unit;
    unit_cfg.enable = enable;
    SETSOCKOPT(RTDRV_DEBUG_REPCTQ_STICK_ENABLE_SET, &unit_cfg, rtdrv_unitCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_hal_repctQueueFetchEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    rtdrv_unitCfg_t unit_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&unit_cfg, 0, sizeof(rtdrv_unitCfg_t));

    unit_cfg.unit = unit;
    GETSOCKOPT(RTDRV_DEBUG_REPCTQ_FETCH_ENABLE_GET, &unit_cfg, rtdrv_unitCfg_t, 1);
    *pEnable = unit_cfg.enable;

    return RT_ERR_OK;
}

int32 rtrpc_hal_repctQueueFetchEnable_set(uint32 unit, rtk_enable_t enable)
{
    rtdrv_unitCfg_t unit_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&unit_cfg, 0, sizeof(rtdrv_unitCfg_t));

    unit_cfg.unit = unit;
    unit_cfg.enable = enable;
    SETSOCKOPT(RTDRV_DEBUG_REPCTQ_FETCH_ENABLE_SET, &unit_cfg, rtdrv_unitCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_hal_getWatchdogCnt(uint32 unit, uint32 type, uint32 * count)
{
    rtdrv_unitCfg_t unit_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&unit_cfg, 0, sizeof(rtdrv_unitCfg_t));

    unit_cfg.unit = unit;
    unit_cfg.cntr= type;
    GETSOCKOPT(RTDRV_DEBUG_WATCHDOG_CNT_GET, &unit_cfg, rtdrv_unitCfg_t, 1);
    *count = unit_cfg.data;
    return RT_ERR_OK;
}


/* Function Name:
 *      hal_setWatchdogMonitorEnable
 * Description:
 *      Eanble/Disable watchdog monitor
 * Input:
 *      enable - enable or disable
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 */
int32 hal_setWatchdogMonitorEnable(uint32 enable)
{
    rtdrv_unitCfg_t unit_cfg;
    uint32 unit = RTRPC_UNIT_NOT_USED;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&unit_cfg, 0, sizeof(rtdrv_unitCfg_t));

    unit_cfg.enable = enable;
    SETSOCKOPT(RTDRV_DEBUG_WATCHDOG_MON_ENABLE_SET, &unit_cfg, rtdrv_unitCfg_t, 1);
    return RT_ERR_OK;
}

int32 debug_mem_show(uint32 unit)
{
    rtdrv_regCfg_t reg_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&reg_cfg, 0, sizeof(rtdrv_regCfg_t));

    reg_cfg.unit = unit;
    SETSOCKOPT(RTDRV_DEBUG_MEM_SHOW, &reg_cfg, rtdrv_regCfg_t, 1);

    return RT_ERR_OK;
} /* end of debug_mem_show */


int32 rtrpc_debug_mem_baseAddr_show(uint32 unit)
{
    rtdrv_regCfg_t reg_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&reg_cfg, 0, sizeof(rtdrv_regCfg_t));

    reg_cfg.unit = unit;
    SETSOCKOPT(RTDRV_DEBUG_MEM_BASEADDR_SHOW, &reg_cfg, rtdrv_regCfg_t, 1);

    return RT_ERR_OK;
} /* end of debug_mem_show */



#if defined(CONFIG_SDK_RTL8295R) || defined(CONFIG_SDK_RTL8214QF)
int32
rtrpc_hal_rtl8295_reg_read(uint32 unit, uint32 port, uint32 reg_addr, uint32 *pData)
{
    rtdrv_scRegInfo_t       scRegInfo;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&scRegInfo, 0, sizeof(scRegInfo));
    scRegInfo.unit  = unit;
    scRegInfo.port  = port;
    scRegInfo.addr  = reg_addr;

    GETSOCKOPT(RTDRV_DIAG_SC_REG_READ, &scRegInfo, rtdrv_scRegInfo_t, 1);

    *pData = scRegInfo.data;

    return RT_ERR_OK;
}

int32
rtrpc_hal_rtl8295_reg_write(uint32 unit, rtk_port_t port, uint32 reg_addr, uint32 data)
{
    rtdrv_scRegInfo_t       scRegInfo;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&scRegInfo, 0, sizeof(scRegInfo));
    scRegInfo.unit  = unit;
    scRegInfo.port  = port;
    scRegInfo.addr  = reg_addr;
    scRegInfo.data  = data;

    SETSOCKOPT(RTDRV_DIAG_SC_REG_READ, &scRegInfo, rtdrv_scRegInfo_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_hal_rtl8295_sds_read(uint32 unit, rtk_port_t port, uint32 sds_idx, uint32 page, uint32 reg, uint32 *pData)
{
    rtdrv_scSdsInfo_t       scSdsInfo;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&scSdsInfo, 0, sizeof(scSdsInfo));
    scSdsInfo.unit  = unit;
    scSdsInfo.port  = port;
    scSdsInfo.sds   = sds_idx;
    scSdsInfo.page  = page;
    scSdsInfo.reg   = reg;

    GETSOCKOPT(RTDRV_DIAG_SC_SDS_READ, &scSdsInfo, rtdrv_scSdsInfo_t, 1);

    *pData = scSdsInfo.data;

    return RT_ERR_OK;
}

int32
rtrpc_hal_rtl8295_sds_write(uint32 unit, rtk_port_t port, uint32 sds_idx, uint32 page, uint32 reg, uint32 data)
{
    rtdrv_scSdsInfo_t       scSdsInfo;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&scSdsInfo, 0, sizeof(scSdsInfo));
    scSdsInfo.unit  = unit;
    scSdsInfo.port  = port;
    scSdsInfo.sds   = sds_idx;
    scSdsInfo.page  = page;
    scSdsInfo.reg   = reg;
    scSdsInfo.data  = data;

    SETSOCKOPT(RTDRV_DIAG_SC_SDS_WRITE, &scSdsInfo, rtdrv_scSdsInfo_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_phy_8295_diag_set(uint32 unit, rtk_port_t port, rtk_port_t mdxMacId, uint32 sds, uint8 *name)
{
    rtdrv_scPatch_t     info;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&info, 0, sizeof(rtdrv_scPatch_t));
    info.unit     = unit;
    info.port = port;
    info.mdxMacId = mdxMacId;
    info.sds      = sds;
    strcpy((char *)info.name, (char *)name);

    SETSOCKOPT(RTDRV_DIAG_SC_PATCH, &info, rtdrv_scPatch_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_phy_8295_patch_debugEnable_set(uint32 enable)
{
    rtdrv_scDbg_enable_t        dbg;
    uint32 unit = RTRPC_UNIT_NOT_USED;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&dbg, 0, sizeof(rtdrv_scDbg_enable_t));
    dbg.enable = enable;

    SETSOCKOPT(RTDRV_DIAG_SC_PATCH_DBG, &dbg, rtdrv_scDbg_enable_t, 1);

    return RT_ERR_OK;
}
#endif /* defined(CONFIG_SDK_RTL8295R) || defined(CONFIG_SDK_RTL8214QF) */

#if defined(CONFIG_SDK_RTL8295R)
int32
rtrpc_phy_8295r_rxCaliConfPort_set(uint32 unit, rtk_port_t port, phy_8295_rxCaliConf_t *conf)
{
    rtdrv_8295_rxCaliConf_t    cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&cfg, 0, sizeof(rtdrv_8295_rxCaliConf_t));
    cfg.unit = unit;
    cfg.port = port;
    cfg.dacLongCableOffset         = conf->dacLongCableOffset;
    cfg.s1rxCaliEnable             = conf->s1.rxCaliEnable;
    cfg.s1tap0InitVal              = conf->s1.tap0InitVal;
    cfg.s1vthMinThr                = conf->s1.vthMinThr;
    cfg.s1eqHoldEnable             = conf->s1.eqHoldEnable;
    cfg.s1dfeTap1_4Enable          = conf->s1.dfeTap1_4Enable;
    cfg.s1dfeAuto                  = conf->s1.dfeAuto;
    cfg.s0rxCaliEnable             = conf->s0.rxCaliEnable;
    cfg.s0tap0InitVal              = conf->s0.tap0InitVal;
    cfg.s0vthMinThr                = conf->s0.vthMinThr;
    cfg.s0eqHoldEnable             = conf->s0.eqHoldEnable;
    cfg.s0dfeTap1_4Enable          = conf->s0.dfeTap1_4Enable;
    cfg.s0dfeAuto                  = conf->s0.dfeAuto;

    SETSOCKOPT(RTDRV_DIAG_SC_8295R_RXCALICONF_SET, &cfg, rtdrv_8295_rxCaliConf_t, 1);
    return RT_ERR_OK;
}

int32
rtrpc_phy_8295r_rxCaliConfPort_get(uint32 unit, rtk_port_t port, phy_8295_rxCaliConf_t *conf)
{
    rtdrv_8295_rxCaliConf_t    info;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&info, 0, sizeof(rtdrv_8295_rxCaliConf_t));
    info.unit = unit;
    info.port = port;

    GETSOCKOPT(RTDRV_DIAG_SC_8295R_RXCALICONF_GET, &info, rtdrv_8295_rxCaliConf_t, 1);

    conf->dacLongCableOffset         = info.dacLongCableOffset;
    conf->s1.rxCaliEnable            = info.s1rxCaliEnable;
    conf->s1.tap0InitVal             = info.s1tap0InitVal;
    conf->s1.vthMinThr               = info.s1vthMinThr;
    conf->s1.eqHoldEnable            = info.s1eqHoldEnable;
    conf->s1.dfeTap1_4Enable         = info.s1dfeTap1_4Enable;
    conf->s1.dfeAuto                 = info.s1dfeAuto;
    conf->s0.rxCaliEnable            = info.s0rxCaliEnable;
    conf->s0.tap0InitVal             = info.s0tap0InitVal;
    conf->s0.vthMinThr               = info.s0vthMinThr;
    conf->s0.eqHoldEnable            = info.s0eqHoldEnable;
    conf->s0.dfeTap1_4Enable         = info.s0dfeTap1_4Enable;
    conf->s0.dfeAuto                 = info.s0dfeAuto;

    return RT_ERR_OK;
}
#endif


#if defined(CONFIG_SDK_RTL8214QF)
int32
rtrpc_phy_8214qf_rxCaliConf_set(uint32 unit, rtk_port_t basePort, phy_8295_rxCaliConf_t *conf)
{
    rtdrv_8295_rxCaliConf_t    cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&cfg, 0, sizeof(rtdrv_8295_rxCaliConf_t));
    cfg.unit = unit;
    cfg.port = basePort;
    cfg.dacLongCableOffset         = conf->dacLongCableOffset;
    cfg.s1rxCaliEnable             = conf->s1.rxCaliEnable;
    cfg.s1tap0InitVal              = conf->s1.tap0InitVal;
    cfg.s1vthMinThr                = conf->s1.vthMinThr;
    cfg.s1eqHoldEnable             = conf->s1.eqHoldEnable;
    cfg.s1dfeTap1_4Enable          = conf->s1.dfeTap1_4Enable;
    cfg.s1dfeAuto                  = conf->s1.dfeAuto;
    cfg.s0rxCaliEnable             = conf->s0.rxCaliEnable;
    cfg.s0tap0InitVal              = conf->s0.tap0InitVal;
    cfg.s0vthMinThr                = conf->s0.vthMinThr;
    cfg.s0eqHoldEnable             = conf->s0.eqHoldEnable;
    cfg.s0dfeTap1_4Enable          = conf->s0.dfeTap1_4Enable;
    cfg.s0dfeAuto                  = conf->s0.dfeAuto;

    SETSOCKOPT(RTDRV_DIAG_SC_8214QF_RXCALICONF_SET, &cfg, rtdrv_8295_rxCaliConf_t, 1);
    return RT_ERR_OK;
}

int32
rtrpc_phy_8214qf_rxCaliConf_get(uint32 unit, rtk_port_t basePort, phy_8295_rxCaliConf_t *conf)
{
    rtdrv_8295_rxCaliConf_t    info;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&info, 0, sizeof(rtdrv_8295_rxCaliConf_t));
    info.unit = unit;
    info.port = basePort;

    GETSOCKOPT(RTDRV_DIAG_SC_8214QF_RXCALICONF_GET, &info, rtdrv_8295_rxCaliConf_t, 1);

    conf->dacLongCableOffset         = info.dacLongCableOffset;
    conf->s1.rxCaliEnable            = info.s1rxCaliEnable;
    conf->s1.tap0InitVal             = info.s1tap0InitVal;
    conf->s1.vthMinThr               = info.s1vthMinThr;
    conf->s1.eqHoldEnable            = info.s1eqHoldEnable;
    conf->s1.dfeTap1_4Enable         = info.s1dfeTap1_4Enable;
    conf->s1.dfeAuto                 = info.s1dfeAuto;
    conf->s0.rxCaliEnable            = info.s0rxCaliEnable;
    conf->s0.tap0InitVal             = info.s0tap0InitVal;
    conf->s0.vthMinThr               = info.s0vthMinThr;
    conf->s0.eqHoldEnable            = info.s0eqHoldEnable;
    conf->s0.dfeTap1_4Enable         = info.s0dfeTap1_4Enable;
    conf->s0.dfeAuto                 = info.s0dfeAuto;

    return RT_ERR_OK;
}
#endif


int32
rtrpc_phy_8390_10gMedia_set(uint32 unit, rtk_port_t port, rtk_port_10gMedia_t media)
{
    rtdrv_portCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&cfg, 0, sizeof(rtdrv_portCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&cfg.media_10g, &media, sizeof(rtk_port_10gMedia_t));
    SETSOCKOPT(RTDRV_DIAG_SC_8390_10GMEDIA_SET, &cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_mac_debug_sds_rxCaliEnable_get(uint32 unit, uint32 sds, uint32 *pEnable)
{
    rtdrv_sdsCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&cfg, 0, sizeof(rtdrv_sdsCfg_t));

    cfg.unit = unit;
    cfg.sds = sds;
    GETSOCKOPT(RTDRV_DEBUG_SDS_RXCALI_ENABLE_GET, &cfg, rtdrv_sdsCfg_t, 1);
    *pEnable = cfg.en;

    return RT_ERR_OK;
}   /* end of mac_debug_sds_rxCaliEnable_get */

int32
rtrpc_mac_debug_sds_rxCaliEnable_set(uint32 unit, uint32 sds, rtk_enable_t enable)
{
    rtdrv_sdsCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&cfg, 0, sizeof(rtdrv_sdsCfg_t));

    cfg.unit = unit;
    cfg.sds = sds;
    cfg.en = enable;
    SETSOCKOPT(RTDRV_DEBUG_SDS_RXCALI_ENABLE_SET, &cfg, rtdrv_sdsCfg_t, 1);

    return RT_ERR_OK;
}   /* end of mac_debug_sds_rxCaliEnable_get */

int32
rtrpc_mac_debug_sds_rxCaliStatus_get(uint32 unit, uint32 sds, rtk_port_phySdsRxCaliStatus_t *pStatus)
{
    rtdrv_sdsCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&cfg, 0, sizeof(rtdrv_sdsCfg_t));

    cfg.unit = unit;
    cfg.sds = sds;
    GETSOCKOPT(RTDRV_DEBUG_SDS_RXCALI_STATUS_GET, &cfg, rtdrv_sdsCfg_t, 1);
    *pStatus = cfg.rxCaliStatus;

    return RT_ERR_OK;
}   /* end of mac_debug_sds_rxCaliStatus_get */

int32
rtrpc_mac_debug_sds_rxCali(uint32 unit, uint32 sds, uint32 retryCnt)
{
    rtdrv_sdsCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&cfg, 0, sizeof(rtdrv_sdsCfg_t));

    cfg.unit = unit;
    cfg.sds = sds;
    cfg.cnt = retryCnt;
    SETSOCKOPT(RTDRV_DEBUG_SDS_RXCALI_START, &cfg, rtdrv_sdsCfg_t, 1);

    return RT_ERR_OK;
}   /* end of mac_debug_sds_rxCali */

int32
rtrpc_mac_debug_sds_rxCali_debugEnable_set(rtk_enable_t enable)
{
    rtdrv_sdsCfg_t cfg;
    uint32 unit = RTRPC_UNIT_NOT_USED;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&cfg, 0, sizeof(rtdrv_sdsCfg_t));

    cfg.en = enable;
    SETSOCKOPT(RTDRV_DEBUG_SDS_RXCALI_DEBUG_ENABLE_SET, &cfg, rtdrv_sdsCfg_t, 1);

    return RT_ERR_OK;
}   /* end of mac_debug_sds_rxCali_debugEnable_set */



int32
rtrpc__dal_phy_debug_cmd(uint32 unit, char *cmd_str, rtk_portmask_t *portmask, uint32 param1, uint32 param2, uint32 param3, uint32 param4, uint32 param5)
{
    rtdrv_phyDebugCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&cfg, 0, sizeof(rtdrv_phyDebugCfg_t));

    cfg.unit = unit;
    strcpy((char *)cfg.cmd_str, (char *)cmd_str);
    cfg.portmask = *portmask;
    cfg.param1 = param1;
    cfg.param2 = param2;
    cfg.param3 = param3;
    cfg.param4 = param4;
    cfg.param5 = param5;
    GETSOCKOPT(RTDRV_DEBUG_PHY_CMD_GET, &cfg, rtdrv_phyDebugCfg_t, 1);

    return RT_ERR_OK;
}
#endif //#ifdef RTUSR


