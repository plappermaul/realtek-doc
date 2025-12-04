/*
 * Copyright (C) 2011 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 *
 *
 * $Revision$
 * $Date$
 *
 * Purpose : GMac Driver OMCI Processor
 *
 * Feature : GMac Driver OMCI Processor
 *
 */

#if defined(CONFIG_SDK_KERNEL_LINUX)
#include <linux/skbuff.h>
#include <linux/interrupt.h>
#if defined(CONFIG_RTL9600_SERIES)
#include "re8686.h"
#elif defined(CONFIG_RTL9601B_SERIES)
#include "re8686_rtl9601b.h"
#elif defined(CONFIG_RTL9602C_SERIES)
#include "re8686_rtl9602c.h"
#else
#include "re8686_nic.h"
#endif
#endif

#include <module/gpon/gpon_defs.h>
#include <module/gpon/gpon_omci.h>
#include <module/gpon/gpon_debug.h>
#include <module/gpon/gpon.h>
#include <hal/common/halctrl.h>

typedef struct gpon_omci_extend_header_s{
    uint16 tc_id;
    uint8 msg_id;
    uint8 dev_id;
    uint32 me_id;
    uint16 msg_len;
    uint16 dummy;
}gpon_omci_extend_header_t;


uint8 omciHeader[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x02,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
    0x88, 0xb5 };

extern int re8686_tx_with_Info ( unsigned char* pPayload, unsigned short length, void* pInfo );

static int32 gpon_omci_mirror (
    rtk_gpon_omci_msg_t* mirroredOMCIMsg
)
{
    int32 ret = 0;
#if defined(CONFIG_SDK_KERNEL_LINUX)
    gpon_omci_extend_header_t *extend_head;
    struct tx_info ptxInfo;
    char* mirroredEthPkt = NULL;

    if (0xFFFFFFFF == gpon_omciMirroringPort)
    {
        return -1;
    }
    // check mirroring port is valid or not
    if (!HAL_IS_PORT_EXIST(gpon_omciMirroringPort))
    {
        return -1;
    }
#endif

    if (!mirroredOMCIMsg)
    {
        return -1;
    }

#if defined(CONFIG_SDK_KERNEL_LINUX)
    mirroredEthPkt =
        (char*)kmalloc(
                sizeof(uint8) * (mirroredOMCIMsg->len + sizeof(omciHeader)),
                GFP_ATOMIC);
    if (!mirroredEthPkt)
    {
        return -1;
    }

    memset(
        mirroredEthPkt,
        0x00,
        sizeof(uint8) * (mirroredOMCIMsg->len + sizeof(omciHeader)));
    memcpy(
        mirroredEthPkt,
        omciHeader,
        sizeof(uint8) * sizeof(omciHeader));
    memcpy(
        mirroredEthPkt + sizeof(omciHeader),
        mirroredOMCIMsg->msg,
        sizeof(uint8) * mirroredOMCIMsg->len);

    memset(&ptxInfo, 0, sizeof(struct tx_info));
    GMAC_TXINFO_DISLRN(&ptxInfo) = 1;
    GMAC_TXINFO_KEEP(&ptxInfo) = 1;
    GMAC_TXINFO_CPUTAG(&ptxInfo) = 1;
    GMAC_TXINFO_TX_PMASK(&ptxInfo) = (1 << gpon_omciMirroringPort);

    if(mirroredOMCIMsg->msg[3] == 0x0A)
    {
        ret = re8686_tx_with_Info(mirroredEthPkt, RTK_GPON_OMCI_BASELINE_MSG_LEN_TX+14, &ptxInfo);
    }
    else
    {
        extend_head = (gpon_omci_extend_header_t *)mirroredOMCIMsg->msg;
        if(extend_head->msg_len <= RTK_GPON_OMCI_MSG_MAX_CONTENT_LEN)
        {
            ret = re8686_tx_with_Info(mirroredEthPkt, extend_head->msg_len+RTK_GPON_OMCI_MSG_HEADER_LEN+14, &ptxInfo);
        }
        else
        {
            ret = re8686_tx_with_Info(mirroredEthPkt, RTK_GPON_OMCI_MSG_LEN_TX+14, &ptxInfo);
            osal_printf("Mirror OMCI message length overflow, length:%d\n", extend_head->msg_len);
        }
    }
    kfree(mirroredEthPkt);
    mirroredEthPkt = NULL;
#endif
    return ret;
}


int32 gpon_omci_tx ( gpon_dev_obj_t* obj, rtk_gpon_omci_msg_t* omci )
{
    int32 ret = 0;
    rtk_port_t ponPort;
#if defined(CONFIG_SDK_KERNEL_LINUX)
    gpon_omci_extend_header_t *extend_head;
#endif

    rtk_gpon_port_get(&ponPort);

    if (obj->us_omci_flow != obj->scheInfo.max_flow)
    {
        GPON_OS_Log(GPON_LOG_LEVEL_OMCI, "Send OMCI [port %d]: %02x%02x %02x %02x %02x%02x%02x%02x [baseaddr:%p]",
            obj->us_omci_flow, omci->msg[0], omci->msg[1], omci->msg[2], omci->msg[3], omci->msg[4], omci->msg[5], omci->msg[6], omci->msg[7], obj->base_addr);
        GPON_OS_Log(GPON_LOG_LEVEL_OMCI, "%02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x",
            omci->msg[8], omci->msg[9], omci->msg[10], omci->msg[11], omci->msg[12], omci->msg[13], omci->msg[14], omci->msg[15],
            omci->msg[16], omci->msg[17], omci->msg[18], omci->msg[19], omci->msg[20], omci->msg[21], omci->msg[22], omci->msg[23]);
        GPON_OS_Log(GPON_LOG_LEVEL_OMCI, "%02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x",
            omci->msg[24], omci->msg[25], omci->msg[26], omci->msg[27], omci->msg[28], omci->msg[29], omci->msg[30], omci->msg[31],
            omci->msg[32], omci->msg[33], omci->msg[34], omci->msg[35], omci->msg[36], omci->msg[37], omci->msg[38], omci->msg[39]);
        GPON_OS_Log(GPON_LOG_LEVEL_OMCI, "%02x%02x %02x%02x %02x%02x%02x%02x ",
            omci->msg[40], omci->msg[41], omci->msg[42], omci->msg[43], omci->msg[44], omci->msg[45], omci->msg[46], omci->msg[47]);

#if defined(CONFIG_SDK_KERNEL_LINUX)
        {
            struct tx_info ptxInfo;

            osal_memset(&ptxInfo, 0, sizeof(struct tx_info));

            GMAC_TXINFO_CPUTAG_PSEL(&ptxInfo) = 1;
            GMAC_TXINFO_DISLRN(&ptxInfo) = 1;
            GMAC_TXINFO_KEEP(&ptxInfo) = 1;
            GMAC_TXINFO_CPUTAG(&ptxInfo) = 1;
#if defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
#else
            GMAC_TXINFO_EFID(&ptxInfo) = 1;
            GMAC_TXINFO_ENHANCE_FID(&ptxInfo) = 0;
#endif
            GMAC_TXINFO_TX_PMASK(&ptxInfo) = (1 << ponPort);
            GMAC_TXINFO_TX_DST_STREAM_ID(&ptxInfo) = obj->scheInfo.omcc_flow;

            GMAC_TXINFO_ASPRI(&ptxInfo)=1;
            GMAC_TXINFO_CPUTAG_PRI(&ptxInfo)=7;

            if(omci->msg[3] == 0x0A)
            {
                ret = re8686_tx_with_Info(omci->msg, RTK_GPON_OMCI_BASELINE_MSG_LEN_TX, &ptxInfo);
            }
            else
            {
                extend_head = (gpon_omci_extend_header_t *)omci->msg;
                if(extend_head->msg_len <= RTK_GPON_OMCI_MSG_MAX_CONTENT_LEN)
                {
                	ret = re8686_tx_with_Info(omci->msg, extend_head->msg_len+RTK_GPON_OMCI_MSG_HEADER_LEN, &ptxInfo);
                }
                else
                {
                    ret = re8686_tx_with_Info(omci->msg, RTK_GPON_OMCI_MSG_LEN_TX, &ptxInfo);
                    osal_printf("OMCI message length overflow, length:%d\n", extend_head->msg_len);
                }
            }
        }
#endif
        if (0 == ret)
        {
            obj->cnt_cpu_omci_tx++;
            obj->cnt_cpu_omci_tx_nor++;
            gpon_omci_mirror(omci);
        }
        else
        {
        }
        return ret;
    }
    else
    {
        osal_printf("flow_id=%d\n", obj->us_omci_flow);
        return RT_ERR_INPUT;
    }
}

int32 gpon_omci_rx ( gpon_dev_obj_t* obj, rtk_gpon_omci_msg_t* omci )
{
    if (obj->status != GPON_STATE_O5 && obj->status != GPON_STATE_O6)
    {
        GPON_OS_Log(GPON_LOG_LEVEL_WARNING, "Rcv OMCI in state %s", gpon_dbg_fsm_status_str(obj->status));
        return RT_ERR_OUT_OF_RANGE;
    }
    else
    {
        obj->cnt_cpu_omci_rx++;
        GPON_OS_Log(GPON_LOG_LEVEL_OMCI, "Rcv OMCI: %02x%02x %02x %02x %02x%02x%02x%02x ",
            omci->msg[0], omci->msg[1], omci->msg[2], omci->msg[3], omci->msg[4], omci->msg[5], omci->msg[6], omci->msg[7]);
        GPON_OS_Log(GPON_LOG_LEVEL_OMCI, "%02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x",
            omci->msg[8], omci->msg[9], omci->msg[10], omci->msg[11], omci->msg[12], omci->msg[13], omci->msg[14], omci->msg[15],
            omci->msg[16], omci->msg[17], omci->msg[18], omci->msg[19], omci->msg[20], omci->msg[21], omci->msg[22], omci->msg[23]);
        GPON_OS_Log(GPON_LOG_LEVEL_OMCI, "%02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x",
            omci->msg[24], omci->msg[25], omci->msg[26], omci->msg[27], omci->msg[28], omci->msg[29], omci->msg[30], omci->msg[31],
            omci->msg[32], omci->msg[33], omci->msg[34], omci->msg[35], omci->msg[36], omci->msg[37], omci->msg[38], omci->msg[39]);
        GPON_OS_Log(GPON_LOG_LEVEL_OMCI, "%02x%02x%02x%02x",
            omci->msg[40], omci->msg[41], omci->msg[42], omci->msg[43]);

        if (obj->omci_callback)
        {
            (*obj->omci_callback)(omci);
        }
        gpon_omci_mirror(omci);
    }
    return RT_ERR_OK;
}

