#ifndef _CA_FT2_CFG_H_
#define _CA_FT2_CFG_H_

#ifdef CONFIG_CA_EFUSE
typedef struct ca_phy_param_s {
        uint32_t sw_patch       : 1;
        uint32_t phy_cmd        : 2;
        uint32_t reserved3_0    : 1;

        uint32_t gphy_cal_cmplt : 1;
        uint32_t dgac_lb_dn_up  : 1;
        uint32_t spd_chg        : 1;
        uint32_t reserved3_1    : 9;

        uint32_t en_ado_cal     : 1;
        uint32_t en_rc_cal      : 1;
        uint32_t en_r_cal       : 1;

        uint32_t en_amp_cal     : 1;
        uint32_t disable_500m   : 1;
        uint32_t reserved3_2    : 11;
} ca_phy_param_t;
#else
typedef struct ca_phy_param_s {
        uint32_t reserved3_0    : 4;
        uint32_t gphy_cal_cmplt : 1;
        uint32_t reserved3_1    : 3;

        uint32_t sw_patch       : 1;
        uint32_t phy_cmd        : 2;
        uint32_t reserved3_2    : 2;
        uint32_t dgac_lb_dn_up  : 1;
        uint32_t spd_chg        : 1;
        uint32_t reserved3_3    : 1;

        uint32_t en_ado_cal     : 1;
        uint32_t en_rc_cal      : 1;
        uint32_t en_r_cal       : 1;
        uint32_t en_amp_cal     : 1;
        uint32_t disable_500m   : 1;
        uint32_t reserved3_4    : 11;
} ca_phy_param_t;
#endif

typedef struct ca_phy_k_s {
        uint32_t rc_cal_len     : 16;
        uint32_t amp_cal        : 16;
        uint32_t adc0_cal       : 16;
        uint32_t r_cal          : 4;
        uint32_t reserved       : 12;
} ca_phy_k_t;


typedef struct ca_ft2_cfg_s {
        uint32_t uuid;
        uint32_t hvsid          : 8;
        uint32_t reserved1_0    : 24;
        uint32_t reserved2;

        ca_phy_param_t param;

        uint32_t phy_patch[3];

        uint32_t test_prog_ver;

        ca_phy_k_t phy_k_data[4];
} ca_ft2_cfg_t;

#define FT2_CFG_PHY_PARAM_OFFSET        0x0C
#define FT2_CFG_PHY_PATCH_OFFSET        0x10
#define FT2_CFG_TEST_PROG_VER_OFFSET    0x1C
#define FT2_CFG_PHY_CALIB_OFFSET        0x20


#endif
