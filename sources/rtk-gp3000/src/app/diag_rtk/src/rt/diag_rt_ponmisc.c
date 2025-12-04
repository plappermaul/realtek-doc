/*
 * Copyright (C) 2019 Realtek Semiconductor Corp.
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
 * Purpose : Definition of PON misc API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) PON misc
 *
 */


#include <stdio.h>
#include <string.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <diag_util.h>
#include <diag_str.h>
#include <parser/cparser_priv.h>

#include <rtk/rt/rt_ponmisc.h>

/*
 * rt_ponmisc init
 */
cparser_result_t
cparser_cmd_rt_ponmisc_init(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rt_ponmisc_init(), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_ponmisc_init */

/*
 * rt_ponmisc get mode
 */
cparser_result_t
cparser_cmd_rt_ponmisc_get_mode(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rt_ponmisc_ponMode_t ponMode;
    rt_ponmisc_ponSpeed_t ponSpeed;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rt_ponmisc_modeSpeed_get(&ponMode,&ponSpeed), ret);

    diag_util_mprintf("============================================================\r\n");
    diag_util_mprintf(" PON MODE : ");
    if(ponMode == RT_GPON_MODE)
    {
        if(ponSpeed == RT_1G25G_SPEED)
            diag_util_mprintf("GPON");
        else if(ponSpeed == RT_BOTH2DOT5G_SPEED)
            diag_util_mprintf("GPON 2.5G US");
        else if(ponSpeed == RT_DN10G_SPEED)
            diag_util_mprintf("XG-PON");
        else if(ponSpeed == RT_BOTH10G_SPEED)
            diag_util_mprintf("XGS-PON");
    }
    else if(ponMode == RT_EPON_MODE)
    {
        if(ponSpeed == RT_1G25G_SPEED)
            diag_util_mprintf("EPON");
        else if(ponSpeed == RT_DN10G_SPEED)
            diag_util_mprintf("10G/1G EPON");
        else if(ponSpeed == RT_BOTH10G_SPEED)
            diag_util_mprintf("10G/10G EPON");
    }
    else if(ponMode == RT_NGPON2_MODE)
    {
        if(ponSpeed == RT_DN10G_SPEED)
            diag_util_mprintf("NG-PON2 10G/2.5G");
        else if(ponSpeed == RT_BOTH10G_SPEED)
            diag_util_mprintf("NG-PON2 10G/10G");
    }
    else if((ponMode >= RT_XFI_MODE)&&(ponMode <= RT_USXGMII_MODE))
    {
        if(ponMode == RT_XFI_MODE)
            diag_util_mprintf("XFI : ");
        else if(ponMode == RT_FIBER_MODE)
            diag_util_mprintf("FIBER : ");
        else if(ponMode == RT_SGMII_MODE)
            diag_util_mprintf("SGMII : ");
        else if(ponMode == RT_USXGMII_MODE)
            diag_util_mprintf("USXGMII : ");
        else 
        {
            diag_util_mprintf("Not support\r\n");
            diag_util_mprintf("============================================================\r\n");
            return CPARSER_OK;
        }

        if(ponSpeed == RT_10M_SPEED)
            diag_util_mprintf("10M");
        else if(ponSpeed == RT_100M_SPEED)
            diag_util_mprintf("100M");
        else if(ponSpeed == RT_500M_SPEED)
            diag_util_mprintf("500M");
        else if(ponSpeed == RT_1G_SPEED)
            diag_util_mprintf("1G");
        else if(ponSpeed == RT_2DOT5G_SPEED)
            diag_util_mprintf("2.5G");
        else if(ponSpeed == RT_5G_SPEED)
            diag_util_mprintf("5G");
        else if(ponSpeed == RT_10G_SPEED)
            diag_util_mprintf("10G");
        else
            diag_util_mprintf("Not support");
    }
    else
        diag_util_mprintf("Not support");

    diag_util_mprintf("\r\n");
    diag_util_mprintf("============================================================\r\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_ponmisc_get_mode */

/*
 * rt_ponmisc set mode <UINT:mode> speed <UINT:speed>
 */
cparser_result_t
cparser_cmd_rt_ponmisc_set_mode_mode_speed_speed(
    cparser_context_t *context,
    uint32_t  *mode_ptr,
    uint32_t  *speed_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rt_ponmisc_ponMode_t mode;
    rt_ponmisc_ponSpeed_t speed;

    DIAG_UTIL_PARAM_CHK();

    mode = *mode_ptr;
    speed = *speed_ptr;

    DIAG_UTIL_ERR_CHK(rt_ponmisc_modeSpeed_set(mode,speed), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_ponmisc_set_mode_mode_speed_speed */

/*
 * rt_ponmisc get sid <INT:sid>
 */
cparser_result_t
cparser_cmd_rt_ponmisc_get_sid_sid(
    cparser_context_t *context,
    int32_t  *sid_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rt_ponmisc_sidInfo_t sidInfo;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rt_ponmisc_sid_get(*sid_ptr,&sidInfo), ret);

    if(sidInfo.enFlag >= RTK_ENABLE_END || sidInfo.dqFlag >= RTK_ENABLE_END)
        return CPARSER_OK;

    diag_util_mprintf("sid = %d\n",*sid_ptr);
    diag_util_mprintf("state = %s\n",diagStr_enable[sidInfo.enFlag]);
    diag_util_mprintf("dstPort = %d\n",sidInfo.dstPort);
    diag_util_mprintf("priority = %d\n",sidInfo.pri);
    diag_util_mprintf("flowId = %d\n",sidInfo.flowId);
    diag_util_mprintf("deepqueue = %s\n",diagStr_enable[sidInfo.dqFlag]);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_ponmisc_get_sid_sid */

/*
 * rt_ponmisc set sid <INT:sid> state ( disable | enable ) dpn <INT:dpn> pri <INT:pri> flowid <INT:flowid> dq ( disable | enable ) */
cparser_result_t
cparser_cmd_rt_ponmisc_set_sid_sid_state_disable_enable_dpn_dpn_pri_pri_flowid_flowid_dq_disable_enable(
    cparser_context_t *context,
    int32_t  *sid_ptr,
    int32_t  *dpn_ptr,
    int32_t  *pri_ptr,
    int32_t  *flowid_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rt_ponmisc_sidInfo_t sidInfo;

    DIAG_UTIL_PARAM_CHK();

    if('e' == TOKEN_CHAR(5, 0))
        sidInfo.enFlag = ENABLED;
    else
        sidInfo.enFlag = DISABLED;

    sidInfo.dstPort = *dpn_ptr;
    sidInfo.pri = *pri_ptr;
    sidInfo.flowId = *flowid_ptr;

    if('e' == TOKEN_CHAR(13, 0))
        sidInfo.dqFlag = ENABLED;
    else
        sidInfo.dqFlag = DISABLED;

    DIAG_UTIL_ERR_CHK(rt_ponmisc_sid_set(*sid_ptr,sidInfo), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_ponmisc_set_sid_sid_state_disable_enable_dpn_dpn_pri_pri_flowid_flowid_dq_disable_enable */

/*
 * rt_ponmisc get transceiver ( vendor-name | sn | part-number | temperature | voltage | bias-current | tx-power | rx-power )
 */
cparser_result_t
cparser_cmd_rt_ponmisc_get_transceiver_vendor_name_sn_part_number_temperature_voltage_bias_current_tx_power_rx_power(
    cparser_context_t *context)
{
    rt_transceiver_data_t dataCfg, readableCfg;
    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    memset(&readableCfg, 0, sizeof(rt_transceiver_data_t));
    readableCfg.buf[0] = '\0';

    if(0 == osal_strcmp(TOKEN_STR(3), "vendor-name"))
    {
        DIAG_UTIL_ERR_CHK(rt_ponmisc_transceiver_get(RT_TRANSCEIVER_PARA_TYPE_VENDOR_NAME, &dataCfg), ret);
        _get_data_by_type(RT_TRANSCEIVER_PARA_TYPE_VENDOR_NAME, (rtk_transceiver_data_t *)&dataCfg, (rtk_transceiver_data_t *)&readableCfg);
        diag_util_mprintf("Vendor Name: ");
    }else if(0 == osal_strcmp(TOKEN_STR(3), "sn"))
    {
        DIAG_UTIL_ERR_CHK(rt_ponmisc_transceiver_get(RT_TRANSCEIVER_PARA_TYPE_SN, &dataCfg), ret);
        _get_data_by_type(RT_TRANSCEIVER_PARA_TYPE_SN, (rtk_transceiver_data_t *)&dataCfg, (rtk_transceiver_data_t *)&readableCfg);
        diag_util_mprintf("SN: ");
    }else if (0 == osal_strcmp(TOKEN_STR(3), "part-number"))
    {
        DIAG_UTIL_ERR_CHK(rt_ponmisc_transceiver_get(RT_TRANSCEIVER_PARA_TYPE_VENDOR_PART_NUM, &dataCfg), ret);
        _get_data_by_type(RT_TRANSCEIVER_PARA_TYPE_VENDOR_PART_NUM, (rtk_transceiver_data_t *)&dataCfg, (rtk_transceiver_data_t *)&readableCfg);
        diag_util_mprintf("Part Number: ");
    }else if (0 == osal_strcmp(TOKEN_STR(3), "temperature"))
    {
        DIAG_UTIL_ERR_CHK(rt_ponmisc_transceiver_get(RT_TRANSCEIVER_PARA_TYPE_TEMPERATURE, &dataCfg), ret);
        _get_data_by_type(RT_TRANSCEIVER_PARA_TYPE_TEMPERATURE, (rtk_transceiver_data_t *)&dataCfg, (rtk_transceiver_data_t *)&readableCfg);
        diag_util_mprintf("Temperature: ");
    }else if (0 == osal_strcmp(TOKEN_STR(3), "voltage"))
    {
        DIAG_UTIL_ERR_CHK(rt_ponmisc_transceiver_get(RT_TRANSCEIVER_PARA_TYPE_VOLTAGE, &dataCfg), ret);
        _get_data_by_type(RT_TRANSCEIVER_PARA_TYPE_VOLTAGE, (rtk_transceiver_data_t *)&dataCfg, (rtk_transceiver_data_t *)&readableCfg);
        diag_util_mprintf("Voltage: ");
    }else if (0 == osal_strcmp(TOKEN_STR(3), "bias-current"))
    {
        DIAG_UTIL_ERR_CHK(rt_ponmisc_transceiver_get(RT_TRANSCEIVER_PARA_TYPE_BIAS_CURRENT, &dataCfg), ret);
        _get_data_by_type(RT_TRANSCEIVER_PARA_TYPE_BIAS_CURRENT, (rtk_transceiver_data_t *)&dataCfg, (rtk_transceiver_data_t *)&readableCfg);
        diag_util_mprintf("Bias Current: ");
    }else if (0 == osal_strcmp(TOKEN_STR(3), "tx-power"))
    {
        DIAG_UTIL_ERR_CHK(rt_ponmisc_transceiver_get(RT_TRANSCEIVER_PARA_TYPE_TX_POWER, &dataCfg), ret);
        _get_data_by_type(RT_TRANSCEIVER_PARA_TYPE_TX_POWER, (rtk_transceiver_data_t *)&dataCfg, (rtk_transceiver_data_t *)&readableCfg);
        diag_util_mprintf("Tx Power: ");
    }else if (0 == osal_strcmp(TOKEN_STR(3), "rx-power"))
    {
        DIAG_UTIL_ERR_CHK(rt_ponmisc_transceiver_get(RT_TRANSCEIVER_PARA_TYPE_RX_POWER, &dataCfg), ret);
        _get_data_by_type(RT_TRANSCEIVER_PARA_TYPE_RX_POWER, (rtk_transceiver_data_t *)&dataCfg, (rtk_transceiver_data_t *)&readableCfg);
        diag_util_mprintf("Rx Power: ");
    }

    diag_util_mprintf("%s\n", readableCfg.buf);
    return CPARSER_OK;
}    /* end of cparser_cmd_rt_ponmisc_get_transceiver_vendor_name_sn_part_number_temperature_voltage_bias_current_tx_power_rx_power */

/*
* rt_ponmsic set transceiver tx-power scale <UINT:scale>
*/
cparser_result_t
cparser_cmd_rt_ponmisc_set_transceiver_tx_power_scale_scale(
    cparser_context_t *context,
    uint32_t  *scale_ptr)
{

    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();
       
    DIAG_UTIL_ERR_CHK(rt_ponmisc_transceiver_tx_power_scale_set(*scale_ptr), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_ponmisc_set_transceiver_tx_power_scale_scale */

/*
 * rt_ponmisc get burstPolarityReverse state
 */
cparser_result_t
cparser_cmd_rt_ponmisc_get_burstPolarityReverse_state(
    cparser_context_t *context)
{
    rt_ponmisc_polarity_t enable;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rt_ponmisc_burstPolarityReverse_get(&enable), ret);
    diag_util_printf("state:%s\n",diagStr_enable[enable]);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_ponmisc_get_burstpolarityreverse_state */

/*
 * rt_ponmisc set burstPolarityReverse state ( disable | enable )
 */
cparser_result_t
cparser_cmd_rt_ponmisc_set_burstPolarityReverse_state_disable_enable(
    cparser_context_t *context)
{
    rt_ponmisc_polarity_t enable;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    if('e'==TOKEN_CHAR(4,0))
        enable = RT_PONMISC_POLARITY_REVERSE_ON;
    else
        enable = RT_PONMISC_POLARITY_REVERSE_OFF;

    DIAG_UTIL_ERR_CHK(rt_ponmisc_burstPolarityReverse_set(enable), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_ponmisc_set_burstpolarityreverse_state_disable_enable */

/*
 * rt_ponmisc get rxlosPolarityReverse state
 */
cparser_result_t
cparser_cmd_rt_ponmisc_get_rxlosPolarityReverse_state(
    cparser_context_t *context)
{
    rt_ponmisc_polarity_t enable;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rt_ponmisc_rxlosPolarityReverse_get(&enable), ret);
    diag_util_printf("state:%s\n",diagStr_enable[enable]);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_ponmisc_get_rxlospolarityreverse_state */

/*
 * rt_ponmisc set rxlosPolarityReverse state ( disable | enable )
 */
cparser_result_t
cparser_cmd_rt_ponmisc_set_rxlosPolarityReverse_state_disable_enable(
    cparser_context_t *context)
{
    rt_ponmisc_polarity_t enable;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    if('e'==TOKEN_CHAR(4,0))
        enable = RT_PONMISC_POLARITY_REVERSE_ON;
    else
        enable = RT_PONMISC_POLARITY_REVERSE_OFF;

    DIAG_UTIL_ERR_CHK(rt_ponmisc_rxlosPolarityReverse_set(enable), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_ponmisc_set_rxlospolarityreverse_state_disable_enable */


/*
 * rt_ponmisc get force-laser-on state
 */
cparser_result_t
cparser_cmd_rt_ponmisc_get_force_laser_on_state(
    cparser_context_t *context)
{
    rt_ponmisc_laser_status_t state;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rt_ponmisc_forceLaserState_get(&state), ret);

    diag_util_mprintf("Laser status:");
    if(RT_PONMISC_LASER_STATUS_NORMAL==state)
        diag_util_mprintf("normal\n");
    else if(RT_PONMISC_LASER_STATUS_FORCE_ON==state)
        diag_util_mprintf("force-on\n");
    else
        diag_util_mprintf("force-off\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_ponmisc_get_force_laser_on_state */

/*
 * rt_ponmisc set force-laser-on state ( disable | enable )
 */
cparser_result_t
cparser_cmd_rt_ponmisc_set_force_laser_on_state_disable_enable(
    cparser_context_t *context)
{
    rt_ponmisc_laser_status_t state;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    if('e'==TOKEN_CHAR(4,0))
        state = RT_PONMISC_LASER_STATUS_FORCE_ON;
    else
        state = RT_PONMISC_LASER_STATUS_NORMAL;

    DIAG_UTIL_ERR_CHK(rt_ponmisc_forceLaserState_set(state), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_ponmisc_set_force_laser_on_state_disable_enable */

/*
 * rt_ponmisc get force-prbs
 */
cparser_result_t
cparser_cmd_rt_ponmisc_get_force_prbs(
    cparser_context_t *context)
{
    rt_ponmisc_prbs_t prbsCfg;
    int32 ret = RT_ERR_FAILED;
    char* prbsStr [] = {
        "OFF",
        "prbs31",
        "prbs23",
        "prbs15",
        "prbs11",
        "prbs9",
        "prbs7",
        "prbs3"
    };

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    memset (&prbsCfg, 0x00, sizeof (rt_ponmisc_prbs_t));
    DIAG_UTIL_ERR_CHK(rt_ponmisc_forcePRBS_get(&prbsCfg), ret);
    diag_util_printf("PRBS MODE:%s\n", prbsStr[prbsCfg]);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_ponmisc_get_force_prbs */

/*
 * rt_ponmisc set force-prbs ( off | prbs31 | prbs23 | prbs15 | prbs11 | prbs09 | prbs07 | prbs03 )
 */
cparser_result_t
cparser_cmd_rt_ponmisc_set_force_prbs_off_prbs31_prbs23_prbs15_prbs11_prbs09_prbs07_prbs03(
    cparser_context_t *context)
{
    rt_ponmisc_prbs_t prbsCfg;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    if (0 == strcmp("off",TOKEN_STR(3)))
        prbsCfg = RT_PONMISC_PRBS_OFF;
    else if (0 == strcmp("prbs31",TOKEN_STR(3)))
        prbsCfg = RT_PONMISC_PRBS_31;
    else if (0 == strcmp("prbs23",TOKEN_STR(3)))
        prbsCfg = RT_PONMISC_PRBS_23;
    else if (0 == strcmp("prbs15",TOKEN_STR(3)))
        prbsCfg = RT_PONMISC_PRBS_15;
    else if (0 == strcmp("prbs11", TOKEN_STR(3)))
        prbsCfg = RT_PONMISC_PRBS_11;
    else if (0 == strcmp("prbs09", TOKEN_STR(3)))
        prbsCfg = RT_PONMISC_PRBS_9;
    else if (0 == strcmp("prbs07",TOKEN_STR(3)))
        prbsCfg = RT_PONMISC_PRBS_7;
    else if (0 == strcmp("prbs03",TOKEN_STR(3)))
        prbsCfg = RT_PONMISC_PRBS_3;
    else
	    return CPARSER_ERR_INVALID_PARAMS;

    DIAG_UTIL_ERR_CHK(rt_ponmisc_forcePRBS_set(prbsCfg), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_ponmisc_set_force_prbs_off_prbs31_prbs23_prbs15_prbs11_prbs09_prbs07_prbs03 */

/*
 * rt_ponmisc get force-prbs-rx
 */
cparser_result_t
cparser_cmd_rt_ponmisc_get_force_prbs_rx(
    cparser_context_t *context)
{
    rt_ponmisc_prbs_t prbsRxCfg;
    int32 ret = RT_ERR_FAILED;
    char* prbsStr [] = {
        "OFF",
        "prbs31",
        "prbs23",
        "prbs15",
        "prbs11",
        "prbs9",
        "prbs7",
        "prbs3"
    };

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    memset (&prbsRxCfg, 0x00, sizeof (rt_ponmisc_prbs_t));
    DIAG_UTIL_ERR_CHK(rt_ponmisc_forcePRBS_rx_get(&prbsRxCfg), ret);
    diag_util_printf("PRBS RX MODE:%s\n", prbsStr[prbsRxCfg]);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_ponmisc_get_force_prbs */

/*
 * rt_ponmisc set force-prbs-rx ( off | prbs31 | prbs23 | prbs15 | prbs11 | prbs09 | prbs07 | prbs03 )
 */
cparser_result_t
cparser_cmd_rt_ponmisc_set_force_prbs_rx_off_prbs31_prbs23_prbs15_prbs11_prbs09_prbs07_prbs03(
    cparser_context_t *context)
{
    rt_ponmisc_prbs_t prbsRxCfg;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    if (0 == strcmp("off",TOKEN_STR(3)))
        prbsRxCfg = RT_PONMISC_PRBS_OFF;
    else if (0 == strcmp("prbs31",TOKEN_STR(3)))
        prbsRxCfg = RT_PONMISC_PRBS_31;
    else if (0 == strcmp("prbs23",TOKEN_STR(3)))
        prbsRxCfg = RT_PONMISC_PRBS_23;
    else if (0 == strcmp("prbs15",TOKEN_STR(3)))
        prbsRxCfg = RT_PONMISC_PRBS_15;
    else if (0 == strcmp("prbs11", TOKEN_STR(3)))
        prbsRxCfg = RT_PONMISC_PRBS_11;
    else if (0 == strcmp("prbs09", TOKEN_STR(3)))
        prbsRxCfg = RT_PONMISC_PRBS_9;
    else if (0 == strcmp("prbs07",TOKEN_STR(3)))
        prbsRxCfg = RT_PONMISC_PRBS_7;
    else if (0 == strcmp("prbs03",TOKEN_STR(3)))
        prbsRxCfg = RT_PONMISC_PRBS_3;
    else
	    return CPARSER_ERR_INVALID_PARAMS;

    DIAG_UTIL_ERR_CHK(rt_ponmisc_forcePRBS_rx_set(prbsRxCfg), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_ponmisc_set_force_prbs_off_prbs31_prbs23_prbs15_prbs11_prbs09_prbs07_prbs03 */

/*
 * rt_ponmisc get prbs-rx-status
 */
cparser_result_t
cparser_cmd_rt_ponmisc_get_prbs_rx_status(
    cparser_context_t *context)
{
    rt_ponmisc_prbsRxStatus_t prbsRxStatus;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    memset (&prbsRxStatus, 0x00, sizeof (rt_ponmisc_prbsRxStatus_t));
    DIAG_UTIL_ERR_CHK(rt_ponmisc_prbsRxStatus_get(&prbsRxStatus), ret);

    if(prbsRxStatus.prbsRxState == RT_PONMISC_PRBS_RX_READY)
    {
        diag_util_printf("PRBS RX Error Count: %u\n", prbsRxStatus.prbsRxErrCnt);
    }
    else if(prbsRxStatus.prbsRxState == RT_PONMISC_PRBS_RX_LOS)
    {
        diag_util_printf("PRBS RX State: LOS!\n");
    }
    else if(prbsRxStatus.prbsRxState == RT_PONMISC_PRBS_RX_NOTAVAILABLE)
    {
        diag_util_printf("PRBS RX State: Not Available!\n");
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_ponmisc_get_prbs_rx_status */


#define BIT_RATE_125M  0
#define BIT_RATE_1D25G  1
#define BIT_RATE_2D488G  2
#define BIT_RATE_3D125G  3
#define BIT_RATE_9D95328G  4
#define BIT_RATE_10D3125G  5
#define GIGA_BITS (1000000000)

double bit_rate[6] = {0.125, 1.25, 2.488, 3.125, 9.95328, 10.3125};  //{100M, 1G, 2.4G, 2.5G, 9.8G, 10G}
double pass_criteria[6] = {0.000000000001, 0.000000000001, 0.0000000001, 0.0001, 0.001, 0.001};  //{100M, 1G, 2.4G, 2.5G, 9.8G, 10G}
/*
 * rt_ponmisc get prbs-rx-ber
 */
cparser_result_t
cparser_cmd_rt_ponmisc_get_prbs_rx_ber_timeout(
    cparser_context_t *context,
    int32_t  *timeout_ptr)
{
    rt_ponmisc_ponMode_t ponMode = RT_PONMODE_END;
    rt_ponmisc_ponSpeed_t ponSpeed = RT_PONSPEED_END;
    rt_ponmisc_prbsRxStatus_t prbsRxStatus;
    int32 ret = RT_ERR_FAILED;
    uint32_t ds_rate = 0; 
    double bit_error = 0; 
    double bit_error_rate = 0;
    uint32_t i = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rt_ponmisc_modeSpeed_get(&ponMode,&ponSpeed), ret);

    if(ponMode == RT_GPON_MODE)
    {
        if((ponSpeed == RT_1G25G_SPEED) || (ponSpeed == RT_BOTH2DOT5G_SPEED))
            ds_rate = BIT_RATE_2D488G;
        else
            ds_rate = BIT_RATE_9D95328G;
    }
    else if(ponMode == RT_EPON_MODE)
    {
        if(ponSpeed == RT_1G25G_SPEED)
            ds_rate = BIT_RATE_1D25G;
        else
            ds_rate = BIT_RATE_10D3125G;
    }
    else if(ponMode == RT_NGPON2_MODE)
    {
        ds_rate = BIT_RATE_9D95328G;
    }
    else if((ponMode >= RT_XFI_MODE)&&(ponMode <= RT_USXGMII_MODE))
    {
        if(ponSpeed == RT_100M_SPEED)
            ds_rate = BIT_RATE_125M;
        else if(ponSpeed == RT_1G_SPEED)
            ds_rate = BIT_RATE_1D25G;
        else if(ponSpeed == RT_2DOT5G_SPEED)
            ds_rate = BIT_RATE_3D125G;
        else if(ponSpeed == RT_10G_SPEED)
            ds_rate = BIT_RATE_10D3125G;
        else
        {
            diag_util_mprintf("Not supported speed\r\n");
            return CPARSER_OK;
        }
    }
    else
    {
        diag_util_mprintf("Not supported mode\r\n");
        return CPARSER_OK;
    }

    memset (&prbsRxStatus, 0x00, sizeof (rt_ponmisc_prbsRxStatus_t));
    DIAG_UTIL_ERR_CHK(rt_ponmisc_prbsRxStatus_get(&prbsRxStatus), ret);

    if(prbsRxStatus.prbsRxState == RT_PONMISC_PRBS_RX_READY)
    {
        diag_util_printf("PRBS RX BER Calculation START(%ds, Ctrl+z to stop): ", *timeout_ptr);
    }
    else if(prbsRxStatus.prbsRxState == RT_PONMISC_PRBS_RX_LOS)
    {
        diag_util_printf("PRBS RX State: LOS!\n");
        return CPARSER_OK;
    }
    else if(prbsRxStatus.prbsRxState == RT_PONMISC_PRBS_RX_NOTAVAILABLE)
    {
        diag_util_printf("PRBS RX State: Not Available!\n");
        return CPARSER_OK;
    }

    memset (&prbsRxStatus, 0x00, sizeof (rt_ponmisc_prbsRxStatus_t));
    DIAG_UTIL_ERR_CHK(rt_ponmisc_prbsRxStatus_get(&prbsRxStatus), ret);
    sleep(1);
    for(i = 0; i < *timeout_ptr; i++)
    {
        memset (&prbsRxStatus, 0x00, sizeof (rt_ponmisc_prbsRxStatus_t));
        DIAG_UTIL_ERR_CHK(rt_ponmisc_prbsRxStatus_get(&prbsRxStatus), ret);
        if(prbsRxStatus.prbsRxState == RT_PONMISC_PRBS_RX_LOS)
        {
            diag_util_printf("PRBS RX State: LOS!\n");
            return CPARSER_OK;
        }
        else if(prbsRxStatus.prbsRxState == RT_PONMISC_PRBS_RX_NOTAVAILABLE)
        {
            diag_util_printf("PRBS RX State: Not Available!\n");
            return CPARSER_OK;
        }
        else if((prbsRxStatus.prbsRxErrCnt == 0xFFFF) && (ponSpeed == RT_1G25G_SPEED) && (ponMode == RT_GPON_MODE)) /* 07c gpon mode support only 16 bit coutner */
        {
            diag_util_printf("Please check test environment \n(PRBS Rate, PRBS pattern type and RX Power) !\n");
            return CPARSER_OK;
        }
        else if((prbsRxStatus.prbsRxErrCnt > 6250000) && (ponSpeed == RT_1G25G_SPEED) && (ponMode == RT_EPON_MODE)) /* 07c epon mode, with wrong prbs pattern */
        {
            diag_util_printf("Please check test environment \n(PRBS Rate, PRBS pattern type and RX Power) !\n");
            return CPARSER_OK;
        }
        bit_error += prbsRxStatus.prbsRxErrCnt;
        diag_util_printf(".");
        sleep(1);
    }

    bit_error_rate = bit_error/(bit_rate[ds_rate]*(*timeout_ptr)*GIGA_BITS);
    diag_util_printf("\n\nPRBS RX BER Calculation DONE!!! \n");
    diag_util_printf("timeout = %d(s) \n", *timeout_ptr);
    diag_util_printf("bit error count = %15.0f \n", bit_error);
    diag_util_printf("total bit count = %15.0f \n", bit_rate[ds_rate]*(*timeout_ptr)*GIGA_BITS);
    diag_util_printf("BER = %.6e \n",bit_error_rate);
    diag_util_printf("Result = %s \n", (bit_error_rate > pass_criteria[ds_rate])? "FAIL":"PASS");

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_ponmisc_get_prbs_rx_ber_timeout */

/*
 * rt_ponmisc get self-rogue
 */
cparser_result_t
cparser_cmd_rt_ponmisc_get_self_rogue(
    cparser_context_t *context)
{
    rtk_enable_t rogueCfg;
    rt_ponmisc_rogue_cnt_t rogueCnt;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    memset (&rogueCfg, 0x00, sizeof (rtk_enable_t));
    DIAG_UTIL_ERR_CHK(rt_ponmisc_selfRogue_cfg_get(&rogueCfg), ret);

    memset (&rogueCnt, 0x00, sizeof (rt_ponmisc_rogue_cnt_t));
    DIAG_UTIL_ERR_CHK(rt_ponmisc_selfRogue_count_get(&rogueCnt), ret);

    diag_util_printf("Self rogue detect config state: %s\n", diagStr_enable[rogueCfg]);
    diag_util_printf("Self rogue detect mismatch cnt: %u\n", rogueCnt.rogue_mismatch);
    diag_util_printf("Self rogue detect toolong cnt: %u\n", rogueCnt.rogue_toolong);
    
    return CPARSER_OK;
}    /* end of cparser_cmd_rt_ponmisc_get_prbs_rx_status */

/*
 * rt_ponmisc set self-rogue state ( disable | enable )
 */
cparser_result_t
cparser_cmd_rt_ponmisc_set_self_rogue_state_disable_enable(
    cparser_context_t *context)
{
    rtk_enable_t rogueCfg;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    if('e'==TOKEN_CHAR(4,0))
        rogueCfg = ENABLED;
    else
        rogueCfg = DISABLED;

    DIAG_UTIL_ERR_CHK(rt_ponmisc_selfRogue_cfg_set(rogueCfg), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_ponmisc_get_prbs_rx_status */

/*
 * rt_ponmisc set self-rogue clear-cnt
 */
cparser_result_t
cparser_cmd_rt_ponmisc_set_self_rogue_clear_cnt(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rt_ponmisc_selfRogue_count_clear(), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_ponmisc_get_prbs_rx_status */

