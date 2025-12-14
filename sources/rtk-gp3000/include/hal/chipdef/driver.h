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
 * Purpose : driver symbol and data type definition in the SDK.
 *
 * Feature : driver symbol and data type definition
 *
 */

#ifndef __HAL_CHIPDEF_DRIVER_H__
#define __HAL_CHIPDEF_DRIVER_H__

/*
 * Include Files
 */

#include <common/rt_type.h>
#include <hal/chipdef/allreg.h>
#include <hal/chipdef/allmem.h>
#include <hal/phy/phydef.h>
/*
 * Data Type Definition
 */

/* Definition type enum for major driver */
typedef enum rt_driver_type_e
{
#if defined(CONFIG_SDK_APOLLO)
    RT_DRIVER_APOLLO,
    RT_DRIVER_APOLLO_REV_B,
#endif
#if defined(CONFIG_SDK_APOLLOMP)
    RT_DRIVER_APOLLOMP_A,
#endif
#if defined(CONFIG_SDK_RTL9601B)
    RT_DRIVER_RTL9601B,
#endif
#if defined(CONFIG_SDK_RTL9602C)
    RT_DRIVER_RTL9602C,
#endif
#if defined(CONFIG_SDK_RTL9607C)
    RT_DRIVER_RTL9607C,
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
    RT_DRIVER_RTL9603CVD,
#endif
    RT_DRIVER_KNOWN,
    RT_DRIVER_END
} rt_driver_type_t;

/* Definition type enum for major driver */

/* Definition mac driver service APIs */
typedef struct rt_macdrv_s
{
    int32   (*fMdrv_init)(void);
    int32   (*fMdrv_miim_read)(rtk_port_t, uint32, uint32, uint32 *);
    int32   (*fMdrv_miim_write)(rtk_port_t, uint32, uint32, uint32);
    int32   (*fMdrv_table_read)(uint32, uint32, uint32 *);
    int32   (*fMdrv_table_write)(uint32, uint32, uint32 *);
    int32   (*fMdrv_table_clear)(uint32, uint32, uint32);
} rt_macdrv_t;

/* Define major driver structure */
typedef struct rt_driver_s
{
    rt_driver_type_t    type;
    uint32              driver_id;
    uint32              driver_rev_id;
    rtk_reg_t           *pReg_list;
    rtk_table_t         *pTable_list;
    rt_macdrv_t         *pMacdrv;
    uint32              reg_idx_max;
    uint32              regField_idx_max;
    uint32              table_idx_max;
} rt_driver_t;


/*
 * Data Declaration
 */

/* Declare the external major drivers */

extern rt_driver_t apollo_driver;
extern rt_driver_t apollo_driver_rev_b;
extern rt_driver_t apollomp_a_driver;

#if defined(CONFIG_SDK_RTL9601B)
extern rt_driver_t rtl9601b_a_driver;
#endif
#if defined(CONFIG_SDK_RTL9602C)
extern rt_driver_t rtl9602c_a_driver;
#endif
#if defined(CONFIG_SDK_RTL9607C)
extern rt_driver_t rtl9607c_a_driver;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
extern rt_driver_t rtl9603cvd_a_driver;
#endif

/*
 * Function Declaration
 */

/* Function Name:
 *      hal_find_driver
 * Description:
 *      Find the mac major driver from SDK supported driver lists.
 * Input:
 *      driver_id     - driver chip id
 *      driver_rev_id - driver chip revision id
 * Output:
 *      None
 * Return:
 *      NULL        - Not found
 *      Otherwise   - Pointer of mac driver structure that found
 * Note:
 *      The function have found the exactly driver from SDK supported driver lists.
 */
extern rt_driver_t *
hal_find_driver(uint32 driver_id, uint32 driver_rev_id);


#endif  /* __HAL_CHIPDEF_DRIVER_H__ */
