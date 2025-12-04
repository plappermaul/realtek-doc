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
 * $Revision$
 * $Date$
 *
 * Purpose : Define diag shell database.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) Diag shell database.
 */
 
#include <stdio.h>
#include <string.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <rtk/switch.h>
#include <diag_om.h>

static int  current_chip_id = DIAG_OM_CHIP_ID_DEFAULT;
static rtk_switch_devInfo_t chip_devInfo;

int32
diag_om_get_chip_id(int *chipid)
{
    if (NULL == chipid)
        return RT_ERR_FAILED;
        
    *chipid = current_chip_id;
    return RT_ERR_OK; 
} /* end of diag_get_chip_id */

int32
diag_om_set_chip_id(int chipid)
{
    if (chipid > DIAG_OM_CHIP_ID_MAX)
        return RT_ERR_FAILED;
        
    current_chip_id = chipid;
    return RT_ERR_OK; 
} /* end of diag_set_chip_id */

int32
diag_om_get_deviceInfo(rtk_switch_devInfo_t *pDevInfo)
{
  
    if (NULL == pDevInfo)
        return RT_ERR_FAILED;
        
    memcpy(pDevInfo, &chip_devInfo, sizeof(rtk_switch_devInfo_t));
    return RT_ERR_OK; 
} /* end of diag_om_get_deviceInfo */

static rt_portType_info_t portInfo;
int32
diag_om_set_deviceInfo(void)
{
    rtk_switch_devInfo_t devInfo;


    memset(&devInfo, 0, sizeof(rtk_switch_devInfo_t));
#ifdef CONFIG_XDSL_RG_DIAGSHELL
    portInfo.portNum = 7;
    portInfo.max = 5;
    portInfo.min = 0;
    portInfo.portmask.bits[0] = 0x7f;

    devInfo.chipId    = 0x8685;
    devInfo.revision  = 0xc;
    devInfo.port_number = 7;
    memcpy(&devInfo.fe,&portInfo,sizeof(portInfo));
    memcpy(&devInfo.ge,&portInfo,sizeof(portInfo));
    memcpy(&devInfo.ether,&portInfo,sizeof(portInfo));
    memcpy(&devInfo.all,&portInfo,sizeof(portInfo));
#else
    if (rtk_switch_deviceInfo_get(&devInfo) != RT_ERR_OK)
    {
        return RT_ERR_FAILED;
    }
#endif    
    memcpy(&chip_devInfo, &devInfo, sizeof(rtk_switch_devInfo_t));
    return RT_ERR_OK; 
} /* end of diag_om_set_deviceInfo */



