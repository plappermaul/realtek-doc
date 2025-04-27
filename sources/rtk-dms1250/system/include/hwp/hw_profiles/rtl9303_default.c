/*
 * Copyright(c) Realtek Semiconductor Corporation, 2017
 * All rights reserved.
 *
 * Purpose : define how hardware design
 *
 * Feature : default hardware profile
 *
 */

static hwp_swDescp_t rtl9303_default_swDescp = {

    .port.descp = {
        { .mac_id =  0, .attr = HWP_TBD, .eth = HWP_TBD,  .medi = HWP_SERDES, .sds_idx = 0, .phy_idx = HWP_NONE, .smi = 0, .phy_addr =   0, .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id =  8, .attr = HWP_TBD, .eth = HWP_TBD,  .medi = HWP_SERDES, .sds_idx = 1, .phy_idx = HWP_NONE, .smi = 1, .phy_addr =   8, .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id = 16, .attr = HWP_TBD, .eth = HWP_TBD,  .medi = HWP_SERDES, .sds_idx = 2, .phy_idx = HWP_NONE, .smi = 2, .phy_addr =  16, .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id = 20, .attr = HWP_TBD, .eth = HWP_TBD,  .medi = HWP_SERDES, .sds_idx = 3, .phy_idx = HWP_NONE, .smi = 2, .phy_addr =  20, .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id = 24, .attr = HWP_TBD, .eth = HWP_TBD,  .medi = HWP_SERDES, .sds_idx = 4, .phy_idx = HWP_NONE, .smi = 3, .phy_addr =  24, .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id = 25, .attr = HWP_TBD, .eth = HWP_TBD,  .medi = HWP_SERDES, .sds_idx = 5, .phy_idx = HWP_NONE, .smi = 3, .phy_addr =  25, .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id = 26, .attr = HWP_TBD, .eth = HWP_TBD,  .medi = HWP_SERDES, .sds_idx = 6, .phy_idx = HWP_NONE, .smi = 3, .phy_addr =  26, .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id = 27, .attr = HWP_TBD, .eth = HWP_TBD,  .medi = HWP_SERDES, .sds_idx = 7, .phy_idx = HWP_NONE, .smi = 3, .phy_addr =  27, .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id = 28, .attr = HWP_CPU, .eth = HWP_NONE, .medi = HWP_NONE,   .sds_idx = HWP_NONE, .phy_idx = HWP_NONE, .smi = HWP_NONE, .phy_addr =  HWP_NONE, .led_c = HWP_NONE, .led_f = HWP_NONE,.led_layout = HWP_NONE,  },
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
        [0] = { .sds_id = 2, .mode = RTK_MII_10GR,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [1] = { .sds_id = 3, .mode = RTK_MII_10GR,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [2] = { .sds_id = 4, .mode = RTK_MII_10GR,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [3] = { .sds_id = 5, .mode = RTK_MII_10GR,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [4] = { .sds_id = 6, .mode = RTK_MII_10GR,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [5] = { .sds_id = 7, .mode = RTK_MII_10GR,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [6] = { .sds_id = 8, .mode = RTK_MII_10GR,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [7] = { .sds_id = 9, .mode = RTK_MII_10GR,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [8] = { .sds_id = HWP_END },
    }, /* serdes.descp */

    .phy.descp = {
        [0] = { .chip = HWP_END },
    },   /* .phy.descp */

};


/*
 * hardware profile
 */
static hwp_hwProfile_t rtl9303_defaultHwp = {

    .identifier.name        = "RTL9303_DEFAULT",
    .identifier.id          = HWP_RTL9303_DEFAULT,
    .swDescp = {
        [0]                 = &rtl9303_default_swDescp,
    }

};
