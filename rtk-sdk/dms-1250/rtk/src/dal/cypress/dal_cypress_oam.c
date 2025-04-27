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
 * Purpose : Definition those public OAM routing APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) OAM (802.3ah) configuration
 *           2) CFM (802.1ag) configuration
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
#include <drv/nic/nic.h>
#include <osal/sem.h>
#include <osal/memory.h>
#include <hal/chipdef/allreg.h>
#include <hal/chipdef/cypress/rtk_cypress_reg_struct.h>
#include <hal/mac/reg.h>
#include <hal/common/halctrl.h>
#include <dal/dal_mgmt.h>
#include <dal/cypress/dal_cypress_oam.h>
#include <rtk/oam.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
static uint32               oam_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};
static osal_mutex_t         oam_sem[RTK_MAX_NUM_OF_UNIT];
static uint32               ethDmRxTimestampIdx;

/*
 * Macro Declaration
 */
/* OAM semaphore handling */
#define OAM_SEM_LOCK(unit)                                                          \
do {                                                                                \
    if (osal_sem_mutex_take(oam_sem[unit], OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)     \
    {                                                                               \
        RT_ERR(RT_ERR_SEM_LOCK_FAILED, (MOD_DAL|MOD_OAM),"semaphore lock failed");  \
        return RT_ERR_SEM_LOCK_FAILED;                                              \
    }                                                                               \
} while(0)

#define OAM_SEM_UNLOCK(unit)                                                            \
do {                                                                                    \
    if (osal_sem_mutex_give(oam_sem[unit]) != RT_ERR_OK)                                \
    {                                                                                   \
        RT_ERR(RT_ERR_SEM_UNLOCK_FAILED, (MOD_DAL|MOD_OAM),"semaphore unlock failed");  \
        return RT_ERR_SEM_UNLOCK_FAILED;                                                \
    }                                                                                   \
} while(0)

/*
 * Function Declaration
 */

/* Function Name:
 *      dal_cypress_oamMapper_init
 * Description:
 *      Hook oam module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must Hook oam module before calling any oam APIs.
 */
int32
dal_cypress_oamMapper_init(dal_mapper_t *pMapper)
{
    /* OAM function */
    pMapper->oam_init = dal_cypress_oam_init;
    pMapper->oam_portDyingGaspPayload_set = dal_cypress_oam_portDyingGaspPayload_set;
    pMapper->oam_dyingGaspSend_set = dal_cypress_oam_dyingGaspSend_set;
    pMapper->oam_autoDyingGaspEnable_get = dal_cypress_oam_autoDyingGaspEnable_get;
    pMapper->oam_autoDyingGaspEnable_set = dal_cypress_oam_autoDyingGaspEnable_set;
    pMapper->oam_dyingGaspWaitTime_get = dal_cypress_oam_dyingGaspWaitTime_get;
    pMapper->oam_dyingGaspWaitTime_set = dal_cypress_oam_dyingGaspWaitTime_set;
    pMapper->oam_loopbackMacSwapEnable_get = dal_cypress_oam_loopbackMacSwapEnable_get;
    pMapper->oam_loopbackMacSwapEnable_set = dal_cypress_oam_loopbackMacSwapEnable_set;
    pMapper->oam_portLoopbackMuxAction_get = dal_cypress_oam_portLoopbackMuxAction_get;
    pMapper->oam_portLoopbackMuxAction_set = dal_cypress_oam_portLoopbackMuxAction_set;
    pMapper->oam_dyingGaspPktCnt_get = dal_cypress_oam_dyingGaspPktCnt_get;
    pMapper->oam_dyingGaspPktCnt_set = dal_cypress_oam_dyingGaspPktCnt_set;

    /* CFM */
    pMapper->oam_cfmCcmPcp_get           = dal_cypress_oam_cfmCcmPcp_get;
    pMapper->oam_cfmCcmPcp_set           = dal_cypress_oam_cfmCcmPcp_set;
    pMapper->oam_cfmCcmCfi_get           = dal_cypress_oam_cfmCcmCfi_get;
    pMapper->oam_cfmCcmCfi_set           = dal_cypress_oam_cfmCcmCfi_set;
    pMapper->oam_cfmCcmTpid_get          = dal_cypress_oam_cfmCcmTpid_get;
    pMapper->oam_cfmCcmTpid_set          = dal_cypress_oam_cfmCcmTpid_set;
    pMapper->oam_cfmCcmInstLifetime_get = dal_cypress_oam_cfmCcmInstLifetime_get;
    pMapper->oam_cfmCcmInstLifetime_set = dal_cypress_oam_cfmCcmInstLifetime_set;
    pMapper->oam_cfmCcmMepid_get         = dal_cypress_oam_cfmCcmMepid_get;
    pMapper->oam_cfmCcmMepid_set         = dal_cypress_oam_cfmCcmMepid_set;
    pMapper->oam_cfmCcmIntervalField_get = dal_cypress_oam_cfmCcmIntervalField_get;
    pMapper->oam_cfmCcmIntervalField_set = dal_cypress_oam_cfmCcmIntervalField_set;
    pMapper->oam_cfmCcmMdl_get           = dal_cypress_oam_cfmCcmMdl_get;
    pMapper->oam_cfmCcmMdl_set           = dal_cypress_oam_cfmCcmMdl_set;
    pMapper->oam_cfmCcmInstTagStatus_get = dal_cypress_oam_cfmCcmInstTagStatus_get;
    pMapper->oam_cfmCcmInstTagStatus_set = dal_cypress_oam_cfmCcmInstTagStatus_set;
    pMapper->oam_cfmCcmInstVid_get       = dal_cypress_oam_cfmCcmInstVid_get;
    pMapper->oam_cfmCcmInstVid_set       = dal_cypress_oam_cfmCcmInstVid_set;
    pMapper->oam_cfmCcmInstMaid_get      = dal_cypress_oam_cfmCcmInstMaid_get;
    pMapper->oam_cfmCcmInstMaid_set      = dal_cypress_oam_cfmCcmInstMaid_set;
    pMapper->oam_cfmCcmInstTxStatus_get  = dal_cypress_oam_cfmCcmInstTxStatus_get;
    pMapper->oam_cfmCcmInstTxStatus_set  = dal_cypress_oam_cfmCcmInstTxStatus_set;
    pMapper->oam_cfmCcmInstInterval_get  = dal_cypress_oam_cfmCcmInstInterval_get;
    pMapper->oam_cfmCcmInstInterval_set  = dal_cypress_oam_cfmCcmInstInterval_set;
    pMapper->oam_cfmCcmTxInstPort_get    = dal_cypress_oam_cfmCcmTxInstPort_get;
    pMapper->oam_cfmCcmTxInstPort_set    = dal_cypress_oam_cfmCcmTxInstPort_set;
    pMapper->oam_cfmCcmRxInstVid_get     = dal_cypress_oam_cfmCcmRxInstVid_get;
    pMapper->oam_cfmCcmRxInstVid_set     = dal_cypress_oam_cfmCcmRxInstVid_set;
    pMapper->oam_cfmCcmRxInstPort_get    = dal_cypress_oam_cfmCcmRxInstPort_get;
    pMapper->oam_cfmCcmRxInstPort_set    = dal_cypress_oam_cfmCcmRxInstPort_set;
    pMapper->oam_cfmCcmKeepalive_get     = dal_cypress_oam_cfmCcmKeepalive_get;
    /* CFM (ETH-DM) */
    pMapper->oam_cfmPortEthDmEnable_get  = dal_cypress_oam_cfmPortEthDmEnable_get;
    pMapper->oam_cfmPortEthDmEnable_set  = dal_cypress_oam_cfmPortEthDmEnable_set;
    pMapper->oam_cfmEthDmRxTimestamp_get = dal_cypress_oam_cfmEthDmRxTimestamp_get;

    return RT_ERR_OK;
}


/* Module Name : OAM */
#if defined(CONFIG_SDK_DRIVER_NIC)
static void
_oam_dyingGasp_tx_callback(uint32 unit, drv_nic_pkt_t *pPacket, void *pCookie)
{
    if (pCookie != NULL)
        drv_nic_pkt_free(unit, pCookie);

    if (pPacket != NULL)
        drv_nic_pkt_free(unit, pPacket);
    return;
}
#endif /* CONFIG_SDK_DRIVER_NIC */

/* Function Name:
 *      dal_cypress_oam_init
 * Description:
 *      Initialize OAM module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize OAM module before calling any OAM APIs.
 */
int32
dal_cypress_oam_init(uint32 unit)
{
    if (INIT_COMPLETED == oam_init[unit])
        return RT_ERR_OK;

    RT_INIT_REENTRY_CHK(oam_init[unit]);
    oam_init[unit] = INIT_NOT_COMPLETED;

    /* create semaphore */
    oam_sem[unit] = osal_sem_mutex_create();
    if (0 == oam_sem[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_OAM), "semaphore create failed");
        return RT_ERR_FAILED;
    }

    /* set init flag to complete init */
    oam_init[unit] = INIT_COMPLETED;

    return RT_ERR_OK;
}   /* end of dal_cypress_oam_init */

/* Function Name:
 *      dal_cypress_oam_loopbackMacSwapEnable_get
 * Description:
 *      Get enable status of swap MAC address (source MAC & destination MAC)
 *      for OAM loopback feature.
 * Input:
 *      unit - unit id
 * Output:
 *      pEnable - pointer to enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      Take swap action when OAM loopback status is enable and parser in "Loopback" state.
 */
int32
dal_cypress_oam_loopbackMacSwapEnable_get(uint32 unit,
                                          rtk_enable_t *pEnable)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OAM), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(oam_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    OAM_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_field_read(unit, CYPRESS_OAM_CTRLr, CYPRESS_MAC_SWAPf,
                    pEnable) ) != RT_ERR_OK)
    {
        OAM_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OAM), "");
        return ret;
    }

    OAM_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OAM), "pEnable=%d", pEnable);

    return RT_ERR_OK;
}   /* end of dal_cypress_oam_loopbackMacSwapEnable_get */

/* Function Name:
 *      dal_cypress_oam_loopbackMacSwapEnable_set
 * Description:
 *      Set enable status of swap MAC address (source MAC & destination MAC)
 *      for OAM loopback feature.
 * Input:
 *      unit    - unit id
 *      enable  - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      Take swap action when OAM loopback status is enable and parser in "Loopback" state.
 */
int32
dal_cypress_oam_loopbackMacSwapEnable_set(uint32 unit,
                                          rtk_enable_t enable)
{
    int32   ret;
    uint32  en;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OAM), "unit=%d, enable=%d", unit, enable);

    /* check Init status */
    RT_INIT_CHK(oam_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    if (DISABLED == enable)
        en = 0;
    else if (ENABLED == enable)
        en = 1;

    OAM_SEM_LOCK(unit);

    /* set value to CHIP */
    if ((ret = reg_field_write(unit, CYPRESS_OAM_CTRLr, CYPRESS_MAC_SWAPf,
                    &en) ) != RT_ERR_OK)
    {
        OAM_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OAM), "");
        return ret;
    }

    OAM_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_cypress_oam_loopbackMacSwapEnable_set */

/* Function Name:
 *      dal_cypress_oam_portLoopbackMuxAction_get
 * Description:
 *      Get action of multiplexer on specfic port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pAction - pointer to multiplexer action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      Multiplexer action is as following:
 *         chip value 0 - ACTION_DROP
 *         chip value 1 - ACTION_FORWARD
 */
int32
dal_cypress_oam_portLoopbackMuxAction_get(uint32 unit,
        rtk_port_t port, rtk_action_t *pAction)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OAM), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(oam_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    OAM_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_array_field_read(unit, CYPRESS_OAM_PORT_ACT_CTRLr,
                    port, REG_ARRAY_INDEX_NONE, CYPRESS_MUX_ACTf,
                    pAction)) != RT_ERR_OK)
    {
        OAM_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OAM), "");
        return ret;
    }

    OAM_SEM_UNLOCK(unit);

    if (0 == *pAction)
        *pAction = ACTION_DROP;
    else if(1 == *pAction)
        *pAction = ACTION_FORWARD;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OAM), "pAction=%d", *pAction);

    return RT_ERR_OK;
}   /* end of dal_cypress_oam_portLoopbackMuxAction_get */

/* Function Name:
 *      dal_cypress_oam_portLoopbackMuxAction_set
 * Description:
 *      Set action of multiplexer on specfic port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      action  - multiplexer action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      Multiplexer action is as following:
 *         chip value 0 - ACTION_DROP
 *         chip value 1 - ACTION_FORWARD
 */
int32
dal_cypress_oam_portLoopbackMuxAction_set(uint32 unit,
        rtk_port_t port, rtk_action_t action)
{
    int32   ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OAM), "unit=%d, port=%d, action=%d",
            unit, port, action);

    /* check Init status */
    RT_INIT_CHK(oam_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((action >= ACTION_END), RT_ERR_INPUT);

    if (ACTION_DROP == action)
        val = 0;
    else if (ACTION_FORWARD == action)
        val = 1;
    else
    {
        RT_LOG(RT_ERR_INPUT, (MOD_DAL|MOD_OAM), "");
        return RT_ERR_INPUT;
    }

    OAM_SEM_LOCK(unit);

    /* set forward action */
    if ((ret = reg_array_field_write(unit, CYPRESS_OAM_PORT_ACT_CTRLr,
                    port, REG_ARRAY_INDEX_NONE, CYPRESS_MUX_ACTf,
                    &val)) != RT_ERR_OK)
    {
        OAM_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OAM), "");
        return ret;
    }

    OAM_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_cypress_oam_portLoopbackMuxAction_set */

/* Function Name:
 *      dal_cypress_oam_portDyingGaspPayload_set
 * Description:
 *      Set the payload of dying gasp frame to specified ports.
 * Input:
 *      unit        - unit id
 *      port        - port id
 *      pPayload    - payload
 *      len         - payload length
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      This API is used to configure the content of dying gasp in specific port.
 */
int32
dal_cypress_oam_portDyingGaspPayload_set(uint32 unit, rtk_port_t port,
    uint8 *pPayload, uint32 len)
{
#if defined(CONFIG_SDK_DRIVER_NIC)
    int32   ret;
    drv_nic_pkt_t *pPacket;

    if (RT_ERR_OK != drv_nic_pkt_alloc(unit, RTK_OAM_DYINGGASPPAYLOAD_MAX, 0, &pPacket))
    {
        osal_printf("[%s]: Alloc packet failed.\n", __FUNCTION__);
        return RT_ERR_FAILED;
    }
    osal_memcpy(pPacket->head, pPayload, len);
    pPacket->length = len;
    pPacket->data = pPacket->head;
    pPacket->tail = pPacket->data + pPacket->length;
    pPacket->end = pPacket->tail;
    pPacket->as_txtag = TRUE;
    pPacket->tx_tag.dg_pkt = TRUE;
    pPacket->tx_tag.bp_fltr1 = TRUE;
    pPacket->tx_tag.bp_fltr2 = TRUE;
    pPacket->tx_tag.fwd_type = NIC_FWD_TYPE_PHYISCAL;
    pPacket->tx_tag.dst_port_mask   = ((uint64)1 << port) & 0xffffffff;
    pPacket->tx_tag.dst_port_mask_1 = ((uint64)1 << port) >> 32;

    ret = drv_nic_pkt_tx(unit, pPacket, _oam_dyingGasp_tx_callback, NULL);
    if (ret)
    {
        drv_nic_pkt_free(unit, pPacket);
        return RT_ERR_FAILED;
    }


    /* Patch for Mantis-0017650: Send dying gasp twice */
    if (RT_ERR_OK != drv_nic_pkt_alloc(unit, RTK_OAM_DYINGGASPPAYLOAD_MAX, 0, &pPacket))
    {
        osal_printf("[%s]: Alloc packet failed.\n", __FUNCTION__);
        return RT_ERR_FAILED;
    }
    osal_memcpy(pPacket->head, pPayload, len);
    pPacket->length = len;
    pPacket->data = pPacket->head;
    pPacket->tail = pPacket->data + pPacket->length;
    pPacket->end = pPacket->tail;
    pPacket->as_txtag = TRUE;
    pPacket->tx_tag.dg_pkt = TRUE;
    pPacket->tx_tag.bp_fltr1 = TRUE;
    pPacket->tx_tag.bp_fltr2 = TRUE;
    pPacket->tx_tag.fwd_type = NIC_FWD_TYPE_PHYISCAL;
    pPacket->tx_tag.dst_port_mask    = ((uint64)1 << port) & 0xffffffff;
    pPacket->tx_tag.dst_port_mask_1 = ((uint64)1 << port) >> 32;

    ret = drv_nic_pkt_tx(unit, pPacket, _oam_dyingGasp_tx_callback, NULL);
    if (ret)
    {
        drv_nic_pkt_free(unit, pPacket);
        return RT_ERR_FAILED;
    }
#else
    return RT_ERR_FAILED;
#endif/* CONFIG_SDK_DRIVER_NIC */


    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_oam_DyingGaspSend_set
 * Description:
 *      Start sending dying gasp frame to specified ports.
 * Input:
 *      unit        - unit id
 *      enable  - trigger dying gasp with enabled ports.
 * Output:
 *      pEnable - pointer buffer
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      This API is used when CPU want to send dying gasp by itself.
 */
int32
dal_cypress_oam_dyingGaspSend_set(uint32 unit, rtk_enable_t enable)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OAM), "unit=%d en=%d", unit, enable);

    /* check Init status */
    RT_INIT_CHK(oam_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    OAM_SEM_LOCK(unit);

    /* Trigger Dying Gasp Function */
    if ((ret = reg_field_write(unit, CYPRESS_OAM_GLB_DYING_GASP_CTRLr,
                    CYPRESS_DYING_GASP_TRIGf, &enable) ) != RT_ERR_OK)
    {
        OAM_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OAM), "");
        return ret;
    }

    OAM_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_cypress_oam_dyingGaspSend_set */

/* Function Name:
 *       dal_cypress_oam_dyingGaspWaitTime_get
 * Description:
 *       Get waiting time of sending dying gasp after voltage is lower than expeted.
 * Input:
 *      unit - unit id
 * Output:
 *      time - waiting time
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      Granularity of waiting time is 10 ns.
 */
int32
dal_cypress_oam_dyingGaspWaitTime_get(uint32 unit, uint32 *pTime)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OAM), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(oam_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pTime), RT_ERR_NULL_POINTER);

    OAM_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_field_read(unit, CYPRESS_OAM_GLB_DYING_GASP_CTRLr,
                              CYPRESS_TBP_VALf, pTime) ) != RT_ERR_OK)
    {
        OAM_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OAM), "");
        return ret;
    }

    OAM_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OAM), "pTime=%d", *pTime);

    return RT_ERR_OK;
}   /*end of dal_cypress_oam_dyingGaspWaitTime_get*/

/* Function Name:
 *      dal_cypress_oam_dyingGaspWaitTime_set
 * Description:
 *      Set waiting time of sending dying gasp after voltage is lower than expeted.
 * Input:
 *      unit - unit id
 *      time - waiting time
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_OUT_OF_RANGE     - input parameter out of range
 * Note:
 *      Granularity of waiting time is 10 ns.
 */
int32
dal_cypress_oam_dyingGaspWaitTime_set(uint32 unit, uint32 time)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OAM), "unit=%d, time=%d", unit, time);

    /* check Init status */
    RT_INIT_CHK(oam_init[unit]);

    RT_PARAM_CHK(time > HAL_DYING_GASP_SUSTAIN_TIME_MAX(unit), RT_ERR_OUT_OF_RANGE);

    OAM_SEM_LOCK(unit);

    /* set the time duration */
    if ((ret = reg_field_write(unit, CYPRESS_OAM_GLB_DYING_GASP_CTRLr,
                               CYPRESS_TBP_VALf, &time) ) != RT_ERR_OK)
    {
        OAM_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OAM), "");
        return ret;
    }

    OAM_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_cypress_oam_dyingGaspWaitTime_set */

/* Function Name:
 *      dal_cypress_oam_autoDyingGaspEnable_get
 * Description:
 *      Get enable status of sending dying gasp automatically on specified port
 *      when voltage is lower than expected.
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pEnable - pointer to enable status of sending dying gasp automatically
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_oam_autoDyingGaspEnable_get(uint32 unit, rtk_port_t port,
                                        rtk_enable_t *pEnable)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OAM), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(oam_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);

    OAM_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_array_field_read(unit, CYPRESS_OAM_PORT_DYING_GASP_CTRLr,
                    port, REG_ARRAY_INDEX_NONE, CYPRESS_PORT_OAM_DYING_GASP_ENf,
                    pEnable) ) != RT_ERR_OK)
    {
        OAM_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OAM), "");
        return ret;
    }

    OAM_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OAM), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
}   /* end of dal_cypress_oam_autoDyingGaspEnable_get */

/* Function Name:
 *      dal_cypress_oam_autoDyingGaspEnable_set
 * Description:
 *      Set enable status of sending dying gasp automatically on specified port
 *      when voltage is lower than expected.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      enable  - enable status of sending dying gasp automatically
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
int32
dal_cypress_oam_autoDyingGaspEnable_set(uint32 unit, rtk_port_t port,
                                        rtk_enable_t enable)
{
    int32   ret;
    uint32  en;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OAM), "unit=%d, port=%d, enable=%d", unit, port, enable);

    /* check Init status */
    RT_INIT_CHK(oam_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    if (DISABLED == enable)
        en = 0;
    else if (ENABLED == enable)
        en = 1;

    OAM_SEM_LOCK(unit);

    /* set enable status of per port dying gasp */
    if ((ret = reg_array_field_write(unit, CYPRESS_OAM_PORT_DYING_GASP_CTRLr,
                    port, REG_ARRAY_INDEX_NONE, CYPRESS_PORT_OAM_DYING_GASP_ENf,
                    &en) ) != RT_ERR_OK)
    {
        OAM_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OAM), "");
        return ret;
    }

    OAM_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_cypress_oam_autoDyingGaspEnable_set */

/* Function Name:
 *      dal_cypress_oam_cfmCcmPcp_get
 * Description:
 *      Get priority code point value for generate CCM frame.
 * Input:
 *      unit    - unit id
 * Output:
 *      pcp  - pointer buffer
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_oam_cfmCcmPcp_get(uint32 unit, uint32 *pcp)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OAM), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(oam_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pcp), RT_ERR_NULL_POINTER);

    OAM_SEM_LOCK(unit);

    /* get value from chip */
    if ((ret = reg_field_read(unit, CYPRESS_CCM_TAG_CTRLr,
                    CYPRESS_PCPf, pcp) ) != RT_ERR_OK)
    {
        OAM_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OAM), "");
        return ret;
    }

    OAM_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_cypress_oam_cfmCcmPcp_get */

/* Function Name:
 *      dal_cypress_oam_cfmCcmPcp_set
 * Description:
 *      Set priority code point value for generate CCM frame.
 * Input:
 *      unit    - unit id
 *      pcp     - priority code point value for generate CCM frame.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
int32
dal_cypress_oam_cfmCcmPcp_set(uint32 unit, uint32 pcp)
{
    int32   ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OAM), "unit=%d pcp=%d", unit, pcp);

    /* check Init status */
    RT_INIT_CHK(oam_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((pcp > RTK_DOT1P_PRIORITY_MAX), RT_ERR_INPUT);

    val = pcp;

    OAM_SEM_LOCK(unit);

    /* set value to chip */
    if ((ret = reg_field_write(unit, CYPRESS_CCM_TAG_CTRLr,
                    CYPRESS_PCPf, &val) ) != RT_ERR_OK)
    {
        OAM_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OAM), "");
        return ret;
    }

    OAM_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_cypress_oam_cfmCcmPcp_set */

/* Function Name:
 *      dal_cypress_oam_cfmCcmCfi_get
 * Description:
 *      Get canonical format identifier value for generate CCM frame.
 * Input:
 *      unit    - unit id
 * Output:
 *      cfi  - pointer buffer
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_oam_cfmCcmCfi_get(uint32 unit, uint32 *cfi)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OAM), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(oam_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == cfi), RT_ERR_NULL_POINTER);

    OAM_SEM_LOCK(unit);

    /* get value from chip */
    if ((ret = reg_field_read(unit, CYPRESS_CCM_TAG_CTRLr,
                    CYPRESS_CFIf, cfi) ) != RT_ERR_OK)
    {
        OAM_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OAM), "");
        return ret;
    }

    OAM_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_cypress_oam_cfmCcmCfi_get */

/* Function Name:
 *      dal_cypress_oam_cfmCcmCfi_set
 * Description:
 *      Set canonical format identifier value for generate CCM frame.
 * Input:
 *      unit    - unit id
 *      cfi     - canonical format identifier value for generate CCM frame.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
int32
dal_cypress_oam_cfmCcmCfi_set(uint32 unit, uint32 cfi)
{
    int32   ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OAM), "unit=%d cfi=%d", unit, cfi);

    /* check Init status */
    RT_INIT_CHK(oam_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((cfi > RTK_DOT1P_DEI_MAX), RT_ERR_INPUT);

    val = cfi;

    OAM_SEM_LOCK(unit);

    /* set value to chip */
    if ((ret = reg_field_write(unit, CYPRESS_CCM_TAG_CTRLr,
                    CYPRESS_CFIf, &val) ) != RT_ERR_OK)
    {
        OAM_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OAM), "");
        return ret;
    }

    OAM_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_cypress_oam_cfmCcmCfi_set */

/* Function Name:
 *      dal_cypress_oam_cfmCcmTpid_get
 * Description:
 *      Get TPID value for generate CCM frame.
 * Input:
 *      unit    - unit id
 * Output:
 *      tpid  - pointer buffer
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_oam_cfmCcmTpid_get(uint32 unit, uint32 *tpid)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OAM), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(oam_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == tpid), RT_ERR_NULL_POINTER);

    OAM_SEM_LOCK(unit);

    /* get value from chip */
    if ((ret = reg_field_read(unit, CYPRESS_CCM_TAG_CTRLr,
                    CYPRESS_TPIDf, tpid) ) != RT_ERR_OK)
    {
        OAM_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OAM), "");
        return ret;
    }

    OAM_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_cypress_oam_cfmCcmTpid_get */

/* Function Name:
 *      dal_cypress_oam_cfmCcmTpid_set
 * Description:
 *      Set TPID value for generate CCM frame.
 * Input:
 *      unit    - unit id
 *      tpid    - TPID value for generate CCM frame.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
int32
dal_cypress_oam_cfmCcmTpid_set(uint32 unit, uint32 tpid)
{
    int32   ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OAM), "unit=%d tpid=%x", unit, tpid);

    /* check Init status */
    RT_INIT_CHK(oam_init[unit]);

    val = tpid;

    /* parameter check */
    RT_PARAM_CHK((tpid > RTK_TPID_MAX), RT_ERR_INPUT);

    OAM_SEM_LOCK(unit);

    /* set value to chip */
    if ((ret = reg_field_write(unit, CYPRESS_CCM_TAG_CTRLr,
                    CYPRESS_TPIDf, &val) ) != RT_ERR_OK)
    {
        OAM_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OAM), "");
        return ret;
    }

    OAM_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_cypress_oam_cfmCcmTpid_set */

/* Function Name:
 *      dal_cypress_oam_cfmCcmInstLifetime_get
 * Description:
 *      Get reset ERP instance counter when receiving the coresponding
 *      instance CCM frame.
 * Input:
 *      unit        - unit id
 *      instance    - control entry instance
 * Output:
 *      lifetime    - pointer buffer
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_oam_cfmCcmInstLifetime_get(uint32 unit, uint32 instance,
                                        uint32 *lifetime)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OAM), "unit=%d instance=%d", unit, instance);

    /* check Init status */
    RT_INIT_CHK(oam_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((instance >= RTK_CFM_TX_INSTANCE_MAX), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == lifetime), RT_ERR_NULL_POINTER);

    OAM_SEM_LOCK(unit);

    /* get value from chip */
    if ((ret = reg_array_field_read(unit, CYPRESS_CCM_LIFETIME_CTRLr,
                    REG_ARRAY_INDEX_NONE, instance, CYPRESS_LIFETIMEf,
                    lifetime) ) != RT_ERR_OK)
    {
        OAM_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OAM), "");
        return ret;
    }

    OAM_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_cypress_oam_cfmCcmInstLifetime_get */

/* Function Name:
 *      dal_cypress_oam_cfmCcmInstLifetime_set
 * Description:
 *      Set reset ERP instance counter when receiving the coresponding
 *      instance CCM frame.
 * Input:
 *      unit        - unit id
 *      instance    - control entry instance
 *      lifetime    - reset ERP counter time.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
int32
dal_cypress_oam_cfmCcmInstLifetime_set(uint32 unit, uint32 instance,
                                        uint32 lifetime)
{
    int32   ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OAM), "unit=%d instance=%d lifetime=%d",
           unit, instance, lifetime);

    /* check Init status */
    RT_INIT_CHK(oam_init[unit]);

    val = lifetime;

    /* parameter check */
    RT_PARAM_CHK((instance >= RTK_CFM_TX_INSTANCE_MAX), RT_ERR_INPUT);
    RT_PARAM_CHK((lifetime > RTK_CFM_RESET_LIFETIME_MAX), RT_ERR_INPUT);

    OAM_SEM_LOCK(unit);

    /* set value to chip */
    if ((ret = reg_array_field_write(unit, CYPRESS_CCM_LIFETIME_CTRLr,
                    REG_ARRAY_INDEX_NONE, instance, CYPRESS_LIFETIMEf,
                    &val) ) != RT_ERR_OK)
    {
        OAM_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OAM), "");
        return ret;
    }

    OAM_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_cypress_oam_cfmCcmInstLifetime_set */

/* Function Name:
 *      dal_cypress_oam_cfmCcmMepid_get
 * Description:
 *      Get MEPID to be inserted to generated CCM frame.
 * Input:
 *      unit    - unit id
 * Output:
 *      mepid   - pointer buffer
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_oam_cfmCcmMepid_get(uint32 unit, uint32 *mepid)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OAM), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(oam_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == mepid), RT_ERR_NULL_POINTER);

    OAM_SEM_LOCK(unit);

    /* get value from chip */
    if ((ret = reg_field_read(unit, CYPRESS_CCM_TX_CTRLr,
                    CYPRESS_MEPIDf, mepid) ) != RT_ERR_OK)
    {
        OAM_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OAM), "");
        return ret;
    }

    OAM_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_cypress_oam_cfmCcmMepid_get */

/* Function Name:
 *      dal_cypress_oam_cfmCcmMepid_set
 * Description:
 *      Set MEPID to be inserted to generated CCM frame.
 * Input:
 *      unit    - unit id
 *      mepid   - reset ERP counter time.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
int32
dal_cypress_oam_cfmCcmMepid_set(uint32 unit, uint32 mepid)
{
    int32   ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OAM), "unit=%d mepid=%d", unit, mepid);

    /* check Init status */
    RT_INIT_CHK(oam_init[unit]);

    val = mepid;

    /* parameter check */
    RT_PARAM_CHK((mepid > RTK_CFM_MEPID_MAX), RT_ERR_INPUT);

    OAM_SEM_LOCK(unit);

    /* set value to chip */
    if ((ret = reg_field_write(unit, CYPRESS_CCM_TX_CTRLr,
                    CYPRESS_MEPIDf, &val) ) != RT_ERR_OK)
    {
        OAM_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OAM), "");
        return ret;
    }

    OAM_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_cypress_oam_cfmCcmMepid_set */

/* Function Name:
 *      dal_cypress_oam_cfmCcmIntervalField_get
 * Description:
 *      Get value to be inserted to interval field in flag for generated CCM frame.
 * Input:
 *      unit    - unit id
 * Output:
 *      interval    - interval field in flag.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_oam_cfmCcmIntervalField_get(uint32 unit, uint32 *interval)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OAM), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(oam_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == interval), RT_ERR_NULL_POINTER);

    OAM_SEM_LOCK(unit);

    /* get value from chip */
    if ((ret = reg_field_read(unit, CYPRESS_CCM_TX_CTRLr,
                    CYPRESS_LIFETIME_CODEf, interval) ) != RT_ERR_OK)
    {
        OAM_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OAM), "");
        return ret;
    }

    OAM_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_cypress_oam_cfmCcmIntervalField_get */

/* Function Name:
 *      dal_cypress_oam_cfmCcmIntervalField_set
 * Description:
 *      Set value to be inserted to interval field in flag for generated CCM frame.
 * Input:
 *      unit        - unit id
 *      interval    - interval field in flag.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
int32
dal_cypress_oam_cfmCcmIntervalField_set(uint32 unit, uint32 interval)
{
    int32   ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OAM), "unit=%d lifetime=%d", unit, interval);

    /* check Init status */
    RT_INIT_CHK(oam_init[unit]);

    val = interval;

    /* parameter check */
    RT_PARAM_CHK((interval >= RTK_CFM_LIFETIME_MAX), RT_ERR_INPUT);

    OAM_SEM_LOCK(unit);

    /* set value to chip */
    if ((ret = reg_field_write(unit, CYPRESS_CCM_TX_CTRLr,
                    CYPRESS_LIFETIME_CODEf, &val) ) != RT_ERR_OK)
    {
        OAM_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OAM), "");
        return ret;
    }

    OAM_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_cypress_oam_cfmCcmIntervalField_set */

/* Function Name:
 *      dal_cypress_oam_cfmCcmMdl_get
 * Description:
 *      Get MD level to be inserted to generated CCM frame.
 * Input:
 *      unit    - unit id
 * Output:
 *      mdl     - pointer buffer
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_oam_cfmCcmMdl_get(uint32 unit, uint32 *mdl)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OAM), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(oam_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == mdl), RT_ERR_NULL_POINTER);

    OAM_SEM_LOCK(unit);

    /* get value from chip */
    if ((ret = reg_field_read(unit, CYPRESS_CCM_TX_CTRLr,
                    CYPRESS_MDLf, mdl) ) != RT_ERR_OK)
    {
        OAM_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OAM), "");
        return ret;
    }

    OAM_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_cypress_oam_cfmCcmMdl_get */

/* Function Name:
 *      dal_cypress_oam_cfmCcmMdl_set
 * Description:
 *      Set MD level to be inserted to generated CCM frame.
 * Input:
 *      unit    - unit id
 *      mdl     - MD level insert to CCM frame
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
int32
dal_cypress_oam_cfmCcmMdl_set(uint32 unit, uint32 mdl)
{
    int32   ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OAM), "unit=%d mdl=%d", unit, mdl);

    /* check Init status */
    RT_INIT_CHK(oam_init[unit]);

    val = mdl;

    /* parameter check */
    RT_PARAM_CHK((mdl >= RTK_CFM_MDL_MAX), RT_ERR_INPUT);

    OAM_SEM_LOCK(unit);

    /* set value to chip */
    if ((ret = reg_field_write(unit, CYPRESS_CCM_TX_CTRLr,
                    CYPRESS_MDLf, &val) ) != RT_ERR_OK)
    {
        OAM_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OAM), "");
        return ret;
    }

    OAM_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_cypress_oam_cfmCcmMdl_set */

/* Function Name:
 *      dal_cypress_oam_cfmCcmInstTagStatus_get
 * Description:
 *      Get tag status for instance member to be inserted to generated CCM frame.
 * Input:
 *      unit        - unit id
 *      instance    - tx control entry instance
 * Output:
 *      enable  - pointer buffer
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_oam_cfmCcmInstTagStatus_get(uint32 unit, uint32 instance,
                                        rtk_enable_t *enable)
{
    int32   ret;
    uint32  en;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OAM), "unit=%d instance=%d", unit, instance);

    /* check Init status */
    RT_INIT_CHK(oam_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((instance >= RTK_CFM_TX_INSTANCE_MAX), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == enable), RT_ERR_NULL_POINTER);

    OAM_SEM_LOCK(unit);

    /* get value from chip */
    if ((ret = reg_array_field_read(unit, CYPRESS_CCM_TX_INST_CTRLr,
                    REG_ARRAY_INDEX_NONE, instance, CYPRESS_VID_ADDf,
                    &en) ) != RT_ERR_OK)
    {
        OAM_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OAM), "");
        return ret;
    }

    OAM_SEM_UNLOCK(unit);

    if (0 == en)
        *enable = DISABLED;
    else if (1 == en)
        *enable = ENABLED;

    return RT_ERR_OK;
}   /* end of dal_cypress_oam_cfmCcmInstTagStatus_get */

/* Function Name:
 *      dal_cypress_oam_cfmCcmInstTagStatus_set
 * Description:
 *      Set tag status for instance member to be inserted to generated CCM frame.
 * Input:
 *      unit        - unit id
 *      instance    - tx control entry instance
 *      enable      - tag status.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
int32
dal_cypress_oam_cfmCcmInstTagStatus_set(uint32 unit, uint32 instance,
                                        rtk_enable_t enable)
{
    int32   ret;
    uint32  en;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OAM), "unit=%d instance=%d enable=%d",
           unit, instance, enable);

    /* check Init status */
    RT_INIT_CHK(oam_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((instance >= RTK_CFM_TX_INSTANCE_MAX), RT_ERR_INPUT);
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    if (DISABLED == enable)
        en = 0;
    else if (ENABLED == enable)
        en = 1;

    OAM_SEM_LOCK(unit);

    /* set value to chip */
    if ((ret = reg_array_field_write(unit, CYPRESS_CCM_TX_INST_CTRLr,
                    REG_ARRAY_INDEX_NONE, instance, CYPRESS_VID_ADDf,
                    &en) ) != RT_ERR_OK)
    {
        OAM_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OAM), "");
        return ret;
    }

    OAM_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_cypress_oam_cfmCcmInstTagStatus_set */

/* Function Name:
 *      dal_cypress_oam_cfmCcmInstVid_get
 * Description:
 *      Get vlan id for instance member to be inserted to generated CCM frame.
 * Input:
 *      unit        - unit id
 *      instance    - tx control entry instance
 * Output:
 *      vid     - pointer buffer
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid vid
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_oam_cfmCcmInstVid_get(uint32 unit, uint32 instance,
                                  rtk_vlan_t *vid)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OAM), "unit=%d instance=%d", unit, instance);

    /* check Init status */
    RT_INIT_CHK(oam_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((instance >= RTK_CFM_TX_INSTANCE_MAX), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == vid), RT_ERR_NULL_POINTER);

    OAM_SEM_LOCK(unit);

    /* get value from chip */
    if ((ret = reg_array_field_read(unit, CYPRESS_CCM_TX_INST_CTRLr,
                    REG_ARRAY_INDEX_NONE, instance, CYPRESS_VIDf,
                    vid) ) != RT_ERR_OK)
    {
        OAM_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OAM), "");
        return ret;
    }

    OAM_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_cypress_oam_cfmCcmInstVid_get */

/* Function Name:
 *      dal_cypress_oam_cfmCcmInstVid_set
 * Description:
 *      Set vlan id for instance member to be inserted to generated CCM frame.
 * Input:
 *      unit        - unit id
 *      instance    - tx control entry instance
 *      vid         - vlan id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_VLAN_VID         - invalid vid
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
int32
dal_cypress_oam_cfmCcmInstVid_set(uint32 unit, uint32 instance,
                                  rtk_vlan_t vid)
{
    int32   ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OAM), "unit=%d instance=%d vid=%d",
           unit, instance, vid);

    /* check Init status */
    RT_INIT_CHK(oam_init[unit]);

    val = vid;

    /* parameter check */
    RT_PARAM_CHK((instance >= RTK_CFM_TX_INSTANCE_MAX), RT_ERR_INPUT);
    RT_PARAM_CHK((vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);

    OAM_SEM_LOCK(unit);

    /* set value to chip */
    if ((ret = reg_array_field_write(unit, CYPRESS_CCM_TX_INST_CTRLr,
                    REG_ARRAY_INDEX_NONE, instance, CYPRESS_VIDf,
                    &val) ) != RT_ERR_OK)
    {
        OAM_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OAM), "");
        return ret;
    }

    OAM_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_cypress_oam_cfmCcmInstVid_set */

/* Function Name:
 *      dal_cypress_oam_cfmCcmInstMaid_get
 * Description:
 *      Get vlan id for instance member to be inserted to generated CCM frame.
 * Input:
 *      unit        - unit id
 *      instance    - tx control entry instance
 * Output:
 *      maid     - pointer buffer
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_oam_cfmCcmInstMaid_get(uint32 unit, uint32 instance,
                                   uint32 *maid)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OAM), "unit=%d instance=%d", unit, instance);

    /* check Init status */
    RT_INIT_CHK(oam_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((instance >= RTK_CFM_TX_INSTANCE_MAX), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == maid), RT_ERR_NULL_POINTER);

    OAM_SEM_LOCK(unit);

    /* get value from chip */
    if ((ret = reg_array_field_read(unit, CYPRESS_CCM_TX_INST_CTRLr,
                    REG_ARRAY_INDEX_NONE, instance, CYPRESS_MAIDf,
                    maid) ) != RT_ERR_OK)
    {
        OAM_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OAM), "");
        return ret;
    }

    OAM_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_cypress_oam_cfmCcmInstMaid_get */

/* Function Name:
 *      dal_cypress_oam_cfmCcmInstMaid_set
 * Description:
 *      Set MAID for instance member to be inserted to generated CCM frame.
 * Input:
 *      unit        - unit id
 *      instance    - tx control entry instance
 *      maid        - MA id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
int32
dal_cypress_oam_cfmCcmInstMaid_set(uint32 unit, uint32 instance,
                                   uint32 maid)
{
    int32   ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OAM), "unit=%d instance=%d maid=%d",
           unit, instance, maid);

    /* check Init status */
    RT_INIT_CHK(oam_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((instance >= RTK_CFM_TX_INSTANCE_MAX), RT_ERR_INPUT);
    RT_PARAM_CHK((maid >= RTK_CFM_MAID_MAX), RT_ERR_INPUT);

    val = maid;

    OAM_SEM_LOCK(unit);

    /* set value to chip */
    if ((ret = reg_array_field_write(unit, CYPRESS_CCM_TX_INST_CTRLr,
                    REG_ARRAY_INDEX_NONE, instance, CYPRESS_MAIDf,
                    &val) ) != RT_ERR_OK)
    {
        OAM_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OAM), "");
        return ret;
    }

    OAM_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_cypress_oam_cfmCcmInstMaid_set */

/* Function Name:
 *      dal_cypress_oam_cfmCcmInstTxStatus_get
 * Description:
 *      Get tx status for instance member to generated CCM frame.
 * Input:
 *      unit        - unit id
 *      instance    - tx control entry instance
 * Output:
 *      enable      - tx status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
int32
dal_cypress_oam_cfmCcmInstTxStatus_get(uint32 unit, uint32 instance,
                                       rtk_enable_t *enable)
{
    int32   ret;
    uint32  en;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OAM), "unit=%d instance=%d", unit, instance);

    /* check Init status */
    RT_INIT_CHK(oam_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((instance >= RTK_CFM_TX_INSTANCE_MAX), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == enable), RT_ERR_NULL_POINTER);

    OAM_SEM_LOCK(unit);

    /* get value from chip */
    if ((ret = reg_array_field_read(unit, CYPRESS_CCM_TX_INST_CTRLr,
                    REG_ARRAY_INDEX_NONE, instance, CYPRESS_TXf,
                    &en) ) != RT_ERR_OK)
    {
        OAM_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OAM), "");
        return ret;
    }

    OAM_SEM_UNLOCK(unit);

    if (0 == en)
        *enable = DISABLED;
    else if (1 == en)
        *enable = ENABLED;

    return RT_ERR_OK;
}   /* end of dal_cypress_oam_cfmCcmInstTxStatus_get */

/* Function Name:
 *      dal_cypress_oam_cfmCcmInstTxStatus_set
 * Description:
 *      Set tx status for instance member to generated CCM frame.
 * Input:
 *      unit        - unit id
 *      instance    - tx control entry instance
 *      enable      - tx status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
int32
dal_cypress_oam_cfmCcmInstTxStatus_set(uint32 unit, uint32 instance,
                                       rtk_enable_t enable)
{
    int32   ret;
    uint32  en;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OAM), "unit=%d instance=%d enable=%d",
           unit, instance, enable);

    /* check Init status */
    RT_INIT_CHK(oam_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((instance >= RTK_CFM_TX_INSTANCE_MAX), RT_ERR_INPUT);
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    if (DISABLED == enable)
        en = 0;
    else if (ENABLED == enable)
        en = 1;

    OAM_SEM_LOCK(unit);

    /* set value to chip */
    if ((ret = reg_array_field_write(unit, CYPRESS_CCM_TX_INST_CTRLr,
                    REG_ARRAY_INDEX_NONE, instance, CYPRESS_TXf,
                    &en) ) != RT_ERR_OK)
    {
        OAM_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OAM), "");
        return ret;
    }

    OAM_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_cypress_oam_cfmCcmInstTxStatus_set */

/* Function Name:
 *      dal_cypress_oam_cfmCcmInstInterval_get
 * Description:
 *      Get CCM frame transmit interval.
 * Input:
 *      unit        - unit id
 *      instance    - tx control entry instance
 * Output:
 *      interval    - pointer buffer
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_oam_cfmCcmInstInterval_get(uint32 unit, uint32 instance,
                                       uint32 *interval)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OAM), "unit=%d instance=%d", unit, instance);

    /* check Init status */
    RT_INIT_CHK(oam_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((instance >= RTK_CFM_TX_INSTANCE_MAX), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == interval), RT_ERR_NULL_POINTER);

    OAM_SEM_LOCK(unit);

    /* get value from chip */
    if ((ret = reg_array_field_read(unit, CYPRESS_CCM_TX_INST_CTRLr,
                    REG_ARRAY_INDEX_NONE, instance, CYPRESS_INTLVf,
                    interval) ) != RT_ERR_OK)
    {
        OAM_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OAM), "");
        return ret;
    }

    OAM_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_cypress_oam_cfmCcmInstInterval_get */

/* Function Name:
 *      dal_cypress_oam_cfmCcmInstInterval_set
 * Description:
 *      Set CCM frame transmit interval.
 * Input:
 *      unit        - unit id
 *      instance    - tx control entry instance
 *      interval    - transmit interval
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
int32
dal_cypress_oam_cfmCcmInstInterval_set(uint32 unit, uint32 instance,
                                       uint32 interval)
{
    int32   ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OAM), "unit=%d instance=%d interval=%d",
           unit, instance, interval);

    /* check Init status */
    RT_INIT_CHK(oam_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((instance >= RTK_CFM_TX_INSTANCE_MAX), RT_ERR_INPUT);
    RT_PARAM_CHK((interval > RTK_CFM_TX_INTERVAL), RT_ERR_INPUT);

    val = interval;

    OAM_SEM_LOCK(unit);

    /* set value to chip */
    if ((ret = reg_array_field_write(unit, CYPRESS_CCM_TX_INST_CTRLr,
                    REG_ARRAY_INDEX_NONE, instance, CYPRESS_INTLVf,
                    &val) ) != RT_ERR_OK)
    {
        OAM_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OAM), "");
        return ret;
    }

    OAM_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_cypress_oam_cfmCcmInstInterval_set */

/* Function Name:
 *      dal_cypress_oam_cfmCcmTxInstPort_get
 * Description:
 *      Get tx instance member.
 * Input:
 *      unit        - unit id
 *      instance    - tx control entry instance
 *      index       - instance member index
 * Output:
 *      port    - pointer buffer
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_oam_cfmCcmTxInstPort_get(uint32 unit, uint32 instance,
                                     uint32 index, rtk_port_t *port)
{
    int32   ret;
    uint32  portField;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OAM), "unit=%d instance=%d index=%d",
           unit, instance, index);

    /* check Init status */
    RT_INIT_CHK(oam_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((instance >= RTK_CFM_TX_INSTANCE_MAX), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == port), RT_ERR_NULL_POINTER);

    if (0 == index)
        portField = CYPRESS_P0f;
    else if (1 == index)
        portField = CYPRESS_P1f;
    else
        return RT_ERR_INPUT;


    OAM_SEM_LOCK(unit);

    /* get value from chip */
    if ((ret = reg_array_field_read(unit, CYPRESS_CCM_TX_INST_P_CFGr,
                    REG_ARRAY_INDEX_NONE, instance, portField,
                    port) ) != RT_ERR_OK)
    {
        OAM_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OAM), "");
        return ret;
    }

    OAM_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_cypress_oam_cfmCcmTxInstPort_get */

/* Function Name:
 *      dal_cypress_oam_cfmCcmTxInstPort_set
 * Description:
 *      Set tx instance member.
 * Input:
 *      unit        - unit id
 *      instance    - tx control entry instance
 *      index       - instance member index
 *      port        - transmit interval
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
int32
dal_cypress_oam_cfmCcmTxInstPort_set(uint32 unit, uint32 instance,
                                     uint32 index, rtk_port_t port)
{
    int32   ret;
    uint32  portField;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OAM), "unit=%d instance=%d index=%d port=%d",
           unit, instance, index, port);

    /* check Init status */
    RT_INIT_CHK(oam_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((instance >= RTK_CFM_TX_INSTANCE_MAX), RT_ERR_INPUT);
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);

    val = port;

    if (0 == index)
        portField = CYPRESS_P0f;
    else if (0 == index)
        portField = CYPRESS_P1f;
    else
        return RT_ERR_INPUT;


    OAM_SEM_LOCK(unit);

    /* set value to chip */
    if ((ret = reg_array_field_write(unit, CYPRESS_CCM_TX_INST_P_CFGr,
                    REG_ARRAY_INDEX_NONE, instance, portField,
                    &val) ) != RT_ERR_OK)
    {
        OAM_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OAM), "");
        return ret;
    }

    OAM_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_cypress_oam_cfmCcmTxInstPort_set */

/* Function Name:
 *      dal_cypress_oam_cfmCcmRxInstVid_get
 * Description:
 *      Get rx instance accept vlan id.
 * Input:
 *      unit        - unit id
 *      instance    - rx control entry instance
 * Output:
 *      vid    - pointer buffer
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      If vid = 0, it won't check vid.
 */
int32
dal_cypress_oam_cfmCcmRxInstVid_get(uint32 unit, uint32 instance,
                                    rtk_vlan_t *vid)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OAM), "unit=%d instance=%d", unit, instance);

    /* check Init status */
    RT_INIT_CHK(oam_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((instance >= RTK_CFM_RX_INSTANCE_MAX), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == vid), RT_ERR_NULL_POINTER);

    OAM_SEM_LOCK(unit);

    /* get value from chip */
    if ((ret = reg_array_field_read(unit, CYPRESS_CCM_RX_INST_CTRLr,
                    REG_ARRAY_INDEX_NONE, instance, CYPRESS_VIDf,
                    vid) ) != RT_ERR_OK)
    {
        OAM_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OAM), "");
        return ret;
    }

    OAM_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_cypress_oam_cfmCcmRxInstVid_get */

/* Function Name:
 *      dal_cypress_oam_cfmCcmRxInstVid_set
 * Description:
 *      Set rx instance accept vlan id.
 * Input:
 *      unit        - unit id
 *      instance    - rx control entry instance
 *      vid         - accept vlan id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_VLAN_VID         - invalid vid
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      If vid = 0, it won't check vid.
 */
int32
dal_cypress_oam_cfmCcmRxInstVid_set(uint32 unit, uint32 instance,
                                    rtk_vlan_t vid)
{
    int32   ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OAM), "unit=%d instance=%d vid=%d",
           unit, instance, vid);

    /* check Init status */
    RT_INIT_CHK(oam_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((instance >= RTK_CFM_RX_INSTANCE_MAX), RT_ERR_INPUT);
    RT_PARAM_CHK((vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);

    val = vid;

    OAM_SEM_LOCK(unit);

    /* set value to chip */
    if ((ret = reg_array_field_write(unit, CYPRESS_CCM_RX_INST_CTRLr,
                    REG_ARRAY_INDEX_NONE, instance, CYPRESS_VIDf,
                    &val) ) != RT_ERR_OK)
    {
        OAM_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OAM), "");
        return ret;
    }

    OAM_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_cypress_oam_cfmCcmRxInstVid_set */

/* Function Name:
 *      dal_cypress_oam_cfmCcmRxInstPort_get
 * Description:
 *      Get rx instance member.
 * Input:
 *      unit        - unit id
 *      instance    - rx control entry instance
 *      index       - instance member index
 * Output:
 *      port    - pointer buffer
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_oam_cfmCcmRxInstPort_get(uint32 unit, uint32 instance,
                                     uint32 index, rtk_port_t *port)
{
    int32   ret;
    uint32  portField;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OAM), "unit=%d instance=%d index=%d",
           unit, instance, index);

    /* check Init status */
    RT_INIT_CHK(oam_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((instance >= RTK_CFM_RX_INSTANCE_MAX), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == port), RT_ERR_NULL_POINTER);

    if (0 == index)
        portField = CYPRESS_P0f;
    else if (1 == index)
        portField = CYPRESS_P1f;
    else
        return RT_ERR_INPUT;

    OAM_SEM_LOCK(unit);

    /* get value from chip */
    if ((ret = reg_array_field_read(unit, CYPRESS_CCM_RX_INST_CTRLr,
                    REG_ARRAY_INDEX_NONE, instance, portField,
                    port) ) != RT_ERR_OK)
    {
        OAM_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OAM), "");
        return ret;
    }

    OAM_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_cypress_oam_cfmCcmRxInstPort_get */

/* Function Name:
 *      dal_cypress_oam_cfmCcmRxInstPort_set
 * Description:
 *      Set rx instance member.
 * Input:
 *      unit        - unit id
 *      instance    - rx control entry instance
 *      index       - instance member index
 *      port        - transmit interval
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
int32
dal_cypress_oam_cfmCcmRxInstPort_set(uint32 unit, uint32 instance,
                                     uint32 index, rtk_port_t port)
{
    int32   ret;
    uint32  portField;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OAM), "unit=%d instance=%d index=%d port=%d",
           unit, instance, index, port);

    /* check Init status */
    RT_INIT_CHK(oam_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((instance >= RTK_CFM_TX_INSTANCE_MAX), RT_ERR_INPUT);
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);

    val = port;

    if (0 == index)
        portField = CYPRESS_P0f;
    else if (1 == index)
        portField = CYPRESS_P1f;
    else
        return RT_ERR_INPUT;

    OAM_SEM_LOCK(unit);

    /* set value to chip */
    if ((ret = reg_array_field_write(unit, CYPRESS_CCM_RX_INST_CTRLr,
                    REG_ARRAY_INDEX_NONE, instance, portField,
                    &val) ) != RT_ERR_OK)
    {
        OAM_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OAM), "");
        return ret;
    }

    OAM_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_cypress_oam_cfmCcmRxInstPort_set */

/* Function Name:
 *      dal_cypress_oam_cfmCcmKeepalive_get
 * Description:
 *      Get rx instance member.
 * Input:
 *      unit        - unit id
 *      instance    - rx control entry instance
 *      index       - instance member index
 * Output:
 *      keepalive   - pointer buffer
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_oam_cfmCcmKeepalive_get(uint32 unit, uint32 instance,
                                    uint32 index, uint32 *keepalive)
{
    int32   ret;
    uint32  reg;
    uint32  field;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OAM), "unit=%d instance=%d index=%d",
           unit, instance, index);

    /* check Init status */
    RT_INIT_CHK(oam_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((instance >= RTK_CFM_TX_INSTANCE_MAX), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == keepalive), RT_ERR_NULL_POINTER);

    if (0 == index)
    {
        reg     = CYPRESS_CCM_INST_CNTR_0r;
        field   = CYPRESS_CNTR0f;
    }
    else if (1 == index)
    {
        reg     = CYPRESS_CCM_INST_CNTR_1r;
        field   = CYPRESS_CNTR1f;
    }
    else
    {
        return RT_ERR_INPUT;
    }

    OAM_SEM_LOCK(unit);

    /* get value from chip */
    if ((ret = reg_array_field_read(unit, reg, REG_ARRAY_INDEX_NONE, instance,
                    field, keepalive) ) != RT_ERR_OK)
    {
        OAM_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OAM), "");
        return ret;
    }

    OAM_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_cypress_oam_cfmCcmKeepalive_get */

/* Function Name:
 *      dal_cypress_oam_cfmPortEthDmEnable_get
 * Description:
 *      Get enable status of CFM ETH-DM feature on the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_oam_cfmPortEthDmEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OAM), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(oam_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    OAM_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_array_field_read(unit, CYPRESS_ETH_DM_PORT_ENr,
                    port, REG_ARRAY_INDEX_NONE, CYPRESS_ENf, &val) ) != RT_ERR_OK)
    {
        OAM_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OAM), "");
        return ret;
    }

    OAM_SEM_UNLOCK(unit);

    *pEnable = (val)? ENABLED : DISABLED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OAM), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
} /* end of dal_cypress_oam_cfmPortEthDmEnable_get */

/* Function Name:
 *      dal_cypress_oam_cfmPortEthDmEnable_set
 * Description:
 *      Set enable status of CFM ETH-DM feature on the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      enable  - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
int32
dal_cypress_oam_cfmPortEthDmEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OAM), "unit=%d, port=%d, enable=%d", unit, port, enable);

    /* check Init status */
    RT_INIT_CHK(oam_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    val = (ENABLED == enable)? 1 : 0;

    OAM_SEM_LOCK(unit);

    /* set forward action */
    if ((ret = reg_array_field_write(unit, CYPRESS_ETH_DM_PORT_ENr,
                    port, REG_ARRAY_INDEX_NONE, CYPRESS_ENf, &val) ) != RT_ERR_OK)
    {
        OAM_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OAM), "");
        return ret;
    }

    OAM_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_oam_cfmPortEthDmEnable_set */

/* Function Name:
 *      dal_cypress_oam_cfmEthDmRxTimestamp_get
 * Description:
 *      Get ETH-DM ingress timestamp according to the entry index from the specified device.
 * Input:
 *      unit       - unit id
 *      index      - entry index
 * Output:
 *      pTimeStamp - pointer buffer of ingress timestamp
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      1. Each timestamp can only be read once. (it will be reset after it has been read.)
 *      2. Timestamps have to be read sequentially (the order of index) or it will be skipped.
 */
extern int32
dal_cypress_oam_cfmEthDmRxTimestamp_get(uint32 unit, uint32 index, rtk_time_timeStamp_t *pTimeStamp)
{
    int32   ret;
    uint32  idx, nsec, sec;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OAM), "unit=%d, index=%d", unit, index);

    /* check Init status */
    RT_INIT_CHK(oam_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(index >= HAL_MAX_NUM_OF_ETHDM_RX_TIMESTAMP(unit), RT_ERR_INPUT);

    OAM_SEM_LOCK(unit);

    /* get the rx-timestamp with the specified index */
    do {
        idx = ethDmRxTimestampIdx;

        /* must read nsec (lower address) first to trigger H/W to latch the internal data */
        if ((ret = reg_array_field_read(unit, CYPRESS_ETH_DM_RX_TIMEr,  REG_ARRAY_INDEX_NONE,
                                        idx, CYPRESS_NSECf, &nsec)) != RT_ERR_OK)
        {
            OAM_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_OAM), "");
            return ret;
        }

        if ((ret = reg_array_field_read(unit, CYPRESS_ETH_DM_RX_TIMEr, REG_ARRAY_INDEX_NONE,
                                        idx, CYPRESS_SECf, &sec)) != RT_ERR_OK)
        {
            OAM_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_OAM), "");
            return ret;
        }

        /* jump to the next entry */
        ethDmRxTimestampIdx = (idx + 1) % HAL_MAX_NUM_OF_ETHDM_RX_TIMESTAMP(unit);
    } while (index != idx);

    OAM_SEM_UNLOCK(unit);

    /* return value */
    pTimeStamp->nsec = nsec;
    pTimeStamp->sec = sec;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_oam_dyingGaspPktCnt_get
 * Description:
 *      Get dying gasp send packet count.
 * Input:
 *      unit    - unit id
 * Output:
 *      pCnt    - packet count configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_oam_dyingGaspPktCnt_get(uint32 unit, uint32 *pCnt)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OAM), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(oam_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pCnt), RT_ERR_NULL_POINTER);

    OAM_SEM_LOCK(unit);

    if ((ret = reg_field_read(unit, CYPRESS_OAM_GLB_DYING_GASP_CTRLr,
                              CYPRESS_DYING_GASP_PKTCNTf, pCnt) ) != RT_ERR_OK)
    {
        OAM_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OAM), "");
        return ret;
    }

    OAM_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_cypress_oam_dyingGaspPktCnt_get */

/* Function Name:
 *      dal_cypress_oam_dyingGaspPktCnt_set
 * Description:
 *      Set dying gasp send packet count.
 * Input:
 *      unit    - unit id
 *      enable  - trigger dying gasp with enabled ports.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      The packet count is 0 ~ 7.
 */
int32
dal_cypress_oam_dyingGaspPktCnt_set(uint32 unit, uint32 cnt)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OAM), "unit=%d, cnt=%d", unit, cnt);

    /* check Init status */
    RT_INIT_CHK(oam_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_DYING_GASP_PKT_CNT(unit) < cnt), RT_ERR_INPUT);

    OAM_SEM_LOCK(unit);

    if ((ret = reg_field_write(unit, CYPRESS_OAM_GLB_DYING_GASP_CTRLr,
                    CYPRESS_DYING_GASP_PKTCNTf, &cnt) ) != RT_ERR_OK)
    {
        OAM_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OAM), "");
        return ret;
    }

    OAM_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_cypress_oam_dyingGaspPktCnt_set */

