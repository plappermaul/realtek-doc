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
#include <common/rt_type.h>
#include <rtk/init.h>
#include <rtk/default.h>
#include <dal/dal_mgmt.h>
#include <osal/lib.h>
#include <rtk/ldd.h>

#include <osal/lib.h>
#include <osal/print.h>
/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */
rtk_ldd_mapper_t ldd_mapper;
rtk_ldd_mapper_t *pLddMapper = &ldd_mapper;


/*
 * Macro Declaration
 */


/*
 * Function Declaration
 */
int32 rtk_lddMapper_attach(rtk_ldd_mapper_t *mapper)
{
    if (NULL == mapper)
        return RT_ERR_FAILED;

    osal_memcpy(pLddMapper, mapper, sizeof(rtk_ldd_mapper_t));

    return RT_ERR_OK;
}

int32 rtk_lddMapper_deattach(void)
{
    osal_memset(pLddMapper, 0, sizeof(rtk_ldd_mapper_t));

    return RT_ERR_OK;
}

int32 rtk_ldd_i2c_init(rtk_i2c_port_t port)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_i2c_init)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_i2c_init(port);
    RTK_API_UNLOCK();

    return ret;
}

int32 rtk_ldd_gpio_init(void)
{
    /* no GPIO pin info? */
#if 1
    return RT_ERR_OK;
#else
    int32   ret;

    if (NULL == pLddMapper->ldd_gpio_init)
        return RT_ERR_OK;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_gpio_init();
    RTK_API_UNLOCK();

    return ret;
#endif
}

int32 rtk_ldd_reset(rtk_ldd_reset_mode_t mode)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_reset)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_reset(mode);
    RTK_API_UNLOCK();

    return ret;
}

int32 rtk_ldd_calibration_state_set(rtk_enable_t state)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_calibration_state_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_calibration_state_set(state);
    RTK_API_UNLOCK();

    return ret;
}

int32 rtk_ldd_parameter_set(uint32 length, uint32 offset, uint8 *flash_data)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_parameter_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_parameter_set(length, offset, flash_data);
    RTK_API_UNLOCK();

    return ret;
}

int32 rtk_ldd_parameter_get(uint32 length, uint32 offset, uint8 *flash_data)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_parameter_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_parameter_get(length, offset, flash_data);
    RTK_API_UNLOCK();

    return ret;
}

int32 rtk_ldd_loopMode_set(rtk_ldd_loop_mode_t mode)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_loop_mode_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_loop_mode_set(mode);
    RTK_API_UNLOCK();

    return ret;
}

int32 rtk_ldd_laserLut_set(rtk_ldd_loop_mode_t mode, uint8 *lut_data)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_laser_lut_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_laser_lut_set(mode, lut_data);
    RTK_API_UNLOCK();

    return ret;
}

int32 rtk_ldd_apdLut_set(uint8 *lut_data)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_apd_lut_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_apd_lut_set(lut_data);
    RTK_API_UNLOCK();

    return ret;
}

int32 rtk_ldd_apcEnableFlow_set(rtk_ldd_apc_func_t func, rtk_ldd_loop_mode_t mode)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_apc_enable_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_apc_enable_set(func, mode);
    RTK_API_UNLOCK();

    return ret;
}

int32 rtk_ldd_txEnableFlow_set(rtk_ldd_tx_func_t func, rtk_ldd_loop_mode_t mode, uint8 *lut_data)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_tx_enable_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_tx_enable_set(func, mode, lut_data);
    RTK_API_UNLOCK();

    return ret;
}

int32 rtk_ldd_rxEnableFlow_set(rtk_ldd_rx_func_t func)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_rx_enable_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_rx_enable_set(func);
    RTK_API_UNLOCK();

    return ret;
}

int32 rtk_ldd_powerOnStatus_get(uint32 *result)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_power_on_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_power_on_get(result);
    RTK_API_UNLOCK();

    return ret;
}

int32 rtk_ldd_tx_power_get(uint32 mpd0, uint32 *v_mpd, uint32 *i_mpd)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_tx_power_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_tx_power_get(mpd0, v_mpd, i_mpd);
    RTK_API_UNLOCK();

    return ret;
}

int32 rtk_ldd_rx_power_get(uint32 rssi_v0, uint32 *v_rssi, uint32 *i_rssi)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_rx_power_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_rx_power_get(rssi_v0, v_rssi, i_rssi);
    RTK_API_UNLOCK();

    return ret;
}
#if 0
int32 rtk_ldd_rssiVoltage_get(uint32 *rssi_voltage)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_rssi_voltage_get)
        return RT_ERR_OK;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_rssi_voltage_get(rssi_voltage);
    RTK_API_UNLOCK();

    return ret;
}
#endif

int32 rtk_ldd_rssiVoltage_get(rtk_ldd_cfg_t *ldd_cfg)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_rssi_voltage_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_rssi_voltage_get(ldd_cfg);
    RTK_API_UNLOCK();

    return ret;
}


int32 rtk_ldd_rssiV0_get(uint32 *rssi_v0)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_rssi_v0_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_rssi_v0_get(1, rssi_v0);
    RTK_API_UNLOCK();

    return ret;
}

int32 rtk_ldd_vdd_get(uint32 *vdd)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_vdd_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_vdd_get(vdd);
    RTK_API_UNLOCK();

    return ret;
}

int32 rtk_ldd_mpd0_get(uint16 count, uint32 *value)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_mpd0_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_mpd0_get(count, value);
    RTK_API_UNLOCK();

    return ret;
}

int32 rtk_ldd_temperature_get(uint16 *temp)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_temperature_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_temperature_get(temp);
    RTK_API_UNLOCK();

    return ret;
}

int32 rtk_ldd_tx_bias_get(uint32 *bias)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_tx_bias_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_tx_bias_get(bias);
    RTK_API_UNLOCK();

    return ret;
}

int32 rtk_ldd_tx_mod_get(uint32 *mod)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_tx_mod_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_tx_mod_get(mod);
    RTK_API_UNLOCK();

    return ret;
}

int32 rtk_ldd_tx_bias_set(uint32 bias)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_tx_bias_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_tx_bias_set(bias);
    RTK_API_UNLOCK();

    return ret;
}

int32 rtk_ldd_tx_mod_set(uint32 mod)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_tx_mod_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_tx_mod_set(mod);
    RTK_API_UNLOCK();

    return ret;
}

int32 rtk_ldd_driver_version_get(uint32 *ver)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_driver_version_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_driver_version_get(ver);
    RTK_API_UNLOCK();

    return ret;
}

int32 rtk_ldd_steering_mode_set(void)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_steering_mode_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_steering_mode_set();
    RTK_API_UNLOCK();

    return ret;
}

int32 rtk_ldd_integration_mode_set(void)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_integration_mode_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_integration_mode_set();
    RTK_API_UNLOCK();

    return ret;
}

int32 rtk_ldd_steering_mode_fixup(void)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_steering_mode_fixup)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_steering_mode_fixup();
    RTK_API_UNLOCK();

    return ret;
}

int32 rtk_ldd_integration_mode_fixup(void)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_integration_mode_fixup)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_integration_mode_fixup();
    RTK_API_UNLOCK();

    return ret;
}

int32 rtk_ldd_config_refresh(void)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_config_refresh)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_config_refresh();
    RTK_API_UNLOCK();

    return ret;
}

int32 rtk_ldd_txCross_set(rtk_enable_t enable, uint16 sign, uint16 str)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_txCross_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_txCross_set(enable, sign, str);
    RTK_API_UNLOCK();

    return ret;
}

int32 rtk_ldd_txCross_get(rtk_enable_t *pEnable, uint16 *pSign, uint16 *pStr)
{
    int32 ret;

    if (NULL == pLddMapper->ldd_txCross_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_txCross_get(pEnable, pSign, pStr);
    RTK_API_UNLOCK();

    return ret;
}


int32 rtk_ldd_apcIavg_set(uint16 value)
{
    int32 ret;

    if (NULL == pLddMapper->ldd_apcIavg_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    
    RTK_API_LOCK();
    ret = pLddMapper->ldd_apcIavg_set(value);
    RTK_API_UNLOCK();
    
    return ret;
}


int32 rtk_ldd_apcIavg_get(uint16 *pValue)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_apcIavg_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_apcIavg_get(pValue);
    RTK_API_UNLOCK();

    return ret;
}

int32 rtk_ldd_apcEr_set(uint16 value)
{
    int32 ret;

    if (NULL == pLddMapper->ldd_apcEr_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    
    RTK_API_LOCK();
    ret = pLddMapper->ldd_apcEr_set(value);
    RTK_API_UNLOCK();
    
    return ret;
}


int32 rtk_ldd_apcEr_get(uint16 *pValue)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_apcEr_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_apcEr_get(pValue);
    RTK_API_UNLOCK();

    return ret;
}

int32 rtk_ldd_apcErTrim_set(uint16 value)
{
    int32 ret;

    if (NULL == pLddMapper->ldd_apcErTrim_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    
    RTK_API_LOCK();
    ret = pLddMapper->ldd_apcErTrim_set(value);
    RTK_API_UNLOCK();
    
    return ret;
}


int32 rtk_ldd_apcErTrim_get(uint16 *pValue)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_apcErTrim_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_apcErTrim_get(pValue);
    RTK_API_UNLOCK();

    return ret;
}

int32 rtk_ldd_rxlosRefDac_set(uint16 value)
{
    int32 ret;

    if (NULL == pLddMapper->ldd_rxlosRefDac_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    
    RTK_API_LOCK();
    ret = pLddMapper->ldd_rxlosRefDac_set(value);
    RTK_API_UNLOCK();
    
    return ret;
}


int32 rtk_ldd_rxlosRefDac_get(uint16 *pValue)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_rxlosRefDac_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_rxlosRefDac_get(pValue);
    RTK_API_UNLOCK();

    return ret;
}


int32 rtk_ldd_rxlosHystSel_set(uint16 value)
{
    int32 ret;

    if (NULL == pLddMapper->ldd_rxlosHystSel_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    
    RTK_API_LOCK();
    ret = pLddMapper->ldd_rxlosHystSel_set(value);
    RTK_API_UNLOCK();
    
    return ret;
}


int32 rtk_ldd_rxlosHystSel_get(uint16 *pValue)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_rxlosHystSel_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_rxlosHystSel_get(pValue);
    RTK_API_UNLOCK();

    return ret;
}

int32 rtk_ldd_chip_init(void)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_chip_init)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_chip_init();
    RTK_API_UNLOCK();

    return ret;
}

int32 rtk_ldd_api_test(uint8 *flash_data)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_api_test)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_api_test(flash_data);
    RTK_API_UNLOCK();

    return ret;
}

int32 rtk_ldd_reset2(uint32 devId, rtk_ldd_reset_mode_t mode)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_reset2)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_reset2(devId, mode);
    RTK_API_UNLOCK();

    return ret;
}

int32 rtk_ldd_parameter2_set(uint32 devId, uint32 address, uint32 data)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_parameter2_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_parameter2_set(devId, address, data);
    RTK_API_UNLOCK();

    return ret;
}

int32 rtk_ldd_parameter2_get(uint32 devId, uint32 address, uint32 *pData)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_parameter2_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    //osal_printf("\n%s: %d\n",__FUNCTION__, __LINE__); 

    RTK_API_LOCK();
    ret = pLddMapper->ldd_parameter2_get(devId, address, pData);
    RTK_API_UNLOCK();

    return ret;
}

int32 rtk_ldd_loopMode2_set(uint32 devId, uint32 mode)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_loop_mode2_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_loop_mode2_set(devId, mode);
    RTK_API_UNLOCK();

    return ret;
}

int32 rtk_ldd_laserLut2_set(uint32 devId, uint32 entry, uint32 bias, uint32 mod)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_laserLut2_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_laserLut2_set(devId, entry, bias, mod);
    RTK_API_UNLOCK();

    return ret;
}

int32 rtk_ldd_laserLut2_get(uint32 devId, uint32 entry, uint32 *pBias, uint32 *pMod)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_laserLut2_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_laserLut2_get(devId, entry, pBias, pMod);
    RTK_API_UNLOCK();

    return ret;
}

int32 rtk_ldd_apdLut2_set(uint32 devId, uint32 entry, uint32 data)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_apdLut2_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_apdLut2_set(devId, entry, data);
    RTK_API_UNLOCK();

    return ret;
}

int32 rtk_ldd_apdLut2_get(uint32 devId, uint32 entry, uint32 *pData)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_apdLut2_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_apdLut2_get(devId, entry, pData);
    RTK_API_UNLOCK();

    return ret;
}

int32 rtk_ldd_mpd02_get(uint32 devId, uint16 count, uint32 *value)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_mpd02_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_mpd02_get(devId, count, value);
    RTK_API_UNLOCK();

    return ret;
}

int32 rtk_ldd_temperature2_get(uint32 devId, uint32 *temp)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_temperature2_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_temperature2_get(devId, temp);
    RTK_API_UNLOCK();

    return ret;
}

int32 rtk_ldd_tx_bias2_get(uint32 devId, uint32 *bias)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_tx_bias2_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_tx_bias2_get(devId, bias);
    RTK_API_UNLOCK();

    return ret;
}

int32 rtk_ldd_tx_mod2_get(uint32 devId, uint32 *mod)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_tx_mod2_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_tx_mod2_get(devId, mod);
    RTK_API_UNLOCK();

    return ret;
}

int32 rtk_ldd_tx_bias2_set(uint32 devId, uint32 bias)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_tx_bias2_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_tx_bias2_set(devId, bias);
    RTK_API_UNLOCK();

    return ret;
}

int32 rtk_ldd_tx_mod2_set(uint32 devId, uint32 mod)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_tx_mod2_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_tx_mod2_set(devId, mod);
    RTK_API_UNLOCK();

    return ret;
}

int32 rtk_ldd_txCross2_set(uint32 devId, rtk_enable_t enable, uint32 sign, uint32 str)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_txCross2_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_txCross2_set(devId, enable, sign, str);
    RTK_API_UNLOCK();

    return ret;
}

int32 rtk_ldd_txCross2_get(uint32 devId, rtk_enable_t *pEnable, uint32 *pSign, uint32 *pStr)
{
    int32 ret;

    if (NULL == pLddMapper->ldd_txCross2_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_txCross2_get(devId, pEnable, pSign, pStr);
    RTK_API_UNLOCK();

    return ret;
}


int32 rtk_ldd_apcIavg2_set(uint32 devId, uint32 value)
{
    int32 ret;

    if (NULL == pLddMapper->ldd_apcIavg2_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    
    RTK_API_LOCK();
    ret = pLddMapper->ldd_apcIavg2_set(devId, value);
    RTK_API_UNLOCK();
    
    return ret;
}


int32 rtk_ldd_apcIavg2_get(uint32 devId, uint32 *pValue)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_apcIavg2_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_apcIavg2_get(devId, pValue);
    RTK_API_UNLOCK();

    return ret;
}

int32 rtk_ldd_apcEr2_set(uint32 devId, uint32 value)
{
    int32 ret;

    if (NULL == pLddMapper->ldd_apcEr2_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    
    RTK_API_LOCK();
    ret = pLddMapper->ldd_apcEr2_set(devId, value);
    RTK_API_UNLOCK();
    
    return ret;
}


int32 rtk_ldd_apcEr2_get(uint32 devId, uint32 *pValue)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_apcEr2_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_apcEr2_get(devId, pValue);
    RTK_API_UNLOCK();

    return ret;
}

int32 rtk_ldd_apcErTrim2_set(uint32 devId, uint32 value)
{
    int32 ret;

    if (NULL == pLddMapper->ldd_apcErTrim2_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    
    RTK_API_LOCK();
    ret = pLddMapper->ldd_apcErTrim2_set(devId, value);
    RTK_API_UNLOCK();
    
    return ret;
}


int32 rtk_ldd_apcErTrim2_get(uint32 devId, uint32 *pValue)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_apcErTrim2_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_apcErTrim2_get(devId, pValue);
    RTK_API_UNLOCK();

    return ret;
}

int32 rtk_ldd_rxlosRefDac2_set(uint32 devId, uint32 value)
{
    int32 ret;

    if (NULL == pLddMapper->ldd_rxlosRefDac2_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    
    RTK_API_LOCK();
    ret = pLddMapper->ldd_rxlosRefDac2_set(devId, value);
    RTK_API_UNLOCK();
    
    return ret;
}


int32 rtk_ldd_rxlosRefDac2_get(uint32 devId, uint32 *pValue)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_rxlosRefDac2_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_rxlosRefDac2_get(devId, pValue);
    RTK_API_UNLOCK();

    return ret;
}


int32 rtk_ldd_rxlosHystSel2_set(uint32 devId, uint32 value)
{
    int32 ret;

    if (NULL == pLddMapper->ldd_rxlosHystSel2_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    
    RTK_API_LOCK();
    ret = pLddMapper->ldd_rxlosHystSel2_set(devId, value);
    RTK_API_UNLOCK();
    
    return ret;
}


int32 rtk_ldd_rxlosHystSel2_get(uint32 devId, uint32 *pValue)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_rxlosHystSel2_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_rxlosHystSel2_get(devId, pValue);
    RTK_API_UNLOCK();

    return ret;
}

int32 rtk_ldd_sdadc_code2_get(uint32 devId, uint32 sdadc_type, uint32 *code_data)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_sdadc_code2_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_sdadc_code2_get(devId, sdadc_type, code_data);
    RTK_API_UNLOCK();

    
    return RT_ERR_OK;
}

int32 rtk_ldd_efuse2_set(uint32 devId, uint32 entry, uint32 data)
{
    int32 ret;

    if (NULL == pLddMapper->ldd_efuse2_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    
    RTK_API_LOCK();
    ret = pLddMapper->ldd_efuse2_set(devId, entry, data);
    RTK_API_UNLOCK();
    
    return ret;
}


int32 rtk_ldd_efuse2_get(uint32 devId, uint32 entry, uint32 *pData)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_efuse2_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_efuse2_get(devId, entry, pData);
    RTK_API_UNLOCK();

    return ret;
}

int32 rtk_ldd_vdd2_get(uint32 devId, uint32 *vdd)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_vdd2_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_vdd2_get(devId, vdd);
    RTK_API_UNLOCK();

    return ret;
}


int32 rtk_ldd_chip_init2(uint32 devId)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_chip_init2)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_chip_init2(devId);
    RTK_API_UNLOCK();

    return ret;
}

int32 rtk_ldd_rx_power2_get(uint32 devId, rtk_ldd_rxpwr_t *pLddRxPwr)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_rx_power2_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_rx_power2_get(devId, pLddRxPwr);
    RTK_API_UNLOCK();

    return ret;
}

int32 rtk_ldd_tx_power2_get(uint32 devId, rtk_ldd_txpwr_t *pLddTxPwr)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_tx_power2_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_tx_power2_get(devId, pLddTxPwr);
    RTK_API_UNLOCK();

    return ret;
}

int32 rtk_ldd_config2_refresh(uint32 devId)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_config2_refresh)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_config2_refresh(devId);
    RTK_API_UNLOCK();

    return ret;
}

int32 rtk_ldd_regBit_set(uint32 devId, uint32 reg, uint32 rBit, uint32 value)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_regBit_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_regBit_set(devId, reg, rBit, value);
    RTK_API_UNLOCK();

    return ret;
}

int32 rtk_ldd_regBit_get(uint32 devId, uint32 reg, uint32 rBit, uint32 *pValue)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_regBit_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_regBit_get(devId, reg, rBit, pValue);
    RTK_API_UNLOCK();

    //osal_printf("\n%s: %d  addr = 0x%x rBit = 0x%x, data = 0x%x, ret = %d \n",__FUNCTION__, __LINE__, reg, rBit, *pValue, ret); 

    return ret;
}

int32 rtk_ldd_regBits_set(uint32 devId, uint32 reg, uint32 rBits, uint32 value)
{
	int32	ret;
	
	if (NULL == pLddMapper->ldd_regBits_set)
		return RT_ERR_DRIVER_NOT_FOUND;
	
	RTK_API_LOCK();
	ret = pLddMapper->ldd_regBits_set(devId, reg, rBits, value);
	RTK_API_UNLOCK();
	
	return ret;
}


int32 rtk_ldd_regBits_get(uint32 devId, uint32 reg, uint32 rBits, uint32 *pValue)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_regBits_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    //osal_printf("\n%s: %d\n",__FUNCTION__, __LINE__); 

    RTK_API_LOCK();
    ret = pLddMapper->ldd_regBits_get(devId, reg, rBits, pValue);
    RTK_API_UNLOCK();

    return ret;
}

int32 rtk_ldd_txEsConfig_set(uint32 devId, uint32 mode, uint32 value)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_txEsConfig_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_txEsConfig_set(devId, mode, value);
    RTK_API_UNLOCK();

    
    return RT_ERR_OK;
}

int32 rtk_ldd_txEsConfig_get(uint32 devId, uint32 mode, uint32 *pValue)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_txEsConfig_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_txEsConfig_get(devId, mode, pValue);
    RTK_API_UNLOCK();

    
    return RT_ERR_OK;
}

int32 rtk_ldd_dac_set(uint32 devId, uint32 dac_offset, uint32 dac_code)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_dac_set)
        return RT_ERR_DRIVER_NOT_FOUND;

	//osal_printf("\n%s: %d  dac_offset = 0x%x dac_code = 0x%x \n",__FUNCTION__, __LINE__, dac_offset, dac_code);

    RTK_API_LOCK();
    ret = pLddMapper->ldd_dac_set(devId, dac_offset, dac_code);
    RTK_API_UNLOCK();

    
    return RT_ERR_OK;
}

int32 rtk_ldd_dac_get(uint32 devId, uint32 *pDac_offset, uint32 *pDac_code)
{
    int32   ret;

    if (NULL == pLddMapper->ldd_dac_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = pLddMapper->ldd_dac_get(devId, pDac_offset, pDac_code);
    RTK_API_UNLOCK();
    
	//osal_printf("\n%s: %d  dac_offset = 0x%x dac_code = 0x%x \n",__FUNCTION__, __LINE__, *pDac_offset, *pDac_code);
   
    return RT_ERR_OK;
}

