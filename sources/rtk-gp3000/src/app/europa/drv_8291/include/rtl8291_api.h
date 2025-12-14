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
 * Purpose : Definition of GPIO API
 *
 * Feature : Provide the APIs to enable and configure GPIO
 *
 */

#ifndef __RTL8291_API_H__
#define __RTL8291_API_H__

/*
 * Include Files
 */

#include <rtk/i2c.h>
#include <rtk/ldd.h>


//Temperature Related definition
//273.15*1024 
#define RTL8291_TEMP_ZERO        0x4449A
//19bits, 9 bits integer, 10 bits fraction.
#define RTL8291_TEMP_MASK        0x7FFFF 


#define RTL8291_RX_32BIT_MODE                0
#define RTL8291_RX_64BIT_MODE                1
#define RTL8291_RX_REAL_MODE                 0x2
#define RTL8291_RX_TRANS_MODE                0xFF
#define RTL8291_RX_CURR2PWR_MODE             0xFE
#define RTL8291_RX_HARDWARE_MODE             0x3

#define RTL8291_TX_HARDWARE_MODE             0x1
#define RTL8291_TX_REAL_MODE                 0x2
#define RTL8291_TX_CURR2PWR_MODE             0x3


typedef enum rtl8291_bosa_type_e
{
    RTL8291_BOSA_APD,
    RTL8291_BOSA_PIN,
    RTL8291_BOSA_TYPE_END
} rtl8291_bosa_type_t;

typedef enum rtl8291_rxcal_mode_e
{
    RTL8291_22BIT_MODE = 0,
    RTL8291_14BIT_MODE,
    RTL8291_12BIT_MODE,
    RTL8291_BITMODE_END
} rtl8291_rxcal_mode_t;

typedef enum rtl8291_pwr_unit_e
{
    RTL8291_NA_MODE = 0,
    RTL8291_001UA_MODE,        
    RTL8291_01UA_MODE,
    RTL8291_UNIT_MODE_END
} rtl8291_pwr_unit_t;

typedef enum rtl8291_txes_mode_s
{
    RTL8291_TXES_VG_MODE = 0,
    RTL8291_TXES_VG_STR = 0,
    RTL8291_TXES_EMP_LEVEL,
    RTL8291_TXES_VG_AMP,
    RTL8291_TXES_DELY_RISING,
    RTL8291_TXES_DELY_FALLING,
    RTL8291_TXES_ADJ_ENABLE,   
    RTL8291_TXES_MODE_END
} rtl8291_txes_mode_t;


#if 0
typedef struct  rtl8291_param_s
{
    uint32  devId;
    uint32  rssi_v0;
    uint32  vdd_v0;
    uint32  gnd_v0;
    uint32  half_v0;
    int32   rssi_k;    
    uint32  rssi_i;
    uint32  rssi_voltage;
    uint32  rx_power;
    uint32  tx_lsb;
    uint32  tx_code_250u;
    uint32  tx_r_mpd;
    uint32  mpd_i;
    uint32  tx_power;     
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
    int32   rx_shft_a;
    int32   rx_shft_b;
    int32   rx_shft_c; 
    int32   tx_shft_a;
    int32   tx_shft_b;
    int32   tx_shft_c; 	
    int8    temp_offset;
    uint32  steering_mode;
    uint16  rssi_r1;
    uint16  rssi_r2;        
    uint32  rx_power_mode;
    uint32  iavg1;
    int16   temp_0;
    int16   temp_0_offset;
    uint16  ibim_temp;    
    int16   rxddmi_offset;
    uint32  *data;
}  rtl8291_param_t;
#endif

typedef struct  rtl8291_param_s
{
    uint32  devId;
    uint32  rx_lsb;
    uint32  rx_code_200u;
    uint32  rx_r6400;   
    uint32  rssi_i;
    uint32  rx_power;
    uint32  tx_lsb;
    uint32  tx_code_250u;
    uint32  tx_r_mpd;
    uint32  mpd_i;
    uint32  tx_power;     
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
    int32   rx_shft_a;
    int32   rx_shft_b;
    int32   rx_shft_c; 
    int32   tx_shft_a;
    int32   tx_shft_b;
    int32   tx_shft_c; 	
    int8    temp_offset;
    uint32  steering_mode;
    uint16  rssi_r1;
    uint16  rssi_r2;        
    uint32  rx_power_mode;
    uint32  iavg1;
    int16   temp_0;
    int16   temp_0_offset;
    uint16  ibim_temp;    
    int16   rxddmi_offset;
    uint32  *data;
}  rtl8291_param_t;


typedef struct  rtl8291_rxpwr_s
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
}  rtl8291_rxpwr_t;

typedef struct  rtl8291_txpwr_s
{
    uint32  op_mode;
    uint32  rssi_code1;
    uint32  rssi_code2;     
    uint32  rssi_code3;
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
}  rtl8291_txpwr_t;


typedef enum rtl8291_sdadc_code_type_s
{
    RTL8291_CODE_TEM = 0,
    RTL8291_CODE_CALV,
    RTL8291_CODE_PS,
    RTL8291_CODE_TXAPC,
    RTL8291_CODE_LDO,
    RTL8291_CODE_RSSIINP_SEL,
    RTL8291_CODE_RSSIINP,
    RTL8291_CODE_RSSIINN_SEL,
    RTL8291_CODE_RSSIINN,
    RTL8291_CODE_RSSIFL_SEL,
    RTL8291_CODE_RSSIFL,
    RTL8291_CODE_VLASERN,
    RTL8291_CODE_ADCIN,
    RTL8291_CODE_PS_TX,
    RTL8291_CODE_PS_RX,
    RTL8291_CODE_VBG,
    RTL8291_CODE_CALG, 
    RTL8291_CODE_TXAPC_RSEL,
    RTL8291_SDADC_CODE_END
} rtl8291_sdadc_code_type_t;

typedef struct  rtl8291_sdadc_code_s
{
    uint32  tem_code;
    uint32  calv_code;     
    uint32  ps_code;
    uint32  txapc_code;
    uint32  ldo_code;
    uint32  rssiinp_sel;	
    uint32  rssiinp_code;
    uint32  rssiinn_sel;	
    uint32  rssiinn_code;
    uint32  rssifl_code;     
    uint32  vlasern_code;
    uint32  adcin_code;
    uint32  pstx_code;
    uint32  psrx_code;
    uint32  vbg_code; 	
    uint32  calg_code; 	
}  rtl8291_sdadc_code_t;

extern int _8291_debug_flag(uint32 devId);
extern int32 rtl8291_chipInfo_get(uint32 devId, uint32 *rtl_num, uint32 *ver);
extern int32 rtl8291_sdadc_code_get(uint32 devId, uint32 sdadc_type, uint32 *code_data);
extern int32  rtl8291_reset(uint32 devId,  rtk_ldd_reset_mode_t mode);
extern int32  rtl8291_driver_version_get(uint32 *ver);

extern int32  rtl8291_dac_set(uint32 devId, uint32 dac_offset, uint32 dac_code);
extern int32  rtl8291_dac_get(uint32 devId, uint32 *pDac_offset, uint32 *pDac_code);

extern int32 rtl8291_rssiR6400_get(uint32 devId, uint32 *value);
extern int32  rtl8291_vdd_get(uint32 devId, uint32 *vdd);
extern int32  rtl8291_apdLut_set(uint32 devId, uint32 entry, uint32 data);
extern int32  rtl8291_apdLut_get(uint32 devId, uint32 entry, uint32 *pData);
extern int32  rtl8291_laserLut_set(uint32 devId, uint32 entry, uint32 bias, uint32 mod);
extern int32  rtl8291_laserLut_get(uint32 devId, uint32 entry, uint32 *pBias, uint32 *pMod);

extern int32  rtl8291_parameter_set(uint32 devId, uint32 address, uint32 data);
extern int32  rtl8291_parameter_get(uint32 devId, uint32 address, uint32 *pData);
   
extern int32  rtl8291_efuse_set(uint32 devId, uint32 entry, uint32 data);
extern int32  rtl8291_efuse_get(uint32 devId, uint32 entry, uint32 *pData);
extern int32  rtl8291_txLutEnable_set(uint32 devId, uint32 enable);       
extern int32  rtl8291_txLutEnable_get(uint32 devId, uint32 *pEnable);
extern int32  rtl8291_apdLutEnable_set(uint32 devId, uint32 enable);       
extern int32  rtl8291_apdLutEnable_get(uint32 devId, uint32 *pEnable);
extern int32  _rtl8291_rxCurr2Pwr_get(uint32 devId, rtl8291_rxpwr_t *pLdd_rxpwr);
extern int32  rtl8291_rxPower_get(uint32 devId, rtl8291_rxpwr_t *pLdd_rxpwr);
//extern int32  _rtl8291_rssiCurrent_get(uint32 devId, rtl8291_rxpwr_t *pLdd_rxpwr);
extern int32  rtl8291_rxRssiCurrent_get(uint32 devId, rtl8291_rxpwr_t *pLdd_rxpwr);
extern int32  rtl8291_rxRssiCurrentHw_get(uint32 devId, rtl8291_rxpwr_t *pLdd_rxpwr);

extern int32  _rtl8291_mpdCurrent_get(uint32 devId, rtl8291_txpwr_t *pLdd_txpwr);
extern int32  rtl8291_txMpdCurrent_get(uint32 devId, rtl8291_txpwr_t *pLdd_txpwr);
extern int32  rtl8291_txMpdCurrentHw_get(uint32 devId, rtl8291_txpwr_t *pLdd_txpwr);

extern int32  _rtl8291_txCurr2Pwr_get(uint32 devId, rtl8291_txpwr_t *pLdd_txpwr);
extern int32  rtl8291_txPower_get(uint32 devId, rtl8291_txpwr_t *pLdd_txpwr);

extern int32  rtl8291_config_refresh(uint32 devId);


#if 1
extern int32  rtl8291_apcLdo_set(uint32 devId, uint32 apc_ldo);
extern int32  rtl8291_apcLdo_get(uint32 devId, uint32 *pApc_ldo);
extern int32  rtl8291_txLdo_set(uint32 devId, uint32 tx_ldo);
extern int32  rtl8291_txLdo_get(uint32 devId, uint32 *pTx_ldo);
extern int32  rtl8291_rxLdo_set(uint32 devId, uint32 rx_ldo);
extern int32  rtl8291_rxLdo_get(uint32 devId, uint32 *pRx_ldo);
#if 0
extern int32  rtl8291_txBleedN_set(uint32 devId, uint32 enable);
extern int32  rtl8291_txBleedN_get(uint32 devId, uint32 *pEnable);
extern int32  rtl8291_txBleedP_set(uint32 devId, uint32 enable);
extern int32  rtl8291_txBleedP_get(uint32 devId, uint32 *pEnable);
extern int32  rtl8291_txBackTerm_set(uint32 devId, uint32 type, uint32 value);
extern int32  rtl8291_txBackTerm_get(uint32 devId, uint32 *pType, uint32 *pValue);
extern int32  rtl8291_txInterface_set(uint32 devId, uint32 value);
extern int32  rtl8291_txInterface_get(uint32 devId, uint32 *pValue);

extern int32  rtl8291_txIntenalRlpn_set(uint32 devId, uint32 value);
extern int32  rtl8291_txIntenalRlpn_get(uint32 devId, uint32 *pValue);
#endif
extern int32  rtl8291_txCrossEn_set(uint32 devId, uint32 enable);
extern int32  rtl8291_txCrossEn_get(uint32 devId, uint32 *pEnable);
extern int32  rtl8291_txCrossAdj_set(uint32 devId, uint32 sign, uint32 str);
extern int32  rtl8291_txCrossAdj_get(uint32 devId, uint32 *pSign, uint32 *pStr);
#if 0
extern int32  rtl8291_txBiasDac_set(uint32 devId, uint32 value);
extern int32  rtl8291_txBiasDac_get(uint32 devId, uint32 *pValue);
extern int32  rtl8291_txModDac_set(uint32 devId, uint32 value);
extern int32  rtl8291_txModDac_get(uint32 devId, uint32 *pValue);
#endif
extern int32  rtl8291_txInPolSwap_set(uint32 devId, uint32 value);
extern int32  rtl8291_txInPolSwap_get(uint32 devId, uint32 *pValue);
extern int32  rtl8291_txOutPolSwap_set(uint32 devId, uint32 value);
extern int32  rtl8291_txOutPolSwap_get(uint32 devId, uint32 *pValue);
#if 0
extern int32  rtl8291_txDcdCalEn_set(uint32 devId, uint32 enable);
extern int32  rtl8291_txDcdCalEn_get(uint32 devId, uint32 *pEnable);
#endif
extern int32  rtl8291_apcLaserOnDelay_set(uint32 devId, uint32 value);
extern int32  rtl8291_apcLaserOnDelay_get(uint32 devId, uint32 *pValue);
extern int32  rtl8291_apcSettleCnt_set(uint32 devId, uint32 value);
extern int32  rtl8291_apcSettleCnt_get(uint32 devId, uint32 *pValue);
extern int32  rtl8291_apcIbiasInitCode_set(uint32 devId, uint32 value);
extern int32  rtl8291_apcIbiasInitCode_get(uint32 devId, uint32 *pValue);
extern int32  rtl8291_apcImodInitCode_set(uint32 devId, uint32 value);
extern int32  rtl8291_apcImodInitCode_get(uint32 devId, uint32 *pValue);
extern int32  rtl8291_apcLoopMode_set(uint32 devId, uint32 mode);
extern int32  rtl8291_apcLoopMode_get(uint32 devId, uint32 *pMode);
extern int32  rtl8291_apcIavg_set(uint32 devId, uint32 value);
extern int32  rtl8291_apcIavg_get(uint32 devId, uint32 *pValue);
extern int32  rtl8291_apcEr_set(uint32 devId, uint32 value);
extern int32  rtl8291_apcEr_get(uint32 devId, uint32 *pValue);
#if 0
extern int32  rtl8291_apcErTrim_set(uint32 devId, uint32 value);
extern int32  rtl8291_apcErTrim_get(uint32 devId, uint32 *pValue);
#endif
extern int32  rtl8291_apcCmpd_set(uint32 devId, uint32 value);
extern int32  rtl8291_apcCmpd_get(uint32 devId, uint32 *pValue);
extern int32  rtl8291_apcLpfBw_set(uint32 devId, uint32 value);
extern int32  rtl8291_apcLpfBw_get(uint32 devId, uint32 *pValue);
extern int32  rtl8291_apcApcLoopGain_set(uint32 devId, uint32 value);
extern int32  rtl8291_apcApcLoopGain_get(uint32 devId, uint32 *pValue);
extern int32  rtl8291_apcErcLoopGain_set(uint32 devId, uint32 value);
extern int32  rtl8291_apcErcLoopGain_get(uint32 devId, uint32 *pValue);
extern int32  rtl8291_apcErcChopperEn_set(uint32 devId, uint32 enable);
extern int32  rtl8291_apcErcChopperEn_get(uint32 devId, uint32 *pEnable);
extern int32  rtl8291_apcCrossEn_set(uint32 devId, uint32 enable);
extern int32  rtl8291_apcCrossEn_get(uint32 devId, uint32 *pEnable);
extern int32  rtl8291_apcCrossAdj_set(uint32 devId, uint32 sign, uint32 str);
extern int32  rtl8291_apcCrossAdj_get(uint32 devId, uint32 *pSign, uint32 *pStr);
extern int32  rtl8291_apcModMaxEn_set(uint32 devId, uint32 enable);
extern int32  rtl8291_apcModMaxEn_get(uint32 devId, uint32 *pEnable);
extern int32  rtl8291_apcModMax_set(uint32 devId, uint32 value);
extern int32  rtl8291_apcModMax_get(uint32 devId, uint32 *pValue);
extern int32  rtl8291_apcModMinEn_set(uint32 devId, uint32 enable);
extern int32  rtl8291_apcModMinEn_get(uint32 devId, uint32 *pEnable);
extern int32  rtl8291_apcModMin_set(uint32 devId, uint32 value);
extern int32  rtl8291_apcModMin_get(uint32 devId, uint32 *pValue);
extern int32  rtl8291_apcBiasMaxEn_set(uint32 devId, uint32 enable);
extern int32  rtl8291_apcBiasMaxEn_get(uint32 devId, uint32 *pEnable);
extern int32  rtl8291_apcBiasMax_set(uint32 devId, uint32 value);
extern int32  rtl8291_apcBiasMax_get(uint32 devId, uint32 *pValue);
extern int32  rtl8291_apcBiasMinEn_set(uint32 devId, uint32 enable);
extern int32  rtl8291_apcBiasMinEn_get(uint32 devId, uint32 *pEnable);
extern int32  rtl8291_apcBiasMin_set(uint32 devId, uint32 value);
extern int32  rtl8291_apcBiasMin_get(uint32 devId, uint32 *pValue);
extern int32  rtl8291_apcPwrLvlMaxEr_set(uint32 devId, uint32 value);
extern int32  rtl8291_apcPwrLvlMaxEr_get(uint32 devId, uint32 *pValue);
extern int32  rtl8291_apcApcTimer_set(uint32 devId, uint32 value);
extern int32  rtl8291_apcApcTimer_get(uint32 devId, uint32 *pValue);
extern int32  rtl8291_apcErcTimer_set(uint32 devId, uint32 value);
extern int32  rtl8291_apcErcTimer_get(uint32 devId, uint32 *pValue);
extern int32  rtl8291_apcImod_biasInitCode_set(uint32 devId, uint32 mod_value, uint32 bias_value);
extern int32  rtl8291_tx_mod_bias_set(uint32 devId, uint32 mod, uint32 bias);
extern int32  rtl8291_fsuEnable_set(uint32 devId, uint32 enable);
extern int32  rtl8291_fsuEnable_get(uint32 devId, uint32 *pEnable);
extern int32  rtl8291_fsuMode_set(uint32 devId, uint32 mode);
extern int32  rtl8291_fsuMode_get(uint32 devId, uint32 *pMode);
extern int32  rtl8291_fsuApcLoopGain_set(uint32 devId, uint32 value);
extern int32  rtl8291_fsuApcLoopGain_get(uint32 devId, uint32 *pValue);
extern int32  rtl8291_fsuApcRampb_set(uint32 devId, uint32 value);
extern int32  rtl8291_fsuApcRampb_get(uint32 devId, uint32 *pValue);
extern int32  rtl8291_fsuApcRampm_set(uint32 devId, uint32 value);
extern int32  rtl8291_fsuApcRampm_get(uint32 devId, uint32 *pValue);
extern int32  rtl8291_fsuRstCount_set(uint32 devId, uint32 mode);
extern int32  rtl8291_fsuRstCount_get(uint32 devId, uint32 *pMode);
extern int32  rtl8291_fsuSettleCount_set(uint32 devId, uint32 value);
extern int32  rtl8291_fsuSettleCount_get(uint32 devId, uint32 *pValue);
extern int32  rtl8291_fsuErcLoopGain_set(uint32 devId, uint32 value);
extern int32  rtl8291_fsuErcLoopGain_get(uint32 devId, uint32 *pValue);
extern int32  rtl8291_fsuErcRampm_set(uint32 devId, uint32 value);
extern int32  rtl8291_fsuErcRampm_get(uint32 devId, uint32 *pValue);

extern int32  rtl8291_rxOutPolSwap_set(uint32 devId, uint32 state);
extern int32  rtl8291_rxOutPolSwap_get(uint32 devId, uint32 *pState);
extern int32  rtl8291_rxOutSwing_set(uint32 devId, uint32 value);      
extern int32  rtl8291_rxOutSwing_get(uint32 devId, uint32 *pValue);
extern int32  rtl8291_rxDisOffcan_set(uint32 devId, uint32 disable);      
extern int32  rtl8291_rxDisOffcan_get(uint32 devId, uint32 *pDisable);
extern int32  rtl8291_txsdMode_set(uint32 devId, uint32 mode);     
extern int32  rtl8291_txsdMode_get(uint32 devId, uint32 *pMode);

extern int32  rtl8291_txsdTh_set(uint32 devId, uint32 value);
            
extern int32  rtl8291_txsdTh_get(uint32 devId, uint32 *pValue);

extern int32  rtl8291_txsdTiaGain_set(uint32 devId, uint32 state);
            
extern int32  rtl8291_txsdTiaGain_get(uint32 devId, uint32 *pState);

extern int32  rtl8291_txsdHighLoopGain_set(uint32 devId, uint32 gain);
                
extern int32  rtl8291_txsdHighLoopGain_get(uint32 devId, uint32 *pGain);

extern int32  rtl8291_txsdLowLoopGain_set(uint32 devId, uint32 gain);
                    
extern int32  rtl8291_txsdLowLoopGain_get(uint32 devId, uint32 *pGain);
     
extern int32  rtl8291_txsdOnRstCount_set(uint32 devId, uint32 value);
                
extern int32  rtl8291_txsdOnRstCount_get(uint32 devId, uint32 *pValue);

extern int32  rtl8291_txsdOffRstCount_set(uint32 devId, uint32 value);
                    
extern int32  rtl8291_txsdOffRstCount_get(uint32 devId, uint32 *pValue);

extern int32  rtl8291_txsdFaultTimer_set(uint32 devId, uint32 value);
                    
extern int32  rtl8291_txsdFaultTimer_get(uint32 devId, uint32 *pValue);

extern int32  rtl8291_txdisCtrl_set(uint32 devId, uint32 value);                         
                           
extern int32  rtl8291_txdisCtrl_get(uint32 devId, uint32 *pValue);

extern int32  rtl8291_txDisPinEn_set(uint32 devId, uint32 enable);
                    
extern int32  rtl8291_txDisPinEn_get(uint32 devId, uint32 *pEnable);

extern int32  rtl8291_txSdPinEn_set(uint32 devId, uint32 enable);
                    
extern int32  rtl8291_txSdPinEn_get(uint32 devId, uint32 *pEnable);

extern int32  rtl8291_rxlosRefDac_set(uint32 devId, uint32 code);

extern int32  rtl8291_rxlosRefDac_get(uint32 devId, uint32 *pCode);

extern int32  rtl8291_rxlosHystSel_set(uint32 devId, uint32 value);
           
extern int32  rtl8291_rxlosHystSel_get(uint32 devId, uint32 *pValue);
  
extern int32  rtl8291_rxlosSampleSel_set(uint32 devId, uint32 value);
       
extern int32  rtl8291_rxlosSampleSel_get(uint32 devId, uint32 *pValue);
        
extern int32  rtl8291_rxlosRangSel_set(uint32 devId, uint32 value);
        
extern int32  rtl8291_rxlosRangSel_get(uint32 devId, uint32 *pValue);


    
extern int32  rtl8291_rxlosDebounceSel_set(uint32 devId, uint32 value);
       
extern int32  rtl8291_rxlosDebounceSel_get(uint32 devId, uint32 *pValue);

extern int32  rtl8291_rxlosDebounceOpt_set(uint32 devId, uint32 value);
       
extern int32  rtl8291_rxlosDebounceOpt_get(uint32 devId, uint32 *pValue);

extern int32  rtl8291_rxlosSquelch_set(uint32 devId, uint32 value);
       
extern int32  rtl8291_rxlosSquelch_get(uint32 devId, uint32 *pValue);


#if 0    
extern int32  rtl8291_rxlosChopperFreq_set(uint32 devId, uint32 value);
       
extern int32  rtl8291_rxlosChopperFreq_get(uint32 devId, uint32 *pValue);
          
extern int32  rtl8291_rxlosLaMagComp_set(uint32 devId, uint32 value);
           
extern int32  rtl8291_rxlosLaMagComp_get(uint32 devId, uint32 *pValue);

extern int32 rtl8291_rxlosBufAutozero_set(uint32 devId, uint32 value);
            
extern int32  rtl8291_rxlosBufAutozero_get(uint32 devId, uint32 *pValue);
#endif
      
extern int32  rtl8291_rxlosResetb_set(uint32 devId, uint32 value);

extern int32  rtl8291_rxlosResetb_get(uint32 devId, uint32 *pValue);
    
extern int32  rtl8291_rxlosClkMode_set(uint32 devId, uint32 value);
           
extern int32  rtl8291_rxlosClkMode_get(uint32 devId, uint32 *pValue);

extern int32  rtl8291_rxlosTestMode_set(uint32 devId, uint32 value);
         
extern int32  rtl8291_rxlosTestMode_get(uint32 devId, uint32 *pValue);

extern int32 rtl8291_rxlosAssertSel_set(uint32 devId, uint32 value);
            
extern int32  rtl8291_rxlosAssertSel_get(uint32 devId, uint32 *pValue);
      
extern int32  rtl8291_rxlosPolarity_set(uint32 devId, uint32 value);

extern int32  rtl8291_rxlosPolarity_get(uint32 devId, uint32 *pValue);

extern int32  rtl8291_rxlosPinEn_set(uint32 devId, uint32 enable);
                    
extern int32  rtl8291_rxlosPinEn_get(uint32 devId, uint32 *pEnable);

extern int32  rtl8291_rxSleepPin_set(uint32 devId, uint32 enable);
        
extern int32  rtl8291_rxSleepPin_get(uint32 devId, uint32 *pEnable);

extern int32  rtl8291_rxSleepForceLos_set(uint32 devId, uint32 enable);
        
extern int32  rtl8291_rxSleepForceLos_get(uint32 devId, uint32 *pEnable);

extern int32  rtl8291_rxSleepPdLa_set(uint32 devId, uint32 enable);
        
extern int32  rtl8291_rxSleepPdLa_get(uint32 devId, uint32 *pEnable);

extern int32  rtl8291_rxSleepPdRssi_set(uint32 devId, uint32 enable);
        
extern int32  rtl8291_rxSleepPdRssi_get(uint32 devId, uint32 *pEnable);

extern int32  rtl8291_rxSleepPdVcm_set(uint32 devId, uint32 enable);
        
extern int32  rtl8291_rxSleepPdVcm_get(uint32 devId, uint32 *pEnable);

extern int32  rtl8291_rxSleepPdBooster_set(uint32 devId, uint32 enable);  

extern int32  rtl8291_rxSleepPdBooster_get(uint32 devId, uint32 *pEnable);

extern int32  rtl8291_autoPowerLeveling_set(uint32 devId, uint32 enable);
        
extern int32  rtl8291_autoPowerLeveling_get(uint32 devId, uint32 *pEnable);

extern int32  rtl8291_autoPowerLevelingHth_set(uint32 devId, uint32 value);

extern int32  rtl8291_autoPowerLevelingHth_get(uint32 devId, uint32 *pValue);

extern int32  rtl8291_autoPowerLevelingLth_set(uint32 devId, uint32 value);

extern int32  rtl8291_autoPowerLevelingLth_get(uint32 devId, uint32 *pValue);

extern int32  rtl8291_ddmiTxpwrLatchSel_set(uint32 devId, uint32 value);

extern int32  rtl8291_ddmiTxpwrLatchSel_get(uint32 devId, uint32 *pValue);

extern int32 rtl8291_txEsEmpVgMode_set(uint32 devId, uint32 enable);

extern int32 rtl8291_txEsEmpVgMode_get(uint32 devId, uint32 *pEnable);

extern int32 rtl8291_txEsEmpVgStr_set(uint32 devId, uint32 str);

extern int32 rtl8291_txEsEmpVgStr_get(uint32 devId, uint32 *pStr);

extern int32 rtl8291_txEsEmpLevel_set(uint32 devId, uint32 level);

extern int32 rtl8291_txEsEmpLevel_get(uint32 devId, uint32 *plevel);

extern int32 rtl8291_txEsEmpVgAmp_set(uint32 devId, uint32 vg_amp);

extern int32 rtl8291_txEsEmpVgAmp_get(uint32 devId, uint32 *pVg_amp);

extern int32 rtl8291_txEsEmpDelay_set(uint32 devId, uint32 rising, uint32 falling);

extern int32 rtl8291_txEsEmpDelay_get(uint32 devId, uint32 *pRising, uint32 *pFalling);
extern int32 rtl8291_txEsAdjEn_get(uint32 devId, uint32 *pEnable);
extern int32 rtl8291_txEsConfig_set(uint32 devId, uint32 mode, uint32 value);

extern int32 rtl8291_txEsConfig_set(uint32 devId, uint32 mode, uint32 value);
extern int32 rtl8291_txEsConfig_get(uint32 devId, uint32 mode, uint32 *pValue);

extern int32 rtl8291_vhvlGenRefOpt_set(uint32 devId, uint32 value);
extern int32 rtl8291_vhvlGenRefOpt_get(uint32 devId, uint32 *pValue);
extern int32 rtl8291_cmlNcapSel_set(uint32 devId, uint32 value);
extern int32 rtl8291_cmlNcapSel_get(uint32 devId, uint32 *pValue);

extern int32  rtl8291_calTrigger_set(uint32 devId);
extern int32 rtl8291_roData_get(uint32 devId, uint32 page, uint32 datasel, uint32 *pRo_data);
extern int32 rtl8291_ibx_set(uint32 devId);
#if 0
extern int32 rtl8291_forceRk_set(uint32 devId, uint32 rx_rxo_l, uint32 rx_rk400l, uint32 rk_force_en_l);
#endif
extern int32  rtl8291_ercComp_set(uint32 devId, uint32 step);

extern int32  rtl8291_tx_bias_set(uint32 devId, uint32 bias);
extern int32  rtl8291_tx_mod_set(uint32 devId, uint32 mod);
extern int32  rtl8291_tx_bias_get(uint32 devId, uint32 *bias);
extern int32  rtl8291_tx_mod_get(uint32 devId, uint32 *mod);
extern int32 rtl8291_txCross_set(uint32 devId, rtk_enable_t enable, uint32 sign, uint32 str);
extern int32 rtl8291_txCross_get(uint32 devId, rtk_enable_t *pEnable, uint32 *pSign, uint32 *pStr); 
extern int32  rtl8291_initProcFaultAction_set(uint32 devId, uint32 enable);
extern void rtl8291_fault_status(uint32 devId);
#if 0
extern int32 rtl8291_rssiK_get(uint32 devId, uint32 *value);
extern int32 rtl8291_mpdK_get(uint32 devId, uint32 *value);
extern int32 rtl8291_mpdR500_get(uint32 devId, uint32 *value);
#endif
extern int32  rtl8291_temperature_get(uint32 devId, uint32 *temp);
#endif

#if 0
extern int32  rtl8291_booster_set(uint32 devId, uint16 type);
extern int32  rtl8291_bosaType_get(uint32 devId, uint32 *pType);

extern int32  rtl8291_txInTypOpt_set(uint32 devId, uint32 opt);
extern int32  rtl8291_txInTypOpt_get(uint32 devId, uint32 *pOpt);
extern int32  rtl8291_txDcdCalLutSel_set(uint32 devId, uint32 value);
extern int32  rtl8291_txDcdCalLutSel_get(uint32 devId, uint32 *pValue);
extern int32  rtl8291_rxlosInputSel_set(uint32 devId, uint32 value);
extern int32  rtl8291_rxlosInputSel_get(uint32 devId, uint32 *pValue);
extern int32  rtl8291_rxlosChopperEn_set(uint32 devId, uint32 value);
extern int32  rtl8291_rxlosChopperEn_get(uint32 devId, uint32 *pValue);

#endif

#endif  /* __RTL8291_API_H__ */
