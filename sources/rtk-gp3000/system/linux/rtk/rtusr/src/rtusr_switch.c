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
 * $Revision$
 * $Date$
 *
 * Purpose : Definition of Switch Global API
 *
 * Feature : The file have include the following module and sub-modules
 *           (1) Switch parameter settings
 *           (2) Management address and vlan configuration.
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
#include <common/rt_type.h>
#include <hal/chipdef/chip.h>

/*
 * Symbol Definition
 */
/*
 * Function Declaration
 */

/* Module Name    : Switch     */
/* Sub-module Name: Switch parameter settings */

/* Function Name:
 *      rtk_switch_init
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
int32
rtk_switch_init(void)
{
    rtdrv_switchCfg_t switch_cfg;

    /* function body */

    osal_memset(&switch_cfg,0x0,sizeof(rtdrv_switchCfg_t));
    SETSOCKOPT(RTDRV_SWITCH_INIT, &switch_cfg, rtdrv_switchCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_switch_init */

/* Module Name    : Switch     */
/* Sub-module Name: Switch parameter settings */

/* Function Name:
 *      rtk_switch_deviceInfo_get
 * Description:
 *      Get device information of the specific unit
 * Input:
 *      none
 * Output:
 *      pDevInfo - pointer to the device information
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_switch_deviceInfo_get(rtk_switch_devInfo_t *pDevInfo)
{
    rtdrv_switchCfg_t switch_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pDevInfo), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&switch_cfg.devInfo, pDevInfo, sizeof(rtk_switch_devInfo_t));
    GETSOCKOPT(RTDRV_SWITCH_DEVICEINFO_GET, &switch_cfg, rtdrv_switchCfg_t, 1);
    osal_memcpy(pDevInfo, &switch_cfg.devInfo, sizeof(rtk_switch_devInfo_t));

    return RT_ERR_OK;
}   /* end of rtk_switch_deviceInfo_get */


/* Function Name:
 *      rtk_switch_phyPortId_get
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
int32
rtk_switch_phyPortId_get(rtk_switch_port_name_t portName, int32 *pPortId)
{
    rtdrv_switchCfg_t switch_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortId), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&switch_cfg.portName, &portName, sizeof(rtk_switch_port_name_t));
    GETSOCKOPT(RTDRV_SWITCH_PHYPORTID_GET, &switch_cfg, rtdrv_switchCfg_t, 1);
    osal_memcpy(pPortId, &switch_cfg.portId, sizeof(int32));

    return RT_ERR_OK;
}   /* end of rtk_switch_phyPortId_get */


/* Function Name:
 *      rtk_switch_logicalPort_get
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
int32
rtk_switch_logicalPort_get(int32 portId, rtk_switch_port_name_t *pPortName)
{
    rtdrv_switchCfg_t switch_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortName), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&switch_cfg.portId, &portId, sizeof(int32));
    GETSOCKOPT(RTDRV_SWITCH_LOGICALPORT_GET, &switch_cfg, rtdrv_switchCfg_t, 1);
    osal_memcpy(pPortName, &switch_cfg.portName, sizeof(rtk_switch_port_name_t));

    return RT_ERR_OK;
}   /* end of rtk_switch_logicalPort_get */


/* Function Name:
 *      rtk_switch_port2PortMask_set
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
int32
rtk_switch_port2PortMask_set(rtk_portmask_t *pPortMask, rtk_switch_port_name_t portName)
{
    rtdrv_switchCfg_t switch_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortMask), RT_ERR_NULL_POINTER);

    /* function body */

    osal_memset(&switch_cfg,0x0,sizeof(rtdrv_switchCfg_t));
    osal_memcpy(&switch_cfg.portMask, pPortMask, sizeof(rtk_portmask_t));
    osal_memcpy(&switch_cfg.portName, &portName, sizeof(rtk_switch_port_name_t));
    SETSOCKOPT(RTDRV_SWITCH_PORT2PORTMASK_SET, &switch_cfg, rtdrv_switchCfg_t, 1);
    osal_memcpy(pPortMask, &switch_cfg.portMask, sizeof(rtk_portmask_t));

    return RT_ERR_OK;
}   /* end of rtk_switch_port2PortMask_set */



/* Function Name:
 *      rtk_switch_port2PortMask_set
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
int32
rtk_switch_port2PortMask_clear(rtk_portmask_t *pPortMask, rtk_switch_port_name_t portName)
{
    rtdrv_switchCfg_t switch_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortMask), RT_ERR_NULL_POINTER);

    /* function body */

    osal_memset(&switch_cfg,0x0,sizeof(rtdrv_switchCfg_t));
    osal_memcpy(&switch_cfg.portMask, pPortMask, sizeof(rtk_portmask_t));
    osal_memcpy(&switch_cfg.portName, &portName, sizeof(rtk_switch_port_name_t));
    SETSOCKOPT(RTDRV_SWITCH_PORT2PORTMASK_CLEAR, &switch_cfg, rtdrv_switchCfg_t, 1);
    osal_memcpy(pPortMask, &switch_cfg.portMask, sizeof(rtk_portmask_t));

    return RT_ERR_OK;
}   /* end of rtk_switch_port2PortMask_clear */



/* Function Name:
 *      rtk_switch_portIdInMask_check
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
int32
rtk_switch_portIdInMask_check(rtk_portmask_t *pPortMask, rtk_switch_port_name_t portName)
{
    rtdrv_switchCfg_t switch_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortMask), RT_ERR_NULL_POINTER);

    /* function body */

    osal_memset(&switch_cfg,0x0,sizeof(rtdrv_switchCfg_t));
    osal_memcpy(&switch_cfg.portName, &portName, sizeof(rtk_switch_port_name_t));
    GETSOCKOPT(RTDRV_SWITCH_PORTIDINMASK_CHECK, &switch_cfg, rtdrv_switchCfg_t, 1);
    osal_memcpy(pPortMask, &switch_cfg.portMask, sizeof(rtk_portmask_t));

    return RT_ERR_OK;
}   /* end of rtk_switch_portIdInMask_check */




/* Function Name:
 *      rtk_switch_portMask_Clear
 * Description:
 *      Clear all port mask
 * Input:
 *      pPortMask    - port mask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32
rtk_switch_portMask_Clear(rtk_portmask_t *pPortMask)
{
    rtdrv_switchCfg_t switch_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortMask), RT_ERR_NULL_POINTER);

    /* function body */

    osal_memset(&switch_cfg,0x0,sizeof(rtdrv_switchCfg_t));
    osal_memcpy(&switch_cfg.portMask, pPortMask, sizeof(rtk_portmask_t));
    GETSOCKOPT(RTDRV_SWITCH_PORTMASK_CLEAR, &switch_cfg, rtdrv_switchCfg_t, 1);
    osal_memcpy(pPortMask, &switch_cfg.portMask, sizeof(rtk_portmask_t));

    return RT_ERR_OK;
}   /* end of rtk_switch_portMask_Clear */



/* Function Name:
 *      rtk_switch_allPortMask_set
 * Description:
 *      Set all switch port to mask
 * Input:
 *      pPortMask    - port mask
 * Output:
 *      pPortMask    - port mask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32
rtk_switch_allPortMask_set(rtk_portmask_t *pPortMask)
{
    rtdrv_switchCfg_t switch_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortMask), RT_ERR_NULL_POINTER);

    /* function body */

    osal_memset(&switch_cfg,0x0,sizeof(rtdrv_switchCfg_t));
    osal_memcpy(&switch_cfg.portMask, pPortMask, sizeof(rtk_portmask_t));
    SETSOCKOPT(RTDRV_SWITCH_ALLPORTMASK_SET, &switch_cfg, rtdrv_switchCfg_t, 1);
    osal_memcpy(pPortMask, &switch_cfg.portMask, sizeof(rtk_portmask_t));

    return RT_ERR_OK;
}   /* end of rtk_switch_allPortMask_set */



/* Function Name:
 *      rtk_switch_allExtPortMask_set
 * Description:
 *      Set all extention port to mask
 * Input:
 *      pPortMask    - port mask
 * Output:
 *      pPortMask    - port mask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32
rtk_switch_allExtPortMask_set(rtk_portmask_t *pPortMask)
{
    rtdrv_switchCfg_t switch_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortMask), RT_ERR_NULL_POINTER);

    /* function body */

    osal_memset(&switch_cfg,0x0,sizeof(rtdrv_switchCfg_t));
    osal_memcpy(&switch_cfg.portMask, pPortMask, sizeof(rtk_portmask_t));
    SETSOCKOPT(RTDRV_SWITCH_ALLEXTPORTMASK_SET, &switch_cfg, rtdrv_switchCfg_t, 1);
    osal_memcpy(pPortMask, &switch_cfg.portMask, sizeof(rtk_portmask_t));

    return RT_ERR_OK;
}   /* end of rtk_switch_allExtPortMask_set */


/* Function Name:
 *      rtk_switch_nextPortInMask_get
 * Description:
 *      Get next port id in the port mask
 * Input:
 *      pPortMask    - port mask
 *      pPortId      - given port id to get the next port in the port mask
 * Output:
 *      pPortId      - the next port id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      -RT_ERR_OK means the pPortId record the next port,
 *           others do not have any port for given port id next port
 *      -Set *pPortId to RTK_SWITCH_GET_FIRST_PORT to get the first port
 */
int32
rtk_switch_nextPortInMask_get(rtk_portmask_t *pPortMask, int32 *pPortId)
{
    rtdrv_switchCfg_t switch_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortMask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pPortId), RT_ERR_NULL_POINTER);

    /* function body */

    osal_memcpy(&switch_cfg.portMask, pPortMask, sizeof(rtk_portmask_t));
    osal_memcpy(&switch_cfg.portId, pPortId, sizeof(int32));
    GETSOCKOPT(RTDRV_SWITCH_NEXTPORTINMASK_GET, &switch_cfg, rtdrv_switchCfg_t, 1);
    osal_memcpy(pPortMask, &switch_cfg.portMask, sizeof(rtk_portmask_t));
    osal_memcpy(pPortId, &switch_cfg.portId, sizeof(int32));

    return RT_ERR_OK;
}   /* end of rtk_switch_nextPortInMask_get */

/* Function Name:
  *      rtk_switch_maxPktLenLinkSpeed_get
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
int32
rtk_switch_maxPktLenLinkSpeed_get(rtk_switch_maxPktLen_linkSpeed_t speed, uint32 *pLen)
{
    rtdrv_switchCfg_t switch_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pLen), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&switch_cfg.speed, &speed, sizeof(rtk_switch_maxPktLen_linkSpeed_t));
    GETSOCKOPT(RTDRV_SWITCH_MAXPKTLENLINKSPEED_GET, &switch_cfg, rtdrv_switchCfg_t, 1);
    osal_memcpy(pLen, &switch_cfg.len, sizeof(uint32));

    return RT_ERR_OK;
}   /* end of rtk_switch_maxPktLenLinkSpeed_get */

/* Function Name:
  *      rtk_switch_maxPktLenLinkSpeed_set
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
int32
rtk_switch_maxPktLenLinkSpeed_set(rtk_switch_maxPktLen_linkSpeed_t speed, uint32 len)
{
    rtdrv_switchCfg_t switch_cfg;

    /* function body */

    osal_memset(&switch_cfg,0x0,sizeof(rtdrv_switchCfg_t));
    osal_memcpy(&switch_cfg.speed, &speed, sizeof(rtk_switch_maxPktLen_linkSpeed_t));
    osal_memcpy(&switch_cfg.len, &len, sizeof(uint32));
    SETSOCKOPT(RTDRV_SWITCH_MAXPKTLENLINKSPEED_SET, &switch_cfg, rtdrv_switchCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_switch_maxPktLenLinkSpeed_set */


/* Module Name    : Switch     */
/* Sub-module Name: Management address and vlan configuration */


/* Function Name:
 *      rtk_switch_mgmtMacAddr_get
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
int32
rtk_switch_mgmtMacAddr_get(rtk_mac_t *pMac)
{
    rtdrv_switchCfg_t switch_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pMac), RT_ERR_NULL_POINTER);

    /* function body */
    GETSOCKOPT(RTDRV_SWITCH_MGMTMACADDR_GET, &switch_cfg, rtdrv_switchCfg_t, 1);
    osal_memcpy(pMac, &switch_cfg.mac, sizeof(rtk_mac_t));

    return RT_ERR_OK;
}   /* end of rtk_switch_mgmtMacAddr_get */

/* Function Name:
 *      rtk_switch_mgmtMacAddr_set
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
int32
rtk_switch_mgmtMacAddr_set(rtk_mac_t *pMac)
{
    rtdrv_switchCfg_t switch_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pMac), RT_ERR_NULL_POINTER);

    /* function body */

    osal_memset(&switch_cfg,0x0,sizeof(rtdrv_switchCfg_t));
    osal_memcpy(&switch_cfg.mac, pMac, sizeof(rtk_mac_t));
    SETSOCKOPT(RTDRV_SWITCH_MGMTMACADDR_SET, &switch_cfg, rtdrv_switchCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_switch_mgmtMacAddr_set */



/* Function Name:
 *      rtk_switch_mgmtMacAddr_set
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
int32
rtk_switch_chip_reset(void)
{
    rtdrv_switchCfg_t switch_cfg;

    osal_memset(&switch_cfg,0x0,sizeof(rtdrv_switchCfg_t));
    osal_memset(&switch_cfg, 0, sizeof(rtdrv_switchCfg_t));
    SETSOCKOPT(RTDRV_SWITCH_CHIP_RESET, &switch_cfg, rtdrv_switchCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_switch_mgmtMacAddr_set */

/* Function Name:
 *      rtk_switch_version_get
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
int32
rtk_switch_version_get(uint32 *pChipId, uint32 *pRev, uint32 *pSubtype)
{
    rtdrv_switchCfg_t switch_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pChipId), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pRev), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pSubtype), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&switch_cfg.subtype, pSubtype, sizeof(uint32));
    GETSOCKOPT(RTDRV_SWITCH_VERSION_GET, &switch_cfg, rtdrv_switchCfg_t, 1);
    osal_memcpy(pChipId, &switch_cfg.chipId, sizeof(uint32));
    osal_memcpy(pRev, &switch_cfg.rev, sizeof(uint32));
    osal_memcpy(pSubtype, &switch_cfg.subtype, sizeof(uint32));

    return RT_ERR_OK;
}   /* end of rtk_switch_version_get */

/* Function Name:
 *      rtk_switch_patch_info_get
 * Description:
 *      Get patch info
 * Input:
 *      idx        - patch addr
 *      pData      - patch data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 */
int32
rtk_switch_patch_info_get(uint32 idx, uint32 *pData)
{
    rtdrv_switchCfg_t switch_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&switch_cfg.patchIdx, &idx, sizeof(uint32));
    GETSOCKOPT(RTDRV_SWITCH_PATCH_INFO_GET, &switch_cfg, rtdrv_switchCfg_t, 1);
    osal_memcpy(pData, &switch_cfg.patchData, sizeof(uint32));

    return RT_ERR_OK;
}   /* end of rtk_switch_patch_info_get */

/* Function Name:
 *      rtk_switch_csExtId_get
 * Description:
 *      Get customer identification
 * Input:
 *      pExtId     - ext id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 */
int32
rtk_switch_csExtId_get(uint32 *pExtId)
{
    rtdrv_switchCfg_t switch_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pExtId), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&switch_cfg.csExtId, pExtId, sizeof(uint32));
    GETSOCKOPT(RTDRV_SWITCH_CSEXTID_GET, &switch_cfg, rtdrv_switchCfg_t, 1);
    osal_memcpy(pExtId, &switch_cfg.csExtId, sizeof(uint32));

    return RT_ERR_OK;
}   /* end of rtk_switch_csExtId_get */

/* Function Name:
  *      rtk_switch_maxPktLenByPort_get
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
int32
rtk_switch_maxPktLenByPort_get(rtk_port_t port, uint32 *pLen)
{
	rtdrv_switchCfg_t switch_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pLen), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&switch_cfg.portId, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_SWITCH_MAXPKTLENBYPORT_GET, &switch_cfg, rtdrv_switchCfg_t, 1);
    osal_memcpy(pLen, &switch_cfg.len, sizeof(uint32));

	return RT_ERR_OK;
} /*end of rtk_switch_maxPktLenByPort_get*/

/* Function Name:
  *      rtk_switch_maxPktLenByPort_set
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
int32
rtk_switch_maxPktLenByPort_set(rtk_port_t port, uint32 len)
{
	rtdrv_switchCfg_t switch_cfg;

    /* function body */

    osal_memset(&switch_cfg,0x0,sizeof(rtdrv_switchCfg_t));
    osal_memcpy(&switch_cfg.portId, &port, sizeof(rtk_port_t));
    osal_memcpy(&switch_cfg.len, &len, sizeof(uint32));
    SETSOCKOPT(RTDRV_SWITCH_MAXPKTLENBYPORT_SET, &switch_cfg, rtdrv_switchCfg_t, 1);

    return RT_ERR_OK;
} /*end of rtk_switch_maxPktLenByPort_set*/

/* Function Name:
  *      rtk_switch_changeDuplex_get
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
int32
rtk_switch_changeDuplex_get(rtk_enable_t *pState)
{
	rtdrv_switchCfg_t switch_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&switch_cfg, 0, sizeof(rtdrv_switchCfg_t));
    GETSOCKOPT(RTDRV_SWITCH_CHANGEDUPLEX_GET, &switch_cfg, rtdrv_switchCfg_t, 1);
    osal_memcpy(pState, &switch_cfg.state, sizeof(rtk_enable_t));

	return RT_ERR_OK;
} /* end of rtk_switch_changeDuplex_get */

/* Function Name:
  *      rtk_switch_changeDuplex_set
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
int32
rtk_switch_changeDuplex_set(rtk_enable_t state)
{
	rtdrv_switchCfg_t switch_cfg;

    /* function body */

    osal_memset(&switch_cfg,0x0,sizeof(rtdrv_switchCfg_t));
    osal_memcpy(&switch_cfg.state, &state, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_SWITCH_CHANGEDUPLEX_SET, &switch_cfg, rtdrv_switchCfg_t, 1);

    return RT_ERR_OK;
} /* end of rtk_switch_changeDuplex_set */

/* Function Name:
 *      rtk_switch_system_init
 * Description:
 *		Set system application initial
 * Input:
 *		pMode	- initial mode 
 * Output:
 *		None
 * Return:
 *		RT_ERR_OK
 *		RT_ERR_FAILED
 * Note:
 *	   This function is used for SFU/SFP application system
 */
int32
rtk_switch_system_init(rtk_switch_system_mode_t *pMode)
{
    rtdrv_switchCfg_t switch_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pMode), RT_ERR_NULL_POINTER);

    /* function body */

    osal_memset(&switch_cfg,0x0,sizeof(rtdrv_switchCfg_t));
    osal_memcpy(&switch_cfg.mode, pMode, sizeof(rtk_switch_system_mode_t));
    SETSOCKOPT(RTDRV_SWITCH_SYSTEM_INIT, &switch_cfg, rtdrv_switchCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_switch_system_init */



/* Function Name:
  *      rtk_switch_thermal_get
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
int32
rtk_switch_thermal_get(int32 *thermalIntger, int32 *thermalDecimal)
{
    rtdrv_switchCfg_t switch_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == thermalIntger), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == thermalDecimal), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&switch_cfg.thermalDecimal, thermalDecimal, sizeof(uint32));
    GETSOCKOPT(RTDRV_SWITCH_THERMAL_GET, &switch_cfg, rtdrv_switchCfg_t, 1);
    osal_memcpy(thermalIntger, &switch_cfg.thermalIntger, sizeof(int32));
    osal_memcpy(thermalDecimal, &switch_cfg.thermalDecimal, sizeof(int32));

    return RT_ERR_OK;
}   /* end of rtk_switch_thermal_get */


