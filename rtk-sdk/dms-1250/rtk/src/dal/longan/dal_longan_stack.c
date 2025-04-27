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
#include <osal/time.h>
#include <hal/chipdef/allmem.h>
#include <hal/chipdef/allreg.h>
#include <hal/chipdef/longan/rtk_longan_table_struct.h>
#include <hal/chipdef/longan/rtk_longan_reg_struct.h>
#include <hal/mac/reg.h>
#include <hal/mac/mem.h>
#include <hal/common/halctrl.h>
#include <dal/longan/dal_longan_stack.h>
#include <dal/longan/dal_longan_l2.h>
#include <dal/longan/dal_longan_port.h>
#include <dal/longan/dal_longan_trunk.h>
#include <dal/longan/dal_longan_flowctrl.h>
#include <dal/longan/dal_longan_qos.h>

#include <rtk/default.h>
#include <rtk/stack.h>

#include <hal/phy/phy_rtl9300.h>


/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
static uint32                   stack_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};
static osal_mutex_t             stack_sem[RTK_MAX_NUM_OF_UNIT];
static rtk_qos_scheduling_type_t ori_type[RTK_MAX_NUM_OF_UNIT][RTL9300_UPLINK_PORT_END - RTL9300_UPLINK_PORT_START + 1] = {{WFQ}};

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


#define RTK_STACK_CASCADE_PORTMASK ((1<<24) | (1<<25))
#define RTL9300_STACK_PORT_NUM      (RTL9300_UPLINK_PORT_END - RTL9300_UPLINK_PORT_START + 1)
#define LONGAN_PORT_STACK_DELAY     (10)

/*
 * Function Declaration
 */

static int32 _dal_longan_stack_init_config(uint32 unit);
static int32 _dal_longan_stack_cascadeMode_init_config(uint32 unit, rtk_stack_cascadeCfg_t *pCascadeCfg);

/* Function Name:
 *      dal_longan_stackMapper_init
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
dal_longan_stackMapper_init(dal_mapper_t *pMapper)
{
    pMapper->stack_init = dal_longan_stack_init;
    pMapper->stack_cascade_init = dal_longan_stack_cascadeMode_init;
    pMapper->stack_port_get = dal_longan_stack_port_get;
    pMapper->stack_port_set = dal_longan_stack_port_set;
    pMapper->stack_devId_get = dal_longan_stack_devId_get;
    pMapper->stack_devId_set = dal_longan_stack_devId_set;
    pMapper->stack_masterDevId_get = dal_longan_stack_masterDevId_get;
    pMapper->stack_masterDevId_set = dal_longan_stack_masterDevId_set;
    pMapper->stack_loopGuard_get = dal_longan_stack_loopGuard_get;
    pMapper->stack_loopGuard_set = dal_longan_stack_loopGuard_set;
    pMapper->stack_devPortMap_get = dal_longan_stack_devPortMap_get;
    pMapper->stack_devPortMap_set = dal_longan_stack_devPortMap_set;
    pMapper->stack_nonUcastBlockPort_get = dal_longan_stack_nonUcastBlockPort_get;
    pMapper->stack_nonUcastBlockPort_set = dal_longan_stack_nonUcastBlockPort_set;
    pMapper->stack_shrink_get = dal_longan_stack_shrink_get;
    pMapper->stack_shrink_set = dal_longan_stack_shrink_set;

    return RT_ERR_OK;
}

/* Function Name:
 *      _dal_longan_stack_port_del
 * Description:
 *      Del stacking ports from the specified device.
 * Input:
 *      unit - unit id
 *      port - pointer buffer of stacking ports
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      In 9300, only port 24~27 can be configured as stacking ports.
 */
int32
_dal_longan_stack_port_del (uint32 unit, rtk_port_t port)
{
    int32 ret;
    uint32  val;
    rtk_portmask_t cur_stk_pmsk;

    /* check Init status */
    RT_INIT_CHK(stack_init[unit]);


    STACK_SEM_LOCK(unit);
    if (HAL_STACK_PORT(unit, port))
    {
        val = 0;
        HAL_STACK_PORTMASK_GET(unit, &cur_stk_pmsk);
        RTK_PORTMASK_PORT_CLEAR(cur_stk_pmsk,port);
        rt_util_upinkPort_mask2Reg(unit, &cur_stk_pmsk, &val);

        /* set value to CHIP*/
        if ((ret = reg_field_write(unit,
                              LONGAN_STK_GLB_CTRLr,
                              LONGAN_STK_PORT_SELf,
                              &val)) != RT_ERR_OK)
        {
            STACK_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_STACK), "");
            return ret;
        }
    }
    osal_time_mdelay(LONGAN_PORT_STACK_DELAY);

    STACK_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_longan_stack_port_set */

/* Function Name:
 *      _dal_longan_stack_port_add
 * Description:
 *      Add stacking ports to the specified device.
 * Input:
 *      unit - unit id
 *      port - pointer buffer of stacking ports
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      In 9300, only port 24~27 can be configured as stacking ports.
 */
int32
_dal_longan_stack_port_add (uint32 unit, rtk_port_t port)
{
    int32 ret;
    uint32  val;
    rtk_portmask_t cur_stk_pmsk;

    /* check Init status */
    RT_INIT_CHK(stack_init[unit]);


    STACK_SEM_LOCK(unit);
    if (HAL_STACK_PORT(unit, port))
    {
        val = 0;
        HAL_STACK_PORTMASK_GET(unit, &cur_stk_pmsk);
        RTK_PORTMASK_PORT_SET(cur_stk_pmsk,port);
        rt_util_upinkPort_mask2Reg(unit, &cur_stk_pmsk, &val);

        /* set value to CHIP*/
        if ((ret = reg_field_write(unit,
                              LONGAN_STK_GLB_CTRLr,
                              LONGAN_STK_PORT_SELf,
                              &val)) != RT_ERR_OK)
        {
            STACK_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_STACK), "");
            return ret;
        }
    }
    osal_time_mdelay(LONGAN_PORT_STACK_DELAY);
    STACK_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_longan_stack_port_set */

/* Function Name:
 *      dal_longan_stack_init
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
dal_longan_stack_init(uint32 unit)
{
    int32   ret;

    RT_INIT_REENTRY_CHK(stack_init[unit]);
    stack_init[unit] = INIT_NOT_COMPLETED;

    /* create semaphore */
    stack_sem[unit] = osal_sem_mutex_create();
    if (0 == stack_sem[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_STACK), "semaphore create failed");
        return RT_ERR_FAILED;
    }

    /* set init flag to complete init */
    stack_init[unit] = INIT_COMPLETED;

    if (HWP_UNIT_VALID_LOCAL(unit))
    {
        /* initialize default configuration */
        if ((ret = _dal_longan_stack_init_config(unit)) != RT_ERR_OK)
        {
            stack_init[unit] = INIT_NOT_COMPLETED;

            RT_ERR(ret, (MOD_STACK|MOD_DAL), "init default configuration failed");
            return ret;
        }
    }

    return RT_ERR_OK;
}   /* end of dal_longan_stack_init */

/* Function Name:
 *      dal_longan_stack_cascadeMode_init
 * Description:
 *      Initialize cascade mode.
 * Input:
 *      unit  - unit id
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
dal_longan_stack_cascadeMode_init(uint32 unit)
{
    int32 ret;
    rtk_stack_cascadeCfg_t cascadeCfg;

    /* check Init status */
    RT_INIT_CHK(stack_init[unit]);

    if(HWP_CASCADE_MODE()==FALSE)
        return RT_ERR_OK;

    if((RTL9301_CHIP_ID_24G == HWP_CHIP_ID(unit)) && (1==HWP_CHIP_REV(unit)) )
    {
        cascadeCfg.e2e_fc_en = DISABLED;
        cascadeCfg.e2e_fc_normalPort_en = DISABLED;
        cascadeCfg.e2e_ntfy_en = DISABLED;
    }
    else if((RTL9301_CHIP_ID_24G == HWP_CHIP_ID(unit)) && (2==HWP_CHIP_REV(unit)) )
    {
        cascadeCfg.e2e_fc_en = ENABLED;
        cascadeCfg.e2e_fc_normalPort_en = ENABLED;
        cascadeCfg.e2e_ntfy_en = DISABLED;
    }
    else
    {
        cascadeCfg.e2e_fc_en = ENABLED;
        cascadeCfg.e2e_fc_normalPort_en = ENABLED;
        cascadeCfg.e2e_ntfy_en = ENABLED;
    }


    /* initialize default configuration */
    if ((ret = _dal_longan_stack_cascadeMode_init_config(unit, &cascadeCfg)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_STACK|MOD_DAL), "init Cascade configuration failed");
        return ret;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_longan_stack_cascadeMode_init
 * Description:
 *      Initialize cascade mode.
 * Input:
 *      unit  - unit id
 * Output:
 *      pCascadeCfg - pointer to cascade configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
static int32
_dal_longan_stack_cascadeMode_init_config(uint32 unit, rtk_stack_cascadeCfg_t *pCascadeCfg)
{
    int32 ret;
    uint32  val;
    uint32 port;
    uint32 devID;
    uint32 masterDevID;
    uint32 slaveDevID;
    uint32 reg_field;
    rtk_portmask_t cascadePortmask;


    osal_memset(&cascadePortmask, 0, sizeof(rtk_portmask_t));
    HWP_GET_ATTRI_PORTMASK(unit, HWP_CASCADE_ID, cascadePortmask);

    masterDevID = HAL_UNIT_TO_DEV_ID(HWP_MY_UNIT_ID());
    slaveDevID = HAL_UNIT_TO_DEV_ID(HWP_CASCADE_SLAVE_UNIT_ID());

    devID = HAL_UNIT_TO_DEV_ID(unit);

    /*unit configuration*/
    if ((ret = dal_longan_stack_devId_set(unit, devID)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_STACK|MOD_DAL), "");
        return ret;
    }

    if ((ret = dal_longan_stack_masterDevId_set(unit, masterDevID)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_STACK|MOD_DAL), "");
        return ret;
    }

    /*stacking port & trunk configuration*/
    if ((ret = dal_longan_stack_port_set(unit, &cascadePortmask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_STACK|MOD_DAL), "");
        return ret;
    }

    if ((ret = dal_longan_trunk_stkTrkPort_set(unit, 0, &cascadePortmask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_STACK|MOD_DAL), "");
        return ret;
    }

    if ((ret = dal_longan_trunk_distributionAlgorithmTypeParam_set(unit, PARAM_TYPE_L2, 0, 0x1)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_STACK|MOD_DAL), "");
        return ret;
    }

    if ((ret = dal_longan_trunk_distributionAlgorithmTypeParam_set(unit, PARAM_TYPE_L3, 0, 0x1)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_STACK|MOD_DAL), "");
        return ret;
    }

    if ((ret = dal_longan_trunk_stkTrkHash_set(unit, STACK_TRK_HASH_RECALCULATE)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_STACK|MOD_DAL), "");
        return ret;
    }

    if ((ret = dal_longan_stack_devPortMap_set(unit, (devID==masterDevID ? slaveDevID : masterDevID), &cascadePortmask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_STACK|MOD_DAL), "");
        return ret;
    }

    /*Enable stacking Auto Learn*/
    if ((ret = dal_longan_l2_stkLearningEnable_set(unit, ENABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_STACK|MOD_DAL), "");
        return ret;
    }

    STACK_SEM_LOCK(unit);

    /*stacking port FC disable*/
    for(port=24; port<28; port++)
    {
        if(!(cascadePortmask.bits[0] & (1<<port)))
            continue;

        val=1;
        if ((ret = reg_array_field_write(unit, LONGAN_MAC_FORCE_MODE_CTRLr, port, REG_ARRAY_INDEX_NONE, LONGAN_MAC_FORCE_FC_ENf, &val)) != RT_ERR_OK)
        {
            STACK_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            return ret;
        }

        val=0;
        if ((ret = reg_array_field_write(unit, LONGAN_MAC_FORCE_MODE_CTRLr, port, REG_ARRAY_INDEX_NONE, LONGAN_RX_PAUSE_ENf, &val)) != RT_ERR_OK)
        {
            STACK_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            return ret;
        }
        if ((ret = reg_array_field_write(unit, LONGAN_MAC_FORCE_MODE_CTRLr, port, REG_ARRAY_INDEX_NONE, LONGAN_TX_PAUSE_ENf, &val)) != RT_ERR_OK)
        {
            STACK_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            return ret;
        }

        if(HWP_PORT_ETH(unit, port) == HWP_SXGE)
        {
            val = 1;
            reg_field = LONGAN_P24_IN_SXG_MODEf;
            if(port==24)
                reg_field = LONGAN_P24_IN_SXG_MODEf;
            else if(port==25)
                reg_field = LONGAN_P25_IN_SXG_MODEf;


            if(port==24 || port==25)
            {
                if ((ret = reg_field_write(unit, LONGAN_EGBW_RATE_SXG_CTRLr, reg_field, &val)) != RT_ERR_OK)
                {
                    STACK_SEM_UNLOCK(unit);
                    RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
                    return ret;
                }
            }
        }
    }


    val = 0;
    rt_util_upinkPort_mask2Reg(unit, &cascadePortmask, &val);
    if ((ret = reg_field_write(unit, LONGAN_ETE_FC_CTRLr, LONGAN_CASCADE_PORTMASKf, &val)) != RT_ERR_OK)
    {
        STACK_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STACK), "");
        return ret;
    }

    val = pCascadeCfg->e2e_fc_en==ENABLED ? 1 : 0;
    if ((ret = reg_field_write(unit, LONGAN_ETE_FC_CTRLr, LONGAN_ENf, &val)) != RT_ERR_OK)
    {
        STACK_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STACK), "");
        return ret;
    }

    val = pCascadeCfg->e2e_fc_normalPort_en==ENABLED ? 1 : 0;
    if ((ret = reg_field_write(unit, LONGAN_ETE_FC_CTRLr, LONGAN_NRM_PORT_ETE_FC_ENf, &val)) != RT_ERR_OK)
    {
        STACK_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STACK), "");
        return ret;
    }

    val = pCascadeCfg->e2e_ntfy_en==ENABLED ? 1 : 0;
    if ((ret = reg_field_write(unit, LONGAN_L2_NTFY_PKT_REMOTEL_THRr, LONGAN_REMOTE_BACK_PRESSf, &val)) != RT_ERR_OK)
    {
        STACK_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STACK), "");
        return ret;
    }
    val = 0xf;
    if ((ret = reg_field_write(unit, LONGAN_L2_NTFY_PKT_REMOTEL_THRr, LONGAN_ONf, &val)) != RT_ERR_OK)
    {
        STACK_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STACK), "");
        return ret;
    }
    val = 0x8;
    if ((ret = reg_field_write(unit, LONGAN_L2_NTFY_PKT_REMOTEL_THRr, LONGAN_OFFf, &val)) != RT_ERR_OK)
    {
        STACK_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STACK), "");
        return ret;
    }

    /*cascade Master/Slave Mode configuration*/
    val =(devID==masterDevID ? 0: 1);
    if ((ret = reg_field_write(unit, LONGAN_STK_CASCADE_CTRLr, LONGAN_CASCADE_SLAVE_MODEf, &val)) != RT_ERR_OK)
    {
        STACK_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STACK), "");
        return ret;
    }
    val = masterDevID;
    if ((ret = reg_field_write(unit, LONGAN_STK_CASCADE_CTRLr, LONGAN_CASCADE_MASTER_IDf, &val)) != RT_ERR_OK)
    {
        STACK_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STACK), "");
        return ret;
    }

#ifndef __BOOTLOADER__
    /* Enable Ext CPU Interrupt*/
    if(unit == HWP_CASCADE_SLAVE_UNIT_ID())
    {
        val = 1;
        if ((ret = reg_field_write(unit, LONGAN_IMR_GLBr, LONGAN_IMR_EXT_CPUf, &val)) != RT_ERR_OK)
        {
            STACK_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_STACK), "");
            return ret;
        }
        val = 3;
        if ((ret = reg_field_write(unit, LONGAN_ISR_SW_INT_MODEr, LONGAN_SW_INT_MODEf, &val)) != RT_ERR_OK)
        {
            STACK_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_STACK), "");
            return ret;
        }
        val = 3;
        if ((ret = reg_field_write(unit, LONGAN_ISR_SW_INT_MODEr, LONGAN_SW_INT_PULSE_INTERVALf, &val)) != RT_ERR_OK)
        {
            STACK_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_STACK), "");
            return ret;
        }
    }
#endif

    STACK_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}


/* Module Name    : Trunk                    */
/* Sub-module Name: User configuration stack */

/* Function Name:
 *      dal_longan_stack_port_get
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
dal_longan_stack_port_get (uint32 unit, rtk_portmask_t *pStkPorts)
{
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STACK), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(stack_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pStkPorts), RT_ERR_NULL_POINTER);

    STACK_SEM_LOCK(unit);

    HAL_STACK_PORTMASK_GET(unit, pStkPorts);

    STACK_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STACK), "pStkPorts->bits=0x%x", pStkPorts->bits[0]);

    return RT_ERR_OK;
}   /* end of dal_longan_stack_port_get */

/* Function Name:
 *      dal_longan_stack_port_set
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
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      In 9300, only port 24~27 can be configured as stacking ports.
 */
int32
dal_longan_stack_port_set (uint32 unit, rtk_portmask_t *pStkPorts)
{
    int32 ret;
    uint32  val;
#if defined(__BOOTLOADER__)
    uint32  temp;
#endif
    uint32  i;
    uint32  stk_portmask;
    rtk_enable_t enable;
    rtk_portmask_t cur_stk_pmsk;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STACK), "unit=%d pStkPorts->bits=0x%x", unit,
        pStkPorts->bits[0]);

    /* check Init status */
    RT_INIT_CHK(stack_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PMSK_EXIST_ATTRI(unit, pStkPorts, HWP_UPLINK, HWP_OPERATION_OR), RT_ERR_PORT_MASK);

    /* SS-822 Before change stack port, keep port admin status is disabled */
    RTK_PORTMASK_RESET(cur_stk_pmsk);
    HAL_STACK_PORTMASK_GET(unit, &cur_stk_pmsk);

    RTK_PORTMASK_XOR(cur_stk_pmsk,*pStkPorts);

    for(i = RTL9300_UPLINK_PORT_START; i <= RTL9300_UPLINK_PORT_END; i++)
    {
        if(RTK_PORTMASK_IS_PORT_SET(cur_stk_pmsk,i))
        {
            if ((ret = dal_longan_port_adminEnable_get(unit, i, &enable)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STACK), "");
                return ret;
            }
            if(ENABLED == enable)
            {
                return RT_ERR_OPER_DENIED;
            }

        }
    }



    STACK_SEM_LOCK(unit);

    val = 0;
    rt_util_upinkPort_mask2Reg(unit, pStkPorts, &val);

    /* get entry from CHIP*/
    if ((ret = reg_field_read(unit,
                          LONGAN_STK_GLB_CTRLr,
                          LONGAN_STK_PORT_SELf,
                          &stk_portmask)) != RT_ERR_OK)
    {
        STACK_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STACK), "");
        return ret;
    }

    /* set value to CHIP*/
    if ((ret = reg_field_write(unit,
                          LONGAN_STK_GLB_CTRLr,
                          LONGAN_STK_PORT_SELf,
                          &val)) != RT_ERR_OK)
    {
        STACK_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STACK), "");
        return ret;
    }

    /* keep stack ports to DB */
    HAL_STACK_PORTMASK_SET(unit, pStkPorts);
    osal_time_mdelay(LONGAN_PORT_STACK_DELAY);
    STACK_SEM_UNLOCK(unit);

    /*SS-899*/
    stk_portmask ^= val;
    for (i = 0; i < RTL9300_STACK_PORT_NUM; i++)
    {
        if (1 == ((stk_portmask >> i) & 0x1))
        {
            if (1 == ((val >> i) & 0x1))
            {
#if defined(__BOOTLOADER__)
                RT_ERR_CHK(reg_array_field_read(unit, LONGAN_SCHED_PORT_ALGO_CTRLr,
                                    RTL9300_UPLINK_PORT_START + i, REG_ARRAY_INDEX_NONE, LONGAN_SCHED_TYPEf, (uint32*)&ori_type[unit][i]), ret);
                temp = 1;
                RT_ERR_CHK(reg_array_field_write(unit, LONGAN_SCHED_PORT_ALGO_CTRLr,
                                    RTL9300_UPLINK_PORT_START + i, REG_ARRAY_INDEX_NONE, LONGAN_SCHED_TYPEf, &temp), ret);

#else
                RT_ERR_CHK(dal_longan_qos_schedulingAlgorithm_get(unit, RTL9300_UPLINK_PORT_START + i, &ori_type[unit][i]), ret);
                RT_ERR_CHK(dal_longan_qos_schedulingAlgorithm_set(unit, RTL9300_UPLINK_PORT_START + i, WRR), ret);
#endif
            }
            else
            {
#if defined(__BOOTLOADER__)
                RT_ERR_CHK(reg_array_field_write(unit, LONGAN_SCHED_PORT_ALGO_CTRLr,
                                    RTL9300_UPLINK_PORT_START + i, REG_ARRAY_INDEX_NONE, LONGAN_SCHED_TYPEf, (uint32*)&ori_type[unit][i]), ret);
#else
                RT_ERR_CHK(dal_longan_qos_schedulingAlgorithm_set(unit, RTL9300_UPLINK_PORT_START + i, ori_type[unit][i]), ret);
#endif
            }
        }
    }

    STACK_SEM_LOCK(unit);
    /*LONGAN-1504*/
    val = (val == 0) ? 0 : 1;
    if ((ret = reg_field_write(unit, LONGAN_DMY_REG0_QUEUEr, LONGAN_FORCE_ALL_PORT_SECOND_EGR_DROPf, &val)) != RT_ERR_OK)
    {
        STACK_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STACK), "");
        return ret;
    }
    STACK_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_longan_stack_port_set */


/* Function Name:
 *      dal_longan_stack_devId_get
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
dal_longan_stack_devId_get(uint32 unit, uint32 *pMyDevID)
{
    int32 ret;
    uint32  val = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STACK), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(stack_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pMyDevID), RT_ERR_NULL_POINTER);

    STACK_SEM_LOCK(unit);

    /* get entry from CHIP*/
    if ((ret = reg_field_read(unit,
                          LONGAN_STK_GLB_CTRLr,
                          LONGAN_MY_DEV_IDf,
                          &val)) != RT_ERR_OK)
    {
        STACK_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STACK), "");
        return ret;
    }

    STACK_SEM_UNLOCK(unit);
    *pMyDevID = val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STACK), "*pMyDevID = ", *pMyDevID);

    return RT_ERR_OK;
}   /* end of dal_longan_stack_devId_get */

/* Function Name:
 *      dal_longan_stack_devId_set
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
 *      (1) 9300 supports 16 stacked devices, thus myDevID ranges from 0 to 15.
 */
int32
dal_longan_stack_devId_set(uint32 unit, uint32 myDevID)
{
    int32 ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STACK), "unit=%d myDevID=%d", unit, myDevID);

    /* check Init status */
    RT_INIT_CHK(stack_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(myDevID >= HAL_MAX_NUM_OF_DEV(unit), RT_ERR_DEV_ID);

    STACK_SEM_LOCK(unit);

    val = myDevID;

    /* get entry from CHIP*/
    if ((ret = reg_field_write(unit,
                          LONGAN_STK_GLB_CTRLr,
                          LONGAN_MY_DEV_IDf,
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
}   /* end of dal_longan_stack_devId_set */

/* Function Name:
 *      dal_longan_stack_masterDevId_get
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
dal_longan_stack_masterDevId_get(uint32 unit, uint32 *pMasterDevID)
{
    int32 ret;
    uint32  val = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STACK), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(stack_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pMasterDevID), RT_ERR_NULL_POINTER);

    STACK_SEM_LOCK(unit);

    /* get entry from CHIP*/
    if ((ret = reg_field_read(unit,
                          LONGAN_STK_GLB_CTRLr,
                          LONGAN_MASTER_DEV_IDf,
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
}   /* end of dal_longan_stack_masterDevId_get */

/* Function Name:
 *      dal_longan_stack_masterDevId_set
 * Description:
 *      Set the master device ID to the specified device.
 * Input:
 *      unit                   - unit id
 *      masterDevID         - device ID of the master switch
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID           - invalid unit id
 *      RT_ERR_NOT_INIT          - The module is not initial
 * Note:
 *      (1) 9300 supports 16 stacked units, thus masterDevID ranges from 0 to 15.
 */
int32
dal_longan_stack_masterDevId_set(uint32 unit, uint32 masterDevID)
{
    int32 ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STACK), "unit=%d masterDevID=%d", unit, masterDevID);

    /* check Init status */
    RT_INIT_CHK(stack_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(masterDevID >= HAL_MAX_NUM_OF_DEV(unit), RT_ERR_DEV_ID);

    STACK_SEM_LOCK(unit);

    val = masterDevID;

    /* get entry from CHIP*/
    if ((ret = reg_field_write(unit,
                          LONGAN_STK_GLB_CTRLr,
                          LONGAN_MASTER_DEV_IDf,
                          &val)) != RT_ERR_OK)
    {
        STACK_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STACK), "");
        return ret;
    }

    STACK_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_longan_stack_masterDevId_set */

/* Function Name:
 *      dal_longan_stack_loopGuard_get
 * Description:
 *      Get the enable status of dropping packets with source unit ID the same as the unit ID of the switch from the specified device.
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
dal_longan_stack_loopGuard_get(uint32 unit, rtk_enable_t *pEnable)
{
    int32 ret;
    uint32  val = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STACK), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(stack_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    STACK_SEM_LOCK(unit);

    /* get entry from CHIP*/
    if ((ret = reg_field_read(unit,
                          LONGAN_STK_GLB_CTRLr,
                          LONGAN_DROP_MY_DEVf,
                          &val)) != RT_ERR_OK)
    {
        STACK_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STACK), "");
        return ret;
    }

    STACK_SEM_UNLOCK(unit);
    *pEnable = val==1 ? ENABLED : DISABLED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STACK), "*pEnable = ", *pEnable);

    return RT_ERR_OK;
}   /* end of dal_longan_stack_dropLoopPacket_get */


/* Function Name:
 *      dal_longan_stack_loopGuard_set
 * Description:
 *      Set the enable status of dropping packets with source unit ID the same as the unit ID of the switch to the specified device.
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
dal_longan_stack_loopGuard_set(uint32 unit, rtk_enable_t enable)
{
    int32 ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STACK), "unit=%d enable=%d", unit, enable);

    /* check Init status */
    RT_INIT_CHK(stack_init[unit]);

    /* parameter check */

    STACK_SEM_LOCK(unit);
    RT_PARAM_CHK(enable >= RTK_ENABLE_END, RT_ERR_INPUT);

    val = enable==ENABLED ? 1 : 0;

    /* get entry from CHIP*/
    if ((ret = reg_field_write(unit,
                          LONGAN_STK_GLB_CTRLr,
                          LONGAN_DROP_MY_DEVf,
                          &val)) != RT_ERR_OK)
    {
        STACK_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STACK), "");
        return ret;
    }

    STACK_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_longan_stack_dropLoopPacket_set */

/* Function Name:
 *      dal_longan_stack_devPortMap_get
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
dal_longan_stack_devPortMap_get (uint32 unit, uint32 dstDevID, rtk_portmask_t *pStkPorts)
{
    int32 ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STACK), "unit=%d, dstDevID=%d",
           unit, dstDevID);

    /* check Init status */
    RT_INIT_CHK(stack_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(dstDevID >= HAL_MAX_NUM_OF_DEV(unit), RT_ERR_DEV_ID);
    RT_PARAM_CHK((NULL == pStkPorts), RT_ERR_NULL_POINTER);

    STACK_SEM_LOCK(unit);
    /* get entry from CHIP*/
    if ((ret = reg_array_field_read(unit,
                          LONGAN_STK_DEV_PORT_MAP_CTRLr,
                          REG_ARRAY_INDEX_NONE,
                          dstDevID,
                          LONGAN_DEV_PORT_MAPf,
                          &val)) != RT_ERR_OK)
    {
        STACK_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STACK), "");
        return ret;
    }

    STACK_SEM_UNLOCK(unit);

    osal_memset(pStkPorts, 0, sizeof(*pStkPorts));
    rt_util_upinkPort_reg2Mask(unit, &val, pStkPorts);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STACK), "pStkPorts->bits=0x%x", pStkPorts->bits[0]);

    return RT_ERR_OK;
}   /* end of dal_longan_stack_devPortMap_get */


/* Function Name:
 *      dal_longan_stack_devPortMap_set _set
 * Description:
 *      Set the stacking ports that packets with specific target unit should forward to for the specified device.
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
 *      (1) Stacking ports in 9300 ranges from 24 to 27.
 */
int32
dal_longan_stack_devPortMap_set (uint32 unit, uint32 dstDevID, rtk_portmask_t *pStkPorts)
{
    int32 ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STACK), "unit=%d, dstDevID=%d, pStkPorts->bits=0x%x",
           unit, dstDevID, pStkPorts->bits[0]);

    /* check Init status */
    RT_INIT_CHK(stack_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(dstDevID >= HAL_MAX_NUM_OF_DEV(unit), RT_ERR_DEV_ID);
    RT_PARAM_CHK(!HWP_PMSK_EXIST_ATTRI(unit, pStkPorts, HWP_UPLINK, HWP_OPERATION_OR), RT_ERR_PORT_MASK);

    val = 0;
    rt_util_upinkPort_mask2Reg(unit, pStkPorts, &val);

    STACK_SEM_LOCK(unit);

    /* set entry to CHIP*/
    if ((ret = reg_array_field_write(unit,
                          LONGAN_STK_DEV_PORT_MAP_CTRLr,
                          REG_ARRAY_INDEX_NONE,
                          dstDevID,
                          LONGAN_DEV_PORT_MAPf,
                          &val)) != RT_ERR_OK)
    {
        STACK_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STACK), "");
        return ret;
    }

    STACK_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_longan_stack_devPortMap_set */

/*
 * Function Declaration
 *      dal_longan_stack_nonUcastBlockPort_get
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
dal_longan_stack_nonUcastBlockPort_get (uint32 unit, uint32 srcDevID, rtk_portmask_t *pBlockStkPorts)
{
    int32 ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STACK), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(stack_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pBlockStkPorts), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(srcDevID>= HAL_MAX_NUM_OF_DEV(unit), RT_ERR_DEV_ID);

    STACK_SEM_LOCK(unit);
    /* get entry from CHIP*/
    if ((ret = reg_array_field_read(unit,
                          LONGAN_STK_NON_UNICAST_BLOCK_CTRLr,
                          REG_ARRAY_INDEX_NONE,
                          srcDevID,
                          LONGAN_NON_UNICAST_BLOCK_PMf,
                          &val)) != RT_ERR_OK)
    {
        STACK_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STACK), "");
        return ret;
    }

    STACK_SEM_UNLOCK(unit);

    osal_memset(pBlockStkPorts, 0, sizeof(*pBlockStkPorts));
    rt_util_upinkPort_reg2Mask(unit, &val, pBlockStkPorts);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STACK), "pBlockStkPorts->bits=0x%x", pBlockStkPorts->bits[0]);

    return RT_ERR_OK;
}   /* end of dal_longan_stack_nonUcastBlockPort_get */


/* Function Name:
 *      dal_longan_stack_nonUcastBlockPort_set
 * Description:
 *      Set the stacking ports that would block ingress and egress non-ucast packets to the specified device.
 * Input:
 *      unit                   - unit id
 *      srcDevID               - source device id
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
 *      (1) Blockable stacking ports in 9300 ranges from 24 to 27.
 */
int32
dal_longan_stack_nonUcastBlockPort_set (uint32 unit, uint32 srcDevID, rtk_portmask_t *pBlockStkPorts)
{
    int32 ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_STACK), "unit=%d, pBlockStkPorts->bits=0x%x", unit, pBlockStkPorts->bits[0]);

    /* check Init status */
    RT_INIT_CHK(stack_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(srcDevID>= HAL_MAX_NUM_OF_DEV(unit), RT_ERR_DEV_ID);
    RT_PARAM_CHK(!HWP_PMSK_EXIST_ATTRI(unit, pBlockStkPorts, HWP_UPLINK, HWP_OPERATION_OR), RT_ERR_PORT_MASK);

    val = 0;
    rt_util_upinkPort_mask2Reg(unit, pBlockStkPorts, &val);

    STACK_SEM_LOCK(unit);
    /* get entry from CHIP*/
    if ((ret = reg_array_field_write(unit,
                          LONGAN_STK_NON_UNICAST_BLOCK_CTRLr,
                          REG_ARRAY_INDEX_NONE,
                          srcDevID,
                          LONGAN_NON_UNICAST_BLOCK_PMf,
                          &val)) != RT_ERR_OK)
    {
        STACK_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STACK), "");
        return ret;
    }

    STACK_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_longan_stack_nonUcastBlockPort_set */

/* Function Name:
 *      dal_longan_stack_shrink_get
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
dal_longan_stack_shrink_get(uint32 unit, rtk_stack_shrinkCtrlType_t shrinkType, uint32 *pVal)
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
                          LONGAN_STK_DBG_CTRLr,
                          LONGAN_STK_PORT_DEBUGf,
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
}   /* end of dal_longan_stack_shrink_get */

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
dal_longan_stack_shrink_set(uint32 unit, rtk_stack_shrinkCtrlType_t shrinkType, uint32 val)
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
                          LONGAN_STK_DBG_CTRLr,
                          LONGAN_STK_PORT_DEBUGf,
                          &enable)) != RT_ERR_OK)
    {
        STACK_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_STACK), "");
        return ret;
    }

    STACK_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_longan_stack_shrink_set */


/* Function Name:
 *      _dal_longan_stack_init_config
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
int32
_dal_longan_stack_init_config(uint32 unit)
{
    int32   ret;
    uint32 idx;
    rtk_portmask_t stkPorts;
    uint32  port;

    osal_memset(&stkPorts, 0, sizeof(rtk_portmask_t));
    stkPorts.bits[0] = RTK_DEFAULT_STACK_PORTMASK_0;
    if ((ret = dal_longan_stack_port_set (unit, &stkPorts)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_STACK|MOD_DAL), "");
        return ret;
    }

    if ((ret = dal_longan_stack_devId_set (unit, HAL_UNIT_TO_DEV_ID(unit))) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_STACK|MOD_DAL), "");
        return ret;
    }

    if ((ret = dal_longan_stack_masterDevId_set (unit, HAL_UNIT_TO_DEV_ID(unit))) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_STACK|MOD_DAL), "");
        return ret;
    }

    if ((ret = dal_longan_stack_loopGuard_set (unit, RTK_DEFAULT_STACK_LOOP_GUARD)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_STACK|MOD_DAL), "");
        return ret;
    }

    osal_memset(&stkPorts, 0, sizeof(rtk_portmask_t));
    stkPorts.bits[0] = RTK_DEFAULT_STACK_PORTMASK_0;
    for(idx = 0; idx < HAL_MAX_NUM_OF_DEV(unit); idx ++)
    {
        if ((ret = dal_longan_stack_nonUcastBlockPort_set (unit, idx, &stkPorts)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_STACK|MOD_DAL), "");
            return ret;
        }
    }

    STACK_SEM_LOCK(unit);

    for (port = 0; port < RTL9300_STACK_PORT_NUM; port++)
    {
        RT_ERR_CHK_EHDL(reg_array_field_read(unit, LONGAN_SCHED_PORT_ALGO_CTRLr,
                            RTL9300_UPLINK_PORT_START + port, REG_ARRAY_INDEX_NONE, LONGAN_SCHED_TYPEf, (uint32*)&ori_type[unit][port]), ret,
                            STACK_SEM_UNLOCK(unit); RT_ERR(ret, (MOD_STACK|MOD_DAL), ""););
    }
    STACK_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of _dal_longan_trunk_init_config */

