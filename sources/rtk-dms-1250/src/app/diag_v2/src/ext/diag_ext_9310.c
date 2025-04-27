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
 * Purpose : Definition those extension command and APIs in the SDK diagnostic shell.
 *
 */

/*
 * Include Files
 */


#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <common/debug/rt_log.h>
#include <common/debug/mem.h>
#include <common/rt_error.h>
#include <common/rt_type.h>
#include <dal/rtrpc/rtrpc_msg.h>
#include <diag_util.h>
#include <diag_om.h>
#include <diag_str.h>
#include <private/drv/swcore/swcore_rtl9310.h>
#include <parser/cparser_priv.h>
#include <rtdrv/ext/rtdrv_netfilter_ext_9310.h>
#include <hal/common/halctrl.h>
#include <hal/chipdef/mango/rtk_mango_reg_struct.h>
#include <hal/mac/reg.h>
#include <ioal/mem32.h>
#ifdef CONFIG_SDK_MODEL_MODE
#include <virtualmac/vmac_target.h>
#endif

#ifdef CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE
  #include <rtrpc/rtrpc_qos.h>
  #include <rtrpc/rtrpc_trap.h>
  #include <rtrpc/rtrpc_pie.h>
  #include <rtrpc/rtrpc_acl.h>
  #include <rtrpc/rtrpc_eee.h>
  #include <rtrpc/rtrpc_switch.h>
  #include <rtrpc/rtrpc_port.h>
  #include <rtrpc/rtrpc_vlan.h>
  #include <rtrpc/rtrpc_diag.h>
  #include <rtrpc/rtrpc_l2.h>
  #include <rtrpc/rtrpc_l3.h>
#endif


/*
 * Macro Definition
 */


/* FPGA */
#ifdef CONFIG_SDK_FPGA_PLATFORM
extern int32 fpga_info_get(uint32 unit, uint32 *pRtl, uint32 *pDate, uint32 *pTime, uint32 *pVersion);
extern int32 fpga_init(uint32 unit, uint32 fpgaVer);
extern int32 fpga_test(uint32 unit);
#endif
extern int32 testIo_set_ocp(uint32 unit, uint32 addr, uint32 val);
extern int32 testIo_get_ocp(uint32 unit, uint32 addr, uint32 *pVal);
extern int32 testIo_set_eFuse_check(uint32 unit);
extern int32 testIo_set_ePhy(uint32 unit, uint32 addr, uint32 val);
extern int32 testIo_get_ePhy(uint32 unit, uint32 addr, uint32 *pVal);
extern int32 testIo_set_eFuse(uint32 unit, uint32 index, uint32 addr, uint32 val);
extern int32 testIo_get_eFuse(uint32 unit, uint32 index, uint32 *pAddr, uint32 *pVal);

/*
 * testio set ocp <UINT:address> <UINT:value>
 */
cparser_result_t cparser_cmd_testio_set_ocp_address_value(cparser_context_t *context,
    uint32_t *address_ptr, uint32_t *value_ptr)
{
    uint32  unit = 0;
    uint32 master_view_unit;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    DIAG_UTIL_ERR_CHK(testIo_set_ocp(unit, *address_ptr, *value_ptr), ret);
    return CPARSER_OK;
}

/*
 * testio get ocp <UINT:address>
 */
cparser_result_t cparser_cmd_testio_get_ocp_address(cparser_context_t *context,
    uint32_t *address_ptr)
{
    uint32  unit = 0;
    uint32 master_view_unit;
    uint32  reg = 0;
    uint32  value  = 0;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    reg = *address_ptr;

    DIAG_UTIL_ERR_CHK(testIo_get_ocp(unit, reg, &value), ret);
    diag_util_mprintf("TestIO OCP %#x : %#x\n", reg, value);
    return CPARSER_OK;
}

/*
 * testio set ephy <UINT:address> <UINT:value>
 */
cparser_result_t cparser_cmd_testio_set_ephy_address_value(cparser_context_t *context,
    uint32_t *address_ptr, uint32_t *value_ptr)
{
    uint32  unit = 0;
    uint32 master_view_unit;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    DIAG_UTIL_ERR_CHK(testIo_set_ePhy(unit, *address_ptr, *value_ptr), ret);
    return CPARSER_OK;
}

/*
 * testio get ephy <UINT:address>
 */
cparser_result_t cparser_cmd_testio_get_ephy_address(cparser_context_t *context,
    uint32_t *address_ptr)
{
    uint32  unit = 0;
    uint32 master_view_unit;
    uint32  reg = 0;
    uint32  value  = 0;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    reg = *address_ptr;

    DIAG_UTIL_ERR_CHK(testIo_get_ePhy(unit, reg, &value), ret);
    diag_util_mprintf("TestIO EPhy %#x : %#x\n", reg, value);
    return CPARSER_OK;
}

/*
 * testio set efuse check
 */
cparser_result_t cparser_cmd_testio_set_efuse_check(cparser_context_t *context)
{
    uint32  unit = 0;
    uint32 master_view_unit;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    DIAG_UTIL_ERR_CHK(testIo_set_eFuse_check(unit), ret);
    return CPARSER_OK;
}

/*
 * testio set efuse entry <UINT:index> ephy <UINT:address> <UINT:value>
 */
cparser_result_t cparser_cmd_testio_set_efuse_entry_index_ephy_address_value(cparser_context_t *context,
    uint32_t *index_ptr, uint32_t *address_ptr, uint32_t *value_ptr)
{
    uint32  unit = 0;
    uint32 master_view_unit;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    DIAG_UTIL_ERR_CHK(testIo_set_eFuse(unit, *index_ptr, *address_ptr, *value_ptr), ret);
    return CPARSER_OK;
}

/*
 * testio get efuse entry <UINT:index>
 */
cparser_result_t cparser_cmd_testio_get_efuse_entry_index(cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32  unit = 0;
    uint32 master_view_unit;
    uint32  reg = 0;
    uint32  value  = 0;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);


    DIAG_UTIL_ERR_CHK(testIo_get_eFuse(unit, *index_ptr, &reg, &value), ret);
    diag_util_mprintf("TestIO %#x : %#x\n", reg, value);
    return CPARSER_OK;
}

/*
 * nic reset system
 */
cparser_result_t cparser_cmd_nic_reset_system(cparser_context_t *context)
{
    uint32  unit = 0;
    uint32 master_view_unit;
    rtdrv_ext_nicCfg_t cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&cfg, 0x0, sizeof(cfg));
    cfg.unit = unit;

    SETSOCKOPT(RTDRV_EXT_NIC_RESET, &cfg, rtdrv_ext_nicCfg_t, 1);
    diag_util_mprintf("NIC reset complete\n");
    return CPARSER_OK;
}


/*
 * bandwidth get egress cpu-tag
 */
cparser_result_t cparser_cmd_bandwidth_get_egress_cpu_tag(cparser_context_t *context)
{
    uint32 unit;
    uint32 master_view_unit;
    rtdrv_ext_switchCfg_t switch_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    switch_cfg.unit = unit;
    GETSOCKOPT(RTDRV_EXT_RATE_EGR_INCLUDE_CPU_TAG_GET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);

    if (ENABLED == switch_cfg.enable)
    {
        diag_util_mprintf("CPU Port Egress Bandwidth : Include CPU Tag\n");
    }
    else
    {
        diag_util_mprintf("CPU Port Egress Bandwidth : Exclude CPU Tag\n");
    }

    return CPARSER_OK;
}

/*
 * bandwidth set egress cpu-tag ( exclude | include )
 */
cparser_result_t cparser_cmd_bandwidth_set_egress_cpu_tag_exclude_include(cparser_context_t *context)
{
    uint32 unit;
    uint32 master_view_unit;
    rtk_enable_t enable = DISABLED;
    rtdrv_ext_switchCfg_t switch_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    switch(TOKEN_CHAR(4,0))
    {
        case 'e':
            enable = DISABLED;
            break;

        case 'i':
            enable = ENABLED;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    switch_cfg.unit = unit;
    switch_cfg.enable = enable;
    SETSOCKOPT(RTDRV_EXT_RATE_EGR_INCLUDE_CPU_TAG_SET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);

    return CPARSER_OK;
}


/*
 *  sdk reset tc
 */
cparser_result_t cparser_cmd_sdk_reset_tc(cparser_context_t *context)
{
    uint32  unit = 0;
    uint32 master_view_unit;
    rtdrv_ext_sdkCfg_t cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    cfg.unit = unit;
    SETSOCKOPT(RTDRV_EXT_SDK_TC_RESET, &cfg, rtdrv_ext_sdkCfg_t, 1);

    return CPARSER_OK;
}

/*
 *  fpga get info
 */
cparser_result_t cparser_cmd_fpga_get_info(cparser_context_t *context)
{
#ifdef CONFIG_SDK_FPGA_PLATFORM
    int32   ret;
    uint32  unit = 0;
    uint32 master_view_unit;
    uint32  rtl_svn_rev;
    uint32  build_date;
    uint32  build_time;
    uint32  fpga_type_and_reg_profile_ver;
    uint32  reg_profile_ver;
    uint32  fpga_type;

    DIAG_UTIL_FUNC_INIT(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    DIAG_UTIL_ERR_CHK(fpga_info_get(unit, &rtl_svn_rev, &build_date, &build_time, &fpga_type_and_reg_profile_ver), ret);

    reg_profile_ver = (fpga_type_and_reg_profile_ver & 0x0000FFFF) >> 0;
    fpga_type = (fpga_type_and_reg_profile_ver & 0xFFFF0000) >> 16;

    diag_util_mprintf("FPGA build time : %04X-%02X/%02X %02X:%02X\n", \
        (build_date & 0xFFFF0000) >> 16,
        (build_date & 0x0000FF00) >> 8,
        (build_date & 0x000000FF) >> 0,
        (build_time & 0x0000FF00) >> 8,
        (build_time & 0x000000FF) >> 0);
    diag_util_mprintf("RTL svn revision : %X\n", rtl_svn_rev);
    /* patch for RTL r13586 FPGA (reg-profile is incorrect) */ if (rtl_svn_rev == 0x13586) reg_profile_ver = 0x622;
    diag_util_mprintf("Reg profile ver. : %X\n", reg_profile_ver);
    diag_util_mprintf("FPGA build type : ");
    switch(fpga_type)
    {
        case 0x2:
            diag_util_mprintf("L2\n");
            break;
        case 0x3:
            diag_util_mprintf("L3\n");
            break;
        case 0xFE:
            diag_util_mprintf("FE\n");
            break;
        default:
            diag_util_mprintf("Unknown (0x%02X)\n", fpga_type);
            break;
    }

    return CPARSER_OK;
#else
    return CPARSER_OK;
#endif
}

/*
 *  fpga init { fe } { fe325 } { cpu-enable } { cpu-disable }
 */
cparser_result_t cparser_cmd_fpga_init_fe_fe325_cpu_enable_cpu_disable(cparser_context_t *context)
{
#ifdef CONFIG_SDK_FPGA_PLATFORM
    uint32  unit = 0;
    uint32 master_view_unit;

    DIAG_UTIL_FUNC_INIT(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* fpga version:
     * 0 - L2/L3
     * 1 - FE
     * 2 - FE (325 MHz)
     * 9 - CPU simulation
     */
    if (TOKEN_NUM >= 3)
    {
        if ('c' == TOKEN_CHAR(2, 0))
        {
            /* Port 52 (CPU port simulation) */

            if ('e' == TOKEN_CHAR(2, 4))
                fpga_init(unit, 9); /* enable */
            else
                fpga_init(unit, 10); /* disable */
        }
        else if ('3' == TOKEN_CHAR(2, 2))
            fpga_init(unit, 2); /* FE 325 MHz */
        else
            fpga_init(unit, 1); /* FE */
    }
    else
        fpga_init(unit, 0); /* L2/L3 */

    return CPARSER_OK;
#else
    return CPARSER_OK;
#endif
}

/*
 *  fpga test
 */
cparser_result_t cparser_cmd_fpga_test(cparser_context_t *context)
{
#ifdef CONFIG_SDK_FPGA_PLATFORM
    uint32  unit = 0;
    uint32 master_view_unit;

    DIAG_UTIL_FUNC_INIT(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    fpga_test(unit);

    return CPARSER_OK;
#else
    return CPARSER_OK;
#endif
}


/* support ONE 32-bit register only */
static int32 _reg_read(uint32 unit, uint32 reg, uint32 *pData)
{
    rtdrv_ext_diagCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    if (pData == NULL)
        return -1;

    osal_memset(&cfg, 0x00, sizeof(rtdrv_ext_diagCfg_t));
    cfg.unit = unit;
    cfg.reg  = reg;
    cfg.idx1 = REG_ARRAY_INDEX_NONE;
    cfg.idx2 = REG_ARRAY_INDEX_NONE;
    GETSOCKOPT(RTDRV_EXT_DIAG_REGARRAY_GET, &cfg, rtdrv_ext_diagCfg_t, 1);

    *pData = cfg.data[0];

    return 0;
}

/* support ONE 32-bit register only */
static int32 _reg_write(uint32 unit, uint32 reg, uint32 *pData)
{
    rtdrv_ext_diagCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    if (pData == NULL)
        return -1;

    osal_memset(&cfg, 0x00, sizeof(rtdrv_ext_diagCfg_t));
    cfg.unit = unit;
    cfg.reg  = reg;
    cfg.idx1 = REG_ARRAY_INDEX_NONE;
    cfg.idx2 = REG_ARRAY_INDEX_NONE;
    cfg.data[0] = *pData;
    SETSOCKOPT(RTDRV_EXT_DIAG_REGARRAY_SET, &cfg, rtdrv_ext_diagCfg_t, 1);

    return 0;
}


static uint32 _bist_reg_ret(uint32 unit, uint32 reg, uint32 msb, uint32 lsb)
{
    uint32  reg_data;
    uint32  reg_mask = 0;
    int32   i;

    _reg_read(unit, reg, &reg_data);

    for (i=lsb; i<=msb; i++)
    {
        reg_mask |= (1 << i);
    }

    return ((reg_data & reg_mask) >> lsb);
}

static int32 _bist_reg_update(uint32 unit, uint32 reg, uint32 msb, uint32 lsb, uint32 val)
{
    uint32  reg_data;
    uint32  reg_mask = 0;
    uint32  reg_value;
    int32   i;

    _reg_read(unit, reg, &reg_data);

    for (i=lsb; i<=msb; i++)
    {
        reg_mask |= (1 << i);
    }
    reg_value = ((val) << (lsb)) & (reg_mask);

    /* update reg data */
    reg_data = ((reg_data) & ~(reg_mask)) | (reg_value);

    _reg_write(unit, reg, &reg_data);

    return 0;
}

static int32 _bist_reg_check(uint32 unit, uint32 reg, uint32 msb, uint32 lsb, uint32 val, char *pStr)
{
    uint32  reg_data;
    uint32  reg_mask = 0;
    uint32  reg_value;
    int32   i;

    _reg_read(unit, reg, &reg_data);

    for (i=lsb; i<=msb; i++)
    {
        reg_mask |= (1 << i);
    }
    reg_value = ((val) << (lsb)) & (reg_mask);

    if ((reg_data & reg_mask) != (reg_value))
    {
#if 0
        osal_printf("Reg %s [%d:%d] = 0x%X (!= 0x%X)\n", pStr, msb, lsb, (reg_data & reg_mask) >> lsb, val);
#endif
        return -1;
    }

    return 0;
}

static int32 _bist_mdelay(uint32 msec)
{
    return usleep(msec * 1000);
}

#if 1
#define _BIST_PRINTF    osal_printf
#else
#define _BIST_PRINTF    DIAG_UTIL_MPRINTF
#endif

#define _BIST_REG_RET(_unit, _regName, _msb, _lsb)              (_bist_reg_ret((_unit), MANGO_##_regName##r, (_msb), (_lsb)))
#define _BIST_REG_UPD(_unit, _regName, _msb, _lsb, _updValue)   (_bist_reg_update((_unit), MANGO_##_regName##r, (_msb), (_lsb), (_updValue)))
#define _BIST_REG_CHK(_unit, _regName, _msb, _lsb, _chkValue)   (_bist_reg_check((_unit), MANGO_##_regName##r, (_msb), (_lsb), (_chkValue), #_regName))
#define _BIST_REG_ERR(_unit, _regName, _msb, _lsb, _chkValue)                       \
    do {                                                                            \
        if (_BIST_REG_CHK(_unit, _regName, _msb, _lsb, _chkValue))                  \
            _BIST_PRINTF("Reg %s[%d:%d] = 0x%X\n", #_regName, _msb, _lsb,           \
                (_bist_reg_ret((_unit), MANGO_##_regName##r, (_msb), (_lsb))));     \
    } while (0)


/*
 * diag test mbist
 */
cparser_result_t cparser_cmd_diag_test_mbist(cparser_context_t *context)
{
#if 0
    uint32 unit;
    uint32 master_view_unit;

    DIAG_UTIL_FUNC_INIT(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    //osal_printf("MODEL_NAME_INFO = 0x%08X\n", _BIST_REG_RET(unit, MODEL_NAME_INFO, 31, 0));

    _BIST_PRINTF("==============MAC_MBIST===========================\n");

    _BIST_PRINTF("==============MBIST_PG0 & PG1=====================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        _BIST_REG_UPD(unit, MBIST_MAC_RESET_00, 31, 0, 0x16b);
        _bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_PG00_RX_MAC01, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG00_RX_MAC05, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG00_TX_MAC01, 31, 0, 0x20000000)))
        {
            _BIST_PRINTF("MBIST_PG0 Fail\n");
        }

        if ((_BIST_REG_CHK(unit, MBIST_PG01_RX_MAC01, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG01_RX_MAC05, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG01_TX_MAC01, 31, 0, 0x20000000)))
        {
            _BIST_PRINTF("MBIST_PG1 Fail\n");
        }

        _BIST_REG_UPD(unit, MBIST_MAC_RESET_00, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_PG2 & PG3=====================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        _BIST_REG_UPD(unit, MBIST_MAC_RESET_00, 31, 0, 0x5ac00);
        _bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_PG02_RX_MAC01, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG02_RX_MAC05, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG02_TX_MAC01, 31, 0, 0x20000000)))
        {
            _BIST_PRINTF("MBIST_PG2 Fail\n");
        }

        if ((_BIST_REG_CHK(unit, MBIST_PG03_RX_MAC01, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG03_RX_MAC05, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG03_TX_MAC01, 31, 0, 0x20000000)))
        {
            _BIST_PRINTF("MBIST_PG3 Fail\n");
        }

        _BIST_REG_UPD(unit, MBIST_MAC_RESET_00, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_PG4 & PG5=====================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        _BIST_REG_UPD(unit, MBIST_MAC_RESET_00, 31, 0, 0x16b00000);
        _bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_PG04_RX_MAC01, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG04_RX_MAC05, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG04_TX_MAC01, 31, 0, 0x20000000)))
        {
            _BIST_PRINTF("MBIST_PG4 Fail\n");
        }

        if ((_BIST_REG_CHK(unit, MBIST_PG05_RX_MAC01, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG05_RX_MAC05, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG05_TX_MAC01, 31, 0, 0x20000000)))
        {
            _BIST_PRINTF("MBIST_PG5 Fail\n");
        }

        _BIST_REG_UPD(unit, MBIST_MAC_RESET_00, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_PG6===========================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        _BIST_REG_UPD(unit, MBIST_MAC_RESET_01, 31, 0, 0x67);
        _bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_PG06_RX_MAC08, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG06_RX_MAC04, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG06_RX_MAC06, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG06_TX_MAC08, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG06_TX_MAC02, 31, 0, 0x20000000)))
        {
            _BIST_PRINTF("MBIST_PG6 Fail\n");
        }

        _BIST_REG_UPD(unit, MBIST_MAC_RESET_01, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_PG7===========================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        _BIST_REG_UPD(unit, MBIST_MAC_RESET_01, 31, 0, 0x3380);
        _bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_PG07_RX_MAC08, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG07_RX_MAC04, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG07_RX_MAC06, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG07_TX_MAC08, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG07_TX_MAC02, 31, 0, 0x20000000)))
        {
            _BIST_PRINTF("MBIST_PG7 Fail\n");
        }

        _BIST_REG_UPD(unit, MBIST_MAC_RESET_01, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_PG8===========================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        _BIST_REG_UPD(unit, MBIST_MAC_RESET_02, 31, 0, 0x37);
        _bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_PG08_RX_MAC10, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG08_RX_MAC07, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG08_RX_MAC11, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG08_TX_MAC10, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG08_TX_MAC12, 31, 0, 0x20000000)))
        {
            _BIST_PRINTF("MBIST_PG8 Fail\n");
        }

        _BIST_REG_UPD(unit, MBIST_MAC_RESET_02, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_PG9===========================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        _BIST_REG_UPD(unit, MBIST_MAC_RESET_02, 31, 0, 0xdc0);
        _bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_PG09_RX_MAC10, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG09_RX_MAC07, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG09_RX_MAC11, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG09_TX_MAC10, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG09_TX_MAC12, 31, 0, 0x20000000)))
        {
            _BIST_PRINTF("MBIST_PG9 Fail\n");
        }

        _BIST_REG_UPD(unit, MBIST_MAC_RESET_02, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_PG10===========================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        _BIST_REG_UPD(unit, MBIST_MAC_RESET_02, 31, 0, 0x37000);
        _bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_PG10_RX_MAC10, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG10_RX_MAC07, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG10_RX_MAC11, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG10_TX_MAC10, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG10_TX_MAC12, 31, 0, 0x20000000)))
        {
            _BIST_PRINTF("MBIST_PG10 Fail\n");
        }

        _BIST_REG_UPD(unit, MBIST_MAC_RESET_02, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_PG11===========================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        _BIST_REG_UPD(unit, MBIST_MAC_RESET_02, 31, 0, 0xdc0000);
        _bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_PG11_RX_MAC10, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG11_RX_MAC07, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG11_RX_MAC11, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG11_TX_MAC10, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG11_TX_MAC12, 31, 0, 0x20000000)))
        {
            _BIST_PRINTF("MBIST_PG11 Fail\n");
        }

        _BIST_REG_UPD(unit, MBIST_MAC_RESET_02, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_PG12===========================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        _BIST_REG_UPD(unit, MBIST_MAC_RESET_01, 31, 0, 0x39c000);
        _bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_PG12_RX_MAC03, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG12_RX_MAC09, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG12_RX_MAC07, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG12_TX_MAC02, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG12_PG_MAC13, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG12_PG_MAC14, 31, 0, 0x20000000)))
        {
            _BIST_PRINTF("MBIST_PG12 Fail\n");
        }

        _BIST_REG_UPD(unit, MBIST_MAC_RESET_01, 31, 0, 0x0);
    }

    /*
     * ALE
     */

    _BIST_PRINTF("==============ALE_MBIST===========================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        _BIST_REG_UPD(unit, MBIST_ALE_START_0, 31, 0, 0xff);
        _bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_ALE01, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ALE02, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ALE03, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ALE04, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ALE05, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ALE06, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ALE07, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ALE08, 31, 0, 0x20000000)))
        {
            _BIST_REG_ERR(unit, MBIST_ALE01, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ALE02, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ALE03, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ALE04, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ALE05, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ALE06, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ALE07, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ALE08, 31, 0, 0x20000000);
            _BIST_PRINTF("MBIST_ALE01~08 have Fail\n");
        }

        _BIST_REG_UPD(unit, MBIST_ALE_START_0, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_MALE9~16======================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        _BIST_REG_UPD(unit, MBIST_ALE_START_0, 31, 0, 0xff00);
        _bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_ALE09, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ALE10, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ALE011, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ALE012, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ALE13, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ALE14, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ALE15, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ALE16, 31, 0, 0x20000000)))
        {
            _BIST_REG_ERR(unit, MBIST_ALE09, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ALE10, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ALE011, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ALE012, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ALE13, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ALE14, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ALE15, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ALE16, 31, 0, 0x20000000);
            _BIST_PRINTF("MBIST_ALE09~16 have Fail\n");
        }

        _BIST_REG_UPD(unit, MBIST_ALE_START_0, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_MALE17~24======================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        _BIST_REG_UPD(unit, MBIST_ALE_START_0, 31, 0, 0xff0000);
        _bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_ALE17, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ALE18, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ALE19, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ALE20, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ALE21, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ALE22, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ALE23, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ALE24, 31, 0, 0x20000000)))
        {
            _BIST_REG_ERR(unit, MBIST_ALE17, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ALE18, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ALE19, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ALE20, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ALE21, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ALE22, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ALE23, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ALE24, 31, 0, 0x20000000);
            _BIST_PRINTF("MBIST_ALE17~24 have Fail\n");
        }

        _BIST_REG_UPD(unit, MBIST_ALE_START_0, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_MALE25~32======================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        _BIST_REG_UPD(unit, MBIST_ALE_START_0, 31, 0, 0xff000000);
        _bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_ALE25, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ALE26, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ALE27, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ALE28, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ALE29, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ALE30, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ALE31, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ALE32, 31, 0, 0x20000000)))
        {
            _BIST_REG_ERR(unit, MBIST_ALE25, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ALE26, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ALE27, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ALE28, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ALE29, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ALE30, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ALE31, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ALE32, 31, 0, 0x20000000);
            _BIST_PRINTF("MBIST_ALE25~32 have Fail\n");
        }

        _BIST_REG_UPD(unit, MBIST_ALE_START_0, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_MALE33~39======================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        _BIST_REG_UPD(unit, MBIST_ALE_START_1, 31, 0, 0x7f);
        _bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_ALE33, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ALE34, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ALE35, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ALE36, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ALE37, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ALE38, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ALE39, 31, 0, 0x20000000)))
        {
            _BIST_REG_ERR(unit, MBIST_ALE33, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ALE34, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ALE35, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ALE36, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ALE37, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ALE38, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ALE39, 31, 0, 0x20000000);
            _BIST_PRINTF("MBIST_ALE33~39 have Fail\n");
        }

        _BIST_REG_UPD(unit, MBIST_ALE_START_1, 31, 0, 0x0);
    }

    /*
     * ACL
     */

    _BIST_PRINTF("==============ACL_MBIST======================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        _BIST_REG_UPD(unit, MBIST_ACL_START, 31, 0, 0xff);
        _bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_TCM00_ACL1, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_TCM01_ACL1, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_TCM02_ACL1, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_TCM03_ACL1, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_TCM04_ACL1, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_TCM05_ACL1, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_TCM06_ACL1, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_TCM07_ACL1, 31, 0, 0x20000000)))
        {
            _BIST_REG_ERR(unit, MBIST_TCM00_ACL1, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_TCM01_ACL1, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_TCM02_ACL1, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_TCM03_ACL1, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_TCM04_ACL1, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_TCM05_ACL1, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_TCM06_ACL1, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_TCM07_ACL1, 31, 0, 0x20000000);
            _BIST_PRINTF("MBIST_TCM00~07_ACL1 have Fail\n");
        }

        _BIST_REG_UPD(unit, MBIST_ACL_START, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MACL1_TCM08~15======================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        _BIST_REG_UPD(unit, MBIST_ACL_START, 31, 0, 0xff00);
        _bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_TCM08_ACL1, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_TCM09_ACL1, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_TCM10_ACL1, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_TCM11_ACL1, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_TCM12_ACL1, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_TCM13_ACL1, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_TCM14_ACL1, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_TCM15_ACL1, 31, 0, 0x20000000)))
        {
            _BIST_REG_ERR(unit, MBIST_TCM08_ACL1, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_TCM09_ACL1, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_TCM10_ACL1, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_TCM11_ACL1, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_TCM12_ACL1, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_TCM13_ACL1, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_TCM14_ACL1, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_TCM15_ACL1, 31, 0, 0x20000000);
            _BIST_PRINTF("MBIST_TCM08~15_ACL1 have Fail\n");
        }

        _BIST_REG_UPD(unit, MBIST_ACL_START, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MACL2_LOG00~03======================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        _BIST_REG_UPD(unit, MBIST_ACL_START, 31, 0, 0xf0000);
        _bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_LOG00_ACL2, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_LOG01_ACL2, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_LOG02_ACL2, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_LOG03_ACL2, 31, 0, 0x20000000)))
        {
            _BIST_REG_ERR(unit, MBIST_LOG00_ACL2, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_LOG01_ACL2, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_LOG02_ACL2, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_LOG03_ACL2, 31, 0, 0x20000000);
            _BIST_PRINTF("MBIST_LOG00~03_ACL2 have Fail\n");
        }

        _BIST_REG_UPD(unit, MBIST_ACL_START, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MACL3_ACT00~07======================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        _BIST_REG_UPD(unit, MBIST_ACL_START, 31, 0, 0xff00000);
        _bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_ACT00_ACL3, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ACT01_ACL3, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ACT02_ACL3, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ACT03_ACL3, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ACT04_ACL3, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ACT05_ACL3, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ACT06_ACL3, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ACT07_ACL3, 31, 0, 0x20000000)))
        {
            _BIST_REG_ERR(unit, MBIST_ACT00_ACL3, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ACT01_ACL3, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ACT02_ACL3, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ACT03_ACL3, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ACT04_ACL3, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ACT05_ACL3, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ACT06_ACL3, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ACT07_ACL3, 31, 0, 0x20000000);
            _BIST_PRINTF("MBIST_ACT00~07_ACL3 have Fail\n");
        }

        _BIST_REG_UPD(unit, MBIST_ACL_START, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_MIB 00~02======================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        _BIST_REG_UPD(unit, MBIST_MIB_START, 31, 0, 0x7);
        _bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_MIB00, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_MIB01, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_MIB02, 31, 0, 0x20000000)))
        {
            _BIST_REG_ERR(unit, MBIST_MIB00, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_MIB01, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_MIB02, 31, 0, 0x20000000);
            _BIST_PRINTF("MBIST_MIB00~02 have Fail\n");
        }

        _BIST_REG_UPD(unit, MBIST_MIB_START, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_ENCAP test======================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        _BIST_REG_UPD(unit, MBIST_ENCAP_START, 31, 0, 0x3fC00);
        _bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_ENCAP01, 31,  0, 0x0)) ||
            (_BIST_REG_CHK(unit, MBIST_ENCAP09, 31, 24, 0xaa)))
        {
            _BIST_PRINTF("MBIST_ENCAP_MPG00~04 have Fail\n");
        }

        if ((_BIST_REG_CHK(unit, MBIST_ENCAP03, 31,  0, 0x0)) ||
            (_BIST_REG_CHK(unit, MBIST_ENCAP09, 23, 16, 0xaa)))
        {
            _BIST_PRINTF("MBIST_ENCAP_MPT00~04 have Fail\n");
        }

        _BIST_REG_UPD(unit, MBIST_ENCAP_START, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_ENCAP MPF & MPHF 1~4======================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        _BIST_REG_UPD(unit, MBIST_ENCAP_START, 31, 0, 0x3fC0000);
        _bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_ENCAP05, 31, 0, 0x0)) ||
            (_BIST_REG_CHK(unit, MBIST_ENCAP09, 15, 8, 0xaa)))
        {
            _BIST_PRINTF("MBIST_ENCAP_MPF00~04 have Fail\n");
        }

        if ((_BIST_REG_CHK(unit, MBIST_ENCAP07, 31, 0, 0x0)) ||
            (_BIST_REG_CHK(unit, MBIST_ENCAP09,  7, 0, 0xaa)))
        {
            _BIST_PRINTF("MBIST_ENCAP_MPHF00~04 have Fail\n");
        }

        _BIST_REG_UPD(unit, MBIST_ENCAP_START, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_ENCAP MTCAM======================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        _BIST_REG_UPD(unit, MBIST_ENCAP_START, 31, 0, 0x4000000);
        _bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_ENCAP_TCAM, 31, 0, 0x2000000)))
        {
            _BIST_PRINTF("MBIST_ENCAP_MTCAM Fail\n");
        }

        _BIST_REG_UPD(unit, MBIST_ENCAP_START, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_ENCAP MKELL & MRMK & & MEVC & MMPLS & MTS======================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        _BIST_REG_UPD(unit, MBIST_ENCAP_START, 31, 0, 0x7f);
        _bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_ENCAP11, 28, 24, 0x0)) ||
            (_BIST_REG_CHK(unit, MBIST_ENCAP13, 30, 28, 0x2)))
        {
            _BIST_PRINTF("MBIST_ENCAP_MKELL Fail\n");
        }

        if ((_BIST_REG_CHK(unit, MBIST_ENCAP11, 21, 16, 0x0)) ||
            (_BIST_REG_CHK(unit, MBIST_ENCAP13, 27, 20, 0x22)))
        {
            _BIST_PRINTF("MBIST_ENCAP_MTS Fail\n");
        }

        if ((_BIST_REG_CHK(unit, MBIST_ENCAP11, 13, 12, 0x0)) ||
            (_BIST_REG_CHK(unit, MBIST_ENCAP13, 19, 12, 0x22)))
        {
            _BIST_PRINTF("MBIST_ENCAP_MRMK Fail\n");
        }

        if ((_BIST_REG_CHK(unit, MBIST_ENCAP11,  8,  8, 0x0)) ||
            (_BIST_REG_CHK(unit, MBIST_ENCAP13, 11,  8, 0x2)))
        {
            _BIST_PRINTF("MBIST_ENCAP_MEVC Fail\n");
        }

        if ((_BIST_REG_CHK(unit, MBIST_ENCAP11,  4,  4, 0x0)) ||
            (_BIST_REG_CHK(unit, MBIST_ENCAP13,  7,  4, 0x2)))
        {
            _BIST_PRINTF("MBIST_ENCAP_MMPLS Fail\n");
        }

        _BIST_REG_UPD(unit, MBIST_ENCAP_START, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_ENCAP MBS & MUNTAG======================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        _BIST_REG_UPD(unit, MBIST_ENCAP_START, 31, 0, 0x380);
        _bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_ENCAP11, 0, 0, 0x0)) ||
            (_BIST_REG_CHK(unit, MBIST_ENCAP13, 3, 0, 0x2)) ||
            (_BIST_REG_CHK(unit, MBISR_ENCAP01, 31, 0, 0x2000000)))
        {
            _BIST_PRINTF("MBIST_ENCAP_MBS Fail\n");
        }

        if ((_BIST_REG_CHK(unit, MBISR_ENCAP02, 31, 0, 0x2000000)))
        {
            _BIST_PRINTF("MBIST_ENCAP_MUNTAG Fail\n");
        }

        _BIST_REG_UPD(unit, MBIST_ENCAP_START, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_PAR 00~03======================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        _BIST_REG_UPD(unit, MBIST_PAR_START, 31, 0, 0xf);
        _bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_PAR01_0, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PAR01_1, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PAR02,   31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PAR03,   31, 0, 0x20000000)))
        {
            _BIST_REG_ERR(unit, MBIST_PAR01_0, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_PAR01_1, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_PAR02,   31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_PAR03,   31, 0, 0x20000000);
            _BIST_PRINTF("MBIST_PAR00~03 have Fail\n");
        }

        _BIST_REG_UPD(unit, MBIST_PAR_START, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_IGR 01~07======================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        _BIST_REG_UPD(unit, MBIST_IGR_START, 31, 0, 0x3f);
        _bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_IGR01_0, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_IGR01_1, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_IGR02,   31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_IGR03,   31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_IGR04,   31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_IGR05,   31, 0, 0x20000000)))
        {
            _BIST_REG_ERR(unit, MBIST_IGR01_0, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_IGR01_1, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_IGR02,   31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_IGR03,   31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_IGR04,   31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_IGR05,   31, 0, 0x20000000);
            _BIST_PRINTF("MBIST_IGR00~05 have Fail\n");
        }

        _BIST_REG_UPD(unit, MBIST_IGR_START, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============IGR06~07======================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        _BIST_REG_UPD(unit, MBIST_IGR_START, 31, 0, 0xfc0);
        _bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_IGR06_0, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_IGR06_1, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_IGR06_2, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_IGR06_3, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_IGR06_4, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_IGR07,   31, 0, 0x20000000)))
        {
            _BIST_REG_ERR(unit, MBIST_IGR06_0, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_IGR06_1, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_IGR06_2, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_IGR06_3, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_IGR06_4, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_IGR07,   31, 0, 0x20000000);
            _BIST_PRINTF("MBIST_IGR06~07 have Fail\n");
        }

        _BIST_REG_UPD(unit, MBIST_IGR_START, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_EGR======================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        _BIST_REG_UPD(unit, MBIST_EGR_START_0, 31, 0, 0x1fe0);
        _bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_EGR01, 23, 8, 0x0)) ||
            (_BIST_REG_CHK(unit, MBIST_EGR11, 12, 5, 0xff)))
        {
            _BIST_PRINTF("MBIST_EGR01~07 have Fail\n");
        }

        _BIST_REG_UPD(unit, MBIST_EGR_START_0, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============EGR08~11======================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        _BIST_REG_UPD(unit, MBIST_EGR_START_0, 31, 0, 0x1e000);
        _bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_EGR01, 31, 24, 0x0)) ||
            (_BIST_REG_CHK(unit, MBIST_EGR11, 16, 13, 0xf)))
        {
            _BIST_PRINTF("MBIST_EGR08~11 have Fail\n");
        }

        _BIST_REG_UPD(unit, MBIST_EGR_START_0, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============EGR12~19======================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        _BIST_REG_UPD(unit, MBIST_EGR_START_0, 31, 0, 0x1fe0000);
        _bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_EGR02, 15,  0, 0x0)) ||
            (_BIST_REG_CHK(unit, MBIST_EGR11, 24, 17, 0xff)))
        {
            _BIST_PRINTF("MBIST_EGR12~19 have Fail\n");
        }

        _BIST_REG_UPD(unit, MBIST_EGR_START_0, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============EGR20~27======================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        _BIST_REG_UPD(unit, MBIST_EGR_START_0, 31, 0, 0xfe000000);
        _BIST_REG_UPD(unit, MBIST_EGR_START_1, 31, 0, 0x1);
        _bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_EGR02, 31, 16, 0x0)) ||
            (_BIST_REG_CHK(unit, MBIST_EGR11, 31, 25, 0x7f)) ||
            (_BIST_REG_CHK(unit, MBIST_EGR12,  0,  0, 0x1)))
        {
            _BIST_PRINTF("MBIST_EGR20~27 have Fail\n");
        }

        _BIST_REG_UPD(unit, MBIST_EGR_START_0, 31, 0, 0x0);
        _BIST_REG_UPD(unit, MBIST_EGR_START_1, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============EGR28~35======================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        _BIST_REG_UPD(unit, MBIST_EGR_START_1, 31, 0, 0x1fe);
        _bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_EGR03, 15,  0, 0x0)) ||
            (_BIST_REG_CHK(unit, MBIST_EGR12,  8,  1, 0xff)))
        {
            _BIST_PRINTF("MBIST_EGR28~35 have Fail\n");
        }

        _BIST_REG_UPD(unit, MBIST_EGR_START_1, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============EGR36~43======================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        _BIST_REG_UPD(unit, MBIST_EGR_START_1, 31, 0, 0x1fe00);
        _bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_EGR03, 31, 16, 0x0)) ||
            (_BIST_REG_CHK(unit, MBIST_EGR12, 16,  9, 0xff)))
        {
            _BIST_PRINTF("MBIST_EGR36~43 have Fail\n");
        }

        _BIST_REG_UPD(unit, MBIST_EGR_START_1, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============EGR44~51======================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        _BIST_REG_UPD(unit, MBIST_EGR_START_1, 31, 0, 0x1fe0000);
        _bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_EGR04, 15,  0, 0x0)) ||
            (_BIST_REG_CHK(unit, MBIST_EGR12, 24, 17, 0xff)))
        {
            _BIST_PRINTF("MBIST_EGR44~51 have Fail\n");
        }

        _BIST_REG_UPD(unit, MBIST_EGR_START_1, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============EGR52~57======================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        _BIST_REG_UPD(unit, MBIST_EGR_START_1, 31, 0, 0x7e000000);
        _bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_EGR04, 27, 16, 0x0)) ||
            (_BIST_REG_CHK(unit, MBIST_EGR12, 30, 25, 0x3f)))
        {
            _BIST_PRINTF("MBIST_EGR52~57 have Fail\n");
        }

        _BIST_REG_UPD(unit, MBIST_EGR_START_1, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============EGR_MDRC_CC & MPORT & MPORT_EXT & MLINK======================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        _BIST_REG_UPD(unit, MBIST_EGR_START_0, 31, 0, 0x1e);
        _bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_EGR01, 5, 1, 0x0)) ||
            (_BIST_REG_CHK(unit, MBIST_EGR11, 4, 1, 0xf)))
        {
            _BIST_PRINTF("MBIST_EGR_MDRC_CC & MPORT & MPORT_EXT & MLINK have Fail\n");
        }

        _BIST_REG_UPD(unit, MBIST_EGR_START_0, 31, 0, 0x0);
    }

    /* clear MBIST test */
    _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x0);

    /*
     * Data Retention Fault
     */

    _BIST_PRINTF("==============MAC_DRF_MBIST======================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        _BIST_REG_UPD(unit, MBIST_MAC_RESET_00, 31, 0, 0xb);
        _bist_mdelay(100);

        if ((_BIST_REG_CHK(unit, MBIST_PG00_RX_MAC01, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG00_RX_MAC05, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG00_TX_MAC01, 24, 24, 0x01)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_PG00_RX_MAC01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG00_RX_MAC01, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG00_RX_MAC05 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG00_RX_MAC05, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG00_TX_MAC01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG00_TX_MAC01, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_MAC_RESUME_00, 31, 0, 0xb);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_MAC_RESUME_00, 31, 0, 0x0);
        }

        _bist_mdelay(100);
        if ((_BIST_REG_CHK(unit, MBIST_PG00_RX_MAC01, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG00_RX_MAC05, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG00_TX_MAC01, 24, 24, 0x01)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_PG00_RX_MAC01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG00_RX_MAC01, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG00_RX_MAC05 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG00_RX_MAC05, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG00_TX_MAC01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG00_TX_MAC01, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_MAC_RESUME_00, 31, 0, 0xb);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_MAC_RESUME_00, 31, 0, 0x0);
            _bist_mdelay(100);
            if ((_BIST_REG_CHK(unit, MBIST_PG00_RX_MAC01, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG00_RX_MAC05, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG00_TX_MAC01, 28, 12, 0x10000)))
            {
                _BIST_PRINTF("MBIST_DRF_PG0 Fail\n");
            }
        }

        _BIST_REG_UPD(unit, MBIST_MAC_RESET_00, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_PG1======================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        _BIST_REG_UPD(unit, MBIST_MAC_RESET_00, 31, 0, 0x160);
        _bist_mdelay(100);

        if ((_BIST_REG_CHK(unit, MBIST_PG01_RX_MAC01, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG01_RX_MAC05, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG01_TX_MAC01, 24, 24, 0x01)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_PG01_RX_MAC01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG01_RX_MAC01, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG01_RX_MAC05 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG01_RX_MAC05, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG01_TX_MAC01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG01_TX_MAC01, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_MAC_RESUME_00, 31, 0, 0x160);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_MAC_RESUME_00, 31, 0, 0x0);
        }

        _bist_mdelay(100);
        if ((_BIST_REG_CHK(unit, MBIST_PG01_RX_MAC01, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG01_RX_MAC05, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG01_TX_MAC01, 24, 24, 0x01)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_PG01_RX_MAC01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG01_RX_MAC01, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG01_RX_MAC05 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG01_RX_MAC05, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG01_TX_MAC01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG01_TX_MAC01, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_MAC_RESUME_00, 31, 0, 0x160);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_MAC_RESUME_00, 31, 0, 0x0);
            _bist_mdelay(100);
            if ((_BIST_REG_CHK(unit, MBIST_PG01_RX_MAC01, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG01_RX_MAC05, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG01_TX_MAC01, 28, 12, 0x10000)))
            {
                _BIST_PRINTF("MBIST_DRF_PG1 Fail\n");
            }
        }

        _BIST_REG_UPD(unit, MBIST_MAC_RESET_00, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_PG2======================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        _BIST_REG_UPD(unit, MBIST_MAC_RESET_00, 31, 0, 0x2c00);
        _bist_mdelay(100);

        if ((_BIST_REG_CHK(unit, MBIST_PG02_RX_MAC01, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG02_RX_MAC05, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG02_TX_MAC01, 24, 24, 0x01)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_PG02_RX_MAC01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG02_RX_MAC01, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG02_RX_MAC05 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG02_RX_MAC05, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG02_TX_MAC01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG02_TX_MAC01, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_MAC_RESUME_00, 31, 0, 0x2c00);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_MAC_RESUME_00, 31, 0, 0x0);
        }

        _bist_mdelay(100);
        if ((_BIST_REG_CHK(unit, MBIST_PG02_RX_MAC01, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG02_RX_MAC05, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG02_TX_MAC01, 24, 24, 0x01)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_PG02_RX_MAC01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG02_RX_MAC01, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG02_RX_MAC05 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG02_RX_MAC05, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG02_TX_MAC01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG02_TX_MAC01, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_MAC_RESUME_00, 31, 0, 0x2c00);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_MAC_RESUME_00, 31, 0, 0x0);
            _bist_mdelay(100);
            if ((_BIST_REG_CHK(unit, MBIST_PG02_RX_MAC01, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG02_RX_MAC05, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG02_TX_MAC01, 28, 12, 0x10000)))
            {
                _BIST_PRINTF("MBIST_DRF_PG2 Fail\n");
            }
        }

        _BIST_REG_UPD(unit, MBIST_MAC_RESET_00, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_PG3======================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        _BIST_REG_UPD(unit, MBIST_MAC_RESET_00, 31, 0, 0x58000);
        _bist_mdelay(100);

        if ((_BIST_REG_CHK(unit, MBIST_PG03_RX_MAC01, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG03_RX_MAC05, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG03_TX_MAC01, 24, 24, 0x01)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_PG03_RX_MAC01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG03_RX_MAC01, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG03_RX_MAC05 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG03_RX_MAC05, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG03_TX_MAC01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG03_TX_MAC01, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_MAC_RESUME_00, 31, 0, 0x58000);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_MAC_RESUME_00, 31, 0, 0x0);
        }

        _bist_mdelay(100);
        if ((_BIST_REG_CHK(unit, MBIST_PG03_RX_MAC01, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG03_RX_MAC05, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG03_TX_MAC01, 24, 24, 0x01)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_PG03_RX_MAC01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG03_RX_MAC01, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG03_RX_MAC05 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG03_RX_MAC05, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG03_TX_MAC01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG03_TX_MAC01, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_MAC_RESUME_00, 31, 0, 0x58000);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_MAC_RESUME_00, 31, 0, 0x0);
            _bist_mdelay(100);
            if ((_BIST_REG_CHK(unit, MBIST_PG03_RX_MAC01, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG03_RX_MAC05, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG03_TX_MAC01, 28, 12, 0x10000)))
            {
                _BIST_PRINTF("MBIST_DRF_PG3 Fail\n");
            }
        }

        _BIST_REG_UPD(unit, MBIST_MAC_RESET_00, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_PG4======================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        _BIST_REG_UPD(unit, MBIST_MAC_RESET_00, 31, 0, 0xb00000);
        _bist_mdelay(100);

        if ((_BIST_REG_CHK(unit, MBIST_PG04_RX_MAC01, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG04_RX_MAC05, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG04_TX_MAC01, 24, 24, 0x01)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_PG04_RX_MAC01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG04_RX_MAC01, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG04_RX_MAC05 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG04_RX_MAC05, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG04_TX_MAC01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG04_TX_MAC01, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_MAC_RESUME_00, 31, 0, 0xb00000);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_MAC_RESUME_00, 31, 0, 0x0);
        }

        _bist_mdelay(100);
        if ((_BIST_REG_CHK(unit, MBIST_PG04_RX_MAC01, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG04_RX_MAC05, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG04_TX_MAC01, 24, 24, 0x01)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_PG04_RX_MAC01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG04_RX_MAC01, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG04_RX_MAC05 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG04_RX_MAC05, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG04_TX_MAC01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG04_TX_MAC01, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_MAC_RESUME_00, 31, 0, 0xb00000);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_MAC_RESUME_00, 31, 0, 0x0);
            _bist_mdelay(100);
            if ((_BIST_REG_CHK(unit, MBIST_PG04_RX_MAC01, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG04_RX_MAC05, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG04_TX_MAC01, 28, 12, 0x10000)))
            {
                _BIST_PRINTF("MBIST_DRF_PG4 Fail\n");
            }
        }

        _BIST_REG_UPD(unit, MBIST_MAC_RESET_00, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_PG5======================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        _BIST_REG_UPD(unit, MBIST_MAC_RESET_00, 31, 0, 0x16000000);
        _bist_mdelay(100);

        if ((_BIST_REG_CHK(unit, MBIST_PG05_RX_MAC01, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG05_RX_MAC05, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG05_TX_MAC01, 24, 24, 0x01)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_PG05_RX_MAC01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG05_RX_MAC01, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG05_RX_MAC05 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG05_RX_MAC05, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG05_TX_MAC01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG05_TX_MAC01, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_MAC_RESUME_00, 31, 0, 0x16000000);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_MAC_RESUME_00, 31, 0, 0x0);
        }

        _bist_mdelay(100);
        if ((_BIST_REG_CHK(unit, MBIST_PG05_RX_MAC01, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG05_RX_MAC05, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG05_TX_MAC01, 24, 24, 0x01)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_PG05_RX_MAC01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG05_RX_MAC01, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG05_RX_MAC05 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG05_RX_MAC05, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG05_TX_MAC01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG05_TX_MAC01, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_MAC_RESUME_00, 31, 0, 0x16000000);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_MAC_RESUME_00, 31, 0, 0x0);
            _bist_mdelay(100);
            if ((_BIST_REG_CHK(unit, MBIST_PG05_RX_MAC01, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG05_RX_MAC05, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG05_TX_MAC01, 28, 12, 0x10000)))
            {
                _BIST_PRINTF("MBIST_DRF_PG5 Fail\n");
            }
        }

        _BIST_REG_UPD(unit, MBIST_MAC_RESET_00, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_PG6======================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        _BIST_REG_UPD(unit, MBIST_MAC_RESET_01, 31, 0, 0x67);
        _bist_mdelay(100);

        if ((_BIST_REG_CHK(unit, MBIST_PG06_RX_MAC08, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG06_RX_MAC04, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG06_RX_MAC06, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG06_TX_MAC08, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG06_TX_MAC02, 24, 24, 0x01)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_PG06_RX_MAC08 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG06_RX_MAC08, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG06_RX_MAC04 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG06_RX_MAC04, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG06_RX_MAC06 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG06_RX_MAC06, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG06_TX_MAC08 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG06_TX_MAC08, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG06_TX_MAC02 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG06_TX_MAC02, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_MAC_RESUME_01, 31, 0, 0x67);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_MAC_RESUME_01, 31, 0, 0x0);
        }

        _bist_mdelay(100);
        if ((_BIST_REG_CHK(unit, MBIST_PG06_RX_MAC08, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG06_RX_MAC04, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG06_RX_MAC06, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG06_TX_MAC08, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG06_TX_MAC02, 24, 24, 0x01)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_PG06_RX_MAC08 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG06_RX_MAC08, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG06_RX_MAC04 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG06_RX_MAC04, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG06_RX_MAC06 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG06_RX_MAC06, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG06_TX_MAC08 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG06_TX_MAC08, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG06_TX_MAC02 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG06_TX_MAC02, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_MAC_RESUME_01, 31, 0, 0x67);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_MAC_RESUME_01, 31, 0, 0x0);
            _bist_mdelay(100);
            if ((_BIST_REG_CHK(unit, MBIST_PG06_RX_MAC08, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG06_RX_MAC04, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG06_RX_MAC06, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG06_TX_MAC08, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG06_TX_MAC02, 28, 12, 0x10000)))
            {
                _BIST_PRINTF(" wait 500ms MBIST_PG06_RX_MAC08 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG06_RX_MAC08, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_PG06_RX_MAC04 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG06_RX_MAC04, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_PG06_RX_MAC06 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG06_RX_MAC06, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_PG06_TX_MAC08 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG06_TX_MAC08, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_PG06_TX_MAC02 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG06_TX_MAC02, 28, 12));
                _BIST_PRINTF("MBIST_DRF_PG6 Fail\n");
            }
        }

        _BIST_REG_UPD(unit, MBIST_MAC_RESET_01, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_PG7======================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        _BIST_REG_UPD(unit, MBIST_MAC_RESET_01, 31, 0, 0x3380);
        _bist_mdelay(100);

        if ((_BIST_REG_CHK(unit, MBIST_PG07_RX_MAC08, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG07_RX_MAC04, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG07_RX_MAC06, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG07_TX_MAC08, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG07_TX_MAC02, 24, 24, 0x01)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_PG07_RX_MAC08 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG07_RX_MAC08, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG07_RX_MAC04 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG07_RX_MAC04, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG07_RX_MAC06 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG07_RX_MAC06, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG07_TX_MAC08 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG07_TX_MAC08, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG07_TX_MAC02 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG07_TX_MAC02, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_MAC_RESUME_01, 31, 0, 0x3380);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_MAC_RESUME_01, 31, 0, 0x0);
        }

        _bist_mdelay(100);
        if ((_BIST_REG_CHK(unit, MBIST_PG07_RX_MAC08, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG07_RX_MAC04, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG07_RX_MAC06, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG07_TX_MAC08, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG07_TX_MAC02, 24, 24, 0x01)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_PG07_RX_MAC08 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG07_RX_MAC08, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG07_RX_MAC04 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG07_RX_MAC04, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG07_RX_MAC06 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG07_RX_MAC06, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG07_TX_MAC08 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG07_TX_MAC08, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG07_TX_MAC02 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG07_TX_MAC02, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_MAC_RESUME_01, 31, 0, 0x3380);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_MAC_RESUME_01, 31, 0, 0x0);
            _bist_mdelay(100);
            if ((_BIST_REG_CHK(unit, MBIST_PG07_RX_MAC08, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG07_RX_MAC04, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG07_RX_MAC06, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG07_TX_MAC08, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG07_TX_MAC02, 28, 12, 0x10000)))
            {
                _BIST_PRINTF(" wait 500ms MBIST_PG07_RX_MAC08 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG07_RX_MAC08, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_PG07_RX_MAC04 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG07_RX_MAC04, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_PG07_RX_MAC06 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG07_RX_MAC06, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_PG07_TX_MAC08 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG07_TX_MAC08, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_PG07_TX_MAC02 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG07_TX_MAC02, 28, 12));
                _BIST_PRINTF("MBIST_DRF_PG7 Fail\n");
            }
        }

        _BIST_REG_UPD(unit, MBIST_MAC_RESET_01, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_PG8======================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        _BIST_REG_UPD(unit, MBIST_MAC_RESET_02, 31, 0, 0x37);
        _bist_mdelay(100);

        if ((_BIST_REG_CHK(unit, MBIST_PG08_RX_MAC10, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG08_RX_MAC07, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG08_RX_MAC11, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG08_TX_MAC10, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG08_TX_MAC12, 24, 24, 0x01)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_PG08_RX_MAC10 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG08_RX_MAC10, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG08_RX_MAC07 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG08_RX_MAC07, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG08_RX_MAC11 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG08_RX_MAC11, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG08_TX_MAC10 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG08_TX_MAC10, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG08_TX_MAC12 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG08_TX_MAC12, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_MAC_RESUME_02, 31, 0, 0x37);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_MAC_RESUME_02, 31, 0, 0x0);
        }

        _bist_mdelay(100);
        if ((_BIST_REG_CHK(unit, MBIST_PG08_RX_MAC10, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG08_RX_MAC07, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG08_RX_MAC11, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG08_TX_MAC10, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG08_TX_MAC12, 24, 24, 0x01)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_PG08_RX_MAC10 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG08_RX_MAC10, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG08_RX_MAC07 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG08_RX_MAC07, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG08_RX_MAC11 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG08_RX_MAC11, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG08_TX_MAC10 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG08_TX_MAC10, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG08_TX_MAC12 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG08_TX_MAC12, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_MAC_RESUME_02, 31, 0, 0x37);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_MAC_RESUME_02, 31, 0, 0x0);
            _bist_mdelay(100);
            if ((_BIST_REG_CHK(unit, MBIST_PG08_RX_MAC10, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG08_RX_MAC07, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG08_RX_MAC11, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG08_TX_MAC10, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG08_TX_MAC12, 28, 12, 0x10000)))
            {
                _BIST_PRINTF(" wait 500ms MBIST_PG08_RX_MAC10 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG08_RX_MAC10, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_PG08_RX_MAC07 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG08_RX_MAC07, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_PG08_RX_MAC11 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG08_RX_MAC11, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_PG08_TX_MAC10 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG08_TX_MAC10, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_PG08_TX_MAC12 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG08_TX_MAC12, 28, 12));
                _BIST_PRINTF("MBIST_DRF_PG8 Fail\n");
            }
        }

        _BIST_REG_UPD(unit, MBIST_MAC_RESET_02, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_PG9======================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        _BIST_REG_UPD(unit, MBIST_MAC_RESET_02, 31, 0, 0xdc0);
        _bist_mdelay(100);

        if ((_BIST_REG_CHK(unit, MBIST_PG09_RX_MAC10, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG09_RX_MAC07, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG09_RX_MAC11, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG09_TX_MAC10, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG09_TX_MAC12, 24, 24, 0x01)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_PG09_RX_MAC10 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG09_RX_MAC10, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG09_RX_MAC07 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG09_RX_MAC07, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG09_RX_MAC11 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG09_RX_MAC11, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG09_TX_MAC10 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG09_TX_MAC10, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG09_TX_MAC12 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG09_TX_MAC12, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_MAC_RESUME_02, 31, 0, 0xdc0);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_MAC_RESUME_02, 31, 0, 0x0);
        }

        _bist_mdelay(100);
        if ((_BIST_REG_CHK(unit, MBIST_PG09_RX_MAC10, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG09_RX_MAC07, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG09_RX_MAC11, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG09_TX_MAC10, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG09_TX_MAC12, 24, 24, 0x01)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_PG09_RX_MAC10 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG09_RX_MAC10, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG09_RX_MAC07 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG09_RX_MAC07, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG09_RX_MAC11 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG09_RX_MAC11, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG09_TX_MAC10 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG09_TX_MAC10, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG09_TX_MAC12 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG09_TX_MAC12, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_MAC_RESUME_02, 31, 0, 0xdc0);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_MAC_RESUME_02, 31, 0, 0x0);
            _bist_mdelay(100);
            if ((_BIST_REG_CHK(unit, MBIST_PG09_RX_MAC10, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG09_RX_MAC07, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG09_RX_MAC11, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG09_TX_MAC10, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG09_TX_MAC12, 28, 12, 0x10000)))
            {
                _BIST_PRINTF(" wait 500ms MBIST_PG09_RX_MAC10 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG09_RX_MAC10, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_PG09_RX_MAC07 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG09_RX_MAC07, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_PG09_RX_MAC11 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG09_RX_MAC11, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_PG09_TX_MAC10 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG09_TX_MAC10, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_PG09_TX_MAC12 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG09_TX_MAC12, 28, 12));
                _BIST_PRINTF("MBIST_DRF_PG9 Fail\n");
            }
        }

        _BIST_REG_UPD(unit, MBIST_MAC_RESET_02, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_PG10======================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        _BIST_REG_UPD(unit, MBIST_MAC_RESET_02, 31, 0, 0x37000);
        _bist_mdelay(100);

        if ((_BIST_REG_CHK(unit, MBIST_PG10_RX_MAC10, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG10_RX_MAC07, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG10_RX_MAC11, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG10_TX_MAC10, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG10_TX_MAC12, 24, 24, 0x01)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_PG10_RX_MAC10 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG10_RX_MAC10, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG10_RX_MAC07 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG10_RX_MAC07, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG10_RX_MAC11 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG10_RX_MAC11, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG10_TX_MAC10 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG10_TX_MAC10, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG10_TX_MAC12 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG10_TX_MAC12, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_MAC_RESUME_02, 31, 0, 0x37000);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_MAC_RESUME_02, 31, 0, 0x0);
        }

        _bist_mdelay(100);
        if ((_BIST_REG_CHK(unit, MBIST_PG10_RX_MAC10, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG10_RX_MAC07, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG10_RX_MAC11, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG10_TX_MAC10, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG10_TX_MAC12, 24, 24, 0x01)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_PG10_RX_MAC10 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG10_RX_MAC10, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG10_RX_MAC07 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG10_RX_MAC07, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG10_RX_MAC11 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG10_RX_MAC11, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG10_TX_MAC10 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG10_TX_MAC10, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG10_TX_MAC12 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG10_TX_MAC12, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_MAC_RESUME_02, 31, 0, 0x37000);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_MAC_RESUME_02, 31, 0, 0x0);
            _bist_mdelay(100);
            if ((_BIST_REG_CHK(unit, MBIST_PG10_RX_MAC10, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG10_RX_MAC07, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG10_RX_MAC11, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG10_TX_MAC10, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG10_TX_MAC12, 28, 12, 0x10000)))
            {
                _BIST_PRINTF(" wait 500ms MBIST_PG10_RX_MAC10 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG10_RX_MAC10, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_PG10_RX_MAC07 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG10_RX_MAC07, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_PG10_RX_MAC11 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG10_RX_MAC11, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_PG10_TX_MAC10 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG10_TX_MAC10, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_PG10_TX_MAC12 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG10_TX_MAC12, 28, 12));
                _BIST_PRINTF("MBIST_DRF_PG10 Fail\n");
            }
        }

        _BIST_REG_UPD(unit, MBIST_MAC_RESET_02, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_PG11======================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        _BIST_REG_UPD(unit, MBIST_MAC_RESET_02, 31, 0, 0xDC0000);
        _bist_mdelay(100);

        if ((_BIST_REG_CHK(unit, MBIST_PG11_RX_MAC10, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG11_RX_MAC07, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG11_RX_MAC11, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG11_TX_MAC10, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG11_TX_MAC12, 24, 24, 0x01)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_PG11_RX_MAC10 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG11_RX_MAC10, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG11_RX_MAC07 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG11_RX_MAC07, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG11_RX_MAC11 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG11_RX_MAC11, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG11_TX_MAC10 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG11_TX_MAC10, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG11_TX_MAC12 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG11_TX_MAC12, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_MAC_RESUME_02, 31, 0, 0xDC0000);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_MAC_RESUME_02, 31, 0, 0x0);
        }

        _bist_mdelay(100);
        if ((_BIST_REG_CHK(unit, MBIST_PG11_RX_MAC10, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG11_RX_MAC07, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG11_RX_MAC11, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG11_TX_MAC10, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG11_TX_MAC12, 24, 24, 0x01)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_PG11_RX_MAC10 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG11_RX_MAC10, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG11_RX_MAC07 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG11_RX_MAC07, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG11_RX_MAC11 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG11_RX_MAC11, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG11_TX_MAC10 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG11_TX_MAC10, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG11_TX_MAC12 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG11_TX_MAC12, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_MAC_RESUME_02, 31, 0, 0xDC0000);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_MAC_RESUME_02, 31, 0, 0x0);
            _bist_mdelay(100);
            if ((_BIST_REG_CHK(unit, MBIST_PG11_RX_MAC10, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG11_RX_MAC07, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG11_RX_MAC11, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG11_TX_MAC10, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG11_TX_MAC12, 28, 12, 0x10000)))
            {
                _BIST_PRINTF(" wait 500ms MBIST_PG11_RX_MAC10 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG11_RX_MAC10, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_PG11_RX_MAC07 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG11_RX_MAC07, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_PG11_RX_MAC11 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG11_RX_MAC11, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_PG11_TX_MAC10 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG11_TX_MAC10, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_PG11_TX_MAC12 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG11_TX_MAC12, 28, 12));
                _BIST_PRINTF("MBIST_DRF_PG11 Fail\n");
            }
        }

        _BIST_REG_UPD(unit, MBIST_MAC_RESET_02, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_PG12======================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        _BIST_REG_UPD(unit, MBIST_MAC_RESET_01, 31, 0, 0x39c000);
        _bist_mdelay(100);

        if ((_BIST_REG_CHK(unit, MBIST_PG12_RX_MAC03, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG12_RX_MAC09, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG12_RX_MAC07, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG12_TX_MAC02, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG12_PG_MAC13, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG12_PG_MAC14, 24, 24, 0x01)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_PG12_RX_MAC03 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG12_RX_MAC03, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG12_RX_MAC09 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG12_RX_MAC09, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG12_RX_MAC07 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG12_RX_MAC07, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG12_TX_MAC02 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG12_TX_MAC02, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG12_PG_MAC13 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG12_PG_MAC13, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG12_PG_MAC14 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG12_PG_MAC14, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_MAC_RESUME_01, 31, 0, 0x39c000);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_MAC_RESUME_01, 31, 0, 0x0);
        }

        _bist_mdelay(100);
        if ((_BIST_REG_CHK(unit, MBIST_PG12_RX_MAC03, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG12_RX_MAC09, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG12_RX_MAC07, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG12_TX_MAC02, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG12_PG_MAC13, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG12_PG_MAC14, 24, 24, 0x01)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_PG12_RX_MAC03 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG12_RX_MAC03, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG12_RX_MAC09 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG12_RX_MAC09, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG12_RX_MAC07 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG12_RX_MAC07, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG12_TX_MAC02 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG12_TX_MAC02, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG12_PG_MAC13 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG12_PG_MAC13, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG12_PG_MAC14 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG12_PG_MAC14, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_MAC_RESUME_01, 31, 0, 0x39c000);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_MAC_RESUME_01, 31, 0, 0x0);
            _bist_mdelay(100);
            if ((_BIST_REG_CHK(unit, MBIST_PG12_RX_MAC03, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG12_RX_MAC09, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG12_RX_MAC07, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG12_TX_MAC02, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG12_PG_MAC13, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG12_PG_MAC14, 28, 12, 0x10000)))
            {
                _BIST_PRINTF(" wait 500ms MBIST_PG12_RX_MAC03 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG12_RX_MAC03, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_PG12_RX_MAC09 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG12_RX_MAC09, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_PG12_RX_MAC07 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG12_RX_MAC07, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_PG12_TX_MAC02 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG12_TX_MAC02, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_PG12_PG_MAC13 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG12_PG_MAC13, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_PG12_PG_MAC14 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG12_PG_MAC14, 28, 12));
                _BIST_PRINTF("MBIST_DRF_PG12 Fail\n");
            }
        }

        _BIST_REG_UPD(unit, MBIST_MAC_RESET_01, 31, 0, 0x0);
    }

    /*
     * ALE_DRF_MBIST
     */

    _BIST_PRINTF("==============DRF_MBIST_ALE1~8======================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        _BIST_REG_UPD(unit, MBIST_ALE_START_0, 31, 0, 0xff);
        _bist_mdelay(100);

        if ((_BIST_REG_CHK(unit, MBIST_ALE01, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE02, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE03, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE04, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE05, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE06, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE07, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE08, 24, 24, 0x01)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_ALE01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE01, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE02 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE02, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE03 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE03, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE04 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE04, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE05 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE05, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE06 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE06, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE07 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE07, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE08 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE08, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_ALE_RESUME_0, 31, 0, 0xff);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_ALE_RESUME_0, 31, 0, 0x0);
        }

        _bist_mdelay(100);
        if ((_BIST_REG_CHK(unit, MBIST_ALE01, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE02, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE03, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE04, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE05, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE06, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE07, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE08, 24, 24, 0x01)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_ALE01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE01, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE02 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE02, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE03 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE03, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE04 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE04, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE05 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE05, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE06 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE06, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE07 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE07, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE08 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE08, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_ALE_RESUME_0, 31, 0, 0xff);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_ALE_RESUME_0, 31, 0, 0x0);
            _bist_mdelay(100);
            if ((_BIST_REG_CHK(unit, MBIST_ALE01, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ALE02, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ALE03, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ALE04, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ALE05, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ALE06, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ALE07, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ALE08, 28, 12, 0x10000)))
            {
                _BIST_PRINTF(" wait 500ms MBIST_ALE01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE01, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_ALE02 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE02, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_ALE03 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE03, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_ALE04 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE04, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_ALE05 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE05, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_ALE06 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE06, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_ALE07 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE07, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_ALE08 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE08, 28, 12));
                _BIST_PRINTF("MBIST_DRF_ALE1~8 have Fail\n");
            }
        }

        _BIST_REG_UPD(unit, MBIST_ALE_START_0, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============DRF_MBIST_ALE9~16======================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        _BIST_REG_UPD(unit, MBIST_ALE_START_0, 31, 0, 0xff00);
        _bist_mdelay(100);

        if ((_BIST_REG_CHK(unit, MBIST_ALE09, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE10, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE011, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE012, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE13, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE14, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE15, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE16, 24, 24, 0x01)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_ALE09 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE09, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE10 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE10, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE011 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE011, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE012 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE012, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE13 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE13, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE14 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE14, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE15 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE15, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE16 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE16, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_ALE_RESUME_0, 31, 0, 0xff00);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_ALE_RESUME_0, 31, 0, 0x0);
        }

        _bist_mdelay(100);
        if ((_BIST_REG_CHK(unit, MBIST_ALE09, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE10, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE011, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE012, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE13, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE14, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE15, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE16, 24, 24, 0x01)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_ALE09 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE09, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE10 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE10, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE011 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE011, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE012 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE012, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE13 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE13, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE14 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE14, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE15 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE15, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE16 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE16, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_ALE_RESUME_0, 31, 0, 0xff00);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_ALE_RESUME_0, 31, 0, 0x0);
            _bist_mdelay(100);
            if ((_BIST_REG_CHK(unit, MBIST_ALE09, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ALE10, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ALE011, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ALE012, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ALE13, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ALE14, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ALE15, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ALE16, 28, 12, 0x10000)))
            {
                _BIST_PRINTF(" wait 500ms MBIST_ALE09 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE09, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_ALE10 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE10, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_ALE011 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE011, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_ALE012 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE012, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_ALE13 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE13, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_ALE14 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE14, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_ALE15 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE15, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_ALE16 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE16, 28, 12));
                _BIST_PRINTF("MBIST_DRF_ALE9~16 have Fail\n");
            }
        }

        _BIST_REG_UPD(unit, MBIST_ALE_START_0, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============DRF_MBIST_ALE17~24======================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        _BIST_REG_UPD(unit, MBIST_ALE_START_0, 31, 0, 0xff0000);
        _bist_mdelay(100);

        if ((_BIST_REG_CHK(unit, MBIST_ALE17, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE18, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE19, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE20, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE21, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE22, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE23, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE24, 24, 24, 0x01)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_ALE17 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE17, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE18 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE18, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE19 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE19, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE20 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE20, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE21 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE21, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE22 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE22, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE23 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE23, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE24 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE24, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_ALE_RESUME_0, 31, 0, 0xff0000);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_ALE_RESUME_0, 31, 0, 0x0);
        }

        _bist_mdelay(100);
        if ((_BIST_REG_CHK(unit, MBIST_ALE17, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE18, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE19, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE20, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE21, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE22, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE23, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE24, 24, 24, 0x01)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_ALE17 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE17, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE18 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE18, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE19 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE19, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE20 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE20, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE21 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE21, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE22 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE22, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE23 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE23, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE24 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE24, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_ALE_RESUME_0, 31, 0, 0xff0000);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_ALE_RESUME_0, 31, 0, 0x0);
            _bist_mdelay(100);
            if ((_BIST_REG_CHK(unit, MBIST_ALE17, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ALE18, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ALE19, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ALE20, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ALE21, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ALE22, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ALE23, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ALE24, 28, 12, 0x10000)))
            {
                _BIST_PRINTF(" wait 500ms MBIST_ALE17 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE17, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_ALE18 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE18, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_ALE19 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE19, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_ALE20 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE20, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_ALE21 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE21, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_ALE22 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE22, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_ALE23 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE23, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_ALE24 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE24, 28, 12));
                _BIST_PRINTF("MBIST_DRF_ALE17~24 have Fail\n");
            }
        }

        _BIST_REG_UPD(unit, MBIST_ALE_START_0, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============DRF_MBIST_ALE25~32======================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        _BIST_REG_UPD(unit, MBIST_ALE_START_0, 31, 0, 0xff000000);
        _bist_mdelay(100);

        if ((_BIST_REG_CHK(unit, MBIST_ALE25, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE26, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE27, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE28, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE29, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE30, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE31, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE32, 24, 24, 0x01)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_ALE25 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE25, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE26 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE26, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE27 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE27, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE28 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE28, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE29 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE29, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE30 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE30, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE31 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE31, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE32 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE32, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_ALE_RESUME_0, 31, 0, 0xff000000);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_ALE_RESUME_0, 31, 0, 0x0);
        }

        _bist_mdelay(100);
        if ((_BIST_REG_CHK(unit, MBIST_ALE25, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE26, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE27, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE28, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE29, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE30, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE31, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE32, 24, 24, 0x01)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_ALE25 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE25, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE26 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE26, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE27 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE27, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE28 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE28, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE29 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE29, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE30 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE30, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE31 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE31, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE32 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE32, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_ALE_RESUME_0, 31, 0, 0xff000000);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_ALE_RESUME_0, 31, 0, 0x0);
            _bist_mdelay(100);
            if ((_BIST_REG_CHK(unit, MBIST_ALE25, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ALE26, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ALE27, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ALE28, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ALE29, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ALE30, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ALE31, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ALE32, 28, 12, 0x10000)))
            {
                _BIST_PRINTF(" wait 500ms MBIST_ALE25 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE25, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_ALE26 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE26, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_ALE27 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE27, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_ALE28 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE28, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_ALE29 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE29, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_ALE30 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE30, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_ALE31 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE31, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_ALE32 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE32, 28, 12));
                _BIST_PRINTF("MBIST_DRF_ALE25~32 have Fail\n");
            }
        }

        _BIST_REG_UPD(unit, MBIST_ALE_START_0, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============DRF_MBIST_ALE33~39======================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        _BIST_REG_UPD(unit, MBIST_ALE_START_1, 31, 0, 0x7f);
        _bist_mdelay(100);

        if ((_BIST_REG_CHK(unit, MBIST_ALE33, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE34, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE35, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE36, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE37, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE38, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE39, 24, 24, 0x01)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_ALE33 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE33, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE34 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE34, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE35 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE35, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE36 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE36, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE37 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE37, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE38 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE38, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE39 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE39, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_ALE_RESUME_1, 31, 0, 0x7f);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_ALE_RESUME_1, 31, 0, 0x0);
        }

        _bist_mdelay(100);
        if ((_BIST_REG_CHK(unit, MBIST_ALE33, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE34, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE35, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE36, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE37, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE38, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE39, 24, 24, 0x01)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_ALE33 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE33, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE34 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE34, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE35 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE35, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE36 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE36, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE37 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE37, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE38 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE38, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE39 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE39, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_ALE_RESUME_1, 31, 0, 0x7f);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_ALE_RESUME_1, 31, 0, 0x0);
            _bist_mdelay(100);
            if ((_BIST_REG_CHK(unit, MBIST_ALE33, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ALE34, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ALE35, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ALE36, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ALE37, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ALE38, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ALE39, 28, 12, 0x10000)))
            {
                _BIST_PRINTF(" wait 500ms MBIST_ALE33 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE33, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_ALE34 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE34, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_ALE35 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE35, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_ALE36 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE36, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_ALE37 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE37, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_ALE38 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE38, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_ALE39 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE39, 28, 12));
                _BIST_PRINTF("MBIST_DRF_ALE25~32 have Fail\n");
            }
        }

        _BIST_REG_UPD(unit, MBIST_ALE_START_1, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============DRF_MACL1_TCM00~07======================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        _BIST_REG_UPD(unit, MBIST_ACL_START, 31, 0, 0xff);
        _bist_mdelay(100);

        if ((_BIST_REG_CHK(unit, MBIST_TCM00_ACL1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_TCM01_ACL1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_TCM02_ACL1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_TCM03_ACL1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_TCM04_ACL1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_TCM05_ACL1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_TCM06_ACL1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_TCM07_ACL1, 24, 24, 0x01)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_TCM00_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM00_ACL1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_TCM01_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM01_ACL1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_TCM02_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM02_ACL1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_TCM03_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM03_ACL1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_TCM04_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM04_ACL1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_TCM05_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM05_ACL1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_TCM06_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM06_ACL1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_TCM07_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM07_ACL1, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_ACL_RESUME, 31, 0, 0xff);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_ACL_RESUME, 31, 0, 0x0);
        }

        _bist_mdelay(100);
        if ((_BIST_REG_CHK(unit, MBIST_TCM00_ACL1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_TCM01_ACL1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_TCM02_ACL1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_TCM03_ACL1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_TCM04_ACL1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_TCM05_ACL1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_TCM06_ACL1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_TCM07_ACL1, 24, 24, 0x01)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_TCM00_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM00_ACL1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_TCM01_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM01_ACL1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_TCM02_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM02_ACL1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_TCM03_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM03_ACL1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_TCM04_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM04_ACL1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_TCM05_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM05_ACL1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_TCM06_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM06_ACL1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_TCM07_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM07_ACL1, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_ACL_RESUME, 31, 0, 0xff);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_ACL_RESUME, 31, 0, 0x0);
            _bist_mdelay(100);
            if ((_BIST_REG_CHK(unit, MBIST_TCM00_ACL1, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_TCM01_ACL1, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_TCM02_ACL1, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_TCM03_ACL1, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_TCM04_ACL1, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_TCM05_ACL1, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_TCM06_ACL1, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_TCM07_ACL1, 28, 12, 0x10000)))
            {
                _BIST_PRINTF(" wait 500ms MBIST_TCM00_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM00_ACL1, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_TCM01_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM01_ACL1, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_TCM02_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM02_ACL1, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_TCM03_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM03_ACL1, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_TCM04_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM04_ACL1, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_TCM05_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM05_ACL1, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_TCM06_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM06_ACL1, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_TCM07_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM07_ACL1, 28, 12));
                _BIST_PRINTF("MBIST_DRF_TCM00~07_ACL1 have Fail\n");
            }
        }

        _BIST_REG_UPD(unit, MBIST_ACL_START, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============DRF_MACL1_TCM08~15======================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        _BIST_REG_UPD(unit, MBIST_ACL_START, 31, 0, 0xff00);
        _bist_mdelay(100);

        if ((_BIST_REG_CHK(unit, MBIST_TCM08_ACL1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_TCM09_ACL1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_TCM10_ACL1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_TCM11_ACL1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_TCM12_ACL1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_TCM13_ACL1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_TCM14_ACL1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_TCM15_ACL1, 24, 24, 0x01)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_TCM08_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM08_ACL1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_TCM09_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM09_ACL1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_TCM10_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM10_ACL1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_TCM11_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM11_ACL1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_TCM12_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM12_ACL1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_TCM13_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM13_ACL1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_TCM14_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM14_ACL1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_TCM15_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM15_ACL1, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_ACL_RESUME, 31, 0, 0xff00);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_ACL_RESUME, 31, 0, 0x0);
        }

        _bist_mdelay(100);
        if ((_BIST_REG_CHK(unit, MBIST_TCM08_ACL1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_TCM09_ACL1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_TCM10_ACL1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_TCM11_ACL1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_TCM12_ACL1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_TCM13_ACL1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_TCM14_ACL1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_TCM15_ACL1, 24, 24, 0x01)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_TCM08_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM08_ACL1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_TCM09_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM09_ACL1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_TCM10_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM10_ACL1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_TCM11_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM11_ACL1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_TCM12_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM12_ACL1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_TCM13_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM13_ACL1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_TCM14_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM14_ACL1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_TCM15_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM15_ACL1, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_ACL_RESUME, 31, 0, 0xff00);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_ACL_RESUME, 31, 0, 0x0);
            _bist_mdelay(100);
            if ((_BIST_REG_CHK(unit, MBIST_TCM08_ACL1, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_TCM09_ACL1, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_TCM10_ACL1, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_TCM11_ACL1, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_TCM12_ACL1, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_TCM13_ACL1, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_TCM14_ACL1, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_TCM15_ACL1, 28, 12, 0x10000)))
            {
                _BIST_PRINTF(" wait 500ms MBIST_TCM08_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM08_ACL1, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_TCM09_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM09_ACL1, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_TCM10_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM10_ACL1, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_TCM11_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM11_ACL1, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_TCM12_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM12_ACL1, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_TCM13_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM13_ACL1, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_TCM14_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM14_ACL1, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_TCM15_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM15_ACL1, 28, 12));
                _BIST_PRINTF("MBIST_DRF_TCM08~15_ACL1 have Fail\n");
            }
        }

        _BIST_REG_UPD(unit, MBIST_ACL_START, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============DRF_MACL2_LOG00~03======================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        _BIST_REG_UPD(unit, MBIST_ACL_START, 31, 0, 0xf0000);
        _bist_mdelay(100);

        if ((_BIST_REG_CHK(unit, MBIST_LOG00_ACL2, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_LOG01_ACL2, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_LOG02_ACL2, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_LOG03_ACL2, 24, 24, 0x01)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_LOG00_ACL2 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_LOG00_ACL2, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_LOG01_ACL2 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_LOG01_ACL2, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_LOG02_ACL2 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_LOG02_ACL2, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_LOG03_ACL2 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_LOG03_ACL2, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_ACL_RESUME, 31, 0, 0xf0000);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_ACL_RESUME, 31, 0, 0x0);
        }

        _bist_mdelay(100);
        if ((_BIST_REG_CHK(unit, MBIST_LOG00_ACL2, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_LOG01_ACL2, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_LOG02_ACL2, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_LOG03_ACL2, 24, 24, 0x01)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_LOG00_ACL2 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_LOG00_ACL2, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_LOG01_ACL2 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_LOG01_ACL2, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_LOG02_ACL2 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_LOG02_ACL2, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_LOG03_ACL2 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_LOG03_ACL2, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_ACL_RESUME, 31, 0, 0xf0000);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_ACL_RESUME, 31, 0, 0x0);
            _bist_mdelay(100);
            if ((_BIST_REG_CHK(unit, MBIST_LOG00_ACL2, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_LOG01_ACL2, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_LOG02_ACL2, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_LOG03_ACL2, 28, 12, 0x10000)))
            {
                _BIST_PRINTF(" wait 500ms MBIST_LOG00_ACL2 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_LOG00_ACL2, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_LOG01_ACL2 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_LOG01_ACL2, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_LOG02_ACL2 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_LOG02_ACL2, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_LOG03_ACL2 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_LOG03_ACL2, 28, 12));
                _BIST_PRINTF("MBIST_DRF_LOG00~03_ACL2 have Fail\n");
            }
        }

        _BIST_REG_UPD(unit, MBIST_ACL_START, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============DRF_MACL3_ACT00~07======================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        _BIST_REG_UPD(unit, MBIST_ACL_START, 31, 0, 0xff00000);
        _bist_mdelay(100);

        if ((_BIST_REG_CHK(unit, MBIST_ACT00_ACL3, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ACT01_ACL3, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ACT02_ACL3, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ACT03_ACL3, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ACT04_ACL3, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ACT05_ACL3, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ACT06_ACL3, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ACT07_ACL3, 24, 24, 0x01)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_ACT00_ACL3 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ACT00_ACL3, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ACT01_ACL3 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ACT01_ACL3, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ACT02_ACL3 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ACT02_ACL3, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ACT03_ACL3 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ACT03_ACL3, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ACT04_ACL3 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ACT04_ACL3, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ACT05_ACL3 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ACT05_ACL3, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ACT06_ACL3 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ACT06_ACL3, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ACT07_ACL3 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ACT07_ACL3, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_ACL_RESUME, 31, 0, 0xff00000);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_ACL_RESUME, 31, 0, 0x0);
        }

        _bist_mdelay(100);
        if ((_BIST_REG_CHK(unit, MBIST_ACT00_ACL3, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ACT01_ACL3, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ACT02_ACL3, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ACT03_ACL3, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ACT04_ACL3, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ACT05_ACL3, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ACT06_ACL3, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ACT07_ACL3, 24, 24, 0x01)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_ACT00_ACL3 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ACT00_ACL3, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ACT01_ACL3 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ACT01_ACL3, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ACT02_ACL3 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ACT02_ACL3, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ACT03_ACL3 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ACT03_ACL3, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ACT04_ACL3 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ACT04_ACL3, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ACT05_ACL3 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ACT05_ACL3, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ACT06_ACL3 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ACT06_ACL3, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ACT07_ACL3 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ACT07_ACL3, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_ACL_RESUME, 31, 0, 0xff00000);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_ACL_RESUME, 31, 0, 0x0);
            _bist_mdelay(100);
            if ((_BIST_REG_CHK(unit, MBIST_ACT00_ACL3, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ACT01_ACL3, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ACT02_ACL3, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ACT03_ACL3, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ACT04_ACL3, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ACT05_ACL3, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ACT06_ACL3, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ACT07_ACL3, 28, 12, 0x10000)))
            {
                _BIST_PRINTF(" wait 500ms MBIST_ACT00_ACL3 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ACT00_ACL3, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_ACT01_ACL3 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ACT01_ACL3, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_ACT02_ACL3 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ACT02_ACL3, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_ACT03_ACL3 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ACT03_ACL3, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_ACT04_ACL3 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ACT04_ACL3, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_ACT05_ACL3 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ACT05_ACL3, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_ACT06_ACL3 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ACT06_ACL3, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_ACT07_ACL3 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ACT07_ACL3, 28, 12));
                _BIST_PRINTF("MBIST_DRF_LOG00~03_ACL2 have Fail\n");
            }
        }

        _BIST_REG_UPD(unit, MBIST_ACL_START, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_DRF_MIB 00~02======================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        _BIST_REG_UPD(unit, MBIST_MIB_START, 31, 0, 0x7);
        _bist_mdelay(100);

        if ((_BIST_REG_CHK(unit, MBIST_MIB00, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_MIB01, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_MIB02, 24, 24, 0x01)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_MIB00 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_MIB00, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_MIB01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_MIB01, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_MIB02 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_MIB02, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_MIB_RESUME, 31, 0, 0x7);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_MIB_RESUME, 31, 0, 0x0);
        }

        _bist_mdelay(100);
        if ((_BIST_REG_CHK(unit, MBIST_MIB00, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_MIB01, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_MIB02, 24, 24, 0x01)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_MIB00 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_MIB00, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_MIB01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_MIB01, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_MIB02 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_MIB02, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_MIB_RESUME, 31, 0, 0x7);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_MIB_RESUME, 31, 0, 0x0);
            _bist_mdelay(100);
            if ((_BIST_REG_CHK(unit, MBIST_MIB00, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_MIB01, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_MIB02, 28, 12, 0x10000)))
            {
                _BIST_PRINTF(" wait 500ms MBIST_MIB00 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_MIB00, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_MIB01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_MIB01, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_MIB02 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_MIB02, 28, 12));
                _BIST_PRINTF("MBIST_DRF_MIB00~02 have Fail\n");
            }
        }

        _BIST_REG_UPD(unit, MBIST_MIB_START, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_DRF_ENCAP test======================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        _BIST_REG_UPD(unit, MBIST_ENCAP_START, 31, 0, 0x3fC00);
        _bist_mdelay(100);

        if ((_BIST_REG_CHK(unit, MBIST_ENCAP10, 31, 24, 0x0f)) &&
            (_BIST_REG_CHK(unit, MBIST_ENCAP10, 23, 16, 0x0f)))
        {
            _BIST_PRINTF(" wait 500ms mbist_MPG1~4_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ENCAP10, 31, 24));
            _BIST_PRINTF(" wait 500ms mbist_MPT1~4_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ENCAP10, 23, 16));
        } else {
            _BIST_REG_UPD(unit, MBIST_ENCAP_RESUME, 31, 0, 0x3fc00);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_ENCAP_RESUME, 31, 0, 0x0);
        }

        _bist_mdelay(100);
        if ((_BIST_REG_CHK(unit, MBIST_ENCAP10, 31, 24, 0x0f)) &&
            (_BIST_REG_CHK(unit, MBIST_ENCAP10, 23, 16, 0x0f)))
        {
            _BIST_PRINTF(" wait 500ms mbist_MPG1~4_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ENCAP10, 31, 24));
            _BIST_PRINTF(" wait 500ms mbist_MPT1~4_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ENCAP10, 23, 16));
        } else {
            _BIST_REG_UPD(unit, MBIST_ENCAP_RESUME, 31, 0, 0x3fc00);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_ENCAP_RESUME, 31, 0, 0x0);
            _bist_mdelay(100);

            if ((_BIST_REG_CHK(unit, MBIST_ENCAP02, 31, 0, 0x0)) ||
                (_BIST_REG_CHK(unit, MBIST_ENCAP09, 31, 24, 0x55)))
            {
                _BIST_PRINTF("MBIST_DRF_ENCAP_MPG00~04 have Fail\n");
            }

            if ((_BIST_REG_CHK(unit, MBIST_ENCAP04, 31, 0, 0x0)) ||
                (_BIST_REG_CHK(unit, MBIST_ENCAP09, 23, 16, 0x55)))
            {
                _BIST_PRINTF("MBIST_DRF_ENCAP_MPT00~04 have Fail\n");
            }
        }

        _BIST_REG_UPD(unit, MBIST_ENCAP_START, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_DRF_ENCAP MPF & MPHF 1~4======================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        _BIST_REG_UPD(unit, MBIST_ENCAP_START, 31, 0, 0x3fC0000);
        _bist_mdelay(100);

        if ((_BIST_REG_CHK(unit, MBIST_ENCAP10, 15, 8, 0x0f)) &&
            (_BIST_REG_CHK(unit, MBIST_ENCAP10,  7, 0, 0x0f)))
        {
            _BIST_PRINTF(" wait 500ms mbist_MPF1~4_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ENCAP10, 15, 8));
            _BIST_PRINTF(" wait 500ms mbist_MPHF1~4_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ENCAP10, 7, 0));
        } else {
            _BIST_REG_UPD(unit, MBIST_ENCAP_RESUME, 31, 0, 0x3fC0000);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_ENCAP_RESUME, 31, 0, 0x0);
        }

        _bist_mdelay(100);
        if ((_BIST_REG_CHK(unit, MBIST_ENCAP10, 15,  8, 0x0f)) &&
            (_BIST_REG_CHK(unit, MBIST_ENCAP10,  7,  0, 0x0f)))
        {
            _BIST_PRINTF(" wait 500ms mbist_MPF1~4_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ENCAP10, 15,  8));
            _BIST_PRINTF(" wait 500ms mbist_MPHF1~4_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ENCAP10,  7,  0));
        } else {
            _BIST_REG_UPD(unit, MBIST_ENCAP_RESUME, 31, 0, 0x3fc0000);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_ENCAP_RESUME, 31, 0, 0x0);
            _bist_mdelay(100);

            if ((_BIST_REG_CHK(unit, MBIST_ENCAP06, 31, 0, 0x0)) ||
                (_BIST_REG_CHK(unit, MBIST_ENCAP09, 15, 8, 0x55)))
            {
                _BIST_PRINTF("MBIST_DRF_ENCAP_MPF00~04 have Fail\n");
            }

            if ((_BIST_REG_CHK(unit, MBIST_ENCAP08, 31, 0, 0x0)) ||
                (_BIST_REG_CHK(unit, MBIST_ENCAP09,  7, 0, 0x55)))
            {
                _BIST_PRINTF("MBIST_DRF_ENCAP_MPHF00~04 have Fail\n");
            }
        }

        _BIST_REG_UPD(unit, MBIST_ENCAP_START, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_DRF_ENCAP MTCAM======================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        _BIST_REG_UPD(unit, MBIST_ENCAP_START, 31, 0, 0x4000000);
        _bist_mdelay(100);

        if ((_BIST_REG_CHK(unit, MBIST_ENCAP_TCAM, 16, 16, 0x01)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_ENCAP_TCAM = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ENCAP_TCAM, 16, 16));
        } else {
            _BIST_REG_UPD(unit, MBIST_ENCAP_RESUME, 31, 0, 0x4000000);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_ENCAP_RESUME, 31, 0, 0x0);
        }

        _bist_mdelay(100);
        if ((_BIST_REG_CHK(unit, MBIST_ENCAP_TCAM, 16, 16, 0x01)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_ENCAP_TCAM = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ENCAP_TCAM, 16, 16));
        } else {
            _BIST_REG_UPD(unit, MBIST_ENCAP_RESUME, 31, 0, 0x4000000);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_ENCAP_RESUME, 31, 0, 0x0);
            _bist_mdelay(100);

            if ((_BIST_REG_CHK(unit, MBIST_ENCAP_TCAM, 31, 0, 0x1000000)))
            {
                _BIST_PRINTF("MBIST_DRF_ENCAP_MTCAM Fail\n");
            }
        }

        _BIST_REG_UPD(unit, MBIST_ENCAP_START, 31, 0, 0x0);
    }

    _BIST_PRINTF("===========MBIST_DRF_ENCAP MKELL & MRMK & & MEVC & MMPLS & MTS ==========\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        _BIST_REG_UPD(unit, MBIST_ENCAP_START, 31, 0, 0x7f);
        _bist_mdelay(100);

        if ((_BIST_REG_CHK(unit, MBIST_ENCAP13, 30, 30, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ENCAP13, 27, 20, 0x44)) &&
            (_BIST_REG_CHK(unit, MBIST_ENCAP13, 19, 12, 0x44)) &&
            (_BIST_REG_CHK(unit, MBIST_ENCAP13, 10, 10, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ENCAP13,  6,  6, 0x01)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_DRF_ENCAP_MKELL_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ENCAP13, 30, 30));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_ENCAP_MTS_pause = 0x%08X\n",   _BIST_REG_RET(unit, MBIST_ENCAP13, 27, 20));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_ENCAP_MRMK_pause = 0x%08X\n",  _BIST_REG_RET(unit, MBIST_ENCAP13, 19, 12));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_ENCAP_MEVC_pause = 0x%08X\n",  _BIST_REG_RET(unit, MBIST_ENCAP13, 10, 10));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_ENCAP_MPLS_pause = 0x%08X\n",  _BIST_REG_RET(unit, MBIST_ENCAP13,  6,  6));
        } else {
            _BIST_REG_UPD(unit, MBIST_ENCAP_RESUME, 31, 0, 0x7f);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_ENCAP_RESUME, 31, 0, 0x0);
        }

        _bist_mdelay(100);
        if ((_BIST_REG_CHK(unit, MBIST_ENCAP13, 30, 30, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ENCAP13, 27, 20, 0x44)) &&
            (_BIST_REG_CHK(unit, MBIST_ENCAP13, 19, 12, 0x44)) &&
            (_BIST_REG_CHK(unit, MBIST_ENCAP13, 10, 10, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ENCAP13,  6,  6, 0x01)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_DRF_ENCAP_MKELL_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ENCAP13, 30, 30));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_ENCAP_MTS_pause = 0x%08X\n",   _BIST_REG_RET(unit, MBIST_ENCAP13, 27, 20));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_ENCAP_MRMK_pause = 0x%08X\n",  _BIST_REG_RET(unit, MBIST_ENCAP13, 19, 12));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_ENCAP_MEVC_pause = 0x%08X\n",  _BIST_REG_RET(unit, MBIST_ENCAP13, 10, 10));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_ENCAP_MPLS_pause = 0x%08X\n",  _BIST_REG_RET(unit, MBIST_ENCAP13,  6,  6));
        } else {
            _BIST_REG_UPD(unit, MBIST_ENCAP_RESUME, 31, 0, 0x7f);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_ENCAP_RESUME, 31, 0, 0x0);
            _bist_mdelay(100);

            if ((_BIST_REG_CHK(unit, MBIST_ENCAP12, 28, 24, 0x0)) ||
                (_BIST_REG_CHK(unit, MBIST_ENCAP13, 30, 28, 0x1)))
            {
                _BIST_PRINTF("MBIST_DRF_ENCAP_MKELL Fail\n");
            }

            if ((_BIST_REG_CHK(unit, MBIST_ENCAP12, 21, 16, 0x0)) ||
                (_BIST_REG_CHK(unit, MBIST_ENCAP13, 27, 20, 0x11)))
            {
                _BIST_PRINTF("MBIST_DRF_ENCAP_MTS Fail\n");
            }

            if ((_BIST_REG_CHK(unit, MBIST_ENCAP12, 13, 12, 0x0)) ||
                (_BIST_REG_CHK(unit, MBIST_ENCAP13, 19, 12, 0x11)))
            {
                _BIST_PRINTF("MBIST_DRF_ENCAP_MRMK Fail\n");
            }

            if ((_BIST_REG_CHK(unit, MBIST_ENCAP12,  8,  8, 0x0)) ||
                (_BIST_REG_CHK(unit, MBIST_ENCAP13, 11,  8, 0x1)))
            {
                _BIST_PRINTF("MBIST_DRF_ENCAP_MEVC Fail\n");
            }


            if ((_BIST_REG_CHK(unit, MBIST_ENCAP12,  4,  4, 0x0)) ||
                (_BIST_REG_CHK(unit, MBIST_ENCAP13,  7,  4, 0x1)))
            {
                _BIST_PRINTF("MBIST_DRF_ENCAP_MMPLS Fail\n");
            }
        }

        _BIST_REG_UPD(unit, MBIST_ENCAP_START, 31, 0, 0x0);
    }

    _BIST_PRINTF("================MBIST_DRF_ENCAP MBS & MUNTAG==============\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        _BIST_REG_UPD(unit, MBIST_ENCAP_START, 31, 0, 0x380);
        _bist_mdelay(100);

        if ((_BIST_REG_CHK(unit, MBIST_ENCAP13,  2,  2, 0x01)) &&
            (_BIST_REG_CHK(unit, MBISR_ENCAP01, 16, 16, 0x01)) &&
            (_BIST_REG_CHK(unit, MBISR_ENCAP02, 16, 16, 0x01)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_DRF_ENCAP_MBS0_pause = 0x%08X\n",   _BIST_REG_RET(unit, MBIST_ENCAP13,  2,  2));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_ENCAP_MBS1_pause = 0x%08X\n",   _BIST_REG_RET(unit, MBISR_ENCAP01, 16, 16));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_ENCAP_MUNTAG_pause = 0x%08X\n", _BIST_REG_RET(unit, MBISR_ENCAP02, 16, 16));
        } else {
            _BIST_REG_UPD(unit, MBIST_ENCAP_RESUME, 31, 0, 0x380);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_ENCAP_RESUME, 31, 0, 0x0);
        }

        _bist_mdelay(100);
        if ((_BIST_REG_CHK(unit, MBIST_ENCAP13,  2,  2, 0x01)) &&
            (_BIST_REG_CHK(unit, MBISR_ENCAP01, 16, 16, 0x01)) &&
            (_BIST_REG_CHK(unit, MBISR_ENCAP02, 16, 16, 0x01)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_DRF_ENCAP_MBS0_pause = 0x%08X\n",   _BIST_REG_RET(unit, MBIST_ENCAP13,  2,  2));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_ENCAP_MBS1_pause = 0x%08X\n",   _BIST_REG_RET(unit, MBISR_ENCAP01, 16, 16));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_ENCAP_MUNTAG_pause = 0x%08X\n", _BIST_REG_RET(unit, MBISR_ENCAP02, 16, 16));
        } else {
            _BIST_REG_UPD(unit, MBIST_ENCAP_RESUME, 31, 0, 0x380);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_ENCAP_RESUME, 31, 0, 0x0);
            _bist_mdelay(100);

            if ((_BIST_REG_CHK(unit, MBIST_ENCAP13,  3,  0, 0x1)) ||
                (_BIST_REG_CHK(unit, MBIST_ENCAP12,  0,  0, 0x0)))
            {
                _BIST_PRINTF("MBIST_ENCAP_MBS Fail\n");
            }

            if ((_BIST_REG_CHK(unit, MBISR_ENCAP01, 24,  8, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBISR_ENCAP02, 24,  8, 0x10000)))
            {
                _BIST_PRINTF("MBIST_ENCAP_MUNTAG Fail\n");
            }
        }

        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x0);
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x0);
        _BIST_REG_UPD(unit, MBIST_ENCAP_START, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_DRF_PAR 00~03==================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        _BIST_REG_UPD(unit, MBIST_PAR_START, 31, 0, 0xf);
        _bist_mdelay(100);

        if ((_BIST_REG_CHK(unit, MBIST_PAR01_0, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PAR01_1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PAR02,   24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PAR03,   24, 24, 0x01)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_DRF_PAR01_0_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PAR01_0, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_PAR01_1_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PAR01_1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_PAR02_pause = 0x%08X\n",   _BIST_REG_RET(unit, MBIST_PAR02, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_PAR03_pause = 0x%08X\n",   _BIST_REG_RET(unit, MBIST_PAR03, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_PAR_RESUME, 31, 0, 0xf);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_PAR_RESUME, 31, 0, 0x0);
        }

        _bist_mdelay(100);
        if ((_BIST_REG_CHK(unit, MBIST_PAR01_0, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PAR01_1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PAR02,   24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PAR03,   24, 24, 0x01)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_DRF_PAR01_0_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PAR01_0, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_PAR01_1_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PAR01_1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_PAR02_pause = 0x%08X\n",   _BIST_REG_RET(unit, MBIST_PAR02, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_PAR03_pause = 0x%08X\n",   _BIST_REG_RET(unit, MBIST_PAR03, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_PAR_RESUME, 31, 0, 0xf);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_PAR_RESUME, 31, 0, 0x0);
            _bist_mdelay(100);

            if ((_BIST_REG_CHK(unit, MBIST_PAR01_0, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PAR01_0, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PAR02, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PAR03, 28, 12, 0x10000)))
            {
                _BIST_PRINTF("MBIST_DRF_PAR00~03 have Fail\n");
            }
        }

        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x0);
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x0);
        _BIST_REG_UPD(unit, MBIST_PAR_START, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_DRF_IGR 01~05==================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        _BIST_REG_UPD(unit, MBIST_IGR_START, 31, 0, 0x3f);
        _bist_mdelay(100);

        if ((_BIST_REG_CHK(unit, MBIST_IGR01_0, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_IGR01_1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_IGR02,   24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_IGR03,   24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_IGR04,   24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_IGR05,   24, 24, 0x01)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_DRF_IGR01_0_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_IGR01_0, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_IGR01_1_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_IGR01_1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_IGR02_pause = 0x%08X\n",   _BIST_REG_RET(unit, MBIST_IGR02, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_IGR03_pause = 0x%08X\n",   _BIST_REG_RET(unit, MBIST_IGR03, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_IGR04_pause = 0x%08X\n",   _BIST_REG_RET(unit, MBIST_IGR04, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_IGR05_pause = 0x%08X\n",   _BIST_REG_RET(unit, MBIST_IGR05, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_IGR_RESUME, 31, 0, 0x3f);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_IGR_RESUME, 31, 0, 0x0);
        }

        _bist_mdelay(100);
        if ((_BIST_REG_CHK(unit, MBIST_IGR01_0, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_IGR01_1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_IGR02,   24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_IGR03,   24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_IGR04,   24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_IGR05,   24, 24, 0x01)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_DRF_IGR01_0_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_IGR01_0, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_IGR01_1_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_IGR01_1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_IGR02_pause = 0x%08X\n",   _BIST_REG_RET(unit, MBIST_IGR02, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_IGR03_pause = 0x%08X\n",   _BIST_REG_RET(unit, MBIST_IGR03, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_IGR04_pause = 0x%08X\n",   _BIST_REG_RET(unit, MBIST_IGR04, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_IGR05_pause = 0x%08X\n",   _BIST_REG_RET(unit, MBIST_IGR05, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_IGR_RESUME, 31, 0, 0x3f);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_IGR_RESUME, 31, 0, 0x0);
            _bist_mdelay(100);

            if ((_BIST_REG_CHK(unit, MBIST_IGR01_0, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_IGR01_1, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_IGR02, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_IGR03, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_IGR04, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_IGR05, 28, 12, 0x10000)))
            {
                _BIST_PRINTF("MBIST_DRF_IGR01~05 have Fail\n");
            }
        }

        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x0);
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x0);
        _BIST_REG_UPD(unit, MBIST_IGR_START, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_DRF_IGR 01~07==================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        _BIST_REG_UPD(unit, MBIST_IGR_START, 31, 0, 0xfc0);
        _bist_mdelay(100);

        if ((_BIST_REG_CHK(unit, MBIST_IGR06_0, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_IGR06_1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_IGR06_2, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_IGR06_3, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_IGR06_4, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_IGR07,   24, 24, 0x01)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_DRF_IGR06_0_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_IGR06_0, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_IGR06_1_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_IGR06_1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_IGR06_2_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_IGR06_2, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_IGR06_3_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_IGR06_3, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_IGR06_4_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_IGR06_4, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_IGR07_pause = 0x%08X\n",   _BIST_REG_RET(unit, MBIST_IGR07, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_IGR_RESUME, 31, 0, 0xfc0);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_IGR_RESUME, 31, 0, 0x0);
        }

        _bist_mdelay(100);
        if ((_BIST_REG_CHK(unit, MBIST_IGR06_0, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_IGR06_1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_IGR06_2, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_IGR06_3, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_IGR06_4, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_IGR07,   24, 24, 0x01)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_DRF_IGR06_0_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_IGR06_0, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_IGR06_1_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_IGR06_1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_IGR06_2_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_IGR06_2, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_IGR06_3_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_IGR06_3, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_IGR06_4_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_IGR06_4, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_IGR07_pause = 0x%08X\n",   _BIST_REG_RET(unit, MBIST_IGR07, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_IGR_RESUME, 31, 0, 0xfc0);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_IGR_RESUME, 31, 0, 0x0);
            _bist_mdelay(100);

            if ((_BIST_REG_CHK(unit, MBIST_IGR06_0, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_IGR06_1, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_IGR06_2, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_IGR06_3, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_IGR06_4, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_IGR07, 28, 12, 0x10000)))
            {
                _BIST_PRINTF("MBIST_DRF_IGR06~07 have Fail\n");
            }
        }

        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x0);
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x0);
        _BIST_REG_UPD(unit, MBIST_IGR_START, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_DRF_EGR =======================\n");

    _BIST_PRINTF("==================EGR00~07========================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        _BIST_REG_UPD(unit, MBIST_EGR_START_0, 31, 0, 0x1fe0);
        _bist_mdelay(100);

        if ((_BIST_REG_CHK(unit, MBIST_EGR18, 12,  5, 0xff)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_DRF_EGR00~07_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_EGR18, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_EGR_RESUME_0, 31, 0, 0x1fe0);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_EGR_RESUME_0, 31, 0, 0x0);
        }

        _bist_mdelay(100);
        if ((_BIST_REG_CHK(unit, MBIST_EGR18, 12,  5, 0xff)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_DRF_EGR00~07_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_EGR18, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_EGR_RESUME_0, 31, 0, 0x1fe0);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_EGR_RESUME_0, 31, 0, 0x0);
            _bist_mdelay(100);

            if ((_BIST_REG_CHK(unit, MBIST_EGR06, 23,  8, 0x0)) ||
                (_BIST_REG_CHK(unit, MBIST_EGR14, 12,  5, 0xff)))
            {
                _BIST_PRINTF("MBIST_DRF_EGR01~07 have Fail\n");
            }
        }

        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x0);
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x0);
        _BIST_REG_UPD(unit, MBIST_EGR_START_0, 31, 0, 0x0);
    }

    _BIST_PRINTF("==================EGR08~11========================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        _BIST_REG_UPD(unit, MBIST_EGR_START_0, 31, 0, 0x1e000);
        _bist_mdelay(100);

        if ((_BIST_REG_CHK(unit, MBIST_EGR18, 16, 13, 0x0f)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_DRF_EGR08~11_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_EGR18, 16, 13));
        } else {
            _BIST_REG_UPD(unit, MBIST_EGR_RESUME_0, 31, 0, 0x1e000);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_EGR_RESUME_0, 31, 0, 0x0);
        }

        _bist_mdelay(100);
        if ((_BIST_REG_CHK(unit, MBIST_EGR18, 16, 13, 0x0f)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_DRF_EGR08~11_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_EGR18, 16, 13));
        } else {
            _BIST_REG_UPD(unit, MBIST_EGR_RESUME_0, 31, 0, 0x1e000);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_EGR_RESUME_0, 31, 0, 0x0);
            _bist_mdelay(100);

            if ((_BIST_REG_CHK(unit, MBIST_EGR06, 31, 24, 0x0)) ||
                (_BIST_REG_CHK(unit, MBIST_EGR14, 16, 13, 0xff)))
            {
                _BIST_PRINTF("MBIST_DRF_EGR08~11 have Fail\n");
            }
        }

        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x0);
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x0);
        _BIST_REG_UPD(unit, MBIST_EGR_START_0, 31, 0, 0x0);
    }

    _BIST_PRINTF("==================EGR12~19========================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        _BIST_REG_UPD(unit, MBIST_EGR_START_0, 31, 0, 0x1fe0000);
        _bist_mdelay(100);

        if ((_BIST_REG_CHK(unit, MBIST_EGR18, 24, 17, 0xff)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_DRF_EGR12~19_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_EGR18, 24, 17));
        } else {
            _BIST_REG_UPD(unit, MBIST_EGR_RESUME_0, 31, 0, 0x1fe0000);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_EGR_RESUME_0, 31, 0, 0x0);
        }

        _bist_mdelay(100);
        if ((_BIST_REG_CHK(unit, MBIST_EGR18, 24, 17, 0xff)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_DRF_EGR12~19_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_EGR18, 24, 17));
        } else {
            _BIST_REG_UPD(unit, MBIST_EGR_RESUME_0, 31, 0, 0x1fe0000);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_EGR_RESUME_0, 31, 0, 0x0);
            _bist_mdelay(100);

            if ((_BIST_REG_CHK(unit, MBIST_EGR07, 15, 0, 0x0)) ||
                (_BIST_REG_CHK(unit, MBIST_EGR14, 24, 17, 0xff)))
            {
                _BIST_PRINTF("MBIST_DRF_EGR12~19 have Fail\n");
            }
        }

        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x0);
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x0);
        _BIST_REG_UPD(unit, MBIST_EGR_START_0, 31, 0, 0x0);
    }

    _BIST_PRINTF("==================EGR20~27========================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        _BIST_REG_UPD(unit, MBIST_EGR_START_0, 31, 0, 0xfe000000);
        _BIST_REG_UPD(unit, MBIST_EGR_START_1, 31, 0, 0x1);
        _bist_mdelay(100);

        if ((_BIST_REG_CHK(unit, MBIST_EGR18, 31, 25, 0x7f)) &&
            (_BIST_REG_CHK(unit, MBIST_EGR19,  0,  0, 0x1)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_DRF_EGR20~26_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_EGR18, 31, 25));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_EGR27_pause = 0x%08X\n",    _BIST_REG_RET(unit, MBIST_EGR19,  0,  0));
        } else {
            _BIST_REG_UPD(unit, MBIST_EGR_RESUME_0, 31, 0, 0xfe000000);
            _BIST_REG_UPD(unit, MBIST_EGR_RESUME_1, 31, 0, 0x1);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_EGR_RESUME_0, 31, 0, 0x0);
            _BIST_REG_UPD(unit, MBIST_EGR_RESUME_1, 31, 0, 0x0);
        }

        _bist_mdelay(100);
        if ((_BIST_REG_CHK(unit, MBIST_EGR18, 31, 25, 0x7f)) &&
            (_BIST_REG_CHK(unit, MBIST_EGR19,  0,  0, 0x1)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_DRF_EGR20~26_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_EGR18, 31, 25));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_EGR27_pause = 0x%08X\n",    _BIST_REG_RET(unit, MBIST_EGR19,  0,  0));
        } else {
            _BIST_REG_UPD(unit, MBIST_EGR_RESUME_0, 31, 0, 0xfe000000);
            _BIST_REG_UPD(unit, MBIST_EGR_RESUME_1, 31, 0, 0x1);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_EGR_RESUME_0, 31, 0, 0x0);
            _BIST_REG_UPD(unit, MBIST_EGR_RESUME_1, 31, 0, 0x0);
            _bist_mdelay(100);

            if ((_BIST_REG_CHK(unit, MBIST_EGR07, 31, 16, 0x0)) ||
                (_BIST_REG_CHK(unit, MBIST_EGR14, 31, 25, 0x7f)) ||
                (_BIST_REG_CHK(unit, MBIST_EGR15,  0,  0, 0x1)))
            {
                _BIST_PRINTF("MBIST_DRF_EGR20~27 have Fail\n");
            }
        }

        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x0);
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x0);
        _BIST_REG_UPD(unit, MBIST_EGR_START_0, 31, 0, 0x0);
        _BIST_REG_UPD(unit, MBIST_EGR_START_1, 31, 0, 0x0);
    }

    _BIST_PRINTF("==================EGR28~35========================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        _BIST_REG_UPD(unit, MBIST_EGR_START_1, 31, 0, 0x1fe);
        _bist_mdelay(100);

        if ((_BIST_REG_CHK(unit, MBIST_EGR19,  8,  1, 0xff)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_DRF_EGR28~35_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_EGR19,  8,  1));
        } else {
            _BIST_REG_UPD(unit, MBIST_EGR_RESUME_1, 31, 0, 0x1fe);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_EGR_RESUME_1, 31, 0, 0x0);
        }

        _bist_mdelay(100);
        if ((_BIST_REG_CHK(unit, MBIST_EGR19,  8,  1, 0xff)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_DRF_EGR28~35_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_EGR19,  8,  1));
        } else {
            _BIST_REG_UPD(unit, MBIST_EGR_RESUME_1, 31, 0, 0x1fe);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_EGR_RESUME_1, 31, 0, 0x0);
            _bist_mdelay(100);

            if ((_BIST_REG_CHK(unit, MBIST_EGR08, 15,  0, 0x0)) ||
                (_BIST_REG_CHK(unit, MBIST_EGR15,  8,  1, 0xff)))
            {
                _BIST_PRINTF("MBIST_DRF_EGR28~35 have Fail\n");
            }
        }

        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x0);
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x0);
        _BIST_REG_UPD(unit, MBIST_EGR_START_1, 31, 0, 0x0);
    }

    _BIST_PRINTF("==================EGR36~43========================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        _BIST_REG_UPD(unit, MBIST_EGR_START_1, 31, 0, 0x1fe00);
        _bist_mdelay(100);

        if ((_BIST_REG_CHK(unit, MBIST_EGR19, 16,  9, 0xff)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_DRF_EGR36~43_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_EGR19, 16,  9));
        } else {
            _BIST_REG_UPD(unit, MBIST_EGR_RESUME_1, 31, 0, 0x1fe00);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_EGR_RESUME_1, 31, 0, 0x0);
        }

        _bist_mdelay(100);
        if ((_BIST_REG_CHK(unit, MBIST_EGR19, 16,  9, 0xff)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_DRF_EGR36~43_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_EGR19, 16,  9));
        } else {
            _BIST_REG_UPD(unit, MBIST_EGR_RESUME_1, 31, 0, 0x1fe00);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_EGR_RESUME_1, 31, 0, 0x0);
            _bist_mdelay(100);

            if ((_BIST_REG_CHK(unit, MBIST_EGR08, 31, 16, 0x0)) ||
                (_BIST_REG_CHK(unit, MBIST_EGR15, 16,  9, 0xff)))
            {
                _BIST_PRINTF("MBIST_DRF_EGR36~43 have Fail\n");
            }
        }

        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x0);
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x0);
        _BIST_REG_UPD(unit, MBIST_EGR_START_1, 31, 0, 0x0);
    }

    _BIST_PRINTF("==================EGR44~51========================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        _BIST_REG_UPD(unit, MBIST_EGR_START_1, 31, 0, 0x1fe0000);
        _bist_mdelay(100);

        if ((_BIST_REG_CHK(unit, MBIST_EGR19, 24, 17, 0xff)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_DRF_EGR44~51_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_EGR19, 24, 17));
        } else {
            _BIST_REG_UPD(unit, MBIST_EGR_RESUME_1, 31, 0, 0x1fe0000);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_EGR_RESUME_1, 31, 0, 0x0);
        }

        _bist_mdelay(100);
        if ((_BIST_REG_CHK(unit, MBIST_EGR19, 24, 17, 0xff)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_DRF_EGR44~51_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_EGR19, 24, 17));
        } else {
            _BIST_REG_UPD(unit, MBIST_EGR_RESUME_1, 31, 0, 0x1fe0000);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_EGR_RESUME_1, 31, 0, 0x0);
            _bist_mdelay(100);

            if ((_BIST_REG_CHK(unit, MBIST_EGR09, 15,  0, 0x0)) ||
                (_BIST_REG_CHK(unit, MBIST_EGR15, 24, 17, 0xff)))
            {
                _BIST_PRINTF("MBIST_DRF_EGR44~51 have Fail\n");
            }
        }

        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x0);
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x0);
        _BIST_REG_UPD(unit, MBIST_EGR_START_1, 31, 0, 0x0);
    }

    _BIST_PRINTF("==================EGR52~57========================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        _BIST_REG_UPD(unit, MBIST_EGR_START_1, 31, 0, 0x7e000000);
        _bist_mdelay(100);

        if ((_BIST_REG_CHK(unit, MBIST_EGR19, 30, 25, 0x3f)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_DRF_EGR52~57_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_EGR19, 30, 25));
        } else {
            _BIST_REG_UPD(unit, MBIST_EGR_RESUME_1, 31, 0, 0x7e000000);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_EGR_RESUME_1, 31, 0, 0x0);
        }

        _bist_mdelay(100);
        if ((_BIST_REG_CHK(unit, MBIST_EGR19, 30, 25, 0x3f)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_DRF_EGR52~57_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_EGR19, 30, 25));
        } else {
            _BIST_REG_UPD(unit, MBIST_EGR_RESUME_1, 31, 0, 0x7e000000);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_EGR_RESUME_1, 31, 0, 0x0);
            _bist_mdelay(100);

            if ((_BIST_REG_CHK(unit, MBIST_EGR09, 31, 16, 0x0)) ||
                (_BIST_REG_CHK(unit, MBIST_EGR15, 30, 25, 0x3f)))
            {
                _BIST_PRINTF("MBIST_DRF_EGR52~57 have Fail\n");
            }
        }

        _BIST_REG_UPD(unit, MBIST_EGR_START_1, 31, 0, 0x0);
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x0);
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x0);
    }

    _BIST_PRINTF("==================EGR_MDRC_CC & MPORT & MPORT_EXT & MLINK========================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        _BIST_REG_UPD(unit, MBIST_EGR_START_0, 31, 0, 0x1e);
        _bist_mdelay(100);

        if ((_BIST_REG_CHK(unit, MBIST_EGR18,  4,  1, 0x0f)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_DRF_MDRC_CC & MPORT & MPORT_EXT & MLINK_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_EGR18,  4,  1));
        } else {
            _BIST_REG_UPD(unit, MBIST_EGR_RESUME_0, 31, 0, 0x1e);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_EGR_RESUME_0, 31, 0, 0x0);
        }

        _bist_mdelay(100);
        if ((_BIST_REG_CHK(unit, MBIST_EGR18,  4,  1, 0x0f)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_DRF_MDRC_CC & MPORT & MPORT_EXT & MLINK_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_EGR18,  4,  1));
        } else {
            _BIST_REG_UPD(unit, MBIST_EGR_RESUME_0, 31, 0, 0x1e);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_EGR_RESUME_0, 31, 0, 0x0);
            _bist_mdelay(100);

            if ((_BIST_REG_CHK(unit, MBIST_EGR06,  5,  1, 0x0)) ||
                (_BIST_REG_CHK(unit, MBIST_EGR14,  4,  1, 0xf)))
            {
                _BIST_PRINTF("MBIST_DRF_MDRC_CC & MPORT & MPORT_EXT & MLINK have Fail\n");
            }
        }

        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x0);
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x0);
        _BIST_REG_UPD(unit, MBIST_EGR_START_0, 31, 0, 0x0);
    }

    return CPARSER_OK;
#else
    uint32 unit;
    uint32 master_view_unit;

    DIAG_UTIL_FUNC_INIT(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    //osal_printf("MODEL_NAME_INFO = 0x%08X\n", _BIST_REG_RET(unit, MODEL_NAME_INFO, 31, 0));

    _BIST_PRINTF("==============MAC_MBIST===========================\n");

    _BIST_PRINTF("==============MBIST_PG0 & PG1=====================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        _BIST_REG_UPD(unit, MBIST_MAC_RESET_00, 31, 0, (0x16b00000 | 0x5ac00 | 0x16b));
        _bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_PG00_RX_MAC01, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG00_RX_MAC05, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG00_TX_MAC01, 31, 0, 0x20000000)))
        {
            _BIST_PRINTF("MBIST_PG0 Fail\n");
        }

        if ((_BIST_REG_CHK(unit, MBIST_PG01_RX_MAC01, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG01_RX_MAC05, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG01_TX_MAC01, 31, 0, 0x20000000)))
        {
            _BIST_PRINTF("MBIST_PG1 Fail\n");
        }

        //_BIST_REG_UPD(unit, MBIST_MAC_RESET_00, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_PG2 & PG3=====================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        //_BIST_REG_UPD(unit, MBIST_MAC_RESET_00, 31, 0, 0x5ac00);
        //_bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_PG02_RX_MAC01, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG02_RX_MAC05, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG02_TX_MAC01, 31, 0, 0x20000000)))
        {
            _BIST_PRINTF("MBIST_PG2 Fail\n");
        }

        if ((_BIST_REG_CHK(unit, MBIST_PG03_RX_MAC01, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG03_RX_MAC05, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG03_TX_MAC01, 31, 0, 0x20000000)))
        {
            _BIST_PRINTF("MBIST_PG3 Fail\n");
        }

        //_BIST_REG_UPD(unit, MBIST_MAC_RESET_00, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_PG4 & PG5=====================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        //_BIST_REG_UPD(unit, MBIST_MAC_RESET_00, 31, 0, 0x16b00000);
        //_bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_PG04_RX_MAC01, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG04_RX_MAC05, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG04_TX_MAC01, 31, 0, 0x20000000)))
        {
            _BIST_PRINTF("MBIST_PG4 Fail\n");
        }

        if ((_BIST_REG_CHK(unit, MBIST_PG05_RX_MAC01, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG05_RX_MAC05, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG05_TX_MAC01, 31, 0, 0x20000000)))
        {
            _BIST_PRINTF("MBIST_PG5 Fail\n");
        }

        _BIST_REG_UPD(unit, MBIST_MAC_RESET_00, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_PG6===========================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        _BIST_REG_UPD(unit, MBIST_MAC_RESET_01, 31, 0, (0x39c000 | 0x3380 | 0x67));
        _bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_PG06_RX_MAC08, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG06_RX_MAC04, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG06_RX_MAC06, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG06_TX_MAC08, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG06_TX_MAC02, 31, 0, 0x20000000)))
        {
            _BIST_PRINTF("MBIST_PG6 Fail\n");
        }

        //_BIST_REG_UPD(unit, MBIST_MAC_RESET_01, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_PG7===========================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        //_BIST_REG_UPD(unit, MBIST_MAC_RESET_01, 31, 0, 0x3380);
        //_bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_PG07_RX_MAC08, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG07_RX_MAC04, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG07_RX_MAC06, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG07_TX_MAC08, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG07_TX_MAC02, 31, 0, 0x20000000)))
        {
            _BIST_PRINTF("MBIST_PG7 Fail\n");
        }

        //_BIST_REG_UPD(unit, MBIST_MAC_RESET_01, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_PG8===========================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        _BIST_REG_UPD(unit, MBIST_MAC_RESET_02, 31, 0, (0xdc0000 | 0x37000 | 0xdc0 | 0x37));
        _bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_PG08_RX_MAC10, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG08_RX_MAC07, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG08_RX_MAC11, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG08_TX_MAC10, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG08_TX_MAC12, 31, 0, 0x20000000)))
        {
            _BIST_PRINTF("MBIST_PG8 Fail\n");
        }

        //_BIST_REG_UPD(unit, MBIST_MAC_RESET_02, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_PG9===========================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        //_BIST_REG_UPD(unit, MBIST_MAC_RESET_02, 31, 0, 0xdc0);
        //_bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_PG09_RX_MAC10, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG09_RX_MAC07, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG09_RX_MAC11, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG09_TX_MAC10, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG09_TX_MAC12, 31, 0, 0x20000000)))
        {
            _BIST_PRINTF("MBIST_PG9 Fail\n");
        }

        //_BIST_REG_UPD(unit, MBIST_MAC_RESET_02, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_PG10===========================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        //_BIST_REG_UPD(unit, MBIST_MAC_RESET_02, 31, 0, 0x37000);
        //_bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_PG10_RX_MAC10, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG10_RX_MAC07, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG10_RX_MAC11, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG10_TX_MAC10, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG10_TX_MAC12, 31, 0, 0x20000000)))
        {
            _BIST_PRINTF("MBIST_PG10 Fail\n");
        }

        //_BIST_REG_UPD(unit, MBIST_MAC_RESET_02, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_PG11===========================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        //_BIST_REG_UPD(unit, MBIST_MAC_RESET_02, 31, 0, 0xdc0000);
        //_bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_PG11_RX_MAC10, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG11_RX_MAC07, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG11_RX_MAC11, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG11_TX_MAC10, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG11_TX_MAC12, 31, 0, 0x20000000)))
        {
            _BIST_PRINTF("MBIST_PG11 Fail\n");
        }

        _BIST_REG_UPD(unit, MBIST_MAC_RESET_02, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_PG12===========================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        //_BIST_REG_UPD(unit, MBIST_MAC_RESET_01, 31, 0, 0x39c000);
        //_bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_PG12_RX_MAC03, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG12_RX_MAC09, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG12_RX_MAC07, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG12_TX_MAC02, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG12_PG_MAC13, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PG12_PG_MAC14, 31, 0, 0x20000000)))
        {
            _BIST_PRINTF("MBIST_PG12 Fail\n");
        }

        _BIST_REG_UPD(unit, MBIST_MAC_RESET_01, 31, 0, 0x0);
    }

    /*
     * ALE
     */

    _BIST_PRINTF("==============ALE_MBIST===========================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        _BIST_REG_UPD(unit, MBIST_ALE_START_0, 31, 0, (0xff000000 | 0xff0000 | 0xff00 | 0xff));
        _bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_ALE01, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ALE02, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ALE03, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ALE04, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ALE05, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ALE06, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ALE07, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ALE08, 31, 0, 0x20000000)))
        {
            _BIST_REG_ERR(unit, MBIST_ALE01, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ALE02, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ALE03, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ALE04, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ALE05, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ALE06, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ALE07, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ALE08, 31, 0, 0x20000000);
            _BIST_PRINTF("MBIST_ALE01~08 have Fail\n");
        }

        //_BIST_REG_UPD(unit, MBIST_ALE_START_0, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_MALE9~16======================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        //_BIST_REG_UPD(unit, MBIST_ALE_START_0, 31, 0, 0xff00);
        //_bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_ALE09, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ALE10, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ALE011, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ALE012, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ALE13, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ALE14, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ALE15, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ALE16, 31, 0, 0x20000000)))
        {
            _BIST_REG_ERR(unit, MBIST_ALE09, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ALE10, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ALE011, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ALE012, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ALE13, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ALE14, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ALE15, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ALE16, 31, 0, 0x20000000);
            _BIST_PRINTF("MBIST_ALE09~16 have Fail\n");
        }

        //_BIST_REG_UPD(unit, MBIST_ALE_START_0, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_MALE17~24======================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        //_BIST_REG_UPD(unit, MBIST_ALE_START_0, 31, 0, 0xff0000);
        //_bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_ALE17, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ALE18, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ALE19, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ALE20, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ALE21, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ALE22, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ALE23, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ALE24, 31, 0, 0x20000000)))
        {
            _BIST_REG_ERR(unit, MBIST_ALE17, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ALE18, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ALE19, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ALE20, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ALE21, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ALE22, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ALE23, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ALE24, 31, 0, 0x20000000);
            _BIST_PRINTF("MBIST_ALE17~24 have Fail\n");
        }

        //_BIST_REG_UPD(unit, MBIST_ALE_START_0, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_MALE25~32======================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        //_BIST_REG_UPD(unit, MBIST_ALE_START_0, 31, 0, 0xff000000);
        //_bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_ALE25, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ALE26, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ALE27, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ALE28, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ALE29, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ALE30, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ALE31, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ALE32, 31, 0, 0x20000000)))
        {
            _BIST_REG_ERR(unit, MBIST_ALE25, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ALE26, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ALE27, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ALE28, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ALE29, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ALE30, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ALE31, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ALE32, 31, 0, 0x20000000);
            _BIST_PRINTF("MBIST_ALE25~32 have Fail\n");
        }

        _BIST_REG_UPD(unit, MBIST_ALE_START_0, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_MALE33~39======================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        _BIST_REG_UPD(unit, MBIST_ALE_START_1, 31, 0, 0x7f);
        _bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_ALE33, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ALE34, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ALE35, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ALE36, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ALE37, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ALE38, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ALE39, 31, 0, 0x20000000)))
        {
            _BIST_REG_ERR(unit, MBIST_ALE33, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ALE34, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ALE35, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ALE36, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ALE37, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ALE38, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ALE39, 31, 0, 0x20000000);
            _BIST_PRINTF("MBIST_ALE33~39 have Fail\n");
        }

        _BIST_REG_UPD(unit, MBIST_ALE_START_1, 31, 0, 0x0);
    }

    /*
     * ACL
     */

    _BIST_PRINTF("==============ACL_MBIST======================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        _BIST_REG_UPD(unit, MBIST_ACL_START, 31, 0, (0xff00000 | 0xf0000 | 0xff00 | 0xff));
        _bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_TCM00_ACL1, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_TCM01_ACL1, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_TCM02_ACL1, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_TCM03_ACL1, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_TCM04_ACL1, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_TCM05_ACL1, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_TCM06_ACL1, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_TCM07_ACL1, 31, 0, 0x20000000)))
        {
            _BIST_REG_ERR(unit, MBIST_TCM00_ACL1, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_TCM01_ACL1, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_TCM02_ACL1, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_TCM03_ACL1, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_TCM04_ACL1, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_TCM05_ACL1, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_TCM06_ACL1, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_TCM07_ACL1, 31, 0, 0x20000000);
            _BIST_PRINTF("MBIST_TCM00~07_ACL1 have Fail\n");
        }

        //_BIST_REG_UPD(unit, MBIST_ACL_START, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MACL1_TCM08~15======================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        //_BIST_REG_UPD(unit, MBIST_ACL_START, 31, 0, 0xff00);
        //_bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_TCM08_ACL1, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_TCM09_ACL1, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_TCM10_ACL1, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_TCM11_ACL1, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_TCM12_ACL1, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_TCM13_ACL1, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_TCM14_ACL1, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_TCM15_ACL1, 31, 0, 0x20000000)))
        {
            _BIST_REG_ERR(unit, MBIST_TCM08_ACL1, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_TCM09_ACL1, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_TCM10_ACL1, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_TCM11_ACL1, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_TCM12_ACL1, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_TCM13_ACL1, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_TCM14_ACL1, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_TCM15_ACL1, 31, 0, 0x20000000);
            _BIST_PRINTF("MBIST_TCM08~15_ACL1 have Fail\n");
        }

        //_BIST_REG_UPD(unit, MBIST_ACL_START, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MACL2_LOG00~03======================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        //_BIST_REG_UPD(unit, MBIST_ACL_START, 31, 0, 0xf0000);
        //_bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_LOG00_ACL2, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_LOG01_ACL2, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_LOG02_ACL2, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_LOG03_ACL2, 31, 0, 0x20000000)))
        {
            _BIST_REG_ERR(unit, MBIST_LOG00_ACL2, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_LOG01_ACL2, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_LOG02_ACL2, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_LOG03_ACL2, 31, 0, 0x20000000);
            _BIST_PRINTF("MBIST_LOG00~03_ACL2 have Fail\n");
        }

        //_BIST_REG_UPD(unit, MBIST_ACL_START, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MACL3_ACT00~07======================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        //_BIST_REG_UPD(unit, MBIST_ACL_START, 31, 0, 0xff00000);
        //_bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_ACT00_ACL3, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ACT01_ACL3, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ACT02_ACL3, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ACT03_ACL3, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ACT04_ACL3, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ACT05_ACL3, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ACT06_ACL3, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_ACT07_ACL3, 31, 0, 0x20000000)))
        {
            _BIST_REG_ERR(unit, MBIST_ACT00_ACL3, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ACT01_ACL3, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ACT02_ACL3, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ACT03_ACL3, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ACT04_ACL3, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ACT05_ACL3, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ACT06_ACL3, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_ACT07_ACL3, 31, 0, 0x20000000);
            _BIST_PRINTF("MBIST_ACT00~07_ACL3 have Fail\n");
        }

        _BIST_REG_UPD(unit, MBIST_ACL_START, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_MIB 00~02======================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        _BIST_REG_UPD(unit, MBIST_MIB_START, 31, 0, 0x7);
        _bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_MIB00, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_MIB01, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_MIB02, 31, 0, 0x20000000)))
        {
            _BIST_REG_ERR(unit, MBIST_MIB00, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_MIB01, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_MIB02, 31, 0, 0x20000000);
            _BIST_PRINTF("MBIST_MIB00~02 have Fail\n");
        }

        _BIST_REG_UPD(unit, MBIST_MIB_START, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_ENCAP test======================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        _BIST_REG_UPD(unit, MBIST_ENCAP_START, 31, 0, (0x4000000 | 0x3fC0000 | 0x3fC00 | 0x380 | 0x7f));
        _bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_ENCAP01, 31,  0, 0x0)) ||
            (_BIST_REG_CHK(unit, MBIST_ENCAP09, 31, 24, 0xaa)))
        {
            _BIST_PRINTF("MBIST_ENCAP_MPG00~04 have Fail\n");
        }

        if ((_BIST_REG_CHK(unit, MBIST_ENCAP03, 31,  0, 0x0)) ||
            (_BIST_REG_CHK(unit, MBIST_ENCAP09, 23, 16, 0xaa)))
        {
            _BIST_PRINTF("MBIST_ENCAP_MPT00~04 have Fail\n");
        }

        //_BIST_REG_UPD(unit, MBIST_ENCAP_START, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_ENCAP MPF & MPHF 1~4======================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        //_BIST_REG_UPD(unit, MBIST_ENCAP_START, 31, 0, 0x3fC0000);
        //_bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_ENCAP05, 31, 0, 0x0)) ||
            (_BIST_REG_CHK(unit, MBIST_ENCAP09, 15, 8, 0xaa)))
        {
            _BIST_PRINTF("MBIST_ENCAP_MPF00~04 have Fail\n");
        }

        if ((_BIST_REG_CHK(unit, MBIST_ENCAP07, 31, 0, 0x0)) ||
            (_BIST_REG_CHK(unit, MBIST_ENCAP09,  7, 0, 0xaa)))
        {
            _BIST_PRINTF("MBIST_ENCAP_MPHF00~04 have Fail\n");
        }

        //_BIST_REG_UPD(unit, MBIST_ENCAP_START, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_ENCAP MTCAM======================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        //_BIST_REG_UPD(unit, MBIST_ENCAP_START, 31, 0, 0x4000000);
        //_bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_ENCAP_TCAM, 31, 0, 0x2000000)))
        {
            _BIST_PRINTF("MBIST_ENCAP_MTCAM Fail\n");
        }

        //_BIST_REG_UPD(unit, MBIST_ENCAP_START, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_ENCAP MKELL & MRMK & & MEVC & MMPLS & MTS======================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        //_BIST_REG_UPD(unit, MBIST_ENCAP_START, 31, 0, 0x7f);
        //_bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_ENCAP11, 28, 24, 0x0)) ||
            (_BIST_REG_CHK(unit, MBIST_ENCAP13, 30, 28, 0x2)))
        {
            _BIST_PRINTF("MBIST_ENCAP_MKELL Fail\n");
        }

        if ((_BIST_REG_CHK(unit, MBIST_ENCAP11, 21, 16, 0x0)) ||
            (_BIST_REG_CHK(unit, MBIST_ENCAP13, 27, 20, 0x22)))
        {
            _BIST_PRINTF("MBIST_ENCAP_MTS Fail\n");
        }

        if ((_BIST_REG_CHK(unit, MBIST_ENCAP11, 13, 12, 0x0)) ||
            (_BIST_REG_CHK(unit, MBIST_ENCAP13, 19, 12, 0x22)))
        {
            _BIST_PRINTF("MBIST_ENCAP_MRMK Fail\n");
        }

        if ((_BIST_REG_CHK(unit, MBIST_ENCAP11,  8,  8, 0x0)) ||
            (_BIST_REG_CHK(unit, MBIST_ENCAP13, 11,  8, 0x2)))
        {
            _BIST_PRINTF("MBIST_ENCAP_MEVC Fail\n");
        }

        if ((_BIST_REG_CHK(unit, MBIST_ENCAP11,  4,  4, 0x0)) ||
            (_BIST_REG_CHK(unit, MBIST_ENCAP13,  7,  4, 0x2)))
        {
            _BIST_PRINTF("MBIST_ENCAP_MMPLS Fail\n");
        }

        //_BIST_REG_UPD(unit, MBIST_ENCAP_START, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_ENCAP MBS & MUNTAG======================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        //_BIST_REG_UPD(unit, MBIST_ENCAP_START, 31, 0, 0x380);
        //_bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_ENCAP11, 0, 0, 0x0)) ||
            (_BIST_REG_CHK(unit, MBIST_ENCAP13, 3, 0, 0x2)) ||
            (_BIST_REG_CHK(unit, MBISR_ENCAP01, 31, 0, 0x2000000)))
        {
            _BIST_PRINTF("MBIST_ENCAP_MBS Fail\n");
        }

        if ((_BIST_REG_CHK(unit, MBISR_ENCAP02, 31, 0, 0x2000000)))
        {
            _BIST_PRINTF("MBIST_ENCAP_MUNTAG Fail\n");
        }

        _BIST_REG_UPD(unit, MBIST_ENCAP_START, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_PAR 00~03======================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        _BIST_REG_UPD(unit, MBIST_PAR_START, 31, 0, 0xf);
        _bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_PAR01_0, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PAR01_1, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PAR02,   31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_PAR03,   31, 0, 0x20000000)))
        {
            _BIST_REG_ERR(unit, MBIST_PAR01_0, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_PAR01_1, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_PAR02,   31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_PAR03,   31, 0, 0x20000000);
            _BIST_PRINTF("MBIST_PAR00~03 have Fail\n");
        }

        _BIST_REG_UPD(unit, MBIST_PAR_START, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_IGR 01~07======================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        _BIST_REG_UPD(unit, MBIST_IGR_START, 31, 0, (0xfc0 | 0x3f));
        _bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_IGR01_0, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_IGR01_1, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_IGR02,   31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_IGR03,   31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_IGR04,   31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_IGR05,   31, 0, 0x20000000)))
        {
            _BIST_REG_ERR(unit, MBIST_IGR01_0, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_IGR01_1, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_IGR02,   31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_IGR03,   31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_IGR04,   31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_IGR05,   31, 0, 0x20000000);
            _BIST_PRINTF("MBIST_IGR00~05 have Fail\n");
        }

        //_BIST_REG_UPD(unit, MBIST_IGR_START, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============IGR06~07======================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        //_BIST_REG_UPD(unit, MBIST_IGR_START, 31, 0, 0xfc0);
        //_bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_IGR06_0, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_IGR06_1, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_IGR06_2, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_IGR06_3, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_IGR06_4, 31, 0, 0x20000000)) ||
            (_BIST_REG_CHK(unit, MBIST_IGR07,   31, 0, 0x20000000)))
        {
            _BIST_REG_ERR(unit, MBIST_IGR06_0, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_IGR06_1, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_IGR06_2, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_IGR06_3, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_IGR06_4, 31, 0, 0x20000000);
            _BIST_REG_ERR(unit, MBIST_IGR07,   31, 0, 0x20000000);
            _BIST_PRINTF("MBIST_IGR06~07 have Fail\n");
        }

        _BIST_REG_UPD(unit, MBIST_IGR_START, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_EGR======================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        _BIST_REG_UPD(unit, MBIST_EGR_START_0, 31, 0, (0xfe000000 | 0x1fe0000 | 0x1e000 | 0x1fe0| 0x1e));
        _BIST_REG_UPD(unit, MBIST_EGR_START_1, 31, 0, (0x7e000000 | 0x1fe0000 | 0x1fe00 | 0x1fe | 0x1));
        _bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_EGR01, 23, 8, 0x0)) ||
            (_BIST_REG_CHK(unit, MBIST_EGR11, 12, 5, 0xff)))
        {
            _BIST_PRINTF("MBIST_EGR01~07 have Fail\n");
        }

        //_BIST_REG_UPD(unit, MBIST_EGR_START_0, 31, 0, 0x0);
        //_BIST_REG_UPD(unit, MBIST_EGR_START_1, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============EGR08~11======================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        //_BIST_REG_UPD(unit, MBIST_EGR_START_0, 31, 0, 0x1e000);
        //_bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_EGR01, 31, 24, 0x0)) ||
            (_BIST_REG_CHK(unit, MBIST_EGR11, 16, 13, 0xf)))
        {
            _BIST_PRINTF("MBIST_EGR08~11 have Fail\n");
        }

        //_BIST_REG_UPD(unit, MBIST_EGR_START_0, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============EGR12~19======================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        //_BIST_REG_UPD(unit, MBIST_EGR_START_0, 31, 0, 0x1fe0000);
        //_bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_EGR02, 15,  0, 0x0)) ||
            (_BIST_REG_CHK(unit, MBIST_EGR11, 24, 17, 0xff)))
        {
            _BIST_PRINTF("MBIST_EGR12~19 have Fail\n");
        }

        //_BIST_REG_UPD(unit, MBIST_EGR_START_0, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============EGR20~27======================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        //_BIST_REG_UPD(unit, MBIST_EGR_START_0, 31, 0, 0xfe000000);
        //_BIST_REG_UPD(unit, MBIST_EGR_START_1, 31, 0, 0x1);
        //_bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_EGR02, 31, 16, 0x0)) ||
            (_BIST_REG_CHK(unit, MBIST_EGR11, 31, 25, 0x7f)) ||
            (_BIST_REG_CHK(unit, MBIST_EGR12,  0,  0, 0x1)))
        {
            _BIST_PRINTF("MBIST_EGR20~27 have Fail\n");
        }

        //_BIST_REG_UPD(unit, MBIST_EGR_START_0, 31, 0, 0x0);
        //_BIST_REG_UPD(unit, MBIST_EGR_START_1, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============EGR28~35======================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        //_BIST_REG_UPD(unit, MBIST_EGR_START_1, 31, 0, 0x1fe);
        //_bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_EGR03, 15,  0, 0x0)) ||
            (_BIST_REG_CHK(unit, MBIST_EGR12,  8,  1, 0xff)))
        {
            _BIST_PRINTF("MBIST_EGR28~35 have Fail\n");
        }

        //_BIST_REG_UPD(unit, MBIST_EGR_START_1, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============EGR36~43======================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        //_BIST_REG_UPD(unit, MBIST_EGR_START_1, 31, 0, 0x1fe00);
        //_bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_EGR03, 31, 16, 0x0)) ||
            (_BIST_REG_CHK(unit, MBIST_EGR12, 16,  9, 0xff)))
        {
            _BIST_PRINTF("MBIST_EGR36~43 have Fail\n");
        }

        //_BIST_REG_UPD(unit, MBIST_EGR_START_1, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============EGR44~51======================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        //_BIST_REG_UPD(unit, MBIST_EGR_START_1, 31, 0, 0x1fe0000);
        //_bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_EGR04, 15,  0, 0x0)) ||
            (_BIST_REG_CHK(unit, MBIST_EGR12, 24, 17, 0xff)))
        {
            _BIST_PRINTF("MBIST_EGR44~51 have Fail\n");
        }

        //_BIST_REG_UPD(unit, MBIST_EGR_START_1, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============EGR52~57======================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        //_BIST_REG_UPD(unit, MBIST_EGR_START_1, 31, 0, 0x7e000000);
        //_bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_EGR04, 27, 16, 0x0)) ||
            (_BIST_REG_CHK(unit, MBIST_EGR12, 30, 25, 0x3f)))
        {
            _BIST_PRINTF("MBIST_EGR52~57 have Fail\n");
        }

        //_BIST_REG_UPD(unit, MBIST_EGR_START_1, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============EGR_MDRC_CC & MPORT & MPORT_EXT & MLINK======================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x1);
        //_BIST_REG_UPD(unit, MBIST_EGR_START_0, 31, 0, 0x1e);
        //_bist_mdelay(50);

        if ((_BIST_REG_CHK(unit, MBIST_EGR01, 5, 1, 0x0)) ||
            (_BIST_REG_CHK(unit, MBIST_EGR11, 4, 1, 0xf)))
        {
            _BIST_PRINTF("MBIST_EGR_MDRC_CC & MPORT & MPORT_EXT & MLINK have Fail\n");
        }

        _BIST_REG_UPD(unit, MBIST_EGR_START_0, 31, 0, 0x0);
        _BIST_REG_UPD(unit, MBIST_EGR_START_1, 31, 0, 0x0);
    }

    /* clear MBIST test */
    _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x0);

    /*
     * Data Retention Fault
     */

    _BIST_PRINTF("==============MAC_DRF_MBIST======================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        _BIST_REG_UPD(unit, MBIST_MAC_RESET_00, 31, 0, (0x16000000 | 0xb00000 | 0x58000 | 0x2c00 | 0x160 | 0xb));
        //_bist_mdelay(100);

        if ((_BIST_REG_CHK(unit, MBIST_PG00_RX_MAC01, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG00_RX_MAC05, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG00_TX_MAC01, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG01_RX_MAC01, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG01_RX_MAC05, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG01_TX_MAC01, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG02_RX_MAC01, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG02_RX_MAC05, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG02_TX_MAC01, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG03_RX_MAC01, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG03_RX_MAC05, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG03_TX_MAC01, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG04_RX_MAC01, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG04_RX_MAC05, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG04_TX_MAC01, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG05_RX_MAC01, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG05_RX_MAC05, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG05_TX_MAC01, 24, 24, 0x01))
            )
        {
            _BIST_PRINTF(" wait 500ms MBIST_PG00_RX_MAC01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG00_RX_MAC01, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG00_RX_MAC05 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG00_RX_MAC05, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG00_TX_MAC01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG00_TX_MAC01, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG01_RX_MAC01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG01_RX_MAC01, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG01_RX_MAC05 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG01_RX_MAC05, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG01_TX_MAC01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG01_TX_MAC01, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG02_RX_MAC01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG02_RX_MAC01, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG02_RX_MAC05 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG02_RX_MAC05, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG02_TX_MAC01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG02_TX_MAC01, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG03_RX_MAC01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG03_RX_MAC01, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG03_RX_MAC05 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG03_RX_MAC05, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG03_TX_MAC01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG03_TX_MAC01, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG04_RX_MAC01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG04_RX_MAC01, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG04_RX_MAC05 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG04_RX_MAC05, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG04_TX_MAC01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG04_TX_MAC01, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG05_RX_MAC01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG05_RX_MAC01, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG05_RX_MAC05 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG05_RX_MAC05, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG05_TX_MAC01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG05_TX_MAC01, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_MAC_RESUME_00, 31, 0, (0x16000000 | 0xb00000 | 0x58000 | 0x2c00 | 0x160 | 0xb));
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_MAC_RESUME_00, 31, 0, 0x0);
        }

        _bist_mdelay(100);
        if ((_BIST_REG_CHK(unit, MBIST_PG00_RX_MAC01, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG00_RX_MAC05, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG00_TX_MAC01, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG01_RX_MAC01, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG01_RX_MAC05, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG01_TX_MAC01, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG02_RX_MAC01, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG02_RX_MAC05, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG02_TX_MAC01, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG03_RX_MAC01, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG03_RX_MAC05, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG03_TX_MAC01, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG04_RX_MAC01, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG04_RX_MAC05, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG04_TX_MAC01, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG05_RX_MAC01, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG05_RX_MAC05, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG05_TX_MAC01, 24, 24, 0x01))
            )
        {
            _BIST_PRINTF(" wait 500ms MBIST_PG00_RX_MAC01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG00_RX_MAC01, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG00_RX_MAC05 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG00_RX_MAC05, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG00_TX_MAC01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG00_TX_MAC01, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG01_RX_MAC01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG01_RX_MAC01, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG01_RX_MAC05 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG01_RX_MAC05, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG01_TX_MAC01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG01_TX_MAC01, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG02_RX_MAC01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG02_RX_MAC01, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG02_RX_MAC05 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG02_RX_MAC05, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG02_TX_MAC01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG02_TX_MAC01, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG03_RX_MAC01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG03_RX_MAC01, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG03_RX_MAC05 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG03_RX_MAC05, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG03_TX_MAC01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG03_TX_MAC01, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG04_RX_MAC01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG04_RX_MAC01, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG04_RX_MAC05 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG04_RX_MAC05, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG04_TX_MAC01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG04_TX_MAC01, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG05_RX_MAC01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG05_RX_MAC01, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG05_RX_MAC05 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG05_RX_MAC05, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG05_TX_MAC01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG05_TX_MAC01, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_MAC_RESUME_00, 31, 0, (0x16000000 | 0xb00000 | 0x58000 | 0x2c00 | 0x160 | 0xb));
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_MAC_RESUME_00, 31, 0, 0x0);
            _bist_mdelay(100);
            if ((_BIST_REG_CHK(unit, MBIST_PG00_RX_MAC01, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG00_RX_MAC05, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG00_TX_MAC01, 28, 12, 0x10000)))
            {
                _BIST_PRINTF("MBIST_DRF_PG0 Fail\n");
            }

            if ((_BIST_REG_CHK(unit, MBIST_PG01_RX_MAC01, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG01_RX_MAC05, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG01_TX_MAC01, 28, 12, 0x10000)))
            {
                _BIST_PRINTF("MBIST_DRF_PG1 Fail\n");
            }

            if ((_BIST_REG_CHK(unit, MBIST_PG02_RX_MAC01, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG02_RX_MAC05, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG02_TX_MAC01, 28, 12, 0x10000)))
            {
                _BIST_PRINTF("MBIST_DRF_PG2 Fail\n");
            }

            if ((_BIST_REG_CHK(unit, MBIST_PG03_RX_MAC01, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG03_RX_MAC05, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG03_TX_MAC01, 28, 12, 0x10000)))
            {
                _BIST_PRINTF("MBIST_DRF_PG3 Fail\n");
            }

            if ((_BIST_REG_CHK(unit, MBIST_PG04_RX_MAC01, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG04_RX_MAC05, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG04_TX_MAC01, 28, 12, 0x10000)))
            {
                _BIST_PRINTF("MBIST_DRF_PG4 Fail\n");
            }

            if ((_BIST_REG_CHK(unit, MBIST_PG05_RX_MAC01, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG05_RX_MAC05, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG05_TX_MAC01, 28, 12, 0x10000)))
            {
                _BIST_PRINTF("MBIST_DRF_PG5 Fail\n");
            }
        }

        //_BIST_REG_UPD(unit, MBIST_MAC_RESET_00, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_PG1======================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        //_BIST_REG_UPD(unit, MBIST_MAC_RESET_00, 31, 0, 0x160);
        //_bist_mdelay(100);

        //if ((_BIST_REG_CHK(unit, MBIST_PG01_RX_MAC01, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_PG01_RX_MAC05, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_PG01_TX_MAC01, 24, 24, 0x01)))
        //{
        //    _BIST_PRINTF(" wait 500ms MBIST_PG01_RX_MAC01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG01_RX_MAC01, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_PG01_RX_MAC05 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG01_RX_MAC05, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_PG01_TX_MAC01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG01_TX_MAC01, 24, 24));
        //} else {
        //    _BIST_REG_UPD(unit, MBIST_MAC_RESUME_00, 31, 0, 0x160);
        //    _bist_mdelay(50);
        //    _BIST_REG_UPD(unit, MBIST_MAC_RESUME_00, 31, 0, 0x0);
        //}

        //_bist_mdelay(100);
        //if ((_BIST_REG_CHK(unit, MBIST_PG01_RX_MAC01, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_PG01_RX_MAC05, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_PG01_TX_MAC01, 24, 24, 0x01)))
        //{
        //    _BIST_PRINTF(" wait 500ms MBIST_PG01_RX_MAC01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG01_RX_MAC01, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_PG01_RX_MAC05 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG01_RX_MAC05, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_PG01_TX_MAC01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG01_TX_MAC01, 24, 24));
        //} else {
        //    _BIST_REG_UPD(unit, MBIST_MAC_RESUME_00, 31, 0, 0x160);
        //    _bist_mdelay(50);
        //    _BIST_REG_UPD(unit, MBIST_MAC_RESUME_00, 31, 0, 0x0);
        //    _bist_mdelay(100);
        //    if ((_BIST_REG_CHK(unit, MBIST_PG01_RX_MAC01, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_PG01_RX_MAC05, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_PG01_TX_MAC01, 28, 12, 0x10000)))
        //    {
        //        _BIST_PRINTF("MBIST_DRF_PG1 Fail\n");
        //    }
        //}

        //_BIST_REG_UPD(unit, MBIST_MAC_RESET_00, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_PG2======================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        //_BIST_REG_UPD(unit, MBIST_MAC_RESET_00, 31, 0, 0x2c00);
        //_bist_mdelay(100);

        //if ((_BIST_REG_CHK(unit, MBIST_PG02_RX_MAC01, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_PG02_RX_MAC05, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_PG02_TX_MAC01, 24, 24, 0x01)))
        //{
        //    _BIST_PRINTF(" wait 500ms MBIST_PG02_RX_MAC01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG02_RX_MAC01, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_PG02_RX_MAC05 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG02_RX_MAC05, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_PG02_TX_MAC01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG02_TX_MAC01, 24, 24));
        //} else {
        //    _BIST_REG_UPD(unit, MBIST_MAC_RESUME_00, 31, 0, 0x2c00);
        //    _bist_mdelay(50);
        //    _BIST_REG_UPD(unit, MBIST_MAC_RESUME_00, 31, 0, 0x0);
        //}

        //_bist_mdelay(100);
        //if ((_BIST_REG_CHK(unit, MBIST_PG02_RX_MAC01, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_PG02_RX_MAC05, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_PG02_TX_MAC01, 24, 24, 0x01)))
        //{
        //    _BIST_PRINTF(" wait 500ms MBIST_PG02_RX_MAC01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG02_RX_MAC01, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_PG02_RX_MAC05 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG02_RX_MAC05, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_PG02_TX_MAC01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG02_TX_MAC01, 24, 24));
        //} else {
        //    _BIST_REG_UPD(unit, MBIST_MAC_RESUME_00, 31, 0, 0x2c00);
        //    _bist_mdelay(50);
        //    _BIST_REG_UPD(unit, MBIST_MAC_RESUME_00, 31, 0, 0x0);
        //    _bist_mdelay(100);
        //    if ((_BIST_REG_CHK(unit, MBIST_PG02_RX_MAC01, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_PG02_RX_MAC05, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_PG02_TX_MAC01, 28, 12, 0x10000)))
        //    {
        //        _BIST_PRINTF("MBIST_DRF_PG2 Fail\n");
        //    }
        //}

        //_BIST_REG_UPD(unit, MBIST_MAC_RESET_00, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_PG3======================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        //_BIST_REG_UPD(unit, MBIST_MAC_RESET_00, 31, 0, 0x58000);
        //_bist_mdelay(100);

        //if ((_BIST_REG_CHK(unit, MBIST_PG03_RX_MAC01, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_PG03_RX_MAC05, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_PG03_TX_MAC01, 24, 24, 0x01)))
        //{
        //    _BIST_PRINTF(" wait 500ms MBIST_PG03_RX_MAC01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG03_RX_MAC01, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_PG03_RX_MAC05 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG03_RX_MAC05, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_PG03_TX_MAC01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG03_TX_MAC01, 24, 24));
        //} else {
        //    _BIST_REG_UPD(unit, MBIST_MAC_RESUME_00, 31, 0, 0x58000);
        //    _bist_mdelay(50);
        //    _BIST_REG_UPD(unit, MBIST_MAC_RESUME_00, 31, 0, 0x0);
        //}

        //_bist_mdelay(100);
        //if ((_BIST_REG_CHK(unit, MBIST_PG03_RX_MAC01, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_PG03_RX_MAC05, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_PG03_TX_MAC01, 24, 24, 0x01)))
        //{
        //    _BIST_PRINTF(" wait 500ms MBIST_PG03_RX_MAC01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG03_RX_MAC01, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_PG03_RX_MAC05 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG03_RX_MAC05, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_PG03_TX_MAC01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG03_TX_MAC01, 24, 24));
        //} else {
        //    _BIST_REG_UPD(unit, MBIST_MAC_RESUME_00, 31, 0, 0x58000);
        //    _bist_mdelay(50);
        //    _BIST_REG_UPD(unit, MBIST_MAC_RESUME_00, 31, 0, 0x0);
        //    _bist_mdelay(100);
        //    if ((_BIST_REG_CHK(unit, MBIST_PG03_RX_MAC01, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_PG03_RX_MAC05, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_PG03_TX_MAC01, 28, 12, 0x10000)))
        //    {
        //        _BIST_PRINTF("MBIST_DRF_PG3 Fail\n");
        //    }
        //}

        //_BIST_REG_UPD(unit, MBIST_MAC_RESET_00, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_PG4======================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        //_BIST_REG_UPD(unit, MBIST_MAC_RESET_00, 31, 0, 0xb00000);
        //_bist_mdelay(100);

        //if ((_BIST_REG_CHK(unit, MBIST_PG04_RX_MAC01, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_PG04_RX_MAC05, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_PG04_TX_MAC01, 24, 24, 0x01)))
        //{
        //    _BIST_PRINTF(" wait 500ms MBIST_PG04_RX_MAC01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG04_RX_MAC01, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_PG04_RX_MAC05 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG04_RX_MAC05, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_PG04_TX_MAC01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG04_TX_MAC01, 24, 24));
        //} else {
        //    _BIST_REG_UPD(unit, MBIST_MAC_RESUME_00, 31, 0, 0xb00000);
        //    _bist_mdelay(50);
        //    _BIST_REG_UPD(unit, MBIST_MAC_RESUME_00, 31, 0, 0x0);
        //}

        //_bist_mdelay(100);
        //if ((_BIST_REG_CHK(unit, MBIST_PG04_RX_MAC01, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_PG04_RX_MAC05, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_PG04_TX_MAC01, 24, 24, 0x01)))
        //{
        //    _BIST_PRINTF(" wait 500ms MBIST_PG04_RX_MAC01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG04_RX_MAC01, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_PG04_RX_MAC05 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG04_RX_MAC05, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_PG04_TX_MAC01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG04_TX_MAC01, 24, 24));
        //} else {
        //    _BIST_REG_UPD(unit, MBIST_MAC_RESUME_00, 31, 0, 0xb00000);
        //    _bist_mdelay(50);
        //    _BIST_REG_UPD(unit, MBIST_MAC_RESUME_00, 31, 0, 0x0);
        //    _bist_mdelay(100);
        //    if ((_BIST_REG_CHK(unit, MBIST_PG04_RX_MAC01, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_PG04_RX_MAC05, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_PG04_TX_MAC01, 28, 12, 0x10000)))
        //    {
        //        _BIST_PRINTF("MBIST_DRF_PG4 Fail\n");
        //    }
        //}

        //_BIST_REG_UPD(unit, MBIST_MAC_RESET_00, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_PG5======================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        //_BIST_REG_UPD(unit, MBIST_MAC_RESET_00, 31, 0, 0x16000000);
        //_bist_mdelay(100);

        //if ((_BIST_REG_CHK(unit, MBIST_PG05_RX_MAC01, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_PG05_RX_MAC05, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_PG05_TX_MAC01, 24, 24, 0x01)))
        //{
        //    _BIST_PRINTF(" wait 500ms MBIST_PG05_RX_MAC01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG05_RX_MAC01, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_PG05_RX_MAC05 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG05_RX_MAC05, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_PG05_TX_MAC01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG05_TX_MAC01, 24, 24));
        //} else {
        //    _BIST_REG_UPD(unit, MBIST_MAC_RESUME_00, 31, 0, 0x16000000);
        //    _bist_mdelay(50);
        //    _BIST_REG_UPD(unit, MBIST_MAC_RESUME_00, 31, 0, 0x0);
        //}

        //_bist_mdelay(100);
        //if ((_BIST_REG_CHK(unit, MBIST_PG05_RX_MAC01, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_PG05_RX_MAC05, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_PG05_TX_MAC01, 24, 24, 0x01)))
        //{
        //    _BIST_PRINTF(" wait 500ms MBIST_PG05_RX_MAC01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG05_RX_MAC01, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_PG05_RX_MAC05 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG05_RX_MAC05, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_PG05_TX_MAC01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG05_TX_MAC01, 24, 24));
        //} else {
        //    _BIST_REG_UPD(unit, MBIST_MAC_RESUME_00, 31, 0, 0x16000000);
        //    _bist_mdelay(50);
        //    _BIST_REG_UPD(unit, MBIST_MAC_RESUME_00, 31, 0, 0x0);
        //    _bist_mdelay(100);
        //    if ((_BIST_REG_CHK(unit, MBIST_PG05_RX_MAC01, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_PG05_RX_MAC05, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_PG05_TX_MAC01, 28, 12, 0x10000)))
        //    {
        //        _BIST_PRINTF("MBIST_DRF_PG5 Fail\n");
        //    }
        //}

        //_BIST_REG_UPD(unit, MBIST_MAC_RESET_00, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_PG6======================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        _BIST_REG_UPD(unit, MBIST_MAC_RESET_01, 31, 0, (0x39c000 | 0x3380 | 0x67));
        _bist_mdelay(100);

        if ((_BIST_REG_CHK(unit, MBIST_PG06_RX_MAC08, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG06_RX_MAC04, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG06_RX_MAC06, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG06_TX_MAC08, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG06_TX_MAC02, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG07_RX_MAC08, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG07_RX_MAC04, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG07_RX_MAC06, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG07_TX_MAC08, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG07_TX_MAC02, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG12_RX_MAC03, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG12_RX_MAC09, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG12_RX_MAC07, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG12_TX_MAC02, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG12_PG_MAC13, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG12_PG_MAC14, 24, 24, 0x01))
            )
        {
            _BIST_PRINTF(" wait 500ms MBIST_PG06_RX_MAC08 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG06_RX_MAC08, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG06_RX_MAC04 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG06_RX_MAC04, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG06_RX_MAC06 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG06_RX_MAC06, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG06_TX_MAC08 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG06_TX_MAC08, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG06_TX_MAC02 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG06_TX_MAC02, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG07_RX_MAC08 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG07_RX_MAC08, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG07_RX_MAC04 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG07_RX_MAC04, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG07_RX_MAC06 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG07_RX_MAC06, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG07_TX_MAC08 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG07_TX_MAC08, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG07_TX_MAC02 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG07_TX_MAC02, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG12_RX_MAC03 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG12_RX_MAC03, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG12_RX_MAC09 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG12_RX_MAC09, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG12_RX_MAC07 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG12_RX_MAC07, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG12_TX_MAC02 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG12_TX_MAC02, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG12_PG_MAC13 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG12_PG_MAC13, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG12_PG_MAC14 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG12_PG_MAC14, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_MAC_RESUME_01, 31, 0, (0x39c000 | 0x3380 | 0x67));
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_MAC_RESUME_01, 31, 0, 0x0);
        }

        _bist_mdelay(100);
        if ((_BIST_REG_CHK(unit, MBIST_PG06_RX_MAC08, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG06_RX_MAC04, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG06_RX_MAC06, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG06_TX_MAC08, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG06_TX_MAC02, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG07_RX_MAC08, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG07_RX_MAC04, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG07_RX_MAC06, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG07_TX_MAC08, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG07_TX_MAC02, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG12_RX_MAC03, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG12_RX_MAC09, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG12_RX_MAC07, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG12_TX_MAC02, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG12_PG_MAC13, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG12_PG_MAC14, 24, 24, 0x01))
            )
        {
            _BIST_PRINTF(" wait 500ms MBIST_PG06_RX_MAC08 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG06_RX_MAC08, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG06_RX_MAC04 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG06_RX_MAC04, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG06_RX_MAC06 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG06_RX_MAC06, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG06_TX_MAC08 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG06_TX_MAC08, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG06_TX_MAC02 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG06_TX_MAC02, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG07_RX_MAC08 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG07_RX_MAC08, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG07_RX_MAC04 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG07_RX_MAC04, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG07_RX_MAC06 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG07_RX_MAC06, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG07_TX_MAC08 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG07_TX_MAC08, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG07_TX_MAC02 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG07_TX_MAC02, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG12_RX_MAC03 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG12_RX_MAC03, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG12_RX_MAC09 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG12_RX_MAC09, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG12_RX_MAC07 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG12_RX_MAC07, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG12_TX_MAC02 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG12_TX_MAC02, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG12_PG_MAC13 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG12_PG_MAC13, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG12_PG_MAC14 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG12_PG_MAC14, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_MAC_RESUME_01, 31, 0, (0x39c000 | 0x3380 | 0x67));
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_MAC_RESUME_01, 31, 0, 0x0);
            _bist_mdelay(100);
            if ((_BIST_REG_CHK(unit, MBIST_PG06_RX_MAC08, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG06_RX_MAC04, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG06_RX_MAC06, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG06_TX_MAC08, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG06_TX_MAC02, 28, 12, 0x10000))
                )
            {
                _BIST_PRINTF(" wait 500ms MBIST_PG06_RX_MAC08 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG06_RX_MAC08, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_PG06_RX_MAC04 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG06_RX_MAC04, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_PG06_RX_MAC06 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG06_RX_MAC06, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_PG06_TX_MAC08 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG06_TX_MAC08, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_PG06_TX_MAC02 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG06_TX_MAC02, 28, 12));
                _BIST_PRINTF("MBIST_DRF_PG6 Fail\n");
            }
            if ((_BIST_REG_CHK(unit, MBIST_PG07_RX_MAC08, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG07_RX_MAC04, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG07_RX_MAC06, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG07_TX_MAC08, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG07_TX_MAC02, 28, 12, 0x10000)))
            {
                _BIST_PRINTF("MBIST_DRF_PG7 Fail\n");
            }
            if ((_BIST_REG_CHK(unit, MBIST_PG12_RX_MAC03, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG12_RX_MAC09, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG12_RX_MAC07, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG12_TX_MAC02, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG12_PG_MAC13, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG12_PG_MAC14, 28, 12, 0x10000)))
            {
                _BIST_PRINTF("MBIST_DRF_PG12 Fail\n");
            }
        }

        _BIST_REG_UPD(unit, MBIST_MAC_RESET_01, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_PG7======================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        //_BIST_REG_UPD(unit, MBIST_MAC_RESET_01, 31, 0, 0x3380);
        //_bist_mdelay(100);

        //if ((_BIST_REG_CHK(unit, MBIST_PG07_RX_MAC08, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_PG07_RX_MAC04, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_PG07_RX_MAC06, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_PG07_TX_MAC08, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_PG07_TX_MAC02, 24, 24, 0x01)))
        //{
        //    _BIST_PRINTF(" wait 500ms MBIST_PG07_RX_MAC08 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG07_RX_MAC08, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_PG07_RX_MAC04 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG07_RX_MAC04, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_PG07_RX_MAC06 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG07_RX_MAC06, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_PG07_TX_MAC08 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG07_TX_MAC08, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_PG07_TX_MAC02 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG07_TX_MAC02, 24, 24));
        //} else {
        //    _BIST_REG_UPD(unit, MBIST_MAC_RESUME_01, 31, 0, 0x3380);
        //    _bist_mdelay(50);
        //    _BIST_REG_UPD(unit, MBIST_MAC_RESUME_01, 31, 0, 0x0);
        //}

        //_bist_mdelay(100);
        //if ((_BIST_REG_CHK(unit, MBIST_PG07_RX_MAC08, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_PG07_RX_MAC04, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_PG07_RX_MAC06, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_PG07_TX_MAC08, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_PG07_TX_MAC02, 24, 24, 0x01)))
        //{
        //    _BIST_PRINTF(" wait 500ms MBIST_PG07_RX_MAC08 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG07_RX_MAC08, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_PG07_RX_MAC04 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG07_RX_MAC04, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_PG07_RX_MAC06 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG07_RX_MAC06, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_PG07_TX_MAC08 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG07_TX_MAC08, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_PG07_TX_MAC02 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG07_TX_MAC02, 24, 24));
        //} else {
        //    _BIST_REG_UPD(unit, MBIST_MAC_RESUME_01, 31, 0, 0x3380);
        //    _bist_mdelay(50);
        //    _BIST_REG_UPD(unit, MBIST_MAC_RESUME_01, 31, 0, 0x0);
        //    _bist_mdelay(100);
        //    if ((_BIST_REG_CHK(unit, MBIST_PG07_RX_MAC08, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_PG07_RX_MAC04, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_PG07_RX_MAC06, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_PG07_TX_MAC08, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_PG07_TX_MAC02, 28, 12, 0x10000)))
        //    {
        //        _BIST_PRINTF("MBIST_DRF_PG7 Fail\n");
        //    }
        //}

        //_BIST_REG_UPD(unit, MBIST_MAC_RESET_01, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_PG8======================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        _BIST_REG_UPD(unit, MBIST_MAC_RESET_02, 31, 0, (0xDC0000 | 0x37000 | 0xdc0 | 0x37));
        _bist_mdelay(100);

        if ((_BIST_REG_CHK(unit, MBIST_PG08_RX_MAC10, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG08_RX_MAC07, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG08_RX_MAC11, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG08_TX_MAC10, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG08_TX_MAC12, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG09_RX_MAC10, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG09_RX_MAC07, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG09_RX_MAC11, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG09_TX_MAC10, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG09_TX_MAC12, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG10_RX_MAC10, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG10_RX_MAC07, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG10_RX_MAC11, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG10_TX_MAC10, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG10_TX_MAC12, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG11_RX_MAC10, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG11_RX_MAC07, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG11_RX_MAC11, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG11_TX_MAC10, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG11_TX_MAC12, 24, 24, 0x01))
            )
        {
            _BIST_PRINTF(" wait 500ms MBIST_PG08_RX_MAC10 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG08_RX_MAC10, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG08_RX_MAC07 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG08_RX_MAC07, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG08_RX_MAC11 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG08_RX_MAC11, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG08_TX_MAC10 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG08_TX_MAC10, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG08_TX_MAC12 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG08_TX_MAC12, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG09_RX_MAC10 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG09_RX_MAC10, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG09_RX_MAC07 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG09_RX_MAC07, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG09_RX_MAC11 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG09_RX_MAC11, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG09_TX_MAC10 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG09_TX_MAC10, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG09_TX_MAC12 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG09_TX_MAC12, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG10_RX_MAC10 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG10_RX_MAC10, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG10_RX_MAC07 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG10_RX_MAC07, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG10_RX_MAC11 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG10_RX_MAC11, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG10_TX_MAC10 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG10_TX_MAC10, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG10_TX_MAC12 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG10_TX_MAC12, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG11_RX_MAC10 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG11_RX_MAC10, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG11_RX_MAC07 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG11_RX_MAC07, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG11_RX_MAC11 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG11_RX_MAC11, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG11_TX_MAC10 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG11_TX_MAC10, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG11_TX_MAC12 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG11_TX_MAC12, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_MAC_RESUME_02, 31, 0, (0xDC0000 | 0x37000 | 0xdc0 | 0x37));
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_MAC_RESUME_02, 31, 0, 0x0);
        }

        _bist_mdelay(100);
        if ((_BIST_REG_CHK(unit, MBIST_PG08_RX_MAC10, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG08_RX_MAC07, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG08_RX_MAC11, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG08_TX_MAC10, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG08_TX_MAC12, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG09_RX_MAC10, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG09_RX_MAC07, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG09_RX_MAC11, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG09_TX_MAC10, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG09_TX_MAC12, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG10_RX_MAC10, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG10_RX_MAC07, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG10_RX_MAC11, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG10_TX_MAC10, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG10_TX_MAC12, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG11_RX_MAC10, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG11_RX_MAC07, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG11_RX_MAC11, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG11_TX_MAC10, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PG11_TX_MAC12, 24, 24, 0x01))
            )
        {
            _BIST_PRINTF(" wait 500ms MBIST_PG08_RX_MAC10 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG08_RX_MAC10, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG08_RX_MAC07 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG08_RX_MAC07, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG08_RX_MAC11 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG08_RX_MAC11, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG08_TX_MAC10 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG08_TX_MAC10, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG08_TX_MAC12 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG08_TX_MAC12, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG09_RX_MAC10 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG09_RX_MAC10, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG09_RX_MAC07 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG09_RX_MAC07, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG09_RX_MAC11 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG09_RX_MAC11, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG09_TX_MAC10 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG09_TX_MAC10, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG09_TX_MAC12 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG09_TX_MAC12, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG10_RX_MAC10 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG10_RX_MAC10, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG10_RX_MAC07 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG10_RX_MAC07, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG10_RX_MAC11 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG10_RX_MAC11, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG10_TX_MAC10 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG10_TX_MAC10, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG10_TX_MAC12 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG10_TX_MAC12, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG11_RX_MAC10 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG11_RX_MAC10, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG11_RX_MAC07 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG11_RX_MAC07, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG11_RX_MAC11 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG11_RX_MAC11, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG11_TX_MAC10 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG11_TX_MAC10, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_PG11_TX_MAC12 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG11_TX_MAC12, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_MAC_RESUME_02, 31, 0, (0xDC0000 | 0x37000 | 0xdc0 | 0x37));
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_MAC_RESUME_02, 31, 0, 0x0);
            _bist_mdelay(100);
            if ((_BIST_REG_CHK(unit, MBIST_PG08_RX_MAC10, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG08_RX_MAC07, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG08_RX_MAC11, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG08_TX_MAC10, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG08_TX_MAC12, 28, 12, 0x10000)))
            {
                _BIST_PRINTF(" wait 500ms MBIST_PG08_RX_MAC10 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG08_RX_MAC10, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_PG08_RX_MAC07 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG08_RX_MAC07, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_PG08_RX_MAC11 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG08_RX_MAC11, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_PG08_TX_MAC10 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG08_TX_MAC10, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_PG08_TX_MAC12 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG08_TX_MAC12, 28, 12));
                _BIST_PRINTF("MBIST_DRF_PG8 Fail\n");
            }

            if ((_BIST_REG_CHK(unit, MBIST_PG09_RX_MAC10, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG09_RX_MAC07, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG09_RX_MAC11, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG09_TX_MAC10, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG09_TX_MAC12, 28, 12, 0x10000)))
            {
                _BIST_PRINTF("MBIST_DRF_PG9 Fail\n");
            }
            if ((_BIST_REG_CHK(unit, MBIST_PG10_RX_MAC10, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG10_RX_MAC07, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG10_RX_MAC11, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG10_TX_MAC10, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG10_TX_MAC12, 28, 12, 0x10000)))
            {
                _BIST_PRINTF("MBIST_DRF_PG10 Fail\n");
            }

            if ((_BIST_REG_CHK(unit, MBIST_PG11_RX_MAC10, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG11_RX_MAC07, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG11_RX_MAC11, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG11_TX_MAC10, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PG11_TX_MAC12, 28, 12, 0x10000)))
            {
                _BIST_PRINTF("MBIST_DRF_PG11 Fail\n");
            }
        }

        _BIST_REG_UPD(unit, MBIST_MAC_RESET_02, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_PG9======================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        //_BIST_REG_UPD(unit, MBIST_MAC_RESET_02, 31, 0, 0xdc0);
        //_bist_mdelay(100);

        //if ((_BIST_REG_CHK(unit, MBIST_PG09_RX_MAC10, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_PG09_RX_MAC07, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_PG09_RX_MAC11, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_PG09_TX_MAC10, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_PG09_TX_MAC12, 24, 24, 0x01)))
        //{
        //    _BIST_PRINTF(" wait 500ms MBIST_PG09_RX_MAC10 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG09_RX_MAC10, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_PG09_RX_MAC07 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG09_RX_MAC07, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_PG09_RX_MAC11 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG09_RX_MAC11, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_PG09_TX_MAC10 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG09_TX_MAC10, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_PG09_TX_MAC12 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG09_TX_MAC12, 24, 24));
        //} else {
        //    _BIST_REG_UPD(unit, MBIST_MAC_RESUME_02, 31, 0, 0xdc0);
        //    _bist_mdelay(50);
        //    _BIST_REG_UPD(unit, MBIST_MAC_RESUME_02, 31, 0, 0x0);
        //}

        //_bist_mdelay(100);
        //if ((_BIST_REG_CHK(unit, MBIST_PG09_RX_MAC10, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_PG09_RX_MAC07, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_PG09_RX_MAC11, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_PG09_TX_MAC10, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_PG09_TX_MAC12, 24, 24, 0x01)))
        //{
        //    _BIST_PRINTF(" wait 500ms MBIST_PG09_RX_MAC10 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG09_RX_MAC10, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_PG09_RX_MAC07 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG09_RX_MAC07, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_PG09_RX_MAC11 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG09_RX_MAC11, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_PG09_TX_MAC10 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG09_TX_MAC10, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_PG09_TX_MAC12 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG09_TX_MAC12, 24, 24));
        //} else {
        //    _BIST_REG_UPD(unit, MBIST_MAC_RESUME_02, 31, 0, 0xdc0);
        //    _bist_mdelay(50);
        //    _BIST_REG_UPD(unit, MBIST_MAC_RESUME_02, 31, 0, 0x0);
        //    _bist_mdelay(100);
        //    if ((_BIST_REG_CHK(unit, MBIST_PG09_RX_MAC10, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_PG09_RX_MAC07, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_PG09_RX_MAC11, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_PG09_TX_MAC10, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_PG09_TX_MAC12, 28, 12, 0x10000)))
        //    {
        //        _BIST_PRINTF("MBIST_DRF_PG9 Fail\n");
        //    }
        //}

        //_BIST_REG_UPD(unit, MBIST_MAC_RESET_02, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_PG10======================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        //_BIST_REG_UPD(unit, MBIST_MAC_RESET_02, 31, 0, 0x37000);
        //_bist_mdelay(100);

        //if ((_BIST_REG_CHK(unit, MBIST_PG10_RX_MAC10, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_PG10_RX_MAC07, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_PG10_RX_MAC11, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_PG10_TX_MAC10, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_PG10_TX_MAC12, 24, 24, 0x01)))
        //{
        //    _BIST_PRINTF(" wait 500ms MBIST_PG10_RX_MAC10 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG10_RX_MAC10, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_PG10_RX_MAC07 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG10_RX_MAC07, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_PG10_RX_MAC11 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG10_RX_MAC11, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_PG10_TX_MAC10 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG10_TX_MAC10, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_PG10_TX_MAC12 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG10_TX_MAC12, 24, 24));
        //} else {
        //    _BIST_REG_UPD(unit, MBIST_MAC_RESUME_02, 31, 0, 0x37000);
        //    _bist_mdelay(50);
        //    _BIST_REG_UPD(unit, MBIST_MAC_RESUME_02, 31, 0, 0x0);
        //}

        //_bist_mdelay(100);
        //if ((_BIST_REG_CHK(unit, MBIST_PG10_RX_MAC10, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_PG10_RX_MAC07, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_PG10_RX_MAC11, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_PG10_TX_MAC10, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_PG10_TX_MAC12, 24, 24, 0x01)))
        //{
        //    _BIST_PRINTF(" wait 500ms MBIST_PG10_RX_MAC10 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG10_RX_MAC10, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_PG10_RX_MAC07 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG10_RX_MAC07, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_PG10_RX_MAC11 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG10_RX_MAC11, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_PG10_TX_MAC10 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG10_TX_MAC10, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_PG10_TX_MAC12 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG10_TX_MAC12, 24, 24));
        //} else {
        //    _BIST_REG_UPD(unit, MBIST_MAC_RESUME_02, 31, 0, 0x37000);
        //    _bist_mdelay(50);
        //    _BIST_REG_UPD(unit, MBIST_MAC_RESUME_02, 31, 0, 0x0);
        //    _bist_mdelay(100);
        //    if ((_BIST_REG_CHK(unit, MBIST_PG10_RX_MAC10, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_PG10_RX_MAC07, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_PG10_RX_MAC11, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_PG10_TX_MAC10, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_PG10_TX_MAC12, 28, 12, 0x10000)))
        //    {
        //        _BIST_PRINTF("MBIST_DRF_PG10 Fail\n");
        //    }
        //}

        //_BIST_REG_UPD(unit, MBIST_MAC_RESET_02, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_PG11======================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        //_BIST_REG_UPD(unit, MBIST_MAC_RESET_02, 31, 0, 0xDC0000);
        //_bist_mdelay(100);

        //if ((_BIST_REG_CHK(unit, MBIST_PG11_RX_MAC10, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_PG11_RX_MAC07, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_PG11_RX_MAC11, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_PG11_TX_MAC10, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_PG11_TX_MAC12, 24, 24, 0x01)))
        //{
        //    _BIST_PRINTF(" wait 500ms MBIST_PG11_RX_MAC10 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG11_RX_MAC10, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_PG11_RX_MAC07 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG11_RX_MAC07, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_PG11_RX_MAC11 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG11_RX_MAC11, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_PG11_TX_MAC10 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG11_TX_MAC10, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_PG11_TX_MAC12 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG11_TX_MAC12, 24, 24));
        //} else {
        //    _BIST_REG_UPD(unit, MBIST_MAC_RESUME_02, 31, 0, 0xDC0000);
        //    _bist_mdelay(50);
        //    _BIST_REG_UPD(unit, MBIST_MAC_RESUME_02, 31, 0, 0x0);
        //}

        //_bist_mdelay(100);
        //if ((_BIST_REG_CHK(unit, MBIST_PG11_RX_MAC10, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_PG11_RX_MAC07, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_PG11_RX_MAC11, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_PG11_TX_MAC10, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_PG11_TX_MAC12, 24, 24, 0x01)))
        //{
        //    _BIST_PRINTF(" wait 500ms MBIST_PG11_RX_MAC10 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG11_RX_MAC10, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_PG11_RX_MAC07 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG11_RX_MAC07, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_PG11_RX_MAC11 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG11_RX_MAC11, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_PG11_TX_MAC10 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG11_TX_MAC10, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_PG11_TX_MAC12 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG11_TX_MAC12, 24, 24));
        //} else {
        //    _BIST_REG_UPD(unit, MBIST_MAC_RESUME_02, 31, 0, 0xDC0000);
        //    _bist_mdelay(50);
        //    _BIST_REG_UPD(unit, MBIST_MAC_RESUME_02, 31, 0, 0x0);
        //    _bist_mdelay(100);
        //    if ((_BIST_REG_CHK(unit, MBIST_PG11_RX_MAC10, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_PG11_RX_MAC07, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_PG11_RX_MAC11, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_PG11_TX_MAC10, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_PG11_TX_MAC12, 28, 12, 0x10000)))
        //    {
        //        _BIST_PRINTF("MBIST_DRF_PG11 Fail\n");
        //    }
        //}

        //_BIST_REG_UPD(unit, MBIST_MAC_RESET_02, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_PG12======================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        //_BIST_REG_UPD(unit, MBIST_MAC_RESET_01, 31, 0, 0x39c000);
        //_bist_mdelay(100);

        //if ((_BIST_REG_CHK(unit, MBIST_PG12_RX_MAC03, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_PG12_RX_MAC09, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_PG12_RX_MAC07, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_PG12_TX_MAC02, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_PG12_PG_MAC13, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_PG12_PG_MAC14, 24, 24, 0x01)))
        //{
        //    _BIST_PRINTF(" wait 500ms MBIST_PG12_RX_MAC03 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG12_RX_MAC03, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_PG12_RX_MAC09 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG12_RX_MAC09, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_PG12_RX_MAC07 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG12_RX_MAC07, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_PG12_TX_MAC02 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG12_TX_MAC02, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_PG12_PG_MAC13 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG12_PG_MAC13, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_PG12_PG_MAC14 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG12_PG_MAC14, 24, 24));
        //} else {
        //    _BIST_REG_UPD(unit, MBIST_MAC_RESUME_01, 31, 0, 0x39c000);
        //    _bist_mdelay(50);
        //    _BIST_REG_UPD(unit, MBIST_MAC_RESUME_01, 31, 0, 0x0);
        //}

        //_bist_mdelay(100);
        //if ((_BIST_REG_CHK(unit, MBIST_PG12_RX_MAC03, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_PG12_RX_MAC09, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_PG12_RX_MAC07, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_PG12_TX_MAC02, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_PG12_PG_MAC13, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_PG12_PG_MAC14, 24, 24, 0x01)))
        //{
        //    _BIST_PRINTF(" wait 500ms MBIST_PG12_RX_MAC03 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG12_RX_MAC03, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_PG12_RX_MAC09 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG12_RX_MAC09, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_PG12_RX_MAC07 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG12_RX_MAC07, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_PG12_TX_MAC02 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG12_TX_MAC02, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_PG12_PG_MAC13 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG12_PG_MAC13, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_PG12_PG_MAC14 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PG12_PG_MAC14, 24, 24));
        //} else {
        //    _BIST_REG_UPD(unit, MBIST_MAC_RESUME_01, 31, 0, 0x39c000);
        //    _bist_mdelay(50);
        //    _BIST_REG_UPD(unit, MBIST_MAC_RESUME_01, 31, 0, 0x0);
        //    _bist_mdelay(100);
        //    if ((_BIST_REG_CHK(unit, MBIST_PG12_RX_MAC03, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_PG12_RX_MAC09, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_PG12_RX_MAC07, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_PG12_TX_MAC02, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_PG12_PG_MAC13, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_PG12_PG_MAC14, 28, 12, 0x10000)))
        //    {
        //        _BIST_PRINTF("MBIST_DRF_PG12 Fail\n");
        //    }
        //}

        _BIST_REG_UPD(unit, MBIST_MAC_RESET_01, 31, 0, 0x0);
    }

    /*
     * ALE_DRF_MBIST
     */

    _BIST_PRINTF("==============DRF_MBIST_ALE1~8======================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        _BIST_REG_UPD(unit, MBIST_ALE_START_0, 31, 0, (0xff000000 | 0xff0000 | 0xff00 | 0xff));
        _bist_mdelay(100);

        if ((_BIST_REG_CHK(unit, MBIST_ALE01, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE02, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE03, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE04, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE05, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE06, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE07, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE08, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE09, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE10, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE011, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE012, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE13, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE14, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE15, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE16, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE17, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE18, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE19, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE20, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE21, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE22, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE23, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE24, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE25, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE26, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE27, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE28, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE29, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE30, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE31, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE32, 24, 24, 0x01))
            )
        {
            _BIST_PRINTF(" wait 500ms MBIST_ALE01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE01, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE02 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE02, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE03 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE03, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE04 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE04, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE05 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE05, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE06 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE06, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE07 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE07, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE08 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE08, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE09 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE09, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE10 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE10, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE011 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE011, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE012 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE012, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE13 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE13, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE14 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE14, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE15 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE15, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE16 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE16, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE17 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE17, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE18 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE18, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE19 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE19, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE20 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE20, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE21 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE21, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE22 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE22, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE23 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE23, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE24 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE24, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_ALE_RESUME_0, 31, 0, (0xff000000 | 0xff0000 | 0xff00 | 0xff));
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_ALE_RESUME_0, 31, 0, 0x0);
        }

        _bist_mdelay(100);
        if ((_BIST_REG_CHK(unit, MBIST_ALE01, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE02, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE03, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE04, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE05, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE06, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE07, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE08, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE09, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE10, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE011, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE012, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE13, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE14, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE15, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE16, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE17, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE18, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE19, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE20, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE21, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE22, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE23, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE24, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE25, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE26, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE27, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE28, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE29, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE30, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE31, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE32, 24, 24, 0x01))
            )
        {
            _BIST_PRINTF(" wait 500ms MBIST_ALE01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE01, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE02 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE02, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE03 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE03, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE04 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE04, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE05 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE05, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE06 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE06, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE07 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE07, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE08 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE08, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE09 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE09, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE10 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE10, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE011 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE011, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE012 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE012, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE13 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE13, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE14 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE14, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE15 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE15, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE16 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE16, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE17 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE17, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE18 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE18, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE19 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE19, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE20 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE20, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE21 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE21, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE22 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE22, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE23 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE23, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE24 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE24, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE25 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE25, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE26 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE26, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE27 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE27, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE28 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE28, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE29 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE29, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE30 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE30, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE31 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE31, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE32 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE32, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_ALE_RESUME_0, 31, 0, (0xff000000 | 0xff0000 | 0xff00 | 0xff));
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_ALE_RESUME_0, 31, 0, 0x0);
            _bist_mdelay(100);
            if ((_BIST_REG_CHK(unit, MBIST_ALE01, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ALE02, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ALE03, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ALE04, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ALE05, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ALE06, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ALE07, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ALE08, 28, 12, 0x10000)))
            {
                _BIST_PRINTF("MBIST_DRF_ALE1~8 have Fail\n");
            }
            if ((_BIST_REG_CHK(unit, MBIST_ALE09, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ALE10, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ALE011, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ALE012, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ALE13, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ALE14, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ALE15, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ALE16, 28, 12, 0x10000)))
            {
                _BIST_PRINTF("MBIST_DRF_ALE9~16 have Fail\n");
            }
            if ((_BIST_REG_CHK(unit, MBIST_ALE17, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ALE18, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ALE19, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ALE20, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ALE21, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ALE22, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ALE23, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ALE24, 28, 12, 0x10000)))
            {
                _BIST_PRINTF("MBIST_DRF_ALE17~24 have Fail\n");
            }
            if ((_BIST_REG_CHK(unit, MBIST_ALE25, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ALE26, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ALE27, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ALE28, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ALE29, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ALE30, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ALE31, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ALE32, 28, 12, 0x10000)))
            {
                _BIST_PRINTF("MBIST_DRF_ALE25~32 have Fail\n");
            }
        }

        _BIST_REG_UPD(unit, MBIST_ALE_START_0, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============DRF_MBIST_ALE9~16======================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        //_BIST_REG_UPD(unit, MBIST_ALE_START_0, 31, 0, 0xff00);
        //_bist_mdelay(100);

        //if ((_BIST_REG_CHK(unit, MBIST_ALE09, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_ALE10, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_ALE011, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_ALE012, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_ALE13, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_ALE14, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_ALE15, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_ALE16, 24, 24, 0x01)))
        //{
        //    _BIST_PRINTF(" wait 500ms MBIST_ALE09 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE09, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_ALE10 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE10, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_ALE011 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE011, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_ALE012 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE012, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_ALE13 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE13, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_ALE14 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE14, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_ALE15 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE15, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_ALE16 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE16, 24, 24));
        //} else {
        //    _BIST_REG_UPD(unit, MBIST_ALE_RESUME_0, 31, 0, 0xff00);
        //    _bist_mdelay(50);
        //    _BIST_REG_UPD(unit, MBIST_ALE_RESUME_0, 31, 0, 0x0);
        //}

        //_bist_mdelay(100);
        //if ((_BIST_REG_CHK(unit, MBIST_ALE09, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_ALE10, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_ALE011, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_ALE012, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_ALE13, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_ALE14, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_ALE15, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_ALE16, 24, 24, 0x01)))
        //{
        //    _BIST_PRINTF(" wait 500ms MBIST_ALE09 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE09, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_ALE10 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE10, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_ALE011 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE011, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_ALE012 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE012, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_ALE13 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE13, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_ALE14 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE14, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_ALE15 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE15, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_ALE16 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE16, 24, 24));
        //} else {
        //    _BIST_REG_UPD(unit, MBIST_ALE_RESUME_0, 31, 0, 0xff00);
        //    _bist_mdelay(50);
        //    _BIST_REG_UPD(unit, MBIST_ALE_RESUME_0, 31, 0, 0x0);
        //    _bist_mdelay(100);
        //    if ((_BIST_REG_CHK(unit, MBIST_ALE09, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_ALE10, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_ALE011, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_ALE012, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_ALE13, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_ALE14, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_ALE15, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_ALE16, 28, 12, 0x10000)))
        //    {
        //        _BIST_PRINTF("MBIST_DRF_ALE9~16 have Fail\n");
        //    }
        //}

        //_BIST_REG_UPD(unit, MBIST_ALE_START_0, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============DRF_MBIST_ALE17~24======================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        //_BIST_REG_UPD(unit, MBIST_ALE_START_0, 31, 0, 0xff0000);
        //_bist_mdelay(100);

        //if ((_BIST_REG_CHK(unit, MBIST_ALE17, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_ALE18, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_ALE19, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_ALE20, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_ALE21, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_ALE22, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_ALE23, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_ALE24, 24, 24, 0x01)))
        //{
        //    _BIST_PRINTF(" wait 500ms MBIST_ALE17 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE17, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_ALE18 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE18, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_ALE19 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE19, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_ALE20 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE20, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_ALE21 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE21, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_ALE22 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE22, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_ALE23 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE23, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_ALE24 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE24, 24, 24));
        //} else {
        //    _BIST_REG_UPD(unit, MBIST_ALE_RESUME_0, 31, 0, 0xff0000);
        //    _bist_mdelay(50);
        //    _BIST_REG_UPD(unit, MBIST_ALE_RESUME_0, 31, 0, 0x0);
        //}

        //_bist_mdelay(100);
        //if ((_BIST_REG_CHK(unit, MBIST_ALE17, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_ALE18, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_ALE19, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_ALE20, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_ALE21, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_ALE22, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_ALE23, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_ALE24, 24, 24, 0x01)))
        //{
        //    _BIST_PRINTF(" wait 500ms MBIST_ALE17 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE17, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_ALE18 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE18, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_ALE19 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE19, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_ALE20 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE20, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_ALE21 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE21, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_ALE22 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE22, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_ALE23 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE23, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_ALE24 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE24, 24, 24));
        //} else {
        //    _BIST_REG_UPD(unit, MBIST_ALE_RESUME_0, 31, 0, 0xff0000);
        //    _bist_mdelay(50);
        //    _BIST_REG_UPD(unit, MBIST_ALE_RESUME_0, 31, 0, 0x0);
        //    _bist_mdelay(100);
        //    if ((_BIST_REG_CHK(unit, MBIST_ALE17, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_ALE18, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_ALE19, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_ALE20, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_ALE21, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_ALE22, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_ALE23, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_ALE24, 28, 12, 0x10000)))
        //    {
        //        _BIST_PRINTF("MBIST_DRF_ALE17~24 have Fail\n");
        //    }
        //}

        //_BIST_REG_UPD(unit, MBIST_ALE_START_0, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============DRF_MBIST_ALE25~32======================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        //_BIST_REG_UPD(unit, MBIST_ALE_START_0, 31, 0, 0xff000000);
        //_bist_mdelay(100);

        //if ((_BIST_REG_CHK(unit, MBIST_ALE25, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_ALE26, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_ALE27, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_ALE28, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_ALE29, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_ALE30, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_ALE31, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_ALE32, 24, 24, 0x01)))
        //{
        //    _BIST_PRINTF(" wait 500ms MBIST_ALE25 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE25, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_ALE26 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE26, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_ALE27 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE27, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_ALE28 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE28, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_ALE29 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE29, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_ALE30 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE30, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_ALE31 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE31, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_ALE32 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE32, 24, 24));
        //} else {
        //    _BIST_REG_UPD(unit, MBIST_ALE_RESUME_0, 31, 0, 0xff000000);
        //    _bist_mdelay(50);
        //    _BIST_REG_UPD(unit, MBIST_ALE_RESUME_0, 31, 0, 0x0);
        //}

        //_bist_mdelay(100);
        //if ((_BIST_REG_CHK(unit, MBIST_ALE25, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_ALE26, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_ALE27, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_ALE28, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_ALE29, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_ALE30, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_ALE31, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_ALE32, 24, 24, 0x01)))
        //{
        //    _BIST_PRINTF(" wait 500ms MBIST_ALE25 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE25, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_ALE26 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE26, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_ALE27 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE27, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_ALE28 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE28, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_ALE29 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE29, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_ALE30 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE30, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_ALE31 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE31, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_ALE32 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE32, 24, 24));
        //} else {
        //    _BIST_REG_UPD(unit, MBIST_ALE_RESUME_0, 31, 0, 0xff000000);
        //    _bist_mdelay(50);
        //    _BIST_REG_UPD(unit, MBIST_ALE_RESUME_0, 31, 0, 0x0);
        //    _bist_mdelay(100);
        //    if ((_BIST_REG_CHK(unit, MBIST_ALE25, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_ALE26, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_ALE27, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_ALE28, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_ALE29, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_ALE30, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_ALE31, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_ALE32, 28, 12, 0x10000)))
        //    {
        //        _BIST_PRINTF("MBIST_DRF_ALE25~32 have Fail\n");
        //    }
        //}

        //_BIST_REG_UPD(unit, MBIST_ALE_START_0, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============DRF_MBIST_ALE33~39======================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        _BIST_REG_UPD(unit, MBIST_ALE_START_1, 31, 0, 0x7f);
        _bist_mdelay(100);

        if ((_BIST_REG_CHK(unit, MBIST_ALE33, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE34, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE35, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE36, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE37, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE38, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE39, 24, 24, 0x01)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_ALE33 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE33, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE34 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE34, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE35 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE35, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE36 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE36, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE37 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE37, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE38 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE38, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE39 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE39, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_ALE_RESUME_1, 31, 0, 0x7f);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_ALE_RESUME_1, 31, 0, 0x0);
        }

        _bist_mdelay(100);
        if ((_BIST_REG_CHK(unit, MBIST_ALE33, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE34, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE35, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE36, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE37, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE38, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ALE39, 24, 24, 0x01)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_ALE33 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE33, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE34 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE34, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE35 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE35, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE36 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE36, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE37 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE37, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE38 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE38, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ALE39 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE39, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_ALE_RESUME_1, 31, 0, 0x7f);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_ALE_RESUME_1, 31, 0, 0x0);
            _bist_mdelay(100);
            if ((_BIST_REG_CHK(unit, MBIST_ALE33, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ALE34, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ALE35, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ALE36, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ALE37, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ALE38, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ALE39, 28, 12, 0x10000)))
            {
                _BIST_PRINTF(" wait 500ms MBIST_ALE33 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE33, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_ALE34 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE34, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_ALE35 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE35, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_ALE36 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE36, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_ALE37 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE37, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_ALE38 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE38, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_ALE39 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ALE39, 28, 12));
                _BIST_PRINTF("MBIST_DRF_ALE25~32 have Fail\n");
            }
        }

        _BIST_REG_UPD(unit, MBIST_ALE_START_1, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============DRF_MACL1_TCM00~07======================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        _BIST_REG_UPD(unit, MBIST_ACL_START, 31, 0, (0xff00000 | 0xf0000 | 0xff00 | 0xff));
        _bist_mdelay(100);

        if ((_BIST_REG_CHK(unit, MBIST_TCM00_ACL1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_TCM01_ACL1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_TCM02_ACL1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_TCM03_ACL1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_TCM04_ACL1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_TCM05_ACL1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_TCM06_ACL1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_TCM07_ACL1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_TCM08_ACL1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_TCM09_ACL1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_TCM10_ACL1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_TCM11_ACL1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_TCM12_ACL1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_TCM13_ACL1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_TCM14_ACL1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_TCM15_ACL1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_LOG00_ACL2, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_LOG01_ACL2, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_LOG02_ACL2, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_LOG03_ACL2, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ACT00_ACL3, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ACT01_ACL3, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ACT02_ACL3, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ACT03_ACL3, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ACT04_ACL3, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ACT05_ACL3, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ACT06_ACL3, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ACT07_ACL3, 24, 24, 0x01))
            )
        {
            _BIST_PRINTF(" wait 500ms MBIST_TCM00_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM00_ACL1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_TCM01_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM01_ACL1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_TCM02_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM02_ACL1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_TCM03_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM03_ACL1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_TCM04_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM04_ACL1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_TCM05_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM05_ACL1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_TCM06_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM06_ACL1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_TCM07_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM07_ACL1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_TCM08_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM08_ACL1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_TCM09_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM09_ACL1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_TCM10_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM10_ACL1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_TCM11_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM11_ACL1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_TCM12_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM12_ACL1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_TCM13_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM13_ACL1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_TCM14_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM14_ACL1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_TCM15_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM15_ACL1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_LOG00_ACL2 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_LOG00_ACL2, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_LOG01_ACL2 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_LOG01_ACL2, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_LOG02_ACL2 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_LOG02_ACL2, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_LOG03_ACL2 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_LOG03_ACL2, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ACT00_ACL3 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ACT00_ACL3, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ACT01_ACL3 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ACT01_ACL3, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ACT02_ACL3 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ACT02_ACL3, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ACT03_ACL3 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ACT03_ACL3, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ACT04_ACL3 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ACT04_ACL3, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ACT05_ACL3 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ACT05_ACL3, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ACT06_ACL3 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ACT06_ACL3, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ACT07_ACL3 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ACT07_ACL3, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_ACL_RESUME, 31, 0, (0xff00000 | 0xf0000 | 0xff00 | 0xff));
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_ACL_RESUME, 31, 0, 0x0);
        }

        _bist_mdelay(100);
        if ((_BIST_REG_CHK(unit, MBIST_TCM00_ACL1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_TCM01_ACL1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_TCM02_ACL1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_TCM03_ACL1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_TCM04_ACL1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_TCM05_ACL1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_TCM06_ACL1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_TCM07_ACL1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_TCM08_ACL1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_TCM09_ACL1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_TCM10_ACL1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_TCM11_ACL1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_TCM12_ACL1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_TCM13_ACL1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_TCM14_ACL1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_TCM15_ACL1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_LOG00_ACL2, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_LOG01_ACL2, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_LOG02_ACL2, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_LOG03_ACL2, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ACT00_ACL3, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ACT01_ACL3, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ACT02_ACL3, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ACT03_ACL3, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ACT04_ACL3, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ACT05_ACL3, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ACT06_ACL3, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ACT07_ACL3, 24, 24, 0x01))
            )
        {
            _BIST_PRINTF(" wait 500ms MBIST_TCM00_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM00_ACL1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_TCM01_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM01_ACL1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_TCM02_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM02_ACL1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_TCM03_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM03_ACL1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_TCM04_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM04_ACL1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_TCM05_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM05_ACL1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_TCM06_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM06_ACL1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_TCM07_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM07_ACL1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_TCM08_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM08_ACL1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_TCM09_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM09_ACL1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_TCM10_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM10_ACL1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_TCM11_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM11_ACL1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_TCM12_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM12_ACL1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_TCM13_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM13_ACL1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_TCM14_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM14_ACL1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_TCM15_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM15_ACL1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_LOG00_ACL2 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_LOG00_ACL2, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_LOG01_ACL2 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_LOG01_ACL2, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_LOG02_ACL2 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_LOG02_ACL2, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_LOG03_ACL2 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_LOG03_ACL2, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ACT00_ACL3 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ACT00_ACL3, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ACT01_ACL3 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ACT01_ACL3, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ACT02_ACL3 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ACT02_ACL3, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ACT03_ACL3 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ACT03_ACL3, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ACT04_ACL3 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ACT04_ACL3, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ACT05_ACL3 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ACT05_ACL3, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ACT06_ACL3 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ACT06_ACL3, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_ACT07_ACL3 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ACT07_ACL3, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_ACL_RESUME, 31, 0, (0xff00000 | 0xf0000 | 0xff00 | 0xff));
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_ACL_RESUME, 31, 0, 0x0);
            _bist_mdelay(100);
            if ((_BIST_REG_CHK(unit, MBIST_TCM00_ACL1, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_TCM01_ACL1, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_TCM02_ACL1, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_TCM03_ACL1, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_TCM04_ACL1, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_TCM05_ACL1, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_TCM06_ACL1, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_TCM07_ACL1, 28, 12, 0x10000)))
            {
                _BIST_PRINTF("MBIST_DRF_TCM00~07_ACL1 have Fail\n");
            }

            if ((_BIST_REG_CHK(unit, MBIST_TCM08_ACL1, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_TCM09_ACL1, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_TCM10_ACL1, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_TCM11_ACL1, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_TCM12_ACL1, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_TCM13_ACL1, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_TCM14_ACL1, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_TCM15_ACL1, 28, 12, 0x10000)))
            {
                _BIST_PRINTF("MBIST_DRF_TCM08~15_ACL1 have Fail\n");
            }

            if ((_BIST_REG_CHK(unit, MBIST_LOG00_ACL2, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_LOG01_ACL2, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_LOG02_ACL2, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_LOG03_ACL2, 28, 12, 0x10000)))
            {
                _BIST_PRINTF("MBIST_DRF_LOG00~03_ACL2 have Fail\n");
            }
            if ((_BIST_REG_CHK(unit, MBIST_ACT00_ACL3, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ACT01_ACL3, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ACT02_ACL3, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ACT03_ACL3, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ACT04_ACL3, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ACT05_ACL3, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ACT06_ACL3, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_ACT07_ACL3, 28, 12, 0x10000)))
            {
                _BIST_PRINTF("MBIST_DRF_LOG00~03_ACL2 have Fail\n");
            }
        }

        _BIST_REG_UPD(unit, MBIST_ACL_START, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============DRF_MACL1_TCM08~15======================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        //_BIST_REG_UPD(unit, MBIST_ACL_START, 31, 0, 0xff00);
        //_bist_mdelay(100);

        //if ((_BIST_REG_CHK(unit, MBIST_TCM08_ACL1, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_TCM09_ACL1, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_TCM10_ACL1, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_TCM11_ACL1, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_TCM12_ACL1, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_TCM13_ACL1, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_TCM14_ACL1, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_TCM15_ACL1, 24, 24, 0x01)))
        //{
        //    _BIST_PRINTF(" wait 500ms MBIST_TCM08_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM08_ACL1, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_TCM09_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM09_ACL1, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_TCM10_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM10_ACL1, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_TCM11_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM11_ACL1, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_TCM12_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM12_ACL1, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_TCM13_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM13_ACL1, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_TCM14_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM14_ACL1, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_TCM15_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM15_ACL1, 24, 24));
        //} else {
        //    _BIST_REG_UPD(unit, MBIST_ACL_RESUME, 31, 0, 0xff00);
        //    _bist_mdelay(50);
        //    _BIST_REG_UPD(unit, MBIST_ACL_RESUME, 31, 0, 0x0);
        //}

        //_bist_mdelay(100);
        //if ((_BIST_REG_CHK(unit, MBIST_TCM08_ACL1, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_TCM09_ACL1, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_TCM10_ACL1, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_TCM11_ACL1, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_TCM12_ACL1, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_TCM13_ACL1, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_TCM14_ACL1, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_TCM15_ACL1, 24, 24, 0x01)))
        //{
        //    _BIST_PRINTF(" wait 500ms MBIST_TCM08_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM08_ACL1, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_TCM09_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM09_ACL1, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_TCM10_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM10_ACL1, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_TCM11_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM11_ACL1, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_TCM12_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM12_ACL1, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_TCM13_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM13_ACL1, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_TCM14_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM14_ACL1, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_TCM15_ACL1 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_TCM15_ACL1, 24, 24));
        //} else {
        //    _BIST_REG_UPD(unit, MBIST_ACL_RESUME, 31, 0, 0xff00);
        //    _bist_mdelay(50);
        //    _BIST_REG_UPD(unit, MBIST_ACL_RESUME, 31, 0, 0x0);
        //    _bist_mdelay(100);
        //    if ((_BIST_REG_CHK(unit, MBIST_TCM08_ACL1, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_TCM09_ACL1, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_TCM10_ACL1, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_TCM11_ACL1, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_TCM12_ACL1, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_TCM13_ACL1, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_TCM14_ACL1, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_TCM15_ACL1, 28, 12, 0x10000)))
        //    {
        //        _BIST_PRINTF("MBIST_DRF_TCM08~15_ACL1 have Fail\n");
        //    }
        //}

        //_BIST_REG_UPD(unit, MBIST_ACL_START, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============DRF_MACL2_LOG00~03======================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        //_BIST_REG_UPD(unit, MBIST_ACL_START, 31, 0, 0xf0000);
        //_bist_mdelay(100);

        //if ((_BIST_REG_CHK(unit, MBIST_LOG00_ACL2, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_LOG01_ACL2, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_LOG02_ACL2, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_LOG03_ACL2, 24, 24, 0x01)))
        //{
        //    _BIST_PRINTF(" wait 500ms MBIST_LOG00_ACL2 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_LOG00_ACL2, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_LOG01_ACL2 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_LOG01_ACL2, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_LOG02_ACL2 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_LOG02_ACL2, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_LOG03_ACL2 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_LOG03_ACL2, 24, 24));
        //} else {
        //    _BIST_REG_UPD(unit, MBIST_ACL_RESUME, 31, 0, 0xf0000);
        //    _bist_mdelay(50);
        //    _BIST_REG_UPD(unit, MBIST_ACL_RESUME, 31, 0, 0x0);
        //}

        //_bist_mdelay(100);
        //if ((_BIST_REG_CHK(unit, MBIST_LOG00_ACL2, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_LOG01_ACL2, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_LOG02_ACL2, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_LOG03_ACL2, 24, 24, 0x01)))
        //{
        //    _BIST_PRINTF(" wait 500ms MBIST_LOG00_ACL2 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_LOG00_ACL2, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_LOG01_ACL2 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_LOG01_ACL2, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_LOG02_ACL2 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_LOG02_ACL2, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_LOG03_ACL2 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_LOG03_ACL2, 24, 24));
        //} else {
        //    _BIST_REG_UPD(unit, MBIST_ACL_RESUME, 31, 0, 0xf0000);
        //    _bist_mdelay(50);
        //    _BIST_REG_UPD(unit, MBIST_ACL_RESUME, 31, 0, 0x0);
        //    _bist_mdelay(100);
        //    if ((_BIST_REG_CHK(unit, MBIST_LOG00_ACL2, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_LOG01_ACL2, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_LOG02_ACL2, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_LOG03_ACL2, 28, 12, 0x10000)))
        //    {
        //        _BIST_PRINTF("MBIST_DRF_LOG00~03_ACL2 have Fail\n");
        //    }
        //}

        //_BIST_REG_UPD(unit, MBIST_ACL_START, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============DRF_MACL3_ACT00~07======================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        //_BIST_REG_UPD(unit, MBIST_ACL_START, 31, 0, 0xff00000);
        //_bist_mdelay(100);

        //if ((_BIST_REG_CHK(unit, MBIST_ACT00_ACL3, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_ACT01_ACL3, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_ACT02_ACL3, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_ACT03_ACL3, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_ACT04_ACL3, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_ACT05_ACL3, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_ACT06_ACL3, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_ACT07_ACL3, 24, 24, 0x01)))
        //{
        //    _BIST_PRINTF(" wait 500ms MBIST_ACT00_ACL3 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ACT00_ACL3, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_ACT01_ACL3 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ACT01_ACL3, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_ACT02_ACL3 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ACT02_ACL3, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_ACT03_ACL3 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ACT03_ACL3, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_ACT04_ACL3 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ACT04_ACL3, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_ACT05_ACL3 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ACT05_ACL3, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_ACT06_ACL3 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ACT06_ACL3, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_ACT07_ACL3 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ACT07_ACL3, 24, 24));
        //} else {
        //    _BIST_REG_UPD(unit, MBIST_ACL_RESUME, 31, 0, 0xff00000);
        //    _bist_mdelay(50);
        //    _BIST_REG_UPD(unit, MBIST_ACL_RESUME, 31, 0, 0x0);
        //}

        //_bist_mdelay(100);
        //if ((_BIST_REG_CHK(unit, MBIST_ACT00_ACL3, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_ACT01_ACL3, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_ACT02_ACL3, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_ACT03_ACL3, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_ACT04_ACL3, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_ACT05_ACL3, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_ACT06_ACL3, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_ACT07_ACL3, 24, 24, 0x01)))
        //{
        //    _BIST_PRINTF(" wait 500ms MBIST_ACT00_ACL3 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ACT00_ACL3, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_ACT01_ACL3 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ACT01_ACL3, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_ACT02_ACL3 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ACT02_ACL3, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_ACT03_ACL3 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ACT03_ACL3, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_ACT04_ACL3 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ACT04_ACL3, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_ACT05_ACL3 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ACT05_ACL3, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_ACT06_ACL3 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ACT06_ACL3, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_ACT07_ACL3 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ACT07_ACL3, 24, 24));
        //} else {
        //    _BIST_REG_UPD(unit, MBIST_ACL_RESUME, 31, 0, 0xff00000);
        //    _bist_mdelay(50);
        //    _BIST_REG_UPD(unit, MBIST_ACL_RESUME, 31, 0, 0x0);
        //    _bist_mdelay(100);
        //    if ((_BIST_REG_CHK(unit, MBIST_ACT00_ACL3, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_ACT01_ACL3, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_ACT02_ACL3, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_ACT03_ACL3, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_ACT04_ACL3, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_ACT05_ACL3, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_ACT06_ACL3, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_ACT07_ACL3, 28, 12, 0x10000)))
        //    {
        //        _BIST_PRINTF("MBIST_DRF_LOG00~03_ACL2 have Fail\n");
        //    }
        //}

        //_BIST_REG_UPD(unit, MBIST_ACL_START, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_DRF_MIB 00~02======================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        _BIST_REG_UPD(unit, MBIST_MIB_START, 31, 0, 0x7);
        _bist_mdelay(100);

        if ((_BIST_REG_CHK(unit, MBIST_MIB00, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_MIB01, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_MIB02, 24, 24, 0x01)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_MIB00 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_MIB00, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_MIB01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_MIB01, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_MIB02 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_MIB02, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_MIB_RESUME, 31, 0, 0x7);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_MIB_RESUME, 31, 0, 0x0);
        }

        _bist_mdelay(100);
        if ((_BIST_REG_CHK(unit, MBIST_MIB00, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_MIB01, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_MIB02, 24, 24, 0x01)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_MIB00 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_MIB00, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_MIB01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_MIB01, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_MIB02 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_MIB02, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_MIB_RESUME, 31, 0, 0x7);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_MIB_RESUME, 31, 0, 0x0);
            _bist_mdelay(100);
            if ((_BIST_REG_CHK(unit, MBIST_MIB00, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_MIB01, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_MIB02, 28, 12, 0x10000)))
            {
                _BIST_PRINTF(" wait 500ms MBIST_MIB00 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_MIB00, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_MIB01 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_MIB01, 28, 12));
                _BIST_PRINTF(" wait 500ms MBIST_MIB02 = 0x%08X\n", _BIST_REG_RET(unit, MBIST_MIB02, 28, 12));
                _BIST_PRINTF("MBIST_DRF_MIB00~02 have Fail\n");
            }
        }

        _BIST_REG_UPD(unit, MBIST_MIB_START, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_DRF_ENCAP test======================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        _BIST_REG_UPD(unit, MBIST_ENCAP_START, 31, 0, (0x4000000 | 0x3fC0000 | 0x3fC00 | 0x380 | 0x7f));
        _bist_mdelay(100);

        if ((_BIST_REG_CHK(unit, MBIST_ENCAP10, 31, 24, 0x0f)) &&
            (_BIST_REG_CHK(unit, MBIST_ENCAP10, 23, 16, 0x0f)) &&
            (_BIST_REG_CHK(unit, MBIST_ENCAP10, 15, 8, 0x0f)) &&
            (_BIST_REG_CHK(unit, MBIST_ENCAP10,  7, 0, 0x0f)) &&
            (_BIST_REG_CHK(unit, MBIST_ENCAP_TCAM, 16, 16, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ENCAP13, 30, 30, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ENCAP13, 27, 20, 0x44)) &&
            (_BIST_REG_CHK(unit, MBIST_ENCAP13, 19, 12, 0x44)) &&
            (_BIST_REG_CHK(unit, MBIST_ENCAP13, 10, 10, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ENCAP13,  6,  6, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ENCAP13,  2,  2, 0x01)) &&
            (_BIST_REG_CHK(unit, MBISR_ENCAP01, 16, 16, 0x01)) &&
            (_BIST_REG_CHK(unit, MBISR_ENCAP02, 16, 16, 0x01))
            )
        {
            _BIST_PRINTF(" wait 500ms mbist_MPG1~4_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ENCAP10, 31, 24));
            _BIST_PRINTF(" wait 500ms mbist_MPT1~4_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ENCAP10, 23, 16));
            _BIST_PRINTF(" wait 500ms mbist_MPF1~4_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ENCAP10, 15, 8));
            _BIST_PRINTF(" wait 500ms mbist_MPHF1~4_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ENCAP10, 7, 0));
            _BIST_PRINTF(" wait 500ms MBIST_ENCAP_TCAM = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ENCAP_TCAM, 16, 16));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_ENCAP_MKELL_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ENCAP13, 30, 30));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_ENCAP_MTS_pause = 0x%08X\n",   _BIST_REG_RET(unit, MBIST_ENCAP13, 27, 20));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_ENCAP_MRMK_pause = 0x%08X\n",  _BIST_REG_RET(unit, MBIST_ENCAP13, 19, 12));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_ENCAP_MEVC_pause = 0x%08X\n",  _BIST_REG_RET(unit, MBIST_ENCAP13, 10, 10));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_ENCAP_MPLS_pause = 0x%08X\n",  _BIST_REG_RET(unit, MBIST_ENCAP13,  6,  6));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_ENCAP_MBS0_pause = 0x%08X\n",   _BIST_REG_RET(unit, MBIST_ENCAP13,  2,  2));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_ENCAP_MBS1_pause = 0x%08X\n",   _BIST_REG_RET(unit, MBISR_ENCAP01, 16, 16));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_ENCAP_MUNTAG_pause = 0x%08X\n", _BIST_REG_RET(unit, MBISR_ENCAP02, 16, 16));
        } else {
            _BIST_REG_UPD(unit, MBIST_ENCAP_RESUME, 31, 0, (0x4000000 | 0x3fC0000 | 0x3fC00 | 0x380 | 0x7f));
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_ENCAP_RESUME, 31, 0, 0x0);
        }

        _bist_mdelay(100);
        if ((_BIST_REG_CHK(unit, MBIST_ENCAP10, 31, 24, 0x0f)) &&
            (_BIST_REG_CHK(unit, MBIST_ENCAP10, 23, 16, 0x0f)) &&
            (_BIST_REG_CHK(unit, MBIST_ENCAP10, 15,  8, 0x0f)) &&
            (_BIST_REG_CHK(unit, MBIST_ENCAP10,  7,  0, 0x0f)) &&
            (_BIST_REG_CHK(unit, MBIST_ENCAP_TCAM, 16, 16, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ENCAP13, 30, 30, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ENCAP13, 27, 20, 0x44)) &&
            (_BIST_REG_CHK(unit, MBIST_ENCAP13, 19, 12, 0x44)) &&
            (_BIST_REG_CHK(unit, MBIST_ENCAP13, 10, 10, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ENCAP13,  6,  6, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_ENCAP13,  2,  2, 0x01)) &&
            (_BIST_REG_CHK(unit, MBISR_ENCAP01, 16, 16, 0x01)) &&
            (_BIST_REG_CHK(unit, MBISR_ENCAP02, 16, 16, 0x01))
            )
        {
            _BIST_PRINTF(" wait 500ms mbist_MPG1~4_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ENCAP10, 31, 24));
            _BIST_PRINTF(" wait 500ms mbist_MPT1~4_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ENCAP10, 23, 16));
            _BIST_PRINTF(" wait 500ms mbist_MPF1~4_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ENCAP10, 15,  8));
            _BIST_PRINTF(" wait 500ms mbist_MPHF1~4_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ENCAP10,  7,  0));
            _BIST_PRINTF(" wait 500ms MBIST_ENCAP_TCAM = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ENCAP_TCAM, 16, 16));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_ENCAP_MKELL_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ENCAP13, 30, 30));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_ENCAP_MTS_pause = 0x%08X\n",   _BIST_REG_RET(unit, MBIST_ENCAP13, 27, 20));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_ENCAP_MRMK_pause = 0x%08X\n",  _BIST_REG_RET(unit, MBIST_ENCAP13, 19, 12));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_ENCAP_MEVC_pause = 0x%08X\n",  _BIST_REG_RET(unit, MBIST_ENCAP13, 10, 10));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_ENCAP_MPLS_pause = 0x%08X\n",  _BIST_REG_RET(unit, MBIST_ENCAP13,  6,  6));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_ENCAP_MBS0_pause = 0x%08X\n",   _BIST_REG_RET(unit, MBIST_ENCAP13,  2,  2));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_ENCAP_MBS1_pause = 0x%08X\n",   _BIST_REG_RET(unit, MBISR_ENCAP01, 16, 16));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_ENCAP_MUNTAG_pause = 0x%08X\n", _BIST_REG_RET(unit, MBISR_ENCAP02, 16, 16));
        } else {
            _BIST_REG_UPD(unit, MBIST_ENCAP_RESUME, 31, 0, (0x4000000 | 0x3fC0000 | 0x3fC00 | 0x380 | 0x7f));
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_ENCAP_RESUME, 31, 0, 0x0);
            _bist_mdelay(100);

            if ((_BIST_REG_CHK(unit, MBIST_ENCAP02, 31, 0, 0x0)) ||
                (_BIST_REG_CHK(unit, MBIST_ENCAP09, 31, 24, 0x55)))
            {
                _BIST_PRINTF("MBIST_DRF_ENCAP_MPG00~04 have Fail\n");
            }

            if ((_BIST_REG_CHK(unit, MBIST_ENCAP04, 31, 0, 0x0)) ||
                (_BIST_REG_CHK(unit, MBIST_ENCAP09, 23, 16, 0x55)))
            {
                _BIST_PRINTF("MBIST_DRF_ENCAP_MPT00~04 have Fail\n");
            }


            if ((_BIST_REG_CHK(unit, MBIST_ENCAP06, 31, 0, 0x0)) ||
                (_BIST_REG_CHK(unit, MBIST_ENCAP09, 15, 8, 0x55)))
            {
                _BIST_PRINTF("MBIST_DRF_ENCAP_MPF00~04 have Fail\n");
            }

            if ((_BIST_REG_CHK(unit, MBIST_ENCAP08, 31, 0, 0x0)) ||
                (_BIST_REG_CHK(unit, MBIST_ENCAP09,  7, 0, 0x55)))
            {
                _BIST_PRINTF("MBIST_DRF_ENCAP_MPHF00~04 have Fail\n");
            }


            if ((_BIST_REG_CHK(unit, MBIST_ENCAP_TCAM, 31, 0, 0x1000000)))
            {
                _BIST_PRINTF("MBIST_DRF_ENCAP_MTCAM Fail\n");
            }


            if ((_BIST_REG_CHK(unit, MBIST_ENCAP12, 28, 24, 0x0)) ||
                (_BIST_REG_CHK(unit, MBIST_ENCAP13, 30, 28, 0x1)))
            {
                _BIST_PRINTF("MBIST_DRF_ENCAP_MKELL Fail\n");
            }

            if ((_BIST_REG_CHK(unit, MBIST_ENCAP12, 21, 16, 0x0)) ||
                (_BIST_REG_CHK(unit, MBIST_ENCAP13, 27, 20, 0x11)))
            {
                _BIST_PRINTF("MBIST_DRF_ENCAP_MTS Fail\n");
            }

            if ((_BIST_REG_CHK(unit, MBIST_ENCAP12, 13, 12, 0x0)) ||
                (_BIST_REG_CHK(unit, MBIST_ENCAP13, 19, 12, 0x11)))
            {
                _BIST_PRINTF("MBIST_DRF_ENCAP_MRMK Fail\n");
            }

            if ((_BIST_REG_CHK(unit, MBIST_ENCAP12,  8,  8, 0x0)) ||
                (_BIST_REG_CHK(unit, MBIST_ENCAP13, 11,  8, 0x1)))
            {
                _BIST_PRINTF("MBIST_DRF_ENCAP_MEVC Fail\n");
            }


            if ((_BIST_REG_CHK(unit, MBIST_ENCAP12,  4,  4, 0x0)) ||
                (_BIST_REG_CHK(unit, MBIST_ENCAP13,  7,  4, 0x1)))
            {
                _BIST_PRINTF("MBIST_DRF_ENCAP_MMPLS Fail\n");
            }


            if ((_BIST_REG_CHK(unit, MBIST_ENCAP13,  3,  0, 0x1)) ||
                (_BIST_REG_CHK(unit, MBIST_ENCAP12,  0,  0, 0x0)))
            {
                _BIST_PRINTF("MBIST_ENCAP_MBS Fail\n");
            }

            if ((_BIST_REG_CHK(unit, MBISR_ENCAP01, 24,  8, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBISR_ENCAP02, 24,  8, 0x10000)))
            {
                _BIST_PRINTF("MBIST_ENCAP_MUNTAG Fail\n");
            }
        }

        //_BIST_REG_UPD(unit, MBIST_ENCAP_START, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_DRF_ENCAP MPF & MPHF 1~4======================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        //_BIST_REG_UPD(unit, MBIST_ENCAP_START, 31, 0, 0x3fC0000);
        //_bist_mdelay(100);

        //if ((_BIST_REG_CHK(unit, MBIST_ENCAP10, 15, 8, 0x0f)) &&
        //    (_BIST_REG_CHK(unit, MBIST_ENCAP10,  7, 0, 0x0f)))
        //{
        //    _BIST_PRINTF(" wait 500ms mbist_MPF1~4_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ENCAP10, 15, 8));
        //    _BIST_PRINTF(" wait 500ms mbist_MPHF1~4_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ENCAP10, 7, 0));
        //} else {
        //    _BIST_REG_UPD(unit, MBIST_ENCAP_RESUME, 31, 0, 0x3fC0000);
        //    _bist_mdelay(50);
        //    _BIST_REG_UPD(unit, MBIST_ENCAP_RESUME, 31, 0, 0x0);
        //}

        //_bist_mdelay(100);
        //if ((_BIST_REG_CHK(unit, MBIST_ENCAP10, 15,  8, 0x0f)) &&
        //    (_BIST_REG_CHK(unit, MBIST_ENCAP10,  7,  0, 0x0f)))
        //{
        //    _BIST_PRINTF(" wait 500ms mbist_MPF1~4_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ENCAP10, 15,  8));
        //    _BIST_PRINTF(" wait 500ms mbist_MPHF1~4_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ENCAP10,  7,  0));
        //} else {
        //    _BIST_REG_UPD(unit, MBIST_ENCAP_RESUME, 31, 0, 0x3fc0000);
        //    _bist_mdelay(50);
        //    _BIST_REG_UPD(unit, MBIST_ENCAP_RESUME, 31, 0, 0x0);
        //    _bist_mdelay(100);

        //    if ((_BIST_REG_CHK(unit, MBIST_ENCAP06, 31, 0, 0x0)) ||
        //        (_BIST_REG_CHK(unit, MBIST_ENCAP09, 15, 8, 0x55)))
        //    {
        //        _BIST_PRINTF("MBIST_DRF_ENCAP_MPF00~04 have Fail\n");
        //    }

        //    if ((_BIST_REG_CHK(unit, MBIST_ENCAP08, 31, 0, 0x0)) ||
        //        (_BIST_REG_CHK(unit, MBIST_ENCAP09,  7, 0, 0x55)))
        //    {
        //        _BIST_PRINTF("MBIST_DRF_ENCAP_MPHF00~04 have Fail\n");
        //    }
        //}

        //_BIST_REG_UPD(unit, MBIST_ENCAP_START, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_DRF_ENCAP MTCAM======================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        //_BIST_REG_UPD(unit, MBIST_ENCAP_START, 31, 0, 0x4000000);
        //_bist_mdelay(100);

        //if ((_BIST_REG_CHK(unit, MBIST_ENCAP_TCAM, 16, 16, 0x01)))
        //{
        //    _BIST_PRINTF(" wait 500ms MBIST_ENCAP_TCAM = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ENCAP_TCAM, 16, 16));
        //} else {
        //    _BIST_REG_UPD(unit, MBIST_ENCAP_RESUME, 31, 0, 0x4000000);
        //    _bist_mdelay(50);
        //    _BIST_REG_UPD(unit, MBIST_ENCAP_RESUME, 31, 0, 0x0);
        //}

        //_bist_mdelay(100);
        //if ((_BIST_REG_CHK(unit, MBIST_ENCAP_TCAM, 16, 16, 0x01)))
        //{
        //    _BIST_PRINTF(" wait 500ms MBIST_ENCAP_TCAM = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ENCAP_TCAM, 16, 16));
        //} else {
        //    _BIST_REG_UPD(unit, MBIST_ENCAP_RESUME, 31, 0, 0x4000000);
        //    _bist_mdelay(50);
        //    _BIST_REG_UPD(unit, MBIST_ENCAP_RESUME, 31, 0, 0x0);
        //    _bist_mdelay(100);

        //    if ((_BIST_REG_CHK(unit, MBIST_ENCAP_TCAM, 31, 0, 0x1000000)))
        //    {
        //        _BIST_PRINTF("MBIST_DRF_ENCAP_MTCAM Fail\n");
        //    }
        //}

        //_BIST_REG_UPD(unit, MBIST_ENCAP_START, 31, 0, 0x0);
    }

    _BIST_PRINTF("===========MBIST_DRF_ENCAP MKELL & MRMK & & MEVC & MMPLS & MTS ==========\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        //_BIST_REG_UPD(unit, MBIST_ENCAP_START, 31, 0, 0x7f);
        //_bist_mdelay(100);

        //if ((_BIST_REG_CHK(unit, MBIST_ENCAP13, 30, 30, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_ENCAP13, 27, 20, 0x44)) &&
        //    (_BIST_REG_CHK(unit, MBIST_ENCAP13, 19, 12, 0x44)) &&
        //    (_BIST_REG_CHK(unit, MBIST_ENCAP13, 10, 10, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_ENCAP13,  6,  6, 0x01)))
        //{
        //    _BIST_PRINTF(" wait 500ms MBIST_DRF_ENCAP_MKELL_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ENCAP13, 30, 30));
        //    _BIST_PRINTF(" wait 500ms MBIST_DRF_ENCAP_MTS_pause = 0x%08X\n",   _BIST_REG_RET(unit, MBIST_ENCAP13, 27, 20));
        //    _BIST_PRINTF(" wait 500ms MBIST_DRF_ENCAP_MRMK_pause = 0x%08X\n",  _BIST_REG_RET(unit, MBIST_ENCAP13, 19, 12));
        //    _BIST_PRINTF(" wait 500ms MBIST_DRF_ENCAP_MEVC_pause = 0x%08X\n",  _BIST_REG_RET(unit, MBIST_ENCAP13, 10, 10));
        //    _BIST_PRINTF(" wait 500ms MBIST_DRF_ENCAP_MPLS_pause = 0x%08X\n",  _BIST_REG_RET(unit, MBIST_ENCAP13,  6,  6));
        //} else {
        //    _BIST_REG_UPD(unit, MBIST_ENCAP_RESUME, 31, 0, 0x7f);
        //    _bist_mdelay(50);
        //    _BIST_REG_UPD(unit, MBIST_ENCAP_RESUME, 31, 0, 0x0);
        //}

        //_bist_mdelay(100);
        //if ((_BIST_REG_CHK(unit, MBIST_ENCAP13, 30, 30, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_ENCAP13, 27, 20, 0x44)) &&
        //    (_BIST_REG_CHK(unit, MBIST_ENCAP13, 19, 12, 0x44)) &&
        //    (_BIST_REG_CHK(unit, MBIST_ENCAP13, 10, 10, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_ENCAP13,  6,  6, 0x01)))
        //{
        //    _BIST_PRINTF(" wait 500ms MBIST_DRF_ENCAP_MKELL_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_ENCAP13, 30, 30));
        //    _BIST_PRINTF(" wait 500ms MBIST_DRF_ENCAP_MTS_pause = 0x%08X\n",   _BIST_REG_RET(unit, MBIST_ENCAP13, 27, 20));
        //    _BIST_PRINTF(" wait 500ms MBIST_DRF_ENCAP_MRMK_pause = 0x%08X\n",  _BIST_REG_RET(unit, MBIST_ENCAP13, 19, 12));
        //    _BIST_PRINTF(" wait 500ms MBIST_DRF_ENCAP_MEVC_pause = 0x%08X\n",  _BIST_REG_RET(unit, MBIST_ENCAP13, 10, 10));
        //    _BIST_PRINTF(" wait 500ms MBIST_DRF_ENCAP_MPLS_pause = 0x%08X\n",  _BIST_REG_RET(unit, MBIST_ENCAP13,  6,  6));
        //} else {
        //    _BIST_REG_UPD(unit, MBIST_ENCAP_RESUME, 31, 0, 0x7f);
        //    _bist_mdelay(50);
        //    _BIST_REG_UPD(unit, MBIST_ENCAP_RESUME, 31, 0, 0x0);
        //    _bist_mdelay(100);

        //    if ((_BIST_REG_CHK(unit, MBIST_ENCAP12, 28, 24, 0x0)) ||
        //        (_BIST_REG_CHK(unit, MBIST_ENCAP13, 30, 28, 0x1)))
        //    {
        //        _BIST_PRINTF("MBIST_DRF_ENCAP_MKELL Fail\n");
        //    }

        //    if ((_BIST_REG_CHK(unit, MBIST_ENCAP12, 21, 16, 0x0)) ||
        //        (_BIST_REG_CHK(unit, MBIST_ENCAP13, 27, 20, 0x11)))
        //    {
        //        _BIST_PRINTF("MBIST_DRF_ENCAP_MTS Fail\n");
        //    }

        //    if ((_BIST_REG_CHK(unit, MBIST_ENCAP12, 13, 12, 0x0)) ||
        //        (_BIST_REG_CHK(unit, MBIST_ENCAP13, 19, 12, 0x11)))
        //    {
        //        _BIST_PRINTF("MBIST_DRF_ENCAP_MRMK Fail\n");
        //    }

        //    if ((_BIST_REG_CHK(unit, MBIST_ENCAP12,  8,  8, 0x0)) ||
        //        (_BIST_REG_CHK(unit, MBIST_ENCAP13, 11,  8, 0x1)))
        //    {
        //        _BIST_PRINTF("MBIST_DRF_ENCAP_MEVC Fail\n");
        //    }


        //    if ((_BIST_REG_CHK(unit, MBIST_ENCAP12,  4,  4, 0x0)) ||
        //        (_BIST_REG_CHK(unit, MBIST_ENCAP13,  7,  4, 0x1)))
        //    {
        //        _BIST_PRINTF("MBIST_DRF_ENCAP_MMPLS Fail\n");
        //    }
        //}

        //_BIST_REG_UPD(unit, MBIST_ENCAP_START, 31, 0, 0x0);
    }

    _BIST_PRINTF("================MBIST_DRF_ENCAP MBS & MUNTAG==============\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        //_BIST_REG_UPD(unit, MBIST_ENCAP_START, 31, 0, 0x380);
        //_bist_mdelay(100);

        //if ((_BIST_REG_CHK(unit, MBIST_ENCAP13,  2,  2, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBISR_ENCAP01, 16, 16, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBISR_ENCAP02, 16, 16, 0x01)))
        //{
        //    _BIST_PRINTF(" wait 500ms MBIST_DRF_ENCAP_MBS0_pause = 0x%08X\n",   _BIST_REG_RET(unit, MBIST_ENCAP13,  2,  2));
        //    _BIST_PRINTF(" wait 500ms MBIST_DRF_ENCAP_MBS1_pause = 0x%08X\n",   _BIST_REG_RET(unit, MBISR_ENCAP01, 16, 16));
        //    _BIST_PRINTF(" wait 500ms MBIST_DRF_ENCAP_MUNTAG_pause = 0x%08X\n", _BIST_REG_RET(unit, MBISR_ENCAP02, 16, 16));
        //} else {
        //    _BIST_REG_UPD(unit, MBIST_ENCAP_RESUME, 31, 0, 0x380);
        //    _bist_mdelay(50);
        //    _BIST_REG_UPD(unit, MBIST_ENCAP_RESUME, 31, 0, 0x0);
        //}

        //_bist_mdelay(100);
        //if ((_BIST_REG_CHK(unit, MBIST_ENCAP13,  2,  2, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBISR_ENCAP01, 16, 16, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBISR_ENCAP02, 16, 16, 0x01)))
        //{
        //    _BIST_PRINTF(" wait 500ms MBIST_DRF_ENCAP_MBS0_pause = 0x%08X\n",   _BIST_REG_RET(unit, MBIST_ENCAP13,  2,  2));
        //    _BIST_PRINTF(" wait 500ms MBIST_DRF_ENCAP_MBS1_pause = 0x%08X\n",   _BIST_REG_RET(unit, MBISR_ENCAP01, 16, 16));
        //    _BIST_PRINTF(" wait 500ms MBIST_DRF_ENCAP_MUNTAG_pause = 0x%08X\n", _BIST_REG_RET(unit, MBISR_ENCAP02, 16, 16));
        //} else {
        //    _BIST_REG_UPD(unit, MBIST_ENCAP_RESUME, 31, 0, 0x380);
        //    _bist_mdelay(50);
        //    _BIST_REG_UPD(unit, MBIST_ENCAP_RESUME, 31, 0, 0x0);
        //    _bist_mdelay(100);

        //    if ((_BIST_REG_CHK(unit, MBIST_ENCAP13,  3,  0, 0x1)) ||
        //        (_BIST_REG_CHK(unit, MBIST_ENCAP12,  0,  0, 0x0)))
        //    {
        //        _BIST_PRINTF("MBIST_ENCAP_MBS Fail\n");
        //    }

        //    if ((_BIST_REG_CHK(unit, MBISR_ENCAP01, 24,  8, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBISR_ENCAP02, 24,  8, 0x10000)))
        //    {
        //        _BIST_PRINTF("MBIST_ENCAP_MUNTAG Fail\n");
        //    }
        //}

        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x0);
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x0);
        _BIST_REG_UPD(unit, MBIST_ENCAP_START, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_DRF_PAR 00~03==================\n");
    {
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        _BIST_REG_UPD(unit, MBIST_PAR_START, 31, 0, 0xf);
        _bist_mdelay(100);

        if ((_BIST_REG_CHK(unit, MBIST_PAR01_0, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PAR01_1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PAR02,   24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PAR03,   24, 24, 0x01)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_DRF_PAR01_0_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PAR01_0, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_PAR01_1_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PAR01_1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_PAR02_pause = 0x%08X\n",   _BIST_REG_RET(unit, MBIST_PAR02, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_PAR03_pause = 0x%08X\n",   _BIST_REG_RET(unit, MBIST_PAR03, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_PAR_RESUME, 31, 0, 0xf);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_PAR_RESUME, 31, 0, 0x0);
        }

        _bist_mdelay(100);
        if ((_BIST_REG_CHK(unit, MBIST_PAR01_0, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PAR01_1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PAR02,   24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_PAR03,   24, 24, 0x01)))
        {
            _BIST_PRINTF(" wait 500ms MBIST_DRF_PAR01_0_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PAR01_0, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_PAR01_1_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_PAR01_1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_PAR02_pause = 0x%08X\n",   _BIST_REG_RET(unit, MBIST_PAR02, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_PAR03_pause = 0x%08X\n",   _BIST_REG_RET(unit, MBIST_PAR03, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_PAR_RESUME, 31, 0, 0xf);
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_PAR_RESUME, 31, 0, 0x0);
            _bist_mdelay(100);

            if ((_BIST_REG_CHK(unit, MBIST_PAR01_0, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PAR01_0, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PAR02, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_PAR03, 28, 12, 0x10000)))
            {
                _BIST_PRINTF("MBIST_DRF_PAR00~03 have Fail\n");
            }
        }

        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x0);
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x0);
        _BIST_REG_UPD(unit, MBIST_PAR_START, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_DRF_IGR 01~05==================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        _BIST_REG_UPD(unit, MBIST_IGR_START, 31, 0, (0xfc0 | 0x3f));
        _bist_mdelay(100);

        if ((_BIST_REG_CHK(unit, MBIST_IGR01_0, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_IGR01_1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_IGR02,   24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_IGR03,   24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_IGR04,   24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_IGR05,   24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_IGR06_0, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_IGR06_1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_IGR06_2, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_IGR06_3, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_IGR06_4, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_IGR07,   24, 24, 0x01))
            )
        {
            _BIST_PRINTF(" wait 500ms MBIST_DRF_IGR01_0_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_IGR01_0, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_IGR01_1_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_IGR01_1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_IGR02_pause = 0x%08X\n",   _BIST_REG_RET(unit, MBIST_IGR02, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_IGR03_pause = 0x%08X\n",   _BIST_REG_RET(unit, MBIST_IGR03, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_IGR04_pause = 0x%08X\n",   _BIST_REG_RET(unit, MBIST_IGR04, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_IGR05_pause = 0x%08X\n",   _BIST_REG_RET(unit, MBIST_IGR05, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_IGR06_0_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_IGR06_0, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_IGR06_1_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_IGR06_1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_IGR06_2_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_IGR06_2, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_IGR06_3_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_IGR06_3, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_IGR06_4_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_IGR06_4, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_IGR07_pause = 0x%08X\n",   _BIST_REG_RET(unit, MBIST_IGR07, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_IGR_RESUME, 31, 0, (0xfc0 | 0x3f));
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_IGR_RESUME, 31, 0, 0x0);
        }

        _bist_mdelay(100);
        if ((_BIST_REG_CHK(unit, MBIST_IGR01_0, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_IGR01_1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_IGR02,   24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_IGR03,   24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_IGR04,   24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_IGR05,   24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_IGR06_0, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_IGR06_1, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_IGR06_2, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_IGR06_3, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_IGR06_4, 24, 24, 0x01)) &&
            (_BIST_REG_CHK(unit, MBIST_IGR07,   24, 24, 0x01))
            )
        {
            _BIST_PRINTF(" wait 500ms MBIST_DRF_IGR01_0_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_IGR01_0, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_IGR01_1_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_IGR01_1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_IGR02_pause = 0x%08X\n",   _BIST_REG_RET(unit, MBIST_IGR02, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_IGR03_pause = 0x%08X\n",   _BIST_REG_RET(unit, MBIST_IGR03, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_IGR04_pause = 0x%08X\n",   _BIST_REG_RET(unit, MBIST_IGR04, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_IGR05_pause = 0x%08X\n",   _BIST_REG_RET(unit, MBIST_IGR05, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_IGR06_0_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_IGR06_0, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_IGR06_1_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_IGR06_1, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_IGR06_2_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_IGR06_2, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_IGR06_3_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_IGR06_3, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_IGR06_4_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_IGR06_4, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_IGR07_pause = 0x%08X\n",   _BIST_REG_RET(unit, MBIST_IGR07, 24, 24));
        } else {
            _BIST_REG_UPD(unit, MBIST_IGR_RESUME, 31, 0, (0xfc0 | 0x3f));
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_IGR_RESUME, 31, 0, 0x0);
            _bist_mdelay(100);

            if ((_BIST_REG_CHK(unit, MBIST_IGR01_0, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_IGR01_1, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_IGR02, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_IGR03, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_IGR04, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_IGR05, 28, 12, 0x10000)))
            {
                _BIST_PRINTF("MBIST_DRF_IGR01~05 have Fail\n");
            }

            if ((_BIST_REG_CHK(unit, MBIST_IGR06_0, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_IGR06_1, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_IGR06_2, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_IGR06_3, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_IGR06_4, 28, 12, 0x10000)) ||
                (_BIST_REG_CHK(unit, MBIST_IGR07, 28, 12, 0x10000)))
            {
                _BIST_PRINTF("MBIST_DRF_IGR06~07 have Fail\n");
            }
        }

        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x0);
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x0);
        //_BIST_REG_UPD(unit, MBIST_IGR_START, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_DRF_IGR 01~07==================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        //_BIST_REG_UPD(unit, MBIST_IGR_START, 31, 0, 0xfc0);
        //_bist_mdelay(100);

        //if ((_BIST_REG_CHK(unit, MBIST_IGR06_0, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_IGR06_1, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_IGR06_2, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_IGR06_3, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_IGR06_4, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_IGR07,   24, 24, 0x01)))
        //{
        //    _BIST_PRINTF(" wait 500ms MBIST_DRF_IGR06_0_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_IGR06_0, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_DRF_IGR06_1_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_IGR06_1, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_DRF_IGR06_2_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_IGR06_2, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_DRF_IGR06_3_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_IGR06_3, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_DRF_IGR06_4_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_IGR06_4, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_DRF_IGR07_pause = 0x%08X\n",   _BIST_REG_RET(unit, MBIST_IGR07, 24, 24));
        //} else {
        //    _BIST_REG_UPD(unit, MBIST_IGR_RESUME, 31, 0, 0xfc0);
        //    _bist_mdelay(50);
        //    _BIST_REG_UPD(unit, MBIST_IGR_RESUME, 31, 0, 0x0);
        //}

        //_bist_mdelay(100);
        //if ((_BIST_REG_CHK(unit, MBIST_IGR06_0, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_IGR06_1, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_IGR06_2, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_IGR06_3, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_IGR06_4, 24, 24, 0x01)) &&
        //    (_BIST_REG_CHK(unit, MBIST_IGR07,   24, 24, 0x01)))
        //{
        //    _BIST_PRINTF(" wait 500ms MBIST_DRF_IGR06_0_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_IGR06_0, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_DRF_IGR06_1_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_IGR06_1, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_DRF_IGR06_2_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_IGR06_2, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_DRF_IGR06_3_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_IGR06_3, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_DRF_IGR06_4_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_IGR06_4, 24, 24));
        //    _BIST_PRINTF(" wait 500ms MBIST_DRF_IGR07_pause = 0x%08X\n",   _BIST_REG_RET(unit, MBIST_IGR07, 24, 24));
        //} else {
        //    _BIST_REG_UPD(unit, MBIST_IGR_RESUME, 31, 0, 0xfc0);
        //    _bist_mdelay(50);
        //    _BIST_REG_UPD(unit, MBIST_IGR_RESUME, 31, 0, 0x0);
        //    _bist_mdelay(100);

        //    if ((_BIST_REG_CHK(unit, MBIST_IGR06_0, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_IGR06_1, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_IGR06_2, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_IGR06_3, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_IGR06_4, 28, 12, 0x10000)) ||
        //        (_BIST_REG_CHK(unit, MBIST_IGR07, 28, 12, 0x10000)))
        //    {
        //        _BIST_PRINTF("MBIST_DRF_IGR06~07 have Fail\n");
        //    }
        //}

        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x0);
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x0);
        _BIST_REG_UPD(unit, MBIST_IGR_START, 31, 0, 0x0);
    }

    _BIST_PRINTF("==============MBIST_DRF_EGR =======================\n");

    _BIST_PRINTF("==================EGR00~07========================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        _BIST_REG_UPD(unit, MBIST_EGR_START_0, 31, 0, (0xfe000000 | 0x1fe0000 | 0x1e000 | 0x1fe0 | 0x1e));
        _BIST_REG_UPD(unit, MBIST_EGR_START_1, 31, 0, (0x7e000000 | 0x1fe0000 | 0x1fe00 | 0x1fe | 0x1));
        _bist_mdelay(100);

        if ((_BIST_REG_CHK(unit, MBIST_EGR18, 12,  5, 0xff)) &&
            (_BIST_REG_CHK(unit, MBIST_EGR18, 16, 13, 0x0f)) &&
            (_BIST_REG_CHK(unit, MBIST_EGR18, 24, 17, 0xff)) &&
            (_BIST_REG_CHK(unit, MBIST_EGR18, 31, 25, 0x7f)) &&
            (_BIST_REG_CHK(unit, MBIST_EGR19,  0,  0, 0x1)) &&
            (_BIST_REG_CHK(unit, MBIST_EGR19,  8,  1, 0xff)) &&
            (_BIST_REG_CHK(unit, MBIST_EGR19, 16,  9, 0xff)) &&
            (_BIST_REG_CHK(unit, MBIST_EGR19, 24, 17, 0xff)) &&
            (_BIST_REG_CHK(unit, MBIST_EGR19, 30, 25, 0x3f)) &&
            (_BIST_REG_CHK(unit, MBIST_EGR18,  4,  1, 0x0f))
            )
        {
            _BIST_PRINTF(" wait 500ms MBIST_DRF_EGR00~07_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_EGR18, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_EGR08~11_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_EGR18, 16, 13));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_EGR12~19_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_EGR18, 24, 17));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_EGR20~26_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_EGR18, 31, 25));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_EGR27_pause = 0x%08X\n",    _BIST_REG_RET(unit, MBIST_EGR19,  0,  0));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_EGR28~35_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_EGR19,  8,  1));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_EGR36~43_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_EGR19, 16,  9));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_EGR44~51_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_EGR19, 24, 17));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_EGR52~57_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_EGR19, 30, 25));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_MDRC_CC & MPORT & MPORT_EXT & MLINK_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_EGR18,  4,  1));
        } else {
            _BIST_REG_UPD(unit, MBIST_EGR_RESUME_0, 31, 0, (0xfe000000 | 0x1fe0000 | 0x1e000 | 0x1fe0 | 0x1e));
            _BIST_REG_UPD(unit, MBIST_EGR_RESUME_1, 31, 0, (0x7e000000 | 0x1fe0000 | 0x1fe00 | 0x1fe | 0x1));
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_EGR_RESUME_0, 31, 0, 0x0);
            _BIST_REG_UPD(unit, MBIST_EGR_RESUME_1, 31, 0, 0x0);
        }

        _bist_mdelay(100);
        if ((_BIST_REG_CHK(unit, MBIST_EGR18, 12,  5, 0xff)) &&
            (_BIST_REG_CHK(unit, MBIST_EGR18, 16, 13, 0x0f)) &&
            (_BIST_REG_CHK(unit, MBIST_EGR18, 24, 17, 0xff)) &&
            (_BIST_REG_CHK(unit, MBIST_EGR18, 31, 25, 0x7f)) &&
            (_BIST_REG_CHK(unit, MBIST_EGR19,  0,  0, 0x1)) &&
            (_BIST_REG_CHK(unit, MBIST_EGR19,  8,  1, 0xff)) &&
            (_BIST_REG_CHK(unit, MBIST_EGR19, 16,  9, 0xff)) &&
            (_BIST_REG_CHK(unit, MBIST_EGR19, 24, 17, 0xff)) &&
            (_BIST_REG_CHK(unit, MBIST_EGR19, 30, 25, 0x3f)) &&
            (_BIST_REG_CHK(unit, MBIST_EGR18,  4,  1, 0x0f))
            )
        {
            _BIST_PRINTF(" wait 500ms MBIST_DRF_EGR00~07_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_EGR18, 24, 24));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_EGR08~11_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_EGR18, 16, 13));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_EGR12~19_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_EGR18, 24, 17));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_EGR20~26_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_EGR18, 31, 25));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_EGR27_pause = 0x%08X\n",    _BIST_REG_RET(unit, MBIST_EGR19,  0,  0));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_EGR28~35_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_EGR19,  8,  1));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_EGR36~43_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_EGR19, 16,  9));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_EGR44~51_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_EGR19, 24, 17));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_EGR52~57_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_EGR19, 30, 25));
            _BIST_PRINTF(" wait 500ms MBIST_DRF_MDRC_CC & MPORT & MPORT_EXT & MLINK_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_EGR18,  4,  1));
        } else {
            _BIST_REG_UPD(unit, MBIST_EGR_RESUME_0, 31, 0, (0xfe000000 | 0x1fe0000 | 0x1e000 | 0x1fe0 | 0x1e));
            _BIST_REG_UPD(unit, MBIST_EGR_RESUME_1, 31, 0, (0x7e000000 | 0x1fe0000 | 0x1fe00 | 0x1fe | 0x1));
            _bist_mdelay(50);
            _BIST_REG_UPD(unit, MBIST_EGR_RESUME_0, 31, 0, 0x0);
            _BIST_REG_UPD(unit, MBIST_EGR_RESUME_1, 31, 0, 0x0);
            _bist_mdelay(100);

            if ((_BIST_REG_CHK(unit, MBIST_EGR06, 23,  8, 0x0)) ||
                (_BIST_REG_CHK(unit, MBIST_EGR14, 12,  5, 0xff)))
            {
                _BIST_PRINTF("MBIST_DRF_EGR01~07 have Fail\n");
            }

            if ((_BIST_REG_CHK(unit, MBIST_EGR06, 31, 24, 0x0)) ||
                (_BIST_REG_CHK(unit, MBIST_EGR14, 16, 13, 0xff)))
            {
                _BIST_PRINTF("MBIST_DRF_EGR08~11 have Fail\n");
            }

            if ((_BIST_REG_CHK(unit, MBIST_EGR07, 15, 0, 0x0)) ||
                (_BIST_REG_CHK(unit, MBIST_EGR14, 24, 17, 0xff)))
            {
                _BIST_PRINTF("MBIST_DRF_EGR12~19 have Fail\n");
            }

            if ((_BIST_REG_CHK(unit, MBIST_EGR07, 31, 16, 0x0)) ||
                (_BIST_REG_CHK(unit, MBIST_EGR14, 31, 25, 0x7f)) ||
                (_BIST_REG_CHK(unit, MBIST_EGR15,  0,  0, 0x1)))
            {
                _BIST_PRINTF("MBIST_DRF_EGR20~27 have Fail\n");
            }

            if ((_BIST_REG_CHK(unit, MBIST_EGR08, 15,  0, 0x0)) ||
                (_BIST_REG_CHK(unit, MBIST_EGR15,  8,  1, 0xff)))
            {
                _BIST_PRINTF("MBIST_DRF_EGR28~35 have Fail\n");
            }

            if ((_BIST_REG_CHK(unit, MBIST_EGR08, 31, 16, 0x0)) ||
                (_BIST_REG_CHK(unit, MBIST_EGR15, 16,  9, 0xff)))
            {
                _BIST_PRINTF("MBIST_DRF_EGR36~43 have Fail\n");
            }

            if ((_BIST_REG_CHK(unit, MBIST_EGR09, 15,  0, 0x0)) ||
                (_BIST_REG_CHK(unit, MBIST_EGR15, 24, 17, 0xff)))
            {
                _BIST_PRINTF("MBIST_DRF_EGR44~51 have Fail\n");
            }

            if ((_BIST_REG_CHK(unit, MBIST_EGR09, 31, 16, 0x0)) ||
                (_BIST_REG_CHK(unit, MBIST_EGR15, 30, 25, 0x3f)))
            {
                _BIST_PRINTF("MBIST_DRF_EGR52~57 have Fail\n");
            }

            if ((_BIST_REG_CHK(unit, MBIST_EGR06,  5,  1, 0x0)) ||
                (_BIST_REG_CHK(unit, MBIST_EGR14,  4,  1, 0xf)))
            {
                _BIST_PRINTF("MBIST_DRF_MDRC_CC & MPORT & MPORT_EXT & MLINK have Fail\n");
            }
        }

        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x0);
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x0);
        //_BIST_REG_UPD(unit, MBIST_EGR_START_0, 31, 0, 0x0);
    }

    _BIST_PRINTF("==================EGR08~11========================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        //_BIST_REG_UPD(unit, MBIST_EGR_START_0, 31, 0, 0x1e000);
        //_bist_mdelay(100);

        //if ((_BIST_REG_CHK(unit, MBIST_EGR18, 16, 13, 0x0f)))
        //{
        //    _BIST_PRINTF(" wait 500ms MBIST_DRF_EGR08~11_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_EGR18, 16, 13));
        //} else {
        //    _BIST_REG_UPD(unit, MBIST_EGR_RESUME_0, 31, 0, 0x1e000);
        //    _bist_mdelay(50);
        //    _BIST_REG_UPD(unit, MBIST_EGR_RESUME_0, 31, 0, 0x0);
        //}

        //_bist_mdelay(100);
        //if ((_BIST_REG_CHK(unit, MBIST_EGR18, 16, 13, 0x0f)))
        //{
        //    _BIST_PRINTF(" wait 500ms MBIST_DRF_EGR08~11_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_EGR18, 16, 13));
        //} else {
        //    _BIST_REG_UPD(unit, MBIST_EGR_RESUME_0, 31, 0, 0x1e000);
        //    _bist_mdelay(50);
        //    _BIST_REG_UPD(unit, MBIST_EGR_RESUME_0, 31, 0, 0x0);
        //    _bist_mdelay(100);

        //    if ((_BIST_REG_CHK(unit, MBIST_EGR06, 31, 24, 0x0)) ||
        //        (_BIST_REG_CHK(unit, MBIST_EGR14, 16, 13, 0xff)))
        //    {
        //        _BIST_PRINTF("MBIST_DRF_EGR08~11 have Fail\n");
        //    }
        //}

        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x0);
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x0);
        //_BIST_REG_UPD(unit, MBIST_EGR_START_0, 31, 0, 0x0);
    }

    _BIST_PRINTF("==================EGR12~19========================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        //_BIST_REG_UPD(unit, MBIST_EGR_START_0, 31, 0, 0x1fe0000);
        //_bist_mdelay(100);

        //if ((_BIST_REG_CHK(unit, MBIST_EGR18, 24, 17, 0xff)))
        //{
        //    _BIST_PRINTF(" wait 500ms MBIST_DRF_EGR12~19_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_EGR18, 24, 17));
        //} else {
        //    _BIST_REG_UPD(unit, MBIST_EGR_RESUME_0, 31, 0, 0x1fe0000);
        //    _bist_mdelay(50);
        //    _BIST_REG_UPD(unit, MBIST_EGR_RESUME_0, 31, 0, 0x0);
        //}

        //_bist_mdelay(100);
        //if ((_BIST_REG_CHK(unit, MBIST_EGR18, 24, 17, 0xff)))
        //{
        //    _BIST_PRINTF(" wait 500ms MBIST_DRF_EGR12~19_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_EGR18, 24, 17));
        //} else {
        //    _BIST_REG_UPD(unit, MBIST_EGR_RESUME_0, 31, 0, 0x1fe0000);
        //    _bist_mdelay(50);
        //    _BIST_REG_UPD(unit, MBIST_EGR_RESUME_0, 31, 0, 0x0);
        //    _bist_mdelay(100);

        //    if ((_BIST_REG_CHK(unit, MBIST_EGR07, 15, 0, 0x0)) ||
        //        (_BIST_REG_CHK(unit, MBIST_EGR14, 24, 17, 0xff)))
        //    {
        //        _BIST_PRINTF("MBIST_DRF_EGR12~19 have Fail\n");
        //    }
        //}

        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x0);
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x0);
        //_BIST_REG_UPD(unit, MBIST_EGR_START_0, 31, 0, 0x0);
    }

    _BIST_PRINTF("==================EGR20~27========================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        //_BIST_REG_UPD(unit, MBIST_EGR_START_0, 31, 0, 0xfe000000);
        //_BIST_REG_UPD(unit, MBIST_EGR_START_1, 31, 0, 0x1);
        //_bist_mdelay(100);

        //if ((_BIST_REG_CHK(unit, MBIST_EGR18, 31, 25, 0x7f)) &&
        //    (_BIST_REG_CHK(unit, MBIST_EGR19,  0,  0, 0x1)))
        //{
        //    _BIST_PRINTF(" wait 500ms MBIST_DRF_EGR20~26_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_EGR18, 31, 25));
        //    _BIST_PRINTF(" wait 500ms MBIST_DRF_EGR27_pause = 0x%08X\n",    _BIST_REG_RET(unit, MBIST_EGR19,  0,  0));
        //} else {
        //    _BIST_REG_UPD(unit, MBIST_EGR_RESUME_0, 31, 0, 0xfe000000);
        //    _BIST_REG_UPD(unit, MBIST_EGR_RESUME_1, 31, 0, 0x1);
        //    _bist_mdelay(50);
        //    _BIST_REG_UPD(unit, MBIST_EGR_RESUME_0, 31, 0, 0x0);
        //    _BIST_REG_UPD(unit, MBIST_EGR_RESUME_1, 31, 0, 0x0);
        //}

        //_bist_mdelay(100);
        //if ((_BIST_REG_CHK(unit, MBIST_EGR18, 31, 25, 0x7f)) &&
        //    (_BIST_REG_CHK(unit, MBIST_EGR19,  0,  0, 0x1)))
        //{
        //    _BIST_PRINTF(" wait 500ms MBIST_DRF_EGR20~26_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_EGR18, 31, 25));
        //    _BIST_PRINTF(" wait 500ms MBIST_DRF_EGR27_pause = 0x%08X\n",    _BIST_REG_RET(unit, MBIST_EGR19,  0,  0));
        //} else {
        //    _BIST_REG_UPD(unit, MBIST_EGR_RESUME_0, 31, 0, 0xfe000000);
        //    _BIST_REG_UPD(unit, MBIST_EGR_RESUME_1, 31, 0, 0x1);
        //    _bist_mdelay(50);
        //    _BIST_REG_UPD(unit, MBIST_EGR_RESUME_0, 31, 0, 0x0);
        //    _BIST_REG_UPD(unit, MBIST_EGR_RESUME_1, 31, 0, 0x0);
        //    _bist_mdelay(100);

        //    if ((_BIST_REG_CHK(unit, MBIST_EGR07, 31, 16, 0x0)) ||
        //        (_BIST_REG_CHK(unit, MBIST_EGR14, 31, 25, 0x7f)) ||
        //        (_BIST_REG_CHK(unit, MBIST_EGR15,  0,  0, 0x1)))
        //    {
        //        _BIST_PRINTF("MBIST_DRF_EGR20~27 have Fail\n");
        //    }
        //}

        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x0);
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x0);
        //_BIST_REG_UPD(unit, MBIST_EGR_START_0, 31, 0, 0x0);
        //_BIST_REG_UPD(unit, MBIST_EGR_START_1, 31, 0, 0x0);
    }

    _BIST_PRINTF("==================EGR28~35========================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        //_BIST_REG_UPD(unit, MBIST_EGR_START_1, 31, 0, 0x1fe);
        //_bist_mdelay(100);

        //if ((_BIST_REG_CHK(unit, MBIST_EGR19,  8,  1, 0xff)))
        //{
        //    _BIST_PRINTF(" wait 500ms MBIST_DRF_EGR28~35_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_EGR19,  8,  1));
        //} else {
        //    _BIST_REG_UPD(unit, MBIST_EGR_RESUME_1, 31, 0, 0x1fe);
        //    _bist_mdelay(50);
        //    _BIST_REG_UPD(unit, MBIST_EGR_RESUME_1, 31, 0, 0x0);
        //}

        //_bist_mdelay(100);
        //if ((_BIST_REG_CHK(unit, MBIST_EGR19,  8,  1, 0xff)))
        //{
        //    _BIST_PRINTF(" wait 500ms MBIST_DRF_EGR28~35_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_EGR19,  8,  1));
        //} else {
        //    _BIST_REG_UPD(unit, MBIST_EGR_RESUME_1, 31, 0, 0x1fe);
        //    _bist_mdelay(50);
        //    _BIST_REG_UPD(unit, MBIST_EGR_RESUME_1, 31, 0, 0x0);
        //    _bist_mdelay(100);

        //    if ((_BIST_REG_CHK(unit, MBIST_EGR08, 15,  0, 0x0)) ||
        //        (_BIST_REG_CHK(unit, MBIST_EGR15,  8,  1, 0xff)))
        //    {
        //        _BIST_PRINTF("MBIST_DRF_EGR28~35 have Fail\n");
        //    }
        //}

        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x0);
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x0);
        //_BIST_REG_UPD(unit, MBIST_EGR_START_1, 31, 0, 0x0);
    }

    _BIST_PRINTF("==================EGR36~43========================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        //_BIST_REG_UPD(unit, MBIST_EGR_START_1, 31, 0, 0x1fe00);
        //_bist_mdelay(100);

        //if ((_BIST_REG_CHK(unit, MBIST_EGR19, 16,  9, 0xff)))
        //{
        //    _BIST_PRINTF(" wait 500ms MBIST_DRF_EGR36~43_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_EGR19, 16,  9));
        //} else {
        //    _BIST_REG_UPD(unit, MBIST_EGR_RESUME_1, 31, 0, 0x1fe00);
        //    _bist_mdelay(50);
        //    _BIST_REG_UPD(unit, MBIST_EGR_RESUME_1, 31, 0, 0x0);
        //}

        //_bist_mdelay(100);
        //if ((_BIST_REG_CHK(unit, MBIST_EGR19, 16,  9, 0xff)))
        //{
        //    _BIST_PRINTF(" wait 500ms MBIST_DRF_EGR36~43_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_EGR19, 16,  9));
        //} else {
        //    _BIST_REG_UPD(unit, MBIST_EGR_RESUME_1, 31, 0, 0x1fe00);
        //    _bist_mdelay(50);
        //    _BIST_REG_UPD(unit, MBIST_EGR_RESUME_1, 31, 0, 0x0);
        //    _bist_mdelay(100);

        //    if ((_BIST_REG_CHK(unit, MBIST_EGR08, 31, 16, 0x0)) ||
        //        (_BIST_REG_CHK(unit, MBIST_EGR15, 16,  9, 0xff)))
        //    {
        //        _BIST_PRINTF("MBIST_DRF_EGR36~43 have Fail\n");
        //    }
        //}

        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x0);
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x0);
        //_BIST_REG_UPD(unit, MBIST_EGR_START_1, 31, 0, 0x0);
    }

    _BIST_PRINTF("==================EGR44~51========================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        //_BIST_REG_UPD(unit, MBIST_EGR_START_1, 31, 0, 0x1fe0000);
        //_bist_mdelay(100);

        //if ((_BIST_REG_CHK(unit, MBIST_EGR19, 24, 17, 0xff)))
        //{
        //    _BIST_PRINTF(" wait 500ms MBIST_DRF_EGR44~51_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_EGR19, 24, 17));
        //} else {
        //    _BIST_REG_UPD(unit, MBIST_EGR_RESUME_1, 31, 0, 0x1fe0000);
        //    _bist_mdelay(50);
        //    _BIST_REG_UPD(unit, MBIST_EGR_RESUME_1, 31, 0, 0x0);
        //}

        //_bist_mdelay(100);
        //if ((_BIST_REG_CHK(unit, MBIST_EGR19, 24, 17, 0xff)))
        //{
        //    _BIST_PRINTF(" wait 500ms MBIST_DRF_EGR44~51_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_EGR19, 24, 17));
        //} else {
        //    _BIST_REG_UPD(unit, MBIST_EGR_RESUME_1, 31, 0, 0x1fe0000);
        //    _bist_mdelay(50);
        //    _BIST_REG_UPD(unit, MBIST_EGR_RESUME_1, 31, 0, 0x0);
        //    _bist_mdelay(100);

        //    if ((_BIST_REG_CHK(unit, MBIST_EGR09, 15,  0, 0x0)) ||
        //        (_BIST_REG_CHK(unit, MBIST_EGR15, 24, 17, 0xff)))
        //    {
        //        _BIST_PRINTF("MBIST_DRF_EGR44~51 have Fail\n");
        //    }
        //}

        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x0);
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x0);
        //_BIST_REG_UPD(unit, MBIST_EGR_START_1, 31, 0, 0x0);
    }

    _BIST_PRINTF("==================EGR52~57========================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        //_BIST_REG_UPD(unit, MBIST_EGR_START_1, 31, 0, 0x7e000000);
        //_bist_mdelay(100);

        //if ((_BIST_REG_CHK(unit, MBIST_EGR19, 30, 25, 0x3f)))
        //{
        //    _BIST_PRINTF(" wait 500ms MBIST_DRF_EGR52~57_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_EGR19, 30, 25));
        //} else {
        //    _BIST_REG_UPD(unit, MBIST_EGR_RESUME_1, 31, 0, 0x7e000000);
        //    _bist_mdelay(50);
        //    _BIST_REG_UPD(unit, MBIST_EGR_RESUME_1, 31, 0, 0x0);
        //}

        //_bist_mdelay(100);
        //if ((_BIST_REG_CHK(unit, MBIST_EGR19, 30, 25, 0x3f)))
        //{
        //    _BIST_PRINTF(" wait 500ms MBIST_DRF_EGR52~57_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_EGR19, 30, 25));
        //} else {
        //    _BIST_REG_UPD(unit, MBIST_EGR_RESUME_1, 31, 0, 0x7e000000);
        //    _bist_mdelay(50);
        //    _BIST_REG_UPD(unit, MBIST_EGR_RESUME_1, 31, 0, 0x0);
        //    _bist_mdelay(100);

        //    if ((_BIST_REG_CHK(unit, MBIST_EGR09, 31, 16, 0x0)) ||
        //        (_BIST_REG_CHK(unit, MBIST_EGR15, 30, 25, 0x3f)))
        //    {
        //        _BIST_PRINTF("MBIST_DRF_EGR52~57 have Fail\n");
        //    }
        //}

        //_BIST_REG_UPD(unit, MBIST_EGR_START_1, 31, 0, 0x0);
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x0);
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x0);
    }

    _BIST_PRINTF("==================EGR_MDRC_CC & MPORT & MPORT_EXT & MLINK========================\n");
    {
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x2);
        //_BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x1);
        //_BIST_REG_UPD(unit, MBIST_EGR_START_0, 31, 0, 0x1e);
        //_bist_mdelay(100);

        //if ((_BIST_REG_CHK(unit, MBIST_EGR18,  4,  1, 0x0f)))
        //{
        //    _BIST_PRINTF(" wait 500ms MBIST_DRF_MDRC_CC & MPORT & MPORT_EXT & MLINK_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_EGR18,  4,  1));
        //} else {
        //    _BIST_REG_UPD(unit, MBIST_EGR_RESUME_0, 31, 0, 0x1e);
        //    _bist_mdelay(50);
        //    _BIST_REG_UPD(unit, MBIST_EGR_RESUME_0, 31, 0, 0x0);
        //}

        //_bist_mdelay(100);
        //if ((_BIST_REG_CHK(unit, MBIST_EGR18,  4,  1, 0x0f)))
        //{
        //    _BIST_PRINTF(" wait 500ms MBIST_DRF_MDRC_CC & MPORT & MPORT_EXT & MLINK_pause = 0x%08X\n", _BIST_REG_RET(unit, MBIST_EGR18,  4,  1));
        //} else {
        //    _BIST_REG_UPD(unit, MBIST_EGR_RESUME_0, 31, 0, 0x1e);
        //    _bist_mdelay(50);
        //    _BIST_REG_UPD(unit, MBIST_EGR_RESUME_0, 31, 0, 0x0);
        //    _bist_mdelay(100);

        //    if ((_BIST_REG_CHK(unit, MBIST_EGR06,  5,  1, 0x0)) ||
        //        (_BIST_REG_CHK(unit, MBIST_EGR14,  4,  1, 0xf)))
        //    {
        //        _BIST_PRINTF("MBIST_DRF_MDRC_CC & MPORT & MPORT_EXT & MLINK have Fail\n");
        //    }
        //}

        _BIST_REG_UPD(unit, MBIST_EGR_START_0, 31, 0, 0x0);
        _BIST_REG_UPD(unit, MBIST_EGR_START_1, 31, 0, 0x0);
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 10, 10, 0x0);
        _BIST_REG_UPD(unit, GLB_MBIST_CTRL, 1, 0, 0x0);
    }

    return CPARSER_OK;

#endif
}


static uint32 _reg32_read(uint32 unit, uint32 addr)
{
    uint32  value = 0;

    debug_mem_read(unit, addr, &value);

    return value;
}


#define _REG32_READ(_unit, _addr)               (_reg32_read((_unit), (_addr)))
#define _REG32_WRITE(_unit, _addr, _val)        (debug_mem_write((_unit), (_addr), (_val)))

#define _BIST_REG32_NEQ(_unit, _addr, _val)     (_REG32_READ((_unit), (_addr)) != (_val))
#define _BIST_REG32_PRINT(_unit, _addr)                 \
    do {                                                \
        _BIST_PRINTF("Failed - Reg 0x%08X = 0x%08X\n",  \
            (_addr), _REG32_READ((_unit), (_addr)));    \
    } while (0)



/*
 * diag test socbist
 */
cparser_result_t cparser_cmd_diag_test_socbist(cparser_context_t *context)
{
    uint32  unit = 0;
    uint32 master_view_unit;
    int32   result = CPARSER_OK;

    DIAG_UTIL_FUNC_INIT(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    _BIST_PRINTF("==============SOC_MBIST===========================\n");


    _BIST_PRINTF("1. L2 WS and TAG BIST\n");
    _REG32_WRITE(unit, 0xb8000430, 0xC0000000);
    _bist_mdelay(50);
    if (_BIST_REG32_NEQ(unit, 0xb8000430, 0xE0000000))
    {
        _BIST_REG32_PRINT(unit, 0xb8000430);
        result = CPARSER_NOT_OK;
    }
    _REG32_WRITE(unit, 0xb8000430, 0x0);


    _BIST_PRINTF("2. L2 TAG BIST\n");
    _REG32_WRITE(unit, 0xb8000434, 0xC0000000);
    _bist_mdelay(50);
    if (_BIST_REG32_NEQ(unit, 0xb8000434, 0xE0000000))
    {
        _BIST_REG32_PRINT(unit, 0xb8000434);
        result = CPARSER_NOT_OK;
    }
    _REG32_WRITE(unit, 0xb8000434, 0x0);


    _BIST_PRINTF("3. L2 DATA BIST\n");
    _REG32_WRITE(unit, 0xb8000438, 0xF0000000);
    _bist_mdelay(50);
    if (_BIST_REG32_NEQ(unit, 0xb8000438, 0xF8000000))
    {
        _BIST_REG32_PRINT(unit, 0xb8000438);
        result = CPARSER_NOT_OK;
    }
    _REG32_WRITE(unit, 0xb8000438, 0x0);


    _BIST_PRINTF("4. Core0 ICache BIST\n");
    _REG32_WRITE(unit, 0xb8000440, 0xF8000000);
    _bist_mdelay(50);
    if (_BIST_REG32_NEQ(unit, 0xb8000440, 0xFC000000))
    {
        _BIST_REG32_PRINT(unit, 0xb8000440);
        result = CPARSER_NOT_OK;
    }
    _REG32_WRITE(unit, 0xb8000440, 0x0);


    _BIST_PRINTF("5. Core0 DCache BIST0 - (BYPASSED!!)\n");
#if 0   /* cannot run this case with CPU/Memory */
    _REG32_WRITE(unit, 0xb8000444, 0xFC000000);
    _bist_mdelay(50);
    if (_BIST_REG32_NEQ(unit, 0xb8000444, 0xFE000000))
    {
        _BIST_REG32_PRINT(unit, 0xb8000444);
        result = CPARSER_NOT_OK;
    }
    _REG32_WRITE(unit, 0xb8000444, 0x0);
#endif

    _BIST_PRINTF("6. Core1 ICache BIST\n");
    _REG32_WRITE(unit, 0xb800044C, 0xF8000000);
    _bist_mdelay(50);
    if (_BIST_REG32_NEQ(unit, 0xb800044C, 0xFC000000))
    {
        _BIST_REG32_PRINT(unit, 0xb800044C);
        result = CPARSER_NOT_OK;
    }
    _REG32_WRITE(unit, 0xb800044C, 0x0);


    _BIST_PRINTF("7. Core1 DCache BIST0\n");
    _REG32_WRITE(unit, 0xb8000450, 0xFC000000);
    _bist_mdelay(50);
    if (_BIST_REG32_NEQ(unit, 0xb8000450, 0xFE000000))
    {
        _BIST_REG32_PRINT(unit, 0xb8000450);
        result = CPARSER_NOT_OK;
    }
    _REG32_WRITE(unit, 0xb8000450, 0x0);


    _BIST_PRINTF("8. Sheipa SRAM and ROM BIST\n");
    _REG32_WRITE(unit, 0xb8000464, 0xdda20b63);
    _REG32_WRITE(unit, 0xb8000468, 0x5f8f699a);
    _REG32_WRITE(unit, 0xb8000458, 0xC0060000);
    _bist_mdelay(50);
    if (_BIST_REG32_NEQ(unit, 0xb8000458, 0xE0070000))
    {
        _BIST_REG32_PRINT(unit, 0xb8000458);
        result = CPARSER_NOT_OK;
    }
    _REG32_WRITE(unit, 0xb8000458, 0x0);


    _BIST_PRINTF("9. Sheipa CMD, RG and RD SRAM BIST\n");
    _REG32_WRITE(unit, 0xb800045C, 0xC0300C00);
    _bist_mdelay(50);
    if (_BIST_REG32_NEQ(unit, 0xb800045C, 0xE0380E00))
    {
        _BIST_REG32_PRINT(unit, 0xb800045C);
        result = CPARSER_NOT_OK;
    }
    _REG32_WRITE(unit, 0xb800045C, 0x0);


    _BIST_PRINTF("10. Sheipa WD and MSK SRAM BIST\n");
    _REG32_WRITE(unit, 0xb8000460, 0xC0300000);
    _bist_mdelay(50);
    if (_BIST_REG32_NEQ(unit, 0xb8000460, 0xE0380000))
    {
        _BIST_REG32_PRINT(unit, 0xb8000460);
        result = CPARSER_NOT_OK;
    }
    _REG32_WRITE(unit, 0xb8000460, 0x0);


    _BIST_PRINTF("11. EMAC and GMAC BIST\n");
    _REG32_WRITE(unit, 0xb800047C, 0x26D486E7);
    _REG32_WRITE(unit, 0xb8000480, 0x5CF70382);
    _REG32_WRITE(unit, 0xb8000484, 0x68e5ef31);
    _REG32_WRITE(unit, 0xb800046C, 0xFFFFC000);
    _bist_mdelay(50);
    if (_BIST_REG32_NEQ(unit, 0xb8000474, 0xFF800000) || _BIST_REG32_NEQ(unit, 0xb8000478, 0x0))
    {
        _BIST_REG32_PRINT(unit, 0xb8000474);  //Data=0xFF800000, means All EMAC and GMAC BIST Done
        _BIST_REG32_PRINT(unit, 0xb8000478);  //Data=0, means All EMAC and GMAC BIST PASS
        result = CPARSER_NOT_OK;
    }
    _REG32_WRITE(unit, 0xb800046C, 0x0);


    _BIST_PRINTF("==============SOC_DRF_BIST===========================\n");

    _BIST_PRINTF("1. DRF_L2 WS and TAG BIST\n");
    _REG32_WRITE(unit, 0xb8000430, 0x48000000);
    _bist_mdelay(100);
    if (_BIST_REG32_NEQ(unit, 0xb8000430, 0x48800000))
    {
        _BIST_PRINTF(" - Delay 100ms NO pause signal\n");
    } else {
        _REG32_WRITE(unit, 0xb8000430, 0x4C000000);
    }
    _bist_mdelay(50);
    _REG32_WRITE(unit, 0xb8000430, 0x48000000);
    _bist_mdelay(100);
    if (_BIST_REG32_NEQ(unit, 0xb8000430, 0x48800000))
    {
        _BIST_PRINTF(" - Delay 100ms NO pause signal\n");
    } else {
        _REG32_WRITE(unit, 0xb8000430, 0x4C000000);
    }
    _bist_mdelay(50);
    _REG32_WRITE(unit, 0xb8000430, 0x48000000);
    _bist_mdelay(100);
    if (_BIST_REG32_NEQ(unit, 0xb8000430, 0x4A000000))
    {
        _BIST_PRINTF(" - Fail\n");
        result = CPARSER_NOT_OK;
    }
    _REG32_WRITE(unit, 0xb8000430, 0x0);


    _BIST_PRINTF("2. DRF_L2 TAG BIST\n");
    _REG32_WRITE(unit, 0xb8000434, 0x40100000);
    _bist_mdelay(100);
    if (_BIST_REG32_NEQ(unit, 0xb8000434, 0x40100200))
    {
        _BIST_PRINTF(" - Delay 100ms NO pause signal\n");
    } else {
        _REG32_WRITE(unit, 0xb8000434, 0x40180000);
    }
    _bist_mdelay(50);
    _REG32_WRITE(unit, 0xb8000434, 0x40100000);
    _bist_mdelay(100);
    if (_BIST_REG32_NEQ(unit, 0xb8000434, 0x40100200))
    {
        _BIST_PRINTF(" - Delay 100ms NO pause signal\n");
    } else {
        _REG32_WRITE(unit, 0xb8000434, 0x40180000);
    }
    _bist_mdelay(50);
    _REG32_WRITE(unit, 0xb8000434, 0x40100000);
    _bist_mdelay(100);
    if (_BIST_REG32_NEQ(unit, 0xb8000434, 0x40140000))
    {
        _BIST_PRINTF(" - Fail\n");
        result = CPARSER_NOT_OK;
    }
    _REG32_WRITE(unit, 0xb8000434, 0x0);


    _BIST_PRINTF("3. DRF_L2 DATA BIST\n");
    _REG32_WRITE(unit, 0xb8000438, 0xD0000000);
    _REG32_WRITE(unit, 0xb800043c, 0x80000000);
    _bist_mdelay(100);
    if (_BIST_REG32_NEQ(unit, 0xb800043c, 0x80001000))
    {
        _BIST_PRINTF(" - Delay 100ms NO pause signal\n");
    } else {
        _REG32_WRITE(unit, 0xb800043c, 0xC0000000);
    }
    _bist_mdelay(50);
    _REG32_WRITE(unit, 0xb800043c, 0x80000000);
    _bist_mdelay(100);
    if (_BIST_REG32_NEQ(unit, 0xb800043c, 0x80001000))
    {
        _BIST_PRINTF(" - Delay 100ms NO pause signal\n");
    } else {
        _REG32_WRITE(unit, 0xb800043c, 0xC0000000);
    }
    _bist_mdelay(50);
    _REG32_WRITE(unit, 0xb800043c, 0x80000000);
    _bist_mdelay(100);
    if (_BIST_REG32_NEQ(unit, 0xb800043c, 0xA0000000))
    {
        _BIST_PRINTF(" - Fail\n");
        result = CPARSER_NOT_OK;
    }
    _REG32_WRITE(unit, 0xb800043c, 0x0);


    _BIST_PRINTF("4. DRF_Core0 ICache BIST\n");
    _REG32_WRITE(unit, 0xb8000440, 0xE8010000);
    _bist_mdelay(100);
    if (_BIST_REG32_NEQ(unit, 0xb8000440, 0xE8010010))
    {
        _BIST_PRINTF(" - Delay 100ms NO pause signal\n");
    } else {
        _REG32_WRITE(unit, 0xb8000440, 0xE8018000);
    }
    _bist_mdelay(50);
    _REG32_WRITE(unit, 0xb8000440, 0xE8010000);
    _bist_mdelay(100);
    if (_BIST_REG32_NEQ(unit, 0xb8000440, 0xE8010010))
    {
        _BIST_PRINTF(" - Delay 100ms NO pause signal\n");
    } else {
        _REG32_WRITE(unit, 0xb8000440, 0xE8018000);
    }
    _bist_mdelay(50);
    _REG32_WRITE(unit, 0xb8000440, 0xE8010000);
    _bist_mdelay(100);

    if (_BIST_REG32_NEQ(unit, 0xb8000440, 0xE8014000))
    {
        _BIST_PRINTF(" - Fail\n");
        result = CPARSER_NOT_OK;
    }
    _REG32_WRITE(unit, 0xb8000440, 0x0);


    _BIST_PRINTF("5. DRF_Core0 DCache BIST1 - (BYPASSED!!)\n");
#if 0   /* cannot run this case with CPU/Memory */
    _REG32_WRITE(unit, 0xb8000444, 0xF4000000);
    _REG32_WRITE(unit, 0xb8000448, 0x40000000);
    _bist_mdelay(100);
    if (_BIST_REG32_NEQ(unit, 0xb8000448, 0x40000080))
    {
        _BIST_PRINTF(" - Delay 100ms NO pause signal\n");
    } else {
        _REG32_WRITE(unit, 0xb8000448, 0xc0000000);
    }
    _bist_mdelay(50);
    _REG32_WRITE(unit, 0xb8000448, 0x40000000);
    _bist_mdelay(100);
    if (_BIST_REG32_NEQ(unit, 0xb8000448, 0x40000080))
    {
        _BIST_PRINTF(" - Delay 100ms NO pause signal\n");
    } else {
        _REG32_WRITE(unit, 0xb8000448, 0xc0000000);
    }
    _bist_mdelay(50);
    _REG32_WRITE(unit, 0xb8000448, 0x40000000);
    _bist_mdelay(100);
    if (_BIST_REG32_NEQ(unit, 0xb8000448, 0x60000000))
    {
        _BIST_PRINTF(" - Fail\n");
        result = CPARSER_NOT_OK;
    }
    _REG32_WRITE(unit, 0xb8000448, 0x0);
#endif


    _BIST_PRINTF("6. DRF_Core1 ICache BIST\n");
    _REG32_WRITE(unit, 0xb800044c, 0xE8010000);
    _bist_mdelay(100);
    if (_BIST_REG32_NEQ(unit, 0xb800044c, 0xE8010010))
    {
        _BIST_PRINTF(" - Delay 100ms NO pause signal\n");
    } else {
        _REG32_WRITE(unit, 0xb800044c, 0xE8018000);
    }
    _bist_mdelay(50);
    _REG32_WRITE(unit, 0xb800044c, 0xE8010000);
    _bist_mdelay(100);
    if (_BIST_REG32_NEQ(unit, 0xb800044c, 0xE8010010))
    {
        _BIST_PRINTF(" - Delay 100ms NO pause signal\n");
    } else {
        _REG32_WRITE(unit, 0xb800044c, 0xE8018000);
    }
    _bist_mdelay(50);
    _REG32_WRITE(unit, 0xb800044c, 0xE8010000);
    _bist_mdelay(100);
    if (_BIST_REG32_NEQ(unit, 0xb800044c, 0xE8014000))
    {
        _BIST_PRINTF(" - Fail\n");
        result = CPARSER_NOT_OK;
    }
    _REG32_WRITE(unit, 0xb800044c, 0x0);


    _BIST_PRINTF("7. DRF_Core1 DCache BIST0\n");
    _REG32_WRITE(unit, 0xb8000450, 0xF4000000);
    _REG32_WRITE(unit, 0xb8000454, 0x40000000);
    _bist_mdelay(100);
    if (_BIST_REG32_NEQ(unit, 0xb8000454, 0x40000080))
    {
        _BIST_PRINTF(" - Delay 100ms NO pause signal\n");
    } else {
        _REG32_WRITE(unit, 0xb8000454, 0xc0000000);
    }
    _bist_mdelay(50);
    _REG32_WRITE(unit, 0xb8000454, 0x40000000);
    _bist_mdelay(100);
    if (_BIST_REG32_NEQ(unit, 0xb8000454, 0x40000080))
    {
        _BIST_PRINTF(" - Delay 100ms NO pause signal\n");
    } else {
        _REG32_WRITE(unit, 0xb8000454, 0xc0000000);
    }
    _bist_mdelay(50);
    _REG32_WRITE(unit, 0xb8000454, 0x40000000);
    _bist_mdelay(100);
    if (_BIST_REG32_NEQ(unit, 0xb8000454, 0x60000000))
    {
        _BIST_PRINTF(" - Fail\n");
        result = CPARSER_NOT_OK;
    }
    _REG32_WRITE(unit, 0xb8000454, 0x0);


    _BIST_PRINTF("8. DRF_Sheipa SRAM and ROM BIST\n");
    _REG32_WRITE(unit, 0xb8000458, 0x48020000);
    _REG32_WRITE(unit, 0xb800045C, 0x48120480);
    _bist_mdelay(100);
    if (_BIST_REG32_NEQ(unit, 0xb8000458, 0x48820000))
    {
        _BIST_PRINTF(" - Delay 100ms NO pause signal\n");
    } else {
        _REG32_WRITE(unit, 0xb8000458, 0x4C020000);
    }
    if (_BIST_REG32_NEQ(unit, 0xb800045c, 0x48922488))
    {
        _BIST_PRINTF(" - Delay 100ms NO pause signal\n");
    } else {
        _REG32_WRITE(unit, 0xb800045c, 0x4C1304C0);
    }
    _bist_mdelay(50);
    _REG32_WRITE(unit, 0xb8000458, 0x48020000);
    _REG32_WRITE(unit, 0xb800045C, 0x48120480);
    _bist_mdelay(100);
    if (_BIST_REG32_NEQ(unit, 0xb8000458, 0x48820000))
    {
        _BIST_PRINTF(" - Delay 100ms NO pause signal\n");
    } else {
        _REG32_WRITE(unit, 0xb8000458, 0x4C020000);
    }
    if (_BIST_REG32_NEQ(unit, 0xb800045c, 0x48922488))
    {
        _BIST_PRINTF(" - Delay 100ms NO pause signal\n");
    } else {
        _REG32_WRITE(unit, 0xb800045c, 0x4C1304C0);
    }
    _bist_mdelay(50);
    _REG32_WRITE(unit, 0xb8000458, 0x48020000);
    _REG32_WRITE(unit, 0xb800045C, 0x48120480);
    _bist_mdelay(100);

    if ((_REG32_READ(unit, 0xb8000458) != 0x4A020000) ||
        (_REG32_READ(unit, 0xb800045c) != 0x4A1284A0))
    {
        _BIST_PRINTF(" - Fail\n");
        result = CPARSER_NOT_OK;
    }
    _REG32_WRITE(unit, 0xb8000458, 0x0);
    _REG32_WRITE(unit, 0xb800045c, 0x0);


    _BIST_PRINTF("9. DRF_Sheipa CMD, RG and RD SRAM BIST\n");
    _REG32_WRITE(unit, 0xb800045c, 0x48120480);
    _bist_mdelay(100);
    if (_BIST_REG32_NEQ(unit, 0xb800045c, 0x48922488))
    {
        _BIST_PRINTF(" - Delay 100ms NO pause signal\n");
    } else {
        _REG32_WRITE(unit, 0xb800045c, 0x4C1304C0);
    }
    _bist_mdelay(50);
    _REG32_WRITE(unit, 0xb800045c, 0x48120480);
    _bist_mdelay(100);
    if (_BIST_REG32_NEQ(unit, 0xb800045c, 0x48922488))
    {
        _BIST_PRINTF(" - Delay 100ms NO pause signal\n");
    } else {
        _REG32_WRITE(unit, 0xb800045c, 0x4C1304C0);
    }
    _bist_mdelay(50);
    _REG32_WRITE(unit, 0xb800045c, 0x48120480);
    _bist_mdelay(100);
    if (_BIST_REG32_NEQ(unit, 0xb800045c, 0x4A1284A0))
    {
        _BIST_PRINTF(" - Fail\n");
        result = CPARSER_NOT_OK;
    }
    _REG32_WRITE(unit, 0xb800045c, 0x0);


    _BIST_PRINTF("10. DRF_Sheipa WD and MSK SRAM BIST\n");
    _REG32_WRITE(unit, 0xb8000460, 0x48120000);
    _bist_mdelay(100);
    if (_BIST_REG32_NEQ(unit, 0xb8000460, 0x48922000))
    {
        _BIST_PRINTF(" - Delay 100ms NO pause signal\n");
    } else {
        _REG32_WRITE(unit, 0xb8000460, 0x4C130000);
    }
    _bist_mdelay(50);
    _REG32_WRITE(unit, 0xb8000460, 0x48120000);
    _bist_mdelay(100);
    if (_BIST_REG32_NEQ(unit, 0xb8000460, 0x48922000))
    {
        _BIST_PRINTF(" - Delay 100ms NO pause signal\n");
    } else {
        _REG32_WRITE(unit, 0xb8000460, 0x4C130000);
    }
    _bist_mdelay(50);
    _REG32_WRITE(unit, 0xb8000460, 0x48120000);
    _bist_mdelay(100);
    if (_BIST_REG32_NEQ(unit, 0xb8000460, 0x4A128000))
    {
        _BIST_PRINTF(" - Fail\n");
        result = CPARSER_NOT_OK;
    }
    _REG32_WRITE(unit, 0xb8000460, 0x0);


    _BIST_PRINTF("11. DRF_EMAC and GMAC BIST\n");
    _REG32_WRITE(unit, 0xb800046C, 0x7FFF80);
    _bist_mdelay(100);
    if (_BIST_REG32_NEQ(unit, 0xb8000474, 0xFE00))
    {
        _BIST_PRINTF(" - Delay 100ms NO pause signal\n");
    } else {
        _REG32_WRITE(unit, 0xb800046C, 0x7FFFFF);
    }
    _bist_mdelay(50);
    _REG32_WRITE(unit, 0xb800046C, 0x7FFF80);
    _bist_mdelay(100);
    if (_BIST_REG32_NEQ(unit, 0xb8000474, 0xFE00))
    {
        _BIST_PRINTF(" - Delay 100ms NO pause signal\n");
    } else {
        _REG32_WRITE(unit, 0xb800046C, 0x7FFFFF);
    }
    _bist_mdelay(50);
    _REG32_WRITE(unit, 0xb800046C, 0x7FFF80);
    _bist_mdelay(100);
    if ((_REG32_READ(unit, 0xb8000474) != 0x7F0000) ||
        (_REG32_READ(unit, 0xb8000478) != 0x0))
    {
        _BIST_PRINTF(" - Fail\n");
        result = CPARSER_NOT_OK;
    }
    _REG32_WRITE(unit, 0xb800046C, 0x0);

    _BIST_PRINTF("\n");
    if (result == CPARSER_OK)
    {
        _BIST_PRINTF("All Passed!\n");
    } else {
        _BIST_PRINTF("FAILED!\n");
    }

    return result;
}


/** MIB **/
/*
 * mib set reset-value ( 0 | 1 )
 */
cparser_result_t cparser_cmd_mib_set_reset_value_0_1(cparser_context_t *context)
{
    uint32  unit = 0;
    uint32 master_view_unit;
    rtdrv_ext_mibCfg_t cntr_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    cntr_cfg.unit = unit;
    switch(TOKEN_CHAR(3,0))
    {
        case '0':
            cntr_cfg.rst_val = 0;
            break;

        case '1':
            cntr_cfg.rst_val = 1;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }
    SETSOCKOPT(RTDRV_EXT_MIB_RST_VAL_SET, &cntr_cfg, rtdrv_ext_mibCfg_t, 1);

    return CPARSER_OK;
}

/*
 *  sqa reset
 */
cparser_result_t cparser_cmd_sqa_reset(cparser_context_t *context)
{
    uint32  unit;
    uint32  master_view_unit;
    int32   ret = RT_ERR_FAILED;
    rtk_mac_t mac;
    uint32  i, j, group_idx_max, acl_max_block;
    rtk_port_t              port;
    rtk_qos_priSelWeight_t  weight;
    rtdrv_ext_switchCfg_t   switch_cfg;
    rtdrv_ext_l2Cfg_t       l2_cfg;
    rtk_qos_priSrcRemap_t   srcVal;
    rtk_acl_clear_t         clear_info;
    rtk_mac_t               rma = {{0x01,0x80,0xC2,0x00,0x00,0x01}};
    rtk_mac_t               rma_slow_proto = {{0x01,0x80,0xC2,0x00,0x00,0x02}};
    rtk_switch_devInfo_t    devInfo;
    rtdrv_ext_portCfg_t     port_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&devInfo, 0, sizeof(rtk_switch_devInfo_t));
    if ((ret = diag_om_get_deviceInfo(unit, &devInfo)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return ret;
    }

    /* 1. set Priority Remapping */
    for (i = 0; i <= 1; i++)
    {
        for (j = 0; j <= 7; j++)
        {
            srcVal.src.tag.dei = i;
            srcVal.src.tag.pri = j;
            DIAG_UTIL_ERR_CHK(rtk_qos_priRemap_set(unit, PRI_SRC_INNER_USER_PRI, srcVal, j), ret);
            DIAG_UTIL_ERR_CHK(rtk_qos_priRemap_set(unit, PRI_SRC_OUTER_USER_PRI, srcVal, j), ret);
        }
    }

    /* 2. set Port-based Priority, same as chip default */
    DIAG_UTIL_PORTMASK_SCAN(devInfo.all, port)
    {
        srcVal.src.port = port;
        DIAG_UTIL_ERR_CHK(rtk_qos_priRemap_set(unit, PRI_SRC_PB_PRI, srcVal, 0), ret);
    }

    /* 3. RMA */
    DIAG_UTIL_ERR_CHK(rtk_trap_rmaAction_set(unit, &rma, MGMT_ACTION_DROP), ret);
    DIAG_UTIL_ERR_CHK(rtk_trap_rmaAction_set(unit, &rma_slow_proto, MGMT_ACTION_DROP), ret);

    /* 4. Adjust priority to queue ID */
#if defined(CONFIG_SDK_FPGA_PLATFORM)
    DIAG_UTIL_ERR_CHK(rtk_qos_pri2QidMap_set(unit, 0, 0), ret);
    DIAG_UTIL_ERR_CHK(rtk_qos_pri2QidMap_set(unit, 1, 1), ret);
    DIAG_UTIL_ERR_CHK(rtk_qos_pri2QidMap_set(unit, 2, 2), ret);
    DIAG_UTIL_ERR_CHK(rtk_qos_pri2QidMap_set(unit, 3, 3), ret);
    DIAG_UTIL_ERR_CHK(rtk_qos_pri2QidMap_set(unit, 4, 0), ret);
    DIAG_UTIL_ERR_CHK(rtk_qos_pri2QidMap_set(unit, 5, 1), ret);
    DIAG_UTIL_ERR_CHK(rtk_qos_pri2QidMap_set(unit, 6, 2), ret);
    DIAG_UTIL_ERR_CHK(rtk_qos_pri2QidMap_set(unit, 7, 3), ret);
#else
    DIAG_UTIL_ERR_CHK(rtk_qos_pri2QidMap_set(unit, 0, 0), ret);
    DIAG_UTIL_ERR_CHK(rtk_qos_pri2QidMap_set(unit, 1, 1), ret);
    DIAG_UTIL_ERR_CHK(rtk_qos_pri2QidMap_set(unit, 2, 2), ret);
    DIAG_UTIL_ERR_CHK(rtk_qos_pri2QidMap_set(unit, 3, 3), ret);
    DIAG_UTIL_ERR_CHK(rtk_qos_pri2QidMap_set(unit, 4, 4), ret);
    DIAG_UTIL_ERR_CHK(rtk_qos_pri2QidMap_set(unit, 5, 5), ret);
    DIAG_UTIL_ERR_CHK(rtk_qos_pri2QidMap_set(unit, 6, 6), ret);
    DIAG_UTIL_ERR_CHK(rtk_qos_pri2QidMap_set(unit, 7, 7), ret);
#endif

    /* 5. adjust priority selection weight */
    osal_memset((void *)&weight, 0x0, sizeof(rtk_qos_priSelWeight_t));
    weight.weight_of_innerTag = 7;
    weight.weight_of_portBased = 1;
    DIAG_OM_GET_CHIP_CAPACITY(unit, group_idx_max, pri_sel_group_index_max);
    for (i = 0; i <= group_idx_max; i++)
    {
        DIAG_UTIL_ERR_CHK(rtk_qos_priSelGroup_set(unit, i, &weight), ret);
    }

    /* 6. Reset ACL rule entry*/
    DIAG_OM_GET_CHIP_CAPACITY(unit, acl_max_block, max_num_of_pie_block);
    for (i = 0; i < acl_max_block; ++i)
    {
         DIAG_UTIL_ERR_CHK(rtk_pie_phase_set(unit, i, PIE_PHASE_VACL), ret);
    }

    clear_info.start_idx = 0;
    clear_info.end_idx   = (acl_max_block * 128) - 1;
    DIAG_UTIL_ERR_CHK(rtk_acl_rule_del(unit, ACL_PHASE_VACL, &clear_info), ret);

    /* 7. EEE disabled */
    DIAG_UTIL_PORTMASK_SCAN(devInfo.all, port)
    {
        ret = rtk_eee_portEnable_set(unit,  port, DISABLED);
        if(ret != RT_ERR_FAILED && ret != RT_ERR_OK)
            return ret;
    }

    /* 8. EEE+ disabled: RTL9310 doesn't support */

    /* 9. Set pppoe-IP-parse = disable */
    DIAG_UTIL_ERR_CHK(rtk_switch_pppoeIpParseEnable_set(unit, ENABLED), ret);

    /* 11. enable L2 CAM */
    l2_cfg.unit = unit;
    l2_cfg.enable = ENABLED;
    SETSOCKOPT(RTDRV_EXT_L2_CMA_ENABLE_SET, &l2_cfg, rtdrv_ext_l2Cfg_t, 1);

    /* 12. Backpresssure Method : JAM mode, same as chip default */
    switch_cfg.unit = unit;
    switch_cfg.bkpres = 0;
    SETSOCKOPT(RTDRV_EXT_SWITCH_BKPRES_SET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);

    /* 13. Disable 48pass1, same as chip default */
    switch_cfg.unit = unit;
    switch_cfg.half_48pass1 = DISABLED;
    SETSOCKOPT(RTDRV_EXT_SWITCH_48PASS1_SET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);

    /* 14. set Port State Enabled */
    DIAG_UTIL_PORTMASK_SCAN(devInfo.all, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_port_adminEnable_set(unit, port, ENABLED), ret);
    }

    /* 15. Command: switch set limit-pause state disable */
    switch_cfg.unit = unit;
    switch_cfg.limit_pause = DISABLED;
    SETSOCKOPT(RTDRV_EXT_SWITCH_LIMITPAUSE_SET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);

    /* 16. reset switch MAC address */
    mac.octet[0] = 0x0;
    mac.octet[1] = 0x11;
    mac.octet[2] = 0x83;
    mac.octet[3] = 0x89;
    mac.octet[4] = 0x23;
    mac.octet[5] = 0x79;
    DIAG_UTIL_ERR_CHK(rtk_switch_mgmtMacAddr_set(unit, &mac), ret);

    /* 17. Set Special Congest */
    DIAG_UTIL_PORTMASK_SCAN(devInfo.ether, port)
    {
        port_cfg.unit = unit;
        port_cfg.port = port;
        port_cfg.full_sec = 0;
        port_cfg.half_sec = 0;
        SETSOCKOPT(RTDRV_EXT_PORT_SPECIAL_CONGEST_PORT_SUSTAIN_TIMER_SET, &port_cfg, rtdrv_ext_portCfg_t, 1);
        SETSOCKOPT(RTDRV_EXT_PORT_SPECIAL_CONGEST_PORT_SUSTAIN_TIMER_HALF_SET, &port_cfg, rtdrv_ext_portCfg_t, 1);

    }

    /* 18. Disable MAC_GLB_CTRL.MAC_48PASS1_DROP_EN */
    switch_cfg.unit = unit;
    switch_cfg.enable = DISABLED;
    SETSOCKOPT(RTDRV_EXT_SWITCH_MAC_48PASS1_DROP_SET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);

    osal_printf("Finish!\n");


#if 0 /* Following are added by SQA requirement */
    /* vlan set egress port all inner keep-tag state enable
       vlan set ingress port all inner keep-tag state enable */
    for (i=0; i<= 51; i++)
    {
        rtk_vlan_portIgrTagKeepEnable_get(unit, i, &outer_state, &inner_state);
        rtk_vlan_portIgrTagKeepEnable_set(unit, i, outer_state, ENABLED);

        rtk_vlan_portEgrTagKeepEnable_get(unit, i, &outer_state, &inner_state);
        rtk_vlan_portEgrTagKeepEnable_set(unit, i, outer_state, ENABLED);
    }

    /* Disable 48pass1 */
    ioal_mem32_read(unit, RTL8390_MAC_GLB_CTRL_ADDR, &val);
    val &= ~RTL8390_MAC_GLB_CTRL_HALF_48PASS1_EN_MASK;
    ioal_mem32_write(unit, RTL8390_MAC_GLB_CTRL_ADDR, val);


    /* Command: switch set interrupt link-change state enable */
//    SETSOCKOPT(RTDRV_EXT_SWITCH_INTR_LINK_CHANGE_ENABLE_SET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);


    /* Set inner-tag remarking source = internal-priority */
    rtk_qos_1pRemarkSrcSel_set(unit, PRI_SRC_INT_PRI);

    /* Set DSCP remarking source = internal-priority */
    rtk_qos_dscpRemarkSrcSel_set(unit, PRI_SRC_INT_PRI);

    /* Set default outer-tag priority = copy-internal-priority */
    for (i=0; i<= 52; i++)
    {
        rtk_qos_portOuter1pDfltPriSrcSel_set(unit, i, OUTER_1P_DFLT_SRC_INT_PRI);
    }


    /* enable all ports */
    for (i=0; i<= 51; i++)
        rtk_port_adminEnable_set(0, i, ENABLED);

#if defined(CONFIG_SDK_FPGA_PLATFORM)
    DIAG_UTIL_ERR_CHK(rtk_qos_queueNum_set(unit, 4), ret);

    /* reset FC threshold */
    ioal_mem32_write(unit, RTL8390_FC_DROP_THR_ADDR, 0x3ff);
    ioal_mem32_write(unit, RTL8390_FC_GLB_HI_THR_ADDR, 0x02320226);
    ioal_mem32_write(unit, RTL8390_FC_GLB_LO_THR_ADDR, 0x0168015e);
    ioal_mem32_write(unit, RTL8390_FC_GLB_FCOFF_HI_THR_ADDR, 0x02320226);
    ioal_mem32_write(unit, RTL8390_FC_GLB_FCOFF_LO_THR_ADDR, 0x0168015e);
    addr = RTL8390_FC_P_HI_THR_ADDR(0);
    ioal_mem32_write(unit, addr, 0x0064005e);
    addr = RTL8390_FC_P_LO_THR_ADDR(0);
    ioal_mem32_write(unit, addr, 0x00140014);
    addr = RTL8390_FC_P_FCOFF_HI_THR_ADDR(0);
    ioal_mem32_write(unit, addr, 0x0064005e);

    /* configure mode to trigger changing (T,B) and burst size */
    rtk_rate_stormControlRateMode_set(unit, BASED_ON_PKT);

    /* configure low threshold of input bandwidth leaky bucket */
    ioal_mem32_write(unit, RTL8390_IGR_BWCTRL_CTRL_ADDR, 0x17ff0);
#else
    DIAG_UTIL_ERR_CHK(rtk_qos_queueNum_set(unit, 8), ret);

    ioal_mem32_write(unit, RTL8390_FC_DROP_THR_ADDR, 0xfff);
    ioal_mem32_write(unit, RTL8390_FC_GLB_HI_THR_ADDR, 0x07780746);
    ioal_mem32_write(unit, RTL8390_FC_GLB_LO_THR_ADDR, 0x03520320);
    ioal_mem32_write(unit, RTL8390_FC_GLB_FCOFF_HI_THR_ADDR, 0x07780746);
    ioal_mem32_write(unit, RTL8390_FC_GLB_FCOFF_LO_THR_ADDR, 0x03520320);
    addr = RTL8390_FC_P_HI_THR_ADDR(0);
    ioal_mem32_write(unit, addr, 0x012c00fa);
    addr = RTL8390_FC_P_LO_THR_ADDR(0);
    ioal_mem32_write(unit, addr, 0x00140014);
    addr = RTL8390_FC_P_FCOFF_HI_THR_ADDR(0);
    ioal_mem32_write(unit, addr, 0x012c00fa);
#endif


#endif
    return CPARSER_OK;
}

/*
 *  sqa reset table vlan
 *
 *  Note:
 *      clear VLAN-related tables (IVC: IP-based/MAC-based entries)
 *      (but excluding VLAN table, due to it's a dedicate command already.)
 */
cparser_result_t cparser_cmd_sqa_reset_table_vlan(cparser_context_t *context)
{
    int32   ret = RT_ERR_FAILED;
    uint32  unit;
    uint32  master_view_unit;
    uint32  blk_idx, idx, entry_idx;
#ifdef CONFIG_SDK_FPGA_PLATFORM
    uint32  blk_num = 3;
    uint32  entry_num = 2;
#else
    uint32  blk_num = 16;
    uint32  entry_num = 128;
#endif
    rtk_vlan_igrVlanCnvtBlk_mode_t mode;
    rtk_vlan_macVlanEntry_t macEntry;
    rtk_vlan_ipSubnetVlanEntry_t ipEntry;
    rtk_vlan_igrVlanCnvtEntry_t ivcEntry;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&macEntry, 0x00, sizeof(rtk_vlan_macVlanEntry_t));
    osal_memset(&ipEntry, 0x00, sizeof(rtk_vlan_ipSubnetVlanEntry_t));
    osal_memset(&ivcEntry, 0x00, sizeof(rtk_vlan_igrVlanCnvtEntry_t));

    /*DIAG_UTIL_ERR_CHK(rtk_vlan_destroyAll(unit, TRUE), ret);*/
    for (blk_idx=0; blk_idx<blk_num; blk_idx++)
    {
        DIAG_UTIL_ERR_CHK(rtk_vlan_igrVlanCnvtBlkMode_get(unit, blk_idx, &mode), ret);

        for (idx=0; idx<entry_num; idx++)
        {
            entry_idx = (entry_num * blk_idx) + idx;

            if (CONVERSION_MODE_MAC_BASED == mode)
            {
                DIAG_UTIL_ERR_CHK(rtk_vlan_macBasedVlanEntry_set(unit, entry_idx, &macEntry), ret);
            }
            else if (CONVERSION_MODE_IP_SUBNET_BASED == mode)
            {
                DIAG_UTIL_ERR_CHK(rtk_vlan_ipSubnetBasedVlanEntry_set(unit, entry_idx, &ipEntry), ret);
            }
            else if (CONVERSION_MODE_C2SC == mode)
            {
                DIAG_UTIL_ERR_CHK(rtk_vlan_igrVlanCnvtEntry_set(unit, entry_idx, &ivcEntry), ret);
            }
        }
    }

    return CPARSER_OK;
}


/*
 *  sqa testcase l3hit ( 0 | 1 | 2 )
 */
cparser_result_t cparser_cmd_sqa_testcase_l3hit_0_1_2(cparser_context_t *context)
{
    int32   ret = RT_ERR_FAILED;
    uint32  unit;
    uint32  master_view_unit;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    switch(TOKEN_CHAR(3,0))
    {
    case '0':   /* RESET */
        {
            rtk_l3_host_t           l3Host;
            rtk_l3_route_t          l3Route;
            int32                   idx;

            idx = -1;   /* from beginning */
            do
            {
                /* L3 host entry */
                RT_ERR_CHK(rtk_l3_host_t_init(&l3Host), ret);
                RT_ERR_CHK(rtk_l3_host_getNext(unit, RTK_L3_FLAG_NONE, &idx, &l3Host), ret);
                osal_printf("Host idx = %d\n", idx);

                if (idx >= 0)
                {
                    l3Host.flags |= RTK_L3_FLAG_HIT_CLEAR;
                    RT_ERR_CHK(rtk_l3_host_find(unit, &l3Host), ret);
                }
            } while (idx >= 0);

            idx = -1;   /* from beginning */
            do
            {
                /* L3 route entry */
                RT_ERR_CHK(rtk_l3_route_t_init(&l3Route), ret);
                RT_ERR_CHK(rtk_l3_route_getNext(unit, RTK_L3_FLAG_NONE, &idx, &l3Route), ret);
                osal_printf("Route idx = %d\n", idx);

                if (idx >= 0)
                {
                    l3Route.flags |= RTK_L3_FLAG_HIT_CLEAR;
                    RT_ERR_CHK(rtk_l3_route_get(unit, &l3Route), ret);
                }
            } while (idx >= 0);
        }
        break;

    case '1':   /* Check odd */
        {
            rtk_l3_host_t           l3Host;
            rtk_l3_route_t          l3Route;
            int32                   idx;

            /* Odd (SIP) -> Even (DIP) */

            idx = -1;   /* from beginning */
            do
            {
                /* L3 host entry */
                RT_ERR_CHK(rtk_l3_host_t_init(&l3Host), ret);
                RT_ERR_CHK(rtk_l3_host_getNext(unit, RTK_L3_FLAG_NONE, &idx, &l3Host), ret);
                //osal_printf("Host idx = %d\n", idx);

                if (idx >= 0)
                {
                    if ((l3Host.ip_addr.ipv4) & 0x1)
                    {
                        /* Hit bit should be zero */
                        if (l3Host.flags & RTK_L3_FLAG_HIT)
                        {
                            osal_printf("ERROR: idx = %d, IPv4 = 0x%08X (hit)\n", idx, l3Host.ip_addr.ipv4);
                        }
                    } else {
                        /* Hit bit should be set */
                        if (!(l3Host.flags & RTK_L3_FLAG_HIT))
                        {
                            osal_printf("ERROR: idx = %d, IPv4 = 0x%08X (clear)\n", idx, l3Host.ip_addr.ipv4);
                        }
                    }
                }
            } while (idx < 8192);   /* only check the first 8K entry (2^n) */

            idx = -1;   /* from beginning */
            do
            {
                /* L3 route entry */
                RT_ERR_CHK(rtk_l3_route_t_init(&l3Route), ret);
                RT_ERR_CHK(rtk_l3_route_getNext(unit, RTK_L3_FLAG_NONE, &idx, &l3Route), ret);
                //osal_printf("Route idx = %d\n", idx);

                if (idx >= 0)
                {
                    if ((l3Route.ip_addr.ipv4) & 0x1)
                    {
                        /* Hit bit should be zero */
                        if (l3Route.flags & RTK_L3_FLAG_HIT)
                        {
                            osal_printf("ERROR: idx = %d, IPv4 = 0x%08X (hit)\n", idx, l3Route.ip_addr.ipv4);
                        }
                    } else {
                        /* Hit bit should be set */
                        if (!(l3Route.flags & RTK_L3_FLAG_HIT))
                        {
                            osal_printf("ERROR: idx = %d, IPv4 = 0x%08X (clear)\n", idx, l3Route.ip_addr.ipv4);
                        }
                    }
                }
            } while (idx < 8192);   /* only check the first 8K entry (2^n) */
        }
        break;

    case '2':   /* Check even */
        {
            rtk_l3_host_t           l3Host;
            rtk_l3_route_t          l3Route;
            int32                   idx;

            /* Odd (SIP) -> Even (DIP) */

            idx = -1;   /* from beginning */
            do
            {
                /* L3 host entry */
                RT_ERR_CHK(rtk_l3_host_t_init(&l3Host), ret);
                RT_ERR_CHK(rtk_l3_host_getNext(unit, RTK_L3_FLAG_NONE, &idx, &l3Host), ret);
                //osal_printf("Host idx = %d\n", idx);

                if (idx >= 0)
                {
                    if (!((l3Host.ip_addr.ipv4) & 0x1))
                    {
                        /* Hit bit should be zero */
                        if (l3Host.flags & RTK_L3_FLAG_HIT)
                        {
                            osal_printf("ERROR: idx = %d, IPv4 = 0x%08X (hit)\n", idx, l3Host.ip_addr.ipv4);
                        }
                    } else {
                        /* Hit bit should be set */
                        if (!(l3Host.flags & RTK_L3_FLAG_HIT))
                        {
                            osal_printf("ERROR: idx = %d, IPv4 = 0x%08X (clear)\n", idx, l3Host.ip_addr.ipv4);
                        }
                    }
                }
            } while (idx < 8192);   /* only check the first 8K entry (2^n) */

            idx = -1;   /* from beginning */
            do
            {
                /* L3 route entry */
                RT_ERR_CHK(rtk_l3_route_t_init(&l3Route), ret);
                RT_ERR_CHK(rtk_l3_route_getNext(unit, RTK_L3_FLAG_NONE, &idx, &l3Route), ret);
                //osal_printf("Route idx = %d\n", idx);

                if (idx >= 0)
                {
                    if (!((l3Route.ip_addr.ipv4) & 0x1))
                    {
                        /* Hit bit should be zero */
                        if (l3Route.flags & RTK_L3_FLAG_HIT)
                        {
                            osal_printf("ERROR: idx = %d, IPv4 = 0x%08X (hit)\n", idx, l3Route.ip_addr.ipv4);
                        }
                    } else {
                        /* Hit bit should be set */
                        if (!(l3Route.flags & RTK_L3_FLAG_HIT))
                        {
                            osal_printf("ERROR: idx = %d, IPv4 = 0x%08X (clear)\n", idx, l3Route.ip_addr.ipv4);
                        }
                    }
                }
            } while (idx < 8192);   /* only check the first 8K entry (2^n) */
        }
        break;
    }

    return CPARSER_OK;
}


/** SWITCH **/

cparser_result_t cparser_cmd_switch_reset_software_queue_reset(cparser_context_t *context)
{
    uint32 unit;
    uint32 master_view_unit;
    rtdrv_ext_switchCfg_t switch_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    switch_cfg.unit = unit;
    switch_cfg.value = 1;
    SETSOCKOPT(RTDRV_EXT_SWITCH_SW_QUERE_RESET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);

    return CPARSER_OK;
}

cparser_result_t cparser_cmd_switch_set_48_pass_1_state_disable_enable(cparser_context_t *context)
{
    uint32 unit;
    uint32 master_view_unit;
    rtk_enable_t enable = DISABLED;
    rtdrv_ext_switchCfg_t switch_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    switch(TOKEN_CHAR(4,0))
    {
        case 'd':
            enable = DISABLED;
            break;

        case 'e':
            enable = ENABLED;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    switch_cfg.unit = unit;
    switch_cfg.half_48pass1 = enable;
    SETSOCKOPT(RTDRV_EXT_SWITCH_48PASS1_SET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);

    return CPARSER_OK;
}

/*
 * switch set limit-pause state ( disable | 128-pause | 32-pause )
 */
cparser_result_t cparser_cmd_switch_set_limit_pause_state_disable_128_pause_32_pause(cparser_context_t *context)
{
    uint32 unit;
    uint32 master_view_unit;
    uint32 value;
    rtdrv_ext_switchCfg_t switch_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    switch(TOKEN_CHAR(4,0))
    {
        case 'd':
            value = 0;
            break;
        case '1':
            value = 0x1;
            break;
        case '3':
            value = 0x2;
            break;
        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    switch_cfg.unit = unit;
    switch_cfg.value = value;
    SETSOCKOPT(RTDRV_EXT_SWITCH_LIMITPAUSE_SET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);

    return CPARSER_OK;
}

/*
 *  switch set ipg-compensation ( ge | 10g ) state ( disable | enable )
 */
cparser_result_t cparser_cmd_switch_set_ipg_compensation_ge_10g_state_disable_enable(cparser_context_t *context)
{
    uint32 unit;
    uint32 master_view_unit;
    rtk_enable_t enable = DISABLED;
    rtdrv_ext_switchCfg_t switch_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    switch(TOKEN_CHAR(5,0))
    {
        case 'd':
            enable = DISABLED;
            break;

        case 'e':
            enable = ENABLED;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    switch(TOKEN_CHAR(3,0))
    {
        case 'g': /* ge */
            switch_cfg.unit = unit;
            switch_cfg.ipg_cmpstn = enable;
            SETSOCKOPT(RTDRV_EXT_SWITCH_IPGCOMSTN_SET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);
            break;

        case '1': /* 10g */
            switch_cfg.unit = unit;
            switch_cfg.ipg_cmpstn = enable;
            SETSOCKOPT(RTDRV_EXT_SWITCH_IPGCOMSTN_10G_SET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}

/*
 *  switch set ipg-compensation ( 2_5g | 5g ) state ( disable | enable )
 */
cparser_result_t cparser_cmd_switch_set_ipg_compensation_2_5g_5g_state_disable_enable(cparser_context_t *context)
{
    uint32 unit;
    uint32 master_view_unit;
    rtk_enable_t enable = DISABLED;
    rtdrv_ext_switchCfg_t switch_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    switch (TOKEN_CHAR(5,0))
    {
        case 'd':
            enable = DISABLED;
            break;

        case 'e':
            enable = ENABLED;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    switch(TOKEN_CHAR(3,0))
    {
        case '2': /* 2.5g */
            switch_cfg.unit = unit;
            switch_cfg.ipg_cmpstn = enable;
            SETSOCKOPT(RTDRV_EXT_SWITCH_IPGCOMSTN_2_5G_SET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);
            break;

        case '5': /* 5g */
            switch_cfg.unit = unit;
            switch_cfg.ipg_cmpstn = enable;
            SETSOCKOPT(RTDRV_EXT_SWITCH_IPGCOMSTN_5G_SET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}

/*
 *  switch set ipg-compensation ( ge | 10g ) ( 65ppm | 90ppm )
 */
cparser_result_t cparser_cmd_switch_set_ipg_compensation_ge_10g_65ppm_90ppm(cparser_context_t *context)
{
    uint32 unit;
    uint32 master_view_unit;
    uint32 ipg_cmpstn_sel = 1;
    rtdrv_ext_switchCfg_t switch_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    switch(TOKEN_CHAR(4,0))
    {
        case '6':
            ipg_cmpstn_sel = 0;
            break;

        case '9':
            ipg_cmpstn_sel = 1;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    switch(TOKEN_CHAR(3,0))
    {
        case 'g': /* ge */
            switch_cfg.unit = unit;
            switch_cfg.ipg_cmpstn_sel = ipg_cmpstn_sel;
            SETSOCKOPT(RTDRV_EXT_SWITCH_IPGCOMSTN_SEL_SET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);
            break;

        case '1': /* 10g */
            switch_cfg.unit = unit;
            switch_cfg.ipg_cmpstn_sel = ipg_cmpstn_sel;
            SETSOCKOPT(RTDRV_EXT_SWITCH_IPGCOMSTN_10G_SEL_SET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}

/*
 *  switch set ipg-compensation ( 2_5g | 5g ) ( 65ppm | 90ppm )
 */
cparser_result_t cparser_cmd_switch_set_ipg_compensation_2_5g_5g_65ppm_90ppm(cparser_context_t *context)
{
    uint32 unit;
    uint32 master_view_unit;
    uint32 ipg_cmpstn_sel = 1;
    rtdrv_ext_switchCfg_t switch_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    switch(TOKEN_CHAR(4,0))
    {
        case '6':
            ipg_cmpstn_sel = 0;
            break;

        case '9':
            ipg_cmpstn_sel = 1;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    switch(TOKEN_CHAR(3,0))
    {
        case '2': /* 2.5g */
            switch_cfg.unit = unit;
            switch_cfg.ipg_cmpstn_sel = ipg_cmpstn_sel;
            SETSOCKOPT(RTDRV_EXT_SWITCH_IPGCOMSTN_2_5G_SEL_SET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);
            break;

        case '5': /* 5g */
            switch_cfg.unit = unit;
            switch_cfg.ipg_cmpstn_sel = ipg_cmpstn_sel;
            SETSOCKOPT(RTDRV_EXT_SWITCH_IPGCOMSTN_5G_SEL_SET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}

/*
 *  switch set ipg-min-length 10m_100m <UINT:len>
 */
cparser_result_t cparser_cmd_switch_set_ipg_min_length_10m_100m_len(cparser_context_t *context,
    uint32_t *len_ptr)
{
    uint32 unit;
    uint32 master_view_unit;
    rtdrv_ext_switchCfg_t switch_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    switch_cfg.unit = unit;
    switch_cfg.min_ipg = *len_ptr;
    SETSOCKOPT(RTDRV_EXT_SWITCH_IPGMINLEN_10M_100M_SET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);

    return CPARSER_OK;
}

/*
 *  switch set ipg-min-length 1g_2_5g <UINT:len>
 */
cparser_result_t cparser_cmd_switch_set_ipg_min_length_1g_2_5g_len(cparser_context_t *context,
    uint32_t *len_ptr)
{
    uint32 unit;
    uint32 master_view_unit;
    rtdrv_ext_switchCfg_t switch_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    switch_cfg.unit = unit;
    switch_cfg.min_ipg = *len_ptr;
    SETSOCKOPT(RTDRV_EXT_SWITCH_IPGMINLEN_1G_2_5G_SET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);

    return CPARSER_OK;
}

/*
 *  switch set interrupt link-change state ( disable | enable )
 */
cparser_result_t cparser_cmd_switch_set_interrupt_link_change_state_disable_enable(cparser_context_t *context)
{
    uint32 unit;
    uint32 master_view_unit;
    rtdrv_ext_switchCfg_t switch_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    switch_cfg.unit = unit;
    switch (TOKEN_CHAR(5,0))
    {
        case 'd':
            SETSOCKOPT(RTDRV_EXT_SWITCH_INTR_LINK_CHANGE_DISABLE_SET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);
            break;

        default:
            SETSOCKOPT(RTDRV_EXT_SWITCH_INTR_LINK_CHANGE_ENABLE_SET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);
    }

    return CPARSER_OK;
}

/*
 * switch set back-pressure ( jam | defer )
 */
cparser_result_t cparser_cmd_switch_set_back_pressure_jam_defer(cparser_context_t *context)
{
    uint32 unit;
    uint32 master_view_unit;
    uint32 bkpres = 1;
    rtdrv_ext_switchCfg_t switch_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    switch(TOKEN_CHAR(3,0))
    {
        case 'j':
            bkpres = 0;
            break;

        case 'd':
            bkpres = 1;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    switch_cfg.unit = unit;
    switch_cfg.bkpres = bkpres;
    SETSOCKOPT(RTDRV_EXT_SWITCH_BKPRES_SET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);

    return CPARSER_OK;
}

/*
 * switch set pass-all-mode port ( <PORT_LIST:ports> | all ) state ( disable | enable )
 */
cparser_result_t cparser_cmd_switch_set_pass_all_mode_port_ports_all_state_disable_enable(cparser_context_t *context,
    char **ports_ptr)
{
    uint32 unit, port;
    uint32  master_view_unit;
    int32  ret = RT_ERR_FAILED;
    rtk_enable_t enable = DISABLED;
    diag_portlist_t portlist;
    rtdrv_ext_switchCfg_t switch_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    switch(TOKEN_CHAR(6,0))
    {
        case 'd':
            enable = DISABLED;
            break;

        case 'e':
            enable = ENABLED;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    switch_cfg.unit = unit;
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        switch_cfg.port = port;
        switch_cfg.pass_all_mode = enable;
        SETSOCKOPT(RTDRV_EXT_SWITCH_PASSALLMODE_SET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);
    }

    return CPARSER_OK;
}

/*
 * switch set bypass-tx-crc port ( <PORT_LIST:ports> | all ) state ( disable | enable )
 */
cparser_result_t cparser_cmd_switch_set_bypass_tx_crc_port_ports_all_state_disable_enable(cparser_context_t *context,
    char **ports_ptr)
{
    uint32 unit, port;
    uint32  master_view_unit;
    int32  ret = RT_ERR_FAILED;
    rtk_enable_t enable = DISABLED;
    diag_portlist_t portlist;
    rtdrv_ext_switchCfg_t switch_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    switch(TOKEN_CHAR(6,0))
    {
        case 'd':
            enable = DISABLED;
            break;

        case 'e':
            enable = ENABLED;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    switch_cfg.unit = unit;
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        switch_cfg.port = port;
        switch_cfg.bypass_tx_crc = enable;
        SETSOCKOPT(RTDRV_EXT_SWITCH_BYPASSTXCRC_SET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);
    }

    return CPARSER_OK;
}

/*
 * switch set padding-under-size port ( <PORT_LIST:ports> | all ) state ( disable | enable )
 */
cparser_result_t cparser_cmd_switch_set_padding_under_size_port_ports_all_state_disable_enable(cparser_context_t *context,
    char **ports_ptr)
{
    uint32 unit, port;
    uint32  master_view_unit;
    int32  ret = RT_ERR_FAILED;
    rtk_enable_t enable = DISABLED;
    diag_portlist_t portlist;
    rtdrv_ext_switchCfg_t switch_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    switch(TOKEN_CHAR(6,0))
    {
        case 'd':
            enable = DISABLED;
            break;

        case 'e':
            enable = ENABLED;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    switch_cfg.unit = unit;
    switch_cfg.enable = enable;
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        switch_cfg.port = port;
        SETSOCKOPT(RTDRV_EXT_SWITCH_PADDINGUNDSIZE_SET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);
    }

    return CPARSER_OK;
}

/** QoS **/

/** Port **/
/*
 * port set special-congest drain-out-thresh <UINT:threshold> duplex ( full | half )
 */
cparser_result_t cparser_cmd_port_set_special_congest_drain_out_thresh_threshold_duplex_full_half(
    cparser_context_t *context,
    uint32_t *threshold_ptr)
{
    uint32 unit;
    uint32 master_view_unit;
    rtdrv_ext_portCfg_t port_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    port_cfg.unit = unit;

    if (TOKEN_CHAR(6,0) == 'f')
    {
        port_cfg.full_th = *threshold_ptr;
        SETSOCKOPT(RTDRV_EXT_PORT_SPECIAL_CONGEST_DRAIN_OUT_THRESH_SET, &port_cfg, rtdrv_ext_portCfg_t, 1);
    }
    else if (TOKEN_CHAR(6,0) == 'h')
    {
        port_cfg.half_th = *threshold_ptr;
        SETSOCKOPT(RTDRV_EXT_PORT_SPECIAL_CONGEST_DRAIN_OUT_THRESH_HALF_SET, &port_cfg, rtdrv_ext_portCfg_t, 1);
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}

/*
 * port set special-congest port ( <PORT_LIST:ports> | all ) sustain-timer <UINT:second> duplex ( full | half )
 */
cparser_result_t cparser_cmd_port_set_special_congest_port_ports_all_sustain_timer_second_duplex_full_half(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *second_ptr)
{
    uint32 unit, port;
    uint32  master_view_unit;
    int32  ret = RT_ERR_FAILED;
    uint32 optid;
    diag_portlist_t portlist;
    rtdrv_ext_portCfg_t port_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    port_cfg.unit = unit;
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if (TOKEN_CHAR(8,0) == 'f')
    {
        optid = RTDRV_EXT_PORT_SPECIAL_CONGEST_PORT_SUSTAIN_TIMER_SET;
        port_cfg.full_sec = *second_ptr;
    }
    else if (TOKEN_CHAR(8,0) == 'h')
    {
        optid = RTDRV_EXT_PORT_SPECIAL_CONGEST_PORT_SUSTAIN_TIMER_HALF_SET;
        port_cfg.half_sec = *second_ptr;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        port_cfg.port = port;
        SETSOCKOPT(optid, &port_cfg, rtdrv_ext_portCfg_t, 1);
    }

    return CPARSER_OK;
}



/*
 * GET
 */


/** MIB **/
/*
 * mib get reset-value
 */
cparser_result_t cparser_cmd_mib_get_reset_value(cparser_context_t *context)
{
    uint32  unit = 0;
    uint32 master_view_unit;
    rtdrv_ext_mibCfg_t cntr_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    cntr_cfg.unit = unit;
    GETSOCKOPT(RTDRV_EXT_MIB_RST_VAL_GET, &cntr_cfg, rtdrv_ext_mibCfg_t, 1);

    diag_util_mprintf("\tReset MIB Value : %d \n", cntr_cfg.rst_val);

    return CPARSER_OK;
}

/*
 * mib dump tx-error-counter port ( <PORT_LIST:ports> | all )
 */
cparser_result_t cparser_cmd_mib_dump_tx_error_counter_port_ports_all(cparser_context_t *context,
    char **ports_ptr)
{
    uint32 unit, port;
    uint32  master_view_unit;
    int32  ret = RT_ERR_FAILED;
    rtdrv_ext_mibCfg_t cntr_cfg;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("TX CRC Error Counter\n");
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cntr_cfg.unit = unit;
        cntr_cfg.port = port;
        GETSOCKOPT(RTDRV_EXT_MIB_TX_ERR_CNTR_GET, &cntr_cfg, rtdrv_ext_mibCfg_t, 1);
        diag_util_mprintf("\tPort %2d : %d\n", port, cntr_cfg.cntr);
    }

    return CPARSER_OK;
}

/*
 * mib reset tx-error-counter port ( <PORT_LIST:ports> | all )
 */
cparser_result_t cparser_cmd_mib_reset_tx_error_counter_port_ports_all(cparser_context_t *context,
    char **ports_ptr)
{
    uint32 unit, port;
    uint32  master_view_unit;
    int32  ret = RT_ERR_FAILED;
    rtdrv_ext_mibCfg_t cntr_cfg;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cntr_cfg.unit = unit;
        cntr_cfg.port = port;
        cntr_cfg.rst_val = 0x1;
        SETSOCKOPT(RTDRV_EXT_MIB_TX_ERR_CNTR_RESET, &cntr_cfg, rtdrv_ext_mibCfg_t, 1);
    }

    return CPARSER_OK;
}

/** SWITCH **/
cparser_result_t cparser_cmd_switch_get_48_pass_1_state(cparser_context_t *context)
{
    uint32 unit;
    uint32 master_view_unit;
    rtk_enable_t enable = DISABLED;
    rtdrv_ext_switchCfg_t switch_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    switch_cfg.unit = unit;
    GETSOCKOPT(RTDRV_EXT_SWITCH_48PASS1_GET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);
    enable = switch_cfg.half_48pass1;

    diag_util_mprintf("\t48pass1 State: %s\n", (enable == ENABLED) ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);

    return CPARSER_OK;
}

/*
 * switch get limit-pause state
 */
cparser_result_t cparser_cmd_switch_get_limit_pause_state(cparser_context_t *context)
{
    uint32 unit;
    uint32 master_view_unit;
    uint32 value;
    rtdrv_ext_switchCfg_t switch_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    switch_cfg.unit = unit;
    GETSOCKOPT(RTDRV_EXT_SWITCH_LIMITPAUSE_GET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);
    value = switch_cfg.value;

    if (value == 0)
        diag_util_mprintf("\tLimit Pause State : %s\n", DIAG_STR_DISABLE);
    else
        diag_util_mprintf("\tLimit Pause State : %s\n", (value == 0x1) ? "128-pause frames" : "32-pause frames");

    return CPARSER_OK;
}

/*
 *  switch get ipg-compensation ( ge | 10g ) state
 */
cparser_result_t cparser_cmd_switch_get_ipg_compensation_ge_10g_state(cparser_context_t *context)
{
    uint32 unit;
    uint32 master_view_unit;
    rtk_enable_t enable = DISABLED;
    rtdrv_ext_switchCfg_t switch_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    switch(TOKEN_CHAR(3,0))
    {
        case 'g': /* ge */
            switch_cfg.unit = unit;
            GETSOCKOPT(RTDRV_EXT_SWITCH_IPGCOMSTN_GET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);
            enable = switch_cfg.ipg_cmpstn;
            diag_util_mprintf("\tGE IPG Compensation State : %s\n", (enable == ENABLED) ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
            break;

        case '1': /* 10g */
            switch_cfg.unit = unit;
            GETSOCKOPT(RTDRV_EXT_SWITCH_IPGCOMSTN_10G_GET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);
            enable = switch_cfg.ipg_cmpstn;
            diag_util_mprintf("\t10G IPG Compensation State : %s\n", (enable == ENABLED) ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}

/*
 *  switch get ipg-compensation ( 2_5g | 5g ) state
 */
cparser_result_t cparser_cmd_switch_get_ipg_compensation_2_5g_5g_state(cparser_context_t * context)
{
    uint32 unit;
    uint32 master_view_unit;
    rtk_enable_t enable = DISABLED;
    rtdrv_ext_switchCfg_t switch_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    switch(TOKEN_CHAR(3,0))
    {
        case '2': /* 2.5g */
            switch_cfg.unit = unit;
            GETSOCKOPT(RTDRV_EXT_SWITCH_IPGCOMSTN_2_5G_GET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);
            enable = switch_cfg.ipg_cmpstn;
            diag_util_mprintf("\t2.5G IPG Compensation State : %s\n", (enable == ENABLED) ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
            break;

        case '5': /* 5g */
            switch_cfg.unit = unit;
            GETSOCKOPT(RTDRV_EXT_SWITCH_IPGCOMSTN_5G_GET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);
            enable = switch_cfg.ipg_cmpstn;
            diag_util_mprintf("\t5G IPG Compensation State : %s\n", (enable == ENABLED) ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}

/*
 *  switch get ipg-compensation ( ge | 10g )
 */
cparser_result_t cparser_cmd_switch_get_ipg_compensation_ge_10g(cparser_context_t *context)
{
    uint32 unit;
    uint32 master_view_unit;
    rtdrv_ext_switchCfg_t switch_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    switch(TOKEN_CHAR(3,0))
    {
        case 'g': /* ge */
            switch_cfg.unit = unit;
            GETSOCKOPT(RTDRV_EXT_SWITCH_IPGCOMSTN_SEL_GET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);
            diag_util_mprintf("\tGE IPG Compensation : %s\n", (switch_cfg.ipg_cmpstn_sel == 1) ? "90ppm" : "65ppm");
            break;

        case '1': /* 10g */
            switch_cfg.unit = unit;
            GETSOCKOPT(RTDRV_EXT_SWITCH_IPGCOMSTN_10G_SEL_GET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);
            diag_util_mprintf("\t10G IPG Compensation : %s\n", (switch_cfg.ipg_cmpstn_sel == 1) ? "90ppm" : "65ppm");
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}

/*
 *  switch get ipg-compensation ( 2_5g | 5g )
 */
cparser_result_t cparser_cmd_switch_get_ipg_compensation_2_5g_5g(cparser_context_t *context)
{
    uint32 unit;
    uint32 master_view_unit;
    rtdrv_ext_switchCfg_t switch_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    switch(TOKEN_CHAR(3,0))
    {
        case '2': /* 2.5g */
            switch_cfg.unit = unit;
            GETSOCKOPT(RTDRV_EXT_SWITCH_IPGCOMSTN_2_5G_SEL_GET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);
            diag_util_mprintf("\t2.5G IPG Compensation : %s\n", (switch_cfg.ipg_cmpstn_sel == 1) ? "90ppm" : "65ppm");
            break;

        case '5': /* 5g */
            switch_cfg.unit = unit;
            GETSOCKOPT(RTDRV_EXT_SWITCH_IPGCOMSTN_5G_SEL_GET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);
            diag_util_mprintf("\t5G IPG Compensation : %s\n", (switch_cfg.ipg_cmpstn_sel == 1) ? "90ppm" : "65ppm");
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}

/*
 * switch get ipg-min-length
 */
cparser_result_t cparser_cmd_switch_get_ipg_min_length(cparser_context_t *context)
{
    uint32 unit;
    uint32 master_view_unit;
    rtdrv_ext_switchCfg_t switch_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    switch_cfg.unit = unit;
    GETSOCKOPT(RTDRV_EXT_SWITCH_IPGMINLEN_10M_100M_GET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);
    diag_util_mprintf("10M and 100M IPG Minimum Receive Length Configuration : %d\n", switch_cfg.min_ipg);

    GETSOCKOPT(RTDRV_EXT_SWITCH_IPGMINLEN_1G_2_5G_GET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);
    diag_util_mprintf("1G and 2.5G IPG Minimum Receive Length Configuration : %d\n", switch_cfg.min_ipg);

    return CPARSER_OK;
}

cparser_result_t cparser_cmd_switch_get_back_pressure(cparser_context_t *context)
{
    uint32 unit;
    uint32 master_view_unit;
    rtdrv_ext_switchCfg_t switch_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    switch_cfg.unit = unit;
    GETSOCKOPT(RTDRV_EXT_SWITCH_BKPRES_GET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);

    diag_util_mprintf("\tBackpressure : %s\n", (switch_cfg.bkpres == 1) ? "Defer mode" : "Jam mode");

    return CPARSER_OK;
}

/*
 * switch get pass-all-mode port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t cparser_cmd_switch_get_pass_all_mode_port_ports_all_state(cparser_context_t *context,
    char **ports_ptr)
{
    uint32 unit, port;
    uint32  master_view_unit;
    int32  ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtdrv_ext_switchCfg_t switch_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    switch_cfg.unit = unit;


    diag_util_mprintf("Pass All Packets Mode Status\n");

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        switch_cfg.port = port;
        GETSOCKOPT(RTDRV_EXT_SWITCH_PASSALLMODE_GET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);
        diag_util_mprintf("\tPort %2d : %s\n", port, (switch_cfg.pass_all_mode == ENABLED) ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
    }

    return CPARSER_OK;
}

/*
 * switch get rx-check-crc port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t cparser_cmd_switch_get_rx_check_crc_port_ports_all_state(cparser_context_t *context,
    char **ports_ptr)
{
    uint32 unit, port;
    uint32  master_view_unit;
    int32  ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtdrv_ext_switchCfg_t switch_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    switch_cfg.unit = unit;


    diag_util_mprintf("RX Check CRC Function Status\n");

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        switch_cfg.port = port;
        GETSOCKOPT(RTDRV_EXT_SWITCH_RXCHECKCRC_GET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);
        diag_util_mprintf("\tPort %2d : %s\n", port, (switch_cfg.rx_check_crc == ENABLED) ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
    }

    return CPARSER_OK;
}

/*
 * switch get bypass-tx-crc port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t cparser_cmd_switch_get_bypass_tx_crc_port_ports_all_state(cparser_context_t *context,
    char **ports_ptr)
{
    uint32 unit, port;
    uint32  master_view_unit;
    int32  ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtdrv_ext_switchCfg_t switch_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    switch_cfg.unit = unit;


    diag_util_mprintf("Bypress TX Recalculating CRC Status\n");

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        switch_cfg.port = port;
        GETSOCKOPT(RTDRV_EXT_SWITCH_BYPASSTXCRC_GET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);
        diag_util_mprintf("\tPort %2d : %s\n", port, (switch_cfg.bypass_tx_crc == ENABLED) ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
    }

    return CPARSER_OK;
}

/*
 * switch get padding-under-size port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t cparser_cmd_switch_get_padding_under_size_port_ports_all_state(cparser_context_t *context,
    char **ports_ptr)
{
    uint32 unit, port;
    uint32  master_view_unit;
    int32  ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtdrv_ext_switchCfg_t switch_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    switch_cfg.unit = unit;
    diag_util_mprintf("Padding Under Size State\n");

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        switch_cfg.port = port;
        GETSOCKOPT(RTDRV_EXT_SWITCH_PADDINGUNDSIZE_GET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);
        diag_util_mprintf("\tPort %2d : %s\n", port, (switch_cfg.enable == ENABLED) ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
    }

    return CPARSER_OK;
}

/** QoS **/

/** Port **/

/*
 * port get special-congest drain-out-thresh
 */
cparser_result_t cparser_cmd_port_get_special_congest_drain_out_thresh(cparser_context_t *context)
{
    uint32 unit;
    uint32 master_view_unit;
    rtdrv_ext_portCfg_t port_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&port_cfg, 0x00, sizeof(rtdrv_ext_portCfg_t));
    port_cfg.unit = unit;
    GETSOCKOPT(RTDRV_EXT_PORT_SPECIAL_CONGEST_DRAIN_OUT_THRESH_GET, &port_cfg, rtdrv_ext_portCfg_t, 1);
    diag_util_mprintf("\tFull Duplex Drain-Out Threshold : %d\n", port_cfg.full_th);
    diag_util_mprintf("\tHalf Duplex Drain-Out Threshold : %d\n", port_cfg.half_th);

    return CPARSER_OK;
}

/*
 * port get special-congest port ( <PORT_LIST:ports> | all )
 */
cparser_result_t cparser_cmd_port_get_special_congest_port_ports_all(cparser_context_t *context, char **ports_ptr)
{
    uint32      unit, port;
    uint32  master_view_unit;
    int32       ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtdrv_ext_portCfg_t port_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&port_cfg, 0x00, sizeof(rtdrv_ext_portCfg_t));
    port_cfg.unit = unit;
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        port_cfg.port = port;
        GETSOCKOPT(RTDRV_EXT_PORT_SPECIAL_CONGEST_PORT_GET, &port_cfg, rtdrv_ext_portCfg_t, 1);
        diag_util_mprintf("Port %2d : \n", port);
        diag_util_mprintf("\tFull Duplex Sustain Timer : %d\n", port_cfg.full_sec);
        diag_util_mprintf("\tHalf Duplex Sustain Timer : %d\n", port_cfg.half_sec);
    }

    return CPARSER_OK;
}

cparser_result_t cparser_cmd_eee_get_tx_wake_mode(cparser_context_t *context)
{
    uint32  unit = 0;
    uint32 master_view_unit;
    rtdrv_ext_eeeCfg_t  eee_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    eee_cfg.unit = unit;
    GETSOCKOPT(RTDRV_EXT_EEE_TX_WAKE_MODE_GET, &eee_cfg, rtdrv_ext_eeeCfg_t, 1);

    diag_util_printf("\tEEE TX Wake Mode : ");
    switch(eee_cfg.wakeMode)
    {
        case WAKE_MODE_ANY_PKT_TX:
            diag_util_mprintf("packet-tx\n");
            break;
        case WAKE_MODE_QOS_BASED:
            diag_util_mprintf("qos-based\n");
            break;
        default:
            break;
    }

    return CPARSER_OK;

}

cparser_result_t cparser_cmd_eee_get_port_ports_all_status(cparser_context_t *context,
    char **ports_ptr)
{
    uint32  unit = 0;
    uint32 master_view_unit;
    int32  ret = RT_ERR_FAILED;
    rtk_port_t  port;
    diag_portlist_t portlist;
    rtdrv_ext_eeeCfg_t  eee_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    eee_cfg.unit = unit;
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    diag_util_mprintf("EEE or EEEP Status:\n");
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        eee_cfg.port = port;
        GETSOCKOPT(RTDRV_EXT_EEE_PORT_STATUS_GET, &eee_cfg, rtdrv_ext_eeeCfg_t, 1);
        diag_util_mprintf("\tPort %2d :\n", port);
        if (EEE_POWER_WAKE_STATE == eee_cfg.rxState)
            diag_util_mprintf("\t\tRX : Wake\n");
        else if (EEE_POWER_SLEEP_STATE == eee_cfg.rxState)
            diag_util_mprintf("\t\tRX : Sleep\n");

        if (EEE_POWER_WAKE_STATE == eee_cfg.txState)
            diag_util_mprintf("\t\tTX : Wake\n");
        else if (EEE_POWER_SLEEP_STATE == eee_cfg.txState)
            diag_util_mprintf("\t\tTX : Sleep\n");
    }

    return CPARSER_OK;
}

cparser_result_t cparser_cmd_eee_get_link_up_delay(cparser_context_t *context)
{
    uint32 unit;
    uint32 master_view_unit;
    rtdrv_ext_eeeCfg_t  eee_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    eee_cfg.unit = unit;
    GETSOCKOPT(RTDRV_EXT_EEE_LINK_UP_DELAY_GET, &eee_cfg, rtdrv_ext_eeeCfg_t, 1);

    diag_util_printf("\tEEE Link Up Delay ");
    switch(eee_cfg.linkUpDelay)
    {
        case 0:
            diag_util_mprintf(": 1024 ms\n");
            break;
        case 1:
            diag_util_mprintf(": 512 ms\n");
            break;
        case 2:
            diag_util_mprintf(": 256 ms\n");
            break;
        case 3:
            diag_util_mprintf(": 1 ms\n");
            break;
        default:
            break;
    }

    return CPARSER_OK;
}

cparser_result_t cparser_cmd_eee_get_multi_wake_state(cparser_context_t *context)
{
    uint32 unit;
    uint32 master_view_unit;
    rtdrv_ext_eeeCfg_t  eee_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    eee_cfg.unit = unit;
    GETSOCKOPT(RTDRV_EXT_EEE_MULTI_WAKE_STATE_GET, &eee_cfg, rtdrv_ext_eeeCfg_t, 1);

    diag_util_printf("\tEEE Multi-wake State : ");
    switch(eee_cfg.enable)
    {
        case DISABLED:
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);
            break;
        case ENABLED:
            diag_util_mprintf("%s\n",DIAG_STR_ENABLE);
            break;
        default:
            break;
    }

    return CPARSER_OK;
}

cparser_result_t cparser_cmd_eee_get_multi_wake_interval(cparser_context_t *context)
{
    uint32 unit;
    uint32 master_view_unit;
    rtdrv_ext_eeeCfg_t  eee_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    eee_cfg.unit = unit;
    GETSOCKOPT(RTDRV_EXT_EEE_MULTI_WAKE_INTERVAL_GET, &eee_cfg, rtdrv_ext_eeeCfg_t, 1);

    diag_util_printf("\tEEE Multi-wake Interval : ");
    switch(eee_cfg.interval)
    {
        case 0:
            diag_util_mprintf("1 us\n");
            break;
        case 1:
            diag_util_mprintf("2 us\n");
            break;
        case 2:
            diag_util_mprintf("3 us\n");
            break;
        case 3:
            diag_util_mprintf("4 us\n");
            break;
        default:
            break;
    }

    return CPARSER_OK;
}

cparser_result_t cparser_cmd_eee_get_multi_wake_port_num(cparser_context_t *context)
{
    uint32 unit;
    uint32 master_view_unit;
    rtdrv_ext_eeeCfg_t  eee_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    eee_cfg.unit = unit;
    GETSOCKOPT(RTDRV_EXT_EEE_MULTI_WAKE_PORT_NUM_GET, &eee_cfg, rtdrv_ext_eeeCfg_t, 1);

    diag_util_printf("\tEEE Multi-wake Port Number of a Group : ");
    switch(eee_cfg.portNum)
    {
        case 0:
            diag_util_mprintf("2 ports\n");
            break;
        case 1:
            diag_util_mprintf("4 ports\n");
            break;
        case 2:
            diag_util_mprintf("6 ports\n");
            break;
        case 3:
            diag_util_mprintf("8 ports\n");
            break;
        case 4:
            diag_util_mprintf("10 ports\n");
            break;
        case 5:
            diag_util_mprintf("12 ports\n");
            break;
        case 6:
            diag_util_mprintf("16 ports\n");
            break;
        case 7:
            diag_util_mprintf("20 ports\n");
            break;
        default:
            break;
    }

    return CPARSER_OK;
}

cparser_result_t cparser_cmd_eee_set_tx_wake_mode_packet_tx_qos_based(cparser_context_t *context)
{
    uint32 unit;
    uint32 master_view_unit;
    rtdrv_ext_eeeCfg_t  eee_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    switch(TOKEN_CHAR(4,0))
    {
        case 'p':
            eee_cfg.wakeMode = WAKE_MODE_ANY_PKT_TX;
            break;

        case 'q':
            eee_cfg.wakeMode = WAKE_MODE_QOS_BASED;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    eee_cfg.unit = unit;
    SETSOCKOPT(RTDRV_EXT_EEE_TX_WAKE_MODE_SET, &eee_cfg, rtdrv_ext_eeeCfg_t, 1);

    return CPARSER_OK;
}

cparser_result_t cparser_cmd_eee_set_link_up_delay_1ms_256ms_512ms_1024ms(cparser_context_t *context)
{
    uint32 unit;
    uint32 master_view_unit;
    rtdrv_ext_eeeCfg_t  eee_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    switch(TOKEN_CHAR(3,1))
    {
        case 'm': /*1ms*/
            eee_cfg.linkUpDelay = 3;
            break;
        case '5': /*256ms*/
            eee_cfg.linkUpDelay = 2;
            break;
        case '1': /*512ms*/
            eee_cfg.linkUpDelay = 1;
            break;
        case '0': /*1024ms*/
            eee_cfg.linkUpDelay = 0;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    eee_cfg.unit = unit;
    SETSOCKOPT(RTDRV_EXT_EEE_LINK_UP_DELAY_SET, &eee_cfg, rtdrv_ext_eeeCfg_t, 1);

    return CPARSER_OK;
}

cparser_result_t cparser_cmd_eee_set_multi_wake_state_disable_enable(cparser_context_t *context)
{
    uint32 unit;
    uint32 master_view_unit;
    rtdrv_ext_eeeCfg_t  eee_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    switch(TOKEN_CHAR(4,0))
    {
        case 'd': /*disable*/
            eee_cfg.enable = DISABLED;
            break;
        case 'e': /*enable*/
            eee_cfg.enable = ENABLED;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    eee_cfg.unit = unit;
    SETSOCKOPT(RTDRV_EXT_EEE_MULTI_WAKE_STATE_SET, &eee_cfg, rtdrv_ext_eeeCfg_t, 1);

    return CPARSER_OK;
}

cparser_result_t cparser_cmd_eee_set_multi_wake_interval_1us_2us_3us_4us(cparser_context_t *context)
{
    uint32 unit;
    uint32 master_view_unit;
    rtdrv_ext_eeeCfg_t  eee_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    switch(TOKEN_CHAR(4,0))
    {
        case '1': /*1us*/
            eee_cfg.interval = 0;
            break;
        case '2': /*2us*/
            eee_cfg.interval = 1;
            break;
        case '3': /*3us*/
            eee_cfg.interval = 2;
            break;
        case '4': /*4us*/
            eee_cfg.interval = 3;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    eee_cfg.unit = unit;
    SETSOCKOPT(RTDRV_EXT_EEE_MULTI_WAKE_INTERVAL_SET, &eee_cfg, rtdrv_ext_eeeCfg_t, 1);

    return CPARSER_OK;
}

cparser_result_t cparser_cmd_eee_set_multi_wake_port_num_2port_4port_6port_8port_10port_12port_16port_20port(cparser_context_t *context)
{
    uint32 unit;
    uint32 master_view_unit;
    rtdrv_ext_eeeCfg_t  eee_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    switch(TOKEN_CHAR(4,0))
    {
        case '2':
            if('p' == TOKEN_CHAR(4,1)) /*2port*/
                eee_cfg.portNum = 0;
            else /*20port*/
                eee_cfg.portNum = 7;
            break;
        case '4': /*4port*/
            eee_cfg.portNum = 1;
            break;
        case '6': /*6port*/
            eee_cfg.portNum = 2;
            break;
        case '8': /*8port*/
            eee_cfg.portNum = 3;
            break;
        case '1':
            if('0' == TOKEN_CHAR(4,1)) /*10port*/
                eee_cfg.portNum = 4;
            else if('2' == TOKEN_CHAR(4,1)) /*12port*/
                eee_cfg.portNum = 5;
            else
                eee_cfg.portNum = 6; /*16port*/
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    eee_cfg.unit = unit;
    SETSOCKOPT(RTDRV_EXT_EEE_MULTI_WAKE_PORT_NUM_SET, &eee_cfg, rtdrv_ext_eeeCfg_t, 1);

    return CPARSER_OK;
}

cparser_result_t cparser_cmd_eeep_get_rx_tx_sleep_rate(cparser_context_t *context)
{
    uint32 unit;
    uint32 master_view_unit;
    rtdrv_ext_eeeCfg_t  eee_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    eee_cfg.unit = unit;

    switch(TOKEN_CHAR(2,0))
    {
        case 't':
            GETSOCKOPT(RTDRV_EXT_EEEP_TX_SLEEP_RATE_GET, &eee_cfg, rtdrv_ext_eeeCfg_t, 1);
            diag_util_mprintf("\tEEEP TX Sleep Rate (unit:0.5Mbps): %d\n", eee_cfg.rate);
            break;

        case 'r':
            GETSOCKOPT(RTDRV_EXT_EEEP_RX_SLEEP_RATE_GET, &eee_cfg, rtdrv_ext_eeeCfg_t, 1);
            diag_util_mprintf("\tEEEP RX Sleep Rate (unit:0.5Mbps): %d\n", eee_cfg.rate);
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}

cparser_result_t cparser_cmd_eeep_set_rx_tx_sleep_rate_rate(cparser_context_t *context,
    uint32_t *rate_ptr)
{
    uint32 unit;
    uint32 master_view_unit;
    rtdrv_ext_eeeCfg_t  eee_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    eee_cfg.unit = unit;
    eee_cfg.rate = *rate_ptr;

    switch(TOKEN_CHAR(2,0))
    {
        case 't':
            SETSOCKOPT(RTDRV_EXT_EEEP_TX_SLEEP_RATE_SET, &eee_cfg, rtdrv_ext_eeeCfg_t, 1);
            break;

        case 'r':
            SETSOCKOPT(RTDRV_EXT_EEEP_RX_SLEEP_RATE_SET, &eee_cfg, rtdrv_ext_eeeCfg_t, 1);
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}

/*
 * iol dump
 */
cparser_result_t cparser_cmd_iol_dump(cparser_context_t *context)
{
    uint32 unit;
    uint32 master_view_unit;
    rtdrv_ext_iolCfg_t  iol_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    iol_cfg.unit = unit;
    GETSOCKOPT(RTDRV_EXT_IOL_COLLISION_MAX_ATTEMPT_GET, &iol_cfg, rtdrv_ext_iolCfg_t, 1);
    diag_util_printf("Collision-max-attempt: ");
    switch(iol_cfg.action)
    {
        case 0:
            diag_util_mprintf("retry\n");
            break;
        case 1:
            diag_util_mprintf("drop\n");
            break;
        default:
            break;
    }

    GETSOCKOPT(RTDRV_EXT_IOL_ERROR_LENGTH_GET, &iol_cfg, rtdrv_ext_iolCfg_t, 1);
    diag_util_printf("Error-length: ");
    switch(iol_cfg.action)
    {
        case 0:
            diag_util_mprintf("forward\n");
            break;
        case 1:
            diag_util_mprintf("drop\n");
            break;
        default:
            break;
    }

    GETSOCKOPT(RTDRV_EXT_IOL_LATE_COLLISION_GET, &iol_cfg, rtdrv_ext_iolCfg_t, 1);
    diag_util_printf("Late-collision: ");
    switch(iol_cfg.action)
    {
        case 0:
            diag_util_mprintf("re-transmit\n");
            break;
        case 1:
            diag_util_mprintf("drop\n");
            break;
        default:
            break;
    }

    GETSOCKOPT(RTDRV_EXT_IOL_MAX_LENGTH_GET, &iol_cfg, rtdrv_ext_iolCfg_t, 1);
    diag_util_printf("Max-length: ");
    switch(iol_cfg.enable)
    {
        case DISABLED:
            diag_util_mprintf("disable\n");
            break;
        case ENABLED:
            diag_util_mprintf("enable\n");
            break;
        default:
            break;
    }

#if 0
    GETSOCKOPT(RTDRV_EXT_IOL_MAX_LENGTH_PORT_GET, &iol_cfg, rtdrv_ext_iolCfg_t, 1);
    diag_util_printf("Max-length: ");
    switch(iol_cfg.enable)
    {
        case DISABLED:
            diag_util_mprintf("disable\n");
            break;
        case ENABLED:
            diag_util_mprintf("enable\n");
            break;
        default:
            break;
    }
#endif

    return CPARSER_OK;
}

/*
 * iol get collision-max-attempt
 */
cparser_result_t cparser_cmd_iol_get_collision_max_attempt(cparser_context_t *context)
{
    uint32 unit;
    uint32 master_view_unit;
    rtdrv_ext_iolCfg_t  iol_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    iol_cfg.unit = unit;
    GETSOCKOPT(RTDRV_EXT_IOL_COLLISION_MAX_ATTEMPT_GET, &iol_cfg, rtdrv_ext_iolCfg_t, 1);

    diag_util_printf("Collision-max-attempt: ");
    switch(iol_cfg.action)
    {
        case 0:
            diag_util_mprintf("retry\n");
            break;
        case 1:
            diag_util_mprintf("drop\n");
            break;
        default:
            break;
    }

    return CPARSER_OK;
}

/*
 * iol get error-length
 */
cparser_result_t cparser_cmd_iol_get_error_length(cparser_context_t *context)
{
    uint32 unit;
    uint32 master_view_unit;
    rtdrv_ext_iolCfg_t  iol_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    iol_cfg.unit = unit;
    GETSOCKOPT(RTDRV_EXT_IOL_ERROR_LENGTH_GET, &iol_cfg, rtdrv_ext_iolCfg_t, 1);

    diag_util_printf("Error-length: ");
    switch(iol_cfg.action)
    {
        case 0:
            diag_util_mprintf("forward\n");
            break;
        case 1:
            diag_util_mprintf("drop\n");
            break;
        default:
            break;
    }

    return CPARSER_OK;
}

/*
 * iol get late-collision
 */
cparser_result_t cparser_cmd_iol_get_late_collision(cparser_context_t *context)
{
    uint32 unit;
    uint32 master_view_unit;
    rtdrv_ext_iolCfg_t  iol_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    iol_cfg.unit = unit;
    GETSOCKOPT(RTDRV_EXT_IOL_LATE_COLLISION_GET, &iol_cfg, rtdrv_ext_iolCfg_t, 1);

    diag_util_printf("Late-collision: ");
    switch(iol_cfg.action)
    {
        case 0:
            diag_util_mprintf("re-transmit\n");
            break;
        case 1:
            diag_util_mprintf("drop\n");
            break;
        default:
            break;
    }

    return CPARSER_OK;
}

/*
 * iol get max-length
 */
cparser_result_t cparser_cmd_iol_get_max_length(cparser_context_t *context)
{
    uint32 unit;
    uint32 master_view_unit;
    rtdrv_ext_iolCfg_t  iol_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    iol_cfg.unit = unit;
    GETSOCKOPT(RTDRV_EXT_IOL_MAX_LENGTH_GET, &iol_cfg, rtdrv_ext_iolCfg_t, 1);

    diag_util_printf("Max-length: ");
    switch(iol_cfg.enable)
    {
        case DISABLED:
            diag_util_mprintf("disable\n");
            break;
        case ENABLED:
            diag_util_mprintf("enable\n");
            break;
        default:
            break;
    }

    return CPARSER_OK;
}

/*
 * iol set collision-max-attempt ( retry | drop )
 */
cparser_result_t cparser_cmd_iol_set_collision_max_attempt_retry_drop(cparser_context_t *context)
{
    uint32 unit;
    uint32 master_view_unit;
    rtdrv_ext_iolCfg_t  iol_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    switch(TOKEN_CHAR(3,0))
    {
        case 'r': /*retry*/
            iol_cfg.action = 0;
            break;
        case 'd': /*drop*/
            iol_cfg.action = 1;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    iol_cfg.unit = unit;
    SETSOCKOPT(RTDRV_EXT_IOL_COLLISION_MAX_ATTEMPT_SET, &iol_cfg, rtdrv_ext_iolCfg_t, 1);

    return CPARSER_OK;
}

/*
 * iol set error-length ( forward | drop )
 */
cparser_result_t cparser_cmd_iol_set_error_length_forward_drop(cparser_context_t *context)
{
    uint32 unit;
    uint32 master_view_unit;
    rtdrv_ext_iolCfg_t  iol_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    switch(TOKEN_CHAR(3,0))
    {
        case 'f': /*forward*/
            iol_cfg.action = 0;
            break;
        case 'd': /*drop*/
            iol_cfg.action = 1;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    iol_cfg.unit = unit;
    SETSOCKOPT(RTDRV_EXT_IOL_ERROR_LENGTH_SET, &iol_cfg, rtdrv_ext_iolCfg_t, 1);

    return CPARSER_OK;
}

/*
 * iol set late-collision ( re-transmit | drop )
 */
cparser_result_t cparser_cmd_iol_set_late_collision_re_transmit_drop(cparser_context_t *context)
{
    uint32 unit;
    uint32 master_view_unit;
    rtdrv_ext_iolCfg_t  iol_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    switch(TOKEN_CHAR(3,0))
    {
        case 'r': /*re-transmit*/
            iol_cfg.action = 0;
            break;
        case 'd': /*drop*/
            iol_cfg.action = 1;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    iol_cfg.unit = unit;
    SETSOCKOPT(RTDRV_EXT_IOL_LATE_COLLISION_SET, &iol_cfg, rtdrv_ext_iolCfg_t, 1);

    return CPARSER_OK;
}

/*
 * iol set max-length ( disable | enable )
 */
cparser_result_t cparser_cmd_iol_set_max_length_disable_enable(cparser_context_t *context)
{
    uint32 unit;
    uint32 master_view_unit;
    rtdrv_ext_iolCfg_t  iol_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    switch(TOKEN_CHAR(3,0))
    {
        case 'd': /*disable*/
            iol_cfg.enable = DISABLED;
            break;
        case 'e': /*enable*/
            iol_cfg.enable = ENABLED;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    iol_cfg.unit = unit;
    SETSOCKOPT(RTDRV_EXT_IOL_MAX_LENGTH_SET, &iol_cfg, rtdrv_ext_iolCfg_t, 1);

    return CPARSER_OK;
}

/*
 * pktgen get tx-cmd
 */
cparser_result_t
cparser_cmd_pktgen_get_tx_cmd(cparser_context_t *context)
{
    uint32                  unit;
    uint32                  master_view_unit;
    rtdrv_ext_pktGenCfg_t   cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    cfg.unit = unit;
    GETSOCKOPT(RTDRV_EXT_PKTGEN_TX_CMD_GET, &cfg, rtdrv_ext_pktGenCfg_t, 1);

    diag_util_mprintf("\tPacket generation TX command : ");

    switch (cfg.status)
    {
        case 0:
            diag_util_mprintf("None\n");
            break;
        case 1:
            diag_util_mprintf("start\n");
            break;
        case 2:
            diag_util_mprintf("stop and reset counter\n");
            break;
        case 3:
            diag_util_mprintf("stop and hold counter\n");
            break;
        default:
            break;
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_get_tx_cmd */

/*
 * pktgen set tx-cmd ( start | stop-and-reset-counter | stop-and-hold-counter )
 */
cparser_result_t
cparser_cmd_pktgen_set_tx_cmd_start_stop_and_reset_counter_stop_and_hold_counter(
    cparser_context_t *context)
{
    uint32                  unit;
    uint32                  master_view_unit;
    rtdrv_ext_pktGenCfg_t   cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    cfg.unit = unit;

    if ('a' == TOKEN_CHAR(3, 2))
        cfg.status = 1;
    else if ('r' == TOKEN_CHAR(3, 9))
        cfg.status = 2;
    else
        cfg.status = 3;

    SETSOCKOPT(RTDRV_EXT_PKTGEN_TX_CMD_SET, &cfg, rtdrv_ext_pktGenCfg_t, 1);

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_set_tx_cmd_start_stop_and_reset_counter_stop_and_hold_counter */

/*
 * pktgen get state
 */
cparser_result_t
cparser_cmd_pktgen_get_state(
    cparser_context_t *context)
{
    uint32                  unit;
    uint32                  master_view_unit;
    rtdrv_ext_pktGenCfg_t   cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    cfg.unit = unit;
    GETSOCKOPT(RTDRV_EXT_PKTGEN_STATE_GET, &cfg, rtdrv_ext_pktGenCfg_t, 1);

    diag_util_mprintf("\tPacket generation status : ");
    if (0 == cfg.enable)
        diag_util_mprintf("%s\n", DIAG_STR_DISABLE);
    else if (1 == cfg.enable)
        diag_util_mprintf("%s\n",DIAG_STR_ENABLE);
    else
        diag_util_mprintf("\n");

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_get_state */

/*
 * pktgen set state ( enable | disable )
 */
cparser_result_t
cparser_cmd_pktgen_set_state_enable_disable(
    cparser_context_t *context)
{
    uint32                  unit;
    uint32                  master_view_unit;
    rtdrv_ext_pktGenCfg_t   cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    cfg.unit = unit;
    switch(TOKEN_CHAR(3,0))
    {
        case 'd': /*disable*/
            cfg.enable = DISABLED;
            break;
        case 'e': /*enable*/
            cfg.enable = ENABLED;
            break;
        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }
    SETSOCKOPT(RTDRV_EXT_PKTGEN_STATE_SET, &cfg, rtdrv_ext_pktGenCfg_t, 1);

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_set_state_enable_disable */

/*
 * pktgen get port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t
cparser_cmd_pktgen_get_port_ports_all_state(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32                  unit, ret;
    uint32  master_view_unit;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    cfg.unit = unit;

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        GETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STATE_GET, &cfg,
                rtdrv_ext_pktGenCfg_t, 1);
        diag_util_mprintf("Port %2d : \n", port);
        diag_util_mprintf("\tTX state : ");
        if (0 == cfg.enable)
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);
        else if (1 == cfg.enable)
            diag_util_mprintf("%s\n",DIAG_STR_ENABLE);
        else
            diag_util_mprintf("\n");
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_get_port_ports_all_state */

/*
 * pktgen set port ( <PORT_LIST:ports> | all ) state ( enable | disable )
 */
cparser_result_t
cparser_cmd_pktgen_set_port_ports_all_state_enable_disable(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32                  unit;
    uint32                  master_view_unit;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    cfg.unit = unit;
    switch(TOKEN_CHAR(5,0))
    {
        case 'd': /*disable*/
            cfg.enable = DISABLED;
            break;
        case 'e': /*enable*/
            cfg.enable = ENABLED;
            break;
        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        SETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STATE_SET, &cfg,
                rtdrv_ext_pktGenCfg_t, 1);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_set_port_ports_all_state_enable_disable */

/*
 * pktgen get port ( <PORT_LIST:ports> | all ) tx-done state
 */
cparser_result_t
cparser_cmd_pktgen_get_port_ports_all_tx_done_state(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32                  unit;
    uint32                  master_view_unit;
    int32                   ret;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    cfg.unit = unit;
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        GETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_TX_DONE_STATE_GET, &cfg,
                rtdrv_ext_pktGenCfg_t, 1);
        diag_util_mprintf("Port %2d : \n", port);
        diag_util_mprintf("\tTX done state : ");
        if (0 == cfg.enable)
            diag_util_mprintf("Normal\n");
        else if (1 == cfg.enable)
            diag_util_mprintf("Finished\n");
        else
            diag_util_mprintf("\n");
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_get_port_ports_all_tx_done_state */

/*
 * pktgen get port ( <PORT_LIST:ports> | all ) frag-pkt action
 */
cparser_result_t
cparser_cmd_pktgen_get_port_ports_all_frag_pkt_action(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32                  unit;
    uint32                  master_view_unit;
    int32                   ret;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    cfg.unit = unit;
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        GETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_FRAG_PKT_ACTION_GET, &cfg,
                rtdrv_ext_pktGenCfg_t, 1);
        diag_util_mprintf("Port %2d : \n", port);
        diag_util_mprintf("\tFragment packet action : ");
        if (0 == cfg.action)
            diag_util_mprintf("Drop\n");
        else if (1 == cfg.action)
            diag_util_mprintf("Trap\n");
        else
            diag_util_mprintf("\n");
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_get_port_ports_all_frag_pkt_action */

/*
 * pktgen set port ( <PORT_LIST:ports> | all ) frag-pkt action ( drop | trap )
 */
cparser_result_t
cparser_cmd_pktgen_set_port_ports_all_frag_pkt_action_drop_trap(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32                  unit;
    uint32                  master_view_unit;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    cfg.unit = unit;
    switch(TOKEN_CHAR(6,0))
    {
        case 'd':
            cfg.action = 0;
            break;
        case 't':
            cfg.action = 1;
            break;
        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        SETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_FRAG_PKT_ACTION_SET, &cfg,
                rtdrv_ext_pktGenCfg_t, 1);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_set_port_ports_all_frag_pkt_action_drop_trap */

/*
 * pktgen get port ( <PORT_LIST:ports> | all ) oversize-pkt action
 */
cparser_result_t
cparser_cmd_pktgen_get_port_ports_all_oversize_pkt_action(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32                  unit;
    uint32                  master_view_unit;
    int32                   ret;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    cfg.unit = unit;
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        GETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_OVERSIZE_PKT_ACTION_GET, &cfg,
                rtdrv_ext_pktGenCfg_t, 1);
        diag_util_mprintf("Port %2d : \n", port);
        diag_util_mprintf("\tOver size packet action : ");
        if (0 == cfg.action)
            diag_util_mprintf("Drop\n");
        else if (1 == cfg.action)
            diag_util_mprintf("Trap\n");
        else
            diag_util_mprintf("\n");
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_get_port_ports_all_oversize_pkt_action */

/*
 * pktgen set port ( <PORT_LIST:ports> | all ) oversize-pkt action ( drop | trap )
 */
cparser_result_t
cparser_cmd_pktgen_set_port_ports_all_oversize_pkt_action_drop_trap(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32                  unit;
    uint32                  master_view_unit;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    cfg.unit = unit;
    switch(TOKEN_CHAR(6,0))
    {
        case 'd':
            cfg.action = 0;
            break;
        case 't':
            cfg.action = 1;
            break;
        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        SETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_OVERSIZE_PKT_ACTION_SET, &cfg,
                rtdrv_ext_pktGenCfg_t, 1);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_set_port_ports_all_oversize_pkt_action_drop_trap */

/*
 * pktgen get port ( <PORT_LIST:ports> | all ) undersize-pkt action
 */
cparser_result_t
cparser_cmd_pktgen_get_port_ports_all_undersize_pkt_action(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32                  unit;
    uint32                  master_view_unit;
    int32                   ret;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    cfg.unit = unit;
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        GETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_UNDERSIZE_PKT_ACTION_GET, &cfg,
                rtdrv_ext_pktGenCfg_t, 1);
        diag_util_mprintf("Port %2d : \n", port);
        diag_util_mprintf("\tUnder size packet action : ");
        if (0 == cfg.action)
            diag_util_mprintf("Drop\n");
        else if (1 == cfg.action)
            diag_util_mprintf("Trap\n");
        else
            diag_util_mprintf("\n");
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_get_port_ports_all_undersize_pkt_action */

/*
 * pktgen set port ( <PORT_LIST:ports> | all ) undersize-pkt action ( drop | trap )
 */
cparser_result_t
cparser_cmd_pktgen_set_port_ports_all_undersize_pkt_action_drop_trap(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32                  unit;
    uint32                  master_view_unit;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    cfg.unit = unit;
    switch(TOKEN_CHAR(6,0))
    {
        case 'd':
            cfg.action = 0;
            break;
        case 't':
            cfg.action = 1;
            break;
        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        SETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_UNDERSIZE_PKT_ACTION_SET, &cfg,
                rtdrv_ext_pktGenCfg_t, 1);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_set_port_ports_all_undersize_pkt_action_drop_trap */

/*
 * pktgen get port ( <PORT_LIST:ports> | all ) bad-crc-pkt action
 */
cparser_result_t
cparser_cmd_pktgen_get_port_ports_all_bad_crc_pkt_action(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32                  unit;
    uint32                  master_view_unit;
    int32                   ret;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    cfg.unit = unit;
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        GETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_BADCRC_PKT_ACTION_GET, &cfg,
                rtdrv_ext_pktGenCfg_t, 1);
        diag_util_mprintf("Port %2d : \n", port);
        diag_util_mprintf("\tBad CRC packet action : ");
        if (0 == cfg.action)
            diag_util_mprintf("Drop\n");
        else if (1 == cfg.action)
            diag_util_mprintf("Trap\n");
        else
            diag_util_mprintf("\n");
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_get_port_ports_all_bad_crc_pkt_action */

/*
 * pktgen set port ( <PORT_LIST:ports> | all ) bad-crc-pkt action ( drop | trap )
 */
cparser_result_t
cparser_cmd_pktgen_set_port_ports_all_bad_crc_pkt_action_drop_trap(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32                  unit;
    uint32                  master_view_unit;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    cfg.unit = unit;
    switch(TOKEN_CHAR(6,0))
    {
        case 'd':
            cfg.action = 0;
            break;
        case 't':
            cfg.action = 1;
            break;
        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        SETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_BADCRC_PKT_ACTION_SET, &cfg,
                rtdrv_ext_pktGenCfg_t, 1);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_set_port_ports_all_bad_crc_pkt_action_drop_trap */

/*
 * pktgen get port ( <PORT_LIST:ports> | all ) tx-pkt-cnt
 */
cparser_result_t
cparser_cmd_pktgen_get_port_ports_all_tx_pkt_cnt(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32                  unit;
    uint32                  master_view_unit;
    int32                   ret;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    cfg.unit = unit;
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        GETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_TX_PKT_CNT_GET, &cfg,
                rtdrv_ext_pktGenCfg_t, 1);
        diag_util_mprintf("Port %2d TX packet counter: %d\n", port, cfg.status);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_get_port_ports_all_tx_pkt_cnt */

/*
 * pktgen set port ( <PORT_LIST:ports> | all ) tx-pkt-cnt <UINT:pktcnt>
 */
cparser_result_t
cparser_cmd_pktgen_set_port_ports_all_tx_pkt_cnt_pktcnt(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *pktcnt_ptr)
{
    uint32                  unit;
    uint32                  master_view_unit;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    cfg.unit = unit;
    cfg.status = *pktcnt_ptr;

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        SETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_TX_PKT_CNT_SET, &cfg,
                rtdrv_ext_pktGenCfg_t, 1);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_set_port_ports_all_tx_pkt_cnt_pktcnt */

#define DIAG_EXT_PKTGEN_STREAM_MAX      2

/*
 * pktgen get port ( <PORT_LIST:ports> | all ) stream <UINT:stream_idx> bad-crc state
 */
cparser_result_t
cparser_cmd_pktgen_get_port_ports_all_stream_stream_idx_bad_crc_state(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *stream_idx_ptr)
{
    uint32                  unit;
    uint32                  master_view_unit;
    int32                   ret;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (*stream_idx_ptr >= DIAG_EXT_PKTGEN_STREAM_MAX)
    {
        diag_util_printf("User stream config: Error!\n");
        return CPARSER_NOT_OK;
    }

    cfg.unit = unit;
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        cfg.stream_idx = *stream_idx_ptr;
        GETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_BADCRC_STATE_GET, &cfg,
                rtdrv_ext_pktGenCfg_t, 1);
        diag_util_mprintf("Port %2d stream %d TX bad CRC state: ",
                port, *stream_idx_ptr);

        if (0 == cfg.status)
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);
        else if (1 == cfg.status)
            diag_util_mprintf("%s\n",DIAG_STR_ENABLE);
        else
            diag_util_mprintf("\n");
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_get_port_ports_all_bad_crc_state */

/*
 * pktgen set port ( <PORT_LIST:ports> | all ) stream <UINT:stream_idx> bad-crc state ( enable | disable )
 */
cparser_result_t
cparser_cmd_pktgen_set_port_ports_all_stream_stream_idx_bad_crc_state_enable_disable(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *stream_idx_ptr)
{
    uint32                  unit;
    uint32                  master_view_unit;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    if (*stream_idx_ptr >= DIAG_EXT_PKTGEN_STREAM_MAX)
    {
        diag_util_printf("User stream config: Error!\n");
        return CPARSER_NOT_OK;
    }

    cfg.unit = unit;
    cfg.stream_idx = *stream_idx_ptr;
    switch(TOKEN_CHAR(8,0))
    {
        case 'd': /*disable*/
            cfg.status = DISABLED;
            break;
        case 'e': /*enable*/
            cfg.status = ENABLED;
            break;
        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        SETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_BADCRC_STATE_SET, &cfg,
                rtdrv_ext_pktGenCfg_t, 1);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_set_port_ports_all_stream_stream_idx_bad_crc_state_enable_disable */

/*
 * pktgen get port ( <PORT_LIST:ports> | all ) stream <UINT:stream_idx> len-type
 */
cparser_result_t
cparser_cmd_pktgen_get_port_ports_all_stream_stream_idx_len_type(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *stream_idx_ptr)
{
    uint32                  unit;
    uint32                  master_view_unit;
    int32                   ret;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (*stream_idx_ptr >= DIAG_EXT_PKTGEN_STREAM_MAX)
    {
        diag_util_printf("User stream config: Error!\n");
        return CPARSER_NOT_OK;
    }

    cfg.unit = unit;
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        cfg.stream_idx = *stream_idx_ptr;
        GETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STREAM_LENTYPE_GET, &cfg,
                rtdrv_ext_pktGenCfg_t, 1);
        diag_util_mprintf("Port %2d stream %d packet length type: ",
                port, *stream_idx_ptr);

        if (0 == cfg.status)
            diag_util_mprintf("Fixed\n");
        else if (1 == cfg.status)
            diag_util_mprintf("Random\n");
        else if (2 == cfg.status)
            diag_util_mprintf("Increamental\n");
        else
            diag_util_mprintf("\n");
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_get_port_ports_all_stream_stream_idx_len_type */

/*
 * pktgen set port ( <PORT_LIST:ports> | all ) stream <UINT:stream_idx> len-type ( fixed | random | increamental )
 */
cparser_result_t
cparser_cmd_pktgen_set_port_ports_all_stream_stream_idx_len_type_fixed_random_increamental(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *stream_idx_ptr)
{
    uint32                  unit;
    uint32                  master_view_unit;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    if (*stream_idx_ptr >= DIAG_EXT_PKTGEN_STREAM_MAX)
    {
        diag_util_printf("User stream config: Error!\n");
        return CPARSER_NOT_OK;
    }

    cfg.unit = unit;
    cfg.stream_idx = *stream_idx_ptr;
    switch(TOKEN_CHAR(7,0))
    {
        case 'f':
            cfg.status = 0;
            break;
        case 'r':
            cfg.status = 1;
            break;
        case 'i':
            cfg.status = 2;
            break;
        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        SETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STREAM_LENTYPE_SET, &cfg,
                rtdrv_ext_pktGenCfg_t, 1);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_set_port_ports_all_stream_stream_idx_len_type_fixed_random_increamental */

/*
 * pktgen get port ( <PORT_LIST:ports> | all ) stream <UINT:stream_idx> random-content state
 */
cparser_result_t
cparser_cmd_pktgen_get_port_ports_all_stream_stream_idx_random_content_state(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *stream_idx_ptr)
{
    uint32                  unit;
    uint32                  master_view_unit;
    int32                   ret;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (*stream_idx_ptr >= DIAG_EXT_PKTGEN_STREAM_MAX)
    {
        diag_util_printf("User stream config: Error!\n");
        return CPARSER_NOT_OK;
    }

    cfg.unit = unit;
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        cfg.stream_idx = *stream_idx_ptr;
        GETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STREAM_RANDOM_CONTENT_STATE_GET, &cfg,
                rtdrv_ext_pktGenCfg_t, 1);
        diag_util_mprintf("Port %2d stream %d random content state: ",
                port, *stream_idx_ptr);

        if (0 == cfg.status)
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);
        else if (1 == cfg.status)
            diag_util_mprintf("%s\n",DIAG_STR_ENABLE);
        else
            diag_util_mprintf("\n");
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_get_port_ports_all_stream_stream_idx_random_content_state */

/*
 * pktgen set port ( <PORT_LIST:ports> | all ) stream <UINT:stream_idx> random-content state ( enable | disable )
 */
cparser_result_t
cparser_cmd_pktgen_set_port_ports_all_stream_stream_idx_random_content_state_enable_disable(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *stream_idx_ptr)
{
    uint32                  unit;
    uint32                  master_view_unit;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    if (*stream_idx_ptr >= DIAG_EXT_PKTGEN_STREAM_MAX)
    {
        diag_util_printf("User stream config: Error!\n");
        return CPARSER_NOT_OK;
    }

    cfg.unit = unit;
    cfg.stream_idx = *stream_idx_ptr;
    switch(TOKEN_CHAR(8,0))
    {
        case 'd': /*disable*/
            cfg.status = DISABLED;
            break;
        case 'e': /*enable*/
            cfg.status = ENABLED;
            break;
        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        SETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STREAM_RANDOM_CONTENT_STATE_SET, &cfg,
                rtdrv_ext_pktGenCfg_t, 1);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_set_port_ports_all_stream_stream_idx_random_content_state_enable_disable */

/*
 * pktgen get port ( <PORT_LIST:ports> | all ) stream <UINT:stream_idx> random-offset
 */
cparser_result_t
cparser_cmd_pktgen_get_port_ports_all_stream_stream_idx_random_offset(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *stream_idx_ptr)
{
    uint32                  unit;
    uint32                  master_view_unit;
    int32                   ret;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (*stream_idx_ptr >= DIAG_EXT_PKTGEN_STREAM_MAX)
    {
        diag_util_printf("User stream config: Error!\n");
        return CPARSER_NOT_OK;
    }

    cfg.unit = unit;
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        cfg.stream_idx = *stream_idx_ptr;
        GETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STREAM_RANDOM_OFFSET_GET, &cfg,
                rtdrv_ext_pktGenCfg_t, 1);
        diag_util_mprintf("Port %2d stream %d random content offset: %d\n",
                port, *stream_idx_ptr, cfg.status);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_get_port_ports_all_stream_stream_idx_random_offset */

/*
 * pktgen set port ( <PORT_LIST:ports> | all ) stream <UINT:stream_idx> random-offset <UINT:offset>
 */
cparser_result_t
cparser_cmd_pktgen_set_port_ports_all_stream_stream_idx_random_offset_offset(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *stream_idx_ptr,
    uint32_t *offset_ptr)
{
    uint32                  unit;
    uint32                  master_view_unit;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    if (*stream_idx_ptr >= DIAG_EXT_PKTGEN_STREAM_MAX)
    {
        diag_util_printf("User stream config: Error!\n");
        return CPARSER_NOT_OK;
    }

    cfg.unit = unit;
    cfg.stream_idx = *stream_idx_ptr;
    cfg.status = *offset_ptr;
    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        SETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STREAM_RANDOM_OFFSET_SET, &cfg,
                rtdrv_ext_pktGenCfg_t, 1);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_set_port_ports_all_stream_stream_idx_random_offset_offset */

/*
 * pktgen get port ( <PORT_LIST:ports> | all ) stream <UINT:stream_idx> sa-inc state
 */
cparser_result_t
cparser_cmd_pktgen_get_port_ports_all_stream_stream_idx_sa_inc_state(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *stream_idx_ptr)
{
    uint32                  unit;
    uint32                  master_view_unit;
    int32                   ret;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (*stream_idx_ptr >= DIAG_EXT_PKTGEN_STREAM_MAX)
    {
        diag_util_printf("User stream config: Error!\n");
        return CPARSER_NOT_OK;
    }

    cfg.unit = unit;
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        cfg.stream_idx = *stream_idx_ptr;
        GETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STREAM_SA_INC_STATE_GET, &cfg,
                rtdrv_ext_pktGenCfg_t, 1);
        diag_util_mprintf("Port %2d stream %d SA increament state: ",
                port, *stream_idx_ptr);

        if (0 == cfg.status)
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);
        else if (1 == cfg.status)
            diag_util_mprintf("%s\n",DIAG_STR_ENABLE);
        else
            diag_util_mprintf("\n");
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_get_port_ports_all_stream_stream_idx_sa_inc_state */

/*
 * pktgen set port ( <PORT_LIST:ports> | all ) stream <UINT:stream_idx> sa-inc state ( enable | disable )
 */
cparser_result_t
cparser_cmd_pktgen_set_port_ports_all_stream_stream_idx_sa_inc_state_enable_disable(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *stream_idx_ptr)
{
    uint32                  unit;
    uint32                  master_view_unit;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    if (*stream_idx_ptr >= DIAG_EXT_PKTGEN_STREAM_MAX)
    {
        diag_util_printf("User stream config: Error!\n");
        return CPARSER_NOT_OK;
    }

    cfg.unit = unit;
    cfg.stream_idx = *stream_idx_ptr;
    switch(TOKEN_CHAR(8,0))
    {
        case 'd': /*disable*/
            cfg.status = DISABLED;
            break;
        case 'e': /*enable*/
            cfg.status = ENABLED;
            break;
        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        SETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STREAM_SA_INC_STATE_SET, &cfg,
                rtdrv_ext_pktGenCfg_t, 1);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_set_port_ports_all_stream_stream_idx_sa_inc_state_enable_disable */

/*
 * pktgen get port ( <PORT_LIST:ports> | all ) stream <UINT:stream_idx> da-inc state
 */
cparser_result_t
cparser_cmd_pktgen_get_port_ports_all_stream_stream_idx_da_inc_state(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *stream_idx_ptr)
{
    uint32                  unit;
    uint32                  master_view_unit;
    int32                   ret;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (*stream_idx_ptr >= DIAG_EXT_PKTGEN_STREAM_MAX)
    {
        diag_util_printf("User stream config: Error!\n");
        return CPARSER_NOT_OK;
    }

    cfg.unit = unit;
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        cfg.stream_idx = *stream_idx_ptr;
        GETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STREAM_DA_INC_STATE_GET, &cfg,
                rtdrv_ext_pktGenCfg_t, 1);
        diag_util_mprintf("Port %2d stream %d DA increament state: ",
                port, *stream_idx_ptr);

        if (0 == cfg.status)
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);
        else if (1 == cfg.status)
            diag_util_mprintf("%s\n",DIAG_STR_ENABLE);
        else
            diag_util_mprintf("\n");
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_get_port_ports_all_stream_stream_idx_da_inc_state */

/*
 * pktgen set port ( <PORT_LIST:ports> | all ) stream <UINT:stream_idx> da-inc state ( enable | disable )
 */
cparser_result_t
cparser_cmd_pktgen_set_port_ports_all_stream_stream_idx_da_inc_state_enable_disable(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *stream_idx_ptr)
{
    uint32                  unit;
    uint32                  master_view_unit;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    if (*stream_idx_ptr >= DIAG_EXT_PKTGEN_STREAM_MAX)
    {
        diag_util_printf("User stream config: Error!\n");
        return CPARSER_NOT_OK;
    }

    cfg.unit = unit;
    cfg.stream_idx = *stream_idx_ptr;
    switch(TOKEN_CHAR(8,0))
    {
        case 'd': /*disable*/
            cfg.status = DISABLED;
            break;
        case 'e': /*enable*/
            cfg.status = ENABLED;
            break;
        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        SETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STREAM_DA_INC_STATE_SET, &cfg,
                rtdrv_ext_pktGenCfg_t, 1);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_set_port_ports_all_stream_stream_idx_da_inc_state_enable_disable */

/*
 * pktgen get port ( <PORT_LIST:ports> | all ) stream <UINT:stream_idx> tx-pkt-cnt
 */
cparser_result_t
cparser_cmd_pktgen_get_port_ports_all_stream_stream_idx_tx_pkt_cnt(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *stream_idx_ptr)
{
    uint32                  unit;
    uint32                  master_view_unit;
    int32                   ret;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (*stream_idx_ptr >= DIAG_EXT_PKTGEN_STREAM_MAX)
    {
        diag_util_printf("User stream config: Error!\n");
        return CPARSER_NOT_OK;
    }

    cfg.unit = unit;
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        cfg.stream_idx = *stream_idx_ptr;
        GETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STREAM_TX_PKT_CNT_GET, &cfg,
                rtdrv_ext_pktGenCfg_t, 1);
        diag_util_mprintf("Port %2d stream %d TX packet counter: %d\n",
                port, *stream_idx_ptr, cfg.status);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_get_port_ports_all_stream_stream_idx_tx_pkt_cnt */

/*
 * pktgen set port ( <PORT_LIST:ports> | all ) stream <UINT:stream_idx> tx-pkt-cnt <UINT:pktcnt>
 */
cparser_result_t
cparser_cmd_pktgen_set_port_ports_all_stream_stream_idx_tx_pkt_cnt_pktcnt(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *stream_idx_ptr,
    uint32_t *pktcnt_ptr)
{
    uint32                  unit;
    uint32                  master_view_unit;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    if (*stream_idx_ptr >= DIAG_EXT_PKTGEN_STREAM_MAX)
    {
        diag_util_printf("User stream config: Error!\n");
        return CPARSER_NOT_OK;
    }

    cfg.unit = unit;
    cfg.stream_idx = *stream_idx_ptr;
    cfg.status = *pktcnt_ptr;

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        SETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STREAM_TX_PKT_CNT_SET, &cfg,
                rtdrv_ext_pktGenCfg_t, 1);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_set_port_ports_all_stream_stream_idx_tx_pkt_cnt_pktcnt */

/*
 * pktgen get port ( <PORT_LIST:ports> | all ) stream <UINT:stream_idx> pkt-len
 */
cparser_result_t
cparser_cmd_pktgen_get_port_ports_all_stream_stream_idx_pkt_len(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *stream_idx_ptr)
{
    uint32                  unit;
    uint32                  master_view_unit;
    rtdrv_ext_pktGenCfg_t   cfg;
    int32                   ret;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (*stream_idx_ptr >= DIAG_EXT_PKTGEN_STREAM_MAX)
    {
        diag_util_printf("User stream config: Error!\n");
        return CPARSER_NOT_OK;
    }

    cfg.unit = unit;
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        cfg.stream_idx = *stream_idx_ptr;
        GETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STREAM_PKT_LEN_GET, &cfg,
                rtdrv_ext_pktGenCfg_t, 1);
        diag_util_mprintf("Port %2d stream %d TX packet length start: %d end: %d\n",
                port, *stream_idx_ptr, cfg.pktlen_start, cfg.pktlen_end);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_get_port_ports_all_stream_stream_idx_pkt_len */

/*
 * pktgen set port ( <PORT_LIST:ports> | all ) stream <UINT:stream_idx> pkt-len start <UINT:start_val> end <UINT:end_val>
 */
cparser_result_t
cparser_cmd_pktgen_set_port_ports_all_stream_stream_idx_pkt_len_start_start_val_end_end_val(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *stream_idx_ptr,
    uint32_t *start_val_ptr,
    uint32_t *end_val_ptr)
{
    uint32                  unit;
    uint32                  master_view_unit;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    if (*stream_idx_ptr >= DIAG_EXT_PKTGEN_STREAM_MAX)
    {
        diag_util_printf("User stream config: Error!\n");
        return CPARSER_NOT_OK;
    }

    cfg.unit = unit;
    cfg.stream_idx = *stream_idx_ptr;
    cfg.pktlen_start = *start_val_ptr;
    cfg.pktlen_end = *end_val_ptr;

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        SETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STREAM_PKT_LEN_SET, &cfg,
                rtdrv_ext_pktGenCfg_t, 1);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_set_port_ports_all_stream_stream_idx_pkt_len_start_start_val_end_end_val */

/*
 * pktgen get port ( <PORT_LIST:ports> | all ) stream <UINT:stream_idx> sa-repeat-cnt
 */
cparser_result_t
cparser_cmd_pktgen_get_port_ports_all_stream_stream_idx_sa_repeat_cnt(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *stream_idx_ptr)
{
    uint32                  unit;
    uint32                  master_view_unit;
    int32                   ret;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (*stream_idx_ptr >= DIAG_EXT_PKTGEN_STREAM_MAX)
    {
        diag_util_printf("User stream config: Error!\n");
        return CPARSER_NOT_OK;
    }

    cfg.unit = unit;
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        cfg.stream_idx = *stream_idx_ptr;
        GETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STREAM_SA_REPEAT_CNT_GET, &cfg,
                rtdrv_ext_pktGenCfg_t, 1);
        diag_util_mprintf("Port %2d stream %d SA repeat counter: %d\n",
                port, *stream_idx_ptr, cfg.status);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_get_port_ports_all_stream_stream_idx_sa_repeat_cnt */

/*
 * pktgen set port ( <PORT_LIST:ports> | all ) stream <UINT:stream_idx> sa-repeat-cnt <UINT:repeat_cnt>
 */
cparser_result_t
cparser_cmd_pktgen_set_port_ports_all_stream_stream_idx_sa_repeat_cnt_repeat_cnt(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *stream_idx_ptr,
    uint32_t *repeat_cnt_ptr)
{
    uint32                  unit;
    uint32                  master_view_unit;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    if (*stream_idx_ptr >= DIAG_EXT_PKTGEN_STREAM_MAX)
    {
        diag_util_printf("User stream config: Error!\n");
        return CPARSER_NOT_OK;
    }

    cfg.unit = unit;
    cfg.stream_idx = *stream_idx_ptr;
    cfg.status = *repeat_cnt_ptr;

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        SETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STREAM_SA_REPEAT_CNT_SET, &cfg,
                rtdrv_ext_pktGenCfg_t, 1);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_set_port_ports_all_stream_stream_idx_sa_repeat_cnt_repeat_cnt */

/*
 * pktgen get port ( <PORT_LIST:ports> | all ) stream <UINT:stream_idx> da-repeat-cnt
 */
cparser_result_t
cparser_cmd_pktgen_get_port_ports_all_stream_stream_idx_da_repeat_cnt(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *stream_idx_ptr)
{
    uint32                  unit;
    uint32                  master_view_unit;
    rtdrv_ext_pktGenCfg_t   cfg;
    int32                   ret;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (*stream_idx_ptr >= DIAG_EXT_PKTGEN_STREAM_MAX)
    {
        diag_util_printf("User stream config: Error!\n");
        return CPARSER_NOT_OK;
    }

    cfg.unit = unit;
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        cfg.stream_idx = *stream_idx_ptr;
        GETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STREAM_DA_REPEAT_CNT_GET, &cfg,
                rtdrv_ext_pktGenCfg_t, 1);
        diag_util_mprintf("Port %2d stream %d DA repeat counter: %d\n",
                port, *stream_idx_ptr, cfg.status);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_get_port_ports_all_stream_stream_idx_da_repeat_cnt */

/*
 * pktgen set port ( <PORT_LIST:ports> | all ) stream <UINT:stream_idx> da-repeat-cnt <UINT:repeat_cnt>
 */
cparser_result_t
cparser_cmd_pktgen_set_port_ports_all_stream_stream_idx_da_repeat_cnt_repeat_cnt(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *stream_idx_ptr,
    uint32_t *repeat_cnt_ptr)
{
    uint32                  unit;
    uint32                  master_view_unit;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    if (*stream_idx_ptr >= DIAG_EXT_PKTGEN_STREAM_MAX)
    {
        diag_util_printf("User stream config: Error!\n");
        return CPARSER_NOT_OK;
    }

    cfg.unit = unit;
    cfg.stream_idx = *stream_idx_ptr;
    cfg.status = *repeat_cnt_ptr;

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        SETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STREAM_DA_REPEAT_CNT_SET, &cfg,
                rtdrv_ext_pktGenCfg_t, 1);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_set_port_ports_all_stream_stream_idx_da_repeat_cnt_repeat_cnt */

/*
 * pktgen set port ( <PORT_LIST:ports> | all ) stream <UINT:stream_idx> field sa <MACADDR:mac>
 */
cparser_result_t
cparser_cmd_pktgen_set_port_ports_all_stream_stream_idx_field_sa_mac(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *stream_idx_ptr,
    cparser_macaddr_t *mac_ptr)
{
    uint32                  unit;
    uint32                  master_view_unit;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    if (*stream_idx_ptr >= DIAG_EXT_PKTGEN_STREAM_MAX)
    {
        diag_util_printf("User stream config: Error!\n");
        return CPARSER_NOT_OK;
    }

    cfg.unit = unit;
    cfg.stream_idx = *stream_idx_ptr;
    osal_memcpy(&cfg.sa.octet, mac_ptr->octet, ETHER_ADDR_LEN);

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        SETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STREAM_FIELD_SA_SET, &cfg,
                rtdrv_ext_pktGenCfg_t, 1);
    }

    return CPARSER_OK;
}

/*
 * pktgen set port ( <PORT_LIST:ports> | all ) stream <UINT:stream_idx> field da <MACADDR:mac>
 */
cparser_result_t
cparser_cmd_pktgen_set_port_ports_all_stream_stream_idx_field_da_mac(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *stream_idx_ptr,
    cparser_macaddr_t *mac_ptr)
{
    uint32                  unit;
    uint32                  master_view_unit;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    if (*stream_idx_ptr >= DIAG_EXT_PKTGEN_STREAM_MAX)
    {
        diag_util_printf("User stream config: Error!\n");
        return CPARSER_NOT_OK;
    }

    cfg.unit = unit;
    cfg.stream_idx = *stream_idx_ptr;
    osal_memcpy(&cfg.da.octet, mac_ptr->octet, ETHER_ADDR_LEN);

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        SETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STREAM_FIELD_DA_SET, &cfg, rtdrv_ext_pktGenCfg_t, 1);
    }

    return CPARSER_OK;
}

/*
 * pktgen set port ( <PORT_LIST:ports> | all ) stream <UINT:stream_idx> field ether-type <UINT:ether_type>
 */
cparser_result_t
cparser_cmd_pktgen_set_port_ports_all_stream_stream_idx_field_ether_type_ether_type(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *stream_idx_ptr,
    uint32_t *ether_type)
{
    uint32                  unit;
    uint32                  master_view_unit;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    if (*stream_idx_ptr >= DIAG_EXT_PKTGEN_STREAM_MAX)
    {
        diag_util_printf("User stream config: Error!\n");
        return CPARSER_NOT_OK;
    }

    cfg.unit = unit;
    cfg.stream_idx = *stream_idx_ptr;
    cfg.etherType = (uint16)*ether_type;

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        SETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STREAM_FIELD_ETHTYPE_SET, &cfg, rtdrv_ext_pktGenCfg_t, 1);
    }

    return CPARSER_OK;
}

/*
 * pktgen set port ( <PORT_LIST:ports> | all ) stream <UINT:stream_idx> field vlan-header <UINT:vlan_header>
 */
cparser_result_t
cparser_cmd_pktgen_set_port_ports_all_stream_stream_idx_field_vlan_header_vlan_header(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *stream_idx_ptr,
    uint32_t *vlan_header)
{
    uint32                  unit;
    uint32                  master_view_unit;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    if (*stream_idx_ptr >= DIAG_EXT_PKTGEN_STREAM_MAX)
    {
        diag_util_printf("User stream config: Error!\n");
        return CPARSER_NOT_OK;
    }

    cfg.unit = unit;
    cfg.stream_idx = *stream_idx_ptr;
    cfg.vlanHdr = *vlan_header;

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        SETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STREAM_FIELD_OTAG_SET, &cfg, rtdrv_ext_pktGenCfg_t, 1);
    }

    return CPARSER_OK;
}

/*
 * pktgen set port ( <PORT_LIST:ports> | all ) stream <UINT:stream_idx> field vlan-header state ( enable | disable )
 */
cparser_result_t
cparser_cmd_pktgen_set_port_ports_all_stream_stream_idx_field_vlan_header_state_enable_disable(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *stream_idx_ptr)
{
    uint32                  unit;
    uint32                  master_view_unit;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    if (*stream_idx_ptr >= DIAG_EXT_PKTGEN_STREAM_MAX)
    {
        diag_util_printf("User stream config: Error!\n");
        return CPARSER_NOT_OK;
    }

    cfg.unit = unit;
    cfg.stream_idx = *stream_idx_ptr;
    switch(TOKEN_CHAR(9,0))
    {
        case 'd': /*disable*/
            cfg.status = DISABLED;
            break;
        case 'e': /*enable*/
            cfg.status = ENABLED;
            break;
        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        SETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STREAM_FIELD_OTAG_STATE_SET, &cfg, rtdrv_ext_pktGenCfg_t, 1);
    }

    return CPARSER_OK;
}

/*
 * pktgen set port ( <PORT_LIST:ports> | all ) stream <UINT:stream_idx> field payload-type ( fix-pattern | incr | repeat-pattern | zero )
 */
cparser_result_t
cparser_cmd_pktgen_set_port_ports_all_stream_stream_idx_field_payload_type_fix_pattern_incr_repeat_pattern_zero(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *stream_idx_ptr)
{
    uint32                  unit;
    uint32                  master_view_unit;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    if (*stream_idx_ptr >= DIAG_EXT_PKTGEN_STREAM_MAX)
    {
        diag_util_printf("User stream config: Error!\n");
        return CPARSER_NOT_OK;
    }

    cfg.unit = unit;
    cfg.stream_idx = *stream_idx_ptr;
    cfg.pattern = 0;
    switch(TOKEN_CHAR(8,0))
    {
        case 'z': /*zero*/
            cfg.patternType = RTDRV_EXT_SPG_PAYLOAD_ZERO;
            break;
        case 'i': /*increment*/
            cfg.patternType = RTDRV_EXT_SPG_PAYLOAD_INCR;
            break;
        case 'f': /*fix*/
            cfg.patternType = RTDRV_EXT_SPG_PAYLOAD_FIX;
            break;
        case 'r': /*repeat*/
            cfg.patternType = RTDRV_EXT_SPG_PAYLOAD_REPEAT;
            break;
        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        SETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STREAM_FIELD_PAYLOAD_TYPE_SET, &cfg, rtdrv_ext_pktGenCfg_t, 1);
    }

    return CPARSER_OK;
}

/*
 * pktgen set port ( <PORT_LIST:ports> | all ) stream <UINT:stream_idx> field payload-pattern <UINT:pattern>
 */
cparser_result_t
cparser_cmd_pktgen_set_port_ports_all_stream_stream_idx_field_payload_pattern_pattern(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *stream_idx_ptr,
    uint32_t *pattern_ptr)
{
    uint32                  unit;
    uint32                  master_view_unit;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    if (*stream_idx_ptr >= DIAG_EXT_PKTGEN_STREAM_MAX)
    {
        diag_util_printf("User stream config: Error!\n");
        return CPARSER_NOT_OK;
    }

    cfg.unit = unit;
    cfg.stream_idx = *stream_idx_ptr;
    cfg.pattern = *pattern_ptr;

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        SETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STREAM_FIELD_PAYLOAD_PATTERN_SET, &cfg, rtdrv_ext_pktGenCfg_t, 1);
    }

    return CPARSER_OK;
}

/*
 * pktgen set port ( <PORT_LIST:ports> | all ) tx
 */
cparser_result_t
cparser_cmd_pktgen_set_port_ports_all_tx(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32                  unit;
    uint32                  master_view_unit;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    cfg.unit = unit;

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        SETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_TX, &cfg, rtdrv_ext_pktGenCfg_t, 1);
    }

    return CPARSER_OK;
}

/*
 * pktgen set port ( <PORT_LIST:ports> | all ) tx dying-gasp { <UINT:len> }
 */
cparser_result_t
cparser_cmd_pktgen_set_port_ports_all_tx_dying_gasp_len(
    cparser_context_t *context, char **ports_ptr, uint32_t *len)
{
    uint32                  unit;
    uint32                  master_view_unit;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;
    uint32                  length;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    cfg.unit = unit;

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    if (6 == TOKEN_NUM)
        length = 1518;
    else
        length = *len;

    if (length < 64 || length > 1518)
        return CPARSER_NOT_OK;


    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        cfg.len = length;
        SETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_TX_DYING_GASP, &cfg, rtdrv_ext_pktGenCfg_t, 1);
    }

    return CPARSER_OK;
}

/*
 * pktgen dump port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_pktgen_dump_port_ports_all(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32                  unit, i;
    uint32  master_view_unit;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    cfg.unit = unit;

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        diag_util_mprintf("Port %2d :\n", port);

        GETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STATE_GET, &cfg, rtdrv_ext_pktGenCfg_t, 1);
        diag_util_mprintf("\tTX state : ");
        if (0 == cfg.enable)
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);
        else if (1 == cfg.enable)
            diag_util_mprintf("%s\n",DIAG_STR_ENABLE);
        else
            diag_util_mprintf("\n");

        GETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_FRAG_PKT_ACTION_GET, &cfg, rtdrv_ext_pktGenCfg_t, 1);
        diag_util_mprintf("\tFragment packet action : ");
        if (0 == cfg.action)
            diag_util_mprintf("Drop\n");
        else if (1 == cfg.action)
            diag_util_mprintf("Trap\n");
        else
            diag_util_mprintf("\n");

        GETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_OVERSIZE_PKT_ACTION_GET, &cfg, rtdrv_ext_pktGenCfg_t, 1);
        diag_util_mprintf("\tOver size packet action : ");
        if (0 == cfg.action)
            diag_util_mprintf("Drop\n");
        else if (1 == cfg.action)
            diag_util_mprintf("Trap\n");
        else
            diag_util_mprintf("\n");

        GETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_UNDERSIZE_PKT_ACTION_GET, &cfg, rtdrv_ext_pktGenCfg_t, 1);
        diag_util_mprintf("\tUnder size packet action : ");
        if (0 == cfg.action)
            diag_util_mprintf("Drop\n");
        else if (1 == cfg.action)
            diag_util_mprintf("Trap\n");
        else
            diag_util_mprintf("\n");

        GETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_BADCRC_PKT_ACTION_GET, &cfg, rtdrv_ext_pktGenCfg_t, 1);
        diag_util_mprintf("\tBad CRC packet action : ");
        if (0 == cfg.action)
            diag_util_mprintf("Drop\n");
        else if (1 == cfg.action)
            diag_util_mprintf("Trap\n");
        else
            diag_util_mprintf("\n");

        GETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_TX_PKT_CNT_GET, &cfg, rtdrv_ext_pktGenCfg_t, 1);
        diag_util_mprintf("\tTX packet counter: %d\n", cfg.status);


        for (i = 0; i < 2; i++)
        {
            cfg.stream_idx = i;
            GETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_BADCRC_STATE_GET, &cfg, rtdrv_ext_pktGenCfg_t, 1);
            diag_util_mprintf("\tstream %d TX bad CRC state: ", i);
            if (0 == cfg.status)
                diag_util_mprintf("%s\n", DIAG_STR_DISABLE);
            else if (1 == cfg.status)
                diag_util_mprintf("%s\n",DIAG_STR_ENABLE);
            else
                diag_util_mprintf("\n");
        }
        for (i = 0; i < 2; i++)
        {
            cfg.stream_idx = i;
            GETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STREAM_LENTYPE_GET, &cfg, rtdrv_ext_pktGenCfg_t, 1);
            diag_util_mprintf("\tstream %d packet length type: ", i);

            if (0 == cfg.status)
                diag_util_mprintf("Fixed\n");
            else if (1 == cfg.status)
                diag_util_mprintf("Random\n");
            else if (2 == cfg.status)
                diag_util_mprintf("Increamental\n");
            else
                diag_util_mprintf("\n");
        }
        for (i = 0; i < 2; i++)
        {
            cfg.stream_idx = i;
            GETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STREAM_RANDOM_CONTENT_STATE_GET, &cfg,
                    rtdrv_ext_pktGenCfg_t, 1);
            diag_util_mprintf("\tstream %d random content state: ", i);

            if (0 == cfg.status)
                diag_util_mprintf("%s\n", DIAG_STR_DISABLE);
            else if (1 == cfg.status)
                diag_util_mprintf("%s\n",DIAG_STR_ENABLE);
            else
                diag_util_mprintf("\n");
        }
        for (i = 0; i < 2; i++)
        {
            cfg.stream_idx = i;
            GETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STREAM_RANDOM_OFFSET_GET, &cfg, rtdrv_ext_pktGenCfg_t, 1);
            diag_util_mprintf("\tstream %d random content offset: %d\n", i, cfg.status);
        }
        for (i = 0; i < 2; i++)
        {
            cfg.stream_idx = i;
            GETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STREAM_SA_INC_STATE_GET, &cfg, rtdrv_ext_pktGenCfg_t, 1);
            diag_util_mprintf("\tstream %d SA increament state: ", i);

            if (0 == cfg.status)
                diag_util_mprintf("%s\n", DIAG_STR_DISABLE);
            else if (1 == cfg.status)
                diag_util_mprintf("%s\n",DIAG_STR_ENABLE);
            else
                diag_util_mprintf("\n");
        }
        for (i = 0; i < 2; i++)
        {
            cfg.stream_idx = i;
            GETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STREAM_DA_INC_STATE_GET, &cfg, rtdrv_ext_pktGenCfg_t, 1);
            diag_util_mprintf("\tstream %d DA increament state: ", i);

            if (0 == cfg.status)
                diag_util_mprintf("%s\n", DIAG_STR_DISABLE);
            else if (1 == cfg.status)
                diag_util_mprintf("%s\n",DIAG_STR_ENABLE);
            else
                diag_util_mprintf("\n");
        }
        for (i = 0; i < 2; i++)
        {
            cfg.stream_idx = i;
            GETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STREAM_TX_PKT_CNT_GET, &cfg, rtdrv_ext_pktGenCfg_t, 1);
            diag_util_mprintf("\tstream %d TX packet counter: %d\n", i, cfg.status);
        }
        for (i = 0; i < 2; i++)
        {
            cfg.stream_idx = i;
            GETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STREAM_PKT_LEN_GET, &cfg, rtdrv_ext_pktGenCfg_t, 1);
            diag_util_mprintf("\tstream %d TX packet length start: %d end: %d\n", i, cfg.pktlen_start, cfg.pktlen_end);
        }
        for (i = 0; i < 2; i++)
        {
            cfg.stream_idx = i;
            GETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STREAM_SA_REPEAT_CNT_GET, &cfg, rtdrv_ext_pktGenCfg_t, 1);
            diag_util_mprintf("\tstream %d SA repeat counter: %d\n", i, cfg.status);
        }
        for (i = 0; i < 2; i++)
        {
            cfg.stream_idx = i;
            GETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STREAM_DA_REPEAT_CNT_GET, &cfg, rtdrv_ext_pktGenCfg_t, 1);
            diag_util_mprintf("\tstream %d DA repeat counter: %d\n", i, cfg.status);
        }
        diag_util_mprintf("\n");
    }

    return CPARSER_OK;
}

/* l2-table add mac-ucast <UINT:vid> <MACADDR:mac> devID <UINT:devID> ( port | trunk ) <UINT:id> tagif <UINT:tagif> agg_vid <UINT:agg_vid> agg_pri <UINT:agg_pri> { sa-block } { da-block } { static } { nexthop } { suspend } { aged } */
cparser_result_t cparser_cmd_l2_table_add_mac_ucast_vid_mac_devID_devID_port_trunk_id_tagif_tagif_agg_vid_agg_vid_agg_pri_agg_pri_sa_block_da_block_static_nexthop_suspend_aged(cparser_context_t *context,
    uint32_t *vid_ptr,
    cparser_macaddr_t *mac_ptr,
    uint32_t *unit_ptr,
    uint32_t *id_ptr,
    uint32_t *tagif_ptr,
    uint32_t *aggVid_ptr,
    uint32_t *aggPri_ptr)
{
    int32           flag_num = 0;
    uint32          sa_block = FALSE;
    uint32          da_block = FALSE;
    uint32          is_static = FALSE;
    uint32          nexthop = FALSE;
    uint32          suspend = FALSE;
    uint32          aged = FALSE;
    uint32          unit = 0;
    uint32          master_view_unit;
    int32           ret = RT_ERR_FAILED;
    rtk_l2_ucastAddr_t  l2_uAddr;

    DIAG_UTIL_PARAM_CHK();
    RT_PARAM_CHK((NULL == vid_ptr), CPARSER_ERR_INVALID_PARAMS);
    RT_PARAM_CHK((NULL == mac_ptr), CPARSER_ERR_INVALID_PARAMS);
    RT_PARAM_CHK((NULL == id_ptr), CPARSER_ERR_INVALID_PARAMS);
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&l2_uAddr, 0, sizeof(rtk_l2_ucastAddr_t));

    if ((TRUE == diag_util_isBcastMacAddr(mac_ptr->octet)) || (TRUE == diag_util_isMcastMacAddr(mac_ptr->octet)))
    {
        diag_util_printf("Broadcast and Multicast MAC address is not allowed to configure.\n");
        return CPARSER_NOT_OK;
    }

    /*from first optional token*/
    for(flag_num = 15; flag_num < TOKEN_NUM; flag_num++)
    {
        if ('s' == TOKEN_CHAR(flag_num, 0))
        {
            if ('a' == TOKEN_CHAR(flag_num, 1))
            {
                sa_block = TRUE;
            }
            else if ('t' == TOKEN_CHAR(flag_num, 1))
            {
                is_static = TRUE;
            }
            else if ('u' == TOKEN_CHAR(flag_num, 1))
            {
                suspend = TRUE;
            }
            else
            {
                diag_util_printf("User config: Error!\n");
                return CPARSER_NOT_OK;
            }
        }
        else if ('d' == TOKEN_CHAR(flag_num, 0))
        {
            da_block = TRUE;
        }
        else if ('n' == TOKEN_CHAR(flag_num, 0))
        {
            nexthop = TRUE;
        }
        else if ('a' == TOKEN_CHAR(flag_num, 0))
        {
            aged = TRUE;
        }
        else
        {
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
        }
    }


    /* Fill structure */
    l2_uAddr.vid        = *vid_ptr;
    l2_uAddr.devID      = *unit_ptr;
    if (*tagif_ptr)
        l2_uAddr.flags |= RTK_L2_UCAST_FLAG_TAG_STS;
    l2_uAddr.agg_vid = *aggVid_ptr;
    l2_uAddr.agg_pri = *aggPri_ptr;
    osal_memcpy(l2_uAddr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);

    if ('p' == TOKEN_CHAR(7, 0))
    {
        l2_uAddr.port = *id_ptr;
    }
    else if ('t' == TOKEN_CHAR(7, 0))
    {
        l2_uAddr.flags |= RTK_L2_UCAST_FLAG_TRUNK_PORT;
        l2_uAddr.trk_gid = *id_ptr;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    if(sa_block)
        l2_uAddr.flags |= RTK_L2_UCAST_FLAG_SA_BLOCK;

    if(da_block)
        l2_uAddr.flags |= RTK_L2_UCAST_FLAG_DA_BLOCK;

    if(is_static)
        l2_uAddr.flags |= RTK_L2_UCAST_FLAG_STATIC;

    if(nexthop)
        l2_uAddr.flags |= RTK_L2_UCAST_FLAG_NEXTHOP;

    if(suspend)
        l2_uAddr.state |= RTK_L2_UCAST_STATE_SUSPEND;

    if(aged)
    {
        l2_uAddr.isAged = 1;

        if(DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
            l2_uAddr.port = 0x1f;
        if(DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID))
            l2_uAddr.port = 0x3f;

        l2_uAddr.flags &= ~RTK_L2_UCAST_FLAG_TRUNK_PORT;
    }


    DIAG_UTIL_ERR_CHK(rtk_l2_addr_add(unit, &l2_uAddr), ret);

    return CPARSER_OK;
}

/* l2-table get aging-unit */
cparser_result_t cparser_cmd_l2_table_get_aging_unit(cparser_context_t *context)
{
    rtdrv_ext_l2Cfg_t   l2_cfg;
    uint32 master_view_unit;

    DIAG_OM_GET_UNIT_ID(l2_cfg.unit);
    master_view_unit = l2_cfg.unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &l2_cfg.unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("\tAging Time                                     : ");

    GETSOCKOPT(RTDRV_EXT_L2_AGING_UNIT_GET, &l2_cfg, rtdrv_ext_l2Cfg_t, 1);

    diag_util_mprintf("%d (0.1 seconds).\n", l2_cfg.aging_time);

    return CPARSER_OK;
}

/* l2-table set aging-unit <UINT:time> */
cparser_result_t cparser_cmd_l2_table_set_aging_unit_time(cparser_context_t *context,
    uint32_t *time_ptr)
{
    rtdrv_ext_l2Cfg_t   l2_cfg;
    uint32 master_view_unit;

    DIAG_OM_GET_UNIT_ID(l2_cfg.unit);
    master_view_unit = l2_cfg.unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &l2_cfg.unit);
    DIAG_UTIL_PARAM_CHK();
    l2_cfg.aging_time = *time_ptr;

    SETSOCKOPT(RTDRV_EXT_L2_AGING_UNIT_SET, &l2_cfg, rtdrv_ext_l2Cfg_t, 1);

    return CPARSER_OK;
}

/* l2-table clear table */
cparser_result_t cparser_cmd_l2_table_clear_table(cparser_context_t *context)
{
    rtdrv_ext_l2Cfg_t   l2_cfg;
    uint32 master_view_unit;

    DIAG_OM_GET_UNIT_ID(l2_cfg.unit);
    master_view_unit = l2_cfg.unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &l2_cfg.unit);
    DIAG_UTIL_PARAM_CHK();

    SETSOCKOPT(RTDRV_EXT_L2_TBL_CLEAR, &l2_cfg, rtdrv_ext_l2Cfg_t, 1);

    return CPARSER_OK;
}

/* l2-table get cam state */
cparser_result_t cparser_cmd_l2_table_get_cam_state(cparser_context_t *context)
{
    rtdrv_ext_l2Cfg_t   l2_cfg;
    uint32 master_view_unit;

    DIAG_OM_GET_UNIT_ID(l2_cfg.unit);
    master_view_unit = l2_cfg.unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &l2_cfg.unit);
    DIAG_UTIL_PARAM_CHK();

    GETSOCKOPT(RTDRV_EXT_L2_CMA_ENABLE_GET, &l2_cfg, rtdrv_ext_l2Cfg_t, 1);

    diag_util_mprintf("L2 table cam: %s\n", l2_cfg.enable ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);

    return CPARSER_OK;
}

/* l2-table set cam state ( enable | disable ) */
cparser_result_t cparser_cmd_l2_table_set_cam_state_enable_disable(cparser_context_t *context)
{
    rtdrv_ext_l2Cfg_t   l2_cfg;
    uint32 master_view_unit;

    DIAG_OM_GET_UNIT_ID(l2_cfg.unit);
    master_view_unit = l2_cfg.unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &l2_cfg.unit);
    DIAG_UTIL_PARAM_CHK();

    switch(TOKEN_CHAR(4,0))
    {
        case 'd': /*disable*/
            l2_cfg.enable = DISABLED;
            break;
        case 'e': /*enable*/
            l2_cfg.enable = ENABLED;
            break;
        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    SETSOCKOPT(RTDRV_EXT_L2_CMA_ENABLE_SET, &l2_cfg, rtdrv_ext_l2Cfg_t, 1);

    return CPARSER_OK;
}

/* acl get meter counter */
cparser_result_t cparser_cmd_acl_get_meter_counter(cparser_context_t *context)
{
    rtdrv_ext_aclCfg_t   acl_cfg;
    uint32  master_view_unit;

    DIAG_OM_GET_UNIT_ID(acl_cfg.unit);
    master_view_unit = acl_cfg.unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &acl_cfg.unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    GETSOCKOPT(RTDRV_EXT_ACL_METER_COUNTER_GET, &acl_cfg, rtdrv_ext_aclCfg_t, 1);
    diag_util_printf("Counter of Meter %u unit ", acl_cfg.index);
    switch(acl_cfg.counterUnit)
    {
        case 0:
            diag_util_mprintf("1Byte\n");
            break;
        case 1:
            diag_util_mprintf("16Byte\n");
            break;
        case 2:
            diag_util_mprintf("Packet\n");
            break;
    }

    diag_util_mprintf("GreenCounter %d\n", acl_cfg.greenCounter);
    diag_util_mprintf("YellowCounter %d\n", acl_cfg.yellowCounter);
    diag_util_mprintf("RedCounter %d\n", acl_cfg.redCounter);
    diag_util_mprintf("TotalCounter %d\n", acl_cfg.totalCounter);

    return CPARSER_OK;
}

/* acl set meter counter ( 1byte | 16byte | packet ) <UINT:index> */
cparser_result_t cparser_cmd_acl_set_meter_counter_1byte_16byte_packet_index(cparser_context_t *context,
    uint32_t *index_ptr)
{
    rtdrv_ext_aclCfg_t   acl_cfg;
    uint32  master_view_unit;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(acl_cfg.unit);
    master_view_unit = acl_cfg.unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &acl_cfg.unit);
    switch(TOKEN_CHAR(4,1))
    {
        case 'b':
            acl_cfg.counterUnit = 0;
            break;
        case '6':
            acl_cfg.counterUnit = 1;
            break;
        case 'a':
            acl_cfg.counterUnit = 2;
            break;
        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    acl_cfg.index = *index_ptr;
    acl_cfg.clear = 1;

    SETSOCKOPT(RTDRV_EXT_ACL_METER_COUNTER_SET, &acl_cfg, rtdrv_ext_aclCfg_t, 1);

    return CPARSER_OK;
}

/* acl reset meter counter */
cparser_result_t cparser_cmd_acl_reset_meter_counter(cparser_context_t *context)
{
    rtdrv_ext_aclCfg_t   acl_cfg;
    uint32  master_view_unit;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(acl_cfg.unit);
    master_view_unit = acl_cfg.unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &acl_cfg.unit);
    acl_cfg.clear = 1;
    SETSOCKOPT(RTDRV_EXT_ACL_METER_COUNTER_RESET, &acl_cfg, rtdrv_ext_aclCfg_t, 1);

    return CPARSER_OK;
}

/* time get ptp ( <PORT_LIST:ports> | all ) timestamp ( rx-sync | rx-dreq | rx-pdrq | rx-pdrp | tx-sync | tx-dreq | tx-pdrq | tx-pdrp ) */
cparser_result_t cparser_cmd_time_get_ptp_ports_all_timestamp_rx_sync_rx_dreq_rx_pdrq_rx_pdrp_tx_sync_tx_dreq_tx_pdrq_tx_pdrp(cparser_context_t *context,
    char **ports_ptr)
{
    uint32_t            unit;
    uint32              master_view_unit;
    rtk_port_t          port;
    diag_portlist_t     portlist;
    rtdrv_ext_timeCfg_t time_cfg;
    char                ts_str[8][8] = { "RX-SYNC", "RX-DREQ", "RX-PDRQ", "RX-PDRP", "TX-SYNC", "TX-DREQ", "TX-PDRQ", "TX-PDRP" };

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    time_cfg.unit = unit;
    /* Type of Timestamp */
    if ('r' == TOKEN_CHAR(5, 0))
    {   /* Rx */
        time_cfg.type = 0;
    }
    else
    {   /* Tx */
        time_cfg.type = 4;
    }

    if ('s' == TOKEN_CHAR(5, 3))
    {   /* Sync */
        time_cfg.type += 0;
    }
    else if ('d' == TOKEN_CHAR(5, 3))
    {   /* Delay Req */
        time_cfg.type += 1;
    }
    else if ('q' == TOKEN_CHAR(5, 6))
    {   /* Pdelay Req */
        time_cfg.type += 2;
    }
    else
    {   /* Pdelay Resp */
        time_cfg.type += 3;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        time_cfg.port = port;
        /* clear */
        time_cfg.sid = 0;
        time_cfg.sec = 0;
        time_cfg.nsec = 0;
        GETSOCKOPT(RTDRV_EXT_TIME_PORT_PTP_TIMESTAMP_GET, &time_cfg, rtdrv_ext_timeCfg_t, 1);
        diag_util_mprintf("\tPort: %2d\n", port);
        diag_util_mprintf("\t\t%s: SID = %5d, TS = %10u.%09u [0x%04X, 0x%08X, 0x%08X]\n",
            ts_str[time_cfg.type], time_cfg.sid, time_cfg.sec, time_cfg.nsec,
            time_cfg.sid, time_cfg.sec, time_cfg.nsec);
    }

    return CPARSER_OK;
}

/* nic set pkt-send src-mac <MACADDR:src_mac> dst-mac <MACADDR:dst_mac> unit <UNIT:unit> port ( <PORT_LIST:ports> | all ) { cpu-tag } { trunk-hash }*/
cparser_result_t cparser_cmd_nic_set_pkt_send_src_mac_src_mac_dst_mac_dst_mac_unit_unit_port_ports_all_cpu_tag_trunk_hash(cparser_context_t *context,
    cparser_macaddr_t *src_mac_ptr,
    cparser_macaddr_t *dst_mac_ptr,
    uint32_t *unit_ptr,
    char **ports_ptr)
{
    uint32_t    unit;
    uint32      master_view_unit;
    int32  ret = RT_ERR_FAILED;
    uint32_t    i;
    diag_portlist_t     portlist;
    rtdrv_ext_nicSendCfg_t nicSendCfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    nicSendCfg.unit = unit;
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&nicSendCfg, 0, sizeof(nicSendCfg));
    osal_memset(&portlist, 0, sizeof(portlist));

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 8) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    nicSendCfg.isCpuTag = FALSE;
    nicSendCfg.isTrunkHash = FALSE;

    for (i = 9; i < TOKEN_NUM; i++)
    {
        if ('c' == TOKEN_CHAR(i,0))
        {
            nicSendCfg.isCpuTag = TRUE;
        }

        if ('t' == TOKEN_CHAR(i,0))
            nicSendCfg.isTrunkHash = TRUE;
    }

    DIAG_UTIL_ERR_CHK(diag_util_str2mac(nicSendCfg.src_mac.octet, (char *)TOKEN_STR(4)), ret);
    DIAG_UTIL_ERR_CHK(diag_util_str2mac(nicSendCfg.dst_mac.octet, (char *)TOKEN_STR(6)), ret);
    osal_memcpy(&nicSendCfg.txPortmask, &portlist.portmask, sizeof(portlist.portmask));

    SETSOCKOPT(RTDRV_EXT_NIC_PKT_SEND_SET, &nicSendCfg, rtdrv_ext_nicSendCfg_t, 1);

    return CPARSER_OK;
}

/*
 *  l2-table dump mac-ucast cam
 */
cparser_result_t
cparser_cmd_l2_table_dump_mac_ucast_cam(cparser_context_t *context)
{
    uint32             scan_idx = 0;
    uint32             total_entry = 0;
    uint32             unit = 0;
    uint32 master_view_unit;
    int32              ret = RT_ERR_FAILED;
    rtk_l2_ucastAddr_t l2_data;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&l2_data, 0, sizeof(rtk_l2_ucastAddr_t));

    diag_util_mprintf("Index | MAC Address       | VID  | SPA  | SaBlock  | DaBlock  | Static  | Nexthop  | Suspend | Aggreate VID\n");
    diag_util_mprintf("------+-------------------+------+------+----------+----------+---------+----------+---------+-------------\n");


    rtdrv_ext_l2Cfg_t   l2_cfg;

    DIAG_OM_GET_UNIT_ID(l2_cfg.unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    GETSOCKOPT(RTDRV_EXT_L2_UC_SIZE_GET, &l2_cfg, rtdrv_ext_l2Cfg_t, 1);

    /* show all l2 cam table */
    scan_idx = l2_cfg.data - 1; /* get the first entry */
    while (1)
    {
        if ((ret = rtk_l2_nextValidAddr_get(unit, (int32 *)&scan_idx, 0, &l2_data)) != RT_ERR_OK)
        {
            //diag_util_mprintf("Warning:%d \n", ret);
            break;
        }

        diag_util_mprintf("%5d | %02X:%02X:%02X:%02X:%02X:%02X | %4d | %4d | %8d | %8d | %7d | %8d | %7d | %4d\n",
            scan_idx,
            l2_data.mac.octet[0],l2_data.mac.octet[1],l2_data.mac.octet[2],l2_data.mac.octet[3],l2_data.mac.octet[4],l2_data.mac.octet[5],
            l2_data.vid, l2_data.port, (l2_data.flags&RTK_L2_UCAST_FLAG_SA_BLOCK)?1:0, (l2_data.flags&RTK_L2_UCAST_FLAG_DA_BLOCK)?1:0,
            (l2_data.flags&RTK_L2_UCAST_FLAG_STATIC)?1:0, (l2_data.flags&RTK_L2_UCAST_FLAG_NEXTHOP)?1:0,
            (l2_data.state&RTK_L2_UCAST_STATE_SUSPEND)?1:0, l2_data.agg_vid);

        total_entry++;
    }

    diag_util_mprintf("\nTotal Number Of Entries :%d\n", total_entry);

    return CPARSER_OK;
}

/*
 * port get polling-phy port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t
cparser_cmd_port_get_polling_phy_port_ports_all_state(
    cparser_context_t *context, char **ports_ptr)
{
    rtdrv_ext_portCfg_t portCfg;
    diag_portlist_t     portlist;
    uint32              unit, port;
    uint32  master_view_unit;
    int32               ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    portCfg.unit = unit;

    diag_util_mprintf("Polling PHY state:\n");

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        portCfg.port = port;
        GETSOCKOPT(RTDRV_EXT_PORT_MACPOLLINGPHYSTATE_GET, &portCfg, rtdrv_ext_portCfg_t, 1);
        diag_util_mprintf("\tPort %2d : %s\n", port, (portCfg.state == ENABLED) ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_port_get_polling_phy_port_ports_all_state */

/*
 * port set polling-phy port ( <PORT_LIST:ports> | all ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_port_set_polling_phy_port_ports_all_state_disable_enable(
    cparser_context_t *context, char **ports_ptr)
{
    rtdrv_ext_portCfg_t portCfg;
    rtk_enable_t        enable;
    diag_portlist_t     portlist;
    uint32              unit, port;
    uint32  master_view_unit;
    int32               ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    switch(TOKEN_CHAR(6,0))
    {
        case 'd':
            enable = DISABLED;
            break;

        case 'e':
            enable = ENABLED;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    portCfg.unit = unit;
    portCfg.state = enable;

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        portCfg.port = port;
        SETSOCKOPT(RTDRV_EXT_PORT_MACPOLLINGPHYSTATE_SET, &portCfg, rtdrv_ext_portCfg_t, 1);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_port_set_polling_phy_port_ports_all_state_disable_enable */


/*
 * port get mac-force port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t
cparser_cmd_port_get_mac_force_port_ports_all_state(
    cparser_context_t *context,
    char **ports_ptr)
{
    rtdrv_ext_portCfg_t portCfg;
    diag_portlist_t     portlist;
    uint32              unit, port;
    uint32  master_view_unit;
    int32               ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    portCfg.unit = unit;

    diag_util_mprintf("MAC force mode state:\n");

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        portCfg.port = port;
        GETSOCKOPT(RTDRV_EXT_PORT_MACFORCESTATE_GET, &portCfg, rtdrv_ext_portCfg_t, 1);
        diag_util_mprintf("\tPort %2d : %s\n", port, (portCfg.state == ENABLED) ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_port_get_mac_force_port_ports_all_state */


/*
 * port set mac-force port ( <PORT_LIST:ports> | all ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_port_set_mac_force_port_ports_all_state_disable_enable(
    cparser_context_t *context,
    char **ports_ptr)
{
    rtdrv_ext_portCfg_t portCfg;
    rtk_enable_t        enable;
    diag_portlist_t     portlist;
    uint32              unit, port;
    uint32  master_view_unit;
    int32               ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    switch(TOKEN_CHAR(6,0))
    {
        case 'd':
            enable = DISABLED;
            break;

        case 'e':
            enable = ENABLED;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    portCfg.unit = unit;
    portCfg.state = enable;

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        portCfg.port = port;
        SETSOCKOPT(RTDRV_EXT_PORT_MACFORCESTATE_SET, &portCfg, rtdrv_ext_portCfg_t, 1);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_port_set_mac_force_port_ports_all_state_disable_enable */


/*
 * port get mac-force port ( <PORT_LIST:ports> | all ) link
 */
cparser_result_t
cparser_cmd_port_get_mac_force_port_ports_all_link(
    cparser_context_t *context,
    char **ports_ptr)
{
    rtdrv_ext_portCfg_t portCfg;
    diag_portlist_t     portlist;
    uint32              unit, port;
    uint32  master_view_unit;
    int32               ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    portCfg.unit = unit;

    diag_util_mprintf("MAC force mode Link state:\n");

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        portCfg.port = port;
        GETSOCKOPT(RTDRV_EXT_PORT_MACFORCELINK_GET, &portCfg, rtdrv_ext_portCfg_t, 1);
        diag_util_mprintf("\tPort %2d : %s\n", port, (portCfg.state == ENABLED) ? "Up" : "Down");
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_port_get_mac_force_port_ports_all_link_up_down */


/*
 * port set mac-force port ( <PORT_LIST:ports> | all ) link ( up | down )
 */
cparser_result_t
cparser_cmd_port_set_mac_force_port_ports_all_link_up_down(
    cparser_context_t *context,
    char **ports_ptr)
{
    rtdrv_ext_portCfg_t portCfg;
    rtk_enable_t        enable;
    diag_portlist_t     portlist;
    uint32              unit, port;
    uint32  master_view_unit;
    int32               ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    switch(TOKEN_CHAR(6,0))
    {
        case 'd':
            enable = DISABLED;
            break;

        case 'u':
            enable = ENABLED;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    portCfg.unit = unit;
    portCfg.state = enable;

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        portCfg.port = port;
        SETSOCKOPT(RTDRV_EXT_PORT_MACFORCELINK_SET, &portCfg, rtdrv_ext_portCfg_t, 1);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_port_set_mac_force_port_ports_all_link_up_down */

/*
 * port get mac-force port ( <PORT_LIST:ports> | all ) flowctrl
 */
cparser_result_t
cparser_cmd_port_get_mac_force_port_ports_all_flowctrl(
    cparser_context_t *context,
    char **ports_ptr)
{
    rtdrv_ext_portCfg_t portCfg;
    diag_portlist_t     portlist;
    uint32              unit, port;
    uint32  master_view_unit;
    int32               ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    portCfg.unit = unit;

    diag_util_mprintf("MAC force mode Flow Control:\n");

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        portCfg.port = port;
        GETSOCKOPT(RTDRV_EXT_PORT_MACFORCEFLOWCTRL_GET, &portCfg, rtdrv_ext_portCfg_t, 1);
        diag_util_mprintf("\tPort %2d : %s\n", port, (portCfg.state == ENABLED) ? "Enabled" : "Disabled");
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_port_get_mac_force_port_ports_all_flowctrl */


/*
 * port set mac-force port ( <PORT_LIST:ports> | all ) flowctrl ( enable | disable )
 */
cparser_result_t
cparser_cmd_port_set_mac_force_port_ports_all_flowctrl_enable_disable(
    cparser_context_t *context,
    char **ports_ptr)
{
    rtdrv_ext_portCfg_t portCfg;
    rtk_enable_t        enable;
    diag_portlist_t     portlist;
    uint32              unit, port;
    uint32  master_view_unit;
    int32               ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    switch(TOKEN_CHAR(6,0))
    {
        case 'd':
            enable = DISABLED;
            break;

        case 'e':
            enable = ENABLED;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    portCfg.unit = unit;
    portCfg.state = enable;

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        portCfg.port = port;
        SETSOCKOPT(RTDRV_EXT_PORT_MACFORCEFLOWCTRL_SET, &portCfg, rtdrv_ext_portCfg_t, 1);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_port_set_mac_force_port_ports_all_flowctrl_enable_disable */

/*
 * port set phy-cali port ( <PORT_LIST:ports> | all ) */
cparser_result_t
cparser_cmd_port_set_phy_cali_port_ports_all(
    cparser_context_t *context,
    char **ports_ptr)
{
    rtdrv_ext_portCfg_t portCfg;
    diag_portlist_t     portlist;
    uint32              unit, port;
    uint32  master_view_unit;
    int32               ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    portCfg.unit = unit;

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        portCfg.port = port;
        SETSOCKOPT(RTDRV_EXT_PORT_PHYCALI_SET, &portCfg, rtdrv_ext_portCfg_t, 1);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_port_set_phy_cali_port_ports_all */

/*
 * stack get debug state
 */
cparser_result_t
cparser_cmd_stack_get_debug_state(
    cparser_context_t *context)
{
    uint32              unit;
    uint32              master_view_unit;
    rtdrv_ext_stackCfg_t stackCfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("stack debug mode state:\n");

    stackCfg.unit = unit;

    GETSOCKOPT(RTDRV_EXT_STACK_DEBUG_GET, &stackCfg, rtdrv_ext_stackCfg_t, 1);
    diag_util_mprintf("\t%s\n", (stackCfg.enable == ENABLED) ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);

    return CPARSER_OK;
}   /* end of cparser_cmd_stack_get_debug_state */

/*
 * stack set debug state ( disable | enable )
 */
cparser_result_t
cparser_cmd_stack_set_debug_state_disable_enable(
    cparser_context_t *context)
{
    rtdrv_ext_stackCfg_t stackCfg;
    uint32              unit;
    uint32              master_view_unit;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    stackCfg.unit = unit;

    switch(TOKEN_CHAR(4,0))
    {
        case 'd':
            stackCfg.enable = DISABLED;
            break;

        case 'e':
            stackCfg.enable = ENABLED;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    SETSOCKOPT(RTDRV_EXT_STACK_DEBUG_SET, &stackCfg, rtdrv_ext_stackCfg_t, 1);

    return CPARSER_OK;
}   /* end of cparser_cmd_stack_set_debug_state_disable_enable */

/*
 * trunk get local-table ( <UINT:entry_idx> | all )
 */
cparser_result_t
cparser_cmd_trunk_get_local_table_entry_idx_all(
    cparser_context_t *context,
    uint32_t *entry_idx_ptr)
{
    uint32              unit;
    uint32              master_view_unit;
    uint32              entryStart, entryEnd, i;
    rtdrv_ext_trunkCfg_t trunkCfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    if(TOKEN_CHAR(3,0) == 'a')
    {
        entryStart = 0;
        entryEnd = 51;
    }
    else
    {
        entryStart = entryEnd = *entry_idx_ptr;
    }

    for(i = entryStart; i <= entryEnd; i ++)
    {
        diag_util_mprintf("trunk local table entry %d:\n", i);

        trunkCfg.unit = unit;
        trunkCfg.id = i;

        GETSOCKOPT(RTDRV_EXT_TRUNK_LOCAL_ENTRY_GET, &trunkCfg, rtdrv_ext_trunkCfg_t, 1);

        diag_util_mprintf("\tNum TX candi: %d\n", trunkCfg.localEntry.num_tx_candi);
        diag_util_mprintf("\tHash Mask index: %d\n", trunkCfg.localEntry.hash_msk_idx);
        diag_util_mprintf("\tTrunk port 7: %d\n", trunkCfg.localEntry.trk_port7);
        diag_util_mprintf("\tTrunk port 6: %d\n", trunkCfg.localEntry.trk_port6);
        diag_util_mprintf("\tTrunk port 5: %d\n", trunkCfg.localEntry.trk_port5);
        diag_util_mprintf("\tTrunk port 4: %d\n", trunkCfg.localEntry.trk_port4);
        diag_util_mprintf("\tTrunk port 3: %d\n", trunkCfg.localEntry.trk_port3);
        diag_util_mprintf("\tTrunk port 2: %d\n", trunkCfg.localEntry.trk_port2);
        diag_util_mprintf("\tTrunk port 1: %d\n", trunkCfg.localEntry.trk_port1);
        diag_util_mprintf("\tTrunk port 0: %d\n", trunkCfg.localEntry.trk_port0);
        diag_util_mprintf("\tSep Dlf Bcast Port: %d\n", trunkCfg.localEntry.sep_dlf_bcast_port);
        diag_util_mprintf("\tSep Known MC Port: %d\n", trunkCfg.localEntry.sep_kwn_mc_port);
        diag_util_mprintf("\tNum Avl TX candi: %d\n", trunkCfg.localEntry.num_avl_tx_candi);
        diag_util_mprintf("\tAvl Trunk port 7: %d\n", trunkCfg.localEntry.avl_trk_port7);
        diag_util_mprintf("\tAvl Trunk port 6: %d\n", trunkCfg.localEntry.avl_trk_port6);
        diag_util_mprintf("\tAvl Trunk port 5: %d\n", trunkCfg.localEntry.avl_trk_port5);
        diag_util_mprintf("\tAvl Trunk port 4: %d\n", trunkCfg.localEntry.avl_trk_port4);
        diag_util_mprintf("\tAvl Trunk port 3: %d\n", trunkCfg.localEntry.avl_trk_port3);
        diag_util_mprintf("\tAvl Trunk port 2: %d\n", trunkCfg.localEntry.avl_trk_port2);
        diag_util_mprintf("\tAvl Trunk port 1: %d\n", trunkCfg.localEntry.avl_trk_port1);
        diag_util_mprintf("\tAvl Trunk port 0: %d\n", trunkCfg.localEntry.avl_trk_port0);
        diag_util_mprintf("\tAvl Sep Dlf Bcast Port: %d\n", trunkCfg.localEntry.avl_sep_dlf_bcast_port);
        diag_util_mprintf("\tAvl Dft Known MC Port: %d\n", trunkCfg.localEntry.avl_sep_kwn_mc_port);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_trunk_get_local_table_entry_idx_all */

/*
 * trunk reset
 */
cparser_result_t
cparser_cmd_trunk_reset(
    cparser_context_t *context)
{
    uint32              unit;
    uint32              master_view_unit;
    rtdrv_ext_trunkCfg_t trunkCfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    trunkCfg.unit = unit;

    SETSOCKOPT(RTDRV_EXT_TRUNK_RESET, &trunkCfg, rtdrv_ext_trunkCfg_t, 1);

    diag_util_mprintf("Trunk Configuraiton Reset Done\n");

    return CPARSER_OK;
}   /* end of cparser_cmd_trunk_reset */

/*
 * trunk set member trunk-group <UINT:trunk_id> port ( <DEV_PORT_LIST:dev_ports> | none ) */
cparser_result_t
cparser_cmd_trunk_set_member_trunk_group_trunk_id_port_dev_ports_none(
    cparser_context_t *context,
    uint32 *trunk_id_ptr,
    char **dev_ports_ptr)
{
    uint32              unit;
    uint32              master_view_unit;
    rtdrv_ext_trunkCfg_t trunkCfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    trunkCfg.unit = unit;
    trunkCfg.id = *trunk_id_ptr;

    osal_memset(&trunkCfg.trk_ports, 0, sizeof(trunkCfg.trk_ports));

    if ('n' != TOKEN_CHAR(6,0))
    {
        diag_util_str2devPorts(*dev_ports_ptr, &trunkCfg.trk_ports);
    }

    SETSOCKOPT(RTDRV_EXT_TRUNK_MEMBER_SET, &trunkCfg, rtdrv_ext_trunkCfg_t, 1);

    return CPARSER_OK;
}   /* end of cparser_cmd_trunk_set_member_trunk_group_trunk_id_port_dev_ports_none */

/*
 * switch get padding-content
 */
cparser_result_t
cparser_cmd_switch_get_padding_content(
    cparser_context_t *context)
{
    rtdrv_ext_switchCfg_t   cfg;
    uint32                  unit;
    uint32                  master_view_unit;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    cfg.unit = unit;
    GETSOCKOPT(RTDRV_EXT_SWITCH_PADDINCONTENT_GET, &cfg, rtdrv_ext_switchCfg_t, 1);

    diag_util_mprintf("Padding content: 0x%x\n", cfg.value);

    return CPARSER_OK;
}   /* end of cparser_cmd_switch_get_padding_content */

/*
 * switch set padding-content <UINT:byte>
 */
cparser_result_t
cparser_cmd_switch_set_padding_content_byte(
    cparser_context_t *context,
    uint32_t *byte_ptr)
{
    rtdrv_ext_switchCfg_t   cfg;
    uint32                  unit;
    uint32                  master_view_unit;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    if (*byte_ptr > 0xFF)
    {
        diag_util_printf("Error! value is over %d\n", 0xFF);
        return CPARSER_NOT_OK;
    }

    cfg.unit = unit;
    cfg.value = *byte_ptr;
    SETSOCKOPT(RTDRV_EXT_SWITCH_PADDINCONTENT_SET, &cfg, rtdrv_ext_switchCfg_t, 1);

    return CPARSER_OK;
}   /* end of cparser_cmd_switch_set_padding_content_UNIT32:_byte */

/*
 * register get remote <UINT:unit> <UINT:address>
 */
cparser_result_t
cparser_cmd_register_get_remote_unit_address(
    cparser_context_t *context,
    uint32_t *unit_ptr,
    uint32_t *address_ptr)
{
    rtdrv_ext_remoteAccessCfg_t   cfg;
    uint32                  unit;
    uint32                  master_view_unit;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    cfg.unit = unit;
    cfg.targetUnit = *unit_ptr;
    cfg.addr = *address_ptr;


    GETSOCKOPT(RTDRV_EXT_REMOTE_ACCESS_REG_GET, &cfg, rtdrv_ext_remoteAccessCfg_t, 1);

    diag_util_mprintf("Unit %d Register 0x%x: 0x%x\n", *unit_ptr, *address_ptr, cfg.value);

    return CPARSER_OK;
}   /* end of cparser_cmd_register_get_remote_unit_address */

/*
 * register set remote <UINT:unit> <UINT:address> <UINT:value>
 */
cparser_result_t
cparser_cmd_register_set_remote_unit_address_value(
    cparser_context_t *context,
    uint32_t *unit_ptr,
    uint32_t *address_ptr,
    uint32_t *value_ptr)
{
    rtdrv_ext_remoteAccessCfg_t   cfg;
    uint32                  unit;
    uint32                  master_view_unit;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    cfg.unit = unit;
    cfg.targetUnit = *unit_ptr;
    cfg.addr = *address_ptr;
    cfg.value = *value_ptr;

    SETSOCKOPT(RTDRV_EXT_REMOTE_ACCESS_REG_SET, &cfg, rtdrv_ext_remoteAccessCfg_t, 1);

    return CPARSER_OK;
}   /* end of cparser_cmd_register_set_remote_unit_address_value */

/*
 * acl get mirror-sflow-bypass-post state
 */
cparser_result_t
cparser_cmd_acl_get_mirror_sflow_bypass_post_state(
    cparser_context_t *context)
{
    rtdrv_ext_aclCfg_t   cfg;
    uint32                  unit;
    uint32                  master_view_unit;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    cfg.unit = unit;

    GETSOCKOPT(RTDRV_EXT_ACL_MIRROR_SFLOW_BYPASS_POST_GET, &cfg, rtdrv_ext_aclCfg_t, 1);

    diag_util_mprintf("Mirror/Sflow Bypass Post ACL : %s\n", cfg.enable? "Enabled":"Disabled");

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_get_mirror_sflow_bypass_post_state */

/*
 * acl set mirror-sflow-bypass-post state ( disable | enable )
 */
cparser_result_t
cparser_cmd_acl_set_mirror_sflow_bypass_post_state_disable_enable(
    cparser_context_t *context)
{
    rtdrv_ext_aclCfg_t   acl_cfg;
    uint32  master_view_unit;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(acl_cfg.unit);
    master_view_unit = acl_cfg.unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &acl_cfg.unit);

    switch(TOKEN_CHAR(3,0))
    {
        case 'd':
            acl_cfg.enable = DISABLED;
            break;

        case 'e':
            acl_cfg.enable = ENABLED;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    SETSOCKOPT(RTDRV_EXT_ACL_MIRROR_SFLOW_BYPASS_POST_SET, &acl_cfg, rtdrv_ext_aclCfg_t, 1);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_mirror_sflow_bypass_post_state_disable_enable */


/*
 * diag get reg-array <UINT:unit> <UINT:reg_id> <UINT:idx1> <UINT:idx2> <UINT:words>
 */
cparser_result_t
cparser_cmd_diag_get_reg_array_unit_reg_id_idx1_idx2_words(
    cparser_context_t *context,
    uint32_t *unit_ptr,
    uint32_t *reg_id_ptr,
    uint32_t *idx1_ptr,
    uint32_t *idx2_ptr,
    uint32_t *words_ptr)
{
    uint32  unit;
    uint32  master_view_unit;
    rtdrv_ext_diagCfg_t cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();
    uint32  i;

    cfg.unit = *unit_ptr;
    cfg.reg  = *reg_id_ptr;
    cfg.idx1 = *idx1_ptr;
    cfg.idx2 = *idx2_ptr;
    GETSOCKOPT(RTDRV_EXT_DIAG_REGARRAY_GET, &cfg, rtdrv_ext_diagCfg_t, 1);
    DIAG_UTIL_MPRINTF("\"");
    for (i=0; i<(*words_ptr); i++)
    {
        if ((i > RTDRV_EXT_DIAGCFG_DATA_WORD_MAX) || (i > (*words_ptr)))
            break;

        DIAG_UTIL_MPRINTF("%08X", cfg.data[i]);
    }
    DIAG_UTIL_MPRINTF("\"\n");

    return CPARSER_OK;
}   /* end of cparser_cmd_diag_get_reg_array_unit_reg_id_idx1_idx2_words */

/*
 * diag set reg-array <UINT:unit> <UINT:reg_id> <UINT:idx1> <UINT:idx2> <UINT:words> <STRING:value_hex>
 */
cparser_result_t
cparser_cmd_diag_set_reg_array_unit_reg_id_idx1_idx2_words_value_hex(
    cparser_context_t *context,
    uint32_t *unit_ptr,
    uint32_t *reg_id_ptr,
    uint32_t *idx1_ptr,
    uint32_t *idx2_ptr,
    uint32_t *words_ptr,
    char **value_hex_ptr)
{
    uint32  unit;
    uint32  master_view_unit;
    rtdrv_ext_diagCfg_t cfg;
    uint32  i, offset;
    char    chr;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    cfg.unit = *unit_ptr;
    cfg.reg  = *reg_id_ptr;
    cfg.idx1 = *idx1_ptr;
    cfg.idx2 = *idx2_ptr;
    osal_memset(cfg.data, 0x00, sizeof(cfg.data));
    for (i=0, offset=0; i<strlen(*value_hex_ptr); i++)
    {
        if (((*words_ptr) > RTDRV_EXT_DIAGCFG_DATA_WORD_MAX) || (i > ((*words_ptr) * 8)))
            break;

        chr = (*(*value_hex_ptr + i));

        if ((chr >= '0') && (chr <= '9'))
        {
            cfg.data[(offset/8)] |= ((chr - '0') << (28 - ((offset%8)*4)));
            offset++;
        }
        else if ((chr >= 'a') && (chr >= 'f'))
        {
            cfg.data[(offset/8)] |= (((chr - 'a') + 0xA) << (28 - ((offset%8)*4)));
            offset++;
        }
        else if ((chr >= 'A') && (chr <= 'F'))
        {
            cfg.data[(offset/8)] |= (((chr - 'A') + 0xA) << (28 - ((offset%8)*4)));
            offset++;
        }
    }
    SETSOCKOPT(RTDRV_EXT_DIAG_REGARRAY_SET, &cfg, rtdrv_ext_diagCfg_t, 1);

    return CPARSER_OK;
}   /* end of cparser_cmd_diag_set_reg_array_unit_reg_id_idx1_idx2_words_value_hex */


/*
 * diag get reg-array-field <UINT:unit> <UINT:reg_id> <UINT:idx1> <UINT:idx2> <UINT:field_id> <UINT:words>
 */
cparser_result_t
cparser_cmd_diag_get_reg_array_field_unit_reg_id_idx1_idx2_field_id_words(
    cparser_context_t *context,
    uint32_t *unit_ptr,
    uint32_t *reg_id_ptr,
    uint32_t *idx1_ptr,
    uint32_t *idx2_ptr,
    uint32_t *field_id_ptr,
    uint32_t *words_ptr)
{
    uint32  unit;
    uint32  master_view_unit;
    rtdrv_ext_diagCfg_t cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();
    uint32  i;

    cfg.unit = *unit_ptr;
    cfg.reg  = *reg_id_ptr;
    cfg.idx1 = *idx1_ptr;
    cfg.idx2 = *idx2_ptr;
    cfg.field = *field_id_ptr;
    GETSOCKOPT(RTDRV_EXT_DIAG_REGARRAYFIELD_GET, &cfg, rtdrv_ext_diagCfg_t, 1);
    DIAG_UTIL_MPRINTF("\"");
    for (i=0; i<(*words_ptr); i++)
    {
        if ((i > RTDRV_EXT_DIAGCFG_DATA_WORD_MAX) || (i > (*words_ptr)))
            break;

        DIAG_UTIL_MPRINTF("%08X", cfg.data[i]);
    }
    DIAG_UTIL_MPRINTF("\"\n");

    return CPARSER_OK;
}   /* end of cparser_cmd_diag_get_reg_array_field_unit_reg_id_idx1_idx2_field_id_words */

/*
 * diag set reg-array-field <UINT:unit> <UINT:reg_id> <UINT:idx1> <UINT:idx2> <UINT:field_id> <UINT:words> <STRING:value_hex>
 */
cparser_result_t
cparser_cmd_diag_set_reg_array_field_unit_reg_id_idx1_idx2_field_id_words_value_hex(
    cparser_context_t *context,
    uint32_t *unit_ptr,
    uint32_t *reg_id_ptr,
    uint32_t *idx1_ptr,
    uint32_t *idx2_ptr,
    uint32_t *field_id_ptr,
    uint32_t *words_ptr,
    char **value_hex_ptr)
{
    uint32  unit;
    uint32  master_view_unit;
    rtdrv_ext_diagCfg_t cfg;
    uint32  i, offset;
    char    chr;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    cfg.unit = *unit_ptr;
    cfg.reg  = *reg_id_ptr;
    cfg.idx1 = *idx1_ptr;
    cfg.idx2 = *idx2_ptr;
    cfg.field = *field_id_ptr;
    osal_memset(cfg.data, 0x00, sizeof(cfg.data));
    for (i=0, offset=0; i<strlen(*value_hex_ptr); i++)
    {
        if (((*words_ptr) > RTDRV_EXT_DIAGCFG_DATA_WORD_MAX) || (i > ((*words_ptr) * 8)))
            break;

        chr = (*(*value_hex_ptr + i));

        if ((chr >= '0') && (chr <= '9'))
        {
            cfg.data[(offset/8)] |= ((chr - '0') << (28 - ((offset%8)*4)));
            offset++;
        }
        else if ((chr >= 'a') && (chr >= 'f'))
        {
            cfg.data[(offset/8)] |= (((chr - 'a') + 0xA) << (28 - ((offset%8)*4)));
            offset++;
        }
        else if ((chr >= 'A') && (chr <= 'F'))
        {
            cfg.data[(offset/8)] |= (((chr - 'A') + 0xA) << (28 - ((offset%8)*4)));
            offset++;
        }
    }
    SETSOCKOPT(RTDRV_EXT_DIAG_REGARRAYFIELD_SET, &cfg, rtdrv_ext_diagCfg_t, 1);

    return CPARSER_OK;
}   /* end of cparser_cmd_diag_set_reg_array_field_unit_reg_id_idx1_idx2_field_id_words_value_hex */

/*
 * diag get table-entry <UINT:unit> <UINT:table_id> <UINT:entry_addr> <UINT:words>
 */
cparser_result_t
cparser_cmd_diag_get_table_entry_unit_table_id_entry_addr_words(
    cparser_context_t *context,
    uint32_t *unit_ptr,
    uint32_t *table_id_ptr,
    uint32_t *entry_addr_ptr,
    uint32_t *words_ptr)
{
    uint32  unit;
    uint32  master_view_unit;
    rtdrv_ext_diagCfg_t cfg;
    uint32  i;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    cfg.unit  = *unit_ptr;
    cfg.table = *table_id_ptr;
    cfg.addr  = *entry_addr_ptr;
    GETSOCKOPT(RTDRV_EXT_DIAG_TABLEENTRY_GET, &cfg, rtdrv_ext_diagCfg_t, 1);
    DIAG_UTIL_MPRINTF("\"");
    for (i=0; i<(*words_ptr); i++)
    {
        if ((i > RTDRV_EXT_DIAGCFG_DATA_WORD_MAX) || (i > (*words_ptr)))
            break;

        DIAG_UTIL_MPRINTF("%08X", cfg.data[i]);
    }
    DIAG_UTIL_MPRINTF("\"\n");

    return CPARSER_OK;
}   /* end of cparser_cmd_diag_get_table_entry_unit_table_id_entry_addr_words */

/*
 * diag set table-entry <UINT:unit> <UINT:table_id> <UINT:entry_addr> <UINT:words> <STRING:value_hex>
 */
cparser_result_t
cparser_cmd_diag_set_table_entry_unit_table_id_entry_addr_words_value_hex(
    cparser_context_t *context,
    uint32_t *unit_ptr,
    uint32_t *table_id_ptr,
    uint32_t *entry_addr_ptr,
    uint32_t *words_ptr,
    char **value_hex_ptr)
{
    uint32  unit;
    uint32  master_view_unit;
    rtdrv_ext_diagCfg_t cfg;
    uint32  i, offset;
    char    chr;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    cfg.unit  = *unit_ptr;
    cfg.table = *table_id_ptr;
    cfg.addr  = *entry_addr_ptr;
    osal_memset(cfg.data, 0x00, sizeof(cfg.data));
    for (i=0, offset=0; i<strlen(*value_hex_ptr); i++)
    {
        if (((*words_ptr) > RTDRV_EXT_DIAGCFG_DATA_WORD_MAX) || (i > ((*words_ptr) * 8)))
            break;

        chr = (*(*value_hex_ptr + i));

        if ((chr >= '0') && (chr <= '9'))
        {
            cfg.data[(offset/8)] |= ((chr - '0') << (28 - ((offset%8)*4)));
            offset++;
        }
        else if ((chr >= 'a') && (chr >= 'f'))
        {
            cfg.data[(offset/8)] |= (((chr - 'a') + 0xA) << (28 - ((offset%8)*4)));
            offset++;
        }
        else if ((chr >= 'A') && (chr <= 'F'))
        {
            cfg.data[(offset/8)] |= (((chr - 'A') + 0xA) << (28 - ((offset%8)*4)));
            offset++;
        }
    }
    SETSOCKOPT(RTDRV_EXT_DIAG_TABLEENTRY_SET, &cfg, rtdrv_ext_diagCfg_t, 1);

    return CPARSER_OK;
}   /* end of cparser_cmd_diag_set_table_entry_unit_table_id_entry_addr_words_value_hex */

/*
 * thermal get current (low | high)
 */
cparser_result_t
cparser_cmd_thermal_get_current_low_high(
    cparser_context_t *context)
{
    uint32  unit;
    uint32  master_view_unit;
    rtdrv_ext_thermalCfg_t cfg;
    double temp;
    char *osrStr[] = {"128", "256", "512", "1024"};
    char *digtStr[] = {"2nd", "3rd"};

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    if(TOKEN_CHAR(3,0) == 'l')
    {
         cfg.isLow = 1;
    }
    else
        cfg.isLow = 0;

    cfg.unit  = unit;
    cfg.meterId = 0;
    GETSOCKOPT(RTDRV_EXT_THERMAL_CURRENT_GET, &cfg, rtdrv_ext_thermalCfg_t, 1);

    DIAG_UTIL_MPRINTF("meter ID: 0\n");
    DIAG_UTIL_MPRINTF("regA: 0x%x\n", cfg.regA);
    DIAG_UTIL_MPRINTF("regB: 0x%x\n", cfg.regB);
    DIAG_UTIL_MPRINTF("OSR Select: %s\n", osrStr[cfg.osrSel]);
    DIAG_UTIL_MPRINTF("DIGT Order Select: %s\n", digtStr[cfg.digtOdrSel]);
    if((1<<18) & cfg.ct_out)
        temp = (double)((cfg.ct_out & 0x3FFFF) ^ 0x3FFFF) / 1024.0 * -1;
    else
        temp = (double)(cfg.ct_out) / 1024.0;

    DIAG_UTIL_MPRINTF("CT OUT: %f*C (0x%x)\n", temp, cfg.ct_out);

    cfg.meterId = 1;
    GETSOCKOPT(RTDRV_EXT_THERMAL_CURRENT_GET, &cfg, rtdrv_ext_thermalCfg_t, 1);

    DIAG_UTIL_MPRINTF("\nmeter ID: 1\n");
    DIAG_UTIL_MPRINTF("regA: 0x%x\n", cfg.regA);
    DIAG_UTIL_MPRINTF("regB: 0x%x\n", cfg.regB);
    DIAG_UTIL_MPRINTF("OSR Select: %s\n", osrStr[cfg.osrSel]);
    DIAG_UTIL_MPRINTF("DIGT Order Select: %s\n", digtStr[cfg.digtOdrSel]);
    if((1<<18) & cfg.ct_out)
        temp = (double)((cfg.ct_out & 0x3FFFF) ^ 0x3FFFF) / 1024.0 * -1;
    else
        temp = (double)(cfg.ct_out) / 1024.0;

    DIAG_UTIL_MPRINTF("CT OUT: %f*C (0x%x)\n", temp, cfg.ct_out);

    return CPARSER_OK;
}   /* end of cparser_cmd_thermal_get_current */

/*
 * nic send packet pri <UINT:start> <UINT:end> len <UINT:len> num <UINT:num>
 */
cparser_result_t
cparser_cmd_nic_send_packet_pri_start_end_len_len_num_num(
    cparser_context_t *context,
    uint32_t *start_ptr,
    uint32_t *end_ptr,
    uint32_t *len_ptr,
    uint32_t *num_ptr)
{
    uint32  unit;
    uint32  master_view_unit;
    rtdrv_ext_nicCfg_t cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&cfg, 0x00, sizeof(cfg));

    cfg.unit  = unit;
    cfg.start = *start_ptr;
    cfg.end  = *end_ptr;
    cfg.len  = *len_ptr;
    cfg.num  = *num_ptr;

    SETSOCKOPT(RTDRV_EXT_NIC_PKT_SEND_PRI_SET, &cfg, rtdrv_ext_nicCfg_t, 1);

    return CPARSER_OK;
}   /* end of cparser_cmd_nic_send_packet_pri_start_end_num_num */

/*
 * diag send <UINT:val>
 */
cparser_result_t
cparser_cmd_diag_send_val(
    cparser_context_t *context,
    uint32_t *val_ptr)
{
    uint32  unit;
    uint32  master_view_unit;
    rtdrv_ext_diagCfg_t cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&cfg, 0x00, sizeof(cfg));
    cfg.unit  = unit;
    cfg.field = *val_ptr;

    SETSOCKOPT(RTDRV_EXT_DIAG_SEND_VAL, &cfg, rtdrv_ext_diagCfg_t, 1);

    return CPARSER_OK;
}   /* end of cparser_cmd_diag_send_val */

