/*
 * Copyright (C) 2018 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: $
 * $Date: $
 *
 * Purpose : Definition of Switch Global API
 *
 * Feature : The file have include the following module and sub-modules
 *           (1) Switch parameter settings
 *           (2) Management address and vlan configuration.
 *
 */

/*
 * Include Files
 */
#if defined(CONFIG_SDK_KERNEL_LINUX)
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#include <linux/string.h>
#include <linux/version.h>
#endif

#include <common/rt_type.h>
#include <hal/chipdef/chip.h>
#include <rtk/init.h>
#include <rtk/default.h>
#include <rtk/l2.h>
#include <dal/dal_mgmt.h>
#include <common/util/rt_util.h>
#include <hal/common/halctrl.h>
#include <rtk/rt/rt_switch.h>
#include <rtk/rt/rt_rate.h>
#include <rtk/rt/rt_port.h>


#ifdef CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE
#include <rt/include/rt_ext_mapper.h>

static rt_ext_mapper_t* RT_EXT_MAPPER = NULL;
#endif


#ifdef CONFIG_EXTERNAL_SWITCH
#include <dal/dal_ext_switch_mapper.h>
#endif

rt_switch_mode_t curMode = RT_SWITCH_MODE_END;


/*
 * Function Declaration
 */
#if defined(CONFIG_SDK_KERNEL_LINUX)
struct proc_dir_entry* rt_api_proc_dir = NULL;
struct proc_dir_entry* rt_rate_proc_dir = NULL;
struct proc_dir_entry* rt_rate_v_idx_entry = NULL;

EXPORT_SYMBOL(rt_api_proc_dir);

static int rt_rate_v_idx_read(struct seq_file *seq, void *v)
{
    seq_printf(seq, "echo all > /proc/rt_api/rt_rate/v_idx to get rt rate meter virtual index mapping\n");
    seq_printf(seq, "echo v_idx > /proc/rt_api/rt_rate/v_idx to get rt rate meter virtual index mapping\n");

    return 0;
}

static int rt_rate_v_idx_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
    unsigned char tmpBuf[16] = {0};
    int len = (count > 15) ? 15 : count;
    rt_rate_meter_mapping_t meterMap;
    uint32 v_idx,init_val,max_val,rate,bucketSize;
    int32 ret;
    rt_enable_t ifgInclude;
#ifdef CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE
    rt_internal_meterConfig_t meterConf;

    if(NULL == RT_EXT_MAPPER)
    {
        RT_EXT_MAPPER = rt_ext_mapper_get();
    }
#endif

    if (buffer && !copy_from_user(tmpBuf, buffer, len))
    {
        if(strncmp(tmpBuf,"all",3) == 0)
        {
            init_val = 0;
            max_val = RT_RATE_V_IDX_MAX_NUM;
        }
        else
        {
            tmpBuf[len] = '\0';
            init_val = simple_strtoul(tmpBuf, NULL, 10);
            max_val = init_val + 1;
        }

        for(v_idx=init_val;v_idx<max_val;v_idx++)
        {
            ret=rt_rate_shareMeterMappingHw_get(v_idx,&meterMap);
            if(ret != RT_ERR_OK)
            {
                continue;
            }

            printk("v_idx=%5d type=%2d hw_type=%2d hw_index=%4d hw_index_ext=%4d\n",v_idx,meterMap.type,meterMap.hw_type,meterMap.hw_index,meterMap.hw_index_ext);

            if(meterMap.hw_index == -1)
                continue;

            switch(meterMap.type)
            {
                case RT_METER_TYPE_STORM:
                    printk("\tStorm\n");
                    break;
                case RT_METER_TYPE_HOST:
                    printk("\tHost\n");
                    break;
                case RT_METER_TYPE_FLOW:
                    printk("\tFlow\n");
                    break;
                case RT_METER_TYPE_ACL:
                    printk("\tACL\n");
                    break;
                case RT_METER_TYPE_SW:
                    printk("\tSW\n");
                    break;
                default:
                    continue;
                    break;
            }

            switch(meterMap.hw_type)
            {
                case RT_METER_HW_TYPE_L2:
                    ret = RT_MAPPER->rate_shareMeter_get(meterMap.hw_index,&rate,&ifgInclude);
                    if(ret != RT_ERR_OK)
                    {
                        printk("\t\tcan't get L2 meter\n");
                        continue;
                    }
                    ret = RT_MAPPER->rate_shareMeterBucket_get(meterMap.hw_index,&bucketSize);
                    if(ret != RT_ERR_OK)
                    {
                        printk("\t\tcan't get L2 bucket size\n");
                        continue;
                    }
                    printk("\t\tL2 rate = %8d Kbps, burst = %4d\n",rate,bucketSize);
                    break;
#ifdef CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE
                case RT_METER_HW_TYPE_L3:
                case RT_METER_HW_TYPE_SW:
                    if(NULL == RT_EXT_MAPPER)
                    {
                        printk("\t\tcan't get RT_EXT_MAPPER\n");
                        continue;
                    }
                    
                    ret = RT_EXT_MAPPER->internal_l3SwMeter_get(meterMap.type,meterMap.hw_index,&meterConf);
                    if(ret != RT_ERR_OK)
                    {
                        printk("\t\tcan't get L3 meter\n");
                        continue;
                    }
                    printk("\t\tL3 rate = %8d Kbps, burst = %4d\n",meterConf.rate,meterConf.bucket_size);
                    break;
#endif
                default:
                    continue;
                    break;
            }

#ifdef CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE
#if defined(CONFIG_SDK_CA8279) || defined(CONFIG_SDK_CA8277B) || defined(CONFIG_SDK_RTL8198F)
            if(meterMap.type == RT_METER_TYPE_STORM)
            {
                    if(NULL == RT_EXT_MAPPER)
                    {
                        printk("\t\tcan't get RT_EXT_MAPPER\n");
                        continue;
                    }
                    
                    ret = RT_EXT_MAPPER->internal_l3SwMeter_get(RT_METER_TYPE_ACL,meterMap.hw_index_ext,&meterConf);
                    if(ret != RT_ERR_OK)
                    {
                        printk("\t\tcan't get L3 meter\n");
                        continue;
                    }
                    printk("\t\tL3 rate = %8d Kbps, burst = %4d\n",meterConf.rate,meterConf.bucket_size);
            }
#endif
#endif

#if 0
            switch(meterMap.type)
            {
                case RT_METER_TYPE_STORM:
                    ret = RT_MAPPER->rate_shareMeter_get(meterMap.hw_index,&rate,&ifgInclude);
                    if(ret != RT_ERR_OK)
                    {
                        printk("\t\tcan't get L2 meter\n");
                        continue;
                    }
                    ret = RT_MAPPER->rate_shareMeterBucket_get(meterMap.hw_index,&bucketSize);
                    if(ret != RT_ERR_OK)
                    {
                        printk("\t\tcan't get L2 bucket size\n");
                        continue;
                    }
                    printk("\t\tL2 rate = %8d Kbps, burst = %4d\n",rate,bucketSize);
#ifdef CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE
#if defined(CONFIG_SDK_CA8279) || defined(CONFIG_SDK_CA8277B) || defined(CONFIG_SDK_RTL8198F)
                    if(NULL == RT_EXT_MAPPER)
                    {
                        printk("\t\tcan't get RT_EXT_MAPPER\n");
                        continue;
                    }
                    
                    ret = RT_EXT_MAPPER->internal_l3SwMeter_get(RT_METER_TYPE_ACL,meterMap.hw_index_ext,&meterConf);
                    if(ret != RT_ERR_OK)
                    {
                        printk("\t\tcan't get L3 meter\n");
                        continue;
                    }
                    printk("\t\tL3 rate = %8d Kbps, burst = %4d\n",meterConf.rate,meterConf.bucket_size);
#endif
#endif
                    break;
#ifdef CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE
                case RT_METER_TYPE_HOST:
#if defined(CONFIG_SDK_CA8279) || defined(CONFIG_SDK_CA8277B) || defined(CONFIG_SDK_RTL8198F)
                    if(NULL == RT_EXT_MAPPER)
                    {
                        printk("\t\tcan't get RT_EXT_MAPPER\n");
                        continue;
                    }
                    
                    ret = RT_EXT_MAPPER->internal_l3SwMeter_get(meterMap.type,meterMap.hw_index,&meterConf);
                    if(ret != RT_ERR_OK)
                    {
                        printk("\t\tcan't get L3 meter\n");
                        continue;
                    }
                    printk("\t\tL3 rate = %8d Kbps, burst = %4d\n",meterConf.rate,meterConf.bucket_size);
#elif defined(CONFIG_SDK_RTL9607C)
                    ret = RT_MAPPER->rate_shareMeter_get(meterMap.hw_index,&rate,&ifgInclude);
                    if(ret != RT_ERR_OK)
                    {
                        printk("\t\tcan't get L2 meter\n");
                        continue;
                    }
                    ret = RT_MAPPER->rate_shareMeterBucket_get(meterMap.hw_index,&bucketSize);
                    if(ret != RT_ERR_OK)
                    {
                        printk("\t\tcan't get L2 bucket size\n");
                        continue;
                    }
                    printk("\t\tL2 rate = %8d Kbps, burst = %4d\n",rate,bucketSize);
#endif
                    break;
                case RT_METER_TYPE_FLOW:
                    if(NULL == RT_EXT_MAPPER)
                    {
                        printk("\t\tcan't get RT_EXT_MAPPER\n");
                        continue;
                    }
                    
                    ret = RT_EXT_MAPPER->internal_l3SwMeter_get(meterMap.type,meterMap.hw_index,&meterConf);
                    if(ret != RT_ERR_OK)
                    {
                        printk("\t\tcan't get L3 meter\n");
                        continue;
                    }
                    printk("\t\tL3 rate = %8d Kbps, burst = %4d\n",meterConf.rate,meterConf.bucket_size);
                    break;
                case RT_METER_TYPE_ACL:
#if defined(CONFIG_SDK_CA8279) || defined(CONFIG_SDK_CA8277B) || defined(CONFIG_SDK_RTL8198F)
                    if(NULL == RT_EXT_MAPPER)
                    {
                        printk("\t\tcan't get RT_EXT_MAPPER\n");
                        continue;
                    }
                    
                    ret = RT_EXT_MAPPER->internal_l3SwMeter_get(meterMap.type,meterMap.hw_index,&meterConf);
                    if(ret != RT_ERR_OK)
                    {
                        printk("\t\tcan't get L3 meter\n");
                        continue;
                    }
                    printk("\t\tL3 rate = %8d Kbps, burst = %4d\n",meterConf.rate,meterConf.bucket_size);
#elif defined(CONFIG_SDK_RTL9607C)
                    ret = RT_MAPPER->rate_shareMeter_get(meterMap.hw_index,&rate,&ifgInclude);
                    if(ret != RT_ERR_OK)
                    {
                        printk("\t\tcan't get L2 meter\n");
                        continue;
                    }
                    ret = RT_MAPPER->rate_shareMeterBucket_get(meterMap.hw_index,&bucketSize);
                    if(ret != RT_ERR_OK)
                    {
                        printk("\t\tcan't get L2 bucket size\n");
                        continue;
                    }
                    printk("\t\tL2 rate = %8d Kbps, burst = %4d\n",rate,bucketSize);
#endif
                    break;
                case RT_METER_TYPE_SW:
                    if(NULL == RT_EXT_MAPPER)
                    {
                        printk("\t\tcan't get RT_EXT_MAPPER\n");
                        continue;
                    }
                    
                    ret = RT_EXT_MAPPER->internal_l3SwMeter_get(meterMap.type,meterMap.hw_index,&meterConf);
                    if(ret != RT_ERR_OK)
                    {
                        printk("\t\tcan't get L3 meter\n");
                        continue;
                    }
                    printk("\t\tL3 rate = %8d Kbps, burst = %4d\n",meterConf.rate,meterConf.bucket_size);
                    break;
#endif
                default:
                    continue;
                    break;
            }
#endif
        }
        
        return count;
    }
    return -EFAULT;
}

static int rt_rate_v_idx_open_proc(struct inode *inode, struct file *file)
{
    return single_open(file, rt_rate_v_idx_read, inode->i_private);
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
static const struct proc_ops rt_rate_v_idx_fop = {
    .proc_open           = rt_rate_v_idx_open_proc,
    .proc_write          = rt_rate_v_idx_write,
    .proc_read           = seq_read,
    .proc_lseek          = seq_lseek,
    .proc_release        = single_release,
};
#else
static const struct file_operations rt_rate_v_idx_fop = {
    .owner          = THIS_MODULE,
    .open           = rt_rate_v_idx_open_proc,
    .write          = rt_rate_v_idx_write,
    .read           = seq_read,
    .llseek         = seq_lseek,
    .release        = single_release,
};
#endif
#endif

/* Module Name    : Switch     */
/* Sub-module Name: Switch parameter settings */

/* Function Name:
 *      rt_switch_init
 * Description:
 *      Initialize switch module of the specified device.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Module must be initialized before using all of APIs in this module
 */
int32
rt_switch_init(void)
{
    int32   ret;
 
    /* function body */
    if (NULL == RT_MAPPER->switch_init)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->switch_init();
    RTK_API_UNLOCK();
    if(RT_ERR_OK != ret)
        return ret;
#if defined(CONFIG_SDK_KERNEL_LINUX)
    if(NULL == rt_api_proc_dir) {
        rt_api_proc_dir = proc_mkdir ("rt_api", NULL);

        if (NULL == rt_rate_proc_dir) {
            rt_rate_proc_dir = proc_mkdir ("rt_rate", rt_api_proc_dir);

            rt_rate_v_idx_entry = proc_create("v_idx", 0644, rt_rate_proc_dir, &rt_rate_v_idx_fop);
            if (!rt_rate_v_idx_entry) {
                printk("rt_rate_v_idx_entry, create proc failed!");
            }
        }
    }
#endif

    /*move this to the end of rt_init_without_pon to avoid mode_set fail due to l2 not initial*/
    /*ret = rt_switch_mode_set(RT_SWITCH_MODE_HYBRID);*/

    return ret;
}   /* end of rt_switch_init */

/* Function Name:
 *      rt_switch_deviceInfo_get
 * Description:
 *      Get device information of the specific unit
 * Input:
 *      none
 * Output:
 *      pDevInfo - pointer to the device information
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rt_switch_deviceInfo_get(rt_switch_devInfo_t *pDevInfo)
{
    int32   ret = RT_ERR_OK;
    hal_control_t *pHal_control;
#if defined(CONFIG_EXTERNAL_SWITCH)
    rtk_switch_devInfo_t extSwDevInfo;
#endif

    RT_PARAM_CHK((NULL == pDevInfo), RT_ERR_NULL_POINTER);

#if defined(CONFIG_EXTERNAL_SWITCH)
    memset(&extSwDevInfo, 0, sizeof(extSwDevInfo));
#endif

    RTK_API_LOCK();
    if ((pHal_control = hal_ctrlInfo_get()) == NULL)
    {
        ret = RT_ERR_FAILED;
    }
    RTK_API_UNLOCK();

    if(ret != RT_ERR_OK)
        return ret;

    pDevInfo->chipId    = pHal_control->chip_id;
    pDevInfo->revision  = pHal_control->chip_rev_id;
    pDevInfo->port_number = pHal_control->pDev_info->pPortinfo->port_number;
    pDevInfo->fe        = pHal_control->pDev_info->pPortinfo->fe;
    pDevInfo->ge        = pHal_control->pDev_info->pPortinfo->ge;
    pDevInfo->ge_combo  = pHal_control->pDev_info->pPortinfo->ge_combo;
    pDevInfo->serdes    = pHal_control->pDev_info->pPortinfo->serdes;
    pDevInfo->ether     = pHal_control->pDev_info->pPortinfo->ether;
    pDevInfo->all       = pHal_control->pDev_info->pPortinfo->all;
    pDevInfo->cpuPort   = pHal_control->pDev_info->pPortinfo->cpuPort;
    pDevInfo->rgmiiPort = pHal_control->pDev_info->pPortinfo->rgmiiPort;
    pDevInfo->dsl        = pHal_control->pDev_info->pPortinfo->dsl;
    pDevInfo->ext        = pHal_control->pDev_info->pPortinfo->ext;
    pDevInfo->cpu        = pHal_control->pDev_info->pPortinfo->cpu;
    pDevInfo->ponPort    = pHal_control->pDev_info->pPortinfo->ponPort;
    pDevInfo->swpbo_lb   = pHal_control->pDev_info->pPortinfo->swpbo_lb;
    pDevInfo->swPboLbPort= pHal_control->pDev_info->pPortinfo->swPboLbPort;
    memcpy(&pDevInfo->capacityInfo, pHal_control->pDev_info->pCapacityInfo, sizeof(rt_register_capacity_t));


#if defined(CONFIG_EXTERNAL_SWITCH)  
    /* function body */
    if (NULL == RT_EXT_SWITCH_MAPPER || NULL == RT_EXT_SWITCH_MAPPER->switch_deviceInfo_get)
        return RT_ERR_OK;
    RTK_API_LOCK();
    ret = RT_EXT_SWITCH_MAPPER->switch_deviceInfo_get((rtk_switch_devInfo_t *)&extSwDevInfo);
    RTK_API_UNLOCK();

    pDevInfo->fe.portmask.bits[0]  |=  extSwDevInfo.fe.portmask.bits[0];
    pDevInfo->ge.portmask.bits[0]  |=  extSwDevInfo.ge.portmask.bits[0];
#endif


    return ret;
} /* end of rt_switch_deviceInfo_get */

/* Function Name:
 *      rt_switch_phyPortId_get
 * Description:
 *      Get physical port id from logical port name
 * Input:
 *      portName - logical port name
 * Output:
 *      pPortId  - pointer to the physical port id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Call RTK API the port ID must get from this API
 */
int32
rt_switch_phyPortId_get(rt_switch_port_name_t portName, int32 *pPortId)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->switch_phyPortId_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->switch_phyPortId_get(portName, pPortId);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_switch_phyPortId_get */

/* Function Name:
 *      rt_switch_version_get
 * Description:
 *      Get chip version
 * Input:
 *      pChipId    - chip id
 *      pRev       - revision id
 *      pSubtype   - sub type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 */
int32
rt_switch_version_get(uint32 *pChipId, uint32 *pRev, uint32 *pSubtype)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->switch_version_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->switch_version_get(pChipId, pRev, pSubtype);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_switch_version_get */

/* Function Name:
  *      rt_switch_maxPktLenByPort_get
  * Description:
  *      Get the max packet length setting of specific port
  * Input:
  *      port - speed type
  * Output:
  *      pLen - pointer to the max packet length
  * Return:
  *      RT_ERR_OK
  *      RT_ERR_FAILED
  *      RT_ERR_NULL_POINTER - input parameter may be null pointer
  *      RT_ERR_INPUT        - invalid enum speed type
  * Note:
  */
int32
rt_switch_maxPktLenByPort_get(rt_port_t port, uint32 *pLen)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->switch_maxPktLenByPort_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->switch_maxPktLenByPort_get(port, pLen);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_switch_maxPktLenByPort_get */

/* Function Name:
  *      rt_switch_maxPktLenByPort_set
  * Description:
  *      Set the max packet length of specific port
  * Input:
  *      port  - port
  *      len   - max packet length
  * Output:
  *      None
  * Return:
  *      RT_ERR_OK
  *      RT_ERR_FAILED
  *      RT_ERR_INPUT   - invalid enum speed type
  * Note:
  */
int32
rt_switch_maxPktLenByPort_set(rt_port_t port, uint32 len)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->switch_maxPktLenByPort_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->switch_maxPktLenByPort_set(port, len);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_switch_maxPktLenByPort_set */

/* Function Name:
  *      rt_switch_mode_set
  * Description:
  *      Set switch mode
  * Input:
  *      mode  - switch mode
  * Output:
  *      None
  * Return:
  *      RT_ERR_OK
  *      RT_ERR_FAILED
  *      RT_ERR_INPUT   - invalid enum mode type
  * Note:
  */
extern int32
rt_switch_mode_set(rt_switch_mode_t mode)
{
    int32   ret = RT_ERR_OK;
    if(mode >= RT_SWITCH_MODE_END)
        return RT_ERR_INPUT;

    if(curMode == mode)
        return RT_ERR_OK;

    curMode = mode;
#ifdef CONFIG_LUNA_G3_SERIES
    rt_port_t port;
    rt_port_uniType_t type;
    rtk_action_t fwdAction;
    if(mode == RT_SWITCH_MODE_SFU)
    {
        type = RT_PORT_UNI_TYPE_PPTP;
        fwdAction = ACTION_FORWARD;
    }
    else
    {
        type = RT_PORT_UNI_TYPE_VEIP;
        fwdAction = ACTION_FOLLOW_FB;
    }

    HAL_SCAN_ALL_ETH_PORT(port)
    {
        if(!HAL_IS_PON_PORT(port))
        {
            ret = rt_port_uniType_set(port,type);
            if(ret != RT_ERR_OK && ret != RT_ERR_NOT_INIT)
                return ret;
        }
    }

    if (NULL == RT_MAPPER->l2_portLookupMissAction_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    if (NULL == RT_MAPPER->l2_lookupMissAction_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();

    ret = RT_MAPPER->l2_lookupMissAction_set(DLF_TYPE_IPMC, fwdAction);
    if(ret != RT_ERR_OK)
    {
        RTK_API_UNLOCK();
        return ret;
    }
    ret = RT_MAPPER->l2_lookupMissAction_set(DLF_TYPE_BCAST, fwdAction);
    if(ret != RT_ERR_OK)
    {
        RTK_API_UNLOCK();
        return ret;
    }
    ret = RT_MAPPER->l2_lookupMissAction_set(DLF_TYPE_MCAST, fwdAction);
    if(ret != RT_ERR_OK)
    {
        RTK_API_UNLOCK();
        return ret;
    }

    /*For PON port, always set uc to forward*/
    ret = RT_MAPPER->l2_portLookupMissAction_set(HAL_GET_PON_PORT(), DLF_TYPE_UCAST, ACTION_FORWARD);
    if(ret != RT_ERR_OK)
    {
        RTK_API_UNLOCK();
        return ret;
    }

    RTK_API_UNLOCK();
    return ret;
#else
    /* TBD */
#endif
    return ret;
}

/* Function Name:
  *      rt_switch_mode_get
  * Description:
  *      Get switch mode
  * Input:
  *      None
  * Output:
  *      mode  - switch mode
  * Return:
  *      RT_ERR_OK
  *      RT_ERR_FAILED
  *      RT_ERR_INPUT   - invalid enum mode type
  * Note:
  */
extern int32
rt_switch_mode_get(rt_switch_mode_t *pMode)
{
    *pMode = curMode;
    return 0;
}

