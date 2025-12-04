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
 * $Revision: 42093 $
 * $Date: 2013-08-15 22:14:29 +0800 (星期四, 15 八月 2013) $
 *
 * Purpose : Definition of Switch Global API
 *
 * Feature : The file have include the following module and sub-modules
 *           (1) Switch parameter settings
 *           (2) Management address and vlan configuration.
 *
 */

#ifndef __DAL_RTL9603CVD_SWITCH_H__
#define __DAL_RTL9603CVD_SWITCH_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <hal/chipdef/chip.h>
#include <rtk/switch.h>

/*
 * Symbol Definition
 */


/* information of device */
typedef enum dal_rtl9603cvd_switch_asic_type_e
{
    RTL9603CVD_ASIC_PON,
    RTL9603CVD_ASIC_FIBER,
    RTL9603CVD_ASIC_END
} dal_rtl9603cvd_switch_asic_type_t;

typedef enum dal_rtl9603cvd_switch_txcrc_type_e
{
    RTL9603CVD_TXCRC_PKTCHANGED,
    RTL9603CVD_TXCRC_DISABLE,
    RTL9603CVD_TXCRC_RESERVED,
    RTL9603CVD_TXCRC_ALWAYS,
    RTL9603CVD_TXCRC_END,
} dal_rtl9603cvd_switch_txcrc_type_t;

#define RTL9603CVD_CHIP_SUB_TYPE_RTL9603C_VD4       0x01
#define RTL9603CVD_CHIP_SUB_TYPE_RTL9603C_VD5       0x04
#define RTL9603CVD_CHIP_SUB_TYPE_RTL9603C_VD6       0x08
#define RTL9603CVD_CHIP_SUB_TYPE_RTL9602C_VD5       0x05
#define RTL9603CVD_CHIP_SUB_TYPE_RTL9603CE_VD       0x09
#define RTL9603CVD_CHIP_SUB_TYPE_RTL9601D_VD3       0x14
#define RTL9603CVD_CHIP_SUB_TYPE_RTL9601D_VD4       0x18
#define RTL9603CVD_CHIP_SUB_TYPE_RTL9601D_VD5       0x1c
#define RTL9603CVD_CHIP_SUB_TYPE_RTL9601DW_VD5      0x1d

/*
 * Data Declaration
 */

/*
 * Function Declaration
 */

/* Module Name    : Switch     */
/* Sub-module Name: Switch parameter settings */

/* Function Name:
 *      dal_rtl9603cvd_switch_init
 * Description:
 *      Initialize switch module of the specified device.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Module must be initialized before using all of APIs in this module
 */
extern int32
dal_rtl9603cvd_switch_init(void);

/* Module Name    : Switch     */
/* Sub-module Name: Switch parameter settings */

/* Function Name:
 *      dal_rtl9603cvd_switch_phyPortId_get
 * Description:
 *      Get physical port id from logical port name
 * Input:
 *      portName - logical port name
 * Output:
 *      pPortId  - pointer to the physical port id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Call RTK API the port ID must get from this API
 */
extern int32
dal_rtl9603cvd_switch_phyPortId_get(rtk_switch_port_name_t portName, int32 *pPortId);

/* Function Name:
 *      dal_rtl9603cvd_switch_logicalPort_get
 * Description:
 *      Get logical port name from physical port id
 * Input:
 *      portId  - physical port id
 * Output:
 *      pPortName - pointer to logical port name
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
extern int32
dal_rtl9603cvd_switch_logicalPort_get(int32 portId, rtk_switch_port_name_t *pPortName);

/* Function Name:
 *      dal_rtl9603cvd_switch_port2PortMask_set
 * Description:
 *      Set port id to the portlist
 * Input:
 *      pPortMask    - port mask
 *      portName     - logical port name
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Call RTK API the port mask must set by this API
 */
extern int32
dal_rtl9603cvd_switch_port2PortMask_set(rtk_portmask_t *pPortMask, rtk_switch_port_name_t portName);



/* Function Name:
 *      dal_rtl9603cvd_switch_port2PortMask_set
 * Description:
 *      Set port id to the portlist
 * Input:
 *      pPortMask    - port mask
 *      portName     - logical port name
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Call RTK API the port mask must set by this API
 */
extern int32
dal_rtl9603cvd_switch_port2PortMask_clear(rtk_portmask_t *pPortMask, rtk_switch_port_name_t portName);



/* Function Name:
 *      dal_rtl9603cvd_switch_portIdInMask_check
 * Description:
 *      Check if given port is in port list
 * Input:
 *      pPortMask    - port mask
 *      portName     - logical port name
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
extern int32
dal_rtl9603cvd_switch_portIdInMask_check(rtk_portmask_t *pPortMask, rtk_switch_port_name_t portName);

#if 0
/* Function Name:
  *      dal_rtl9603cvd_switch_maxPktLenLinkSpeed_get
  * Description:
  *      Get the max packet length setting of the specific speed type
  * Input:
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
dal_rtl9603cvd_switch_maxPktLenLinkSpeed_get(rtk_switch_maxPktLen_linkSpeed_t speed, uint32 *pLen);

/* Function Name:
  *      dal_rtl9603cvd_switch_maxPktLenLinkSpeed_set
  * Description:
  *      Set the max packet length of the specific speed type
  * Input:
  *      speed - speed type
  *      len   - max packet length
  * Output:
  *      None
  * Return:
  *      RT_ERR_OK
  *      RT_ERR_FAILED
  *      RT_ERR_INPUT   - invalid enum speed type
  * Note:
  *      Max packet length setting speed type
  *      - MAXPKTLEN_LINK_SPEED_FE
  *      - MAXPKTLEN_LINK_SPEED_GE
  */
extern int32
dal_rtl9603cvd_switch_maxPktLenLinkSpeed_set(rtk_switch_maxPktLen_linkSpeed_t speed, uint32 len);
#endif


/* Module Name    : Switch     */
/* Sub-module Name: Management address and vlan configuration */


/* Function Name:
 *      dal_rtl9603cvd_switch_mgmtMacAddr_get
 * Description:
 *      Get MAC address of switch.
 * Input:
 *      None
 * Output:
 *      pMac - pointer to MAC address
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_rtl9603cvd_switch_mgmtMacAddr_get(rtk_mac_t *pMac);

/* Function Name:
 *      dal_rtl9603cvd_switch_mgmtMacAddr_set
 * Description:
 *      Set MAC address of switch.
 * Input:
 *      pMac - MAC address
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
dal_rtl9603cvd_switch_mgmtMacAddr_set(rtk_mac_t *pMac);


/* Function Name:
 *      dal_apollo_switch_chip_reset
 * Description:
 *      Reset switch chip
 * Input:
 *      None
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
dal_rtl9603cvd_switch_chip_reset(void);



/* Function Name:
 *      dal_rtl9603cvd_switch_version_get
 * Description:
 *      Get chip version
 * Input:
 *      pChipId    - chip id
 *      pRev       - revision id
 *      pSubtype   - sub type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 */
extern int32
dal_rtl9603cvd_switch_version_get(uint32 *pChipId, uint32 *pRev, uint32 *pSubtype);
/* Function Name:
  *      dal_rtl9603cvd_switch_maxPktLenByPort_get
  * Description:
  *      Get the max packet length setting of specific port
  * Input:
  *      port - speed type
  * Output:
  *      pLen - pointer to the max packet length
  * Return:
  *      RT_ERR_OK
  *      RT_ERR_FAILED
  *      RT_ERR_NULL_POINTER - input parameter may be null pointer
  *      RT_ERR_INPUT        - invalid enum speed type
  * Note:
  */
extern int32
dal_rtl9603cvd_switch_maxPktLenByPort_get(rtk_port_t port, uint32 *pLen);

/* Function Name:
  *      dal_rtl9603cvd_switch_maxPktLenByPort_set
  * Description:
  *      Set the max packet length of specific port
  * Input:
  *      port  - port
  *      len   - max packet length
  * Output:
  *      None
  * Return:
  *      RT_ERR_OK
  *      RT_ERR_FAILED
  *      RT_ERR_INPUT   - invalid enum speed type
  * Note:
  */
extern int32
dal_rtl9603cvd_switch_maxPktLenByPort_set(rtk_port_t port, uint32 len);

/* Function Name:
  *      dal_rtl9603cvd_switch_changeDuplex_get
  * Description:
  *      Get change duplex function state
  * Input:
  *      enable   - change duplex state
  * Output:
  *      None
  * Return:
  *      RT_ERR_OK
  *      RT_ERR_FAILED
  *      RT_ERR_INPUT   - invalid enum speed type
  * Note:
  */
extern int32
dal_rtl9603cvd_switch_changeDuplex_get(rtk_enable_t *pState);

/* Function Name:
  *      dal_rtl9603cvd_switch_changeDuplex_set
  * Description:
  *      Set change duplex function state
  * Input:
  *      enable   - change duplex state
  * Output:
  *      None
  * Return:
  *      RT_ERR_OK
  *      RT_ERR_FAILED
  *      RT_ERR_INPUT   - invalid enum speed type
  * Note:
  * This function only apply to local N-way enable but link
  * partner in force mode. In that way, the local link status
  * will be 100Mb/half duplex. This function will change
  * local link status to 100Mb/full duplex under specific
  * condition.
  */
extern int32
dal_rtl9603cvd_switch_changeDuplex_set(rtk_enable_t state);


/* Function Name:
  *      dal_rtl9603cvd_switch_system_init
  * Description:
  *      Set system application initial
  * Input:
  *      mode   - initial mode
  * Output:
  *      None
  * Return:
  *      RT_ERR_OK
  *      RT_ERR_FAILED
  * Note:
  *     This function is used for SFU application system
  */
extern int32
dal_rtl9603cvd_switch_system_init(rtk_switch_system_mode_t *pMode);


/* Function Name:
  *      dal_dal9603cvd_switch_thermal_get
  * Description:
  *      Get soc thermal value
  * Input:
  *      none
  * Output:
  *       thermalIntger  - pointer to thermal value for integer part
  *       thermalDecimal - pointer to thermal value for Decimal part
  * Return:
  *      RT_ERR_OK
  *      RT_ERR_FAILED
  *      RT_ERR_NULL_POINTER - input parameter may be null pointer
  *      RT_ERR_INPUT        - invalid enum speed type
  * Note:
  */
extern int32
dal_dal9603cvd_switch_thermal_get(int32 *thermalIntger, int32 *thermalDecimal);
#endif /* __DAL_RTL9601B_SWITCH_H__ */

