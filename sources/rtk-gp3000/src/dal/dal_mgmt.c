/*
 * Copyright (C) 2009 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/debug/rt_log.h>
#include <dal/dal_mgmt.h>
#include <dal/dal_mapper.h>

#if defined(CONFIG_SDK_APOLLO)
#include <dal/apollo/dal_apollo_mapper.h>
#endif

#if defined(CONFIG_SDK_APOLLOMP)
#include <dal/apollomp/dal_apollomp_mapper.h>
#endif

#if defined(CONFIG_SDK_RTL9601B)
#include <dal/rtl9601b/dal_rtl9601b_mapper.h>
#endif

#if defined(CONFIG_SDK_RTL9602C)
#include <dal/rtl9602c/dal_rtl9602c_mapper.h>
#endif

#if defined(CONFIG_SDK_RTL9607C)
#include <dal/rtl9607c/dal_rtl9607c_mapper.h>
#endif

#if defined(CONFIG_SDK_LUNA_G3)
#if defined(CONFIG_ARCH_RTL8198F)
#include <dal/rtl8198f/dal_rtl8198f_mapper.h>
#else
#include <dal/luna_g3/dal_luna_g3_mapper.h>
#endif
#endif

#if defined(CONFIG_SDK_CA8279)
#include <dal/ca8279/dal_ca8279_mapper.h>
#endif

#if defined(CONFIG_SDK_CA8277B)
#include <dal/ca8277b/dal_ca8277b_mapper.h>
#endif

#if defined(CONFIG_SDK_RTL8277C)
#include <dal/rtl8277c/dal_rtl8277c_mapper.h>
#endif

#if defined(CONFIG_SDK_RTL9607F)
#include <dal/rtl9607f/dal_rtl9607f_mapper.h>
#endif

#if defined(CONFIG_SDK_RTL9603CVD)
#include <dal/rtl9603cvd/dal_rtl9603cvd_mapper.h>
#endif

#include <hal/chipdef/chip.h>
#include <hal/common/halctrl.h>

#if defined(CONFIG_MULTICHIP_ONEIMAGE)
#if defined(CONFIG_SDK_KERNEL_LINUX)
#include <soc/cortina/cortina-soc.h>
#endif
#endif

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */

dal_mgmt_info_t         Mgmt_node;

dal_mgmt_info_t         *pMgmt_node;

static dal_mapper_info_t dal_mapper_database[] =
{
#if defined(CONFIG_SDK_APOLLO)
    {APOLLO_CHIP_ID,    NULL},
#endif
#if defined(CONFIG_SDK_APOLLOMP)
    {APOLLOMP_CHIP_ID,  NULL},
#endif
#if defined(CONFIG_SDK_RTL9601B)
    {RTL9601B_CHIP_ID,  NULL},
#endif
#if defined(CONFIG_SDK_RTL9602C)
    {RTL9602C_CHIP_ID,  NULL},
#endif
#if defined(CONFIG_SDK_RTL9607C)
    {RTL9607C_CHIP_ID,  NULL},
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
    {RTL9603CVD_CHIP_ID,  NULL},
#endif
#if defined(CONFIG_SDK_LUNA_G3)
    {LUNA_G3_CHIP_ID,  NULL},
#endif
#if defined(CONFIG_ARCH_RTL8198F)
    {RTL8198F_CHIP_ID,  NULL},
#endif
#if defined(CONFIG_SDK_CA8279)
    {CA8279_CHIP_ID,  NULL},
#endif
#if defined(CONFIG_SDK_CA8277B)
    {CA8277B_CHIP_ID,  NULL},
#endif
#if defined(CONFIG_SDK_RTL8277C)
    {RTL8277C_CHIP_ID,  NULL},
#endif
#if defined(CONFIG_SDK_RTL9607F)
    {RTL9607F_CHIP_ID,  NULL},
#endif
};

/*
 * Macro Declaration
 */

/*
 * Function Declaration
 */
static int32 dal_mgmt_attachDevice(void);
static dal_mapper_t *dal_mgmt_find_mapper(void);

/* Module Name : */

/* Function Name:
 *      dal_mgmt_initDevice
 * Description:
 *      Initilize specified device(hook related driver, initialize database of device in MGMT,
 *      execute initialized function of each component
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
int32 dal_mgmt_initDevice(void)
{
    int32   ret;

    /* Hook related driver and initialize database of device via dal_mgmt_attachDevice.
       If fail, return error code.
     */
    
    ret = dal_mgmt_attachDevice();
    if (RT_ERR_OK != ret){
        RT_DBG(LOG_MAJOR_ERR, (MOD_INIT|MOD_DAL), "dal_mgmt_attachDevice Failed!!");
        return ret;
    }
    RT_DBG(LOG_EVENT, (MOD_INIT|MOD_DAL), "dal_mgmt_attachDevice Completed!!");


    return ret;

} /* dal_mgmt_initDevice() */


/* Function Name:
 *      dal_mgmt_attachDevice
 * Description:
 *      Initilize specified device(hook related driver, initialize database of device in MGMT,
 *      execute initialized function of each component
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_FAILED       - initialize fail
 *      RT_ERR_OK           - initialize success
 * Note:
 *      RTK must call this function before do other kind of action.
 */
static int32 dal_mgmt_attachDevice(void)
{
    pMgmt_node=&Mgmt_node;


    if ((pMgmt_node->pMapper = dal_mgmt_find_mapper()) == NULL)
    {

        return RT_ERR_FAILED;
    }
    pMgmt_node->init = INIT_COMPLETED;

    return RT_ERR_OK;

} /* end of dal_mgmt_attachDevice */

#if defined(CONFIG_MULTICHIP_ONEIMAGE)
#if defined(CONFIG_SDK_KERNEL_LINUX)
extern uint32_t RTK_SOC_CHIP_ID;
#endif
#endif

/* Function Name:
 *      dal_mgmt_attachDevice
 * Description:
 *      Initilize specified device(hook related driver, initialize database of device in MGMT,
 *      execute initialized function of each component
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      NULL       - FAILED to find mapper for this chip
 * Note:
 *      RTK must call this function before do other kind of action.
 */
static dal_mapper_t *
dal_mgmt_find_mapper(void)
{
    
#if defined(CONFIG_MULTICHIP_ONEIMAGE)
#if defined(CONFIG_SDK_KERNEL_LINUX)
    if(RTK_SOC_CHIP_ID == CHIP_CA8277B)
    {
#if defined(CONFIG_SDK_CA8277B)
        return dal_ca8277b_mapper_get();
#else
        return NULL;
#endif
    }
    else if(RTK_SOC_CHIP_ID == CHIP_CA8277C)
    {
#if defined(CONFIG_SDK_RTL8277C)
        return dal_rtl8277c_mapper_get(); 
#else
        return NULL;
#endif
    }
    else if(RTK_SOC_CHIP_ID == CHIP_RTL9607F)
    {
#if defined(CONFIG_SDK_RTL9607F)
        return dal_rtl9607f_mapper_get(); 
#else
        return NULL;
#endif
    }
    else
        return NULL;
#endif
#endif

    uint32  mapper_size = sizeof(dal_mapper_database)/sizeof(dal_mapper_info_t);
    uint32  mapper_index;

    hal_control_t *pHal_ctrl = hal_ctrlInfo_get();

    /*mapper init*/
    for (mapper_index = 0; mapper_index < mapper_size; mapper_index++)
    {
        #if defined(CONFIG_SDK_APOLLO)
        if (APOLLO_CHIP_ID == dal_mapper_database[mapper_index].chip_id)
            dal_mapper_database[mapper_index].pMapper=dal_apollo_mapper_get();    
        #endif
        #if defined(CONFIG_SDK_APOLLOMP)
        if (APOLLOMP_CHIP_ID == dal_mapper_database[mapper_index].chip_id)
            dal_mapper_database[mapper_index].pMapper=dal_apollomp_mapper_get();    
        #endif
        #if defined(CONFIG_SDK_RTL9601B)
        if(RTL9601B_CHIP_ID == dal_mapper_database[mapper_index].chip_id)
            dal_mapper_database[mapper_index].pMapper=dal_rtl9601b_mapper_get();    
        #endif
        #if defined(CONFIG_SDK_RTL9602C)
        if(RTL9602C_CHIP_ID == dal_mapper_database[mapper_index].chip_id)
            dal_mapper_database[mapper_index].pMapper=dal_rtl9602c_mapper_get();    
        #endif
		#if defined(CONFIG_SDK_RTL9607C)
		if(RTL9607C_CHIP_ID == dal_mapper_database[mapper_index].chip_id)
            dal_mapper_database[mapper_index].pMapper=dal_rtl9607c_mapper_get();    
        #endif
		#if defined(CONFIG_SDK_RTL9603CVD)
		if(RTL9603CVD_CHIP_ID == dal_mapper_database[mapper_index].chip_id)
            dal_mapper_database[mapper_index].pMapper=dal_rtl9603cvd_mapper_get();
        #endif
        #if defined(CONFIG_SDK_LUNA_G3)
        if(LUNA_G3_CHIP_ID == dal_mapper_database[mapper_index].chip_id)
            dal_mapper_database[mapper_index].pMapper=dal_luna_g3_mapper_get();
        #endif
        #if defined(CONFIG_ARCH_RTL8198F)
        if(RTL8198F_CHIP_ID == dal_mapper_database[mapper_index].chip_id)
            dal_mapper_database[mapper_index].pMapper=dal_rtl8198f_mapper_get();
        #endif
        #if defined(CONFIG_SDK_CA8279)
        if(CA8279_CHIP_ID == dal_mapper_database[mapper_index].chip_id)
            dal_mapper_database[mapper_index].pMapper=dal_ca8279_mapper_get();
        #endif
        #if defined(CONFIG_SDK_CA8277B)
        if(CA8277B_CHIP_ID == dal_mapper_database[mapper_index].chip_id)
            dal_mapper_database[mapper_index].pMapper=dal_ca8277b_mapper_get();
        #endif
        #if defined(CONFIG_SDK_RTL8277C)
        if(RTL8277C_CHIP_ID == dal_mapper_database[mapper_index].chip_id)
            dal_mapper_database[mapper_index].pMapper=dal_rtl8277c_mapper_get();
        #endif
        #if defined(CONFIG_SDK_RTL9607F)
        if(RTL9607F_CHIP_ID == dal_mapper_database[mapper_index].chip_id)
            dal_mapper_database[mapper_index].pMapper=dal_rtl9607f_mapper_get();
        #endif
    }

    for (mapper_index = 0; mapper_index < mapper_size; mapper_index++)
    {
        if (dal_mapper_database[mapper_index].chip_id == pHal_ctrl->chip_id)
        {
            return dal_mapper_database[mapper_index].pMapper;
        }
    }

    return NULL;
} /* end of dal_mgmt_find_mapper */

