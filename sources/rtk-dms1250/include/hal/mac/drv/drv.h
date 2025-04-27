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
 * Purpose : mac driver service APIs in the SDK.
 *
 * Feature : mac driver service APIs
 *
 */

#ifndef __HAL_MAC_DRV_H__
#define __HAL_MAC_DRV_H__

/*
 * Symbol Definition
 */
#define ACCESS_PHY_TIMEOUT_TIME             (2 * 1000 * 1000) /* microseconds */

/*
 * Macro Declaration
 */
/* table semaphore handling */
#define MEM_SEM_LOCK(unit, group)\
do {\
    if (osal_sem_mutex_take(hal_ctrl[unit].tbl_sem[group], OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "table semaphore lock group %d failed", group);\
        return RT_ERR_FAILED;\
    }\
} while(0)

#define MEM_SEM_UNLOCK(unit, group)\
do {\
    if (osal_sem_mutex_give(hal_ctrl[unit].tbl_sem[group]) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "table semaphore unlock group %d failed", group);\
        return RT_ERR_FAILED;\
    }\
} while(0)

/* PHY semaphore handling */
#define PHY_SEM_LOCK(unit)\
do {\
    if (osal_sem_mutex_take(hal_ctrl[unit].phy_sem, OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "PHY semaphore lock failed");\
        return RT_ERR_FAILED;\
    }\
} while(0)

#define PHY_SEM_UNLOCK(unit)\
do {\
    if (osal_sem_mutex_give(hal_ctrl[unit].phy_sem) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "PHY semaphore unlock failed");\
        return RT_ERR_FAILED;\
    }\
} while(0)


#define PHY_BUSY_WAIT_LOOP(unit, REG, MASK)\
    do {\
        osal_usecs_t    t, now, t_wait=0;\
        uint32 regVal;\
        int32   chkCnt=0;\
        osal_time_usecs_get(&t);\
        do {\
            chkCnt++;\
            if (reg_read(unit, REG, &regVal) != RT_ERR_OK)\
            {\
                PHY_SEM_UNLOCK(unit);\
                return RT_ERR_FAILED;\
            }\
            if (0 == (regVal & MASK))\
            {\
                break;\
            }\
            osal_time_usecs_get(&now);\
            t_wait += ((now >= t) ? (now - t) : (0xFFFFFFFF - t + now));\
            t = now;\
        } while (t_wait <= ACCESS_PHY_TIMEOUT_TIME);\
        if (t_wait > ACCESS_PHY_TIMEOUT_TIME)\
        {\
            PHY_SEM_UNLOCK(unit);\
            return RT_ERR_BUSYWAIT_TIMEOUT;\
        }\
    } while (0)


#define PHY_WAIT_CMD_COMPLETE(unit, REG, COMP_MASK, FAIL_MASK)\
    do {\
        int32   rv;\
        if ((rv = drv_phy_smiCmdComplete_wait(unit, REG, COMP_MASK, FAIL_MASK, ACCESS_PHY_TIMEOUT_TIME)) != RT_ERR_OK)\
        {\
            PHY_SEM_UNLOCK(unit);\
            return rv;\
        }\
    } while(0)


/*
 * Symbol Definition
 */



/*
 * Function Declaration
 */

#if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
/* Function Name:
 *      table_name_get
 * Description:
 *      Get table name of the specified register index.
 * Input:
 *      unit  - unit id
 *      table   - register index
 * Output:
 *      pData - pointer buffer of table name
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      None
 */
extern int32
table_name_get(uint32 unit, uint32 table, char *pData);
#endif

/* Function Name:
 *      rtl83xx_mdcSem_callback
 * Description:
 *
 * Input:
 *      unit  - unit id
 *      type  -
 * Output:
 *      pData - pointer buffer of table name
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
 extern int32 rtl83xx_mdcSem_callback(uint32 unit, uint32 type);


/* Function Name:
 *      drv_phy_smiCmdComplete_wait
 * Description:
 *      MAC access PHY wait command complete
 * Input:
 *      unit  - unit id
 *      ctrl_reg  - SMI access PHY control reg
 *      cmd_mask  - command bit mask
 *      fail_mask - fail bit mask
 *      timeout   - timeout time in milli-second
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
extern int32
drv_phy_smiCmdComplete_wait(int32 unit, uint32 ctrl_reg, uint32 cmd_mask, uint32 fail_mask, uint32 timeout);

#endif  /* __HAL_MAC_DRV_H__ */

