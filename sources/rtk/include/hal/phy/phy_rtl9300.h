/*
 * Copyright (C) 2016-2022 Realtek Semiconductor Corp.
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
 * Purpose : MAC 9300 internal PHY Driver APIs.
 *
 * Feature : MAC 9300 internal PHY Driver APIs
 *
 */

#ifndef __HAL_PHY_PHY_RTL9300_H__
#define __HAL_PHY_PHY_RTL9300_H__

/*
 * Include Files
 */
#include <common/rt_type.h>


#define PORT_NUM_IN_9300SDS                     1

typedef struct sds_reg_field_s
{
    uint32 reg;
    uint32 field;
}sds_reg_field_t;

typedef struct sds_mode_s
{
    rtk_serdesMode_t    sdsMode;
}sds_mode_t;

extern rt_phyInfo_t phy_9300sds_info;

/* Function Name:
 *      phy_9300_linkChange_process
 * Description:
 *      For 10G SerDes link change process
 * Input:
 *      unit          - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
extern int32
phy_9300_linkChange_process(uint32 unit, rtk_port_t port, rtk_port_linkStatus_t linkSts);

/* Function Name:
 *      phy_9300_10gmedia_get
 * Description:
 *      Get 9300 10 serdes PHY media type.
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
 *      None
 */
extern int32
phy_9300_10gmedia_get(uint32 unit, rtk_port_t port, rtk_port_10gMedia_t *pMedia);

/* Function Name:
 *      phy_9300_10gMedia_set
 * Description:
 *
 * Input:
 *      unit - unit id
 *      port - port id
 *      media
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_INPUT
 *      RT_ERR_CHIP_NOT_SUPPORTED
 *      RT_ERR_TIMEOUT
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
phy_9300_10gMedia_set(uint32 unit, rtk_port_t port, rtk_port_10gMedia_t media);

/* Function Name:
 *      phy_9300_speed_get
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
phy_9300_speed_get(uint32 unit, rtk_port_t port, rtk_port_speed_t *pSpeed);

/* Function Name:
 *      phy_9300_serdes_mapperInit
 * Description:
 *      Initialize PHY 9300 serdes driver.
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
phy_9300_serdes_mapperInit(rt_phydrv_t *pPhydrv);

#endif /* __HAL_PHY_PHY_RTL9300_H__ */




