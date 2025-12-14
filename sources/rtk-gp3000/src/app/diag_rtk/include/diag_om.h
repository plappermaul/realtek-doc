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
 
#ifndef _DIAG_OM_H_
#define _DIAG_OM_H_

#include <flag/cmd_flag.h>

#define DIAG_OM_CHIP_ID_DEFAULT  (0)
#define DIAG_OM_CHIP_ID_MAX      (5)

#define DIAG_OM_GET_CHIP_ID(unit)\
do {\
    if (diag_om_get_chip_id(&(unit)) != RT_ERR_OK)\
    {\
        return CPARSER_NOT_OK;\
    }\
} while (0)

#define DIAG_OM_GET_CHIP_CAPACITY(capacity, capacity_name) \
do {\
    rtk_switch_devInfo_t    devInfo;\
    \
    if (diag_om_get_deviceInfo(&devInfo) != RT_ERR_OK)\
    {\
        (capacity) = 0;\
        break;\
    }\
    \
    (capacity) = devInfo.capacityInfo.capacity_name;\
} while (0)

int32 diag_om_get_chip_id(int *chipid);
int32 diag_om_set_chip_id(int chipid);
int32 diag_om_get_deviceInfo(rtk_switch_devInfo_t *pDevInfo);
int32 diag_om_set_deviceInfo(void);

#endif /* end of _DIAG_OM_H_ */
