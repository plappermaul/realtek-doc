/*
 * Copyright(c) Realtek Semiconductor Corporation, 2015
 * All rights reserved.
 *
 * Purpose : Related implementation of the RTL9300 FPGA board
 *
 * Feature : RTL9300 FPGA board database
 *
 */


hwp_swDescp_t rtl9301_2x8218b_4x8218d_2x8295r_swDescp1 = {

    .chip_id                    = RTL9301_CHIP_ID_24G,
    .swcore_supported           = TRUE,
    .swcore_access_method       = HWP_SW_ACC_MEM,
    .swcore_spi_chip_select     = HWP_NOT_USED,
    .nic_supported              = TRUE,

    .port.descp = {
        { .mac_id =  0, .attr = HWP_ETH,             .eth = HWP_GE,   .medi = HWP_COPPER,  .sds_idx = 0,             .phy_idx = 0,        .smi = 0,        .phy_addr = 0,        .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id =  1, .attr = HWP_ETH,             .eth = HWP_GE,   .medi = HWP_COPPER,  .sds_idx = 0,             .phy_idx = 0,        .smi = 0,        .phy_addr = 1,        .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id =  2, .attr = HWP_ETH,             .eth = HWP_GE,   .medi = HWP_COPPER,  .sds_idx = 0,             .phy_idx = 0,        .smi = 0,        .phy_addr = 2,        .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id =  3, .attr = HWP_ETH,             .eth = HWP_GE,   .medi = HWP_COPPER,  .sds_idx = 0,             .phy_idx = 0,        .smi = 0,        .phy_addr = 3,        .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id =  4, .attr = HWP_ETH,             .eth = HWP_GE,   .medi = HWP_COPPER,  .sds_idx = 1,             .phy_idx = 0,        .smi = 0,        .phy_addr = 4,        .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id =  5, .attr = HWP_ETH,             .eth = HWP_GE,   .medi = HWP_COPPER,  .sds_idx = 1,             .phy_idx = 0,        .smi = 0,        .phy_addr = 5,        .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id =  6, .attr = HWP_ETH,             .eth = HWP_GE,   .medi = HWP_COPPER,  .sds_idx = 1,             .phy_idx = 0,        .smi = 0,        .phy_addr = 6,        .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id =  7, .attr = HWP_ETH,             .eth = HWP_GE,   .medi = HWP_COPPER,  .sds_idx = 1,             .phy_idx = 0,        .smi = 0,        .phy_addr = 7,        .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id =  8, .attr = HWP_ETH,             .eth = HWP_GE,   .medi = HWP_COPPER,  .sds_idx = 2,             .phy_idx = 1,        .smi = 1,        .phy_addr = 8,        .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id =  9, .attr = HWP_ETH,             .eth = HWP_GE,   .medi = HWP_COPPER,  .sds_idx = 2,             .phy_idx = 1,        .smi = 1,        .phy_addr = 9,        .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id = 10, .attr = HWP_ETH,             .eth = HWP_GE,   .medi = HWP_COPPER,  .sds_idx = 2,             .phy_idx = 1,        .smi = 1,        .phy_addr = 10,       .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id = 11, .attr = HWP_ETH,             .eth = HWP_GE,   .medi = HWP_COPPER,  .sds_idx = 2,             .phy_idx = 1,        .smi = 1,        .phy_addr = 11,       .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id = 12, .attr = HWP_ETH,             .eth = HWP_GE,   .medi = HWP_COPPER,  .sds_idx = 2,             .phy_idx = 1,        .smi = 1,        .phy_addr = 12,       .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id = 13, .attr = HWP_ETH,             .eth = HWP_GE,   .medi = HWP_COPPER,  .sds_idx = 2,             .phy_idx = 1,        .smi = 1,        .phy_addr = 13,       .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id = 14, .attr = HWP_ETH,             .eth = HWP_GE,   .medi = HWP_COPPER,  .sds_idx = 2,             .phy_idx = 1,        .smi = 1,        .phy_addr = 14,       .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id = 15, .attr = HWP_ETH,             .eth = HWP_GE,   .medi = HWP_COPPER,  .sds_idx = 2,             .phy_idx = 1,        .smi = 1,        .phy_addr = 15,       .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id = 16, .attr = HWP_ETH,             .eth = HWP_GE,   .medi = HWP_COPPER,  .sds_idx = 3,             .phy_idx = 2,        .smi = 2,        .phy_addr = 16,       .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id = 17, .attr = HWP_ETH,             .eth = HWP_GE,   .medi = HWP_COPPER,  .sds_idx = 3,             .phy_idx = 2,        .smi = 2,        .phy_addr = 17,       .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id = 18, .attr = HWP_ETH,             .eth = HWP_GE,   .medi = HWP_COPPER,  .sds_idx = 3,             .phy_idx = 2,        .smi = 2,        .phy_addr = 18,       .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id = 19, .attr = HWP_ETH,             .eth = HWP_GE,   .medi = HWP_COPPER,  .sds_idx = 3,             .phy_idx = 2,        .smi = 2,        .phy_addr = 19,       .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id = 20, .attr = HWP_ETH,             .eth = HWP_GE,   .medi = HWP_COPPER,  .sds_idx = 3,             .phy_idx = 2,        .smi = 2,        .phy_addr = 20,       .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id = 21, .attr = HWP_ETH,             .eth = HWP_GE,   .medi = HWP_COPPER,  .sds_idx = 3,             .phy_idx = 2,        .smi = 2,        .phy_addr = 21,       .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id = 22, .attr = HWP_ETH,             .eth = HWP_GE,   .medi = HWP_COPPER,  .sds_idx = 3,             .phy_idx = 2,        .smi = 2,        .phy_addr = 22,       .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id = 23, .attr = HWP_ETH,             .eth = HWP_GE,   .medi = HWP_COPPER,  .sds_idx = 3,             .phy_idx = 2,        .smi = 2,        .phy_addr = 23,       .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id = 24, .attr = HWP_ETH|HWP_CASCADE, .eth = HWP_SXGE, .medi = HWP_SERDES,  .sds_idx = SBM(4)|SBM(5), .phy_idx = HWP_NONE, .smi = HWP_NONE, .phy_addr = HWP_NONE, .led_c = HWP_NONE, .led_f = HWP_NONE,.led_layout = HWP_NONE,},
        { .mac_id = 25, .attr = HWP_ETH|HWP_CASCADE, .eth = HWP_SXGE, .medi = HWP_SERDES,  .sds_idx = SBM(6)|SBM(7), .phy_idx = HWP_NONE, .smi = HWP_NONE, .phy_addr = HWP_NONE, .led_c = HWP_NONE, .led_f = HWP_NONE,.led_layout = HWP_NONE,},
        { .mac_id = 26, .attr = HWP_ETH,             .eth = HWP_XGE,  .medi = HWP_SERDES,  .sds_idx = 8,             .phy_idx = HWP_NONE, .smi = HWP_NONE, .phy_addr = HWP_NONE, .led_c = 1,        .led_f = 1,       .led_layout = SINGLE_SET,},
        { .mac_id = 27, .attr = HWP_ETH,             .eth = HWP_XGE,  .medi = HWP_SERDES,  .sds_idx = 9,             .phy_idx = HWP_NONE, .smi = HWP_NONE, .phy_addr = HWP_NONE, .led_c = 1,        .led_f = 1,       .led_layout = SINGLE_SET,},
        { .mac_id = 28, .attr = HWP_CPU,             .eth = HWP_NONE, .medi = HWP_NONE,    .sds_idx = HWP_NONE,      .phy_idx = HWP_NONE, .smi = HWP_NONE, .phy_addr = HWP_NONE, .led_c = HWP_NONE, .led_f = HWP_NONE,.led_layout = HWP_NONE,  },
        { .mac_id = HWP_END },
    },  /* port.descp */

    .led.descp = {
        .led_active = LED_ACTIVE_LOW,
        .led_if_sel = LED_IF_SEL_SERIAL,
        .led_definition_set[0].led[0] = 0x380,        /* 10/100Mbps link */
        .led_definition_set[0].led[1] = 0x220,        /* 1000Mbps link */
        .led_definition_set[0].led[2] = 0x580,        /* 10/100Mbps Link Blinking */
        .led_definition_set[0].led[3] = 0x420,        /* 1000Mbps Link Blinking*/
        .led_definition_set[0].led[4] = HWP_LED_END,  /* None */
        .led_definition_set[1].led[0] = 0xB80,        /* 10G Link */
        .led_definition_set[1].led[1] = 0xA20,        /* 10G Link Blinking */
        .led_definition_set[1].led[2] = HWP_LED_END,  /* None */
    },/* led.descp */

    .serdes.descp = {
        [0] = { .sds_id = 0, .mode = RTK_MII_QSGMII,     .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [1] = { .sds_id = 1, .mode = RTK_MII_QSGMII,     .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [2] = { .sds_id = 2, .mode = RTK_MII_XSGMII,     .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [3] = { .sds_id = 3, .mode = RTK_MII_XSGMII,     .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [4] = { .sds_id = 4, .mode = RTK_MII_RXAUI_PLUS, .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [5] = { .sds_id = 5, .mode = RTK_MII_RXAUI_PLUS, .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [6] = { .sds_id = 6, .mode = RTK_MII_RXAUI_PLUS, .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [7] = { .sds_id = 7, .mode = RTK_MII_RXAUI_PLUS, .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [8] = { .sds_id = 8, .mode = RTK_MII_10GR,       .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [9] = { .sds_id = 9, .mode = RTK_MII_10GR,       .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [10] = { .sds_id = HWP_END },
    }, /* serdes.descp */

    .phy.descp = {
        [0] = { .chip = RTK_PHYTYPE_RTL8218B, .mac_id = 0, .phy_max = 8 },
        [1] = { .chip = RTK_PHYTYPE_RTL8218D, .mac_id = 8, .phy_max = 8 },
        [2] = { .chip = RTK_PHYTYPE_RTL8218D, .mac_id = 16,.phy_max = 8 },
        [3] = { .chip = HWP_END },
    }   /* .phy.descp */
};


hwp_swDescp_t rtl9301_2x8218b_4x8218d_2x8295r_swDescp2 = {

    .chip_id                    = RTL9301_CHIP_ID_24G,
    .swcore_supported           = TRUE,
    .swcore_access_method       = HWP_SW_ACC_SPI,
    .swcore_spi_chip_select     = 1,
    .nic_supported              = FALSE,

    .port.descp = {
        { .mac_id =  0, .attr = HWP_ETH,             .eth = HWP_GE,   .medi = HWP_COPPER,  .sds_idx = 0,             .phy_idx = 0,        .smi = 0,        .phy_addr = 0,        .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id =  1, .attr = HWP_ETH,             .eth = HWP_GE,   .medi = HWP_COPPER,  .sds_idx = 0,             .phy_idx = 0,        .smi = 0,        .phy_addr = 1,        .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id =  2, .attr = HWP_ETH,             .eth = HWP_GE,   .medi = HWP_COPPER,  .sds_idx = 0,             .phy_idx = 0,        .smi = 0,        .phy_addr = 2,        .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id =  3, .attr = HWP_ETH,             .eth = HWP_GE,   .medi = HWP_COPPER,  .sds_idx = 0,             .phy_idx = 0,        .smi = 0,        .phy_addr = 3,        .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id =  4, .attr = HWP_ETH,             .eth = HWP_GE,   .medi = HWP_COPPER,  .sds_idx = 1,             .phy_idx = 0,        .smi = 0,        .phy_addr = 4,        .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id =  5, .attr = HWP_ETH,             .eth = HWP_GE,   .medi = HWP_COPPER,  .sds_idx = 1,             .phy_idx = 0,        .smi = 0,        .phy_addr = 5,        .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id =  6, .attr = HWP_ETH,             .eth = HWP_GE,   .medi = HWP_COPPER,  .sds_idx = 1,             .phy_idx = 0,        .smi = 0,        .phy_addr = 6,        .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id =  7, .attr = HWP_ETH,             .eth = HWP_GE,   .medi = HWP_COPPER,  .sds_idx = 1,             .phy_idx = 0,        .smi = 0,        .phy_addr = 7,        .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id =  8, .attr = HWP_ETH,             .eth = HWP_GE,   .medi = HWP_COPPER,  .sds_idx = 2,             .phy_idx = 1,        .smi = 1,        .phy_addr = 8,        .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id =  9, .attr = HWP_ETH,             .eth = HWP_GE,   .medi = HWP_COPPER,  .sds_idx = 2,             .phy_idx = 1,        .smi = 1,        .phy_addr = 9,        .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id = 10, .attr = HWP_ETH,             .eth = HWP_GE,   .medi = HWP_COPPER,  .sds_idx = 2,             .phy_idx = 1,        .smi = 1,        .phy_addr = 10,       .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id = 11, .attr = HWP_ETH,             .eth = HWP_GE,   .medi = HWP_COPPER,  .sds_idx = 2,             .phy_idx = 1,        .smi = 1,        .phy_addr = 11,       .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id = 12, .attr = HWP_ETH,             .eth = HWP_GE,   .medi = HWP_COPPER,  .sds_idx = 2,             .phy_idx = 1,        .smi = 1,        .phy_addr = 12,       .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id = 13, .attr = HWP_ETH,             .eth = HWP_GE,   .medi = HWP_COPPER,  .sds_idx = 2,             .phy_idx = 1,        .smi = 1,        .phy_addr = 13,       .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id = 14, .attr = HWP_ETH,             .eth = HWP_GE,   .medi = HWP_COPPER,  .sds_idx = 2,             .phy_idx = 1,        .smi = 1,        .phy_addr = 14,       .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id = 15, .attr = HWP_ETH,             .eth = HWP_GE,   .medi = HWP_COPPER,  .sds_idx = 2,             .phy_idx = 1,        .smi = 1,        .phy_addr = 15,       .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id = 16, .attr = HWP_ETH,             .eth = HWP_GE,   .medi = HWP_COPPER,  .sds_idx = 3,             .phy_idx = 2,        .smi = 2,        .phy_addr = 16,       .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id = 17, .attr = HWP_ETH,             .eth = HWP_GE,   .medi = HWP_COPPER,  .sds_idx = 3,             .phy_idx = 2,        .smi = 2,        .phy_addr = 17,       .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id = 18, .attr = HWP_ETH,             .eth = HWP_GE,   .medi = HWP_COPPER,  .sds_idx = 3,             .phy_idx = 2,        .smi = 2,        .phy_addr = 18,       .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id = 19, .attr = HWP_ETH,             .eth = HWP_GE,   .medi = HWP_COPPER,  .sds_idx = 3,             .phy_idx = 2,        .smi = 2,        .phy_addr = 19,       .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id = 20, .attr = HWP_ETH,             .eth = HWP_GE,   .medi = HWP_COPPER,  .sds_idx = 3,             .phy_idx = 2,        .smi = 2,        .phy_addr = 20,       .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id = 21, .attr = HWP_ETH,             .eth = HWP_GE,   .medi = HWP_COPPER,  .sds_idx = 3,             .phy_idx = 2,        .smi = 2,        .phy_addr = 21,       .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id = 22, .attr = HWP_ETH,             .eth = HWP_GE,   .medi = HWP_COPPER,  .sds_idx = 3,             .phy_idx = 2,        .smi = 2,        .phy_addr = 22,       .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id = 23, .attr = HWP_ETH,             .eth = HWP_GE,   .medi = HWP_COPPER,  .sds_idx = 3,             .phy_idx = 2,        .smi = 2,        .phy_addr = 23,       .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id = 24, .attr = HWP_ETH|HWP_CASCADE, .eth = HWP_SXGE, .medi = HWP_SERDES,  .sds_idx = SBM(4)|SBM(5), .phy_idx = HWP_NONE, .smi = HWP_NONE, .phy_addr = HWP_NONE, .led_c = HWP_NONE, .led_f = HWP_NONE,.led_layout = HWP_NONE,},
        { .mac_id = 25, .attr = HWP_ETH|HWP_CASCADE, .eth = HWP_SXGE, .medi = HWP_SERDES,  .sds_idx = SBM(6)|SBM(7), .phy_idx = HWP_NONE, .smi = HWP_NONE, .phy_addr = HWP_NONE, .led_c = HWP_NONE, .led_f = HWP_NONE,.led_layout = HWP_NONE,},
#if defined(CONFIG_SDK_RTL8295R)
        { .mac_id = 26, .attr = HWP_ETH,             .eth = HWP_XGE,  .medi = HWP_FIBER,   .sds_idx = 8,             .phy_idx = 3,        .smi = 3,        .phy_addr = 27,        .led_c = 1,       .led_f = 1,       .led_layout = SINGLE_SET,},
        { .mac_id = 27, .attr = HWP_ETH,             .eth = HWP_XGE,  .medi = HWP_FIBER,   .sds_idx = 9,             .phy_idx = 4,        .smi = 3,        .phy_addr = 26,        .led_c = 1,       .led_f = 1,       .led_layout = SINGLE_SET,},
#else
        { .mac_id = 26, .attr = HWP_ETH,             .eth = HWP_XGE,  .medi = HWP_SERDES,  .sds_idx = 8,             .phy_idx = HWP_NONE, .smi = HWP_NONE, .phy_addr = HWP_NONE,  .led_c = 1,       .led_f = 1,       .led_layout = SINGLE_SET,},
        { .mac_id = 27, .attr = HWP_ETH,             .eth = HWP_XGE,  .medi = HWP_SERDES,  .sds_idx = 9,             .phy_idx = HWP_NONE, .smi = HWP_NONE, .phy_addr = HWP_NONE,  .led_c = 1,       .led_f = 1,       .led_layout = SINGLE_SET,},
#endif
        { .mac_id = 28, .attr = HWP_CPU,             .eth = HWP_NONE, .medi = HWP_NONE,    .sds_idx = HWP_NONE,      .phy_idx = HWP_NONE, .smi = HWP_NONE, .phy_addr = HWP_NONE, .led_c = HWP_NONE, .led_f = HWP_NONE,.led_layout = HWP_NONE,  },
        { .mac_id = HWP_END },
    },  /* port.descp */

    .led.descp = {
        .led_active = LED_ACTIVE_LOW,
        .led_if_sel = LED_IF_SEL_SERIAL,
        .led_definition_set[0].led[0] = 0xB80,        /* 10/100Mbps link */
        .led_definition_set[0].led[1] = 0xA20,        /* 1000Mbps link */
        .led_definition_set[0].led[2] = 0x580,        /* 10/100Mbps Link Blinking */
        .led_definition_set[0].led[3] = 0x420,        /* 1000Mbps Link Blinking*/
        .led_definition_set[0].led[4] = HWP_LED_END,  /* None */
        .led_definition_set[1].led[0] = 0xA01,        /* 10G Link */
        .led_definition_set[1].led[1] = 0x601,        /* 10G Link Blinking */
        .led_definition_set[1].led[2] = HWP_LED_END,  /* None */
    },/* led.descp */

    .serdes.descp = {
        [0] = { .sds_id = 0, .mode = RTK_MII_QSGMII,     .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [1] = { .sds_id = 1, .mode = RTK_MII_QSGMII,     .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [2] = { .sds_id = 2, .mode = RTK_MII_XSGMII,     .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [3] = { .sds_id = 3, .mode = RTK_MII_XSGMII,     .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [4] = { .sds_id = 4, .mode = RTK_MII_RXAUI_PLUS, .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [5] = { .sds_id = 5, .mode = RTK_MII_RXAUI_PLUS, .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [6] = { .sds_id = 6, .mode = RTK_MII_RXAUI_PLUS, .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [7] = { .sds_id = 7, .mode = RTK_MII_RXAUI_PLUS, .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [8] = { .sds_id = 8, .mode = RTK_MII_10GR,       .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [9] = { .sds_id = 9, .mode = RTK_MII_10GR,       .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [10] = { .sds_id = HWP_END },
    }, /* serdes.descp */

    .phy.descp = {
        [0] = { .chip = RTK_PHYTYPE_RTL8218B, .mac_id = 0, .phy_max = 8 },
        [1] = { .chip = RTK_PHYTYPE_RTL8218D, .mac_id = 8, .phy_max = 8 },
        [2] = { .chip = RTK_PHYTYPE_RTL8218D, .mac_id = 16,.phy_max = 8 },
#if defined(CONFIG_SDK_RTL8295R)
        [3] = { .chip = RTK_PHYTYPE_RTL8295R, .mac_id = 26,.phy_max = 1 },
        [4] = { .chip = RTK_PHYTYPE_RTL8295R, .mac_id = 27,.phy_max = 1 },
        [5] = { .chip = HWP_END },
#else
        [3] = { .chip = HWP_END },
#endif
    }   /* .phy.descp */
};


/*
 * hardware profile
 */
static hwp_hwProfile_t rtl9301_2x8218b_4x8218d_2x8295r_cascade = {

    .identifier.name        = "RTL9301_2x8218B_4x8218D_2x8295R_CASCADE",
    .identifier.id          = HWP_RTL9301_2x8218B_4x8218D_2x8295R_CASCADE,

    .soc.swDescp_index      = 0,
    .soc.slaveInterruptPin  = 18,
    .sw_count               = 2,
    .swDescp = {
        [0]                 = &rtl9301_2x8218b_4x8218d_2x8295r_swDescp1,
        [1]                 = &rtl9301_2x8218b_4x8218d_2x8295r_swDescp2,
    }
};


