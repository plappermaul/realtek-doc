/*
 * Copyright(c) Realtek Semiconductor Corporation, 2008
 * All rights reserved.
 *
 * $Revision: 83205 $
 * $Date: 2017-11-07 19:41:42 +0800 (Tue, 07 Nov 2017) $
 *
 * Purpose : Realtek Switch SDK rtrpc API Module
 *
 * Feature : The file have include the following module and sub-modules
 *           1) trunk
 *
 */

#include <rtk/stack.h>
#include <dal/rtrpc/rtrpc_stack.h>
#include <dal/rtrpc/rtrpc_msg.h>
#include <rtdrv/rtdrv_netfilter.h>


int32
rtrpc_stack_port_get (uint32 unit, rtk_portmask_t *pStkPorts)
{
    rtdrv_stackCfg_t stack_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pStkPorts), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&stack_cfg, 0, sizeof(stack_cfg));
    osal_memcpy(&stack_cfg.unit, &unit, sizeof(uint32));
    GETSOCKOPT(RTDRV_STACK_PORT_GET, &stack_cfg, rtdrv_stackCfg_t, 1);
    osal_memcpy(pStkPorts, &stack_cfg.stkPorts, sizeof(rtk_portmask_t));

    return RT_ERR_OK;
}   /* end of rtk_stack_port_get */

int32
rtrpc_stack_port_set (uint32 unit, rtk_portmask_t *pStkPorts)
{
    rtdrv_stackCfg_t stack_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&stack_cfg, 0, sizeof(stack_cfg));
    osal_memcpy(&stack_cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&stack_cfg.stkPorts, pStkPorts, sizeof(rtk_portmask_t));
    SETSOCKOPT(RTDRV_STACK_PORT_SET, &stack_cfg, rtdrv_stackCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_stack_port_set */

int32
rtrpc_stack_devId_get(uint32 unit, uint32 *pMyDevID)
{
    rtdrv_stackCfg_t stack_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pMyDevID), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&stack_cfg, 0, sizeof(stack_cfg));
    osal_memcpy(&stack_cfg.unit, &unit, sizeof(uint32));
    GETSOCKOPT(RTDRV_STACK_UNIT_GET, &stack_cfg, rtdrv_stackCfg_t, 1);
    osal_memcpy(pMyDevID, &stack_cfg.myDevID, sizeof(uint32));

    return RT_ERR_OK;
}   /* end of rtk_stack_devId_get */

int32
rtrpc_stack_devId_set(uint32 unit, uint32 myDevID)
{
    rtdrv_stackCfg_t stack_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&stack_cfg, 0, sizeof(stack_cfg));
    osal_memcpy(&stack_cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&stack_cfg.myDevID, &myDevID, sizeof(uint32));
    SETSOCKOPT(RTDRV_STACK_UNIT_SET, &stack_cfg, rtdrv_stackCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_stack_devId_set */

int32
rtrpc_stack_masterDevId_get(uint32 unit, uint32 *pMasterDevID)
{
    rtdrv_stackCfg_t stack_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pMasterDevID), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&stack_cfg, 0, sizeof(stack_cfg));
    osal_memcpy(&stack_cfg.unit, &unit, sizeof(uint32));
    GETSOCKOPT(RTDRV_STACK_MASTERUNIT_GET, &stack_cfg, rtdrv_stackCfg_t, 1);
    osal_memcpy(pMasterDevID, &stack_cfg.masterDevID, sizeof(uint32));

    return RT_ERR_OK;
}   /* end of rtk_stack_masterDevId_get */

int32
rtrpc_stack_masterDevId_set(uint32 unit, uint32 masterDevID)
{
    rtdrv_stackCfg_t stack_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&stack_cfg, 0, sizeof(stack_cfg));
    osal_memcpy(&stack_cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&stack_cfg.masterDevID, &masterDevID, sizeof(uint32));
    SETSOCKOPT(RTDRV_STACK_MASTERUNIT_SET, &stack_cfg, rtdrv_stackCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_stack_masterDevId_set */

int32
rtrpc_stack_loopGuard_get(uint32 unit, rtk_enable_t *pEnable)
{
    rtdrv_stackCfg_t stack_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&stack_cfg, 0, sizeof(stack_cfg));
    osal_memcpy(&stack_cfg.unit, &unit, sizeof(uint32));
    GETSOCKOPT(RTDRV_STACK_LOOPGUARD_GET, &stack_cfg, rtdrv_stackCfg_t, 1);
    osal_memcpy(pEnable, &stack_cfg.enable, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_stack_loopGuard_get */

int32
rtrpc_stack_loopGuard_set(uint32 unit, rtk_enable_t enable)
{
    rtdrv_stackCfg_t stack_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&stack_cfg, 0, sizeof(stack_cfg));
    osal_memcpy(&stack_cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&stack_cfg.enable, &enable, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_STACK_LOOPGUARD_SET, &stack_cfg, rtdrv_stackCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_stack_loopGuard_set */

int32
rtrpc_stack_devPortMap_get (uint32 unit, uint32 dstDevID, rtk_portmask_t *pStkPorts)
{
    rtdrv_stackCfg_t stack_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pStkPorts), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&stack_cfg, 0, sizeof(stack_cfg));
    osal_memcpy(&stack_cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&stack_cfg.dstDevID, &dstDevID, sizeof(uint32));
    GETSOCKOPT(RTDRV_STACK_UNITPORTMAP_GET, &stack_cfg, rtdrv_stackCfg_t, 1);
    osal_memcpy(pStkPorts, &stack_cfg.stkPorts, sizeof(rtk_portmask_t));

    return RT_ERR_OK;
}   /* end of rtk_stack_devPortMap_get */

int32
rtrpc_stack_devPortMap_set (uint32 unit, uint32 dstDevID, rtk_portmask_t *pStkPorts)
{
    rtdrv_stackCfg_t stack_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&stack_cfg, 0, sizeof(stack_cfg));
    osal_memcpy(&stack_cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&stack_cfg.dstDevID, &dstDevID, sizeof(uint32));
    osal_memcpy(&stack_cfg.stkPorts, pStkPorts, sizeof(rtk_portmask_t));
    SETSOCKOPT(RTDRV_STACK_UNITPORTMAP_SET, &stack_cfg, rtdrv_stackCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_stack_devPortMap_set */

int32
rtrpc_stack_nonUcastBlockPort_get (uint32 unit, uint32 srcDevID, rtk_portmask_t *pBlockStkPorts)
{
    rtdrv_stackCfg_t stack_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pBlockStkPorts), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&stack_cfg, 0, sizeof(stack_cfg));
    stack_cfg.unit = unit;
    stack_cfg.srcDevID = srcDevID;
    GETSOCKOPT(RTDRV_STACK_NONUCASTBLOCKPORT_GET, &stack_cfg, rtdrv_stackCfg_t, 1);
    osal_memcpy(pBlockStkPorts, &stack_cfg.stkPorts, sizeof(rtk_portmask_t));

    return RT_ERR_OK;
}   /* end of rtk_stack_nonUcastBlockPort_get */

int32
rtrpc_stack_nonUcastBlockPort_set (uint32 unit, uint32 srcDevID, rtk_portmask_t *pBlockStkPorts)
{
    rtdrv_stackCfg_t stack_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&stack_cfg, 0, sizeof(stack_cfg));
    stack_cfg.unit = unit;
    stack_cfg.srcDevID = srcDevID;
    osal_memcpy(&stack_cfg.stkPorts, pBlockStkPorts, sizeof(rtk_portmask_t));
    SETSOCKOPT(RTDRV_STACK_NONUCASTBLOCKPORT_SET, &stack_cfg, rtdrv_stackCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_stack_nonUcastBlockPort_set */

/* Function Name:
 *      rtk_stack_rmtIntrTxEnable_get
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
int32
rtrpc_stack_rmtIntrTxEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    rtdrv_stackCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(cfg));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    GETSOCKOPT(RTDRV_STACK_RMTINTRTXENABLE_GET, &cfg, rtdrv_stackCfg_t, 1);
    osal_memcpy(pEnable, &cfg.enable, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_stack_rmtIntrTxEnable_get */

/* Function Name:
 *      rtk_stack_rmtIntrTxEnable_set
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
int32
rtrpc_stack_rmtIntrTxEnable_set(uint32 unit, rtk_enable_t enable)
{
    rtdrv_stackCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(cfg));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.enable, &enable, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_STACK_RMTINTRTXENABLE_SET, &cfg, rtdrv_stackCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_stack_rmtIntrTxEnable_set */

/* Function Name:
 *      rtk_stack_rmtIntrTxTriggerEnable_get
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
int32
rtrpc_stack_rmtIntrTxTriggerEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    rtdrv_stackCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(cfg));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    GETSOCKOPT(RTDRV_STACK_RMTINTRTXTRIGGERENABLE_GET, &cfg, rtdrv_stackCfg_t, 1);
    osal_memcpy(pEnable, &cfg.enable, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_stack_rmtIntrTxTriggerEnable_get */

/* Function Name:
 *      rtk_stack_rmtIntrTxTriggerEnable_set
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
int32
rtrpc_stack_rmtIntrTxTriggerEnable_set(uint32 unit, rtk_enable_t enable)
{
    rtdrv_stackCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(cfg));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.enable, &enable, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_STACK_RMTINTRTXTRIGGERENABLE_SET, &cfg, rtdrv_stackCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_stack_rmtIntrTxTriggerEnable_set */

/* Function Name:
 *      rtk_stack_rmtIntrRxSeqCmpMargin_get
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
int32
rtrpc_stack_rmtIntrRxSeqCmpMargin_get(uint32 unit, int32 *pMargin)
{
    rtdrv_stackCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pMargin), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(cfg));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    GETSOCKOPT(RTDRV_STACK_RMTINTRRXSEQCMPMARGIN_GET, &cfg, rtdrv_stackCfg_t, 1);
    osal_memcpy(pMargin, &cfg.margin, sizeof(int32));

    return RT_ERR_OK;
}   /* end of rtk_stack_rmtIntrRxSeqCmpMargin_get */

/* Function Name:
 *      rtk_stack_rmtIntrRxSeqCmpMargin_set
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
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
rtrpc_stack_rmtIntrRxSeqCmpMargin_set(uint32 unit, int32 margin)
{
    rtdrv_stackCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(cfg));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.margin, &margin, sizeof(int32));
    SETSOCKOPT(RTDRV_STACK_RMTINTRRXSEQCMPMARGIN_SET, &cfg, rtdrv_stackCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_stack_rmtIntrRxSeqCmpMargin_set */

/* Function Name:
 *      rtk_stack_rmtIntrRxForceUpdateEnable_get
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
int32
rtrpc_stack_rmtIntrRxForceUpdateEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    rtdrv_stackCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(cfg));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    GETSOCKOPT(RTDRV_STACK_RMTINTRRXFORCEUPDATEENABLE_GET, &cfg, rtdrv_stackCfg_t, 1);
    osal_memcpy(pEnable, &cfg.enable, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_stack_rmtIntrRxForceUpdateEnable_get */

/* Function Name:
 *      rtk_stack_rmtIntrRxForceUpdateEnable_set
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
int32
rtrpc_stack_rmtIntrRxForceUpdateEnable_set(uint32 unit, rtk_enable_t enable)
{
    rtdrv_stackCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(cfg));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.enable, &enable, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_STACK_RMTINTRRXFORCEUPDATEENABLE_SET, &cfg, rtdrv_stackCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_stack_rmtIntrRxForceUpdateEnable_set */

/* Function Name:
 *      rtk_stack_rmtIntrInfo_get
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
int32
rtrpc_stack_rmtIntrInfo_get(uint32 unit, rtk_stack_rmtIntrInfo_t *pInfo)
{
    rtdrv_stackCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(cfg));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.info, pInfo, sizeof(rtk_stack_rmtIntrInfo_t));
    GETSOCKOPT(RTDRV_STACK_RMTINTRINFO_GET, &cfg, rtdrv_stackCfg_t, 1);
    osal_memcpy(pInfo, &cfg.info, sizeof(rtk_stack_rmtIntrInfo_t));

    return RT_ERR_OK;
}   /* end of rtk_stack_rmtIntrInfo_get */

int32
rtrpc_stack_shrink_get(uint32 unit, rtk_stack_shrinkCtrlType_t shrinkType, uint32 *pVal)
{
    rtdrv_stackCfg_t stack_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pVal), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&stack_cfg, 0, sizeof(stack_cfg));
    osal_memcpy(&stack_cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&stack_cfg.shrinkType, &shrinkType, sizeof(rtk_stack_shrinkCtrlType_t));
    GETSOCKOPT(RTDRV_STACK_SHRINK_GET, &stack_cfg, rtdrv_stackCfg_t, 1);
    osal_memcpy(pVal, &stack_cfg.val, sizeof(uint32));

    return RT_ERR_OK;
}   /* end of rtrpc_stack_shrink_get */

int32
rtrpc_stack_shrink_set(uint32 unit, rtk_stack_shrinkCtrlType_t shrinkType, uint32 val)
{
    rtdrv_stackCfg_t stack_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&stack_cfg, 0, sizeof(stack_cfg));
    osal_memcpy(&stack_cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&stack_cfg.shrinkType, &shrinkType, sizeof(rtk_stack_shrinkCtrlType_t));
    osal_memcpy(&stack_cfg.val, &val, sizeof(uint32));
    SETSOCKOPT(RTDRV_STACK_SHRINK_SET, &stack_cfg, rtdrv_stackCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtrpc_stack_shrink_set */


int32
rtrpc_stackMapper_init(dal_mapper_t *pMapper)
{
    pMapper->stack_port_get = rtrpc_stack_port_get ;
    pMapper->stack_port_set = rtrpc_stack_port_set ;
    pMapper->stack_devId_get = rtrpc_stack_devId_get;
    pMapper->stack_devId_set = rtrpc_stack_devId_set;
    pMapper->stack_masterDevId_get = rtrpc_stack_masterDevId_get;
    pMapper->stack_masterDevId_set = rtrpc_stack_masterDevId_set;
    pMapper->stack_loopGuard_get = rtrpc_stack_loopGuard_get;
    pMapper->stack_loopGuard_set = rtrpc_stack_loopGuard_set;
    pMapper->stack_devPortMap_get = rtrpc_stack_devPortMap_get ;
    pMapper->stack_devPortMap_set = rtrpc_stack_devPortMap_set ;
    pMapper->stack_nonUcastBlockPort_get = rtrpc_stack_nonUcastBlockPort_get ;
    pMapper->stack_nonUcastBlockPort_set = rtrpc_stack_nonUcastBlockPort_set ;
    pMapper->stack_rmtIntrTxEnable_get = rtrpc_stack_rmtIntrTxEnable_get;
    pMapper->stack_rmtIntrTxEnable_set = rtrpc_stack_rmtIntrTxEnable_set;
    pMapper->stack_rmtIntrTxTriggerEnable_get = rtrpc_stack_rmtIntrTxTriggerEnable_get;
    pMapper->stack_rmtIntrTxTriggerEnable_set = rtrpc_stack_rmtIntrTxTriggerEnable_set;
    pMapper->stack_rmtIntrRxSeqCmpMargin_get = rtrpc_stack_rmtIntrRxSeqCmpMargin_get;
    pMapper->stack_rmtIntrRxSeqCmpMargin_set = rtrpc_stack_rmtIntrRxSeqCmpMargin_set;
    pMapper->stack_rmtIntrRxForceUpdateEnable_get = rtrpc_stack_rmtIntrRxForceUpdateEnable_get;
    pMapper->stack_rmtIntrRxForceUpdateEnable_set = rtrpc_stack_rmtIntrRxForceUpdateEnable_set;
    pMapper->stack_shrink_get = rtrpc_stack_shrink_get;
    pMapper->stack_shrink_set = rtrpc_stack_shrink_set;
    pMapper->stack_rmtIntrInfo_get = rtrpc_stack_rmtIntrInfo_get;
    return RT_ERR_OK;
}
