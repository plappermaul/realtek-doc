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
 * Purpose : mac driver service APIs in the SDK.
 *
 * Feature : mac driver service APIs
 *
 */

#ifndef __HAL_MAC_DRV_H__
#define __HAL_MAC_DRV_H__

#include <hal/chipdef/apollo/apollo_table_struct.h>
/*
 * Macro Declaration
 */

/*
 * Function Declaration
 */

/* Function Name:
 *      table_find
 * Description:
 *      Find this kind of table structure in this specified chip.
 * Input:
 *      table - table index
 * Output:
 *      None
 * Return:
 *      NULL      - Not found
 *      Otherwise - Pointer of table structure that found
 * Note:
 *      None
 */
extern rtk_table_t *
table_find (uint32 table);


/* Function Name:
 *      apollo_serdes_patch
 * Description:
 *      Patch the Serdes configuration.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void apollo_serdes_patch(void);

/* Function Name:
 *      apollo_init
 * Description:
 *      Initialize the specified settings of the chip.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
extern int32
apollo_init(void);

/* Function Name:
 *      apollo_table_read
 * Description:
 *      Read one specified table entry by table index.
 * Input:
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
 *      1. The addr argument of RTL8389 PIE table is not continuous bits from
 *         LSB bits, we do one compiler option patch for this.
 *      2. If you don't use the RTL8389 chip, please turn off the "RTL8389"
 *         definition symbol, then performance will be improved.
 */
extern int32
apollo_table_read(
    uint32  table,
    uint32  addr,
    uint32  *pData);

/* Function Name:
 *      apollo_table_write
 * Description:
 *      Write one specified table entry by table index.
 * Input:
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
 *      1. The addr argument of RTL8389 PIE table is not continuous bits from
 *         LSB bits, we do one compiler option patch for this.
 *      2. If you don't use the RTL8389 chip, please turn off the "RTL8389"
 *         definition symbol, then performance will be improved.
 */
extern int32
apollo_table_write(
    uint32  table,
    uint32  addr,
    uint32  *pData);

extern int32
apollo_miim_read(
    rtk_port_t  port,
    uint32      page,
    uint32      phy_reg,
    uint32      *pData);

extern int32
apollo_miim_write(
    rtk_port_t  port,
    uint32      page,
    uint32      phy_reg,
    uint32      data);

/* Function Name:
 *      apollo_table_clear
 * Description:
 *      Write one specified table entry by table index.
 * Input:
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
 */
extern int32
apollo_table_clear(
    uint32  table,
    uint32  startIdx,
    uint32  endIdx);

int32
apollo_interPhy_write(
    uint32      phyID,
    uint32      page,
    uint32      phyRegAddr,
    uint16      data);

int32
apollo_interPhy_read(
    uint32      phyID,
    uint32      page,
    uint32      phyRegAddr,
    uint16      *pData);


/*********************************************************/
/*                  APOLLO MP                            */
/*********************************************************/

/* Function Name:
 *      apollomp_init
 * Description:
 *      Initialize the specified settings of the chip.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
extern int32
apollomp_init(void);

/* Function Name:
 *      apollomp_table_write
 * Description:
 *      Write one specified table entry by table index.
 * Input:
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
 *      1. The addr argument of RTL8389 PIE table is not continuous bits from
 *         LSB bits, we do one compiler option patch for this.
 *      2. If you don't use the RTL8389 chip, please turn off the "RTL8389"
 *         definition symbol, then performance will be improved.
 */
extern int32
apollomp_table_write(
    uint32  table,
    uint32  addr,
    uint32  *pData);

/* Function Name:
 *      apollomp_table_read
 * Description:
 *      Read one specified table entry by table index.
 * Input:
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
 */
extern int32
apollomp_table_read(
    uint32  table,
    uint32  addr,
    uint32  *pData);


/* Function Name:
 *      apollomp_table_clear
 * Description:
 *      clear specified table entry by table index range.
 * Input:
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
 */
extern int32
apollomp_table_clear(
    uint32  table,
    uint32  startIdx,
    uint32  endIdx);

/* Function Name:
 *      apollomp_miim_read
 * Description:
 *      Get PHY registers from apollo family chips.
 * Input:
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
 *      1. port valid range is 0 ~ 5
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
extern int32
apollomp_miim_read(
    rtk_port_t  port,
    uint32      page,
    uint32      phyReg,
    uint32      *pData);

/* Function Name:
 *      apollomp_miim_write
 * Description:
 *      Set PHY registers in apollo family chips.
 * Input:
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
 *      1. port valid range is 0 ~ 5
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
extern int32
apollomp_miim_write (
    rtk_port_t  port,
    uint32      page,
    uint32      phyReg,
    uint32      data);

extern int32
apollomp_interPhy_read(
    uint32      phyID,
    uint32      page,
    uint32      phyRegAddr,
    uint16      *pData);

extern int32
apollomp_interPhy_write(
    uint32      phyID,
    uint32      page,
    uint32      phyRegAddr,
    uint16      data);

int32
apollomp_extPhy_read(
    uint32      phyID,
    uint32      phyRegAddr,
    uint16      *pData);

extern int32
apollomp_extPhy_write(
    uint32      phyID,
    uint32      phyRegAddr,
    uint16      data);


/*********************************************************/
/*                  RTL9601B                            */
/*********************************************************/

/* Function Name:
 *      rtl9601b_init
 * Description:
 *      Initialize the specified settings of the chip.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
extern int32
rtl9601b_init(void);

/* Function Name:
 *     rtl9601b_table_write
 * Description:
 *      Write one specified table entry by table index.
 * Input:
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
 *      1. The addr argument of RTL8389 PIE table is not continuous bits from
 *         LSB bits, we do one compiler option patch for this.
 *      2. If you don't use the RTL8389 chip, please turn off the "RTL8389"
 *         definition symbol, then performance will be improved.
 */
extern int32
rtl9601b_table_write(
    uint32  table,
    uint32  addr,
    uint32  *pData);

/* Function Name:
 *      rtl9601b_table_read
 * Description:
 *      Read one specified table entry by table index.
 * Input:
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
 */
extern int32
rtl9601b_table_read(
    uint32  table,
    uint32  addr,
    uint32  *pData);


/* Function Name:
 *      rtl9601b_table_clear
 * Description:
 *      clear specified table entry by table index range.
 * Input:
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
 */
extern int32
rtl9601b_table_clear(
    uint32  table,
    uint32  startIdx,
    uint32  endIdx);

/* Function Name:
 *      rtl9601b_miim_read
 * Description:
 *      Get PHY registers from apollo family chips.
 * Input:
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
 *      1. port valid range is 0 ~ 5
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
extern int32
rtl9601b_miim_read(
    rtk_port_t  port,
    uint32      page,
    uint32      phyReg,
    uint32      *pData);

/* Function Name:
 *      rtl9601b_miim_write
 * Description:
 *      Set PHY registers in apollo family chips.
 * Input:
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
 *      1. port valid range is 0 ~ 5
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
extern int32
rtl9601b_miim_write (
    rtk_port_t  port,
    uint32      page,
    uint32      phyReg,
    uint32      data);

extern int32
rtl9601b_interPhy_read(
    uint32      phyID,
    uint32      page,
    uint32      phyRegAddr,
    uint16      *pData);

extern int32
rtl9601b_interPhy_write(
    uint32      phyID,
    uint32      page,
    uint32      phyRegAddr,
    uint16      data);

extern int32
rtl9601b_ocpInterPhy_read(
    uint32      phyID,
    uint32      ocpAddr,
    uint16      *pData);


extern int32
rtl9601b_ocpInterPhy_write(
    uint32      phyID,
    uint32      ocpAddr,
    uint16      data);


/*********************************************************/
/*                  RTL9602C                            */
/*********************************************************/
extern int32
rtl9602c_init(void);

extern int32
rtl9602c_table_write(
    uint32  table,
    uint32  addr,
    uint32  *pData);

extern int32
rtl9602c_table_read(
    uint32  table,
    uint32  addr,
    uint32  *pData);

extern int32
rtl9602c_table_clear(
    uint32  table,
    uint32  startIdx,
    uint32  endIdx);

extern int32
rtl9602c_miim_read(
    rtk_port_t  port,
    uint32      page,
    uint32      phyReg,
    uint32      *pData);

extern int32
rtl9602c_miim_write (
    rtk_port_t  port,
    uint32      page,
    uint32      phyReg,
    uint32      data);

extern int32
rtl9602c_ocpInterPhy_read(
    uint32      phyID,
    uint32      ocpAddr,
    uint16      *pData);

extern int32
rtl9602c_ocpInterPhy_write(
    uint32      phyID,
    uint32      ocpAddr,
    uint16      data);


/*********************************************************/
/*                  RTL9607C                            */
/*********************************************************/
extern int32
rtl9607c_init(void);

extern int32
rtl9607c_table_write(
    uint32  table,
    uint32  addr,
    uint32  *pData);

extern int32
rtl9607c_table_read(
    uint32  table,
    uint32  addr,
    uint32  *pData);

extern int32
rtl9607c_table_clear(
    uint32  table,
    uint32  startIdx,
    uint32  endIdx);

extern int32
rtl9607c_miim_read(
    rtk_port_t  port,
    uint32      page,
    uint32      phyReg,
    uint32      *pData);

extern int32
rtl9607c_miim_write (
    rtk_port_t  port,
    uint32      page,
    uint32      phyReg,
    uint32      data);

extern int32
rtl9607c_ocpInterPhy_read(
    uint32      phyID,
    uint32      ocpAddr,
    uint16      *pData);

extern int32
rtl9607c_ocpInterPhy_write(
    uint32      phyID,
    uint32      ocpAddr,
    uint16      data);


/*********************************************************/
/*                  RTL9603CVD                            */
/*********************************************************/
extern int32
rtl9603cvd_init(void);

extern int32
rtl9603cvd_table_write(
    uint32  table,
    uint32  addr,
    uint32  *pData);

extern int32
rtl9603cvd_table_read(
    uint32  table,
    uint32  addr,
    uint32  *pData);

extern int32
rtl9603cvd_table_clear(
    uint32  table,
    uint32  startIdx,
    uint32  endIdx);

extern int32
rtl9603cvd_miim_read(
    rtk_port_t  port,
    uint32      page,
    uint32      phyReg,
    uint32      *pData);

extern int32
rtl9603cvd_miim_write (
    rtk_port_t  port,
    uint32      page,
    uint32      phyReg,
    uint32      data);

extern int32
rtl9603cvd_ocpInterPhy_read(
    uint32      phyID,
    uint32      ocpAddr,
    uint16      *pData);

extern int32
rtl9603cvd_ocpInterPhy_write(
    uint32      phyID,
    uint32      ocpAddr,
    uint16      data);
#endif  /* __HAL_MAC_DRV_H__ */
