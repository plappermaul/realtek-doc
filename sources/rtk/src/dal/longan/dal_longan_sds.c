
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
 * $Revision: 80942 $
 * $Date: 2017-08-01 14:55:23 +0800 (Tue, 01 Aug 2017) $
 *
 * Purpose : Definition those public global APIs and its data type in the SDK.
 *
 * Feature :  SerDes configuration
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/debug/rt_log.h>
#include <soc/type.h>
#include <osal/time.h>
#include <osal/sem.h>
#include <osal/lib.h>
#include <osal/memory.h>
#include <hal/chipdef/allreg.h>
#include <hal/common/halctrl.h>
#include <hal/mac/reg.h>
#include <hal/mac/serdes.h>
#include <hal/mac/drv/drv_rtl9300.h>
#include <hal/phy/phy_rtl9300.h>
#include <hal/chipdef/longan/rtk_longan_reg_struct.h>
#include <dal/dal_construct.h>
#include <dal/dal_mapper.h>
#include <dal/longan/dal_longan_sds.h>
#include <dal/longan/dal_longan_port.h>
#include <dal/dal_phy.h>

extern sds_conf_t *conf_rtl9300_cust_board_list[];

/*
 * Symbol Definition
 */
#define DAL_RX_CALI_ENABLE              ENABLED
#define HWP_SDS_10G(unit, sdsID)      (((sdsID) >= 2) && ((sdsID) <=9))
#define RX_IDLE_TIMEOUT_TIME          1000*10 /*us*/
#define CLK_RDY_TIMEOUT_TIME          1000*1000*2 /*us*/
#define  DELAY_MS                                 10

#define SDS_MODE_IS_10G(mode) ( \
        RTK_MII_10GR == mode || \
        RTK_MII_USXGMII_10GSXGMII == mode || \
        RTK_MII_USXGMII_10GQXGMII == mode)

#define SDS_MODE_IS_2500(mode) ( \
        RTK_MII_HISGMII == mode || \
        RTK_MII_2500Base_X == mode)

#define SDS_MODE_IS_1000(mode) ( \
        RTK_MII_1000BX_FIBER == mode || \
        RTK_MII_SGMII == mode || \
        RTK_MII_100BX_FIBER == mode)

#define SDS_PAGE_IS_DIG(page)  (page < 4)

#define SDS_TAP0  0
#define SDS_TAP1  1
#define SDS_TAP2  2
#define SDS_TAP3  3
#define SDS_TAP4  4

#define SDS_DVCS0  0
#define SDS_DVCS1  1
#define SDS_DVCS2  2
#define SDS_DVCS3  3
#define SDS_DVCS4  4
#define SDS_DVCS5  5


/*EYE REG/PAGE/BIT define*/
#define EYE_SCAN_EN_PAGE    0x2E
#define EYE_SCAN_EN_REG     0x08
#define EYE_SCAN_EN_BIT     14

#define EYE_CK_SEL_PAGE         0x2E
#define EYE_CK_SEL_REG          0x08
#define EYE_CK_SEL_BIT              15

#define EYE_RSEL_PAGE       0x2E
#define EYE_RSEL_REG         0x07
#define EYE_RSEL_BITHigh    15
#define EYE_RSEL_BITLow     14

#define EYE_BIAS_ADJ_PAGE   0x2E
#define EYE_BIAS_ADJ_REG        0x0A
#define EYE_BIAS_ADJ_BITHigh       13
#define EYE_BIAS_ADJ_BITLow         12

#define EYE_PI_ADJ_PAGE         0x2E
#define EYE_PI_ADJ_REG          0x03
#define EYE_PI_ADJ_BITHigh          15
#define EYE_PI_ADJ_BITLow           14

#define EYE_PI_EN_PAGE              0x2E
#define EYE_PI_EN_REG                   0x1A
#define EYE_PI_EN_BITHigh           9
#define EYE_PI_EN_BITLow                6

#define EYE_PI_ST_PAGE              0x2E
#define EYE_PI_ST_REG               0x1B
#define EYE_PI_ST_BITHigh           15
#define EYE_PI_ST_BITLow            0

#define EYE_REF_CTRL_PAGE           0x2E
#define EYE_REF_CTRL_REG            0x1A
#define EYE_REF_CTRL_BITHigh        5
#define EYE_REF_CTRL_BITLow         0

#define EYE_MDIO_CTRL_PAGE           0x2E
#define EYE_MDIO_CTRL_REG            0x0
#define EYE_MDIO_CTRL_BITHigh        0
#define EYE_MDIO_CTRL_BITLow        0


/*
 * Data Declaration
 */
typedef struct dal_longan_sds_info_s
{
    uint32          waMonRxCalied;  /* rxCali is done in waMon. This flag shall be clear when link change down */
    rtk_enable_t rxCaliEn;
    osal_mutex_t   rxCali_sem;
    dal_longan_sds_rxCaliProcessSts_t   rxCali_process_sts;
    rtk_port_phySdsRxCaliStatus_t rxCaliStatus;
}dal_longan_sds_info_t;

typedef struct dal_longan_sds_rxCaliInfo_s
{
    rtk_sds_rxCaliConf_t param;

    struct
    {
        uint32          flag_recal;
        uint32          leq_recal;
    } status;
} dal_longan_sds_rxCaliInfo_t;
typedef struct fiber100m_mode_info_s
{
    uint32 chage_flag[RTK_MAX_NUM_OF_UNIT][RTK_MAX_SDS_PER_UNIT];
}fiber100m_mode_info_t;

static uint32               patchDbgEnable = 0;
static uint32               sds_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};
static osal_mutex_t    sds_sem[RTK_MAX_NUM_OF_UNIT];
static rtk_sds_rxCaliConf_t      rxCaliConf[RTK_MAX_NUM_OF_UNIT][RTK_MAX_SDS_PER_UNIT];
static dal_longan_sds_info_t                sdsInfo[RTK_MAX_NUM_OF_UNIT][RTK_MAX_SDS_PER_UNIT];
uint32 sdsRxCali_init[RTK_MAX_NUM_OF_UNIT][RTK_MAX_SDS_PER_UNIT] = {{INIT_NOT_COMPLETED}};
static rtk_sds_testMode_t  testMode[RTK_MAX_NUM_OF_UNIT][RTK_MAX_SDS_PER_UNIT] = {{RTK_SDS_TESTMODE_DISABLE}};
static fiber100m_mode_info_t fiber100m_info;

rtk_sds_rxCaliConf_t rtl9300_rxCaliConf_serdes_myParam =
{
    .dacLongCableOffset    = 0,

    .tap0_init_val         = 0x1f,
    .vth_min               = 0x1,
    .eqHoldEnable          = ENABLED,
    .dfeTap1_4Enable       = ENABLED,
};

rtk_sds_rxCaliConf_t rtl9300_rxCaliConf_phy_myParam =
{
    .dacLongCableOffset    = 0,     /*donot care*/

    .tap0_init_val         = 0x1f,
    .vth_min               = 0x1,
    .eqHoldEnable          = DISABLED,
    .dfeTap1_4Enable       = ENABLED,
};

sds_tx_param_t rtl9301_24G_txParam_s4 = {
    .eyeParam_valid = TRUE,
    .eyeParam =  {/*impedance*/0x8, /*pre_amp*/0x0,  /* main_amp */0x9,  /*post_amp*/0x0,  /*pre_en*/0x0,  /*post_en*/0x0},
    .tx_param_misc = NULL,
};

sds_tx_param_t rtl9301_24G_txParam_s6 = {
    .eyeParam_valid = TRUE,
    .eyeParam =  {/*impedance*/0x8, /*pre_amp*/0x1,  /* main_amp */0x9,  /*post_amp*/0x0,  /*pre_en*/0x1,  /*post_en*/0x0},
    .tx_param_misc = NULL,
};

sds_tx_param_t rtl9301_24G_txParam_s8 = {
    .eyeParam_valid = TRUE,
    .eyeParam =  {/*impedance*/0x8, /*pre_amp*/0x1,  /* main_amp */0x9,  /*post_amp*/0x1,  /*pre_en*/0x1,  /*post_en*/0x1},
    .tx_param_misc = NULL,
};

sds_tx_param_t rtl9301_24G_txParam_s9 = {
    .eyeParam_valid = TRUE,
    .eyeParam =  {/*impedance*/0x8, /*pre_amp*/0x1,  /* main_amp */0x9,  /*post_amp*/0x1,  /*pre_en*/0x1,  /*post_en*/0x1},
    .tx_param_misc = NULL,
};

sds_tx_param_t rtl9301_24G_dac_txParam_s4 = {
    .eyeParam_valid = TRUE,
    .eyeParam =  {/*impedance*/0x8,/*pre_amp*/0x0,  /* main_amp */0x10,  /*post_amp*/0x0,  /*pre_en*/0x0,  /*post_en*/0x0},
    .tx_param_misc = NULL,
};

sds_tx_param_t rtl9301_24G_dac_txParam_s6 = {
    .eyeParam_valid = TRUE,
    .eyeParam =  {/*impedance*/0x8,/*pre_amp*/0x0,  /* main_amp */0x10,  /*post_amp*/0x0,  /*pre_en*/0x0,  /*post_en*/0x0},
    .tx_param_misc = NULL,
};

sds_tx_param_t rtl9301_24G_dac_txParam_s8 = {
    .eyeParam_valid = TRUE,
    .eyeParam =  {/*impedance*/0x8,/*pre_amp*/0x0,  /* main_amp */0x10,  /*post_amp*/0x0,  /*pre_en*/0x0,  /*post_en*/0x0},
    .tx_param_misc = NULL,
};

sds_tx_param_t rtl9301_24G_dac_txParam_s9 = {
    .eyeParam_valid = TRUE,
    .eyeParam =  {/*impedance*/0x8,/*pre_amp*/0x0,  /* main_amp */0x10,  /*post_amp*/0x0,  /*pre_en*/0x0,  /*post_en*/0x0},
    .tx_param_misc = NULL,
};

sds_tx_param_t rtl9301_24G_cascade_master_txParam_s8 = {
    .eyeParam_valid = TRUE,
    .eyeParam =  {/*impedance*/0xf, /*pre_amp*/0x2,  /* main_amp */0xc,  /*post_amp*/0x2,  /*pre_en*/0x1,  /*post_en*/0x1},
    .tx_param_misc = NULL,
};

sds_tx_param_t rtl9301_24G_cascade_master_txParam_s9 = {
    .eyeParam_valid = TRUE,
    .eyeParam =  {/*impedance*/0xf, /*pre_amp*/0x1,  /* main_amp */0xb,  /*post_amp*/0x2,  /*pre_en*/0x1,  /*post_en*/0x1},
    .tx_param_misc = NULL,
};

sds_tx_param_t rtl9301_24G_dac_cascade_master_txParam_s8 = {
    .eyeParam_valid = TRUE,
    .eyeParam =  {/*impedance*/0x8, /*pre_amp*/0x8,  /* main_amp */0x1f,  /*post_amp*/0xe,  /*pre_en*/0x1,  /*post_en*/0x1},
    .tx_param_misc = NULL,
};

sds_tx_param_t rtl9301_24G_dac_cascade_master_txParam_s9 = {
    .eyeParam_valid = TRUE,
    .eyeParam =  {/*impedance*/0x8, /*pre_amp*/0x8,  /* main_amp */0x1f,  /*post_amp*/0xc,  /*pre_en*/0x1,  /*post_en*/0x1},
    .tx_param_misc = NULL,
};

sds_tx_param_t rtl9303_80G_txParam_s2 = {
    .eyeParam_valid = TRUE,
    .eyeParam =  {/*impedance*/0x8, /*pre_amp*/0x2,  /* main_amp */0x9,  /*post_amp*/0x2,  /*pre_en*/0x1,  /*post_en*/0x1},
    .tx_param_misc = NULL,
};

sds_tx_param_t rtl9303_80G_txParam_s3 = {
    .eyeParam_valid = TRUE,
    .eyeParam =  {/*impedance*/0x8, /*pre_amp*/0x1,  /* main_amp */0x9,  /*post_amp*/0x1,  /*pre_en*/0x1,  /*post_en*/0x1},
    .tx_param_misc = NULL,
};

sds_tx_param_t rtl9303_80G_txParam_s4 = {
    .eyeParam_valid = TRUE,
    .eyeParam =  {/*impedance*/0x8, /*pre_amp*/0x0,  /* main_amp */0x9,  /*post_amp*/0x0,  /*pre_en*/0x0,  /*post_en*/0x0},
    .tx_param_misc = NULL,
};

sds_tx_param_t rtl9303_80G_txParam_s5 = {
    .eyeParam_valid = TRUE,
    .eyeParam =  {/*impedance*/0x8, /*pre_amp*/0x0,  /* main_amp */0x9,  /*post_amp*/0x0,  /*pre_en*/0x0,  /*post_en*/0x0},
    .tx_param_misc = NULL,
};

sds_tx_param_t rtl9303_80G_txParam_s6 = {
    .eyeParam_valid = TRUE,
    .eyeParam =  {/*impedance*/0x8, /*pre_amp*/0x1,  /* main_amp */0x9,  /*post_amp*/0x0,  /*pre_en*/0x1,  /*post_en*/0x0},
    .tx_param_misc = NULL,
};

sds_tx_param_t rtl9303_80G_txParam_s7 = {
    .eyeParam_valid = TRUE,
    .eyeParam =  {/*impedance*/0x8, /*pre_amp*/0x2,  /* main_amp */0x9,  /*post_amp*/0x0,  /*pre_en*/0x1,  /*post_en*/0x0},
    .tx_param_misc = NULL,
};

sds_tx_param_t rtl9303_80G_txParam_s8 = {
    .eyeParam_valid = TRUE,
    .eyeParam =  {/*impedance*/0x8, /*pre_amp*/0x1,  /* main_amp */0x9,  /*post_amp*/0x1,  /*pre_en*/0x1,  /*post_en*/0x1},
    .tx_param_misc = NULL,
};

sds_tx_param_t rtl9303_80G_txParam_s9 = {
    .eyeParam_valid = TRUE,
    .eyeParam =  {/*impedance*/0x8, /*pre_amp*/0x2,  /* main_amp */0x9,  /*post_amp*/0x2,  /*pre_en*/0x1,  /*post_en*/0x1},
    .tx_param_misc = NULL,
};

sds_tx_param_t rtl9303_80G_dac_txParam_s2 = {
    .eyeParam_valid = TRUE,
    .eyeParam =  {/*impedance*/0x8,/*pre_amp*/0x0,  /* main_amp */0x10,  /*post_amp*/0x0,  /*pre_en*/0x0,  /*post_en*/0x0},
    .tx_param_misc = NULL,
};

sds_tx_param_t rtl9303_80G_dac_txParam_s3 = {
    .eyeParam_valid = TRUE,
    .eyeParam =  {/*impedance*/0x8,/*pre_amp*/0x0,  /* main_amp */0x10,  /*post_amp*/0x0,  /*pre_en*/0x0,  /*post_en*/0x0},
    .tx_param_misc = NULL,
};

sds_tx_param_t rtl9303_80G_dac_txParam_s4 = {
    .eyeParam_valid = TRUE,
    .eyeParam =  {/*impedance*/0x8,/*pre_amp*/0x0,  /* main_amp */0x10,  /*post_amp*/0x0,  /*pre_en*/0x0,  /*post_en*/0x0},
    .tx_param_misc = NULL,
};

sds_tx_param_t rtl9303_80G_dac_txParam_s5 = {
    .eyeParam_valid = TRUE,
    .eyeParam =  {/*impedance*/0x8,/*pre_amp*/0x0,  /* main_amp */0x10,  /*post_amp*/0x0,  /*pre_en*/0x0,  /*post_en*/0x0},
    .tx_param_misc = NULL,
};

sds_tx_param_t rtl9303_80G_dac_txParam_s6 = {
    .eyeParam_valid = TRUE,
    .eyeParam =  {/*impedance*/0x8,/*pre_amp*/0x0,  /* main_amp */0x10,  /*post_amp*/0x0,  /*pre_en*/0x0,  /*post_en*/0x0},
    .tx_param_misc = NULL,
};

sds_tx_param_t rtl9303_80G_dac_txParam_s7 = {
    .eyeParam_valid = TRUE,
    .eyeParam =  {/*impedance*/0x8,/*pre_amp*/0x0,  /* main_amp */0x10,  /*post_amp*/0x0,  /*pre_en*/0x0,  /*post_en*/0x0},
    .tx_param_misc = NULL,
};

sds_tx_param_t rtl9303_80G_dac_txParam_s8 = {
    .eyeParam_valid = TRUE,
    .eyeParam =  {/*impedance*/0x8,/*pre_amp*/0x0,  /* main_amp */0x10,  /*post_amp*/0x0,  /*pre_en*/0x0,  /*post_en*/0x0},
    .tx_param_misc = NULL,
};

sds_tx_param_t rtl9303_80G_dac_txParam_s9 = {
    .eyeParam_valid = TRUE,
    .eyeParam =  {/*impedance*/0x8,/*pre_amp*/0x0,  /* main_amp */0x10,  /*post_amp*/0x0,  /*pre_en*/0x0,  /*post_en*/0x0},
    .tx_param_misc = NULL,
};

sds_conf_per_sds_t sds_cfg_rtl9301_24G_s4 = {
    .sds_param = NULL,
    .fiber10g_8431_param = &rtl9301_24G_txParam_s4,
    .dac50cm_tx_param = &rtl9301_24G_dac_txParam_s4,
    .dac100cm_tx_param = &rtl9301_24G_dac_txParam_s4,
    .dac300cm_tx_param = &rtl9301_24G_dac_txParam_s4,
    .dac500cm_tx_param = &rtl9301_24G_dac_txParam_s4,
};

sds_conf_per_sds_t sds_cfg_rtl9301_24G_s6 = {
    .sds_param = NULL,
    .fiber10g_8431_param = &rtl9301_24G_txParam_s6,
    .dac50cm_tx_param = &rtl9301_24G_dac_txParam_s6,
    .dac100cm_tx_param = &rtl9301_24G_dac_txParam_s6,
    .dac300cm_tx_param = &rtl9301_24G_dac_txParam_s6,
    .dac500cm_tx_param = &rtl9301_24G_dac_txParam_s6,
};

sds_conf_per_sds_t sds_cfg_rtl9301_24G_s8 = {
    .sds_param = NULL,
    .fiber10g_8431_param = &rtl9301_24G_txParam_s8,
    .dac50cm_tx_param = &rtl9301_24G_dac_txParam_s8,
    .dac100cm_tx_param = &rtl9301_24G_dac_txParam_s8,
    .dac300cm_tx_param = &rtl9301_24G_dac_txParam_s8,
    .dac500cm_tx_param = &rtl9301_24G_dac_txParam_s8,
};

sds_conf_per_sds_t sds_cfg_rtl9301_24G_s9 = {
    .sds_param = NULL,
    .fiber10g_8431_param = &rtl9301_24G_txParam_s9,
    .dac50cm_tx_param = &rtl9301_24G_dac_txParam_s9,
    .dac100cm_tx_param = &rtl9301_24G_dac_txParam_s9,
    .dac300cm_tx_param = &rtl9301_24G_dac_txParam_s9,
    .dac500cm_tx_param = &rtl9301_24G_dac_txParam_s9,
};

sds_conf_per_unit_t sds_cfg_rtl9301_24G = {
    .sds_cfg = {
            [0] = NULL,
            [1] = NULL,
            [2] = NULL,
            [3] = NULL,
            [4] = &sds_cfg_rtl9301_24G_s4,
            [5] = NULL,
            [6] = &sds_cfg_rtl9301_24G_s6,
            [7] = NULL,
            [8] = &sds_cfg_rtl9301_24G_s8,
            [9] = &sds_cfg_rtl9301_24G_s9,
        },
};

sds_conf_per_sds_t sds_cfg_rtl9301_24G_cascade_master_s8 = {
    .sds_param = NULL,
    .fiber10g_8431_param = &rtl9301_24G_cascade_master_txParam_s8,
    .dac50cm_tx_param = &rtl9301_24G_dac_cascade_master_txParam_s8,
    .dac100cm_tx_param = &rtl9301_24G_dac_cascade_master_txParam_s8,
    .dac300cm_tx_param = &rtl9301_24G_dac_cascade_master_txParam_s8,
    .dac500cm_tx_param = &rtl9301_24G_dac_cascade_master_txParam_s8,
};

sds_conf_per_sds_t sds_cfg_rtl9301_24G_cascade_master_s9 = {
    .sds_param = NULL,
    .fiber10g_8431_param = &rtl9301_24G_cascade_master_txParam_s9,
    .dac50cm_tx_param = &rtl9301_24G_dac_cascade_master_txParam_s9,
    .dac100cm_tx_param = &rtl9301_24G_dac_cascade_master_txParam_s9,
    .dac300cm_tx_param = &rtl9301_24G_dac_cascade_master_txParam_s9,
    .dac500cm_tx_param = &rtl9301_24G_dac_cascade_master_txParam_s9,
};

sds_conf_per_unit_t sds_cfg_rtl9301_24G_cascade_master = {
    .sds_cfg = {
            [0] = NULL,
            [1] = NULL,
            [2] = NULL,
            [3] = NULL,
            [4] = NULL,
            [5] = NULL,
            [6] = NULL,
            [7] = NULL,
            [8] = &sds_cfg_rtl9301_24G_cascade_master_s8,
            [9] = &sds_cfg_rtl9301_24G_cascade_master_s9,
        },
};

sds_conf_per_sds_t sds_cfg_rtl9303_80G_s2 = {
    .sds_param = NULL,
    .fiber10g_8431_param = &rtl9303_80G_txParam_s2,
    .dac50cm_tx_param = &rtl9303_80G_dac_txParam_s2,
    .dac100cm_tx_param = &rtl9303_80G_dac_txParam_s2,
    .dac300cm_tx_param = &rtl9303_80G_dac_txParam_s2,
    .dac500cm_tx_param = &rtl9303_80G_dac_txParam_s2,
};

sds_conf_per_sds_t sds_cfg_rtl9303_80G_s3 = {
    .sds_param = NULL,
    .fiber10g_8431_param = &rtl9303_80G_txParam_s3,
    .dac50cm_tx_param = &rtl9303_80G_dac_txParam_s3,
    .dac100cm_tx_param = &rtl9303_80G_dac_txParam_s3,
    .dac300cm_tx_param = &rtl9303_80G_dac_txParam_s3,
    .dac500cm_tx_param = &rtl9303_80G_dac_txParam_s3,
};

sds_conf_per_sds_t sds_cfg_rtl9303_80G_s4 = {
    .sds_param = NULL,
    .fiber10g_8431_param = &rtl9303_80G_txParam_s4,
    .dac50cm_tx_param = &rtl9303_80G_dac_txParam_s4,
    .dac100cm_tx_param = &rtl9303_80G_dac_txParam_s4,
    .dac300cm_tx_param = &rtl9303_80G_dac_txParam_s4,
    .dac500cm_tx_param = &rtl9303_80G_dac_txParam_s4,
};

sds_conf_per_sds_t sds_cfg_rtl9303_80G_s5 = {
    .sds_param = NULL,
    .fiber10g_8431_param = &rtl9303_80G_txParam_s5,
    .dac50cm_tx_param = &rtl9303_80G_dac_txParam_s5,
    .dac100cm_tx_param = &rtl9303_80G_dac_txParam_s5,
    .dac300cm_tx_param = &rtl9303_80G_dac_txParam_s5,
    .dac500cm_tx_param = &rtl9303_80G_dac_txParam_s5,
};

sds_conf_per_sds_t sds_cfg_rtl9303_80G_s6 = {
    .sds_param = NULL,
    .fiber10g_8431_param = &rtl9303_80G_txParam_s6,
    .dac50cm_tx_param = &rtl9303_80G_dac_txParam_s6,
    .dac100cm_tx_param = &rtl9303_80G_dac_txParam_s6,
    .dac300cm_tx_param = &rtl9303_80G_dac_txParam_s6,
    .dac500cm_tx_param = &rtl9303_80G_dac_txParam_s6,
};

sds_conf_per_sds_t sds_cfg_rtl9303_80G_s7 = {
    .sds_param = NULL,
    .fiber10g_8431_param = &rtl9303_80G_txParam_s7,
    .dac50cm_tx_param = &rtl9303_80G_dac_txParam_s7,
    .dac100cm_tx_param = &rtl9303_80G_dac_txParam_s7,
    .dac300cm_tx_param = &rtl9303_80G_dac_txParam_s7,
    .dac500cm_tx_param = &rtl9303_80G_dac_txParam_s7,
};

sds_conf_per_sds_t sds_cfg_rtl9303_80G_s8 = {
    .sds_param = NULL,
    .fiber10g_8431_param = &rtl9303_80G_txParam_s8,
    .dac50cm_tx_param = &rtl9303_80G_dac_txParam_s8,
    .dac100cm_tx_param = &rtl9303_80G_dac_txParam_s8,
    .dac300cm_tx_param = &rtl9303_80G_dac_txParam_s8,
    .dac500cm_tx_param = &rtl9303_80G_dac_txParam_s8,
};

sds_conf_per_sds_t sds_cfg_rtl9303_80G_s9 = {
    .sds_param = NULL,
    .fiber10g_8431_param = &rtl9303_80G_txParam_s9,
    .dac50cm_tx_param = &rtl9303_80G_dac_txParam_s9,
    .dac100cm_tx_param = &rtl9303_80G_dac_txParam_s9,
    .dac300cm_tx_param = &rtl9303_80G_dac_txParam_s9,
    .dac500cm_tx_param = &rtl9303_80G_dac_txParam_s9,
};

sds_conf_per_unit_t sds_cfg_rtl9303_80G = {
    .sds_cfg = {
            [0] = NULL,
            [1] = NULL,
            [2] = &sds_cfg_rtl9303_80G_s2,
            [3] = &sds_cfg_rtl9303_80G_s3,
            [4] = &sds_cfg_rtl9303_80G_s4,
            [5] = &sds_cfg_rtl9303_80G_s5,
            [6] = &sds_cfg_rtl9303_80G_s6,
            [7] = &sds_cfg_rtl9303_80G_s7,
            [8] = &sds_cfg_rtl9303_80G_s8,
            [9] = &sds_cfg_rtl9303_80G_s9,
        },
};

static sds_tx_param_t fiber10g_8431_param[RTK_MAX_NUM_OF_UNIT][RTK_MAX_SDS_PER_UNIT];
static sds_tx_param_t dac50cm_tx_param[RTK_MAX_NUM_OF_UNIT][RTK_MAX_SDS_PER_UNIT];
static sds_tx_param_t dac100cm_tx_param[RTK_MAX_NUM_OF_UNIT][RTK_MAX_SDS_PER_UNIT];
static sds_tx_param_t dac300cm_tx_param[RTK_MAX_NUM_OF_UNIT][RTK_MAX_SDS_PER_UNIT];
static sds_tx_param_t dac500cm_tx_param[RTK_MAX_NUM_OF_UNIT][RTK_MAX_SDS_PER_UNIT];

/*
 * Macro Definition
 */
#undef SDS_FIELD_R
#undef SDS_FIELD_W
#define SDS_FIELD_R(_u, _s, _p, _r, _end, _start, _d) \
    dal_longan_sds_field_read(_u, _s, _p, _r, _end, _start, _d)
#define SDS_FIELD_W(_u, _s, _p, _r, _end, _start, _d) \
    dal_longan_sds_field_write(_u, _s, _p, _r, _end, _start, _d)

#define BITS_MASK(mBit, lBit)           (uint32)((((uint64)0x1 << (mBit - lBit + 1)) - 1) << lBit)

/* debug print */
#define PATCH_PRINT(fmt, args...)                                                                   \
            do {                                                                                    \
                if (patchDbgEnable)                                        \
                    osal_printf("%s,%d : "fmt"\n", "RX_CAL", __LINE__, ##args);                     \
            }while(0)

#define SDS_FIELD_WRITE(_unit,  _sdsId, _page, _reg, _mBit, _lBit, _val)              \
            hal_sds_field_write(_unit,  _sdsId, _page, _reg, _mBit, _lBit, _val)

#define SDS_FIELD_READ(_unit,  _sdsId, _page, _reg, _mBit, _lBit, _pVal)             \
           hal_sds_field_read(_unit,  _sdsId, _page, _reg, _mBit, _lBit, _pVal)

/* trap semaphore handling */
#define SDS_SEM_LOCK(unit)    \
do {\
    if (osal_sem_mutex_take(sds_sem[unit], OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_LOCK_FAILED, (MOD_DAL|MOD_SDS), "semaphore lock failed");\
        return RT_ERR_SEM_LOCK_FAILED;\
    }\
} while(0)

#define SDS_SEM_UNLOCK(unit)   \
do {\
    if (osal_sem_mutex_give(sds_sem[unit]) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_UNLOCK_FAILED, (MOD_DAL|MOD_SDS), "semaphore unlock failed");\
        return RT_ERR_SEM_UNLOCK_FAILED;\
    }\
} while(0)


#define RXCALI_SEM_LOCK(unit, sdsId)    \
do {\
    if (osal_sem_mutex_take(sdsInfo[unit][sdsId].rxCali_sem, OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_LOCK_FAILED, (MOD_DAL|MOD_SDS), "semaphore lock failed");\
        return RT_ERR_SEM_LOCK_FAILED;\
    }\
} while(0)

#define RXCALI_SEM_UNLOCK(unit, sdsId)   \
do {\
    if (osal_sem_mutex_give(sdsInfo[unit][sdsId].rxCali_sem) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_UNLOCK_FAILED, (MOD_DAL|MOD_SDS), "semaphore unlock failed");\
        return RT_ERR_SEM_UNLOCK_FAILED;\
    }\
} while(0)

/*
 * Function Declaration
 */

void _dal_longan_sds_rxCali_LEQ_Read(uint32 unit, uint32 sdsId, rtk_sds_leq_t *pLEQ_bin);
void _dal_longan_sds_rxCali_LEQ_Manual(uint32 unit, uint32 sdsId, rtk_sds_leq_t *pLeq);
void _dal_longan_sds_rxCali_a_start_1d3(uint32 unit, uint32 sdsId, dal_longan_sds_rxCaliInfo_t *pinfo);
static int32  _dal_longan_sds_mode_get(uint32 unit, uint32 sds, rt_serdesMode_t* pMode);

/* Function Name:
 *      dal_longan_sds_reg_get
 * Description:
 *      Get SerDes register through logic sds id
 * Input:
 *      unit    - unit id
 *      sds     - logic SerDes index
 *      page    - page
 *      reg     - register index/address
 * Output:
 *      pData   - register value
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 *      RT_ERR_CHIP_NOT_SUPPORTED - chip not support
 * Note:
 *      None
 */
int32
dal_longan_sds_reg_get(uint32 unit, uint32 sds, uint32 page, uint32 reg, uint32 *pData)
{
    int32 ret = RT_ERR_OK;
    uint32 sds_mapped= 0;

    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    if(SDS_PAGE_IS_DIG(page))
    {
        RT_ERR_CHK(drv_rtl9300_sds2XsgmSds_get(unit, sds, &sds_mapped), ret);
        RT_ERR_CHK(hal_serdes_reg_get(unit, sds_mapped, page, reg, pData), ret);
    }
    else
    {
        RT_ERR_CHK(hal_serdes_reg_get(unit, sds, page, reg, pData), ret);
    }

    return ret;
}

/* Function Name:
 *      dal_longan_sds_reg_set
 * Description:
 *      Set SerDes register through logic sds id
 * Input:
 *      unit    - unit id
 *      sds     - logic SerDes index
 *      page    - page
 *      reg     - register index/address
 *      data    - register value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 *      RT_ERR_CHIP_NOT_SUPPORTED - chip not support
 * Note:
 *      None
 */
int32
dal_longan_sds_reg_set(uint32 unit, uint32 sds, uint32 page, uint32 reg, uint32 data)
{
    int32 ret = RT_ERR_OK;
    uint32 sds_mapped= 0;

    if(SDS_PAGE_IS_DIG(page))
    {
        RT_ERR_CHK(drv_rtl9300_sds2XsgmSds_get(unit, sds, &sds_mapped), ret);
        RT_ERR_CHK(hal_serdes_reg_set(unit, sds_mapped, page, reg, data), ret);
    }
    else
    {
        RT_ERR_CHK(hal_serdes_reg_set(unit, sds, page, reg, data), ret);
    }

    return ret;
}

/* Function Name:
 *      dal_longan_sds_field_read
 * Description:
 *      Read SerDes field through logic sds id
 * Input:
 *      unit - unit id
 *      sds  - logic SerDes id
 *      page - page number
 *      reg  - register
 *      endBit    - end bit of configure field
 *      startBit  - start bit of configure field
 * Output:
 *      data - SerDes Configuration
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
int32
dal_longan_sds_field_read(uint32 unit, uint32 sds, uint32 page, uint32 reg,
    uint32 endBit, uint32 startBit, uint32 *data)
{
    int32 ret = RT_ERR_OK;
    uint32 sds_mapped= 0;

    RT_PARAM_CHK((NULL == data), RT_ERR_NULL_POINTER);

    if(SDS_PAGE_IS_DIG(page))
    {
        RT_ERR_CHK(drv_rtl9300_sds2XsgmSds_get(unit, sds, &sds_mapped), ret);
        RT_ERR_CHK(hal_sds_field_read(unit, sds_mapped, page, reg, endBit, startBit, data), ret);
    }
    else
    {
        RT_ERR_CHK(hal_sds_field_read(unit, sds, page, reg, endBit, startBit, data), ret);
    }

    return ret;
}

/* Function Name:
 *      dal_longan_sds_field_write
 * Description:
 *      Write SerDes field through logic sds id
 * Input:
 *      unit - unit id
 *      sds  - logic SerDes id
 *      page - page number
 *      reg  - register
 *      endBit    - end bit of configure field
 *      startBit  - start bit of configure field
 *      data - SerDes Configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
int32
dal_longan_sds_field_write(uint32 unit, uint32 sds, uint32 page, uint32 reg,
    uint32 endBit, uint32 startBit, uint32 data)
{
    int32 ret = RT_ERR_OK;
    uint32 sds_mapped= 0;

    if(SDS_PAGE_IS_DIG(page))
    {
        RT_ERR_CHK(drv_rtl9300_sds2XsgmSds_get(unit, sds, &sds_mapped), ret);
        RT_ERR_CHK(hal_sds_field_write(unit, sds_mapped, page, reg, endBit, startBit, data), ret);
    }
    else
    {
        RT_ERR_CHK(hal_sds_field_write(unit, sds, page, reg, endBit, startBit, data), ret);
    }

    return ret;
}

/* Function Name:
 *      dal_longan_sds_xsg_field_read
 * Description:
 *      Read SerDes xsgmii & rsgmii reg field through logic sds id
 * Input:
 *      unit - unit id
 *      sds  - logic SerDes id
 *      page - page number
 *      reg  - register
 *      endBit    - end bit of configure field
 *      startBit  - start bit of configure field
 * Output:
 *      data - SerDes Configuration Array
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
int32
dal_longan_sds_xsg_field_read(uint32 unit, uint32 sds, uint32 page, uint32 reg,
    uint32 endBit, uint32 startBit, uint32 *data)
{
    int32 ret = RT_ERR_OK;
    uint32 sds_mapped= 0;

    RT_PARAM_CHK((NULL == data), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((sds != 2 && sds != 3), RT_ERR_PORT_NOT_SUPPORTED);

    if(SDS_PAGE_IS_DIG(page))
    {
        RT_ERR_CHK(drv_rtl9300_sds2XsgmSds_get(unit, sds, &sds_mapped), ret);
        RT_ERR_CHK(hal_sds_field_read(unit, sds_mapped + 0, page, reg, endBit, startBit, data), ret);
        RT_ERR_CHK(hal_sds_field_read(unit, sds_mapped + 1, page, reg, endBit, startBit, data + 1), ret);
    }
    else
    {
        RT_ERR_CHK(hal_sds_field_read(unit, sds, page, reg, endBit, startBit, data), ret);
    }

    return ret;
}

/* Function Name:
 *      dal_longan_sds_xsg_field_write
 * Description:
 *      Write SerDes xsgmii & rsgmii reg field through logic sds id
 * Input:
 *      unit - unit id
 *      sds  - logic SerDes id
 *      page - page number
 *      reg  - register
 *      endBit    - end bit of configure field
 *      startBit  - start bit of configure field
 *      data - SerDes Configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
int32
dal_longan_sds_xsg_field_write(uint32 unit, uint32 sds, uint32 page, uint32 reg,
    uint32 endBit, uint32 startBit, uint32 data)
{
    int32 ret = RT_ERR_OK;
    uint32 sds_mapped = 0;

    RT_PARAM_CHK((sds != 2 && sds != 3), RT_ERR_PORT_NOT_SUPPORTED);

    if(SDS_PAGE_IS_DIG(page))
    {
        RT_ERR_CHK(drv_rtl9300_sds2XsgmSds_get(unit, sds, &sds_mapped), ret);
        RT_ERR_CHK(hal_sds_field_write(unit, sds_mapped + 0, page, reg, endBit, startBit, data), ret);
        RT_ERR_CHK(hal_sds_field_write(unit, sds_mapped + 1, page, reg, endBit, startBit, data), ret);
    }
    else
    {
        RT_ERR_CHK(hal_sds_field_write(unit, sds, page, reg, endBit, startBit, data), ret);
    }

    return ret;
}

/* Function Name:
 *      dal_longan_sds_clkRdy_get
 * Description:
 *      Check serdes clock ready
 * Input:
 *      unit - unit id
 *      sds  - SerDes id
 * Output:
 *      pClkRdy - ready value
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
int32
dal_longan_sds_clkRdy_get(uint32 unit, uint32 sds, uint32 *pClkRdy)
{
    uint32  val = 0;
    int32   ret = RT_ERR_OK;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SDS), "unit=%d,sds=%d", unit, sds);

    /* check Init status */
    RT_INIT_CHK(sds_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_SDS_EXIST(unit, sds), RT_ERR_OUT_OF_RANGE);

    /* function body */
    SDS_SEM_LOCK(unit);
    if (RTK_MII_XSGMII == HWP_SDS_MODE(unit, sds))
    {
        RT_ERR_HDL(SDS_FIELD_W(unit, 2, 0x1f, 0x2, 15, 0, 53), ERR, ret);
        RT_ERR_HDL(SDS_FIELD_R(unit, 2, 0x1f, 20, 5, 4, &val), ERR, ret);
    }
ERR:
    SDS_SEM_UNLOCK(unit);

    *pClkRdy = (val == 3) ? (1) : (0);

    return ret;
}

int32
_dal_longan_sds_clk_wait(int32 unit, uint32 sds, uint32 timeout)
{
    osal_usecs_t    t, now, t_wait=0;
    uint32          clkRdy = 0;
    int32           chkCnt=0;

    osal_time_usecs_get(&t);
    do {
        chkCnt++;
        osal_time_usecs_get(&now);

        dal_longan_sds_clkRdy_get(unit, sds, &clkRdy);

        if (1 == clkRdy)
        {
            return RT_ERR_OK;
        }

        t_wait += ((now >= t) ? (now - t) : (0xFFFFFFFF - t + now));
        t = now;
    } while ((t_wait <= timeout));

    if (t_wait > timeout)
    {
        return RT_ERR_TIMEOUT;
    }

    return RT_ERR_OK;
}

int32
_dal_longan_sds_clk_routine(uint32 unit)
{
    int32 ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SDS), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(sds_init[unit]);

    /* function body */
    SDS_SEM_LOCK(unit);

    /*Step1:  force powndn*/
    RT_ERR_HDL(SDS_FIELD_W(unit, 2, 0x20, 0, 7, 6, 0x3), ERR, ret);
    RT_ERR_HDL(SDS_FIELD_W(unit, 3, 0x20, 0, 7, 6, 0x3), ERR, ret);

    SDS_SEM_UNLOCK(unit);

    if((ret = _dal_longan_sds_clk_wait(unit, 2, CLK_RDY_TIMEOUT_TIME) != RT_ERR_OK))
    {
        RT_LOG(LOG_INFO, (MOD_DAL|MOD_SDS),  "sds clock not rdy");

        SDS_FIELD_W(unit, 2, 0x20, 0, 7, 6, 0);
        SDS_FIELD_W(unit, 3, 0x20, 0, 7, 6, 0);
        return ret;
    }

    SDS_SEM_LOCK(unit);

    RT_ERR_HDL(SDS_FIELD_W(unit, 2, 0x20, 0x2, 11, 10, 0x1), ERR, ret);
    osal_time_mdelay(DELAY_MS);
    RT_ERR_HDL(SDS_FIELD_W(unit, 2, 0x20, 0x2, 11, 10, 0x0), ERR, ret);
    osal_time_mdelay(DELAY_MS);
    RT_ERR_HDL(SDS_FIELD_W(unit, 10, 0x20, 2, 11, 10, 0x1), ERR, ret);
    osal_time_mdelay(DELAY_MS);
    RT_ERR_HDL(SDS_FIELD_W(unit, 10, 0x20, 2, 11, 10, 0x0), ERR, ret);
    osal_time_mdelay(DELAY_MS);
    RT_ERR_HDL(SDS_FIELD_W(unit, 2, 0x1f, 15, 5, 4, 0x1), ERR, ret);
    osal_time_mdelay(DELAY_MS);
    RT_ERR_HDL(SDS_FIELD_W(unit, 2, 0x20, 0x2, 11, 10, 0x3), ERR, ret);
    osal_time_mdelay(DELAY_MS);
    RT_ERR_HDL(SDS_FIELD_W(unit, 10, 0x20, 0x2, 11, 10, 0x3), ERR, ret);
    osal_time_mdelay(DELAY_MS);

    /*up clk to 320M*/
    RT_ERR_HDL(SDS_FIELD_W(unit, 2, 0x2f, 23, 1, 0, 0x1), ERR, ret);
    RT_ERR_HDL(SDS_FIELD_W(unit, 2, 0x2f, 29, 11, 2, 0x190), ERR, ret);
    RT_ERR_HDL(SDS_FIELD_W(unit, 2, 0x2f, 30, 15, 6, 0x190), ERR, ret);
    RT_ERR_HDL(SDS_FIELD_W(unit, 2, 0x2f, 31, 15, 8, 0x30), ERR, ret);
    RT_ERR_HDL(SDS_FIELD_W(unit, 2, 0x2f, 29, 15, 14, 0x0), ERR, ret);
    RT_ERR_HDL(SDS_FIELD_W(unit, 2, 0x2f, 26, 8, 8,  0x0), ERR, ret);
    /*toggle CMU*/
    RT_ERR_HDL(SDS_FIELD_W(unit, 2, 0x21, 11, 3, 3, 1), ERR, ret);
    RT_ERR_HDL(SDS_FIELD_W(unit, 2, 0x21, 11, 2, 2, 0), ERR, ret);
    RT_ERR_HDL(SDS_FIELD_W(unit, 2, 0x21, 11, 2, 2, 1), ERR, ret);
    RT_ERR_HDL(SDS_FIELD_W(unit, 2, 0x21, 11, 3, 3, 0), ERR, ret);

    osal_time_mdelay(500);

    /*reset icg*/
    RT_ERR_HDL(SDS_FIELD_W(unit, 2, 0x1f, 15, 5, 4, 0), ERR, ret);
    osal_time_mdelay(DELAY_MS);
    RT_ERR_HDL(SDS_FIELD_W(unit, 2, 0x1f, 15, 5, 4, 1), ERR, ret);
    osal_time_mdelay(DELAY_MS);

    /*step5: down clk to 257M*/
    RT_ERR_HDL(SDS_FIELD_W(unit, 2, 0x2f, 23, 1, 0, 0x0), ERR, ret);
    RT_ERR_HDL(SDS_FIELD_W(unit, 2, 0x2f, 29, 11, 2, 0x1b8), ERR, ret);
    RT_ERR_HDL(SDS_FIELD_W(unit, 2, 0x2f, 30, 15, 6, 0x1b8), ERR, ret);
    RT_ERR_HDL(SDS_FIELD_W(unit, 2, 0x2f, 31, 15, 8, 0x35), ERR, ret);
    RT_ERR_HDL(SDS_FIELD_W(unit, 2, 0x2f, 29, 15, 14, 0x1), ERR, ret);
    RT_ERR_HDL(SDS_FIELD_W(unit, 2, 0x2f, 26, 8, 8,  0x1), ERR, ret);
    /*toggle CMU*/
    RT_ERR_HDL(SDS_FIELD_W(unit, 2, 0x21, 11, 3, 3, 1), ERR, ret);
    RT_ERR_HDL(SDS_FIELD_W(unit, 2, 0x21, 11, 2, 2, 0), ERR, ret);
    RT_ERR_HDL(SDS_FIELD_W(unit, 2, 0x21, 11, 2, 2, 1), ERR, ret);
    RT_ERR_HDL(SDS_FIELD_W(unit, 2, 0x21, 11, 3, 3, 0), ERR, ret);
    osal_time_mdelay(500);

    /*reset icg*/
    RT_ERR_HDL(SDS_FIELD_W(unit, 2, 0x1f, 15, 5, 4, 0), ERR, ret);
    osal_time_mdelay(DELAY_MS);

    /*toggle ber-notify*/
    RT_ERR_HDL(SDS_FIELD_W(unit, 2, 0x20, 2, 13, 12, 0x1), ERR, ret);
    osal_time_mdelay(DELAY_MS);
    RT_ERR_HDL(SDS_FIELD_W(unit, 2, 0x20, 2, 13, 12, 0x0), ERR, ret);
    osal_time_mdelay(DELAY_MS);
    RT_ERR_HDL(SDS_FIELD_W(unit, 10, 0x20, 2, 13, 12, 0x1), ERR, ret);
    osal_time_mdelay(DELAY_MS);
    RT_ERR_HDL(SDS_FIELD_W(unit, 10, 0x20, 2, 13, 12, 0x0), ERR, ret);
    osal_time_mdelay(DELAY_MS);

    /*release powndn*/
    RT_ERR_HDL(SDS_FIELD_W(unit, 2, 0x20, 0, 7, 6, 0), ERR, ret);
    RT_ERR_HDL(SDS_FIELD_W(unit, 3, 0x20, 0, 7, 6, 0), ERR, ret);

    _dal_phy_macIntfSerdes_reset(unit, 8);
    _dal_phy_macIntfSerdes_reset(unit, 16);

    /*sds rx rst*/
    RT_ERR_HDL(SDS_FIELD_W(unit, 2, 0x2e, 0x15, 4, 4, 0x1), ERR, ret);
    osal_time_mdelay(DELAY_MS);
    RT_ERR_HDL(SDS_FIELD_W(unit, 2, 0x2e, 0x15, 4, 4, 0x0), ERR, ret);

    osal_time_mdelay(DELAY_MS);

    RT_ERR_HDL(SDS_FIELD_W(unit, 3, 0x2e, 0x15, 4, 4, 0x1), ERR, ret);
    osal_time_mdelay(DELAY_MS);
    RT_ERR_HDL(SDS_FIELD_W(unit, 3, 0x2e, 0x15, 4, 4, 0x0), ERR, ret);

ERR:
    SDS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_sds_clk_routine
 * Description:
 *      execute SerDes clock routine.
 * Input:
 *      unit          - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
dal_longan_sds_clk_routine(uint32 unit)
{
    int32 ret = RT_ERR_OK;
    uint32 val;

    ioal_mem32_read(unit, 0x4, &val);

    if((val & 0xf) != 0)
        return ret;

    if((HWP_SDS_MODE(unit, 2) != RTK_MII_XSGMII) ||
        (HWP_SDS_MODE(unit, 3) != RTK_MII_XSGMII))
        return ret;

    if(((RTK_PHYTYPE_RTL8218D != HWP_SDS_ID2PHYMODEL(unit, 2)) &&
        (RTK_PHYTYPE_RTL8218E != HWP_SDS_ID2PHYMODEL(unit, 2))) ||
        ((RTK_PHYTYPE_RTL8218D != HWP_SDS_ID2PHYMODEL(unit, 3)) &&
        (RTK_PHYTYPE_RTL8218E != HWP_SDS_ID2PHYMODEL(unit, 3))))
        return ret;

    if((ret = _dal_longan_sds_clk_routine(unit) != RT_ERR_OK))
    {
       RT_LOG(LOG_INFO, (MOD_DAL|MOD_SDS),  "sds clock routine failed");
        return ret;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_longan_sds_modeReg_set
 * Description:
 *      Configure mac serdes mode
 * Input:
 *      unit - unit id
 *      sds  - SerDes id
 *      mode - Serdes mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
int32  dal_longan_sds_modeReg_set(uint32 unit, uint32 sds, rt_serdesMode_t mode)
{
    int32   ret = RT_ERR_OK;
    uint32 reg,field;
    uint32 sds_mode,sds_off;
    uint32 sub_sds_mode;
    reg_field_t sds_mode_sel[] =
    {
        {LONGAN_SDS_MODE_SEL_0r, LONGAN_SDS0_MODE_SELf},
        {LONGAN_SDS_MODE_SEL_0r, LONGAN_SDS1_MODE_SELf},
        {LONGAN_SDS_MODE_SEL_0r, LONGAN_SDS2_MODE_SELf},
        {LONGAN_SDS_MODE_SEL_0r, LONGAN_SDS3_MODE_SELf},
        {LONGAN_SDS_MODE_SEL_1r, LONGAN_SDS4_MODE_SELf},
        {LONGAN_SDS_MODE_SEL_1r, LONGAN_SDS5_MODE_SELf},
        {LONGAN_SDS_MODE_SEL_1r, LONGAN_SDS6_MODE_SELf},
        {LONGAN_SDS_MODE_SEL_1r, LONGAN_SDS7_MODE_SELf},
        {LONGAN_SDS_MODE_SEL_2r, LONGAN_SDS8_MODE_SELf},
        {LONGAN_SDS_MODE_SEL_2r, LONGAN_SDS9_MODE_SELf},
        {LONGAN_SDS_MODE_SEL_3r, LONGAN_SDS10_MODE_SELf},
        {LONGAN_SDS_MODE_SEL_3r, LONGAN_SDS11_MODE_SELf},
    };

    switch (mode)
    {
        case RTK_MII_DISABLE:
            /* serdes off */
            sds_mode = 0x1f;
            break;
        case RTK_MII_SGMII:
            /* serdes mode QSGMII */
            sds_mode = 0x2;
            break;
        case RTK_MII_1000BX_FIBER:
            /* serdes mode 1000BX_FIBER */
            sds_mode = 0x4;
            break;
        case RTK_MII_100BX_FIBER:
            /* serdes mode FIBER100 */
            sds_mode = 0x5;
            break;
        case RTK_MII_QSGMII:
            /* serdes mode QSGMII */
            sds_mode = 0x6;
            break;
        case RTK_MII_RSGMII:
            /* serdes mode RSGMII */
            sds_mode = 0x9;
            break;
        case RTK_MII_USXGMII_10GQXGMII:
        case RTK_MII_USXGMII_10GSXGMII:
            /* serdes mode USXGMII */
            sds_mode = 0xd;
            break;
        case RTK_MII_XSGMII:
            /* serdes mode XSGMII */
            sds_mode = 0x10;
            break;
        case RTK_MII_HISGMII:
            /* serdes mode HISGMII */
            sds_mode = 0x12;
            break;
        case RTK_MII_2500Base_X:
            /* serdes mode 2500Base_X */
            sds_mode = 0x16;
            break;
        case RTK_MII_RXAUI_LITE:
            /* serdes mode RXAUI_LITE */
            sds_mode = 0x17;
            break;
        case RTK_MII_RXAUI_PLUS:
            /* serdes mode RXAUI_PLUS */
            sds_mode = 0x19;
            break;
        case RTK_MII_10GR:
            /* serdes mode 10G Base-R */
            sds_mode = 0x1a;
            break;
        case RTK_MII_10GR1000BX_AUTO:
            /* serdes mode 10GR1000BX_AUTO */
            sds_mode = 0x1b;
            break;
        default:
            return RT_ERR_OK;
    }

    /*Serdes off first*/
    sds_off = 0x1f;
    if ((ret = reg_field_write(unit, sds_mode_sel[sds].reg, sds_mode_sel[sds].field, &sds_off)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SDS), "");
        return ret;
    }
    if(mode != RTK_MII_DISABLE)
    {
        /*Set serdes mode to chip*/
        if ((ret = reg_field_write(unit, sds_mode_sel[sds].reg, sds_mode_sel[sds].field, &sds_mode)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SDS), "");
            return ret;
        }
    }

    if((mode == RTK_MII_USXGMII_10GQXGMII) ||(mode == RTK_MII_USXGMII_10GSXGMII))
    {
        switch(sds)
        {
            case 2:
                reg = LONGAN_SDS_SUBMODE_CTRL0r;
                field = LONGAN_S2_USXGMII_SUBMODEf;
                break;
            case 3:
                reg = LONGAN_SDS_SUBMODE_CTRL0r;
                field = LONGAN_S3_USXGMII_SUBMODEf;
                break;
            case 4:
                reg = LONGAN_SDS_SUBMODE_CTRL1r;
                field = LONGAN_S4_USXGMII_SUBMODEf;
                break;
            case 5:
                reg = LONGAN_SDS_SUBMODE_CTRL1r;
                field = LONGAN_S5_USXGMII_SUBMODEf;
                break;
            case 6:
                reg = LONGAN_SDS_SUBMODE_CTRL1r;
                field = LONGAN_S6_USXGMII_SUBMODEf;
                break;
            case 7:
                reg = LONGAN_SDS_SUBMODE_CTRL1r;
                field = LONGAN_S7_USXGMII_SUBMODEf;
                break;
            case 8:
                reg = LONGAN_SDS_SUBMODE_CTRL1r;
                field = LONGAN_S8_USXGMII_SUBMODEf;
                break;
            case 9:
                reg = LONGAN_SDS_SUBMODE_CTRL1r;
                field = LONGAN_S9_USXGMII_SUBMODEf;
                break;
            default:
                return RT_ERR_OK;
        }
        sub_sds_mode = (mode == RTK_MII_USXGMII_10GQXGMII) ? (2) : (0);

        /*Set serdes mode to chip*/
        if ((ret = reg_field_write(unit,reg, field, &sub_sds_mode)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SDS), "");
            return ret;
        }
    }

    return RT_ERR_OK;
}

static int32 _dal_longan_sds_pll_cfg(uint32 unit, uint32 sds, rt_serdesMode_t mode,uint32 *lc_enable)
{
    uint32 sds_neighbor = 0;
    uint32 lane0Sds = (sds % 2) ? (sds - 1) : (sds);
    uint32 sds_pll_data = 0;
    rt_serdesMode_t mode_neighbor = RTK_MII_NONE;
    int32 ret = RT_ERR_OK;

    if(sds % 2)
        sds_neighbor = sds - 1;
    else
        sds_neighbor = sds + 1;

    if ((ret = _dal_longan_sds_mode_get(unit, sds_neighbor, &mode_neighbor)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SDS), "");
        return ret;
    }

    SDS_FIELD_R(unit, lane0Sds, 0x20, 18, 15, 8, &sds_pll_data);

    switch (mode)
    {
        case RTK_MII_DISABLE:
            *lc_enable = 0;
            break;
        case RTK_MII_10GR:
            *lc_enable = 1;
            break;
        case RTK_MII_1000BX_FIBER:
        case RTK_MII_SGMII:
        case RTK_MII_100BX_FIBER:
            if(HWP_SDS_MODE(unit, sds_neighbor) == RTK_MII_NONE)
            {
                *lc_enable = 0;
            }
            else
            {
                if(sds_pll_data == 0x35)
                {
                    RT_ERR(RT_ERR_CHIP_NOT_SUPPORTED, (MOD_DAL|MOD_SDS), "");
                    return RT_ERR_CHIP_NOT_SUPPORTED;
                }
                *lc_enable = 0;
            }
            break;
        case RTK_MII_HISGMII:
        case RTK_MII_2500Base_X:
            if(HWP_SDS_MODE(unit, sds_neighbor) == RTK_MII_NONE)
            {
                *lc_enable = 0;
            }
            else if(SDS_MODE_IS_10G(HWP_SDS_MODE(unit, sds)) && SDS_MODE_IS_10G(HWP_SDS_MODE(unit, sds_neighbor)))
            {
                if(SDS_MODE_IS_1000(mode_neighbor))
                {
                    RT_ERR(RT_ERR_CHIP_NOT_SUPPORTED, (MOD_DAL|MOD_SDS), "");
                    return RT_ERR_CHIP_NOT_SUPPORTED;
                }
                *lc_enable = 0;
            }
            else if(SDS_MODE_IS_2500(HWP_SDS_MODE(unit, sds)) && SDS_MODE_IS_2500(HWP_SDS_MODE(unit, sds_neighbor)))
            {
                *lc_enable = 1;
            }
            else if(SDS_MODE_IS_10G(HWP_SDS_MODE(unit, sds)) && SDS_MODE_IS_2500(HWP_SDS_MODE(unit, sds_neighbor)))
            {
                if(SDS_MODE_IS_1000(mode_neighbor))
                {
                    RT_ERR(RT_ERR_CHIP_NOT_SUPPORTED, (MOD_DAL|MOD_SDS), "");
                    return RT_ERR_CHIP_NOT_SUPPORTED;
                }
                *lc_enable = 0;
            }
            else if(SDS_MODE_IS_2500(HWP_SDS_MODE(unit, sds)) && SDS_MODE_IS_10G(HWP_SDS_MODE(unit, sds_neighbor)))
            {
                if(SDS_MODE_IS_1000(mode_neighbor))
                {
                    RT_ERR(RT_ERR_CHIP_NOT_SUPPORTED, (MOD_DAL|MOD_SDS), "");
                    return RT_ERR_CHIP_NOT_SUPPORTED;
                }
                *lc_enable = 0;
            }
            else if(SDS_MODE_IS_2500(HWP_SDS_MODE(unit, sds)) && SDS_MODE_IS_1000(HWP_SDS_MODE(unit, sds_neighbor)))
            {
                *lc_enable = 1;
            }
            else
            {
                RT_ERR(RT_ERR_CHIP_NOT_SUPPORTED, (MOD_DAL|MOD_SDS), "");
                return RT_ERR_CHIP_NOT_SUPPORTED;
            }
            break;
        default:
            return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}

/* Function Name:
 *      _dal_longan_sds_10gSdsMode_force
 * Description:
 *      Force 10g SerDes mode
 * Input:
 *      unit - unit id
 *      sds  - SerDes id
 *      mode - Serdes mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
int32 _dal_longan_sds_10gSdsMode_force(uint32 unit, uint32 sds, rt_serdesMode_t mode)
{
    int32  ret = RT_ERR_OK;
    uint32 clkRdy0,clkRdy1,clkRdy2;
    uint32 lane0Sds;
    uint32 mBit,lBit;
    uint32 value;
    uint32 loop;
    uint32 resVal;
    uint32 LC_enable = 0;
    uint32 LC_value = 0;

    switch (mode)
    {
        case RTK_MII_DISABLE:
            /* serdes off */
            value = 0x1f;
            break;
        case RTK_MII_10GR:
            /* serdes mode 10G Base-R */
            value = 0x1a;
            LC_value = 0x5;
            break;
        case RTK_MII_1000BX_FIBER:
            /* serdes mode FIBER1G */
            value = 0x4;
            LC_value = 0x1;
            break;
        case RTK_MII_SGMII:
            /* serdes mode SGMII */
            value = 0x2;
            LC_value = 0x1;
            break;
        case RTK_MII_HISGMII:
            /* serdes mode HISGMII */
            value = 0x12;
            LC_value = 0x3;
            break;
        case RTK_MII_2500Base_X:
            /* serdes mode 2500Base_X */
            value = 0x16;
            LC_value = 0x3;
            break;
        case RTK_MII_100BX_FIBER:
            /* serdes mode FIBER1G */
            value = 0x5;
            LC_value = 0x1;
            break;
        default:
            return RT_ERR_FAILED;
    }
    if ((ret = _dal_longan_sds_pll_cfg(unit, sds, mode, &LC_enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SDS), "");
        return ret;
    }

    lane0Sds = (sds%2) ? (sds - 1) : (sds);

    /*force powndn*/
    SDS_FIELD_W(unit, sds, 0x20, 0, 7, 6, 0x3);
    /*Force mode enable*/
    SDS_FIELD_W(unit, sds, 0x1f, 9, 6, 6, 1);
    /*Serdes off*/
    SDS_FIELD_W(unit, sds, 0x1f, 9, 11, 7, 0x1f);

    if(RTK_MII_DISABLE == mode)
    {
        if(fiber100m_info.chage_flag[unit][sds] == 1)
        {
            /*enable nway*/
            SDS_FIELD_W(unit, sds, 2, 0, 12, 12, 1);
            /*set speed default*/
            SDS_FIELD_W(unit, sds, 2, 0, 6, 6, 1);
            SDS_FIELD_W(unit, sds, 2, 0, 13, 13, 0);
            fiber100m_info.chage_flag[unit][sds] = 0;
        }
        return RT_ERR_OK;
    }

    if(lane0Sds == sds)
        SDS_FIELD_W(unit, lane0Sds, 0x20, 18, 5, 4,  0x1);
    else
        SDS_FIELD_W(unit, lane0Sds, 0x20, 18, 7, 6,  0x1);

        SDS_FIELD_W(unit, sds, 0x20, 0, 5, 4,  0x3);

    if(LC_enable == 1)
        SDS_FIELD_W(unit, lane0Sds, 0x20, 18, 11, 8, LC_value);
    else
        SDS_FIELD_W(unit, lane0Sds, 0x20, 18, 15, 12, LC_value);

#if 0
    /*analog force LC & ring enable*/
    SDS_FIELD_W(unit, lane0Sds, 0x21, 11, 3, 0,  0xf);
#endif

    if(lane0Sds == sds)
    {
        if(LC_enable == 1)
            SDS_FIELD_W(unit, lane0Sds, 0x20, 18, 5, 4, 0x3);
        else
            SDS_FIELD_W(unit, lane0Sds, 0x20, 18, 5, 4, 0x1);
    }
    else
    {
        if(LC_enable == 1)
            SDS_FIELD_W(unit, lane0Sds, 0x20, 18, 7, 6, 0x3);
        else
            SDS_FIELD_W(unit, lane0Sds, 0x20, 18, 7, 6, 0x1);
    }

    if(LC_enable == 1)
        /* LC enable*/
        SDS_FIELD_W(unit, lane0Sds, 0x20, 18, 1, 0, 0x3);
    else
        /* ring enable*/
        SDS_FIELD_W(unit, lane0Sds, 0x20, 18, 3, 2, 0x3);

    /*Force sds mode*/
    SDS_FIELD_W(unit, sds, 0x1f, 9, 6, 6, 1);
    SDS_FIELD_W(unit, sds, 0x1f, 9, 11, 7, value);

    if(RTK_MII_100BX_FIBER  == mode)
    {
        /*disable nway*/
        SDS_FIELD_W(unit, sds, 2, 0, 12, 12, 0);
        /*set speed 100m*/
        SDS_FIELD_W(unit, sds, 2, 0, 6, 6, 0);
        SDS_FIELD_W(unit, sds, 2, 0, 13, 13, 1);
        fiber100m_info.chage_flag[unit][sds] = 1;
    }

    /*Toggle LC or Ring*/
    for(loop = 0; loop < 20; loop++)
    {
        osal_time_mdelay(150);
        hal_serdes_reg_set(unit, lane0Sds, 0x1f, 2, 53);

        mBit = (lane0Sds == sds) ? (4) : (5);
        lBit = (lane0Sds == sds) ? (4) : (5);

        SDS_FIELD_R(unit, lane0Sds, 0x1f, 20, mBit, lBit, &clkRdy0);
        osal_time_mdelay(10);
        SDS_FIELD_R(unit, lane0Sds, 0x1f, 20, mBit, lBit, &clkRdy1);
        osal_time_mdelay(10);
        SDS_FIELD_R(unit, lane0Sds, 0x1f, 20, mBit, lBit, &clkRdy2);

        if((clkRdy0 == 1) && (clkRdy1 == 1) && (clkRdy2 == 1))
        {
            if(RTK_MII_10GR == mode)
            {
                SDS_FIELD_R(unit, sds, 0x6, 0x1, 2, 2, &resVal);
                SDS_FIELD_W(unit, sds, 0x6, 0x1, 2, 2, 0x1);

                /*FSM reset*/
                SDS_FIELD_W(unit, sds, 0x6, 0x2, 12, 12, 0x1);
                osal_time_mdelay(10);
                SDS_FIELD_W(unit, sds, 0x6, 0x2, 12, 12, 0x0);
                osal_time_mdelay(10);

                SDS_FIELD_R(unit, sds, 0x5, 0, 12, 12, &value);
                SDS_FIELD_R(unit, sds, 0x5, 0, 12, 12, &value);

                SDS_FIELD_W(unit, sds, 0x6, 0x1, 2, 2, resVal);

                SDS_FIELD_W(unit, sds, 0x6, 0x2, 12, 12, 0x1);
                osal_time_mdelay(10);
                SDS_FIELD_W(unit, sds, 0x6, 0x2, 12, 12, 0x0);
                osal_time_mdelay(10);

                if(value == 1)
                    break;
            }
            else
            {
                break;
            }
        }

        mBit = (RTK_MII_10GR == mode) ? (3) : (1);
        lBit = (RTK_MII_10GR == mode) ? (2) : (0);

        SDS_FIELD_W(unit, lane0Sds, 0x21, 11, mBit, lBit, 0x2);
        osal_time_mdelay(10);
        SDS_FIELD_W(unit, lane0Sds, 0x21, 11, mBit, lBit, 0x3);
    }

    /*release powndn*/
    SDS_FIELD_W(unit, sds, 0x20, 0, 7, 6, 0);

    /*sds rx rst*/
    SDS_FIELD_W(unit, sds, 0x2e, 0x15, 4, 4, 0x1);
    osal_time_mdelay(5);
    SDS_FIELD_W(unit, sds, 0x2e, 0x15, 4, 4, 0x0);

    return RT_ERR_OK;
}

/* Function Name:
 *      _dal_longan_sds_mode_get
 * Description:
 *      Get SerDes mode without sem lock
 * Input:
 *      unit - unit id
 *      sds  - SerDes id
 * Output:
 *      pMode - pointer to serdes mode
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
static int32  _dal_longan_sds_mode_get(uint32 unit, uint32 sds, rt_serdesMode_t* pMode)
{
    int32   ret = RT_ERR_OK;
    uint32  value;
    reg_field_t sds_mode_sel[] =
    {
        {LONGAN_SDS_MODE_SEL_0r, LONGAN_SDS0_MODE_SELf},
        {LONGAN_SDS_MODE_SEL_0r, LONGAN_SDS1_MODE_SELf},
        {LONGAN_SDS_MODE_SEL_0r, LONGAN_SDS2_MODE_SELf},
        {LONGAN_SDS_MODE_SEL_0r, LONGAN_SDS3_MODE_SELf},
        {LONGAN_SDS_MODE_SEL_1r, LONGAN_SDS4_MODE_SELf},
        {LONGAN_SDS_MODE_SEL_1r, LONGAN_SDS5_MODE_SELf},
        {LONGAN_SDS_MODE_SEL_1r, LONGAN_SDS6_MODE_SELf},
        {LONGAN_SDS_MODE_SEL_1r, LONGAN_SDS7_MODE_SELf},
        {LONGAN_SDS_MODE_SEL_2r, LONGAN_SDS8_MODE_SELf},
        {LONGAN_SDS_MODE_SEL_2r, LONGAN_SDS9_MODE_SELf},
        {LONGAN_SDS_MODE_SEL_3r, LONGAN_SDS10_MODE_SELf},
        {LONGAN_SDS_MODE_SEL_3r, LONGAN_SDS11_MODE_SELf},
    };

    /* parameter check */
    RT_PARAM_CHK(NULL == pMode, RT_ERR_NULL_POINTER);

    if((HWP_SDS_MODE(unit, sds) == RTK_MII_10GR) || \
        (HWP_SDS_MODE(unit, sds) == RTK_MII_1000BX_FIBER) || \
        (HWP_SDS_MODE(unit, sds) == RTK_MII_HISGMII) || \
        (HWP_SDS_MODE(unit, sds) == RTK_MII_100BX_FIBER) || \
        (HWP_SDS_MODE(unit, sds) == RTK_MII_2500Base_X) || \
        (HWP_SDS_MODE(unit, sds) == RTK_MII_SGMII))
    {
        /* get mode from serdes */
        if ((ret = SDS_FIELD_R(unit, sds, 0x1f, 9, 11, 7, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SDS), "");
            return ret;
        }
        if (value == 0)
        {
            /* get mode from mac */
            if ((ret = reg_field_read(unit, sds_mode_sel[sds].reg, sds_mode_sel[sds].field, &value)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SDS), "");
                return ret;
            }
        }
    }
    else
    {
        /* get mode from mac */
        if ((ret = reg_field_read(unit, sds_mode_sel[sds].reg, sds_mode_sel[sds].field, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SDS), "");
            return ret;
        }
    }

    switch(value)
    {
        case 0x2:
            *pMode = RTK_MII_SGMII;
            break;
        case 0x4:
            *pMode = RTK_MII_1000BX_FIBER;
            break;
        case 0x5:
            *pMode = RTK_MII_100BX_FIBER;
            break;
        case 0x6:
            *pMode = RTK_MII_QSGMII;
            break;
        case 0x9:
            *pMode = RTK_MII_RSGMII;
            break;
        case 0xd:
            *pMode = RTK_MII_USXGMII_10GQXGMII;
            break;
        case 0x10:
            *pMode = RTK_MII_XSGMII;
            break;
        case 0x12:
            *pMode = RTK_MII_HISGMII;
            break;
        case 0x16:
            *pMode = RTK_MII_2500Base_X;
            break;
        case 0x17:
            *pMode = RTK_MII_RXAUI_LITE;
            break;
        case 0x19:
            *pMode = RTK_MII_RXAUI_PLUS;
            break;
        case 0x1a:
            *pMode = RTK_MII_10GR;
            break;
        case 0x1b:
            *pMode = RTK_MII_10GR1000BX_AUTO;
            break;
        case 0x1f:
            *pMode = RTK_MII_DISABLE;
            break;
        default:
            return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_sds_mode_get
 * Description:
 *      Get SerDes mode
 * Input:
 *      unit - unit id
 *      sds  - SerDes id
 * Output:
 *      pMode - pointer to serdes mode
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
int32  dal_longan_sds_mode_get(uint32 unit, uint32 sds, rt_serdesMode_t* pMode)
{
    int32   ret = RT_ERR_OK;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SDS), "unit=%d,sds=%d", unit, sds);

    /* check Init status */
    RT_INIT_CHK(sds_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_SDS_EXIST(unit, sds), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(NULL == pMode, RT_ERR_NULL_POINTER);

    SDS_SEM_LOCK(unit);

    ret = _dal_longan_sds_mode_get(unit, sds, pMode);

    SDS_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      dal_longan_sds_mode_set
 * Description:
 *      Set SerDes mode
 * Input:
 *      unit - unit id
 *      sds  - SerDes id
 *      mode - Serdes mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
int32  dal_longan_sds_mode_set(uint32 unit, uint32 sds, rt_serdesMode_t mode)
{
    int32   ret = RT_ERR_OK;
    rtk_port_linkStatus_t linkSts;
    rtk_enable_t enable;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SDS), "unit=%d,sds=%d,mode=%d", unit, sds,mode);

    /* check Init status */
    RT_INIT_CHK(sds_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_SDS_EXIST(unit, sds), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((mode >= RTK_MII_END), RT_ERR_INPUT);

    if((HWP_SDS_MODE(unit, sds) == RTK_MII_10GR) || \
        (HWP_SDS_MODE(unit, sds) == RTK_MII_1000BX_FIBER) || \
        (HWP_SDS_MODE(unit, sds) == RTK_MII_HISGMII) || \
        (HWP_SDS_MODE(unit, sds) == RTK_MII_100BX_FIBER) || \
        (HWP_SDS_MODE(unit, sds) == RTK_MII_2500Base_X) || \
        (HWP_SDS_MODE(unit, sds) == RTK_MII_SGMII))
    {
        _dal_longan_port_macForceLink_get(unit,  HWP_SDS_ID2MACID(unit, sds), &enable, &linkSts);
        _dal_longan_port_macForceLink_set(unit,  HWP_SDS_ID2MACID(unit, sds), ENABLED, PORT_LINKDOWN,PORT_DEFAULT);

        osal_time_mdelay(20);

        SDS_SEM_LOCK(unit);
        if ((ret = _dal_longan_sds_10gSdsMode_force(unit, sds, RTK_MII_DISABLE)) != RT_ERR_OK)
        {
            SDS_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_SDS), "");
            return ret;
        }
        SDS_SEM_UNLOCK(unit);

        _dal_longan_port_macForceLink_set(unit,  HWP_SDS_ID2MACID(unit, sds), enable, linkSts,PORT_DEFAULT);

        dal_longan_sds_rxCaliRegDefault_init(unit,sds);
        dal_longan_sds_rxCaliStatus_clear(unit, sds);

        SDS_SEM_LOCK(unit);
        if(mode != RTK_MII_DISABLE)
        {
            if ((ret = _dal_longan_sds_10gSdsMode_force(unit, sds, mode)) != RT_ERR_OK)
            {
                SDS_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_SDS), "");
                return ret;
            }
        }
        SDS_SEM_UNLOCK(unit);

    }
    else
    {
        SDS_SEM_LOCK(unit);
        if ((ret = dal_longan_sds_modeReg_set(unit, sds, mode)) != RT_ERR_OK)
        {
            SDS_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_SDS), "");
            return ret;
        }
        SDS_SEM_UNLOCK(unit);
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_sds_leq_get
 * Description:
 *      Get SerDes LEQ
 * Input:
 *      unit - unit id
 *      sds  - SerDes id
 * Output:
 *      pLeq - LEQ configuration
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
int32
dal_longan_sds_leq_get(uint32 unit, uint32 sds, rtk_sds_leq_t *pLeq)
{
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SDS), "unit=%d,sds=%d", unit, sds);

    /* check Init status */
    RT_INIT_CHK(sds_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((!HWP_SDS_EXIST(unit, sds)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pLeq), RT_ERR_NULL_POINTER);

    if(!HWP_SDS_10G(unit, sds))
    {
        RT_LOG(LOG_INFO, (MOD_DAL|MOD_SDS), "unit=%d,sds=%d is NoT 10G SDS", unit, sds);
        return RT_ERR_OK;
    }

    SDS_SEM_LOCK(unit);
    _dal_longan_sds_rxCali_LEQ_Read(unit, sds, pLeq);
    SDS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SDS), "leq=%d", pLeq->val);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_sds_leq_set
 * Description:
 *      Set SerDes LEQ
 * Input:
 *      unit - unit id
 *      sds  - SerDes id
 *      pLeq  - LEQ configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
int32
dal_longan_sds_leq_set(uint32 unit, uint32 sds, rtk_sds_leq_t *pLeq)
{
    int32   ret = RT_ERR_OK;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SDS), "unit=%d,sds=%d", unit, sds);

    /* check Init status */
    RT_INIT_CHK(sds_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_SDS_EXIST(unit, sds), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pLeq), RT_ERR_NULL_POINTER);

    if(!HWP_SDS_10G(unit, sds))
    {
        RT_LOG(LOG_INFO, (MOD_DAL|MOD_SDS), "unit=%d,sds=%d is NoT 10G SDS", unit, sds);
        return RT_ERR_OK;
    }

    /* function body */
    SDS_SEM_LOCK(unit);
    _dal_longan_sds_rxCali_LEQ_Manual(unit, sds, pLeq);
    SDS_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      dal_longan_sds_leq_adapt
 * Description:
 *      Set SerDes LEQ adapt
 * Input:
 *      unit - unit id
 *      sds  - SerDes id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
int32
dal_longan_sds_leq_adapt(uint32 unit, uint32 sds)
{
    int32   ret = RT_ERR_OK;
    dal_longan_sds_rxCaliInfo_t rxCaliInfo;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SDS), "unit=%d,sds=%d", unit, sds);

    /* check Init status */
    RT_INIT_CHK(sds_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_SDS_EXIST(unit, sds), RT_ERR_OUT_OF_RANGE);

    if(!HWP_SDS_10G(unit, sds))
    {
        RT_LOG(LOG_INFO, (MOD_DAL|MOD_SDS), "unit=%d,sds=%d is NoT 10G SDS", unit, sds);
        return RT_ERR_OK;
    }

    /* function body */
    osal_memset(&rxCaliInfo, 0, sizeof(dal_longan_sds_rxCaliInfo_t));
    SDS_SEM_LOCK(unit);
    osal_memcpy(&rxCaliInfo.param, &rxCaliConf[unit][sds], sizeof(rtk_sds_rxCaliConf_t));
    SDS_SEM_UNLOCK(unit);

    RXCALI_SEM_LOCK(unit,sds);
    _dal_longan_sds_rxCali_a_start_1d3(unit, sds, &rxCaliInfo);
    RXCALI_SEM_UNLOCK(unit, sds);

    return ret;
}

/* Function Name:
 *      dal_longan_sds_xsgNwayEn_set
 * Description:
 *      Set SerDes XSG N-way state
 * Input:
 *      unit - unit id
 *      sds  - SerDes id
 *      en   - Configure XSG N-way state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
int32
dal_longan_sds_xsgNwayEn_set(uint32 unit, uint32 sds, rtk_enable_t en)
{
    uint32  val;
    int32   ret = RT_ERR_OK;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SDS), "unit=%d,sds=%d,en=%d", unit, sds, en);

    /* check Init status */
    RT_INIT_CHK(sds_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_SDS_EXIST(unit, sds), RT_ERR_OUT_OF_RANGE);

    /* function body */
    val = (ENABLED == en) ? (0) : (1);

    SDS_SEM_LOCK(unit);

    RT_ERR_HDL(dal_longan_sds_xsg_field_write(unit, sds, 0x0, 0x2, 9, 8, val), ERR, ret);

ERR:
    SDS_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      dal_longan_sds_cmuBand_get
 * Description:
 *      Get SerDes CMU band
 * Input:
 *      unit - unit id
 *      sds  - SerDes id
 * Output:
 *      band - CMU band value
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
int32
dal_longan_sds_cmuBand_get(uint32 unit, uint32 sds, uint32 *band)
{
    uint32  page;
    uint32  enVal;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SDS), "unit=%d,sds=%d", unit, sds);

    /* check Init status */
    RT_INIT_CHK(sds_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_SDS_EXIST(unit, sds), RT_ERR_OUT_OF_RANGE);

    /* function body */
    SDS_SEM_LOCK(unit);
    RT_ERR_HDL(drv_rtl9300_sdsCmuPage_get(unit, sds, &page), ERR, ret);
    sds = (sds%2) ? (sds - 1) : (sds);

    RT_ERR_HDL(SDS_FIELD_W(unit, sds, page, 0x1c, 15, 15, 1), ERR, ret);
    RT_ERR_HDL(SDS_FIELD_W(unit, sds + 1, page, 0x1c, 15, 15, 1), ERR, ret);

    RT_ERR_HDL(SDS_FIELD_R(unit, sds, page, 27, 1, 1, &enVal), ERR, ret);

    if(enVal == 0)/*Auto mode*/
    {
        RT_ERR_HDL(hal_serdes_reg_set(unit, sds, 0x1f, 0x02, 31), ERR, ret);
        RT_ERR_HDL(SDS_FIELD_R(unit, sds, 0x1f, 0x15, 5, 1, band), ERR, ret);
    }
    else
    {
        RT_ERR_HDL(SDS_FIELD_R(unit, sds, page, 30, 4, 0, band), ERR, ret);
    }
ERR:
    SDS_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      dal_longan_sds_cmuBand_set
 * Description:
 *      Set SerDes CMU band state and value
 * Input:
 *      unit - unit id
 *      sds  - SerDes id
 *      en   - Configure CMU band state
 *      val  - CMU band
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
int32
dal_longan_sds_cmuBand_set(uint32 unit, uint32 sds, rtk_enable_t en, uint32 val)
{
    uint32  page;
    uint32  enVal;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SDS), "unit=%d,sds=%d,en=%d,val=%d", unit, sds, en, val);

    /* check Init status */
    RT_INIT_CHK(sds_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_SDS_EXIST(unit, sds), RT_ERR_OUT_OF_RANGE);

    /* function body */
    enVal = (ENABLED == en) ? (0) : (1);

    SDS_SEM_LOCK(unit);
    RT_ERR_HDL(drv_rtl9300_sdsCmuPage_get(unit, sds, &page), ERR, ret);
    sds = (sds%2) ? (sds - 1) : (sds);

    RT_ERR_HDL(SDS_FIELD_W(unit, sds, page, 0x1c, 15, 15, 1), ERR, ret);
    RT_ERR_HDL(SDS_FIELD_W(unit, sds + 1, page, 0x1c, 15, 15, 1), ERR, ret);

    RT_ERR_HDL(SDS_FIELD_W(unit, sds, page, 27, 1, 1, enVal), ERR, ret);
    RT_ERR_HDL(SDS_FIELD_W(unit, sds, page, 30, 4, 0, val), ERR, ret);

    RT_ERR_HDL(hal_mac_serdes_rst(unit, sds), ERR, ret);

ERR:
    SDS_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      dal_longan_sds_symErr_clear
 * Description:
 *      Clear the SerDes symbol error count.
 * Input:
 *      unit      - unit id
 *      sds       - SerDes id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_longan_sds_symErr_clear(uint32 unit, uint32 sds)
{
    uint32  i;
    rtk_sds_symErr_t info;
    rt_serdesMode_t mode;
    int32   ret = RT_ERR_OK;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SDS), "unit=%d,sds=%d", unit, sds);

    /* check Init status */
    RT_INIT_CHK(sds_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_SDS_EXIST(unit, sds), RT_ERR_OUT_OF_RANGE);

    if((ret = dal_longan_sds_mode_get(unit, sds, &mode))!= RT_ERR_OK)
        return ret;

    /* function body */
    SDS_SEM_LOCK(unit);

    if (RTK_MII_XSGMII == mode)
    {
        for (i = 0; i < 4; ++i)
        {
            RT_ERR_HDL(dal_longan_sds_xsg_field_write(unit, sds, 0x1, 24, 2, 0, i), ERR, ret);
            RT_ERR_HDL(dal_longan_sds_xsg_field_write(unit, sds, 0x1, 3, 15, 8, 0x0), ERR, ret);
            RT_ERR_HDL(dal_longan_sds_xsg_field_write(unit, sds, 0x1, 2, 15, 0, 0x0), ERR, ret);
        }

        RT_ERR_HDL(dal_longan_sds_xsg_field_write(unit, sds, 0x1, 0, 15, 0, 0x0), ERR, ret);
        RT_ERR_HDL(dal_longan_sds_xsg_field_write(unit, sds, 0x1, 1, 15, 8, 0x0), ERR, ret);
    }
    else if((RTK_MII_10GR== mode) ||
                (RTK_MII_USXGMII_10GSXGMII== mode))
    {
        /*Read Clear*/
        RT_ERR_HDL(SDS_FIELD_R(unit, sds, 0x5, 1, 15, 15, &info.latch_blk_lock), ERR, ret);
        RT_ERR_HDL(SDS_FIELD_R(unit, sds, 0x5, 1, 14, 14, &info.latch_hiber), ERR, ret);
        RT_ERR_HDL(SDS_FIELD_R(unit, sds, 0x5, 1, 13, 8, &info.ber), ERR, ret);
        RT_ERR_HDL(SDS_FIELD_R(unit, sds, 0x5, 1, 7, 0, &info.blk_err), ERR, ret);
    }
    else if(RTK_MII_QSGMII == mode)
    {
        for (i = 0; i < 4; ++i)
        {
            RT_ERR_HDL(SDS_FIELD_W(unit, sds, 0x1, 24, 2, 0, i), ERR, ret);
            RT_ERR_HDL(SDS_FIELD_W(unit, sds, 0x1, 3, 15, 8, 0x0), ERR, ret);
            RT_ERR_HDL(SDS_FIELD_W(unit, sds, 0x1, 2, 15, 0, 0x0), ERR, ret);
        }
        RT_ERR_HDL(SDS_FIELD_W(unit, sds, 0x1, 0, 15, 0, 0x0), ERR, ret);
        RT_ERR_HDL(SDS_FIELD_W(unit, sds, 0x1, 1, 15, 8, 0x0), ERR, ret);
    }
    else
    {
        RT_ERR_HDL(SDS_FIELD_W(unit, sds, 0x1, 24, 2, 0, 0), ERR, ret);

        RT_ERR_HDL(SDS_FIELD_W(unit, sds, 0x1, 3, 15, 8, 0), ERR, ret);
        RT_ERR_HDL(SDS_FIELD_W(unit, sds, 0x1, 2, 15, 0, 0), ERR, ret);
    }
ERR:
    SDS_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      dal_longan_sds_symErr_get
 * Description:
 *      Get the SerDes symbol error count.
 * Input:
 *      unit      - unit id
 *      sds       - SerDes id
 * Output:
 *      info      - symbol error count information
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_longan_sds_symErr_get(uint32 unit, uint32 sds, rtk_sds_symErr_t *info)
{
    rt_serdesMode_t mode;
    uint32  val, i;
    uint32  xsg_val[2] = {0};
    int32   ret = RT_ERR_OK;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SDS), "unit=%d,sds=%d", unit, sds);

    /* check Init status */
    RT_INIT_CHK(sds_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_SDS_EXIST(unit, sds), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(NULL == info, RT_ERR_NULL_POINTER);

    osal_memset(info, 0x0, sizeof(rtk_sds_symErr_t));

    if((ret = dal_longan_sds_mode_get(unit, sds, &mode))!= RT_ERR_OK)
        return ret;

    /* function body */
    SDS_SEM_LOCK(unit);

    if (RTK_MII_XSGMII == mode)
    {
        RT_ERR_HDL(dal_longan_sds_xsg_field_read(unit, sds, 0x1, 1, 15, 8, xsg_val), ERR, ret);
        info->all[0] = xsg_val[0] << 16;
        info->all[1] = xsg_val[1] << 16;
        RT_ERR_HDL(dal_longan_sds_xsg_field_read(unit, sds, 0x1, 0, 15, 0, xsg_val), ERR, ret);
        info->all[0] |= xsg_val[0];
        info->all[1] |= xsg_val[1];

        for (i = 0; i < 4; ++i)
        {
            RT_ERR_HDL(dal_longan_sds_xsg_field_write(unit, sds, 0x1, 24, 2, 0, i), ERR, ret);

            RT_ERR_HDL(dal_longan_sds_xsg_field_read(unit, sds, 0x1, 3, 15, 8, xsg_val), ERR, ret);
            info->ch[i] = xsg_val[0] << 16;
            info->ch[i + 4] = xsg_val[1] << 16;
            RT_ERR_HDL(dal_longan_sds_xsg_field_read(unit, sds, 0x1, 2, 15, 0, xsg_val), ERR, ret);
            info->ch[i] |= xsg_val[0];
            info->ch[i + 4] |= xsg_val[1];
        }
    }
    else if((RTK_MII_10GR== mode) ||
                (RTK_MII_USXGMII_10GSXGMII== mode))
    {
        RT_ERR_HDL(hal_serdes_reg_get(unit, sds, 0x5, 1, &val), ERR, ret);

        info->latch_blk_lock = (val >> 15) & 0x1;
        info->latch_hiber = (val >> 14) & 0x1;
        info->ber = (val >> 8) & 0x3f;
        info->blk_err = (val & 0xff);
        info->ch[0] = info->blk_err;
    }
    else if(RTK_MII_USXGMII_10GQXGMII== mode)
    {
        RT_ERR_HDL(SDS_FIELD_R(unit, sds, 0x7, 28, 15, 15, &info->latch_hiber), ERR, ret);
        RT_ERR_HDL(SDS_FIELD_R(unit, sds, 0x7, 28, 11, 11, &info->latch_blk_lock), ERR, ret);
    }
    else if(RTK_MII_QSGMII == mode)
    {
        RT_ERR_HDL(SDS_FIELD_R(unit, sds, 0x1, 1, 15, 8, &val), ERR, ret);
        info->all[0] = val << 16;
        RT_ERR_HDL(SDS_FIELD_R(unit, sds, 0x1, 0, 15, 0, &val), ERR, ret);
        info->all[0] |= val;

        for (i = 0; i < 4; ++i)
        {
            RT_ERR_HDL(SDS_FIELD_W(unit, sds, 0x1, 24, 2, 0, i), ERR, ret);

            RT_ERR_HDL(SDS_FIELD_R(unit, sds, 0x1, 3, 15, 8, &val), ERR, ret);
            info->ch[i] = val << 16;
            RT_ERR_HDL(SDS_FIELD_R(unit, sds, 0x1, 2, 15, 0, &val), ERR, ret);
            info->ch[i] |= val;
        }
    }
    else
    {
        RT_ERR_HDL(SDS_FIELD_W(unit, sds, 0x1, 24, 2, 0, 0), ERR, ret);

        RT_ERR_HDL(SDS_FIELD_R(unit, sds, 0x1, 3, 15, 8, &val), ERR, ret);
        info->ch[0] = val << 16;
        RT_ERR_HDL(SDS_FIELD_R(unit, sds, 0x1, 2, 15, 0, &val), ERR, ret);
        info->ch[0] |= val;
    }
ERR:
    SDS_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      dal_longan_sds_clk_inv
 * Description:
 *      Serdes clock inverse..
 * Input:
 *      unit    - unit id
 *      sds     - serdes id
 *      val     - val
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_OUT_OF_RANGE - Serdes index is not support.
 * Note:
 *      None
 */
int32
dal_longan_sds_clk_inv(uint32 unit, uint32 sds, uint32 val)
{
    int32   ret = RT_ERR_OK;
    uint32 baseSds;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SDS), "unit=%d,sds=%d", unit, sds);

    /* check Init status */
    RT_INIT_CHK(sds_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_SDS_EXIST(unit, sds), RT_ERR_OUT_OF_RANGE);

    /* function body */
    SDS_SEM_LOCK(unit);

    if (RTK_MII_RXAUI_PLUS == HWP_SDS_MODE(unit, sds))
    {
        baseSds = HWP_MULTI_SDS_BASESDS(unit, sds);
        RT_ERR_HDL(SDS_FIELD_W(unit, baseSds, 0xA, 13, 9, 9, val), ERR, ret);
    }
ERR:
    SDS_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      dal_longan_sds_rx_rst
 * Description:
 *      Reset Serdes Rx and original patch are kept.
 * Input:
 *      unit    - unit id
 *      sds     - serdes id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_OUT_OF_RANGE - Serdes index is not support.
 * Note:
 *      None
 */
int32
dal_longan_sds_rx_rst(uint32 unit, uint32 sdsId)
{
    rt_serdesMode_t mode;
    int32 ret = RT_ERR_OK;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SDS), "unit=%d,sds=%d", unit, sdsId);

    /* check Init status */
    RT_INIT_CHK(sds_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_SDS_EXIST(unit, sdsId), RT_ERR_OUT_OF_RANGE);

    if((ret = dal_longan_sds_mode_get(unit, sdsId, &mode))!= RT_ERR_OK)
        return ret;

    /* function body */
    SDS_SEM_LOCK(unit);

    if(RTK_MII_1000BX_FIBER == mode)
    {
        SDS_FIELD_W(unit, sdsId, 0x24, 0x15, 4, 4, 0x1);
        osal_time_mdelay(5);
        SDS_FIELD_W(unit, sdsId, 0x24, 0x15, 4, 4, 0x0);
    }
    else if((RTK_MII_USXGMII_10GQXGMII == mode) ||
        (RTK_MII_USXGMII_10GSXGMII == mode) ||
        (RTK_MII_10GR == mode)||
        (RTK_MII_XSGMII == mode))
    {
        SDS_FIELD_W(unit, sdsId, 0x2e, 0x15, 4, 4, 0x1);
        osal_time_mdelay(5);
        SDS_FIELD_W(unit, sdsId, 0x2e, 0x15, 4, 4, 0x0);
    }

    SDS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_sds_linkSts_get
 * Description:
 *      Get the SerDes link status.
 * Input:
 *      unit      - unit id
 *      sds       - SerDes id
 * Output:
 *      info      - link status information
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_longan_sds_linkSts_get(uint32 unit, uint32 sds, rtk_sds_linkSts_t *info)
{
    rt_serdesMode_t mode;
    uint32  xsg_val[2] = {0};
    int32   ret = RT_ERR_OK;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SDS), "unit=%d,sds=%d", unit, sds);

    /* check Init status */
    RT_INIT_CHK(sds_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_SDS_EXIST(unit, sds), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(NULL == info, RT_ERR_NULL_POINTER);

    if((ret = dal_longan_sds_mode_get(unit, sds, &mode))!= RT_ERR_OK)
        return ret;

    /* function body */
    SDS_SEM_LOCK(unit);

    if (RTK_MII_XSGMII == mode)
    {
        RT_ERR_HDL(dal_longan_sds_xsg_field_read(unit, sds, 0x1, 29, 8, 0, xsg_val), ERR, ret);
        info->sts = xsg_val[0];
        info->sts1 = xsg_val[1];
        RT_ERR_HDL(dal_longan_sds_xsg_field_read(unit, sds, 0x1, 30, 8, 0, xsg_val), ERR, ret);
        info->latch_sts = xsg_val[0];
        info->latch_sts1 = xsg_val[1];
    }
    else if((RTK_MII_10GR== mode) ||
                (RTK_MII_USXGMII_10GSXGMII== mode))
    {
        RT_ERR_HDL(SDS_FIELD_R(unit, sds, 0x5, 0, 12, 12, &info->sts), ERR, ret);
        RT_ERR_HDL(SDS_FIELD_R(unit, sds, 0x4, 1, 2, 2, &info->latch_sts), ERR, ret);
    }
    else if(RTK_MII_USXGMII_10GQXGMII== mode)
    {
        RT_ERR_HDL(SDS_FIELD_R(unit, sds, 0x5, 0, 12, 12, &info->sts), ERR, ret);
        RT_ERR_HDL(SDS_FIELD_R(unit, sds, 0x4, 1, 2, 2, &info->latch_sts), ERR, ret);
        RT_ERR_HDL(SDS_FIELD_R(unit, sds, 0x7, 21, 14, 14, &info->latch_sts1), ERR, ret);
    }
    else
    {
        RT_ERR_HDL(SDS_FIELD_R(unit, sds, 0x1, 29, 8, 0, &info->sts), ERR, ret);
        RT_ERR_HDL(SDS_FIELD_R(unit, sds, 0x1, 30, 8, 0, &info->latch_sts), ERR, ret);
    }
ERR:
    SDS_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      dal_longan_sds_linkFault_get
 * Description:
 *      Get serdes link fault message.
 * Input:
 *      unit      - unit id
 *      sds       - SerDes id
 * Output:
 *      pRxFault      - rxFault information
 *      pTxFault      - txFault information
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      For 8G Serdes mode
 */
int32
dal_longan_sds_linkFault_get(uint32 unit, uint32 sds, uint32 *pRxFault, uint32 *pTxFault)
{
    int32   ret = RT_ERR_OK;

    /* parameter check */
    RT_PARAM_CHK((!HWP_SDS_EXIST(unit, sds)), RT_ERR_OUT_OF_RANGE);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SDS), "unit=%d,sds=%d", unit, sds);

    /* function body */
    SDS_SEM_LOCK(unit);

    if (RTK_MII_RXAUI_PLUS == HWP_SDS_MODE(unit, sds))
    {
        RT_ERR_HDL(SDS_FIELD_R(unit, sds, 0x8, 0x8, 10, 10, pRxFault), ERR, ret);
        RT_ERR_HDL(SDS_FIELD_R(unit, sds, 0x8, 0x8, 11, 11, pTxFault), ERR, ret);
    }
ERR:
    SDS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SDS), "pRxFault=%d,pTxFault=%d", *pRxFault,pTxFault);

    return ret;
}

/* Function Name:
 *      dal_longan_sds_linkFault_check
 * Description:
 *      Check the serdes link fault problem.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      For 8G Serdes mode.
 */
int32 dal_longan_sds_linkFault_check(uint32 unit)
{
    int32 ret;
    uint32 sdsId;
    rt_serdesMode_t mode;
    uint32 rxIdle;
    uint32 txFault0 = 0, rxFault0 = 0;
    uint32 txFault1 = 0, rxFault1 = 0;
    uint32 txFault2 = 0, rxFault2 = 0;

    HWP_SDS_TRAVS(unit, sdsId)
    {
        if((ret = dal_longan_sds_mode_get(unit, sdsId, &mode))!= RT_ERR_OK)
            return ret;

        if(RTK_MII_RXAUI_PLUS != mode)
            continue;

        if(sdsId != HWP_MULTI_SDS_BASESDS(unit, sdsId))
            continue;

        if((ret = dal_longan_sds_10gRxIdle_get(unit, sdsId, &rxIdle))!= RT_ERR_OK)
            return ret;

        if(0 != rxIdle)
            continue;

        if((ret = dal_longan_sds_linkFault_get(unit, sdsId, &rxFault0, &txFault0)) != RT_ERR_OK)
            return ret;

       if((ret = dal_longan_sds_linkFault_get(unit, sdsId, &rxFault1, &txFault1)) != RT_ERR_OK)
            return ret;

       if((ret = dal_longan_sds_linkFault_get(unit, sdsId, &rxFault2, &txFault2)) != RT_ERR_OK)
            return ret;

        if(txFault1 !=0 && txFault2 !=0)
        {
            dal_longan_sds_clk_inv(unit, sdsId, 0);
        }
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_sds_10gRxIdle_get
 * Description:
 *      Get the RxIdle for 10G Sds.
 * Input:
 *      unit      - unit id
 *      sds       - SerDes id
 * Output:
 *      pRxIdle      - rxIdle information
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_longan_sds_10gRxIdle_get(uint32 unit, uint32 sds, uint32 *pRxIdle)
{
    int32   ret = RT_ERR_OK;

    /* parameter check */
    RT_PARAM_CHK((!HWP_SDS_EXIST(unit, sds)), RT_ERR_OUT_OF_RANGE);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SDS), "unit=%d,sds=%d", unit, sds);

    if(!HWP_SDS_10G(unit, sds))
    {
        RT_LOG(LOG_INFO, (MOD_DAL|MOD_SDS), "unit=%d,sds=%d is NoT 10G SDS", unit, sds);
        return RT_ERR_OK;
    }

    /* function body */
    SDS_SEM_LOCK(unit);

    if(0 == (sds %2))
    {
        RT_ERR_HDL(SDS_FIELD_W(unit, sds, 0x1f, 0x2, 15, 0, 53), ERR, ret);
        RT_ERR_HDL(SDS_FIELD_R(unit, sds, 0x1f, 0x14, 0, 0, pRxIdle), ERR, ret);
    }
    else
    {
        RT_ERR_HDL(SDS_FIELD_W(unit, sds - 1, 0x1f, 0x2, 15, 0, 53), ERR, ret);
        RT_ERR_HDL(SDS_FIELD_R(unit, sds - 1, 0x1f, 0x14, 1, 1, pRxIdle), ERR, ret);
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SDS), "pRxIdle=%d", *pRxIdle);
ERR:
    SDS_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      dal_longan_sds_10gBerNotify_get
 * Description:
 *      Get the BerNotify for 10G Sds.
 * Input:
 *      unit      - unit id
 *      sds       - SerDes id
 * Output:
 *      pBerNotify      - rxIdle information
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_longan_sds_10gBerNotify_get(uint32 unit, uint32 sds, uint32 *pBerNotify)
{
    int32   ret = RT_ERR_OK;

    /* parameter check */
    RT_PARAM_CHK((!HWP_SDS_EXIST(unit, sds)), RT_ERR_OUT_OF_RANGE);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SDS), "unit=%d,sds=%d", unit, sds);

    if(!HWP_SDS_10G(unit, sds))
    {
        RT_LOG(LOG_INFO, (MOD_DAL|MOD_SDS), "unit=%d,sds=%d is NoT 10G SDS", unit, sds);
        return RT_ERR_OK;
    }

    /* function body */
    SDS_SEM_LOCK(unit);

    if(0 == (sds %2))
    {
        RT_ERR_HDL(SDS_FIELD_W(unit, sds, 0x1f, 0x2, 15, 0, 53), ERR, ret);
        RT_ERR_HDL(SDS_FIELD_R(unit, sds, 0x1f, 0x14, 2, 2, pBerNotify), ERR, ret);
    }
    else
    {
        RT_ERR_HDL(SDS_FIELD_W(unit, sds - 1, 0x1f, 0x2, 15, 0, 53), ERR, ret);
        RT_ERR_HDL(SDS_FIELD_R(unit, sds - 1, 0x1f, 0x14, 3, 3, pBerNotify), ERR, ret);
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SDS), "pBerNotify=%d", *pBerNotify);
ERR:
    SDS_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      drv_phy_smiCmdComplete_wait
 * Description:
 *      MAC access PHY wait command ready
 * Input:
 *      unit  - unit id
 *      ctrl_reg  - SMI access PHY control reg
 *      cmd_mask  - command complete bit mask
 *      fail_mask - fail bit mask
 *      timeout   - timeout time in milli-second
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
int32
_dal_longan_sds_10gRxIdleRdy_wait(int32 unit, uint32 sdsId, uint32 timeout)
{
    osal_usecs_t    t, now, t_wait=0;
    uint32          rxIdle = 0;
    int32           chkCnt=0;

    osal_time_usecs_get(&t);
    do {
        chkCnt++;
        osal_time_usecs_get(&now);
        if (dal_longan_sds_10gRxIdle_get(unit, sdsId, &rxIdle) != RT_ERR_OK)
        {
            return RT_ERR_FAILED;
        }

        if (0 == rxIdle)
        {
            return RT_ERR_OK;
        }

        t_wait += ((now >= t) ? (now - t) : (0xFFFFFFFF - t + now));
        t = now;
    } while ((t_wait <= timeout));

    if (t_wait > timeout)
    {
        return RT_ERR_TIMEOUT;
    }

    return RT_ERR_OK;
}

static int32
_dal_longan_sds_testDis_set(uint32 unit, uint32 sds)
{
    int32   ret;

    /* PRBS9 */
    RT_ERR_CHK(SDS_FIELD_W(unit, sds, 0x5, 0xa, 7, 6, 0x0), ret);
    RT_ERR_CHK(SDS_FIELD_W(unit, sds, 0x6, 0x0, 12, 8, 0x0), ret);
    RT_ERR_CHK(SDS_FIELD_W(unit, sds, 0x6, 0x0, 14, 13, 0x0), ret);
    RT_ERR_CHK(SDS_FIELD_W(unit, sds, 0x6, 0xe, 8, 0, 0x0), ret);
    RT_ERR_CHK(SDS_FIELD_W(unit, sds, 0x6, 0xf, 15, 0, 0x0), ret);

    /* PRBS31 */
    RT_ERR_CHK(SDS_FIELD_W(unit, sds, 0x5, 0xa, 5, 4, 0x0), ret);
    RT_ERR_CHK(SDS_FIELD_W(unit, sds, 0x6, 0x0, 12, 8, 0x0), ret);
    RT_ERR_CHK(SDS_FIELD_W(unit, sds, 0x6, 0x0, 14, 13, 0x0), ret);
    RT_ERR_CHK(SDS_FIELD_W(unit, sds, 0x6, 0xe, 8, 0, 0x0), ret);
    RT_ERR_CHK(SDS_FIELD_W(unit, sds, 0x6, 0xf, 15, 0, 0x0), ret);

    /* Square 8 */
    RT_ERR_CHK(SDS_FIELD_W(unit, sds, 0x5, 0xa, 3, 3, 0x0), ret);
    RT_ERR_CHK(SDS_FIELD_W(unit, sds, 0x5, 0xa, 1, 1, 0x0), ret);

    RT_ERR_CHK(SDS_FIELD_W(unit, sds, 0x6, 0x0, 13, 13, 0x0), ret);
    RT_ERR_CHK(SDS_FIELD_W(unit, sds, 0x6, 0x0, 7, 4, 0x0), ret);

    return RT_ERR_OK;
}   /* end of _dal_longan_sds_testDis_set */

static int32
_dal_longan_sds_testPrbs9_set(uint32 unit, uint32 sds)
{
    int32   ret;

    RT_ERR_CHK(SDS_FIELD_W(unit, sds, 0x5, 0xa, 13, 4, 0x0), ret);
    RT_ERR_CHK(SDS_FIELD_W(unit, sds, 0x5, 0xa, 1, 1, 0x0), ret);

    RT_ERR_CHK(SDS_FIELD_W(unit, sds, 0x5, 0xa, 7, 6, 0x3), ret);
    RT_ERR_CHK(SDS_FIELD_W(unit, sds, 0x6, 0x0, 12, 8, 0x2), ret);
    RT_ERR_CHK(SDS_FIELD_W(unit, sds, 0x6, 0x0, 14, 13, 0x3), ret);
    RT_ERR_CHK(SDS_FIELD_W(unit, sds, 0x6, 0xe, 8, 0, 0x2), ret);
    RT_ERR_CHK(SDS_FIELD_W(unit, sds, 0x6, 0xf, 15, 0, 0x2), ret);

    return RT_ERR_OK;
}   /* end of _dal_longan_sds_testPrbs9_set */

static int32
_dal_longan_sds_testPrbs31_set(uint32 unit, uint32 sds)
{
    int32   ret;

    RT_ERR_CHK(SDS_FIELD_W(unit, sds, 0x5, 0xa, 13, 4, 0x0), ret);
    RT_ERR_CHK(SDS_FIELD_W(unit, sds, 0x5, 0xa, 1, 1, 0x0), ret);

    RT_ERR_CHK(SDS_FIELD_W(unit, sds, 0x5, 0xa, 5, 4, 0x3), ret);
    RT_ERR_CHK(SDS_FIELD_W(unit, sds, 0x6, 0x0, 12, 8, 0x2), ret);
    RT_ERR_CHK(SDS_FIELD_W(unit, sds, 0x6, 0x0, 14, 13, 0x3), ret);
    RT_ERR_CHK(SDS_FIELD_W(unit, sds, 0x6, 0xe, 8, 0, 0x2), ret);
    RT_ERR_CHK(SDS_FIELD_W(unit, sds, 0x6, 0xf, 15, 0, 0x2), ret);

    return RT_ERR_OK;
}   /* end of _dal_longan_sds_testPrbs31_set */

static int32
_dal_longan_sds_testSquare8_set(uint32 unit, uint32 sds)
{
    int32   ret;

    RT_ERR_CHK(SDS_FIELD_W(unit, sds, 0x5, 0xa, 3, 3, 0x1), ret);
    RT_ERR_CHK(SDS_FIELD_W(unit, sds, 0x5, 0xa, 1, 1, 0x1), ret);

    RT_ERR_CHK(SDS_FIELD_W(unit, sds, 0x6, 0x0, 13, 13, 0x1), ret);
    RT_ERR_CHK(SDS_FIELD_W(unit, sds, 0x6, 0x0, 7, 4, 0x8), ret);

    return RT_ERR_OK;
}   /* end of _dal_longan_sds_testSquare8_set */

/* Function Name:
 *      dal_longan_sds_10gEyeParam_load
 * Description:
 *      Load 10g SerDes eye parameter
 * Input:
 *      unit - unit id
 *      sds  - SerDes id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
int32
 dal_longan_sds_10gEyeParam_load(uint32 unit, uint32 sds)
{
    sds_tx_param_t  *tx_param = NULL;
    rtk_port_10gMedia_t  media;
    rtk_port_t port;
    int32 ret;

    if(!HWP_SDS_10G(unit, sds))
        return RT_ERR_OK;

    if(RTK_MII_10GR != HWP_SDS_MODE(unit, sds))
        return RT_ERR_OK;

    port = HWP_SDS_ID2MACID(unit, sds);

    if(HWP_NONE == port || (!HWP_SERDES_PORT(unit, port)))
        return RT_ERR_OK;

    if((ret =phy_9300_10gmedia_get( unit, port, &media)) != RT_ERR_OK)
    {
        return ret;
    }

    switch(media)
    {
        case PORT_10GMEDIA_FIBER_10G:
            tx_param = &fiber10g_8431_param[unit][sds];
            break;
        case PORT_10GMEDIA_DAC_50CM:
            tx_param = &dac50cm_tx_param[unit][sds];
            break;
        case PORT_10GMEDIA_DAC_100CM:
            tx_param = &dac100cm_tx_param[unit][sds];
            break;
        case PORT_10GMEDIA_DAC_300CM:
            tx_param = &dac300cm_tx_param[unit][sds];
            break;
        case PORT_10GMEDIA_DAC_500CM:
            tx_param = &dac500cm_tx_param[unit][sds];
            break;
        default:
            return RT_ERR_OK;
    }

    if(tx_param->eyeParam_valid)
    {
        if((ret =dal_longan_sds_eyeParam_set(unit, sds,  tx_param->eyeParam)) != RT_ERR_OK)
            return ret;
    }

    if(tx_param->tx_param_misc != NULL)
    {
        while(tx_param->tx_param_misc->page != RTK_SDS_PAGEREGVAL_END)
        {
            if((ret =hal_serdes_reg_set(unit, sds,
                            tx_param->tx_param_misc->page,
                            tx_param->tx_param_misc->reg,
                            tx_param->tx_param_misc->data)) != RT_ERR_OK)
            {
                return ret;
            }

            tx_param->tx_param_misc++;
        }
    }
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_sds_testMode_set
 * Description:
 *      Set SerDes test mode.
 * Input:
 *      unit      - unit id
 *      sds       - SerDes id
 *      mode      - test mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 * Note:
 *      None
 */
int32
dal_longan_sds_testMode_set(uint32 unit, uint32 sds, rtk_sds_testMode_t mode)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SDS), "unit=%d,sds=%d,mode=%d", unit, sds, mode);

    /* check Init status */
    RT_INIT_CHK(sds_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_SDS_EXIST(unit, sds), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((RTK_SDS_TESTMODE_END <= mode), RT_ERR_INPUT);

    /* function body */
    SDS_SEM_LOCK(unit);

    switch (mode)
    {
        case RTK_SDS_TESTMODE_DISABLE:
            RT_ERR_HDL(_dal_longan_sds_testDis_set(unit, sds), ERR, ret);
            break;
        case RTK_SDS_TESTMODE_PRBS9:
            RT_ERR_HDL(_dal_longan_sds_testPrbs9_set(unit, sds), ERR, ret);
            break;
        case RTK_SDS_TESTMODE_PRBS31:
            RT_ERR_HDL(_dal_longan_sds_testPrbs31_set(unit, sds), ERR, ret);
            break;
        case RTK_SDS_TESTMODE_SQUARE8:
            RT_ERR_HDL(_dal_longan_sds_testSquare8_set(unit, sds), ERR, ret);
            break;
        default:
            RT_LOG(LOG_DEBUG, (MOD_PIE|MOD_DAL), "mode=%u", mode);
            SDS_SEM_UNLOCK(unit);
            return RT_ERR_FAILED;
    }
ERR:
    SDS_SEM_UNLOCK(unit);

    testMode[unit][sds] = mode;

    dal_longan_sds_10gEyeParam_load(unit, sds);

    return ret;
}   /* end of dal_longan_sds_testMode_set */

/* Function Name:
 *      dal_longan_sds_eyeParam_get
 * Description:
 *      Get SerDes eye parameter
 * Input:
 *      unit - unit id
 *      sds  - SerDes id
 * Output:
 *      pParam - eye parameter
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
int32
dal_longan_sds_eyeParam_get(uint32 unit, uint32 sds, rtk_sds_eyeParam_t  *pParam)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SDS), "unit=%d,sds=%d", unit, sds);

    /* check Init status */
    RT_INIT_CHK(sds_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_SDS_EXIST(unit, sds), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pParam), RT_ERR_NULL_POINTER);

    osal_memset(pParam, 0x0, sizeof(rtk_sds_eyeParam_t));

    /* function body */
    SDS_SEM_LOCK(unit);

    if(HWP_SDS_10G(unit, sds))
    {
        switch(HWP_SDS_MODE(unit, sds))
        {
            case RTK_MII_100BX_FIBER:
            case RTK_MII_1000BX_FIBER:
            case RTK_MII_SGMII:
                RT_ERR_HDL(SDS_FIELD_R(unit, sds, 0x25, 0x1, 15, 11, &pParam->pre_amp), ERR, ret);
                RT_ERR_HDL(SDS_FIELD_R(unit, sds, 0x25, 0x7, 0, 0, &pParam->pre_en), ERR, ret);
                RT_ERR_HDL(SDS_FIELD_R(unit, sds, 0x25, 0x7, 8, 4, &pParam->main_amp), ERR, ret);
                RT_ERR_HDL(SDS_FIELD_R(unit, sds, 0x25, 0x6, 4, 0, &pParam->post_amp), ERR, ret);
                RT_ERR_HDL(SDS_FIELD_R(unit, sds, 0x25, 0x7, 3, 3, &pParam->post_en), ERR, ret);
                RT_ERR_HDL(SDS_FIELD_R(unit, sds, 0x25, 0x18, 15, 12, &pParam->impedance), ERR, ret);
                break;
            case RTK_MII_RSGMII:
                RT_ERR_HDL(SDS_FIELD_R(unit, sds, 0x27, 0x1, 15, 11, &pParam->pre_amp), ERR, ret);
                RT_ERR_HDL(SDS_FIELD_R(unit, sds, 0x27, 0x7, 0, 0, &pParam->pre_en), ERR, ret);
                RT_ERR_HDL(SDS_FIELD_R(unit, sds, 0x27, 0x7, 8, 4, &pParam->main_amp), ERR, ret);
                RT_ERR_HDL(SDS_FIELD_R(unit, sds, 0x27, 0x6, 4, 0, &pParam->post_amp), ERR, ret);
                RT_ERR_HDL(SDS_FIELD_R(unit, sds, 0x27, 0x7, 3, 3, &pParam->post_en), ERR, ret);
                RT_ERR_HDL(SDS_FIELD_R(unit, sds, 0x27, 0x18, 15, 12, &pParam->impedance), ERR, ret);
                break;
            case RTK_MII_2500Base_X:
            case RTK_MII_HISGMII:
                RT_ERR_HDL(SDS_FIELD_R(unit, sds, 0x29, 0x1, 15, 11, &pParam->pre_amp), ERR, ret);
                RT_ERR_HDL(SDS_FIELD_R(unit, sds, 0x29, 0x7, 0, 0, &pParam->pre_en), ERR, ret);
                RT_ERR_HDL(SDS_FIELD_R(unit, sds, 0x29, 0x7, 8, 4, &pParam->main_amp), ERR, ret);
                RT_ERR_HDL(SDS_FIELD_R(unit, sds, 0x29, 0x6, 4, 0, &pParam->post_amp), ERR, ret);
                RT_ERR_HDL(SDS_FIELD_R(unit, sds, 0x29, 0x7, 3, 3, &pParam->post_en), ERR, ret);
                RT_ERR_HDL(SDS_FIELD_R(unit, sds, 0x29, 0x18, 15, 12, &pParam->impedance), ERR, ret);
                break;
            case RTK_MII_QSGMII:
                RT_ERR_HDL(SDS_FIELD_R(unit, sds, 0x2b, 0x1, 15, 11, &pParam->pre_amp), ERR, ret);
                RT_ERR_HDL(SDS_FIELD_R(unit, sds, 0x2b, 0x7, 0, 0, &pParam->pre_en), ERR, ret);
                RT_ERR_HDL(SDS_FIELD_R(unit, sds, 0x2b, 0x7, 8, 4, &pParam->main_amp), ERR, ret);
                RT_ERR_HDL(SDS_FIELD_R(unit, sds, 0x2b, 0x6, 4, 0, &pParam->post_amp), ERR, ret);
                RT_ERR_HDL(SDS_FIELD_R(unit, sds, 0x2b, 0x7, 3, 3, &pParam->post_en), ERR, ret);
                RT_ERR_HDL(SDS_FIELD_R(unit, sds, 0x2b, 0x18, 15, 12, &pParam->impedance), ERR, ret);
                break;
            case RTK_MII_RXAUI_LITE:
            case RTK_MII_RXAUI_PLUS:
                RT_ERR_HDL(SDS_FIELD_R(unit, sds, 0x2d, 0x1, 15, 11, &pParam->pre_amp), ERR, ret);
                RT_ERR_HDL(SDS_FIELD_R(unit, sds, 0x2d, 0x7, 0, 0, &pParam->pre_en), ERR, ret);
                RT_ERR_HDL(SDS_FIELD_R(unit, sds, 0x2d, 0x7, 8, 4, &pParam->main_amp), ERR, ret);
                RT_ERR_HDL(SDS_FIELD_R(unit, sds, 0x2d, 0x6, 4, 0, &pParam->post_amp), ERR, ret);
                RT_ERR_HDL(SDS_FIELD_R(unit, sds, 0x2d, 0x7, 3, 3, &pParam->post_en), ERR, ret);
                RT_ERR_HDL(SDS_FIELD_R(unit, sds, 0x2d, 0x18, 15, 12, &pParam->impedance), ERR, ret);
                break;
            case RTK_MII_XSGMII:
            case RTK_MII_QHSGMII:
            case RTK_MII_10GR:
            case RTK_MII_USXGMII_10GSXGMII:
            case RTK_MII_USXGMII_10GDXGMII:
            case RTK_MII_USXGMII_10GQXGMII:
                RT_ERR_HDL(SDS_FIELD_R(unit, sds, 0x2f, 0x1, 15, 11, &pParam->pre_amp), ERR, ret);
                RT_ERR_HDL(SDS_FIELD_R(unit, sds, 0x2f, 0x7, 0, 0, &pParam->pre_en), ERR, ret);
                RT_ERR_HDL(SDS_FIELD_R(unit, sds, 0x2f, 0x7, 8, 4, &pParam->main_amp), ERR, ret);
                RT_ERR_HDL(SDS_FIELD_R(unit, sds, 0x2f, 0x6, 4, 0, &pParam->post_amp), ERR, ret);
                RT_ERR_HDL(SDS_FIELD_R(unit, sds, 0x2f, 0x7, 3, 3, &pParam->post_en), ERR, ret);
                RT_ERR_HDL(SDS_FIELD_R(unit, sds, 0x2f, 0x18, 15, 12, &pParam->impedance), ERR, ret);
                break;
            default:
                RT_LOG(LOG_DEBUG, (MOD_PIE|MOD_DAL), "sds mode=%u", HWP_SDS_MODE(unit, sds));
                SDS_SEM_UNLOCK(unit);
                return RT_ERR_FAILED;
        }
    }
    else
    {
        RT_ERR_HDL(SDS_FIELD_R(unit, sds, 0x2a, 0x2, 12, 10, &pParam->main_amp), ERR, ret);
    }
ERR:
    SDS_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_longan_sds_eyeParam_get */
/* Function Name:
 *      _dal_longan_sds_txParam_config_save
 * Description:
 *      Save SerDes eye parameters
 * Input:
 *      unit - unit id
 *      sds  - SerDes id
 *      param  - eye parameter value
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */

void
_dal_longan_sds_txParam_config_save(uint32 unit, uint32 sds, rtk_sds_eyeParam_t  param)
{
    rtk_port_10gMedia_t  media;
    rtk_port_t port;

    port = HWP_SDS_ID2MACID(unit, sds);

    if(HWP_10GE_SERDES_PORT(unit, port))
    {
        phy_9300_10gmedia_get( unit, port, &media);

        switch(media)
        {
            case PORT_10GMEDIA_FIBER_10G:
                fiber10g_8431_param[unit][sds].eyeParam.main_amp = param.main_amp;
                fiber10g_8431_param[unit][sds].eyeParam.post_amp = param.post_amp;
                fiber10g_8431_param[unit][sds].eyeParam.post_en = param.post_en;
                fiber10g_8431_param[unit][sds].eyeParam.pre_amp = param.pre_amp;
                fiber10g_8431_param[unit][sds].eyeParam.pre_en = param.pre_en;
                fiber10g_8431_param[unit][sds].eyeParam.impedance = param.impedance;
                break;
            case PORT_10GMEDIA_DAC_50CM:
                dac50cm_tx_param[unit][sds].eyeParam.main_amp = param.main_amp;
                dac50cm_tx_param[unit][sds].eyeParam.post_amp = param.post_amp;
                dac50cm_tx_param[unit][sds].eyeParam.post_en = param.post_en;
                dac50cm_tx_param[unit][sds].eyeParam.pre_amp = param.pre_amp;
                dac50cm_tx_param[unit][sds].eyeParam.pre_en = param.pre_en;
                dac50cm_tx_param[unit][sds].eyeParam.impedance = param.impedance;
                break;
            case PORT_10GMEDIA_DAC_100CM:
                dac100cm_tx_param[unit][sds].eyeParam.main_amp = param.main_amp;
                dac100cm_tx_param[unit][sds].eyeParam.post_amp = param.post_amp;
                dac100cm_tx_param[unit][sds].eyeParam.post_en = param.post_en;
                dac100cm_tx_param[unit][sds].eyeParam.pre_amp = param.pre_amp;
                dac100cm_tx_param[unit][sds].eyeParam.pre_en = param.pre_en;
                dac100cm_tx_param[unit][sds].eyeParam.impedance = param.impedance;
                break;
            case PORT_10GMEDIA_DAC_300CM:
                dac300cm_tx_param[unit][sds].eyeParam.main_amp = param.main_amp;
                dac300cm_tx_param[unit][sds].eyeParam.post_amp = param.post_amp;
                dac300cm_tx_param[unit][sds].eyeParam.post_en = param.post_en;
                dac300cm_tx_param[unit][sds].eyeParam.pre_amp = param.pre_amp;
                dac300cm_tx_param[unit][sds].eyeParam.pre_en = param.pre_en;
                dac300cm_tx_param[unit][sds].eyeParam.impedance = param.impedance;
                break;
            case PORT_10GMEDIA_DAC_500CM:
                dac500cm_tx_param[unit][sds].eyeParam.main_amp = param.main_amp;
                dac500cm_tx_param[unit][sds].eyeParam.post_amp = param.post_amp;
                dac500cm_tx_param[unit][sds].eyeParam.post_en = param.post_en;
                dac500cm_tx_param[unit][sds].eyeParam.pre_amp = param.pre_amp;
                dac500cm_tx_param[unit][sds].eyeParam.pre_en = param.pre_en;
                dac500cm_tx_param[unit][sds].eyeParam.impedance = param.impedance;
                break;
            default:
                RT_LOG(LOG_INFO, (MOD_DAL|MOD_SDS), " do not save tx param ,media=%d",  media);
        }
    }
}

/* Function Name:
 *      dal_longan_sds_eyeParam_set
 * Description:
 *      Set SerDes eye parameters
 * Input:
 *      unit - unit id
 *      sds  - SerDes id
 *      param  - eye parameter value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
int32
dal_longan_sds_eyeParam_set(uint32 unit, uint32 sds, rtk_sds_eyeParam_t  param)
{
    int32   ret;


    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SDS), "unit=%d,sds=%d,param=%d", unit, sds, param);
    /* check Init status */
    RT_INIT_CHK(sds_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_SDS_EXIST(unit, sds), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(((HWP_SDS_10G(unit, sds)) &&
            (param.pre_en >= RTK_ENABLE_END)), RT_ERR_INPUT);
    RT_PARAM_CHK(((HWP_SDS_10G(unit, sds)) &&
            (param.post_en >= RTK_ENABLE_END)), RT_ERR_INPUT);

    /* function body */
    SDS_SEM_LOCK(unit);

    if(HWP_SDS_10G(unit, sds))
    {
        switch(HWP_SDS_MODE(unit, sds))
        {
            case RTK_MII_100BX_FIBER:
            case RTK_MII_1000BX_FIBER:
            case RTK_MII_SGMII:
                RT_ERR_HDL(SDS_FIELD_W(unit, sds, 0x25, 0x1, 15, 11, param.pre_amp), ERR, ret);
                RT_ERR_HDL(SDS_FIELD_W(unit, sds, 0x25, 0x7, 0, 0, param.pre_en), ERR, ret);
                RT_ERR_HDL(SDS_FIELD_W(unit, sds, 0x25, 0x7, 8, 4, param.main_amp), ERR, ret);
                RT_ERR_HDL(SDS_FIELD_W(unit, sds, 0x25, 0x6, 4, 0, param.post_amp), ERR, ret);
                RT_ERR_HDL(SDS_FIELD_W(unit, sds, 0x25, 0x7, 3, 3, param.post_en), ERR, ret);
                RT_ERR_HDL(SDS_FIELD_W(unit, sds, 0x25, 0x18, 15, 12, param.impedance), ERR, ret);
                break;
            case RTK_MII_RSGMII:
                RT_ERR_HDL(SDS_FIELD_W(unit, sds, 0x27, 0x1, 15, 11, param.pre_amp), ERR, ret);
                RT_ERR_HDL(SDS_FIELD_W(unit, sds, 0x27, 0x7, 0, 0, param.pre_en), ERR, ret);
                RT_ERR_HDL(SDS_FIELD_W(unit, sds, 0x27, 0x7, 8, 4, param.main_amp), ERR, ret);
                RT_ERR_HDL(SDS_FIELD_W(unit, sds, 0x27, 0x6, 4, 0, param.post_amp), ERR, ret);
                RT_ERR_HDL(SDS_FIELD_W(unit, sds, 0x27, 0x7, 3, 3, param.post_en), ERR, ret);
                RT_ERR_HDL(SDS_FIELD_W(unit, sds, 0x27, 0x18, 15, 12, param.impedance), ERR, ret);
                break;
            case RTK_MII_2500Base_X:
            case RTK_MII_HISGMII:
                RT_ERR_HDL(SDS_FIELD_W(unit, sds, 0x29, 0x1, 15, 11, param.pre_amp), ERR, ret);
                RT_ERR_HDL(SDS_FIELD_W(unit, sds, 0x29, 0x7, 0, 0, param.pre_en), ERR, ret);
                RT_ERR_HDL(SDS_FIELD_W(unit, sds, 0x29, 0x7, 8, 4, param.main_amp), ERR, ret);
                RT_ERR_HDL(SDS_FIELD_W(unit, sds, 0x29, 0x6, 4, 0, param.post_amp), ERR, ret);
                RT_ERR_HDL(SDS_FIELD_W(unit, sds, 0x29, 0x7, 3, 3, param.post_en), ERR, ret);
                RT_ERR_HDL(SDS_FIELD_W(unit, sds, 0x29, 0x18, 15, 12, param.impedance), ERR, ret);
                break;
            case RTK_MII_QSGMII:
                RT_ERR_HDL(SDS_FIELD_W(unit, sds, 0x2b, 0x1, 15, 11, param.pre_amp), ERR, ret);
                RT_ERR_HDL(SDS_FIELD_W(unit, sds, 0x2b, 0x7, 0, 0, param.pre_en), ERR, ret);
                RT_ERR_HDL(SDS_FIELD_W(unit, sds, 0x2b, 0x7, 8, 4, param.main_amp), ERR, ret);
                RT_ERR_HDL(SDS_FIELD_W(unit, sds, 0x2b, 0x6, 4, 0, param.post_amp), ERR, ret);
                RT_ERR_HDL(SDS_FIELD_W(unit, sds, 0x2b, 0x7, 3, 3, param.post_en), ERR, ret);
                RT_ERR_HDL(SDS_FIELD_W(unit, sds, 0x2b, 0x18, 15, 12, param.impedance), ERR, ret);
                break;
            case RTK_MII_RXAUI_LITE:
            case RTK_MII_RXAUI_PLUS:
                RT_ERR_HDL(SDS_FIELD_W(unit, sds, 0x2d, 0x1, 15, 11, param.pre_amp), ERR, ret);
                RT_ERR_HDL(SDS_FIELD_W(unit, sds, 0x2d, 0x7, 0, 0, param.pre_en), ERR, ret);
                RT_ERR_HDL(SDS_FIELD_W(unit, sds, 0x2d, 0x7, 8, 4, param.main_amp), ERR, ret);
                RT_ERR_HDL(SDS_FIELD_W(unit, sds, 0x2d, 0x6, 4, 0, param.post_amp), ERR, ret);
                RT_ERR_HDL(SDS_FIELD_W(unit, sds, 0x2d, 0x7, 3, 3, param.post_en), ERR, ret);
                RT_ERR_HDL(SDS_FIELD_W(unit, sds, 0x2d, 0x18, 15, 12, param.impedance), ERR, ret);
                break;
            case RTK_MII_XSGMII:
            case RTK_MII_QHSGMII:
            case RTK_MII_10GR:
            case RTK_MII_USXGMII_10GSXGMII:
            case RTK_MII_USXGMII_10GDXGMII:
            case RTK_MII_USXGMII_10GQXGMII:
                RT_ERR_HDL(SDS_FIELD_W(unit, sds, 0x2f, 0x1, 15, 11, param.pre_amp), ERR, ret);
                RT_ERR_HDL(SDS_FIELD_W(unit, sds, 0x2f, 0x7, 0, 0, param.pre_en), ERR, ret);
                RT_ERR_HDL(SDS_FIELD_W(unit, sds, 0x2f, 0x7, 8, 4, param.main_amp), ERR, ret);
                RT_ERR_HDL(SDS_FIELD_W(unit, sds, 0x2f, 0x6, 4, 0, param.post_amp), ERR, ret);
                RT_ERR_HDL(SDS_FIELD_W(unit, sds, 0x2f, 0x7, 3, 3, param.post_en), ERR, ret);
                RT_ERR_HDL(SDS_FIELD_W(unit, sds, 0x2f, 0x18, 15, 12, param.impedance), ERR, ret);
                break;
            default:
                RT_LOG(LOG_DEBUG, (MOD_PIE|MOD_DAL), "sds mode=%u", HWP_SDS_MODE(unit, sds));
                SDS_SEM_UNLOCK(unit);
                _dal_longan_sds_txParam_config_save(unit,sds,param);
                return RT_ERR_FAILED;
        }
    }
    else
    {
        RT_ERR_HDL(SDS_FIELD_W(unit, sds, 0x2a, 0x2, 12, 10, param.main_amp), ERR, ret);
    }
ERR:
    SDS_SEM_UNLOCK(unit);
    _dal_longan_sds_txParam_config_save(unit,sds,param);
    return ret;
}   /* end of dal_longan_sds_eyeParam_set */

void
_dal_longan_sds_rxCali_foregroundOffsetCal_Read(uint32 unit, uint32 sdsId, uint32 *fgcal_list)
{
    uint32 fpcal_binary;
    uint32 fpcal_gray;


    if(0 == (sdsId %2))
        hal_serdes_reg_set(unit, sdsId, 0x1f, 0x2, 0x2f);
    else
        hal_serdes_reg_set(unit, sdsId - 1, 0x1f, 0x2, 0x31);

     //##Page0x2E, Reg0x15[9], REG0_RX_EN_TEST=[1]
    SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x15, 9, 9, 0x1);
     //##Page0x21, Reg0x06[11 6], REG0_RX_DEBUG_SEL=[1 0 x x x x]
    SDS_FIELD_WRITE(unit, sdsId, 0x21, 0x06, 11, 6, 0x20);
     //##Page0x2F, Reg0x0C[5 0], REG0_COEF_SEL=[0 0 1 1 1 1]
    SDS_FIELD_WRITE(unit, sdsId, 0x2f, 0x0c, 5, 0, 0xf);
     //##FGCAL Gray Read Out
    SDS_FIELD_READ(unit, sdsId, 0x1f, 0x14, 5, 0, &fpcal_gray);
     //##Page0x2F, Reg0x0C[5 0], REG0_COEF_SEL=[0 0 1 1 1 0]
    SDS_FIELD_WRITE(unit, sdsId, 0x2f, 0x0c, 5, 0, 0xe);
     //##FGCAL binary Read Out
    SDS_FIELD_READ(unit, sdsId, 0x1f, 0x14, 5, 0, &fpcal_binary);

    PATCH_PRINT("fpcal_binary:%u,fpcal_gray:%u",fpcal_binary,fpcal_gray);

    fgcal_list[0] = fpcal_binary;
    fgcal_list[1] = fpcal_gray;
}

void
_dal_longan_sds_rxCali_DCVS_Manual(uint32 unit, uint32 sdsId, uint32 dcvsId, uint32 manual, uint32 *pDvcsList)
{
    if(manual == 1)
    {
        switch(dcvsId)
        {
            case SDS_DVCS0:
                SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x1e, 14, 14, 0x1);
                SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x1c, 4, 4, *(pDvcsList + 0));
                SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x1c, 3, 0, *(pDvcsList + 1));
                break;
            case SDS_DVCS1:
                SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x1e, 13, 13, 0x1);
                SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x1d, 15, 15, *(pDvcsList + 0));
                SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x1d, 14, 11, *(pDvcsList + 1));
                break;
            case SDS_DVCS2:
                SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x1e, 12, 12, 0x1);
                SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x1d, 10, 10, *(pDvcsList + 0));
                SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x1d, 9, 6, *(pDvcsList + 1));
                break;
            case SDS_DVCS3:
                SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x1e, 11, 11, 0x1);
                SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x1d, 5, 5, *(pDvcsList + 0));
                SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x1d, 4, 1, *(pDvcsList + 1));
                break;
            case SDS_DVCS4:
                SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x01, 15, 15, 0x1);
                SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x02, 10, 10, *(pDvcsList + 0));
                SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x02, 9, 6, *(pDvcsList + 1));
                break;
            case SDS_DVCS5:
                SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x02, 11, 11, 0x1);
                SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x11, 4, 4, *(pDvcsList + 0));
                SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x11, 3, 0, *(pDvcsList + 1));
                break;
            default:
                break;
        }
    }
    else
    {
        switch(dcvsId)
        {
            case SDS_DVCS0:
                SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x1e, 14, 14, 0x0);
                break;
            case SDS_DVCS1:
                SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x1e, 13, 13, 0x0);
                break;
            case SDS_DVCS2:
                SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x1e, 12, 12, 0x0);
                break;
            case SDS_DVCS3:
                SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x1e, 11, 11, 0x0);
                break;
            case SDS_DVCS4:
                SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x01, 15, 15, 0x0);
                break;
            case SDS_DVCS5:
                SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x02, 11, 11, 0x0);
                break;
            default:
                break;
        }
        osal_time_mdelay(1);
    }
}

void
_dal_longan_sds_rxCali_DCVS_Read(uint32 unit, uint32 sdsId, uint32 dcvsId, uint32 *pDcvsList)
{
    uint32 dcvs_sign_out = 0;
    uint32 dcvs_coef_bin = 0;
    uint32 dcvs_manual = 0;

    if(0 == (sdsId %2))
        hal_serdes_reg_set(unit, sdsId, 0x1f, 0x2, 0x2f);
    else
        hal_serdes_reg_set(unit, sdsId - 1, 0x1f, 0x2, 0x31);

     //##Page0x2E, Reg0x15[9], REG0_RX_EN_TEST=[1]
    SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x15, 9, 9, 0x1);
    //##Page0x21, Reg0x06[11 6], REG0_RX_DEBUG_SEL=[1 0 x x x x]
    SDS_FIELD_WRITE(unit, sdsId, 0x21, 0x06, 11, 6, 0x20);

    switch(dcvsId)
    {
        case SDS_DVCS0:
            SDS_FIELD_WRITE(unit, sdsId, 0x2f, 0x0c, 5, 0, 0x22);
            osal_time_mdelay(1);
            //##DCVS0 Read Out
            SDS_FIELD_READ(unit, sdsId, 0x1f, 0x14, 4, 4, &dcvs_sign_out);
            SDS_FIELD_READ(unit, sdsId, 0x1f, 0x14, 3, 0, &dcvs_coef_bin);
            SDS_FIELD_READ(unit, sdsId, 0x2e, 0x1e, 14, 14, &dcvs_manual);
            break;
        case SDS_DVCS1:
            SDS_FIELD_WRITE(unit, sdsId, 0x2f, 0x0c, 5, 0, 0x23);
            osal_time_mdelay(1);
            //##DCVS0 Read Out
            SDS_FIELD_READ(unit, sdsId, 0x1f, 0x14, 4, 4, &dcvs_sign_out);
            SDS_FIELD_READ(unit, sdsId, 0x1f, 0x14, 3, 0, &dcvs_coef_bin);
            SDS_FIELD_READ(unit, sdsId, 0x2e, 0x1e, 13, 13, &dcvs_manual);
            break;
        case SDS_DVCS2:
            SDS_FIELD_WRITE(unit, sdsId, 0x2f, 0x0c, 5, 0, 0x24);
            osal_time_mdelay(1);
            //##DCVS0 Read Out
            SDS_FIELD_READ(unit, sdsId, 0x1f, 0x14, 4, 4, &dcvs_sign_out);
            SDS_FIELD_READ(unit, sdsId, 0x1f, 0x14, 3, 0, &dcvs_coef_bin);
            SDS_FIELD_READ(unit, sdsId, 0x2e, 0x1e, 12, 12, &dcvs_manual);
            break;
        case SDS_DVCS3:
            SDS_FIELD_WRITE(unit, sdsId, 0x2f, 0x0c, 5, 0, 0x25);
            osal_time_mdelay(1);
            //##DCVS0 Read Out
            SDS_FIELD_READ(unit, sdsId, 0x1f, 0x14, 4, 4, &dcvs_sign_out);
            SDS_FIELD_READ(unit, sdsId, 0x1f, 0x14, 3, 0, &dcvs_coef_bin);
            SDS_FIELD_READ(unit, sdsId, 0x2e, 0x1e, 11, 11, &dcvs_manual);
            break;
        case SDS_DVCS4:
            SDS_FIELD_WRITE(unit, sdsId, 0x2f, 0x0c, 5, 0, 0x2c);
            osal_time_mdelay(1);
            //##DCVS0 Read Out
            SDS_FIELD_READ(unit, sdsId, 0x1f, 0x14, 4, 4, &dcvs_sign_out);
            SDS_FIELD_READ(unit, sdsId, 0x1f, 0x14, 3, 0, &dcvs_coef_bin);
            SDS_FIELD_READ(unit, sdsId, 0x2e, 0x01, 15, 15, &dcvs_manual);
            break;
        case SDS_DVCS5:
            SDS_FIELD_WRITE(unit, sdsId, 0x2f, 0x0c, 5, 0, 0x2d);
            osal_time_mdelay(1);
            //##DCVS0 Read Out
            SDS_FIELD_READ(unit, sdsId, 0x1f, 0x14, 4, 4, &dcvs_sign_out);
            SDS_FIELD_READ(unit, sdsId, 0x1f, 0x14, 3, 0, &dcvs_coef_bin);
            SDS_FIELD_READ(unit, sdsId, 0x2e, 0x02, 11, 11, &dcvs_manual);
            break;
        default:
            break;
    }

    if(1 == dcvs_sign_out)
        PATCH_PRINT("DCVS %u Sign: -", dcvsId);
    else
        PATCH_PRINT("DCVS %u Sign: +", dcvsId);

    PATCH_PRINT("DCVS %u Even Coef = %u", dcvsId,dcvs_coef_bin);
    PATCH_PRINT("DCVS %u Maunal = %u",dcvsId, dcvs_manual);

    *(pDcvsList + 0) = dcvs_sign_out;
    *(pDcvsList + 1) = dcvs_coef_bin;
}

void
_dal_longan_sds_rxCali_LEQ_Manual(uint32 unit, uint32 sdsId, rtk_sds_leq_t *pLeq)
{
    if(ENABLED == pLeq->manual)
    {
        SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x18, 15, 15, 0x1);
        SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x16, 14, 10, pLeq->val);
    }
    else
    {
        SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x18, 15, 15, 0x0);
        osal_time_mdelay(100);
    }
}

void
_dal_longan_sds_rxCali_LEQ_Offset_Manual(uint32 unit, uint32 sdsId, uint32 manual, uint32 offset)
{
    if(1 == manual)
    {
        SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x17, 6, 2, offset);
    }
    else
    {
        SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x17, 6, 2, offset);
        osal_time_mdelay(1);
    }
}

void _dal_longan_sds_rxCali_gray_to_bin(uint32 unit, uint32  gray_code, uint32 *pLEQ_bin)
{
    uint32 i,j,m;
    uint32 mask;
    uint32 pG[5];
    uint32  pC[5];
    uint32 bitNum = 5;

    for(i = 0; i < bitNum; i++)
    {
        mask = 1 << i;
        *(pG + i) = (gray_code & mask) >> i;
    }

    m = bitNum - 1;

    *(pC + m) = *(pG + m);

    for(i = 0; i < m; i++)
    {
        *(pC + i) = *(pG + i);
        for(j  = i + 1; j < bitNum; j++)
        {
             *(pC + i) = (*(pC + i)) ^ (*(pG + j));
        }
    }

    *pLEQ_bin = 0;
    for(i = 0; i < bitNum; i++)
    {
        *pLEQ_bin = *pLEQ_bin + (*(pC + i) << i);
    }
}

void _dal_longan_sds_rxCali_LEQ_Read(uint32 unit, uint32 sdsId, rtk_sds_leq_t *pLEQ_bin)
{
    uint32 LEQ_gray;

    if(0 == (sdsId %2))
        hal_serdes_reg_set(unit, sdsId, 0x1f, 0x2, 0x2f);
    else
        hal_serdes_reg_set(unit, sdsId - 1, 0x1f, 0x2, 0x31);

     //##Page0x2E, Reg0x15[9], REG0_RX_EN_TEST=[1]
    SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x15, 9, 9, 0x1);
     //##Page0x21, Reg0x06[11 6], REG0_RX_DEBUG_SEL=[0 1 x x x x]
    SDS_FIELD_WRITE(unit, sdsId, 0x21, 0x06, 11, 6, 0x10);
    osal_time_mdelay(1);
     //##LEQ Read Out
    SDS_FIELD_READ(unit, sdsId, 0x1f, 0x14, 7, 3, &LEQ_gray);
    SDS_FIELD_READ(unit, sdsId, 0x2e, 0x18, 15, 15, &pLEQ_bin->manual);
    _dal_longan_sds_rxCali_gray_to_bin(unit, LEQ_gray, &pLEQ_bin->val);

    PATCH_PRINT("LEQ_gray:%u,LEQ_bin:%u",LEQ_gray, pLEQ_bin->val);
    PATCH_PRINT("LEQ Maunal:%u",pLEQ_bin->manual);
}

void
_dal_longan_sds_rxCali_Vth_Manual(uint32 unit, uint32 sdsId, uint32 manual, uint32 *pVthList)
{
    if(manual == 1)
    {
        SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x0f, 13, 13, 0x1);
        SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x13, 5, 3, *(pVthList + 0));
        SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x13, 2, 0, *(pVthList + 1));
    }
    else
    {
        SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x0f, 13, 13, 0x0);
        osal_time_mdelay(10);
    }
}

void
_dal_longan_sds_rxCali_Vth_Read(uint32 unit, uint32 sdsId, uint32 *pVthList)
{
    uint32 vthp_set_bin;
    uint32 vthn_set_bin;
    uint32 vth_manual;

     //##Page0x1F, Reg0x02[15 0], REG_DBGO_SEL=[0x002F]; //Lane0
     //##Page0x1F, Reg0x02[15 0], REG_DBGO_SEL=[0x0031]; //Lane1
    if(0 == (sdsId %2))
        hal_serdes_reg_set(unit, sdsId, 0x1f, 0x2, 0x2f);
    else
        hal_serdes_reg_set(unit, sdsId - 1, 0x1f, 0x2, 0x31);

     //##Page0x2E, Reg0x15[9], REG0_RX_EN_TEST=[1]
    SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x15, 9, 9, 0x1);
    //##Page0x21, Reg0x06[11 6], REG0_RX_DEBUG_SEL=[1 0 x x x x]
    SDS_FIELD_WRITE(unit, sdsId, 0x21, 0x06, 11, 6, 0x20);
    //##Page0x2F, Reg0x0C[5 0], REG0_COEF_SEL=[0 0 1 1 0 0]
    SDS_FIELD_WRITE(unit, sdsId, 0x2f, 0x0c, 5, 0, 0xc);

    osal_time_mdelay(1);
    //##VthP & VthN Read Out
    SDS_FIELD_READ(unit, sdsId, 0x1f, 0x14, 2, 0, &vthp_set_bin);
    SDS_FIELD_READ(unit, sdsId, 0x1f, 0x14, 5, 3, &vthn_set_bin);

    *(pVthList + 0) = vthp_set_bin;
    *(pVthList + 1) = vthn_set_bin;

    PATCH_PRINT("vth_set_bin = %u",vthp_set_bin);
    PATCH_PRINT("vth_set_bin = %u",vthn_set_bin);

    SDS_FIELD_READ(unit, sdsId, 0x2e, 0x0f, 13, 13, &vth_manual);
    PATCH_PRINT("Vth Maunal = %u",vth_manual);
}

void
_dal_longan_sds_rxCali_TAP_Manual(uint32 unit, uint32 sdsId, uint32 tapId, uint32 manual, uint32 *pTapList)
{
    if(manual == 1)
    {
        switch(tapId)
        {
            case SDS_TAP0:
                //##REG0_LOAD_IN_INIT[0]=1; REG0_TAP0_INIT[5:0]=Tap0_Value
                SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x0f, (tapId + 7), (tapId + 7), 0x1);
                SDS_FIELD_WRITE(unit, sdsId, 0x2f, 0x03, 5, 5, *(pTapList + 0));
                SDS_FIELD_WRITE(unit, sdsId, 0x2f, 0x03, 4, 0, *(pTapList + 1));
                break;
            case SDS_TAP1:
                SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x0f, (tapId + 7), (tapId + 7), 0x1);
                SDS_FIELD_WRITE(unit, sdsId, 0x21, 0x07, 6, 6, *(pTapList + 0));
                SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x09, 11, 6, *(pTapList + 1));
                SDS_FIELD_WRITE(unit, sdsId, 0x21, 0x07, 5, 5, *(pTapList + 2));
                SDS_FIELD_WRITE(unit, sdsId, 0x2f, 0x12, 5, 0, *(pTapList + 3));
                break;
            case SDS_TAP2:
                SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x0f, (tapId + 7), (tapId + 7), 0x1);
                SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x09, 5, 5, *(pTapList + 0));
                SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x09, 4, 0, *(pTapList + 1));
                SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x0a, 11, 11, *(pTapList + 2));
                SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x0a, 10, 6, *(pTapList + 3));
                break;
            case SDS_TAP3:
                SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x0f, (tapId + 7), (tapId + 7), 0x1);
                SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x0a, 5, 5, *(pTapList + 0));
                SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x0a, 4, 0, *(pTapList + 1));
                SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x06, 5, 5, *(pTapList + 2));
                SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x06, 4, 0, *(pTapList + 3));
                break;
            case SDS_TAP4:
                SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x0f, (tapId + 7), (tapId + 7), 0x1);
                SDS_FIELD_WRITE(unit, sdsId, 0x2f, 0x01, 5, 5, *(pTapList + 0));
                SDS_FIELD_WRITE(unit, sdsId, 0x2f, 0x01, 4, 0, *(pTapList + 1));
                SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x06, 11, 11, *(pTapList + 2));
                SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x06, 10, 6, *(pTapList + 3));
                break;
            default:
                break;
        }
    }
    else
    {
        SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x0f, (tapId + 7), (tapId + 7), 0x0);
        osal_time_mdelay(10);
    }
}

void
_dal_longan_sds_rxCali_TAP_Read(uint32 unit, uint32 sdsId, uint32 tapId, uint32 *pTapList)
{
    uint32 tap0_sign_out;
    uint32 tap0_coef_bin;
    uint32 tap_sign_out_even;
    uint32 tap_coef_bin_even;
    uint32 tap_sign_out_odd;
    uint32 tap_coef_bin_odd;
    uint32 tap_manual;

    if(0 == (sdsId %2))
        hal_serdes_reg_set(unit, sdsId, 0x1f, 0x2, 0x2f);
    else
        hal_serdes_reg_set(unit, sdsId - 1, 0x1f, 0x2, 0x31);

     //##Page0x2E, Reg0x15[9], REG0_RX_EN_TEST=[1]
    SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x15, 9, 9, 0x1);
    //##Page0x21, Reg0x06[11 6], REG0_RX_DEBUG_SEL=[1 0 x x x x]
    SDS_FIELD_WRITE(unit, sdsId, 0x21, 0x06, 11, 6, 0x20);

    if(tapId == 0)
    {
        //##Page0x2F, Reg0x0C[5 0], REG0_COEF_SEL=[0 0 0 0 0 1]
        SDS_FIELD_WRITE(unit, sdsId, 0x2f, 0x0c, 5, 0, 0);
        //##Tap1 Even Read Out
        osal_time_mdelay(1);
        SDS_FIELD_READ(unit, sdsId, 0x1f, 0x14, 5, 5, &tap0_sign_out);
        SDS_FIELD_READ(unit, sdsId, 0x1f, 0x14, 4, 0, &tap0_coef_bin);

        if(1 == tap0_sign_out)
            PATCH_PRINT("Tap0 Sign : -");
        else
            PATCH_PRINT("Tap0 Sign : +");

        PATCH_PRINT("tap0_coef_bin = %u", tap0_coef_bin);

        *(pTapList + 0) = tap0_sign_out;
        *(pTapList + 1) = tap0_coef_bin;

        SDS_FIELD_READ(unit, sdsId, 0x2e, 0x0f, 7, 7, &tap_manual);
        PATCH_PRINT("tap0 Maunal = %u",tap_manual);
    }
    else
    {
        //##Page0x2F, Reg0x0C[5 0], REG0_COEF_SEL=[0 0 0 0 0 1]
        SDS_FIELD_WRITE(unit, sdsId, 0x2f, 0x0c, 5, 0, tapId);
        osal_time_mdelay(1);
        //##Tap1 Even Read Out
        SDS_FIELD_READ(unit, sdsId, 0x1f, 0x14, 5, 5, &tap_sign_out_even);
        SDS_FIELD_READ(unit, sdsId, 0x1f, 0x14, 4, 0, &tap_coef_bin_even);

        //##Page0x2F, Reg0x0C[5 0], REG0_COEF_SEL=[0 0 0 1 1 0]
        SDS_FIELD_WRITE(unit, sdsId, 0x2f, 0x0c, 5, 0, (tapId + 5));
        //##Tap1 Odd Read Out
        SDS_FIELD_READ(unit, sdsId, 0x1f, 0x14, 5, 5, &tap_sign_out_odd);
        SDS_FIELD_READ(unit, sdsId, 0x1f, 0x14, 4, 0, &tap_coef_bin_odd);

        if(1 == tap_sign_out_even)
            PATCH_PRINT("Tap %u Even Sign: -", tapId);
        else
            PATCH_PRINT("Tap %u Even Sign: +", tapId);

        PATCH_PRINT("Tap %u Even Coef = %u", tapId,tap_coef_bin_even);

        if(1 == tap_sign_out_odd)
            PATCH_PRINT("Tap %u Odd Sign: -", tapId);
        else
            PATCH_PRINT("Tap %u Odd Sign: +", tapId);

        PATCH_PRINT("Tap %u Odd Coef = %u", tapId,tap_coef_bin_odd);

        *(pTapList + 0) = tap_sign_out_even;
        *(pTapList + 1) = tap_coef_bin_even;
        *(pTapList + 2) = tap_sign_out_odd;
        *(pTapList + 3) = tap_coef_bin_odd;

        SDS_FIELD_READ(unit, sdsId, 0x2e, 0x0f, (7 + tapId), (7 + tapId), &tap_manual);
        PATCH_PRINT("tap %u Maunal = %u",tapId, tap_manual);
    }
}

void
_dal_longan_sds_rxCali_a_start_1d1(uint32 unit, uint32 sdsId, dal_longan_sds_rxCaliInfo_t *pinfo)
{
    rtk_port_10gMedia_t media;

    PATCH_PRINT("start_1.1.1 initial value for sds %u", sdsId);
    hal_serdes_reg_set(unit, sdsId, 6,  0, 0);
    //#offset_SEQ
    SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x1e, 15, 15, 0x1);
    //#FGCAL
    SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x01, 14, 14, 0x0);
    SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x1c, 10, 5, 0x20);
    SDS_FIELD_WRITE(unit, sdsId, 0x2f, 0x02, 0, 0, 0x1);
    //##DCVS
    SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x1e, 14, 11, 0x0);
    SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x01, 15, 15, 0x1);
    SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x02, 11, 11, 0x1);
    SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x1c, 4, 0, 0x0);
    SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x1d, 15, 11, 0x0);
    SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x1d, 10, 6, 0x0);
    SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x1d, 5, 1, 0x0);
    SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x02, 10, 6, 0x0);
    SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x11, 4, 0, 0x0);
    SDS_FIELD_WRITE(unit, sdsId, 0x2f, 0x00, 3, 0, 0xf);
    SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x04, 6, 6, 0x1);
    SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x04, 7, 7, 0x1);
    //##LEQ
    SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x16, 14, 8, 0x20);
    SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x03, 13, 8, 0x0a);
    SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x00, 13, 13, 0x00);
    //##DFE
    SDS_FIELD_WRITE(unit, sdsId, 0x2f, 0x03, 5, 0, pinfo->param.tap0_init_val);
    SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x09, 11, 6, 0x0);
    SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x09, 5, 0, 0x0);
    SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x0a, 5, 0, 0x0);
    SDS_FIELD_WRITE(unit, sdsId, 0x2f, 0x01, 5, 0, 0x0);
    SDS_FIELD_WRITE(unit, sdsId, 0x2f, 0x12, 5, 0, 0x0);
    SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x0a, 11, 6, 0x0);
    SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x06, 5, 0, 0x0);
    SDS_FIELD_WRITE(unit, sdsId, 0x2f, 0x01, 5, 0, 0x0);
    //##Vth
    SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x13, 5, 3, 0x7);
    SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x13, 2, 0, 0x7);
    SDS_FIELD_WRITE(unit, sdsId, 0x2f, 0x0b, 5, 3, pinfo->param.vth_min);

    PATCH_PRINT("end_1.1.1 --");

    PATCH_PRINT("start_1.1.2 Load DFE init. value");

    SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x0f, 13, 7, 0x7f);

    PATCH_PRINT("end_1.1.2");

    PATCH_PRINT("start_1.1.3 disable LEQ training,enable DFE clock");

    SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x17, 7, 7, 0x0);
    SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x17, 6, 2, 0x0);
    SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x0c, 8, 8, 0x0);
    SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x0b, 4, 4, 0x1);
    SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x12, 14, 14, 0x0);
    SDS_FIELD_WRITE(unit, sdsId, 0x2f, 0x02, 15, 15, 0x0);

    PATCH_PRINT("end_1.1.3 --");

    PATCH_PRINT("start_1.1.4 offset cali setting");

    SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x0f, 15, 14, 0x3);

    PATCH_PRINT("end_1.1.4");

    PATCH_PRINT("start_1.1.5 LEQ and DFE setting");

    if(RTK_PHYTYPE_SERDES == HWP_SDS_ID2PHYMODEL(unit, sdsId))
    {
        phy_9300_10gmedia_get( unit, HWP_SDS_ID2MACID(unit, sdsId), &media);

        if(media == PORT_10GMEDIA_FIBER_10G)
        {
            SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x03, 13, 8, 0x1f);
            SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x00, 13, 13, 0x01);
            SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x16, 14, 8, 0);
        }
    }

    SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x16, 3, 2, 0x2);
    SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x0f, 6, 0, 0x5f);
    SDS_FIELD_WRITE(unit, sdsId, 0x2f, 0x05, 7, 2, 0x1f);

    PATCH_PRINT("end_1.1.5");
}

void
_dal_longan_sds_rxCali_a_start_1d2d1(uint32 unit, uint32 sdsId)
{
    PATCH_PRINT("start_1.2.1ForegroundOffsetCal_Manual");

    //Gray config endis to 1
    SDS_FIELD_WRITE(unit, sdsId, 0x2f, 0x02, 2, 2, 0x1);

    //ForegroundOffsetCal_Manual(auto mode)
    SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x01, 14, 14, 0x0);

    PATCH_PRINT("end_1.2.1");
}

void
_dal_longan_sds_rxCali_a_start_1d2d2(uint32 unit, uint32 sdsId)
{

    PATCH_PRINT("start_1.2.2 SDS Rx reset");
    //Force Rx-Run = 0
    SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x15, 8, 8, 0x0);
    //SDS Rx reset
    dal_longan_sds_rx_rst(unit, sdsId);

    PATCH_PRINT("end_1.2.2");
}

void
_dal_longan_sds_rxCali_a_start_1d2d3(uint32 unit, uint32 sdsId)
{
    uint32 fgcal_list[2];
    uint32 offset_range;

    PATCH_PRINT("start_1.2.3 Foreground Calibration");

    while(1)
    {
        _dal_longan_sds_rxCali_foregroundOffsetCal_Read(unit, sdsId, fgcal_list);
        SDS_FIELD_READ(unit, sdsId, 0x2e, 0x15, 15, 14, &offset_range);

        if(fgcal_list[0] > 60 || fgcal_list[0] < 3)
        {
            if(3 == offset_range)
            {
                PATCH_PRINT("Foreground Calibration Result is marginal!");
                break;
            }
            else
            {
                offset_range++;
                SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x15, 15, 14, offset_range);
                _dal_longan_sds_rxCali_a_start_1d2d2(unit, sdsId);
            }
        }
        else
        {
            break;
        }
    }
    PATCH_PRINT("end_1.2.3");
}


void
_dal_longan_sds_rxCali_a_start_1d2d4(uint32 unit, uint32 sdsId)
{
    uint32 *pDcvsList = NULL;

    PATCH_PRINT("start_1.2.4 Gray config endis to 1");

    //Gray config endis to 1
    SDS_FIELD_WRITE(unit, sdsId, 0x2f, 0x02, 2, 2, 0x1);

    _dal_longan_sds_rxCali_DCVS_Manual(unit, sdsId, SDS_DVCS0 , 0, pDcvsList);
    _dal_longan_sds_rxCali_DCVS_Manual(unit, sdsId, SDS_DVCS1 , 0, pDcvsList);
    _dal_longan_sds_rxCali_DCVS_Manual(unit, sdsId, SDS_DVCS2 , 0, pDcvsList);
    _dal_longan_sds_rxCali_DCVS_Manual(unit, sdsId, SDS_DVCS3 , 0, pDcvsList);

    PATCH_PRINT("end_1.2.4");
}
void
_dal_longan_sds_rxCali_a_start_1d2d5(uint32 unit, uint32 sdsId)
{

}
void
_dal_longan_sds_rxCali_a_start_1d2d6(uint32 unit, uint32 sdsId)
{
    uint32 pDcvsList[2];


    _dal_longan_sds_rxCali_DCVS_Read(unit, sdsId, SDS_DVCS1,  pDcvsList);
    _dal_longan_sds_rxCali_DCVS_Manual(unit, sdsId, SDS_DVCS4 , 1, pDcvsList);

    _dal_longan_sds_rxCali_DCVS_Read(unit, sdsId, SDS_DVCS3,  pDcvsList);
    _dal_longan_sds_rxCali_DCVS_Manual(unit, sdsId, SDS_DVCS5 , 1, pDcvsList);
}

void
_dal_longan_sds_rxCali_a_start_1d2(uint32 unit, uint32 sdsId)
{
    dal_longan_sds_rx_rst(unit, sdsId);
    _dal_longan_sds_rxCali_a_start_1d2d1(unit, sdsId);
    _dal_longan_sds_rxCali_a_start_1d2d2(unit, sdsId);
    _dal_longan_sds_rxCali_a_start_1d2d3(unit, sdsId);
    _dal_longan_sds_rxCali_a_start_1d2d6(unit, sdsId);
    _dal_longan_sds_rxCali_a_start_1d2d2(unit, sdsId);
}

void
_dal_longan_sds_rxCali_a_start_1d3d1(uint32 unit, uint32 sdsId)
{
    rtk_sds_leq_t   leq;
    PATCH_PRINT("start_1.3.1");

    //##1.3.1
    if(RTK_PHYTYPE_SERDES != HWP_SDS_ID2PHYMODEL(unit, sdsId))
    {
        SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0xc, 8, 8, 0);
    }
    SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x17, 7, 7, 0x0);

    leq.manual = DISABLED;
    leq.val = 0;
    _dal_longan_sds_rxCali_LEQ_Manual(unit, sdsId, &leq);

    PATCH_PRINT("end_1.3.1");
}

void
_dal_longan_sds_rxCali_a_start_1d3d2(uint32 unit, uint32 sdsId, dal_longan_sds_rxCaliInfo_t *pinfo)
{
    uint32 sum10,int10;
    rtk_sds_leq_t   avg10;
    rtk_sds_leq_t   LEQ_bin;
    rtk_sds_leq_t   temp;
    uint32 i;
    rtk_port_10gMedia_t media;

    PATCH_PRINT("start_1.3.2");

    sum10 = 0;
    for(i = 0; i < 10; i++)
    {
        dal_longan_sds_leq_get(unit,sdsId,&temp);
        sum10 = sum10 + temp.val;
    }
    
    phy_9300_10gmedia_get( unit, HWP_SDS_ID2MACID(unit, sdsId), &media);

    avg10.manual = ENABLED;
    avg10.val = (sum10/10) + (((sum10%10) >=5) ? 1 : 0) + pinfo->param.dacLongCableOffset;
    int10 = sum10/10;

    switch(media)
    {
        case PORT_10GMEDIA_DAC_50CM:
        case PORT_10GMEDIA_DAC_100CM:
            avg10.val = avg10.val + 1;
            break;
        case PORT_10GMEDIA_DAC_300CM:
            avg10.val = avg10.val + 3;
            break;
        case PORT_10GMEDIA_DAC_500CM:
            avg10.val = avg10.val + 5;
            break;
        default:
            break;
    }
    PATCH_PRINT("sum10:%u,avg10:%u,int10:%u",sum10,avg10.val,int10);

    if(RTK_PHYTYPE_SERDES == HWP_SDS_ID2PHYMODEL(unit, sdsId) &&
        !HWP_CASCADE_PORT(unit, HWP_SDS_ID2MACID(unit, sdsId)))
    {
        _dal_longan_sds_rxCali_LEQ_Offset_Manual(unit, sdsId, 1, 0);
        SDS_FIELD_WRITE(unit, sdsId, 0x2e, 0x17, 7, 7, pinfo->param.eqHoldEnable);
        dal_longan_sds_leq_set(unit, sdsId, &avg10);
    }
    else
    {
        /*Nothing to do*/
    }

    dal_longan_sds_leq_get(unit, sdsId, &LEQ_bin);
    PATCH_PRINT("Sds:%u LEQ = %u",sdsId, LEQ_bin.val);

    PATCH_PRINT("end_1.3.2");

}

void
_dal_longan_sds_rxCali_a_start_1d3(uint32 unit, uint32 sdsId, dal_longan_sds_rxCaliInfo_t *pinfo)
{
    _dal_longan_sds_rxCali_a_start_1d3d1(unit, sdsId);
	_dal_longan_sds_rxCali_a_start_1d3d2(unit, sdsId, pinfo);
}

void
_dal_longan_sds_rxCali_a_start_1d4d1(uint32 unit, uint32 sdsId)
{
    uint32 pVthList[2];
    uint32 pTap0List[4];

    PATCH_PRINT("start_1.4.1");

    //##1.4.1
   _dal_longan_sds_rxCali_Vth_Manual(unit, sdsId, 0, pVthList);
   _dal_longan_sds_rxCali_TAP_Manual(unit, sdsId, SDS_TAP0, 0, pTap0List);

   osal_time_mdelay(200);

    PATCH_PRINT("end_1.4.1");
}

void
_dal_longan_sds_rxCali_a_start_1d4d2(uint32 unit, uint32 sdsId, dal_longan_sds_rxCaliInfo_t *pinfo)
{
    uint32 pVthList[2];
    uint32 pTap0List[2];

    PATCH_PRINT("start_1.4.2");

	_dal_longan_sds_rxCali_Vth_Read(unit, sdsId,  pVthList);  
    osal_time_udelay(50);
    _dal_longan_sds_rxCali_Vth_Manual(unit, sdsId, 1, pVthList);

    osal_time_mdelay(100);

    _dal_longan_sds_rxCali_TAP_Read(unit, sdsId, SDS_TAP0, pTap0List);
    osal_time_udelay(50);
    _dal_longan_sds_rxCali_TAP_Manual(unit, sdsId, SDS_TAP0, 1, pTap0List);

    PATCH_PRINT("end_1.4.2");
}

void
_dal_longan_sds_rxCali_a_start_1d4(uint32 unit, uint32 sdsId, dal_longan_sds_rxCaliInfo_t *pinfo)
{
    _dal_longan_sds_rxCali_a_start_1d4d1(unit, sdsId);
    _dal_longan_sds_rxCali_a_start_1d4d2(unit, sdsId, pinfo);
}

void
_dal_longan_sds_rxCali_a_start_1d5d1(uint32 unit, uint32 sdsId)
{
    PATCH_PRINT("start_1.5.1");
    dal_longan_sds_rx_rst(unit, sdsId);

    PATCH_PRINT("end_1.5.1");
}

void
_dal_longan_sds_rxCali_a_start_1d5d2(uint32 unit, uint32 sdsId, dal_longan_sds_rxCaliInfo_t *pinfo)
{
    uint32 pTap1List[4] = {1,4,1,4};
    rtk_port_10gMedia_t media;

    PATCH_PRINT("start_1.5.2");
    //##1.5.2

    if(RTK_PHYTYPE_SERDES == HWP_SDS_ID2PHYMODEL(unit, sdsId))
    {
        phy_9300_10gmedia_get( unit, HWP_SDS_ID2MACID(unit, sdsId), &media);

        if((media >= PORT_10GMEDIA_DAC_50CM) && (media < PORT_10GMEDIA_DAC_END))
        {
            _dal_longan_sds_rxCali_TAP_Manual(unit, sdsId, SDS_TAP1, 1, pTap1List);
        }
    }
    else
    {
        _dal_longan_sds_rxCali_TAP_Manual(unit, sdsId, SDS_TAP1, 1, pTap1List);
    }

    osal_time_mdelay(100);

    PATCH_PRINT("end_1.5.2");
}

void
_dal_longan_sds_rxCali_a_start_1d5(uint32 unit, uint32 sdsId, dal_longan_sds_rxCaliInfo_t *pinfo)
{
    if(ENABLED == pinfo->param.dfeTap1_4Enable)
    {
        _dal_longan_sds_rxCali_a_start_1d5d1(unit, sdsId);
        _dal_longan_sds_rxCali_a_start_1d5d2(unit, sdsId, pinfo);
    }
}

void
_dal_longan_sds_rxCali_a_startall(uint32 unit, uint32 sdsId, dal_longan_sds_rxCaliInfo_t *pinfo)
{
    PATCH_PRINT(" Serdes %u Rx Calibration ...",sdsId);

    /* <<< startall >>> */
    _dal_longan_sds_rxCali_a_start_1d1(unit, sdsId, pinfo);

    _dal_longan_sds_rxCali_a_start_1d2(unit, sdsId);
    _dal_longan_sds_rxCali_a_start_1d3(unit, sdsId, pinfo);

    _dal_longan_sds_rxCali_a_start_1d5(unit, sdsId, pinfo);
}

/* Function Name:
 *      dal_longan_sds_rxCaliEnable_get
 * Description:
 *      Get the SerDes rx calibration status.
 * Input:
 *      unit      - unit id
 *      sds       - SerDes id
 * Output:
 *      pStatus      - pointer to  status of rx calibration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_longan_sds_rxCaliStatus_clear(uint32 unit, uint32 sdsId)
{
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SDS), "unit=%d,sds:%d", unit,sdsId);

    /* check Init status */
    RT_INIT_CHK(sds_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_SDS_EXIST(unit, sdsId), RT_ERR_OUT_OF_RANGE);

    /* function body */
    SDS_SEM_LOCK(unit);
    sdsInfo[unit][sdsId].rxCaliStatus = PHY_SDS_RXCALI_STATUS_NOINFO;
    SDS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_sds_rxCaliEnable_get
 * Description:
 *      Get the SerDes rx calibration status.
 * Input:
 *      unit      - unit id
 *      sds       - SerDes id
 * Output:
 *      pStatus      - pointer to  status of rx calibration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
 int32
dal_longan_sds_rxCaliStatus_get(uint32 unit, uint32 sdsId, rtk_port_phySdsRxCaliStatus_t *pStatus)
{
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SDS), "unit=%d,sds:%d", unit,sdsId);

    /* check Init status */
    RT_INIT_CHK(sds_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_SDS_EXIST(unit, sdsId), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(NULL == pStatus, RT_ERR_NULL_POINTER);

    /* function body */
    SDS_SEM_LOCK(unit);
    *pStatus = sdsInfo[unit][sdsId].rxCaliStatus;
    SDS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SDS), "pStatus=%d", *pStatus);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_sds_rxCaliEnable_get
 * Description:
 *      Get the SerDes rx cali enable status.
 * Input:
 *      unit      - unit id
 *      sds       - SerDes id
 * Output:
 *      pEnable      - pointer to the sds enable status of rx calibration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_longan_sds_rxCaliEnable_get(uint32 unit, uint32 sdsId, rtk_enable_t *pEnable)
{
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SDS), "unit=%d,sds:%d", unit,sdsId);

    /* check Init status */
    RT_INIT_CHK(sds_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_SDS_EXIST(unit, sdsId), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(NULL == pEnable, RT_ERR_NULL_POINTER);

    /* function body */
    SDS_SEM_LOCK(unit);
    *pEnable = sdsInfo[unit][sdsId].rxCaliEn;
    SDS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SDS), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_sds_rxCaliEnable_set
 * Description:
 *      Set the SerDes rx cali enable status.
 * Input:
 *      unit      - unit id
 *      sds       - SerDes id
 *      enable  - enable status of rx calibration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 * Note:
 *      None
 */
int32
dal_longan_sds_rxCaliEnable_set(uint32 unit, uint32 sdsId, rtk_enable_t enable)
{
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SDS), "unit=%d,sds:%d,conf=%d", unit, sdsId,enable);

    /* check Init status */
    RT_INIT_CHK(sds_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_SDS_EXIST(unit, sdsId), RT_ERR_OUT_OF_RANGE);

    /* function body */
    SDS_SEM_LOCK(unit);
    sdsInfo[unit][sdsId].rxCaliEn = enable;
    SDS_SEM_UNLOCK(unit);

    if(DISABLED == enable)
    {
        dal_longan_sds_rxCaliStatus_clear(unit, sdsId);
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_rxCaliProcessSts_get
 * Description:
 *      Get the SerDes rx calibration process status.
 * Input:
 *      unit      - unit id
 *      sds       - SerDes id
 * Output:
 *      pStatus      - pointer to  process status of rx calibration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_longan_rxCaliProcessSts_get(uint32 unit, uint32 sdsId,dal_longan_sds_rxCaliProcessSts_t *pStatus)
{
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SDS), "unit=%d,sds:%d", unit,sdsId);

    /* check Init status */
    RT_INIT_CHK(sds_init[unit]);

    /* function body */
    SDS_SEM_LOCK(unit);
    *pStatus = sdsInfo[unit][sdsId].rxCali_process_sts;
    SDS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_rxCaliProcessSts_set
 * Description:
 *      Set the SerDes rx calibration process status.
 * Input:
 *      unit      - unit id
 *      sds      - SerDes id
 *      sts       - rx calibration process status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 * Note:
 *      None
 */
int32
dal_longan_rxCaliProcessSts_set(uint32 unit, uint32 sdsId, dal_longan_sds_rxCaliProcessSts_t status)
{
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SDS), "unit=%d,sds:%d,sts=%d", unit, sdsId, status);

    /* check Init status */
    RT_INIT_CHK(sds_init[unit]);

    /* function body */
    SDS_SEM_LOCK(unit);
    sdsInfo[unit][sdsId].rxCali_process_sts = status;
    SDS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_sds_rxCaliConf_get
 * Description:
 *      Get the SerDes rx calibration configration.
 * Input:
 *      unit      - unit id
 *      sds       - SerDes id
 * Output:
 *      pConf      - pointer to the sds rx calibration configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_longan_sds_rxCaliConf_get(uint32 unit, uint32 sdsId, rtk_sds_rxCaliConf_t  *pConf)
{
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SDS), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(sds_init[unit]);

    RT_PARAM_CHK((NULL == pConf), RT_ERR_NULL_POINTER);

    /* function body */
    SDS_SEM_LOCK(unit);
    osal_memcpy(pConf, &rxCaliConf[unit][sdsId], sizeof(rtk_sds_rxCaliConf_t));
    SDS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SDS), "pConf->cableOffset =%d", pConf->dacLongCableOffset);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_sds_rxCaliConf_set
 * Description:
 *      Config the SerDes rx calibration.
 * Input:
 *      unit      - unit id
 *      sds       - SerDes id
 *      conf      - rx calibration conf
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 * Note:
 *      None
 */
int32
dal_longan_sds_rxCaliConf_set(uint32 unit, uint32 sdsId, rtk_sds_rxCaliConf_t  conf)
{
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SDS), "unit=%d,conf.cableOffset=%d", unit, conf.dacLongCableOffset);

    /* check Init status */
    RT_INIT_CHK(sds_init[unit]);

    /* function body */
    SDS_SEM_LOCK(unit);
    osal_memcpy(&rxCaliConf[unit][sdsId], &conf, sizeof(rtk_sds_rxCaliConf_t));
    SDS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

int32
_dal_longan_sds_rxReCali_check(uint32 unit, uint32 sdsId, uint32 *pRxRecali)
{
    rtk_sds_symErr_t symErr0,symErr1;

    if(RTK_PHYTYPE_NONE != HWP_SDS_ID2PHYMODEL(unit, sdsId))
    {
        dal_longan_sds_symErr_clear(unit, sdsId);
        dal_longan_sds_symErr_clear(unit, sdsId);

        dal_longan_sds_symErr_get(unit, sdsId,  &symErr0);
        osal_time_mdelay(1);
        dal_longan_sds_symErr_get(unit, sdsId,  &symErr1);

        if (RTK_MII_XSGMII == HWP_SDS_MODE(unit, sdsId))
        {
            if((symErr1.all[0] - symErr0.all[0] > 100) || (symErr1.all[1] - symErr0.all[1] > 100) ||
                (symErr1.all[0] >= 0xffff00) || (symErr1.all[1] >= 0xffff00))
            {
                *pRxRecali= 1;
                PATCH_PRINT("xsgmii symbol error occur,flag recal is set");
            }
        }
        else if((RTK_MII_10GR == HWP_SDS_MODE(unit, sdsId)) ||
                    (RTK_MII_USXGMII_10GQXGMII == HWP_SDS_MODE(unit, sdsId)) ||
                    (RTK_MII_USXGMII_10GSXGMII == HWP_SDS_MODE(unit, sdsId)))
        {
            if(symErr1.blk_err > 0)
            {
                *pRxRecali= 1;
                PATCH_PRINT("10gr symbol blk error (%u) > 0,flag recal is set",symErr1.blk_err);
            }
        }
    }

    return RT_ERR_OK;
}

int32
dal_longan_sds_rxCali_start(uint32 unit, uint32 sdsId)
{
    int32       ret = RT_ERR_OK;
    dal_longan_sds_rxCaliInfo_t rxCaliInfo;

    if(!IF_CHIP_TYPE_1(unit))
    {
        osal_memset(&rxCaliInfo, 0, sizeof(dal_longan_sds_rxCaliInfo_t));

        SDS_SEM_LOCK(unit);
        osal_memcpy(&rxCaliInfo.param, &rxCaliConf[unit][sdsId], sizeof(rtk_sds_rxCaliConf_t));
        SDS_SEM_UNLOCK(unit);

        RXCALI_SEM_LOCK(unit, sdsId);
        _dal_longan_sds_rxCali_a_startall(unit, sdsId,  &rxCaliInfo);
        RXCALI_SEM_UNLOCK(unit, sdsId);

        _dal_longan_sds_rxReCali_check(unit, sdsId, &rxCaliInfo.status.flag_recal);
        RT_LOG(LOG_INFO, (MOD_DAL|MOD_SDS), "unit=%u,sds=%u flag_recal=%d", unit, sdsId, rxCaliInfo.status.flag_recal);
        if (rxCaliInfo.status.flag_recal == 0)
        {
            ret = RT_ERR_OK;
        }
        else
        {
            ret = RT_ERR_FAILED;
        }
    }

    return ret;
}

/* Function Name:
 *      dal_longan_sds_rxCali
 * Description:
 *      execute SerDes rx calibration.
 * Input:
 *      unit          - unit id
 *      sds          - SerDes id
 *      retryCnt   - retry count
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
dal_longan_sds_rxCali(uint32 unit, uint32 sdsId, uint32 retryCnt)
{
    int32       ret, procCnt;
    rtk_port_phySdsRxCaliStatus_t   *pRxCaliSts;

    pRxCaliSts = &sdsInfo[unit][sdsId].rxCaliStatus;

    if(sdsInfo[unit][sdsId].rxCaliEn == DISABLED)
        return RT_ERR_OK;

    procCnt = 0;
    do {
        ret = dal_longan_sds_rxCali_start(unit, sdsId);

        if (ret == RT_ERR_OK)
        {
            break;
        }

        osal_time_mdelay(50);
        procCnt++;
    } while (procCnt <= retryCnt);

    if (ret == RT_ERR_OK)
    {
        *pRxCaliSts = PHY_SDS_RXCALI_STATUS_OK;
    }
    else
    {
        *pRxCaliSts = PHY_SDS_RXCALI_STATUS_FAILED;
        if (retryCnt > 0)
        {
            RT_LOG(LOG_INFO, (MOD_DAL|MOD_SDS), "unit=%u,sds=%u cali failed.", unit, sdsId);
        }
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_sds_rxCaliRegDefault_init
 * Description:
 *      Initialize rx calibration registers to default value
 * Input:
 *      unit - unit ID
 *      sdsId - PHY SerDes ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
dal_longan_sds_rxCaliRegDefault_init(uint32 unit, uint32 sds)
{
    dal_longan_sds_rxCaliInfo_t rxCaliInfo;

    RT_LOG(LOG_INFO, (MOD_DAL|MOD_SDS), "unit=%d,sds:%d,reg default", unit, sds);

    /* check Init status */
    RT_INIT_CHK(sds_init[unit]);

    osal_memset(&rxCaliInfo, 0, sizeof(dal_longan_sds_rxCaliInfo_t));

    SDS_SEM_LOCK(unit);

    if(RTK_PHYTYPE_SERDES == HWP_SDS_ID2PHYMODEL(unit, sds))
        osal_memcpy(&rxCaliInfo.param, &rtl9300_rxCaliConf_serdes_myParam, sizeof(rtk_sds_rxCaliConf_t));
    else
        osal_memcpy(&rxCaliInfo.param, &rtl9300_rxCaliConf_phy_myParam, sizeof(rtk_sds_rxCaliConf_t));

    SDS_SEM_UNLOCK(unit);

    RXCALI_SEM_LOCK(unit, sds);
    _dal_longan_sds_rxCali_a_start_1d1(unit, sds, &rxCaliInfo);
    RXCALI_SEM_UNLOCK(unit, sds);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_sds_rxCali_debugEnable_set
 * Description:
 *      Enable debug msg for SerDes rx calibration
 * Input:
 *      enable      - enable print debug msg
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
dal_longan_sds_rxCali_debugEnable_set(rtk_enable_t enable)
{
    if(enable)
        patchDbgEnable = 1;
    else
        patchDbgEnable = 0;
    return RT_ERR_OK;
}


/*scan_en 1bit, ck_sel 1bit, rsel 2bit, bais_adj 2bit, pi_adj 2bit*/
int32 _dal_longan_sds_eye_scan_enable(uint32 unit, uint32 sds, uint32 scan_en, uint32 ck_sel, uint32 rsel, uint32 bias_adj, uint32 pi_adj)
{
    int32 ret;

    /*EYE_SCAN_EN Setup*/
    RT_ERR_CHK(SDS_FIELD_W(unit, sds, EYE_SCAN_EN_PAGE, EYE_SCAN_EN_REG,
                    EYE_SCAN_EN_BIT, EYE_SCAN_EN_BIT, scan_en), ret);


    RT_ERR_CHK(SDS_FIELD_W(unit, sds, EYE_SCAN_EN_PAGE, EYE_SCAN_EN_REG,
                    EYE_SCAN_EN_BIT, EYE_SCAN_EN_BIT, 0), ret);

    RT_ERR_CHK(SDS_FIELD_W(unit, sds, EYE_SCAN_EN_PAGE, EYE_SCAN_EN_REG,
                    EYE_SCAN_EN_BIT, EYE_SCAN_EN_BIT, scan_en), ret);

    /*EYE_CK_SEL Setup*/
    RT_ERR_CHK(SDS_FIELD_W(unit, sds, EYE_CK_SEL_PAGE, EYE_CK_SEL_REG,
                    EYE_CK_SEL_BIT, EYE_CK_SEL_BIT, ck_sel), ret);

    /*REG_EYE_RSEL[1:0] Setup*/
    RT_ERR_CHK(SDS_FIELD_W(unit, sds, EYE_RSEL_PAGE, EYE_RSEL_REG,
                    EYE_RSEL_BITHigh, EYE_RSEL_BITLow, rsel), ret);

    /*REG_EYE_BIAS_ADJ[1:0] Setup*/
    RT_ERR_CHK(SDS_FIELD_W(unit, sds, EYE_BIAS_ADJ_PAGE, EYE_BIAS_ADJ_REG,
                    EYE_BIAS_ADJ_BITHigh, EYE_BIAS_ADJ_BITLow, bias_adj), ret);

    /*REG_PI_EYE_RSEL[1:0] Setup*/
    RT_ERR_CHK(SDS_FIELD_W(unit, sds, EYE_PI_ADJ_PAGE, EYE_PI_ADJ_REG,
                    EYE_PI_ADJ_BITHigh, EYE_PI_ADJ_BITLow, pi_adj), ret);

    return RT_ERR_OK;
}


int32 _dal_longan_sds_eye_pi_set(uint32 unit, uint32 sds, uint32 xAxis)
{
    int32   ret;
    uint32 piEn, piSt;
    uint32 piEnList[] = {0x9, 0xC, 0x6, 0x3};
    uint32 piStList[2][16] = {
        {0xFFFF, 0x7FFF, 0x3FFF, 0x1FFF,
         0x0FFF, 0x07FF, 0x03FF, 0x01FF,
         0x00FF, 0x007F, 0x003F, 0x001F,
         0x000F, 0x0007, 0x0003, 0x0001},
        {0x0000, 0x8000, 0xC000, 0xE000,
         0xF000, 0xF800, 0xFC00, 0xFE00,
         0xFF00, 0xFF80, 0xFFC0, 0xFFE0,
         0xFFF0, 0xFFF8, 0xFFFC, 0xFFFE}};

    piEn = piEnList[xAxis/16];
    piSt = piStList[(xAxis/16)%2][xAxis%16];

    RT_ERR_CHK(SDS_FIELD_W(unit, sds, EYE_PI_EN_PAGE, EYE_PI_EN_REG,
                EYE_PI_EN_BITHigh, EYE_PI_EN_BITLow, piEn), ret);


    RT_ERR_CHK(SDS_FIELD_W(unit, sds, EYE_PI_ST_PAGE, EYE_PI_ST_REG,
                EYE_PI_ST_BITHigh, EYE_PI_ST_BITLow, piSt), ret);

    return RT_ERR_OK;
}

int32 _dal_longan_sds_eye_ref_set(uint32 unit, uint32 sds, uint32 yAxis)
{
    int32 ret;

    RT_ERR_CHK(SDS_FIELD_W(unit, sds, EYE_REF_CTRL_PAGE, EYE_REF_CTRL_REG,
                EYE_REF_CTRL_BITHigh, EYE_REF_CTRL_BITLow, yAxis), ret);

    return RT_ERR_OK;
}

int32 _dal_longan_sds_eye_dbgOut(uint32 unit, uint32 sds)
{
    int32 ret;

    if (0 == sds % 2)
    {
        RT_ERR_CHK(hal_serdes_reg_set(unit, sds, 0x1f, 0x02, 0x0037), ret);
    }
    else
    {
        RT_ERR_CHK(hal_serdes_reg_set(unit, sds - 1, 0x1f, 0x02, 0x0039), ret);
    }

    return RT_ERR_OK;
}

int32 _dal_longan_sds_eye_mdio_clk(uint32 unit, uint32 sds)
{
    int32 ret = RT_ERR_FAILED;

    RT_ERR_CHK(SDS_FIELD_W(unit, sds, EYE_MDIO_CTRL_PAGE, EYE_MDIO_CTRL_REG,
                    EYE_MDIO_CTRL_BITHigh, EYE_MDIO_CTRL_BITLow, 0x1), ret);

    RT_ERR_CHK(SDS_FIELD_W(unit, sds, EYE_MDIO_CTRL_PAGE, EYE_MDIO_CTRL_REG,
                        EYE_MDIO_CTRL_BITHigh, EYE_MDIO_CTRL_BITLow, 0x0), ret);

    return RT_ERR_OK;
}

uint32 _dal_longan_sds_eye_pointRead(uint32 unit, uint32 sds, uint32 *val)
{
    int ret;
    uint32 ReadOutPage = 0x1F, ReadOutReg = 0x14;

    if ((ret = _dal_longan_sds_eye_mdio_clk(unit, sds)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_HAL|MOD_SDS), "unit=%u,sds=%u, EYE point read error!", unit, sds);
        return ret;
    }

    if((ret = hal_serdes_reg_get(unit, sds, ReadOutPage, ReadOutReg, val)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_HAL|MOD_SDS), "unit=%u,sds=%u, get serdes reg val error!", unit, sds);
        return ret;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_longan_sds_eyeMonitor_start
 * Description:
 *      port eye monitor start.
 * Input:
 *      port   - port id
 *      frameNum - frame number
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
int32
dal_longan_sds_eyeMonitor_start(uint32 unit, uint32 sds, uint32 frameNum)
{
    int32 ret;
    uint32  xAxis, yAxis, frame;
    uint32  val, data_size;
    uint32  *eyeData = NULL;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SDS), "unit=%d,sds=%d,frameNum=%d", unit, sds, frameNum);

    /* check Init status */
    RT_INIT_CHK(sds_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_SDS_EXIST(unit, sds), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(frameNum > RTK_EYE_MON_FRAME_MAX, RT_ERR_OUT_OF_RANGE);

    SDS_SEM_LOCK(unit);

    data_size = sizeof(uint32) * RTK_EYE_MON_FRAME_MAX * RTK_EYE_MON_X_MAX * RTK_EYE_MON_Y_MAX;
    if ((eyeData = osal_alloc(data_size)) == NULL)
    {
        return RT_ERR_FAILED;
    }
    osal_memset(eyeData, 0, data_size);

    /* enable */
    RT_ERR_HDL(_dal_longan_sds_eye_scan_enable(unit, sds, 0x1, 0x0, 0x1, 0x1, 0x1), Err, ret);

    for (xAxis = 0; xAxis < RTK_EYE_MON_X_MAX; xAxis++)
    {
        RT_ERR_HDL(_dal_longan_sds_eye_pi_set(unit, sds, xAxis), Err, ret);


        for (yAxis = 0; yAxis < RTK_EYE_MON_Y_MAX; yAxis++)
        {
            RT_ERR_HDL(_dal_longan_sds_eye_ref_set(unit, sds, yAxis), Err, ret);

            RT_ERR_HDL(_dal_longan_sds_eye_dbgOut(unit, sds), Err, ret);
            for (frame = 0; frame < frameNum; frame++)
            {
                RT_ERR_HDL(_dal_longan_sds_eye_pointRead(unit, sds, &val), Err, ret);
                RT_ERR_HDL(_dal_longan_sds_eye_pointRead(unit, sds, &val), Err, ret);
                eyeData[RTK_EYE_MON_DATA_POS(frame, xAxis, yAxis)] = val;
            }
        }
    }

    for (xAxis = 0; xAxis < RTK_EYE_MON_X_MAX; xAxis++)
        for (yAxis = 0; yAxis < RTK_EYE_MON_Y_MAX; yAxis++)
            for (frame = 0; frame < frameNum; frame++ )
             {
                    osal_printf("[%d, %d : %d]\n", xAxis, yAxis, eyeData[RTK_EYE_MON_DATA_POS(frame, xAxis, yAxis)]);
             }

Err:
    if (eyeData)
        osal_free(eyeData);

    SDS_SEM_UNLOCK(unit);

    return ret;
}


int32 dal_longan_sds_rxCaliConf_cust(uint32 unit)
{
    sds_conf_t  *p_rtl9300_sds_cust_conf = NULL;
    sds_conf_t  **pp_sds_conf;
    rtk_port_t  port;
    uint32      sds;

    int32 ret = RT_ERR_OK;

    /* Search if there is customer's serdes config for current board  */
    for (pp_sds_conf = conf_rtl9300_cust_board_list; *pp_sds_conf != NULL; pp_sds_conf++)
    {
        if ((*pp_sds_conf)->hwp_id == HWP_IDENTIFIER_ID())
        {
            p_rtl9300_sds_cust_conf = *pp_sds_conf;
            break;
        }
    }

    if (p_rtl9300_sds_cust_conf == NULL)
    {
        return RT_ERR_OK;
    }

    if (p_rtl9300_sds_cust_conf->sds_conf_per_unit[unit] == NULL)
    {
        return RT_ERR_OK;
    }

    HWP_10GE_PORT_TRAVS(unit, port)
    {
        sds = HWP_PORT_SDSID(unit, port);

        if (p_rtl9300_sds_cust_conf->sds_conf_per_unit[unit]->sds_cfg[sds] == NULL)
            continue;

        if (p_rtl9300_sds_cust_conf->sds_conf_per_unit[unit]->sds_cfg[sds]->rxCaliConf != NULL)
        {
            if((ret = dal_longan_sds_rxCaliConf_set(unit,
                sds, *p_rtl9300_sds_cust_conf->sds_conf_per_unit[unit]->sds_cfg[sds]->rxCaliConf)) != RT_ERR_OK)
            {
                return ret;

            }
        }
    }

    return ret;
}

int32 dal_longan_sds_txParam_cust(uint32 unit)
{
    sds_conf_t  *p_rtl9300_sds_cust_conf = NULL;
    sds_conf_t  **pp_sds_conf;
    rtk_port_t  port;
    uint32      sds;

    /* Search if there is customer's serdes config for current board  */
    for (pp_sds_conf = conf_rtl9300_cust_board_list; *pp_sds_conf != NULL; pp_sds_conf++)
    {
        if ((*pp_sds_conf)->hwp_id == HWP_IDENTIFIER_ID())
        {
            p_rtl9300_sds_cust_conf = *pp_sds_conf;
            break;
        }
    }

    if (p_rtl9300_sds_cust_conf == NULL)
    {
        return RT_ERR_OK;
    }

    if (p_rtl9300_sds_cust_conf->sds_conf_per_unit[unit] == NULL)
    {
        return RT_ERR_OK;
    }

    HWP_10GE_PORT_TRAVS(unit, port)
    {
        if(HWP_SERDES_PORT(unit, port))
        {
            sds = HWP_PORT_SDSID(unit, port);

            if (p_rtl9300_sds_cust_conf->sds_conf_per_unit[unit]->sds_cfg[sds] == NULL)
                continue;

            if (p_rtl9300_sds_cust_conf->sds_conf_per_unit[unit]->sds_cfg[sds]->fiber10g_8431_param != NULL)
            {
                if(p_rtl9300_sds_cust_conf->sds_conf_per_unit[unit]->sds_cfg[sds]->fiber10g_8431_param->eyeParam_valid)
                {
                    fiber10g_8431_param[unit][sds].eyeParam_valid = TRUE;

                    osal_memcpy(&fiber10g_8431_param[unit][sds].eyeParam,
                        &p_rtl9300_sds_cust_conf->sds_conf_per_unit[unit]->sds_cfg[sds]->fiber10g_8431_param->eyeParam,
                        sizeof(rtk_sds_eyeParam_t));
                }
                if(p_rtl9300_sds_cust_conf->sds_conf_per_unit[unit]->sds_cfg[sds]->fiber10g_8431_param->tx_param_misc != NULL)
                {
                    fiber10g_8431_param[unit][sds].tx_param_misc =
                        p_rtl9300_sds_cust_conf->sds_conf_per_unit[unit]->sds_cfg[sds]->fiber10g_8431_param->tx_param_misc;
                }
            }

            if (p_rtl9300_sds_cust_conf->sds_conf_per_unit[unit]->sds_cfg[sds]->dac50cm_tx_param != NULL)
            {
                if(p_rtl9300_sds_cust_conf->sds_conf_per_unit[unit]->sds_cfg[sds]->dac50cm_tx_param->eyeParam_valid)
                {
                    dac50cm_tx_param[unit][sds].eyeParam_valid = TRUE;

                    osal_memcpy(&dac50cm_tx_param[unit][sds].eyeParam,
                        &p_rtl9300_sds_cust_conf->sds_conf_per_unit[unit]->sds_cfg[sds]->dac50cm_tx_param->eyeParam,
                        sizeof(rtk_sds_eyeParam_t));
                }
                if(p_rtl9300_sds_cust_conf->sds_conf_per_unit[unit]->sds_cfg[sds]->dac50cm_tx_param->tx_param_misc != NULL)
                {
                    dac50cm_tx_param[unit][sds].tx_param_misc =
                        p_rtl9300_sds_cust_conf->sds_conf_per_unit[unit]->sds_cfg[sds]->dac50cm_tx_param->tx_param_misc;
                }
            }

            if (p_rtl9300_sds_cust_conf->sds_conf_per_unit[unit]->sds_cfg[sds]->dac100cm_tx_param != NULL)
            {
                if(p_rtl9300_sds_cust_conf->sds_conf_per_unit[unit]->sds_cfg[sds]->dac100cm_tx_param->eyeParam_valid)
                {
                    dac100cm_tx_param[unit][sds].eyeParam_valid = TRUE;

                    osal_memcpy(&dac100cm_tx_param[unit][sds].eyeParam,
                        &p_rtl9300_sds_cust_conf->sds_conf_per_unit[unit]->sds_cfg[sds]->dac100cm_tx_param->eyeParam,
                        sizeof(rtk_sds_eyeParam_t));
                }
                if(p_rtl9300_sds_cust_conf->sds_conf_per_unit[unit]->sds_cfg[sds]->dac100cm_tx_param->tx_param_misc != NULL)
                {
                    dac100cm_tx_param[unit][sds].tx_param_misc =
                        p_rtl9300_sds_cust_conf->sds_conf_per_unit[unit]->sds_cfg[sds]->dac100cm_tx_param->tx_param_misc;
                }
            }

            if (p_rtl9300_sds_cust_conf->sds_conf_per_unit[unit]->sds_cfg[sds]->dac300cm_tx_param != NULL)
            {
                if(p_rtl9300_sds_cust_conf->sds_conf_per_unit[unit]->sds_cfg[sds]->dac300cm_tx_param->eyeParam_valid)
                {
                    dac300cm_tx_param[unit][sds].eyeParam_valid = TRUE;

                    osal_memcpy(&dac300cm_tx_param[unit][sds].eyeParam,
                        &p_rtl9300_sds_cust_conf->sds_conf_per_unit[unit]->sds_cfg[sds]->dac300cm_tx_param->eyeParam,
                        sizeof(rtk_sds_eyeParam_t));
                }
                if(p_rtl9300_sds_cust_conf->sds_conf_per_unit[unit]->sds_cfg[sds]->dac300cm_tx_param->tx_param_misc != NULL)
                {
                    dac300cm_tx_param[unit][sds].tx_param_misc =
                        p_rtl9300_sds_cust_conf->sds_conf_per_unit[unit]->sds_cfg[sds]->dac300cm_tx_param->tx_param_misc;
                }
            }
            if (p_rtl9300_sds_cust_conf->sds_conf_per_unit[unit]->sds_cfg[sds]->dac500cm_tx_param != NULL)
            {
                if(p_rtl9300_sds_cust_conf->sds_conf_per_unit[unit]->sds_cfg[sds]->dac500cm_tx_param->eyeParam_valid)
                {
                    dac500cm_tx_param[unit][sds].eyeParam_valid = TRUE;

                    osal_memcpy(&dac500cm_tx_param[unit][sds].eyeParam,
                        &p_rtl9300_sds_cust_conf->sds_conf_per_unit[unit]->sds_cfg[sds]->dac500cm_tx_param->eyeParam,
                        sizeof(rtk_sds_eyeParam_t));
                }
                if(p_rtl9300_sds_cust_conf->sds_conf_per_unit[unit]->sds_cfg[sds]->dac500cm_tx_param->tx_param_misc != NULL)
                {
                    dac500cm_tx_param[unit][sds].tx_param_misc =
                        p_rtl9300_sds_cust_conf->sds_conf_per_unit[unit]->sds_cfg[sds]->dac500cm_tx_param->tx_param_misc;
                }
            }
        }
    }

    return RT_ERR_OK;

}

int32
dal_longan_sds_txParam_config_init(uint32 unit)
{
    sds_conf_per_unit_t *p_chip_txParam = NULL;
    uint32 sds;
    rtk_port_t port;


    HWP_SDS_TRAVS(unit, sds)
    {
        fiber10g_8431_param[unit][sds].eyeParam_valid = FALSE;
        fiber10g_8431_param[unit][sds].tx_param_misc = NULL;

        dac50cm_tx_param[unit][sds].eyeParam_valid = FALSE;
        dac50cm_tx_param[unit][sds].tx_param_misc = NULL;

        dac100cm_tx_param[unit][sds].eyeParam_valid = FALSE;
        dac100cm_tx_param[unit][sds].tx_param_misc = NULL;

        dac300cm_tx_param[unit][sds].eyeParam_valid = FALSE;
        dac300cm_tx_param[unit][sds].tx_param_misc = NULL;

        dac500cm_tx_param[unit][sds].eyeParam_valid = FALSE;
        dac500cm_tx_param[unit][sds].tx_param_misc = NULL;
    }

    if(RTL9301_CHIP_ID == HWP_CHIP_ID(unit))
    {
        if(HWP_CASCADE_MODE() && (HWP_CASCADE_SLAVE_UNIT_ID() != unit))/*cascade master unit*/
        {
            p_chip_txParam = &sds_cfg_rtl9301_24G_cascade_master;
        }
        else
         {
            p_chip_txParam = &sds_cfg_rtl9301_24G;
        }
    }
    else/*include 9303 and 9302 chip mode*/
    {
        p_chip_txParam = &sds_cfg_rtl9303_80G;
    }

    HWP_10GE_PORT_TRAVS(unit, port)
    {
        if(HWP_SERDES_PORT(unit, port))
        {
            sds = HWP_PORT_SDSID(unit, port);

            if(p_chip_txParam->sds_cfg[sds] == NULL)
                continue;

            if(p_chip_txParam->sds_cfg[sds]->fiber10g_8431_param != NULL)
            {
                fiber10g_8431_param[unit][sds] = *(p_chip_txParam->sds_cfg[sds]->fiber10g_8431_param);
            }
            if(p_chip_txParam->sds_cfg[sds]->fiber10g_8431_param->tx_param_misc != NULL)
            {
                dac300cm_tx_param[unit][sds].tx_param_misc = p_chip_txParam->sds_cfg[sds]->fiber10g_8431_param->tx_param_misc;
            }

            if(p_chip_txParam->sds_cfg[sds]->dac50cm_tx_param != NULL)
            {
                dac50cm_tx_param[unit][sds] = *(p_chip_txParam->sds_cfg[sds]->dac50cm_tx_param);
            }
            if(p_chip_txParam->sds_cfg[sds]->dac50cm_tx_param->tx_param_misc != NULL)
            {
                dac300cm_tx_param[unit][sds].tx_param_misc = p_chip_txParam->sds_cfg[sds]->dac50cm_tx_param->tx_param_misc;
            }

            if(p_chip_txParam->sds_cfg[sds]->dac100cm_tx_param != NULL)
            {
                dac100cm_tx_param[unit][sds] = *(p_chip_txParam->sds_cfg[sds]->dac100cm_tx_param);
            }
            if(p_chip_txParam->sds_cfg[sds]->dac100cm_tx_param->tx_param_misc != NULL)
            {
                dac300cm_tx_param[unit][sds].tx_param_misc = p_chip_txParam->sds_cfg[sds]->dac100cm_tx_param->tx_param_misc;
            }

            if(p_chip_txParam->sds_cfg[sds]->dac300cm_tx_param != NULL)
            {
                dac300cm_tx_param[unit][sds] = *(p_chip_txParam->sds_cfg[sds]->dac300cm_tx_param);
            }
            if(p_chip_txParam->sds_cfg[sds]->dac300cm_tx_param->tx_param_misc != NULL)
            {
                dac300cm_tx_param[unit][sds].tx_param_misc = p_chip_txParam->sds_cfg[sds]->dac300cm_tx_param->tx_param_misc;
            }

            if(p_chip_txParam->sds_cfg[sds]->dac500cm_tx_param != NULL)
            {
                dac500cm_tx_param[unit][sds] = *(p_chip_txParam->sds_cfg[sds]->dac500cm_tx_param);
            }
            if(p_chip_txParam->sds_cfg[sds]->dac500cm_tx_param->tx_param_misc != NULL)
            {
                dac300cm_tx_param[unit][sds].tx_param_misc = p_chip_txParam->sds_cfg[sds]->dac500cm_tx_param->tx_param_misc;
            }
        }
    }

    dal_longan_sds_txParam_cust(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_sds_loopback_get
 * Description:
 *      Get SerDes loopback status
 * Input:
 *      unit    - unit id
 *      sds     - user SerDis id
 * Output:
 *      pStatus - SerDes loopback status
 * Return:
 *      RT_ERR_FAILED - initialize fail
 *      RT_ERR_OK     - initialize success
 * Note:
 *      None
 */
int32
dal_longan_sds_loopback_get(uint32 unit, uint32 sds, rtk_enable_t *pStatus)
{
    uint32  port;
    uint32  val;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SDS), "unit=%d,sds=%d", unit, sds);

    /* check Init status */
    RT_INIT_CHK(sds_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((!HWP_SDS_EXIST(unit, sds)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pStatus), RT_ERR_NULL_POINTER);

    port = HWP_SDS_ID2MACID(unit, sds);
    RT_PARAM_CHK((!HWP_PHY_EXIST(unit, port)), RT_ERR_INPUT);

    /* function body */
    SDS_SEM_LOCK(unit);

    /* SP_CFG_DIG_LPK */
    RT_ERR_HDL(SDS_FIELD_R(unit, sds, 0, 0, 4, 4, &val), ERR, ret);

    *pStatus = (1 == val)? ENABLED: DISABLED;

    /* FP_TGR1_CFG_AFE_LPK */
    RT_ERR_HDL(SDS_FIELD_R(unit, sds, 6, 0x1, 2, 2, &val), ERR, ret);

    SDS_SEM_UNLOCK(unit);

    if ( ((val == 0) && (*pStatus != DISABLED))
        ||((val != 0) && (*pStatus != ENABLED)))
    {
        RT_LOG(LOG_WARNING, (MOD_DAL|MOD_SDS), "Get loopback: regs inconsisten!");
    }

    return ret;

ERR:
    SDS_SEM_UNLOCK(unit);
    RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_SDS), "Get loopback fail");

    return ret;
}   /* end of dal_longan_sds_loopback_get */

/* Function Name:
 *      dal_longan_sds_loopback_set
 * Description:
 *      Set SerDes loopback status
 * Input:
 *      unit    - unit id
 *      sds     - user SerDis id
 *      status  - SerDes loopback status
 * Output:
 * Return:
 *      RT_ERR_FAILED - initialize fail
 *      RT_ERR_OK     - initialize success
 * Note:
 *      None
 */
int32
dal_longan_sds_loopback_set(uint32 unit, uint32 sds, rtk_enable_t status)
{
    uint32  port;
    uint32  val;
    rt_serdesMode_t mode;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SDS), "unit=%d,sds=%d,status=%d", unit, sds, status);

    /* check Init status */
    RT_INIT_CHK(sds_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((!HWP_SDS_EXIST(unit, sds)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(status >= RTK_ENABLE_END, RT_ERR_INPUT);

    port = HWP_SDS_ID2MACID(unit, sds);
    RT_PARAM_CHK((!HWP_PHY_EXIST(unit, port)), RT_ERR_INPUT);

    /* function body */
    val = (ENABLED == status)? 1: 0;

    SDS_SEM_LOCK(unit);

    /*Fiber1G*/
    /* SP_CFG_DIG_LPK */
    RT_ERR_HDL(SDS_FIELD_W(unit, sds, 0, 0, 4, 4, val), ERR, ret);

    /*10GR*/
    /* FP_TGR1_CFG_AFE_LPK */
    RT_ERR_HDL(SDS_FIELD_W(unit, sds, 6, 0x1, 2, 2, val), ERR, ret);

    SDS_SEM_UNLOCK(unit);

    dal_longan_sds_mode_get(unit, sds, &mode);
    dal_longan_sds_mode_set(unit, sds, mode);
    osal_time_mdelay(1);

    return ret;

ERR:
    SDS_SEM_UNLOCK(unit);
    RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_SDS), "Set loopback fail");

    return ret;
}   /* end of dal_longan_sds_loopback_set */

/* Function Name:
 *      dal_longan_sds_fiberUnidirEnable_get
 * Description:
 *      Get fiber unidirection enable status of the specific serdes
 * Input:
 *      unit           - unit id
 *      sds            - SerDes id
 * Output:
 *      pEnable        - pointer to the enable status of fiber unidirection
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
dal_longan_sds_fiberUnidirEnable_get(uint32 unit, uint32 sds, rtk_enable_t *pEnable)
{
    int32 ret = RT_ERR_OK;
    uint32 data = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SDS), "unit=%d, sds=%d", unit, sds);

    /* check Init status */
    RT_INIT_CHK(sds_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_SDS_EXIST(unit, sds), RT_ERR_OUT_OF_RANGE);

    /* function body */
    SDS_SEM_LOCK(unit);

    if((sds >= 0) && (sds < 4))
        RT_ERR_HDL(reg_array_field_read(unit, LONGAN_SDS_INTF_CTRL0r, sds, REG_ARRAY_INDEX_NONE, \
                LONGAN_UNIDIR_TX_ABLE_S0f, &data ), ERR, ret);
    else if((sds >= 4) && (sds < 10))
        RT_ERR_HDL(reg_array_field_read(unit, LONGAN_SDS_INTF_CTRL1r, sds, REG_ARRAY_INDEX_NONE, \
                LONGAN_UNIDIR_TX_ABLE_S4f, &data ), ERR, ret);
    else if((sds >= 10) && (sds < 12))
        RT_ERR_HDL(reg_array_field_read(unit, LONGAN_SDS_INTF_CTRL2r, sds, REG_ARRAY_INDEX_NONE, \
                LONGAN_UNIDIR_TX_ABLE_S10f,&data ), ERR, ret);

    if(data == 1)
    {
        *pEnable = ENABLED;
    }
    else
    {
        *pEnable = DISABLED;
    }

ERR:
    SDS_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      dal_longan_sds_fiberUnidirEnable_set
 * Description:
 *      Set fiber unidirection enable status of the specific serdes
 * Input:
 *      unit           - unit id
 *      sds            - SerDes id
 *      enable         - enable status of fiber unidirection
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
dal_longan_sds_fiberUnidirEnable_set(uint32 unit, uint32 sds, rtk_enable_t enable)
{
    int32 ret = RT_ERR_OK;
    uint32 data;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SDS), "unit=%d, sds=%d", unit, sds);

    /* check Init status */
    RT_INIT_CHK(sds_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_SDS_EXIST(unit, sds), RT_ERR_OUT_OF_RANGE);

    /* function body */
    SDS_SEM_LOCK(unit);

    if(ENABLED == enable)
    {
        data = 1;
    }
    else
    {
        data = 0;
    }

    if((sds >= 0) && (sds < 4))
        RT_ERR_HDL(reg_array_field_write(unit, LONGAN_SDS_INTF_CTRL0r, sds, REG_ARRAY_INDEX_NONE, \
                LONGAN_UNIDIR_TX_ABLE_S0f, &data ), ERR, ret);
    else if((sds >= 4) && (sds < 10))
        RT_ERR_HDL(reg_array_field_write(unit, LONGAN_SDS_INTF_CTRL1r, sds, REG_ARRAY_INDEX_NONE, \
                LONGAN_UNIDIR_TX_ABLE_S4f, &data ), ERR, ret);
    else if((sds >= 10) && (sds < 12))
        RT_ERR_HDL(reg_array_field_write(unit, LONGAN_SDS_INTF_CTRL2r, sds, REG_ARRAY_INDEX_NONE, \
                LONGAN_UNIDIR_TX_ABLE_S10f,&data ), ERR, ret);

ERR:
    SDS_SEM_UNLOCK(unit);

    return ret;
}


/* Function Name:
 *      dal_longan_sds_ctrl_get
 * Description:
 *      Get SerDes specific settings
 * Input:
 *      unit      - unit id
 *      sds       - SerDes id
 *      ctrlType  - setting type
 * Output:
 *      pValue    - pointer to setting value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
dal_longan_sds_ctrl_get(uint32 unit, uint32 sds, rtk_sds_ctrl_t ctrlType,
    uint32 *pValue)
{
    int32 ret = RT_ERR_OK;
    uint32 data = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SDS), "unit=%d, sds=%d, ctrlType=%d", unit, sds, ctrlType);

    /* check Init status */
    RT_INIT_CHK(sds_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((!HWP_SDS_EXIST(unit, sds)), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_SDS_CRTL_END <= ctrlType), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pValue), RT_ERR_NULL_POINTER);

    /* function body */
    SDS_SEM_LOCK(unit);

    switch (ctrlType)
    {
        case RTK_SDS_CTRL_USXGMII_AN:
            ret = SDS_FIELD_R(unit, sds, 0x7, 17, 3, 0, &data);
            if(data == 0xf)
                *pValue = 1;
            else if(data == 0x0)
                *pValue = 0;
            else
            {
                *pValue = 0;
                ret = RT_ERR_FAILED;
            }
            break;
        default:
            break;
    }

    SDS_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_longan_sds_ctrl_get */

/* Function Name:
 *      dal_longan_sds_ctrl_set
 * Description:
 *      Set SerDes specific settings
 * Input:
 *      unit      - unit id
 *      sds       - SerDes id
 *      ctrlType  - setting type
 *      value     - setting value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
dal_longan_sds_ctrl_set(uint32 unit, uint32 sds, rtk_sds_ctrl_t ctrlType,
    uint32 value)
{
    int32 ret = RT_ERR_OK;
    uint32 data = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SDS), "unit=%d, sds=%d, ctrlType=%d, value=%d", unit, sds, ctrlType, value);

    /* check Init status */
    RT_INIT_CHK(sds_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((!HWP_SDS_EXIST(unit, sds)), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_SDS_CRTL_END <= ctrlType), RT_ERR_INPUT);

    /* function body */
    SDS_SEM_LOCK(unit);

    switch (ctrlType)
    {
        case RTK_SDS_CTRL_USXGMII_AN:
            if (1 == value)
                data = 0xf;
            else
                data = 0x0;
            ret = SDS_FIELD_W(unit, sds, 0x7, 17, 3, 0, data);
            break;
        default:
            break;
    }

    SDS_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_longan_sds_ctrl_set */

/* Function Name:
 *      dal_longan_sds_init
 * Description:
 *      Initialize switch module of the specified device.
 * Input:
 *      unit          - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      1. Module must be initialized before using all of APIs in this module
 */
int32
dal_longan_sds_init(uint32 unit)
{
    int32   ret = RT_ERR_OK;
    uint32  sdsId;
    rtk_port_t basePort;

    RT_INIT_REENTRY_CHK(sds_init[unit]);
    sds_init[unit] = INIT_NOT_COMPLETED;

    /* create semaphore */
    sds_sem[unit] = osal_sem_mutex_create();
    if (0 == sds_sem[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_SDS), "semaphore create failed");
        return RT_ERR_FAILED;
    }

    HWP_SDS_TRAVS(unit, sdsId)
    {
        sdsInfo[unit][sdsId].rxCali_sem = osal_sem_mutex_create();

        if (0 == sdsInfo[unit][sdsId].rxCali_sem)
        {
            RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_SDS), "semaphore create failed");
            return RT_ERR_FAILED;
        }
        sdsInfo[unit][sdsId].rxCaliEn = DAL_RX_CALI_ENABLE;
    }

    /* set init flag to complete init */
    sds_init[unit] = INIT_COMPLETED;

    if (HWP_UNIT_VALID_LOCAL(unit))
    {
        HWP_SDS_TRAVS(unit, sdsId)
        {
            if(!HWP_SDS_10G(unit, sdsId))
                continue;

            basePort = HWP_SDS_ID2MACID(unit, sdsId);

            if((RTK_PHYTYPE_SERDES == HWP_SDS_ID2PHYMODEL(unit, sdsId)) &&
                (!HWP_CASCADE_PORT(unit, basePort)))
            {
                if((ret = dal_longan_sds_rxCaliConf_set(unit, sdsId, rtl9300_rxCaliConf_serdes_myParam)) != RT_ERR_OK)
                    return ret;
            }

            if((RTK_PHYTYPE_NONE != HWP_SDS_ID2PHYMODEL(unit, sdsId)) &&
                (RTK_PHYTYPE_SERDES != HWP_SDS_ID2PHYMODEL(unit, sdsId)))
            {
                if((ret = dal_longan_sds_rxCaliConf_set(unit, sdsId, rtl9300_rxCaliConf_phy_myParam)) != RT_ERR_OK)
                    return ret;
            }

            if((RTK_PHYTYPE_SERDES == HWP_SDS_ID2PHYMODEL(unit, sdsId) && !HWP_CASCADE_PORT(unit, basePort)) ||
                (RTK_PHYTYPE_NONE == HWP_SDS_ID2PHYMODEL(unit, sdsId)))
                continue;

            if((HWP_SDS_MODE(unit, sdsId) != RTK_MII_USXGMII_10GDXGMII)&&
                (HWP_SDS_MODE(unit, sdsId) != RTK_MII_USXGMII_10GSXGMII)&&
                (HWP_SDS_MODE(unit, sdsId) != RTK_MII_USXGMII_10GQXGMII)&&
                (HWP_SDS_MODE(unit, sdsId) != RTK_MII_QHSGMII) &&
                (HWP_SDS_MODE(unit, sdsId) != RTK_MII_10GR) &&
                (HWP_SDS_MODE(unit, sdsId) != RTK_MII_XSGMII))
                continue;

            if(_dal_longan_sds_10gRxIdleRdy_wait(unit, sdsId, RX_IDLE_TIMEOUT_TIME) == RT_ERR_OK)
            {
                osal_time_mdelay(200);
                dal_longan_sds_rxCali(unit, sdsId, 0);
            }
        }
        /*SS-435*/
        if((ret =dal_longan_sds_clk_routine(unit)) != RT_ERR_OK)
            return ret;
        /*SS-447*/
        if((ret =dal_longan_sds_linkFault_check(unit)) != RT_ERR_OK)
            return ret;

        if((ret =dal_longan_sds_rxCaliConf_cust(unit)) != RT_ERR_OK)
            return ret;

        if((ret =dal_longan_sds_txParam_config_init(unit)) != RT_ERR_OK)
            return ret;
    } /* if (HWP_UNIT_VALID_LOCAL(unit)) */

    return ret;
} /* end of dal_longan_sds_init */

/* Function Name:
 *      dal_longan_sds_macSds_set
 * Description:
 *      Set mac SerDes mode.
 * Input:
 *      unit    - unit id
 *      sds     - user SerDes id
 *      mode    - SerDes mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_OUT_OF_RANGE
 * Note:
 *      Only used for loader.
 */
int32 dal_longan_sds_macSds_set(uint32 unit, uint32 sds, rtk_serdesMode_t mode)
{
    int32   ret = RT_ERR_OK;

    /* parameter check */
    RT_PARAM_CHK(!HWP_SDS_EXIST(unit, sds), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((mode >= RTK_MII_END), RT_ERR_INPUT);

    SDS_SEM_LOCK(unit);
    if ((ret = _dal_longan_sds_10gSdsMode_force(unit, sds, mode)) != RT_ERR_OK)
    {
        SDS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_SDS), "");
        return ret;
    }
    SDS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_sdsMapper_init
 * Description:
 *      Hook SerDes module of the specified device.
 * Input:
 *      pMapper - pointer of mapper
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Must Hook SerDes module before calling any switch APIs.
 */
int32
dal_longan_sdsMapper_init(dal_mapper_t *pMapper)
{
    pMapper->sds_init = dal_longan_sds_init;
    pMapper->sds_leq_get = dal_longan_sds_leq_get;
    pMapper->sds_leq_set = dal_longan_sds_leq_set;
    pMapper->sds_leq_adapt = dal_longan_sds_leq_adapt;
    pMapper->sds_xsgNwayEn_set = dal_longan_sds_xsgNwayEn_set;
    pMapper->sds_cmuBand_get = dal_longan_sds_cmuBand_get;
    pMapper->sds_cmuBand_set = dal_longan_sds_cmuBand_set;
    pMapper->sds_symErr_get = dal_longan_sds_symErr_get;
    pMapper->sds_symErr_clear = dal_longan_sds_symErr_clear;
    pMapper->sds_rx_rst = dal_longan_sds_rx_rst;
    pMapper->sds_linkSts_get = dal_longan_sds_linkSts_get;
    pMapper->sds_testMode_set = dal_longan_sds_testMode_set;
    pMapper->sds_eyeParam_get = dal_longan_sds_eyeParam_get;
    pMapper->sds_eyeParam_set = dal_longan_sds_eyeParam_set;
    pMapper->sds_rxCaliConf_get = dal_longan_sds_rxCaliConf_get;
    pMapper->sds_rxCaliConf_set = dal_longan_sds_rxCaliConf_set;
    pMapper->sds_loopback_get = dal_longan_sds_loopback_get;
    pMapper->sds_loopback_set = dal_longan_sds_loopback_set;

    pMapper->sds_ctrl_get = dal_longan_sds_ctrl_get;
    pMapper->sds_ctrl_set = dal_longan_sds_ctrl_set;

    pMapper->_sds_mode_get = dal_longan_sds_mode_get;
    pMapper->_sds_mode_set = dal_longan_sds_mode_set;

    pMapper->sds_eyeMonitor_start = dal_longan_sds_eyeMonitor_start;

    return RT_ERR_OK;
}   /* end of dal_longan_sdsMapper_init */
