/*
 * Copyright (C) 2021 Realtek Semiconductor Corp. 
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
 *           (1) MDIO funciton initization and configuration
 *           (2) Cluase 22/45 MDIO read/write APIs
 *
 */
#ifndef __RT_MDIO_H__
#define __RT_MDIO_H__

/*
 * Symbol Definition
 */
typedef enum rt_mdio_format_e
{
    RT_MDIO_FMT_C22 = 0,    /* MDIO uses IEEE 802.3 clause 22 format */
    RT_MDIO_FMT_C45,        /* MDIO uses IEEE 802.3 clause 45 format */
    RT_MDIO_FMT_END
} rt_mdio_format_t;

typedef struct rt_mdio_info_s
{
    rt_mdio_format_t   format;   /* RT_MDIO_FMT_C22 or RT_MDIO_FMT_C45 */
    uint8 set;      /* Physical MDIO set of ASIC */
    uint8 phyAddr;  /* PHY ID in Clause 22, PHY address in Clause 45 */
    uint8 dev;      /* Clause 45 only */
    uint16 reg;     /* Register number */
} rt_mdio_info_t;

/*
 * Function Declaration
 */
/* Function Name:
 *      rt_mdio_init
 * Description:
 *      Init MDIO configuration
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - OK
 *      RT_ERR_FAILED   - failed
 * Note:
 *      Init MDIO configuration
 */
extern int32
rt_mdio_init(void);


/* Function Name:
 *      rt_mdio_cfg_set
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
 *      RT_ERR_OK       - OK
 *      RT_ERR_FAILED   - failed
 * Note:
 *      Must set MDIO configuration before MDIO read/write
 *      The collective of APIs 
 *          rt_mdio_cfg_set/rt_mdio_cfg_get
 *          rt_mdio_c22_write/rt_mdio_c22_read 
 *          rt_mdio_c45_write/rt_mdio_c45_read
 *      are not recommended anymore. User are encurage to use
 *          rt_mdio_read/rt_mdio_write
 *      instead
 */
extern int32
rt_mdio_cfg_set(uint8 set,uint8 port,uint8 phyid,rt_mdio_format_t fmt);

/* Function Name:
 *      rt_mdio_cfg_get
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
 *      RT_ERR_OK       - OK
 *      RT_ERR_FAILED   - failed
 * Note:
 *      Must check MDIO configuration before MDIO read/write
 *      The collective of APIs 
 *          rt_mdio_cfg_set/rt_mdio_cfg_get
 *          rt_mdio_c22_write/rt_mdio_c22_read 
 *          rt_mdio_c45_write/rt_mdio_c45_read
 *      are not recommended anymore. User are encurage to use
 *          rt_mdio_read/rt_mdio_write
 *      instead
 */
extern int32
rt_mdio_cfg_get(uint8 *set,uint8 *port,uint8 *phyid,rt_mdio_format_t *fmt);

/* Function Name:
 *      rt_mdio_c22_write
 * Description:
 *      Write MDIO by clause 22
 * Input:
 *      reg   - clause 22 of IEEE 802.3 define
 *      data  - clause 22 of IEEE 802.3 define
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - OK
 *      RT_ERR_FAILED   - failed
 * Note:
 *      The collective of APIs 
 *          rt_mdio_cfg_set/rt_mdio_cfg_get
 *          rt_mdio_c22_write/rt_mdio_c22_read 
 *          rt_mdio_c45_write/rt_mdio_c45_read
 *      are not recommended anymore. User are encurage to use
 *          rt_mdio_read/rt_mdio_write
 *      instead
 */
extern int32
rt_mdio_c22_write(uint8 c22_reg,uint16 data);

/* Function Name:
 *      rt_mdio_c22_read
 * Description:
 *      Read MDIO by clause 22
 * Input:
 *      reg   - clause 22 of IEEE 802.3 define
 * Output:
 *      data  - clause 22 of IEEE 802.3 define
 * Return:
 *      RT_ERR_OK       - OK
 *      RT_ERR_FAILED   - failed
 * Note:
 *      The collective of APIs 
 *          rt_mdio_cfg_set/rt_mdio_cfg_get
 *          rt_mdio_c22_write/rt_mdio_c22_read 
 *          rt_mdio_c45_write/rt_mdio_c45_read
 *      are not recommended anymore. User are encurage to use
 *          rt_mdio_read/rt_mdio_write
 *      instead
 */
extern int32
rt_mdio_c22_read(uint8 c22_reg,uint16 *data);

/* Function Name:
 *      rt_mdio_c45_write
 * Description:
 *      Write MDIO by clause 45 
 * Input:
 *      dev   - clause 45 of IEEE 802.3 define
 *      reg   - clause 45 of IEEE 802.3 define
 *      data  - clause 45 of IEEE 802.3 define
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - OK
 *      RT_ERR_FAILED   - failed
 * Note:
 *      The collective of APIs 
 *          rt_mdio_cfg_set/rt_mdio_cfg_get
 *          rt_mdio_c22_write/rt_mdio_c22_read 
 *          rt_mdio_c45_write/rt_mdio_c45_read
 *      are not recommended anymore. User are encurage to use
 *          rt_mdio_read/rt_mdio_write
 *      instead
 */
extern int32
rt_mdio_c45_write(uint8 dev,uint16 c45_reg,uint16 data);

/* Function Name:
 *      rt_mdio_c45_read
 * Description:
 *      Read MDIO by clause 45 
 * Input:
 *      dev   - clause 45 of IEEE 802.3 define
 *      reg   - clause 45 of IEEE 802.3 define
 * Output:
 *      data  - clause 45 of IEEE 802.3 define
 * Return:
 *      RT_ERR_OK       - OK
 *      RT_ERR_FAILED   - failed
 * Note:
 *      The collective of APIs 
 *          rt_mdio_cfg_set/rt_mdio_cfg_get
 *          rt_mdio_c22_write/rt_mdio_c22_read 
 *          rt_mdio_c45_write/rt_mdio_c45_read
 *      are not recommended anymore. User are encurage to use
 *          rt_mdio_read/rt_mdio_write
 *      instead
 */
extern int32
rt_mdio_c45_read(uint8 dev,uint16 c45_reg,uint16 *data);

/* Function Name:
 *      rt_mdio_read
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
 *      The APIs rt_mdio_read/rt_mdio_write replace the collective APIs
 *          rt_mdio_cfg_set/rt_mdio_cfg_get
 *          rt_mdio_c22_write/rt_mdio_c22_read 
 *          rt_mdio_c45_write/rt_mdio_c45_read
 */
extern int32
rt_mdio_read(rt_mdio_info_t *info, uint16 *data);

/* Function Name:
 *      rt_mdio_write
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
 *      The APIs rt_mdio_read/rt_mdio_write replace the collective APIs
 *          rt_mdio_cfg_set/rt_mdio_cfg_get
 *          rt_mdio_c22_write/rt_mdio_c22_read 
 *          rt_mdio_c45_write/rt_mdio_c45_read
 */
int32
rt_mdio_write(rt_mdio_info_t *info, uint16 data);

#endif /* __RT_MDIO_H__ */

