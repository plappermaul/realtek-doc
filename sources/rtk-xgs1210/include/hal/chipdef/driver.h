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
 * $Revision: 89490 $
 * $Date: 2018-06-19 23:53:34 +0800 (Tue, 19 Jun 2018) $
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
#include <common/rt_autoconf.h>
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
#if defined(CONFIG_SDK_RTL8390)
    RT_DRIVER_RTL8390_A,
#endif
#if defined(CONFIG_SDK_RTL8380)
    RT_DRIVER_RTL8380_A,
#endif
#if defined(CONFIG_SDK_RTL9310)
    RT_DRIVER_RTL9310_A,
#endif
#if defined(CONFIG_SDK_RTL9300)
    RT_DRIVER_RTL9300_A,
#endif
    RT_DRIVER_END
} rt_driver_type_t;

/* Definition mac driver service APIs */
typedef struct rt_macdrv_s
{
    int32   (*fMdrv_init)(uint32);
    int32   (*fMdrv_miim_read)(uint32, rtk_port_t, uint32, uint32, uint32 *);
    int32   (*fMdrv_miim_write)(uint32, rtk_port_t, uint32, uint32, uint32);
    int32   (*fMdrv_miim_park_read)(uint32, rtk_port_t, uint32, uint32, uint32, uint32 *);
    int32   (*fMdrv_miim_park_write)(uint32, rtk_port_t, uint32, uint32, uint32, uint32);
    int32   (*fMdrv_miim_broadcast_write)(uint32, uint32, uint32, uint32);
    int32   (*fMdrv_miim_extParkPage_read)(uint32, rtk_port_t, uint32, uint32, uint32, uint32, uint32 *);
    int32   (*fMdrv_miim_extParkPage_write)(uint32, rtk_port_t, uint32,uint32, uint32, uint32, uint32);
    int32   (*fMdrv_miim_extParkPage_portmask_write)(uint32, rtk_portmask_t, uint32,uint32, uint32, uint32, uint32);
    int32   (*fMdrv_miim_mmd_read)(uint32, rtk_port_t, uint32, uint32, uint32 *);
    int32   (*fMdrv_miim_mmd_write)(uint32, rtk_port_t, uint32, uint32, uint32);
    int32   (*fMdrv_miim_mmd_portmask_write)(uint32, rtk_portmask_t, uint32, uint32, uint32);
    int32   (*fMdrv_table_read)(uint32, uint32, uint32, uint32 *);
    int32   (*fMdrv_table_write)(uint32, uint32, uint32, uint32 *);
    int32   (*fMdrv_port_probe)(uint32);
    int32   (*fMdrv_miim_portmask_write)(uint32, rtk_portmask_t, uint32, uint32, uint32);
    int32   (*fMdrv_miim_pollingEnable_get)(uint32, rtk_port_t, rtk_enable_t *);
    int32   (*fMdrv_miim_pollingEnable_set)(uint32, rtk_port_t, rtk_enable_t);
    int32   (*fMdrv_mac_serdes_rst)(uint32, uint32);
    int32   (*fMdrv_mac_serdes_read)(uint32, uint32, uint32, uint32, uint32 *);
    int32   (*fMdrv_mac_serdes_write)(uint32, uint32, uint32, uint32, uint32);
    int32   (*fMdrv_smi_read)(uint32, uint32, uint32, uint32, uint32, uint32 *);
    int32   (*fMdrv_smi_write)(uint32, uint32, uint32, uint32, uint32, uint32);
    int32   (*fMdrv_miim_portSmiMdxProto_get)(uint32 unit, rtk_port_t port, drv_smi_mdxProtoSel_t *pProto);
    int32   (*fMdrv_miim_portSmiMdxProto_set)(uint32 unit, rtk_port_t port, drv_smi_mdxProtoSel_t proto);
} rt_macdrv_t;

/* Define major driver structure */
typedef struct rt_driver_s
{
    rt_driver_type_t    type;
    uint32              driver_id;
    uint32              driver_rev_id;
    rtk_reg_t           **pReg_list;
#if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
    rtk_regName_t		**pReg_name_list;
#endif
    rtk_table_t         **pTable_list;
#if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
    rtk_tableName_t		**pTable_name_list;
#endif
    rtk_portmask_t      *pPortmask_list;
    rt_macdrv_t         *pMacdrv;
    uint32              reg_idx_max;
    uint32              regField_idx_max;
    uint32              table_idx_max;
    uint16              vendor_id;
} rt_driver_t;


/*
 * Data Declaration
 */

/* Declare the external major drivers */
#if defined(CONFIG_SDK_RTL8390)
extern rt_driver_t rtl8390_a_driver;
#endif
#if defined(CONFIG_SDK_RTL8380)
extern rt_driver_t rtl8380_a_driver;
#endif
#if defined(CONFIG_SDK_RTL9310)
extern rt_driver_t rtl9310_a_driver;
#endif
#if defined(CONFIG_SDK_RTL9300)
extern rt_driver_t rtl9300_a_driver;
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
