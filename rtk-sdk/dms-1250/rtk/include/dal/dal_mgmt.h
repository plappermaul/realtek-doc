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
 * Purpose : Use to Management each device
 *
 * Feature : The file have include the following module and sub-modules
 *           1) Initialize system
 *           2) Initialize device
 *           3) Mangement Devices
 *
 */
#ifndef __DAL_MGMT_H__
#define __DAL_MGMT_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <dal/dal_mapper.h>
#include <osal/sem.h>

/*
 * Symbol Definition
 */
typedef void (*pMapper_init_t) (dal_mapper_t*);
typedef int32 (*p_t) (void);


typedef struct dal_mgmt_info_s
{
    uint32          init_status;
    dal_mapper_t    *pMapper;
} dal_mgmt_info_t;

/*
 * Data Declaration
 */
extern dal_mgmt_info_t      *pMgmt_node[];
extern osal_mutex_t         mgmt_unit_sem;

/*
 * Macro Definition
 */
#define RT_MGMT(unit)       pMgmt_node[unit]
#define RT_MAPPER(unit)     RT_MGMT(unit)->pMapper

#define RTK_UNIT_LOCK()\
    do {\
        if (osal_sem_mutex_take(mgmt_unit_sem, OSAL_SEM_WAIT_FOREVER) != 0)\
            return RT_ERR_FAILED;\
    } while(0)

#define RTK_UNIT_UNLOCK()\
    do {\
        if (osal_sem_mutex_give(mgmt_unit_sem) != RT_ERR_OK)\
            return RT_ERR_FAILED;\
    } while(0)


#define RTK_DAL_MAP(func,unit,args...)\
    do{\
        dal_mapper_t *mapper;\
        RTK_UNIT_LOCK();\
        RT_PARAM_CHK( HWP_UNIT_VALID_STACK(unit), RT_ERR_UNIT_ID);\
        mapper = RT_MAPPER(unit);\
        RTK_UNIT_UNLOCK();\
        return mapper->func(unit,##args);\
    }while(0);



/*
 * Function Declaration
 */

/* Function Name:
 *      dal_mgmt_mapper_remap
 * Description:
 *      The unid ID of a chip has been changed. This function remaps the pMgmt_node[]
 * Input:
 *      original_unitID - the original unit ID
 *      new_unitID      - the new unit ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_FAILED   - changing failed
 *      RT_ERR_OK       - changing success
 * Note:
 *
 */
extern int32 dal_mgmt_mapper_remap(uint32 original_unitID, uint32 new_unitID);



/* Module Name : */

/* Function Name:
 *      dal_mgmt_init
 * Description:
 *      Initilize DAL(semaphore, database clear)
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_FAILED   - initialize fail
 *      RT_ERR_OK       - initialize success
 * Note:
 *      RTK must call this function before do other kind of action.
 */
extern int32
dal_mgmt_init(void);

/* Function Name:
 *      dal_mgmt_mapper_clear
 * Description:
 *      clear Mapper
 * Input:
 *      unit         - the unit to be initialized
 * Output:
 *      None
 * Return:
 *      NULL         - FAILED to find mapper for this chip
 *      dal_mapper_t - SUCCESSED to find mapper for this chip
 * Note:
 *      RTK must call this function before do other kind of action on this unit.
 */
extern int32
dal_mgmt_mapper_clear(uint32 unit);

/* Function Name:
 *      dal_mgmt_initDevice
 * Description:
 *      Initilize specified device(hook related driver, initialize database of device in MGMT,
 *      execute initialized function of each component
 * Input:
 *      unit            - unit ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_FAILED   - initialize fail
 *      RT_ERR_OK       - initialize success
 *      RT_ERR_UNIT_ID  - error unit id
 * Note:
 *      RTK must call this function before do other kind of action on this unit.
 */
int32 dal_mgmt_initDevice(uint32 unit);

/* Function Name:
 *      dal_mgmt_misc_init
 * Description:
 *      Initilize DAL miscellaneous items for whole system
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_FAILED   - initialize fail
 *      RT_ERR_OK       - initialize success
 * Note:
 *
 */
extern int32
dal_mgmt_misc_init(void);

#endif /* __DAL_MGMT_H__ */
