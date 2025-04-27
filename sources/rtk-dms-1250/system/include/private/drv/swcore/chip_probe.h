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
 * $Revision: 71708 $
 * $Date: 2016-09-19 11:31:17 +0800 (Mon, 19 Sep 2016) $
 *
 * Purpose : chip symbol and data type definition in the SDK.
 *
 * Feature : chip symbol and data type definition
 *
 */

#ifndef __DRV_SWCORE_CHIP_PROBE_H__
#define __DRV_SWCORE_CHIP_PROBE_H__

/*
 * Include Files
 */
#include <common/type.h>
#include <hal/chipdef/chip.h>
#include <ioal/mem32.h>


/*
 * Symbol Definition
 */

#define FAMILY_ID_MASK              (0xFFF00000)
#define FAMILY_ID(chip_id)          (drv_swcore_chipFamilyId_get(chip_id))

typedef struct cid_group_s
{
    uint32 cid;     /* Chip ID  */
    uint32 gid;     /* Group ID */
} cid_group_t;

typedef struct cid_prefix_group_s
{
    uint32 cid;
    uint32 gid;
} cid_prefix_group_t;
#define CID_PREFIX_MATCH(_g, _cid)    (_g.cid == FAMILY_ID(_cid))
#define CID_GROUP_NONE              (-1)


/*
 * Data Type Definition
 */

/*
 * Function Declaration
 */
void
drv_swcore_earlyInit_start(void);

void
drv_swcore_earlyInit_end(void);

void
drv_swcore_ioalCB_register(ioal_mem32_read_t read_f,ioal_mem32_write_t write_f);


/* Function Name:
 *      drv_swcore_chipFamilyId_get
 * Description:
 *      Get chip family id
 * Input:
 *      chip_id         - chip id (e.g. RTL9301_CHIP_ID,...)
 * Output:
 *      None
 * Return:
 *      chip family ID
 */
uint32
drv_swcore_chipFamilyId_get(uint32 chip_id);


/* Function Name:
 *      drv_swcore_cid_get
 * Description:
 *      Get chip id and chip revision id.
 * Input:
 *      unit           - unit id
 * Output:
 *      pChip_id       - pointer buffer of chip id
 *      pChip_rev_id   - pointer buffer of chip revision id
 * Return:
 *      RT_ERR_OK      - OK
 *      RT_ERR_UNIT_ID - invalid unit id
 *      RT_ERR_NULL_POINTER
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
drv_swcore_cid_get(uint32 unit, uint32 *pChip_id, uint32 *pChip_rev_id);

/* Function Name:
 *      drv_swcore_cid_cmp
 * Description:
 *      Compare cmp_id with the chip id of unit
 * Input:
 *      unit           - unit id
 *      cmp_id         - chip id for compare
 * Output:
 *      None
 * Return:
 *      0              - identical
 *      RT_ERR_FAILED  - not identical
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
drv_swcore_cid_cmp(uint32 unit, uint32 cmp_id);

/* Function Name:
 *      drv_swcore_family_cid_get
 * Description:
 *      Get Chip Family ID
 * Input:
 *      unit            - unit id
 * Output:
 *      pFamily_id      - Family_id
 * Return:
 *      RT_ERR_OK       - OK
 *      RT_ERR_FAILED   - Failed
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
drv_swcore_family_cid_get(uint32 unit, uint32 * pFamily_id);

#endif  /* __DRV_SWCORE_CHIP_PROBE_H__ */

