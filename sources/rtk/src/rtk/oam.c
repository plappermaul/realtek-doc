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
 *           (1) OAM (802.3ah) configuration
 *           (2) CFM (802.1ag) configuration
 */

/*
 * Include Files
 */
#include <common/rt_autoconf.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <dal/dal_mgmt.h>
#include <rtk/default.h>
#include <rtk/oam.h>

#include <dal/dal_linkFaultMon.h>

/*
 * Symbol Definition
 */

/*
 * Macro Declaration
 */

/*
 * Function Declaration
 */

/* Function Name:
 *      rtk_oam_init
 * Description:
 *      Initialize OAM module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      Must initialize OAM module before calling any OAM APIs.
 * Changes:
 *      None
 */
int32
rtk_oam_init(uint32 unit)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->oam_init(unit);
} /* end of rtk_oam_init */

/* Module Name    : OAM               */
/* Sub-module Name: OAM configuration */

/* Function Name:
 *      rtk_oam_portDyingGaspPayload_set
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
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      (1) The ports send out dying gasp frame is set by rtk_oam_autoDyingGaspEnable_set.
 *      (2) rtk_oam_autoDyingGaspEnable_set must be invoked before rtk_oam_portDyingGaspPayload_set.
 *      (3) The pPayload and len should not include CRC. It is handled by lower layer driver internally.
 * Changes:
 *      None
 */
int32
rtk_oam_portDyingGaspPayload_set(uint32 unit, rtk_port_t port, uint8 *pPayload,
    uint32 len)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->oam_portDyingGaspPayload_set(unit, port, pPayload, len);
}   /* end of rtk_oam_portDyingGaspPayload_set */

/* Function Name:
 *      rtk_oam_dyingGaspSend_set
 * Description:
 *      Start sending dying gasp frame to specified ports.
 * Input:
 *      unit   - unit id
 *      enable - trigger dying gasp with enabled ports.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      (1) This API will be used when CPU want to send dying gasp by itself.
 *      (2) The ports send out dying gasp frame is setting by
 *          rtk_oam_autoDyingGaspEnable_get.
 * Changes:
 *      None
 */
int32
rtk_oam_dyingGaspSend_set(uint32 unit, rtk_enable_t enable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->oam_dyingGaspSend_set(unit, enable);
} /* end of rtk_oam_dyingGaspSend_set */

/* Function Name:
 *      rtk_oam_autoDyingGaspEnable_get
 * Description:
 *      Get enable status of sending dying gasp automatically on specified port
 *      when voltage is lower than expected.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to enable status of sending dying gasp automatically
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      Time of voltage is lower than expected is set by
 *      rtk_oam_dyingGaspWaitTime_set.
 * Changes:
 *      None
 */
int32
rtk_oam_autoDyingGaspEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->oam_autoDyingGaspEnable_get(unit, port, pEnable);
} /* end of rtk_oam_autoDyingGaspEnable_get */

/* Function Name:
 *      rtk_oam_autoDyingGaspEnable_set
 * Description:
 *      Set enable status of sending dying gasp automatically on specified port
 *      when voltage is lower than expected.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - enable status of sending dying gasp automatically
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      Time of voltage is lower than expected is set by
 *      rtk_oam_dyingGaspWaitTime_set.
 * Changes:
 *      None
 */
int32
rtk_oam_autoDyingGaspEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->oam_autoDyingGaspEnable_set(unit, port, enable);
} /* end of rtk_oam_autoDyingGaspEnable_set */

/* Function Name:
 *      rtk_oam_dyingGaspWaitTime_get
 * Description:
 *      Get sustained time of low voltage detection before triggering dying gasp.
 * Input:
 *      unit - unit id
 * Output:
 *      time - sustained time
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      (1) Granularity of sustained time is 10 ns for 8390/9310. Range of sustained time is 0~0xFFFF.
 *      (2) Granularity of sustained time is 40 ns for 9300. Range of sustained time is 0~0xFFFF.
 *      (3) The status of sending dying gasp automatically is set by rtk_oam_autoDyingGaspEnable_set.
 * Changes:
 *      None
 */
int32
rtk_oam_dyingGaspWaitTime_get(uint32 unit, uint32 *pTime)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->oam_dyingGaspWaitTime_get(unit, pTime);
} /* end of rtk_oam_dyingGaspWaitTime_get */

/* Function Name:
 *      rtk_oam_dyingGaspWaitTime_set
 * Description:
 *      Set sustained time of low voltage detection before triggering dying gasp.
 * Input:
 *      unit - unit id
 *      time - sustained time
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      (1) Granularity of sustained time is 10 ns for 8390/9310. Range of sustained time is 0~0xFFFF.
 *      (2) Granularity of sustained time is 40 ns for 9300. Range of sustained time is 0~0xFFFF.
 *      (3) The status of sending dying gasp automatically is set by rtk_oam_autoDyingGaspEnable_set.
 * Changes:
 *      None
 */
int32
rtk_oam_dyingGaspWaitTime_set(uint32 unit, uint32 time)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->oam_dyingGaspWaitTime_set(unit, time);
} /* end of rtk_oam_dyingGaspWaitTime_set */

/* Module Name    : OAM               */
/* Sub-module Name: CFM configuration */

/* Function Name:
 *      rtk_oam_loopbackMacSwapEnable_get
 * Description:
 *      Get enable status of swap MAC address (source MAC & destination MAC)
 *      for OAM loopback feature.
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - pointer to enable status of MAC swap function
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      (1) Swap action takes effect for following condition:
 *          - OAMPDU action is set to "ACTION_TRAP2CPU". It can be configured by 'rtk_trap_oamPDUAction_set'.
 *          - parser action is set to "Loopback". It can be configured by 'rtk_trap_portOamLoopbackParAction_set'.
 *      (2) Swap action only applies to non-OAMPDU packet.
 * Changes:
 *      None
 */
int32
rtk_oam_loopbackMacSwapEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->oam_loopbackMacSwapEnable_get(unit, pEnable);
} /* end of rtk_oam_loopbackMacSwapEnable_get */

/* Function Name:
 *      rtk_oam_loopbackMacSwapEnable_set
 * Description:
 *      Set enable status of swap MAC address (source MAC & destination MAC)
 *      for OAM loopback feature.
 * Input:
 *      unit   - unit id
 *      enable - enable status of MAC swap
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      (1) Swap action takes effect for following condition:
 *          - OAMPDU action is set to "ACTION_TRAP2CPU". It can be configured by 'rtk_trap_oamPDUAction_set'.
 *          - parser action is set to "Loopback". It can be configured by 'rtk_trap_portOamLoopbackParAction_set'.
 *      (2) Swap action only applies to non-OAMPDU packet.
 * Changes:
 *      None
 */
int32
rtk_oam_loopbackMacSwapEnable_set(uint32 unit, rtk_enable_t enable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->oam_loopbackMacSwapEnable_set(unit, enable);
} /* end of rtk_oam_loopbackMacSwapEnable_set */

/* Function Name:
 *      rtk_oam_portLoopbackMuxAction_get
 * Description:
 *      Get action of multiplexer on specific port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pAction - pointer to multiplexer action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      (1) Multiplexer action will be token effect
 *          when loopback status is enable (ACTION_TRAP2CPU) is setting by
 *          rtk_trap_oamPDUAction_set.
 *      (2) Multiplexer action is as following:
 *          - ACTION_FORWARD
 *          - ACTION_DROP
 * Changes:
 *      None
 */
int32
rtk_oam_portLoopbackMuxAction_get(uint32 unit, rtk_port_t port, rtk_action_t *pAction)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->oam_portLoopbackMuxAction_get(unit, port, pAction);
} /* end of rtk_oam_portLoopbackMuxAction_get */

/* Function Name:
 *      rtk_oam_portLoopbackMuxAction_set
 * Description:
 *      Set action of multiplexer on specific port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      action - multiplexer action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      (1) Multiplexer action will be token effect
 *          when loopback status is enable (ACTION_TRAP2CPU) is setting by
 *          rtk_trap_oamPDUAction_set.
 *      (2) Multiplexer action is as following:
 *          - ACTION_FORWARD
 *          - ACTION_DROP
 * Changes:
 *      None
 */
int32
rtk_oam_portLoopbackMuxAction_set(uint32 unit, rtk_port_t port, rtk_action_t action)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->oam_portLoopbackMuxAction_set(unit, port, action);
} /* end of rtk_oam_portLoopbackMuxAction_set */

/* Function Name:
 *      rtk_oam_cfmCcmPcp_get
 * Description:
 *      Get priority code point value for generate CCM frame.
 * Input:
 *      unit - unit id
 * Output:
 *      pPcp - pointer buffer of priority code point value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390, 9310
 * Note:
 *      Support configurate fields for generate content of CCM frame is as following:
 *      - enable                is setting by rtk_oam_cfmCcmInstTxStatus_set
 *      - TPID                  is setting by rtk_oam_cfmCcmTpid_set
 *      - tag status            is setting by rtk_oam_cfmCcmInstTagStatus_set
 *      - priority code point   is setting by rtk_oam_cfmCcmPcp_set
 *      - CFI                   is setting by rtk_oam_cfmCcmCfi_set
 *      - VID                   is setting by rtk_oam_cfmCcmInstVid_set
 *      - MEPID                 is setting by rtk_oam_cfmCcmInstTxMepid_set
 *      - interval field        is setting by rtk_oam_cfmCcmInstTxIntervalField_set
 *      - MD level              is setting by rtk_oam_cfmCcmInstTxMdl_set
 *      - MAID                  is setting by rtk_oam_cfmCcmInstMaid_set
 *      - tx interval           is setting by rtk_oam_cfmCcmInstTxIntervalField_set
 *      - member                is setting by rtk_oam_cfmCcmInstTxMember_set
 * Changes:
 *      None
 */
int32
rtk_oam_cfmCcmPcp_get(uint32 unit, uint32 *pPcp)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->oam_cfmCcmPcp_get(unit, pPcp);
} /* end of rtk_oam_cfmCcmPcp_get */

/* Function Name:
 *      rtk_oam_cfmCcmPcp_set
 * Description:
 *      Set priority code point value for generate CCM frame.
 * Input:
 *      unit - unit id
 *      pcp  - priority code point value for generate CCM frame.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8390, 9310
 * Note:
 *      Support configurate fields for generate content of CCM frame is as following:
 *      - enable                is setting by rtk_oam_cfmCcmInstTxStatus_set
 *      - TPID                  is setting by rtk_oam_cfmCcmTpid_set
 *      - tag status            is setting by rtk_oam_cfmCcmInstTagStatus_set
 *      - priority code point   is setting by rtk_oam_cfmCcmPcp_set
 *      - CFI                   is setting by rtk_oam_cfmCcmCfi_set
 *      - VID                   is setting by rtk_oam_cfmCcmInstVid_set
 *      - MEPID                 is setting by rtk_oam_cfmCcmInstTxMepid_set
 *      - interval field        is setting by rtk_oam_cfmCcmInstTxIntervalField_set
 *      - MD level              is setting by rtk_oam_cfmCcmInstTxMdl_set
 *      - MAID                  is setting by rtk_oam_cfmCcmInstMaid_set
 *      - tx interval           is setting by rtk_oam_cfmCcmInstTxIntervalField_set
 *      - member                is setting by rtk_oam_cfmCcmInstTxMember_set
 * Changes:
 *      None
 */
int32
rtk_oam_cfmCcmPcp_set(uint32 unit, uint32 pcp)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->oam_cfmCcmPcp_set(unit, pcp);
} /* end of rtk_oam_cfmCcmPcp_set */

/* Function Name:
 *      rtk_oam_cfmCcmCfi_get
 * Description:
 *      Get canonical format identifier value for generate CCM frame.
 * Input:
 *      unit - unit id
 * Output:
 *      pCfi - pointer buffer of canonical format identifier value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390, 9310
 * Note:
 *      Support configurate fields for generate content of CCM frame is as following:
 *      - enable                is setting by rtk_oam_cfmCcmInstTxStatus_set
 *      - TPID                  is setting by rtk_oam_cfmCcmTpid_set
 *      - tag status            is setting by rtk_oam_cfmCcmInstTagStatus_set
 *      - priority code point   is setting by rtk_oam_cfmCcmPcp_set
 *      - CFI                   is setting by rtk_oam_cfmCcmCfi_set
 *      - VID                   is setting by rtk_oam_cfmCcmInstVid_set
 *      - MEPID                 is setting by rtk_oam_cfmCcmInstTxMepid_set
 *      - interval field        is setting by rtk_oam_cfmCcmInstTxIntervalField_set
 *      - MD level              is setting by rtk_oam_cfmCcmInstTxMdl_set
 *      - MAID                  is setting by rtk_oam_cfmCcmInstMaid_set
 *      - tx interval           is setting by rtk_oam_cfmCcmInstTxIntervalField_set
 *      - member                is setting by rtk_oam_cfmCcmInstTxMember_set
 * Changes:
 *      None
 */
int32
rtk_oam_cfmCcmCfi_get(uint32 unit, uint32 *pCfi)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->oam_cfmCcmCfi_get(unit, pCfi);
} /* end of rtk_oam_cfmCcmCfi_get */

/* Function Name:
 *      rtk_oam_cfmCcmCfi_set
 * Description:
 *      Set canonical format identifier value for generate CCM frame.
 * Input:
 *      unit - unit id
 *      cfi  - canonical format identifier value for generate CCM frame.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8390, 9310
 * Note:
 *      Support configurate fields for generate content of CCM frame is as following:
 *      - enable                is setting by rtk_oam_cfmCcmInstTxStatus_set
 *      - TPID                  is setting by rtk_oam_cfmCcmTpid_set
 *      - tag status            is setting by rtk_oam_cfmCcmInstTagStatus_set
 *      - priority code point   is setting by rtk_oam_cfmCcmPcp_set
 *      - CFI                   is setting by rtk_oam_cfmCcmCfi_set
 *      - VID                   is setting by rtk_oam_cfmCcmInstVid_set
 *      - MEPID                 is setting by rtk_oam_cfmCcmInstTxMepid_set
 *      - interval field        is setting by rtk_oam_cfmCcmInstTxIntervalField_set
 *      - MD level              is setting by rtk_oam_cfmCcmInstTxMdl_set
 *      - MAID                  is setting by rtk_oam_cfmCcmInstMaid_set
 *      - tx interval           is setting by rtk_oam_cfmCcmInstTxIntervalField_set
 *      - member                is setting by rtk_oam_cfmCcmInstTxMember_set
 * Changes:
 *      None
 */
int32
rtk_oam_cfmCcmCfi_set(uint32 unit, uint32 cfi)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->oam_cfmCcmCfi_set(unit, cfi);
} /* end of rtk_oam_cfmCcmCfi_set */

/* Function Name:
 *      rtk_oam_cfmCcmTpid_get
 * Description:
 *      Get TPID value for generate CCM frame.
 * Input:
 *      unit  - unit id
 * Output:
 *      pTpid - pointer buffer of TPID value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390, 9310
 * Note:
 *      Support configurate fields for generate content of CCM frame is as following:
 *      - enable                is setting by rtk_oam_cfmCcmInstTxStatus_set
 *      - TPID                  is setting by rtk_oam_cfmCcmTpid_set
 *      - tag status            is setting by rtk_oam_cfmCcmInstTagStatus_set
 *      - priority code point   is setting by rtk_oam_cfmCcmPcp_set
 *      - CFI                   is setting by rtk_oam_cfmCcmCfi_set
 *      - VID                   is setting by rtk_oam_cfmCcmInstVid_set
 *      - MEPID                 is setting by rtk_oam_cfmCcmInstTxMepid_set
 *      - interval field        is setting by rtk_oam_cfmCcmInstTxIntervalField_set
 *      - MD level              is setting by rtk_oam_cfmCcmInstTxMdl_set
 *      - MAID                  is setting by rtk_oam_cfmCcmInstMaid_set
 *      - tx interval           is setting by rtk_oam_cfmCcmInstTxIntervalField_set
 *      - member                is setting by rtk_oam_cfmCcmInstTxMember_set
 * Changes:
 *      None
 */
int32
rtk_oam_cfmCcmTpid_get(uint32 unit, uint32 *pTpid)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->oam_cfmCcmTpid_get(unit, pTpid);
} /* end of rtk_oam_cfmCcmTpid_get */

/* Function Name:
 *      rtk_oam_cfmCcmTpid_set
 * Description:
 *      Set TPID value for generate CCM frame.
 * Input:
 *      unit - unit id
 *      tpid - TPID value for generate CCM frame.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8390, 9310
 * Note:
 *      Support configurate fields for generate content of CCM frame is as following:
 *      - enable                is setting by rtk_oam_cfmCcmInstTxStatus_set
 *      - TPID                  is setting by rtk_oam_cfmCcmTpid_set
 *      - tag status            is setting by rtk_oam_cfmCcmInstTagStatus_set
 *      - priority code point   is setting by rtk_oam_cfmCcmPcp_set
 *      - CFI                   is setting by rtk_oam_cfmCcmCfi_set
 *      - VID                   is setting by rtk_oam_cfmCcmInstVid_set
 *      - MEPID                 is setting by rtk_oam_cfmCcmInstTxMepid_set
 *      - interval field        is setting by rtk_oam_cfmCcmInstTxIntervalField_set
 *      - MD level              is setting by rtk_oam_cfmCcmInstTxMdl_set
 *      - MAID                  is setting by rtk_oam_cfmCcmInstMaid_set
 *      - tx interval           is setting by rtk_oam_cfmCcmInstTxIntervalField_set
 *      - member                is setting by rtk_oam_cfmCcmInstTxMember_set
 * Changes:
 *      None
 */
int32
rtk_oam_cfmCcmTpid_set(uint32 unit, uint32 tpid)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->oam_cfmCcmTpid_set(unit, tpid);
} /* end of rtk_oam_cfmCcmTpid_set */

/* Function Name:
 *      rtk_oam_cfmCcmInstLifetime_get
 * Description:
 *      Get lifetime of specified instance.
 * Input:
 *      unit      - unit id
 *      instance  - CCM instance
 * Output:
 *      pLifetime - pointer buffer to lifetime of the instance
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390, 9310
 * Note:
 *      (1) The unit of lifetime is mili-second. An internal alive timer keeps count down every ms and is
 *          reset to the value of lifetime when receiving the corresponding CCM frame.
 *          A CCM interrupt is triggered if the internal timer counts down to 0.
 *      (2) Support information for receiving CCM frame is as following:
 *          - lifetime  is setting by rtk_oam_cfmCcmInstLifetime_set
 *          - vid       is setting by rtk_oam_cfmCcmRxInstVid_set
 *          - port      is setting by rtk_oam_cfmCcmRxInstPort_set
 *          - keepalive is get by rtk_oam_cfmCcmInstAliveTime_get
 * Changes:
 *      None
 */
int32
rtk_oam_cfmCcmInstLifetime_get(uint32 unit, uint32 instance, uint32 *pLifetime)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->oam_cfmCcmInstLifetime_get(unit, instance, pLifetime);
} /* end of rtk_oam_cfmCcmInstLifetime_get */

/* Function Name:
 *      rtk_oam_cfmCcmInstLifetime_set
 * Description:
 *      Set lifetime to specified instance.
 * Input:
 *      unit     - unit id
 *      instance - CCM instance
 *      lifetime - lifetime of the instance
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8390, 9310
 * Note:
 *      (1) The unit of lifetime is mili-second. An internal alive timer keeps count down every ms and is
 *          reset to the value of lifetime when receiving the corresponding CCM frame.
 *          A CCM interrupt is triggered if the internal timer counts down to 0.
 *      (2) Support information for receiving CCM frame is as following:
 *          - lifetime  is setting by rtk_oam_cfmCcmInstLifetime_set
 *          - vid       is setting by rtk_oam_cfmCcmRxInstVid_set
 *          - port      is setting by rtk_oam_cfmCcmRxInstPort_set
 *          - keepalive is get by rtk_oam_cfmCcmInstAliveTime_get
 * Changes:
 *      None
 */
int32
rtk_oam_cfmCcmInstLifetime_set(uint32 unit, uint32 instance, uint32 lifetime)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->oam_cfmCcmInstLifetime_set(unit, instance, lifetime);
} /* end of rtk_oam_cfmCcmInstLifetime_set */

/* Function Name:
 *      rtk_oam_cfmCcmMepid_get
 * Description:
 *      Get MEPID to be inserted to generated CCM frame.
 * Input:
 *      unit   - unit id
 * Output:
 *      pMepid - pointer buffer of MEPID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390
 * Note:
 *      Support configurate fields for generate content of CCM frame is as following:
 *      - enable                is setting by rtk_oam_cfmCcmInstTxStatus_set
 *      - TPID                  is setting by rtk_oam_cfmCcmTpid_set
 *      - tag status            is setting by rtk_oam_cfmCcmInstTagStatus_set
 *      - priority code point   is setting by rtk_oam_cfmCcmPcp_set
 *      - CFI                   is setting by rtk_oam_cfmCcmCfi_set
 *      - VID                   is setting by rtk_oam_cfmCcmInstVid_set
 *      - MEPID                 is setting by rtk_oam_cfmCcmInstTxMepid_set
 *      - interval field        is setting by rtk_oam_cfmCcmInstTxIntervalField_set
 *      - MD level              is setting by rtk_oam_cfmCcmInstTxMdl_set
 *      - MAID                  is setting by rtk_oam_cfmCcmInstMaid_set
 *      - tx interval           is setting by rtk_oam_cfmCcmInstTxIntervalField_set
 *      - member                is setting by rtk_oam_cfmCcmInstTxMember_set
 * Changes:
 *      None
 */
int32
rtk_oam_cfmCcmMepid_get(uint32 unit, uint32 *pMepid)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->oam_cfmCcmMepid_get(unit, pMepid);
} /* end of rtk_oam_cfmCcmMepid_get */

/* Function Name:
 *      rtk_oam_cfmCcmMepid_set
 * Description:
 *      Set MEPID to be inserted to generated CCM frame.
 * Input:
 *      unit  - unit id
 *      mepid - MEP id.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8390
 * Note:
 *      Support configurate fields for generate content of CCM frame is as following:
 *      - enable                is setting by rtk_oam_cfmCcmInstTxStatus_set
 *      - TPID                  is setting by rtk_oam_cfmCcmTpid_set
 *      - tag status            is setting by rtk_oam_cfmCcmInstTagStatus_set
 *      - priority code point   is setting by rtk_oam_cfmCcmPcp_set
 *      - CFI                   is setting by rtk_oam_cfmCcmCfi_set
 *      - VID                   is setting by rtk_oam_cfmCcmInstVid_set
 *      - MEPID                 is setting by rtk_oam_cfmCcmInstTxMepid_set
 *      - interval field        is setting by rtk_oam_cfmCcmInstTxIntervalField_set
 *      - MD level              is setting by rtk_oam_cfmCcmInstTxMdl_set
 *      - MAID                  is setting by rtk_oam_cfmCcmInstMaid_set
 *      - tx interval           is setting by rtk_oam_cfmCcmInstTxIntervalField_set
 *      - member                is setting by rtk_oam_cfmCcmInstTxMember_set
 * Changes:
 *      None
 */
int32
rtk_oam_cfmCcmMepid_set(uint32 unit, uint32 mepid)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->oam_cfmCcmMepid_set(unit, mepid);
} /* end of rtk_oam_cfmCcmMepid_set */

/* Function Name:
 *      rtk_oam_cfmCcmIntervalField_get
 * Description:
 *      Get value to be inserted to interval field in flag for generated CCM frame.
 * Input:
 *      unit      - unit id
 * Output:
 *      pInterval - pointer buffer of interval field in flag.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390
 * Note:
 *      Support configurate fields for generate content of CCM frame is as following:
 *      - enable                is setting by rtk_oam_cfmCcmInstTxStatus_set
 *      - TPID                  is setting by rtk_oam_cfmCcmTpid_set
 *      - tag status            is setting by rtk_oam_cfmCcmInstTagStatus_set
 *      - priority code point   is setting by rtk_oam_cfmCcmPcp_set
 *      - CFI                   is setting by rtk_oam_cfmCcmCfi_set
 *      - VID                   is setting by rtk_oam_cfmCcmInstVid_set
 *      - MEPID                 is setting by rtk_oam_cfmCcmInstTxMepid_set
 *      - interval field        is setting by rtk_oam_cfmCcmInstTxIntervalField_set
 *      - MD level              is setting by rtk_oam_cfmCcmInstTxMdl_set
 *      - MAID                  is setting by rtk_oam_cfmCcmInstMaid_set
 *      - tx interval           is setting by rtk_oam_cfmCcmInstTxIntervalField_set
 *      - member                is setting by rtk_oam_cfmCcmInstTxMember_set
 * Changes:
 *      None
 */
int32
rtk_oam_cfmCcmIntervalField_get(uint32 unit, uint32 *pInterval)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->oam_cfmCcmIntervalField_get(unit, pInterval);
} /* end of rtk_oam_cfmCcmIntervalField_get */

/* Function Name:
 *      rtk_oam_cfmCcmIntervalField_set
 * Description:
 *      Set value to be inserted to interval field in flag for generated CCM frame.
 * Input:
 *      unit     - unit id
 *      interval - interval field in flag.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8390
 * Note:
 *      Support configurate fields for generate content of CCM frame is as following:
 *      - enable                is setting by rtk_oam_cfmCcmInstTxStatus_set
 *      - TPID                  is setting by rtk_oam_cfmCcmTpid_set
 *      - tag status            is setting by rtk_oam_cfmCcmInstTagStatus_set
 *      - priority code point   is setting by rtk_oam_cfmCcmPcp_set
 *      - CFI                   is setting by rtk_oam_cfmCcmCfi_set
 *      - VID                   is setting by rtk_oam_cfmCcmInstVid_set
 *      - MEPID                 is setting by rtk_oam_cfmCcmInstTxMepid_set
 *      - interval field        is setting by rtk_oam_cfmCcmInstTxIntervalField_set
 *      - MD level              is setting by rtk_oam_cfmCcmInstTxMdl_set
 *      - MAID                  is setting by rtk_oam_cfmCcmInstMaid_set
 *      - tx interval           is setting by rtk_oam_cfmCcmInstTxIntervalField_set
 *      - member                is setting by rtk_oam_cfmCcmInstTxMember_set
 * Changes:
 *      None
 */
int32
rtk_oam_cfmCcmIntervalField_set(uint32 unit, uint32 interval)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->oam_cfmCcmIntervalField_set(unit, interval);
} /* end of rtk_oam_cfmCcmIntervalField_set */

/* Function Name:
 *      rtk_oam_cfmCcmMdl_get
 * Description:
 *      Get MD level to be inserted to generated CCM frame.
 * Input:
 *      unit - unit id
 * Output:
 *      pMdl  - pointer buffer of MD level
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390
 * Note:
 *      Support configurate fields for generate content of CCM frame is as following:
 *      - enable                is setting by rtk_oam_cfmCcmInstTxStatus_set
 *      - TPID                  is setting by rtk_oam_cfmCcmTpid_set
 *      - tag status            is setting by rtk_oam_cfmCcmInstTagStatus_set
 *      - priority code point   is setting by rtk_oam_cfmCcmPcp_set
 *      - CFI                   is setting by rtk_oam_cfmCcmCfi_set
 *      - VID                   is setting by rtk_oam_cfmCcmInstVid_set
 *      - MEPID                 is setting by rtk_oam_cfmCcmInstTxMepid_set
 *      - interval field        is setting by rtk_oam_cfmCcmInstTxIntervalField_set
 *      - MD level              is setting by rtk_oam_cfmCcmInstTxMdl_set
 *      - MAID                  is setting by rtk_oam_cfmCcmInstMaid_set
 *      - tx interval           is setting by rtk_oam_cfmCcmInstTxIntervalField_set
 *      - member                is setting by rtk_oam_cfmCcmInstTxMember_set
 * Changes:
 *      None
 */
int32
rtk_oam_cfmCcmMdl_get(uint32 unit, uint32 *pMdl)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->oam_cfmCcmMdl_get(unit, pMdl);
} /* end of rtk_oam_cfmCcmMdl_get */

/* Function Name:
 *      rtk_oam_cfmCcmMdl_set
 * Description:
 *      Set MD level to be inserted to generated CCM frame.
 * Input:
 *      unit - unit id
 *      mdl  - MD level insert to CCM frame
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8390
 * Note:
 *      Support configurate fields for generate content of CCM frame is as following:
 *      - enable                is setting by rtk_oam_cfmCcmInstTxStatus_set
 *      - TPID                  is setting by rtk_oam_cfmCcmTpid_set
 *      - tag status            is setting by rtk_oam_cfmCcmInstTagStatus_set
 *      - priority code point   is setting by rtk_oam_cfmCcmPcp_set
 *      - CFI                   is setting by rtk_oam_cfmCcmCfi_set
 *      - VID                   is setting by rtk_oam_cfmCcmInstVid_set
 *      - MEPID                 is setting by rtk_oam_cfmCcmInstTxMepid_set
 *      - interval field        is setting by rtk_oam_cfmCcmInstTxIntervalField_set
 *      - MD level              is setting by rtk_oam_cfmCcmInstTxMdl_set
 *      - MAID                  is setting by rtk_oam_cfmCcmInstMaid_set
 *      - tx interval           is setting by rtk_oam_cfmCcmInstTxIntervalField_set
 *      - member                is setting by rtk_oam_cfmCcmInstTxMember_set
 * Changes:
 *      None
 */
int32
rtk_oam_cfmCcmMdl_set(uint32 unit, uint32 mdl)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->oam_cfmCcmMdl_set(unit, mdl);
} /* end of rtk_oam_cfmCcmMdl_set */

/* Function Name:
 *      rtk_oam_cfmCcmInstTagStatus_get
 * Description:
 *      Get VLAN tag status of the generated CCM frame.
 * Input:
 *      unit     - unit id
 *      instance - tx control entry instance
 * Output:
 *      pEnable  - pointer buffer of VLAN tag status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390, 9310
 * Note:
 *      Indicate whether to insert VLAN tag to the generated CCM frame.
 *      Support configurate fields for generate content of CCM frame is as following:
 *      - enable                is setting by rtk_oam_cfmCcmInstTxStatus_set
 *      - TPID                  is setting by rtk_oam_cfmCcmTpid_set
 *      - tag status            is setting by rtk_oam_cfmCcmInstTagStatus_set
 *      - priority code point   is setting by rtk_oam_cfmCcmPcp_set
 *      - CFI                   is setting by rtk_oam_cfmCcmCfi_set
 *      - VID                   is setting by rtk_oam_cfmCcmInstVid_set
 *      - MEPID                 is setting by rtk_oam_cfmCcmInstTxMepid_set
 *      - interval field        is setting by rtk_oam_cfmCcmInstTxIntervalField_set
 *      - MD level              is setting by rtk_oam_cfmCcmInstTxMdl_set
 *      - MAID                  is setting by rtk_oam_cfmCcmInstMaid_set
 *      - tx interval           is setting by rtk_oam_cfmCcmInstTxIntervalField_set
 *      - member                is setting by rtk_oam_cfmCcmInstTxMember_set
 * Changes:
 *      None
 */
int32
rtk_oam_cfmCcmInstTagStatus_get(uint32 unit, uint32 instance, rtk_enable_t *pEnable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->oam_cfmCcmInstTagStatus_get(unit, instance, pEnable);
} /* end of rtk_oam_cfmCcmInstTagStatus_get */

/* Function Name:
 *      rtk_oam_cfmCcmInstTagStatus_set
 * Description:
 *      Set VLAN tag status of the generated CCM frame.
 * Input:
 *      unit     - unit id
 *      instance - tx control entry instance
 *      enable   - tag status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8390, 9310
 * Note:
 *      Indicate whether to insert VLAN tag to the generated CCM frame.
 *      Support configurate fields for generate content of CCM frame is as following:
 *      - enable                is setting by rtk_oam_cfmCcmInstTxStatus_set
 *      - TPID                  is setting by rtk_oam_cfmCcmTpid_set
 *      - tag status            is setting by rtk_oam_cfmCcmInstTagStatus_set
 *      - priority code point   is setting by rtk_oam_cfmCcmPcp_set
 *      - CFI                   is setting by rtk_oam_cfmCcmCfi_set
 *      - VID                   is setting by rtk_oam_cfmCcmInstVid_set
 *      - MEPID                 is setting by rtk_oam_cfmCcmInstTxMepid_set
 *      - interval field        is setting by rtk_oam_cfmCcmInstTxIntervalField_set
 *      - MD level              is setting by rtk_oam_cfmCcmInstTxMdl_set
 *      - MAID                  is setting by rtk_oam_cfmCcmInstMaid_set
 *      - tx interval           is setting by rtk_oam_cfmCcmInstTxIntervalField_set
 *      - member                is setting by rtk_oam_cfmCcmInstTxMember_set
 * Changes:
 *      None
 */
int32
rtk_oam_cfmCcmInstTagStatus_set(uint32 unit, uint32 instance, rtk_enable_t enable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->oam_cfmCcmInstTagStatus_set(unit, instance, enable);
} /* end of rtk_oam_cfmCcmInstTagStatus_set */

/* Function Name:
 *      rtk_oam_cfmCcmInstVid_get
 * Description:
 *      Get vlan id for instance member to be inserted to generated CCM frame.
 * Input:
 *      unit     - unit id
 *      instance - tx control entry instance
 * Output:
 *      pVid     - pointer buffer of vlan id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390, 9310
 * Note:
 *      Support configurate fields for generate content of CCM frame is as following:
 *      - enable                is setting by rtk_oam_cfmCcmInstTxStatus_set
 *      - TPID                  is setting by rtk_oam_cfmCcmTpid_set
 *      - tag status            is setting by rtk_oam_cfmCcmInstTagStatus_set
 *      - priority code point   is setting by rtk_oam_cfmCcmPcp_set
 *      - CFI                   is setting by rtk_oam_cfmCcmCfi_set
 *      - VID                   is setting by rtk_oam_cfmCcmInstVid_set
 *      - MEPID                 is setting by rtk_oam_cfmCcmInstTxMepid_set
 *      - interval field        is setting by rtk_oam_cfmCcmInstTxIntervalField_set
 *      - MD level              is setting by rtk_oam_cfmCcmInstTxMdl_set
 *      - MAID                  is setting by rtk_oam_cfmCcmInstMaid_set
 *      - tx interval           is setting by rtk_oam_cfmCcmInstTxIntervalField_set
 *      - member                is setting by rtk_oam_cfmCcmInstTxMember_set
 * Changes:
 *      None
 */
int32
rtk_oam_cfmCcmInstVid_get(uint32 unit, uint32 instance, rtk_vlan_t *pVid)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->oam_cfmCcmInstVid_get(unit, instance, pVid);
} /* end of rtk_oam_cfmInstVid_get */

/* Function Name:
 *      rtk_oam_cfmCcmInstVid_set
 * Description:
 *      Set vlan id for instance member to be inserted to generated CCM frame.
 * Input:
 *      unit     - unit id
 *      instance - tx control entry instance
 *      vid      - vlan id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 *      RT_ERR_VLAN_VID - invalid vid
 * Applicable:
 *      8390, 9310
 * Note:
 *      Support configurate fields for generate content of CCM frame is as following:
 *      - enable                is setting by rtk_oam_cfmCcmInstTxStatus_set
 *      - TPID                  is setting by rtk_oam_cfmCcmTpid_set
 *      - tag status            is setting by rtk_oam_cfmCcmInstTagStatus_set
 *      - priority code point   is setting by rtk_oam_cfmCcmPcp_set
 *      - CFI                   is setting by rtk_oam_cfmCcmCfi_set
 *      - VID                   is setting by rtk_oam_cfmCcmInstVid_set
 *      - MEPID                 is setting by rtk_oam_cfmCcmInstTxMepid_set
 *      - interval field        is setting by rtk_oam_cfmCcmInstTxIntervalField_set
 *      - MD level              is setting by rtk_oam_cfmCcmInstTxMdl_set
 *      - MAID                  is setting by rtk_oam_cfmCcmInstMaid_set
 *      - tx interval           is setting by rtk_oam_cfmCcmInstTxIntervalField_set
 *      - member                is setting by rtk_oam_cfmCcmInstTxMember_set
 * Changes:
 *      None
 */
int32
rtk_oam_cfmCcmInstVid_set(uint32 unit, uint32 instance, rtk_vlan_t vid)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->oam_cfmCcmInstVid_set(unit, instance, vid);
} /* end of rtk_oam_cfmCcmInstVid_set */

/* Function Name:
 *      rtk_oam_cfmCcmInstMaid_get
 * Description:
 *      Get MAID for instance member to be inserted to generated CCM frame.
 * Input:
 *      unit     - unit id
 *      instance - tx control entry instance
 * Output:
 *      pMaid    - pointer buffer of MAID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390, 9310
 * Note:
 *      Support configurate fields for generate content of CCM frame is as following:
 *      - enable                is setting by rtk_oam_cfmCcmInstTxStatus_set
 *      - TPID                  is setting by rtk_oam_cfmCcmTpid_set
 *      - tag status            is setting by rtk_oam_cfmCcmInstTagStatus_set
 *      - priority code point   is setting by rtk_oam_cfmCcmPcp_set
 *      - CFI                   is setting by rtk_oam_cfmCcmCfi_set
 *      - VID                   is setting by rtk_oam_cfmCcmInstVid_set
 *      - MEPID                 is setting by rtk_oam_cfmCcmInstTxMepid_set
 *      - interval field        is setting by rtk_oam_cfmCcmInstTxIntervalField_set
 *      - MD level              is setting by rtk_oam_cfmCcmInstTxMdl_set
 *      - MAID                  is setting by rtk_oam_cfmCcmInstMaid_set
 *      - tx interval           is setting by rtk_oam_cfmCcmInstTxIntervalField_set
 *      - member                is setting by rtk_oam_cfmCcmInstTxMember_set
 * Changes:
 *      None
 */
int32
rtk_oam_cfmCcmInstMaid_get(uint32 unit, uint32 instance, uint32 *pMaid)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->oam_cfmCcmInstMaid_get(unit, instance, pMaid);
} /* end of rtk_oam_cfmInstMaid_get */

/* Function Name:
 *      rtk_oam_cfmCcmInstMaid_set
 * Description:
 *      Set MAID for instance member to be inserted to generated CCM frame.
 * Input:
 *      unit     - unit id
 *      instance - tx control entry instance
 *      maid     - MA id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8390, 9310
 * Note:
 *      Support configurate fields for generate content of CCM frame is as following:
 *      - enable                is setting by rtk_oam_cfmCcmInstTxStatus_set
 *      - TPID                  is setting by rtk_oam_cfmCcmTpid_set
 *      - tag status            is setting by rtk_oam_cfmCcmInstTagStatus_set
 *      - priority code point   is setting by rtk_oam_cfmCcmPcp_set
 *      - CFI                   is setting by rtk_oam_cfmCcmCfi_set
 *      - VID                   is setting by rtk_oam_cfmCcmInstVid_set
 *      - MEPID                 is setting by rtk_oam_cfmCcmInstTxMepid_set
 *      - interval field        is setting by rtk_oam_cfmCcmInstTxIntervalField_set
 *      - MD level              is setting by rtk_oam_cfmCcmInstTxMdl_set
 *      - MAID                  is setting by rtk_oam_cfmCcmInstMaid_set
 *      - tx interval           is setting by rtk_oam_cfmCcmInstTxIntervalField_set
 *      - member                is setting by rtk_oam_cfmCcmInstTxMember_set
 * Changes:
 *      None
 */
int32
rtk_oam_cfmCcmInstMaid_set(uint32 unit, uint32 instance, uint32 maid)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->oam_cfmCcmInstMaid_set(unit, instance, maid);
} /* end of rtk_oam_cfmCcmInstMaid_set */

/* Function Name:
 *      rtk_oam_cfmCcmInstTxStatus_get
 * Description:
 *      Get enable status of transmitting CCM frame on specified instance.
 * Input:
 *      unit     - unit id
 *      instance - tx control ent instance
 * Output:
 *      pEnable  - pointer buffer of enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390, 9310
 * Note:
 *      Support configurate fields for generate content of CCM frame is as following:
 *      - enable                is setting by rtk_oam_cfmCcmInstTxStatus_set
 *      - TPID                  is setting by rtk_oam_cfmCcmTpid_set
 *      - tag status            is setting by rtk_oam_cfmCcmInstTagStatus_set
 *      - priority code point   is setting by rtk_oam_cfmCcmPcp_set
 *      - CFI                   is setting by rtk_oam_cfmCcmCfi_set
 *      - VID                   is setting by rtk_oam_cfmCcmInstVid_set
 *      - MEPID                 is setting by rtk_oam_cfmCcmInstTxMepid_set
 *      - interval field        is setting by rtk_oam_cfmCcmInstTxIntervalField_set
 *      - MD level              is setting by rtk_oam_cfmCcmInstTxMdl_set
 *      - MAID                  is setting by rtk_oam_cfmCcmInstMaid_set
 *      - tx interval           is setting by rtk_oam_cfmCcmInstTxIntervalField_set
 *      - member                is setting by rtk_oam_cfmCcmInstTxMember_set
 * Changes:
 *      None
 */
int32
rtk_oam_cfmCcmInstTxStatus_get(uint32 unit, uint32 instance, rtk_enable_t *pEnable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->oam_cfmCcmInstTxStatus_get(unit, instance, pEnable);
} /* end of rtk_oam_cfmCcmInstTxStatus_get */

/* Function Name:
 *      rtk_oam_cfmCcmInstTxStatus_set
 * Description:
 *      Set enable status of transmitting CCM frame on specified instance.
 * Input:
 *      unit     - unit id
 *      instance - tx control entry instance
 *      enable   - tx status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8390, 9310
 * Note:
 *      Support configurate fields for generate content of CCM frame is as following:
 *      - enable                is setting by rtk_oam_cfmCcmInstTxStatus_set
 *      - TPID                  is setting by rtk_oam_cfmCcmTpid_set
 *      - tag status            is setting by rtk_oam_cfmCcmInstTagStatus_set
 *      - priority code point   is setting by rtk_oam_cfmCcmPcp_set
 *      - CFI                   is setting by rtk_oam_cfmCcmCfi_set
 *      - VID                   is setting by rtk_oam_cfmCcmInstVid_set
 *      - MEPID                 is setting by rtk_oam_cfmCcmInstTxMepid_set
 *      - interval field        is setting by rtk_oam_cfmCcmInstTxIntervalField_set
 *      - MD level              is setting by rtk_oam_cfmCcmInstTxMdl_set
 *      - MAID                  is setting by rtk_oam_cfmCcmInstMaid_set
 *      - tx interval           is setting by rtk_oam_cfmCcmInstTxIntervalField_set
 *      - member                is setting by rtk_oam_cfmCcmInstTxMember_set
 * Changes:
 *      None
 */
int32
rtk_oam_cfmCcmInstTxStatus_set(uint32 unit, uint32 instance, rtk_enable_t enable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->oam_cfmCcmInstTxStatus_set(unit, instance, enable);
} /* end of rtk_oam_cfmInstTxStatus_set */

/* Function Name:
 *      rtk_oam_cfmCcmInstInterval_get
 * Description:
 *      Get CCM frame transmit interval.
 * Input:
 *      unit     - unit id
 *      instance - tx control entry instance
 * Output:
 *      pInterval - pointer buffer of CCM frame transmit interval
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390, 9310
 * Note:
 *      Support configurate fields for generate content of CCM frame is as following:
 *      - enable                is setting by rtk_oam_cfmCcmInstTxStatus_set
 *      - TPID                  is setting by rtk_oam_cfmCcmTpid_set
 *      - tag status            is setting by rtk_oam_cfmCcmInstTagStatus_set
 *      - priority code point   is setting by rtk_oam_cfmCcmPcp_set
 *      - CFI                   is setting by rtk_oam_cfmCcmCfi_set
 *      - VID                   is setting by rtk_oam_cfmCcmInstVid_set
 *      - MEPID                 is setting by rtk_oam_cfmCcmInstTxMepid_set
 *      - interval field        is setting by rtk_oam_cfmCcmInstTxIntervalField_set
 *      - MD level              is setting by rtk_oam_cfmCcmInstTxMdl_set
 *      - MAID                  is setting by rtk_oam_cfmCcmInstMaid_set
 *      - tx interval           is setting by rtk_oam_cfmCcmInstTxIntervalField_set
 *      - member                is setting by rtk_oam_cfmCcmInstTxMember_set
 * Changes:
 *      None
 */
int32
rtk_oam_cfmCcmInstInterval_get(uint32 unit, uint32 instance, uint32 *pInterval)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->oam_cfmCcmInstInterval_get(unit, instance, pInterval);
} /* end of rtk_oam_cfmCcmInstInterval_get */

/* Function Name:
 *      rtk_oam_cfmCcmInstInterval_set
 * Description:
 *      Set CCM frame transmit interval.
 * Input:
 *      unit     - unit id
 *      instance - tx control entry instance
 *      interval - transmit interval
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8390, 9310
 * Note:
 *      Support configurate fields for generate content of CCM frame is as following:
 *      - enable                is setting by rtk_oam_cfmCcmInstTxStatus_set
 *      - TPID                  is setting by rtk_oam_cfmCcmTpid_set
 *      - tag status            is setting by rtk_oam_cfmCcmInstTagStatus_set
 *      - priority code point   is setting by rtk_oam_cfmCcmPcp_set
 *      - CFI                   is setting by rtk_oam_cfmCcmCfi_set
 *      - VID                   is setting by rtk_oam_cfmCcmInstVid_set
 *      - MEPID                 is setting by rtk_oam_cfmCcmInstTxMepid_set
 *      - interval field        is setting by rtk_oam_cfmCcmInstTxIntervalField_set
 *      - MD level              is setting by rtk_oam_cfmCcmInstTxMdl_set
 *      - MAID                  is setting by rtk_oam_cfmCcmInstMaid_set
 *      - tx interval           is setting by rtk_oam_cfmCcmInstTxIntervalField_set
 *      - member                is setting by rtk_oam_cfmCcmInstTxMember_set
 * Changes:
 *      None
 */
int32
rtk_oam_cfmCcmInstInterval_set(uint32 unit, uint32 instance, uint32 interval)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->oam_cfmCcmInstInterval_set(unit, instance, interval);
} /* end of rtk_oam_cfmCcmInstInterval_set */

/* Function Name:
 *      rtk_oam_cfmCcmTxInstPort_get
 * Description:
 *      Get tx instance member.
 * Input:
 *      unit     - unit id
 *      instance - tx control entry instance
 *      index    - instance member index
 * Output:
 *      pPort    - pointer buffer of tx instance member
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390
 * Note:
 *      Support configurate fields for generate content of CCM frame is as following:
 *      - enable                is setting by rtk_oam_cfmCcmInstTxStatus_set
 *      - TPID                  is setting by rtk_oam_cfmCcmTpid_set
 *      - tag status            is setting by rtk_oam_cfmCcmInstTagStatus_set
 *      - priority code point   is setting by rtk_oam_cfmCcmPcp_set
 *      - CFI                   is setting by rtk_oam_cfmCcmCfi_set
 *      - VID                   is setting by rtk_oam_cfmCcmInstVid_set
 *      - MEPID                 is setting by rtk_oam_cfmCcmInstTxMepid_set
 *      - interval field        is setting by rtk_oam_cfmCcmInstTxIntervalField_set
 *      - MD level              is setting by rtk_oam_cfmCcmInstTxMdl_set
 *      - MAID                  is setting by rtk_oam_cfmCcmInstMaid_set
 *      - tx interval           is setting by rtk_oam_cfmCcmInstTxIntervalField_set
 *      - member                is setting by rtk_oam_cfmCcmInstTxMember_set
 * Changes:
 *      None
 */
int32
rtk_oam_cfmCcmTxInstPort_get(uint32 unit, uint32 instance, uint32 index, rtk_port_t *pPort)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->oam_cfmCcmTxInstPort_get(unit, instance, index, pPort);
} /* end of rtk_oam_cfmCcmTxInstPort_get */

/* Function Name:
 *      rtk_oam_cfmCcmTxInstPort_set
 * Description:
 *      Set tx instance member.
 * Input:
 *      unit     - unit id
 *      instance - tx control entry instance
 *      index    - instance member index
 *      port     - instance member port
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 *      RT_ERR_PORT_ID  - invalid port id
 * Applicable:
 *      8390
 * Note:
 *      Support configurate fields for generate content of CCM frame is as following:
 *      - enable                is setting by rtk_oam_cfmCcmInstTxStatus_set
 *      - TPID                  is setting by rtk_oam_cfmCcmTpid_set
 *      - tag status            is setting by rtk_oam_cfmCcmInstTagStatus_set
 *      - priority code point   is setting by rtk_oam_cfmCcmPcp_set
 *      - CFI                   is setting by rtk_oam_cfmCcmCfi_set
 *      - VID                   is setting by rtk_oam_cfmCcmInstVid_set
 *      - MEPID                 is setting by rtk_oam_cfmCcmInstTxMepid_set
 *      - interval field        is setting by rtk_oam_cfmCcmInstTxIntervalField_set
 *      - MD level              is setting by rtk_oam_cfmCcmInstTxMdl_set
 *      - MAID                  is setting by rtk_oam_cfmCcmInstMaid_set
 *      - tx interval           is setting by rtk_oam_cfmCcmInstTxIntervalField_set
 *      - member                is setting by rtk_oam_cfmCcmInstTxMember_set
 * Changes:
 *      None
 */
int32
rtk_oam_cfmCcmTxInstPort_set(uint32 unit, uint32 instance, uint32 index, rtk_port_t port)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->oam_cfmCcmTxInstPort_set(unit, instance, index, port);
} /* end of rtk_oam_cfmCcmTxInstPort_set */

/* Function Name:
 *      rtk_oam_cfmCcmRxInstVid_get
 * Description:
 *      Get binding VLAN of rx instance.
 * Input:
 *      unit     - unit id
 *      instance - rx control entry instance
 * Output:
 *      pVid     - pointer buffer of binding VLAN
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390, 9310
 * Note:
 *      Set vid=0 to disable binding VLAN to instance.
 *      Support information for receiving CCM frame is as following:
 *      - lifetime  is setting by rtk_oam_cfmCcmInstLifetime_set
 *      - vid       is setting by rtk_oam_cfmCcmRxInstVid_set
 *      - port      is setting by rtk_oam_cfmCcmRxInstPort_set
 *      - keepalive is get by rtk_oam_cfmCcmInstAliveTime_get
 * Changes:
 *      None
 */
int32
rtk_oam_cfmCcmRxInstVid_get(uint32 unit, uint32 instance, rtk_vlan_t *pVid)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->oam_cfmCcmRxInstVid_get(unit, instance, pVid);
} /* end of rtk_oam_cfmCcmRxInstVid_get */

/* Function Name:
 *      rtk_oam_cfmCcmRxInstVid_set
 * Description:
 *      Set binding VLAN of rx instance.
 * Input:
 *      unit     - unit id
 *      instance - rx control entry instance
 *      vid      - accept vlan id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 *      RT_ERR_VLAN_VID - invalid vid
 * Applicable:
 *      8390, 9310
 * Note:
 *      Set vid=0 to disable binding VLAN to instance.
 *      Support information for receiving CCM frame is as following:
 *      - lifetime  is setting by rtk_oam_cfmCcmInstLifetime_set
 *      - vid       is setting by rtk_oam_cfmCcmRxInstVid_set
 *      - port      is setting by rtk_oam_cfmCcmRxInstPort_set
 *      - keepalive is get by rtk_oam_cfmCcmInstAliveTime_get
 * Changes:
 *      None
 */
int32
rtk_oam_cfmCcmRxInstVid_set(uint32 unit, uint32 instance, rtk_vlan_t vid)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->oam_cfmCcmRxInstVid_set(unit, instance, vid);
} /* end of rtk_oam_cfmCcmRxInstVid_set */

/* Function Name:
 *      rtk_oam_cfmCcmRxInstPort_get
 * Description:
 *      Get rx instance member.
 * Input:
 *      unit     - unit id
 *      instance - rx control entry instance
 *      index    - instance member index
 * Output:
 *      pPort    - pointer buffer of rx instance member
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390
 * Note:
 *      Support information for receiving CCM frame is as following:
 *      - lifetime  is setting by rtk_oam_cfmCcmInstLifetime_set
 *      - vid       is setting by rtk_oam_cfmCcmRxInstVid_set
 *      - port      is setting by rtk_oam_cfmCcmRxInstPort_set
 *      - keepalive is get by rtk_oam_cfmCcmInstAliveTime_get
 * Changes:
 *      None
 */
int32
rtk_oam_cfmCcmRxInstPort_get(uint32 unit, uint32 instance, uint32 index, rtk_port_t *pPort)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->oam_cfmCcmRxInstPort_get(unit, instance, index, pPort);
} /* end of rtk_oam_cfmCcmRxInstPort_get */

/* Function Name:
 *      rtk_oam_cfmCcmRxInstPort_set
 * Description:
 *      Set rx instance member.
 * Input:
 *      unit     - unit id
 *      instance - rx control entry instance
 *      index    - instance member index
 *      port     - instance member port
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 *      RT_ERR_PORT_ID  - invalid port id
 * Applicable:
 *      8390
 * Note:
 *      Support information for receiving CCM frame is as following:
 *      - lifetime  is setting by rtk_oam_cfmCcmInstLifetime_set
 *      - vid       is setting by rtk_oam_cfmCcmRxInstVid_set
 *      - port      is setting by rtk_oam_cfmCcmRxInstPort_set
 *      - keepalive is get by rtk_oam_cfmCcmInstAliveTime_get
 * Changes:
 *      None
 */
int32
rtk_oam_cfmCcmRxInstPort_set(uint32 unit, uint32 instance, uint32 index, rtk_port_t port)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->oam_cfmCcmRxInstPort_set(unit, instance, index, port);
} /* end of rtk_oam_cfmCcmRxInstPort_set */

/* Function Name:
 *      rtk_oam_cfmCcmInstAliveTime_get
 * Description:
 *      Get rx instance member alive time.
 * Input:
 *      unit     - unit id
 *      instance - rx control entry instance
 *      index    - instance member index
 * Output:
 *      pTime    - pointer buffer of rx instance member alive time
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390, 9310
 * Note:
 *      (1) The unit of time is mili-second.
 *      (2) Each instance member port has an internal alive timer. It keeps count down every ms
 *          and is reset to the value of lifetime when receiving the corresponding CCM frame.
 *          A CCM interrupt is triggered if the internal timer counts down to 0.
 *      (3) Support information for receiving CCM frame is as following:
 *          - lifetime  is setting by rtk_oam_cfmCcmInstLifetime_set
 *          - vid       is setting by rtk_oam_cfmCcmRxInstVid_set
 *          - port      is setting by rtk_oam_cfmCcmRxInstPort_set
 *          - keepalive is get by rtk_oam_cfmCcmInstAliveTime_get
 * Changes:
 *      None
 */
int32
rtk_oam_cfmCcmInstAliveTime_get(uint32 unit, uint32 instance, uint32 index, uint32 *pTime)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->oam_cfmCcmKeepalive_get(unit, instance, index, pTime);
} /* end of rtk_oam_cfmCcmInstAliveTime_get */

/* Function Name:
 *      rtk_oam_cfmPortEthDmEnable_get
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
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_oam_cfmPortEthDmEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->oam_cfmPortEthDmEnable_get(unit, port, pEnable);
} /* end of rtk_oam_cfmPortEthDmEnable_get */

/* Function Name:
 *      rtk_oam_cfmPortEthDmEnable_set
 * Description:
 *      Set enable status of CFM ETH-DM feature on the specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8390, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_oam_cfmPortEthDmEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->oam_cfmPortEthDmEnable_set(unit, port, enable);
} /* end of rtk_oam_cfmPortEthDmEnable_set */


/* Function Name:
 *      rtk_oam_cfmEthDmRxTimestamp_get
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
 * Applicable:
 *      8390, 9310
 * Note:
 *      1. Each timestamp can only be read once. (it will be reset after it has been read.)
 *      2. Timestamps have to be read sequentially (the order of index) or it will be skipped.
 * Changes:
 *      None
 */
int32
rtk_oam_cfmEthDmRxTimestamp_get(
    uint32 unit,
    uint32 index,
    rtk_time_timeStamp_t *pTimeStamp)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->oam_cfmEthDmRxTimestamp_get(unit, index, pTimeStamp);
} /* end of rtk_oam_cfmEthDmRxTimestamp_get */

/* Function Name:
 *      rtk_oam_cfmEthDmTxDelay_get
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
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_oam_cfmEthDmTxDelay_get(
    uint32 unit,
    rtk_oam_cfmEthDmTxDelay_t *pTxDelay)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->oam_cfmEthDmTxDelay_get(unit, pTxDelay);
} /* end of rtk_oam_cfmEthDmTxDelay_get */

/* Function Name:
 *      rtk_oam_cfmEthDmTxDelay_set
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
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_oam_cfmEthDmTxDelay_set(
    uint32                      unit,
    rtk_oam_cfmEthDmTxDelay_t txDelay)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->oam_cfmEthDmTxDelay_set(unit, txDelay);
} /* end of rtk_oam_cfmEthDmTxDelay_set */

/* Function Name:
 *      rtk_oam_cfmEthDmRefTime_get
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
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_oam_cfmEthDmRefTime_get(uint32 unit, rtk_time_timeStamp_t *pTimeStamp)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->oam_cfmEthDmRefTime_get(unit, pTimeStamp);
} /* end of rtk_oam_cfmEthDmRefTime_get */

/* Function Name:
 *      rtk_oam_cfmEthDmRefTime_set
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
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_oam_cfmEthDmRefTime_set(uint32 unit, rtk_time_timeStamp_t timeStamp)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->oam_cfmEthDmRefTime_set(unit, timeStamp);
} /* end of rtk_oam_cfmEthDmRefTime_set */


/* Function Name:
 *      rtk_oam_cfmEthDmRefTimeEnable_get
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
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_oam_cfmEthDmRefTimeEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->oam_cfmEthDmRefTimeEnable_get(unit, pEnable);
} /* end of rtk_oam_cfmEthDmRefTimeEnable_get */

/* Function Name:
 *      rtk_oam_cfmEthDmRefTimeEnable_set
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
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_oam_cfmEthDmRefTimeEnable_set(uint32 unit, rtk_enable_t enable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->oam_cfmEthDmRefTimeEnable_set(unit, enable);
} /* end of rtk_oam_cfmEthDmRefTimeEnable_set */

/* Function Name:
 *      rtk_oam_cfmEthDmRefTimeFreq_get
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
 * Applicable:
 *      9310
 * Note:
 *      The frequency configuration decides the reference time tick frequency.
 *      The default value is 0x8000000.
 *      If it is configured to 0x4000000, the tick frequency would be half of default.
 *      If it is configured to 0xC000000, the tick frequency would be one and half times of default.
 *
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_oam_cfmEthDmRefTimeFreq_get(uint32 unit, uint32 *pFreq)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->oam_cfmEthDmRefTimeFreq_get(unit, pFreq);
} /* end of rtk_oam_cfmEthDmRefTimeFreq_get */

/* Function Name:
 *      rtk_oam_cfmEthDmRefTimeFreq_set
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
 * Applicable:
 *      9310
 * Note:
 *      The frequency configuration decides the reference time tick frequency.
 *      The default value is 0x8000000.
 *      If it is configured to 0x4000000, the tick frequency would be half of default.
 *      If it is configured to 0xC000000, the tick frequency would be one and half times of default.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_oam_cfmEthDmRefTimeFreq_set(uint32 unit, uint32 freq)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->oam_cfmEthDmRefTimeFreq_set(unit, freq);
} /* end of rtk_oam_cfmEthDmRefTimeFreq_set */


/* Function Name:
 *      rtk_oam_dyingGaspPktCnt_get
 * Description:
 *      Get dying gasp send packet count.
 * Input:
 *      unit - unit id
 * Output:
 *      pCnt - packet count configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      (1) Multiple dying gasp packets can be sent continously for robustness.
 *      (2) The valid packet count is 0 ~ 7.
 * Changes:
 *      None
 */
int32
rtk_oam_dyingGaspPktCnt_get(uint32 unit, uint32 *pCnt)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->oam_dyingGaspPktCnt_get(unit, pCnt);
} /* end of rtk_oam_dyingGaspPktCnt_get */

/* Function Name:
 *      rtk_oam_dyingGaspPktCnt_set
 * Description:
 *      Set dying gasp send packet count.
 * Input:
 *      unit - unit id
 *      cnt  - trigger dying gasp with enabled ports.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      (1) Multiple dying gasp packets can be sent continously for robustness.
 *      (2) The valid packet count is 0 ~ 7.
 * Changes:
 *      None
 */
int32
rtk_oam_dyingGaspPktCnt_set(uint32 unit, uint32 cnt)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->oam_dyingGaspPktCnt_set(unit, cnt);
} /* end of rtk_oam_dyingGaspPktCnt_set */

/* Function Name:
 *      rtk_oam_linkFaultMonEnable_set
 * Description:
 *      Set enable status of link fault monitor
 * Input:
 *      unit    - unit ID
 *      enable  - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - initialize success
 *      RT_ERR_FAILED   - initialize fail
 *      RT_ERR_NOT_INIT - The module is not initial
 * Applicable:
 *      8390
 * Note:
 *      When enable link fault monitor, all CCM interrupt will be callback to upper layer.
 * Changes:
 *      None
 */
int32 rtk_oam_linkFaultMonEnable_set(uint32 unit, rtk_enable_t enable)
{
#if defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE)
    #if defined(CONFIG_SDK_RTL8390) && !defined(__MODEL_USER__)
    return dal_linkFaultMonEnable_set(enable);
    #endif
#endif
    return RT_ERR_FAILED;
}   /* end of rtk_oam_linkFaultMonEnable_set */

/* Function Name:
 *      rtk_oam_linkFaultMon_register
 * Description:
 *      Register callback function for link fault detect notification
 * Input:
 *      unit	 - unit ID
 *      callback - callback function for link fault detect
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390
 * Note:
 *      None
 * Changes:
 *      None
 */
int32 rtk_oam_linkFaultMon_register(uint32 unit, rtk_oam_linkFaultMon_callback_t callback)
{
#if defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE)
    #if defined(CONFIG_SDK_RTL8390) && !defined(__MODEL_USER__)
    return dal_linkFaultMon_register(callback);
    #endif
#endif

    return RT_ERR_FAILED;
}   /* end of rtk_oam_linkFaultMon_register */

/* Function Name:
 *      rtk_oam_linkFaultMon_unregister
 * Description:
 *      Unregister callback function for link fault detect notification
 * Input:
 *      unit		- unit ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Applicable:
 *      8390
 * Note:
 *      None
 * Changes:
 *      None
 */
int32 rtk_oam_linkFaultMon_unregister(uint32 unit)
{
#if defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE)
    #if defined(CONFIG_SDK_RTL8390) && !defined(__MODEL_USER__)
    return dal_linkFaultMon_unregister();
    #endif
#endif

    return RT_ERR_FAILED;
}   /* end of rtk_oam_linkFaultMon_unregister */

/* Function Name:
 *      rtk_oam_pduLearningEnable_get
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
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_oam_pduLearningEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->oam_pduLearningEnable_get(unit, pEnable);
}   /* end of rtk_oam_pduLearningEnable_get */

/* Function Name:
 *      rtk_oam_pduLearningEnable_set
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
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_oam_pduLearningEnable_set(uint32 unit, rtk_enable_t enable)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->oam_pduLearningEnable_set(unit, enable);
}   /* end of rtk_oam_pduLearningEnable_set */

/* Function Name:
 *      rtk_oam_cfmCcmInstTxMepid_get
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
 * Applicable:
 *      9310
 * Note:
 *      - enable                is setting by rtk_oam_cfmCcmInstTxStatus_set
 *      - TPID                  is setting by rtk_oam_cfmCcmTpid_set
 *      - tag status            is setting by rtk_oam_cfmCcmInstTagStatus_set
 *      - priority code point   is setting by rtk_oam_cfmCcmPcp_set
 *      - CFI                   is setting by rtk_oam_cfmCcmCfi_set
 *      - VID                   is setting by rtk_oam_cfmCcmInstVid_set
 *      - MEPID                 is setting by rtk_oam_cfmCcmInstTxMepid_set
 *      - interval field        is setting by rtk_oam_cfmCcmInstTxIntervalField_set
 *      - MD level              is setting by rtk_oam_cfmCcmInstTxMdl_set
 *      - MAID                  is setting by rtk_oam_cfmCcmInstMaid_set
 *      - tx interval           is setting by rtk_oam_cfmCcmInstTxIntervalField_set
 *      - member                is setting by rtk_oam_cfmCcmInstTxMember_set
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_oam_cfmCcmInstTxMepid_get(uint32 unit, uint32 instance, uint32 *mepid)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->oam_cfmCcmInstTxMepid_get(unit, instance, mepid);
}   /* end of rtk_oam_cfmCcmInstTxMepid_get */

/* Function Name:
 *      rtk_oam_cfmCcmInstTxMepid_set
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
 * Applicable:
 *      9310
 * Note:
 *      - enable                is setting by rtk_oam_cfmCcmInstTxStatus_set
 *      - TPID                  is setting by rtk_oam_cfmCcmTpid_set
 *      - tag status            is setting by rtk_oam_cfmCcmInstTagStatus_set
 *      - priority code point   is setting by rtk_oam_cfmCcmPcp_set
 *      - CFI                   is setting by rtk_oam_cfmCcmCfi_set
 *      - VID                   is setting by rtk_oam_cfmCcmInstVid_set
 *      - MEPID                 is setting by rtk_oam_cfmCcmInstTxMepid_set
 *      - interval field        is setting by rtk_oam_cfmCcmInstTxIntervalField_set
 *      - MD level              is setting by rtk_oam_cfmCcmInstTxMdl_set
 *      - MAID                  is setting by rtk_oam_cfmCcmInstMaid_set
 *      - tx interval           is setting by rtk_oam_cfmCcmInstTxIntervalField_set
 *      - member                is setting by rtk_oam_cfmCcmInstTxMember_set
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_oam_cfmCcmInstTxMepid_set(uint32 unit, uint32 instance, uint32 mepid)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->oam_cfmCcmInstTxMepid_set(unit, instance, mepid);
}   /* end of rtk_oam_cfmCcmInstTxMepid_set */

/* Function Name:
 *      rtk_oam_cfmCcmInstTxIntervalField_get
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
 * Applicable:
 *      9310
 * Note:
 *      Support configurate fields for generate content of CCM frame is as following:
 *      - enable                is setting by rtk_oam_cfmCcmInstTxStatus_set
 *      - TPID                  is setting by rtk_oam_cfmCcmTpid_set
 *      - tag status            is setting by rtk_oam_cfmCcmInstTagStatus_set
 *      - priority code point   is setting by rtk_oam_cfmCcmPcp_set
 *      - CFI                   is setting by rtk_oam_cfmCcmCfi_set
 *      - VID                   is setting by rtk_oam_cfmCcmInstVid_set
 *      - MEPID                 is setting by rtk_oam_cfmCcmInstTxMepid_set
 *      - interval field        is setting by rtk_oam_cfmCcmInstTxIntervalField_set
 *      - MD level              is setting by rtk_oam_cfmCcmInstTxMdl_set
 *      - MAID                  is setting by rtk_oam_cfmCcmInstMaid_set
 *      - tx interval           is setting by rtk_oam_cfmCcmInstTxIntervalField_set
 *      - member                is setting by rtk_oam_cfmCcmInstTxMember_set
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_oam_cfmCcmInstTxIntervalField_get(uint32 unit, uint32 instance, uint32 *interval)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->oam_cfmCcmInstTxIntervalField_get(unit, instance, interval);
}   /* end of rtk_oam_cfmCcmInstTxIntervalField_get */

/* Function Name:
 *      rtk_oam_cfmCcmInstTxIntervalField_set
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
 * Applicable:
 *      9310
 * Note:
 *      Support configurate fields for generate content of CCM frame is as following:
 *      - enable                is setting by rtk_oam_cfmCcmInstTxStatus_set
 *      - TPID                  is setting by rtk_oam_cfmCcmTpid_set
 *      - tag status            is setting by rtk_oam_cfmCcmInstTagStatus_set
 *      - priority code point   is setting by rtk_oam_cfmCcmPcp_set
 *      - CFI                   is setting by rtk_oam_cfmCcmCfi_set
 *      - VID                   is setting by rtk_oam_cfmCcmInstVid_set
 *      - MEPID                 is setting by rtk_oam_cfmCcmInstTxMepid_set
 *      - interval field        is setting by rtk_oam_cfmCcmInstTxIntervalField_set
 *      - MD level              is setting by rtk_oam_cfmCcmInstTxMdl_set
 *      - MAID                  is setting by rtk_oam_cfmCcmInstMaid_set
 *      - tx interval           is setting by rtk_oam_cfmCcmInstTxIntervalField_set
 *      - member                is setting by rtk_oam_cfmCcmInstTxMember_set
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_oam_cfmCcmInstTxIntervalField_set(uint32 unit, uint32 instance, uint32 interval)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->oam_cfmCcmInstTxIntervalField_set(unit, instance, interval);
}   /* end of rtk_oam_cfmCcmInstTxIntervalField_set */

/* Function Name:
 *      rtk_oam_cfmCcmInstTxMdl_get
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
 * Applicable:
 *      9310
 * Note:
 *      Support configurate fields for generate content of CCM frame is as following:
 *      - enable                is setting by rtk_oam_cfmCcmInstTxStatus_set
 *      - TPID                  is setting by rtk_oam_cfmCcmTpid_set
 *      - tag status            is setting by rtk_oam_cfmCcmInstTagStatus_set
 *      - priority code point   is setting by rtk_oam_cfmCcmPcp_set
 *      - CFI                   is setting by rtk_oam_cfmCcmCfi_set
 *      - VID                   is setting by rtk_oam_cfmCcmInstVid_set
 *      - MEPID                 is setting by rtk_oam_cfmCcmInstTxMepid_set
 *      - interval field        is setting by rtk_oam_cfmCcmInstTxIntervalField_set
 *      - MD level              is setting by rtk_oam_cfmCcmInstTxMdl_set
 *      - MAID                  is setting by rtk_oam_cfmCcmInstMaid_set
 *      - tx interval           is setting by rtk_oam_cfmCcmInstTxIntervalField_set
 *      - member                is setting by rtk_oam_cfmCcmInstTxMember_set
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_oam_cfmCcmInstTxMdl_get(uint32 unit, uint32 instance, uint32 *mdl)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->oam_cfmCcmInstTxMdl_get(unit, instance, mdl);
}   /* end of rtk_oam_cfmCcmInstTxMdl_get */

/* Function Name:
 *      rtk_oam_cfmCcmInstTxMdl_set
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
 * Applicable:
 *      9310
 * Note:
 *      Support configurate fields for generate content of CCM frame is as following:
 *      - enable                is setting by rtk_oam_cfmCcmInstTxStatus_set
 *      - TPID                  is setting by rtk_oam_cfmCcmTpid_set
 *      - tag status            is setting by rtk_oam_cfmCcmInstTagStatus_set
 *      - priority code point   is setting by rtk_oam_cfmCcmPcp_set
 *      - CFI                   is setting by rtk_oam_cfmCcmCfi_set
 *      - VID                   is setting by rtk_oam_cfmCcmInstVid_set
 *      - MEPID                 is setting by rtk_oam_cfmCcmInstTxMepid_set
 *      - interval field        is setting by rtk_oam_cfmCcmInstTxIntervalField_set
 *      - MD level              is setting by rtk_oam_cfmCcmInstTxMdl_set
 *      - MAID                  is setting by rtk_oam_cfmCcmInstMaid_set
 *      - tx interval           is setting by rtk_oam_cfmCcmInstTxIntervalField_set
 *      - member                is setting by rtk_oam_cfmCcmInstTxMember_set
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_oam_cfmCcmInstTxMdl_set(uint32 unit, uint32 instance, uint32 mdl)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->oam_cfmCcmInstTxMdl_set(unit, instance, mdl);
}   /* end of rtk_oam_cfmCcmInstTxMdl_set */

/* Function Name:
 *      rtk_oam_cfmCcmInstTxMember_get
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
 * Applicable:
 *      9310
 * Note:
 *      Support configurate fields for generate content of CCM frame is as following:
 *      - enable                is setting by rtk_oam_cfmCcmInstTxStatus_set
 *      - TPID                  is setting by rtk_oam_cfmCcmTpid_set
 *      - tag status            is setting by rtk_oam_cfmCcmInstTagStatus_set
 *      - priority code point   is setting by rtk_oam_cfmCcmPcp_set
 *      - CFI                   is setting by rtk_oam_cfmCcmCfi_set
 *      - VID                   is setting by rtk_oam_cfmCcmInstVid_set
 *      - MEPID                 is setting by rtk_oam_cfmCcmInstTxMepid_set
 *      - interval field        is setting by rtk_oam_cfmCcmInstTxIntervalField_set
 *      - MD level              is setting by rtk_oam_cfmCcmInstTxMdl_set
 *      - MAID                  is setting by rtk_oam_cfmCcmInstMaid_set
 *      - tx interval           is setting by rtk_oam_cfmCcmInstTxIntervalField_set
 *      - member                is setting by rtk_oam_cfmCcmInstTxMember_set
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_oam_cfmCcmInstTxMember_get(uint32 unit, uint32 instance,
    rtk_oam_cfmInstMember_t *member)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->oam_cfmCcmInstTxMember_get(unit, instance, member);
}   /* end of rtk_oam_cfmCcmInstTxMember_get */

/* Function Name:
 *      rtk_oam_cfmCcmInstTxMember_set
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
 * Applicable:
 *      9310
 * Note:
 *      Support configurate fields for generate content of CCM frame is as following:
 *      - enable                is setting by rtk_oam_cfmCcmInstTxStatus_set
 *      - TPID                  is setting by rtk_oam_cfmCcmTpid_set
 *      - tag status            is setting by rtk_oam_cfmCcmInstTagStatus_set
 *      - priority code point   is setting by rtk_oam_cfmCcmPcp_set
 *      - CFI                   is setting by rtk_oam_cfmCcmCfi_set
 *      - VID                   is setting by rtk_oam_cfmCcmInstVid_set
 *      - MEPID                 is setting by rtk_oam_cfmCcmInstTxMepid_set
 *      - interval field        is setting by rtk_oam_cfmCcmInstTxIntervalField_set
 *      - MD level              is setting by rtk_oam_cfmCcmInstTxMdl_set
 *      - MAID                  is setting by rtk_oam_cfmCcmInstMaid_set
 *      - tx interval           is setting by rtk_oam_cfmCcmInstTxIntervalField_set
 *      - member                is setting by rtk_oam_cfmCcmInstTxMember_set
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_oam_cfmCcmInstTxMember_set(uint32 unit, uint32 instance,
    rtk_oam_cfmInstMember_t *member)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->oam_cfmCcmInstTxMember_set(unit, instance, member);
}   /* end of rtk_oam_cfmCcmInstTxMember_set */

/* Function Name:
 *      rtk_oam_cfmCcmInstRxMember_get
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
 * Applicable:
 *      9310
 * Note:
 *      Support information for receiving CCM frame is as following:
 *      - lifetime  is setting by rtk_oam_cfmCcmInstLifetime_set
 *      - vid       is setting by rtk_oam_cfmCcmRxInstVid_set
 *      - port      is setting by rtk_oam_cfmCcmRxInstPort_set
 *      - keepalive is get by rtk_oam_cfmCcmInstAliveTime_get
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_oam_cfmCcmInstRxMember_get(uint32 unit, uint32 instance,
    rtk_oam_cfmInstMember_t *member)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->oam_cfmCcmInstRxMember_get(unit, instance, member);
}   /* end of rtk_oam_cfmCcmInstRxMember_get */

/* Function Name:
 *      rtk_oam_cfmCcmInstRxMember_set
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
 * Applicable:
 *      9310
 * Note:
 *      Support information for receiving CCM frame is as following:
 *      - lifetime  is setting by rtk_oam_cfmCcmInstLifetime_set
 *      - vid       is setting by rtk_oam_cfmCcmRxInstVid_set
 *      - port      is setting by rtk_oam_cfmCcmRxInstPort_set
 *      - keepalive is get by rtk_oam_cfmCcmInstAliveTime_get
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_oam_cfmCcmInstRxMember_set(uint32 unit, uint32 instance,
    rtk_oam_cfmInstMember_t *member)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->oam_cfmCcmInstRxMember_set(unit, instance, member);
}   /* end of rtk_oam_cfmCcmInstRxMember_set */


