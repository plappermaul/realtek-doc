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
 * Purpose : Definition those public OAM & CFM APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *              1) OAM
 *              2) CFM
 */

#ifndef __DAL_MANGO_OAM_H__
#define __DAL_MANGO_OAM_H__


/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <dal/dal_mapper.h>

/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */


/*
 * Macro Declaration
 */


/*
 * Function Declaration
 */

/* Function Name:
 *      dal_mango_oamMapper_init
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
extern int32
dal_mango_oamMapper_init(dal_mapper_t *pMapper);

/* Module Name : OAM */
/* Function Name:
 *      dal_mango_oam_init
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
extern int32
dal_mango_oam_init(uint32 unit);

/* Function Name:
 *      dal_mango_oam_loopbackMacSwapEnable_get
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
 *      Take swap action when OAM loopback status is enable and
 *      parser in "Loopback" state.
 */
extern int32
dal_mango_oam_loopbackMacSwapEnable_get(uint32 unit, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_mango_oam_loopbackMacSwapEnable_set
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
 *      Take swap action when OAM loopback status is enable and
 *      parser in "Loopback" state.
 */
extern int32
dal_mango_oam_loopbackMacSwapEnable_set(uint32 unit, rtk_enable_t enable);

/* Function Name:
 *      dal_mango_oam_portLoopbackMuxAction_get
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
extern int32
dal_mango_oam_portLoopbackMuxAction_get(uint32 unit,
        rtk_port_t port, rtk_action_t *pAction);

/* Function Name:
 *      dal_mango_oam_portLoopbackMuxAction_set
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
extern int32
dal_mango_oam_portLoopbackMuxAction_set(uint32 unit,
    rtk_port_t port, rtk_action_t action);

/* Function Name:
 *      dal_mango_oam_portDyingGaspPayload_set
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
extern int32
dal_mango_oam_portDyingGaspPayload_set(uint32 unit, rtk_port_t port,
    uint8 *pPayload, uint32 len);

/* Function Name:
 *      dal_mango_oam_dyingGaspSend_set
 * Description:
 *      Start sending dying gasp frame to specified ports.
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
 *      This API is used when CPU want to send dying gasp by itself.
 */
extern int32
dal_mango_oam_dyingGaspSend_set(uint32 unit, rtk_enable_t enable);

/* Function Name:
 *       dal_mango_oam_dyingGaspWaitTime_get
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
extern int32
dal_mango_oam_dyingGaspWaitTime_get(uint32 unit, uint32 *pTime);

/* Function Name:
 *      dal_mango_oam_dyingGaspWaitTime_set
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
extern int32
dal_mango_oam_dyingGaspWaitTime_set(uint32 unit, uint32 time);

/* Function Name:
 *      dal_mango_oam_autoDyingGaspEnable_get
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
extern int32
dal_mango_oam_autoDyingGaspEnable_get(uint32 unit, rtk_port_t port,
    rtk_enable_t *pEnable);

/* Function Name:
 *      dal_mango_oam_autoDyingGaspEnable_set
 * Description:
 *      Set enable status of sending dying gasp automatically on specified port
 *      when voltage is lower than expected.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      enable  - enable status of sending dying gasp automatically
 * Output:
 *      pEnable - pointer buffer
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_mango_oam_autoDyingGaspEnable_set(uint32 unit, rtk_port_t port,
    rtk_enable_t enable);

/* Function Name:
 *      dal_mango_oam_cfmCcmPcp_get
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
extern int32
dal_mango_oam_cfmCcmPcp_get(uint32 unit, uint32 *pcp);

/* Function Name:
 *      dal_mango_oam_cfmCcmPcp_set
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
extern int32
dal_mango_oam_cfmCcmPcp_set(uint32 unit, uint32 pcp);

/* Function Name:
 *      dal_mango_oam_cfmCcmCfi_get
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
extern int32
dal_mango_oam_cfmCcmCfi_get(uint32 unit, uint32 *cfi);

/* Function Name:
 *      dal_mango_oam_cfmCcmCfi_set
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
extern int32
dal_mango_oam_cfmCcmCfi_set(uint32 unit, uint32 cfi);

/* Function Name:
 *      dal_mango_oam_cfmCcmTpid_get
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
extern int32
dal_mango_oam_cfmCcmTpid_get(uint32 unit, uint32 *tpid);

/* Function Name:
 *      dal_mango_oam_cfmCcmTpid_set
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
extern int32
dal_mango_oam_cfmCcmTpid_set(uint32 unit, uint32 tpid);

/* Function Name:
 *      dal_mango_oam_cfmCcmInstLifetime_get
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
extern int32
dal_mango_oam_cfmCcmInstLifetime_get(uint32 unit, uint32 instance,
    uint32 *lifetime);

/* Function Name:
 *      dal_mango_oam_cfmCcmInstLifetime_set
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
extern int32
dal_mango_oam_cfmCcmInstLifetime_set(uint32 unit, uint32 instance,
    uint32 lifetime);

/* Function Name:
 *      dal_mango_oam_cfmCcmInstTxMepid_get
 * Description:
 *      Get MEPID to be inserted to generated CCM frame.
 * Input:
 *      unit        - unit id
 *      instance    - tx control entry instance
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
extern int32
dal_mango_oam_cfmCcmInstTxMepid_get(uint32 unit, uint32 instance, uint32 *mepid);

/* Function Name:
 *      dal_mango_oam_cfmCcmInstTxMepid_set
 * Description:
 *      Set MEPID to be inserted to generated CCM frame.
 * Input:
 *      unit        - unit id
 *      instance    - tx control entry instance
 *      mepid       - reset ERP counter time.
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
extern int32
dal_mango_oam_cfmCcmInstTxMepid_set(uint32 unit, uint32 instance, uint32 mepid);

/* Function Name:
 *      dal_mango_oam_cfmCcmInstTxIntervalField_get
 * Description:
 *      Get value to be inserted to interval field in flag for generated CCM frame.
 * Input:
 *      unit        - unit id
 *      instance    - tx control entry instance
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
extern int32
dal_mango_oam_cfmCcmInstTxIntervalField_get(uint32 unit, uint32 instance, uint32 *interval);

/* Function Name:
 *      dal_mango_oam_cfmCcmInstTxIntervalField_set
 * Description:
 *      Set value to be inserted to interval field in flag for generated CCM frame.
 * Input:
 *      unit        - unit id
 *      instance    - tx control entry instance
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
extern int32
dal_mango_oam_cfmCcmInstTxIntervalField_set(uint32 unit, uint32 instance, uint32 interval);

/* Function Name:
 *      dal_mango_oam_cfmCcmInstTxMdl_get
 * Description:
 *      Get MD level to be inserted to generated CCM frame.
 * Input:
 *      unit        - unit id
 *      instance    - tx control entry instance
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
extern int32
dal_mango_oam_cfmCcmInstTxMdl_get(uint32 unit, uint32 instance, uint32 *mdl);

/* Function Name:
 *      dal_mango_oam_cfmCcmInstTxMdl_set
 * Description:
 *      Set MD level to be inserted to generated CCM frame.
 * Input:
 *      unit        - unit id
 *      instance    - tx control entry instance
 *      mdl         - MD level insert to CCM frame
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
extern int32
dal_mango_oam_cfmCcmInstTxMdl_set(uint32 unit, uint32 instance, uint32 mdl);

/* Function Name:
 *      dal_mango_oam_cfmCcmInstTagStatus_get
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
extern int32
dal_mango_oam_cfmCcmInstTagStatus_get(uint32 unit, uint32 instance,
    rtk_enable_t *enable);

/* Function Name:
 *      dal_mango_oam_cfmCcmInstTagStatus_set
 * Description:
 *      Set tag status for instance member to be inserted to generated CCM frame.
 * Input:
 *      unit        - unit id
 *      instance    - tx control entry instance
 *      enable  - tag status.
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
extern int32
dal_mango_oam_cfmCcmInstTagStatus_set(uint32 unit, uint32 instance,
    rtk_enable_t enable);

/* Function Name:
 *      dal_mango_oam_cfmCcmInstVid_get
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
extern int32
dal_mango_oam_cfmCcmInstVid_get(uint32 unit, uint32 instance,
    rtk_vlan_t *vid);

/* Function Name:
 *      dal_mango_oam_cfmCcmInstVid_set
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
extern int32
dal_mango_oam_cfmCcmInstVid_set(uint32 unit, uint32 instance,
    rtk_vlan_t vid);

/* Function Name:
 *      dal_mango_oam_cfmCcmInstMaid_get
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
extern int32
dal_mango_oam_cfmCcmInstMaid_get(uint32 unit, uint32 instance,
    uint32 *maid);

/* Function Name:
 *      dal_mango_oam_cfmCcmInstMaid_set
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
extern int32
dal_mango_oam_cfmCcmInstMaid_set(uint32 unit, uint32 instance,
    uint32 maid);

/* Function Name:
 *      dal_mango_oam_cfmCcmInstTxStatus_get
 * Description:
 *      Get tx status for instance member to generated CCM frame.
 * Input:
 *      unit        - unit id
 *      instance    - tx control entry instance
 * Output:
 *      enable  - pointer buffer
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_mango_oam_cfmCcmInstTxStatus_get(uint32 unit, uint32 instance,
    rtk_enable_t *enable);

/* Function Name:
 *      dal_mango_oam_cfmCcmInstTxStatus_set
 * Description:
 *      Set tx status for instance member to generated CCM frame.
 * Input:
 *      unit        - unit id
 *      instance    - tx control entry instance
 *      enable      - tx status.
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
extern int32
dal_mango_oam_cfmCcmInstTxStatus_set(uint32 unit, uint32 instance,
    rtk_enable_t enable);

/* Function Name:
 *      dal_mango_oam_cfmInstInterval_get
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
extern int32
dal_mango_oam_cfmCcmInstInterval_get(uint32 unit, uint32 instance,
    uint32 *interval);

/* Function Name:
 *      dal_mango_oam_cfmCcmInstInterval_set
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
extern int32
dal_mango_oam_cfmCcmInstInterval_set(uint32 unit, uint32 instance,
    uint32 interval);

/* Function Name:
 *      dal_mango_oam_cfmCcmRxInstVid_get
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
extern int32
dal_mango_oam_cfmCcmRxInstVid_get(uint32 unit, uint32 instance,
    rtk_vlan_t *vid);

/* Function Name:
 *      dal_mango_oam_cfmCcmRxInstVid_set
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
extern int32
dal_mango_oam_cfmCcmRxInstVid_set(uint32 unit, uint32 instance,
    rtk_vlan_t vid);

/* Function Name:
 *      dal_mango_oam_cfmCcmKeepalive_get
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
extern int32
dal_mango_oam_cfmCcmKeepalive_get(uint32 unit, uint32 instance,
    uint32 index, uint32 *keepalive);

/* Function Name:
 *      dal_mango_oam_cfmPortEthDmEnable_get
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
extern int32
dal_mango_oam_cfmPortEthDmEnable_get(uint32 unit, rtk_port_t port,
    rtk_enable_t *pEnable);

/* Function Name:
 *      dal_mango_oam_cfmPortEthDmEnable_set
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
extern int32
dal_mango_oam_cfmPortEthDmEnable_set(uint32 unit, rtk_port_t port,
    rtk_enable_t enable);

/* Function Name:
 *      dal_mango_oam_cfmEthDmRxTimestamp_get
 * Description:
 *      Get ETH-DM ingress timestamp according to the entry index from the specified device.
 * Input:
 *      unit       - unit id
 *      index     - entry index
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
 *      None
 */
extern int32
dal_mango_oam_cfmEthDmRxTimestamp_get(uint32 unit, uint32 index,
    rtk_time_timeStamp_t *pTimeStamp);

/* Function Name:
 *      dal_mango_oam_cfmEthDmTxDelay_get
 * Description:
 *      Get ETH-DM egress delay adjustment of all link speeds from the specified device.
 * Input:
 *      unit     - unit id
 * Output:
 *      pTxDelay - pointer buffer of egress delay adjustment of all link speeds (unit:8 nanoseconds)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_oam_cfmEthDmTxDelay_get(uint32 unit,
    rtk_oam_cfmEthDmTxDelay_t *pTxDelay);

/* Function Name:
 *      dal_mango_oam_cfmEthDmTxDelay_set
 * Description:
 *      Set ETH-DM egress delay adjustment of all link up speeds from the specified device.
 * Input:
 *      unit     - unit id
 *      txDelay  - Egress delay adjustment of all link speeds (unit:8 nanoseconds)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 * Note:
 *      None
 */
extern int32
dal_mango_oam_cfmEthDmTxDelay_set(uint32 unit,
    rtk_oam_cfmEthDmTxDelay_t txDelay);

/* Function Name:
 *      dal_mango_oam_cfmEthDmRefTime_get
 * Description:
 *      Get the ETH-DM reference time of the specified device.
 * Input:
 *      unit       - unit id
 * Output:
 *      pTimeStamp - pointer buffer of the reference time
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_oam_cfmEthDmRefTime_get(uint32 unit, rtk_time_timeStamp_t *pTimeStamp);

/* Function Name:
 *      dal_mango_oam_cfmEthDmRefTime_set
 * Description:
 *      Set the ETH-DM reference time of the specified device.
 * Input:
 *      unit      - unit id
 *      timeStamp - reference timestamp value
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
extern int32
dal_mango_oam_cfmEthDmRefTime_set(uint32 unit, rtk_time_timeStamp_t timeStamp);

/* Function Name:
 *      dal_mango_oam_cfmEthDmRefTimeEnable_get
 * Description:
 *      Get the enable state of ETH-DM reference time of the specified device.
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
extern int32
dal_mango_oam_cfmEthDmRefTimeEnable_get(uint32 unit, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_mango_oam_cfmEthDmRefTimeEnable_set
 * Description:
 *      Set the enable state of ETH-DM reference time of the specified device.
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
extern int32
dal_mango_oam_cfmEthDmRefTimeEnable_set(uint32 unit, rtk_enable_t enable);

/* Function Name:
 *      dal_mango_oam_cfmEthDmRefTimeFreq_get
 * Description:
 *      Get the frequency of ETH-DM reference time of the specified device.
 * Input:
 *      unit    - unit id
 * Output:
 *      pFreq  - pointer to reference time frequency
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The frequency configuration decides the reference time tick frequency.
 *      The default value is 0x8000000.
 *      If it is configured to 0x4000000, the tick frequency would be half of default.
 *      If it is configured to 0xC000000, the tick frequency would be one and half times of default.
 *
 */
extern int32
dal_mango_oam_cfmEthDmRefTimeFreq_get(uint32 unit, uint32 *pFreq);

/* Function Name:
 *      dal_mango_oam_cfmEthDmRefTimeFreq_set
 * Description:
 *      Set the frequency of ETH-DM reference time of the specified device.
 * Input:
 *      unit   - unit id
 *      freq   - reference time frequency
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      The frequency configuration decides the reference time tick frequency.
 *      The default value is 0x8000000.
 *      If it is configured to 0x4000000, the tick frequency would be half of default.
 *      If it is configured to 0xC000000, the tick frequency would be one and half times of default.
 */
extern int32
dal_mango_oam_cfmEthDmRefTimeFreq_set(uint32 unit, uint32 freq);

/* Function Name:
 *      dal_mango_oam_dyingGaspPktCnt_get
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
extern int32
dal_mango_oam_dyingGaspPktCnt_get(uint32 unit, uint32 *pCnt);

/* Function Name:
 *      dal_mango_oam_dyingGaspPktCnt_set
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
extern int32
dal_mango_oam_dyingGaspPktCnt_set(uint32 unit, uint32 cnt);

/*
 * Function Name:
 *      dal_mango_oam_pduLearningEnable_get
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
extern int32
dal_mango_oam_pduLearningEnable_get(uint32 unit, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_mango_oam_pduLearningEnable_set
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
extern int32
dal_mango_oam_pduLearningEnable_set(uint32 unit, rtk_enable_t enable);

/* Function Name:
 *      dal_mango_oam_cfmCcmInstTxMember_get
 * Description:
 *      Get tx instance member.
 * Input:
 *      unit        - unit id
 *      instance    - tx control entry instance
 * Output:
 *      member      - Tx intance member configuration
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
extern int32
dal_mango_oam_cfmCcmInstTxMember_get(uint32 unit, uint32 instance,
    rtk_oam_cfmInstMember_t *member);

/* Function Name:
 *      dal_mango_oam_cfmCcmInstTxMember_set
 * Description:
 *      Set tx instance member.
 * Input:
 *      unit        - unit id
 *      instance    - tx control entry instance
 *      member      - Tx intance member configuration
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
extern int32
dal_mango_oam_cfmCcmInstTxMember_set(uint32 unit, uint32 instance,
    rtk_oam_cfmInstMember_t *member);

/* Function Name:
 *      dal_mango_oam_cfmCcmInstRxMember_get
 * Description:
 *      Get rx instance member.
 * Input:
 *      unit        - unit id
 *      instance    - rx control entry instance
 * Output:
 *      member      - Rx intance member configuration
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
extern int32
dal_mango_oam_cfmCcmInstRxMember_get(uint32 unit, uint32 instance,
    rtk_oam_cfmInstMember_t *member);

/* Function Name:
 *      dal_mango_oam_cfmCcmInstRxMember_set
 * Description:
 *      Set rx instance member.
 * Input:
 *      unit        - unit id
 *      instance    - rx control entry instance
 *      member      - Rx intance member configuration
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
extern int32
dal_mango_oam_cfmCcmInstRxMember_set(uint32 unit, uint32 instance,
    rtk_oam_cfmInstMember_t *member);

#endif /* __DAL_MANGO_OAM_H__ */

