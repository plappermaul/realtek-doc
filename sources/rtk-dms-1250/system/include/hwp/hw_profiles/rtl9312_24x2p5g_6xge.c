/*
 * Copyright(c) Realtek Semiconductor Corporation, 2015
 * All rights reserved.
 *
 * Purpose : Related implementation of the RTL9300 FPGA board
 *
 * Feature : RTL9300 FPGA board database
 *
 */



/*
 * 12*10G mode
 */
static hwp_swDescp_t rtl9312_24x2p5g_6xge_swDescp = {

    .chip_id                    = RTL9312_CHIP_ID,
    .swcore_supported           = TRUE,
    .swcore_access_method       = HWP_SW_ACC_MEM,
    .swcore_spi_chip_select     = HWP_NOT_USED,
    .nic_supported              = TRUE,

    .port.descp = {
        { .mac_id =  0, .attr = HWP_ETH,   .eth = HWP_2_5GE, .medi = HWP_COPPER,  .sds_idx = 0,       .phy_idx = 0,        .smi = 0,        .phy_addr = 0,        .led_c = 2,        .led_f = HWP_NONE, .led_layout = SINGLE_SET,},
        { .mac_id =  1, .attr = HWP_ETH,   .eth = HWP_2_5GE, .medi = HWP_COPPER,  .sds_idx = 0,       .phy_idx = 0,        .smi = 0,        .phy_addr = 1,        .led_c = 2,        .led_f = HWP_NONE, .led_layout = SINGLE_SET,},
        { .mac_id =  4, .attr = HWP_ETH,   .eth = HWP_2_5GE, .medi = HWP_COPPER,  .sds_idx = 0,       .phy_idx = 0,        .smi = 0,        .phy_addr = 2,        .led_c = 2,        .led_f = HWP_NONE, .led_layout = SINGLE_SET,},
        { .mac_id =  5, .attr = HWP_ETH,   .eth = HWP_2_5GE, .medi = HWP_COPPER,  .sds_idx = 0,       .phy_idx = 0,        .smi = 0,        .phy_addr = 3,        .led_c = 2,        .led_f = HWP_NONE, .led_layout = SINGLE_SET,},
        { .mac_id =  8, .attr = HWP_ETH,   .eth = HWP_2_5GE, .medi = HWP_COPPER,  .sds_idx = 1,       .phy_idx = 1,        .smi = 0,        .phy_addr = 4,        .led_c = 2,        .led_f = HWP_NONE, .led_layout = SINGLE_SET,},
        { .mac_id =  9, .attr = HWP_ETH,   .eth = HWP_2_5GE, .medi = HWP_COPPER,  .sds_idx = 1,       .phy_idx = 1,        .smi = 0,        .phy_addr = 5,        .led_c = 2,        .led_f = HWP_NONE, .led_layout = SINGLE_SET,},
        { .mac_id = 12, .attr = HWP_ETH,   .eth = HWP_2_5GE, .medi = HWP_COPPER,  .sds_idx = 1,       .phy_idx = 1,        .smi = 0,        .phy_addr = 6,        .led_c = 2,        .led_f = HWP_NONE, .led_layout = SINGLE_SET,},
        { .mac_id = 13, .attr = HWP_ETH,   .eth = HWP_2_5GE, .medi = HWP_COPPER,  .sds_idx = 1,       .phy_idx = 1,        .smi = 0,        .phy_addr = 7,        .led_c = 2,        .led_f = HWP_NONE, .led_layout = SINGLE_SET,},
        { .mac_id = 16, .attr = HWP_ETH,   .eth = HWP_2_5GE, .medi = HWP_COPPER,  .sds_idx = 2,       .phy_idx = 2,        .smi = 0,        .phy_addr = 8,        .led_c = 2,        .led_f = HWP_NONE, .led_layout = SINGLE_SET,},
        { .mac_id = 17, .attr = HWP_ETH,   .eth = HWP_2_5GE, .medi = HWP_COPPER,  .sds_idx = 2,       .phy_idx = 2,        .smi = 0,        .phy_addr = 9,        .led_c = 2,        .led_f = HWP_NONE, .led_layout = SINGLE_SET,},
        { .mac_id = 20, .attr = HWP_ETH,   .eth = HWP_2_5GE, .medi = HWP_COPPER,  .sds_idx = 2,       .phy_idx = 2,        .smi = 0,        .phy_addr = 10,       .led_c = 2,        .led_f = HWP_NONE, .led_layout = SINGLE_SET,},
        { .mac_id = 21, .attr = HWP_ETH,   .eth = HWP_2_5GE, .medi = HWP_COPPER,  .sds_idx = 2,       .phy_idx = 2,        .smi = 0,        .phy_addr = 11,       .led_c = 2,        .led_f = HWP_NONE, .led_layout = SINGLE_SET,},
        { .mac_id = 24, .attr = HWP_ETH,   .eth = HWP_2_5GE, .medi = HWP_COPPER,  .sds_idx = 3,       .phy_idx = 3,        .smi = 0,        .phy_addr = 12,       .led_c = 2,        .led_f = HWP_NONE, .led_layout = SINGLE_SET,},
        { .mac_id = 25, .attr = HWP_ETH,   .eth = HWP_2_5GE, .medi = HWP_COPPER,  .sds_idx = 3,       .phy_idx = 3,        .smi = 0,        .phy_addr = 13,       .led_c = 2,        .led_f = HWP_NONE, .led_layout = SINGLE_SET,},
        { .mac_id = 28, .attr = HWP_ETH,   .eth = HWP_2_5GE, .medi = HWP_COPPER,  .sds_idx = 3,       .phy_idx = 3,        .smi = 0,        .phy_addr = 14,       .led_c = 2,        .led_f = HWP_NONE, .led_layout = SINGLE_SET,},
        { .mac_id = 29, .attr = HWP_ETH,   .eth = HWP_2_5GE, .medi = HWP_COPPER,  .sds_idx = 3,       .phy_idx = 3,        .smi = 0,        .phy_addr = 15,       .led_c = 2,        .led_f = HWP_NONE, .led_layout = SINGLE_SET,},
        { .mac_id = 32, .attr = HWP_ETH,   .eth = HWP_2_5GE, .medi = HWP_COPPER,  .sds_idx = 4,       .phy_idx = 4,        .smi = 0,        .phy_addr = 16,       .led_c = 2,        .led_f = HWP_NONE, .led_layout = SINGLE_SET,},
        { .mac_id = 33, .attr = HWP_ETH,   .eth = HWP_2_5GE, .medi = HWP_COPPER,  .sds_idx = 4,       .phy_idx = 4,        .smi = 0,        .phy_addr = 17,       .led_c = 2,        .led_f = HWP_NONE, .led_layout = SINGLE_SET,},
        { .mac_id = 36, .attr = HWP_ETH,   .eth = HWP_2_5GE, .medi = HWP_COPPER,  .sds_idx = 4,       .phy_idx = 4,        .smi = 0,        .phy_addr = 18,       .led_c = 2,        .led_f = HWP_NONE, .led_layout = SINGLE_SET,},
        { .mac_id = 37, .attr = HWP_ETH,   .eth = HWP_2_5GE, .medi = HWP_COPPER,  .sds_idx = 4,       .phy_idx = 4,        .smi = 0,        .phy_addr = 19,       .led_c = 2,        .led_f = HWP_NONE, .led_layout = SINGLE_SET,},
        { .mac_id = 40, .attr = HWP_ETH,   .eth = HWP_2_5GE, .medi = HWP_COPPER,  .sds_idx = 5,       .phy_idx = 5,        .smi = 0,        .phy_addr = 20,       .led_c = 2,        .led_f = HWP_NONE, .led_layout = SINGLE_SET,},
        { .mac_id = 41, .attr = HWP_ETH,   .eth = HWP_2_5GE, .medi = HWP_COPPER,  .sds_idx = 5,       .phy_idx = 5,        .smi = 0,        .phy_addr = 21,       .led_c = 2,        .led_f = HWP_NONE, .led_layout = SINGLE_SET,},
        { .mac_id = 44, .attr = HWP_ETH,   .eth = HWP_2_5GE, .medi = HWP_COPPER,  .sds_idx = 5,       .phy_idx = 5,        .smi = 0,        .phy_addr = 22,       .led_c = 2,        .led_f = HWP_NONE, .led_layout = SINGLE_SET,},
        { .mac_id = 45, .attr = HWP_ETH,   .eth = HWP_2_5GE, .medi = HWP_COPPER,  .sds_idx = 5,       .phy_idx = 5,        .smi = 0,        .phy_addr = 23,       .led_c = 2,        .led_f = HWP_NONE, .led_layout = SINGLE_SET,},
        { .mac_id = 48, .attr = HWP_ETH,   .eth = HWP_XGE,   .medi = HWP_SERDES,  .sds_idx = 6,       .phy_idx = HWP_NONE, .smi = HWP_NONE, .phy_addr = HWP_NONE, .led_c = HWP_NONE, .led_f = 3,        .led_layout = SINGLE_SET,},
        { .mac_id = 50, .attr = HWP_ETH,   .eth = HWP_XGE,   .medi = HWP_SERDES,  .sds_idx = 7,       .phy_idx = HWP_NONE, .smi = HWP_NONE, .phy_addr = HWP_NONE, .led_c = HWP_NONE, .led_f = 3,        .led_layout = SINGLE_SET,},
        { .mac_id = 52, .attr = HWP_ETH,   .eth = HWP_XGE,   .medi = HWP_SERDES,  .sds_idx = 8,       .phy_idx = HWP_NONE, .smi = HWP_NONE, .phy_addr = HWP_NONE, .led_c = HWP_NONE, .led_f = 3,        .led_layout = SINGLE_SET,},
        { .mac_id = 53, .attr = HWP_ETH,   .eth = HWP_XGE,   .medi = HWP_SERDES,  .sds_idx = 9,       .phy_idx = HWP_NONE, .smi = HWP_NONE, .phy_addr = HWP_NONE, .led_c = HWP_NONE, .led_f = 3,        .led_layout = SINGLE_SET,},
        { .mac_id = 54, .attr = HWP_ETH,   .eth = HWP_XGE,   .medi = HWP_SERDES,  .sds_idx = 10,      .phy_idx = HWP_NONE, .smi = HWP_NONE, .phy_addr = HWP_NONE, .led_c = HWP_NONE, .led_f = 3,        .led_layout = SINGLE_SET,},
        { .mac_id = 55, .attr = HWP_ETH,   .eth = HWP_XGE,   .medi = HWP_SERDES,  .sds_idx = 11,      .phy_idx = HWP_NONE, .smi = HWP_NONE, .phy_addr = HWP_NONE, .led_c = HWP_NONE, .led_f = 3,        .led_layout = SINGLE_SET,},
        { .mac_id = 56, .attr = HWP_CPU,   .eth = HWP_NONE,  .medi = HWP_NONE,    .sds_idx = HWP_NONE,.phy_idx = HWP_NONE, .smi = HWP_NONE, .phy_addr = HWP_NONE, .led_c = HWP_NONE, .led_f = HWP_NONE, .led_layout = HWP_NONE,  },
        { .mac_id = HWP_END },
    },  /* port.descp */

    .led.descp = {
        .led_active = LED_ACTIVE_HIGH,
        .led_if_sel = LED_IF_SEL_SINGLE_COLOR_SCAN,
        .led_definition_set[2].led[0] = 0x0A01,
        .led_definition_set[2].led[1] = 0x0A20,
        .led_definition_set[2].led[2] = HWP_LED_END,      /* None */
        .led_definition_set[3].led[0] = 0x0A01,
        .led_definition_set[3].led[1] = 0x0A20,
        .led_definition_set[3].led[2] = HWP_LED_END,      /* None */
    },/* led.descp */

    .serdes.descp = {
        [0] = { .sds_id = 2,   .mode = RTK_MII_USXGMII_10GQXGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [1] = { .sds_id = 3,   .mode = RTK_MII_USXGMII_10GQXGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [2] = { .sds_id = 4,   .mode = RTK_MII_USXGMII_10GQXGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [3] = { .sds_id = 5,   .mode = RTK_MII_USXGMII_10GQXGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [4] = { .sds_id = 6,   .mode = RTK_MII_USXGMII_10GQXGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [5] = { .sds_id = 7,   .mode = RTK_MII_USXGMII_10GQXGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [6] = { .sds_id = 8,   .mode = RTK_MII_10GR,               .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_CHANGE },
        [7] = { .sds_id = 9,   .mode = RTK_MII_10GR,               .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_CHANGE },
        [8] = { .sds_id = 10,  .mode = RTK_MII_10GR,               .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_CHANGE },
        [9] = { .sds_id = 11,  .mode = RTK_MII_10GR,               .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_CHANGE },
        [10] = { .sds_id = 12, .mode = RTK_MII_10GR,               .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_CHANGE },
        [11] = { .sds_id = 13, .mode = RTK_MII_10GR,               .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_CHANGE },
        [12] = { .sds_id = HWP_END },
    }, /* serdes.descp */

    .phy.descp = {
        [0] = { .chip = RTK_PHYTYPE_CUST1, .mac_id = 0,  .phy_max = 4 },
        [1] = { .chip = RTK_PHYTYPE_CUST1, .mac_id = 8,  .phy_max = 4 },
        [2] = { .chip = RTK_PHYTYPE_CUST1, .mac_id = 16, .phy_max = 4 },
        [3] = { .chip = RTK_PHYTYPE_CUST1, .mac_id = 24, .phy_max = 4 },
        [4] = { .chip = RTK_PHYTYPE_CUST1, .mac_id = 32, .phy_max = 4 },
        [5] = { .chip = RTK_PHYTYPE_CUST1, .mac_id = 40, .phy_max = 4 },
        [6] = { .chip = HWP_END },
    }   /* .phy.descp */
};


/*
 * hardware profile
 */
static hwp_hwProfile_t rtl9312_24x2p5g_6xge = {

    .identifier.name        = "RTL9312_24X2P5G_6XGE",
    .identifier.id          = HWP_RTL9312_24X2P5G_6XGE,

    .soc.swDescp_index      = 0,
    .soc.slaveInterruptPin  = HWP_NONE,

    .sw_count               = 1,
    .swDescp = {
        [0]                 = &rtl9312_24x2p5g_6xge_swDescp,
    }

};
