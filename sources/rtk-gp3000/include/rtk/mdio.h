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
 * Purpose : Definition of MDIO API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) MDIO
 *
 */
#ifndef __RTK_MDIO_H__
#define __RTK_MDIO_H__

typedef enum rtk_mdio_format_e
{
    MDIO_FMT_C22 = 0,
    MDIO_FMT_C45,
    MDIO_FMT_END
} rtk_mdio_format_t;

/* Function Name:
 *      rtk_mdio_init
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
rtk_mdio_init(void);


/* Function Name:
 *      rtk_mdio_cfg_set
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
rtk_mdio_cfg_set(uint8 set,uint8 port,uint8 phyid,rtk_mdio_format_t fmt);

/* Function Name:
 *      rtk_mdio_cfg_get
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
rtk_mdio_cfg_get(uint8 *set,uint8 *port,uint8 *phyid,rtk_mdio_format_t *fmt);

/* Function Name:
 *      rtk_mdio_c22_write
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
rtk_mdio_c22_write(uint8 c22_reg,uint16 data);

/* Function Name:
 *      rtk_mdio_c22_read
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
rtk_mdio_c22_read(uint8 c22_reg,uint16 *data);

/* Function Name:
 *      rtk_mdio_c45_write
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
rtk_mdio_c45_write(uint8 dev,uint16 c45_reg,uint16 data);

/* Function Name:
 *      rtk_mdio_c45_write
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
rtk_mdio_c45_read(uint8 dev,uint16 c45_reg,uint16 *data);
 
#endif /* __RTK_MDIO_H__ */
