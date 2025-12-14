#ifndef __EUROPA_H__
#define __EUROPA_H__

#define EUROPA_DRIVER_VERSION 0x000207 /* 0.2.7 */

#define EUROPA_DRIVER_VERSION_MAJOR ((EUROPA_DRIVER_VERSION&0xFF0000)>>16)
#define EUROPA_DRIVER_VERSION_MINOR ((EUROPA_DRIVER_VERSION&0x00FF00)>>8)
#define EUROPA_DRIVER_VERSION_PATCH (EUROPA_DRIVER_VERSION&0x0000FF)


#include <common/type.h>
#include "europa_reg_definition.h"

#define EUROPA_EXTERNAL_EEPROM_RW_TEST   0
#define EUROPA_EXTERNAL_EEPROM_SIZE   0x2000
#define EUROPA_UART_DISPLAY              0

//DDMI A2 address 128
//#define EUROPA_LOOP_MODE_REG                    0x180
//DDMI A2 address 129
#define EUROPA_DEBUG_REG                            0x181
#define EUROPA_DEBUG_LUT_MASK                   0x1
#define EUROPA_DEBUG_DDMI_MASK                 0x2
#define EUROPA_DEBUG_INTERRUPT_MASK        0x4

#define EUROPA_DELAY_REG                            0x182

#if 0  // tempest -- move to include/module/europa.h
#define EUROPA_DDMI_A0_ADDR                     0
#define EUROPA_DDMI_A2_ADDR                     256
#define EUROPA_A4_PATCH_LEN_ADDR           512
#define EUROPA_PATCH_A4_ADDR                   513
#define EUROPA_DCL_P0_ADDR                        1346
#define EUROPA_DCL_P1_ADDR                        1347
#define EUROPA_DCL_PAVG_ADDR                    1348

#define EUROPA_FLASH_DATA_SIZE                   2000
#endif

// tempest +
#define EUROPA_PARAMETER_LOCATION       "/var/config/europa.data"
#define EUROPA_PARAMETER_SIZE           1600
#define EUROPA_FLASH_DATA_SIZE          2000
// FIXME
// FIXME what is flash data/parameter size ??  1000 ? 1600 ? 2000 ? 0x1000 ?
// FIXME 
// FIXME @sdk/include/module/europa/europa.h
// FIXME  #define EUROPA_FLASH_DATA_SIZE                   2000
// FIXME 
// FIXME @sdk/src/app/diag_rtk/src/diag_debug.c
// FIXME  #define EUROPA_PARAMETER_SIZE                    1600
// FIXME 
// FIXME @sdk/src/app/europa/src/europa_cli.c
// FIXME  #define EUROPA_PARAMETER_SIZE                    0x1000
// FIXME 
// FIXME @sdk/src/module/europa_drv/init.c

#define EUROPA_DDMI_A0_ADDR                                      0
#define EUROPA_DDMI_A2_ADDR                                      0x100
#define EUROPA_A4_ADDR                                              0x200
#define EUROPA_A4_ADDR_BASE                                   (EUROPA_A4_ADDR+1)

// FIXME  duplicate definition....
#define EUROPA_A4_PATCH_LEN_ADDR           512
#define EUROPA_PATCH_A4_ADDR                   513

#define EUROPA_APD_LUT_ADDR                                    0x300
#define EUROPA_LASER_LUT_ADDR                                 0x400
#define EUROPA_DCL_P0_ADDR                                       1346
#define EUROPA_DCL_P1_ADDR                                       1347
#define EUROPA_DCL_PAVG_ADDR                                  1348
#define EUROPA_W57_APCDIG_DAC_BIAS_ADDR           1349
#define EUROPA_RX_A_ADDR                                          1350
#define EUROPA_RX_B_ADDR                                          1354
#define EUROPA_RX_C_ADDR                                          1358
#define EUROPA_RSSI_V0_ADDR                                     1362
#define EUROPA_MPD0_ADDR                                          1366
#define EUROPA_CROSS_ADDR					1370
#define EUROPA_TX_A_ADDR                                          1372
#define EUROPA_TX_B_ADDR                                          1376
#define EUROPA_TX_C_ADDR                                          1380
#define EUROPA_TEMP_OFFSET_ADDR                                   1384
#define EUROPA_TEMP_INTR_SCALE_ADDR                               1385
#define EUROPA_RX_TH_ADDR                                         1386
#define EUROPA_RX_DETH_ADDR                                       1387
// tempest -




#define REG_DDMI_EXTERNAL_EEPROM_BASE_ADDR 0x2


typedef enum europa_laser_fun_e
{
    EUROPA_LASER_FUN_TX,
    EUROPA_LASER_FUN_VDD_LDX,
    EUROPA_LASER_FUN_BOOSTER,
    EUROPA_LASER_FUN_RX,
    EUROPA_LASER_FUN_RSSI,
    EUROPA_LASER_FUN_SDADC,
    EUROPA_LASER_FUN_DAC,
    EUROPA_LASER_FUN_APC,
    EUROPA_LASER_FUN_END
} europa_laser_fun_t;



typedef enum europa_laser_fault_act_e
{
    EUROPA_FAULT_INTR_8051_ONLY= 0,
    EUROPA_FAULT_INTR_8051_TX_VDD_LDX = 1,
    EUROPA_FAULT_INTR_8051_TX_VDD_LDX_BOOSTER = 2,
    EUROPA_FAULT_INTR_8051_TX_VDD_LDX_BOOSTER_RSSI_SDADC_DAC = 3,
    EUROPA_FAULT_INTR_END
} europa_laser_fault_act_t;

typedef enum europa_laser_tx_dis_ctl_fault_act_e
{
    EUROPA_UNDER_TX_FAULT_INTR_TX_APC= 0,
    EUROPA_UNDER_TX_FAULT_INTR_TX_ONLY = 1,
    EUROPA_UNDER_TX_FAULT_INTR_APC_ONLY = 2,
    EUROPA_UNDER_TX_FAULT_INTR_NOP = 3,
    EUROPA_UNDER_TX_FAULT_INTR_END
} europa_laser_tx_dis_ctl_fault_act_t;

typedef enum europa_laser_rx_over_fault_act_e
{
    EUROPA_RX_OVER_FAULT_INTR_BOOSTER= 1,
    EUROPA_RX_OVER_FAULT_INTR_BOOSTER_RX = 2,
    EUROPA_RX_OVER_FAULT_INTR_END
} europa_laser_rx_over_fault_act_t;


typedef enum apc_loop_mode_e
{
    APC_DOL_MODE=0,
    APC_DCL_MODE,
    APC_SCL_BIAS_MODE,
    APC_SCL_MOD_MODE,
    APC_MODE_END,
}apc_loop_mode_t;


typedef enum tx_enable_fun_e
{
    TX_EN_BIAS_MOD_PROTECT,
    TX_EN_ENABLE_TX,
    TX_EN_BIAS_MOD_LUT,
    TX_EN_P0_P1_PAV,
    TX_EN_LOOP_MODE,    
    TX_EN_BURST_MODE_CTRL,
    TX_EN_WAIT_500MS,
    TX_EN_SET_W74,
    TX_EN_FAULT_ENABLE,
    TX_EN_END
} tx_enable_fun_t;

typedef enum apc_enable_fun_e
{
    APC_EN_MCU_READY,
    APC_EN_CHECK_READY,
    APC_EN_TX_SD_THRESHOLD,
    APC_EN_APC_BIAS_POWER_ON,
    APC_EN_APC_POWER_SET,   
    APC_EN_APC_DIGITAL_POWER_ON,    
    APC_EN_R_PROCESS_DETECT,
    APC_EN_TX_RPD_RCODE_OUT,    
    APC_EN_OFFSET_CALIBR,    
    APC_EN_MEASURE_MPD0,
    APC_EN_WAIT_CALIBR,
    APC_EN_MOVE_CALIBR_RESULT,
    APC_EN_END
} apc_enable_fun_t;

typedef enum rx_enable_fun_e
{
    RX_ENABLE_BOOSTER,
    RX_ENABLE_RX,
    RX_ENABLE_RSSI,
    RX_ENABLE_DAC,
    RX_ENABLE_END
} rx_enable_fun_t;

typedef enum tx_voltage_e
{
    SDADC_TX_VDD,
    SDADC_APC_VDD,
    SDADC_VDD_END
} tx_voltage_t;

typedef struct europa_volt_para_s
{
    uint32 rssi_code;
    uint32 vdd_code;
    uint32 gnd_code;
    uint32 ldo_code;
    uint32 half_vdd_code;
    uint32 volt_half_vdd;
    uint32 volt_ldo;
    uint32 volt_rssi;
} europa_volt_para_t;

#endif /* End of __EUROPA_H__ */

