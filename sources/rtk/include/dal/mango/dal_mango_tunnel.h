/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Definition those public tunneling APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *           (1) Tunneling APIs
 *
 */
#ifndef __DAL_MANGO_TUNNEL_H__
#define __DAL_MANGO_TUNNEL_H__


/*
 * Include Files
 */
#include <dal/dal_mapper.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */

/*
 * Macro Declaration
 */
#define DAL_MANGO_TUNNEL_ENTRY_IDX_TO_ADDR(_idx)        (((_idx)/3) + (((_idx)%3)*128))
#define DAL_MANGO_TUNNEL_ENTRY_ADDR_TO_IDX(_addr)       (((_addr)/128) + (((_addr)%128)*3))

/*
 * Function Declaration
 */

/* Function Name:
 *      dal_mango_tunnelMapper_init
 * Description:
 *      Hook tunnel module of the specified device.
 * Input:
 *      pMapper - pointer of mapper
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Must Hook tunnel module before calling any tunnel APIs.
 */
extern int32
dal_mango_tunnelMapper_init(dal_mapper_t *pMapper);

/* Function Name:
 *      dal_mango_tunnel_init
 * Description:
 *      Initialize Tunnel module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      Must initialize Tunnel module before calling any tunneling APIs.
 */
extern int32
dal_mango_tunnel_init(uint32 unit);

/* Function Name:
 *      _dal_mango_tunnel_intf2tunnelIdx
 * Description:
 *      Get the tunnel index of an interface ID.
 * Input:
 *      unit   - unit id
 *      pIndex - pointer to the tunnel idx
 *      intfId - interface id
 * Output:
 *      pIndex - pointer to the tunnel idx
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NULL_POINTER   - null pointer
 *      RT_ERR_ENTRY_NOTFOUND - entry is not found
 * Note:
 *      Must initialize Tunnel module before calling any Tunnel APIs.
 */
extern int32
_dal_mango_tunnel_intf2tunnelIdx(uint32 unit, uint32 *pIndex, rtk_intf_id_t intfId);

/* Function Name:
 *      _dal_mango_tunnel_tunnelIdx2intf
 * Description:
 *      Get the interface ID of a tunnel index.
 * Input:
 *      unit      - unit id
 *      pIntfId   - pointer to the interface id
 *      tunnelIdx - tunnel idx
 * Output:
 *      pIntfId   - pointer to the tunnel idx
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NULL_POINTER   - null pointer
 *      RT_ERR_ENTRY_NOTFOUND - entry is not found
 * Note:
 *      Must initialize Tunnel module before calling any Tunnel APIs.
 */
extern int32
_dal_mango_tunnel_tunnelIdx2intf(uint32 unit, rtk_intf_id_t *pIntfId, uint32 tunnelIdx);

/* Module Name    : Tunneling                */
/* Sub-module Name: Tunneling error handling */

/* Function Name:
 *      dal_mango_tunnel_info_t_init
 * Description:
 *      Initialize a rtk_tunnel_info_t_init structure
 * Input:
 *      unit  - unit id
 *      pInfo - pointer to tunnel information
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Note:
 */
extern int32
dal_mango_tunnel_info_t_init(rtk_tunnel_info_t *pInfo);

/* Function Name:
 *      dal_mango_tunnel_info_get
 * Description:
 *      Get tunnel-related information
 * Input:
 *      unit  - unit id
 *      pInfo - pointer to tunnel information
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Note:
 */
extern int32
dal_mango_tunnel_info_get(uint32 unit, rtk_tunnel_info_t *pInfo);

/* Function Name:
 *      dal_mango_tunnel_info_t_init
 * Description:
 *      Initialize a rtk_tunnel_info_t_init structure
 * Input:
 *      unit  - unit id
 *      pInfo - pointer to tunnel information
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Note:
 */
extern int32
dal_mango_tunnel_intf_t_init(rtk_tunnel_intf_t *pIntf);

/* Function Name:
 *      dal_mango_tunnel_intf_create
 * Description:
 *      Create a new tunnel interface
 * Input:
 *      unit  - unit id
 *      pIntf - pointer to tunnel interface containing the basic inputs
 * Output:
 *      pIntf - pointer to tunnel interface (including all info)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID                  - invalid unit id
 *      RT_ERR_NOT_INIT                 - the module is not initial
 *      RT_ERR_NULL_POINTER             - input parameter may be a null pointer
 *      RT_ERR_TBL_FULL                 - table is full
 *      RT_ERR_INTF_MTU_VARIETY_EXCEEDS - variety of MTU values is over the maximum H/W supports
 * Note:
 *      (1) Basic required input parameters of the pRoute as input:
 *          type and corresponding fields about that tunnel.
 */
extern int32
dal_mango_tunnel_intf_create(uint32 unit, rtk_tunnel_intf_t *pIntf);

/* Function Name:
 *      dal_mango_tunnel_intf_destroy
 * Description:
 *      Destroy a tunnel interface
 * Input:
 *      unit   - unit id
 *      intfId - tunnel interface ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NOT_INIT       - the module is not initial
 *      RT_ERR_ENTRY_NOTFOUND - entry is not found
 * Note:
 */
extern int32
dal_mango_tunnel_intf_destroy(uint32 unit, rtk_intf_id_t intfId);

/* Function Name:
 *      dal_mango_tunnel_intf_destroyAll
 * Description:
 *      Destroy all tunnel interfaces
 * Input:
 *      unit  - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Note:
 */
extern int32
dal_mango_tunnel_intf_destroyAll(uint32 unit);

/* Function Name:
 *      dal_mango_tunnel_intf_get
 * Description:
 *      Get a tunnel interface by interface ID.
 * Input:
 *      unit  - unit id
 *      pIntf - pointer to tunnel interface (interface id)
 * Output:
 *      pIntf - pointer to tunnel interface (all info)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_tunnel_intf_get(uint32 unit, rtk_tunnel_intf_t *pIntf);

/* Function Name:
 *      dal_mango_tunnel_intf_set
 * Description:
 *      Set a tunnel interface by interface ID.
 * Input:
 *      unit  - unit id
 *      pIntf - pointer to tunnel interface (interface id)
 * Output:
 *      pIntf - pointer to tunnel interface (all info)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID                  - invalid unit id
 *      RT_ERR_NOT_INIT                 - the module is not initial
 *      RT_ERR_NULL_POINTER             - input parameter may be a null pointer
 *      RT_ERR_INPUT                    - invalid input parameter
 *      RT_ERR_OUT_OF_RANGE             - input parameter out of range
 *      RT_ERR_INTF_MTU_VARIETY_EXCEEDS - variety of MTU values is over the maximum H/W supports
 * Note:
 *      (1) It cannot change the type of tunnel interface. (pIntf->type won't be referred.)
 *          Need to destroy it then create a new one.
 */
extern int32
dal_mango_tunnel_intf_set(uint32 unit, rtk_tunnel_intf_t *pIntf);

/* Function Name:
 *      dal_mango_tunnel_intfPathId_get
 * Description:
 *      Get the path ID of the specified tunnel interface
 * Input:
 *      unit    - unit id
 *      intfId  - tunnel interface id
 * Output:
 *      pPathId - pointer to the argurment
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_INPUT          - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_tunnel_intfPathId_get(uint32 unit, rtk_intf_id_t intfId, rtk_l3_pathId_t *pPathId);

/* Function Name:
 *      dal_mango_tunnel_intfPathId_set
 * Description:
 *      Set the path ID of the specified tunnel interface
 * Input:
 *      unit   - unit id
 *      intfId - tunnel interface id
 *      pathId - control type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT       - the module is not initial
 *      RT_ERR_INPUT          - invalid input parameter
 *      RT_ERR_OUT_OF_RANGE   - input parameter out of range
 *      RT_ERR_ENTRY_NOTFOUND - entry not found
 * Note:
 *      (1) 9310 only supports Nexthop-type path ID (not ECMP)
 */
extern int32
dal_mango_tunnel_intfPathId_set(uint32 unit, rtk_intf_id_t intfId, rtk_l3_pathId_t pathId);

/* Function Name:
 *      dal_mango_tunnel_intfPath_get
 * Description:
 *      Get the path information of the specified tunnel interface
 * Input:
 *      unit          - unit id
 *      intfId        - tunnel interface id
 * Output:
 *      pNhDmacIdx    - pointer to nexthop DMAC entry index
 *      pL3EgrIntfIdx - pointer to L3 egress interface index
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_tunnel_intfPath_get(uint32 unit, rtk_intf_id_t intfId, uint32 *pNhDmacIdx, uint32 *pL3EgrIntfIdx);

/* Function Name:
 *      dal_mango_tunnel_intfPath_set
 * Description:
 *      Set the path ID of the specified tunnel interface
 * Input:
 *      unit         - unit id
 *      intfId       - tunnel interface id
 *      nhDmacIdx    - nexthop DMAC entry index
 *      l3EgrIntfIdx - L3 egress interface index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT       - the module is not initial
 *      RT_ERR_INPUT          - invalid input parameter
 *      RT_ERR_OUT_OF_RANGE   - input parameter out of range
 *      RT_ERR_ENTRY_NOTFOUND - entry not found
 * Note:
 *      None
 */
extern int32
dal_mango_tunnel_intfPath_set(uint32 unit, rtk_intf_id_t intfId, uint32 nhDmacIdx, uint32 l3EgrIntfIdx);

/* Function Name:
 *      dal_mango_tunnel_intfStats_get
 * Description:
 *      Get statistic counters of the specified tunnel interface
 * Input:
 *      unit   - unit id
 *      intfId - tunnel interface id
 * Output:
 *      pStats - pointer to the statistic data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_NOT_INIT        - the module is not initial
 *      RT_ERR_NULL_POINTER    - input parameter may be a null pointer
 *      RT_ERR_NOT_TUNNEL_INTF - input interface type is not tunnel
 * Note:
 *      None
 */
extern int32
dal_mango_tunnel_intfStats_get(uint32 unit, rtk_intf_id_t intfId, rtk_tunnel_intf_stats_t *pStats);

/* Function Name:
 *      dal_mango_tunnel_intfStats_reset
 * Description:
 *      Reset statistic counters of the specified tunnel interface
 * Input:
 *      unit   - unit id
 *      intfId - tunnel interface id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_NOT_INIT        - the module is not initial
 *      RT_ERR_NULL_POINTER    - input parameter may be a null pointer
 *      RT_ERR_NOT_TUNNEL_INTF - input interface type is not tunnel
 * Note:
 *      None
 */
extern int32
dal_mango_tunnel_intfStats_reset(uint32 unit, rtk_intf_id_t intfId);

/* Function Name:
 *      dal_mango_tunnel_qosProfile_get
 * Description:
 *      Get the QoS profile with the specified index.
 * Input:
 *      unit     - unit id
 *      idx      - index of QoS profile
 * Output:
 *      pProfile - pointer to the QoS prifle
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Note:
 */
extern int32
dal_mango_tunnel_qosProfile_get(uint32 unit, uint32 idx, rtk_tunnel_qosProfile_t *pProfile);

/* Function Name:
 *      dal_mango_tunnel_qosProfile_set
 * Description:
 *      Set the QoS profile with the specified index.
 * Input:
 *      unit     - unit id
 *      idx      - index of QoS profile
 *      pProfile - pointer to the QoS prifle
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 */
extern int32
dal_mango_tunnel_qosProfile_set(uint32 unit, uint32 idx, rtk_tunnel_qosProfile_t profile);


/* Function Name:
 *      dal_mango_tunnel_globalCtrl_get
 * Description:
 *      Get the global configuration of the specified control type
 * Input:
 *      unit - unit id
 *      type - control type
 * Output:
 *      pArg - pointer to the argurment
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Note:
 */
extern int32
dal_mango_tunnel_globalCtrl_get(uint32 unit, rtk_tunnel_globalCtrlType_t type, int32 *pArg);

/* Function Name:
 *      dal_mango_tunnel_globalCtrl_set
 * Description:
 *      Set the global configuration of the specified control type
 * Input:
 *      unit - unit id
 *      type - control type
 *      arg  - argurment
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Note:
 */
extern int32
dal_mango_tunnel_globalCtrl_set(uint32 unit, rtk_tunnel_globalCtrlType_t type, int32 arg);

/* Function Name:
 *      dal_mango_capwap_udpPort_get
 * Description:
 *      Get UDP port number of CAPWAP control and data packet.
 * Input:
 *      unit	    - unit id
 * Output:
 *      pCtrl_port	- pointer to udp control port
 *      pData_port 	- pointer to udp data port
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT 	- The module is not initial
 *      RT_ERR_NULL_POINTER	- input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_capwap_udpPort_get(uint32 unit, uint32 *pCtrl_port, uint32 *pData_port);

/* Function Name:
 *      dal_mango_capwap_udpPort_set
 * Description:
 *      Set UDP port number of CAPWAP control and data packet.
 * Input:
 *      unit	    - unit id
 *      ctrl_port	- udp control port
 *      data_port 	- udp data port
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_mango_capwap_udpPort_set(uint32 unit, uint32 ctrl_port, uint32 data_port);

extern int32
_dal_mango_tunnel_tunnelIntfEntry_find(uint32 unit, rtk_intf_id_t intfId, uint32 *pIndex);

#endif /* __RTK_TUNNEL_H__ */

