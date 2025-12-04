/*
 * Copyright (C) 2012 Realtek Semiconductor Corp.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Realtek Switch SDK Rtusr API Module
 *
 * Feature : The file include the debug tools
 *           1) packet send/receive
 *
 */

/*
 * Include Files
 */
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <osal/lib.h>
#include <rtk/rtusr/include/rtusr_util.h>
#include <rtdrv/rtdrv_netfilter.h>
#include <rtk/rtusr/rtusr_pkt.h>

/*
 * Symbol Definition
 */

/*
 * Macro Declaration
 */

/*
 * Data Declaration
 */
static struct pkt_dbg_tx_info txInfo;
static uint16 pkt_length = 0;
static unsigned char pkt_payload[PKT_DEBUG_PKT_LENGTH_MAX] = { 0 };

/*
 * Function Declaration
 */
int32 rtk_pkt_rxDump_get(uint8 *pPayload, uint16 max_length, uint16 *pPkt_length, struct pkt_dbg_rx_info *pInfo, uint16 *pEnable)
{
    rtdrv_pktdbg_t *pPkt_info;
    int32 ret;
    
    pPkt_info = (rtdrv_pktdbg_t *)malloc(sizeof(rtdrv_pktdbg_t));
    if(NULL == pPkt_info)
    {
        return RT_ERR_NULL_POINTER;
    }
    pPkt_info->length = max_length;
    GETSOCKOPT_WITH_RET(RTDRV_PKT_RXDUMP_GET, pPkt_info, rtdrv_pktdbg_t, 1, ret);
    if(ret!=0)
    {
        free(pPkt_info);
        return ret;    
    }

    memcpy(pPayload, &pPkt_info->buf[0], max_length);
    *pInfo = pPkt_info->rx_info;
    *pPkt_length = pPkt_info->rx_length;
    *pEnable = pPkt_info->enable;
    free(pPkt_info);

    return RT_ERR_OK;
}

int32 rtk_pkt_rxFifoDump_get(uint16 fifo_idx, uint8 *pPayload)
{
    int32 ret;
    rtdrv_pktfifodbg_t *pPkt;

    pPkt = (rtdrv_pktfifodbg_t *)malloc(sizeof(rtdrv_pktfifodbg_t));
    if(NULL == pPkt)
    {
        return RT_ERR_NULL_POINTER;
    }
    pPkt->fifo_idx = fifo_idx;
    GETSOCKOPT_WITH_RET(RTDRV_PKTFIFO_RXDUMP_GET, pPkt, rtdrv_pktfifodbg_t, 1, ret);

    if(0!=ret)
    {
        free(pPkt);
        return ret;    
    }


    if(pPkt->valid)
    {
        memcpy(pPayload, &pPkt->buf[0], sizeof(pPkt->buf));
        ret = RT_ERR_OK;
    }
    else
    {
        ret = RT_ERR_FAILED;
    }
    
    free(pPkt);

    return ret;
}

int32 rtk_pkt_rxDump_clear(void)
{
    int32 dummy=0;
    SETSOCKOPT(RTDRV_PKT_RXDUMP_CLEAR, &dummy, uint32, 1);
    return RT_ERR_OK;
}

int32 rtk_pkt_rxDumpEnable_set(uint32 enable)
{
    SETSOCKOPT(RTDRV_PKT_RXDUMP_ENABLE, &enable, uint32, 1);
    return RT_ERR_OK;
}

#if defined(CONFIG_SDK_RTL9607C)
int32 rtk_pkt_rxGmacNum_set(uint32 gmac)
{
    SETSOCKOPT(RTDRV_PKT_RXGMAC_SET, &gmac, uint32, 1);
    return RT_ERR_OK;
}

int32 rtk_pkt_rxGmacNum_get(uint32 *pGmac)
{
    uint32 gmac;
    if(NULL == pGmac)
    {
        return RT_ERR_INPUT;
    }
    GETSOCKOPT(RTDRV_PKT_RXGMAC_GET, &gmac, uint32, 1);
    *pGmac = gmac;
    return RT_ERR_OK;
}

#endif

int32 rtk_pkt_txAddr_set(rtk_mac_t *pDst, rtk_mac_t *pSrc)
{
    if((NULL == pDst) || (NULL == pSrc))
    {
        return RT_ERR_NULL_POINTER;
    }

    /* Set destination mac address */
    pkt_payload[0] = pDst->octet[0];
    pkt_payload[1] = pDst->octet[1];
    pkt_payload[2] = pDst->octet[2];
    pkt_payload[3] = pDst->octet[3];
    pkt_payload[4] = pDst->octet[4];
    pkt_payload[5] = pDst->octet[5];

    /* Set source mac address */
    pkt_payload[6] = pSrc->octet[0];
    pkt_payload[7] = pSrc->octet[1];
    pkt_payload[8] = pSrc->octet[2];
    pkt_payload[9] = pSrc->octet[3];
    pkt_payload[10] = pSrc->octet[4];
    pkt_payload[11] = pSrc->octet[5];

    if(pkt_length < PKT_DEBUG_PKT_MACADDR_OFFSET)
        pkt_length = PKT_DEBUG_PKT_MACADDR_OFFSET;

    return RT_ERR_OK;
}

int32 rtk_pkt_txAddr_get(rtk_mac_t *pDst, rtk_mac_t *pSrc)
{
    if((NULL == pDst) || (NULL == pSrc))
    {
        return RT_ERR_NULL_POINTER;
    }

    /* Set destination mac address */
    pDst->octet[0] = pkt_payload[0];
    pDst->octet[1] = pkt_payload[1];
    pDst->octet[2] = pkt_payload[2];
    pDst->octet[3] = pkt_payload[3];
    pDst->octet[4] = pkt_payload[4];
    pDst->octet[5] = pkt_payload[5];

    /* Set source mac address */
    pSrc->octet[0] = pkt_payload[6]; 
    pSrc->octet[1] = pkt_payload[7]; 
    pSrc->octet[2] = pkt_payload[8]; 
    pSrc->octet[3] = pkt_payload[9]; 
    pSrc->octet[4] = pkt_payload[10];
    pSrc->octet[5] = pkt_payload[11];
    
    return RT_ERR_OK;
}

int32 rtk_pkt_txPkt_set(uint16 pos, uint8 *pData, uint16 length)
{
    uint16 write_length;
    
    if(NULL == pData)
        return RT_ERR_NULL_POINTER;

    if(pos + length > PKT_DEBUG_PKT_LENGTH_MAX)
        write_length = PKT_DEBUG_PKT_LENGTH_MAX - pos;
    else
        write_length = length;

    osal_memcpy(&pkt_payload[pos], pData, write_length);

    if(write_length + pos > pkt_length)
    {
        pkt_length = write_length + pos;
    }

    return RT_ERR_OK;
}

int32 rtk_pkt_txPadding_set(uint16 start, uint16 end, uint16 length)
{
    int i;
    uint16 total_length;
    uint16 pos;

    if(length >= PKT_DEBUG_PKT_LENGTH_MAX)
        total_length = PKT_DEBUG_PKT_LENGTH_MAX;
    else
        total_length = length;

    if(end == 0)
    {
        for(i = start; i < total_length; i++)
        {
            pkt_payload[i] = 0;
        }
    }
    else
    {
        if(start > end)
            return RT_ERR_INPUT;

        pos = end + 1;
        for(i = start; pos < total_length; i++, pos++)
        {
            if(i > end)
                i = start;
            
            pkt_payload[pos] = pkt_payload[i];
        }
    }
    pkt_length = total_length;

    return RT_ERR_OK;
}

int32 rtk_pkt_txBuffer_get(uint8 *pPayload, uint16 max_length, uint16 *pPkt_length)
{
    if((NULL == pPayload) || (NULL == pPkt_length))
        return RT_ERR_NULL_POINTER;

    *pPkt_length = pkt_length;
    osal_memcpy(pPayload, &pkt_payload[0], (pkt_length <= max_length) ? pkt_length : max_length);
    return RT_ERR_OK;
}

int32 rtk_pkt_txBuffer_clear(void)
{
    pkt_length = 0;
    osal_memset(pkt_payload, 0, sizeof(unsigned char) * PKT_DEBUG_PKT_LENGTH_MAX);

    txInfo.keep               = 0;
    txInfo.blu                = 0;
    txInfo.vsel               = 0;
    txInfo.dislrn             = 0;
    txInfo.cputag_ipcs        = 0;
    txInfo.cputag_l4cs        = 0;
    txInfo.cputag_psel        = 0;
    txInfo.aspri              = 0;
    txInfo.cputag_pri         = 0;
    txInfo.tx_pppoe_action    = 0;
    txInfo.tx_pppoe_idx       = 0;
    txInfo.efid               = 0;
    txInfo.enhance_fid        = 0;
    txInfo.extspa             = 0;
    txInfo.tx_portmask        = 0;
    txInfo.tx_dst_stream_id   = 0;
    txInfo.sb                 = 0;
    txInfo.l34keep            = 0;

    return RT_ERR_OK;
}

int32 rtk_pkt_tx_send(uint32 count)
{
    rtdrv_pktdbg_t *pPkt_info;
    int32 ret;
    
    if(0 == pkt_length)
    {
        return RT_ERR_NOT_INIT;
    }

    pPkt_info = (rtdrv_pktdbg_t *)malloc(sizeof(rtdrv_pktdbg_t));
    if(NULL == pPkt_info)
    {
        return RT_ERR_NULL_POINTER;
    }
    osal_memset(pPkt_info,0x0,sizeof(rtdrv_pktdbg_t));

    pPkt_info->tx_count = count;
    pPkt_info->length = (pkt_length < PKT_DEBUG_PKT_MIN) ? PKT_DEBUG_PKT_MIN : pkt_length;
    pPkt_info->tx_info = txInfo;
    osal_memcpy(&pPkt_info->buf[0], &pkt_payload[0], (pPkt_info->length <= PKT_DEBUG_PKT_LENGTH_MAX) ? pPkt_info->length : PKT_DEBUG_PKT_LENGTH_MAX);
    SETSOCKOPT_WITH_RET(RTDRV_PKT_SEND, pPkt_info, rtdrv_pktdbg_t, 1, ret);
    if(0!=ret)
    {
        free(pPkt_info);
        return ret;    
    }
    
    free(pPkt_info);

    return RT_ERR_OK;
}

int32 rtk_pkt_continuouslyTx_send(uint32 enable)
{
    rtdrv_pktdbg_t *pPkt_info;
    int ret;

    if(0 == pkt_length)
    {
        return RT_ERR_NOT_INIT;
    }

    pPkt_info = (rtdrv_pktdbg_t *)malloc(sizeof(rtdrv_pktdbg_t));
    if(NULL == pPkt_info)
    {
        return RT_ERR_NULL_POINTER;
    }

    osal_memset(pPkt_info,0x0,sizeof(rtdrv_pktdbg_t));


    pPkt_info->tx_enable = enable;
    pPkt_info->length = (pkt_length < PKT_DEBUG_PKT_MIN) ? PKT_DEBUG_PKT_MIN : pkt_length;
    pPkt_info->tx_info = txInfo;
    osal_memcpy(&pPkt_info->buf[0], &pkt_payload[0], (pPkt_info->length <= PKT_DEBUG_PKT_LENGTH_MAX) ? pPkt_info->length : PKT_DEBUG_PKT_LENGTH_MAX);
    SETSOCKOPT_WITH_RET(RTDRV_PKT_CONTINUS_SEND, pPkt_info, rtdrv_pktdbg_t, 1, ret);

    if(0!=ret)
    {
        free(pPkt_info);
        return ret;
    }

    free(pPkt_info);
    return RT_ERR_OK;
}

int32 rtk_pkt_continuouslyTxCnt_set(uint32 count)
{
    if(0 == count)
    {
        return RT_ERR_INPUT;
    }

    SETSOCKOPT(RTDRV_PKT_CONTINUSCNT_SET, &count, uint32, 1);

    return RT_ERR_OK;
}

int32 rtk_pkt_continuouslyTxCnt_get(uint32 *pCount)
{
    uint32 count;

    if(NULL == pCount)
    {
        return RT_ERR_INPUT;
    }

    GETSOCKOPT(RTDRV_PKT_CONTINUSCNT_GET, &count, uint32, 1);
    *pCount = count;

    return RT_ERR_OK;
}

int32 rtk_pkt_txCpuTag_set(struct pkt_dbg_cputag cputag)
{
    txInfo.keep               = cputag.keep;
    txInfo.blu                = cputag.l2br;
    txInfo.vsel               = cputag.vsel;
    txInfo.dislrn             = cputag.dislrn;
    txInfo.cputag_ipcs        = cputag.l3cs;
    txInfo.cputag_l4cs        = cputag.l4cs;
    txInfo.cputag_psel        = cputag.psel;
    txInfo.aspri              = cputag.prisel;
    txInfo.cputag_pri         = cputag.priority;
    txInfo.tx_pppoe_action    = cputag.pppoeact;
    txInfo.tx_pppoe_idx       = cputag.pppoeidx;
    txInfo.efid               = cputag.efid;
    txInfo.enhance_fid        = cputag.efid_value;
    txInfo.extspa             = cputag.extspa;
    txInfo.tx_portmask        = cputag.txmask_vidx;
    txInfo.tx_dst_stream_id   = cputag.pon_streamid;
    txInfo.sb                 = cputag.sb;
    txInfo.l34keep            = cputag.l34keep;
#if defined(CONFIG_SDK_RTL9607C)
    txInfo.tx_gmac            = cputag.tx_gmac;
#endif
    return RT_ERR_OK;
}

int32 rtk_pkt_txCpuTag_get(struct pkt_dbg_cputag *pCputag)
{
    if(NULL == pCputag)
        return RT_ERR_NULL_POINTER;

    pCputag->l3cs         = txInfo.cputag_ipcs;
    pCputag->l4cs         = txInfo.cputag_l4cs;
    pCputag->txmask_vidx  = txInfo.tx_portmask;
    pCputag->efid         = txInfo.efid;
    pCputag->efid_value   = txInfo.enhance_fid;
    pCputag->prisel       = txInfo.aspri;
    pCputag->priority     = txInfo.cputag_pri;
    pCputag->keep         = txInfo.keep;
    pCputag->vsel         = txInfo.vsel;
    pCputag->dislrn       = txInfo.dislrn;
    pCputag->psel         = txInfo.cputag_psel;
    pCputag->sb           = txInfo.sb;
    pCputag->l34keep      = txInfo.l34keep;
    pCputag->extspa       = txInfo.extspa;
    pCputag->pppoeact     = txInfo.tx_pppoe_action;
    pCputag->pppoeidx     = txInfo.tx_pppoe_idx;
    pCputag->l2br         = txInfo.blu;
    pCputag->pon_streamid = txInfo.tx_dst_stream_id;
#if defined(CONFIG_SDK_RTL9607C)
    pCputag->tx_gmac      = txInfo.tx_gmac;
#endif
    return RT_ERR_OK;
}

int32 rtk_pkt_txCmd_set(struct pkt_dbg_tx_info inTxInfo)
{
    txInfo.ipcs               = inTxInfo.ipcs;
    txInfo.l4cs               = inTxInfo.l4cs;
    txInfo.vidl               = inTxInfo.vidl;
    txInfo.prio               = inTxInfo.prio;
    txInfo.cfi                = inTxInfo.cfi;
    txInfo.vidh               = inTxInfo.vidh;

    return RT_ERR_OK;
}

int32 rtk_pkt_txCmd_get(struct pkt_dbg_tx_info *pOutTxInfo)
{
    if(NULL == pOutTxInfo)
        return RT_ERR_NULL_POINTER;

    pOutTxInfo->ipcs          = txInfo.ipcs;
    pOutTxInfo->l4cs          = txInfo.l4cs;
    pOutTxInfo->vidl          = txInfo.vidl;
    pOutTxInfo->prio          = txInfo.prio;
    pOutTxInfo->cfi           = txInfo.cfi;
    pOutTxInfo->vidh          = txInfo.vidh;
    
    return RT_ERR_OK;
}

int32 rtk_pkt_txLsoParam_set(struct pkt_dbg_lso_param inLsoParam, struct pkt_dbg_lso_param inLsoParamValid)
{
    if(inLsoParamValid.lenfix_en) {
        txInfo.lso_param.lenfix_en = inLsoParam.lenfix_en;
        txInfo.lso_param_valid.lenfix_en = 1;
    }
    if(inLsoParamValid.bypass_en) {
        txInfo.lso_param.bypass_en = inLsoParam.bypass_en;
        txInfo.lso_param_valid.bypass_en = 1;
    }
    if(inLsoParamValid.udp_en) {
        txInfo.lso_param.udp_en = inLsoParam.udp_en;
        txInfo.lso_param_valid.udp_en = 1;
    }
    if(inLsoParamValid.tcp_en) {
        txInfo.lso_param.tcp_en = inLsoParam.tcp_en;
        txInfo.lso_param_valid.tcp_en = 1;
    }
    if(inLsoParamValid.ipv6_en) {
        txInfo.lso_param.ipv6_en = inLsoParam.ipv6_en;
        txInfo.lso_param_valid.ipv6_en = 1;
    }
    if(inLsoParamValid.ipv4_en) {
        txInfo.lso_param.ipv4_en = inLsoParam.ipv4_en;
        txInfo.lso_param_valid.ipv4_en = 1;
    }
    if(inLsoParamValid.segment_en) {
        txInfo.lso_param.segment_en = inLsoParam.segment_en;
        txInfo.lso_param_valid.segment_en = 1;
    }
    if(inLsoParamValid.unknow_tag_en) {
        txInfo.lso_param.unknow_tag_en = inLsoParam.unknow_tag_en;
        txInfo.lso_param_valid.unknow_tag_en = 1;
    }
    if(inLsoParamValid.segment_size) {
        txInfo.lso_param.segment_size = inLsoParam.segment_size;
        txInfo.lso_param_valid.segment_size = 1;
    }
    if(inLsoParamValid.packetSize) {
        txInfo.lso_param.packetSize = inLsoParam.packetSize;
        txInfo.lso_param_valid.packetSize = 1;
    }

    return RT_ERR_OK;
}

int32 rtk_pkt_txLsoParam_get(struct pkt_dbg_lso_param *pOutLsoParam, struct pkt_dbg_lso_param *pOutLsoParamValid)
{
    if((NULL == pOutLsoParam) || (NULL == pOutLsoParamValid))
    {
        return RT_ERR_NULL_POINTER;
    }

    memset(pOutLsoParam, 0, sizeof(struct pkt_dbg_lso_param));
    memset(pOutLsoParamValid, 0, sizeof(struct pkt_dbg_lso_param));

    if(txInfo.lso_param_valid.lenfix_en)
    {
        pOutLsoParam->lenfix_en = txInfo.lso_param_valid.lenfix_en;
        pOutLsoParamValid->lenfix_en = 1;
    }
    if(txInfo.lso_param_valid.bypass_en)
    {
        pOutLsoParam->bypass_en = txInfo.lso_param_valid.bypass_en;
        pOutLsoParamValid->bypass_en = 1;
    }
    if(txInfo.lso_param_valid.udp_en)
    {
        pOutLsoParam->udp_en = txInfo.lso_param_valid.udp_en;
        pOutLsoParamValid->udp_en = 1;
    }
    if(txInfo.lso_param_valid.tcp_en)
    {
        pOutLsoParam->tcp_en = txInfo.lso_param_valid.tcp_en;
        pOutLsoParamValid->tcp_en = 1;
    }
    if(txInfo.lso_param_valid.ipv6_en)
    {
        pOutLsoParam->ipv6_en = txInfo.lso_param_valid.ipv6_en;
        pOutLsoParamValid->ipv6_en = 1;
    }
    if(txInfo.lso_param_valid.ipv4_en)
    {
        pOutLsoParam->ipv4_en = txInfo.lso_param_valid.ipv4_en;
        pOutLsoParamValid->ipv4_en = 1;
    }
    if(txInfo.lso_param_valid.segment_en)
    {
        pOutLsoParam->segment_en = txInfo.lso_param_valid.segment_en;
        pOutLsoParamValid->segment_en = 1;
    }
    if(txInfo.lso_param_valid.unknow_tag_en)
    {
        pOutLsoParam->unknow_tag_en = txInfo.lso_param_valid.unknow_tag_en;
        pOutLsoParamValid->unknow_tag_en = 1;
    }
    if(txInfo.lso_param_valid.segment_size)
    {
        pOutLsoParam->segment_size = txInfo.lso_param_valid.segment_size;
        pOutLsoParamValid->segment_size = 1;
    }
    if(txInfo.lso_param_valid.packetSize)
    {
        pOutLsoParam->packetSize = txInfo.lso_param_valid.packetSize;
        pOutLsoParamValid->packetSize = 1;
    }

    return RT_ERR_OK;
}

int32 rtk_pkt_txLsoParam_clear(void)
{
    txInfo.lso_param_valid.lenfix_en = 0;
    txInfo.lso_param_valid.bypass_en = 0;
    txInfo.lso_param_valid.udp_en = 0;
    txInfo.lso_param_valid.tcp_en = 0;
    txInfo.lso_param_valid.ipv6_en = 0;
    txInfo.lso_param_valid.ipv4_en = 0;
    txInfo.lso_param_valid.segment_en = 0;
    txInfo.lso_param_valid.unknow_tag_en = 0;
    txInfo.lso_param_valid.segment_size = 0;
    txInfo.lso_param_valid.packetSize = 0;

    return RT_ERR_OK;
}

int32 rtk_pkt_txHeaderA_set(struct pkt_dbg_header_a inHeaderA, struct pkt_dbg_header_a inHeaderAValid)
{
    if(inHeaderAValid.cmd_asel)
    {
        txInfo.header_a.cmd_asel = inHeaderA.cmd_asel;
        txInfo.header_a_valid.cmd_asel = 1;
    }
    if(inHeaderAValid.cmd_op)
    {
        txInfo.header_a.cmd_op = inHeaderA.cmd_op;
        txInfo.header_a_valid.cmd_op = 1;
    }
    if(inHeaderAValid.cmd_mode)
    {
        txInfo.header_a.cmd_mode = inHeaderA.cmd_mode;
        txInfo.header_a_valid.cmd_mode = 1;
    }
    if(inHeaderAValid.ptp_type)
    {
        txInfo.header_a.ptp_type = inHeaderA.ptp_type;
        txInfo.header_a_valid.ptp_type = 1;
    }
    if(inHeaderAValid.cmd_offset)
    {
        txInfo.header_a.cmd_offset = inHeaderA.cmd_offset;
        txInfo.header_a_valid.cmd_offset = 1;
    }
    if(inHeaderAValid.udp_cksum_offset)
    {
        txInfo.header_a.udp_cksum_offset = inHeaderA.udp_cksum_offset;
        txInfo.header_a_valid.udp_cksum_offset = 1;
    }
    if(inHeaderAValid.cpu_flg)
    {
        txInfo.header_a.cpu_flg = inHeaderA.cpu_flg;
        txInfo.header_a_valid.cpu_flg = 1;
    }
    if(inHeaderAValid.deep_q)
    {
        txInfo.header_a.deep_q = inHeaderA.deep_q;
        txInfo.header_a_valid.deep_q = 1;
    }
    if(inHeaderAValid.pol_grp_id)
    {
        txInfo.header_a.pol_grp_id = inHeaderA.pol_grp_id;
        txInfo.header_a_valid.pol_grp_id = 1;
    }
    if(inHeaderAValid.pol_id)
    {
        txInfo.header_a.pol_id = inHeaderA.pol_id;
        txInfo.header_a_valid.pol_id = 1;
    }
    if(inHeaderAValid.pol_en)
    {
        txInfo.header_a.pol_en = inHeaderA.pol_en;
        txInfo.header_a_valid.pol_en = 1;
    }
    if(inHeaderAValid.mark)
    {
        txInfo.header_a.mark = inHeaderA.mark;
        txInfo.header_a_valid.mark = 1;
    }
    if(inHeaderAValid.mirror)
    {
        txInfo.header_a.mirror = inHeaderA.mirror;
        txInfo.header_a_valid.mirror = 1;
    }
    if(inHeaderAValid.no_drop)
    {
        txInfo.header_a.no_drop = inHeaderA.no_drop;
        txInfo.header_a_valid.no_drop = 1;
    }
    if(inHeaderAValid.rx_pkt_type)
    {
        txInfo.header_a.rx_pkt_type = inHeaderA.rx_pkt_type;
        txInfo.header_a_valid.rx_pkt_type = 1;
    }
    if(inHeaderAValid.drop_code)
    {
        txInfo.header_a.drop_code = inHeaderA.drop_code;
        txInfo.header_a_valid.drop_code = 1;
    }
    if(inHeaderAValid.mcgid)
    {
        txInfo.header_a.mcgid = inHeaderA.mcgid;
        txInfo.header_a_valid.mcgid = 1;
    }
    if(inHeaderAValid.hdr_type)
    {
        txInfo.header_a.hdr_type = inHeaderA.hdr_type;
        txInfo.header_a_valid.hdr_type = 1;
    }
    if(inHeaderAValid.fe_bypass)
    {
        txInfo.header_a.fe_bypass = inHeaderA.fe_bypass;
        txInfo.header_a_valid.fe_bypass = 1;
    }
    if(inHeaderAValid.pkt_size)
    {
        txInfo.header_a.pkt_size = inHeaderA.pkt_size;
        txInfo.header_a_valid.pkt_size = 1;
    }
    if(inHeaderAValid.lspid)
    {
        txInfo.header_a.lspid = inHeaderA.lspid;
        txInfo.header_a_valid.lspid = 1;
    }
    if(inHeaderAValid.ldpid)
    {
        txInfo.header_a.ldpid = inHeaderA.ldpid;
        txInfo.header_a_valid.ldpid = 1;
    }
    if(inHeaderAValid.cos)
    {
        txInfo.header_a.cos = inHeaderA.cos;
        txInfo.header_a_valid.cos = 1;
    }

    return RT_ERR_OK;
}

int32 rtk_pkt_txHeaderA_get(struct pkt_dbg_header_a *pOutHeaderA, struct pkt_dbg_header_a *pOutHeaderAValid)
{
    if((NULL == pOutHeaderA) || (NULL == pOutHeaderAValid))
    {
        return RT_ERR_NULL_POINTER;
    }

    memset(pOutHeaderA, 0, sizeof(struct pkt_dbg_header_a));
    memset(pOutHeaderAValid, 0, sizeof(struct pkt_dbg_header_a));

    if(txInfo.header_a_valid.cmd_asel)
    {
        pOutHeaderA->cmd_asel = txInfo.header_a.cmd_asel;
        pOutHeaderAValid->cmd_asel = 1;
    }
    if(txInfo.header_a_valid.cmd_op)
    {
        pOutHeaderA->cmd_op = txInfo.header_a.cmd_op;
        pOutHeaderAValid->cmd_op = 1;
    }
    if(txInfo.header_a_valid.cmd_mode)
    {
        pOutHeaderA->cmd_mode = txInfo.header_a.cmd_mode;
        pOutHeaderAValid->cmd_mode = 1;
    }
    if(txInfo.header_a_valid.ptp_type)
    {
        pOutHeaderA->ptp_type = txInfo.header_a.ptp_type;
        pOutHeaderAValid->ptp_type = 1;
    }
    if(txInfo.header_a_valid.cmd_offset)
    {
        pOutHeaderA->cmd_offset = txInfo.header_a.cmd_offset;
        pOutHeaderAValid->cmd_offset = 1;
    }
    if(txInfo.header_a_valid.udp_cksum_offset)
    {
        pOutHeaderA->udp_cksum_offset = txInfo.header_a.udp_cksum_offset;
        pOutHeaderAValid->udp_cksum_offset = 1;
    }
    if(txInfo.header_a_valid.cpu_flg)
    {
        pOutHeaderA->cpu_flg = txInfo.header_a.cpu_flg;
        pOutHeaderAValid->cpu_flg = 1;
    }
    if(txInfo.header_a_valid.deep_q)
    {
        pOutHeaderA->deep_q = txInfo.header_a.deep_q;
        pOutHeaderAValid->deep_q = 1;
    }
    if(txInfo.header_a_valid.pol_grp_id)
    {
        pOutHeaderA->pol_grp_id = txInfo.header_a.pol_grp_id;
        pOutHeaderAValid->pol_grp_id = 1;
    }
    if(txInfo.header_a_valid.pol_id)
    {
        pOutHeaderA->pol_id = txInfo.header_a.pol_id;
        pOutHeaderAValid->pol_id = 1;
    }
    if(txInfo.header_a_valid.pol_en)
    {
        pOutHeaderA->pol_en = txInfo.header_a.pol_en;
        pOutHeaderAValid->pol_en = 1;
    }
    if(txInfo.header_a_valid.mark)
    {
        pOutHeaderA->mark = txInfo.header_a.mark;
        pOutHeaderAValid->mark = 1;
    }
    if(txInfo.header_a_valid.mirror)
    {
        pOutHeaderA->mirror = txInfo.header_a.mirror;
        pOutHeaderAValid->mirror = 1;
    }
    if(txInfo.header_a_valid.no_drop)
    {
        pOutHeaderA->no_drop = txInfo.header_a.no_drop;
        pOutHeaderAValid->no_drop = 1;
    }
    if(txInfo.header_a_valid.rx_pkt_type)
    {
        pOutHeaderA->rx_pkt_type = txInfo.header_a.rx_pkt_type;
        pOutHeaderAValid->rx_pkt_type = 1;
    }
    if(txInfo.header_a_valid.drop_code)
    {
        pOutHeaderA->drop_code = txInfo.header_a.drop_code;
        pOutHeaderAValid->drop_code = 1;
    }
    if(txInfo.header_a_valid.mcgid)
    {
        pOutHeaderA->mcgid = txInfo.header_a.mcgid;
        pOutHeaderAValid->mcgid = 1;
    }
    if(txInfo.header_a_valid.hdr_type)
    {
        pOutHeaderA->hdr_type = txInfo.header_a.hdr_type;
        pOutHeaderAValid->hdr_type = 1;
    }
    if(txInfo.header_a_valid.fe_bypass)
    {
        pOutHeaderA->fe_bypass = txInfo.header_a.fe_bypass;
        pOutHeaderAValid->fe_bypass = 1;
    }
    if(txInfo.header_a_valid.pkt_size)
    {
        pOutHeaderA->pkt_size = txInfo.header_a.pkt_size;
        pOutHeaderAValid->pkt_size = 1;
    }
    if(txInfo.header_a_valid.lspid)
    {
        pOutHeaderA->lspid = txInfo.header_a.lspid;
        pOutHeaderAValid->lspid = 1;
    }
    if(txInfo.header_a_valid.ldpid)
    {
        pOutHeaderA->ldpid = txInfo.header_a.ldpid;
        pOutHeaderAValid->ldpid = 1;
    }
    if(txInfo.header_a_valid.cos)
    {
        pOutHeaderA->cos = txInfo.header_a.cos;
        pOutHeaderAValid->cos = 1;
    }

    return RT_ERR_OK;
}

int32 rtk_pkt_txHeaderA_clear(void)
{
    txInfo.header_a_valid.cmd_asel = 0;
    txInfo.header_a_valid.cmd_op = 0;
    txInfo.header_a_valid.cmd_mode = 0;
    txInfo.header_a_valid.ptp_type = 0;
    txInfo.header_a_valid.cmd_offset = 0;
    txInfo.header_a_valid.udp_cksum_offset = 0;
    txInfo.header_a_valid.cpu_flg = 0;
    txInfo.header_a_valid.deep_q = 0;
    txInfo.header_a_valid.pol_grp_id = 0;
    txInfo.header_a_valid.pol_id = 0;
    txInfo.header_a_valid.pol_en = 0;
    txInfo.header_a_valid.mark = 0;
    txInfo.header_a_valid.mirror = 0;
    txInfo.header_a_valid.no_drop = 0;
    txInfo.header_a_valid.rx_pkt_type = 0;
    txInfo.header_a_valid.drop_code = 0;
    txInfo.header_a_valid.mcgid = 0;
    txInfo.header_a_valid.hdr_type = 0;
    txInfo.header_a_valid.fe_bypass = 0;
    txInfo.header_a_valid.pkt_size = 0;
    txInfo.header_a_valid.lspid = 0;
    txInfo.header_a_valid.ldpid = 0;
    txInfo.header_a_valid.cos = 0;

    return RT_ERR_OK;
}

int32 rtk_pkt_txHeaderCpu_set(struct pkt_dbg_header_cpu inHeaderCpu, struct pkt_dbg_header_cpu inHeaderCpuValid)
{
    if(inHeaderCpuValid.crc32)
    {
        txInfo.header_cpu.crc32 = inHeaderCpu.crc32;
        txInfo.header_cpu_valid.crc32 = 1;
    }
    if(inHeaderCpuValid.crc16)
    {
        txInfo.header_cpu.crc16 = inHeaderCpu.crc16;
        txInfo.header_cpu_valid.crc16 = 1;
    }
    if(inHeaderCpuValid.trap_rsn)
    {
        txInfo.header_cpu.trap_rsn = inHeaderCpu.trap_rsn;
        txInfo.header_cpu_valid.trap_rsn = 1;
    }
    if(inHeaderCpuValid.sw_idx)
    {
        txInfo.header_cpu.sw_idx = inHeaderCpu.sw_idx;
        txInfo.header_cpu_valid.sw_idx = 1;
    }

    return RT_ERR_OK;
}

int32 rtk_pkt_txHeaderCpu_get(struct pkt_dbg_header_cpu *pOutHeaderCpu, struct pkt_dbg_header_cpu *pOutHeaderCpuValid)
{
    if((NULL == pOutHeaderCpu) || (NULL == pOutHeaderCpuValid))
    {
        return RT_ERR_NULL_POINTER;
    }

    memset(pOutHeaderCpu, 0, sizeof(struct pkt_dbg_header_cpu));
    memset(pOutHeaderCpuValid, 0, sizeof(struct pkt_dbg_header_cpu));

    if(txInfo.header_cpu_valid.crc32)
    {
        pOutHeaderCpu->crc32 = txInfo.header_cpu.crc32;
        pOutHeaderCpuValid->crc32 = 1;
    }
    if(txInfo.header_cpu_valid.crc16)
    {
        pOutHeaderCpu->crc16 = txInfo.header_cpu.crc16;
        pOutHeaderCpuValid->crc16 = 1;
    }
    if(txInfo.header_cpu_valid.trap_rsn)
    {
        pOutHeaderCpu->trap_rsn = txInfo.header_cpu.trap_rsn;
        pOutHeaderCpuValid->trap_rsn = 1;
    }
    if(txInfo.header_cpu_valid.sw_idx)
    {
        pOutHeaderCpu->sw_idx = txInfo.header_cpu.sw_idx;
        pOutHeaderCpuValid->sw_idx = 1;
    }

    return RT_ERR_OK;
}

