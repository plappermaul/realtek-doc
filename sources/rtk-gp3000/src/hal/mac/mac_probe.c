/*
 * Copyright (C) 2011 Realtek Semiconductor Corp.
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
#include <ioal/mem32.h>
#include <hal/chipdef/chip.h>
#include <hal/chipdef/driver.h>
#include <hal/common/halctrl.h>
#include <hal/mac/mac_probe.h>
#include <hal/mac/mac_probe_tool.h>

#if defined(CONFIG_SDK_APOLLOMP)
#include <dal/apollomp/dal_apollomp_tool.h>
#include <dal/apollomp/dal_apollomp_switch.h>
#endif

#if defined(CONFIG_SDK_RTL9602C)
#include <dal/rtl9602c/dal_rtl9602c_switch.h>
#endif

#if defined(CONFIG_SDK_RTL9607C)
#include <dal/rtl9607c/dal_rtl9607c_tool.h>
#include <dal/rtl9607c/dal_rtl9607c_switch.h>
#endif

#if defined(CONFIG_SDK_RTL9603CVD)
#include <dal/rtl9603cvd/dal_rtl9603cvd_tool.h>
#include <dal/rtl9603cvd/dal_rtl9603cvd_switch.h>
#endif

/*
 * Symbol Definition
 */
#define VALUE_NO_INIT   (-1)


/*
 * Macro Definition
 */
#define HAL_ADD_PORT(pDev, portType, port)\
do {\
    pDev->pPortinfo->portType.portNum++;\
    if (pDev->pPortinfo->portType.min < 0) { pDev->pPortinfo->portType.min = port;}\
    if (port > pDev->pPortinfo->portType.max) {pDev->pPortinfo->portType.max = port;}\
    RTK_PORTMASK_PORT_SET(pDev->pPortinfo->portType.portmask, port);\
} while(0);

#define HAL_DEL_PORT(pDev, portType, port)\
do {\
    RTK_PORTMASK_PORT_CLEAR(pDev->pPortinfo->portType.portmask, port);\
    pDev->pPortinfo->portType.portNum--;\
    if (pDev->pPortinfo->portType.portNum == 0) { pDev->pPortinfo->portType.min = pDev->pPortinfo->portType.max = VALUE_NO_INIT;}\
    if (port == pDev->pPortinfo->portType.min) { pDev->pPortinfo->portType.min = RTK_PORTMASK_GET_FIRST_PORT(pDev->pPortinfo->portType.portmask);}\
    if (port == pDev->pPortinfo->portType.max) {pDev->pPortinfo->portType.max = RTK_PORTMASK_GET_LAST_PORT(pDev->pPortinfo->portType.portmask);}\
} while(0);

/*
 * Function Declaration
 */
static int32 _hal_portInfo_init(rt_device_t *pDev);


/*
 * Symbol Definition
 */
extern rt_device_t apollo_device;
/*
 * Macro Definition
 */




/* Function Name:
 *      drv_swcore_cid_get
 * Description:
 *      Get chip id and chip revision id.
 * Input:
 *      None
 * Output:
 *      pChip_id       - pointer buffer of chip id
 *      pChip_rev_id   - pointer buffer of chip revision id
 * Return:
 *      RT_ERR_OK      - OK
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32
drv_swcore_cid_get(uint32 *pChip_id, uint32 *pChip_rev_id)
{
    /* parameter check */
    RT_PARAM_CHK((NULL == pChip_id), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pChip_rev_id), RT_ERR_NULL_POINTER);

#if defined(FORCE_PROBE_APOLLO)

    *pChip_id = APOLLO_CHIP_ID;
    *pChip_rev_id = CHIP_REV_ID_0;

#elif defined(FORCE_PROBE_APOLLO_REV_B)

    *pChip_id = APOLLO_CHIP_ID;
    *pChip_rev_id = CHIP_REV_ID_A;

#elif defined(FORCE_PROBE_APOLLOMP)

    *pChip_id = APOLLOMP_CHIP_ID;
    *pChip_rev_id = CHIP_REV_ID_A;

#elif defined(FORCE_PROBE_APOLLOMP_REV_B)

    *pChip_id = APOLLOMP_CHIP_ID;
    *pChip_rev_id = CHIP_REV_ID_B;
#elif defined(FORCE_PROBE_RTL9601B)

    *pChip_id = RTL9601B_CHIP_ID;
    *pChip_rev_id = CHIP_REV_ID_0;
#elif defined(FORCE_PROBE_RTL9602C)

    *pChip_id = RTL9602C_CHIP_ID;
    *pChip_rev_id = CHIP_REV_ID_A;
#elif defined(FORCE_PROBE_RTL9602C_REV_B)

    *pChip_id = RTL9602C_CHIP_ID;
    *pChip_rev_id = CHIP_REV_ID_B;
#elif defined(FORCE_PROBE_RTL9607C)

    *pChip_id = RTL9607C_CHIP_ID;
    *pChip_rev_id = CHIP_REV_ID_A;
#elif defined(FORCE_PROBE_RTL9607C_REV_B)

    *pChip_id = RTL9607C_CHIP_ID;
    *pChip_rev_id = CHIP_REV_ID_B;
#elif defined(CONFIG_SDK_LUNA_G3)

    *pChip_id = LUNA_G3_CHIP_ID;
    *pChip_rev_id = CHIP_REV_ID_0;
#elif defined(CONFIG_SDK_CA8279)

    *pChip_id = CA8279_CHIP_ID;
    *pChip_rev_id = CHIP_REV_ID_0;
#elif defined(CONFIG_SDK_CA8277B)

    *pChip_id = CA8277B_CHIP_ID;
    *pChip_rev_id = CHIP_REV_ID_0;
#elif defined(CONFIG_SDK_RTL8277C)

    *pChip_id = RTL8277C_CHIP_ID;
    *pChip_rev_id = CHIP_REV_ID_0;
#elif defined(CONFIG_SDK_RTL9607F)

    *pChip_id = RTL9607F_CHIP_ID;
    *pChip_rev_id = CHIP_REV_ID_0;
#elif defined(FORCE_PROBE_RTL9603CVD)

    *pChip_id = RTL9603CVD_CHIP_ID;
    *pChip_rev_id = CHIP_REV_ID_A;
#else
    mac_probe_tool_cid_get(pChip_id, pChip_rev_id);
#endif
     return RT_ERR_OK;
}
#if defined(RTL_CYGWIN_EMULATE)
#else
/* Function Name:
 *      _hal_portInfo_prepare
 * Description:
 *      Modify port info base on chip sub type (9601 and 9603)
 * Input:
 *      pDev - pointer buffer of device database for inited
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK - OK
 * Note:
 */
 static int32
_hal_portInfo_prepare(rt_device_t *pDev)
{

#if defined(CONFIG_SDK_APOLLOMP)
    uint32  chipSubtype;
    /* 9601 and 9603 need to change the port information */
    if(APOLLOMP_CHIP_ID == pDev->chip_id)
    {
        if(dal_apollomp_switch_get_chipSubType(&chipSubtype) == RT_ERR_OK)
        {
            switch(chipSubtype)
            {
                case APPOLOMP_CHIP_SUB_TYPE_RTL9601: /* 9601 UTP port is port 2 */
                    pDev->pPortinfo->portType[0] = RT_PORT_NONE;
                    pDev->pPortinfo->portType[1] = RT_PORT_NONE;
                    pDev->pPortinfo->portType[3] = RT_PORT_NONE;
                break;

                case APPOLOMP_CHIP_SUB_TYPE_RTL9602B: /* 9602 UTP port is port 2,3 */
                    pDev->pPortinfo->portType[0] = RT_PORT_NONE;
                    pDev->pPortinfo->portType[1] = RT_PORT_NONE;
                    pDev->pPortinfo->portType[2] = RT_FE_PORT;
                    pDev->pPortinfo->portType[3] = RT_GE_PORT;
                break;

                case APPOLOMP_CHIP_SUB_TYPE_RTL9602: /* 9602 UTP port 0-2 FE, 3 GE */
                case APPOLOMP_CHIP_SUB_TYPE_RTL9603: /* 9603 UTP port 0-2 FE, 3 GE */
                    pDev->pPortinfo->portType[0] = RT_FE_PORT;
                    pDev->pPortinfo->portType[1] = RT_FE_PORT;
                    pDev->pPortinfo->portType[2] = RT_FE_PORT;
                    pDev->pPortinfo->portType[3] = RT_GE_PORT;
                break;
                default:
                    break;
            }
        }
    }
#endif


#if defined(CONFIG_SDK_RTL9601B)
    /* 9601B no need to change port information, now. */
    if(RTL9601B_CHIP_ID == pDev->chip_id)
    {
    }
#endif

#if defined(CONFIG_SDK_RTL9602C)
    uint32  chipId,rev,subType  = 0;
    /* 9601C need to change port information*/
    if(RTL9602C_CHIP_ID == pDev->chip_id)
    {
    #if defined(CONFIG_FORCE_SDK_RTL9602C_1GSFU)
        pDev->pPortinfo->portType[0] = RT_PORT_NONE;
        pDev->pPortinfo->portType[1] = RT_GE_PORT;
    #else
        if(dal_rtl9602c_switch_version_get(&chipId,&rev,&subType) == RT_ERR_OK)
        {
            switch(subType)
            {
                case RTL9602C_CHIP_SUB_TYPE_RTL9601C:
                case RTL9602C_CHIP_SUB_TYPE_RTL9601C_VB:
                    pDev->pPortinfo->portType[0] = RT_GE_PORT;
                    pDev->pPortinfo->portType[1] = RT_PORT_NONE;
                break;
                case RTL9602C_CHIP_SUB_TYPE_RTL9601D_VA3:
                case RTL9602C_CHIP_SUB_TYPE_RTL9601D_VA4:
        #if defined(CONFIG_9601D_SFU)
                    pDev->pPortinfo->portType[0] = RT_PORT_NONE;
                    pDev->pPortinfo->portType[1] = RT_GE_PORT;
        #elif defined(CONFIG_9601D_SFP)
                    pDev->pPortinfo->portType[0] = RT_GE_PORT;
                    pDev->pPortinfo->portType[1] = RT_PORT_NONE;
        #elif defined(CONFIG_9601D_HGU)
                    pDev->pPortinfo->portType[0] = RT_PORT_NONE;
                    pDev->pPortinfo->portType[1] = RT_GE_PORT;
        #else
                    pDev->pPortinfo->portType[0] = RT_GE_PORT;
                    pDev->pPortinfo->portType[1] = RT_GE_PORT;
        #endif
                break;

                default:
                    break;
            }
        }
    #endif
    }
#endif

#if defined(CONFIG_SDK_RTL9607C)
    /* Change port information according to actual IC's port design */
    if(RTL9607C_CHIP_ID == pDev->chip_id)
    {
        uint32 i;
        uint32 capability = 0;

        if(dal_rtl9607c_tool_get_capability(&capability) == RT_ERR_OK)
        {
            for(i = 0 ; i < 5 ; i ++)
            {
                if(capability & (1<<i))
                {
                    pDev->pPortinfo->portType[i] = RT_PORT_NONE;
                }
                else if(capability & (1<<(i+5)))
                {
                    pDev->pPortinfo->portType[i] = RT_FE_PORT;
                }
            }
        }
    }
#endif

#if defined(CONFIG_SDK_RTL9603CVD)
    /* Change port information according to actual IC's port design */
    if(RTL9603CVD_CHIP_ID == pDev->chip_id)
    {
        uint32 i;
        uint32 capability = 0;

        if(dal_rtl9603cvd_tool_get_capability(&capability) == RT_ERR_OK)
        {
            for(i = 0 ; i < 4 ; i ++)
            {
                if(capability & (1<<i))
                {
                    pDev->pPortinfo->portType[i] = RT_PORT_NONE;
                }
                else if(i == 3 && (capability & (1<<4)))
                {
                    pDev->pPortinfo->portType[i] = RT_FE_PORT;
                }
            }
        }
    }
#endif

#if defined(CONFIG_SDK_CA8279) || defined(CONFIG_SDK_CA8277B) || defined(CONFIG_SDK_RTL8277C) || defined(CONFIG_SDK_RTL9607F)
#if defined(CONFIG_RG_G3_L2FE_POL_OFFLOAD)   //loop back mode reserve port 5 resource to port 6
    pDev->pPortinfo->portType[5] = RT_PORT_NONE;
    pDev->pPortinfo->portType[6] = RT_PORT_NONE;
#endif
#endif

    return RT_ERR_OK;
} /* end of _hal_portInfo_prepare */
#endif

/* Function Name:
 *      _hal_portInfo_init
 * Description:
 *      Init port info, it will be used to fill port mask, max, min value for each port type
 * Input:
 *      pDev - pointer buffer of device database for inited
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK - OK
 * Note:
 */
static int32
_hal_portInfo_init(rt_device_t *pDev)
{
    int32  port;
    rt_portType_info_t sdsPortBakup;
#if defined(RTL_CYGWIN_EMULATE)
#else

    _hal_portInfo_prepare(pDev);
#endif
    /* clear port info database */
    pDev->pPortinfo->port_number = 0;
    pDev->pPortinfo->cpuPort = VALUE_NO_INIT;

    pDev->pPortinfo->fe.portNum = 0;
    pDev->pPortinfo->fe.max = VALUE_NO_INIT;
    pDev->pPortinfo->fe.min = VALUE_NO_INIT;
    osal_memset(&(pDev->pPortinfo->fe.portmask), 0, sizeof(rtk_portmask_t));

    pDev->pPortinfo->fe_int.portNum = 0;
    pDev->pPortinfo->fe_int.max = VALUE_NO_INIT;
    pDev->pPortinfo->fe_int.min = VALUE_NO_INIT;
    osal_memset(&(pDev->pPortinfo->fe_int.portmask), 0, sizeof(rtk_portmask_t));

    pDev->pPortinfo->ge.portNum = 0;
    pDev->pPortinfo->ge.max = VALUE_NO_INIT;
    pDev->pPortinfo->ge.min = VALUE_NO_INIT;
    osal_memset(&(pDev->pPortinfo->ge.portmask), 0, sizeof(rtk_portmask_t));

    pDev->pPortinfo->ether.portNum = 0;
    pDev->pPortinfo->ether.max = VALUE_NO_INIT;
    pDev->pPortinfo->ether.min = VALUE_NO_INIT;
    osal_memset(&(pDev->pPortinfo->ether.portmask), 0, sizeof(rtk_portmask_t));

    pDev->pPortinfo->ge_combo.portNum = 0;
    pDev->pPortinfo->ge_combo.max = VALUE_NO_INIT;
    pDev->pPortinfo->ge_combo.min = VALUE_NO_INIT;
    osal_memset(&(pDev->pPortinfo->ge_combo.portmask), 0, sizeof(rtk_portmask_t));

    osal_memcpy(&sdsPortBakup, &pDev->pPortinfo->serdes, sizeof(rt_portType_info_t)); /* backup for SDS combo ports */
    pDev->pPortinfo->serdes.portNum = 0;
    pDev->pPortinfo->serdes.max = VALUE_NO_INIT;
    pDev->pPortinfo->serdes.min = VALUE_NO_INIT;
    osal_memset(&(pDev->pPortinfo->serdes.portmask), 0, sizeof(rtk_portmask_t));

    pDev->pPortinfo->cpu.portNum = 0;
    pDev->pPortinfo->cpu.max = VALUE_NO_INIT;
    pDev->pPortinfo->cpu.min = VALUE_NO_INIT;
    osal_memset(&(pDev->pPortinfo->cpu.portmask), 0, sizeof(rtk_portmask_t));

    pDev->pPortinfo->all.portNum = 0;
    pDev->pPortinfo->all.max = VALUE_NO_INIT;
    pDev->pPortinfo->all.min = VALUE_NO_INIT;
    osal_memset(&(pDev->pPortinfo->all.portmask), 0, sizeof(rtk_portmask_t));

    pDev->pPortinfo->swpbo_lb.portNum = 0;
    pDev->pPortinfo->swpbo_lb.max = VALUE_NO_INIT;
    pDev->pPortinfo->swpbo_lb.min = VALUE_NO_INIT;
    osal_memset(&(pDev->pPortinfo->swpbo_lb.portmask), 0, sizeof(rtk_portmask_t));

    pDev->pPortinfo->xe.portNum = 0;
    pDev->pPortinfo->xe.max = VALUE_NO_INIT;
    pDev->pPortinfo->xe.min = VALUE_NO_INIT;
    osal_memset(&(pDev->pPortinfo->xe.portmask), 0, sizeof(rtk_portmask_t));

    /* Check each port type and fill related info */
    for (port = 0; port < RTK_MAX_NUM_OF_PORTS; port++)
    {
        switch (pDev->pPortinfo->portType[port])
        {
            case RT_INT_FE_PORT:
                HAL_ADD_PORT(pDev, fe_int, port);
                HAL_ADD_PORT(pDev, fe, port);
                HAL_ADD_PORT(pDev, ether, port);
                HAL_ADD_PORT(pDev, all, port);
                break;

            case RT_FE_PORT:
                HAL_ADD_PORT(pDev, fe, port);
                HAL_ADD_PORT(pDev, ether, port);
                HAL_ADD_PORT(pDev, all, port);
                break;

            case RT_GE_PORT:
                HAL_ADD_PORT(pDev, ge, port);
                HAL_ADD_PORT(pDev, ether, port);
                HAL_ADD_PORT(pDev, all, port);
                break;

            case RT_GE_SERDES_PORT:
                HAL_ADD_PORT(pDev, serdes, port);
                HAL_ADD_PORT(pDev, all, port);
                break;

            case RT_CPU_PORT:
#if defined(CONFIG_SDK_RTL9607C)
                if (RTL9607C_CHIP_ID == pDev->chip_id)
                    pDev->pPortinfo->cpuPort = 9;
                else
#endif
#if defined(CONFIG_LUNA_G3_SERIES)
                pDev->pPortinfo->cpuPort = 16;
#else
                pDev->pPortinfo->cpuPort = port;
#endif
                HAL_ADD_PORT(pDev, cpu, port);
                HAL_ADD_PORT(pDev, all, port);
                break;
            case RT_SWPBO_PORT:
                HAL_ADD_PORT(pDev, swpbo_lb, port);
                break;

            case RT_XE_PORT:
                HAL_ADD_PORT(pDev, xe, port);
                HAL_ADD_PORT(pDev, ether, port);
                HAL_ADD_PORT(pDev, all, port);
                break;

            default:
                continue;
        }

        pDev->pPortinfo->port_number++;
    }

    /* recover for SDS combo ports */
    osal_memcpy(&pDev->pPortinfo->serdes, &sdsPortBakup, sizeof(rt_portType_info_t));


    return RT_ERR_OK;
} /* end of _hal_portInfo_init */


/*
 * Function Declaration
 */

/* Function Name:
 *      mac_probe
 * Description:
 *      Probe the MAC chip in the specified chip.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
int32
mac_probe(void)
{
    int32   ret = RT_ERR_FAILED;
    uint32  temp_chip_id = 0, temp_chip_rev_id = 0;
    rt_device_t *pDev = NULL;
#if !defined(CONFIG_LUNA_G3_SERIES)
    rt_driver_t *pMdriver = NULL;
#endif


    /* Get chip_id/chip_rev_id and check the value */
    if ((ret = drv_swcore_cid_get(&temp_chip_id, &temp_chip_rev_id)) != RT_ERR_OK)
    {
        RT_ERR(ret, MOD_HAL, "drv_swcore_cid_get failed!!");
        return ret;
    }

    hal_ctrl.chip_id = temp_chip_id;
    hal_ctrl.chip_rev_id = temp_chip_rev_id;


    RT_LOG(LOG_INFO, MOD_HAL, "chip_id=0x%X, rev_id=0x%X",
           hal_ctrl.chip_id, hal_ctrl.chip_rev_id);


    /* Find device */
    if ((pDev = hal_find_device(hal_ctrl.chip_id, hal_ctrl.chip_rev_id)) == NULL)
    {
        RT_DBG(LOG_MINOR_ERR, MOD_HAL, "hal_find_device(chip_id=0x%X, rev_id=0x%X) failed!!",\
               hal_ctrl.chip_id, hal_ctrl.chip_rev_id);

        return RT_ERR_CHIP_NOT_FOUND;
    }

    hal_ctrl.pDev_info = pDev;
#if defined(CONFIG_LUNA_G3_SERIES)
    //Luna G3 don't need mac driver
    hal_ctrl.pChip_driver = NULL;
#else
    /* Find chip major driver */
    if ((pMdriver = hal_find_driver(hal_ctrl.pDev_info->driver_id,\
        hal_ctrl.pDev_info->driver_rev_id)) == NULL)
    {
        RT_DBG(LOG_MINOR_ERR, MOD_HAL, "hal_find_driver(drv_id=0x%X, drv_rev_id=0x%X) failed!!",\
               hal_ctrl.pDev_info->driver_id, hal_ctrl.pDev_info->driver_rev_id);
        return RT_ERR_DRIVER_NOT_FOUND;
    }

    hal_ctrl.pChip_driver = pMdriver;
#endif
    hal_ctrl.chip_flags |= HAL_CHIP_ATTACHED;


    /* Initialize port information database */
    if ((ret = _hal_portInfo_init(hal_ctrl.pDev_info)) != RT_ERR_OK)
    {
        RT_ERR(ret, MOD_HAL, "_hal_portInfo_init() failed!!");
        return ret;
    }

    return RT_ERR_OK;
} /* end of mac_probe */


/* Function Name:
 *      mac_init
 * Description:
 *      Probe the MAC chip in the specified chip.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
int32
mac_init(void)
{
    int32   ret = RT_ERR_FAILED;


    /* Initialize MAC */
    if ((ret = hal_ctrl.pChip_driver->pMacdrv->fMdrv_init()) != RT_ERR_OK)
    {
        RT_ERR(ret, MOD_HAL, "mac driver init failed!!");
        return ret;
    }
    hal_ctrl.chip_flags |= HAL_CHIP_INITED;
    return RT_ERR_OK;
} /* end of mac_init */
#if defined(CONFIG_SDK_KERNEL_LINUX)
EXPORT_SYMBOL(drv_swcore_cid_get);
#endif
