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

/*
 * Include Files
 */
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <dal/dal_mgmt.h>
#include <rtk/default.h>
#include <rtk/mirror.h>
#include <dal/dal_waMon.h>

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
int32
rtk_mirror_init(uint32 unit)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->mirror_init(unit);
} /* end of rtk_mirror_init */


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
int32
rtk_mirror_group_init(
    uint32              unit,
    rtk_mirror_entry_t  *pMirrorEntry)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->mirror_group_init(unit, pMirrorEntry);
} /* end of rtk_mirror_group_init */

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
int32
rtk_mirror_group_get(
    uint32              unit,
    uint32              mirror_id,
    rtk_mirror_entry_t  *pMirrorEntry)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->mirror_group_get(unit, mirror_id, pMirrorEntry);

} /* end of rtk_mirror_group_get */

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
int32
rtk_mirror_group_set(
    uint32              unit,
    uint32              mirror_id,
    rtk_mirror_entry_t  *pMirrorEntry)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->mirror_group_set(unit, mirror_id, pMirrorEntry);

} /* end of rtk_mirror_group_set */

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
int32
rtk_mirror_rspanIgrMode_get(uint32 unit, uint32 mirror_id, rtk_mirror_rspanIgrMode_t *pIgrMode)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->mirror_rspanIgrMode_get(unit, mirror_id, pIgrMode);
} /* end of rtk_mirror_rspanIgrMode_get */

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
int32
rtk_mirror_rspanIgrMode_set(uint32 unit, uint32 mirror_id, rtk_mirror_rspanIgrMode_t igrMode)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->mirror_rspanIgrMode_set(unit, mirror_id, igrMode);
} /* end of rtk_mirror_rspanIgrMode_set */

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
int32
rtk_mirror_rspanEgrMode_get(uint32 unit, uint32 mirror_id, rtk_mirror_rspanEgrMode_t *pEgrMode)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->mirror_rspanEgrMode_get(unit, mirror_id, pEgrMode);
} /* rtk_mirror_rspanEgrMode_get */

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
int32
rtk_mirror_rspanEgrMode_set(uint32 unit, uint32 mirror_id, rtk_mirror_rspanEgrMode_t egrMode)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->mirror_rspanEgrMode_set(unit, mirror_id, egrMode);
} /* rtk_mirror_rspanEgrMode_set */


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
int32
rtk_mirror_rspanTag_get(uint32 unit, uint32 mirror_id, rtk_mirror_rspanTag_t *pTag)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->mirror_rspanTag_get(unit, mirror_id, pTag);
} /* end of rtk_mirror_rspanTag_get */

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
int32
rtk_mirror_rspanTag_set(uint32 unit, uint32 mirror_id, rtk_mirror_rspanTag_t *pTag)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->mirror_rspanTag_set(unit, mirror_id, pTag);
} /* end of rtk_mirror_rspanTag_set */

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
int32
rtk_mirror_sflowMirrorSampleRate_get(uint32 unit, uint32 mirror_id, uint32 *pRate)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->mirror_sflowMirrorSampleRate_get(unit, mirror_id, pRate);
} /* end of rtk_mirror_sflowMirrorSampleRate_get */

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
int32
rtk_mirror_sflowMirrorSampleRate_set(uint32 unit, uint32 mirror_id, uint32 rate)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->mirror_sflowMirrorSampleRate_set(unit, mirror_id, rate);
} /* end of rtk_mirror_sflowMirrorSampleRate_set */

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
int32
rtk_mirror_egrQueue_get(uint32 unit, rtk_enable_t *pEnable, rtk_qid_t *pQid)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->mirror_egrQueue_get(unit, pEnable, pQid);
}   /* end of rtk_mirror_egrQueue_get */

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
rtk_mirror_egrQueue_set(uint32 unit, rtk_enable_t enable, rtk_qid_t qid)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->mirror_egrQueue_set(unit, enable, qid);
}   /* end of rtk_mirror_egrQueue_set */

/* Module Name    : Mirror           */
/* Sub-module Name: Port-based SFLOW */

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
int32
rtk_mirror_sflowPortIgrSampleRate_get(uint32 unit, rtk_port_t port, uint32 *pRate)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->mirror_sflowPortIgrSampleRate_get(unit, port, pRate);
} /* end of rtk_mirror_sflowPortIgrSampleRate_get */

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
int32
rtk_mirror_sflowPortIgrSampleRate_set(uint32 unit, rtk_port_t port, uint32 rate)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->mirror_sflowPortIgrSampleRate_set(unit, port, rate);
} /* end of rtk_mirror_sflowPortIgrSampleRate_set */

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
int32
rtk_mirror_sflowPortEgrSampleRate_get(uint32 unit, rtk_port_t port, uint32 *pRate)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->mirror_sflowPortEgrSampleRate_get(unit, port, pRate);
} /* end of rtk_mirror_sflowPortEgrSampleRate_get */

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
int32
rtk_mirror_sflowPortEgrSampleRate_set(uint32 unit, rtk_port_t port, uint32 rate)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->mirror_sflowPortEgrSampleRate_set(unit, port, rate);
} /* end of rtk_mirror_sflowPortEgrSampleRate_set */

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
int32
rtk_mirror_sflowSampleCtrl_get(uint32 unit, rtk_sflowSampleCtrl_t *pCtrl)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->mirror_sflowSampleCtrl_get(unit, pCtrl);
} /* end of rtk_mirror_sflowSampleCtrl_get */

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
int32
rtk_mirror_sflowSampleCtrl_set(uint32 unit, rtk_sflowSampleCtrl_t ctrl)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->mirror_sflowSampleCtrl_set(unit, ctrl);
} /* end of rtk_mirror_sflowSampleCtrl_set */

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
int32
rtk_mirror_sflowSampleTarget_get(uint32 unit, rtk_sflow_sampleTarget_t *pTarget)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->mirror_sflowSampleTarget_get(unit, pTarget);
}   /* end of rtk_mirror_sflowSampleTarget_get */

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
int32
rtk_mirror_sflowSampleTarget_set(uint32 unit, rtk_sflow_sampleTarget_t target)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->mirror_sflowSampleTarget_set(unit, target);
}   /* end of rtk_mirror_sflowSampleTarget_set */

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
int32
rtk_mirror_sflowStsChg_register(uint32 unit, sflowStsChg_cb_f stsChg_cb)
{
    return dal_waMon_pgMon_sflow_register(stsChg_cb);
}
#endif

