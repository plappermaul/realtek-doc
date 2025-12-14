
#ifndef __98X_PHERIPHERAL_H__
#define __98X_PHERIPHERAL_H__

#define	int_dma_uart0	27
#define int_dma_uart1	28
#define	int_ssi0	29
#define int_ssi1	30
#define int_gdma	31
#define	int_iis		32
#define rtc_alarm_int	54
#define	ipsec_irq	57


#if 0//defined(RTL8196F_SUPPORT)
// SPI 
#include "../../../base/spi/dw_spi_base.h"
#include "../../../base/spi/dw_spi_base_test.h"
#include "../spi/dw_hal_spi.h"

// GDMA 
#include "../../../base/gdma/dw_gdma_base.h"
#include "../gdma/dw_hal_gdma.h"

// I2C 
#include "../../../base/i2c_rtk/rtk_i2c_base.h"
#include "../../../base/i2c_rtk/rtk_i2c_base_test.h"
#include "../i2c_rtk/rtk_hal_i2c.h"

#include "../../../base/i2c_dw/dw_i2c_base.h"
#include "../../../base/i2c_dw/dw_i2c_base_test.h"
#include "../i2c_dw/dw_hal_i2c.h"

// UART 
#include "../../../base/uart/rtk_uart_base.h"
#include "../uart/rtk_hal_uart.h"

// PCIE
#include "../../../base/pcie/dw_pcie_base.h"
#include "../pcie/dw_hal_pcie.h"

// I2S
#include "../../../base/i2s/rtk_i2s_base.h"
#include "../i2s/rtk_hal_i2s.h"

// PCM
#include "../../../base/pcm/rtk_pcm_base.h"
#include "../pcm/rtk_hal_pcm.h"

// WLAN
//#include "../../../base/wlan/wl_common_hw.h"
//#include "../wlan/rtk_hal_wlan.h"
//#include "../../../base/wlan/wl_common_ctrl.h"
//#include "../../../base/wlan/rtk_wlan_base.h"
//#include "../../../base/wlan/rtk_wlan_base_test.h"
//#include "../../../base/wlan/wl_log.h"
//#include "../../../base/wlan/HalPwrSeqCmd.h"
//#include "../../../base/wlan/wl_fw_verify.h"
//#include "../wlan/Hal8196FPwrSeq.h"
//#include "../wlan/rtk_wlan_extend.h"


// PWM
#include "../../../base/pwm/rtk_pwm_base.h"
#include "../../../base/pwm/rtk_pwm_base_test.h"
#include "../pwm/rtk_hal_pwm.h"

//Timer
#include "../../../base/timer/dw_timer_base.h"
#include "../../../base/timer/dw_timer_base_test.h"
#include "../timer/dw_hal_timer.h"

#elif 1 //defined(RTL8198F_SUPPORT)
// SPI
#include "../../../base/spi/dw_4_02a/dw_spi_base.h"
#include "../../../base/spi/dw_4_02a/dw_spi_base_test.h"
#include "../spi/dw_hal_spi.h"
#include "../../../extend/98X/spi/dw_spi_extend_test.h"

// GDMA
#include "../../../base/gdma/dw_2_22a/dw_gdma_base.h"
#include "../gdma/dw_hal_gdma.h"
#include "../../../extend/98X/gdma/dw_gdma_extend_test.h"

// I2C
//#include "../../../base/i2c_rtk/rtk_i2c_base.h"
//#include "../../../base/i2c_rtk/rtk_i2c_base_test.h"
//#include "../i2c_rtk/rtk_hal_i2c.h"

//#include "../../../base/i2c_dw/dw_1_14a/dw_i2c_base.h"
//#include "../../../base/i2c_dw/dw_1_14a/dw_i2c_base_test.h"
//#include "../i2c_dw/dw_hal_i2c.h"

// UART
#include "../../../base/uart/rtk_v1_0/rtk_uart_base.h"
#include "../uart/rtk_hal_uart.h"

// PCIE
//#include "../../../base/pcie/dw_4_60a/dw_pcie_base.h"
//#include "../pcie/dw_hal_pcie.h"

// I2S
#include "../../../base/i2s/rtk_v1_1/rtk_i2s_base.h"
#include "../i2s/rtk_hal_i2s.h"

// PCM
//#include "../../../base/pcm/rtk_v1_0/rtk_pcm_base.h"
//#include "../pcm/rtk_hal_pcm.h"

// WLAN

//#include "../../../base/wlan/rtk_98f/wl_common_hw.h"
//#include "../../../base/wlan/rtk_98f/HalPwrSeqCmd.h"
//#include "../wlan/Hal8198FPwrSeq.h"
#ifdef CONFIG_CMD_RTK_WLAN_MP_8198F_8814A
#include "../../8814A/wlan/Hal8814APwrSeq.h"
#endif
//#include "../wlan/rtk_hal_wlan.h"
//#include "../../../base/wlan/rtk_98f/wl_common_ctrl.h"
//#include "../../../base/wlan/rtk_98f/rtk_wlan_base.h"
//#include "../../../base/wlan/rtk_98f/rtk_wlan_base_test.h"
//#include "../../../base/wlan/rtk_98f/wl_log.h"

//#include "../../../base/wlan/rtk_98f/wl_fw_verify.h"
//#include "../wlan/rtk_wlan_SWPS_test.h"
//#include "../wlan/rtk_wlan_extend.h"
//#include "../wlan/rtk_wlan_extend_reg.h"
//#include "../wlan/rtk_wlan_extend_bit.h"
#ifdef CONFIG_CMD_RTK_WLAN_MP_8198F_8814A
#include "../../../base/wlan/rtk_8814A/wl_fw_verify.h"
#include "../../8814A/wlan/rtk_wlan_SWPS_test.h"
#include "../../8814A/wlan/rtk_wlan_extend.h"
#include "../../8814A/wlan/rtk_wlan_extend_reg.h"
#include "../../8814A/wlan/rtk_wlan_extend_bit.h"
#endif

// PWM
//#include "../../../base/pwm/rtk_v1_0/rtk_pwm_base.h"
//#include "../../../base/pwm/rtk_v1_0/rtk_pwm_base_test.h"
//#include "../pwm/rtk_hal_pwm.h"

//Timer
//#include "../../../base/timer/dw_v2_05a/dw_timer_base.h"
//#include "../../../base/timer/dw_v2_05a/dw_timer_base_test.h"
//#include "../timer/dw_hal_timer.h"


//RTC
#include "../../../base/rtc/ca_v1_0/ca_rtc_base.h"
#include "../../../base/rtc/ca_v1_0/ca_rtc_base_test.h"
#include "../rtc/ca_hal_rtc.h"

//MISC
//#include "../../../base/misc/misc_v1_0/misc_base.h"
//#include "../../../base/misc/misc_v1_0/misc_base_test.h"
//#include "../misc/hal_misc.h"

//NE
#ifdef CONFIG_CMD_RTK_NE_8198F

#if 1//TBD: fix w/ .h file
typedef unsigned long long      ca_uint64_t;
#define CA_ETH_ADDR_LEN     6
typedef unsigned char           ca_uint8_t;
typedef ca_uint8_t ca_mac_addr_t[CA_ETH_ADDR_LEN];
typedef unsigned short          ca_uint16_t;
typedef ca_uint16_t             ca_device_id_t;
#else
#include "../../../base/ne/ca_v1_0/ne_base.h"
#endif

//#include "../ne/aal_common.h"
#include "../ne/aal_init.h"
#include "../ne/aal_ni_l2.h"
//#include "../ne/aal_arb.h"
//#include "../ne/g3lite_registers.h"
//#include "../ne/aal_l2_tm.h"
//#include "../ne/ne_hw_init.h"
//#include "../ne/osal_cmn.h"
//#include "../ne/osal_spinlock.h"
//#include "../ne/scfg.h"
//#include "../ne/util_misc.h"
#endif/* CONFIG_CMD_RTK_NE_8198F */

#endif

#endif //#ifndef __98X_PHERIPHERAL_H__
