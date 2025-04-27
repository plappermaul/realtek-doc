/*
 * Copyright(c) Realtek Semiconductor Corporation, 2015
 * All rights reserved.
 *
 * Purpose : Related implementation of the RTL9302B board
 *
 * Feature : RTL9302C_2xRTL8224_2XGE demo board
 *
 */

static hwp_swDescp_t rtl9302c_2x8224_2xge_swDescp = {

    .chip_id                    = RTL9302C_CHIP_ID,
    .swcore_supported           = TRUE,
    .swcore_access_method       = HWP_SW_ACC_MEM,
    .swcore_spi_chip_select     = HWP_NOT_USED,
    .nic_supported              = TRUE,

    .port.descp = {
        { .mac_id =  0, .attr = HWP_ETH,   .eth = HWP_2_5GE,    .medi = HWP_COPPER, .sds_idx = 0, .phy_idx = 0,        .smi = 0,        .phy_addr =  0,        .led_c = 0,        .led_f = HWP_NONE,.led_layout = SINGLE_SET, .phy_mdi_pin_swap = 0,},
        { .mac_id =  1, .attr = HWP_ETH,   .eth = HWP_2_5GE,    .medi = HWP_COPPER, .sds_idx = 0, .phy_idx = 0,        .smi = 0,        .phy_addr =  1,        .led_c = 0,        .led_f = HWP_NONE,.led_layout = SINGLE_SET, .phy_mdi_pin_swap = 0,},
        { .mac_id =  2, .attr = HWP_ETH,   .eth = HWP_2_5GE,    .medi = HWP_COPPER, .sds_idx = 0, .phy_idx = 0,        .smi = 0,        .phy_addr =  2,        .led_c = 0,        .led_f = HWP_NONE,.led_layout = SINGLE_SET, .phy_mdi_pin_swap = 1,},
        { .mac_id =  3, .attr = HWP_ETH,   .eth = HWP_2_5GE,    .medi = HWP_COPPER, .sds_idx = 0, .phy_idx = 0,        .smi = 0,        .phy_addr =  3,        .led_c = 0,        .led_f = HWP_NONE,.led_layout = SINGLE_SET, .phy_mdi_pin_swap = 1,},
        { .mac_id = 16, .attr = HWP_ETH,   .eth = HWP_2_5GE, .medi = HWP_COPPER, .sds_idx = 1, .phy_idx = 1,        .smi = 1,        .phy_addr =  0,        .led_c = 0,        .led_f = HWP_NONE,.led_layout = SINGLE_SET, .phy_mdi_pin_swap = 0,},
        { .mac_id = 17, .attr = HWP_ETH,   .eth = HWP_2_5GE, .medi = HWP_COPPER, .sds_idx = 1, .phy_idx = 1,        .smi = 1,        .phy_addr =  1,        .led_c = 0,        .led_f = HWP_NONE,.led_layout = SINGLE_SET, .phy_mdi_pin_swap = 0,},
        { .mac_id = 18, .attr = HWP_ETH,   .eth = HWP_2_5GE, .medi = HWP_COPPER, .sds_idx = 1, .phy_idx = 1,        .smi = 1,        .phy_addr =  2,        .led_c = 0,        .led_f = HWP_NONE,.led_layout = SINGLE_SET, .phy_mdi_pin_swap = 1,},
        { .mac_id = 19, .attr = HWP_ETH,   .eth = HWP_2_5GE, .medi = HWP_COPPER, .sds_idx = 1, .phy_idx = 1,        .smi = 1,        .phy_addr =  3,        .led_c = 0,        .led_f = HWP_NONE,.led_layout = SINGLE_SET, .phy_mdi_pin_swap = 1,},
        { .mac_id = 24, .attr = HWP_ETH,   .eth = HWP_XGE,   .medi = HWP_SERDES, .sds_idx = 2, .phy_idx = HWP_NONE, .smi = HWP_NONE, .phy_addr = HWP_NONE,  .led_c = 3,        .led_f = 3,       .led_layout = SINGLE_SET,},
        { .mac_id = 25, .attr = HWP_ETH,   .eth = HWP_XGE,   .medi = HWP_SERDES, .sds_idx = 3, .phy_idx = HWP_NONE, .smi = HWP_NONE, .phy_addr = HWP_NONE,  .led_c = 3,        .led_f = 3,       .led_layout = SINGLE_SET,},
        { .mac_id = 28, .attr = HWP_CPU,   .eth = HWP_NONE,  .medi = HWP_NONE,   .sds_idx = HWP_NONE, .phy_idx = HWP_NONE, .smi = HWP_NONE, .phy_addr =  HWP_NONE, .led_c = HWP_NONE, .led_f = HWP_NONE,.led_layout = HWP_NONE,  },
        { .mac_id = HWP_END },
    },  /* port.descp */

    .led.descp = {
        .led_active = LED_ACTIVE_LOW,
        .led_if_sel = LED_IF_SEL_SERIAL,
        .led_definition_set[0].led[0] = 0xA08,        /* 2.5 Gbps link/act */
        .led_definition_set[0].led[1] = 0xB80,        /* 10M/100Mbps link/act */
        .led_definition_set[0].led[2] = 0xA20,        /* 1000Mbps link/act */
        .led_definition_set[0].led[3] = HWP_LED_END,      /* None */  
        .led_definition_set[3].led[0] = 0x0A20,       /*(1010 0010 0000) 1G, Link,ACT*/
        .led_definition_set[3].led[1] = 0x0A01,       /*(1010 0000 0001) 10G, Link,ACT*/
        .led_definition_set[3].led[2] = 0x0000,       /*Not connect*/
        .led_definition_set[3].led[3] = HWP_LED_END,  /* None */

    },/* led.descp */

    .serdes.descp = {
        [0] = { .sds_id = 2, .mode = RTK_MII_USXGMII_10GQXGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [1] = { .sds_id = 4, .mode = RTK_MII_USXGMII_10GQXGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [2] = { .sds_id = 6, .mode = RTK_MII_10GR,     .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [3] = { .sds_id = 7, .mode = RTK_MII_10GR,     .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [4] = { .sds_id = HWP_END },
    }, /* serdes.descp */

    .phy.descp = {
        [0] = { .chip = RTK_PHYTYPE_RTL8224,       .mac_id = 0,  .phy_max = 4 },
        [1] = { .chip = RTK_PHYTYPE_RTL8224,       .mac_id = 16,  .phy_max = 4 },
        [2] = { .chip = HWP_END },
    },   /* .phy.descp */

};


/*
 * hardware profile
 */
static hwp_hwProfile_t rtl9302c_2x8224_2xge = {

    .identifier.name        = "RTL9302C_2xRTL8224_2XGE",
    .identifier.id          = HWP_RTL9302C_2xRTL8224_2XGE,

    .soc.swDescp_index      = 0,
    .soc.slaveInterruptPin  = HWP_NONE,

    .sw_count               = 1,
    .swDescp = {
        [0]                 = &rtl9302c_2x8224_2xge_swDescp,
    }

};

