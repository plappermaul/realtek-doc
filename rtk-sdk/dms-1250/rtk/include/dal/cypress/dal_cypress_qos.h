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
 * Purpose : Definition those public QoS APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *            1) Ingress priority decision
 *            2) Egress remark
 *            3) Queue scheduling
 *
 */

#ifndef __DAL_CYPRESS_QOS_H__
#define __DAL_CYPRESS_QOS_H__


/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/qos.h>
#include <dal/dal_mapper.h>


/*
 * Symbol Definition
 */
#define QOS_SWRED_SYSTEM_DP0_DROP_RATE      0
#define QOS_SWRED_SYSTEM_DP1_DROP_RATE      (QOS_SWRED_SYSTEM_DP0_DROP_RATE)
#define QOS_SWRED_SYSTEM_DP2_DROP_RATE      (QOS_SWRED_SYSTEM_DP0_DROP_RATE)
#define QOS_SWRED_SYSTEM_DP0_HIGH_THRESH    4095
#define QOS_SWRED_SYSTEM_DP1_HIGH_THRESH    (QOS_SWRED_SYSTEM_DP0_HIGH_THRESH)
#define QOS_SWRED_SYSTEM_DP2_HIGH_THRESH    (QOS_SWRED_SYSTEM_DP0_HIGH_THRESH)
#define QOS_SWRED_SYSTEM_DP0_LOW_THRESH     4095
#define QOS_SWRED_SYSTEM_DP1_LOW_THRESH     (QOS_SWRED_SYSTEM_DP0_LOW_THRESH)
#define QOS_SWRED_SYSTEM_DP2_LOW_THRESH     (QOS_SWRED_SYSTEM_DP0_LOW_THRESH)

#define QOS_SWRED_QUEUE_DP0_DROP_RATE       255
#define QOS_SWRED_QUEUE_DP1_DROP_RATE       255
#define QOS_SWRED_QUEUE_DP2_DROP_RATE       255
#define QOS_SWRED_QUEUE_DP0_HIGH_THRESH     78
#define QOS_SWRED_QUEUE_DP1_HIGH_THRESH     74
#define QOS_SWRED_QUEUE_DP2_HIGH_THRESH     70
#define QOS_SWRED_QUEUE_DP0_LOW_THRESH      68
#define QOS_SWRED_QUEUE_DP1_LOW_THRESH      64
#define QOS_SWRED_QUEUE_DP2_LOW_THRESH      60

/*
 * Data Declaration
 */


/*
 * Function Declaration
 */

/* Function Name:
 *      dal_cypress_qosMapper_init
 * Description:
 *      Hook qos module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must Hook qos module before calling any qos APIs.
 */
extern int32
dal_cypress_qosMapper_init(dal_mapper_t *pMapper);

/* Function Name:
 *      dal_cypress_qos_init
 * Description:
 *      Configure QoS initial settings with queue number assigment to each port
 * Input:
 *      unit                - unit id
 *      queueNum            - Queue number of each port, it is available at 1~8
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_QUEUE_NUM     - Invalid queue number
 * Note:
 *      This API will initialize related QoS setting with queue number assignment.
 *      The initialization does the following actions:
 *      1. set input bandwidth control parameters to default values
 *      2. set priority decision parameters
 *      3. set scheduling parameters
 *      4. disable port remark ability
 *      5. set flow control thresholds
 */
extern int32
dal_cypress_qos_init(uint32 unit, uint32 queueNum);

/* Function Name:
 *      dal_cypress_qos_priSelGroup_get
 * Description:
 *      Get weight of each priority assignment on specified priority selection group.
 * Input:
 *      unit            - unit id
 *      grp_idx         - index of priority selection group
 * Output:
 *      pWeightOfPriSel - pointer to weight of each priority assignment
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
dal_cypress_qos_priSelGroup_get(uint32 unit, uint32 grp_idx, rtk_qos_priSelWeight_t *pWeightOfPriSel);

/* Function Name:
 *      dal_cypress_qos_priSelGroup_set
 * Description:
 *      Set weight of each priority assignment on specified priority selection group.
 * Input:
 *      unit            - unit id
 *      grp_idx         - index of priority selection group
 *      pWeightOfPriSel - weight of each priority assignment
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_qos_priSelGroup_set(uint32 unit, uint32 grp_idx, rtk_qos_priSelWeight_t *pWeightOfPriSel);

/* Function Name:
 *      dal_cypress_qos_portPriSelGroup_get
 * Description:
 *      Get priority selection group for specified port.
 * Input:
 *      unit            - unit id
 *      port            - port id
 * Output:
 *      pPriSelGrp_idx  - pointer to index of priority selection group
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
dal_cypress_qos_portPriSelGroup_get(uint32 unit, rtk_port_t port, uint32 *pPriSelGrp_idx);

/* Function Name:
 *      dal_cypress_qos_portPriSelGroup_set
 * Description:
 *      Set priority selection group for specified port.
 * Input:
 *      unit            - unit id
 *      port            - port id
 *      priSelGrp_idx   - index of priority selection group
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
dal_cypress_qos_portPriSelGroup_set(uint32 unit, rtk_port_t port, uint32 priSelGrp_idx);

/* Function Name:
 *      dal_cypress_qos_portPri_get
 * Description:
 *      Get internal priority of one port.
 * Input:
 *      unit                - unit id
 *      port                - port id
 * Output:
 *      *pInt_pri           - Priorities assigment for specific port. (range from 0 ~ 7, 7 is
 *                            the highest prioirty)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID               - Invalid port id
 *      RT_ERR_NULL_POINTER          - NULL pointer
 * Note:
 *    None.
 */
extern int32
dal_cypress_qos_portPri_get(uint32 unit, rtk_port_t port, rtk_pri_t *pInt_pri);

/* Function Name:
 *      dal_cypress_qos_portPri_set
 * Description:
 *      Set internal priority of one port.
 * Input:
 *      unit                - unit id
 *      port                - port id
 *      int_pri             - Priorities assigment for specific port. (range from 0 ~ 7, 7 is
 *                            the highest prioirty)
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID               - Invalid port id
 *      RT_ERR_QOS_INT_PRIORITY      - Invalid priority value
 * Note:
 *    This API can set port to 3 bits internal priority mapping.
 *    When a packet is received from a port, a port based priority will be assigned
 *    by the mapping setting.
 *    By default, the mapping priorities for all ports are 0.
 */
extern int32
dal_cypress_qos_portPri_set(uint32 unit, rtk_port_t port, rtk_pri_t int_pri);

/* Function Name:
 *      dal_cypress_qos_portPriRemapEnable_get
 * Description:
 *      Get status of port-based priority remapping.
 * Input:
 *      unit      - unit id
 * Output:
 *      pEnable   - status of port-based priority remapping
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *    None.
 */
extern int32
dal_cypress_qos_portPriRemapEnable_get(uint32 unit, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_cypress_qos_portPriRemapEnable_set
 * Description:
 *      Set status of port-based priority remapping.
 * Input:
 *      unit      - unit id
 *      pEnable   - status of port-based priority remapping
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
dal_cypress_qos_portPriRemapEnable_set(uint32 unit, rtk_enable_t enable);

/* Function Name:
 *      dal_cypress_qos_sysPortPriRemapSel_get
 * Description:
 *      Get port-based priority remapping table.
 * Input:
 *      unit    - unit id
 *      pType   - remapping table selection
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
extern int32
dal_cypress_qos_sysPortPriRemapSel_get(uint32 unit, rtk_qos_portPriRemapSel_t *pType);

/* Function Name:
 *      dal_cypress_qos_sysPortPriRemapSel_set
 * Description:
 *      Set port-based priority remapping table.
 * Input:
 *      unit    - unit id
 *      type    - remapping table selection
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
dal_cypress_qos_sysPortPriRemapSel_set(uint32 unit, rtk_qos_portPriRemapSel_t type);

/* Function Name:
 *      dal_cypress_qos_dscpPriRemap_get
 * Description:
 *      Get the internal priority that DSCP value remap.
 * Input:
 *      unit                - unit id
 *      dscp                - DSCP value of receiving frame (0~63)
 * Output:
 *      *pInt_pri           - Priorities assigment for specific port. (range from 0 ~ 7, 7 is
 *                            the highest prioirty)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_QOS_DSCP_VALUE        - Invalid DSCP value
 *      RT_ERR_NULL_POINTER          - NULL pointer
 * Note:
 *      None.
 */
extern int32
dal_cypress_qos_dscpPriRemap_get(uint32 unit, uint32 dscp, rtk_pri_t *pInt_pri);

/* Function Name:
 *      dal_cypress_qos_dpSrcSel_get
 * Description:
 *      Get drop precedence source.
 * Input:
 *      unit - unit id
 * Output:
 *      pType  - DP mapping source
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_qos_dpSrcSel_get(uint32 unit, rtk_qos_dpSrc_t *pType);

/* Function Name:
 *      dal_cypress_qos_dpSrcSel_set
 * Description:
 *      Set drop precedence source.
 * Input:
 *      unit - unit id
 *      port - port id
 *      pType   - DP mapping source
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
dal_cypress_qos_dpSrcSel_set(uint32 unit, rtk_qos_dpSrc_t type);

/* Function Name:
 *      dal_cypress_qos_portDEISrcSel_get
 * Description:
 *      Get DEI source of specified port.
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pType  - type of DEI
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
dal_cypress_qos_portDEISrcSel_get(uint32 unit, rtk_port_t port, rtk_qos_deiSel_t *pType);

/* Function Name:
 *      dal_cypress_qos_portDEISrcSel_set
 * Description:
 *      Set DEI source of specified port.
 * Input:
 *      unit - unit id
 *      port - port id
 *      type - type of DEI
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
dal_cypress_qos_portDEISrcSel_set(uint32 unit, rtk_port_t port, rtk_qos_deiSel_t type);

/* Function Name:
 *      dal_cypress_qos_deiDpRemap_get
 * Description:
 *      Get drop precedence of specified DEI.
 * Input:
 *      unit - unit id
 *      dei  - DEI
 * Output:
 *      pDp  - pointer to drop precedence
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
dal_cypress_qos_deiDpRemap_get(uint32 unit, uint32 dei, uint32 *pDp);

/* Function Name:
 *      dal_cypress_qos_deiDpRemap_set
 * Description:
 *      Set drop precedence to specified DEI.
 * Input:
 *      unit - unit id
 *      dei  - DEI
 *      dp   - drop precedence
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_DROP_PRECEDENCE  - invalid drop precedence
 * Note:
 *      None
 */
extern int32
dal_cypress_qos_deiDpRemap_set(uint32 unit, uint32 dei, uint32 dp);

/* Function Name:
 *      dal_cypress_qos_dscpDpRemap_get
 * Description:
 *      Get drop precedence of specified DSCP.
 * Input:
 *      unit - unit id
 *      dscp - DSCP value of receiving frame (0~63)
 * Output:
 *      pDp  - pointer to drop precedence
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_QOS_DSCP_VALUE   - Invalid DSCP value
 * Note:
 *      None
 */
extern int32
dal_cypress_qos_dscpDpRemap_get(uint32 unit, uint32 dscp, uint32 *pDp);

/* Function Name:
 *      dal_cypress_qos_dscpDpRemap_set
 * Description:
 *      Set drop precedence to specified DSCP.
 * Input:
 *      unit - unit id
 *      dscp - DSCP value of receiving frame (0~63)
 *      pDp  - drop precedence
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_QOS_DSCP_VALUE   - Invalid DSCP value
 *      RT_ERR_DROP_PRECEDENCE  - invalid drop precedence
 * Note:
 *      None
 */
extern int32
dal_cypress_qos_dscpDpRemap_set(uint32 unit, uint32 dscp, uint32 dp);

/* Function Name:
 *      dal_cypress_qos_dpRemap_get
 * Description:
 *      Get DEI/DSCP mapping to drop precedence.
 * Input:
 *      unit   - unit id
 *      src    - drop precedence remap source type
 *      srcVal - remap source value
 * Output:
 *      pDp    - pointer to drop precedence
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_QOS_DSCP_VALUE  - Invalid DSCP value
 *      RT_ERR_QOS_DEI_VALUE   - Invalid DEI/CFI value
 *      RT_ERR_QOS_1P_PRIORITY - Invalid priority value
 *      RT_ERR_NULL_POINTER   - input parameter may be null pointer
 * Note:
 *      The API can get configuration of DEI/DSCP to DP remapping table.
 */
extern int32
dal_cypress_qos_dpRemap_get(uint32 unit, rtk_qos_dpSrc_t src, rtk_qos_dpSrcRemap_t srcVal, uint32 *pDp);

/* Function Name:
 *      dal_cypress_qos_dpRemap_set
 * Description:
 *      Set DEI/DSCP mapping to drop precedence.
 * Input:
 *      unit   - unit id
 *      src    - drop precedence remap source type
 *      srcVal - remap source value
 *      dp     - drop precedence
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_QOS_DSCP_VALUE  - Invalid DSCP value
 *      RT_ERR_QOS_DEI_VALUE   - Invalid DEI/CFI value
 *      RT_ERR_DROP_PRECEDENCE - invalid drop precedence
 * Note:
 *      The API can configure DEI/DSCP to DP remapping table.
 */
extern int32
dal_cypress_qos_dpRemap_set(uint32 unit, rtk_qos_dpSrc_t src, rtk_qos_dpSrcRemap_t srcVal, uint32 dp);

/* Function Name:
 *      dal_cypress_qos_dscpPriRemap_set
 * Description:
 *      Set the internal priority that DSCP value remap.
 * Input:
 *      unit                - unit id
 *      dscp                - DSCP value of receiving frame (0~63)
 *      int_pri             - Priorities assigment for specific port. (range from 0 ~ 7, 7 is
 *                            the highest prioirty)
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_QOS_DSCP_VALUE        - Invalid DSCP value
 *      RT_ERR_QOS_INT_PRIORITY      - Invalid priority value
 * Note:
 *      The Differentiated Service Code Point is a selector for router's per-hop behaviours.
 *      As a selector, there is no implication that a numerically greater DSCP implies a better
 *      network service. As can be seen, the DSCP totally overlaps the old precedence field of TOS.
 *      So if values of DSCP are carefully chosen then backward compatibility can be achieved.
 */
extern int32
dal_cypress_qos_dscpPriRemap_set(uint32 unit, uint32 dscp, rtk_pri_t int_pri);

/* Function Name:
 *      dal_cypress_qos_1pPriRemap_get
 * Description:
 *      Get the internal priority that 802.1p priority remap.
 * Input:
 *      unit                    - unit id
 *      dot1p_pri               - 802.1p priority value (range from 0 ~ 7)
 * Output:
 *      *pInt_pri               - internal priority value (range from 0 ~ 7)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_QOS_1P_PRIORITY       - Invalid 802.1p priority
 *      RT_ERR_NULL_POINTER          - NULL pointer
 * Note:
 *      None.
 */
extern int32
dal_cypress_qos_1pPriRemap_get(uint32 unit, rtk_pri_t dot1p_pri, rtk_pri_t *pInt_pri);

/* Function Name:
 *      dal_cypress_qos_1pPriRemap_set
 * Description:
 *      Set the internal priority that 802.1p priority remap.
 * Input:
 *      unit                    - unit id
 *      dot1p_pri               - 802.1p priority value (range from 0 ~ 7)
 *      int_pri                 - internal priority value (range from 0 ~ 7)
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_QOS_1P_PRIORITY       - Invalid 802.1p priority
 *      RT_ERR_QOS_INT_PRIORITY      - Invalid priority value
 * Note:
 *      None.
 */
extern int32
dal_cypress_qos_1pPriRemap_set(uint32 unit, rtk_pri_t dot1p_pri, rtk_pri_t int_pri);

/* Function Name:
 *      dal_cypress_qos_outer1pPriRemap_get
 * Description:
 *      Get the internal priority that outer 802.1p priority remap.
 * Input:
 *      unit      - unit id
 *      dei       - DEI
 *      dot1p_pri - 802.1p priority value (range from 0 ~ 7)
 * Output:
 *      pInt_pri  - internal priority value (range from 0 ~ 7)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_QOS_1P_PRIORITY - Invalid 802.1p priority
 *      RT_ERR_NULL_POINTER    - NULL pointer
 *      RT_ERR_QOS_DEI_VALUE   - invalid dei
 * Note:
 *      None.
 */
extern int32
dal_cypress_qos_outer1pPriRemap_get(uint32 unit, uint32 dei, rtk_pri_t dot1p_pri, rtk_pri_t *pInt_pri);

/* Function Name:
 *      dal_cypress_qos_outer1pPriRemap_set
 * Description:
 *      Set the internal priority that outer 802.1p priority remap.
 * Input:
 *      unit      - unit id
 *      dei       - DEI
 *      dot1p_pri - 802.1p priority value (range from 0 ~ 7)
 *      int_pri   - internal priority value (range from 0 ~ 7)
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_QOS_1P_PRIORITY  - Invalid 802.1p priority
 *      RT_ERR_QOS_DEI_VALUE    - invalid dei
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority value
 * Note:
 *      None.
 */
extern int32
dal_cypress_qos_outer1pPriRemap_set(uint32 unit, uint32 dei, rtk_pri_t dot1p_pri, rtk_pri_t int_pri);

/* Function Name:
 *      dal_cypress_qos_priRemap_get
 * Description:
 *      Get specified priority source mapping to internal priority.
 * Input:
 *      unit   - unit id
 *      src    - priority remap source type
 *      srcVal - remap source value
 * Output:
 *      pPri   - pointer to internal priority
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID         - Invalid unit id
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_PORT_ID         - Invalid port id
 *      RT_ERR_QOS_DSCP_VALUE  - Invalid DSCP value
 *      RT_ERR_QOS_DEI_VALUE   - Invalid DEI/CFI value
 *      RT_ERR_QOS_1P_PRIORITY - Invalid priority value
 *      RT_ERR_NULL_POINTER    - Input parameter may be null pointer
 * Note:
 *      Supported rtk_qos_priSrc_t is as following:
 *      - PRI_SRC_INNER_USER_PRI
 *      - PRI_SRC_OUTER_USER_PRI
 *      - PRI_SRC_DSCP
 *      - PRI_SRC_PB_PRI
 */
extern int32
dal_cypress_qos_priRemap_get(uint32 unit, rtk_qos_priSrc_t src, rtk_qos_priSrcRemap_t srcVal, uint32 *pPri);

/* Function Name:
 *      dal_cypress_qos_priRemap_set
 * Description:
 *      Set specified priority source mapping to internal priority.
 * Input:
 *      unit   - unit id
 *      src    - priority remap source type
 *      srcVal - remap source value
 *      pri    - internal priority
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID         - Invalid unit id
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_PORT_ID         - Invalid port id
 *      RT_ERR_QOS_DSCP_VALUE  - Invalid DSCP value
 *      RT_ERR_QOS_DEI_VALUE   - Invalid DEI/CFI value
 *      RT_ERR_QOS_1P_PRIORITY - Invalid priority value
 *      RT_ERR_QOS_INT_PRIORITY- Invalid internal priority value
 * Note:
 *      Supported rtk_qos_priSrc_t is as following:
 *      - PRI_SRC_INNER_USER_PRI
 *      - PRI_SRC_OUTER_USER_PRI
 *      - PRI_SRC_DSCP
 *      - PRI_SRC_PB_PRI
 */
extern int32
dal_cypress_qos_priRemap_set(uint32 unit, rtk_qos_priSrc_t src, rtk_qos_priSrcRemap_t srcVal, uint32 pri);

/* Function Name:
 *      dal_cypress_qos_priRemapEnable_get
 * Description:
 *      Get priority remapping status
 * Input:
 *      unit    - unit id
 *      src     - priority remap type
 * Output:
 *      pEnable - port-base outer priority remapping status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Supported priority remap type rtk_qos_priSrc_t is as following:
 *      - PRI_SRC_PB_PRI
 */
extern int32
dal_cypress_qos_priRemapEnable_get(uint32 unit, rtk_qos_priSrc_t src, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_cypress_qos_priRemapEnable_set
 * Description:
 *      Set priority remapping status
 * Input:
 *      unit   - unit id
 *      src    - priority remap type
 *      enable - port-base inner priority remapping status
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      Supported priority remap type rtk_qos_priSrc_t is as following:
 *      - PRI_SRC_PB_PRI
 */
extern int32
dal_cypress_qos_priRemapEnable_set(uint32 unit, rtk_qos_priSrc_t src, rtk_enable_t enable);

/* Function Name:
 *      dal_cypress_qos_queueNum_get
 * Description:
 *      Get the number of queue for the system.
 * Input:
 *      unit                    - unit id
 * Output:
 *      *pQueue_num             - the number of queue (1~8).
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER      - NULL pointer
 * Note:
 *      None.
 */
extern int32
dal_cypress_qos_queueNum_get(uint32 unit, uint32 *pQueue_num);

/* Function Name:
 *      dal_cypress_qos_queueNum_set
 * Description:
 *      Set the number of queue for the system.
 * Input:
 *      unit                    - unit id
 *      queue_num               - the number of queue (1~8).
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_QUEUE_NUM             - Invalid queue number
 * Note:
 *      None.
 */
extern int32
dal_cypress_qos_queueNum_set(uint32 unit, uint32 queue_num);

/* Function Name:
 *      dal_cypress_qos_priMap_get
 * Description:
 *      Get the value of internal priority to QID mapping table.
 * Input:
 *      unit                    - unit id
 *      queue_num               - the number of queue (1~8).
 * Output:
 *     	*pPri2qid               - array of internal priority on a queue
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_QUEUE_NUM             - Invalid queue number
 *      RT_ERR_NULL_POINTER          - NULL pointer
 * Note:
 *      None.
 */
extern int32
dal_cypress_qos_priMap_get(uint32 unit, uint32 queue_num, rtk_qos_pri2queue_t *pPri2qid);

/* Function Name:
 *      dal_cypress_qos_priMap_set
 * Description:
 *      Set the value of internal priority to QID mapping table.
 * Input:
 *      unit                    - unit id
 *      queue_num               - the number of queue (1~8).
 *      *pPri2qid               - array of internal priority on a queue
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_QUEUE_NUM             - Invalid queue number
 * Note:
 *      Below is an example of internal priority to QID mapping table.
 *      When queue numbers are 8, the pri2qid are pri2qid[0]=0, pri2qid[1]=1, pri2qid[2]=2..., etc.
 *
 *                  Number of Available Output Queue
 *        Priority  1   2   3   4   5   6   7   8
 *              0   0   0   0   0   0   0   0   0
 *              1   0   0   0   0   0   0   0   1
 *              2   0   0   0   1   1   1   1   2
 *              3   0   0   0   1   1   2   2   3
 *              4   0   1   1   2   2   3   3   4
 *              5   0   1   1   2   3   4   4   5
 *              6   0   1   2   3   4   5   5   6
 *              7   0   1   2   3   4   5   6   7
 */
extern int32
dal_cypress_qos_priMap_set(uint32 unit, uint32 queue_num, rtk_qos_pri2queue_t *pPri2qid);

/* Function Name:
 *      dal_cypress_qos_pri2QidMap_get
 * Description:
 *      Get internal priority to normal port QID mapping value.
 * Input:
 *      unit      - unit id
 *      pri       - internal priority
 * Output:
 *      pQid      - queue id of normal port
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PRIORITY     - Invalid Priority value
 *      RT_ERR_NULL_POINTER - Input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_qos_pri2QidMap_get(uint32 unit, rtk_pri_t pri, rtk_qid_t *pQid);

/* Function Name:
 *      dal_cypress_qos_pri2QidMap_set
 * Description:
 *      Set internal priority to normal port QID mapping value.
 * Input:
 *      unit      - unit id
 *      pri       - internal priority
 *      qid       - queue id of normal port
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PRIORITY     - Invalid Priority value
 *      RT_ERR_QUEUE_ID     - Invalid queue ID
 * Note:
 *      None
 */
extern int32
dal_cypress_qos_pri2QidMap_set(uint32 unit, rtk_pri_t pri, rtk_qid_t qid);

/* Function Name:
 *      dal_cypress_qos_1pDfltPri_get
 * Description:
 *      Get default inner-priority value
 * Input:
 *      unit       - unit id
 * Output:
 *      pDot1p_pri - default 802.1p priority value (range from 0 ~ 7)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      None.
 */
extern int32
dal_cypress_qos_1pDfltPri_get(uint32 unit, rtk_pri_t *pDot1p_pri);

/* Function Name:
 *      dal_cypress_qos_1pDfltPri_set
 * Description:
 *      Set default inner-priority value
 * Input:
 *      unit      - unit id
 *      dot1p_pri - default 802.1p priority value (range from 0 ~ 7)
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_QOS_1P_PRIORITY       - Invalid dot1p priority
 * Note:
 *      None.
 */
extern int32
dal_cypress_qos_1pDfltPri_set(uint32 unit, rtk_pri_t dot1p_pri);

/* Function Name:
 *      dal_cypress_qos_1pDfltPriSrcSel_get
 * Description:
 *      Get default inner-priority source
 * Input:
 *      unit       - unit id
 * Output:
 *      pType      - default inner dot1p priority source
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER     - NULL pointer
 * Note:
 *      None.
 */
extern int32
dal_cypress_qos_1pDfltPriSrcSel_get(uint32 unit, rtk_qos_1pDfltPriSrc_t *pType);

/* Function Name:
 *      dal_cypress_qos_1pDfltPriSrcSel_set
 * Description:
 *      Set default inner-priority source
 * Input:
 *      unit      - unit id
 *      type      - default inner dot1p priority source
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None.
 */
extern int32
dal_cypress_qos_1pDfltPriSrcSel_set(uint32 unit, rtk_qos_1pDfltPriSrc_t type);

/* Function Name:
 *      dal_cypress_qos_port1pRemarkEnable_get
 * Description:
 *      Get 802.1p remark status for a port
 * Input:
 *      unit        - unit id
 *      port        - port id
 * Output:
 *      *pEnable    - status of 802.1p remark
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID               - Invalid port id
 *      RT_ERR_NULL_POINTER          - NULL pointer
 * Note:
 *    The status of 802.1p remark:
 *    - DISABLED
 *    - ENABLED
 */
extern int32
dal_cypress_qos_port1pRemarkEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_cypress_qos_port1pRemarkEnable_set
 * Description:
 *      Set 802.1p remark status for a port
 * Input:
 *      unit        - unit id
 *      port        - port id.
 *      enable      - status of 802.1p remark
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID   - Invalid port id
 *      RT_ERR_INPUT     - invalid input parameter
 * Note:
 *    The status of 802.1p remark:
 *    - DISABLED
 *    - ENABLED
 */
extern int32
dal_cypress_qos_port1pRemarkEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      dal_cypress_qos_1pRemarking_get
 * Description:
 *      Get the internal priority/original inner priority/original outer priority/dscp to
 *      remarkd 802.1p priority(3bits) mapping.
 * Input:
 *      unit    - unit id
 *      src     - remark source
 *      val     - remark source value
 * Output:
 *      pPri - remarked 802.1p priority value (range from 0 ~ 7)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - Invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_qos_1pRemarking_get(uint32 unit, rtk_qos_1pRmkSrc_t src, rtk_qos_1pRmkVal_t val, rtk_pri_t *pPri);

/* Function Name:
 *      dal_cypress_qos_1pRemarking_set
 * Description:
 *      Set the internal priority/original inner priority/original outer priority/dscp to
 *      remarkd 802.1p priority(3bits) mapping.
 * Input:
 *      unit    - unit id
 *      src     - remark source
 *      val     - remark source value
 *      pri     - remarked 802.1p priority value (range from 0 ~ 7)
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - Invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority
 *      RT_ERR_QOS_1P_PRIORITY  - Invalid dot1p priority
 * Note:
 *      None
 */
extern int32
dal_cypress_qos_1pRemarking_set(uint32 unit, rtk_qos_1pRmkSrc_t src, rtk_qos_1pRmkVal_t val, rtk_pri_t pri);

/* Function Name:
 *      dal_cypress_qos_1pRemark_get
 * Description:
 *      Get the internal priority (3bits) to remarkd 802.1p priority(3bits) mapping.
 * Input:
 *      unit                - unit id
 *      int_pri             - internal priority value (range from 0 ~ 7)
 * Output:
 *      *pDot1p_pri         - remarked 802.1p priority value (range from 0 ~ 7)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_QOS_INT_PRIORITY      - Invalid priority
 *      RT_ERR_NULL_POINTER          - NULL pointer
 * Note:
 *      None.
 */
extern int32
dal_cypress_qos_1pRemark_get(uint32 unit, rtk_pri_t int_pri, rtk_pri_t *pDot1p_pri);

/* Function Name:
 *      dal_cypress_qos_1pRemark_set
 * Description:
 *      Set the internal priority(3bits) to remarked 802.1p priority(3bits) mapping.
 * Input:
 *      unit                - unit id
 *      int_pri             - internal priority value (range from 0 ~ 7)
 *      dot1p_pri           - remarked 802.1p priority value (range from 0 ~ 7)
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_QOS_1P_PRIORITY       - Invalid dot1p priority
 *      RT_ERR_QOS_INT_PRIORITY      - Invalid priority
 * Note:
 *      802.1p remark functionality can map the internal priority to 802.1p priority before a packet
 *      is going to be transmited.
 */
extern int32
dal_cypress_qos_1pRemark_set(uint32 unit, rtk_pri_t int_pri, rtk_pri_t dot1p_pri);

/* Function Name:
 *      dal_cypress_qos_1pRemarkSrcSel_get
 * Description:
 *      Get the remarking source of 802.1p priority(3bits) remarking.
 * Input:
 *      unit    - unit id
 * Output:
 *      pType   - remarking source
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER     - NULL pointer
 * Note:
 *      None.
 */
extern int32
dal_cypress_qos_1pRemarkSrcSel_get(uint32 unit, rtk_qos_1pRmkSrc_t *pType);

/* Function Name:
 *      dal_cypress_qos_1pRemarkSrcSel_set
 * Description:
 *      Set the remarking source of 802.1p priority(3bits) remarking.
 * Input:
 *      unit    - unit id
 *      type    - remarking source
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None.
 */
extern int32
dal_cypress_qos_1pRemarkSrcSel_set(uint32 unit, rtk_qos_1pRmkSrc_t type);

/* Function Name:
 *      dal_cypress_qos_portOut1pRemarkEnable_get
 * Description:
 *      Get enable status of outer dot1p remarking on specified port
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to enable status of outer dot1p remarking
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *    The status of 802.1p remark:
 *    - DISABLED
 *    - ENABLED
 */
extern int32
dal_cypress_qos_portOut1pRemarkEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_cypress_qos_portOut1pRemarkEnable_set
 * Description:
 *      Set enable status of outer dot1p remarking on specified port
 * Input:
 *      unit   - unit id
 *      port   - port id.
 *      enable - enable status of outer dot1p remarking
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - Invalid port id
 *      RT_ERR_INPUT   - invalid input parameter
 * Note:
 *    The status of 802.1p remark:
 *    - DISABLED
 *    - ENABLED
 */
extern int32
dal_cypress_qos_portOut1pRemarkEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      dal_cypress_qos_outer1pRemark_get
 * Description:
 *      Get the internal priority (3bits) to remarkd outer dot1p priority(3bits) mapping.
 * Input:
 *      unit       - unit id
 *      int_pri    - internal priority value (range from 0 ~ 7)
 * Output:
 *      pDot1p_pri - remarked outer dot1p priority value (range from 0 ~ 7)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority
 *      RT_ERR_NULL_POINTER     - NULL pointer
 * Note:
 *      None.
 */
extern int32
dal_cypress_qos_outer1pRemark_get(uint32 unit, rtk_pri_t int_pri, rtk_pri_t *pDot1p_pri);

/* Function Name:
 *      dal_cypress_qos_outer1pRemark_set
 * Description:
 *      Set the internal priority(3bits) to remarked outer dot1p priority(3bits) mapping.
 * Input:
 *      unit      - unit id
 *      int_pri   - internal priority value (range from 0 ~ 7)
 *      dot1p_pri - remarked 802.1p priority value (range from 0 ~ 7)
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority
 *      RT_ERR_QOS_1P_PRIORITY  - Invalid dot1p priority
 * Note:
 *      Outer dot1p remark functionality can map the internal priority or original tag priority
 *      to outer dot1p priority before a packet is going to be transmited.
 */
extern int32
dal_cypress_qos_outer1pRemark_set(uint32 unit, rtk_pri_t int_pri, rtk_pri_t dot1p_pri);

/* Function Name:
 *      dal_cypress_qos_outer1pRemarking_get
 * Description:
 *      Get the original outer priority/dscp to remarkd outer 802.1p priority(3bits) mapping.
 * Input:
 *      unit    - unit id
 *      src     - remark source
 *      val     - remark source value
 * Output:
 *      dot1p_pri - remarked outer 802.1p priority value (range from 0 ~ 7)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - Invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      Supported remarking source is as following:
 *      - rtk_qos_outer1pRmkSrc_t
 *      - OUTER_1P_RMK_SRC_INT_PRI
 *      - OUTER_1P_RMK_SRC_USER_PRI
 *      - OUTER_1P_RMK_SRC_INNER_USER_PRI
 *      - OUTER_1P_RMK_SRC_DSCP
 */
extern int32
dal_cypress_qos_outer1pRemarking_get(uint32 unit, rtk_qos_outer1pRmkSrc_t src, rtk_qos_outer1pRmkVal_t val, rtk_pri_t *pPri);

/* Function Name:
 *      dal_cypress_qos_outer1pRemarking_set
 * Description:
 *      Set the original outer priority/dscp to remarkd outer 802.1p priority(3bits) mapping.
 * Input:
 *      unit    - unit id
 *      src     - remark source
 *      val     - remark source value
 *      pri     - remarked outer 802.1p priority value (range from 0 ~ 7)
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - Invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority
 *      RT_ERR_QOS_1P_PRIORITY  - Invalid dot1p priority
 * Note:
 *      Supported remarking source is as following:
 *      - rtk_qos_outer1pRmkSrc_t
 *      - OUTER_1P_RMK_SRC_INT_PRI
 *      - OUTER_1P_RMK_SRC_USER_PRI
 *      - OUTER_1P_RMK_SRC_INNER_USER_PRI
 *      - OUTER_1P_RMK_SRC_DSCP
 */
extern int32
dal_cypress_qos_outer1pRemarking_set(uint32 unit, rtk_qos_outer1pRmkSrc_t src, rtk_qos_outer1pRmkVal_t val, rtk_pri_t pri);

/* Function Name:
 *      dal_cypress_qos_outer1pRemarkSrcSel_get
 * Description:
 *      Get the remarking source of outer dot1p priority remarking.
 * Input:
 *      unit    - unit id
 * Output:
 *      pType   - remarking source
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER     - NULL pointer
 * Note:
 *      None.
 */
extern int32
dal_cypress_qos_outer1pRemarkSrcSel_get(uint32 unit, rtk_qos_outer1pRmkSrc_t *pType);

/* Function Name:
 *      dal_cypress_qos_outer1pRemarkSrcSel_set
 * Description:
 *      Set the remarking source of outer dot1p priority remarking.
 * Input:
 *      unit    - unit id
 *      type    - remarking source
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None.
 */
extern int32
dal_cypress_qos_outer1pRemarkSrcSel_set(uint32 unit, rtk_qos_outer1pRmkSrc_t type);

/* Function Name:
 *      dal_cypress_qos_outer1pDfltPri_get
 * Description:
 *      Get default outer-priority value
 * Input:
 *      unit       - unit id
 * Output:
 *      pDot1p_pri - default 802.1p priority value (range from 0 ~ 7)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None.
 */
extern int32
dal_cypress_qos_outer1pDfltPri_get(uint32 unit, rtk_pri_t *pDot1p_pri);

/* Function Name:
 *      dal_cypress_qos_outer1pDfltPri_set
 * Description:
 *      Set default outer priority value
 * Input:
 *      unit      - unit id
 *      dot1p_pri - default 802.1p priority value (range from 0 ~ 7)
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_QOS_1P_PRIORITY - Invalid dot1p priority
 * Note:
 *      None.
 */
extern int32
dal_cypress_qos_outer1pDfltPri_set(uint32 unit, rtk_pri_t dot1p_pri);

/* Function Name:
 *      dal_cypress_qos_outer1pDfltPriCfgSrcSel_get
 * Description:
 *      Get default outer-priority configured source
 * Input:
 *      unit       - unit id
 * Output:
 *      pDflt_sel  - default selection
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None.
 */
extern int32
dal_cypress_qos_outer1pDfltPriCfgSrcSel_get(uint32 unit, rtk_qos_outer1pDfltCfgSrc_t *pDflt_sel);

/* Function Name:
 *      dal_cypress_qos_outer1pDfltPriCfgSrcSel_set
 * Description:
 *      Set default outer-priority configured source
 * Input:
 *      unit      - unit id
 *      dflt_sel  - default selection
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_INPUT           - invalid input parameter
 * Note:
 *      None.
 */
extern int32
dal_cypress_qos_outer1pDfltPriCfgSrcSel_set(uint32 unit, rtk_qos_outer1pDfltCfgSrc_t dflt_sel);

/* Function Name:
 *      dal_cypress_qos_portOuter1pDfltPriSrcSel_get
 * Description:
 *      Get default outer-priority source of specified port
 * Input:
 *      unit       - unit id
 *      port       - port id
 * Output:
 *      pType      - default outer dot1p priority source
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_NULL_POINTER     - NULL pointer
 * Note:
 *      None.
 */extern int32
dal_cypress_qos_portOuter1pDfltPriSrcSel_get(uint32 unit, rtk_port_t port, rtk_qos_outer1pDfltSrc_t *pType);

/* Function Name:
 *      dal_cypress_qos_portOuter1pDfltPriSrcSel_set
 * Description:
 *      Set default outer-priority source of specified port
 * Input:
 *      unit      - unit id
 *      port      - port id
 *      type      - default outer dot1p priority source
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None.
 */
extern int32
dal_cypress_qos_portOuter1pDfltPriSrcSel_set(uint32 unit, rtk_port_t port, rtk_qos_outer1pDfltSrc_t type);

/* Function Name:
 *      dal_cypress_qos_portDscpRemarkEnable_get
 * Description:
 *      Get DSCP remark status for a port
 * Input:
 *      unit                - unit id
 *      port                - port id
 * Output:
 *      *pEnable            - status of DSCP remark
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID       - Invalid port id
 *      RT_ERR_NULL_POINTER  - NULL pointer
 * Note:
 *    The status of DSCP remark:
 *    - DISABLED
 *    - ENABLED
 */
extern int32
dal_cypress_qos_portDscpRemarkEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_cypress_qos_portDscpRemarkEnable_set
 * Description:
 *      Set DSCP remark status for a port
 * Input:
 *      unit            - unit id
 *      port            - port id
 *      enable          - status of DSCP remark
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID   - Invalid port id
 *      RT_ERR_INPUT     - invalid input parameter
 * Note:
 *    The status of DSCP remark:
 *    - DISABLED
 *    - ENABLED
 */
extern int32
dal_cypress_qos_portDscpRemarkEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      dal_cypress_qos_dscpRemarking_get
 * Description:
 *      Get the internal priority/original inner priority/original outer priority/dscp to
 *      remarkd DSCP mapping.
 * Input:
 *      unit    - unit id
 *      src     - remark source
 *      val     - remark source value
 * Output:
 *      pDscp    - remarked DSCP value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - Invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_qos_dscpRemarking_get(uint32 unit, rtk_qos_dscpRmkSrc_t src, rtk_qos_dscpRmkVal_t val, uint32 *pDscp);

/* Function Name:
 *      dal_cypress_qos_dscpRemarking_set
 * Description:
 *      Set the internal priority/original inner priority/original outer priority/dscp to
 *      remarkd DSCP mapping.
 * Input:
 *      unit    - unit id
 *      src     - remark source
 *      val     - remark source value
 *      dscp    - dscp value
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - Invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority
 *      RT_ERR_QOS_1P_PRIORITY  - Invalid dot1p priority
 * Note:
 *      None
 */
extern int32
dal_cypress_qos_dscpRemarking_set(uint32 unit, rtk_qos_dscpRmkSrc_t src, rtk_qos_dscpRmkVal_t val, uint32 dscp);

/* Function Name:
 *      dal_cypress_qos_dscpRemark_get
 * Description:
 *      Get the internal priority (3bits) to remarked DSCP mapping.
 * Input:
 *      unit                - unit id
 *      int_pri             - internal priority value (range from 0 ~ 7)
 * Output:
 *      *pDscp              - remarked DSCP value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_QOS_INT_PRIORITY      - Invalid priority
 *      RT_ERR_NULL_POINTER          - NULL pointer
 * Note:
 *      None.
 */
extern int32
dal_cypress_qos_dscpRemark_get(uint32 unit, rtk_pri_t int_pri, uint32 *pDscp);

/* Function Name:
 *      dal_cypress_qos_dscpRemark_set
 * Description:
 *      Set the internal priority (3bits) to remarked DSCP mapping.
 * Input:
 *      unit                - unit id
 *      int_pri             - internal priority value (range from 0 ~ 7)
 *      dscp                - remarked DSCP value
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_QOS_INT_PRIORITY      - Invalid priority
 *      RT_ERR_QOS_DSCP_VALUE        - Invalid dscp value
 * Note:
 *      DSCP remark functionality can map the internal priority to DSCP before a packet is going
 *      to be transmited.
 */
extern int32
dal_cypress_qos_dscpRemark_set(uint32 unit, rtk_pri_t int_pri, uint32 dscp);

/* Function Name:
 *      dal_cypress_qos_dscp2Dot1pRemark_get
 * Description:
 *      Get DSCP to remarked 802.1p priority(3bits) mapping.
 * Input:
 *      unit    - unit id
 *      dscp    - DSCP value
 * Output:
 *      pDot1p_pri   - remarked 802.1p priority value (range from 0 ~ 7)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_QOS_DSCP_VALUE   - Invalid dscp value
 *      RT_ERR_NULL_POINTER     - NULL pointer
 * Note:
 *      None.
 */
extern int32
dal_cypress_qos_dscp2Dot1pRemark_get(uint32 unit, uint32 dscp, rtk_pri_t *pDot1p_pri);

/* Function Name:
 *      dal_cypress_qos_dscp2Dot1pRemark_set
 * Description:
 *      Set DSCP to remarked 802.1p priority(3bits) mapping.
 * Input:
 *      unit      - unit id
 *      dscp      - DSCP value
 *      dot1p_pri - remarked 802.1p priority value (range from 0 ~ 7)
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_QOS_DSCP_VALUE   - Invalid dscp value
 *      RT_ERR_QOS_1P_PRIORITY  - Invalid dot1p priority
 * Note:
 *      None.
 */
extern int32
dal_cypress_qos_dscp2Dot1pRemark_set(uint32 unit, uint32 dscp, rtk_pri_t dot1p_pri);

/* Function Name:
 *      dal_cypress_qos_dscp2Outer1pRemark_get
 * Description:
 *      Get DSCP to remarked outer dot1p priority(3bits) mapping.
 * Input:
 *      unit    - unit id
 *      dscp    - DSCP value
 * Output:
 *      pDot1p_pri   - remarked outer dot1p priority value (range from 0 ~ 7)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_QOS_DSCP_VALUE   - Invalid dscp value
 *      RT_ERR_NULL_POINTER     - NULL pointer
 * Note:
 *      None.
 */
extern int32
dal_cypress_qos_dscp2Outer1pRemark_get(uint32 unit, uint32 dscp, rtk_pri_t *pDot1p_pri);

/* Function Name:
 *      dal_cypress_qos_dscp2Outer1pRemark_set
 * Description:
 *      Set DSCP to remarked outer dot1p priority(3bits) mapping.
 * Input:
 *      unit      - unit id
 *      dscp      - DSCP value
 *      dot1p_pri - remarked outer dot1p priority value (range from 0 ~ 7)
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_QOS_DSCP_VALUE   - Invalid dscp value
 *      RT_ERR_QOS_1P_PRIORITY  - Invalid dot1p priority
 * Note:
 *      None.
 */
extern int32
dal_cypress_qos_dscp2Outer1pRemark_set(uint32 unit, uint32 dscp, rtk_pri_t dot1p_pri);

/* Function Name:
 *      dal_cypress_qos_dscp2DscpRemark_get
 * Description:
 *      Get DSCP to remarked DSCP mapping.
 * Input:
 *      unit    - unit id
 *      dscp    - DSCP value
 * Output:
 *      pDscp   - remarked DSCP value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_QOS_DSCP_VALUE   - Invalid dscp value
 *      RT_ERR_NULL_POINTER     - NULL pointer
 * Note:
 *      None.
 */
extern int32
dal_cypress_qos_dscp2DscpRemark_get(uint32 unit, uint32 dscp, uint32 *pDscp);

/* Function Name:
 *      dal_cypress_qos_dscp2DscpRemark_set
 * Description:
 *      Set DSCP to remarked DSCP mapping.
 * Input:
 *      unit    - unit id
 *      dscp    - DSCP value
 *      rmkDscp - remarked DSCP value
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_QOS_DSCP_VALUE   - Invalid dscp value
 * Note:
 *      None.
 */
extern int32
dal_cypress_qos_dscp2DscpRemark_set(uint32 unit, uint32 dscp, uint32 rmkDscp);

/* Function Name:
 *      dal_cypress_qos_dscpRemarkSrcSel_get
 * Description:
 *      Get the remarking source of DSCP remarking.
 * Input:
 *      unit    - unit id
 * Output:
 *      pType   - remarking source
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER     - NULL pointer
 * Note:
 *      None.
 */
extern int32
dal_cypress_qos_dscpRemarkSrcSel_get(uint32 unit, rtk_qos_dscpRmkSrc_t *pType);

/* Function Name:
 *      dal_cypress_qos_dscpRemarkSrcSel_set
 * Description:
 *      Set the remarking source of DSCP remarking.
 * Input:
 *      unit    - unit id
 *      type    - remarking source
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None.
 */
extern int32
dal_cypress_qos_dscpRemarkSrcSel_set(uint32 unit, rtk_qos_dscpRmkSrc_t type);

/* Function Name:
 *      dal_cypress_qos_deiRemark_get
 * Description:
 *      Get the internal drop precedence to remarked DEI mapping.
 * Input:
 *      unit       - unit id
 *      int_pri    - internal drop precedence (range from 0 ~ 2)
 * Output:
 *      pDei       - remarked DEI value (range from 0 ~ 1)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_QOS_DROP_PRECEDENCE  - Invalid drop precedence
 *      RT_ERR_NULL_POINTER         - NULL pointer
 * Note:
 *      None.
 */
extern int32
dal_cypress_qos_deiRemark_get(uint32 unit, uint32 dp, uint32 *pDei);

/* Function Name:
 *      dal_cypress_qos_deiRemark_set
 * Description:
 *      Set the internal drop precedence to remarked DEI mapping.
 * Input:
 *      unit      - unit id
 *      dp        - internal drop precedence (range from 0 ~ 2)
 *      dei       - remarked DEI value (range from 0 ~ 1)
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_QOS_DEI_VALUE        - Invalid DEI value
 *      RT_ERR_QOS_DROP_PRECEDENCE  - Invalid drop precedence
 * Note:
 *      DEI remark functionality can map the internal drop precedence
 *      to DEI in outer tag or inner-tag before a packet is going to be transmited.
 */
extern int32
dal_cypress_qos_deiRemark_set(uint32 unit, uint32 dp, uint32 dei);

/* Function Name:
 *      dal_cypress_qos_deiRemarking_get
 * Description:
 *      Get the dp to DEI remarking mapping.
 * Input:
 *      unit    - unit id
 *      src     - remark source
 *      val     - remark source value
 * Output:
 *      pDei    - remarked DEI value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - Invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_qos_deiRemarking_get(uint32 unit, rtk_qos_deiRmkSrc_t src, rtk_qos_deiRmkVal_t val, uint32 *pDei);

/* Function Name:
 *      dal_cypress_qos_deiRemarking_set
 * Description:
 *      Set the dp to DEI remarking mapping.
 * Input:
 *      unit    - unit id
 *      src     - remark source
 *      val     - remark source value
 *      dei     - remarked DEI value
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - Invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority
 *      RT_ERR_QOS_1P_PRIORITY  - Invalid dot1p priority
 * Note:
 *      None
 */
extern int32
dal_cypress_qos_deiRemarking_set(uint32 unit, rtk_qos_deiRmkSrc_t src, rtk_qos_deiRmkVal_t val, uint32 dei);

/* Function Name:
 *      dal_cypress_qos_portDeiRemarkEnable_get
 * Description:
 *      Get DEI remark status for a port
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - status of DEI remark
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *    The status of DEI remark:
 *    - DISABLED
 *    - ENABLED
 */
extern int32
dal_cypress_qos_portDeiRemarkEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_cypress_qos_portDeiRemarkEnable_set
 * Description:
 *      Set DEI remark status for a port
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - status of DEI remark
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - Invalid port id
 *      RT_ERR_INPUT   - invalid input parameter
 * Note:
 *    The status of DEI remark:
 *    - DISABLED
 *    - ENABLED
 */
extern int32
dal_cypress_qos_portDeiRemarkEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      dal_cypress_qos_portDeiRemarkTagSel_get
 * Description:
 *      Get DEI remarking tag selection of specified port.
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pType  - type of DEI
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
dal_cypress_qos_portDeiRemarkTagSel_get(uint32 unit, rtk_port_t port, rtk_qos_deiSel_t *pType);

/* Function Name:
 *      dal_cypress_qos_portDeiRemarkTagSel_set
 * Description:
 *      Set DEI remarking tag selection of specified port.
 * Input:
 *      unit - unit id
 *      port - port id
 *      type - type of DEI
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
dal_cypress_qos_portDeiRemarkTagSel_set(uint32 unit, rtk_port_t port, rtk_qos_deiSel_t type);

/* Function Name:
 *      dal_cypress_qos_schedulingAlgorithm_get
 * Description:
 *      Get the scheduling algorithm of the port.
 * Input:
 *      unit                - unit id
 *      port                - port id
 * Output:
 *      *pScheduling_type   - type of scheduling algorithm.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID       - Invalid port id
 *      RT_ERR_NULL_POINTER  - NULL pointer
 * Note:
 *    The types of scheduling algorithm:
 *    - WFQ
 *    - WRR
 */
extern int32
dal_cypress_qos_schedulingAlgorithm_get(
    uint32                      unit,
    rtk_port_t                  port,
    rtk_qos_scheduling_type_t   *pScheduling_type);

/* Function Name:
 *      dal_cypress_qos_schedulingAlgorithm_set
 * Description:
 *      Set the scheduling algorithm of the port.
 * Input:
 *      unit            - unit id
 *      port            - port id
 *      scheduling_type - type of scheduling algorithm.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID       - Invalid port id
 *      RT_ERR_QOS_SCHE_TYPE - Error scheduling algorithm type
 * Note:
 *    The types of scheduling algorithm:
 *    - WFQ
 *    - WRR
 */
extern int32
dal_cypress_qos_schedulingAlgorithm_set(
    uint32                      unit,
    rtk_port_t                  port,
    rtk_qos_scheduling_type_t   scheduling_type);

/* Function Name:
 *      dal_cypress_qos_schedulingQueue_get
 * Description:
 *      Get the scheduling types and weights of queues on specific port in egress scheduling.
 * Input:
 *      unit        - unit id
 *      port        - port id
 * Output:
 *      *pQweights  - the array of weights for WRR/WFQ queue (valid:1~128, 0 for STRICT_PRIORITY queue)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID           - Invalid port id
 *      RT_ERR_NULL_POINTER      - NULL pointer
 * Note:
 *    The types of queue are: WFQ_WRR_PRIORITY or STRICT_PRIORITY.
 *    If the weight is 0 then the type is STRICT_PRIORITY, else the type is WFQ_WRR_PRIORITY.
 */
extern int32
dal_cypress_qos_schedulingQueue_get(uint32 unit, rtk_port_t port, rtk_qos_queue_weights_t *pQweights);

/* Function Name:
 *      dal_cypress_qos_schedulingQueue_set
 * Description:
 *      Set the scheduling types and weights of queues on specific port in egress scheduling.
 * Input:
 *      unit        - unit id
 *      port        - port id
 *      *pQweights  - the array of weights for WRR/WFQ queue (valid:1~128, 0 for STRICT_PRIORITY queue)
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID           - Invalid port id
 *      RT_ERR_NULL_POINTER      - Null pointer
 *      RT_ERR_QOS_QUEUE_WEIGHT	- Invalid queue weight
 * Note:
 *    The types of queue are: WFQ_WRR_PRIORITY or STRICT_PRIORITY.
 *    If the weight is 0 then the type is STRICT_PRIORITY, else the type is WFQ_WRR_PRIORITY.
 */
extern int32
dal_cypress_qos_schedulingQueue_set(uint32 unit, rtk_port_t port, rtk_qos_queue_weights_t *pQweights);

/* Function Name:
 *      dal_cypress_qos_portAvbStreamReservationClassEnable_get
 * Description:
 *      Destroy one specified mstp instance from the specified device.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      srClass - stream class
 * Output:
 *      pEnable - status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_PORT                 - invalid port id
 *      RT_ERR_AVB_INVALID_SR_CLASS - Invalid SR Class
 *      RT_ERR_INPUT                - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_cypress_qos_portAvbStreamReservationClassEnable_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_qos_avbSrClass_t    srClass,
    rtk_enable_t            *pEnable);

/* Function Name:
 *      dal_cypress_qos_portAvbStreamReservationClassEnable_set
 * Description:
 *      Set status of the specified stream class of the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      srClass - stream class
 *      enable  - status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT     - invalid port id
 *      RT_ERR_AVB_INVALID_SR_CLASS - Invalid SR Class
 *      RT_ERR_INPUT                - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_cypress_qos_portAvbStreamReservationClassEnable_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_qos_avbSrClass_t    srClass,
    rtk_enable_t            enable);


/* Function Name:
 *      dal_cypress_qos_avbStreamReservationConfig_get
 * Description:
 *      Get the configuration of Stream Reservation in the specified device.
 * Input:
 *      unit    - unit id
 * Output:
 *      pSrConf - pointer buffer of configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_qos_avbStreamReservationConfig_get(uint32 unit, rtk_qos_avbSrConf_t *pSrConf);

/* Function Name:
 *      dal_cypress_qos_avbStreamReservationConfig_set
 * Description:
 *      Set the configuration of Stream Reservation in the specified device.
 * Input:
 *      unit   - unit id
 *      pSrConf - pointer buffer of configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_QOS_1P_PRIORITY  - Invalid dot1p priority
 *      RT_ERR_QUEUE_ID         - invalid queue id
 * Note:
 *      None
 */
extern int32
dal_cypress_qos_avbStreamReservationConfig_set(uint32 unit, rtk_qos_avbSrConf_t *pSrConf);

/* Function Name:
 *      dal_cypress_qos_pkt2CpuPriRemap_get
 * Description:
 *      Get the value of new priority to remapping the original internal priority for the packets that normal forwarded to CPU.
 * Input:
 *      unit    - unit id
 *      intPri  - original internal
 * Output:
 *      pNewPri - new priority
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority
 *      RT_ERR_NULL_POINTER     - NULL pointer
 * Note:
 */
extern int32
dal_cypress_qos_pkt2CpuPriRemap_get(uint32 unit, rtk_pri_t intPri, rtk_pri_t *pNewPri);

/* Function Name:
 *      dal_cypress_qos_pkt2CpuPriRemap_set
 * Description:
 *      Set the value of new priority to remapping the original internal priority for the packets that normal forwarded to CPU.
 * Input:
 *      unit   - unit id
 *      intPri - original internal
 *      newPri - new priority
  * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority
 * Note:
 */
extern int32
dal_cypress_qos_pkt2CpuPriRemap_set(uint32 unit, rtk_pri_t intPri, rtk_pri_t newPri);


/* Module Name    : QoS              */
/* Sub-module Name: Congestion avoidance */

/* Function Name:
 *      dal_cypress_qos_congAvoidAlgo_get
 * Description:
 *      Get algorithm of congestion avoidance.
 * Input:
 *      unit  - unit id
 * Output:
 *      pAlgo - pointer to algorithm of congestion avoidance
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      Algorithm is as following:
 *      - CONG_AVOID_SWRED
 *      - CONG_AVOID_TD
 */
extern int32
dal_cypress_qos_congAvoidAlgo_get(uint32 unit, rtk_qos_congAvoidAlgo_t *pAlgo);

/* Function Name:
 *      dal_cypress_qos_congAvoidAlgo_set
 * Description:
 *      Set algorithm of congestion avoidance.
 * Input:
 *      unit - unit id
 *      algo - algorithm of congestion avoidance
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      Algorithm is as following:
 *      - CONG_AVOID_SWRED
 *      - CONG_AVOID_TD
 */
extern int32
dal_cypress_qos_congAvoidAlgo_set(uint32 unit, rtk_qos_congAvoidAlgo_t algo);

/* Function Name:
 *      dal_cypress_qos_congAvoidSysThresh_get
 * Description:
 *      Get system threshold of congestion avoidance.
 * Input:
 *      unit             - unit id
 *      dp               - drop precedence
 * Output:
 *      pCongAvoidThresh - pointer to system threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_DROP_PRECEDENCE  - invalid drop precedence
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_qos_congAvoidSysThresh_get(
    uint32                      unit,
    uint32                      dp,
    rtk_qos_congAvoidThresh_t   *pCongAvoidThresh);

/* Function Name:
 *      dal_cypress_qos_congAvoidSysThresh_set
 * Description:
 *      Set system threshold of congestion avoidance.
 * Input:
 *      unit             - unit id
 *      dp               - drop precedence
 *      pCongAvoidThresh - system threshold
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_DROP_PRECEDENCE  - invalid drop precedence
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_cypress_qos_congAvoidSysThresh_set(
    uint32                      unit,
    uint32                      dp,
    rtk_qos_congAvoidThresh_t   *pCongAvoidThresh);

/* Function Name:
 *      dal_cypress_qos_congAvoidSysDropProbability_get
 * Description:
 *      Get system threshold of congestion avoidance.
 * Input:
 *      unit             - unit id
 *      dp               - drop precedence
 * Output:
 *      pProbability     - pointer to drop probability
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_DROP_PRECEDENCE  - invalid drop precedence
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_qos_congAvoidSysDropProbability_get(
    uint32  unit,
    uint32  dp,
    uint32  *pProbability);

/* Function Name:
 *      dal_cypress_qos_congAvoidSysDropProbability_set
 * Description:
 *      Set system drop probability of congestion avoidance.
 * Input:
 *      unit             - unit id
 *      dp               - drop precedence
 *      probability      - drop probability
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_DROP_PRECEDENCE  - invalid drop precedence
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_cypress_qos_congAvoidSysDropProbability_set(
    uint32  unit,
    uint32  dp,
    uint32  probability);

/* Function Name:
 *      dal_cypress_qos_congAvoidGlobalQueueThresh_get
 * Description:
 *      Get global queue threshold of congestion avoidance.
 * Input:
 *      unit             - unit id
 *      queue            - queue id
 *      dp               - drop precedence
 * Output:
 *      pCongAvoidThresh - pointer to system threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_QUEUE_ID         - invalid queue id
 *      RT_ERR_DROP_PRECEDENCE  - invalid drop precedence
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_qos_congAvoidGlobalQueueThresh_get(
    uint32                      unit,
    rtk_qid_t                   queue,
    uint32                      dp,
    rtk_qos_congAvoidThresh_t   *pCongAvoidThresh);

/* Function Name:
 *      dal_cypress_qos_congAvoidGlobalQueueThresh_set
 * Description:
 *      Set global queue threshold of congestion avoidance.
 * Input:
 *      unit             - unit id
 *      queue            - queue id
 *      dp               - drop precedence
 *      pCongAvoidThresh - system threshold
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_QUEUE_ID         - invalid queue id
 *      RT_ERR_DROP_PRECEDENCE  - invalid drop precedence
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_cypress_qos_congAvoidGlobalQueueThresh_set(
    uint32                      unit,
    rtk_qid_t                   queue,
    uint32                      dp,
    rtk_qos_congAvoidThresh_t   *pCongAvoidThresh);

/* Function Name:
 *      dal_cypress_qos_congAvoidGlobalQueueDropProbability_get
 * Description:
 *      Get global queue drop probability of congestion avoidance.
 * Input:
 *      unit             - unit id
 *      queue            - queue id
 *      dp               - drop precedence
 * Output:
 *      pProbability     - pointer to drop probability
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_QUEUE_ID         - invalid queue id
 *      RT_ERR_DROP_PRECEDENCE  - invalid drop precedence
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_qos_congAvoidGlobalQueueDropProbability_get(
    uint32      unit,
    rtk_qid_t   queue,
    uint32      dp,
    uint32      *pProbability);

/* Function Name:
 *      dal_cypress_qos_congAvoidGlobalQueueDropProbability_set
 * Description:
 *      Set system drop probability of congestion avoidance.
 * Input:
 *      unit             - unit id
 *      queue            - queue id
 *      dp               - drop precedence
 *      probability      - drop probability
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_QUEUE_ID         - invalid queue id
 *      RT_ERR_DROP_PRECEDENCE  - invalid drop precedence
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_cypress_qos_congAvoidGlobalQueueDropProbability_set(
    uint32      unit,
    rtk_qid_t   queue,
    uint32      dp,
    uint32      probability);

/* Function Name:
 *      dal_cypress_qos_congAvoidGlobalQueueConfig_get
 * Description:
 *      Get queue drop probability and thresholds of congestion avoidance.
 * Input:
 *      unit                   - unit id
 *      queue                  - queue id
 *      dp                     - drop precedence
 * Output:
 *      pCongAvoidThresh       - pointer of drop probability and thresholds
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_QUEUE_ID        - invalid queue id
 *      RT_ERR_DROP_PRECEDENCE - invalid drop precedence
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_qos_congAvoidGlobalQueueConfig_get(
    uint32    unit,
    rtk_qid_t queue,
    uint32    dp,
    rtk_qos_congAvoidThresh_t *pCongAvoidThresh);

/* Function Name:
 *      dal_cypress_qos_congAvoidGlobalQueueConfig_set
 * Description:
 *      Set queue drop probability and thresholds of congestion avoidance.
 * Input:
 *      unit             - unit id
 *      queue            - queue id
 *      dp               - drop precedence
 *      pCongAvoidThresh - pointer of drop probability and thresholds
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_QUEUE_ID        - invalid queue id
 *      RT_ERR_DROP_PRECEDENCE - invalid drop precedence
 *      RT_ERR_INPUT           - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_cypress_qos_congAvoidGlobalQueueConfig_set(
    uint32    unit,
    rtk_qid_t queue,
    uint32    dp,
    rtk_qos_congAvoidThresh_t *pCongAvoidThresh);

/* Function Name:
 *      dal_cypress_qos_invldDscpEnable_get
 * Description:
 *      Get the invalid DSCP status in the specified device
 * Input:
 *      unit      - unit id
 * Output:
 *      pEnable   - status of invalid DSCP
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None.
 */
extern int32
dal_cypress_qos_invldDscpEnable_get(uint32 unit, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_cypress_qos_invldDscpEnable_set
 * Description:
 *      Set the invalid dscp status in the specified device
 * Input:
 *      unit     - unit id
 *      enable   - status of invalid DSCP
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_INPUT          - invalid input parameter
 * Note:
 *      None.
 */
extern int32
dal_cypress_qos_invldDscpEnable_set(uint32 unit, rtk_enable_t enable);

/* Function Name:
 *      dal_cypress_qos_invldDscpVal_get
 * Description:
 *      Get the invalid dscp value in the specified device
 * Input:
 *      unit      - unit id
 * Output:
 *      pDscp     - pointer to dscp value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None.
 */
extern int32
dal_cypress_qos_invldDscpVal_get(uint32 unit, uint32 *pDscp);

/* Function Name:
 *      dal_cypress_qos_invldDscpVal_set
 * Description:
 *      Set the invalid dscp value in the specified device
 * Input:
 *      unit     - unit id
 *      dscp     - dscp value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_QOS_DSCP_VALUE - Invalid dscp value
 * Note:
 *      None.
 */
extern int32
dal_cypress_qos_invldDscpVal_set(uint32 unit, uint32 dscp);

#endif /* __DAL_CYPRESS_QOS_H__ */

