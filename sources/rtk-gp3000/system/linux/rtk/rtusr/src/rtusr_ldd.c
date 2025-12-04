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
 * $Revision: 58888 $
 * $Date: 2015-05-22 13:31:10 +0800 (Fri, 22 May 2015) $
 *
 * Purpose : Definition of LDD APIs
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) LDD control APIs
 *           (2) DDMI APIs
 */


/*
 * Include Files
 */
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <osal/lib.h>
#include <rtk/rtusr/include/rtusr_util.h>
#include <rtdrv/rtdrv_netfilter.h>


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
int32 rtk_ldd_i2c_init(rtk_i2c_port_t i2cPort)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
    ldd_cfg.i2cPort = i2cPort;
    SETSOCKOPT(RTDRV_LDD_I2C_INIT, &ldd_cfg, rtk_ldd_cfg_t, 1);

    return RT_ERR_OK;
}

int32 rtk_ldd_gpio_init(void)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
    SETSOCKOPT(RTDRV_LDD_GPIO_INIT, &ldd_cfg, rtk_ldd_cfg_t, 1);

    return RT_ERR_OK;
}

int32 rtk_ldd_reset(rtk_ldd_reset_mode_t mode)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
    ldd_cfg.reset = mode;
    SETSOCKOPT(RTDRV_LDD_RESET, &ldd_cfg, rtk_ldd_cfg_t, 1);

    return RT_ERR_OK;
}

int32 rtk_ldd_calibration_state_set(rtk_enable_t state)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
    ldd_cfg.state = state;
    SETSOCKOPT(RTDRV_LDD_CALIBRATION_STATE_SET, &ldd_cfg, rtk_ldd_cfg_t, 1);

    return RT_ERR_OK;
}

int32 rtk_ldd_parameter_set(uint32 length, uint32 offset, uint8 *flash_data)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
    ldd_cfg.rset_code = length;
    ldd_cfg.ldo_code = offset;
    ldd_cfg.flash_data = flash_data;
    SETSOCKOPT(RTDRV_LDD_PARAMETER_SET, &ldd_cfg, rtk_ldd_cfg_t, 1);

    return RT_ERR_OK;
}

int32 rtk_ldd_parameter_get(uint32 length, uint32 offset, uint8 *flash_data)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == flash_data), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
    ldd_cfg.rset_code = length;
    ldd_cfg.ldo_code = offset;
    ldd_cfg.flash_data = flash_data;
    GETSOCKOPT(RTDRV_LDD_PARAMETER_GET, &ldd_cfg, rtk_ldd_cfg_t, 1);

    return RT_ERR_OK;
}

int32 rtk_ldd_apcEnableFlow_set(rtk_ldd_apc_func_t func, rtk_ldd_loop_mode_t mode)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
    ldd_cfg.apc_func = func;
    ldd_cfg.loop_mode = mode;
    SETSOCKOPT(RTDRV_LDD_APC_ENABLE_SET, &ldd_cfg, rtk_ldd_cfg_t, 1);

    return RT_ERR_OK;
}

int32 rtk_ldd_loopMode_set(rtk_ldd_loop_mode_t mode)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
    ldd_cfg.loop_mode = mode;
    SETSOCKOPT(RTDRV_LDD_LOOP_MODE_SET, &ldd_cfg, rtk_ldd_cfg_t, 1);

    return RT_ERR_OK;
}

int32 rtk_ldd_laserLut_set(rtk_ldd_loop_mode_t mode, uint8 *lut_data)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == lut_data), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
    ldd_cfg.loop_mode = mode;
    ldd_cfg.flash_data = lut_data;
    SETSOCKOPT(RTDRV_LDD_LASER_LUT_SET, &ldd_cfg, rtk_ldd_cfg_t, 1);

    return RT_ERR_OK;
}

int32 rtk_ldd_apdLut_set(uint8 *lut_data)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == lut_data), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
    ldd_cfg.flash_data = lut_data;
    SETSOCKOPT(RTDRV_LDD_APD_LUT_SET, &ldd_cfg, rtk_ldd_cfg_t, 1);

    return RT_ERR_OK;
}

int32 rtk_ldd_txEnableFlow_set(rtk_ldd_tx_func_t func, rtk_ldd_loop_mode_t mode, uint8 *lut_data)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == lut_data), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
    ldd_cfg.tx_func = func;
    ldd_cfg.loop_mode = mode;
    ldd_cfg.flash_data = lut_data;
    SETSOCKOPT(RTDRV_LDD_TX_ENABLE_SET, &ldd_cfg, rtk_ldd_cfg_t, 1);

    return RT_ERR_OK;
}

int32 rtk_ldd_rxEnableFlow_set(rtk_ldd_rx_func_t func)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
    ldd_cfg.rx_func = func;
    SETSOCKOPT(RTDRV_LDD_RX_ENABLE_SET, &ldd_cfg, rtk_ldd_cfg_t, 1);

    return RT_ERR_OK;
}

int32 rtk_ldd_powerOnStatus_get(uint32 *result)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == result), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
    GETSOCKOPT(RTDRV_LDD_POWER_ON_GET, &ldd_cfg, rtk_ldd_cfg_t, 1);
    *result = ldd_cfg.adc_gnd_code;

    return RT_ERR_OK;
}

int32 rtk_ldd_tx_power_get(uint32 mpd0, uint32 *v_mpd, uint32 *i_mpd)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == v_mpd), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == i_mpd), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
    ldd_cfg.vdd_code = mpd0;
    GETSOCKOPT(RTDRV_LDD_TX_POWER_GET, &ldd_cfg, rtk_ldd_cfg_t, 1);
    *i_mpd = ldd_cfg.half_vdd_code;
    *v_mpd = ldd_cfg.gnd_code;

    return RT_ERR_OK;
}

int32 rtk_ldd_rx_power_get(uint32 rssi_v0, uint32 *v_rssi,uint32 *i_rssi)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == v_rssi), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == i_rssi), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
    ldd_cfg.rssi_v0 = rssi_v0;
    GETSOCKOPT(RTDRV_LDD_RX_POWER_GET, &ldd_cfg, rtk_ldd_cfg_t, 1);
    *i_rssi = ldd_cfg.rssi_voltage2;
    *v_rssi = ldd_cfg.rssi_voltage;

    return RT_ERR_OK;
}

#if 0
int32 rtk_ldd_rssiVoltage_get(uint32 *rssi_voltage)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == rssi_voltage), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
    GETSOCKOPT(RTDRV_LDD_RSSI_VOLTAGE_GET, &ldd_cfg, rtk_ldd_cfg_t, 1);
    *rssi_voltage = ldd_cfg.rssi_voltage;

    return RT_ERR_OK;
}
#endif

int32 rtk_ldd_rssiVoltage_get(rtk_ldd_cfg_t *ldd_cfg)
{
    //rtk_ldd_cfg_t rtdrv_cfg;
    rtk_ldd_cfg_t  rtdrv_cfg;	

    /* parameter check */
    RT_PARAM_CHK((NULL == ldd_cfg), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&rtdrv_cfg, 0, sizeof(rtk_ldd_cfg_t));	
    osal_memcpy(&rtdrv_cfg, ldd_cfg, sizeof(rtk_ldd_cfg_t));	
    GETSOCKOPT(RTDRV_LDD_RSSI_VOLTAGE_GET, &rtdrv_cfg, rtk_ldd_cfg_t, 1);
    osal_memcpy(ldd_cfg, &rtdrv_cfg, sizeof(rtk_ldd_cfg_t));		

    return RT_ERR_OK;
}

int32 rtk_ldd_rssiV0_get(uint32 *rssi_v0)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == rssi_v0), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
    GETSOCKOPT(RTDRV_LDD_RSSI_V0_GET, &ldd_cfg, rtk_ldd_cfg_t, 1);
    *rssi_v0 = ldd_cfg.rssi_v0;

    return RT_ERR_OK;
}

int32 rtk_ldd_vdd_get(uint32 *vdd)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == vdd), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
    GETSOCKOPT(RTDRV_LDD_VDD_GET, &ldd_cfg, rtk_ldd_cfg_t, 1);
    *vdd = ldd_cfg.vdd_code;

    return RT_ERR_OK;
}

int32 rtk_ldd_mpd0_get(uint16 count, uint32 *value)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == value), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
    ldd_cfg.count= count;
    GETSOCKOPT(RTDRV_LDD_MPD0_GET, &ldd_cfg, rtk_ldd_cfg_t, 1);

    *value = ldd_cfg.gnd_code;

    return RT_ERR_OK;
}

int32 rtk_ldd_temperature_get(uint16 *temp)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == temp), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
    GETSOCKOPT(RTDRV_LDD_TEMPERATURE_GET, &ldd_cfg, rtk_ldd_cfg_t, 1);
    *temp = ldd_cfg.temperature;

    return RT_ERR_OK;
}

int32 rtk_ldd_tx_bias_get(uint32 *bias)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == bias), RT_ERR_NULL_POINTER);

    /* function body */
    GETSOCKOPT(RTDRV_LDD_TX_BIAS_GET, &ldd_cfg, rtk_ldd_cfg_t, 1);
    *bias = ldd_cfg.tx_bias;

    return RT_ERR_OK;
}

int32 rtk_ldd_tx_mod_get(uint32 *mod)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == mod), RT_ERR_NULL_POINTER);

    /* function body */
    GETSOCKOPT(RTDRV_LDD_TX_MOD_GET, &ldd_cfg, rtk_ldd_cfg_t, 1);
    *mod = ldd_cfg.tx_mod;

    return RT_ERR_OK;
}

int32 rtk_ldd_tx_bias_set(uint32 bias)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
    ldd_cfg.tx_bias = bias;
    SETSOCKOPT(RTDRV_LDD_TX_BIAS_SET, &ldd_cfg, rtk_ldd_cfg_t, 1);

    return RT_ERR_OK;
}

int32 rtk_ldd_tx_mod_set(uint32 mod)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
    ldd_cfg.tx_mod = mod;
    SETSOCKOPT(RTDRV_LDD_TX_MOD_SET, &ldd_cfg, rtk_ldd_cfg_t, 1);

    return RT_ERR_OK;
}

int32 rtk_ldd_driver_version_get(uint32 *ver)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == ver), RT_ERR_NULL_POINTER);

    /* function body */
    GETSOCKOPT(RTDRV_LDD_DRIVER_VERSION_GET, &ldd_cfg, rtk_ldd_cfg_t, 1);
    *ver = ldd_cfg.driver_version;

    return RT_ERR_OK;
}

int32 rtk_ldd_steering_mode_set(void)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
    SETSOCKOPT(RTDRV_LDD_STEERING_MODE_SET, &ldd_cfg, rtk_ldd_cfg_t, 1);

    return RT_ERR_OK;
}

int32 rtk_ldd_integration_mode_set(void)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
    SETSOCKOPT(RTDRV_LDD_INTEGRATION_MODE_SET, &ldd_cfg, rtk_ldd_cfg_t, 1);

    return RT_ERR_OK;
}

int32 rtk_ldd_steering_mode_fixup(void)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
    SETSOCKOPT(RTDRV_LDD_STEERING_MODE_FIXUP, &ldd_cfg, rtk_ldd_cfg_t, 1);

    return RT_ERR_OK;
}

int32 rtk_ldd_integration_mode_fixup(void)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
    SETSOCKOPT(RTDRV_LDD_INTEGRATION_MODE_FIXUP, &ldd_cfg, rtk_ldd_cfg_t, 1);

    return RT_ERR_OK;
}

int32 rtk_ldd_config_refresh(void)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
    SETSOCKOPT(RTDRV_LDD_CONFIG_REFRESH, &ldd_cfg, rtk_ldd_cfg_t, 1);

    return RT_ERR_OK;
}


int32 rtk_ldd_txCross_set(rtk_enable_t enable, uint16 sign, uint16 str)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
    ldd_cfg.state       = enable;
    ldd_cfg.count       = sign;	
    ldd_cfg.temperature = str;		
    SETSOCKOPT(RTDRV_LDD_TXCROSS_SET, &ldd_cfg, rtk_ldd_cfg_t, 1);

    return RT_ERR_OK;
}


int32 rtk_ldd_txCross_get(rtk_enable_t *pEnable, uint16 *pSign, uint16 *pStr)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pSign), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pStr), RT_ERR_NULL_POINTER);

    /* function body */
    GETSOCKOPT(RTDRV_LDD_TXCROSS_GET, &ldd_cfg, rtk_ldd_cfg_t, 1);
    *pEnable = ldd_cfg.state;
	*pSign   = ldd_cfg.count;
	*pStr    = ldd_cfg.temperature;
    
    return RT_ERR_OK;
}

int32 rtk_ldd_apcIavg_set(uint16 value)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
    ldd_cfg.count       = value;	
    SETSOCKOPT(RTDRV_LDD_APC_IAVG_SET, &ldd_cfg, rtk_ldd_cfg_t, 1);

    return RT_ERR_OK;
}


int32 rtk_ldd_apcIavg_get(uint16 *pValue)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pValue), RT_ERR_NULL_POINTER);

    /* function body */
    GETSOCKOPT(RTDRV_LDD_APC_IAVG_GET, &ldd_cfg, rtk_ldd_cfg_t, 1);
	*pValue   = ldd_cfg.count;
    
    return RT_ERR_OK;
}

int32 rtk_ldd_apcEr_set(uint16 value)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
    ldd_cfg.count       = value;	
    SETSOCKOPT(RTDRV_LDD_APC_ER_SET, &ldd_cfg, rtk_ldd_cfg_t, 1);

    return RT_ERR_OK;
}


int32 rtk_ldd_apcEr_get(uint16 *pValue)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pValue), RT_ERR_NULL_POINTER);

    /* function body */
    GETSOCKOPT(RTDRV_LDD_APC_ER_GET, &ldd_cfg, rtk_ldd_cfg_t, 1);
	*pValue   = ldd_cfg.count;
    
    return RT_ERR_OK;
}

int32 rtk_ldd_apcErTrim_set(uint16 value)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
    ldd_cfg.count       = value;	
    SETSOCKOPT(RTDRV_LDD_APC_ERTRIM_SET, &ldd_cfg, rtk_ldd_cfg_t, 1);

    return RT_ERR_OK;
}


int32 rtk_ldd_apcErTrim_get(uint16 *pValue)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pValue), RT_ERR_NULL_POINTER);

    /* function body */
    GETSOCKOPT(RTDRV_LDD_APC_ERTRIM_GET, &ldd_cfg, rtk_ldd_cfg_t, 1);
	*pValue   = ldd_cfg.count;
    
    return RT_ERR_OK;
}

int32 rtk_ldd_rxlosRefDac_set(uint16 code)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
    ldd_cfg.count       = code;	
    SETSOCKOPT(RTDRV_LDD_RXLOS_REFDAC_SET, &ldd_cfg, rtk_ldd_cfg_t, 1);

    return RT_ERR_OK;
}


int32 rtk_ldd_rxlosRefDac_get(uint16 *pCode)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pCode), RT_ERR_NULL_POINTER);

    /* function body */
    GETSOCKOPT(RTDRV_LDD_RXLOS_REFDAC_GET, &ldd_cfg, rtk_ldd_cfg_t, 1);
	*pCode   = ldd_cfg.count;
    
    return RT_ERR_OK;
}



int32 rtk_ldd_rxlosHystSel_set(uint16 value)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
    ldd_cfg.count       = value;	
    SETSOCKOPT(RTDRV_LDD_RXLOS_HYSTSEL_SET, &ldd_cfg, rtk_ldd_cfg_t, 1);

    return RT_ERR_OK;
}


int32 rtk_ldd_rxlosHystSel_get(uint16 *pValue)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pValue), RT_ERR_NULL_POINTER);

    /* function body */
    GETSOCKOPT(RTDRV_LDD_RXLOS_HYSTSEL_GET, &ldd_cfg, rtk_ldd_cfg_t, 1);
	*pValue   = ldd_cfg.count;
    
    return RT_ERR_OK;
}

int32 rtk_ldd_chip_init(void)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
    SETSOCKOPT(RTDRV_LDD_CHIP_INIT, &ldd_cfg, rtk_ldd_cfg_t, 1);

    return RT_ERR_OK;
}

int32 rtk_ldd_api_test(uint8 *flash_data )
{
    rtk_ldd_cfg_t ldd_cfg;

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
	ldd_cfg.flash_data = flash_data;
    SETSOCKOPT(RTDRV_LDD_API_TEST, &ldd_cfg, rtk_ldd_cfg_t, 1);

    return RT_ERR_OK;
}

int32 rtk_ldd_rx_power2_get(uint32 devId, rtk_ldd_rxpwr_t *pLddRxPwr)

{
    //rtk_ldd_cfg_t rtdrv_cfg;
    rtk_ldd_cfg_t  ldd_cfg;    

    /* parameter check */
    RT_PARAM_CHK((NULL == pLddRxPwr), RT_ERR_NULL_POINTER);

    //printf("\n%s: %d\n",__FUNCTION__, __LINE__); 

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
    ldd_cfg.devId = devId;
    osal_memcpy(&ldd_cfg.rxp, pLddRxPwr, sizeof(rtk_ldd_rxpwr_t));    
    GETSOCKOPT(RTDRV_LDD_RX_POWER2_GET, &ldd_cfg, rtk_ldd_cfg_t, 1);
    osal_memcpy(pLddRxPwr, &ldd_cfg.rxp, sizeof(rtk_ldd_rxpwr_t));        

    return RT_ERR_OK;
}

int32 rtk_ldd_tx_power2_get(uint32 devId, rtk_ldd_txpwr_t *pLddTxPwr)

{
    //rtk_ldd_cfg_t rtdrv_cfg;
    rtk_ldd_cfg_t  ldd_cfg;    

    /* parameter check */
    RT_PARAM_CHK((NULL == pLddTxPwr), RT_ERR_NULL_POINTER);

    //printf("\n%s: %d\n",__FUNCTION__, __LINE__); 

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
    ldd_cfg.devId = devId;
    osal_memcpy(&ldd_cfg.txp, pLddTxPwr, sizeof(rtk_ldd_txpwr_t));    
    GETSOCKOPT(RTDRV_LDD_TX_POWER2_GET, &ldd_cfg, rtk_ldd_cfg_t, 1);
    osal_memcpy(pLddTxPwr, &ldd_cfg.txp, sizeof(rtk_ldd_txpwr_t));        

    return RT_ERR_OK;
}

int32 rtk_ldd_reset2(uint32 devId, rtk_ldd_reset_mode_t mode)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
    ldd_cfg.devId = devId;
    ldd_cfg.reset = mode;
    SETSOCKOPT(RTDRV_LDD_RESET2, &ldd_cfg, rtk_ldd_cfg_t, 1);

    return RT_ERR_OK;
}

int32 rtk_ldd_parameter2_set(uint32 devId, uint32 address, uint32 data)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
    ldd_cfg.devId = devId;
    ldd_cfg.vdd_code= address;
    ldd_cfg.gnd_code= data;
    SETSOCKOPT(RTDRV_LDD_PARAMETER2_SET, &ldd_cfg, rtk_ldd_cfg_t, 1);

    return RT_ERR_OK;
}

int32 rtk_ldd_parameter2_get(uint32 devId, uint32 address, uint32 *pData)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    //printf("\n%s: %d addr = 0x%x data = 0x%x \n",__FUNCTION__, __LINE__, address, *pData); 

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
    ldd_cfg.devId = devId;    
    ldd_cfg.vdd_code = address;
    GETSOCKOPT(RTDRV_LDD_PARAMETER2_GET, &ldd_cfg, rtk_ldd_cfg_t, 1);
    *pData    = ldd_cfg.gnd_code;

    //printf("\n%s: %d addr = 0x%x data = 0x%x \n",__FUNCTION__, __LINE__, address, *pData); 

    return RT_ERR_OK;
}

int32 rtk_ldd_loopMode2_set(uint32 devId, uint32 mode)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
    ldd_cfg.devId = devId;    
    ldd_cfg.vdd_code = mode;
    SETSOCKOPT(RTDRV_LDD_LOOP_MODE2_SET, &ldd_cfg, rtk_ldd_cfg_t, 1);

    return RT_ERR_OK;
}

int32 rtk_ldd_laserLut2_set(uint32 devId, uint32 entry, uint32 bias, uint32 mod)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
    ldd_cfg.devId = devId;    
    ldd_cfg.vdd_code= entry;
    ldd_cfg.gnd_code= bias;
    ldd_cfg.half_vdd_code = mod;
    SETSOCKOPT(RTDRV_LDD_LASER_LUT2_SET, &ldd_cfg, rtk_ldd_cfg_t, 1);

    return RT_ERR_OK;
}

int32 rtk_ldd_laserLut2_get(uint32 devId, uint32 entry, uint32 *pBias, uint32 *pMod)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pBias), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pMod), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));    
    ldd_cfg.devId = devId;
    ldd_cfg.vdd_code = entry; 	
    GETSOCKOPT(RTDRV_LDD_LASER_LUT2_GET, &ldd_cfg, rtk_ldd_cfg_t, 1);
    *pBias   = ldd_cfg.gnd_code;
    *pMod    = ldd_cfg.half_vdd_code;
	
    return RT_ERR_OK;
}

int32 rtk_ldd_apdLut2_set(uint32 devId, uint32 entry, uint32 data)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
    ldd_cfg.devId = devId;
    ldd_cfg.vdd_code = entry;
    ldd_cfg.gnd_code = data;
    SETSOCKOPT(RTDRV_LDD_APD_LUT2_SET, &ldd_cfg, rtk_ldd_cfg_t, 1);

    return RT_ERR_OK;
}

int32 rtk_ldd_apdLut2_get(uint32 devId, uint32 entry, uint32 *pData)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));    
    ldd_cfg.devId = devId;
    ldd_cfg.vdd_code = entry; 	
    GETSOCKOPT(RTDRV_LDD_APD_LUT2_GET, &ldd_cfg, rtk_ldd_cfg_t, 1);
    *pData   = ldd_cfg.gnd_code;
	
    return RT_ERR_OK;
}


int32 rtk_ldd_mpd02_get(uint32 devId, uint16 count, uint32 *value)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == value), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
    ldd_cfg.devId = devId;    
    ldd_cfg.count= count;
    GETSOCKOPT(RTDRV_LDD_MPD02_GET, &ldd_cfg, rtk_ldd_cfg_t, 1);

    *value = ldd_cfg.gnd_code;

    return RT_ERR_OK;
}

int32 rtk_ldd_temperature2_get(uint32 devId, uint32 *temp)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == temp), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
    ldd_cfg.devId = devId;    
    GETSOCKOPT(RTDRV_LDD_TEMPERATURE2_GET, &ldd_cfg, rtk_ldd_cfg_t, 1);
    *temp = ldd_cfg.vdd_code;

    return RT_ERR_OK;
}

int32 rtk_ldd_tx_bias2_get(uint32 devId, uint32 *bias)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == bias), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
    ldd_cfg.devId = devId;
    GETSOCKOPT(RTDRV_LDD_TX_BIAS2_GET, &ldd_cfg, rtk_ldd_cfg_t, 1);
    *bias = ldd_cfg.tx_bias;

    return RT_ERR_OK;
}

int32 rtk_ldd_tx_mod2_get(uint32 devId, uint32 *mod)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == mod), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
    ldd_cfg.devId = devId;
    GETSOCKOPT(RTDRV_LDD_TX_MOD2_GET, &ldd_cfg, rtk_ldd_cfg_t, 1);
    *mod = ldd_cfg.tx_mod;

    return RT_ERR_OK;
}

int32 rtk_ldd_tx_bias2_set(uint32 devId, uint32 bias)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
    ldd_cfg.devId = devId;    
    ldd_cfg.tx_bias = bias;
    SETSOCKOPT(RTDRV_LDD_TX_BIAS2_SET, &ldd_cfg, rtk_ldd_cfg_t, 1);

    return RT_ERR_OK;
}

int32 rtk_ldd_tx_mod2_set(uint32 devId, uint32 mod)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
    ldd_cfg.devId = devId;    
    ldd_cfg.tx_mod = mod;
    SETSOCKOPT(RTDRV_LDD_TX_MOD2_SET, &ldd_cfg, rtk_ldd_cfg_t, 1);

    return RT_ERR_OK;
}

int32 rtk_ldd_txCross2_set(uint32 devId, rtk_enable_t enable, uint32 sign, uint32 str)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
    ldd_cfg.devId = devId;    
    ldd_cfg.state       = enable;
    ldd_cfg.vdd_code = sign;    
    ldd_cfg.gnd_code = str;        
    SETSOCKOPT(RTDRV_LDD_TXCROSS2_SET, &ldd_cfg, rtk_ldd_cfg_t, 1);

    return RT_ERR_OK;
}


int32 rtk_ldd_txCross2_get(uint32 devId, rtk_enable_t *pEnable, uint32 *pSign, uint32 *pStr)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pSign), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pStr), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
    ldd_cfg.devId = devId;    
    GETSOCKOPT(RTDRV_LDD_TXCROSS2_GET, &ldd_cfg, rtk_ldd_cfg_t, 1);
    *pEnable = ldd_cfg.state;
    *pSign   = ldd_cfg.vdd_code;
    *pStr    = ldd_cfg.gnd_code;
    
    return RT_ERR_OK;
}

int32 rtk_ldd_apcIavg2_set(uint32 devId, uint32 value)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
    ldd_cfg.devId = devId;        
    ldd_cfg.vdd_code = value;    
    SETSOCKOPT(RTDRV_LDD_APC_IAVG2_SET, &ldd_cfg, rtk_ldd_cfg_t, 1);

    return RT_ERR_OK;
}


int32 rtk_ldd_apcIavg2_get(uint32 devId, uint32 *pValue)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pValue), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
    ldd_cfg.devId = devId;    
    GETSOCKOPT(RTDRV_LDD_APC_IAVG2_GET, &ldd_cfg, rtk_ldd_cfg_t, 1);
    *pValue   = ldd_cfg.vdd_code;
    
    return RT_ERR_OK;
}

int32 rtk_ldd_apcEr2_set(uint32 devId, uint32 value)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
    ldd_cfg.devId = devId;    
    ldd_cfg.vdd_code= value;    
    SETSOCKOPT(RTDRV_LDD_APC_ER2_SET, &ldd_cfg, rtk_ldd_cfg_t, 1);

    return RT_ERR_OK;
}


int32 rtk_ldd_apcEr2_get(uint32 devId, uint32 *pValue)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pValue), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
    ldd_cfg.devId = devId;    
    GETSOCKOPT(RTDRV_LDD_APC_ER2_GET, &ldd_cfg, rtk_ldd_cfg_t, 1);
    *pValue   = ldd_cfg.vdd_code;
    
    return RT_ERR_OK;
}

int32 rtk_ldd_apcErTrim2_set(uint32 devId, uint32 value)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
    ldd_cfg.devId = devId;    
    ldd_cfg.vdd_code= value;    
    SETSOCKOPT(RTDRV_LDD_APC_ERTRIM2_SET, &ldd_cfg, rtk_ldd_cfg_t, 1);

    return RT_ERR_OK;
}


int32 rtk_ldd_apcErTrim2_get(uint32 devId, uint32 *pValue)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pValue), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
    ldd_cfg.devId = devId;        
    GETSOCKOPT(RTDRV_LDD_APC_ERTRIM2_GET, &ldd_cfg, rtk_ldd_cfg_t, 1);
    *pValue   = ldd_cfg.vdd_code;
    
    return RT_ERR_OK;
}

int32 rtk_ldd_rxlosRefDac2_set(uint32 devId, uint32 code)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
    ldd_cfg.devId = devId;        
    ldd_cfg.vdd_code= code;    
    SETSOCKOPT(RTDRV_LDD_RXLOS_REFDAC2_SET, &ldd_cfg, rtk_ldd_cfg_t, 1);

    return RT_ERR_OK;
}


int32 rtk_ldd_rxlosRefDac2_get(uint32 devId, uint32 *pCode)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pCode), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
    ldd_cfg.devId = devId;        
    GETSOCKOPT(RTDRV_LDD_RXLOS_REFDAC2_GET, &ldd_cfg, rtk_ldd_cfg_t, 1);
    *pCode   = ldd_cfg.vdd_code;
    
    return RT_ERR_OK;
}

int32 rtk_ldd_rxlosHystSel2_set(uint32 devId, uint32 value)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
    ldd_cfg.devId = devId;        
    ldd_cfg.vdd_code= value;    
    SETSOCKOPT(RTDRV_LDD_RXLOS_HYSTSEL2_SET, &ldd_cfg, rtk_ldd_cfg_t, 1);

    return RT_ERR_OK;
}


int32 rtk_ldd_rxlosHystSel2_get(uint32 devId, uint32 *pValue)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pValue), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));    
    ldd_cfg.devId = devId;        
    GETSOCKOPT(RTDRV_LDD_RXLOS_HYSTSEL2_GET, &ldd_cfg, rtk_ldd_cfg_t, 1);
    *pValue   = ldd_cfg.vdd_code;
    
    return RT_ERR_OK;
}

int32 rtk_ldd_sdadc_code2_get(uint32 devId, uint32 sdadc_type, uint32 *code_data)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == code_data), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));    
    ldd_cfg.devId = devId;
    ldd_cfg.vdd_code = sdadc_type; 	
    GETSOCKOPT(RTDRV_LDD_SDADC_CODE2_GET, &ldd_cfg, rtk_ldd_cfg_t, 1);
    *code_data   = ldd_cfg.gnd_code;
    
    return RT_ERR_OK;
}

int32 rtk_ldd_efuse2_set(uint32 devId, uint32 entry, uint32 data)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
    ldd_cfg.devId = devId;        
    ldd_cfg.vdd_code = entry;
    ldd_cfg.gnd_code = data;	
    SETSOCKOPT(RTDRV_LDD_EFUSE2_SET, &ldd_cfg, rtk_ldd_cfg_t, 1);

    return RT_ERR_OK;
}


int32 rtk_ldd_efuse2_get(uint32 devId, uint32 entry, uint32 *pData)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));    
    ldd_cfg.devId = devId;
    ldd_cfg.vdd_code = entry; 	
    GETSOCKOPT(RTDRV_LDD_EFUSE2_GET, &ldd_cfg, rtk_ldd_cfg_t, 1);
    *pData   = ldd_cfg.gnd_code;
	
    return RT_ERR_OK;
}

int32 rtk_ldd_vdd2_get(uint32 devId, uint32 *vdd)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == vdd), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
    ldd_cfg.devId = devId;	
    GETSOCKOPT(RTDRV_LDD_VDD2_GET, &ldd_cfg, rtk_ldd_cfg_t, 1);
    *vdd = ldd_cfg.vdd_code;

    return RT_ERR_OK;
}

int32 rtk_ldd_regBit_set(uint32 devId, uint32 reg, uint32 rBit, uint32 value)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
    ldd_cfg.devId = devId;
    ldd_cfg.vdd_code= reg;
    ldd_cfg.half_vdd_code= rBit;	
    ldd_cfg.gnd_code= value;
    SETSOCKOPT(RTDRV_LDD_REGBIT_SET, &ldd_cfg, rtk_ldd_cfg_t, 1);

    return RT_ERR_OK;
}

int32 rtk_ldd_regBit_get(uint32 devId, uint32 reg, uint32 rBit, uint32 *pValue)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pValue), RT_ERR_NULL_POINTER);

    //printf("\n%s: %d addr = 0x%x rBit = 0x%x, data = 0x%x \n",__FUNCTION__, __LINE__, reg, rBit, *pValue); 

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
    ldd_cfg.devId = devId;    
    ldd_cfg.vdd_code = reg;
    ldd_cfg.half_vdd_code= rBit;
    GETSOCKOPT(RTDRV_LDD_REGBIT_GET, &ldd_cfg, rtk_ldd_cfg_t, 1);
    *pValue    = ldd_cfg.gnd_code;

    //printf("\n%s: %d addr = 0x%x rBit = 0x%x, data = 0x%x \n",__FUNCTION__, __LINE__, reg, rBit, *pValue); 

    return RT_ERR_OK;
}

int32 rtk_ldd_regBits_set(uint32 devId, uint32 reg, uint32 rBits, uint32 value)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
    ldd_cfg.devId = devId;
    ldd_cfg.vdd_code= reg;
    ldd_cfg.half_vdd_code= rBits;	
    ldd_cfg.gnd_code= value;
    SETSOCKOPT(RTDRV_LDD_REGBITS_SET, &ldd_cfg, rtk_ldd_cfg_t, 1);

    return RT_ERR_OK;
}

int32 rtk_ldd_regBits_get(uint32 devId, uint32 reg, uint32 rBits, uint32 *pValue)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pValue), RT_ERR_NULL_POINTER);

    //printf("\n%s: %d addr = 0x%x data = 0x%x \n",__FUNCTION__, __LINE__, reg, *pValue); 

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
    ldd_cfg.devId = devId;    
    ldd_cfg.vdd_code = reg;
    ldd_cfg.half_vdd_code= rBits;
    GETSOCKOPT(RTDRV_LDD_REGBITS_GET, &ldd_cfg, rtk_ldd_cfg_t, 1);
    *pValue    = ldd_cfg.gnd_code;

    //printf("\n%s: %d addr = 0x%x data = 0x%x \n",__FUNCTION__, __LINE__, reg, *pValue); 

    return RT_ERR_OK;
}

int32 rtk_ldd_txEsConfig_set(uint32 devId, uint32 mode, uint32 value)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));    
    ldd_cfg.devId = devId;
    ldd_cfg.vdd_code = mode; 
    ldd_cfg.gnd_code = value; 	
    SETSOCKOPT(RTDRV_LDD_TXES_CONFIG_SET, &ldd_cfg, rtk_ldd_cfg_t, 1);	
    
    return RT_ERR_OK;
}

int32 rtk_ldd_txEsConfig_get(uint32 devId, uint32 mode, uint32 *pValue)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pValue), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));    
    ldd_cfg.devId = devId;
    ldd_cfg.vdd_code = mode; 	
    GETSOCKOPT(RTDRV_LDD_TXES_CONFIG_GET, &ldd_cfg, rtk_ldd_cfg_t, 1);
    *pValue   = ldd_cfg.gnd_code;
    
    return RT_ERR_OK;
}

int32 rtk_ldd_dac_set(uint32 devId, uint32 dac_offset, uint32 dac_code)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));    
    ldd_cfg.devId = devId;
    ldd_cfg.vdd_code = dac_offset; 
    ldd_cfg.gnd_code = dac_code; 	
	//printf("\n%s: %d  dac_offset = 0x%x dac_code = 0x%x \n",__FUNCTION__, __LINE__, ldd_cfg.vdd_code, ldd_cfg.gnd_code);
    SETSOCKOPT(RTDRV_LDD_DAC_SET, &ldd_cfg, rtk_ldd_cfg_t, 1);	
    
    return RT_ERR_OK;
}

int32 rtk_ldd_dac_get(uint32 devId, uint32 *pDac_offset, uint32 *pDac_code)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pDac_offset), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pDac_code), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));    
    ldd_cfg.devId = devId;
    GETSOCKOPT(RTDRV_LDD_DAC_GET, &ldd_cfg, rtk_ldd_cfg_t, 1);
    *pDac_offset   = ldd_cfg.vdd_code;	
    *pDac_code   = ldd_cfg.gnd_code;
    //printf("\n%s: %d pDac_offset = 0x%x pDac_code = 0x%x \n",__FUNCTION__, __LINE__, ldd_cfg.vdd_code, ldd_cfg.gnd_code);    
    return RT_ERR_OK;
}

int32 rtk_ldd_config2_refresh(uint32 devId)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
    ldd_cfg.devId = devId;    
    SETSOCKOPT(RTDRV_LDD_CONFIG2_REFRESH, &ldd_cfg, rtk_ldd_cfg_t, 1);

    return RT_ERR_OK;
}

int32 rtk_ldd_chip_init2(uint32 devId)
{
    rtk_ldd_cfg_t ldd_cfg;

    /* function body */
    osal_memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
    ldd_cfg.devId = devId;        
    SETSOCKOPT(RTDRV_LDD_CHIP_INIT2, &ldd_cfg, rtk_ldd_cfg_t, 1);

    return RT_ERR_OK;
}

