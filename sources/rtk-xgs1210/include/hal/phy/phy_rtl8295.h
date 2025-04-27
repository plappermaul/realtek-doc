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
 * $Revision: 90617 $
 * $Date: 2018-07-23 11:42:05 +0800 (Mon, 23 Jul 2018) $
 *
 * Purpose : PHY 8295R/8214QF Driver APIs.
 *
 * Feature : PHY 8295R/8214QF Driver APIs
 *
 */
#ifndef __PHY_RTL8295_H__
#define __PHY_RTL8295_H__

#define PORT_NUM_IN_8295R                           1
#define PORT_NUM_IN_8214QF                          4

#define PHY_8295_SDSID_S0_SLV                       2
#define PHY_8295_SDSID_S1_SLV                       3
#define PHY_8295_SDSID_BCAST                        8

#define PHY_8295_PAGE_BASE_OFFSET_S0                        256
#define PHY_8295_PAGE_BASE_OFFSET_S1                        768
#define PHY_8295_PAGE_BASE_OFFSET_S0_SLV                    512
#define PHY_8295_PAGE_BASE_OFFSET_S1_SLV                    2304
#define PHY_8295_PAGE_BASE_OFFSET_S4                        1024
#define PHY_8295_PAGE_BASE_OFFSET_S5                        1280
#define PHY_8295_PAGE_BASE_OFFSET_S6                        1536
#define PHY_8295_PAGE_BASE_OFFSET_S7                        1792
#define PHY_8295_PAGE_BASE_OFFSET_BCAST                     2048
#define PHY_8295_SDS_PAGE_NUM_MAX                           256 /* maximum page number per serdes */


#define PHY_8295_LINK_RETRY_CNT                     3

#define RTL8295R_RESL_STS_LINK(status)      ((status & 0x1 << 2)?PORT_LINKUP:PORT_LINKDOWN)
#define RTL8295R_RESL_STS_DUPLEX(status)    ((status & 0x1 << 3)?PORT_FULL_DUPLEX:PORT_HALF_DUPLEX)
#define RTL8295R_RESL_STS_TXPAUSE(status)   ((status & 0x1 << 6)?1:0)
#define RTL8295R_RESL_STS_RXPAUSE(status)   ((status & 0x1 << 7)?1:0)
#define RTL8295R_RESL_STS_EEE(status)       ((status & 0x1 << 8)?1:0)
#define RTL8295R_RESL_STS_SPEED(status)     phy_8295r_resolutionResult2Speed(status)

#if defined(CONFIG_SDK_RTL8295R)
extern rt_phyInfo_t phy_8295R_info;
#endif

#if defined(CONFIG_SDK_RTL8214QF)
extern rt_phyInfo_t phy_8214QF_info;
#endif

extern int32
phy_8295r_10gMedia_get(uint32 unit, rtk_port_t port, rtk_port_10gMedia_t *pMedia);

extern int32
phy_8295r_10gMedia_set(uint32 unit, rtk_port_t port, rtk_port_10gMedia_t media);

extern int32
phy_8295r_autoNegoEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

extern void
phy_8295Rdrv_mapperInit(rt_phydrv_t *pPhydrv);

extern void
phy_8214QFdrv_mapperInit(rt_phydrv_t *pPhydrv);

extern int32
phy_8295_mdxMacId_get(uint32 unit, rtk_port_t port, uint32 *pMdxMacId);
extern int32
phy_8295_reg_read(uint32 unit, rtk_port_t port, uint32 reg_addr, uint32 *pData);
extern int32
phy_8295_reg_write(uint32 unit, rtk_port_t port, uint32 reg_addr, uint32 data);
extern int32
phy_8295_sds_read(uint32 unit, rtk_port_t port, uint32 sds_idx, uint32 page, uint32 reg, uint32 *pData);
extern int32
phy_8295_sds_write(uint32 unit, rtk_port_t port, uint32 sds_idx, uint32 page, uint32 reg, uint32 data);

extern int32
phy_8295_8214_chipRevId_get(uint32 unit, rtk_port_t port, uint16 *chip_rev_id);

extern int32
phy_8295r_swPollEnable_get(uint32 unit, uint32 port);

extern int32
phy_8295r_resolutionReg_get(uint32 unit, uint32 port, uint32 *pResl);

extern int32
phy_8295r_swMacPollPhyStatus_get(uint32 unit, uint32 port, rtk_port_swMacPollPhyStatus_t *pphyStatus);

extern rtk_port_speed_t
phy_8295r_resolutionResult2Speed(uint32 resl);

extern int32
phy_8295r_linkChange_process(uint32 unit, rtk_port_t port, rtk_port_linkStatus_t linkSts);

extern int32
phy_8295r_portSdsRxCaliEnable_set(uint32 unit, uint32 port, uint32 sdsId, rtk_enable_t enable);

extern int32
phy_8295_diag_set(uint32 unit, rtk_port_t port, rtk_port_t mdxMacId, uint32 sds, uint8 *name);

#if defined(CONFIG_SDK_RTL8295R)
extern int32
phy_8295r_macIntfSerdes_reset(uint32 unit, rtk_port_t port);
#endif

#if defined(CONFIG_SDK_RTL8214QF)
extern int32
phy_8214qf_macIntfSerdes_reset(uint32 unit, rtk_port_t basePort);
#endif

#if defined(CONFIG_SDK_RTL8214QF)
extern int32
phy_8214qf_sdsSgmiiStatus_get(uint32 unit, rtk_port_t port, rtk_port_linkStatus_t *pLink, uint32 *pSpeed, uint32 *pDuplex);
#endif

extern int32
phy_8295_phyReg_read(uint32 unit, rtk_port_t mdxMacId, uint32 phy_page, uint32 phy_reg, uint32 *pData);

extern int32
phy_8295_phyReg_write(uint32 unit, rtk_port_t mdxMacId, uint32 phy_page, uint32 phy_reg, uint32 data);

extern int32
phy_8295_sdsReg_read(uint32 unit, uint32 mdxMacId, uint32 sdsId, uint32 s0_phy_page, uint32 phy_reg, uint32 *pData);

extern int32
phy_8295_sdsReg_write(uint32 unit, uint32 mdxMacId, uint32 sdsId, uint32 s0_phy_page, uint32 phy_reg, uint32 data);

#endif /* __PHY_RTL8295_H__ */


