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

/*
 * Include Files
 */
#include <common/rt_autoconf.h>
#include <private/drv/swcore/swcore.h>
#include <private/drv/swcore/chip_probe.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/debug/rt_log.h>
#include <common/rt_chip.h>
#include <osal/sem.h>
#include <osal/lib.h>
#include <osal/memory.h>
#include <dal/dal_mgmt.h>
#include <dal/dal_mapper.h>
#include <dal/dal_linkMon.h>
#include <dal/dal_waMon.h>
#include <dal/dal_phy.h>
#include <dal/cypress/dal_cypress_mapper.h>
#include <dal/maple/dal_maple_mapper.h>
#include <dal/mango/dal_mango_mapper.h>
#include <dal/longan/dal_longan_mapper.h>
#include <dal/rtrpc/rtrpc_mapper.h>
#include <hal/chipdef/chip.h>
#include <hal/chipdef/chipdef.h>
#include <hal/common/halctrl.h>
#include <dal/rtrpc/rtrpc_mapper.h>

#if defined(CONFIG_SDK_RTL8390)
#include <dal/dal_linkFaultMon.h>
#endif

/*
 * Symbol Definition
 */
#undef  CONFIG_SDK_WA_LIMIT_LEARN_COUNT
#define CONFIG_SDK_WA_FORWARD_TABLE

/*
 * Data Declaration
 */
static uint32           mgmt_init = INIT_NOT_COMPLETED;
dal_mgmt_info_t         *pMgmt_node[RTK_MAX_NUM_OF_UNIT];
osal_mutex_t            mgmt_unit_sem = 0;
static osal_mutex_t     mgmt_sem = 0;
dal_mapper_t            *mgmt_dal_mapper[RT_CHIP_FAMILY_INDEX_END]={NULL};


#define SEM_LOCK()\
    do {\
        if (osal_sem_mutex_take(mgmt_sem, OSAL_SEM_WAIT_FOREVER) != 0)\
        {\
            RT_LOG(LOG_DEBUG, MOD_DAL, "semaphore lock failed");\
            return RT_ERR_FAILED;\
        }\
    } while(0)

#define SEM_UNLOCK()\
    do {\
        if (osal_sem_mutex_give(mgmt_sem) != RT_ERR_OK)\
        {\
            RT_LOG(LOG_DEBUG, MOD_DAL, "semaphore unlock failed");\
            return RT_ERR_FAILED;\
        }\
    } while(0)


/*
 * Function Declaration
 */


/* Function Name:
 *      dal_mgmt_mapper_hook
 * Description:
 *      Hook related mapper initialized function and execute it.
 * Input:
 *      unit    - the unit to be initialized
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - SUCCESSED to find mapper and initial for this chip
 *      RT_ERR_FAILED   - FAILED to find mapper and initial for this chip
 * Note:
 *      None
 */
static int32
dal_mgmt_mapper_hook(uint32 unit, dal_mapper_t *pMapper)
{
    pMapper_init_t dal_macMapper_init = NULL;

    /*Dal phy mapper init.*/
    dal_phyMapper_init(pMapper);

    /*Dal mac mapper function register.*/
    switch(HWP_CHIP_FAMILY_ID(unit))
    {
#if defined(CONFIG_SDK_RTL8380)
        case RTL8380_FAMILY_ID:
        case RTL8330_FAMILY_ID:
            dal_macMapper_init = dal_mapleMapper_init;
            break;
#endif

#if defined(CONFIG_SDK_RTL8390)
        case RTL8390_FAMILY_ID:
        case RTL8350_FAMILY_ID:
            dal_macMapper_init = dal_cypressMapper_init;
            break;
#endif

#if defined(CONFIG_SDK_RTL9300)
        case RTL9300_FAMILY_ID:
            dal_macMapper_init = dal_longanMapper_init;
            break;
#endif

#if defined(CONFIG_SDK_RTL9310)
        case RTL9310_FAMILY_ID:
            dal_macMapper_init = dal_mangoMapper_init;
            break;
#endif

        default:
            dal_macMapper_init = NULL;
            return RT_ERR_FAILED;
    }

#ifdef CONFIG_RISE
  #ifndef __BOOTLOADER__
    if(!HWP_UNIT_VALID_LOCAL(unit))
    {
        dal_macMapper_init = rtrpc_Mapper_init;
    }
  #endif
#endif

    /*Dal mac mapper init.*/
    dal_macMapper_init(pMapper);

    return RT_ERR_OK;
} /* end of dal_mgmt_mapper_hook */


/* Function Name:
 *      dal_mgmt_mapper_allocate
 * Description:
 *      Allocate memory for mapper.
 * Input:
 *      unit         - the unit ID
 * Output:
 *      None
 * Return:
 *      NULL         - FAILED to allocate
 *      dal_mapper_t - SUCCESSED
 * Note:
 *
 */
dal_mapper_t* dal_mgmt_mapper_allocate(uint32 unit)
{
    dal_mapper_t    *pMapper = NULL;
    uintptr         *fP;
    uint32          i = 0;

    pMapper = (dal_mapper_t *)osal_alloc(sizeof(dal_mapper_t));
    if (NULL == pMapper)
        return NULL;

    /* init all callback of mapper to a default function */
    fP = (uintptr *)pMapper;
    for (i = 0; i < sizeof(dal_mapper_t)/sizeof(void *); i++,  fP++)
        *fP = (uintptr)(dal_common_unavail);

    return pMapper;
}


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
int32 dal_mgmt_mapper_remap(uint32 original_unitID,uint32 new_unitID)
{
    rt_chip_family_index_t chipFamilyIndex;

    RTK_UNIT_LOCK();

    RT_MGMT(original_unitID)->init_status = INIT_NOT_COMPLETED;
    RT_MGMT(new_unitID)->init_status = INIT_NOT_COMPLETED;

    if(RT_ERR_OK != rt_chip_familyIndex_get(original_unitID,&chipFamilyIndex))
    {
        RTK_UNIT_UNLOCK();
        return RT_ERR_FAILED;
    }

    if(NULL == mgmt_dal_mapper[chipFamilyIndex])
    {
        RTK_UNIT_UNLOCK();
        return RT_ERR_FAILED;
    }

    //RT_MGMT(original_unitID)->pMapper = NULL;
    RT_MGMT(new_unitID)->pMapper = mgmt_dal_mapper[chipFamilyIndex];
    RT_MGMT(new_unitID)->init_status = INIT_COMPLETED;

    RTK_UNIT_UNLOCK();

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_mgmt_mapper_init
 * Description:
 *      Find mapper of this unit, or allocate and hook related driver for it.
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
//static
dal_mapper_t* dal_mgmt_mapper_init(uint32 unit)
{
    rt_chip_family_index_t chipFamilyIndex;

    if(RT_ERR_OK != rt_chip_familyIndex_get(unit,&chipFamilyIndex))
    {
        return NULL;
    }

    if(NULL == mgmt_dal_mapper[chipFamilyIndex])
    {
        mgmt_dal_mapper[chipFamilyIndex] = dal_mgmt_mapper_allocate(unit);
        if (NULL == mgmt_dal_mapper[chipFamilyIndex])
            return NULL;

        if (RT_ERR_OK != dal_mgmt_mapper_hook(unit,  mgmt_dal_mapper[chipFamilyIndex]))
        {
            osal_free(mgmt_dal_mapper[chipFamilyIndex]);
            return NULL;
        }
    }

    return mgmt_dal_mapper[chipFamilyIndex];

}

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
int32
dal_mgmt_mapper_clear(uint32 unit)
{
    if (RT_MGMT(unit) == NULL)
    {
        return RT_ERR_OK;
    }

    RTK_UNIT_LOCK();
    if (RT_MGMT(unit)->init_status == INIT_COMPLETED)
    {
        RT_MGMT(unit)->init_status = INIT_NOT_COMPLETED;
        RT_MGMT(unit)->pMapper = NULL;
    }
    RTK_UNIT_UNLOCK();

    return RT_ERR_OK;
}

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
int32 dal_mgmt_initDevice(uint32 unit)
{
    int32 ret=RT_ERR_OK;
    hal_control_t *pHal_ctrl;

    RT_INIT_MSG("  Mgmt_dev init (unit %d) \n",unit);

    /* Check whether device is exist in lower layer(MAL) */
    if ((pHal_ctrl = hal_ctrlInfo_get(unit)) == NULL)
    {
        RT_INIT_ERR(RT_ERR_CHIP_NOT_FOUND, (MOD_INIT|MOD_DAL), "Unit %d chip not found!", unit);
        return RT_ERR_CHIP_NOT_FOUND;
    }

    /* Hook related driver and initialize database of device */
    if (NULL == (RT_MAPPER(unit) = dal_mgmt_mapper_init(unit)))
    {
        RT_INIT_ERR(RT_ERR_FAILED, (MOD_INIT|MOD_DAL), "dal_mgmt_mapper_init Failed!!");
        return RT_ERR_FAILED;
    }
    RT_MGMT(unit)->init_status = INIT_COMPLETED;
    RT_LOG(LOG_EVENT, (MOD_INIT|MOD_DAL), "dal_mgmt_mapper_init Completed!!");


    /* Call init function of this device. */
    ret = RT_MAPPER(unit)->_init(unit);
    if (RT_ERR_OK != ret){
        RT_INIT_ERR(ret, (MOD_INIT|MOD_DAL), "RT_MAPPER(unit)->_init Failed!!");
        return ret;
    }
    RT_LOG(LOG_EVENT, (MOD_INIT|MOD_DAL), "RT_MAPPER(unit)->_init Completed!!");

#ifndef __BOOTLOADER__
#if !defined(__MODEL_USER__)
    if(HWP_UNIT_VALID_LOCAL(unit))
    {
        ret = dal_linkMon_devInit(unit);
        if (RT_ERR_OK != ret){
            RT_INIT_ERR(ret, (MOD_INIT|MOD_DAL), "dal_linkMon_devInit Failed!!");
            return ret;
        }
        RT_LOG(LOG_EVENT, (MOD_INIT|MOD_DAL), "dal_linkMon_devInit Completed!!");
    }
#endif
#endif //__BOOTLOADER__

    return RT_ERR_OK;

} /* dal_mgmt_initDevice() */


/* Function Name:
 *      dal_mgmt_misc_init
 * Description:
 *      Initilize miscellaneous items for whole system
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
int32 dal_mgmt_misc_init(void)
{
    int32 ret=RT_ERR_OK;

    /* Check  whether DAL MGMT was initialized */
    RT_INIT_CHK(mgmt_init);

#if defined(CONFIG_SDK_FPGA_PLATFORM)
    return RT_ERR_OK;
#endif

#if !defined (__MODEL_USER__)
    {
  #ifndef __BOOTLOADER__
        if ((ret = dal_waMon_init()) != RT_ERR_OK)
            return ret;

        ret = dal_waMon_enable();
        if ((RT_ERR_OK != ret) && (RT_ERR_THREAD_EXIST != ret))
        {
            RT_INIT_ERR(ret, (MOD_INIT|MOD_DAL), "dal_waMon_enable Failed!!");
            return ret;
        }
        ret = RT_ERR_OK;
        RT_LOG(LOG_EVENT, (MOD_INIT|MOD_DAL), "dal_waMon_enable Success!!");
  #endif
    }
#endif

    return ret;
}


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
int32 dal_mgmt_init(void)
{
    int unit;

    /* if was initilized, return RT_ERROR */
    RT_INIT_REENTRY_CHK(mgmt_init);

    /* Initialize the MGMT database */
    osal_memset(pMgmt_node, 0, sizeof(pMgmt_node));
    osal_memset(mgmt_dal_mapper, 0, sizeof(mgmt_dal_mapper));

    for(unit=0;unit<RTK_MAX_NUM_OF_UNIT;unit++)
    {
        /* Allocate memory for device database and initilize it. */
        pMgmt_node[unit] = (dal_mgmt_info_t *)osal_alloc(sizeof(dal_mgmt_info_t));
        if (0 == pMgmt_node[unit]){
            RT_LOG(LOG_DEBUG, MOD_DAL, "failed to create management node");
            return RT_ERR_FAILED;
        }
        osal_memset(pMgmt_node[unit], 0, sizeof(dal_mgmt_info_t));
    }

    /* create semaphore */
    mgmt_sem = osal_sem_mutex_create();
    if (0 == mgmt_sem){
        RT_INIT_ERR(RT_ERR_FAILED, (MOD_INIT|MOD_DAL), "mgmt_sem create failed");
        return RT_ERR_FAILED;
    }

    mgmt_unit_sem = osal_sem_mutex_create();
    if (0 == mgmt_unit_sem){
        RT_INIT_ERR(RT_ERR_FAILED, (MOD_INIT|MOD_DAL), "mgmt_unit_sem create failed");
        return RT_ERR_FAILED;
    }

#ifndef __BOOTLOADER__
    if (dal_linkMon_init() != RT_ERR_OK)
    {
        return RT_ERR_FAILED;
    }

  #if defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE)
    #if defined(CONFIG_SDK_RTL8390) && !defined(__MODEL_USER__)
    if (dal_linkFaultMon_init() != RT_ERR_OK)
    {
        return RT_ERR_FAILED;
    }
    #endif
  #endif
#endif

    /* if above thing work correct, Set Init flag on and return RT_ERROR_NONE */
    mgmt_init   = INIT_COMPLETED;

    return RT_ERR_OK;
} /* end of dal_mgmt_init */



