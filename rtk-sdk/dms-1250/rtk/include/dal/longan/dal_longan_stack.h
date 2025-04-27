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
#ifndef __DAL_LONGAN_STACK_H__
#define __DAL_LONGAN_STACK_H__

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

/*
 * Macro Definition
 */

/*
 * Function Declaration
 */

/* Module Name : STACK */

extern int32
_dal_longan_stack_port_del (uint32 unit, rtk_port_t port);
extern int32
_dal_longan_stack_port_add (uint32 unit, rtk_port_t port);
/* Function Name:
 *      dal_longan_stackMapper_init
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
dal_longan_stackMapper_init(dal_mapper_t *pMapper);

/* Function Name:
 *      dal_longan_stack_init
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
dal_longan_stack_init(uint32 unit);

/* Function Name:
 *      dal_longan_stack_cascadeMode_init
 * Description:
 *      Initialize cascade mode.
 * Input:
 *      unit  - unit id
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
dal_longan_stack_cascadeMode_init(uint32 unit);


/* Module Name    : Trunk                    */
/* Sub-module Name: User configuration stack */

/* Function Name:
 *      dal_longan_stack_port_get
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
dal_longan_stack_port_get (uint32 unit, rtk_portmask_t *pStkPorts);


/* Function Name:
 *      dal_longan_stack_port_set
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
 *      RT_ERR_NOT_INIT - The module is not initial
 * Note:
 *      In 9300, only port 24~27 can be configured as stacking ports.
 */
extern int32
dal_longan_stack_port_set (uint32 unit, rtk_portmask_t *pStkPorts);

/* Function Name:
 *      dal_longan_stack_devId_get
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
 * Changes:
 *      [3.2.0]
 *          New added function.
 */
extern int32
dal_longan_stack_devId_get(uint32 unit, uint32 *pMyDevID);

/* Function Name:
 *      dal_longan_stack_devId_set
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
 *      (1) 9300 supports 16 stacked devices, thus myDevID ranges from 0 to 15.
 * Changes:
 *      [3.2.0]
 *          New added function.
 */
extern int32
dal_longan_stack_devId_set(uint32 unit, uint32 myDevID);

/* Function Name:
 *      dal_longan_stack_masterDevId_get
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
 * Changes:
 *      [3.2.0]
 *          New added function.
 */
extern int32
dal_longan_stack_masterDevId_get(uint32 unit, uint32 *pMasterDevID);

/* Function Name:
 *      dal_longan_stack_masterDevId_set
 * Description:
 *      Set the master device ID to the specified device.
 * Input:
 *      unit                   - unit id
 *      masterDevID         - device ID of the master switch
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID           - invalid unit id
 *      RT_ERR_NOT_INIT          - The module is not initial
 * Note:
 *      (1) 9300 supports 16 stacked units, thus masterDevID ranges from 0 to 15.
 * Changes:
 *      [3.2.0]
 *          New added function.
 */
extern int32
dal_longan_stack_masterDevId_set(uint32 unit, uint32 masterDevID);

/* Function Name:
 *      dal_longan_stack_loopGuard_get
 * Description:
 *      Get the enable status of dropping packets with source unit ID the same as the unit ID of the switch from the specified device.
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
dal_longan_stack_loopGuard_get(uint32 unit, rtk_enable_t *pEnable);


/* Function Name:
 *      dal_longan_stack_loopGuard_set
 * Description:
 *      Set the enable status of dropping packets with source unit ID the same as the unit ID of the switch to the specified device.
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
dal_longan_stack_loopGuard_set(uint32 unit, rtk_enable_t enable);

/* Function Name:
 *      dal_longan_stack_devPortMap_get
 * Description:
 *      Get the stacking ports that packets with specific target unit should forward to from the specified device.
 * Input:
 *      unit                   - unit id
 *      dstDevID                - device ID of forwarding target
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
dal_longan_stack_devPortMap_get (uint32 unit, uint32 dstDevID, rtk_portmask_t *pStkPorts);

/* Function Name:
 *      dal_longan_stack_devPortMap_set _set
 * Description:
 *      Set the stacking ports that packets with specific target unit should forward to for the specified device.
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
 *      (1) Stacking ports in 9300 ranges from 24 to 27.
 */
extern int32
dal_longan_stack_devPortMap_set (uint32 unit, uint32 dstDevID, rtk_portmask_t *pStkPorts);

/*
 * Function Declaration
 *      dal_longan_stack_nonUcastBlockPort_get
 * Description:
 *      Get the stacking ports that would block ingress and egress non-unicast packets from the specified device.
 * Input:
 *      unit                   - unit id
 *      srcDevID               - source device id
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
dal_longan_stack_nonUcastBlockPort_get (uint32 unit, uint32 srcDevID, rtk_portmask_t *pBlockStkPorts);

/* Function Name:
 *      dal_longan_stack_nonUcastBlockPort_set
 * Description:
 *      Set the stacking ports that would block ingress and egress non-ucast packets to the specified device.
 * Input:
 *      unit                   - unit id
 *      srcDevID               - source device id
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
 *      (1) Blockable stacking ports in 9300 ranges from 24 to 27.
 */
extern int32
dal_longan_stack_nonUcastBlockPort_set (uint32 unit, uint32 srcDevID, rtk_portmask_t *pBlockStkPorts);


/* Function Name:
 *      dal_longan_stack_shrink_get
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
dal_longan_stack_shrink_get(uint32 unit, rtk_stack_shrinkCtrlType_t shrinkType, uint32 *pVal);

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
dal_longan_stack_shrink_set(uint32 unit, rtk_stack_shrinkCtrlType_t shrinkType, uint32 val);

#endif /* __DAL_LONGAN_STACK_H__ */


