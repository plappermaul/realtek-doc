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

#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <osal/lib.h>
#include <rtk/rtusr/include/rtusr_util.h>
#include <rtdrv/rtdrv_netfilter.h>
#include <common/rt_type.h>

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
int32
rtk_mdio_init(void)
{
    rtdrv_mdioCfg_t mdio_cfg;
    memset(&mdio_cfg, 0x0, sizeof(rtdrv_mdioCfg_t));

    /* function body */
    SETSOCKOPT(RTDRV_MDIO_INIT, &mdio_cfg, rtdrv_mdioCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_mdio_init */

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
int32
rtk_mdio_cfg_set(uint8 set,uint8 port,uint8 phyid,rtk_mdio_format_t fmt)
{
    rtdrv_mdioCfg_t mdio_cfg;
    memset(&mdio_cfg, 0x0, sizeof(rtdrv_mdioCfg_t));

    /* function body */
    osal_memcpy(&mdio_cfg.set, &set, sizeof(uint8));
    osal_memcpy(&mdio_cfg.port, &port, sizeof(uint8));
    osal_memcpy(&mdio_cfg.phyid, &phyid, sizeof(uint8));
    osal_memcpy(&mdio_cfg.fmt, &fmt, sizeof(rtk_mdio_format_t));
    SETSOCKOPT(RTDRV_MDIO_CFG_SET, &mdio_cfg, rtdrv_mdioCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_mdio_cfg_set */

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
int32
rtk_mdio_cfg_get(uint8 *set,uint8 *port,uint8 *phyid,rtk_mdio_format_t *fmt)
{
    rtdrv_mdioCfg_t mdio_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == set), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == port), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == phyid), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == fmt), RT_ERR_NULL_POINTER);

    /* function body */
    GETSOCKOPT(RTDRV_MDIO_CFG_GET, &mdio_cfg, rtdrv_mdioCfg_t, 1);
    osal_memcpy(set, &mdio_cfg.set, sizeof(uint8));
    osal_memcpy(port, &mdio_cfg.port, sizeof(uint8));
    osal_memcpy(phyid, &mdio_cfg.phyid, sizeof(uint8));
    osal_memcpy(fmt, &mdio_cfg.fmt, sizeof(rtk_mdio_format_t));

    return RT_ERR_OK;
}   /* end of rtk_mdio_cfg_get */

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
int32
rtk_mdio_c22_write(uint8 c22_reg,uint16 data)
{
    rtdrv_mdioCfg_t mdio_cfg;
    memset(&mdio_cfg, 0x0, sizeof(rtdrv_mdioCfg_t));

    /* function body */
    osal_memcpy(&mdio_cfg.c22_reg, &c22_reg, sizeof(uint8));
    osal_memcpy(&mdio_cfg.data, &data, sizeof(uint16));
    SETSOCKOPT(RTDRV_MDIO_C22_WRITE, &mdio_cfg, rtdrv_mdioCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_mdio_c22_write */

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
int32
rtk_mdio_c22_read(uint8 c22_reg,uint16 *data)
{
    rtdrv_mdioCfg_t mdio_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == data), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&mdio_cfg.c22_reg, &c22_reg, sizeof(uint8));
    GETSOCKOPT(RTDRV_MDIO_C22_READ, &mdio_cfg, rtdrv_mdioCfg_t, 1);
    osal_memcpy(data, &mdio_cfg.data, sizeof(uint16));

    return RT_ERR_OK;
}   /* end of rtk_mdio_c22_read */

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
int32
rtk_mdio_c45_write(uint8 dev,uint16 c45_reg,uint16 data)
{
    rtdrv_mdioCfg_t mdio_cfg;
    memset(&mdio_cfg, 0x0, sizeof(rtdrv_mdioCfg_t));

    /* function body */
    osal_memcpy(&mdio_cfg.dev, &dev, sizeof(uint8));
    osal_memcpy(&mdio_cfg.c45_reg, &c45_reg, sizeof(uint16));
    osal_memcpy(&mdio_cfg.data, &data, sizeof(uint16));
    SETSOCKOPT(RTDRV_MDIO_C45_WRITE, &mdio_cfg, rtdrv_mdioCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_mdio_c45_write */

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
int32
rtk_mdio_c45_read(uint8 dev,uint16 c45_reg,uint16 *data)
{
    rtdrv_mdioCfg_t mdio_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == data), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&mdio_cfg.dev, &dev, sizeof(uint8));
    osal_memcpy(&mdio_cfg.c45_reg, &c45_reg, sizeof(uint16));
    GETSOCKOPT(RTDRV_MDIO_C45_READ, &mdio_cfg, rtdrv_mdioCfg_t, 1);
    osal_memcpy(data, &mdio_cfg.data, sizeof(uint16));

    return RT_ERR_OK;
}   /* end of rtk_mdio_c45_read */
 
