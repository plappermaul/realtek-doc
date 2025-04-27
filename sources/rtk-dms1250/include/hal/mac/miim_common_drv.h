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
 * Purpose : MIIM service APIs in the SDK.
 *
 * Feature : MIIM service APIs
 *
 */

#ifndef __HAL_COMMON_MIIM_COMMON_DRV_H__
#define __HAL_COMMON_MIIM_COMMON_DRV_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/port.h>
#include <rtk/diag.h>

/*
 * Function Declaration
 */


/* Function Name:
 *      hal_miim_read
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
 *      1. port valid range is 0 ~ 27
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
extern int32
hal_miim_read(
    uint32      unit,
    rtk_port_t  port,
    uint32      page,
    uint32      phy_reg,
    uint32      *pData);

/* Function Name:
 *      hal_miim_write
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
 *      1. port valid range is 0 ~ 27
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
extern int32
hal_miim_write(
    uint32      unit,
    rtk_port_t  port,
    uint32      page,
    uint32      phy_reg,
    uint32      data);

/* Function Name:
 *      hal_miim_park_read
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
 *      1. port valid range is 0 ~ 27
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
extern int32
hal_miim_park_read(
    uint32      unit,
    rtk_port_t  port,
    uint32      page,
    uint32      parkPage,
    uint32      phy_reg,
    uint32      *pData);

/* Function Name:
 *      hal_miim_park_write
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
 *      1. port valid range is 0 ~ 27
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
extern int32
hal_miim_park_write(
    uint32      unit,
    rtk_port_t  port,
    uint32      page,
    uint32      parkPage,
    uint32      phy_reg,
    uint32      data);


/* Function Name:
 *      hal_miim_parkBits_read
 * Description:
 *      Get value from the specified bits of the PHY registers.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      page    - PHY page
 *      parkPage    - PHY park page
 *      phy_reg - PHY register
 *      msb     - most significant bit for reading the value
 *      lsb     - least significant bit for reading the value
 * Output:
  *      pVal   - pointer buffer of read [msb:lsb] value of the PHY register
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      1. port valid range is 0 ~ 27
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
extern int32
hal_miim_parkBits_read(
    uint32      unit,
    rtk_port_t  port,
    uint32      page,
    uint32      parkPage,
    uint32      phy_reg,
    uint32      msb,
    uint32      lsb,
    uint32      *pVal);

/* Function Name:
 *      hal_miim_parkBits_write
 * Description:
 *      Set value to the specified bits of the PHY registers.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      page    - PHY page
 *      parkPage    - PHY park page
 *      phy_reg - PHY register
 *      msb     - most significant bit for writing the value
 *      lsb     - least significant bit for writing the value
 *      val    - value for write to [msb:lsb] of the PHY register
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      1. port valid range is 0 ~ 27
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
extern int32
hal_miim_parkBits_write(
    uint32      unit,
    rtk_port_t  port,
    uint32      page,
    uint32      parkPage,
    uint32      phy_reg,
    uint32      msb,
    uint32      lsb,
    uint32      val);

/* Function Name:
 *      hal_miim_portmask_write
 * Description:
 *      Set PHY registers in those portmask.
 * Input:
 *      unit     - unit id
 *      portmask - portmask
 *      page     - PHY page
 *      phy_reg  - PHY register
 *      data     - write data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      1. portmask valid range is bit 0 ~ 27
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
extern int32
hal_miim_portmask_write(
    uint32          unit,
    rtk_portmask_t  portmask,
    uint32          page,
    uint32          phy_reg,
    uint32          data);

/* Function Name:
 *      hal_miim_broadcast_write
 * Description:
 *      Set PHY registers with broadcast mechanism.
 * Input:
 *      unit    - unit id
 *      page    - PHY page
 *      phy_reg - PHY register
 *      data    - write data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      1. page valid range is 0 ~ 31
 *      2. phy_reg valid range is 0 ~ 31
 */
extern int32
hal_miim_broadcast_write(
    uint32      unit,
    uint32      page,
    uint32      phy_reg,
    uint32      data);

/* Function Name:
 *      hal_miim_extParkPage_read
 * Description:
 *      Get PHY registers.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      mainPage    - main page id
 *      extPage     - extension page id
 *      parkPage    - parking page id
 *      phy_reg - PHY register
 * Output:
 *      pData   - pointer buffer of read data
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      1. port valid range is 0 ~ 27
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
extern int32
hal_miim_extParkPage_read(
    uint32      unit,
    rtk_port_t  port,
    uint32      mainPage,
    uint32      extPage,
    uint32      parkPage,
    uint32      phy_reg,
    uint32      *pData);

/* Function Name:
 *      hal_miim_extParkPage_write
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
 *      1. port valid range is 0 ~ 27
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
extern int32
hal_miim_extParkPage_write(
    uint32      unit,
    rtk_port_t  port,
    uint32      mainPage,
    uint32      extPage,
    uint32      parkPage,
    uint32      phy_reg,
    uint32      data);

/* Function Name:
 *      hal_miim_extParkPage_portmask_write
 * Description:
 *      Set PHY registers in those portmask.
 * Input:
 *      unit     - unit id
 *      portmask - portmask
 *      mainPage    - main page id
 *      extPage     - extension page id
 *      parkPage    - parking page id
 *      phy_reg  - PHY register
 *      data     - write data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      1. portmask valid range is bit 0 ~ 27
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
extern int32
hal_miim_extParkPage_portmask_write(
    uint32          unit,
    rtk_portmask_t  portmask,
    uint32          mainPage,
    uint32          extPage,
    uint32          parkPage,
    uint32          phy_reg,
    uint32          data);

/* Function Name:
 *      hal_miim_mmd_read
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
 *      1. port valid range is 0 ~ 27
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
extern int32
hal_miim_mmd_read(
    uint32      unit,
    rtk_port_t  port,
    uint32      mmdAddr,
    uint32      mmdReg,
    uint32      *pData);

/* Function Name:
 *      hal_miim_mmd_write
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
 *      1. port valid range is 0 ~ 27
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
extern int32
hal_miim_mmd_write(
    uint32      unit,
    rtk_port_t  port,
    uint32      mmdAddr,
    uint32      mmdReg,
    uint32      data);

/* Function Name:
 *      hal_miim_mmd_portmask_write
 * Description:
 *      Set PHY registers in those portmask.
 * Input:
 *      unit     - unit id
 *      portmask - portmask
 *      mmdAddr  - mmd device address
 *      mmdReg   - mmd reg id
 *      data     - write data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      1. portmask valid range is bit 0 ~ 27
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
extern int32
hal_miim_mmd_portmask_write(
    uint32          unit,
    rtk_portmask_t  portmask,
    uint32          mmdAddr,
    uint32          mmdReg,
    uint32          data);

/* Function Name:
 *      hal_miim_pollingEnable_get
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
hal_miim_pollingEnable_get(
    uint32          unit,
    rtk_port_t      port,
    rtk_enable_t    *pEnabled);

/* Function Name:
 *      hal_miim_pollingEnable_set
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
hal_miim_pollingEnable_set(
    uint32          unit,
    rtk_port_t      port,
    rtk_enable_t    enabled);

/* Function Name:
 *      hal_miim_pollingPauseAllPort_set
 * Description:
 *      Set the mac polling PHY status to disable
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pRestore_portmask - backup the polling enabled ports for Resume API to restore the polling state
 * Return:
 *      RT_ERR_OK     - OK
 * Note:
 *      None
 */
extern int32
hal_miim_pollingPauseAllPort_set(
    uint32          unit,
    rtk_portmask_t  *pRestore_portmask);

/* Function Name:
 *      hal_miim_pollingResumeAllPort_set
 * Description:
 *      Set the mac polling PHY status to enable according to the pRestore_portmask
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      pRestore_portmask - Restore the polling state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 * Note:
 *      None
 */
extern int32
hal_miim_pollingResumeAllPort_set(
    uint32          unit,
    rtk_portmask_t  *pRestore_portmask);

/* Function Name:
 *      hal_miim_pollingWa_lock
 * Description:
 *      Get mac polling PHY enable status of the specified port and disable mac polling PHY.
 *      This API is used for workaround PHY page being changed by polling mechanism problem.
 * Input:
 *      unit     - unit id
 *      port     - port id
 * Output:
 *      pRestore_state - pointer buffer of mac polling PHY status
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
extern int32
hal_miim_pollingWa_lock(uint32          unit,
                        rtk_port_t      port,
                        rtk_enable_t    *pRestore_state);

/* Function Name:
 *      hal_miim_pollingWa_unlock
 * Description:
 *      Restore mac polling PHY status of the specified port.
 *      This API is used for workaround PHY page being changed by polling mechanism problem.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      restore_state - Restoring mac polling PHY status which is obtained from hal_miim_pollingWa_lock()
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
extern int32
hal_miim_pollingWa_unlock(uint32          unit,
                          rtk_port_t      port,
                          rtk_enable_t    restore_state);


/* Function Name:
 *      hal_miim_portSmiMdxProto_get
 * Description:
 *      Configure MDC/MDIO protocol for an SMI interface
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      *pProto - protocol as Clause 22 or Clause 45
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 *      RT_ERR_INPUT  - error smi id or proto
 * Note:
 *      None
 */
extern int32
hal_miim_portSmiMdxProto_get(uint32 unit, rtk_port_t port, drv_smi_mdxProtoSel_t *pProto);

/* Function Name:
 *      hal_miim_portSmiMdxProto_set
 * Description:
 *      Configure MDC/MDIO protocol for an SMI interface
 * Input:
 *      unit - unit id
 *      port - port id
 *      proto  - protocol as Clause 22 or Clause 45
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 *      RT_ERR_INPUT  - error smi id or proto
 * Note:
 *      None
 */
extern int32
hal_miim_portSmiMdxProto_set(uint32 unit, rtk_port_t port, drv_smi_mdxProtoSel_t proto);

#endif  /* __HAL_COMMON_MIIM_COMMON_DRV_H__ */

