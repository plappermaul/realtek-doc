/*
 * Copyright(c) Realtek Semiconductor Corporation, 2017
 * All rights reserved.
 *
 * Purpose : define how hardware design
 *
 * Feature : default hardware profile
 *
 */

static hwp_swDescp_t rtl9301_default_swDescp = {

    .port.descp = {
        { .mac_id =  0, .attr = 0, .eth = 0, .medi = 0, .sds_idx = 0,  .phy_idx = HWP_TBD, .smi = 0, .phy_addr = 0,  .led_c = 0,   .led_f = 0, .led_layout = SINGLE_SET,},
        { .mac_id =  1, .attr = 0, .eth = 0, .medi = 0, .sds_idx = 0,  .phy_idx = HWP_TBD, .smi = 0, .phy_addr = 1,  .led_c = 0,   .led_f = 0, .led_layout = SINGLE_SET,},
        { .mac_id =  2, .attr = 0, .eth = 0, .medi = 0, .sds_idx = 0,  .phy_idx = HWP_TBD, .smi = 0, .phy_addr = 2,  .led_c = 0,   .led_f = 0, .led_layout = SINGLE_SET,},
        { .mac_id =  3, .attr = 0, .eth = 0, .medi = 0, .sds_idx = 0,  .phy_idx = HWP_TBD, .smi = 0, .phy_addr = 3,  .led_c = 0,   .led_f = 0, .led_layout = SINGLE_SET,},
        { .mac_id =  4, .attr = 0, .eth = 0, .medi = 0, .sds_idx = 1,  .phy_idx = HWP_TBD, .smi = 0, .phy_addr = 4,  .led_c = 0,   .led_f = 0, .led_layout = SINGLE_SET,},
        { .mac_id =  5, .attr = 0, .eth = 0, .medi = 0, .sds_idx = 1,  .phy_idx = HWP_TBD, .smi = 0, .phy_addr = 5,  .led_c = 0,   .led_f = 0, .led_layout = SINGLE_SET,},
        { .mac_id =  6, .attr = 0, .eth = 0, .medi = 0, .sds_idx = 1,  .phy_idx = HWP_TBD, .smi = 0, .phy_addr = 6,  .led_c = 0,   .led_f = 0, .led_layout = SINGLE_SET,},
        { .mac_id =  7, .attr = 0, .eth = 0, .medi = 0, .sds_idx = 1,  .phy_idx = HWP_TBD, .smi = 0, .phy_addr = 7,  .led_c = 0,   .led_f = 0, .led_layout = SINGLE_SET,},
        { .mac_id =  8, .attr = 0, .eth = 0, .medi = 0, .sds_idx = 2,  .phy_idx = HWP_TBD, .smi = 1, .phy_addr = 8,  .led_c = 0,   .led_f = 0, .led_layout = SINGLE_SET,},
        { .mac_id =  9, .attr = 0, .eth = 0, .medi = 0, .sds_idx = 2,  .phy_idx = HWP_TBD, .smi = 1, .phy_addr = 9,  .led_c = 0,   .led_f = 0, .led_layout = SINGLE_SET,},
        { .mac_id = 10, .attr = 0, .eth = 0, .medi = 0, .sds_idx = 2,  .phy_idx = HWP_TBD, .smi = 1, .phy_addr = 10, .led_c = 0,   .led_f = 0, .led_layout = SINGLE_SET,},
        { .mac_id = 11, .attr = 0, .eth = 0, .medi = 0, .sds_idx = 2,  .phy_idx = HWP_TBD, .smi = 1, .phy_addr = 11, .led_c = 0,   .led_f = 0, .led_layout = SINGLE_SET,},
        { .mac_id = 12, .attr = 0, .eth = 0, .medi = 0, .sds_idx = 2,  .phy_idx = HWP_TBD, .smi = 1, .phy_addr = 12, .led_c = 0,   .led_f = 0, .led_layout = SINGLE_SET,},
        { .mac_id = 13, .attr = 0, .eth = 0, .medi = 0, .sds_idx = 2,  .phy_idx = HWP_TBD, .smi = 1, .phy_addr = 13, .led_c = 0,   .led_f = 0, .led_layout = SINGLE_SET,},
        { .mac_id = 14, .attr = 0, .eth = 0, .medi = 0, .sds_idx = 2,  .phy_idx = HWP_TBD, .smi = 1, .phy_addr = 14, .led_c = 0,   .led_f = 0, .led_layout = SINGLE_SET,},
        { .mac_id = 15, .attr = 0, .eth = 0, .medi = 0, .sds_idx = 2,  .phy_idx = HWP_TBD, .smi = 1, .phy_addr = 15, .led_c = 0,   .led_f = 0, .led_layout = SINGLE_SET,},
        { .mac_id = 16, .attr = 0, .eth = 0, .medi = 0, .sds_idx = 3,  .phy_idx = HWP_TBD, .smi = 2, .phy_addr = 16, .led_c = 0,   .led_f = 0, .led_layout = SINGLE_SET,},
        { .mac_id = 17, .attr = 0, .eth = 0, .medi = 0, .sds_idx = 3,  .phy_idx = HWP_TBD, .smi = 2, .phy_addr = 17, .led_c = 0,   .led_f = 0, .led_layout = SINGLE_SET,},
        { .mac_id = 18, .attr = 0, .eth = 0, .medi = 0, .sds_idx = 3,  .phy_idx = HWP_TBD, .smi = 2, .phy_addr = 18, .led_c = 0,   .led_f = 0, .led_layout = SINGLE_SET,},
        { .mac_id = 19, .attr = 0, .eth = 0, .medi = 0, .sds_idx = 3,  .phy_idx = HWP_TBD, .smi = 2, .phy_addr = 19, .led_c = 0,   .led_f = 0, .led_layout = SINGLE_SET,},
        { .mac_id = 20, .attr = 0, .eth = 0, .medi = 0, .sds_idx = 3,  .phy_idx = HWP_TBD, .smi = 2, .phy_addr = 20, .led_c = 0,   .led_f = 0, .led_layout = SINGLE_SET,},
        { .mac_id = 21, .attr = 0, .eth = 0, .medi = 0, .sds_idx = 3,  .phy_idx = HWP_TBD, .smi = 2, .phy_addr = 21, .led_c = 0,   .led_f = 0, .led_layout = SINGLE_SET,},
        { .mac_id = 22, .attr = 0, .eth = 0, .medi = 0, .sds_idx = 3,  .phy_idx = HWP_TBD, .smi = 2, .phy_addr = 22, .led_c = 0,   .led_f = 0, .led_layout = SINGLE_SET,},
        { .mac_id = 23, .attr = 0, .eth = 0, .medi = 0, .sds_idx = 3,  .phy_idx = HWP_TBD, .smi = 2, .phy_addr = 23, .led_c = 0,   .led_f = 0, .led_layout = SINGLE_SET,},
        { .mac_id = 24, .attr = 0, .eth = 0, .medi = 0, .sds_idx = 4,  .phy_idx = HWP_TBD, .smi = 3, .phy_addr = 24, .led_c = 0,   .led_f = 0, .led_layout = DOUBLE_SET,},
        { .mac_id = 25, .attr = 0, .eth = 0, .medi = 0, .sds_idx = 5,  .phy_idx = HWP_TBD, .smi = 3, .phy_addr = 25, .led_c = 0,   .led_f = 0, .led_layout = DOUBLE_SET,},
        { .mac_id = 26, .attr = 0, .eth = 0, .medi = 0, .sds_idx = 6,  .phy_idx = HWP_TBD, .smi = 3, .phy_addr = 26, .led_c = 0,   .led_f = 0, .led_layout = DOUBLE_SET,},
        { .mac_id = 27, .attr = 0, .eth = 0, .medi = 0, .sds_idx = 7,  .phy_idx = HWP_TBD, .smi = 3, .phy_addr = 27, .led_c = 0,   .led_f = 0, .led_layout = DOUBLE_SET,},
        { .mac_id = 28, .attr = HWP_CPU, .eth = HWP_NONE,.medi = HWP_NONE,.sds_idx = HWP_NONE,.phy_idx = HWP_NONE,.smi = HWP_NONE, .phy_addr = HWP_NONE, .led_c = HWP_NONE, .led_f = HWP_NONE,.led_layout = HWP_NONE,  },
        { .mac_id = HWP_END },
    },  /* port.descp */

    .led.descp = {
        .led_active = LED_ACTIVE_LOW,
        .led_if_sel = LED_IF_SEL_SERIAL,
        .led_definition_set[0].led[0] = 0xB80,        /* 10/100Mbps link/act */
        .led_definition_set[0].led[1] = 0xA20,        /* 1000Mbps link/act */
        .led_definition_set[0].led[2] = 0xA01,        /* duplex mode */
        .led_definition_set[0].led[3] = 0x0,          /* Not blink */
        .led_definition_set[0].led[4] = HWP_LED_END,      /* None */
    },/* led.descp */

    .serdes.descp = {
        [0] = { .sds_id = 0, .mode = RTK_MII_QSGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [1] = { .sds_id = 1, .mode = RTK_MII_QSGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [2] = { .sds_id = 2, .mode = RTK_MII_XSGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [3] = { .sds_id = 3, .mode = RTK_MII_XSGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [4] = { .sds_id = 4, .mode = RTK_MII_10GR,    .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [5] = { .sds_id = 6, .mode = RTK_MII_10GR,    .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [6] = { .sds_id = 8, .mode = RTK_MII_10GR,    .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [7] = { .sds_id = 9, .mode = RTK_MII_10GR,    .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [8] = { .sds_id = HWP_END },
    }, /* serdes.descp */

    .phy.descp = {
        [0] = { .chip = HWP_END },
    },   /* .phy.descp */

};


/*
 * hardware profile
 */
static hwp_hwProfile_t rtl9301_defaultHwp = {

    .identifier.name        = "RTL9301_DEFAULT",
    .identifier.id          = HWP_RTL9301_DEFAULT,
    .swDescp = {
        [0]                 = &rtl9301_default_swDescp,
    }

};
