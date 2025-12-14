/*
 * Copyright (C) 2012 Realtek Semiconductor Corp. 
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated, 
 * modified or distributed under the authorized license from Realtek. 
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER 
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED. 
 *
 * $Revision: 61228 $
 * $Date: 2016-03-16 14:49:14 +0800 (Thu, 16 Mar 2016) $
 *
 * Purpose : Definition of TIME API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) MDIO
 *
 */
#ifndef __DAL_RTL9607F_MDIO_H__
#define __DAL_RTL9607F_MDIO_H__

/* Function Name:
 *      dal_rtl9607f_mdio_init
 * Description:
 *      Init MDIO configuration
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Init MDIO configuration
 */
extern int32
dal_rtl9607f_mdio_init(void);

/* Function Name:
 *      dal_rtl9607f_mdio_cfg_set
 * Description:
 *      Set MDIO configuration
 * Input:
 *      set   - Select which set of MDIO
 *      port  - Select shich virtual port to access MDIO
 *      phyid - IEEE 802.3
 *      fmt   - MDIO format to access
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must set MDIO configuration before MDIO read/write
 */
extern int32
dal_rtl9607f_mdio_cfg_set(uint8 set,uint8 port,uint8 phyid,rtk_mdio_format_t fmt);

/* Function Name:
 *      dal_rtl9607f_mdio_cfg_get
 * Description:
 *      Get MDIO configuration
 * Input:
 *      None
 * Output:
 *      set   - Select which set of MDIO
 *      port  - Select shich virtual port to access MDIO
 *      phyid - clause 22 of IEEE 802.3 define
 *      fmt   - MDIO format to access
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must check MDIO configuration before MDIO read/write
 */
extern int32
dal_rtl9607f_mdio_cfg_get(uint8 *set,uint8 *port,uint8 *phyid,rtk_mdio_format_t *fmt);

/* Function Name:
 *      dal_rtl9607f_mdio_c22_write
 * Description:
 *      Write MDIO by clause 22
 * Input:
 *      reg   - clause 22 of IEEE 802.3 define
 *      data  - clause 22 of IEEE 802.3 define
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      
 */
extern int32
dal_rtl9607f_mdio_c22_write(uint8 c22_reg,uint16 data);

/* Function Name:
 *      dal_rtl9607f_mdio_c22_read
 * Description:
 *      Read MDIO by clause 22
 * Input:
 *      reg   - clause 22 of IEEE 802.3 define
 * Output:
 *      data  - clause 22 of IEEE 802.3 define
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      
 */
extern int32
dal_rtl9607f_mdio_c22_read(uint8 c22_reg,uint16 *data);

/* Function Name:
 *      dal_rtl9607f_mdio_c45_write
 * Description:
 *      Write MDIO by clause 45 
 * Input:
 *      dev   - clause 45 of IEEE 802.3 define
 *      reg   - clause 45 of IEEE 802.3 define
 *      data  - clause 45 of IEEE 802.3 define
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      
 */
extern int32
dal_rtl9607f_mdio_c45_write(uint8 dev,uint16 c45_reg,uint16 data);

/* Function Name:
 *      dal_rtl9607f_mdio_c45_write
 * Description:
 *      Read MDIO by clause 45 
 * Input:
 *      dev   - clause 45 of IEEE 802.3 define
 *      reg   - clause 45 of IEEE 802.3 define
 * Output:
 *      data  - clause 45 of IEEE 802.3 define
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      
 */
extern int32
dal_rtl9607f_mdio_c45_read(uint8 dev,uint16 c45_reg,uint16 *data);

#if defined(CONFIG_COMMON_RT_API)
/* Function Name:
 *      dal_rtl9607f_mdio_read
 * Description:
 *      Read from MDIO bus
 * Input:
 *      info  - the necessary info for MDIO access
 * Output:
 *      data  - read data from MDIO bus
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      The APIs dal_rtl9607f_mdio_read/dal_rtl9607f_mdio_write replace the collective APIs
 *          dal_rtl9607f_mdio_cfg_set/dal_rtl9607f_mdio_cfg_get
 *          dal_rtl9607f_mdio_c22_write/dal_rtl9607f_mdio_c22_read 
 *          dal_rtl9607f_mdio_c45_write/dal_rtl9607f_mdio_c45_read
 */
extern int32
dal_rtl9607f_mdio_read(rt_mdio_info_t *info, uint16 *data);

/* Function Name:
 *      dal_rtl9607f_mdio_write
 * Description:
 *      Read from MDIO bus
 * Input:
 *      info  - the necessary info for MDIO access
 *      data  - write data to MDIO bus
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      The APIs dal_rtl9607f_mdio_read/dal_rtl9607f_mdio_write replace the collective APIs
 *          dal_rtl9607f_mdio_cfg_set/dal_rtl9607f_mdio_cfg_get
 *          dal_rtl9607f_mdio_c22_write/dal_rtl9607f_mdio_c22_read 
 *          dal_rtl9607f_mdio_c45_write/dal_rtl9607f_mdio_c45_read
 */
extern int32
dal_rtl9607f_mdio_write(rt_mdio_info_t *info, uint16 data);
#endif
 
#endif /* __DAL_RTL9607F_MDIO_H__ */

