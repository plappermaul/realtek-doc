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
 * $Revision: 40647 $
 * $Date: 2013-07-01 15:36:16 +0800 (Mon, 01 Jul 2013) $
 *
 * Purpose :
 *
 * Feature : 
 *
 */

#ifndef __EUROPA_CLI_H__
#define __EUROPA_CLI_H__

#define RTL8290B_PARAMETER_SIZE                               0x1000
#define RTL8290B_BLOCK_SIZE                                   0x100
#define RTL8290B_FILE_SIZE                                    0x1000
#define RTL8290B_FILE_LOCATION                                "/var/config/rtl8290b.data"


#define RTL8290B_FLASH_DATA_SIZE                              2000

#define RTL8290B_DDMI_A0_ADDR                                 0
#define RTL8290B_DDMI_A2_ADDR                                 0x100
#define RTL8290B_A4_ADDR                                      0x200

#define RTL8290B_A4_PATCH_LEN_ADDR                            512
#define RTL8290B_A4_PATCH_START_ADDR                          513

#define RTL8290B_APD_LUT_ADDR                                 0x300
#define RTL8290B_LASER_LUT_ADDR                               0x400
#define RTL8290B_DCL_P0_ADDR                                  1346
#define RTL8290B_DCL_P1_ADDR                                  1347
#define RTL8290B_DCL_PAVG_ADDR                                1348
#define RTL8290B_W57_APCDIG_DAC_BIAS_ADDR                     1349
#define RTL8290B_RX_A_ADDR                                    1350
#define RTL8290B_RX_B_ADDR                                    1354
#define RTL8290B_RX_C_ADDR                                    1358
#define RTL8290B_RSSI_V0_ADDR                                 1362
#define RTL8290B_MPD0_ADDR                                    1366
#define RTL8290B_CROSS_ADDR                                   1370
#define RTL8290B_TX_A_ADDR                                    1372
#define RTL8290B_TX_B_ADDR                                    1376
#define RTL8290B_TX_C_ADDR                                    1380
#define RTL8290B_TEMP_OFFSET_ADDR                             1384
#define RTL8290B_TEMP_INTR_SCALE_ADDR                         1385
#define RTL8290B_RX_TH_ADDR                                   1386
#define RTL8290B_RX_DETH_ADDR                                 1387
#define RTL8290B_GND_V0_ADDR                                  1388
#define RTL8290B_VDD_V0_ADDR                                  1392
#define RTL8290B_HALF_VDD_V0_ADDR                             1396
#define RTL8290B_RX_K_ADDR                                    1520
#define RTL8290B_MPD1_ADDR                                    1524


#define RTL8290B_PARAM_ADDR_BASE                                   1400
#define RTL8290B_BOSA_TYPE_OFFSET_ADDR                                0
#define RTL8290B_APC_LDO_OFFSET_ADDR                                  1
#define RTL8290B_TX_LDO_OFFSET_ADDR                                   2
#define RTL8290B_RX_LDO_OFFSET_ADDR                                   3
#define RTL8290B_TX_BLEED_EN_N_OFFSET_ADDR                            4
#define RTL8290B_TX_BLEED_EN_P_OFFSET_ADDR                            5
#define RTL8290B_TX_BACK_TERM_TYPE_OFFSET_ADDR                        6
#define RTL8290B_TX_BACK_TERM_VAL_OFFSET_ADDR                         7
#define RTL8290B_TX_INTERFACE_OFFSET_ADDR                             8
#define RTL8290B_TX_IN_TYPE_OPT_OFFSET_ADDR                           9
#define RTL8290B_TX_INT_RLPN_OFFSET_ADDR                             10
#define RTL8290B_TX_CROSS_EN_OFFSET_ADDR                             11
#define RTL8290B_TX_CROSS_SIGN_OFFSET_ADDR                           12
#define RTL8290B_TX_CROSS_STR_OFFSET_ADDR                            13
#define RTL8290B_TX_BIAS_DAC_OFFSET_OFFSET_ADDR                      14
#define RTL8290B_TX_MOD_DAC_OFFSET_OFFSET_ADDR                       15
#define RTL8290B_TX_IN_POL_SWAP_OFFSET_ADDR                          16
#define RTL8290B_TX_OUT_POL_SWAP_OFFSET_ADDR                         17
#define RTL8290B_APC_LASER_ON_DELAY_OFFSET_ADDR                      18
#define RTL8290B_APC_SETTLE_COUNT_OFFSET_ADDR                        19
#define RTL8290B_APC_IBIAS_INIT_OFFSET_ADDR                          20
#define RTL8290B_APC_IMOD_INIT_OFFSET_ADDR                           22
#define RTL8290B_APC_LOOP_MODE_OFFSET_ADDR                           24
#define RTL8290B_APC_LOOP_MODE_EX_OFFSET_ADDR                        25
#define RTL8290B_APC_IAVG_SET_OFFSET_ADDR                            26
#define RTL8290B_APC_ER_SET_OFFSET_ADDR                              27
#define RTL8290B_APC_ER_TRIM_OFFSET_ADDR                             28
#define RTL8290B_APC_CMPD_OFFSET_ADDR                                29
#define RTL8290B_APC_LPF_BW_OFFSET_ADDR                              30
#define RTL8290B_APC_LOOP_GAIN_OFFSET_ADDR                           31
#define RTL8290B_ERC_LOOP_GAIN_OFFSET_ADDR                           32
#define RTL8290B_ERC_CHOPPER_EN_OFFSET_ADDR                          33
#define RTL8290B_APC_CROSS_EN_OFFSET_ADDR                            34
#define RTL8290B_APC_CROSS_STR_OFFSET_ADDR                           35
#define RTL8290B_APC_MOD_MAX_OFFSET_ADDR                             36
#define RTL8290B_APC_BIAS_MAX_OFFSET_ADDR                            37
#define RTL8290B_APC_BIAS_MIN_OFFSET_ADDR                            38
#define RTL8290B_APC_PWRLVL_MAX_ER_OFFSET_ADDR                       39                                                             
#define RTL8290B_FSU_EBABLE_OFFSET_ADDR                              40
#define RTL8290B_FSU_MODE_OFFSET_ADDR                                41
#define RTL8290B_FSU_APC_LOOP_GAIN_OFFSET_ADDR                       42
#define RTL8290B_FSU_APC_RAMPB_OFFSET_ADDR                           43
#define RTL8290B_FSU_APC_RAMPM_OFFSET_ADDR                           44
#define RTL8290B_FSU_RST_COUNT_OFFSET_ADDR                           45
#define RTL8290B_FSU_SETTLE_COUNT_OFFSET_ADDR                        46
#define RTL8290B_FSU_ERC_LOOP_GAIN_OFFSET_ADDR                       47
#define RTL8290B_FSU_ERC_RAMPM_OFFSET_ADDR                           48                                                             
#define RTL8290B_RX_OUT_POL_SWAP_OFFSET_ADDR                         49
#define RTL8290B_RX_OUT_SWING_OFFSET_ADDR                            50
#define RTL8290B_RX_DIS_OFFCAN_OFFSET_ADDR                           51
#define RTL8290B_RX_SLEEP_PIN_OFFSET_ADDR                            52
#define RTL8290B_TXSD_MODE_OFFSET_ADDR                               53
#define RTL8290B_TXSD_TH_OFFSET_ADDR                                 54
#define RTL8290B_TXSD_TIA_GAIN_OFFSET_ADDR                           55
#define RTL8290B_TXSD_HIGH_LOOP_GAIN_OFFSET_ADDR                     56
#define RTL8290B_TXSD_LOW_LOOP_GAIN_OFFSET_ADDR                      57
#define RTL8290B_TXSD_ON_RST_COUNT_OFFSET_ADDR                       58
#define RTL8290B_TXSD_OFF_RST_COUNT_OFFSET_ADDR                      59
#define RTL8290B_TXDIS_CTL_OFFSET_ADDR                               60                                                          
#define RTL8290B_RXLOS_REF_DAC_OFFSET_ADDR                           61
#define RTL8290B_RXLOS_HYST_SEL_OFFSET_ADDR                          62
#define RTL8290B_RXLOS_SAMPLE_SEL_OFFSET_ADDR                        63
#define RTL8290B_RXLOS_RANGE_SEL_OFFSET_ADDR                         64
#define RTL8290B_RXLOS_INPUT_SEL_OFFSET_ADDR                         65
#define RTL8290B_RXLOS_DEBOUNCE_SEL_OFFSET_ADDR                      66
#define RTL8290B_RXLOS_CHOPPER_EN_OFFSET_ADDR                        67
#define RTL8290B_RXLOS_CHOPPER_FREQ_OFFSET_ADDR                      68
#define RTL8290B_RXLOS_LA_MAG_COMP_OFFSET_ADDR                       69
#define RTL8290B_RXLOS_BUF_AUTOZERO_OFFSET_ADDR                      70
#define RTL8290B_RXLOS_RESETB_OFFSET_ADDR                            71
#define RTL8290B_RXLOS_CLK_MODE_OFFSET_ADDR                          72
#define RTL8290B_RXLOS_TESTMODE_OFFSET_ADDR                          73
#define RTL8290B_RXLOS_ASSERT_SEL_OFFSET_ADDR                        74
#define RTL8290B_RXLOS_POLARITY_OFFSET_ADDR                          75
#define RTL8290B_RXLOS_PIN_EN_OFFSET_ADDR                            76
#define RTL8290B_TXDIS_PIN_EN_OFFSET_ADDR                            77
#define RTL8290B_TXSD_PIN_EN_OFFSET_ADDR                             78
#define RTL8290B_AUTO_POWER_LEVELING_OFFSET_ADDR                     79
#define RTL8290B_AUTO_POWER_LEVELING_HTH_OFFSET_ADDR                 80
#define RTL8290B_AUTO_POWER_LEVELING_LTH_OFFSET_ADDR                 82
#define RTL8290B_RXLOS_DEBOUNCE_OPT_OFFSET_ADDR                      84
#define RTL8290B_RXLOS_SQUELCH_OFFSET_ADDR                           85
#define RTL8290B_RX_SLEEP_FORCE_LOS_OFFSET_ADDR                      86
#define RTL8290B_RX_SLEEP_PD_LA_OFFSET_ADDR                          87
#define RTL8290B_RX_SLEEP_PD_BOOSTER_OFFSET_ADDR                     88
#define RTL8290B_RX_SLEEP_PD_RSSI_OFFSET_ADDR                        89
#define RTL8290B_RX_SLEEP_PD_VCM_OFFSET_ADDR                         90
#define RTL8290B_TX_DCD_CAL_EN_OFFSET_ADDR                           91
#define RTL8290B_TX_DCD_CAL_LUT_SEL_OFFSET_ADDR                      92
#define RTL8290B_APC_TIMER_OFFSET_ADDR                               93
#define RTL8290B_ERC_TIMER_OFFSET_ADDR                               94
#define RTL8290B_APC_MOD_MAX_EN_OFFSET_ADDR                          95
#define RTL8290B_APC_BIAS_MAX_EN_OFFSET_ADDR                         96
#define RTL8290B_APC_BIAS_MIN_EN_OFFSET_ADDR                         97
#define RTL8290B_DDMI_TX_POW_LATCH_SEL_OFFSET_ADDR                   98
#define RTL8290B_INIT_PROC_FAULT_ACTION_OFFSET_ADDR                  99
#define RTL8290B_WORKAROUND_TXSD_MIS_OFFSET_ADDR                     100
#define RTL8290B_INIT_IBIM_SEL_OFFSET_ADDR                           101
#define RTL8290B_TXSD_FAULT_TIMER_OFFSET_ADDR                        102
#define RTL8290B_RX_RSSI_R1_OFFSET_ADDR                                     103
#define RTL8290B_RX_RSSI_R2_OFFSET_ADDR                                     105
#define RTL8290B_WORKAROUND_FALSE_TXSD_DIS_OFFSET_ADDR               107
#define RTL8290B_LOOP_MON_INVERVAL_OFFSET_ADDR                       108
#define RTL8290B_CAL_TX_IAVG_OFFSET_ADDR                             109
#define RTL8290B_TEMP0_OFFSET_ADDR                                   110
#define RTL8290B_TEMP0_OFF_OFFSET_ADDR                               112
#define RTL8290B_ERC_COMP_OFFSET_ADDR                                114
#define RTL8290B_RX_K_OFFSET_ADDR                                    120
#define RTL8290B_MPD1_OFFSET_ADDR                                    124

#define RTL8290B_EPON_APC_ER_SET_OFFSET_ADDR                         128
#define RTL8290B_EPON_APC_ER_TRIM_OFFSET_ADDR                        129
#define RTL8290B_EPON_APC_CMPD_OFFSET_ADDR                           130
#define RTL8290B_EPON_APC_LPF_BW_OFFSET_ADDR                         131
#define RTL8290B_EPON_APC_LOOP_GAIN_OFFSET_ADDR                      132
#define RTL8290B_EPON_APC_CROSS_EN_OFFSET_ADDR                       133
#define RTL8290B_EPON_APC_CROSS_STR_OFFSET_ADDR                      134
#define RTL8290B_EPON_ERC_LOOP_GAIN_OFFSET_ADDR                      135
#define RTL8290B_EPON_ERC_CHOPPER_EN_OFFSET_ADDR                     136
#define RTL8290B_EPON_APC_TIMER_OFFSET_ADDR                          137
#define RTL8290B_EPON_ERC_TIMER_OFFSET_ADDR                          138
#define RTL8290B_EPON_ERC_COMP_OFFSET_ADDR                           139
#define RTL8290B_DUAL_PON_MODE_OFFSET_ADDR                           140
#define RTL8290B_RX_LOWER_BOUND_OFFSET_ADDR                          141
#define RTL8290B_CAL_IBIAS_OFFSET_ADDR                               142
#define RTL8290B_CAL_IMOD_OFFSET_ADDR                                146
#define RTL8290B_CAL_EPON_IBIAS_OFFSET_ADDR                          150
#define RTL8290B_CAL_EPON_IMOD_OFFSET_ADDR                           154
#define RTL8290B_TX_TEMP_OFFSET_ADDR                                 158
#define RTL8290B_IBIM_WORKAROUND_OFFSET_ADDR                         160
#define RTL8290B_SOFTWARE_POWER_LEVEL_ENABLE_OFFSET_ADDR             162
#define RTL8290B_SOFTWARE_POWER_LEVEL_HIGH_OFFSET_ADDR               164
#define RTL8290B_SOFTWARE_POWER_LEVEL_LOW_OFFSET_ADDR                166
#define RTL8290B_RSSI_CODE_SEL_OFFSET_ADDR                           168
#define RTL8290B_RSSI_V0_2_OFFSET_ADDR                               170
#define RTL8290B_RSSI_K2_OFFSET_ADDR                                 174
#define RTL8290B_RXDDMI_OFFSET_OFFSET_ADDR                           178



//#define RTL8290B_I2C_NUM_ADDR                                 1600
#define RTL8290B_TX_TEMP_TEST_OFFSET_ADDR                            200


#define RTL8290B_STRING_SIZE          256

#define EUROPA_LDD_8290B_MODE           1
#define EUROPA_LDD_8290C_MODE           2
#define EUROPA_LDD_8291_MODE            3
#define EUROPA_LDD_UNKNOWN_MODE         -1

#define EUROPA_CAL_FILE_LOCATION   "/var/tmp/europa_cal.data"
#define EUROPA_CAL_FILE_SIZE       0x1000
#define EUROPA_CAL_TXPWR1_ADDR     0
#define EUROPA_CAL_TXMPD1_ADDR     4
#define EUROPA_CAL_TXPWR2_ADDR     8
#define EUROPA_CAL_TXMPD2_ADDR     12
#define EUROPA_CAL_RXPWR1_ADDR     16
#define EUROPA_CAL_RXPWR2_ADDR     20
#define EUROPA_CAL_RXPWR3_ADDR     24
#define EUROPA_CAL_RXRSSI1_ADDR    28
#define EUROPA_CAL_RXRSSI2_ADDR    32
#define EUROPA_CAL_RXRSSI3_ADDR    36
#define EUROPA_CAL_RSSIV0_ADDR     40
#define EUROPA_CAL_MPD0_ADDR       44
#define EUROPA_CAL_RX_A_ADDR       48
#define EUROPA_CAL_RX_B_ADDR       52
#define EUROPA_CAL_RX_C_ADDR       56
#define EUROPA_CAL_TX_A_ADDR       60
#define EUROPA_CAL_TX_B_ADDR       64
#define EUROPA_CAL_TX_C_ADDR       68
#define EUROPA_CAL_TOFFSET_ADDR    72
#define EUROPA_CAL_TSCALE_ADDR     76
#define EUROPA_CAL_TX_IAVG_ADDR    80
#define EUROPA_CAL_LASERLUT_BASE_ADDR    84
#define EUROPA_CAL_LASERLUT_SIZE         (171*20)
#define EUROPA_CAL_RXSH1_ADDR     0xE00
#define EUROPA_CAL_RXSH2_ADDR     0xE04
#define EUROPA_CAL_RXSH3_ADDR     0xE08
#define EUROPA_CAL_TXSH1_ADDR     0xE0C
#define EUROPA_CAL_TXSH2_ADDR     0xE10
#define EUROPA_CAL_TXSH3_ADDR     0xE14


typedef struct  europa_rxpwr_s
{
    uint32  rssi_v;
    uint32  rssi_i;
    uint32  rssi_code;
    uint32  gnd_code;
    uint32  vdd_code;
    uint32  half_vdd_code;
    uint32  rssi_code2;	
}  europa_rxpwr_t;

typedef enum europa_sdadc_code_e{
    EUROPA_SDADC_LDO_TX_CODE = 0,
    EUROPA_SDADC_LDO_APC_CODE,
    EUROPA_SDADC_LDO_RXLA_CODE,
    EUROPA_SDADC_LDO_LV_CODE,
    EUROPA_SDADC_HALF_VDD_CODE,
    EUROPA_SDADC_GND_CODE,
    EUROPA_SDADC_RSSI_CODE,
    EUROPA_SDADC_RSET1_CODE,	
    EUROPA_SDADC_CODE_END,
} europa_sdadc_code_t;

typedef struct europa_rxparam_s
{
    uint32                  rssi_v0;	
    uint32                  vdd_v0;
    uint32                  gnd_v0;
    uint32                  half_v0;
    uint32                  rssi_r1;
    uint32                  rssi_r2;
    int32                   rssi_k;
    int32                   rx_a;
    int32                   rx_b;
    int32                   rx_c;
    int32                   loopcnt;
    int32                   mode;
    uint32                  rssi_v0_2;	
    int32                   rssi_k_2;
    uint32                  message;
} europa_rxparam_t;


/*
 * Function Declaration
 */

extern void _europa_cal_data_add(uint32 addr, uint32 data);
extern void _europa_cal_data_get(uint32 addr, uint32 *pdata);
extern int _europa_ldd_parser(void);

static int europa_cli_delete(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_flash_init(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_param_open(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_param_close(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_param_clear(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_flash_set(
    int argc,
    char *argv[],
    FILE *fp);

static int _europa_load_data(
    unsigned char *ptr,
    FILE *fp,
    int *file_len); 
    
static int europa_cli_flash_get(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_long_set(
    int argc,
    char *argv[],
    FILE *fp);   

static int europa_cli_all_get(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_ddmi_a0_set(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_ddmi_a2_set(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_a4_set(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_apd_lut_set(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_laser_lut_set(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_P0_set(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_P1_set(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_VendorName_set(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_VendorPN_set(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_VendorRev_set(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_VendorSN_set(
    int argc,
    char *argv[],
    FILE *fp);


static int europa_cli_DateCode_set(
    int argc,
    char *argv[],
    FILE *fp);


static int europa_cli_ddmi_a0_get(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_ddmi_a2_get(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_a4_get(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_apd_lut_get(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_laser_lut_get(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_P0P1_get(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_VendorName_get(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_VendorPN_get(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_VendorRev_get(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_VendorSN_get(
    int argc,
    char *argv[],
    FILE *fp);


static int europa_cli_DateCode_get(
    int argc,
    char *argv[],
    FILE *fp);


static int europa_cli_vmpdCal_get(
    int argc,
    char *argv[],
    FILE *fp);


static int europa_cli_txpower_get(
    int argc,
    char *argv[],
    FILE *fp);


static int europa_cli_rxpower_get(
    int argc,
    char *argv[],
    FILE *fp);


static int europa_cli_txbias_get(
    int argc,
    char *argv[],
    FILE *fp);


static int europa_cli_txmod_get(
    int argc,
    char *argv[],
    FILE *fp);


static int europa_cli_rssiv0_get(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_rssiv0_2_get(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_rssi_voltage_get(
	int argc,
	char *argv[],
	FILE *fp);

static int europa_cli_rssi_voltage2_get(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_rssi_voltage3_get(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_rssi_voltage4_get(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_rssi_voltage5_get(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_temperature_get(
    int argc,
	char *argv[],
	FILE *fp);

static int europa_cli_vdd_get(
	int argc,
	char *argv[],
    FILE *fp);

static int europa_cli_rxparam_set(
		int argc,
		char *argv[],
		FILE *fp);

static int europa_cli_rxparam_get(
		int argc,
		char *argv[],
		FILE *fp);

static int europa_cli_txparam_set(
		int argc,
		char *argv[],
		FILE *fp);

static int europa_cli_txparam_get(
		int argc,
		char *argv[],
		FILE *fp);

static int europa_cli_flash_a0_get(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_flash_a2_get(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_flash_a4_get(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_flash_apd_lut_get(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_flash_laser_lut_get(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_flash_param_get(
    int argc,
    char *argv[],
    FILE *fp);


static int europa_cli_flash_getAll(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_ddmi_a0_getAll(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_ddmi_a2_getAll(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_a4_getAll(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_a5_getAll(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_register_getAll(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_register_get(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_register_set(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_register_page_get(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_dac_set(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_dac_get(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_cal_tx_ddmi(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_cal_rx_ddmi(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_cal_power(
    int argc,
    char *argv[],
    FILE *fp);
static int europa_cli_cal_er(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_cal_trim(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_cal_cross(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_cal_toffset(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_cal_tscale(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_cal_qreg_x1_y1_x2_y2_x3_y3(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_cal_los(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_cal_hyst(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_cal_cmpd(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_cal_lpf(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_txif_set(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_txif_get(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_cal_vbr(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_cal_rxlos(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_cal_rxsd(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_cal_autopwr(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_cal_init(
	int argc,
	char *argv[],
	FILE *fp);  

static int europa_cli_cal_load(
	int argc,
	char *argv[],
	FILE *fp);  

static int europa_cli_cal_save(
	int argc,
	char *argv[],
	FILE *fp);    

static int europa_cli_cal_refresh(
	int argc,
	char *argv[],
	FILE *fp);

static int europa_cli_cal_rxtoffset(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_cal_erc_comp(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_cal_txes(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_cal_prbs(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_cal_epon_er(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_cal_epon_trim(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_cal_epon_prbs(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_cal_epon_cmpd(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_cal_epon_lpf(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_cal_epon_apc_cross(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_cal_epon_erc_comp(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_cal_ibias_max(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_cal_imod_max(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_cal_ibias_min(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_cal_level(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_cal_ben(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_cal_rxpol(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_cal_rxlospol(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_driver_version_get(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_laserinfo_get(
		int argc,
		char *argv[],
		FILE *fp);

static int europa_cli_gen_laserlut(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_laserlut_set(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_laserlut_get(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_laserlut_dump(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_laserinfo_get(
        int argc,
        char *argv[],
        FILE *fp);

static int europa_cli_gen_laserlut2_data(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_gen_laserlut2_clear(
	int argc,
	char *argv[],
	FILE *fp);

static int europa_cli_gen_laserlut2_calc(
	int argc,
	char *argv[],
	FILE *fp);

static int europa_cli_cal_laserlut(
	int argc,
	char *argv[],
	FILE *fp);

static int europa_cli_apdlut_set(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_txbias_set(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_txmod_set(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_loopmode_set(
    int argc,
    char *argv[],
    FILE *fp);


static int europa_cli_loopmode_get(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_rssi_resistor_set(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_rssi_resistor_get(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_booster_set(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_booster_get(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_apcdig_get(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_shotBurst_set(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_sdadc_code_get(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_txinfo_get(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_test_get(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_chip_init(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_reset(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_fsuTest_set(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_efuse_set(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_efuse_get(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_efuse_dump(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_sram_dump(
    int argc,
    char *argv[],
    FILE *fp);

static int europa_cli_update_flash(
    int argc,
    char *argv[],
    FILE *fp);
#endif /* __EUROPA_CLI_H__ */

