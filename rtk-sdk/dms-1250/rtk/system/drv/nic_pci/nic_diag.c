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
 * $Revision: 97105 $
 * $Date: 2019-05-22 18:58:45 +0800 (三, 22  5月 2019) $
 *
 * Purpose : Definition those public NIC(Network Interface Controller) APIs and
 *           its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) CPU tag
 *           2) NIC tx
 *           3) NIC rx
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <drv/nic/nic.h>
#include <private/drv/nic/nic_diag.h>
#include <common/debug/rt_log.h>
#include <common/rt_error.h>
#include <dal/dal_mgmt.h>
#include <osal/lib.h>

/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */
nic_txTagStatus_t txTagStatus;
uint8 txTag[32];
rtk_portmask_t txPortmask;
uint32  isDataAuto;
uint8   txData[10000];
uint32  packetLen;
uint32  usr_lb_sts[RTK_MAX_NUM_OF_UNIT];

/*
 * Function Declaration
 */
#ifndef __BOOTLOADER__
void
_dummy_tx_callback(uint32 unit, drv_nic_pkt_t *pPacket, void *pCookie)
{
    return;
}

/*
 * Function Declaration
 *      drv_nic_tag_set
 * Description:
 *      Set TX tag of CPU TX packet.
 * Input:
 *      unit    - unit id
 *      tagStatus - CPU TX tag status
 *      pTxTag    - pointer to CPU TX tag, it is valid only if tagStatus is manual
 *      pPortmask    - pointer to TX portmask, it is valid only if tagStatus is auto
 * Output:
 *      None
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
 *      [SDK_3.1.1]
 *          New added function.
 */
int32
drv_nic_tag_set(uint32 unit, nic_txTagStatus_t tagStatus, uint8 *pTxTag,  rtk_portmask_t *pPortmask)
{
    if (NULL == pTxTag)
        return RT_ERR_NULL_POINTER;
    if (NULL == pPortmask)
        return RT_ERR_NULL_POINTER;

    txTagStatus = tagStatus;
    osal_memcpy(txTag, pTxTag, sizeof(txTag));
    osal_memcpy(&txPortmask, pPortmask, sizeof(txPortmask));

    return RT_ERR_OK;
}   /* end of drv_nic_tag_set */

/* Function Name:
 *      drv_nic_txData_set
 * Description:
 *      Set TX data of CPU TX packet TX.
 * Input:
 *      unit    - unit id
 *      isAuto - if packet data is auto generated
 *      pTxData    - pointer of packet TX data, it would be ignored if isAuto is true
 *      len   - packet size
 * Output:
 *      None
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
 *      [SDK_3.1.1]
 *          New added function.
 */
int32
drv_nic_txData_set(uint32 unit, uint8 isAuto, uint8 *pTxData, uint32 len)
{
    uint32  bufSize;

    if (NULL == pTxData)
        return RT_ERR_NULL_POINTER;

    bufSize = (len <= sizeof(txData)) ? len : sizeof(txData);
    isDataAuto = isAuto;
    osal_memcpy(txData, pTxData, bufSize);
    packetLen = bufSize;

    return RT_ERR_OK;
}   /* end of drv_nic_txData_set */

/* Function Name:
 *      drv_nic_diagPkt_send
 * Description:
 *      Trigger to send packets from CPU.
 * Input:
 *      unit    - unit id
 *      num   - number of TX frame
 * Output:
 *      None
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
 *      [SDK_3.1.1]
 *          New added function.
 */
int32
drv_nic_diagPkt_send(uint32 unit, uint32 num)
{
#if defined(CONFIG_SDK_RTL9310)
    int32   ret, cnt = 0, i, j;
    drv_nic_pkt_t *pPacket;


    if (packetLen == 0)
        packetLen = 64;

    while (1)
    {
        if (RT_ERR_OK != drv_nic_pkt_alloc(unit, packetLen, 0, &pPacket))
        {
            osal_printf("[%s]: Alloc packet failed.\n", __FUNCTION__);
            return RT_ERR_FAILED;
        }

        pPacket->length         = packetLen;
        pPacket->tail           = pPacket->data + packetLen;
        pPacket->txIncludeCRC   = TRUE;

        if (NIC_TXTAG_NONE == txTagStatus)
            pPacket->as_txtag       = FALSE;
        else
            pPacket->as_txtag       = TRUE;

        /* Setting CPU TX tag or payload */
        if (NIC_TXTAG_MANUAL == txTagStatus)
        {
            if ((txTag[6] & 0xf) == 1)
                pPacket->tx_tag.fwd_type = NIC_FWD_TYPE_PHYISCAL;
            else if ((txTag[6] & 0xf) == 2)
                pPacket->tx_tag.fwd_type = NIC_FWD_TYPE_LOGICAL;
            else if ((txTag[6] & 0xf) == 4)
                pPacket->tx_tag.fwd_type = NIC_FWD_TYPE_PHYISCAL_ONE_HOP;
            else if ((txTag[6] & 0xf) == 5)
                pPacket->tx_tag.fwd_type = NIC_FWD_TYPE_LOGICAL_ONE_HOP;
            else
                pPacket->tx_tag.fwd_type = txTag[6] & 0xf;

            pPacket->tx_tag.acl_act     = txTag[7] >> 7;
            pPacket->tx_tag.cngst_drop  = (txTag[7] >> 6) & 0x1;
            pPacket->tx_tag.dm_pkt      = (txTag[7] >> 5) & 0x1;
            pPacket->tx_tag.dg_pkt      = (txTag[7] >> 4) & 0x1;
            pPacket->tx_tag.bp_fltr     = (txTag[7] >> 3) & 0x1;
            pPacket->tx_tag.bp_stp      = (txTag[7] >> 2) & 0x1;
            pPacket->tx_tag.bp_vlan_egr = (txTag[7] >> 1) & 0x1;
            pPacket->tx_tag.as_tagSts   = (txTag[7]) & 0x1;
            pPacket->tx_tag.l3_act      = txTag[8] >> 7;
            pPacket->tx_tag.ori_tagif_en= (txTag[8] >> 6) & 0x1;
            pPacket->tx_tag.as_priority = (txTag[8] >> 5) & 0x1;
            pPacket->tx_tag.priority    = (txTag[8]) & 0x1f;
            pPacket->tx_tag.ori_itagif  = (txTag[9] >> 7) & 0x1;
            pPacket->tx_tag.ori_otagif  = (txTag[9] >> 6) & 0x1;
            pPacket->tx_tag.fvid_sel    = (txTag[9] >> 5) & 0x1;
            pPacket->tx_tag.fvid_en     = (txTag[9] >> 4);
            pPacket->tx_tag.fvid        = ((txTag[9] & 0xf) << 8) | (txTag[10]);
            pPacket->tx_tag.src_filter_en = (txTag[11] >> 7);
            pPacket->tx_tag.sp_is_trk   = (txTag[11] >> 6) & 0x1;
            pPacket->tx_tag.spn         = ((txTag[11] & 0x3f) << 4) | (txTag[12] >> 4);
            pPacket->tx_tag.dev_id      = txTag[12] & 0xf;
            pPacket->tx_tag.dst_port_mask_1 = ((uint32)txTag[13] << 16) | ((uint32)txTag[14] << 8) | txTag[15];
            pPacket->tx_tag.dst_port_mask = htonl(*(uint32*)(txTag + 16));
        }
        else if (NIC_TXTAG_AUTO == txTagStatus)
        {
            pPacket->tx_tag.fwd_type        = NIC_FWD_TYPE_LOGICAL;
            pPacket->tx_tag.dst_port_mask   = txPortmask.bits[0];
            pPacket->tx_tag.dst_port_mask_1 = txPortmask.bits[1];
        }

        if(isDataAuto)
        {
            /* Setting DA/SA */
            for (i = 0; i < 12; i++)
                pPacket->data[i] = 0;
            pPacket->data[5] = 0x01;
            pPacket->data[7] = 0xE0;
            pPacket->data[8] = 0x4C;

            for (i = 12, j = 0; i < packetLen; i++, j++)
                pPacket->data[i] = j & 0xff;
        }
        else
        {
            osal_memcpy( pPacket->data, txData, packetLen);
            pPacket->data[3] = cnt >> 16;
            pPacket->data[4] = (cnt >> 8) & 0xff;
            pPacket->data[5] = cnt & 0xff;
        }


        if ((ret = drv_nic_pkt_tx(unit, pPacket, NULL, NULL)))
        {
            osal_printf("%s():%d  ret:%#x\n", __FUNCTION__, __LINE__, ret);
            drv_nic_pkt_free(unit, pPacket);
            break;
        }


        if (num != 0)
        {
            cnt++;
            if (cnt == num)
                break;
        }
    }

#endif

    return RT_ERR_OK;
}   /* end of drv_nic_diagPkt_send */

/* Function Name:
 *      drv_nic_loopbackMode_get
 * Description:
 *      Get the loopback mode status of user's callback example function in SDK user mode.
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_UNIT_ID      - invalid unit id
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 */
int32
drv_nic_loopbackMode_get(uint32 unit, rtk_enable_t *pEnable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID), RT_ERR_UNIT_ID);

    *pEnable = usr_lb_sts[unit];
    return RT_ERR_OK;
}

/* Function Name:
 *      drv_nic_loopbackMode_set
 * Description:
 *      Set user's callback example function to loopback mode in SDK user mode.
 * Input:
 *      unit    - unit id
 *      enable  - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_UNIT_ID      - invalid unit id
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 */
int32
drv_nic_loopbackMode_set(uint32 unit, rtk_enable_t enable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID), RT_ERR_UNIT_ID);

    usr_lb_sts[unit] = enable;
    return RT_ERR_OK;
}
#endif

