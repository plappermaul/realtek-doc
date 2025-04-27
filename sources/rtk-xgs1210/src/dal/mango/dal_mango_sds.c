
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
 * $Revision: 80942 $
 * $Date: 2017-08-01 14:55:23 +0800 (Tue, 01 Aug 2017) $
 *
 * Purpose : Definition those public global APIs and its data type in the SDK.
 *
 * Feature :  SerDes configuration
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/debug/rt_log.h>
#include <osal/sem.h>
#include <osal/lib.h>
#include <osal/memory.h>
#include <osal/time.h>
#include <hal/chipdef/allreg.h>
#include <hal/chipdef/mango/rtk_mango_reg_struct.h>
#include <hal/common/halctrl.h>
#include <hal/mac/reg.h>
#include <hal/mac/serdes.h>
#include <hal/mac/drv/drv_rtl9310.h>
#include <hal/phy/phy_rtl9310.h>
#include <dal/dal_mapper.h>
#include <dal/mango/dal_mango_sds.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
static uint32           sds_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};
static osal_mutex_t     sds_sem[RTK_MAX_NUM_OF_UNIT];

/*
 * Macro Definition
 */
/* trap semaphore handling */
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
 *      dal_mango_sds_leq_get
 * Description:
 *      Get SerDes LEQ
 * Input:
 *      unit - unit id
 *      sds  - user SerDes id
 * Output:
 *      leq - LEQ value
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
int32
dal_mango_sds_leq_get(uint32 unit, uint32 sds, uint32 *leq)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SDS), "unit=%d,sds=%d,leq=%d", unit, sds, leq);

    /* check Init status */
    RT_INIT_CHK(sds_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((!HWP_SDS_EXIST(unit, sds)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == leq), RT_ERR_NULL_POINTER);

    if (sds < 2)
        return RT_ERR_FAILED;

    SDS_SEM_LOCK(unit);

    RT_ERR_HDL(phy_rtl9310_leq_get(unit, sds, leq), ERR, ret);

ERR:
    SDS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_sds_leq_get */

/* Function Name:
 *      dal_mango_sds_leq_set
 * Description:
 *      Set SerDes LEQ
 * Input:
 *      unit - unit id
 *      sds  - user SerDes id
 *      leq  - LEQ value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
int32
dal_mango_sds_leq_set(uint32 unit, uint32 sds, uint32 leq)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SDS), "unit=%d,sds=%d,leq=%d", unit, sds, leq);

    /* check Init status */
    RT_INIT_CHK(sds_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((!HWP_SDS_EXIST(unit, sds)), RT_ERR_OUT_OF_RANGE);

    if (sds < 2)
        return RT_ERR_FAILED;

    SDS_SEM_LOCK(unit);

    RT_ERR_HDL(phy_rtl9310_leq_set(unit, sds, leq), ERR, ret);

ERR:
    SDS_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_sds_leq_set */

/* Function Name:
 *      dal_mango_sds_leq_adapt
 * Description:
 *      Set SerDes LEQ adapt
 * Input:
 *      unit - unit id
 *      sds  - user SerDes id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
int32
dal_mango_sds_leq_adapt(uint32 unit, uint32 sds)
{
    int32   ret;

    RT_ERR_CHK(phy_rtl9310_rxCali(unit, sds), ret);

    return ret;
}   /* end of dal_mango_sds_leq_adapt */

/* Function Name:
 *      dal_mango_sds_testModeCnt_get
 * Description:
 *      Get SerDes test mode counter
 * Input:
 *      unit      - unit id
 *      sds       - user SerDes id
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
dal_mango_sds_testModeCnt_get(uint32 unit, uint32 sds, uint32 *cnt)
{
    uint32  asds;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SDS), "unit=%d,sds=%d", unit, sds);

    /* check Init status */
    RT_INIT_CHK(sds_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_SDS_EXIST(unit, sds), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == cnt), RT_ERR_NULL_POINTER);

    /* function body */
    SDS_SEM_LOCK(unit);

    RT_ERR_HDL(drv_rtl9310_sds2AnaSds_get(unit, sds, &asds), ERR, ret);
    RT_ERR_HDL(SDS_FIELD_R(unit, asds, 0x5, 0xb, 15, 0, cnt), ERR, ret);
ERR:
    SDS_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_sds_testModeCnt_get */

/* Function Name:
 *      _dal_mango_sds_testDis_set
 * Description:
 *      Disable SerDes test mode.
 * Input:
 *      unit      - unit id
 *      sds       - analog SerDes id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 * Note:
 *      None
 */
static int32
_dal_mango_sds_testDis_set(uint32 unit, uint32 sds)
{
    int32   ret;

    /* PRBS9 */
    RT_ERR_CHK(SDS_FIELD_W(unit, sds, 0x5, 0xa, 7, 6, 0x0), ret);

    /* PRBS31 */
    RT_ERR_CHK(SDS_FIELD_W(unit, sds, 0x5, 0xa, 5, 4, 0x0), ret);

    /* Square 8 */
    RT_ERR_CHK(SDS_FIELD_W(unit, sds, 0x5, 0xa, 1, 1, 0x0), ret);

    RT_ERR_CHK(SDS_FIELD_W(unit, sds, 0x6, 0x0, 14, 13, 0x0), ret);

    return RT_ERR_OK;
}   /* end of _dal_mango_sds_testDis_set */

/* Function Name:
 *      _dal_mango_sds_testPrbs9_set
 * Description:
 *      Set SerDes test PRBS-9 mode.
 * Input:
 *      unit      - unit id
 *      sds       - analog SerDes id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 * Note:
 *      None
 */
static int32
_dal_mango_sds_testPrbs9_set(uint32 unit, uint32 sds)
{
    int32   ret;

    RT_ERR_CHK(SDS_FIELD_W(unit, sds, 0x5, 0xa, 13, 4, 0x0), ret);
    RT_ERR_CHK(SDS_FIELD_W(unit, sds, 0x5, 0xa, 1, 1, 0x0), ret);

    RT_ERR_CHK(SDS_FIELD_W(unit, sds, 0x5, 0xa, 7, 6, 0x3), ret);
    RT_ERR_CHK(SDS_FIELD_W(unit, sds, 0x6, 0x0, 12, 8, 0x2), ret);
    RT_ERR_CHK(SDS_FIELD_W(unit, sds, 0x6, 0x0, 14, 13, 0x3), ret);
    RT_ERR_CHK(SDS_FIELD_W(unit, sds, 0x6, 0xe, 8, 0, 0x2), ret);
    RT_ERR_CHK(SDS_FIELD_W(unit, sds, 0x6, 0xf, 15, 0, 0x2), ret);

    return RT_ERR_OK;
}   /* end of _dal_mango_sds_testPrbs9_set */

/* Function Name:
 *      _dal_mango_sds_testPrbs31_set
 * Description:
 *      Set SerDes test PRBS-31 mode.
 * Input:
 *      unit      - unit id
 *      sds       - analog SerDes id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 * Note:
 *      None
 */
static int32
_dal_mango_sds_testPrbs31_set(uint32 unit, uint32 sds)
{
    int32   ret;

    RT_ERR_CHK(SDS_FIELD_W(unit, sds, 0x5, 0xa, 13, 4, 0x0), ret);
    RT_ERR_CHK(SDS_FIELD_W(unit, sds, 0x5, 0xa, 1, 1, 0x0), ret);

    RT_ERR_CHK(SDS_FIELD_W(unit, sds, 0x5, 0xa, 5, 4, 0x3), ret);
    RT_ERR_CHK(SDS_FIELD_W(unit, sds, 0x6, 0x0, 12, 8, 0x2), ret);
    RT_ERR_CHK(SDS_FIELD_W(unit, sds, 0x6, 0x0, 14, 13, 0x3), ret);
    RT_ERR_CHK(SDS_FIELD_W(unit, sds, 0x6, 0xe, 8, 0, 0x2), ret);
    RT_ERR_CHK(SDS_FIELD_W(unit, sds, 0x6, 0xf, 15, 0, 0x2), ret);

    return RT_ERR_OK;
}   /* end of _dal_mango_sds_testPrbs31_set */

/* Function Name:
 *      _dal_mango_sds_testSquare8_set
 * Description:
 *      Set SerDes test 8081 mode.
 * Input:
 *      unit      - unit id
 *      sds       - analog SerDes id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 * Note:
 *      None
 */
static int32
_dal_mango_sds_testSquare8_set(uint32 unit, uint32 sds)
{
    int32   ret;

    RT_ERR_CHK(SDS_FIELD_W(unit, sds, 0x5, 0xa, 13, 4, 0x0), ret);
    RT_ERR_CHK(SDS_FIELD_W(unit, sds, 0x5, 0xa, 1, 1, 0x0), ret);

    RT_ERR_CHK(SDS_FIELD_W(unit, sds, 0x6, 0, 14, 13, 0x3), ret);
    RT_ERR_CHK(SDS_FIELD_W(unit, sds, 0x6, 0, 7, 4, 0x8), ret);
    RT_ERR_CHK(SDS_FIELD_W(unit, sds, 0x5, 0xa, 1, 1, 1), ret);

    return RT_ERR_OK;
}   /* end of _dal_mango_sds_testSquare8_set */

/* Function Name:
 *      dal_mango_sds_testMode_set
 * Description:
 *      Set SerDes test mode.
 * Input:
 *      unit      - unit id
 *      sds       - user SerDes id
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
dal_mango_sds_testMode_set(uint32 unit, uint32 sds, rtk_sds_testMode_t mode)
{
    uint32  asds;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SDS), "unit=%d,sds=%d,mode=%d", unit, sds, mode);

    /* check Init status */
    RT_INIT_CHK(sds_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_SDS_EXIST(unit, sds), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((RTK_SDS_TESTMODE_END <= mode), RT_ERR_INPUT);

    /* function body */
    SDS_SEM_LOCK(unit);

    RT_ERR_HDL(drv_rtl9310_sds2AnaSds_get(unit, sds, &asds), ERR, ret);

    switch (mode)
    {
        case RTK_SDS_TESTMODE_DISABLE:
            RT_ERR_HDL(_dal_mango_sds_testDis_set(unit, asds), ERR, ret);
            break;
        case RTK_SDS_TESTMODE_PRBS9:
            RT_ERR_HDL(_dal_mango_sds_testPrbs9_set(unit, asds), ERR, ret);
            break;
        case RTK_SDS_TESTMODE_PRBS31:
            RT_ERR_HDL(_dal_mango_sds_testPrbs31_set(unit, asds), ERR, ret);
            break;
        case RTK_SDS_TESTMODE_SQUARE8:
            RT_ERR_HDL(_dal_mango_sds_testSquare8_set(unit, asds), ERR, ret);
            break;
        default:
            RT_LOG(LOG_DEBUG, (MOD_SDS|MOD_DAL), "mode=%u", mode);
            SDS_SEM_UNLOCK(unit);
            return RT_ERR_FAILED;
    }
ERR:
    SDS_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_sds_testMode_set */

/* Function Name:
 *      dal_mango_sds_symErr_get
 * Description:
 *      Get the SerDes symbol error count.
 * Input:
 *      unit      - unit id
 *      sds       - user SerDes id
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
dal_mango_sds_symErr_get(uint32 unit, uint32 sds, rtk_sds_symErr_t *info)
{
    int32   ret = RT_ERR_OK;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SDS), "unit=%d,sds=%d", unit, sds);

    /* check Init status */
    RT_INIT_CHK(sds_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_SDS_EXIST(unit, sds), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(NULL == info, RT_ERR_NULL_POINTER);

    /* function body */
    SDS_SEM_LOCK(unit);

    RT_ERR_HDL(phy_rtl9310_symErr_get(unit, sds, info), ERR, ret);

ERR:
    SDS_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_sds_symErr_get */

/* Function Name:
 *      dal_mango_sds_symErr_clear
 * Description:
 *      Clear the SerDes symbol error count.
 * Input:
 *      unit      - unit id
 *      sds       - user SerDes id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_mango_sds_symErr_clear(uint32 unit, uint32 sds)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SDS), "unit=%d,sds=%d", unit, sds);

    /* check Init status */
    RT_INIT_CHK(sds_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_SDS_EXIST(unit, sds), RT_ERR_OUT_OF_RANGE);

    /* function body */
    SDS_SEM_LOCK(unit);

    RT_ERR_HDL(phy_rtl9310_symErr_clear(unit, sds), ERR, ret);

ERR:
    SDS_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_sds_symErr_clear */

/* Function Name:
 *      dal_mango_sds_linkSts_get
 * Description:
 *      Get the SerDes link status.
 * Input:
 *      unit      - unit id
 *      sds       - user SerDes id
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
dal_mango_sds_linkSts_get(uint32 unit, uint32 sds, rtk_sds_linkSts_t *info)
{
    int32   ret = RT_ERR_OK;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SDS), "unit=%d,sds=%d", unit, sds);

    /* check Init status */
    RT_INIT_CHK(sds_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_SDS_EXIST(unit, sds), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(NULL == info, RT_ERR_NULL_POINTER);

    /* function body */
    SDS_SEM_LOCK(unit);

    RT_ERR_HDL(phy_rtl9310_linkSts_get(unit, sds, info), ERR, ret);

ERR:
    SDS_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_sds_linkSts_get */

/* Function Name:
 *      dal_mango_sds_xsgNwayEn_set
 * Description:
 *      Set SerDes XSG N-way state
 * Input:
 *      unit - unit id
 *      sds  - user SerDes id
 *      en   - Configure XSG N-way state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
int32
dal_mango_sds_xsgNwayEn_set(uint32 unit, uint32 sds, rtk_enable_t en)
{
    uint32  xsg_sdsid_0, xsg_sdsid_1;
    uint32  val;
    int32   ret = RT_ERR_OK;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SDS), "unit=%d,sds=%d,en=%d", unit, sds, en);

    /* check Init status */
    RT_INIT_CHK(sds_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_SDS_EXIST(unit, sds), RT_ERR_OUT_OF_RANGE);

    /* function body */
    if (ENABLED == en)
        val = 0;
    else
        val = 1;

    SDS_SEM_LOCK(unit);

    RT_ERR_HDL(drv_rtl9310_sds2XsgmSds_get(unit, sds, &xsg_sdsid_0), ERR, ret);
    xsg_sdsid_1 = xsg_sdsid_0 + 1;

    RT_ERR_HDL(SDS_FIELD_W(unit, xsg_sdsid_0, 0x0, 0x2, 9, 8, val), ERR, ret);
    RT_ERR_HDL(SDS_FIELD_W(unit, xsg_sdsid_1, 0x0, 0x2, 9, 8, val), ERR, ret);

ERR:
    SDS_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_sds_xsgNwayEn_set */

/* Function Name:
 *      dal_mango_sds_cmuBand_get
 * Description:
 *      Get SerDes CMU band
 * Input:
 *      unit - unit id
 *      sds  - user SerDes id
 * Output:
 *      band - CMU band value
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
int32
dal_mango_sds_cmuBand_get(uint32 unit, uint32 sds, uint32 *band)
{
    uint32  page, asds;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SDS), "unit=%d,sds=%d", unit, sds);

    /* check Init status */
    RT_INIT_CHK(sds_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_SDS_EXIST(unit, sds), RT_ERR_OUT_OF_RANGE);

    /* function body */
    SDS_SEM_LOCK(unit);

    sds -= (sds % 2);
    RT_ERR_HDL(drv_rtl9310_sdsCmuPage_get(unit, sds, &page), ERR, ret);
    RT_ERR_HDL(drv_rtl9310_sds2AnaSds_get(unit, sds, &asds), ERR, ret);
    page += 1;

    RT_ERR_HDL(hal_serdes_reg_set(unit, asds, 0x1f, 0x02, 73), ERR, ret);
    RT_ERR_HDL(SDS_FIELD_W(unit, asds, page, 0x5, 15, 15, 1), ERR, ret);
    RT_ERR_HDL(SDS_FIELD_R(unit, asds, 0x1f, 0x15, 8, 3, band), ERR, ret);

ERR:
    SDS_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_sds_cmuBand_get */

/* Function Name:
 *      dal_mango_sds_cmuBand_set
 * Description:
 *      Set SerDes CMU band state and value
 * Input:
 *      unit - unit id
 *      sds  - user SerDes id
 *      en   - Configure CMU band state
 *      val  - CMU band
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
int32
dal_mango_sds_cmuBand_set(uint32 unit, uint32 sds, rtk_enable_t en, uint32 val)
{
    uint32  page, asds;
    uint32  enVal;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SDS), "unit=%d,sds=%d,en=%d,val=%d", unit, sds, en, val);

    /* check Init status */
    RT_INIT_CHK(sds_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_SDS_EXIST(unit, sds), RT_ERR_OUT_OF_RANGE);

    /* function body */
    if (ENABLED == en)
        enVal = 0;
    else
        enVal = 1;

    SDS_SEM_LOCK(unit);
    sds -= (sds % 2);
    RT_ERR_HDL(drv_rtl9310_sdsCmuPage_get(unit, sds, &page), ERR, ret);
    sds = sds & 0xFFFFFFFE;
    RT_ERR_HDL(drv_rtl9310_sds2AnaSds_get(unit, sds, &asds), ERR, ret);
    page += 1;

    RT_ERR_HDL(SDS_FIELD_W(unit, asds, page, 0x7, 13, 13, enVal), ERR, ret);
    RT_ERR_HDL(SDS_FIELD_W(unit, asds, page, 0x7, 11, 11, enVal), ERR, ret);
    RT_ERR_HDL(SDS_FIELD_W(unit, asds, page, 0x7, 4, 0, val), ERR, ret);

    RT_ERR_HDL(phy_rtl9310_sds_rst(unit, sds), ERR, ret);

ERR:
    SDS_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_sds_cmuBand_set */

/* Function Name:
 *      _dal_mango_sds_eye_dbg_out
 * Description:
 *      Set eye monitor debug
 * Input:
 *      unit    - unit id
 *      sds     - analog SerDis id
 * Output:
 *      None
 * Return:
 *      RT_ERR_FAILED - initialize fail
 *      RT_ERR_OK     - initialize success
 * Note:
 *      None
 */
static int32
_dal_mango_sds_eye_dbg_out(uint32 unit, uint32 sds)
{
    uint32  EYE_DBG_OUT_PAGE = 0x1F;
    uint32  EYE_DBG_OUT_REG = 0x02;
    int32   ret;

    if (0 == (sds % 2))
        RT_ERR_CHK(hal_serdes_reg_set(unit, sds, EYE_DBG_OUT_PAGE, EYE_DBG_OUT_REG, 0x0039), ret);
    else
        RT_ERR_CHK(hal_serdes_reg_set(unit, (sds - 1), EYE_DBG_OUT_PAGE, EYE_DBG_OUT_REG, 0x0057), ret);

    return RT_ERR_OK;
}   /* end of _dal_mango_sds_eye_dbg_out */

/* Function Name:
 *      _dal_mango_sds_eye_scan_en
 * Description:
 *      Enable eye monitor
 * Input:
 *      unit    - unit id
 *      sds     - analog SerDis id
 * Output:
 *      None
 * Return:
 *      RT_ERR_FAILED - initialize fail
 *      RT_ERR_OK     - initialize success
 * Note:
 *      None
 */
static int32
_dal_mango_sds_eye_scan_en(uint32 unit, uint32 sds)
{
    uint32  EYE_SCAN_EN_PAGE = 0x21;
    uint32  EYE_SCAN_EN_REG = 0x1;
    uint32  EYE_SCAN_EN_BIT = 2;
    uint32  EYE_CK_SEL_PAGE = 0x21;
    uint32  EYE_CK_SEL_REG = 0x01;
    uint32  EYE_CK_SEL_BIT = 9;
    int32   ret;

    RT_ERR_CHK(SDS_FIELD_W(unit, sds, EYE_SCAN_EN_PAGE, EYE_SCAN_EN_REG,
            EYE_SCAN_EN_BIT, EYE_SCAN_EN_BIT, 1), ret);

    RT_ERR_CHK(SDS_FIELD_W(unit, sds, EYE_SCAN_EN_PAGE, EYE_SCAN_EN_REG,
            EYE_SCAN_EN_BIT, EYE_SCAN_EN_BIT, 0), ret);

    RT_ERR_CHK(SDS_FIELD_W(unit, sds, EYE_SCAN_EN_PAGE, EYE_SCAN_EN_REG,
            EYE_SCAN_EN_BIT, EYE_SCAN_EN_BIT, 1), ret);

    RT_ERR_CHK(SDS_FIELD_W(unit, sds, EYE_CK_SEL_PAGE, EYE_CK_SEL_REG,
            EYE_CK_SEL_BIT, EYE_CK_SEL_BIT, 0), ret);

    return RT_ERR_OK;
}   /* end of _dal_mango_sds_eye_scan_en */

/* Function Name:
 *      _dal_mango_sds_eye_pi_set
 * Description:
 *      Set eye monitor PI value
 * Input:
 *      unit    - unit id
 *      sds     - analog SerDis id
 *      xAxis   - x axis (PI) value
 * Output:
 *      None
 * Return:
 *      RT_ERR_FAILED - initialize fail
 *      RT_ERR_OK     - initialize success
 * Note:
 *      None
 */
static int32
_dal_mango_sds_eye_pi_set(uint32 unit, uint32 sds, uint32 xAxis)
{
    uint32  EYE_PI_EN_PAGE = 0x21;
    uint32  EYE_PI_EN_REG = 0x02;
    uint32  EYE_PI_EN_BITHigh = 15;
    uint32  EYE_PI_EN_BITLow = 12;
    uint32  EYE_PI_ST_PAGE = 0x21;
    uint32  EYE_PI_ST_REG = 0x03;
    uint32  EYE_PI_ST_BITHigh = 15;
    uint32  EYE_PI_ST_BITLow = 0;
    uint32  piEn, piSt;
    uint32  piEnList[] = {0x9, 0xC, 0x6, 0x3};
    uint32  piStList[2][16] = {
                {0xFFFF, 0x7FFF, 0x3FFF, 0x1FFF,
                 0x0FFF, 0x07FF, 0x03FF, 0x01FF,
                 0x00FF, 0x007F, 0x003F, 0x001F,
                 0x000F, 0x0007, 0x0003, 0x0001},
                {0x0000, 0x8000, 0xC000, 0xE000,
                 0xF000, 0xF800, 0xFC00, 0xFE00,
                 0xFF00, 0xFF80, 0xFFC0, 0xFFE0,
                 0xFFF0, 0xFFF8, 0xFFFC, 0xFFFE}};
    int32   ret;

    piEn = piEnList[xAxis / 16];
    piSt = piStList[(xAxis / 16) % 2][xAxis % 16];

    RT_ERR_CHK(SDS_FIELD_W(unit, sds, EYE_PI_EN_PAGE, EYE_PI_EN_REG,
            EYE_PI_EN_BITHigh, EYE_PI_EN_BITLow, piEn), ret);

    RT_ERR_CHK(SDS_FIELD_W(unit, sds, EYE_PI_ST_PAGE, EYE_PI_ST_REG,
            EYE_PI_ST_BITHigh, EYE_PI_ST_BITLow, piSt), ret);

    return RT_ERR_OK;
}   /* end of _dal_mango_sds_eye_pi_set */

/* Function Name:
 *      _dal_mango_sds_eye_ref_set
 * Description:
 *      Set eye monitor ref value
 * Input:
 *      unit    - unit id
 *      sds     - analog SerDis id
 *      yAxis   - y axis (ref) value
 * Output:
 *      None
 * Return:
 *      RT_ERR_FAILED - initialize fail
 *      RT_ERR_OK     - initialize success
 * Note:
 *      None
 */
static int32
_dal_mango_sds_eye_ref_set(uint32 unit, uint32 sds, uint32 yAxis)
{
    uint32  EYE_REF_CTRL_PAGE = 0x21;
    uint32  EYE_REF_CTRL_REG = 0x02;
    uint32  EYE_REF_CTRL_BITHigh = 11;
    uint32  EYE_REF_CTRL_BITLow = 6;
    int32   ret;

    RT_ERR_CHK(SDS_FIELD_W(unit, sds, EYE_REF_CTRL_PAGE, EYE_REF_CTRL_REG,
            EYE_REF_CTRL_BITHigh, EYE_REF_CTRL_BITLow, yAxis), ret);

    return RT_ERR_OK;
}   /* end of _dal_mango_sds_eye_ref_set */

/* Function Name:
 *      _dal_mango_sds_eye_mdioClk
 * Description:
 *      Toggle eye monitor MDIO clock
 * Input:
 *      unit    - unit id
 *      sds     - analog SerDis id
 * Output:
 *      None
 * Return:
 *      RT_ERR_FAILED - initialize fail
 *      RT_ERR_OK     - initialize success
 * Note:
 *      None
 */
static int32
_dal_mango_sds_eye_mdioClk(uint32 unit, uint32 sds)
{
    int32   ret;

    RT_ERR_CHK(SDS_FIELD_W(unit, sds, 0x21, 0x04, 0, 0, 1), ret);
    RT_ERR_CHK(SDS_FIELD_W(unit, sds, 0x21, 0x04, 0, 0, 0), ret);

    return RT_ERR_OK;
}   /* end of _dal_mango_sds_eye_mdioClk */

/* Function Name:
 *      _dal_mango_sds_eyePointRead
 * Description:
 *      Read eye monitor point
 * Input:
 *      unit    - unit id
 *      sds     - analog SerDis id
 * Output:
 *      val - value of eye monitor point
 * Return:
 *      RT_ERR_FAILED - initialize fail
 *      RT_ERR_OK     - initialize success
 * Note:
 *      None
 */
static int32
_dal_mango_sds_eyePointRead(uint32 unit, uint32 sds, uint32 *val)
{
    uint32  readPage = 0x1F;
    uint32  readReg = 0x15;
    int32   ret;

    RT_ERR_CHK(_dal_mango_sds_eye_mdioClk(unit, sds), ret);
    RT_ERR_CHK(hal_serdes_reg_get(unit, sds, readPage, readReg, val), ret);

    do {
        RT_ERR_CHK(_dal_mango_sds_eye_mdioClk(unit, sds), ret);
        RT_ERR_CHK(hal_serdes_reg_get(unit, sds, readPage, readReg, val), ret);
    } while (((*val >> 14) & 0x3) != 0x3);

    return RT_ERR_OK;
}   /* end of _dal_mango_sds_eyePointRead */

/* Function Name:
 *      dal_mango_sds_eyeMonitor_start
 * Description:
 *      Eye monitor start.
 * Input:
 *      unit    - unit id
 *      sds     - user SerDis id
 *      frameNum- frame number
 * Output:
 *      None
 * Return:
 *      RT_ERR_FAILED - initialize fail
 *      RT_ERR_OK     - initialize success
 * Note:
 *      None
 */
int32
dal_mango_sds_eyeMonitor_start(uint32 unit, uint32 sds, uint32 frameNum)
{
    uint32  xAxis, yAxis, frame;
    uint32  aSds;
    uint32  val, data_size;
    uint32  *eyeData = NULL;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SDS), "unit=%d,sds=%d,frameNum=%d", unit, sds, frameNum);

    /* check Init status */
    RT_INIT_CHK(sds_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_SDS_EXIST(unit, sds), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(frameNum > RTK_EYE_MON_FRAME_MAX, RT_ERR_OUT_OF_RANGE);

    data_size = sizeof(uint32) * RTK_EYE_MON_FRAME_MAX * RTK_EYE_MON_X_MAX * RTK_EYE_MON_Y_MAX;
    if ((eyeData = osal_alloc(data_size)) == NULL)
    {
        return RT_ERR_FAILED;
    }
    osal_memset(eyeData, 0, data_size);

    /* function body */
    SDS_SEM_LOCK(unit);

    RT_ERR_HDL(drv_rtl9310_sds2AnaSds_get(unit, sds, &aSds), ERR, ret);

    /* enable */
    RT_ERR_HDL(_dal_mango_sds_eye_dbg_out(unit, aSds), ERR, ret);
    RT_ERR_HDL(_dal_mango_sds_eye_scan_en(unit, aSds), ERR, ret);

    /* set x axis */
    for (xAxis = 0; xAxis < RTK_EYE_MON_X_MAX; ++xAxis)
    {
        RT_ERR_HDL(_dal_mango_sds_eye_pi_set(unit, aSds, xAxis), ERR, ret);

        /* set y axis */
        for (yAxis = 0; yAxis < RTK_EYE_MON_Y_MAX; ++yAxis)
        {
            RT_ERR_HDL(_dal_mango_sds_eye_ref_set(unit, aSds, yAxis), ERR, ret);

            /* get val */
            for (frame = 0; frame < frameNum; ++frame)
            {
                RT_ERR_HDL(_dal_mango_sds_eyePointRead(unit, aSds, &val), ERR, ret);
                eyeData[RTK_EYE_MON_DATA_POS(frame, xAxis, yAxis)] = val;
            }
        }
    }

    for (xAxis = 0; xAxis < RTK_EYE_MON_X_MAX; ++xAxis)
        for (yAxis = 0; yAxis < RTK_EYE_MON_Y_MAX; ++yAxis)
            for (frame = 0; frame < frameNum; ++frame)
                osal_printf("[%d, %d : %d]\n", xAxis, yAxis, eyeData[RTK_EYE_MON_DATA_POS(frame, xAxis, yAxis)]);

ERR:
    if (eyeData)
    {
        osal_free(eyeData);
    }

    SDS_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_sds_eyeMonitor_start */

/* Function Name:
 *      dal_mango_sds_rst
 * Description:
 *      Reset Serdes.
 * Input:
 *      unit    - unit id
 *      sds     - user serdes id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_OUT_OF_RANGE - Serdes index is not support.
 * Note:
 *      None
 */
int32
dal_mango_sds_rst(uint32  unit, uint32 sds)
{
    int32   ret;

    SDS_SEM_LOCK(unit);

    RT_ERR_HDL(phy_rtl9310_sds_rst(unit, sds), ERR, ret);

ERR:
    SDS_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_sds_rst */

/* Function Name:
 *      dal_mango_sds_rx_rst
 * Description:
 *      Reset Serdes Rx and original patch are kept.
 * Input:
 *      unit    - unit id
 *      sds     - user serdes id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_OUT_OF_RANGE - Serdes index is not support.
 * Note:
 *      None
 */
int32
dal_mango_sds_rx_rst(uint32  unit, uint32 sds)
{
    int32   ret;

    SDS_SEM_LOCK(unit);
    RT_ERR_HDL(phy_rtl9310_rx_rst(unit, sds), ERR, ret);

ERR:
    SDS_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_sds_rx_rst */

/* Function Name:
 *      dal_mango_sds_rxCali
 * Description:
 *      execute SerDes rx calibration.
 * Input:
 *      unit          - unit id
 *      sds          - SerDes id
 *      retryCnt   - retry count
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
dal_mango_sds_rxCali(uint32 unit, uint32 sds, uint32 retryCnt)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SDS), "unit=%d,sds:%d", unit,sds);

    /* check Init status */
    RT_INIT_CHK(sds_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_SDS_EXIST(unit, sds), RT_ERR_OUT_OF_RANGE);

    /* function body */
    SDS_SEM_LOCK(unit);

    RT_ERR_HDL(phy_rtl9310_rxCali(unit, sds), ERR, ret);

ERR:
    SDS_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_sds_rxCali */

/* Function Name:
 *      dal_mango_sds_rxCaliEnable_get
 * Description:
 *      Get the SerDes rx cali enable status.
 * Input:
 *      unit      - unit id
 *      sds       - SerDes id
 * Output:
 *      pEnable      - pointer to the sds enable status of rx calibration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_mango_sds_rxCaliEnable_get(uint32 unit, uint32 sds, rtk_enable_t *pEnable)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SDS), "unit=%d,sds:%d", unit,sds);

    /* check Init status */
    RT_INIT_CHK(sds_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_SDS_EXIST(unit, sds), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(NULL == pEnable, RT_ERR_NULL_POINTER);

    /* function body */
    SDS_SEM_LOCK(unit);

    RT_ERR_HDL(phy_rtl9310_sdsRxCaliEnable_get(unit, sds, pEnable), ERR, ret);

ERR:
    SDS_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_sds_rxCaliEnable_get */

/* Function Name:
 *      dal_mango_sds_rxCaliEnable_set
 * Description:
 *      Set the SerDes rx cali status.
 * Input:
 *      unit      - unit id
 *      sds       - SerDes id
 *      enable    - status of rx calibration
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
dal_mango_sds_rxCaliEnable_set(uint32 unit, uint32 sds, rtk_enable_t enable)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SDS), "unit=%d,sds:%d,enable=%d", unit, sds, enable);

    /* check Init status */
    RT_INIT_CHK(sds_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_SDS_EXIST(unit, sds), RT_ERR_OUT_OF_RANGE);

    /* function body */
    SDS_SEM_LOCK(unit);

    RT_ERR_HDL(phy_rtl9310_sdsRxCaliEnable_set(unit, sds, enable), ERR, ret);

ERR:
    SDS_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_sds_rxCaliEnable_set */

/* Function Name:
 *      dal_mango_sds_rxCaliDbgEnable_set
 * Description:
 *      Set debug msg status for SerDes rx calibration
 * Input:
 *      enable      - enable print debug msg
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
dal_mango_sds_rxCaliDbgEnable_set(rtk_enable_t enable)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SDS), "enable=%d", enable);

    RT_ERR_CHK(phy_rtl9310_sdsRxCaliDbgEnable_set(enable), ret);

    return ret;
}   /* end of dal_mango_sds_rxCaliDbgEnable_set */

/* Function Name:
 *      dal_mango_sds_rxCaliConf_get
 * Description:
 *      Get the SerDes rx calibration configration.
 * Input:
 *      unit      - unit id
 *      sds       - SerDes id
 * Output:
 *      pConf      - pointer to the sds rx calibration configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_mango_sds_rxCaliConf_get(uint32 unit, uint32 sds, rtk_sds_rxCaliConf_t *pConf)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SDS), "unit=%d,sds=%d", unit, sds);

    /* check Init status */
    RT_INIT_CHK(sds_init[unit]);

    RT_PARAM_CHK((NULL == pConf), RT_ERR_NULL_POINTER);

    /* function body */
    SDS_SEM_LOCK(unit);

    RT_ERR_HDL(phy_rtl9310_sdsRxCaliConf_get(unit, sds, pConf), ERR, ret);

ERR:
    SDS_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_sds_rxCaliConf_get */

/* Function Name:
 *      dal_mango_sds_rxCaliConf_set
 * Description:
 *      Config the SerDes rx calibration.
 * Input:
 *      unit      - unit id
 *      sds       - SerDes id
 *      conf      - rx calibration conf
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
dal_mango_sds_rxCaliConf_set(uint32 unit, uint32 sds, rtk_sds_rxCaliConf_t conf)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SDS), "unit=%d,sds=%d", unit, sds);

    /* check Init status */
    RT_INIT_CHK(sds_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_SDS_EXIST(unit, sds), RT_ERR_OUT_OF_RANGE);

    /* function body */
    SDS_SEM_LOCK(unit);

    RT_ERR_HDL(phy_rtl9310_sdsRxCaliConf_set(unit, sds, conf), ERR, ret);

ERR:
    SDS_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_sds_rxCaliConf_set */

/* Function Name:
 *      dal_mango_sds_txParam_get
 * Description:
 *      Get SerDes Tx parameter
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
dal_mango_sds_txParam_get(uint32 unit, uint32 sds, rtk_sds_eyeParam_t *pParam)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SDS), "unit=%d,sds=%d", unit, sds);

    /* check Init status */
    RT_INIT_CHK(sds_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_SDS_EXIST(unit, sds), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pParam), RT_ERR_NULL_POINTER);

    /* function body */
    SDS_SEM_LOCK(unit);

    RT_ERR_HDL(phy_rtl9310_sdsTxParam_get(unit, sds, pParam), ERR, ret);

ERR:
    SDS_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_sds_txParam_get */

/* Function Name:
 *      dal_mango_sds_txParam_set
 * Description:
 *      Set SerDes Tx parameters
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
dal_mango_sds_txParam_set(uint32 unit, uint32 sds, rtk_sds_eyeParam_t param)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SDS), "unit=%d,sds=%d", unit, sds);

    /* check Init status */
    RT_INIT_CHK(sds_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_SDS_EXIST(unit, sds), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(param.pre_en >= RTK_ENABLE_END, RT_ERR_INPUT);
    RT_PARAM_CHK(param.post_en >= RTK_ENABLE_END, RT_ERR_INPUT);

    /* function body */
    SDS_SEM_LOCK(unit);

    RT_ERR_HDL(phy_rtl9310_sdsTxParam_set(unit, sds, param), ERR, ret);

ERR:
    SDS_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_sds_txParam_set */

/* Function Name:
 *      dal_mango_sds_init
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
dal_mango_sds_init(uint32 unit)
{
    RT_INIT_REENTRY_CHK(sds_init[unit]);
    sds_init[unit] = INIT_NOT_COMPLETED;

    /* create semaphore */
    sds_sem[unit] = osal_sem_mutex_create();
    if (0 == sds_sem[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_SDS), "semaphore create failed");
        return RT_ERR_FAILED;
    }

    /* set init flag to complete init */
    sds_init[unit] = INIT_COMPLETED;

    return RT_ERR_OK;
} /* end of dal_mango_sds_init */

/* Function Name:
 *      dal_mango_sdsMapper_init
 * Description:
 *      Hook SerDes module of the specified device.
 * Input:
 *      pMapper - pointer of mapper
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Must Hook SerDes module before calling any switch APIs.
 */
int32
dal_mango_sdsMapper_init(dal_mapper_t *pMapper)
{
    pMapper->sds_init = dal_mango_sds_init;
    pMapper->sds_symErr_get = dal_mango_sds_symErr_get;
    pMapper->sds_symErr_clear = dal_mango_sds_symErr_clear;
    pMapper->sds_linkSts_get = dal_mango_sds_linkSts_get;
    pMapper->sds_testModeCnt_get = dal_mango_sds_testModeCnt_get;
    pMapper->sds_testMode_set = dal_mango_sds_testMode_set;
    pMapper->sds_rx_rst = dal_mango_sds_rx_rst;
    pMapper->sds_leq_get = dal_mango_sds_leq_get;
    pMapper->sds_leq_set = dal_mango_sds_leq_set;
    pMapper->sds_leq_adapt = dal_mango_sds_leq_adapt;
    pMapper->sds_xsgNwayEn_set = dal_mango_sds_xsgNwayEn_set;
    pMapper->sds_cmuBand_get = dal_mango_sds_cmuBand_get;
    pMapper->sds_cmuBand_set = dal_mango_sds_cmuBand_set;
    pMapper->sds_eyeMonitor_start = dal_mango_sds_eyeMonitor_start;
    pMapper->sds_eyeParam_get = dal_mango_sds_txParam_get;
    pMapper->sds_eyeParam_set = dal_mango_sds_txParam_set;
    pMapper->sds_rxCaliConf_get = dal_mango_sds_rxCaliConf_get;
    pMapper->sds_rxCaliConf_set = dal_mango_sds_rxCaliConf_set;

    return RT_ERR_OK;
}   /* end of dal_mango_sdsMapper_init */

/* Function Name:
 *      dal_mango_sds_rxCaliStatus_get
 * Description:
 *      Get the SerDes rx calibration status.
 * Input:
 *      unit      - unit id
 *      sds       - SerDes id
 * Output:
 *      pStatus      - pointer to  status of rx calibration
 * Return:
 *      RT_ERR_OK
 * Note:
 *      None
 */
int32
dal_mango_sds_rxCaliStatus_get(uint32 unit, uint32 sds, rtk_port_phySdsRxCaliStatus_t *pStatus)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SDS), "unit=%d,sds:%d", unit,sds);

    /* check Init status */
    RT_INIT_CHK(sds_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_SDS_EXIST(unit, sds), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(NULL == pStatus, RT_ERR_NULL_POINTER);

    /* function body */
    SDS_SEM_LOCK(unit);

    RT_ERR_HDL(phy_rtl9310_sdsRxCaliStatus_get(unit, sds, pStatus), ERR, ret);

ERR:
    SDS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_sds_rxCaliStatus_get */
