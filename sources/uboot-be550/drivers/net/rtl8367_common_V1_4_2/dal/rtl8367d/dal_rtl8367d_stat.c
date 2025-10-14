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
 * Feature : Here is a list of all functions and variables in MIB module.
 *
 */

#include <rtk_switch.h>
#include <rtk_error.h>
#include <dal/rtl8367d/dal_rtl8367d_stat.h>

#include <linux/string.h>

#include <dal/rtl8367d/rtl8367d_asicdrv.h>

#define MIB_NOT_SUPPORT     (0xFFFF)
static rtk_api_ret_t _get_asic_mib_idx(rtk_stat_port_type_t cnt_idx, RTL8367D_MIBCOUNTER *pMib_idx)
{
    RTL8367D_MIBCOUNTER mib_asic_idx[STAT_PORT_CNTR_END]=
    {
        ifInOctets,                     /* STAT_IfInOctets */
        dot3StatsFCSErrors,             /* STAT_Dot3StatsFCSErrors */
        dot3StatsSymbolErrors,          /* STAT_Dot3StatsSymbolErrors */
        dot3InPauseFrames,              /* STAT_Dot3InPauseFrames */
        dot3ControlInUnknownOpcodes,    /* STAT_Dot3ControlInUnknownOpcodes */
        etherStatsFragments,            /* STAT_EtherStatsFragments */
        etherStatsJabbers,              /* STAT_EtherStatsJabbers */
        ifInUcastPkts,                  /* STAT_IfInUcastPkts */
        etherStatsDropEvents,           /* STAT_EtherStatsDropEvents */
        etherStatsOctets,               /* STAT_EtherStatsOctets */
        etherStatsUnderSizePkts,        /* STAT_EtherStatsUnderSizePkts */
        etherOversizeStats,             /* STAT_EtherOversizeStats */
        etherStatsPkts64Octets,         /* STAT_EtherStatsPkts64Octets */
        etherStatsPkts65to127Octets,    /* STAT_EtherStatsPkts65to127Octets */
        etherStatsPkts128to255Octets,   /* STAT_EtherStatsPkts128to255Octets */
        etherStatsPkts256to511Octets,   /* STAT_EtherStatsPkts256to511Octets */
        etherStatsPkts512to1023Octets,  /* STAT_EtherStatsPkts512to1023Octets */
        etherStatsPkts1024to1518Octets, /* STAT_EtherStatsPkts1024to1518Octets */
        ifInMulticastPkts,              /* STAT_EtherStatsMulticastPkts */
        ifInBroadcastPkts,              /* STAT_EtherStatsBroadcastPkts */
        ifOutOctets,                    /* STAT_IfOutOctets */
        dot3StatsSingleCollisionFrames, /* STAT_Dot3StatsSingleCollisionFrames */
        dot3StatMultipleCollisionFrames,/* STAT_Dot3StatsMultipleCollisionFrames */
        dot3sDeferredTransmissions,     /* STAT_Dot3StatsDeferredTransmissions */
        dot3StatsLateCollisions,        /* STAT_Dot3StatsLateCollisions */
        etherStatsCollisions,           /* STAT_EtherStatsCollisions */
        dot3StatsExcessiveCollisions,   /* STAT_Dot3StatsExcessiveCollisions */
        dot3OutPauseFrames,             /* STAT_Dot3OutPauseFrames */
        MIB_NOT_SUPPORT,                /* STAT_Dot1dBasePortDelayExceededDiscards */
        dot1dTpPortInDiscards,          /* STAT_Dot1dTpPortInDiscards */
        ifOutUcastPkts,                 /* STAT_IfOutUcastPkts */
        ifOutMulticastPkts,             /* STAT_IfOutMulticastPkts */
        ifOutBroadcastPkts,             /* STAT_IfOutBroadcastPkts */
        outOampduPkts,                  /* STAT_OutOampduPkts */
        inOampduPkts,                   /* STAT_InOampduPkts */
        MIB_NOT_SUPPORT,                /* STAT_PktgenPkts */
        inMldChecksumError,             /* STAT_InMldChecksumError */
        inIgmpChecksumError,            /* STAT_InIgmpChecksumError */
        inMldSpecificQuery,             /* STAT_InMldSpecificQuery */
        inMldGeneralQuery,              /* STAT_InMldGeneralQuery */
        inIgmpSpecificQuery,            /* STAT_InIgmpSpecificQuery */
        inIgmpGeneralQuery,             /* STAT_InIgmpGeneralQuery */
        inMldLeaves,                    /* STAT_InMldLeaves */
        inIgmpLeaves,                   /* STAT_InIgmpInterfaceLeaves */
        inIgmpJoinsSuccess,             /* STAT_InIgmpJoinsSuccess */
        inIgmpJoinsFail,                /* STAT_InIgmpJoinsFail */
        inMldJoinsSuccess,              /* STAT_InMldJoinsSuccess */
        inMldJoinsFail,                 /* STAT_InMldJoinsFail */
        inReportSuppressionDrop,        /* STAT_InReportSuppressionDrop */
        inLeaveSuppressionDrop,         /* STAT_InLeaveSuppressionDrop */
        outIgmpReports,                 /* STAT_OutIgmpReports */
        outIgmpLeaves,                  /* STAT_OutIgmpLeaves */
        outIgmpGeneralQuery,            /* STAT_OutIgmpGeneralQuery */
        outIgmpSpecificQuery,           /* STAT_OutIgmpSpecificQuery */
        outMldReports,                  /* STAT_OutMldReports */
        outMldLeaves,                   /* STAT_OutMldLeaves */
        outMldGeneralQuery,             /* STAT_OutMldGeneralQuery */
        outMldSpecificQuery,            /* STAT_OutMldSpecificQuery */
        inKnownMulticastPkts,           /* STAT_InKnownMulticastPkts */
        ifInMulticastPkts,              /* STAT_IfInMulticastPkts */
        ifInBroadcastPkts,              /* STAT_IfInBroadcastPkts */
        ifOutDiscards                   /* STAT_IfOutDiscards */
    };

    if(cnt_idx >= STAT_PORT_CNTR_END)
        return RT_ERR_STAT_INVALID_PORT_CNTR;

    if(mib_asic_idx[cnt_idx] == MIB_NOT_SUPPORT)
        return RT_ERR_CHIP_NOT_SUPPORTED;

    *pMib_idx = mib_asic_idx[cnt_idx];
    return RT_ERR_OK;
}

static rtk_api_ret_t _rtl8367d_getMIBsCounter(rtk_uint32 port, RTL8367D_MIBCOUNTER mibIdx, rtk_uint64* pCounter)
{
    ret_t retVal;
    rtk_uint32 regAddr;
    rtk_uint32 regData;
    rtk_uint32 mibAddr;
    rtk_uint32 mibOff=0;

    /* address offset to MIBs counter */
    CONST rtk_uint16 mibLength[RTL8367D_MIBS_NUMBER]= {
        4,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
        4,2,2,2,2,2,2,2,2,
        4,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
        2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2};

    rtk_uint16 i;
    rtk_uint64 mibCounter;


    if(port > RTL8367D_PORTIDMAX)
        return RT_ERR_PORT_ID;

    if(mibIdx >= RTL8367D_MIBS_NUMBER)
        return RT_ERR_STAT_INVALID_CNTR;

    if(dot1dTpLearnedEntryDiscards == mibIdx)
    {
        mibAddr = RTL8367D_MIB_LEARNENTRYDISCARD_OFFSET;
    }
    else
    {
        i = 0;
        mibOff = RTL8367D_MIB_PORT_OFFSET * port;

        if(port > 7)
            mibOff = mibOff + 68;

        while(i < mibIdx)
        {
            mibOff += mibLength[i];
            i++;
        }

        mibAddr = mibOff;
    }

    /* Read MIB addr before writing */
    retVal = rtl8367d_getAsicReg(RTL8367D_REG_MIB_ADDRESS, &regData);
    if(retVal != RT_ERR_OK)
        return retVal;

    if (regData == (mibAddr >> 2))
    {
        /* Write MIB addr to an alternate value */
        retVal = rtl8367d_setAsicReg(RTL8367D_REG_MIB_ADDRESS, (mibAddr >> 2) + 1);
        if(retVal != RT_ERR_OK)
            return retVal;

        while(1)
        {
            retVal = rtl8367d_getAsicReg(RTL8367D_REG_MIB_ADDRESS, &regData);
            if(retVal != RT_ERR_OK)
                return retVal;

            if(regData == ((mibAddr >> 2) + 1))
            {
                break;
            }

            retVal = rtl8367d_setAsicReg(RTL8367D_REG_MIB_ADDRESS, (mibAddr >> 2) + 1);
            if(retVal != RT_ERR_OK)
                return retVal;
        }

        /* polling busy flag */
        i = 100;
        while(i > 0)
        {
            /*read MIB control register*/
            retVal = rtl8367d_getAsicReg(RTL8367D_REG_MIB_CTRL0,&regData);
            if(retVal != RT_ERR_OK)
                return retVal;

            if((regData & RTL8367D_MIB_CTRL0_BUSY_FLAG_MASK) == 0)
            {
                break;
            }

            i--;
        }

        if(regData & RTL8367D_MIB_CTRL0_BUSY_FLAG_MASK)
            return RT_ERR_BUSYWAIT_TIMEOUT;

        if(regData & RTL8367D_RESET_FLAG_MASK)
            return RT_ERR_STAT_CNTR_FAIL;
    }

    /*writing access counter address first*/
    /*This address is SRAM address, and SRAM address = MIB register address >> 2*/
    /*then ASIC will prepare 64bits counter wait for being retrived*/
    /*Write Mib related address to access control register*/
    retVal = rtl8367d_setAsicReg(RTL8367D_REG_MIB_ADDRESS, (mibAddr >> 2));
    if(retVal != RT_ERR_OK)
        return retVal;

    /* polling MIB Addr register */
    while(1)
    {
        retVal = rtl8367d_getAsicReg(RTL8367D_REG_MIB_ADDRESS, &regData);
        if(retVal != RT_ERR_OK)
            return retVal;

        if(regData == (mibAddr >> 2))
        {
            break;
        }

        retVal = rtl8367d_setAsicReg(RTL8367D_REG_MIB_ADDRESS, (mibAddr >> 2));
        if(retVal != RT_ERR_OK)
            return retVal;
    }

    /* polling busy flag */
    i = 100;
    while(i > 0)
    {
        /*read MIB control register*/
        retVal = rtl8367d_getAsicReg(RTL8367D_REG_MIB_CTRL0,&regData);
        if(retVal != RT_ERR_OK)
            return retVal;

        if((regData & RTL8367D_MIB_CTRL0_BUSY_FLAG_MASK) == 0)
        {
            break;
        }

        i--;
    }

    if(regData & RTL8367D_MIB_CTRL0_BUSY_FLAG_MASK)
        return RT_ERR_BUSYWAIT_TIMEOUT;

    if(regData & RTL8367D_RESET_FLAG_MASK)
        return RT_ERR_STAT_CNTR_FAIL;

    mibCounter = 0;
    i = mibLength[mibIdx];
    if(4 == i)
        regAddr = RTL8367D_REG_MIB_COUNTER0 + 3;
    else
        regAddr = RTL8367D_REG_MIB_COUNTER0 + ((mibOff + 1) % 4);

    while(i)
    {
        retVal = rtl8367d_getAsicReg(regAddr, &regData);
        if(retVal != RT_ERR_OK)
            return retVal;

        mibCounter = (mibCounter << 16) | (regData & 0xFFFF);

        regAddr --;
        i --;

    }

    *pCounter = mibCounter;

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rtl8367d_stat_global_reset
 * Description:
 *      Reset global MIB counter.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *      Reset MIB counter of ports. API will use global reset while port mask is all-ports.
 */
rtk_api_ret_t dal_rtl8367d_stat_global_reset(void)
{
    rtk_api_ret_t retVal;
    rtk_uint32 regData;
    rtk_uint32 regBits;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    regBits = RTL8367D_GLOBAL_RESET_MASK |
                RTL8367D_QM_RESET_MASK |
                    (0xFF<<RTL8367D_PORT0_RESET_OFFSET) |
                    ((rtk_uint32)0x7 << 13);
    regData = ((TRUE << RTL8367D_GLOBAL_RESET_OFFSET) & RTL8367D_GLOBAL_RESET_MASK) |
                ((FALSE << RTL8367D_QM_RESET_OFFSET) & RTL8367D_QM_RESET_MASK) |
                (((0 & 0xFF) << RTL8367D_PORT0_RESET_OFFSET) & (0xFF<<RTL8367D_PORT0_RESET_OFFSET)) |
                (((0 >> 8)&0x7) << 13);

    if ((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_MIB_CTRL0, regBits, (regData >> RTL8367D_PORT0_RESET_OFFSET))) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_stat_port_reset
 * Description:
 *      Reset per port MIB counter by port.
 * Input:
 *      port - port id.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *
 */
rtk_api_ret_t dal_rtl8367d_stat_port_reset(rtk_port_t port)
{
    rtk_api_ret_t retVal;
    rtk_uint32 portmask;
    rtk_uint32 regData;
    rtk_uint32 regBits;
    rtk_uint32 phyPort;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check port valid */
    RTK_CHK_PORT_VALID(port);

    phyPort = rtk_switch_port_L2P_get(port);
    if (phyPort == UNDEFINE_PHY_PORT)
        return RT_ERR_PORT_ID;

    portmask = 1 << phyPort;

    regBits = RTL8367D_GLOBAL_RESET_MASK |
                RTL8367D_QM_RESET_MASK |
                    (0xFF<<RTL8367D_PORT0_RESET_OFFSET) |
                    ((rtk_uint32)0x7 << 13);
    regData = ((FALSE << RTL8367D_GLOBAL_RESET_OFFSET) & RTL8367D_GLOBAL_RESET_MASK) |
                ((FALSE << RTL8367D_QM_RESET_OFFSET) & RTL8367D_QM_RESET_MASK) |
                (((portmask & 0xFF) << RTL8367D_PORT0_RESET_OFFSET) & (0xFF<<RTL8367D_PORT0_RESET_OFFSET)) |
                (((portmask >> 8)&0x7) << 13);

    if ((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_MIB_CTRL0, regBits, (regData >> RTL8367D_PORT0_RESET_OFFSET))) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_stat_queueManage_reset
 * Description:
 *      Reset queue manage MIB counter.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *
 */
rtk_api_ret_t dal_rtl8367d_stat_queueManage_reset(void)
{
    rtk_api_ret_t retVal;
    rtk_uint32 regData;
    rtk_uint32 regBits;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();


    regBits = RTL8367D_GLOBAL_RESET_MASK |
                RTL8367D_QM_RESET_MASK |
                    (0xFF<<RTL8367D_PORT0_RESET_OFFSET) |
                    ((rtk_uint32)0x7 << 13);
    regData = ((FALSE << RTL8367D_GLOBAL_RESET_OFFSET) & RTL8367D_GLOBAL_RESET_MASK) |
                ((TRUE << RTL8367D_QM_RESET_OFFSET) & RTL8367D_QM_RESET_MASK) |
                (((0 & 0xFF) << RTL8367D_PORT0_RESET_OFFSET) & (0xFF<<RTL8367D_PORT0_RESET_OFFSET)) |
                (((0 >> 8)&0x7) << 13);

    if ((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_MIB_CTRL0, regBits, (regData >> RTL8367D_PORT0_RESET_OFFSET))) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_stat_global_get
 * Description:
 *      Get global MIB counter
 * Input:
 *      cntr_idx - global counter index.
 * Output:
 *      pCntr - global counter value.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      Get global MIB counter by index definition.
 */
rtk_api_ret_t dal_rtl8367d_stat_global_get(rtk_stat_global_type_t cntr_idx, rtk_stat_counter_t *pCntr)
{
    rtk_api_ret_t retVal;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if(NULL == pCntr)
        return RT_ERR_NULL_POINTER;

    if (cntr_idx!=DOT1D_TP_LEARNED_ENTRY_DISCARDS_INDEX)
        return RT_ERR_STAT_INVALID_GLOBAL_CNTR;

    if ((retVal = _rtl8367d_getMIBsCounter(0, dot1dTpLearnedEntryDiscards, pCntr)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rtl8367d_stat_global_getAll
 * Description:
 *      Get all global MIB counter
 * Input:
 *      None
 * Output:
 *      pGlobal_cntrs - global counter structure.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      Get all global MIB counter by index definition.
 */
rtk_api_ret_t dal_rtl8367d_stat_global_getAll(rtk_stat_global_cntr_t *pGlobal_cntrs)
{
    rtk_api_ret_t retVal;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if(NULL == pGlobal_cntrs)
        return RT_ERR_NULL_POINTER;

    if ((retVal = _rtl8367d_getMIBsCounter(0, (RTL8367D_MIBCOUNTER)DOT1D_TP_LEARNED_ENTRY_DISCARDS_INDEX, &pGlobal_cntrs->dot1dTpLearnedEntryDiscards)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rtl8367d_stat_port_get
 * Description:
 *      Get per port MIB counter by index
 * Input:
 *      port        - port id.
 *      cntr_idx    - port counter index.
 * Output:
 *      pCntr - MIB retrived counter.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *      Get per port MIB counter by index definition.
 */
rtk_api_ret_t dal_rtl8367d_stat_port_get(rtk_port_t port, rtk_stat_port_type_t cntr_idx, rtk_stat_counter_t *pCntr)
{
    rtk_api_ret_t       retVal;
    RTL8367D_MIBCOUNTER mib_idx;
    rtk_stat_counter_t  second_cnt;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if(NULL == pCntr)
        return RT_ERR_NULL_POINTER;

    /* Check port valid */
    RTK_CHK_PORT_VALID(port);

    if (cntr_idx>=STAT_PORT_CNTR_END)
        return RT_ERR_STAT_INVALID_PORT_CNTR;

    if((retVal = _get_asic_mib_idx(cntr_idx, &mib_idx)) != RT_ERR_OK)
        return retVal;

    if(mib_idx == MIB_NOT_SUPPORT)
        return RT_ERR_CHIP_NOT_SUPPORTED;

    if ((retVal = _rtl8367d_getMIBsCounter(rtk_switch_port_L2P_get(port), mib_idx, pCntr)) != RT_ERR_OK)
        return retVal;

    if(cntr_idx == STAT_EtherStatsMulticastPkts)
    {
        if((retVal = _get_asic_mib_idx(STAT_IfOutMulticastPkts, &mib_idx)) != RT_ERR_OK)
            return retVal;

        if((retVal = _rtl8367d_getMIBsCounter(rtk_switch_port_L2P_get(port), mib_idx, &second_cnt)) != RT_ERR_OK)
            return retVal;

        *pCntr += second_cnt;
    }

    if(cntr_idx == STAT_EtherStatsBroadcastPkts)
    {
        if((retVal = _get_asic_mib_idx(STAT_IfOutBroadcastPkts, &mib_idx)) != RT_ERR_OK)
            return retVal;

        if((retVal = _rtl8367d_getMIBsCounter(rtk_switch_port_L2P_get(port), mib_idx, &second_cnt)) != RT_ERR_OK)
            return retVal;

        *pCntr += second_cnt;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_stat_port_getAll
 * Description:
 *      Get all counters of one specified port in the specified device.
 * Input:
 *      port - port id.
 * Output:
 *      pPort_cntrs - buffer pointer of counter value.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      Get all MIB counters of one port.
 */
rtk_api_ret_t dal_rtl8367d_stat_port_getAll(rtk_port_t port, rtk_stat_port_cntr_t *pPort_cntrs)
{
    rtk_api_ret_t retVal;
    rtk_uint32 mibIndex;
    rtk_uint64 mibCounter;
    rtk_uint32 *accessPtr;
    /* address offset to MIBs counter */
    CONST_T rtk_uint16 mibLength[STAT_PORT_CNTR_END]= {
        2,1,1,1,1,1,1,1,1,
        2,1,1,1,1,1,1,1,1,1,1,
        2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if(NULL == pPort_cntrs)
        return RT_ERR_NULL_POINTER;

    /* Check port valid */
    RTK_CHK_PORT_VALID(port);

    accessPtr = (rtk_uint32*)pPort_cntrs;
    for (mibIndex=0;mibIndex<STAT_PORT_CNTR_END;mibIndex++)
    {
        if ((retVal = dal_rtl8367d_stat_port_get(port, mibIndex, &mibCounter)) != RT_ERR_OK)
        {
            if (retVal == RT_ERR_CHIP_NOT_SUPPORTED)
                mibCounter = 0;
            else
                return retVal;
        }

        if (2 == mibLength[mibIndex])
            *(rtk_uint64*)accessPtr = mibCounter;
        else if (1 == mibLength[mibIndex])
            *accessPtr = mibCounter;
        else
            return RT_ERR_FAILED;

        accessPtr+=mibLength[mibIndex];
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_stat_logging_counterCfg_set
 * Description:
 *      Set the type and mode of Logging Counter
 * Input:
 *      idx     - The index of Logging Counter. Should be even number only.(0,2,4,6,8.....14)
 *      mode    - 32 bits or 64 bits mode
 *      type    - Packet counter or byte counter
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_OUT_OF_RANGE - Out of range.
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      Set the type and mode of Logging Counter.
 */
rtk_api_ret_t dal_rtl8367d_stat_logging_counterCfg_set(rtk_uint32 idx, rtk_logging_counter_mode_t mode, rtk_logging_counter_type_t type)
{
    rtk_api_ret_t retVal;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if(idx > RTL8367D_MIB_MAX_LOG_CNT_IDX)
        return RT_ERR_OUT_OF_RANGE;

    if((idx % 2) == 1)
        return RT_ERR_INPUT;

    if(mode >= LOGGING_MODE_END)
        return RT_ERR_OUT_OF_RANGE;

    if(type >= LOGGING_TYPE_END)
        return RT_ERR_OUT_OF_RANGE;

    if((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_MIB_CTRL5, (idx / 2),(rtk_uint32)type)) != RT_ERR_OK)
        return retVal;

    if((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_MIB_CTRL3, (idx / 2),(rtk_uint32)mode)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_stat_logging_counterCfg_get
 * Description:
 *      Get the type and mode of Logging Counter
 * Input:
 *      idx     - The index of Logging Counter. Should be even number only.(0,2,4,6,8.....14)
 * Output:
 *      pMode   - 32 bits or 64 bits mode
 *      pType   - Packet counter or byte counter
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_OUT_OF_RANGE - Out of range.
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_NULL_POINTER - NULL Pointer
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      Get the type and mode of Logging Counter.
 */
rtk_api_ret_t dal_rtl8367d_stat_logging_counterCfg_get(rtk_uint32 idx, rtk_logging_counter_mode_t *pMode, rtk_logging_counter_type_t *pType)
{
    rtk_api_ret_t   retVal;
    rtk_uint32      type, mode;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if(idx > RTL8367D_MIB_MAX_LOG_CNT_IDX)
        return RT_ERR_OUT_OF_RANGE;

    if((idx % 2) == 1)
        return RT_ERR_INPUT;

    if(pMode == NULL)
        return RT_ERR_NULL_POINTER;

    if(pType == NULL)
        return RT_ERR_NULL_POINTER;


    if((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_MIB_CTRL5, (idx / 2),&type)) != RT_ERR_OK)
        return retVal;

    if((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_MIB_CTRL3, (idx / 2),&mode)) != RT_ERR_OK)
        return retVal;

    *pMode = (rtk_logging_counter_mode_t)mode;
    *pType = (rtk_logging_counter_type_t)type;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_stat_logging_counter_reset
 * Description:
 *      Reset Logging Counter
 * Input:
 *      idx     - The index of Logging Counter. (0~15)
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_OUT_OF_RANGE - Out of range.
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *      Reset Logging Counter.
 */
rtk_api_ret_t dal_rtl8367d_stat_logging_counter_reset(rtk_uint32 idx)
{
    rtk_api_ret_t   retVal;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if(idx > RTL8367D_MIB_MAX_LOG_CNT_IDX)
        return RT_ERR_OUT_OF_RANGE;

    if((retVal = rtl8367d_setAsicReg(RTL8367D_REG_MIB_CTRL1, 1<<idx)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_stat_logging_counter_get
 * Description:
 *      Get Logging Counter
 * Input:
 *      idx     - The index of Logging Counter. (0~15)
 * Output:
 *      pCnt    - Logging counter value
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_OUT_OF_RANGE - Out of range.
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *      Get Logging Counter.
 */
rtk_api_ret_t dal_rtl8367d_stat_logging_counter_get(rtk_uint32 idx, rtk_uint32 *pCnt)
{
    rtk_api_ret_t   retVal;
    rtk_uint32 regAddr;
    rtk_uint32 regData;
    rtk_uint32 mibAddr;
    rtk_uint16 i;
    rtk_uint64 mibCounter;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if(NULL == pCnt)
        return RT_ERR_NULL_POINTER;

    if(idx > RTL8367D_MIB_MAX_LOG_CNT_IDX)
        return RT_ERR_OUT_OF_RANGE;

    mibAddr = RTL8367D_MIB_LOG_CNT_OFFSET + ((idx / 2) * 4);

    retVal = rtl8367d_setAsicReg(RTL8367D_REG_MIB_ADDRESS, (mibAddr >> 2));
    if(retVal != RT_ERR_OK)
        return retVal;

    /*read MIB control register*/
    retVal = rtl8367d_getAsicReg(RTL8367D_REG_MIB_CTRL0, &regData);
    if(retVal != RT_ERR_OK)
        return retVal;

    if(regData & RTL8367D_MIB_CTRL0_BUSY_FLAG_MASK)
        return RT_ERR_BUSYWAIT_TIMEOUT;

    if(regData & RTL8367D_RESET_FLAG_MASK)
        return RT_ERR_STAT_CNTR_FAIL;

    mibCounter = 0;
    if((idx % 2) == 1)
        regAddr = RTL8367D_REG_MIB_COUNTER0 + 3;
    else
        regAddr = RTL8367D_REG_MIB_COUNTER0 + 1;

    for(i = 0; i <= 1; i++)
    {
        retVal = rtl8367d_getAsicReg(regAddr, &regData);

        if(retVal != RT_ERR_OK)
            return retVal;

        mibCounter = (mibCounter << 16) | (regData & 0xFFFF);

        regAddr --;
    }

    *pCnt = mibCounter;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_stat_lengthMode_set
 * Description:
 *      Set Legnth mode.
 * Input:
 *      txMode     - The length counting mode
 *      rxMode     - The length counting mode
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_INPUT        - Out of range.
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *
 */
rtk_api_ret_t dal_rtl8367d_stat_lengthMode_set(rtk_stat_lengthMode_t txMode, rtk_stat_lengthMode_t rxMode)
{
    rtk_api_ret_t   retVal;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if(txMode >= LENGTH_MODE_END)
        return RT_ERR_INPUT;

    if(rxMode >= LENGTH_MODE_END)
        return RT_ERR_INPUT;

    if( (retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_MIB_RMON_LEN_CTRL, RTL8367D_TX_LENGTH_CTRL_OFFSET, (rtk_uint32)txMode)) != RT_ERR_OK)
        return retVal;

    if( (retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_MIB_RMON_LEN_CTRL, RTL8367D_RX_LENGTH_CTRL_OFFSET, (rtk_uint32)rxMode)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_stat_lengthMode_get
 * Description:
 *      Get Legnth mode.
 * Input:
 *      None.
 * Output:
 *      pTxMode       - The length counting mode
 *      pRxMode       - The length counting mode
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_INPUT        - Out of range.
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 */
rtk_api_ret_t dal_rtl8367d_stat_lengthMode_get(rtk_stat_lengthMode_t *pTxMode, rtk_stat_lengthMode_t *pRxMode)
{
    rtk_api_ret_t   retVal;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if(NULL == pTxMode)
        return RT_ERR_NULL_POINTER;

    if(NULL == pRxMode)
        return RT_ERR_NULL_POINTER;

    if( (retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_MIB_RMON_LEN_CTRL, RTL8367D_TX_LENGTH_CTRL_OFFSET, (rtk_uint32 *)pTxMode)) != RT_ERR_OK)
        return retVal;

    if( (retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_MIB_RMON_LEN_CTRL, RTL8367D_RX_LENGTH_CTRL_OFFSET, (rtk_uint32 *)pRxMode)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}


