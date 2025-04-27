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

#ifndef __RTK_OAM_H__
#define __RTK_OAM_H__

/*
 * Include Files
 */
#include <common/rt_type.h>

/*
 * Symbol Definition
 */
/* length of MAID */
#define RTK_MAX_LEN_OF_CFM_MAID     48

/* Length of dying gasp TLV */
#define RTK_OAM_REMOTE_TLV_LEN      16
#define RTK_OAM_LOCAL_TLV_LEN       16
#define RTK_OAM_DYINGGASPPAYLOAD_MAX    1522


#define RTK_CFM_INSTANCE_MAX        8       /* max instance of CFM */
#define RTK_CFM_MDL_MAX             8       /* max md level of CFM */
#define RTK_CFM_RESET_LIFETIME_MAX  0x3FF
#define RTK_CFM_MEPID_MAX           0x1FFF
#define RTK_CFM_LIFETIME_MAX        8
#define RTK_CFM_MAID_MAX            8
#define RTK_CFM_TX_INTERVAL         0x3FF   /* max tx interval (msec) */
#define RTK_CFM_CCM_PORT_MAX        2       /* max ports of CCM control member */
#define RTK_CFM_TX_INSTANCE_MAX     8       /* max tx instance of CCM control */
#define RTK_CFM_RX_INSTANCE_MAX     8       /* max rx instance of CCM control */

/*
 * Data Declaration
 */
/* Status of transmitting test frame */
typedef enum rtk_oam_testFrameTxStatus_e
{
    TX_FINISH = 0,
    TX_NOT_FINISH,
    TEST_FRAME_TX_END
} rtk_oam_testFrameTxStatus_t;

/* SA action of loopback frame */
typedef enum rtk_oam_loopbackSA_action_e
{
    SA_ACTION_KEEP_MAC = 0,
    SA_ACTION_USE_SWITCH_MAC,
    SA_ACTION_USE_DA,
    SA_ACTION_USE_USER_DEFINE_MAC,
    SA_ACTION_END
} rtk_oam_loopbackSA_action_t;

/* DA action of loopback frame */
typedef enum rtk_oam_loopbackDA_action_e
{
    DA_ACTION_KEEP_MAC = 0,
    DA_ACTION_USE_SWITCH_MAC,
    DA_ACTION_USE_SA,
    DA_ACTION_USE_USER_DEFINE_MAC,
    DA_ACTION_END
} rtk_oam_loopbackDA_action_t;

/* Header of CCM frame */
typedef struct rtk_oam_ccmFrame_s
{
    rtk_pktType_t   pktType;    /* ETHERNET_II_PACKET or SNAP_PACKET */
    rtk_enable_t    enable_outerTag;
    uint32          outer_tpid;
    rtk_vlan_t      outer_vid;
    rtk_pri_t       outer_pri;
    uint32          outer_dei;
    rtk_enable_t    enable_innerTag;
    uint32          inner_tpid;
    rtk_vlan_t      inner_vid;
    rtk_pri_t       inner_pri;
    uint32          inner_cfi;
    rtk_mac_t       dest_mac;
    rtk_mac_t       source_mac;
} rtk_oam_ccmFrame_t;

/* CFM configuration of each MA */
typedef struct rtk_oam_cfm_s
{
    uint32      md_level;
    uint8       maid[RTK_MAX_LEN_OF_CFM_MAID];
} rtk_oam_cfm_t;

/* Control of replying CFM loopback */
typedef struct rtk_oam_cfmLoopbackCtrl_s
{
    rtk_enable_t    keep_innerTag;
    rtk_enable_t    keep_outerTag;
} rtk_oam_cfmLoopbackCtrl_t;

typedef struct rtk_oam_linkFaultEvent_s
{
    uint8   instance;
    uint8   instancePort;
} rtk_oam_linkFaultEvent_t;

/* ETH-DM egress delay adjustment that would be added in egress time stamp. Unit:8 nanoseconds  */
typedef struct rtk_oam_cfmEthDmTxDelay_s
{
    uint8 delay_10M; /*Egress delay adjustment for ports link at 10M bps speed*/
    uint8 delay_100M; /*Egress delay adjustment for ports link at 100M bps speed */
    uint8 delay_1G; /* Egress delay adjustment for ports link at 500M, 1G and 2.5G bps speed */
    uint8 delay_10G; /* Egress delay adjustment for ports link at 10G bps speed */
} rtk_oam_cfmEthDmTxDelay_t;

typedef struct rtk_oam_cfmInstMember_s
{
    rtk_port_t      member0_port;
    rtk_port_t      member1_port;
    rtk_enable_t    member0_truk_present;
    rtk_enable_t    member1_truk_present;
} rtk_oam_cfmInstMember_t;

typedef int32 (*rtk_oam_linkFaultMon_callback_t)(rtk_oam_linkFaultEvent_t *);

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
extern int32
rtk_oam_init(uint32 unit);

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
extern int32
rtk_oam_portDyingGaspPayload_set(uint32 unit, rtk_port_t port, uint8 *pPayload,
    uint32 len);

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
extern int32
rtk_oam_dyingGaspSend_set(uint32 unit, rtk_enable_t enable);

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
extern int32
rtk_oam_autoDyingGaspEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

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
extern int32
rtk_oam_autoDyingGaspEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

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
extern int32
rtk_oam_dyingGaspWaitTime_get(uint32 unit, uint32 *pTime);

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
extern int32
rtk_oam_dyingGaspWaitTime_set(uint32 unit, uint32 time);

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
extern int32
rtk_oam_loopbackMacSwapEnable_get(uint32 unit, rtk_enable_t *pEnable);

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
extern int32
rtk_oam_loopbackMacSwapEnable_set(uint32 unit, rtk_enable_t enable);

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
extern int32
rtk_oam_portLoopbackMuxAction_get(uint32 unit, rtk_port_t port, rtk_action_t *pAction);

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
extern int32
rtk_oam_portLoopbackMuxAction_set(uint32 unit, rtk_port_t port, rtk_action_t action);

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
extern int32
rtk_oam_cfmCcmPcp_get(uint32 unit, uint32 *pPcp);

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
extern int32
rtk_oam_cfmCcmPcp_set(uint32 unit, uint32 pcp);

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
extern int32
rtk_oam_cfmCcmCfi_get(uint32 unit, uint32 *pCfi);

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
extern int32
rtk_oam_cfmCcmCfi_set(uint32 unit, uint32 cfi);

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
extern int32
rtk_oam_cfmCcmTpid_get(uint32 unit, uint32 *pTpid);

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
extern int32
rtk_oam_cfmCcmTpid_set(uint32 unit, uint32 tpid);

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
extern int32
rtk_oam_cfmCcmInstLifetime_get(uint32 unit, uint32 instance, uint32 *pLifetime);

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
extern int32
rtk_oam_cfmCcmInstLifetime_set(uint32 unit, uint32 instance, uint32 lifetime);

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
extern int32
rtk_oam_cfmCcmMepid_get(uint32 unit, uint32 *pMepid);

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
extern int32
rtk_oam_cfmCcmMepid_set(uint32 unit, uint32 mepid);

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
extern int32
rtk_oam_cfmCcmIntervalField_get(uint32 unit, uint32 *pInterval);

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
extern int32
rtk_oam_cfmCcmIntervalField_set(uint32 unit, uint32 interval);

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
extern int32
rtk_oam_cfmCcmMdl_get(uint32 unit, uint32 *pMdl);

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
extern int32
rtk_oam_cfmCcmMdl_set(uint32 unit, uint32 mdl);

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
extern int32
rtk_oam_cfmCcmInstTagStatus_get(uint32 unit, uint32 instance, rtk_enable_t *pEnable);

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
extern int32
rtk_oam_cfmCcmInstTagStatus_set(uint32 unit, uint32 instance, rtk_enable_t enable);

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
extern int32
rtk_oam_cfmCcmInstVid_get(uint32 unit, uint32 instance, rtk_vlan_t *pVid);

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
extern int32
rtk_oam_cfmCcmInstVid_set(uint32 unit, uint32 instance, rtk_vlan_t vid);

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
extern int32
rtk_oam_cfmCcmInstMaid_get(uint32 unit, uint32 instance, uint32 *pMaid);

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
extern int32
rtk_oam_cfmCcmInstMaid_set(uint32 unit, uint32 instance, uint32 maid);

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
extern int32
rtk_oam_cfmCcmInstTxStatus_get(uint32 unit, uint32 instance, rtk_enable_t *pEnable);

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
extern int32
rtk_oam_cfmCcmInstTxStatus_set(uint32 unit, uint32 instance, rtk_enable_t enable);

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
extern int32
rtk_oam_cfmCcmInstInterval_get(uint32 unit, uint32 instance, uint32 *pInterval);

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
extern int32
rtk_oam_cfmCcmInstInterval_set(uint32 unit, uint32 instance, uint32 interval);

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
extern int32
rtk_oam_cfmCcmTxInstPort_get(uint32 unit, uint32 instance, uint32 index, rtk_port_t *pPort);

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
extern int32
rtk_oam_cfmCcmTxInstPort_set(uint32 unit, uint32 instance, uint32 index, rtk_port_t port);

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
extern int32
rtk_oam_cfmCcmRxInstVid_get(uint32 unit, uint32 instance, rtk_vlan_t *pVid);

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
extern int32
rtk_oam_cfmCcmRxInstVid_set(uint32 unit, uint32 instance, rtk_vlan_t vid);

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
extern int32
rtk_oam_cfmCcmRxInstPort_get(uint32 unit, uint32 instance, uint32 index, rtk_port_t *pPort);

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
extern int32
rtk_oam_cfmCcmRxInstPort_set(uint32 unit, uint32 instance, uint32 index, rtk_port_t port);

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
extern int32
rtk_oam_cfmCcmInstAliveTime_get(uint32 unit, uint32 instance, uint32 index, uint32 *pTime);

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
extern int32
rtk_oam_cfmPortEthDmEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);


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
extern int32
rtk_oam_cfmPortEthDmEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

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
extern int32
rtk_oam_cfmEthDmRxTimestamp_get(
    uint32 unit,
    uint32 index,
    rtk_time_timeStamp_t *pTimeStamp);

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
extern int32
rtk_oam_cfmEthDmTxDelay_get(
    uint32 unit,
    rtk_oam_cfmEthDmTxDelay_t *pTxDelay);

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
extern int32
rtk_oam_cfmEthDmTxDelay_set(
    uint32                      unit,
    rtk_oam_cfmEthDmTxDelay_t txDelay);

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
extern int32
rtk_oam_cfmEthDmRefTime_get(uint32 unit, rtk_time_timeStamp_t *pTimeStamp);

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
extern int32
rtk_oam_cfmEthDmRefTime_set(uint32 unit, rtk_time_timeStamp_t timeStamp);

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
extern int32
rtk_oam_cfmEthDmRefTimeEnable_get(uint32 unit, rtk_enable_t *pEnable);

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
extern int32
rtk_oam_cfmEthDmRefTimeEnable_set(uint32 unit, rtk_enable_t enable);

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
extern int32
rtk_oam_cfmEthDmRefTimeFreq_get(uint32 unit, uint32 *pFreq);

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
extern int32
rtk_oam_cfmEthDmRefTimeFreq_set(uint32 unit, uint32 freq);


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
extern int32
rtk_oam_dyingGaspPktCnt_get(uint32 unit, uint32 *pCnt);

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
extern int32
rtk_oam_dyingGaspPktCnt_set(uint32 unit, uint32 cnt);

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
extern int32 rtk_oam_linkFaultMonEnable_set(uint32 unit, rtk_enable_t enable);

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
extern int32 rtk_oam_linkFaultMon_register(uint32 unit, rtk_oam_linkFaultMon_callback_t callback);

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
extern int32 rtk_oam_linkFaultMon_unregister(uint32 unit);

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
extern int32
rtk_oam_pduLearningEnable_get(uint32 unit, rtk_enable_t *pEnable);

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
extern int32
rtk_oam_pduLearningEnable_set(uint32 unit, rtk_enable_t enable);

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
extern int32
rtk_oam_cfmCcmInstTxMepid_get(uint32 unit, uint32 instance, uint32 *mepid);

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
extern int32
rtk_oam_cfmCcmInstTxMepid_set(uint32 unit, uint32 instance, uint32 mepid);

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
extern int32
rtk_oam_cfmCcmInstTxIntervalField_get(uint32 unit, uint32 instance, uint32 *interval);

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
extern int32
rtk_oam_cfmCcmInstTxIntervalField_set(uint32 unit, uint32 instance, uint32 interval);

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
extern int32
rtk_oam_cfmCcmInstTxMdl_get(uint32 unit, uint32 instance, uint32 *mdl);

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
extern int32
rtk_oam_cfmCcmInstTxMdl_set(uint32 unit, uint32 instance, uint32 mdl);

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
extern int32
rtk_oam_cfmCcmInstTxMember_get(uint32 unit, uint32 instance,
    rtk_oam_cfmInstMember_t *member);

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
extern int32
rtk_oam_cfmCcmInstTxMember_set(uint32 unit, uint32 instance,
    rtk_oam_cfmInstMember_t *member);

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
extern int32
rtk_oam_cfmCcmInstRxMember_get(uint32 unit, uint32 instance,
    rtk_oam_cfmInstMember_t *member);

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
extern int32
rtk_oam_cfmCcmInstRxMember_set(uint32 unit, uint32 instance,
    rtk_oam_cfmInstMember_t *member);

#endif /* __RTK_OAM_H__ */
