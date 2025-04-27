/*
 * Copyright(c) Realtek Semiconductor Corporation, 2016
 * All rights reserved.
 *
 * Purpose : Related implementation of the RTL8351M DEMO board.
 *
 * Feature : RTL8351M DEMO board database.
 *
 */

static hwp_swDescp_t rtl8351m_demo_swDescp = {

    .chip_id                    = RTL8351M_CHIP_ID,
    .swcore_supported           = TRUE,
    .swcore_access_method       = HWP_SW_ACC_MEM,
    .swcore_spi_chip_select     = HWP_NOT_USED,
    .nic_supported              = TRUE,

    .port.descp = {
        { .mac_id =  0, .attr = HWP_ETH,   .eth = HWP_FE,   .medi = HWP_COPPER, .sds_idx =  0,       .phy_idx = 0,        .smi = 0, .phy_addr =  0,       .led_c = 0,        .led_f = 0,        .led_layout = SINGLE_SET, },
        { .mac_id =  1, .attr = HWP_ETH,   .eth = HWP_FE,   .medi = HWP_COPPER, .sds_idx =  0,       .phy_idx = 0,        .smi = 0, .phy_addr =  1,       .led_c = 0,        .led_f = 0,        .led_layout = SINGLE_SET, },
        { .mac_id =  2, .attr = HWP_ETH,   .eth = HWP_FE,   .medi = HWP_COPPER, .sds_idx =  0,       .phy_idx = 0,        .smi = 0, .phy_addr =  2,       .led_c = 0,        .led_f = 0,        .led_layout = SINGLE_SET, },
        { .mac_id =  3, .attr = HWP_ETH,   .eth = HWP_FE,   .medi = HWP_COPPER, .sds_idx =  0,       .phy_idx = 0,        .smi = 0, .phy_addr =  3,       .led_c = 0,        .led_f = 0,        .led_layout = SINGLE_SET, },
        { .mac_id =  4, .attr = HWP_ETH,   .eth = HWP_FE,   .medi = HWP_COPPER, .sds_idx =  1,       .phy_idx = 0,        .smi = 0, .phy_addr =  4,       .led_c = 0,        .led_f = 0,        .led_layout = SINGLE_SET, },
        { .mac_id =  5, .attr = HWP_ETH,   .eth = HWP_FE,   .medi = HWP_COPPER, .sds_idx =  1,       .phy_idx = 0,        .smi = 0, .phy_addr =  5,       .led_c = 0,        .led_f = 0,        .led_layout = SINGLE_SET, },
        { .mac_id =  6, .attr = HWP_ETH,   .eth = HWP_FE,   .medi = HWP_COPPER, .sds_idx =  1,       .phy_idx = 0,        .smi = 0, .phy_addr =  6,       .led_c = 0,        .led_f = 0,        .led_layout = SINGLE_SET, },
        { .mac_id =  7, .attr = HWP_ETH,   .eth = HWP_FE,   .medi = HWP_COPPER, .sds_idx =  1,       .phy_idx = 0,        .smi = 0, .phy_addr =  7,       .led_c = 0,        .led_f = 0,        .led_layout = SINGLE_SET, },
        { .mac_id =  8, .attr = HWP_ETH,   .eth = HWP_FE,   .medi = HWP_COPPER, .sds_idx =  2,       .phy_idx = 1,        .smi = 0, .phy_addr =  8,       .led_c = 0,        .led_f = 0,        .led_layout = SINGLE_SET, },
        { .mac_id =  9, .attr = HWP_ETH,   .eth = HWP_FE,   .medi = HWP_COPPER, .sds_idx =  2,       .phy_idx = 1,        .smi = 0, .phy_addr =  9,       .led_c = 0,        .led_f = 0,        .led_layout = SINGLE_SET, },
        { .mac_id = 10, .attr = HWP_ETH,   .eth = HWP_FE,   .medi = HWP_COPPER, .sds_idx =  2,       .phy_idx = 1,        .smi = 0, .phy_addr = 10,       .led_c = 0,        .led_f = 0,        .led_layout = SINGLE_SET, },
        { .mac_id = 11, .attr = HWP_ETH,   .eth = HWP_FE,   .medi = HWP_COPPER, .sds_idx =  2,       .phy_idx = 1,        .smi = 0, .phy_addr = 11,       .led_c = 0,        .led_f = 0,        .led_layout = SINGLE_SET, },
        { .mac_id = 12, .attr = HWP_ETH,   .eth = HWP_FE,   .medi = HWP_COPPER, .sds_idx =  3,       .phy_idx = 1,        .smi = 0, .phy_addr = 12,       .led_c = 0,        .led_f = 0,        .led_layout = SINGLE_SET, },
        { .mac_id = 13, .attr = HWP_ETH,   .eth = HWP_FE,   .medi = HWP_COPPER, .sds_idx =  3,       .phy_idx = 1,        .smi = 0, .phy_addr = 13,       .led_c = 0,        .led_f = 0,        .led_layout = SINGLE_SET, },
        { .mac_id = 14, .attr = HWP_ETH,   .eth = HWP_FE,   .medi = HWP_COPPER, .sds_idx =  3,       .phy_idx = 1,        .smi = 0, .phy_addr = 14,       .led_c = 0,        .led_f = 0,        .led_layout = SINGLE_SET, },
        { .mac_id = 15, .attr = HWP_ETH,   .eth = HWP_FE,   .medi = HWP_COPPER, .sds_idx =  3,       .phy_idx = 1,        .smi = 0, .phy_addr = 15,       .led_c = 0,        .led_f = 0,        .led_layout = SINGLE_SET, },
        { .mac_id = 16, .attr = HWP_ETH,   .eth = HWP_FE,   .medi = HWP_COPPER, .sds_idx =  4,       .phy_idx = 2,        .smi = 0, .phy_addr = 16,       .led_c = 0,        .led_f = 0,        .led_layout = SINGLE_SET, },
        { .mac_id = 17, .attr = HWP_ETH,   .eth = HWP_FE,   .medi = HWP_COPPER, .sds_idx =  4,       .phy_idx = 2,        .smi = 0, .phy_addr = 17,       .led_c = 0,        .led_f = 0,        .led_layout = SINGLE_SET, },
        { .mac_id = 18, .attr = HWP_ETH,   .eth = HWP_FE,   .medi = HWP_COPPER, .sds_idx =  4,       .phy_idx = 2,        .smi = 0, .phy_addr = 18,       .led_c = 0,        .led_f = 0,        .led_layout = SINGLE_SET, },
        { .mac_id = 19, .attr = HWP_ETH,   .eth = HWP_FE,   .medi = HWP_COPPER, .sds_idx =  4,       .phy_idx = 2,        .smi = 0, .phy_addr = 19,       .led_c = 0,        .led_f = 0,        .led_layout = SINGLE_SET, },
        { .mac_id = 20, .attr = HWP_ETH,   .eth = HWP_FE,   .medi = HWP_COPPER, .sds_idx =  5,       .phy_idx = 2,        .smi = 0, .phy_addr = 20,       .led_c = 0,        .led_f = 0,        .led_layout = SINGLE_SET, },
        { .mac_id = 21, .attr = HWP_ETH,   .eth = HWP_FE,   .medi = HWP_COPPER, .sds_idx =  5,       .phy_idx = 2,        .smi = 0, .phy_addr = 21,       .led_c = 0,        .led_f = 0,        .led_layout = SINGLE_SET, },
        { .mac_id = 22, .attr = HWP_ETH,   .eth = HWP_FE,   .medi = HWP_COPPER, .sds_idx =  5,       .phy_idx = 2,        .smi = 0, .phy_addr = 22,       .led_c = 0,        .led_f = 0,        .led_layout = SINGLE_SET, },
        { .mac_id = 23, .attr = HWP_ETH,   .eth = HWP_FE,   .medi = HWP_COPPER, .sds_idx =  5,       .phy_idx = 2,        .smi = 0, .phy_addr = 23,       .led_c = 0,        .led_f = 0,        .led_layout = SINGLE_SET, },
        { .mac_id = 24, .attr = HWP_ETH,   .eth = HWP_GE,   .medi = HWP_COMBO,  .sds_idx =  6,       .phy_idx = 3,        .smi = 1, .phy_addr =  0,       .led_c = 0,        .led_f = 0,        .led_layout = DOUBLE_SET, },
        { .mac_id = 25, .attr = HWP_ETH,   .eth = HWP_GE,   .medi = HWP_COMBO,  .sds_idx =  6,       .phy_idx = 3,        .smi = 1, .phy_addr =  1,       .led_c = 0,        .led_f = 0,        .led_layout = DOUBLE_SET, },
        { .mac_id = 26, .attr = HWP_ETH,   .eth = HWP_GE,   .medi = HWP_COMBO,  .sds_idx =  6,       .phy_idx = 3,        .smi = 1, .phy_addr =  2,       .led_c = 0,        .led_f = 0,        .led_layout = DOUBLE_SET, },
        { .mac_id = 27, .attr = HWP_ETH,   .eth = HWP_GE,   .medi = HWP_COMBO,  .sds_idx =  6,       .phy_idx = 3,        .smi = 1, .phy_addr =  3,       .led_c = 0,        .led_f = 0,        .led_layout = DOUBLE_SET, },
        { .mac_id = 52, .attr = HWP_CPU,   .eth = HWP_NONE, .medi = HWP_NONE,   .sds_idx = HWP_NONE, .phy_idx = HWP_NONE, .smi = HWP_NONE, .phy_addr = HWP_NONE, .led_c = HWP_NONE, .led_f = HWP_NONE, .led_layout = HWP_NONE,   },
        { .mac_id = HWP_END},
    },  /* port.descp */

    /* LED layout settting */
    .led.descp = {
        .led_active = LED_ACTIVE_HIGH,
        .led_if_sel = LED_IF_SEL_BI_COLOR_SCAN,
        /* LED definition - set 0 */
        .led_definition_set[0].led[0] = 0xF,    /* 10/100Mbps link/act */
        .led_definition_set[0].led[1] = 0xA,    /* 1000Mbps link/act */
        .led_definition_set[0].led[2] = 0x6,    /* duplex mode */
        .led_definition_set[0].led[3] = HWP_LED_END,
    },/* led.descp */

    .serdes.descp = {
        [0]  = { .sds_id = 0,  .mode = RTK_MII_QSGMII, .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [1]  = { .sds_id = 1,  .mode = RTK_MII_QSGMII, .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [2]  = { .sds_id = 2,  .mode = RTK_MII_QSGMII, .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [3]  = { .sds_id = 3,  .mode = RTK_MII_QSGMII, .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [4]  = { .sds_id = 4,  .mode = RTK_MII_QSGMII, .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [5]  = { .sds_id = 5,  .mode = RTK_MII_QSGMII, .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [6]  = { .sds_id = 6,  .mode = RTK_MII_QSGMII, .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [7]  = { .sds_id = HWP_END},
    },  /* serdes.descp */

    .phy.descp = {
        [0] = { .chip = RTK_PHYTYPE_RTL8208L, .mac_id = 0 , .phy_max = 8 },
        [1] = { .chip = RTK_PHYTYPE_RTL8208L, .mac_id = 8 , .phy_max = 8 },
        [2] = { .chip = RTK_PHYTYPE_RTL8208L, .mac_id = 16, .phy_max = 8 },
        [3] = { .chip = RTK_PHYTYPE_RTL8214B, .mac_id = 24, .phy_max = 4 },
        [4] = { .chip = HWP_END},
    },   /* .phy.descp */
};


/*
 * hardware profile
 */
static hwp_hwProfile_t rtl8351m_demo = {

    .identifier.name        = "RTL8351M",
    .identifier.id          = HWP_RTL8351M,

    .soc.swDescp_index      = 0,
    .soc.slaveInterruptPin  = HWP_NONE,

    .sw_count               = 1,
    .swDescp = {
        [0]                 = &rtl8351m_demo_swDescp,
    }

};
