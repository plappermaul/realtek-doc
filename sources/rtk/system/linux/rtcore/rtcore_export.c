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
 * Purpose : Realtek Switch SDK Core Module.
 *
 * Feature : Realtek Switch SDK Core Module
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <dev_config.h>
#include <common/debug/rt_log.h>
#include <common/debug/mem.h>
#include <common/util/rt_util_system.h>
#include <common/rt_chip.h>
#include <hwp/hw_profile.h>
#include <common/util/rt_util_time.h>
#include <common/util/rt_util_intr.h>
#include <rtcore/rtcore.h>
#include <common/rtcore/rtcore_init.h>
#include <osal/cache.h>
#include <osal/isr.h>
#include <osal/memory.h>
#include <osal/print.h>
#include <osal/time.h>
#include <osal/sem.h>
#include <osal/thread.h>
#include <osal/spl.h>
#include <osal/wait.h>
#include <osal/atomic.h>
#include <ioal/mem32.h>
#include <ioal/ioal_init.h>
#include <drv/watchdog/watchdog.h>
#include <private/drv/swcore/swcore.h>
#include <private/drv/swcore/chip_probe.h>
#if defined(CONFIG_SDK_DRIVER_NIC) && defined(CONFIG_SDK_DRIVER_NIC_KERNEL_MODE)
#include <drv/nic/nic.h>
#include <private/drv/nic/nic_diag.h>
#include <private/drv/nic/nic_rtl9300.h>
#endif
#if defined(CONFIG_SDK_UART1)
#include <drv/uart/uart.h>
#endif
#if defined(CONFIG_SDK_DRIVER_GPIO)
#include <drv/gpio/gpio.h>
#include <drv/gpio/generalCtrl_gpio.h>
#endif
#if defined(CONFIG_SDK_DRIVER_SPI)
#include <drv/spi/spi.h>
#include <private/drv/spi/spi_private.h>
#endif
#if defined(CONFIG_SDK_RTL8231)
#include <private/drv/rtl8231/rtl8231.h>
#include <drv/gpio/ext_gpio.h>
#endif /* CONFIG_SDK_RTL8231 */

#if defined(CONFIG_SDK_MODEL_MODE)
#include <ioal/ioal_init.h>
#include <virtualmac/vmac_target.h>
#endif /*end of #if defined(CONFIG_SDK_MODEL_MODE)*/

#if defined(CONFIG_SDK_DRIVER_L2NTFY)
#include <drv/l2ntfy/l2ntfy.h>
#include <private/drv/l2ntfy/l2ntfy_util.h>

#endif
#if defined(CONFIG_SDK_DRIVER_I2C)
#include <drv/i2c/i2c.h>
#endif
#include <drv/intr/intr.h>
#include <drv/tc/tc.h>
#include <common/util/rt_util_time.h>
#include <common/util/rt_util_intrk.h>

#include <hwp/hwp_util.h>
#include <ioal/phy_reset.h>
#include <ioal/ioal_log.h>
#include <ioal/ioal_param.h>

/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */


/*
 * Macro Declaration
 */

/*
 * Function Declaration
 */

/* BSP functions */
#if defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE)
  #if defined(CONFIG_SDK_DRIVER_GPIO)
  EXPORT_SYMBOL(drv_gpio_pin_init);
  EXPORT_SYMBOL(drv_gpio_dataBit_init);
  EXPORT_SYMBOL(drv_gpio_dataBit_get);
  EXPORT_SYMBOL(drv_gpio_dataBit_set);
  EXPORT_SYMBOL(drv_gpio_isr_get);
  EXPORT_SYMBOL(drv_gpio_isr_clear);
  EXPORT_SYMBOL(drv_gpio_intrHandler_register);
  EXPORT_SYMBOL(drv_gpio_intrHandler_unregister);
  EXPORT_SYMBOL(drv_generalCtrlGPIO_dev_init);
  EXPORT_SYMBOL(drv_generalCtrlGPIO_devEnable_set);
  EXPORT_SYMBOL(drv_generalCtrlGPIO_pin_init);
  EXPORT_SYMBOL(drv_generalCtrlGPIO_dataBit_set);
  EXPORT_SYMBOL(drv_generalCtrlGPIO_dataBit_get);
  EXPORT_SYMBOL(drv_generalCtrlGPIO_direction_set);
  EXPORT_SYMBOL(drv_generalCtrlGPIO_intrHandler_register);
  EXPORT_SYMBOL(drv_generalCtrlGPIO_intrHandler_unregister);
  #endif /* CONFIG_SDK_DRIVER_GPIO */
  #if defined(CONFIG_SDK_DRIVER_SPI)
  EXPORT_SYMBOL(drv_spiPin_init);
  EXPORT_SYMBOL(drv_spi_write);
  EXPORT_SYMBOL(drv_spi_read);
  #endif
  #if defined(CONFIG_SDK_DRIVER_I2C)
  EXPORT_SYMBOL(drv_i2c_init);
  EXPORT_SYMBOL(drv_i2c_dev_init);
  EXPORT_SYMBOL(drv_i2c_read);
  EXPORT_SYMBOL(drv_i2c_write);
  #endif
#endif //CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE
EXPORT_SYMBOL(rtk_dev);
EXPORT_SYMBOL(drv_watchdog_scale_set);
EXPORT_SYMBOL(drv_watchdog_scale_get);
EXPORT_SYMBOL(drv_watchdog_enable_set);
EXPORT_SYMBOL(drv_watchdog_enable_get);
EXPORT_SYMBOL(drv_watchdog_kick);
EXPORT_SYMBOL(drv_watchdog_threshold_set);
EXPORT_SYMBOL(drv_watchdog_threshold_get);

EXPORT_SYMBOL(debug_mem_read);
EXPORT_SYMBOL(debug_mem_write);
EXPORT_SYMBOL(debug_mem_show);
EXPORT_SYMBOL(rtk_uk_sharedMem);

EXPORT_SYMBOL(rt_log_init);
EXPORT_SYMBOL(rt_log);
EXPORT_SYMBOL(rt_log_reset);
EXPORT_SYMBOL(rt_log_enable_get);
EXPORT_SYMBOL(rt_log_enable_set);
EXPORT_SYMBOL(rt_log_level_get);
EXPORT_SYMBOL(rt_log_level_set);
EXPORT_SYMBOL(rt_log_level_reset);
EXPORT_SYMBOL(rt_log_mask_get);
EXPORT_SYMBOL(rt_log_mask_set);
EXPORT_SYMBOL(rt_log_mask_reset);
EXPORT_SYMBOL(rt_log_type_get);
EXPORT_SYMBOL(rt_log_type_set);
EXPORT_SYMBOL(rt_log_type_reset);
EXPORT_SYMBOL(rt_log_format_get);
EXPORT_SYMBOL(rt_log_format_set);
EXPORT_SYMBOL(rt_log_format_reset);
EXPORT_SYMBOL(rt_log_moduleMask_get);
EXPORT_SYMBOL(rt_log_moduleMask_set);
EXPORT_SYMBOL(rt_log_moduleMask_reset);
EXPORT_SYMBOL(rt_log_moduleName_get);
EXPORT_SYMBOL(rt_log_config_get);

#if defined(CONFIG_SDK_UART1)
EXPORT_SYMBOL(drv_uart_clearfifo);
EXPORT_SYMBOL(drv_uart_interface_set);
EXPORT_SYMBOL(drv_uart_getc);
EXPORT_SYMBOL(drv_uart_gets);
EXPORT_SYMBOL(drv_uart_putc);
EXPORT_SYMBOL(drv_uart_puts);
EXPORT_SYMBOL(drv_uart_baudrate_get);
EXPORT_SYMBOL(drv_uart_baudrate_set);
#endif

EXPORT_SYMBOL(osal_sem_create);
EXPORT_SYMBOL(osal_sem_destroy);
EXPORT_SYMBOL(osal_sem_take);
EXPORT_SYMBOL(osal_sem_give);
EXPORT_SYMBOL(osal_sem_mutex_create);
EXPORT_SYMBOL(osal_sem_mutex_destroy);
EXPORT_SYMBOL(osal_sem_mutex_take);
EXPORT_SYMBOL(osal_sem_mutex_give);

EXPORT_SYMBOL(osal_alloc);
EXPORT_SYMBOL(osal_free);

EXPORT_SYMBOL(osal_time_usec2Ticks_get);
EXPORT_SYMBOL(osal_time_seconds_get);
EXPORT_SYMBOL(osal_time_usecs_get);
EXPORT_SYMBOL(osal_time_udelay);
EXPORT_SYMBOL(osal_time_usleep);
EXPORT_SYMBOL(osal_time_sleep);
EXPORT_SYMBOL(osal_time_mdelay);

EXPORT_SYMBOL(osal_cache_memory_flush);

EXPORT_SYMBOL(osal_isr_register);
EXPORT_SYMBOL(osal_isr_unregister);
EXPORT_SYMBOL(osal_isr_disable_irq);
EXPORT_SYMBOL(osal_isr_enable_irq);
EXPORT_SYMBOL(osal_isr_disable_interrupt);
EXPORT_SYMBOL(osal_isr_enable_interrupt);
EXPORT_SYMBOL(osal_isr_disable_interrupt_save);
EXPORT_SYMBOL(osal_isr_enable_interrupt_restore);

EXPORT_SYMBOL(osal_thread_destroy);
EXPORT_SYMBOL(osal_thread_name);
EXPORT_SYMBOL(osal_thread_exit);
EXPORT_SYMBOL(osal_thread_create);
EXPORT_SYMBOL(osal_thread_self);

EXPORT_SYMBOL(osal_atomic_inc);
EXPORT_SYMBOL(osal_atomic_add_return);
EXPORT_SYMBOL(osal_atomic_set);
EXPORT_SYMBOL(osal_atomic_dec_return);
EXPORT_SYMBOL(osal_atomic_sub_return);
EXPORT_SYMBOL(osal_atomic_read);


EXPORT_SYMBOL(ioal_mem32_hraCb_register);
EXPORT_SYMBOL(ioal_mem32_check);
EXPORT_SYMBOL(ioal_mem32_read);
EXPORT_SYMBOL(ioal_mem32_write);
EXPORT_SYMBOL(ioal_mem32_field_read);
EXPORT_SYMBOL(ioal_mem32_field_write);
EXPORT_SYMBOL(ioal_init_unitID_change);
EXPORT_SYMBOL(ioal_debug_show);


EXPORT_SYMBOL(osal_spl_spin_unlock);
EXPORT_SYMBOL(osal_spl_spin_lock);
EXPORT_SYMBOL(osal_spl_spin_lock_create);
EXPORT_SYMBOL(osal_spl_spin_lock_destroy);
EXPORT_SYMBOL(osal_spl_spin_unlock_irqrestore);
EXPORT_SYMBOL(osal_spl_spin_lock_irqsave);

EXPORT_SYMBOL(osal_wait_event);
EXPORT_SYMBOL(osal_wake_up);
EXPORT_SYMBOL(osal_wait_module_create);
EXPORT_SYMBOL(osal_wait_module_destroy);
EXPORT_SYMBOL(osal_wait_module_list);

EXPORT_SYMBOL(drv_swcore_CPU_freq_get);
EXPORT_SYMBOL(drv_swcore_register_dump);
EXPORT_SYMBOL(drv_swcore_jtag_intf_get);
EXPORT_SYMBOL(drv_swcore_jtag_intf_set);
//EXPORT_SYMBOL(rtk_chip_type);


#if defined(CONFIG_SDK_DRIVER_NIC) && defined(CONFIG_SDK_DRIVER_NIC_KERNEL_MODE)
EXPORT_SYMBOL(drv_nic_init);
EXPORT_SYMBOL(drv_nic_pkt_tx);
EXPORT_SYMBOL(drv_nic_rx_start);
EXPORT_SYMBOL(drv_nic_rx_stop);
EXPORT_SYMBOL(drv_nic_rx_register);
EXPORT_SYMBOL(drv_nic_rx_unregister);
EXPORT_SYMBOL(drv_nic_dbg_get);
EXPORT_SYMBOL(drv_nic_dbg_set);
EXPORT_SYMBOL(drv_nic_cntr_clear);
EXPORT_SYMBOL(drv_nic_cntr_dump);
EXPORT_SYMBOL(drv_nic_ringbuf_dump);
EXPORT_SYMBOL(drv_nic_pktHdrMBuf_dump);
EXPORT_SYMBOL(drv_nic_rx_status_get);
EXPORT_SYMBOL(drv_nic_pkt_alloc);
EXPORT_SYMBOL(drv_nic_pkt_free);
EXPORT_SYMBOL(drv_nic_reset);
EXPORT_SYMBOL(drv_nic_tag_set);
EXPORT_SYMBOL(drv_nic_txData_set);
EXPORT_SYMBOL(drv_nic_diagPkt_send);
#endif /* CONFIG_SDK_DRIVER_NIC && CONFIG_SDK_DRIVER_NIC_KERNEL_MODE */

#if defined(CONFIG_SDK_DRIVER_L2NTFY) && defined(CONFIG_SDK_DRIVER_NIC_KERNEL_MODE)
EXPORT_SYMBOL(drv_l2ntfy_init);
EXPORT_SYMBOL(drv_l2ntfy_register);
EXPORT_SYMBOL(drv_l2ntfy_unregister);
EXPORT_SYMBOL(drv_l2ntfy_pkt_handler);
EXPORT_SYMBOL(drv_l2ntfy_enable_get);
EXPORT_SYMBOL(drv_l2ntfy_enable_set);
EXPORT_SYMBOL(drv_l2ntfy_backPressureThresh_get);
EXPORT_SYMBOL(drv_l2ntfy_backPressureThresh_set);
EXPORT_SYMBOL(drv_l2ntfy_notificationEventEnable_get);
EXPORT_SYMBOL(drv_l2ntfy_notificationEventEnable_set);
EXPORT_SYMBOL(drv_l2ntfy_counter_dump);
EXPORT_SYMBOL(drv_l2ntfy_counter_clear);
EXPORT_SYMBOL(drv_l2ntfy_sizeInfo_get);
EXPORT_SYMBOL(drv_l2ntfy_debug_get);
EXPORT_SYMBOL(drv_l2ntfy_event_dump);
EXPORT_SYMBOL(drv_l2ntfy_debug_set);
EXPORT_SYMBOL(drv_l2ntfy_dst_get);
EXPORT_SYMBOL(drv_l2ntfy_dst_set);
EXPORT_SYMBOL(drv_l2ntfy_reset);
EXPORT_SYMBOL(drv_l2ntfy_iTag_get);
EXPORT_SYMBOL(drv_l2ntfy_iTag_set);
EXPORT_SYMBOL(drv_l2ntfy_magicNum_get);
EXPORT_SYMBOL(drv_l2ntfy_magicNum_set);
EXPORT_SYMBOL(drv_l2ntfy_macAddr_get);
EXPORT_SYMBOL(drv_l2ntfy_macAddr_set);
EXPORT_SYMBOL(drv_l2ntfy_maxEvent_get);
EXPORT_SYMBOL(drv_l2ntfy_maxEvent_set);
EXPORT_SYMBOL(drv_l2ntfy_timeout_get);
EXPORT_SYMBOL(drv_l2ntfy_timeout_set);
#if defined(CONFIG_SDK_RTL9300)
extern int32 r9300_l2ntfy_l2_cb_register(uint32 unit, drv_l2ntfy_portNewMacOp_cb_f portNewMacOp_cb, drv_l2ntfy_agingTime_cb_f agingTime_cb);
EXPORT_SYMBOL(r9300_l2ntfy_l2_cb_register);
#endif
#if defined(CONFIG_SDK_RTL9310)
extern int32 r9310_l2ntfy_l2_cb_register(uint32 unit, drv_l2ntfy_portNewMacOp_cb_f portNewMacOp_cb, drv_l2ntfy_agingTime_cb_f agingTime_cb);
EXPORT_SYMBOL(r9310_l2ntfy_l2_cb_register);
#endif
#endif

#if defined(CONFIG_SDK_RTL8231)
EXPORT_SYMBOL(drv_rtl8231_i2c_read);
EXPORT_SYMBOL(drv_rtl8231_i2c_write);
EXPORT_SYMBOL(drv_rtl8231_mdc_read);
EXPORT_SYMBOL(drv_rtl8231_mdc_write);
EXPORT_SYMBOL(drv_rtl8231_mdcSem_register);
EXPORT_SYMBOL(drv_rtl8231_directAccess_set);
EXPORT_SYMBOL(drv_extGpio_dataBit_get);
EXPORT_SYMBOL(drv_extGpio_dataBit_set);
EXPORT_SYMBOL(drv_extGpio_sync_start);
EXPORT_SYMBOL(drv_extGpio_syncEnable_get);
EXPORT_SYMBOL(drv_extGpio_devEnable_set);
EXPORT_SYMBOL(drv_extGpio_pin_get);
EXPORT_SYMBOL(drv_extGpio_pin_init);
EXPORT_SYMBOL(drv_extGpio_devReady_get);
EXPORT_SYMBOL(drv_extGpio_devEnable_get);
EXPORT_SYMBOL(drv_extGpio_syncStatus_get);
EXPORT_SYMBOL(drv_extGpio_syncEnable_set);
EXPORT_SYMBOL(drv_extGpio_dev_get);
EXPORT_SYMBOL(drv_extGpio_dev_init);
EXPORT_SYMBOL(drv_extGpio_devRecovery_start);
EXPORT_SYMBOL(drv_extGpio_reg_read);
EXPORT_SYMBOL(drv_extGpio_reg_write);
EXPORT_SYMBOL(drv_extGpio_direction_get);
EXPORT_SYMBOL(drv_extGpio_direction_set);
EXPORT_SYMBOL(drv_extGpio_i2c_init);
EXPORT_SYMBOL(drv_extGpio_i2c_read);
EXPORT_SYMBOL(drv_extGpio_i2c_write);
#endif /* CONFIG_SDK_RTL8231 */

#if defined(CONFIG_SDK_MODEL_MODE)
#ifdef CONFIG_SDK_MODEL_MODE_USER
extern int8 *pVirtualSWTable;
EXPORT_SYMBOL(pVirtualSWTable);
#endif  /* CONFIG_SDK_MODEL_MODE_USER */
EXPORT_SYMBOL(ioal_init);
EXPORT_SYMBOL(vmac_setCaredICType);
EXPORT_SYMBOL(vmac_getTarget);
EXPORT_SYMBOL(vmac_setTarget);
EXPORT_SYMBOL(vmac_setRegAccessType);
EXPORT_SYMBOL(vmac_getRegAccessType);
#endif /*end of #if defined(CONFIG_SDK_MODEL_MODE) */

EXPORT_SYMBOL(portDescpEmpty);
EXPORT_SYMBOL(serdesDescpEmpty);
EXPORT_SYMBOL(swDescpEmpty);
EXPORT_SYMBOL(hwpEmpty);
EXPORT_SYMBOL(unitInfoEmpty);
EXPORT_SYMBOL(parsedInfoEmpty);
EXPORT_SYMBOL(hwp_myHwProfile);
EXPORT_SYMBOL(unitMapStruct);
EXPORT_SYMBOL(hwp_unit_show);
EXPORT_SYMBOL(hwp_parsedInfo_show);
EXPORT_SYMBOL(hwp_info_show);
EXPORT_SYMBOL(hwp_unit_get_next);
EXPORT_SYMBOL(hwp_portmaskValid_Chk);
EXPORT_SYMBOL(hwp_portmaskAttriValid_Chk);
EXPORT_SYMBOL(hwp_profile_find);
EXPORT_SYMBOL(hwp_identifier_get);
EXPORT_SYMBOL(hwp_get_port_by_baseport_offset);
EXPORT_SYMBOL(hwp_get_offset_by_baseport_port);
#ifdef RTK_HWP_DEFAULT_PROFILE
EXPORT_SYMBOL(hwp_defaultProfilePort_update);
EXPORT_SYMBOL(hwp_defaultProfilePort_build);
EXPORT_SYMBOL(hwp_defaultProfilePhy_build);
#endif /* RTK_HWP_DEFAULT_PROFILE */
EXPORT_SYMBOL(hwp_profilePhy_del);
EXPORT_SYMBOL(hwp_profilePhy_update);
EXPORT_SYMBOL(hwp_useDefHwp);
EXPORT_SYMBOL(hwp_parsedInfo_buildup);
EXPORT_SYMBOL(hwp_chipInfo_update);
EXPORT_SYMBOL(hwp_multiSdsPortLaneNum_get);
EXPORT_SYMBOL(hwp_ExternalCpu);
EXPORT_SYMBOL(hwp_unit_attach);
EXPORT_SYMBOL(hwp_unit_detach);





#if defined(CONFIG_SDK_TC_DRV)
EXPORT_SYMBOL(drv_tc_enable_set);
EXPORT_SYMBOL(drv_tc_mode_set);
EXPORT_SYMBOL(drv_tc_divFactor_set);
EXPORT_SYMBOL(drv_tc_dataInitValue_set);
EXPORT_SYMBOL(drv_tc_intEnable_set);
EXPORT_SYMBOL(drv_tc_intState_get);
EXPORT_SYMBOL(drv_tc_intState_clear);
EXPORT_SYMBOL(drv_tc_counterValue_get);
#endif

EXPORT_SYMBOL(rt_util_sysMac_get);
EXPORT_SYMBOL(rt_util_ledInitFlag_get);


#if defined(CONFIG_SDK_TC_TC1_TIME)
EXPORT_SYMBOL(rt_util_hpt_init);
EXPORT_SYMBOL(rt_util_hpt_get);
EXPORT_SYMBOL(rt_util_hpt_record);
EXPORT_SYMBOL(rt_util_hpt_print);
EXPORT_SYMBOL(rt_util_hptRecord_print);
EXPORT_SYMBOL(rt_util_hpt_udelay);

#endif

EXPORT_SYMBOL(drv_intr_isrSts_get);
EXPORT_SYMBOL(drv_intr_swcoreImrExtGpioPinEnable_set);
EXPORT_SYMBOL(drv_intr_imrEnable_set);
EXPORT_SYMBOL(drv_intr_swcoreSts_get);

EXPORT_SYMBOL(rt_util_intr_isr_register);
EXPORT_SYMBOL(rt_util_intr_isr_unregister);
EXPORT_SYMBOL(rt_util_intrk_swcore_handler);

EXPORT_SYMBOL(ioal_phy_reset);
EXPORT_SYMBOL(ioal_log_bootMsgLevel_get);
EXPORT_SYMBOL(log_initErrEn);
EXPORT_SYMBOL(log_initMsgEn);
EXPORT_SYMBOL(rt_log_bootMsgEnable_set);
EXPORT_SYMBOL(ioal_param_hwpIdKey_get);
EXPORT_SYMBOL(ioal_param_ledInitFlag_get);
EXPORT_SYMBOL(ioal_param_sysMac_get);
EXPORT_SYMBOL(ioal_param_phyXfmrRj45Impd_get);

EXPORT_SYMBOL(rt_chip_familyIndex_get);
EXPORT_SYMBOL(rt_util_flashEnv_get);
EXPORT_SYMBOL(rt_util_flashVar_get);

EXPORT_SYMBOL(rtk_unit2devID);
EXPORT_SYMBOL(rtk_dev2unitID);




