
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
 * $Revision: 76138 $
 * $Date: 2017-03-03 17:14:11 +0800 (Fri, 03 Mar 2017) $
 *
 * Purpose : Definition those public global APIs and its data type in the SDK.
 *
 * Feature :  Parameter settings for the system-wise view
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_bitop.h>
#include <common/debug/rt_log.h>
#include <osal/sem.h>
#include <osal/lib.h>
#include <osal/memory.h>
#include <hal/chipdef/allmem.h>
#include <hal/chipdef/allreg.h>
#include <hal/chipdef/cypress/rtk_cypress_table_struct.h>
#include <hal/chipdef/cypress/rtk_cypress_reg_struct.h>
#include <hal/mac/reg.h>
#include <hal/mac/mem.h>
#include <hal/common/halctrl.h>
#include <dal/cypress/dal_cypress_switch.h>
#include <rtk/default.h>
#include <rtk/switch.h>
#include <dal/cypress/dal_cypress_port.h>
#include <dal/dal_mgmt.h>
#include <hal/common/miim.h>
#include <osal/time.h>

#include <ioal/mem32.h>

/*
 * Symbol Definition
 */
#define DAL_SERDES_NUM_MAX  14

/*
 * Data Declaration
 */
static uint32               switch_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};
static osal_mutex_t         switch_sem[RTK_MAX_NUM_OF_UNIT];

/*
 * Macro Definition
 */
/* trap semaphore handling */
#define SWITCH_SEM_LOCK(unit)    \
do {\
    if (osal_sem_mutex_take(switch_sem[unit], OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_LOCK_FAILED, (MOD_DAL|MOD_SWITCH), "semaphore lock failed");\
        return RT_ERR_SEM_LOCK_FAILED;\
    }\
} while(0)
#define SWITCH_SEM_UNLOCK(unit)   \
do {\
    if (osal_sem_mutex_give(switch_sem[unit]) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_UNLOCK_FAILED, (MOD_DAL|MOD_SWITCH), "semaphore unlock failed");\
        return RT_ERR_SEM_UNLOCK_FAILED;\
    }\
} while(0)

/*
 * Function Declaration
 */
/* Function Name:
 *      dal_cypress_switchMapper_init
 * Description:
 *      Hook switch module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must Hook switch module before calling any switch APIs.
 */
int32
dal_cypress_switchMapper_init(dal_mapper_t *pMapper)
{

    pMapper->switch_init = dal_cypress_switch_init;
    pMapper->switch_cpuMaxPktLen_get = dal_cypress_switch_cpuMaxPktLen_get;
    pMapper->switch_cpuMaxPktLen_set = dal_cypress_switch_cpuMaxPktLen_set;
    pMapper->switch_maxPktLenLinkSpeed_get = dal_cypress_switch_maxPktLenLinkSpeed_get;
    pMapper->switch_maxPktLenLinkSpeed_set = dal_cypress_switch_maxPktLenLinkSpeed_set;
    pMapper->switch_maxPktLenTagLenCntIncEnable_get = dal_cypress_switch_maxPktLenTagLenCntIncEnable_get;
    pMapper->switch_maxPktLenTagLenCntIncEnable_set = dal_cypress_switch_maxPktLenTagLenCntIncEnable_set;
    pMapper->switch_snapMode_get = dal_cypress_switch_snapMode_get;
    pMapper->switch_snapMode_set = dal_cypress_switch_snapMode_set;
    pMapper->switch_chksumFailAction_get = dal_cypress_switch_chksumFailAction_get;
    pMapper->switch_chksumFailAction_set = dal_cypress_switch_chksumFailAction_set;
    pMapper->switch_recalcCRCEnable_get = dal_cypress_switch_recalcCRCEnable_get;
    pMapper->switch_recalcCRCEnable_set = dal_cypress_switch_recalcCRCEnable_set;
    pMapper->switch_mgmtMacAddr_get = dal_cypress_switch_mgmtMacAddr_get;
    pMapper->switch_mgmtMacAddr_set = dal_cypress_switch_mgmtMacAddr_set;
    pMapper->switch_IPv4Addr_get = dal_cypress_switch_IPv4Addr_get;
    pMapper->switch_IPv4Addr_set = dal_cypress_switch_IPv4Addr_set;
    pMapper->switch_pppoeIpParseEnable_get = dal_cypress_switch_pppoeIpParseEnable_get;
    pMapper->switch_pppoeIpParseEnable_set = dal_cypress_switch_pppoeIpParseEnable_set;
    pMapper->switch_pkt2CpuTypeFormat_get = dal_cypress_switch_pkt2CpuTypeFormat_get;
    pMapper->switch_pkt2CpuTypeFormat_set = dal_cypress_switch_pkt2CpuTypeFormat_set;
    pMapper->switch_cpuPktTruncateEnable_get = dal_cypress_switch_cpuPktTruncateEnable_get;
    pMapper->switch_cpuPktTruncateEnable_set = dal_cypress_switch_cpuPktTruncateEnable_set;
    pMapper->switch_cpuPktTruncateLen_get = dal_cypress_switch_cpuPktTruncateLen_get;
    pMapper->switch_cpuPktTruncateLen_set = dal_cypress_switch_cpuPktTruncateLen_set;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_switch_init
 * Description:
 *      Initialize switch module of the specified device.
 * Input:
 *      unit          - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      1. Module must be initialized before using all of APIs in this module
 */
int32
dal_cypress_switch_init(uint32 unit)
{
    RT_INIT_REENTRY_CHK(switch_init[unit]);
    switch_init[unit] = INIT_NOT_COMPLETED;

    /* create semaphore */
    switch_sem[unit] = osal_sem_mutex_create();
    if (0 == switch_sem[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_SWITCH), "semaphore create failed");
        return RT_ERR_FAILED;
    }

    /* set init flag to complete init */
    switch_init[unit] = INIT_COMPLETED;
    dal_cypress_switch_pppoeIpParseEnable_set(unit, ENABLED);

    return RT_ERR_OK;
} /* end of dal_cypress_switch_init */

/* Function Name:
 *      dal_cypress_switch_cpuMaxPktLen_get
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
 *      RT_ERR_INPUT        - invalid packet direction
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_switch_cpuMaxPktLen_get(uint32 unit, rtk_switch_pktDir_t dir, uint32 *pLen)
{
    int32   ret;
    uint32  regField;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(switch_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(((PKTDIR_RX != dir) && (PKTDIR_TX != dir)), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pLen), RT_ERR_NULL_POINTER);

    regField = (PKTDIR_RX == dir)? CYPRESS_RX_MAX_LENf : CYPRESS_TX_MAX_LENf;

    SWITCH_SEM_LOCK(unit);
    /* get entry from CHIP*/
    if ((ret = reg_field_read(unit, CYPRESS_DMA_IF_PKT_FLTR_CTRLr, regField, pLen)) != RT_ERR_OK)
    {
        SWITCH_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }
    SWITCH_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH), "pLen=%x", *pLen);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_switch_cpuMaxPktLen_set
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
 *      RT_ERR_INPUT        - invalid packet direction
 *      RT_ERR_OUT_OF_RANGE - invalid packet length
 * Note:
 *      None
 */
int32
dal_cypress_switch_cpuMaxPktLen_set(uint32 unit, rtk_switch_pktDir_t dir, uint32 len)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(switch_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(((PKTDIR_RX != dir) && (PKTDIR_TX != dir)), RT_ERR_INPUT);
    RT_PARAM_CHK((len > HAL_MAX_ACCEPT_FRAME_LEN(unit)), RT_ERR_OUT_OF_RANGE);

    SWITCH_SEM_LOCK(unit);
    /* set entry to CHIP*/
    if (PKTDIR_RX == dir)
    {   /* RX */
        if ((ret = reg_field_write(unit, CYPRESS_DMA_IF_PKT_FLTR_CTRLr, CYPRESS_RX_MAX_LENf, &len)) != RT_ERR_OK)
        {
            SWITCH_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
            return ret;
        }
    }
    if (PKTDIR_TX == dir)
    {   /* TX */
        if ((ret = reg_field_write(unit, CYPRESS_DMA_IF_PKT_FLTR_CTRLr, CYPRESS_TX_MAX_LENf, &len)) != RT_ERR_OK)
        {
            SWITCH_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
            return ret;
        }
    }
    SWITCH_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_switch_maxPktLenLinkSpeed_get
 * Description:
 *      Get the max packet length setting of the specific speed type
 * Input:
 *      unit  - unit id
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
dal_cypress_switch_maxPktLenLinkSpeed_get(uint32 unit, rtk_switch_maxPktLen_linkSpeed_t speed, uint32 *pLen)
{
    int32   ret;
    uint32  reg_idx;
    uint32  field_idx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH), "unit=%d, speed=%d", unit, speed);

    /* check Init status */
    RT_INIT_CHK(switch_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pLen), RT_ERR_NULL_POINTER);

    switch (speed)
    {
        case MAXPKTLEN_LINK_SPEED_FE:
            reg_idx = CYPRESS_MAC_MAX_LEN_CTRLr;
            field_idx = CYPRESS_MAX_LEN_100M_10M_SELf;
            break;
        case MAXPKTLEN_LINK_SPEED_GE:
            reg_idx = CYPRESS_MAC_MAX_LEN_CTRLr;
            field_idx = CYPRESS_MAX_LEN_10G_1G_SELf;
            break;
        default:
            return RT_ERR_INPUT;
    }

    SWITCH_SEM_LOCK(unit);

    if ((ret = reg_field_read(unit, reg_idx, field_idx, pLen)) != RT_ERR_OK)
    {
        SWITCH_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }

    SWITCH_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH), "pLen=%x", *pLen);

    return RT_ERR_OK;
} /* end of dal_cypress_switch_maxPktLenLinkSpeed_get */

/* Function Name:
 *      dal_cypress_switch_maxPktLenLinkSpeed_set
 * Description:
 *      Set the max packet length of the specific speed type
 * Input:
 *      unit  - unit id
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
int32
dal_cypress_switch_maxPktLenLinkSpeed_set(uint32 unit, rtk_switch_maxPktLen_linkSpeed_t speed, uint32 len)
{
    int32   ret;
    uint32  reg_idx;
    uint32  field_idx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH), "unit=%d, speed=%d, len=%d", unit, speed, len);

    /* check Init status */
    RT_INIT_CHK(switch_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(len > HAL_MAX_ACCEPT_FRAME_LEN(unit), RT_ERR_OUT_OF_RANGE);

    switch (speed)
    {
        case MAXPKTLEN_LINK_SPEED_FE:
            reg_idx = CYPRESS_MAC_MAX_LEN_CTRLr;
            field_idx = CYPRESS_MAX_LEN_100M_10M_SELf;
            break;
        case MAXPKTLEN_LINK_SPEED_GE:
            reg_idx = CYPRESS_MAC_MAX_LEN_CTRLr;
            field_idx = CYPRESS_MAX_LEN_10G_1G_SELf;
            break;
        default:
            return RT_ERR_INPUT;
    }

    SWITCH_SEM_LOCK(unit);

    /* program value to CHIP*/
    if ((ret = reg_field_write(unit, reg_idx, field_idx, &len)) != RT_ERR_OK)
    {
        SWITCH_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }

    SWITCH_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_switch_maxPktLenLinkSpeed_set */

/* Function Name:
 *      dal_cypress_switch_maxPktLenLinkSpeed_get
 * Description:
 *      Get include or exclude tag length state of max packet length in the specified device.
 * Input:
 *      unit        - unit id
 * Output:
 *      pEnable     - pointer buffer of including/excluding tag length
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_switch_maxPktLenTagLenCntIncEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(switch_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    SWITCH_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_field_read(unit, CYPRESS_MAC_MAX_LEN_CTRLr, CYPRESS_MAX_LEN_TAG_INCf, &val)) != RT_ERR_OK)
    {
        SWITCH_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");

        return ret;
    }

    SWITCH_SEM_UNLOCK(unit);

    if (0 == val)
        *pEnable = DISABLED;
    else
        *pEnable = ENABLED;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_switch_maxPktLenLinkSpeed_set
 * Description:
 *      Set the max packet length to include or exclude tag length in the specified device.
 * Input:
 *      unit        - unit id
 *      enable      - include/exclude Tag length
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT   - invalid input parameter
 * Note:
 *      None
 */
int32
dal_cypress_switch_maxPktLenTagLenCntIncEnable_set(uint32 unit, rtk_enable_t enable)
{
    int32   ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH), "unit=%d, enable=%d", unit, enable);

    /* check Init status */
    RT_INIT_CHK(switch_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    if (DISABLED == enable)
        val = 0;
    else
        val = 1;

    SWITCH_SEM_LOCK(unit);

    /* program value to CHIP*/
    if ((ret = reg_field_write(unit, CYPRESS_MAC_MAX_LEN_CTRLr, CYPRESS_MAX_LEN_TAG_INCf, &val)) != RT_ERR_OK)
    {
        SWITCH_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }

    SWITCH_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_switch_snapMode_get
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
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      SNAP mode is as following
 *      - SNAP_MODE_AAAA03000000
 *      - SNAP_MODE_AAAA03
 */
int32
dal_cypress_switch_snapMode_get(uint32 unit, rtk_snapMode_t *pSnapMode)
{
    int32   ret;
    uint32  val = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(switch_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pSnapMode), RT_ERR_NULL_POINTER);

    SWITCH_SEM_LOCK(unit);

    if((ret = reg_field_read(unit, CYPRESS_PARSER_CTRLr, \
                                   CYPRESS_RFC1042_OUI_IGNOREf, &val)) != RT_ERR_OK)
    {
        SWITCH_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }
    *pSnapMode = (val == 0)? SNAP_MODE_AAAA03000000 : SNAP_MODE_AAAA03;

    SWITCH_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_switch_snapMode_get */

/* Function Name:
 *      dal_cypress_switch_snapMode_set
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
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      SNAP mode is as following
 *      - SNAP_MODE_AAAA03000000
 *      - SNAP_MODE_AAAA03
 */
int32
dal_cypress_switch_snapMode_set(uint32 unit, rtk_snapMode_t snapMode)
{
    int32   ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH), "unit=%d, snapMode=%d", unit, snapMode);

    /* check Init status */
    RT_INIT_CHK(switch_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((snapMode != SNAP_MODE_AAAA03000000) && (snapMode != SNAP_MODE_AAAA03), RT_ERR_INPUT);

    SWITCH_SEM_LOCK(unit);

    val = (snapMode == SNAP_MODE_AAAA03)? 1 : 0;
    if((ret = reg_field_write(unit, CYPRESS_PARSER_CTRLr, \
                                    CYPRESS_RFC1042_OUI_IGNOREf, &val)) != RT_ERR_OK)
    {
        SWITCH_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }

    SWITCH_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_switch_snapMode_set */

/* Function Name:
 *      dal_cypress_switch_chksumFailAction_get
 * Description:
 *      Get forwarding action of checksum error on specified port.
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      failType - checksum fail type
 * Output:
 *      pAction  - pointer to forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Checksum fail type is as following
 *      - LAYER2_CHKSUM_FAIL
 *
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 */
int32
dal_cypress_switch_chksumFailAction_get(
    uint32                      unit,
    rtk_port_t                  port,
    rtk_switch_chksum_fail_t    failType,
    rtk_action_t                *pAction)
{
    int32   ret;
    uint32  val = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(switch_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((failType != LAYER2_CHKSUM_FAIL), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    SWITCH_SEM_LOCK(unit);

    if(failType == LAYER2_CHKSUM_FAIL)
    {
        if((ret = reg_array_field_read(unit, CYPRESS_MAC_PORT_CTRLr, port, REG_ARRAY_INDEX_NONE, \
                                             CYPRESS_RX_CHK_CRC_ENf, &val)) != RT_ERR_OK)
        {
            SWITCH_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
            return ret;
        }
        *pAction = (val == 0)? ACTION_FORWARD : ACTION_DROP;
    }

    SWITCH_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH), "failType=%d", failType);

    return RT_ERR_OK;
} /* end of dal_cypress_switch_chksumFailAction_get */

/* Function Name:
 *      dal_cypress_switch_chksumFailAction_set
 * Description:
 *      Set forwarding action of checksum error on specified port.
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      failType - checksum fail type
 *      action   - forwarding action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_PORT_ID    - invalid port id
 *      RT_ERR_FWD_ACTION - invalid error forwarding action
 *      RT_ERR_INPUT      - invalid input parameter
 * Note:
 *      Checksum fail type is as following
 *      - LAYER2_CHKSUM_FAIL
 *
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 */
int32
dal_cypress_switch_chksumFailAction_set(
    uint32                      unit,
    rtk_port_t                  port,
    rtk_switch_chksum_fail_t    failType,
    rtk_action_t                action)
{
    int32   ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH), "unit=%d, port=%d, action=%d", unit, port, action);

    /* check Init status */
    RT_INIT_CHK(switch_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((failType != LAYER2_CHKSUM_FAIL), RT_ERR_INPUT);
    RT_PARAM_CHK((action != ACTION_DROP) && (action != ACTION_FORWARD), RT_ERR_INPUT);

    SWITCH_SEM_LOCK(unit);

    if(failType == LAYER2_CHKSUM_FAIL)
    {
        val = (action == ACTION_DROP)? 1 : 0;
        if((ret = reg_array_field_write(unit, CYPRESS_MAC_PORT_CTRLr, port, REG_ARRAY_INDEX_NONE, \
                                              CYPRESS_RX_CHK_CRC_ENf, &val)) != RT_ERR_OK)
        {
            SWITCH_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
            return ret;
        }
    }

    SWITCH_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_switch_chksumFailAction_set */

/* Function Name:
 *      dal_cypress_switch_recalcCRCEnable_get
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
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      When enable, mirrored packet with bad CRC will be recaculate at mirroring port.
 */
int32
dal_cypress_switch_recalcCRCEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  val = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(switch_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    SWITCH_SEM_LOCK(unit);

    if((ret = reg_array_field_read(unit, CYPRESS_MAC_PORT_CTRLr, port, REG_ARRAY_INDEX_NONE, \
                                         CYPRESS_BYP_TX_CRCf, &val)) != RT_ERR_OK)
    {
        SWITCH_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }
    *pEnable = (val == 0)? ENABLED : DISABLED;

    SWITCH_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_switch_recalcCRCEnable_get */

/* Function Name:
 *      dal_cypress_switch_recalcCRCEnable_set
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
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      When enable, mirrored packet with bad CRC will be recaculate at mirroring port.
 */
int32
dal_cypress_switch_recalcCRCEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH), "unit=%d, port=%d, enable=%d", unit, port, enable);

    /* check Init status */
    RT_INIT_CHK(switch_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    SWITCH_SEM_LOCK(unit);

    val = (enable == DISABLED)? 1 : 0;
    if((ret = reg_array_field_write(unit, CYPRESS_MAC_PORT_CTRLr, port, REG_ARRAY_INDEX_NONE, \
                                          CYPRESS_BYP_TX_CRCf, &val)) != RT_ERR_OK)
    {
        SWITCH_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }

    SWITCH_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_switch_recalcCRCEnable_set */

/* Function Name:
 *      dal_cypress_switch_mgmtMacAddr_get
 * Description:
 *      Get Mac address of switch.
 * Input:
 *      unit    - unit id
 * Output:
 *      pMac - pointer to Mac address
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *
 */
int32
dal_cypress_switch_mgmtMacAddr_get(uint32 unit, rtk_mac_t *pMac)
{
    int32   ret;
    uint32 val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(switch_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pMac), RT_ERR_NULL_POINTER);

    SWITCH_SEM_LOCK(unit);

    if ((ret = reg_field_read(unit, CYPRESS_MAC_ADDR_CTRLr, CYPRESS_SW_MAC_ADDR_31_0f, &val)) != RT_ERR_OK)
    {
        SWITCH_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }
    pMac->octet[2] = (val >> 24) & 0xff;
    pMac->octet[3] = (val >> 16) & 0xff;
    pMac->octet[4] = (val >> 8) & 0xff;
    pMac->octet[5] = (val >> 0) & 0xff;

    if ((ret = reg_field_read(unit, CYPRESS_MAC_ADDR_CTRLr, CYPRESS_SW_MAC_ADDR_47_32f, &val)) != RT_ERR_OK)
    {
        SWITCH_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }
    pMac->octet[0] = (val >> 8) & 0xff;
    pMac->octet[1] = (val >> 0) & 0xff;

    SWITCH_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "pMac=%x-%x-%x-%x-%x-%x",
           pMac->octet[0], pMac->octet[1], pMac->octet[2],
           pMac->octet[3], pMac->octet[4], pMac->octet[5]);

    return RT_ERR_OK;
} /*end of dal_cypress_switch_mgmtMacAddr_get*/

/* Function Name:
 *      dal_cypress_switch_mgmtMacAddr_set
 * Description:
 *      Set Mac address of switch.
 * Input:
 *      unit   - unit id
 *      pMac - pointer to Mac address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_MAC              - invalid mac address
 * Note:
 *      None
 */
int32
dal_cypress_switch_mgmtMacAddr_set(uint32 unit, rtk_mac_t *pMac)
{
    int32   ret;
    uint32 val;
    rtk_port_t port;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(switch_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pMac), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(((pMac->octet[0] & BITMASK_1B) != 0), RT_ERR_MAC);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "pMac=%x-%x-%x-%x-%x-%x",
           pMac->octet[0], pMac->octet[1], pMac->octet[2],
           pMac->octet[3], pMac->octet[4], pMac->octet[5]);

    SWITCH_SEM_LOCK(unit);

    val = (pMac->octet[2] << 24) | (pMac->octet[3] << 16) | (pMac->octet[4] << 8) | (pMac->octet[5] << 0);
    if ((ret = reg_field_write(unit, CYPRESS_MAC_ADDR_CTRLr, CYPRESS_SW_MAC_ADDR_31_0f, &val)) != RT_ERR_OK)
    {
        SWITCH_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }

    val = (pMac->octet[0] << 8) | (pMac->octet[1] << 0);
    if ((ret = reg_field_write(unit, CYPRESS_MAC_ADDR_CTRLr, CYPRESS_SW_MAC_ADDR_47_32f, &val)) != RT_ERR_OK)
    {
        SWITCH_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }

    /* Patch: Set router mac entry 0 */
    {
        val = (pMac->octet[2] << 24) | (pMac->octet[3] << 16) | (pMac->octet[4] << 8) | (pMac->octet[5] << 0);
        if((ret = reg_array_field_write(unit, CYPRESS_ROUTING_SA_CTRLr, REG_ARRAY_INDEX_NONE,
                            0, CYPRESS_SA_31_0f, &val)) != RT_ERR_OK)
        {
            SWITCH_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
            return ret;
        }
        val = (pMac->octet[0] << 8) | (pMac->octet[1] << 0);
        if((ret = reg_array_field_write(unit, CYPRESS_ROUTING_SA_CTRLr, REG_ARRAY_INDEX_NONE,
                            0, CYPRESS_SA_47_32f, &val)) != RT_ERR_OK)
        {
            SWITCH_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
            return ret;
        }
    }

    SWITCH_SEM_UNLOCK(unit);

    /* sync the Switch MAC address to all PHYs */
    HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
    {
        ret = phy_ptpSwitchMacAddr_set(unit, port, pMac);
        if ((ret != RT_ERR_OK) && (ret != RT_ERR_PORT_NOT_SUPPORTED))
        {
            RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
            return ret;
        }
    }

    return RT_ERR_OK;
} /*end of dal_cypress_switch_mgmtMacAddr_set*/

/* Function Name:
 *      dal_cypress_switch_IPv4Addr_get
 * Description:
 *      Get IPv4 address of switch.
 * Input:
 *      unit    - unit id
 * Output:
 *      pIpAddr - pointer to IPv4 address
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *
 */
int32
dal_cypress_switch_IPv4Addr_get(uint32 unit, uint32 *pIpAddr)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(switch_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIpAddr), RT_ERR_NULL_POINTER);

    SWITCH_SEM_LOCK(unit);

    if((ret = reg_field_read(unit, CYPRESS_SPCL_TRAP_SWITCH_IPV4_ADDRr, CYPRESS_IPADDRf, pIpAddr)) != RT_ERR_OK)
    {
        SWITCH_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }

    SWITCH_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH), "pIpAddr=%d", *pIpAddr);
    return RT_ERR_OK;
}  /*end of dal_cypress_switch_IPv4Addr_get*/


/* Function Name:
 *      dal_cypress_switch_IPv4Addr_set
 * Description:
 *      Set IPv4 address of switch.
 * Input:
 *      unit   - unit id
 *      ipAddr - IPv4 address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_IPV4_ADDRESS     - invalid IPv4 address
 * Note:
 *      None
 */
int32
dal_cypress_switch_IPv4Addr_set(uint32 unit, uint32 ipAddr)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH), "unit=%d, ipAddr=%d", unit, ipAddr);

    /* check Init status */
    RT_INIT_CHK(switch_init[unit]);

    SWITCH_SEM_LOCK(unit);

    if((ret = reg_field_write(unit, CYPRESS_SPCL_TRAP_SWITCH_IPV4_ADDRr, CYPRESS_IPADDRf, &ipAddr)) != RT_ERR_OK)
    {
        SWITCH_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }

    SWITCH_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /*end of dal_cypress_switch_IPv4Addr_set*/

/* Function Name:
 *      dal_cypress_switch_pppoeIpParseEnable_get
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
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_switch_pppoeIpParseEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  enable;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(switch_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    SWITCH_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_field_read(unit, CYPRESS_L2_CTRL_0r, CYPRESS_PPPOE_PARSE_ENf, &enable)) != RT_ERR_OK)
    {
        SWITCH_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }
    SWITCH_SEM_UNLOCK(unit);

    /* translate chip's value to definition */
    /* The definition of pppoe pass-through differs from that of chip */
    if (1 == enable)
    {
        *pEnable = DISABLED;
    }
    else
    {
        *pEnable =ENABLED ;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_switch_pppoeIpParseEnable_set
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
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
int32
dal_cypress_switch_pppoeIpParseEnable_set(uint32 unit, rtk_enable_t enable)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH), "unit=%d, enable=%d",
           unit, enable);

    /* check Init status */
    RT_INIT_CHK(switch_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((ENABLED != enable && DISABLED != enable), RT_ERR_INPUT);

    /* translate definition to chip's value  */
    /* The definition of pppoe pass-through differs from that of chip */
    if (ENABLED == enable)
    {
        value = 0;
    }
    else
    {
        value = 1;
    }

    SWITCH_SEM_LOCK(unit);

    /* programming value to CHIP*/
    if ((ret = reg_field_write(unit, CYPRESS_L2_CTRL_0r, CYPRESS_PPPOE_PARSE_ENf, &value)) != RT_ERR_OK)
    {
        SWITCH_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }
    SWITCH_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_switch_pkt2CpuTypeFormat_get
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
 *
 *      The type of format:
 *      - TRAP_PKT_ORIGINAL
 *      - TRAP_PKT_MODIFIED
 */
int32
dal_cypress_switch_pkt2CpuTypeFormat_get(uint32 unit, rtk_switch_pkt2CpuType_t type,
    rtk_pktFormat_t *pFormat)
{
    uint32  value, field;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH), "unit=%d, type=%d", unit, type);

    /* check Init status */
    RT_INIT_CHK(switch_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((PKT2CPU_TYPE_END <= type), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pFormat), RT_ERR_NULL_POINTER);

    switch (type)
    {
        case PKT2CPU_TYPE_FORWARD:
            field = CYPRESS_FWD_PKT_FMTf;
            break;
        case PKT2CPU_TYPE_TRAP:
            field = CYPRESS_TRAP_PKT_FMTf;
            break;
        default:
            return RT_ERR_INPUT;
    }

    SWITCH_SEM_LOCK(unit);

    ret = reg_field_read(unit, CYPRESS_DMA_IF_PKT_CTRLr, field, &value);
    if (ret != RT_ERR_OK)
    {
        SWITCH_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }

    SWITCH_SEM_UNLOCK(unit);

    switch (value)
    {
        case 0:
            *pFormat = ORIGINAL_PACKET;
            break;
        case 1:
            *pFormat = MODIFIED_PACKET;
            break;
        default:
            return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}   /* end of dal_cypress_switch_pkt2CpuTypeFormat_get */

/* Function Name:
 *      dal_cypress_switch_pkt2CpuTypeFormat_set
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
int32
dal_cypress_switch_pkt2CpuTypeFormat_set(uint32 unit, rtk_switch_pkt2CpuType_t type,
    rtk_pktFormat_t format)
{
    uint32  value, field;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH), "unit=%d, type=%d, format=%d", unit, type, format);

    /* check Init status */
    RT_INIT_CHK(switch_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((PKT2CPU_TYPE_END <= type), RT_ERR_INPUT);
    RT_PARAM_CHK((PKT_FORMAT_END <= format), RT_ERR_INPUT);

    switch (type)
    {
        case PKT2CPU_TYPE_FORWARD:
            field = CYPRESS_FWD_PKT_FMTf;
            break;
        case PKT2CPU_TYPE_TRAP:
            field = CYPRESS_TRAP_PKT_FMTf;
            break;
        default:
            return RT_ERR_INPUT;
    }

    switch (format)
    {
        case ORIGINAL_PACKET:
            value = 0;
            break;
        case MODIFIED_PACKET:
            value = 1;
            break;
        default:
            return RT_ERR_INPUT;
    }

    SWITCH_SEM_LOCK(unit);

    ret = reg_field_write(unit, CYPRESS_DMA_IF_PKT_CTRLr, field, &value);
    if (ret != RT_ERR_OK)
    {
        SWITCH_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }

    SWITCH_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_cypress_switch_pkt2CpuTypeFormat_set */

/* Function Name:
 *      dal_cypress_switch_cpuPktTruncateEnable_get
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
int32
dal_cypress_switch_cpuPktTruncateEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  val = 0;

    /* check Init status */
    RT_INIT_CHK(switch_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    SWITCH_SEM_LOCK(unit);
    if ((ret = reg_field_read(unit, CYPRESS_DMA_IF_CTRLr, CYPRESS_RX_TRUNCATE_ENf, &val)) != RT_ERR_OK)
    {
        SWITCH_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }
    *pEnable = (val == 1)? ENABLED : DISABLED;
    SWITCH_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_switch_cpuPktTruncateEnable_set
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
int32
dal_cypress_switch_cpuPktTruncateEnable_set(uint32 unit, rtk_enable_t enable)
{
    int32   ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH), "unit=%d, enable=%d", unit, enable);

    /* check Init status */
    RT_INIT_CHK(switch_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    SWITCH_SEM_LOCK(unit);
    val = (enable == ENABLED)? 1 : 0;
    if ((ret = reg_field_write(unit, CYPRESS_DMA_IF_CTRLr, CYPRESS_RX_TRUNCATE_ENf, &val)) != RT_ERR_OK)
    {
        SWITCH_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }
    SWITCH_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_switch_cpuPktTruncateLen_get
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
int32
dal_cypress_switch_cpuPktTruncateLen_get(uint32 unit, uint32 *pLen)
{
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(switch_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pLen), RT_ERR_NULL_POINTER);

    SWITCH_SEM_LOCK(unit);
    if ((ret = reg_field_read(unit, CYPRESS_DMA_IF_CTRLr, CYPRESS_RX_TRUNCATE_LENf, pLen)) != RT_ERR_OK)
    {
        SWITCH_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }
    SWITCH_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH), "pLen=%x", *pLen);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_switch_cpuPktTruncateLen_set
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
int32
dal_cypress_switch_cpuPktTruncateLen_set(uint32 unit, uint32 len)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH), "unit=%d, len=%d", unit, len);

    /* check Init status */
    RT_INIT_CHK(switch_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((len > HAL_MAX_ACCEPT_FRAME_LEN(unit)), RT_ERR_OUT_OF_RANGE);

    SWITCH_SEM_LOCK(unit);
    if ((ret = reg_field_write(unit, CYPRESS_DMA_IF_CTRLr, CYPRESS_RX_TRUNCATE_LENf, &len)) != RT_ERR_OK)
    {
        SWITCH_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }
    SWITCH_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

