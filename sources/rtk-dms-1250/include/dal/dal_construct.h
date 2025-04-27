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
 * Purpose : Set the parameters of PHY/MAC to chips.
 *
 * Feature : Structure and APIs for parameters of PHY/MAC settings
 *
 */

#ifndef __CONSTRUCT_H__
#define __CONSTRUCT_H__

//#define CNSTRT_DEBUG        /* define to enable debug message */
//#define CNSTRT_DEBUG_REG    /* define to enable MAC/PHY register accessing debug message */

/*
 * Include Files
 */
#include <ioal/mem32.h>
#include <common/debug/rt_log.h>
#include <common/rt_autoconf.h>
#include <common/rt_type.h>
#include <common/error.h>
#include <common/debug/rt_log.h>
#include <soc/type.h>
#include <ioal/ioal_init.h>
#include <osal/time.h>
#include <hal/common/miim.h>
#include <hal/mac/reg.h>
#include <hal/mac/miim_common_drv.h>
#include <hal/common/halctrl.h>
#include <hal/phy/phydef.h>
#include <hal/phy/phy_common.h>
#include <osal/print.h>



/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */


/*
 * Macro Definition
 */
#ifdef CNSTRT_DEBUG
  #define CNSTRT_PRINT(fmt,args...)         osal_printf(fmt, ## args)
  #define CNSTRT_MSG(fmt,args...)           osal_printf(fmt, ## args)
#else
  #define CNSTRT_PRINT(fmt,args...)
  #define CNSTRT_MSG(fmt,args...)
#endif
#define CNSTRT_ERR(fmt,args...)             osal_printf(fmt, ## args)

extern int debug_regShow_set(int x);
extern int debug_regShow_get(void);

#define RTK_DEBUG_LEVEL 0





#define RTK_MII_READ(unit,port,p,r,pVal)            hal_miim_park_read(unit,port,p,0x1F,r,pVal)
#define RTK_MII_WRITE(unit,port,p,r,val)            hal_miim_park_write(unit,port,p,0x1F,r,val)
#define RTK_MII_PORTMASK_WRITE(unit, pm,p,r,val)    phy_reg_portmask_set(unit,pm,p,r,val)

#define RTK_SDS_PAGEREGVAL_END                  (0xFF)
#define RTK_MII_MAXPAGE8380                        (0xFFF)
#define RTK_MII_MAXPAGE8390                        (0x1FFF)
#define RTK_MII_MAXPAGE9300                        (0xFFF)
#define RTK_RTL8218B_TCA_CUT_A                     0x0
#define RTK_RTL8218B_TCA_CUT_B                     0x1
#define RTK_DBG_PRINT(level,fmt,args...)           do { if (level <= RTK_DEBUG_LEVEL) rt_log_printf(fmt, ## args); } while(0)
#define RTK_TOTAL_SERDES_8380                      6


typedef void (*constrcutFunc_t)(uint32 unit);
#define CONSTRUCT_DESCRIPTION_LEN           64
typedef struct construct_s{
    constrcutFunc_t pFunc;
    uint8 description[CONSTRUCT_DESCRIPTION_LEN];
} construct_t;

typedef struct {
    unsigned int    reg;
    unsigned int    offset;
    unsigned int    endBit;
    unsigned int    startBit;
    unsigned int    val;
} confcode_serdes_patch_t;


typedef struct {
    uint16   page;
    uint8    reg;
    uint16   data;
} sds_config;

typedef struct sds_tx_param_s {
    uint32 eyeParam_valid;
    rtk_sds_eyeParam_t eyeParam;
    sds_config *tx_param_misc;
}sds_tx_param_t;

typedef struct sds_conf_per_sds_s
{
    sds_config *sds_param;
    rtk_sds_rxCaliConf_t *rxCaliConf;
    sds_tx_param_t *fiber10g_8431_param;
    sds_tx_param_t *dac50cm_tx_param;
    sds_tx_param_t *dac100cm_tx_param;
    sds_tx_param_t *dac300cm_tx_param;
    sds_tx_param_t *dac500cm_tx_param;
}sds_conf_per_sds_t;

typedef struct sds_conf_per_unit_s {
    sds_conf_per_sds_t  *sds_cfg[RTK_MAX_SDS_PER_UNIT];
} sds_conf_per_unit_t;

typedef struct sds_conf_s {
    int                     hwp_id; /* mapping to hwp_hwProfile_t->identifier.id */
    sds_conf_per_unit_t *sds_conf_per_unit[RTK_MAX_NUM_OF_UNIT_LOCAL+1];
} sds_conf_t;

typedef struct reg_field_s
{
    uint32 reg;
    uint32 field;
}reg_field_t;

typedef struct patch_s{
    rt_serdesMode_t    sds_mode;
    uint32                  laneNum;  /*for multiSerdes port,ex 9300 rxaui_plus*/
    phy_type_t          phy_type;
    sds_config           *patch;
    int32                   patch_size;
} patch_t;

typedef struct patch_list_s{
    patch_t *patch_list_5g;
    uint32    size_5g;
    patch_t *patch_list_10g_odd;
    uint32    size_10g_odd;
    patch_t *patch_list_10g_even;
    uint32    size_10g_even;
}patch_list_t;

int  chip_construct(uint32 unit);

#endif  /* __CONSTRUCT_H__ */

