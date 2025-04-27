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
 * Purpose : Define diag shell database.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) Diag shell database.
 */

#ifndef _DIAG_OM_H_
#define _DIAG_OM_H_

#define DIAG_OM_UNIT_ID_DEFAULT  (0)
#define DIAG_OM_UNIT_ID_MAX      RTK_MAX_UNIT_ID

typedef enum compatible_status_e
{
    DIAG_NOT_BACKWARD_COMPATIBLE,
    DIAG_BACKWARD_COMPATIBLE,
    DIAG_BACKWARD_COMPATIBLE_END
} compatible_status_t;

extern compatible_status_t  backwardCompatible_flag;
#define IS_BACKWARD_COMPATIBLE (backwardCompatible_flag == DIAG_BACKWARD_COMPATIBLE)
/*
 * Get chip unit ID
 */
#define DIAG_OM_GET_UNIT_ID(unit)\
do {\
    if (diag_om_get_unitId((int *)&(unit)) != RT_ERR_OK)\
    {\
        return CPARSER_NOT_OK;\
    }\
} while (0)

#define DIAG_OM_GET_CHIP_CAPACITY(unit, capacity, capacity_name) \
do {\
    rtk_switch_devInfo_t    devInfo;\
    \
    if (diag_om_get_deviceInfo((unit), &devInfo) != RT_ERR_OK)\
    {\
        (capacity) = 0;\
        break;\
    }\
    \
    (capacity) = devInfo.capacityInfo.capacity_name;\
} while (0)

#define DIAG_OM_GET_CHIPID(chipId)          (diag_om_get_chipId(unit, chipId))
#define DIAG_OM_GET_TESTCHIPID(chipId)      (diag_om_get_testChipId(unit, chipId))
#define DIAG_OM_GET_FAMILYID(familyId)      (diag_om_get_familyId(unit, familyId))

int32 diag_om_get_unitId(int *unit);
int32 diag_om_set_unitId(int unit);
int32 diag_om_get_deviceInfo(uint32 unit, rtk_switch_devInfo_t *pDevInfo);
int32 diag_om_set_deviceInfo(uint32 unit);
int32 diag_om_get_chipId(uint32 unit, uint32 chipId);
int32 diag_om_get_familyId(uint32 unit, uint32 familyId);
int32 diag_om_get_testChipId(uint32 unit, uint32 chipId);
int32 diag_om_get_promptString(uint8 *buf, uint32 bufSz, uint32 unit);
int32 diag_om_get_firstAvailableUnit(uint32 *unit);
int32 diag_om_set_backwardCompatible(compatible_status_t status);

#endif /* end of _DIAG_OM_H_ */
