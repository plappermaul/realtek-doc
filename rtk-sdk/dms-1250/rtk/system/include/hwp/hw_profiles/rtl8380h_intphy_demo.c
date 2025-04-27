/*
 * Copyright(c) Realtek Semiconductor Corporation, 2016
 * All rights reserved.
 *
 * Purpose : Related implementation of the RTL8380H_INTPHY_2FIB_1G_DEMO demo board.
 *
 * Feature : RTL8380H_INTPHY_2FIB_1G_DEMO demo board database
 *
 */


static hwp_swDescp_t rtl8380h_intphy_demo_swDescp = {

    .chip_id                    = RTL8380M_CHIP_ID,
    .swcore_supported           = TRUE,
    .swcore_access_method       = HWP_SW_ACC_MEM,
    .swcore_spi_chip_select     = HWP_NOT_USED,
    .nic_supported              = TRUE,

    .port.descp = {
        { .mac_id =  8, .attr = HWP_ETH,   .eth = HWP_GE,   .medi = HWP_COPPER, .sds_idx = HWP_NONE, .phy_idx = 0,        .smi = 0,        .phy_addr = 8,        .led_c = 0,        .led_f = HWP_NONE, .led_layout = SINGLE_SET, },
        { .mac_id =  9, .attr = HWP_ETH,   .eth = HWP_GE,   .medi = HWP_COPPER, .sds_idx = HWP_NONE, .phy_idx = 0,        .smi = 0,        .phy_addr = 9,        .led_c = 0,        .led_f = HWP_NONE, .led_layout = SINGLE_SET, },
        { .mac_id = 10, .attr = HWP_ETH,   .eth = HWP_GE,   .medi = HWP_COPPER, .sds_idx = HWP_NONE, .phy_idx = 0,        .smi = 0,        .phy_addr =10,        .led_c = 0,        .led_f = HWP_NONE, .led_layout = SINGLE_SET, },
        { .mac_id = 11, .attr = HWP_ETH,   .eth = HWP_GE,   .medi = HWP_COPPER, .sds_idx = HWP_NONE, .phy_idx = 0,        .smi = 0,        .phy_addr =11,        .led_c = 0,        .led_f = HWP_NONE, .led_layout = SINGLE_SET, },
        { .mac_id = 12, .attr = HWP_ETH,   .eth = HWP_GE,   .medi = HWP_COPPER, .sds_idx = HWP_NONE, .phy_idx = 0,        .smi = 0,        .phy_addr =12,        .led_c = 0,        .led_f = HWP_NONE, .led_layout = SINGLE_SET, },
        { .mac_id = 13, .attr = HWP_ETH,   .eth = HWP_GE,   .medi = HWP_COPPER, .sds_idx = HWP_NONE, .phy_idx = 0,        .smi = 0,        .phy_addr =13,        .led_c = 0,        .led_f = HWP_NONE, .led_layout = SINGLE_SET, },
        { .mac_id = 28, .attr = HWP_CPU,   .eth = HWP_NONE, .medi = HWP_NONE,   .sds_idx = HWP_NONE, .phy_idx = HWP_NONE, .smi = HWP_NONE, .phy_addr = HWP_NONE, .led_c = HWP_NONE, .led_f = HWP_NONE, .led_layout = HWP_NONE,   },
        { .mac_id = HWP_END },
    },  /* port.descp */

    .led.descp = {
        .led_active = LED_ACTIVE_HIGH,
        .led_if_sel = LED_IF_SEL_SINGLE_COLOR_SCAN,
        .led_definition_set[0].led[0] = 0x7,          /* 10/100Mbps link/act */
        .led_definition_set[0].led[1] = 0x0,          /* link/act */
        .led_definition_set[0].led[2] = HWP_LED_END,      /* None */
    },/* led.descp */

    .serdes.descp = {
        [0] = { .sds_id = 0, .mode = RTK_MII_DISABLE,         .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [1] = { .sds_id = 1, .mode = RTK_MII_DISABLE,         .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [2] = { .sds_id = 2, .mode = RTK_MII_DISABLE,         .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [3] = { .sds_id = 3, .mode = RTK_MII_DISABLE,         .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [4] = { .sds_id = 4, .mode = RTK_MII_DISABLE,    .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [5] = { .sds_id = 5, .mode = RTK_MII_DISABLE,    .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [6] = { .sds_id = HWP_END },
    }, /* serdes.descp */

    .phy.descp = {
        [0] =   { .chip = RTK_PHYTYPE_RTL8218B, .mac_id = 8,   .phy_max = 6 },
        [1] =   { .chip = HWP_END },
    },   /* .phy.descp */
};


/*
 * hardware profile
 */
static hwp_hwProfile_t rtl8380h_intphy_demo = {

    .identifier.name        = "RTL8380H_INTPHY_DEMO",
    .identifier.id          = HWP_RTL8380H_INTPHY_DEMO,

    .soc.swDescp_index      = 0,
    .soc.slaveInterruptPin  = HWP_NONE,

    .sw_count               = 1,
    .swDescp = {
        [0]                 = &rtl8380h_intphy_demo_swDescp,
    }

};
