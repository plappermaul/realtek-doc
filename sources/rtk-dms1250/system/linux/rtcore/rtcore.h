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
 * $Revision: 102110 $
 * $Date: 2019-11-28 21:16:01 +0800 (Thu, 28 Nov 2019) $
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
#include <common/error.h>

/*
 * Symbol Definition
 */
#define RTCORE_DEV_NAME         "/dev/rtcore"
#define MEM_RESERVED_SIZE       4096

#ifndef RTCORE_DEV_NUM
#define RTCORE_DEV_NUM          1
#define RTCORE_DEV_NUM_DEFAULT  0
#endif

#define RTCORE_IOCTL_MAGIC      'R'
#define RTCORE_IOCTL_DATA_NUM   13

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
#define RTCORE_INTR_ENABLE_SET  _IOWR(RTCORE_IOCTL_MAGIC, 18, int)
#define RTCORE_INTR_WAIT        _IOWR(RTCORE_IOCTL_MAGIC, 19, int)
#define RTCORE_INTR_ISR_ID_SET  _IOWR(RTCORE_IOCTL_MAGIC, 20, int)

#define RTCORE_GPIO_ISR_STS_SHADOW_GET      _IOWR(RTCORE_IOCTL_MAGIC, 21, int)

#define RTCORE_EXTGPIO_REG_WRITE        _IOWR(RTCORE_IOCTL_MAGIC, 23, int)
#define RTCORE_EXTGPIO_DEV_INIT         _IOWR(RTCORE_IOCTL_MAGIC, 24, int)
#define RTCORE_EXTGPIO_DEVENABLE_GET    _IOWR(RTCORE_IOCTL_MAGIC, 25, int)
#define RTCORE_EXTGPIO_REG_READ         _IOWR(RTCORE_IOCTL_MAGIC, 26, int)
#define RTCORE_EXTGPIO_DEVREADY_GET     _IOWR(RTCORE_IOCTL_MAGIC, 27, int)
#define RTCORE_EXTGPIO_DEV_GET          _IOWR(RTCORE_IOCTL_MAGIC, 28, int)
#define RTCORE_EXTGPIO_DEVENABLE_SET    _IOWR(RTCORE_IOCTL_MAGIC, 29, int)
#define RTCORE_EXTGPIO_SYNCENABLE_GET   _IOWR(RTCORE_IOCTL_MAGIC, 30, int)
#define RTCORE_EXTGPIO_SYNCENABLE_SET   _IOWR(RTCORE_IOCTL_MAGIC, 31, int)
#define RTCORE_EXTGPIO_SYNCSTATUS_GET   _IOWR(RTCORE_IOCTL_MAGIC, 32, int)
#define RTCORE_EXTGPIO_SYNC_START       _IOWR(RTCORE_IOCTL_MAGIC, 33, int)
#define RTCORE_EXTGPIO_PIN_GET          _IOWR(RTCORE_IOCTL_MAGIC, 34, int)
#define RTCORE_EXTGPIO_PIN_INIT         _IOWR(RTCORE_IOCTL_MAGIC, 35, int)
#define RTCORE_EXTGPIO_DATABIT_GET      _IOWR(RTCORE_IOCTL_MAGIC, 36, int)
#define RTCORE_EXTGPIO_DATABIT_SET      _IOWR(RTCORE_IOCTL_MAGIC, 37, int)
#define RTCORE_RTL8231_I2C_READ         _IOWR(RTCORE_IOCTL_MAGIC, 38, int)
#define RTCORE_RTL8231_I2C_WRITE        _IOWR(RTCORE_IOCTL_MAGIC, 39, int)
#define RTCORE_RTL8231_MDC_READ         _IOWR(RTCORE_IOCTL_MAGIC, 40, int)
#define RTCORE_RTL8231_MDC_WRITE        _IOWR(RTCORE_IOCTL_MAGIC, 41, int)
#define RTCORE_EXTGPIO_DEVRECOVERY_START    _IOWR(RTCORE_IOCTL_MAGIC, 42, int)
#define RTCORE_EXTGPIO_DIRECTION_GET        _IOWR(RTCORE_IOCTL_MAGIC, 43, int)
#define RTCORE_EXTGPIO_DIRECTION_SET        _IOWR(RTCORE_IOCTL_MAGIC, 44, int)
#define RTCORE_EXTGPIO_I2C_INIT             _IOWR(RTCORE_IOCTL_MAGIC, 45, int)
#define RTCORE_EXTGPIO_I2C_READ             _IOWR(RTCORE_IOCTL_MAGIC, 46, int)
#define RTCORE_EXTGPIO_I2C_WRITE            _IOWR(RTCORE_IOCTL_MAGIC, 47, int)
#define RTCORE_NIC_PKT_TX                   _IOWR(RTCORE_IOCTL_MAGIC, 48, int)
#define RTCORE_NIC_RESET                    _IOWR(RTCORE_IOCTL_MAGIC, 49, int)

#define RTCORE_WATCHDOG_INIT                _IOWR(RTCORE_IOCTL_MAGIC, 50, int)
#define RTCORE_WATCHDOG_SCALE_SET           _IOWR(RTCORE_IOCTL_MAGIC, 53, int)
#define RTCORE_WATCHDOG_SCALE_GET           _IOWR(RTCORE_IOCTL_MAGIC, 54, int)

#define RTCORE_WATCHDOG_ENABLE_SET          _IOWR(RTCORE_IOCTL_MAGIC, 55, int)
#define RTCORE_WATCHDOG_ENABLE_GET          _IOWR(RTCORE_IOCTL_MAGIC, 56, int)
#define RTCORE_WATCHDOG_KICK                _IOWR(RTCORE_IOCTL_MAGIC, 57, int)
#define RTCORE_WATCHDOG_THRESHOLD_SET       _IOWR(RTCORE_IOCTL_MAGIC, 58, int)
#define RTCORE_WATCHDOG_THRESHOLD_GET       _IOWR(RTCORE_IOCTL_MAGIC, 59, int)

#define RTCORE_GENCTRL_GPIO_DEV_INIT		_IOWR(RTCORE_IOCTL_MAGIC, 61, int)
#define RTCORE_GENCTRL_GPIO_DEV_ENABLE_SET	_IOWR(RTCORE_IOCTL_MAGIC, 62, int)
#define RTCORE_GENCTRL_GPIO_PIN_INIT		_IOWR(RTCORE_IOCTL_MAGIC, 63, int)
#define RTCORE_GENCTRL_GPIO_DATABIT_SET		_IOWR(RTCORE_IOCTL_MAGIC, 64, int)
#define RTCORE_GENCTRL_GPIO_DATABIT_GET		_IOWR(RTCORE_IOCTL_MAGIC, 65, int)

#define RTCORE_UART1_CHAR_GET		        _IOWR(RTCORE_IOCTL_MAGIC, 66, int)
#define RTCORE_UART1_CHAR_PUT		        _IOWR(RTCORE_IOCTL_MAGIC, 67, int)
#define RTCORE_UART1_BAUDRATE_GET		    _IOWR(RTCORE_IOCTL_MAGIC, 68, int)
#define RTCORE_UART1_BAUDRATE_SET		    _IOWR(RTCORE_IOCTL_MAGIC, 69, int)
#define RTCORE_UART1_INTERFACE_SET         _IOWR(RTCORE_IOCTL_MAGIC, 70, int)

#define RTCORE_DEBUG_REGISTER_DUMP		    _IOWR(RTCORE_IOCTL_MAGIC, 71, int)

#define RTCORE_SPI_READ                     _IOWR(RTCORE_IOCTL_MAGIC, 72, int)
#define RTCORE_SPI_WRITE                    _IOWR(RTCORE_IOCTL_MAGIC, 73, int)
#define RTCORE_SPI_INIT                     _IOWR(RTCORE_IOCTL_MAGIC, 74, int)

#define RTCORE_I2C_INIT                     _IOWR(RTCORE_IOCTL_MAGIC, 75, int)
#define RTCORE_I2C_DEV_INIT                 _IOWR(RTCORE_IOCTL_MAGIC, 76, int)
#define RTCORE_I2C_READ                     _IOWR(RTCORE_IOCTL_MAGIC, 77, int)
#define RTCORE_I2C_WRITE                    _IOWR(RTCORE_IOCTL_MAGIC, 78, int)
#define RTCORE_REG_READ                     _IOWR(RTCORE_IOCTL_MAGIC, 79, int)
#define RTCORE_REG_WRITE                    _IOWR(RTCORE_IOCTL_MAGIC, 80, int)

#define RTCORE_MEM_SIZE_INFO                _IOWR(RTCORE_IOCTL_MAGIC, 81, int)
#define RTCORE_PCI_INFO_GET                 _IOWR(RTCORE_IOCTL_MAGIC, 90, int)

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

typedef enum rtcore_intr_type_e
{
    INTR_TYPE_NIC = 0,    /* 0 */
    INTR_TYPE_SWCORE,     /* 1 */
    INTR_TYPE_NTFY,
    INTR_TYPE_END
} rtcore_intr_type_t;


typedef struct rtk_uk_shared_s
{
    rt_timeval_t rt_tv1; /* for TC debug tool, to store time */

} rtk_uk_shared_t;

extern rtk_uk_shared_t *rtk_uk_sharedMem;

/*
 * Macro Declaration
 */

/*
 * Function Declaration
 */



#endif /* __RTCORE_DRV_H__ */

