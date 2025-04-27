/*
 * Copyright (C) 2011 Realtek Semiconductor Corp.
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
#include <hal/chipdef/longan/rtk_longan_reg_struct.h>
#include <hal/mac/reg.h>
#include <hal/common/halctrl.h>
#include <rtk/oam.h>
#include <dal/dal_mapper.h>
#include <dal/longan/dal_longan_oam.h>


/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
static uint32               oam_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};
static osal_mutex_t         oam_sem[RTK_MAX_NUM_OF_UNIT];

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

/* Module Name : OAM */

/* Function Name:
 *      dal_longan_oamMapper_init
 * Description:
 *      Hook oam module of the specified device.
 * Input:
 *      pMapper - pointer of mapper
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Must Hook oam module before calling any oam APIs.
 */
int32
dal_longan_oamMapper_init(dal_mapper_t *pMapper)
{

    pMapper->oam_init = dal_longan_oam_init;
    pMapper->oam_portDyingGaspPayload_set = dal_longan_oam_portDyingGaspPayload_set;
    pMapper->oam_dyingGaspSend_set = dal_longan_oam_dyingGaspSend_set;
    pMapper->oam_autoDyingGaspEnable_get = dal_longan_oam_autoDyingGaspEnable_get;
    pMapper->oam_autoDyingGaspEnable_set = dal_longan_oam_autoDyingGaspEnable_set;
    pMapper->oam_dyingGaspWaitTime_get = dal_longan_oam_dyingGaspWaitTime_get;
    pMapper->oam_dyingGaspWaitTime_set = dal_longan_oam_dyingGaspWaitTime_set;
    pMapper->oam_loopbackMacSwapEnable_get = dal_longan_oam_loopbackMacSwapEnable_get;
    pMapper->oam_loopbackMacSwapEnable_set = dal_longan_oam_loopbackMacSwapEnable_set;
    pMapper->oam_portLoopbackMuxAction_get = dal_longan_oam_portLoopbackMuxAction_get;
    pMapper->oam_portLoopbackMuxAction_set = dal_longan_oam_portLoopbackMuxAction_set;
    pMapper->oam_dyingGaspPktCnt_get = dal_longan_oam_dyingGaspPktCnt_get;
    pMapper->oam_dyingGaspPktCnt_set = dal_longan_oam_dyingGaspPktCnt_set;
    pMapper->oam_pduLearningEnable_get = dal_longan_oam_pduLearningEnable_get;
    pMapper->oam_pduLearningEnable_set = dal_longan_oam_pduLearningEnable_set;

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_longan_oam_init
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
dal_longan_oam_init(uint32 unit)
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
}   /* end of dal_longan_oam_init */

/* Module Name : OAM */
#if defined(CONFIG_SDK_DRIVER_NIC)
static void
_longan_oam_dyingGasp_tx_callback(uint32 unit, drv_nic_pkt_t *pPacket, void *pCookie)
{
    if (pCookie != NULL)
        drv_nic_pkt_free(unit, pCookie);

    if (pPacket != NULL)
        drv_nic_pkt_free(unit, pPacket);
    return;
}
#endif

/* Function Name:
 *      dal_longan_oam_portDyingGaspPayload_set
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
dal_longan_oam_portDyingGaspPayload_set(uint32 unit, rtk_port_t port,
    uint8 *pPayload, uint32 len)
{
#if defined(CONFIG_SDK_DRIVER_NIC)
    int32   ret;
    drv_nic_pkt_t *pPacket;

    /* check Init status */
    RT_INIT_CHK(oam_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pPayload), RT_ERR_NULL_POINTER);

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
    pPacket->as_txtag               = TRUE;
    pPacket->tx_tag.fwd_type        = NIC_FWD_TYPE_PHYISCAL;
    pPacket->tx_tag.dg_pkt          = TRUE;
    pPacket->tx_tag.bp_fltr         = TRUE;
    pPacket->tx_tag.bp_stp          = TRUE;
    pPacket->tx_tag.bp_vlan_egr     = TRUE;
    pPacket->tx_tag.dst_port_mask   = ((uint32)1 << port) & 0xffffffff;

    ret = drv_nic_pkt_tx(unit, pPacket, _longan_oam_dyingGasp_tx_callback, NULL);
    if (ret)
    {
        drv_nic_pkt_free(unit, pPacket);
        return RT_ERR_FAILED;
    }
#endif

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_oam_DyingGaspSend_set
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
dal_longan_oam_dyingGaspSend_set(uint32 unit, rtk_enable_t enable)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OAM), "unit=%d en=%d", unit, enable);

    /* check Init status */
    RT_INIT_CHK(oam_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    OAM_SEM_LOCK(unit);

    /* Trigger Dying Gasp Function */
    if ((ret = reg_field_write(unit, LONGAN_OAM_GLB_DYING_GASP_CTRLr,
                    LONGAN_DYING_GASP_TRIGf, &enable) ) != RT_ERR_OK)
    {
        OAM_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OAM), "");
        return ret;
    }

    OAM_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_longan_oam_dyingGaspSend_set */

/* Function Name:
 *       dal_longan_oam_dyingGaspWaitTime_get
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
 *      Granularity of waiting time is 40 ns.
 */
int32
dal_longan_oam_dyingGaspWaitTime_get(uint32 unit, uint32 *pTime)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OAM), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(oam_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pTime), RT_ERR_NULL_POINTER);

    OAM_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_field_read(unit, LONGAN_OAM_GLB_DYING_GASP_CTRLr,
                              LONGAN_TBP_VALf, pTime) ) != RT_ERR_OK)
    {
        OAM_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OAM), "");
        return ret;
    }

    OAM_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OAM), "pTime=%d", *pTime);

    return RT_ERR_OK;
}   /*end of dal_longan_oam_dyingGaspWaitTime_get*/

/* Function Name:
 *      dal_longan_oam_dyingGaspWaitTime_set
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
 *      Granularity of waiting time is 40 ns.
 */
int32
dal_longan_oam_dyingGaspWaitTime_set(uint32 unit, uint32 time)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OAM), "unit=%d, time=%d", unit, time);

    /* check Init status */
    RT_INIT_CHK(oam_init[unit]);

    RT_PARAM_CHK(time > HAL_DYING_GASP_SUSTAIN_TIME_MAX(unit), RT_ERR_OUT_OF_RANGE);

    OAM_SEM_LOCK(unit);

    /* set the time duration */
    if ((ret = reg_field_write(unit, LONGAN_OAM_GLB_DYING_GASP_CTRLr,
                               LONGAN_TBP_VALf, &time) ) != RT_ERR_OK)
    {
        OAM_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OAM), "");
        return ret;
    }

    OAM_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_longan_oam_dyingGaspWaitTime_set */

/* Function Name:
 *      dal_longan_oam_autoDyingGaspEnable_get
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
dal_longan_oam_autoDyingGaspEnable_get(uint32 unit, rtk_port_t port,
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
    if ((ret = reg_array_field_read(unit, LONGAN_OAM_PORT_DYING_GASP_CTRLr,
                    port, REG_ARRAY_INDEX_NONE, LONGAN_PORT_OAM_DYING_GASP_ENf,
                    pEnable) ) != RT_ERR_OK)
    {
        OAM_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OAM), "");
        return ret;
    }

    OAM_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OAM), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
}   /* end of dal_longan_oam_autoDyingGaspEnable_get */

/* Function Name:
 *      dal_longan_oam_autoDyingGaspEnable_set
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
dal_longan_oam_autoDyingGaspEnable_set(uint32 unit, rtk_port_t port,
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
    if ((ret = reg_array_field_write(unit, LONGAN_OAM_PORT_DYING_GASP_CTRLr,
                    port, REG_ARRAY_INDEX_NONE, LONGAN_PORT_OAM_DYING_GASP_ENf,
                    &en) ) != RT_ERR_OK)
    {
        OAM_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OAM), "");
        return ret;
    }

    OAM_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_longan_oam_autoDyingGaspEnable_set */

/* Function Name:
 *      dal_longan_oam_loopbackMacSwapEnable_get
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
dal_longan_oam_loopbackMacSwapEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    int32   ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OAM), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(oam_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    OAM_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_field_read(unit, LONGAN_OAM_CTRLr, LONGAN_MAC_SWAPf,
                    pEnable) ) != RT_ERR_OK)
    {
        OAM_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OAM), "");
        return ret;
    }

    OAM_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OAM), "pEnable=%d", pEnable);

    return RT_ERR_OK;
}   /* end of dal_longan_oam_loopbackMacSwapEnable_get */

/* Function Name:
 *      dal_longan_oam_loopbackMacSwapEnable_set
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
dal_longan_oam_loopbackMacSwapEnable_set(uint32 unit, rtk_enable_t enable)
{
    int32   ret = RT_ERR_FAILED;
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
    if ((ret = reg_field_write(unit, LONGAN_OAM_CTRLr, LONGAN_MAC_SWAPf,
                    &en) ) != RT_ERR_OK)
    {
        OAM_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OAM), "");
        return ret;
    }

    OAM_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_longan_oam_loopbackMacSwapEnable_set */

/* Function Name:
 *      dal_longan_oam_portLoopbackMuxAction_get
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
dal_longan_oam_portLoopbackMuxAction_get(uint32 unit,
    rtk_port_t port, rtk_action_t *pAction)
{
    int32   ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OAM), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(oam_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    OAM_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_array_field_read(unit, LONGAN_OAM_PORT_ACT_CTRLr,
                    port, REG_ARRAY_INDEX_NONE, LONGAN_MUX_ACTf,
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
}   /* end of dal_longan_oam_portLoopbackMuxAction_get */

/* Function Name:
 *      dal_longan_oam_portLoopbackMuxAction_set
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
dal_longan_oam_portLoopbackMuxAction_set(uint32 unit,
    rtk_port_t port, rtk_action_t action)
{
    int32   ret = RT_ERR_FAILED;
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
    if ((ret = reg_array_field_write(unit, LONGAN_OAM_PORT_ACT_CTRLr,
                    port, REG_ARRAY_INDEX_NONE, LONGAN_MUX_ACTf,
                    &val)) != RT_ERR_OK)
    {
        OAM_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OAM), "");
        return ret;
    }

    OAM_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_longan_oam_portLoopbackMuxAction_set */

/* Function Name:
 *      dal_longan_oam_dyingGaspPktCnt_get
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
dal_longan_oam_dyingGaspPktCnt_get(uint32 unit, uint32 *pCnt)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OAM), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(oam_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pCnt), RT_ERR_NULL_POINTER);

    OAM_SEM_LOCK(unit);

    if ((ret = reg_field_read(unit, LONGAN_OAM_GLB_DYING_GASP_CTRLr,
                              LONGAN_DYING_GASP_PKTCNTf, pCnt) ) != RT_ERR_OK)
    {
        OAM_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OAM), "");
        return ret;
    }

    OAM_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_longan_oam_dyingGaspPktCnt_get */

/* Function Name:
 *      dal_longan_oam_dyingGaspPktCnt_set
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
dal_longan_oam_dyingGaspPktCnt_set(uint32 unit, uint32 cnt)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OAM), "unit=%d, cnt=%d", unit, cnt);

    /* check Init status */
    RT_INIT_CHK(oam_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_DYING_GASP_PKT_CNT(unit) < cnt), RT_ERR_INPUT);

    OAM_SEM_LOCK(unit);

    if ((ret = reg_field_write(unit, LONGAN_OAM_GLB_DYING_GASP_CTRLr,
                    LONGAN_DYING_GASP_PKTCNTf, &cnt) ) != RT_ERR_OK)
    {
        OAM_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OAM), "");
        return ret;
    }

    OAM_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_longan_oam_dyingGaspPktCnt_set */

/*
 * Function Name:
 *      dal_longan_oam_pduLearningEnable_get
 * Description:
 *      Get enable status of SMAC learning for OAM PDU.
 * Input:
 *      unit       - unit id
 * Output:
 *      pEnable    - pointer to enable status of SMAC learning
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_RMA_ADDR     - invalid invalid RMA address
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_longan_oam_pduLearningEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    uint32  val;
    int32   ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OAM), "unit=%d",unit);

    /* check Init status */
    RT_INIT_CHK(oam_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    OAM_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_field_read(unit, LONGAN_OAM_CTRLr, LONGAN_LRNf,
            &val) ) != RT_ERR_OK)
    {
        OAM_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OAM), "");
        return ret;
    }

    OAM_SEM_UNLOCK(unit);

    if (0 == val)
    {
        *pEnable = DISABLED;
    }
    else
    {
        *pEnable = ENABLED;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OAM), "pEnable=%d", pEnable);

    return RT_ERR_OK;
}   /* end of dal_longan_oam_pduLearningEnable_get */

/* Function Name:
 *      dal_longan_oam_pduLearningEnable_set
 * Description:
 *      Set enable status of SMAC learning for OAM PDU.
 * Input:
 *      unit       - unit id
 *      enable     - enable status of SMAC learning
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_RMA_ADDR     - invalid invalid RMA address
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      None
 */
int32
dal_longan_oam_pduLearningEnable_set(uint32 unit, rtk_enable_t enable)
{
    uint32  val;
    int32   ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OAM), "unit=%d,enable=%d",unit, enable);

    /* check Init status */
    RT_INIT_CHK(oam_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    /* function body */
    if (DISABLED == enable)
        val = 0;
    else if (ENABLED == enable)
        val = 1;

    OAM_SEM_LOCK(unit);

    /* set value to CHIP */
    if ((ret = reg_field_write(unit, LONGAN_OAM_CTRLr, LONGAN_LRNf,
            &val) ) != RT_ERR_OK)
    {
        OAM_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_OAM), "");
        return ret;
    }

    OAM_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_longan_oam_pduLearningEnable_set */


