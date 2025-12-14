#ifndef __EUROPA_8290C_H__
#define __EUROPA_8290C_H__

#define RTL8290C_CMD_VERSION    "1.0.6"

#define RTL8290C_PARAMETER_SIZE                               0x1000
#define RTL8290C_BLOCK_SIZE                                   0x100
#define RTL8290C_FILE_SIZE                                    0x1000
#define RTL8290C_DEVICE_0_FILE                                "/var/config/rtl8290c_dev0.data"
#define RTL8290C_DEVICE_1_FILE                                "/var/config/rtl8290c_dev1.data"
#define RTL8290C_DEVICE_2_FILE                                "/var/config/rtl8290c_dev2.data"
#define RTL8290C_DEVICE_3_FILE                                "/var/config/rtl8290c_dev3.data"
#define RTL8290C_DEVICE_4_FILE                                "/var/config/rtl8290c_dev4.data"


#define RTL8290C_FLASH_DATA_SIZE                              0x900

#define RTL8290C_DDMI_A0_ADDR_BASE                                 0
#define RTL8290C_DDMI_A2_ADDR_BASE                                 0x100
#define RTL8290C_A2_PAGE0_ADDR_BASE                                0x180
#define RTL8290C_A2_PAGE1_ADDR_BASE                                0x200
#define RTL8290C_WB_ADDR_BASE                                      0x280
#define RTL8290C_RB_ADDR_BASE                                      0x300
#define RTL8290C_DIG_ADDR_BASE                                     0x380
#define RTL8290C_TXLUT_ADDR_BASE                                   0x400
#define RTL8290C_APDLUT_ADDR_BASE                                  0x480
#define RTL8290C_RSVLUT1_ADDR_BASE                                 0x500
#define RTL8290C_RSVLUT2_ADDR_BASE                                 0x580
#define RTL8290C_INDACC_ADDR_BASE                                  0x600
#define RTL8290C_INFO_ADDR_BASE                                    0x680
#define RTL8290C_DIGEX_ADDR_BASE                                   0x700

#define RTL8290C_FLASH_PAGE_SIZE                                   0x80

#define RTL8290C_PARAMETER_VERSION_ADDR                            0x900
#define RTL8290C_PARAMETER_VERSION_VALUE                           0x24062701

#define RTL8290C_PARAMETER_BASE_ADDR                              0x780
#define RTL8290C_BOSA_TYPE_OFFSET_ADDR                                0
#define RTL8290C_INIT_PROC_FAULT_ACTION_OFFSET_ADDR                   1
#define RTL8290C_INIT_IBIM_SEL_OFFSET_ADDR                            2
#define RTL8290C_LOOP_MON_INVERVAL_OFFSET_ADDR                        3

#define RTL8290C_TX_BACK_TERM_TYPE_OFFSET_ADDR                        4
#define RTL8290C_TX_BACK_TERM_VAL_OFFSET_ADDR                         5
#define RTL8290C_TX_INTERFACE_OFFSET_ADDR                             6
#define RTL8290C_TX_CROSS_EN_OFFSET_ADDR                              7
#define RTL8290C_TX_CROSS_SIGN_OFFSET_ADDR                            8
#define RTL8290C_TX_CROSS_STR_OFFSET_ADDR                             9
#define RTL8290C_TX_IN_POL_SWAP_OFFSET_ADDR                          10
#define RTL8290C_TX_OUT_POL_SWAP_OFFSET_ADDR                         11

#define RTL8290C_APC_LASER_ON_DELAY_OFFSET_ADDR                      12
#define RTL8290C_APC_SETTLE_COUNT_OFFSET_ADDR                        13
#define RTL8290C_APC_IBIAS_INIT_OFFSET_ADDR                          14
#define RTL8290C_APC_IMOD_INIT_OFFSET_ADDR                           15
#define RTL8290C_APC_LOOP_MODE_OFFSET_ADDR                           16

#define RTL8290C_APC_IAVG_SET_OFFSET_ADDR                            17
#define RTL8290C_APC_ER_SET_OFFSET_ADDR                              18
#define RTL8290C_APC_ER_TRIM_OFFSET_ADDR                             19
#define RTL8290C_APC_CMPD_OFFSET_ADDR                                20
#define RTL8290C_APC_LPF_BW_OFFSET_ADDR                              21
#define RTL8290C_APC_LOOP_GAIN_OFFSET_ADDR                           22
#define RTL8290C_ERC_LOOP_GAIN_OFFSET_ADDR                           23
#define RTL8290C_ERC_CHOPPER_EN_OFFSET_ADDR                          24
#define RTL8290C_APC_CROSS_EN_OFFSET_ADDR                            25
#define RTL8290C_APC_CROSS_STR_OFFSET_ADDR                           26
#define RTL8290C_APC_MOD_MAX_OFFSET_ADDR                             27
#define RTL8290C_APC_MOD_MAX_EN_OFFSET_ADDR                          28
#define RTL8290C_APC_BIAS_MAX_OFFSET_ADDR                            29
#define RTL8290C_APC_BIAS_MAX_EN_OFFSET_ADDR                         30
#define RTL8290C_APC_BIAS_MIN_OFFSET_ADDR                            31
#define RTL8290C_APC_BIAS_MIN_EN_OFFSET_ADDR                         32
#define RTL8290C_APC_TIMER_OFFSET_ADDR                               33
#define RTL8290C_ERC_TIMER_OFFSET_ADDR                               34

#define RTL8290C_DUAL_PON_MODE_OFFSET_ADDR                           35
#define RTL8290C_EPON_APC_ER_SET_OFFSET_ADDR                         36
#define RTL8290C_EPON_APC_ER_TRIM_OFFSET_ADDR                        37
#define RTL8290C_EPON_APC_CMPD_OFFSET_ADDR                           38
#define RTL8290C_EPON_APC_LPF_BW_OFFSET_ADDR                         39
#define RTL8290C_EPON_APC_LOOP_GAIN_OFFSET_ADDR                      40
#define RTL8290C_EPON_APC_CROSS_EN_OFFSET_ADDR                       41
#define RTL8290C_EPON_APC_CROSS_STR_OFFSET_ADDR                      42
#define RTL8290C_EPON_ERC_LOOP_GAIN_OFFSET_ADDR                      43
#define RTL8290C_EPON_ERC_CHOPPER_EN_OFFSET_ADDR                     44
#define RTL8290C_EPON_APC_TIMER_OFFSET_ADDR                          45
#define RTL8290C_EPON_ERC_TIMER_OFFSET_ADDR                          46
#define RTL8290C_EPON_ERC_COMP_OFFSET_ADDR                           47

#define RTL8290C_FSU_EBABLE_OFFSET_ADDR                              48
#define RTL8290C_FSU_APC_RAMPB_OFFSET_ADDR                           49
#define RTL8290C_FSU_APC_RAMPM_OFFSET_ADDR                           50
#define RTL8290C_FSU_ERC_RAMPM_OFFSET_ADDR                           51

#define RTL8290C_RX_OUT_POL_SWAP_OFFSET_ADDR                         52
#define RTL8290C_RX_OUT_SWING_OFFSET_ADDR                            53
#define RTL8290C_RX_DIS_OFFCAN_OFFSET_ADDR                           54

#define RTL8290C_TXSD_MODE_OFFSET_ADDR                               55
#define RTL8290C_TXSD_TH_OFFSET_ADDR                                 56
#define RTL8290C_TXDIS_PIN_EN_OFFSET_ADDR                            57
#define RTL8290C_TXSD_PIN_EN_OFFSET_ADDR                             58
#define RTL8290C_TXSD_FAULT_TIMER_OFFSET_ADDR                        59

#define RTL8290C_RXLOS_REF_DAC_OFFSET_ADDR                           60
#define RTL8290C_RXLOS_HYST_SEL_OFFSET_ADDR                          61
#define RTL8290C_RXLOS_RANGE_SEL_OFFSET_ADDR                         62
#define RTL8290C_RXLOS_DEBOUNCE_SEL_OFFSET_ADDR                      63
#define RTL8290C_RXLOS_DEBOUNCE_OPT_OFFSET_ADDR                      64
#define RTL8290C_RXLOS_SQUELCH_OFFSET_ADDR                           65
#define RTL8290C_RXLOS_LA_MAG_COMP_OFFSET_ADDR                       66
#define RTL8290C_RXLOS_RESETB_OFFSET_ADDR                            67
#define RTL8290C_RXLOS_CLK_MODE_OFFSET_ADDR                          68
#define RTL8290C_RXLOS_TESTMODE_OFFSET_ADDR                          69
#define RTL8290C_RXLOS_ASSERT_SEL_OFFSET_ADDR                        70
#define RTL8290C_RXLOS_POLARITY_OFFSET_ADDR                          71
#define RTL8290C_RXLOS_PIN_EN_OFFSET_ADDR                            72

#define RTL8290C_RX_SLEEP_PIN_OFFSET_ADDR                            73
#define RTL8290C_RX_SLEEP_FORCE_LOS_OFFSET_ADDR                      74
#define RTL8290C_RX_SLEEP_PD_LA_OFFSET_ADDR                          75
#define RTL8290C_RX_SLEEP_PD_BOOSTER_OFFSET_ADDR                     76
#define RTL8290C_RX_SLEEP_PD_RSSI_OFFSET_ADDR                        77
#define RTL8290C_RX_SLEEP_PD_VCM_OFFSET_ADDR                         78

#define RTL8290C_DDMI_TX_POW_LATCH_SEL_OFFSET_ADDR                   79
#define RTL8290C_TX_DDMI_MPD1_OFFSET_ADDR                            80
#define RTL8290C_TX_DDMI_IAVG_SET1_OFFSET_ADDR                       84
#define RTL8290C_RX_LOWER_BOUND_OFFSET_ADDR                          85

#define RTL8290C_TEMP_OFFSET_ADDR                                    86
#define RTL8290C_TEMP_INTR_SCALE_ADDR                                87
#define RTL8290C_CAL_IBIAS_OFFSET_ADDR                               88
#define RTL8290C_CAL_IMOD_OFFSET_ADDR                                92
#define RTL8290C_CAL_EPON_IBIAS_OFFSET_ADDR                          96
#define RTL8290C_CAL_EPON_IMOD_OFFSET_ADDR                           100


#define RTL8290C_RX_A_ADDR                                           104
#define RTL8290C_RX_B_ADDR                                           106
#define RTL8290C_RX_C_ADDR                                           108
#define RTL8290C_SHIFT_RX_A_ADDR                                     110
#define RTL8290C_SHIFT_RX_B_ADDR                                     111
#define RTL8290C_SHIFT_RX_C_ADDR                                     112
#define RTL8290C_TX_A_ADDR                                           113
#define RTL8290C_TX_B_ADDR                                           115
#define RTL8290C_TX_C_ADDR                                           117
#define RTL8290C_SHIFT_TX_A_ADDR                                     119
#define RTL8290C_SHIFT_TX_B_ADDR                                     120
#define RTL8290C_SHIFT_TX_C_ADDR                                     121
#define RTL8290C_ERC_COMP_OFFSET_ADDR                                122
#define RTL8290C_APD_LUT_EN_OFFSET_ADDR                              123
#define RTL8290C_TX_LUT_EN_OFFSET_ADDR                               124

//20240318: SD add
#define RTL8290C_TX_DCD_CAL_EN_OFFSET_ADDR                           125
#define RTL8290C_TX_DCD_CAL_LUT_SEL_OFFSET_ADDR                      126
#define RTL8290C_TX_DCD_LUT0_OFFSET_ADDR                             127
#define RTL8290C_TX_DCD_LUT1_OFFSET_ADDR                             128
#define RTL8290C_TX_DCD_LUT2_OFFSET_ADDR                             129
#define RTL8290C_TX_DCD_LUT3_OFFSET_ADDR                             130
#define RTL8290C_TX_DCD_LUT4_OFFSET_ADDR                             131
#define RTL8290C_TX_DCD_LUT5_OFFSET_ADDR                             132 
#define RTL8290C_TX_DCD_LUT6_OFFSET_ADDR                             133
#define RTL8290C_TX_DCD_LUT7_OFFSET_ADDR                             134
#define RTL8290C_TX_PCX_K_OFFSET_ADDR                                135
#define RTL8290C_TX_NCX_K_OFFSET_ADDR                                136

#define RTL8290C_RX_32BIT_MODE                0
#define RTL8290C_RX_64BIT_MODE                1
#define RTL8290C_RX_REAL_MODE                 0x2
#define RTL8290C_RX_TRANS_MODE                0xFF
#define RTL8290C_RX_CURR2PWR_MODE             0xFE

#define RTL8290C_TX_REAL_MODE                 0x2
#define RTL8290C_TX_CURR2PWR_MODE             0x3

// Fixed shift bits for RX power coefficients, no use in dynamic shift algorithm
#define RX_A_SHIFT 30
#define RX_B_SHIFT 13
#define RX_C_SHIFT 12

// shift bits for TX power coefficients
#define TX_A_SHIFT 8
#define TX_B_SHIFT 5

#define RTL8290C_RSSI_CODE_STANDARD          6400
#define RTL8290C_MPD_CODE_STANDARD           500


typedef enum rtl8290c_sdadc_code_type_s
{
    RTL8290C_CODE_TEM = 0,
    RTL8290C_CODE_CALV,
    RTL8290C_CODE_PS,
    RTL8290C_CODE_TXAPC,
    RTL8290C_CODE_LDO,
    RTL8290C_CODE_RSSIINP_SEL,
    RTL8290C_CODE_RSSIINP,
    RTL8290C_CODE_RSSIINN_SEL,
    RTL8290C_CODE_RSSIINN,
    RTL8290C_CODE_RSSIFL_SEL,
    RTL8290C_CODE_RSSIFL,
    RTL8290C_CODE_VLASERN,
    RTL8290C_CODE_ADCIN,
    RTL8290C_CODE_PS_TX,
    RTL8290C_CODE_PS_RX,
    RTL8290C_CODE_VBG,
    RTL8290C_CODE_CALG,
    RTL8290C_CODE_TXAPC_RSEL,    
    RTL8290C_SDADC_CODE_END
} rtl8290c_sdadc_code_type_t;


static unsigned char rtl8290c_a0_reg[0x100] = {
0x02, 0x04, 0x0b, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x03, 0x0c, 0x00, 0x14, 0xc8,
0x00, 0x00, 0x00, 0x00, 0x52, 0x45, 0x41, 0x4c, 0x54, 0x45, 0x4b, 0x20, 0x20, 0x20, 0x20, 0x20,
0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0x00, 0x52, 0x54, 0x4c, 0x38, 0x32, 0x39, 0x30, 0x20,
0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x30, 0x30, 0x30, 0x31, 0x05, 0x1e, 0x00, 0xff,
0x00, 0x06, 0x00, 0x00, 0x76, 0x65, 0x6e, 0x64, 0x6f, 0x72, 0x70, 0x61, 0x72, 0x74, 0x6e, 0x75,
0x6d, 0x62, 0x65, 0x72, 0x32, 0x30, 0x31, 0x34, 0x30, 0x31, 0x32, 0x33, 0x68, 0x80, 0x02, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

static unsigned char rtl8290c_a2l_reg[0x80] = {
0x7a, 0xd7, 0x8f, 0x21, 0x32, 0xd7, 0x62, 0x81, 0x13, 0x26, 0x43, 0x2a, 0xbb, 0x77, 0x4d, 0x88,
0x2e, 0xb5, 0x48, 0x2b, 0x06, 0xba, 0x48, 0x46, 0x1c, 0x1a, 0x4c, 0x0d, 0xc8, 0x32, 0x2f, 0x01,
0xef, 0x7a, 0x45, 0xea, 0x4d, 0xb0, 0x26, 0x3e, 0x37, 0xaf, 0x86, 0x62, 0x21, 0xcb, 0x42, 0x08,
0x30, 0x7e, 0x80, 0xc0, 0x30, 0x76, 0xbe, 0x3a, 0x64, 0x58, 0x41, 0xeb, 0xcb, 0x33, 0x5b, 0xbd,
0x58, 0x7d, 0x3a, 0x2e, 0xc4, 0x48, 0xa1, 0x4f, 0xa4, 0x2c, 0x1f, 0xbe, 0x84, 0x7f, 0x1f, 0x15,
0x9a, 0x9e, 0xe5, 0xc1, 0x36, 0x79, 0x82, 0x87, 0x91, 0x91, 0xa9, 0xaf, 0x02, 0xe2, 0xed, 0x22,
0x00, 0x2e, 0xc4, 0x20, 0xe9, 0xaa, 0x60, 0x48, 0xe6, 0x02, 0xc0, 0xf8, 0x3e, 0x90, 0x00, 0x69,
0xf6, 0x91, 0x84, 0x0b, 0x66, 0x48, 0xc6, 0xa6, 0xac, 0xe2, 0xd6, 0x8c, 0x31, 0xe8, 0x6a, 0xff
};

static unsigned char rtl8290c_wb_reg[0x80] = {
0x90, 0x92, 0x80, 0x80, 0x00, 0x3e, 0x00, 0x70, 0xdc, 0xfe, 0x60, 0xf8, 0x84, 0x10, 0x0a, 0x70,
0x0f, 0x00, 0x10, 0xe3, 0xbe, 0xff, 0x05, 0x4d, 0x08, 0x7a, 0x06, 0xc0, 0xaa, 0xa8, 0x00, 0x00,
0x08, 0x00, 0x00, 0x10, 0x00, 0x00, 0x14, 0x80, 0x00, 0x00, 0x00, 0x00, 0x5d, 0xb2, 0x00, 0x00,
0xe1, 0x10, 0x98, 0x50, 0x81, 0x06, 0x54, 0x6c, 0x6c, 0x6c, 0x4c, 0x74, 0x8c, 0x48, 0x54, 0x03,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0xc5, 0x80, 0x01, 0x84,
0x6e, 0x5c, 0x00, 0x00, 0x10, 0x55, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x04, 0x00, 0x00,
0x00, 0x00, 0x72, 0x44, 0x58, 0xbc, 0x80, 0x00, 0x0d, 0x03, 0x00, 0x00, 0x20, 0x21, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static unsigned char rtl8290c_digital_reg[0x80] = {
0x01, 0xe0, 0x00, 0x00, 0x06, 0xc0, 0x00, 0x04, 0x01, 0x30, 0x00, 0x34, 0x00, 0xff, 0xff, 0x00,
0x15, 0x82, 0x00, 0x01, 0x00, 0x00, 0x0c, 0x2a, 0x00, 0x32, 0xff, 0x03, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0xa0, 0xc0, 0x00, 0x32, 0x06, 0xe0, 0xe0, 0xe0, 0xbf, 0xfe, 0x00,
0x00, 0x00, 0x00, 0xff, 0x00, 0x10, 0x00, 0x00, 0x7b, 0x00, 0x00, 0x00, 0xa0, 0x89, 0x40, 0x02,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0xff, 0x80, 0x00, 0x01, 0xa0, 0x00, 0x00, 0x00, 0x15, 0x00,
0x00, 0x00, 0x00, 0xf8, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x23, 0x0f, 0x03, 0x7d, 0x08, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static unsigned char rtl8290c_txlut_reg[0x80] = {
0xd0, 0x7f, 0xc3, 0x42, 0x0b, 0x79, 0xdd, 0x08, 0xa0, 0x50, 0x80, 0x6d, 0x24, 0xe3, 0x6a, 0xa0,
0x8f, 0x67, 0x8a, 0x44, 0x33, 0xb0, 0x4a, 0x88, 0xa0, 0xde, 0xe2, 0xf8, 0xe5, 0x5f, 0x88, 0xbf,
0x91, 0xa9, 0xba, 0xad, 0x02, 0xc1, 0x87, 0x65, 0xc6, 0x55, 0x04, 0x0e, 0xcb, 0x63, 0x15, 0x8a,
0x8f, 0xa6, 0x1a, 0xdc, 0x45, 0x8c, 0x2a, 0x04, 0x90, 0x5d, 0x02, 0x06, 0xf0, 0xf7, 0x23, 0x0d,
0x96, 0x9f, 0x5c, 0x67, 0x9e, 0x05, 0x35, 0x65, 0x5c, 0x08, 0xc8, 0x02, 0x8e, 0xd4, 0x15, 0x35,
0xbd, 0x14, 0x0c, 0x51, 0x9a, 0xd0, 0x3e, 0x99, 0xa9, 0x73, 0x64, 0x6a, 0xe9, 0x5c, 0x51, 0xd8,
0x45, 0xcf, 0xc3, 0x00, 0xe8, 0xc1, 0x96, 0x9d, 0xe0, 0x58, 0xa4, 0x44, 0x57, 0x74, 0x11, 0x20,
0x95, 0x88, 0x81, 0x81, 0x00, 0x23, 0xb9, 0x08, 0x5d, 0xb3, 0x90, 0x72, 0x75, 0x99, 0x82, 0x46
};

static unsigned char rtl8290c_apdlut_reg[0x80] = {
0xf9, 0x41, 0x5b, 0xc9, 0xdf, 0x24, 0x4a, 0xc8, 0xa2, 0xad, 0x28, 0x25, 0xb6, 0x9d, 0x1d, 0x29,
0xb6, 0x1b, 0x28, 0x01, 0xe6, 0x60, 0x65, 0x9d, 0xe5, 0xb0, 0x8a, 0xcd, 0x6e, 0x29, 0x0b, 0x28,
0xda, 0xfb, 0xe8, 0x4d, 0xa2, 0xe8, 0xf4, 0xd4, 0x15, 0x92, 0xc7, 0xf1, 0x41, 0x06, 0xf8, 0x5f,
0x81, 0xaf, 0xaf, 0x13, 0x52, 0x69, 0x0b, 0xbf, 0xab, 0x40, 0x6d, 0x1c, 0x72, 0x03, 0xe2, 0xea,
0x4d, 0x69, 0xd2, 0x87, 0xdc, 0x20, 0x40, 0x00, 0x69, 0x76, 0x60, 0xa0, 0x13, 0xa0, 0x58, 0xd7,
0x88, 0x90, 0x33, 0x3d, 0xe5, 0x74, 0x40, 0xf9, 0x01, 0x11, 0x14, 0x0f, 0x2a, 0xdc, 0x68, 0xd8,
0x01, 0xc1, 0x95, 0x30, 0x51, 0x00, 0xd7, 0xb8, 0x87, 0x44, 0xc0, 0xb4, 0x24, 0x36, 0x8c, 0x24,
0x01, 0x26, 0x2c, 0x9c, 0x93, 0x6b, 0x90, 0x4f, 0x2e, 0xe9, 0x2f, 0x9f, 0x7e, 0x2e, 0x23, 0x5c
};

static unsigned char rtl8290c_rsvlut1_reg[0x80] = {
0xa4, 0x33, 0x3c, 0xb4, 0x06, 0xe5, 0x47, 0x70, 0xc2, 0x50, 0x5c, 0xc9, 0xc5, 0x66, 0x72, 0xb0,
0x46, 0x56, 0xab, 0x35, 0xb0, 0xae, 0xba, 0x6b, 0x41, 0xcf, 0x95, 0xe2, 0x0a, 0xc8, 0x15, 0x82,
0xc8, 0xea, 0x72, 0x8a, 0x2d, 0xa1, 0x4c, 0x0b, 0xb0, 0x47, 0x6e, 0x96, 0xb4, 0xdc, 0x36, 0x01,
0x41, 0xce, 0xc5, 0x42, 0x29, 0x24, 0x4b, 0x15, 0x89, 0x4c, 0x1d, 0x93, 0xfe, 0x65, 0xae, 0xaf,
0x54, 0xe2, 0x5c, 0x29, 0x82, 0x43, 0x8c, 0xd2, 0xd6, 0x63, 0x41, 0x89, 0xb7, 0xcb, 0x7c, 0x27,
0x6b, 0x58, 0x05, 0x06, 0x61, 0x82, 0x45, 0x52, 0x55, 0x84, 0x09, 0x41, 0x18, 0x10, 0x0f, 0xa2,
0x7b, 0x03, 0x3a, 0xa0, 0x6f, 0x0a, 0xae, 0x21, 0x15, 0x1f, 0x39, 0x48, 0xae, 0xdf, 0x4a, 0xc5,
0x37, 0x7d, 0x31, 0x3e, 0x85, 0x33, 0x2c, 0xa9, 0x28, 0xd2, 0xe1, 0xac, 0xda, 0xb2, 0xc0, 0x80
};

static unsigned char rtl8290c_rsvlut2_reg[0x80] = {
0xeb, 0x78, 0x51, 0x36, 0x01, 0x40, 0x89, 0x54, 0xb4, 0xcd, 0x45, 0x74, 0x44, 0xe6, 0x00, 0x86,
0x78, 0x35, 0x29, 0xb9, 0x78, 0x8f, 0x0c, 0x8d, 0xc9, 0x67, 0x62, 0x54, 0xf4, 0x3e, 0x19, 0x29,
0x3a, 0x6b, 0x38, 0x1e, 0x24, 0x11, 0xae, 0x1e, 0xde, 0xd9, 0x26, 0x8e, 0x19, 0x18, 0x90, 0x7e,
0xd9, 0x7d, 0x9c, 0x56, 0xda, 0x78, 0x08, 0xbf, 0xee, 0x07, 0x9d, 0x2b, 0xf3, 0xb8, 0x71, 0xa7,
0xee, 0x60, 0x20, 0x02, 0x0a, 0x2c, 0x6c, 0x20, 0x28, 0xce, 0x78, 0x72, 0x39, 0xa3, 0xa9, 0xad,
0x94, 0xe9, 0xa4, 0x95, 0x58, 0xd8, 0x98, 0x90, 0x24, 0x81, 0xd1, 0xb1, 0x99, 0xcf, 0x03, 0x2c,
0x38, 0x11, 0x45, 0xe2, 0xf0, 0x76, 0xcb, 0x5c, 0xa2, 0xb2, 0x23, 0x62, 0x6e, 0xa0, 0xe8, 0x2b,
0x0a, 0x82, 0xa9, 0xee, 0x38, 0xa8, 0xcf, 0x7d, 0x84, 0x10, 0x04, 0xbb, 0xed, 0x38, 0xff, 0x0a,
};

static unsigned char rtl8290c_digitalext_reg[0x80] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static unsigned char rtl8290c_user_parameter[0x100] = {
0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x00,     0x00, 0x08, 0x00, 0x01, 0x00, 0x02, 0x80, 0xFF, /*15*/
0x03, 0x40, 0x08, 0x10, 0x05, 0x02, 0x05, 0x05,     0x01, 0x01, 0x88, 0xCC, 0x01, 0x4D, 0x01, 0x0B, /*31*/
0x01, 0x03, 0x03, 0x01, 0x07, 0x00, 0x05, 0x00,     0x05, 0x01, 0x88, 0x05, 0x01, 0x03, 0x03, 0x00, /*47*/
0x01, 0x01, 0x03, 0x00, 0x00, 0x00, 0x01, 0x00,     0x04, 0x01, 0x01, 0x04, 0x24, 0x03, 0x00, 0x03, /*63*/
0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,     0x01, 0x00, 0x01, 0x01, 0x00, 0x01, 0x00, 0x00, /*79*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2A,     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*95*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*111*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x88, /*127*/
0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x00,     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*143*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
#if 0
static float rtl8290c_er_factor[16][16] = {
	{1.000, 1.071, 1.125, 1.167, 1.200, 1.227, 1.250, 1.269,	1.286, 1.300, 1.324, 1.342, 1.357, 1.370, 1.385, 1.397},
	{0.933, 1.000, 1.050, 1.089, 1.120, 1.145, 1.167, 1.185,	1.200, 1.213, 1.235, 1.253, 1.267, 1.278, 1.292, 1.303},
	{0.889, 0.952, 1.000, 1.037, 1.067, 1.091, 1.111, 1.128,	1.143, 1.156, 1.176, 1.193, 1.206, 1.217, 1.231, 1.241},
	{0.857, 0.918, 0.964, 1.000, 1.029, 1.052, 1.071, 1.088,	1.102, 1.114, 1.134, 1.150,	1.163, 1.174, 1.187, 1.197},
	{0.833, 0.893, 0.938, 0.972, 1.000, 1.023, 1.042, 1.058,	1.071, 1.083, 1.103, 1.118,	1.131, 1.141, 1.154, 1.164},
	{0.815, 0.873, 0.917, 0.951, 0.978, 1.000, 1.019, 1.034,	1.048, 1.059, 1.078, 1.094,	1.106, 1.116, 1.128, 1.138},
	{0.800, 0.857, 0.900, 0.933, 0.960, 0.982, 1.000, 1.015,	1.029, 1.040, 1.059, 1.074,	1.086, 1.096, 1.108, 1.117},
	{0.788, 0.844, 0.886, 0.919, 0.945, 0.967, 0.985, 1.000,	1.013, 1.024, 1.043, 1.057,	1.069, 1.079, 1.091, 1.100},
	{0.778, 0.833, 0.875, 0.907, 0.933, 0.955, 0.972, 0.987,	1.000, 1.011, 1.029, 1.044,	1.056, 1.065, 1.077, 1.086},
	{0.769, 0.824, 0.865, 0.897, 0.923, 0.944, 0.962, 0.976,	0.989, 1.000, 1.018, 1.032,	1.044, 1.054, 1.065, 1.074},
	{0.756, 0.810, 0.850, 0.881, 0.907, 0.927, 0.944, 0.959,	0.971, 0.982, 1.000, 1.014,	1.025, 1.035, 1.046, 1.055},
	{0.745, 0.798, 0.838, 0.869, 0.894, 0.914, 0.931, 0.946,	0.958, 0.969, 0.986, 1.000,	1.011, 1.020, 1.032, 1.041},
	{0.737, 0.789, 0.829, 0.860, 0.884, 0.904, 0.921, 0.935,	0.947, 0.958, 0.975, 0.989,	1.000, 1.009, 1.020, 1.029},
	{0.730, 0.782, 0.821, 0.852, 0.876, 0.896, 0.913, 0.927,	0.939, 0.949, 0.966, 0.980,	0.991, 1.000, 1.011, 1.020},
	{0.722, 0.774, 0.813, 0.843, 0.867, 0.886, 0.903, 0.917,	0.929, 0.939, 0.956, 0.969,	0.980, 0.989, 1.000, 1.009},
	{0.716, 0.767, 0.806, 0.835, 0.859, 0.879, 0.895, 0.909,	0.921, 0.931, 0.948, 0.961,	0.972, 0.981, 0.991, 1.000},
};

static float rtl8290c_er_trim_factor[32] = {
0.900, 0.906, 0.913, 0.919, 0.925, 0.931, 0.938, 0.944, 0.950, 0.956, 0.963, 0.969, 0.975, 0.981, 0.988, 0.994,
1.000, 1.006, 1.013, 1.019, 1.025, 1.031, 1.038, 1.044, 1.050, 1.056, 1.063, 1.069, 1.075, 1.081, 1.088, 1.094
};
#endif

static float rtl8290c_er_factor[16][16] = {
{1.000000, 1.071429, 1.125000, 1.166667, 1.200000, 1.227273, 1.250000, 1.269231,   1.285714, 1.300000, 1.323529, 1.342105, 1.357143, 1.369565, 1.384615, 1.396552}, 
{0.933333, 1.000000, 1.050000, 1.088889, 1.120000, 1.145455, 1.166667, 1.184615,   1.200000, 1.213333, 1.235294, 1.252632, 1.266667, 1.278261, 1.292308, 1.303448}, 
{0.888889, 0.952381, 1.000000, 1.037037, 1.066667, 1.090909, 1.111111, 1.128205,   1.142857, 1.155556, 1.176471, 1.192982, 1.206349, 1.217391, 1.230769, 1.241379}, 
{0.857143, 0.918367, 0.964286, 1.000000, 1.028571, 1.051948, 1.071429, 1.087912,   1.102041, 1.114286, 1.134454, 1.150376, 1.163265, 1.173913, 1.186813, 1.197044}, 
{0.833333, 0.892857, 0.937500, 0.972222, 1.000000, 1.022727, 1.041667, 1.057692,   1.071429, 1.083333, 1.102941, 1.118421, 1.130952, 1.141304, 1.153846, 1.163793}, 
{0.814815, 0.873016, 0.916667, 0.950617, 0.977778, 1.000000, 1.018519, 1.034188,   1.047619, 1.059259, 1.078431, 1.093567, 1.105820, 1.115942, 1.128205, 1.137931}, 
{0.800000, 0.857143, 0.900000, 0.933333, 0.960000, 0.981818, 1.000000, 1.015385,   1.028571, 1.040000, 1.058824, 1.073684, 1.085714, 1.095652, 1.107692, 1.117241}, 
{0.787879, 0.844156, 0.886364, 0.919192, 0.945455, 0.966942, 0.984848, 1.000000,   1.012987, 1.024242, 1.042781, 1.057416, 1.069264, 1.079051, 1.090909, 1.100313}, 
{0.777778, 0.833333, 0.875000, 0.907407, 0.933333, 0.954545, 0.972222, 0.987179,   1.000000, 1.011111, 1.029412, 1.043860, 1.055556, 1.065217, 1.076923, 1.086207}, 
{0.769231, 0.824176, 0.865385, 0.897436, 0.923077, 0.944056, 0.961538, 0.976331,   0.989011, 1.000000, 1.018100, 1.032389, 1.043956, 1.053512, 1.065089, 1.074271}, 
{0.755556, 0.809524, 0.850000, 0.881481, 0.906667, 0.927273, 0.944444, 0.958974,   0.971429, 0.982222, 1.000000, 1.014035, 1.025397, 1.034783, 1.046154, 1.055172}, 
{0.745098, 0.798319, 0.838235, 0.869281, 0.894118, 0.914439, 0.931373, 0.945701,   0.957983, 0.968627, 0.986159, 1.000000, 1.011204, 1.020460, 1.031674, 1.040568}, 
{0.736842, 0.789474, 0.828947, 0.859649, 0.884211, 0.904306, 0.921053, 0.935223,   0.947368, 0.957895, 0.975232, 0.988920, 1.000000, 1.009153, 1.020243, 1.029038}, 
{0.730159, 0.782313, 0.821429, 0.851852, 0.876190, 0.896104, 0.912698, 0.926740,   0.938776, 0.949206, 0.966387, 0.979950, 0.990930, 1.000000, 1.010989, 1.019704}, 
{0.722222, 0.773810, 0.812500, 0.842593, 0.866667, 0.886364, 0.902778, 0.916667,   0.928571, 0.938889, 0.955882, 0.969298, 0.980159, 0.989130, 1.000000, 1.008621}, 
{0.716049, 0.767196, 0.805556, 0.835391, 0.859259, 0.878788, 0.895062, 0.908832,   0.920635, 0.930864, 0.947712, 0.961014, 0.971781, 0.980676, 0.991453, 1.000000}
};
static float rtl8290c_er_trim_factor[32][32] = {
{1.00000, 1.00694, 1.01389, 1.02083, 1.02778, 1.03472, 1.04167, 1.04861,  1.05556, 1.06250, 1.06944, 1.07639, 1.08333, 1.09028, 1.09722, 1.10417,  1.11111, 1.11806, 1.12500, 1.13194, 1.13889, 1.14583, 1.15278, 1.15972,  1.16667, 1.17361, 1.18056, 1.18750, 1.19444, 1.20139, 1.20833, 1.21528},
{0.99310, 1.00000, 1.00690, 1.01379, 1.02069, 1.02759, 1.03448, 1.04138,  1.04828, 1.05517, 1.06207, 1.06897, 1.07586, 1.08276, 1.08966, 1.09655,  1.10345, 1.11034, 1.11724, 1.12414, 1.13103, 1.13793, 1.14483, 1.15172,  1.15862, 1.16552, 1.17241, 1.17931, 1.18621, 1.19310, 1.20000, 1.20690},
{0.98630, 0.99315, 1.00000, 1.00685, 1.01370, 1.02055, 1.02740, 1.03425,  1.04110, 1.04795, 1.05479, 1.06164, 1.06849, 1.07534, 1.08219, 1.08904,  1.09589, 1.10274, 1.10959, 1.11644, 1.12329, 1.13014, 1.13699, 1.14384,  1.15068, 1.15753, 1.16438, 1.17123, 1.17808, 1.18493, 1.19178, 1.19863},
{0.97959, 0.98639, 0.99320, 1.00000, 1.00680, 1.01361, 1.02041, 1.02721,  1.03401, 1.04082, 1.04762, 1.05442, 1.06122, 1.06803, 1.07483, 1.08163,  1.08844, 1.09524, 1.10204, 1.10884, 1.11565, 1.12245, 1.12925, 1.13605,  1.14286, 1.14966, 1.15646, 1.16327, 1.17007, 1.17687, 1.18367, 1.19048},
{0.97297, 0.97973, 0.98649, 0.99324, 1.00000, 1.00676, 1.01351, 1.02027,  1.02703, 1.03378, 1.04054, 1.04730, 1.05405, 1.06081, 1.06757, 1.07432,  1.08108, 1.08784, 1.09459, 1.10135, 1.10811, 1.11486, 1.12162, 1.12838,  1.13514, 1.14189, 1.14865, 1.15541, 1.16216, 1.16892, 1.17568, 1.18243},
{0.96644, 0.97315, 0.97987, 0.98658, 0.99329, 1.00000, 1.00671, 1.01342,  1.02013, 1.02685, 1.03356, 1.04027, 1.04698, 1.05369, 1.06040, 1.06711,  1.07383, 1.08054, 1.08725, 1.09396, 1.10067, 1.10738, 1.11409, 1.12081,  1.12752, 1.13423, 1.14094, 1.14765, 1.15436, 1.16107, 1.16779, 1.17450},
{0.96000, 0.96667, 0.97333, 0.98000, 0.98667, 0.99333, 1.00000, 1.00667,  1.01333, 1.02000, 1.02667, 1.03333, 1.04000, 1.04667, 1.05333, 1.06000,  1.06667, 1.07333, 1.08000, 1.08667, 1.09333, 1.10000, 1.10667, 1.11333,  1.12000, 1.12667, 1.13333, 1.14000, 1.14667, 1.15333, 1.16000, 1.16667},
{0.95364, 0.96026, 0.96689, 0.97351, 0.98013, 0.98675, 0.99338, 1.00000,  1.00662, 1.01325, 1.01987, 1.02649, 1.03311, 1.03974, 1.04636, 1.05298,  1.05960, 1.06623, 1.07285, 1.07947, 1.08609, 1.09272, 1.09934, 1.10596,  1.11258, 1.11921, 1.12583, 1.13245, 1.13907, 1.14570, 1.15232, 1.15894},
{0.94737, 0.95395, 0.96053, 0.96711, 0.97368, 0.98026, 0.98684, 0.99342,  1.00000, 1.00658, 1.01316, 1.01974, 1.02632, 1.03289, 1.03947, 1.04605,  1.05263, 1.05921, 1.06579, 1.07237, 1.07895, 1.08553, 1.09211, 1.09868,  1.10526, 1.11184, 1.11842, 1.12500, 1.13158, 1.13816, 1.14474, 1.15132},
{0.94118, 0.94771, 0.95425, 0.96078, 0.96732, 0.97386, 0.98039, 0.98693,  0.99346, 1.00000, 1.00654, 1.01307, 1.01961, 1.02614, 1.03268, 1.03922,  1.04575, 1.05229, 1.05882, 1.06536, 1.07190, 1.07843, 1.08497, 1.09150,  1.09804, 1.10458, 1.11111, 1.11765, 1.12418, 1.13072, 1.13725, 1.14379},
{0.93506, 0.94156, 0.94805, 0.95455, 0.96104, 0.96753, 0.97403, 0.98052,  0.98701, 0.99351, 1.00000, 1.00649, 1.01299, 1.01948, 1.02597, 1.03247,  1.03896, 1.04545, 1.05195, 1.05844, 1.06494, 1.07143, 1.07792, 1.08442,  1.09091, 1.09740, 1.10390, 1.11039, 1.11688, 1.12338, 1.12987, 1.13636},
{0.92903, 0.93548, 0.94194, 0.94839, 0.95484, 0.96129, 0.96774, 0.97419,  0.98065, 0.98710, 0.99355, 1.00000, 1.00645, 1.01290, 1.01935, 1.02581,  1.03226, 1.03871, 1.04516, 1.05161, 1.05806, 1.06452, 1.07097, 1.07742,  1.08387, 1.09032, 1.09677, 1.10323, 1.10968, 1.11613, 1.12258, 1.12903},
{0.92308, 0.92949, 0.93590, 0.94231, 0.94872, 0.95513, 0.96154, 0.96795,  0.97436, 0.98077, 0.98718, 0.99359, 1.00000, 1.00641, 1.01282, 1.01923,  1.02564, 1.03205, 1.03846, 1.04487, 1.05128, 1.05769, 1.06410, 1.07051,  1.07692, 1.08333, 1.08974, 1.09615, 1.10256, 1.10897, 1.11538, 1.12179},
{0.91720, 0.92357, 0.92994, 0.93631, 0.94268, 0.94904, 0.95541, 0.96178,  0.96815, 0.97452, 0.98089, 0.98726, 0.99363, 1.00000, 1.00637, 1.01274,  1.01911, 1.02548, 1.03185, 1.03822, 1.04459, 1.05096, 1.05732, 1.06369,  1.07006, 1.07643, 1.08280, 1.08917, 1.09554, 1.10191, 1.10828, 1.11465},
{0.91139, 0.91772, 0.92405, 0.93038, 0.93671, 0.94304, 0.94937, 0.95570,  0.96203, 0.96835, 0.97468, 0.98101, 0.98734, 0.99367, 1.00000, 1.00633,  1.01266, 1.01899, 1.02532, 1.03165, 1.03797, 1.04430, 1.05063, 1.05696,  1.06329, 1.06962, 1.07595, 1.08228, 1.08861, 1.09494, 1.10127, 1.10759},
{0.90566, 0.91195, 0.91824, 0.92453, 0.93082, 0.93711, 0.94340, 0.94969,  0.95597, 0.96226, 0.96855, 0.97484, 0.98113, 0.98742, 0.99371, 1.00000,  1.00629, 1.01258, 1.01887, 1.02516, 1.03145, 1.03774, 1.04403, 1.05031,  1.05660, 1.06289, 1.06918, 1.07547, 1.08176, 1.08805, 1.09434, 1.10063},
{0.90000, 0.90625, 0.91250, 0.91875, 0.92500, 0.93125, 0.93750, 0.94375,  0.95000, 0.95625, 0.96250, 0.96875, 0.97500, 0.98125, 0.98750, 0.99375,  1.00000, 1.00625, 1.01250, 1.01875, 1.02500, 1.03125, 1.03750, 1.04375,  1.05000, 1.05625, 1.06250, 1.06875, 1.07500, 1.08125, 1.08750, 1.09375},
{0.89441, 0.90062, 0.90683, 0.91304, 0.91925, 0.92547, 0.93168, 0.93789,  0.94410, 0.95031, 0.95652, 0.96273, 0.96894, 0.97516, 0.98137, 0.98758,  0.99379, 1.00000, 1.00621, 1.01242, 1.01863, 1.02484, 1.03106, 1.03727,  1.04348, 1.04969, 1.05590, 1.06211, 1.06832, 1.07453, 1.08075, 1.08696},
{0.88889, 0.89506, 0.90123, 0.90741, 0.91358, 0.91975, 0.92593, 0.93210,  0.93827, 0.94444, 0.95062, 0.95679, 0.96296, 0.96914, 0.97531, 0.98148,  0.98765, 0.99383, 1.00000, 1.00617, 1.01235, 1.01852, 1.02469, 1.03086,  1.03704, 1.04321, 1.04938, 1.05556, 1.06173, 1.06790, 1.07407, 1.08025},
{0.88344, 0.88957, 0.89571, 0.90184, 0.90798, 0.91411, 0.92025, 0.92638,  0.93252, 0.93865, 0.94479, 0.95092, 0.95706, 0.96319, 0.96933, 0.97546,  0.98160, 0.98773, 0.99387, 1.00000, 1.00613, 1.01227, 1.01840, 1.02454,  1.03067, 1.03681, 1.04294, 1.04908, 1.05521, 1.06135, 1.06748, 1.07362},
{0.87805, 0.88415, 0.89024, 0.89634, 0.90244, 0.90854, 0.91463, 0.92073,  0.92683, 0.93293, 0.93902, 0.94512, 0.95122, 0.95732, 0.96341, 0.96951,  0.97561, 0.98171, 0.98780, 0.99390, 1.00000, 1.00610, 1.01220, 1.01829,  1.02439, 1.03049, 1.03659, 1.04268, 1.04878, 1.05488, 1.06098, 1.06707},
{0.87273, 0.87879, 0.88485, 0.89091, 0.89697, 0.90303, 0.90909, 0.91515,  0.92121, 0.92727, 0.93333, 0.93939, 0.94545, 0.95152, 0.95758, 0.96364,  0.96970, 0.97576, 0.98182, 0.98788, 0.99394, 1.00000, 1.00606, 1.01212,  1.01818, 1.02424, 1.03030, 1.03636, 1.04242, 1.04848, 1.05455, 1.06061},
{0.86747, 0.87349, 0.87952, 0.88554, 0.89157, 0.89759, 0.90361, 0.90964,  0.91566, 0.92169, 0.92771, 0.93373, 0.93976, 0.94578, 0.95181, 0.95783,  0.96386, 0.96988, 0.97590, 0.98193, 0.98795, 0.99398, 1.00000, 1.00602,  1.01205, 1.01807, 1.02410, 1.03012, 1.03614, 1.04217, 1.04819, 1.05422},
{0.86228, 0.86826, 0.87425, 0.88024, 0.88623, 0.89222, 0.89820, 0.90419,  0.91018, 0.91617, 0.92216, 0.92814, 0.93413, 0.94012, 0.94611, 0.95210,  0.95808, 0.96407, 0.97006, 0.97605, 0.98204, 0.98802, 0.99401, 1.00000,  1.00599, 1.01198, 1.01796, 1.02395, 1.02994, 1.03593, 1.04192, 1.04790},
{0.85714, 0.86310, 0.86905, 0.87500, 0.88095, 0.88690, 0.89286, 0.89881,  0.90476, 0.91071, 0.91667, 0.92262, 0.92857, 0.93452, 0.94048, 0.94643,  0.95238, 0.95833, 0.96429, 0.97024, 0.97619, 0.98214, 0.98810, 0.99405,  1.00000, 1.00595, 1.01190, 1.01786, 1.02381, 1.02976, 1.03571, 1.04167},
{0.85207, 0.85799, 0.86391, 0.86982, 0.87574, 0.88166, 0.88757, 0.89349,  0.89941, 0.90533, 0.91124, 0.91716, 0.92308, 0.92899, 0.93491, 0.94083,  0.94675, 0.95266, 0.95858, 0.96450, 0.97041, 0.97633, 0.98225, 0.98817,  0.99408, 1.00000, 1.00592, 1.01183, 1.01775, 1.02367, 1.02959, 1.03550},
{0.84706, 0.85294, 0.85882, 0.86471, 0.87059, 0.87647, 0.88235, 0.88824,  0.89412, 0.90000, 0.90588, 0.91176, 0.91765, 0.92353, 0.92941, 0.93529,  0.94118, 0.94706, 0.95294, 0.95882, 0.96471, 0.97059, 0.97647, 0.98235,  0.98824, 0.99412, 1.00000, 1.00588, 1.01176, 1.01765, 1.02353, 1.02941},
{0.84211, 0.84795, 0.85380, 0.85965, 0.86550, 0.87135, 0.87719, 0.88304,  0.88889, 0.89474, 0.90058, 0.90643, 0.91228, 0.91813, 0.92398, 0.92982,  0.93567, 0.94152, 0.94737, 0.95322, 0.95906, 0.96491, 0.97076, 0.97661,  0.98246, 0.98830, 0.99415, 1.00000, 1.00585, 1.01170, 1.01754, 1.02339},
{0.83721, 0.84302, 0.84884, 0.85465, 0.86047, 0.86628, 0.87209, 0.87791,  0.88372, 0.88953, 0.89535, 0.90116, 0.90698, 0.91279, 0.91860, 0.92442,  0.93023, 0.93605, 0.94186, 0.94767, 0.95349, 0.95930, 0.96512, 0.97093,  0.97674, 0.98256, 0.98837, 0.99419, 1.00000, 1.00581, 1.01163, 1.01744},
{0.83237, 0.83815, 0.84393, 0.84971, 0.85549, 0.86127, 0.86705, 0.87283,  0.87861, 0.88439, 0.89017, 0.89595, 0.90173, 0.90751, 0.91329, 0.91908,  0.92486, 0.93064, 0.93642, 0.94220, 0.94798, 0.95376, 0.95954, 0.96532,  0.97110, 0.97688, 0.98266, 0.98844, 0.99422, 1.00000, 1.00578, 1.01156},
{0.82759, 0.83333, 0.83908, 0.84483, 0.85057, 0.85632, 0.86207, 0.86782,  0.87356, 0.87931, 0.88506, 0.89080, 0.89655, 0.90230, 0.90805, 0.91379,  0.91954, 0.92529, 0.93103, 0.93678, 0.94253, 0.94828, 0.95402, 0.95977,  0.96552, 0.97126, 0.97701, 0.98276, 0.98851, 0.99425, 1.00000, 1.00575},
{0.82286, 0.82857, 0.83429, 0.84000, 0.84571, 0.85143, 0.85714, 0.86286,  0.86857, 0.87429, 0.88000, 0.88571, 0.89143, 0.89714, 0.90286, 0.90857,  0.91429, 0.92000, 0.92571, 0.93143, 0.93714, 0.94286, 0.94857, 0.95429,  0.96000, 0.96571, 0.97143, 0.97714, 0.98286, 0.98857, 0.99429, 1.00000}
};

//extern int _europa_ldd_parser(void);

extern int _rtl8290c_flash_init(uint32 devId);
extern int _rtl8290c_flash_delete(uint32 devId);
extern int _rtl8290c_load_data(unsigned char *ptr, uint32 devId, int *file_len);
extern int _rtl8290c_save_data(unsigned char *ptr, uint32 devId, int *file_len);
extern int _8290c_cli_string_set(uint32 devId, int start_addr, uint32 size, char *str);
extern int _8290c_cli_string_get(uint32 devId, int start_addr, uint32 size, char *str);
extern int _8290c_cli_flashBlock_set( uint32 devId, int start_addr, unsigned int size);
extern int _8290c_cli_flashBlock_get(uint32 devId, int start_addr, int length);
extern int rtl8290c_cli_reset(uint32 devId, int argc, char *argv[], FILE *fp);
extern int rtl8290c_cli_temperature_get(uint32 devId);
extern int rtl8290c_cli_vdd_get(uint32 devId);
extern int rtl8290c_cli_txbias_set(uint32 devId, int argc, char *argv[], FILE *fp);
extern int rtl8290c_cli_txbias_get(uint32 devId);
extern int rtl8290c_cli_txmod_set(uint32 devId, int argc, char *argv[], FILE *fp);
extern int rtl8290c_cli_txmod_get(uint32 devId);
extern int rtl8290c_cli_loopmode_set(uint32 devId, int argc, char *argv[], FILE *fp);
extern int rtl8290c_cli_loopmode_get(uint32 devId, int argc, char *argv[], FILE *fp);
extern int rtl8290c_cli_register_set(uint32 devId, int argc, char *argv[], FILE *fp);
extern int rtl8290c_cli_register_get(uint32 devId, int argc, char *argv[], FILE *fp);
extern int rtl8290c_cli_efuse_set(uint32 devId, int argc, char *argv[], FILE *fp);
extern int rtl8290c_cli_efuse_get(uint32 devId, int argc, char *argv[], FILE *fp);
extern int rtl8290c_cli_update_flash(uint32 devId);

extern int rtl8290c_cli_laserlut_set(uint32 devId);
extern int rtl8290c_cli_laserinfo_get(uint32 devId);
extern int rtl8290c_cli_rxpower_get(uint32 devId, uint32 loopcnt);
extern int rtl8290c_cli_rxpower2_get(int argc, char *argv[], FILE *fp);
extern int rtl8290c_cli_rxparam_set(uint32 devId, int argc, char *argv[], FILE *fp);
extern int rtl8290c_cli_rxparam_get(uint32 devId, int argc, char *argv[], FILE *fp);
extern int rtl8290c_cli_txparam_set(uint32 devId, int argc, char *argv[], FILE *fp);
extern int rtl8290c_cli_txparam_get(uint32 devId, int argc, char *argv[], FILE *fp);
extern int rtl8290c_cli_cal_rx_ddmi(uint32 devId, int argc, char *argv[], FILE *fp);
extern int rtl8290c_cli_cal_tx_ddmi(uint32 devId, int argc, char *argv[], FILE *fp);
extern int rtl8290c_cli_txpower_get(uint32 devId, uint32 loopcnt, uint32 r_mpd, uint32 code_250u, uint32 i_cal);
extern int rtl8290c_cli_cal_power(uint32 devId, int argc, char *argv[], FILE *fp);
extern int rtl8290c_cli_cal_er(uint32 devId, int argc, char *argv[], FILE *fp);
extern int rtl8290c_cli_cal_trim(uint32 devId, int argc, char *argv[], FILE *fp);
extern int rtl8290c_cli_cal_los(uint32 devId, int argc, char *argv[], FILE *fp);
extern int rtl8290c_cli_cal_hyst(uint32 devId, int argc, char *argv[], FILE *fp);
extern int rtl8290c_cli_cal_cmpd(uint32 devId, int argc, char *argv[], FILE *fp);
extern int rtl8290c_cli_cal_lpf(uint32 devId, int argc, char *argv[], FILE *fp);
extern int rtl8290c_cli_cal_cross(uint32 devId, int argc, char *argv[], FILE *fp);
extern int rtl8290c_cli_cal_toffset(uint32 devId, int argc, char *argv[], FILE *fp);
extern int rtl8290c_cli_cal_tscale(uint32 devId, int argc, char *argv[], FILE *fp);
extern int rtl8290c_cli_cal_rxpol(uint32 devId, int argc, char *argv[], FILE *fp);
extern int rtl8290c_cli_cal_rxlospol(uint32 devId, int argc, char *argv[], FILE *fp);
extern int rtl8290c_cli_cal_rxlos(uint32 devId, int argc, char *argv[], FILE *fp);
extern int rtl8290c_cli_cal_prbs(uint32 devId, int argc, char *argv[], FILE *fp);
extern int rtl8290c_cli_cal_epon_prbs(uint32 devId, int argc, char *argv[], FILE *fp);
extern int rtl8290c_cli_cal_refresh(uint32 devId, int argc, char *argv[], FILE *fp);

#endif /* __EUROPA_8290C_H__ */

