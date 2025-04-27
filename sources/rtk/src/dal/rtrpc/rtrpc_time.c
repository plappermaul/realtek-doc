/*
 * Copyright(c) Realtek Semiconductor Corporation, 2008
 * All rights reserved.
 *
 * $Revision: 84354 $
 * $Date: 2017-12-12 15:53:27 +0800 (Tue, 12 Dec 2017) $
 *
 * Purpose : Realtek Switch SDK rtrpc API Module
 *
 * Feature : The file have include the following module and sub-modules
 *           1) Clock/Time
 *
 */

#include <rtk/time.h>
#include <dal/rtrpc/rtrpc_time.h>
#include <dal/rtrpc/rtrpc_msg.h>
#include <rtdrv/rtdrv_netfilter.h>


int32 rtrpc_time_portPtpEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    rtdrv_timeCfg_t time_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&time_cfg, 0, sizeof(time_cfg));

    time_cfg.unit = unit;
    time_cfg.port = port;
    GETSOCKOPT(RTDRV_TIME_PORT_PTP_ENABLE_GET, &time_cfg, rtdrv_timeCfg_t, 1);
    *pEnable = time_cfg.enable;

    return RT_ERR_OK;
}

int32 rtrpc_time_portPtpEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    rtdrv_timeCfg_t time_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&time_cfg, 0, sizeof(time_cfg));

    time_cfg.unit   = unit;
    time_cfg.port   = port;
    time_cfg.enable = enable;
    SETSOCKOPT(RTDRV_TIME_PORT_PTP_ENABLE_SET, &time_cfg, rtdrv_timeCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_time_portPtpRxTimestamp_get(uint32 unit, rtk_port_t port, rtk_time_ptpIdentifier_t identifier, rtk_time_timeStamp_t *pTimeStamp)
{
    rtdrv_timeCfg_t time_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&time_cfg, 0, sizeof(time_cfg));

    time_cfg.unit = unit;
    time_cfg.port = port;
    time_cfg.identifier = identifier;
    GETSOCKOPT(RTDRV_TIME_PORT_PTP_RX_TIME_GET, &time_cfg, rtdrv_timeCfg_t, 1);
    *pTimeStamp = time_cfg.timeStamp;

    return RT_ERR_OK;
}

int32 rtrpc_time_portPtpTxTimestamp_get(uint32 unit, rtk_port_t port, rtk_time_ptpIdentifier_t identifier, rtk_time_timeStamp_t *pTimeStamp)
{
    rtdrv_timeCfg_t time_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&time_cfg, 0, sizeof(time_cfg));

    time_cfg.unit = unit;
    time_cfg.port = port;
    time_cfg.identifier = identifier;
    GETSOCKOPT(RTDRV_TIME_PORT_PTP_TX_TIME_GET, &time_cfg, rtdrv_timeCfg_t, 1);
    *pTimeStamp = time_cfg.timeStamp;

    return RT_ERR_OK;
}

int32 rtrpc_time_portRefTime_get(uint32 unit, rtk_port_t port, rtk_time_timeStamp_t *pTimeStamp)
{
    rtdrv_timeCfg_t time_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&time_cfg, 0, sizeof(time_cfg));

    time_cfg.unit = unit;
    time_cfg.port = port;
    GETSOCKOPT(RTDRV_TIME_PORT_REF_TIME_GET, &time_cfg, rtdrv_timeCfg_t, 1);
    *pTimeStamp = time_cfg.timeStamp;

    return RT_ERR_OK;
}

int32 rtrpc_time_portRefTime_set(uint32 unit, rtk_portmask_t portmask, rtk_time_timeStamp_t timeStamp, uint32 exec)
{
    rtdrv_timeCfg_t time_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&time_cfg, 0, sizeof(time_cfg));

    time_cfg.unit = unit;
    time_cfg.portmask = portmask;
    time_cfg.timeStamp = timeStamp;
     time_cfg.enable = exec;
    SETSOCKOPT(RTDRV_TIME_PORT_REF_TIME_SET, &time_cfg, rtdrv_timeCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_time_portRefTimeAdjust_set(uint32 unit, rtk_portmask_t portmask, uint32 sign, rtk_time_timeStamp_t timeStamp, uint32 exec)
{
    rtdrv_timeCfg_t time_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&time_cfg, 0, sizeof(time_cfg));

    time_cfg.unit = unit;
    time_cfg.portmask = portmask;
    time_cfg.sign = sign;
    time_cfg.timeStamp = timeStamp;
    time_cfg.enable = exec;
    SETSOCKOPT(RTDRV_TIME_PORT_REF_TIME_ADJUST_SET, &time_cfg, rtdrv_timeCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_time_portRefTimeEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    rtdrv_timeCfg_t time_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&time_cfg, 0, sizeof(time_cfg));

    time_cfg.unit = unit;
    time_cfg.port = port;
    GETSOCKOPT(RTDRV_TIME_PORT_REF_TIME_ENABLE_GET, &time_cfg, rtdrv_timeCfg_t, 1);
    *pEnable = time_cfg.enable;

    return RT_ERR_OK;
}

int32 rtrpc_time_portRefTimeEnable_set(uint32 unit, rtk_portmask_t portmask, rtk_enable_t enable)
{
    rtdrv_timeCfg_t time_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&time_cfg, 0, sizeof(time_cfg));

    time_cfg.unit = unit;
    time_cfg.portmask = portmask;
    time_cfg.enable = enable;
    SETSOCKOPT(RTDRV_TIME_PORT_REF_TIME_ENABLE_SET, &time_cfg, rtdrv_timeCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_time_portRefTimeFreq_get(uint32 unit, rtk_port_t port, uint32 *pFreq)
{
    rtdrv_timeCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(cfg));
    cfg.unit = unit;
    cfg.port = port;
    GETSOCKOPT(RTDRV_TIME_PORT_REF_TIME_FREQ_GET, &cfg, rtdrv_timeCfg_t, 1);
    *pFreq = cfg.freq;

    return RT_ERR_OK;
}


int32
rtrpc_time_portRefTimeFreq_set(uint32 unit, rtk_portmask_t portmask, uint32 freq)
{
    rtdrv_timeCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(cfg));
    cfg.unit = unit;
    cfg.portmask = portmask;
    cfg.freq = freq;
    SETSOCKOPT(RTDRV_TIME_PORT_REF_TIME_FREQ_SET, &cfg, rtdrv_timeCfg_t, 1);

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_time_correctionFieldTransparentValue_get
 * Description:
 *      Get the transparent value of correction field for transparent clock.
 * Input:
 *      oriCf   - original correction field value
 *      rxTimeStamp   - RX time stamp of the packet
 * Output:
 *      pTransCf - pointer to transparent correction field value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      This API calculate the value of transparent correction field by original correction field - rxTimeStamp.
 *      Software should replace the correction field in packet with the transparent correction field and send the packet.
 *      Hardware would replace the correction field by the result of TX TimeStamp - RX TimeStamp + original correction field.
 */
int32
rtrpc_time_correctionFieldTransparentValue_get(uint32 unit, int64 oriCf, rtk_time_timeStamp_t rxTimeStamp, int64 *pTransCf)
{
    rtdrv_timeCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pTransCf), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(cfg));
    cfg.unit = unit;
    cfg.oriCf = oriCf;
    osal_memcpy(&cfg.rxTimeStamp, &rxTimeStamp, sizeof(rtk_time_timeStamp_t));

    GETSOCKOPT(RTDRV_TIME_CORRECTION_FIELD_TRANSPARENT_VALUE_GET, &cfg, rtdrv_timeCfg_t, 1);
    *pTransCf = cfg.transCf;

    return RT_ERR_OK;
}   /* end of rtk_time_correctionFieldTransparentValue_get */


int32
rtrpc_time_portPtpMacAddr_get(uint32 unit, rtk_port_t port, rtk_mac_t *pMac)
{
    rtdrv_timeCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pMac), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(cfg));
    cfg.unit = unit;
    cfg.port = port;

    GETSOCKOPT(RTDRV_TIME_PORT_MAC_GET, &cfg, rtdrv_timeCfg_t, 1);
    osal_memcpy(pMac, &cfg.mac, sizeof(cfg.mac));

    return RT_ERR_OK;
} /* end of rtrpc_time_portPtpMacAddr_get */

int32
rtrpc_time_portPtpMacAddr_set(uint32 unit, rtk_port_t port, rtk_mac_t *pMac)
{
    rtdrv_timeCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(cfg));
    cfg.unit = unit;
    cfg.port = port;
    osal_memcpy(&cfg.mac, pMac, sizeof(cfg.mac));

    SETSOCKOPT(RTDRV_TIME_PORT_MAC_SET, &cfg, rtdrv_timeCfg_t, 1);

    return RT_ERR_OK;
} /* end of rtrpc_time_ptpMacAddr_set */


int32
rtrpc_time_portPtpMacAddrRange_get(uint32 unit, rtk_port_t port, uint32 *pRange)
{
    rtdrv_timeCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRange), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(cfg));
    cfg.unit = unit;
    cfg.port = port;

    GETSOCKOPT(RTDRV_TIME_PORT_MAC_RANGE_GET, &cfg, rtdrv_timeCfg_t, 1);
    *pRange = cfg.range;

    return RT_ERR_OK;
} /* end of rtrpc_time_portPtpMacAddrRange_get */

int32
rtrpc_time_portPtpMacAddrRange_set(uint32 unit, rtk_port_t port, uint32 range)
{
    rtdrv_timeCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(cfg));
    cfg.unit = unit;
    cfg.port = port;
    cfg.range = range;

    SETSOCKOPT(RTDRV_TIME_PORT_MAC_RANGE_SET, &cfg, rtdrv_timeCfg_t, 1);

    return RT_ERR_OK;
} /* end of rtrpc_time_ptpMacAddrRange_set */

int32
rtrpc_time_portPtpVlanTpid_get(uint32 unit, rtk_port_t port, rtk_vlanType_t type, uint32 idx, uint32 *pTpid)
{
    rtdrv_timeCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pTpid), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(cfg));
    cfg.unit = unit;
    cfg.port = port;
    cfg.type = type;
    cfg.idx = idx;

    GETSOCKOPT(RTDRV_TIME_PORT_VLAN_TPID_GET, &cfg, rtdrv_timeCfg_t, 1);
    *pTpid = cfg.tpid;

    return RT_ERR_OK;
} /* end of rtrpc_time_portPtpVlanTpid_get */

int32
rtrpc_time_portPtpVlanTpid_set(uint32 unit, rtk_port_t port, rtk_vlanType_t type, uint32 idx, uint32 tpid)
{
    rtdrv_timeCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(cfg));
    cfg.unit = unit;
    cfg.port = port;
    cfg.type = type;
    cfg.idx = idx;
    cfg.tpid = tpid;

    SETSOCKOPT(RTDRV_TIME_PORT_VLAN_TPID_SET, &cfg, rtdrv_timeCfg_t, 1);

    return RT_ERR_OK;
} /* end of rtrpc_time_portPtpVlanTpid_get */

int32
rtrpc_time_portPtpOper_get(uint32 unit, rtk_port_t port, rtk_time_operCfg_t *pOperCfg)
{
    rtdrv_timeCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pOperCfg), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(cfg));
    cfg.unit = unit;
    cfg.port = port;

    GETSOCKOPT(RTDRV_TIME_PORT_OPER_GET, &cfg, rtdrv_timeCfg_t, 1);
    osal_memcpy(pOperCfg, &cfg.operCfg, sizeof(cfg.operCfg));

    return RT_ERR_OK;
} /* end of rtrpc_time_portPtpOper_get */


int32
rtrpc_time_portPtpOper_set(uint32 unit, rtk_port_t port, rtk_time_operCfg_t *pOperCfg)
{
    rtdrv_timeCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(cfg));
    cfg.unit = unit;
    cfg.port = port;
    osal_memcpy(&cfg.operCfg, pOperCfg, sizeof(cfg.operCfg));

    SETSOCKOPT(RTDRV_TIME_PORT_OPER_SET, &cfg, rtdrv_timeCfg_t, 1);

    return RT_ERR_OK;
} /* end of rtrpc_time_portPtpOper_set */


int32
rtrpc_time_portPtpLatchTime_get(uint32 unit, rtk_port_t port, rtk_time_timeStamp_t *pLatchTime)
{
    rtdrv_timeCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pLatchTime), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(cfg));
    cfg.unit = unit;
    cfg.port = port;

    GETSOCKOPT(RTDRV_TIME_PORT_LATCH_TIME_GET, &cfg, rtdrv_timeCfg_t, 1);
    osal_memcpy(pLatchTime, &cfg.timeStamp, sizeof(cfg.operCfg));

    return RT_ERR_OK;
} /* end of rtrpc_time_portPtpLatchTime_get */

int32
rtrpc_time_portPtpRefTimeFreqCfg_get(uint32 unit, rtk_port_t port, uint32 *pFreqCfg, uint32 *pFreqCur)
{
    rtdrv_timeCfg_t time_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&time_cfg, 0, sizeof(time_cfg));
    time_cfg.unit = unit;
    time_cfg.port = port;
    GETSOCKOPT(RTDRV_TIME_PORT_PTP_REF_TIME_FREQ_CFG_GET, &time_cfg, rtdrv_timeCfg_t, 1);
    *pFreqCfg = time_cfg.freq;
    *pFreqCur = time_cfg.data;

    return RT_ERR_OK;
}

int32
rtrpc_time_portPtpRefTimeFreqCfg_set(uint32 unit, rtk_port_t port, uint32 freq, uint32 apply)
{
    rtdrv_timeCfg_t time_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&time_cfg, 0, sizeof(time_cfg));
    time_cfg.unit = unit;
    time_cfg.port = port;
    time_cfg.freq = freq;
    time_cfg.data = apply;
    SETSOCKOPT(RTDRV_TIME_PORT_PTP_REF_TIME_FREQ_CFG_SET, &time_cfg, rtdrv_timeCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_time_portPtpTxInterruptStatus_get(uint32 unit, rtk_port_t port, uint32 *pIntrSts)
{
    rtdrv_timeCfg_t time_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&time_cfg, 0, sizeof(time_cfg));
    time_cfg.unit = unit;
    time_cfg.port = port;
    GETSOCKOPT(RTDRV_TIME_PORT_PTP_TX_INTERRUPT_STATUS_GET, &time_cfg, rtdrv_timeCfg_t, 1);
    *pIntrSts = time_cfg.data;

    return RT_ERR_OK;
}

int32
rtrpc_time_portPtpInterruptEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    rtdrv_timeCfg_t time_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&time_cfg, 0, sizeof(time_cfg));
    time_cfg.unit = unit;
    time_cfg.port = port;
    GETSOCKOPT(RTDRV_TIME_PORT_PTP_INTERRUPT_ENABLE_GET, &time_cfg, rtdrv_timeCfg_t, 1);
    *pEnable = time_cfg.enable;

    return RT_ERR_OK;
}

int32
rtrpc_time_portPtpInterruptEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    rtdrv_timeCfg_t time_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&time_cfg, 0, sizeof(time_cfg));
    time_cfg.unit = unit;
    time_cfg.port = port;
    time_cfg.enable = enable;
    SETSOCKOPT(RTDRV_TIME_PORT_PTP_INTERRUPT_ENABLE_SET, &time_cfg, rtdrv_timeCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_time_portPtpTxTimestampFifo_get(uint32 unit, rtk_port_t port, rtk_time_txTimeEntry_t *pTimeEntry)
{
    rtdrv_timeCfg_t time_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pTimeEntry), RT_ERR_NULL_POINTER);

    osal_memset(&time_cfg, 0, sizeof(time_cfg));
    time_cfg.unit = unit;
    time_cfg.port = port;
    GETSOCKOPT(RTDRV_TIME_PORT_PTP_TX_TIMESTAMP_FIFO_GET, &time_cfg, rtdrv_timeCfg_t, 1);
    osal_memcpy(pTimeEntry, &time_cfg.txTimeEntry, sizeof(time_cfg.txTimeEntry));

    return RT_ERR_OK;
}

int32
rtrpc_time_portPtp1PPSOutput_get(uint32 unit, rtk_port_t port, uint32 *pPulseWidth, rtk_enable_t *pEnable)
{
    rtdrv_timeCfg_t time_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&time_cfg, 0, sizeof(time_cfg));
    time_cfg.unit = unit;
    time_cfg.port = port;
    GETSOCKOPT(RTDRV_TIME_PORT_PTP_1PPS_OUTPUT_GET, &time_cfg, rtdrv_timeCfg_t, 1);
    *pPulseWidth = time_cfg.data;
    *pEnable = time_cfg.enable;

    return RT_ERR_OK;
}

int32
rtrpc_time_portPtp1PPSOutput_set(uint32 unit, rtk_port_t port, uint32 pulseWidth, rtk_enable_t enable)
{
    rtdrv_timeCfg_t time_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&time_cfg, 0, sizeof(time_cfg));
    time_cfg.unit = unit;
    time_cfg.port = port;
    time_cfg.data = pulseWidth;
    time_cfg.enable = enable;
    SETSOCKOPT(RTDRV_TIME_PORT_PTP_1PPS_OUTPUT_SET, &time_cfg, rtdrv_timeCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_time_portPtpClockOutput_get(uint32 unit, rtk_port_t port, rtk_time_clkOutput_t *pClkOutput)
{
    rtdrv_timeCfg_t time_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pClkOutput), RT_ERR_NULL_POINTER);

    osal_memset(&time_cfg, 0, sizeof(time_cfg));
    time_cfg.unit = unit;
    time_cfg.port = port;
    GETSOCKOPT(RTDRV_TIME_PORT_PTP_CLOCK_OUTPUT_GET, &time_cfg, rtdrv_timeCfg_t, 1);
    osal_memcpy(pClkOutput, &time_cfg.clkOutput, sizeof(time_cfg.clkOutput));

    return RT_ERR_OK;
}

int32
rtrpc_time_portPtpClockOutput_set(uint32 unit, rtk_port_t port, rtk_time_clkOutput_t *pClkOutput)
{
    rtdrv_timeCfg_t time_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&time_cfg, 0, sizeof(time_cfg));
    time_cfg.unit = unit;
    time_cfg.port = port;
    osal_memcpy(&time_cfg.clkOutput, pClkOutput, sizeof(time_cfg.clkOutput));
    SETSOCKOPT(RTDRV_TIME_PORT_PTP_CLOCK_OUTPUT_SET, &time_cfg, rtdrv_timeCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_time_portPtpOutputSigSel_get(uint32 unit, rtk_port_t port, rtk_time_outSigSel_t *pOutSigSel)
{
    rtdrv_timeCfg_t time_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&time_cfg, 0, sizeof(time_cfg));
    time_cfg.unit = unit;
    time_cfg.port = port;
    GETSOCKOPT(RTDRV_TIME_PORT_PTP_OUTPUT_SIG_SEL_GET, &time_cfg, rtdrv_timeCfg_t, 1);
    *pOutSigSel = time_cfg.data;

    return RT_ERR_OK;
}

int32
rtrpc_time_portPtpOutputSigSel_set(uint32 unit, rtk_port_t port, rtk_time_outSigSel_t outSigSel)
{
    rtdrv_timeCfg_t time_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&time_cfg, 0, sizeof(time_cfg));
    time_cfg.unit = unit;
    time_cfg.port = port;
    time_cfg.data = outSigSel;
    SETSOCKOPT(RTDRV_TIME_PORT_PTP_OUTPUT_SIG_SEL_SET, &time_cfg, rtdrv_timeCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_time_portPtpTransEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    rtdrv_timeCfg_t time_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&time_cfg, 0, sizeof(time_cfg));
    time_cfg.unit = unit;
    time_cfg.port = port;
    GETSOCKOPT(RTDRV_TIME_PORT_PTP_TRANS_ENABLE_GET, &time_cfg, rtdrv_timeCfg_t, 1);
    *pEnable = time_cfg.enable;

    return RT_ERR_OK;
}

int32
rtrpc_time_portPtpTransEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    rtdrv_timeCfg_t time_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&time_cfg, 0, sizeof(time_cfg));
    time_cfg.unit = unit;
    time_cfg.port = port;
    time_cfg.enable = enable;
    SETSOCKOPT(RTDRV_TIME_PORT_PTP_TRANS_ENABLE_SET, &time_cfg, rtdrv_timeCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_time_portPtpLinkDelay_get(uint32 unit, rtk_port_t port, uint32 *pLinkDelay)
{
    rtdrv_timeCfg_t time_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&time_cfg, 0, sizeof(time_cfg));
    time_cfg.unit = unit;
    time_cfg.port = port;
    GETSOCKOPT(RTDRV_TIME_PORT_PTP_LINK_DELAY_GET, &time_cfg, rtdrv_timeCfg_t, 1);
    *pLinkDelay = time_cfg.data;

    return RT_ERR_OK;
}

int32
rtrpc_time_portPtpLinkDelay_set(uint32 unit, rtk_port_t port, uint32 linkDelay)
{
    rtdrv_timeCfg_t time_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&time_cfg, 0, sizeof(time_cfg));
    time_cfg.unit = unit;
    time_cfg.port = port;
    time_cfg.data = linkDelay;
    SETSOCKOPT(RTDRV_TIME_PORT_PTP_LINK_DELAY_SET, &time_cfg, rtdrv_timeCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_time_portPtpFullSec_get(uint32 unit, rtk_port_t port, uint64 partialSec,
    uint32 bitLen, uint64 *pFullSec)
{
    rtdrv_timeCfg_t cfg;
    uint32 master_view_unit = unit;

    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pFullSec), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_timeCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&cfg.partialSec, &partialSec, sizeof(uint64));
    osal_memcpy(&cfg.bitLen, &bitLen, sizeof(uint32));
    GETSOCKOPT(RTDRV_TIME_PORTPTPFULLSEC_GET, &cfg, rtdrv_timeCfg_t, 1);
    osal_memcpy(pFullSec, &cfg.fullSec, sizeof(uint64));

    return RT_ERR_OK;
}   /* end of rtrpc_time_portPtpFullSec_get */

int32
rtrpc_timeMapper_init(dal_mapper_t *pMapper)
{
    pMapper->time_portPtpEnable_get = rtrpc_time_portPtpEnable_get;
    pMapper->time_portPtpEnable_set = rtrpc_time_portPtpEnable_set;
    pMapper->time_portPtpRxTimestamp_get = rtrpc_time_portPtpRxTimestamp_get;
    pMapper->time_portPtpTxTimestamp_get = rtrpc_time_portPtpTxTimestamp_get;
    pMapper->time_portRefTime_get = rtrpc_time_portRefTime_get;
    pMapper->time_portRefTime_set = rtrpc_time_portRefTime_set;
    pMapper->time_portRefTimeAdjust_set = rtrpc_time_portRefTimeAdjust_set;
    pMapper->time_portRefTimeEnable_get = rtrpc_time_portRefTimeEnable_get;
    pMapper->time_portRefTimeEnable_set = rtrpc_time_portRefTimeEnable_set;
    pMapper->time_portRefTimeFreq_get = rtrpc_time_portRefTimeFreq_get;
    pMapper->time_portRefTimeFreq_set = rtrpc_time_portRefTimeFreq_set;
    pMapper->time_portPtpMacAddr_get = rtrpc_time_portPtpMacAddr_get;
    pMapper->time_portPtpMacAddr_set = rtrpc_time_portPtpMacAddr_set;
    pMapper->time_portPtpMacAddrRange_get = rtrpc_time_portPtpMacAddrRange_get;
    pMapper->time_portPtpMacAddrRange_set = rtrpc_time_portPtpMacAddrRange_set;
    pMapper->time_portPtpVlanTpid_get = rtrpc_time_portPtpVlanTpid_get;
    pMapper->time_portPtpVlanTpid_set = rtrpc_time_portPtpVlanTpid_set;
    pMapper->time_portPtpOper_get = rtrpc_time_portPtpOper_get;
    pMapper->time_portPtpOper_set = rtrpc_time_portPtpOper_set;
    pMapper->time_portPtpLatchTime_get = rtrpc_time_portPtpLatchTime_get;
    pMapper->time_portPtpRefTimeFreqCfg_get = rtrpc_time_portPtpRefTimeFreqCfg_get;
    pMapper->time_portPtpRefTimeFreqCfg_set = rtrpc_time_portPtpRefTimeFreqCfg_set;
    pMapper->time_portPtpTxInterruptStatus_get = rtrpc_time_portPtpTxInterruptStatus_get;
    pMapper->time_portPtpInterruptEnable_get = rtrpc_time_portPtpInterruptEnable_get;
    pMapper->time_portPtpInterruptEnable_set = rtrpc_time_portPtpInterruptEnable_set;
    pMapper->time_portPtpTxTimestampFifo_get = rtrpc_time_portPtpTxTimestampFifo_get;
    pMapper->time_portPtp1PPSOutput_get = rtrpc_time_portPtp1PPSOutput_get;
    pMapper->time_portPtp1PPSOutput_set = rtrpc_time_portPtp1PPSOutput_set;
    pMapper->time_portPtpClockOutput_get = rtrpc_time_portPtpClockOutput_get;
    pMapper->time_portPtpClockOutput_set = rtrpc_time_portPtpClockOutput_set;
    pMapper->time_portPtpOutputSigSel_get = rtrpc_time_portPtpOutputSigSel_get;
    pMapper->time_portPtpOutputSigSel_set = rtrpc_time_portPtpOutputSigSel_set;
    pMapper->time_portPtpTransEnable_get = rtrpc_time_portPtpTransEnable_get;
    pMapper->time_portPtpTransEnable_set = rtrpc_time_portPtpTransEnable_set;
    pMapper->time_portPtpLinkDelay_get = rtrpc_time_portPtpLinkDelay_get;
    pMapper->time_portPtpLinkDelay_set = rtrpc_time_portPtpLinkDelay_set;
    pMapper->time_portPtpFullSec_get = rtrpc_time_portPtpFullSec_get;

    return RT_ERR_OK;
}



