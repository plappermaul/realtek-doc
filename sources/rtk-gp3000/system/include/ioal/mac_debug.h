/*
 * Copyright (C) 2012 Realtek Semiconductor Corp. 
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated, 
 * modified or distributed under the authorized license from Realtek. 
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER 
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED. 
 *
 * Purpose : For linux kernel mode
 *           MAC debug APIs in the SDK
 *
 * Feature : MAC debug APIs
 *
 */

#ifndef __MAC_DEBUG_H__
#define __MAC_DEBUG_H__


#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
#include <rtk/rtusr/rtusr_pkt.h>
#endif /* defined(CONFIG_LINUX_USER_SHELL) */


/* Function Name:
 *      mac_debug_rxPkt_enable
 * Description:
 *      Due to the rx packet debug might impact the performance, this API is to
 *      enable/disable the rx packet debug function
 * Input:
 *      enable - enable or disable the rx packet storage
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Dump command always success
 */
extern int32 mac_debug_rxPkt_enable(uint32 enable);

/* Function Name:
 *      mac_debug_rxPkt_get
 * Description:
 *      Get the last received packet
 * Input:
 *      pPayload   - entire packet to be sent
 *      buf_length - specified the maximum length to be dumped
 *      pkt_length - get the actul length of packet
 *      pInfo      - rx descriptor of packet
 *      pEnable    - get the dump copy state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Dump command always success
 */
extern int32 mac_debug_rxPkt_get(uint8 *pPayload, uint16 buf_length, uint16 *pPkt_len, struct pkt_dbg_rx_info *pInfo, unsigned int *pEnable);

/* Function Name:
 *      mac_debug_rxPktFifo_get
 * Description:
 *      Get the last received packet
 * Input:
 *      fifo_idx   - FIFO index to be dumped
 *      pPayload   - entire packet to be sent
 *      buf_length - specified the maximum length to be dumped
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 */
extern int32 
mac_debug_rxPktFifo_get(uint16 fifo_idx, uint8 *pPayload, uint16 buf_length);

/* Function Name:
 *      mac_debug_rxPkt_clear
 * Description:
 *      Clear the last received packet storage buffer
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Clear command always success
 */
extern int32 mac_debug_rxPkt_clear(void);

/* Function Name:
 *      mac_debug_txPkt_send
 * Description:
 *      Send the packet in the buffer
 * Input:
 *      pPayload - entire packet to be sent
 *      length   - packet length
 *      pInfo    - tx descriptor of packet
 *      count    - tx repeat count
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Dump command always success
 */
extern int32 
mac_debug_txPkt_send(uint8 *pPayload, uint16 length, struct pkt_dbg_tx_info *pInfo, uint32 count);

/* Function Name:
 *      mac_debug_txPktCont_send
 * Description:
 *      Send the packet in the buffer continuously
 * Input:
 *      pPayload - entire packet to be sent
 *      length   - packet length
 *      pInfo    - tx descriptor of packet
 *      enable   - enable/disable continous sending
 * Output:
 *      None
 * Return:
 *      Continue send command always success
 *      RT_ERR_OK
 */
extern int32 
mac_debug_txPktCont_send(uint8 *pPayload, uint16 length, struct pkt_dbg_tx_info *pInfo, uint8 enable);

/* Function Name:
 *      mac_debug_txPktContCnt_set
 * Description:
 *      Set send count for each time sending thread wake up
 * Input:
 *      count    - tx repeat count
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 */
extern int32 
mac_debug_txPktContCnt_set(uint32 count);

/* Function Name:
 *      mac_debug_txPktContCnt_get
 * Description:
 *      Get send count for each time sending thread wake up
 * Input:
 *      None
 * Output:
 *      pCount    - tx repeat count
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_INPUT
 */
extern int32 
mac_debug_txPktContCnt_get(uint32 *pCount);


#if defined(CONFIG_SDK_RTL9607C)
/* Function Name:
 *      mac_debug_rxGmac_set
 * Description:
 *      Set debug rx dump from what gmac num
 * Input:
 *      gmac     - rx gmac number
 * Output:
 *      
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_INPUT
 */
int32 
mac_debug_rxGmac_set(uint32 gmac);

/* Function Name:
 *      mac_debug_rxGmac_get
 * Description:
 *      Get debug rx dump from what gmac num
 * Input:
 *      
 * Output:
 *      pGmac     - rx gmac number
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_INPUT
 */
int32 
mac_debug_rxGmac_get(uint32 *pGmac);
#endif

#endif /* __MAC_DEBUG_H__ */

