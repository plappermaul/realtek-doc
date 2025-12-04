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

#include <rtk/init.h>
#include <rtk/default.h>
#include <rtk/mdio.h>
#include <dal/dal_mgmt.h>
#include <common/rt_type.h>

#if defined(CONFIG_SDK_KERNEL_LINUX)
#if defined(CONFIG_LUNA_G3_SERIES)
#include <linux/mutex.h>
DEFINE_MUTEX(rtkMdioMutex);

#define RTK_MDIO_ACCESS_LOCK      mutex_lock(&rtkMdioMutex)
#define RTK_MDIO_ACCESS_UNLOCK    mutex_unlock(&rtkMdioMutex)
#else
#include <linux/spinlock.h>
#if defined(CONFIG_KERNEL_2_6_30)
spinlock_t rtkMdioSpinLock = SPIN_LOCK_UNLOCKED;
#else
DEFINE_SPINLOCK(rtkMdioSpinLock);
#endif
unsigned long rtkMdioSpinflags;

#define RTK_MDIO_ACCESS_LOCK      spin_lock_irqsave(&rtkMdioSpinLock,rtkMdioSpinflags)
#define RTK_MDIO_ACCESS_UNLOCK    spin_unlock_irqrestore(&rtkMdioSpinLock,rtkMdioSpinflags)
#endif
#else
#define RTK_MDIO_ACCESS_LOCK
#define RTK_MDIO_ACCESS_UNLOCK
#endif


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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->mdio_init)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->mdio_init();
    RTK_API_UNLOCK();
    return ret;
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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->mdio_cfg_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->mdio_cfg_set(set, port, phyid, fmt);
    RTK_API_UNLOCK();
    return ret;
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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->mdio_cfg_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->mdio_cfg_get(set, port, phyid, fmt);
    RTK_API_UNLOCK();
    return ret;
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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->mdio_c22_write)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    RTK_MDIO_ACCESS_LOCK;
    ret = RT_MAPPER->mdio_c22_write(c22_reg, data);
    RTK_MDIO_ACCESS_UNLOCK;
    RTK_API_UNLOCK();
    return ret;
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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->mdio_c22_read)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    RTK_MDIO_ACCESS_LOCK;
    ret = RT_MAPPER->mdio_c22_read(c22_reg, data);
    RTK_MDIO_ACCESS_UNLOCK;
    RTK_API_UNLOCK();
    return ret;
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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->mdio_c45_write)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    RTK_MDIO_ACCESS_LOCK;
    ret = RT_MAPPER->mdio_c45_write(dev, c45_reg, data);
    RTK_MDIO_ACCESS_UNLOCK;
    RTK_API_UNLOCK();
    return ret;
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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->mdio_c45_read)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    RTK_MDIO_ACCESS_LOCK;
    ret = RT_MAPPER->mdio_c45_read(dev, c45_reg, data);
    RTK_MDIO_ACCESS_UNLOCK;
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_mdio_c45_read */
 
