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

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/debug/rt_log.h>
#include <soc/type.h>
#include <hwp/hw_profile.h>
#include <hwp/hwp_util.h>
#include <hal/common/halctrl.h>
#include <hal/mac/miim_common_drv.h>
#include <hal/mac/rtl8295.h>
#include <hal/mac/drv/drv_rtl9310.h>
#include <hal/phy/rtl8295_reg_def.h>
#include <hal/phy/phydef.h>
#include <hal/phy/identify.h>
#include <hal/phy/phy_common.h>
#include <hal/phy/phy_rtl8295.h>
#include <hal/phy/phy_rtl8295_patch.h>
#include <dal/dal_mgmt.h>
#include <osal/memory.h>
#include <osal/time.h>


#if defined(CONFIG_SDK_RTL8390)
  #include <hal/phy/phy_rtl8390.h>
#endif

#define RT_8295_LOG(level, module, fmt, args...)      RT_LOG(level, module, "%s:%d:"fmt, __FUNCTION__, __LINE__, ##args)
#define RT_8295_EYEMON_LOG(fmt, args...)              RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), fmt, ##args)

/*
 * Symbol Definition
 */
#define PHY_8295_10G_PORT_SDSID_START     1
#define PHY_8295_1G_PORT_SDSID_START      4
#define PHY_8295_1G_PORT_SDSID_END        7

#define PHY_8295_10G_SDS_MAX              2

#define PHY_8295_SDS_MODE_SGMII                     0x2     /* SGMII (1.25G) */
#define PHY_8295_SDS_MODE_FIB1G                     0x4     /* 1000BASE-X/FIB1G */
#define PHY_8295_SDS_MODE_FIB100M                   0x5     /* 100BASE-FX/FIB100M */
#define PHY_8295_SDS_MODE_QSGMII                    0x6     /* QSGMII */
#define PHY_8295_SDS_MODE_FIB2_5G                   0x16    /* 2500BASE-X */
#define PHY_8295_SDS_MODE_10GR                      0x1A    /* 10GBASE-R */
#define PHY_8295_SDS_MODE_USXGMII                   0xD     /* USXGMII (10.3125G) */
#define PHY_8295_SDS_MODE_XSGMII                    0x10    /* XSGMII (10.3125G) */
#define PHY_8295_SDS_MODE_OFF                       0x1F


#define RTL8295_MMD_DEV_VENDOR_1                            30
#define PHY_8295_PAGEREG_TO_MMDREG(phyPage, phyReg)         ((phyPage * 8) + (phyReg - 16))


#define PHY_8295_SDS_REG_SEP_RXIDLE_MASK            (0x1 << 1)

#define PHY_8295_PORT_10G                           (0x1 << 1)
#define PHY_8295_PORT_1G                            (0x1 << 2)
#define PHY_8295_PORT_2_5G                          (0x1 << 3)

/* SDS_PAGE=0x2, FIB_REG05 fields definition when serdes mode is SGMII */
#define PHY_8295_SDS_PAGE02_FIB_REG05_SGMII_LINK_OFFSET     15
#define PHY_8295_SDS_PAGE02_FIB_REG05_SGMII_LINK_MASK       (0x1 << PHY_8295_SDS_PAGE02_FIB_REG05_SGMII_LINK_OFFSET)
#define PHY_8295_SDS_PAGE02_FIB_REG05_SGMII_DUPLEX_OFFSET   12
#define PHY_8295_SDS_PAGE02_FIB_REG05_SGMII_DUPLEX_MASK     (0x1 << PHY_8295_SDS_PAGE02_FIB_REG05_SGMII_DUPLEX_OFFSET)
#define PHY_8295_SDS_PAGE02_FIB_REG05_SGMII_SPEED_OFFSET    10
#define PHY_8295_SDS_PAGE02_FIB_REG05_SGMII_SPEED_MASK      (0x3 << PHY_8295_SDS_PAGE02_FIB_REG05_SGMII_SPEED_OFFSET)



typedef struct phy_8295_portInfo_s
{
    uint32          unit;
    rtk_port_t      port;
    uint32          mdxMacId;
}phy_8295_portInfo_t;


/* PTP SerDes register index */
typedef enum {
    RTL8295_PTP_SXP0_REG00 = 0,
    RTL8295_PTP_SXP0_REG01,
    RTL8295_PTP_SXP0_REG02,
    RTL8295_PTP_SXP0_REG03,
    RTL8295_PTP_SXP0_REG04,
    RTL8295_PTP_SXP0_REG05,
    RTL8295_PTP_SXP0_REG06,
    RTL8295_PTP_SXP0_REG07,
    RTL8295_PTP_SXP0_REG08,
    RTL8295_PTP_SXP0_REG09,
    RTL8295_PTP_SXP0_REG10,
    RTL8295_PTP_SXP0_REG11,
    RTL8295_PTP_SXP0_REG12,
    RTL8295_PTP_SXP0_REG13,
    RTL8295_PTP_SXP0_REG_END
}phy_8295_ptpSxP0RegIdx_t;

/* PTP time command */
typedef enum phy_8295_ptpTimeNsecCmd_e
{
    PHY_8295_PTP_TIME_NSEC_CMD_READ = 0x0,
    PHY_8295_PTP_TIME_NSEC_CMD_WRITE,
    PHY_8295_PTP_TIME_NSEC_CMD_ADJ_INC,
    PHY_8295_PTP_TIME_NSEC_CMD_ADJ_DEC,
}phy_8295_ptpTimeNsecCmd_t;


typedef struct phy_8295r_info_s
{
    rtk_portmask_t          portEnState;    /* port enable state. 1:enable, 0:disabled */
    uint32                  sdsModeCfg[RTK_MAX_PORT_PER_UNIT];
    rtk_port_10gMedia_t     port10gMedia[RTK_MAX_PORT_PER_UNIT];
  #if defined(CONFIG_SDK_RTL8295R)
    rt_phyctrl_t            *macSdsCtrl[RTK_MAX_PORT_PER_UNIT];
    rtk_portmask_t          port10gReady;  /* port in 10G speed is ready for TX/RX */
    rtk_portmask_t          s1RxCaliEn;
    rtk_portmask_t          s0RxCaliEn;
    rtk_portmask_t          macRxCalied;   /* indicate MAC RX cali is done. */
    rtk_portmask_t          s0RxCalied;    /* indicate S0 RX cali is done */
    osal_mutex_t                    rxCali_sem[RTK_MAX_PORT_PER_UNIT];
    phy_8295_rxCali_process_sts_t   rxCali_process_sts[RTK_MAX_PORT_PER_UNIT];
    rtk_port_phySdsRxCaliStatus_t   rxCaliStatus[RTK_MAX_NUM_OF_PORTS][PHY_8295_10G_SDS_MAX];
  #endif
}phy_8295r_info_t;


/*
 * Data Declaration
 */
#if defined(CONFIG_SDK_RTL8295R)
rt_phyInfo_t phy_8295R_info =
{
    .phy_num    = PORT_NUM_IN_8295R,
    .eth_type   = HWP_XGE,
    .isComboPhy = {0, 0, 0, 0, 0, 0, 0, 0},
    .flags      = RTK_PHYINFO_FLAG_NONE | RTK_PHYINFO_FLAG_FORCE_RES,

    .xGePhyLocalDuplexAbilityDev            = 0,
    .xGePhyLocalDuplexAbilityAddr           = 0,
    .xGePhyLocalDuplexAbilityBit            = 0,

    .xGePhyLocal1000MSpeedAbilityDev        = 0,
    .xGePhyLocal1000MSpeedAbilityAddr       = 0,
    .xGePhyLocal1000MSpeedAbilityBit        = 0,

    .xGePhyLinkPartner1000MSpeedAbilityDev  = 0,
    .xGePhyLinkPartner1000MSpeedAbilityAddr = 0,
    .xGePhyLinkPartner1000MSpeedAbilityBit  = 0,
};
#endif

#if defined(CONFIG_SDK_RTL8214QF)
rt_phyInfo_t phy_8214QF_info =
{
    .phy_num    = PORT_NUM_IN_8214QF,
    .eth_type   = HWP_GE,
    .isComboPhy = {0, 0, 0, 0, 0, 0, 0, 0},
    .flags      = RTK_PHYINFO_FLAG_NONE,

    .xGePhyLocalDuplexAbilityDev            = 0,
    .xGePhyLocalDuplexAbilityAddr           = 0,
    .xGePhyLocalDuplexAbilityBit            = 0,

    .xGePhyLocal1000MSpeedAbilityDev        = 0,
    .xGePhyLocal1000MSpeedAbilityAddr       = 0,
    .xGePhyLocal1000MSpeedAbilityBit        = 0,

    .xGePhyLinkPartner1000MSpeedAbilityDev  = 0,
    .xGePhyLinkPartner1000MSpeedAbilityAddr = 0,
    .xGePhyLinkPartner1000MSpeedAbilityBit  = 0,
};
#endif


#if defined(CONFIG_SDK_RTL8224QF)
rt_phyInfo_t phy_8224QF_info =
{
    .phy_num    = PORT_NUM_IN_8224QF,
    .eth_type   = HWP_2_5GE,
    .isComboPhy = {0, 0, 0, 0, 0, 0, 0, 0},
    .flags      = RTK_PHYINFO_FLAG_NONE,

    .xGePhyLocalDuplexAbilityDev            = 0,
    .xGePhyLocalDuplexAbilityAddr           = 0,
    .xGePhyLocalDuplexAbilityBit            = 0,

    .xGePhyLocal1000MSpeedAbilityDev        = 0,
    .xGePhyLocal1000MSpeedAbilityAddr       = 0,
    .xGePhyLocal1000MSpeedAbilityBit        = 0,

    .xGePhyLinkPartner1000MSpeedAbilityDev  = 0,
    .xGePhyLinkPartner1000MSpeedAbilityAddr = 0,
    .xGePhyLinkPartner1000MSpeedAbilityBit  = 0,
};
#endif
/* <<< SerDes page offset >>> */
static uint32  sds_reg_page_offset[] =
                    { PHY_8295_PAGE_BASE_OFFSET_S0,     /* 256 */   //0 for Serdes S0
                      PHY_8295_PAGE_BASE_OFFSET_S1,     /* 768 */   //1 for Serdes S1
                      PHY_8295_PAGE_BASE_OFFSET_S0_SLV, /* 512 */   //2 for S0_SLV
                      PHY_8295_PAGE_BASE_OFFSET_S1_SLV, /* 2304 */  //3 for S1_SLV
                      PHY_8295_PAGE_BASE_OFFSET_S4,     /* 1024 */  //4 for Serdes S4
                      PHY_8295_PAGE_BASE_OFFSET_S5,     /* 1280 */  //5 for Serdes S5
                      PHY_8295_PAGE_BASE_OFFSET_S6,     /* 1536 */  //6 for Serdes S6
                      PHY_8295_PAGE_BASE_OFFSET_S7,     /* 1792 */  //7 for Serdes S7
                      PHY_8295_PAGE_BASE_OFFSET_BCAST,  /* 2048 */  //8 for BCAST
                    };
#define PHY_8295_SDS_REG_PAGE_OFFSET_IDX_MAX            (sizeof(sds_reg_page_offset)/sizeof(uint32))


#if defined(CONFIG_SDK_RTL8295R)
  /* SDK configurations */
  static int32              g8295rMacAutoDetectSdsMode = TRUE;      /* when set to FALSE, SDK will be involved to set MAC side when 95R S1 changes between 10G/1G */
  static int32              g8295rLinkPassThroughEn = FALSE;
  static int32              g8295rS0SdsAutoChgEn = TRUE;
  static int32              g8295rStatusSwPoll = TRUE;
#endif
  int32              g8295ProcWa = TRUE;
  int32              g8295ProcLinkChg = TRUE;
  uint32             rtl8295EyeMonDbg = FALSE;
  uint32             rtl8295EyeMonRegChk = FALSE;

phy_8295r_info_t    *g8295rInfo[RTK_MAX_NUM_OF_UNIT] = { NULL };

#define MAC_SDS_CTRL(unit, port)            g8295rInfo[unit]->macSdsCtrl[port]
#define PORT_10G_MEDIA(unit, port)          g8295rInfo[unit]->port10gMedia[port]
#define PORT_SDS_MODE_CFG(unit, port)       g8295rInfo[unit]->sdsModeCfg[port]
#define PORT_ENABLE_STATE(unit, port)       (RTK_PORTMASK_IS_PORT_SET(g8295rInfo[unit]->portEnState, port)?ENABLED:DISABLED)


/*
 * Macro Declaration
 */
#define BITS_MASK(mBit, lBit)           (uint32)((((uint64)0x1 << (mBit - lBit + 1)) - 1) << lBit)


#define PHY_8295_PHYREG_READ(unit, mdxMacId, phyPage, phyReg, pData)                                \
            do {                                                                                    \
                uint32      _rv;                                                                    \
                if ((_rv = phy_8295_phyReg_read(unit, mdxMacId, phyPage, phyReg, pData)) != RT_ERR_OK)\
                {                                                                                   \
                    RT_ERR(_rv, (MOD_HAL), "unit %u macId %u reg %u.%u", unit, mdxMacId, phyPage, phyReg);\
                    return _rv;                                                                     \
                }                                                                                   \
            } while(0)

#define PHY_8295_PHYREG_WRITE(unit, mdxMacId, phyPage, phyReg, data)                                \
            do {                                                                                    \
                uint32      _rv;                                                                    \
                if ((_rv = phy_8295_phyReg_write(unit, mdxMacId, phyPage, phyReg, data)) != RT_ERR_OK)\
                {                                                                                   \
                    RT_ERR(_rv, (MOD_HAL), "unit %u macId %u reg %u.%u data %04x", unit, mdxMacId, phyPage, phyReg, data);\
                    return _rv;                                                                     \
                }                                                                                   \
            } while(0)

#define PHY_8295_PHYREG_MSK_WRITE(unit, mdxMacId, phyPage, phyReg, data, msk)                       \
            do {                                                                                    \
                uint32      _rv, _data;                                                             \
                if ((_rv = phy_8295_phyReg_read(unit, mdxMacId, phyPage, phyReg, &_data)) != RT_ERR_OK)\
                {                                                                                   \
                    RT_ERR(_rv, (MOD_HAL), "unit %u macId %u reg %u.%u", unit, mdxMacId, phyPage, phyReg);\
                    return _rv;                                                                     \
                }                                                                                   \
                _data &= ~ msk;                                                                     \
                _data |= data & msk;                                                                \
                if ((_rv = phy_8295_phyReg_write(unit, mdxMacId, phyPage, phyReg, _data)) != RT_ERR_OK)\
                {                                                                                   \
                    RT_ERR(_rv, (MOD_HAL), "unit %u macId %u reg %u.%u data %04x", unit, mdxMacId, phyPage, phyReg, _data);\
                    return _rv;                                                                     \
                }                                                                                   \
            } while(0)

#define PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, s0PhyPage, phyReg, pData)                       \
            do {                                                                                    \
                uint32      _rv;                                                                    \
                if ((_rv = phy_8295_sdsReg_read(unit, mdxMacId, sdsId, s0PhyPage, phyReg, pData)) != RT_ERR_OK)\
                {                                                                                   \
                    RT_ERR(_rv, (MOD_HAL), "unit %u macId %u sds %u read %u.%u", unit, mdxMacId, sdsId, s0PhyPage, phyReg);\
                    return _rv;                                                                     \
                }                                                                                   \
            } while (0)

#define PHY_8295_SDSREG_WRITE(unit, mdxMacId, sdsId, s0PhyPage, phyReg, data)                       \
            do {                                                                                    \
                uint32      _rv;                                                                    \
                if ((_rv = phy_8295_sdsReg_write(unit, mdxMacId, sdsId, s0PhyPage, phyReg, data)) != RT_ERR_OK)\
                {                                                                                   \
                    RT_ERR(_rv, (MOD_HAL), "unit %u macId %u sds %u write %u.%u", unit, mdxMacId, sdsId, s0PhyPage, phyReg);\
                    return _rv;                                                                     \
                }                                                                                   \
            } while (0)

/* PTP register macro for read/write SxP0_RegN: e.g. PTP_S4P0_REG00, PTP_S5P0_REG01, PTP_TG_REG01, ... */
#define PHY_8295_PTP_SXP0_REGN_READ(unit, mdxMacId, sdsId, regIdx, pData)                           \
            do {                                                                                    \
                uint32      _rv;                                                                    \
                if ((_rv = _phy_8295_PtpSXP0RegN_read(unit, mdxMacId, sdsId, regIdx, pData)) != RT_ERR_OK)\
                {                                                                                   \
                    RT_ERR(_rv, (MOD_HAL), "unit %u macId %u sds %u regIdx %u", unit, mdxMacId, sdsId, regIdx);\
                    return _rv;                                                                     \
                }                                                                                   \
            } while(0)

#define PHY_8295_PTP_SXP0_REGN_WRITE(unit, mdxMacId, sdsId, regIdx, data)                           \
            do {                                                                                    \
                uint32      _rv;                                                                    \
                if ((_rv = _phy_8295_PtpSXP0RegN_write(unit, mdxMacId, sdsId, regIdx, data)) != RT_ERR_OK)\
                {                                                                                   \
                    RT_ERR(_rv, (MOD_HAL), "unit %u macId %u sds %u regIdx %u data %04x", unit, mdxMacId, sdsId, regIdx, data);\
                    return _rv;                                                                     \
                }                                                                                   \
            } while(0)

#define PHY_8295_MDX_MACID_GET(unit, port, pmdxMacId)                                               \
            do {                                                                                    \
                uint32      _rv;                                                                    \
                *pmdxMacId = 0;                                                                     \
                if ((_rv = phy_8295_mdxMacId_get(unit, port, pmdxMacId)) != RT_ERR_OK)              \
                {                                                                                   \
                    RT_ERR(_rv, (MOD_HAL), "unit=%u,port=%u get MDX MACID!", unit, port);           \
                    return _rv;                                                                     \
                }                                                                                   \
            } while (0)

#define PHY_8295_SDSID_GET(unit, port, pSdsId)                                                      \
            do {                                                                                    \
                uint32      _rv;                                                                    \
                *pSdsId = 0;                                                                        \
                if ((_rv = _phy_8295_portToSdsId_get(unit, port, pSdsId)) != RT_ERR_OK)             \
                {                                                                                   \
                    RT_ERR(_rv, (MOD_HAL), "unit=%u,port=%u get sdsId!", unit, port);               \
                    return _rv;                                                                     \
                }                                                                                   \
            } while(0)



/* PTP register macro */
#define PHY_8295_PTP_WAIT_CMD_READY(unit, mdxMacId)                                                 \
            do {                                                                                    \
                uint32      _d, _cnt = 0;                                                           \
                int32       _rv;                                                                    \
                do {                                                                                \
                    /* PTP command register will response within nanoseconds. so, simply poll a few rounds. */\
                    if (_cnt >= 10) {                                                               \
                        _rv = RT_ERR_BUSYWAIT_TIMEOUT;                                              \
                        RT_ERR(_rv, (MOD_HAL), "unit %u mdxMacId %u timeout", unit, mdxMacId);      \
                        return _rv;                                                                 \
                    }                                                                               \
                    PHY_8295_PHYREG_READ(unit, mdxMacId, RTL8295_PTP_CTRL_PTP_TIME_REG01_PAGE, RTL8295_PTP_CTRL_PTP_TIME_REG01_REG, &_d);\
                    _cnt++;                                                                         \
                } while (_d & RTL8295_PTP_CTRL_PTP_TIME_REG01_PTP_TIME_EXEC_MASK);                  \
            } while (0)


#define RXCALI_SEM_LOCK(unit, port)    \
                    do {\
                        if (osal_sem_mutex_take(g8295rInfo[unit]->rxCali_sem[port], OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)\
                        {\
                            RT_ERR(RT_ERR_SEM_LOCK_FAILED, (MOD_HAL), "semaphore lock failed");\
                            return RT_ERR_SEM_LOCK_FAILED;\
                        }\
                    } while(0)

#define RXCALI_SEM_UNLOCK(unit, port)   \
                    do {\
                        if (osal_sem_mutex_give(g8295rInfo[unit]->rxCali_sem[port]) != RT_ERR_OK)\
                        {\
                            RT_ERR(RT_ERR_SEM_UNLOCK_FAILED, (MOD_HAL), "semaphore unlock failed");\
                            return RT_ERR_SEM_UNLOCK_FAILED;\
                        }\
                    } while(0)

/*
 * Function Declaration
 */
int32 _switch_port_autoNegoEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *enable);
void _switch_port_autoNegoEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);
int32 _phy_8295_serdesMode_get(uint32 unit, rtk_port_t port, uint32 *pMode);
int32 _phy_8295_portSdsMode_set(uint32 unit, rtk_port_t port, uint32 mode);
int32 _phy_8295_serdesModeReg_set(uint32 unit, rtk_port_t port, uint32 mdxMacId, uint32 sdsId, uint32 mode);
int32 _phy_8295r_fiberRx_10g_reset(uint32 unit, rtk_port_t port, uint32 mdxMacId, uint32 sdsId);
int32 _phy_8295r_serdes_reset(uint32 unit, rtk_port_t port);
int32 _phy_8214qf_serdes_reset(uint32 unit, rtk_port_t port);
int32 phy_8214qf_fiberRx_reset(uint32 unit, rtk_port_t port);
#if defined(CONFIG_SDK_RTL8295R)
int32 phy_8295r_rxCali_process_sts_get(uint32 unit, uint32 port);
int32 phy_8295r_rxCali_process_sts_set(uint32 unit, uint32 port, phy_8295_rxCali_process_sts_t rxCali_sts);
#endif


/* Function Name:
 *      _phy_8295_regSdsModeToRtSdsMode
 * Description:
 *      Traslate 8295 SerDes mode to rt_serdesMode_t enum.
 * Input:
 *      chipSdsMode    - 8295 SerDes mode register value
 * Output:
 *      pSdsMode  - rt_serdesMode_t enum
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
_phy_8295_regSdsModeToRtSdsMode(uint32 regSdsMode, rt_serdesMode_t *pSdsMode)
{
    switch(regSdsMode)
    {
      case PHY_8295_SDS_MODE_OFF:
        *pSdsMode = RTK_MII_DISABLE;
        break;
      case PHY_8295_SDS_MODE_QSGMII:
        *pSdsMode = RTK_MII_QSGMII;
        break;
      case PHY_8295_SDS_MODE_XSGMII:
        *pSdsMode = RTK_MII_XSGMII;
        break;
      case PHY_8295_SDS_MODE_10GR:
        *pSdsMode = RTK_MII_10GR;
        break;
      case PHY_8295_SDS_MODE_SGMII:
        *pSdsMode = RTK_MII_SGMII;
        break;
      case PHY_8295_SDS_MODE_FIB1G:
        *pSdsMode = RTK_MII_1000BX_FIBER;
        break;
      case PHY_8295_SDS_MODE_FIB2_5G:
        *pSdsMode = RTK_MII_2500Base_X;
        break;
      case PHY_8295_SDS_MODE_USXGMII:
        *pSdsMode = RTK_MII_USXGMII_10GQXGMII;
        break;
      default:
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}



#if defined(CONFIG_SDK_RTL8295R)
int32
_switch_port_10gMedia_convert(uint32 unit, rtk_port_t port, rtk_port_10gMedia_t media, rtk_port_10gMedia_t *pMacMedia)
{
    switch (media)
    {
      case PORT_10GMEDIA_FIBER_10G:
      case PORT_10GMEDIA_DAC_50CM:
      case PORT_10GMEDIA_DAC_100CM:
      case PORT_10GMEDIA_DAC_300CM:
      case PORT_10GMEDIA_DAC_500CM: //DAC_LONG
        *pMacMedia = PORT_10GMEDIA_FIBER_10G;
        return RT_ERR_OK;
      case PORT_10GMEDIA_FIBER_1G:
        *pMacMedia = PORT_10GMEDIA_FIBER_1G;
        return RT_ERR_OK;
      case PORT_10GMEDIA_NONE:
        *pMacMedia = PORT_10GMEDIA_NONE;
        return RT_ERR_OK;
      default:
        *pMacMedia = PORT_10GMEDIA_FIBER_10G;
        return RT_ERR_OK;
    }
}
#endif


#if defined(CONFIG_SDK_RTL8295R)
int32
_switch_port_10gMedia_get(uint32 unit, rtk_port_t port, rtk_port_10gMedia_t *pMedia)
{
    int32       ret;

    if ((MAC_SDS_CTRL(unit, port) == NULL) || (MAC_SDS_CTRL(unit, port)->pPhydrv == NULL))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if ((ret = MAC_SDS_CTRL(unit, port)->pPhydrv->fPhydrv_10gMedia_get(unit, port, pMedia)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HAL), "unit=%u,port=%u", unit, port);
        return ret;
    }

    return RT_ERR_OK;
}
#endif

#if defined(CONFIG_SDK_RTL8295R)
int32
_switch_port_10gMedia_set(uint32 unit, rtk_port_t port, rtk_port_10gMedia_t media)
{
    rtk_port_10gMedia_t       pre_media = PORT_10GMEDIA_END;
    int32       ret;

    if ((MAC_SDS_CTRL(unit, port) == NULL) || (MAC_SDS_CTRL(unit, port)->pPhydrv == NULL))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    MAC_SDS_CTRL(unit, port)->pPhydrv->fPhydrv_10gMedia_get(unit, port, &pre_media);
    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,media:%d -> %d. clear macRxCalied", unit, port, pre_media, media);

    RTK_PORTMASK_PORT_CLEAR(g8295rInfo[unit]->macRxCalied, port);
    if (pre_media != PORT_10GMEDIA_NONE)
    {
        MAC_SDS_CTRL(unit, port)->pPhydrv->fPhydrv_10gMedia_set(unit, port, PORT_10GMEDIA_NONE);
    }

    if ((ret = MAC_SDS_CTRL(unit, port)->pPhydrv->fPhydrv_10gMedia_set(unit, port, media)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HAL), "unit=%u,port=%u", unit, port);
        return ret;
    }

    if (media == PORT_10GMEDIA_FIBER_1G)
    {
        rtk_enable_t    sw_enable;
        if ((_switch_port_autoNegoEnable_get(unit, port, &sw_enable) == RT_ERR_OK) && (sw_enable != DISABLED))
        {
            _switch_port_autoNegoEnable_set(unit, port, DISABLED);
        }
    }

    return RT_ERR_OK;
}
#endif


#if defined(CONFIG_SDK_RTL8295R)
void
_switch_port_enable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32       ret;

    if ((MAC_SDS_CTRL(unit, port) == NULL) || (MAC_SDS_CTRL(unit, port)->pPhydrv == NULL))
    {
        return;
    }

    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,switch port EN:%u", unit, port, enable);
    if ((ret = MAC_SDS_CTRL(unit, port)->pPhydrv->fPhydrv_enable_set(unit, port, enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HAL), "unit=%u,port=%u", unit, port);
        return;
    }
    return;
}
#endif

#if defined(CONFIG_SDK_RTL8295R)
int32
_switch_port_autoNegoEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *enable)
{
    int32       ret;

    if ((MAC_SDS_CTRL(unit, port) == NULL) || (MAC_SDS_CTRL(unit, port)->pPhydrv == NULL))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if ((ret = MAC_SDS_CTRL(unit, port)->pPhydrv->fPhydrv_autoNegoEnable_get(unit, port, enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HAL), "unit=%u,port=%u", unit, port);
        return ret;
    }
    return RT_ERR_OK;
}
#endif

#if defined(CONFIG_SDK_RTL8295R)
void
_switch_port_autoNegoEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32       ret;

    if ((MAC_SDS_CTRL(unit, port) == NULL) || (MAC_SDS_CTRL(unit, port)->pPhydrv == NULL))
    {
        return;
    }

    if ((ret = MAC_SDS_CTRL(unit, port)->pPhydrv->fPhydrv_autoNegoEnable_set(unit, port, enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HAL), "unit=%u,port=%u", unit, port);
        return;
    }
    return;
}
#endif


#if defined(CONFIG_SDK_RTL8295R)
void
_switch_port_macForceLinkDown_enable(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;

    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%d,macForceDownEn=%d", unit, port, enable);

    #if defined(CONFIG_SDK_RTL9310)
    if (HWP_9310_FAMILY_ID(unit))
        drv_rtl9310_portMacForceLink_set(unit, port, enable, PORT_LINKDOWN, DRV_RTL9310_FRCLINK_MODULE_PHY);
    else
    #endif  /* CONFIG_SDK_RTL9310 */
    if ((ret = RT_MAPPER(unit)->_port_macForceLink_set(unit, port, enable, PORT_LINKDOWN,PORT_DEFAULT)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HAL), "unit=%u,port=%u,en=%d", unit, port, enable);
    }
}
#endif


#if defined(CONFIG_SDK_RTL8295R)
/*
 * Convert 95R 10g-media to MAC serdes mode
 */
void
_switch_sdsMode_convert(uint32 unit, rtk_port_t port, rtk_port_10gMedia_t media, rt_serdesMode_t *macSdsMode)
{
    uint32      macSdsId;

    if ((macSdsId = HWP_PORT_SDSID(unit, port)) == HWP_NONE)
    {
        *macSdsMode = RTK_MII_NONE;
        return;
    }

    if (HWP_SDS_MODE(unit, macSdsId) == RTK_MII_USXGMII_10GSXGMII)
    {
        *macSdsMode = RTK_MII_USXGMII_10GSXGMII;
        return;
    }

    //RTK_MII_10GR
    if (media == PORT_10GMEDIA_NONE)
    {
        *macSdsMode = RTK_MII_DISABLE;
    }
    else if (media == PORT_10GMEDIA_FIBER_1G)
    {
        *macSdsMode = RTK_MII_1000BX_FIBER;
    }
    else
    {
        *macSdsMode = RTK_MII_10GR;
    }
    return;
}

int32
_switch_sdsMode_get(uint32 unit, rtk_port_t port, rtk_serdesMode_t *macSdsMode)
{
    int32           ret;
    uint32          macSdsId;

    *macSdsMode = RTK_MII_NONE;
    if (RT_MAPPER(unit) == NULL || (RT_MAPPER(unit)->_sds_mode_get == NULL))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if ((macSdsId = HWP_PORT_SDSID(unit, port)) == HWP_NONE)
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }
    if ((ret = RT_MAPPER(unit)->_sds_mode_get(unit, macSdsId, macSdsMode)) != RT_ERR_OK)
    {
        if (ret != RT_ERR_CHIP_NOT_SUPPORTED)
        {
            RT_8295_LOG(LOG_INFO, (MOD_PORT), "unit=%u,macSds=%u get fail", unit, macSdsId);
        }
        return ret;
    }
    //RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,switch SDS:%u", unit, port, *macSdsMode);
    return RT_ERR_OK;
}

void
_switch_sdsMode_set(uint32 unit, rtk_port_t port, rtk_serdesMode_t macSdsMode)
{
    int32           ret;
    uint32          macSdsId;


    if (RT_MAPPER(unit) == NULL || (RT_MAPPER(unit)->_sds_mode_set == NULL))
    {
        return;
    }

    if ((macSdsId = HWP_PORT_SDSID(unit, port)) == HWP_NONE)
    {
        return;
    }
    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,sds=%u,macSdsMode:%u", unit, port, macSdsId, macSdsMode);
    if ((ret = RT_MAPPER(unit)->_sds_mode_set(unit, macSdsId, macSdsMode)) != RT_ERR_OK)
    {
        RT_8295_LOG(LOG_INFO, (MOD_PORT), "unit=%u,macSds=%u set fail", unit, macSdsId);
        return;
    }
    return;
}
#endif


#if defined(CONFIG_SDK_RTL8295R)
void
_switch_serdes_init(uint32 unit, rtk_port_t port)
{
    int32       ret;

    if (!HWP_10GE_PORT(unit, port))
    {
        return;
    }

    if ((MAC_SDS_CTRL(unit, port) == NULL) || (MAC_SDS_CTRL(unit, port)->pPhydrv == NULL))
    {
        return;
    }

    if (MAC_SDS_CTRL(unit, port)->pPhydrv->fPhydrv_init == NULL)
    {
        return;
    }

    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,drvIdx:%d", unit, port, MAC_SDS_CTRL(unit, port)->pPhydrv->phydrv_index);
    if ((ret = MAC_SDS_CTRL(unit, port)->pPhydrv->fPhydrv_init(unit, port)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HAL), "unit=%u,port=%u", unit, port);
        return;
    }

    if (HWP_UNIT_VALID_LOCAL(unit))
    {
        if (HWP_8390_FAMILY_ID(unit))
        {
            RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,MAC port enable", unit, port);
            if ((ret = MAC_SDS_CTRL(unit, port)->pPhydrv->fPhydrv_enable_set(unit, port, ENABLED)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_HAL), "unit=%u,port=%u", unit, port);
                return;
            }
        }
    }
}
#endif

#if defined(CONFIG_SDK_RTL8295R)
void
_switch_serdesDrv_init(uint32 unit, rtk_port_t port)
{

    if (MAC_SDS_CTRL(unit, port) == NULL)
    {
        /* probe for switch serdes driver */
        MAC_SDS_CTRL(unit, port) = phy_identify_10GeSerdesDrv_find(unit, port);
        if (MAC_SDS_CTRL(unit, port) == NULL)
        {
            RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u unable to find MAC driver.", unit, port);
        }
        else
        {
            if (HWP_UNIT_VALID_LOCAL(unit))
            {
                _switch_serdes_init(unit, port);
            }
        }
    }

    return;
}
#endif

/* Function Name:
 *      phy_8295_mdxMacId_get
 * Description:
 *      Transfer port to the MAC ID that are used for access 8295
 * Input:
 *      unit        - unit id
 *      port        - port number
 * Output:
 *      pMdxMacId   - MAC ID for MDC/MDIO
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
int32
phy_8295_mdxMacId_get(uint32 unit, rtk_port_t port, uint32 *pMdxMacId)
{
    rtk_port_t      base_port;
    if (HWP_CHIP_ID(unit) == RTL8396M_CHIP_ID)
    {
        /* 96M 10G port use Cl.22 to access to 95R */
        if (HWP_10GE_PORT(unit, port))
        {
             drv_smi_mdxProtoSel_t   mdxProto = DRV_SMI_MDX_PROTO_C22;
            if (   (hal_miim_portSmiMdxProto_get(unit, port, &mdxProto) == RT_ERR_OK)
                && (mdxProto == DRV_SMI_MDX_PROTO_C22))
            {
                if (port == 24)
                {
                    *pMdxMacId = 25;
                    return RT_ERR_OK;
                }
                else if (port == 36)
                {
                    *pMdxMacId = 26;
                    return RT_ERR_OK;
                }
            }
        }
    }

    if ((base_port = HWP_PHY_BASE_MACID(unit, port)) != HWP_NONE)
    {
        *pMdxMacId = base_port;
        return RT_ERR_OK;
    }

    return RT_ERR_FAILED;
}


/*
 * Get PHY SMI protocol
 */
int32
_phy_8295_portMacSmiProto_get(uint32 unit, rtk_port_t port, drv_smi_mdxProtoSel_t *pProto)
{
    drv_smi_mdxProtoSel_t   proto;

    if (hal_miim_portSmiMdxProto_get(unit, port, &proto) == RT_ERR_OK)
        *pProto = proto;
    else
        *pProto = DRV_SMI_MDX_PROTO_C22;
    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8295_phyReg_read
 * Description:
 *      Read 8295 register using PHY page, reg format.
 *      When the MDC/MDIO is using Clause 45 format, this API will transfer the register to MMD.
 * Input:
 *      unit - unit ID
 *      mdxMacId - mac ID for access the chip
 *      phy_page - PHY page number
 *      phy_reg - PHY reg number
 * Output:
 *      pData - data value of the register
 * Return:
 *      None
 * Note:
 *      This API only valid for access registers that defined in register file.
 */
int32
phy_8295_phyReg_read(uint32 unit, rtk_port_t mdxMacId, uint32 phy_page, uint32 phy_reg, uint32 *pData)
{
    uint32  mmdReg;
    hal_control_t   *pHalCtrl;
    drv_smi_mdxProtoSel_t   proto;

    if (phy_reg < 16)
    {
        /* not able to transfer stander reg to MMD */
        return RT_ERR_INPUT;
    }

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_CHIP_NOT_FOUND;

    if (_phy_8295_portMacSmiProto_get(unit, mdxMacId, &proto) != RT_ERR_OK)
    {
        return RT_ERR_FAILED;
    }

    if (proto == DRV_SMI_MDX_PROTO_C22)
    {
        if (MACDRV(pHalCtrl)->fMdrv_miim_read == NULL)
            return RT_ERR_CHIP_NOT_SUPPORTED;

        return MACDRV(pHalCtrl)->fMdrv_miim_read(unit, mdxMacId, phy_page, phy_reg, pData);
    }
    else if (proto == DRV_SMI_MDX_PROTO_C45)
    {
        if (MACDRV(pHalCtrl)->fMdrv_miim_mmd_read == NULL)
            return RT_ERR_CHIP_NOT_SUPPORTED;

        mmdReg = PHY_8295_PAGEREG_TO_MMDREG(phy_page, phy_reg);
        return MACDRV(pHalCtrl)->fMdrv_miim_mmd_read(unit, mdxMacId, RTL8295_MMD_DEV_VENDOR_1, mmdReg, pData);
    }

    return RT_ERR_CHIP_NOT_SUPPORTED;
}

/* Function Name:
 *      phy_8295_phyReg_write
 * Description:
 *      Write 8295 register using PHY page, reg format.
 *      When the MDC/MDIO is using Clause 45 format, this API will transfer the register to MMD.
 * Input:
 *      unit - unit ID
 *      mdxMacId - mac ID for access the chip
 *      phy_page - PHY page number
 *      phy_reg - PHY reg number
 *      data - data value set to the register
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      This API only valid for access registers that defined in register file.
 */
int32
phy_8295_phyReg_write(uint32 unit, rtk_port_t mdxMacId, uint32 phy_page, uint32 phy_reg, uint32 data)
{
    uint32      mmdReg;
    hal_control_t   *pHalCtrl;
    drv_smi_mdxProtoSel_t   proto;

    if (phy_reg < 16)
    {
        /* not able to transfer stander reg to MMD */
        return RT_ERR_INPUT;
    }

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_CHIP_NOT_FOUND;

    if (_phy_8295_portMacSmiProto_get(unit, mdxMacId, &proto) != RT_ERR_OK)
    {
        return RT_ERR_FAILED;
    }

    if (proto == DRV_SMI_MDX_PROTO_C22)
    {
        if (MACDRV(pHalCtrl)->fMdrv_miim_write == NULL)
            return RT_ERR_CHIP_NOT_SUPPORTED;

        return MACDRV(pHalCtrl)->fMdrv_miim_write(unit, mdxMacId, phy_page, phy_reg, data);
    }
    else if (proto == DRV_SMI_MDX_PROTO_C45)
    {
        if (MACDRV(pHalCtrl)->fMdrv_miim_mmd_write == NULL)
            return RT_ERR_CHIP_NOT_SUPPORTED;

        mmdReg = PHY_8295_PAGEREG_TO_MMDREG(phy_page, phy_reg);
        return MACDRV(pHalCtrl)->fMdrv_miim_mmd_write(unit, mdxMacId, RTL8295_MMD_DEV_VENDOR_1, mmdReg, data);
    }

    return RT_ERR_CHIP_NOT_SUPPORTED;
}


/* Function Name:
 *      phy_8295_sdsReg_read
 * Description:
 *      Write 8295 serdes register
 *      When the MDC/MDIO is using Clause 45 format, this API will transfer the register to MMD.
 * Input:
 *      unit - unit ID
 *      mdxMacId - mac ID for access the chip
 *      s0_phy_page - using S0 phy page for indicating the what register to access.
 *              the function will use sdsId to find the real phy page of the serdes's register.
 *      phy_reg - PHY reg number
 * Output:
 *      pData - data value of the register
 * Return:
 *      None
 * Note:
 *      This API only valid for access registers that defined in register file.
 */
int32
phy_8295_sdsReg_read(uint32 unit, uint32 mdxMacId, uint32 sdsId, uint32 s0_phy_page, uint32 phy_reg, uint32 *pData)
{
    uint32  page_idx;

    if (sdsId >= PHY_8295_SDS_REG_PAGE_OFFSET_IDX_MAX)
    {
        return RT_ERR_OUT_OF_RANGE;
    }

    if ((page_idx = s0_phy_page - PHY_8295_PAGE_BASE_OFFSET_S0) >= PHY_8295_SDS_PAGE_NUM_MAX)
    {
        return RT_ERR_OUT_OF_RANGE;
    }

    return phy_8295_phyReg_read(unit, mdxMacId, (sds_reg_page_offset[sdsId] + page_idx), phy_reg, pData);
}


/* Function Name:
 *      phy_8295_sdsReg_write
 * Description:
 *      Write 8295 serdes register
 *      When the MDC/MDIO is using Clause 45 format, this API will transfer the register to MMD.
 * Input:
 *      unit - unit ID
 *      mdxMacId - mac ID for access the chip
 *      s0_phy_page - using S0 phy page for indicating the what register to access.
 *              the function will use sdsId to find the real phy page of the serdes's register.
 *      phy_reg - PHY reg number
 *      data - data value set to the register
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      This API only valid for access registers that defined in register file.
 */
int32
phy_8295_sdsReg_write(uint32 unit, uint32 mdxMacId, uint32 sdsId, uint32 s0_phy_page, uint32 phy_reg, uint32 data)
{
    uint32  page_idx;

    if (sdsId >= PHY_8295_SDS_REG_PAGE_OFFSET_IDX_MAX)
    {
        return RT_ERR_OUT_OF_RANGE;
    }

    if ((page_idx = s0_phy_page - PHY_8295_PAGE_BASE_OFFSET_S0) >= PHY_8295_SDS_PAGE_NUM_MAX)
    {
        return RT_ERR_OUT_OF_RANGE;
    }

    return phy_8295_phyReg_write(unit, mdxMacId, (sds_reg_page_offset[sdsId] + page_idx), phy_reg, data);
}


/*
 * port ID to 8295 SerDes ID
 */
int32
_phy_8295_portToSdsId_get(uint32 unit, rtk_port_t port, uint32 *pSdsId)
{
    if ((HWP_PHY_MODEL_BY_PORT(unit, port) == RTK_PHYTYPE_RTL8295R) || (HWP_PHY_MODEL_BY_PORT(unit, port) == RTK_PHYTYPE_RTL8295R_C22))
    {
        *pSdsId = PHY_8295_10G_PORT_SDSID_START;
        return RT_ERR_OK;
    }
    else if ((HWP_PHY_MODEL_BY_PORT(unit, port) == RTK_PHYTYPE_RTL8214QF) || (HWP_PHY_MODEL_BY_PORT(unit, port) == RTK_PHYTYPE_RTL8214QF_NC5))
    {
        *pSdsId = port - (HWP_PHY_BASE_MACID(unit, port)) + PHY_8295_1G_PORT_SDSID_START;
        if ((*pSdsId < PHY_8295_1G_PORT_SDSID_START) || (*pSdsId > PHY_8295_1G_PORT_SDSID_END))
        {
            return RT_ERR_FAILED;
        }
        return RT_ERR_OK;
    }
    else if ((HWP_PHY_MODEL_BY_PORT(unit, port) == RTK_PHYTYPE_RTL8224QF))
    {
        uint32 base_port;
        uint8 offset;
        base_port = HWP_PHY_BASE_MACID(unit, port);
        if (hwp_get_offset_by_baseport_port(unit, base_port, port, &offset) == RT_ERR_OK)
        {
            if ((*pSdsId = offset + PHY_8295_1G_PORT_SDSID_START) > PHY_8295_1G_PORT_SDSID_END)
            {
                return RT_ERR_FAILED;
            }
            return RT_ERR_OK;
        }
    }
    return RT_ERR_FAILED;
}


/* Function Name:
 *      _phy_8295_rxCaliParam_get
 * Description:
 *      Set rx calibration parameters occording to port type and serdes ID
 * Input:
 *      unit        - unit ID
 *      port        - port ID
 *      sdsId       - SerDes ID
 * Output:
 *      rxCaliParam - rx calibration parameters
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
void
_phy_8295_rxCaliParam_get(uint32 unit, rtk_port_t port, uint32 sdsId, phy_8295_rxCaliParam_t *rxCaliParam)
{
    int32                   ret;
    uint16                  cable_offset = 0;
#if defined(CONFIG_SDK_RTL8295R) || defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
    phy_8295_rxCaliConf_t   rxCaliConf;
#endif
    phy_8295_rxCaliSdsConf_t    *sdsCfg = NULL;

    if (sdsCfg);        /* to avoid compile warning */
    if (cable_offset);  /* to avoid compile warning */

    osal_memset(rxCaliParam, 0, sizeof(phy_8295_rxCaliParam_t));

    if ((sdsId != 0) && (sdsId != 1))
    {
        ret = RT_ERR_OUT_OF_RANGE;
        RT_ERR(ret, (MOD_HAL), "unit=%u,port=%u,sdsId=%u,Invalid serdes ID. use default.", unit, port, sdsId);
        phy_8295_rxCaliParamDefault_get(sdsId, rxCaliParam);
        return;
    }

    if (g8295rInfo[unit] == NULL)
    {
        ret = RT_ERR_NOT_INIT;
        RT_ERR(ret, (MOD_HAL), "unit=%u,port=%u,sdsId=%u,g8295rInfo not init. use default.", unit, port, sdsId);
        phy_8295_rxCaliParamDefault_get(sdsId, rxCaliParam);
        return;
    }

#if defined(CONFIG_SDK_RTL8295R)
    if (HWP_PHY_MODEL_BY_PORT(unit, port) == RTK_PHYTYPE_RTL8295R)
    {
        if ((ret = phy_8295r_rxCaliConfPort_get(unit, port, &rxCaliConf)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_HAL), "unit=%u,port=%u,sdsId=%u,rxCaliConf_get fail. use default.", unit, port, sdsId);
            /* use default parameter if fail to get the config */
            phy_8295_rxCaliParamDefault_get(sdsId, rxCaliParam);
            return;
        }

        if (sdsId == 1)
        {
            sdsCfg = &rxCaliConf.s1;
            switch (PORT_10G_MEDIA(unit, port))
            {
              case PORT_10GMEDIA_DAC_300CM:
              case PORT_10GMEDIA_DAC_500CM: //DAC_LONG
                cable_offset = rxCaliConf.dacLongCableOffset;
                break;
              default:
                break;
            }
        }
        else
        {
            sdsCfg = &rxCaliConf.s0;
        }

        rxCaliParam->cable_offset   = cable_offset;
        rxCaliParam->tap0_init_val  = sdsCfg->tap0InitVal;
        rxCaliParam->vth_min        = sdsCfg->vthMinThr;
        if (sdsCfg->eqHoldEnable == DISABLED)
            rxCaliParam->flags |= PHY_8295_RXCALI_PARAM_FLAG_EQ_HOLD_DISABLE;
        if (sdsCfg->dfeTap1_4Enable == DISABLED)
            rxCaliParam->flags |= PHY_8295_RXCALI_PARAM_FLAG_DFETAP1_4_DISABLE;
        if (sdsCfg->dfeAuto == DISABLED)
            rxCaliParam->flags |= PHY_8295_RXCALI_PARAM_FLAG_DFEAUTO_DISABLE;
        return;
    }
    else
#endif
#if defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
    if (   (HWP_PHY_MODEL_BY_PORT(unit, port) == RTK_PHYTYPE_RTL8214QF) || (HWP_PHY_MODEL_BY_PORT(unit, port) == RTK_PHYTYPE_RTL8214QF_NC5)
        || (HWP_PHY_MODEL_BY_PORT(unit, port) == RTK_PHYTYPE_RTL8224QF))
    {
        uint32  basePort = HWP_PHY_BASE_MACID(unit, port);
        if ((ret = phy_8214qf_rxCaliConf_get(unit, basePort, &rxCaliConf)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_HAL), "unit=%u,port=%u,sdsId=%u,rxCaliConf_get fail. use default.", unit, basePort, sdsId);
            /* use default parameter if fail to get the config */
            phy_8295_rxCaliParamDefault_get(sdsId, rxCaliParam);
            return;
        }

        RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,sdsId=%u cableOffset=%u s0(rxcali=%u tap0=0x%x vth=0x%x qe=%u dfeTap1=%u dfeAuto=%u) s1(rxcali=%u tap0=0x%x vth=0x%x qe=%u dfeTap1=%u dfeAuto=%u)",
                    unit, basePort, sdsId,
                    rxCaliConf.dacLongCableOffset,
                    rxCaliConf.s0.rxCaliEnable, rxCaliConf.s0.tap0InitVal, rxCaliConf.s0.vthMinThr, rxCaliConf.s0.eqHoldEnable, rxCaliConf.s0.dfeTap1_4Enable, rxCaliConf.s0.dfeAuto,
                    rxCaliConf.s1.rxCaliEnable, rxCaliConf.s1.tap0InitVal, rxCaliConf.s1.vthMinThr, rxCaliConf.s1.eqHoldEnable, rxCaliConf.s1.dfeTap1_4Enable, rxCaliConf.s1.dfeAuto);
        if (sdsId == 1)
        {
            sdsCfg = &rxCaliConf.s1;
        }
        else
        {
            sdsCfg = &rxCaliConf.s0;
        }

        rxCaliParam->cable_offset   = rxCaliConf.dacLongCableOffset;
        rxCaliParam->tap0_init_val  = sdsCfg->tap0InitVal;
        rxCaliParam->vth_min        = sdsCfg->vthMinThr;
        if (sdsCfg->eqHoldEnable == DISABLED)
            rxCaliParam->flags |= PHY_8295_RXCALI_PARAM_FLAG_EQ_HOLD_DISABLE;
        if (sdsCfg->dfeTap1_4Enable == DISABLED)
            rxCaliParam->flags |= PHY_8295_RXCALI_PARAM_FLAG_DFETAP1_4_DISABLE;
        if (sdsCfg->dfeAuto == DISABLED)
            rxCaliParam->flags |= PHY_8295_RXCALI_PARAM_FLAG_DFEAUTO_DISABLE;
        return;
    }
    else
#endif
    {
        phy_8295_rxCaliParamDefault_get(sdsId, rxCaliParam);
        return;
    }

    return;
}


/* Function Name:
 *      _phy_8295_ptpSXP0RegNPageReg_get
 * Description:
 *      Get PTP per SerDes register page, reg: PTP_S[X]P0_REG[N] or PTP_TG_REG[N]
 *      (X is 4 ~ 7; N is 00~12, TG is for S1)
 * Input:
 *      sdsId       - SerDes ID
 *      regIdx      - register number, 0~12
 * Output:
 *      phyPage   - register PHY page
 *      phyReg   - register PHY reg index
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32
_phy_8295_ptpSXP0RegNPageReg_get(uint32 sdsId, phy_8295_ptpSxP0RegIdx_t regIdx, uint32 *phyPage, uint32 *phyReg)
{
    if (regIdx == RTL8295_PTP_SXP0_REG13)
    {
        switch(sdsId)
        {
          case 1:
            *phyPage = RTL8295_PKG_CTRL_PTP_TG_REG13_PAGE;
            *phyReg  = RTL8295_PKG_CTRL_PTP_TG_REG13_REG;
            break;
          case 4:
            *phyPage = RTL8295_PKG_CTRL_PTP_S4P0_REG13_PAGE;
            *phyReg  = RTL8295_PKG_CTRL_PTP_S4P0_REG13_REG;
            break;
          case 5:
            *phyPage = RTL8295_PKG_CTRL_PTP_S5P0_REG13_PAGE;
            *phyReg  = RTL8295_PKG_CTRL_PTP_S5P0_REG13_REG;
            break;
          case 6:
            *phyPage = RTL8295_PKG_CTRL_PTP_S6P0_REG13_PAGE;
            *phyReg  = RTL8295_PKG_CTRL_PTP_S6P0_REG13_REG;
            break;
          case 7:
            *phyPage = RTL8295_PKG_CTRL_PTP_S7P0_REG13_PAGE;
            *phyReg  = RTL8295_PKG_CTRL_PTP_S7P0_REG13_REG;
            break;
          default:
            return RT_ERR_FAILED;
        }
        return RT_ERR_OK;
    }
    else if (regIdx == RTL8295_PTP_SXP0_REG12)
    {
        switch(sdsId)
        {
          case 1:
            *phyPage = RTL8295_PTP_CTRL_PTP_TG_REG12_PAGE;
            *phyReg  = RTL8295_PTP_CTRL_PTP_TG_REG12_REG;
            break;
          case 4:
            *phyPage = RTL8295_PTP_CTRL_PTP_S4P0_REG12_PAGE;
            *phyReg  = RTL8295_PTP_CTRL_PTP_S4P0_REG12_REG;
            break;
          case 5:
            *phyPage = RTL8295_PTP_CTRL_PTP_S5P0_REG12_PAGE;
            *phyReg  = RTL8295_PTP_CTRL_PTP_S5P0_REG12_REG;
            break;
          case 6:
            *phyPage = RTL8295_PTP_CTRL_PTP_S6P0_REG12_PAGE;
            *phyReg  = RTL8295_PTP_CTRL_PTP_S6P0_REG12_REG;
            break;
          case 7:
            *phyPage = RTL8295_PTP_CTRL_PTP_S7P0_REG12_PAGE;
            *phyReg  = RTL8295_PTP_CTRL_PTP_S7P0_REG12_REG;
            break;
          default:
            return RT_ERR_FAILED;
        }
        return RT_ERR_OK;
    }
    else if (regIdx == RTL8295_PTP_SXP0_REG11)
    {
        switch(sdsId)
        {
          case 1:
            *phyPage = RTL8295_PTP_CTRL_PTP_TG_REG11_PAGE;
            *phyReg  = RTL8295_PTP_CTRL_PTP_TG_REG11_REG;
            break;
          case 4:
            *phyPage = RTL8295_PTP_CTRL_PTP_S4P0_REG11_PAGE;
            *phyReg  = RTL8295_PTP_CTRL_PTP_S4P0_REG11_REG;
            break;
          case 5:
            *phyPage = RTL8295_PTP_CTRL_PTP_S5P0_REG11_PAGE;
            *phyReg  = RTL8295_PTP_CTRL_PTP_S5P0_REG11_REG;
            break;
          case 6:
            *phyPage = RTL8295_PTP_CTRL_PTP_S6P0_REG11_PAGE;
            *phyReg  = RTL8295_PTP_CTRL_PTP_S6P0_REG11_REG;
            break;
          case 7:
            *phyPage = RTL8295_PTP_CTRL_PTP_S7P0_REG11_PAGE;
            *phyReg  = RTL8295_PTP_CTRL_PTP_S7P0_REG11_REG;
            break;
          default:
            return RT_ERR_FAILED;
        }
        return RT_ERR_OK;
    }
    else if (regIdx == RTL8295_PTP_SXP0_REG10)
    {
        switch(sdsId)
        {
          case 1:
            *phyPage = RTL8295_PTP_CTRL_PTP_TG_REG10_PAGE;
            *phyReg  = RTL8295_PTP_CTRL_PTP_TG_REG10_REG;
            break;
          case 4:
            *phyPage = RTL8295_PTP_CTRL_PTP_S4P0_REG10_PAGE;
            *phyReg  = RTL8295_PTP_CTRL_PTP_S4P0_REG10_REG;
            break;
          case 5:
            *phyPage = RTL8295_PTP_CTRL_PTP_S5P0_REG10_PAGE;
            *phyReg  = RTL8295_PTP_CTRL_PTP_S5P0_REG10_REG;
            break;
          case 6:
            *phyPage = RTL8295_PTP_CTRL_PTP_S6P0_REG10_PAGE;
            *phyReg  = RTL8295_PTP_CTRL_PTP_S6P0_REG10_REG;
            break;
          case 7:
            *phyPage = RTL8295_PTP_CTRL_PTP_S7P0_REG10_PAGE;
            *phyReg  = RTL8295_PTP_CTRL_PTP_S7P0_REG10_REG;
            break;
          default:
            return RT_ERR_FAILED;
        }
        return RT_ERR_OK;
    }
    else if (regIdx == RTL8295_PTP_SXP0_REG09)
    {
        switch(sdsId)
        {
          case 1:
            *phyPage = RTL8295_PTP_CTRL_PTP_TG_REG09_PAGE;
            *phyReg  = RTL8295_PTP_CTRL_PTP_TG_REG09_REG;
            break;
          case 4:
            *phyPage = RTL8295_PTP_CTRL_PTP_S4P0_REG09_PAGE;
            *phyReg  = RTL8295_PTP_CTRL_PTP_S4P0_REG09_REG;
            break;
          case 5:
            *phyPage = RTL8295_PTP_CTRL_PTP_S5P0_REG09_PAGE;
            *phyReg  = RTL8295_PTP_CTRL_PTP_S5P0_REG09_REG;
            break;
          case 6:
            *phyPage = RTL8295_PTP_CTRL_PTP_S6P0_REG09_PAGE;
            *phyReg  = RTL8295_PTP_CTRL_PTP_S6P0_REG09_REG;
            break;
          case 7:
            *phyPage = RTL8295_PTP_CTRL_PTP_S7P0_REG09_PAGE;
            *phyReg  = RTL8295_PTP_CTRL_PTP_S7P0_REG09_REG;
            break;
          default:
            return RT_ERR_FAILED;
        }
        return RT_ERR_OK;
    }
    else if (regIdx == RTL8295_PTP_SXP0_REG08)
    {
        switch(sdsId)
        {
          case 1:
            *phyPage = RTL8295_PTP_CTRL_PTP_TG_REG08_PAGE;
            *phyReg  = RTL8295_PTP_CTRL_PTP_TG_REG08_REG;
            break;
          case 4:
            *phyPage = RTL8295_PTP_CTRL_PTP_S4P0_REG08_PAGE;
            *phyReg  = RTL8295_PTP_CTRL_PTP_S4P0_REG08_REG;
            break;
          case 5:
            *phyPage = RTL8295_PTP_CTRL_PTP_S5P0_REG08_PAGE;
            *phyReg  = RTL8295_PTP_CTRL_PTP_S5P0_REG08_REG;
            break;
          case 6:
            *phyPage = RTL8295_PTP_CTRL_PTP_S6P0_REG08_PAGE;
            *phyReg  = RTL8295_PTP_CTRL_PTP_S6P0_REG08_REG;
            break;
          case 7:
            *phyPage = RTL8295_PTP_CTRL_PTP_S7P0_REG08_PAGE;
            *phyReg  = RTL8295_PTP_CTRL_PTP_S7P0_REG08_REG;
            break;
          default:
            return RT_ERR_FAILED;
        }
        return RT_ERR_OK;
    }
    else if (regIdx == RTL8295_PTP_SXP0_REG07)
    {
        switch(sdsId)
        {
          case 1:
            *phyPage = RTL8295_PTP_CTRL_PTP_TG_REG07_PAGE;
            *phyReg  = RTL8295_PTP_CTRL_PTP_TG_REG07_REG;
            break;
          case 4:
            *phyPage = RTL8295_PTP_CTRL_PTP_S4P0_REG07_PAGE;
            *phyReg  = RTL8295_PTP_CTRL_PTP_S4P0_REG07_REG;
            break;
          case 5:
            *phyPage = RTL8295_PTP_CTRL_PTP_S5P0_REG07_PAGE;
            *phyReg  = RTL8295_PTP_CTRL_PTP_S5P0_REG07_REG;
            break;
          case 6:
            *phyPage = RTL8295_PTP_CTRL_PTP_S6P0_REG07_PAGE;
            *phyReg  = RTL8295_PTP_CTRL_PTP_S6P0_REG07_REG;
            break;
          case 7:
            *phyPage = RTL8295_PTP_CTRL_PTP_S7P0_REG07_PAGE;
            *phyReg  = RTL8295_PTP_CTRL_PTP_S7P0_REG07_REG;
            break;
          default:
            return RT_ERR_FAILED;
        }
        return RT_ERR_OK;
    }
    else if (regIdx == RTL8295_PTP_SXP0_REG06)
    {
        switch(sdsId)
        {
          case 1:
            *phyPage = RTL8295_PTP_CTRL_PTP_TG_REG06_PAGE;
            *phyReg  = RTL8295_PTP_CTRL_PTP_TG_REG06_REG;
            break;
          case 4:
            *phyPage = RTL8295_PTP_CTRL_PTP_S4P0_REG06_PAGE;
            *phyReg  = RTL8295_PTP_CTRL_PTP_S4P0_REG06_REG;
            break;
          case 5:
            *phyPage = RTL8295_PTP_CTRL_PTP_S5P0_REG06_PAGE;
            *phyReg  = RTL8295_PTP_CTRL_PTP_S5P0_REG06_REG;
            break;
          case 6:
            *phyPage = RTL8295_PTP_CTRL_PTP_S6P0_REG06_PAGE;
            *phyReg  = RTL8295_PTP_CTRL_PTP_S6P0_REG06_REG;
            break;
          case 7:
            *phyPage = RTL8295_PTP_CTRL_PTP_S7P0_REG06_PAGE;
            *phyReg  = RTL8295_PTP_CTRL_PTP_S7P0_REG06_REG;
            break;
          default:
            return RT_ERR_FAILED;
        }
        return RT_ERR_OK;
    }
    else if (regIdx == RTL8295_PTP_SXP0_REG05)
    {
        switch(sdsId)
        {
          case 1:
            *phyPage = RTL8295_PTP_CTRL_PTP_TG_REG05_PAGE;
            *phyReg  = RTL8295_PTP_CTRL_PTP_TG_REG05_REG;
            break;
          case 4:
            *phyPage = RTL8295_PTP_CTRL_PTP_S4P0_REG05_PAGE;
            *phyReg  = RTL8295_PTP_CTRL_PTP_S4P0_REG05_REG;
            break;
          case 5:
            *phyPage = RTL8295_PTP_CTRL_PTP_S5P0_REG05_PAGE;
            *phyReg  = RTL8295_PTP_CTRL_PTP_S5P0_REG05_REG;
            break;
          case 6:
            *phyPage = RTL8295_PTP_CTRL_PTP_S6P0_REG05_PAGE;
            *phyReg  = RTL8295_PTP_CTRL_PTP_S6P0_REG05_REG;
            break;
          case 7:
            *phyPage = RTL8295_PTP_CTRL_PTP_S7P0_REG05_PAGE;
            *phyReg  = RTL8295_PTP_CTRL_PTP_S7P0_REG05_REG;
            break;
          default:
            return RT_ERR_FAILED;
        }
        return RT_ERR_OK;
    }
    else if (regIdx == RTL8295_PTP_SXP0_REG04)
    {
        switch(sdsId)
        {
          case 1:
            *phyPage = RTL8295_PTP_CTRL_PTP_TG_REG04_PAGE;
            *phyReg  = RTL8295_PTP_CTRL_PTP_TG_REG04_REG;
            break;
          case 4:
            *phyPage = RTL8295_PTP_CTRL_PTP_S4P0_REG04_PAGE;
            *phyReg  = RTL8295_PTP_CTRL_PTP_S4P0_REG04_REG;
            break;
          case 5:
            *phyPage = RTL8295_PTP_CTRL_PTP_S5P0_REG04_PAGE;
            *phyReg  = RTL8295_PTP_CTRL_PTP_S5P0_REG04_REG;
            break;
          case 6:
            *phyPage = RTL8295_PTP_CTRL_PTP_S6P0_REG04_PAGE;
            *phyReg  = RTL8295_PTP_CTRL_PTP_S6P0_REG04_REG;
            break;
          case 7:
            *phyPage = RTL8295_PTP_CTRL_PTP_S7P0_REG04_PAGE;
            *phyReg  = RTL8295_PTP_CTRL_PTP_S7P0_REG04_REG;
            break;
          default:
            return RT_ERR_FAILED;
        }
        return RT_ERR_OK;
    }
    else if (regIdx == RTL8295_PTP_SXP0_REG03)
    {
        switch(sdsId)
        {
          case 1:
            *phyPage = RTL8295_PTP_CTRL_PTP_TG_REG03_PAGE;
            *phyReg  = RTL8295_PTP_CTRL_PTP_TG_REG03_REG;
            break;
          case 4:
            *phyPage = RTL8295_PTP_CTRL_PTP_S4P0_REG03_PAGE;
            *phyReg  = RTL8295_PTP_CTRL_PTP_S4P0_REG03_REG;
            break;
          case 5:
            *phyPage = RTL8295_PTP_CTRL_PTP_S5P0_REG03_PAGE;
            *phyReg  = RTL8295_PTP_CTRL_PTP_S5P0_REG03_REG;
            break;
          case 6:
            *phyPage = RTL8295_PTP_CTRL_PTP_S6P0_REG03_PAGE;
            *phyReg  = RTL8295_PTP_CTRL_PTP_S6P0_REG03_REG;
            break;
          case 7:
            *phyPage = RTL8295_PTP_CTRL_PTP_S7P0_REG03_PAGE;
            *phyReg  = RTL8295_PTP_CTRL_PTP_S7P0_REG03_REG;
            break;
          default:
            return RT_ERR_FAILED;
        }
        return RT_ERR_OK;
    }
    else if (regIdx == RTL8295_PTP_SXP0_REG02)
    {
        switch(sdsId)
        {
          case 1:
            *phyPage = RTL8295_PTP_CTRL_PTP_TG_REG02_PAGE;
            *phyReg  = RTL8295_PTP_CTRL_PTP_TG_REG02_REG;
            break;
          case 4:
            *phyPage = RTL8295_PTP_CTRL_PTP_S4P0_REG02_PAGE;
            *phyReg  = RTL8295_PTP_CTRL_PTP_S4P0_REG02_REG;
            break;
          case 5:
            *phyPage = RTL8295_PTP_CTRL_PTP_S5P0_REG02_PAGE;
            *phyReg  = RTL8295_PTP_CTRL_PTP_S5P0_REG02_REG;
            break;
          case 6:
            *phyPage = RTL8295_PTP_CTRL_PTP_S6P0_REG02_PAGE;
            *phyReg  = RTL8295_PTP_CTRL_PTP_S6P0_REG02_REG;
            break;
          case 7:
            *phyPage = RTL8295_PTP_CTRL_PTP_S7P0_REG02_PAGE;
            *phyReg  = RTL8295_PTP_CTRL_PTP_S7P0_REG02_REG;
            break;
          default:
            return RT_ERR_FAILED;
        }
        return RT_ERR_OK;
    }
    else if (regIdx == RTL8295_PTP_SXP0_REG01)
    {
        switch(sdsId)
        {
          case 1:
            *phyPage = RTL8295_PTP_CTRL_PTP_TG_REG01_PAGE;
            *phyReg  = RTL8295_PTP_CTRL_PTP_TG_REG01_REG;
            break;
          case 4:
            *phyPage = RTL8295_PTP_CTRL_PTP_S4P0_REG01_PAGE;
            *phyReg  = RTL8295_PTP_CTRL_PTP_S4P0_REG01_REG;
            break;
          case 5:
            *phyPage = RTL8295_PTP_CTRL_PTP_S5P0_REG01_PAGE;
            *phyReg  = RTL8295_PTP_CTRL_PTP_S5P0_REG01_REG;
            break;
          case 6:
            *phyPage = RTL8295_PTP_CTRL_PTP_S6P0_REG01_PAGE;
            *phyReg  = RTL8295_PTP_CTRL_PTP_S6P0_REG01_REG;
            break;
          case 7:
            *phyPage = RTL8295_PTP_CTRL_PTP_S7P0_REG01_PAGE;
            *phyReg  = RTL8295_PTP_CTRL_PTP_S7P0_REG01_REG;
            break;
          default:
            return RT_ERR_FAILED;
        }
        return RT_ERR_OK;
    }
    else if (regIdx == RTL8295_PTP_SXP0_REG00)
    {
        switch(sdsId)
        {
          case 1:
            *phyPage = RTL8295_PTP_CTRL_PTP_TG_REG00_PAGE;
            *phyReg  = RTL8295_PTP_CTRL_PTP_TG_REG00_REG;
            break;
          case 4:
            *phyPage = RTL8295_PTP_CTRL_PTP_S4P0_REG00_PAGE;
            *phyReg  = RTL8295_PTP_CTRL_PTP_S4P0_REG00_REG;
            break;
          case 5:
            *phyPage = RTL8295_PTP_CTRL_PTP_S5P0_REG00_PAGE;
            *phyReg  = RTL8295_PTP_CTRL_PTP_S5P0_REG00_REG;
            break;
          case 6:
            *phyPage = RTL8295_PTP_CTRL_PTP_S6P0_REG00_PAGE;
            *phyReg  = RTL8295_PTP_CTRL_PTP_S6P0_REG00_REG;
            break;
          case 7:
            *phyPage = RTL8295_PTP_CTRL_PTP_S7P0_REG00_PAGE;
            *phyReg  = RTL8295_PTP_CTRL_PTP_S7P0_REG00_REG;
            break;
          default:
            return RT_ERR_FAILED;
        }
        return RT_ERR_OK;
    }

    return RT_ERR_FAILED;
}


/* Function Name:
 *      _phy_8295_PtpSXP0RegN_read
 * Description:
 *      Read PTP per SerDes register: PTP_S[X]P0_REG[N] or PTP_TG_REG[N]
 *      (X is 4 ~ 7; N is 00~12, TG is for S1)
 * Input:
 *      unit        - unit ID
 *      mdxMacId    - MDC/MDIO MAC ID to the chip
 *      sdsId       - SerDes ID
 *      regIdx      - register number, 0~12
 * Output:
 *      pData       - register data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_INPUT
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
_phy_8295_PtpSXP0RegN_read(uint32 unit, uint32 mdxMacId, uint32 sdsId, phy_8295_ptpSxP0RegIdx_t regIdx, uint32 *pData)
{
    uint32      phyPage, phyReg;
    int32       ret;

    if ((ret = _phy_8295_ptpSXP0RegNPageReg_get(sdsId, regIdx, &phyPage, &phyReg)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HAL), "sds %u regIdx %d", sdsId, regIdx);
        return ret;
    }

    PHY_8295_PHYREG_READ(unit, mdxMacId, phyPage, phyReg, pData);
    return RT_ERR_OK;
}

int32
_phy_8295_PtpSXP0RegN_write(uint32 unit, uint32 mdxMacId, uint32 sdsId, phy_8295_ptpSxP0RegIdx_t regIdx, uint32 data)
{
    uint32      phyPage, phyReg;
    int32       ret;

    if ((ret = _phy_8295_ptpSXP0RegNPageReg_get(sdsId, regIdx, &phyPage, &phyReg)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HAL), "sds %u regIdx %d", sdsId, regIdx);
        return ret;
    }

    PHY_8295_PHYREG_WRITE(unit, mdxMacId, phyPage, phyReg, data);
    return RT_ERR_OK;
}

#if defined(CONFIG_SDK_RTL8295R)
/* Function Name:
 *      phy_8295r_media_get
 * Description:
 *      Get PHY media type.
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pMedia - pointer buffer of phy media type
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - invalid parameter
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      1. media type is PORT_MEDIA_FIBER
 */
int32
phy_8295r_media_get(uint32 unit, rtk_port_t port, rtk_port_media_t *pMedia)
{

    *pMedia = PORT_MEDIA_FIBER;

    return RT_ERR_OK;
}
#endif


#if defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8214qf_media_get
 * Description:
 *      Get PHY media type.
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pMedia - pointer buffer of phy media type
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - invalid parameter
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      1. media type is PORT_MEDIA_FIBER
 */
int32
phy_8214qf_media_get(uint32 unit, rtk_port_t port, rtk_port_media_t *pMedia)
{

    *pMedia = PORT_MEDIA_FIBER;

    return RT_ERR_OK;
}
#endif

int32
_phy_8295_serdesModeRegOffsetMask_get(uint32 sdsId, uint32 *pPhyPage, uint32 *pPhyReg, uint32 *pOffset, uint32 *pMask)
{
    switch (sdsId)
    {
      case 0:
        *pPhyPage = RTL8295_SDS_CTRL_SDS_CTRL_S0_PAGE;
        *pPhyReg  = RTL8295_SDS_CTRL_SDS_CTRL_S0_REG;
        *pOffset = RTL8295_SDS_CTRL_SDS_CTRL_S0_SERDES_MODE_S0_OFFSET;
        *pMask   = RTL8295_SDS_CTRL_SDS_CTRL_S0_SERDES_MODE_S0_MASK;
        break;
      case 1:
        *pPhyPage = RTL8295_SDS_CTRL_SDS_CTRL_S1_PAGE;
        *pPhyReg  = RTL8295_SDS_CTRL_SDS_CTRL_S1_REG;
        *pOffset = RTL8295_SDS_CTRL_SDS_CTRL_S1_SERDES_MODE_S1_OFFSET;
        *pMask   = RTL8295_SDS_CTRL_SDS_CTRL_S1_SERDES_MODE_S1_MASK;
        break;
      case 4:
        *pPhyPage = RTL8295_SDS_CTRL_SDS_CTRL_S4_PAGE;
        *pPhyReg  = RTL8295_SDS_CTRL_SDS_CTRL_S4_REG;
        *pOffset = RTL8295_SDS_CTRL_SDS_CTRL_S4_SERDES_MODE_S4_OFFSET;
        *pMask   = RTL8295_SDS_CTRL_SDS_CTRL_S4_SERDES_MODE_S4_MASK;
        break;
      case 5:
        *pPhyPage = RTL8295_SDS_CTRL_SDS_CTRL_S5_PAGE;
        *pPhyReg  = RTL8295_SDS_CTRL_SDS_CTRL_S5_REG;
        *pOffset = RTL8295_SDS_CTRL_SDS_CTRL_S5_SERDES_MODE_S5_OFFSET;
        *pMask   = RTL8295_SDS_CTRL_SDS_CTRL_S5_SERDES_MODE_S5_MASK;
        break;
      case 6:
        *pPhyPage = RTL8295_SDS_CTRL_SDS_CTRL_S6_PAGE;
        *pPhyReg  = RTL8295_SDS_CTRL_SDS_CTRL_S6_REG;
        *pOffset = RTL8295_SDS_CTRL_SDS_CTRL_S6_SERDES_MODE_S6_OFFSET;
        *pMask   = RTL8295_SDS_CTRL_SDS_CTRL_S6_SERDES_MODE_S6_MASK;
        break;
      case 7:
        *pPhyPage = RTL8295_SDS_CTRL_SDS_CTRL_S7_PAGE;
        *pPhyReg  = RTL8295_SDS_CTRL_SDS_CTRL_S7_REG;
        *pOffset = RTL8295_SDS_CTRL_SDS_CTRL_S7_SERDES_MODE_S7_OFFSET;
        *pMask   = RTL8295_SDS_CTRL_SDS_CTRL_S7_SERDES_MODE_S7_MASK;
        break;
      default:
        return RT_ERR_PORT_ID;
    }/* end switch */
    return RT_ERR_OK;
}

int32
_phy_8295_serdesModeReg_get(uint32 unit, rtk_port_t port, uint32 mdxMacId, uint32 sdsId, uint32 *pMode)
{
    uint32      data, offset, mask;
    uint32      phyPage, phyReg;
    int32       ret;

    if ((ret = _phy_8295_serdesModeRegOffsetMask_get(sdsId, &phyPage, &phyReg, &offset, &mask)) != RT_ERR_OK)
    {
        return ret;
    }

    PHY_8295_PHYREG_READ(unit, mdxMacId, phyPage, phyReg, &data);
    *pMode = REG32_FIELD_GET(data, offset, mask);
    return RT_ERR_OK;
}

int32
_phy_8295_serdesMode_get(uint32 unit, rtk_port_t port, uint32 *pMode)
{
    uint32              mdxMacId, sdsId;

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);

    PHY_8295_SDSID_GET(unit, port, &sdsId);
    return _phy_8295_serdesModeReg_get(unit, port, mdxMacId, sdsId, pMode);
}

int32
_phy_8295_serdesModeReg_set(uint32 unit, rtk_port_t port, uint32 mdxMacId, uint32 sdsId, uint32 mode)
{
    uint32      data, offset, mask;
    uint32      phyPage, phyReg;
    int32       ret;

    if ((ret = _phy_8295_serdesModeRegOffsetMask_get(sdsId, &phyPage, &phyReg, &offset, &mask)) != RT_ERR_OK)
    {
        return ret;
    }

    PHY_8295_PHYREG_READ(unit, mdxMacId, phyPage, phyReg, &data);

    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,mdx=%u S%u mode %X -> %X", unit, port, mdxMacId, sdsId, (data & mask), mode);

    data = REG32_FIELD_SET(data, mode, offset, mask);

    PHY_8295_PHYREG_WRITE(unit, mdxMacId, phyPage, phyReg, data);

    return RT_ERR_OK;

}

int32
_phy_8295_portSdsMode_set(uint32 unit, rtk_port_t port, uint32 mode)
{
    uint32              mdxMacId, sdsId;

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);

    PHY_8295_SDSID_GET(unit, port, &sdsId);

    return _phy_8295_serdesModeReg_set(unit, port, mdxMacId, sdsId, mode);

}

#if defined(CONFIG_SDK_RTL8295R)
int32
_phy_8295_10GePortType_get(uint32 unit, rtk_port_t port, uint32 *port_flag)
{
    int32       ret;
    uint32      sdsMode;

    *port_flag = 0;
    if ((ret = _phy_8295_serdesMode_get(unit, port, &sdsMode)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HAL), "unit=%u,port=%u sdsMode", unit, port);
        return RT_ERR_FAILED;
    }

    switch (sdsMode)
    {
      case PHY_8295_SDS_MODE_10GR:
        *port_flag = PHY_8295_PORT_10G;
        return RT_ERR_OK;
      case PHY_8295_SDS_MODE_FIB1G:
      case PHY_8295_SDS_MODE_FIB100M:
        *port_flag = PHY_8295_PORT_1G;
        return RT_ERR_OK;
      case PHY_8295_SDS_MODE_FIB2_5G:
        *port_flag = PHY_8295_PORT_2_5G;
        return RT_ERR_OK;
      case PHY_8295_SDS_MODE_OFF:
        /* get from config */
        switch (PORT_10G_MEDIA(unit, port))
        {
          case PORT_10GMEDIA_FIBER_10G:
          case PORT_10GMEDIA_DAC_50CM:
          case PORT_10GMEDIA_DAC_100CM:
          case PORT_10GMEDIA_DAC_300CM:
          case PORT_10GMEDIA_DAC_500CM: //DAC_LONG
            *port_flag = PHY_8295_PORT_10G;
            return RT_ERR_OK;
          case PORT_10GMEDIA_FIBER_1G:
            *port_flag = PHY_8295_PORT_1G;
            return RT_ERR_OK;
          default:
            break;
        }
        *port_flag = PHY_8295_PORT_1G | PHY_8295_PORT_10G;
        return RT_ERR_OK;

      default:
        RT_ERR(ret, (MOD_HAL), "unit=%u,port=%u unknown mode %u", unit, port, sdsMode);
        return RT_ERR_PORT_NOT_SUPPORTED;
    }
}
#endif


#if defined(CONFIG_SDK_RTL8295R)
int32
_phy_8295r_autoNegoAbilityStaFiberStdReg_set(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility)
{
    uint32          mdxMacId, sdsId;
    uint32          phyData0;
    uint32          phyData4;
    rtk_enable_t    enable;

    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u", unit, port);

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);

    PHY_8295_SDSID_GET(unit, port, &sdsId);

    phy_8295r_autoNegoEnable_get(unit, port, &enable);


    /* get value from CHIP*/
    /* PHY_AN_ADVERTISEMENT_REG */
    PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_FIB_REG04_PAGE, RTL8295_SDS0_FIB_REG04_REG, &phyData4);

    phyData4 = phyData4 & ~(_1000BaseX_Pause_R4_MASK | _1000BaseX_AsymmetricPause_R4_MASK);
    phyData4 = phyData4 | (pAbility->FC << _1000BaseX_Pause_R4_OFFSET)
            | (pAbility->AsyFC << _1000BaseX_AsymmetricPause_R4_OFFSET);
    phyData4 = phyData4 & ~(_1000BaseX_HalfDuplex_R4_MASK | _1000BaseX_FullDuplex_R4_MASK);
    phyData4 = phyData4 | (pAbility->Half_1000 << _1000BaseX_HalfDuplex_R4_OFFSET)
            | (pAbility->Full_1000 << _1000BaseX_FullDuplex_R4_OFFSET);

    PHY_8295_SDSREG_WRITE(unit, mdxMacId, sdsId, RTL8295_SDS0_FIB_REG04_PAGE, RTL8295_SDS0_FIB_REG04_REG, phyData4);

    /* Force re-autonegotiation if AN is on*/
    if (ENABLED == enable)
    {
        /* PHY_CONTROL_REG */
        PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_FIB_REG00_PAGE, RTL8295_SDS0_FIB_REG00_REG, &phyData0);

        phyData0 = phyData0 & ~(RestartAutoNegotiation_MASK);
        phyData0 = phyData0 | (enable << RestartAutoNegotiation_OFFSET);

        PHY_8295_SDSREG_WRITE(unit, mdxMacId, sdsId, RTL8295_SDS0_FIB_REG00_PAGE, RTL8295_SDS0_FIB_REG00_REG, phyData0);
    }
    return RT_ERR_OK;
}
#endif


void
_phy_std_1000BaseXAnAdvertise_reg_to_struct(uint32 phyData, rtk_port_phy_ability_t *pAbility)
{
    /* 1000Base-X Auto-Negotiation Advertisement */
    pAbility->FC = (phyData & _1000BaseX_Pause_R4_MASK) >> _1000BaseX_Pause_R4_OFFSET;
    pAbility->AsyFC = (phyData & _1000BaseX_AsymmetricPause_R4_MASK) >> _1000BaseX_AsymmetricPause_R4_OFFSET;
    pAbility->Half_10 = 0;
    pAbility->Full_10 = 0;
    pAbility->Half_100 = 0;
    pAbility->Full_100 = 0;
    pAbility->Half_1000 = (phyData & _1000BaseX_HalfDuplex_R4_MASK) >> _1000BaseX_HalfDuplex_R4_OFFSET;
    pAbility->Full_1000 = (phyData & _1000BaseX_FullDuplex_R4_MASK) >> _1000BaseX_FullDuplex_R4_OFFSET;
}

void
_phy_std_speedSel_strcut_to_reg(uint32 speedSel, uint32 *pData)
{
    *pData = *pData & ~(SpeedSelection1_MASK | SpeedSelection0_MASK);
    *pData = *pData | (((speedSel & 2) << (SpeedSelection1_OFFSET - 1)) | ((speedSel & 1) << SpeedSelection0_OFFSET));
}

uint32
_phy_std_speedSel_reg_to_strcut(uint32 data)
{
    return (((data & SpeedSelection1_MASK) >> (SpeedSelection1_OFFSET -1)
            | ((data & SpeedSelection0_MASK) >> SpeedSelection0_OFFSET)));
}

#if defined(CONFIG_SDK_RTL8295R)
/* Function Name:
 *      phy_8295r_rxCali_process_sts_get
 * Description:
 *      return the RX Calibration Process Status
 * Input:
 *      unit    - unit ID
 *      port    - port ID
 * Output:
 *      95R RX Calibration prcoess status
 * Return:
 *      PHY_8295_RXCALI_PROCESS_STS_IDLE
 *      PHY_8295_RXCALI_PROCESS_STS_PROCESS
 * Note:
 *      None
 */
int32
phy_8295r_rxCali_process_sts_get(uint32 unit, uint32 port)
{
    int32 result;

    RXCALI_SEM_LOCK(unit, port);
    result = g8295rInfo[unit]->rxCali_process_sts[port];
    RXCALI_SEM_UNLOCK(unit, port);

    return result;
}

/* Function Name:
 *      phy_8295r_rxCali_process_sts_set
 * Description:
 *      set RX Calibration Process Status
 * Input:
 *      unit       - unit ID
 *      port       - port ID
 *      rxCali_sts - rxCali process status
 * Output:
 *      NA
 * Return:
 *      NA
 * Note:
 *      None
 */
int32
phy_8295r_rxCali_process_sts_set(uint32 unit, uint32 port, phy_8295_rxCali_process_sts_t rxCali_sts)
{

    RXCALI_SEM_LOCK(unit, port);
    g8295rInfo[unit]->rxCali_process_sts[port] = rxCali_sts;
    RXCALI_SEM_UNLOCK(unit, port);

    return RT_ERR_OK;
}
#endif

#if defined(CONFIG_SDK_RTL8295R)
/* Function Name:
 *      phy_8295r_swPollEnable_get
 * Description:
 *      return is SW poll
 * Input:
 *      unit    - unit ID
 *      port    - port ID
 * Output:
 *      None
 * Return:
 *      TRUE
 *      FALSE
 * Note:
 *      None
 */
int32
phy_8295r_swPollEnable_get(uint32 unit, uint32 port)
{
    return (int32)g8295rStatusSwPoll;
}
#endif

#if defined(CONFIG_SDK_RTL8295R)
/* Function Name:
 *      phy_8295r_resolutionReg_get
 * Description:
 *      Get resolution register
 * Input:
 *      unit    - unit ID
 *      port    - port ID
 * Output:
 *      pResl   - register data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8295r_resolutionReg_get(uint32 unit, uint32 port, uint32 *pResl)
{
    uint32          mdxMacId, d;
    int32           ret;
    drv_smi_mdxProtoSel_t       proto;

    *pResl = 0;
    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);

    if ((ret = hal_miim_portSmiMdxProto_get(unit, port, &proto)) != RT_ERR_OK)
    {
        /* default using C22 */
        proto = DRV_SMI_MDX_PROTO_C22;
    }

    if (proto == DRV_SMI_MDX_PROTO_C45)
    {
        ret = phy_common_reg_mmd_get(unit, mdxMacId, 31, 42036, &d);
    }
    else
    {
        ret = phy_common_reg_get(unit, mdxMacId, 0, 26, &d);
    }

    if (ret != RT_ERR_OK)
    {
        RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u get reg26 failed %x", unit, port, ret);
        return ret;
    }

    *pResl = d;
    return RT_ERR_OK;
}
#endif

#if defined(CONFIG_SDK_RTL8295R)
/* Function Name:
 *      phy_8295r_swMacPollPhyStatus_get
 * Description:
 *      Get PHY status
 * Input:
 *      unit    - unit ID
 *      port    - port ID
 * Output:
 *      pphyStatus  - PHY status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8295r_swMacPollPhyStatus_get(uint32 unit, uint32 port, rtk_port_swMacPollPhyStatus_t *pphyStatus)
{
    int32           ret;

    if(phy_8295r_rxCali_process_sts_get(unit, port) == PHY_8295_RXCALI_PROCESS_STS_IDLE)
    {
        pphyStatus->media = PORT_MEDIA_FIBER;
        if ((ret = phy_8295r_resolutionReg_get(unit, port, &pphyStatus->reslStatus)) != RT_ERR_OK)
        {
            return ret;
        }
    }
    else
    {
        RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u PHY_8295_RXCALI_PROCESS is processing!!!", unit, port);
        return RT_ERR_NOT_FINISH;
    }
    return RT_ERR_OK;
}
#endif


#if defined(CONFIG_SDK_RTL8295R)
/* Function Name:
 *      phy_8295r_resolutionResult2Speed
 * Description:
 *      Get speed from resolution register data
 * Input:
 *      resl    - register data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
rtk_port_speed_t
phy_8295r_resolutionResult2Speed(uint32 resl)
{
    uint32              val;
    rtk_port_speed_t    speed;

    /* [10,9,5,4] */
    val = ((resl & 0x600) >> 7) | ((resl & 0x30) >> 4);
    if (val == 0)
        speed = PORT_SPEED_10M;
    else if (val == 1)
        speed = PORT_SPEED_100M;
    else if (val == 2)
        speed = PORT_SPEED_1000M;
    else if (val == 3)
        speed = PORT_SPEED_500M;
    else if (val == 4)
        speed = PORT_SPEED_10G;
    else if (val == 5)
        speed = PORT_SPEED_2_5G;
    else if (val == 6)
        speed = PORT_SPEED_5G;
    else //unknwon
        speed = PORT_SPEED_END;

    return speed;

}
#endif

#if defined(CONFIG_SDK_RTL8295R)
/* Function Name:
 *      phy_8295r_10gMedia_get
 * Description:
 *      Get 10G port media of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pMedia  - pointer to the media type
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8295r_10gMedia_get(uint32 unit, rtk_port_t port, rtk_port_10gMedia_t *pMedia)
{
    *pMedia = PORT_10G_MEDIA(unit, port);
    return RT_ERR_OK;
}
#endif


#if defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8224qf_10gMedia_get
 * Description:
 *      Get port media of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pMedia  - pointer to the media type
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8224qf_10gMedia_get(uint32 unit, rtk_port_t port, rtk_port_10gMedia_t *pMedia)
{
    *pMedia = PORT_10G_MEDIA(unit, port);
    return RT_ERR_OK;
}
#endif


#if defined(CONFIG_SDK_RTL8295R)
/* Function Name:
 *      _phy_8295r_txParam_set
 * Description:
 *      Configure TX parameter
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      media   - 10G media
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
_phy_8295r_txParam_set(uint32 unit, rtk_port_t port, rtk_port_10gMedia_t media)
{
  return phy_8295r_patch_txParam_set(unit, port, media);
}

/* Function Name:
 *      _phy_8295r_10gPortVoltage_set
 * Description:
 *      Configure 10g Port voltage
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      media   - 10G media
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
_phy_8295r_10gPortVoltage_set(uint32 unit, uint32 port, uint32 mdxMacId, uint32 media)
{
    uint32      voltage = 0, data;

    switch (media)
    {
      case PORT_10GMEDIA_FIBER_10G:
      case PORT_10GMEDIA_DAC_50CM:
      case PORT_10GMEDIA_DAC_100CM:
      case PORT_10GMEDIA_DAC_300CM:
      case PORT_10GMEDIA_DAC_500CM:
        voltage = 0; //low
        break;
      case PORT_10GMEDIA_FIBER_1G:
      case PORT_10GMEDIA_NONE:
      default:
        voltage = 1;//high
        break;
    }/* end switch */

    PHY_8295_SDSREG_READ(unit, mdxMacId, 1, RTL8295_SDS0_ANA_SPD_10P3125G_EXT_REG29_PAGE, RTL8295_SDS0_ANA_SPD_10P3125G_EXT_REG29_REG, &data);
    data = REG32_FIELD_SET(data, voltage, 2, (0x1 << 2));
    PHY_8295_SDSREG_WRITE(unit, mdxMacId, 1, RTL8295_SDS0_ANA_SPD_10P3125G_EXT_REG29_PAGE, RTL8295_SDS0_ANA_SPD_10P3125G_EXT_REG29_REG, data);
    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,voltage set p=0x2F,r=0x1D:0x%X", unit, port, data);

    return RT_ERR_OK;
}

#if defined(CONFIG_SDK_RTL8295R)
/* Function Name:
 *      _phy_8295r_speedStdReg_set
 * Description:
 *      Configure standard register
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      speed   - speed (rtk_port_speed_t)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
_phy_8295r_speedStdReg_set(uint32 unit, rtk_port_t port, uint32 speed)
{
    uint32          speedSel;
    uint32          data;
    uint32          mdxMacId, sdsId;

    /* force speed */
    switch (speed)
    {
      case PORT_SPEED_10G:
        return RT_ERR_OK;
      case PORT_SPEED_1000M:
        speedSel = 0x2;
        break;
      case PORT_SPEED_100M:
        speedSel = 0x1;
        break;
      default:
        return RT_ERR_INPUT;
    }/* switch */

    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,sel=%u", unit, port, speedSel);

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);

    PHY_8295_SDSID_GET(unit, port, &sdsId);

    /* PHY_CONTROL_REG */
    PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_FIB_REG00_PAGE, RTL8295_SDS0_FIB_REG00_REG, &data);

    _phy_std_speedSel_strcut_to_reg(speedSel, &data);

    PHY_8295_SDSREG_WRITE(unit, mdxMacId, sdsId, RTL8295_SDS0_FIB_REG00_PAGE, RTL8295_SDS0_FIB_REG00_REG, data);

    return RT_ERR_OK;

}
#endif

/* Function Name:
 *      _phy_8295r_10gMedia_set
 * Description:
 *      Start 10gMedia activate process
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      media - port media
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
_phy_8295r_10gMedia_set(uint32 unit, rtk_port_t port, rtk_port_10gMedia_t media)
{
    int32                   ret;
    uint32                  sdsMode, preSdsMode;
    uint32                  mdxMacId;
    uint32                  preMacMedia = PORT_10GMEDIA_NONE, macMedia = PORT_10GMEDIA_NONE;
    rtk_serdesMode_t        preMacSdsMode = RTK_MII_DISABLE, macSdsMode = RTK_MII_DISABLE;

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);


    if ((ret = _phy_8295_serdesMode_get(unit, port, &preSdsMode)) != RT_ERR_OK)
        return ret;

    _switch_port_macForceLinkDown_enable(unit, port, ENABLED);
    switch (media)
    {
      case PORT_10GMEDIA_FIBER_10G:
      case PORT_10GMEDIA_DAC_50CM:
      case PORT_10GMEDIA_DAC_100CM:
        sdsMode = PHY_8295_SDS_MODE_10GR;
        break;
      case PORT_10GMEDIA_DAC_300CM:
      case PORT_10GMEDIA_DAC_500CM://DAC_LONG
        sdsMode = PHY_8295_SDS_MODE_10GR;
        break;
      case PORT_10GMEDIA_FIBER_1G:
        sdsMode = PHY_8295_SDS_MODE_FIB1G;
        _switch_port_macForceLinkDown_enable(unit, port, DISABLED);
        break;
      case PORT_10GMEDIA_NONE:
        sdsMode = PHY_8295_SDS_MODE_OFF;
        break;
      default:
        return RT_ERR_INPUT;
    }/* end switch */
    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,media=%d,sdsMode=0x%x", unit, port, media, sdsMode);

    PORT_SDS_MODE_CFG(unit, port) = sdsMode;

    RTK_PORTMASK_PORT_CLEAR(g8295rInfo[unit]->port10gReady, port);
    g8295rInfo[unit]->rxCaliStatus[port][1] = PHY_SDS_RXCALI_STATUS_NOINFO;

    /* disable serdes first */
    _phy_8295_portSdsMode_set(unit, port, PHY_8295_SDS_MODE_OFF);
    /* initial RX registers to default */
    phy_8295_rxCaliRegDefault_init(unit, port, 1);

    if (sdsMode != PHY_8295_SDS_MODE_OFF)
    {
        if ((ret = _phy_8295_portSdsMode_set(unit, port, sdsMode)) != RT_ERR_OK)
        {
            return ret;
        }
    }

    PORT_10G_MEDIA(unit, port) = media;

    /* disable MAC media */
    if (g8295rMacAutoDetectSdsMode == FALSE)
    {
        if (HWP_8390_FAMILY_ID(unit))
        {
            _switch_port_10gMedia_convert(unit, port, media, &macMedia);
            _switch_port_10gMedia_get(unit, port, &preMacMedia);
            if (preMacMedia != macMedia)
            {
                _switch_port_10gMedia_set(unit, port, PORT_10GMEDIA_NONE);
            }
        }
    }

    /* configure S0 SerDes mode */
    if (g8295rS0SdsAutoChgEn == FALSE)
    {
        uint32          sdsModeS0;

        switch (media)
        {
          case PORT_10GMEDIA_FIBER_1G:
            sdsModeS0 = PHY_8295_SDS_MODE_FIB1G;
            break;
          default:
            sdsModeS0 = PHY_8295_SDS_MODE_10GR;
            break;
        }

        _phy_8295_serdesModeReg_set(unit, port, mdxMacId, 0, sdsModeS0);
    }

    if (sdsMode != PHY_8295_SDS_MODE_OFF)
    {
        _phy_8295r_serdes_reset(unit, port);
    }


    /* config MAC media */
    if (g8295rMacAutoDetectSdsMode == FALSE)
    {
        if (HWP_8390_FAMILY_ID(unit))
        {
            RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,macMedia prev %u cur %u", unit, port, preMacMedia, macMedia);
            if (preMacMedia != macMedia)
            {
                _switch_port_10gMedia_set(unit, port, macMedia);
            }
        }
        else
        {
            _switch_sdsMode_convert(unit, port, media, &macSdsMode);
            _switch_sdsMode_get(unit, port, &preMacSdsMode);
            RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,sdsMode prev %u cur %u", unit, port, preMacSdsMode, macSdsMode);
            if (preMacSdsMode != macSdsMode)
            {
                _switch_sdsMode_set(unit, port, macSdsMode);
            }
        }

    }

    return RT_ERR_OK;
}


/* Function Name:
 *      phy_8295r_10gMedia_set
 * Description:
 *      Set 10G port media of the specific port
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      media - port media
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8295r_10gMedia_set(uint32 unit, rtk_port_t port, rtk_port_10gMedia_t media)
{
    uint32                  mdxMacId;
    uint32                  speed;

    if (!g8295rInfo[unit])
    {
        return RT_ERR_NOT_INIT;
    }

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);

    _phy_8295r_10gPortVoltage_set(unit, port, mdxMacId, media);

    _phy_8295r_txParam_set(unit, port, media);

    if (media == PORT_10GMEDIA_FIBER_1G)
        speed = PORT_SPEED_1000M;
    else
        speed = PORT_SPEED_10G;
    _phy_8295r_speedStdReg_set(unit, port, speed);

    if (PORT_ENABLE_STATE(unit, port) == DISABLED)
    {
        PORT_10G_MEDIA(unit, port) = media;
        return RT_ERR_OK;
    }

    return _phy_8295r_10gMedia_set(unit, port, media);
}
#endif


#if defined(CONFIG_SDK_RTL8224QF)
int32
_phy_8224qf_fiberMediaEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable, rtk_port_10gMedia_t media)
{
    int32   ret;
    uint32  mode;
    uint32  mdxMacId, sdsId, page, reg, data;

    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,enable=%u,media=%d", unit, port, enable, media);

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);
    PHY_8295_SDSID_GET(unit, port, &sdsId);
    switch(sdsId)
    {
      case 4:
        page = RTL8295_SDS_CTRL_SDS_CTRL_S4_PAGE;
        reg  = RTL8295_SDS_CTRL_SDS_CTRL_S4_REG;
        break;
      case 5:
        page = RTL8295_SDS_CTRL_SDS_CTRL_S5_PAGE;
        reg  = RTL8295_SDS_CTRL_SDS_CTRL_S5_REG;
        break;
      case 6:
        page = RTL8295_SDS_CTRL_SDS_CTRL_S6_PAGE;
        reg  = RTL8295_SDS_CTRL_SDS_CTRL_S6_REG;
        break;
      case 7:
        page = RTL8295_SDS_CTRL_SDS_CTRL_S7_PAGE;
        reg  = RTL8295_SDS_CTRL_SDS_CTRL_S7_REG;
        break;
      default:
        RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,sdsId=%u error port id", unit, port, sdsId);
        return RT_ERR_PORT_ID;
    }


    if (enable == ENABLED && media != PORT_10GMEDIA_NONE)
    {
        switch (media)
        {
          case PORT_10GMEDIA_FIBER_2_5G:
            mode = PHY_8295_SDS_MODE_FIB2_5G;
            break;
          case PORT_10GMEDIA_FIBER_1G:
            mode = PHY_8295_SDS_MODE_FIB1G;
            break;
          default:
            ret = RT_ERR_INPUT;
            RT_ERR(ret, (MOD_HAL|MOD_PORT), "unit %u port %u fiber-media %u not support", unit, port, media);
            return ret;
        }

        if ((ret = phy_common_enable_set(unit, port, enable)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_HAL|MOD_PORT), "unit %u port %u", unit, port);
            return ret;
        }

        _phy_8295_portSdsMode_set(unit, port, PHY_8295_SDS_MODE_OFF);
        osal_time_mdelay(10);
        if ((ret = _phy_8295_portSdsMode_set(unit, port, mode)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_HAL|MOD_PORT), "unit %u port %u", unit, port);
            return ret;
        }
        _phy_8214qf_serdes_reset(unit, port);

        PHY_8295_PHYREG_READ(unit, mdxMacId, page, reg, &data);
        data = REG32_FIELD_SET(data, 0, RTL8295_SDS_CTRL_SDS_CTRL_S4_SDS_FRC_LD_S4_OFFSET, RTL8295_SDS_CTRL_SDS_CTRL_S4_SDS_FRC_LD_S4_MASK);
        PHY_8295_PHYREG_WRITE(unit, mdxMacId, page, reg, data);
    }
    else
    {
        if ((ret = _phy_8295_portSdsMode_set(unit, port, PHY_8295_SDS_MODE_OFF)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_HAL|MOD_PORT), "unit %u port %u", unit, port);
            return ret;
        }

        PHY_8295_PHYREG_READ(unit, mdxMacId, page, reg, &data);
        data = REG32_FIELD_SET(data, 1, RTL8295_SDS_CTRL_SDS_CTRL_S4_SDS_FRC_LD_S4_OFFSET, RTL8295_SDS_CTRL_SDS_CTRL_S4_SDS_FRC_LD_S4_MASK);
        PHY_8295_PHYREG_WRITE(unit, mdxMacId, page, reg, data);

        if ((ret = phy_common_enable_set(unit, port, enable)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_HAL|MOD_PORT), "unit %u port %u", unit, port);
            return ret;
        }
    }

    return ret;
}
#endif

#if defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8224qf_10gMedia_set
 * Description:
 *      Set port media of the specific port
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      media - port media
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8224qf_10gMedia_set(uint32 unit, rtk_port_t port, rtk_port_10gMedia_t media)
{
    int32                   ret;

    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,media=%d", unit, port, media);

    if (!g8295rInfo[unit])
    {
        return RT_ERR_NOT_INIT;
    }

    switch (media)
    {
      case PORT_10GMEDIA_FIBER_2_5G:
      case PORT_10GMEDIA_FIBER_1G:
      case PORT_10GMEDIA_NONE:
        break;
      default:
        ret = RT_ERR_INPUT;
        RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit %u port %u fiber-media %u not support", unit, port, media);
        return ret;
    }

    if ((ret = _phy_8224qf_fiberMediaEnable_set(unit, port, PORT_ENABLE_STATE(unit, port), media)) == RT_ERR_OK)
    {
        PORT_10G_MEDIA(unit, port) = media;
    }

    return ret;
}
#endif

#if defined(CONFIG_SDK_RTL8295R)
/* Function Name:
 *      phy_8295r_autoNegoEnable_get
 * Description:
 *      Get autonegotiation enable status of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8295r_autoNegoEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    uint32          port_flag;
    uint32          mdxMacId, sdsId;
    uint32          phyData0;

    _phy_8295_10GePortType_get(unit, port, &port_flag);

    if (port_flag & PHY_8295_PORT_10G)
    {
        /* 10G not support Auto-Nego */
        *pEnable = DISABLED;
        return RT_ERR_OK;
    }

    if (port_flag & PHY_8295_PORT_1G)
    {
        PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);

        PHY_8295_SDSID_GET(unit, port, &sdsId);

         /* PHY_CONTROL_REG */
        PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_FIB_REG00_PAGE, RTL8295_SDS0_FIB_REG00_REG, &phyData0);

        if (phyData0 & AutoNegotiationEnable_MASK)
        {
            *pEnable = ENABLED;
        }
        else
        {
            *pEnable = DISABLED;
        }

        return RT_ERR_OK;
    }

    return RT_ERR_FAILED;
}
#endif


#if defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8214qf_autoNegoEnable_get
 * Description:
 *      Get autonegotiation enable status of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8214qf_autoNegoEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    return phy_common_autoNegoEnable_get(unit, port, pEnable);
}
#endif

#if defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8224qf_autoNegoEnable_get
 * Description:
 *      Get autonegotiation enable status of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8224qf_autoNegoEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    uint32 speed;
    int32 ret;

    if ((ret = phy_8224qf_speed_get(unit, port, &speed)) != RT_ERR_OK)
        return ret;

    if (PORT_SPEED_2_5G == speed)
    {
        *pEnable = DISABLED;
        return RT_ERR_OK;
    }

    return phy_common_autoNegoEnable_get(unit, port, pEnable);
}
#endif

#if defined(CONFIG_SDK_RTL8295R)
/* Function Name:
 *      phy_8295r_autoNegoEnable_set
 * Description:
 *      Set autonegotiation enable status of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8295r_autoNegoEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    uint32          port_flag;
    uint32          mdxMacId, sdsId;
    uint32          phyData0;

    _phy_8295_10GePortType_get(unit, port, &port_flag);

    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,EN=%d,flag=%x", unit, port, enable, port_flag);

    if (port_flag & PHY_8295_PORT_10G)
    {
        if (enable == DISABLED)
        {
            return RT_ERR_OK;
        }
        else
        {
            /* 10G not support Auto-Nego */
            return RT_ERR_PORT_NOT_SUPPORTED;
        }
    }

    if (port_flag & PHY_8295_PORT_1G)
    {
        PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);

        PHY_8295_SDSID_GET(unit, port, &sdsId);

        /* PHY_CONTROL_REG */
        PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_FIB_REG00_PAGE, RTL8295_SDS0_FIB_REG00_REG, &phyData0);

        phyData0 = phyData0 & ~(AutoNegotiationEnable_MASK | RestartAutoNegotiation_MASK);
        phyData0 = phyData0 | ((enable << AutoNegotiationEnable_OFFSET) | (1 << RestartAutoNegotiation_OFFSET));

        PHY_8295_SDSREG_WRITE(unit, mdxMacId, sdsId, RTL8295_SDS0_FIB_REG00_PAGE, RTL8295_SDS0_FIB_REG00_REG, phyData0);

        return RT_ERR_OK;
    }

    return RT_ERR_FAILED;
}
#endif


#if defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8214qf_autoNegoEnable_set
 * Description:
 *      Set autonegotiation enable status of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8214qf_autoNegoEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,EN=%d", unit, port, enable);

    return phy_common_autoNegoEnable_set(unit, port, enable);
}
#endif


#if defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8224qf_autoNegoEnable_set
 * Description:
 *      Set autonegotiation enable status of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8224qf_autoNegoEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    uint32 speed;
    int32 ret;

    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,EN=%d", unit, port, enable);
    if ((ret = phy_8224qf_speed_get(unit, port, &speed)) != RT_ERR_OK)
    {
        RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,ret=%x", unit, port, ret);
        return ret;
    }

    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,speed=%x", unit, port, speed);
    if (PORT_SPEED_2_5G == speed)
    {
        if(ENABLED == enable)
        {
            return RT_ERR_PORT_NOT_SUPPORTED;
        }

        return RT_ERR_OK;
    }

    return phy_common_autoNegoEnable_set(unit, port, enable);
} /* end of phy_8224qf_autoNegoEnable_set */
#endif

#if defined(CONFIG_SDK_RTL8295R)
/* Function Name:
 *      phy_8295r_autoNegoAbility_get
 * Description:
 *      Get ability advertisement for auto negotiation of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pAbility - pointer to PHY auto negotiation ability
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8295r_autoNegoAbility_get(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility)
{
    uint32          port_flag;
    uint32          mdxMacId, sdsId;
    uint32          data, val;

     osal_memset(pAbility, 0, sizeof(rtk_port_phy_ability_t));

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);

    PHY_8295_SDSID_GET(unit, port, &sdsId);

    _phy_8295_10GePortType_get(unit, port, &port_flag);

    if (port_flag & PHY_8295_PORT_10G)
    {
        PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_WDIG_REG11_PAGE, RTL8295_SDS0_WDIG_REG11_REG, &data);
        val = REG32_FIELD_GET(data, RTL8295_SDS0_WDIG_REG11_RX_FCTRL_10G_OFFSET, RTL8295_SDS0_WDIG_REG11_RX_FCTRL_10G_MASK);
        if (val == 1)
            pAbility->FC = ENABLED;
        else
            pAbility->FC = DISABLED;
        return RT_ERR_OK;
    }

    if (port_flag & PHY_8295_PORT_1G)
    {
        uint32          phyData4;

        /* PHY_AN_ADVERTISEMENT_REG */
        PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_FIB_REG04_PAGE, RTL8295_SDS0_FIB_REG04_REG, &phyData4);

        _phy_std_1000BaseXAnAdvertise_reg_to_struct(phyData4, pAbility);
        return RT_ERR_OK;
    }


    return RT_ERR_FAILED;
}
#endif


#if defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8214qf_autoNegoAbility_get
 * Description:
 *      Get ability advertisement for auto negotiation of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pAbility - pointer to PHY auto negotiation ability
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8214qf_autoNegoAbility_get(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility)
{
    int32           ret;
    uint32          phyData4;

    osal_memset(pAbility, 0, sizeof(rtk_port_phy_ability_t));

    /* get value from CHIP*/
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_AN_ADVERTISEMENT_REG, &phyData4)) != RT_ERR_OK)
        return ret;

    _phy_std_1000BaseXAnAdvertise_reg_to_struct(phyData4, pAbility);

    return RT_ERR_OK;

}
#endif


#if defined(CONFIG_SDK_RTL8295R)
/* Function Name:
 *      phy_8295r_autoNegoAbility_set
 * Description:
 *      Set ability advertisement for auto negotiation of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 *      pAbility  - auto negotiation ability that is going to set to PHY
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8295r_autoNegoAbility_set(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility)
{
    uint32          port_flag;
    uint32          mdxMacId, sdsId;
    uint32          val, data;

    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u 1000F:%u 100F:%u 10F:%u FC:%u", unit, port, pAbility->Full_1000, pAbility->Full_100, pAbility->Full_10, pAbility->FC);

    _phy_8295_10GePortType_get(unit, port, &port_flag);

    if (port_flag & PHY_8295_PORT_10G)
    {
        if (pAbility->FC == ENABLED)
            val = 1;
        else
            val = 0;
        PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);
        PHY_8295_SDSID_GET(unit, port, &sdsId);
        /* force flow-conftrol */
        PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_WDIG_REG11_PAGE, RTL8295_SDS0_WDIG_REG11_REG, &data);
        data = REG32_FIELD_SET(data, val, RTL8295_SDS0_WDIG_REG11_RX_FCTRL_10G_OFFSET, RTL8295_SDS0_WDIG_REG11_RX_FCTRL_10G_MASK);
        data = REG32_FIELD_SET(data, val, RTL8295_SDS0_WDIG_REG11_TX_FCTRL_10G_OFFSET, RTL8295_SDS0_WDIG_REG11_TX_FCTRL_10G_MASK);
        PHY_8295_SDSREG_WRITE(unit, mdxMacId, sdsId, RTL8295_SDS0_WDIG_REG11_PAGE, RTL8295_SDS0_WDIG_REG11_REG, data);
        return RT_ERR_OK;
    }

    if (port_flag & PHY_8295_PORT_1G)
    {
        return _phy_8295r_autoNegoAbilityStaFiberStdReg_set(unit, port, pAbility);
    }

    return RT_ERR_PORT_NOT_SUPPORTED;
}
#endif


#if defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8214qf_autoNegoAbility_set
 * Description:
 *      Set ability advertisement for auto negotiation of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 *      pAbility  - auto negotiation ability that is going to set to PHY
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8214qf_autoNegoAbility_set(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility)
{
    int32           ret;
    uint32          phyData0;
    uint32          phyData4;
    rtk_enable_t    enable;

    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u 1000F:%u 100F:%u 10F:%u FC:%u", unit, port, pAbility->Full_1000, pAbility->Full_100, pAbility->Full_10, pAbility->FC);

    phy_common_autoNegoEnable_get(unit, port, &enable);

    /* get value from CHIP*/
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_AN_ADVERTISEMENT_REG, &phyData4)) != RT_ERR_OK)
        return ret;

    phyData4 = phyData4 & ~(_1000BaseX_Pause_R4_MASK | _1000BaseX_AsymmetricPause_R4_MASK);
    phyData4 = phyData4 | (pAbility->FC << _1000BaseX_Pause_R4_OFFSET)
            | (pAbility->AsyFC << _1000BaseX_AsymmetricPause_R4_OFFSET);
    phyData4 = phyData4 & ~(_1000BaseX_HalfDuplex_R4_MASK | _1000BaseX_FullDuplex_R4_MASK);
    phyData4 = phyData4 | (pAbility->Half_1000 << _1000BaseX_HalfDuplex_R4_OFFSET)
            | (pAbility->Full_1000 << _1000BaseX_FullDuplex_R4_OFFSET);


    if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_AN_ADVERTISEMENT_REG, phyData4)) != RT_ERR_OK)
        return ret;

    /* Force re-autonegotiation if AN is on*/
    if (ENABLED == enable)
    {
        if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData0)) != RT_ERR_OK)
            return ret;

        phyData0 = phyData0 & ~(RestartAutoNegotiation_MASK);
        phyData0 = phyData0 | (enable << RestartAutoNegotiation_OFFSET);

        if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, phyData0)) != RT_ERR_OK)
            return ret;
    }
    return ret;
}
#endif


#if defined(CONFIG_SDK_RTL8295R)
/* Function Name:
 *      phy_8295r_duplex_get
 * Description:
 *      Get duplex mode status of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pDuplex - pointer to PHY duplex mode status
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8295r_duplex_get(uint32 unit, rtk_port_t port, uint32 *pDuplex)
{
    uint32          port_flag;
    uint32          mdxMacId, sdsId;
    uint32          data;

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);

    PHY_8295_SDSID_GET(unit, port, &sdsId);

    _phy_8295_10GePortType_get(unit, port, &port_flag);

    if (port_flag & PHY_8295_PORT_10G)
    {
        *pDuplex = PORT_FULL_DUPLEX;
        return RT_ERR_OK;
    }

    if (port_flag & PHY_8295_PORT_1G)
    {
        /* PHY_CONTROL_REG */
        PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_FIB_REG00_PAGE, RTL8295_SDS0_FIB_REG00_REG, &data);

        *pDuplex = REG32_FIELD_GET(data, DuplexMode_OFFSET, DuplexMode_MASK);
        return RT_ERR_OK;
    }

    return RT_ERR_PORT_NOT_SUPPORTED;

}
#endif


#if defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8214qf_duplex_get
 * Description:
 *      Get duplex mode status of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pDuplex - pointer to PHY duplex mode status
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8214qf_duplex_get(uint32 unit, rtk_port_t port, uint32 *pDuplex)
{
    return phy_common_duplex_get(unit, port, pDuplex);
}
#endif


#if defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8224qf_duplex_get
 * Description:
 *      Get duplex mode status of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pDuplex - pointer to PHY duplex mode status
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8224qf_duplex_get(uint32 unit, rtk_port_t port, uint32 *pDuplex)
{
    uint32 speed;
    int32 ret;

    if ((ret = phy_8224qf_speed_get(unit, port, &speed)) != RT_ERR_OK)
        return ret;

    if (PORT_SPEED_2_5G == speed)
    {
        *pDuplex = PORT_FULL_DUPLEX;
        return RT_ERR_OK;
    }

    return phy_common_duplex_get(unit, port, pDuplex);
}
#endif


#if defined(CONFIG_SDK_RTL8295R)
/* Function Name:
 *      phy_8295r_duplex_set
 * Description:
 *      Set duplex mode status of the specific port
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      duplex        - duplex mode of the port, full or half
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8295r_duplex_set(uint32 unit, rtk_port_t port, uint32 duplex)
{
    uint32          port_flag;
    uint32          mdxMacId, sdsId;
    uint32          data;

    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,duplex=%u", unit, port, duplex);

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);

    PHY_8295_SDSID_GET(unit, port, &sdsId);

    _phy_8295_10GePortType_get(unit, port, &port_flag);

    if (port_flag & PHY_8295_PORT_10G)
    {
        if (PORT_FULL_DUPLEX != duplex)
            return RT_ERR_PORT_NOT_SUPPORTED;
        return RT_ERR_OK;
    }

    if (port_flag & PHY_8295_PORT_1G)
    {
        /* PHY_CONTROL_REG */
        PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_FIB_REG00_PAGE, RTL8295_SDS0_FIB_REG00_REG, &data);
        if (duplex)
            data = REG32_FIELD_SET(data, 1, DuplexMode_OFFSET, DuplexMode_MASK);
        else
            data = REG32_FIELD_SET(data, 0, DuplexMode_OFFSET, DuplexMode_MASK);

        PHY_8295_SDSREG_WRITE(unit, mdxMacId, sdsId, RTL8295_SDS0_FIB_REG00_PAGE, RTL8295_SDS0_FIB_REG00_REG, data);

        return RT_ERR_OK;
    }

    return RT_ERR_PORT_NOT_SUPPORTED;

}
#endif


#if defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8214qf_duplex_set
 * Description:
 *      Set duplex mode status of the specific port
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      duplex        - duplex mode of the port, full or half
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8214qf_duplex_set(uint32 unit, rtk_port_t port, uint32 duplex)
{
    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,duplex=%u", unit, port, duplex);
    return phy_common_duplex_set(unit, port, duplex);
}
#endif


#if defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8224qf_duplex_set
 * Description:
 *      Set duplex mode status of the specific port
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      duplex        - duplex mode of the port, full or half
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_INPUT  - invalid parameter
 * Note:
 *      None
 */
int32
phy_8224qf_duplex_set(uint32 unit, rtk_port_t port, uint32 duplex)
{
    uint32  speed;
    int32   ret;

    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,duplex=%u", unit, port, duplex);
    if (PORT_FULL_DUPLEX != duplex)
        return RT_ERR_INPUT;

    if ((ret = phy_8224qf_speed_get(unit, port, &speed)) != RT_ERR_OK)
        return ret;

    if (PORT_SPEED_2_5G == speed)
    {
        return RT_ERR_OK;
    }

    return phy_common_duplex_set(unit, port, duplex);
}
#endif


#if defined(CONFIG_SDK_RTL8295R)
/* Function Name:
 *      phy_8214qf_speed_get
 * Description:
 *      Get link speed status of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pSpeed - pointer to PHY link speed
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8295r_speed_get(uint32 unit, rtk_port_t port, rtk_port_speed_t *pSpeed)
{
    uint32          mdxMacId, data;


    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);

    /* get from status */
    PHY_8295_PHYREG_READ(unit, mdxMacId, RTL8295_SDS_CTRL_SDS_10G_LINK_PAGE, RTL8295_SDS_CTRL_SDS_10G_LINK_REG, &data);
    if (data & RTL8295_SDS_CTRL_SDS_10G_LINK_SDS_S1TG_LINK_MASK)
    {
        *pSpeed = PORT_SPEED_10G;
        return RT_ERR_OK;
    }

    PHY_8295_PHYREG_READ(unit, mdxMacId, RTL8295_SDS_CTRL_SDS_1G_LINK_PAGE, RTL8295_SDS_CTRL_SDS_1G_LINK_REG, &data);
    if (data & RTL8295_SDS_CTRL_SDS_1G_LINK_SDS_S1P0_LINK_MASK)
    {
        *pSpeed = PORT_SPEED_1000M;
        return RT_ERR_OK;
    }

    if (!g8295rInfo[unit])
    {
        return RT_ERR_FAILED;
    }

    switch (PORT_10G_MEDIA(unit, port))
    {
      case PORT_10GMEDIA_FIBER_1G:
        *pSpeed = PORT_SPEED_1000M;
        return RT_ERR_OK;
      case PORT_10GMEDIA_FIBER_10G:
      case PORT_10GMEDIA_DAC_50CM:
      case PORT_10GMEDIA_DAC_100CM:
      case PORT_10GMEDIA_DAC_300CM:
      case PORT_10GMEDIA_DAC_500CM: //DAC_LONG
      case PORT_10GMEDIA_NONE:
        *pSpeed = PORT_SPEED_10G;
        return RT_ERR_OK;
      default:
        return RT_ERR_FAILED;
    }
}
#endif


#if defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8214qf_speed_get
 * Description:
 *      Get link speed status of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pSpeed - pointer to PHY link speed
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8214qf_speed_get(uint32 unit, rtk_port_t port, rtk_port_speed_t *pSpeed)
{
    int32           ret;
    uint32          phyData0, speedSel;


    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData0)) != RT_ERR_OK)
        return ret;

    speedSel = _phy_std_speedSel_reg_to_strcut(phyData0);

    if (speedSel == 0x2)
    {
        *pSpeed = PORT_SPEED_1000M;
    }
    else if (speedSel == 0x1)
    {
        *pSpeed = PORT_SPEED_100M;
    }
    else if (speedSel == 0x0)
    {
        *pSpeed = PORT_SPEED_10M;
    }
    else
    {
        return RT_ERR_OUT_OF_RANGE;
    }

    return RT_ERR_OK;
}
#endif


#if defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8224qf_speed_get
 * Description:
 *      Get link speed status of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pSpeed - pointer to PHY link speed
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8224qf_speed_get(uint32 unit, rtk_port_t port, rtk_port_speed_t *pSpeed)
{
    /* function body */
    switch(PORT_10G_MEDIA(unit, port))
    {
      case PORT_10GMEDIA_FIBER_2_5G:
        *pSpeed = PORT_SPEED_2_5G;
        break;
      case PORT_10GMEDIA_FIBER_1G:
        *pSpeed = PORT_SPEED_1000M;
        break;
      default:
        *pSpeed = PORT_SPEED_2_5G;
        break;
    }

    return RT_ERR_OK;
}
#endif

#if defined(CONFIG_SDK_RTL8295R)
/* Function Name:
 *      phy_8295r_speed_set
 * Description:
 *      Set speed mode status of the specific port
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      speed         - link speed status 10/100/1000
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - invalid parameter
 * Note:
 *      None
 */
int32
phy_8295r_speed_set(uint32 unit, rtk_port_t port, rtk_port_speed_t speed)
{

    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,speed:%u", unit, port, speed);

    /* force speed */

    switch (PORT_10G_MEDIA(unit, port))
    {
      case PORT_10GMEDIA_FIBER_10G:
      case PORT_10GMEDIA_DAC_50CM:
      case PORT_10GMEDIA_DAC_100CM:
      case PORT_10GMEDIA_DAC_300CM:
      case PORT_10GMEDIA_DAC_500CM: //DAC_LONG
        switch (speed)
        {
          case PORT_SPEED_10G:
            return RT_ERR_OK;
          default:
            return RT_ERR_PORT_NOT_SUPPORTED;
        }
        break;
      case PORT_10GMEDIA_FIBER_1G:
        switch (speed)
        {
          case PORT_SPEED_1000M:
            return _phy_8295r_speedStdReg_set(unit, port, speed);
          default:
            return RT_ERR_PORT_NOT_SUPPORTED;
        }
        break;
      case PORT_10GMEDIA_NONE:
        /* just return OK. speed is meaning less if media does not selected yet. */
        return RT_ERR_OK;
      default:
        return RT_ERR_FAILED;
    }
}
#endif

#if defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8214qf_speed_set
 * Description:
 *      Set speed mode status of the specific port
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      speed         - link speed status 10/100/1000
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - invalid parameter
 *      RT_ERR_CHIP_NOT_SUPPORTED - copper media chip is not supported Force-1000
 * Note:
 *      None
 */
int32
phy_8214qf_speed_set(uint32 unit, rtk_port_t port, rtk_port_speed_t speed)
{
    int32           ret;
    uint32          speedSel;
    uint32          data;

    switch (speed)
    {
      case PORT_SPEED_1000M:
        speedSel = 0x2;
        break;
      case PORT_SPEED_100M:
        speedSel = 0x1;
        break;
      default:
        return RT_ERR_INPUT;
    }

    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,sel=%u", unit, port, speedSel);

    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &data)) != RT_ERR_OK)
        return ret;

    _phy_std_speedSel_strcut_to_reg(speedSel, &data);

    if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, data)) != RT_ERR_OK)
        return ret;

    return RT_ERR_OK;
}
#endif

#if defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8224qf_speed_set
 * Description:
 *      Set speed mode status of the specific port
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      speed         - link speed status 10/100/1000
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - invalid parameter
 *      RT_ERR_CHIP_NOT_SUPPORTED - copper media chip is not supported Force-1000
 * Note:
 *      None
 */
int32
phy_8224qf_speed_set(uint32 unit, rtk_port_t port, rtk_port_speed_t speed)
{
    uint32  speedSel, data;
    int32   ret;
    rtk_port_10gMedia_t media;

    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,speed=%u", unit, port, speed);

    /* to backward compatible to 1G fiber, also config the fiber-media */
    if (PORT_SPEED_1000M == speed)
        media = PORT_10GMEDIA_FIBER_1G;
    else if (PORT_SPEED_2_5G == speed)
        media = PORT_10GMEDIA_FIBER_2_5G;
    else
        return RT_ERR_FAILED;

    if ((ret = _phy_8224qf_fiberMediaEnable_set(unit, port, PORT_ENABLE_STATE(unit, port), media)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HAL|MOD_PORT), "unit %u port %u", unit, port);
        return ret;
    }
    PORT_10G_MEDIA(unit, port) = media;


    switch(PORT_10G_MEDIA(unit, port))
    {
      case PORT_10GMEDIA_FIBER_1G:
        if (PORT_SPEED_1000M == speed)
        {
            speedSel = 0x2;
            if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &data)) != RT_ERR_OK)
                return ret;
            _phy_std_speedSel_strcut_to_reg(speedSel, &data);
            if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, data)) != RT_ERR_OK)
                return ret;
        }
        else
        {
            ret =  RT_ERR_PORT_NOT_SUPPORTED;
        }
        break;
      case PORT_10GMEDIA_FIBER_2_5G:
        if (PORT_SPEED_2_5G == speed)
            ret =  RT_ERR_OK;
        else
            ret =  RT_ERR_PORT_NOT_SUPPORTED;
        break;
      case  PORT_10GMEDIA_NONE:
        ret = RT_ERR_OK;
        break;
      default:
        return RT_ERR_FAILED;
    }

    return ret;
}
#endif

#if defined(CONFIG_SDK_RTL8295R)
/* Function Name:
 *      phy_8295r_enable_set
 * Description:
 *      Set PHY interface status of the specific port
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      enable        - admin configuration of PHY interface
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8295r_enable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32           ret;
    uint32          mdxMacId;
    uint32          sdsId, data;

    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,EN=%d", unit, port, enable);

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);

    PHY_8295_SDSID_GET(unit, port, &sdsId);

    /* 10G reg */
    PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_TGR_STD_0_REG00_PAGE, RTL8295_SDS0_TGR_STD_0_REG00_REG, &data);
    if (enable == ENABLED)
        data = REG32_FIELD_SET(data, 0, PowerDown_OFFSET, PowerDown_MASK);
    else
        data = REG32_FIELD_SET(data, 1, PowerDown_OFFSET, PowerDown_MASK);

    PHY_8295_SDSREG_WRITE(unit, mdxMacId, sdsId, RTL8295_SDS0_TGR_STD_0_REG00_PAGE, RTL8295_SDS0_TGR_STD_0_REG00_REG, data);

    /* 1G reg */
    /* PHY_CONTROL_REG */
    PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_FIB_REG00_PAGE, RTL8295_SDS0_FIB_REG00_REG, &data);

    if (enable == ENABLED)
        data = REG32_FIELD_SET(data, 0, PowerDown_OFFSET, PowerDown_MASK);
    else
        data = REG32_FIELD_SET(data, 1, PowerDown_OFFSET, PowerDown_MASK);

    PHY_8295_SDSREG_WRITE(unit, mdxMacId, sdsId, RTL8295_SDS0_FIB_REG00_PAGE, RTL8295_SDS0_FIB_REG00_REG, data);

    if (g8295rInfo[unit])
    {
        if (enable)
        {
            RTK_PORTMASK_PORT_SET(g8295rInfo[unit]->portEnState, port);

            if ((ret = _phy_8295r_10gMedia_set(unit, port, PORT_10G_MEDIA(unit, port))) != RT_ERR_OK)
            {
                return ret;
            }
        }
        else
        {
            g8295rInfo[unit]->rxCaliStatus[port][1] = PHY_SDS_RXCALI_STATUS_NOINFO;
            RTK_PORTMASK_PORT_CLEAR(g8295rInfo[unit]->portEnState, port);
            _phy_8295_portSdsMode_set(unit, port, PHY_8295_SDS_MODE_OFF);
            _switch_port_macForceLinkDown_enable(unit, port, ENABLED);
        }
    }

    return RT_ERR_OK;
}
#endif

#if defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8214qf_enable_set
 * Description:
 *      Set PHY interface status of the specific port
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      enable        - admin configuration of PHY interface
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8214qf_enable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32       ret;
    uint32      mdxMacId, sdsId, page, reg, data;


    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,EN=%d", unit, port, enable);

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);
    PHY_8295_SDSID_GET(unit, port, &sdsId);
    switch(sdsId)
    {
      case 4:
        page = RTL8295_SDS_CTRL_SDS_CTRL_S4_PAGE;
        reg  = RTL8295_SDS_CTRL_SDS_CTRL_S4_REG;
        break;
      case 5:
        page = RTL8295_SDS_CTRL_SDS_CTRL_S5_PAGE;
        reg  = RTL8295_SDS_CTRL_SDS_CTRL_S5_REG;
        break;
      case 6:
        page = RTL8295_SDS_CTRL_SDS_CTRL_S6_PAGE;
        reg  = RTL8295_SDS_CTRL_SDS_CTRL_S6_REG;
        break;
      case 7:
        page = RTL8295_SDS_CTRL_SDS_CTRL_S7_PAGE;
        reg  = RTL8295_SDS_CTRL_SDS_CTRL_S7_REG;
        break;
      default:
        RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,sdsId=%u error port id", unit, port, sdsId);
        return RT_ERR_PORT_ID;
    }

    if (g8295rInfo[unit])
    {
        if (enable == DISABLED)
        {
            RTK_PORTMASK_PORT_CLEAR(g8295rInfo[unit]->portEnState, port);
        }
        else
        {
            RTK_PORTMASK_PORT_SET(g8295rInfo[unit]->portEnState, port);
        }
    }

    if (enable == DISABLED)
    {
        PHY_8295_PHYREG_READ(unit, mdxMacId, page, reg, &data);
        data = REG32_FIELD_SET(data, 1, RTL8295_SDS_CTRL_SDS_CTRL_S4_SDS_FRC_LD_S4_OFFSET, RTL8295_SDS_CTRL_SDS_CTRL_S4_SDS_FRC_LD_S4_MASK);
        PHY_8295_PHYREG_WRITE(unit, mdxMacId, page, reg, data);

        if ((ret = phy_common_enable_set(unit, port, enable)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_HAL|MOD_PORT), "unit %u port %u", unit, port);
            return ret;
        }
    }
    else if (enable == ENABLED)
    {
        if ((ret = phy_common_enable_set(unit, port, enable)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_HAL|MOD_PORT), "unit %u port %u", unit, port);
            return ret;
        }
        _phy_8214qf_serdes_reset(unit, port);

        PHY_8295_PHYREG_READ(unit, mdxMacId, page, reg, &data);
        data = REG32_FIELD_SET(data, 0, RTL8295_SDS_CTRL_SDS_CTRL_S4_SDS_FRC_LD_S4_OFFSET, RTL8295_SDS_CTRL_SDS_CTRL_S4_SDS_FRC_LD_S4_MASK);
        PHY_8295_PHYREG_WRITE(unit, mdxMacId, page, reg, data);
    }

    return RT_ERR_OK;
}
#endif


#if defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8224qf_enable_set
 * Description:
 *      Set PHY interface status of the specific port
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      enable        - admin configuration of PHY interface
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8224qf_enable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;

    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,EN=%d", unit, port, enable);

    if (!g8295rInfo[unit])
    {
        ret = RT_ERR_NOT_INIT;
        RT_ERR(ret, (MOD_HAL|MOD_PORT), "unit %u port %u", unit, port);
        return ret;
    }

    if ((ret = _phy_8224qf_fiberMediaEnable_set(unit, port, enable, PORT_10G_MEDIA(unit, port))) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HAL|MOD_PORT), "unit %u port %u", unit, port);
        return ret;
    }

    if (enable == DISABLED)
    {
        RTK_PORTMASK_PORT_CLEAR(g8295rInfo[unit]->portEnState, port);
    }
    else
    {
        RTK_PORTMASK_PORT_SET(g8295rInfo[unit]->portEnState, port);
    }

    return ret;
}
#endif

#if defined(CONFIG_SDK_RTL8295R) || defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8295_8214_fiberNwayForceLink_get
 * Description:
 *      When fiber port is configured N-way,
 *      which can link with link partner is configured force mode.
 * Input:
 *      unit  - unit id
 *      port  - port id
 * Output:
 *      pEnable - fiber Nway force links status
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8295_8214_fiberNwayForceLink_get(uint32 unit, rtk_port_t port,
    rtk_enable_t *pEnable)
{
    uint32      data;
    uint32      mdxMacId, sdsId;

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);

    PHY_8295_SDSID_GET(unit, port, &sdsId);

    PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_SDS_REG04_PAGE, RTL8295_SDS0_SDS_REG04_REG, &data);
    *pEnable = REG32_FIELD_GET(data, 0x2, (0x1 << 0x2));    /* SP_CFG_EN_LINK_FIB1G */
    return RT_ERR_OK;
}
#endif


#if defined(CONFIG_SDK_RTL8295R) || defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8295_8214_fiberNwayForceLink_set
 * Description:
 *      When fiber port is configured N-way,
 *      which can link with link partner is configured force mode.
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      enable - fiber Nway force links status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8295_8214_fiberNwayForceLink_set(uint32 unit, rtk_port_t port,
    rtk_enable_t enable)
{
    uint32      data;
    uint32      mdxMacId, sdsId;

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);

    PHY_8295_SDSID_GET(unit, port, &sdsId);

    PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_SDS_REG04_PAGE, RTL8295_SDS0_SDS_REG04_REG, &data);
    data = REG32_FIELD_SET(data, enable, 0x2, (0x1 << 0x2));    /* SP_CFG_EN_LINK_FIB1G */
    PHY_8295_SDSREG_WRITE(unit, mdxMacId, sdsId, RTL8295_SDS0_SDS_REG04_PAGE, RTL8295_SDS0_SDS_REG04_REG, data);
    return RT_ERR_OK;
}
#endif

#if !defined(__BOOTLOADER__)
#if defined(CONFIG_SDK_RTL8295R) || defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8295_8214_fiberUnidirEnable_set
 * Description:
 *      Set fiber unidirection enable status of the specific port
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - enable status of fiber unidirection
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8295_8214_fiberUnidirEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    uint32              mdxMacId, sdsId;
    uint32              data;

    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,EN=%d", unit, port, enable);

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);

    PHY_8295_SDSID_GET(unit, port, &sdsId);

    PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_FIB_REG00_PAGE, RTL8295_SDS0_FIB_REG00_REG, &data);
    if (enable)
    {
        data = REG32_FIELD_SET(data, 1, 0x5, (0x1 << 0x5));         /* FRCTX */
    }
    else
    {
        data = REG32_FIELD_SET(data, 0, 0x5, (0x1 << 0x5));
    }
    PHY_8295_SDSREG_WRITE(unit, mdxMacId, sdsId, RTL8295_SDS0_FIB_REG00_PAGE, RTL8295_SDS0_FIB_REG00_REG, data);

    return RT_ERR_OK;

}
#endif
#endif/* #if !defined(__BOOTLOADER__) */

#if !defined(__BOOTLOADER__)
#if defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8214qf_fiberUnidirEnable_set
 * Description:
 *      Set fiber unidirection enable status of the specific port
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - enable status of fiber unidirection
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8214qf_fiberUnidirEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{

    uint32  mdxMacId, sdsId, bondModeSel;
    uint32  page, reg, data;

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);
    PHY_8295_SDSID_GET(unit, port, &sdsId);

    PHY_8295_PHYREG_READ(unit, mdxMacId, RTL8295_GLB_CTRL_BOND_STR_REG1_PAGE, RTL8295_GLB_CTRL_BOND_STR_REG1_REG, &data);
    bondModeSel = REG32_FIELD_GET(data, RTL8295_GLB_CTRL_BOND_STR_REG1_BOND_MODE_SEL_OFFSET, RTL8295_GLB_CTRL_BOND_STR_REG1_BOND_MODE_SEL_MASK);
    if (bondModeSel == 0xB)
    {
        /* 14qf cascade slave set force link 1G on S0 P0~P3 */
        switch(sdsId)
        {
        case 4:
            page = RTL8295_SDS_CTRL_SDS_ABLTY_S0_P0_PAGE;
            reg  = RTL8295_SDS_CTRL_SDS_ABLTY_S0_P0_REG;
            break;
        case 5:
            page = RTL8295_SDS_CTRL_SDS_ABLTY_S0_P1_PAGE;
            reg  = RTL8295_SDS_CTRL_SDS_ABLTY_S0_P1_REG;
            break;
        case 6:
            page = RTL8295_SDS_CTRL_SDS_ABLTY_S0_P2_PAGE;
            reg  = RTL8295_SDS_CTRL_SDS_ABLTY_S0_P2_REG;
            break;
        case 7:
            page = RTL8295_SDS_CTRL_SDS_ABLTY_S0_P3_PAGE;
            reg  = RTL8295_SDS_CTRL_SDS_ABLTY_S0_P3_REG;
            break;
        default:
            RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,sdsId=%u error port id", unit, port, sdsId);
            return RT_ERR_PORT_ID;
        }

        PHY_8295_PHYREG_READ(unit, mdxMacId, page, reg, &data);
        if (enable == ENABLED)
        {
            /* force mode enable */
            data = REG32_FIELD_SET(data, 1, RTL8295_SDS_CTRL_SDS_ABLTY_S0_P0_ABLTY_SRC_S0_P0_OFFSET, RTL8295_SDS_CTRL_SDS_ABLTY_S0_P0_ABLTY_SRC_S0_P0_MASK);
            /* force link up */
            data = REG32_FIELD_SET(data, 1, RTL8295_SDS_CTRL_SDS_ABLTY_S0_P0_PHY_LINK_S0_P0_OFFSET, RTL8295_SDS_CTRL_SDS_ABLTY_S0_P0_PHY_LINK_S0_P0_MASK);
            /* force full duplex */
            data = REG32_FIELD_SET(data, 1, RTL8295_SDS_CTRL_SDS_ABLTY_S0_P0_DUPLEX_S0_P0_OFFSET, RTL8295_SDS_CTRL_SDS_ABLTY_S0_P0_DUPLEX_S0_P0_MASK);
            /* force speed at 1000M */
            data = REG32_FIELD_SET(data, 0, RTL8295_SDS_CTRL_SDS_ABLTY_S0_P0_PHY_SPD_B3_S0_P0_OFFSET, RTL8295_SDS_CTRL_SDS_ABLTY_S0_P0_PHY_SPD_B3_S0_P0_MASK);
            data = REG32_FIELD_SET(data, 2, RTL8295_SDS_CTRL_SDS_ABLTY_S0_P0_PHY_SPD_S0_P0_OFFSET, RTL8295_SDS_CTRL_SDS_ABLTY_S0_P0_PHY_SPD_S0_P0_MASK);
        }
        else
        {
            /* undo force mode and clear link, duplex, speed */
            data = REG32_FIELD_SET(data, 0, RTL8295_SDS_CTRL_SDS_ABLTY_S0_P0_ABLTY_SRC_S0_P0_OFFSET, RTL8295_SDS_CTRL_SDS_ABLTY_S0_P0_ABLTY_SRC_S0_P0_MASK);
            data = REG32_FIELD_SET(data, 0, RTL8295_SDS_CTRL_SDS_ABLTY_S0_P0_PHY_LINK_S0_P0_OFFSET, RTL8295_SDS_CTRL_SDS_ABLTY_S0_P0_PHY_LINK_S0_P0_MASK);
            data = REG32_FIELD_SET(data, 0, RTL8295_SDS_CTRL_SDS_ABLTY_S0_P0_DUPLEX_S0_P0_OFFSET, RTL8295_SDS_CTRL_SDS_ABLTY_S0_P0_DUPLEX_S0_P0_MASK);
            data = REG32_FIELD_SET(data, 0, RTL8295_SDS_CTRL_SDS_ABLTY_S0_P0_PHY_SPD_B3_S0_P0_OFFSET, RTL8295_SDS_CTRL_SDS_ABLTY_S0_P0_PHY_SPD_B3_S0_P0_MASK);
            data = REG32_FIELD_SET(data, 0, RTL8295_SDS_CTRL_SDS_ABLTY_S0_P0_PHY_SPD_S0_P0_OFFSET, RTL8295_SDS_CTRL_SDS_ABLTY_S0_P0_PHY_SPD_S0_P0_MASK);
        }

        RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,bondModeSel=%u,extCtlpage=%u extCtlreg=%u en=%u data=0x%X\n", unit, port, bondModeSel, page, reg, enable, data);
        PHY_8295_PHYREG_WRITE(unit, mdxMacId, page, reg, data);
    }

    return phy_8295_8214_fiberUnidirEnable_set(unit, port, enable);
}
#endif/* end #if defined(CONFIG_SDK_RTL8214QF) */
#endif/* #if !defined(__BOOTLOADER__) */

#if !defined(__BOOTLOADER__)
#if defined(CONFIG_SDK_RTL8295R)
/* Function Name:
 *      phy_8295r_fiberUnidirEnable_set
 * Description:
 *      Set fiber unidirection enable status of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      enable  - enable status of fiber unidirection
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8295r_fiberUnidirEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    uint32      ret;
    uint32      data, val, mdxMacId;

    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,EN=%d", unit, port, enable);

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);


    /* uni-direction for 1G */
    if ((ret = phy_8295_8214_fiberUnidirEnable_set(unit, port, enable)) != RT_ERR_OK)
    {
        return ret;
    }

    /* uni-direction for 10G */
    PHY_8295_PHYREG_READ(unit, mdxMacId, RTL8295_EXT_CTRL_UNI_DIR_10G_PAGE, RTL8295_EXT_CTRL_UNI_DIR_10G_REG, &data);
    val = (enable == ENABLED) ? 1 : 0;
    data = REG32_FIELD_SET(data, val, RTL8295_EXT_CTRL_UNI_DIR_10G_UNI_DIR_S1_OFFSET, RTL8295_EXT_CTRL_UNI_DIR_10G_UNI_DIR_S1_MASK);
    PHY_8295_PHYREG_WRITE(unit, mdxMacId, RTL8295_EXT_CTRL_UNI_DIR_10G_PAGE, RTL8295_EXT_CTRL_UNI_DIR_10G_REG, data);

    return RT_ERR_OK;
}
#endif
#endif/* #if !defined(__BOOTLOADER__) */

#if !defined(__BOOTLOADER__)
#if defined(CONFIG_SDK_RTL8295R) || defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8295_8214_fiberOAMLoopBackEnable_set
 * Description:
 *      Set Fiber-Port OAM Loopback feature,
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      enable - Fiber-Port OAM Loopback feature
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8295_8214_fiberOAMLoopBackEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,EN=%d", unit, port, enable);

    return phy_8295_8214_fiberUnidirEnable_set(unit, port, enable);

}
#endif
#endif /* #if !defined(__BOOTLOADER__) */

#if !defined(__BOOTLOADER__)
#if defined(CONFIG_SDK_RTL8295R)
/* Function Name:
 *      phy_8295r_fiberOAMLoopBackEnable_set
 * Description:
 *      Set Fiber-Port OAM Loopback feature,
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      enable - Fiber-Port OAM Loopback feature
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8295r_fiberOAMLoopBackEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{

    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,EN=%d", unit, port, enable);

    return phy_8295r_fiberUnidirEnable_set(unit, port, enable);
}
#endif
#endif/* #if !defined(__BOOTLOADER__) */

#if defined(CONFIG_SDK_RTL8295R)
/* Function Name:
 *      phy_8295r_speedStatus_get
 * Description:
 *      Get PHY operational link speed status from proprietary register
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pSpeed - pointer to PHY operational link speed
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
int32
phy_8295r_speedStatus_get(uint32 unit, rtk_port_t port, rtk_port_speed_t *pSpeed)
{
    rtk_port_duplex_t duplex;

    return phy_8295r_speedDuplexStatus_get(unit, port, pSpeed, &duplex);
}

/* Function Name:
 *      phy_8295r_speedDuplexStatus_get
 * Description:
 *      Get PHY operational link speed-duplex status from proprietary register
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pSpeed - pointer to PHY operational link speed
 *      pDuplex - pointer to PHY operational link duplex
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
int32
phy_8295r_speedDuplexStatus_get(uint32 unit, rtk_port_t port, rtk_port_speed_t *pSpeed, rtk_port_duplex_t *pDuplex)
{
    uint32          mdxMacId;
    int32           ret;
    drv_smi_mdxProtoSel_t       proto;

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);

    if ((ret = hal_miim_portSmiMdxProto_get(unit, port, &proto)) != RT_ERR_OK)
    {
        /* default using C22 */
        proto = DRV_SMI_MDX_PROTO_C22;
    }

    if (proto == DRV_SMI_MDX_PROTO_C45)
    {
        ret = phy_common_c45_speedDuplexStatusResReg_get(unit, port, pSpeed, pDuplex);
    }
    else
    {
        ret = phy_common_speedDuplexStatusResReg_get(unit, port, pSpeed, pDuplex);
    }

    return RT_ERR_OK;
}
#endif /* CONFIG_SDK_RTL8295R */

#if defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8214qf_speedDuplexStatus_get
 * Description:
 *      Get PHY operational link speed-duplex status from proprietary register
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pSpeed - pointer to PHY operational link speed
 *      pDuplex - pointer to PHY operational link duplex
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
int32
phy_8214qf_speedDuplexStatus_get(uint32 unit, rtk_port_t port, rtk_port_speed_t *pSpeed, rtk_port_duplex_t *pDuplex)
{
    uint32 phyData;

    /* clear latched status */
    hal_miim_read(unit, port, PHY_PAGE_0, 26, &phyData);

    return phy_common_speedDuplexStatusResReg_get(unit, port, pSpeed, pDuplex);
}
#endif

#if defined(CONFIG_SDK_RTL8295R) || defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8295_8214_ptpSwitchMacAddr_get
 * Description:
 *      Get the Switch MAC address setting of PHY of the specified port.
 * Input:
 *      unit           - unit id
 *      port           - port id
 *      pSwitchMacAddr - point to the Switch MAC Address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
phy_8295_8214_ptpSwitchMacAddr_get(uint32 unit, rtk_port_t port, rtk_mac_t *pSwitchMacAddr)
{
    uint32      mac_h, mac_m, mac_l;
    uint32      mdxMacId;


    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u", unit, port);

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);

    PHY_8295_PHYREG_READ(unit, mdxMacId, RTL8295_PTP_CTRL_PTP_TIME_REG05_PAGE, RTL8295_PTP_CTRL_PTP_TIME_REG05_REG, &mac_l);
    mac_l = REG32_FIELD_GET(mac_l,    RTL8295_PTP_CTRL_PTP_TIME_REG05_MAC_ADDR_L_OFFSET, RTL8295_PTP_CTRL_PTP_TIME_REG05_MAC_ADDR_L_MASK);

    PHY_8295_PHYREG_READ(unit, mdxMacId, RTL8295_PTP_CTRL_PTP_TIME_REG06_PAGE, RTL8295_PTP_CTRL_PTP_TIME_REG06_REG, &mac_m);
    mac_m = REG32_FIELD_GET(mac_m,    RTL8295_PTP_CTRL_PTP_TIME_REG06_MAC_ADDR_M_OFFSET, RTL8295_PTP_CTRL_PTP_TIME_REG06_MAC_ADDR_M_MASK);

    PHY_8295_PHYREG_READ(unit, mdxMacId, RTL8295_PTP_CTRL_PTP_TIME_REG07_PAGE, RTL8295_PTP_CTRL_PTP_TIME_REG07_REG, &mac_h);
    mac_h = REG32_FIELD_GET(mac_h,    RTL8295_PTP_CTRL_PTP_TIME_REG07_MAC_ADDR_H_OFFSET, RTL8295_PTP_CTRL_PTP_TIME_REG07_MAC_ADDR_H_MASK);


    pSwitchMacAddr->octet[0] = (uint8)(mac_h >> 8);
    pSwitchMacAddr->octet[1] = (uint8)(mac_h & 0xFF);
    pSwitchMacAddr->octet[2] = (uint8)(mac_m >> 8);
    pSwitchMacAddr->octet[3] = (uint8)(mac_m & 0xFF);
    pSwitchMacAddr->octet[4] = (uint8)(mac_l >> 8);
    pSwitchMacAddr->octet[5] = (uint8)(mac_l & 0xFF);

    return RT_ERR_OK;
}
#endif


#if defined(CONFIG_SDK_RTL8295R) || defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8295_8214_ptpSwitchMacAddr_set
 * Description:
 *      Set the Switch MAC address setting of PHY of the specified port.
 * Input:
 *      unit           - unit id
 *      port           - port id
 *      pSwitchMacAddr - point to the Switch MAC Address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
phy_8295_8214_ptpSwitchMacAddr_set(uint32 unit, rtk_port_t port, rtk_mac_t *pSwitchMacAddr)
{
    uint32      mac_h, mac_m, mac_l;
    uint32      mdxMacId;

    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u", unit, port);

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);

    mac_h = REG32_FIELD_SET(0, ((pSwitchMacAddr->octet[0] << 8) | (pSwitchMacAddr->octet[1])),
                            RTL8295_PTP_CTRL_PTP_TIME_REG07_MAC_ADDR_H_OFFSET, RTL8295_PTP_CTRL_PTP_TIME_REG07_MAC_ADDR_H_MASK);
    mac_m = REG32_FIELD_SET(0, ((pSwitchMacAddr->octet[2] << 8) | (pSwitchMacAddr->octet[3])),
                            RTL8295_PTP_CTRL_PTP_TIME_REG06_MAC_ADDR_M_OFFSET, RTL8295_PTP_CTRL_PTP_TIME_REG06_MAC_ADDR_M_MASK);
    mac_l = REG32_FIELD_SET(0, ((pSwitchMacAddr->octet[4] << 8) | (pSwitchMacAddr->octet[5])),
                            RTL8295_PTP_CTRL_PTP_TIME_REG05_MAC_ADDR_L_OFFSET, RTL8295_PTP_CTRL_PTP_TIME_REG05_MAC_ADDR_L_MASK);

    PHY_8295_PHYREG_WRITE(unit, mdxMacId, RTL8295_PTP_CTRL_PTP_TIME_REG07_PAGE, RTL8295_PTP_CTRL_PTP_TIME_REG07_REG, mac_h);
    PHY_8295_PHYREG_WRITE(unit, mdxMacId, RTL8295_PTP_CTRL_PTP_TIME_REG06_PAGE, RTL8295_PTP_CTRL_PTP_TIME_REG06_REG, mac_m);
    PHY_8295_PHYREG_WRITE(unit, mdxMacId, RTL8295_PTP_CTRL_PTP_TIME_REG05_PAGE, RTL8295_PTP_CTRL_PTP_TIME_REG05_REG, mac_l);

    return RT_ERR_OK;
}
#endif


#if defined(CONFIG_SDK_RTL8295R) || defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8295_8214_ptpRefTime_get
 * Description:
 *      Get the reference time of PHY of the specified port.
 * Input:
 *      unit       - unit id
 * Output:
 *      pTimeStamp - pointer buffer of the reference time
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
phy_8295_8214_ptpRefTime_get(uint32 unit, rtk_port_t port, rtk_time_timeStamp_t *pTimeStamp)
{
    int32       ret;
    uint32      data, time_l, time_m, time_h;
    uint32      mdxMacId;

    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u", unit, port);
    if (HWP_PHY_BASE_MACID(unit, port) != port)
    {
        ret = RT_ERR_PORT_ID;
        RT_ERR(ret, (MOD_HAL), "unit=%u,port=%u baseMac", unit, port);
        return ret;
    }

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);

    PHY_8295_PHYREG_READ(unit, mdxMacId, RTL8295_PTP_CTRL_PTP_TIME_REG04_PAGE, RTL8295_PTP_CTRL_PTP_TIME_REG04_REG, &data);
    data = REG32_FIELD_SET(data, PHY_8295_PTP_TIME_NSEC_CMD_READ,
                    RTL8295_PTP_CTRL_PTP_TIME_REG04_PTP_TIME_NSEC_CMD_OFFSET, RTL8295_PTP_CTRL_PTP_TIME_REG04_PTP_TIME_NSEC_CMD_MASK);
    PHY_8295_PHYREG_WRITE(unit, mdxMacId, RTL8295_PTP_CTRL_PTP_TIME_REG04_PAGE, RTL8295_PTP_CTRL_PTP_TIME_REG04_REG, data);

    PHY_8295_PHYREG_READ(unit, mdxMacId, RTL8295_PTP_CTRL_PTP_TIME_REG01_PAGE, RTL8295_PTP_CTRL_PTP_TIME_REG01_REG, &data);
    data = REG32_FIELD_SET(data, 0x1,
                    RTL8295_PTP_CTRL_PTP_TIME_REG01_PTP_TIME_EXEC_OFFSET, RTL8295_PTP_CTRL_PTP_TIME_REG01_PTP_TIME_EXEC_MASK);
    PHY_8295_PHYREG_WRITE(unit, mdxMacId, RTL8295_PTP_CTRL_PTP_TIME_REG01_PAGE, RTL8295_PTP_CTRL_PTP_TIME_REG01_REG, data);

    PHY_8295_PTP_WAIT_CMD_READY(unit, mdxMacId);

    PHY_8295_PHYREG_READ(unit, mdxMacId, RTL8295_PTP_CTRL_PTP_TIME_REG11_PAGE, RTL8295_PTP_CTRL_PTP_TIME_REG11_REG, &data);
    time_h = REG32_FIELD_GET(data,    RTL8295_PTP_CTRL_PTP_TIME_REG11_RD_PTP_TIME_NSEC_H_OFFSET, RTL8295_PTP_CTRL_PTP_TIME_REG11_RD_PTP_TIME_NSEC_H_MASK);

    PHY_8295_PHYREG_READ(unit, mdxMacId, RTL8295_PTP_CTRL_PTP_TIME_REG10_PAGE, RTL8295_PTP_CTRL_PTP_TIME_REG10_REG, &data);
    time_l = REG32_FIELD_GET(data,    RTL8295_PTP_CTRL_PTP_TIME_REG10_RD_PTP_TIME_NSEC_L_OFFSET, RTL8295_PTP_CTRL_PTP_TIME_REG10_RD_PTP_TIME_NSEC_L_MASK);

    pTimeStamp->nsec = time_l | (time_h << 16);

    PHY_8295_PHYREG_READ(unit, mdxMacId, RTL8295_PKG_CTRL_PTP_TIME_REG23_PAGE, RTL8295_PKG_CTRL_PTP_TIME_REG23_REG, &data);
    time_h = REG32_FIELD_GET(data,    RTL8295_PKG_CTRL_PTP_TIME_REG23_RD_PTP_TIME_SEC_HH_OFFSET, RTL8295_PKG_CTRL_PTP_TIME_REG23_RD_PTP_TIME_SEC_HH_MASK);

    PHY_8295_PHYREG_READ(unit, mdxMacId, RTL8295_PTP_CTRL_PTP_TIME_REG13_PAGE, RTL8295_PTP_CTRL_PTP_TIME_REG13_REG, &data);
    time_m = REG32_FIELD_GET(data,    RTL8295_PTP_CTRL_PTP_TIME_REG13_RD_PTP_TIME_SEC_H_OFFSET, RTL8295_PTP_CTRL_PTP_TIME_REG13_RD_PTP_TIME_SEC_H_MASK);

    PHY_8295_PHYREG_READ(unit, mdxMacId, RTL8295_PTP_CTRL_PTP_TIME_REG12_PAGE, RTL8295_PTP_CTRL_PTP_TIME_REG12_REG, &data);
    time_l = REG32_FIELD_GET(data,    RTL8295_PTP_CTRL_PTP_TIME_REG12_RD_PTP_TIME_SEC_L_OFFSET, RTL8295_PTP_CTRL_PTP_TIME_REG12_RD_PTP_TIME_SEC_L_MASK);

    pTimeStamp->sec = (uint64)time_l | ((uint64)time_m << 16) | ((uint64)time_h << 32);

    return RT_ERR_OK;
}
#endif


#if defined(CONFIG_SDK_RTL8295R) || defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8295_8214_ptpRefTime_set
 * Description:
 *      Set the reference time of PHY of the specified port.
 * Input:
 *      unit      - unit id
 *      timeStamp - reference timestamp value
 *      exec        - 0 : do not execute, 1: execute
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
phy_8295_8214_ptpRefTime_set(uint32 unit, rtk_port_t port, rtk_time_timeStamp_t timeStamp, uint32 exec)
{
    int32       ret;
    uint32      data, time_l, time_m, time_h;
    uint32      mdxMacId;


    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u", unit, port);
    if (HWP_PHY_BASE_MACID(unit, port) != port)
    {
        ret = RT_ERR_PORT_ID;
        RT_ERR(ret, (MOD_HAL), "unit=%u,port=%u baseMac", unit, port);
        return ret;
    }

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);

    PHY_8295_PHYREG_READ(unit, mdxMacId, RTL8295_PTP_CTRL_PTP_TIME_REG04_PAGE, RTL8295_PTP_CTRL_PTP_TIME_REG04_REG, &data);
    data = REG32_FIELD_SET(data, PHY_8295_PTP_TIME_NSEC_CMD_WRITE,
                    RTL8295_PTP_CTRL_PTP_TIME_REG04_PTP_TIME_NSEC_CMD_OFFSET, RTL8295_PTP_CTRL_PTP_TIME_REG04_PTP_TIME_NSEC_CMD_MASK);
    PHY_8295_PHYREG_WRITE(unit, mdxMacId, RTL8295_PTP_CTRL_PTP_TIME_REG04_PAGE, RTL8295_PTP_CTRL_PTP_TIME_REG04_REG, data);

    time_l = timeStamp.sec & 0xFFFF;
    time_m = ((timeStamp.sec) >> 16) & 0xFFFF;
    time_h = (timeStamp.sec) >> 32;
    PHY_8295_PHYREG_WRITE(unit, mdxMacId, RTL8295_PTP_CTRL_PTP_TIME_REG02_PAGE, RTL8295_PTP_CTRL_PTP_TIME_REG02_REG, time_l);
    PHY_8295_PHYREG_WRITE(unit, mdxMacId, RTL8295_PTP_CTRL_PTP_TIME_REG03_PAGE, RTL8295_PTP_CTRL_PTP_TIME_REG03_REG, time_m);
    PHY_8295_PHYREG_WRITE(unit, mdxMacId, RTL8295_PKG_CTRL_PTP_TIME_REG22_PAGE, RTL8295_PKG_CTRL_PTP_TIME_REG22_REG, time_h);

    time_l = timeStamp.nsec & 0xFFFF;
    time_h = (timeStamp.nsec) >> 16;
    data = REG32_FIELD_SET(0, time_h, RTL8295_PTP_CTRL_PTP_TIME_REG01_PTP_TIME_NSEC_H_OFFSET, RTL8295_PTP_CTRL_PTP_TIME_REG01_PTP_TIME_NSEC_H_MASK);
    if(exec)
        data = REG32_FIELD_SET(data, 0x1, RTL8295_PTP_CTRL_PTP_TIME_REG01_PTP_TIME_EXEC_OFFSET, RTL8295_PTP_CTRL_PTP_TIME_REG01_PTP_TIME_EXEC_MASK);
    PHY_8295_PHYREG_WRITE(unit, mdxMacId, RTL8295_PTP_CTRL_PTP_TIME_REG00_PAGE, RTL8295_PTP_CTRL_PTP_TIME_REG00_REG, time_l);
    PHY_8295_PHYREG_WRITE(unit, mdxMacId, RTL8295_PTP_CTRL_PTP_TIME_REG01_PAGE, RTL8295_PTP_CTRL_PTP_TIME_REG01_REG, data);

    PHY_8295_PTP_WAIT_CMD_READY(unit, mdxMacId);

    return RT_ERR_OK;

}
#endif


#if defined(CONFIG_SDK_RTL8295R) || defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8295_8214_ptpRefTimeAdjust_set
 * Description:
 *      Adjust the reference time of PHY of the specified port.
 * Input:
 *      unit      - unit id
 *      port    - port id
 *      sign      - significant
 *      timeStamp - reference timestamp value
 *      exec        - 0 : do not execute, 1: execute
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      sign=0 for positive adjustment, sign=1 for negative adjustment.
 */
int32
phy_8295_8214_ptpRefTimeAdjust_set(uint32 unit, rtk_port_t port, uint32 sign, rtk_time_timeStamp_t timeStamp, uint32 exec)
{
    int32       ret;
    uint32      data, time_l, time_m, time_h;
    uint32      cmd;
    uint32      mdxMacId;

    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u", unit, port);
    if (HWP_PHY_BASE_MACID(unit, port) != port)
    {
        ret = RT_ERR_PORT_ID;
        RT_ERR(ret, (MOD_HAL), "unit=%u,port=%u baseMac", unit, port);
        return ret;
    }

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);

    if (sign == 0)
    {
        cmd = PHY_8295_PTP_TIME_NSEC_CMD_ADJ_INC;
    } else
    {
        cmd = PHY_8295_PTP_TIME_NSEC_CMD_ADJ_DEC;
    }
    PHY_8295_PHYREG_READ(unit, mdxMacId, RTL8295_PTP_CTRL_PTP_TIME_REG04_PAGE, RTL8295_PTP_CTRL_PTP_TIME_REG04_REG, &data);
    data = REG32_FIELD_SET(data, cmd,
                    RTL8295_PTP_CTRL_PTP_TIME_REG04_PTP_TIME_NSEC_CMD_OFFSET, RTL8295_PTP_CTRL_PTP_TIME_REG04_PTP_TIME_NSEC_CMD_MASK);
    PHY_8295_PHYREG_WRITE(unit, mdxMacId, RTL8295_PTP_CTRL_PTP_TIME_REG04_PAGE, RTL8295_PTP_CTRL_PTP_TIME_REG04_REG, data);

    /* adjust Timer of PHY */
    time_l = (timeStamp.sec) & 0xFFFF;
    time_m = ((timeStamp.sec) >> 16) & 0xFFFF;
    time_h = (timeStamp.sec) >> 32;
    PHY_8295_PHYREG_WRITE(unit, mdxMacId, RTL8295_PTP_CTRL_PTP_TIME_REG02_PAGE, RTL8295_PTP_CTRL_PTP_TIME_REG02_REG, time_l);
    PHY_8295_PHYREG_WRITE(unit, mdxMacId, RTL8295_PTP_CTRL_PTP_TIME_REG03_PAGE, RTL8295_PTP_CTRL_PTP_TIME_REG03_REG, time_m);
    PHY_8295_PHYREG_WRITE(unit, mdxMacId, RTL8295_PKG_CTRL_PTP_TIME_REG22_PAGE, RTL8295_PKG_CTRL_PTP_TIME_REG22_REG, time_h);

    time_l = timeStamp.nsec & 0xFFFF;
    time_h = (timeStamp.nsec) >> 16;
    data = REG32_FIELD_SET(0, time_h, RTL8295_PTP_CTRL_PTP_TIME_REG01_PTP_TIME_NSEC_H_OFFSET, RTL8295_PTP_CTRL_PTP_TIME_REG01_PTP_TIME_NSEC_H_MASK);
    if(exec)
        data = REG32_FIELD_SET(data, 0x1, RTL8295_PTP_CTRL_PTP_TIME_REG01_PTP_TIME_EXEC_OFFSET, RTL8295_PTP_CTRL_PTP_TIME_REG01_PTP_TIME_EXEC_MASK);
    PHY_8295_PHYREG_WRITE(unit, mdxMacId, RTL8295_PTP_CTRL_PTP_TIME_REG00_PAGE, RTL8295_PTP_CTRL_PTP_TIME_REG00_REG, time_l);
    PHY_8295_PHYREG_WRITE(unit, mdxMacId, RTL8295_PTP_CTRL_PTP_TIME_REG01_PAGE, RTL8295_PTP_CTRL_PTP_TIME_REG01_REG, data);

    PHY_8295_PTP_WAIT_CMD_READY(unit, mdxMacId);


    return RT_ERR_OK;
}
#endif


#if defined(CONFIG_SDK_RTL8295R) || defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8295_8214_ptpRefTimeEnable_get
 * Description:
 *      Get the enable state of reference time of PHY of the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
phy_8295_8214_ptpRefTimeEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32       ret;
    uint32      data;
    uint32      mdxMacId;

    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u", unit, port);

    if (HWP_PHY_BASE_MACID(unit, port) != port)
    {
        ret = RT_ERR_PORT_ID;
        RT_ERR(ret, (MOD_HAL), "unit=%u,port=%u baseMac", unit, port);
        return ret;
    }

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);

    PHY_8295_PHYREG_READ(unit, mdxMacId, RTL8295_PTP_CTRL_PTP_TIME_REG04_PAGE, RTL8295_PTP_CTRL_PTP_TIME_REG04_REG, &data);

    if (data & (RTL8295_PTP_CTRL_PTP_TIME_REG04_TIMER_EN_FRC_MASK | RTL8295_PTP_CTRL_PTP_TIME_REG04_TIMER_1588_EN_MASK))
    {
        *pEnable = ENABLED;
    }
    else
    {
        *pEnable = DISABLED;
    }

    return RT_ERR_OK;
}
#endif

#if defined(CONFIG_SDK_RTL8295R) || defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8214qf_ptpRefTimeEnable_set
 * Description:
 *      Set the enable state of reference time of PHY of the specified port.
 * Input:
 *      unit   - unit id
 *      port    - port id
 *      enable - status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
phy_8295_8214_ptpRefTimeEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32       ret;
    uint32      data;
    uint32      mdxMacId;

    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,enable=%d", unit, port, enable);
    if (HWP_PHY_BASE_MACID(unit, port) != port)
    {
        ret = RT_ERR_PORT_ID;
        RT_ERR(ret, (MOD_HAL), "unit=%u,port=%u baseMac", unit, port);
        return ret;
    }

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);

    PHY_8295_PHYREG_READ(unit, mdxMacId, RTL8295_PTP_CTRL_PTP_TIME_REG04_PAGE, RTL8295_PTP_CTRL_PTP_TIME_REG04_REG, &data);
    if (enable == ENABLED)
    {
        data = REG32_FIELD_SET(data, 1, RTL8295_PTP_CTRL_PTP_TIME_REG04_TIMER_EN_FRC_OFFSET, RTL8295_PTP_CTRL_PTP_TIME_REG04_TIMER_EN_FRC_MASK);
        data = REG32_FIELD_SET(data, 1, RTL8295_PTP_CTRL_PTP_TIME_REG04_TIMER_1588_EN_OFFSET, RTL8295_PTP_CTRL_PTP_TIME_REG04_TIMER_1588_EN_MASK);
    }
    else
    {
        data = REG32_FIELD_SET(data, 0, RTL8295_PTP_CTRL_PTP_TIME_REG04_TIMER_EN_FRC_OFFSET, RTL8295_PTP_CTRL_PTP_TIME_REG04_TIMER_EN_FRC_MASK);
        data = REG32_FIELD_SET(data, 0, RTL8295_PTP_CTRL_PTP_TIME_REG04_TIMER_1588_EN_OFFSET, RTL8295_PTP_CTRL_PTP_TIME_REG04_TIMER_1588_EN_MASK);
    }

    PHY_8295_PHYREG_WRITE(unit, mdxMacId, RTL8295_PTP_CTRL_PTP_TIME_REG04_PAGE, RTL8295_PTP_CTRL_PTP_TIME_REG04_REG, data);

    return RT_ERR_OK;
}
#endif

#if defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8214qf_ptpEnable_get
 * Description:
 *      Get PTP status of the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT         - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
phy_8214qf_ptpEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    uint32      data;
    uint32      mdxMacId;
    uint32      sdsId;

    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u", unit, port);

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);

    PHY_8295_SDSID_GET(unit, port, &sdsId);

    /* PTP_SXP0_REG12 */
    PHY_8295_PTP_SXP0_REGN_READ(unit, mdxMacId, sdsId, RTL8295_PTP_SXP0_REG12, &data);
    if (data & (RTL8295_PTP_CTRL_PTP_S4P0_REG12_CFG_UDP_EN_MASK | RTL8295_PTP_CTRL_PTP_S4P0_REG12_CFG_ETH_EN_MASK))
    {
        *pEnable = ENABLED;
    }
    else
    {
        *pEnable = DISABLED;
    }

    return RT_ERR_OK;
}
#endif

#if defined(CONFIG_SDK_RTL8295R)
/* Function Name:
 *      phy_8295_8214_ptpEnable_get
 * Description:
 *      Get PTP status of the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT         - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
phy_8295r_ptpEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32        ret;
    uint32      data;
    uint32      mdxMacId;
    uint32      sdsId, speed;

    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u", unit, port);

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);

    if ((ret = phy_8295r_speed_get(unit, port, &speed)) != RT_ERR_OK)
    {
        return ret;
    }

    if (PORT_SPEED_10G == speed)
    {
        /* PTP_SXP0_REG12 */
        PHY_8295_PHYREG_READ(unit, mdxMacId, RTL8295_PTP_CTRL_PTP_TG_REG12_PAGE, RTL8295_PTP_CTRL_PTP_TG_REG12_REG, &data);
        if (data & (RTL8295_PTP_CTRL_PTP_TG_REG12_CFG_UDP_EN_MASK | RTL8295_PTP_CTRL_PTP_TG_REG12_CFG_ETH_EN_MASK))
        {
            *pEnable = ENABLED;
        }
        else
        {
            *pEnable = DISABLED;
        }
    }
    else
    {
        PHY_8295_SDSID_GET(unit, port, &sdsId);
        /* PTP_SXP0_REG12 */
        PHY_8295_PTP_SXP0_REGN_READ(unit, mdxMacId, sdsId, RTL8295_PTP_SXP0_REG12, &data);
        if (data & (RTL8295_PTP_CTRL_PTP_S4P0_REG12_CFG_UDP_EN_MASK | RTL8295_PTP_CTRL_PTP_S4P0_REG12_CFG_ETH_EN_MASK))
        {
            *pEnable = ENABLED;
        }
        else
        {
            *pEnable = DISABLED;
        }
    }

    return RT_ERR_OK;
}
#endif

#if defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8214qf_ptpEnable_set
 * Description:
 *      Set PTP status of the specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT     - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
phy_8214qf_ptpEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    uint32      data;
    uint32      mdxMacId;
    uint32      sdsId;


    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,enable=%d", unit, port, enable);
    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);

    PHY_8295_SDSID_GET(unit, port, &sdsId);

    /* PTP_SXP0_REG12 */
    PHY_8295_PTP_SXP0_REGN_READ(unit, mdxMacId, sdsId, RTL8295_PTP_SXP0_REG12, &data);
    if (enable == ENABLED)
    {
        data = REG32_FIELD_SET(data, 1, RTL8295_PTP_CTRL_PTP_S4P0_REG12_CFG_UDP_EN_OFFSET, RTL8295_PTP_CTRL_PTP_S4P0_REG12_CFG_UDP_EN_MASK);
        data = REG32_FIELD_SET(data, 1, RTL8295_PTP_CTRL_PTP_S4P0_REG12_CFG_ETH_EN_OFFSET, RTL8295_PTP_CTRL_PTP_S4P0_REG12_CFG_ETH_EN_MASK);
    }
    else
    {
        data = REG32_FIELD_SET(data, 0, RTL8295_PTP_CTRL_PTP_S4P0_REG12_CFG_UDP_EN_OFFSET, RTL8295_PTP_CTRL_PTP_S4P0_REG12_CFG_UDP_EN_MASK);
        data = REG32_FIELD_SET(data, 0, RTL8295_PTP_CTRL_PTP_S4P0_REG12_CFG_ETH_EN_OFFSET, RTL8295_PTP_CTRL_PTP_S4P0_REG12_CFG_ETH_EN_MASK);
    }

    PHY_8295_PTP_SXP0_REGN_WRITE(unit, mdxMacId, sdsId, RTL8295_PTP_SXP0_REG12, data);

    return RT_ERR_OK;
}
#endif

#if defined(CONFIG_SDK_RTL8295R)
/* Function Name:
 *      phy_8295r_ptpEnable_set
 * Description:
 *      Set PTP status of the specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT     - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
phy_8295r_ptpEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    uint32      data;
    uint32      mdxMacId;
    uint32      sdsId;

    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,enable=%d", unit, port, enable);
    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);


    /* Configure 10G */
    PHY_8295_PHYREG_READ(unit, mdxMacId, RTL8295_PTP_CTRL_PTP_TG_REG12_PAGE, RTL8295_PTP_CTRL_PTP_TG_REG12_REG, &data);

    if (enable == ENABLED)
    {
        data = REG32_FIELD_SET(data, 1, RTL8295_PTP_CTRL_PTP_TG_REG12_CFG_UDP_EN_OFFSET, RTL8295_PTP_CTRL_PTP_TG_REG12_CFG_UDP_EN_MASK);
        data = REG32_FIELD_SET(data, 1, RTL8295_PTP_CTRL_PTP_TG_REG12_CFG_ETH_EN_OFFSET, RTL8295_PTP_CTRL_PTP_TG_REG12_CFG_ETH_EN_MASK);
    }
    else
    {
        data = REG32_FIELD_SET(data, 0, RTL8295_PTP_CTRL_PTP_TG_REG12_CFG_UDP_EN_OFFSET, RTL8295_PTP_CTRL_PTP_TG_REG12_CFG_UDP_EN_MASK);
        data = REG32_FIELD_SET(data, 0, RTL8295_PTP_CTRL_PTP_TG_REG12_CFG_ETH_EN_OFFSET, RTL8295_PTP_CTRL_PTP_TG_REG12_CFG_ETH_EN_MASK);
    }

    PHY_8295_PHYREG_WRITE(unit, mdxMacId, RTL8295_PTP_CTRL_PTP_TG_REG12_PAGE, RTL8295_PTP_CTRL_PTP_TG_REG12_REG, data);

    /* Configure 1G */
    PHY_8295_SDSID_GET(unit, port, &sdsId);
    /* PTP_SXP0_REG12 */
    PHY_8295_PTP_SXP0_REGN_READ(unit, mdxMacId, sdsId, RTL8295_PTP_SXP0_REG12, &data);
    if (enable == ENABLED)
    {
        data = REG32_FIELD_SET(data, 1, RTL8295_PTP_CTRL_PTP_S4P0_REG12_CFG_UDP_EN_OFFSET, RTL8295_PTP_CTRL_PTP_S4P0_REG12_CFG_UDP_EN_MASK);
        data = REG32_FIELD_SET(data, 1, RTL8295_PTP_CTRL_PTP_S4P0_REG12_CFG_ETH_EN_OFFSET, RTL8295_PTP_CTRL_PTP_S4P0_REG12_CFG_ETH_EN_MASK);
    }
    else
    {
        data = REG32_FIELD_SET(data, 0, RTL8295_PTP_CTRL_PTP_S4P0_REG12_CFG_UDP_EN_OFFSET, RTL8295_PTP_CTRL_PTP_S4P0_REG12_CFG_UDP_EN_MASK);
        data = REG32_FIELD_SET(data, 0, RTL8295_PTP_CTRL_PTP_S4P0_REG12_CFG_ETH_EN_OFFSET, RTL8295_PTP_CTRL_PTP_S4P0_REG12_CFG_ETH_EN_MASK);
    }
    PHY_8295_PTP_SXP0_REGN_WRITE(unit, mdxMacId, sdsId, RTL8295_PTP_SXP0_REG12, data);


    return RT_ERR_OK;
}
#endif


#if defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8214qf_ptpRxTimestamp_get
 * Description:
 *      Get PTP Rx timstamp according to the PTP identifier on the dedicated port from the specified device.
 * Input:
 *      unit       - unit id
 *      port       - port id
 *      identifier - indentifier of PTP packet
 * Output:
 *      pTimeStamp - pointer buffer of TIME timestamp
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
phy_8214qf_ptpRxTimestamp_get(uint32 unit, rtk_port_t port,
    rtk_time_ptpIdentifier_t identifier, rtk_time_timeStamp_t *pTimeStamp)
{
    int32       ret = RT_ERR_OK;
    uint32      data, time_l, time_m, time_h;
    uint32      mdxMacId, sdsId;
    uint32      seqId;

    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u", unit, port);
    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);

    PHY_8295_SDSID_GET(unit, port, &sdsId);

    switch (identifier.msgType)
    {
      case PTP_MSG_TYPE_SYNC:
        /* PTP_SXP0_REG04.RX_SYNC_SEQ_ID */
        PHY_8295_PTP_SXP0_REGN_READ(unit, mdxMacId, sdsId, RTL8295_PTP_SXP0_REG04, &data);
        seqId = REG32_FIELD_GET(data, RTL8295_PTP_CTRL_PTP_S4P0_REG04_RX_SYNC_SEQ_ID_OFFSET, RTL8295_PTP_CTRL_PTP_S4P0_REG04_RX_SYNC_SEQ_ID_MASK);
        break;
      case PTP_MSG_TYPE_DELAY_REQ:
        /* PTP_SXP0_REG05.RX_DELAY_REQ_SEQ_ID */
        PHY_8295_PTP_SXP0_REGN_READ(unit, mdxMacId, sdsId, RTL8295_PTP_SXP0_REG05, &data);
        seqId = REG32_FIELD_GET(data, RTL8295_PTP_CTRL_PTP_S4P0_REG05_RX_DELAY_REQ_SEQ_ID_OFFSET, RTL8295_PTP_CTRL_PTP_S4P0_REG05_RX_DELAY_REQ_SEQ_ID_MASK);
        break;
      case PTP_MSG_TYPE_PDELAY_REQ:
        /* PTP_SXP0_REG06.RX_PDELAY_REQ_SEQ_ID */
        PHY_8295_PTP_SXP0_REGN_READ(unit, mdxMacId, sdsId, RTL8295_PTP_SXP0_REG06, &data);
        seqId = REG32_FIELD_GET(data, RTL8295_PTP_CTRL_PTP_S4P0_REG06_RX_PDELAY_REQ_SEQ_ID_OFFSET, RTL8295_PTP_CTRL_PTP_S4P0_REG06_RX_PDELAY_REQ_SEQ_ID_MASK);
        break;
      case PTP_MSG_TYPE_PDELAY_RESP:
        /* PTP_SXP0_REG06.RX_PDELAY_RESP_SEQ_I D */
        PHY_8295_PTP_SXP0_REGN_READ(unit, mdxMacId, sdsId, RTL8295_PTP_SXP0_REG07, &data);
        seqId = REG32_FIELD_GET(data, RTL8295_PTP_CTRL_PTP_S4P0_REG07_RX_PDELAY_RESP_SEQ_ID_OFFSET, RTL8295_PTP_CTRL_PTP_S4P0_REG07_RX_PDELAY_RESP_SEQ_ID_MASK);
        break;
      default:
        return RT_ERR_INPUT;
    }

    /* Input sequence ID NEED match currently sequence ID of chip */
    if (seqId != identifier.sequenceId)
    {
        ret = RT_ERR_INPUT;
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "unit=%u,port=%u mdxMacId %u sds %u seqId %u", unit, port, mdxMacId, sdsId, seqId);
        goto CLEAR_PEND;
    }

    /* Get sec */
    /* PTP_SXP0_REG10.PORT_SEC_B15_0 */
    PHY_8295_PTP_SXP0_REGN_READ(unit, mdxMacId, sdsId, RTL8295_PTP_SXP0_REG10, &time_l);
    /* PTP_SXP0_REG11.PORT_SEC_B31_16 */
    PHY_8295_PTP_SXP0_REGN_READ(unit, mdxMacId, sdsId, RTL8295_PTP_SXP0_REG11, &time_m);
    /* PTP_SXP0_REG13.PORT_SEC_B47_32 */
    PHY_8295_PTP_SXP0_REGN_READ(unit, mdxMacId, sdsId, RTL8295_PTP_SXP0_REG13, &time_h);

    pTimeStamp->sec = (REG32_FIELD_GET(time_m, RTL8295_PTP_CTRL_PTP_S4P0_REG11_PORT_SEC_B31_16_OFFSET, RTL8295_PTP_CTRL_PTP_S4P0_REG11_PORT_SEC_B31_16_MASK) << 16)
                     | REG32_FIELD_GET(time_l, RTL8295_PTP_CTRL_PTP_S4P0_REG10_PORT_SEC_B15_0_OFFSET, RTL8295_PTP_CTRL_PTP_S4P0_REG10_PORT_SEC_B15_0_MASK);

    pTimeStamp->sec |= (uint64)time_h << 32;

    /* Get nsec */
    /* PTP_SXP0_REG08.PORT_NSEC_B15_0 */
    PHY_8295_PTP_SXP0_REGN_READ(unit, mdxMacId, sdsId, RTL8295_PTP_SXP0_REG08, &time_l);
    /* PTP_SXP0_REG09.PORT_NSEC_B29_16 */
    PHY_8295_PTP_SXP0_REGN_READ(unit, mdxMacId, sdsId, RTL8295_PTP_SXP0_REG09, &time_h);
    pTimeStamp->nsec = (REG32_FIELD_GET(time_h, RTL8295_PTP_CTRL_PTP_S4P0_REG09_PORT_NSEC_B29_16_OFFSET, RTL8295_PTP_CTRL_PTP_S4P0_REG09_PORT_NSEC_B29_16_MASK) << 16)
                      | REG32_FIELD_GET(time_l, RTL8295_PTP_CTRL_PTP_S4P0_REG08_PORT_NSEC_B15_0_OFFSET, RTL8295_PTP_CTRL_PTP_S4P0_REG08_PORT_NSEC_B15_0_MASK);

  CLEAR_PEND:
    /* Clear the pending status */
    /* PTP_SXP0_REG12 */
    PHY_8295_PTP_SXP0_REGN_READ(unit, mdxMacId, sdsId, RTL8295_PTP_SXP0_REG12, &data);
    switch (identifier.msgType)
    {
      case PTP_MSG_TYPE_SYNC:
        /* RX_SYNC */
        data = REG32_FIELD_SET(data, 1, RTL8295_PTP_CTRL_PTP_S4P0_REG12_RX_SYNC_OFFSET, RTL8295_PTP_CTRL_PTP_S4P0_REG12_RX_SYNC_MASK);
        break;
      case PTP_MSG_TYPE_DELAY_REQ:
        /* RX_DELAY_REQ */
        data = REG32_FIELD_SET(data, 1, RTL8295_PTP_CTRL_PTP_S4P0_REG12_RX_DELAY_REQ_OFFSET, RTL8295_PTP_CTRL_PTP_S4P0_REG12_RX_DELAY_REQ_MASK);
        break;
      case PTP_MSG_TYPE_PDELAY_REQ:
        /* RX_PDELAY_REQ */
        data = REG32_FIELD_SET(data, 1, RTL8295_PTP_CTRL_PTP_S4P0_REG12_RX_PDELAY_REQ_OFFSET, RTL8295_PTP_CTRL_PTP_S4P0_REG12_RX_PDELAY_REQ_MASK);
        break;
      case PTP_MSG_TYPE_PDELAY_RESP:
        /* RX_PDELAY_RESP */
        data = REG32_FIELD_SET(data, 1, RTL8295_PTP_CTRL_PTP_S4P0_REG12_RX_PDELAY_RESP_OFFSET, RTL8295_PTP_CTRL_PTP_S4P0_REG12_RX_PDELAY_RESP_MASK);
        break;
    }
    PHY_8295_PTP_SXP0_REGN_WRITE(unit, mdxMacId, sdsId, RTL8295_PTP_SXP0_REG12, data);

    return ret;
}
#endif

#if defined(CONFIG_SDK_RTL8295R)
/* Function Name:
 *      phy_8295r_ptpRxTimestamp_get
 * Description:
 *      Get PTP Rx timstamp according to the PTP identifier on the dedicated port from the specified device.
 * Input:
 *      unit       - unit id
 *      port       - port id
 *      identifier - indentifier of PTP packet
 * Output:
 *      pTimeStamp - pointer buffer of TIME timestamp
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
phy_8295r_ptpRxTimestamp_get(uint32 unit, rtk_port_t port,
    rtk_time_ptpIdentifier_t identifier, rtk_time_timeStamp_t *pTimeStamp)
{
    int32       ret = RT_ERR_OK;
    uint32      data, time_l, time_m, time_h;
    uint32      mdxMacId, sdsId;
    uint32      seqId;
    uint32      speed;

    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u", unit, port);
    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);

    PHY_8295_SDSID_GET(unit, port, &sdsId);

    if ((ret = phy_8295r_speed_get(unit, port, &speed)) != RT_ERR_OK)
    {
        return ret;
    }

    if (PORT_SPEED_10G == speed)
    {
        switch (identifier.msgType)
        {
          case PTP_MSG_TYPE_SYNC:
            PHY_8295_PHYREG_READ(unit, mdxMacId, RTL8295_PTP_CTRL_PTP_TG_REG04_PAGE, RTL8295_PTP_CTRL_PTP_TG_REG04_REG, &seqId);
            break;
          case PTP_MSG_TYPE_DELAY_REQ:
            PHY_8295_PHYREG_READ(unit, mdxMacId, RTL8295_PTP_CTRL_PTP_TG_REG05_PAGE, RTL8295_PTP_CTRL_PTP_TG_REG05_REG, &seqId);
            break;
          case PTP_MSG_TYPE_PDELAY_REQ:
            PHY_8295_PHYREG_READ(unit, mdxMacId, RTL8295_PTP_CTRL_PTP_TG_REG06_PAGE, RTL8295_PTP_CTRL_PTP_TG_REG06_REG, &seqId);
            break;
          case PTP_MSG_TYPE_PDELAY_RESP:
            PHY_8295_PHYREG_READ(unit, mdxMacId, RTL8295_PTP_CTRL_PTP_TG_REG07_PAGE, RTL8295_PTP_CTRL_PTP_TG_REG07_REG, &seqId);
            break;
          default:
            return RT_ERR_INPUT;
        }
    }
    else
    {
        switch (identifier.msgType)
        {
          case PTP_MSG_TYPE_SYNC:
            /* PTP_SXP0_REG04.RX_SYNC_SEQ_ID */
            PHY_8295_PTP_SXP0_REGN_READ(unit, mdxMacId, sdsId, RTL8295_PTP_SXP0_REG04, &data);
            seqId = REG32_FIELD_GET(data, RTL8295_PTP_CTRL_PTP_S4P0_REG04_RX_SYNC_SEQ_ID_OFFSET, RTL8295_PTP_CTRL_PTP_S4P0_REG04_RX_SYNC_SEQ_ID_MASK);
            break;
          case PTP_MSG_TYPE_DELAY_REQ:
            /* PTP_SXP0_REG05.RX_DELAY_REQ_SEQ_ID */
            PHY_8295_PTP_SXP0_REGN_READ(unit, mdxMacId, sdsId, RTL8295_PTP_SXP0_REG05, &data);
            seqId = REG32_FIELD_GET(data, RTL8295_PTP_CTRL_PTP_S4P0_REG05_RX_DELAY_REQ_SEQ_ID_OFFSET, RTL8295_PTP_CTRL_PTP_S4P0_REG05_RX_DELAY_REQ_SEQ_ID_MASK);
            break;
          case PTP_MSG_TYPE_PDELAY_REQ:
            /* PTP_SXP0_REG06.RX_PDELAY_REQ_SEQ_ID */
            PHY_8295_PTP_SXP0_REGN_READ(unit, mdxMacId, sdsId, RTL8295_PTP_SXP0_REG06, &data);
            seqId = REG32_FIELD_GET(data, RTL8295_PTP_CTRL_PTP_S4P0_REG06_RX_PDELAY_REQ_SEQ_ID_OFFSET, RTL8295_PTP_CTRL_PTP_S4P0_REG06_RX_PDELAY_REQ_SEQ_ID_MASK);
            break;
          case PTP_MSG_TYPE_PDELAY_RESP:
            /* PTP_SXP0_REG06.RX_PDELAY_RESP_SEQ_I D */
            PHY_8295_PTP_SXP0_REGN_READ(unit, mdxMacId, sdsId, RTL8295_PTP_SXP0_REG07, &data);
            seqId = REG32_FIELD_GET(data, RTL8295_PTP_CTRL_PTP_S4P0_REG07_RX_PDELAY_RESP_SEQ_ID_OFFSET, RTL8295_PTP_CTRL_PTP_S4P0_REG07_RX_PDELAY_RESP_SEQ_ID_MASK);
            break;
          default:
            return RT_ERR_INPUT;
        }
    }

    /* Input sequence ID NEED match currently sequence ID of chip */
    if (seqId != identifier.sequenceId)
    {
        ret = RT_ERR_INPUT;
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "unit=%u,port=%u mdxMacId %u sds %u seqId %u speed %d", unit, port, mdxMacId, sdsId, seqId, speed);
        goto CLEAR_PEND;
    }

    if (PORT_SPEED_10G == speed)
    {
        /* Get sec */
        PHY_8295_PHYREG_READ(unit, mdxMacId, RTL8295_PTP_CTRL_PTP_TG_REG10_PAGE, RTL8295_PTP_CTRL_PTP_TG_REG10_REG, &time_l);
        PHY_8295_PHYREG_READ(unit, mdxMacId, RTL8295_PTP_CTRL_PTP_TG_REG11_PAGE, RTL8295_PTP_CTRL_PTP_TG_REG11_REG, &time_m);
        PHY_8295_PHYREG_READ(unit, mdxMacId, RTL8295_PKG_CTRL_PTP_TG_REG13_PAGE, RTL8295_PKG_CTRL_PTP_TG_REG13_REG, &time_h);
        pTimeStamp->sec = (uint64)time_l | ((uint64)time_m << 16) | ((uint64)time_h << 32);

        /* Get nsec */
        PHY_8295_PHYREG_READ(unit, mdxMacId, RTL8295_PTP_CTRL_PTP_TG_REG08_PAGE, RTL8295_PTP_CTRL_PTP_TG_REG08_REG, &time_l);
        PHY_8295_PHYREG_READ(unit, mdxMacId, RTL8295_PTP_CTRL_PTP_TG_REG09_PAGE, RTL8295_PTP_CTRL_PTP_TG_REG09_REG, &time_h);
        pTimeStamp->nsec= (time_h << 16) | time_l;
    }
    else
    {
        /* Get sec */
        /* PTP_SXP0_REG10.PORT_SEC_B15_0 */
        PHY_8295_PTP_SXP0_REGN_READ(unit, mdxMacId, sdsId, RTL8295_PTP_SXP0_REG10, &time_l);
        /* PTP_SXP0_REG11.PORT_SEC_B31_16 */
        PHY_8295_PTP_SXP0_REGN_READ(unit, mdxMacId, sdsId, RTL8295_PTP_SXP0_REG11, &time_m);
        /* PTP_SXP0_REG13.PORT_SEC_B47_32 */
        PHY_8295_PTP_SXP0_REGN_READ(unit, mdxMacId, sdsId, RTL8295_PTP_SXP0_REG13, &time_h);

        pTimeStamp->sec = (REG32_FIELD_GET(time_m, RTL8295_PTP_CTRL_PTP_S4P0_REG11_PORT_SEC_B31_16_OFFSET, RTL8295_PTP_CTRL_PTP_S4P0_REG11_PORT_SEC_B31_16_MASK) << 16)
                         | REG32_FIELD_GET(time_l, RTL8295_PTP_CTRL_PTP_S4P0_REG10_PORT_SEC_B15_0_OFFSET, RTL8295_PTP_CTRL_PTP_S4P0_REG10_PORT_SEC_B15_0_MASK);

        pTimeStamp->sec |= (uint64)time_h << 32;

        /* Get nsec */
        /* PTP_SXP0_REG08.PORT_NSEC_B15_0 */
        PHY_8295_PTP_SXP0_REGN_READ(unit, mdxMacId, sdsId, RTL8295_PTP_SXP0_REG08, &time_l);
        /* PTP_SXP0_REG09.PORT_NSEC_B29_16 */
        PHY_8295_PTP_SXP0_REGN_READ(unit, mdxMacId, sdsId, RTL8295_PTP_SXP0_REG09, &time_h);
        pTimeStamp->nsec = (REG32_FIELD_GET(time_h, RTL8295_PTP_CTRL_PTP_S4P0_REG09_PORT_NSEC_B29_16_OFFSET, RTL8295_PTP_CTRL_PTP_S4P0_REG09_PORT_NSEC_B29_16_MASK) << 16)
                          | REG32_FIELD_GET(time_l, RTL8295_PTP_CTRL_PTP_S4P0_REG08_PORT_NSEC_B15_0_OFFSET, RTL8295_PTP_CTRL_PTP_S4P0_REG08_PORT_NSEC_B15_0_MASK);
    }

CLEAR_PEND:
    /* Clear the pending status */

    if (PORT_SPEED_10G == speed)
    {

        PHY_8295_PHYREG_READ(unit, mdxMacId, RTL8295_PTP_CTRL_PTP_TG_REG12_PAGE, RTL8295_PTP_CTRL_PTP_TG_REG12_REG, &data);

        data &= ~0xFF;

        switch (identifier.msgType)
        {
          case PTP_MSG_TYPE_SYNC:
            /* TX_SYNC */
            data = REG32_FIELD_SET(data, 1, RTL8295_PTP_CTRL_PTP_TG_REG12_RX_SYNC_OFFSET, RTL8295_PTP_CTRL_PTP_TG_REG12_RX_SYNC_MASK);
            break;
          case PTP_MSG_TYPE_DELAY_REQ:
            /* TX_DELAY_REQ */
            data = REG32_FIELD_SET(data, 1, RTL8295_PTP_CTRL_PTP_TG_REG12_RX_DELAY_REQ_OFFSET, RTL8295_PTP_CTRL_PTP_TG_REG12_RX_DELAY_REQ_MASK);
            break;
          case PTP_MSG_TYPE_PDELAY_REQ:
            /* TX_PDELAY_REQ */
            data = REG32_FIELD_SET(data, 1, RTL8295_PTP_CTRL_PTP_TG_REG12_RX_PDELAY_REQ_OFFSET, RTL8295_PTP_CTRL_PTP_TG_REG12_RX_PDELAY_REQ_MASK);
            break;
          case PTP_MSG_TYPE_PDELAY_RESP:
            /* TX_PDELAY_RESP */
            data = REG32_FIELD_SET(data, 1, RTL8295_PTP_CTRL_PTP_TG_REG12_RX_PDELAY_RESP_OFFSET, RTL8295_PTP_CTRL_PTP_TG_REG12_RX_PDELAY_RESP_MASK);
            break;
        }

        PHY_8295_PHYREG_WRITE(unit, mdxMacId, RTL8295_PTP_CTRL_PTP_TG_REG12_PAGE, RTL8295_PTP_CTRL_PTP_TG_REG12_REG, data);
    }
    else
    {
        /* PTP_SXP0_REG12 */
        PHY_8295_PTP_SXP0_REGN_READ(unit, mdxMacId, sdsId, RTL8295_PTP_SXP0_REG12, &data);
        switch (identifier.msgType)
        {
          case PTP_MSG_TYPE_SYNC:
            /* RX_SYNC */
            data = REG32_FIELD_SET(data, 1, RTL8295_PTP_CTRL_PTP_S4P0_REG12_RX_SYNC_OFFSET, RTL8295_PTP_CTRL_PTP_S4P0_REG12_RX_SYNC_MASK);
            break;
          case PTP_MSG_TYPE_DELAY_REQ:
            /* RX_DELAY_REQ */
            data = REG32_FIELD_SET(data, 1, RTL8295_PTP_CTRL_PTP_S4P0_REG12_RX_DELAY_REQ_OFFSET, RTL8295_PTP_CTRL_PTP_S4P0_REG12_RX_DELAY_REQ_MASK);
            break;
          case PTP_MSG_TYPE_PDELAY_REQ:
            /* RX_PDELAY_REQ */
            data = REG32_FIELD_SET(data, 1, RTL8295_PTP_CTRL_PTP_S4P0_REG12_RX_PDELAY_REQ_OFFSET, RTL8295_PTP_CTRL_PTP_S4P0_REG12_RX_PDELAY_REQ_MASK);
            break;
          case PTP_MSG_TYPE_PDELAY_RESP:
            /* RX_PDELAY_RESP */
            data = REG32_FIELD_SET(data, 1, RTL8295_PTP_CTRL_PTP_S4P0_REG12_RX_PDELAY_RESP_OFFSET, RTL8295_PTP_CTRL_PTP_S4P0_REG12_RX_PDELAY_RESP_MASK);
            break;
        }
        PHY_8295_PTP_SXP0_REGN_WRITE(unit, mdxMacId, sdsId, RTL8295_PTP_SXP0_REG12, data);
    }

    return ret;
}
#endif

#if defined(CONFIG_SDK_RTL8295R) || defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8214qf_ptpTxTimestamp_get
 * Description:
 *      Get PTP Tx timstamp according to the PTP identifier on the dedicated port from the specified device.
 * Input:
 *      unit       - unit id
 *      port       - port id
 *      identifier - indentifier of PTP packet
 * Output:
 *      pTimeStamp - pointer buffer of TIME timestamp
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
phy_8214qf_ptpTxTimestamp_get(uint32 unit, rtk_port_t port,
    rtk_time_ptpIdentifier_t identifier, rtk_time_timeStamp_t *pTimeStamp)
{
    int32       ret = RT_ERR_OK;
    uint32      data, time_l, time_m, time_h;
    uint32      mdxMacId, sdsId;
    uint32      seqId;

    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u", unit, port);
    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);

    PHY_8295_SDSID_GET(unit, port, &sdsId);

    switch (identifier.msgType)
    {
      case PTP_MSG_TYPE_SYNC:
        /* PTP_SXP0_REG00.TX_SYNC_SEQ_ID */
        PHY_8295_PTP_SXP0_REGN_READ(unit, mdxMacId, sdsId, RTL8295_PTP_SXP0_REG00, &data);
        seqId = REG32_FIELD_GET(data, RTL8295_PTP_CTRL_PTP_S4P0_REG00_TX_SYNC_SEQ_ID_OFFSET, RTL8295_PTP_CTRL_PTP_S4P0_REG00_TX_SYNC_SEQ_ID_MASK);
        break;
      case PTP_MSG_TYPE_DELAY_REQ:
        /* PTP_SXP0_REG01.TX_DELAY_REQ_SEQ_ID */
        PHY_8295_PTP_SXP0_REGN_READ(unit, mdxMacId, sdsId, RTL8295_PTP_SXP0_REG01, &data);
        seqId = REG32_FIELD_GET(data, RTL8295_PTP_CTRL_PTP_S4P0_REG01_TX_DELAY_REQ_SEQ_ID_OFFSET, RTL8295_PTP_CTRL_PTP_S4P0_REG01_TX_DELAY_REQ_SEQ_ID_MASK);
        break;
      case PTP_MSG_TYPE_PDELAY_REQ:
        /* PTP_SXP0_REG02.TX_PDELAY_REQ_SEQ_ID */
        PHY_8295_PTP_SXP0_REGN_READ(unit, mdxMacId, sdsId, RTL8295_PTP_SXP0_REG02, &data);
        seqId = REG32_FIELD_GET(data, RTL8295_PTP_CTRL_PTP_S4P0_REG02_TX_PDELAY_REQ_SEQ_ID_OFFSET, RTL8295_PTP_CTRL_PTP_S4P0_REG02_TX_PDELAY_REQ_SEQ_ID_MASK);
        break;
      case PTP_MSG_TYPE_PDELAY_RESP:
        /* PTP_SXP0_REG03.TX_PDELAY_RESP_SEQ_I D */
        PHY_8295_PTP_SXP0_REGN_READ(unit, mdxMacId, sdsId, RTL8295_PTP_SXP0_REG03, &data);
        seqId = REG32_FIELD_GET(data, RTL8295_PTP_CTRL_PTP_S4P0_REG03_TX_PDELAY_RESP_SEQ_ID_OFFSET, RTL8295_PTP_CTRL_PTP_S4P0_REG03_TX_PDELAY_RESP_SEQ_ID_MASK);
        break;
      default:
        return RT_ERR_INPUT;
    }

    /* Input sequence ID NEED match currently sequence ID of chip */
    if (seqId != identifier.sequenceId)
    {
        ret = RT_ERR_INPUT;
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "unit=%u,port=%u mdxMacId %u sds %u seqId %u", unit, port, mdxMacId, sdsId, seqId);
        goto CLEAR_PEND;
    }

    /* Get sec */
    /* PTP_SXP0_REG10.PORT_SEC_B15_0 */
    PHY_8295_PTP_SXP0_REGN_READ(unit, mdxMacId, sdsId, RTL8295_PTP_SXP0_REG10, &time_l);
    /* PTP_SXP0_REG11.PORT_SEC_B31_16 */
    PHY_8295_PTP_SXP0_REGN_READ(unit, mdxMacId, sdsId, RTL8295_PTP_SXP0_REG11, &time_m);
    /* PTP_SXP0_REG13.PORT_SEC_B47_32 */
    PHY_8295_PTP_SXP0_REGN_READ(unit, mdxMacId, sdsId, RTL8295_PTP_SXP0_REG13, &time_h);

    pTimeStamp->sec = (REG32_FIELD_GET(time_m, RTL8295_PTP_CTRL_PTP_S4P0_REG11_PORT_SEC_B31_16_OFFSET, RTL8295_PTP_CTRL_PTP_S4P0_REG11_PORT_SEC_B31_16_MASK) << 16)
                     | REG32_FIELD_GET(time_l, RTL8295_PTP_CTRL_PTP_S4P0_REG10_PORT_SEC_B15_0_OFFSET, RTL8295_PTP_CTRL_PTP_S4P0_REG10_PORT_SEC_B15_0_MASK);

    pTimeStamp->sec |= (uint64)time_h << 32;

    /* Get nsec */
    /* PTP_SXP0_REG08.PORT_NSEC_B15_0 */
    PHY_8295_PTP_SXP0_REGN_READ(unit, mdxMacId, sdsId, RTL8295_PTP_SXP0_REG08, &time_l);
    /* PTP_SXP0_REG09.PORT_NSEC_B29_16 */
    PHY_8295_PTP_SXP0_REGN_READ(unit, mdxMacId, sdsId, RTL8295_PTP_SXP0_REG09, &time_h);
    pTimeStamp->nsec = (REG32_FIELD_GET(time_h, RTL8295_PTP_CTRL_PTP_S4P0_REG09_PORT_NSEC_B29_16_OFFSET, RTL8295_PTP_CTRL_PTP_S4P0_REG09_PORT_NSEC_B29_16_MASK) << 16)
                      | REG32_FIELD_GET(time_l, RTL8295_PTP_CTRL_PTP_S4P0_REG08_PORT_NSEC_B15_0_OFFSET, RTL8295_PTP_CTRL_PTP_S4P0_REG08_PORT_NSEC_B15_0_MASK);

CLEAR_PEND:
    /* Clear the pending status */
    /* PTP_SXP0_REG12 */
    PHY_8295_PTP_SXP0_REGN_READ(unit, mdxMacId, sdsId, RTL8295_PTP_SXP0_REG12, &data);
    switch (identifier.msgType)
    {
      case PTP_MSG_TYPE_SYNC:
        /* TX_SYNC */
        data = REG32_FIELD_SET(data, 1, RTL8295_PTP_CTRL_PTP_S4P0_REG12_TX_SYNC_OFFSET, RTL8295_PTP_CTRL_PTP_S4P0_REG12_TX_SYNC_MASK);
        break;
      case PTP_MSG_TYPE_DELAY_REQ:
        /* TX_DELAY_REQ */
        data = REG32_FIELD_SET(data, 1, RTL8295_PTP_CTRL_PTP_S4P0_REG12_TX_DELAY_REQ_OFFSET, RTL8295_PTP_CTRL_PTP_S4P0_REG12_TX_DELAY_REQ_MASK);
        break;
      case PTP_MSG_TYPE_PDELAY_REQ:
        /* TX_PDELAY_REQ */
        data = REG32_FIELD_SET(data, 1, RTL8295_PTP_CTRL_PTP_S4P0_REG12_TX_PDELAY_REQ_OFFSET, RTL8295_PTP_CTRL_PTP_S4P0_REG12_TX_PDELAY_REQ_MASK);
        break;
      case PTP_MSG_TYPE_PDELAY_RESP:
        /* TX_PDELAY_RESP */
        data = REG32_FIELD_SET(data, 1, RTL8295_PTP_CTRL_PTP_S4P0_REG12_TX_PDELAY_RESP_OFFSET, RTL8295_PTP_CTRL_PTP_S4P0_REG12_TX_PDELAY_RESP_MASK);
        break;
    }
    PHY_8295_PTP_SXP0_REGN_WRITE(unit, mdxMacId, sdsId, RTL8295_PTP_SXP0_REG12, data);

    return ret;
}
#endif

#if defined(CONFIG_SDK_RTL8295R)
/* Function Name:
 *      phy_8295r_ptpTxTimestamp_get
 * Description:
 *      Get PTP Tx timstamp according to the PTP identifier on the dedicated port from the specified device.
 * Input:
 *      unit       - unit id
 *      port       - port id
 *      identifier - indentifier of PTP packet
 * Output:
 *      pTimeStamp - pointer buffer of TIME timestamp
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
phy_8295r_ptpTxTimestamp_get(uint32 unit, rtk_port_t port,
    rtk_time_ptpIdentifier_t identifier, rtk_time_timeStamp_t *pTimeStamp)
{
    int32       ret = RT_ERR_OK;
    uint32      data, time_l, time_m, time_h;
    uint32      mdxMacId, sdsId;
    uint32      seqId;
    uint32      speed;

    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u", unit, port);
    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);

    PHY_8295_SDSID_GET(unit, port, &sdsId);

    if ((ret = phy_8295r_speed_get(unit, port, &speed)) != RT_ERR_OK)
    {
        return ret;
    }

    if (PORT_SPEED_10G == speed)
    {

        switch (identifier.msgType)
        {
          case PTP_MSG_TYPE_SYNC:
            PHY_8295_PHYREG_READ(unit, mdxMacId, RTL8295_PTP_CTRL_PTP_TG_REG00_PAGE, RTL8295_PTP_CTRL_PTP_TG_REG00_REG, &seqId);
            break;
          case PTP_MSG_TYPE_DELAY_REQ:
            PHY_8295_PHYREG_READ(unit, mdxMacId, RTL8295_PTP_CTRL_PTP_TG_REG01_PAGE, RTL8295_PTP_CTRL_PTP_TG_REG01_REG, &seqId);
            break;
          case PTP_MSG_TYPE_PDELAY_REQ:
            PHY_8295_PHYREG_READ(unit, mdxMacId, RTL8295_PTP_CTRL_PTP_TG_REG02_PAGE, RTL8295_PTP_CTRL_PTP_TG_REG02_REG, &seqId);
            break;
          case PTP_MSG_TYPE_PDELAY_RESP:
            PHY_8295_PHYREG_READ(unit, mdxMacId, RTL8295_PTP_CTRL_PTP_TG_REG03_PAGE, RTL8295_PTP_CTRL_PTP_TG_REG03_REG, &seqId);
            break;
          default:
            return RT_ERR_INPUT;
        }
    }
    else
    {
         switch (identifier.msgType)
        {
          case PTP_MSG_TYPE_SYNC:
            /* PTP_SXP0_REG00.TX_SYNC_SEQ_ID */
            PHY_8295_PTP_SXP0_REGN_READ(unit, mdxMacId, sdsId, RTL8295_PTP_SXP0_REG00, &data);
            seqId = REG32_FIELD_GET(data, RTL8295_PTP_CTRL_PTP_S4P0_REG00_TX_SYNC_SEQ_ID_OFFSET, RTL8295_PTP_CTRL_PTP_S4P0_REG00_TX_SYNC_SEQ_ID_MASK);
            break;
          case PTP_MSG_TYPE_DELAY_REQ:
            /* PTP_SXP0_REG01.TX_DELAY_REQ_SEQ_ID */
            PHY_8295_PTP_SXP0_REGN_READ(unit, mdxMacId, sdsId, RTL8295_PTP_SXP0_REG01, &data);
            seqId = REG32_FIELD_GET(data, RTL8295_PTP_CTRL_PTP_S4P0_REG01_TX_DELAY_REQ_SEQ_ID_OFFSET, RTL8295_PTP_CTRL_PTP_S4P0_REG01_TX_DELAY_REQ_SEQ_ID_MASK);
            break;
          case PTP_MSG_TYPE_PDELAY_REQ:
            /* PTP_SXP0_REG02.TX_PDELAY_REQ_SEQ_ID */
            PHY_8295_PTP_SXP0_REGN_READ(unit, mdxMacId, sdsId, RTL8295_PTP_SXP0_REG02, &data);
            seqId = REG32_FIELD_GET(data, RTL8295_PTP_CTRL_PTP_S4P0_REG02_TX_PDELAY_REQ_SEQ_ID_OFFSET, RTL8295_PTP_CTRL_PTP_S4P0_REG02_TX_PDELAY_REQ_SEQ_ID_MASK);
            break;
          case PTP_MSG_TYPE_PDELAY_RESP:
            /* PTP_SXP0_REG03.TX_PDELAY_RESP_SEQ_I D */
            PHY_8295_PTP_SXP0_REGN_READ(unit, mdxMacId, sdsId, RTL8295_PTP_SXP0_REG03, &data);
            seqId = REG32_FIELD_GET(data, RTL8295_PTP_CTRL_PTP_S4P0_REG03_TX_PDELAY_RESP_SEQ_ID_OFFSET, RTL8295_PTP_CTRL_PTP_S4P0_REG03_TX_PDELAY_RESP_SEQ_ID_MASK);
            break;
          default:
            return RT_ERR_INPUT;
        }
    }

    /* Input sequence ID NEED match currently sequence ID of chip */
    if (seqId != identifier.sequenceId)
    {
        ret = RT_ERR_INPUT;
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "unit=%u,port=%u mdxMacId %u sds %u seqId %u", unit, port, mdxMacId, sdsId, seqId);
        goto CLEAR_PEND;
    }

    if (PORT_SPEED_10G == speed)
    {

        /* Get sec */
        PHY_8295_PHYREG_READ(unit, mdxMacId, RTL8295_PTP_CTRL_PTP_TG_REG10_PAGE, RTL8295_PTP_CTRL_PTP_TG_REG10_REG, &time_l);
        PHY_8295_PHYREG_READ(unit, mdxMacId, RTL8295_PTP_CTRL_PTP_TG_REG11_PAGE, RTL8295_PTP_CTRL_PTP_TG_REG11_REG, &time_m);
        PHY_8295_PHYREG_READ(unit, mdxMacId, RTL8295_PKG_CTRL_PTP_TG_REG13_PAGE, RTL8295_PKG_CTRL_PTP_TG_REG13_REG, &time_h);
        pTimeStamp->sec = (uint64)time_l | ((uint64)time_m << 16) | ((uint64)time_h << 32);

        /* Get nsec */
        PHY_8295_PHYREG_READ(unit, mdxMacId, RTL8295_PTP_CTRL_PTP_TG_REG08_PAGE, RTL8295_PTP_CTRL_PTP_TG_REG08_REG, &time_l);
        PHY_8295_PHYREG_READ(unit, mdxMacId, RTL8295_PTP_CTRL_PTP_TG_REG09_PAGE, RTL8295_PTP_CTRL_PTP_TG_REG09_REG, &time_h);
        pTimeStamp->nsec= (time_h << 16) | time_l;
    }
    else
    {
        /* Get sec */
        /* PTP_SXP0_REG10.PORT_SEC_B15_0 */
        PHY_8295_PTP_SXP0_REGN_READ(unit, mdxMacId, sdsId, RTL8295_PTP_SXP0_REG10, &time_l);
        /* PTP_SXP0_REG11.PORT_SEC_B31_16 */
        PHY_8295_PTP_SXP0_REGN_READ(unit, mdxMacId, sdsId, RTL8295_PTP_SXP0_REG11, &time_m);
        /* PTP_SXP0_REG13.PORT_SEC_B47_32 */
        PHY_8295_PTP_SXP0_REGN_READ(unit, mdxMacId, sdsId, RTL8295_PTP_SXP0_REG13, &time_h);

        pTimeStamp->sec = (REG32_FIELD_GET(time_m, RTL8295_PTP_CTRL_PTP_S4P0_REG11_PORT_SEC_B31_16_OFFSET, RTL8295_PTP_CTRL_PTP_S4P0_REG11_PORT_SEC_B31_16_MASK) << 16)
                         | REG32_FIELD_GET(time_l, RTL8295_PTP_CTRL_PTP_S4P0_REG10_PORT_SEC_B15_0_OFFSET, RTL8295_PTP_CTRL_PTP_S4P0_REG10_PORT_SEC_B15_0_MASK);

        pTimeStamp->sec |= (uint64)time_h << 32;

        /* Get nsec */
        /* PTP_SXP0_REG08.PORT_NSEC_B15_0 */
        PHY_8295_PTP_SXP0_REGN_READ(unit, mdxMacId, sdsId, RTL8295_PTP_SXP0_REG08, &time_l);
        /* PTP_SXP0_REG09.PORT_NSEC_B29_16 */
        PHY_8295_PTP_SXP0_REGN_READ(unit, mdxMacId, sdsId, RTL8295_PTP_SXP0_REG09, &time_h);
        pTimeStamp->nsec = (REG32_FIELD_GET(time_h, RTL8295_PTP_CTRL_PTP_S4P0_REG09_PORT_NSEC_B29_16_OFFSET, RTL8295_PTP_CTRL_PTP_S4P0_REG09_PORT_NSEC_B29_16_MASK) << 16)
                          | REG32_FIELD_GET(time_l, RTL8295_PTP_CTRL_PTP_S4P0_REG08_PORT_NSEC_B15_0_OFFSET, RTL8295_PTP_CTRL_PTP_S4P0_REG08_PORT_NSEC_B15_0_MASK);
    }

CLEAR_PEND:
    /* Clear the pending status */

    if (PORT_SPEED_10G == speed)
    {
        PHY_8295_PHYREG_READ(unit, mdxMacId, RTL8295_PTP_CTRL_PTP_TG_REG12_PAGE, RTL8295_PTP_CTRL_PTP_TG_REG12_REG, &data);

        data &= ~0xFF;

        switch (identifier.msgType)
        {
          case PTP_MSG_TYPE_SYNC:
            /* TX_SYNC */
            data = REG32_FIELD_SET(data, 1, RTL8295_PTP_CTRL_PTP_TG_REG12_TX_SYNC_OFFSET, RTL8295_PTP_CTRL_PTP_TG_REG12_TX_SYNC_MASK);
            break;
          case PTP_MSG_TYPE_DELAY_REQ:
            /* TX_DELAY_REQ */
            data = REG32_FIELD_SET(data, 1, RTL8295_PTP_CTRL_PTP_TG_REG12_TX_DELAY_REQ_OFFSET, RTL8295_PTP_CTRL_PTP_TG_REG12_TX_DELAY_REQ_MASK);
            break;
          case PTP_MSG_TYPE_PDELAY_REQ:
            /* TX_PDELAY_REQ */
            data = REG32_FIELD_SET(data, 1, RTL8295_PTP_CTRL_PTP_TG_REG12_TX_PDELAY_REQ_OFFSET, RTL8295_PTP_CTRL_PTP_TG_REG12_TX_PDELAY_REQ_MASK);
            break;
          case PTP_MSG_TYPE_PDELAY_RESP:
            /* TX_PDELAY_RESP */
            data = REG32_FIELD_SET(data, 1, RTL8295_PTP_CTRL_PTP_TG_REG12_TX_PDELAY_RESP_OFFSET, RTL8295_PTP_CTRL_PTP_TG_REG12_TX_PDELAY_RESP_MASK);
            break;
        }

        PHY_8295_PHYREG_WRITE(unit, mdxMacId, RTL8295_PTP_CTRL_PTP_TG_REG12_PAGE, RTL8295_PTP_CTRL_PTP_TG_REG12_REG, data);
    }
    else
    {
        /* Clear the pending status */
        /* PTP_SXP0_REG12 */
        PHY_8295_PTP_SXP0_REGN_READ(unit, mdxMacId, sdsId, RTL8295_PTP_SXP0_REG12, &data);
        switch (identifier.msgType)
        {
          case PTP_MSG_TYPE_SYNC:
            /* TX_SYNC */
            data = REG32_FIELD_SET(data, 1, RTL8295_PTP_CTRL_PTP_S4P0_REG12_TX_SYNC_OFFSET, RTL8295_PTP_CTRL_PTP_S4P0_REG12_TX_SYNC_MASK);
            break;
          case PTP_MSG_TYPE_DELAY_REQ:
            /* TX_DELAY_REQ */
            data = REG32_FIELD_SET(data, 1, RTL8295_PTP_CTRL_PTP_S4P0_REG12_TX_DELAY_REQ_OFFSET, RTL8295_PTP_CTRL_PTP_S4P0_REG12_TX_DELAY_REQ_MASK);
            break;
          case PTP_MSG_TYPE_PDELAY_REQ:
            /* TX_PDELAY_REQ */
            data = REG32_FIELD_SET(data, 1, RTL8295_PTP_CTRL_PTP_S4P0_REG12_TX_PDELAY_REQ_OFFSET, RTL8295_PTP_CTRL_PTP_S4P0_REG12_TX_PDELAY_REQ_MASK);
            break;
          case PTP_MSG_TYPE_PDELAY_RESP:
            /* TX_PDELAY_RESP */
            data = REG32_FIELD_SET(data, 1, RTL8295_PTP_CTRL_PTP_S4P0_REG12_TX_PDELAY_RESP_OFFSET, RTL8295_PTP_CTRL_PTP_S4P0_REG12_TX_PDELAY_RESP_MASK);
            break;
        }
        PHY_8295_PTP_SXP0_REGN_WRITE(unit, mdxMacId, sdsId, RTL8295_PTP_SXP0_REG12, data);
    }

    return ret;
}
#endif

#if defined(CONFIG_SDK_RTL8295R) || defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8295_8214_ptpRefTimeFreq_get
 * Description:
 *      Get the frequency of reference time of PHY of the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pFreq  - pointer to reference time frequency
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The frequency configuration decides the reference time tick frequency.
 *      The default value is 0x8000000.
 *      If it is configured to 0x4000000, the tick frequency would be half of default.
 *      If it is configured to 0xC000000, the tick frequency would be one and half times of default.
 */
int32
phy_8295_8214_ptpRefTimeFreq_get(uint32 unit, rtk_port_t port, uint32 *pFreq)
{
    uint32 reg_val;
    uint32 mdxMacId;

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);
    PHY_8295_PHYREG_READ(unit, mdxMacId, RTL8295_PTP_CTRL_PTP_TIME_REG17_PAGE, RTL8295_PTP_CTRL_PTP_TIME_REG17_REG, &reg_val);


    *pFreq = reg_val;

    PHY_8295_PHYREG_READ(unit, mdxMacId, RTL8295_PTP_CTRL_PTP_TIME_REG18_PAGE, RTL8295_PTP_CTRL_PTP_TIME_REG18_REG, &reg_val);

    *pFreq |= reg_val << 16;

    return RT_ERR_OK;
} /* end of phy_8295_8214_ptpRefTimeFreq_get */
#endif

#if defined(CONFIG_SDK_RTL8295R) || defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8295_8214_ptpRefTimeFreq_set
 * Description:
 *      Set the frequency of reference time of PHY of the specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      freq   - reference time frequency
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      The frequency configuration decides the reference time tick frequency.
 *      The default value is 0x8000000.
 *      If it is configured to 0x4000000, the tick frequency would be half of default.
 *      If it is configured to 0xC000000, the tick frequency would be one and half times of default.
 */
int32
phy_8295_8214_ptpRefTimeFreq_set(uint32 unit, rtk_port_t port, uint32 freq)
{
    uint32 mdxMacId;

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);
    PHY_8295_PHYREG_WRITE(unit, mdxMacId, RTL8295_PTP_CTRL_PTP_TIME_REG17_PAGE, RTL8295_PTP_CTRL_PTP_TIME_REG17_REG, freq);
    PHY_8295_PHYREG_WRITE(unit, mdxMacId, RTL8295_PTP_CTRL_PTP_TIME_REG18_PAGE, RTL8295_PTP_CTRL_PTP_TIME_REG18_REG, (freq>>16));
    return RT_ERR_OK;
} /* end of phy_8295_8214_ptpRefTimeFreq_set */
#endif

#if defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8214qf_ptpInterruptStatus_get
 * Description:
 *      Get PTP interrupt status of the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pIsPortIntr - port interrupt triggered status
 *      pIntrSts - interrupt status of RX/TX PTP frame types
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT         - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
phy_8214qf_ptpInterruptStatus_get(uint32 unit, rtk_port_t port, uint32 *pIsPortIntr, uint32 *pIntrSts)
{
    uint32 reg_val;
    uint32 mdxMacId;
    uint32 sdsId;

    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u", unit, port);

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);
    PHY_8295_PHYREG_READ(unit, mdxMacId, RTL8295_PTP_CTRL_PTP_TIME_REG15_PAGE, RTL8295_PTP_CTRL_PTP_TIME_REG15_REG, &reg_val);

    if (reg_val & (1 << (port % 8)))
    {
        *pIsPortIntr = ENABLED;
    } else {
        *pIsPortIntr = DISABLED;
    }

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);

    PHY_8295_SDSID_GET(unit, port, &sdsId);

    /* PTP_SXP0_REG12 */
    PHY_8295_PTP_SXP0_REGN_READ(unit, mdxMacId, sdsId, RTL8295_PTP_SXP0_REG12, &reg_val);

    *pIntrSts = reg_val & 0xFF;

    PHY_8295_PTP_SXP0_REGN_WRITE(unit, mdxMacId, sdsId, RTL8295_PTP_SXP0_REG12, reg_val);

    return RT_ERR_OK;
}
#endif

#if defined(CONFIG_SDK_RTL8295R)
/* Function Name:
 *      phy_8295r_ptpInterruptStatus_get
 * Description:
 *      Get PTP interrupt status of the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pIsPortIntr - port interrupt triggered status
 *      pIntrSts - interrupt status of RX/TX PTP frame types
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT         - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
phy_8295r_ptpInterruptStatus_get(uint32 unit, rtk_port_t port, uint32 *pIsPortIntr, uint32 *pIntrSts)
{
    uint32 reg_val;
    uint32 mdxMacId;
    uint32 sdsId;
    uint32 speed;
    int32 ret;
    rtk_port_t intr_port;

    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u", unit, port);

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);
    PHY_8295_PHYREG_READ(unit, mdxMacId, RTL8295_PTP_CTRL_PTP_TIME_REG15_PAGE, RTL8295_PTP_CTRL_PTP_TIME_REG15_REG, &reg_val);

    if ((ret = phy_8295r_speed_get(unit, port, &speed)) != RT_ERR_OK)
    {
        return ret;
    }

    if (PORT_SPEED_10G == speed)
        intr_port = 4;
    else
        intr_port = port;

    if (reg_val & (1 << (intr_port % 8)))
    {
        *pIsPortIntr = ENABLED;
    } else {
        *pIsPortIntr = DISABLED;
    }

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);

    PHY_8295_SDSID_GET(unit, port, &sdsId);

    if(PORT_SPEED_10G == speed)
    {
        PHY_8295_PHYREG_READ(unit, mdxMacId, RTL8295_PTP_CTRL_PTP_TG_REG12_PAGE, RTL8295_PTP_CTRL_PTP_TG_REG12_REG, &reg_val);
        *pIntrSts = reg_val & 0xFF;
        PHY_8295_PHYREG_WRITE(unit, mdxMacId, RTL8295_PTP_CTRL_PTP_TG_REG12_PAGE, RTL8295_PTP_CTRL_PTP_TG_REG12_REG, reg_val);
    }
    else
    {
        /* PTP_SXP0_REG12 */
        PHY_8295_PTP_SXP0_REGN_READ(unit, mdxMacId, sdsId, RTL8295_PTP_SXP0_REG12, &reg_val);
        *pIntrSts = reg_val & 0xFF;
        PHY_8295_PTP_SXP0_REGN_WRITE(unit, mdxMacId, sdsId, RTL8295_PTP_SXP0_REG12, reg_val);
    }

    return RT_ERR_OK;
}
#endif

#if defined(CONFIG_SDK_RTL8295R) || defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8295_8214_ptpInterruptEnable_get
 * Description:
 *      Get PTP interrupt enable status of the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT         - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
phy_8295_8214_ptpInterruptEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    uint32 reg_val;
    uint32 mdxMacId;

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);
    PHY_8295_PHYREG_READ(unit, mdxMacId, RTL8295_PTP_CTRL_PTP_TIME_REG14_PAGE, RTL8295_PTP_CTRL_PTP_TIME_REG14_REG, &reg_val);

    if (reg_val & 0xFF)
    {
        *pEnable = ENABLED;
    } else {
        *pEnable = DISABLED;
    }

    return RT_ERR_OK;
}
#endif

#if defined(CONFIG_SDK_RTL8295R) || defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8295_8214_ptpInterruptEnable_set
 * Description:
 *      Set PTP interrupt enable status of the specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT     - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
phy_8295_8214_ptpInterruptEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    uint32 reg_val;
    uint32 mdxMacId;

    reg_val = (enable == ENABLED)? 0xFF  : 0;

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);
    PHY_8295_PHYREG_MSK_WRITE(unit, mdxMacId, RTL8295_PTP_CTRL_PTP_TIME_REG14_PAGE, RTL8295_PTP_CTRL_PTP_TIME_REG14_REG, reg_val, 0xFF);

    return RT_ERR_OK;
}
#endif

#if defined(CONFIG_SDK_RTL8295R) || defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      _phy_8295_8214_ptpIgrTpidPageReg_get
 * Description:
 *      Get phy page and reg of PTP IGR TPID register
 * Input:
 *      type  -  vlan type
 *      tpid_idx - TPID index
 * Output:
 *      phyPage - register's phy page
 *      phyReg - register's phy reg
 * Return:
 *      RT_ERR_OK
 * Note:
 *      None
 */
int32
_phy_8295_8214_ptpIgrTpidPageReg_get(rtk_vlanType_t type, uint32 tpid_idx, uint32 *phyPage, uint32 *phyReg)
{

    if (INNER_VLAN == type)
    {
        switch(tpid_idx)
        {
            case 0:
                *phyPage = RTL8295_PTP_CTRL_PTP_TIME_REG09_PAGE;
                *phyReg  = RTL8295_PTP_CTRL_PTP_TIME_REG09_REG;
                break;
            case 1:
                *phyPage = RTL8295_PKG_CTRL_PTP_TIME_REG25_PAGE;
                *phyReg  = RTL8295_PKG_CTRL_PTP_TIME_REG25_REG;
                break;
            case 2:
                *phyPage = RTL8295_PKG_CTRL_PTP_TIME_REG27_PAGE;
                *phyReg  = RTL8295_PKG_CTRL_PTP_TIME_REG27_REG;
                break;
            case 3:
                *phyPage = RTL8295_PKG_CTRL_PTP_TIME_REG29_PAGE;
                *phyReg  = RTL8295_PKG_CTRL_PTP_TIME_REG29_REG;
                break;
           default:
                *phyPage = RTL8295_PTP_CTRL_PTP_TIME_REG09_PAGE;
                *phyReg  = RTL8295_PTP_CTRL_PTP_TIME_REG09_REG;
                break;
        }
    }
    else
    {
        switch(tpid_idx)
        {
            case 0:
                *phyPage = RTL8295_PTP_CTRL_PTP_TIME_REG08_PAGE;
                *phyReg  = RTL8295_PTP_CTRL_PTP_TIME_REG08_REG;
                break;
            case 1:
                *phyPage = RTL8295_PKG_CTRL_PTP_TIME_REG24_PAGE;
                *phyReg  = RTL8295_PKG_CTRL_PTP_TIME_REG24_REG;
                break;
            case 2:
                *phyPage = RTL8295_PKG_CTRL_PTP_TIME_REG26_PAGE;
                *phyReg  = RTL8295_PKG_CTRL_PTP_TIME_REG26_REG;
                break;
            case 3:
                *phyPage = RTL8295_PKG_CTRL_PTP_TIME_REG28_PAGE;
                *phyReg  = RTL8295_PKG_CTRL_PTP_TIME_REG28_REG;
                break;
           default:
                *phyPage = RTL8295_PTP_CTRL_PTP_TIME_REG08_PAGE;
                *phyReg  = RTL8295_PTP_CTRL_PTP_TIME_REG08_REG;
                break;
        }
    }
    return RT_ERR_OK;
}
#endif

#if defined(CONFIG_SDK_RTL8295R) || defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8295_8214_ptpIgrTpid_get
 * Description:
 *      Get inner/outer TPID of the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      type  -  vlan type
 *      tpid_idx - TPID index
 * Output:
 *      pTpid   - TPID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT         - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
phy_8295_8214_ptpIgrTpid_get(uint32 unit, rtk_port_t port, rtk_vlanType_t type, uint32 tpid_idx, uint32 *pTpid)
{
    uint32 phyPage, phyReg, reg_val;
    uint32 mdxMacId;

    if (_phy_8295_8214_ptpIgrTpidPageReg_get(type, tpid_idx, &phyPage, &phyReg) != RT_ERR_OK)
        return RT_ERR_FAILED;

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);
    PHY_8295_PHYREG_READ(unit, mdxMacId, phyPage, phyReg, &reg_val);
    *pTpid = reg_val;

    return RT_ERR_OK;
}
#endif

#if defined(CONFIG_SDK_RTL8295R) || defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8295_8214_ptpIgrTpid_set
 * Description:
 *      Set inner/outer TPID of the specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      type  -  vlan type
 *      tpid_idx - TPID index
 *      tpid   - TPID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT     - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
phy_8295_8214_ptpIgrTpid_set(uint32 unit, rtk_port_t port, rtk_vlanType_t type, uint32 tpid_idx, uint32 tpid)
{
    uint32 phyPage, phyReg, reg_val;
    uint32 mdxMacId;

    if (_phy_8295_8214_ptpIgrTpidPageReg_get(type, tpid_idx, &phyPage, &phyReg) != RT_ERR_OK)
        return RT_ERR_FAILED;

    reg_val = tpid;

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);
    PHY_8295_PHYREG_WRITE(unit, mdxMacId, phyPage, phyReg, reg_val);

    return RT_ERR_OK;
}
#endif

#if defined(CONFIG_SDK_RTL8295R) || defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8295_8214_ptpSwitchMacRange_get
 * Description:
 *      Get MAC address range of the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pRange - pointer to MAC address range
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT         - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
phy_8295_8214_ptpSwitchMacRange_get(uint32 unit, rtk_port_t port, uint32 *pRange)
{
    uint32 reg_val;
    uint32 mdxMacId;

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);
    PHY_8295_PHYREG_READ(unit, mdxMacId, RTL8295_PTP_CTRL_PTP_TIME_REG04_PAGE, RTL8295_PTP_CTRL_PTP_TIME_REG04_REG, &reg_val);

    *pRange =  (reg_val >> 6) & 0x3FF;

    return RT_ERR_OK;
}
#endif

#if defined(CONFIG_SDK_RTL8295R) || defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8295_8214_ptpSwitchMacRange_set
 * Description:
 *      Set MAC address range of the specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      pMac  -  pointer to MAC address
 *      range - MAC address range
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT     - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
phy_8295_8214_ptpSwitchMacRange_set(uint32 unit, rtk_port_t port, uint32 range)
{
    uint32 reg_val;
    uint32 mdxMacId;

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);

    reg_val =  range << 6;
    PHY_8295_PHYREG_MSK_WRITE(unit, mdxMacId, RTL8295_PTP_CTRL_PTP_TIME_REG04_PAGE, RTL8295_PTP_CTRL_PTP_TIME_REG04_REG, reg_val, RTL8295_PTP_CTRL_PTP_TIME_REG04_MAC_RANGE_MASK);

    return RT_ERR_OK;
}
#endif

#if defined(CONFIG_SDK_RTL8295R)
/* Function Name:
 *      phy_8295r_loopback_get
 * Description:
 *      Get PHY Loopback mode of the specific port
 * Input:
 *      unit                - unit id
 *      port                - port id
 * Output:
 *      pEnable             - loopback mode status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8295r_loopback_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    uint32          mdxMacId, sdsId;
    uint32          data;
    rtk_enable_t    enable1g;

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);

    PHY_8295_SDSID_GET(unit, port, &sdsId);

    /* 10G */
    PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_TGR_PRO_0_REG01_PAGE, RTL8295_SDS0_TGR_PRO_0_REG01_REG, &data);
    *pEnable = (REG32_FIELD_GET(data, RTL8295_SDS0_TGR_PRO_0_REG01_FP_TGR1_CFG_DIG_LPK_OFFSET, RTL8295_SDS0_TGR_PRO_0_REG01_FP_TGR1_CFG_DIG_LPK_MASK) == 1) ? ENABLED : DISABLED;

    /* 1G */
    PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_SDS_REG00_PAGE, RTL8295_SDS0_SDS_REG00_REG, &data);
    enable1g = (REG32_FIELD_GET(data, RTL8295_SDS0_SDS_REG00_SP_CFG_DIG_LPK_OFFSET, RTL8295_SDS0_SDS_REG00_SP_CFG_DIG_LPK_MASK) == 1) ? ENABLED : DISABLED;
    if (*pEnable != enable1g)
    {
        RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u, 10gEn:%d 1gEn:%d no match!", unit, port, *pEnable, enable1g);
    }

    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u, enable:%d", unit, port, *pEnable);
    return RT_ERR_OK;
}
#endif

#if defined(CONFIG_SDK_RTL8295R)
/* Function Name:
 *      phy_8295r_loopback_set
 * Description:
 *      Set PHY Loopback mode of the specific port
 * Input:
 *      unit                - unit id
 *      port                - port id
 *      enable              - ENABLED: Enable loopback;
 *                            DISABLED: Disable loopback. PHY back to normal operation.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      If the polarity is inversed, need to change it back to "normal" state before perform loopback function.
 */
int32
phy_8295r_loopback_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    uint32          mdxMacId, sdsId;
    uint32          data, val, old_val;
    uint32          s0_mode, s1_mode;

    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,enable=%d", unit, port, enable);


    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);

    PHY_8295_SDSID_GET(unit, port, &sdsId);

    if (enable == ENABLED)
        val = 1;
    else
        val = 0;

    /* 10G */
    PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_TGR_PRO_0_REG01_PAGE, RTL8295_SDS0_TGR_PRO_0_REG01_REG, &data);
    old_val = REG32_FIELD_GET(data, RTL8295_SDS0_TGR_PRO_0_REG01_FP_TGR1_CFG_DIG_LPK_OFFSET, RTL8295_SDS0_TGR_PRO_0_REG01_FP_TGR1_CFG_DIG_LPK_MASK);

    if ((old_val != val) && (val == 0))
    {
        /* when change loopback state to disable, clear portReady status for rx-cali */
        switch (PORT_10G_MEDIA(unit, port))
        {
          case PORT_10GMEDIA_FIBER_10G:
          case PORT_10GMEDIA_DAC_50CM:
          case PORT_10GMEDIA_DAC_100CM:
          case PORT_10GMEDIA_DAC_300CM:
          case PORT_10GMEDIA_DAC_500CM: //DAC_LONG
          case PORT_10GMEDIA_NONE:
            RTK_PORTMASK_PORT_CLEAR(g8295rInfo[unit]->port10gReady, port);
            _switch_port_macForceLinkDown_enable(unit, port, ENABLED);
            break;
          default:
            break;
        }
    }

    data = REG32_FIELD_SET(data, val, RTL8295_SDS0_TGR_PRO_0_REG01_FP_TGR1_CFG_DIG_LPK_OFFSET, RTL8295_SDS0_TGR_PRO_0_REG01_FP_TGR1_CFG_DIG_LPK_MASK);
    PHY_8295_SDSREG_WRITE(unit, mdxMacId, sdsId, RTL8295_SDS0_TGR_PRO_0_REG01_PAGE, RTL8295_SDS0_TGR_PRO_0_REG01_REG, data);

    /* 1G */
    PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_SDS_REG00_PAGE, RTL8295_SDS0_SDS_REG00_REG, &data);
    data = REG32_FIELD_SET(data, val, RTL8295_SDS0_SDS_REG00_SP_CFG_DIG_LPK_OFFSET, RTL8295_SDS0_SDS_REG00_SP_CFG_DIG_LPK_MASK);
    PHY_8295_SDSREG_WRITE(unit, mdxMacId, sdsId, RTL8295_SDS0_SDS_REG00_PAGE, RTL8295_SDS0_SDS_REG00_REG, data);

    _phy_8295_serdesModeReg_get(unit, port, mdxMacId, 0, &s0_mode);
    _phy_8295_serdesModeReg_get(unit, port, mdxMacId, 1, &s1_mode);
    _phy_8295_serdesModeReg_set(unit, port, mdxMacId, 0, PHY_8295_SDS_MODE_OFF);
    _phy_8295_serdesModeReg_set(unit, port, mdxMacId, 1, PHY_8295_SDS_MODE_OFF);
    _phy_8295_serdesModeReg_set(unit, port, mdxMacId, 0, s0_mode);
    _phy_8295_serdesModeReg_set(unit, port, mdxMacId, 1, s1_mode);

     _phy_8295r_serdes_reset(unit, port);

    return RT_ERR_OK;
}
#endif


#if defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8214qf_loopback_set
 * Description:
 *      Set PHY Loopback mode of the specific port
 * Input:
 *      unit                - unit id
 *      port                - port id
 *      enable              - ENABLED: Enable loopback;
 *                            DISABLED: Disable loopback. PHY back to normal operation.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      If the polarity is inversed, need to change it back to "normal" state before perform loopback function.
 */
int32
phy_8214qf_loopback_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32           ret;

    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,enable=%d", unit, port, enable);

    /* set power-down to disable analog TX to bring peer's link status to down when loopback set to enable */
    if (enable == ENABLED)
    {
        phy_common_enable_set(unit, port, DISABLED);
    }

    /* set loopback */
    if ((ret = phy_common_loopback_set(unit, port, enable)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Recover power-down state when loopback set to disable */
    if (enable == DISABLED)
    {
        if (PORT_ENABLE_STATE(unit, port) == ENABLED)
        {
            phy_common_enable_set(unit, port, ENABLED);
        }
    }

    _phy_8214qf_serdes_reset(unit, port);
    return RT_ERR_OK;
}
#endif


#if defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8224qf_loopback_set
 * Description:
 *      Set PHY Loopback mode of the specific port
 * Input:
 *      unit                - unit id
 *      port                - port id
 *      enable              - ENABLED: Enable loopback;
 *                            DISABLED: Disable loopback. PHY back to normal operation.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      If the polarity is inversed, need to change it back to "normal" state before perform loopback function.
 */
int32
phy_8224qf_loopback_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32           ret;

    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,enable=%d", unit, port, enable);

    /* set power-down to disable analog TX to bring peer's link status to down when loopback set to enable */
    if (enable == ENABLED)
    {
        phy_common_enable_set(unit, port, DISABLED);
    }

    /* set loopback */
    if ((ret = phy_common_loopback_set(unit, port, enable)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Recover power-down state when loopback set to disable */
    if (enable == DISABLED)
    {
        if (PORT_ENABLE_STATE(unit, port) == ENABLED)
        {
            phy_common_enable_set(unit, port, ENABLED);
        }
    }

    _phy_8214qf_serdes_reset(unit, port);
    return RT_ERR_OK;
}
#endif

#if defined(CONFIG_SDK_RTL8295R) || defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8295_8214_fiberTxDis_set
 * Description:
 *      Set PHY Tx disable signal of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      enable  - ENABLED: Enable Tx disable signal;
 *                DISABLED: Disable Tx disable signal.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8295_8214_fiberTxDis_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    uint32              mdxMacId, sdsId;
    uint32              data, val;

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);

    PHY_8295_SDSID_GET(unit, port, &sdsId);

    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,EN=%d", unit, port, enable);
    PHY_8295_PHYREG_READ(unit, mdxMacId, RTL8295_GLB_CTRL_TX_DIS_PAGE, RTL8295_GLB_CTRL_TX_DIS_REG, &data);
    if (enable == ENABLED)
        val = 0;
    else
        val = 1;

    switch (sdsId)
    {
      case 1:
        //data = REG32_FIELD_SET(data, val, RTL8295_GLB_CTRL_TX_DIS_TX_DIS1_GPI_EN_OFFSET, RTL8295_GLB_CTRL_TX_DIS_TX_DIS1_GPI_EN_MASK);
        data = REG32_FIELD_SET(data, val, RTL8295_GLB_CTRL_TX_DIS_TX_DIS4_GPI_EN_OFFSET, RTL8295_GLB_CTRL_TX_DIS_TX_DIS4_GPI_EN_MASK);
        break;
      case 4:
        data = REG32_FIELD_SET(data, val, RTL8295_GLB_CTRL_TX_DIS_TX_DIS4_GPI_EN_OFFSET, RTL8295_GLB_CTRL_TX_DIS_TX_DIS4_GPI_EN_MASK);
        break;
      case 5:
        data = REG32_FIELD_SET(data, val, RTL8295_GLB_CTRL_TX_DIS_TX_DIS5_GPI_EN_OFFSET, RTL8295_GLB_CTRL_TX_DIS_TX_DIS5_GPI_EN_MASK);
        break;
      case 6:
        data = REG32_FIELD_SET(data, val, RTL8295_GLB_CTRL_TX_DIS_TX_DIS6_GPI_EN_OFFSET, RTL8295_GLB_CTRL_TX_DIS_TX_DIS6_GPI_EN_MASK);
        break;
      case 7:
        data = REG32_FIELD_SET(data, val, RTL8295_GLB_CTRL_TX_DIS_TX_DIS7_GPI_EN_OFFSET, RTL8295_GLB_CTRL_TX_DIS_TX_DIS7_GPI_EN_MASK);
        break;
      default:
        return RT_ERR_PORT_ID;
    }/* end switch */
    PHY_8295_PHYREG_WRITE(unit, mdxMacId, RTL8295_GLB_CTRL_TX_DIS_PAGE, RTL8295_GLB_CTRL_TX_DIS_REG, data);
    return RT_ERR_OK;
}
#endif


#if defined(CONFIG_SDK_RTL8295R) || defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8295_8214_fiberTxDisPin_set
 * Description:
 *      Set PHY Tx disable signal of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      data   - GPO pin value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8295_8214_fiberTxDisPin_set(uint32 unit, rtk_port_t port, uint32 data)
{
    uint32              mdxMacId, sdsId;
    uint32              reg_data;

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);

    PHY_8295_SDSID_GET(unit, port, &sdsId);

    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,sds=%u,data=%u", unit, port, sdsId, data);
    PHY_8295_PHYREG_READ(unit, mdxMacId, RTL8295_GLB_CTRL_TX_DIS_PAGE, RTL8295_GLB_CTRL_TX_DIS_REG, &reg_data);
    switch (sdsId)
    {
      case 1:
        //reg_data = REG32_FIELD_SET(reg_data, data, RTL8295_GLB_CTRL_TX_DIS_TX_DIS1_OFFSET, RTL8295_GLB_CTRL_TX_DIS_TX_DIS1_MASK);
        reg_data = REG32_FIELD_SET(reg_data, data, RTL8295_GLB_CTRL_TX_DIS_TX_DIS4_OFFSET, RTL8295_GLB_CTRL_TX_DIS_TX_DIS4_MASK);
        break;
      case 4:
        reg_data = REG32_FIELD_SET(reg_data, data, RTL8295_GLB_CTRL_TX_DIS_TX_DIS4_OFFSET, RTL8295_GLB_CTRL_TX_DIS_TX_DIS4_MASK);
        break;
      case 5:
        reg_data = REG32_FIELD_SET(reg_data, data, RTL8295_GLB_CTRL_TX_DIS_TX_DIS5_OFFSET, RTL8295_GLB_CTRL_TX_DIS_TX_DIS5_MASK);
        break;
      case 6:
        reg_data = REG32_FIELD_SET(reg_data, data, RTL8295_GLB_CTRL_TX_DIS_TX_DIS6_OFFSET, RTL8295_GLB_CTRL_TX_DIS_TX_DIS6_MASK);
        break;
      case 7:
        reg_data = REG32_FIELD_SET(reg_data, data, RTL8295_GLB_CTRL_TX_DIS_TX_DIS7_OFFSET, RTL8295_GLB_CTRL_TX_DIS_TX_DIS7_MASK);
        break;
      default:
        return RT_ERR_PORT_ID;
    }/* end switch */
    PHY_8295_PHYREG_WRITE(unit, mdxMacId, RTL8295_GLB_CTRL_TX_DIS_PAGE, RTL8295_GLB_CTRL_TX_DIS_REG, reg_data);
    return RT_ERR_OK;
}
#endif

#if defined(CONFIG_SDK_RTL8295R) || defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8295_8214_fiberRxEnable_get
 * Description:
 *      Get fiber Rx enable status of the specific port
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pEnable - fiber Rx enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      None
 */
int32
phy_8295_8214_fiberRxEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    uint32              mdxMacId, sdsId;
    uint32              data, val;


    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);

    PHY_8295_SDSID_GET(unit, port, &sdsId);

    PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_ANA_MISC_REG02_PAGE, RTL8295_SDS0_ANA_MISC_REG02_REG, &data);
    val = REG32_FIELD_GET(data, RTL8295_SDS0_ANA_MISC_REG02_FRC_BER_NOTIFY_ON_OFFSET, RTL8295_SDS0_ANA_MISC_REG02_FRC_BER_NOTIFY_ON_MASK);
    if (val == 1)
        *pEnable = DISABLED;
    else
        *pEnable = ENABLED;

    return RT_ERR_OK;
}
#endif

#if defined(CONFIG_SDK_RTL8295R) || defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8295_8214_fiberRxEnable_set
 * Description:
 *      Set fiber Rx enable status of the specific port
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - fiber Rx enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      None
 */
int32
phy_8295_8214_fiberRxEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    uint32              mdxMacId, sdsId;
    uint32              data;

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);

    PHY_8295_SDSID_GET(unit, port, &sdsId);

    if (enable == DISABLED)
    {
        /* Disable RX */
        /* 10G */
        if (sdsId == 1)
        {
            PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_TGR_PRO_0_REG00_PAGE, RTL8295_SDS0_TGR_PRO_0_REG00_REG, &data);
            /* set FP_TGR0_SEL_BER_NOTIFY 0 */
            data = REG32_FIELD_SET(data, 0, RTL8295_SDS0_TGR_PRO_0_REG00_FP_TGR0_SEL_BER_NOTIFY_OFFSET, RTL8295_SDS0_TGR_PRO_0_REG00_FP_TGR0_SEL_BER_NOTIFY_MASK);
            PHY_8295_SDSREG_WRITE(unit, mdxMacId, sdsId, RTL8295_SDS0_TGR_PRO_0_REG00_PAGE, RTL8295_SDS0_TGR_PRO_0_REG00_REG, data);
        }

        /* 1.25G/5G/125M */
        PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_SDS_REG14_PAGE, RTL8295_SDS0_SDS_REG14_REG, &data);
        data = REG32_FIELD_SET(data, 1, RTL8295_SDS0_SDS_REG14_SP_SEL_CALIBOK_OFFSET, RTL8295_SDS0_SDS_REG14_SP_SEL_CALIBOK_MASK);
        PHY_8295_SDSREG_WRITE(unit, mdxMacId, sdsId, RTL8295_SDS0_SDS_REG14_PAGE, RTL8295_SDS0_SDS_REG14_REG, data);

        PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_ANA_MISC_REG02_PAGE, RTL8295_SDS0_ANA_MISC_REG02_REG, &data);
        data = REG32_FIELD_SET(data, 1, RTL8295_SDS0_ANA_MISC_REG02_FRC_BER_NOTIFY_ON_OFFSET, RTL8295_SDS0_ANA_MISC_REG02_FRC_BER_NOTIFY_ON_MASK);
        PHY_8295_SDSREG_WRITE(unit, mdxMacId, sdsId, RTL8295_SDS0_ANA_MISC_REG02_PAGE, RTL8295_SDS0_ANA_MISC_REG02_REG, data);
    }
    else
    {
        /* Enable Rx  */
        PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_ANA_MISC_REG02_PAGE, RTL8295_SDS0_ANA_MISC_REG02_REG, &data);
        data = REG32_FIELD_SET(data, 0, RTL8295_SDS0_ANA_MISC_REG02_FRC_BER_NOTIFY_ON_OFFSET, RTL8295_SDS0_ANA_MISC_REG02_FRC_BER_NOTIFY_ON_MASK);
        PHY_8295_SDSREG_WRITE(unit, mdxMacId, sdsId, RTL8295_SDS0_ANA_MISC_REG02_PAGE, RTL8295_SDS0_ANA_MISC_REG02_REG, data);

        /* 1.25G/5G/125M */
        PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_SDS_REG14_PAGE, RTL8295_SDS0_SDS_REG14_REG, &data);
        data = REG32_FIELD_SET(data, 0, RTL8295_SDS0_SDS_REG14_SP_SEL_CALIBOK_OFFSET, RTL8295_SDS0_SDS_REG14_SP_SEL_CALIBOK_MASK);
        PHY_8295_SDSREG_WRITE(unit, mdxMacId, sdsId, RTL8295_SDS0_SDS_REG14_PAGE, RTL8295_SDS0_SDS_REG14_REG, data);

        /* 10G */
        if (sdsId == 1)
        {
            PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_TGR_PRO_0_REG00_PAGE, RTL8295_SDS0_TGR_PRO_0_REG00_REG, &data);
            /* set FP_TGR0_SEL_BER_NOTIFY 0 */
            data = REG32_FIELD_SET(data, 0, RTL8295_SDS0_TGR_PRO_0_REG00_FP_TGR0_SEL_BER_NOTIFY_OFFSET, RTL8295_SDS0_TGR_PRO_0_REG00_FP_TGR0_SEL_BER_NOTIFY_MASK);
            PHY_8295_SDSREG_WRITE(unit, mdxMacId, sdsId, RTL8295_SDS0_TGR_PRO_0_REG00_PAGE, RTL8295_SDS0_TGR_PRO_0_REG00_REG, data);
        }
    }

    return RT_ERR_OK;
}
#endif


#if defined(CONFIG_SDK_RTL8295R)
/* Function Name:
 *      phy_8295r_fiberRxEnable_set
 * Description:
 *      Set fiber Rx enable status of the specific port
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - fiber Rx enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      None
 */
int32
phy_8295r_fiberRxEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32       ret;
    uint32      mdxMacId, sdsId;
    uint32      data10g = 0, data1p25g = 0, sdsCtrlData, sdsCtrlDataTmp;
    uint32      rxEnSelfOffset = RTL8295_SDS0_ANA_SPD_10P3125G_REG21_REG0_RX_EN_SELF_OFFSET, rxDisOffset = RTL8295_SDS_CTRL_SDS_CTRL_S1_RX_DISABLE_S1_OFFSET;

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);
    PHY_8295_SDSID_GET(unit, port, &sdsId);
    if (enable == DISABLED)
    {
        PHY_8295_PHYREG_READ(unit, mdxMacId, RTL8295_SDS_CTRL_SDS_CTRL_S1_PAGE, RTL8295_SDS_CTRL_SDS_CTRL_S1_REG, &sdsCtrlData);
        PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_ANA_SPD_1P25G_REG21_PAGE, RTL8295_SDS0_ANA_SPD_1P25G_REG21_REG, &data1p25g);
        PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_ANA_SPD_10P3125G_REG21_PAGE, RTL8295_SDS0_ANA_SPD_10P3125G_REG21_REG, &data10g);
        /* RX_DISABLE=1 */
        sdsCtrlDataTmp = REG32_FIELD_SET(sdsCtrlData, 1, rxDisOffset, (0x1 << rxDisOffset));
        PHY_8295_PHYREG_WRITE(unit, mdxMacId, RTL8295_SDS_CTRL_SDS_CTRL_S1_PAGE, RTL8295_SDS_CTRL_SDS_CTRL_S1_REG, sdsCtrlDataTmp);
        /* RX_ENSELF=1 */
        data10g = REG32_FIELD_SET(data10g, 1, rxEnSelfOffset, (0x1 << rxEnSelfOffset));
        PHY_8295_SDSREG_WRITE(unit, mdxMacId, sdsId, RTL8295_SDS0_ANA_SPD_10P3125G_REG21_PAGE, RTL8295_SDS0_ANA_SPD_10P3125G_REG21_REG, data10g);
        data1p25g = REG32_FIELD_SET(data1p25g, 1, rxEnSelfOffset, (0x1 << rxEnSelfOffset));
        PHY_8295_SDSREG_WRITE(unit, mdxMacId, sdsId, RTL8295_SDS0_ANA_SPD_1P25G_REG21_PAGE, RTL8295_SDS0_ANA_SPD_1P25G_REG21_REG, data1p25g);
    }

    ret = phy_8295_8214_fiberRxEnable_set(unit, port, enable);

    if (enable == DISABLED)
    {
        /* RX_ENSELF=0 */
        data1p25g = REG32_FIELD_SET(data1p25g, 0, rxEnSelfOffset, (0x1 << rxEnSelfOffset));
        PHY_8295_SDSREG_WRITE(unit, mdxMacId, sdsId, RTL8295_SDS0_ANA_SPD_1P25G_REG21_PAGE, RTL8295_SDS0_ANA_SPD_1P25G_REG21_REG, data1p25g);
        data10g = REG32_FIELD_SET(data10g, 0, rxEnSelfOffset, (0x1 << rxEnSelfOffset));
        PHY_8295_SDSREG_WRITE(unit, mdxMacId, sdsId, RTL8295_SDS0_ANA_SPD_10P3125G_REG21_PAGE, RTL8295_SDS0_ANA_SPD_10P3125G_REG21_REG, data10g);
        /* RX_DISABLE= recover */
        PHY_8295_PHYREG_WRITE(unit, mdxMacId, RTL8295_SDS_CTRL_SDS_CTRL_S1_PAGE, RTL8295_SDS_CTRL_SDS_CTRL_S1_REG, sdsCtrlData);
    }

    return ret;
}
#endif

#if defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8214qf_fiberRxEnable_set
 * Description:
 *      Set fiber Rx enable status of the specific port
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - fiber Rx enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      None
 */
int32
phy_8214qf_fiberRxEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    uint32          ret;
    uint32          data = 0, sdsCtrlData, sdsCtrlDataTmp;
    uint32          mdxMacId, sdsId;
    uint32          rxEnSelfOffset = RTL8295_SDS4_ANA_SPD_1P25G_REG08_REG_RX_EN_SELF_OFFSET, rxDisOffset = RTL8295_SDS_CTRL_SDS_CTRL_S4_RX_DISABLE_S4_OFFSET;
    uint32          sdsCtrlAdrrPage = RTL8295_SDS_CTRL_SDS_CTRL_S4_PAGE, sdsCtrlAdrrReg = RTL8295_SDS_CTRL_SDS_CTRL_S4_REG;

    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u fiberRxEnable %d", unit, port, enable);

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);
    PHY_8295_SDSID_GET(unit, port, &sdsId);
    if (enable == DISABLED)
    {
        if (sdsId == 4)
        {
            sdsCtrlAdrrPage = RTL8295_SDS_CTRL_SDS_CTRL_S4_PAGE;
            sdsCtrlAdrrReg = RTL8295_SDS_CTRL_SDS_CTRL_S4_REG;
        }
        else if (sdsId == 5)
        {
            sdsCtrlAdrrPage = RTL8295_SDS_CTRL_SDS_CTRL_S5_PAGE;
            sdsCtrlAdrrReg = RTL8295_SDS_CTRL_SDS_CTRL_S5_REG;
        }
        else if (sdsId == 6)
        {
            sdsCtrlAdrrPage = RTL8295_SDS_CTRL_SDS_CTRL_S6_PAGE;
            sdsCtrlAdrrReg = RTL8295_SDS_CTRL_SDS_CTRL_S6_REG;
        }
        else if (sdsId == 7)
        {
            sdsCtrlAdrrPage = RTL8295_SDS_CTRL_SDS_CTRL_S7_PAGE;
            sdsCtrlAdrrReg = RTL8295_SDS_CTRL_SDS_CTRL_S7_REG;
        }
        else
        {
            ret = RT_ERR_PORT_ID;
            RT_ERR(ret, (MOD_HAL), "unit=%u,port=%u invalid serdes id %u", unit, port, sdsId);
            return RT_ERR_OK;
        }

        PHY_8295_PHYREG_READ(unit, mdxMacId, sdsCtrlAdrrPage, sdsCtrlAdrrReg, &sdsCtrlData);
        PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_ANA_SPD_1P25G_REG08_PAGE, RTL8295_SDS0_ANA_SPD_1P25G_REG08_REG, &data);

        /* RX_DISABLE=1 */
        sdsCtrlDataTmp = REG32_FIELD_SET(sdsCtrlData, 1, rxDisOffset, (0x1 << rxDisOffset));
        PHY_8295_PHYREG_WRITE(unit, mdxMacId, sdsCtrlAdrrPage, sdsCtrlAdrrReg, sdsCtrlDataTmp);
        /* RX_ENSELF=1 */
        data = REG32_FIELD_SET(data, 1, rxEnSelfOffset, (0x1 << rxEnSelfOffset));
        PHY_8295_SDSREG_WRITE(unit, mdxMacId, sdsId, RTL8295_SDS0_ANA_SPD_1P25G_REG08_PAGE, RTL8295_SDS0_ANA_SPD_1P25G_REG08_REG, data);
    }

    ret = phy_8295_8214_fiberRxEnable_set(unit, port, enable);

    if (enable == DISABLED)
    {
        /* RX_ENSELF=0 */
        data = REG32_FIELD_SET(data, 0, rxEnSelfOffset, (0x1 << rxEnSelfOffset));
        PHY_8295_SDSREG_WRITE(unit, mdxMacId, sdsId, RTL8295_SDS0_ANA_SPD_1P25G_REG08_PAGE, RTL8295_SDS0_ANA_SPD_1P25G_REG08_REG, data);
        /* RX_DISABLE=Recover */
        PHY_8295_PHYREG_WRITE(unit, mdxMacId, sdsCtrlAdrrPage, sdsCtrlAdrrReg, sdsCtrlData);
    }

    return ret;
}
#endif


#if defined(CONFIG_SDK_RTL8295R) || defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8295_8214_chipRevId_get
 * Description:
 *      Get chip revision ID
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pChip_rev_id - chip revision ID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8295_8214_chipRevId_get(uint32 unit, rtk_port_t port, uint16 *chip_rev_id)
{
    uint32              mdxMacId;
    uint32              data;

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);
    PHY_8295_PHYREG_READ(unit, mdxMacId, RTL8295_GLB_CTRL_CHIP_INFO_PAGE, RTL8295_GLB_CTRL_CHIP_INFO_REG, &data);
    *chip_rev_id = (uint16) REG32_FIELD_GET(data, RTL8295_GLB_CTRL_CHIP_INFO_CHIP_VER_OFFSET, RTL8295_GLB_CTRL_CHIP_INFO_CHIP_VER_MASK);

    return RT_ERR_OK;
}
#endif


/* Function Name:
 *      _phy_8295_8214_polar_get
 * Description:
 *      Get port polarity configure
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pPolarCtrl - polarity configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
_phy_8295_8214_polar_get(uint32 unit, rtk_port_t port, rtk_port_phyPolarCtrl_t *pPolarCtrl)
{
    uint32              mdxMacId, sdsId;
    uint32              data;

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);
    PHY_8295_SDSID_GET(unit, port, &sdsId);

    osal_memset(pPolarCtrl, 0, sizeof(rtk_port_phyPolarCtrl_t));

    if (sdsId == 0 || sdsId == 1)
    {
        PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_TGR_PRO_0_REG02_PAGE, RTL8295_SDS0_TGR_PRO_0_REG02_REG, &data);
        pPolarCtrl->phy_polar_rx = (REG32_FIELD_GET(data, RTL8295_SDS0_TGR_PRO_0_REG02_FP_TGR2_CFG_INV_HSI_OFFSET, RTL8295_SDS0_TGR_PRO_0_REG02_FP_TGR2_CFG_INV_HSI_MASK)) ? PHY_POLARITY_INVERSE : PHY_POLARITY_NORMAL; /* FP_TGR2_CFG_INV_HSI */
        pPolarCtrl->phy_polar_tx = (REG32_FIELD_GET(data, RTL8295_SDS0_TGR_PRO_0_REG02_FP_TGR2_CFG_INV_HSO_OFFSET, RTL8295_SDS0_TGR_PRO_0_REG02_FP_TGR2_CFG_INV_HSO_MASK)) ? PHY_POLARITY_INVERSE : PHY_POLARITY_NORMAL; /* FP_TGR2_CFG_INV_HSO */
    }
    else
    {
        PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_SDS_REG00_PAGE, RTL8295_SDS0_SDS_REG00_REG, &data);
        pPolarCtrl->phy_polar_rx = (REG32_FIELD_GET(data, RTL8295_SDS0_SDS_REG00_SP_INV_HSI_OFFSET, RTL8295_SDS0_SDS_REG00_SP_INV_HSI_MASK)) ? PHY_POLARITY_INVERSE : PHY_POLARITY_NORMAL; /* SP_INV_HSI */
        pPolarCtrl->phy_polar_tx = (REG32_FIELD_GET(data, RTL8295_SDS0_SDS_REG00_SP_INV_HSO_OFFSET, RTL8295_SDS0_SDS_REG00_SP_INV_HSO_MASK)) ? PHY_POLARITY_INVERSE : PHY_POLARITY_NORMAL; /* SP_INV_HSO */
    }

    return RT_ERR_OK;
}


#if defined(CONFIG_SDK_RTL8295R)
/* Function Name:
 *      phy_8295r_polar_get
 * Description:
 *      Get port polarity configure
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pPolarCtrl - polarity configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8295r_polar_get(uint32 unit, rtk_port_t port, rtk_port_phyPolarCtrl_t *pPolarCtrl)
{
    return _phy_8295_8214_polar_get(unit, port, pPolarCtrl);
}
#endif

#if defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8214qf_polar_get
 * Description:
 *      Get port polarity configure
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pPolarCtrl - polarity configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8214qf_polar_get(uint32 unit, rtk_port_t port, rtk_port_phyPolarCtrl_t *pPolarCtrl)
{
    return _phy_8295_8214_polar_get(unit, port, pPolarCtrl);
}
#endif


/* Function Name:
 *      phy_8295_8214_polar_set
 * Description:
 *      Configure port polarity
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      polarCtrl - polarity configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8295_8214_polar_set(uint32 unit, rtk_port_t port, rtk_port_phyPolarCtrl_t *pPolarCtrl)
{
    uint32              mdxMacId, sdsId;
    uint32              data, rxVal, txVal;

    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,polar: rx:%d tx:%d", unit, port, pPolarCtrl->phy_polar_rx, pPolarCtrl->phy_polar_tx);

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);
    PHY_8295_SDSID_GET(unit, port, &sdsId);

    rxVal = (pPolarCtrl->phy_polar_rx == PHY_POLARITY_INVERSE) ? 1 : 0;
    txVal = (pPolarCtrl->phy_polar_tx == PHY_POLARITY_INVERSE) ? 1 : 0;
    if (sdsId == 0 || sdsId == 1)
    {
        PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_TGR_PRO_0_REG02_PAGE, RTL8295_SDS0_TGR_PRO_0_REG02_REG, &data);
        data = REG32_FIELD_SET(data, rxVal, RTL8295_SDS0_TGR_PRO_0_REG02_FP_TGR2_CFG_INV_HSI_OFFSET, RTL8295_SDS0_TGR_PRO_0_REG02_FP_TGR2_CFG_INV_HSI_MASK);   /* FP_TGR2_CFG_INV_HSI */
        data = REG32_FIELD_SET(data, txVal, RTL8295_SDS0_TGR_PRO_0_REG02_FP_TGR2_CFG_INV_HSO_OFFSET, RTL8295_SDS0_TGR_PRO_0_REG02_FP_TGR2_CFG_INV_HSO_MASK);   /* FP_TGR2_CFG_INV_HSO */
        PHY_8295_SDSREG_WRITE(unit, mdxMacId, sdsId, RTL8295_SDS0_TGR_PRO_0_REG02_PAGE, RTL8295_SDS0_TGR_PRO_0_REG02_REG, data);
    }

    PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_SDS_REG00_PAGE, RTL8295_SDS0_SDS_REG00_REG, &data);
    data = REG32_FIELD_SET(data, rxVal, RTL8295_SDS0_SDS_REG00_SP_INV_HSI_OFFSET, RTL8295_SDS0_SDS_REG00_SP_INV_HSI_MASK); /* SP_INV_HSI */
    data = REG32_FIELD_SET(data, txVal, RTL8295_SDS0_SDS_REG00_SP_INV_HSO_OFFSET, RTL8295_SDS0_SDS_REG00_SP_INV_HSO_MASK); /* SP_INV_HSO */
    PHY_8295_SDSREG_WRITE(unit, mdxMacId, sdsId, RTL8295_SDS0_SDS_REG00_PAGE, RTL8295_SDS0_SDS_REG00_REG, data);

    return RT_ERR_OK;
}


#if defined(CONFIG_SDK_RTL8295R)
/* Function Name:
 *      phy_8295r_polar_set
 * Description:
 *      Configure port polarity
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      polarCtrl - polarity configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8295r_polar_set(uint32 unit, rtk_port_t port, rtk_port_phyPolarCtrl_t *pPolarCtrl)
{
    return phy_8295_8214_polar_set(unit, port, pPolarCtrl);
}
#endif


#if defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8214qf_polar_set
 * Description:
 *      Configure port polarity
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      polarCtrl - polarity configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8214qf_polar_set(uint32 unit, rtk_port_t port, rtk_port_phyPolarCtrl_t *pPolarCtrl)
{
    return phy_8295_8214_polar_set(unit, port, pPolarCtrl);
}
#endif


#if defined(CONFIG_SDK_RTL8295R)
/* Function Name:
 *      phy_sdsEyeParam_get
 * Description:
 *      Get SerDes eye parameters
 * Input:
 *      unit    - unit ID
 *      port    - Base mac ID of the PHY
 * Output:
 *      pEyeParam - eye parameter
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
int32
phy_8295r_sdsEyeParam_get(uint32 unit, rtk_port_t port, uint32 sdsId, rtk_sds_eyeParam_t *pEyeParam)
{
    uint32          mdxMacId;
    uint32          data;

    if ((sdsId != 0) && (sdsId != 1))
    {
        return RT_ERR_INPUT;
    }
    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);

    /* SDS 0x2F 0x10 */
    PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_ANA_SPD_10P3125G_EXT_REG16_PAGE, RTL8295_SDS0_ANA_SPD_10P3125G_EXT_REG16_REG, &data);
    pEyeParam->pre_amp  = REG32_FIELD_GET(data, RTL8295_SDS0_ANA_SPD_10P3125G_EXT_REG16_REG0_PRE_AMP_OFFSET, RTL8295_SDS0_ANA_SPD_10P3125G_EXT_REG16_REG0_PRE_AMP_MASK);
    pEyeParam->main_amp = REG32_FIELD_GET(data, RTL8295_SDS0_ANA_SPD_10P3125G_EXT_REG16_REG0_MAIN_AMP_OFFSET, RTL8295_SDS0_ANA_SPD_10P3125G_EXT_REG16_REG0_MAIN_AMP_MASK);
    pEyeParam->post_amp = REG32_FIELD_GET(data, RTL8295_SDS0_ANA_SPD_10P3125G_EXT_REG16_REG0_POST_AMP_OFFSET, RTL8295_SDS0_ANA_SPD_10P3125G_EXT_REG16_REG0_POST_AMP_MASK);

    /* SDS 0x2F 0xF */
    PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_ANA_SPD_10P3125G_EXT_REG15_PAGE, RTL8295_SDS0_ANA_SPD_10P3125G_EXT_REG15_REG, &data);
    pEyeParam->pre_en  = REG32_FIELD_GET(data, RTL8295_SDS0_ANA_SPD_10P3125G_EXT_REG15_REG0_PRE_EN_OFFSET, RTL8295_SDS0_ANA_SPD_10P3125G_EXT_REG15_REG0_PRE_EN_MASK);
    pEyeParam->post_en = REG32_FIELD_GET(data, RTL8295_SDS0_ANA_SPD_10P3125G_EXT_REG15_REG0_POST_EN_OFFSET, RTL8295_SDS0_ANA_SPD_10P3125G_EXT_REG15_REG0_POST_EN_MASK);

    /* S0/S1 share one impedance register which is located on S0 0x21 0x5 */
    PHY_8295_SDSREG_READ(unit, mdxMacId, 0, RTL8295_SDS0_ANA_COM_REG05_PAGE, RTL8295_SDS0_ANA_COM_REG05_REG, &data);
    pEyeParam->impedance = REG32_FIELD_GET(data, RTL8295_SDS0_ANA_COM_REG05_REG_Z0_PADJR_OFFSET, RTL8295_SDS0_ANA_COM_REG05_REG_Z0_PADJR_MASK);

    return RT_ERR_OK;
}
#endif

#if defined(CONFIG_SDK_RTL8295R)
/* Function Name:
 *      phy_sdsEyeParam_set
 * Description:
 *      Set SerDes eye parameters
 * Input:
 *      unit    - unit ID
 *      port    - Base mac ID of the PHY
 *      pEyeParam - eye parameter
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
int32
phy_8295r_sdsEyeParam_set(uint32 unit, rtk_port_t port, uint32 sdsId, rtk_sds_eyeParam_t *pEyeParam)
{
    uint32          mdxMacId;
    uint32          data;

    if ((sdsId != 0) && (sdsId != 1))
    {
        return RT_ERR_INPUT;
    }
    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);

    /* SDS 0x2F 0x10 */
    PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_ANA_SPD_10P3125G_EXT_REG16_PAGE, RTL8295_SDS0_ANA_SPD_10P3125G_EXT_REG16_REG, &data);
    data = REG32_FIELD_SET(data, pEyeParam->pre_amp, RTL8295_SDS0_ANA_SPD_10P3125G_EXT_REG16_REG0_PRE_AMP_OFFSET, RTL8295_SDS0_ANA_SPD_10P3125G_EXT_REG16_REG0_PRE_AMP_MASK);
    data = REG32_FIELD_SET(data, pEyeParam->main_amp, RTL8295_SDS0_ANA_SPD_10P3125G_EXT_REG16_REG0_MAIN_AMP_OFFSET, RTL8295_SDS0_ANA_SPD_10P3125G_EXT_REG16_REG0_MAIN_AMP_MASK);
    data = REG32_FIELD_SET(data, pEyeParam->post_amp, RTL8295_SDS0_ANA_SPD_10P3125G_EXT_REG16_REG0_POST_AMP_OFFSET, RTL8295_SDS0_ANA_SPD_10P3125G_EXT_REG16_REG0_POST_AMP_MASK);
    PHY_8295_SDSREG_WRITE(unit, mdxMacId, sdsId, RTL8295_SDS0_ANA_SPD_10P3125G_EXT_REG16_PAGE, RTL8295_SDS0_ANA_SPD_10P3125G_EXT_REG16_REG, data);

    /* SDS 0x2F 0xF */
    PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_ANA_SPD_10P3125G_EXT_REG15_PAGE, RTL8295_SDS0_ANA_SPD_10P3125G_EXT_REG15_REG, &data);
    data = REG32_FIELD_SET(data, pEyeParam->pre_en, RTL8295_SDS0_ANA_SPD_10P3125G_EXT_REG15_REG0_PRE_EN_OFFSET, RTL8295_SDS0_ANA_SPD_10P3125G_EXT_REG15_REG0_PRE_EN_MASK);
    data = REG32_FIELD_SET(data, pEyeParam->post_en, RTL8295_SDS0_ANA_SPD_10P3125G_EXT_REG15_REG0_POST_EN_OFFSET, RTL8295_SDS0_ANA_SPD_10P3125G_EXT_REG15_REG0_POST_EN_MASK);
    PHY_8295_SDSREG_WRITE(unit, mdxMacId, sdsId, RTL8295_SDS0_ANA_SPD_10P3125G_EXT_REG15_PAGE, RTL8295_SDS0_ANA_SPD_10P3125G_EXT_REG15_REG, data);

    /* S0/S1 share one impedance register which is located on S0 0x21 0x5 */
    PHY_8295_SDSREG_READ(unit, mdxMacId, 0, RTL8295_SDS0_ANA_COM_REG05_PAGE, RTL8295_SDS0_ANA_COM_REG05_REG, &data);
    data = REG32_FIELD_SET(data, pEyeParam->impedance, RTL8295_SDS0_ANA_COM_REG05_REG_Z0_PADJR_OFFSET, RTL8295_SDS0_ANA_COM_REG05_REG_Z0_PADJR_MASK);
    PHY_8295_SDSREG_WRITE(unit, mdxMacId, 0, RTL8295_SDS0_ANA_COM_REG05_PAGE, RTL8295_SDS0_ANA_COM_REG05_REG, data);

    return RT_ERR_OK;
}
#endif


#if defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8214qf_sdsEyeParam_get
 * Description:
 *      Get SerDes eye parameters
 * Input:
 *      unit    - unit ID
 *      port    - Base mac ID of the PHY
 * Output:
 *      pEyeParam - eye parameter
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
int32
phy_8214qf_sdsEyeParam_get(uint32 unit, rtk_port_t port, uint32 sdsId, rtk_sds_eyeParam_t *pEyeParam)
{
    uint32          mdxMacId;
    uint32          data;

    if (port != HWP_PHY_BASE_MACID(unit, port))
    {
        return RT_ERR_PORT_ID;
    }

    if ((sdsId != 0) && (sdsId != 1))
    {
        return RT_ERR_FAILED;
    }

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);

    osal_memset(pEyeParam, 0, sizeof(rtk_sds_eyeParam_t));

    /* SDS 0x2F 0x10 */
    PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_ANA_SPD_10P3125G_EXT_REG16_PAGE, RTL8295_SDS0_ANA_SPD_10P3125G_EXT_REG16_REG, &data);
    pEyeParam->pre_amp  = REG32_FIELD_GET(data, RTL8295_SDS0_ANA_SPD_10P3125G_EXT_REG16_REG0_PRE_AMP_OFFSET, RTL8295_SDS0_ANA_SPD_10P3125G_EXT_REG16_REG0_PRE_AMP_MASK);
    pEyeParam->main_amp = REG32_FIELD_GET(data, RTL8295_SDS0_ANA_SPD_10P3125G_EXT_REG16_REG0_MAIN_AMP_OFFSET, RTL8295_SDS0_ANA_SPD_10P3125G_EXT_REG16_REG0_MAIN_AMP_MASK);
    pEyeParam->post_amp = REG32_FIELD_GET(data, RTL8295_SDS0_ANA_SPD_10P3125G_EXT_REG16_REG0_POST_AMP_OFFSET, RTL8295_SDS0_ANA_SPD_10P3125G_EXT_REG16_REG0_POST_AMP_MASK);

    /* SDS 0x2F 0xF */
    PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_ANA_SPD_10P3125G_EXT_REG15_PAGE, RTL8295_SDS0_ANA_SPD_10P3125G_EXT_REG15_REG, &data);
    pEyeParam->pre_en  = REG32_FIELD_GET(data, RTL8295_SDS0_ANA_SPD_10P3125G_EXT_REG15_REG0_PRE_EN_OFFSET, RTL8295_SDS0_ANA_SPD_10P3125G_EXT_REG15_REG0_PRE_EN_MASK);
    pEyeParam->post_en = REG32_FIELD_GET(data, RTL8295_SDS0_ANA_SPD_10P3125G_EXT_REG15_REG0_POST_EN_OFFSET, RTL8295_SDS0_ANA_SPD_10P3125G_EXT_REG15_REG0_POST_EN_MASK);

    if (sdsId == 0)
    {
        /* S0/S1 share one impedance register which is located on S0 0x21 0x5 */
        PHY_8295_SDSREG_READ(unit, mdxMacId, 0, RTL8295_SDS0_ANA_COM_REG05_PAGE, RTL8295_SDS0_ANA_COM_REG05_REG, &data);
        pEyeParam->impedance = REG32_FIELD_GET(data, RTL8295_SDS0_ANA_COM_REG05_REG_Z0_PADJR_OFFSET, RTL8295_SDS0_ANA_COM_REG05_REG_Z0_PADJR_MASK);
    }
    else
    {
        pEyeParam->impedance = 0;
    }

    return RT_ERR_OK;
}
#endif

#if defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8214qf_sdsEyeParam_set
 * Description:
 *      Set SerDes eye parameters
 * Input:
 *      unit    - unit ID
 *      port    - Base mac ID of the PHY
 *      sdsId   - SerDes ID of the PHY
 *      pEyeParam - eye parameter
 *                  impedance is not supported for configure.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      None
 */
int32
phy_8214qf_sdsEyeParam_set(uint32 unit, rtk_port_t port, uint32 sdsId, rtk_sds_eyeParam_t *pEyeParam)
{
    uint32          mdxMacId;
    uint32          data;

    if (port != HWP_PHY_BASE_MACID(unit, port))
    {
        return RT_ERR_PORT_ID;
    }

    if ((sdsId != 0) && (sdsId != 1))
    {
        return RT_ERR_FAILED;
    }

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);

    /* SDS 0x2F 0x10 */
    PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_ANA_SPD_10P3125G_EXT_REG16_PAGE, RTL8295_SDS0_ANA_SPD_10P3125G_EXT_REG16_REG, &data);
    data = REG32_FIELD_SET(data, pEyeParam->pre_amp, RTL8295_SDS0_ANA_SPD_10P3125G_EXT_REG16_REG0_PRE_AMP_OFFSET, RTL8295_SDS0_ANA_SPD_10P3125G_EXT_REG16_REG0_PRE_AMP_MASK);
    data = REG32_FIELD_SET(data, pEyeParam->main_amp, RTL8295_SDS0_ANA_SPD_10P3125G_EXT_REG16_REG0_MAIN_AMP_OFFSET, RTL8295_SDS0_ANA_SPD_10P3125G_EXT_REG16_REG0_MAIN_AMP_MASK);
    data = REG32_FIELD_SET(data, pEyeParam->post_amp, RTL8295_SDS0_ANA_SPD_10P3125G_EXT_REG16_REG0_POST_AMP_OFFSET, RTL8295_SDS0_ANA_SPD_10P3125G_EXT_REG16_REG0_POST_AMP_MASK);
    PHY_8295_SDSREG_WRITE(unit, mdxMacId, sdsId, RTL8295_SDS0_ANA_SPD_10P3125G_EXT_REG16_PAGE, RTL8295_SDS0_ANA_SPD_10P3125G_EXT_REG16_REG, data);

    /* SDS 0x2F 0xF */
    PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_ANA_SPD_10P3125G_EXT_REG15_PAGE, RTL8295_SDS0_ANA_SPD_10P3125G_EXT_REG15_REG, &data);
    data = REG32_FIELD_SET(data, pEyeParam->pre_en, RTL8295_SDS0_ANA_SPD_10P3125G_EXT_REG15_REG0_PRE_EN_OFFSET, RTL8295_SDS0_ANA_SPD_10P3125G_EXT_REG15_REG0_PRE_EN_MASK);
    data = REG32_FIELD_SET(data, pEyeParam->post_en, RTL8295_SDS0_ANA_SPD_10P3125G_EXT_REG15_REG0_POST_EN_OFFSET, RTL8295_SDS0_ANA_SPD_10P3125G_EXT_REG15_REG0_POST_EN_MASK);
    PHY_8295_SDSREG_WRITE(unit, mdxMacId, sdsId, RTL8295_SDS0_ANA_SPD_10P3125G_EXT_REG15_PAGE, RTL8295_SDS0_ANA_SPD_10P3125G_EXT_REG15_REG, data);

    /* not support configure impedance */

    return RT_ERR_OK;
}
#endif


#if defined(CONFIG_SDK_RTL8295R)
/* Function Name:
 *      phy_8295r_ipgShrinkEnable_get
 * Description:
 *      Get the statue of IPG Shrink the specific port in the specific unit
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pValue - pointer to status of IPG Shrink
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
_phy_8295r_ipgShrinkEnable_get(uint32 unit, rtk_port_t port, uint32 *pValue)
{
    uint32                  mdxMacId;
    uint32                  data;

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);
    PHY_8295_PHYREG_READ(unit, mdxMacId, RTL8295_SDS_CTRL_SDS_CTRL_S1_PAGE, RTL8295_SDS_CTRL_SDS_CTRL_S1_REG, &data);
    *pValue = REG32_FIELD_GET(data, RTL8295_SDS_CTRL_SDS_CTRL_S1_ANA_10P9375G_S1_OFFSET, RTL8295_SDS_CTRL_SDS_CTRL_S1_ANA_10P9375G_S1_MASK);
    return RT_ERR_OK;
}
#endif /* CONFIG_SDK_RTL8295R */

#if defined(CONFIG_SDK_RTL8295R)
/* Function Name:
 *      phy_8295r_ipgShrinkEnable_set
 * Description:
 *      Set the statue of IPG Shrink of the specific port in the specific unit
 *      Enable PHY's IPG shrink when switch enables stacking on the port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      value - status of IPG Shrink. 1: enable, 0: disable.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - input parameter out of range
 * Note:
 *      None
 */
int32
_phy_8295r_ipgShrinkEnable_set(uint32 unit, rtk_port_t port, uint32 value)
{
    uint32                  mdxMacId;
    uint32                  data, data_tmp, data_sds_reg13;

    if ((value != 1) && (value != 0))
    {
        return RT_ERR_INPUT;
    }

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);

    /*
     *   [bit6] Enable 10.9375G 10GBASE-R mode
     *      0x0: 10.3125G in 10GBASE-R
     *      0x1: 10.9375G in 10GBASE-R
     */

    /* disable serdes, set mode and recover serdes  */
    /* S0 */
    PHY_8295_PHYREG_READ(unit, mdxMacId, RTL8295_SDS_CTRL_SDS_CTRL_S0_PAGE, RTL8295_SDS_CTRL_SDS_CTRL_S0_REG, &data);
    /* disable serdes */
    data_tmp = REG32_FIELD_SET(data, PHY_8295_SDS_MODE_OFF, RTL8295_SDS_CTRL_SDS_CTRL_S0_SERDES_MODE_S0_OFFSET, RTL8295_SDS_CTRL_SDS_CTRL_S0_SERDES_MODE_S0_MASK);
    PHY_8295_PHYREG_WRITE(unit, mdxMacId, RTL8295_SDS_CTRL_SDS_CTRL_S0_PAGE, RTL8295_SDS_CTRL_SDS_CTRL_S0_REG, data_tmp);
    osal_time_mdelay(100);

    /* configure per-serdes register */
    PHY_8295_SDSREG_READ(unit, mdxMacId, 0, RTL8295_SDS0_TGR_PRO_0_REG13_PAGE, RTL8295_SDS0_TGR_PRO_0_REG13_REG, &data_sds_reg13);
    data_sds_reg13 = REG32_FIELD_SET(data_sds_reg13, value, RTL8295_SDS0_TGR_PRO_0_REG13_FT_TGR13_CFG_STK_MODE_OFFSET, RTL8295_SDS0_TGR_PRO_0_REG13_FT_TGR13_CFG_STK_MODE_MASK);
    PHY_8295_SDSREG_WRITE(unit, mdxMacId, 0, RTL8295_SDS0_TGR_PRO_0_REG13_PAGE, RTL8295_SDS0_TGR_PRO_0_REG13_REG, data_sds_reg13);

    /* set mode with original serdes mode config */
    data = REG32_FIELD_SET(data, value, RTL8295_SDS_CTRL_SDS_CTRL_S0_ANA_10P9375G_S0_OFFSET, RTL8295_SDS_CTRL_SDS_CTRL_S0_ANA_10P9375G_S0_MASK);
    PHY_8295_PHYREG_WRITE(unit, mdxMacId, RTL8295_SDS_CTRL_SDS_CTRL_S0_PAGE, RTL8295_SDS_CTRL_SDS_CTRL_S0_REG, data);
    osal_time_mdelay(100);

    /* S1 */
    PHY_8295_PHYREG_READ(unit, mdxMacId, RTL8295_SDS_CTRL_SDS_CTRL_S1_PAGE, RTL8295_SDS_CTRL_SDS_CTRL_S1_REG, &data);
    /* disable serdes */
    data_tmp = REG32_FIELD_SET(data, PHY_8295_SDS_MODE_OFF, RTL8295_SDS_CTRL_SDS_CTRL_S1_SERDES_MODE_S1_OFFSET, RTL8295_SDS_CTRL_SDS_CTRL_S1_SERDES_MODE_S1_MASK);
    PHY_8295_PHYREG_WRITE(unit, mdxMacId, RTL8295_SDS_CTRL_SDS_CTRL_S1_PAGE, RTL8295_SDS_CTRL_SDS_CTRL_S1_REG, data_tmp);
    osal_time_mdelay(100);

    /* configure per-serdes register */
    PHY_8295_SDSREG_READ(unit, mdxMacId, 1, RTL8295_SDS0_TGR_PRO_0_REG13_PAGE, RTL8295_SDS0_TGR_PRO_0_REG13_REG, &data_sds_reg13);
    data_sds_reg13 = REG32_FIELD_SET(data_sds_reg13, value, RTL8295_SDS0_TGR_PRO_0_REG13_FT_TGR13_CFG_STK_MODE_OFFSET, RTL8295_SDS0_TGR_PRO_0_REG13_FT_TGR13_CFG_STK_MODE_MASK);
    PHY_8295_SDSREG_WRITE(unit, mdxMacId, 1, RTL8295_SDS0_TGR_PRO_0_REG13_PAGE, RTL8295_SDS0_TGR_PRO_0_REG13_REG, data_sds_reg13);

    /* set mode with original serdes mode config */
    data = REG32_FIELD_SET(data, value, RTL8295_SDS_CTRL_SDS_CTRL_S1_ANA_10P9375G_S1_OFFSET, RTL8295_SDS_CTRL_SDS_CTRL_S1_ANA_10P9375G_S1_MASK);
    PHY_8295_PHYREG_WRITE(unit, mdxMacId, RTL8295_SDS_CTRL_SDS_CTRL_S1_PAGE, RTL8295_SDS_CTRL_SDS_CTRL_S1_REG, data);

    return RT_ERR_OK;

}
#endif /* CONFIG_SDK_RTL8295R */

#if defined(CONFIG_SDK_RTL8295R)
/* Function Name:
 *      phy_8295r_ctrl_get
 * Description:
 *      Get PHY settings for specific feature
 * Input:
 *      unit      - unit id
 *      port      - port id
 *      ctrl_type - setting type
 * Output:
 *      pValue    - pointer to setting value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_NOT_SUPPORTED
 * Note:
 *      None
 */
int32
phy_8295r_ctrl_get(uint32 unit, rtk_port_t port, rtk_phy_ctrl_t ctrl_type, uint32 *pValue)
{
    int32  ret = 0;

    switch (ctrl_type)
    {
      case RTK_PHY_CTRL_IPG_SHRINK:
        _phy_8295r_ipgShrinkEnable_get(unit, port, pValue);
        break;
      default:
        return RT_ERR_PORT_NOT_SUPPORTED;
    } /* end switch */

    return ret;
}
#endif /* CONFIG_SDK_RTL8295R */

#if defined(CONFIG_SDK_RTL8295R)
/* Function Name:
 *      phy_8295r_ctrl_set
 * Description:
 *      Set PHY settings for specific feature
 * Input:
 *      unit      - unit id
 *      port      - port id
 *      ctrl_type - setting type
 *      value     - setting value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_NOT_SUPPORTED
 * Note:
 *      None
 */
int32
phy_8295r_ctrl_set(uint32 unit, rtk_port_t port, rtk_phy_ctrl_t ctrl_type, uint32 value)
{
    int32  ret = 0;

    switch (ctrl_type)
    {
      case RTK_PHY_CTRL_IPG_SHRINK:
        ret = _phy_8295r_ipgShrinkEnable_set(unit, port, value);
        break;
      default:
        return RT_ERR_PORT_NOT_SUPPORTED;
    } /* end switch */
    return ret;
}
#endif /* CONFIG_SDK_RTL8295R */

#if defined(CONFIG_SDK_RTL8295R)
/* Function Name:
 *      phy_8295r_reg_get
 * Description:
 *      Get PHY registers.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      page    - PHY page
 *      phy_reg - PHY register
 * Output:
 *      pData   - pointer buffer of read data
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
int32
phy_8295r_reg_get(uint32 unit,  rtk_port_t port, uint32 page, uint32 phy_reg, uint32 *pData)
{
    uint32              mdxMacId;

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);
    return hal_miim_read(unit, mdxMacId, page, phy_reg, pData);
}
#endif

#if defined(CONFIG_SDK_RTL8295R)
/* Function Name:
 *      phy_8295r_reg_set
 * Description:
 *      Set PHY registers.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      page    - PHY page
 *      phy_reg - PHY register
 *      data    - write data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
int32
phy_8295r_reg_set(uint32 unit,  rtk_port_t port, uint32 page, uint32 phy_reg, uint32 data)
{
    uint32              mdxMacId;

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);
    return hal_miim_write(unit, port, page, phy_reg, data);
}
#endif


#if defined(CONFIG_SDK_RTL8295R)
/* Function Name:
 *      phy_8295r_reg_park_get
 * Description:
 *      Get PHY registers.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      page    - PHY page
 *      parkPage    - PHY park page
 *      phy_reg - PHY register
 * Output:
 *      pData   - pointer buffer of read data
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
int32
phy_8295r_reg_park_get(uint32 unit, rtk_port_t port, uint32 page, uint32 parkPage, uint32 phy_reg, uint32 *pData)
{
    uint32              mdxMacId;

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);
    return hal_miim_park_read(unit, mdxMacId, page, parkPage, phy_reg, pData);
}
#endif

#if defined(CONFIG_SDK_RTL8295R)
/* Function Name:
 *      phy_8295r_reg_park_set
 * Description:
 *      Set PHY registers.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      page    - PHY page
 *      parkPage    - PHY park page
 *      phy_reg - PHY register
 *      data    - write data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
int32
phy_8295r_reg_park_set(
    uint32      unit,
    rtk_port_t  port,
    uint32      page,
    uint32      parkPage,
    uint32      phy_reg,
    uint32      data)
{
    uint32              mdxMacId;

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);
    return hal_miim_park_write(unit, mdxMacId, page, parkPage, phy_reg, data);
}
#endif


#if defined(CONFIG_SDK_RTL8295R)
/* Function Name:
 *      phy_8295r_reg_mmd_get
 * Description:
 *      Get PHY registers.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      mmdAddr - mmd device address
 *      mmdReg  - mmd reg id
 * Output:
 *      pData   - pointer buffer of read data
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
int32
phy_8295r_reg_mmd_get(uint32 unit, rtk_port_t port, uint32 mmdAddr, uint32 mmdReg, uint32 *pData)
{
    uint32              mdxMacId;

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);
    return hal_miim_mmd_read(unit, mdxMacId, mmdAddr, mmdReg, pData);
}
#endif

#if defined(CONFIG_SDK_RTL8295R)
/* Function Name:
 *      phy_8295r_reg_mmd_set
 * Description:
 *      Set PHY registers.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      mainPage    - main page id
 *      extPage     - extension page id
 *      parkPage    - parking page id
 *      phy_reg - PHY register
 *      data    - write data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
int32
phy_8295r_reg_mmd_set(uint32 unit, rtk_port_t port, uint32 mmdAddr, uint32 mmdReg, uint32 data)
{
    uint32              mdxMacId;

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);
    return hal_miim_mmd_write(unit, mdxMacId, mmdAddr, mmdReg, data);
}
#endif

#if defined(CONFIG_SDK_RTL8295R)
int32
_phy_8295r_10gMediaDb_init(uint32 unit, rtk_port_t port)
{
    int32                   ret;
    uint32                  sdsMode;
    rtk_port_10gMedia_t     media;

    if ((ret = _phy_8295_serdesMode_get(unit, port, &sdsMode)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HAL), "unit=%u,port=%u", unit, port);
        return ret;
    }

    switch (sdsMode)
    {
      case PHY_8295_SDS_MODE_10GR:
        media = PORT_10GMEDIA_FIBER_10G;
        break;
      case PHY_8295_SDS_MODE_FIB1G:
        media = PORT_10GMEDIA_FIBER_1G;
        break;
      case PHY_8295_SDS_MODE_OFF:
        media = PORT_10GMEDIA_NONE;
        break;
      default:
        RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,mode=0x%X unexpected mode", unit, port, sdsMode);
        PORT_10G_MEDIA(unit, port) = PORT_10GMEDIA_NONE;
        return RT_ERR_INPUT;
    }/* end switch */

    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,mode=0x%X,media=%d", unit, port, sdsMode, media);

    PORT_10G_MEDIA(unit, port) = media;
    return RT_ERR_OK;

}
#endif


#if defined(CONFIG_SDK_RTL8295R)
int32
_phy_8295r_rxCaliS1(uint32 unit, rtk_port_t port, int32 retryCnt)
{
    int32       ret, procCnt, sdsId = 1;
    uint32      resl;
    rtk_enable_t    pollSts;
    phy_8295_rxCaliParam_t  rxCaliParam;
    rtk_port_phySdsRxCaliStatus_t   *pRxCaliSts;

    _phy_8295_rxCaliParam_get(unit, port, sdsId, &rxCaliParam);

    pRxCaliSts = &g8295rInfo[unit]->rxCaliStatus[port][sdsId];
    *pRxCaliSts = PHY_SDS_RXCALI_STATUS_NOINFO;
    procCnt = 0;

    if (g8295rStatusSwPoll == FALSE)
    {
        RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,Disable MACpollPHY", unit, port);
        if ((ret = hal_miim_pollingEnable_get(unit, port, &pollSts)) != RT_ERR_OK)
        {
            RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,get MACpollPHY ret=%x", unit, port, ret);
        }
        if ((ret = hal_miim_pollingEnable_set(unit, port, DISABLED)) != RT_ERR_OK)
        {
            RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,set MACpollPHY ret=%x", unit, port, ret);
        }
    }

    do {
        ret = phy_8295_rxCali_start(unit, port, sdsId, &rxCaliParam);

        /* read twice to clear letched down */
        phy_8295r_resolutionReg_get(unit, port, &resl);
        phy_8295r_resolutionReg_get(unit, port, &resl);

        if (ret == RT_ERR_OK)
        {
            break;
        }
        procCnt++;
    } while (procCnt <= retryCnt);

    if (g8295rStatusSwPoll == FALSE)
    {
        RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,recover MACpollPHY %u", unit, port, pollSts);
        hal_miim_pollingEnable_set(unit, port, pollSts);
    }

    if (ret == RT_ERR_OK)
    {
        *pRxCaliSts = PHY_SDS_RXCALI_STATUS_OK;
    }
    else
    {
        *pRxCaliSts = PHY_SDS_RXCALI_STATUS_FAILED;
        /* initial RX registers to default */
        phy_8295_rxCaliRegDefault_init(unit, port, sdsId);
        if (retryCnt > 0)
        {
            RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,sds=%u cali failed.", unit, port, sdsId);
        }
    }
    return ret;
}
#endif

#if defined(CONFIG_SDK_RTL8295R)
int32
_phy_8295r_rxCaliS0(uint32 unit, rtk_port_t port)
{
    int32       ret;
    int32       try_cnt;
    uint32      sdsMode, data;
    uint32      mdxMacId, sdsId = 0;
    phy_8295_rxCaliParam_t  rxCaliParam;

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);

    if ((ret = _phy_8295_serdesModeReg_get(unit, port, mdxMacId, sdsId, &sdsMode) != RT_ERR_OK))
    {
        RT_ERR(ret, (MOD_HAL), "unit=%u,port=%u S0 mode get failed", unit, port);
        return ret;
    }

    if (sdsMode != PHY_8295_SDS_MODE_10GR)
    {
        return RT_ERR_NOT_ALLOWED;
    }

    /* S0 calibration process */
    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u S0 RxCali", unit, port);

    try_cnt = PHY_8295_LINK_RETRY_CNT;
    ret = RT_ERR_PORT_LINKDOWN;
    /* wait for link up */
    do {
        /* check S0 link state */
        /* read twice */
        PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_TGR_STD_0_REG01_PAGE, RTL8295_SDS0_TGR_STD_0_REG01_REG, &data);
        PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_TGR_STD_0_REG01_PAGE, RTL8295_SDS0_TGR_STD_0_REG01_REG, &data);
        if (data & (0x1 << RTL8295_SDS0_TGR_STD_0_REG01_FP_PCS1_RX_LINK_OFFSET))
        {
            /* link is up */
            ret = RT_ERR_OK;
            RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u S0 link is up (%u)", unit, port, try_cnt);
            break;
        }
        else
        {
            /* wait sometime for link up */
            osal_time_usleep(1000);
        }
        try_cnt--;
    } while (try_cnt > 0);

    if (ret != RT_ERR_OK)
    {
        RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u S0 link is down", unit, port);
    }

    _phy_8295_rxCaliParam_get(unit, port, 0, &rxCaliParam);
    try_cnt = PHY_8295_LINK_RETRY_CNT;
    do {
        ret = phy_8295_rxCali_start(unit, port, 0, &rxCaliParam);

        /* check S0 link-up */
        /* read twice */
        PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_TGR_STD_0_REG01_PAGE, RTL8295_SDS0_TGR_STD_0_REG01_REG, &data);
        PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_TGR_STD_0_REG01_PAGE, RTL8295_SDS0_TGR_STD_0_REG01_REG, &data);
        if (data & (0x1 << RTL8295_SDS0_TGR_STD_0_REG01_FP_PCS1_RX_LINK_OFFSET))
        {
            RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u S0 link is up", unit, port);
            break;
        }
        else
        {
            RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u S0 link is down", unit, port);
        }

        try_cnt--;
    } while (try_cnt > 0);

    if (try_cnt <= 0)
    {
        g8295rInfo[unit]->rxCaliStatus[port][0] = PHY_SDS_RXCALI_STATUS_FAILED;
        ret = RT_ERR_PORT_LINKDOWN;
        RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u S0 cali timeout. SDS_10G_LINK=0x%x", unit, port, data);
        return ret;
    }
    else
    {
        g8295rInfo[unit]->rxCaliStatus[port][0] = PHY_SDS_RXCALI_STATUS_OK;
        return RT_ERR_OK;
    }
}
#endif


#if defined(CONFIG_SDK_RTL8295R)
int32
_phy_8295r_rxCali_process(uint32 unit, rtk_port_t port)
{
    uint32          mode;
    int32           ret;
    rtk_enable_t    lbEn = DISABLED;

    /* link change up process */
    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,10gMedia=%u", unit, port, PORT_10G_MEDIA(unit, port));

    /* skip the process if serdes is off */
    if ((_phy_8295_serdesMode_get(unit, port, &mode) == RT_ERR_OK) && (mode == PHY_8295_SDS_MODE_OFF))
    {
        RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,SDS is OFF", unit, port);
        return RT_ERR_ABORT;
    }

    /* skip the process if loopback is enabled */
    if ((phy_8295r_loopback_get(unit, port, &lbEn) == RT_ERR_OK) && (lbEn == ENABLED))
    {
        g8295rInfo[unit]->rxCaliStatus[port][1] = PHY_SDS_RXCALI_STATUS_NOINFO;
        RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,loopback enabled ", unit, port);
        RTK_PORTMASK_PORT_SET(g8295rInfo[unit]->port10gReady, port);
    }
    else
    {
        ret = RT_ERR_OK;
        switch (PORT_10G_MEDIA(unit, port))
        {
          case PORT_10GMEDIA_FIBER_10G:
          case PORT_10GMEDIA_DAC_50CM:
          case PORT_10GMEDIA_DAC_100CM:
            if (RTK_PORTMASK_IS_PORT_SET(g8295rInfo[unit]->s1RxCaliEn, port))
            {
                RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u rxCaliS1", unit, port);
                ret = _phy_8295r_rxCaliS1(unit, port, 0);
            }
            break;
          case PORT_10GMEDIA_DAC_300CM:
          case PORT_10GMEDIA_DAC_500CM: //DAC_LONG
            RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u long.rxCaliS1", unit, port);
            ret = _phy_8295r_rxCaliS1(unit, port, 0);
            break;
          case PORT_10GMEDIA_FIBER_1G:
          default:
            return RT_ERR_OK;
        }

        if (ret == RT_ERR_OK)
        {
            RTK_PORTMASK_PORT_SET(g8295rInfo[unit]->port10gReady, port);
        }
    }



    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,S0 caliEn=%u,calied=%u", unit, port,
                (RTK_PORTMASK_IS_PORT_SET(g8295rInfo[unit]->s0RxCaliEn, port)?1:0),
                (RTK_PORTMASK_IS_PORT_SET(g8295rInfo[unit]->s0RxCalied, port)?1:0));
    if (   RTK_PORTMASK_IS_PORT_SET(g8295rInfo[unit]->s0RxCaliEn, port)
        && RTK_PORTMASK_IS_PORT_CLEAR(g8295rInfo[unit]->s0RxCalied, port))
    {
        if ((ret = _phy_8295r_rxCaliS0(unit, port)) == RT_ERR_OK)
        {
            RTK_PORTMASK_PORT_SET(g8295rInfo[unit]->s0RxCalied, port);
        }
    }

    return RT_ERR_OK;
}
#endif


#if defined(CONFIG_SDK_RTL8295R)
int32
_phy_8295r_linkChangeDown_process(uint32 unit, rtk_port_t port)
{
    /* clear status */
    RTK_PORTMASK_PORT_CLEAR(g8295rInfo[unit]->port10gReady, port);
    if (PORT_10G_MEDIA(unit, port) != PORT_10GMEDIA_FIBER_1G)
    {
        _switch_port_macForceLinkDown_enable(unit, port, ENABLED);
    }

    /* initial RX registers to default */
    phy_8295_rxCaliRegDefault_init(unit, port, 1);

    return RT_ERR_OK;
}
#endif


#if defined(CONFIG_SDK_RTL8295R)
int32
phy_8295r_linkChange_process(uint32 unit, rtk_port_t port, rtk_port_linkStatus_t linkSts)
{
    if (g8295ProcLinkChg == FALSE)
        return RT_ERR_OK;

    if (!g8295rInfo[unit])
        return RT_ERR_OK;

    if (port >= RTK_MAX_PORT_PER_UNIT)
    {
        return RT_ERR_OK;
    }

    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,linkChg=%s", unit, port, ((linkSts==PORT_LINKUP)?"UP":"DOWN"));

    if(phy_8295r_rxCali_process_sts_get(unit, port) != PHY_8295_RXCALI_PROCESS_STS_IDLE)
    {
        RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,RX CALIBRATION PROCESS is ongoing", unit, port);
        return RT_ERR_OK;
    }
    phy_8295r_rxCali_process_sts_set(unit, port, PHY_8295_RXCALI_PROCESS_STS_PROCESS);
    if (linkSts == PORT_LINKUP)
    {
        /* link change up process */
        /* do nothing */
    }
    else
    {
        /* link change down process */
        _phy_8295r_linkChangeDown_process(unit, port);
    }
    phy_8295r_rxCali_process_sts_set(unit, port, PHY_8295_RXCALI_PROCESS_STS_IDLE);

#if defined(CONFIG_SDK_RTL8390)
    if (HWP_8390_FAMILY_ID(unit))
    {
        if (linkSts == PORT_LINKUP)
        {
            if (!RTK_PORTMASK_IS_PORT_SET(g8295rInfo[unit]->macRxCalied, port))
            {
                RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,macRxCali", unit, port);
                phy_8390_linkChange_process(unit, port, linkSts);
                RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,macRxCali done", unit, port);
                RTK_PORTMASK_PORT_SET(g8295rInfo[unit]->macRxCalied, port);
            }
        }
    }
#endif /* #if defined(CONFIG_SDK_RTL8390) */


    return RT_ERR_OK;
}
#endif


#if defined(CONFIG_SDK_RTL8295R)
int32
_phy_8295r_fiberRx_1g_check(uint32 unit, rtk_port_t port, uint32 mdxMacId, uint32 sdsId, uint32 *pStatus)
{
    uint32      data, data1;
    uint32      counter;

    *pStatus = PHY_FIBER_RX_STATUS_0;

    PHY_8295_PHYREG_READ(unit, mdxMacId, RTL8295_SDS_CTRL_SDS_1G_LINK_PAGE, RTL8295_SDS_CTRL_SDS_1G_LINK_REG, &data);
    if (data & RTL8295_SDS_CTRL_SDS_1G_LINK_SDS_S1P0_LINK_MASK)
    {
        return RT_ERR_OK;
    }

    if (HWP_PHY_MODEL_BY_PORT(unit, port) == RTK_PHYTYPE_RTL8295R_C22)
    {
        PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_SDS_EXT_REG31_PAGE, RTL8295_SDS0_SDS_EXT_REG31_REG, &data);
        if (data & PHY_8295_SDS_REG_SEP_RXIDLE_MASK)
        {
            return RT_ERR_OK;
        }
    }
    else
    {
        PHY_8295_PHYREG_READ(unit, mdxMacId, RTL8295_SDS_CTRL_RXAUI_SEL_CHN_PAGE, RTL8295_SDS_CTRL_RXAUI_SEL_CHN_REG, &data);
        if (data & RTL8295_SDS_CTRL_RXAUI_SEL_CHN_RXIDLE_S1_MASK)
        {
            return RT_ERR_OK;
        }
    }

    /* select channel */
    PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_SDS_EXT_REG24_PAGE, RTL8295_SDS0_SDS_EXT_REG24_REG, &data);
    // bit[2:0] set 0
    data = REG32_FIELD_SET(data, 0, 0, 0x7);
    PHY_8295_SDSREG_WRITE(unit, mdxMacId, sdsId, RTL8295_SDS0_SDS_EXT_REG24_PAGE, RTL8295_SDS0_SDS_EXT_REG24_REG, data);

    /* check counter */
    PHY_8295_SDSREG_WRITE(unit, mdxMacId, sdsId, RTL8295_SDS0_SDS_EXT_REG02_PAGE, RTL8295_SDS0_SDS_EXT_REG02_REG, 0);

    PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_SDS_EXT_REG02_PAGE, RTL8295_SDS0_SDS_EXT_REG02_REG, &data);
    // bit[15:0]
    counter = REG32_FIELD_GET(data, 0, 0xFFFF);

    PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_SDS_EXT_REG03_PAGE, RTL8295_SDS0_SDS_EXT_REG03_REG, &data1);
    counter |= REG32_FIELD_GET(data1, 8, (0xF << 8)) << 15;
    if (counter == 0)
    {
        return RT_ERR_OK;
    }

    *pStatus = PHY_FIBER_RX_STATUS_1;
    return RT_ERR_OK;
}
#endif

#if defined(CONFIG_SDK_RTL8295R)
/*
 * Check RX Idle for 10G
 */
int32
_phy_8295r_es_10g_rxIdle(uint32 unit, rtk_port_t port, uint32 mdxMacId, uint32 sdsId, uint32 *pRxIdle)
{
    uint32          data;
    /* S1 RXIDLE [15] */
    /* Readback Page 0x0 reg 0x1c, set [9:0] 0x06 */
    PHY_8295_SDSREG_READ(unit, mdxMacId, 2, RTL8295_SDS0_SDS_REG28_PAGE, RTL8295_SDS0_SDS_REG28_REG, &data);
    data = REG32_FIELD_SET(data, 0x6, 0, BITS_MASK(9, 0));
    PHY_8295_SDSREG_WRITE(unit, mdxMacId, 2, RTL8295_SDS0_SDS_REG28_PAGE, RTL8295_SDS0_SDS_REG28_REG, data);
    PHY_8295_SDSREG_READ(unit, mdxMacId, 2, RTL8295_SDS0_SDS_REG29_PAGE, RTL8295_SDS0_SDS_REG29_REG, &data);

    /* en_RXTEST */
    PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_ANA_SPD_10P3125G_REG21_PAGE, RTL8295_SDS0_ANA_SPD_10P3125G_REG21_REG, &data);
    data = REG32_FIELD_SET(data, 0x1, 9, BITS_MASK(9, 9));
    PHY_8295_SDSREG_WRITE(unit, mdxMacId, sdsId, RTL8295_SDS0_ANA_SPD_10P3125G_REG21_PAGE, RTL8295_SDS0_ANA_SPD_10P3125G_REG21_REG, data);

    PHY_8295_SDSREG_READ(unit, mdxMacId, 2, RTL8295_SDS0_SDS_REG29_PAGE, RTL8295_SDS0_SDS_REG29_REG, &data);
    *pRxIdle = (data >> 15) & 0x1;

    /* dis_RXTEST */
    PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_ANA_SPD_10P3125G_REG21_PAGE, RTL8295_SDS0_ANA_SPD_10P3125G_REG21_REG, &data);
    data = REG32_FIELD_SET(data, 0x0, 9, BITS_MASK(9, 9));
    PHY_8295_SDSREG_WRITE(unit, mdxMacId, sdsId, RTL8295_SDS0_ANA_SPD_10P3125G_REG21_PAGE, RTL8295_SDS0_ANA_SPD_10P3125G_REG21_REG, data);

    /* soft reset */
    PHY_8295_PHYREG_WRITE(unit, mdxMacId, RTL8295_GLB_CTRL_SOFT_RESET_CTRL_PAGE, RTL8295_GLB_CTRL_SOFT_RESET_CTRL_REG, 0x6);
    return RT_ERR_OK;
}
#endif

#if defined(CONFIG_SDK_RTL8295R)
/*
 * Check RX error for 10G
 */
int32
_phy_8295r_fiberRx_10g_check(uint32 unit, rtk_port_t port, uint32 mdxMacId, uint32 sdsId, uint32 *pStatus)
{
    uint32                  data, rxIdle;

    *pStatus = PHY_FIBER_RX_STATUS_0;

    PHY_8295_PHYREG_READ(unit, mdxMacId, RTL8295_SDS_CTRL_SDS_10G_LINK_PAGE, RTL8295_SDS_CTRL_SDS_10G_LINK_REG, &data);
    if (data & RTL8295_SDS_CTRL_SDS_10G_LINK_SDS_S1TG_LINK_MASK)
    {
        //if (g8295rInfo[unit]->rxCaliStatus[port][1] != PHY_SDS_RXCALI_STATUS_OK)
        if (RTK_PORTMASK_IS_PORT_CLEAR(g8295rInfo[unit]->port10gReady, port))
        {
            *pStatus = PHY_FIBER_RX_STATUS_1;
            return RT_ERR_OK;
        }

        return RT_ERR_OK;
    }

    if (HWP_PHY_MODEL_BY_PORT(unit, port) == RTK_PHYTYPE_RTL8295R_C22)
    {
        if (_phy_8295r_es_10g_rxIdle(unit, port, mdxMacId, sdsId, &rxIdle) != RT_ERR_OK)
        {
            return RT_ERR_OK;
        }
    }
    else
    {
        PHY_8295_PHYREG_READ(unit, mdxMacId, RTL8295_SDS_CTRL_RXAUI_SEL_CHN_PAGE, RTL8295_SDS_CTRL_RXAUI_SEL_CHN_REG, &data);
        rxIdle = REG32_FIELD_GET(data, RTL8295_SDS_CTRL_RXAUI_SEL_CHN_RXIDLE_S1_OFFSET, RTL8295_SDS_CTRL_RXAUI_SEL_CHN_RXIDLE_S1_MASK);
    }

    if (rxIdle)
    {
        return RT_ERR_OK;
    }
    *pStatus = PHY_FIBER_RX_STATUS_1;
    return RT_ERR_OK;
}
#endif

#if defined(CONFIG_SDK_RTL8295R)
/*
 * reset RX for 10G
 */
int32
_phy_8295r_fiberRx_10g_reset(uint32 unit, rtk_port_t port, uint32 mdxMacId, uint32 sdsId)
{
    uint32          data, sdsCtrlData, sdsCtrlDataTmp;
    uint32          data_TGR_PRO_0_REG00, data_ANA_MISC_REG02, data_ANA_SPD;
    uint32          rxEnSelfOffset = RTL8295_SDS0_ANA_SPD_10P3125G_REG21_REG0_RX_EN_SELF_OFFSET, rxDisOffset = RTL8295_SDS_CTRL_SDS_CTRL_S1_RX_DISABLE_S1_OFFSET;

    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%d reset", unit, port);


    PHY_8295_PHYREG_READ(unit, mdxMacId, RTL8295_SDS_CTRL_SDS_CTRL_S1_PAGE, RTL8295_SDS_CTRL_SDS_CTRL_S1_REG, &sdsCtrlData);
    PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_ANA_SPD_10P3125G_REG21_PAGE, RTL8295_SDS0_ANA_SPD_10P3125G_REG21_REG, &data_ANA_SPD);

    /* RX_DISABLE=1 */
    sdsCtrlDataTmp = REG32_FIELD_SET(sdsCtrlData, 1, rxDisOffset, (0x1 << rxDisOffset));
    PHY_8295_PHYREG_WRITE(unit, mdxMacId, RTL8295_SDS_CTRL_SDS_CTRL_S1_PAGE, RTL8295_SDS_CTRL_SDS_CTRL_S1_REG, sdsCtrlDataTmp);

    /* PORT_LINK_TEST METHOD_1 */
    PHY_8295_PHYREG_READ(unit, mdxMacId, RTL8295_GLB_CTRL_SOFT_RESET_CTRL_PAGE, RTL8295_GLB_CTRL_SOFT_RESET_CTRL_REG, &data);
    data = REG32_FIELD_SET(data, 1, 1, 0x1<<1);
    PHY_8295_PHYREG_WRITE(unit, mdxMacId, RTL8295_GLB_CTRL_SOFT_RESET_CTRL_PAGE, RTL8295_GLB_CTRL_SOFT_RESET_CTRL_REG, data);

    /* RX_ENSELF=1 */
    data_ANA_SPD = REG32_FIELD_SET(data_ANA_SPD, 1, rxEnSelfOffset, (0x1 << rxEnSelfOffset));
    PHY_8295_SDSREG_WRITE(unit, mdxMacId, sdsId, RTL8295_SDS0_ANA_SPD_10P3125G_REG21_PAGE, RTL8295_SDS0_ANA_SPD_10P3125G_REG21_REG, data_ANA_SPD);

    /* RX_ENSELF=0 */
    data_ANA_SPD = REG32_FIELD_SET(data_ANA_SPD, 0, rxEnSelfOffset, (0x1 << rxEnSelfOffset));
    PHY_8295_SDSREG_WRITE(unit, mdxMacId, sdsId, RTL8295_SDS0_ANA_SPD_10P3125G_REG21_PAGE, RTL8295_SDS0_ANA_SPD_10P3125G_REG21_REG, data_ANA_SPD);

    /* FP_TGR0_SEL_BER_NOTIFY=0 -> FRC_BER_NOTIFY_ON=1 -> FP_TGR0_SEL_BER_NOTIFY=0 -> FRC_BER_NOTIFY_ON=0 */
    PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_TGR_PRO_0_REG00_PAGE, RTL8295_SDS0_TGR_PRO_0_REG00_REG, &data_TGR_PRO_0_REG00);
    /* FP_TGR0_SEL_BER_NOTIFY=0 */
    data = REG32_FIELD_SET(data_TGR_PRO_0_REG00, 0, RTL8295_SDS0_TGR_PRO_0_REG00_FP_TGR0_SEL_BER_NOTIFY_OFFSET, RTL8295_SDS0_TGR_PRO_0_REG00_FP_TGR0_SEL_BER_NOTIFY_MASK);
    PHY_8295_SDSREG_WRITE(unit, mdxMacId, sdsId, RTL8295_SDS0_TGR_PRO_0_REG00_PAGE, RTL8295_SDS0_TGR_PRO_0_REG00_REG, data);
    /* FRC_BER_NOTIFY_ON=1 */
    PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_ANA_MISC_REG02_PAGE, RTL8295_SDS0_ANA_MISC_REG02_REG, &data_ANA_MISC_REG02);
    data = REG32_FIELD_SET(data_ANA_MISC_REG02, 1, RTL8295_SDS0_ANA_MISC_REG02_FRC_BER_NOTIFY_ON_OFFSET, RTL8295_SDS0_ANA_MISC_REG02_FRC_BER_NOTIFY_ON_MASK);
    PHY_8295_SDSREG_WRITE(unit, mdxMacId, sdsId, RTL8295_SDS0_ANA_MISC_REG02_PAGE, RTL8295_SDS0_ANA_MISC_REG02_REG, data);
    /* FP_TGR0_SEL_BER_NOTIFY recover */
    PHY_8295_SDSREG_WRITE(unit, mdxMacId, sdsId, RTL8295_SDS0_ANA_MISC_REG02_PAGE, RTL8295_SDS0_ANA_MISC_REG02_REG, data_ANA_MISC_REG02);
    /* FP_TGR0_SEL_BER_NOTIFY=0 */
    data = REG32_FIELD_SET(data_TGR_PRO_0_REG00, 0, RTL8295_SDS0_TGR_PRO_0_REG00_FP_TGR0_SEL_BER_NOTIFY_OFFSET, RTL8295_SDS0_TGR_PRO_0_REG00_FP_TGR0_SEL_BER_NOTIFY_MASK);
    PHY_8295_SDSREG_WRITE(unit, mdxMacId, sdsId, RTL8295_SDS0_TGR_PRO_0_REG00_PAGE, RTL8295_SDS0_TGR_PRO_0_REG00_REG, data);



    /* RX_DISABLE=0 */
    /* Recover the reg value instead of force it to 0 because RX_DISABLE reg is also used by other API. */
    PHY_8295_PHYREG_WRITE(unit, mdxMacId, RTL8295_SDS_CTRL_SDS_CTRL_S1_PAGE, RTL8295_SDS_CTRL_SDS_CTRL_S1_REG, sdsCtrlData);

    return RT_ERR_OK;
}
#endif

#if defined(CONFIG_SDK_RTL8295R)
/*
 * reset RX for 1G
 */
int32
_phy_8295r_fiberRx_1g_reset(uint32 unit, rtk_port_t port, uint32 mdxMacId, uint32 sdsId)
{
    uint32          data, sdsCtrlData, sdsCtrlDataTmp;
    uint32          rxEnSelfOffset = RTL8295_SDS0_ANA_SPD_1P25G_REG21_REG0_RX_EN_SELF_OFFSET, rxDisOffset = RTL8295_SDS_CTRL_SDS_CTRL_S1_RX_DISABLE_S1_OFFSET;


    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%d reset", unit, port);

    PHY_8295_PHYREG_READ(unit, mdxMacId, RTL8295_SDS_CTRL_SDS_CTRL_S1_PAGE, RTL8295_SDS_CTRL_SDS_CTRL_S1_REG, &sdsCtrlData);
    PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_ANA_SPD_1P25G_REG21_PAGE, RTL8295_SDS0_ANA_SPD_1P25G_REG21_REG, &data);

    /* RX_DISABLE=1 */
    sdsCtrlDataTmp = REG32_FIELD_SET(sdsCtrlData, 1, rxDisOffset, (0x1 << rxDisOffset));
    PHY_8295_PHYREG_WRITE(unit, mdxMacId, RTL8295_SDS_CTRL_SDS_CTRL_S1_PAGE, RTL8295_SDS_CTRL_SDS_CTRL_S1_REG, sdsCtrlDataTmp);

    /* RX_ENSELF=1 */
    data = REG32_FIELD_SET(data, 1, rxEnSelfOffset, (0x1 << rxEnSelfOffset));
    PHY_8295_SDSREG_WRITE(unit, mdxMacId, sdsId, RTL8295_SDS0_ANA_SPD_1P25G_REG21_PAGE, RTL8295_SDS0_ANA_SPD_1P25G_REG21_REG, data);

    /* RX_ENSELF=0 */
    data = REG32_FIELD_SET(data, 0, rxEnSelfOffset, (0x1 << rxEnSelfOffset));
    PHY_8295_SDSREG_WRITE(unit, mdxMacId, sdsId, RTL8295_SDS0_ANA_SPD_1P25G_REG21_PAGE, RTL8295_SDS0_ANA_SPD_1P25G_REG21_REG, data);

    /* RX_DISABLE=0 */
    /* Recover the reg value instead of force it to 0 because RX_DISABLE reg is also used by other API. */
    PHY_8295_PHYREG_WRITE(unit, mdxMacId, RTL8295_SDS_CTRL_SDS_CTRL_S1_PAGE, RTL8295_SDS_CTRL_SDS_CTRL_S1_REG, sdsCtrlData);
    return RT_ERR_OK;
}
#endif


/* Function Name:
 *      _phy_8295_serdes01Xge_reset
 * Description:
 *      Reset 10GE serdes 0 and 1.
 * Input:
 *      unit        - unit id
 *      port        - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
int32
_phy_8295_serdes01Xge_reset(uint32 unit, rtk_port_t port)
{
    uint32 data1,data2;
    uint32 mdxMacId;

    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%d serdes rst", unit, port);

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);

    PHY_8295_PHYREG_READ(unit, mdxMacId, RTL8295_GLB_CTRL_SOFT_RESET_CTRL_PAGE, RTL8295_GLB_CTRL_SOFT_RESET_CTRL_REG, &data2);

    data2 = REG32_FIELD_SET(data2, 1, 1, (0x1<<1));
    PHY_8295_PHYREG_WRITE(unit, mdxMacId, RTL8295_GLB_CTRL_SOFT_RESET_CTRL_PAGE, RTL8295_GLB_CTRL_SOFT_RESET_CTRL_REG, data2);

    PHY_8295_SDSREG_READ(unit, mdxMacId, 0, RTL8295_SDS0_ANA_MISC_REG02_PAGE, RTL8295_SDS0_ANA_MISC_REG02_REG, &data1);
    PHY_8295_SDSREG_READ(unit, mdxMacId, 1, RTL8295_SDS0_ANA_MISC_REG02_PAGE, RTL8295_SDS0_ANA_MISC_REG02_REG, &data2);

    data1 = REG32_FIELD_SET(data1, 1, 10, (0x1<<10));
    data2 = REG32_FIELD_SET(data2, 1, 10, (0x1<<10));

    PHY_8295_SDSREG_WRITE(unit, mdxMacId, 0, RTL8295_SDS0_ANA_MISC_REG02_PAGE, RTL8295_SDS0_ANA_MISC_REG02_REG, data1);
    PHY_8295_SDSREG_WRITE(unit, mdxMacId, 1, RTL8295_SDS0_ANA_MISC_REG02_PAGE, RTL8295_SDS0_ANA_MISC_REG02_REG, data2);

    osal_time_mdelay(1);

    data1 = REG32_FIELD_SET(data1, 0, 10, (0x1<<10));
    data2 = REG32_FIELD_SET(data2, 0, 10, (0x1<<10));

    PHY_8295_SDSREG_WRITE(unit, mdxMacId, 0, RTL8295_SDS0_ANA_MISC_REG02_PAGE, RTL8295_SDS0_ANA_MISC_REG02_REG, data1);
    PHY_8295_SDSREG_WRITE(unit, mdxMacId, 1, RTL8295_SDS0_ANA_MISC_REG02_PAGE, RTL8295_SDS0_ANA_MISC_REG02_REG, data2);
    return RT_ERR_OK;
}


#if defined(CONFIG_SDK_RTL8295R)
/* Function Name:
 *      _phy_8295r_serdes_reset
 * Description:
 *      Reset 8295R port's serdes.
 * Input:
 *      unit        - unit id
 *      port        - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
int32
_phy_8295r_serdes_reset(uint32 unit, rtk_port_t port)
{
    return _phy_8295_serdes01Xge_reset(unit, port);
}



/* Function Name:
 *      phy_8295r_macIntfSerdes_reset
 * Description:
 *      Reset PHY's MAC interface SerDes
 * Input:
 *      unit     - unit id
 *      basePort - base port id of the PHY
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8295r_macIntfSerdes_reset(uint32 unit, rtk_port_t port)
{
    int32   ret;
    uint32  s0_mode, s1_mode;
    uint32  mdxMacId;

    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%d serdes rst", unit, port);

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);

    _phy_8295_serdesModeReg_get(unit, port, mdxMacId, 0, &s0_mode);
    _phy_8295_serdesModeReg_get(unit, port, mdxMacId, 1, &s1_mode);
    _phy_8295_serdesModeReg_set(unit, port, mdxMacId, 0, PHY_8295_SDS_MODE_OFF);
    _phy_8295_serdesModeReg_set(unit, port, mdxMacId, 1, PHY_8295_SDS_MODE_OFF);
    _phy_8295_serdesModeReg_set(unit, port, mdxMacId, 0, s0_mode);
    _phy_8295_serdesModeReg_set(unit, port, mdxMacId, 1, s1_mode);

    ret = _phy_8295_serdes01Xge_reset(unit, port);

    return ret;
}
#endif

#if defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      _phy_8214qf_serdes_reset
 * Description:
 *      Reset 8214QF port's serdes.
 * Input:
 *      unit        - unit id
 *      port        - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
int32
_phy_8214qf_serdes_reset(uint32 unit, rtk_port_t port)
{
    uint32 data1;
    uint32 mdxMacId;
    uint32 sdsId;

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);
    PHY_8295_SDSID_GET(unit, port, &sdsId);

    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u serdes %u rst", unit, port, sdsId);

    PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_ANA_MISC_REG02_PAGE, RTL8295_SDS0_ANA_MISC_REG02_REG, &data1);
    data1 = REG32_FIELD_SET(data1, 1, 10, (0x1<<10));
    PHY_8295_SDSREG_WRITE(unit, mdxMacId, sdsId, RTL8295_SDS0_ANA_MISC_REG02_PAGE, RTL8295_SDS0_ANA_MISC_REG02_REG, data1);

    osal_time_mdelay(1);

    data1 = REG32_FIELD_SET(data1, 0, 10, (0x1<<10));
    PHY_8295_SDSREG_WRITE(unit, mdxMacId, sdsId, RTL8295_SDS0_ANA_MISC_REG02_PAGE, RTL8295_SDS0_ANA_MISC_REG02_REG, data1);

    phy_8214qf_fiberRx_reset(unit, port);

    return RT_ERR_OK;
}
#endif


#if defined(CONFIG_SDK_RTL8295R)
/* Function Name:
 *      phy_8295r_fiberRx_check
 * Description:
 *      Check Fiber Rx status
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pStatus -Fiber RX status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8295r_fiberRx_check(uint32 unit, rtk_port_t port, uint32 *pStatus)
{
    uint32                  mdxMacId;
    uint32                  sdsId;
    rtk_enable_t            enable;

    *pStatus = PHY_FIBER_RX_STATUS_0;

    if (g8295ProcWa == FALSE)
    {
        return RT_ERR_OK;
    }

    if (!g8295rInfo[unit])
    {
        return RT_ERR_OK;
    }

    if (PORT_ENABLE_STATE(unit, port) == DISABLED)
    {
        return RT_ERR_OK;
    }

    if(phy_8295r_rxCali_process_sts_get(unit, port) != PHY_8295_RXCALI_PROCESS_STS_IDLE)
    {
        return RT_ERR_OK;
    }

    if ((phy_8295_8214_fiberRxEnable_get(unit, port, &enable) == RT_ERR_OK) && (enable == DISABLED))
    {
        /* RX is configured as disabled */
        return RT_ERR_OK;
    }

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);
    PHY_8295_SDSID_GET(unit, port, &sdsId);

    switch (PORT_10G_MEDIA(unit, port))
    {
      case PORT_10GMEDIA_FIBER_10G:
      case PORT_10GMEDIA_DAC_50CM:
      case PORT_10GMEDIA_DAC_100CM:
      case PORT_10GMEDIA_DAC_300CM:
      case PORT_10GMEDIA_DAC_500CM: //DAC_LONG
        _phy_8295r_fiberRx_10g_check(unit, port, mdxMacId, sdsId, pStatus);
        break;
      case PORT_10GMEDIA_FIBER_1G:
        _phy_8295r_fiberRx_1g_check(unit, port, mdxMacId, sdsId, pStatus);
        break;
      default:
        return RT_ERR_OK;
    }

    return RT_ERR_OK;
}
#endif

#if defined(CONFIG_SDK_RTL8295R)
/* Function Name:
 *      phy_8295r_fiberRx_reset
 * Description:
 *      Reset Fiber Rx part
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8295r_fiberRx_reset(uint32 unit, rtk_port_t port)
{
    uint32                  mdxMacId;
    uint32                  sdsId;
    uint32                  data;
    uint32                  rxCaliFlag = FALSE;

    if (!g8295rInfo[unit])
    {
        return RT_ERR_OK;
    }

    if(phy_8295r_rxCali_process_sts_get(unit, port) != PHY_8295_RXCALI_PROCESS_STS_IDLE)
    {
        return RT_ERR_OK;
    }

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);
    PHY_8295_SDSID_GET(unit, port, &sdsId);

    switch (PORT_10G_MEDIA(unit, port))
    {
      case PORT_10GMEDIA_FIBER_10G:
      case PORT_10GMEDIA_DAC_50CM:
      case PORT_10GMEDIA_DAC_100CM:
        PHY_8295_PHYREG_READ(unit, mdxMacId, RTL8295_SDS_CTRL_SDS_10G_LINK_PAGE, RTL8295_SDS_CTRL_SDS_10G_LINK_REG, &data);
        if ((data & RTL8295_SDS_CTRL_SDS_10G_LINK_SDS_S1TG_LINK_MASK) == 0) /* link is down */
        {
            _phy_8295r_fiberRx_10g_reset(unit, port, mdxMacId, sdsId);
        }
        rxCaliFlag = TRUE;
        break;
      case PORT_10GMEDIA_DAC_300CM:
      case PORT_10GMEDIA_DAC_500CM: //DAC_LONG
        RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%d long.rxCaliS1", unit, port);
        rxCaliFlag = TRUE;
        break;
      case PORT_10GMEDIA_FIBER_1G:
        _phy_8295r_fiberRx_1g_reset(unit, port, mdxMacId, sdsId);
        break;
      default:
        break;
    }


    if (rxCaliFlag == TRUE)
    {
        RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%d do rxCali", unit, port);
         if(phy_8295r_rxCali_process_sts_get(unit, port) != PHY_8295_RXCALI_PROCESS_STS_IDLE)
        {
            RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,RX CALIBRATION PROCESS is ongoing", unit, port);
            return RT_ERR_OK;
        }
        phy_8295r_rxCali_process_sts_set(unit, port, PHY_8295_RXCALI_PROCESS_STS_PROCESS);

        _phy_8295r_rxCali_process(unit, port);

        if (RTK_PORTMASK_IS_PORT_SET(g8295rInfo[unit]->port10gReady, port))
        {
            _switch_port_macForceLinkDown_enable(unit, port, DISABLED);
        }

        phy_8295r_rxCali_process_sts_set(unit, port, PHY_8295_RXCALI_PROCESS_STS_IDLE);
    }

    return RT_ERR_OK;
}
#endif

#if defined(CONFIG_SDK_RTL8295R)
/*
 * Initial S0
 */
int32
phy_8295r_s0_init(uint32 unit, rtk_port_t port)
{
    uint32          mdxMacId;
    uint32          data, val;

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);

    if (HWP_8390_FAMILY_ID(unit))
    {
        /* set switch medium as None */
        _switch_port_10gMedia_set(unit, port, PORT_10GMEDIA_NONE);
    }

    /* Enable link-pass-through (SDS_FRCLD_10G_EN = 1) */
    PHY_8295_PHYREG_READ(unit, mdxMacId, RTL8295_SDS_CTRL_SDS_REG_BCST_PAGE, RTL8295_SDS_CTRL_SDS_REG_BCST_REG, &data);

    val = (g8295rLinkPassThroughEn) ? 1 : 0;
    data = REG32_FIELD_SET(data, val, RTL8295_SDS_CTRL_SDS_REG_BCST_SDS_FRCLD_10G_EN_OFFSET, RTL8295_SDS_CTRL_SDS_REG_BCST_SDS_FRCLD_10G_EN_MASK);

    val = (g8295rS0SdsAutoChgEn) ? 1 : 0;
    data = REG32_FIELD_SET(data, val, RTL8295_SDS_CTRL_SDS_REG_BCST_SDS_AUTO_CHG_EN_OFFSET, RTL8295_SDS_CTRL_SDS_REG_BCST_SDS_AUTO_CHG_EN_MASK);

    PHY_8295_PHYREG_WRITE(unit, mdxMacId, RTL8295_SDS_CTRL_SDS_REG_BCST_PAGE, RTL8295_SDS_CTRL_SDS_REG_BCST_REG, data);

    /* disable S1 serdes */
    _phy_8295_portSdsMode_set(unit, port, PHY_8295_SDS_MODE_OFF);

    return RT_ERR_OK;
}
#endif

#if defined(CONFIG_SDK_RTL8295R)
/* Function Name:
 *      phy_8295r_portSdsRxCaliEnable_set
 * Description:
 *      Enable/Disable serdes RX cali of the port
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      enable  - enable state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8295r_portSdsRxCaliEnable_set(uint32 unit, uint32 port, uint32 sdsId, rtk_enable_t enable)
{
    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit %u port %u sdsId %u EN:%d", unit, port, sdsId, enable);

    if (g8295rInfo[unit] == NULL)
    {
        return RT_ERR_NULL_POINTER;
    }

    switch(sdsId)
    {
      case 0:
        if (enable == ENABLED)
        {
            RTK_PORTMASK_PORT_SET(g8295rInfo[unit]->s0RxCaliEn, port);
        }
        else
        {
            RTK_PORTMASK_PORT_CLEAR(g8295rInfo[unit]->s0RxCaliEn, port);
        }
        break;
      case 1:
        if (enable == ENABLED)
        {
            RTK_PORTMASK_PORT_SET(g8295rInfo[unit]->s1RxCaliEn, port);
        }
        else
        {
            RTK_PORTMASK_PORT_CLEAR(g8295rInfo[unit]->s1RxCaliEn, port);
            g8295rInfo[unit]->rxCaliStatus[port][1] = PHY_SDS_RXCALI_STATUS_NOINFO;
        }
        break;
      default:
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}
#endif


#if defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8214qf_fiberRx_check
 * Description:
 *      Check Fiber Rx status
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pStatus -Fiber RX status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8214qf_fiberRx_check(uint32 unit, rtk_port_t port, uint32 *pStatus)
{
    uint32          mdxMacId;
    uint32          sdsId;
    uint32          data, data1, link_mask, counter, rxIdleMsk=0;
    rtk_enable_t    enable;

    *pStatus = PHY_FIBER_RX_STATUS_0;

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);

    PHY_8295_SDSID_GET(unit, port, &sdsId);

    link_mask = 0x1 << (RTL8295_SDS_CTRL_SDS_1G_LINK_SDS_S4P0_LINK_OFFSET + sdsId - PHY_8295_1G_PORT_SDSID_START);
    PHY_8295_PHYREG_READ(unit, mdxMacId, RTL8295_SDS_CTRL_SDS_1G_LINK_PAGE, RTL8295_SDS_CTRL_SDS_1G_LINK_REG, &data);
    if (data & link_mask)
    {
        /* link is up */
        return RT_ERR_OK;
    }

    if (hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &data) != RT_ERR_OK)
    {
        return RT_ERR_OK;
    }
    if (data & PowerDown_MASK)
    {
        /* power is down */
        return RT_ERR_OK;
    }

    if ((phy_8295_8214_fiberRxEnable_get(unit, port, &enable) == RT_ERR_OK) && (enable == DISABLED))
    {
        /* RX is configured as disabled */
        return RT_ERR_OK;
    }

    /* check RX-Idle */
    if (HWP_PHY_MODEL_BY_PORT(unit, port) == RTK_PHYTYPE_RTL8214QF_NC5)
    {
        PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_SDS_EXT_REG31_PAGE, RTL8295_SDS0_SDS_EXT_REG31_REG, &data);
        if (data & PHY_8295_SDS_REG_SEP_RXIDLE_MASK)
        {
            return RT_ERR_OK;
        }
    }
    else
    {
        switch(sdsId)
        {
          case 4:
            rxIdleMsk = RTL8295_SDS_CTRL_RXAUI_SEL_CHN_RXIDLE_S4_MASK;
            break;
          case 5:
            rxIdleMsk = RTL8295_SDS_CTRL_RXAUI_SEL_CHN_RXIDLE_S5_MASK;
            break;
          case 6:
            rxIdleMsk = RTL8295_SDS_CTRL_RXAUI_SEL_CHN_RXIDLE_S6_MASK;
            break;
          case 7:
            rxIdleMsk = RTL8295_SDS_CTRL_RXAUI_SEL_CHN_RXIDLE_S7_MASK;
            break;
          default:
            return RT_ERR_OK;
        }
        PHY_8295_PHYREG_READ(unit, mdxMacId, RTL8295_SDS_CTRL_RXAUI_SEL_CHN_PAGE, RTL8295_SDS_CTRL_RXAUI_SEL_CHN_REG, &data);
        if (data & rxIdleMsk)
        {
            return RT_ERR_OK;
        }
    }


    if ((HWP_PHY_MODEL_BY_PORT(unit, port) == RTK_PHYTYPE_RTL8214QF_NC5))
    {
        /* select channel */
        PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_SDS_EXT_REG24_PAGE, RTL8295_SDS0_SDS_EXT_REG24_REG, &data);
        // bit[2:0] set 0
        data = REG32_FIELD_SET(data, 0, 0, 0x7);
        PHY_8295_SDSREG_WRITE(unit, mdxMacId, sdsId, RTL8295_SDS0_SDS_EXT_REG24_PAGE, RTL8295_SDS0_SDS_EXT_REG24_REG, data);
        /* check counter */

        PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_SDS_EXT_REG02_PAGE, RTL8295_SDS0_SDS_EXT_REG02_REG, &data);
        // bit[15:0]
        counter = REG32_FIELD_GET(data, 0, 0xFFFF);

        PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_SDS_EXT_REG03_PAGE, RTL8295_SDS0_SDS_EXT_REG03_REG, &data1);
        counter |= REG32_FIELD_GET(data1, 8, (0xF << 8)) << 15;
    }
    else
    {
        PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_SDS_EXT_REG00_PAGE, RTL8295_SDS0_SDS_EXT_REG00_REG, &data);
        counter = REG32_FIELD_GET(data, 0, 0xFFFF);
    }


    if (counter > 0)
    {
       *pStatus = PHY_FIBER_RX_STATUS_1;
       RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit %u port %u errCnt=%u", unit, port, counter);
    }
    return RT_ERR_OK;
}
#endif


#if defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8214qf_fiberRx_reset
 * Description:
 *      Reset Fiber Rx part
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8214qf_fiberRx_reset(uint32 unit, rtk_port_t port)
{
    uint32          ret = RT_ERR_OK;
    uint32          data, sdsCtrlData, sdsCtrlDataTmp;
    uint32          mdxMacId;
    uint32          sdsId;
    uint32          rxEnSelfOffset = 9, rxDisOffset = RTL8295_SDS_CTRL_SDS_CTRL_S4_RX_DISABLE_S4_OFFSET;
    uint32          sdsCtrlAdrrPage, sdsCtrlAdrrReg;

    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u fiberRx reset", unit, port);

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);
    PHY_8295_SDSID_GET(unit, port, &sdsId);

    if (sdsId == 4)
    {
        sdsCtrlAdrrPage = RTL8295_SDS_CTRL_SDS_CTRL_S4_PAGE;
        sdsCtrlAdrrReg = RTL8295_SDS_CTRL_SDS_CTRL_S4_REG;
    }
    else if (sdsId == 5)
    {
        sdsCtrlAdrrPage = RTL8295_SDS_CTRL_SDS_CTRL_S5_PAGE;
        sdsCtrlAdrrReg = RTL8295_SDS_CTRL_SDS_CTRL_S5_REG;
    }
    else if (sdsId == 6)
    {
        sdsCtrlAdrrPage = RTL8295_SDS_CTRL_SDS_CTRL_S6_PAGE;
        sdsCtrlAdrrReg = RTL8295_SDS_CTRL_SDS_CTRL_S6_REG;
    }
    else if (sdsId == 7)
    {
        sdsCtrlAdrrPage = RTL8295_SDS_CTRL_SDS_CTRL_S7_PAGE;
        sdsCtrlAdrrReg = RTL8295_SDS_CTRL_SDS_CTRL_S7_REG;
    }
    else
    {
        ret = RT_ERR_PORT_ID;
        RT_ERR(ret, (MOD_HAL), "unit=%u,port=%u invalid serdes id %u", unit, port, sdsId);
        return ret;
    }

    /* reset: RX_DISABLE=1 -> RX_ENSELF=1 -> RX_ENSELF=0 -> RX_DISABLE=0 */

    PHY_8295_PHYREG_READ(unit, mdxMacId, sdsCtrlAdrrPage, sdsCtrlAdrrReg, &sdsCtrlData);
    PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_ANA_SPD_1P25G_REG08_PAGE, RTL8295_SDS0_ANA_SPD_1P25G_REG08_REG, &data);

    /* RX_DISABLE=1 */
    sdsCtrlDataTmp = REG32_FIELD_SET(sdsCtrlData, 1, rxDisOffset, (0x1 << rxDisOffset));
    PHY_8295_PHYREG_WRITE(unit, mdxMacId, sdsCtrlAdrrPage, sdsCtrlAdrrReg, sdsCtrlDataTmp);

    /* RX_ENSELF=1 */
    data = REG32_FIELD_SET(data, 1, rxEnSelfOffset, (0x1 << rxEnSelfOffset));
    PHY_8295_SDSREG_WRITE(unit, mdxMacId, sdsId, RTL8295_SDS0_ANA_SPD_1P25G_REG08_PAGE, RTL8295_SDS0_ANA_SPD_1P25G_REG08_REG, data);

    /* RX_ENSELF=0 */
    data = REG32_FIELD_SET(data, 0, rxEnSelfOffset, (0x1 << rxEnSelfOffset));
    PHY_8295_SDSREG_WRITE(unit, mdxMacId, sdsId, RTL8295_SDS0_ANA_SPD_1P25G_REG08_PAGE, RTL8295_SDS0_ANA_SPD_1P25G_REG08_REG, data);

    /* RX_DISABLE=0 */
    /* Recover the reg value instead of force it to 0 because RX_DISABLE reg is also used by other API. */
    //sdsCtrlDataTmp = REG32_FIELD_SET(sdsCtrlDataTmp, 0, rxDisOffset, (0x1 << rxDisOffset));
    PHY_8295_PHYREG_WRITE(unit, mdxMacId, sdsCtrlAdrrPage, sdsCtrlAdrrReg, sdsCtrlData);

    /* clear counter */
    if ((HWP_PHY_MODEL_BY_PORT(unit, port) == RTK_PHYTYPE_RTL8214QF_NC5))
    {
        PHY_8295_SDSREG_WRITE(unit, mdxMacId, sdsId, RTL8295_SDS0_SDS_EXT_REG02_PAGE, RTL8295_SDS0_SDS_EXT_REG02_REG, 0);
    }
    else
    {
        PHY_8295_SDSREG_WRITE(unit, mdxMacId, sdsId, RTL8295_SDS0_SDS_EXT_REG00_PAGE, RTL8295_SDS0_SDS_EXT_REG00_REG, 0);
    }

    return RT_ERR_OK;
}
#endif

#if defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8214qf_macIntfSerdes_reset
 * Description:
 *      Reset PHY's MAC interface SerDes
 * Input:
 *      unit     - unit id
 *      basePort - base port id of the PHY
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8214qf_macIntfSerdes_reset(uint32 unit, rtk_port_t basePort)
{
    int32       ret;
    uint32      sdsMode, data;
    uint32      mdxMacId, rxEnSelfOffset = 4;

    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u macIntfSerdes reset", unit, basePort);

    PHY_8295_MDX_MACID_GET(unit, basePort, &mdxMacId);
    if ((ret = _phy_8295_serdesModeReg_get(unit, basePort, mdxMacId, 0, &sdsMode)) != RT_ERR_OK)
    {
        return ret;
    }

    if (sdsMode == PHY_8295_SDS_MODE_QSGMII)
    {
        /* 5G serdes reset */
        PHY_8295_SDSREG_READ(unit, mdxMacId, 0, RTL8295_SDS0_ANA_SPD_5G_REG21_PAGE, RTL8295_SDS0_ANA_SPD_5G_REG21_REG, &data);
        /* 0x2A, 0x15, bit4 = 1 */
        data = REG32_FIELD_SET(data, 1, rxEnSelfOffset, (0x1 << rxEnSelfOffset));
        PHY_8295_SDSREG_WRITE(unit, mdxMacId, 0, RTL8295_SDS0_ANA_SPD_5G_REG21_PAGE, RTL8295_SDS0_ANA_SPD_5G_REG21_REG, data);
        /* 0x2A, 0x15, bit4 = 0 */
        data = REG32_FIELD_SET(data, 0, rxEnSelfOffset, (0x1 << rxEnSelfOffset));
        PHY_8295_SDSREG_WRITE(unit, mdxMacId, 0, RTL8295_SDS0_ANA_SPD_5G_REG21_PAGE, RTL8295_SDS0_ANA_SPD_5G_REG21_REG, data);
    }
    else
    {
        /* 10G serdes reset */
        _phy_8295_serdes01Xge_reset(unit, basePort);
    }
    return RT_ERR_OK;
}
#endif

#if !defined(__BOOTLOADER__)
#if defined(CONFIG_SDK_RTL8214QF)
/* Function Name:
 *      phy_8214qf_sds_get
 * Description:
 *      Get PHY SerDes information
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      sdsCfg  - SerDes configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      None
 */
int32
phy_8214qf_sds_get(uint32 unit, rtk_port_t port, rtk_sdsCfg_t *sdsCfg)
{

    switch (PORT_SDS_MODE_CFG(unit, port))
    {
      case PHY_8295_SDS_MODE_SGMII:
        sdsCfg->sdsMode = RTK_MII_SGMII;
        break;
      case PHY_8295_SDS_MODE_FIB1G:
        sdsCfg->sdsMode = RTK_MII_1000BX_FIBER;
        break;
      case PHY_8295_SDS_MODE_FIB100M:
        sdsCfg->sdsMode = RTK_MII_100BX_FIBER;
        break;
      case PHY_8295_SDS_MODE_OFF:
        sdsCfg->sdsMode = RTK_MII_DISABLE;
        break;
      default:
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}
#endif/* #if defined(CONFIG_SDK_RTL8214QF) */
#endif/* #if !defined(__BOOTLOADER__) */

#if !defined(__BOOTLOADER__)
#if defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8224qf_sds_get
 * Description:
 *      Get PHY SerDes information
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      sdsCfg  - SerDes configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      None
 */
int32
phy_8224qf_sds_get(uint32 unit, rtk_port_t port, rtk_sdsCfg_t *sdsCfg)
{
    int32   ret;
    uint32  mode;

    if ((ret = _phy_8295_serdesMode_get(unit, port, &mode)) != RT_ERR_OK)
    {
        RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u sds mode get fail %x", unit, port, ret);
        return ret;
    }

    switch (mode)
    {
      case PHY_8295_SDS_MODE_SGMII:
        sdsCfg->sdsMode = RTK_MII_SGMII;
        break;
      case PHY_8295_SDS_MODE_FIB2_5G:
        sdsCfg->sdsMode = RTK_MII_2500Base_X;
        break;
      case PHY_8295_SDS_MODE_FIB1G:
        sdsCfg->sdsMode = RTK_MII_1000BX_FIBER;
        break;
      case PHY_8295_SDS_MODE_FIB100M:
        sdsCfg->sdsMode = RTK_MII_100BX_FIBER;
        break;
      case PHY_8295_SDS_MODE_OFF:
        sdsCfg->sdsMode = RTK_MII_DISABLE;
        break;
      default:
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}
#endif/* #if defined(CONFIG_SDK_RTL8224QF) */
#endif/* #if !defined(__BOOTLOADER__) */

#if !defined(__BOOTLOADER__)
#if defined(CONFIG_SDK_RTL8214QF)
/* Function Name:
 *      phy_8214qf_sds_set
 * Description:
 *      Set PHY SerDes information
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      sdsCfg  - SerDes configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      None
 */
int32
phy_8214qf_sds_set(uint32 unit, rtk_port_t port, rtk_sdsCfg_t *sdsCfg)
{
    int32       ret = RT_ERR_OK;
    uint32      mode;
    uint32      speed;

    switch (sdsCfg->sdsMode)
    {
      case RTK_MII_SGMII:
        mode = PHY_8295_SDS_MODE_SGMII;
        speed = PORT_SPEED_1000M;
        break;
      case RTK_MII_1000BX_FIBER:
        mode = PHY_8295_SDS_MODE_FIB1G;
        speed = PORT_SPEED_1000M;
        break;
      case RTK_MII_100BX_FIBER:
        mode = PHY_8295_SDS_MODE_FIB100M;
        speed = PORT_SPEED_100M;
        break;
      case RTK_MII_DISABLE:
        mode = PHY_8295_SDS_MODE_OFF;
        speed = PORT_SPEED_1000M;
        break;
      default:
        return RT_ERR_PORT_NOT_SUPPORTED;
    }

    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,sdsMode=0x%x,PS=%d", unit, port, mode, PORT_ENABLE_STATE(unit, port));

    phy_8214qf_speed_set(unit, port, speed);

    PORT_SDS_MODE_CFG(unit, port) = mode;

    _phy_8295_portSdsMode_set(unit, port, PHY_8295_SDS_MODE_OFF);
    ret = _phy_8295_portSdsMode_set(unit, port, mode);
    _phy_8214qf_serdes_reset(unit, port);

    return ret;
}
#endif/* #if defined(CONFIG_SDK_RTL8214QF) */
#endif/* #if !defined(__BOOTLOADER__) */


#if !defined(__BOOTLOADER__)
#if defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8224qf_sds_set
 * Description:
 *      Set PHY SerDes information
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      sdsCfg  - SerDes configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      None
 */
int32
phy_8224qf_sds_set(uint32 unit, rtk_port_t port, rtk_sdsCfg_t *sdsCfg)
{
    int32       ret = RT_ERR_OK;
    uint32      mode;
    uint32      speed;
    rtk_port_10gMedia_t media;

    media = PORT_10G_MEDIA(unit, port);
    if (PORT_10GMEDIA_FIBER_1G == media)
    {
        switch (sdsCfg->sdsMode)
        {
          case RTK_MII_SGMII:
            mode = PHY_8295_SDS_MODE_SGMII;
            speed = PORT_SPEED_1000M;
            break;
          case RTK_MII_1000BX_FIBER:
            mode = PHY_8295_SDS_MODE_FIB1G;
            speed = PORT_SPEED_1000M;
            break;
          default:
            return RT_ERR_PORT_NOT_SUPPORTED;
        }
    }
    else
    {
        RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,media=%d is not fiber1g", unit, port, media);
        return RT_ERR_FAILED;
    }

    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,media=%d,sdsMode=0x%x,PS=%d", unit, port, media, mode, PORT_ENABLE_STATE(unit, port));
    phy_8224qf_speed_set(unit, port, speed);

    _phy_8295_portSdsMode_set(unit, port, PHY_8295_SDS_MODE_OFF);
    ret = _phy_8295_portSdsMode_set(unit, port, mode);
    _phy_8214qf_serdes_reset(unit, port);

    return ret;
}
#endif/* #if defined(CONFIG_SDK_RTL8224QF) */
#endif/* #if !defined(__BOOTLOADER__) */

#if defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_82x4qf_sdsSgmiiStatus_get
 * Description:
 *      Set PHY SerDes SGMII status of linl, speed, and duplex
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pLink   - link up or down
 *      pSpeed  - link speed
 *      pDuplex - duplex status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_ALLOWED      - serdes is not in SGMII mode
 * Note:
 *      None
 */
int32
phy_82x4qf_sdsSgmiiStatus_get(uint32 unit, rtk_port_t port, rtk_port_linkStatus_t *pLink, uint32 *pSpeed, uint32 *pDuplex)
{
    int32   ret;
    uint32  mode, mdxMacId, sdsId, data, speedSel;


    if (PORT_SDS_MODE_CFG(unit, port) != PHY_8295_SDS_MODE_SGMII)
    {
        return RT_ERR_NOT_ALLOWED;
    }

    if ((ret = _phy_8295_serdesMode_get(unit, port, &mode)) != RT_ERR_OK)
    {
        return ret;
    }

    if (mode != PHY_8295_SDS_MODE_SGMII)
    {
        /* serdes may be disabled. return down */
        *pLink = PORT_LINKDOWN;
        *pDuplex = 1;
        *pSpeed = PORT_SPEED_10M;
        //RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,sdsMode=0x%x (not SGMII)", unit, port, mode);
        return RT_ERR_OK;
    }

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);
    PHY_8295_SDSID_GET(unit, port, &sdsId);
    /* read the register only when serdes mode is in SGMII mode */
    PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_FIB_REG05_PAGE, RTL8295_SDS0_FIB_REG05_REG, &data);
    *pLink =  (REG32_FIELD_GET(data, PHY_8295_SDS_PAGE02_FIB_REG05_SGMII_LINK_OFFSET, PHY_8295_SDS_PAGE02_FIB_REG05_SGMII_LINK_MASK) == 1) ? PORT_LINKUP : PORT_LINKDOWN;
    *pDuplex = REG32_FIELD_GET(data, PHY_8295_SDS_PAGE02_FIB_REG05_SGMII_DUPLEX_OFFSET, PHY_8295_SDS_PAGE02_FIB_REG05_SGMII_DUPLEX_MASK);
    speedSel = REG32_FIELD_GET(data, PHY_8295_SDS_PAGE02_FIB_REG05_SGMII_SPEED_OFFSET, PHY_8295_SDS_PAGE02_FIB_REG05_SGMII_SPEED_MASK);
    //RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,data=0x%x,link=%d,duplex=%u,speed=%x", unit, port, data, *pLink, *pDuplex, speedSel);
    if (speedSel == 0x2)
        *pSpeed = PORT_SPEED_1000M;
    else if (speedSel == 0x1)
        *pSpeed = PORT_SPEED_100M;
    else if (speedSel == 0x0)
        *pSpeed = PORT_SPEED_10M;
    else
        return RT_ERR_OUT_OF_RANGE;

    return RT_ERR_OK;
}
#endif


#if defined(CONFIG_SDK_RTL8295R)
/* Function Name:
 *      phy_8295r_sdsRxCaliStatus_get
 * Description:
 *      Get PHY SerDes rx-calibration status
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      sdsId   - serdes ID
 * Output:
 *      rtk_port_phySdsRxCaliStatus_t   - rx-calibration status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_UNIT_ID - invalid unit id
 *      RT_ERR_OUT_OF_RANGE - invalid serdes id
 * Applicable:
 *      8295r
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
phy_8295r_sdsRxCaliStatus_get(uint32 unit, rtk_port_t port, uint32 sdsId, rtk_port_phySdsRxCaliStatus_t *pStatus)
{

    if (g8295rInfo[unit] == NULL)
    {
        return RT_ERR_NOT_INIT;
    }

    if (sdsId >= PHY_8295_10G_SDS_MAX)
    {
        return RT_ERR_OUT_OF_RANGE;
    }

    *pStatus = g8295rInfo[unit]->rxCaliStatus[port][sdsId];
    return RT_ERR_OK;

}
#endif


#if defined(CONFIG_SDK_RTL8295R) || defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8295_8214_sdsTestMode_set
 * Description:
 *      Set SerDes test mode.
 * Input:
 *      unit        - unit id
 *      port        - base mac ID number of the PHY
 *      sdsId       - SerDes id
 *      testMode    - test mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT         - invalid port id
 * Note:
 *      None
 */
int32
phy_8295_8214_sdsTestMode_set(uint32 unit, rtk_port_t port, uint32 sdsId, rtk_sds_testMode_t testMode)
{
    uint32 mdxMacId = 0;

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);

    switch (testMode)
    {
        case RTK_SDS_TESTMODE_DISABLE:
            hal_rtl8295_sds_write(unit, mdxMacId, sdsId, 0x5, 0xA, 0x0000);
            hal_rtl8295_sds_write(unit, mdxMacId, sdsId, 0x6, 0xe, 0x0000);
            hal_rtl8295_sds_write(unit, mdxMacId, sdsId, 0x6, 0xf, 0x0000);
            hal_rtl8295_sds_write(unit, mdxMacId, sdsId, 0x6, 0x0, 0x0000);
            break;
        case RTK_SDS_TESTMODE_PRBS7:
            hal_rtl8295_sds_write(unit, mdxMacId, sdsId, 0x5, 0xA, 0x3000);
            hal_rtl8295_sds_write(unit, mdxMacId, sdsId, 0x6, 0xe, 0x0002);
            hal_rtl8295_sds_write(unit, mdxMacId, sdsId, 0x6, 0xf, 0x0002);
            hal_rtl8295_sds_write(unit, mdxMacId, sdsId, 0x6, 0x0, 0x6200);
            break;
        case RTK_SDS_TESTMODE_PRBS9:
            hal_rtl8295_sds_write(unit, mdxMacId, sdsId, 0x5, 0xA, 0x00C0);
            hal_rtl8295_sds_write(unit, mdxMacId, sdsId, 0x6, 0xe, 0x0002);
            hal_rtl8295_sds_write(unit, mdxMacId, sdsId, 0x6, 0xf, 0x0002);
            hal_rtl8295_sds_write(unit, mdxMacId, sdsId, 0x6, 0x0, 0x6200);
            break;
        case RTK_SDS_TESTMODE_PRBS31:
            hal_rtl8295_sds_write(unit, mdxMacId, sdsId, 0x5, 0xA, 0x0030);
            hal_rtl8295_sds_write(unit, mdxMacId, sdsId, 0x6, 0xe, 0x0002);
            hal_rtl8295_sds_write(unit, mdxMacId, sdsId, 0x6, 0xf, 0x0002);
            hal_rtl8295_sds_write(unit, mdxMacId, sdsId, 0x6, 0x0, 0x6200);
            break;
        case RTK_SDS_TESTMODE_SQUARE8:
            hal_rtl8295_sds_write(unit, mdxMacId, sdsId, 0x5, 0xA, 0x0002);
            hal_rtl8295_sds_write(unit, mdxMacId, sdsId, 0x6, 0x0, 0x6280);
            break;
        default:
            return RT_ERR_INPUT;
    }

    return RT_ERR_OK;
}
#endif

#if defined(CONFIG_SDK_RTL8295R) || defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8295_8214_sdsTestModeCnt_get
 * Description:
 *      Get SerDes test mode test pattern error counter.
 * Input:
 *      unit        - unit id
 *      port        - base mac ID number of the PHY
 *      sdsId       - SerDes id
 * Output:
 *      pCnt        - test pattern error counter
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT         - invalid port id
 * Note:
 *      The test pattern error counter register is read-clear.
 */
int32
phy_8295_8214_sdsTestModeCnt_get(uint32 unit, rtk_port_t port, uint32 sdsId, uint32 *pCnt)
{
    uint32 mdxMacId = 0;
    uint32 data = 0;

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);

    hal_rtl8295_sds_read(unit, mdxMacId, sdsId, 0x5, 0xB, &data);
    *pCnt = data;

    return RT_ERR_OK;
}
#endif


#if defined(CONFIG_SDK_RTL8295R)
/* Function Name:
 *      _phy_8295r_mdcProto_sel
 * Description:
 *      Select protocol for mdc access
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - fail
 * Note:
 *      None
 */
int32
_phy_8295r_mdcProto_sel(uint32 unit, rtk_port_t port)
{
    drv_smi_mdxProtoSel_t   proto = DRV_SMI_MDX_PROTO_C22;

    if (   (HWP_8390_50_FAMILY(unit))
        || (HWP_PHY_MODEL_BY_PORT(unit, port) == RTK_PHYTYPE_RTL8295R_C22))
    {
        g8295rStatusSwPoll = TRUE;
        return RT_ERR_OK;
    }

    g8295rStatusSwPoll = FALSE;
    if (hal_miim_portSmiMdxProto_get(unit, port, &proto) != RT_ERR_OK)
    {
        return RT_ERR_OK;
    }

    if (proto == DRV_SMI_MDX_PROTO_C22)
    {
        g8295rStatusSwPoll = TRUE;
        RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u use C22 and sw-poll enable", unit, port);
    }
    else
    {
        RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u use C45 and HW polling. sw-poll disable", unit, port);
        g8295rStatusSwPoll = FALSE;
    }
    return RT_ERR_OK;
}
#endif

#if defined(CONFIG_SDK_RTL8295R)
/* Function Name:
 *      _phy_8295r_rxCaliEn_init
 * Description:
 *      S0 rx cali. is needed for PCB trace length >= 10 inch
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
void
_phy_8295r_rxCaliEn_init(uint32 unit, rtk_port_t port, rtk_portmask_t *ps1RxCaliEn, rtk_portmask_t *ps0RxCaliEn)
{
    phy_8295_rxCaliConf_t   rxCaliConf;

    /* default set enable for S0 */
    RTK_PORTMASK_PORT_SET(*ps0RxCaliEn, port);
    /* default set enable for S1 */
    RTK_PORTMASK_PORT_SET(*ps1RxCaliEn, port);

    if (phy_8295r_rxCaliConfPort_get(unit, port, &rxCaliConf) == RT_ERR_OK)
    {
        if (rxCaliConf.s1.rxCaliEnable == DISABLED)
            RTK_PORTMASK_PORT_CLEAR(*ps1RxCaliEn, port);

        if (rxCaliConf.s0.rxCaliEnable == DISABLED)
            RTK_PORTMASK_PORT_CLEAR(*ps0RxCaliEn, port);
    }
    else
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "unit=%u,port=%u get rxCaliConf failed", unit, port);
    }
}
#endif


/* Function Name:
 *      phy_8295r_init
 * Description:
 *      Initialize PHY 8295R
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
#if defined(CONFIG_SDK_RTL8295R)
int32
phy_8295r_init(uint32 unit, rtk_port_t port)
{
    uint32      sdsId;

    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u", unit, port);

    if (HWP_PHY_BASE_MACID(unit, port) == port)
    {
        /* initial config for adjust reg and process flow */
      #if defined(CONFIG_SDK_RTL8390)
        if (HWP_8390_50_FAMILY(unit))
        {
            g8295rMacAutoDetectSdsMode  = FALSE;
            g8295rLinkPassThroughEn     = FALSE;
            g8295rS0SdsAutoChgEn        = FALSE;
        }
        else
      #endif
        if (((sdsId = HWP_PORT_SDSID(unit, port)) != HWP_NONE))
        {
            if (HWP_SDS_MODE(unit, sdsId) == RTK_MII_USXGMII_10GSXGMII)
            {
                g8295rMacAutoDetectSdsMode  = TRUE;
                g8295rLinkPassThroughEn     = FALSE;
                g8295rS0SdsAutoChgEn        = TRUE;
            }
            else //RTK_MII_10GR
            {
                g8295rMacAutoDetectSdsMode  = FALSE;
                if (HWP_PHY_MODEL_BY_PORT(unit, port) == RTK_PHYTYPE_RTL8295R_C22)
                {
                    g8295rLinkPassThroughEn     = TRUE;
                }
                else
                {
                    g8295rLinkPassThroughEn     = FALSE;
                }
                g8295rS0SdsAutoChgEn        = FALSE;
            }
        }

        if (g8295rInfo[unit] == NULL)
        {
            int32       sz = sizeof(phy_8295r_info_t);
            if ((g8295rInfo[unit] = osal_alloc(sz)) == NULL)
            {
                RT_ERR(RT_ERR_MEM_ALLOC, (MOD_HAL), "unit=%u,port=%u", unit, port);
                return RT_ERR_MEM_ALLOC;
            }
            osal_memset(g8295rInfo[unit], 0, sz);
        }
        /* create RX Cali semaphore */
        g8295rInfo[unit]->rxCali_sem[port] = osal_sem_mutex_create();

        g8295rInfo[unit]->rxCali_process_sts[port] = PHY_8295_RXCALI_PROCESS_STS_IDLE;
        _phy_8295r_rxCaliEn_init(unit, port, &g8295rInfo[unit]->s1RxCaliEn, &g8295rInfo[unit]->s0RxCaliEn);

        if (HWP_8390_FAMILY_ID(unit))
        {
            /* probe/init for MAC SerDes driver if needed */
            _switch_serdesDrv_init(unit, port);
        }
    }

    _phy_8295r_mdcProto_sel(unit, port);

    if (HWP_UNIT_VALID_LOCAL(unit))
    {
        phy_8295r_s0_init(unit, port);
    }

    _phy_8295r_10gMediaDb_init(unit, port);

   return RT_ERR_OK;
}
#endif


#if defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      _phy_8214qf_rxCali_needed
 * Description:
 *      rx cali. is needed for PCB trace length >= 10 inch
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      None
 * Return:
 *      TRUE - rx calibration is needed on the S0 or S1
 *      FALSE - rx calibration is not needed on the S0 or S1
 * Note:
 *      None
 */
int32
_phy_8214qf_rxCali_needed(uint32 unit, rtk_port_t basePort, uint32 sdsId)
{
    phy_8295_rxCaliConf_t   rxCaliConf;

    if (phy_8214qf_rxCaliConf_get(unit, basePort, &rxCaliConf) == RT_ERR_OK)
    {
        RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,sdsId=%u,conf.s0.rxCaliEnable=%u,conf.s1.rxCaliEnable=%u", unit, basePort, sdsId, rxCaliConf.s0.rxCaliEnable, rxCaliConf.s1.rxCaliEnable);
        if ((sdsId == 1) && (rxCaliConf.s1.rxCaliEnable == ENABLED))
            return TRUE;
        else if ((sdsId == 0) && (rxCaliConf.s0.rxCaliEnable == ENABLED))
            return TRUE;
        else
            return FALSE;
    }
    else
    {
        RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u get rxCaliConf failed", unit);
    }

    return FALSE;
}

/* Function Name:
 *      _phy_8214qf_rxCali
 * Description:
 *      Perform rx calibration if it is needed
 * Input:
 *      unit - unit id
 *      basePort - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      None
 */
int32
_phy_8214qf_rxCali(uint32 unit, rtk_port_t basePort)
{
    uint32      mdxMacId, mode, sdsId;
    phy_8295_rxCaliParam_t  rxCaliParam;

    PHY_8295_MDX_MACID_GET(unit, basePort, &mdxMacId);

    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u", unit, basePort);
    sdsId = 1;
    mode = PHY_8295_SDS_MODE_OFF;
    if (   (_phy_8295_serdesModeReg_get(unit, basePort, mdxMacId, sdsId, &mode) == RT_ERR_OK)
        && ((mode == PHY_8295_SDS_MODE_XSGMII) || (mode == PHY_8295_SDS_MODE_USXGMII))
        && (_phy_8214qf_rxCali_needed(unit, basePort, sdsId)))
    {
        RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,sds=%u rxCali", unit, basePort, sdsId);
        _phy_8295_rxCaliParam_get(unit, basePort, sdsId, &rxCaliParam);
        phy_8295_rxCali_start(unit, basePort, sdsId, &rxCaliParam);
    }
    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,sdsId=%u,mode=%u", unit, basePort, sdsId, mode);

    sdsId = 0;
    mode = PHY_8295_SDS_MODE_OFF;
    if (   (_phy_8295_serdesModeReg_get(unit, basePort, mdxMacId, sdsId, &mode) == RT_ERR_OK)
        && ((mode == PHY_8295_SDS_MODE_XSGMII) || (mode == PHY_8295_SDS_MODE_USXGMII))
        && (_phy_8214qf_rxCali_needed(unit, basePort, sdsId)))
    {
        RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,sds=%u rxCali", unit, basePort, sdsId);
        _phy_8295_rxCaliParam_get(unit, basePort, sdsId, &rxCaliParam);
        phy_8295_rxCali_start(unit, basePort, sdsId, &rxCaliParam);
    }
    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,sdsId=%u,mode=%u", unit, basePort, sdsId, mode);

    return RT_ERR_OK;
}
#endif



/* Function Name:
 *      phy_8295_sdsLeq_get
 * Description:
 *      Get the statue of LEQ of the specific PHY's SerDes in the specific unit
 * Input:
 *      unit    - unit id
 *      port    - base port id of the PHY
 *      sdsId   - SerDes of the PHY
 * Output:
 *      pManual_en - pointer to manual LEQ config satus
 *      pLeq_val   - pointer to current LEQ value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
int32
phy_8295_sdsLeq_get(uint32 unit, rtk_port_t basePort, uint32 sdsId, rtk_enable_t *pManual_en, uint32 *pLeq_val)
{
    uint32      mdxMacId, data;

    if ((sdsId != 0) && (sdsId != 1))
    {
        return RT_ERR_INPUT;
    }

    if (basePort != HWP_PHY_BASE_MACID(unit, basePort))
    {
        return RT_ERR_PORT_ID;
    }

    PHY_8295_MDX_MACID_GET(unit, basePort, &mdxMacId);
    /* 0x2E 0x18 [15:15] */
    PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_ANA_SPD_10P3125G_REG24_PAGE, RTL8295_SDS0_ANA_SPD_10P3125G_REG24_REG, &data);
    if (REG32_FIELD_GET(data, RTL8295_SDS0_ANA_SPD_10P3125G_REG24_REG0_EQ_SELREG_OFFSET, RTL8295_SDS0_ANA_SPD_10P3125G_REG24_REG0_EQ_SELREG_MASK) == 1)
    {
        *pManual_en = ENABLED;
    }
    else
    {
        *pManual_en = DISABLED;
    }

    /* 0x2E 0x16 [14:8] */
    PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_ANA_SPD_10P3125G_REG22_PAGE, RTL8295_SDS0_ANA_SPD_10P3125G_REG22_REG, &data);
    *pLeq_val = REG32_FIELD_GET(data, RTL8295_SDS0_ANA_SPD_10P3125G_REG22_REG0_FILTER_OUT_OFFSET, RTL8295_SDS0_ANA_SPD_10P3125G_REG22_REG0_FILTER_OUT_MASK);
    return RT_ERR_OK;
}


/* Function Name:
 *      phy_8295_sdsLeq_set
 * Description:
 *      Get the statue of LEQ of the specific PHY's SerDes in the specific unit
 * Input:
 *      unit    - unit id
 *      port    - base port id of the PHY
 *      sdsId   - SerDes of the PHY
 *      manual_en - ENABLED: LEQ in manual-mode; DISABLED: LEQ is auto-mode.
 *      leq_val - Fixed LEQ value when manual_en is set to ENABLED;
 *                this field is not used in driver when manual_en set to DISABLED, just keep it set to 0.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
int32
phy_8295_sdsLeq_set(uint32 unit, rtk_port_t basePort, uint32 sdsId, rtk_enable_t manual_en, uint32 leq_val)
{
    uint32      mdxMacId, data, data_reg24;

    if ((sdsId != 0) && (sdsId != 1))
    {
        return RT_ERR_INPUT;
    }

    if (basePort != HWP_PHY_BASE_MACID(unit, basePort))
    {
        return RT_ERR_PORT_ID;
    }

    PHY_8295_MDX_MACID_GET(unit, basePort, &mdxMacId);

    if (manual_en == ENABLED)
    {
        /* 0x2E 0x18 [15:15] = 0 */
        PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_ANA_SPD_10P3125G_REG24_PAGE, RTL8295_SDS0_ANA_SPD_10P3125G_REG24_REG, &data_reg24);
        data_reg24 = REG32_FIELD_SET(data_reg24, 0, RTL8295_SDS0_ANA_SPD_10P3125G_REG24_REG0_EQ_SELREG_OFFSET, RTL8295_SDS0_ANA_SPD_10P3125G_REG24_REG0_EQ_SELREG_MASK);
        PHY_8295_SDSREG_WRITE(unit, mdxMacId, sdsId, RTL8295_SDS0_ANA_SPD_10P3125G_REG24_PAGE, RTL8295_SDS0_ANA_SPD_10P3125G_REG24_REG, data_reg24);

        /* 0x2E 0x17 [7:7] = 0 */
        PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_ANA_SPD_10P3125G_REG23_PAGE, RTL8295_SDS0_ANA_SPD_10P3125G_REG23_REG, &data);
        data = REG32_FIELD_SET(data, 0, RTL8295_SDS0_ANA_SPD_10P3125G_REG23_REG0_EQHOLD_OFFSET, RTL8295_SDS0_ANA_SPD_10P3125G_REG23_REG0_EQHOLD_MASK);
        PHY_8295_SDSREG_WRITE(unit, mdxMacId, sdsId, RTL8295_SDS0_ANA_SPD_10P3125G_REG23_PAGE, RTL8295_SDS0_ANA_SPD_10P3125G_REG23_REG, data);

        /* 0x2E 0x16 [14:8] = leq_val */
        PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_ANA_SPD_10P3125G_REG22_PAGE, RTL8295_SDS0_ANA_SPD_10P3125G_REG22_REG, &data);
        data = REG32_FIELD_SET(data, leq_val, RTL8295_SDS0_ANA_SPD_10P3125G_REG22_REG0_FILTER_OUT_OFFSET, RTL8295_SDS0_ANA_SPD_10P3125G_REG22_REG0_FILTER_OUT_MASK);
        PHY_8295_SDSREG_WRITE(unit, mdxMacId, sdsId, RTL8295_SDS0_ANA_SPD_10P3125G_REG22_PAGE, RTL8295_SDS0_ANA_SPD_10P3125G_REG22_REG, data);

        /* 0x2E 0x18 [15:15] = 1 */
        data_reg24 = REG32_FIELD_SET(data_reg24, 1, RTL8295_SDS0_ANA_SPD_10P3125G_REG24_REG0_EQ_SELREG_OFFSET, RTL8295_SDS0_ANA_SPD_10P3125G_REG24_REG0_EQ_SELREG_MASK);
        PHY_8295_SDSREG_WRITE(unit, mdxMacId, sdsId, RTL8295_SDS0_ANA_SPD_10P3125G_REG24_PAGE, RTL8295_SDS0_ANA_SPD_10P3125G_REG24_REG, data_reg24);
    }
    else
    {
        /* 0x2E 0x18 [15:15] = 0 */
        PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_ANA_SPD_10P3125G_REG24_PAGE, RTL8295_SDS0_ANA_SPD_10P3125G_REG24_REG, &data_reg24);
        data_reg24 = REG32_FIELD_SET(data_reg24, 0, RTL8295_SDS0_ANA_SPD_10P3125G_REG24_REG0_EQ_SELREG_OFFSET, RTL8295_SDS0_ANA_SPD_10P3125G_REG24_REG0_EQ_SELREG_MASK);
        PHY_8295_SDSREG_WRITE(unit, mdxMacId, sdsId, RTL8295_SDS0_ANA_SPD_10P3125G_REG24_PAGE, RTL8295_SDS0_ANA_SPD_10P3125G_REG24_REG, data_reg24);

        /* 0x2E 0x17 [7:7] = 0 */
        PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_ANA_SPD_10P3125G_REG23_PAGE, RTL8295_SDS0_ANA_SPD_10P3125G_REG23_REG, &data);
        data = REG32_FIELD_SET(data, 0, RTL8295_SDS0_ANA_SPD_10P3125G_REG23_REG0_EQHOLD_OFFSET, RTL8295_SDS0_ANA_SPD_10P3125G_REG23_REG0_EQHOLD_MASK);
        PHY_8295_SDSREG_WRITE(unit, mdxMacId, sdsId, RTL8295_SDS0_ANA_SPD_10P3125G_REG23_PAGE, RTL8295_SDS0_ANA_SPD_10P3125G_REG23_REG, data);

        /* 0x2E 0x17 [7:7] = 1 */
        data = REG32_FIELD_SET(data, 1, RTL8295_SDS0_ANA_SPD_10P3125G_REG23_REG0_EQHOLD_OFFSET, RTL8295_SDS0_ANA_SPD_10P3125G_REG23_REG0_EQHOLD_MASK);
        PHY_8295_SDSREG_WRITE(unit, mdxMacId, sdsId, RTL8295_SDS0_ANA_SPD_10P3125G_REG23_PAGE, RTL8295_SDS0_ANA_SPD_10P3125G_REG23_REG, data);
    }

    return RT_ERR_OK;
}



#if defined(CONFIG_SDK_RTL8214QF)
/* Function Name:
 *      phy_8214qf_init
 * Description:
 *      Initialize PHY 8214QF
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8214qf_init(uint32 unit, rtk_port_t port)
{
    uint32      basePort;


    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u", unit, port);

    if (g8295rInfo[unit] == NULL)
    {
        int32       sz = sizeof(phy_8295r_info_t);
        if ((g8295rInfo[unit] = osal_alloc(sz)) == NULL)
        {
            RT_ERR(RT_ERR_MEM_ALLOC, (MOD_HAL), "unit=%u,port=%u", unit, port);
            return RT_ERR_MEM_ALLOC;
        }
        osal_memset(g8295rInfo[unit], 0, sz);
    }

    PORT_SDS_MODE_CFG(unit, port) = PHY_8295_SDS_MODE_FIB1G;

    if (HWP_UNIT_VALID_LOCAL(unit))
    {
        if ((basePort = HWP_PHY_BASE_MACID(unit, port)) == port)
        {
            _phy_8214qf_rxCali(unit, basePort);
        }

        _phy_8295_portSdsMode_set(unit, port, PHY_8295_SDS_MODE_FIB1G);
    }

    return RT_ERR_OK;
}
#endif


#if defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8224qf_init
 * Description:
 *      Initialize PHY 8214QF
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8224qf_init(uint32 unit, rtk_port_t port)
{
    uint32      basePort;


    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u", unit, port);

    if (g8295rInfo[unit] == NULL)
    {
        int32       sz = sizeof(phy_8295r_info_t);
        if ((g8295rInfo[unit] = osal_alloc(sz)) == NULL)
        {
            RT_ERR(RT_ERR_MEM_ALLOC, (MOD_HAL), "unit=%u,port=%u", unit, port);
            return RT_ERR_MEM_ALLOC;
        }
        osal_memset(g8295rInfo[unit], 0, sz);
    }

    PORT_10G_MEDIA(unit, port) = PORT_10GMEDIA_FIBER_2_5G;

    if (HWP_UNIT_VALID_LOCAL(unit))
    {
        if ((basePort = HWP_PHY_BASE_MACID(unit, port)) == port)
        {
            _phy_8214qf_rxCali(unit, basePort);
        }

        _phy_8295_portSdsMode_set(unit, port, PHY_8295_SDS_MODE_OFF);
    }

    return RT_ERR_OK;
}
#endif


/* Function Name:
 *      _phy_8295_eyeMon_sds_read
 * Description:
 *      serdes read for eye monitor
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      sdsId   - serdes ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
_phy_8295_eyeMon_sds_read(uint32 unit, rtk_port_t mdxMacId, uint32 sdsId, uint32 page, uint32 reg, uint32 *pData)
{
    int32       ret;
    ret = hal_rtl8295_sds_read(unit, mdxMacId, sdsId, page, reg, pData);
    return ret;
}

/* Function Name:
 *      _phy_8295_eyeMon_sds_write
 * Description:
 *      serdes write for eye monitor
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      sdsId   - serdes ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
_phy_8295_eyeMon_sds_write(uint32 unit, rtk_port_t mdxMacId, uint32 sdsId, uint32 page, uint32 reg, uint32 data)
{
    int32       ret;
    ret = hal_rtl8295_sds_write(unit, mdxMacId, sdsId, page, reg, data);
    return ret;
}

/* Function Name:
 *      _phy_8295_eyeMon_sds_writeBits
 * Description:
 *      serdes write bits for eye monitor
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      sdsId   - serdes ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
_phy_8295_eyeMon_sds_writeBits(uint32 unit, uint32 mdxMacId, uint32 sdsId, uint32 page, uint32 reg, uint32 mBit, uint32 lBit, uint32 val)
{
    int32           ret;
    uint32          data;
    uint32          mask = (uint32)((((uint64)0x1 << (mBit - lBit + 1)) - 1) << lBit);

    if ((ret = _phy_8295_eyeMon_sds_read(unit, mdxMacId, sdsId, page, reg, &data)) != RT_ERR_OK)
    {
        return ret;
    }

    data = REG32_FIELD_SET(data, val, lBit, mask);
    if ((ret = _phy_8295_eyeMon_sds_write(unit, mdxMacId, sdsId, page, reg, data)) != RT_ERR_OK)
    {
        RT_8295_EYEMON_LOG("unit:%u macId:%u sdsId:%u page:0x%x reg:0x%x data:0x%x (set mbit:%u lbit:%u val:%x) fail", unit, mdxMacId, sdsId, page, reg, data, mBit, lBit, val);
    }

    if (rtl8295EyeMonRegChk)
    {
        uint32      rb_val;
        data = 0;
        _phy_8295_eyeMon_sds_read(unit, mdxMacId, sdsId, page, reg, &data);
        rb_val = REG32_FIELD_GET(data, lBit, mask);
        if (rb_val != val)
        {
            RT_8295_EYEMON_LOG("unit:%u macId:%u sdsId:%u page:0x%x reg:0x%x data:0x%x (set mbit:%u lbit:%u val:%x) readback %x check fail",
                    unit, mdxMacId, sdsId, page, reg, data, mBit, lBit, val, rb_val);
        }
    }

    return ret;
}

/* Function Name:
 *      _phy_8295_eyeMon_sds_writeBits
 * Description:
 *      eye monitor set PI_EN and PI_ST
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      sdsId   - serdes ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
void
_phy_8295_eyeMon_pi_set(uint32 unit, uint32 mdxMacId, uint32 sdsId, uint32 idx)
{
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
    piEn = piEnList[idx/16];
    piSt = piStList[(idx/16)%2][idx%16];

    if (rtl8295EyeMonDbg)
    {
        RT_8295_EYEMON_LOG("idx:%u piEn:0x%04X piSt:0x%04X", idx, piEn, piSt);
    }

    _phy_8295_eyeMon_sds_writeBits(unit, mdxMacId, sdsId, 0x2e, 0x1a, 9, 6, piEn);
    _phy_8295_eyeMon_sds_writeBits(unit, mdxMacId, sdsId, 0x2e, 0x1b, 15, 0, piSt);

}

/* Function Name:
 *      _phy_8295_eyeMon_sds_writeBits
 * Description:
 *      eye monitor set REF
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      sdsId   - serdes ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
void
_phy_8295_eyeMon_ref_set(uint32 unit, uint32 mdxMacId, uint32 sdsId, uint32 idx)
{
    if (rtl8295EyeMonDbg)
    {
        RT_8295_EYEMON_LOG("idx:%u", idx);
    }

    _phy_8295_eyeMon_sds_writeBits(unit, mdxMacId, sdsId, 0x2e, 0x1a, 5, 0, idx);
}

/* Function Name:
 *      _phy_8295_eyeMon_scan_en
 * Description:
 *      eye monitor eanble scan
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      sdsId   - serdes ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
void
_phy_8295_eyeMon_scan_en(uint32 unit, uint32 mdxMacId, uint32 sdsId)
{
    RT_8295_EYEMON_LOG("unit:%u macId:%u sdsId:%u enable eye scan", unit, mdxMacId, sdsId);
    _phy_8295_eyeMon_sds_writeBits(unit, mdxMacId, sdsId, 0x2e, 0x8, 15, 15, 0x0);
    /* eye_scan_en: 1 -> 0 -> 1 */
    _phy_8295_eyeMon_sds_writeBits(unit, mdxMacId, sdsId, 0x2e, 0x8, 14, 14, 0x1);
    _phy_8295_eyeMon_sds_writeBits(unit, mdxMacId, sdsId, 0x2e, 0x8, 14, 14, 0x0);
    _phy_8295_eyeMon_sds_writeBits(unit, mdxMacId, sdsId, 0x2e, 0x8, 14, 14, 0x1);

    _phy_8295_eyeMon_sds_writeBits(unit, mdxMacId, sdsId, 0x2e, 0x2, 12, 11, 0x1);
}

/* Function Name:
 *      _phy_8295_eyeMon_MDIOCLK
 * Description:
 *      eye monitor mdio clock
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      sdsId   - serdes ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
void
_phy_8295_eyeMon_MDIOCLK(uint32 unit, uint32 port, uint32 sdsId)
{
    uint32      CKMDIO;

    _phy_8295_eyeMon_sds_read(unit, port, sdsId, 0x2F, 0x1D, &CKMDIO);
    CKMDIO=CKMDIO & 0xFFDF;
    CKMDIO=CKMDIO | 0x0020;
    _phy_8295_eyeMon_sds_write(unit, port, sdsId, 0x2F, 0x1D, CKMDIO);

    _phy_8295_eyeMon_sds_read(unit, port, sdsId, 0x2F, 0x1D, &CKMDIO);
    CKMDIO=CKMDIO & 0xFFDF;
    CKMDIO=CKMDIO | 0x0000;
    _phy_8295_eyeMon_sds_write(unit, port, sdsId, 0x2F, 0x1D, CKMDIO);

}

/* Function Name:
 *      _phy_8295_eyeMon_readOut
 * Description:
 *      eye monitor data read out
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      sdsId   - serdes ID
 * Output:
 *      pdata   - return data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
_phy_8295_eyeMon_readOut(uint32 unit, uint32 mdxMacId, uint32 sdsId, uint32 *pdata)
{
    /* WDIG_PAGE, WDIG_REG16 */
    return _phy_8295_eyeMon_sds_read(unit, mdxMacId, ((sdsId==1) ? PHY_8295_SDSID_S0_SLV : sdsId), 0x1F, 0x10, pdata);
}

/* Function Name:
 *      _phy_8295_eyeMon_eyePointRead
 * Description:
 *      eye monitor read eye point
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      sdsId   - serdes ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
_phy_8295_eyeMon_eyePointRead(uint32 unit, uint32 port, uint32 sdsId, uint32 *val)
{
    int32   retry = 100, ret = RT_ERR_OK;

    do
    {
        _phy_8295_eyeMon_MDIOCLK(unit, port, sdsId);

        _phy_8295_eyeMon_readOut(unit, port, sdsId, val);
        if (rtl8295EyeMonDbg == TRUE)
        {
            RT_8295_EYEMON_LOG("S%u 0x1F 0x10 = 0x%x", sdsId, *val);
        }
        retry--;
        if (retry <= 0)
        {
            ret = RT_ERR_BUSYWAIT_TIMEOUT;
            RT_ERR(ret, (MOD_HAL), "unit %u macId %u sds %u readOut retry timeout. val=%x", unit, port, sdsId, *val);
            break;
        }
    } while (((*val >> 14) & 0x3) != 0x3);

    return ret;
}


/* Function Name:
 *      _phy_8295_eyeMon_proc
 * Description:
 *      Trigger eye monitor process function
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      sds     - serdes ID
 *      frameNum- frame number
 *      pHdlr   - handler
 * Output:
 *      pDb     - output database
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
_phy_8295_eyeMon_proc(uint32 unit, uint32 mdxMacId, uint32 sdsId, uint32 frameNum, phy_sds_eyeMon_hdlr_t pHdlr, void *pDb)
{
    uint32  x, y, frame;
    uint32  ReadBack;
    uint32  val;
    int32   ret = RT_ERR_FAILED;

    /* enable */
    _phy_8295_eyeMon_scan_en(unit, mdxMacId, sdsId);

    /* set x */
    for (x = 0; x < RTK_EYE_MON_X_MAX; ++x)
    {
        _phy_8295_eyeMon_pi_set(unit, mdxMacId, sdsId, x);

        /* set y */
        for (y = 0; y < RTK_EYE_MON_Y_MAX; ++y)
        {
            _phy_8295_eyeMon_ref_set(unit, mdxMacId, sdsId, y);

            _phy_8295_eyeMon_MDIOCLK(unit, mdxMacId, sdsId);

            _phy_8295_eyeMon_readOut(unit, mdxMacId, sdsId, &ReadBack);

            /* get val */
            for (frame = 0; frame < frameNum; ++frame)
            {
                if ((ret = _phy_8295_eyeMon_eyePointRead(unit, mdxMacId, sdsId, &val)) != RT_ERR_OK)
                {
                    goto EXIT;
                }
                pHdlr(x, y, frame, pDb, val);
            }
        }
    }

EXIT:
    return ret;
}


/* Function Name:
 *      phy_8295_eyeMonitor_start
 * Description:
 *      Trigger eye monitor function
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      sdsId   - serdes ID
 *      frameNum- frame number
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8295_eyeMonitor_start(uint32 unit, uint32 port, uint32 sdsId, uint32 frameNum)
{
    uint32  mdxMacId;
    uint32  x, y, frame;
    uint32  *eyeData = NULL;
    uint32  data_size;
    int32   ret;

    if ((sdsId != 0) && (sdsId != 1))
    {
        return RT_ERR_INPUT;
    }

    if (frameNum > RTK_EYE_MON_FRAME_MAX)
    {
        RT_8295_EYEMON_LOG("frameNum %u exceed maxmum %u!", frameNum, RTK_EYE_MON_FRAME_MAX);
        return RT_ERR_FAILED;
    }

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);

    data_size = sizeof(uint32) * RTK_EYE_MON_FRAME_MAX * RTK_EYE_MON_X_MAX * RTK_EYE_MON_Y_MAX;
    if ((eyeData = osal_alloc(data_size)) == NULL)
    {
        RT_8295_EYEMON_LOG("malloc %u fail!\n", data_size);
        return RT_ERR_FAILED;
    }
    osal_memset(eyeData, 0, data_size);

    RT_8295_EYEMON_LOG("uint:%u port:%u sdsId:%u frameNum:%u", unit, port, sdsId, frameNum);

    if ((ret = _phy_8295_eyeMon_proc(unit, mdxMacId, sdsId, frameNum, phy_common_eyeMonPixel_get, (void *)eyeData)) != RT_ERR_OK)
    {
        goto EXIT;
    }

    for (x = 0; x < RTK_EYE_MON_X_MAX; ++x)
    {
        for (y = 0; y < RTK_EYE_MON_Y_MAX; ++y)
        {
            for (frame = 0; frame < frameNum; ++frame)
            {
                osal_printf("[%d, %d : %d]\n", x, y, eyeData[RTK_EYE_MON_DATA_POS(frame, x, y)]);
            }
        }
    }

EXIT:
    if (eyeData)
    {
        osal_free(eyeData);
    }
    return RT_ERR_OK;
}


/* Function Name:
 *      phy_8295_eyeMonitorInfo_get
 * Description:
 *      Get eye monitor height and width
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      sdsId   - serdes ID
 *      frameNum- frame number
 * Output:
 *      pInfo   - eye monitor information
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8295_eyeMonitorInfo_get(uint32 unit, uint32 port, uint32 sds, uint32 frameNum, rtk_sds_eyeMonInfo_t *pInfo)
{
    rtk_eye_monBmap_t   *eye;
    int32           xAxis, yAxis;
    uint32          data_size, i;
    uint8           maxHeight, height;
    uint8           maxWidth, width;
    uint8           width_sample_pos[] = {(RTK_EYE_MON_Y_MAX - 10), (RTK_EYE_MON_Y_MAX + 10)};
    int32           ret;
    uint32          mdxMacId;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PHY), "unit=%u,port=%u,sds=%u,frameNum=%u", unit, port, sds, frameNum);

    RT_PARAM_CHK(frameNum > RTK_EYE_MON_FRAME_MAX, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pInfo), RT_ERR_NULL_POINTER);

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);

    data_size = sizeof(rtk_eye_monBmap_t) * RTK_EYE_MON_X_MAX;
    if ((eye = osal_alloc(data_size)) == NULL)
    {
        return RT_ERR_MEM_ALLOC;
    }
    osal_memset(eye, 0, data_size);

    if ((ret = _phy_8295_eyeMon_proc(unit, mdxMacId, sds, frameNum, phy_common_eyeMonInfo_update, (void *)eye)) != RT_ERR_OK)
    {
        goto ERR;
    }

    maxHeight = 0;
    for (xAxis = 0; xAxis < RTK_EYE_MON_X_MAX; ++xAxis)
    {
        height = 0;
        for (yAxis = 0; yAxis < RTK_EYE_MON_ARXIS_Y_MAX; ++yAxis)
        {
            if (BITMAP_IS_SET(eye[xAxis].arr, yAxis))
            {
                if (maxHeight < height)
                {
                    maxHeight = height;
                }

                height = 0;
            }
            else
                ++height;
        }

        if (maxHeight < height)
        {
            maxHeight = height;
        }
    }

    pInfo->height = maxHeight;

    maxWidth = 0;
    for (i = 0; i < sizeof(width_sample_pos)/sizeof(uint8); ++i)
    {
        yAxis = width_sample_pos[i];
        width = 0;
        for (xAxis = 0; xAxis < RTK_EYE_MON_X_MAX; ++xAxis)
        {
            if (BITMAP_IS_SET(eye[xAxis].arr, yAxis))
            {
                if (maxWidth < width)
                {
                    maxWidth = width;
                }

                width = 0;
            }
            else
                ++width;
        }

        if (maxWidth < width)
        {
            maxWidth = width;
        }
    }

    pInfo->width = maxWidth;

ERR:
    osal_free(eye);
    return ret;
}


/* Function Name:
 *      phy_8295r_portEyeMonitor_start
 * Description:
 *      Trigger eye monitor function
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      sdsId   - serdes id
 *      frameNum- frame number
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8295r_portEyeMonitor_start(uint32 unit, rtk_port_t port, uint32 sdsId, uint32 frameNum)
{
    return phy_8295_eyeMonitor_start(unit, port, sdsId, frameNum);
}

#if defined(CONFIG_SDK_RTL8214QF)
/* Function Name:
 *      phy_8214qf_macIntfSdsRxCali_start
 * Description:
 *      Start Rx Calibration of a PHY's SerDes
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      sdsId   - serdes id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8214qf_macIntfSdsRxCali_start(uint32 unit, rtk_port_t port, uint32 sdsId)
{
    int32       ret;
    phy_8295_rxCaliParam_t  rxCaliParam;

    _phy_8295_rxCaliParam_get(unit, port, sdsId, &rxCaliParam);
    ret = phy_8295_rxCali_start(unit, port, sdsId, &rxCaliParam);
    return ret;
}
#endif

#if defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8214qf_macIntfSerdesMode_get
 * Description:
 *      Get PHY's MAC side serdes mode
 * Input:
 *      unit   - unit id
 *      port    - Base port ID of the PHY
 * Output:
 *      pMode   - serdes mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID
 * Note:
 *      None
 */
int32
phy_8214qf_macIntfSerdesMode_get(uint32 unit, rtk_port_t port, rt_serdesMode_t *pMode)
{
    uint32      mdxMacId, mode, sdsId;
    int32       ret;

    if (port != HWP_PHY_BASE_MACID(unit, port))
    {
        return RT_ERR_PORT_ID;
    }

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);
    sdsId = 0;
    if ((ret = _phy_8295_serdesModeReg_get(unit, port, mdxMacId, sdsId, &mode)) != RT_ERR_OK)
    {
        return ret;
    }

    return _phy_8295_regSdsModeToRtSdsMode(mode, pMode);
}
#endif


#if defined(CONFIG_SDK_RTL8214QF)
/* Function Name:
 *      phy_8214qf_macIntfSerdesLinkStatus_get
 * Description:
 *      Get PHY's MAC side interface serdes link status
 * Input:
 *      unit    - unit ID
 *      port    - Base port ID of the PHY
 * Output:
 *      pStatus - link status of the SerDes
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8214qf_macIntfSerdesLinkStatus_get(uint32 unit, rtk_port_t port, rtk_phy_macIntfSdsLinkStatus_t *pStatus)
{
    uint32      mdxMacId, mode, data, sdsId;
    int32       ret;

    if (port != HWP_PHY_BASE_MACID(unit, port))
    {
        return RT_ERR_PORT_ID;
    }

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);
    osal_memset(pStatus, 0, sizeof(rtk_phy_macIntfSdsLinkStatus_t));

    sdsId = 0;
    if ((ret = _phy_8295_serdesModeReg_get(unit, port, mdxMacId, sdsId, &mode)) != RT_ERR_OK)
    {
        return ret;
    }

    PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_SDS_EXT_REG29_PAGE, RTL8295_SDS0_SDS_EXT_REG29_REG, &data);
    PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_SDS_EXT_REG29_PAGE, RTL8295_SDS0_SDS_EXT_REG29_REG, &data);
    pStatus->link_status[sdsId] = ((data & 0x1FF) == 0x1FF) ? PORT_LINKUP : PORT_LINKDOWN;
    if (mode == PHY_8295_SDS_MODE_QSGMII)
    {
        pStatus->sds_num = 1;
        return RT_ERR_OK;
    }

    sdsId = 1;
    PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_SDS_EXT_REG29_PAGE, RTL8295_SDS0_SDS_EXT_REG29_REG, &data);
    PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_SDS_EXT_REG29_PAGE, RTL8295_SDS0_SDS_EXT_REG29_REG, &data);
    pStatus->link_status[sdsId] = ((data & 0x1FF) == 0x1FF) ? PORT_LINKUP : PORT_LINKDOWN;
    pStatus->sds_num = 2;
    return RT_ERR_OK;
}
#endif


#if defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8224qf_macIntfSerdesLinkStatus_get
 * Description:
 *      Get PHY's MAC side interface serdes link status
 * Input:
 *      unit    - unit ID
 *      port    - Base port ID of the PHY
 * Output:
 *      pStatus - link status of the SerDes
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8224qf_macIntfSerdesLinkStatus_get(uint32 unit, rtk_port_t port, rtk_phy_macIntfSdsLinkStatus_t *pStatus)
{
    uint32      mdxMacId, mode, data, sdsId;
    int32       ret;

    if (port != HWP_PHY_BASE_MACID(unit, port))
    {
        return RT_ERR_PORT_ID;
    }

    PHY_8295_MDX_MACID_GET(unit, port, &mdxMacId);
    osal_memset(pStatus, 0, sizeof(rtk_phy_macIntfSdsLinkStatus_t));

    sdsId = 0;
    if ((ret = _phy_8295_serdesModeReg_get(unit, port, mdxMacId, sdsId, &mode)) != RT_ERR_OK)
    {
        return ret;
    }

    if (mode != PHY_8295_SDS_MODE_USXGMII)
    {
        RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u S0 sds mode 0x%x is not USXGMII", unit, port, mode);
        return RT_ERR_FAILED;
    }

    PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_TGR_STD_0_REG01_PAGE, RTL8295_SDS0_TGR_STD_0_REG01_REG, &data);
    PHY_8295_SDSREG_READ(unit, mdxMacId, sdsId, RTL8295_SDS0_TGR_STD_0_REG01_PAGE, RTL8295_SDS0_TGR_STD_0_REG01_REG, &data);

    if ((data & RTL8295_SDS0_TGR_STD_0_REG01_FP_PCS1_RX_LINK_MASK) && (data & RTL8295_SDS0_TGR_STD_0_REG01_FP_PCS1_FAULT_DET_MASK))
        pStatus->link_status[sdsId] = PORT_LINKUP;
    else
        pStatus->link_status[sdsId] = PORT_LINKDOWN;
    pStatus->sds_num = 1;
    return RT_ERR_OK;
}
#endif

/*
 * for diag
 */
int32
phy_8295_diag_set(uint32 unit, rtk_port_t port, rtk_port_t mdxMacId, uint32 sds, uint8 *name)
{
    /*
     * Command:  diag set sc patch <UINT:port> <UINT:mdx_macId> <UINT:serdes> <STRING:name>
     */
    if (!osal_strcmp((char *)name, "rxCali"))
    {
        RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,mdxMacId=%u,sdsId=%u", unit, port, mdxMacId, sds);

#if defined(CONFIG_SDK_RTL8295R)
        if (HWP_PHY_MODEL_BY_PORT(unit, port) == RTK_PHYTYPE_RTL8295R)
        {
            if (phy_8295r_rxCali_process_sts_get(unit, port) != PHY_8295_RXCALI_PROCESS_STS_IDLE)
            {
                RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u PHY_8295_RXCALI_PROCESS is processing!!!", unit, port);
                return RT_ERR_NOT_ALLOWED;
            }
            phy_8295r_rxCali_process_sts_set(unit, port, PHY_8295_RXCALI_PROCESS_STS_PROCESS);
        }
#endif

#if defined(CONFIG_SDK_RTL8295R)
        if ((HWP_PHY_MODEL_BY_PORT(unit, port) == RTK_PHYTYPE_RTL8295R) && (sds == 1))
        {
            _phy_8295r_rxCaliS1(unit, port, 0);
        }
        else
#endif
        {
            phy_8295_rxCaliParam_t  rxCaliParam;
            _phy_8295_rxCaliParam_get(unit, port, sds, &rxCaliParam);
            phy_8295_rxCali_start(unit, port, sds, &rxCaliParam);
        }

#if defined(CONFIG_SDK_RTL8295R)
        if (HWP_PHY_MODEL_BY_PORT(unit, port) == RTK_PHYTYPE_RTL8295R)
        {
            phy_8295r_rxCali_process_sts_set(unit, port, PHY_8295_RXCALI_PROCESS_STS_IDLE);
        }
#endif

    }
#if defined(CONFIG_SDK_RTL8295R)
    else if (!osal_strcmp((char *)name, "port_cali_enable"))
    {
        int32   ret;
        RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,S%u cali enable", unit, port, sds);
        if ((ret = phy_8295r_portSdsRxCaliEnable_set(unit, port, sds, ENABLED)) != RT_ERR_OK)
        {
            osal_printf("unit=%u,port=%u,S%u cali enable failed %x", unit, port, sds, ret);
        }
    }
    else if (!osal_strcmp((char *)name, "port_cali_disable"))
    {
        int32   ret;
        RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,S%u cali disable", unit, mdxMacId, sds);
        if ((ret = phy_8295r_portSdsRxCaliEnable_set(unit, port, sds, DISABLED)) != RT_ERR_OK)
        {
            osal_printf("unit=%u,port=%u,S%u cali disable failed %x", unit, port, sds, ret);
        }
    }
#endif
    else if (!osal_strcmp((char *)name, "link_chg_enable"))
    {
        g8295ProcLinkChg = TRUE;
        osal_printf("g8295ProcLinkChg=%u", g8295ProcLinkChg);
    }
    else if (!osal_strcmp((char *)name, "link_chg_disable"))
    {
        g8295ProcLinkChg = FALSE;
        osal_printf("g8295ProcLinkChg=%u", g8295ProcLinkChg);
    }
    else if (!osal_strcmp((char *)name, "wa_enable"))
    {
        g8295ProcWa = TRUE;
        osal_printf("g8295ProcWa=%u", g8295ProcWa);
    }
    else if (!osal_strcmp((char *)name, "wa_disable"))
    {
        g8295ProcWa = FALSE;
        osal_printf("g8295ProcWa=%u", g8295ProcWa);
    }
    else if (!osal_strcmp((char *)name, "dump"))
    {
#if defined(CONFIG_SDK_RTL8295R)
        osal_printf("g8295rMacAutoDetectSdsMode=%u\n", g8295rMacAutoDetectSdsMode);
        osal_printf("g8295rLinkPassThroughEn=%u\n", g8295rLinkPassThroughEn);
        osal_printf("g8295rS0SdsAutoChgEn=%u\n", g8295rS0SdsAutoChgEn);
        osal_printf("g8295rStatusSwPoll=%u\n", g8295rStatusSwPoll);
#endif
        osal_printf("g8295ProcWa=%u\n", g8295ProcWa);
        osal_printf("g8295ProcLinkChg=%u\n", g8295ProcLinkChg);
    }
    else if (!osal_strcmp((char *)name, "cmu_dump"))
    {
        uint32      data, Band_value, Band_state;
        hal_rtl8295_reg_write(unit, port, 0x1C, 0x2000);            //chipreg w 0x1c 0x2000
        hal_rtl8295_sds_write(unit, port, 0, 0x1F, 0x2, 0x0044);    //sdsrw w 0 0x1F 0x2 0x0044
        hal_rtl8295_sds_read(unit, port, 0, 0x21, 0x0, &data);
        data = REG32_FIELD_SET(data, 1, 5, (0x1 << 5));
        hal_rtl8295_sds_write(unit, port, 0, 0x21, 0x0, data);      //sdsrwb w 0 0x21 0x0 5 5 1
        hal_rtl8295_reg_read(unit, port, 0x20, &data);              //set datat01 [chipreg r 0x20]
        Band_value = REG32_FIELD_GET(data, 10, (0x1F << 10));       //set ::Band_value [readbit $datat01 14 10]
        osal_printf(" Band_val=%u\n", Band_value);                  //puts " Band_val=$::Band_value"
        hal_rtl8295_reg_read(unit, port, 0x24, &data);              //set datat02 [chipreg r 0x24]
        Band_state = REG32_FIELD_GET(data, 1, (0x7 << 1));          //set ::Band_state [readbit $datat02 3 1]
        osal_printf(" Band_state=%u\n", Band_state);                //puts " Band_state=$::Band_state"
        hal_rtl8295_sds_write(unit, port, 0, 0x1F, 0x2, 0x0000);    //sdsrw w 0 0x1F 0x2 0x0000
        hal_rtl8295_sds_read(unit, port, 0, 0x21, 0x0, &data);
        data = REG32_FIELD_SET(data, 0, 5, (0x1 << 5));
        hal_rtl8295_sds_write(unit, port, 0, 0x21, 0x0, data);      //sdsrwb w 0 0x21 0x0 5 5 0
    }
    else
    {
        RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "diag command not found!");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      _phy_8295_s0s1Diag_dump
 * Description:
 *      dump S0/S1 status registers
 * Input:
 *      unit - unit id
 *      port - base mac ID number of the PHY
 *      mdxMacId - MDC/MDIO MAC ID
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
int32
_phy_8295_s0s1Diag_dump(uint32 unit, rtk_port_t port, rtk_port_t mdxMacId)
{
    uint32      data0, data1;

    osal_printf("unit %u port %u serdes sym-err\n", unit, port);
    /* S0: page 276 reg 17 [7:0]. S1: page 788 reg 17 [7:0] */
    PHY_8295_SDSREG_READ(unit, mdxMacId, 0, RTL8295_SDS0_TGR_STD_1_REG01_PAGE, RTL8295_SDS0_TGR_STD_1_REG01_REG, &data0);
    PHY_8295_SDSREG_READ(unit, mdxMacId, 1, RTL8295_SDS0_TGR_STD_1_REG01_PAGE, RTL8295_SDS0_TGR_STD_1_REG01_REG, &data1);
    osal_printf("  S0=0x%x, S1=0x%x\n", (REG32_FIELD_GET(data0, 0, (0xFF << 0))), (REG32_FIELD_GET(data1, 0, (0xFF << 0))));

    return RT_ERR_OK;
}


/* Function Name:
 *      _phy_8214qf_diag_dump
 * Description:
 *      dump 8214QF status register
 * Input:
 *      unit - unit id
 *      port - base mac ID number of the PHY
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
int32
_phy_8214qf_diag_dump(uint32 unit, uint32 port)
{
    int32       ret;

    if (port != HWP_PHY_BASE_MACID(unit, port))
    {
        osal_printf("port %u is not base macId %u\n", port, HWP_PHY_BASE_MACID(unit, port));
        return RT_ERR_PORT_ID;
    }

    if ((ret = _phy_8295_s0s1Diag_dump(unit, port, port)) != RT_ERR_OK)
    {
        osal_printf("_phy_8295_s0s1_diag_dump Error:0x%x\n", ret);
    }

    return RT_ERR_OK;
}




#if defined(CONFIG_SDK_RTL8295R)
/* Function Name:
 *      phy_8295Rdrv_mapperInit
 * Description:
 *      Initialize PHY 8295R driver.
 * Input:
 *      pPhydrv - pointer of phy driver
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void
phy_8295Rdrv_mapperInit(rt_phydrv_t *pPhydrv)
{
    pPhydrv->phydrv_index = RT_PHYDRV_RTL8295R;
    pPhydrv->fPhydrv_init = phy_8295r_init;
    pPhydrv->fPhydrv_media_get = phy_8295r_media_get;
    pPhydrv->fPhydrv_autoNegoEnable_get = phy_8295r_autoNegoEnable_get;
    pPhydrv->fPhydrv_autoNegoEnable_set = phy_8295r_autoNegoEnable_set;
    pPhydrv->fPhydrv_autoNegoAbility_get = phy_8295r_autoNegoAbility_get;
    pPhydrv->fPhydrv_autoNegoAbility_set = phy_8295r_autoNegoAbility_set;
    pPhydrv->fPhydrv_duplex_get = phy_8295r_duplex_get;
    pPhydrv->fPhydrv_duplex_set = phy_8295r_duplex_set;
    pPhydrv->fPhydrv_speed_get = phy_8295r_speed_get;
    pPhydrv->fPhydrv_speed_set = phy_8295r_speed_set;
    pPhydrv->fPhydrv_enable_set = phy_8295r_enable_set;
    pPhydrv->fPhydrv_fiberNwayForceLinkEnable_get = phy_8295_8214_fiberNwayForceLink_get;
    pPhydrv->fPhydrv_fiberNwayForceLinkEnable_set = phy_8295_8214_fiberNwayForceLink_set;
#if !defined(__BOOTLOADER__)
    pPhydrv->fPhydrv_fiberOAMLoopBackEnable_set = phy_8295r_fiberOAMLoopBackEnable_set;
    pPhydrv->fPhydrv_ptpSwitchMacAddr_get = phy_8295_8214_ptpSwitchMacAddr_get;
    pPhydrv->fPhydrv_ptpSwitchMacAddr_set = phy_8295_8214_ptpSwitchMacAddr_set;
    pPhydrv->fPhydrv_ptpRefTime_get = phy_8295_8214_ptpRefTime_get;
    pPhydrv->fPhydrv_ptpRefTime_set = phy_8295_8214_ptpRefTime_set;
    pPhydrv->fPhydrv_ptpRefTimeAdjust_set = phy_8295_8214_ptpRefTimeAdjust_set;
    pPhydrv->fPhydrv_ptpRefTimeEnable_get = phy_8295_8214_ptpRefTimeEnable_get;
    pPhydrv->fPhydrv_ptpRefTimeEnable_set = phy_8295_8214_ptpRefTimeEnable_set;
    pPhydrv->fPhydrv_ptpEnable_get = phy_8295r_ptpEnable_get;
    pPhydrv->fPhydrv_ptpEnable_set = phy_8295r_ptpEnable_set;
    pPhydrv->fPhydrv_ptpRxTimestamp_get = phy_8295r_ptpRxTimestamp_get;
    pPhydrv->fPhydrv_ptpTxTimestamp_get = phy_8295r_ptpTxTimestamp_get;

    pPhydrv->fPhydrv_ptpRefTimeFreq_get = phy_8295_8214_ptpRefTimeFreq_get;
    pPhydrv->fPhydrv_ptpRefTimeFreq_set = phy_8295_8214_ptpRefTimeFreq_set;
    pPhydrv->fPhydrv_ptpInterruptStatus_get = phy_8295r_ptpInterruptStatus_get;
    pPhydrv->fPhydrv_ptpInterruptEnable_get = phy_8295_8214_ptpInterruptEnable_get;
    pPhydrv->fPhydrv_ptpInterruptEnable_set = phy_8295_8214_ptpInterruptEnable_set;
    pPhydrv->fPhydrv_ptpIgrTpid_get = phy_8295_8214_ptpIgrTpid_get;
    pPhydrv->fPhydrv_ptpIgrTpid_set = phy_8295_8214_ptpIgrTpid_set;
    pPhydrv->fPhydrv_ptpSwitchMacRange_get = phy_8295_8214_ptpSwitchMacRange_get;
    pPhydrv->fPhydrv_ptpSwitchMacRange_set = phy_8295_8214_ptpSwitchMacRange_set;
#endif
    pPhydrv->fPhydrv_reg_get = phy_8295r_reg_get;
    pPhydrv->fPhydrv_reg_set = phy_8295r_reg_set;
    pPhydrv->fPhydrv_reg_park_get = phy_8295r_reg_park_get;
    pPhydrv->fPhydrv_reg_park_set = phy_8295r_reg_park_set;
    pPhydrv->fPhydrv_reg_mmd_get = phy_8295r_reg_mmd_get;
    pPhydrv->fPhydrv_reg_mmd_set = phy_8295r_reg_mmd_set;
    pPhydrv->fPhydrv_loopback_get = phy_8295r_loopback_get;
    pPhydrv->fPhydrv_loopback_set = phy_8295r_loopback_set;
    pPhydrv->fPhydrv_fiberTxDis_set = phy_8295_8214_fiberTxDis_set;
    pPhydrv->fPhydrv_fiberTxDisPin_set = phy_8295_8214_fiberTxDisPin_set;
#if !defined(__BOOTLOADER__)
    pPhydrv->fPhydrv_fiberRx_check = phy_8295r_fiberRx_check;
    pPhydrv->fPhydrv_fiberRx_reset = phy_8295r_fiberRx_reset;
#endif
    pPhydrv->fPhydrv_fiberRxEnable_get = phy_8295_8214_fiberRxEnable_get;
    pPhydrv->fPhydrv_fiberRxEnable_set = phy_8295r_fiberRxEnable_set;
    pPhydrv->fPhydrv_10gMedia_get = phy_8295r_10gMedia_get;
    pPhydrv->fPhydrv_10gMedia_set = phy_8295r_10gMedia_set;
    pPhydrv->fPhydrv_polar_get = phy_8295r_polar_get;
    pPhydrv->fPhydrv_polar_set = phy_8295r_polar_set;
    pPhydrv->fPhydrv_chipRevId_get = phy_8295_8214_chipRevId_get;
    pPhydrv->fPhydrv_portEyeMonitor_start = phy_8295r_portEyeMonitor_start;
    pPhydrv->fPhydrv_portEyeMonitorInfo_get = phy_8295_eyeMonitorInfo_get;
    pPhydrv->fPhydrv_sdsRxCaliStatus_get = phy_8295r_sdsRxCaliStatus_get;
#if !defined(__BOOTLOADER__)
    pPhydrv->fPhydrv_fiberUnidirEnable_set = phy_8295r_fiberUnidirEnable_set;
#endif
    pPhydrv->fPhydrv_sdsEyeParam_get = phy_8295r_sdsEyeParam_get;
    pPhydrv->fPhydrv_sdsEyeParam_set = phy_8295r_sdsEyeParam_set;
    pPhydrv->fPhydrv_sdsLeq_get = phy_8295_sdsLeq_get;
    pPhydrv->fPhydrv_sdsLeq_set = phy_8295_sdsLeq_set;
    pPhydrv->fPhydrv_speedStatus_get = phy_8295r_speedStatus_get;
    pPhydrv->fPhydrv_speedDuplexStatus_get = phy_8295r_speedDuplexStatus_get;
    pPhydrv->fPhydrv_ctrl_get = phy_8295r_ctrl_get;
    pPhydrv->fPhydrv_ctrl_set = phy_8295r_ctrl_set;

}
#endif


#if defined(CONFIG_SDK_RTL8214QF)
/* Function Name:
 *      phy_8214QFdrv_mapperInit
 * Description:
 *      Initialize PHY 8214QF driver.
 * Input:
 *      pPhydrv - pointer of phy driver
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void
phy_8214QFdrv_mapperInit(rt_phydrv_t *pPhydrv)
{
    pPhydrv->phydrv_index = RT_PHYDRV_RTL8214QF;
    pPhydrv->fPhydrv_init = phy_8214qf_init;
    pPhydrv->fPhydrv_media_get = phy_8214qf_media_get;
    pPhydrv->fPhydrv_autoNegoEnable_get = phy_8214qf_autoNegoEnable_get;
    pPhydrv->fPhydrv_autoNegoEnable_set = phy_8214qf_autoNegoEnable_set;
    pPhydrv->fPhydrv_autoNegoAbility_get = phy_8214qf_autoNegoAbility_get;
    pPhydrv->fPhydrv_autoNegoAbility_set = phy_8214qf_autoNegoAbility_set;
    pPhydrv->fPhydrv_duplex_get = phy_8214qf_duplex_get;
    pPhydrv->fPhydrv_duplex_set = phy_8214qf_duplex_set;
    pPhydrv->fPhydrv_speed_get = phy_8214qf_speed_get;
    pPhydrv->fPhydrv_speed_set = phy_8214qf_speed_set;
    pPhydrv->fPhydrv_enable_set = phy_8214qf_enable_set;
    pPhydrv->fPhydrv_fiberNwayForceLinkEnable_get = phy_8295_8214_fiberNwayForceLink_get;
    pPhydrv->fPhydrv_fiberNwayForceLinkEnable_set = phy_8295_8214_fiberNwayForceLink_set;
#if !defined(__BOOTLOADER__)
    pPhydrv->fPhydrv_fiberOAMLoopBackEnable_set = phy_8295_8214_fiberOAMLoopBackEnable_set;
    pPhydrv->fPhydrv_ptpSwitchMacAddr_get = phy_8295_8214_ptpSwitchMacAddr_get;
    pPhydrv->fPhydrv_ptpSwitchMacAddr_set = phy_8295_8214_ptpSwitchMacAddr_set;
    pPhydrv->fPhydrv_ptpRefTime_get = phy_8295_8214_ptpRefTime_get;
    pPhydrv->fPhydrv_ptpRefTime_set = phy_8295_8214_ptpRefTime_set;
    pPhydrv->fPhydrv_ptpRefTimeAdjust_set = phy_8295_8214_ptpRefTimeAdjust_set;
    pPhydrv->fPhydrv_ptpRefTimeEnable_get = phy_8295_8214_ptpRefTimeEnable_get;
    pPhydrv->fPhydrv_ptpRefTimeEnable_set = phy_8295_8214_ptpRefTimeEnable_set;
    pPhydrv->fPhydrv_ptpEnable_get = phy_8214qf_ptpEnable_get;
    pPhydrv->fPhydrv_ptpEnable_set = phy_8214qf_ptpEnable_set;
    pPhydrv->fPhydrv_ptpRxTimestamp_get = phy_8214qf_ptpRxTimestamp_get;
    pPhydrv->fPhydrv_ptpTxTimestamp_get = phy_8214qf_ptpTxTimestamp_get;

    pPhydrv->fPhydrv_ptpRefTimeFreq_get = phy_8295_8214_ptpRefTimeFreq_get;
    pPhydrv->fPhydrv_ptpRefTimeFreq_set = phy_8295_8214_ptpRefTimeFreq_set;
    pPhydrv->fPhydrv_ptpInterruptStatus_get = phy_8214qf_ptpInterruptStatus_get;
    pPhydrv->fPhydrv_ptpInterruptEnable_get = phy_8295_8214_ptpInterruptEnable_get;
    pPhydrv->fPhydrv_ptpInterruptEnable_set = phy_8295_8214_ptpInterruptEnable_set;
    pPhydrv->fPhydrv_ptpIgrTpid_get = phy_8295_8214_ptpIgrTpid_get;
    pPhydrv->fPhydrv_ptpIgrTpid_set = phy_8295_8214_ptpIgrTpid_set;
    pPhydrv->fPhydrv_ptpSwitchMacRange_get = phy_8295_8214_ptpSwitchMacRange_get;
    pPhydrv->fPhydrv_ptpSwitchMacRange_set = phy_8295_8214_ptpSwitchMacRange_set;
 #endif
    pPhydrv->fPhydrv_reg_get = phy_common_reg_get;
    pPhydrv->fPhydrv_reg_set = phy_common_reg_set;
    pPhydrv->fPhydrv_reg_park_get = phy_common_reg_park_get;
    pPhydrv->fPhydrv_reg_park_set = phy_common_reg_park_set;
    pPhydrv->fPhydrv_loopback_get = phy_common_loopback_get;
    pPhydrv->fPhydrv_loopback_set = phy_8214qf_loopback_set;
    pPhydrv->fPhydrv_fiberTxDis_set = phy_8295_8214_fiberTxDis_set;
    pPhydrv->fPhydrv_fiberTxDisPin_set = phy_8295_8214_fiberTxDisPin_set;
#if !defined(__BOOTLOADER__)
    pPhydrv->fPhydrv_fiberRx_check = phy_8214qf_fiberRx_check;
    pPhydrv->fPhydrv_fiberRx_reset = phy_8214qf_fiberRx_reset;
#endif
    pPhydrv->fPhydrv_fiberRxEnable_get = phy_8295_8214_fiberRxEnable_get;
    pPhydrv->fPhydrv_fiberRxEnable_set = phy_8214qf_fiberRxEnable_set;
    pPhydrv->fPhydrv_polar_get = phy_8214qf_polar_get;
    pPhydrv->fPhydrv_polar_set = phy_8214qf_polar_set;
    pPhydrv->fPhydrv_chipRevId_get = phy_8295_8214_chipRevId_get;
    pPhydrv->fPhydrv_portEyeMonitor_start = phy_8295_eyeMonitor_start;
    pPhydrv->fPhydrv_portEyeMonitorInfo_get = phy_8295_eyeMonitorInfo_get;
#if !defined(__BOOTLOADER__)
    pPhydrv->fPhydrv_sds_get = phy_8214qf_sds_get;
    pPhydrv->fPhydrv_sds_set = phy_8214qf_sds_set;
    pPhydrv->fPhydrv_fiberUnidirEnable_set = phy_8214qf_fiberUnidirEnable_set;
#endif
    pPhydrv->fPhydrv_sdsEyeParam_get = phy_8214qf_sdsEyeParam_get;
    pPhydrv->fPhydrv_sdsEyeParam_set = phy_8214qf_sdsEyeParam_set;
    pPhydrv->fPhydrv_sdsLeq_get = phy_8295_sdsLeq_get;
    pPhydrv->fPhydrv_sdsLeq_set = phy_8295_sdsLeq_set;
    pPhydrv->fPhydrv_reset_set = phy_common_reset_set;
    pPhydrv->fPhydrv_linkStatus_get = phy_common_linkStatus_get;
    pPhydrv->fPhydrv_peerAutoNegoAbility_get = phy_common_fiberPeerAutoNegoAbility_get;
    pPhydrv->fPhydrv_macIntfSerdesMode_get = phy_8214qf_macIntfSerdesMode_get;
    pPhydrv->fPhydrv_macIntfSerdesLinkStatus_get = phy_8214qf_macIntfSerdesLinkStatus_get;
    pPhydrv->fPhydrv_speedStatus_get = phy_common_speedStatusResReg_get;
    pPhydrv->fPhydrv_speedDuplexStatus_get = phy_8214qf_speedDuplexStatus_get;

}
#endif


#if defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8214QFdrv_mapperInit
 * Description:
 *      Initialize PHY 8214QF driver.
 * Input:
 *      pPhydrv - pointer of phy driver
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void
phy_8224QFdrv_mapperInit(rt_phydrv_t *pPhydrv)
{
    pPhydrv->phydrv_index = RT_PHYDRV_RTL8224QF;
    pPhydrv->fPhydrv_init = phy_8224qf_init;
    pPhydrv->fPhydrv_media_get = phy_8214qf_media_get;
    pPhydrv->fPhydrv_autoNegoEnable_get = phy_8224qf_autoNegoEnable_get;
    pPhydrv->fPhydrv_autoNegoEnable_set = phy_8224qf_autoNegoEnable_set;
    pPhydrv->fPhydrv_autoNegoAbility_get = phy_8214qf_autoNegoAbility_get;
    pPhydrv->fPhydrv_autoNegoAbility_set = phy_8214qf_autoNegoAbility_set;
    pPhydrv->fPhydrv_duplex_get = phy_8224qf_duplex_get;
    pPhydrv->fPhydrv_duplex_set = phy_8224qf_duplex_set;
    pPhydrv->fPhydrv_speed_get = phy_8224qf_speed_get;
    pPhydrv->fPhydrv_speed_set = phy_8224qf_speed_set;
    pPhydrv->fPhydrv_enable_set = phy_8224qf_enable_set;
    pPhydrv->fPhydrv_fiberNwayForceLinkEnable_get = phy_8295_8214_fiberNwayForceLink_get;
    pPhydrv->fPhydrv_fiberNwayForceLinkEnable_set = phy_8295_8214_fiberNwayForceLink_set;
#if !defined(__BOOTLOADER__)
    pPhydrv->fPhydrv_fiberOAMLoopBackEnable_set = phy_8295_8214_fiberOAMLoopBackEnable_set;
    pPhydrv->fPhydrv_ptpSwitchMacAddr_get = phy_8295_8214_ptpSwitchMacAddr_get;
    pPhydrv->fPhydrv_ptpSwitchMacAddr_set = phy_8295_8214_ptpSwitchMacAddr_set;
    pPhydrv->fPhydrv_ptpRefTime_get = phy_8295_8214_ptpRefTime_get;
    pPhydrv->fPhydrv_ptpRefTime_set = phy_8295_8214_ptpRefTime_set;
    pPhydrv->fPhydrv_ptpRefTimeAdjust_set = phy_8295_8214_ptpRefTimeAdjust_set;
    pPhydrv->fPhydrv_ptpRefTimeEnable_get = phy_8295_8214_ptpRefTimeEnable_get;
    pPhydrv->fPhydrv_ptpRefTimeEnable_set = phy_8295_8214_ptpRefTimeEnable_set;
    pPhydrv->fPhydrv_ptpEnable_get = phy_8214qf_ptpEnable_get;
    pPhydrv->fPhydrv_ptpEnable_set = phy_8214qf_ptpEnable_set;
    pPhydrv->fPhydrv_ptpRxTimestamp_get = phy_8214qf_ptpRxTimestamp_get;
    pPhydrv->fPhydrv_ptpTxTimestamp_get = phy_8214qf_ptpTxTimestamp_get;
    pPhydrv->fPhydrv_ptpRefTimeFreq_get = phy_8295_8214_ptpRefTimeFreq_get;
    pPhydrv->fPhydrv_ptpRefTimeFreq_set = phy_8295_8214_ptpRefTimeFreq_set;
    pPhydrv->fPhydrv_ptpInterruptStatus_get = phy_8214qf_ptpInterruptStatus_get;
    pPhydrv->fPhydrv_ptpInterruptEnable_get = phy_8295_8214_ptpInterruptEnable_get;
    pPhydrv->fPhydrv_ptpInterruptEnable_set = phy_8295_8214_ptpInterruptEnable_set;
    pPhydrv->fPhydrv_ptpIgrTpid_get = phy_8295_8214_ptpIgrTpid_get;
    pPhydrv->fPhydrv_ptpIgrTpid_set = phy_8295_8214_ptpIgrTpid_set;
    pPhydrv->fPhydrv_ptpSwitchMacRange_get = phy_8295_8214_ptpSwitchMacRange_get;
    pPhydrv->fPhydrv_ptpSwitchMacRange_set = phy_8295_8214_ptpSwitchMacRange_set;
#endif
    pPhydrv->fPhydrv_reg_get = phy_common_reg_get;
    pPhydrv->fPhydrv_reg_set = phy_common_reg_set;
    pPhydrv->fPhydrv_reg_park_get = phy_common_reg_park_get;
    pPhydrv->fPhydrv_reg_park_set = phy_common_reg_park_set;
    pPhydrv->fPhydrv_loopback_get = phy_common_loopback_get;
    pPhydrv->fPhydrv_loopback_set = phy_8224qf_loopback_set;
    pPhydrv->fPhydrv_fiberTxDis_set = phy_8295_8214_fiberTxDis_set;
    pPhydrv->fPhydrv_fiberTxDisPin_set = phy_8295_8214_fiberTxDisPin_set;
#if !defined(__BOOTLOADER__)
    pPhydrv->fPhydrv_fiberRx_check = phy_8214qf_fiberRx_check;
    pPhydrv->fPhydrv_fiberRx_reset = phy_8214qf_fiberRx_reset;
#endif
    pPhydrv->fPhydrv_fiberRxEnable_get = phy_8295_8214_fiberRxEnable_get;
    pPhydrv->fPhydrv_fiberRxEnable_set = phy_8214qf_fiberRxEnable_set;
    pPhydrv->fPhydrv_10gMedia_get = phy_8224qf_10gMedia_get;
    pPhydrv->fPhydrv_10gMedia_set = phy_8224qf_10gMedia_set;
    pPhydrv->fPhydrv_polar_get = phy_8214qf_polar_get;
    pPhydrv->fPhydrv_polar_set = phy_8214qf_polar_set;
    pPhydrv->fPhydrv_chipRevId_get = phy_8295_8214_chipRevId_get;
    pPhydrv->fPhydrv_portEyeMonitor_start = phy_8295_eyeMonitor_start;
    pPhydrv->fPhydrv_portEyeMonitorInfo_get = phy_8295_eyeMonitorInfo_get;
#if !defined(__BOOTLOADER__)
    pPhydrv->fPhydrv_sds_get = phy_8224qf_sds_get;
    pPhydrv->fPhydrv_sds_set = phy_8224qf_sds_set;
    pPhydrv->fPhydrv_fiberUnidirEnable_set = phy_8214qf_fiberUnidirEnable_set;
#endif
    pPhydrv->fPhydrv_sdsEyeParam_get = phy_8214qf_sdsEyeParam_get;
    pPhydrv->fPhydrv_sdsEyeParam_set = phy_8214qf_sdsEyeParam_set;
    pPhydrv->fPhydrv_sdsTestMode_set = phy_8295_8214_sdsTestMode_set;
    pPhydrv->fPhydrv_sdsTestModeCnt_get = phy_8295_8214_sdsTestModeCnt_get;
    pPhydrv->fPhydrv_sdsLeq_get = phy_8295_sdsLeq_get;
    pPhydrv->fPhydrv_sdsLeq_set = phy_8295_sdsLeq_set;
    pPhydrv->fPhydrv_reset_set = phy_common_reset_set;
    pPhydrv->fPhydrv_linkStatus_get = phy_common_linkStatus_get;
    pPhydrv->fPhydrv_peerAutoNegoAbility_get = phy_common_fiberPeerAutoNegoAbility_get;
    pPhydrv->fPhydrv_macIntfSerdesMode_get = phy_8214qf_macIntfSerdesMode_get;
    pPhydrv->fPhydrv_macIntfSerdesLinkStatus_get = phy_8224qf_macIntfSerdesLinkStatus_get;
    pPhydrv->fPhydrv_speedStatus_get = phy_common_speedStatusResReg_get;
    pPhydrv->fPhydrv_speedDuplexStatus_get = phy_common_speedDuplexStatusResReg_get;
}
#endif


