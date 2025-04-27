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
 * Purpose : PHY 8295R/8214QF/8224QF Driver APIs.
 *
 * Feature : PHY 8295R/8214QF/8224QF Driver APIs
 *
 */
#ifndef __PHY_RTL8295_H__
#define __PHY_RTL8295_H__

#define PORT_NUM_IN_8295R                           1
#define PORT_NUM_IN_8214QF                          4
#define PORT_NUM_IN_8224QF                          4

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

#if defined(CONFIG_SDK_RTL8224QF)
extern rt_phyInfo_t phy_8224QF_info;
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
extern void
phy_8224QFdrv_mapperInit(rt_phydrv_t *pPhydrv);

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

#if defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
extern int32
phy_8214qf_macIntfSerdes_reset(uint32 unit, rtk_port_t basePort);
#endif

#if defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
extern int32
phy_8214qf_sdsSgmiiStatus_get(uint32 unit, rtk_port_t port, rtk_port_linkStatus_t *pLink, uint32 *pSpeed, uint32 *pDuplex);
#endif

#if defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
extern int32
phy_8214qf_macIntfSdsRxCali_start(uint32 unit, rtk_port_t port, uint32 sdsId);
#endif

#if defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
extern int32
phy_8214qf_sdsEyeParam_get(uint32 unit, rtk_port_t port, uint32 sdsId, rtk_sds_eyeParam_t *pEyeParam);
#endif

#if defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
extern int32
phy_8214qf_sdsEyeParam_set(uint32 unit, rtk_port_t port, uint32 sdsId, rtk_sds_eyeParam_t *pEyeParam);
#endif

extern int32
phy_8295_phyReg_read(uint32 unit, rtk_port_t mdxMacId, uint32 phy_page, uint32 phy_reg, uint32 *pData);

extern int32
phy_8295_phyReg_write(uint32 unit, rtk_port_t mdxMacId, uint32 phy_page, uint32 phy_reg, uint32 data);

extern int32
phy_8295_sdsReg_read(uint32 unit, uint32 mdxMacId, uint32 sdsId, uint32 s0_phy_page, uint32 phy_reg, uint32 *pData);

extern int32
phy_8295_sdsReg_write(uint32 unit, uint32 mdxMacId, uint32 sdsId, uint32 s0_phy_page, uint32 phy_reg, uint32 data);

#if defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
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
extern int32
_phy_8214qf_diag_dump(uint32 unit, uint32 port);
#endif

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
extern int32
phy_8295r_speedStatus_get(uint32 unit, rtk_port_t port, rtk_port_speed_t *pSpeed);

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
extern int32
phy_8295r_speedDuplexStatus_get(uint32 unit, rtk_port_t port, rtk_port_speed_t *pSpeed, rtk_port_duplex_t *pDuplex);
#endif /* CONFIG_SDK_RTL8295R */



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
extern int32
phy_8214qf_enable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

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
extern int32
phy_8224qf_enable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

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
extern int32
phy_8224qf_speed_get(uint32 unit, rtk_port_t port, rtk_port_speed_t *pSpeed);

#endif /* __PHY_RTL8295_H__ */


