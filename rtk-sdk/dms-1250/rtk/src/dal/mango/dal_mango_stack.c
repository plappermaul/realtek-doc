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
 * Purpose : Definition those public STACK APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) Stack
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_bitop.h>
#include <common/util/rt_util.h>
#include <common/debug/rt_log.h>
#include <osal/sem.h>
#include <osal/lib.h>
#include <osal/memory.h>
#include <ioal/mem32.h>
#include <hal/chipdef/allmem.h>
#include <hal/chipdef/allreg.h>
#include <hal/chipdef/mango/rtk_mango_table_struct.h>
#include <hal/chipdef/mango/rtk_mango_reg_struct.h>
#include <hal/mac/reg.h>
#include <hal/mac/mem.h>
#include <hal/common/halctrl.h>
#include <dal/mango/dal_mango_stack.h>
#include <dal/mango/dal_mango_trunk.h>
#include <rtk/default.h>
#include <rtk/stack.h>
#include <dal/mango/dal_mango_port.h>
#include <hal/mac/serdes.h>
#include <hal/mac/drv/drv_rtl9310.h>

/*
 * Symbol Definition
 */
#define NON_DEF_PORT_IDX (0x3F)
#define DAL_MANGO_STACK_RMT_INTR_CMP_SEQ_MARGIN_MAX     (31)

/*
 * Data Declaration
 */
static uint32                   stack_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};
static osal_mutex_t         stack_sem[RTK_MAX_NUM_OF_UNIT];
uint32                            stackPortId2Idx[RTK_MAX_NUM_OF_UNIT][RTK_MAX_PORT_PER_UNIT];
rtk_port_t                       *idx2stackPortId[RTK_MAX_NUM_OF_UNIT];

/*
 * Macro Definition
 */
/* stack semaphore handling */
#define STACK_SEM_LOCK(unit)    \
do {\
    if (osal_sem_mutex_take(stack_sem[unit], OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_LOCK_FAILED, (MOD_DAL|MOD_STACK), "semaphore lock failed");\
        return RT_ERR_SEM_LOCK_FAILED;\
    }\
} while(0)

#define STACK_SEM_UNLOCK(unit)   \
do {\
    if (osal_sem_mutex_give(stack_sem[unit]) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_UNLOCK_FAILED, (MOD_DAL|MOD_STACK), "semaphore unlock failed");\
        return RT_ERR_SEM_UNLOCK_FAILED;\
    }\
} while(0)


/*
 * Function Declaration
 */

static int32 _dal_mango_stack_init_config(uint32 unit);

/* Function Name:
 *      dal_mango_stackMapper_init
 * Description:
 *      Hook stack module of the specified device.
 * Input:
 *      pMapper - pointer of mapper
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Must Hook stack module before calling any stack APIs.
 */
int32
dal_mango_stackMapper_init(dal_mapper_t *pMapper)
{
    pMapper->stack_init = dal_mango_stack_init;
    pMapper->stack_port_get = dal_mango_stack_port_get;
    pMapper->stack_port_set = dal_mango_stack_port_set;
    pMapper->stack_devId_get = dal_mango_stack_devId_get;
    pMapper->stack_devId_set = dal_mango_stack_devId_set;
    pMapper->stack_masterDevId_get = dal_mango_stack_masterDevId_get;
    pMapper->stack_masterDevId_set = dal_mango_stack_masterDevId_set;
    pMapper->stack_loopGuard_get = dal_mango_stack_loopGuard_get;
    pMapper->stack_loopGuard_set = dal_mango_stack_loopGuard_set;
    pMapper->stack_devPortMap_get = dal_mango_stack_devPortMap_get;
    pMapper->stack_devPortMap_set = dal_mango_stack_devPortMap_set;
    pMapper->stack_nonUcastBlockPort_get = dal_mango_stack_nonUcastBlockPort_get;
    pMapper->stack_nonUcastBlockPort_set = dal_mango_stack_nonUcastBlockPort_set;
    pMapper->stack_rmtIntrTxEnable_get = dal_mango_stack_rmtIntrTxEnable_get;
    pMapper->stack_rmtIntrTxEnable_set = dal_mango_stack_rmtIntrTxEnable_set;
    pMapper->stack_rmtIntrTxTriggerEnable_get = dal_mango_stack_rmtIntrTxTriggerEnable_get;
    pMapper->stack_rmtIntrTxTriggerEnable_set = dal_mango_stack_rmtIntrTxTriggerEnable_set;
    pMapper->stack_rmtIntrRxSeqCmpMargin_get = dal_mango_stack_rmtIntrRxSeqCmpMargin_get;
    pMapper->stack_rmtIntrRxSeqCmpMargin_set = dal_mango_stack_rmtIntrRxSeqCmpMargin_set;
    pMapper->stack_rmtIntrRxForceUpdateEnable_get = dal_mango_stack_rmtIntrRxForceUpdateEnable_get;
    pMapper->stack_rmtIntrRxForceUpdateEnable_set = dal_mango_stack_rmtIntrRxForceUpdateEnable_set;
    pMapper->stack_rmtIntrInfo_get = dal_mango_stack_rmtIntrInfo_get;
    pMapper->stack_shrink_get = dal_mango_stack_shrink_get;
    pMapper->stack_shrink_set = dal_mango_stack_shrink_set;

    return RT_ERR_OK;
}


/* Function Name:
 *      _dal_mango_stack_remote_reg_access_read
 * Description:
 *      Read register of a specified remote unit
 * Input:
 *      unit  - remote unit id
 *      reg   - register id
 * Output:
 *      value - value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Internal API (used by OSAL APIs)
 */
static int32 _dal_mango_stack_remote_reg_access_read(uint32 unit, uint32 addr, uint32 *pVal)
{
    int32  ret = RT_ERR_FAILED;
    uint32 myUnit = HWP_MY_UNIT_ID();
    uint32 regVal;
    uint32 chkCnt;

    if (HWP_9310_FAMILY_ID(myUnit))
    {
        /* check status */
        chkCnt = 1000;
        do {
            RT_ERR_CHK(reg_field_read(myUnit, MANGO_RMT_REG_ACCESS_CTRLr, MANGO_EXECf, &regVal), ret);

            if (0 == regVal)
                break;

            /* timeout ? */
            if (0 == --chkCnt)
                return RT_ERR_FAILED;
        } while (1);

        /* make a request of reading remote register */
        RT_ERR_CHK(reg_field_write(myUnit, MANGO_RMT_REG_ACCESS_CTRLr, MANGO_DEVf, &unit), ret);
        RT_ERR_CHK(reg_field_write(myUnit, MANGO_RMT_REG_ACCESS_CTRLr, MANGO_ADDRf, &addr), ret);
        regVal = 0; /* read operation */
        RT_ERR_CHK(reg_field_write(myUnit, MANGO_RMT_REG_ACCESS_CTRLr, MANGO_RWOPf, &regVal), ret);
        regVal = 1; /* execute */
        RT_ERR_CHK(reg_field_write(myUnit, MANGO_RMT_REG_ACCESS_CTRLr, MANGO_EXECf, &regVal), ret);

        /* check status (wait for the result) */
        chkCnt = 10000;
        do {
            RT_ERR_CHK(reg_field_read(myUnit, MANGO_RMT_REG_ACCESS_CTRLr, MANGO_EXECf, &regVal), ret);

            if (0 == regVal)
                return RT_ERR_FAILED;

            /* timeout ? */
            if (0 == --chkCnt)
                return RT_ERR_FAILED;
        } while (1);

        /* check result */
        RT_ERR_CHK(reg_field_read(myUnit, MANGO_RMT_REG_ACCESS_CTRLr, MANGO_RESULTf, &regVal), ret);
        if (0 != regVal)
            return RT_ERR_FAILED;

        RT_ERR_CHK(reg_field_read(myUnit, MANGO_RMT_REG_ACCESS_DATAr, MANGO_DATAf, pVal), ret);

        return RT_ERR_OK;
    }

    return RT_ERR_FAILED;
}

/* Function Name:
 *      _dal_mango_stack_remote_reg_access_write
 * Description:
 *      Write register of a specified remote unit
 * Input:
 *      unit  - remote unit id
 *      reg   - register id
 *      value - value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Internal API (used by OSAL APIs)
 */
static int32 _dal_mango_stack_remote_reg_access_write(uint32 unit, uint32 addr, uint32 val)
{
    int32  ret = RT_ERR_FAILED;
    uint32 myUnit = HWP_MY_UNIT_ID();
    uint32 regVal;
    uint32 chkCnt;

    if (HWP_9310_FAMILY_ID(myUnit))
    {
        /* check status */
        chkCnt = 1000;
        do {
            RT_ERR_CHK(reg_field_read(myUnit, MANGO_RMT_REG_ACCESS_CTRLr, MANGO_EXECf, &regVal), ret);

            if (0 == regVal)
                break;

            /* timeout ? */
            if (0 == --chkCnt)
                return RT_ERR_FAILED;
        } while (1);

        /* make a request of writing remote register */
        RT_ERR_CHK(reg_field_write(myUnit, MANGO_RMT_REG_ACCESS_DATAr, MANGO_DATAf, &val), ret);

        RT_ERR_CHK(reg_field_write(myUnit, MANGO_RMT_REG_ACCESS_CTRLr, MANGO_DEVf, &unit), ret);
        RT_ERR_CHK(reg_field_write(myUnit, MANGO_RMT_REG_ACCESS_CTRLr, MANGO_ADDRf, &addr), ret);
        regVal = 1; /* write operation */
        RT_ERR_CHK(reg_field_write(myUnit, MANGO_RMT_REG_ACCESS_CTRLr, MANGO_RWOPf, &regVal), ret);
        regVal = 1; /* execute */
        RT_ERR_CHK(reg_field_write(myUnit, MANGO_RMT_REG_ACCESS_CTRLr, MANGO_EXECf, &regVal), ret);

        /* check status (wait for the result) */
        chkCnt = 10000;
        do {
            RT_ERR_CHK(reg_field_read(myUnit, MANGO_RMT_REG_ACCESS_CTRLr, MANGO_EXECf, &regVal), ret);

            if (0 == regVal)
                return RT_ERR_FAILED;

            /* timeout ? */
            if (0 == --chkCnt)
                return RT_ERR_FAILED;
        } while (1);

        /* check result */
        RT_ERR_CHK(reg_field_read(myUnit, MANGO_RMT_REG_ACCESS_CTRLr, MANGO_RESULTf, &regVal), ret);
        if (0 != regVal)
            return RT_ERR_FAILED;

        return RT_ERR_OK;
    }

    return RT_ERR_FAILED;
}

/* Function Name:
 *      dal_mango_stack_init
 * Description:
 *      Initialize stack module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      Must initialize stack module before calling any stack APIs.
 */
int32
dal_mango_stack_init(uint32 unit)
{
    int32  ret = RT_ERR_OK;
    uint32 idx;
    rtk_port_t port;

    RT_INIT_REENTRY_CHK(stack_init[unit]);
    stack_init[unit] = INIT_NOT_COMPLETED;

    /* register callback function if it's my unit id */
    if (unit == HWP_MY_UNIT_ID())
    {
        RT_ERR_CHK(ioal_mem32_hraCb_register(unit, IOAL_CB_READ, (void *)_dal_mango_stack_remote_reg_access_read), ret);
        RT_ERR_CHK(ioal_mem32_hraCb_register(unit, IOAL_CB_WRITE, (void *)_dal_mango_stack_remote_reg_access_write), ret);
    }

    /* create semaphore */
    stack_sem[unit] = osal_sem_mutex_create();
    if (0 == stack_sem[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_STACK), "semaphore create failed");
        return RT_ERR_FAILED;
    }

    /* set init flag to complete init */
    stack_init[unit] = INIT_COMPLETED;

    /* initialize default configuration */
    for(idx = 0; idx < RTK_MAX_UNIT_ID; idx++)
    {
        if (NULL == idx2stackPortId[idx])
        {
            idx2stackPortId[idx] = (rtk_port_t *)osal_alloc(HAL_MAX_NUM_OF_STACK_PORT(unit) * sizeof(rtk_port_t));
            if (NULL == idx2stackPortId[idx])
            {
                RT_ERR(RT_ERR_FAILED, (MOD_TRUNK|MOD_DAL), "memory allocate failed");
                goto err_free;
            }
            else
                osal_memset(idx2stackPortId[idx], 0, HAL_MAX_NUM_OF_STACK_PORT(unit) * sizeof(rtk_port_t));
        }
    }

    for(idx = 0; idx < HAL_MAX_NUM_OF_STACK_PORT(unit); idx++)
    {
        idx2stackPortId[unit][idx] = NON_DEF_PORT_IDX;
    }

    for(port = 0; port < RTK_MAX_PORT_PER_UNIT; port++)
    {
        stackPortId2Idx[unit][port] = NON_DEF_PORT_IDX;
    }

    if (HWP_UNIT_VALID_LOCAL(unit))
    {
        if ((ret = _dal_mango_stack_init_config(unit)) != RT_ERR_OK)
        {
            stack_init[unit] = INIT_NOT_COMPLETED;

            RT_ERR(ret, (MOD_STACK|MOD_DAL), "init default configuration failed");
            return ret;
        }
    }

    return ret;

err_free:

    stack_init[unit] = INIT_NOT_COMPLETED;
    for(unit = 0; unit < RTK_MAX_NUM_OF_UNIT; unit ++)
    {
        if (NULL != idx2stackPortId[unit])
            osal_free(idx2stackPortId[unit]);
        else
            continue;
    }
    return RT_ERR_MEM_ALLOC;
}   /* end of dal_mango_stack_init */

/* Module Name    : Stack                    */
/* Sub-module Name: User configuration stack */

/* Function Name:
 *      dal_mango_stack_port_get
 * Description:
 *      Get the stacking port from the specified device.
 * Input:
 *      unit  - unit id
 * Output:
 *      pStkPorts - pointer buffer of stacking ports
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_mango_stack_port_get (uint32 unit, rtk_portmask_t *pStkPorts)
{
    int32 ret;
    rtk_port_t port;
    uint32 idx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STACK), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(stack_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pStkPorts), RT_ERR_NULL_POINTER);

    STACK_SEM_LOCK(unit);
    /* get entry from CHIP*/

    osal_memset(pStkPorts, 0, sizeof(*pStkPorts));

    for (idx = 0; idx < HAL_MAX_NUM_OF_STACK_PORT(unit); idx++)
    {
        if ((ret = reg_array_field_read(unit,
                          MANGO_STK_PORT_ID_CTRLr,
                          REG_ARRAY_INDEX_NONE,
                          idx,
                          MANGO_STK_PORT_IDf,
                          &port)) != RT_ERR_OK)
        {
            STACK_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_STACK), "");
            return ret;
        }

        if(NON_DEF_PORT_IDX != port)
            RTK_PORTMASK_PORT_SET(*pStkPorts, port);
    }

    STACK_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STACK), "pStkPorts pmsk0=0x%x, pmsk1=0x%x",
           RTK_PORTMASK_WORD_GET(*pStkPorts, 0),
           RTK_PORTMASK_WORD_GET(*pStkPorts, 1));

    return RT_ERR_OK;
}   /* end of dal_mango_stack_port_get */

/* Function Name:
 *      dal_mango_stack_port_set
 * Description:
 *      Set stacking ports to the specified device.
 * Input:
 *      unit - unit id
 *      pStkPorts - pointer buffer of stacking ports
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_STACK_PORT_RX_EN - MAC RX disabled serdes port can't be configured as stacking port
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      In 9310, any port can be configured as stacking ports.
 */
int32
dal_mango_stack_port_set (uint32 unit, rtk_portmask_t *pStkPorts)
{
    int32 ret = RT_ERR_OK, ret_port = RT_ERR_OK;
    uint32  idx = 0, targetUnit;
    rtk_port_t port;
    rtk_trk_t trkId;
    rtk_portmask_t mapPorts[RTK_MAX_UNIT_ID+1], blockPorts[RTK_MAX_UNIT_ID+1], stkTrkPorts[8];
    rtk_portmask_t  stkPortCfg, stkPortOri, stkPortDiff;
    rtk_enable_t portEbl[RTK_MAX_NUM_OF_PORTS], ebl;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STACK), "unit=%d pStkPorts pmsk0=0x%x, pmsk1=0x%x", unit,
        RTK_PORTMASK_WORD_GET(*pStkPorts, 0),
        RTK_PORTMASK_WORD_GET(*pStkPorts, 1));

    /* check Init status */
    RT_INIT_CHK(stack_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(RTK_PORTMASK_GET_PORT_COUNT(*pStkPorts) > HAL_MAX_NUM_OF_STACK_PORT(unit), RT_ERR_STACK_PORT_NUM);
    RT_PARAM_CHK(hwp_portmaskValid_Chk(unit, pStkPorts), RT_ERR_PORT_MASK);

    RTK_PORTMASK_SCAN(*pStkPorts, port)
    {
        RT_ERR_CHK(drv_port_moduleRxEnable_get(unit, port, DRV_RTL9310_PORT_TXRX_EN_MOD_PORT, &ebl), ret);
        if(DISABLED == ebl && HWP_SERDES_PORT(unit, port)) //a serdes port can't be stacking port when RX_EN=disabled to avoid MAC issue
        {
            return RT_ERR_STACK_PORT_RX_EN;
        }
    }

    RTK_PORTMASK_ASSIGN(stkPortCfg, *pStkPorts);

    RT_ERR_CHK(dal_mango_stack_port_get(unit, &stkPortOri), ret);
    stkPortDiff.bits[0] = stkPortOri.bits[0] ^ pStkPorts->bits[0];
    stkPortDiff.bits[1] = stkPortOri.bits[1] ^ pStkPorts->bits[1];

    RTK_PORTMASK_SCAN(stkPortDiff, port)
    {
        RT_ERR_HDL(dal_mango_port_adminEnable_get(unit, port, &portEbl[port]), ERR_HDL_0, ret);
        RT_ERR_HDL(dal_mango_port_adminEnable_set(unit, port, DISABLED), ERR_HDL_0, ret);
    }

    STACK_SEM_LOCK(unit);

    /* get entry from CHIP*/
    for(targetUnit = 0; targetUnit <= RTK_MAX_UNIT_ID; targetUnit ++)
    {
        RT_ERR_HDL(_dal_mango_stack_devPortMap_get (unit, targetUnit, &mapPorts[targetUnit]), ERR_HDL_1, ret);

        mapPorts[targetUnit].bits[0] &= pStkPorts->bits[0];
        mapPorts[targetUnit].bits[1] &= pStkPorts->bits[1];

        RT_ERR_HDL(_dal_mango_stack_nonUcastBlockPort_get (unit, targetUnit, &blockPorts[targetUnit]), ERR_HDL_1, ret);
        blockPorts[targetUnit].bits[0] &= pStkPorts->bits[0];
        blockPorts[targetUnit].bits[1] &= pStkPorts->bits[1];
    }

    RT_ERR_HDL(_dal_mango_trunk_semLock(unit), ERR_HDL_1, ret);

    osal_memset(stkTrkPorts, 0, sizeof(stkTrkPorts));

    for(trkId = 0; trkId < 8; trkId ++)
    {
        RT_ERR_HDL(_dal_mango_trunk_stkTrkPort_get(unit, trkId, &stkTrkPorts[trkId]), ERR_HDL_2, ret);
        stkTrkPorts[trkId].bits[0] &= pStkPorts->bits[0];
        stkTrkPorts[trkId].bits[1] &= pStkPorts->bits[1];
    }

    for(idx = 0; idx < HAL_MAX_NUM_OF_STACK_PORT(unit); idx ++)
    {
        if (idx2stackPortId[unit][idx] != NON_DEF_PORT_IDX)
        {
            if(RTK_PORTMASK_IS_PORT_CLEAR(*pStkPorts, idx2stackPortId[unit][idx]))
            {
                stackPortId2Idx[unit][idx2stackPortId[unit][idx]] = NON_DEF_PORT_IDX;
                idx2stackPortId[unit][idx] = NON_DEF_PORT_IDX;

                RT_ERR_HDL(reg_array_field_write(unit,
                          MANGO_STK_PORT_ID_CTRLr,
                          REG_ARRAY_INDEX_NONE,
                          idx,
                          MANGO_STK_PORT_IDf,
                          &idx2stackPortId[unit][idx]), ERR_HDL_2, ret);
            }
            else
            {
                port = idx2stackPortId[unit][idx];
                RTK_PORTMASK_PORT_CLEAR(*pStkPorts, idx2stackPortId[unit][idx]);
            }
        }
    }

    idx = 0;

    RTK_PORTMASK_SCAN(*pStkPorts, port)
    {
        while( idx2stackPortId[unit][idx] != NON_DEF_PORT_IDX)
        {
            idx ++;
            if (idx >= HAL_MAX_NUM_OF_STACK_PORT(unit))
            {
                ret = RT_ERR_STACK_PORT_NUM;
                goto ERR_HDL_2;
            }
        }

        RT_ERR_HDL(reg_array_field_write(unit,
                          MANGO_STK_PORT_ID_CTRLr,
                          REG_ARRAY_INDEX_NONE,
                          idx,
                          MANGO_STK_PORT_IDf,
                          &port), ERR_HDL_2, ret);

        stackPortId2Idx[unit][port] = idx;
        idx2stackPortId[unit][idx] = port;
    }

    for(targetUnit = 0; targetUnit <= RTK_MAX_UNIT_ID; targetUnit ++)
    {
        RT_ERR_HDL(_dal_mango_stack_devPortMap_set (unit, targetUnit, &mapPorts[targetUnit]), ERR_HDL_2, ret);

        RT_ERR_HDL(_dal_mango_stack_nonUcastBlockPort_set (unit, targetUnit, &blockPorts[targetUnit]), ERR_HDL_2, ret);
    }

    for(trkId = 0; trkId < 8; trkId ++)
    {
        RT_ERR_HDL(_dal_mango_trunk_stkTrkPort_set(unit, trkId, &stkTrkPorts[trkId]), ERR_HDL_2, ret);
    }

    /* keep stack ports to DB */
    HAL_STACK_PORTMASK_SET(unit, &stkPortCfg);

ERR_HDL_2:
    _dal_mango_trunk_semUnlock(unit);
ERR_HDL_1:
    STACK_SEM_UNLOCK(unit);
ERR_HDL_0:


    RTK_PORTMASK_SCAN(stkPortDiff, port)
    {
        RT_ERR_CHK(dal_mango_port_adminEnable_set(unit, port, portEbl[port]), ret_port);
    }

    if(RT_ERR_OK != ret)
        RT_ERR(ret, (MOD_DAL|MOD_STACK), "");

    return RT_ERR_OK;
}   /* end of dal_mango_stack_port_set */


/* Function Name:
 *      dal_mango_stack_devId_get
 * Description:
 *      Get the switch device ID from the specified device.
 * Input:
 *      unit                   - unit id
 * Output:
 *      pMyDevID              - pointer buffer of device ID of the switch
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_mango_stack_devId_get(uint32 unit, uint32 *pMyDevID)
{
    int32 ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STACK), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(stack_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pMyDevID), RT_ERR_NULL_POINTER);

    STACK_SEM_LOCK(unit);
    /* get entry from CHIP*/
    if ((ret = reg_field_read(unit,
                          MANGO_STK_GBL_CTRLr,
                          MANGO_MY_DEV_IDf,
                          &val)) != RT_ERR_OK)
    {
        STACK_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STACK), "");
        return ret;
    }
    STACK_SEM_UNLOCK(unit);
    *pMyDevID = val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STACK), "*pMyDevID = %u", *pMyDevID);

    return RT_ERR_OK;
}   /* end of dal_mango_stack_devId_get */

/* Function Name:
 *      dal_mango_stack_devId_set
 * Description:
 *      Set the switch device ID to the specified device.
 * Input:
 *      unit                   - unit id
 *      myDevID              - device ID of the switch
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID           - invalid unit id
 *      RT_ERR_NOT_INIT          - The module is not initial
 * Note:
 *      (1) 9310 supports 16 stacked devices, thus myDevID ranges from 0 to 15.
 */
int32
dal_mango_stack_devId_set(uint32 unit, uint32 myDevID)
{
    int32 ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STACK), "unit=%u myDevID=%u", unit, myDevID);

    /* check Init status */
    RT_INIT_CHK(stack_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(myDevID >= HAL_MAX_NUM_OF_STACK_DEV(unit), RT_ERR_STACK_DEV_ID);

    STACK_SEM_LOCK(unit);

    val = myDevID;

    /* get entry from CHIP*/
    if ((ret = reg_field_write(unit,
                          MANGO_STK_GBL_CTRLr,
                          MANGO_MY_DEV_IDf,
                          &val)) != RT_ERR_OK)
    {
        STACK_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STACK), "");
        return ret;
    }

    STACK_SEM_UNLOCK(unit);

    rtk_unit2devID[unit] = myDevID;
    rtk_dev2unitID[myDevID] = unit;

    return RT_ERR_OK;
}   /* end of dal_mango_stack_devId_set */

/* Function Name:
 *      dal_mango_stack_masterDevId_get
 * Description:
 *      Get the master device ID from the specified device.
 * Input:
 *      unit                   - unit id
 * Output:
 *      pMasterDevID        - pointer buffer of device ID of the master switch
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_mango_stack_masterDevId_get(uint32 unit, uint32 *pMasterDevID)
{
    int32 ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STACK), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(stack_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pMasterDevID), RT_ERR_NULL_POINTER);

    STACK_SEM_LOCK(unit);
    /* get entry from CHIP*/
    if ((ret = reg_field_read(unit,
                          MANGO_STK_GBL_CTRLr,
                          MANGO_MASTER_DEV_IDf,
                          &val)) != RT_ERR_OK)
    {
        STACK_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STACK), "");
        return ret;
    }
    STACK_SEM_UNLOCK(unit);
    *pMasterDevID = val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STACK), "*pMasterDevID = ", *pMasterDevID);

    return RT_ERR_OK;
}   /* end of dal_mango_stack_masterDevId_get */

/* Function Name:
 *      dal_mango_stack_masterDevId_set
 * Description:
 *      Set the master device ID to the specified device.
 * Input:
 *      unit                   - unit id
 *      masterDevId         - device ID of the master switch
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID           - invalid unit id
 *      RT_ERR_NOT_INIT          - The module is not initial
 * Note:
 *      (1) 9310 supports 16 stacked devices, thus masterDevID ranges from 0 to 15.
 */
int32
dal_mango_stack_masterDevId_set(uint32 unit, uint32 masterDevID)
{
    int32 ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STACK), "unit=%d masterDevID=%d", unit, masterDevID);

    /* check Init status */
    RT_INIT_CHK(stack_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(masterDevID >= HAL_MAX_NUM_OF_STACK_DEV(unit), RT_ERR_STACK_DEV_ID);

    STACK_SEM_LOCK(unit);

    val = masterDevID;

    /* get entry from CHIP*/
    if ((ret = reg_field_write(unit,
                          MANGO_STK_GBL_CTRLr,
                          MANGO_MASTER_DEV_IDf,
                          &val)) != RT_ERR_OK)
    {
        STACK_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STACK), "");
        return ret;
    }

    STACK_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_stack_masterDevId_set */

/* Function Name:
 *      dal_mango_stack_loopGuard_get
 * Description:
 *      Get the enable status of dropping packets with source device ID the same as the device ID of the switch from the specified device.
 * Input:
 *      unit          - unit id
 * Output:
 *      pEnable     - pointer buffer of enable state of loop guard mechanism
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_mango_stack_loopGuard_get(uint32 unit, rtk_enable_t *pEnable)
{
    int32 ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STACK), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(stack_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    STACK_SEM_LOCK(unit);
    /* get entry from CHIP*/
    if ((ret = reg_field_read(unit,
                          MANGO_STK_GBL_CTRLr,
                          MANGO_DROP_MY_DEVf,
                          &val)) != RT_ERR_OK)
    {
        STACK_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STACK), "");
        return ret;
    }
    STACK_SEM_UNLOCK(unit);
    *pEnable = val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STACK), "*pEnable = ", *pEnable);

    return RT_ERR_OK;
}   /* end of dal_mango_stack_loopGuard_get */


/* Function Name:
 *      dal_mango_stack_loopGuard_set
 * Description:
 *      Set the enable status of dropping packets with source device ID the same as the device ID of the switch to the specified device.
 * Input:
 *      unit          - unit id
 *      enable - enable state of loop guard mechanism
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_mango_stack_loopGuard_set(uint32 unit, rtk_enable_t enable)
{
    int32 ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STACK), "unit=%d enable=%d", unit, enable);

    /* check Init status */
    RT_INIT_CHK(stack_init[unit]);

    /* parameter check */

    STACK_SEM_LOCK(unit);

    val = enable;

    /* get entry from CHIP*/
    if ((ret = reg_field_write(unit,
                          MANGO_STK_GBL_CTRLr,
                          MANGO_DROP_MY_DEVf,
                          &val)) != RT_ERR_OK)
    {
        STACK_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STACK), "");
        return ret;
    }

    STACK_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_stack_loopGuard_set */

int32
_dal_mango_stack_devPortMap_get (uint32 unit, uint32 dstDevID, rtk_portmask_t *pStkPorts)
{
    int32 ret;
    uint32 val;
    uint32 idx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STACK), "unit=%d, dstDevID=%d",
           unit, dstDevID);

    /* parameter check */
    RT_PARAM_CHK(dstDevID >= HAL_MAX_NUM_OF_STACK_DEV(unit), RT_ERR_STACK_DEV_ID);
    RT_PARAM_CHK((NULL == pStkPorts), RT_ERR_NULL_POINTER);

    /* get entry from CHIP*/
    if ((ret = reg_array_field_read(unit,
                          MANGO_STK_DEV_PORT_MAP_CTRLr,
                          REG_ARRAY_INDEX_NONE,
                          dstDevID,
                          MANGO_DEV_PORT_MAPf,
                          &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_STACK), "");
        return ret;
    }

    osal_memset(pStkPorts, 0, sizeof(*pStkPorts));
    for(idx = 0; idx < HAL_MAX_NUM_OF_STACK_PORT(unit); idx ++)
    {
        if((1 << idx) & val)
            RTK_PORTMASK_PORT_SET(*pStkPorts, idx2stackPortId[unit][idx]);
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STACK), "pStkPorts pmsk0=0x%x, pmsk1=0x%x",
        RTK_PORTMASK_WORD_GET(*pStkPorts, 0),
        RTK_PORTMASK_WORD_GET(*pStkPorts, 1));

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_stack_devPortMap_get
 * Description:
 *      Get the stacking ports that packets with specific target unit should forward to from the specified device.
 * Input:
 *      unit                   - unit id
 *      dstDevID                - device ID of forwarding target
 * Output:
 *      pStkPorts           - pointer buffer of egress stacking ports
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_mango_stack_devPortMap_get (uint32 unit, uint32 dstDevID, rtk_portmask_t *pStkPorts)
{
    int32 ret;

    /* check Init status */
    RT_INIT_CHK(stack_init[unit]);

    STACK_SEM_LOCK(unit);
    /* get entry from CHIP*/
    if ((ret = _dal_mango_stack_devPortMap_get (unit, dstDevID, pStkPorts)) != RT_ERR_OK)
    {
        STACK_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STACK), "");
        return ret;
    }

    STACK_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_stack_devPortMap_get */


int32
_dal_mango_stack_devPortMap_set (uint32 unit, uint32 dstDevID, rtk_portmask_t *pStkPorts)
{
    int32 ret;
    uint32  val;
    rtk_port_t port;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STACK), "unit=%d, dstDevID=%d, pStkPorts pmsk0=0x%x, pmsk1=0x%x",
           unit, dstDevID,
           RTK_PORTMASK_WORD_GET(*pStkPorts, 0),
           RTK_PORTMASK_WORD_GET(*pStkPorts, 1));

    /* parameter check */
    RT_PARAM_CHK(dstDevID >= HAL_MAX_NUM_OF_STACK_DEV(unit), RT_ERR_STACK_DEV_ID);
    RT_PARAM_CHK((NULL == pStkPorts), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(hwp_portmaskValid_Chk(unit, pStkPorts), RT_ERR_PORT_MASK);

    val = 0;
    RTK_PORTMASK_SCAN(*pStkPorts, port)
    {
        if(NON_DEF_PORT_IDX != stackPortId2Idx[unit][port])
        {
            val |= 1 << stackPortId2Idx[unit][port];
        }
        else
        {
            RT_ERR(RT_ERR_PORT_ID, (MOD_DAL|MOD_STACK), "");
            return RT_ERR_STACK_PORT_ID;
        }
    }

    /* set entry to CHIP*/
    if ((ret = reg_array_field_write(unit,
                          MANGO_STK_DEV_PORT_MAP_CTRLr,
                          REG_ARRAY_INDEX_NONE,
                          dstDevID,
                          MANGO_DEV_PORT_MAPf,
                          &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_STACK), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_stack_devPortMap_set
 * Description:
 *      Set the stacking ports that packets with specific target device should forward to for the specified device.
 * Input:
 *      unit                   - unit id
 *      dstDevID              - device ID of forwarding target
 *      pStkPorts           - pointer buffer of egress stacking ports
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) Stacking ports in 9310 ranges from 0 to 55.
 */
int32
dal_mango_stack_devPortMap_set (uint32 unit, uint32 dstDevID, rtk_portmask_t *pStkPorts)
{
    int32 ret;

    /* check Init status */
    RT_INIT_CHK(stack_init[unit]);

    STACK_SEM_LOCK(unit);
    if ((ret = _dal_mango_stack_devPortMap_set (unit, dstDevID, pStkPorts)) != RT_ERR_OK)
    {
        STACK_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STACK), "");
        return ret;
    }

    STACK_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_stack_devPortMap_set */

/* Function Name:
 *      dal_mango_stack_linkUp_handler_pre
 * Description:
 *      Stacking header description error prevent:
 *      1. reset stacking port if ever link changes to fix stacking description error
 *      2. enable RX_EN to avoid stacking description error
 *      3. disable serdes RX to avoid fragment packet received
 * Input:
 *      unit           - unit id
 *      port           - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32 dal_mango_stack_linkUp_handler_pre(int32 unit, rtk_port_t port)
{
    int32 ret;
    uint32  idx = 0, val = NON_DEF_PORT_IDX;
    rtk_enable_t rx_en;
    uint32 sds, asds;


    STACK_SEM_LOCK(unit);
    /* Break LINK UP status */

    idx = stackPortId2Idx[unit][port];

    if(NON_DEF_PORT_IDX != idx)
    {
        if ((ret = reg_array_field_write(unit,
                  MANGO_STK_PORT_ID_CTRLr,
                  REG_ARRAY_INDEX_NONE,
                  idx,
                  MANGO_STK_PORT_IDf,
                  &val)) != RT_ERR_OK)
        {
            STACK_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_STACK), "");
            return ret;
        }
        if ((ret = reg_array_field_write(unit,
                  MANGO_STK_PORT_ID_CTRLr,
                  REG_ARRAY_INDEX_NONE,
                  idx,
                  MANGO_STK_PORT_IDf,
                  &port)) != RT_ERR_OK)
        {
            STACK_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_STACK), "");
            return ret;
        }

        rx_en = 1; // if RX_EN = 0 and a packet is received on stacking port, stacking header description would malfunction
        RT_ERR_HDL(reg_array_field_write(unit, MANGO_MAC_L2_PORT_CTRLr,
                port, REG_ARRAY_INDEX_NONE, MANGO_RX_ENf, &rx_en), ERR, ret);

        sds = HWP_PORT_SDSID(unit, port);
        RT_ERR_HDL(drv_rtl9310_sds2AnaSds_get(unit, sds, &asds), ERR, ret);

        val = 1; //disable serdes RX to avoid MAC release receving a fragment packet
        RT_ERR_HDL(SDS_FIELD_W(unit, asds, 0x6, 0x1, 7, 7, val), ERR, ret);
    }
ERR:
    STACK_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_stack_linkUp_handler_post
 * Description:
 *      Avoid stacking header description error:
 *      1. enable serdes RX after MAC force down released to avoid a fragment packet is received when MAC force link is released
 * Input:
 *      unit           - unit id
 *      port           - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32 dal_mango_stack_linkUp_handler_post(int32 unit, rtk_port_t port)
{
    int32 ret;
    uint32  idx = 0, val = NON_DEF_PORT_IDX;
    uint32 sds, asds;


    STACK_SEM_LOCK(unit);
    /* Break LINK UP status */

    idx = stackPortId2Idx[unit][port];

    if(NON_DEF_PORT_IDX != idx)
    {
        sds = HWP_PORT_SDSID(unit, port);
        RT_ERR_HDL(drv_rtl9310_sds2AnaSds_get(unit, sds, &asds), ERR, ret);

        val = 0; //enable serdes RX after MAC force release to avoid a fragment packet is received
        RT_ERR_HDL(SDS_FIELD_W(unit, asds, 0x6, 0x1, 7, 7, val), ERR, ret);
    }
ERR:
    STACK_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}


int32
_dal_mango_stack_nonUcastBlockPort_get (uint32 unit, uint32 srcDevID, rtk_portmask_t *pBlockStkPorts)
{
    int32 ret;
    uint32 val;
    uint32 idx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STACK), "unit=%d", unit);

    /* parameter check */
    RT_PARAM_CHK(srcDevID >= HAL_MAX_NUM_OF_STACK_DEV(unit), RT_ERR_STACK_DEV_ID);
    RT_PARAM_CHK((NULL == pBlockStkPorts), RT_ERR_NULL_POINTER);

    /* get entry from CHIP*/
    if ((ret = reg_array_field_read(unit,
                          MANGO_STK_NON_UNICAST_BLOCK_CTRLr,
                          REG_ARRAY_INDEX_NONE,
                          srcDevID,
                          MANGO_NON_UNICAST_BLOCK_PMf,
                          &val)) != RT_ERR_OK)
    {
         RT_ERR(ret, (MOD_DAL|MOD_STACK), "");
        return ret;
    }

    osal_memset(pBlockStkPorts, 0, sizeof(*pBlockStkPorts));
    for(idx = 0; idx < HAL_MAX_NUM_OF_STACK_PORT(unit); idx ++)
    {
        if((1 << idx) & val)
            RTK_PORTMASK_PORT_SET(*pBlockStkPorts, idx2stackPortId[unit][idx]);
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STACK), "pBlockStkPorts pmsk0=0x%x, pmsk1=0x%x",
        RTK_PORTMASK_WORD_GET(*pBlockStkPorts, 0),
        RTK_PORTMASK_WORD_GET(*pBlockStkPorts, 1));

    return RT_ERR_OK;
}   /* end of dal_mango_stack_nonUcastBlockPort_get */

/*
 * Function Declaration
 *      dal_mango_stack_nonUcastBlockPort_get
 * Description:
 *      Get the stacking ports that would block ingress and egress non-unicast packets from the specified device.
 * Input:
 *      unit                   - unit id
 *      srcDevID               - source device id
 * Output:
 *      pBlockStkPorts    - pointer buffer of blocked stacking ports
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_mango_stack_nonUcastBlockPort_get (uint32 unit, uint32 srcDevID, rtk_portmask_t *pBlockStkPorts)
{
    int32 ret;

    /* check Init status */
    RT_INIT_CHK(stack_init[unit]);

    STACK_SEM_LOCK(unit);
    /* get entry from CHIP*/
    if ((ret = _dal_mango_stack_nonUcastBlockPort_get(unit, srcDevID, pBlockStkPorts)) != RT_ERR_OK)
    {
        STACK_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STACK), "");
        return ret;
    }

    STACK_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_stack_nonUcastBlockPort_get */

int32
_dal_mango_stack_nonUcastBlockPort_set (uint32 unit, uint32 srcDevID, rtk_portmask_t *pBlockStkPorts)
{
    int32 ret;
    uint32  val;
    rtk_port_t port;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STACK), "unit=%d, pBlockStkPorts pmsk0=0x%x, pmsk1=0x%x", unit,
        RTK_PORTMASK_WORD_GET(*pBlockStkPorts, 0),
        RTK_PORTMASK_WORD_GET(*pBlockStkPorts, 1));

    /* parameter check */
    RT_PARAM_CHK(srcDevID >= HAL_MAX_NUM_OF_STACK_DEV(unit), RT_ERR_STACK_DEV_ID);
    RT_PARAM_CHK((NULL == pBlockStkPorts), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(hwp_portmaskValid_Chk(unit, pBlockStkPorts), RT_ERR_PORT_MASK);

    val = 0;
    RTK_PORTMASK_SCAN(*pBlockStkPorts, port)
    {
        if(NON_DEF_PORT_IDX != stackPortId2Idx[unit][port])
        {
            val |= 1 << stackPortId2Idx[unit][port];
        }
        else
        {
            RT_ERR(RT_ERR_PORT_ID, (MOD_DAL|MOD_STACK), "");
            return RT_ERR_STACK_PORT_ID;
        }
    }

    /* get entry from CHIP*/
    if ((ret = reg_array_field_write(unit,
                          MANGO_STK_NON_UNICAST_BLOCK_CTRLr,
                          REG_ARRAY_INDEX_NONE,
                          srcDevID,
                          MANGO_NON_UNICAST_BLOCK_PMf,
                          &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_STACK), "");
        return ret;
    }


    return RT_ERR_OK;
}   /* end of dal_mango_stack_nonUcastBlockPort_set */

/* Function Name:
 *      dal_mango_stack_nonUcastBlockPort_set
 * Description:
 *      Set the stacking ports that would block ingress and egress non-ucast packets to the specified device.
 * Input:
 *      unit                   - unit id
 *      srcDevID               - source unit id
 *      pBlockStkPorts    - pointer buffer of blocked stacking ports
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) Blockable stacking ports in 9310 ranges from 0 to 55.
 */
int32
dal_mango_stack_nonUcastBlockPort_set (uint32 unit, uint32 srcDevID, rtk_portmask_t *pBlockStkPorts)
{
    int32 ret;

    /* check Init status */
    RT_INIT_CHK(stack_init[unit]);

    STACK_SEM_LOCK(unit);
    if ((ret = _dal_mango_stack_nonUcastBlockPort_set (unit, srcDevID, pBlockStkPorts)) != RT_ERR_OK)
    {
        STACK_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STACK), "");
        return ret;
    }

    STACK_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_stack_nonUcastBlockPort_set */


/* Function Name:
 *      dal_mango_stack_rmtIntrTxEnable_get
 * Description:
 *      Get enable status of Remote Interrupt Notification transmission.
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - pointer to enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_mango_stack_rmtIntrTxEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STACK), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(stack_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    STACK_SEM_LOCK(unit);

    if ((ret = reg_array_field_read(unit, MANGO_RMT_INTR_CTRLr, \
        REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, MANGO_TX_ENf, pEnable)) != RT_ERR_OK)
    {
        STACK_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STACK), "");
        return ret;
    }

    STACK_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_stack_rmtIntrTxEnable_get */

/* Function Name:
 *      dal_mango_stack_rmtIntrTxEnable_set
 * Description:
 *      Set enable status of Remote Interrupt Notification transmission.
 * Input:
 *      unit   - unit id
 *      enable - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
dal_mango_stack_rmtIntrTxEnable_set(uint32 unit, rtk_enable_t enable)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STACK), "unit=%d,enable=%d", unit, enable);

    /* check Init status */
    RT_INIT_CHK(stack_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    /* function body */
    STACK_SEM_LOCK(unit);

    if ((ret = reg_array_field_write(unit, MANGO_RMT_INTR_CTRLr, \
        REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, MANGO_TX_ENf, &enable)) != RT_ERR_OK)
    {
        STACK_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STACK), "");
        return ret;
    }

    STACK_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_stack_rmtIntrTxEnable_set */

/* Function Name:
 *      dal_mango_stack_rmtIntrTxTriggerEnable_get
 * Description:
 *      Get enable status of Remote Interrupt Notification transmission trigger.
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - pointer to enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_mango_stack_rmtIntrTxTriggerEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STACK), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(stack_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    STACK_SEM_LOCK(unit);

    if ((ret = reg_array_field_read(unit, MANGO_RMT_INTR_CTRLr, \
        REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, MANGO_TX_TRIGf, pEnable)) != RT_ERR_OK)
    {
        STACK_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STACK), "");
        return ret;
    }

    STACK_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_stack_rmtIntrTxTriggerEnable_get */

/* Function Name:
 *      dal_mango_stack_rmtIntrTxTriggerEnable_set
 * Description:
 *      Set enable status of Remote Interrupt Notification transmission trigger.
 * Input:
 *      unit   - unit id
 *      enable - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      The enable status will be clear automatically once the transmission has been done.
 */
int32
dal_mango_stack_rmtIntrTxTriggerEnable_set(uint32 unit, rtk_enable_t enable)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STACK), "unit=%d,enable=%d", unit, enable);

    /* check Init status */
    RT_INIT_CHK(stack_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    /* function body */
    STACK_SEM_LOCK(unit);

    if ((ret = reg_array_field_write(unit, MANGO_RMT_INTR_CTRLr, \
        REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, MANGO_TX_TRIGf, &enable)) != RT_ERR_OK)
    {
        STACK_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STACK), "");
        return ret;
    }

    STACK_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_stack_rmtIntrTxTriggerEnable_set */

/* Function Name:
 *      dal_mango_stack_rmtIntrRxSeqCmpMargin_get
 * Description:
 *      Get the comparing margin of the sequence ID of receiving Remote Interrupt Notification.
 * Input:
 *      unit    - unit id
 * Output:
 *      pMargin - pointer to margin value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_mango_stack_rmtIntrRxSeqCmpMargin_get(uint32 unit, int32 *pMargin)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STACK), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(stack_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pMargin), RT_ERR_NULL_POINTER);

    /* function body */
    STACK_SEM_LOCK(unit);

    if ((ret = reg_array_field_read(unit, MANGO_RMT_INTR_CTRLr, \
        REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, MANGO_CMP_SEQ_MARGINf, (uint32*)pMargin)) != RT_ERR_OK)
    {
        STACK_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STACK), "");
        return ret;
    }

    STACK_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_stack_rmtIntrRxSeqCmpMargin_get */

/* Function Name:
 *      dal_mango_stack_rmtIntrRxSeqCmpMargin_set
 * Description:
 *      Set the comparing margin of the sequence ID of receiving Remote Interrupt Notification.
 * Input:
 *      unit   - unit id
 *      margin - margin value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      None
 */
int32
dal_mango_stack_rmtIntrRxSeqCmpMargin_set(uint32 unit, int32 margin)
{
    int32   ret;
    uint32  value = (uint32)margin;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STACK), "unit=%d,margin=%d", unit, margin);

    /* check Init status */
    RT_INIT_CHK(stack_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((margin > DAL_MANGO_STACK_RMT_INTR_CMP_SEQ_MARGIN_MAX), RT_ERR_OUT_OF_RANGE);

    /* function body */
    STACK_SEM_LOCK(unit);

    if ((ret = reg_array_field_write(unit, MANGO_RMT_INTR_CTRLr, \
        REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, MANGO_CMP_SEQ_MARGINf, &value)) != RT_ERR_OK)
    {
        STACK_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STACK), "");
        return ret;
    }

    STACK_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_stack_rmtIntrRxSeqCmpMargin_set */

/* Function Name:
 *      dal_mango_stack_rmtIntrRxForceUpdateEnable_get
 * Description:
 *      Get the force enable status of updating when receives a Remote Interrupt Notification.
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - pointer to enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_mango_stack_rmtIntrRxForceUpdateEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STACK), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(stack_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    STACK_SEM_LOCK(unit);

    if ((ret = reg_array_field_read(unit, MANGO_RMT_INTR_CTRLr, \
        REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, MANGO_RX_FORCE_UPDATEf, pEnable)) != RT_ERR_OK)
    {
        STACK_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STACK), "");
        return ret;
    }

    STACK_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_stack_rmtIntrRxForceUpdateEnable_get */

/* Function Name:
 *      dal_mango_stack_rmtIntrRxForceUpdateEnable_set
 * Description:
 *      Set the force enable status of updating when receives a Remote Interrupt Notification.
 * Input:
 *      unit   - unit id
 *      enable - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      The enable status will be clear automatically once the updating has been done.
 */
int32
dal_mango_stack_rmtIntrRxForceUpdateEnable_set(uint32 unit, rtk_enable_t enable)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STACK), "unit=%d,enable=%d", unit, enable);

    /* check Init status */
    RT_INIT_CHK(stack_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    /* function body */
    STACK_SEM_LOCK(unit);

    if ((ret = reg_array_field_write(unit, MANGO_RMT_INTR_CTRLr, \
        REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, MANGO_RX_FORCE_UPDATEf, &enable)) != RT_ERR_OK)
    {
        STACK_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STACK), "");
        return ret;
    }

    STACK_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_stack_rmtIntrRxForceUpdateEnable_set */

/* Function Name:
 *      dal_mango_stack_rmtIntrInfo_get
 * Description:
 *      Get the information about Remote Interrupt Notification.
 * Input:
 *      unit  - unit id
 * Output:
 *      pInfo - pointer to information
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_mango_stack_rmtIntrInfo_get(uint32 unit, rtk_stack_rmtIntrInfo_t *pInfo)
{
    int32   ret;
    uint32  unit_id;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STACK), "unit=%d,pInfo=%p", unit, pInfo);

    /* check Init status */
    RT_INIT_CHK(stack_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pInfo), RT_ERR_NULL_POINTER);

    /* function body */
    STACK_SEM_LOCK(unit);

    osal_memset(pInfo, 0x00, sizeof(rtk_stack_rmtIntrInfo_t));

    for (unit_id=0; unit_id<RTK_MAX_NUM_OF_UNIT; unit_id++)
    {
        /* get info for each unit */
        if ((ret = reg_array_field_read(unit, MANGO_RMT_INTR_INFOr, \
            REG_ARRAY_INDEX_NONE, unit_id, MANGO_RX_SEQ_IDf, &value)) != RT_ERR_OK)
        {
            STACK_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_STACK), "");
            return ret;
        }
        pInfo->unit[unit_id].last_rx_seq_id = value;

        if ((ret = reg_array_field_read(unit, MANGO_RMT_INTR_INFOr, \
            REG_ARRAY_INDEX_NONE, unit_id, MANGO_SEQ_IDf, &value)) != RT_ERR_OK)
        {
            STACK_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_STACK), "");
            return ret;
        }
        pInfo->unit[unit_id].rx_seq_id = value;

        if ((ret = reg_array_field_read(unit, MANGO_RMT_INTR_DATA_CCMr, \
            REG_ARRAY_INDEX_NONE, unit_id, MANGO_ISR_CCMf, &value)) != RT_ERR_OK)
        {
            STACK_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_STACK), "");
            return ret;
        }
        pInfo->unit[unit_id].isr_ccm = value;

        /* portmask */
        if ((ret = reg_array_field_read(unit, MANGO_RMT_INTR_DATA_LINK_STSr, \
            REG_ARRAY_INDEX_NONE, unit_id, MANGO_PORT_LINK_STS_LOf, &value)) != RT_ERR_OK)
        {
            STACK_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_STACK), "");
            return ret;
        }
        pInfo->unit[unit_id].port_link_sts.bits[0] = value;

        if ((ret = reg_array_field_read(unit, MANGO_RMT_INTR_DATA_LINK_STSr, \
            REG_ARRAY_INDEX_NONE, unit_id, MANGO_PORT_LINK_STS_HIf, &value)) != RT_ERR_OK)
        {
            STACK_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_STACK), "");
            return ret;
        }
        pInfo->unit[unit_id].port_link_sts.bits[1] = value;
    }

    STACK_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_stack_rmtIntrInfo_get */

/* Function Name:
 *      dal_mango_stack_shrink_get
 * Description:
 *      Get the enable status of shrink type for stacking
 * Input:
 *      unit          - unit id
 *      shrinkType    - control type
 * Output:
 *      pVal          - status of shrink type
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      None
 */
int32
dal_mango_stack_shrink_get(uint32 unit, rtk_stack_shrinkCtrlType_t shrinkType, uint32 *pVal)
{
    int32   ret;
    uint32  enable;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STACK), "unit=%d,shrinkType=%d", unit, shrinkType);


    /* check Init status */
    RT_INIT_CHK(stack_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pVal), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTK_STACK_SHRINK_ALL_EN != shrinkType), RT_ERR_INPUT);

    /* function body */
    STACK_SEM_LOCK(unit);

    if ((ret = reg_field_read(unit,
                          MANGO_STK_DBG_CTRLr,
                          MANGO_STK_PORT_DEBUGf,
                          &enable)) != RT_ERR_OK)
    {
        STACK_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STACK), "");
        return ret;
    }

    STACK_SEM_UNLOCK(unit);

    if (enable == 1)
    {
        *pVal = DISABLED;
    }
    else
    {
        *pVal = ENABLED;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STACK), "*pVal = ", *pVal);

    return RT_ERR_OK;
}   /* end of dal_mango_stack_shrink_get */

/* Function Name:
 *      dal_mango_stack_shrink_set
 * Description:
 *      Set the enable status of shrink type for stacking
 * Input:
 *      unit        - unit id
 *      shrinkType  - control type
 *      val         - status of shrink type
 * Output:
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      None
 */
int32
dal_mango_stack_shrink_set(uint32 unit, rtk_stack_shrinkCtrlType_t shrinkType, uint32 val)
{
    int32   ret;
    uint32  enable;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STACK), "unit=%d,shrinkType=%d,val=%d", unit, shrinkType, val);

    /* check Init status */
    RT_INIT_CHK(stack_init[unit]);


    /* parameter check */
    RT_PARAM_CHK((RTK_STACK_SHRINK_ALL_EN != shrinkType), RT_ERR_INPUT);

    if (val == ENABLED)
    {
        enable = 0;
    }
    else
    {
        enable = 1;
    }

    /* function body */
    STACK_SEM_LOCK(unit);

    if ((ret = reg_field_write(unit,
                          MANGO_STK_DBG_CTRLr,
                          MANGO_STK_PORT_DEBUGf,
                          &enable)) != RT_ERR_OK)
    {
        STACK_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STACK), "");
        return ret;
    }

    STACK_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_stack_shrink_set */


/* Function Name:
 *      _dal_mango_stack_init_config
 * Description:
 *      Initialize default configuration for stack module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize stack module before calling this API
 */
static int32
_dal_mango_stack_init_config(uint32 unit)
{
    int32   ret;
    uint32 devID;

    devID = HAL_UNIT_TO_DEV_ID(unit);

    if ((ret = dal_mango_stack_devId_set (unit, devID)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_TRUNK|MOD_DAL), "");
        return ret;
    }

    if ((ret = dal_mango_stack_masterDevId_set (unit, devID)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_TRUNK|MOD_DAL), "");
        return ret;
    }

    if ((ret = dal_mango_stack_loopGuard_set (unit, RTK_DEFAULT_STACK_LOOP_GUARD)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_TRUNK|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of _dal_mango_trunk_init_config */

