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
 * $Revision: 42664 $
 * $Date: 2013-09-09 11:17:16 +0800 (Mon, 09 Sep 2013) $
 *
 * Purpose : Definition of LDD API
 *
 * Feature : Provide the APIs to enable and configure LDD
 *
 */

#ifndef __RTK_LDD_API_H__
#define __RTK_LDD_API_H__


/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/i2c.h>


/*
 * Symbol Definition
 */

#define LDD_DRIVER_VERSION_MAJOR(V) ((V&0xFF0000)>>16)
#define LDD_DRIVER_VERSION_MINOR(V) ((V&0x00FF00)>>8)
#define LDD_DRIVER_VERSION_PATCH(V) (V&0x0000FF)

/*
 * Data Declaration
 */
typedef enum
{
    LDD_RESET_CHIP       = 0,
    LDD_RESET_DIGITAL    = 1,
    LDD_RESET_8051       = 2,
    LDD_RESET_ANALOG     = 3,
    LDD_RESET_END
} rtk_ldd_reset_mode_t;

typedef enum
{
    LDD_LOOP_DOL         = 0,
    LDD_LOOP_DCL         = 1,
    LDD_LOOP_SCL_BIAS    = 2,
    LDD_LOOP_SCL_MOD     = 3,
    LDD_LOOP_MODE_END
} rtk_ldd_loop_mode_t;

//000: DOL, 001: APC_B, 010: APC_M, 011:APC and ERC timing division
//100: DOL, 101: SCL_B, 110: ERC only, 111: APC and ERC simultaneously
typedef enum
{
    LDD_LOOP2_DOL          = 0,
    LDD_LOOP2_APC_B        = 1,
    LDD_LOOP2_APC_M        = 2,
    LDD_LOOP2_DCL          = 3,
    LDD_LOOP2_SCL_B        = 5,
    LDD_LOOP2_ERC          = 6,
    LDD_LOOP2_APC_ERC_S    = 7,
    LDD_LOOP2_MODE_END
} rtk_ldd_loop_mode2_t;

typedef enum
{
    LDD_APC_MCU_READY,
    LDD_APC_CHECK_READY,
    LDD_APC_TX_SD_THRESHOLD,
    LDD_APC_BIAS_POWER_ON,
    LDD_APC_POWER_SET,
    LDD_APC_DIGITAL_POWER_ON,
    LDD_APC_R_PROCESS_DETECT,
    LDD_APC_TX_RPD_RCODE_OUT,
    LDD_APC_OFFSET_CALIBR,
    LDD_APC_MEASURE_MPD0,
    LDD_APC_WAIT_CALIBR,
    LDD_APC_MOVE_CALIBR_RESULT,
    LDD_APC_FUNC_END
} rtk_ldd_apc_func_t;

typedef enum
{
    LDD_TX_BIAS_MOD_PROTECT,
    LDD_TX_ENABLE_TX,
    LDD_TX_BIAS_MOD_LUT,
    LDD_TX_P0_P1_PAV,
    LDD_TX_LOOP_MODE,
    LDD_TX_BURST_MODE_CTRL,
    LDD_TX_WAIT_500MS,
    LDD_TX_FAULT_ENABLE,
    LDD_TX_ENABLE_TX_SD,
    LDD_TX_FUNC_END
} rtk_ldd_tx_func_t;

typedef enum
{
    LDD_RX_BOOSTER,
    LDD_RX_RX,
    LDD_RX_RSSI,
    LDD_RX_TX_SD_PIN,
    LDD_RX_DAC,
    LDD_RX_THRESHOLD,
    LDD_RX_ENABLE_RX_SD,
    LDD_RX_FUNC_END
} rtk_ldd_rx_func_t;

typedef enum rtk_ldd_txes_mode_s
{
    LDD_TXES_VG_MODE = 0,
    LDD_TXES_VG_STR,
    LDD_TXES_EMP_LEVEL,
    LDD_TXES_VG_AMP,
    LDD_TXES_DELY_RISING,
    LDD_TXES_DELY_FALLING,
    LDD_TXES_ADJ_ENABLE,    
    LDD_TXES_TEST_MODE,   
    LDD_TXES_MODE_END
} rtk_ldd_txes_mode_t;

typedef struct
{
    uint32  rssi_v0;
    uint32  rssi_voltage;
    uint32  rssi_i;
    uint32  rssi_v0_2;
    uint32  rssi_voltage_2;
    uint32  rssi_i_2;
    uint32  mpd0;
    uint32  mpd1;
    uint32  mpd_i;
    uint32  bias;
    uint32  modulation;
    uint32  temperature;
    uint32  vdd;
    int32   rx_a;
    int32   rx_b;
    int32   rx_c;
    int32   tx_a;
    int32   tx_b;
    int32   tx_c;
    int8    temp_offset;   /* temperature offset of Tj and Tbosa */
    uint8   temp_intr_scale; /* granuality of temperature interrupt 
                   (for Vapd temperature compensation) */
    uint8   rx_th;           /* RX SD/LOS assert threshold */
    uint8   rx_deth;         /* RX SD/LOS de-assert threshold */
} rtk_ldd_param_t;

typedef struct  rtk_ldd_rxpwr_s
{
    uint32  op_mode;
    uint32  rssi_code1;
    uint32  rssi_code2;     
    uint32  rssi_code3;
    uint32  rx_lsb;
    uint32  rx_code_200u;
    uint32  rx_r6400;
    uint32  i_rssi;    
    uint32  rx_power;        
    int32   rx_a;
    int32   rx_b;
    int32   rx_c; 
    int32   data_a;
    int32   data_b;
    int32   data_c;      
}  rtk_ldd_rxpwr_t;

typedef struct  rtk_ldd_txpwr_s
{
    uint32  op_mode;
    uint32  rssi_code1;
    uint32  rssi_code2;     
    uint32  i_cal;
    uint32  tx_lsb;
    uint32  tx_code_250u;
    uint32  tx_r_mpd;
    uint32  i_mpd;    
    uint32  tx_power;        
    int32   tx_a;
    int32   tx_b;
    int32   tx_c; 
    int32   data_a;
    int32   data_b;
    int32   data_c;      
}  rtk_ldd_txpwr_t;


typedef struct 
{
    uint32                  devId;
    uint32                  rssi_voltage;
    uint32                  rssi_voltage2;
    uint32                  rssi_v0;
    uint32                  vdd_code;
    uint32                  gnd_code;
    uint32                  half_vdd_code;
    uint32                  rssi_code;
    rtk_i2c_port_t          i2cPort;
    uint32                  rset_code;
    uint32                  ldo_code;
    uint8                   *flash_data;
    rtk_ldd_reset_mode_t    reset;
    uint32                  adc_gnd_code;
    uint16                  count;
    uint32                  rx_ldo_code;
    rtk_ldd_apc_func_t      apc_func;
    rtk_ldd_loop_mode_t     loop_mode;
    rtk_ldd_tx_func_t       tx_func;
    rtk_ldd_rx_func_t       rx_func;
    uint16                  temperature;
    uint32                  tx_bias;
    uint32                  tx_mod;
    rtk_enable_t            state;
    uint32                  driver_version;
    rtk_ldd_rxpwr_t         rxp;    
    rtk_ldd_txpwr_t         txp;
} rtk_ldd_cfg_t;


typedef struct
{
    int32   (*ldd_i2c_init)(rtk_i2c_port_t port);
    int32   (*ldd_gpio_init)(void);
    int32   (*ldd_reset)(rtk_ldd_reset_mode_t mode);
    int32   (*ldd_calibration_state_set)(rtk_enable_t state);
    int32   (*ldd_parameter_set)(uint32 length, uint32 offset, uint8 *flash_data);
    int32   (*ldd_parameter_get)(uint32 length, uint32 offset, uint8 *flash_data);
    int32   (*ldd_loop_mode_set)(rtk_ldd_loop_mode_t mode);
    int32   (*ldd_laser_lut_set)(rtk_ldd_loop_mode_t mode, uint8 *lut_data);
    int32   (*ldd_apd_lut_set)(uint8 *lut_data);
    int32   (*ldd_apc_enable_set)(rtk_ldd_apc_func_t func, rtk_ldd_loop_mode_t mode);
    int32   (*ldd_tx_enable_set)(rtk_ldd_tx_func_t func, rtk_ldd_loop_mode_t mode, uint8 *lut_data);
    int32   (*ldd_rx_enable_set)(rtk_ldd_rx_func_t func);
    int32   (*ldd_power_on_get)(uint32 *result);
    int32   (*ldd_tx_power_get)(uint32 mpd0, uint32 *v_mpd, uint32 *i_mpd);
    int32   (*ldd_rx_power_get)(uint32 rssi_v0, uint32 *v_rssi, uint32 *i_rssi);
    int32   (*ldd_rssi_voltage_get)(rtk_ldd_cfg_t *ldd_cfg);
    int32   (*ldd_rssi_v0_get)(uint16 count, uint32 *rssi_v0);
    int32   (*ldd_vdd_get)(uint32 *vdd);
    int32   (*ldd_mpd0_get)(uint16 count, uint32 *value);
    int32   (*ldd_temperature_get)(uint16 *temp);
    int32   (*ldd_tx_bias_get)(uint32 *bias);
    int32   (*ldd_tx_mod_get)(uint32 *mod);
    int32   (*ldd_tx_bias_set)(uint32 bias);
    int32   (*ldd_tx_mod_set)(uint32 mod);
    int32   (*ldd_driver_version_get)(uint32 *ver);
    int32   (*ldd_steering_mode_set)(void);
    int32   (*ldd_integration_mode_set)(void);
    int32   (*ldd_steering_mode_fixup)(void);
    int32   (*ldd_integration_mode_fixup)(void);
    int32   (*ldd_config_refresh)(void);
    int32   (*ldd_txCross_set)(rtk_enable_t enable, uint16 sign, uint16 str);
    int32   (*ldd_txCross_get)(rtk_enable_t *pEnable, uint16 *pSign, uint16 *pStr);
    int32   (*ldd_apcIavg_set)(uint16 value);
    int32   (*ldd_apcIavg_get)(uint16 *pValue);
    int32   (*ldd_apcEr_set)(uint16 value);
    int32   (*ldd_apcEr_get)(uint16 *pValue);
    int32   (*ldd_apcErTrim_set)(uint16 value);    
    int32   (*ldd_apcErTrim_get)(uint16 *pValue);
    int32   (*ldd_rxlosRefDac_set)(uint16 code);
    int32   (*ldd_rxlosRefDac_get)(uint16 *pCode);
    int32   (*ldd_rxlosHystSel_set)(uint16 value);               
    int32   (*ldd_rxlosHystSel_get)(uint16 *pValue); 
    int32   (*ldd_chip_init)(void);
    int32   (*ldd_api_test)(uint8 *flash_data);
    int32   (*ldd_reset2)(uint32 devId, rtk_ldd_reset_mode_t mode);
    int32   (*ldd_parameter2_set)(uint32 devId, uint32 address, uint32 data);
    int32   (*ldd_parameter2_get)(uint32 devId, uint32 address, uint32 *pData);
    int32   (*ldd_loop_mode2_set)(uint32 devId, uint32 mode);
    int32   (*ldd_laserLut2_set)(uint32 devId, uint32 entry, uint32 bias, uint32 mod);    
    int32   (*ldd_laserLut2_get)(uint32 devId, uint32 entry, uint32 *pBias, uint32 *pMod);
    int32   (*ldd_apdLut2_set)(uint32 devId, uint32 entry, uint32 data);
    int32   (*ldd_apdLut2_get)(uint32 devId, uint32 entry, uint32 *pData);    
    int32   (*ldd_mpd02_get)(uint32 devId, uint16 count, uint32 *value);
    int32   (*ldd_temperature2_get)(uint32 devId, uint32 *temp);
    int32   (*ldd_tx_bias2_get)(uint32 devId, uint32 *bias);
    int32   (*ldd_tx_mod2_get)(uint32 devId, uint32 *mod);
    int32   (*ldd_tx_bias2_set)(uint32 devId, uint32 bias);
    int32   (*ldd_tx_mod2_set)(uint32 devId, uint32 mod);
    int32   (*ldd_txCross2_set)(uint32 devId, rtk_enable_t enable, uint32 sign, uint32 str);
    int32   (*ldd_txCross2_get)(uint32 devId, rtk_enable_t *pEnable, uint32 *pSign, uint32 *pStr);
    int32   (*ldd_apcIavg2_set)(uint32 devId, uint32 value);
    int32   (*ldd_apcIavg2_get)(uint32 devId, uint32 *pValue);
    int32   (*ldd_apcEr2_set)(uint32 devId, uint32 value);
    int32   (*ldd_apcEr2_get)(uint32 devId, uint32 *pValue);
    int32   (*ldd_apcErTrim2_set)(uint32 devId, uint32 value);    
    int32   (*ldd_apcErTrim2_get)(uint32 devId, uint32 *pValue);
    int32   (*ldd_rxlosRefDac2_set)(uint32 devId, uint32 code);
    int32   (*ldd_rxlosRefDac2_get)(uint32 devId, uint32 *pCode);
    int32   (*ldd_rxlosHystSel2_set)(uint32 devId, uint32 value);               
    int32   (*ldd_rxlosHystSel2_get)(uint32 devId, uint32 *pValue); 
    int32   (*ldd_chip_init2)(uint32 devId);
    int32   (*ldd_rx_power2_get)(uint32 devId, rtk_ldd_rxpwr_t *pLddRxPwr); 
    int32   (*ldd_tx_power2_get)(uint32 devId, rtk_ldd_txpwr_t *pLddTxPwr);        
    int32   (*ldd_sdadc_code2_get)(uint32 devId, uint32 sdadc_type, uint32 *code_data);    
    int32   (*ldd_efuse2_set)(uint32 devId, uint32 entry, uint32 data);
    int32   (*ldd_efuse2_get)(uint32 devId, uint32 entry, uint32 *pData);
    int32   (*ldd_vdd2_get)(uint32 devId, uint32 *vdd);    
    int32   (*ldd_config2_refresh)(uint32 devId);
    int32   (*ldd_regBit_set)(uint32 devId, uint32 reg, uint32 rBit, uint32 value);
    int32   (*ldd_regBit_get)(uint32 devId, uint32 reg, uint32 rBit, uint32 *pValue);
    int32   (*ldd_regBits_set)(uint32 devId, uint32 reg, uint32 rBits, uint32 value);
    int32   (*ldd_regBits_get)(uint32 devId, uint32 reg, uint32 rBits, uint32 *pValue);	
    int32   (*ldd_txEsConfig_set)(uint32 devId, uint32 mode, uint32 value);
    int32   (*ldd_txEsConfig_get)(uint32 devId, uint32 mode, uint32 *pValue);		
    int32   (*ldd_dac_set)(uint32 devId,uint32 dac_offset,uint32 dac_code);
    int32   (*ldd_dac_get)(uint32 devId, uint32 *pDac_offset, uint32 *pDac_code);		
} rtk_ldd_mapper_t;


/*
 * Function Declaration
 */
extern int32
rtk_lddMapper_attach(rtk_ldd_mapper_t *mapper);

extern int32
rtk_lddMapper_deattach(void);

extern int32
rtk_ldd_i2c_init(rtk_i2c_port_t port);

extern int32
rtk_ldd_gpio_init(void);

extern int32
rtk_ldd_reset(rtk_ldd_reset_mode_t mode);

extern int32
rtk_ldd_calibration_state_set(rtk_enable_t state);

extern int32
rtk_ldd_parameter_set(uint32 length, uint32 offset, uint8 *flash_data);

extern int32
rtk_ldd_parameter_get(uint32 length, uint32 offset, uint8 *flash_data);

extern int32
rtk_ldd_loopMode_set(rtk_ldd_loop_mode_t mode);

extern int32
rtk_ldd_laserLut_set(rtk_ldd_loop_mode_t mode, uint8 *lut_data);

extern int32
rtk_ldd_apdLut_set(uint8 *lut_data);

extern int32
rtk_ldd_apcEnableFlow_set(rtk_ldd_apc_func_t func, rtk_ldd_loop_mode_t mode);

extern int32
rtk_ldd_txEnableFlow_set(rtk_ldd_tx_func_t func, rtk_ldd_loop_mode_t mode, uint8 *lut_data);

extern int32
rtk_ldd_rxEnableFlow_set(rtk_ldd_rx_func_t func);

extern int32
rtk_ldd_powerOnStatus_get(uint32 *result);

extern int32
rtk_ldd_tx_power_get(uint32 mpd0, uint32 *v_mpd, uint32 *i_mpd);

extern int32
rtk_ldd_rx_power_get(uint32 rssi_v0, uint32 *v_rssi,uint32 *i_rssi);

#if 0
extern int32
rtk_ldd_rssiVoltage_get(uint32 *rssi_voltage);
#endif
extern int32
rtk_ldd_rssiVoltage_get(rtk_ldd_cfg_t *ldd_cfg);

extern int32
rtk_ldd_rssiV0_get(uint32 *rssi_v0);

extern int32
rtk_ldd_vdd_get(uint32 *vdd);

extern int32
rtk_ldd_mpd0_get(uint16 count, uint32 *value);

extern int32
rtk_ldd_temperature_get(uint16 *temp);

extern int32
rtk_ldd_tx_bias_get(uint32 *bias);

extern int32
rtk_ldd_tx_mod_get(uint32 *mod);

extern int32
rtk_ldd_tx_bias_set(uint32 bias);

extern int32
rtk_ldd_tx_mod_set(uint32 mod);

extern int32
rtk_ldd_driver_version_get(uint32 *ver);

extern int32
rtk_ldd_steering_mode_set(void);

extern int32
rtk_ldd_integration_mode_set(void);

extern int32
rtk_ldd_steering_mode_fixup(void);

extern int32
rtk_ldd_integration_mode_fixup(void);

extern int32
rtk_ldd_config_refresh(void);

extern int32    
rtk_ldd_txCross_set(rtk_enable_t enable, uint16 sign, uint16 str);

extern int32    
rtk_ldd_txCross_get(rtk_enable_t *pEnable, uint16 *pSign, uint16 *pStr);

extern int32    
rtk_ldd_apcIavg_set(uint16 value);

extern int32
rtk_ldd_apcIavg_get(uint16 *pValue);

extern int32    
rtk_ldd_apcEr_set(uint16 value);

extern int32    
rtk_ldd_apcEr_get(uint16 *pValue);

extern int32    
rtk_ldd_apcErTrim_set(uint16 value);    
 
extern int32    
rtk_ldd_apcErTrim_get(uint16 *pValue);

extern int32    
rtk_ldd_rxlosRefDac_set(uint16 code);

extern int32    
rtk_ldd_rxlosRefDac_get(uint16 *pCode);

extern int32    
rtk_ldd_rxlosHystSel_set(uint16 value);                 
             
extern int32
rtk_ldd_rxlosHystSel_get(uint16 *pValue); 

extern int32
rtk_ldd_chip_init(void);

extern int32
rtk_ldd_api_test(uint8 *flash_data);

extern int32
rtk_ldd_reset2(uint32 devId, rtk_ldd_reset_mode_t mode);

extern int32
rtk_ldd_parameter2_set(uint32 devId,uint32 address, uint32 data);

extern int32
rtk_ldd_parameter2_get(uint32 devId, uint32 address, uint32 *pData);

extern int32
rtk_ldd_loopMode2_set(uint32 devId, rtk_ldd_loop_mode_t mode);

extern int32
rtk_ldd_laserLut2_set(uint32 devId, uint32 entry, uint32 bias, uint32 mod);

extern int32
rtk_ldd_laserLut2_get(uint32 devId, uint32 entry, uint32 *pBias, uint32 *pMod);

extern int32
rtk_ldd_apdLut2_set(uint32 devId, uint32 entry, uint32 data);

extern int32
rtk_ldd_apdLut2_get(uint32 devId, uint32 entry, uint32 *pData);


extern int32
rtk_ldd_mpd02_get(uint32 devId, uint16 count, uint32 *value);

extern int32
rtk_ldd_temperature2_get(uint32 devId, uint32 *temp);

extern int32
rtk_ldd_tx_bias2_get(uint32 devId, uint32 *bias);

extern int32
rtk_ldd_tx_mod2_get(uint32 devId, uint32 *mod);

extern int32
rtk_ldd_tx_bias2_set(uint32 devId, uint32 bias);

extern int32
rtk_ldd_tx_mod2_set(uint32 devId, uint32 mod);

extern int32
rtk_ldd_txCross2_set(uint32 devId, rtk_enable_t enable, uint32 sign, uint32 str);

extern int32
rtk_ldd_txCross2_get(uint32 devId, rtk_enable_t *pEnable, uint32 *pSign, uint32 *pStr);    

extern int32
rtk_ldd_apcIavg2_set(uint32 devId, uint32 value);

extern int32
rtk_ldd_apcIavg2_get(uint32 devId, uint32 *pValue);

extern int32    
rtk_ldd_apcEr2_set(uint32 devId, uint32 value);

extern int32    
rtk_ldd_apcEr2_get(uint32 devId, uint32 *pValue);

extern int32    
rtk_ldd_apcErTrim2_set(uint32 devId, uint32 value);    
  
extern int32    
rtk_ldd_apcErTrim2_get(uint32 devId, uint32 *pValue);

extern int32    
rtk_ldd_rxlosRefDac2_set(uint32 devId, uint32 code);

extern int32    
rtk_ldd_rxlosRefDac2_get(uint32 devId, uint32 *pCode);

extern int32    
rtk_ldd_rxlosHystSel2_set(uint32 devId, uint32 value);                 
                 
extern int32
rtk_ldd_rxlosHystSel2_get(uint32 devId, uint32 *pValue); 

extern int32
rtk_ldd_chip_init2(uint32 devId);

extern int32
rtk_ldd_rx_power2_get(uint32 devId, rtk_ldd_rxpwr_t *pLddRxPwr);

extern int32
rtk_ldd_tx_power2_get(uint32 devId, rtk_ldd_txpwr_t *pLddTxPwr);

extern int32
rtk_ldd_sdadc_code2_get(uint32 devId, uint32 sdadc_type, uint32 *code_data);

extern int32
rtk_ldd_efuse2_set(uint32 devId, uint32 entry, uint32 data);

extern int32
rtk_ldd_efuse2_get(uint32 devId, uint32 entry, uint32 *pData);

extern int32
rtk_ldd_vdd2_get(uint32 devId, uint32 *vdd);

extern int32
rtk_ldd_config2_refresh(uint32 devId);

extern int32
rtk_ldd_regBit_set(uint32 devId, uint32 reg, uint32 rBit, uint32 value);

extern int32
rtk_ldd_regBit_get(uint32 devId, uint32 reg, uint32 rBit, uint32 *pValue);

extern int32
rtk_ldd_regBits_set(uint32 devId, uint32 reg, uint32 rBits, uint32 value);

extern int32
rtk_ldd_regBits_get(uint32 devId, uint32 reg, uint32 rBits, uint32 *pValue); 

extern int32
rtk_ldd_txEsConfig_set(uint32 devId, uint32 mode, uint32 value);

extern int32
rtk_ldd_txEsConfig_get(uint32 devId, uint32 mode, uint32 *pValue);

extern int32
rtk_ldd_dac_set(uint32 devId, uint32 dac_offset, uint32 dac_code);

extern int32
rtk_ldd_dac_get(uint32 devId, uint32 *pDac_offset, uint32 *pDac_code);

#endif  /* __RTK_LDD_API_H__ */
