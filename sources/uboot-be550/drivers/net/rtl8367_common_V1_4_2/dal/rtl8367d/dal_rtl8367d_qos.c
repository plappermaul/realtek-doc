/*
 * Copyright (C) 2013 Realtek Semiconductor Corp.
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
 * Purpose : RTK switch high-level API for RTL8367/RTL8367D
 * Feature : Here is a list of all functions and variables in QoS module.
 *
 */

#include <rtk_switch.h>
#include <rtk_error.h>
#include <dal/rtl8367d/dal_rtl8367d_qos.h>
#include <linux/string.h>

#include <dal/rtl8367d/rtl8367d_asicdrv.h>

/* Function Name:
 *      dal_rtl8367d_qos_init
 * Description:
 *      Configure Qos default settings with queue number assigment to each port.
 * Input:
 *      queueNum - Queue number of each port.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will initialize related Qos setting with queue number assigment.
 *      The queue number is from 1 to 8.
 */
rtk_api_ret_t dal_rtl8367d_qos_init(rtk_queue_num_t queueNum)
{
    CONST_T rtk_uint16 g_prioritytToQid[8][8]= {
            {0, 0,0,0,0,0,0,0},
            {0, 0,0,0,7,7,7,7},
            {0, 0,0,0,1,1,7,7},
            {0, 0,1,1,2,2,7,7},
            {0, 0,1,1,2,3,7,7},
            {0, 0,1,2,3,4,7,7},
            {0, 0,1,2,3,4,5,7},
            {0, 1,2,3,4,5,6,7}
    };

    CONST_T rtk_uint32 g_priorityDecision[8] = {0x01, 0x80,0x04,0x02,0x20,0x40,0x10,0x08};
    CONST_T rtk_uint32 g_prioritytRemap[8] = {0,1,2,3,4,5,6,7};

    rtk_api_ret_t retVal;
    rtk_uint32 qmapidx;
    rtk_uint32 priority;
    rtk_uint32 priDec;
    rtk_uint32 port;
    rtk_uint32 dscp;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if (queueNum <= 0 || queueNum > RTK_MAX_NUM_OF_QUEUE)
        return RT_ERR_QUEUE_NUM;

    /*Set Output Queue Number*/
    if (RTK_MAX_NUM_OF_QUEUE == queueNum)
        qmapidx = 0;
    else
        qmapidx = queueNum;

    RTK_SCAN_ALL_PHY_PORTMASK(port)
    {
        if ((retVal = rtl8367d_setAsicRegBits((RTL8367D_REG_QOS_PORT_QUEUE_NUMBER_CTRL0 + (port >> 2)), (0x7 << ((port & 0x3) << 2)), qmapidx)) != RT_ERR_OK)
            return retVal;
    }

    /*Set Priority to Qid*/
    for (priority = 0; priority <= RTK_PRIMAX; priority++)
    {
        if ((retVal = rtl8367d_setAsicRegBits((RTL8367D_REG_QOS_1Q_PRIORITY_TO_QID_CTRL0 + ((queueNum - 1) << 1) + (priority >> 2)), (RTL8367D_QOS_1Q_PRIORITY_TO_QID_CTRL0_PRIORITY0_TO_QID_MASK << ((priority & 0x3) << 2)), g_prioritytToQid[queueNum - 1][priority])) != RT_ERR_OK)
            return retVal;
    }

    /*Priority Decision Order*/
    for (priDec = 0;priDec < RTL8367D_PRIDEC_END;priDec++)
    {
        if ((retVal = rtl8367d_setAsicRegBits((RTL8367D_REG_QOS_INTERNAL_PRIORITY_DECISION_CTRL0 + (priDec >> 1)), (RTL8367D_QOS_INTERNAL_PRIORITY_DECISION_CTRL0_QOS_PORT_WEIGHT_MASK << ((priDec & 1) << 3)), g_priorityDecision[priDec])) != RT_ERR_OK)
            return retVal;
        if ((retVal = rtl8367d_setAsicRegBits((RTL8367D_REG_QOS_INTERNAL_PRIORITY_DECISION_CTRL1 + (priDec >> 1)), (RTL8367D_QOS_INTERNAL_PRIORITY_DECISION_CTRL0_QOS_PORT_WEIGHT_MASK << ((priDec & 1) << 3)), g_priorityDecision[priDec])) != RT_ERR_OK)
            return retVal;
    }

    /*Set Port-based Priority to 0*/
    RTK_SCAN_ALL_PHY_PORTMASK(port)
    {
        if ((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_QOS_PORTBASED_PRIORITY_CTRL0 + (port >> 2), (0x7 << ((port & 0x3) << 2)), 0)) != RT_ERR_OK)
            return retVal;
    }

    RTK_SCAN_ALL_PHY_PORTMASK(port)
    {
        /*Disable 1p Remarking*/
        if ((retVal = rtl8367d_setAsicRegBit((RTL8367D_REG_PORT0_MISC_CFG + (port << 5)), RTL8367D_PORT0_MISC_CFG_DOT1Q_REMARK_ENABLE_OFFSET, DISABLED)) != RT_ERR_OK)
            return retVal;
        /*Disable DSCP Remarking*/
        if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_SWITCH_CTRL0, RTL8367D_PORT0_REMARKING_DSCP_ENABLE_OFFSET + port, DISABLED)) != RT_ERR_OK)
            return retVal;
    }

    /*Set 1p & DSCP  Priority Remapping & Remarking*/
    for (priority = 0; priority <= RTL8367D_PRIMAX; priority++)
    {
        if ((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_QOS_1Q_PRIORITY_REMAPPING_CTRL0 + (priority >> 2), (0x7 << ((priority & 0x3) << 2)), g_prioritytRemap[priority])) != RT_ERR_OK)
            return retVal;

        if ((retVal = rtl8367d_setAsicRegBits((RTL8367D_REG_QOS_1Q_REMARK_CTRL0 + (priority >> 2)), (0x7 << ((priority & 0x3) << 2)), 0)) != RT_ERR_OK)
            return retVal;

        if ((retVal = rtl8367d_setAsicRegBits((RTL8367D_REG_QOS_DSCP_REMARK_CTRL0 + (priority >> 1)), (0x3F << (((priority) & 0x1) << 3)), 0)) != RT_ERR_OK)
            return retVal;
    }

    /*Set DSCP Priority*/
    for (dscp = 0; dscp <= 63; dscp++)
    {
        if ((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_QOS_DSCP_TO_PRIORITY_CTRL0 + (dscp >> 2), (0x7 << ((dscp & 0x3) << 2)), 0)) != RT_ERR_OK)
            return retVal;
    }

    /* Finetune B/T value */
    if((retVal = rtl8367d_setAsicReg(0x1722, 0x1158)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_qos_priSel_set
 * Description:
 *      Configure the priority order among different priority mechanism.
 * Input:
 *      index - Priority decision table index (0~1)
 *      pPriDec - Priority assign for port, dscp, 802.1p, cvlan, svlan, acl based priority decision.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_QOS_SEL_PRI_SOURCE   - Invalid priority decision source parameter.
 * Note:
 *      ASIC will follow user priority setting of mechanisms to select mapped queue priority for receiving frame.
 *      If two priority mechanisms are the same, the ASIC will chose the highest priority from mechanisms to
 *      assign queue priority to receiving frame.
 *      The priority sources are:
 *      - RTL8367D_PRIDEC_PORT
 *      - RTL8367D_PRIDEC_ACL
 *      - RTL8367D_PRIDEC_DSCP
 *      - RTL8367D_PRIDEC_1Q
 *      - RTL8367D_PRIDEC_1AD
 */
rtk_api_ret_t dal_rtl8367d_qos_priSel_set(rtk_qos_priDecTbl_t index, rtk_priority_select_t *pPriDec)
{
    rtk_api_ret_t retVal;
    rtk_uint32 port_pow;
    rtk_uint32 dot1q_pow;
    rtk_uint32 dscp_pow;
    rtk_uint32 acl_pow;
    rtk_uint32 svlan_pow;
    rtk_uint32 i;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if (index < 0 || index >= PRIDECTBL_END)
        return RT_ERR_ENTRY_INDEX;

    if (pPriDec->port_pri >= 5 || pPriDec->dot1q_pri >= 5 || pPriDec->acl_pri >= 5 || pPriDec->dscp_pri >= 5 ||
       pPriDec->svlan_pri >= 5)
        return RT_ERR_QOS_SEL_PRI_SOURCE;

    port_pow = 1;
    for (i = pPriDec->port_pri; i > 0; i--)
        port_pow = (port_pow)*2;

    dot1q_pow = 1;
    for (i = pPriDec->dot1q_pri; i > 0; i--)
        dot1q_pow = (dot1q_pow)*2;

    acl_pow = 1;
    for (i = pPriDec->acl_pri; i > 0; i--)
        acl_pow = (acl_pow)*2;

    dscp_pow = 1;
    for (i = pPriDec->dscp_pri; i > 0; i--)
        dscp_pow = (dscp_pow)*2;

    svlan_pow = 1;
    for (i = pPriDec->svlan_pri; i > 0; i--)
        svlan_pow = (svlan_pow)*2;

    switch(index)
    {
        case PRIDECTBL_IDX0:
            if ((retVal = rtl8367d_setAsicRegBits((RTL8367D_REG_QOS_INTERNAL_PRIORITY_DECISION_CTRL0 + (RTL8367D_PRIDEC_PORT >> 1)), (RTL8367D_QOS_INTERNAL_PRIORITY_DECISION_CTRL0_QOS_PORT_WEIGHT_MASK << ((RTL8367D_PRIDEC_PORT & 1) << 3)), port_pow)) != RT_ERR_OK)
                return retVal;

            if ((retVal = rtl8367d_setAsicRegBits((RTL8367D_REG_QOS_INTERNAL_PRIORITY_DECISION_CTRL0 + (RTL8367D_PRIDEC_ACL >> 1)), (RTL8367D_QOS_INTERNAL_PRIORITY_DECISION_CTRL0_QOS_PORT_WEIGHT_MASK << ((RTL8367D_PRIDEC_ACL & 1) << 3)), acl_pow)) != RT_ERR_OK)
                return retVal;

            if ((retVal = rtl8367d_setAsicRegBits((RTL8367D_REG_QOS_INTERNAL_PRIORITY_DECISION_CTRL0 + (RTL8367D_PRIDEC_DSCP >> 1)), (RTL8367D_QOS_INTERNAL_PRIORITY_DECISION_CTRL0_QOS_PORT_WEIGHT_MASK << ((RTL8367D_PRIDEC_DSCP & 1) << 3)), dscp_pow)) != RT_ERR_OK)
                return retVal;

            if ((retVal = rtl8367d_setAsicRegBits((RTL8367D_REG_QOS_INTERNAL_PRIORITY_DECISION_CTRL0 + (RTL8367D_PRIDEC_1Q >> 1)), (RTL8367D_QOS_INTERNAL_PRIORITY_DECISION_CTRL0_QOS_PORT_WEIGHT_MASK << ((RTL8367D_PRIDEC_1Q & 1) << 3)), dot1q_pow)) != RT_ERR_OK)
                return retVal;

            if ((retVal = rtl8367d_setAsicRegBits((RTL8367D_REG_QOS_INTERNAL_PRIORITY_DECISION_CTRL0 + (RTL8367D_PRIDEC_1AD >> 1)), (RTL8367D_QOS_INTERNAL_PRIORITY_DECISION_CTRL0_QOS_PORT_WEIGHT_MASK << ((RTL8367D_PRIDEC_1AD & 1) << 3)), svlan_pow)) != RT_ERR_OK)
                return retVal;
            break;

        case PRIDECTBL_IDX1:
            if ((retVal = rtl8367d_setAsicRegBits((RTL8367D_REG_QOS_INTERNAL_PRIORITY_DECISION2_CTRL0 + (RTL8367D_PRIDEC_PORT >> 1)), (RTL8367D_QOS_INTERNAL_PRIORITY_DECISION2_CTRL0_QOS_PORT_WEIGHT_MASK << ((RTL8367D_PRIDEC_PORT & 1) << 3)), port_pow)) != RT_ERR_OK)
                return retVal;

            if ((retVal = rtl8367d_setAsicRegBits((RTL8367D_REG_QOS_INTERNAL_PRIORITY_DECISION2_CTRL0 + (RTL8367D_PRIDEC_ACL >> 1)), (RTL8367D_QOS_INTERNAL_PRIORITY_DECISION2_CTRL0_QOS_PORT_WEIGHT_MASK << ((RTL8367D_PRIDEC_ACL & 1) << 3)), acl_pow)) != RT_ERR_OK)
                return retVal;

            if ((retVal = rtl8367d_setAsicRegBits((RTL8367D_REG_QOS_INTERNAL_PRIORITY_DECISION2_CTRL0 + (RTL8367D_PRIDEC_DSCP >> 1)), (RTL8367D_QOS_INTERNAL_PRIORITY_DECISION2_CTRL0_QOS_PORT_WEIGHT_MASK << ((RTL8367D_PRIDEC_DSCP & 1) << 3)), dscp_pow)) != RT_ERR_OK)
                return retVal;

            if ((retVal = rtl8367d_setAsicRegBits((RTL8367D_REG_QOS_INTERNAL_PRIORITY_DECISION2_CTRL0 + (RTL8367D_PRIDEC_1Q >> 1)), (RTL8367D_QOS_INTERNAL_PRIORITY_DECISION2_CTRL0_QOS_PORT_WEIGHT_MASK << ((RTL8367D_PRIDEC_1Q & 1) << 3)), dot1q_pow)) != RT_ERR_OK)
                return retVal;

            if ((retVal = rtl8367d_setAsicRegBits((RTL8367D_REG_QOS_INTERNAL_PRIORITY_DECISION2_CTRL0 + (RTL8367D_PRIDEC_1AD >> 1)), (RTL8367D_QOS_INTERNAL_PRIORITY_DECISION2_CTRL0_QOS_PORT_WEIGHT_MASK << ((RTL8367D_PRIDEC_1AD & 1) << 3)), svlan_pow)) != RT_ERR_OK)
                return retVal;
            break;
        default:
            return RT_ERR_INPUT;

    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_qos_priSel_get
 * Description:
 *      Get the priority order configuration among different priority mechanism.
 * Input:
 *      index - Priority decision table index (0~1)
 * Output:
 *      pPriDec - Priority assign for port, dscp, 802.1p, cvlan, svlan, acl based priority decision .
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *      ASIC will follow user priority setting of mechanisms to select mapped queue priority for receiving frame.
 *      If two priority mechanisms are the same, the ASIC will chose the highest priority from mechanisms to
 *      assign queue priority to receiving frame.
 *      The priority sources are:
 *      - RTL8367D_PRIDEC_PORT,
 *      - RTL8367D_PRIDEC_ACL,
 *      - RTL8367D_PRIDEC_DSCP,
 *      - RTL8367D_PRIDEC_1Q,
 *      - RTL8367D_PRIDEC_1AD,
 */
rtk_api_ret_t dal_rtl8367d_qos_priSel_get(rtk_qos_priDecTbl_t index, rtk_priority_select_t *pPriDec)
{

    rtk_api_ret_t retVal;
    rtk_int32 i;
    rtk_uint32 port_pow;
    rtk_uint32 dot1q_pow;
    rtk_uint32 dscp_pow;
    rtk_uint32 acl_pow;
    rtk_uint32 svlan_pow;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if (index < 0 || index >= PRIDECTBL_END)
        return RT_ERR_ENTRY_INDEX;

    memset(pPriDec, 0x00, sizeof(rtk_priority_select_t));

    switch(index)
    {
        case PRIDECTBL_IDX0:
            if ((retVal = rtl8367d_getAsicRegBits((RTL8367D_REG_QOS_INTERNAL_PRIORITY_DECISION_CTRL0 + (RTL8367D_PRIDEC_PORT >> 1)), (RTL8367D_QOS_INTERNAL_PRIORITY_DECISION_CTRL0_QOS_PORT_WEIGHT_MASK << ((RTL8367D_PRIDEC_PORT & 1) << 3)), &port_pow)) != RT_ERR_OK)
                return retVal;

            if ((retVal = rtl8367d_getAsicRegBits((RTL8367D_REG_QOS_INTERNAL_PRIORITY_DECISION_CTRL0 + (RTL8367D_PRIDEC_ACL >> 1)), (RTL8367D_QOS_INTERNAL_PRIORITY_DECISION_CTRL0_QOS_PORT_WEIGHT_MASK << ((RTL8367D_PRIDEC_ACL & 1) << 3)), &acl_pow)) != RT_ERR_OK)
                return retVal;

            if ((retVal = rtl8367d_getAsicRegBits((RTL8367D_REG_QOS_INTERNAL_PRIORITY_DECISION_CTRL0 + (RTL8367D_PRIDEC_DSCP >> 1)), (RTL8367D_QOS_INTERNAL_PRIORITY_DECISION_CTRL0_QOS_PORT_WEIGHT_MASK << ((RTL8367D_PRIDEC_DSCP & 1) << 3)), &dscp_pow)) != RT_ERR_OK)
                return retVal;

            if ((retVal = rtl8367d_getAsicRegBits((RTL8367D_REG_QOS_INTERNAL_PRIORITY_DECISION_CTRL0 + (RTL8367D_PRIDEC_1Q >> 1)), (RTL8367D_QOS_INTERNAL_PRIORITY_DECISION_CTRL0_QOS_PORT_WEIGHT_MASK << ((RTL8367D_PRIDEC_1Q & 1) << 3)), &dot1q_pow)) != RT_ERR_OK)
                return retVal;

            if ((retVal = rtl8367d_getAsicRegBits((RTL8367D_REG_QOS_INTERNAL_PRIORITY_DECISION_CTRL0 + (RTL8367D_PRIDEC_1AD >> 1)), (RTL8367D_QOS_INTERNAL_PRIORITY_DECISION_CTRL0_QOS_PORT_WEIGHT_MASK << ((RTL8367D_PRIDEC_1AD & 1) << 3)), &svlan_pow)) != RT_ERR_OK)
                return retVal;
            break;

        case PRIDECTBL_IDX1:
            if ((retVal = rtl8367d_getAsicRegBits((RTL8367D_REG_QOS_INTERNAL_PRIORITY_DECISION2_CTRL0 + (RTL8367D_PRIDEC_PORT >> 1)), (RTL8367D_QOS_INTERNAL_PRIORITY_DECISION2_CTRL0_QOS_PORT_WEIGHT_MASK << ((RTL8367D_PRIDEC_PORT & 1) << 3)), &port_pow)) != RT_ERR_OK)
                return retVal;

            if ((retVal = rtl8367d_getAsicRegBits((RTL8367D_REG_QOS_INTERNAL_PRIORITY_DECISION2_CTRL0 + (RTL8367D_PRIDEC_ACL >> 1)), (RTL8367D_QOS_INTERNAL_PRIORITY_DECISION2_CTRL0_QOS_PORT_WEIGHT_MASK << ((RTL8367D_PRIDEC_ACL & 1) << 3)), &acl_pow)) != RT_ERR_OK)
                return retVal;

            if ((retVal = rtl8367d_getAsicRegBits((RTL8367D_REG_QOS_INTERNAL_PRIORITY_DECISION2_CTRL0 + (RTL8367D_PRIDEC_DSCP >> 1)), (RTL8367D_QOS_INTERNAL_PRIORITY_DECISION2_CTRL0_QOS_PORT_WEIGHT_MASK << ((RTL8367D_PRIDEC_DSCP & 1) << 3)), &dscp_pow)) != RT_ERR_OK)
                return retVal;

            if ((retVal = rtl8367d_getAsicRegBits((RTL8367D_REG_QOS_INTERNAL_PRIORITY_DECISION2_CTRL0 + (RTL8367D_PRIDEC_1Q >> 1)), (RTL8367D_QOS_INTERNAL_PRIORITY_DECISION2_CTRL0_QOS_PORT_WEIGHT_MASK << ((RTL8367D_PRIDEC_1Q & 1) << 3)), &dot1q_pow)) != RT_ERR_OK)
                return retVal;

            if ((retVal = rtl8367d_getAsicRegBits((RTL8367D_REG_QOS_INTERNAL_PRIORITY_DECISION2_CTRL0 + (RTL8367D_PRIDEC_1AD >> 1)), (RTL8367D_QOS_INTERNAL_PRIORITY_DECISION2_CTRL0_QOS_PORT_WEIGHT_MASK << ((RTL8367D_PRIDEC_1AD & 1) << 3)), &svlan_pow)) != RT_ERR_OK)
                return retVal;
            break;

        default:
            return RT_ERR_INPUT;

    }

    for (i = 31; i >= 0; i--)
    {
        if (port_pow & (1 << i))
        {
            pPriDec->port_pri = i;
            break;
        }
    }

    for (i = 31; i >= 0; i--)
    {
        if (dot1q_pow & (1 << i))
        {
            pPriDec->dot1q_pri = i;
            break;
        }
    }

    for (i = 31; i >= 0; i--)
    {
        if (acl_pow & (1 << i))
        {
            pPriDec->acl_pri = i;
            break;
        }
    }

    for (i = 31; i >= 0; i--)
    {
        if (dscp_pow & (1 << i))
        {
            pPriDec->dscp_pri = i;
            break;
        }
    }

    for (i = 31; i >= 0; i--)
    {
        if (svlan_pow & (1 << i))
        {
            pPriDec->svlan_pri = i;
            break;
        }
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_qos_1pPriRemap_set
 * Description:
 *      Configure 1Q priorities mapping to internal absolute priority.
 * Input:
 *      dot1p_pri   - 802.1p priority value.
 *      int_pri     - internal priority value.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_INPUT            - Invalid input parameters.
 *      RT_ERR_VLAN_PRIORITY    - Invalid 1p priority.
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority.
 * Note:
 *      Priority of 802.1Q assignment for internal asic priority, and it is used for queue usage and packet scheduling.
 */
rtk_api_ret_t dal_rtl8367d_qos_1pPriRemap_set(rtk_pri_t dot1p_pri, rtk_pri_t int_pri)
{
    rtk_api_ret_t retVal;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if (dot1p_pri > RTL8367D_PRIMAX || int_pri > RTL8367D_PRIMAX)
        return  RT_ERR_VLAN_PRIORITY;

    if ((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_QOS_1Q_PRIORITY_REMAPPING_CTRL0 + (dot1p_pri >> 2), (0x7 << ((dot1p_pri & 0x3) << 2)), int_pri)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_qos_1pPriRemap_get
 * Description:
 *      Get 1Q priorities mapping to internal absolute priority.
 * Input:
 *      dot1p_pri - 802.1p priority value .
 * Output:
 *      pInt_pri - internal priority value.
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_VLAN_PRIORITY    - Invalid priority.
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority.
 * Note:
 *      Priority of 802.1Q assigment for internal asic priority, and it is uesed for queue usage and packet scheduling.
 */
rtk_api_ret_t dal_rtl8367d_qos_1pPriRemap_get(rtk_pri_t dot1p_pri, rtk_pri_t *pInt_pri)
{
    rtk_api_ret_t retVal;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if (dot1p_pri > RTL8367D_PRIMAX)
        return  RT_ERR_QOS_INT_PRIORITY;

    if ((retVal = rtl8367d_getAsicRegBits(RTL8367D_REG_QOS_1Q_PRIORITY_REMAPPING_CTRL0 + (dot1p_pri >> 2), (0x7 << ((dot1p_pri & 0x3) << 2)), pInt_pri)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_qos_dscpPriRemap_set
 * Description:
 *      Map dscp value to internal priority.
 * Input:
 *      dscp    - Dscp value of receiving frame
 *      int_pri - internal priority value .
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_INPUT            - Invalid input parameters.
 *      RT_ERR_QOS_DSCP_VALUE   - Invalid DSCP value.
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority.
 * Note:
 *      The Differentiated Service Code Point is a selector for router's per-hop behaviors. As a selector, there is no implication that a numerically
 *      greater DSCP implies a better network service. As can be seen, the DSCP totally overlaps the old precedence field of TOS. So if values of
 *      DSCP are carefully chosen then backward compatibility can be achieved.
 */
rtk_api_ret_t dal_rtl8367d_qos_dscpPriRemap_set(rtk_dscp_t dscp, rtk_pri_t int_pri)
{
    rtk_api_ret_t retVal;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if (int_pri > RTL8367D_PRIMAX )
        return RT_ERR_QOS_INT_PRIORITY;

    if (dscp > RTL8367D_DSCPMAX)
        return RT_ERR_QOS_DSCP_VALUE;

    if ((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_QOS_DSCP_TO_PRIORITY_CTRL0 + (dscp >> 2), (0x7 << ((dscp & 0x3) << 2)), int_pri)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_qos_dscpPriRemap_get
 * Description:
 *      Get dscp value to internal priority.
 * Input:
 *      dscp - Dscp value of receiving frame
 * Output:
 *      pInt_pri - internal priority value.
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_QOS_DSCP_VALUE   - Invalid DSCP value.
 * Note:
 *      The Differentiated Service Code Point is a selector for router's per-hop behaviors. As a selector, there is no implication that a numerically
 *      greater DSCP implies a better network service. As can be seen, the DSCP totally overlaps the old precedence field of TOS. So if values of
 *      DSCP are carefully chosen then backward compatibility can be achieved.
 */
rtk_api_ret_t dal_rtl8367d_qos_dscpPriRemap_get(rtk_dscp_t dscp, rtk_pri_t *pInt_pri)
{
    rtk_api_ret_t retVal;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if (dscp > RTL8367D_DSCPMAX)
        return RT_ERR_QOS_DSCP_VALUE;

    if ((retVal = rtl8367d_getAsicRegBits(RTL8367D_REG_QOS_DSCP_TO_PRIORITY_CTRL0 + (dscp >> 2), (0x7 << ((dscp & 0x3) << 2)), pInt_pri)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_qos_portPri_set
 * Description:
 *      Configure priority usage to each port.
 * Input:
 *      port - Port id.
 *      int_pri - internal priority value.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_PORT_ID          - Invalid port number.
 *      RT_ERR_QOS_SEL_PORT_PRI - Invalid port priority.
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority.
 * Note:
 *      The API can set priority of port assignments for queue usage and packet scheduling.
 */
rtk_api_ret_t dal_rtl8367d_qos_portPri_set(rtk_port_t port, rtk_pri_t int_pri)
{
    rtk_api_ret_t retVal;
    rtk_uint32 phy_port;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check Port Valid */
    RTK_CHK_PORT_VALID(port);

    if (int_pri > RTL8367D_PRIMAX )
        return RT_ERR_QOS_INT_PRIORITY;

    phy_port = rtk_switch_port_L2P_get(port);

    if ((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_QOS_PORTBASED_PRIORITY_CTRL0 + (phy_port >> 2), (0x7 << ((phy_port & 0x3) << 2)), int_pri)) != RT_ERR_OK)

        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_qos_portPri_get
 * Description:
 *      Get priority usage to each port.
 * Input:
 *      port - Port id.
 * Output:
 *      pInt_pri - internal priority value.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      The API can get priority of port assignments for queue usage and packet scheduling.
 */
rtk_api_ret_t dal_rtl8367d_qos_portPri_get(rtk_port_t port, rtk_pri_t *pInt_pri)
{
    rtk_api_ret_t retVal;
    rtk_uint32 phy_port;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check Port Valid */
    RTK_CHK_PORT_VALID(port);

    phy_port = rtk_switch_port_L2P_get(port);

    if ((retVal = rtl8367d_getAsicRegBits(RTL8367D_REG_QOS_PORTBASED_PRIORITY_CTRL0 + (phy_port >> 2), (0x7 << ((phy_port & 0x3) << 2)), pInt_pri)) != RT_ERR_OK)
        return retVal;


    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_qos_queueNum_set
 * Description:
 *      Set output queue number for each port.
 * Input:
 *      port    - Port id.
 *      index   - Mapping queue number (1~8)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 * Note:
 *      The API can set the output queue number of the specified port. The queue number is from 1 to 8.
 */
rtk_api_ret_t dal_rtl8367d_qos_queueNum_set(rtk_port_t port, rtk_queue_num_t queue_num)
{
    rtk_api_ret_t retVal;
    rtk_uint32 phy_port;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check Port Valid */
    RTK_CHK_PORT_VALID(port);

    if ((0 == queue_num) || (queue_num > RTK_MAX_NUM_OF_QUEUE))
        return RT_ERR_FAILED;

    if (RTK_MAX_NUM_OF_QUEUE == queue_num)
        queue_num = 0;

    phy_port = rtk_switch_port_L2P_get(port);

    if ((retVal = rtl8367d_setAsicRegBits((RTL8367D_REG_QOS_PORT_QUEUE_NUMBER_CTRL0 + (phy_port >> 2)), (0x7 << ((phy_port & 0x3) << 2)), queue_num)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_qos_queueNum_get
 * Description:
 *      Get output queue number.
 * Input:
 *      port - Port id.
 * Output:
 *      pQueue_num - Mapping queue number
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      The API will return the output queue number of the specified port. The queue number is from 1 to 8.
 */
rtk_api_ret_t dal_rtl8367d_qos_queueNum_get(rtk_port_t port, rtk_queue_num_t *pQueue_num)
{
    rtk_api_ret_t retVal;
    rtk_uint32 qidx;
    rtk_uint32 phy_port;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check Port Valid */
    RTK_CHK_PORT_VALID(port);

    phy_port = rtk_switch_port_L2P_get(port);

    if ((retVal = rtl8367d_getAsicRegBits((RTL8367D_REG_QOS_PORT_QUEUE_NUMBER_CTRL0 + (phy_port >> 2)), (0x7 << ((phy_port & 0x3) << 2)), &qidx)) != RT_ERR_OK)
        return retVal;

    if (0 == qidx)
        *pQueue_num = 8;
    else
        *pQueue_num = qidx;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_qos_priMap_set
 * Description:
 *      Set output queue number for each port.
 * Input:
 *      queue_num   - Queue number usage.
 *      pPri2qid    - Priority mapping to queue ID.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_INPUT            - Invalid input parameters.
 *      RT_ERR_QUEUE_NUM        - Invalid queue number.
 *      RT_ERR_QUEUE_ID         - Invalid queue id.
 *      RT_ERR_PORT_ID          - Invalid port number.
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority.
 * Note:
 *      ASIC supports priority mapping to queue with different queue number from 1 to 8.
 *      For different queue numbers usage, ASIC supports different internal available queue IDs.
 */
rtk_api_ret_t dal_rtl8367d_qos_priMap_set(rtk_queue_num_t queue_num, rtk_qos_pri2queue_t *pPri2qid)
{
    rtk_api_ret_t retVal;
    rtk_uint32 pri;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if ((0 == queue_num) || (queue_num > RTK_MAX_NUM_OF_QUEUE))
        return RT_ERR_QUEUE_NUM;

    for (pri = 0; pri <= RTK_PRIMAX; pri++)
    {
        if (pPri2qid->pri2queue[pri] > RTK_QIDMAX)
            return RT_ERR_QUEUE_ID;

        if ((retVal = rtl8367d_setAsicRegBits((RTL8367D_REG_QOS_1Q_PRIORITY_TO_QID_CTRL0 + ((queue_num - 1) << 1) + (pri >> 2)), (RTL8367D_QOS_1Q_PRIORITY_TO_QID_CTRL0_PRIORITY0_TO_QID_MASK << ((pri & 0x3) << 2)), pPri2qid->pri2queue[pri])) != RT_ERR_OK)
            return retVal;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_qos_priMap_get
 * Description:
 *      Get priority to queue ID mapping table parameters.
 * Input:
 *      queue_num - Queue number usage.
 * Output:
 *      pPri2qid - Priority mapping to queue ID.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 * Note:
 *      The API can return the mapping queue id of the specified priority and queue number.
 *      The queue number is from 1 to 8.
 */
rtk_api_ret_t dal_rtl8367d_qos_priMap_get(rtk_queue_num_t queue_num, rtk_qos_pri2queue_t *pPri2qid)
{
    rtk_api_ret_t retVal;
    rtk_uint32 pri;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if ((0 == queue_num) || (queue_num > RTK_MAX_NUM_OF_QUEUE))
        return RT_ERR_QUEUE_NUM;

    for (pri = 0; pri <= RTK_PRIMAX; pri++)
    {

        if ((retVal = rtl8367d_getAsicRegBits((RTL8367D_REG_QOS_1Q_PRIORITY_TO_QID_CTRL0 + ((queue_num-1) << 1) + (pri >> 2)), (RTL8367D_QOS_1Q_PRIORITY_TO_QID_CTRL0_PRIORITY0_TO_QID_MASK << ((pri & 0x3) << 2)), &pPri2qid->pri2queue[pri])) != RT_ERR_OK)
            return retVal;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_qos_schedulingQueue_set
 * Description:
 *      Set weight and type of queues in dedicated port.
 * Input:
 *      port        - Port id.
 *      pQweights   - The array of weights for WRR/WFQ queue (0 for STRICT_PRIORITY queue).
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_PORT_ID          - Invalid port number.
 *      RT_ERR_QOS_QUEUE_WEIGHT - Invalid queue weight.
 * Note:
 *      The API can set weight and type, strict priority or weight fair queue (WFQ) for
 *      dedicated port for using queues. If queue id is not included in queue usage,
 *      then its type and weight setting in dummy for setting. There are priorities
 *      as queue id in strict queues. It means strict queue id 5 carrying higher priority
 *      than strict queue id 4. The WFQ queue weight is from 1 to 127, and weight 0 is
 *      for strict priority queue type.
 */
rtk_api_ret_t dal_rtl8367d_qos_schedulingQueue_set(rtk_port_t port, rtk_qos_queue_weights_t *pQweights)
{
    rtk_api_ret_t retVal;
    rtk_uint32 qid;
    rtk_uint32 phy_port;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check Port Valid */
    RTK_CHK_PORT_VALID(port);

    for (qid = 0; qid < RTL8367D_QUEUENO; qid ++)
    {

        if (pQweights->weights[qid] > QOS_WEIGHT_MAX)
            return RT_ERR_QOS_QUEUE_WEIGHT;

        phy_port = rtk_switch_port_L2P_get(port);

        if (0 == pQweights->weights[qid])
        {
            if ((retVal = rtl8367d_setAsicRegBit((RTL8367D_REG_SCHEDULE_QUEUE_TYPE_CTRL0 + (phy_port >> 1)), (((phy_port & 0x1) << 3) + qid),RTL8367D_QTYPE_STRICT)) != RT_ERR_OK)
                return retVal;
        }
        else
        {
            if ((retVal = rtl8367d_setAsicRegBit((RTL8367D_REG_SCHEDULE_QUEUE_TYPE_CTRL0 + (phy_port >> 1)), (((phy_port & 0x1) << 3) + qid),RTL8367D_QTYPE_WFQ)) != RT_ERR_OK)
                return retVal;

            if ((retVal = rtl8367d_setAsicReg((RTL8367D_REG_SCHEDULE_PORT0_QUEUE0_WFQ_WEIGHT + (phy_port << 3) + qid), pQweights->weights[qid])) != RT_ERR_OK)
                return retVal;
        }
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_qos_schedulingQueue_get
 * Description:
 *      Get weight and type of queues in dedicated port.
 * Input:
 *      port - Port id.
 * Output:
 *      pQweights - The array of weights for WRR/WFQ queue (0 for STRICT_PRIORITY queue).
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      The API can get weight and type, strict priority or weight fair queue (WFQ) for dedicated port for using queues.
 *      The WFQ queue weight is from 1 to 127, and weight 0 is for strict priority queue type.
 */
rtk_api_ret_t dal_rtl8367d_qos_schedulingQueue_get(rtk_port_t port, rtk_qos_queue_weights_t *pQweights)
{
    rtk_api_ret_t retVal;
    rtk_uint32 qid,qtype,qweight,phy_port;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check Port Valid */
    RTK_CHK_PORT_VALID(port);

    phy_port = rtk_switch_port_L2P_get(port);

    for (qid = 0; qid < RTL8367D_QUEUENO; qid++)
    {
        if ((retVal = rtl8367d_getAsicRegBit((RTL8367D_REG_SCHEDULE_QUEUE_TYPE_CTRL0 + (phy_port >> 1)), (((phy_port & 0x1) << 3) + qid),&qtype)) != RT_ERR_OK)
            return retVal;

        if (RTL8367D_QTYPE_STRICT == qtype)
        {
            pQweights->weights[qid] = 0;
        }
        else if (RTL8367D_QTYPE_WFQ == qtype)
        {
            if ((retVal = rtl8367d_getAsicReg((RTL8367D_REG_SCHEDULE_PORT0_QUEUE0_WFQ_WEIGHT + (phy_port << 3) + qid), &qweight)) != RT_ERR_OK)
                return retVal;
            pQweights->weights[qid] = qweight;
        }
    }
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_qos_1pRemarkEnable_set
 * Description:
 *      Set 1p Remarking state
 * Input:
 *      port        - Port id.
 *      enable      - State of per-port 1p Remarking
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 *      RT_ERR_ENABLE       - Invalid enable parameter.
 * Note:
 *      The API can enable or disable 802.1p remarking ability for whole system.
 *      The status of 802.1p remark:
 *      - DISABLED
 *      - ENABLED
 */
rtk_api_ret_t dal_rtl8367d_qos_1pRemarkEnable_set(rtk_port_t port, rtk_enable_t enable)
{
    rtk_api_ret_t retVal;
    rtk_uint32 phy_port;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check Port Valid */
    RTK_CHK_PORT_VALID(port);

    if (enable >= RTK_ENABLE_END)
        return RT_ERR_INPUT;

    phy_port = rtk_switch_port_L2P_get(port);

    if ((retVal = rtl8367d_setAsicRegBit((RTL8367D_REG_PORT0_MISC_CFG + (phy_port << 5)), RTL8367D_PORT0_MISC_CFG_DOT1Q_REMARK_ENABLE_OFFSET, enable)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_qos_1pRemarkEnable_get
 * Description:
 *      Get 802.1p remarking ability.
 * Input:
 *      port - Port id.
 * Output:
 *      pEnable - Status of 802.1p remark.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      The API can get 802.1p remarking ability.
 *      The status of 802.1p remark:
 *      - DISABLED
 *      - ENABLED
 */
rtk_api_ret_t dal_rtl8367d_qos_1pRemarkEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    rtk_api_ret_t retVal;
    rtk_uint32 phy_port;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check Port Valid */
    RTK_CHK_PORT_VALID(port);

    phy_port = rtk_switch_port_L2P_get(port);

    if ((retVal = rtl8367d_getAsicRegBit((RTL8367D_REG_PORT0_MISC_CFG + (phy_port << 5)), RTL8367D_PORT0_MISC_CFG_DOT1Q_REMARK_ENABLE_OFFSET, pEnable)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_qos_1pRemark_set
 * Description:
 *      Set 802.1p remarking parameter.
 * Input:
 *      int_pri     - Internal priority value.
 *      dot1p_pri   - 802.1p priority value.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_VLAN_PRIORITY    - Invalid 1p priority.
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority.
 * Note:
 *      The API can set 802.1p parameters source priority and new priority.
 */
rtk_api_ret_t dal_rtl8367d_qos_1pRemark_set(rtk_pri_t int_pri, rtk_pri_t dot1p_pri)
{
    rtk_api_ret_t retVal;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if (int_pri > RTL8367D_PRIMAX )
        return RT_ERR_QOS_INT_PRIORITY;

    if (dot1p_pri > RTL8367D_PRIMAX)
        return RT_ERR_VLAN_PRIORITY;

    if ((retVal = rtl8367d_setAsicRegBits((RTL8367D_REG_QOS_1Q_REMARK_CTRL0 + (int_pri >> 2)), (0x7 << ((int_pri & 0x3) << 2)), dot1p_pri)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_qos_1pRemark_get
 * Description:
 *      Get 802.1p remarking parameter.
 * Input:
 *      int_pri - Internal priority value.
 * Output:
 *      pDot1p_pri - 802.1p priority value.
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority.
 * Note:
 *      The API can get 802.1p remarking parameters. It would return new priority of ingress priority.
 */
rtk_api_ret_t dal_rtl8367d_qos_1pRemark_get(rtk_pri_t int_pri, rtk_pri_t *pDot1p_pri)
{
    rtk_api_ret_t retVal;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if (int_pri > RTL8367D_PRIMAX )
        return RT_ERR_QOS_INT_PRIORITY;

    if ((retVal = rtl8367d_getAsicRegBits((RTL8367D_REG_QOS_1Q_REMARK_CTRL0 + (int_pri >> 2)), (0x7 << ((int_pri & 0x3) << 2)), pDot1p_pri)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_qos_1pRemarkSrcSel_set
 * Description:
 *      Set remarking source of 802.1p remarking.
 * Input:
 *      type      - remarking source
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT            - invalid input parameter

 * Note:
 *      The API can configure 802.1p remark functionality to map original 802.1p value or internal
 *      priority to TX DSCP value.
 */
rtk_api_ret_t dal_rtl8367d_qos_1pRemarkSrcSel_set(rtk_qos_1pRmkSrc_t type)
{
    rtk_api_ret_t retVal;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if (type >= DOT1P_RMK_SRC_END )
        return RT_ERR_QOS_INT_PRIORITY;

    if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_RMK_CFG_SEL_CTRL, RTL8367D_RMK_1Q_CFG_SEL_OFFSET, type)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_qos_1pRemarkSrcSel_get
 * Description:
 *      Get remarking source of 802.1p remarking.
 * Input:
 *      none
 * Output:
 *      pType      - remarking source
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
rtk_api_ret_t dal_rtl8367d_qos_1pRemarkSrcSel_get(rtk_qos_1pRmkSrc_t *pType)
{
    rtk_api_ret_t retVal;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if ((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_RMK_CFG_SEL_CTRL, RTL8367D_RMK_1Q_CFG_SEL_OFFSET, pType)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_qos_dscpRemarkEnable_set
 * Description:
 *      Set DSCP remarking ability.
 * Input:
 *      port    - Port id.
 *      enable  - status of DSCP remark.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_PORT_ID          - Invalid port number.
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority.
 *      RT_ERR_ENABLE           - Invalid enable parameter.
 * Note:
 *      The API can enable or disable DSCP remarking ability for whole system.
 *      The status of DSCP remark:
 *      - DISABLED
 *      - ENABLED
 */
rtk_api_ret_t dal_rtl8367d_qos_dscpRemarkEnable_set(rtk_port_t port, rtk_enable_t enable)
{
    rtk_api_ret_t retVal;
    rtk_uint32 phy_port;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check Port Valid */
    RTK_CHK_PORT_VALID(port);

    if (enable >= RTK_ENABLE_END)
        return RT_ERR_INPUT;

    phy_port = rtk_switch_port_L2P_get(port);

    if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_SWITCH_CTRL0, RTL8367D_PORT0_REMARKING_DSCP_ENABLE_OFFSET + phy_port, enable)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_qos_dscpRemarkEnable_get
 * Description:
 *      Get DSCP remarking ability.
 * Input:
 *      port - Port id.
 * Output:
 *      pEnable - status of DSCP remarking.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      The API can get DSCP remarking ability.
 *      The status of DSCP remark:
 *      - DISABLED
 *      - ENABLED
 */
rtk_api_ret_t dal_rtl8367d_qos_dscpRemarkEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    rtk_api_ret_t retVal;
    rtk_uint32 phy_port;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check Port Valid */
    RTK_CHK_PORT_VALID(port);

    phy_port = rtk_switch_port_L2P_get(port);

    if ((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_SWITCH_CTRL0, RTL8367D_PORT0_REMARKING_DSCP_ENABLE_OFFSET + phy_port, pEnable)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_qos_dscpRemark_set
 * Description:
 *      Set DSCP remarking parameter.
 * Input:
 *      int_pri - Internal priority value.
 *      dscp    - DSCP value.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority.
 *      RT_ERR_QOS_DSCP_VALUE   - Invalid DSCP value.
 * Note:
 *      The API can set DSCP value and mapping priority.
 */
rtk_api_ret_t dal_rtl8367d_qos_dscpRemark_set(rtk_pri_t int_pri, rtk_dscp_t dscp)
{
    rtk_api_ret_t retVal;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if (int_pri > RTK_PRIMAX )
        return RT_ERR_QOS_INT_PRIORITY;

    if (dscp > RTK_DSCPMAX)
        return RT_ERR_QOS_DSCP_VALUE;

    if ((retVal = rtl8367d_setAsicRegBits((RTL8367D_REG_QOS_DSCP_REMARK_CTRL0 + (int_pri >> 1)), (0x3F << (((int_pri) & 0x1) << 3)), dscp)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_qos_dscpRemark_get
 * Description:
 *      Get DSCP remarking parameter.
 * Input:
 *      int_pri - Internal priority value.
 * Output:
 *      Dscp - DSCP value.
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority.
 * Note:
 *      The API can get DSCP parameters. It would return DSCP value for mapping priority.
 */

rtk_api_ret_t dal_rtl8367d_qos_dscpRemark_get(rtk_pri_t int_pri, rtk_dscp_t *pDscp)
{
    rtk_api_ret_t retVal;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if (int_pri > RTK_PRIMAX )
        return RT_ERR_QOS_INT_PRIORITY;

    if ((retVal = rtl8367d_getAsicRegBits((RTL8367D_REG_QOS_DSCP_REMARK_CTRL0 + (int_pri >> 1)), (0x3F << (((int_pri) & 0x1) << 3)), pDscp)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_qos_dscpRemarkSrcSel_set
 * Description:
 *      Set remarking source of DSCP remarking.
 * Input:
 *      type      - remarking source
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT            - invalid input parameter

 * Note:
 *      The API can configure DSCP remark functionality to map original DSCP value or internal
 *      priority to TX DSCP value.
 */
rtk_api_ret_t dal_rtl8367d_qos_dscpRemarkSrcSel_set(rtk_qos_dscpRmkSrc_t type)
{
    rtk_api_ret_t retVal;
    rtk_uint32 regData;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if (type >= DSCP_RMK_SRC_END )
        return RT_ERR_QOS_INT_PRIORITY;

    if (type == DSCP_RMK_SRC_DSCP )
        return RT_ERR_QOS_INT_PRIORITY;

    switch (type)
    {
        case DSCP_RMK_SRC_INT_PRI:
            regData = 0;
            break;
        case DSCP_RMK_SRC_USER_PRI:
            regData = 1;
            break;
        default:
            return RT_ERR_QOS_INT_PRIORITY;
    }

    if ((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_RMK_CFG_SEL_CTRL, RTL8367D_RMK_DSCP_CFG_SEL_MASK, regData)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_qos_dscpRemarkSrcSel_get
 * Description:
 *      Get remarking source of DSCP remarking.
 * Input:
 *      none
 * Output:
 *      pType      - remarking source
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
rtk_api_ret_t dal_rtl8367d_qos_dscpRemarkSrcSel_get(rtk_qos_dscpRmkSrc_t *pType)
{
    rtk_api_ret_t retVal;
    rtk_uint32 regData;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if ((retVal = rtl8367d_getAsicRegBits(RTL8367D_REG_RMK_CFG_SEL_CTRL, RTL8367D_RMK_DSCP_CFG_SEL_MASK, &regData)) != RT_ERR_OK)
        return retVal;

    switch (regData)
    {
        case 0:
            *pType = DSCP_RMK_SRC_INT_PRI;
            break;
        case 1:
            *pType = DSCP_RMK_SRC_USER_PRI;
            break;
        default:
            return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_qos_schedulingType_set
 * Description:
 *      Set scheduling type.
 * Input:
 *      type      - scheduling type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter

 * Note:
 *      The API can configure QoS scheduling type.
 */
rtk_api_ret_t dal_rtl8367d_qos_schedulingType_set(rtk_qos_scheduling_type_t type)
{
    rtk_api_ret_t retVal;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if (type >= SCHEDULING_TYPE_END )
        return RT_ERR_QOS_SCHE_TYPE;

    if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_SCH_WRR_OPT, RTL8367D_CFG_WRR_MODE_OFFSET, type)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_qos_schedulingType_get
 * Description:
 *      Get type of scheduling.
 * Input:
 *      none
 * Output:
 *      pType      - scheduling type
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer

 * Note:
 *      The API can get QoS scheduling type
 */
rtk_api_ret_t dal_rtl8367d_qos_schedulingType_get(rtk_qos_scheduling_type_t *pType)
{
    rtk_api_ret_t retVal;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if ((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_SCH_WRR_OPT, RTL8367D_CFG_WRR_MODE_OFFSET, pType)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_qos_portPriSelIndex_set
 * Description:
 *      Configure priority decision index to each port.
 * Input:
 *      port - Port id.
 *      index - priority decision index.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_PORT_ID          - Invalid port number.
 *      RT_ERR_ENTRY_INDEX - Invalid entry index.
 * Note:
 *      The API can set priority of port assignments for queue usage and packet scheduling.
 */
rtk_api_ret_t dal_rtl8367d_qos_portPriSelIndex_set(rtk_port_t port, rtk_qos_priDecTbl_t index)
{
    rtk_api_ret_t retVal;
    rtk_uint32 phy_port;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check Port Valid */
    RTK_CHK_PORT_VALID(port);

    if (index >= PRIDECTBL_END )
        return RT_ERR_ENTRY_INDEX;

    phy_port = rtk_switch_port_L2P_get(port);

    if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_QOS_INTERNAL_PRIORITY_DECISION_IDX, phy_port, index)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_qos_portPriSelIndex_get
 * Description:
 *      Get priority decision index from each port.
 * Input:
 *      port - Port id.
 * Output:
 *      pIndex - priority decision index.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      The API can get priority of port assignments for queue usage and packet scheduling.
 */
rtk_api_ret_t dal_rtl8367d_qos_portPriSelIndex_get(rtk_port_t port, rtk_qos_priDecTbl_t *pIndex)
{
    rtk_api_ret_t retVal;
    rtk_uint32 phy_port;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check Port Valid */
    RTK_CHK_PORT_VALID(port);

    phy_port = rtk_switch_port_L2P_get(port);

    if ((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_QOS_INTERNAL_PRIORITY_DECISION_IDX, phy_port, pIndex)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}


