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
 * $Revision: 81532 $
 * $Date: 2017-08-18 16:50:57 +0800 (Fri, 18 Aug 2017) $
 *
 * Purpose : Definition those public SerDes APIs and its data type in the SDK.
 *
 * Feature : SerDes configuration
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/debug/rt_log.h>
#include <ioal/mem32.h>
#include <osal/sem.h>
#include <osal/lib.h>
#include <osal/memory.h>
#include <osal/time.h>
#include <hal/chipdef/allreg.h>
#include <hal/chipdef/cypress/rtk_cypress_reg_struct.h>
#include <hal/common/halctrl.h>
#include <hal/mac/reg.h>
#include <hal/mac/serdes.h>
#include <dal/cypress/dal_cypress_sds.h>
#include <hal/phy/phy_rtl8390.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
static uint32               sds_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};
static osal_mutex_t         sds_sem[RTK_MAX_NUM_OF_UNIT];

/*
 * Macro Definition
 */
/* vlan semaphore handling */
#define SDS_SEM_LOCK(unit)    \
do {\
    if (osal_sem_mutex_take(sds_sem[unit], OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_LOCK_FAILED, (MOD_DAL|MOD_SDS), "semaphore lock failed");\
        return RT_ERR_SEM_LOCK_FAILED;\
    }\
} while(0)
#define SDS_SEM_UNLOCK(unit)   \
do {\
    if (osal_sem_mutex_give(sds_sem[unit]) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_UNLOCK_FAILED, (MOD_DAL|MOD_SDS), "semaphore unlock failed");\
        return RT_ERR_SEM_UNLOCK_FAILED;\
    }\
} while(0)

/*
 * Function Declaration
 */

/* Function Name:
 *      dal_cypress_sds_testModeCnt_get
 * Description:
 *      Get SerDes test mode counter
 * Input:
 *      unit      - unit id
 *      sds       - SerDes id
 * Output:
 *      cnt       - SerDes test mode counter
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_sds_testModeCnt_get(uint32 unit, uint32 sds, uint32 *cnt)
{
    uint32  ofst, val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SDS), "unit=%d,sds=%d", unit, sds);

    /* check Init status */
    RT_INIT_CHK(sds_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_SDS_EXIST(unit, sds), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == cnt), RT_ERR_NULL_POINTER);

    /* function body */
    SDS_SEM_LOCK(unit);

    if (8 == sds)
        ofst = 0;
    else
        ofst = 0x800;

    ioal_mem32_read(unit, 0xb254 + ofst, &val);

    SDS_SEM_UNLOCK(unit);

    *cnt = (val >> 16);

    return RT_ERR_OK;
}   /* end of dal_cypress_sds_testModeCnt_get */

static int32
_dal_cypress_sds_testDis_set(uint32 unit, uint32 sds)
{
    uint32  ofst, val;

    if (8 == sds)
        ofst = 0;
    else
        ofst = 0x800;

    /* PRBS9 */
    ioal_mem32_read(unit, 0xb254 + ofst, &val);
    val &= ~(0x3f);
    ioal_mem32_write(unit, 0xb254 + ofst, val);

    /* PRBS31 */
    ioal_mem32_read(unit, 0xb254 + ofst, &val);
    val &= ~(0x3 << 4);
    ioal_mem32_write(unit, 0xb254 + ofst, val);

    ioal_mem32_read(unit, 0xb280 + ofst, &val);
    val &= ~(0x3 << 13);
    ioal_mem32_write(unit, 0xb280 + ofst, val);

    /* Square 8 */
    ioal_mem32_read(unit, 0xb320 + ofst, &val);
    val &= ~(1 << 13);
    val &= ~(1 << 14);
    ioal_mem32_write(unit, 0xb320 + ofst, val);

    /* reset */
    ioal_mem32_read(unit, 0xb320 + ofst, &val);
    val &= ~(1 << 3);
    ioal_mem32_write(unit, 0xb320 + ofst, val);

    ioal_mem32_read(unit, 0xb340 + ofst, &val);
    val |= (1 << 15);
    ioal_mem32_write(unit, 0xb340 + ofst, val);

    osal_time_mdelay(100);

    ioal_mem32_read(unit, 0xb340 + ofst, &val);
    val &= ~(1 << 15);
    ioal_mem32_write(unit, 0xb340 + ofst, val);

    ioal_mem32_read(unit, 0xb284 + ofst, &val);
    val |= (1 << 12);
    ioal_mem32_write(unit, 0xb284 + ofst, val);

    osal_time_mdelay(100);

    ioal_mem32_read(unit, 0xb284 + ofst, &val);
    val &= ~(1 << 12);
    ioal_mem32_write(unit, 0xb284 + ofst, val);

    return RT_ERR_OK;
}   /* end of _dal_cypress_sds_testDis_set */

static int32
_dal_cypress_sds_testPrbs9_set(uint32 unit, uint32 sds)
{
    uint32  ofst, val;

    _dal_cypress_sds_testDis_set(unit, sds);

    if (8 == sds)
        ofst = 0;
    else
        ofst = 0x800;

    ioal_mem32_read(unit, 0xb254 + ofst, &val);
    val |= (0x3f);
    ioal_mem32_write(unit, 0xb254 + ofst, val);

    return RT_ERR_OK;
}   /* end of _dal_cypress_sds_testPrbs9_set */

static int32
_dal_cypress_sds_testPrbs31_set(uint32 unit, uint32 sds)
{
    uint32  ofst, val;

    _dal_cypress_sds_testDis_set(unit, sds);

    if (8 == sds)
        ofst = 0;
    else
        ofst = 0x800;

    ioal_mem32_read(unit, 0xb320 + ofst, &val);
    val &= ~(0x1 << 3);
    ioal_mem32_write(unit, 0xb320 + ofst, val);

    ioal_mem32_read(unit, 0xb340 + ofst, &val);
    val |= (0x1 << 15);
    ioal_mem32_write(unit, 0xb340 + ofst, val);

    osal_time_mdelay(100);

    ioal_mem32_read(unit, 0xb340 + ofst, &val);
    val &= ~(0x1 << 15);
    ioal_mem32_write(unit, 0xb340 + ofst, val);

    ioal_mem32_read(unit, 0xb284 + ofst, &val);
    val |= (0x1 << 12);
    ioal_mem32_write(unit, 0xb284 + ofst, val);

    osal_time_mdelay(100);

    ioal_mem32_read(unit, 0xb284 + ofst, &val);
    val &= ~(0x1 << 12);
    ioal_mem32_write(unit, 0xb284 + ofst, val);

    ioal_mem32_read(unit, 0xb254 + ofst, &val);
    val |= (0x3 << 4);
    ioal_mem32_write(unit, 0xb254 + ofst, val);

    ioal_mem32_read(unit, 0xb280 + ofst, &val);
    val |= (0x3 << 13);
    val &= ~(0x1F << 8);
    val |= (0x2 << 8);
    ioal_mem32_write(unit, 0xb280 + ofst, val);

    return RT_ERR_OK;
}   /* end of _dal_cypress_sds_testPrbs31_set */

static int32
_dal_cypress_sds_testSquare8_set(uint32 unit, uint32 sds)
{
    uint32  ofst, val;

    _dal_cypress_sds_testDis_set(unit, sds);

    if (8 == sds)
        ofst = 0;
    else
        ofst = 0x800;

    ioal_mem32_read(unit, 0xb254 + ofst, &val);
    val |= (1 << 1);
    ioal_mem32_write(unit, 0xb254 + ofst, val);

    ioal_mem32_read(unit, 0xb280 + ofst, &val);
    val |= (0x3 << 13);
    val &= ~(0xF << 4);
    val |= (8 << 4);
    ioal_mem32_write(unit, 0xb280 + ofst, val);

    return RT_ERR_OK;
}   /* end of _dal_cypress_sds_testSquare8_set */

/* Function Name:
 *      dal_cypress_sds_testMode_set
 * Description:
 *      Set SerDes test mode.
 * Input:
 *      unit      - unit id
 *      sds       - SerDes id
 *      mode      - test mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 * Note:
 *      None
 */
int32
dal_cypress_sds_testMode_set(uint32 unit, uint32 sds, rtk_sds_testMode_t mode)
{
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SDS), "unit=%d,sds=%d,mode=%d", unit, sds, mode);

    /* check Init status */
    RT_INIT_CHK(sds_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_SDS_EXIST(unit, sds), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((RTK_SDS_TESTMODE_END <= mode), RT_ERR_INPUT);

    /* function body */
    SDS_SEM_LOCK(unit);

    switch (mode)
    {
        case RTK_SDS_TESTMODE_DISABLE:
            _dal_cypress_sds_testDis_set(unit, sds);
            break;
        case RTK_SDS_TESTMODE_PRBS9:
            _dal_cypress_sds_testPrbs9_set(unit, sds);
            break;
        case RTK_SDS_TESTMODE_PRBS31:
            _dal_cypress_sds_testPrbs31_set(unit, sds);
            break;
        case RTK_SDS_TESTMODE_SQUARE8:
            _dal_cypress_sds_testSquare8_set(unit, sds);
            break;
        default:
            RT_LOG(LOG_DEBUG, (MOD_PIE|MOD_DAL), "mode=%u", mode);
            return RT_ERR_FAILED;
    }

    SDS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_cypress_sds_testMode_set */

/* Function Name:
 *      dal_cypress_sds_symErr_get
 * Description:
 *      Get the SerDes symbol error count.
 * Input:
 *      unit      - unit id
 *      sds       - SerDes id
 * Output:
 *      info      - symbol error count information
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_sds_symErr_get(uint32 unit, uint32 sds, rtk_sds_symErr_t *info)
{
    uint32  addr, val, i;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SDS), "unit=%d,sds=%d", unit, sds);

    /* check Init status */
    RT_INIT_CHK(sds_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_SDS_EXIST(unit, sds), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(NULL == info, RT_ERR_NULL_POINTER);

    /* function body */
    SDS_SEM_LOCK(unit);

    addr = 0xa070 + ((sds / 2) * 0x400);
    if (sds % 2 != 0)
        addr += 0x100;

    for (i = 0x10; i <= 0x13; ++i)
    {
        ioal_mem32_write(unit, addr, i);
        ioal_mem32_read(unit, addr, &val);
        info->ch[i - 0x10] = val >> 16;
    }

    SDS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_cypress_sds_symErr_get */

/* Function Name:
 *      dal_cypress_port_sdsLinkSts_get
 * Description:
 *      Get the SerDes link status.
 * Input:
 *      unit      - unit id
 *      sds       - SerDes id
 * Output:
 *      info      - link status information
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_sds_linkSts_get(uint32 unit, uint32 sds, rtk_sds_linkSts_t *info)
{
    uint32  addr;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SDS), "unit=%d,sds=%d", unit, sds);

    /* check Init status */
    RT_INIT_CHK(sds_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_SDS_EXIST(unit, sds), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(NULL == info, RT_ERR_NULL_POINTER);

    /* function body */
    SDS_SEM_LOCK(unit);

    addr = 0xa07c + ((sds / 2) * 0x400);
    if (sds % 2 != 0)
        addr += 0x100;

    ioal_mem32_read(unit, addr, &info->sts);

    SDS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_cypress_sds_linkSts_get */

/* Function Name:
 *      dal_cypress_sds_rx_rst
 * Description:
 *      Reset Serdes and original patch are kept.
 * Input:
 *      unit    - unit id
 *      sds_num    - serdes id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_OUT_OF_RANGE - Serdes index is not support.
 * Note:
 *      None
 */
int32
dal_cypress_sds_rx_rst(uint32  unit, uint32 sds_num)
{
    int32   ret;

    RT_ERR_CHK(hal_mac_serdes_rst(unit, sds_num), ret);

    return RT_ERR_OK;
}   /* end of dal_cypress_sds_rx_rst */

/* Function Name:
 *      dal_cypress_sds_init
 * Description:
 *      Initialize switch module of the specified device.
 * Input:
 *      unit          - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      1. Module must be initialized before using all of APIs in this module
 */
int32
dal_cypress_sds_init(uint32 unit)
{
    uint32  val;

    RT_INIT_REENTRY_CHK(sds_init[unit]);
    sds_init[unit] = INIT_NOT_COMPLETED;

    /* create semaphore */
    sds_sem[unit] = osal_sem_mutex_create();
    if (0 == sds_sem[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_SDS), "semaphore create failed");
        return RT_ERR_FAILED;
    }

    val = 0;
    reg_field_write(unit, CYPRESS_SDS0_1_ANA_RG_EXTr, CYPRESS_S0_REG_CLKOUT_ENf, &val);
    reg_field_write(unit, CYPRESS_SDS2_3_ANA_RG_EXTr, CYPRESS_S0_REG_CLKOUT_ENf, &val);
    reg_field_write(unit, CYPRESS_SDS4_5_ANA_RG_EXTr, CYPRESS_S0_REG_CLKOUT_ENf, &val);
    reg_field_write(unit, CYPRESS_SDS6_7_ANA_RG_EXTr, CYPRESS_S0_REG_CLKOUT_ENf, &val);
    reg_field_write(unit, CYPRESS_SDS8_9_ANA_TGr, CYPRESS_S0_REG_CK25MO_E4f, &val);
    reg_field_write(unit, CYPRESS_SDS8_9_ANA_TGr, CYPRESS_S0_REG_CK25MO_ENf, &val);
    reg_field_write(unit, CYPRESS_SDS10_11_ANA_RG_EXTr, CYPRESS_S0_REG_CLKOUT_ENf, &val);
    reg_field_write(unit, CYPRESS_SDS12_13_ANA_TGr, CYPRESS_S0_REG_CK25MO_E4f, &val);
    reg_field_write(unit, CYPRESS_SDS12_13_ANA_TGr, CYPRESS_S0_REG_CK25MO_ENf, &val);

    /* set init flag to complete init */
    sds_init[unit] = INIT_COMPLETED;

    return RT_ERR_OK;
} /* end of dal_cypress_sds_init */

/* Function Name:
 *      dal_cypress_sdsMapper_init
 * Description:
 *      Hook port module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must Hook port module before calling any port APIs.
 */
int32
dal_cypress_sdsMapper_init(dal_mapper_t *pMapper)
{
    pMapper->sds_init = dal_cypress_sds_init;
    pMapper->sds_symErr_get = dal_cypress_sds_symErr_get;
    pMapper->sds_linkSts_get = dal_cypress_sds_linkSts_get;
    pMapper->sds_testModeCnt_get = dal_cypress_sds_testModeCnt_get;
    pMapper->sds_testMode_set = dal_cypress_sds_testMode_set;
    pMapper->sds_rx_rst = dal_cypress_sds_rx_rst;

    return RT_ERR_OK;
}   /* end of dal_cypress_sdsMapper_init */
