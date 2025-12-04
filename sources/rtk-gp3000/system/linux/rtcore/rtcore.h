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
 * Purpose : Realtek Switch SDK Core Module.
 *
 * Feature : Realtek Switch SDK Core Module
 *
 */

#ifndef __RTCORE_DRV_H__
#define __RTCORE_DRV_H__

/*
 * Include Files
 */
#include <linux/ioctl.h>
#include <common/error.h>

/*
 * Symbol Definition
 */
#define RTCORE_DEV_NAME         "/dev/rtcore"

#ifndef RTCORE_DEV_NUM
#define RTCORE_DEV_NUM          1
#endif

#define RTCORE_IOCTL_MAGIC      'R'
#define RTCORE_IOCTL_DATA_NUM   8

#define RTCORE_CID_GET          _IOWR(RTCORE_IOCTL_MAGIC, 0, int)
#define RTCORE_SEM_CREATE       _IOWR(RTCORE_IOCTL_MAGIC, 1, int)   
#define RTCORE_SEM_DESTROY      _IO(RTCORE_IOCTL_MAGIC, 2)   
#define RTCORE_SEM_TAKE         _IOWR(RTCORE_IOCTL_MAGIC, 3, int)
#define RTCORE_SEM_GIVE         _IOWR(RTCORE_IOCTL_MAGIC, 4, int)
#define RTCORE_UT_RUN           _IOWR(RTCORE_IOCTL_MAGIC, 5, int)
#define RTCORE_NIC_DBG_GET      _IOWR(RTCORE_IOCTL_MAGIC, 6, int)
#define RTCORE_NIC_DBG_SET      _IOWR(RTCORE_IOCTL_MAGIC, 7, int)
#define RTCORE_NIC_CNTR_CLEAR   _IOWR(RTCORE_IOCTL_MAGIC, 8, int)
#define RTCORE_NIC_CNTR_DUMP    _IOWR(RTCORE_IOCTL_MAGIC, 9, int)
#define RTCORE_NIC_BUF_DUMP     _IOWR(RTCORE_IOCTL_MAGIC, 10, int)
#define RTCORE_NIC_PHMBUF_DUMP  _IOWR(RTCORE_IOCTL_MAGIC, 11, int)
#define RTCORE_NIC_RX_START     _IOWR(RTCORE_IOCTL_MAGIC, 12, int)
#define RTCORE_NIC_RX_STOP      _IOWR(RTCORE_IOCTL_MAGIC, 13, int)
#define RTCORE_NIC_RX_STATUS_GET    _IOWR(RTCORE_IOCTL_MAGIC, 14, int)
#define RTCORE_CID_CMP          _IOWR(RTCORE_IOCTL_MAGIC, 15, int)
#define RTCORE_CACHE_FLUSH      _IOWR(RTCORE_IOCTL_MAGIC, 16, int)
#define RTCORE_GPIO_DATABIT_GET _IOWR(RTCORE_IOCTL_MAGIC, 17, int)
#define RTCORE_INTR_ENABLE_SET  _IOWR(RTCORE_IOCTL_MAGIC, 18, int)
#define RTCORE_INTR_WAIT        _IOWR(RTCORE_IOCTL_MAGIC, 19, int)
#define RTCORE_GPIO_DATABIT_SET _IOWR(RTCORE_IOCTL_MAGIC, 20, int)
#define RTCORE_GPIO_INIT        _IOWR(RTCORE_IOCTL_MAGIC, 21, int)

/*
 * Data Declaration
 */
typedef struct rtcore_ioctl_s
{
    int32 ret;
    int32 data[RTCORE_IOCTL_DATA_NUM];                
} rtcore_ioctl_t;

typedef struct rtcore_dev_data_s
{
    uint32 log_level;
    uint32 log_mask;
    uint32 log_type;
    uint64 log_module_mask;
    uint32 log_format;
    uint32 log_level_bak;
    uint32 log_mask_bak;

} rtcore_dev_data_t;

/*
 * Macro Declaration
 */

/*
 * Function Declaration
 */



#endif /* __RTCORE_DRV_H__ */

