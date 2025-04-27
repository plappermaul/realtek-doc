/*
 * Copyright(c) Realtek Semiconductor Corporation, 2008
 * All rights reserved.
 *
 * $Revision: 75072 $
 * $Date: 2017-01-05 20:13:38 +0800 (Thu, 05 Jan 2017) $
 *
 * Purpose : Realtek Switch SDK rtrpc API Module
 *
 * Feature : The file have include the following module and sub-modules
 *           1) l2 address
 *
 */

#include <dal/rtrpc/rtrpc_sys.h>
#include <dal/rtrpc/rtrpc_msg.h>
#include <rtdrv/rtdrv_netfilter.h>



int32
hwp_info_show(uint32 unit)
{
    rtdrv_sysCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_sysCfg_t));
    config.unit = unit;
    GETSOCKOPT(RTDRV_SYS_HWP_DUMP_INFO, &config, rtdrv_sysCfg_t, 1);

    return RT_ERR_OK;
}

int32
hwp_parsedInfo_show(uint32 unit)
{
    rtdrv_sysCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_sysCfg_t));
    config.unit = unit;
    GETSOCKOPT(RTDRV_SYS_HWP_DUMP_PARSED, &config, rtdrv_sysCfg_t, 1);

    return RT_ERR_OK;
}

int32
hwp_unit_show(uint32 unit)
{
    rtdrv_sysCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_sysCfg_t));
    config.unit = unit;
    GETSOCKOPT(RTDRV_SYS_HWP_DUMP_UNIT, &config, rtdrv_sysCfg_t, 1);

    return RT_ERR_OK;
}

#ifdef RTUSR
int32
rise_show_dc(uint32 unit)
{
    rtdrv_sysCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_sysCfg_t));
    config.unit = unit;
    GETSOCKOPT(RTDRV_SYS_RTSTK_DC_SHOW, &config, rtdrv_sysCfg_t, 1);

    return RT_ERR_OK;
}

int32
rise_show_dcbox(int indent, void *box, void *uplink_PortConn)
{
    rtdrv_sysCfg_t config;
    uint32 unit = RTRPC_UNIT_NOT_USED;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_sysCfg_t));
    config.unit = 0;
    GETSOCKOPT(RTDRV_SYS_RTSTK_DCBOX_SHOW, &config, rtdrv_sysCfg_t, 1);

    return RT_ERR_OK;
}

int32
rise_show_ta(void *db)
{
    rtdrv_sysCfg_t config;
    uint32 unit = RTRPC_UNIT_NOT_USED;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_sysCfg_t));
    GETSOCKOPT(RTDRV_SYS_RTSTK_TA_SHOW, &config, rtdrv_sysCfg_t, 1);

    return RT_ERR_OK;
}

int32
rise_show_cfg(uint32 unit)
{
    rtdrv_sysCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_sysCfg_t));
    config.unit = unit;
    GETSOCKOPT(RTDRV_SYS_RTSTK_CFG_SHOW, &config, rtdrv_sysCfg_t, 1);

    return RT_ERR_OK;
}

int32
rise_show_port(uint32 unit)
{
    rtdrv_sysCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_sysCfg_t));
    config.unit = unit;
    GETSOCKOPT(RTDRV_SYS_RTSTK_PORT_SHOW, &config, rtdrv_sysCfg_t, 1);

    return RT_ERR_OK;
}

int32
rise_show_cmd(uint32 unit, char *cmd_str, int cmd_int)
{
    rtdrv_sysCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_sysCfg_t));
    config.unit = unit;
    strcpy((char *)config.cmd_str,(char *)cmd_str);
    config.cmd_int = cmd_int;
    GETSOCKOPT(RTDRV_SYS_RTSTK_CMD_SHOW, &config, rtdrv_sysCfg_t, 1);

    return RT_ERR_OK;
}

int32
rise_application_kickoff(uint32 *boxID_list)
{
    rtdrv_sysCfg_t config;
    uint32 unit=0;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_sysCfg_t));
    GETSOCKOPT(RTDRV_SYS_RISE_KICKOFF_WAIT, &config, rtdrv_sysCfg_t, 1);
    *boxID_list = config.tmp;

    return RT_ERR_OK;
}

#endif//RTUSR

