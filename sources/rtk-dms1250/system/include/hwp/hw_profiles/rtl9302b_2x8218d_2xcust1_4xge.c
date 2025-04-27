/*
 * Copyright(c) Realtek Semiconductor Corporation, 2015
 * All rights reserved.
 *
 * Purpose : Related implementation of the RTL9302B board
 *
 * Feature : RTL9302B  16*1G+8×2.5G UTP + 4×10G Fiber RTL9302C*1(RTL9302C + 2*8218D + 2*AQR412)
 *
 */

static hwp_swDescp_t rtl9302b_2x8218d_2xcust1_4xge_swDescp = {

    .chip_id                    = RTL9302B_CHIP_ID_8X2_5G,
    .swcore_supported           = TRUE,
    .swcore_access_method       = HWP_SW_ACC_MEM,
    .swcore_spi_chip_select     = HWP_NOT_USED,
    .nic_supported              = TRUE,

    .port.descp = {
        { .mac_id =  0, .attr = HWP_ETH,   .eth = HWP_GE,    .medi = HWP_COPPER, .sds_idx = 0, .phy_idx = 0,        .smi = 0,        .phy_addr =  0,        .led_c = 0,        .led_f = HWP_NONE,.led_layout = SINGLE_SET,},
        { .mac_id =  1, .attr = HWP_ETH,   .eth = HWP_GE,    .medi = HWP_COPPER, .sds_idx = 0, .phy_idx = 0,        .smi = 0,        .phy_addr =  1,        .led_c = 0,        .led_f = HWP_NONE,.led_layout = SINGLE_SET,},
        { .mac_id =  2, .attr = HWP_ETH,   .eth = HWP_GE,    .medi = HWP_COPPER, .sds_idx = 0, .phy_idx = 0,        .smi = 0,        .phy_addr =  2,        .led_c = 0,        .led_f = HWP_NONE,.led_layout = SINGLE_SET,},
        { .mac_id =  3, .attr = HWP_ETH,   .eth = HWP_GE,    .medi = HWP_COPPER, .sds_idx = 0, .phy_idx = 0,        .smi = 0,        .phy_addr =  3,        .led_c = 0,        .led_f = HWP_NONE,.led_layout = SINGLE_SET,},
        { .mac_id =  4, .attr = HWP_ETH,   .eth = HWP_GE,    .medi = HWP_COPPER, .sds_idx = 0, .phy_idx = 0,        .smi = 0,        .phy_addr =  4,        .led_c = 0,        .led_f = HWP_NONE,.led_layout = SINGLE_SET,},
        { .mac_id =  5, .attr = HWP_ETH,   .eth = HWP_GE,    .medi = HWP_COPPER, .sds_idx = 0, .phy_idx = 0,        .smi = 0,        .phy_addr =  5,        .led_c = 0,        .led_f = HWP_NONE,.led_layout = SINGLE_SET,},
        { .mac_id =  6, .attr = HWP_ETH,   .eth = HWP_GE,    .medi = HWP_COPPER, .sds_idx = 0, .phy_idx = 0,        .smi = 0,        .phy_addr =  6,        .led_c = 0,        .led_f = HWP_NONE,.led_layout = SINGLE_SET,},
        { .mac_id =  7, .attr = HWP_ETH,   .eth = HWP_GE,    .medi = HWP_COPPER, .sds_idx = 0, .phy_idx = 0,        .smi = 0,        .phy_addr =  7,        .led_c = 0,        .led_f = HWP_NONE,.led_layout = SINGLE_SET,},
        { .mac_id =  8, .attr = HWP_ETH,   .eth = HWP_GE,    .medi = HWP_COPPER, .sds_idx = 1, .phy_idx = 1,        .smi = 1,        .phy_addr =  8,        .led_c = 0,        .led_f = HWP_NONE,.led_layout = SINGLE_SET,},
        { .mac_id =  9, .attr = HWP_ETH,   .eth = HWP_GE,    .medi = HWP_COPPER, .sds_idx = 1, .phy_idx = 1,        .smi = 1,        .phy_addr =  9,        .led_c = 0,        .led_f = HWP_NONE,.led_layout = SINGLE_SET,},
        { .mac_id = 10, .attr = HWP_ETH,   .eth = HWP_GE,    .medi = HWP_COPPER, .sds_idx = 1, .phy_idx = 1,        .smi = 1,        .phy_addr = 10,        .led_c = 0,        .led_f = HWP_NONE,.led_layout = SINGLE_SET,},
        { .mac_id = 11, .attr = HWP_ETH,   .eth = HWP_GE,    .medi = HWP_COPPER, .sds_idx = 1, .phy_idx = 1,        .smi = 1,        .phy_addr = 11,        .led_c = 0,        .led_f = HWP_NONE,.led_layout = SINGLE_SET,},
        { .mac_id = 12, .attr = HWP_ETH,   .eth = HWP_GE,    .medi = HWP_COPPER, .sds_idx = 1, .phy_idx = 1,        .smi = 1,        .phy_addr = 12,        .led_c = 0,        .led_f = HWP_NONE,.led_layout = SINGLE_SET,},
        { .mac_id = 13, .attr = HWP_ETH,   .eth = HWP_GE,    .medi = HWP_COPPER, .sds_idx = 1, .phy_idx = 1,        .smi = 1,        .phy_addr = 13,        .led_c = 0,        .led_f = HWP_NONE,.led_layout = SINGLE_SET,},
        { .mac_id = 14, .attr = HWP_ETH,   .eth = HWP_GE,    .medi = HWP_COPPER, .sds_idx = 1, .phy_idx = 1,        .smi = 1,        .phy_addr = 14,        .led_c = 0,        .led_f = HWP_NONE,.led_layout = SINGLE_SET,},
        { .mac_id = 15, .attr = HWP_ETH,   .eth = HWP_GE,    .medi = HWP_COPPER, .sds_idx = 1, .phy_idx = 1,        .smi = 1,        .phy_addr = 15,        .led_c = 0,        .led_f = HWP_NONE,.led_layout = SINGLE_SET,},
        { .mac_id = 16, .attr = HWP_ETH,   .eth = HWP_2_5GE, .medi = HWP_COPPER, .sds_idx = 2, .phy_idx = 2,        .smi = 2,        .phy_addr =  0,        .led_c = 0,        .led_f = HWP_NONE,.led_layout = SINGLE_SET,},
        { .mac_id = 17, .attr = HWP_ETH,   .eth = HWP_2_5GE, .medi = HWP_COPPER, .sds_idx = 2, .phy_idx = 2,        .smi = 2,        .phy_addr =  1,        .led_c = 0,        .led_f = HWP_NONE,.led_layout = SINGLE_SET,},
        { .mac_id = 18, .attr = HWP_ETH,   .eth = HWP_2_5GE, .medi = HWP_COPPER, .sds_idx = 2, .phy_idx = 2,        .smi = 2,        .phy_addr =  2,        .led_c = 0,        .led_f = HWP_NONE,.led_layout = SINGLE_SET,},
        { .mac_id = 19, .attr = HWP_ETH,   .eth = HWP_2_5GE, .medi = HWP_COPPER, .sds_idx = 2, .phy_idx = 2,        .smi = 2,        .phy_addr =  3,        .led_c = 0,        .led_f = HWP_NONE,.led_layout = SINGLE_SET,},
        { .mac_id = 20, .attr = HWP_ETH,   .eth = HWP_2_5GE, .medi = HWP_COPPER, .sds_idx = 3, .phy_idx = 3,        .smi = 3,        .phy_addr =  4,        .led_c = 0,        .led_f = HWP_NONE,.led_layout = SINGLE_SET,},
        { .mac_id = 21, .attr = HWP_ETH,   .eth = HWP_2_5GE, .medi = HWP_COPPER, .sds_idx = 3, .phy_idx = 3,        .smi = 3,        .phy_addr =  5,        .led_c = 0,        .led_f = HWP_NONE,.led_layout = SINGLE_SET,},
        { .mac_id = 22, .attr = HWP_ETH,   .eth = HWP_2_5GE, .medi = HWP_COPPER, .sds_idx = 3, .phy_idx = 3,        .smi = 3,        .phy_addr =  6,        .led_c = 0,        .led_f = HWP_NONE,.led_layout = SINGLE_SET,},
        { .mac_id = 23, .attr = HWP_ETH,   .eth = HWP_2_5GE, .medi = HWP_COPPER, .sds_idx = 3, .phy_idx = 3,        .smi = 3,        .phy_addr =  7,        .led_c = 0,        .led_f = HWP_NONE,.led_layout = SINGLE_SET,},
        { .mac_id = 24, .attr = HWP_ETH,   .eth = HWP_XGE,   .medi = HWP_SERDES, .sds_idx = 4, .phy_idx = HWP_NONE, .smi = HWP_NONE, .phy_addr = HWP_NONE,  .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id = 25, .attr = HWP_ETH,   .eth = HWP_XGE,   .medi = HWP_SERDES, .sds_idx = 5, .phy_idx = HWP_NONE, .smi = HWP_NONE, .phy_addr = HWP_NONE,  .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id = 26, .attr = HWP_ETH,   .eth = HWP_XGE,   .medi = HWP_SERDES, .sds_idx = 6, .phy_idx = HWP_NONE, .smi = HWP_NONE, .phy_addr = HWP_NONE,  .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id = 27, .attr = HWP_ETH,   .eth = HWP_XGE,   .medi = HWP_SERDES, .sds_idx = 7, .phy_idx = HWP_NONE, .smi = HWP_NONE, .phy_addr = HWP_NONE,  .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id = 28, .attr = HWP_CPU,   .eth = HWP_NONE,  .medi = HWP_NONE,   .sds_idx = HWP_NONE, .phy_idx = HWP_NONE, .smi = HWP_NONE, .phy_addr =  HWP_NONE, .led_c = HWP_NONE, .led_f = HWP_NONE,.led_layout = HWP_NONE,  },
        { .mac_id = HWP_END },
    },  /* port.descp */

    .led.descp = {
        .led_active = LED_ACTIVE_LOW,
        .led_if_sel = LED_IF_SEL_SERIAL,
        .led_definition_set[0].led[0] = 0xA01,        /* 10Gbps link/act */
        .led_definition_set[0].led[1] = 0xA20,        /* 1000Mbps link/act */
        .led_definition_set[0].led[2] = 0xA02,        /* 5Gbps link/act */
        .led_definition_set[0].led[3] = 0xA08,        /* 2.5Gbps link/act */
        .led_definition_set[0].led[4] = HWP_LED_END,      /* None */
    },/* led.descp */

    .serdes.descp = {
        [0] = { .sds_id = 2, .mode = RTK_MII_XSGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [1] = { .sds_id = 3, .mode = RTK_MII_XSGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [2] = { .sds_id = 4, .mode = RTK_MII_USXGMII_10GQXGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [3] = { .sds_id = 5, .mode = RTK_MII_USXGMII_10GQXGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [4] = { .sds_id = 6, .mode = RTK_MII_10GR,     .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [5] = { .sds_id = 7, .mode = RTK_MII_10GR,     .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [6] = { .sds_id = 8, .mode = RTK_MII_10GR,     .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [7] = { .sds_id = 9, .mode = RTK_MII_10GR,     .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [8] = { .sds_id = HWP_END },
    }, /* serdes.descp */

    .phy.descp = {
        [0] = { .chip = RTK_PHYTYPE_RTL8218D,       .mac_id = 0,  .phy_max = 8 },
        [1] = { .chip = RTK_PHYTYPE_RTL8218D,       .mac_id = 8,  .phy_max = 8 },
        [2] = { .chip = RTK_PHYTYPE_CUST1/* 412 */, .mac_id = 16, .phy_max = 4 },
        [3] = { .chip = RTK_PHYTYPE_CUST1/* 412 */, .mac_id = 20, .phy_max = 4 },
        [4] = { .chip = HWP_END },
    },   /* .phy.descp */

};


/*
 * hardware profile
 */
static hwp_hwProfile_t rtl9302b_2x8218d_2xcust1_4xge = {

    .identifier.name        = "RTL9302B_2x8218D_2xCUST1_4xGE",
    .identifier.id          = HWP_RTL9302B_2x8218D_2xCUST1_4XGE,

    .soc.swDescp_index      = 0,
    .soc.slaveInterruptPin  = HWP_NONE,

    .sw_count               = 1,
    .swDescp = {
        [0]                 = &rtl9302b_2x8218d_2xcust1_4xge_swDescp,
    }

};

