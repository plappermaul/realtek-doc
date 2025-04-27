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
 * Purpose : Definition of QoS API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) Ingress Priority Decision
 *           (2) Egress Remarking
 *           (3) Queue Scheduling
 *           (4) Congestion avoidance
 */

#ifndef __RTK_QOS_H__
#define __RTK_QOS_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/port.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
/* Types of queue scheduling algorithm */
typedef enum rtk_qos_scheduling_type_e
{
    WFQ = 0,    /* Weighted-Fair-Queue */
    WRR,        /* Weighted-Round-Robin */
    SCHEDULING_TYPE_END
} rtk_qos_scheduling_type_t;

/* Types of priority queue */
typedef enum rtk_qos_queue_type_e
{
    WFQ_WRR_PRIORITY = 0,   /* WFQ/WRR */
    STRICT_PRIORITY,        /* Strict Priority */
    QUEUE_TYPE_END
} rtk_qos_queue_type_t;

/* Algorithm of congestion avoidance */
typedef enum rtk_qos_congAvoidAlgo_e
{
    CONG_AVOID_SRED,    /* Applicable to 8380 */
    CONG_AVOID_SWRED,   /* Applicable to 8390, 9300, 9310 */
    CONG_AVOID_TD,      /* Applicable to 8380, 8390, 9300, 9310 */
    CONG_AVOID_END
} rtk_qos_congAvoidAlgo_t;

/* Types of drop precedence source */
typedef enum rtk_qos_dpSrc_e
{
    DP_SRC_DEI_BASED = 0,   /* Applicable to 8390 */
    DP_SRC_DSCP_BASED,      /* Applicable to 8390, 9300, 9310 */
    DP_SRC_INNER_PRI_BASED, /* Applicable to 9300, 9310 */
    DP_SRC_OUTER_PRI_BASED, /* Applicable to 9300, 9310 */
    DP_SRC_MPLS_BASED,      /* Applicable to 9310 */
    DP_SRC_END

} rtk_qos_dpSrc_t;

/* Types of DEI source */
typedef enum rtk_qos_deiSel_e
{
    DEI_SEL_INNER_TAG = 0,
    DEI_SEL_OUTER_TAG,
    DEI_SEL_END
} rtk_qos_deiSel_t;

/* Types of priority source */
typedef enum rtk_qos_priSrc_e
{
    PRI_SRC_INT_PRI = 0,
    PRI_SRC_DFLT_PRI,
    PRI_SRC_INNER_USER_PRI,
    PRI_SRC_OUTER_USER_PRI,
    PRI_SRC_DSCP,
    PRI_SRC_PB_PRI,
    PRI_SRC_PB_INNER_PRI,
    PRI_SRC_PB_OUTER_PRI,
    PRI_SRC_PROTO_VLAN,
    PRI_SRC_MAC_VLAN,
    PRI_SRC_DP,
    /*applicable to 9300/9310*/
    PRI_SRC_DP_INT_PRI,
    /*applicable to 9310*/
    PRI_SRC_80211E,
    PRI_SRC_MPLS,
    PRI_SRC_PE_PRI, /* 802.1BR */
    PRI_SRC_END
} rtk_qos_priSrc_t;

/* Types of port-based priority remapping selection */
typedef enum rtk_qos_portPriRemapSel_e
{
    P_PRI_REMAP_INNER_PRI_TBL = 0,  /* Applicable to 8390 */
    P_PRI_REMAP_INNER_PRI_CFI0_TBL, /* Applicable to 9300, 9310 */
    P_PRI_REMAP_OUTER_PRI_DEI0_TBL, /* Applicable to 8390, 9300, 9310 */
    P_PRI_REMAP_END
} rtk_qos_portPriRemapSel_t;

/* Types of priority remap source */
typedef struct rtk_qos_priSrcRemap_e
{
    union
    {
        rtk_port_t port;
        uint32 dscp;
#if defined(CONFIG_SDK_RTL9310)
        uint32 tc;
        uint32 tid;
#endif
        struct
        {
            uint32 dei;
            rtk_pri_t pri;
        }tag;
    }src;
} rtk_qos_priSrcRemap_t;

/* Types of drop precedence remap source */
typedef struct rtk_qos_dpSrcRemap_e
{
    union
    {
        uint32 dscp;
#if defined(CONFIG_SDK_RTL9310)
        uint32 tc;
#endif
        struct
        {
            uint32 dei;
            rtk_pri_t pri;
        }tag;
    }src;
} rtk_qos_dpSrcRemap_t;

typedef struct rtk_qos_dpSelWeight_s
{
    uint32 weight_of_dscp;     /* Applicable to 9300, 9310 */
    uint32 weight_of_innerTag; /* Applicable to 9300, 9310 */
    uint32 weight_of_outerTag; /* Applicable to 9300, 9310 */
#if defined(CONFIG_SDK_RTL9310)
    uint32 weight_of_mpls;     /* Applicable to 9310 */
#endif
} rtk_qos_dpSelWeight_t;

/* Types of inner priority remarking source */
typedef enum rtk_qos_1pRmkSrc_e
{
    DOT_1P_RMK_SRC_INT_PRI = PRI_SRC_INT_PRI,
    DOT_1P_RMK_SRC_USER_PRI = PRI_SRC_INNER_USER_PRI,
    DOT_1P_RMK_SRC_OUTER_USER_PRI = PRI_SRC_OUTER_USER_PRI,
    DOT_1P_RMK_SRC_DSCP = PRI_SRC_DSCP,
    DOT_1P_RMK_SRC_END = PRI_SRC_END
} rtk_qos_1pRmkSrc_t;

typedef struct rtk_qos_1pRmkVal_s
{
    union
    {
        struct
        {
            rtk_pri_t val;
        }pri;
        struct
        {
            uint32 val;
        }dscp;
    };
} rtk_qos_1pRmkVal_t;

/* Types of default inner priority */
typedef enum rtk_qos_1pDfltSrc_s
{
    INNER_1P_DFLT_SRC_INT_PRI = PRI_SRC_INT_PRI,
    INNER_1P_DFLT_SRC_DFLT_PRI = PRI_SRC_DFLT_PRI,
    /* Applicable to 8390 */
    INNER_1P_DFLT_SRC_PB_PRI = PRI_SRC_PB_PRI,
    /* Applicable to 8380 */
    INNER_1P_DFLT_SRC_PB_INNER_PRI = PRI_SRC_PB_INNER_PRI,
    INNER_1P_DFLT_SRC_END = PRI_SRC_END
} rtk_qos_1pDfltPriSrc_t;

/* Types of outer priority remarking source */
typedef enum rtk_qos_outer1pRmkSrc_e
{
    OUTER_1P_RMK_SRC_INT_PRI = PRI_SRC_INT_PRI ,
    OUTER_1P_RMK_SRC_USER_PRI = PRI_SRC_OUTER_USER_PRI,
    OUTER_1P_RMK_SRC_INNER_USER_PRI = PRI_SRC_INNER_USER_PRI,
    OUTER_1P_RMK_SRC_DSCP = PRI_SRC_DSCP,
    OUTER_1P_RMK_SRC_END = PRI_SRC_END
} rtk_qos_outer1pRmkSrc_t;

typedef struct rtk_qos_outer1pRmkVal_s
{
    union
    {
        struct
        {
            rtk_pri_t val;
        }pri;
        struct
        {
            uint32 val;
        }dscp;
    };
} rtk_qos_outer1pRmkVal_t;

/* Types of default outer priority configured source */
typedef enum rtk_qos_outer1pDfltCfgSrc_e
{
    OUTER_1P_DFLT_CFG_SRC_INGRESS = 0,
    OUTER_1P_DFLT_CFG_SRC_EGRESS,
    OUTER_1P_DFLT_CFG_SRC_END
} rtk_qos_outer1pDfltCfgSrc_t;

/* Types of default inner priority configured source */
typedef enum rtk_qos_1pDfltCfgSrc_e
{
    INNER_1P_DFLT_CFG_SRC_INGRESS = 0,
    INNER_1P_DFLT_CFG_SRC_EGRESS,
    INNER_1P_DFLT_CFG_SRC_END
} rtk_qos_1pDfltCfgSrc_t;

/* Types of DSCP remarking source */
typedef enum rtk_qos_dscpRmkSrc_e
{
    DSCP_RMK_SRC_INT_PRI = PRI_SRC_INT_PRI ,
    DSCP_RMK_SRC_DSCP = PRI_SRC_DSCP,
    DSCP_RMK_SRC_DP = PRI_SRC_DP,
    DSCP_RMK_SRC_USER_PRI = PRI_SRC_INNER_USER_PRI,
    DSCP_RMK_SRC_OUTER_USER_PRI = PRI_SRC_OUTER_USER_PRI,
    /*Applicable to 9300, 9310*/
    DSCP_RMK_SRC_DP_INT_PRI = PRI_SRC_DP_INT_PRI,
    DSCP_RMK_SRC_END = PRI_SRC_END
} rtk_qos_dscpRmkSrc_t;

typedef struct rtk_qos_dscpRmkVal_s
{
    union
    {
        struct
        {
            rtk_pri_t val;
        }pri;
        struct
        {
            uint32 val;
        }dscp;
        struct
        {
            uint32 val;
        }dp;
        struct
        {
            rtk_pri_t   pri;
            uint32      dp;
        }intPriDp;
    };
}rtk_qos_dscpRmkVal_t;


/* Types of DEI remark source */
typedef enum rtk_qos_deiRmkSrc_e
{
    DEI_RMK_SRC_INT_PRI = 0,
    DEI_RMK_SRC_DP,
    DEI_RMK_SRC_END
} rtk_qos_deiRmkSrc_t;

typedef struct rtk_qos_deiRmkVal_s
{
    union
    {
        struct
        {
            rtk_pri_t val;
        }pri;
        struct
        {
            uint32 val;
        }dp;
    };
} rtk_qos_deiRmkVal_t;

/* Types of default outer priority */
typedef enum rtk_qos_outer1pDfltSrc_e
{
    OUTER_1P_DFLT_SRC_INT_PRI = PRI_SRC_INT_PRI ,
    OUTER_1P_DFLT_SRC_USER_PRI = PRI_SRC_INNER_USER_PRI,
    OUTER_1P_DFLT_SRC_DFLT_PRI = PRI_SRC_DFLT_PRI,
    /* Applicable to 8390 */
    OUTER_1P_DFLT_SRC_PB_PRI = PRI_SRC_PB_PRI,
    /* Applicable to 8380 */
    OUTER_1P_DFLT_SRC_PB_OUTER_PRI = PRI_SRC_PB_OUTER_PRI,
    OUTER_1P_DFLT_SRC_END = PRI_SRC_END
} rtk_qos_outer1pDfltSrc_t;

/* AVB SR Class */
typedef enum rtk_qos_avbSrClass_e
{
    AVB_SR_CLASS_A = 0,
    AVB_SR_CLASS_B,
    AVB_SR_CLASS_END
} rtk_qos_avbSrClass_t;

/* Priority to queue mapping structure */
typedef struct rtk_qos_pri2queue_s
{
    uint32 pri2queue[RTK_MAX_NUM_OF_PRIORITY];
} rtk_qos_pri2queue_t;

/* Queue weights structure */
typedef struct rtk_qos_queue_weights_s
{
    uint32 weights[RTK_MAX_NUM_OF_QUEUE];
} rtk_qos_queue_weights_t;

/* Weight of each priority source */
typedef struct rtk_qos_priSelWeight_s
{
    uint32 weight_of_dscp;
    uint32 weight_of_innerTag;
    uint32 weight_of_outerTag;
#if defined(CONFIG_SDK_RTL8390)
    uint32 weight_of_inAcl;
#endif
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    uint32 weight_of_portBased;
    uint32 weight_of_macVlan;
    uint32 weight_of_protoVlan;
#endif
#if defined(CONFIG_SDK_RTL8380)
    uint32 weight_of_portBasedIpri;
    uint32 weight_of_portBasedOpri;
#endif
#if defined(CONFIG_SDK_RTL9300)  || defined(CONFIG_SDK_RTL9310)
    uint32 weight_of_vlanAcl;
    uint32 weight_of_routing;
#endif
#if defined(CONFIG_SDK_RTL9310)
    uint32 weight_of_peTag; /* 802.1BR */
    uint32 weight_of_80211e;
    uint32 weight_of_tunnel;
    uint32 weight_of_mpls;
#endif
} rtk_qos_priSelWeight_t;

/* Threshold for congestion avoidance */
typedef struct rtk_qos_congAvoidThresh_s
{
    uint32  maxThresh;
    uint32  minThresh;
    uint32  probability;
} rtk_qos_congAvoidThresh_t;

/* Threshold for WRED */
typedef struct rtk_qos_wredThresh_s
{
    uint32  maxThresh;
    uint32  minThresh;
} rtk_qos_wredThresh_t;

/* AVB SR Configuration */
typedef struct rtk_qos_avbSrConf_e
{
    rtk_pri_t class_a_priority;                 /* priority to identify class A traffic */
    rtk_pri_t class_b_priority;                 /* priority to identify class B traffic */
    rtk_qid_t class_a_queue_id;                 /* force class A packet to use this queue */
    rtk_qid_t class_b_queue_id;                 /* force class B packet to use this queue */
    rtk_qid_t class_non_a_redirect_queue_id;    /* force non-class A packet to use this queue */
    rtk_qid_t class_non_b_redirect_queue_id;    /* force non-class B packet to use this queue */
    rtk_pri_t class_non_a_remark_priority;      /* force non-class A packet to be remarked with this priority */
    rtk_pri_t class_non_b_remark_priority;      /* force non-class B packet to be remarked with this priority */
} rtk_qos_avbSrConf_t;

/*
 * Function Declaration
 */

/* Function Name:
 *      rtk_qos_init
 * Description:
 *      Configure QoS initial settings with queue number assigment to each port
 * Input:
 *      unit     - unit id
 *      queueNum - Queue number of each port, ranges from 1~8
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID   - Invalid unit id
 *      RT_ERR_QUEUE_NUM - Invalid queue number
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) This API will initialize related QoS setting with queue number assignment.
 *      (2) The initialization does the following actions:
 *          - set input bandwidth control parameters to default values
 *          - set priority decision parameters
 *          - set scheduling parameters
 *          - disable port remark ability
 *          - set flow control thresholds
 * Changes:
 *      None
 */
extern int32
rtk_qos_init(uint32 unit, uint32 queueNum);

/* Module Name    : QoS                       */
/* Sub-module Name: Ingress priority decision */

/* Function Name:
 *      rtk_qos_priSelGroup_get
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
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) Each port can bind a priority selection group through 'rtk_qos_portPriSelGroup_set'.
 *      (2) The valid range of grp_idx is 0~3 in 8390 & 8380.
 * Changes:
 *      None
 */
extern int32
rtk_qos_priSelGroup_get(uint32 unit, uint32 grp_idx, rtk_qos_priSelWeight_t *pWeightOfPriSel);

/* Function Name:
 *      rtk_qos_priSelGroup_set
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
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) Each port can bind a priority selection group through 'rtk_qos_portPriSelGroup_set'.
 *      (2) The valid range of grp_idx is 0~3 in 8390 & 8380.
 * Changes:
 *      None
 */
extern int32
rtk_qos_priSelGroup_set(uint32 unit, uint32 grp_idx, rtk_qos_priSelWeight_t *pWeightOfPriSel);

/* Function Name:
 *      rtk_qos_portPriSelGroup_get
 * Description:
 *      Get priority selection group binding for specified port.
 * Input:
 *      unit           - unit id
 *      port           - port id
 * Output:
 *      pPriSelGrp_idx - pointer to index of priority selection group
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) Priority selection group is configured by 'rtk_qos_priSelGroup_set'.
 *      (2) The valid range of priSelGrp_idx is 0~3 in 8390 & 8380.
 * Changes:
 *      None
 */
extern int32
rtk_qos_portPriSelGroup_get(uint32 unit, rtk_port_t port, uint32 *pPriSelGrp_idx);

/* Function Name:
 *      rtk_qos_portPriSelGroup_set
 * Description:
 *      Set priority selection group binding for specified port.
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      priSelGrp_idx - index of priority selection group
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
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) Priority selection group is configured by 'rtk_qos_priSelGroup_set'.
 *      (2) The valid range of priSelGrp_idx is 0~3 in 8390 & 8380.
 * Changes:
 *      None
 */
extern int32
rtk_qos_portPriSelGroup_set(uint32 unit, rtk_port_t port, uint32 priSelGrp_idx);

/* Function Name:
 *      rtk_qos_sysPortPriRemapSel_get
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
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      In 9300 and 9310, the API is backward compatible to 8390.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_qos_sysPortPriRemapSel_get(uint32 unit, rtk_qos_portPriRemapSel_t *pType);
/* rename rtk_qos_portPriRemapSel_get(uint32 unit, rtk_qos_portPriRemapSel_t *pType); */

/* Function Name:
 *      rtk_qos_sysPortPriRemapSel_set
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
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      In 9300 and 9310, the API is backward compatible to 8390.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_qos_sysPortPriRemapSel_set(uint32 unit, rtk_qos_portPriRemapSel_t type);
/* rename rtk_qos_portPriRemapSel_set(uint32 unit, rtk_qos_portPriRemapSel_t type); */

/* Function Name:
 *      rtk_qos_portPortPriRemapSel_get
 * Description:
 *      Get port-based priority remapping table on specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pType   - remapping table selection
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
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
rtk_qos_portPortPriRemapSel_get(uint32 unit, rtk_port_t port, rtk_qos_portPriRemapSel_t *pType);

/* Function Name:
 *      rtk_qos_portPortPriRemapSel_set
 * Description:
 *      Set port-based priority remapping table on specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      type    - remapping table selection
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
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
rtk_qos_portPortPriRemapSel_set(uint32 unit, rtk_port_t port, rtk_qos_portPriRemapSel_t type);

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
/* Function Name:
 *      rtk_qos_portPri_get
 * Description:
 *      Get internal priority of specific port.
 * Input:
 *      unit     - unit id
 *      port     - port id
 * Output:
 *      pInt_pri - Priorities assigment for specific port. (range from 0 ~ 7, 7 is
 *                 the highest prioirty)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_qos_priRemap_get(unit, src, srcVal, pPri)
 *          Parameters:
 *              src                         -  PRI_SRC_PB_PRI
 *              srcVal.src.port             -  port
 *              pPri                        -  pInt_pri
 */
extern int32
rtk_qos_portPri_get(uint32 unit, rtk_port_t port, rtk_pri_t *pInt_pri);

/* Function Name:
 *      rtk_qos_portPri_set
 * Description:
 *      Set internal priority of specific port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      int_pri - Priorities assigment for specific port. (range from 0 ~ 7, 7 is
 *                the highest prioirty)
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - Invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - Invalid port id
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority value
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      (1) This API can set port to 3 bits internal priority mapping.
 *      (2) When a packet is received from a port, a port based priority will be assigned
 *          by the mapping setting.
 *      (3) By default, the mapping priorities for all ports are 0.
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_qos_priRemap_set(unit, src, srcVal, pri)
 *          Parameters:
 *              src                         -  PRI_SRC_PB_PRI
 *              srcVal.src.port             -  port
 *              pri                         -  int_pri
 */
extern int32
rtk_qos_portPri_set(uint32 unit, rtk_port_t port, rtk_pri_t int_pri);

/* Function Name:
 *      rtk_qos_portPriRemapEnable_get
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
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      In 9300 and 9310, the API is backward compatible to 8390.
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_qos_priRemapEnable_get(unit, src, pEnable)
 *          Parameters:
 *              src                         -  PRI_SRC_PB_PRI
 *              pEnable                     -  pEnable
 */
extern int32
rtk_qos_portPriRemapEnable_get(uint32 unit, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_qos_portPriRemapEnable_set
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
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      In 9300 and 9310, the API is backward compatible to 8390.
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_qos_priRemapEnable_set(unit, src, enable)
 *          Parameters:
 *              src                         -  PRI_SRC_PB_PRI
 *              enable                      -  enable
 */
extern int32
rtk_qos_portPriRemapEnable_set(uint32 unit, rtk_enable_t enable);

/* Function Name:
 *      rtk_qos_portInnerPri_get
 * Description:
 *      Get priority of inner tag on specified port.
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pPri - pointer to priority of inner tag
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_qos_priRemap_get(unit, src, srcVal, pPri)
 *          Parameters:
 *              src                         -  PRI_SRC_PB_INNER_PRI
 *              srcVal.src.port             -  port
 *              pPri                        -  pPri
 */
extern int32
rtk_qos_portInnerPri_get(uint32 unit, rtk_port_t port, rtk_pri_t *pPri);

/* Function Name:
 *      rtk_qos_portInnerPri_set
 * Description:
 *      Set priority of inner tag on specified port.
 * Input:
 *      unit - unit id
 *      port - port id
 *      pri  - priority of inner tag
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID       - invalid unit id
 *      RT_ERR_NOT_INIT      - The module is not initial
 *      RT_ERR_PORT_ID       - invalid port id
 *      RT_ERR_VLAN_PRIORITY - invalid priority
 * Applicable:
 *      8380
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_qos_priRemap_set(unit, src, srcVal, pri)
 *          Parameters:
 *              src                         -  PRI_SRC_PB_INNER_PRI
 *              srcVal.src.port             -  port
 *              pri                         -  pri
 */
extern int32
rtk_qos_portInnerPri_set(uint32 unit, rtk_port_t port, rtk_pri_t pri);
#endif

/* Function Name:
 *      rtk_qos_dpSrcSel_get
 * Description:
 *      Get drop precedence source.
 * Input:
 *      unit  - unit id
 * Output:
 *      pType - DP mapping source
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390
 * Note:
 *      The mapping of DEI -> DP and DSCP -> DP are set through 'rtk_qos_deiDpRemap_set' and
 *      'rtk_qos_dscpDpRemap_set' respectively.
 * Changes:
 *      None
 */
extern int32
rtk_qos_dpSrcSel_get(uint32 unit, rtk_qos_dpSrc_t *pType);

/* Function Name:
 *      rtk_qos_dpSrcSel_set
 * Description:
 *      Set drop precedence source.
 * Input:
 *      unit - unit id
 *      type - DP mapping source
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
 *      The mapping of DEI -> DP and DSCP -> DP are set through 'rtk_qos_deiDpRemap_set' and
 *      'rtk_qos_dscpDpRemap_set' respectively.
 * Changes:
 *      None
 */
extern int32
rtk_qos_dpSrcSel_set(uint32 unit, rtk_qos_dpSrc_t type);

/* Function Name:
 *      rtk_qos_portDEISrcSel_get
 * Description:
 *      Get DEI source of specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pType  - DEI source
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390
 * Note:
 *      (1) Each port can specify the DEI is either from inner-tag or outer-tag.
 *      (2) DEI can be the DP mapping source through configure 'rtk_qos_dpSrcSel_set'.
 *      (3) Change to use rtk_qos_deiRemarkSrcSel_get(unit, pType) for 9300,9310
 * Changes:
 *      None
 */
extern int32
rtk_qos_portDEISrcSel_get(uint32 unit, rtk_port_t port, rtk_qos_deiSel_t *pType);

/* Function Name:
 *      rtk_qos_portDEISrcSel_set
 * Description:
 *      Set DEI source of specified port.
 * Input:
 *      unit - unit id
 *      port - port id
 *      type - DEI source
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
 *      8390
 * Note:
 *      (1) Each port can specify the DEI is either from inner-tag or outer-tag.
 *      (2) DEI can be the DP mapping source through configure 'rtk_qos_dpSrcSel_set'.
 *      (3) Change to use rtk_qos_deiRemarkSrcSel_set(unit, type) for 9300,9310
 * Changes:
 *      None
 */
extern int32
rtk_qos_portDEISrcSel_set(uint32 unit, rtk_port_t port, rtk_qos_deiSel_t type);

/* Function Name:
 *      rtk_qos_portDpSel_get
 * Description:
 *      Get weight of each priority assignment on specified port.
 * Input:
 *      unit            - unit id
 *      port            - port id
 * Output:
 *      pWeightOfDpSel  - pointer to weight of each priority assignment
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
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
rtk_qos_portDpSel_get(uint32 unit, rtk_port_t port, rtk_qos_dpSelWeight_t *pWeightOfDpSel);

/* Function Name:
 *      rtk_qos_portDpSel_set
 * Description:
 *      Set weight of each priority assignment on specified port.
 * Input:
 *      unit            - unit id
 *      port            - port id
 *      pWeightOfDpSel  - weight of each priority assignment
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
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
rtk_qos_portDpSel_set(uint32 unit, rtk_port_t port, rtk_qos_dpSelWeight_t *pWeightOfDpSel);

/* Function Name:
 *      rtk_qos_deiDpRemap_get
 * Description:
 *      Get DEI mapping to drop precedence.
 * Input:
 *      unit - unit id
 *      dei  - DEI
 * Output:
 *      pDp  - pointer to drop precedence
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
 *      The API can get configuration of DEI to DP remapping table.
 * Changes:
 *      None
 */
extern int32
rtk_qos_deiDpRemap_get(uint32 unit, uint32 dei, uint32 *pDp);

/* Function Name:
 *      rtk_qos_deiDpRemap_set
 * Description:
 *      Set DEI mapping to drop precedence.
 * Input:
 *      unit - unit id
 *      dei  - DEI
 *      dp   - drop precedence
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_INPUT           - invalid input parameter
 *      RT_ERR_DROP_PRECEDENCE - invalid drop precedence
 * Applicable:
 *      8390
 * Note:
 *      The API can configure DEI to DP remapping table.
 * Changes:
 *      None
 */
extern int32
rtk_qos_deiDpRemap_set(uint32 unit, uint32 dei, uint32 dp);

/* Function Name:
 *      rtk_qos_dpRemap_get
 * Description:
 *      Get DEI/DSCP/Inner-tag/Outer-tag/MPLS mapping to drop precedence.
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
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      Supported DP remap source is as following:
 *      - rtk_qos_dpSrc_t \ Chip:      8390    9300    9310
 *      - DP_SRC_DEI_BASED             O       X       X
 *      - DP_SRC_DSCP_BASED            O       O       O
 *      - DP_SRC_INNER_PRI_BASED       X       O       O
 *      - DP_SRC_OUTER_PRI_BASED       X       O       O
 *      - DP_SRC_MPLS_BASED            X       O       O
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_qos_dscpDpRemap_get)
 */
extern int32
rtk_qos_dpRemap_get(uint32 unit, rtk_qos_dpSrc_t src, rtk_qos_dpSrcRemap_t srcVal, uint32 *pDp);

/* Function Name:
 *      rtk_qos_dpRemap_set
 * Description:
 *      Set DEI/DSCP/Inner-tag/Outer-tag/MPLS mapping to drop precedence.
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
 *      RT_ERR_QOS_1P_PRIORITY - Invalid priority value
 *      RT_ERR_DROP_PRECEDENCE - invalid drop precedence
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      Supported DP remap source is as following:
 *      - rtk_qos_dpSrc_t \ Chip:      8390    9300    9310
 *      - DP_SRC_DEI_BASED             O       X       X
 *      - DP_SRC_DSCP_BASED            O       O       O
 *      - DP_SRC_INNER_PRI_BASED       X       O       O
 *      - DP_SRC_OUTER_PRI_BASED       X       O       O
 *      - DP_SRC_MPLS_BASED            X       O       O
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_qos_dscpDpRemap_set)
 */
extern int32
rtk_qos_dpRemap_set(uint32 unit, rtk_qos_dpSrc_t src, rtk_qos_dpSrcRemap_t srcVal, uint32 dp);

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
/* Function Name:
 *      rtk_qos_dscpDpRemap_get
 * Description:
 *      Get DSCP mapping to drop precedence.
 * Input:
 *      unit - unit id
 *      dscp - DSCP value of receiving frame (0~63)
 * Output:
 *      pDp  - pointer to drop precedence
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_QOS_DSCP_VALUE - Invalid DSCP value
 *      RT_ERR_NULL_POINTER   - input parameter may be null pointer
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      The API can get configuration of DSCP to DP remapping table.
 *      In 9300 and 9310, the API is backward compatible to 8390.
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_qos_dpRemap_get(unit, src, srcVal, pDp)
 *          Parameters:
 *              src                         -  DP_SRC_DSCP_BASED
 *              srcVal.src.dscp             -  dscp
 *              pDp                         -  pDp
 */
extern int32
rtk_qos_dscpDpRemap_get(uint32 unit, uint32 dscp, uint32 *pDp);

/* Function Name:
 *      rtk_qos_dscpDpRemap_set
 * Description:
 *      Set DSCP mapping to drop precedence.
 * Input:
 *      unit - unit id
 *      dscp - DSCP value of receiving frame (0~63)
 *      dp   - drop precedence
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_QOS_DSCP_VALUE  - Invalid DSCP value
 *      RT_ERR_DROP_PRECEDENCE - invalid drop precedence
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      The API can configure DSCP to DP remapping table.
 *      In 9300 and 9310, the API is backward compatible to 8390.
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_qos_dpRemap_set(unit, src, srcVal, dp)
 *          Parameters:
 *              src                         -  DP_SRC_DSCP_BASED
 *              srcVal.src.dscp             -  dscp
 *              dp                          -  dp
 */
extern int32
rtk_qos_dscpDpRemap_set(uint32 unit, uint32 dscp, uint32 dp);

/* Function Name:
 *      rtk_qos_portOuterPri_get
 * Description:
 *      Get priority of outer tag on specified port.
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pPri - pointer to priority of outer tag
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_qos_priRemap_get(unit, src, srcVal, pPri)
 *          Parameters:
 *              src                         -  PRI_SRC_PB_OUTER_PRI
 *              srcVal.src.port             -  port
 *              pPri                        -  pPri
 */
extern int32
rtk_qos_portOuterPri_get(uint32 unit, rtk_port_t port, rtk_pri_t *pPri);

/* Function Name:
 *      rtk_qos_portOuterPri_set
 * Description:
 *      Set priority of outer tag on specified port.
 * Input:
 *      unit - unit id
 *      port - port id
 *      pri  - priority of outer tag
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID       - invalid unit id
 *      RT_ERR_NOT_INIT      - The module is not initial
 *      RT_ERR_PORT_ID       - invalid port id
 *      RT_ERR_VLAN_PRIORITY - invalid priority
 * Applicable:
 *      8380
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_qos_priRemap_set(unit, src, srcVal, pri)
 *          Parameters:
 *              src                         -  PRI_SRC_PB_OUTER_PRI
 *              srcVal.src.port             -  port
 *              pri                        -  pri
 */
extern int32
rtk_qos_portOuterPri_set(uint32 unit, rtk_port_t port, rtk_pri_t pri);

/* Function Name:
 *      rtk_qos_dscpPriRemap_get
 * Description:
 *      Get the internal priority that DSCP value remap.
 * Input:
 *      unit     - unit id
 *      dscp     - DSCP value of receiving frame (0~63)
 * Output:
 *      pInt_pri - Priorities assigment for specific port. (range from 0 ~ 7, 7 is
 *                  the highest prioirty)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - Invalid unit id
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_QOS_DSCP_VALUE - Invalid DSCP value
 *      RT_ERR_NULL_POINTER   - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      In 9300 and 9310, it is backward compatibility API to 8380/8390.
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_qos_priRemap_get(unit, src, srcVal, pPri)
 *          Parameters:
 *              src                         -  PRI_SRC_DSCP
 *              srcVal.src.dscp             -  dscp
 *              pPri                        -  pInt_pri
 */
extern int32
rtk_qos_dscpPriRemap_get(uint32 unit, uint32 dscp, rtk_pri_t *pInt_pri);

/* Function Name:
 *      rtk_qos_dscpPriRemap_set
 * Description:
 *      Set the internal priority that DSCP value remap.
 * Input:
 *      unit    - unit id
 *      dscp    - DSCP value of receiving frame (0~63)
 *      int_pri - Priorities assigment for specific port. (range from 0 ~ 7, 7 is
 *                the highest prioirty)
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - Invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_QOS_DSCP_VALUE   - Invalid DSCP value
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority value
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      The Differentiated Service Code Point is a selector for router's per-hop behaviours.
 *      As a selector, there is no implication that a numerically greater DSCP implies a better
 *      network service. As can be seen, the DSCP totally overlaps the old precedence field of TOS.
 *      So if values of DSCP are carefully chosen then backward compatibility can be achieved.
 *      In 9300 and 9310, it is backward compatibility API to 8380/8390.
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_qos_priRemap_set(unit, src, srcVal, pri)
 *          Parameters:
 *              src                         -  PRI_SRC_DSCP
 *              srcVal.src.dscp             -  dscp
 *              pri                         -  int_pri
 */
extern int32
rtk_qos_dscpPriRemap_set(uint32 unit, uint32 dscp, rtk_pri_t int_pri);

/* Function Name:
 *      rtk_qos_1pPriRemap_get
 * Description:
 *      Get the internal priority that 802.1p priority remap.
 * Input:
 *      unit      - unit id
 *      dot1p_pri - 802.1p priority value (range from 0 ~ 7)
 * Output:
 *      pInt_pri  - internal priority value (range from 0 ~ 7)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID         - Invalid unit id
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_QOS_1P_PRIORITY - Invalid 802.1p priority
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      In 9300 and 9310, it is backward compatibility API to 8380/8390.
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_qos_priRemap_get(unit, src, srcVal, pPri)
 *          Parameters:
 *              src                         -  PRI_SRC_INNER_USER_PRI
 *              srcVal.src.tag.pri          -  dot1p_pri
 *              srcVal.src.tag.dei          -  0
 *              pPri                        -  pInt_pri
 */
extern int32
rtk_qos_1pPriRemap_get(uint32 unit, rtk_pri_t dot1p_pri, rtk_pri_t *pInt_pri);

/* Function Name:
 *      rtk_qos_1pPriRemap_set
 * Description:
 *      Set the internal priority that 802.1p priority remap.
 * Input:
 *      unit      - unit id
 *      dot1p_pri - 802.1p priority value (range from 0 ~ 7)
 *      int_pri   - internal priority value (range from 0 ~ 7)
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - Invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_QOS_1P_PRIORITY  - Invalid 802.1p priority
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority value
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      In 9300 and 9310, it is backward compatibility API to 8380/8390.
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_qos_priRemap_set(unit, src, srcVal, pri)
 *          Parameters:
 *              src                         -  PRI_SRC_INNER_USER_PRI
 *              srcVal.src.tag.pri          -  dot1p_pri
 *              srcVal.src.tag.dei          -  0
 *              pri                         -  int_pri
 */
extern int32
rtk_qos_1pPriRemap_set(uint32 unit, rtk_pri_t dot1p_pri, rtk_pri_t int_pri);

/* Function Name:
 *      rtk_qos_outer1pPriRemap_get
 * Description:
 *      Get the internal priority that outer 802.1p priority remap.
 * Input:
 *      unit      - unit id
 *      dot1p_pri - 802.1p priority value (range from 0 ~ 7)
 *      dei       - DEI
 * Output:
 *      pInt_pri  - internal priority value (range from 0 ~ 7)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID         - Invalid unit id
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_QOS_1P_PRIORITY - Invalid 802.1p priority
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      DEI 0 and 1 can have different outer 1P priority to internal priority mapping.
 *      In 9300 and 9310, it is backward compatibility API to 8380/8390.
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_qos_priRemap_get(unit, src, srcVal, pPri)
 *          Parameters:
 *              src                         -  PRI_SRC_OUTER_USER_PRI
 *              srcVal.src.tag.pri          -  dot1p_pri
 *              srcVal.src.tag.dei          -  0
 *              pPri                        -  pInt_pri
 */
extern int32
rtk_qos_outer1pPriRemap_get(uint32 unit, rtk_pri_t dot1p_pri, uint32 dei, rtk_pri_t *pInt_pri);

/* Function Name:
 *      rtk_qos_outer1pPriRemap_set
 * Description:
 *      Set the internal priority that outer 802.1p priority remap.
 * Input:
 *      unit      - unit id
 *      dot1p_pri - 802.1p priority value (range from 0 ~ 7)
 *      dei       - DEI
 *      int_pri   - internal priority value (range from 0 ~ 7)
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - Invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_QOS_1P_PRIORITY  - Invalid 802.1p priority
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority value
 *      RT_ERR_QOS_DEI_VALUE    - invalid dei
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      DEI 0 and 1 can have different outer 1P priority to internal priority mapping.
 *      In 9300 and 9310, it is backward compatibility API to 8380/8390.
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_qos_priRemap_set(unit, src, srcVal, pri)
 *          Parameters:
 *              src                         -  PRI_SRC_OUTER_USER_PRI
 *              srcVal.src.tag.pri          -  dot1p_pri
 *              srcVal.src.tag.dei          -  0
 *              pri                         -  int_pri
 */
extern int32
rtk_qos_outer1pPriRemap_set(uint32 unit, rtk_pri_t dot1p_pri, uint32 dei, rtk_pri_t int_pri);
#endif

/* Function Name:
 *      rtk_qos_priRemap_get
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
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_QOS_DSCP_VALUE  - Invalid DSCP value
 *      RT_ERR_QOS_DEI_VALUE   - Invalid DEI/CFI value
 *      RT_ERR_QOS_1P_PRIORITY - Invalid priority value
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Supported priority remap source is as following:
 *      - rtk_qos_priSrc_t \ Chip:      8380    8390    9300    9310
 *      - PRI_SRC_INT_PRI               X       X       X       X
 *      - PRI_SRC_DFLT_PRI              X       X       X       X
 *      - PRI_SRC_INNER_USER_PRI        O       O       O       O
 *      - PRI_SRC_OUTER_USER_PRI        O       O       O       O
 *      - PRI_SRC_DSCP                  O       O       O       O
 *      - PRI_SRC_PB_PRI                X       O       O       O
 *      - PRI_SRC_PB_INNER_PRI          O       X       X       X
 *      - PRI_SRC_PB_OUTER_PRI          O       X       X       X
 *      - PRI_SRC_PROTO_VLAN            X       X       X       X
 *      - PRI_SRC_MAC_VLAN              X       X       X       X
 *      - PRI_SRC_DP                    X       X       X       X
 *      - PRI_SRC_DP_INT_PRI            X       X       X       X
 *      - PRI_SRC_80211E                X       X       X       O
 * Changes:
 *      [SDK_3.0.0]
 *          (1)New added function.
 *          (2)It obsoletes these functions:
 *              rtk_qos_1pPriRemap_get
 *              rtk_qos_dscpPriRemap_get
 *              rtk_qos_outer1pPriRemap_get
 *              rtk_qos_portPri_get
 *              rtk_qos_portInnerPri_get
 *              rtk_qos_portOuterPri_get
 */
extern int32
rtk_qos_priRemap_get(uint32 unit, rtk_qos_priSrc_t src, rtk_qos_priSrcRemap_t srcVal, uint32 *pPri);

/* Function Name:
 *      rtk_qos_priRemap_set
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
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_QOS_DSCP_VALUE  - Invalid DSCP value
 *      RT_ERR_QOS_DEI_VALUE   - Invalid DEI/CFI value
 *      RT_ERR_QOS_1P_PRIORITY - Invalid priority value
 *      RT_ERR_DROP_PRECEDENCE - invalid drop precedence
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Supported priority remap source is as following:
 *      - rtk_qos_priSrc_t \ Chip:      8380    8390    9300    9310
 *      - PRI_SRC_INT_PRI               X       X       X       X
 *      - PRI_SRC_DFLT_PRI              X       X       X       X
 *      - PRI_SRC_INNER_USER_PRI        O       O       O       O
 *      - PRI_SRC_OUTER_USER_PRI        O       O       O       O
 *      - PRI_SRC_DSCP                  O       O       O       O
 *      - PRI_SRC_PB_PRI                X       O       O       O
 *      - PRI_SRC_PB_INNER_PRI          O       X       X       X
 *      - PRI_SRC_PB_OUTER_PRI          O       X       X       X
 *      - PRI_SRC_PROTO_VLAN            X       X       X       X
 *      - PRI_SRC_MAC_VLAN              X       X       X       X
 *      - PRI_SRC_DP                    X       X       X       X
 *      - PRI_SRC_DP_INT_PRI            X       X       X       X
 *      - PRI_SRC_80211E                X       X       X       O
 * Changes:
 *      [SDK_3.0.0]
 *          (1)New added function.
 *          (2)It obsoletes these functions:
 *              rtk_qos_1pPriRemap_set
 *              rtk_qos_dscpPriRemap_set
 *              rtk_qos_outer1pPriRemap_set
 *              rtk_qos_portPri_set
 *              rtk_qos_portInnerPri_set
 *              rtk_qos_portOuterPri_set
 */
extern int32
rtk_qos_priRemap_set(uint32 unit, rtk_qos_priSrc_t src, rtk_qos_priSrcRemap_t srcVal, uint32 pri);

/* Function Name:
 *      rtk_qos_queueNum_get
 * Description:
 *      Get the number of queue for the system.
 * Input:
 *      unit       - unit id
 * Output:
 *      pQueue_num - the number of queue (1~8).
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390
 * Note:
 *      (1) The valid range of queue_num is 1~8
 * Changes:
 *      None
 */
extern int32
rtk_qos_queueNum_get(uint32 unit, uint32 *pQueue_num);

/* Function Name:
 *      rtk_qos_queueNum_set
 * Description:
 *      Set the number of queue for the system.
 * Input:
 *      unit      - unit id
 *      queue_num - the number of queue (1~8).
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID   - Invalid unit id
 *      RT_ERR_NOT_INIT  - The module is not initial
 *      RT_ERR_QUEUE_NUM - Invalid queue number
 * Applicable:
 *      8390
 * Note:
 *      (1) The valid range of queue_num is 1~8
 * Changes:
 *      None
 */
extern int32
rtk_qos_queueNum_set(uint32 unit, uint32 queue_num);

/* Function Name:
 *      rtk_qos_priMap_get
 * Description:
 *      Get the value of internal priority to QID mapping table.
 * Input:
 *      unit      - unit id
 *      queue_num - the number of queue (1~8).
 * Output:
 *      pPri2qid  - array of internal priority on a queue
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_QUEUE_NUM    - Invalid queue number
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390
 * Note:
 *      (1) The valid range of queue_num is 1~8
 *      (2) Change to use rtk_qos_pri2QidMap_get(unit, pri, pQid), queue_num is 8 for 9300,9310
 * Changes:
 *      None
 */
extern int32
rtk_qos_priMap_get(uint32 unit, uint32 queue_num, rtk_qos_pri2queue_t *pPri2qid);

/* Function Name:
 *      rtk_qos_priMap_set
 * Description:
 *      Set the value of internal priority to QID mapping table.
 * Input:
 *      unit      - unit id
 *      queue_num - the number of queue (1~8).
 *      pPri2qid  - array of internal priority on a queue
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_QUEUE_NUM    - Invalid queue number
 *      RT_ERR_QUEUE_ID     - Invalid queue ID
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390
 * Note:
 *      (1) Below is an example of internal priority to QID mapping table.
 *          When queue numbers are 8, the pri2qid are pri2qid[0]=0, pri2qid[1]=1, pri2qid[2]=2..., etc.
 *
 *          -            Number of Available Output Queue
 *          -  Priority  1   2   3   4   5   6   7   8
 *          -        0   0   0   0   0   0   0   0   0
 *          -        1   0   0   0   0   0   0   0   1
 *          -        2   0   0   0   1   1   1   1   2
 *          -        3   0   0   0   1   1   2   2   3
 *          -        4   0   1   1   2   2   3   3   4
 *          -        5   0   1   1   2   3   4   4   5
 *          -        6   0   1   2   3   4   5   5   6
 *          -        7   0   1   2   3   4   5   6   7
 *      (2) Change to use rtk_qos_pri2QidMap_set(unit, pri, qid), queue_num is 8 for 9300,9310
 * Changes:
 *      None
 */
extern int32
rtk_qos_priMap_set(uint32 unit, uint32 queue_num, rtk_qos_pri2queue_t *pPri2qid);

/* Function Name:
 *      rtk_qos_pri2QidMap_get
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
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_qos_pri2QidMap_get(uint32 unit, rtk_pri_t pri, rtk_qid_t *pQid);

/* Function Name:
 *      rtk_qos_pri2QidMap_set
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
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_qos_pri2QidMap_set(uint32 unit, rtk_pri_t pri, rtk_qid_t qid);

/* Function Name:
 *      rtk_qos_cpuQid2QidMap_get
 * Description:
 *      Get CPU QID to normal port QID mapping value.
 * Input:
 *      unit      - unit id
 *      cpuQid    - queue id of CPU port
 * Output:
 *      pQid      - queue id of normal port
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
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
rtk_qos_cpuQid2QidMap_get(uint32 unit, rtk_qid_t cpuQid, rtk_qid_t *pQid);

/* Function Name:
 *      rtk_qos_cpuQid2QidMap_set
 * Description:
 *      Set CPU QID to normal port QID mapping value.
 * Input:
 *      unit      - unit id
 *      cpuQid    - queue id of CPU port
 *      qid       - queue id of normal port
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_QUEUE_ID     - Invalid queue ID
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
rtk_qos_cpuQid2QidMap_set(uint32 unit, rtk_qid_t cpuQid, rtk_qid_t qid);

/* Function Name:
 *      rtk_qos_cpuQid2StackQidMap_get
 * Description:
 *      Get CPU QID to stacking port QID mapping value.
 * Input:
 *      unit      - unit id
 *      cpuQid    - queue id of CPU port
 * Output:
 *      pQid      - queue id of stacking port
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
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
rtk_qos_cpuQid2StackQidMap_get(uint32 unit, rtk_qid_t cpuQid, rtk_qid_t *pQid);

/* Function Name:
 *      rtk_qos_cpuQid2StackQidMap_set
 * Description:
 *      Set CPU QID to stacking port QID mapping value.
 * Input:
 *      unit      - unit id
 *      cpuQid    - queue id of CPU port
 *      qid       - queue id of stacking port
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_QUEUE_ID     - Invalid queue ID
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
rtk_qos_cpuQid2StackQidMap_set(uint32 unit, rtk_qid_t cpuQid, rtk_qid_t qid);


/* Module Name    : QoS           */
/* Sub-module Name: Egress remark */

/* Function Name:
 *      rtk_qos_port1pRemarkEnable_get
 * Description:
 *      Get 802.1p remark status for a port
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - status of 802.1p remark
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      The status of 802.1p remark:
 *      - DISABLED
 *      - ENABLED
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_qos_port1pRemarkEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_qos_port1pRemarkEnable_set
 * Description:
 *      Set 802.1p remark status for a port
 * Input:
 *      unit   - unit id
 *      port   - port id.
 *      enable - status of 802.1p remark
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - Invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - Invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      The status of 802.1p remark:
 *      - DISABLED
 *      - ENABLED
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_qos_port1pRemarkEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      rtk_qos_1pRemarking_get
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
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Supported remarking source is as following:
 *      - rtk_qos_1pRmkSrc_t \ Chip:            8380    8390    9300    9310
 *      - DOT_1P_RMK_SRC_INT_PRI                O       O       O       O
 *      - DOT_1P_RMK_SRC_USER_PRI               O       O       O       O
 *      - DOT_1P_RMK_SRC_OUTER_USER_PRI         X       O       O       O
 *      - DOT_1P_RMK_SRC_DSCP                   X       O       O       O
 * Changes:
 *      [SDK_3.0.0]
 *          (1)New added function.
 *          (2)It obsoletes these functions:
 *              rtk_qos_1pRemark_get
 *              rtk_qos_dscp2Dot1pRemark_get
 */
extern int32
rtk_qos_1pRemarking_get(uint32 unit, rtk_qos_1pRmkSrc_t src, rtk_qos_1pRmkVal_t val, rtk_pri_t *pPri);

/* Function Name:
 *      rtk_qos_1pRemarking_set
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
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Supported remarking source is as following:
 *      - rtk_qos_1pRmkSrc_t \ Chip:            8380    8390    9300    9310
 *      - DOT_1P_RMK_SRC_INT_PRI                O       O       O       O
 *      - DOT_1P_RMK_SRC_USER_PRI               O       O       O       O
 *      - DOT_1P_RMK_SRC_OUTER_USER_PRI         X       O       O       O
 *      - DOT_1P_RMK_SRC_DSCP                   X       O       O       O
 * Changes:
 *      [SDK_3.0.0]
 *          (1)New added function.
 *          (2)It obsoletes these functions:
 *              rtk_qos_1pRemark_set
 *              rtk_qos_dscp2Dot1pRemark_set
 */
extern int32
rtk_qos_1pRemarking_set(uint32 unit, rtk_qos_1pRmkSrc_t src, rtk_qos_1pRmkVal_t val, rtk_pri_t pri);

/* Function Name:
 *      rtk_qos_1pRemarkSrcSel_get
 * Description:
 *      Get remarking source of dot1p remarking.
 * Input:
 *      unit  - unit id
 * Output:
 *      pType - remarking source
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_qos_1pRemarkSrcSel_get(uint32 unit, rtk_qos_1pRmkSrc_t *pType);

/* Function Name:
 *      rtk_qos_1pRemarkSrcSel_set
 * Description:
 *      Set remarking source of dot1p remarking.
 * Input:
 *      unit - unit id
 *      type - remarking source
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      In 8390/9300/9310, 802.1p remark functionality can map the internal priority, original 802.1p
 *      priority, original outer priority or DSCP to 802.1p priority before a packet is going
 *      to be transmited.
 *      In 8380, 802.1p remark functionality can map the internal priority or original 802.1p
 *      priority to 802.1p priority before a packet is going to be transmited.
 * Changes:
 *      None
 */
extern int32
rtk_qos_1pRemarkSrcSel_set(uint32 unit, rtk_qos_1pRmkSrc_t type);

/* Function Name:
 *      rtk_qos_port1pDfltPri_get
 * Description:
 *      Get default inner-priority of specified port
 * Input:
 *      unit  - unit id
 *      port  - port id
 * Output:
 *      pPri    - pointer of default dot1p priority priority
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      None.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_qos_port1pDfltPri_get(uint32 unit, rtk_port_t port, rtk_pri_t *pPri);

/* Function Name:
 *      rtk_qos_port1pDfltPri_set
 * Description:
 *      Set default inner-priority of specified port
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      pri     - default dot1p priority priority
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_qos_port1pDfltPri_set(uint32 unit, rtk_port_t port, rtk_pri_t pri);

/* Function Name:
 *      rtk_qos_port1pDfltPriExt_get
 * Description:
 *      Get default inner-priority of specified port
 * Input:
 *      unit    - unit id
 *      devID - device id
 *      port    - port id
 * Output:
 *      pPri    - pointer of default dot1p priority priority
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_DEV_ID       - invalid device id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Applicable:
 *      9300, 9310
 * Note:
 *      None.
 * Changes:
 *      [3.2.0]
 *          New parameter name.
 */
extern int32
rtk_qos_port1pDfltPriExt_get(uint32 unit, uint32 devID, rtk_port_t port, rtk_pri_t *pPri);

/* Function Name:
 *      rtk_qos_port1pDfltPriExt_set
 * Description:
 *      Set default inner-priority of specified port
 * Input:
 *      unit    - unit id
 *      devID - device id
 *      port    - port id
 *      pri     - default dot1p priority priority
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_DEV_ID       - invalid device id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      9300, 9310
 * Note:
 *      None.
 * Changes:
 *      [3.2.0]
 *          New parameter name.
 */
extern int32
rtk_qos_port1pDfltPriExt_set(uint32 unit, uint32 devID, rtk_port_t port, rtk_pri_t pri);

/* Function Name:
 *      rtk_qos_port1pDfltPriSrcSel_get
 * Description:
 *      Get default inner-priority source of specified port
 * Input:
 *      unit  - unit id
 *      port  - port id
 * Output:
 *      pSrc    - pointer of default dot1p priority source
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      Supported default priority source is as following:
 *      - rtk_qos_1pDfltPriSrc_t \ Chip:        9300    9310
 *      - INNER_1P_DFLT_SRC_INT_PRI             O       O
 *      - INNER_1P_DFLT_SRC_DFLT_PRI            O       O
 *      - INNER_1P_DFLT_SRC_PB_PRI              X       X
 *      - INNER_1P_DFLT_SRC_PB_INNER_PRI        X       X
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_qos_port1pDfltPriSrcSel_get(uint32 unit, rtk_port_t port, rtk_qos_1pDfltPriSrc_t *pSrc);

/* Function Name:
 *      rtk_qos_port1pDfltPriSrcSel_set
 * Description:
 *      Set default inner-priority source of specified port
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      src     - default dot1p priority source
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      9300, 9310
 * Note:
 *      Supported default priority source is as following:
 *      - rtk_qos_1pDfltPriSrc_t \ Chip:        9300    9310
 *      - INNER_1P_DFLT_SRC_INT_PRI             O       O
 *      - INNER_1P_DFLT_SRC_DFLT_PRI            O       O
 *      - INNER_1P_DFLT_SRC_PB_PRI              X       X
 *      - INNER_1P_DFLT_SRC_PB_INNER_PRI        X       X
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_qos_port1pDfltPriSrcSel_set(uint32 unit, rtk_port_t port, rtk_qos_1pDfltPriSrc_t src);

/* Function Name:
 *      rtk_qos_port1pDfltPriSrcSelExt_get
 * Description:
 *      Get default inner-priority source of specified port
 * Input:
 *      unit    - unit id
 *      devID - device id
 *      port    - port id
 * Output:
 *      pSrc    - pointer of default dot1p priority source
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_DEV_ID       - invalid device id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Applicable:
 *      9300, 9310
 * Note:
 *      1. The API works in Stacking architecture.
 *      2. Supported default priority sources are as following:
 *      - INNER_1P_DFLT_SRC_INT_PRI
 *      - INNER_1P_DFLT_SRC_DFLT_PRI
 * Changes:
 *      [3.2.0]
 *          New parameter name.
 */
extern int32
rtk_qos_port1pDfltPriSrcSelExt_get(uint32 unit, uint32 devID, rtk_port_t port, rtk_qos_1pDfltPriSrc_t *pSrc);

/* Function Name:
 *      rtk_qos_port1pDfltPriSrcSelExt_set
 * Description:
 *      Set default inner-priority source of specified port
 * Input:
 *      unit    - unit id
 *      devID - device id
 *      port    - port id
 *      src     - default dot1p priority source
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_DEV_ID       - invalid device id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      9300, 9310
 * Note:
 *      1. The API works in Stacking architecture.
 *      2. Supported default priority sources are as following:
 *      - INNER_1P_DFLT_SRC_INT_PRI
 *      - INNER_1P_DFLT_SRC_DFLT_PRI
 * Changes:
 *      [3.2.0]
 *          New parameter name.
 */
extern int32
rtk_qos_port1pDfltPriSrcSelExt_set(uint32 unit, uint32 devID, rtk_port_t port, rtk_qos_1pDfltPriSrc_t src);

/* Function Name:
 *      rtk_qos_1pDfltPriCfgSrcSel_get
 * Description:
 *      Get default inner priority configuration
 * Input:
 *      unit  - unit id
 * Output:
 *      pCfg  - pointer of default dot1p priority
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      None.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_qos_1pDfltPriCfgSrcSel_get(uint32 unit, rtk_qos_1pDfltCfgSrc_t *pCfg);

/* Function Name:
 *      rtk_qos_1pDfltPriCfgSrcSel_set
 * Description:
 *      Set default inner priority configuration
 * Input:
 *      unit - unit id
 *      cfg  - default dot1p priority
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      9300, 9310
 * Note:
 *      None.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_qos_1pDfltPriCfgSrcSel_set(uint32 unit, rtk_qos_1pDfltCfgSrc_t cfg);

/* Function Name:
 *      rtk_qos_1pDfltPriSrcSel_get
 * Description:
 *      Get default inner-priority source
 * Input:
 *      unit  - unit id
 * Output:
 *      pType - pointer of default outer dot1p priority source
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None.
 * Changes:
 *      None
 */
extern int32
rtk_qos_1pDfltPriSrcSel_get(uint32 unit, rtk_qos_1pDfltPriSrc_t * pType);

/* Function Name:
 *      rtk_qos_1pDfltPriSrcSel_set
 * Description:
 *      Set default inner-priority source
 * Input:
 *      unit - unit id
 *      type - default outer dot1p priority source
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None.
 * Changes:
 *      None
 */
extern int32
rtk_qos_1pDfltPriSrcSel_set(uint32 unit, rtk_qos_1pDfltPriSrc_t type);

/* Function Name:
 *      rtk_qos_1pDfltPri_get
 * Description:
 *      Get default inner-priority value
 * Input:
 *      unit      - unit id
 * Output:
 *      pDot1p_pri - default 802.1p priority value (range from 0 ~ 7)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_qos_1pDfltPri_get(uint32 unit, rtk_pri_t *pDot1p_pri);

/* Function Name:
 *      rtk_qos_1pDfltPri_set
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
 *      RT_ERR_UNIT_ID         - Invalid unit id
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_QOS_1P_PRIORITY - Invalid dot1p priority
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      If inner-untag packets would be transmmited with inner-tag, system takes inner-tag default
 *      priority dot1p_pri to be inner-tag priority.
 * Changes:
 *      None
 */
extern int32
rtk_qos_1pDfltPri_set(uint32 unit, rtk_pri_t dot1p_pri);

/* Function Name:
 *      rtk_qos_portOut1pRemarkEnable_get
 * Description:
 *      Get enable status of outer dot1p remarking on specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id.
 * Output:
 *      pEnable - pointer to enable status of outer dot1p remarking
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_qos_out1pRemarkEnable_get)
 */
extern int32
rtk_qos_portOut1pRemarkEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_qos_portOut1pRemarkEnable_set
 * Description:
 *      Set enable status of outer dot1p remarking on specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id.
 *      enable - enable status of outer dot1p remarking
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_qos_out1pRemarkEnable_set)
 */
extern int32
rtk_qos_portOut1pRemarkEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      rtk_qos_portOuter1pRemarkSrcSel_get
 * Description:
 *      Get the remarking source of outer dot1p priority remarking.
 * Input:
 *      unit  - unit id
 *      port  - port id
 * Output:
 *      pType - remarking source
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380
 * Note:
 *      None.
 * Changes:
 *      None
 */
extern int32
rtk_qos_portOuter1pRemarkSrcSel_get(uint32 unit, rtk_port_t port, rtk_qos_outer1pRmkSrc_t * pType);

/* Function Name:
 *      rtk_qos_portOuter1pRemarkSrcSel_set
 * Description:
 *      Set the remarking source of outer dot1p priority remarking.
 * Input:
 *      unit - unit id
 *      port - port id
 *      type - remarking source
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380
 * Note:
 *      None.
 * Changes:
 *      None
 */
extern int32
rtk_qos_portOuter1pRemarkSrcSel_set(uint32 unit, rtk_port_t port, rtk_qos_outer1pRmkSrc_t type);

/* Function Name:
 *      rtk_qos_outer1pRemarking_get
 * Description:
 *      Get the internal priority/original inner priority/original outer priority/dscp to
 *      remarkd outer 802.1p priority(3bits) mapping.
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
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Supported remarking source is as following:
 *      - rtk_qos_outer1pRmkSrc_t \ Chip:       8380    8390    9300    9310
 *      - OUTER_1P_RMK_SRC_INT_PRI              O       O       O       O
 *      - OUTER_1P_RMK_SRC_USER_PRI             O       O       O       O
 *      - OUTER_1P_RMK_SRC_INNER_USER_PRI       X       O       O       O
 *      - OUTER_1P_RMK_SRC_DSCP                 X       O       O       O
 * Changes:
 *      [SDK_3.0.0]
 *          (1)New added function.
 *          (2)It obsoletes these functions:
 *              rtk_qos_dscp2Outer1pRemark_get
 *              rtk_qos_outer1pRemark_get
 */
extern int32
rtk_qos_outer1pRemarking_get(uint32 unit, rtk_qos_outer1pRmkSrc_t src, rtk_qos_outer1pRmkVal_t val, rtk_pri_t *pPri);

/* Function Name:
 *      rtk_qos_outer1pRemarking_set
 * Description:
 *      Set the internal priority/original inner priority/original outer priority/dscp to
 *      remarkd outer 802.1p priority(3bits) mapping.
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
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Supported remarking source is as following:
 *      - rtk_qos_outer1pRmkSrc_t \ Chip:       8380    8390    9300    9310
 *      - OUTER_1P_RMK_SRC_INT_PRI              O       O       O       O
 *      - OUTER_1P_RMK_SRC_USER_PRI             O       O       O       O
 *      - OUTER_1P_RMK_SRC_INNER_USER_PRI       X       O       O       O
 *      - OUTER_1P_RMK_SRC_DSCP                 X       O       O       O
 * Changes:
 *      [SDK_3.0.0]
 *          (1)New added function.
 *          (2)It obsoletes these functions:
 *              rtk_qos_dscp2Outer1pRemark_set
 *              rtk_qos_outer1pRemark_set
 */
extern int32
rtk_qos_outer1pRemarking_set(uint32 unit, rtk_qos_outer1pRmkSrc_t src, rtk_qos_outer1pRmkVal_t val, rtk_pri_t pri);

/* Function Name:
 *      rtk_qos_outer1pRemarkSrcSel_get
 * Description:
 *      Get remarking source of outer dot1p remarking.
 * Input:
 *      unit  - unit id
 * Output:
 *      pType - remarking source
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      Outer 1P remarking source is either from internal priority or original outer 1P priority.
 *      Use 'rtk_qos_portOut1pRemarkEnable_set' to enable the outer 1P remarking function.
 * Changes:
 *      None
 */
extern int32
rtk_qos_outer1pRemarkSrcSel_get(uint32 unit, rtk_qos_outer1pRmkSrc_t *pType);

/* Function Name:
 *      rtk_qos_outer1pRemarkSrcSel_set
 * Description:
 *      Set remarking source of outer dot1p remarking.
 * Input:
 *      unit - unit id
 *      type - remarking source
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
 *      Outer 1P remarking source is either from internal priority or original outer 1P priority.
 *      Use 'rtk_qos_portOut1pRemarkEnable_set' to enable the outer 1P remarking function.
 * Changes:
 *      None
 */
extern int32
rtk_qos_outer1pRemarkSrcSel_set(uint32 unit, rtk_qos_outer1pRmkSrc_t type);

/* Function Name:
 *      rtk_qos_portOuter1pDfltPri_get
 * Description:
 *      Get default outer-priority of specified port
 * Input:
 *      unit  - unit id
 *      port  - port id
 * Output:
 *      pPri - pointer of default outer dot1p priority
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      None.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_qos_portOuter1pDfltPri_get(uint32 unit, rtk_port_t port, rtk_pri_t *pPri);

/* Function Name:
 *      rtk_qos_portOuter1pDfltPri_set
 * Description:
 *      Set default outer-priority of specified port
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      pri     - default outer dot1p priority
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      9300, 9310
 * Note:
 *      None.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_qos_portOuter1pDfltPri_set(uint32 unit, rtk_port_t port, rtk_pri_t pri);

/* Function Name:
 *      rtk_qos_portOuter1pDfltPriExt_get
 * Description:
 *      Get default outer-priority of specified port
 * Input:
 *      unit    - unit id
 *      devID - device id
 *      port    - port id
 * Output:
 *      pPri - pointer of default outer dot1p priority priority
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_DEV_ID       - invalid device id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      The API works in Stacking architecture.
 * Changes:
 *      [3.2.0]
 *          New parameter name.
 */
extern int32
rtk_qos_portOuter1pDfltPriExt_get(uint32 unit, uint32 devID, rtk_port_t port, rtk_pri_t *pPri);

/* Function Name:
 *      rtk_qos_portOuter1pDfltPriExt_set
 * Description:
 *      Set default outer-priority of specified port
 * Input:
 *      unit    - unit id
 *      devID - device id
 *      port    - port id
 *      pri     - default outer dot1p priority priority
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_DEV_ID       - invalid device id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      9300, 9310
 * Note:
 *      The API works in Stacking architecture.
 * Changes:
 *      [3.2.0]
 *          New parameter name.
 */
extern int32
rtk_qos_portOuter1pDfltPriExt_set(uint32 unit, uint32 devID, rtk_port_t port, rtk_pri_t pri);

/* Function Name:
 *      rtk_qos_portOuter1pDfltPriSrcSel_get
 * Description:
 *      Get default outer-priority source of specified port
 * Input:
 *      unit  - unit id
 *      port  - port id
 * Output:
 *      pType - type of default outer priority source
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      1. If received outer-untag packets would be transmmited with outer-tag, system takes outer-tag
 *         default priority source configuration of egress port to decide outer-tag priority.
 *      2. Supported default priority source is as following:
 *      - rtk_qos_outer1pDfltSrc_t \ Chip:      8380    8390    9300    9310
 *      - OUTER_1P_DFLT_SRC_INT_PRI             O       O       O       O
 *      - OUTER_1P_DFLT_SRC_USER_PRI            O       O       O       O
 *      - OUTER_1P_DFLT_SRC_DFLT_PRI            O       O       O       O
 *      - OUTER_1P_DFLT_SRC_PB_PRI              X       O       X       X
 *      - OUTER_1P_DFLT_SRC_PB_OUTER_PRI        O       X       X       X
 * Changes:
 *      None
 */
extern int32
rtk_qos_portOuter1pDfltPriSrcSel_get(uint32 unit, rtk_port_t port, rtk_qos_outer1pDfltSrc_t *pType);

/* Function Name:
 *      rtk_qos_portOuter1pDfltPriSrcSel_set
 * Description:
 *      Set default outer-priority source of specified port
 * Input:
 *      unit - unit id
 *      port - port id
 *      type - default outer priority source
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
 *      8380, 8390, 9300, 9310
 * Note:
 *      1. If received outer-untag packets would be transmmited with outer-tag, system takes outer-tag
 *         default priority source configuration of egress port to decide outer-tag priority.
 *      2. Supported default priority source is as following:
 *      - rtk_qos_outer1pDfltSrc_t \ Chip:      8380    8390    9300    9310
 *      - OUTER_1P_DFLT_SRC_INT_PRI             O       O       O       O
 *      - OUTER_1P_DFLT_SRC_USER_PRI            O       O       O       O
 *      - OUTER_1P_DFLT_SRC_DFLT_PRI            O       O       O       O
 *      - OUTER_1P_DFLT_SRC_PB_PRI              X       O       X       X
 *      - OUTER_1P_DFLT_SRC_PB_OUTER_PRI        O       X       X       X
 * Changes:
 *      None
 */
extern int32
rtk_qos_portOuter1pDfltPriSrcSel_set(uint32 unit, rtk_port_t port, rtk_qos_outer1pDfltSrc_t type);

/* Function Name:
 *      rtk_qos_portOuter1pDfltPriSrcSelExt_get
 * Description:
 *      Get default outer-priority source of specified port
 * Input:
 *      unit  - unit id
 *      devID - device id
 *      port  - port id
 * Output:
 *      pType - type of default outer priority source
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_DEV_ID       - invalid device id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      1. The API works in Stacking architecture.
 *      2. If received outer-untag packets would be transmmited with outer-tag, system takes outer-tag
 *          default priority source configuration of egress port to decide outer-tag priority.
 *      3. Supported default priority source is as following:
 *      - rtk_qos_outer1pDfltSrc_t \ Chip:      9300    9310
 *      - OUTER_1P_DFLT_SRC_INT_PRI             O       O
 *      - OUTER_1P_DFLT_SRC_USER_PRI            O       O
 *      - OUTER_1P_DFLT_SRC_DFLT_PRI            O       O
 *      - OUTER_1P_DFLT_SRC_PB_PRI              X       X
 *      - OUTER_1P_DFLT_SRC_PB_OUTER_PRI        X       X
 * Changes:
 *      [3.2.0]
 *          New parameter name.
 */
extern int32
rtk_qos_portOuter1pDfltPriSrcSelExt_get(uint32 unit, uint32 devID, rtk_port_t port, rtk_qos_outer1pDfltSrc_t *pType);

/* Function Name:
 *      rtk_qos_portOuter1pDfltPriSrcSelExt_set
 * Description:
 *      Set default outer-priority source of specified port
 * Input:
 *      unit - unit id
 *      devID - device id
 *      port - port id
 *      type - default outer priority source
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_DEV_ID       - invalid device id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      9300, 9310
 * Note:
 *      1. The API works in Stacking architecture.
 *      2. If received outer-untag packets would be transmmited with outer-tag, system takes outer-tag
 *          default priority source configuration of egress port to decide outer-tag priority.
 *      3. Supported default priority source is as following:
 *      - rtk_qos_outer1pDfltSrc_t \ Chip:      9300    9310
 *      - OUTER_1P_DFLT_SRC_INT_PRI             O       O
 *      - OUTER_1P_DFLT_SRC_USER_PRI            O       O
 *      - OUTER_1P_DFLT_SRC_DFLT_PRI            O       O
 *      - OUTER_1P_DFLT_SRC_PB_PRI              X       X
 *      - OUTER_1P_DFLT_SRC_PB_OUTER_PRI        X       X
 * Changes:
 *      [3.2.0]
 *          New parameter name.
 */
extern int32
rtk_qos_portOuter1pDfltPriSrcSelExt_set(uint32 unit, uint32 devID, rtk_port_t port, rtk_qos_outer1pDfltSrc_t type);

/* Function Name:
 *      rtk_qos_outer1pDfltPriCfgSrcSel_get
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
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None.
 * Changes:
 *      None
 */
extern int32
rtk_qos_outer1pDfltPriCfgSrcSel_get(uint32 unit, rtk_qos_outer1pDfltCfgSrc_t *pDflt_sel);

/* Function Name:
 *      rtk_qos_outer1pDfltPriCfgSrcSel_set
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
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      For outer-untag packet, the default TX outer-priority can refer the configuration
 *      of RX port or TX port by the API.
 * Changes:
 *      None
 */
extern int32
rtk_qos_outer1pDfltPriCfgSrcSel_set(uint32 unit, rtk_qos_outer1pDfltCfgSrc_t dflt_sel);

/* Function Name:
 *      rtk_qos_outer1pDfltPri_get
 * Description:
 *      Get default outer-priority value
 * Input:
 *      unit       - unit id
 * Output:
 *      pDot1p_pri - default 802.1p priority value (range from 0 ~ 7)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None.
 * Changes:
 *      None
 */
extern int32
rtk_qos_outer1pDfltPri_get(uint32 unit, rtk_pri_t * pDot1p_pri);

/* Function Name:
 *      rtk_qos_outer1pDfltPri_set
 * Description:
 *      Set default outer -priority value
 * Input:
 *      unit      - unit id
 *      dot1p_pri - default 802.1p priority value (range from 0 ~ 7)
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_QOS_1P_PRIORITY - Invalid dot1p priority
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None.
 * Changes:
 *      None
 */
extern int32
rtk_qos_outer1pDfltPri_set(uint32 unit, rtk_pri_t dot1p_pri);

/* Function Name:
 *      rtk_qos_portDscpRemarkEnable_get
 * Description:
 *      Get DSCP remark status for a port
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - status of DSCP remark
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      The status of DSCP remark:
 *      - DISABLED
 *      - ENABLED
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_qos_dscpRemarkEnable_get)
 */
extern int32
rtk_qos_portDscpRemarkEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_qos_portDscpRemarkEnable_set
 * Description:
 *      Set DSCP remark status for a port
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - status of DSCP remark
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - Invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - Invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      The status of DSCP remark:
 *      - DISABLED
 *      - ENABLED
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_qos_dscpRemarkEnable_set)
 */
extern int32
rtk_qos_portDscpRemarkEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);


/* Function Name:
 *      rtk_qos_dscpRemarking_get
 * Description:
 *      Get the internal priority/original inner priority/original outer priority/dscp/dp/internal priority and dp to
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
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Supported remarking source is as following:
 *      - rtk_qos_dscpRmkSrc_t \ Chip:          8380    8390    9300    9310
 *      - DSCP_RMK_SRC_INT_PRI                  O       O       O       O
 *      - DSCP_RMK_SRC_DSCP                     O       O       O       O
 *      - DSCP_RMK_SRC_DP                       X       X       X       X
 *      - DSCP_RMK_SRC_USER_PRI                 X       O       O       O
 *      - DSCP_RMK_SRC_OUTER_USER_PRI           X       O       O       O
 *      - DSCP_RMK_SRC_DP_INT_PRI               X       X       O       O
 * Changes:
 *      [SDK_3.0.0]
 *          (1)New added function.
 *          (2)It obsoletes these functions:
 *              rtk_qos_dscp2DscpRemark_get
 *              rtk_qos_dscpRemark_get
 */
extern int32
rtk_qos_dscpRemarking_get(uint32 unit, rtk_qos_dscpRmkSrc_t src, rtk_qos_dscpRmkVal_t val, uint32 *pDscp);

/* Function Name:
 *      rtk_qos_dscpRemarking_set
 * Description:
 *      Set the internal priority/original inner priority/original outer priority/dscp/dp/internal priority and dp to
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
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Supported remarking source is as following:
 *      - rtk_qos_dscpRmkSrc_t \ Chip:          8380    8390    9300    9310
 *      - DSCP_RMK_SRC_INT_PRI                  O       O       O       O
 *      - DSCP_RMK_SRC_DSCP                     O       O       O       O
 *      - DSCP_RMK_SRC_DP                       X       X       X       X
 *      - DSCP_RMK_SRC_USER_PRI                 X       O       O       O
 *      - DSCP_RMK_SRC_OUTER_USER_PRI           X       O       O       O
 *      - DSCP_RMK_SRC_DP_INT_PRI               X       X       O       O
 * Changes:
 *      [SDK_3.0.0]
 *          (1)New added function.
 *          (2)It obsoletes these functions:
 *              rtk_qos_dscp2DscpRemark_set
 *              rtk_qos_dscpRemark_set
 */
extern int32
rtk_qos_dscpRemarking_set(uint32 unit, rtk_qos_dscpRmkSrc_t src, rtk_qos_dscpRmkVal_t val, uint32 dscp);

/* Function Name:
 *      rtk_qos_dscpRemarkSrcSel_get
 * Description:
 *      Get remarking source of DSCP remarking.
 * Input:
 *      unit  - unit id
 * Output:
 *      pType - remarking source
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      After specifing the remarking source, please use the corresponding API to specify the remarking mapping.
 * Changes:
 *      None
 */
extern int32
rtk_qos_dscpRemarkSrcSel_get(uint32 unit, rtk_qos_dscpRmkSrc_t *pType);

/* Function Name:
 *      rtk_qos_dscpRemarkSrcSel_set
 * Description:
 *      Set remarking source of DSCP remarking.
 * Input:
 *      unit - unit id
 *      type - remarking source
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      After specifing the remarking source, please use the corresponding API to specify the remarking mapping.
 * Changes:
 *      None
 */
extern int32
rtk_qos_dscpRemarkSrcSel_set(uint32 unit, rtk_qos_dscpRmkSrc_t type);

/* Function Name:
 *      rtk_qos_portDeiRemarkEnable_get
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
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *    The status of DEI remark:
 *    - DISABLED
 *    - ENABLED
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_qos_deiRemarkEnable_get)
 */
 extern int32
rtk_qos_portDeiRemarkEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_qos_portDeiRemarkEnable_set
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
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - Invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *    The status of DEI remark:
 *    - DISABLED
 *    - ENABLED
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_qos_deiRemarkEnable_set)
 */
extern int32
rtk_qos_portDeiRemarkEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      rtk_qos_portDeiRemarkTagSel_get
 * Description:
 *      Get DEI remarking VLAN tag selection of specified port.
 * Input:
 *      unit  - unit id
 *      port  - port id
 * Output:
 *      pType - type of DEI
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
 *      For DEI remark function, per egress port can select outer-tag or inner-tag to do DEI remarking.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_qos_portDeiRemarkTagSel_get(uint32 unit, rtk_port_t port, rtk_qos_deiSel_t *pType);

/* Function Name:
 *      rtk_qos_portDeiRemarkTagSel_set
 * Description:
 *      Set DEI remarking VLAN tag selection of specified port.
 * Input:
 *      unit - unit id
 *      port - port id
 *      type - type of DEI
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
 *      For DEI remark function, per egress port can select outer-tag or inner-tag to do DEI remarking.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_qos_portDeiRemarkTagSel_set(uint32 unit, rtk_port_t port, rtk_qos_deiSel_t type);

/* Function Name:
 *      rtk_qos_deiRemarking_get
 * Description:
 *      Get the internal priority/dp to DEI remarking mapping.
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
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      Supported remarking source is as following:
 *      - rtk_qos_deiRmkSrc_t \ Chip:       8390    9300    9310
 *      - DEI_RMK_SRC_INT_PRI               X       O       O
 *      - DEI_RMK_SRC_DP                    O       O       O
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_qos_deiRemark_get)
 */
extern int32
rtk_qos_deiRemarking_get(uint32 unit, rtk_qos_deiRmkSrc_t src, rtk_qos_deiRmkVal_t val, uint32 *pDei);

/* Function Name:
 *      rtk_qos_deiRemarking_set
 * Description:
 *      Set the internal priority/dp to DEI remarking mapping.
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
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      Supported remarking source is as following:
 *      - rtk_qos_deiRmkSrc_t \ Chip:       8390    9300    9310
 *      - DEI_RMK_SRC_INT_PRI               X       O       O
 *      - DEI_RMK_SRC_DP                    O       O       O
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_qos_deiRemark_set)
 */
extern int32
rtk_qos_deiRemarking_set(uint32 unit, rtk_qos_deiRmkSrc_t src, rtk_qos_deiRmkVal_t val, uint32 dei);

/* Function Name:
 *      rtk_qos_deiRemarkSrcSel_get
 * Description:
 *      Get remarking source of DEI remarking.
 * Input:
 *      unit  - unit id
 * Output:
 *      pType - remarking source
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      After specifing the remarking source, please use the corresponding API to specify the remarking mapping.
 * Changes:
 *      None
 */
extern int32
rtk_qos_deiRemarkSrcSel_get(uint32 unit, rtk_qos_deiRmkSrc_t *pType);

/* Function Name:
 *      rtk_qos_deiRemarkSrcSel_set
 * Description:
 *      Set remarking source of DEI remarking.
 * Input:
 *      unit - unit id
 *      type - remarking source
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      9300, 9310
 * Note:
 *      After specifing the remarking source, please use the corresponding API to specify the remarking mapping.
 * Changes:
 *      None
 */
extern int32
rtk_qos_deiRemarkSrcSel_set(uint32 unit, rtk_qos_deiRmkSrc_t type);

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
/* Function Name:
 *      rtk_qos_1pRemark_get
 * Description:
 *      Get the internal priority (3bits) to remarkd 802.1p priority(3bits) mapping.
 * Input:
 *      unit       - unit id
 *      int_pri    - internal priority value (range from 0 ~ 7)
 * Output:
 *      pDot1p_pri - remarked 802.1p priority value (range from 0 ~ 7)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - Invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_qos_1pRemarking_get(unit, src, val, *pPri)
 *          Parameters:
 *              src                     - DOT_1P_RMK_SRC_INT_PRI
 *              val                     - int_pri
 *              pPri                   - pPri
 */
extern int32
rtk_qos_1pRemark_get(uint32 unit, rtk_pri_t int_pri, rtk_pri_t *pDot1p_pri);

/* Function Name:
 *      rtk_qos_1pRemark_set
 * Description:
 *      Set the internal priority(3bits) to remarked 802.1p priority(3bits) mapping.
 * Input:
 *      unit      - unit id
 *      int_pri   - internal priority value (range from 0 ~ 7)
 *      dot1p_pri - remarked 802.1p priority value (range from 0 ~ 7)
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - Invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority
 *      RT_ERR_QOS_1P_PRIORITY  - Invalid dot1p priority
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      For 8390, int_pri can be internal priority or original inner-priority, and it dependents
 *      on the configuration of API rtk_qos_1pRemarkSrcSel_set().
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_qos_1pRemarking_set(unit, src, val, pri)
 *          Parameters:
 *              src                     - DOT_1P_RMK_SRC_INT_PRI
 *              val                     - int_pri
 *              pri                   - dot1p_pri
 */
extern int32
rtk_qos_1pRemark_set(uint32 unit, rtk_pri_t int_pri, rtk_pri_t dot1p_pri);


/* Function Name:
 *      rtk_qos_outer1pRemark_get
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
 *      RT_ERR_UNIT_ID          - invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_qos_outer1pRemarking_get(unit, src, val, pPri) for 9300,9310
 */
extern int32
rtk_qos_outer1pRemark_get(uint32 unit, rtk_pri_t int_pri, rtk_pri_t *pDot1p_pri);

/* Function Name:
 *      rtk_qos_outer1pRemark_set
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
 *      RT_ERR_UNIT_ID          - invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority
 *      RT_ERR_QOS_1P_PRIORITY  - Invalid dot1p priority
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      For 8390/8380, int_pri can be internal priority or original outer-priority, and it dependents on the
 *      configuration of API rtk_qos_outer1pRemarkSrcSel_set.
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_qos_outer1pRemarking_set(unit, src, val, pri) for 9300,9310
 */
extern int32
rtk_qos_outer1pRemark_set(uint32 unit, rtk_pri_t int_pri, rtk_pri_t dot1p_pri);

/* Function Name:
 *      rtk_qos_dscpRemark_get
 * Description:
 *      Get the internal priority (3bits) to DSCP remarking mapping.
 * Input:
 *      unit    - unit id
 *      int_pri - internal priority value (range from 0 ~ 7)
 * Output:
 *      pDscp   - remarked DSCP value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - Invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) Remark DSCP using the internal priority.
 *      (2) The DSCP remarking source is specified by 'rtk_qos_dscpRemarkSrcSel_set'.
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_qos_dscpRemarking_get(unit, src, val, *pDscp)
 *          Parameters:
 *              src                     - DSCP_RMK_SRC_INT_PRI
 *              val                     - int_pri
 *              pDscp                - pDscp
 */
extern int32
rtk_qos_dscpRemark_get(uint32 unit, rtk_pri_t int_pri, uint32 *pDscp);

/* Function Name:
 *      rtk_qos_dscpRemark_set
 * Description:
 *      Set the internal priority (3bits) to DSCP remarking mapping.
 * Input:
 *      unit    - unit id
 *      int_pri - internal priority value (range from 0 ~ 7)
 *      dscp    - remarked DSCP value
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - Invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority
 *      RT_ERR_QOS_DSCP_VALUE   - Invalid dscp value
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) Remark DSCP using the internal priority.
 *      (2) The DSCP remarking source is specified by 'rtk_qos_dscpRemarkSrcSel_set'.
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_qos_dscpRemarking_set(unit, src, val, dscp)
 *          Parameters:
 *              src                     - DSCP_RMK_SRC_INT_PRI
 *              val                     - int_pri
 *              dscp                   - dscp
 */
extern int32
rtk_qos_dscpRemark_set(uint32 unit, rtk_pri_t int_pri, uint32 dscp);

/* Function Name:
 *      rtk_qos_dscp2DscpRemark_get
 * Description:
 *      Get DSCP to DSCP remarking mapping.
 * Input:
 *      unit  - unit id
 *      dscp  - DSCP value
 * Output:
 *      pDscp - remarked DSCP value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - Invalid unit id
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_QOS_DSCP_VALUE - Invalid dscp value
 *      RT_ERR_NULL_POINTER   - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) Remark DSCP using the original DSCP.
 *      (2) The DSCP remarking source is specified by 'rtk_qos_dscpRemarkSrcSel_set'.
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_qos_dscpRemarking_get(unit, src, val, *pDscp)
 *          Parameters:
 *              src                     - DSCP_RMK_SRC_DSCP
 *              val                     - dscp
 *              pDscp                - pDscp
 */
extern int32
rtk_qos_dscp2DscpRemark_get(uint32 unit, uint32 dscp, uint32 *pDscp);

/* Function Name:
 *      rtk_qos_dscp2DscpRemark_set
 * Description:
 *      Set DSCP to DSCP remarking mapping.
 * Input:
 *      unit    - unit id
 *      dscp    - DSCP value
 *      rmkDscp - remarked DSCP value
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - Invalid unit id
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_QOS_DSCP_VALUE - Invalid dscp value
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) Remark DSCP using the original DSCP.
 *      (2) The DSCP remarking source is specified by 'rtk_qos_dscpRemarkSrcSel_set'.
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_qos_dscpRemarking_set(unit, src, val, dscp)
 *          Parameters:
 *              src                     - DSCP_RMK_SRC_DSCP
 *              val                     - dscp
 *              dscp                  - rmkDscp
 */
extern int32
rtk_qos_dscp2DscpRemark_set(uint32 unit, uint32 dscp, uint32 rmkDscp);

/* Function Name:
 *      rtk_qos_dscp2Dot1pRemark_get
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
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      None.
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_qos_1pRemarking_get(unit, src, val, pPri)
 *          Parameters:
 *              src                     - DOT_1P_RMK_SRC_DSCP
 *              val                     - dscp
 *              pPri                    - pDot1p_pri
 */
extern int32
rtk_qos_dscp2Dot1pRemark_get(uint32 unit, uint32 dscp, rtk_pri_t *pDot1p_pri);

/* Function Name:
 *      rtk_qos_dscp2Dot1pRemark_set
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
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      None.
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_qos_1pRemarking_set(unit, src, val,pri)
 *          Parameters:
 *              src                     - DOT_1P_RMK_SRC_DSCP
 *              val                     - dscp
 *              pri                     - dot1p_pri
 */
extern int32
rtk_qos_dscp2Dot1pRemark_set(uint32 unit, uint32 dscp, rtk_pri_t dot1p_pri);

/* Function Name:
 *      rtk_qos_dscp2Outer1pRemark_get
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
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      None.
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_qos_outer1pRemarking_get(unit, src, val, pPri)
 *          Parameters:
 *              src                     - DOT_1P_RMK_SRC_DSCP
 *              val                     - dscp
 *              pPri                    - pDot1p_pri
 */
extern int32
rtk_qos_dscp2Outer1pRemark_get(uint32 unit, uint32 dscp, rtk_pri_t *pDot1p_pri);

/* Function Name:
 *      rtk_qos_dscp2Outer1pRemark_set
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
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      None.
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_qos_outer1pRemarking_set(unit, src, val,pri)
 *          Parameters:
 *              src                   - DOT_1P_RMK_SRC_DSCP
 *              val                   - dscp
 *              pri                   - dot1p_pri
 */
extern int32
rtk_qos_dscp2Outer1pRemark_set(uint32 unit, uint32 dscp, rtk_pri_t dot1p_pri);

/* Function Name:
 *      rtk_qos_deiRemark_get
 * Description:
 *      Get the internal drop precedence to DEI remarking mapping.
 * Input:
 *      unit - unit id
 *      dp   - internal drop precedence (range from 0 ~ 2)
 * Output:
 *      pDei - remarked DEI value (range from 0 ~ 1)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID             - invalid unit id
 *      RT_ERR_NOT_INIT            - The module is not initial
 *      RT_ERR_QOS_DROP_PRECEDENCE - Invalid drop precedence
 *      RT_ERR_NULL_POINTER        - input parameter may be null pointer
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      Remark DEI using the internal drop precedence.
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_qos_deiRemarking_get(unit, src, val, pDei)
 *          Parameters:
 *              src                     - DEI_RMK_SRC_DP
 *              val                     - dp
 *              pDei                  - pDei
 */
extern int32
rtk_qos_deiRemark_get(uint32 unit, uint32 dp, uint32 *pDei);

/* Function Name:
 *      rtk_qos_deiRemark_set
 * Description:
 *      Set the internal drop precedence to DEI remarking mapping.
 * Input:
 *      unit - unit id
 *      dp   - internal drop precedence (range from 0 ~ 2)
 *      dei  - remarked DEI value (range from 0 ~ 1)
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID             - invalid unit id
 *      RT_ERR_NOT_INIT            - The module is not initial
 *      RT_ERR_QOS_DEI_VALUE       - Invalid DEI value
 *      RT_ERR_QOS_DROP_PRECEDENCE - Invalid drop precedence
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      Remark DEI using the internal drop precedence.
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_qos_deiRemarking_set(unit, src, val, dei)
 *          Parameters:
 *              src                     - DEI_RMK_SRC_DP
 *              val                     - dp
 *              dei                     - dei
 */
extern int32
rtk_qos_deiRemark_set(uint32 unit, uint32 dp, uint32 dei);
#endif

/* Module Name    : QoS              */
/* Sub-module Name: Queue scheduling */

/* Function Name:
 *      rtk_qos_schedulingAlgorithm_get
 * Description:
 *      Get the scheduling algorithm of the port.
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pScheduling_type - type of scheduling algorithm.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      The types of scheduling algorithm:
 *      - WFQ
 *      - WRR
 * Changes:
 *      None
 */
extern int32
rtk_qos_schedulingAlgorithm_get(
    uint32                      unit,
    rtk_port_t                  port,
    rtk_qos_scheduling_type_t   *pScheduling_type);

/* Function Name:
 *      rtk_qos_schedulingAlgorithm_set
 * Description:
 *      Set the scheduling algorithm of the port.
 * Input:
 *      unit            - unit id
 *      port            - port id
 *      scheduling_type - type of scheduling algorithm
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - Invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - Invalid port id
 *      RT_ERR_INPUT    - Invalid scheduling algorithm type
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      The types of scheduling algorithm:
 *      - WFQ
 *      - WRR
 * Changes:
 *      None
 */
extern int32
rtk_qos_schedulingAlgorithm_set(
    uint32                      unit,
    rtk_port_t                  port,
    rtk_qos_scheduling_type_t   scheduling_type);

/* Function Name:
 *      rtk_qos_schedulingQueue_get
 * Description:
 *      Get the scheduling types and weights of queues on specific port in egress scheduling.
 * Input:
 *      unit      - unit id
 *      port      - port id
 * Output:
 *      pQweights - the array of weights for WRR/WFQ queue
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      The types of queue are: WFQ_WRR_PRIORITY or STRICT_PRIORITY.
 *      In 8380:
 *          Valid range of weight is 0~127. 0 is used for STRICT_PRIORITY queue
 *          If the weight is 0 then the type is STRICT_PRIORITY, else the type is WFQ_WRR_PRIORITY.
 *      In 8390:
 *          Valid range of weight is 0~1023. 0 stands for STRICT_PRIORITY queue
 *          If the weight is 0 then the type is STRICT_PRIORITY, else the type is WFQ_WRR_PRIORITY.
 *      In 9300 and 9310:
 *          Valid range of weight is 1~127.
 * Changes:
 *      None
 */
extern int32
rtk_qos_schedulingQueue_get(uint32 unit, rtk_port_t port, rtk_qos_queue_weights_t *pQweights);

/* Function Name:
 *      rtk_qos_schedulingQueue_set
 * Description:
 *      Set the scheduling types and weights of queues on specific port in egress scheduling.
 * Input:
 *      unit      - unit id
 *      port      - port id
 *      pQweights - the array of weights for WRR/WFQ queue
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - Invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - Invalid port id
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_QOS_QUEUE_WEIGHT - Invalid queue weight
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      The types of queue are: WFQ_WRR_PRIORITY or STRICT_PRIORITY.
 *      In 8380:
 *          Valid range of weight is 0~127. 0 is used for STRICT_PRIORITY queue
 *          If the weight is 0 then the type is STRICT_PRIORITY, else the type is WFQ_WRR_PRIORITY.
 *      In 8390:
 *          Valid range of weight is 0~1023. 0 stands for STRICT_PRIORITY queue
 *          If the weight is 0 then the type is STRICT_PRIORITY, else the type is WFQ_WRR_PRIORITY.
 *      In 9300 and 9310:
 *          Valid range of weight is 1~127. STRICT_PRIORITY type is set by rtk_qos_portQueueStrictEnable_set.
 * Changes:
 *      None
 */
extern int32
rtk_qos_schedulingQueue_set(uint32 unit, rtk_port_t port, rtk_qos_queue_weights_t *pQweights);

/* Module Name    : QoS              */
/* Sub-module Name: Congestion avoidance */

/* Function Name:
 *      rtk_qos_congAvoidAlgo_get
 * Description:
 *      Get algorithm of congestion avoidance.
 * Input:
 *      unit  - unit id
 * Output:
 *      pAlgo - pointer to algorithm of congestion avoidance
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Algorithm is as following:
 *      - CONG_AVOID_SRED
 *      - CONG_AVOID_SWRED
 *      - CONG_AVOID_TD
 * Changes:
 *      None
 */
extern int32
rtk_qos_congAvoidAlgo_get(uint32 unit, rtk_qos_congAvoidAlgo_t *pAlgo);

/* Function Name:
 *      rtk_qos_congAvoidAlgo_set
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
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Algorithm is as following:
 *      - CONG_AVOID_SRED
 *      - CONG_AVOID_SWRED
 *      - CONG_AVOID_TD
 * Changes:
 *      None
 */
extern int32
rtk_qos_congAvoidAlgo_set(uint32 unit, rtk_qos_congAvoidAlgo_t algo);

/* Function Name:
 *      rtk_qos_portCongAvoidAlgo_get
 * Description:
 *      Get algorithm of congestion avoidance of specified port.
 * Input:
 *      unit  - unit id
 *      port  - port id
 * Output:
 *      pAlgo - pointer to algorithm of congestion avoidance
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      Algorithm is as following:
 *      - CONG_AVOID_SWRED
 *      - CONG_AVOID_TD
 *      System only supports CONG_AVOID_TD in CPU port.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_qos_portCongAvoidAlgo_get(uint32 unit, rtk_port_t port, rtk_qos_congAvoidAlgo_t *pAlgo);

/* Function Name:
 *      rtk_qos_portCongAvoidAlgo_set
 * Description:
 *      Set algorithm of congestion avoidance of specified port.
 * Input:
 *      unit - unit id
 *      port - port id
 *      algo - algorithm of congestion avoidance
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_INPUT            - invalid input parameter
 * Applicable:
 *      9300, 9310
 * Note:
 *      Algorithm is as following:
 *      - CONG_AVOID_SWRED
 *      - CONG_AVOID_TD
 *      System only supports CONG_AVOID_TD in CPU port.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_qos_portCongAvoidAlgo_set(uint32 unit, rtk_port_t port, rtk_qos_congAvoidAlgo_t algo);

/* Function Name:
 *      rtk_qos_congAvoidSysThresh_get
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
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_DROP_PRECEDENCE - invalid drop precedence
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 * Applicable:
 *      8390
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_qos_congAvoidSysThresh_get(
    uint32                      unit,
    uint32                      dp,
    rtk_qos_congAvoidThresh_t   *pCongAvoidThresh);

/* Function Name:
 *      rtk_qos_congAvoidSysThresh_set
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
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_DROP_PRECEDENCE - invalid drop precedence
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 *      RT_ERR_INPUT           - invalid input parameter
 * Applicable:
 *      8390
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_qos_congAvoidSysThresh_set(
    uint32                      unit,
    uint32                      dp,
    rtk_qos_congAvoidThresh_t   *pCongAvoidThresh);

/* Function Name:
 *      rtk_qos_congAvoidSysDropProbability_get
 * Description:
 *      Get system drop probability of congestion avoidance.
 * Input:
 *      unit         - unit id
 *      dp           - drop precedence
 * Output:
 *      pProbability - pointer to drop probability
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_DROP_PRECEDENCE - invalid drop precedence
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 * Applicable:
 *      8390
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_qos_congAvoidSysDropProbability_get(
    uint32  unit,
    uint32  dp,
    uint32  *pProbability);

/* Function Name:
 *      rtk_qos_congAvoidSysDropProbability_set
 * Description:
 *      Set system drop probability of congestion avoidance.
 * Input:
 *      unit        - unit id
 *      dp          - drop precedence
 *      probability - drop probability
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_DROP_PRECEDENCE - invalid drop precedence
 *      RT_ERR_INPUT           - invalid input parameter
 * Applicable:
 *      8390
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_qos_congAvoidSysDropProbability_set(
    uint32  unit,
    uint32  dp,
    uint32  probability);

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
/* Function Name:
 *      rtk_qos_congAvoidGlobalQueueThresh_get
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
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_QUEUE_ID        - invalid queue id
 *      RT_ERR_DROP_PRECEDENCE - invalid drop precedence
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_qos_congAvoidGlobalQueueConfig_get(unit, queue, dp, pCongAvoidThresh)
 *          Parameters:
 *              queue                           -  queue
 *              dp                              -  dp
 *              pCongAvoidThresh                -  pCongAvoidThresh
 */
extern int32
rtk_qos_congAvoidGlobalQueueThresh_get(
    uint32                      unit,
    rtk_qid_t                   queue,
    uint32                      dp,
    rtk_qos_congAvoidThresh_t   *pCongAvoidThresh);

/* Function Name:
 *      rtk_qos_congAvoidGlobalQueueThresh_set
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
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_QUEUE_ID        - invalid queue id
 *      RT_ERR_DROP_PRECEDENCE - invalid drop precedence
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 *      RT_ERR_INPUT           - invalid input parameter
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_qos_congAvoidGlobalQueueConfig_set(unit, queue, dp, pCongAvoidThresh)
 *          Parameters:
 *              queue                           -  queue
 *              dp                              -  dp
 *              pCongAvoidThresh                -  pCongAvoidThresh
 */
extern int32
rtk_qos_congAvoidGlobalQueueThresh_set(
    uint32                      unit,
    rtk_qid_t                   queue,
    uint32                      dp,
    rtk_qos_congAvoidThresh_t   *pCongAvoidThresh);

/* Function Name:
 *      rtk_qos_congAvoidGlobalQueueDropProbability_get
 * Description:
 *      Get global queue drop probability of congestion avoidance.
 * Input:
 *      unit         - unit id
 *      queue        - queue id
 *      dp           - drop precedence
 * Output:
 *      pProbability - pointer to drop probability
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_QUEUE_ID        - invalid queue id
 *      RT_ERR_DROP_PRECEDENCE - invalid drop precedence
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_qos_congAvoidGlobalQueueConfig_get(unit, queue, dp, pCongAvoidThresh)
 *          Parameters:
 *              queue                           -  queue
 *              dp                              -  dp
 *              pCongAvoidThresh->probability   -  *pProbability
 */
extern int32
rtk_qos_congAvoidGlobalQueueDropProbability_get(
    uint32      unit,
    rtk_qid_t   queue,
    uint32      dp,
    uint32      *pProbability);

/* Function Name:
 *      rtk_qos_congAvoidGlobalQueueDropProbability_set
 * Description:
 *      Set system drop probability of congestion avoidance.
 * Input:
 *      unit        - unit id
 *      queue       - queue id
 *      dp          - drop precedence
 *      probability - drop probability
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
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_qos_congAvoidGlobalQueueConfig_set(unit, queue, dp, pCongAvoidThresh)
 *          Parameters:
 *              queue                           -  queue
 *              dp                              -  dp
 *              pCongAvoidThresh->probability   -  probability
 */
extern int32
rtk_qos_congAvoidGlobalQueueDropProbability_set(
    uint32      unit,
    rtk_qid_t   queue,
    uint32      dp,
    uint32      probability);
#endif

/* Function Name:
 *      rtk_qos_congAvoidGlobalQueueConfig_get
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
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          (1)New added function.
 *          (2)It obsoletes these functions:
 *              rtk_qos_congAvoidGlobalQueueDropProbability_get
 *              rtk_qos_congAvoidGlobalQueueThresh_get
 */
extern int32
rtk_qos_congAvoidGlobalQueueConfig_get(
    uint32    unit,
    rtk_qid_t queue,
    uint32    dp,
    rtk_qos_congAvoidThresh_t *pCongAvoidThresh);

/* Function Name:
 *      rtk_qos_congAvoidGlobalQueueConfig_set
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
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          (1)New added function.
 *          (2)It obsoletes these functions:
 *              rtk_qos_congAvoidGlobalQueueDropProbability_set
 *              rtk_qos_congAvoidGlobalQueueThresh_set
 */
extern int32
rtk_qos_congAvoidGlobalQueueConfig_set(
    uint32    unit,
    rtk_qid_t queue,
    uint32    dp,
    rtk_qos_congAvoidThresh_t *pCongAvoidThresh);

/*
 * Component: WRED
 */

/* Function Name:
 *      rtk_qos_portAvbStreamReservationClassEnable_get
 * Description:
 *      Get status of the specified stream class of the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      srClass - stream class
 * Output:
 *      pEnable - status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID              - invalid unit id
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_PORT                 - invalid port id
 *      RT_ERR_AVB_INVALID_SR_CLASS - Invalid SR Class
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_qos_portAvbStreamReservationClassEnable_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_qos_avbSrClass_t    srClass,
    rtk_enable_t            *pEnable);

/* Function Name:
 *      rtk_qos_portAvbStreamReservationClassEnable_set
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
 *      RT_ERR_UNIT_ID              - invalid unit id
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_PORT                 - invalid port id
 *      RT_ERR_AVB_INVALID_SR_CLASS - Invalid SR Class
 *      RT_ERR_INPUT                - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_qos_portAvbStreamReservationClassEnable_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_qos_avbSrClass_t    srClass,
    rtk_enable_t            enable);

/* Function Name:
 *      rtk_qos_avbStreamReservationConfig_get
 * Description:
 *      Get the configuration of Stream Reservation in the specified device.
 * Input:
 *      unit    - unit id
 * Output:
 *      pSrConf - pointer buffer of configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_qos_avbStreamReservationConfig_get(uint32 unit, rtk_qos_avbSrConf_t *pSrConf);

/* Function Name:
 *      rtk_qos_avbStreamReservationConfig_set
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
 *      RT_ERR_UNIT_ID          - invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_QOS_1P_PRIORITY  - Invalid dot1p priority
 *      RT_ERR_QUEUE_ID         - invalid queue id
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_qos_avbStreamReservationConfig_set(uint32 unit, rtk_qos_avbSrConf_t *pSrConf);

/* Function Name:
 *      rtk_qos_pkt2CpuPriRemap_get
 * Description:
 *      Get the new priority for the packets that normal forwarded to CPU.
 * Input:
 *      unit    - unit id
 *      intPri  - original internal
 * Output:
 *      pNewPri - new priority
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - Invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Applicable:
 *      8380, 8390
 * Note:
 *      System can remap internal priority to a new priority for the packets that normal forwarded to CPU.
 * Changes:
 *      None
 */
extern int32
rtk_qos_pkt2CpuPriRemap_get(uint32 unit, rtk_pri_t intPri, rtk_pri_t *pNewPri);

/* Function Name:
 *      rtk_qos_pkt2CpuPriRemap_set
 * Description:
 *      Set the new priority for the packets that normal forwarded to CPU.
 * Input:
 *      unit   - unit id
 *      intPri - original internal
 *      newPri - new priority
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - Invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority
 * Applicable:
 *      8380, 8390
 * Note:
 *      System can remap internal priority to a new priority for the packets that normal forwarded to CPU.
 * Changes:
 *      None
 */
extern int32
rtk_qos_pkt2CpuPriRemap_set(uint32 unit, rtk_pri_t intPri, rtk_pri_t newPri);

/* Function Name:
 *      rtk_qos_rspanPriRemap_get
 * Description:
 *      Get the internal priority that rspan tag  priority remap.
 * Input:
 *      unit      - unit id
 *      rspan_pri - rspan tag  priority value (range from 0 ~ 7)
 * Output:
 *      pInt_pri  - internal priority value (range from 0 ~ 7)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_QOS_1P_PRIORITY - Invalid rspan tag  priority
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 * Applicable:
 *      8380
 * Note:
 *      None.
 * Changes:
 *      None
 */
extern int32
rtk_qos_rspanPriRemap_get(uint32 unit, rtk_pri_t rspan_pri, rtk_pri_t *pInt_pri);

/* Function Name:
 *      rtk_qos_rspanPriRemap_set
 * Description:
 *      Set the internal priority that rspan tag  priority remap.
 * Input:
 *      unit      - unit id
 *      dot1p_pri - rspan tag  priority value (range from 0 ~ 7)
 *      int_pri   - internal priority value (range from 0 ~ 7)
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_QOS_1P_PRIORITY  - Invalid rspan tag  priority
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority value
 * Applicable:
 *      8380
 * Note:
 *      None.
 * Changes:
 *      None
 */
extern int32
rtk_qos_rspanPriRemap_set(uint32 unit, rtk_pri_t rspan_pri, rtk_pri_t int_pri);

/* Function Name:
 *      rtk_qos_portPri2IgrQMapEnable_get
 * Description:
 *      Get priority to input queue mapping ability.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - ability
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380
 * Note:
 *      None.
 * Changes:
 *      None
 */
extern int32
rtk_qos_portPri2IgrQMapEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_qos_portPri2IgrQMapEnable_set
 * Description:
 *      Set priority to input queue mapping ability.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - ability
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid enable value
 * Applicable:
 *      8380
 * Note:
 *      None.
 * Changes:
 *      None
 */
extern int32
rtk_qos_portPri2IgrQMapEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      rtk_qos_portPri2IgrQMap_get
 * Description:
 *      Get the value of internal priority to QID mapping table.
 * Input:
 *      unit     - unit id
 *      port     - port id
 * Output:
 *      pPri2qid - array of internal priority on a queue
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380
 * Note:
 *      None.
 * Changes:
 *      None
 */
extern int32
rtk_qos_portPri2IgrQMap_get(uint32 unit, rtk_port_t port, rtk_qos_pri2queue_t *pPri2qid);

/* Function Name:
 *      rtk_qos_portPri2IgrQMap_set
 * Description:
 *      Set the value of internal priority to QID mapping table.
 * Input:
 *      unit     - unit id
 *      port     - port id
 * Output:
 *      pPri2qid - array of internal priority on a queue
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380
 * Note:
 *      None.
 * Changes:
 *      None
 */
extern int32
rtk_qos_portPri2IgrQMap_set(uint32 unit, rtk_port_t port, rtk_qos_pri2queue_t *pPri2qid);

/* Function Name:
 *      rtk_qos_portIgrQueueWeight_get
 * Description:
 *      Get the weight of ingress queue.
 * Input:
 *      unit      - unit id
 *      port      - port id
 *      queue  - queue id
 * Output:
 *      pQweights -the weigh of specified queue
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_QUEUE_ID     - invalid queue id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380
 * Note:
 *      If weight == 0, means the queue is STRICT_PRIORITY in 8380.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_qos_portIgrQueueWeight_get(uint32 unit, rtk_port_t port, rtk_qid_t queue, uint32 *pQweight);

/* Function Name:
 *      rtk_qos_portIgrQueueWeight_set
 * Description:
 *      Get the weight of ingress queue.
 * Input:
 *      unit      - unit id
 *      port      - port id
 *      queue  - queue id
 *      qWeights -the weight of specified queue
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_QUEUE_ID     - invalid queue id
 *      RT_ERR_QOS_QUEUE_WEIGHT - Invalid queue weight
 * Applicable:
 *      8380
 * Note:
 *      If weight == 0, means the queue is STRICT_PRIORITY in 8380.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_qos_portIgrQueueWeight_set(uint32 unit, rtk_port_t port, rtk_qid_t queue, uint32 qWeight);

/* Function Name:
 *      rtk_qos_invldDscpVal_get
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
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None.
 * Changes:
 *      None
 */
extern int32
rtk_qos_invldDscpVal_get(uint32 unit, uint32 *pDscp);

/* Function Name:
 *      rtk_qos_invldDscpVal_set
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
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_QOS_DSCP_VALUE - input dscp out of range
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None.
 * Changes:
 *      None
 */
extern int32
rtk_qos_invldDscpVal_set(uint32 unit, uint32 dscp);

/* Function Name:
 *      rtk_qos_invldDscpMask_get
 * Description:
 *      Get the invalid dscp mask in the specified device
 * Input:
 *      unit      - unit id
 * Output:
 *      pDscpMask     - pointer to dscp mask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380
 * Note:
 *      None.
 * Changes:
 *      None
 */
extern int32
rtk_qos_invldDscpMask_get(uint32 unit, uint32 *pDscpMask) ;

/* Function Name:
 *      rtk_qos_invldDscpMask_set
 * Description:
 *      Set the invalid dscp mask in the specified device
 * Input:
 *      unit     - unit id
 *      dscpMask     - dscp mask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_QOS_DSCP_VALUE - input dscp mask out of range
 * Applicable:
 *      8380
 * Note:
 *      None.
 * Changes:
 *      None
 */
extern int32
rtk_qos_invldDscpMask_set(uint32 unit, uint32 dscpMask) ;

/* Function Name:
 *      rtk_qos_portInvldDscpEnable_get
 * Description:
 *      Get invalid DSCP status for a port
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - status of invalid DSCP
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380
 * Note:
 *      The status of invalid DSCP:
 *      - DISABLED
 *      - ENABLED
 * Changes:
 *      None
 */
extern int32
rtk_qos_portInvldDscpEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_qos_portInvldDscpEnable_set
 * Description:
 *      Set invalid DSCP status for a port
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - status of invalid DSCP
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - Invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380
 * Note:
 *      The status of invalid DSCP:
 *      - DISABLED
 *      - ENABLED
 * Changes:
 *      None
 */
extern int32
rtk_qos_portInvldDscpEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable) ;

/* Function Name:
 *      rtk_qos_invldDscpEnable_get
 * Description:
 *      Get invalid DSCP status
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - status of invalid DSCP
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      The status of invalid DSCP:
 *      - DISABLED
 *      - ENABLED
 * Changes:
 *      None
 */
extern int32
rtk_qos_invldDscpEnable_get(uint32 unit, rtk_enable_t *pEnable) ;

/* Function Name:
 *      rtk_qos_invldDscpEnable_set
 * Description:
 *      Set invalid DSCP status
 * Input:
 *      unit   - unit id
 *      enable - status of invalid DSCP
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      The status of invalid DSCP:
 *      - DISABLED
 *      - ENABLED
 * Changes:
 *      None
 */
extern int32
rtk_qos_invldDscpEnable_set(uint32 unit, rtk_enable_t enable) ;

/* Function Name:
 *      rtk_qos_portInnerPriRemapEnable_get
 * Description:
 *      Get port-base inner priority remapping status
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - port-base inner priority remapping status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380
 * Note:
 *      port-base inner priority remapping status:
 *      - DISABLED
 *      - ENABLED
 * Changes:
 *      None
 */
extern int32
rtk_qos_portInnerPriRemapEnable_get(uint32 unit, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_qos_portInnerPriRemapEnable_set
 * Description:
 *      Set port-base inner priority remapping status
 * Input:
 *      unit   - unit id
 *      enable - port-base inner priority remapping status
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380
 * Note:
 *      port-base inner priority remapping status:
 *      - DISABLED
 *      - ENABLED
 * Changes:
 *      None
 */
extern int32
rtk_qos_portInnerPriRemapEnable_set(uint32 unit, rtk_enable_t enable);

/* Function Name:
 *      rtk_qos_portOuterPriRemapEnable_get
 * Description:
 *      Get port-base outer priority remapping status
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - port-base outer priority remapping status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380
 * Note:
 *      port-base outer priority remapping status:
 *      - DISABLED
 *      - ENABLED
 * Changes:
 *      None
 */
extern int32
rtk_qos_portOuterPriRemapEnable_get(uint32 unit, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_qos_portOuterPriRemapEnable_set
 * Description:
 *      Get port-base outer priority remapping status
 * Input:
 *      unit   - unit id
 *      enable - port-base outer priority remapping status
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380
 * Note:
 *      port-base outer priority remapping status:
 *      - DISABLED
 *      - ENABLED
 * Changes:
 *      None
 */
extern int32
rtk_qos_portOuterPriRemapEnable_set(uint32 unit, rtk_enable_t enable);

/* Function Name:
 *      rtk_qos_priRemapEnable_get
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
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      Supported priority remap type is as following:
 *      - rtk_qos_priSrc_t \ Chip:      8390    9300    9310
 *      - PRI_SRC_INT_PRI               X       X       X
 *      - PRI_SRC_INNER_USER_PRI        X       X       X
 *      - PRI_SRC_OUTER_USER_PRI        X       X       X
 *      - PRI_SRC_DSCP                  X       X       X
 *      - PRI_SRC_PB_PRI                O       O       O
 *      - PRI_SRC_PROTO_VLAN            X       O       O
 *      - PRI_SRC_MAC_VLAN              X       O       O
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_qos_portPriRemapEnable_get)
 */
extern int32
rtk_qos_priRemapEnable_get(uint32 unit, rtk_qos_priSrc_t src, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_qos_priRemapEnable_set
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
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      Supported priority remap type is as following:
 *      - rtk_qos_priSrc_t \ Chip:      8390    9300    9310
 *      - PRI_SRC_INT_PRI               X       X       X
 *      - PRI_SRC_INNER_USER_PRI        X       X       X
 *      - PRI_SRC_OUTER_USER_PRI        X       X       X
 *      - PRI_SRC_DSCP                  X       X       X
 *      - PRI_SRC_PB_PRI                O       O       O
 *      - PRI_SRC_PROTO_VLAN            X       O       O
 *      - PRI_SRC_MAC_VLAN              X       O       O
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_qos_portPriRemapEnable_set)
 */
extern int32
rtk_qos_priRemapEnable_set(uint32 unit, rtk_qos_priSrc_t src, rtk_enable_t enable);

/* Function Name:
 *      rtk_qos_portQueueStrictEnable_get
 * Description:
 *      Get enable status of egress queue strict priority for specified port.
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      queue - queue id
 * Output:
 *      pEnable - Pointer to enable status of egress queue strict priority.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_QUEUE_ID     - invalid queue id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_qos_portQueueStrictEnable_get(uint32 unit, rtk_port_t port, rtk_qid_t queue, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_qos_portQueueStrictEnable_set
 * Description:
 *      Set enable status of egress queue strict priority for specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      queue  - queue id
 *      enable - enable status of egress queue strict priority
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_QUEUE_ID - invalid queue id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_qos_portQueueStrictEnable_set(uint32 unit, rtk_port_t port, rtk_qid_t queue, rtk_enable_t enable);

#endif /* __RTK_QOS_H__ */
