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
 * $Revision: 97202 $
 * $Date: 2019-05-27 19:13:10 +0800 (Mon, 27 May 2019) $
 *
 * Purpose : mac driver service APIs in the SDK.
 *
 * Feature : mac driver service APIs
 *
 */

#ifndef __HAL_MAC_DRV_RTL9310_H__
#define __HAL_MAC_DRV_RTL9310_H__

/*
 * Include Files
 */
#define TBL_L2_SRAM_FIND_NEXT   1000
#define TBL_L2_BCAM_FIND_NEXT   1001
/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
extern rt_macdrv_t rtl9310_macdrv;

/*
 * Macro Declaration
 */

#define RTL9310_REGADDR_FIELD_W(_u, _r, _end, _start, _d) \
    drv_rtl9310_regAddrField_write(_u, _r, _end,  _start, _d)

/*
 * Function Declaration
 */


/* Function Name:
 *      drv_rtl9310_regAddrField_write
 * Description:
 *      Write register address field configuration
 * Input:
 *      unit - unit id
 *      reg  - register address
 *      endBit    - end bit of configure field
 *      startBit  - start bit of configure field
 *      data - configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
extern int32
drv_rtl9310_regAddrField_write(uint32 unit, uint32 reg, uint32 endBit,
    uint32 startBit, uint32 data);

/* Function Name:
 *      drv_rtl9310_sds2AnaSds_get
 * Description:
 *      Get analog SerDes from SerDes ID
 * Input:
 *      unit - unit id
 *      sds  - SerDes id
 * Output:
 *      anaSds - analog SerDes id
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
extern int32
drv_rtl9310_sds2AnaSds_get(uint32 unit, uint32 sds, uint32 *anaSds);

/* Function Name:
 *      drv_rtl9310_sds2XsgmSds_get
 * Description:
 *      Get XSGMII SerDes from SerDes ID
 * Input:
 *      unit - unit id
 *      sds  - SerDes id
 * Output:
 *      xsgmSds - XSGMII SerDes id
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
extern int32
drv_rtl9310_sds2XsgmSds_get(uint32 unit, uint32 sds, uint32 *xsgmSds);

/* Function Name:
 *      drv_rtl9310_sdsCmuPage_get
 * Description:
 *      Get SerDes CMU page
 * Input:
 *      unit - unit id
 *      sds  - SerDes id
 * Output:
 *      page - CMU page
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
extern int32
drv_rtl9310_sdsCmuPage_get(uint32 unit, uint32 sds, uint32 *page);

/* Function Name:
 *      drv_rtl9310_portMacForceLink_set
 * Description:
 *      configure mac force link status
 * Input:
 *      unit    - unit id
 *      port   - port ID
 *      forceEn   - force enable
 *      linkSts    - link status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
drv_rtl9310_portMacForceLink_set(uint32 unit, rtk_port_t port, rtk_enable_t en,
    rtk_port_linkStatus_t sts);

/* Function Name:
 *      drv_rtl9310_sds_rx_rst
 * Description:
 *      Reset Serdes Rx and original patch are kept.
 * Input:
 *      unit    - unit id
 *      sds     - user serdes id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_OUT_OF_RANGE - Serdes index is not support.
 * Note:
 *      None
 */
extern int32
drv_rtl9310_sds_rx_rst(uint32  unit, uint32 sds);

/* Function Name:
 *      drv_rtl9310_sds_read
 * Description:
 *      Read SerDes configuration
 * Input:
 *      unit - unit id
 *      sds  - SerDes id
 *      page - page number
 *      reg  - register
 * Output:
 *      data - SerDes Configuration
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
extern int32
drv_rtl9310_sds_read(uint32 unit, uint32 sds, uint32 page, uint32 reg, uint32 *data);

/* Function Name:
 *      drv_rtl9310_sds_write
 * Description:
 *      Write SerDes configuration
 * Input:
 *      unit - unit id
 *      sds  - SerDes id
 *      page - page number
 *      reg  - register
 *      data - SerDes Configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
extern int32
drv_rtl9310_sds_write(uint32 unit, uint32 sds, uint32 page, uint32 reg, uint32 data);

/* Function Name:
 *      rtl9310_port_probe
 * Description:
 *      Probe the select port interface settings of the chip.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
extern int32
rtl9310_port_probe(uint32 unit);


/* Function Name:
 *      rtl9310_init
 * Description:
 *      Initialize the specified settings of the chip.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
extern int32
rtl9310_init(uint32 unit);


/* Function Name:
 *      rtl9310_miim_read
 * Description:
 *      Get PHY registers from rtl9310 family chips.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      page    - PHY page
 *      phy_reg - PHY register
 * Output:
 *      pData   - pointer buffer of read data
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_PAGE_ID  - invalid PHY page id
 *      RT_ERR_PHY_REG_ID   - invalid PHY reg id
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      1. port valid range is 0 ~ 27
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
extern int32
rtl9310_miim_read(
    uint32      unit,
    rtk_port_t  port,
    uint32      page,
    uint32      phy_reg,
    uint32      *pData);



/* Function Name:
 *      rtl9310_miim_write
 * Description:
 *      Set PHY registers in rtl9310 family chips.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      page    - PHY page
 *      phy_reg - PHY register
 *      data    - Read data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_PAGE_ID  - invalid PHY page id
 *      RT_ERR_PHY_REG_ID   - invalid PHY reg id
 * Note:
 *      1. port valid range is 0 ~ 27
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
extern int32
rtl9310_miim_write(
    uint32      unit,
    rtk_port_t  port,
    uint32      page,
    uint32      phy_reg,
    uint32      data);

/* Function Name:
 *      rtl9310_miim_park_read
 * Description:
 *      Get PHY registers from rtl9310 family chips.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      page    - PHY page
 *      parkPage    - PHY park page
 *      phy_reg - PHY register
 * Output:
 *      pData   - pointer buffer of read data
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_PAGE_ID  - invalid PHY page id
 *      RT_ERR_PHY_REG_ID   - invalid PHY reg id
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      1. port valid range is 0 ~ 27
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
extern int32
rtl9310_miim_park_read(
    uint32      unit,
    rtk_port_t  port,
    uint32      page,
    uint32      parkPage,
    uint32      phy_reg,
    uint32      *pData);

/* Function Name:
 *      rtl9310_miim_park_write
 * Description:
 *      Set PHY registers in rtl9310 family chips.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      page    - PHY page
 *      parkPage    - PHY park page
 *      phy_reg - PHY register
 *      data    - Read data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_PAGE_ID  - invalid PHY page id
 *      RT_ERR_PHY_REG_ID   - invalid PHY reg id
 * Note:
 *      1. port valid range is 0 ~ 27
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
extern int32
rtl9310_miim_park_write(
    uint32      unit,
    rtk_port_t  port,
    uint32      page,
    uint32      parkPage,
    uint32      phy_reg,
    uint32      data);

/* Function Name:
 *      rtl9310_miim_portmask_write
 * Description:
 *      Set PHY registers in those portmask of rtl9310 family chips.
 * Input:
 *      unit     - unit id
 *      portmask - portmask
 *      page     - PHY page
 *      phy_reg  - PHY register
 *      data     - Read data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_PAGE_ID  - invalid PHY page id
 *      RT_ERR_PHY_REG_ID   - invalid PHY reg id
 * Note:
 *      1. portmask valid range is bit 0 ~ 27
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
extern int32
rtl9310_miim_portmask_write(
    uint32          unit,
    rtk_portmask_t  portmask,
    uint32          page,
    uint32          phy_reg,
    uint32          data);

/* Function Name:
 *      rtl9310_miim_portmask_write
 * Description:
 *      Set PHY registers in those portmask of rtl9310 family chips.
 * Input:
 *      unit     - unit id
 *      portmask - portmask
 *      page     - PHY page
 *      phy_reg  - PHY register
 *      data     - Read data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_PAGE_ID  - invalid PHY page id
 *      RT_ERR_PHY_REG_ID   - invalid PHY reg id
 * Note:
 *      1. portmask valid range is bit 0 ~ 27
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
extern int32
rtl9310_miim_portmask_write(
    uint32          unit,
    rtk_portmask_t  portmask,
    uint32          page,
    uint32          phy_reg,
    uint32          data);

/* Function Name:
 *      rtl9310_miim_broadcast_write
 * Description:
 *      Set PHY registers in rtl9310 family chips with broadcast mechanism.
 * Input:
 *      unit    - unit id
 *      page    - page id
 *      phy_reg - PHY register
 *      data    - Read data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PHY_PAGE_ID  - invalid PHY page id
 *      RT_ERR_PHY_REG_ID   - invalid PHY reg id
 * Note:
 *      1. page valid range is 0 ~ 31
 *      2. phy_reg valid range is 0 ~ 31
 */
extern int32
rtl9310_miim_broadcast_write(
    uint32      unit,
    uint32      page,
    uint32      phy_reg,
    uint32      data);


/* Function Name:
 *      rtl9310_miim_mmd_read
 * Description:
 *      Get PHY registers from rtl9310 family chips.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      mmdAddr - mmd device address
 *      mmdReg  - mmd reg id
 * Output:
 *      pData   - pointer buffer of read data
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_PAGE_ID  - invalid PHY page id
 *      RT_ERR_PHY_REG_ID   - invalid PHY reg id
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      1. port valid range is 0 ~ 27
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
extern int32
rtl9310_miim_mmd_read(
    uint32      unit,
    rtk_port_t  port,
    uint32      mmdAddr,
    uint32      mmdReg,
    uint32      *pData);


/* Function Name:
 *      rtl9310_miim_mmd_write
 * Description:
 *      Set PHY registers in rtl9310 family chips.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      mmdAddr - mmd device address
 *      mmdReg  - mmd reg id
 *      data    - Read data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_PAGE_ID  - invalid PHY page id
 *      RT_ERR_PHY_REG_ID   - invalid PHY reg id
 * Note:
 *      1. port valid range is 0 ~ 27
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
extern int32
rtl9310_miim_mmd_write(
    uint32      unit,
    rtk_port_t  port,
    uint32      mmdAddr,
    uint32      mmdReg,
    uint32      data);

/* Function Name:
 *      rtl9310_miim_mmd_portmask_write
 * Description:
 *      Set PHY registers in those portmask of rtl9310 family chips.
 * Input:
 *      unit     - unit id
 *      portmask - portmask
 *      mmdAddr  - mmd device address
 *      mmdReg   - mmd reg id
 *      data     - Read data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_PAGE_ID  - invalid PHY page id
 *      RT_ERR_PHY_REG_ID   - invalid PHY reg id
 * Note:
 *      1. portmask valid range is bit 0 ~ 27
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
extern int32
rtl9310_miim_mmd_portmask_write(
    uint32          unit,
    rtk_portmask_t  portmask,
    uint32          mmdAddr,
    uint32          mmdReg,
    uint32          data);

/* Function Name:
 *      rtl9310_table_read
 * Description:
 *      Read one specified table entry by table index.
 * Input:
 *      unit  - unit id
 *      table - table index
 *      addr  - entry address of the table
 * Output:
 *      pData - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 *      None
 */
extern int32
rtl9310_table_read(
    uint32  unit,
    uint32  table,
    uint32  addr,
    uint32  *pData);


/* Function Name:
 *      rtl9310_table_write
 * Description:
 *      Write one specified table entry by table index.
 * Input:
 *      unit  - unit id
 *      table - table index
 *      addr  - entry address of the table
 *      pData - pointer buffer of table entry data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 *      None
 */
extern int32
rtl9310_table_write(
    uint32  unit,
    uint32  table,
    uint32  addr,
    uint32  *pData);

/* Function Name:
 *      rtl9310_miim_pollingEnable_get
 * Description:
 *      Get the mac polling PHY status of the specified port.
 * Input:
 *      unit     - unit id
 *      port     - port id
 * Output:
 *      pEnabled - pointer buffer of mac polling PHY status
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
extern int32
rtl9310_miim_pollingEnable_get(
    uint32          unit,
    rtk_port_t      port,
    rtk_enable_t    *pEnabled);

/* Function Name:
 *      rtl9310_miim_pollingEnable_set
 * Description:
 *      Set the mac polling PHY status of the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      enabled - mac polling PHY status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
extern int32
rtl9310_miim_pollingEnable_set(
    uint32          unit,
    rtk_port_t      port,
    rtk_enable_t    enabled);

/* Function Name:
 *      rtl9310_smiAddr_init
 * Description:
 *      Initialize SMI Address (PHY address).
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *
 */
extern int32 rtl9310_smiAddr_init(uint32 unit);

#endif  /* __HAL_MAC_DRV_RTL9310_H__ */

