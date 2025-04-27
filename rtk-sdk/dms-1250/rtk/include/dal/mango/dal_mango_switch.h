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
 * Purpose : Definition those public global APIs and its data type in the SDK.
 *
 * Feature :  Parameter settings for the system-wise view
 *
 */
#ifndef __DAL_MANGO_SWITCH_H__
#define __DAL_MANGO_SWITCH_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <rtk/switch.h>
#include <dal/dal_mapper.h>


/*
 * Symbol Definition
 */

typedef enum dal_mango_switch_tbl_e
{
    /* must start with `0`,
     * somewhere may have for-loop for each.
     */

    DAL_MANGO_SWITCH_TBL_VLAN = 0,  /* VLAN entry */
    DAL_MANGO_SWITCH_TBL_L2_MAC,    /* L2 MAC entry */
    DAL_MANGO_SWITCH_TBL_L3_IPMC,   /* L3 IPMC table */

    DAL_MANGO_SWITCH_TBL_END,
} dal_mango_switch_tbl_t;


/*
 * Data Declaration
 */

/*
 * Macro Definition
 */


/*
 * Function Declaration
 */

/* Function Name:
 *      dal_mango_switchMapper_init
 * Description:
 *      Hook switch module of the specified device.
 * Input:
 *      pMapper - pointer of mapper
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Must Hook switch module before calling any switch APIs.
 */
extern int32
dal_mango_switchMapper_init(dal_mapper_t *pMapper);

/* Function Name:
 *      dal_mango_switch_init
 * Description:
 *      Initialize switch module of the specified device.
 * Input:
 *      unit          - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      1. Module must be initialized before using all of APIs in this module
 */
extern int32
dal_mango_switch_init(uint32 unit);

/* Function Name:
 *      _dal_mango_switch_tblSemaphore_lock
 * Description:
 *      Lock a table by taking its semaphore
 * Input:
 *      unit - unit id
 *      tbl  - table id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_ENTRY_NOT_FOUND - entry not found
 * Note:
 *      None
 */
extern int32
_dal_mango_switch_tblSemaphore_lock(uint32 unit, dal_mango_switch_tbl_t tbl);

/* Function Name:
 *      _dal_mango_switch_tblSemaphore_unlock
 * Description:
 *      Unlock a table by giving its semaphore
 * Input:
 *      unit - unit id
 *      tbl  - table id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_ENTRY_NOT_FOUND - entry not found
 * Note:
 *      None
 */
extern int32
_dal_mango_switch_tblSemaphore_unlock(uint32 unit, dal_mango_switch_tbl_t tbl);

/* Function Name:
 *      dal_mango_switch_cpuMaxPktLen_get
 * Description:
 *      Get the max packet length setting on CPU port of the specific unit
 * Input:
 *      unit - unit id
 *      dir  - direction of packet
 * Output:
 *      pLen - pointer to the max packet length
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_INPUT        - invalid packet direction
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_switch_cpuMaxPktLen_get(uint32 unit, rtk_switch_pktDir_t dir, uint32 *pLen);

/* Function Name:
 *      dal_mango_switch_cpuMaxPktLen_set
 * Description:
 *      Set the max packet length setting on CPU port of the specific unit
 * Input:
 *      unit - unit id
 *      dir  - direction of packet
 *      len  - max packet length
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_INPUT        - invalid packet direction
 *      RT_ERR_OUT_OF_RANGE - invalid packet length
 * Note:
 *      None
 */
extern int32
dal_mango_switch_cpuMaxPktLen_set(uint32 unit, rtk_switch_pktDir_t dir, uint32 len);

/* Function Name:
 *      dal_mango_switch_snapMode_get
 * Description:
 *      Get SNAP mode.
 * Input:
 *      unit      - unit id
 * Output:
 *      pSnapMode - pointer to SNAP mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      SNAP mode is as following
 *      - SNAP_MODE_AAAA03000000
 *      - SNAP_MODE_AAAA03
 */
extern int32
dal_mango_switch_snapMode_get(uint32 unit, rtk_snapMode_t *pSnapMode);

/* Function Name:
 *      dal_mango_switch_snapMode_set
 * Description:
 *      Set SNAP mode.
 * Input:
 *      unit     - unit id
 *      snapMode - SNAP mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      SNAP mode is as following
 *      - SNAP_MODE_AAAA03000000
 *      - SNAP_MODE_AAAA03
 */
extern int32
dal_mango_switch_snapMode_set(uint32 unit, rtk_snapMode_t snapMode);

/* Function Name:
 *      dal_mango_switch_mgmtMacAddr_get
 * Description:
 *      Get Mac address of switch.
 * Input:
 *      unit - unit id
 * Output:
 *      pMac - pointer to Mac address
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_switch_mgmtMacAddr_get(uint32 unit, rtk_mac_t *pMac);

/* Function Name:
 *      dal_mango_switch_mgmtMacAddr_set
 * Description:
 *      Set Mac address of switch.
 * Input:
 *      unit - unit id
 *      pMac - pointer to Mac address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_MAC          - invalid mac address
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_switch_mgmtMacAddr_set(uint32 unit, rtk_mac_t *pMac);

/* Function Name:
 *      dal_mango_switch_recalcCRCEnable_get
 * Description:
 *      Get enable status of recaculate CRC on specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to enable status of recaculate CRC
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      When enable, mirrored packet with bad CRC will be recaculate at mirroring port.
 */
extern int32
dal_mango_switch_recalcCRCEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_mango_switch_recalcCRCEnable_set
 * Description:
 *      Set enable status of recaculate CRC on specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - enable status of recaculate CRC
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      When enable, mirrored packet with bad CRC will be recaculate at mirroring port.
 */
extern int32
dal_mango_switch_recalcCRCEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      dal_mango_switch_pppoeIpParseEnable_get
 * Description:
 *      Get enable status of PPPoE pass-through functionality.
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - pointer to enable status of PPPoE parse functionality
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_UNIT_ID          - invalid unit id
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_switch_pppoeIpParseEnable_get(uint32 unit, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_mango_switch_pppoeIpParseEnable_set
 * Description:
 *      Set enable status of PPPoE pass-through functionality.
 * Input:
 *      unit   - unit id
 *      enable - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_UNIT_ID          - invalid unit id
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_mango_switch_pppoeIpParseEnable_set(uint32 unit, rtk_enable_t enable);

/* Function Name:
 *      dal_mango_switch_pkt2CpuTypeFormat_get
 * Description:
 *      Get the configuration about content state for packet to CPU
 * Input:
 *      unit                - unit id
 *      type                - method of packet to CPU
 * Output:
 *      pFormat             - type of format
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT         - invalid input parameter
 *      RT_ERR_NULL_POINTER  - NULL pointer
 * Note:
 *      The type of packet to CPU:
 *      - PKT2CPU_TYPE_FORWARD
 *      - PKT2CPU_TYPE_TRAP
 *      - PKT2CPU_ETAG_TYPE_FORWARD
 *      - PKT2CPU_ETAG_TYPE_TRAP
 *
 *      The type of format:
 *      - TRAP_PKT_ORIGINAL
 *      - TRAP_PKT_MODIFIED
 */
extern int32
dal_mango_switch_pkt2CpuTypeFormat_get(uint32 unit, rtk_switch_pkt2CpuType_t type,
    rtk_pktFormat_t *pFormat);

/* Function Name:
 *      dal_mango_switch_pkt2CpuTypeFormat_set
 * Description:
 *      Set the configuration about content state for packet to CPU
 * Input:
 *      unit        - unit id
 *      type        - method of packet to CPU
 *      format      - packet format to CPU
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT      - invalid input parameter
 * Note:
 *      The type of format:
 *      - TRAP_PKT_ORIGINAL
 *      - TRAP_PKT_MODIFIED
 */
extern int32
dal_mango_switch_pkt2CpuTypeFormat_set(uint32 unit, rtk_switch_pkt2CpuType_t type,
    rtk_pktFormat_t format);

/* Function Name:
 *      dal_mango_switch_cpuPktTruncateEnable_get
 * Description:
 *      Get CPU port truncation function state.
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - enable state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) Packet is truncated to specified length if it is trap/forward to CPU and packet length is
 *          over than the truncation length.
 *      (2) The truncation length is specified by rtk_switch_cpuPktTruncateLen_set.
 */
extern int32
dal_mango_switch_cpuPktTruncateEnable_get(uint32 unit, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_mango_switch_cpuPktTruncateEnable_set
 * Description:
 *      Set CPU port truncation function state.
 * Input:
 *      unit    - unit id
 *      enable  - enable state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      (1) Packet is truncated to specified length if it is trap/forward to CPU and packet length is
 *          over than the truncation length.
 *      (2) The truncation length is specified by rtk_switch_cpuPktTruncateLen_set.
 */
extern int32
dal_mango_switch_cpuPktTruncateEnable_set(uint32 unit, rtk_enable_t enable);

/* Function Name:
 *      dal_mango_switch_cpuPktTruncateLen_get
 * Description:
 *      Get the packet length for CPU port truncation function.
 * Input:
 *      unit - unit id
 * Output:
 *      pLen  - packet length to truncate
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) Packet is truncated to specified length if it is trap/forward to CPU and packet length is
 *          over than the truncation length.
 *      (2) The truncation function takes effect if rtk_switch_cpuPktTruncateEnable_set is enabled.
 */
extern int32
dal_mango_switch_cpuPktTruncateLen_get(uint32 unit, uint32 *pLen);

/* Function Name:
 *      dal_mango_switch_cpuPktTruncateLen_set
 * Description:
 *      Set the packet length for CPU port truncation function.
 * Input:
 *      unit - unit id
 *      len  - packet length to truncate
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - invalid truncation length
 * Note:
 *      (1) Packet is truncated to specified length if it is trap/forward to CPU and packet length is
 *          over than the truncation length.
 *      (2) The truncation function takes effect if rtk_switch_cpuPktTruncateEnable_set is enabled.
 */
extern int32
dal_mango_switch_cpuPktTruncateLen_set(uint32 unit, uint32 len);

/* Function Name:
 *      dal_mango_switch_portMaxPktLenLinkSpeed_get
 * Description:
 *      Get the max packet length setting of the specific speed type and port
 * Input:
 *      unit  - unit id
 *      port  - the specific port
 *      speed - speed type
 * Output:
 *      pLen  - pointer to the max packet length
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid enum speed type
 * Note:
 *      Max packet length setting speed type
 *      - MAXPKTLEN_LINK_SPEED_FE
 *      - MAXPKTLEN_LINK_SPEED_GE
 */
extern int32
dal_mango_switch_portMaxPktLenLinkSpeed_get(uint32 unit, rtk_port_t port,
    rtk_switch_maxPktLen_linkSpeed_t speed, uint32 *pLen);

/* Function Name:
 *      dal_mango_switch_portMaxPktLenLinkSpeed_set
 * Description:
 *      Set the max packet length of the specific speed type and port
 * Input:
 *      unit  - unit id
 *      port  - the specific port
 *      speed - speed type
 *      len   - max packet length
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT        - invalid enum speed type
 *      RT_ERR_OUT_OF_RANGE - invalid packet length
 * Note:
 *      Max packet length setting speed type
 *      - MAXPKTLEN_LINK_SPEED_FE
 *      - MAXPKTLEN_LINK_SPEED_GE
 */
extern int32
dal_mango_switch_portMaxPktLenLinkSpeed_set(uint32 unit, rtk_port_t port,
    rtk_switch_maxPktLen_linkSpeed_t speed, uint32 len);

/* Function Name:
 *      dal_mango_switch_portMaxPktLenTagLenCntIncEnable_get
 * Description:
 *      Get include or exclude tag length state of max packet length
 *      in the specified port.
 * Input:
 *      unit    - unit id
 *      port    - the specific port
 * Output:
 *      pEnable     - pointer buffer of including/excluding tag length
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_switch_portMaxPktLenTagLenCntIncEnable_get(uint32 unit, rtk_port_t port,
    rtk_enable_t *pEnable);

/* Function Name:
 *      dal_mango_switch_portMaxPktLenTagLenCntIncEnable_set
 * Description:
 *      Set the max packet length to include or exclude tag length
 *      in the specified port.
 * Input:
 *      unit    - unit id
 *      port    - the specific port
 *      enable  - include/exclude Tag length
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT   - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_mango_switch_portMaxPktLenTagLenCntIncEnable_set(uint32 unit, rtk_port_t port,
    rtk_enable_t enable);

/* Function Name:
 *      dal_mango_switch_flexTblFmt_get
 * Description:
 *      Get the flexible table format
 * Input:
 *      unit - unit id
 * Output:
 *      pFmt - pointer to the table format
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_switch_flexTblFmt_get(uint32 unit, rtk_switch_flexTblFmt_t *pFmt);

/* Function Name:
 *      dal_mango_switch_flexTblFmt_set
 * Description:
 *      Set the flexible table format
 * Input:
 *      unit - unit id
 * Output:
 *      fmt  - table format
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 * Note:
 *      None
 */
extern int32
dal_mango_switch_flexTblFmt_set(uint32 unit, rtk_switch_flexTblFmt_t fmt);

#endif /* __DAL_MANGO_SWITCH_H__ */

