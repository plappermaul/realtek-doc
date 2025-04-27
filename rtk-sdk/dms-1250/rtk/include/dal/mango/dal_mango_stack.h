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
 * Purpose : Definition those public STACK APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) Stacking
 *
 */
#ifndef __DAL_MANGO_STACK_H__
#define __DAL_MANGO_STACK_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <rtk/stack.h>
#include <dal/dal_mapper.h>


/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
extern uint32                            stackPortId2Idx[RTK_MAX_NUM_OF_UNIT][RTK_MAX_PORT_PER_UNIT];
extern rtk_port_t                       *idx2stackPortId[RTK_MAX_NUM_OF_UNIT];

/*
 * Macro Definition
 */


/*
 * Function Declaration
 */

/* Module Name : STACK */


/* Function Name:
 *      dal_mango_stackMapper_init
 * Description:
 *      Hook stack module of the specified device.
 * Input:
 *      pMapper - pointer of mapper
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Must Hook stack module before calling any stack APIs.
 */
extern int32
dal_mango_stackMapper_init(dal_mapper_t *pMapper);

/* Function Name:
 *      dal_mango_stack_init
 * Description:
 *      Initialize stack module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      Must initialize stack module before calling any stack APIs.
 */
extern int32
dal_mango_stack_init(uint32 unit);

/* Module Name    : STACK                    */
/* Sub-module Name: User configuration stack */

/* Function Name:
 *      dal_mango_stack_port_get
 * Description:
 *      Get the stacking port from the specified device.
 * Input:
 *      unit  - unit id
 * Output:
 *      pStkPorts - pointer buffer of stacking ports
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_stack_port_get (uint32 unit, rtk_portmask_t *pStkPorts);

/* Function Name:
 *      dal_mango_stack_port_set
 * Description:
 *      Set stacking ports to the specified device.
 * Input:
 *      unit - unit id
 *      pStkPorts - pointer buffer of stacking ports
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_STACK_PORT_RX_EN - MAC RX disabled serdes port can't be configured as stacking port
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      In 9310, any port can be configured as stacking ports.
 */
extern int32
dal_mango_stack_port_set (uint32 unit, rtk_portmask_t *pStkPorts);

/* Function Name:
 *      dal_mango_stack_devId_get
 * Description:
 *      Get the switch device ID from the specified device.
 * Input:
 *      unit                   - unit id
 * Output:
 *      pMyDevID              - pointer buffer of device ID of the switch
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_stack_devId_get(uint32 unit, uint32 *pMyDevID);

/* Function Name:
 *      dal_mango_stack_devId_set
 * Description:
 *      Set the switch device ID to the specified device.
 * Input:
 *      unit                   - unit id
 *      myDevID              - device ID of the switch
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID           - invalid unit id
 *      RT_ERR_NOT_INIT          - The module is not initial
 * Note:
 *      (1) 9310 supports 16 stacked devices, thus myDevID ranges from 0 to 15.
 */
extern int32
dal_mango_stack_devId_set(uint32 unit, uint32 myDevID);

/* Function Name:
 *      dal_mango_stack_masterDevId_get
 * Description:
 *      Get the master device ID from the specified device.
 * Input:
 *      unit                   - unit id
 * Output:
 *      pMasterDevID        - pointer buffer of device ID of the master switch
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_stack_masterDevId_get(uint32 unit, uint32 *pMasterDevID);

/* Function Name:
 *      dal_mango_stack_masterDevID_set
 * Description:
 *      Set the master device ID to the specified device.
 * Input:
 *      unit                   - unit id
 *      masterUnit         - device ID of the master switch
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID           - invalid unit id
 *      RT_ERR_NOT_INIT          - The module is not initial
 * Note:
 *      (1) 9310 supports 16 stacked devices, thus masterDevID ranges from 0 to 15.
 */
extern int32
dal_mango_stack_masterDevId_set(uint32 unit, uint32 masterDevID);

/* Function Name:
 *      dal_mango_stack_loopGuard_get
 * Description:
 *      Get the enable status of dropping packets with source device ID the same as the device ID of the switch from the specified device.
 * Input:
 *      unit          - unit id
 * Output:
 *      pEnable     - pointer buffer of enable state of loop guard mechanism
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_stack_loopGuard_get(uint32 unit, rtk_enable_t *pEnable);


/* Function Name:
 *      dal_mango_stack_loopGuard_set
 * Description:
 *      Set the enable status of dropping packets with source device ID the same as the device ID of the switch to the specified device.
 * Input:
 *      unit          - unit id
 *      enable - enable state of loop guard mechanism
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_stack_loopGuard_set(uint32 unit, rtk_enable_t enable);

extern int32
_dal_mango_stack_devPortMap_get (uint32 unit, uint32 dstDevID, rtk_portmask_t *pStkPorts);

/* Function Name:
 *      dal_mango_stack_devPortMap_get
 * Description:
 *      Get the stacking ports that packets with specific target device should forward to from the specified device.
 * Input:
 *      unit                   - unit id
 *      dstUnit                - unid ID of forwarding target
 * Output:
 *      pStkPorts           - pointer buffer of egress stacking ports
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_stack_devPortMap_get (uint32 unit, uint32 dstDevID, rtk_portmask_t *pStkPorts);

extern int32
_dal_mango_stack_devPortMap_set (uint32 unit, uint32 dstDevID, rtk_portmask_t *pStkPorts);

/* Function Name:
 *      dal_mango_stack_devPortMap_set _set
 * Description:
 *      Set the stacking ports that packets with specific target device should forward to for the specified device.
 * Input:
 *      unit                   - unit id
 *      dstDevID              - device ID of forwarding target
 *      pStkPorts           - pointer buffer of egress stacking ports
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) Stacking ports in 9310 ranges from 0 to 55.
 */
extern int32
dal_mango_stack_devPortMap_set (uint32 unit, uint32 dstDevID, rtk_portmask_t *pStkPorts);

extern int32
_dal_mango_stack_nonUcastBlockPort_get (uint32 unit, uint32 srcDevID, rtk_portmask_t *pBlockStkPorts);

/* Function Name:
 *      dal_mango_stack_nonUcastBlockPort_get
 * Description:
 *      Get the stacking ports that would block ingress and egress non-unicast packets from the specified device.
 * Input:
 *      unit                   - unit id
 *      srcDevID            - source device id
 * Output:
 *      pBlockStkPorts    - pointer buffer of blocked stacking ports
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_stack_nonUcastBlockPort_get (uint32 unit, uint32 srcDevID, rtk_portmask_t *pBlockStkPorts);

extern int32
_dal_mango_stack_nonUcastBlockPort_set (uint32 unit, uint32 srcDevID, rtk_portmask_t *pBlockStkPorts);

/* Function Name:
 *      dal_mango_stack_nonUcastBlockPort_set
 * Description:
 *      Set the stacking ports that would block ingress and egress non-ucast packets to the specified device.
 * Input:
 *      unit                   - unit id
 *      srcDevID            - source device id
 *      pBlockStkPorts    - pointer buffer of blocked stacking ports
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) Blockable stacking ports in 9310 ranges from 0 to 55.
 */
extern int32
dal_mango_stack_nonUcastBlockPort_set (uint32 unit, uint32 srcDevID, rtk_portmask_t *pBlockStkPorts);

/* Function Name:
 *      dal_mango_stack_linkUp_handler_pre
 * Description:
 *      Stacking header description error prevent:
 *      1. reset stacking port if ever link changes to fix stacking description error
 *      2. enable RX_EN to avoid stacking description error
 *      3. disable serdes RX to avoid fragment packet received
 * Input:
 *      unit           - unit id
 *      port           - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32 dal_mango_stack_linkUp_handler_pre(int32 unit, rtk_port_t port);

/* Function Name:
 *      dal_mango_stack_linkUp_handler_post
 * Description:
 *      Avoid stacking header description error:
 *      1. enable serdes RX after MAC force down released to avoid a fragment packet is received when MAC force link is released
 * Input:
 *      unit           - unit id
 *      port           - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32 dal_mango_stack_linkUp_handler_post(int32 unit, rtk_port_t port);

/* Function Name:
 *      dal_mango_stack_rmtIntrTxEnable_get
 * Description:
 *      Get enable status of Remote Interrupt Notification transmission.
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - pointer to enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_stack_rmtIntrTxEnable_get(uint32 unit, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_mango_stack_rmtIntrTxEnable_set
 * Description:
 *      Set enable status of Remote Interrupt Notification transmission.
 * Input:
 *      unit   - unit id
 *      enable - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_mango_stack_rmtIntrTxEnable_set(uint32 unit, rtk_enable_t enable);

/* Function Name:
 *      dal_mango_stack_rmtIntrTxTriggerEnable_get
 * Description:
 *      Get enable status of Remote Interrupt Notification transmission trigger.
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - pointer to enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_stack_rmtIntrTxTriggerEnable_get(uint32 unit, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_mango_stack_rmtIntrTxTriggerEnable_set
 * Description:
 *      Set enable status of Remote Interrupt Notification transmission trigger.
 * Input:
 *      unit   - unit id
 *      enable - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      The enable status will be clear automatically once the transmission has been done.
 */
extern int32
dal_mango_stack_rmtIntrTxTriggerEnable_set(uint32 unit, rtk_enable_t enable);

/* Function Name:
 *      dal_mango_stack_rmtIntrRxSeqCmpMargin_get
 * Description:
 *      Get the comparing margin of the sequence ID of receiving Remote Interrupt Notification.
 * Input:
 *      unit    - unit id
 * Output:
 *      pMargin - pointer to margin value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_stack_rmtIntrRxSeqCmpMargin_get(uint32 unit, int32 *pMargin);

/* Function Name:
 *      dal_mango_stack_rmtIntrRxSeqCmpMargin_set
 * Description:
 *      Set the comparing margin of the sequence ID of receiving Remote Interrupt Notification.
 * Input:
 *      unit   - unit id
 *      margin - margin value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      None
 */
extern int32
dal_mango_stack_rmtIntrRxSeqCmpMargin_set(uint32 unit, int32 margin);

/* Function Name:
 *      dal_mango_stack_rmtIntrRxForceUpdateEnable_get
 * Description:
 *      Get the force enable status of updating when receives a Remote Interrupt Notification.
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - pointer to enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_stack_rmtIntrRxForceUpdateEnable_get(uint32 unit, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_mango_stack_rmtIntrRxForceUpdateEnable_set
 * Description:
 *      Set the force enable status of updating when receives a Remote Interrupt Notification.
 * Input:
 *      unit   - unit id
 *      enable - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      The enable status will be clear automatically once the updating has been done.
 */
extern int32
dal_mango_stack_rmtIntrRxForceUpdateEnable_set(uint32 unit, rtk_enable_t enable);

/* Function Name:
 *      dal_mango_stack_rmtIntrInfo_get
 * Description:
 *      Get the information about Remote Interrupt Notification.
 * Input:
 *      unit  - unit id
 * Output:
 *      pInfo - pointer to information
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_stack_rmtIntrInfo_get(uint32 unit, rtk_stack_rmtIntrInfo_t *pInfo);


/* Function Name:
 *      dal_mango_stack_shrink_get
 * Description:
 *      Get the enable status of shrink type for stacking
 * Input:
 *      unit          - unit id
 *      shrinkType    - control type
 * Output:
 *      pVal          - status of shrink type
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_mango_stack_shrink_get(uint32 unit, rtk_stack_shrinkCtrlType_t shrinkType, uint32 *pVal);

/* Function Name:
 *      dal_mango_stack_shrink_set
 * Description:
 *      Set the enable status of shrink type for stacking
 * Input:
 *      unit        - unit id
 *      shrinkType  - control type
 *      val         - status of shrink type
 * Output:
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_mango_stack_shrink_set(uint32 unit, rtk_stack_shrinkCtrlType_t shrinkType, uint32 val);


#endif /* __DAL_MANGO_STACK_H__ */


