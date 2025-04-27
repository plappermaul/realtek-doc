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
 * Purpose : Definition of Mirror API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) Port-based mirror
 *           (2) Group-based mirror
 *           (3) RSPAN
 *           (4) Mirror-based SFLOW
 *           (5) Port-based SFLOW
 */

#ifndef __RTK_MIRROR_H__
#define __RTK_MIRROR_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/port.h>

/*
 * Symbol Definition
 */

/* egress mode of mirror on egress port */
typedef enum rtk_mirror_egrMode_e
{
    FORWARD_ALL_PKTS,           /* forwarded and mirrored packet are allowed to send to mirroring port */
    FORWARD_MIRRORED_PKTS_ONLY, /* only mirrored packet is allowed to send to mirroring port */
    MIRROR_EGR_MODE_END
} rtk_mirror_egrMode_t;

/* Ingress mode of RSPAN */
typedef enum rtk_mirror_rspanIgrMode_e
{
    RSPAN_IGR_IGNORE_RSPAN_TAG = 0,/* disable the device to parse RSPAN tag for received packet */
    RSPAN_IGR_HANDLE_RSPAN_TAG,    /* enable the device to parse RSPAN tag for received packet */
    RSPAN_IGR_END
} rtk_mirror_rspanIgrMode_t;

/* Egress mode of RSPAN */
typedef enum rtk_mirror_rspanEgrMode_e
{
    RSPAN_EGR_REMOVE_TAG = 0,
    RSPAN_EGR_ADD_TAG,
    RSPAN_EGR_NO_MODIFY,
    RSPAN_EGR_END
} rtk_mirror_rspanEgrMode_t;

/* sFlow control */
typedef enum rtk_sflowSampleCtrl_e
{
    SFLOW_CTRL_INGRESS,
    SFLOW_CTRL_EGRESS,
    SFLOW_CTRL_END
} rtk_sflowSampleCtrl_t;

typedef enum rtk_sflow_sampleTarget_e
{
    RTK_SFLOW_SAMPLE_LOCAL,
    RTK_SFLOW_SAMPLE_MASTER,
    RTK_SFLOW_SAMPLE_END,
} rtk_sflow_sampleTarget_t;


#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
/* Mirror Type */
typedef enum rtk_mirror_type_e
{
    DISABLE_MIRROR = 0,
    PORT_BASED_MIRROR,
    RSPAN_BASED_MIRROR,
    FLOW_BASED_MIRROR,
    MIRROR_TYPE_END
} rtk_mirror_type_t;

typedef enum rtk_mtp_type_e
{
    MTP_TYPE_NOT_TRK,
    MTP_TYPE_IS_TRK,
    MTP_TYPE_END
} rtk_mtp_type_t;

#endif

typedef enum rtk_mirror_op_e
{
    MIRROR_OP_OR = 0,
    MIRROR_OP_AND,
    MIRROR_OP_END
} rtk_mirror_op_t;

typedef enum rtk_mirror_vlan_mode_e
{
    MIRROR_VLAN_MODEL_NORMAL,
    MIRROR_VLAN_MODEL_SPECIAL,
    MIRROR_VLAN_MODEL_END,
} rtk_mirror_vlan_mode_t;

typedef enum rtk_sflow_arbitor_id_e
{
    SFLOW_ARB_NORMAL_API = 0,
    SFLOW_ARB_0,
    SFLOW_ARB_MAX
} rtk_sflow_arbitor_id_t;


/*
 * Data Declaration
 */
/* configuration of mirror entry */
typedef struct rtk_mirror_entry_s
{
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380)
    rtk_enable_t                mirror_enable;
#endif

#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    rtk_mirror_type_t          mirror_type;
    rtk_mtp_type_t              mtp_type;
#endif

#if defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL9300)
    uint32                          mirroring_devID;
#endif
    rtk_port_t                      mirroring_port;
    rtk_portmask_t              mirrored_igrPorts;
    rtk_portmask_t              mirrored_egrPorts;
    rtk_mirror_op_t             oper_of_igr_and_egr_ports;
    rtk_enable_t                  mirror_orginalPkt;
    rtk_mirror_egrMode_t      mirroring_port_egrMode;

#if defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL9300)  || defined(CONFIG_SDK_RTL9310)
    rtk_enable_t                    self_flter;
    rtk_enable_t                    duplicate_fltr;
    rtk_mirror_vlan_mode_t   mir_mode;
#endif

#if defined(CONFIG_SDK_RTL8380)
    rtk_enable_t     cross_vlan;
    rtk_enable_t     flowBasedOnly;
    uint32              flowBased_pmsk_ignore;
    uint32              mir_qid;
    rtk_enable_t     mir_qid_en;
#endif
} rtk_mirror_entry_t;


/* Tag of RSPAN */
typedef struct rtk_mirror_rspanTag_s
{
#if defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL9300)  || defined(CONFIG_SDK_RTL9310)
    uint32          tpid;
#endif
#if defined(CONFIG_SDK_RTL8390)
    uint32          tpidIdx;
#endif
    rtk_vlan_t      vid;
    rtk_pri_t       pri;
    uint32          cfi;
} rtk_mirror_rspanTag_t;

typedef enum rtk_sflow_stsChg_e
{
    SFLOW_DISABLE = 0,
    SFLOW_ENABLE,
    SFLOW_END
} rtk_sflow_stsChg_t;

typedef int32 (*sflowStsChg_cb_f)(uint32 unit, rtk_sflow_stsChg_t type);


/*
 * Macro Declaration
 */

/*
 * Function Declaration
 */

/* Module Name : Mirror */

/* Function Name:
 *      rtk_mirror_init
 * Description:
 *      Initialize the mirroring database.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Must initialize Mirror module before calling any Mirror APIs.
 * Changes:
 *      None
 */
extern int32
rtk_mirror_init(uint32 unit);

/* Module Name    : Mirror             */
/* Sub-module Name: Group-based mirror */

/* Function Name:
 *      rtk_mirror_group_init
 * Description:
 *      Initialization mirror group entry.
 * Input:
 *      unit         - unit id
 *      pMirrorEntry - mirror entry
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Initialize the mirror entry. The operation is set to ingress OR egress ports.
 *      The mirroring_port, mirrored_igrPorts, and mirrored_egrPorts fields are set to empty,
 *      and should be assigned later by rtk_mirror_group_set API.
 * Changes:
 *      None
 */
extern int32
rtk_mirror_group_init(
    uint32              unit,
    rtk_mirror_entry_t  *pMirrorEntry);

/* Function Name:
 *      rtk_mirror_group_get
 * Description:
 *      Get mirror group entry.
 * Input:
 *      unit         - unit id
 *      mirror_id    - mirror id
 * Output:
 *      pMirrorEntry - mirror entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_MIRROR_ID    - invalid mirror id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The valid range of mirror_id is 0~3 in 8380, 8390, 9300, 9310.
 * Changes:
 *      [SDK_3.0.0]
 *          (1) Add mirror_type, mtp_type, mirroring_unit, self_flter, duplicate_fltr, mir_mode fields in
 *              pMirrorEntry structure for 9300 and 9310
 */
extern int32
rtk_mirror_group_get(
    uint32              unit,
    uint32              mirror_id,
    rtk_mirror_entry_t  *pMirrorEntry);

/* Function Name:
 *      rtk_mirror_group_set
 * Description:
 *      Set mirror group entry.
 * Input:
 *      unit         - unit id
 *      mirror_id    - mirror id
 *      pMirrorEntry - mirror entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID              - invalid unit id
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_MIRROR_ID            - invalid mirror id
 *      RT_ERR_PORT_ID              - invalid mirroring port id
 *      RT_ERR_PORT_MASK            - invalid mirrored ingress or egress portmask
 *      RT_ERR_INPUT                - invalid input parameter
 *      RT_ERR_MIRROR_DP_IN_SPM_DPM - mirroring port can not be in ingress or egress mirrored portmask of any mirroring set
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The valid range of mirror_id is 0~3 in 8380, 8390, 9300, 9310.
 * Changes:
 *      [SDK_3.0.0]
 *          (1) Add mirror_type, mtp_type, mirroring_unit, self_flter, duplicate_fltr, mir_mode fields in
 *              pMirrorEntry structure for 9300 and 9310
 */
extern int32
rtk_mirror_group_set(
    uint32              unit,
    uint32              mirror_id,
    rtk_mirror_entry_t  *pMirrorEntry);

/* Function Name:
 *      rtk_mirror_rspanIgrMode_get
 * Description:
 *      Get ingress mode of RSPAN on specified mirror group.
 * Input:
 *      unit      - unit id
 *      mirror_id - mirror id
 * Output:
 *      pIgrMode  - pointer to ingress mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_MIRROR_ID    - invalid mirror ID
 * Applicable:
 *      8380, 8390
 * Note:
 *      9300, 9310 donot support this featrue no longer.
 *      (1) Ingress mode is as following:
 *          - RSPAN_IGR_HANDLE_RSPAN_TAG
 *          - RSPAN_IGR_IGNORE_RSPAN_TAG
 *      (2) Set RSPAN igress mode to RSPAN_IGR_HANDLE_RSPAN_TAG for destination switch.
 * Changes:
 *      None
 */
extern int32
rtk_mirror_rspanIgrMode_get(uint32 unit, uint32 mirror_id, rtk_mirror_rspanIgrMode_t *pIgrMode);

/* Function Name:
 *      rtk_mirror_rspanIgrMode_set
 * Description:
 *      Set ingress mode of RSPAN on specified mirror group.
 * Input:
 *      unit      - unit id
 *      mirror_id - mirror id
 *      igrMode   - ingress mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID   - invalid unit id
 *      RT_ERR_NOT_INIT  - The module is not initial
 *      RT_ERR_MIRROR_ID - invalid mirror ID
 *      RT_ERR_INPUT     - invalid input parameter
 * Applicable:
 *      8380, 8390
 * Note:
 *      9300, 9310 donot support this featrue no longer.
 *      (1) Ingress mode is as following:
 *          - RSPAN_IGR_HANDLE_RSPAN_TAG
 *          - RSPAN_IGR_IGNORE_RSPAN_TAG
 *      (2) Set RSPAN igress mode to RSPAN_IGR_HANDLE_RSPAN_TAG for destination switch.
 * Changes:
 *      None
 */
extern int32
rtk_mirror_rspanIgrMode_set(uint32 unit, uint32 mirror_id, rtk_mirror_rspanIgrMode_t igrMode);

/* Function Name:
 *      rtk_mirror_rspanEgrMode_get
 * Description:
 *      Get egress mode of RSPAN on specified mirror group.
 * Input:
 *      unit      - unit id
 *      mirror_id - mirror id
 * Output:
 *      pEgrMode  - pointer to egress mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_MIRROR_ID    - invalid mirror ID
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) RSPAN egress mode should be set to RSPAN_EGR_ADD_TAG for source switch and set to RSPAN_EGR_REMOVE_TAG
 *          for destination switch.
 *      (2) Egress mode is as following:
 *          - RSPAN_EGR_REMOVE_TAG
 *          - RSPAN_EGR_ADD_TAG
 *          - RSPAN_EGR_NO_MODIFY
 * Changes:
 *      None
 */
extern int32
rtk_mirror_rspanEgrMode_get(uint32 unit, uint32 mirror_id, rtk_mirror_rspanEgrMode_t *pEgrMode);

/* Function Name:
 *      rtk_mirror_rspanEgrMode_set
 * Description:
 *      Set egress mode of RSPAN on specified mirror group.
 * Input:
 *      unit      - unit id
 *      mirror_id - mirror id
 *      egrMode   - egress mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID   - invalid unit id
 *      RT_ERR_NOT_INIT  - The module is not initial
 *      RT_ERR_MIRROR_ID - invalid mirror ID
 *      RT_ERR_INPUT     - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) RSPAN egress mode should be set to RSPAN_EGR_ADD_TAG for source switch and set to RSPAN_EGR_REMOVE_TAG
 *          for destination switch.
 *      (2) Ingress mode is as following:
 *          - RSPAN_EGR_REMOVE_TAG
 *          - RSPAN_EGR_ADD_TAG
 *          - RSPAN_EGR_NO_MODIFY
 * Changes:
 *      None
 */
extern int32
rtk_mirror_rspanEgrMode_set(uint32 unit, uint32 mirror_id, rtk_mirror_rspanEgrMode_t egrMode);


/* Module Name    : Mirror */
/* Sub-module Name: RSPAN  */

/* Function Name:
 *      rtk_mirror_rspanTag_get
 * Description:
 *      Get content of RSPAN tag on specified mirror group.
 * Input:
 *      unit      - unit id
 *      mirror_id - mirror id
 * Output:
 *      pTag      - pointer to content of RSPAN tag
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_MIRROR_ID    - invalid mirror ID
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) Each mirror group can specify its RSPAN tag content.
 *      (2) pTag->tpidIdx is the index to VLAN outer TPID list and rtk_vlan_outerTpidEntry_set could be used
 *          to configure the outer VLAN TPID.
 * Changes:
 *      None
 */
extern int32
rtk_mirror_rspanTag_get(uint32 unit, uint32 mirror_id, rtk_mirror_rspanTag_t *pTag);

/* Function Name:
 *      rtk_mirror_rspanTag_set
 * Description:
 *      Set content of RSPAN tag on specified mirroring group.
 * Input:
 *      unit      - unit id
 *      mirror_id - mirror id
 *      pTag      - content of RSPAN tag
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_MIRROR_ID    - invalid mirror ID
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_PRIORITY     - invalid priority
 *      RT_ERR_VLAN_VID     - invalid vid
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) Each mirror group can specify its RSPAN tag content.
 *      (2) pTag->tpidIdx is the index to VLAN outer TPID list and rtk_vlan_outerTpidEntry_set could be used
 *          to configure the outer VLAN TPID.
 * Changes:
 *      None
 */
extern int32
rtk_mirror_rspanTag_set(uint32 unit, uint32 mirror_id, rtk_mirror_rspanTag_t *pTag);

/* Module Name    : Mirror             */
/* Sub-module Name: Mirror-based SFLOW */

/* Function Name:
 *      rtk_mirror_sflowMirrorSampleRate_get
 * Description:
 *      Get sampling rate of specified mirror group.
 * Input:
 *      unit      - unit id
 *      mirror_id - mirror id
 * Output:
 *      pRate     - pointer to sampling rate
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_MIRROR_ID    - invalid mirror id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) Set rate to N means that one packet is sampled out of N mirrored packets.
 *      (2) The function is disabled if the rate is set to 0.
 * Changes:
 *      None
 */
extern int32
rtk_mirror_sflowMirrorSampleRate_get(uint32 unit, uint32 mirror_id, uint32 *pRate);

/* Function Name:
 *      rtk_mirror_sflowMirrorSampleRate_set
 * Description:
 *      Set sampling rate of specified mirror group.
 * Input:
 *      unit      - unit id
 *      mirror_id - mirror id
 *      rate      - sampling rate
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_MIRROR_ID    - invalid mirror id
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) Set rate to N means that one packet is sampled out of N mirrored packets.
 *      (2) The function is disabled if the rate is set to 0.
 * Changes:
 *      None
 */
extern int32
rtk_mirror_sflowMirrorSampleRate_set(uint32 unit, uint32 mirror_id, uint32 rate);


/* Function Name:
 *      rtk_mirror_egrQueue_get
 * Description:
 *      Get enable status and output queue ID of mirror packet.
 * Input:
 *      unit      - unit id
 * Output:
 *      pEnable    - pointer to mirror packet dedicated output queue ID enable status
 *      pQid          - pointer to mirror packet output queue ID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_mirror_egrQueue_get(uint32 unit, rtk_enable_t *pEnable, rtk_qid_t *pQid);

/* Function Name:
 *      rtk_mirror_egrQueue_set
 * Description:
 *      Set enable status and output queue ID of mirror packet.
 * Input:
 *      unit      - unit id
 *      enable   - mirror packet dedicated output queue ID enable status
 *      qid        - mirror packet output queue ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_QUEUE_ID     - Invalid queue ID
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) mirror packet would follow this queue configuration if the function enable status is enabled
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_mirror_egrQueue_set(uint32 unit, rtk_enable_t enable, rtk_qid_t qid);

/* Function Name:
 *      rtk_mirror_sflowPortIgrSampleRate_get
 * Description:
 *      Get sampling rate of ingress sampling on specified port.
 * Input:
 *      unit  - unit id
 *      port  - port id
 * Output:
 *      pRate - pointer to sampling rate of ingress sampling
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
 *      (1) The function is disabled if the rate is set to 0.
 * Changes:
 *      None
 */
extern int32
rtk_mirror_sflowPortIgrSampleRate_get(uint32 unit, rtk_port_t port, uint32 *pRate);

/* Function Name:
 *      rtk_mirror_sflowPortIgrSampleRate_set
 * Description:
 *      Set sampling rate of ingress sampling on specified port.
 * Input:
 *      unit - unit id
 *      port - port id
 *      rate - sampling rate of ingress sampling
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      (1) The function is disabled if the rate is set to 0.
 * Changes:
 *      None
 */
extern int32
rtk_mirror_sflowPortIgrSampleRate_set(uint32 unit, rtk_port_t port, uint32 rate);

/* Function Name:
 *      rtk_mirror_sflowPortEgrSampleRate_get
 * Description:
 *      Get sampling rate of egress sampling on specified port.
 * Input:
 *      unit  - unit id
 *      port  - port id
 * Output:
 *      pRate - pointer to sampling rate of egress sampling
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
 *      (1) The function is disabled if the rate is set to 0.
 * Changes:
 *      None
 */
extern int32
rtk_mirror_sflowPortEgrSampleRate_get(uint32 unit, rtk_port_t port, uint32 *pRate);

/* Function Name:
 *      rtk_mirror_sflowPortEgrSampleRate_set
 * Description:
 *      Set sampling rate of egress sampling on specified port.
 * Input:
 *      unit - unit id
 *      port - port id
 *      rate - sampling rate of egress sampling
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      (1) The function is disabled if the rate is set to 0.
 * Changes:
 *      None
 */
extern int32
rtk_mirror_sflowPortEgrSampleRate_set(uint32 unit, rtk_port_t port, uint32 rate);

/* Function Name:
 *      rtk_mirror_sflowSampleCtrl_get
 * Description:
 *      Get sampling preference when a packet is both ingress and egress sampled.
 * Input:
 *      unit  - unit id
 * Output:
 *      pCtrl - pointer to sampling preference
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      The API indicate which sampling to take if a packet is both ingress and egress sampled.
 * Changes:
 *      None
 */
extern int32
rtk_mirror_sflowSampleCtrl_get(uint32 unit, rtk_sflowSampleCtrl_t *pCtrl);

/* Function Name:
 *      rtk_mirror_sflowSampleCtrl_set
 * Description:
 *      Set sampling preference when a packet is both ingress and egress sampled.
 * Input:
 *      unit - unit id
 *      ctrl - sampling preference
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
 *      The API indicate which sampling to take if a packet is both ingress and egress sampled.
 * Changes:
 *      None
 */
extern int32
rtk_mirror_sflowSampleCtrl_set(uint32 unit, rtk_sflowSampleCtrl_t ctrl);

/* Function Name:
 *      rtk_mirror_sflowSampleTarget_get
 * Description:
 *      Get information of sFlow sample packet to local or master CPU.
 * Input:
 *      unit    - unit id
 * Output:
 *      pTarget - pointer to the information of sFlow sample packet
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_mirror_sflowSampleTarget_get(uint32 unit, rtk_sflow_sampleTarget_t *pTarget);

/* Function Name:
 *      rtk_mirror_sflowSampleTarget_set
 * Description:
 *      Set information of MPLS trap packet to local or master CPU.
 * Input:
 *      unit    - unit id
 *      target  - the status of MPLS trap packet
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
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
rtk_mirror_sflowSampleTarget_set(uint32 unit, rtk_sflow_sampleTarget_t target);

#if defined(CONFIG_SFLOW_PG_THREAD)
/* Function Name:
 *      rtk_mirror_sflowStsChg_register
 * Description:
 *      Register to receive callbacks for when sFlow enabling status change due to used-page count exceed.
 * Input:
 *      unit        - unit id
 *      stsChg_cb   - callback function for sFlow enabling status change
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_mirror_sflowStsChg_register(uint32 unit, sflowStsChg_cb_f stsChg_cb);
#endif
#endif /* __RTK_MIRROR_H__ */

