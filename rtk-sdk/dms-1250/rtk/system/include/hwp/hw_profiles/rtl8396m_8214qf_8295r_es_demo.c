/*
 * Copyright(c) Realtek Semiconductor Corporation, 2016
 * All rights reserved.
 *
 * Purpose : Related implementation of the RTL836M DEMO board.
 *
 * Feature : RTL8396M DEMO board database
 *
 */

#define USE_8295R           1

static hwp_swDescp_t rtl8396m_8214qf_8295r_es_demo_swDescp = {

    .chip_id                    = RTL8396M_CHIP_ID,
    .swcore_supported           = TRUE,
    .swcore_access_method       = HWP_SW_ACC_MEM,
    .swcore_spi_chip_select     = HWP_NOT_USED,
    .nic_supported              = TRUE,

    .port.descp = {
        { .mac_id =  0, .attr = HWP_ETH,   .eth = HWP_GE,   .medi = HWP_COPPER,             .sds_idx =  0,       .phy_idx = 0,        .smi = 0,        .phy_addr =  0,       .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id =  1, .attr = HWP_ETH,   .eth = HWP_GE,   .medi = HWP_COPPER,             .sds_idx =  0,       .phy_idx = 0,        .smi = 0,        .phy_addr =  1,       .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id =  2, .attr = HWP_ETH,   .eth = HWP_GE,   .medi = HWP_COPPER,             .sds_idx =  0,       .phy_idx = 0,        .smi = 0,        .phy_addr =  2,       .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id =  3, .attr = HWP_ETH,   .eth = HWP_GE,   .medi = HWP_COPPER,             .sds_idx =  0,       .phy_idx = 0,        .smi = 0,        .phy_addr =  3,       .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id =  4, .attr = HWP_ETH,   .eth = HWP_GE,   .medi = HWP_COPPER,             .sds_idx =  1,       .phy_idx = 0,        .smi = 0,        .phy_addr =  4,       .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id =  5, .attr = HWP_ETH,   .eth = HWP_GE,   .medi = HWP_COPPER,             .sds_idx =  1,       .phy_idx = 0,        .smi = 0,        .phy_addr =  5,       .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id =  6, .attr = HWP_ETH,   .eth = HWP_GE,   .medi = HWP_COPPER,             .sds_idx =  1,       .phy_idx = 0,        .smi = 0,        .phy_addr =  6,       .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id =  7, .attr = HWP_ETH,   .eth = HWP_GE,   .medi = HWP_COPPER,             .sds_idx =  1,       .phy_idx = 0,        .smi = 0,        .phy_addr =  7,       .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id =  8, .attr = HWP_ETH,   .eth = HWP_GE,   .medi = HWP_COPPER,             .sds_idx =  2,       .phy_idx = 1,        .smi = 0,        .phy_addr =  8,       .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id =  9, .attr = HWP_ETH,   .eth = HWP_GE,   .medi = HWP_COPPER,             .sds_idx =  2,       .phy_idx = 1,        .smi = 0,        .phy_addr =  9,       .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id = 10, .attr = HWP_ETH,   .eth = HWP_GE,   .medi = HWP_COPPER,             .sds_idx =  2,       .phy_idx = 1,        .smi = 0,        .phy_addr = 10,       .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id = 11, .attr = HWP_ETH,   .eth = HWP_GE,   .medi = HWP_COPPER,             .sds_idx =  2,       .phy_idx = 1,        .smi = 0,        .phy_addr = 11,       .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id = 12, .attr = HWP_ETH,   .eth = HWP_GE,   .medi = HWP_COPPER,             .sds_idx =  3,       .phy_idx = 1,        .smi = 0,        .phy_addr = 12,       .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id = 13, .attr = HWP_ETH,   .eth = HWP_GE,   .medi = HWP_COPPER,             .sds_idx =  3,       .phy_idx = 1,        .smi = 0,        .phy_addr = 13,       .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id = 14, .attr = HWP_ETH,   .eth = HWP_GE,   .medi = HWP_COPPER,             .sds_idx =  3,       .phy_idx = 1,        .smi = 0,        .phy_addr = 14,       .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id = 15, .attr = HWP_ETH,   .eth = HWP_GE,   .medi = HWP_COPPER,             .sds_idx =  3,       .phy_idx = 1,        .smi = 0,        .phy_addr = 15,       .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id = 16, .attr = HWP_ETH,   .eth = HWP_GE,   .medi = HWP_FIBER,              .sds_idx =  4,       .phy_idx = 2,        .smi = 0,        .phy_addr = 16,       .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id = 17, .attr = HWP_ETH,   .eth = HWP_GE,   .medi = HWP_FIBER,              .sds_idx =  4,       .phy_idx = 2,        .smi = 0,        .phy_addr = 17,       .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id = 18, .attr = HWP_ETH,   .eth = HWP_GE,   .medi = HWP_FIBER,              .sds_idx =  4,       .phy_idx = 2,        .smi = 0,        .phy_addr = 18,       .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
        { .mac_id = 19, .attr = HWP_ETH,   .eth = HWP_GE,   .medi = HWP_FIBER,              .sds_idx =  4,       .phy_idx = 2,        .smi = 0,        .phy_addr = 19,       .led_c = 0,        .led_f = 0,       .led_layout = SINGLE_SET,},
#if defined(CONFIG_SDK_RTL8295R) && (USE_8295R == 1)
        { .mac_id = 24, .attr = HWP_ETH,   .eth = HWP_XGE,  .medi = (HWP_FIBER|HWP_SERDES), .sds_idx =  6,       .phy_idx = 3,        .smi = 1,        .phy_addr = 0,        .led_c = 1,        .led_f = 1,       .led_layout = SINGLE_SET,},
        { .mac_id = 36, .attr = HWP_ETH,   .eth = HWP_XGE,  .medi = (HWP_FIBER|HWP_SERDES), .sds_idx =  7,       .phy_idx = 4,        .smi = 1,        .phy_addr = 1,        .led_c = 1,        .led_f = 1,       .led_layout = SINGLE_SET,},
#else
        { .mac_id = 24, .attr = HWP_ETH,   .eth = HWP_XGE,  .medi = HWP_SERDES,             .sds_idx =  6,       .phy_idx = HWP_NONE, .smi = HWP_NONE, .phy_addr = HWP_NONE, .led_c = 0,        .led_f = 1,       .led_layout = SINGLE_SET,},
        { .mac_id = 36, .attr = HWP_ETH,   .eth = HWP_XGE,  .medi = HWP_SERDES,             .sds_idx =  7,       .phy_idx = HWP_NONE, .smi = HWP_NONE, .phy_addr = HWP_NONE, .led_c = 0,        .led_f = 1,       .led_layout = SINGLE_SET,},
#endif
        { .mac_id = 52, .attr = HWP_CPU,   .eth = HWP_NONE, .medi = HWP_NONE,               .sds_idx = HWP_NONE, .phy_idx = HWP_NONE, .smi = HWP_NONE, .phy_addr = HWP_NONE, .led_c = HWP_NONE, .led_f = HWP_NONE,.led_layout = HWP_NONE,},
        { .mac_id = HWP_END },
    },  /* port.descp */

    .led.descp = {
        .led_active = LED_ACTIVE_HIGH,
        .led_if_sel = LED_IF_SEL_BI_COLOR_SCAN,
        .led_definition_set[0].led[0] = 0xA,        /* 1000Mbps link/act */
        .led_definition_set[0].led[1] = 0xF,        /* 10/100Mbps link/act */
        .led_definition_set[0].led[2] = 0x6,        /* duplex mode */
        .led_definition_set[0].led[3] = HWP_LED_END,

        .led_definition_set[1].led[0] = 0x15,       /* 10G link/act */
        .led_definition_set[1].led[1] = 0xD,        /* 1000/100Mbps link/act */
        .led_definition_set[1].led[2] = 0x6,        /* duplex mode */
        .led_definition_set[1].led[3] = HWP_LED_END,
    }, /* led.descp */

    .serdes.descp = {
        [0]  = { .sds_id = 0,  .mode = RTK_MII_QSGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [1]  = { .sds_id = 1,  .mode = RTK_MII_QSGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [2]  = { .sds_id = 2,  .mode = RTK_MII_QSGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [3]  = { .sds_id = 3,  .mode = RTK_MII_QSGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [4]  = { .sds_id = 4,  .mode = RTK_MII_QSGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [5]  = { .sds_id = 5,  .mode = RTK_MII_DISABLE, .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [6]  = { .sds_id = 8,  .mode = RTK_MII_10GR,    .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [7]  = { .sds_id = 12, .mode = RTK_MII_10GR,    .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [8]  = { .sds_id = HWP_END },
    },  /* serdes.descp */

    .phy.descp = {
        [0] = { .chip = RTK_PHYTYPE_RTL8218B,  .mac_id = 0 , .phy_max = 8 },
        [1] = { .chip = RTK_PHYTYPE_RTL8218B,  .mac_id = 8 , .phy_max = 8 },
        [2] = { .chip = RTK_PHYTYPE_RTL8214QF, .mac_id = 16, .phy_max = 4 },
#if defined(CONFIG_SDK_RTL8295R) && (USE_8295R == 1)
        [3] = { .chip = RTK_PHYTYPE_RTL8295R,  .mac_id = 24, .phy_max = 1 },
        [4] = { .chip = RTK_PHYTYPE_RTL8295R,  .mac_id = 36, .phy_max = 1 },
        [5] = { .chip = HWP_END },
#else
        [3] = { .chip = HWP_END },
#endif
    },   /* .phy.descp */
};


/*
 * hardware profile
 */
static hwp_hwProfile_t rtl8396m_8214qf_8295r_es_demo = {

    .identifier.name    = "RTL8396M_8292_DEMO",
    .identifier.id      = HWP_RTL8396M_8214QF_8295R_ES_DEMO,

    .soc.swDescp_index  = 0,

    .sw_count           = 1,
    .swDescp = {
        [0]             = &rtl8396m_8214qf_8295r_es_demo_swDescp,
    }
};


