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
 * Purpose : PHY 8218D Driver APIs.
 *
 * Feature : PHY 8218D Driver APIs
 *
 */

#ifndef __HAL_PHY_PHY_RTL8218D_H__
#define __HAL_PHY_PHY_RTL8218D_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/port.h>


/*
 * Symbol Definition
 */
#define PORT_NUM_IN_8218D    (8)

#define PHY_8218D_REV_ID_OFFSET                 0
#define PHY_8218D_REV_ID_MASK                   (0x7F << PHY_8218D_REV_ID_OFFSET)
#define PHY_8218D_CHIP_ID_MAP_OFFSET            7
#define PHY_8218D_CHIP_ID_MAP_MASK              (0x7 << PHY_8218D_CHIP_ID_MAP_OFFSET)


/*
 * Data Declaration
 */
extern rt_phyInfo_t phy_8218D_info;

/*
 * Macro Declaration
 */


/*
 * Function Declaration
 */

/* Function Name:
 *      phy_8218Ddrv_ge_mapperInit
 * Description:
 *      Initialize PHY 8218D driver.
 * Input:
 *      pPhydrv - pointer of phy driver
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern void
phy_8218Ddrv_ge_mapperInit(rt_phydrv_t *pPhydrv);


/* Function Name:
 *      phy_8218d_efuseSw_load
 * Description:
 *      software load eFUSE patch to PHY
 * Input:
 *      unit      - unit id
 *      basePort  - base MAC ID of the 8218D
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
phy_8218d_efuseSw_load(uint32 unit, rtk_port_t basePort);

/* Function Name:
 *      phy_8218d_bConf_init
 * Description:
 *      Chip config init
 * Input:
 *      unit      - unit id
 *      basePort  - base MAC ID of the 8218D
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern void
phy_8218d_bConf_init(uint32 unit, rtk_port_t basePort);

/* Function Name:
 *      phy_8218d_efuseSwLoadEnable_get
 * Description:
 *      Read from eFUSE
 * Input:
 *      unit      - unit id
 *      basePort  - base MAC ID of the 8218D
 * Output:
 *      enable    - ENABLED: swfotware load should be performed. DISABLED: no need for swfotware load
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
phy_8218d_efuseSwLoadEnable_get(uint32 unit, rtk_port_t basePort, rtk_enable_t *enable);

/* Function Name:
 *      phy_8218d_ieeeTestMode_set
 * Description:
 *      Set test mode for Giga PHY transmitter test
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      pTestMode->mode  - test mode 1 ~ 4 which is defined in IEEE 40.6.1.1.2
 *      pTestMode->channel  - Channel A, B, C, D, or none
 *      pTestMode->flags -
 *          RTK_PORT_PHYTESTMODE_FLAG_ALL_PHY_PORTS -
 *              apply the test on all ports of the PHY.
 *              To use this feature, the "port" parameter shall set to the first port of the PHY.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_NOT_ALLOWED - The operation is not allowed
 *      RT_ERR_PORT_NOT_SUPPORTED - test mode is not supported
 * Note:
 *      RTK_PORT_PHYTESTMODE_FLAG_ALL_PHY_PORTS only support in mode 4
 */
extern int32
phy_8218d_ieeeTestMode_set(uint32 unit, rtk_port_t port, rtk_port_phyTestMode_t *pTestMode);

/* Function Name:
 *      phy_8218d_enable_set
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
phy_8218d_enable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      phy_8218d_macIntfSdsRxCali_start
 * Description:
 *      Perform rx calibration on PHY's MAC interface SerDes
 * Input:
 *      unit   - unit id
 *      port   - PHY's base mac id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID
 * Note:
 *      None
 */
extern int32
phy_8218d_macIntfSdsRxCali_start(uint32 unit, rtk_port_t port);


/* Function Name:
 *      phy_8218d_macIntfSerdesMode_get
 * Description:
 *      Get PHY's MAC interface SerDes mode
 * Input:
 *      unit   - unit id
 *      port   - PHY's base mac id
 * Output:
 *      pMode   - serdes mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID
 * Note:
 *      None
 */
extern int32
phy_8218d_macIntfSerdesMode_get(uint32 unit, rtk_port_t port, rt_serdesMode_t *pMode);


/* Function Name:
 *      phy_8218d_sdsEyeParam_get
 * Description:
 *      Get SerDes eye parameters
 * Input:
 *      unit    - unit ID
 *      port    - Base mac ID of the PHY
 *      sdsId   - SerDes ID of the PHY
 * Output:
 *      pEyeParam - eye parameter.
 *                  impedance is not supported.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      For XSGMII mode, sdsId is 0; for QSGMII mode sdsId is 0 or 1.
 */
extern int32
phy_8218d_sdsEyeParam_get(uint32 unit, rtk_port_t port, uint32 sdsId, rtk_sds_eyeParam_t *pEyeParam);

/* Function Name:
 *      phy_8218d_sdsEyeParam_set
 * Description:
 *      Set SerDes eye parameters
 * Input:
 *      unit    - unit ID
 *      port    - Base mac ID of the PHY
 *      sdsId   - SerDes ID of the PHY
 *      pEyeParam - eye parameter.
 *                  impedance is not supported.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      For XSGMII mode, sdsId is 0; for QSGMII mode sdsId is 0 or 1.
 */
extern int32
phy_8218d_sdsEyeParam_set(uint32 unit, rtk_port_t port, uint32 sdsId, rtk_sds_eyeParam_t *pEyeParam);

/* Function Name:
 *      phy_8218d_macIntfSerdes_reset
 * Description:
 *      Reset PHY's MAC interface SerDes
 * Input:
 *      unit   - unit id
 *      basePort   - base mac id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID
 * Note:
 *      None
 */
extern int32
phy_8218d_macIntfSerdes_reset(uint32 unit, rtk_port_t port);

#ifdef CONFIG_SDK_RTL8218D
/* Function Name:
 *      phy_8218d_serdes_linkdown_chk
 * Description:
 *      check phy serdes linkdown status
 * Input:
 *      unit   - unit id
 *      port   - base mac id
 *      uint32 * status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID
 * Note:
 *      None
 */
extern int32
phy_8218d_serdes_linkdown_chk(uint32 unit, rtk_port_t port, uint32 * status);
#endif


/* Function Name:
 *      _phy_8218d_diag_dump
 * Description:
 *      dump 8218d status register
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
_phy_8218d_diag_dump(uint32 unit, uint32 port);

#endif /* __HAL_PHY_PHY_RTL8218D_H__ */
