/*
 * Copyright(c) Realtek Semiconductor Corporation, 2016
 * All rights reserved.
 *
 * Purpose : Related implementation of the RTL8381M_INTPHY_8214FC_2FIB_1G demo board.
 *
 * Feature : RTL8381M_INTPHY_8214FC_2FIB_1G demo board database
 *
 */

static hwp_swDescp_t rtl8381m_intphy_8214fc_2fib_1g_demo_swDescp = {

    .chip_id                    = RTL8381M_CHIP_ID,
    .swcore_supported           = TRUE,
    .swcore_access_method       = HWP_SW_ACC_MEM,
    .swcore_spi_chip_select     = HWP_NOT_USED,
    .nic_supported              = TRUE,

    .port.descp = {
        { .mac_id = 8,  .attr = HWP_ETH, .eth = HWP_GE,   .medi = HWP_COPPER, .sds_idx = HWP_NONE, .phy_idx = 0,        .smi = 0,        .phy_addr = 8,        .led_c = 0,       .led_f = HWP_NONE, .led_layout = SINGLE_SET,},
        { .mac_id = 9,  .attr = HWP_ETH, .eth = HWP_GE,   .medi = HWP_COPPER, .sds_idx = HWP_NONE, .phy_idx = 0,        .smi = 0,        .phy_addr = 9,        .led_c = 0,       .led_f = HWP_NONE, .led_layout = SINGLE_SET,},
        { .mac_id = 10, .attr = HWP_ETH, .eth = HWP_GE,   .medi = HWP_COPPER, .sds_idx = HWP_NONE, .phy_idx = 0,        .smi = 0,        .phy_addr = 10,       .led_c = 0,       .led_f = HWP_NONE, .led_layout = SINGLE_SET,},
        { .mac_id = 11, .attr = HWP_ETH, .eth = HWP_GE,   .medi = HWP_COPPER, .sds_idx = HWP_NONE, .phy_idx = 0,        .smi = 0,        .phy_addr = 11,       .led_c = 0,       .led_f = HWP_NONE, .led_layout = SINGLE_SET,},
        { .mac_id = 12, .attr = HWP_ETH, .eth = HWP_GE,   .medi = HWP_COPPER, .sds_idx = HWP_NONE, .phy_idx = 0,        .smi = 0,        .phy_addr = 12,       .led_c = 0,       .led_f = HWP_NONE, .led_layout = SINGLE_SET,},
        { .mac_id = 13, .attr = HWP_ETH, .eth = HWP_GE,   .medi = HWP_COPPER, .sds_idx = HWP_NONE, .phy_idx = 0,        .smi = 0,        .phy_addr = 13,       .led_c = 0,       .led_f = HWP_NONE, .led_layout = SINGLE_SET,},
        { .mac_id = 14, .attr = HWP_ETH, .eth = HWP_GE,   .medi = HWP_COPPER, .sds_idx = HWP_NONE, .phy_idx = 0,        .smi = 0,        .phy_addr = 14,       .led_c = 0,       .led_f = HWP_NONE, .led_layout = SINGLE_SET,},
        { .mac_id = 15, .attr = HWP_ETH, .eth = HWP_GE,   .medi = HWP_COPPER, .sds_idx = HWP_NONE, .phy_idx = 0,        .smi = 0,        .phy_addr = 15,       .led_c = 0,       .led_f = HWP_NONE, .led_layout = SINGLE_SET,},
        { .mac_id = 16, .attr = HWP_ETH, .eth = HWP_GE,   .medi = HWP_COMBO,  .sds_idx = 2,        .phy_idx = 1,        .smi = 0,        .phy_addr = 16,       .led_c = 0,       .led_f = HWP_NONE, .led_layout = SINGLE_SET,},
        { .mac_id = 17, .attr = HWP_ETH, .eth = HWP_GE,   .medi = HWP_COMBO,  .sds_idx = 2,        .phy_idx = 1,        .smi = 0,        .phy_addr = 17,       .led_c = 0,       .led_f = HWP_NONE, .led_layout = SINGLE_SET,},
        { .mac_id = 18, .attr = HWP_ETH, .eth = HWP_GE,   .medi = HWP_COMBO,  .sds_idx = 2,        .phy_idx = 1,        .smi = 0,        .phy_addr = 18,       .led_c = 0,       .led_f = HWP_NONE, .led_layout = SINGLE_SET,},
        { .mac_id = 19, .attr = HWP_ETH, .eth = HWP_GE,   .medi = HWP_COMBO,  .sds_idx = 2,        .phy_idx = 1,        .smi = 0,        .phy_addr = 19,       .led_c = 0,       .led_f = HWP_NONE, .led_layout = SINGLE_SET,},
        { .mac_id = 24, .attr = HWP_ETH, .eth = HWP_GE,   .medi = HWP_SERDES, .sds_idx = 4,        .phy_idx = HWP_NONE, .smi = HWP_NONE, .phy_addr = HWP_NONE, .led_c = 0,       .led_f = HWP_NONE, .led_layout = SINGLE_SET,},
        { .mac_id = 26, .attr = HWP_ETH, .eth = HWP_GE,   .medi = HWP_SERDES, .sds_idx = 5,        .phy_idx = HWP_NONE, .smi = HWP_NONE, .phy_addr = HWP_NONE, .led_c = 0,       .led_f = HWP_NONE, .led_layout = SINGLE_SET,},
        { .mac_id = 28, .attr = HWP_CPU, .eth = HWP_NONE, .medi = HWP_NONE,   .sds_idx = HWP_NONE, .phy_idx = HWP_NONE, .smi = HWP_NONE, .phy_addr = HWP_NONE, .led_c = HWP_NONE,.led_f = HWP_NONE, .led_layout = HWP_NONE,},
        { .mac_id = HWP_END },
    },  /* port.descp */

    /* LED definition - set 0 */
    .led.descp = {
        .led_active = LED_ACTIVE_LOW,
        .led_if_sel = LED_IF_SEL_SERIAL,
        .led_definition_set[0].led[0] = 0x7,          /* Link/Act */
        .led_definition_set[0].led[1] = 0x0,          /* 10M Link/Act */
        .led_definition_set[0].led[2] = HWP_LED_END,      /* none */
    }, /* led.descp */
    .serdes.descp = {
        [0] = { .sds_id = 0, .mode = RTK_MII_DISABLE,          .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL},
        [1] = { .sds_id = 1, .mode = RTK_MII_DISABLE,          .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL},
        [2] = { .sds_id = 2, .mode = RTK_MII_QSGMII,        .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL},
        [3] = { .sds_id = 3, .mode = RTK_MII_DISABLE,          .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL},
        [4] = { .sds_id = 4, .mode = RTK_MII_1000BX_FIBER,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL},
        [5] = { .sds_id = 5, .mode = RTK_MII_1000BX_FIBER,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL},
        [6] = { .sds_id = HWP_END },
    }, /* serdes.descp */

    .phy.descp = {
        [0] =   { .chip = RTK_PHYTYPE_RTL8218B,  .mac_id = 8,   .phy_max = 8 },
        [1] =   { .chip = RTK_PHYTYPE_RTL8214FC, .mac_id = 16,  .phy_max = 4 },
        [2] =   { .chip = HWP_END},
    }   /* .phy.descp */
};

/*
 * hardware profile
 */
static hwp_hwProfile_t rtl8381m_intphy_8214fc_2fib_1g_demo = {

    .identifier.name        = "RTL8381M_INTPHY_8214FC_2FIB_1G_DEMO",
    .identifier.id          = HWP_RTL8381M_INTPHY_8214FC_2FIB_1G_DEMO,

    .soc.swDescp_index      = 0,
    .soc.slaveInterruptPin  = HWP_NONE,

    .sw_count               = 1,
    .swDescp = {
        [0]                 = &rtl8381m_intphy_8214fc_2fib_1g_demo_swDescp,
    }

};
