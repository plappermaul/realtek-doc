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
 *           (1) MDIO
 *
 */

#include <rtk/init.h>
#include <rtk/default.h>
#include <rtk/rt/rt_mdio.h>
#include <dal/dal_mgmt.h>
#include <common/rt_type.h>

#if defined(CONFIG_SDK_KERNEL_LINUX)
#if defined(CONFIG_LUNA_G3_SERIES)
#include <linux/mutex.h>
extern struct mutex rtkMdioMutex;

#define RTK_MDIO_ACCESS_LOCK      mutex_lock(&rtkMdioMutex)
#define RTK_MDIO_ACCESS_UNLOCK    mutex_unlock(&rtkMdioMutex)
#else
#include <linux/spinlock.h>
extern spinlock_t rtkMdioSpinLock;
extern unsigned long rtkMdioSpinflags;
#define RTK_MDIO_ACCESS_LOCK      spin_lock_irqsave(&rtkMdioSpinLock,rtkMdioSpinflags)
#define RTK_MDIO_ACCESS_UNLOCK    spin_unlock_irqrestore(&rtkMdioSpinLock,rtkMdioSpinflags)
#endif
#else
#define RTK_MDIO_ACCESS_LOCK
#define RTK_MDIO_ACCESS_UNLOCK
#endif

/* Function Name:
 *      rt_mdio_init
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
rt_mdio_init(void)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->mdio_init)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->mdio_init();
    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_mdio_init */

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
 *      RT_ERR_OK
 *      RT_ERR_FAILED
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
int32
rt_mdio_cfg_set(uint8 set,uint8 port,uint8 phyid,rt_mdio_format_t fmt)
{
    int32   ret;
    rtk_mdio_format_t rtkFmt;

    /* function body */
    if (NULL == RT_MAPPER->mdio_cfg_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    /* Translate RT data type to RTK data type for DAL */
    switch(fmt)
    {
    case RT_MDIO_FMT_C22:
        rtkFmt = MDIO_FMT_C22;
        break;
    case RT_MDIO_FMT_C45:
        rtkFmt = MDIO_FMT_C45;
        break;
    case RT_MDIO_FMT_END:
    default:
        rtkFmt = MDIO_FMT_END;
        break;
    }

    RTK_API_LOCK();
    ret = RT_MAPPER->mdio_cfg_set(set, port, phyid, rtkFmt);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_mdio_cfg_set */

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
 *      RT_ERR_OK
 *      RT_ERR_FAILED
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
int32
rt_mdio_cfg_get(uint8 *set,uint8 *port,uint8 *phyid,rt_mdio_format_t *fmt)
{
    int32   ret;
    rtk_mdio_format_t rtkFmt;

    /* function body */
    if (NULL == RT_MAPPER->mdio_cfg_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->mdio_cfg_get(set, port, phyid, &rtkFmt);
    RTK_API_UNLOCK();

    /* Translate RTK data type to RT data type for RT API */
    switch(rtkFmt)
    {
    case MDIO_FMT_C22:
        *fmt = RT_MDIO_FMT_C22;
        break;
    case MDIO_FMT_C45:
        *fmt = RT_MDIO_FMT_C45;
        break;
    case MDIO_FMT_END:
    default:
        *fmt = RT_MDIO_FMT_END;
        break;
    }

    return ret;
}   /* end of rt_mdio_cfg_get */

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
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      The collective of APIs 
 *          rt_mdio_cfg_set/rt_mdio_cfg_get
 *          rt_mdio_c22_write/rt_mdio_c22_read 
 *          rt_mdio_c45_write/rt_mdio_c45_read
 *      are not recommended anymore. User are encurage to use
 *          rt_mdio_read/rt_mdio_write
 *      instead
 */
int32
rt_mdio_c22_write(uint8 c22_reg,uint16 data)
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
}   /* end of rt_mdio_c22_write */

/* Function Name:
 *      rt_mdio_c22_read
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
 *      The collective of APIs 
 *          rt_mdio_cfg_set/rt_mdio_cfg_get
 *          rt_mdio_c22_write/rt_mdio_c22_read 
 *          rt_mdio_c45_write/rt_mdio_c45_read
 *      are not recommended anymore. User are encurage to use
 *          rt_mdio_read/rt_mdio_write
 *      instead
 */
int32
rt_mdio_c22_read(uint8 c22_reg,uint16 *data)
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
}   /* end of rt_mdio_c22_read */

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
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      The collective of APIs 
 *          rt_mdio_cfg_set/rt_mdio_cfg_get
 *          rt_mdio_c22_write/rt_mdio_c22_read 
 *          rt_mdio_c45_write/rt_mdio_c45_read
 *      are not recommended anymore. User are encurage to use
 *          rt_mdio_read/rt_mdio_write
 *      instead
 */
int32
rt_mdio_c45_write(uint8 dev,uint16 c45_reg,uint16 data)
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
}   /* end of rt_mdio_c45_write */

/* Function Name:
 *      rt_mdio_c45_write
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
 *      The collective of APIs 
 *          rt_mdio_cfg_set/rt_mdio_cfg_get
 *          rt_mdio_c22_write/rt_mdio_c22_read 
 *          rt_mdio_c45_write/rt_mdio_c45_read
 *      are not recommended anymore. User are encurage to use
 *          rt_mdio_read/rt_mdio_write
 *      instead
 */
int32
rt_mdio_c45_read(uint8 dev,uint16 c45_reg,uint16 *data)
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
}   /* end of rt_mdio_c45_read */

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
int32
rt_mdio_read(rt_mdio_info_t *info, uint16 *data)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->mdio_c45_read)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    RTK_MDIO_ACCESS_LOCK;
    ret = RT_MAPPER->mdio_read(info, data);
    RTK_MDIO_ACCESS_UNLOCK;
    RTK_API_UNLOCK();

    return ret;
}

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
rt_mdio_write(rt_mdio_info_t *info, uint16 data)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->mdio_c45_read)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    RTK_MDIO_ACCESS_LOCK;
    ret = RT_MAPPER->mdio_write(info, data);
    RTK_MDIO_ACCESS_UNLOCK;
    RTK_API_UNLOCK();

    return ret;
}

