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
 * Purpose : PHY 8295 Driver APIs.
 *
 * Feature : PHY 8295 Driver APIs
 *
 */
#ifndef __PHY_8295_PATCH_H__
#define __PHY_8295_PATCH_H__

/*
 * Include Files
 */
#include <hal/phy/construct/conftypes.h>

/*
 * Symbol Definition
 */
typedef enum
{
    PHY_8295_PATCH_DBG_OUT = (0x1 << 1),
    PHY_8295_PATCH_DBG_BUF = (0x1 << 2),
}phy_8295_patch_dbg_t;

typedef enum
{
    PHY_8295_PATCH_TYPE_TXATTR_S18431 = 0,
    PHY_8295_PATCH_TYPE_TXATTR_S1DACLONG,
    PHY_8295_PATCH_TYPE_END
} phy_8295_patch_type_t;

typedef enum phy_8295_rxCali_process_sts_e
{
    PHY_8295_RXCALI_PROCESS_STS_IDLE = 0,
    PHY_8295_RXCALI_PROCESS_STS_PROCESS,
    PHY_8295_RXCALI_PROCESS_STS_END
} phy_8295_rxCali_process_sts_t;

typedef struct phy_8295_pageRegVal_s {
    uint16      page;
    uint16      reg;
    uint16      val;
} phy_8295_pageRegVal_t;

typedef struct phy_8295_pageRegFieldVal_s {
    uint16      page;
    uint16      reg;
    uint8       endBit;
    uint8       startBit;
    uint16      val;
} phy_8295_pageRegFieldVal_t;
#define PHY_8295_PAGEREGVAL_END             0xFFFF

typedef struct
{
    phy_8295_pageRegVal_t      *prv;
} phy_8295r_txParam_t;

typedef struct phy_8295_txParamCfg_s {
    phy_8295r_txParam_t     fiber10g_8431_param;
    phy_8295r_txParam_t     dac50cm_tx_param;
    phy_8295r_txParam_t     dac100cm_tx_param;
    phy_8295r_txParam_t     dac300cm_tx_param;
    phy_8295r_txParam_t     dac500cm_tx_param;
} phy_8295_txParamCfg_t;

typedef struct phy_8295_rxCaliSdsConf_s {
    uint16          rxCaliEnable;               /* enable when PCB trace > 10 inch or enable it for the serdes port. */
    uint16          tap0InitVal;
    uint16          vthMinThr;
    uint16          eqHoldEnable;
    uint16          dfeTap1_4Enable;
    uint16          dfeAuto;
} phy_8295_rxCaliSdsConf_t;

typedef struct phy_8295_rxCaliConf_s {
    uint16                      dacLongCableOffset;
    phy_8295_rxCaliSdsConf_t    s1;
    phy_8295_rxCaliSdsConf_t    s0;
} phy_8295_rxCaliConf_t;


#define PHY_8295_RXCALI_PARAM_FLAG_EQ_HOLD_DISABLE              (0x1 << 0)      /* set the flag when eqHoldEnable is disable. */
#define PHY_8295_RXCALI_PARAM_FLAG_DFETAP1_4_DISABLE            (0x1 << 1)      /* set the flag when dfeTap1_4Enable is disable. */
#define PHY_8295_RXCALI_PARAM_FLAG_DFEAUTO_DISABLE              (0x1 << 2)      /* set the flag when dfeAuto is disable */
typedef struct phy_8295_rxCaliParam_s
{
    uint16      cable_offset;
    uint16      tap0_init_val;
    uint16      vth_min;
    uint32      flags;
} phy_8295_rxCaliParam_t;

typedef struct phy_8295r_conf_entry_s
{
    uint8                       base_mac_id_of_this_phy;
    phy_8295_txParamCfg_t       *tx_param_cfg;      /* 10G tx parameters */
    phy_8295_pageRegFieldVal_t  *conf_param;        /* config parameter will apply to chip in patch code */
    phy_8295_rxCaliConf_t       *rxCaliConf;
} phy_8295r_conf_entry_t;
#define PHY_8295_CONFIG_MACID_END       0xFF

typedef struct phy_8295r_conf_per_unit_s {
    phy_8295r_conf_entry_t  phy_ent[RTK_MAX_PHY_PER_UNIT+1];
} phy_8295r_conf_per_unit_t;

typedef struct phy_8295r_conf_per_hwp_s {
    int                         hwp_id; /* mapping to hwp_hwProfile_t->identifier.id */
    phy_8295r_conf_per_unit_t    *phy_conf_per_unit[RTK_MAX_NUM_OF_UNIT_LOCAL+1];
} phy_8295r_conf_per_hwp_t;

typedef struct phy_8214qf_conf_entry_s
{
    uint8                       base_mac_id_of_this_phy;
    phy_8295_pageRegFieldVal_t  *conf_param;        /* config parameter will apply to chip in patch code */
    phy_8295_rxCaliConf_t       *rxCaliConf;
    uint32                      sds_eye_param_valid; /* TRUE or FALSE */
    phy_eyeParamConf_t          sds_eye_param[RTK_MAX_SDS_PER_PHY]; /* set last entry with .sdsId = PHY_CONFCODE_SDSID_END */
} phy_8214qf_conf_entry_t;

typedef struct phy_8214qf_conf_per_unit_s {
    phy_8214qf_conf_entry_t  phy_ent[RTK_MAX_PHY_PER_UNIT+1];
} phy_8214qf_conf_per_unit_t;

typedef struct phy_8214qf_conf_per_hwp_s {
    int                           hwp_id; /* mapping to hwp_hwProfile_t->identifier.id */
    phy_8214qf_conf_per_unit_t    *phy_conf_per_unit[RTK_MAX_NUM_OF_UNIT_LOCAL+1];
} phy_8214qf_conf_per_hwp_t;


typedef struct phy_8224qf_conf_entry_s
{
    uint8                       base_mac_id_of_this_phy;
    phy_8295_pageRegFieldVal_t  *conf_param;        /* config parameter will apply to chip in patch code */
    phy_8295_rxCaliConf_t       *rxCaliConf;
    uint32                      sds_eye_param_valid; /* TRUE or FALSE */
    phy_eyeParamConf_t          sds_eye_param[RTK_MAX_SDS_PER_PHY]; /* set last entry with .sdsId = PHY_CONFCODE_SDSID_END */
} phy_8224qf_conf_entry_t;

typedef struct phy_8224qf_conf_per_unit_s {
    phy_8224qf_conf_entry_t  phy_ent[RTK_MAX_PHY_PER_UNIT+1];
} phy_8224qf_conf_per_unit_t;

typedef struct phy_8224qf_conf_per_hwp_s {
    int                           hwp_id; /* mapping to hwp_hwProfile_t->identifier.id */
    phy_8224qf_conf_per_unit_t    *phy_conf_per_unit[RTK_MAX_NUM_OF_UNIT_LOCAL+1];
} phy_8224qf_conf_per_hwp_t;


/*
 * Data Declaration
 */

/*
 * Macro Declaration
 */


/*
 * Function Declaration
 */
extern void
phy_8295_patch_20160627(uint32 unit, rtk_port_t BasePort);

extern void
phy_8295_patch_20160711(uint32 unit, uint32 BasePort);

extern void
phy_8295_patch_start_tap0(uint32 unit, uint32 port);

extern int32
phy_8295_rxCali_start(uint32 unit, uint32 basePort, uint32 sdsId, phy_8295_rxCaliParam_t *param);

extern void
phy_8295r_patch_set(uint32 unit, uint32 port);

extern void
phy_8295r_patch_cust_set(uint32 unit, uint32 basePort, phy_8295r_conf_entry_t *pConf_ent);

extern void
phy_8214qf_patch_set(uint32 unit, uint32 port);

extern void
phy_8214qf_patch_cust_set(uint32 unit, uint32 port, phy_8214qf_conf_entry_t *pConf_ent);

extern void
phy_8224qf_patch_set(uint32 unit, uint32 port);

extern void
phy_8224qf_patch_cust_set(uint32 unit, uint32 port, phy_8224qf_conf_entry_t *pConf_ent);

extern int32
phy_8295r_patch_txParam_set(uint32 unit, rtk_port_t port, rtk_port_10gMedia_t media);

extern int32
phy_8295r_txParamCfg_init(uint32 unit, rtk_port_t port, phy_8295_txParamCfg_t *tx_param_cfg);

extern int32
phy_8295r_rxCaliConfPort_set(uint32 unit, rtk_port_t port, phy_8295_rxCaliConf_t *conf);

extern int32
phy_8295r_rxCaliConfPort_get(uint32 unit, rtk_port_t port, phy_8295_rxCaliConf_t *conf);

extern int32
phy_8214qf_rxCaliConf_set(uint32 unit, rtk_port_t basePort, phy_8295_rxCaliConf_t *conf);

extern int32
phy_8214qf_rxCaliConf_get(uint32 unit, rtk_port_t basePort, phy_8295_rxCaliConf_t *conf);

/* diag API */
extern int32
phy_8295_patch_debugEnable_set(uint32 enable);

extern void
phy_8295_rxCaliParamDefault_get(uint32 sdsId, phy_8295_rxCaliParam_t *param);

extern int32
phy_8295_rxCaliRegDefault_init(uint32 unit, uint32 basePort, uint32 sdsId);

#endif /* __PHY_8295_PATCH_H__ */
