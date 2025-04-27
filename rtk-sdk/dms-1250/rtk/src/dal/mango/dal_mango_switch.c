
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
#include <hal/chipdef/mango/rtk_mango_table_struct.h>
#include <hal/chipdef/mango/rtk_mango_reg_struct.h>
#include <hal/mac/reg.h>
#include <hal/mac/mem.h>
#include <hal/common/halctrl.h>
#include <dal/mango/dal_mango_switch.h>
#include <rtk/default.h>
#include <rtk/switch.h>
#include <dal/mango/dal_mango_port.h>
#include <hal/common/miim.h>
#include <osal/time.h>

#include <ioal/mem32.h>

#include <hal/common/miim.h>

/*
 * Symbol Definition
 */
static uint32 _flexTblFmt[] = {
    /* notice: CANNOT change the order */
    RTK_SWITCH_FLEX_TBL_FMT_L2_TUNNEL,
    RTK_SWITCH_FLEX_TBL_FMT_MPLS,
    RTK_SWITCH_FLEX_TBL_FMT_IP_MAC_BINDING,
    RTK_SWITCH_FLEX_TBL_FMT_PE_ECID,
    };

/*
 * Data Declaration
 */
static uint32               switch_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};
static osal_mutex_t         switch_sem[RTK_MAX_NUM_OF_UNIT];
static osal_mutex_t         switch_tbl_sem[RTK_MAX_NUM_OF_UNIT][DAL_MANGO_SWITCH_TBL_END];


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
int32
dal_mango_switchMapper_init(dal_mapper_t *pMapper)
{
    pMapper->switch_init = dal_mango_switch_init;
    pMapper->switch_cpuMaxPktLen_get = dal_mango_switch_cpuMaxPktLen_get;
    pMapper->switch_cpuMaxPktLen_set = dal_mango_switch_cpuMaxPktLen_set;
    pMapper->switch_snapMode_get = dal_mango_switch_snapMode_get;
    pMapper->switch_snapMode_set = dal_mango_switch_snapMode_set;
    pMapper->switch_recalcCRCEnable_get = dal_mango_switch_recalcCRCEnable_get;
    pMapper->switch_recalcCRCEnable_set = dal_mango_switch_recalcCRCEnable_set;
    pMapper->switch_mgmtMacAddr_get = dal_mango_switch_mgmtMacAddr_get;
    pMapper->switch_mgmtMacAddr_set = dal_mango_switch_mgmtMacAddr_set;
    pMapper->switch_pppoeIpParseEnable_get = dal_mango_switch_pppoeIpParseEnable_get;
    pMapper->switch_pppoeIpParseEnable_set = dal_mango_switch_pppoeIpParseEnable_set;
    pMapper->switch_pkt2CpuTypeFormat_get = dal_mango_switch_pkt2CpuTypeFormat_get;
    pMapper->switch_pkt2CpuTypeFormat_set = dal_mango_switch_pkt2CpuTypeFormat_set;
    pMapper->switch_cpuPktTruncateEnable_get = dal_mango_switch_cpuPktTruncateEnable_get;
    pMapper->switch_cpuPktTruncateEnable_set = dal_mango_switch_cpuPktTruncateEnable_set;
    pMapper->switch_cpuPktTruncateLen_get = dal_mango_switch_cpuPktTruncateLen_get;
    pMapper->switch_cpuPktTruncateLen_set = dal_mango_switch_cpuPktTruncateLen_set;
    pMapper->switch_portMaxPktLenLinkSpeed_get = dal_mango_switch_portMaxPktLenLinkSpeed_get;
    pMapper->switch_portMaxPktLenLinkSpeed_set = dal_mango_switch_portMaxPktLenLinkSpeed_set;
    pMapper->switch_portMaxPktLenTagLenCntIncEnable_get = dal_mango_switch_portMaxPktLenTagLenCntIncEnable_get;
    pMapper->switch_portMaxPktLenTagLenCntIncEnable_set = dal_mango_switch_portMaxPktLenTagLenCntIncEnable_set;
    pMapper->switch_flexTblFmt_get = dal_mango_switch_flexTblFmt_get;
    pMapper->switch_flexTblFmt_set = dal_mango_switch_flexTblFmt_set;

    return RT_ERR_OK;
}

int32
_dal_mango_switch_table_init(uint32 unit)
{
    return RT_ERR_OK;
}

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
 * Note:
 *      1. Module must be initialized before using all of APIs in this module
 */
int32
dal_mango_switch_init(uint32 unit)
{
#ifndef __BOOTLOADER__
    dal_mango_switch_tbl_t tbl_idx;
#endif  /* __BOOTLOADER__ */
    RT_INIT_REENTRY_CHK(switch_init[unit]);
    switch_init[unit] = INIT_NOT_COMPLETED;

    /* create semaphore */
    switch_sem[unit] = osal_sem_mutex_create();
    if (0 == switch_sem[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_SWITCH), "semaphore create failed");
        return RT_ERR_FAILED;
    }
#ifndef __BOOTLOADER__
    /* create semaphores for all tables in dal_mango_switch_tbl_t */
    for (tbl_idx = 0; tbl_idx < DAL_MANGO_SWITCH_TBL_END; tbl_idx++)
    {
        switch_tbl_sem[unit][tbl_idx]  = osal_sem_mutex_create();
        if (0 == switch_tbl_sem[unit][tbl_idx])
        {
            RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_SWITCH), "semaphore create failed");
            return RT_ERR_FAILED;
        }
    }
#endif  /* __BOOTLOADER__ */

    if (HWP_UNIT_VALID_LOCAL(unit))
    {
        /* init all switch tables */
        _dal_mango_switch_table_init(unit);
    }

    /* set init flag to complete init */
    switch_init[unit] = INIT_COMPLETED;

    if (HWP_UNIT_VALID_LOCAL(unit))
    {
#ifndef __BOOTLOADER__
        dal_mango_switch_pppoeIpParseEnable_set(unit, ENABLED);
#endif  /* __BOOTLOADER__ */
    }

    return RT_ERR_OK;
} /* end of dal_mango_switch_init */

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
_dal_mango_switch_tblSemaphore_lock(uint32 unit, dal_mango_switch_tbl_t tbl)
{
    if (osal_sem_mutex_take(switch_tbl_sem[unit][tbl], OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_SEM_LOCK_FAILED, (MOD_DAL|MOD_SWITCH), "semaphore lock failed");
        return RT_ERR_SEM_LOCK_FAILED;
    }

    return RT_ERR_OK;
}

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
_dal_mango_switch_tblSemaphore_unlock(uint32 unit, dal_mango_switch_tbl_t tbl)
{
    if (osal_sem_mutex_give(switch_tbl_sem[unit][tbl]) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_SEM_UNLOCK_FAILED, (MOD_DAL|MOD_SWITCH), "semaphore unlock failed");
        return RT_ERR_SEM_UNLOCK_FAILED;
    }

    return RT_ERR_OK;
}

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
 *      RT_ERR_INPUT        - invalid packet direction
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_mango_switch_cpuMaxPktLen_get(uint32 unit, rtk_switch_pktDir_t dir, uint32 *pLen)
{
    uint32  field;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(switch_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(((PKTDIR_RX != dir) && (PKTDIR_TX != dir)), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pLen), RT_ERR_NULL_POINTER);

    if (PKTDIR_RX == dir)
    {
        field = MANGO_CPU_PORT_RX_MAX_LENf;
    }
    else
    {
        field = MANGO_CPU_PORT_TX_MAX_LENf;
    }

    SWITCH_SEM_LOCK(unit);
    /* get entry from CHIP*/
    if ((ret = reg_field_read(unit, MANGO_MAC_L2_CPU_MAX_LEN_CTRLr, field, pLen)) != RT_ERR_OK)
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
 *      RT_ERR_INPUT        - invalid packet direction
 *      RT_ERR_OUT_OF_RANGE - invalid packet length
 * Note:
 *      None
 */
int32
dal_mango_switch_cpuMaxPktLen_set(uint32 unit, rtk_switch_pktDir_t dir, uint32 len)
{
    uint32  field;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(switch_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(((PKTDIR_RX != dir) && (PKTDIR_TX != dir)), RT_ERR_INPUT);
    RT_PARAM_CHK((len > HAL_MAX_ACCEPT_FRAME_LEN(unit)), RT_ERR_OUT_OF_RANGE);

    if (PKTDIR_RX == dir)
    {
        field = MANGO_CPU_PORT_RX_MAX_LENf;
    }
    else
    {
        field = MANGO_CPU_PORT_TX_MAX_LENf;
    }

    SWITCH_SEM_LOCK(unit);

    if ((ret = reg_field_write(unit, MANGO_MAC_L2_CPU_MAX_LEN_CTRLr, field, &len)) != RT_ERR_OK)
    {
        SWITCH_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }

    SWITCH_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

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
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      SNAP mode is as following
 *      - SNAP_MODE_AAAA03000000
 *      - SNAP_MODE_AAAA03
 */
int32
dal_mango_switch_snapMode_get(uint32 unit, rtk_snapMode_t *pSnapMode)
{
    int32   ret;
    uint32  val = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(switch_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pSnapMode), RT_ERR_NULL_POINTER);

    SWITCH_SEM_LOCK(unit);

    if((ret = reg_field_read(unit, MANGO_PARSER_CTRLr,
            MANGO_RFC1042_OUI_IGNOREf, &val)) != RT_ERR_OK)
    {
        SWITCH_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }

    *pSnapMode = (val == 0)? SNAP_MODE_AAAA03000000 : SNAP_MODE_AAAA03;

    SWITCH_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_mango_switch_snapMode_get */

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
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      SNAP mode is as following
 *      - SNAP_MODE_AAAA03000000
 *      - SNAP_MODE_AAAA03
 */
int32
dal_mango_switch_snapMode_set(uint32 unit, rtk_snapMode_t snapMode)
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
    if((ret = reg_field_write(unit, MANGO_PARSER_CTRLr,
            MANGO_RFC1042_OUI_IGNOREf, &val)) != RT_ERR_OK)
    {
        SWITCH_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }

    SWITCH_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_mango_switch_snapMode_set */

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
int32
dal_mango_switch_mgmtMacAddr_get(uint32 unit, rtk_mac_t *pMac)
{
    int32   ret = RT_ERR_FAILED;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(switch_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pMac), RT_ERR_NULL_POINTER);

    SWITCH_SEM_LOCK(unit);

    if ((ret = reg_field_read(unit, MANGO_MAC_L2_ADDR_CTRLr, MANGO_SW_MAC_ADDR_47_32f, &val)) != RT_ERR_OK)
    {
        SWITCH_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }
    pMac->octet[0] = (val >> 8) & 0xff;
    pMac->octet[1] = (val >> 0) & 0xff;

    if ((ret = reg_field_read(unit, MANGO_MAC_L2_ADDR_CTRLr, MANGO_SW_MAC_ADDR_31_0f, &val)) != RT_ERR_OK)
    {
        SWITCH_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }

    pMac->octet[2] = (val >> 24) & 0xff;
    pMac->octet[3] = (val >> 16) & 0xff;
    pMac->octet[4] = (val >> 8) & 0xff;
    pMac->octet[5] = (val >> 0) & 0xff;


    SWITCH_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH), "pMac=%x-%x-%x-%x-%x-%x",
           pMac->octet[0], pMac->octet[1], pMac->octet[2],
           pMac->octet[3], pMac->octet[4], pMac->octet[5]);

    return RT_ERR_OK;
}

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
int32
dal_mango_switch_mgmtMacAddr_set(uint32 unit, rtk_mac_t *pMac)
{
    int32   ret = RT_ERR_FAILED;
    uint32  val;
    rtk_port_t port;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(switch_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pMac), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(((pMac->octet[0] & BITMASK_1B) != 0), RT_ERR_MAC);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH), "pMac=%x-%x-%x-%x-%x-%x",
           pMac->octet[0], pMac->octet[1], pMac->octet[2],
           pMac->octet[3], pMac->octet[4], pMac->octet[5]);

    SWITCH_SEM_LOCK(unit);

    val = (pMac->octet[2] << 24) | (pMac->octet[3] << 16) | (pMac->octet[4] << 8) | (pMac->octet[5] << 0);
    if ((ret = reg_field_write(unit, MANGO_MAC_L2_ADDR_CTRLr, MANGO_SW_MAC_ADDR_31_0f, &val)) != RT_ERR_OK)
    {
        SWITCH_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }

    val = (pMac->octet[0] << 8) | (pMac->octet[1] << 0);
    if ((ret = reg_field_write(unit, MANGO_MAC_L2_ADDR_CTRLr, MANGO_SW_MAC_ADDR_47_32f, &val)) != RT_ERR_OK)
    {
        SWITCH_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
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

        ret = phy_ptpSwitchMacRange_set(unit, port, 0);
        if ((ret != RT_ERR_OK) && (ret != RT_ERR_PORT_NOT_SUPPORTED))
        {
            RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
            return ret;
        }
    }


    return RT_ERR_OK;
}

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
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      When enable, mirrored packet with bad CRC will be recaculate at mirroring port.
 */
int32
dal_mango_switch_recalcCRCEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
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

    if((ret = reg_array_field_read(unit, MANGO_MAC_L2_PORT_CTRLr, port,
            REG_ARRAY_INDEX_NONE, MANGO_BYP_TX_CRCf, &val)) != RT_ERR_OK)
    {
        SWITCH_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }
    *pEnable = (val == 0)? ENABLED : DISABLED;

    SWITCH_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_mango_switch_recalcCRCEnable_get */

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
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      When enable, mirrored packet with bad CRC will be recaculate at mirroring port.
 */
int32
dal_mango_switch_recalcCRCEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
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
    if((ret = reg_array_field_write(unit, MANGO_MAC_L2_PORT_CTRLr, port,
            REG_ARRAY_INDEX_NONE, MANGO_BYP_TX_CRCf, &val)) != RT_ERR_OK)
    {
        SWITCH_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }

    SWITCH_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_mango_switch_recalcCRCEnable_set */

/* Function Name:
 *      dal_mango_switch_pppoeIpParseEnable_get
 * Description:
 *      Get enable status of PPPoE IP-parse functionality.
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
dal_mango_switch_pppoeIpParseEnable_get(uint32 unit, rtk_enable_t *pEnable)
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
    if ((ret = reg_field_read(unit, MANGO_PARSER_CTRLr, MANGO_PPPOE_PARSE_ENf,
            &enable)) != RT_ERR_OK)
    {
        SWITCH_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }
    SWITCH_SEM_UNLOCK(unit);

    /* translate chip's value to definition */
    if (1 == enable)
    {
        *pEnable = ENABLED;
    }
    else
    {
        *pEnable = DISABLED;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
}   /* end of dal_mango_switch_pppoeIpParseEnable_get */

/* Function Name:
 *      dal_mango_switch_pppoeIpParseEnable_set
 * Description:
 *      Set enable status of PPPoE IP-parse functionality.
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
dal_mango_switch_pppoeIpParseEnable_set(uint32 unit, rtk_enable_t enable)
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
    if (ENABLED == enable)
    {
        value = 1;
    }
    else
    {
        value = 0;
    }

    SWITCH_SEM_LOCK(unit);

    /* programming value to CHIP*/
    if ((ret = reg_field_write(unit, MANGO_PARSER_CTRLr, MANGO_PPPOE_PARSE_ENf,
            &value)) != RT_ERR_OK)
    {
        SWITCH_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }
    SWITCH_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_switch_pppoeIpParseEnable_set */

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
int32
dal_mango_switch_pkt2CpuTypeFormat_get(uint32 unit, rtk_switch_pkt2CpuType_t type,
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
            field = MANGO_FWD_PKT_FMTf;
            break;
        case PKT2CPU_TYPE_TRAP:
            field = MANGO_TRAP_PKT_FMTf;
            break;
        case PKT2CPU_TYPE_VLAN:
            field = MANGO_PKT_VLAN_FMTf;
            break;
        case PKT2CPU_ETAG_TYPE_FORWARD:
            field = MANGO_FWD_ETAG_PKT_FMTf;
            break;
        case PKT2CPU_ETAG_TYPE_TRAP:
            field = MANGO_TRAP_ETAG_PKT_FMTf;
            break;
        default:
            return RT_ERR_INPUT;
    }

    SWITCH_SEM_LOCK(unit);

    ret = reg_field_read(unit, MANGO_DMA_IF_PKT_CTRLr, field, &value);
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
}   /* end of dal_mango_switch_pkt2CpuTypeFormat_get */

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
int32
dal_mango_switch_pkt2CpuTypeFormat_set(uint32 unit, rtk_switch_pkt2CpuType_t type,
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
            field = MANGO_FWD_PKT_FMTf;
            break;
        case PKT2CPU_TYPE_TRAP:
            field = MANGO_TRAP_PKT_FMTf;
            break;
        case PKT2CPU_TYPE_VLAN:
            field = MANGO_PKT_VLAN_FMTf;
            break;
        case PKT2CPU_ETAG_TYPE_FORWARD:
            field = MANGO_FWD_ETAG_PKT_FMTf;
            break;
        case PKT2CPU_ETAG_TYPE_TRAP:
            field = MANGO_TRAP_ETAG_PKT_FMTf;
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

    ret = reg_field_write(unit, MANGO_DMA_IF_PKT_CTRLr, field, &value);
    if (ret != RT_ERR_OK)
    {
        SWITCH_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }

    SWITCH_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_switch_pkt2CpuTypeFormat_set */

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
int32
dal_mango_switch_cpuPktTruncateEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  val = 0;

    /* check Init status */
    RT_INIT_CHK(switch_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    SWITCH_SEM_LOCK(unit);
    if ((ret = reg_field_read(unit, MANGO_DMA_IF_CTRLr, MANGO_RX_TRUNCATE_ENf,
            &val)) != RT_ERR_OK)
    {
        SWITCH_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }
    *pEnable = (val == 1)? ENABLED : DISABLED;
    SWITCH_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_switch_cpuPktTruncateEnable_get */

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
int32
dal_mango_switch_cpuPktTruncateEnable_set(uint32 unit, rtk_enable_t enable)
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
    if ((ret = reg_field_write(unit, MANGO_DMA_IF_CTRLr, MANGO_RX_TRUNCATE_ENf,
            &val)) != RT_ERR_OK)
    {
        SWITCH_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }
    SWITCH_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_switch_cpuPktTruncateEnable_set */

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
int32
dal_mango_switch_cpuPktTruncateLen_get(uint32 unit, uint32 *pLen)
{
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(switch_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pLen), RT_ERR_NULL_POINTER);

    SWITCH_SEM_LOCK(unit);
    if ((ret = reg_field_read(unit, MANGO_DMA_IF_CTRLr, MANGO_RX_TRUNCATE_LENf,
            pLen)) != RT_ERR_OK)
    {
        SWITCH_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }
    SWITCH_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH), "pLen=%x", *pLen);

    return RT_ERR_OK;
}   /* end of dal_mango_switch_cpuPktTruncateLen_get */

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
int32
dal_mango_switch_cpuPktTruncateLen_set(uint32 unit, uint32 len)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH), "unit=%d, len=%d", unit, len);

    /* check Init status */
    RT_INIT_CHK(switch_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((len > HAL_MAX_ACCEPT_FRAME_LEN(unit)), RT_ERR_OUT_OF_RANGE);

    SWITCH_SEM_LOCK(unit);
    if ((ret = reg_field_write(unit, MANGO_DMA_IF_CTRLr, MANGO_RX_TRUNCATE_LENf,
            &len)) != RT_ERR_OK)
    {
        SWITCH_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }
    SWITCH_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_switch_cpuPktTruncateLen_set */

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
int32
dal_mango_switch_portMaxPktLenLinkSpeed_get(uint32 unit, rtk_port_t port,
    rtk_switch_maxPktLen_linkSpeed_t speed, uint32 *pLen)
{
    uint32  field;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH), "unit=%d,port=%d,speed=%d", unit, port, speed);

    /* check Init status */
    RT_INIT_CHK(switch_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((MAXPKTLEN_LINK_SPEED_END <= speed), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pLen), RT_ERR_NULL_POINTER);

    /* function body */
    switch (speed)
    {
        case MAXPKTLEN_LINK_SPEED_FE:
            field = MANGO_MAX_LEN_100M_10M_SELf;
            break;
        case MAXPKTLEN_LINK_SPEED_GE:
            field = MANGO_MAX_LEN_1G_2P5G_5G_10G_SELf;
            break;
        default:
            return RT_ERR_INPUT;
    }

    SWITCH_SEM_LOCK(unit);

    if ((ret = reg_array_field_read(unit, MANGO_MAC_L2_PORT_MAX_LEN_CTRLr,
            port, REG_ARRAY_INDEX_NONE, field, pLen)) != RT_ERR_OK)
    {
        SWITCH_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }

    SWITCH_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_switch_portMaxPktLenLinkSpeed_get */

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
int32
dal_mango_switch_portMaxPktLenLinkSpeed_set(uint32 unit, rtk_port_t port,
    rtk_switch_maxPktLen_linkSpeed_t speed, uint32 len)
{
    uint32  field;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH), "unit=%d,port=%d,speed=%d,len=%d", unit, port, speed, len);

    /* check Init status */
    RT_INIT_CHK(switch_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((MAXPKTLEN_LINK_SPEED_END <= speed), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_MAX_ACCEPT_FRAME_LEN(unit) < len), RT_ERR_INPUT);

    /* function body */
    switch (speed)
    {
        case MAXPKTLEN_LINK_SPEED_FE:
            field = MANGO_MAX_LEN_100M_10M_SELf;
            break;
        case MAXPKTLEN_LINK_SPEED_GE:
            field = MANGO_MAX_LEN_1G_2P5G_5G_10G_SELf;
            break;
        default:
            return RT_ERR_INPUT;
    }

    SWITCH_SEM_LOCK(unit);

    if ((ret = reg_array_field_write(unit, MANGO_MAC_L2_PORT_MAX_LEN_CTRLr,
            port, REG_ARRAY_INDEX_NONE, field, &len)) != RT_ERR_OK)
    {
        SWITCH_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }

    SWITCH_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_switch_portMaxPktLenLinkSpeed_set */

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
int32
dal_mango_switch_portMaxPktLenTagLenCntIncEnable_get(uint32 unit, rtk_port_t port,
    rtk_enable_t *pEnable)
{
    uint32  val;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH), "unit=%d,port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(switch_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    SWITCH_SEM_LOCK(unit);

    if ((ret = reg_array_field_read(unit, MANGO_MAC_L2_PORT_MAX_LEN_CTRLr,
            port, REG_ARRAY_INDEX_NONE, MANGO_MAX_LEN_TAG_INCf, &val)) != RT_ERR_OK)
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
}   /* end of dal_mango_switch_portMaxPktLenTagLenCntIncEnable_get */

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
int32
dal_mango_switch_portMaxPktLenTagLenCntIncEnable_set(uint32 unit, rtk_port_t port,
    rtk_enable_t enable)
{
    uint32  val;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH), "unit=%d,port=%d,enable=%d", unit, port, enable);

    /* check Init status */
    RT_INIT_CHK(switch_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    /* function body */
    if (DISABLED == enable)
        val = 0;
    else
        val = 1;

    SWITCH_SEM_LOCK(unit);

    if ((ret = reg_array_field_write(unit, MANGO_MAC_L2_PORT_MAX_LEN_CTRLr,
            port, REG_ARRAY_INDEX_NONE, MANGO_MAX_LEN_TAG_INCf, &val)) != RT_ERR_OK)
    {
        SWITCH_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }

    SWITCH_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_switch_portMaxPktLenTagLenCntIncEnable_set */

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
int32
dal_mango_switch_flexTblFmt_get(uint32 unit, rtk_switch_flexTblFmt_t *pFmt)
{
    int32   ret = RT_ERR_FAILED;
    uint32  val;
    rtk_switch_flexTblFmt_t  format;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(switch_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pFmt), RT_ERR_NULL_POINTER);

    /* function body */
    SWITCH_SEM_LOCK(unit);
    if ((ret = reg_field_read(unit, MANGO_FLEX_TBL_CTRLr, MANGO_FLEX_TBL_FMTf, &val)) != RT_ERR_OK)
    {
        SWITCH_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }
    SWITCH_SEM_UNLOCK(unit);

    if ((ret = RT_UTIL_ACTLIST_ACTION_GET(_flexTblFmt, format, val)) != RT_ERR_OK) \
    {
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }

    *pFmt = format;

    return RT_ERR_OK;
}   /* end of dal_mango_switch_flexTblFmt_get */

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
int32
dal_mango_switch_flexTblFmt_set(uint32 unit, rtk_switch_flexTblFmt_t fmt)
{
    int32   ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH), "unit=%d,fmt=%d", unit, fmt);

    /* check Init status */
    RT_INIT_CHK(switch_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((RTK_SWITCH_FLEX_TBL_FMT_END <= fmt), RT_ERR_INPUT);
    if (RTL9311E_CHIP_ID == HWP_CHIP_ID(unit) && RTK_SWITCH_FLEX_TBL_FMT_MPLS == fmt)
    {
        return RT_ERR_FAILED;
    }

    /* function body */
    if ((ret = RT_UTIL_ACTLIST_INDEX_GET(_flexTblFmt, val, fmt)) != RT_ERR_OK) \
    {
        RT_ERR(ret, (MOD_BPE|MOD_DAL), "");
        return ret;
    }

    SWITCH_SEM_LOCK(unit);
    if ((ret = reg_field_write(unit, MANGO_FLEX_TBL_CTRLr, MANGO_FLEX_TBL_FMTf, &val)) != RT_ERR_OK)
    {
        SWITCH_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }

    // [SS-1136] set global VLAN N:1 function based on the current table format
    if (RTK_SWITCH_FLEX_TBL_FMT_PE_ECID == fmt)
    {
        val = 0;    // disable for BPE
    } else {
        val = 1;    // enable for others
    }
    if ((ret = reg_field_write(unit, MANGO_VLAN_L2TBL_CNVT_CTRLr, MANGO_GLB_VID_CNVT_ENf, &val)) != RT_ERR_OK)
    {
        SWITCH_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }
    SWITCH_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_switch_flexTblFmt_set */

